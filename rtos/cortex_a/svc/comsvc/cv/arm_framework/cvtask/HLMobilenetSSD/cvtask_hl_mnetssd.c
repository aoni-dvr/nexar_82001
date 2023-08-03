/**
 *  @file cvtask_hl_mnetssd.c
 *
 *  Copyright (c) 2020 Ambarella International LP
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
 *  @details High-level cvtask for Mnetssd
 *
 */
#include "AmbaWrap.h"
#include "AmbaKAL.h"
#include "AmbaFS.h"
#include "AmbaPrint.h"
#include "AmbaUtility.h"
#include "AmbaMisraFix.h"
#include "cvtask_ossrv.h"
#include "cvtask_api.h"
#include "cvtask_errno.h"
#include "cvapi_ambaod.h"


#define CVTASK_NAME         "HL_MNETSSD"

#define NUM_CLS             7
#define CONF_THRESHOLD      0.4f
#define OVERLAP_THRESHOLD   0.45f

#define EXP                 2.71828182845904

#define AMBAOD_MAX_BBX_NUM  200

typedef struct {
    UINT32 raw_w;
    UINT32 raw_h;

    UINT32 roi_w;
    UINT32 roi_h;

    UINT32 roi_start_col;
    UINT32 roi_start_row;

    UINT32 net_in_w;
    UINT32 net_in_h;
} hl_win_ctx_t;

typedef struct {
    amba_od_out_t Info;
    amba_od_candidate_t Bbx[AMBAOD_MAX_BBX_NUM];
} AMBA_OD_s;


typedef struct {
    UINT32 total_bbox;
    UINT32 pb_len;
    UINT32 rounding;
    AMBA_OD_s msg2memio;
    hl_win_ctx_t mnetssd_ctx;
    FLOAT *g_prior_box;
    FLOAT *g_conf;
    FLOAT *g_detect_box;
    FLOAT *g_decode_box;
    FLOAT *g_nms_box;
} hl_mnetssd_private_t;

static char *g_cls_name;
static ULONG (*g_pcls_name)[NUM_CLS];
static INT32 num_cls = NUM_CLS;
static const char cls_name[NUM_CLS][16] = {
    "backgrounding",
    "bicycle",
    "bus",
    "car",
    "motorbike",
    "person",
    "train",
};


extern uint32_t cvtask_hl_mnetssd_create(void);

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

static inline UINT16 cal_pos(FLOAT minmax, UINT32 net_in,
                             UINT32 roi_start, UINT32 raw, UINT32 roi)
{
    FLOAT fltmp;
    UINT16 u16tmp;

    /* (((min, max) * net_in) + roi_start) * raw / roi; */

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

    /* (MAX(x_min, x_max) - MIN(x_min, x_max)) * net_in_w / roi_w * raw_w; */
    fltmp = max - min;

    fltmp *= (FLOAT) net_in;

    fltmp /= (FLOAT) roi;

    fltmp *= (FLOAT) raw;

    u16tmp = (UINT16) fltmp;

    return u16tmp;
}

static void send_bbx(hl_mnetssd_private_t *priv, UINT8 target_class, FLOAT x_min, FLOAT y_min,
                     FLOAT x_max, FLOAT y_max, FLOAT score)
{
    const hl_win_ctx_t *p_ctx = &priv->mnetssd_ctx;

    UINT32 raw_w          = p_ctx->raw_w;
    UINT32 raw_h          = p_ctx->raw_h;
    UINT32 roi_w          = p_ctx->roi_w;
    UINT32 roi_h          = p_ctx->roi_h;
    UINT32 roi_start_col  = p_ctx->roi_start_col;
    UINT32 roi_start_row  = p_ctx->roi_start_row;
    UINT32 net_in_w       = p_ctx->net_in_w;
    UINT32 net_in_h       = p_ctx->net_in_h;
    FLOAT fltmp;
    UINT32 u32tmp;

    if (raw_w != 0U) {
        priv->msg2memio.Info.capture_time = 0;
        priv->msg2memio.Info.frame_num = 0;
        priv->msg2memio.Info.objects_offset = (UINT32)sizeof(amba_od_out_t);
        priv->msg2memio.Bbx[priv->msg2memio.Info.num_objects].field = 0;
        priv->msg2memio.Bbx[priv->msg2memio.Info.num_objects].clsId = target_class;
        priv->msg2memio.Bbx[priv->msg2memio.Info.num_objects].bb_start_col =
            (INT32)cal_pos(MIN(x_min, x_max), net_in_w, roi_start_col, raw_w, roi_w);
        priv->msg2memio.Bbx[priv->msg2memio.Info.num_objects].bb_start_row =
            (INT32)cal_pos(MIN(y_min, y_max), net_in_h, roi_start_row, raw_h, roi_h);
        priv->msg2memio.Bbx[priv->msg2memio.Info.num_objects].bb_width_m1 =
            (UINT32)cal_wh(MAX(x_min, x_max), MIN(x_min, x_max), net_in_w, roi_w, raw_w) - 1U;
        priv->msg2memio.Bbx[priv->msg2memio.Info.num_objects].bb_height_m1 =
            (UINT32)cal_wh(MAX(y_min, y_max), MIN(y_min, y_max), net_in_h, roi_h, raw_h) - 1U;
        fltmp = score * 255.0f;
        u32tmp = (UINT32) fltmp;
        priv->msg2memio.Bbx[priv->msg2memio.Info.num_objects].score = u32tmp;
        priv->msg2memio.Info.num_objects = priv->msg2memio.Info.num_objects + 1U;
    }
}

static UINT32 load_binary(const char *filename, FLOAT *dst, UINT32 byte_size)
{
    AMBA_FS_FILE *ifp;
    UINT32 rval = CVTASK_ERR_OK;
    UINT32 count;
    UINT32 ret = 0U,ret1 = 0U;

    ret1 = AmbaFS_FileOpen(filename, "rb", &ifp);
    if ((ifp == NULL) || (ret1 != 0U)) {
        AmbaPrint_PrintStr5("Can't load file %s!", filename, NULL, NULL, NULL, NULL);
        rval = CVTASK_ERR_NULL_POINTER;
    } else {
        ret |= AmbaFS_FileRead(dst, 1, byte_size, ifp, &count);
        if (count == 0U) {
            rval = CVTASK_ERR_GENERAL;
        }
        ret |= AmbaFS_FileClose(ifp);
    }

    (void) ret;
    return rval;
}

static UINT32 get_file_length(const char *filename, UINT32 *pb_len)
{
    AMBA_FS_FILE *ifp;
    UINT32 rval = CVTASK_ERR_OK;
    UINT64 pb_len_u64;
    UINT32 ret = 0U,ret1 = 0U;

    ret1 = AmbaFS_FileOpen(filename, "rb", &ifp);
    if ((ifp == NULL) || (ret1 != 0U)) {
        AmbaPrint_PrintStr5("Can't load file %s!", filename, NULL, NULL, NULL, NULL);
        rval = CVTASK_ERR_NULL_POINTER;
    } else {
        rval = AmbaFS_FileSeek(ifp, 0, AMBA_FS_SEEK_END);
        if (rval != 0U) {
            rval = CVTASK_ERR_GENERAL;
        } else {
            ret |= AmbaFS_FileTell(ifp, &pb_len_u64);
            *pb_len = (UINT32)(pb_len_u64 & 0xFFFFFFFFU);
        }
        ret |= AmbaFS_FileClose(ifp);
    }
    (void) ret;
    return rval;
}

static INT32 check_threshold(const FLOAT *softmax)
{
    INT32 i, rval = 0;

    for (i = 1; i < num_cls; i++) {
        if (softmax[i] >= CONF_THRESHOLD) {
            rval = 1;
            break;
        }
    }

    return rval;
}

static void box_regression(const hl_mnetssd_private_t *priv)
{
    const FLOAT *var;
    const FLOAT *softmax = priv->g_conf;
    const FLOAT *pbox = priv->g_prior_box;
    const FLOAT *detbox = priv->g_detect_box;
    FLOAT *decbox = priv->g_decode_box;
    FLOAT prior_x_center, prior_y_center;
    FLOAT prior_w, prior_h;
    FLOAT det_x_min, det_y_min, det_x_max, det_y_max;
    FLOAT det_x_center, det_y_center;
    FLOAT det_w, det_h;
    UINT32 i, ix4, count = 0;
    ULONG g_prior_box, addrtmp;
    FLOAT fltmp;
    DOUBLE st_exp, temp;
    UINT32 ret = 0U;

    AmbaMisra_TypeCast(&g_prior_box, &priv->g_prior_box);
    addrtmp = g_prior_box + ((ULONG)priv->pb_len >> 1);
    AmbaMisra_TypeCast(&var, &addrtmp);

    for (i = 0; i < priv->total_bbox; i++) {
        if (check_threshold(&softmax[i * (UINT32)num_cls]) != 0) {
            count++;

            ix4 = i * 4U;
            /* prior box */
            prior_x_center = (pbox[ix4] + pbox[ix4 + 2U]) / 2.0f;
            prior_y_center = (pbox[ix4 + 1U] + pbox[ix4 + 3U]) / 2.0f;
            prior_w  = pbox[ix4 + 2U] - pbox[ix4];
            prior_h = pbox[ix4 + 3U] - pbox[ix4 + 1U];

            /* detect box */
            det_x_min = detbox[ix4];
            det_y_min = detbox[ix4 + 1U];
            det_x_max = detbox[ix4 + 2U];
            det_y_max = detbox[ix4 + 3U];
            det_x_center = (var[ix4] * det_x_min * prior_w) + prior_x_center;
            det_y_center = (var[ix4 + 1U] * det_y_min * prior_h) + prior_y_center;

            fltmp = var[ix4 + 2U] * det_x_max;
            st_exp = (DOUBLE) fltmp;
            if(AmbaWrap_pow(EXP, st_exp, &temp) != 0U) {
                AmbaPrint_PrintUInt5("box_regression : AmbaWrap_pow fail", 0U, 0U, 0U, 0U, 0U);
            }
            det_w = (FLOAT) temp;
            det_w *= prior_w;

            fltmp = var[ix4 + 3U] * det_y_max;
            st_exp = (DOUBLE) fltmp;
            if(AmbaWrap_pow(EXP, st_exp, &temp) != 0U) {
                AmbaPrint_PrintUInt5("box_regression : AmbaWrap_pow fail", 0U, 0U, 0U, 0U, 0U);
            }
            det_h = (FLOAT) temp;
            det_h *= prior_h;

            /* decode box */
            decbox[ix4]         = det_x_center - (det_w / 2.0f);
            decbox[ix4 + 1U]    = det_y_center - (det_h / 2.0f);
            decbox[ix4 + 2U]    = det_x_center + (det_w / 2.0f);
            decbox[ix4 + 3U]    = det_y_center + (det_h / 2.0f);

            decbox[ix4]         = (decbox[ix4] < 0.0f) ? 0.0f : decbox[ix4];
            decbox[ix4 + 1U]    = (decbox[ix4 + 1U] < 0.0f) ? 0.0f : decbox[ix4 + 1U];
            decbox[ix4 + 2U]    = (decbox[ix4 + 2U] < 0.0f) ? 0.0f : decbox[ix4 + 2U];
            decbox[ix4 + 3U]    = (decbox[ix4 + 3U] < 0.0f) ? 0.0f : decbox[ix4 + 3U];

            decbox[ix4]         = (decbox[ix4] > 1.0f) ? 1.0f : decbox[ix4];
            decbox[ix4 + 1U]    = (decbox[ix4 + 1U] > 1.0f) ? 1.0f : decbox[ix4 + 1U];
            decbox[ix4 + 2U]    = (decbox[ix4 + 2U] > 1.0f) ? 1.0f : decbox[ix4 + 2U];
            decbox[ix4 + 3U]    = (decbox[ix4 + 3U] > 1.0f) ? 1.0f : decbox[ix4 + 3U];

#if 0
            AmbaPrint("softmax[%d]= %f, %f, %f, %f, %f, %f, %f", i,
                      softmax[i * num_cls + 0], softmax[i * num_cls + 1],
                      softmax[i * num_cls + 2], softmax[i * num_cls + 3],
                      softmax[i * num_cls + 4], softmax[i * num_cls + 5],
                      softmax[i * num_cls + 6]);
            AmbaPrint("decbox[%d]= %f, %f, %f, %f", i,
                      decbox[ix4 + 0], decbox[ix4 + 1],
                      decbox[ix4 + 2], decbox[ix4 + 3]);
            AmbaUtility_FloatToStr(str, 12, softmax[i * num_cls + 1], 4);
            AmbaPrint_PrintStr5("softmax[1]: %s", str, NULL, NULL, NULL, NULL);
            AmbaUtility_FloatToStr(str, 12, softmax[i * num_cls + 2], 4);
            AmbaPrint_PrintStr5("softmax[2]: %s", str, NULL, NULL, NULL, NULL);
            AmbaUtility_FloatToStr(str, 12, softmax[i * num_cls + 3], 4);
            AmbaPrint_PrintStr5("softmax[3]: %s", str, NULL, NULL, NULL, NULL);
            AmbaUtility_FloatToStr(str, 12, softmax[i * num_cls + 4], 4);
            AmbaPrint_PrintStr5("softmax[4]: %s", str, NULL, NULL, NULL, NULL);
            AmbaUtility_FloatToStr(str, 12, softmax[i * num_cls + 5], 4);
            AmbaPrint_PrintStr5("softmax[5]: %s", str, NULL, NULL, NULL, NULL);
            AmbaUtility_FloatToStr(str, 12, softmax[i * num_cls + 6], 4);
            AmbaPrint_PrintStr5("softmax[6]: %s", str, NULL, NULL, NULL, NULL);
#endif
        }
        (void) ret;
    }
    //AmbaPrint_PrintUInt5("filtered boxes: %d", count, 0, 0, 0, 0);
    (void) count;
}

static void swap_row_float(FLOAT *input, UINT32 row_x, UINT32 row_y)
{
    FLOAT temp[5] = { 0.0f };
    const void *dst;

    dst = cvtask_memcpy(temp, &input[row_x * 5U], sizeof(FLOAT) * 5U);
    if(dst == NULL) {
        AmbaPrint_PrintUInt5("swap_row_float step1 error ", 0U, 0U, 0U, 0U, 0U);
    }
    dst = cvtask_memcpy(&input[row_x * 5U], &input[row_y * 5U], sizeof(FLOAT) * 5U);
    if(dst == NULL) {
        AmbaPrint_PrintUInt5("swap_row_float step2 error ", 0U, 0U, 0U, 0U, 0U);
    }
    dst = cvtask_memcpy(&input[row_y * 5U], temp, sizeof(FLOAT) * 5U);
    if(dst == NULL) {
        AmbaPrint_PrintUInt5("swap_row_float step3 error ", 0U, 0U, 0U, 0U, 0U);
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

static void box_nms(hl_mnetssd_private_t *priv, UINT8 target_class, FLOAT threshold)
{
    FLOAT *nms_box = priv->g_nms_box;
    const FLOAT *conf = priv->g_conf;
    const FLOAT *decbox = priv->g_decode_box;
    UINT32 i, j, nms_number = 0, ix5, jx5;
    FLOAT overlap;
    const void *dst;

    dst  = cvtask_memset(nms_box, 0, ((size_t)priv->total_bbox * (size_t)5U * ((size_t)sizeof(FLOAT))));

    /* copy valid data */
    for (i = 0; i < priv->total_bbox; i++) {
        if (conf[(i * (UINT32)num_cls) + target_class] >= CONF_THRESHOLD) {
            dst = cvtask_memcpy(&nms_box[nms_number * 5U], &decbox[i * 4U],
                                sizeof(FLOAT) * 4U);
            if(dst == NULL) {
                AmbaPrint_PrintUInt5("box_nms cvtask_memcpy error ", 0U, 0U, 0U, 0U, 0U);
            }
            nms_box[(nms_number * 5U) + 4U] = conf[(i * (UINT32)num_cls) + target_class];
            nms_number++;
        }
    }

    /* bubble sort */
    for (i = 0; i < nms_number; i++) {
        for (j = 1; j < (nms_number - i); j++) {
            if (nms_box[(j * 5U) + 4U] > nms_box[((j - 1U) * 5U) + 4U]) {
                swap_row_float(nms_box, j, (j - 1U));
            }
        }
    }

    /* nms */
    for (i = 0; i < nms_number; i++) {
        if (nms_box[i * 5U] != 0.0f) {
            ix5 = i * 5U;
#if 0
            AmbaPrint("after nms: nms_box[class=%d][%d] = %f, %f, %f, %f, %f",
                      target_class, i,
                      nms_box[ix5 + 0], nms_box[ix5 + 1],
                      nms_box[ix5 + 2], nms_box[ix5 + 3],
                      nms_box[ix5 + 4]);
#endif

            send_bbx(priv, target_class, nms_box[ix5], nms_box[ix5 + 1U],
                     nms_box[ix5 + 2U], nms_box[ix5 + 3U], nms_box[ix5 + 4U]);

            for (j = i + 1U; j < nms_number; j++) {
                jx5 = j * 5U;
                overlap = box_overlap(nms_box[ix5], nms_box[ix5 + 1U],
                                      nms_box[ix5 + 2U], nms_box[ix5 + 3U],
                                      nms_box[jx5], nms_box[jx5 + 1U],
                                      nms_box[jx5 + 2U], nms_box[jx5 + 3U]);
                if (overlap > threshold) {
                    dst = cvtask_memset(&nms_box[jx5], 0, sizeof(FLOAT) * 5U);
                    if(dst == NULL) {
                        AmbaPrint_PrintUInt5("box_nms cvtask_memset 2 error ", 0U, 0U, 0U, 0U, 0U);
                    }
                }
            }
        }
    }
}

static uint32_t cvtask_hl_mnetssd_query(uint32_t iid, const uint32_t *config,
                                        cvtask_memory_interface_t *dst)
{
    cvtask_memory_interface_t cvtask_hl_mnetssd_interface;
    UINT32 retcode;
    char label_in[32] = "HL_IN_00_IONAME_###", label_out[32] = "HL_OUT_00_IONAME_###";
    const void *ioname;
    const char *str;
    UINT32 str_size, res;
    UINT32 total_bbox;
    UINT32 pb_len = 0U;
    const void *dst_ptr;

    (void) iid;

    retcode = CVTASK_ERR_OK;

    dst_ptr  = cvtask_memset(&cvtask_hl_mnetssd_interface, 0, sizeof(cvtask_hl_mnetssd_interface));
    if(dst_ptr == NULL) {
        AmbaPrint_PrintUInt5("cvtask_hl_mnetssd_query cvtask_memset  error ", 0U, 0U, 0U, 0U, 0U);
    }
    cvtask_hl_mnetssd_interface.Instance_private_storage_needed     = 0U;
    cvtask_hl_mnetssd_interface.DRAM_temporary_scratchpad_needed    = 0U;
    cvtask_hl_mnetssd_interface.num_inputs                          = 2U;
    cvtask_hl_mnetssd_interface.num_outputs                         = 1U;
    cvtask_hl_mnetssd_interface.output[0].buffer_size = (UINT32)sizeof(AMBA_OD_s);

    AmbaPrint_PrintStr5("query cvtask [%s]", CVTASK_NAME, NULL, NULL, NULL, NULL);

    if ((config == NULL) || (dst == NULL)) {
        retcode = CVTASK_ERR_NULL_POINTER;
    } else {
        for (UINT32 OutIdx = 0U; OutIdx < 2U; OutIdx ++) {
            label_in[16] = '0' + (((config[0] + OutIdx) / 100U) % 10U);
            label_in[17] = '0' + (((config[0] + OutIdx) /  10U) % 10U);
            label_in[18] = '0' + (((config[0] + OutIdx)       ) % 10U);

            res = cvtable_find(label_in, &ioname, &str_size);
            AmbaMisra_TypeCast(&str, &ioname);
            if (res != (UINT32)CVTASK_ERR_OK) {

                AmbaPrint_PrintStr5("can't find label [%s]", label_in, NULL, NULL, NULL, NULL);
                retcode = (UINT32)CVTASK_ERR_GENERAL;
                break;
            } else {
                AmbaUtility_StringCopy(cvtask_hl_mnetssd_interface.input[OutIdx].io_name, ((size_t)str_size + (size_t)1U), str);
                cvtask_hl_mnetssd_interface.input[OutIdx].history_needed        = 0U;
            }
        }

        for (UINT32 OutIdx = 0; OutIdx < 1U; OutIdx++) {
            label_out[17] = '0' + (((config[0] + OutIdx) / 100U) % 10U);
            label_out[18] = '0' + (((config[0] + OutIdx) /  10U) % 10U);
            label_out[19] = '0' + (((config[0] + OutIdx)       ) % 10U);
            res = cvtable_find(label_out, &ioname, &str_size);
            AmbaMisra_TypeCast(&str, &ioname);
            if (res != (UINT32)CVTASK_ERR_OK) {
                AmbaPrint_PrintStr5("can't find label %s", label_out, NULL, NULL, NULL, NULL);
                retcode = (UINT32)CVTASK_ERR_GENERAL;
                break;
            } else {
                AmbaUtility_StringCopy(cvtask_hl_mnetssd_interface.output[OutIdx].io_name, ((size_t)str_size + (size_t)1U), str);
            }
        }

        if (retcode != CVTASK_ERR_GENERAL) {
#if defined(CONFIG_QNX) || defined(CONFIG_LINUX)
#if defined (CONFIG_BSP_CV2FSDK_OPTION_A_V110) || defined (CONFIG_BSP_CV28DK_8LAYER_A_V100)
            retcode = get_file_length("d:\\flexidag_mnetssd_sensor/mbox_priorbox.bin", &pb_len);
#else
            retcode = get_file_length("c:\\flexidag_mnetssd_sensor/mbox_priorbox.bin", &pb_len);
#endif
#else
            retcode = get_file_length("./mbox_priorbox.bin", &pb_len);
#endif
            /* / 2(pb and var) / sizeof(float32) / 4(x1, x2, y1, y2) */
            total_bbox = pb_len >> 5;

            /* Set up base task memory */
            *dst = cvtask_hl_mnetssd_interface;  /* Copy data */
            dst->Instance_private_storage_needed = (UINT32)sizeof(hl_mnetssd_private_t) + pb_len +
                                                   (((total_bbox * 4U * 2U) + (total_bbox * (UINT32)num_cls)
                                                           + (total_bbox * 5U)) * ((UINT32)sizeof(FLOAT))) + (UINT32)sizeof(cls_name) + (UINT32)sizeof(*g_pcls_name);
        }
    }

    return retcode;
}

static uint32_t cvtask_hl_mnetssd_init(const cvtask_parameter_interface_t *env,
                                       const uint32_t *config)
{
    UINT32 rval;
    ULONG g_addr, addrtmp;
    hl_mnetssd_private_t *priv;

    (void) env;
    (void) config;
    (void) cls_name;

    AmbaPrint_PrintStr5("init cvtask [%s]", CVTASK_NAME, NULL, NULL, NULL, NULL);
    AmbaMisra_TypeCast(&priv, &env->vpInstance_private_storage);
#if defined(CONFIG_QNX) || defined(CONFIG_LINUX)
#if defined (CONFIG_BSP_CV2FSDK_OPTION_A_V110) || defined (CONFIG_BSP_CV28DK_8LAYER_A_V100)
    rval = get_file_length("d:\\flexidag_mnetssd_sensor/mbox_priorbox.bin", &priv->pb_len);
#else
    rval = get_file_length("c:\\flexidag_mnetssd_sensor/mbox_priorbox.bin", &priv->pb_len);
#endif
#else
    rval = get_file_length("./mbox_priorbox.bin", &priv->pb_len);
#endif
    priv->total_bbox = priv->pb_len >> 5;
    /*
    g_prior_box     = (FLOAT *) env->vpInstance_private_storage;
    g_detect_box    = (FLOAT *) ((UINT32) g_prior_box + pb_len);
    g_decode_box    = (FLOAT *) ((UINT32) g_detect_box + (total_bbox * sizeof(FLOAT) * 4));
    g_conf          = (FLOAT *) ((UINT32) g_decode_box + (total_bbox * sizeof(FLOAT) * 4));
    g_nms_box       = (FLOAT *) ((UINT32) g_conf + (total_bbox * sizeof(FLOAT) * num_cls));
    */
    AmbaMisra_TypeCast(&g_addr, &env->vpInstance_private_storage);
    addrtmp = g_addr + sizeof(hl_mnetssd_private_t);
    AmbaMisra_TypeCast(&priv->g_prior_box, &addrtmp);

    AmbaMisra_TypeCast(&g_addr, &priv->g_prior_box);
    addrtmp = g_addr + priv->pb_len;
    AmbaMisra_TypeCast(&priv->g_detect_box, &addrtmp);

    AmbaMisra_TypeCast(&g_addr, &priv->g_detect_box);
    addrtmp = g_addr + (priv->total_bbox * sizeof(FLOAT) * 4U);
    AmbaMisra_TypeCast(&priv->g_decode_box, &addrtmp);

    AmbaMisra_TypeCast(&g_addr, &priv->g_decode_box);
    addrtmp = g_addr + (priv->total_bbox * sizeof(FLOAT) * 4U);
    AmbaMisra_TypeCast(&priv->g_conf, &addrtmp);

    AmbaMisra_TypeCast(&g_addr, &priv->g_conf);
    addrtmp = g_addr + (priv->total_bbox * sizeof(FLOAT) * (UINT32)num_cls);
    AmbaMisra_TypeCast(&priv->g_nms_box, &addrtmp);

    AmbaMisra_TypeCast(&g_addr, &priv->g_nms_box);
    addrtmp = g_addr + (priv->total_bbox * sizeof(FLOAT) * 5U);
    AmbaMisra_TypeCast(&g_cls_name, &addrtmp);

    AmbaMisra_TypeCast(&g_addr, &g_cls_name);
    addrtmp = g_addr + (sizeof(cls_name));
    AmbaMisra_TypeCast(&g_pcls_name, &addrtmp);

    /* load prior box from file */
#if defined(CONFIG_QNX) || defined(CONFIG_LINUX)
#if defined (CONFIG_BSP_CV2FSDK_OPTION_A_V110) || defined (CONFIG_BSP_CV28DK_8LAYER_A_V100)
    rval = load_binary("d:\\flexidag_mnetssd_sensor/mbox_priorbox.bin", priv->g_prior_box, priv->pb_len);
#else
    rval = load_binary("c:\\flexidag_mnetssd_sensor/mbox_priorbox.bin", priv->g_prior_box, priv->pb_len);
#endif
#else
    rval = load_binary("./mbox_priorbox.bin", priv->g_prior_box, priv->pb_len);
#endif

    return rval;
}

static uint32_t cvtask_hl_mnetssd_process_messages(const cvtask_parameter_interface_t *pCVTaskParams)
{
    UINT32 retcode = CVTASK_ERR_OK;
    UINT32 msg_count = pCVTaskParams->cvtask_num_messages;
    hl_win_ctx_t *p_ctx;
    const hl_win_ctx_t *p_src_ctx = NULL;
    hl_mnetssd_private_t *priv;
    const void *dst;

    AmbaMisra_TypeCast(&priv, &pCVTaskParams->vpInstance_private_storage);
    p_ctx = &priv->mnetssd_ctx;
    if (p_ctx->raw_w == 0U) {
        if (msg_count != 1U) {
            retcode = CVTASK_ERR_GENERAL;
        } else {
            AmbaMisra_TypeCast(&p_src_ctx, &pCVTaskParams->vpMessagePayload[0]);
            dst = cvtask_memcpy(p_ctx, p_src_ctx, sizeof(hl_win_ctx_t));
            if(dst == NULL) {
                AmbaPrint_PrintUInt5("cvtask_hl_mnetssd_process_messages cvtask_memcpy error ", 0U, 0U, 0U, 0U, 0U);
            }
        }
    }

#if 0
    AmbaPrint_PrintUInt5("msg_count: %d, raw_w: %d", msg_count, p_ctx->raw_w, 0, 0, 0);
    AmbaPrint_PrintUInt5("raw: %dx%d, roi: %dx%d",
                         p_ctx->raw_w, p_ctx->raw_h, p_ctx->roi_w, p_ctx->roi_h, 0);
    AmbaPrint_PrintUInt5("roi_start: %d, %d, net_in: %dx%d",
                         p_ctx->roi_start_col, p_ctx->roi_start_row, p_ctx->net_in_w, p_ctx->net_in_h, 0);
#endif

    return retcode;
}

static uint32_t cvtask_hl_mnetssd_run(const cvtask_parameter_interface_t *env)
{
    INT32 i;
    UINT32 size;
    UINT32 retcode;
    UINT32 paddrtmp;
    ULONG vaddrtmp = 0U;
    void *voidtmp = NULL;
    hl_mnetssd_private_t *priv;
    const void *dst;
    const FLOAT *src_ptr = NULL;
    AMBA_OD_s *od_dst;

    (void) voidtmp;
    //AmbaPrint_PrintStr5("run cvtask [%s]", CVTASK_NAME, NULL, NULL, NULL, NULL);
    AmbaMisra_TypeCast(&priv, &env->vpInstance_private_storage);

    retcode = cvtask_hl_mnetssd_process_messages(env);

    /* reset message information */
    dst = cvtask_memset(&priv->msg2memio, 0, sizeof(AMBA_OD_s));
    if(dst == NULL) {
        AmbaPrint_PrintUInt5("cvtask_hl_mnetssd_run cvtask_memset  error ", 0U, 0U, 0U, 0U, 0U);
    }
    AmbaMisra_TypeCast(&voidtmp, &g_pcls_name);
    paddrtmp = (UINT32)ambacv_v2p(voidtmp);
    priv->msg2memio.Info.class_name = paddrtmp;

    /* Send crop window */
    send_bbx(priv, 255, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f);

    /* convert loc data format */
    size = priv->total_bbox * 4U * ((UINT32)sizeof(FLOAT));
    AmbaMisra_TypeCast(&src_ptr, &env->vpInputBuffer[0]);
    dst = cvtask_memcpy(priv->g_detect_box, src_ptr, size);
    if(dst == NULL) {
        AmbaPrint_PrintUInt5("cvtask_hl_mnetssd_run cvtask_memcpy 1 error ", 0U, 0U, 0U, 0U, 0U);
    }
    /* convert conf data format */
    size = priv->total_bbox * (UINT32)num_cls * ((UINT32)sizeof(FLOAT));
    AmbaMisra_TypeCast(&src_ptr, &env->vpInputBuffer[1]);
    dst = cvtask_memcpy(priv->g_conf, src_ptr, size);
    if(dst == NULL) {
        AmbaPrint_PrintUInt5("cvtask_hl_mnetssd_run cvtask_memcpy 2 error ", 0U, 0U, 0U, 0U, 0U);
    }
    /* bounding box regression */
    box_regression(priv);

    /* bounding box nms */
    for (i = 1; i < num_cls; i++) {
        box_nms(priv,(UINT8)i, OVERLAP_THRESHOLD);
    }

    if ((priv->rounding == 0x10001U) && (env->cvtask_frameset_id == 0xffffU)) {
        priv->msg2memio.Info.capture_time    = env->frameset_basetime;
        priv->msg2memio.Info.frame_num       = (0xffffU * priv->rounding) + env->cvtask_frameset_id;
        priv->rounding = 0;
    } else {
        priv->msg2memio.Info.capture_time    = env->frameset_basetime;
        priv->msg2memio.Info.frame_num       = (0xffffU * priv->rounding) + env->cvtask_frameset_id;
        if (priv->msg2memio.Info.frame_num == 0xffffU) {
            priv->rounding += 1U;
        }
    }

    /* Send class name */
    dst = cvtask_memcpy(g_cls_name, &cls_name, sizeof(cls_name));
    if(dst == NULL) {
        AmbaPrint_PrintUInt5("cvtask_hl_mnetssd_run cvtask_memcpy 3 error ", 0U, 0U, 0U, 0U, 0U);
    }
    for (i = 0; i < num_cls; i++) {
        AmbaMisra_TypeCast(&vaddrtmp, &g_cls_name);
        vaddrtmp = vaddrtmp + ((ULONG)i * (ULONG)sizeof(cls_name[0]));
        AmbaMisra_TypeCast(&voidtmp, &vaddrtmp);
        (*g_pcls_name)[i] = (ULONG)ambacv_v2p(voidtmp);
    }

    /* send to camctrl */
    AmbaMisra_TypeCast(&od_dst, &env->vpOutputBuffer[0]);
    dst = cvtask_memcpy(od_dst, &priv->msg2memio, sizeof(priv->msg2memio));
    if(dst == NULL) {
        AmbaPrint_PrintUInt5("cvtask_hl_mnetssd_run cvtask_memcpy 4 error ", 0U, 0U, 0U, 0U, 0U);
    }
    return retcode;
}

uint32_t cvtask_hl_mnetssd_create(void)
{
    UINT32 ret = 0U;
    static const cvtask_entry_t entry = {
        .cvtask_name                = CVTASK_NAME,
        .cvtask_type                = (UINT32) CVTASK_TYPE_ARM,
        .cvtask_api_version         = CVTASK_API_VERSION,
        .cvtask_query               = cvtask_hl_mnetssd_query,
        .cvtask_init                = cvtask_hl_mnetssd_init,
        .cvtask_run                 = cvtask_hl_mnetssd_run,
        .cvtask_process_messages    = cvtask_hl_mnetssd_process_messages,
    };

    // register ourself to the framework so it knows our existence
    ret = cvtask_register(&entry, CVTASK_API_VERSION);
    (void) ret;
    return CVTASK_ERR_OK;
}

#if defined(CONFIG_QNX) || defined(CONFIG_LINUX)
cvtask_declare_create (cvtask_hl_mnetssd_create);
#else
static  initcall_t initcall_cvtask_hl_mnetssd_create
GNU_SECTION_CVTASK_CREATE_FP = cvtask_hl_mnetssd_create;
#endif
