/*
*  @file AmbaDSP_ImgDebugPrint.c
*
* Copyright (c) 2020 Ambarella International LP
*
* This file and its contents ("Software") are protected by intellectual
* property rights including, without limitation, U.S. and/or foreign
* copyrights. This Software is also the confidential and proprietary
* information of Ambarella International LP and its licensors. You may not use, reproduce,
* disclose, distribute, modify, or otherwise prepare derivative works of this
* Software or any portion thereof except pursuant to a signed license agreement
* or nondisclosure agreement with Ambarella International LP or its authorized affiliates.
* In the absence of such an agreement, you agree to promptly notify and return
* this Software to Ambarella International LP.
*
* THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
* INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF NON-INFRINGEMENT,
* MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
* IN NO EVENT SHALL AMBARELLA INTERNATIONAL LP OR ITS AFFILIATES BE LIABLE FOR ANY DIRECT,
* INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
* (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
* LOSS OF USE, DATA, OR PROFITS; COMPUTER FAILURE OR MALFUNCTION; OR BUSINESS
* INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
* CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
* ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
* POSSIBILITY OF SUCH DAMAGE.
*/

#include "ik_data_type.h"
#include "AmbaDSP_ImgArchSystemAPI.h"
#include "AmbaDSP_ImgDebugPrint.h"
#include "AmbaDSP_ImgConfigEntity.h"//amba_ik_cv2_liso_cfg_t
#include "AmbaDSP_ImgContextComponentIF.h"//amba_ik_context_entity_t
#include "AmbaDSP_ImgArchComponentIF.h"


//this file should move to early test and offline parser.


//char amalgam_output_filname[128];//"cv1_amalgam_data_vid1.bin"
//char f_amalgam_output_filname[128];

static amba_img_dsp_debug_print_func_t *p_debug_print_func = NULL;
static uint8 filter_debug_log[LOG_MAX];

void ik_init_debug_print(void)
{
    (void)amba_ik_system_memset(filter_debug_log, 0x0, (uint32)LOG_MAX);
}

uint32 ik_set_debug_log_id(amba_ik_debug_id_e debug_id, uint8 enable)
{
    uint32 rval = IK_OK;
    int32 i;
    if(debug_id == LOG_SHOW_CMD_ID_LIST) {
        amba_ik_system_print_uint32_5("[%d]LOG_SAFETY_INFO",(uint32)LOG_SAFETY_INFO, DC_U, DC_U, DC_U, DC_U);
        amba_ik_system_print_uint32_5("[%d]LOG_VIN_SENSOR_INFO",(uint32)LOG_VIN_SENSOR_INFO, DC_U, DC_U, DC_U, DC_U);
        amba_ik_system_print_uint32_5("[%d]LOG_FE_TONE_CURVE",(uint32)LOG_FE_TONE_CURVE, DC_U, DC_U, DC_U, DC_U);
        amba_ik_system_print_uint32_5("[%d]LOG_FE_STATIC_BLC",(uint32)LOG_FE_STATIC_BLC, DC_U, DC_U, DC_U, DC_U);
        amba_ik_system_print_uint32_5("[%d]LOG_FE_WB_GAIN",(uint32)LOG_FE_WB_GAIN, DC_U, DC_U, DC_U, DC_U);
        amba_ik_system_print_uint32_5("[%d]LOG_HDR_BLEND",(uint32)LOG_HDR_BLEND, DC_U, DC_U, DC_U, DC_U);
        amba_ik_system_print_uint32_5("[%d]LOG_VIGNETTE",(uint32)LOG_VIGNETTE, DC_U, DC_U, DC_U, DC_U);
        amba_ik_system_print_uint32_5("[%d]LOG_DYNAMIC_BAD_PIXEL_CORRECTION",(uint32)LOG_DYNAMIC_BAD_PIXEL_CORRECTION, DC_U, DC_U, DC_U, DC_U);
        amba_ik_system_print_uint32_5("[%d]LOG_STATIC_BAD_PIXEL_COTTECTION",(uint32)LOG_STATIC_BAD_PIXEL_CORRECTION, DC_U, DC_U, DC_U, DC_U);
        amba_ik_system_print_uint32_5("[%d]LOG_CFA_LEAKAGE_FILTER",(uint32)LOG_CFA_LEAKAGE_FILTER, DC_U, DC_U, DC_U, DC_U);
        amba_ik_system_print_uint32_5("[%d]LOG_ANTI_ALIASING",(uint32)LOG_ANTI_ALIASING, DC_U, DC_U, DC_U, DC_U);
        amba_ik_system_print_uint32_5("[%d]LOG_GBGR_MISMATCH",(uint32)LOG_GRGB_MISMATCH, DC_U, DC_U, DC_U, DC_U);
        amba_ik_system_print_uint32_5("[%d]LOG_GLOBAL_DGAIN",(uint32)LOG_GLOBAL_DGAIN, DC_U, DC_U, DC_U, DC_U);
        amba_ik_system_print_uint32_5("[%d]LOG_AE_GAIN",(uint32)LOG_AE_GAIN, DC_U, DC_U, DC_U, DC_U);
        amba_ik_system_print_uint32_5("[%d]LOG_BEFORE_WB_GAIN",(uint32)LOG_BEFORE_WB_GAIN, DC_U, DC_U, DC_U, DC_U);
        amba_ik_system_print_uint32_5("[%d]LOG_CAWARP_COMPENSATION",(uint32)LOG_CAWARP_COMPENSATION, DC_U, DC_U, DC_U, DC_U);
        amba_ik_system_print_uint32_5("[%d]LOG_CONTRAST_ENHANCE",(uint32)LOG_CONTRAST_ENHANCE, DC_U, DC_U, DC_U, DC_U);
        amba_ik_system_print_uint32_5("[%d]LOG_CONTRAST_ENHANCE_IN_TABLE",(uint32)LOG_CONTRAST_ENHANCE_IN_TABLE, DC_U, DC_U, DC_U, DC_U);
        amba_ik_system_print_uint32_5("[%d]LOG_CONTRAST_ENHANCE_OUT_TABLE",(uint32)LOG_CONTRAST_ENHANCE_OUT_TABLE, DC_U, DC_U, DC_U, DC_U);
        amba_ik_system_print_uint32_5("[%d]LOG_AFTER_WB_GAIN",(uint32)LOG_AFTER_WB_GAIN, DC_U, DC_U, DC_U, DC_U);
        amba_ik_system_print_uint32_5("[%d]LOG_CFA_NOISE_FILTER",(uint32)LOG_CFA_NOISE_FILTER, DC_U, DC_U, DC_U, DC_U);
        amba_ik_system_print_uint32_5("[%d]LOG_DEMOSAIC",(uint32)LOG_DEMOSAIC, DC_U, DC_U, DC_U, DC_U);
        amba_ik_system_print_uint32_5("[%d]LOG_RGB_IR",(uint32)LOG_RGB_IR, DC_U, DC_U, DC_U, DC_U);
        amba_ik_system_print_uint32_5("[%d]LOG_COLOR_CORRECTION_REG",(uint32)LOG_COLOR_CORRECTION_REG, DC_U, DC_U, DC_U, DC_U);
        amba_ik_system_print_uint32_5("[%d]LOG_COLOR_CORRECTION",(uint32)LOG_COLOR_CORRECTION, DC_U, DC_U, DC_U, DC_U);
        amba_ik_system_print_uint32_5("[%d]LOG_TONE_CURVE",(uint32)LOG_TONE_CURVE, DC_U, DC_U, DC_U, DC_U);
        amba_ik_system_print_uint32_5("[%d]LOG_RGB_TO_YUV_MATRIX",(uint32)LOG_RGB_TO_YUV_MATRIX, DC_U, DC_U, DC_U, DC_U);
        amba_ik_system_print_uint32_5("[%d]LOG_CHROMA_SCALE",(uint32)LOG_CHROMA_SCALE, DC_U, DC_U, DC_U, DC_U);
        amba_ik_system_print_uint32_5("[%d]LOG_CHROMA_FILTER",(uint32)LOG_CHROMA_FILTER, DC_U, DC_U, DC_U, DC_U);
        amba_ik_system_print_uint32_5("[%d]LOG_CHROMA_MEDIAN_FILTER",(uint32)LOG_CHROMA_MEDIAN_FILTER, DC_U, DC_U, DC_U, DC_U);
        amba_ik_system_print_uint32_5("[%d]LOG_FIRST_LUMA_PROCESSING_MODE",(uint32)LOG_FIRST_LUMA_PROCESSING_MODE, DC_U, DC_U, DC_U, DC_U);
        amba_ik_system_print_uint32_5("[%d]LOG_ADVANCE_SPATIAL_FILTER",(uint32)LOG_ADVANCE_SPATIAL_FILTER, DC_U, DC_U, DC_U, DC_U);
        amba_ik_system_print_uint32_5("[%d]LOG_FIRST_SHARPEN_NOISE_BOTH",(uint32)LOG_FIRST_SHARPEN_NOISE_BOTH, DC_U, DC_U, DC_U, DC_U);
        amba_ik_system_print_uint32_5("[%d]LOG_FIRST_SHARPEN_NOISE_FIR",(uint32)LOG_FIRST_SHARPEN_NOISE_FIR, DC_U, DC_U, DC_U, DC_U);
        amba_ik_system_print_uint32_5("[%d]LOG_FIRST_SHARPEN_NOISE_NOISE",(uint32)LOG_FIRST_SHARPEN_NOISE_NOISE, DC_U, DC_U, DC_U, DC_U);
        amba_ik_system_print_uint32_5("[%d]LOG_FIRST_SHARPEN_NOISE_CORING",(uint32)LOG_FIRST_SHARPEN_NOISE_CORING, DC_U, DC_U, DC_U, DC_U);
        amba_ik_system_print_uint32_5("[%d]LOG_FIRST_SHARPEN_NOISE_CORING_INDEX_SCALE",(uint32)LOG_FIRST_SHARPEN_NOISE_CORING_INDEX_SCALE, DC_U, DC_U, DC_U, DC_U);
        amba_ik_system_print_uint32_5("[%d]LOG_FIRST_SHARPEN_NOISE_MIN_CORING_RESULT",(uint32)LOG_FIRST_SHARPEN_NOISE_MIN_CORING_RESULT, DC_U, DC_U, DC_U, DC_U);
        amba_ik_system_print_uint32_5("[%d]LOG_FIRST_SHARPEN_NOISE_MAX_CORING_RESULT",(uint32)LOG_FIRST_SHARPEN_NOISE_MAX_CORING_RESULT, DC_U, DC_U, DC_U, DC_U);
        amba_ik_system_print_uint32_5("[%d]LOG_FIRST_SHARPEN_NOISE_SCALE_CORING",(uint32)LOG_FIRST_SHARPEN_NOISE_SCALE_CORING, DC_U, DC_U, DC_U, DC_U);

        amba_ik_system_print_uint32_5("[%d]LOG_WARP_COMPENSATION",(uint32)LOG_WARP_COMPENSATION, DC_U, DC_U, DC_U, DC_U);
        amba_ik_system_print_uint32_5("[%d]LOG_VIDEO_MCTF",(uint32)LOG_VIDEO_MCTF, DC_U, DC_U, DC_U, DC_U);
        amba_ik_system_print_uint32_5("[%d]LOG_VIDEO_MCTF_TEMPORAL_ADJUST",(uint32)LOG_VIDEO_MCTF_TEMPORAL_ADJUST, DC_U, DC_U, DC_U, DC_U);
        amba_ik_system_print_uint32_5("[%d]LOG_VIDEO_MCTF_AND_FINAL_SHARPEN",(uint32)LOG_VIDEO_MCTF_AND_FINAL_SHARPEN, DC_U, DC_U, DC_U, DC_U);

        amba_ik_system_print_uint32_5("[%d]LOG_FINAL_SHARPEN_NOISE_BOTH",(uint32)LOG_FINAL_SHARPEN_NOISE_BOTH, DC_U, DC_U, DC_U, DC_U);
        amba_ik_system_print_uint32_5("[%d]LOG_FINAL_SHARPEN_NOISE_BOTH_TBT",(uint32)LOG_FINAL_SHARPEN_NOISE_BOTH_TDT, DC_U, DC_U, DC_U, DC_U);
        amba_ik_system_print_uint32_5("[%d]LOG_FINAL_SHARPEN_NOISE_NOISE",(uint32)LOG_FINAL_SHARPEN_NOISE_NOISE, DC_U, DC_U, DC_U, DC_U);
        amba_ik_system_print_uint32_5("[%d]LOG_FINAL_SHARPEN_NOISE_FIR",(uint32)LOG_FINAL_SHARPEN_NOISE_FIR, DC_U, DC_U, DC_U, DC_U);
        amba_ik_system_print_uint32_5("[%d]LOG_FINAL_SHARPEN_NOISE_CORING",(uint32)LOG_FINAL_SHARPEN_NOISE_CORING, DC_U, DC_U, DC_U, DC_U);
        amba_ik_system_print_uint32_5("[%d]LOG_FINAL_SHARPEN_NOISE_CORING_INDEX_SCALE",(uint32)LOG_FINAL_SHARPEN_NOISE_CORING_INDEX_SCALE, DC_U, DC_U, DC_U, DC_U);
        amba_ik_system_print_uint32_5("[%d]LOG_FINAL_SHARPEN_NOISE_MIN_CORING_RESULT",(uint32)LOG_FINAL_SHARPEN_NOISE_MIN_CORING_RESULT, DC_U, DC_U, DC_U, DC_U);
        amba_ik_system_print_uint32_5("[%d]LOG_FINAL_SHARPEN_NOISE_MAX_CORING_RESULT",(uint32)LOG_FINAL_SHARPEN_NOISE_MAX_CORING_RESULT, DC_U, DC_U, DC_U, DC_U);
        amba_ik_system_print_uint32_5("[%d]LOG_FINAL_SHARPEN_NOISE_SCALE_CORING",(uint32)LOG_FINAL_SHARPEN_NOISE_SCALE_CORING, DC_U, DC_U, DC_U, DC_U);

        amba_ik_system_print_uint32_5("[%d]LOG_DZOOM_INFO",(uint32)LOG_DZOOM_INFO, DC_U, DC_U, DC_U, DC_U);
        amba_ik_system_print_uint32_5("[%d]LOG_WINDOW_SIZE_INFO",(uint32)LOG_WINDOW_SIZE_INFO, DC_U, DC_U, DC_U, DC_U);
        amba_ik_system_print_uint32_5("[%d]LOG_VIN_ACTIVE_WIN",(uint32)LOG_VIN_ACTIVE_WIN, DC_U, DC_U, DC_U, DC_U);
        amba_ik_system_print_uint32_5("[%d]LOG_DUMMY_MARGIN_RANGE",(uint32)LOG_DUMMY_MARGIN_RANGE, DC_U, DC_U, DC_U, DC_U);

        amba_ik_system_print_uint32_5("[%d]LOG_POST_EXECUTE",(uint32)LOG_POST_EXECUTE, DC_U, DC_U, DC_U, DC_U);
        amba_ik_system_print_uint32_5("[%d]LOG_3A_STAT",(uint32)LOG_3A_STAT, DC_U, DC_U, DC_U, DC_U);
        amba_ik_system_print_uint32_5("[%d]LOG_CTX_INIT",(uint32)LOG_CTX_INIT, DC_U, DC_U, DC_U, DC_U);
        amba_ik_system_print_uint32_5("[%d]LOG_SHOW_ALL",(uint32)LOG_SHOW_ALL, DC_U, DC_U, DC_U, DC_U);
    } else if (debug_id == LOG_SHOW_ALL) {
        for (i = 0; i < (int32)LOG_MAX; i++) {
            filter_debug_log[i] = enable;
        }
    } else {
        filter_debug_log[debug_id] = enable;
    }
    return rval;
}

static void amba_ik_debug_print_safety_info(uint32 context_id, const ik_safety_info_t *p_safety_info)
{
    if (p_safety_info == NULL) {
        // fix compile error unused parameter
    } else {
        if (filter_debug_log[LOG_SAFETY_INFO] == 1U) {
            amba_ik_system_print_str_5("[IK] %s",__func__, DC_S, DC_S, DC_S, DC_S);
            amba_ik_system_print_uint32_5("[IK](context_id = %d)", context_id, DC_U, DC_U, DC_U, DC_U);
            amba_ik_system_print_uint32_5("[IK]    update_freq = %d", p_safety_info->update_freq, DC_U, DC_U, DC_U, DC_U);
        }
    }
}

static void amba_ik_debug_print_vin_sensor_info(uint32 context_id, const ik_vin_sensor_info_t *p_vin_sensor)
{
    if (p_vin_sensor == NULL) {
        // fix compile error unused parameter
    } else {
        if (filter_debug_log[LOG_VIN_SENSOR_INFO] == 1U) {
            amba_ik_system_print_str_5("[IK] %s",__func__, DC_S, DC_S, DC_S, DC_S);
            amba_ik_system_print_uint32_5("[IK](context_id = %d)", context_id, DC_U, DC_U, DC_U, DC_U);
            amba_ik_system_print_uint32_5("[IK]    vin_id = %d", p_vin_sensor->vin_id, DC_U, DC_U, DC_U, DC_U);
            amba_ik_system_print_uint32_5("[IK]    sensor_resolution = %d", p_vin_sensor->sensor_resolution, DC_U, DC_U, DC_U, DC_U);
            amba_ik_system_print_uint32_5("[IK]    sensor_pattern = %d", p_vin_sensor->sensor_pattern, DC_U, DC_U, DC_U, DC_U);
            amba_ik_system_print_uint32_5("[IK]    sensor_mode = %d", p_vin_sensor->sensor_mode, DC_U, DC_U, DC_U, DC_U);
            amba_ik_system_print_uint32_5("[IK]    compression = %d", p_vin_sensor->compression, DC_U, DC_U, DC_U, DC_U);
            amba_ik_system_print_int32_5("[IK]    compression_offset = %d", p_vin_sensor->compression_offset, DC_I, DC_I, DC_I, DC_I);
            amba_ik_system_print_uint32_5("[IK]    sensor_readout_mode = %d", p_vin_sensor->sensor_readout_mode, DC_U, DC_U, DC_U, DC_U);
        }
    }
}

static void amba_ik_debug_print_fe_tone_curve(uint32 context_id, const ik_frontend_tone_curve_t *p_tone_curve)
{
    if (p_tone_curve == NULL) {
        // fix compile error unused parameter
    } else {
        if (filter_debug_log[LOG_FE_TONE_CURVE] == 1U) {
            amba_ik_system_print_str_5("[IK] %s",__func__, DC_S, DC_S, DC_S, DC_S);
            amba_ik_system_print_uint32_5("[IK](context_id = %d)", context_id, DC_U, DC_U, DC_U, DC_U);
            amba_ik_system_print_uint32_5("[IK]    enable = %d", p_tone_curve->decompand_enable, DC_U, DC_U, DC_U, DC_U);
            amba_ik_system_print_uint32_5("[IK]    compand_table = %8d %8d ... %8d %8d", p_tone_curve->decompand_table[0],\
                                          p_tone_curve->decompand_table[1],\
                                          p_tone_curve->decompand_table[145-2],\
                                          p_tone_curve->decompand_table[145-1], DC_U);
        }
    }
}

static void amba_ik_debug_print_fe_static_blc(uint32 context_id, const ik_static_blc_level_t *p_blc, uint32 exp_idx)
{
    if (p_blc == NULL) {

    } else {
        if (filter_debug_log[LOG_FE_STATIC_BLC] == 1U) {
            amba_ik_system_print_str_5("[IK] %s",__func__, DC_S, DC_S, DC_S, DC_S);
            amba_ik_system_print_uint32_5("[IK] (context_id = %d, exp_idx %d)", context_id, exp_idx, DC_U, DC_U, DC_U);
            amba_ik_system_print_uint32_5("[IK]    black_r = %d", (uint32)p_blc->black_r, DC_U, DC_U, DC_U, DC_U);
            amba_ik_system_print_uint32_5("[IK]    black_gr = %d", (uint32)p_blc->black_gr, DC_U, DC_U, DC_U, DC_U);
            amba_ik_system_print_uint32_5("[IK]    black_gb = %d", (uint32)p_blc->black_gb, DC_U, DC_U, DC_U, DC_U);
            amba_ik_system_print_uint32_5("[IK]    black_b = %d", (uint32)p_blc->black_b, DC_U, DC_U, DC_U, DC_U);
            amba_ik_system_print_uint32_5("[IK]    black_g = %d", (uint32)p_blc->black_g, DC_U, DC_U, DC_U, DC_U);
            amba_ik_system_print_uint32_5("[IK]    black_ir = %d", (uint32)p_blc->black_ir, DC_U, DC_U, DC_U, DC_U);
        }
    }
}

static void amba_ik_debug_print_fe_wbgain(uint32 context_id, const ik_frontend_wb_gain_t *p_fe_wb, uint32 exp_idx)
{
    if (p_fe_wb == NULL) {

    } else {
        if (filter_debug_log[LOG_FE_WB_GAIN] == 1U) {
            amba_ik_system_print_str_5("[IK] %s",__func__, DC_S, DC_S, DC_S, DC_S);
            amba_ik_system_print_uint32_5("[IK]    (context_id = %d, exp_idx %d)", context_id, exp_idx, DC_U, DC_U, DC_U);
            amba_ik_system_print_uint32_5("[IK]    r_gain = %d", p_fe_wb->r_gain, DC_U, DC_U, DC_U, DC_U);
            amba_ik_system_print_uint32_5("[IK]    g_gain = %d", p_fe_wb->g_gain, DC_U, DC_U, DC_U, DC_U);
            amba_ik_system_print_uint32_5("[IK]    b_gain = %d", p_fe_wb->b_gain, DC_U, DC_U, DC_U, DC_U);
            amba_ik_system_print_uint32_5("[IK]    ir_gain = %d", p_fe_wb->ir_gain, DC_U, DC_U, DC_U, DC_U);
            amba_ik_system_print_uint32_5("[IK]    shutter_ratio = %d", p_fe_wb->shutter_ratio, DC_U, DC_U, DC_U, DC_U);
        }
    }
}

static void amba_ik_debug_print_hdr_blend(uint32 context_id, const ik_hdr_blend_t *p_hdr_blend)
{
    if (p_hdr_blend == NULL) {

    } else {
        if (filter_debug_log[LOG_HDR_BLEND] == 1U) {
            uint32 i;
            amba_ik_system_print_str_5("[IK] %s",__func__, DC_S, DC_S, DC_S, DC_S);
            amba_ik_system_print_uint32_5("[IK](context_id = %d)", context_id, DC_U, DC_U, DC_U, DC_U);
            amba_ik_system_print_uint32_5("[IK]    enable = %d", p_hdr_blend->enable, DC_U, DC_U, DC_U, DC_U);
            for(i = 0U; i < 4U ; i++) {
                amba_ik_system_print_uint32_5("[IK]    delta_t0[%d] = %d", i, p_hdr_blend->delta_t0[i], DC_U, DC_U, DC_U);
            }
            for(i = 0U; i < 4U ; i++) {
                amba_ik_system_print_uint32_5("[IK]    delta_t1[%d] = %d", i, p_hdr_blend->delta_t1[i], DC_U, DC_U, DC_U);
            }
            amba_ik_system_print_uint32_5("[IK]    flicker_threshold = %d", p_hdr_blend->flicker_threshold, DC_U, DC_U, DC_U, DC_U);
            for(i = 0U; i < 4U ; i++) {
                amba_ik_system_print_uint32_5("[IK]    t0_offset[%d] = %d", i, p_hdr_blend->t0_offset[i], DC_U, DC_U, DC_U);
            }
            for(i = 0U; i < 4U ; i++) {
                amba_ik_system_print_uint32_5("[IK]    t1_offset[%d] = %d", i, p_hdr_blend->t1_offset[i], DC_U, DC_U, DC_U);
            }
        }
    }
}

static void amba_ik_debug_print_vig_enable(uint32 context_id, const uint32 enable)
{
    if (filter_debug_log[LOG_VIGNETTE] == 1U) {
        amba_ik_system_print_str_5("[IK] %s",__func__, DC_S, DC_S, DC_S, DC_S);
        amba_ik_system_print_uint32_5("[IK](context_id = %d)", context_id, DC_U, DC_U, DC_U, DC_U);
        amba_ik_system_print_uint32_5("[IK]    enable = %d", enable, DC_U, DC_U, DC_U, DC_U);
    }
}

static void amba_ik_debug_print_geometry(const ik_vin_sensor_geometry_t *p_vin_sensor_geo)
{
    amba_ik_system_print_str_5("[IK] p_vin_sensor_geo:", DC_S, DC_S, DC_S, DC_S, DC_S);
    amba_ik_system_print_uint32_5("[IK]    start_x = %d", p_vin_sensor_geo->start_x, DC_U, DC_U, DC_U, DC_U);
    amba_ik_system_print_uint32_5("[IK]    start_y = %d", p_vin_sensor_geo->start_y, DC_U, DC_U, DC_U, DC_U);
    amba_ik_system_print_uint32_5("[IK]    width = %d", p_vin_sensor_geo->width, DC_U, DC_U, DC_U, DC_U);
    amba_ik_system_print_uint32_5("[IK]    height = %d", p_vin_sensor_geo->height, DC_U, DC_U, DC_U, DC_U);
    amba_ik_system_print_uint32_5("[IK]    h_sub_sample.factor_den = %d", p_vin_sensor_geo->h_sub_sample.factor_den, DC_U, DC_U, DC_U, DC_U);
    amba_ik_system_print_uint32_5("[IK]    h_sub_sample.factor_num = %d", p_vin_sensor_geo->h_sub_sample.factor_num, DC_U, DC_U, DC_U, DC_U);
    amba_ik_system_print_uint32_5("[IK]    v_sub_sample.factor_den = %d", p_vin_sensor_geo->v_sub_sample.factor_den, DC_U, DC_U, DC_U, DC_U);
    amba_ik_system_print_uint32_5("[IK]    v_sub_sample.factor_num = %d", p_vin_sensor_geo->v_sub_sample.factor_num, DC_U, DC_U, DC_U, DC_U);
}

static void amba_ik_debug_print_vig(uint32 context_id, const ik_vignette_t *p_vig)
{
    uint32 y, dir;
    if (p_vig == NULL) {

    } else {
        if (filter_debug_log[LOG_VIGNETTE] == 1U) {
            amba_ik_system_print_str_5("[IK] %s",__func__, DC_S, DC_S, DC_S, DC_S);
            amba_ik_system_print_uint32_5("[IK](context_id = %d)", context_id, DC_U, DC_U, DC_U, DC_U);
            amba_ik_system_print_uint32_5("[IK]    calib_mode_enable = %d", p_vig->calib_mode_enable, DC_U, DC_U, DC_U, DC_U);
            amba_ik_debug_print_geometry(&p_vig->vin_sensor_geo);


            amba_ik_system_print_uint32_5("[IK]    version = 0x%x", p_vig->calib_vignette_info.version, DC_U, DC_U, DC_U, DC_U);
            amba_ik_system_print_uint32_5("[IK]    num_radial_bins_coarse = %d", p_vig->calib_vignette_info.num_radial_bins_coarse, DC_U, DC_U, DC_U, DC_U);
            amba_ik_system_print_uint32_5("[IK]    size_radial_bins_coarse_log = %d", p_vig->calib_vignette_info.size_radial_bins_coarse_log, DC_U, DC_U, DC_U, DC_U);
            amba_ik_system_print_uint32_5("[IK]    num_radial_bins_fine = %d", p_vig->calib_vignette_info.num_radial_bins_fine, DC_U, DC_U, DC_U, DC_U);
            amba_ik_system_print_uint32_5("[IK]    num_radial_bins_fine = %d", p_vig->calib_vignette_info.num_radial_bins_fine, DC_U, DC_U, DC_U, DC_U);
            amba_ik_system_print_uint32_5("[IK]    model_center_x = %d %d %d %d", p_vig->calib_vignette_info.model_center_x_R\
                                          , p_vig->calib_vignette_info.model_center_x_Gr\
                                          , p_vig->calib_vignette_info.model_center_x_B\
                                          , p_vig->calib_vignette_info.model_center_x_Gb, DC_U);
            amba_ik_system_print_uint32_5("[IK]    model_center_y = %d %d %d %d", p_vig->calib_vignette_info.model_center_y_R\
                                          , p_vig->calib_vignette_info.model_center_y_Gr\
                                          , p_vig->calib_vignette_info.model_center_y_B\
                                          , p_vig->calib_vignette_info.model_center_y_Gb, DC_U);

            amba_ik_system_print_str_5("[IK]    vig_gain_tbl: vig_gain_tbl_R", DC_S, DC_S, DC_S, DC_S, DC_S);
            for( dir= 0; dir < 4U ; dir++) {
                for( y= 0; y < 16U ; y++) {
                    amba_ik_system_print_uint32_5(" %d %d %d %d", p_vig->calib_vignette_info.vig_gain_tbl_R[dir][(y*8UL)]\
                                                  , p_vig->calib_vignette_info.vig_gain_tbl_R[dir][(y*8UL)+1UL]\
                                                  , p_vig->calib_vignette_info.vig_gain_tbl_R[dir][(y*8UL)+2UL]\
                                                  , p_vig->calib_vignette_info.vig_gain_tbl_R[dir][(y*8UL)+3UL], DC_U);
                    amba_ik_system_print_uint32_5(" %d %d %d %d", p_vig->calib_vignette_info.vig_gain_tbl_R[dir][(y*8UL)+4UL ]\
                                                  , p_vig->calib_vignette_info.vig_gain_tbl_R[dir][(y*8UL)+5UL]\
                                                  , p_vig->calib_vignette_info.vig_gain_tbl_R[dir][(y*8UL)+6UL]\
                                                  , p_vig->calib_vignette_info.vig_gain_tbl_R[dir][(y*8UL)+7UL], DC_U);
                }
            }
            amba_ik_system_print_str_5("[IK]    vig_gain_tbl: vig_gain_tbl_Gr", DC_S, DC_S, DC_S, DC_S, DC_S);
            for( dir= 0; dir < 4U ; dir++) {
                for( y= 0; y < 16U ; y++) {
                    amba_ik_system_print_uint32_5(" %d %d %d %d", p_vig->calib_vignette_info.vig_gain_tbl_Gr[dir][(y*8UL)]\
                                                  , p_vig->calib_vignette_info.vig_gain_tbl_Gr[dir][(y*8UL)+1UL]\
                                                  , p_vig->calib_vignette_info.vig_gain_tbl_Gr[dir][(y*8UL)+2UL]\
                                                  , p_vig->calib_vignette_info.vig_gain_tbl_Gr[dir][(y*8UL)+3UL], DC_U);
                    amba_ik_system_print_uint32_5(" %d %d %d %d", p_vig->calib_vignette_info.vig_gain_tbl_Gr[dir][(y*8UL)+4UL ]\
                                                  , p_vig->calib_vignette_info.vig_gain_tbl_Gr[dir][(y*8UL)+5UL]\
                                                  , p_vig->calib_vignette_info.vig_gain_tbl_Gr[dir][(y*8UL)+6UL]\
                                                  , p_vig->calib_vignette_info.vig_gain_tbl_Gr[dir][(y*8UL)+7UL], DC_U);
                }
            }
            amba_ik_system_print_str_5("[IK]    vig_gain_tbl: vig_gain_tbl_B", DC_S, DC_S, DC_S, DC_S, DC_S);
            for( dir= 0; dir < 4U ; dir++) {
                for( y= 0; y < 16U ; y++) {
                    amba_ik_system_print_uint32_5(" %d %d %d %d", p_vig->calib_vignette_info.vig_gain_tbl_B[dir][(y*8UL)]\
                                                  , p_vig->calib_vignette_info.vig_gain_tbl_B[dir][(y*8UL)+1UL]\
                                                  , p_vig->calib_vignette_info.vig_gain_tbl_B[dir][(y*8UL)+2UL]\
                                                  , p_vig->calib_vignette_info.vig_gain_tbl_B[dir][(y*8UL)+3UL], DC_U);
                    amba_ik_system_print_uint32_5(" %d %d %d %d", p_vig->calib_vignette_info.vig_gain_tbl_B[dir][(y*8UL)+4UL ]\
                                                  , p_vig->calib_vignette_info.vig_gain_tbl_B[dir][(y*8UL)+5UL]\
                                                  , p_vig->calib_vignette_info.vig_gain_tbl_B[dir][(y*8UL)+6UL]\
                                                  , p_vig->calib_vignette_info.vig_gain_tbl_B[dir][(y*8UL)+7UL], DC_U);
                }
            }
            amba_ik_system_print_str_5("[IK]    vig_gain_tbl: vig_gain_tbl_Gb", DC_S, DC_S, DC_S, DC_S, DC_S);
            for( dir= 0; dir < 4U ; dir++) {
                for( y= 0; y < 16U ; y++) {
                    amba_ik_system_print_uint32_5(" %d %d %d %d", p_vig->calib_vignette_info.vig_gain_tbl_Gb[dir][(y*8UL)]\
                                                  , p_vig->calib_vignette_info.vig_gain_tbl_Gb[dir][(y*8UL)+1UL]\
                                                  , p_vig->calib_vignette_info.vig_gain_tbl_Gb[dir][(y*8UL)+2UL]\
                                                  , p_vig->calib_vignette_info.vig_gain_tbl_Gb[dir][(y*8UL)+3UL], DC_U);
                    amba_ik_system_print_uint32_5(" %d %d %d %d", p_vig->calib_vignette_info.vig_gain_tbl_Gb[dir][(y*8UL)+4UL ]\
                                                  , p_vig->calib_vignette_info.vig_gain_tbl_Gb[dir][(y*8UL)+5UL]\
                                                  , p_vig->calib_vignette_info.vig_gain_tbl_Gb[dir][(y*8UL)+6UL]\
                                                  , p_vig->calib_vignette_info.vig_gain_tbl_Gb[dir][(y*8UL)+7UL], DC_U);
                }
            }
        }
    }
}

static void amba_ik_debug_print_dynamic_bpc(uint32 context_id, const ik_dynamic_bad_pixel_correction_t *p_dynamic_bpc)
{
    if (p_dynamic_bpc==NULL) {

    } else {
        if (filter_debug_log[LOG_DYNAMIC_BAD_PIXEL_CORRECTION] == 1U) {
            amba_ik_system_print_str_5("[IK] %s",__func__, DC_S, DC_S, DC_S, DC_S);
            amba_ik_system_print_uint32_5("[IK](context_id = %d)", context_id, DC_U, DC_U, DC_U, DC_U);
            amba_ik_system_print_uint32_5("[IK]    enable = %d", p_dynamic_bpc->enable, DC_U, DC_U, DC_U, DC_U);
            amba_ik_system_print_uint32_5("[IK]    hot_pixel_strength = %d", p_dynamic_bpc->hot_pixel_strength, DC_U, DC_U, DC_U, DC_U);
            amba_ik_system_print_uint32_5("[IK]    dark_pixel_strength = %d", p_dynamic_bpc->dark_pixel_strength, DC_U, DC_U, DC_U, DC_U);
            amba_ik_system_print_uint32_5("[IK]    correction_method = %d", p_dynamic_bpc->correction_method, DC_U, DC_U, DC_U, DC_U);
        }
    }
}

static void amba_ik_debug_print_static_bpc_enable(uint32 context_id, const uint32 enable)
{
    if (filter_debug_log[LOG_STATIC_BAD_PIXEL_CORRECTION] == 1U) {
        amba_ik_system_print_str_5("[IK] %s",__func__, DC_S, DC_S, DC_S, DC_S);
        amba_ik_system_print_uint32_5("[IK](context_id = %d)", context_id, DC_U, DC_U, DC_U, DC_U);
        amba_ik_system_print_uint32_5("[IK]    enable = %d", enable, DC_U, DC_U, DC_U, DC_U);
    }
}

static void amba_ik_debug_print_static_bpc(uint32 context_id, const ik_static_bad_pxl_cor_t *p_static_bpc)
{
    if (p_static_bpc == NULL) {

    } else {
        if (filter_debug_log[LOG_STATIC_BAD_PIXEL_CORRECTION] == 1U) {
            uint32 misra_u32_addr;
            amba_ik_system_print_str_5("[IK] %s",__func__, DC_S, DC_S, DC_S, DC_S);
            amba_ik_system_print_uint32_5("[IK](context_id = %d)", context_id, DC_U, DC_U, DC_U, DC_U);
            amba_ik_system_print_str_5("[IK]current_vin_sensor_geo", DC_S, DC_S, DC_S, DC_S, DC_S);
            amba_ik_debug_print_geometry(&p_static_bpc->vin_sensor_geo);
            amba_ik_system_print_uint32_5("[IK]    version = 0x%x", p_static_bpc->calib_sbp_info.version, DC_U, DC_U, DC_U, DC_U);
            (void)amba_ik_system_memcpy(&misra_u32_addr, &p_static_bpc->calib_sbp_info.sbp_buffer, sizeof(uint32));
            amba_ik_system_print_uint32_5("[IK]    sbp_buffer = 0x%x", misra_u32_addr, DC_U, DC_U, DC_U, DC_U);
            amba_ik_system_print_str_5("[IK]calib_sbp_info.vin_sensor_geo", DC_S, DC_S, DC_S, DC_S, DC_S);
        }
    }
}

static void amba_ik_debug_print_cfa_leakage_filter(uint32 context_id, const ik_cfa_leakage_filter_t *p_cfa_leakage)
{
    if (p_cfa_leakage == NULL) {

    } else {
        if (filter_debug_log[LOG_DYNAMIC_BAD_PIXEL_CORRECTION] == 1U) {
            amba_ik_system_print_str_5("[IK] %s",__func__, DC_S, DC_S, DC_S, DC_S);
            amba_ik_system_print_uint32_5("[IK]    (context_id = %d)", context_id, DC_U, DC_U, DC_U, DC_U);
            amba_ik_system_print_uint32_5("[IK]    enable = %d", p_cfa_leakage->enable, DC_U, DC_U, DC_U, DC_U);
            amba_ik_system_print_uint32_5("[IK]    alpha_rr = %d", p_cfa_leakage->alpha_rr, DC_U, DC_U, DC_U, DC_U);
            amba_ik_system_print_uint32_5("[IK]    alpha_rb = %d", p_cfa_leakage->alpha_rb, DC_U, DC_U, DC_U, DC_U);
            amba_ik_system_print_uint32_5("[IK]    alpha_br = %d", p_cfa_leakage->alpha_br, DC_U, DC_U, DC_U, DC_U);
            amba_ik_system_print_uint32_5("[IK]    alpha_bb = %d", p_cfa_leakage->alpha_bb, DC_U, DC_U, DC_U, DC_U);
            amba_ik_system_print_uint32_5("[IK]    saturation_level = %d", p_cfa_leakage->saturation_level, DC_U, DC_U, DC_U, DC_U);
        }
    }
}

static void amba_ik_debug_print_anti_aliasing(uint32 context_id, const ik_anti_aliasing_t *p_anti_aliasing)
{
    if (p_anti_aliasing == NULL) {

    } else {
        if (filter_debug_log[LOG_ANTI_ALIASING] == 1U) {
            amba_ik_system_print_str_5("[IK] %s",__func__, DC_S, DC_S, DC_S, DC_S);
            amba_ik_system_print_uint32_5("[IK]    (context_id = %d)", context_id, DC_U, DC_U, DC_U, DC_U);
            amba_ik_system_print_uint32_5("[IK]    enable = %d", p_anti_aliasing->enable, DC_U, DC_U, DC_U, DC_U);
            amba_ik_system_print_uint32_5("[IK]    thresh = %d", p_anti_aliasing->thresh, DC_U, DC_U, DC_U, DC_U);
            amba_ik_system_print_uint32_5("[IK]    log_fractional_correct = %d", p_anti_aliasing->log_fractional_correct, DC_U, DC_U, DC_U, DC_U);
        }
    }
}

static void amba_ik_debug_print_grgb_mismatch(uint32 context_id, const ik_grgb_mismatch_t *p_grgb_mismatch)
{
    if (p_grgb_mismatch == NULL) {

    } else {
        if (filter_debug_log[LOG_GRGB_MISMATCH] == 1U) {
            amba_ik_system_print_str_5("[IK] %s",__func__, DC_S, DC_S, DC_S, DC_S);
            amba_ik_system_print_uint32_5("[IK](context_id = %d)", context_id, DC_U, DC_U, DC_U, DC_U);
            amba_ik_system_print_uint32_5("[IK]    narrow_enable = %d", p_grgb_mismatch->narrow_enable, DC_U, DC_U, DC_U, DC_U);
            amba_ik_system_print_uint32_5("[IK]    wide_enable = %d", p_grgb_mismatch->wide_enable, DC_U, DC_U, DC_U, DC_U);
            amba_ik_system_print_uint32_5("[IK]    wide_safety = %d", p_grgb_mismatch->wide_safety, DC_U, DC_U, DC_U, DC_U);
            amba_ik_system_print_uint32_5("[IK]    wide_thresh = %d", p_grgb_mismatch->wide_thresh, DC_U, DC_U, DC_U, DC_U);
        }
    }
}

static void amba_ik_debug_print_before_ce_wbgain(uint32 context_id, const ik_wb_gain_t *p_wb_dgain)
{
    if (p_wb_dgain==NULL) {

    } else {
        if (filter_debug_log[LOG_BEFORE_WB_GAIN] == 1U) {
            amba_ik_system_print_str_5("[IK] %s",__func__, DC_S, DC_S, DC_S, DC_S);
            amba_ik_system_print_uint32_5("[IK](context_id = %d)", context_id, DC_U, DC_U, DC_U, DC_U);
            amba_ik_system_print_uint32_5("[IK]    gain_r = %d", p_wb_dgain->gain_r, DC_U, DC_U, DC_U, DC_U);
            amba_ik_system_print_uint32_5("[IK]    gain_g = %d", p_wb_dgain->gain_g, DC_U, DC_U, DC_U, DC_U);
            amba_ik_system_print_uint32_5("[IK]    gain_b = %d", p_wb_dgain->gain_b, DC_U, DC_U, DC_U, DC_U);
        }
    }
}

static void amba_ik_debug_print_cawarp_enb(uint32 context_id, const uint32 enable)
{
    if (filter_debug_log[LOG_CAWARP_COMPENSATION] == 1U) {
        amba_ik_system_print_str_5("[IK] %s",__func__, DC_S, DC_S, DC_S, DC_S);
        amba_ik_system_print_uint32_5("[IK](context_id = %d)", context_id, DC_U, DC_U, DC_U, DC_U);
        amba_ik_system_print_uint32_5("[IK]    enable = %d", enable, DC_U, DC_U, DC_U, DC_U);
    }
}

static void amba_ik_debug_print_ce(uint32 context_id, const ik_ce_t *p_ce)
{
    if (p_ce==NULL) {

    } else {
        if (filter_debug_log[LOG_CONTRAST_ENHANCE] == 1U) {
            amba_ik_system_print_str_5("[IK] %s",__func__, DC_S, DC_S, DC_S, DC_S);
            amba_ik_system_print_uint32_5("[IK](context_id = %d)", context_id, DC_U, DC_U, DC_U, DC_U);
            amba_ik_system_print_uint32_5("[IK]    enable = %d", p_ce->enable, DC_U, DC_U, DC_U, DC_U);
            amba_ik_system_print_uint32_5("[IK]    luma_avg_method = %d", p_ce->luma_avg_method, DC_U, DC_U, DC_U, DC_U);
            amba_ik_system_print_int32_5("[IK]    luma_avg_weight_r = %d", p_ce->luma_avg_weight_r, DC_I, DC_I, DC_I, DC_I);
            amba_ik_system_print_int32_5("[IK]    luma_avg_weight_gr = %d", p_ce->luma_avg_weight_gr, DC_I, DC_I, DC_I, DC_I);
            amba_ik_system_print_int32_5("[IK]    luma_avg_weight_gb = %d", p_ce->luma_avg_weight_gb, DC_I, DC_I, DC_I, DC_I);
            amba_ik_system_print_int32_5("[IK]    luma_avg_weight_b = %d", p_ce->luma_avg_weight_b, DC_I, DC_I, DC_I, DC_I);
            amba_ik_system_print_uint32_5("[IK]    fir_enable = %d", p_ce->fir_enable, DC_U, DC_U, DC_U, DC_U);
            amba_ik_system_print_uint32_5("[IK]    fir_coeff = %d %d %d", p_ce->fir_coeff[0], p_ce->fir_coeff[1], p_ce->fir_coeff[2], DC_U, DC_U);
            amba_ik_system_print_uint32_5("[IK]    coring_index_scale_shift = %d", p_ce->coring_index_scale_shift, DC_U, DC_U, DC_U, DC_U);

            amba_ik_system_print_uint32_5("[IK]    coring_index_scale_low = %d", p_ce->coring_index_scale_low, DC_U, DC_U, DC_U, DC_U);
            amba_ik_system_print_uint32_5("[IK]    coring_index_scale_low_delta = %d", p_ce->coring_index_scale_low_delta, DC_U, DC_U, DC_U, DC_U);
            amba_ik_system_print_uint32_5("[IK]    coring_index_scale_low_strength = %d", p_ce->coring_index_scale_low_strength, DC_U, DC_U, DC_U, DC_U);
            amba_ik_system_print_uint32_5("[IK]    coring_index_scale_mid_strength = %d", p_ce->coring_index_scale_mid_strength, DC_U, DC_U, DC_U, DC_U);
            amba_ik_system_print_uint32_5("[IK]    coring_index_scale_high = %d", p_ce->coring_index_scale_high, DC_U, DC_U, DC_U, DC_U);
            amba_ik_system_print_uint32_5("[IK]    coring_index_scale_high_delta = %d", p_ce->coring_index_scale_high_delta, DC_U, DC_U, DC_U, DC_U);
            amba_ik_system_print_uint32_5("[IK]    coring_index_scale_high_strength = %d", p_ce->coring_index_scale_high_strength, DC_U, DC_U, DC_U, DC_U);

            amba_ik_system_print_uint32_5("[IK]    coring_gain_low = %d", p_ce->coring_gain_shift, DC_U, DC_U, DC_U, DC_U);
            amba_ik_system_print_uint32_5("[IK]    coring_gain_low = %d", p_ce->coring_gain_low, DC_U, DC_U, DC_U, DC_U);
            amba_ik_system_print_uint32_5("[IK]    coring_gain_low_delta = %d", p_ce->coring_gain_low_delta, DC_U, DC_U, DC_U, DC_U);
            amba_ik_system_print_uint32_5("[IK]    coring_gain_low_strength = %d", p_ce->coring_gain_low_strength, DC_U, DC_U, DC_U, DC_U);
            amba_ik_system_print_uint32_5("[IK]    coring_gain_mid_strength = %d", p_ce->coring_gain_mid_strength, DC_U, DC_U, DC_U, DC_U);
            amba_ik_system_print_uint32_5("[IK]    coring_gain_high = %d", p_ce->coring_gain_high, DC_U, DC_U, DC_U, DC_U);
            amba_ik_system_print_uint32_5("[IK]    coring_gain_high_delta = %d", p_ce->coring_gain_high_delta, DC_U, DC_U, DC_U, DC_U);
            amba_ik_system_print_uint32_5("[IK]    coring_gain_high_strength = %d", p_ce->coring_gain_high_strength, DC_U, DC_U, DC_U, DC_U);

            amba_ik_system_print_uint32_5("[IK]    coring_table = %d %d ... %d %d", p_ce->coring_table[0], p_ce->coring_table[1], \
                                          p_ce->coring_table[IK_CE_CORING_TABLE_SIZE-2U], \
                                          p_ce->coring_table[IK_CE_CORING_TABLE_SIZE-1U], DC_U);


            amba_ik_system_print_uint32_5("[IK]    boost_gain_shift = %d", p_ce->boost_gain_shift, DC_U, DC_U, DC_U, DC_U);
            amba_ik_system_print_int32_5("[IK]    boost_table = %d %d ... %d %d", p_ce->boost_table[0], p_ce->boost_table[1], \
                                         p_ce->boost_table[IK_CE_BOOST_TABLE_SIZE-2U], \
                                         p_ce->boost_table[IK_CE_BOOST_TABLE_SIZE-1U], DC_I);
            amba_ik_system_print_uint32_5("[IK]    boost_table_size_exp = %d", p_ce->boost_table_size_exp, DC_U, DC_U, DC_U, DC_U);
            amba_ik_system_print_uint32_5("[IK]    radius = %d", p_ce->radius, DC_U, DC_U, DC_U, DC_U);
            amba_ik_system_print_uint32_5("[IK]    epsilon = %d", p_ce->epsilon, DC_U, DC_U, DC_U, DC_U);
        }
    }
}

static void amba_ik_debug_print_ce_input_table(uint32 context_id, const ik_ce_input_table_t *p_ce_input_tbl)
{
    if (p_ce_input_tbl == NULL) {

    } else {
        if (filter_debug_log[LOG_CONTRAST_ENHANCE_IN_TABLE] == 1U) {
            amba_ik_system_print_str_5("[IK] %s",__func__, DC_S, DC_S, DC_S, DC_S);
            amba_ik_system_print_uint32_5("[IK](context_id = %d)", context_id, DC_U, DC_U, DC_U, DC_U);
            amba_ik_system_print_uint32_5("[IK]    input_enable = %d", p_ce_input_tbl->input_enable, DC_U, DC_U, DC_U, DC_U);
            amba_ik_system_print_uint32_5("[IK]    input_table = %d %d ... %d %d", p_ce_input_tbl->input_table[0], p_ce_input_tbl->input_table[1], \
                                          p_ce_input_tbl->input_table[IK_CE_INPUT_TABLE_SIZE-2U], \
                                          p_ce_input_tbl->input_table[IK_CE_INPUT_TABLE_SIZE-1U], DC_U);

        }
    }
}

static void amba_ik_debug_print_ce_output_table(uint32 context_id, const ik_ce_output_table_t *p_ce_output_tbl)
{
    if (p_ce_output_tbl == NULL) {

    } else {
        if (filter_debug_log[LOG_CONTRAST_ENHANCE_OUT_TABLE] == 1U) {
            amba_ik_system_print_str_5("[IK] %s",__func__, DC_S, DC_S, DC_S, DC_S);
            amba_ik_system_print_uint32_5("[IK](context_id = %d)", context_id, DC_U, DC_U, DC_U, DC_U);
            amba_ik_system_print_uint32_5("[IK]    output_enable = %d", p_ce_output_tbl->output_enable, DC_U, DC_U, DC_U, DC_U);
            amba_ik_system_print_uint32_5("[IK]    output_shift = %d", p_ce_output_tbl->output_shift, DC_U, DC_U, DC_U, DC_U);
            amba_ik_system_print_uint32_5("[IK]    output_table = %d %d ... %d %d", p_ce_output_tbl->output_table[0], p_ce_output_tbl->output_table[1], \
                                          p_ce_output_tbl->output_table[IK_CE_OUTPUT_TABLE_SIZE-2U], \
                                          p_ce_output_tbl->output_table[IK_CE_OUTPUT_TABLE_SIZE-1U], DC_U);
        }
    }
}

static void amba_ik_debug_print_after_ce_wbgain(uint32 context_id, const ik_wb_gain_t *p_wb_dgain)
{
    if (p_wb_dgain == NULL) {

    } else {
        if (filter_debug_log[LOG_AFTER_WB_GAIN] == 1U) {
            amba_ik_system_print_str_5("[IK] %s",__func__, DC_S, DC_S, DC_S, DC_S);
            amba_ik_system_print_uint32_5("[IK](context_id = %d)", context_id, DC_U, DC_U, DC_U, DC_U);
            amba_ik_system_print_uint32_5("[IK]    gain_r = %d", p_wb_dgain->gain_r, DC_U, DC_U, DC_U, DC_U);
            amba_ik_system_print_uint32_5("[IK]    gain_g = %d", p_wb_dgain->gain_g, DC_U, DC_U, DC_U, DC_U);
            amba_ik_system_print_uint32_5("[IK]    gain_b = %d", p_wb_dgain->gain_b, DC_U, DC_U, DC_U, DC_U);
        }
    }
}

static void amba_ik_debug_print_cfa_noise_filter(uint32 context_id, const ik_cfa_noise_filter_t *p_cfa_noise_filter)
{
    if (p_cfa_noise_filter == NULL) {

    } else {
        if (filter_debug_log[LOG_CFA_NOISE_FILTER] == 1U) {
            amba_ik_system_print_str_5("[IK] %s",__func__, DC_S, DC_S, DC_S, DC_S);
            amba_ik_system_print_uint32_5("[IK](context_id = %d)", context_id, DC_U, DC_U, DC_U, DC_U);
            amba_ik_system_print_uint32_5("[IK]    enable = %d", p_cfa_noise_filter->enable, DC_U, DC_U, DC_U, DC_U);
            amba_ik_system_print_uint32_5("[IK]    extent_fine = %d %d %d"\
                                          , p_cfa_noise_filter->extent_fine[0]\
                                          , p_cfa_noise_filter->extent_fine[1]\
                                          , p_cfa_noise_filter->extent_fine[2], DC_U, DC_U);
            amba_ik_system_print_uint32_5("[IK]    extent_regular = %d %d %d"\
                                          , p_cfa_noise_filter->extent_regular[0]\
                                          , p_cfa_noise_filter->extent_regular[1]\
                                          , p_cfa_noise_filter->extent_regular[2], DC_U, DC_U);
            amba_ik_system_print_uint32_5("[IK]    original_blend_str = %d %d %d"\
                                          , p_cfa_noise_filter->original_blend_str[0]\
                                          , p_cfa_noise_filter->original_blend_str[1]\
                                          , p_cfa_noise_filter->original_blend_str[2], DC_U, DC_U);
            amba_ik_system_print_uint32_5("[IK]    selectivity_fine = %d", p_cfa_noise_filter->selectivity_fine, DC_U, DC_U, DC_U, DC_U);
            amba_ik_system_print_uint32_5("[IK]    selectivity_regular = %d", p_cfa_noise_filter->selectivity_regular, DC_U, DC_U, DC_U, DC_U);
            amba_ik_system_print_uint32_5("[IK]    strength_fine = %d %d %d"\
                                          , p_cfa_noise_filter->strength_fine[0]\
                                          , p_cfa_noise_filter->strength_fine[1]\
                                          , p_cfa_noise_filter->strength_fine[2], DC_U, DC_U);
            amba_ik_system_print_uint32_5("[IK]    dir_lev_max = %d %d %d"\
                                          , p_cfa_noise_filter->dir_lev_max[0]\
                                          , p_cfa_noise_filter->dir_lev_max[1]\
                                          , p_cfa_noise_filter->dir_lev_max[2], DC_U, DC_U);
            amba_ik_system_print_uint32_5("[IK]    dir_lev_min = %d %d %d"\
                                          , p_cfa_noise_filter->dir_lev_min[0]\
                                          , p_cfa_noise_filter->dir_lev_min[1]\
                                          , p_cfa_noise_filter->dir_lev_min[2], DC_U, DC_U);
            amba_ik_system_print_uint32_5("[IK]    dir_lev_mul = %d %d %d"\
                                          , p_cfa_noise_filter->dir_lev_mul[0]\
                                          , p_cfa_noise_filter->dir_lev_mul[1]\
                                          , p_cfa_noise_filter->dir_lev_mul[2], DC_U, DC_U);
            amba_ik_system_print_int32_5("[IK]    dir_lev_offset = %d %d %d"\
                                         , p_cfa_noise_filter->dir_lev_offset[0]\
                                         , p_cfa_noise_filter->dir_lev_offset[1]\
                                         , p_cfa_noise_filter->dir_lev_offset[2], DC_I, DC_I);
            amba_ik_system_print_uint32_5("[IK]    dir_lev_shift = %d %d %d"\
                                          , p_cfa_noise_filter->dir_lev_shift[0]\
                                          , p_cfa_noise_filter->dir_lev_shift[1]\
                                          , p_cfa_noise_filter->dir_lev_shift[2], DC_U, DC_U);
            amba_ik_system_print_uint32_5("[IK]    lev_mul = %d %d %d"\
                                          , p_cfa_noise_filter->lev_mul[0]\
                                          , p_cfa_noise_filter->lev_mul[1]\
                                          , p_cfa_noise_filter->lev_mul[2], DC_U, DC_U);
            amba_ik_system_print_int32_5("[IK]    lev_offset = %d %d %d"\
                                         , p_cfa_noise_filter->lev_offset[0]\
                                         , p_cfa_noise_filter->lev_offset[1]\
                                         , p_cfa_noise_filter->lev_offset[2], DC_I, DC_I);
            amba_ik_system_print_uint32_5("[IK]    lev_shift = %d %d %d"\
                                          , p_cfa_noise_filter->lev_shift[0]\
                                          , p_cfa_noise_filter->lev_shift[1]\
                                          , p_cfa_noise_filter->lev_shift[2], DC_U, DC_U);
            amba_ik_system_print_uint32_5("[IK]    max_noise_level = %d %d %d"\
                                          , p_cfa_noise_filter->max_noise_level[0]\
                                          , p_cfa_noise_filter->max_noise_level[1]\
                                          , p_cfa_noise_filter->max_noise_level[2], DC_U, DC_U);
            amba_ik_system_print_uint32_5("[IK]    min_noise_level = %d %d %d"\
                                          , p_cfa_noise_filter->min_noise_level[0]\
                                          , p_cfa_noise_filter->min_noise_level[1]\
                                          , p_cfa_noise_filter->min_noise_level[2], DC_U, DC_U);
            amba_ik_system_print_uint32_5("[IK]    apply_to_color_diff_red = %d", p_cfa_noise_filter->apply_to_color_diff_red, DC_U, DC_U, DC_U, DC_U);
            amba_ik_system_print_uint32_5("[IK]    apply_to_color_diff_blue = %d", p_cfa_noise_filter->apply_to_color_diff_blue, DC_U, DC_U, DC_U, DC_U);
            amba_ik_system_print_uint32_5("[IK]    opt_for_rccb_red = %d", p_cfa_noise_filter->opt_for_rccb_red, DC_U, DC_U, DC_U, DC_U);
            amba_ik_system_print_uint32_5("[IK]    opt_for_rccb_blue = %d", p_cfa_noise_filter->opt_for_rccb_blue, DC_U, DC_U, DC_U, DC_U);
            amba_ik_system_print_uint32_5("[IK]    directional_enable = %d", p_cfa_noise_filter->directional_enable, DC_U, DC_U, DC_U, DC_U);
            amba_ik_system_print_uint32_5("[IK]    directional_horvert_edge_strength = %d", p_cfa_noise_filter->directional_horvert_edge_strength, DC_U, DC_U, DC_U, DC_U);
            amba_ik_system_print_uint32_5("[IK]    directional_horvert_edge_strength_bias = %d", p_cfa_noise_filter->directional_horvert_strength_bias, DC_U, DC_U, DC_U, DC_U);
            amba_ik_system_print_uint32_5("[IK]    directional_horvert_original_blend_strength = %d", p_cfa_noise_filter->directional_horvert_original_blend_strength, DC_U, DC_U, DC_U, DC_U);
        }
    }
}

static void amba_ik_debug_print_demosaic(uint32 context_id, const ik_demosaic_t *p_demosaic)
{
    if (p_demosaic == NULL) {

    } else {
        if (filter_debug_log[LOG_DEMOSAIC] == 1U) {
            amba_ik_system_print_str_5("[IK] %s",__func__, DC_S, DC_S, DC_S, DC_S);
            amba_ik_system_print_uint32_5("[IK](context_id = %d)", context_id, DC_U, DC_U, DC_U, DC_U);
            amba_ik_system_print_uint32_5("[IK]    enable = %d", p_demosaic->enable, DC_U, DC_U, DC_U, DC_U);
            amba_ik_system_print_uint32_5("[IK]    activity_thresh = %d", p_demosaic->activity_thresh, DC_U, DC_U, DC_U, DC_U);
            amba_ik_system_print_uint32_5("[IK]    activity_difference_thresh = %d", p_demosaic->activity_difference_thresh, DC_U, DC_U, DC_U, DC_U);
            amba_ik_system_print_uint32_5("[IK]    grad_clip_thresh = %d", p_demosaic->grad_clip_thresh, DC_U, DC_U, DC_U, DC_U);
            amba_ik_system_print_uint32_5("[IK]    grad_noise_thresh = %d", p_demosaic->grad_noise_thresh, DC_U, DC_U, DC_U, DC_U);
            amba_ik_system_print_uint32_5("[IK]    alias_interpolation_strength = %d", p_demosaic->alias_interpolation_strength, DC_U, DC_U, DC_U, DC_U);
            amba_ik_system_print_uint32_5("[IK]    alias_interpolation_thresh = %d", p_demosaic->alias_interpolation_thresh, DC_U, DC_U, DC_U, DC_U);
        }
    }
}

static void amba_ik_debug_print_rgb_to_12y(uint32 context_id, const ik_rgb_to_12y_t *p_rgb_to_12y)
{
    if (p_rgb_to_12y == NULL) {

    } else {
        if (filter_debug_log[LOG_RGB_TO_12Y] == 1U) {
            amba_ik_system_print_str_5("[IK] %s",__func__, DC_S, DC_S, DC_S, DC_S);
            amba_ik_system_print_uint32_5("[IK](context_id = %d)", context_id, DC_U, DC_U, DC_U, DC_U);
            amba_ik_system_print_uint32_5("[IK]    enable = %d", p_rgb_to_12y->enable, DC_U, DC_U, DC_U, DC_U);
            amba_ik_system_print_int32_5("[IK]    matrix_values[0] = %d", p_rgb_to_12y->matrix_values[0], DC_I, DC_I, DC_I, DC_I);
            amba_ik_system_print_int32_5("[IK]    matrix_values[1] = %d", p_rgb_to_12y->matrix_values[1], DC_I, DC_I, DC_I, DC_I);
            amba_ik_system_print_int32_5("[IK]    matrix_values[2] = %d", p_rgb_to_12y->matrix_values[2], DC_I, DC_I, DC_I, DC_I);
            amba_ik_system_print_int32_5("[IK]    y_offset = %d", p_rgb_to_12y->y_offset, DC_I, DC_I, DC_I, DC_I);
        }
    }
}

static void amba_ik_debug_print_luma_noise_reduction(uint32 context_id, const ik_luma_noise_reduction_t *p_luma_noise_reduce)
{
    if (p_luma_noise_reduce == NULL) {

    } else {
        if (filter_debug_log[LOG_LUMA_NOISE_REDUCTION] == 1U) {
            amba_ik_system_print_str_5("[IK] %s",__func__, DC_S, DC_S, DC_S, DC_S);
            amba_ik_system_print_uint32_5("[IK](context_id = %d)", context_id, DC_U, DC_U, DC_U, DC_U);
            amba_ik_system_print_uint32_5("[IK]    enable = %d", p_luma_noise_reduce->enable, DC_U, DC_U, DC_U, DC_U);
            amba_ik_system_print_uint32_5("[IK]    sensor_wb_b = %d", p_luma_noise_reduce->sensor_wb_b, DC_U, DC_U, DC_U, DC_U);
            amba_ik_system_print_uint32_5("[IK]    sensor_wb_g = %d", p_luma_noise_reduce->sensor_wb_g, DC_U, DC_U, DC_U, DC_U);
            amba_ik_system_print_uint32_5("[IK]    sensor_wb_r = %d", p_luma_noise_reduce->sensor_wb_r, DC_U, DC_U, DC_U, DC_U);
            amba_ik_system_print_uint32_5("[IK]    strength0 = %d", p_luma_noise_reduce->strength0, DC_U, DC_U, DC_U, DC_U);
            amba_ik_system_print_uint32_5("[IK]    strength1 = %d", p_luma_noise_reduce->strength1, DC_U, DC_U, DC_U, DC_U);
            amba_ik_system_print_uint32_5("[IK]    strength2 = %d", p_luma_noise_reduce->strength2, DC_U, DC_U, DC_U, DC_U);
            amba_ik_system_print_uint32_5("[IK]    strength2_max_change = %d", p_luma_noise_reduce->strength2_max_change, DC_U, DC_U, DC_U, DC_U);
        }
    }
}

static void amba_ik_debug_print_rgb_ir(uint32 context_id, const ik_rgb_ir_t *p_rgb_ir)
{
    if (p_rgb_ir == NULL) {

    } else {
        if (filter_debug_log[LOG_RGB_IR] == 1U) {
            amba_ik_system_print_str_5("[IK] %s",__func__, DC_S, DC_S, DC_S, DC_S);
            amba_ik_system_print_uint32_5("[IK]    (context_id = %d)", context_id, DC_U, DC_U, DC_U, DC_U);
            amba_ik_system_print_uint32_5("[IK]    mode = %d", p_rgb_ir->mode, DC_U, DC_U, DC_U, DC_U);
            amba_ik_system_print_int32_5("[IK]    ircorrect_offset_b = %d", p_rgb_ir->ircorrect_offset_b, DC_I, DC_I, DC_I, DC_I);
            amba_ik_system_print_int32_5("[IK]    ircorrect_offset_gr = %d", p_rgb_ir->ircorrect_offset_gr, DC_I, DC_I, DC_I, DC_I);
            amba_ik_system_print_int32_5("[IK]    ircorrect_offset_gb = %d", p_rgb_ir->ircorrect_offset_gb, DC_I, DC_I, DC_I, DC_I);
            amba_ik_system_print_int32_5("[IK]    ircorrect_offset_r = %d", p_rgb_ir->ircorrect_offset_r, DC_I, DC_I, DC_I, DC_I);
            amba_ik_system_print_uint32_5("[IK]    mul_base_val = %d", p_rgb_ir->mul_base_val, DC_U, DC_U, DC_U, DC_U);
            amba_ik_system_print_uint32_5("[IK]    mul_delta_high = %d", p_rgb_ir->mul_delta_high, DC_U, DC_U, DC_U, DC_U);
            amba_ik_system_print_uint32_5("[IK]    mul_high = %d", p_rgb_ir->mul_high, DC_U, DC_U, DC_U, DC_U);
            amba_ik_system_print_uint32_5("[IK]    mul_high_val = %d", p_rgb_ir->mul_high_val, DC_U, DC_U, DC_U, DC_U);
            amba_ik_system_print_uint32_5("[IK]    ir_only = %d", p_rgb_ir->ir_only, DC_U, DC_U, DC_U, DC_U);

//            amba_ik_system_print_uint32_5("[IK]    per_color_mul = %d", p_rgb_ir->per_color_mul, DC_U, DC_U, DC_U, DC_U);
//            amba_ik_system_print_uint32_5("[IK]    mul_high_r = %d", p_rgb_ir->mul_high_r, DC_U, DC_U, DC_U, DC_U);
//            amba_ik_system_print_uint32_5("[IK]    mul_delta_high_r = %d", p_rgb_ir->mul_delta_high_r, DC_U, DC_U, DC_U, DC_U);
//            amba_ik_system_print_uint32_5("[IK]    mul_base_val_r = %d", p_rgb_ir->mul_base_val_r, DC_U, DC_U, DC_U, DC_U);
//            amba_ik_system_print_uint32_5("[IK]    mul_high_val_r = %d", p_rgb_ir->mul_high_val_r, DC_U, DC_U, DC_U, DC_U);
//            amba_ik_system_print_uint32_5("[IK]    mul_high_gr = %d", p_rgb_ir->mul_high_gr, DC_U, DC_U, DC_U, DC_U);
//            amba_ik_system_print_uint32_5("[IK]    mul_delta_high_gr = %d", p_rgb_ir->mul_delta_high_gr, DC_U, DC_U, DC_U, DC_U);
//            amba_ik_system_print_uint32_5("[IK]    mul_base_val_gr = %d", p_rgb_ir->mul_base_val_gr, DC_U, DC_U, DC_U, DC_U);
//            amba_ik_system_print_uint32_5("[IK]    mul_high_val_gr = %d", p_rgb_ir->mul_high_val_gr, DC_U, DC_U, DC_U, DC_U);
//            amba_ik_system_print_uint32_5("[IK]    mul_high_gb = %d", p_rgb_ir->mul_high_gb, DC_U, DC_U, DC_U, DC_U);
//            amba_ik_system_print_uint32_5("[IK]    mul_delta_high_gb = %d", p_rgb_ir->mul_delta_high_gb, DC_U, DC_U, DC_U, DC_U);
//            amba_ik_system_print_uint32_5("[IK]    mul_base_val_gb = %d", p_rgb_ir->mul_base_val_gb, DC_U, DC_U, DC_U, DC_U);
//            amba_ik_system_print_uint32_5("[IK]    mul_high_val_gb = %d", p_rgb_ir->mul_high_val_gb, DC_U, DC_U, DC_U, DC_U);
//            amba_ik_system_print_uint32_5("[IK]    mul_high_b = %d", p_rgb_ir->mul_high_b, DC_U, DC_U, DC_U, DC_U);
//            amba_ik_system_print_uint32_5("[IK]    mul_delta_high_b = %d", p_rgb_ir->mul_delta_high_b, DC_U, DC_U, DC_U, DC_U);
//            amba_ik_system_print_uint32_5("[IK]    mul_base_val_b = %d", p_rgb_ir->mul_base_val_b, DC_U, DC_U, DC_U, DC_U);
//            amba_ik_system_print_uint32_5("[IK]    mul_high_val_b = %d", p_rgb_ir->mul_high_val_b, DC_U, DC_U, DC_U, DC_U);
        }
    }
}

static void amba_ik_debug_print_pre_cc_gain(uint32 context_id, const ik_pre_cc_gain_t *p_pre_cc)
{
    if (p_pre_cc == NULL) {

    } else {
        if (filter_debug_log[LOG_PRE_CC_GAIN] == 1U) {
            amba_ik_system_print_str_5("[IK] %s", __func__, DC_S, DC_S, DC_S, DC_S);
            amba_ik_system_print_uint32_5("[IK](context_id = %d)", context_id, DC_U, DC_U, DC_U, DC_U);
            amba_ik_system_print_uint32_5("[IK]    enable = %d", p_pre_cc->enable, DC_U, DC_U, DC_U, DC_U);
            amba_ik_system_print_uint32_5("[IK]    gain_r = %d", p_pre_cc->gain_r, DC_U, DC_U, DC_U, DC_U);;
            amba_ik_system_print_uint32_5("[IK]    gain_g = %d", p_pre_cc->gain_g, DC_U, DC_U, DC_U, DC_U);;
            amba_ik_system_print_uint32_5("[IK]    gain_b = %d", p_pre_cc->gain_b, DC_U, DC_U, DC_U, DC_U);;
        }
    }
}

static void amba_ik_debug_print_cc(uint32 context_id, const ik_color_correction_t *p_cc)
{
    if (p_cc == NULL) {

    } else {
        if (filter_debug_log[LOG_COLOR_CORRECTION] == 1U) {
            amba_ik_system_print_str_5("[IK] %s",__func__, DC_S, DC_S, DC_S, DC_S);
            amba_ik_system_print_uint32_5("[IK](context_id = %d)", context_id, DC_U, DC_U, DC_U, DC_U);
            amba_ik_system_print_uint32_5("[IK]    matrix_en = %d", p_cc->matrix_en, DC_U, DC_U, DC_U, DC_U);
            amba_ik_system_print_uint32_5("[IK]    matrix_shift_minus_8 = %d", p_cc->matrix_shift_minus_8, DC_U, DC_U, DC_U, DC_U);
            amba_ik_system_print_int32_5("[IK]    matrix[0~2] = %d %d %d", p_cc->matrix[0], p_cc->matrix[1], p_cc->matrix[2], DC_I, DC_I);
            amba_ik_system_print_int32_5("[IK]    matrix[3~5] = %d %d %d", p_cc->matrix[3], p_cc->matrix[4], p_cc->matrix[5], DC_I, DC_I);
            amba_ik_system_print_uint32_5("[IK]    matrix_three_d_table = %d %d %d ..." \
                                          , p_cc->matrix_three_d_table[0] \
                                          , p_cc->matrix_three_d_table[1] \
                                          , p_cc->matrix_three_d_table[2], DC_U, DC_U);
        }
    }
}

static void amba_ik_debug_print_tone_curve(uint32 context_id, const ik_tone_curve_t *p_tc)
{
    if (p_tc == NULL) {

    } else {
        if (filter_debug_log[LOG_TONE_CURVE] == 1U) {
            amba_ik_system_print_str_5("[IK] %s",__func__, DC_S, DC_S, DC_S, DC_S);
            amba_ik_system_print_uint32_5("[IK](context_id = %d)", context_id, DC_U, DC_U, DC_U, DC_U);
            amba_ik_system_print_uint32_5("[IK]    tone_curve_red = %d %d ... %d %d %d" \
                                          , p_tc->tone_curve_red[0] \
                                          , p_tc->tone_curve_red[1] \
                                          , p_tc->tone_curve_red[IK_NUM_TONE_CURVE-3U] \
                                          , p_tc->tone_curve_red[IK_NUM_TONE_CURVE-2U] \
                                          , p_tc->tone_curve_red[IK_NUM_TONE_CURVE-1U]);
            amba_ik_system_print_uint32_5("[IK]    tone_curve_green = %d %d ... %d %d %d" \
                                          , p_tc->tone_curve_green[0] \
                                          , p_tc->tone_curve_green[1] \
                                          , p_tc->tone_curve_green[IK_NUM_TONE_CURVE-3U] \
                                          , p_tc->tone_curve_green[IK_NUM_TONE_CURVE-2U] \
                                          , p_tc->tone_curve_green[IK_NUM_TONE_CURVE-1U]);
            amba_ik_system_print_uint32_5("[IK]    tone_curve_blue = %d %d ... %d %d %d" \
                                          , p_tc->tone_curve_blue[0] \
                                          , p_tc->tone_curve_blue[1] \
                                          , p_tc->tone_curve_blue[IK_NUM_TONE_CURVE-3U] \
                                          , p_tc->tone_curve_blue[IK_NUM_TONE_CURVE-2U] \
                                          , p_tc->tone_curve_blue[IK_NUM_TONE_CURVE-1U]);
        }
    }
}

static void amba_ik_debug_print_rgb_to_yuv_matrix(uint32 context_id, const ik_rgb_to_yuv_matrix_t *p_rgb_to_yuv_matrix)
{
    if (p_rgb_to_yuv_matrix == NULL) {

    } else {
        if (filter_debug_log[LOG_RGB_TO_YUV_MATRIX] == 1U) {
            amba_ik_system_print_str_5("[IK] %s",__func__, DC_S, DC_S, DC_S, DC_S);
            amba_ik_system_print_uint32_5("[IK](context_id = %d)", context_id, DC_U, DC_U, DC_U, DC_U);
            amba_ik_system_print_int32_5("[IK]    matrix_values = %d %d %d %d %d" \
                                         , p_rgb_to_yuv_matrix->matrix_values[0] \
                                         , p_rgb_to_yuv_matrix->matrix_values[1] \
                                         , p_rgb_to_yuv_matrix->matrix_values[2] \
                                         , p_rgb_to_yuv_matrix->matrix_values[3] \
                                         , p_rgb_to_yuv_matrix->matrix_values[4]);
            amba_ik_system_print_int32_5("[IK]    %d %d %d %d " \
                                         , p_rgb_to_yuv_matrix->matrix_values[5] \
                                         , p_rgb_to_yuv_matrix->matrix_values[6] \
                                         , p_rgb_to_yuv_matrix->matrix_values[7] \
                                         , p_rgb_to_yuv_matrix->matrix_values[8], DC_I);
            amba_ik_system_print_int32_5("[IK]    y_offset = %d", p_rgb_to_yuv_matrix->y_offset, DC_I, DC_I, DC_I, DC_I);
            amba_ik_system_print_int32_5("[IK]    u_offset = %d", p_rgb_to_yuv_matrix->u_offset, DC_I, DC_I, DC_I, DC_I);
            amba_ik_system_print_int32_5("[IK]    v_offset = %d", p_rgb_to_yuv_matrix->v_offset, DC_I, DC_I, DC_I, DC_I);
        }
    }
}

static void amba_ik_debug_print_chroma_scale(uint32 context_id, const ik_chroma_scale_t *p_chroma_scale)
{
    if (p_chroma_scale == NULL) {

    } else {
        if (filter_debug_log[LOG_CHROMA_SCALE] == 1U) {
            amba_ik_system_print_str_5("[IK] %s",__func__, DC_S, DC_S, DC_S, DC_S);
            amba_ik_system_print_uint32_5("[IK](context_id = %d)", context_id, DC_U, DC_U, DC_U, DC_U);
            amba_ik_system_print_uint32_5("[IK]    enable = %d", p_chroma_scale->enable, DC_U, DC_U, DC_U, DC_U);
            amba_ik_system_print_uint32_5("[IK]    gain_curve = %d %d ... %d %d %d" \
                                          , p_chroma_scale->gain_curve[0] \
                                          , p_chroma_scale->gain_curve[1] \
                                          , p_chroma_scale->gain_curve[IK_NUM_CHROMA_GAIN_CURVE-3U] \
                                          , p_chroma_scale->gain_curve[IK_NUM_CHROMA_GAIN_CURVE-2U] \
                                          , p_chroma_scale->gain_curve[IK_NUM_CHROMA_GAIN_CURVE-1U]);
        }
    }
}

static void amba_ik_debug_print_chroma_filter(uint32 context_id, const ik_chroma_filter_t *p_chroma_filter)
{
    if (p_chroma_filter == NULL) {

    } else {
        if (filter_debug_log[LOG_CHROMA_FILTER] == 1U) {
            amba_ik_system_print_str_5("[IK] %s",__func__, DC_S, DC_S, DC_S, DC_S);
            amba_ik_system_print_uint32_5("[IK](context_id = %d)", context_id, DC_U, DC_U, DC_U, DC_U);
            amba_ik_system_print_uint32_5("[IK]    enable = %d", p_chroma_filter->enable, DC_U, DC_U, DC_U, DC_U);
            amba_ik_system_print_uint32_5("[IK]    noise_level_cb = %d", p_chroma_filter->noise_level_cb, DC_U, DC_U, DC_U, DC_U);
            amba_ik_system_print_uint32_5("[IK]    noise_level_cr = %d", p_chroma_filter->noise_level_cr, DC_U, DC_U, DC_U, DC_U);
            amba_ik_system_print_uint32_5("[IK]    original_blend_strength_cb = %d", p_chroma_filter->original_blend_strength_cb, DC_U, DC_U, DC_U, DC_U);
            amba_ik_system_print_uint32_5("[IK]    original_blend_strength_cr = %d", p_chroma_filter->original_blend_strength_cr, DC_U, DC_U, DC_U, DC_U);
            amba_ik_system_print_uint32_5("[IK]    radius = %d", p_chroma_filter->radius, DC_U, DC_U, DC_U, DC_U);
        }
    }
}

static void amba_ik_debug_print_wide_chroma_filter(uint32 context_id, const ik_wide_chroma_filter_t *p_wide_chroma_filter)
{
    if (p_wide_chroma_filter == NULL) {

    } else {
        if (filter_debug_log[LOG_WIDE_CHROMA_FILTER] == 1U) {
            amba_ik_system_print_str_5("[IK] %s",__func__, DC_S, DC_S, DC_S, DC_S);
            amba_ik_system_print_uint32_5("[IK](context_id = %d)", context_id, DC_U, DC_U, DC_U, DC_U);
            amba_ik_system_print_uint32_5("[IK]    enable = %d", p_wide_chroma_filter->enable, DC_U, DC_U, DC_U, DC_U);
            amba_ik_system_print_uint32_5("[IK]    noise_level_cb = %d", p_wide_chroma_filter->noise_level_cb, DC_U, DC_U, DC_U, DC_U);
            amba_ik_system_print_uint32_5("[IK]    noise_level_cr = %d", p_wide_chroma_filter->noise_level_cr, DC_U, DC_U, DC_U, DC_U);
        }
    }
}

static void amba_ik_debug_print_wide_uv_combine(uint32 context_id, const ik_wide_chroma_filter_combine_t *p_chroma_filter_combine)
{
    if (p_chroma_filter_combine == NULL) {

    } else {
        if (filter_debug_log[LOG_WIDE_CHROMA_COMBINE] == 1U) {
            amba_ik_system_print_str_5("[IK] %s",__func__, DC_S, DC_S, DC_S, DC_S);
            amba_ik_system_print_uint32_5("[IK](context_id = %d)", context_id, DC_U, DC_U, DC_U, DC_U);
            amba_ik_system_print_uint32_5("[IK]    T0_cb = %d", p_chroma_filter_combine->T0_cb, DC_U, DC_U, DC_U, DC_U);
            amba_ik_system_print_uint32_5("[IK]    T0_cr = %d", p_chroma_filter_combine->T0_cr, DC_U, DC_U, DC_U, DC_U);
            amba_ik_system_print_uint32_5("[IK]    T1_cb = %d", p_chroma_filter_combine->T1_cb, DC_U, DC_U, DC_U, DC_U);
            amba_ik_system_print_uint32_5("[IK]    alpha_max_cb = %d", p_chroma_filter_combine->alpha_max_cb, DC_U, DC_U, DC_U, DC_U);
            amba_ik_system_print_uint32_5("[IK]    alpha_max_cr = %d", p_chroma_filter_combine->alpha_max_cr, DC_U, DC_U, DC_U, DC_U);
            amba_ik_system_print_uint32_5("[IK]    alpha_min_cb = %d", p_chroma_filter_combine->alpha_min_cb, DC_U, DC_U, DC_U, DC_U);
            amba_ik_system_print_uint32_5("[IK]    alpha_min_cr = %d", p_chroma_filter_combine->alpha_min_cr, DC_U, DC_U, DC_U, DC_U);
            amba_ik_system_print_uint32_5("[IK]    max_change_cb = %d", p_chroma_filter_combine->max_change_cb, DC_U, DC_U, DC_U, DC_U);
            amba_ik_system_print_uint32_5("[IK]    max_change_cr = %d", p_chroma_filter_combine->max_change_cr, DC_U, DC_U, DC_U, DC_U);
        }
    }
}

static void amba_ik_debug_print_chroma_median_filter(uint32 context_id, const ik_chroma_median_filter_t *p_chroma_median_filter)
{
    if (p_chroma_median_filter == NULL) {

    } else {
        if (filter_debug_log[LOG_CHROMA_MEDIAN_FILTER] == 1U) {
            amba_ik_system_print_str_5("[IK] %s",__func__, DC_S, DC_S, DC_S, DC_S);
            amba_ik_system_print_uint32_5("[IK](context_id = %d)", context_id, DC_U, DC_U, DC_U, DC_U);
            amba_ik_system_print_uint32_5("[IK]    enable = %d", (uint32)p_chroma_median_filter->enable, DC_U, DC_U, DC_U, DC_U);
            amba_ik_system_print_uint32_5("[IK]    cb_adaptive_strength = %d", p_chroma_median_filter->cb_adaptive_strength, DC_U, DC_U, DC_U, DC_U);
            amba_ik_system_print_uint32_5("[IK]    cr_adaptive_strength = %d", p_chroma_median_filter->cr_adaptive_strength, DC_U, DC_U, DC_U, DC_U);
            amba_ik_system_print_uint32_5("[IK]    cb_non_adaptive_strength = %d", p_chroma_median_filter->cb_non_adaptive_strength, DC_U, DC_U, DC_U, DC_U);
            amba_ik_system_print_uint32_5("[IK]    cr_non_adaptive_strength = %d", p_chroma_median_filter->cr_non_adaptive_strength, DC_U, DC_U, DC_U, DC_U);
            amba_ik_system_print_uint32_5("[IK]    cb_adaptive_amount = %d", p_chroma_median_filter->cb_adaptive_amount, DC_U, DC_U, DC_U, DC_U);
            amba_ik_system_print_uint32_5("[IK]    cr_adaptive_amount = %d", p_chroma_median_filter->cr_adaptive_amount, DC_U, DC_U, DC_U, DC_U);
        }
    }
}

static void amba_ik_debug_print_first_luma_proc_mode(uint32 context_id, const ik_first_luma_process_mode_t *p_first_proc_mode)
{
    if (p_first_proc_mode == NULL) {

    } else {
        if (filter_debug_log[LOG_FIRST_LUMA_PROCESSING_MODE] == 1U) {
            amba_ik_system_print_str_5("[IK] %s",__func__, DC_S, DC_S, DC_S, DC_S);
            amba_ik_system_print_uint32_5("[IK](context_id = %d)", context_id, DC_U, DC_U, DC_U, DC_U);
            amba_ik_system_print_uint32_5("[IK]    use_sharpen_not_asf = %d", p_first_proc_mode->use_sharpen_not_asf, DC_U, DC_U, DC_U, DC_U);
        }
    }
}

static void amba_ik_debug_print_level(const ik_level_t *p_lvl)
{
    amba_ik_system_print_uint32_5("[IK]    p_lvl->low = %d", p_lvl->low, DC_U, DC_U, DC_U, DC_U);
    amba_ik_system_print_uint32_5("[IK]    p_lvl->low_delta = %d", p_lvl->low_delta, DC_U, DC_U, DC_U, DC_U);
    amba_ik_system_print_uint32_5("[IK]    p_lvl->low_strength = %d", p_lvl->low_strength, DC_U, DC_U, DC_U, DC_U);
    amba_ik_system_print_uint32_5("[IK]    p_lvl->mid_strength = %d", p_lvl->mid_strength, DC_U, DC_U, DC_U, DC_U);
    amba_ik_system_print_uint32_5("[IK]    p_lvl->high = %d", p_lvl->high, DC_U, DC_U, DC_U, DC_U);
    amba_ik_system_print_uint32_5("[IK]    p_lvl->high_delta = %d", p_lvl->high_delta, DC_U, DC_U, DC_U, DC_U);
    amba_ik_system_print_uint32_5("[IK]    p_lvl->high_strength = %d", p_lvl->high_strength, DC_U, DC_U, DC_U, DC_U);
}

static void amba_ik_debug_print_fir(const ik_asf_fir_t *p_fir)
{
    int32 i, j;

    amba_ik_system_print_uint32_5("[IK]    fir->specify = %d", p_fir->specify, DC_U, DC_U, DC_U, DC_U);
    amba_ik_system_print_str_5("[IK]    fir->per_dir_fir_iso_strengths = ", DC_S, DC_S, DC_S, DC_S, DC_S);
    for (i = 0; i < 9; i++) {
        amba_ik_system_print_uint32_5("%d ", p_fir->per_dir_fir_iso_strengths[i], DC_U, DC_U, DC_U, DC_U);
    }
    amba_ik_system_print_str_5("", DC_S, DC_S, DC_S, DC_S, DC_S);
    amba_ik_system_print_str_5("[IK]    fir->per_dir_fir_dir_strengths = ", DC_S, DC_S, DC_S, DC_S, DC_S);
    for (i = 0; i < 9; i++) {
        amba_ik_system_print_uint32_5("%d ", p_fir->per_dir_fir_dir_strengths[i], DC_U, DC_U, DC_U, DC_U);
    }
    amba_ik_system_print_str_5("", DC_S, DC_S, DC_S, DC_S, DC_S);
    amba_ik_system_print_str_5("[IK]    fir->per_dir_fir_dir_amounts = ", DC_S, DC_S, DC_S, DC_S, DC_S);
    for (i = 0; i < 9; i++) {
        amba_ik_system_print_uint32_5("%d ", p_fir->per_dir_fir_dir_amounts[i], DC_U, DC_U, DC_U, DC_U);
    }
    amba_ik_system_print_str_5("", DC_S, DC_S, DC_S, DC_S, DC_S);
    amba_ik_system_print_str_5("[IK]    fir->coefs = ", DC_S, DC_S, DC_S, DC_S, DC_S);
    for (j = 0; j < 9; j++) {
        for (i = 0; i < 25; i++) {
            amba_ik_system_print_int32_5("%d ", p_fir->coefs[j][i], DC_I, DC_I, DC_I, DC_I);
        }
        amba_ik_system_print_str_5("", DC_S, DC_S, DC_S, DC_S, DC_S);
    }
    amba_ik_system_print_uint32_5("[IK]    fir->strength_iso = %d", p_fir->strength_iso, DC_U, DC_U, DC_U, DC_U);
    amba_ik_system_print_uint32_5("[IK]    fir->strength_dir = %d", p_fir->strength_dir, DC_U, DC_U, DC_U, DC_U);
    amba_ik_system_print_uint32_5("[IK]    fir->wide_edge_detect = %d", p_fir->wide_edge_detect, DC_U, DC_U, DC_U, DC_U);
}

static void amba_ik_debug_print_adv_spat_fltr(uint32 context_id, const ik_adv_spatial_filter_t *p_asf)
{
    if (p_asf == NULL) {
        // fix compile error unused parameter
    } else {
        if (filter_debug_log[LOG_ADVANCE_SPATIAL_FILTER] == 1U) {
            amba_ik_system_print_str_5("[IK] %s",__func__, DC_S, DC_S, DC_S, DC_S);
            amba_ik_system_print_uint32_5("[IK](context_id = %d)", context_id, DC_U, DC_U, DC_U, DC_U);
            amba_ik_system_print_uint32_5("[IK]    enable = %d", p_asf->enable, DC_U, DC_U, DC_U, DC_U);
            amba_ik_system_print_str_5("[IK]    fir :", DC_S, DC_S, DC_S, DC_S, DC_S);
            amba_ik_debug_print_fir(&p_asf->fir);
            amba_ik_system_print_uint32_5("[IK]    directional_decide_t0 = %d", p_asf->directional_decide_t0, DC_U, DC_U, DC_U, DC_U);
            amba_ik_system_print_uint32_5("[IK]    directional_decide_t0 = %d", p_asf->directional_decide_t0, DC_U, DC_U, DC_U, DC_U);
            amba_ik_system_print_uint32_5("[IK]    adapt->alpha_min_up = %d", p_asf->adapt.alpha_min_up, DC_U, DC_U, DC_U, DC_U);
            amba_ik_system_print_uint32_5("[IK]    adapt->alpha_max_up = %d", p_asf->adapt.alpha_max_up, DC_U, DC_U, DC_U, DC_U);
            amba_ik_system_print_uint32_5("[IK]    adapt->t0_up = %d", p_asf->adapt.t0_up, DC_U, DC_U, DC_U, DC_U);
            amba_ik_system_print_uint32_5("[IK]    adapt->t1_up = %d", p_asf->adapt.t1_up, DC_U, DC_U, DC_U, DC_U);
            amba_ik_system_print_uint32_5("[IK]    adapt->alpha_min_down = %d", p_asf->adapt.alpha_min_down, DC_U, DC_U, DC_U, DC_U);
            amba_ik_system_print_uint32_5("[IK]    adapt->alpha_max_down = %d", p_asf->adapt.alpha_max_down, DC_U, DC_U, DC_U, DC_U);
            amba_ik_system_print_uint32_5("[IK]    adapt->t0_down = %d", p_asf->adapt.t0_down, DC_U, DC_U, DC_U, DC_U);
            amba_ik_system_print_uint32_5("[IK]    adapt->t1_down = %d", p_asf->adapt.t1_down, DC_U, DC_U, DC_U, DC_U);
            amba_ik_system_print_str_5("[IK]    level_str_adjust :", DC_S, DC_S, DC_S, DC_S, DC_S);
            amba_ik_debug_print_level(&p_asf->level_str_adjust);
            amba_ik_system_print_str_5("[IK]    t0_t1_div :", DC_S, DC_S, DC_S, DC_S, DC_S);
            amba_ik_debug_print_level(&p_asf->t0_t1_div);
            amba_ik_system_print_uint32_5("[IK]    max_change_not_T0T1_alpha = %d", p_asf->max_change_not_t0_t1_alpha, DC_U, DC_U, DC_U, DC_U);
            amba_ik_system_print_uint32_5("[IK]    max_change_up = %d", p_asf->max_change_up, DC_U, DC_U, DC_U, DC_U);
            amba_ik_system_print_uint32_5("[IK]    max_change_down = %d", p_asf->max_change_down, DC_U, DC_U, DC_U, DC_U);
        }
    }
}

static void amba_ik_debug_print_fst_shp_ns_both(uint32 context_id, const ik_first_sharpen_both_t *p_first_sharpen_both)
{
    if (p_first_sharpen_both==NULL) {

    } else {
        if (filter_debug_log[LOG_FIRST_SHARPEN_NOISE_BOTH] == 1U) {
            amba_ik_system_print_str_5("[IK] %s",__func__, DC_S, DC_S, DC_S, DC_S);
            amba_ik_system_print_uint32_5("[IK](context_id = %d)", context_id, DC_U, DC_U, DC_U, DC_U);
            amba_ik_system_print_uint32_5("[IK]    enable = %d", p_first_sharpen_both->enable, DC_U, DC_U, DC_U, DC_U);
            amba_ik_system_print_uint32_5("[IK]    mode = %d", p_first_sharpen_both->mode, DC_U, DC_U, DC_U, DC_U);
            amba_ik_system_print_uint32_5("[IK]    edge_thresh = %d", p_first_sharpen_both->edge_thresh, DC_U, DC_U, DC_U, DC_U);
            amba_ik_system_print_uint32_5("[IK]    wide_edge_detect = %d", p_first_sharpen_both->wide_edge_detect, DC_U, DC_U, DC_U, DC_U);
            amba_ik_system_print_uint32_5("[IK]    max_change->up5x5 = %d", p_first_sharpen_both->max_change_up5x5, DC_U, DC_U, DC_U, DC_U);
            amba_ik_system_print_uint32_5("[IK]    max_change->down5x5 = %d", p_first_sharpen_both->max_change_down5x5, DC_U, DC_U, DC_U, DC_U);
        }
    }
}
static void amba_ik_debug_print_fstshpns_fir_common(const ik_first_sharpen_fir_t *p_first_sharpen_fir)
{
    int32 i, j;

    amba_ik_system_print_uint32_5("[IK]    fir->specify = %d", p_first_sharpen_fir->specify, DC_U, DC_U, DC_U, DC_U);
    amba_ik_system_print_str_5("[IK]    fir->per_dir_fir_iso_strengths = ", DC_S, DC_S, DC_S, DC_S, DC_S);
    for (i = 0; i < 9; i++) {
        amba_ik_system_print_uint32_5("%d ", p_first_sharpen_fir->per_dir_fir_iso_strengths[i], DC_U, DC_U, DC_U, DC_U);
    }
    amba_ik_system_print_str_5("", DC_S, DC_S, DC_S, DC_S, DC_S);
    amba_ik_system_print_str_5("[IK]    fir->per_dir_fir_dir_strengths = ", DC_S, DC_S, DC_S, DC_S, DC_S);
    for (i = 0; i < 9; i++) {
        amba_ik_system_print_uint32_5("%d ", p_first_sharpen_fir->per_dir_fir_dir_strengths[i], DC_U, DC_U, DC_U, DC_U);
    }
    amba_ik_system_print_str_5("", DC_S, DC_S, DC_S, DC_S, DC_S);
    amba_ik_system_print_str_5("[IK]    fir->per_dir_fir_dir_amounts = ", DC_S, DC_S, DC_S, DC_S, DC_S);
    for (i = 0; i < 9; i++) {
        amba_ik_system_print_uint32_5("%d ", p_first_sharpen_fir->per_dir_fir_dir_amounts[i], DC_U, DC_U, DC_U, DC_U);
    }
    amba_ik_system_print_str_5("", DC_S, DC_S, DC_S, DC_S, DC_S);
    amba_ik_system_print_str_5("[IK]    fir->coefs = ", DC_S, DC_S, DC_S, DC_S, DC_S);
    for (j = 0; j < 9; j++) {
        for (i = 0; i < 25; i++) {
            amba_ik_system_print_int32_5("%d ", p_first_sharpen_fir->coefs[j][i], DC_I, DC_I, DC_I, DC_I);
        }
        amba_ik_system_print_str_5("", DC_S, DC_S, DC_S, DC_S, DC_S);
    }
    amba_ik_system_print_uint32_5("[IK]    fir->strength_iso = %d", p_first_sharpen_fir->strength_iso, DC_U, DC_U, DC_U, DC_U);
    amba_ik_system_print_uint32_5("[IK]    fir->strength_dir = %d", p_first_sharpen_fir->strength_dir, DC_U, DC_U, DC_U, DC_U);
}

static void debug_print_fst_shp_ns_fir(uint32 context_id, const ik_first_sharpen_fir_t *p_first_sharpen_fir)
{
    if (p_first_sharpen_fir == NULL) {

    } else {
        if (filter_debug_log[LOG_FIRST_SHARPEN_NOISE_FIR] == 1U) {
            amba_ik_system_print_str_5("[IK] %s",__func__, DC_S, DC_S, DC_S, DC_S);
            amba_ik_system_print_uint32_5("[IK](context_id = %d)", context_id, DC_U, DC_U, DC_U, DC_U);
            amba_ik_debug_print_fstshpns_fir_common(p_first_sharpen_fir);
        }
    }
}

static void debug_print_fst_shp_ns_noise(uint32 context_id, const ik_first_sharpen_noise_t *p_first_sharpen_noise)
{
    if (p_first_sharpen_noise == NULL) {
        // fix compile error unused parameter
    } else {
        if (filter_debug_log[LOG_FIRST_SHARPEN_NOISE_NOISE] == 1U) {
            amba_ik_system_print_str_5("[IK] %s",__func__, DC_S, DC_S, DC_S, DC_S);
            amba_ik_system_print_uint32_5("[IK](context_id = %d)", context_id, DC_U, DC_U, DC_U, DC_U);
            amba_ik_system_print_uint32_5("[IK]    max_change_up = %d", p_first_sharpen_noise->max_change_up, DC_U, DC_U, DC_U, DC_U);
            amba_ik_system_print_uint32_5("[IK]    max_change_down = %d", p_first_sharpen_noise->max_change_down, DC_U, DC_U, DC_U, DC_U);
            amba_ik_system_print_str_5("[IK]    spatial_fir:", DC_S, DC_S, DC_S, DC_S, DC_S);
            amba_ik_debug_print_fstshpns_fir_common(&p_first_sharpen_noise->spatial_fir);
            amba_ik_system_print_str_5("[IK]    level_str_adjust :", DC_S, DC_S, DC_S, DC_S, DC_S);
            amba_ik_debug_print_level(&p_first_sharpen_noise->level_str_adjust);
            amba_ik_system_print_uint32_5("[IK]    level_str_adjust_not_t0_t1_level_based = %d", p_first_sharpen_noise->level_str_adjust_not_t0_t1_level_based, DC_U, DC_U, DC_U, DC_U);
            amba_ik_system_print_uint32_5("[IK]    t0 = %d", p_first_sharpen_noise->t0, DC_U, DC_U, DC_U, DC_U);
            amba_ik_system_print_uint32_5("[IK]    t1 = %d", p_first_sharpen_noise->t1, DC_U, DC_U, DC_U, DC_U);
            amba_ik_system_print_uint32_5("[IK]    alpha_min = %d", p_first_sharpen_noise->alpha_min, DC_U, DC_U, DC_U, DC_U);
            amba_ik_system_print_uint32_5("[IK]    alpha_max = %d", p_first_sharpen_noise->alpha_max, DC_U, DC_U, DC_U, DC_U);
        }
    }
}

static void debug_print_fst_shp_ns_coring(uint32 context_id, const ik_first_sharpen_coring_t *p_first_sharpen_coring)
{
    if (p_first_sharpen_coring == NULL) {
        // fix compile error unused parameter
    } else {
        if (filter_debug_log[LOG_FIRST_SHARPEN_NOISE_CORING] == 1U) {
            amba_ik_system_print_str_5("[IK] %s",__func__, DC_S, DC_S, DC_S, DC_S);
            amba_ik_system_print_uint32_5("[IK](context_id = %d)", context_id, DC_U, DC_U, DC_U, DC_U);
            amba_ik_system_print_uint32_5("[IK]    coring = %d %d ... %d %d %d" \
                                          , p_first_sharpen_coring->coring[0] \
                                          , p_first_sharpen_coring->coring[1] \
                                          , p_first_sharpen_coring->coring[IK_NUM_CORING_TABLE_INDEX-3U] \
                                          , p_first_sharpen_coring->coring[IK_NUM_CORING_TABLE_INDEX-2U] \
                                          , p_first_sharpen_coring->coring[IK_NUM_CORING_TABLE_INDEX-1U]);
            amba_ik_system_print_uint32_5("[IK]    fractional_bits = %d", p_first_sharpen_coring->fractional_bits, DC_U, DC_U, DC_U, DC_U);
        }
    }
}

static void debug_print_fst_shp_ns_cor_idx_scl(uint32 context_id, const ik_first_sharpen_coring_idx_scale_t *p_first_sharpen_coring_idx_scale)
{
    if (p_first_sharpen_coring_idx_scale==NULL) {

    } else {
        if (filter_debug_log[LOG_FIRST_SHARPEN_NOISE_CORING_INDEX_SCALE] == 1U) {
            amba_ik_system_print_str_5("[IK] %s",__func__, DC_S, DC_S, DC_S, DC_S);
            amba_ik_system_print_uint32_5("[IK](context_id = %d)", context_id, DC_U, DC_U, DC_U, DC_U);
            amba_ik_debug_print_level(p_first_sharpen_coring_idx_scale);
        }
    }
}

static void debug_print_fst_shp_ns_min_cor_rst(uint32 context_id, const ik_first_sharpen_min_coring_result_t *p_first_sharpen_min_coring_result)
{
    if (p_first_sharpen_min_coring_result==NULL) {

    } else {
        if (filter_debug_log[LOG_FIRST_SHARPEN_NOISE_MIN_CORING_RESULT] == 1U) {
            amba_ik_system_print_str_5("[IK] %s",__func__, DC_S, DC_S, DC_S, DC_S);
            amba_ik_system_print_uint32_5("[IK](context_id = %d)", context_id, DC_U, DC_U, DC_U, DC_U);
            amba_ik_debug_print_level(p_first_sharpen_min_coring_result);
        }
    }
}

static void debug_print_fst_shp_ns_max_cor_rst(uint32 context_id, const ik_first_sharpen_max_coring_result_t *p_first_sharpen_max_coring_result)
{
    if (p_first_sharpen_max_coring_result == NULL) {

    } else {
        if (filter_debug_log[LOG_FIRST_SHARPEN_NOISE_MAX_CORING_RESULT] == 1U) {
            const ik_level_t *p_lvl;
            amba_ik_system_print_str_5("[IK] %s",__func__, DC_S, DC_S, DC_S, DC_S);
            amba_ik_system_print_uint32_5("[IK](context_id = %d)", context_id, DC_U, DC_U, DC_U, DC_U);
            (void) amba_ik_system_memcpy(&p_lvl, &p_first_sharpen_max_coring_result, sizeof(ik_level_t*));
            amba_ik_debug_print_level(p_lvl);
        }
    }
}

static void debug_print_fst_shp_ns_scl_cor(uint32 context_id, const ik_first_sharpen_scale_coring_t *p_first_sharpen_scale_coring)
{
    if (p_first_sharpen_scale_coring == NULL) {

    } else {
        if (filter_debug_log[LOG_FIRST_SHARPEN_NOISE_SCALE_CORING] == 1U) {
            amba_ik_system_print_str_5("[IK] %s",__func__, DC_S, DC_S, DC_S, DC_S);
            amba_ik_system_print_uint32_5("[IK](context_id = %d)", context_id, DC_U, DC_U, DC_U, DC_U);
            amba_ik_debug_print_level(p_first_sharpen_scale_coring);
        }
    }
}

static void amba_ik_debug_print_warp_enable(uint32 context_id, const uint32 enable)
{
    if (filter_debug_log[LOG_WARP_COMPENSATION] == 1U) {
        amba_ik_system_print_str_5("[IK] %s",__func__, DC_S, DC_S, DC_S, DC_S);
        amba_ik_system_print_uint32_5("[IK](context_id = %d)", context_id, DC_U, DC_U, DC_U, DC_U);
        amba_ik_system_print_uint32_5("[IK]    enable = %d", enable, DC_U, DC_U, DC_U, DC_U);
    }
}

static void amba_ik_debug_print_level_method(const ik_level_method_t *p_lvl)
{
    amba_ik_system_print_uint32_5("[IK]    p_lvl->high = %d", p_lvl->high, DC_U, DC_U, DC_U, DC_U);
    amba_ik_system_print_uint32_5("[IK]    p_lvl->high_delta = %d", p_lvl->high_delta, DC_U, DC_U, DC_U, DC_U);
    amba_ik_system_print_uint32_5("[IK]    p_lvl->high_strength = %d", p_lvl->high_strength, DC_U, DC_U, DC_U, DC_U);
    amba_ik_system_print_uint32_5("[IK]    p_lvl->low = %d", p_lvl->low, DC_U, DC_U, DC_U, DC_U);
    amba_ik_system_print_uint32_5("[IK]    p_lvl->low_delta = %d", p_lvl->low_delta, DC_U, DC_U, DC_U, DC_U);
    amba_ik_system_print_uint32_5("[IK]    p_lvl->low_strength = %d", p_lvl->low_strength, DC_U, DC_U, DC_U, DC_U);
    amba_ik_system_print_uint32_5("[IK]    p_lvl->method = %d", p_lvl->method, DC_U, DC_U, DC_U, DC_U);
    amba_ik_system_print_uint32_5("[IK]    p_lvl->mid_strength = %d", p_lvl->mid_strength, DC_U, DC_U, DC_U, DC_U);
}

static void amba_ik_debug_print_video_mctf(uint32 context_id, const ik_video_mctf_t *p_video_mctf)
{
    if (p_video_mctf == NULL) {

    } else {
        if (filter_debug_log[LOG_VIDEO_MCTF] == 1U) {
            amba_ik_system_print_str_5("[IK] %s",__func__, DC_S, DC_S, DC_S, DC_S);
            amba_ik_system_print_uint32_5("[IK](context_id = %d)", context_id, DC_U, DC_U, DC_U, DC_U);
            amba_ik_system_print_uint32_5("[IK]    enable = %d", p_video_mctf->enable, DC_U, DC_U, DC_U, DC_U);
            amba_ik_system_print_uint32_5("[IK]    y_narrow_nonsmooth_detect_shift = %d", p_video_mctf->y_narrow_nonsmooth_detect_shift, DC_U, DC_U, DC_U, DC_U);
            amba_ik_system_print_uint32_5("[IK]    y_narrow_nonsmooth_detect_sub = %d %d %d", \
                                          p_video_mctf->y_narrow_nonsmooth_detect_sub[0],\
                                          p_video_mctf->y_narrow_nonsmooth_detect_sub[1],\
                                          p_video_mctf->y_narrow_nonsmooth_detect_sub[2], DC_U, DC_U);
            amba_ik_system_print_uint32_5("[IK]    use_level_based_ta = %d", p_video_mctf->use_level_based_ta, DC_U, DC_U, DC_U, DC_U);
            amba_ik_system_print_uint32_5("[IK]    compression_enable = %d", p_video_mctf->compression_enable, DC_U, DC_U, DC_U, DC_U);

            amba_ik_system_print_str_5("[IK]    y_3d_maxchange:", DC_S, DC_S, DC_S, DC_S, DC_S);
            amba_ik_debug_print_level_method(&p_video_mctf->y_3d_maxchange);
            amba_ik_system_print_str_5("[IK]    cb_3d_maxchange:", DC_S, DC_S, DC_S, DC_S, DC_S);
            amba_ik_debug_print_level_method(&p_video_mctf->cb_3d_maxchange);
            amba_ik_system_print_str_5("[IK]    cr_3d_maxchange:", DC_S, DC_S, DC_S, DC_S, DC_S);
            amba_ik_debug_print_level_method(&p_video_mctf->cr_3d_maxchange);

            amba_ik_system_print_uint32_5("[IK]    y_advanced_iso_enable = %d", p_video_mctf->y_advanced_iso_enable, DC_U, DC_U, DC_U, DC_U);
            amba_ik_system_print_uint32_5("[IK]    y_advanced_iso_max_change_method = %d", p_video_mctf->y_advanced_iso_max_change_method, DC_U, DC_U, DC_U, DC_U);
            amba_ik_system_print_uint32_5("[IK]    y_advanced_iso_max_change_table = %d %d %d", \
                                          p_video_mctf->y_advanced_iso_max_change_table[0],\
                                          p_video_mctf->y_advanced_iso_max_change_table[1],\
                                          p_video_mctf->y_advanced_iso_max_change_table[2], DC_U, DC_U);
            amba_ik_system_print_uint32_5("[IK]    y_advanced_iso_noise_level = %d", p_video_mctf->y_advanced_iso_noise_level, DC_U, DC_U, DC_U, DC_U);
            amba_ik_system_print_uint32_5("[IK]    y_advanced_iso_size = %d", p_video_mctf->y_advanced_iso_size, DC_U, DC_U, DC_U, DC_U);
            amba_ik_system_print_uint32_5("[IK]    cb_advanced_iso_enable = %d", p_video_mctf->cb_advanced_iso_enable, DC_U, DC_U, DC_U, DC_U);
            amba_ik_system_print_uint32_5("[IK]    cb_advanced_iso_max_change_method = %d", p_video_mctf->cb_advanced_iso_max_change_method, DC_U, DC_U, DC_U, DC_U);
            amba_ik_system_print_uint32_5("[IK]    cb_advanced_iso_max_change_table = %d %d %d", \
                                          p_video_mctf->cb_advanced_iso_max_change_table[0],\
                                          p_video_mctf->cb_advanced_iso_max_change_table[1],\
                                          p_video_mctf->cb_advanced_iso_max_change_table[2], DC_U, DC_U);
            amba_ik_system_print_uint32_5("[IK]    cb_advanced_iso_noise_level = %d", p_video_mctf->cb_advanced_iso_noise_level, DC_U, DC_U, DC_U, DC_U);
            amba_ik_system_print_uint32_5("[IK]    cb_advanced_iso_size = %d", p_video_mctf->cb_advanced_iso_size, DC_U, DC_U, DC_U, DC_U);
            amba_ik_system_print_uint32_5("[IK]    cr_advanced_iso_enable = %d", p_video_mctf->cr_advanced_iso_enable, DC_U, DC_U, DC_U, DC_U);
            amba_ik_system_print_uint32_5("[IK]    cr_advanced_iso_max_change_method = %d", p_video_mctf->cr_advanced_iso_max_change_method, DC_U, DC_U, DC_U, DC_U);
            amba_ik_system_print_uint32_5("[IK]    cr_advanced_iso_max_change_table = %d %d %d", \
                                          p_video_mctf->cr_advanced_iso_max_change_table[0],\
                                          p_video_mctf->cr_advanced_iso_max_change_table[1],\
                                          p_video_mctf->cr_advanced_iso_max_change_table[2], DC_U, DC_U);
            amba_ik_system_print_uint32_5("[IK]    cr_advanced_iso_noise_level = %d", p_video_mctf->cr_advanced_iso_noise_level, DC_U, DC_U, DC_U, DC_U);
            amba_ik_system_print_uint32_5("[IK]    cr_advanced_iso_size = %d", p_video_mctf->cr_advanced_iso_size, DC_U, DC_U, DC_U, DC_U);

            amba_ik_system_print_str_5("[IK]    y_level_based_ta:", DC_S, DC_S, DC_S, DC_S, DC_S);
            amba_ik_debug_print_level_method(&p_video_mctf->y_level_based_ta);
            amba_ik_system_print_str_5("[IK]    cb_level_based_ta:", DC_S, DC_S, DC_S, DC_S, DC_S);
            amba_ik_debug_print_level_method(&p_video_mctf->cb_level_based_ta);
            amba_ik_system_print_str_5("[IK]    cr_level_based_ta:", DC_S, DC_S, DC_S, DC_S, DC_S);
            amba_ik_debug_print_level_method(&p_video_mctf->cr_level_based_ta);

            amba_ik_system_print_str_5("[IK]    y_overall_max_change:", DC_S, DC_S, DC_S, DC_S, DC_S);
            amba_ik_debug_print_level_method(&p_video_mctf->y_overall_max_change);
            amba_ik_system_print_str_5("[IK]    cb_overall_max_change:", DC_S, DC_S, DC_S, DC_S, DC_S);
            amba_ik_debug_print_level_method(&p_video_mctf->cb_overall_max_change);
            amba_ik_system_print_str_5("[IK]    cr_overall_max_change:", DC_S, DC_S, DC_S, DC_S, DC_S);
            amba_ik_debug_print_level_method(&p_video_mctf->cr_overall_max_change);

            amba_ik_system_print_str_5("[IK]    y_spat_blend:", DC_S, DC_S, DC_S, DC_S, DC_S);
            amba_ik_debug_print_level_method(&p_video_mctf->y_spat_blend);
            amba_ik_system_print_str_5("[IK]    cb_spat_blend:", DC_S, DC_S, DC_S, DC_S, DC_S);
            amba_ik_debug_print_level_method(&p_video_mctf->cb_spat_blend);
            amba_ik_system_print_str_5("[IK]    cr_spat_blend:", DC_S, DC_S, DC_S, DC_S, DC_S);
            amba_ik_debug_print_level_method(&p_video_mctf->cr_spat_blend);

            amba_ik_system_print_str_5("[IK]    y_spat_filt_max_smth_change:", DC_S, DC_S, DC_S, DC_S, DC_S);
            amba_ik_debug_print_level_method(&p_video_mctf->y_spat_filt_max_smth_change);
            amba_ik_system_print_str_5("[IK]    cb_spat_filt_max_smth_change:", DC_S, DC_S, DC_S, DC_S, DC_S);
            amba_ik_debug_print_level_method(&p_video_mctf->cb_spat_filt_max_smth_change);
            amba_ik_system_print_str_5("[IK]    cr_spat_filt_max_smth_change:", DC_S, DC_S, DC_S, DC_S, DC_S);
            amba_ik_debug_print_level_method(&p_video_mctf->cr_spat_filt_max_smth_change);

            amba_ik_system_print_uint32_5("[IK]    y_spat_smth_wide_edge_detect = %d", p_video_mctf->y_spat_smth_wide_edge_detect, DC_U, DC_U, DC_U, DC_U);
            amba_ik_system_print_uint32_5("[IK]    y_spat_smth_edge_thresh = %d", p_video_mctf->y_spat_smth_edge_thresh, DC_U, DC_U, DC_U, DC_U);
            amba_ik_system_print_str_5("[IK]    y_spat_smth_dir:", DC_S, DC_S, DC_S, DC_S, DC_S);
            amba_ik_debug_print_level_method(&p_video_mctf->y_spat_smth_dir);
            amba_ik_system_print_uint32_5("[IK]    y_spat_smth_wide_edge_detect = %d", p_video_mctf->cb_spat_smth_wide_edge_detect, DC_U, DC_U, DC_U, DC_U);
            amba_ik_system_print_uint32_5("[IK]    cb_spat_smth_edge_thresh = %d", p_video_mctf->cb_spat_smth_edge_thresh, DC_U, DC_U, DC_U, DC_U);
            amba_ik_system_print_str_5("[IK]    cb_spat_smth_dir:", DC_S, DC_S, DC_S, DC_S, DC_S);
            amba_ik_debug_print_level_method(&p_video_mctf->cb_spat_smth_dir);
            amba_ik_system_print_uint32_5("[IK]    cr_spat_smth_wide_edge_detect = %d", p_video_mctf->cr_spat_smth_wide_edge_detect, DC_U, DC_U, DC_U, DC_U);
            amba_ik_system_print_uint32_5("[IK]    cr_spat_smth_edge_thresh = %d", p_video_mctf->cr_spat_smth_edge_thresh, DC_U, DC_U, DC_U, DC_U);
            amba_ik_system_print_str_5("[IK]    cr_spat_smth_dir:", DC_S, DC_S, DC_S, DC_S, DC_S);
            amba_ik_debug_print_level_method(&p_video_mctf->cr_spat_smth_dir);

            amba_ik_system_print_str_5("[IK]    y_spat_smth_iso:", DC_S, DC_S, DC_S, DC_S, DC_S);
            amba_ik_debug_print_level_method(&p_video_mctf->y_spat_smth_iso);
            amba_ik_system_print_str_5("[IK]    cb_spat_smth_iso:", DC_S, DC_S, DC_S, DC_S, DC_S);
            amba_ik_debug_print_level_method(&p_video_mctf->cb_spat_smth_iso);
            amba_ik_system_print_str_5("[IK]    cr_spat_smth_iso:", DC_S, DC_S, DC_S, DC_S, DC_S);
            amba_ik_debug_print_level_method(&p_video_mctf->cr_spat_smth_iso);

            amba_ik_system_print_uint32_5("[IK]    y_spat_smth_direct_decide_t0 = %d", p_video_mctf->y_spat_smth_direct_decide_t0, DC_U, DC_U, DC_U, DC_U);
            amba_ik_system_print_uint32_5("[IK]    y_spat_smth_direct_decide_t1 = %d", p_video_mctf->y_spat_smth_direct_decide_t1, DC_U, DC_U, DC_U, DC_U);
            amba_ik_system_print_uint32_5("[IK]    cb_spat_smth_direct_decide_t0 = %d", p_video_mctf->cb_spat_smth_direct_decide_t0, DC_U, DC_U, DC_U, DC_U);
            amba_ik_system_print_uint32_5("[IK]    cb_spat_smth_direct_decide_t1 = %d", p_video_mctf->cb_spat_smth_direct_decide_t1, DC_U, DC_U, DC_U, DC_U);
            amba_ik_system_print_uint32_5("[IK]    cr_spat_smth_direct_decide_t0 = %d", p_video_mctf->cr_spat_smth_direct_decide_t0, DC_U, DC_U, DC_U, DC_U);
            amba_ik_system_print_uint32_5("[IK]    cr_spat_smth_direct_decide_t1 = %d", p_video_mctf->cr_spat_smth_direct_decide_t1, DC_U, DC_U, DC_U, DC_U);

            amba_ik_system_print_uint32_5("[IK]    y_spatial_max_change = %d", p_video_mctf->y_spatial_max_change, DC_U, DC_U, DC_U, DC_U);
            amba_ik_system_print_str_5("[IK]    y_spatial_max_temporal:", DC_S, DC_S, DC_S, DC_S, DC_S);
            amba_ik_debug_print_level_method(&p_video_mctf->y_spatial_max_temporal);
            amba_ik_system_print_uint32_5("[IK]    cb_spatial_max_change = %d", p_video_mctf->cb_spatial_max_change, DC_U, DC_U, DC_U, DC_U);
            amba_ik_system_print_str_5("[IK]    cb_spatial_max_temporal:", DC_S, DC_S, DC_S, DC_S, DC_S);
            amba_ik_debug_print_level_method(&p_video_mctf->cb_spatial_max_temporal);
            amba_ik_system_print_uint32_5("[IK]    cr_spatial_max_change = %d", p_video_mctf->cr_spatial_max_change, DC_U, DC_U, DC_U, DC_U);
            amba_ik_system_print_str_5("[IK]    cr_spatial_max_temporal:", DC_S, DC_S, DC_S, DC_S, DC_S);
            amba_ik_debug_print_level_method(&p_video_mctf->cr_spatial_max_temporal);

            amba_ik_system_print_uint32_5("[IK]    y_curve.temporal_alpha0 = %d", p_video_mctf->y_curve.temporal_alpha0, DC_U, DC_U, DC_U, DC_U);
            amba_ik_system_print_uint32_5("[IK]    y_curve.temporal_alpha1 = %d", p_video_mctf->y_curve.temporal_alpha1, DC_U, DC_U, DC_U, DC_U);
            amba_ik_system_print_uint32_5("[IK]    y_curve.temporal_alpha2 = %d", p_video_mctf->y_curve.temporal_alpha2, DC_U, DC_U, DC_U, DC_U);
            amba_ik_system_print_uint32_5("[IK]    y_curve.temporal_alpha3 = %d", p_video_mctf->y_curve.temporal_alpha3, DC_U, DC_U, DC_U, DC_U);
            amba_ik_system_print_uint32_5("[IK]    y_curve.temporal_t0 = %d", p_video_mctf->y_curve.temporal_t0, DC_U, DC_U, DC_U, DC_U);
            amba_ik_system_print_uint32_5("[IK]    y_curve.temporal_t1 = %d", p_video_mctf->y_curve.temporal_t1, DC_U, DC_U, DC_U, DC_U);
            amba_ik_system_print_uint32_5("[IK]    y_curve.temporal_t2 = %d", p_video_mctf->y_curve.temporal_t2, DC_U, DC_U, DC_U, DC_U);
            amba_ik_system_print_uint32_5("[IK]    y_curve.temporal_t3 = %d", p_video_mctf->y_curve.temporal_t3, DC_U, DC_U, DC_U, DC_U);
            amba_ik_system_print_uint32_5("[IK]    y_temporal_max_change = %d", p_video_mctf->y_temporal_max_change, DC_U, DC_U, DC_U, DC_U);
            amba_ik_system_print_str_5("[IK]    y_temporal_min_target:", DC_S, DC_S, DC_S, DC_S, DC_S);
            amba_ik_debug_print_level_method(&p_video_mctf->y_temporal_min_target);
            amba_ik_system_print_uint32_5("[IK]    cb_curve.temporal_alpha0 = %d", p_video_mctf->cb_curve.temporal_alpha0, DC_U, DC_U, DC_U, DC_U);
            amba_ik_system_print_uint32_5("[IK]    cb_curve.temporal_alpha1 = %d", p_video_mctf->cb_curve.temporal_alpha1, DC_U, DC_U, DC_U, DC_U);
            amba_ik_system_print_uint32_5("[IK]    cb_curve.temporal_alpha2 = %d", p_video_mctf->cb_curve.temporal_alpha2, DC_U, DC_U, DC_U, DC_U);
            amba_ik_system_print_uint32_5("[IK]    cb_curve.temporal_alpha3 = %d", p_video_mctf->cb_curve.temporal_alpha3, DC_U, DC_U, DC_U, DC_U);
            amba_ik_system_print_uint32_5("[IK]    cb_curve.temporal_t0 = %d", p_video_mctf->cb_curve.temporal_t0, DC_U, DC_U, DC_U, DC_U);
            amba_ik_system_print_uint32_5("[IK]    cb_curve.temporal_t1 = %d", p_video_mctf->cb_curve.temporal_t1, DC_U, DC_U, DC_U, DC_U);
            amba_ik_system_print_uint32_5("[IK]    cb_curve.temporal_t2 = %d", p_video_mctf->cb_curve.temporal_t2, DC_U, DC_U, DC_U, DC_U);
            amba_ik_system_print_uint32_5("[IK]    cb_curve.temporal_t3 = %d", p_video_mctf->cb_curve.temporal_t3, DC_U, DC_U, DC_U, DC_U);
            amba_ik_system_print_uint32_5("[IK]    cb_temporal_max_change = %d", p_video_mctf->cb_temporal_max_change, DC_U, DC_U, DC_U, DC_U);
            amba_ik_system_print_str_5("[IK]    cb_temporal_min_target:", DC_S, DC_S, DC_S, DC_S, DC_S);
            amba_ik_debug_print_level_method(&p_video_mctf->cb_temporal_min_target);
            amba_ik_system_print_uint32_5("[IK]    cr_curve.temporal_alpha0 = %d", p_video_mctf->cr_curve.temporal_alpha0, DC_U, DC_U, DC_U, DC_U);
            amba_ik_system_print_uint32_5("[IK]    cr_curve.temporal_alpha1 = %d", p_video_mctf->cr_curve.temporal_alpha1, DC_U, DC_U, DC_U, DC_U);
            amba_ik_system_print_uint32_5("[IK]    cr_curve.temporal_alpha2 = %d", p_video_mctf->cr_curve.temporal_alpha2, DC_U, DC_U, DC_U, DC_U);
            amba_ik_system_print_uint32_5("[IK]    cr_curve.temporal_alpha3 = %d", p_video_mctf->cr_curve.temporal_alpha3, DC_U, DC_U, DC_U, DC_U);
            amba_ik_system_print_uint32_5("[IK]    cr_curve.temporal_t0 = %d", p_video_mctf->cr_curve.temporal_t0, DC_U, DC_U, DC_U, DC_U);
            amba_ik_system_print_uint32_5("[IK]    cr_curve.temporal_t1 = %d", p_video_mctf->cr_curve.temporal_t1, DC_U, DC_U, DC_U, DC_U);
            amba_ik_system_print_uint32_5("[IK]    cr_curve.temporal_t2 = %d", p_video_mctf->cr_curve.temporal_t2, DC_U, DC_U, DC_U, DC_U);
            amba_ik_system_print_uint32_5("[IK]    cr_curve.temporal_t3 = %d", p_video_mctf->cr_curve.temporal_t3, DC_U, DC_U, DC_U, DC_U);
            amba_ik_system_print_uint32_5("[IK]    cr_temporal_max_change = %d", p_video_mctf->cr_temporal_max_change, DC_U, DC_U, DC_U, DC_U);
            amba_ik_system_print_str_5("[IK]    cr_temporal_min_target:", DC_S, DC_S, DC_S, DC_S, DC_S);
            amba_ik_debug_print_level_method(&p_video_mctf->cr_temporal_min_target);

            amba_ik_system_print_uint32_5("[IK]    y_temporal_ghost_prevent = %d", p_video_mctf->y_temporal_ghost_prevent, DC_U, DC_U, DC_U, DC_U);
            amba_ik_system_print_uint32_5("[IK]    cb_temporal_ghost_prevent = %d", p_video_mctf->cb_temporal_ghost_prevent, DC_U, DC_U, DC_U, DC_U);
            amba_ik_system_print_uint32_5("[IK]    cr_temporal_ghost_prevent = %d", p_video_mctf->cr_temporal_ghost_prevent, DC_U, DC_U, DC_U, DC_U);

            amba_ik_system_print_uint32_5("[IK]    compression_bit_rate_luma = %d", p_video_mctf->compression_bit_rate_luma, DC_U, DC_U, DC_U, DC_U);
            amba_ik_system_print_uint32_5("[IK]    compression_bit_rate_chroma = %d", p_video_mctf->compression_bit_rate_chroma, DC_U, DC_U, DC_U, DC_U);
            amba_ik_system_print_uint32_5("[IK]    compression_dither_disable = %d", p_video_mctf->compression_dither_disable, DC_U, DC_U, DC_U, DC_U);

            amba_ik_system_print_uint32_5("[IK]    y_temporal_artifact_guard = %d", p_video_mctf->y_temporal_artifact_guard, DC_U, DC_U, DC_U, DC_U);
            amba_ik_system_print_uint32_5("[IK]    cb_temporal_artifact_guard = %d", p_video_mctf->cb_temporal_artifact_guard, DC_U, DC_U, DC_U, DC_U);
            amba_ik_system_print_uint32_5("[IK]    cr_temporal_artifact_guard = %d", p_video_mctf->cr_temporal_artifact_guard, DC_U, DC_U, DC_U, DC_U);

            amba_ik_system_print_uint32_5("[IK]    y_combine_strength = %d", p_video_mctf->y_combine_strength, DC_U, DC_U, DC_U, DC_U);
            amba_ik_system_print_uint32_5("[IK]    y_strength_3d = %d", p_video_mctf->y_strength_3d, DC_U, DC_U, DC_U, DC_U);
            amba_ik_system_print_uint32_5("[IK]    y_spatial_weighting = %d %d %d ...", \
                                          p_video_mctf->y_spatial_weighting[0],\
                                          p_video_mctf->y_spatial_weighting[1],\
                                          p_video_mctf->y_spatial_weighting[2], DC_U, DC_U);
            amba_ik_system_print_uint32_5("[IK]    cb_strength_3d = %d", p_video_mctf->cb_strength_3d, DC_U, DC_U, DC_U, DC_U);
            amba_ik_system_print_uint32_5("[IK]    cb_spatial_weighting = %d %d %d ...", \
                                          p_video_mctf->cb_spatial_weighting[0],\
                                          p_video_mctf->cb_spatial_weighting[1],\
                                          p_video_mctf->cb_spatial_weighting[2], DC_U, DC_U);
            amba_ik_system_print_uint32_5("[IK]    cr_strength_3d = %d", p_video_mctf->cr_strength_3d, DC_U, DC_U, DC_U, DC_U);
            amba_ik_system_print_uint32_5("[IK]    cr_spatial_weighting = %d %d %d ...", \
                                          p_video_mctf->cr_spatial_weighting[0],\
                                          p_video_mctf->cr_spatial_weighting[1],\
                                          p_video_mctf->cr_spatial_weighting[2], DC_U, DC_U);

            amba_ik_system_print_uint32_5("[IK]    y_temporal_max_change_not_t0_t1_level_based = %d", p_video_mctf->y_temporal_max_change_not_t0_t1_level_based, DC_U, DC_U, DC_U, DC_U);
            amba_ik_system_print_str_5("[IK]    y_temporal_either_max_change_or_t0_t1_add:", DC_S, DC_S, DC_S, DC_S, DC_S);
            amba_ik_debug_print_level_method(&p_video_mctf->y_temporal_either_max_change_or_t0_t1_add);
            amba_ik_system_print_uint32_5("[IK]    cb_temporal_max_change_not_t0_t1_level_based = %d", p_video_mctf->cb_temporal_max_change_not_t0_t1_level_based, DC_U, DC_U, DC_U, DC_U);
            amba_ik_system_print_str_5("[IK]    cb_temporal_either_max_change_or_t0_t1_add:", DC_S, DC_S, DC_S, DC_S, DC_S);
            amba_ik_debug_print_level_method(&p_video_mctf->cb_temporal_either_max_change_or_t0_t1_add);
            amba_ik_system_print_uint32_5("[IK]    cr_temporal_max_change_not_t0_t1_level_based = %d", p_video_mctf->cr_temporal_max_change_not_t0_t1_level_based, DC_U, DC_U, DC_U, DC_U);
            amba_ik_system_print_str_5("[IK]    cr_temporal_either_max_change_or_t0_t1_add:", DC_S, DC_S, DC_S, DC_S, DC_S);
            amba_ik_debug_print_level_method(&p_video_mctf->cr_temporal_either_max_change_or_t0_t1_add);
        }
    }
}

static void amba_ik_debug_print_video_mctf_ta(uint32 context_id, const ik_video_mctf_ta_t *p_mctf_ta)
{
    if (p_mctf_ta == NULL) {

    } else {
        if (filter_debug_log[LOG_VIDEO_MCTF_TEMPORAL_ADJUST] == 1U) {
            amba_ik_system_print_str_5("[IK] %s",__func__, DC_S, DC_S, DC_S, DC_S);
            amba_ik_system_print_uint32_5("[IK](context_id = %d)", context_id, DC_U, DC_U, DC_U, DC_U);
            amba_ik_system_print_uint32_5("[IK]    slow_mo_sensitivity = %d", p_mctf_ta->slow_mo_sensitivity, DC_U, DC_U, DC_U, DC_U);
            amba_ik_system_print_uint32_5("[IK]    score_noise_robust = %d", p_mctf_ta->score_noise_robust, DC_U, DC_U, DC_U, DC_U);
            amba_ik_system_print_uint32_5("[IK]    lev_adjust_low = %d", p_mctf_ta->lev_adjust_low, DC_U, DC_U, DC_U, DC_U);
            amba_ik_system_print_uint32_5("[IK]    lev_adjust_low_delta = %d", p_mctf_ta->lev_adjust_low_delta, DC_U, DC_U, DC_U, DC_U);
            amba_ik_system_print_uint32_5("[IK]    lev_adjust_low_strength = %d", p_mctf_ta->lev_adjust_low_strength, DC_U, DC_U, DC_U, DC_U);
            amba_ik_system_print_uint32_5("[IK]    lev_adjust_mid_strength = %d", p_mctf_ta->lev_adjust_mid_strength, DC_U, DC_U, DC_U, DC_U);
            amba_ik_system_print_uint32_5("[IK]    lev_adjust_high = %d", p_mctf_ta->lev_adjust_high, DC_U, DC_U, DC_U, DC_U);
            amba_ik_system_print_uint32_5("[IK]    lev_adjust_high_delta = %d", p_mctf_ta->lev_adjust_high_delta, DC_U, DC_U, DC_U, DC_U);
            amba_ik_system_print_uint32_5("[IK]    lev_adjust_high_strength = %d", p_mctf_ta->lev_adjust_high_strength, DC_U, DC_U, DC_U, DC_U);
            amba_ik_system_print_uint32_5("[IK]    p_mctf_ta->y_mctf_ta.min = %d", p_mctf_ta->y_mctf_ta.max, DC_U, DC_U, DC_U, DC_U);
            amba_ik_system_print_uint32_5("[IK]    p_mctf_ta->y_mctf_ta.max = %d", p_mctf_ta->y_mctf_ta.min, DC_U, DC_U, DC_U, DC_U);
            amba_ik_system_print_uint32_5("[IK]    p_mctf_ta->y_mctf_ta.motion_response = %d", p_mctf_ta->y_mctf_ta.motion_response, DC_U, DC_U, DC_U, DC_U);
            amba_ik_system_print_uint32_5("[IK]    p_mctf_ta->y_mctf_ta.noise_base = %d", p_mctf_ta->y_mctf_ta.noise_base, DC_U, DC_U, DC_U, DC_U);
            amba_ik_system_print_uint32_5("[IK]    p_mctf_ta->y_mctf_ta.still_thresh = %d", p_mctf_ta->y_mctf_ta.still_thresh, DC_U, DC_U, DC_U, DC_U);
            amba_ik_system_print_uint32_5("[IK]    p_mctf_ta->cb_mctf_ta.min = %d", p_mctf_ta->cb_mctf_ta.max, DC_U, DC_U, DC_U, DC_U);
            amba_ik_system_print_uint32_5("[IK]    p_mctf_ta->cb_mctf_ta.max = %d", p_mctf_ta->cb_mctf_ta.min, DC_U, DC_U, DC_U, DC_U);
            amba_ik_system_print_uint32_5("[IK]    p_mctf_ta->cb_mctf_ta.motion_response = %d", p_mctf_ta->cb_mctf_ta.motion_response, DC_U, DC_U, DC_U, DC_U);
            amba_ik_system_print_uint32_5("[IK]    p_mctf_ta->cb_mctf_ta.noise_base = %d", p_mctf_ta->cb_mctf_ta.noise_base, DC_U, DC_U, DC_U, DC_U);
            amba_ik_system_print_uint32_5("[IK]    p_mctf_ta->cb_mctf_ta.still_thresh = %d", p_mctf_ta->cb_mctf_ta.still_thresh, DC_U, DC_U, DC_U, DC_U);
            amba_ik_system_print_uint32_5("[IK]    p_mctf_ta->cr_mctf_ta.min = %d", p_mctf_ta->cr_mctf_ta.max, DC_U, DC_U, DC_U, DC_U);
            amba_ik_system_print_uint32_5("[IK]    p_mctf_ta->cr_mctf_ta.max = %d", p_mctf_ta->cr_mctf_ta.min, DC_U, DC_U, DC_U, DC_U);
            amba_ik_system_print_uint32_5("[IK]    p_mctf_ta->cr_mctf_ta.motion_response = %d", p_mctf_ta->cr_mctf_ta.motion_response, DC_U, DC_U, DC_U, DC_U);
            amba_ik_system_print_uint32_5("[IK]    p_mctf_ta->cr_mctf_ta.noise_base = %d", p_mctf_ta->cr_mctf_ta.noise_base, DC_U, DC_U, DC_U, DC_U);
            amba_ik_system_print_uint32_5("[IK]    p_mctf_ta->cr_mctf_ta.still_thresh = %d", p_mctf_ta->cr_mctf_ta.still_thresh, DC_U, DC_U, DC_U, DC_U);
        }
    }
}

static void debug_print_video_mctf_and_fnlshp(uint32 context_id, const ik_pos_dep33_t *p_mctf_and_fnlshp)
{
    if (p_mctf_and_fnlshp == NULL) {

    } else {
        if (filter_debug_log[LOG_VIDEO_MCTF_AND_FINAL_SHARPEN] == 1U) {
            amba_ik_system_print_str_5("[IK] %s",__func__, DC_S, DC_S, DC_S, DC_S);
            amba_ik_system_print_uint32_5("[IK](context_id = %d)", context_id, DC_U, DC_U, DC_U, DC_U);
            amba_ik_system_print_uint32_5("[IK]    pos_dep = %d %d %d", \
                                          p_mctf_and_fnlshp->pos_dep[0][0],\
                                          p_mctf_and_fnlshp->pos_dep[0][1],\
                                          p_mctf_and_fnlshp->pos_dep[0][2], DC_U, DC_U);
        }
    }
}
static void debug_print_fnlshpns_both(uint32 context_id, const ik_final_sharpen_both_t *p_final_sharpen_both)
{
    if (p_final_sharpen_both == NULL) {

    } else {
        if (filter_debug_log[LOG_FINAL_SHARPEN_NOISE_BOTH] == 1U) {
            amba_ik_system_print_str_5("[IK] %s",__func__, DC_S, DC_S, DC_S, DC_S);
            amba_ik_system_print_uint32_5("[IK](context_id = %d)", context_id, DC_U, DC_U, DC_U, DC_U);
            amba_ik_system_print_uint32_5("[IK]    enable = %d", p_final_sharpen_both->enable, DC_U, DC_U, DC_U, DC_U);
            amba_ik_system_print_uint32_5("[IK]    mode = %d", p_final_sharpen_both->mode, DC_U, DC_U, DC_U, DC_U);
            amba_ik_system_print_uint32_5("[IK]    edge_thresh = %d", p_final_sharpen_both->edge_thresh, DC_U, DC_U, DC_U, DC_U);
            amba_ik_system_print_uint32_5("[IK]    wide_edge_detect = %d", p_final_sharpen_both->wide_edge_detect, DC_U, DC_U, DC_U, DC_U);
            amba_ik_system_print_uint32_5("[IK]    max_change->up5x5 = %d", p_final_sharpen_both->max_change.up5x5, DC_U, DC_U, DC_U, DC_U);
            amba_ik_system_print_uint32_5("[IK]    max_change->down5x5 = %d", p_final_sharpen_both->max_change.down5x5, DC_U, DC_U, DC_U, DC_U);
            amba_ik_system_print_uint32_5("[IK]    max_change->up = %d", p_final_sharpen_both->max_change.up, DC_U, DC_U, DC_U, DC_U);
            amba_ik_system_print_uint32_5("[IK]    max_change->down = %d", p_final_sharpen_both->max_change.down, DC_U, DC_U, DC_U, DC_U);
            amba_ik_system_print_uint32_5("[IK]    narrow_nonsmooth_detect_sub = %d %d %d ...", \
                                          p_final_sharpen_both->narrow_nonsmooth_detect_sub[0],\
                                          p_final_sharpen_both->narrow_nonsmooth_detect_sub[1],\
                                          p_final_sharpen_both->narrow_nonsmooth_detect_sub[2], DC_U, DC_U);
            amba_ik_system_print_uint32_5("[IK]    narrow_nonsmooth_detect_shift = %d", p_final_sharpen_both->narrow_nonsmooth_detect_shift, DC_U, DC_U, DC_U, DC_U);
        }
    }
}

static void debug_print_fnlshpns_fir(uint32 context_id, const ik_final_sharpen_fir_t *p_final_sharpen_fir)
{
    const ik_first_sharpen_fir_t *p_fst_fir;//FIX?
    if (p_final_sharpen_fir == NULL) {

    } else {
        if (filter_debug_log[LOG_FINAL_SHARPEN_NOISE_FIR] == 1U) {
            amba_ik_system_print_str_5("[IK] %s",__func__, DC_S, DC_S, DC_S, DC_S);
            amba_ik_system_print_uint32_5("[IK](context_id = %d)", context_id, DC_U, DC_U, DC_U, DC_U);
            (void) amba_ik_system_memcpy(&p_fst_fir, &p_final_sharpen_fir, sizeof(ik_first_sharpen_fir_t*));
            amba_ik_debug_print_fstshpns_fir_common(p_fst_fir);
            amba_ik_system_print_uint32_5("[IK]    pos_fir_artifact_reduce_enable = %d", p_final_sharpen_fir->pos_fir_artifact_reduce_enable, DC_U, DC_U, DC_U, DC_U);
            amba_ik_system_print_uint32_5("[IK]    pos_fir_artifact_reduce_strength = %d", p_final_sharpen_fir->pos_fir_artifact_reduce_strength, DC_U, DC_U, DC_U, DC_U);
            amba_ik_system_print_uint32_5("[IK]    over_shoot_reduce_allow = %d", p_final_sharpen_fir->over_shoot_reduce_allow, DC_U, DC_U, DC_U, DC_U);
            amba_ik_system_print_uint32_5("[IK]    over_shoot_reduce_strength = %d", p_final_sharpen_fir->over_shoot_reduce_strength, DC_U, DC_U, DC_U, DC_U);
            amba_ik_system_print_uint32_5("[IK]    under_shoot_reduce_allow = %d", p_final_sharpen_fir->under_shoot_reduce_allow, DC_U, DC_U, DC_U, DC_U);
            amba_ik_system_print_uint32_5("[IK]    under_shoot_reduce_strength = %d", p_final_sharpen_fir->under_shoot_reduce_strength, DC_U, DC_U, DC_U, DC_U);
        }
    }
}

static void debug_print_fnlshpns_noise(uint32 context_id, const ik_final_sharpen_noise_t *p_final_sharpen_noise)
{
    if (p_final_sharpen_noise == NULL) {

    } else {
        if (filter_debug_log[LOG_FINAL_SHARPEN_NOISE_NOISE] == 1U) {
            ik_sharpen_noise_sharpen_fir_t shp_noise_fir;
            amba_ik_system_print_str_5("[IK] %s",__func__, DC_S, DC_S, DC_S, DC_S);
            amba_ik_system_print_uint32_5("[IK](context_id = %d)", context_id, DC_U, DC_U, DC_U, DC_U);
            amba_ik_system_print_uint32_5("[IK]    max_change_up = %d", p_final_sharpen_noise->max_change_up, DC_U, DC_U, DC_U, DC_U);
            amba_ik_system_print_uint32_5("[IK]    max_change_down = %d", p_final_sharpen_noise->max_change_down, DC_U, DC_U, DC_U, DC_U);
            amba_ik_system_print_str_5("[IK]    spatial_fir:", DC_S, DC_S, DC_S, DC_S, DC_S);
            (void) amba_ik_system_memcpy(&shp_noise_fir, &p_final_sharpen_noise->spatial_fir, sizeof(ik_sharpen_noise_sharpen_fir_t));
            amba_ik_debug_print_fstshpns_fir_common(&shp_noise_fir);
            amba_ik_system_print_str_5("[IK]    level_str_adjust :", DC_S, DC_S, DC_S, DC_S, DC_S);
            amba_ik_debug_print_level_method(&p_final_sharpen_noise->level_str_adjust);
            amba_ik_system_print_uint32_5("[IK]    level_str_adjust_not_t0_t1_level_based = %d", p_final_sharpen_noise->level_str_adjust_not_t0_t1_level_based, DC_U, DC_U, DC_U, DC_U);
            amba_ik_system_print_uint32_5("[IK]    t0 = %d", p_final_sharpen_noise->t0, DC_U, DC_U, DC_U, DC_U);
            amba_ik_system_print_uint32_5("[IK]    t1 = %d", p_final_sharpen_noise->t1, DC_U, DC_U, DC_U, DC_U);
            amba_ik_system_print_uint32_5("[IK]    alpha_min = %d", p_final_sharpen_noise->alpha_min, DC_U, DC_U, DC_U, DC_U);
            amba_ik_system_print_uint32_5("[IK]    alpha_max = %d", p_final_sharpen_noise->alpha_max, DC_U, DC_U, DC_U, DC_U);
            amba_ik_system_print_uint32_5("[IK]    directional_decide_t0 = %d", p_final_sharpen_noise->directional_decide_t0, DC_U, DC_U, DC_U, DC_U);
            amba_ik_system_print_uint32_5("[IK]    directional_decide_t1 = %d", p_final_sharpen_noise->directional_decide_t1, DC_U, DC_U, DC_U, DC_U);
            amba_ik_system_print_uint32_5("[IK]    advanced_iso.enable = %d", p_final_sharpen_noise->advanced_iso.enable, DC_U, DC_U, DC_U, DC_U);
            amba_ik_system_print_uint32_5("[IK]    advanced_iso.max_change_method = %d", p_final_sharpen_noise->advanced_iso.max_change_method, DC_U, DC_U, DC_U, DC_U);
            amba_ik_system_print_uint32_5("[IK]    advanced_iso.max_change_table = %d %d %d ...", \
                                          p_final_sharpen_noise->advanced_iso.max_change_table[0], \
                                          p_final_sharpen_noise->advanced_iso.max_change_table[1], \
                                          p_final_sharpen_noise->advanced_iso.max_change_table[2], DC_U, DC_U);
            amba_ik_system_print_uint32_5("[IK]    advanced_iso.noise_level_method = %d", p_final_sharpen_noise->advanced_iso.noise_level_method, DC_U, DC_U, DC_U, DC_U);
            amba_ik_system_print_uint32_5("[IK]    advanced_iso.noise_level_table = %d %d %d ...", \
                                          p_final_sharpen_noise->advanced_iso.noise_level_table[0], \
                                          p_final_sharpen_noise->advanced_iso.noise_level_table[1], \
                                          p_final_sharpen_noise->advanced_iso.noise_level_table[2], DC_U, DC_U);
            amba_ik_system_print_uint32_5("[IK]    advanced_iso.size = %d", p_final_sharpen_noise->advanced_iso.size, DC_U, DC_U, DC_U, DC_U);
            amba_ik_system_print_uint32_5("[IK]    advanced_iso.str_a_method = %d", p_final_sharpen_noise->advanced_iso.str_a_method, DC_U, DC_U, DC_U, DC_U);
            amba_ik_system_print_uint32_5("[IK]    advanced_iso.str_a_table = %d %d %d ...", \
                                          p_final_sharpen_noise->advanced_iso.str_a_table[0], \
                                          p_final_sharpen_noise->advanced_iso.str_a_table[1], \
                                          p_final_sharpen_noise->advanced_iso.str_a_table[2], DC_U, DC_U);
            amba_ik_system_print_uint32_5("[IK]    advanced_iso.str_b_method = %d", p_final_sharpen_noise->advanced_iso.str_b_method, DC_U, DC_U, DC_U, DC_U);
            amba_ik_system_print_uint32_5("[IK]    advanced_iso.str_b_table = %d %d %d ...", \
                                          p_final_sharpen_noise->advanced_iso.str_b_table[0], \
                                          p_final_sharpen_noise->advanced_iso.str_b_table[1], \
                                          p_final_sharpen_noise->advanced_iso.str_b_table[2], DC_U, DC_U);
        }
    }
}

static void debug_print_fnlshpns_coring(uint32 context_id, const ik_final_sharpen_coring_t *p_final_sharpen_coring)
{
    if (p_final_sharpen_coring == NULL) {

    } else {
        if (filter_debug_log[LOG_FINAL_SHARPEN_NOISE_CORING] == 1U) {
            amba_ik_system_print_str_5("[IK] %s",__func__, DC_S, DC_S, DC_S, DC_S);
            amba_ik_system_print_uint32_5("[IK](context_id = %d)", context_id, DC_U, DC_U, DC_U, DC_U);
            amba_ik_system_print_uint32_5("[IK]    coring = %d %d ... %d %d %d" \
                                          , p_final_sharpen_coring->coring[0] \
                                          , p_final_sharpen_coring->coring[1] \
                                          , p_final_sharpen_coring->coring[IK_NUM_CORING_TABLE_INDEX-3U] \
                                          , p_final_sharpen_coring->coring[IK_NUM_CORING_TABLE_INDEX-2U] \
                                          , p_final_sharpen_coring->coring[IK_NUM_CORING_TABLE_INDEX-1U]);
            amba_ik_system_print_uint32_5("[IK]    fractional_bits = %d", p_final_sharpen_coring->fractional_bits, DC_U, DC_U, DC_U, DC_U);
        }
    }
}

static void debug_print_fnlshpns_cor_idx_scl(uint32 context_id, const ik_final_sharpen_coring_idx_scale_t *p_final_sharpen_coring_idx_scale)
{
    if (p_final_sharpen_coring_idx_scale == NULL) {

    } else {
        if (filter_debug_log[LOG_FINAL_SHARPEN_NOISE_CORING_INDEX_SCALE] == 1U) {
            amba_ik_system_print_str_5("[IK] %s",__func__, DC_S, DC_S, DC_S, DC_S);
            amba_ik_system_print_uint32_5("[IK](context_id = %d)", context_id, DC_U, DC_U, DC_U, DC_U);
            amba_ik_debug_print_level_method(p_final_sharpen_coring_idx_scale);
        }
    }
}

static void debug_print_fnlshpns_min_cor_rst(uint32 context_id, const ik_final_sharpen_min_coring_result_t *p_final_sharpen_min_coring_result)
{
    if (p_final_sharpen_min_coring_result == NULL) {

    } else {
        if (filter_debug_log[LOG_FINAL_SHARPEN_NOISE_MIN_CORING_RESULT] == 1U) {
            amba_ik_system_print_str_5("[IK] %s",__func__, DC_S, DC_S, DC_S, DC_S);
            amba_ik_system_print_uint32_5("[IK](context_id = %d)", context_id, DC_U, DC_U, DC_U, DC_U);
            amba_ik_debug_print_level_method(p_final_sharpen_min_coring_result);
        }
    }
}

static void debug_print_fnlshpns_max_cor_rst(uint32 context_id, const ik_final_sharpen_max_coring_result_t *p_final_sharpen_max_coring_result)
{
    if (p_final_sharpen_max_coring_result == NULL) {

    } else {
        if (filter_debug_log[LOG_FINAL_SHARPEN_NOISE_MAX_CORING_RESULT] == 1U) {
            const ik_level_method_t *p_lvl;
            amba_ik_system_print_str_5("[IK] %s",__func__, DC_S, DC_S, DC_S, DC_S);
            amba_ik_system_print_uint32_5("[IK](context_id = %d)", context_id, DC_U, DC_U, DC_U, DC_U);
            (void) amba_ik_system_memcpy(&p_lvl, &p_final_sharpen_max_coring_result, sizeof(ik_level_t*));
            amba_ik_debug_print_level_method(p_lvl);
        }
    }
}

static void debug_print_fnlshpns_scl_cor(uint32 context_id, const ik_final_sharpen_scale_coring_t *p_final_sharpen_scale_coring)
{
    if (p_final_sharpen_scale_coring == NULL) {

    } else {
        if (filter_debug_log[LOG_FINAL_SHARPEN_NOISE_SCALE_CORING] == 1U) {
            amba_ik_system_print_str_5("[IK] %s",__func__, DC_S, DC_S, DC_S, DC_S);
            amba_ik_system_print_uint32_5("[IK](context_id = %d)", context_id, DC_U, DC_U, DC_U, DC_U);
            amba_ik_debug_print_level_method(p_final_sharpen_scale_coring);
        }
    }
}

static void debug_print_fnlshpns_both_tdt(uint32 context_id, const ik_final_sharpen_both_three_d_table_t *p_final_sharpen_both_three_d_table)
{
    if (p_final_sharpen_both_three_d_table == NULL) {

    } else {
        if (filter_debug_log[LOG_FINAL_SHARPEN_NOISE_BOTH_TDT] == 1U) {
            amba_ik_system_print_str_5("[IK] %s",__func__, DC_S, DC_S, DC_S, DC_S);
            amba_ik_system_print_uint32_5("[IK](context_id = %d)", context_id, DC_U, DC_U, DC_U, DC_U);
            amba_ik_system_print_uint32_5("[IK]    y_tone_offset = %d", p_final_sharpen_both_three_d_table->y_tone_offset, DC_U, DC_U, DC_U, DC_U);
            amba_ik_system_print_uint32_5("[IK]    y_tone_shift = %d", p_final_sharpen_both_three_d_table->y_tone_shift, DC_U, DC_U, DC_U, DC_U);
            amba_ik_system_print_uint32_5("[IK]    u_tone_offset = %d", p_final_sharpen_both_three_d_table->u_tone_offset, DC_U, DC_U, DC_U, DC_U);
            amba_ik_system_print_uint32_5("[IK]    u_tone_shift = %d", p_final_sharpen_both_three_d_table->u_tone_shift, DC_U, DC_U, DC_U, DC_U);
            amba_ik_system_print_uint32_5("[IK]    u_tone_bits = %d", p_final_sharpen_both_three_d_table->u_tone_bits, DC_U, DC_U, DC_U, DC_U);
            amba_ik_system_print_uint32_5("[IK]    v_tone_offset = %d", p_final_sharpen_both_three_d_table->v_tone_offset, DC_U, DC_U, DC_U, DC_U);
            amba_ik_system_print_uint32_5("[IK]    v_tone_shift = %d", p_final_sharpen_both_three_d_table->v_tone_shift, DC_U, DC_U, DC_U, DC_U);
            amba_ik_system_print_uint32_5("[IK]    v_tone_bits = %d", p_final_sharpen_both_three_d_table->v_tone_bits, DC_U, DC_U, DC_U, DC_U);
            amba_ik_system_print_uint32_5("[IK]    coring = ... %d %d ... %d %d %d" \
                                          , p_final_sharpen_both_three_d_table->three_d[11] \
                                          , p_final_sharpen_both_three_d_table->three_d[12] \
                                          , p_final_sharpen_both_three_d_table->three_d[8192U-3U] \
                                          , p_final_sharpen_both_three_d_table->three_d[8192U-2U] \
                                          , p_final_sharpen_both_three_d_table->three_d[8192U-1U]);
        }
    }
}

static void amba_debug_print_dzoom_info(uint32 context_id, const ik_dzoom_info_t *p_dzoom)
{
    if (p_dzoom == NULL) {

    } else {
        if (filter_debug_log[LOG_DZOOM_INFO] == 1U) {
            amba_ik_system_print_str_5("[IK] %s",__func__, DC_S, DC_S, DC_S, DC_S);
            amba_ik_system_print_uint32_5("[IK](context_id = %d)", context_id, DC_U, DC_U, DC_U, DC_U);
            amba_ik_system_print_uint32_5("[IK]    y_tone_offset = %d", p_dzoom->zoom_x, DC_U, DC_U, DC_U, DC_U);
            amba_ik_system_print_uint32_5("[IK]    y_tone_shift = %d", p_dzoom->zoom_y, DC_U, DC_U, DC_U, DC_U);
            amba_ik_system_print_int32_5("[IK]    u_tone_offset = %d", p_dzoom->shift_x, DC_I, DC_I, DC_I, DC_I);
            amba_ik_system_print_int32_5("[IK]    u_tone_shift = %d", p_dzoom->shift_y, DC_I, DC_I, DC_I, DC_I);
        }
    }
}

static void amba_ik_debug_print_window_dimension(const ik_window_dimension_t *p_win)
{
    amba_ik_system_print_uint32_5("[IK]    width = %d", p_win->width, DC_U, DC_U, DC_U, DC_U);
    amba_ik_system_print_uint32_5("[IK]    height = %d", p_win->height, DC_U, DC_U, DC_U, DC_U);
}

static void amba_ik_debug_print_window_geometry(const ik_vin_active_window_t *p_win)
{
    amba_ik_system_print_uint32_5("[IK]    enable = %d", p_win->enable, DC_U, DC_U, DC_U, DC_U);
    amba_ik_system_print_uint32_5("[IK]    start_x = %d", p_win->active_geo.start_x, DC_U, DC_U, DC_U, DC_U);
    amba_ik_system_print_uint32_5("[IK]    start_y = %d", p_win->active_geo.start_y, DC_U, DC_U, DC_U, DC_U);
    amba_ik_system_print_uint32_5("[IK]    width = %d", p_win->active_geo.width, DC_U, DC_U, DC_U, DC_U);
    amba_ik_system_print_uint32_5("[IK]    width = %d", p_win->active_geo.height, DC_U, DC_U, DC_U, DC_U);
}

static void amba_ik_debug_print_window_size_info(uint32 context_id, const ik_window_size_info_t *p_win_size)
{
    if (p_win_size==NULL) {

    } else {
        if (filter_debug_log[LOG_WINDOW_SIZE_INFO] == 1U) {
            amba_ik_system_print_str_5("[IK] %s",__func__, DC_S, DC_S, DC_S, DC_S);
            amba_ik_system_print_uint32_5("[IK](context_id = %d)", context_id, DC_U, DC_U, DC_U, DC_U);
            amba_ik_debug_print_geometry(&p_win_size->vin_sensor);
            amba_ik_system_print_str_5("[IK]    main window:", DC_S, DC_S, DC_S, DC_S, DC_S);
            amba_ik_debug_print_window_dimension(&p_win_size->main_win);
            amba_ik_system_print_str_5("[IK]    prev[0]:", DC_S, DC_S, DC_S, DC_S, DC_S);
            amba_ik_debug_print_window_dimension(&p_win_size->prev[0]);
            amba_ik_system_print_str_5("[IK]    prev[1]:", DC_S, DC_S, DC_S, DC_S, DC_S);
            amba_ik_debug_print_window_dimension(&p_win_size->prev[1]);
            amba_ik_system_print_str_5("[IK]    prev[2]:", DC_S, DC_S, DC_S, DC_S, DC_S);
            amba_ik_debug_print_window_dimension(&p_win_size->prev[2]);
            amba_ik_system_print_str_5("[IK]    thumbnail:", DC_S, DC_S, DC_S, DC_S, DC_S);
            amba_ik_debug_print_window_dimension(&p_win_size->thumbnail);
            amba_ik_system_print_str_5("[IK]    screennail:", DC_S, DC_S, DC_S, DC_S, DC_S);
            amba_ik_debug_print_window_dimension(&p_win_size->screennail);
        }
    }
}

static void amba_ik_debug_print_vin_active_win(uint32 context_id, const ik_vin_active_window_t *p_act_win)
{
    if (p_act_win == NULL) {

    } else {
        if (filter_debug_log[LOG_VIN_ACTIVE_WIN] == 1U) {
            amba_ik_system_print_str_5("[IK] %s",__func__, DC_S, DC_S, DC_S, DC_S);
            amba_ik_system_print_uint32_5("[IK](context_id = %d)", context_id, DC_U, DC_U, DC_U, DC_U);
            amba_ik_system_print_str_5("[IK]    active window:", DC_S, DC_S, DC_S, DC_S, DC_S);
            amba_ik_debug_print_window_geometry(p_act_win);
        }
    }
}

static void amba_ik_debug_print_dummy_margin(uint32 context_id, const ik_dummy_margin_range_t *p_dummy_margin)
{
    if (p_dummy_margin==NULL) {

    } else {
        if (filter_debug_log[LOG_DUMMY_MARGIN_RANGE] == 1U) {
            amba_ik_system_print_str_5("[IK] %s",__func__, DC_S, DC_S, DC_S, DC_S);
            amba_ik_system_print_uint32_5("[IK](context_id = %d)", context_id, DC_U, DC_U, DC_U, DC_U);
            amba_ik_system_print_uint32_5("[IK]    enable = %d", p_dummy_margin->enable, DC_U, DC_U, DC_U, DC_U);
            amba_ik_system_print_uint32_5("[IK]    left = %d", p_dummy_margin->left, DC_U, DC_U, DC_U, DC_U);
            amba_ik_system_print_uint32_5("[IK]    right = %d", p_dummy_margin->right, DC_U, DC_U, DC_U, DC_U);
            amba_ik_system_print_uint32_5("[IK]    top = %d", p_dummy_margin->top, DC_U, DC_U, DC_U, DC_U);
            amba_ik_system_print_uint32_5("[IK]    bottom = %d", p_dummy_margin->bottom, DC_U, DC_U, DC_U, DC_U);
        }
    }
}

static void amba_ik_debug_print_resampler_strength(uint32 context_id, const ik_resampler_strength_t *p_resampler_strength)
{
    if (p_resampler_strength==NULL) {

    } else {
        if (filter_debug_log[LOG_RESAMPLER_STR] == 1U) {
            amba_ik_system_print_str_5("[IK] %s",__func__, DC_S, DC_S, DC_S, DC_S);
            amba_ik_system_print_uint32_5("[IK](context_id = %d)", context_id, DC_U, DC_U, DC_U, DC_U);
            amba_ik_system_print_uint32_5("[IK]    cfa_cut_off_freq = %d", p_resampler_strength->cfa_cut_off_freq, DC_U, DC_U, DC_U, DC_U);
            amba_ik_system_print_uint32_5("[IK]    main_cut_off_freq = %d", p_resampler_strength->main_cut_off_freq, DC_U, DC_U, DC_U, DC_U);
        }
    }
}

void ik_hook_debug_print(void)
{
    static amba_img_dsp_debug_print_func_t debug_print_func;

    debug_print_func.safety_info = amba_ik_debug_print_safety_info;

    debug_print_func.vin_sensor_info = amba_ik_debug_print_vin_sensor_info;
    debug_print_func.fe_tone_curve = amba_ik_debug_print_fe_tone_curve;

    debug_print_func.fe_blc = amba_ik_debug_print_fe_static_blc;
    debug_print_func.fe_wb = amba_ik_debug_print_fe_wbgain;
    debug_print_func.hdr_blend = amba_ik_debug_print_hdr_blend;
    debug_print_func.vignette_enable = amba_ik_debug_print_vig_enable;
    debug_print_func.vignette_info = amba_ik_debug_print_vig;

    debug_print_func.dynamic_bad_pxl_cor = amba_ik_debug_print_dynamic_bpc;
    debug_print_func.static_bpc_enable = amba_ik_debug_print_static_bpc_enable;
    debug_print_func.static_bpc = amba_ik_debug_print_static_bpc;
    debug_print_func.cfa_leakage = amba_ik_debug_print_cfa_leakage_filter;
    debug_print_func.anti_aliasing = amba_ik_debug_print_anti_aliasing;

    debug_print_func.grgb_mismatch = amba_ik_debug_print_grgb_mismatch;
    debug_print_func.before_ce_wb = amba_ik_debug_print_before_ce_wbgain;
    debug_print_func.cawarp_enable = amba_ik_debug_print_cawarp_enb;

    debug_print_func.cawarp = NULL; //TBD
    debug_print_func.ce = amba_ik_debug_print_ce;
    debug_print_func.ce_input_tbl = amba_ik_debug_print_ce_input_table;
    debug_print_func.ce_output_tbl = amba_ik_debug_print_ce_output_table;
    debug_print_func.after_ce_wb = amba_ik_debug_print_after_ce_wbgain;

    debug_print_func.cfa_noise_filter = amba_ik_debug_print_cfa_noise_filter;
    debug_print_func.demosaic = amba_ik_debug_print_demosaic;
    debug_print_func.rgb_to_12y = amba_ik_debug_print_rgb_to_12y;
    debug_print_func.luma_noise_reduce = amba_ik_debug_print_luma_noise_reduction;
    debug_print_func.rgb_ir = amba_ik_debug_print_rgb_ir;
    debug_print_func.pre_cc = amba_ik_debug_print_pre_cc_gain;
    debug_print_func.color_correction = amba_ik_debug_print_cc;

    debug_print_func.tone_curve = amba_ik_debug_print_tone_curve;
    debug_print_func.rgb_to_yuv_matrix = amba_ik_debug_print_rgb_to_yuv_matrix;
    debug_print_func.chroma_sacle = amba_ik_debug_print_chroma_scale;
    debug_print_func.chroma_filter = amba_ik_debug_print_chroma_filter;
    debug_print_func.wide_chroma_filter = amba_ik_debug_print_wide_chroma_filter;
    debug_print_func.wide_chroma_combine = amba_ik_debug_print_wide_uv_combine;
    debug_print_func.chroma_median_filter = amba_ik_debug_print_chroma_median_filter;

    debug_print_func.first_luma_proc_mode = amba_ik_debug_print_first_luma_proc_mode;
    debug_print_func.adv_spat_fltr = amba_ik_debug_print_adv_spat_fltr;
    debug_print_func.first_sharpen_both = amba_ik_debug_print_fst_shp_ns_both;
    debug_print_func.first_sharpen_fir = debug_print_fst_shp_ns_fir;
    debug_print_func.first_sharpen_noise = debug_print_fst_shp_ns_noise;

    debug_print_func.first_sharpen_coring = debug_print_fst_shp_ns_coring;
    debug_print_func.fstshpns_cor_idx_scl = debug_print_fst_shp_ns_cor_idx_scl;
    debug_print_func.fstshpns_min_cor_rst = debug_print_fst_shp_ns_min_cor_rst;
    debug_print_func.fstshpns_max_cor_rst = debug_print_fst_shp_ns_max_cor_rst;
    debug_print_func.fstshpns_scl_cor = debug_print_fst_shp_ns_scl_cor;

    debug_print_func.warp_enable = amba_ik_debug_print_warp_enable;
    debug_print_func.video_mctf = amba_ik_debug_print_video_mctf;
    debug_print_func.video_mctf_ta = amba_ik_debug_print_video_mctf_ta;
    debug_print_func.video_mctf_and_fnlshp = debug_print_video_mctf_and_fnlshp;

    debug_print_func.final_sharpen_both = debug_print_fnlshpns_both;
    debug_print_func.final_sharpen_fir = debug_print_fnlshpns_fir;
    debug_print_func.final_sharpen_noise = debug_print_fnlshpns_noise;
    debug_print_func.final_sharpen_coring = debug_print_fnlshpns_coring;
    debug_print_func.fnlshpns_cor_idx_scl = debug_print_fnlshpns_cor_idx_scl;
    debug_print_func.fnlshpns_min_cor_rst = debug_print_fnlshpns_min_cor_rst;
    debug_print_func.fnlshpns_max_cor_rst = debug_print_fnlshpns_max_cor_rst;
    debug_print_func.fnlshpns_scl_cor = debug_print_fnlshpns_scl_cor;
    debug_print_func.final_sharpen_both_three_d_table = debug_print_fnlshpns_both_tdt;

    debug_print_func.dzoom_info = amba_debug_print_dzoom_info;
    debug_print_func.window_size_info = amba_ik_debug_print_window_size_info;
    debug_print_func.vin_active_win = amba_ik_debug_print_vin_active_win;
    debug_print_func.dummy_margin_range = amba_ik_debug_print_dummy_margin;
    debug_print_func.resamp_strength = amba_ik_debug_print_resampler_strength;

    p_debug_print_func = &debug_print_func;
}

const amba_img_dsp_debug_print_func_t* ik_get_debug_print_func(void)
{
    return p_debug_print_func;
}

void ik_init_debug_print_func(void)
{
    p_debug_print_func = NULL;
}

