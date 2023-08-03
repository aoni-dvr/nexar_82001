/**
 *  @file SCA_OpenOD.c
 *
 *  @copyright Copyright (c) 2019 Ambarella International LP
 *
 *  This file and its contents ("Software") are protected by intellectual
 *  property rights including, without limitation, U.S. and/or foreign
 *  copyrights. This Software is also the confidential and proprietary
 *  information of Ambarella International LP and its licensors. You may not use, reproduce,
 *  disclose, distribute, modify, or otherwise prepare derivative works of this
 *  Software or any portion thereof except pursuant to a signed license agreement
 *  or nondisclosure agreement with Ambarella International LP or its authorized affiliates.
 *  In the absence of such an agreement, you agree to promptly notify and return
 *  this Software to Ambarella International LP.
 *
 *  This file includes sample code and is only for internal testing and evaluation.  If you
 *  distribute this sample code (whether in source, object, or binary code form), it will be
 *  without any warranty or indemnity protection from Ambarella International LP or its affiliates.
 *
 *  THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
 *  INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF NON-INFRINGEMENT,
 *  MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 *  IN NO EVENT SHALL AMBARELLA INTERNATIONAL LP OR ITS AFFILIATES BE LIABLE FOR ANY DIRECT,
 *  INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 *  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 *  LOSS OF USE, DATA, OR PROFITS; COMPUTER FAILURE OR MALFUNCTION; OR BUSINESS
 *  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 *  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 *  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *  POSSIBILITY OF SUCH DAMAGE.
 *
 *  @details Implementation of SCA OpenOD
 *
 */
#include "AmbaTypes.h"
#include "cvapi_memio_interface.h"
#include "cvapi_ambacv_flexidag.h"
#include "cvapi_ambaod.h"
#include "idsp_roi_msg.h"
#include "AmbaPrint.h"
#include "AmbaMisraFix.h"
#include "AmbaWrap.h"
#include "AmbaUtility.h"
#include "SvcCvAlgo.h"
#include "cvapi_cvflow_buffer.h"
#include "cvapi_svccvalgo_openod.h"
#include "cvapi_svccvalgo_ambaod.h"

#define OBJ_THR             0.1f
#define CONF_THR            0.4f
#define OVERLAP_THR         0.45f
#define MAX_BBX_NUM         150U

#define NET_INPUT_W 1024U
#define NET_INPUT_H 640U

#define EXP                 2.71828182845904
#define NUM_CLS             4U
// #define DEBUG

typedef struct {
    UINT32 raw_w;
    UINT32 raw_h;

    UINT32 roi_w;
    UINT32 roi_h;

    UINT32 roi_start_col;
    UINT32 roi_start_row;

    UINT32 net_in_w;
    UINT32 net_in_h;
} openod_win_ctx_t;

typedef struct {
    FLOAT x1;
    FLOAT y1;
    FLOAT x2;
    FLOAT y2;

    UINT32    cls;
    FLOAT     conf;
    UINT32    prev_idx;
    UINT32    next_idx;
} nms_dex_bbx_t;

static openod_win_ctx_t my_ctx;
static amba_roi_config_t roi_cfg;
static flexidag_memblk_t nms_box_memblk;
static flexidag_memblk_t cls_name_blk;
static FLOAT *g_prior_box = NULL;
static UINT32 total_bbox = 0U;
static UINT32 pb_len = 0U;
static UINT32 num_dec_bbx = 0U;
static UINT32 max_idx = 0U;
static UINT32 vpOutNum = 0U;

#ifndef MIN
static inline FLOAT MIN(FLOAT a, FLOAT b)
{
    FLOAT tmp;

    if (a < b) {
        tmp = a;
    } else {
        tmp = b;
    }

    return tmp;
}
#endif

#ifndef MAX
static inline FLOAT MAX(FLOAT a, FLOAT b)
{
    FLOAT tmp;

    if (a > b) {
        tmp = a;
    } else {
        tmp = b;
    }

    return tmp;
}
#endif

static inline FLOAT find_max(const FLOAT *cls, UINT32 len, UINT32 *pos)
{
    UINT32 idx;
    FLOAT max = 0.0f;

    for (idx = 0; idx < len; idx++) {
        if (cls[idx] > max) {
            max = cls[idx];
            *pos = idx;
        }
    }

    return max;
}

static inline FLOAT find_max_i16(const INT16 *cls, UINT32 len, UINT32 *pos, FLOAT int_shift)
{
    UINT32 idx;
    INT16 max = 0;

    for (idx = 0; idx < len; idx++) {
        if (cls[idx] > max) {
            max = cls[idx];
            *pos = idx;
        }
    }

    return ((FLOAT) max * int_shift);
}

static inline UINT16 cal_pos(FLOAT minmax, UINT32 net_in,
                             UINT32 roi_start, UINT32 raw, UINT32 roi)
{
    FLOAT fltmp;
    UINT16 u16tmp;

    fltmp = (FLOAT) net_in;

    fltmp *= minmax;

    fltmp += (FLOAT) roi_start;

    fltmp *= (FLOAT) raw;

    fltmp /= (FLOAT) roi;

    u16tmp = (UINT16) fltmp;

    return u16tmp;
}

static inline UINT16 cal_wh(FLOAT max, FLOAT min, UINT32 net_in,
                            UINT32 roi, UINT32 raw)
{
    FLOAT fltmp;
    UINT16 u16tmp;

    fltmp = max - min;

    fltmp *= (FLOAT) net_in;

    fltmp /= (FLOAT) roi;

    fltmp *= (FLOAT) raw;

    u16tmp = (UINT16) fltmp;

    return u16tmp;
}

static void send_bbx(UINT32 target_class, FLOAT x_min, FLOAT y_min,
                     FLOAT x_max, FLOAT y_max, FLOAT score, amba_od_out_t *od_results, amba_od_candidate_t *od_candidates)
{
    UINT32 raw_w          = my_ctx.raw_w;
    UINT32 raw_h          = my_ctx.raw_h;
    UINT32 roi_w          = my_ctx.roi_w;
    UINT32 roi_h          = my_ctx.roi_h;
    UINT32 roi_start_col  = my_ctx.roi_start_col;
    UINT32 roi_start_row  = my_ctx.roi_start_row;
    UINT32 net_in_w       = my_ctx.net_in_w;
    UINT32 net_in_h       = my_ctx.net_in_h;
    FLOAT fltmp;
    UINT32 u32tmp;

    if (od_results->num_objects >= MAX_BBX_NUM) {
        AmbaPrint_PrintUInt5("BBX overflows (max = %d)!", MAX_BBX_NUM, 0, 0, 0, 0);
    } else if (raw_w == 0U) {
        AmbaPrint_PrintUInt5("Raw width is equal to 0!", 0, 0, 0, 0, 0);
    } else {
        od_candidates[od_results->num_objects].clsId = target_class;

        od_candidates[od_results->num_objects].bb_start_col =
            (int32_t) cal_pos(MIN(x_min, x_max), net_in_w, roi_start_col,
                              raw_w, roi_w);
        od_candidates[od_results->num_objects].bb_start_row =
            (int32_t) cal_pos(MIN(y_min, y_max), net_in_h, roi_start_row,
                              raw_h, roi_h);

        od_candidates[od_results->num_objects].bb_width_m1 =
            (UINT32) cal_wh(MAX(x_min, x_max), MIN(x_min, x_max), net_in_w,
                            roi_w, raw_w) - 1U;
        od_candidates[od_results->num_objects].bb_height_m1 =
            (UINT32) cal_wh(MAX(y_min, y_max), MIN(y_min, y_max), net_in_h,
                            roi_h, raw_h) - 1U;

        fltmp = score * 255.0f;
        u32tmp = (UINT32) fltmp;
        od_candidates[od_results->num_objects].score = u32tmp;

        od_results->num_objects += 1U;
    }
}

static void decode_header(const cvflow_port_header_t *header, ULONG *buf_addr, FLOAT *vpOutShift)
{
    const cvflow_buffer_desc_t *desc;
    ULONG ultmp, ultmp2;
    DOUBLE dtmp, dtmp2;
    UINT32 idx, Ret;

    AmbaMisra_TypeCast(&ultmp, &header);
    for (idx = 0U; idx < header->num_packed; idx++) {
        buf_addr[idx] = ultmp + header->buffer_offset[idx];
        ultmp2 = buf_addr[idx] - sizeof(cvflow_buffer_desc_t);
        AmbaMisra_TypeCast(&desc, &ultmp2);
#ifdef DEBUG
        AmbaPrint_PrintUInt5("VP output %d with data_format(%d, %d, %d, %d)", idx, desc->df[0], desc->df[1], desc->df[2], desc->df[3]);
#endif
        if ((desc->df[0] == 1U) && (desc->df[1] == 2U) && (desc->df[2] == 0U) && (desc->df[3] == 7U)) {
#ifdef DEBUG
            AmbaPrint_PrintUInt5("VP output %d is FP32", idx, 0U, 0U, 0U, 0U);
#endif
            vpOutShift[idx] = 1.0f;
        } else {
            /* Calculate the shift value of mbox_loc for convert int8 to fp32 */
            dtmp = (DOUBLE) desc->df[2];
            Ret = AmbaWrap_pow(2.0, -dtmp, &dtmp2);
            if (Ret != 0U) {
                AmbaPrint_PrintUInt5("AmbaWrap_pow failed", 0U, 0U, 0U, 0U, 0U);
            }
            vpOutShift[idx] = (FLOAT) dtmp2;
        }
    }
}

static void bbox_regression(const char *p_arm_conf, const char *p_arm_loc, const char *p_odm_conf, const char *p_odm_loc, const FLOAT *vpOutShift)
{
    // Support for two data format: FP32, two-byte signed fixed-point
    // FP32 with vpOutShift = 1.0f; fixed-point with vpOutShift < 1.0f
    const FLOAT *arm_conf_fp32, *arm_loc_fp32, *odm_conf_fp32, *odm_loc_fp32;
    const INT16 *arm_conf_i16, *arm_loc_i16, *odm_conf_i16, *odm_loc_i16;
    const FLOAT *var, *pbox = g_prior_box;
    ULONG g_prior_box_ulong, ulongtmp;
    FLOAT prior_x_center, prior_y_center, prior_w, prior_h;
    FLOAT dec_x_center, dec_y_center, dec_w, dec_h;
    FLOAT fltmp, max, arm_conf_tmp, decbox[4];
    DOUBLE exponent, temp;
    UINT32 i, ix4, count = 0, pos = 0, idx, Ret;
    nms_dex_bbx_t *nms_dec_box;

    AmbaMisra_TypeCast(&nms_dec_box, &(nms_box_memblk.pBuffer));

    Ret = AmbaWrap_memset(nms_dec_box, 0x0, total_bbox * sizeof(nms_dex_bbx_t));
    if (Ret != 0U) {
        AmbaPrint_PrintUInt5("AmbaWrap_memset failed", 0U, 0U, 0U, 0U, 0U);
    }
    num_dec_bbx = 0;

    AmbaMisra_TypeCast(&g_prior_box_ulong, &pbox);
    ulongtmp = g_prior_box_ulong + ((ULONG) pb_len >> 1);
    AmbaMisra_TypeCast(&var, &ulongtmp);

    AmbaMisra_TypeCast(&odm_loc_fp32, &p_odm_loc);
    AmbaMisra_TypeCast(&odm_loc_i16, &p_odm_loc);
    AmbaMisra_TypeCast(&odm_conf_fp32, &p_odm_conf);
    AmbaMisra_TypeCast(&odm_conf_i16, &p_odm_conf);
    AmbaMisra_TypeCast(&arm_loc_fp32, &p_arm_loc);
    AmbaMisra_TypeCast(&arm_loc_i16, &p_arm_loc);
    AmbaMisra_TypeCast(&arm_conf_fp32, &p_arm_conf);
    AmbaMisra_TypeCast(&arm_conf_i16, &p_arm_conf);

    for (i = 0; i < total_bbox; i++) {
        if (vpOutShift[3] == 1.0f) {
            arm_conf_tmp = arm_conf_fp32[(i * 2U) + 1U];
        } else {
            arm_conf_tmp = (FLOAT) arm_conf_i16[(i * 2U) + 1U] * vpOutShift[3];
        }
        if (arm_conf_tmp > OBJ_THR) {
            if (vpOutShift[1] == 1.0f) {
                max = find_max(&odm_conf_fp32[i * NUM_CLS], NUM_CLS, &pos);
            } else {
                max = find_max_i16(&odm_conf_i16[i * NUM_CLS], NUM_CLS, &pos, vpOutShift[1]);
            }

            if ((pos != 0U) && (max > CONF_THR)) {
                ix4 = i * 4U;
                // prior box
                prior_x_center = (pbox[ix4] + pbox[ix4 + 2U]) / 2.0f;
                prior_y_center = (pbox[ix4 + 1U] + pbox[ix4 + 3U]) / 2.0f;
                prior_w = pbox[ix4 + 2U] - pbox[ix4];
                prior_h = pbox[ix4 + 3U] - pbox[ix4 + 1U];

                // decode bbox
                if (vpOutShift[2] == 1.0f) {
                    dec_x_center = (var[ix4] * arm_loc_fp32[ix4] * prior_w) +
                                   prior_x_center;
                    dec_y_center = (var[ix4 + 1U] * arm_loc_fp32[ix4 + 1U] * prior_h) +
                                   prior_y_center;

                    fltmp = var[ix4 + 2U] * arm_loc_fp32[ix4 + 2U];
                    exponent = (DOUBLE) fltmp;
                    Ret = AmbaWrap_pow(EXP, exponent, &temp);
                    if (Ret != 0U) {
                        AmbaPrint_PrintUInt5("AmbaWrap_pow failed", 0U, 0U, 0U, 0U, 0U);
                    }

                    dec_w = (FLOAT) temp;
                    dec_w *= prior_w;

                    fltmp = var[ix4 + 3U] * arm_loc_fp32[ix4 + 3U];
                    exponent = (DOUBLE) fltmp;
                    Ret = AmbaWrap_pow(EXP, exponent, &temp);
                    if (Ret != 0U) {
                        AmbaPrint_PrintUInt5("AmbaWrap_pow failed", 0U, 0U, 0U, 0U, 0U);
                    }
                    dec_h = (FLOAT) temp;
                    dec_h *= prior_h;
                } else {
                    dec_x_center = (var[ix4] * (FLOAT) arm_loc_i16[ix4] * vpOutShift[2] * prior_w) +
                                   prior_x_center;
                    dec_y_center = (var[ix4 + 1U] * (FLOAT) arm_loc_i16[ix4 + 1U] * vpOutShift[2] * prior_h) +
                                   prior_y_center;

                    fltmp = var[ix4 + 2U] * (FLOAT) arm_loc_i16[ix4 + 2U] * vpOutShift[2];
                    exponent = (DOUBLE) fltmp;
                    Ret = AmbaWrap_pow(EXP, exponent, &temp);
                    if (Ret != 0U) {
                        AmbaPrint_PrintUInt5("AmbaWrap_pow failed", 0U, 0U, 0U, 0U, 0U);
                    }

                    dec_w = (FLOAT) temp;
                    dec_w *= prior_w;

                    fltmp = var[ix4 + 3U] * (FLOAT) arm_loc_i16[ix4 + 3U] * vpOutShift[2];
                    exponent = (DOUBLE) fltmp;
                    Ret = AmbaWrap_pow(EXP, exponent, &temp);
                    if (Ret != 0U) {
                        AmbaPrint_PrintUInt5("AmbaWrap_pow failed", 0U, 0U, 0U, 0U, 0U);
                    }
                    dec_h = (FLOAT) temp;
                    dec_h *= prior_h;
                }
                // finetune bbox
                if (vpOutShift[0] == 1.0f) {
                    dec_x_center += (var[ix4] * odm_loc_fp32[ix4] * dec_w);
                    dec_y_center += (var[ix4 + 1U] * odm_loc_fp32[ix4 + 1U] * dec_h);

                    fltmp = var[ix4 + 2U] * odm_loc_fp32[ix4 + 2U];
                    exponent = (DOUBLE) fltmp;
                    Ret = AmbaWrap_pow(EXP, exponent, &temp);
                    if (Ret != 0U) {
                        AmbaPrint_PrintUInt5("AmbaWrap_pow failed", 0U, 0U, 0U, 0U, 0U);
                    }
                    dec_w *= (FLOAT) temp;

                    fltmp = var[ix4 + 3U] * odm_loc_fp32[ix4 + 3U];
                    exponent = (DOUBLE) fltmp;
                    Ret = AmbaWrap_pow(EXP, exponent, &temp);
                    if (Ret != 0U) {
                        AmbaPrint_PrintUInt5("AmbaWrap_pow failed", 0U, 0U, 0U, 0U, 0U);
                    }
                    dec_h *= (FLOAT) temp;
                } else {
                    dec_x_center += (var[ix4] * (FLOAT) odm_loc_i16[ix4] * vpOutShift[0] * dec_w);
                    dec_y_center += (var[ix4 + 1U] * (FLOAT) odm_loc_i16[ix4 + 1U] * vpOutShift[0] * dec_h);

                    fltmp = var[ix4 + 2U] * (FLOAT) odm_loc_i16[ix4 + 2U] * vpOutShift[0];
                    exponent = (DOUBLE) fltmp;
                    Ret = AmbaWrap_pow(EXP, exponent, &temp);
                    if (Ret != 0U) {
                        AmbaPrint_PrintUInt5("AmbaWrap_pow failed", 0U, 0U, 0U, 0U, 0U);
                    }
                    dec_w *= (FLOAT) temp;

                    fltmp = var[ix4 + 3U] * (FLOAT) odm_loc_i16[ix4 + 3U] * vpOutShift[0];
                    exponent = (DOUBLE) fltmp;
                    Ret = AmbaWrap_pow(EXP, exponent, &temp);
                    if (Ret != 0U) {
                        AmbaPrint_PrintUInt5("AmbaWrap_pow failed", 0U, 0U, 0U, 0U, 0U);
                    }
                    dec_h *= (FLOAT) temp;
                }

                // copy decoded box
                decbox[0U]  = dec_x_center - (dec_w / 2.0f);
                decbox[1U]  = dec_y_center - (dec_h / 2.0f);
                decbox[2U]  = dec_x_center + (dec_w / 2.0f);
                decbox[3U]  = dec_y_center + (dec_h / 2.0f);

                // boundary checking
                decbox[0U]  = (decbox[0U] < 0.0f) ? 0.0f : decbox[0U];
                decbox[1U]  = (decbox[1U] < 0.0f) ? 0.0f : decbox[1U];
                decbox[2U]  = (decbox[2U] < 0.0f) ? 0.0f : decbox[2U];
                decbox[3U]  = (decbox[3U] < 0.0f) ? 0.0f : decbox[3U];

                decbox[0U]  = (decbox[0U] > 1.0f) ? 1.0f : decbox[0U];
                decbox[1U]  = (decbox[1U] > 1.0f) ? 1.0f : decbox[1U];
                decbox[2U]  = (decbox[2U] > 1.0f) ? 1.0f : decbox[2U];
                decbox[3U]  = (decbox[3U] > 1.0f) ? 1.0f : decbox[3U];

                // copy decoded box
                Ret = AmbaWrap_memcpy(&nms_dec_box[num_dec_bbx], decbox, sizeof(FLOAT) * 4U);
                if (Ret != 0U) {
                    AmbaPrint_PrintUInt5("AmbaWrap_memcpy failed", 0U, 0U, 0U, 0U, 0U);
                }
                nms_dec_box[num_dec_bbx].cls   = pos;
                nms_dec_box[num_dec_bbx].conf  = max;

                // Sort the bbx with conf by using linked list.
                if (num_dec_bbx == 0U) {
                    nms_dec_box[num_dec_bbx].prev_idx = num_dec_bbx;
                    nms_dec_box[num_dec_bbx].next_idx = num_dec_bbx;
                    max_idx = num_dec_bbx;
                } else {
                    if (max > nms_dec_box[max_idx].conf) {
                        nms_dec_box[num_dec_bbx].prev_idx = num_dec_bbx;
                        nms_dec_box[num_dec_bbx].next_idx = max_idx;
                        nms_dec_box[max_idx].prev_idx = num_dec_bbx;
                        max_idx = num_dec_bbx;
                    } else {
                        count = num_dec_bbx;
                        idx = max_idx;
                        while (count > 0U) {
                            if (max > nms_dec_box[idx].conf) {
                                nms_dec_box[num_dec_bbx].prev_idx = nms_dec_box[idx].prev_idx;
                                nms_dec_box[num_dec_bbx].next_idx = idx;
                                nms_dec_box[nms_dec_box[idx].prev_idx].next_idx = num_dec_bbx;
                                nms_dec_box[idx].prev_idx = num_dec_bbx;

                                break;
                            }
                            count--;
                            if (count == 0U) {
                                nms_dec_box[num_dec_bbx].prev_idx = idx;
                                nms_dec_box[num_dec_bbx].next_idx = num_dec_bbx;
                                nms_dec_box[idx].next_idx = num_dec_bbx;
                            }
                            idx = nms_dec_box[idx].next_idx;
                        }
                    }
                }
                num_dec_bbx++;
            }
        }
    }
}

static FLOAT box_overlap(FLOAT x1_min, FLOAT y1_min, FLOAT x1_max, FLOAT y1_max,
                         FLOAT x2_min, FLOAT y2_min, FLOAT x2_max, FLOAT y2_max)
{
    FLOAT xx1 = MAX(x1_min, x2_min);
    FLOAT xx2 = MIN(x1_max, x2_max);
    FLOAT yy1 = MAX(y1_min, y2_min);
    FLOAT yy2 = MIN(y1_max, y2_max);
    FLOAT width = MAX(0.0f, xx2 - xx1);
    FLOAT height = MAX(0.0f, yy2 - yy1);
    FLOAT area = (x2_max - x2_min) * (y2_max - y2_min);

    return (width * height) / area;
}

static void box_nms(amba_od_out_t *od_results, amba_od_candidate_t *od_candidates)
{
    nms_dex_bbx_t *nms_dec_box;
    UINT32 i, j, next, idx;
    FLOAT overlap;

    AmbaMisra_TypeCast(&nms_dec_box, &(nms_box_memblk.pBuffer));

    // nms
    idx = max_idx;
    for (i = 0; i < num_dec_bbx; i++) {

        if (nms_dec_box[idx].conf != 0.0f) {

            send_bbx(nms_dec_box[idx].cls,
                     nms_dec_box[idx].x1, nms_dec_box[idx].y1,
                     nms_dec_box[idx].x2, nms_dec_box[idx].y2,
                     nms_dec_box[idx].conf,
                     od_results, od_candidates);

            next = nms_dec_box[idx].next_idx;
            for (j = i + 1U; j < num_dec_bbx; j++) {
                if (nms_dec_box[next].cls != nms_dec_box[idx].cls) {
                    // skip if they are not the same class
                    next = nms_dec_box[next].next_idx;
                    continue;
                }

                if ((nms_dec_box[next].x1 > nms_dec_box[idx].x2) ||
                    (nms_dec_box[next].x2 < nms_dec_box[idx].x1) ||
                    (nms_dec_box[next].y1 > nms_dec_box[idx].y2) ||
                    (nms_dec_box[next].y2 < nms_dec_box[idx].y1)) {
                    // skip if they are not overlapped
                    next = nms_dec_box[next].next_idx;
                    continue;
                }

                overlap = box_overlap(nms_dec_box[idx].x1, nms_dec_box[idx].y1,
                                      nms_dec_box[idx].x2, nms_dec_box[idx].y2,
                                      nms_dec_box[next].x1, nms_dec_box[next].y1,
                                      nms_dec_box[next].x2, nms_dec_box[next].y2);

                if (overlap > OVERLAP_THR) {
                    // Do not clear the next pointer!!
                    nms_dec_box[next].conf = 0.0f;
                }
                next = nms_dec_box[next].next_idx;
            }
        }
        idx = nms_dec_box[idx].next_idx;
    }
}

static UINT32 OpenOD_Nms(const void* const* In, const void *Out)
{
    ULONG ulongtmp, ulongtmp2;
    const char *p_arm_loc;
    const char *p_arm_conf;
    const char *p_odm_loc;
    const char *p_odm_conf;

    amba_od_out_t *od_results;
    amba_od_candidate_t *od_candidates;

    const cvflow_port_header_t *header;
    ULONG buf_addr[4];
    UINT32 idx, Ret, hdrGroupMode[4];
    FLOAT vpOutShift[4];

    AmbaMisra_TypeCast(&od_results, &Out);

    AmbaMisra_TypeCast(&ulongtmp, &od_results);
    ulongtmp2 = ulongtmp +  (ULONG) sizeof(amba_od_out_t);
    AmbaMisra_TypeCast(&od_candidates, &ulongtmp2);

    Ret = AmbaWrap_memset(od_results, 0x0, sizeof(amba_od_out_t));
    if (Ret != 0U) {
        AmbaPrint_PrintUInt5("AmbaWrap_memset failed", 0U, 0U, 0U, 0U, 0U);
    }
    Ret = AmbaWrap_memset(od_candidates, 0x0, sizeof(amba_od_candidate_t) * MAX_BBX_NUM);
    if (Ret != 0U) {
        AmbaPrint_PrintUInt5("AmbaWrap_memset failed", 0U, 0U, 0U, 0U, 0U);
    }

    Ret = AmbaWrap_memset(buf_addr, 0x0, sizeof(ULONG) * 4U);
    if (Ret != 0U) {
        AmbaPrint_PrintUInt5("AmbaWrap_memset failed", 0U, 0U, 0U, 0U, 0U);
    }
    Ret = AmbaWrap_memset(hdrGroupMode, 0x0, sizeof(UINT32) * 4U);
    if (Ret != 0U) {
        AmbaPrint_PrintUInt5("AmbaWrap_memset failed", 0U, 0U, 0U, 0U, 0U);
    }
    Ret = AmbaWrap_memset(vpOutShift, 0x0, sizeof(FLOAT) * 4U);
    if (Ret != 0U) {
        AmbaPrint_PrintUInt5("AmbaWrap_memset failed", 0U, 0U, 0U, 0U, 0U);
    }

    if (vpOutNum == 1U) { // Group mode with 1 VP output
        AmbaMisra_TypeCast(&ulongtmp, &In[0]);
        AmbaMisra_TypeCast(&header, &In[0]);
        // Check header mode with magic_num of header
#ifdef DEBUG
        AmbaPrint_PrintUInt5("header->magic_num = 0x%x", header->magic_num, 0U, 0U, 0U, 0U);
#endif
        if (header->magic_num == 0xACEFACEDU) {
            // Header mode and grouping to 1 VP output; hdrGroupMode 3
            decode_header(header, buf_addr, vpOutShift);
            for (idx = 0U; idx < header->num_packed; idx ++) {
                hdrGroupMode[idx] = 3U;
            }
        } else {
            // Non-header mode and grouping to 1 VP output; hdrGroupMode 2
            // Need manually fetching the data_format and size from .vas and orc cvtask
            // Pass for too complicated and minute.
#ifdef DEBUG
            AmbaPrint_PrintUInt5("Non-header mode with grouped outputs is not supported", 0U, 0U, 0U, 0U, 0U);
#endif
        }
    } else { // Non-group mode with 4 VP outputs
        for (idx = 0U; idx < vpOutNum; idx++) {
            AmbaMisra_TypeCast(&ulongtmp, &In[idx]);
            AmbaMisra_TypeCast(&header, &In[idx]);
            // Check header mode with magic_num of header
#ifdef DEBUG
            AmbaPrint_PrintUInt5("header->magic_num = 0x%x", header->magic_num, 0U, 0U, 0U, 0U);
#endif
            if (header->magic_num == 0xACEFACEDU) {
                // Header mode and no grouping; hdrGroupMode 1
                hdrGroupMode[idx] = 1U;
                decode_header(header, &buf_addr[idx], &vpOutShift[idx]);
            } else {
                // Non-header mode and 4 VP outputs with USR_OUTPUT_FP32_FLAG; hdrGroupMode 0
                hdrGroupMode[idx] = 0U;
                vpOutShift[idx] = 1.0f;
            }
        }
    }

    // convert odm_loc data format
    if(hdrGroupMode[0] == 0U) {
        AmbaMisra_TypeCast(&p_odm_loc, &In[0]);
    } else {
        AmbaMisra_TypeCast(&p_odm_loc, &buf_addr[0]);
    }

    // convert odm_conf data format
    if(hdrGroupMode[1] == 0U) {
        AmbaMisra_TypeCast(&p_odm_conf, &In[1]);
    } else {
        AmbaMisra_TypeCast(&p_odm_conf, &buf_addr[1]);
    }

    // convert arm_loc data format
    if(hdrGroupMode[2] == 0U) {
        AmbaMisra_TypeCast(&p_arm_loc, &In[2]);
    } else {
        AmbaMisra_TypeCast(&p_arm_loc, &buf_addr[2]);
    }

    // convert arm_conf data format
    if(hdrGroupMode[3] == 0U) {
        AmbaMisra_TypeCast(&p_arm_conf, &In[3]);
    } else {
        AmbaMisra_TypeCast(&p_arm_conf, &buf_addr[3]);
    }

    send_bbx(255U, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, od_results, od_candidates);

    bbox_regression(p_arm_conf, p_arm_loc, p_odm_conf, p_odm_loc, vpOutShift);

    box_nms(od_results, od_candidates);


    od_results->capture_time    = 0U;
    od_results->frame_num       = 0U;
    od_results->objects_offset  = (UINT32) (sizeof(amba_od_out_t));

    od_results->class_name = 0U;
    AmbaMisra_TypeCast(&od_results->class_name, &cls_name_blk.buffer_daddr);
    od_results->class_name_p64 = 0UL;
    AmbaMisra_TypeCast(&od_results->class_name_p64, &cls_name_blk.pBuffer);

    return 0U;
}

static void CCFCbHdlrFD0(const CCF_OUTPUT_s *pEventData)
{
    const SVC_CV_ALGO_HANDLE_s* Hdlr;
    UINT32 idx, Ret;
    SVC_CV_ALGO_OUTPUT_s Out;
    const void* vpOutNms;
    // Length set to 4 for flexibility between using grouping or not
    const void* vpInNms[4];
    amba_od_out_t *pOut;

    AmbaMisra_TypeCast(&Hdlr, &(pEventData->pCvAlgoHdlr));

    Out.pUserData = pEventData->pUserData;
    Out.pOutput = pEventData->pOut;
    Out.pExtOutput = NULL;

    for (idx = 0U; idx < MAX_CALLBACK_NUM; idx++) {
        if (Hdlr->Callback[idx] != NULL) {
            (void)Hdlr->Callback[idx](CALLBACK_EVENT_FLEXIDAG_OUTPUT, &Out);
        }
    }

    Ret = AmbaWrap_memset(vpInNms, 0x0, sizeof(vpInNms));
    if (Ret != 0U) {
        AmbaPrint_PrintUInt5("AmbaWrap_memset failed", 0U, 0U, 0U, 0U, 0U);
    }

    for(idx = 0U; idx < vpOutNum; idx++) {
        (void) AmbaCV_UtilityCmaMemInvalid(&pEventData->pOut->buf[idx]);
        AmbaMisra_TypeCast(&vpInNms[idx], &(pEventData->pOut->buf[idx].pBuffer));
    }
    AmbaMisra_TypeCast(&vpOutNms, &(pEventData->pOut->buf[vpOutNum].pBuffer));

    (void) OpenOD_Nms(vpInNms, vpOutNms);

    AmbaMisra_TypeCast(&pOut,&vpOutNms);
    Out.pExtOutput = pOut;

    for (idx = 0U; idx < MAX_CALLBACK_NUM; idx++) {
        if (Hdlr->Callback[idx] != NULL) {
            (void)Hdlr->Callback[idx](CALLBACK_EVENT_FREE_INPUT, &Out);
            (void)Hdlr->Callback[idx](CALLBACK_EVENT_OUTPUT, &Out);
        }
    }
}

static UINT32 OpenOD_Query(SVC_CV_ALGO_HANDLE_s* Hdlr, SVC_CV_ALGO_QUERY_CFG_s *Cfg)
{
    const UINT32 *pNumScale;
    UINT32 Ret;
    //Use cls_name to store the NUM_CLS char pointers to each class
    static const char* const cls_name[NUM_CLS] = {
        "background",
        "car",
        "cycle",
        "person"
    };

    // assign obj
    Hdlr->pAlgoObj = Cfg->pAlgoObj;
    Cfg->TotalReqBufSz = ((UINT32) 30U<<20U); //30M

    if (Cfg->pExtQueryCfg != NULL) {
        AmbaMisra_TypeCast(&pNumScale, &Cfg->pExtQueryCfg);
        Cfg->TotalReqBufSz *= *pNumScale;
    }

    Ret = AmbaCV_UtilityCmaMemAlloc((UINT32)(sizeof(nms_dex_bbx_t)) * 40800U, 1U, &nms_box_memblk);
    if (Ret != 0U) {
        AmbaPrint_PrintUInt5("AmbaCV_UtilityCmaMemAlloc SCA_OpenOD:nms_box_memblk fail", 0U, 0U, 0U, 0U, 0U);
    }
    Ret = AmbaCV_UtilityCmaMemAlloc((UINT32)(sizeof(cls_name)), 1U, &cls_name_blk);
    if (Ret != 0U) {
        AmbaPrint_PrintUInt5("AmbaCV_UtilityCmaMemAlloc SCA_OpenOD:cls_name_blk fail", 0U, 0U, 0U, 0U, 0U);
    } else {
        Ret = AmbaWrap_memcpy(cls_name_blk.pBuffer, cls_name, sizeof(cls_name));
        if (Ret != 0U) {
            AmbaPrint_PrintUInt5("AmbaWrap_memcpy failed", 0U, 0U, 0U, 0U, 0U);
        }
    }

    return CVALGO_OK;
}

static UINT32 OpenOD_Control(SVC_CV_ALGO_HANDLE_s* Hdlr, const SVC_CV_ALGO_CTRL_CFG_s *Cfg)
{
// UUID is defined in flow table
#define OPENOD_VP_UUID          (2U)

    UINT32 MsgSz = 0U;
    UINT32 UUID = 0U;
    UINT32 MsgCode = 0U;
    UINT32 Ret = 0U, U32tmp, SendMsg = 1U;
    const amba_roi_config_t *cfg_ptr;
    const OPENOD_PB_CTRL_s *cfg_pb;

    Hdlr->State = 2U;

    AmbaMisra_TypeCast(&cfg_ptr, &Cfg->pCtrlParam);
    MsgCode = cfg_ptr->msg_type;

    switch (MsgCode) {
    case CTRL_TYPE_PBOX:
        AmbaMisra_TypeCast(&cfg_pb, &Cfg->pCtrlParam);
        g_prior_box = cfg_pb->pPriorBox;
        pb_len = cfg_pb->PBLen;
        AmbaMisra_TypeCast(&U32tmp,&g_prior_box);
        AmbaPrint_PrintUInt5("Pre-loaded priorbox at 0x%x, with size %d passed by OPENOD_PB_CTRL_s",U32tmp, pb_len,0U,0U,0U);
        total_bbox = pb_len >> 5U;
        SendMsg = 0U;
        break;
    case AMBA_ROI_CONFIG_MSG:
        AmbaMisra_TypeCast(&cfg_ptr, &Cfg->pCtrlParam);
        roi_cfg.image_pyramid_index = cfg_ptr->image_pyramid_index;
        roi_cfg.source_vin = cfg_ptr->source_vin;
        roi_cfg.roi_start_col = cfg_ptr->roi_start_col;
        roi_cfg.roi_start_row = cfg_ptr->roi_start_row;
        roi_cfg.roi_width = cfg_ptr->roi_width;
        roi_cfg.roi_height = cfg_ptr->roi_height;
        if (g_prior_box == NULL) {
            AmbaPrint_PrintUInt5("Please load the priorbox in advance.", 0U, 0U, 0U, 0U, 0U);
            AmbaPrint_PrintUInt5(" Pass the priorbox and its length by OPENOD_PB_CTRL_s && msg_type as CTRL_TYPE_PBOX", 0U, 0U, 0U, 0U, 0U);
        }
        MsgSz = (UINT32) (sizeof(amba_roi_config_t));
        UUID = OPENOD_VP_UUID;
        break;
    default:
        Ret = 0xFFFFFFFFU;
        break;
    }

    if ((Ret == 0U) && (SendMsg == 1U)) {
        CCF_SEND_MSG_CFG_s MsgCfg;
        MsgCfg.MsgSize = MsgSz;
        MsgCfg.pMsg = Cfg->pCtrlParam;
        MsgCfg.UUID = UUID;
        Ret = CvCommFlexi_SendMsg(&Hdlr->FDs[0], &MsgCfg);
    }

    return Ret;
}

static void OpenOD_DefaultConfigCCF(SVC_CV_ALGO_HANDLE_s* pHdlr)
{
    amba_roi_config_t roi_msg;
    amba_roi_config_t *p_roi_msg = &roi_msg;
    SVC_CV_ALGO_CTRL_CFG_s MsgCfg;
    MsgCfg.pExtCtrlCfg = NULL;
    MsgCfg.CtrlType = 0;

    p_roi_msg->msg_type = AMBA_ROI_CONFIG_MSG;
    p_roi_msg->image_pyramid_index = 1U;
    p_roi_msg->source_vin = 0U;
    p_roi_msg->roi_start_col = 9999U;
    p_roi_msg->roi_start_row = 9999U;
    p_roi_msg->roi_width = 1024U;
    p_roi_msg->roi_height = 640U;

    AmbaMisra_TypeCast(&MsgCfg.pCtrlParam, &p_roi_msg);
    (void)OpenOD_Control(pHdlr, &MsgCfg);
}

static UINT32 OpenOD_Create(SVC_CV_ALGO_HANDLE_s* Hdlr, SVC_CV_ALGO_CREATE_CFG_s *Cfg)
{
    CCF_TSK_CTRL_s Ctrl;
    CCF_CREATE_CFG_s CreCfg;
    CCF_OUTPUT_INFO_s OutInfo;
    CCF_REGCB_CFG_s CBCfg;
    UINT32 idx;
    UINT32 Ret;
    const ExtTaskCreateCfg *pExt = NULL;

    // create mempool
    Hdlr->pAlgoBuf = Cfg->pAlgoBuf;
    (void)ArmMemPool_Create(Hdlr->pAlgoBuf, &Hdlr->MemPoolId);
    // clean cb function;
    for (idx = 0U; idx < MAX_CALLBACK_NUM; idx++) {
        Hdlr->Callback[idx] = NULL;
    }

    Hdlr->NumFD = 1U;

    //create cfg for FD0
    AmbaUtility_StringCopy(CreCfg.FDName, 7U, "OpenOD");
    Ctrl.CoreSel = 0xE;
    Ctrl.Priority = 50;
    Ctrl.StackSz = ((UINT32) 16U<<10U); //16K
    CreCfg.RunTskCtrl = Ctrl;
    AmbaMisra_TouchUnused(&Ctrl);

    Ctrl.CoreSel = 0xE;
    Ctrl.Priority = 51;
    Ctrl.StackSz = ((UINT32) 16U<<10U); //16K
    CreCfg.CBTskCtrl = Ctrl;
    AmbaMisra_TouchUnused(&Ctrl);

    if (Cfg->pExtCreateCfg != NULL) {
        AmbaMisra_TypeCast(&pExt, &Cfg->pExtCreateCfg);
        if (pExt->MagicCode == ExtTaskCreateCfgMagic) {
            Ret = AmbaWrap_memcpy(&CreCfg.RunTskCtrl, &pExt->RunTskCtrl, sizeof(CCF_TSK_CTRL_s));
            if (Ret != 0U) {
                AmbaPrint_PrintUInt5("AmbaWrap_memcpy failed", 0U, 0U, 0U, 0U, 0U);
            }
            Ret = AmbaWrap_memcpy(&CreCfg.CBTskCtrl, &pExt->CBTskCtrl, sizeof(CCF_TSK_CTRL_s));
            if (Ret != 0U) {
                AmbaPrint_PrintUInt5("AmbaWrap_memcpy failed", 0U, 0U, 0U, 0U, 0U);
            }
        }
    }

    CreCfg.InBufDepth = 0;
    CreCfg.OutBufDepth = 0;
    CreCfg.MemPoolId = Hdlr->MemPoolId;
    CreCfg.pBin = Cfg->pBin[0];
    Ret = CvCommFlexi_Create(&Hdlr->FDs[0], &CreCfg, &OutInfo);
    //Register Callback for FD0
    if (Ret == CVALGO_OK) {
        vpOutNum = OutInfo.OutputNum;
        CBCfg.Mode = 0U;
        CBCfg.Callback = CCFCbHdlrFD0;
        Ret = CvCommFlexi_RegCallback(&Hdlr->FDs[0], &CBCfg);

        Cfg->OutputNum = OutInfo.OutputNum + 1U;
        for (idx = 0U; idx < OutInfo.OutputNum; idx++) {
            Cfg->OutputSz[idx] = OutInfo.OutputSz[idx];
        }
        Cfg->OutputSz[OutInfo.OutputNum] = (UINT32)(sizeof(amba_od_out_t)) + ((UINT32)(sizeof(amba_od_candidate_t)) * MAX_BBX_NUM);
        Hdlr->State = 1U;
    }

    return Ret;
}

static UINT32 OpenOD_Delete(SVC_CV_ALGO_HANDLE_s* Hdlr, const SVC_CV_ALGO_DELETE_CFG_s *Cfg)
{
    UINT32 Ret;

    (void)Cfg;

    Ret = ArmMemPool_Free(Hdlr->MemPoolId, &(Hdlr->FDs[0].FlexiBuf.State));
    Ret |= ArmMemPool_Free(Hdlr->MemPoolId, &(Hdlr->FDs[0].FlexiBuf.Temp));
    if (Ret == ARM_OK) {
        Ret = CvCommFlexi_Delete(&Hdlr->FDs[0]);
        if (Ret == CVALGO_OK) {
            Ret = ArmMemPool_Delete(Hdlr->MemPoolId);
        }
    }
    if(nms_box_memblk.pBuffer != NULL) {
        if(AmbaCV_UtilityCmaMemFree(&nms_box_memblk) != 0U) {
            AmbaPrint_PrintUInt5("OpenOD AmbaCV_UtilityCmaMemFreenms_box_memblk failed", 0U, 0U, 0U, 0U, 0U);
        }
    }
    if(cls_name_blk.pBuffer != NULL) {
        if(AmbaCV_UtilityCmaMemFree(&cls_name_blk) != 0U) {
            AmbaPrint_PrintUInt5("OpenOD AmbaCV_UtilityCmaMemFree cls_name_blk failed", 0U, 0U, 0U, 0U, 0U);
        }
    }

    return Ret;
}

static UINT32 OpenOD_Feed(SVC_CV_ALGO_HANDLE_s* Hdlr, const SVC_CV_ALGO_FEED_CFG_s *Cfg)
{
    CCF_FEED_CFG_s FeedCfg;
    UINT32 idx, Ret;
    const cv_pic_info_t *PicInfo = NULL;

    if (Hdlr->State == 1U) {
        // send default config
        OpenOD_DefaultConfigCCF(Hdlr);
        Hdlr->State = 2U;
    }

    FeedCfg.pCvAlgoHdlr = Hdlr;
    FeedCfg.pIn = Cfg->pIn;
    FeedCfg.pOut = Cfg->pOut;
    FeedCfg.pUserData = Cfg->pUserData;
    FeedCfg.pOut->num_of_buf = vpOutNum;
    FeedCfg.pInternal = NULL;
    FeedCfg.pRaw = NULL;
    FeedCfg.pPic = NULL;

    AmbaMisra_TypeCast(&PicInfo, &Cfg->pExtFeedCfg);
    my_ctx.raw_w = (UINT32) PicInfo->pyramid.half_octave[0U].roi_width_m1 + 1U;
    my_ctx.raw_h = (UINT32) PicInfo->pyramid.half_octave[0U].roi_height_m1 + 1U;
    my_ctx.roi_w = (UINT32) PicInfo->pyramid.half_octave[roi_cfg.image_pyramid_index].roi_width_m1 + 1U;
    my_ctx.roi_h = (UINT32) PicInfo->pyramid.half_octave[roi_cfg.image_pyramid_index].roi_height_m1 + 1U;
    if (roi_cfg.roi_start_col == 9999U) {
        my_ctx.roi_start_col = (my_ctx.roi_w - NET_INPUT_W) / 2U;
    } else if (my_ctx.roi_start_col > (my_ctx.roi_w - NET_INPUT_W)) {
        my_ctx.roi_start_col = my_ctx.roi_w - NET_INPUT_W;
    } else {
        my_ctx.roi_start_col = roi_cfg.roi_start_col;
    }
    if (roi_cfg.roi_start_row == 9999U) {
        my_ctx.roi_start_row = (my_ctx.roi_h - NET_INPUT_H) / 2U;
    } else if (my_ctx.roi_start_row > (my_ctx.roi_h - NET_INPUT_H)) {
        my_ctx.roi_start_row = my_ctx.roi_h - NET_INPUT_H;
    } else {
        my_ctx.roi_start_row = roi_cfg.roi_start_row;
    }
    my_ctx.net_in_w       = NET_INPUT_W;
    my_ctx.net_in_h       = NET_INPUT_H;

    for (idx = 0U; idx < FeedCfg.pOut->num_of_buf; idx++) {
        (void) AmbaCV_UtilityCmaMemClean(&FeedCfg.pOut->buf[idx]);
    }

    Ret = CvCommFlexi_Feed(&Hdlr->FDs[0], &FeedCfg);

    return Ret;
}

SVC_CV_ALGO_OBJ_s OpenODAlgoObj = {
    .Query = OpenOD_Query,
    .Create = OpenOD_Create,
    .Delete = OpenOD_Delete,
    .Feed = OpenOD_Feed,
    .Control = OpenOD_Control
};

