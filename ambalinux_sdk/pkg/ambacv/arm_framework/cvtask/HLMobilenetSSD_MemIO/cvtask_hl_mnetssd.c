/*
* Copyright (c) 2017-2017 Ambarella, Inc.
*
* This file and its contents ("Software") are protected by intellectual property rights including,
* without limitation, U.S. and/or foreign copyrights.  This Software is also the confidential and
* proprietary information of Ambarella, Inc. and its licensors.  You may not use, reproduce, disclose,
* distribute, modify, or otherwise prepare derivative works of this Software or any portion thereof
* except pursuant to a signed license agreement or nondisclosure agreement with Ambarella, Inc. or
* its authorized affiliates.  In the absence of such an agreement, you agree to promptly notify and
* return this Software to Ambarella, Inc.
*
* THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
* TO, THE IMPLIED WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE
* ARE DISCLAIMED. IN NO EVENT SHALL AMBARELLA, INC. OR ITS AFFILIATES BE LIABLE FOR ANY DIRECT, INDIRECT,
* INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
* OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; COMPUTER FAILURE OR MALFUNCTION; OR
* BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
* LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
* SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#ifndef CONFIG_CV_CONFIG_TX_SDK7
#include <time.h>
#include <math.h>
#include <time.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>
#include <rtos/AmbaRTOSWrapper.h>

#ifdef THREADX_BUILD
#include <AmbaDataType.h>
#include <AmbaPrint.h>
#include <intrinsics.h>
#include <AmbaFS.h>
#define FILE            AMBA_FS_FILE
#define fopen           AmbaFS_fopen
#define fclose          AmbaFS_fclose
#define fread           AmbaFS_fread
#define fwrite          AmbaFS_fwrite
#define ftell           AmbaFS_ftell
#define fseek           AmbaFS_fseek
#define exit(x)         return CVTASK_ERR_GENERAL
#endif

#else
#include "AmbaWrap.h"
#include "cvtask_txsrv.h"
#endif

#include "cvtask_api.h"
#include "cvtask_errno.h"
#include "cvapi_amba_od_interface.h"


#define CVTASK_NAME         "HL_MNETSSD_MEMIO"

#define NUM_CLS             7
#define CONF_THRESHOLD      0.4f
#define OVERLAP_THRESHOLD   0.45f

#define EXP                 2.71828182845904

#define AMBAOD_MAX_BBX_NUM  200
#ifndef CONFIG_CV_CONFIG_TX_SDK7
#define AmbaMisra_TypeCast    AmbaMisra_TypeCast64
#else
#define AmbaMisra_TypeCast    AmbaMisra_TypeCast32
#endif


typedef struct {
    uint32_t raw_w;
    uint32_t raw_h;

    uint32_t roi_w;
    uint32_t roi_h;

    uint32_t roi_start_col;
    uint32_t roi_start_row;

    uint32_t net_in_w;
    uint32_t net_in_h;
} win_ctx_t;

typedef struct{
    amba_od_out_t Info;
    amba_od_candidate_t Bbx[AMBAOD_MAX_BBX_NUM];
} AMBA_OD_s;


typedef struct {
    uint32_t total_bbox;
    uint32_t pb_len;
    uint32_t round;
    AMBA_OD_s msg2memio;
    win_ctx_t mnetssd_ctx;
    FLOAT *g_prior_box;
    FLOAT *g_conf;
    FLOAT *g_detect_box;
    FLOAT *g_decode_box;
    FLOAT *g_nms_box;
    char *g_cls_name;
    uint64_t (*g_pcls_name)[NUM_CLS];
} hl_mnetssd_private_t;

static int32_t num_cls = NUM_CLS;
static const char cls_name[NUM_CLS][16] = {
    "background",
    "bicycle",
    "bus",
    "car",
    "motorbike",
    "person",
    "train",
};


extern uint64_t ambacv_v2p(void *va);
extern uint32_t cvtask_hl_mnetssd_memio_create(void);

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

static inline uint16_t cal_pos(FLOAT minmax, uint32_t net_in,
                        uint32_t roi_start, uint32_t raw, uint32_t roi)
{
    FLOAT fltmp;
    uint16_t u16tmp;

    /* (((min, max) * net_in) + roi_start) * raw / roi; */

    fltmp = (FLOAT) net_in;

    fltmp *= minmax;

    fltmp += (FLOAT) roi_start;

    fltmp *= (FLOAT) raw;

    fltmp /= (FLOAT) roi;

    u16tmp = (uint16_t) fltmp;

    return u16tmp;
}

static inline uint16_t cal_wh(FLOAT max, FLOAT min, uint32_t net_in,
                            uint32_t roi, uint32_t raw)
{
    FLOAT fltmp;
    uint16_t u16tmp;

    /* (MAX(x_min, x_max) - MIN(x_min, x_max)) * net_in_w / roi_w * raw_w; */
    fltmp = max - min;

    fltmp *= (FLOAT) net_in;

    fltmp /= (FLOAT) roi;

    fltmp *= (FLOAT) raw;

    u16tmp = (uint16_t) fltmp;

    return u16tmp;
}

static void send_bbx(hl_mnetssd_private_t *priv, uint8_t target_class, FLOAT x_min, FLOAT y_min,
        FLOAT x_max, FLOAT y_max, FLOAT score)
{
    const win_ctx_t *p_ctx = &priv->mnetssd_ctx;

    uint32_t raw_w          = p_ctx->raw_w;
    uint32_t raw_h          = p_ctx->raw_h;
    uint32_t roi_w          = p_ctx->roi_w;
    uint32_t roi_h          = p_ctx->roi_h;
    uint32_t roi_start_col  = p_ctx->roi_start_col;
    uint32_t roi_start_row  = p_ctx->roi_start_row;
    uint32_t net_in_w       = p_ctx->net_in_w;
    uint32_t net_in_h       = p_ctx->net_in_h;
    FLOAT fltmp;
    uint32_t u32tmp;

    if (raw_w != 0U) {
        priv->msg2memio.Info.capture_time = 0;
        priv->msg2memio.Info.frame_num = 0;
        priv->msg2memio.Info.objects_offset = sizeof(amba_od_out_t);
        priv->msg2memio.Bbx[priv->msg2memio.Info.num_objects].field = 0;
        priv->msg2memio.Bbx[priv->msg2memio.Info.num_objects].clsId = target_class;
        priv->msg2memio.Bbx[priv->msg2memio.Info.num_objects].bb_start_col =
            (int32_t)cal_pos(MIN(x_min, x_max), net_in_w, roi_start_col, raw_w, roi_w);
        priv->msg2memio.Bbx[priv->msg2memio.Info.num_objects].bb_start_row =
            (int32_t)cal_pos(MIN(y_min, y_max), net_in_h, roi_start_row, raw_h, roi_h);
        priv->msg2memio.Bbx[priv->msg2memio.Info.num_objects].bb_width_m1 =
            (uint32_t)cal_wh(MAX(x_min, x_max), MIN(x_min, x_max), net_in_w, roi_w, raw_w) - 1U;
        priv->msg2memio.Bbx[priv->msg2memio.Info.num_objects].bb_height_m1 =
            (uint32_t)cal_wh(MAX(y_min, y_max), MIN(y_min, y_max), net_in_h, roi_h, raw_h) - 1U;
        fltmp = score * 255.0f;
        u32tmp = (uint32_t) fltmp;
        priv->msg2memio.Bbx[priv->msg2memio.Info.num_objects].score = u32tmp;
        priv->msg2memio.Info.num_objects = priv->msg2memio.Info.num_objects + 1U;
    }
}

static uint32_t load_binary(const char *filename, FLOAT *dst, uint32_t byte_size)
{
    FILE_WRAP *ifp;
    uint32_t rval = CVTASK_ERR_OK;
    uint32_t count;

    ifp = fopen_wrap(filename, "rb");

    if (ifp == NULL) {
        AmbaPrint_PrintStr5("Can't load file %s!", filename, NULL, NULL, NULL, NULL);
        rval = CVTASK_ERR_NULL_POINTER;
    } else {
        count = fread_wrap(dst, 1, byte_size, ifp);
        if (count == 0U){
            rval = CVTASK_ERR_GENERAL;
        }
        (void) fclose_wrap(ifp);
    }

    return rval;
}

static uint32_t get_file_length(const char *filename, uint32_t *pb_len)
{
    FILE_WRAP *ifp;
    uint32_t rval = CVTASK_ERR_OK;
    uint64_t pb_len_u64;

    ifp = fopen_wrap(filename, "rb");

    if (ifp == NULL) {
        AmbaPrint_PrintStr5("Can't load file %s!", filename, NULL, NULL, NULL, NULL);
        rval = CVTASK_ERR_NULL_POINTER;
    } else {
        rval = fseek_wrap(ifp, 0, SEEK_END_WRAP);

        if (rval != 0U){
            rval = CVTASK_ERR_GENERAL;
        } else {
            pb_len_u64 = ftell_wrap(ifp);
            *pb_len = (uint32_t)(pb_len_u64 & 0xFFFFFFFFU);
        }
        (void) fclose_wrap(ifp);
    }

    return rval;
}

static int32_t check_threshold(const FLOAT *softmax)
{
    int32_t i, rval = 0;

    for (i = 1; i < num_cls; i++) {
        if (softmax[i] >= CONF_THRESHOLD){
            rval = 1;
            break;
        }
    }

    return rval;
}

static void box_regression(hl_mnetssd_private_t *priv)
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
    uint32_t i, ix4, count = 0;
    uint64_t g_prior_box, addrtmp;
#ifdef CONFIG_CV_CONFIG_TX_SDK7
    FLOAT fltmp;
    DOUBLE exp, temp;
#endif

    AmbaMisra_TypeCast(&g_prior_box, &priv->g_prior_box);
    addrtmp = g_prior_box + (priv->pb_len >> 1);
    AmbaMisra_TypeCast(&var, &addrtmp);

    for (i = 0; i < priv->total_bbox; i++) {
        if (check_threshold(&softmax[i * (uint32_t)num_cls]) != 0) {
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

#ifndef CONFIG_CV_CONFIG_TX_SDK7
            det_w = exp(var[ix4 + 2U] * det_x_max) * prior_w;
            det_h = exp(var[ix4 + 3U] * det_y_max) * prior_h;
#else
            fltmp = var[ix4 + 2U] * det_x_max;
            exp = (DOUBLE) fltmp;
            (void)AmbaWrap_pow(EXP, exp, &temp);
            det_w = (FLOAT) temp;
            det_w *= prior_w;

            fltmp = var[ix4 + 3U] * det_y_max;
            exp = (DOUBLE) fltmp;
            (void)AmbaWrap_pow(EXP, exp, &temp);
            det_h = (FLOAT) temp;
            det_h *= prior_h;
#endif

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
    }
    //AmbaPrint_PrintUInt5("filtered boxes: %d", count, 0, 0, 0, 0);
    AmbaMisra_TouchUnused(&count);
}

static void swap_row_float(FLOAT *input, uint32_t row_x, uint32_t row_y)
{
    FLOAT temp[5] = { 0.0f };

    memcpy(temp, &input[row_x * 5U], sizeof(FLOAT) * 5U);
    memcpy(&input[row_x * 5U], &input[row_y * 5U], sizeof(FLOAT) * 5U);
    memcpy(&input[row_y * 5U], temp, sizeof(FLOAT) * 5U);
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

static void box_nms(hl_mnetssd_private_t *priv, uint8_t target_class, FLOAT threshold)
{
    FLOAT *nms_box = priv->g_nms_box;
    const FLOAT *conf = priv->g_conf;
    const FLOAT *decbox = priv->g_decode_box;
    uint32_t i, j, nms_number = 0, ix5, jx5;
    FLOAT overlap;

    memset(nms_box, 0, priv->total_bbox * 5U * sizeof(FLOAT));

    /* copy valid data */
    for (i = 0; i < priv->total_bbox; i++) {
        if (conf[(i * (uint32_t)num_cls) + target_class] >= CONF_THRESHOLD) {
            memcpy(&nms_box[nms_number * 5U], &decbox[i * 4U],
                    sizeof(FLOAT) * 4U);
            nms_box[(nms_number * 5U) + 4U] = conf[(i * (uint32_t)num_cls) + target_class];
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
                    memset(&nms_box[jx5], 0, sizeof(FLOAT) * 5U);
                }
            }
        }
    }
}

static uint32_t cvtask_hl_mnetssd_query(uint32_t iid, const uint32_t *config,
                      cvtask_memory_interface_t *dst)
{
    cvtask_memory_interface_t cvtask_hl_mnetssd_interface;
    uint32_t retcode;
    char label_in[32] = "HL_IN_00_IONAME_###", label_out[32] = "HL_OUT_00_IONAME_###";
    const void *ioname;
    const char *str;
    uint32_t str_size, res;
    uint32_t total_bbox;
    uint32_t pb_len = 0U;

    (void) iid;

    retcode = CVTASK_ERR_OK;

    memset(&cvtask_hl_mnetssd_interface, 0, sizeof(cvtask_hl_mnetssd_interface));
    cvtask_hl_mnetssd_interface.Instance_private_storage_needed     = 0U;
    cvtask_hl_mnetssd_interface.DRAM_temporary_scratchpad_needed    = 0U;
    cvtask_hl_mnetssd_interface.num_inputs                          = 2U;
    cvtask_hl_mnetssd_interface.num_outputs                         = 1U;
    cvtask_hl_mnetssd_interface.output[0].buffer_size = sizeof(AMBA_OD_s);

    AmbaPrint_PrintStr5("query cvtask [%s]", CVTASK_NAME, NULL, NULL, NULL, NULL);

    if ((config == NULL) || (dst == NULL)) {
        retcode = CVTASK_ERR_NULL_POINTER;
    } else {
        for (uint32_t OutIdx = 0U; OutIdx < 2U; OutIdx ++) {
            label_in[16] = '0' + (((config[0] + OutIdx) / 100U) % 10U);
            label_in[17] = '0' + (((config[0] + OutIdx) /  10U) % 10U);
            label_in[18] = '0' + (((config[0] + OutIdx)       ) % 10U);

            res = cvtable_find(label_in, &ioname, &str_size);
            AmbaMisra_TypeCast(&str, &ioname);
            if (res != (uint32_t)CVTASK_ERR_OK) {

                AmbaPrint_PrintStr5("can't find label [%s]", label_in, NULL, NULL, NULL, NULL);
                retcode = (uint32_t)CVTASK_ERR_GENERAL;
                break;
            } else {
                AmbaUtility_StringCopy(cvtask_hl_mnetssd_interface.input[OutIdx].io_name, str_size + 1U, str);
                cvtask_hl_mnetssd_interface.input[OutIdx].history_needed        = 0U;
            }
        }

        for (uint32_t OutIdx = 0; OutIdx < 1U; OutIdx++) {
            label_out[17] = '0' + (((config[0] + OutIdx) / 100U) % 10U);
            label_out[18] = '0' + (((config[0] + OutIdx) /  10U) % 10U);
            label_out[19] = '0' + (((config[0] + OutIdx)       ) % 10U);
            res = cvtable_find(label_out, &ioname, &str_size);
            AmbaMisra_TypeCast(&str, &ioname);
            if (res != (uint32_t)CVTASK_ERR_OK){
                AmbaPrint_PrintStr5("can't find label %s", label_out, NULL, NULL, NULL, NULL);
                retcode = (uint32_t)CVTASK_ERR_GENERAL;
                break;
            } else {
                AmbaUtility_StringCopy(cvtask_hl_mnetssd_interface.output[OutIdx].io_name, str_size + 1U, str);
            }
        }

        if (retcode != CVTASK_ERR_GENERAL) {
            retcode = get_file_length("./mbox_priorbox.bin", &pb_len);

            /* / 2(pb and var) / sizeof(float32) / 4(x1, x2, y1, y2) */
            total_bbox = pb_len >> 5;

            /* Set up base task memory */
            *dst = cvtask_hl_mnetssd_interface;  /* Copy data */
            dst->Instance_private_storage_needed = sizeof(hl_mnetssd_private_t) + pb_len +
                    (((total_bbox * 4U * 2U) + (total_bbox * (uint32_t)num_cls)
                      + (total_bbox * 5U)) * sizeof(FLOAT)) + sizeof(cls_name) + (sizeof(uint64_t) * (uint32_t)num_cls);
        }
    }

    return retcode;
}

static uint32_t cvtask_hl_mnetssd_init(const cvtask_parameter_interface_t *env,
                     const uint32_t *config)
{
    uint32_t rval;
    uint64_t g_addr, addrtmp;
    hl_mnetssd_private_t *priv;

    (void) env;
    (void) config;
    (void) cls_name;

    AmbaPrint_PrintStr5("init cvtask [%s]", CVTASK_NAME, NULL, NULL, NULL, NULL);
    AmbaMisra_TypeCast(&priv, &env->vpInstance_private_storage);
    get_file_length("./mbox_priorbox.bin", &priv->pb_len);
    priv->total_bbox = priv->pb_len >> 5;
    /*
    g_prior_box     = (FLOAT *) env->vpInstance_private_storage;
    g_detect_box    = (FLOAT *) ((uint32_t) g_prior_box + pb_len);
    g_decode_box    = (FLOAT *) ((uint32_t) g_detect_box + (total_bbox * sizeof(FLOAT) * 4));
    g_conf          = (FLOAT *) ((uint32_t) g_decode_box + (total_bbox * sizeof(FLOAT) * 4));
    g_nms_box       = (FLOAT *) ((uint32_t) g_conf + (total_bbox * sizeof(FLOAT) * num_cls));
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
    addrtmp = g_addr + (priv->total_bbox * sizeof(FLOAT) * (uint32_t)num_cls);
    AmbaMisra_TypeCast(&priv->g_nms_box, &addrtmp);

    AmbaMisra_TypeCast(&g_addr, &priv->g_nms_box);
    addrtmp = g_addr + (priv->total_bbox * sizeof(FLOAT) * 5U);
    AmbaMisra_TypeCast(&priv->g_cls_name, &addrtmp);

    AmbaMisra_TypeCast(&g_addr, &priv->g_cls_name);
    addrtmp = g_addr + (sizeof(cls_name));
    AmbaMisra_TypeCast(&priv->g_pcls_name, &addrtmp);

    /* load prior box from file */
    rval = load_binary("./mbox_priorbox.bin", priv->g_prior_box, priv->pb_len);

    return rval;
}

static uint32_t cvtask_hl_mnetssd_process_messages(const cvtask_parameter_interface_t *pCVTaskParams)
{
    uint32_t retcode = CVTASK_ERR_OK;
    uint32_t msg_count = pCVTaskParams->cvtask_num_messages;
    win_ctx_t *p_ctx;
    hl_mnetssd_private_t *priv;

    AmbaMisra_TypeCast(&priv, &pCVTaskParams->vpInstance_private_storage);
    p_ctx = &priv->mnetssd_ctx;
    if (p_ctx->raw_w == 0U) {
        if (msg_count != 1U) {
            AmbaPrint_PrintUInt5("cvtask_hl_mnetssd_process_messages: msg_count: %d", msg_count, 0U, 0U, 0U, 0U);
            retcode = CVTASK_ERR_GENERAL;
        } else {
            memcpy(p_ctx, pCVTaskParams->vpMessagePayload[0], sizeof(win_ctx_t));
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
    int32_t i;
    uint32_t size;
    uint32_t retcode;
    uint32_t paddrtmp;
    uint64_t vaddrtmp;
    void *voidtmp;
    hl_mnetssd_private_t *priv;

    //AmbaPrint_PrintStr5("run cvtask [%s]", CVTASK_NAME, NULL, NULL, NULL, NULL);
    AmbaMisra_TypeCast(&priv, &env->vpInstance_private_storage);

    retcode = cvtask_hl_mnetssd_process_messages(env);
    if (retcode != CVTASK_ERR_OK) {
        return retcode;
    }

    /* reset message information */
    memset(&priv->msg2memio, 0, sizeof(AMBA_OD_s));
    AmbaMisra_TypeCast(&voidtmp, &priv->g_pcls_name);
    paddrtmp = ambacv_v2p(voidtmp);
    AmbaMisra_TypeCast32(&priv->msg2memio.Info.class_name, &paddrtmp);

    /* Send crop window */
    send_bbx(priv, 255, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f);

    /* convert loc data format */
    size = priv->total_bbox * 4U * sizeof(FLOAT);
    memcpy(priv->g_detect_box, env->vpInputBuffer[0], size);

    /* convert conf data format */
    size = priv->total_bbox * (uint32_t)num_cls * sizeof(FLOAT);
    memcpy(priv->g_conf, env->vpInputBuffer[1], size);

    /* bounding box regression */
    box_regression(priv);

    /* bounding box nms */
    for (i = 1; i < num_cls; i++) {
        box_nms(priv,(uint8_t)i, OVERLAP_THRESHOLD);
    }

    if ((priv->round == 0x10001U) && (env->cvtask_frameset_id == 0xffffU)) {
        priv->msg2memio.Info.capture_time    = env->frameset_basetime;
        priv->msg2memio.Info.frame_num       = (0xffffU * priv->round) + env->cvtask_frameset_id;
        priv->round = 0;
    } else {
        priv->msg2memio.Info.capture_time    = env->frameset_basetime;
        priv->msg2memio.Info.frame_num       = (0xffffU * priv->round) + env->cvtask_frameset_id;
        if (priv->msg2memio.Info.frame_num == 0xffffU) {
            priv->round += 1U;
        }
    }

    /* Send class name */
    memcpy(priv->g_cls_name, &cls_name, sizeof(cls_name));
    for (i = 0; i < num_cls; i++) {
        AmbaMisra_TypeCast(&vaddrtmp, &priv->g_cls_name);
        vaddrtmp = vaddrtmp + ((uint64_t)i * (uint64_t)sizeof(cls_name[0]));
        AmbaMisra_TypeCast(&voidtmp, &vaddrtmp);
        (*(priv->g_pcls_name))[i] = ambacv_v2p(voidtmp);
    }

    /* send to camctrl */
    memcpy(env->vpOutputBuffer[0], &priv->msg2memio, sizeof(priv->msg2memio));

    return retcode;
}

static uint32_t cvtask_hl_mnetssd_get_info(const cvtask_parameter_interface_t *env,
                     uint32_t info_id, void *info_data)
{
    (void) env;
    (void) info_id;
    (void) info_data;

    return CVTASK_ERR_OK;
}

static uint32_t cvtask_hl_mnetssd_finish(const cvtask_parameter_interface_t *env)
{
    (void) env;

    return CVTASK_ERR_OK;
}

uint32_t cvtask_hl_mnetssd_memio_create(void)
{
    static const cvtask_entry_t entry = {
        .cvtask_name                = CVTASK_NAME,
        .cvtask_type                = (uint32_t) CVTASK_TYPE_ARM,
        .cvtask_api_version         = CVTASK_API_VERSION,
        .cvtask_query               = cvtask_hl_mnetssd_query,
        .cvtask_init                = cvtask_hl_mnetssd_init,
        .cvtask_get_info            = cvtask_hl_mnetssd_get_info,
        .cvtask_run                 = cvtask_hl_mnetssd_run,
        .cvtask_process_messages    = cvtask_hl_mnetssd_process_messages,
        .cvtask_finish              = cvtask_hl_mnetssd_finish,
    };

    // register ourself to the framework so it knows our existence
    (void) cvtask_register(&entry, CVTASK_API_VERSION);
    return CVTASK_ERR_OK;
}

cvtask_declare_create (cvtask_hl_mnetssd_memio_create);
