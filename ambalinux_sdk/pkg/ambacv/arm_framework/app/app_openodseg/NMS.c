#include <stdio.h>
#include <math.h>
#include "cvtask_api.h"
#include "cvtask_ossrv.h"
#include "rtos/AmbaRTOSWrapper.h"
#include "NMS.h"


// #define DBG_LOG     AmbaPrint_PrintUInt5
// #define CRI_LOG     AmbaPrint_PrintUInt5

#define OBJ_THR         (0.1f)
#define CONF_THR        (0.4f)
#define OVERLAP_THR     (0.45f)
#define EXP             (2.71828182845904)

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

typedef struct {
    FLOAT *g_prior_box;
    UINT32 total_bbox;
    UINT32 pb_len;
    UINT32 max_idx;
    UINT32 num_dec_bbx;

    nms_dex_bbx_t *G_NmsBox;
    amba_od_candidate_t *ODCandidates;
    UINT32 ODBoxNum;
    win_ctx_t Conf;
} NMS_hndlr_s;

static UINT32 AmbaWrap_pow(DOUBLE base, DOUBLE exponent, void *pV)
{
    UINT32 err = 0;
    DOUBLE v;

    if (pV == NULL) {
        err = 1;
    } else {
        v = pow(base, exponent);
        memcpy(pV, &v, sizeof(v));
        err = 0;
    }

    return err;
}

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
    UINT32 i;
    FLOAT max = 0.0f;

    for (i = 0; i < len; i++) {
        if (cls[i] > max) {
            max = cls[i];
            *pos = i;
        }
    }

    return max;
}

static inline UINT16 cal_pos(FLOAT minmax, UINT32 net_in,
                        UINT32 roi_start, UINT32 raw, UINT32 roi)
{
    FLOAT fltmp;
    UINT16 u16tmp;

    // (((min, max) * net_in) + roi_start) * raw / roi;

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

    // (MAX(x_min, x_max) - MIN(x_min, x_max)) * net_in_w / roi_w * raw_w;
    fltmp = max - min;

    fltmp *= (FLOAT) net_in;

    fltmp /= (FLOAT) roi;

    fltmp *= (FLOAT) raw;

    u16tmp = (UINT16) fltmp;

    return u16tmp;
}

static UINT32 get_file_length(const char *filename, UINT32 *pb_len)
{
    UINT32 ret, len;
    FILE *ifp;

    ifp = fopen(filename, "rb");
    if (ifp == NULL) {
        AmbaPrint_PrintStr5("Can't load file %s!", filename, NULL, NULL, NULL, NULL);
    } else {
        ret = fseek(ifp, 0LL, SEEK_END);
        if (ret != 0U){
            AmbaPrint_PrintUInt5("fseek fail ", 0U, 0U, 0U, 0U, 0U);
        } else {
            len = ftell(ifp);
            *pb_len = len;
            fclose(ifp);
        }
    }
    return ret;
}

static void load_binary(const char *filename, FLOAT *dst, UINT32 byte_size)
{
    UINT32 ret;
    FILE *ifp;

    ifp = fopen(filename, "rb");
    if (ifp == NULL) {
        AmbaPrint_PrintStr5("Can't load file %s!", filename, NULL, NULL, NULL, NULL);
    } else {
        ret = fread(dst, 1, byte_size, ifp);
        if (ret != byte_size){
            AmbaPrint_PrintUInt5("fread fail: %d", ret, 0U, 0U, 0U, 0U);
        } else {
            AmbaPrint_PrintUInt5("Read %d bytes from prior_box bin", ret, 0, 0, 0, 0);
            fclose(ifp);
        }
    }
}

static void send_bbx(NMS_hndlr_s *NMS_hndlr, UINT32 target_class, FLOAT x_min, FLOAT y_min,
        FLOAT x_max, FLOAT y_max, FLOAT score)
{
    UINT32 raw_w          = NMS_hndlr->Conf.raw_w;
    UINT32 raw_h          = NMS_hndlr->Conf.raw_h;
    UINT32 roi_w          = NMS_hndlr->Conf.roi_w;
    UINT32 roi_h          = NMS_hndlr->Conf.roi_h;
    UINT32 roi_start_col  = NMS_hndlr->Conf.roi_start_col;
    UINT32 roi_start_row  = NMS_hndlr->Conf.roi_start_row;
    UINT32 net_in_w       = NMS_hndlr->Conf.net_in_w;
    UINT32 net_in_h       = NMS_hndlr->Conf.net_in_h;
    FLOAT fltmp;
    UINT32 u32tmp;

    if (NMS_hndlr->ODBoxNum >= MAX_BBX_NUM) {
        AmbaPrint_PrintUInt5("BBX overflows (max = %d)!", MAX_BBX_NUM, 0, 0, 0, 0);
    } else if (raw_w == 0U) {
        AmbaPrint_PrintUInt5("Raw width is equal to 0!", 0, 0, 0, 0, 0);
    } else {
        NMS_hndlr->ODCandidates[NMS_hndlr->ODBoxNum].cls = target_class;

        NMS_hndlr->ODCandidates[NMS_hndlr->ODBoxNum].bb_start_col =
            (int32_t) cal_pos(MIN(x_min, x_max), net_in_w, roi_start_col,
                    raw_w, roi_w);
        NMS_hndlr->ODCandidates[NMS_hndlr->ODBoxNum].bb_start_row =
            (int32_t) cal_pos(MIN(y_min, y_max), net_in_h, roi_start_row,
                    raw_h, roi_h);

        NMS_hndlr->ODCandidates[NMS_hndlr->ODBoxNum].bb_width_m1 =
            (UINT32) cal_wh(MAX(x_min, x_max), MIN(x_min, x_max), net_in_w,
                    roi_w, raw_w) - 1U;
        NMS_hndlr->ODCandidates[NMS_hndlr->ODBoxNum].bb_height_m1 =
            (UINT32) cal_wh(MAX(y_min, y_max), MIN(y_min, y_max), net_in_h,
                    roi_h, raw_h) - 1U;

        fltmp = score * 255.0f;
        u32tmp = (UINT32) fltmp;
        NMS_hndlr->ODCandidates[NMS_hndlr->ODBoxNum].score = u32tmp;

        NMS_hndlr->ODBoxNum += 1U;
    }
}

static void bbox_regression(NMS_hndlr_s *NMS_hndlr, FLOAT *g_arm_conf, FLOAT *g_arm_loc, FLOAT *g_odm_conf, FLOAT *g_odm_loc)
{
    FLOAT *var;
    FLOAT *arm_conf = g_arm_conf;
    FLOAT *arm_loc = g_arm_loc;
    FLOAT *odm_conf = g_odm_conf;
    FLOAT *odm_loc = g_odm_loc;
    FLOAT *pbox = NMS_hndlr->g_prior_box;
    FLOAT decbox[4];
    nms_dex_bbx_t *nms_dec_box = NMS_hndlr->G_NmsBox;
    FLOAT prior_x_center, prior_y_center;
    FLOAT prior_w, prior_h;
    FLOAT dec_x_center, dec_y_center;
    FLOAT dec_w, dec_h;
    UINT32 i, ix4, count = 0, pos = 0, idx;
    UINT32 u32tmp;
    FLOAT fltmp, max;
    DOUBLE exp, temp;

    (void) AmbaWrap_memset(nms_dec_box, 0x0, NMS_hndlr->total_bbox * sizeof(nms_dex_bbx_t));
    NMS_hndlr->num_dec_bbx = 0;

    {
        char *pU8;

        u32tmp = NMS_hndlr->pb_len >> 1;
        pU8 = (char *)pbox;
        pU8 += u32tmp;
        var = (FLOAT *)pU8;
    }


    for (i = 0; i < NMS_hndlr->total_bbox; i++) {
        if (arm_conf[(i * 2U) + 1U] > OBJ_THR) {
            max = find_max(&odm_conf[i * NUM_CLS], NUM_CLS, &pos);
            if ((pos != 0U) && (max > CONF_THR)) {
                ix4 = i * 4U;
                // prior box
                prior_x_center = (pbox[ix4] + pbox[ix4 + 2U]) / 2.0f;
                prior_y_center = (pbox[ix4 + 1U] + pbox[ix4 + 3U]) / 2.0f;
                prior_w = pbox[ix4 + 2U] - pbox[ix4];
                prior_h = pbox[ix4 + 3U] - pbox[ix4 + 1U];

                // decode bbox
                dec_x_center = (var[ix4] * arm_loc[ix4] * prior_w) +
                                prior_x_center;
                dec_y_center = (var[ix4 + 1U] * arm_loc[ix4 + 1U] * prior_h) +
                                prior_y_center;

                fltmp = var[ix4 + 2U] * arm_loc[ix4 + 2U];
                exp = (DOUBLE) fltmp;
                (void)AmbaWrap_pow(EXP, exp, &temp);

                dec_w = (FLOAT) temp;
                dec_w *= prior_w;

                fltmp = var[ix4 + 3U] * arm_loc[ix4 + 3U];
                exp = (DOUBLE) fltmp;
                (void)AmbaWrap_pow(EXP, exp, &temp);
                dec_h = (FLOAT) temp;
                dec_h *= prior_h;

                // finetune bbox
                dec_x_center += (var[ix4] * odm_loc[ix4] * dec_w);
                dec_y_center += (var[ix4 + 1U] * odm_loc[ix4 + 1U] * dec_h);

                fltmp = var[ix4 + 2U] * odm_loc[ix4 + 2U];
                exp = (DOUBLE) fltmp;
                (void)AmbaWrap_pow(EXP, exp, &temp);
                dec_w *= (FLOAT) temp;

                fltmp = var[ix4 + 3U] * odm_loc[ix4 + 3U];
                exp = (DOUBLE) fltmp;
                (void)AmbaWrap_pow(EXP, exp, &temp);
                dec_h *= (FLOAT) temp;

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
                (void) AmbaWrap_memcpy(&nms_dec_box[NMS_hndlr->num_dec_bbx], decbox, sizeof(FLOAT) * 4U);
                nms_dec_box[NMS_hndlr->num_dec_bbx].cls   = pos;
                nms_dec_box[NMS_hndlr->num_dec_bbx].conf  = max;

                // Sort the bbx with conf by using linked list.
                if (NMS_hndlr->num_dec_bbx == 0U) {
                    nms_dec_box[NMS_hndlr->num_dec_bbx].prev_idx = NMS_hndlr->num_dec_bbx;
                    nms_dec_box[NMS_hndlr->num_dec_bbx].next_idx = NMS_hndlr->num_dec_bbx;
                    NMS_hndlr->max_idx = NMS_hndlr->num_dec_bbx;
                } else {
                    if (max > nms_dec_box[NMS_hndlr->max_idx].conf) {
                        nms_dec_box[NMS_hndlr->num_dec_bbx].prev_idx = NMS_hndlr->num_dec_bbx;
                        nms_dec_box[NMS_hndlr->num_dec_bbx].next_idx = NMS_hndlr->max_idx;
                        nms_dec_box[NMS_hndlr->max_idx].prev_idx = NMS_hndlr->num_dec_bbx;
                        NMS_hndlr->max_idx = NMS_hndlr->num_dec_bbx;
                    } else {
                        count = NMS_hndlr->num_dec_bbx;
                        idx = NMS_hndlr->max_idx;
                        while (count > 0U) {
                            if (max > nms_dec_box[idx].conf) {
                                nms_dec_box[NMS_hndlr->num_dec_bbx].prev_idx = nms_dec_box[idx].prev_idx;
                                nms_dec_box[NMS_hndlr->num_dec_bbx].next_idx = idx;
                                nms_dec_box[nms_dec_box[idx].prev_idx].next_idx = NMS_hndlr->num_dec_bbx;
                                nms_dec_box[idx].prev_idx = NMS_hndlr->num_dec_bbx;

                                break;
                            }
                            count--;
                            if (count == 0U) {
                                nms_dec_box[NMS_hndlr->num_dec_bbx].prev_idx = idx;
                                nms_dec_box[NMS_hndlr->num_dec_bbx].next_idx = NMS_hndlr->num_dec_bbx;
                                nms_dec_box[idx].next_idx = NMS_hndlr->num_dec_bbx;
                            }
                            idx = nms_dec_box[idx].next_idx;
                        }
                    }
                }
                NMS_hndlr->num_dec_bbx++;
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

static void box_nms(NMS_hndlr_s *NMS_hndlr)
{
    nms_dex_bbx_t *nms_dec_box = NMS_hndlr->G_NmsBox;
    UINT32 i, j, next, idx, num_dec_bbx;
    FLOAT overlap;

    // nms
    idx = NMS_hndlr->max_idx;
    num_dec_bbx = NMS_hndlr->num_dec_bbx;
    for (i = 0; i < num_dec_bbx; i++) {

        if (nms_dec_box[idx].conf != 0.0f) {

            send_bbx(NMS_hndlr, nms_dec_box[idx].cls,
                     nms_dec_box[idx].x1, nms_dec_box[idx].y1,
                     nms_dec_box[idx].x2, nms_dec_box[idx].y2,
                     nms_dec_box[idx].conf);

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

uint32_t NMS_Process(void *hndlr, float *pOdmLoc, float *pOdmConf, float *pArmLoc, float *pArmConf,
                       amba_od_candidate_t *pODOutput, uint32_t *BoxAmount)
{
    NMS_hndlr_s *NMS_hndlr;

    NMS_hndlr = (NMS_hndlr_s *)hndlr;
    if (NMS_hndlr == NULL) {
        AmbaPrint_PrintUInt5("NMS_Process: invalid hndlr!!",0,0,0,0,0);
        return 1;
    }
    if (NMS_hndlr->g_prior_box == NULL) {
        AmbaPrint_PrintUInt5("NMS_Process: invalid prior_box!!",0,0,0,0,0);
        return 1;
    }
    if (NMS_hndlr->G_NmsBox == NULL) {
        AmbaPrint_PrintUInt5("NMS_Process: invalid G_NmsBox!!",0,0,0,0,0);
        return 1;
    }
    if (pODOutput == NULL) {
        AmbaPrint_PrintUInt5("NMS_Process: invalid pODOutput!!",0,0,0,0,0);
        return 1;
    }

    NMS_hndlr->ODCandidates = pODOutput;

    //Clean result
    NMS_hndlr->ODBoxNum = 0;
    (void) AmbaWrap_memset(NMS_hndlr->ODCandidates, 0x0, sizeof(amba_od_candidate_t)*MAX_BBX_NUM);

    send_bbx(NMS_hndlr, 255U, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f);
    bbox_regression(NMS_hndlr, pArmConf, pArmLoc, pOdmConf, pOdmLoc);
    box_nms(NMS_hndlr);

    *BoxAmount = NMS_hndlr->ODBoxNum;

    return 0;
}

uint32_t NMS_LoadPriorBox(char *filename, void **hndlr)
{
    uint32_t ret = 0U;
    NMS_hndlr_s *NMS_hndlr;

    if (hndlr == NULL) {
        AmbaPrint_PrintUInt5("invalid hndlr!!\n",0,0,0,0,0);
        return 1;
    }

    NMS_hndlr = (NMS_hndlr_s *)malloc(sizeof(NMS_hndlr_s));
    *hndlr = NMS_hndlr;
    if (NMS_hndlr == NULL) {
        AmbaPrint_PrintUInt5("fail to create NMS_hndlr!!\n",0,0,0,0,0);
        ret = 1U;
    } else {
        memset(NMS_hndlr, 0, sizeof(NMS_hndlr_s));
    }

    if (ret == 0U) {
        ret = get_file_length(filename, &(NMS_hndlr->pb_len));
        if (ret == 0U) {
            //Use malloc to create priorbox buffer for Linux. This has to change for RTOS case.
            NMS_hndlr->g_prior_box = (float *)malloc(NMS_hndlr->pb_len);
            if (NMS_hndlr->g_prior_box == NULL) {
                AmbaPrint_PrintUInt5("fail to create g_prior_box!!\n",0,0,0,0,0);
                ret = 2;
            } else {
                load_binary(filename, NMS_hndlr->g_prior_box, NMS_hndlr->pb_len);
                NMS_hndlr->total_bbox = NMS_hndlr->pb_len >> 5U;
                NMS_hndlr->G_NmsBox = (nms_dex_bbx_t *)malloc(NMS_hndlr->total_bbox * sizeof(nms_dex_bbx_t));
                if (NMS_hndlr->G_NmsBox == NULL) {
                    AmbaPrint_PrintUInt5("fail to create G_NmsBox!!\n",0,0,0,0,0);
                    ret = 3;
                }
            }
        }
    }

    return ret;
}

uint32_t NMS_Config(void *hndlr, win_ctx_t *Config)
{
    NMS_hndlr_s *NMS_hndlr;

    NMS_hndlr = (NMS_hndlr_s *)hndlr;
    if (NMS_hndlr == NULL) {
        AmbaPrint_PrintUInt5("NMS_Process: invalid hndlr!!",0,0,0,0,0);
        return 1;
    }
    if (Config == NULL) {
        AmbaPrint_PrintUInt5("NMS_Process: invalid Config!!",0,0,0,0,0);
        return 2;
    }

    NMS_hndlr->Conf.raw_w = Config->raw_w;
    NMS_hndlr->Conf.raw_h = Config->raw_h;
    NMS_hndlr->Conf.roi_w = Config->roi_w;
    NMS_hndlr->Conf.roi_h = Config->roi_h;
    NMS_hndlr->Conf.roi_start_col = Config->roi_start_col;
    NMS_hndlr->Conf.roi_start_row = Config->roi_start_row;
    NMS_hndlr->Conf.net_in_w = Config->net_in_w;
    NMS_hndlr->Conf.net_in_h = Config->net_in_h;

    return 0;
}

uint32_t NMS_Release(void *hndlr)
{
    NMS_hndlr_s *NMS_hndlr;

    NMS_hndlr = (NMS_hndlr_s *)hndlr;
    if (NMS_hndlr != NULL) {
        if (NMS_hndlr->g_prior_box != NULL) {
            free(NMS_hndlr->g_prior_box);
        }
        if (NMS_hndlr->G_NmsBox != NULL) {
            free(NMS_hndlr->G_NmsBox);
        }
        free(NMS_hndlr);
    }

    return 0;
}

