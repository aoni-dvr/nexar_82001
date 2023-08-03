/*
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
#include "AmbaDSP_ImgContextDebugUtility.h"
#include "AmbaDSP_ImgArchSystemAPI.h"
#include "AmbaDSP_ImgContextComponentIF.h"
#include "AmbaDSP_ImgContextUtility.h"

static ik_debug_check_func_t *p_debug_check_function = NULL;

static uint32 check_advanced_level_control(char const *parameter_name, const ik_level_t *p_level)
{
    uint32 rval = IK_OK;
    uint32 low0, high0;

    low0 = p_level->low + (1UL<<p_level->low_delta);
    high0 = p_level->high + (1UL<<p_level->high_delta);

    rval |= (uint32)ctx_check_level_control_uint32_parameter_valid_range(parameter_name, "low", p_level->low, 0UL, low0-1UL);
    rval |= (uint32)ctx_check_level_control_uint32_parameter_valid_range(parameter_name, "low0", low0, p_level->low+1UL, p_level->high);
    rval |= (uint32)ctx_check_level_control_uint32_parameter_valid_range(parameter_name, "high", p_level->high, low0, high0-1UL);
    //rval |= (uint32)ctx_check_level_control_uint32_parameter_valid_range(parameter_name, "high0", high0, p_level->high+1UL, 255UL);
    if (rval != IK_OK) {
        amba_ik_system_print_str_5("[IK][ERROR] fail on advanced checker on level control \n", DC_S, DC_S, DC_S, DC_S, DC_S);
        amba_ik_system_print_uint32_5("low:%d, low_delta:%d, high:%d, high_delta:%d\n", p_level->low, p_level->low_delta, p_level->high, p_level->high_delta, DC_U);
        amba_ik_system_print_uint32_5("low0:%d, high0:%d\n", low0, high0, DC_U, DC_U, DC_U);
    } else {
        // misraC
    }

    return rval;
}

static uint32 ctx_check_vin_sensor_info(const ik_vin_sensor_info_t *p_vin_sensor, const char *prefix)
{
    uint32 rval = IK_OK;
    if (p_vin_sensor == NULL) {
        amba_ik_system_print_str_5("[IK][ERROR] amba_ik_debug_check_vin_sensor_info() p_vin_sensor = NULL\n", DC_S, DC_S, DC_S, DC_S, DC_S);
        rval = IK_ERR_0005;
    } else {
        rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "sensor_pattern", p_vin_sensor->sensor_pattern, 0UL, 0x3UL);
        rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "sensor_resolution", p_vin_sensor->sensor_resolution, 0UL, 0x10UL);
        rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "sensor_mode", p_vin_sensor->sensor_mode, 0UL, 0x3UL);
        rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "compression", p_vin_sensor->compression, 0UL, 259UL);
        rval |= (uint32)ctx_check_uint32_parameter_invalid_range(prefix, "compression", p_vin_sensor->compression, 9UL, 256UL);
        rval |= (uint32)ctx_check_int32_parameter_valid_range(prefix, "compression_offset", p_vin_sensor->compression_offset, -16384, 16383);
        rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "sensor_readout_mode", p_vin_sensor->sensor_readout_mode, 0UL, 131071UL);
    }
    return rval;
}

static uint32 ctx_check_fe_tone_curve(const ik_frontend_tone_curve_t *p_fe_tc, const char *prefix)
{
    uint32 rval = IK_OK;
    uint32 i;

    if (p_fe_tc==NULL) {
        amba_ik_system_print_str_5("[IK][ERROR] amba_ik_debug_check_fe_tone_curve() p_fe_tc = NULL\n", DC_S, DC_S, DC_S, DC_S, DC_S);
        rval = IK_ERR_0005;
    } else {
        rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "sensor_pattern", p_fe_tc->decompand_enable, 0UL, 0x1UL);
        if(p_fe_tc->decompand_enable == 1UL) {
            for (i = 0; i < IK_NUM_DECOMPAND_TABLE; i++) {
                rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "decompand_table", p_fe_tc->decompand_table[i], 0UL, 1048575UL);
                if(rval != IK_OK) {
                    amba_ik_system_print_uint32_5("[IK][ERROR] amba_ik_debug_check_fe_tone_curve() decompand_table[%d] = %d\n", i, p_fe_tc->decompand_table[i], DC_U, DC_U, DC_U);
                }
            }
        }
    }
    return rval;
}

static uint32 ctx_check_fe_static_blc(const ik_static_blc_level_t *p_fe_blc, uint32 exp_id, const char *prefix)
{
    uint32 rval = IK_OK;

    if (p_fe_blc==NULL) {
        amba_ik_system_print_str_5("[IK][ERROR] amba_ik_debug_check_fe_static_blc() p_fe_blc = NULL\n", DC_S, DC_S, DC_S, DC_S, DC_S);
        rval = IK_ERR_0005;
    } else {
        rval |= ctx_check_int32_parameter_valid_range(prefix, "black_r", p_fe_blc->black_r, -16384, 16383);
        rval |= ctx_check_int32_parameter_valid_range(prefix, "black_gr", p_fe_blc->black_gr, -16384, 16383);
        rval |= ctx_check_int32_parameter_valid_range(prefix, "black_gb", p_fe_blc->black_gb, -16384, 16383);
        rval |= ctx_check_int32_parameter_valid_range(prefix, "black_b", p_fe_blc->black_b, -16384, 16383);
        if(rval != IK_OK) {
            amba_ik_system_print_uint32_5("[IK][ERROR] amba_ik_debug_check_fe_static_blc(exp_id %d) NG.\n", exp_id, DC_U, DC_U, DC_U, DC_U);
        }
    }
    return rval;
}

static uint32 ctx_check_fe_wb_gain(const ik_frontend_wb_gain_t *p_fe_wb_gain, uint32 exp_id, const char *prefix)
{
    uint32 rval = IK_OK;

    if (p_fe_wb_gain == NULL) {
        amba_ik_system_print_str_5("[IK][ERROR] amba_ik_debug_check_fe_static_blc() p_fe_blc = NULL\n", DC_S, DC_S, DC_S, DC_S, DC_S);
        rval = IK_ERR_0005;
    } else {
        rval |= ctx_check_uint32_parameter_valid_range(prefix, "r_gain", p_fe_wb_gain->r_gain, 0UL, 0x400000UL);
        rval |= ctx_check_uint32_parameter_valid_range(prefix, "g_gain", p_fe_wb_gain->g_gain, 0UL, 0x400000UL);
        rval |= ctx_check_uint32_parameter_valid_range(prefix, "b_gain", p_fe_wb_gain->b_gain, 0UL, 0x400000UL);
        rval |= ctx_check_uint32_parameter_valid_range(prefix, "shutter_ratio", p_fe_wb_gain->shutter_ratio, 1UL, 16383UL);
        if (rval != IK_OK) {
            amba_ik_system_print_uint32_5("[IK][ERROR] amba_ik_debug_check_fe_wb_gain(exp_id %d) NG.\n", exp_id, DC_U, DC_U, DC_U, DC_U);
        }
    }
    return rval;
}

static uint32 ctx_check_dgain_sat_lvl(const ik_frontend_dgain_saturation_level_t *p_dgain_sat, uint32 exp_id, const char *prefix)
{
    uint32 rval = IK_OK;

    if (p_dgain_sat == NULL) {
        amba_ik_system_print_str_5("[IK][ERROR] ik_frontend_dgain_saturation_level_t() p_dgain_sat = NULL\n", DC_S, DC_S, DC_S, DC_S, DC_S);
        rval = IK_ERR_0005;
    } else {
        rval |= ctx_check_uint32_parameter_valid_range(prefix, "pre_level_red", p_dgain_sat->pre_level_red, 0, 1048575);
        rval |= ctx_check_uint32_parameter_valid_range(prefix, "pre_level_green_even", p_dgain_sat->pre_level_green_even, 0, 1048575);
        rval |= ctx_check_uint32_parameter_valid_range(prefix, "pre_level_green_odd", p_dgain_sat->pre_level_green_odd, 0, 1048575);
        rval |= ctx_check_uint32_parameter_valid_range(prefix, "pre_level_blue", p_dgain_sat->pre_level_blue, 0, 1048575);
        rval |= ctx_check_uint32_parameter_valid_range(prefix, "post_level_red", p_dgain_sat->post_level_red, 0, 1048575);
        rval |= ctx_check_uint32_parameter_valid_range(prefix, "post_level_green_even", p_dgain_sat->post_level_green_even, 0, 1048575);
        rval |= ctx_check_uint32_parameter_valid_range(prefix, "post_level_green_odd", p_dgain_sat->post_level_green_odd, 0, 1048575);
        rval |= ctx_check_uint32_parameter_valid_range(prefix, "post_level_blue", p_dgain_sat->post_level_blue, 0, 1048575);
        if (rval != IK_OK) {
            amba_ik_system_print_uint32_5("[IK][ERROR] amba_ik_debug_check_dgain_sat_lvl(exp_id %d) NG.\n", exp_id, DC_U, DC_U, DC_U, DC_U);
        }
    }
    return rval;
}

static uint32 ctx_check_hdr_blend(const ik_hdr_blend_t *p_hdr_blend, const char *prefix)
{
    uint32 rval = IK_OK;

    if (p_hdr_blend == NULL) {
        amba_ik_system_print_str_5("[IK][ERROR] amba_ik_debug_check_hdr_blend() p_fe_blc = NULL\n", DC_S, DC_S, DC_S, DC_S, DC_S);
        rval = IK_ERR_0005;
    } else {
        rval |= ctx_check_uint32_parameter_valid_range(prefix, "output_enable", p_hdr_blend->enable, 0UL, 1UL);
        rval |= ctx_check_uint32_parameter_valid_range(prefix, "delta_t0", p_hdr_blend->delta_t0, 0UL, 19UL);
        rval |= ctx_check_uint32_parameter_valid_range(prefix, "delta_t0", p_hdr_blend->delta_t1, 0UL, 19UL);
        rval |= ctx_check_uint32_parameter_valid_range(prefix, "flicker_threshold", p_hdr_blend->flicker_threshold, 0UL, 256UL);
        rval |= ctx_check_uint32_parameter_valid_range(prefix, "t0_offset", p_hdr_blend->t0_offset, 0UL, (1UL<<20U)-1UL);
        rval |= ctx_check_uint32_parameter_valid_range(prefix, "t0_offset", p_hdr_blend->t1_offset, 0UL, (1UL<<20U)-1UL);
    }
    return rval;
}

static uint32 ctx_check_vignette_compensation(const ik_vignette_t *vignette_compensation, const char *prefix)
{
    uint32 rval = IK_OK;
    if(vignette_compensation == NULL) {
        amba_ik_system_print_str_5("[IK][ERROR] amba_ik_debug_check_vignette_compensation() vignette_compensation = NULL\n", DC_S, DC_S, DC_S, DC_S, DC_S);
        rval = IK_ERR_0005;
    } else {
        rval = ctx_check_uint32_parameter_valid_range(prefix, "calib_mode_enable", vignette_compensation->calib_mode_enable, 0, 1);
        if(vignette_compensation->calib_mode_enable == 1U) {
            rval |= ctx_check_uint32_parameter_valid_range(prefix, "vin_sensor_geo.h_sub_sample.factor_den", vignette_compensation->vin_sensor_geo.h_sub_sample.factor_den, 1UL, 16UL);
            rval |= ctx_check_uint32_parameter_valid_range(prefix, "vin_sensor_geo.h_sub_sample.factor_num", vignette_compensation->vin_sensor_geo.h_sub_sample.factor_num, 1UL, 16UL);
            rval |= ctx_check_uint32_parameter_valid_range(prefix, "vin_sensor_geo.v_sub_sample.factor_den", vignette_compensation->vin_sensor_geo.v_sub_sample.factor_den, 1UL, 16UL);
            rval |= ctx_check_uint32_parameter_valid_range(prefix, "vin_sensor_geo.v_sub_sample.factor_num", vignette_compensation->vin_sensor_geo.v_sub_sample.factor_num, 1UL, 16UL);

            rval |= ctx_check_uint32_parameter_valid_range(prefix, "vin_sensor_geo.start_x", vignette_compensation->vin_sensor_geo.start_x, 0UL, 8192UL);
            rval |= ctx_check_uint32_parameter_valid_range(prefix, "vin_sensor_geo.start_y", vignette_compensation->vin_sensor_geo.start_y, 0UL, 8192UL);
            rval |= ctx_check_uint32_parameter_valid_range(prefix, "vin_sensor_geo.width", vignette_compensation->vin_sensor_geo.width, 0UL, 8192UL);
            rval |= ctx_check_uint32_parameter_valid_range(prefix, "vin_sensor_geo.height", vignette_compensation->vin_sensor_geo.height, 0UL, 8192UL);
            rval |= ctx_check_uint32_parameter_valid_range(prefix, "calib_vignette_info.version", vignette_compensation->calib_vignette_info.version, IK_VIG_VER, IK_VIG_VER);
        }
        rval |= ctx_check_uint32_parameter_valid_range(prefix, "calib_vignette_info.num_radial_bins_coarse", vignette_compensation->calib_vignette_info.num_radial_bins_coarse, 0UL, 127UL);
        rval |= ctx_check_uint32_parameter_valid_range(prefix, "calib_vignette_info.num_radial_bins_fine", vignette_compensation->calib_vignette_info.num_radial_bins_fine, 0UL, 127UL);
        rval |= ctx_check_uint32_parameter_valid_range(prefix, "calib_vignette_info.size_radial_bins_coarse_log", vignette_compensation->calib_vignette_info.size_radial_bins_coarse_log, 0UL, 8UL);
        rval |= ctx_check_uint32_parameter_valid_range(prefix, "calib_vignette_info.size_radial_bins_fine_log", vignette_compensation->calib_vignette_info.size_radial_bins_fine_log, 0UL, 8UL);

        rval |= ctx_check_uint32_parameter_valid_range(prefix, "calib_vignette_info.model_center_x_R", vignette_compensation->calib_vignette_info.model_center_x_R, 0UL, 8192UL);
        rval |= ctx_check_uint32_parameter_valid_range(prefix, "calib_vignette_info.model_center_x_Gr", vignette_compensation->calib_vignette_info.model_center_x_Gr, 0UL, 8192UL);
        rval |= ctx_check_uint32_parameter_valid_range(prefix, "calib_vignette_info.model_center_x_B", vignette_compensation->calib_vignette_info.model_center_x_B, 0UL, 8192UL);
        rval |= ctx_check_uint32_parameter_valid_range(prefix, "calib_vignette_info.model_center_x_Gb", vignette_compensation->calib_vignette_info.model_center_x_Gb, 0UL, 8192UL);

        rval |= ctx_check_uint32_parameter_valid_range(prefix, "calib_vignette_info.model_center_y_R", vignette_compensation->calib_vignette_info.model_center_y_R, 0UL, 8192UL);
        rval |= ctx_check_uint32_parameter_valid_range(prefix, "calib_vignette_info.model_center_y_Gr", vignette_compensation->calib_vignette_info.model_center_y_Gr, 0UL, 8192UL);
        rval |= ctx_check_uint32_parameter_valid_range(prefix, "calib_vignette_info.model_center_y_B", vignette_compensation->calib_vignette_info.model_center_y_B, 0UL, 8192UL);
        rval |= ctx_check_uint32_parameter_valid_range(prefix, "calib_vignette_info.model_center_y_Gb", vignette_compensation->calib_vignette_info.model_center_y_Gb, 0UL, 8192UL);

    }
    return rval;
}

static uint32 ctx_check_dynamic_bpc(const ik_dynamic_bad_pixel_correction_t *dynamic_bpc, const char *prefix)
{
    uint32 rval = IK_OK;
    if (dynamic_bpc == NULL) {
        rval |= IK_ERR_0005;
    } else {
        rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "enable", dynamic_bpc->enable, 0UL, 4UL);
        if(dynamic_bpc->enable > 0U) {
            rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "hot_pixel_strength", dynamic_bpc->hot_pixel_strength, 0UL, 10UL);
            rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "dark_pixel_strength", dynamic_bpc->dark_pixel_strength, 0UL, 10UL);
            rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "correction_method", dynamic_bpc->correction_method, 0UL, 1UL);
            rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "hot_pixel_detection_strength_irmode_redblue", dynamic_bpc->hot_pixel_detection_strength_irmode_redblue, 0UL, 10UL);
            rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "dark_pixel_detection_strength_irmode_redblue", dynamic_bpc->dark_pixel_detection_strength_irmode_redblue, 0UL, 10UL);
        }
    }
    return rval;
}

static uint32 amba_ik_debug_check_histogram_info(const ik_histogram_info_t *p_hist_info, const char *prefix)
{
    uint32 rval = IK_OK;
    uint32 i;
    if (p_hist_info == NULL) {
        rval |= IK_ERR_0005;
    } else {
        for (i = 0U; i < 8U; i++) {
            rval |= (uint32) ctx_check_uint32_parameter_valid_range(prefix, "ae_tile_mask", p_hist_info->ae_tile_mask[i], 0UL, 65536UL);
            if (rval != IK_OK) {
                amba_ik_system_print_uint32_5("[IK][ERROR] amba_ik_debug_check_histogram_info() ae_tile_mask[%d] NG.\n", i, DC_U, DC_U, DC_U, DC_U);
            }
        }
    }
    return rval;
}

static uint32 amba_ik_debug_check_aaa_pseudo_y_info(const ik_aaa_pseudo_y_info_t *p_pseudo_y_info, const char *prefix)
{
    uint32 rval = IK_OK;
    uint32 i;
    if (p_pseudo_y_info == NULL) {
        rval |= IK_ERR_0005;
    } else {
        rval |= (uint32) ctx_check_uint32_parameter_valid_range(prefix, "mode", p_pseudo_y_info->mode, 0UL, 2UL);
        rval |= (uint32) ctx_check_uint32_parameter_valid_range(prefix, "sum_shift", p_pseudo_y_info->sum_shift, 0UL, 3UL);
        for (i = 0U; i < 4U; i++) {
            rval |= (uint32) ctx_check_uint32_parameter_valid_range(prefix, "pixel_weight", p_pseudo_y_info->pixel_weight[i], 0UL, 2UL);
        }
        for (i = 0U; i < 32U; i++) {
            rval |= (uint32) ctx_check_uint32_parameter_valid_range(prefix, "tone_curve", p_pseudo_y_info->tone_curve[i], 0UL, 255UL);
        }
    }
    return rval;
}

static uint32 ctx_check_bpc_valid_range(const ik_static_bad_pxl_cor_t *static_bpc, const char *prefix)
{
    uint32 rval = IK_OK;
    /*amba_ik_sensor_subsampling_s calib_sub_smp_h, calib_sub_smp_v, cur_sub_smp_h, cur_sub_smp_v;
    calib_sub_smp_h = static_bpc->calib_sbp_info.vin_sensor_geo.h_sub_sample;
    calib_sub_smp_v = static_bpc->calib_sbp_info.vin_sensor_geo.v_sub_sample;
    cur_sub_smp_h   = static_bpc->current_vin_sensor_geo.h_sub_sample;
    cur_sub_smp_v   = static_bpc->current_vin_sensor_geo.v_sub_sample;*/

    if (static_bpc == NULL) {
        amba_ik_system_print_str_5("[IK] detect static_bpc get NULL pointer!", DC_S, DC_S, DC_S, DC_S, DC_S);
        rval |= IK_ERR_0005;
    } else {
        /* Disable mode, discarding the checks as follow.*/
        /* Check SBP calibration version*/
        rval |= ctx_check_uint32_parameter_valid_range(prefix, "vin_sensor_geo.h_sub_sample.factor_den", static_bpc->vin_sensor_geo.h_sub_sample.factor_den, 0, 255);
        rval |= ctx_check_uint32_parameter_valid_range(prefix, "vin_sensor_geo.h_sub_sample.factor_num", static_bpc->vin_sensor_geo.h_sub_sample.factor_num, 0, 255);
        rval |= ctx_check_uint32_parameter_valid_range(prefix, "vin_sensor_geo.v_sub_sample.factor_den", static_bpc->vin_sensor_geo.v_sub_sample.factor_den, 0, 255);
        rval |= ctx_check_uint32_parameter_valid_range(prefix, "vin_sensor_geo.v_sub_sample.factor_num", static_bpc->vin_sensor_geo.v_sub_sample.factor_num, 0, 255);

        if (static_bpc->calib_sbp_info.version != IK_SBP_VER) {
            amba_ik_system_print_uint32_5("[IK] SBP Correction Version 0x%X != 0x%X", static_bpc->calib_sbp_info.version, IK_SBP_VER, DC_U, DC_U, DC_U);
            rval |= IK_ERR_0008;
        }
        /* Check whether sensor window width is valid*/
        if ((static_bpc->vin_sensor_geo.width % 32UL) != 0UL) {
            amba_ik_system_print_uint32_5("[IK] Sensor window width %d must be multiple of 32", static_bpc->vin_sensor_geo.width, DC_U, DC_U, DC_U, DC_U);
            rval |= IK_ERR_0008;
        }
        /* Check whether buffer is valid*/
        if (static_bpc->calib_sbp_info.sbp_buffer == NULL) {
            amba_ik_system_print_str_5("[IK] FPN buffer pointer is NULL", DC_S, DC_S, DC_S, DC_S, DC_S);
            rval |= IK_ERR_0008;
        }
    }
    return rval;
}

static uint32 ctx_check_cfa_leakage_filter_valid_range(const ik_cfa_leakage_filter_t *cfa_leakage_filter, const char *prefix)
{
    uint32 rval = IK_OK;
    if (cfa_leakage_filter == NULL) {
        rval |= IK_ERR_0005;
    } else {
        rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "enable", cfa_leakage_filter->enable, 0, 1);
        rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "saturation_level", cfa_leakage_filter->saturation_level, 0, 16383);
    }
    return rval;
}

static uint32 ctx_check_anti_aliasing_valid_range(const ik_anti_aliasing_t *anti_aliasing, const char *prefix)
{
    uint32 rval = IK_OK;
    if (anti_aliasing == NULL) {
        rval |= IK_ERR_0005;
    } else {
        rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "enable", anti_aliasing->enable, 0, 4);
        if (anti_aliasing->enable == 4U) {
            rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "log_fractional_correct", anti_aliasing->log_fractional_correct, 0, 7);
        }
    }
    return rval;
}

static uint32 ctx_check_grgb_mismatch_valid_range(const ik_grgb_mismatch_t *grgb_mismatch, const char *prefix)
{
    uint32 rval = IK_OK;
    if (grgb_mismatch == NULL) {
        rval |= IK_ERR_0005;
    } else {
        rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "narrow_enable", grgb_mismatch->narrow_enable, 0, 1);
        rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "wide_enable", grgb_mismatch->wide_enable, 0, 1);
        rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "wide_safety", grgb_mismatch->wide_safety, 0, 256);
        rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "wide_thresh", grgb_mismatch->wide_thresh, 0, 256);
    }
    return rval;
}

static uint32 ctx_check_wb_gain_valid_range(const ik_wb_gain_t *wb_gain, const char *prefix)
{
    uint32 rval = IK_OK;
    if (wb_gain == NULL) {
        rval |= IK_ERR_0005;
    } else {
        rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "gain_b", wb_gain->gain_b, 1UL, 0xFFFFFF);
        rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "gain_g", wb_gain->gain_g, 1UL, 0xFFFFFF);
        rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "gain_r", wb_gain->gain_r, 1UL, 0xFFFFFF);
    }
    return rval;

}

static uint32 ctx_check_calib_ca_warp_info_valid_range(const ik_cawarp_info_t *calib_ca_warp_info, const char *prefix)
{
    uint32 rval = IK_OK;
    uintptr misra_uintptr;

    rval |= ctx_check_uint32_parameter_valid_range(prefix, "version", calib_ca_warp_info->version, IK_CA_VER, IK_CA_VER);
    rval |= ctx_check_uint32_parameter_valid_range(prefix, "hor_grid_num", calib_ca_warp_info->hor_grid_num, 2UL, 64UL);
    rval |= ctx_check_uint32_parameter_valid_range(prefix, "ver_grid_num", calib_ca_warp_info->ver_grid_num, 2UL, 96UL);
    rval |= ctx_check_uint32_parameter_valid_range(prefix, "tile_width_exp", calib_ca_warp_info->tile_width_exp, 0UL, 9UL);
    rval |= ctx_check_uint32_parameter_valid_range(prefix, "tile_height_exp", calib_ca_warp_info->tile_height_exp, 0UL, 9UL);
    rval |= ctx_check_uint32_parameter_valid_range(prefix, "vin_sensor_geo.width", calib_ca_warp_info->vin_sensor_geo.width, 2UL, 6000UL);
    rval |= ctx_check_uint32_parameter_valid_range(prefix, "vin_sensor_geo.height", calib_ca_warp_info->vin_sensor_geo.height, 2UL, 4000UL);
    rval |= ctx_check_uint32_parameter_valid_range(prefix, "vin_sensor_geo.h_sub_sample.factor_num", calib_ca_warp_info->vin_sensor_geo.h_sub_sample.factor_num, 1UL, 8UL);
    rval |= ctx_check_uint32_parameter_valid_range(prefix, "vin_sensor_geo.h_sub_sample.factor_den", calib_ca_warp_info->vin_sensor_geo.h_sub_sample.factor_den, 1UL, 8UL);
    rval |= ctx_check_uint32_parameter_valid_range(prefix, "vin_sensor_geo.v_sub_sample.factor_num", calib_ca_warp_info->vin_sensor_geo.v_sub_sample.factor_num, 1UL, 8UL);
    rval |= ctx_check_uint32_parameter_valid_range(prefix, "vin_sensor_geo.v_sub_sample.factor_den", calib_ca_warp_info->vin_sensor_geo.v_sub_sample.factor_den, 1UL, 8UL);
    (void)amba_ik_system_memcpy(&misra_uintptr, &calib_ca_warp_info->p_cawarp_red, sizeof(uintptr));
    if (calib_ca_warp_info->p_cawarp_red == NULL) {
        amba_ik_system_print_uint32_5("[IK] calib_ca_warp_info->p_cawarp_red %d", misra_uintptr, DC_U, DC_U, DC_U, DC_U);
        rval |= IK_ERR_0005;
    }
    (void)amba_ik_system_memcpy(&misra_uintptr, &calib_ca_warp_info->p_cawarp_blue, sizeof(uintptr));
    if (calib_ca_warp_info->p_cawarp_blue == NULL) {
        amba_ik_system_print_uint32_5("[IK] calib_ca_warp_info->p_cawarp_blue %d", misra_uintptr, DC_U, DC_U, DC_U, DC_U);
        rval |= IK_ERR_0005;
    }

    if (((calib_ca_warp_info->hor_grid_num - 1UL) << calib_ca_warp_info->tile_width_exp) < calib_ca_warp_info->vin_sensor_geo.width) {
        amba_ik_system_print_uint32_5("[IK] (hor_grid_num %d - 1) * tile_width_exp %d < Calib vin width %d", calib_ca_warp_info->hor_grid_num, (1UL << calib_ca_warp_info->tile_width_exp), calib_ca_warp_info->vin_sensor_geo.width, DC_U, DC_U);
        rval |= IK_ERR_0008;
    }

    if (((calib_ca_warp_info->ver_grid_num - 1UL) << calib_ca_warp_info->tile_height_exp) < calib_ca_warp_info->vin_sensor_geo.height) {
        amba_ik_system_print_uint32_5("[IK] (ver_grid_num %d - 1) * tile_height_exp %d < Calib vin width %d", calib_ca_warp_info->ver_grid_num, (1UL << calib_ca_warp_info->tile_height_exp), calib_ca_warp_info->vin_sensor_geo.height, DC_U, DC_U);
        rval |= IK_ERR_0008;
    }
    return rval;
}

static uint32 ctx_check_ce_valid_range(const ik_ce_t *p_ce, const char *prefix)
{
    uint32 rval = IK_OK;
    int32 i = 0, sum = 0;
    if (p_ce == NULL) {
        rval |= IK_ERR_0005;
    } else {
        rval |= ctx_check_uint32_parameter_valid_range(prefix, "enable", p_ce->enable, 0UL, 1UL);
        rval |= ctx_check_uint32_parameter_valid_range(prefix, "radius", p_ce->radius, 0UL, 64UL);
        rval |= ctx_check_uint32_parameter_valid_range(prefix, "luma_avg_method", p_ce->luma_avg_method, 0UL, 1UL);
        if (rval == IK_OK) {
            // luma_avg_weight check
            if(p_ce->luma_avg_method == 0u) { // avg. mode
                rval |= ctx_check_int32_parameter_valid_range(prefix, "luma_avg_weight_r", p_ce->luma_avg_weight_r, -128, 127);
                rval |= ctx_check_int32_parameter_valid_range(prefix, "luma_avg_weight_gr", p_ce->luma_avg_weight_gr, -128, 127);
                rval |= ctx_check_int32_parameter_valid_range(prefix, "luma_avg_weight_gb", p_ce->luma_avg_weight_gb, -128, 127);
                rval |= ctx_check_int32_parameter_valid_range(prefix, "luma_avg_weight_b", p_ce->luma_avg_weight_b, -128, 127);

                sum = (((p_ce->luma_avg_weight_r + p_ce->luma_avg_weight_gr) + p_ce->luma_avg_weight_gb) + p_ce->luma_avg_weight_b);
                if(sum > 128) {
                    amba_ik_system_print_int32_5("[IK]ERROR! the sum of luma_avg_weight r:%d gr:%d gb:%d b:%d, sum = %d should be less or equal to 128",
                                                 p_ce->luma_avg_weight_r, p_ce->luma_avg_weight_gr, p_ce->luma_avg_weight_gb, p_ce->luma_avg_weight_b, sum);
                    rval |= IK_ERR_0008;
                }
            } else { // max mode
                rval |= ctx_check_int32_parameter_valid_range(prefix, "luma_avg_weight_r", p_ce->luma_avg_weight_r, 0, 127);
                rval |= ctx_check_int32_parameter_valid_range(prefix, "luma_avg_weight_gr", p_ce->luma_avg_weight_gr, 0, 127);
                rval |= ctx_check_int32_parameter_valid_range(prefix, "luma_avg_weight_gb", p_ce->luma_avg_weight_gb, 0, 127);
                rval |= ctx_check_int32_parameter_valid_range(prefix, "luma_avg_weight_b", p_ce->luma_avg_weight_b, 0, 127);
            }
        }

        rval |= ctx_check_uint32_parameter_valid_range(prefix, "fir_enable", p_ce->fir_enable, 0UL, 1UL);
        for (i = 0; i < 3; i++) {
            rval |= ctx_check_uint32_parameter_valid_range(prefix, "fir_coeff", p_ce->fir_coeff[i], 0UL, 64UL);
        }
        rval |= ctx_check_uint32_parameter_valid_range(prefix, "coring_index_scale_shift", p_ce->coring_index_scale_shift, 0UL, 24UL);
        rval |= ctx_check_uint32_parameter_valid_range(prefix, "coring_index_scale_low", p_ce->coring_index_scale_low, 0UL, 255UL);
        rval |= ctx_check_uint32_parameter_valid_range(prefix, "coring_index_scale_low_delta", p_ce->coring_index_scale_low_delta, 0UL, 7UL);
        rval |= ctx_check_uint32_parameter_valid_range(prefix, "coring_index_scale_low_strength", p_ce->coring_index_scale_low_strength, 0UL, 255UL);
        rval |= ctx_check_uint32_parameter_valid_range(prefix, "coring_index_scale_mid_strength", p_ce->coring_index_scale_mid_strength, 0UL, 255UL);
        rval |= ctx_check_uint32_parameter_valid_range(prefix, "coring_index_scale_high", p_ce->coring_index_scale_high, 0UL, 255UL);
        rval |= ctx_check_uint32_parameter_valid_range(prefix, "coring_index_scale_high_delta", p_ce->coring_index_scale_high_delta, 0UL, 7UL);
        rval |= ctx_check_uint32_parameter_valid_range(prefix, "coring_index_scale_high_strength", p_ce->coring_index_scale_high_strength, 0UL, 255UL);
        rval |= ctx_check_uint32_parameter_valid_range(prefix, "coring_gain_shift", p_ce->coring_gain_shift, 0UL, 20UL);
        rval |= ctx_check_uint32_parameter_valid_range(prefix, "coring_gain_low", p_ce->coring_gain_low, 0UL, 255UL);
        rval |= ctx_check_uint32_parameter_valid_range(prefix, "coring_gain_low_delta", p_ce->coring_gain_low_delta, 0UL, 7UL);
        rval |= ctx_check_uint32_parameter_valid_range(prefix, "coring_gain_low_strength", p_ce->coring_gain_low_strength, 0UL, 255UL);
        rval |= ctx_check_uint32_parameter_valid_range(prefix, "coring_gain_mid_strength", p_ce->coring_gain_mid_strength, 0UL, 255UL);
        rval |= ctx_check_uint32_parameter_valid_range(prefix, "coring_gain_high", p_ce->coring_gain_high, 0UL, 255UL);
        rval |= ctx_check_uint32_parameter_valid_range(prefix, "coring_gain_high_delta", p_ce->coring_gain_high_delta, 0UL, 255UL);
        rval |= ctx_check_uint32_parameter_valid_range(prefix, "coring_gain_high_strength", p_ce->coring_gain_high_strength, 0UL, 255UL);

        rval |= ctx_check_uint32_parameter_valid_range(prefix, "boost_gain_shift", p_ce->boost_gain_shift, 0UL, 8UL);
        rval |= ctx_check_uint32_parameter_valid_range(prefix, "boost_table_size_exp", p_ce->boost_table_size_exp, 0UL, 6UL);
        for (i = 0; i < (int32)IK_CE_BOOST_TABLE_SIZE; i++) {
            rval |= ctx_check_int32_parameter_valid_range(prefix, "boost_table", p_ce->boost_table[i], -128, 127);
            if (rval != IK_OK) {
                amba_ik_system_print_uint32_5("[IK]ERROR! boost_table[%d] = %d .boost_table value should be [-128, 127]", (uint32)i, (uint32)p_ce->boost_table[i], DC_U, DC_U, DC_U);
                break;
            }
        }
        rval |= ctx_check_uint32_parameter_valid_range(prefix, "radius", p_ce->radius, 0UL, 3UL);
        rval |= ctx_check_uint32_parameter_valid_range(prefix, "epsilon", p_ce->epsilon, 0UL, 1000UL);

        if (p_ce->enable != 0U) { // advanced level control check
            ik_level_t level;

            level.low = p_ce->coring_index_scale_low;
            level.low_delta = p_ce->coring_index_scale_low_delta;
            level.high = p_ce->coring_index_scale_high;
            level.high_delta = p_ce->coring_index_scale_high_delta;
            rval |= check_advanced_level_control("contrast_enhance->coring_index_scale", &level);

            level.low = p_ce->coring_gain_low;
            level.low_delta = p_ce->coring_gain_low_delta;
            level.high = p_ce->coring_gain_high;
            level.high_delta = p_ce->coring_gain_high_delta;
            rval |= check_advanced_level_control("contrast_enhance->coring_gain", &level);
        }
    }
    return rval;
}

static uint32 ctx_check_ce_out_table_valid_range(const ik_ce_output_table_t *p_ce_out_table, const char *prefix)
{
    uint32 rval = IK_OK;
    if (p_ce_out_table == NULL) {
        rval |= IK_ERR_0005;
    } else {
        rval |= ctx_check_uint32_parameter_valid_range(prefix, "output_enable", p_ce_out_table->output_enable, 0UL, 1UL);
        rval |= ctx_check_uint32_parameter_valid_range(prefix, "output_shift", p_ce_out_table->output_shift, 0UL, 12UL);
    }
    return rval;
}

static uint32 ctx_check_ce_input_table_valid_range(const ik_ce_input_table_t *p_ce_input_table, const char *prefix)
{
    uint32 rval = IK_OK;
    int32 i;
    if (p_ce_input_table == NULL) {
        rval |= IK_ERR_0005;
    } else {
        rval |= ctx_check_uint32_parameter_valid_range(prefix, "input_enable", p_ce_input_table->input_enable, 0UL, 1UL);
        for (i = 0; i < (int32)IK_CE_INPUT_TABLE_SIZE; i++) {
            rval |= ctx_check_uint32_parameter_valid_range(prefix, "input_table", p_ce_input_table->input_table[i], 0UL, 1048575UL);
            if (rval != IK_OK) {
                amba_ik_system_print_uint32_5("[IK]ERROR! input_table[%d] = %d .input table value should be < (2^20-1)", (uint32)i, p_ce_input_table->input_table[i], DC_U, DC_U, DC_U);
                break;
            }
        }
    }
    return rval;
}

static uint32 ctx_check_ce_ext_hds_valid_range(const ik_ce_external_hds_t *p_ce_ext_hds, const char *prefix)
{
    uint32 rval = IK_OK;
    rval |= ctx_check_uint32_parameter_valid_range(prefix, "use_external_hds", (uint32)p_ce_ext_hds->mode, 0UL, 2UL);
    return rval;
}

static uint32 ctx_check_cfa_noise_filter_valid_range(const ik_cfa_noise_filter_t *cfa_noise_filter, const char *prefix)
{
    uint32 rval = IK_OK, i;
    uint32 tmp;
    if (cfa_noise_filter == NULL) {
        rval |= IK_ERR_0005;
    } else {
        rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "enable", cfa_noise_filter->enable, 0, 1);

        for (i = 0U ; i < 3U ; i++) {
            rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "extent_fine", cfa_noise_filter->extent_fine[i], 0, 256);
            rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "noise_level", cfa_noise_filter->noise_level[i], 0, 8192);
            rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "original_blend_str", cfa_noise_filter->original_blend_str[i], 0, 256);
            rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "extent_regular", cfa_noise_filter->extent_regular[i], 0, 256);
            rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "strength_fine", cfa_noise_filter->strength_fine[i], 0, 256);
        }

        rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "selectivity_regular", cfa_noise_filter->selectivity_regular, 0, 256);
        rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "selectivity_fine", cfa_noise_filter->selectivity_fine, 0, 256);

        tmp = cfa_noise_filter->selectivity_regular % 50U;
        if (tmp > 0U) {
            /*cfa_noise_filter->selectivity_regular -= (cfa_noise_filter->selectivity_regular%50);*/
            rval |= IK_ERR_0008;
        }
        tmp = cfa_noise_filter->selectivity_fine % 50U;
        if (tmp > 0U) {
            /*cfa_noise_filter->selectivity_fine -= (cfa_noise_filter->selectivity_fine%50);*/
            rval |= IK_ERR_0008;
        }
        rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "directional_enable", cfa_noise_filter->directional_enable, 0, 1);
        if(cfa_noise_filter->directional_enable == 1U) {
            rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "directional_horvert_edge_strength", cfa_noise_filter->directional_horvert_edge_strength, 8, 255);
            rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "directional_horvert_edge_strength_bias", cfa_noise_filter->directional_horvert_strength_bias, 0, 65535);
            rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "directional_horvert_original_blend_strength", cfa_noise_filter->directional_horvert_original_blend_strength, 0, 256);
        }
    }
    return rval;
}

static uint32 ctx_check_demosaic_valid_range(const ik_demosaic_t *demosaic, const char *prefix)
{
    uint32 rval = IK_OK;
    if (demosaic == NULL) {
        rval |= IK_ERR_0005;
    } else {
        rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "enable", demosaic->enable, 0, 1);
        rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "activity_thresh", demosaic->activity_thresh, 0, 31);
        rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "activity_difference_thresh", demosaic->activity_difference_thresh, 0, 16383);
        rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "grad_clip_thresh", demosaic->grad_clip_thresh, 0, 4095);
        rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "grad_noise_thresh", demosaic->grad_noise_thresh, 0, 4095);
        rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "alias_interpolation_strength", demosaic->alias_interpolation_strength, 0, 38);
    }
    return rval;
}

static uint32 ctx_check_color_correction_reg_valid_range(const ik_color_correction_reg_t *color_correction_reg, const char *prefix)
{
    uint32 rval = IK_OK;

    if ((color_correction_reg == NULL) || (prefix == NULL)) {
        rval = IK_ERR_0005;
    }

    return rval;
}

static uint32 ctx_check_color_correction_valid_range(const ik_color_correction_t *color_correction, const char *prefix)
{
    uint32 rval = IK_OK;

    if ((color_correction == NULL) || (prefix == NULL)) {
        rval = IK_ERR_0005;
    }

    return rval;
}

static uint32 ctx_check_pre_cc_gain_valid_range(const ik_pre_cc_gain_t *pre_cc_gain, const char *prefix)
{
    uint32 rval = IK_OK;

    if (pre_cc_gain == NULL) {
        amba_ik_system_print_str_5("[IK] detect pre_cc_gain get NULL pointer!", DC_S, DC_S, DC_S, DC_S, DC_S);
        rval = IK_ERR_0005;
    } else {
        rval |= ctx_check_uint32_parameter_valid_range(prefix, "pre_cc_gain_r", pre_cc_gain->pre_cc_gain_r, 0UL, 0x1FFFFUL);
        rval |= ctx_check_uint32_parameter_valid_range(prefix, "pre_cc_gain_g", pre_cc_gain->pre_cc_gain_g, 0UL, 0x1FFFFUL);
        rval |= ctx_check_uint32_parameter_valid_range(prefix, "pre_cc_gain_b", pre_cc_gain->pre_cc_gain_b, 0UL, 0x1FFFFUL);
        if (rval != IK_OK) {
            amba_ik_system_print_uint32_5("[IK][ERROR] pre_cc_gain check value NG.\n", DC_U, DC_U, DC_U, DC_U, DC_U);
        }
    }

    return rval;
}

static uint32 ctx_check_rgb_ir_valid_range(const ik_rgb_ir_t *rgb_ir, const char *prefix)
{
    uint32 rval = IK_OK;
    if (rgb_ir == NULL) {
        amba_ik_system_print_str_5("[IK] rgb_ir input address is NULL", NULL, NULL, NULL, NULL, NULL);
        rval |= IK_ERR_0005;
    } else {
        if ((rgb_ir->mode != 0U) && (rgb_ir->mode != 1U) &&(rgb_ir->mode != 2U)) {
            amba_ik_system_print_uint32_5("[IK] rgb_ir->mode: %d should be [0, 1, 2]", rgb_ir->mode, DC_U, DC_U, DC_U, DC_U);
            rval |= IK_ERR_0008;
        }
        rval |= (uint32)ctx_check_int32_parameter_valid_range(prefix, "ircorrect_offset_b", rgb_ir->ircorrect_offset_b, -16384, 16383);
        rval |= (uint32)ctx_check_int32_parameter_valid_range(prefix, "ircorrect_offset_gb", rgb_ir->ircorrect_offset_gb, -16384, 16383);
        rval |= (uint32)ctx_check_int32_parameter_valid_range(prefix, "ircorrect_offset_gr", rgb_ir->ircorrect_offset_gr, -16384, 16383);
        rval |= (uint32)ctx_check_int32_parameter_valid_range(prefix, "ircorrect_offset_r", rgb_ir->ircorrect_offset_r, -16384, 16383);
        rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "mul_base_val", rgb_ir->mul_base_val, 0, 511);
        rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "mul_delta_high", rgb_ir->mul_delta_high, 0, 14);
        rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "mul_high", rgb_ir->mul_high, 0, 16383);
        rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "mul_high_val", rgb_ir->mul_high_val, 0, 511);

//        rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "scale_for_wb", rgb_ir->scale_for_wb, 0, 256);
//        rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "per_color_mul", rgb_ir->per_color_mul, 0, 1);
//        rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "mul_high_r", rgb_ir->mul_high_r, 0, 16383);
//        rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "mul_delta_high_r", rgb_ir->mul_delta_high_r, 0, 14);
//        rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "mul_base_val_r", rgb_ir->mul_base_val_r, 0, 511);
//        rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "mul_high_val_r", rgb_ir->mul_high_val_r, 0, 511);
//        rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "mul_high_gr", rgb_ir->mul_high_gr, 0, 16383);
//        rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "mul_delta_high_gr", rgb_ir->mul_delta_high_gr, 0, 14);
//        rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "mul_base_val_gr", rgb_ir->mul_base_val_gr, 0, 511);
//        rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "mul_high_val_gr", rgb_ir->mul_high_val_gr, 0, 511);
//        rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "mul_high_gb", rgb_ir->mul_high_gb, 0, 16383);
//        rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "mul_delta_high_gb", rgb_ir->mul_delta_high_gb, 0, 14);
//        rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "mul_base_val_gb", rgb_ir->mul_base_val_gb, 0, 511);
//        rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "mul_high_val_gb", rgb_ir->mul_high_val_gb, 0, 511);
//        rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "mul_high_b", rgb_ir->mul_high_b, 0, 16383);
//        rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "mul_delta_high_b", rgb_ir->mul_delta_high_b, 0, 14);
//        rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "mul_base_val_b", rgb_ir->mul_base_val_b, 0, 511);
//        rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "mul_high_val_b", rgb_ir->mul_high_val_b, 0, 511);

    }
    return rval;
}

static uint32 ctx_check_tone_curve_valid_range(const ik_tone_curve_t *tone_curve, const char *prefix)
{
    uint32 rval = IK_OK, i;
    if (tone_curve == NULL) {
        rval |= IK_ERR_0005;
    } else {
        for (i = 0U; i < 256U; i++) {
            rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "tone_curve_red", tone_curve->tone_curve_red[i], 0, 1023);
            rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "tone_curve_green", tone_curve->tone_curve_green[i], 0, 1023);
            rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "tone_curve_blue", tone_curve->tone_curve_blue[i], 0, 1023);
        }
    }
    return rval;
}

static uint32 ctx_check_rgb_to_yuv_matrix_valid_range(const ik_rgb_to_yuv_matrix_t *rgb_to_yuv_matrix, const char *prefix)
{
    uint32 rval = IK_OK, i;
    if (rgb_to_yuv_matrix == NULL) {
        amba_ik_system_print_str_5("[IK] rgb_to_yuv_matrix input address is NULL", NULL, NULL, NULL, NULL, NULL);
        rval |= IK_ERR_0005;
    } else {
        for (i = 0U; i < 9U; i++) {
            rval |= (uint32)ctx_check_int32_parameter_valid_range(prefix, "matrix_values[]", rgb_to_yuv_matrix->matrix_values[i], -4096, 4095);
        }

        rval |= (uint32)ctx_check_int32_parameter_valid_range(prefix, "y_offset", rgb_to_yuv_matrix->y_offset, -1024, 1023);
        rval |= (uint32)ctx_check_int32_parameter_valid_range(prefix, "u_offset", rgb_to_yuv_matrix->u_offset, -256, 255);
        rval |= (uint32)ctx_check_int32_parameter_valid_range(prefix, "v_offset", rgb_to_yuv_matrix->v_offset, -256, 255);
    }
    return rval;
}

static uint32 ctx_check_chroma_scale_valid_range(const ik_chroma_scale_t *chroma_scale, const char *prefix)
{
    uint32 rval = IK_OK, i;
    if (chroma_scale == NULL) {
        amba_ik_system_print_str_5("[IK] chroma_scale input address is NULL", NULL, NULL, NULL, NULL, NULL);
        rval |= IK_ERR_0005;
    } else {
        rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "enable", chroma_scale->enable, 0, 1);
        for (i = 0U; i < IK_NUM_CHROMA_GAIN_CURVE; i++) {
            rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "gain_curve[]", chroma_scale->gain_curve[i], 0, 4095);
        }
    }
    return rval;
}

static uint32 ctx_check_chroma_filter_valid_range(const ik_chroma_filter_t *chroma_filter, const char *prefix)
{
    uint32 rval = IK_OK;
    if (chroma_filter == NULL) {
        rval |= IK_ERR_0005;
    } else {
        rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "enable", chroma_filter->enable, 0, 1);
        rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "noise_level_cb", chroma_filter->noise_level_cb, 0, 255);
        rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "noise_level_cr", chroma_filter->noise_level_cr, 0, 255);
        rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "original_blend_strength_cb", chroma_filter->original_blend_strength_cb, 0, 256);
        rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "original_blend_strength_cr", chroma_filter->original_blend_strength_cr, 0, 256);

        if ((chroma_filter->radius != 32U) && (chroma_filter->radius != 64U) && (chroma_filter->radius != 128U)) {
            amba_ik_system_print_uint32_5("[IK] chroma_filter->radius should be [32, 64, 128], %d is invalid", chroma_filter->radius, DC_U, DC_U, DC_U, DC_U);
            rval |= IK_ERR_0008;
        }
    }

    return rval;
}

static uint32 ctx_check_chroma_median_filter_valid_range(const ik_chroma_median_filter_t *chroma_median_filter, const char *prefix)
{
    uint32 rval = IK_OK;
    if (chroma_median_filter == NULL) {
        amba_ik_system_print_str_5("[IK] chroma_median_filter input address is NULL", NULL, NULL, NULL, NULL, NULL);
        rval |= IK_ERR_0005;
    } else {
        rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "enable", chroma_median_filter->enable, 0, 1);
        rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "cb_adaptive_strength", chroma_median_filter->cb_adaptive_strength, 0, 256);
        rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "cr_adaptive_strength", chroma_median_filter->cr_adaptive_strength, 0, 256);
        rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "cb_non_adaptive_strength", chroma_median_filter->cb_non_adaptive_strength, 0, 31);
        rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "cr_non_adaptive_strength", chroma_median_filter->cr_non_adaptive_strength, 0, 31);
        rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "cb_adaptive_amount", chroma_median_filter->cb_adaptive_amount, 0, 256);
        rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "cr_adaptive_amount", chroma_median_filter->cb_adaptive_amount, 0, 256);
    }
    return rval;
}

static uint32 ctx_check_first_luma_processing_mode_valid_range(const ik_first_luma_process_mode_t *first_luma_process_mode, const char *prefix)
{
    uint32 rval = IK_OK;
    if (first_luma_process_mode == NULL) {
        amba_ik_system_print_str_5("[IK] first_luma_process_mode input is NULL address!", NULL, NULL, NULL, NULL, NULL);
        rval |= IK_ERR_0005;
    } else {
        rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "use_sharpen_not_asf", first_luma_process_mode->use_sharpen_not_asf, 0, 1);
    }
    return rval;
}

static uint32 ctx_check_advance_spatial_filter_valid_range(const ik_adv_spatial_filter_t *advance_spatial_filter, const char *prefix)
{
    uint32 rval = IK_OK, i, j;
    const ik_asf_fir_t *fir;
    if (advance_spatial_filter == NULL) {
        amba_ik_system_print_str_5("[IK] advance_spatial_filter input is NULL address!", NULL, NULL, NULL, NULL, NULL);
        rval |= IK_ERR_0005;
    } else {
        if (advance_spatial_filter->enable!=0U) {
            rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "enable", advance_spatial_filter->enable, 0, 1);
            fir = &advance_spatial_filter->fir;
            rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "fir->specify", fir->specify, 2, 4);
            rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "fir->strength_iso", fir->strength_iso, 0, 256);
            rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "fir->strength_dir", fir->strength_dir, 0, 256);
            for (i = 0U; i < 9U; i++) {
                rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "fir->per_dir_fir_iso_strengths[]", fir->per_dir_fir_iso_strengths[i], 0, 256);
                rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "fir->per_dir_fir_dir_strengths[]", fir->per_dir_fir_dir_strengths[i], 0, 256);
                rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "fir->per_dir_fir_dir_amounts[]", fir->per_dir_fir_dir_amounts[i], 0, 256);
            }
            for (i = 0U; i < 9U; i++) {
                for (j = 0U; j < 25U; j++) {
                    rval |= (uint32)ctx_check_int32_parameter_valid_range(prefix, "fir->coefs[]", fir->coefs[i][j], -1024, 1023);
                }
            }
            rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "adapt->alpha_max_down", advance_spatial_filter->adapt.alpha_max_down, 0, 8);
            rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "adapt->alpha_max_up", advance_spatial_filter->adapt.alpha_max_up, 0, 8);
            rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "adapt->alpha_min_down", advance_spatial_filter->adapt.alpha_min_down, 0, 8);
            rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "adapt->alpha_min_up", advance_spatial_filter->adapt.alpha_min_up, 0, 8);
            rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "adapt->t0_down", advance_spatial_filter->adapt.t0_down, 0, 252);
            if ((advance_spatial_filter->adapt.t0_down % 2U) != 0U) {
                amba_ik_system_print_uint32_5("[IK] advance_spatial_filter->adapt.t0_down = %d should be even", advance_spatial_filter->adapt.t0_down, DC_U, DC_U, DC_U, DC_U);
                rval |= IK_ERR_0008;
            }

            rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "adapt->t0_up", advance_spatial_filter->adapt.t0_up, 0U, 254U);
            if ((advance_spatial_filter->adapt.t0_down % 2U) != 0U) {
                amba_ik_system_print_uint32_5("[IK] advance_spatial_filter->adapt.t0_up = %d should be even", advance_spatial_filter->adapt.t0_up, DC_U, DC_U, DC_U, DC_U);
                rval |= IK_ERR_0008;
            }

            rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "adapt->t1_down", advance_spatial_filter->adapt.t1_down, 2U, 254U);
            if ((advance_spatial_filter->adapt.t1_down % 2U) != 0U) {
                amba_ik_system_print_uint32_5("[IK] advance_spatial_filter->adapt.t1_down = %d should be even", advance_spatial_filter->adapt.t1_down, DC_U, DC_U, DC_U, DC_U);
                rval |= IK_ERR_0008;
            }

            rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "adapt->t1_up", advance_spatial_filter->adapt.t1_up, 2U, 254U);
            if ((advance_spatial_filter->adapt.t0_down % 2U) != 0U) {
                amba_ik_system_print_uint32_5("[IK] advance_spatial_filter->adapt.t1_up = %d should be even", advance_spatial_filter->adapt.t1_up, DC_U, DC_U, DC_U, DC_U);
                rval |= IK_ERR_0008;
            }

            if (advance_spatial_filter->adapt.t0_up > advance_spatial_filter->adapt.t1_up) {
                amba_ik_system_print_uint32_5("[IK] advance_spatial_filter->adapt->T1_up=%d should larger than T0_up= %d in ctx_check_advance_spatial_filter_valid_range", advance_spatial_filter->adapt.t1_up, advance_spatial_filter->adapt.t0_up, DC_U, DC_U, DC_U);
                rval |= IK_ERR_0008;
            }

            if (advance_spatial_filter->adapt.t0_down > advance_spatial_filter->adapt.t1_down) {
                amba_ik_system_print_uint32_5("[IK] advance_spatial_filter->adapt->t1_down=%d should larger than t0_down= %d in ctx_check_advance_spatial_filter_valid_range", advance_spatial_filter->adapt.t1_down, advance_spatial_filter->adapt.t0_down, DC_U, DC_U, DC_U);
                rval |= IK_ERR_0008;
            }

            rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "level_str_adjust->high", advance_spatial_filter->level_str_adjust.high, 1UL, 254UL);
            rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "level_str_adjust->low", advance_spatial_filter->level_str_adjust.low, 0UL, 253UL);
            rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "level_str_adjust->low_delta", advance_spatial_filter->level_str_adjust.low_delta, 0UL, 7UL);
            rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "level_str_adjust->low_strength", advance_spatial_filter->level_str_adjust.low_strength, 0, 64);
            rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "level_str_adjust->high_delta", advance_spatial_filter->level_str_adjust.high_delta, 0UL, 7UL);
            rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "level_str_adjust->high_strength", advance_spatial_filter->level_str_adjust.high_strength, 0, 64);
            rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "level_str_adjust->mid_strength", advance_spatial_filter->level_str_adjust.mid_strength, 0, 64);

            {
                // advanced level control check
                ik_level_t level;

                level.low = advance_spatial_filter->level_str_adjust.low;
                level.low_delta = advance_spatial_filter->level_str_adjust.low_delta;
                level.high = advance_spatial_filter->level_str_adjust.high;
                level.high_delta = advance_spatial_filter->level_str_adjust.high_delta;
                rval |= check_advanced_level_control("advance_spatial_filter->level_str_adjust", &level);
            }
        }
    }
    return rval;
}

static uint32 ctx_check_fstshpns_both_valid_range(const ik_first_sharpen_both_t *first_sharpen_both, const char *prefix)
{
    uint32 rval = IK_OK;
    if (first_sharpen_both == NULL) {
        amba_ik_system_print_str_5("[IK] detect first_sharpen_both get NULL pointer!", NULL, NULL, NULL, NULL, NULL);
        rval |= IK_ERR_0005;
    } else {
        rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "enable", first_sharpen_both->enable, 0, 1);
        rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "mode", first_sharpen_both->mode, 0, 2);
        rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "edge_thresh", first_sharpen_both->edge_thresh, 0, 2047);
        rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "wide_edge_detect", first_sharpen_both->wide_edge_detect, 0, 8);

        rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "max_change.up5x5", first_sharpen_both->max_change_up5x5, 0, 255);
        rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "max_change.down5x5", first_sharpen_both->max_change_down5x5, 0, 255);
    }
    return rval;

}

static uint32 ctx_check_fstshpns_noise_valid_range(const ik_first_sharpen_noise_t *first_sharpen_noise, const char *prefix)
{
    uint32 rval = IK_OK;
    uint32 i = 0, j = 0;
    int32 tmp_int32;
    if (first_sharpen_noise == NULL) {
        amba_ik_system_print_str_5("[IK] detect first_sharpen_noise get NULL pointer!", NULL, NULL, NULL, NULL, NULL);
        rval |= IK_ERR_0005;
    } else {
        rval |= ctx_check_uint32_parameter_valid_range(prefix, "max_change_up", first_sharpen_noise->max_change_up, 0, 255);
        rval |= ctx_check_uint32_parameter_valid_range(prefix, "max_change_down", first_sharpen_noise->max_change_down, 0, 255);
        rval |= ctx_check_uint32_parameter_valid_range(prefix, "spatial_fir.specify", first_sharpen_noise->spatial_fir.specify, 0, 4);
        if (first_sharpen_noise->spatial_fir.specify == 0U) {
            rval |= ctx_check_uint32_parameter_valid_range(prefix, "spatial_fir.strength_iso", first_sharpen_noise->spatial_fir.strength_iso, 0, 256);
        } else if (first_sharpen_noise->spatial_fir.specify == 1U) {
            for (i = 0U; i < 25U; i++) {
                rval |= ctx_check_int32_parameter_valid_range(prefix, "spatial_fir.coefs", first_sharpen_noise->spatial_fir.coefs[0][i], -1024, 1023);
            }
        } else if (first_sharpen_noise->spatial_fir.specify == 2U) {
            rval |= ctx_check_uint32_parameter_valid_range(prefix, "spatial_fir.strength_iso", first_sharpen_noise->spatial_fir.strength_iso, 0, 256);
            rval |= ctx_check_uint32_parameter_valid_range(prefix, "spatial_fir.strength_dir", first_sharpen_noise->spatial_fir.strength_dir, 0, 256);
        } else if (first_sharpen_noise->spatial_fir.specify == 3U) {
            for (i = 0U; i < 9U; i++) {
                rval |= ctx_check_uint32_parameter_valid_range(prefix, "spatial_fir.per_dir_fir_dir_amounts", first_sharpen_noise->spatial_fir.per_dir_fir_dir_amounts[i], 0, 256);
                rval |= ctx_check_uint32_parameter_valid_range(prefix, "spatial_fir.per_dir_fir_dir_strengths", first_sharpen_noise->spatial_fir.per_dir_fir_dir_strengths[i], 0, 256);
                rval |= ctx_check_uint32_parameter_valid_range(prefix, "spatial_fir.per_dir_fir_iso_strengths", first_sharpen_noise->spatial_fir.per_dir_fir_iso_strengths[i], 0, 256);
            }
        } else if (first_sharpen_noise->spatial_fir.specify == 4U) {
            for (j = 0U; j < 9U; j++) {
                for (i = 0U; i < 25U; i++) {
                    rval |= ctx_check_int32_parameter_valid_range(prefix, "spatial_fir.coefs", first_sharpen_noise->spatial_fir.coefs[j][i], -1024, 1023);
                }
            }
        } else {
            rval |= IK_ERR_0008;
        }
        rval |= ctx_check_uint32_parameter_valid_range(prefix, "level_str_adjust.low", first_sharpen_noise->level_str_adjust.low, 0UL, 253UL);
        rval |= ctx_check_uint32_parameter_valid_range(prefix, "level_str_adjust.low_delta", first_sharpen_noise->level_str_adjust.low_delta, 0UL, 7UL);
        rval |= ctx_check_uint32_parameter_valid_range(prefix, "level_str_adjust.high", first_sharpen_noise->level_str_adjust.high, 1UL, 254UL);
        rval |= ctx_check_uint32_parameter_valid_range(prefix, "level_str_adjust.high_delta", first_sharpen_noise->level_str_adjust.high_delta, 0UL, 7UL);
        rval |= ctx_check_uint32_parameter_valid_range(prefix, "level_str_adjust.low_strength", first_sharpen_noise->level_str_adjust.low_strength, 0UL, 255UL);
        rval |= ctx_check_uint32_parameter_valid_range(prefix, "level_str_adjust.mid_strength", first_sharpen_noise->level_str_adjust.mid_strength, 0UL, 255UL);
        rval |= ctx_check_uint32_parameter_valid_range(prefix, "level_str_adjust.high_strength", first_sharpen_noise->level_str_adjust.high_strength, 0UL, 255UL);
        if (first_sharpen_noise->t0 > first_sharpen_noise->t1) {
            amba_ik_system_print_uint32_5("first_sharpen_noise T0=%d  should not larger than T1=%d ", first_sharpen_noise->t0, first_sharpen_noise->t1, DC_U, DC_U, DC_U);
            rval |= IK_ERR_0008;
        }
        tmp_int32 = (int32)first_sharpen_noise->t1 - (int32)first_sharpen_noise->t0;
        if (tmp_int32 > 15) {
            amba_ik_system_print_uint32_5("T1=%d should not larger than T0=%d over 15", first_sharpen_noise->t1, first_sharpen_noise->t0, DC_U, DC_U, DC_U);
            rval |= IK_ERR_0008;
        }

        {
            // advanced level control check
            ik_level_t level;

            level.low = first_sharpen_noise->level_str_adjust.low;
            level.low_delta = first_sharpen_noise->level_str_adjust.low_delta;
            level.high = first_sharpen_noise->level_str_adjust.high;
            level.high_delta = first_sharpen_noise->level_str_adjust.high_delta;
            rval |= check_advanced_level_control("first_sharpen_noise->level_str_adjust", &level);
        }
    }
    return rval;
}

static uint32 ctx_check_fstshpns_fir_valid_range(const ik_first_sharpen_fir_t *first_sharpen_fir, const char *prefix)
{
    uint32 rval = IK_OK;
    uint32  i = 0, j = 0;
    if (first_sharpen_fir == NULL) {
        amba_ik_system_print_str_5("[IK] detect first_sharpen_fir get NULL pointer!", NULL, NULL, NULL, NULL, NULL);
        rval |= IK_ERR_0005;
    } else {
        rval |= ctx_check_uint32_parameter_valid_range(prefix, "specify", first_sharpen_fir->specify, 0, 4);
        if (first_sharpen_fir->specify == 0U) {
            rval |= ctx_check_uint32_parameter_valid_range(prefix, "strength_iso", first_sharpen_fir->strength_iso, 0, 256);
        } else if (first_sharpen_fir->specify == 1U) {
            for (j = 0U; j < 9U; j++) {
                for (i = 0U; i < 25U; i++) {
                    rval |= ctx_check_int32_parameter_valid_range(prefix, "coefs", first_sharpen_fir->coefs[j][i], -1024, 1023);
                }
            }
        } else if (first_sharpen_fir->specify == 2U) {
            rval |= ctx_check_uint32_parameter_valid_range(prefix, "strength_iso", first_sharpen_fir->strength_iso, 0, 256);
            rval |= ctx_check_uint32_parameter_valid_range(prefix, "strength_dir", first_sharpen_fir->strength_dir, 0, 256);
        } else if (first_sharpen_fir->specify == 3U) {
            for (i = 0U; i < 9U; i++) {
                rval |= ctx_check_uint32_parameter_valid_range(prefix, "per_dir_fir_dir_amounts", first_sharpen_fir->per_dir_fir_dir_amounts[i], 0, 256);
                rval |= ctx_check_uint32_parameter_valid_range(prefix, "per_dir_fir_dir_strengths", first_sharpen_fir->per_dir_fir_dir_strengths[i], 0, 256);
                rval |= ctx_check_uint32_parameter_valid_range(prefix, "per_dir_fir_iso_strengths", first_sharpen_fir->per_dir_fir_iso_strengths[i], 0, 256);
            }
        } else if (first_sharpen_fir->specify == 4U) {
            for (j = 0U; j < 9U; j++) {
                for (i = 0U; i < 25U; i++) {
                    rval |= ctx_check_int32_parameter_valid_range(prefix, "coefs", first_sharpen_fir->coefs[j][i], -1024, 1023);
                }
            }
        } else {
            rval |= IK_ERR_0008;
        }
    }
    return rval;
}

static uint32 ctx_check_fstshpns_coring_valid_range(const ik_first_sharpen_coring_t *first_sharpen_coring, const char *prefix)
{
    uint32 rval = IK_OK;
    uint32 i = 0;
    if (first_sharpen_coring == NULL) {
        amba_ik_system_print_str_5("[IK] detect first_sharpen_coring get NULL pointer!", NULL, NULL, NULL, NULL, NULL);
        rval |= IK_ERR_0005;
    } else {
        for (i = 0U; i < 256U; i++) {
            rval |= ctx_check_uint32_parameter_valid_range(prefix, "coring", first_sharpen_coring->coring[i], 0, 31);
            rval |= ctx_check_uint32_parameter_valid_range(prefix, "fractional_bits", first_sharpen_coring->fractional_bits, 1UL, 3UL);
        }
    }
    return rval;
}

static uint32 ctx_check_fstshpns_coring_index_scale_valid_range(const ik_first_sharpen_coring_idx_scale_t *fstshpns_cor_idx_scl, const char *prefix)
{
    uint32 rval = IK_OK;
    if (fstshpns_cor_idx_scl == NULL) {
        amba_ik_system_print_str_5("[IK] detect first_sharpen_coring_idx_scale get NULL pointer!", NULL, NULL, NULL, NULL, NULL);
        rval |= IK_ERR_0005;
    } else {
        rval |= ctx_check_uint32_parameter_valid_range(prefix, "low", fstshpns_cor_idx_scl->low, 0UL, 253UL);
        rval |= ctx_check_uint32_parameter_valid_range(prefix, "low_delta", fstshpns_cor_idx_scl->low_delta, 0UL, 7UL);
        rval |= ctx_check_uint32_parameter_valid_range(prefix, "high_strength", fstshpns_cor_idx_scl->low_strength, 0UL, 255UL);
        rval |= ctx_check_uint32_parameter_valid_range(prefix, "high_strength", fstshpns_cor_idx_scl->mid_strength, 0UL, 255UL);
        rval |= ctx_check_uint32_parameter_valid_range(prefix, "high", fstshpns_cor_idx_scl->high, 1, 254);
        rval |= ctx_check_uint32_parameter_valid_range(prefix, "high_delta", fstshpns_cor_idx_scl->high_delta, 0UL, 7UL);
        rval |= ctx_check_uint32_parameter_valid_range(prefix, "high_strength", fstshpns_cor_idx_scl->high_strength, 0UL, 255UL);

        {
            // advanced level control check
            ik_level_t level;

            level.low = fstshpns_cor_idx_scl->low;
            level.low_delta = fstshpns_cor_idx_scl->low_delta;
            level.high = fstshpns_cor_idx_scl->high;
            level.high_delta = fstshpns_cor_idx_scl->high_delta;
            rval |= check_advanced_level_control("first_sharpen_coring_idx_scale", &level);
        }
    }
    return rval;
}

static uint32 ctx_check_fstshpns_min_coring_result_valid_range(const ik_first_sharpen_min_coring_result_t *fstshpns_min_cor_rst, const char *prefix)
{
    uint32 rval = IK_OK;
    if (fstshpns_min_cor_rst == NULL) {
        amba_ik_system_print_str_5("[IK] detect first_sharpen_min_coring_result get NULL pointer!", NULL, NULL, NULL, NULL, NULL);
        rval |= IK_ERR_0005;
    } else {
        rval |= ctx_check_uint32_parameter_valid_range(prefix, "low", fstshpns_min_cor_rst->low, 0UL, 253UL);
        rval |= ctx_check_uint32_parameter_valid_range(prefix, "low_delta", fstshpns_min_cor_rst->low_delta, 0UL, 7UL);
        rval |= ctx_check_uint32_parameter_valid_range(prefix, "low_strength", fstshpns_min_cor_rst->low_strength, 0UL, 255UL);
        rval |= ctx_check_uint32_parameter_valid_range(prefix, "mid_strength", fstshpns_min_cor_rst->mid_strength, 0UL, 255UL);
        rval |= ctx_check_uint32_parameter_valid_range(prefix, "high", fstshpns_min_cor_rst->high, 1, 254);
        rval |= ctx_check_uint32_parameter_valid_range(prefix, "high_delta", fstshpns_min_cor_rst->high_delta, 0UL, 7UL);
        rval |= ctx_check_uint32_parameter_valid_range(prefix, "high_strength", fstshpns_min_cor_rst->high_strength, 0UL, 255UL);

        {
            // advanced level control check
            ik_level_t level;

            level.low = fstshpns_min_cor_rst->low;
            level.low_delta = fstshpns_min_cor_rst->low_delta;
            level.high = fstshpns_min_cor_rst->high;
            level.high_delta = fstshpns_min_cor_rst->high_delta;
            rval |= check_advanced_level_control("first_sharpen_min_coring_result", &level);
        }
    }
    return rval;
}

static uint32 ctx_check_fstshpns_max_coring_result_valid_range(const ik_first_sharpen_max_coring_result_t *fstshpns_max_cor_rst, const char *prefix)
{
    uint32 rval = IK_OK;
    if (fstshpns_max_cor_rst == NULL) {
        amba_ik_system_print_str_5("[IK] detect first_sharpen_max_coring_result get NULL pointer!", NULL, NULL, NULL, NULL, NULL);
        rval |= IK_ERR_0005;
    } else {
        rval |= ctx_check_uint32_parameter_valid_range(prefix, "low", fstshpns_max_cor_rst->low, 0UL, 253UL);
        rval |= ctx_check_uint32_parameter_valid_range(prefix, "low_delta", fstshpns_max_cor_rst->low_delta, 0UL, 7UL);
        rval |= ctx_check_uint32_parameter_valid_range(prefix, "low_strength", fstshpns_max_cor_rst->low_strength, 0UL, 255UL);
        rval |= ctx_check_uint32_parameter_valid_range(prefix, "mid_strength", fstshpns_max_cor_rst->mid_strength, 0UL, 255UL);
        rval |= ctx_check_uint32_parameter_valid_range(prefix, "high", fstshpns_max_cor_rst->high, 1, 254);
        rval |= ctx_check_uint32_parameter_valid_range(prefix, "high_delta", fstshpns_max_cor_rst->high_delta, 0UL, 7UL);
        rval |= ctx_check_uint32_parameter_valid_range(prefix, "high_strength", fstshpns_max_cor_rst->high_strength, 0UL, 255UL);

        {
            // advanced level control check
            ik_level_t level;

            level.low = fstshpns_max_cor_rst->low;
            level.low_delta = fstshpns_max_cor_rst->low_delta;
            level.high = fstshpns_max_cor_rst->high;
            level.high_delta = fstshpns_max_cor_rst->high_delta;
            rval |= check_advanced_level_control("first_sharpen_max_coring_result", &level);
        }
    }
    return rval;
}

static uint32 ctx_check_fstshpns_scale_coring_valid_range(const ik_first_sharpen_scale_coring_t *fstshpns_scl_cor, const char *prefix)
{
    uint32 rval = IK_OK;
    if (fstshpns_scl_cor == NULL) {
        amba_ik_system_print_str_5("[IK] detect first_sharpen_scale_coring get NULL pointer!", NULL, NULL, NULL, NULL, NULL);
        rval |= IK_ERR_0005;
    } else {
        rval |= ctx_check_uint32_parameter_valid_range(prefix, "low", fstshpns_scl_cor->low, 0UL, 253UL);
        rval |= ctx_check_uint32_parameter_valid_range(prefix, "low_delta", fstshpns_scl_cor->low_delta, 0UL, 7UL);
        rval |= ctx_check_uint32_parameter_valid_range(prefix, "low_strength", fstshpns_scl_cor->low_strength, 0UL, 255UL);
        rval |= ctx_check_uint32_parameter_valid_range(prefix, "mid_strength", fstshpns_scl_cor->mid_strength, 0UL, 255UL);
        rval |= ctx_check_uint32_parameter_valid_range(prefix, "high", fstshpns_scl_cor->high, 1, 254);
        rval |= ctx_check_uint32_parameter_valid_range(prefix, "high_delta", fstshpns_scl_cor->high_delta, 0UL, 7UL);
        rval |= ctx_check_uint32_parameter_valid_range(prefix, "high_strength", fstshpns_scl_cor->high_strength, 0UL, 255UL);

        {
            // advanced level control check
            ik_level_t level;

            level.low = fstshpns_scl_cor->low;
            level.low_delta = fstshpns_scl_cor->low_delta;
            level.high = fstshpns_scl_cor->high;
            level.high_delta = fstshpns_scl_cor->high_delta;
            rval |= check_advanced_level_control("first_sharpen_scale_coring", &level);
        }
    }
    return rval;
}

static uint32 ctx_check_video_mctf_level_control_valid_range(const ik_level_method_t *p_mctf_level, uint8 func_mode, const char *prefix)
{
    uint32 rval = IK_OK;
    if (p_mctf_level == NULL) {
        rval |= IK_ERR_0005;
    } else {
        if((func_mode & (uint8)amba_ik_func_mode_md) != 0U) {
            //method 0,1,2,7,8,9,10,12,14
            if ((p_mctf_level->method != 0U) && (p_mctf_level->method != 1U) && (p_mctf_level->method != 2U) && \
                (p_mctf_level->method != 7U) && (p_mctf_level->method != 8U) && (p_mctf_level->method != 9U) && \
                (p_mctf_level->method != 10U) && (p_mctf_level->method != 12U) && (p_mctf_level->method != 14U)) {
                amba_ik_system_print_uint32_5("mctf_level_control.method %d not valid",p_mctf_level->method, DC_U, DC_U, DC_U, DC_U);
                rval |= IK_ERR_0008;
            }
        } else {
            //method 0,1,2,7,8,12,14
            if ((p_mctf_level->method != 0U) && (p_mctf_level->method != 1U) && (p_mctf_level->method != 2U) && \
                (p_mctf_level->method != 7U) && (p_mctf_level->method != 8U) && (p_mctf_level->method != 12U) && \
                (p_mctf_level->method != 14U)) {
                amba_ik_system_print_uint32_5("mctf_level_control.method %d not valid",p_mctf_level->method, DC_U, DC_U, DC_U, DC_U);
                rval |= IK_ERR_0008;
            }
        }

        rval |= ctx_check_uint32_parameter_valid_range(prefix, "high", p_mctf_level->high, 1UL, 254UL);
        rval |= ctx_check_uint32_parameter_valid_range(prefix, "low", p_mctf_level->low, 0UL, 253UL);
        rval |= ctx_check_uint32_parameter_valid_range(prefix, "high_delta", p_mctf_level->high_delta, 0UL, 7UL);
        rval |= ctx_check_uint32_parameter_valid_range(prefix, "low_delta", p_mctf_level->low_delta, 0UL, 7UL);
        rval |= ctx_check_uint32_parameter_valid_range(prefix, "low_strength", p_mctf_level->low_strength, 0UL, 255UL);
        rval |= ctx_check_uint32_parameter_valid_range(prefix, "mid_strength", p_mctf_level->mid_strength, 0UL, 255UL);
        rval |= ctx_check_uint32_parameter_valid_range(prefix, "high_strength", p_mctf_level->high_strength, 0UL, 255UL);

        {
            // advanced level control check
            ik_level_t level;

            level.low = p_mctf_level->low;
            level.low_delta = p_mctf_level->low_delta;
            level.high = p_mctf_level->high;
            level.high_delta = p_mctf_level->high_delta;
            rval |= check_advanced_level_control("p_mctf_level", &level);
        }
    }
    return rval;
}

static inline uint32 method_check_and_print(const char* error_msg, const uint32 method, const uint32 valid_values[], const uint32 max_index)
{
    uint32 rval = IK_OK, i;

    for (i = 0U ; i < max_index ; i++) {
        if(method == valid_values[i]) {
            break;
        }
        if(i == (max_index-1U)) {
            amba_ik_system_print_str_5("%s", error_msg, DC_S, DC_S, DC_S, DC_S);
            amba_ik_system_print_uint32_5("but get invalid method : %d", method, DC_U, DC_U, DC_U, DC_U);
            rval |= IK_ERR_0008;
        }
    }
    return rval;
}

static uint32 ctx_check_video_mctf_advanced_iso_max_change_method_valid_value(const ik_video_mctf_t *p_mctf_level, const uint8 function_mode)
{
    uint32 rval = IK_OK;
    uint32 tmp_uint32;
    uint32 valid_values[9] = {0,1,2,7,8,12,14,9,10};
    valid_values[0]=0U; //Misra
    if (((uint32)function_mode & (uint32)amba_ik_func_mode_md) != 0UL) {
        tmp_uint32 = p_mctf_level->y_advanced_iso_max_change_method;
        rval |= method_check_and_print("[IK][ERROR] video_mctf.y_advanced_iso_max_change_method should be 0,1,2,7,8,9,10,12,14", tmp_uint32, valid_values, 9U);

        tmp_uint32 = p_mctf_level->cb_advanced_iso_max_change_method;
        rval |= method_check_and_print("[IK][ERROR] video_mctf.cb_advanced_iso_max_change_method should be 0,1,2,7,8,9,10,12,14", tmp_uint32, valid_values, 9U);

        tmp_uint32 = p_mctf_level->cr_advanced_iso_max_change_method;
        rval |= method_check_and_print("[IK][ERROR] video_mctf.cr_advanced_iso_max_change_method should be 0,1,2,7,8,9,10,12,14", tmp_uint32, valid_values, 9U);

    } else {
        tmp_uint32 = p_mctf_level->y_advanced_iso_max_change_method;
        rval |= method_check_and_print("[IK][ERROR] video_mctf.y_advanced_iso_max_change_method should be 0,1,2,7,8,12,14", tmp_uint32, valid_values, 7U);

        tmp_uint32 = p_mctf_level->cb_advanced_iso_max_change_method;
        rval |= method_check_and_print("[IK][ERROR] video_mctf.cb_advanced_iso_max_change_method should be 0,1,2,7,8,12,14", tmp_uint32, valid_values, 7U);

        tmp_uint32 = p_mctf_level->cr_advanced_iso_max_change_method;
        rval |= method_check_and_print("[IK][ERROR] video_mctf.cr_advanced_iso_max_change_method should be 0,1,2,7,8,12,14", tmp_uint32, valid_values, 7U);

    }
    return rval;
}

static inline void error_check_and_print(uint32 rval, const char* error_msg)
{
    if(rval != IK_OK) {
        amba_ik_system_print_str_5("%s \n", error_msg, NULL, NULL, NULL, NULL);
    }
}


static uint32 ctx_check_video_mctf_valid_range(const ik_video_mctf_t *p_mctf_level, uint8 function_mode, const char *prefix)
{
    uint32 rval = IK_OK;
    if (p_mctf_level == NULL) {
        rval |= IK_ERR_0005;
    } else {
        if(p_mctf_level->enable == 1UL) {
            rval |= ctx_check_uint32_parameter_valid_range(prefix, "enable", p_mctf_level->enable, 0U, 1U);
            rval |= ctx_check_uint32_parameter_valid_range(prefix, "y_combine_strength", p_mctf_level->y_combine_strength, 0U, 256U);
            rval |= ctx_check_uint32_parameter_valid_range(prefix, "y_strength_3d", p_mctf_level->y_strength_3d, 0U, 256U);
            rval |= ctx_check_uint32_parameter_valid_range(prefix, "cb_strength_3d", p_mctf_level->cb_strength_3d, 0U, 256U);
            rval |= ctx_check_uint32_parameter_valid_range(prefix, "cr_strength_3d", p_mctf_level->cr_strength_3d, 0U, 256U);

            rval |= ctx_check_uint32_parameter_valid_range(prefix, "y_curve.temporal_t0", p_mctf_level->y_curve.temporal_t0, 0U, 63U);
            rval |= ctx_check_uint32_parameter_valid_range(prefix, "y_curve.temporal_t1", p_mctf_level->y_curve.temporal_t1, 0U, 63U);
            rval |= ctx_check_uint32_parameter_valid_range(prefix, "y_curve.temporal_t2", p_mctf_level->y_curve.temporal_t2, 0U, 63U);
            rval |= ctx_check_uint32_parameter_valid_range(prefix, "y_curve.temporal_t3", p_mctf_level->y_curve.temporal_t3, 0U, 63U);
            rval |= ctx_check_uint32_parameter_valid_range(prefix, "cb_curve.temporal_t0", p_mctf_level->cb_curve.temporal_t0, 0U, 63U);
            rval |= ctx_check_uint32_parameter_valid_range(prefix, "cb_curve.temporal_t1", p_mctf_level->cb_curve.temporal_t1, 0U, 63U);
            rval |= ctx_check_uint32_parameter_valid_range(prefix, "cb_curve.temporal_t2", p_mctf_level->cb_curve.temporal_t2, 0U, 63U);
            rval |= ctx_check_uint32_parameter_valid_range(prefix, "cb_curve.temporal_t3", p_mctf_level->cb_curve.temporal_t3, 0U, 63U);
            rval |= ctx_check_uint32_parameter_valid_range(prefix, "cr_curve.temporal_t0", p_mctf_level->cr_curve.temporal_t0, 0U, 63U);
            rval |= ctx_check_uint32_parameter_valid_range(prefix, "cr_curve.temporal_t1", p_mctf_level->cr_curve.temporal_t1, 0U, 63U);
            rval |= ctx_check_uint32_parameter_valid_range(prefix, "cr_curve.temporal_t2", p_mctf_level->cr_curve.temporal_t2, 0U, 63U);
            rval |= ctx_check_uint32_parameter_valid_range(prefix, "cr_curve.temporal_t3", p_mctf_level->cr_curve.temporal_t3, 0U, 63U);

            rval |= ctx_check_uint32_parameter_valid_range(prefix, "y_temporal_max_change_not_t0_t1_level_based", \
                    p_mctf_level->y_temporal_max_change_not_t0_t1_level_based, 0U, 1U);
            rval |= ctx_check_uint32_parameter_valid_range(prefix, "y_temporal_max_change_not_t0_t1_level_based", \
                    p_mctf_level->cb_temporal_max_change_not_t0_t1_level_based, 0U, 1U);
            rval |= ctx_check_uint32_parameter_valid_range(prefix, "y_temporal_max_change_not_t0_t1_level_based", \
                    p_mctf_level->cr_temporal_max_change_not_t0_t1_level_based, 0U, 1U);


            rval |= ctx_check_video_mctf_level_control_valid_range(&p_mctf_level->y_temporal_either_max_change_or_t0_t1_add,function_mode, prefix);
            error_check_and_print(rval, "[IK][ERROR] check ik_video_mctf_t->y_temporal_either_max_change_or_t0_t1_add fail \n");
            rval |= ctx_check_video_mctf_level_control_valid_range(&p_mctf_level->cb_temporal_either_max_change_or_t0_t1_add,function_mode, prefix);
            error_check_and_print(rval, "[IK][ERROR] check ik_video_mctf_t->cb_temporal_either_max_change_or_t0_t1_add fail \n");
            rval |= ctx_check_video_mctf_level_control_valid_range(&p_mctf_level->cr_temporal_either_max_change_or_t0_t1_add,function_mode, prefix);
            error_check_and_print(rval, "[IK][ERROR] check ik_video_mctf_t->cr_temporal_either_max_change_or_t0_t1_add fail \n");

            rval |= ctx_check_uint32_parameter_valid_range(prefix, "y_narrow_nonsmooth_detect_shift", p_mctf_level->y_narrow_nonsmooth_detect_shift, 0U, 5U);

            rval |= ctx_check_video_mctf_level_control_valid_range(&p_mctf_level->y_spat_blend,function_mode, prefix);
            error_check_and_print(rval, "[IK][ERROR] check ik_video_mctf_t->y_spat_blend fail \n");
            rval |= ctx_check_video_mctf_level_control_valid_range(&p_mctf_level->cb_spat_blend,function_mode, prefix);
            error_check_and_print(rval, "[IK][ERROR] check ik_video_mctf_t->cb_spat_blend fail \n");
            rval |= ctx_check_video_mctf_level_control_valid_range(&p_mctf_level->cr_spat_blend,function_mode, prefix);
            error_check_and_print(rval, "[IK][ERROR] check ik_video_mctf_t->cr_spat_blend fail \n");

            if (p_mctf_level->compression_enable == 1UL) {
                rval |= ctx_check_uint32_parameter_valid_range(prefix, "compression_bit_rate_luma", p_mctf_level->compression_bit_rate_luma, 32UL, 32UL);
                rval |= ctx_check_uint32_parameter_valid_range(prefix, "compression_bit_rate_chroma", p_mctf_level->compression_bit_rate_chroma, 20UL, 20UL);
            }
            //To reduce complexity
            rval |= ctx_check_video_mctf_advanced_iso_max_change_method_valid_value(p_mctf_level, function_mode);

            rval |= ctx_check_uint32_parameter_valid_range(prefix, "y_advanced_iso_enable", p_mctf_level->y_advanced_iso_enable, 0U, 1U);
            rval |= ctx_check_uint32_parameter_valid_range(prefix, "cb_advanced_iso_enable", p_mctf_level->cb_advanced_iso_enable, 0U, 1U);
            rval |= ctx_check_uint32_parameter_valid_range(prefix, "cr_advanced_iso_enable", p_mctf_level->cr_advanced_iso_enable, 0U, 1U);

            rval |= ctx_check_uint32_parameter_valid_range(prefix, "y_advanced_iso_size", p_mctf_level->y_advanced_iso_size, 0U, 1U);
            rval |= ctx_check_uint32_parameter_valid_range(prefix, "cb_advanced_iso_size", p_mctf_level->cb_advanced_iso_size, 0U, 1U);
            rval |= ctx_check_uint32_parameter_valid_range(prefix, "cr_advanced_iso_size", p_mctf_level->cr_advanced_iso_size, 0U, 1U);

            rval |= ctx_check_video_mctf_level_control_valid_range(&p_mctf_level->y_3d_maxchange,function_mode, prefix);
            error_check_and_print(rval, "[IK][ERROR] check ik_video_mctf_t->y_3d_maxchange fail \n");
            rval |= ctx_check_video_mctf_level_control_valid_range(&p_mctf_level->cb_3d_maxchange,function_mode, prefix);
            error_check_and_print(rval, "[IK][ERROR] check ik_video_mctf_t->cb_3d_maxchange fail \n");
            rval |= ctx_check_video_mctf_level_control_valid_range(&p_mctf_level->cr_3d_maxchange,function_mode, prefix);
            error_check_and_print(rval, "[IK][ERROR] check ik_video_mctf_t->cr_3d_maxchange fail \n");

            rval |= ctx_check_video_mctf_level_control_valid_range(&p_mctf_level->y_overall_max_change,function_mode, prefix);
            error_check_and_print(rval, "[IK][ERROR] check ik_video_mctf_t->y_overall_max_change fail \n");
            rval |= ctx_check_video_mctf_level_control_valid_range(&p_mctf_level->cb_overall_max_change,function_mode, prefix);
            error_check_and_print(rval, "[IK][ERROR] check ik_video_mctf_t->cb_overall_max_change fail \n");
            rval |= ctx_check_video_mctf_level_control_valid_range(&p_mctf_level->cr_overall_max_change,function_mode, prefix);
            error_check_and_print(rval, "[IK][ERROR] check ik_video_mctf_t->cr_overall_max_change fail \n");

            rval |= ctx_check_video_mctf_level_control_valid_range(&p_mctf_level->y_spat_filt_max_smth_change,function_mode, prefix);
            error_check_and_print(rval, "[IK][ERROR] check ik_video_mctf_t->y_spat_filt_max_smth_change fail \n");
            rval |= ctx_check_video_mctf_level_control_valid_range(&p_mctf_level->cb_spat_filt_max_smth_change,function_mode, prefix);
            error_check_and_print(rval, "[IK][ERROR] check ik_video_mctf_t->cb_spat_filt_max_smth_change fail \n");
            rval |= ctx_check_video_mctf_level_control_valid_range(&p_mctf_level->cr_spat_filt_max_smth_change,function_mode, prefix);
            error_check_and_print(rval, "[IK][ERROR] check ik_video_mctf_t->cr_spat_filt_max_smth_change fail \n");

            rval |= ctx_check_uint32_parameter_valid_range(prefix, "y_spat_smth_wide_edge_detect", p_mctf_level->y_spat_smth_wide_edge_detect, 0U, 8U);
            rval |= ctx_check_uint32_parameter_valid_range(prefix, "cb_spat_smth_wide_edge_detect", p_mctf_level->cb_spat_smth_wide_edge_detect, 0U, 8U);
            rval |= ctx_check_uint32_parameter_valid_range(prefix, "cr_spat_smth_wide_edge_detect", p_mctf_level->cr_spat_smth_wide_edge_detect, 0U, 8U);
            rval |= ctx_check_uint32_parameter_valid_range(prefix, "cr_spat_smth_wide_edge_detect", p_mctf_level->y_spat_smth_edge_thresh, 0U, 2047U);
            rval |= ctx_check_uint32_parameter_valid_range(prefix, "cr_spat_smth_wide_edge_detect", p_mctf_level->cr_spat_smth_edge_thresh, 0U, 2047U);
            rval |= ctx_check_uint32_parameter_valid_range(prefix, "cr_spat_smth_wide_edge_detect", p_mctf_level->cr_spat_smth_edge_thresh, 0U, 2047U);

            rval |= ctx_check_video_mctf_level_control_valid_range(&p_mctf_level->y_spat_smth_dir,function_mode, prefix);
            error_check_and_print(rval, "[IK][ERROR] check ik_video_mctf_t->y_spat_smth_dir fail \n");
            rval |= ctx_check_video_mctf_level_control_valid_range(&p_mctf_level->cb_spat_smth_dir,function_mode, prefix);
            error_check_and_print(rval, "[IK][ERROR] check ik_video_mctf_t->cb_spat_smth_dir fail \n");
            rval |= ctx_check_video_mctf_level_control_valid_range(&p_mctf_level->cr_spat_smth_dir,function_mode, prefix);
            error_check_and_print(rval, "[IK][ERROR] check ik_video_mctf_t->cr_spat_smth_dir fail \n");

            rval |= ctx_check_video_mctf_level_control_valid_range(&p_mctf_level->y_spat_smth_iso,function_mode, prefix);
            error_check_and_print(rval, "[IK][ERROR] check ik_video_mctf_t->y_spat_smth_iso fail \n");
            rval |= ctx_check_video_mctf_level_control_valid_range(&p_mctf_level->cb_spat_smth_iso,function_mode, prefix);
            error_check_and_print(rval, "[IK][ERROR] check ik_video_mctf_t->cb_spat_smth_iso fail \n");
            rval |= ctx_check_video_mctf_level_control_valid_range(&p_mctf_level->cr_spat_smth_iso,function_mode, prefix);
            error_check_and_print(rval, "[IK][ERROR] check ik_video_mctf_t->cr_spat_smth_iso fail \n");

            rval |= ctx_check_video_mctf_level_control_valid_range(&p_mctf_level->y_level_based_ta,function_mode, prefix);
            error_check_and_print(rval, "[IK][ERROR] check ik_video_mctf_t->y_level_based_ta fail \n");
            rval |= ctx_check_video_mctf_level_control_valid_range(&p_mctf_level->cb_level_based_ta,function_mode, prefix);
            error_check_and_print(rval, "[IK][ERROR] check ik_video_mctf_t->cb_level_based_ta fail \n");
            rval |= ctx_check_video_mctf_level_control_valid_range(&p_mctf_level->cr_level_based_ta,function_mode, prefix);
            error_check_and_print(rval, "[IK][ERROR] check ik_video_mctf_t->cr_level_based_ta fail \n");

            rval |= ctx_check_video_mctf_level_control_valid_range(&p_mctf_level->y_temporal_min_target,function_mode, prefix);
            error_check_and_print(rval, "[IK][ERROR] check ik_video_mctf_t->y_temporal_min_target fail \n");
            rval |= ctx_check_video_mctf_level_control_valid_range(&p_mctf_level->cb_temporal_min_target,function_mode, prefix);
            error_check_and_print(rval, "[IK][ERROR] check ik_video_mctf_t->cb_temporal_min_target fail \n");
            rval |= ctx_check_video_mctf_level_control_valid_range(&p_mctf_level->cr_temporal_min_target,function_mode, prefix);
            error_check_and_print(rval, "[IK][ERROR] check ik_video_mctf_t->cr_temporal_min_target fail \n");

            rval |= ctx_check_video_mctf_level_control_valid_range(&p_mctf_level->y_spatial_max_temporal,function_mode, prefix);
            error_check_and_print(rval, "[IK][ERROR] check ik_video_mctf_t->y_spatial_max_temporal fail \n");
            rval |= ctx_check_video_mctf_level_control_valid_range(&p_mctf_level->cb_spatial_max_temporal,function_mode, prefix);
            error_check_and_print(rval, "[IK][ERROR] check ik_video_mctf_t->cb_spatial_max_temporal fail \n");
            rval |= ctx_check_video_mctf_level_control_valid_range(&p_mctf_level->cr_spatial_max_temporal,function_mode, prefix);
            error_check_and_print(rval, "[IK][ERROR] check ik_video_mctf_t->cr_spatial_max_temporal fail \n");

            rval |= ctx_check_uint32_parameter_valid_range(prefix, "compression_enable", p_mctf_level->compression_enable, 0U, 1U);
            rval |= ctx_check_uint32_parameter_valid_range(prefix, "y_temporal_ghost_prevent", p_mctf_level->y_temporal_ghost_prevent, 0U, 255U);
            rval |= ctx_check_uint32_parameter_valid_range(prefix, "cb_temporal_ghost_prevent", p_mctf_level->cb_temporal_ghost_prevent, 0U, 255U);
            rval |= ctx_check_uint32_parameter_valid_range(prefix, "cr_temporal_ghost_prevent", p_mctf_level->cr_temporal_ghost_prevent, 0U, 255U);
            rval |= ctx_check_uint32_parameter_valid_range(prefix, "y_temporal_artifact_guard", p_mctf_level->y_temporal_artifact_guard, 0U, 255U);
            rval |= ctx_check_uint32_parameter_valid_range(prefix, "cb_temporal_artifact_guard", p_mctf_level->cb_temporal_artifact_guard, 0U, 255U);
            rval |= ctx_check_uint32_parameter_valid_range(prefix, "cr_temporal_artifact_guard", p_mctf_level->cr_temporal_artifact_guard, 0U, 255U);
            if(p_mctf_level->compression_enable == 1UL) {
                rval |= ctx_check_uint32_parameter_valid_range(prefix, "compression_bit_rate_luma", p_mctf_level->compression_bit_rate_luma, 1U, 63U);
                rval |= ctx_check_uint32_parameter_valid_range(prefix, "compression_bit_rate_chroma", p_mctf_level->compression_bit_rate_chroma, 1U, 63U);
            }
            rval |= ctx_check_uint32_parameter_valid_range(prefix, "compression_dither_disable", p_mctf_level->compression_dither_disable, 0U, 1U);
        }
    }
    return rval;
}

static uint32 ctx_check_video_mctf_ta_valid_range(const ik_video_mctf_ta_t *video_mctf_ta, const char *prefix)
{
    uint32 rval = IK_OK;
    if (video_mctf_ta == NULL) {
        rval |= IK_ERR_0005;
    } else {
        rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "edge_adjust_b", video_mctf_ta->edge_adjust_b, 0, 2047);
        rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "edge_adjust_l", video_mctf_ta->edge_adjust_l, 0, 2047);
        rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "edge_adjust_r", video_mctf_ta->edge_adjust_r, 0, 2047);
        rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "edge_adjust_t", video_mctf_ta->edge_adjust_t, 0, 2047);
        rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "slow_mo_sensitivity", video_mctf_ta->slow_mo_sensitivity, 0, 11);
        rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "score_noise_robust", video_mctf_ta->score_noise_robust, 0, 1);
        rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "lev_adjust_high_delta", video_mctf_ta->lev_adjust_high_delta, 0UL, 7UL);
        rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "slow_mo_sensitivity", video_mctf_ta->lev_adjust_low_delta, 0UL, 7UL);
        rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "y_mctf_ta.max", video_mctf_ta->y_mctf_ta.max, 0, 15);
        rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "y_mctf_ta.min", video_mctf_ta->y_mctf_ta.min, 0, 15);
        rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "y_mctf_ta.motion_response", video_mctf_ta->y_mctf_ta.motion_response, 0, 16383);
        rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "y_mctf_ta.noise_base", video_mctf_ta->y_mctf_ta.noise_base, 0, 4095);
        rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "y_mctf_ta.still_thres", video_mctf_ta->y_mctf_ta.still_thresh, 0, 4095);
        rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "cb_mctf_ta.max", video_mctf_ta->cb_mctf_ta.max, 0, 15);
        rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "cb_mctf_ta.min", video_mctf_ta->cb_mctf_ta.min, 0, 15);
        rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "cb_mctf_ta.motion_response", video_mctf_ta->cb_mctf_ta.motion_response, 0, 16383);
        rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "cb_mctf_ta.noise_base", video_mctf_ta->cb_mctf_ta.noise_base, 0, 4095);
        rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "cb_mctf_ta.still_thres", video_mctf_ta->cb_mctf_ta.still_thresh, 0, 4095);
        rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "cr_mctf_ta.max", video_mctf_ta->cr_mctf_ta.max, 0, 15);
        rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "cr_mctf_ta.min", video_mctf_ta->cr_mctf_ta.min, 0, 15);
        rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "cr_mctf_ta.motion_response", video_mctf_ta->cr_mctf_ta.motion_response, 0, 16383);
        rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "cr_mctf_ta.noise_base", video_mctf_ta->cr_mctf_ta.noise_base, 0, 4095);
        rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "cr_mctf_ta.still_thres", video_mctf_ta->cr_mctf_ta.still_thresh, 0, 4095);
    }
    return rval;
}

static uint32 ctx_check_pos_dep33_valid_range(const ik_pos_dep33_t *video_mctf_and_fnlshp, const char *prefix)
{
    uint32 rval = IK_OK;
    uint32 i, j;
    if (video_mctf_and_fnlshp == NULL) {
        rval |= IK_ERR_0005;
    } else {
        for (i = 0U; i < 33U; i++) {
            for (j = 0U; j < 33U; j++) {
                rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "pos_dep", video_mctf_and_fnlshp->pos_dep[i][j], 0U, 31U);
            }
        }
    }
    return rval;
}

static uint32 ctx_check_fnlshpns_both_valid_range(const ik_final_sharpen_both_t *final_sharpen_both, uint8 function_mode, const char *prefix)
{
    uint32 rval = IK_OK;
    uint8 tmp_uint8;
    if (final_sharpen_both == NULL) {
        amba_ik_system_print_str_5("[IK] detect final_sharpen_both get NULL pointer!", NULL, NULL, NULL, NULL, NULL);
        rval |= IK_ERR_0005;
    } else {
        rval |= ctx_check_uint32_parameter_valid_range(prefix, "enable", final_sharpen_both->enable, 0U, 1U);
        if(final_sharpen_both->enable == 1UL) {
            rval |= ctx_check_uint32_parameter_valid_range(prefix, "mode", final_sharpen_both->mode, 0U, 2U);
            rval |= ctx_check_uint32_parameter_valid_range(prefix, "edge_thresh", final_sharpen_both->edge_thresh, 0U, 2047U);
            rval |= ctx_check_uint32_parameter_valid_range(prefix, "wide_edge_detect", final_sharpen_both->wide_edge_detect, 0U, 8U);
            rval |= ctx_check_uint32_parameter_valid_range(prefix, "max_change.up", final_sharpen_both->max_change.up, 0U, 255U);
            rval |= ctx_check_uint32_parameter_valid_range(prefix, "max_change.up5x5", final_sharpen_both->max_change.up5x5, 0U, 255U);
            rval |= ctx_check_uint32_parameter_valid_range(prefix, "max_change.down", final_sharpen_both->max_change.down, 0U, 255U);
            rval |= ctx_check_uint32_parameter_valid_range(prefix, "max_change.down5x5", final_sharpen_both->max_change.down5x5, 0U, 255U);
            //cv2
            rval |= ctx_check_uint32_parameter_valid_range(prefix, "narrow_nonsmooth_detect_shift", final_sharpen_both->narrow_nonsmooth_detect_shift, 0U, 5U);

            if (((uint32)function_mode & (uint32)amba_ik_func_mode_md) != 0UL) {
                rval |= ctx_check_uint32_parameter_valid_range(prefix, "method_6_lev.low", final_sharpen_both->method_6_lev.low, 0U, 253U);
                rval |= ctx_check_uint32_parameter_valid_range(prefix, "method_6_lev.low_delta", final_sharpen_both->method_6_lev.low_delta, 0U, 7U);
                rval |= ctx_check_uint32_parameter_valid_range(prefix, "method_6_lev.high", final_sharpen_both->method_6_lev.high, 1U, 254U);
                rval |= ctx_check_uint32_parameter_valid_range(prefix, "method_6_lev.high_delta", final_sharpen_both->method_6_lev.high_delta, 0U, 7U);
                rval |= ctx_check_uint32_parameter_valid_range(prefix, "method_6_use_local", final_sharpen_both->method_6_use_local, 0U, 1U);
                tmp_uint8 = (uint8)final_sharpen_both->method_6_lev.method;
                if ((tmp_uint8 != 0U) && (tmp_uint8 != 1U) && (tmp_uint8 != 2U) && \
                    (tmp_uint8 != 7U) && (tmp_uint8 != 8U) && (tmp_uint8 != 9U) && \
                    (tmp_uint8 != 10U) && (tmp_uint8 != 12U)) {
                    amba_ik_system_print_uint32_5("fnlshpns_both.method_6_lev.method %d not valid",tmp_uint8, DC_U, DC_U, DC_U, DC_U);
                    rval |= IK_ERR_0008;
                }
            } else {
                //Not support, skip the checking
            }
        }
    }
    return rval;
}

static uint32 check_fnlshpns_noise_valid_range_sub(const ik_final_sharpen_noise_t *final_sharpen_noise, const char *prefix)
{
    uint32 rval = IK_OK;
    uint32 valid_values[7] = {0,1,2,6,7,12,14};
    valid_values[0]=0U; //Misra
    rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "t1", final_sharpen_noise->t1, 0U, 255U);
    rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "alpha_min", final_sharpen_noise->alpha_min, 0U, 16U);
    rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "alpha_max", final_sharpen_noise->alpha_max, 0U, 16U);

    rval |= method_check_and_print("[IK][ERROR] final_sharpen_noise->level_str_adjust.method should be 0,1,2,6,7,12,14", final_sharpen_noise->level_str_adjust.method, valid_values, 7U);

    //cv2
    rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "advanced_iso.enable", final_sharpen_noise->advanced_iso.enable, 0U, 1U);
    rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "advanced_iso.enable", final_sharpen_noise->advanced_iso.size, 0U, 2U);

    rval |= method_check_and_print("[IK][ERROR] final_sharpen_noise->advanced_iso.max_change_method should be 0,1,2,6,7,12,14", final_sharpen_noise->advanced_iso.max_change_method, valid_values, 7U);
    rval |= method_check_and_print("[IK][ERROR] final_sharpen_noise->advanced_iso.noise_level_method should be 0,1,2,6,7,12,14", final_sharpen_noise->advanced_iso.noise_level_method, valid_values, 7U);
    rval |= method_check_and_print("[IK][ERROR] final_sharpen_noise->advanced_iso.str_a_method should be 0,1,2,6,7,12,14", final_sharpen_noise->advanced_iso.str_a_method, valid_values, 7U);
    rval |= method_check_and_print("[IK][ERROR] final_sharpen_noise->advanced_iso.str_b_method should be 0,1,2,6,7,12,14", final_sharpen_noise->advanced_iso.str_b_method, valid_values, 7U);

    return rval;
}

static uint32 ctx_check_fnlshpns_noise_valid_range(const ik_final_sharpen_noise_t *final_sharpen_noise, const char *prefix)
{
    uint32 rval = IK_OK;
    uint32  i = 0, j = 0;
    uint32 tmp;
    if (final_sharpen_noise == NULL) {
        amba_ik_system_print_str_5("[IK] detect final_sharpen_noise get NULL pointer!", NULL, NULL, NULL, NULL, NULL);
        rval |= IK_ERR_0005;
    } else {
        rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "max_change_up", final_sharpen_noise->max_change_up, 0U, 255U);
        rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "max_change_down", final_sharpen_noise->max_change_down, 0U, 255U);
        rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "spatial_fir.specify", final_sharpen_noise->spatial_fir.specify, 0U, 4U);
        if (final_sharpen_noise->spatial_fir.specify == 0U) {
            rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "spatial_fir.strength_iso", final_sharpen_noise->spatial_fir.strength_iso, 0U, 256U);
        } else if (final_sharpen_noise->spatial_fir.specify == 1U) {
            for (i = 0U; i < 25U; i++) {
                rval |= (uint32)ctx_check_int32_parameter_valid_range(prefix, "spatial_fir.coefs", final_sharpen_noise->spatial_fir.coefs[0][i], -1024, 1023);
            }
        } else if (final_sharpen_noise->spatial_fir.specify == 2U) {
            rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "spatial_fir.strength_iso", final_sharpen_noise->spatial_fir.strength_iso, 0U, 256U);
            rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "spatial_fir.strength_dir", final_sharpen_noise->spatial_fir.strength_dir, 0U, 256U);
        } else if (final_sharpen_noise->spatial_fir.specify == 3U) {
            for (i = 0U; i < 9U; i++) {
                rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "spatial_fir.per_dir_fir_dir_amounts", final_sharpen_noise->spatial_fir.per_dir_fir_dir_amounts[i], 0U, 256U);
                rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "spatial_fir.per_dir_fir_dir_strengths", final_sharpen_noise->spatial_fir.per_dir_fir_dir_strengths[i], 0U, 256U);
                rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "spatial_fir.per_dir_fir_iso_strengths", final_sharpen_noise->spatial_fir.per_dir_fir_iso_strengths[i], 0U, 256U);
            }
        } else if (final_sharpen_noise->spatial_fir.specify == 4U) {
            for (j = 0U; j < 9U; j++) {
                for (i = 0U; i < 25U; i++) {
                    rval |= (uint32)ctx_check_int32_parameter_valid_range(prefix, "spatial_fir.coefs", final_sharpen_noise->spatial_fir.coefs[j][i], -1024, 1023);
                }
            }
        } else {
            rval |= IK_ERR_0008;
        }
        rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "level_str_adjust.low", final_sharpen_noise->level_str_adjust.low, 0U, 253U);
        rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "level_str_adjust.low_delta", final_sharpen_noise->level_str_adjust.low_delta, 0U, 7U);
        rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "level_str_adjust.high", final_sharpen_noise->level_str_adjust.high, 1, 254U);
        rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "level_str_adjust.high_delta", final_sharpen_noise->level_str_adjust.high_delta, 0U, 7U);
        rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "level_str_adjust_not_t0_t1_level_based", final_sharpen_noise->level_str_adjust_not_t0_t1_level_based, 0U, 1U);
        rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "t0", final_sharpen_noise->t0, 0U, 255U);

        {
            // advanced level control check
            ik_level_t level;

            level.low = final_sharpen_noise->level_str_adjust.low;
            level.low_delta = final_sharpen_noise->level_str_adjust.low_delta;
            level.high = final_sharpen_noise->level_str_adjust.high;
            level.high_delta = final_sharpen_noise->level_str_adjust.high_delta;
            rval |= check_advanced_level_control("final_sharpen_noise->level_str_adjust", &level);
        }

        if (final_sharpen_noise->t0 > final_sharpen_noise->t1) {
            amba_ik_system_print_uint32_5("[IK] t0 = %d  should not larger than t1 = %d\n", final_sharpen_noise->t0, final_sharpen_noise->t1, DC_U, DC_U, DC_U);
            rval |= IK_ERR_0008;
        }
        tmp = final_sharpen_noise->t1 - final_sharpen_noise->t0;
        if (tmp > 15U) {
            amba_ik_system_print_uint32_5("[IK] t1 = %d should be larger then t0 = %d at least 15\n", final_sharpen_noise->t0, final_sharpen_noise->t1, DC_U, DC_U, DC_U);
            rval |= IK_ERR_0008;
        }
        rval |= check_fnlshpns_noise_valid_range_sub(final_sharpen_noise, prefix);
    }
    return rval;
}

static uint32 ctx_check_fnlshpns_fir_valid_range(const ik_final_sharpen_fir_t *final_sharpen_fir, const char *prefix)
{
    uint32 rval = IK_OK;
    uint32  i = 0, j = 0;
    if (final_sharpen_fir == NULL) {
        amba_ik_system_print_str_5("[IK] detect final_sharpen_fir get NULL pointer!", NULL, NULL, NULL, NULL, NULL);
        rval |= IK_ERR_0005;
    } else {
        rval |= ctx_check_uint32_parameter_valid_range(prefix, "specify", final_sharpen_fir->specify, 0U, 4U);
        if (final_sharpen_fir->specify == 0U) {
            rval |= ctx_check_uint32_parameter_valid_range(prefix, "strength_iso", final_sharpen_fir->strength_iso, 0U, 256U);
        } else if (final_sharpen_fir->specify == 1U) {
            for (j = 0U; j < 25U; j++) {
                rval |= ctx_check_int32_parameter_valid_range(prefix, "coefs", final_sharpen_fir->coefs[0][j], -1024, 1023);
            }
        } else if (final_sharpen_fir->specify == 2U) {
            rval |= ctx_check_uint32_parameter_valid_range(prefix, "strength_iso", final_sharpen_fir->strength_iso, 0U, 256U);
            rval |= ctx_check_uint32_parameter_valid_range(prefix, "strength_dir", final_sharpen_fir->strength_dir, 0U, 256U);
        } else if (final_sharpen_fir->specify == 3U) {
            for (i = 0U; i < 9U; i++) {
                rval |= ctx_check_uint32_parameter_valid_range(prefix, "per_dir_fir_dir_amounts", final_sharpen_fir->per_dir_fir_dir_amounts[i],    0U, 256U);
                rval |= ctx_check_uint32_parameter_valid_range(prefix, "per_dir_fir_dir_strengths", final_sharpen_fir->per_dir_fir_dir_strengths[i], 0U, 256U);
                rval |= ctx_check_uint32_parameter_valid_range(prefix, "per_dir_fir_iso_strengths", final_sharpen_fir->per_dir_fir_iso_strengths[i], 0U, 256U);
            }
        } else if (final_sharpen_fir->specify == 4U) {
            for (j = 0U; j < 9U; j++) {
                for (i = 0U; i < 25U; i++) {
                    rval |= ctx_check_int32_parameter_valid_range(prefix, "coefs", final_sharpen_fir->coefs[j][i], -1024, 1023);
                }
            }
        } else {
            // misraC
        }
        //cv2
        rval |= ctx_check_uint32_parameter_valid_range(prefix, "pos_fir_artifact_reduce_enable", final_sharpen_fir->pos_fir_artifact_reduce_enable, 0U, 1U);
        rval |= ctx_check_uint32_parameter_valid_range(prefix, "pos_fir_artifact_reduce_strength", final_sharpen_fir->pos_fir_artifact_reduce_strength, 0U, 223U);
    }
    return rval;
}

static uint32 ctx_check_fnlshpns_coring_valid_range(const ik_final_sharpen_coring_t *final_sharpen_coring, const char *prefix)
{
    uint32 rval = IK_OK;
    uint32  i = 0;
    if (final_sharpen_coring == NULL) {
        amba_ik_system_print_str_5("[IK] detect final_sharpen_coring get NULL pointer!", NULL, NULL, NULL, NULL, NULL);
        rval |= IK_ERR_0005;
    } else {
        for (i = 0U; i < 256U; i++) {
            rval |= ctx_check_uint32_parameter_valid_range(prefix, "coring", final_sharpen_coring->coring[i], 0, 31);
            rval |= ctx_check_uint32_parameter_valid_range(prefix, "fractional_bits", final_sharpen_coring->fractional_bits, 1UL, 3UL);
        }
    }
    return rval;
}

static uint32 ctx_check_fnlshpns_coring_index_scale_valid_range(const ik_final_sharpen_coring_idx_scale_t *fnlshpns_cor_idx_scl, const char *prefix)
{
    uint32 rval = IK_OK;
    if (fnlshpns_cor_idx_scl == NULL) {
        amba_ik_system_print_str_5("[IK] detect final_sharpen_coring_idx_scale get NULL pointer!", NULL, NULL, NULL, NULL, NULL);
        rval |= IK_ERR_0005;
    } else {
        rval |= ctx_check_uint32_parameter_valid_range(prefix, "low", fnlshpns_cor_idx_scl->low, 0UL, 253UL);
        rval |= ctx_check_uint32_parameter_valid_range(prefix, "low_delta", fnlshpns_cor_idx_scl->low_delta, 0UL, 7UL);
        rval |= ctx_check_uint32_parameter_valid_range(prefix, "high", fnlshpns_cor_idx_scl->high, 1, 254);
        rval |= ctx_check_uint32_parameter_valid_range(prefix, "high_delta", fnlshpns_cor_idx_scl->high_delta, 0UL, 7UL);
        if ((fnlshpns_cor_idx_scl->method != 0U) &&
            (fnlshpns_cor_idx_scl->method != 1U) &&
            (fnlshpns_cor_idx_scl->method != 2U) &&
            (fnlshpns_cor_idx_scl->method != 6U) &&
            (fnlshpns_cor_idx_scl->method != 7U) &&
            (fnlshpns_cor_idx_scl->method != 12U) &&
            (fnlshpns_cor_idx_scl->method != 14U)) {
            amba_ik_system_print_uint32_5("[IK] fnlshpns_cor_idx_scl->method %d , should be 0,1,6,7", fnlshpns_cor_idx_scl->method, DC_U, DC_U, DC_U, DC_U);
            rval |= IK_ERR_0008;
        }

        {
            // advanced level control check
            ik_level_t level;

            level.low = fnlshpns_cor_idx_scl->low;
            level.low_delta = fnlshpns_cor_idx_scl->low_delta;
            level.high = fnlshpns_cor_idx_scl->high;
            level.high_delta = fnlshpns_cor_idx_scl->high_delta;
            rval |= check_advanced_level_control("final_sharpen_coring_idx_scale", &level);
        }
    }
    return rval;
}

static uint32 ctx_check_fnlshpns_min_coring_result_valid_range(const ik_final_sharpen_min_coring_result_t *fnlshpns_min_cor_rst, const char *prefix)
{
    uint32 rval = IK_OK;
    if (fnlshpns_min_cor_rst == NULL) {
        amba_ik_system_print_str_5("[IK] detect fnlshpns_min_cor_rst get NULL pointer!", NULL, NULL, NULL, NULL, NULL);
        rval |= IK_ERR_0005;
    } else {
        rval |= ctx_check_uint32_parameter_valid_range(prefix, "low", fnlshpns_min_cor_rst->low, 0UL, 253UL);
        rval |= ctx_check_uint32_parameter_valid_range(prefix, "low_delta", fnlshpns_min_cor_rst->low_delta, 0UL, 7UL);
        rval |= ctx_check_uint32_parameter_valid_range(prefix, "low_strength", fnlshpns_min_cor_rst->low_strength, 0UL, 255UL);
        rval |= ctx_check_uint32_parameter_valid_range(prefix, "mid_strength", fnlshpns_min_cor_rst->mid_strength, 0UL, 255UL);
        rval |= ctx_check_uint32_parameter_valid_range(prefix, "high", fnlshpns_min_cor_rst->high, 1, 254);
        rval |= ctx_check_uint32_parameter_valid_range(prefix, "high_delta", fnlshpns_min_cor_rst->high_delta, 0UL, 7UL);
        rval |= ctx_check_uint32_parameter_valid_range(prefix, "high_strength", fnlshpns_min_cor_rst->high_strength, 0UL, 255UL);
        if ((fnlshpns_min_cor_rst->method != 0U) &&
            (fnlshpns_min_cor_rst->method != 1U) &&
            (fnlshpns_min_cor_rst->method != 2U) &&
            (fnlshpns_min_cor_rst->method != 6U) &&
            (fnlshpns_min_cor_rst->method != 7U) &&
            (fnlshpns_min_cor_rst->method != 12U) &&
            (fnlshpns_min_cor_rst->method != 14U)) {
            amba_ik_system_print_uint32_5("[IK] fnlshpns_min_cor_rst->method %d , should be 0,1,6,7", fnlshpns_min_cor_rst->method, DC_U, DC_U, DC_U, DC_U);
            rval |= IK_ERR_0008;
        }

        {
            // advanced level control check
            ik_level_t level;

            level.low = fnlshpns_min_cor_rst->low;
            level.low_delta = fnlshpns_min_cor_rst->low_delta;
            level.high = fnlshpns_min_cor_rst->high;
            level.high_delta = fnlshpns_min_cor_rst->high_delta;
            rval |= check_advanced_level_control("fnlshpns_min_cor_rst", &level);
        }
    }
    return rval;
}

static uint32 ctx_check_fnlshpns_max_coring_result_valid_range(const ik_final_sharpen_max_coring_result_t *fnlshpns_max_cor_rst, const char *prefix)
{
    uint32 rval = IK_OK;
    uint32 High0, Low0;
    if (fnlshpns_max_cor_rst == NULL) {
        amba_ik_system_print_str_5("[IK] detect fnlshpns_max_cor_rst get NULL pointer!", NULL, NULL, NULL, NULL, NULL);
        rval |= IK_ERR_0005;
    } else {
        rval |= ctx_check_uint32_parameter_valid_range(prefix, "low", fnlshpns_max_cor_rst->low, 0UL, 253UL);
        rval |= ctx_check_uint32_parameter_valid_range(prefix, "low_delta", fnlshpns_max_cor_rst->low_delta, 0UL, 7UL);
        rval |= ctx_check_uint32_parameter_valid_range(prefix, "low_strength", fnlshpns_max_cor_rst->low_strength, 0UL, 255UL);
        rval |= ctx_check_uint32_parameter_valid_range(prefix, "mid_strength", fnlshpns_max_cor_rst->mid_strength, 0UL, 255UL);
        rval |= ctx_check_uint32_parameter_valid_range(prefix, "high", fnlshpns_max_cor_rst->high, 1, 254);
        rval |= ctx_check_uint32_parameter_valid_range(prefix, "high_delta", fnlshpns_max_cor_rst->high_delta, 0UL, 7UL);
        rval |= ctx_check_uint32_parameter_valid_range(prefix, "high_strength", fnlshpns_max_cor_rst->high_strength, 0UL, 255UL);
        if ((fnlshpns_max_cor_rst->method != 0U) &&
            (fnlshpns_max_cor_rst->method != 1U) &&
            (fnlshpns_max_cor_rst->method != 2U) &&
            (fnlshpns_max_cor_rst->method != 6U) &&
            (fnlshpns_max_cor_rst->method != 7U) &&
            (fnlshpns_max_cor_rst->method != 12U) &&
            (fnlshpns_max_cor_rst->method != 14U)) {
            amba_ik_system_print_uint32_5("[IK] fnlshpns_max_cor_rst->method %d , should be 0,1,6,7,14", fnlshpns_max_cor_rst->method, DC_U, DC_U, DC_U, DC_U);
            rval |= IK_ERR_0008;
        }
        High0 = fnlshpns_max_cor_rst->high + (1UL<< fnlshpns_max_cor_rst->high_delta);
        if (High0 > 255UL) {
            amba_ik_system_print_uint32_5("[IK] fnlshpns_max_cor_rst->high + (1<<high_delta) > 255",DC_U, DC_U, DC_U, DC_U, DC_U);
            rval |= IK_ERR_0008;
        }
        Low0 = fnlshpns_max_cor_rst->low + (1UL<< fnlshpns_max_cor_rst->low_delta);
        if(Low0 > fnlshpns_max_cor_rst->high) {
            amba_ik_system_print_uint32_5("[IK] fnlshpns_max_cor_rst->low + (1<<low_delta) > fnlshpns_max_cor_rst->high",DC_U, DC_U, DC_U, DC_U, DC_U);
            rval |= IK_ERR_0008;
        }

        {
            // advanced level control check
            ik_level_t level;

            level.low = fnlshpns_max_cor_rst->low;
            level.low_delta = fnlshpns_max_cor_rst->low_delta;
            level.high = fnlshpns_max_cor_rst->high;
            level.high_delta = fnlshpns_max_cor_rst->high_delta;
            rval |= check_advanced_level_control("fnlshpns_max_cor_rst", &level);
        }
    }
    return rval;
}

static uint32 ctx_check_fnlshpns_scale_coring_valid_range(const ik_final_sharpen_scale_coring_t *fnlshpns_scl_cor, const char *prefix)
{
    uint32 rval = IK_OK;
    uint32 High0, Low0;
    if (fnlshpns_scl_cor == NULL) {
        amba_ik_system_print_str_5("[IK] detect fnlshpns_scl_cor get NULL pointer!", NULL, NULL, NULL, NULL, NULL);
        rval |= IK_ERR_0005;
    } else {
        rval |= ctx_check_uint32_parameter_valid_range(prefix, "low", fnlshpns_scl_cor->low, 0UL, 253UL);
        rval |= ctx_check_uint32_parameter_valid_range(prefix, "low_delta", fnlshpns_scl_cor->low_delta, 0UL, 7UL);
        rval |= ctx_check_uint32_parameter_valid_range(prefix, "high", fnlshpns_scl_cor->high, 1, 254);
        rval |= ctx_check_uint32_parameter_valid_range(prefix, "high_delta", fnlshpns_scl_cor->high_delta, 0UL, 7UL);
        if ((fnlshpns_scl_cor->method != 0U) &&
            (fnlshpns_scl_cor->method != 1U) &&
            (fnlshpns_scl_cor->method != 2U) &&
            (fnlshpns_scl_cor->method != 6U) &&
            (fnlshpns_scl_cor->method != 7U) &&
            (fnlshpns_scl_cor->method != 12U) &&
            (fnlshpns_scl_cor->method != 14U)) {
            amba_ik_system_print_uint32_5("[IK] fnlshpns_scl_cor->method %d , should be 0,1,6,7,14", fnlshpns_scl_cor->method, DC_U, DC_U, DC_U, DC_U);
            rval |= IK_ERR_0008;
        }
        High0 = fnlshpns_scl_cor->high + (1UL<< fnlshpns_scl_cor->high_delta);
        if (High0 > 255UL) {
            amba_ik_system_print_uint32_5("[IK] fnlshpns_scl_cor->high + (1<<high_delta) > 255",DC_U, DC_U, DC_U, DC_U, DC_U);
            rval |= IK_ERR_0008;
        }
        Low0 = fnlshpns_scl_cor->low + (1UL<< fnlshpns_scl_cor->low_delta);
        if(Low0 > fnlshpns_scl_cor->high) {
            amba_ik_system_print_uint32_5("[IK] fnlshpns_scl_cor->low + (1<<low_delta) > fnlshpns_scl_cor->high",DC_U, DC_U, DC_U, DC_U, DC_U);
            rval |= IK_ERR_0008;
        }

        {
            // advanced level control check
            ik_level_t level;

            level.low = fnlshpns_scl_cor->low;
            level.low_delta = fnlshpns_scl_cor->low_delta;
            level.high = fnlshpns_scl_cor->high;
            level.high_delta = fnlshpns_scl_cor->high_delta;
            rval |= check_advanced_level_control("fnlshpns_scl_cor", &level);
        }
    }
    return rval;
}

static uint32 ctx_check_fnlshpns_both_tdt_valid_range(const ik_final_sharpen_both_three_d_table_t *final_sharpen_both_three_d_table, const char *prefix)
{
    uint32 rval = IK_OK;

    if (final_sharpen_both_three_d_table == NULL) {
        amba_ik_system_print_str_5("[IK] detect final_sharpen_both_three_d_table get NULL pointer!", NULL, NULL, NULL, NULL, NULL);
        rval |= IK_ERR_0005;
    } else {
        rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "y_tone_offset", final_sharpen_both_three_d_table->y_tone_offset, 0U, 255U);
        rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "y_tone_shift", final_sharpen_both_three_d_table->y_tone_shift, 0U, 7U);
        rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "u_tone_offset", final_sharpen_both_three_d_table->u_tone_offset, 0U, 255U);
        rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "u_tone_shift", final_sharpen_both_three_d_table->u_tone_shift, 0U, 7U);
        rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "u_tone_bits", final_sharpen_both_three_d_table->u_tone_bits, 0U, 8U);
        rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "v_tone_offset", final_sharpen_both_three_d_table->v_tone_offset, 0U, 255U);
        rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "v_tone_shift", final_sharpen_both_three_d_table->v_tone_shift, 0U, 7U);
        rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "v_tone_bits", final_sharpen_both_three_d_table->v_tone_bits, 0U, 8U);
    }

    return rval;
}

static uint32 ctx_check_dzoom_info_valid_range(const ik_dzoom_info_t *dzoom_info, const char *prefix)
{
    uint32 rval = IK_OK;
    if ((dzoom_info == NULL) || (prefix == NULL)) {
        rval |= IK_ERR_0005;
    } else {
        if ((dzoom_info->zoom_x < 0x10000UL) && (dzoom_info->enable==1U)) {
            amba_ik_system_print_uint32_5("[IK] detect invalid zoom_x 0x%x. ", dzoom_info->zoom_x, DC_U, DC_U, DC_U, DC_U);
            rval |= IK_ERR_0008;
        }
        if ((dzoom_info->zoom_y < 0x10000UL) && (dzoom_info->enable==1U)) {
            amba_ik_system_print_uint32_5("[IK] detect invalid zoom_y 0x%x. ", dzoom_info->zoom_y, DC_U, DC_U, DC_U, DC_U);
            rval |= IK_ERR_0008;
        }
    }
    return rval;
}

static uint32 ctx_check_flip_mode_info_valid_range(uint32 mode, const char *prefix)
{
    uint32 rval = IK_OK;
    if ((mode==0UL) || (mode==IK_FLIP_RAW_V) || (mode==IK_FLIP_YUV_H) || (mode==IK_FLIP_RAW_V_YUV_H)  || (prefix == NULL)) {
        // OK
    } else {
        amba_ik_system_print_uint32_5("[IK] detect invalid flip mode %d. ", mode, DC_U, DC_U, DC_U, DC_U);
        rval |= IK_ERR_0008;
    }
    return rval;
}

static uint32 ctx_check_window_size_info_valid_range(const ik_window_size_info_t *window_size_info, const char *prefix)
{
    uint32 rval = IK_OK;
    if ((window_size_info == NULL) || (prefix == NULL)) {
        rval |= IK_ERR_0005;
    } else {
        if ((window_size_info->vin_sensor.width == 0U) || (window_size_info->vin_sensor.height == 0U)) {
            amba_ik_system_print_str_5("[IK] vin_sensor() is invalid!", NULL, NULL, NULL, NULL, NULL);
            rval |= IK_ERR_0008;
        }

        if ((window_size_info->main_dim.width == 0U) || (window_size_info->main_dim.height == 0U)) {
            amba_ik_system_print_str_5("[IK] main_win() is invalid!", NULL, NULL, NULL, NULL, NULL);
            rval |= IK_ERR_0008;
        }
    }
    return rval;
}

static uint32 ctx_check_cfa_window_size_info_valid_range(const ik_cfa_window_size_info_t *cfa_window_size_info, const char *prefix)
{
    uint32 rval = IK_OK;
    if ((cfa_window_size_info == NULL) || (prefix == NULL)) {
        rval |= IK_ERR_0005;
    } else {
        if (cfa_window_size_info->enable == 1UL) {
            if ((cfa_window_size_info->cfa.width == 0U) || (cfa_window_size_info->cfa.height == 0U)) {
                amba_ik_system_print_str_5("[IK] cfa_win() is invalid!", NULL, NULL, NULL, NULL, NULL);
                rval |= IK_ERR_0008;
            }
        }
    }
    return rval;
}

static uint32 ctx_check_vin_active_win_valid_range(const ik_vin_active_window_t *p_active_win, const char *prefix)
{
    uint32 rval = IK_OK;
    if ((p_active_win == NULL) || (prefix == NULL)) {
        rval |= IK_ERR_0005;
    } else {
        if(p_active_win->enable == 1UL) {
            if ((p_active_win->active_geo.width == 0U) || (p_active_win->active_geo.height == 0U)) {
                amba_ik_system_print_str_5("[IK] vin_sensor() is invalid!", NULL, NULL, NULL, NULL, NULL);
                rval |= IK_ERR_0008;
            }
            //start_x should be 32 alignment and start_y should be 2 alignment.
            if (((p_active_win->active_geo.start_x & 31U) != 0U) || ((p_active_win->active_geo.start_y & 1U) != 0U)) {
                amba_ik_system_print_uint32_5("[IK] active window(start_x: %d,start_y %d) is invalid! start_x should be 32 alignment and start_y should be 2 alignment!", p_active_win->active_geo.start_x, p_active_win->active_geo.start_y, DC_U, DC_U, DC_U);
                rval |= IK_ERR_0008;
            }
        }
    }
    return rval;
}

static uint32 ctx_check_set_dummy_win_margin_range_info_valid_range(const ik_dummy_margin_range_t *dmy_range, const char *prefix)
{
    uint32 rval = IK_OK;
    if ((dmy_range == NULL) || (prefix == NULL)) {
        rval |= IK_ERR_0005;
    } else {
        if (dmy_range->bottom > 0x10000UL) {
            amba_ik_system_print_uint32_5("[IK] detect invalid p_user_dmy_range->bottom 0x%x. ", dmy_range->bottom, DC_U, DC_U, DC_U, DC_U);
            rval |= IK_ERR_0008;
        }
        if (dmy_range->dummy_top > 0x10000UL) {
            amba_ik_system_print_uint32_5("[IK] detect invalid p_user_dmy_range->dummy_top 0x%x. ", dmy_range->dummy_top, DC_U, DC_U, DC_U, DC_U);
            rval |= IK_ERR_0008;
        }
        if (dmy_range->left > 0x10000UL) {
            amba_ik_system_print_uint32_5("[IK] detect invalid p_user_dmy_range->left 0x%x. ", dmy_range->left, DC_U, DC_U, DC_U, DC_U);
            rval |= IK_ERR_0008;
        }
        if (dmy_range->right > 0x10000UL) {
            amba_ik_system_print_uint32_5("[IK] detect invalid p_user_dmy_range->right 0x%x. ", dmy_range->right, DC_U, DC_U, DC_U, DC_U);
            rval |= IK_ERR_0008;
        }
    }
    return rval;
}

static uint32 ctx_check_resampler_str_valid_range(const ik_resampler_strength_t *p_fe_tc, const char *prefix)
{
    uint32 rval = IK_OK;
    if (p_fe_tc == NULL) {
        rval = IK_ERR_0005;
    } else {
        rval |= (uint32) ctx_check_uint32_parameter_valid_range(prefix, "cfa_cut_off_freq", p_fe_tc->cfa_cut_off_freq, 0U, 7U);
        rval |= (uint32) ctx_check_uint32_parameter_valid_range(prefix, "main_cut_off_freq", p_fe_tc->main_cut_off_freq, 0U, 7U);
    }

    return rval;
}

static uint32 ctx_check_motion_detect_valid_range(const ik_motion_detect_t *motion_detect, const char *prefix)
{
    uint32 rval = IK_OK;
    if (motion_detect == NULL) {
        rval |= IK_ERR_0005;
    } else {
        rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "enable", motion_detect->enable, 0U, 1U);
        if(motion_detect->enable == 1UL) {
            rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "local_lighting", motion_detect->local_lighting, 0U, 1U);
            rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "motion_response", motion_detect->motion_response, 1U, 511U);
            rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "slow_mo_sensitivity", motion_detect->slow_mo_sensitivity, 0U, 19U);
            if ((motion_detect->lev_add_1_method != 0U) && (motion_detect->lev_add_1_method != 2U) && \
                (motion_detect->lev_add_1_method != 7U) && (motion_detect->lev_add_1_method != 12U)) {
                amba_ik_system_print_uint32_5("motion_detect->lev_add_1_method %d not valid",motion_detect->lev_add_1_method, DC_U, DC_U, DC_U, DC_U);
                rval |= IK_ERR_0008;
            }
            if ((motion_detect->lev_add_1_method != 0U) && (motion_detect->lev_add_1_method != 2U) && \
                (motion_detect->lev_add_1_method != 7U) && (motion_detect->lev_add_1_method != 12U)) {
                amba_ik_system_print_uint32_5("motion_detect->lev_add_2_method %d not valid",motion_detect->lev_add_2_method, DC_U, DC_U, DC_U, DC_U);
                rval |= IK_ERR_0008;
            }
            if ((motion_detect->lev_add_1_method != 0U) && (motion_detect->lev_add_1_method != 2U) && \
                (motion_detect->lev_add_1_method != 7U) && (motion_detect->lev_add_1_method != 12U)) {
                amba_ik_system_print_uint32_5("motion_detect->lev_mul_1_method %d not valid",motion_detect->lev_mul_1_method, DC_U, DC_U, DC_U, DC_U);
                rval |= IK_ERR_0008;
            }
            if ((motion_detect->lev_add_1_method != 0U) && (motion_detect->lev_add_1_method != 2U) && \
                (motion_detect->lev_add_1_method != 7U) && (motion_detect->lev_add_1_method != 12U)) {
                amba_ik_system_print_uint32_5("motion_detect->lev_mul_2_method %d not valid",motion_detect->lev_mul_2_method, DC_U, DC_U, DC_U, DC_U);
                rval |= IK_ERR_0008;
            }
        }
    }
    return rval;
}

static uint32 ctx_check_static_bpc_internal_valid_range(const ik_static_bad_pixel_correction_internal_t *static_bpc, const char *prefix)
{
    uint32 rval = IK_OK;

    if (static_bpc == NULL) {
        amba_ik_system_print_str_5("[IK] detect static_bpc get NULL pointer!", DC_S, DC_S, DC_S, DC_S, DC_S);
        rval |= IK_ERR_0005;
    } else {
        /* Disable mode, discarding the checks as follow.*/
        /* Check SBP calibration version*/
        rval |= ctx_check_uint32_parameter_valid_range(prefix, "enable", static_bpc->enable, 0, 3);

        if (static_bpc->p_map == NULL) {
            amba_ik_system_print_str_5("[IK] SBP internal p_map = NULL !\n", DC_S, DC_S, DC_S, DC_S, DC_S);
            rval |= IK_ERR_0008;
        }
    }
    return rval;
}

static uint32 ctx_check_calib_warp_info_valid_range(const ik_warp_info_t *calib_warp_info, const char *prefix)
{
    uint32 rval = IK_OK;
    uintptr misra_uintptr;

    if (calib_warp_info == NULL) {
        amba_ik_system_print_str_5("[IK] detect calib_warp_info get NULL pointer!", DC_S, DC_S, DC_S, DC_S, DC_S);
        rval |= IK_ERR_0005;
    } else {
        rval |= ctx_check_uint32_parameter_valid_range(prefix, "version", calib_warp_info->version, IK_WARP_VER, IK_WARP_VER);
        rval |= ctx_check_uint32_parameter_valid_range(prefix, "hor_grid_num", calib_warp_info->hor_grid_num, 2UL, 256UL);
        rval |= ctx_check_uint32_parameter_valid_range(prefix, "ver_grid_num", calib_warp_info->ver_grid_num, 2UL, 192UL);
        rval |= ctx_check_uint32_parameter_valid_range(prefix, "tile_width_exp", calib_warp_info->tile_width_exp, 0UL, 9UL);
        rval |= ctx_check_uint32_parameter_valid_range(prefix, "tile_height_exp", calib_warp_info->tile_height_exp, 0UL, 9UL);
        rval |= ctx_check_uint32_parameter_valid_range(prefix, "vin_sensor_geo.width", calib_warp_info->vin_sensor_geo.width, 2UL, 6000UL);
        rval |= ctx_check_uint32_parameter_valid_range(prefix, "vin_sensor_geo.height", calib_warp_info->vin_sensor_geo.height, 2UL, 4000UL);
        rval |= ctx_check_uint32_parameter_valid_range(prefix, "vin_sensor_geo.h_sub_sample.factor_num", calib_warp_info->vin_sensor_geo.h_sub_sample.factor_num, 1UL, 8UL);
        rval |= ctx_check_uint32_parameter_valid_range(prefix, "vin_sensor_geo.h_sub_sample.factor_den", calib_warp_info->vin_sensor_geo.h_sub_sample.factor_den, 1UL, 8UL);
        rval |= ctx_check_uint32_parameter_valid_range(prefix, "vin_sensor_geo.v_sub_sample.factor_num", calib_warp_info->vin_sensor_geo.v_sub_sample.factor_num, 1UL, 8UL);
        rval |= ctx_check_uint32_parameter_valid_range(prefix, "vin_sensor_geo.v_sub_sample.factor_den", calib_warp_info->vin_sensor_geo.v_sub_sample.factor_den, 1UL, 8UL);
        (void)amba_ik_system_memcpy(&misra_uintptr, &calib_warp_info->pwarp, sizeof(uintptr));
        if (calib_warp_info->pwarp == NULL) {
            amba_ik_system_print_uint32_5("[IK] calib_warp_info->pwarp %p", misra_uintptr, DC_U, DC_U, DC_U, DC_U);
            rval |= IK_ERR_0008;
        }

        if (((calib_warp_info->hor_grid_num - 1UL) << calib_warp_info->tile_width_exp) < calib_warp_info->vin_sensor_geo.width) {
            amba_ik_system_print_uint32_5("[IK] (hor_grid_num %d - 1) * tile_width_exp %d < Calib vin width %d", calib_warp_info->hor_grid_num, (1UL << calib_warp_info->tile_width_exp), calib_warp_info->vin_sensor_geo.width, DC_U, DC_U);
            rval |= IK_ERR_0008;
        }

        if (((calib_warp_info->ver_grid_num - 1UL) << calib_warp_info->tile_height_exp) < calib_warp_info->vin_sensor_geo.height) {
            amba_ik_system_print_uint32_5("[IK] (ver_grid_num %d - 1) * tile_height_exp %d < Calib vin width %d", calib_warp_info->ver_grid_num, (1UL << calib_warp_info->tile_height_exp), calib_warp_info->vin_sensor_geo.height, DC_U, DC_U);
            rval |= IK_ERR_0008;
        }
    }
    return rval;
}

static uint32 ctx_check_warp_bind_info_valid_range(const ik_bind_info_t *warp_bind_info, const char *prefix)
{
    uint32 rval = IK_OK;
    if ((warp_bind_info == NULL) || (prefix == NULL)) {
        rval |= IK_ERR_0005;
    }
    return rval;
}

#define WARP_VIDEO_MAXHORGRID    (128UL)
#define WARP_VIDEO_MAXVERGRID    (96UL)
#define CAWARP_VIDEO_MAXHORGRID    (32UL)
#define CAWARP_VIDEO_MAXVERGRID    (48UL)
static uint32 ctx_check_warp_internal_valid_range(const ik_warp_internal_info_t *warp_internal, const char *prefix)
{
    uint32 rval = IK_OK;
    uintptr misra_uintptr_01, misra_uintptr_02;
    if ((warp_internal == NULL) || (prefix == NULL)) {
        (void)amba_ik_system_memcpy(&misra_uintptr_01, &warp_internal, sizeof(uintptr));
        amba_ik_system_print_uint32_5("[IK] warp_internal :0x%x", misra_uintptr_01, DC_U, DC_U, DC_U, DC_U);
        rval |= IK_ERR_0005;
    } else if((warp_internal->pwarp_horizontal_table == NULL) || (warp_internal->pwarp_vertical_table == NULL)) {
        (void)amba_ik_system_memcpy(&misra_uintptr_01, &warp_internal->pwarp_horizontal_table, sizeof(uintptr));
        (void)amba_ik_system_memcpy(&misra_uintptr_02, &warp_internal->pwarp_vertical_table, sizeof(uintptr));
        amba_ik_system_print_uint32_5("[IK]pwarp_horizontal_table :0x%x, pwarp_vertical_table :0x%x  ", misra_uintptr_01, misra_uintptr_02, DC_U, DC_U, DC_U);
        rval |= IK_ERR_0008;
    } else {
        if (warp_internal->grid_array_width > WARP_VIDEO_MAXHORGRID) {
            amba_ik_system_print_uint32_5("[IK] [Error]: In video mode, warp_internal->grid_array_width = %d > WARP_VIDEO_MAXHORGRID (%d)"
                                          , warp_internal->grid_array_width, WARP_VIDEO_MAXHORGRID, DC_U, DC_U, DC_U);
            rval |= IK_ERR_0008;
        }
        if (warp_internal->grid_array_height> WARP_VIDEO_MAXVERGRID) {
            amba_ik_system_print_uint32_5("[IK] [Error]: In video mode, warp_internal->grid_array_height = %d > WARP_VIDEO_MAXVERGRID (%d)"
                                          , warp_internal->grid_array_height, WARP_VIDEO_MAXVERGRID, DC_U, DC_U, DC_U);
            rval |= IK_ERR_0008;
        }
        if (warp_internal->vert_warp_grid_array_width > WARP_VIDEO_MAXHORGRID) {
            amba_ik_system_print_uint32_5("[IK] [Error]: In video mode, warp_internal->vert_warp_grid_array_width = %d > WARP_VIDEO_MAXHORGRID (%d)"
                                          , warp_internal->vert_warp_grid_array_width, WARP_VIDEO_MAXHORGRID, DC_U, DC_U, DC_U);
            rval |= IK_ERR_0008;
        }
        if (warp_internal->vert_warp_grid_array_height> WARP_VIDEO_MAXVERGRID) {
            amba_ik_system_print_uint32_5("[IK] [Error]: In video mode, warp_internal->vert_warp_grid_array_height = %d > WARP_VIDEO_MAXVERGRID (%d)"
                                          , warp_internal->vert_warp_grid_array_height, WARP_VIDEO_MAXVERGRID, DC_U, DC_U, DC_U);
            rval |= IK_ERR_0008;
        }
    }
    return rval;
}

static uint32 ctx_check_ca_warp_internal_valid_range(const ik_cawarp_internal_info_t *cawarp_internal, const char *prefix)
{
    uint32 rval = IK_OK;
    uintptr misra_uintptr_01;
    if ((cawarp_internal == NULL) || (prefix == NULL)) {
        (void)amba_ik_system_memcpy(&misra_uintptr_01, &cawarp_internal, sizeof(uintptr));
        amba_ik_system_print_uint32_5("[IK] cawarp_internal :0x%x", misra_uintptr_01, DC_U, DC_U, DC_U, DC_U);
        rval |= IK_ERR_0005;
    } else {
        if (cawarp_internal->horz_pass_grid_array_width> CAWARP_VIDEO_MAXHORGRID) {
            amba_ik_system_print_uint32_5("[IK] [Error]: In video mode, warp_internal->horz_pass_grid_array_width = %d > CAWARP_VIDEO_MAXHORGRID (%d)"
                                          , cawarp_internal->horz_pass_grid_array_width, CAWARP_VIDEO_MAXHORGRID, DC_U, DC_U, DC_U);
            rval |= IK_ERR_0008;
        }
        if (cawarp_internal->horz_pass_grid_array_height> CAWARP_VIDEO_MAXVERGRID) {
            amba_ik_system_print_uint32_5("[IK] [Error]: In video mode, warp_internal->horz_pass_grid_array_height = %d > CAWARP_VIDEO_MAXVERGRID (%d)"
                                          , cawarp_internal->horz_pass_grid_array_height, CAWARP_VIDEO_MAXVERGRID, DC_U, DC_U, DC_U);
            rval |= IK_ERR_0008;
        }
        if (cawarp_internal->vert_pass_grid_array_width> CAWARP_VIDEO_MAXHORGRID) {
            amba_ik_system_print_uint32_5("[IK] [Error]: In video mode, warp_internal->vert_pass_grid_array_width = %d > CAWARP_VIDEO_MAXHORGRID (%d)"
                                          , cawarp_internal->vert_pass_grid_array_width, CAWARP_VIDEO_MAXHORGRID, DC_U, DC_U, DC_U);
            rval |= IK_ERR_0008;
        }
        if (cawarp_internal->vert_pass_grid_array_height> CAWARP_VIDEO_MAXVERGRID) {
            amba_ik_system_print_uint32_5("[IK] [Error]: In video mode, warp_internal->vert_pass_grid_array_height = %d > CAWARP_VIDEO_MAXVERGRID (%d)"
                                          , cawarp_internal->vert_pass_grid_array_height, CAWARP_VIDEO_MAXVERGRID, DC_U, DC_U, DC_U);
            rval |= IK_ERR_0008;
        }
    }
    return rval;
}

static uint32 ctx_check_ae_gain_valid_range(const ik_ae_gain_t *ae_gain, const char *prefix)
{
    uint32 rval = IK_OK;
    if (ae_gain == NULL) {
        rval |= IK_ERR_0005;
    } else {
        rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "ae_gain", ae_gain->ae_gain, 0, 0xFFFFFF);
    }
    return rval;
}

static uint32 ctx_check_global_dgain_valid_range(const ik_global_dgain_t *global_dgain, const char *prefix)
{
    uint32 rval = IK_OK;
    if (global_dgain == NULL) {
        rval |= IK_ERR_0005;
    } else {
        rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "global_dgain", global_dgain->global_dgain, 0, 0xFFFFFF);
    }
    return rval;
}

static uint32 ctx_check_dgain_sat_lvl_valid_range(const ik_dgain_saturation_level_t *dgain_sauration_level, const char *prefix)
{
    uint32 rval = IK_OK;
    if (dgain_sauration_level == NULL) {
        rval |= IK_ERR_0005;
    } else {
        rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "level_red", dgain_sauration_level->level_red, 0, 32767);
        rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "level_green_even", dgain_sauration_level->level_green_even, 0, 32767);
        rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "level_green_odd", dgain_sauration_level->level_green_odd, 0, 32767);
        rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "level_blue", dgain_sauration_level->level_blue, 0, 32767);
    }
    return rval;
}

static uint32 ctx_check_video_mctf_internal_valid_range(const ik_mctf_internal_t *video_mctf_internal, const char *prefix)
{
    uint32 rval = IK_OK;
    if (video_mctf_internal == NULL) {
        rval |= IK_ERR_0005;
    } else {
        rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "ta_frames_combine_num1", video_mctf_internal->ta_frames_combine_num1, 0, 6);
        rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "still_thta_frames_combine_num2resh", video_mctf_internal->ta_frames_combine_num2, 0, 6);
        rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "higher_iq_lower_perf", video_mctf_internal->higher_iq_lower_perf, 0, 1);
    }
    return rval;
}

static uint32 ctx_check_motion_detect_and_mctf_valid_range(const ik_motion_detect_and_mctf_t *p_motion_detect_and_mctf, const char *prefix)
{
    uint32 rval = IK_OK;
    if (p_motion_detect_and_mctf == NULL) {
        rval |= IK_ERR_0005;
    } else {
        rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "local_lighting_radius", p_motion_detect_and_mctf->local_lighting_radius, 0, 68);
    }
    return rval;
}

static uint32 ctx_check_deferred_blc_valid_range(const ik_deferred_blc_level_t *deferred_blc, const char *prefix)
{
    uint32 rval = IK_OK;
    if ((deferred_blc == NULL) || (prefix == NULL)) {
        rval = IK_ERR_0005;
    }

    return rval;
}

static uint32 ctx_check_aaa_stat_info_valid_range(const ik_aaa_stat_info_t *p_stat_info, const char *prefix)
{
    uint32 rval = IK_OK;

    if (p_stat_info == NULL) {
        rval = IK_ERR_0005;
    } else {
        rval |= (uint32) ctx_check_uint32_parameter_valid_range(prefix, "awb_tile_num_col", p_stat_info->awb_tile_num_col, 2U, 32U);
        rval |= (uint32) ctx_check_uint32_parameter_valid_range(prefix, "awb_tile_num_row", p_stat_info->awb_tile_num_row, 2U, 32U);
        rval |= (uint32) ctx_check_uint32_parameter_valid_range(prefix, "ae_tile_num_col", p_stat_info->ae_tile_num_col, 2U, 12U);
        rval |= (uint32) ctx_check_uint32_parameter_valid_range(prefix, "ae_tile_num_row", p_stat_info->ae_tile_num_row, 2U, 8U);
        rval |= (uint32) ctx_check_uint32_parameter_valid_range(prefix, "af_tile_num_col", p_stat_info->af_tile_num_col, 2U, 12U);
        rval |= (uint32) ctx_check_uint32_parameter_valid_range(prefix, "af_tile_num_row", p_stat_info->af_tile_num_row, 2U, 8U);
    }

    return rval;
}

static uint32 ctx_check_af_stat_ex_info_valid_range(const ik_af_stat_ex_info_t *af_stat_ex_info, const char *prefix)
{
    uint32 rval = IK_OK;
    if ((af_stat_ex_info == NULL) || (prefix == NULL)) {
        rval = IK_ERR_0005;
    }

    return rval;
}

static uint32 ctx_check_pg_af_stat_ex_info_valid_range(const ik_pg_af_stat_ex_info_t *pg_af_stat_ex_info, const char *prefix)
{
    uint32 rval = IK_OK;
    if ((pg_af_stat_ex_info == NULL) || (prefix == NULL)) {
        rval = IK_ERR_0005;
    }

    return rval;
}

static uint32 ctx_check_aaa_stat_float_tile_valid_range(const ik_aaa_float_info_t *float_tile_info, const char *prefix)
{
    uint32 rval = IK_OK;
    if ((float_tile_info == NULL) || (prefix == NULL)) {
        rval = IK_ERR_0005;
    }

    return rval;
}

static uint32 ctx_check_fusion(const ik_fusion_t *p_fusion, const char *prefix)
{
    uint32 rval = IK_OK;
    uint32 i;
    if (p_fusion == NULL) {
        rval |= IK_ERR_0005;
    } else {
        rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "enable", p_fusion->enable, 0UL, 1UL);
        if(p_fusion->enable == 1U) {
            rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "pass0_act_mul", p_fusion->act_mul, 0UL, 1023UL);
            rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "pass0_act_sub", p_fusion->act_sub, 0UL, 255UL);
            rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "pass0_sat_mul", p_fusion->sat_mul, 0UL, 1023UL);
            rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "pass0_sat_sub", p_fusion->sat_sub, 0UL, 128UL);
            for (i = 0; i < 16U; i++) {
                rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "pass0_luma_lu[*]", p_fusion->luma_lu[i], 0UL, 545UL);
            }
            for (i = 0; i < 256U; i++) {
                rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "pass1_luma_diff_table[*]", p_fusion->luma_diff_table[i], 0UL, 1023UL);
            }
            rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "pass1_max_rgb", p_fusion->max_rgb, 0UL, 1023UL);
            rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "pass1_max_rgb", p_fusion->max_rgb, 0UL, 1023UL);
            rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "pass1_sat_and_act_mul", p_fusion->sat_and_act_mul, 0UL, 1023UL);
            for (i = 0; i < 10U; i++) {
                rval |= (uint32)ctx_check_int32_parameter_valid_range(prefix, "freq_blend_fir[*]", p_fusion->freq_blend_fir1[i], -256, 256);
                rval |= (uint32)ctx_check_int32_parameter_valid_range(prefix, "freq_blend_fir[*]", p_fusion->freq_blend_fir2[i], -256, 256);
            }
        }
    }
    return rval;
}


static uint32 ctx_check_wide_chroma_filter_combine(const ik_wide_chroma_filter_combine_t *p_wide_chroma_filter_combine, const char *prefix)
{
    uint32 rval = IK_OK;
    if (p_wide_chroma_filter_combine == NULL) {
        rval |= IK_ERR_0005;
    } else {
        rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "T0_cb", p_wide_chroma_filter_combine->T0_cb, 0UL, 62UL);
        rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "T0_cr", p_wide_chroma_filter_combine->T0_cr, 0UL, 62UL);
        rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "T1_cb", p_wide_chroma_filter_combine->T1_cb, 1UL, 63UL);
        rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "T1_cr", p_wide_chroma_filter_combine->T1_cr, 1UL, 63UL);
        if(p_wide_chroma_filter_combine->T0_cb > p_wide_chroma_filter_combine->T1_cb) {
            rval |= IK_ERR_0008;
            amba_ik_system_print_uint32_5("wide_chroma_filter_combine T0_cb=%d  should not larger than T1_cb=%d ", p_wide_chroma_filter_combine->T0_cb, p_wide_chroma_filter_combine->T1_cb, DC_U, DC_U, DC_U);
        }
        if(p_wide_chroma_filter_combine->T0_cr > p_wide_chroma_filter_combine->T1_cr) {
            rval |= IK_ERR_0008;
            amba_ik_system_print_uint32_5("wide_chroma_filter_combine T0_cr=%d  should not larger than T1_cr=%d ", p_wide_chroma_filter_combine->T0_cr, p_wide_chroma_filter_combine->T1_cr, DC_U, DC_U, DC_U);
        }
        rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "alpha_max_cb", p_wide_chroma_filter_combine->alpha_max_cb, 0UL, 255UL);
        rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "alpha_max_cr", p_wide_chroma_filter_combine->alpha_max_cr, 0UL, 255UL);
        rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "alpha_min_cb", p_wide_chroma_filter_combine->alpha_min_cb, 0UL, 255UL);
        rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "alpha_min_cr", p_wide_chroma_filter_combine->alpha_min_cr, 0UL, 255UL);

        rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "T0T1_add_high_cb", p_wide_chroma_filter_combine->T0T1_add_high_cb, 0UL, 255UL);
        rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "T0T1_add_high_cr", p_wide_chroma_filter_combine->T0T1_add_high_cr, 0UL, 255UL);

        rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "T0T1_add_high_delta_cb", p_wide_chroma_filter_combine->T0T1_add_high_delta_cb, 0UL, 7UL);
        rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "T0T1_add_high_delta_cr", p_wide_chroma_filter_combine->T0T1_add_high_delta_cr, 0UL, 7UL);

        rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "T0T1_add_high_strength_cr", \
                p_wide_chroma_filter_combine->T0T1_add_high_strength_cr, 0UL, 255UL);
        rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "T0T1_add_high_strength_cb", \
                p_wide_chroma_filter_combine->T0T1_add_high_strength_cb, 0UL, 255UL);

        rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "T0T1_add_low_cb", \
                p_wide_chroma_filter_combine->T0T1_add_low_cb, 0UL, 255UL);
        rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "T0T1_add_low_cr", \
                p_wide_chroma_filter_combine->T0T1_add_low_cr, 0UL, 255UL);

        rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "T0T1_add_high_delta_cb", p_wide_chroma_filter_combine->T0T1_add_low_delta_cb, 0UL, 7UL);
        rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "T0T1_add_high_delta_cr", p_wide_chroma_filter_combine->T0T1_add_low_delta_cr, 0UL, 7UL);

        rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "T0T1_add_low_strength_cb", \
                p_wide_chroma_filter_combine->T0T1_add_low_strength_cb, 0UL, 255UL);
        rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "T0T1_add_low_strength_cr", \
                p_wide_chroma_filter_combine->T0T1_add_low_strength_cr, 0UL, 255UL);
        rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "T0T1_add_mid_strength_cb", \
                p_wide_chroma_filter_combine->T0T1_add_mid_strength_cb, 0UL, 255UL);
        rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "T0T1_add_mid_strength_cr", \
                p_wide_chroma_filter_combine->T0T1_add_mid_strength_cr, 0UL, 255UL);

        rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "max_change_high_cb", \
                p_wide_chroma_filter_combine->max_change_high_cb, 0UL, 255UL);
        rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "max_change_high_cr", \
                p_wide_chroma_filter_combine->max_change_high_cr, 0UL, 255UL);
        rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "max_change_high_delta_cb", \
                p_wide_chroma_filter_combine->max_change_high_delta_cb, 0UL, 7UL);
        rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "max_change_high_delta_cr", \
                p_wide_chroma_filter_combine->max_change_high_delta_cr, 0UL, 7UL);
        rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "max_change_high_strength_cb", \
                p_wide_chroma_filter_combine->max_change_high_strength_cb, 0UL, 255UL);
        rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "max_change_high_strength_cr", \
                p_wide_chroma_filter_combine->max_change_high_strength_cr, 0UL, 255UL);

        rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "max_change_low_cb", \
                p_wide_chroma_filter_combine->max_change_low_cb, 0UL, 255UL);
        rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "max_change_low_cr", \
                p_wide_chroma_filter_combine->max_change_low_cr, 0UL, 255UL);

        rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "max_change_low_delta_cb", \
                p_wide_chroma_filter_combine->max_change_low_delta_cb, 0UL, 255UL);
        rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "max_change_low_delta_cr", \
                p_wide_chroma_filter_combine->max_change_low_delta_cr, 0UL, 255UL);

        rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "max_change_low_strength_cb", \
                p_wide_chroma_filter_combine->max_change_low_strength_cb, 0UL, 255UL);
        rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "max_change_low_strength_cr", \
                p_wide_chroma_filter_combine->max_change_low_strength_cr, 0UL, 255UL);

        rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "max_change_mid_strength_cb", \
                p_wide_chroma_filter_combine->max_change_mid_strength_cb, 0UL, 255UL);
        rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "max_change_mid_strength_cr", \
                p_wide_chroma_filter_combine->max_change_mid_strength_cr, 0UL, 255UL);

    }
    return rval;
}

static uint32 ctx_check_hi_asf_valid_range(const ik_hi_asf_t *p_hi_asf, const char *prefix)
{
    uint32 rval = IK_OK, i, j;
    const ik_asf_fir_t *fir;
    if (p_hi_asf == NULL) {
        amba_ik_system_print_str_5("[IK] p_hi_asf input is NULL address!", NULL, NULL, NULL, NULL, NULL);
        rval |= IK_ERR_0005;
    } else {
        if (p_hi_asf->enable!=0U) {
            rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "enable", p_hi_asf->enable, 0, 1);
            fir = &p_hi_asf->fir;
            rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "fir->specify", fir->specify, 2, 4);
            rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "fir->strength_iso", fir->strength_iso, 0, 256);
            rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "fir->strength_dir", fir->strength_dir, 0, 256);
            for (i = 0U; i < 9U; i++) {
                rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "fir->per_dir_fir_iso_strengths[]", fir->per_dir_fir_iso_strengths[i], 0, 256);
                rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "fir->per_dir_fir_dir_strengths[]", fir->per_dir_fir_dir_strengths[i], 0, 256);
                rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "fir->per_dir_fir_dir_amounts[]", fir->per_dir_fir_dir_amounts[i], 0, 256);
            }
            for (i = 0U; i < 9U; i++) {
                for (j = 0U; j < 25U; j++) {
                    rval |= (uint32)ctx_check_int32_parameter_valid_range(prefix, "fir->coefs[]", fir->coefs[i][j], -1024, 1023);
                }
            }
            rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "directional_decide_t0", p_hi_asf->directional_decide_t0, 0, 255);
            rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "directional_decide_t1", p_hi_asf->directional_decide_t1, 0, 255);
            if (p_hi_asf->directional_decide_t0 > p_hi_asf->directional_decide_t1) {
                amba_ik_system_print_uint32_5("[IK] p_hi_asf->directional_decide_t0 > p_hi_asf->directional_decide_t1 is incorrect", DC_U, DC_U, DC_U, DC_U, DC_U);
                rval |= IK_ERR_0008;
            }
            rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "adapt->alpha_max_down", p_hi_asf->adapt.alpha_max_down, 0, 8);
            rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "adapt->alpha_max_up", p_hi_asf->adapt.alpha_max_up, 0, 8);
            rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "adapt->alpha_min_down", p_hi_asf->adapt.alpha_min_down, 0, 8);
            rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "adapt->alpha_min_up", p_hi_asf->adapt.alpha_min_up, 0, 8);
            rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "adapt->t0_down", p_hi_asf->adapt.t0_down, 0, 252);
            if ((p_hi_asf->adapt.t0_down % 2U) != 0U) {
                amba_ik_system_print_uint32_5("[IK] p_hi_asf->adapt.t0_down = %d should be even", p_hi_asf->adapt.t0_down, DC_U, DC_U, DC_U, DC_U);
                rval |= IK_ERR_0008;
            }

            rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "adapt->t0_up", p_hi_asf->adapt.t0_up, 0U, 254U);
            if ((p_hi_asf->adapt.t0_down % 2U) != 0U) {
                amba_ik_system_print_uint32_5("[IK] p_hi_asf->adapt.t0_up = %d should be even", p_hi_asf->adapt.t0_up, DC_U, DC_U, DC_U, DC_U);
                rval |= IK_ERR_0008;
            }

            rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "adapt->t1_down", p_hi_asf->adapt.t1_down, 2U, 254U);
            if ((p_hi_asf->adapt.t1_down % 2U) != 0U) {
                amba_ik_system_print_uint32_5("[IK] p_hi_asf->adapt.t1_down = %d should be even", p_hi_asf->adapt.t1_down, DC_U, DC_U, DC_U, DC_U);
                rval |= IK_ERR_0008;
            }

            rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "adapt->t1_up", p_hi_asf->adapt.t1_up, 2U, 254U);
            if ((p_hi_asf->adapt.t0_down % 2U) != 0U) {
                amba_ik_system_print_uint32_5("[IK] p_hi_asf->adapt.t1_up = %d should be even", p_hi_asf->adapt.t1_up, DC_U, DC_U, DC_U, DC_U);
                rval |= IK_ERR_0008;
            }

            if (p_hi_asf->adapt.t0_up > p_hi_asf->adapt.t1_up) {
                amba_ik_system_print_uint32_5("[IK] p_hi_asf->adapt->T1_up=%d should larger than T0_up= %d in ctx_check_hi_asf_valid_range", p_hi_asf->adapt.t1_up, p_hi_asf->adapt.t0_up, DC_U, DC_U, DC_U);
                rval |= IK_ERR_0008;
            }

            if (p_hi_asf->adapt.t0_down > p_hi_asf->adapt.t1_down) {
                amba_ik_system_print_uint32_5("[IK] p_hi_asf->adapt->t1_down=%d should larger than t0_down= %d in ctx_check_hi_asf_valid_range", p_hi_asf->adapt.t1_down, p_hi_asf->adapt.t0_down, DC_U, DC_U, DC_U);
                rval |= IK_ERR_0008;
            }

            rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "level_str_adjust->high", p_hi_asf->level_str_adjust.high, 1UL, 254UL);
            rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "level_str_adjust->low", p_hi_asf->level_str_adjust.low, 0UL, 253UL);
            rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "level_str_adjust->low_delta", p_hi_asf->level_str_adjust.low_delta, 0UL, 7UL);
            rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "level_str_adjust->low_strength", p_hi_asf->level_str_adjust.low_strength, 0, 64);
            rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "level_str_adjust->high_delta", p_hi_asf->level_str_adjust.high_delta, 0UL, 7UL);
            rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "level_str_adjust->high_strength", p_hi_asf->level_str_adjust.high_strength, 0, 64);
            rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "level_str_adjust->mid_strength", p_hi_asf->level_str_adjust.mid_strength, 0, 64);

            rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "t0_t1_div->high", p_hi_asf->t0_t1_div.high, 1UL, 254UL);
            rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "t0_t1_div->low", p_hi_asf->t0_t1_div.low, 0UL, 253UL);
            rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "t0_t1_div->low_delta", p_hi_asf->t0_t1_div.low_delta, 0UL, 7UL);
            rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "t0_t1_div->low_strength", p_hi_asf->t0_t1_div.low_strength, 0, 64);
            rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "t0_t1_div->high_delta", p_hi_asf->t0_t1_div.high_delta, 0UL, 7UL);
            rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "t0_t1_div->high_strength", p_hi_asf->t0_t1_div.high_strength, 0, 64);
            rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "t0_t1_div->mid_strength", p_hi_asf->t0_t1_div.mid_strength, 0, 64);

            rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "max_change_up", p_hi_asf->max_change_not_t0_t1_alpha, 0, 1);

            rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "max_change_up", p_hi_asf->max_change_up, 0, 255);
            rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "max_change_down", p_hi_asf->max_change_down, 0, 255);

            {
                // advanced level control check
                ik_level_t level;

                level.low = p_hi_asf->level_str_adjust.low;
                level.low_delta = p_hi_asf->level_str_adjust.low_delta;
                level.high = p_hi_asf->level_str_adjust.high;
                level.high_delta = p_hi_asf->level_str_adjust.high_delta;
                rval |= check_advanced_level_control("hi_advanced_spatial_filter->level_str_adjust", &level);

                level.low = p_hi_asf->t0_t1_div.low;
                level.low_delta = p_hi_asf->t0_t1_div.low_delta;
                level.high = p_hi_asf->t0_t1_div.high;
                level.high_delta = p_hi_asf->t0_t1_div.high_delta;
                rval |= check_advanced_level_control("hi_advanced_spatial_filter->t0_t1_div", &level);
            }
        }
    }
    return rval;
}

static uint32 ctx_check_hi_asf_a_valid_range(const ik_hi_asf_a_t *p_hi_asf_a, const char *prefix)
{
    uint32 rval = IK_OK, i, j;
    const ik_asf_fir_t *fir;
    if (p_hi_asf_a == NULL) {
        amba_ik_system_print_str_5("[IK] p_hi_asf_a input is NULL address!", NULL, NULL, NULL, NULL, NULL);
        rval |= IK_ERR_0005;
    } else {
        if (p_hi_asf_a->enable!=0U) {
            rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "enable", p_hi_asf_a->enable, 0, 1);
            fir = &p_hi_asf_a->fir;
            rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "fir->specify", fir->specify, 2, 4);
            rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "fir->strength_iso", fir->strength_iso, 0, 256);
            rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "fir->strength_dir", fir->strength_dir, 0, 256);
            for (i = 0U; i < 9U; i++) {
                rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "fir->per_dir_fir_iso_strengths[]", fir->per_dir_fir_iso_strengths[i], 0, 256);
                rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "fir->per_dir_fir_dir_strengths[]", fir->per_dir_fir_dir_strengths[i], 0, 256);
                rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "fir->per_dir_fir_dir_amounts[]", fir->per_dir_fir_dir_amounts[i], 0, 256);
                for (j = 0U; j < 25U; j++) {
                    rval |= (uint32)ctx_check_int32_parameter_valid_range(prefix, "fir->coefs[]", fir->coefs[i][j], -1024, 1023);
                }
            }
            rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "directional_decide_t0", p_hi_asf_a->directional_decide_t0, 0, 255);
            rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "directional_decide_t1", p_hi_asf_a->directional_decide_t1, 0, 255);
            if (p_hi_asf_a->directional_decide_t0 > p_hi_asf_a->directional_decide_t1) {
                amba_ik_system_print_uint32_5("[IK] p_hi_asf_a->directional_decide_t0 > p_hi_asf_a->directional_decide_t1 is incorrect", DC_U, DC_U, DC_U, DC_U, DC_U);
                rval |= IK_ERR_0008;
            }
            rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "adapt->alpha_max_down", p_hi_asf_a->adapt.alpha_max_down, 0, 8);
            rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "adapt->alpha_max_up", p_hi_asf_a->adapt.alpha_max_up, 0, 8);
            rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "adapt->alpha_min_down", p_hi_asf_a->adapt.alpha_min_down, 0, 8);
            rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "adapt->alpha_min_up", p_hi_asf_a->adapt.alpha_min_up, 0, 8);
            rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "adapt->t0_down", p_hi_asf_a->adapt.t0_down, 0, 252);
            if ((p_hi_asf_a->adapt.t0_down % 2U) != 0U) {
                amba_ik_system_print_uint32_5("[IK] p_hi_asf_a->adapt.t0_down = %d should be even", p_hi_asf_a->adapt.t0_down, DC_U, DC_U, DC_U, DC_U);
                rval |= IK_ERR_0008;
            }

            rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "adapt->t0_up", p_hi_asf_a->adapt.t0_up, 0U, 254U);
            if ((p_hi_asf_a->adapt.t0_down % 2U) != 0U) {
                amba_ik_system_print_uint32_5("[IK] p_hi_asf_a->adapt.t0_up = %d should be even", p_hi_asf_a->adapt.t0_up, DC_U, DC_U, DC_U, DC_U);
                rval |= IK_ERR_0008;
            }

            rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "adapt->t1_down", p_hi_asf_a->adapt.t1_down, 2U, 254U);
            if ((p_hi_asf_a->adapt.t1_down % 2U) != 0U) {
                amba_ik_system_print_uint32_5("[IK] p_hi_asf_a->adapt.t1_down = %d should be even", p_hi_asf_a->adapt.t1_down, DC_U, DC_U, DC_U, DC_U);
                rval |= IK_ERR_0008;
            }

            rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "adapt->t1_up", p_hi_asf_a->adapt.t1_up, 2U, 254U);
            if ((p_hi_asf_a->adapt.t0_down % 2U) != 0U) {
                amba_ik_system_print_uint32_5("[IK] p_hi_asf_a->adapt.t1_up = %d should be even", p_hi_asf_a->adapt.t1_up, DC_U, DC_U, DC_U, DC_U);
                rval |= IK_ERR_0008;
            }

            if (p_hi_asf_a->adapt.t0_up > p_hi_asf_a->adapt.t1_up) {
                amba_ik_system_print_uint32_5("[IK] p_hi_asf_a->adapt->T1_up=%d should larger than T0_up= %d in ctx_check_hi_asf_a_valid_range", p_hi_asf_a->adapt.t1_up, p_hi_asf_a->adapt.t0_up, DC_U, DC_U, DC_U);
                rval |= IK_ERR_0008;
            }

            if (p_hi_asf_a->adapt.t0_down > p_hi_asf_a->adapt.t1_down) {
                amba_ik_system_print_uint32_5("[IK] p_hi_asf_a->adapt->t1_down=%d should larger than t0_down= %d in ctx_check_hi_asf_a_valid_range", p_hi_asf_a->adapt.t1_down, p_hi_asf_a->adapt.t0_down, DC_U, DC_U, DC_U);
                rval |= IK_ERR_0008;
            }

            rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "level_str_adjust->high", p_hi_asf_a->level_str_adjust.high, 1UL, 254UL);
            rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "level_str_adjust->low", p_hi_asf_a->level_str_adjust.low, 0UL, 253UL);
            rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "level_str_adjust->low_delta", p_hi_asf_a->level_str_adjust.low_delta, 0UL, 7UL);
            rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "level_str_adjust->low_strength", p_hi_asf_a->level_str_adjust.low_strength, 0, 64);
            rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "level_str_adjust->high_delta", p_hi_asf_a->level_str_adjust.high_delta, 0UL, 7UL);
            rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "level_str_adjust->high_strength", p_hi_asf_a->level_str_adjust.high_strength, 0, 64);
            rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "level_str_adjust->mid_strength", p_hi_asf_a->level_str_adjust.mid_strength, 0, 64);
            if ((p_hi_asf_a->level_str_adjust.method==0UL) || (p_hi_asf_a->level_str_adjust.method==11UL)) {
                // pass
            } else {
                amba_ik_system_print_uint32_5("[IK] p_hi_asf_a->level_str_adjust.method:%d is invalid", p_hi_asf_a->level_str_adjust.method, DC_U, DC_U, DC_U, DC_U);
                rval |= IK_ERR_0008;
            }
            rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "t0_t1_div->high", p_hi_asf_a->t0_t1_div.high, 1UL, 254UL);
            rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "t0_t1_div->low", p_hi_asf_a->t0_t1_div.low, 0UL, 253UL);
            rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "t0_t1_div->low_delta", p_hi_asf_a->t0_t1_div.low_delta, 0UL, 7UL);
            rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "t0_t1_div->low_strength", p_hi_asf_a->t0_t1_div.low_strength, 0, 64);
            rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "t0_t1_div->high_delta", p_hi_asf_a->t0_t1_div.high_delta, 0UL, 7UL);
            rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "t0_t1_div->high_strength", p_hi_asf_a->t0_t1_div.high_strength, 0, 64);
            rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "t0_t1_div->mid_strength", p_hi_asf_a->t0_t1_div.mid_strength, 0, 64);
            if ((p_hi_asf_a->t0_t1_div.method==0UL) || (p_hi_asf_a->t0_t1_div.method==11UL)) {
                // pass
            } else {
                amba_ik_system_print_uint32_5("[IK] p_hi_asf_a->t0_t1_div.method:%d is invalid", p_hi_asf_a->t0_t1_div.method, DC_U, DC_U, DC_U, DC_U);
                rval |= IK_ERR_0008;
            }

            rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "max_change_up", p_hi_asf_a->max_change_not_t0_t1_alpha, 0, 1);

            rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "max_change_up", p_hi_asf_a->max_change_up, 0, 255);
            rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "max_change_down", p_hi_asf_a->max_change_down, 0, 255);
        }
    }
    return rval;
}

static uint32 ctx_check_hi_asf_b1_valid_range(const ik_hi_asf_b1_t *p_hi_asf_b1, const char *prefix)
{
    uint32 rval = IK_OK, i, j;
    const ik_asf_fir_t *fir;
    if (p_hi_asf_b1 == NULL) {
        amba_ik_system_print_str_5("[IK] p_hi_asf_b1 input is NULL address!", NULL, NULL, NULL, NULL, NULL);
        rval |= IK_ERR_0005;
    } else {
        if (p_hi_asf_b1->enable!=0U) {
            rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "enable", p_hi_asf_b1->enable, 0, 1);
            fir = &p_hi_asf_b1->fir;
            rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "fir->specify", fir->specify, 2, 4);
            rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "fir->strength_iso", fir->strength_iso, 0, 256);
            rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "fir->strength_dir", fir->strength_dir, 0, 256);
            for (i = 0U; i < 9U; i++) {
                rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "fir->per_dir_fir_iso_strengths[]", fir->per_dir_fir_iso_strengths[i], 0, 256);
                rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "fir->per_dir_fir_dir_strengths[]", fir->per_dir_fir_dir_strengths[i], 0, 256);
                rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "fir->per_dir_fir_dir_amounts[]", fir->per_dir_fir_dir_amounts[i], 0, 256);
            }
            for (i = 0U; i < 9U; i++) {
                for (j = 0U; j < 25U; j++) {
                    rval |= (uint32)ctx_check_int32_parameter_valid_range(prefix, "fir->coefs[]", fir->coefs[i][j], -1024, 1023);
                }
            }
            rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "directional_decide_t0", p_hi_asf_b1->directional_decide_t0, 0, 255);
            rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "directional_decide_t1", p_hi_asf_b1->directional_decide_t1, 0, 255);
            if (p_hi_asf_b1->directional_decide_t0 > p_hi_asf_b1->directional_decide_t1) {
                amba_ik_system_print_uint32_5("[IK] p_hi_asf_b1->directional_decide_t0 > p_hi_asf_b1->directional_decide_t1 is incorrect", DC_U, DC_U, DC_U, DC_U, DC_U);
                rval |= IK_ERR_0008;
            }
            rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "adapt->alpha_max_down", p_hi_asf_b1->adapt.alpha_max_down, 0, 8);
            rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "adapt->alpha_max_up", p_hi_asf_b1->adapt.alpha_max_up, 0, 8);
            rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "adapt->alpha_min_down", p_hi_asf_b1->adapt.alpha_min_down, 0, 8);
            rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "adapt->alpha_min_up", p_hi_asf_b1->adapt.alpha_min_up, 0, 8);
            rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "adapt->t0_down", p_hi_asf_b1->adapt.t0_down, 0, 252);
            if ((p_hi_asf_b1->adapt.t0_down % 2U) != 0U) {
                amba_ik_system_print_uint32_5("[IK] p_hi_asf_b1->adapt.t0_down = %d should be even", p_hi_asf_b1->adapt.t0_down, DC_U, DC_U, DC_U, DC_U);
                rval |= IK_ERR_0008;
            }

            rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "adapt->t0_up", p_hi_asf_b1->adapt.t0_up, 0U, 254U);
            if ((p_hi_asf_b1->adapt.t0_down % 2U) != 0U) {
                amba_ik_system_print_uint32_5("[IK] p_hi_asf_b1->adapt.t0_up = %d should be even", p_hi_asf_b1->adapt.t0_up, DC_U, DC_U, DC_U, DC_U);
                rval |= IK_ERR_0008;
            }

            rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "adapt->t1_down", p_hi_asf_b1->adapt.t1_down, 2U, 254U);
            if ((p_hi_asf_b1->adapt.t1_down % 2U) != 0U) {
                amba_ik_system_print_uint32_5("[IK] p_hi_asf_b1->adapt.t1_down = %d should be even", p_hi_asf_b1->adapt.t1_down, DC_U, DC_U, DC_U, DC_U);
                rval |= IK_ERR_0008;
            }

            rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "adapt->t1_up", p_hi_asf_b1->adapt.t1_up, 2U, 254U);
            if ((p_hi_asf_b1->adapt.t0_down % 2U) != 0U) {
                amba_ik_system_print_uint32_5("[IK] p_hi_asf_b1->adapt.t1_up = %d should be even", p_hi_asf_b1->adapt.t1_up, DC_U, DC_U, DC_U, DC_U);
                rval |= IK_ERR_0008;
            }

            if (p_hi_asf_b1->adapt.t0_up > p_hi_asf_b1->adapt.t1_up) {
                amba_ik_system_print_uint32_5("[IK] p_hi_asf_b1->adapt->T1_up=%d should larger than T0_up= %d in ctx_check_hi_asf_b1_valid_range", p_hi_asf_b1->adapt.t1_up, p_hi_asf_b1->adapt.t0_up, DC_U, DC_U, DC_U);
                rval |= IK_ERR_0008;
            }

            if (p_hi_asf_b1->adapt.t0_down > p_hi_asf_b1->adapt.t1_down) {
                amba_ik_system_print_uint32_5("[IK] p_hi_asf_b1->adapt->t1_down=%d should larger than t0_down= %d in ctx_check_hi_asf_b1_valid_range", p_hi_asf_b1->adapt.t1_down, p_hi_asf_b1->adapt.t0_down, DC_U, DC_U, DC_U);
                rval |= IK_ERR_0008;
            }

            rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "level_str_adjust->high", p_hi_asf_b1->level_str_adjust.high, 1UL, 254UL);
            rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "level_str_adjust->low", p_hi_asf_b1->level_str_adjust.low, 0UL, 253UL);
            rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "level_str_adjust->low_delta", p_hi_asf_b1->level_str_adjust.low_delta, 0UL, 7UL);
            rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "level_str_adjust->low_strength", p_hi_asf_b1->level_str_adjust.low_strength, 0, 64);
            rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "level_str_adjust->high_delta", p_hi_asf_b1->level_str_adjust.high_delta, 0UL, 7UL);
            rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "level_str_adjust->high_strength", p_hi_asf_b1->level_str_adjust.high_strength, 0, 64);
            rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "level_str_adjust->mid_strength", p_hi_asf_b1->level_str_adjust.mid_strength, 0, 64);

            rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "t0_t1_div->high", p_hi_asf_b1->t0_t1_div.high, 1UL, 254UL);
            rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "t0_t1_div->low", p_hi_asf_b1->t0_t1_div.low, 0UL, 253UL);
            rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "t0_t1_div->low_delta", p_hi_asf_b1->t0_t1_div.low_delta, 0UL, 7UL);
            rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "t0_t1_div->low_strength", p_hi_asf_b1->t0_t1_div.low_strength, 0, 64);
            rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "t0_t1_div->high_delta", p_hi_asf_b1->t0_t1_div.high_delta, 0UL, 7UL);
            rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "t0_t1_div->high_strength", p_hi_asf_b1->t0_t1_div.high_strength, 0, 64);
            rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "t0_t1_div->mid_strength", p_hi_asf_b1->t0_t1_div.mid_strength, 0, 64);

            rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "max_change_up", p_hi_asf_b1->max_change_up, 0, 255);
            rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "max_change_down", p_hi_asf_b1->max_change_down, 0, 255);

            {
                // advanced level control check
                ik_level_t level;

                level.low = p_hi_asf_b1->level_str_adjust.low;
                level.low_delta = p_hi_asf_b1->level_str_adjust.low_delta;
                level.high = p_hi_asf_b1->level_str_adjust.high;
                level.high_delta = p_hi_asf_b1->level_str_adjust.high_delta;
                rval |= check_advanced_level_control("hi_low_advanced_spatial_filter->level_str_adjust", &level);

                level.low = p_hi_asf_b1->t0_t1_div.low;
                level.low_delta = p_hi_asf_b1->t0_t1_div.low_delta;
                level.high = p_hi_asf_b1->t0_t1_div.high;
                level.high_delta = p_hi_asf_b1->t0_t1_div.high_delta;
                rval |= check_advanced_level_control("hi_low_advanced_spatial_filter->t0_t1_div", &level);
            }
        }
    }
    return rval;
}

static uint32 ctx_check_hi_asf_b2_valid_range(const ik_hi_asf_b2_t *p_hi_asf_b2, const char *prefix)
{
    uint32 rval = IK_OK, i, j;
    const ik_asf_fir_t *fir;
    if (p_hi_asf_b2 == NULL) {
        amba_ik_system_print_str_5("[IK] p_hi_asf_b2 input is NULL address!", NULL, NULL, NULL, NULL, NULL);
        rval |= IK_ERR_0005;
    } else {
        if (p_hi_asf_b2->enable!=0U) {
            rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "enable", p_hi_asf_b2->enable, 0, 1);
            fir = &p_hi_asf_b2->fir;
            rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "fir->specify", fir->specify, 2, 4);
            rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "fir->strength_iso", fir->strength_iso, 0, 256);
            rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "fir->strength_dir", fir->strength_dir, 0, 256);
            for (i = 0U; i < 9U; i++) {
                rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "fir->per_dir_fir_iso_strengths[]", fir->per_dir_fir_iso_strengths[i], 0, 256);
                rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "fir->per_dir_fir_dir_strengths[]", fir->per_dir_fir_dir_strengths[i], 0, 256);
                rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "fir->per_dir_fir_dir_amounts[]", fir->per_dir_fir_dir_amounts[i], 0, 256);
                for (j = 0U; j < 25U; j++) {
                    rval |= (uint32)ctx_check_int32_parameter_valid_range(prefix, "fir->coefs[]", fir->coefs[i][j], -1024, 1023);
                }
            }
            rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "directional_decide_t0", p_hi_asf_b2->directional_decide_t0, 0, 255);
            rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "directional_decide_t1", p_hi_asf_b2->directional_decide_t1, 0, 255);
            if (p_hi_asf_b2->directional_decide_t0 > p_hi_asf_b2->directional_decide_t1) {
                amba_ik_system_print_uint32_5("[IK] p_hi_asf_b2->directional_decide_t0 > p_hi_asf_b2->directional_decide_t1 is incorrect", DC_U, DC_U, DC_U, DC_U, DC_U);
                rval |= IK_ERR_0008;
            }
            rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "adapt->alpha_max_down", p_hi_asf_b2->adapt.alpha_max_down, 0, 8);
            rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "adapt->alpha_max_up", p_hi_asf_b2->adapt.alpha_max_up, 0, 8);
            rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "adapt->alpha_min_down", p_hi_asf_b2->adapt.alpha_min_down, 0, 8);
            rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "adapt->alpha_min_up", p_hi_asf_b2->adapt.alpha_min_up, 0, 8);
            rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "adapt->t0_down", p_hi_asf_b2->adapt.t0_down, 0, 252);
            if ((p_hi_asf_b2->adapt.t0_down % 2U) != 0U) {
                amba_ik_system_print_uint32_5("[IK] p_hi_asf_b2->adapt.t0_down = %d should be even", p_hi_asf_b2->adapt.t0_down, DC_U, DC_U, DC_U, DC_U);
                rval |= IK_ERR_0008;
            }

            rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "adapt->t0_up", p_hi_asf_b2->adapt.t0_up, 0U, 254U);
            if ((p_hi_asf_b2->adapt.t0_down % 2U) != 0U) {
                amba_ik_system_print_uint32_5("[IK] p_hi_asf_b2->adapt.t0_up = %d should be even", p_hi_asf_b2->adapt.t0_up, DC_U, DC_U, DC_U, DC_U);
                rval |= IK_ERR_0008;
            }

            rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "adapt->t1_down", p_hi_asf_b2->adapt.t1_down, 2U, 254U);
            if ((p_hi_asf_b2->adapt.t1_down % 2U) != 0U) {
                amba_ik_system_print_uint32_5("[IK] p_hi_asf_b2->adapt.t1_down = %d should be even", p_hi_asf_b2->adapt.t1_down, DC_U, DC_U, DC_U, DC_U);
                rval |= IK_ERR_0008;
            }

            rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "adapt->t1_up", p_hi_asf_b2->adapt.t1_up, 2U, 254U);
            if ((p_hi_asf_b2->adapt.t0_down % 2U) != 0U) {
                amba_ik_system_print_uint32_5("[IK] p_hi_asf_b2->adapt.t1_up = %d should be even", p_hi_asf_b2->adapt.t1_up, DC_U, DC_U, DC_U, DC_U);
                rval |= IK_ERR_0008;
            }

            if (p_hi_asf_b2->adapt.t0_up > p_hi_asf_b2->adapt.t1_up) {
                amba_ik_system_print_uint32_5("[IK] p_hi_asf_b2->adapt->T1_up=%d should larger than T0_up= %d in ctx_check_hi_asf_b2_valid_range", p_hi_asf_b2->adapt.t1_up, p_hi_asf_b2->adapt.t0_up, DC_U, DC_U, DC_U);
                rval |= IK_ERR_0008;
            }

            if (p_hi_asf_b2->adapt.t0_down > p_hi_asf_b2->adapt.t1_down) {
                amba_ik_system_print_uint32_5("[IK] p_hi_asf_b2->adapt->t1_down=%d should larger than t0_down= %d in ctx_check_hi_asf_b2_valid_range", p_hi_asf_b2->adapt.t1_down, p_hi_asf_b2->adapt.t0_down, DC_U, DC_U, DC_U);
                rval |= IK_ERR_0008;
            }

            rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "level_str_adjust->high", p_hi_asf_b2->level_str_adjust.high, 1UL, 254UL);
            rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "level_str_adjust->low", p_hi_asf_b2->level_str_adjust.low, 0UL, 253UL);
            rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "level_str_adjust->low_delta", p_hi_asf_b2->level_str_adjust.low_delta, 0UL, 7UL);
            rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "level_str_adjust->low_strength", p_hi_asf_b2->level_str_adjust.low_strength, 0, 64);
            rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "level_str_adjust->high_delta", p_hi_asf_b2->level_str_adjust.high_delta, 0UL, 7UL);
            rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "level_str_adjust->high_strength", p_hi_asf_b2->level_str_adjust.high_strength, 0, 64);
            rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "level_str_adjust->mid_strength", p_hi_asf_b2->level_str_adjust.mid_strength, 0, 64);
            if ((p_hi_asf_b2->level_str_adjust.method==0UL) || (p_hi_asf_b2->level_str_adjust.method==11UL)) {
                // pass
            } else {
                amba_ik_system_print_uint32_5("[IK] p_hi_asf_b2->level_str_adjust.method:%d is invalid", p_hi_asf_b2->level_str_adjust.method, DC_U, DC_U, DC_U, DC_U);
                rval |= IK_ERR_0008;
            }
            rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "t0_t1_div->high", p_hi_asf_b2->t0_t1_div.high, 1UL, 254UL);
            rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "t0_t1_div->low", p_hi_asf_b2->t0_t1_div.low, 0UL, 253UL);
            rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "t0_t1_div->low_delta", p_hi_asf_b2->t0_t1_div.low_delta, 0UL, 7UL);
            rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "t0_t1_div->low_strength", p_hi_asf_b2->t0_t1_div.low_strength, 0, 64);
            rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "t0_t1_div->high_delta", p_hi_asf_b2->t0_t1_div.high_delta, 0UL, 7UL);
            rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "t0_t1_div->high_strength", p_hi_asf_b2->t0_t1_div.high_strength, 0, 64);
            rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "t0_t1_div->mid_strength", p_hi_asf_b2->t0_t1_div.mid_strength, 0, 64);
            if ((p_hi_asf_b2->t0_t1_div.method==0UL) || (p_hi_asf_b2->t0_t1_div.method==11UL)) {
                // pass
            } else {
                amba_ik_system_print_uint32_5("[IK] p_hi_asf_b2->t0_t1_div.method:%d is invalid", p_hi_asf_b2->t0_t1_div.method, DC_U, DC_U, DC_U, DC_U);
                rval |= IK_ERR_0008;
            }

            rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "max_change_up", p_hi_asf_b2->max_change_up, 0, 255);
            rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "max_change_down", p_hi_asf_b2->max_change_down, 0, 255);
        }
    }
    return rval;
}

static uint32 ctx_check_chroma_asf_valid_range(const ik_chroma_asf_t *p_chroma_asf, const char *prefix)
{
    uint32 rval = IK_OK, i, j;
    const ik_asf_fir_t *fir;
    if (p_chroma_asf == NULL) {
        amba_ik_system_print_str_5("[IK] p_chroma_asf input is NULL address!", NULL, NULL, NULL, NULL, NULL);
        rval |= IK_ERR_0005;
    } else {
        if (p_chroma_asf->enable!=0U) {
            rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "enable", p_chroma_asf->enable, 0, 1);
            fir = &p_chroma_asf->fir;
            rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "fir->specify", fir->specify, 2, 4);
            rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "fir->strength_iso", fir->strength_iso, 0, 256);
            rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "fir->strength_dir", fir->strength_dir, 0, 256);
            for (i = 0U; i < 9U; i++) {
                rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "fir->per_dir_fir_iso_strengths[]", fir->per_dir_fir_iso_strengths[i], 0, 256);
                rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "fir->per_dir_fir_dir_strengths[]", fir->per_dir_fir_dir_strengths[i], 0, 256);
                rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "fir->per_dir_fir_dir_amounts[]", fir->per_dir_fir_dir_amounts[i], 0, 256);
            }
            for (i = 0U; i < 9U; i++) {
                for (j = 0U; j < 25U; j++) {
                    rval |= (uint32)ctx_check_int32_parameter_valid_range(prefix, "fir->coefs[]", fir->coefs[i][j], -1024, 1023);
                }
            }
            rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "directional_decide_t0", p_chroma_asf->directional_decide_t0, 0, 255);
            rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "directional_decide_t1", p_chroma_asf->directional_decide_t1, 0, 255);
            if (p_chroma_asf->directional_decide_t0 > p_chroma_asf->directional_decide_t1) {
                amba_ik_system_print_uint32_5("[IK] p_chroma_asf->directional_decide_t0 > p_chroma_asf->directional_decide_t1 is incorrect", DC_U, DC_U, DC_U, DC_U, DC_U);
                rval |= IK_ERR_0008;
            }
            rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "alpha_min", p_chroma_asf->alpha_min, 0, 8);
            rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "alpha_max", p_chroma_asf->alpha_max, 0, 8);
            rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "t0", p_chroma_asf->t0, 0, 252);
            rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "t1", p_chroma_asf->t1, 2, 254);
            if ((p_chroma_asf->t0>=p_chroma_asf->t1) || ((p_chroma_asf->t0%2UL)==1UL) || ((p_chroma_asf->t1%2UL)==1UL)) {
                amba_ik_system_print_uint32_5("[IK] p_chroma_asf->t0:%d, p_chroma_asf->t1:%d is invalid", p_chroma_asf->t0, p_chroma_asf->t1, DC_U, DC_U, DC_U);
                rval |= IK_ERR_0008;
            }
            rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "level_str_adjust->high", p_chroma_asf->level_str_adjust.high, 1UL, 254UL);
            rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "level_str_adjust->low", p_chroma_asf->level_str_adjust.low, 0UL, 253UL);
            rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "level_str_adjust->low_delta", p_chroma_asf->level_str_adjust.low_delta, 0UL, 7UL);
            rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "level_str_adjust->low_strength", p_chroma_asf->level_str_adjust.low_strength, 0, 64);
            rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "level_str_adjust->high_delta", p_chroma_asf->level_str_adjust.high_delta, 0UL, 7UL);
            rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "level_str_adjust->high_strength", p_chroma_asf->level_str_adjust.high_strength, 0, 64);
            rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "level_str_adjust->mid_strength", p_chroma_asf->level_str_adjust.mid_strength, 0, 64);

            rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "t0_t1_div->high", p_chroma_asf->t0_t1_div.high, 1UL, 254UL);
            rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "t0_t1_div->low", p_chroma_asf->t0_t1_div.low, 0UL, 253UL);
            rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "t0_t1_div->low_delta", p_chroma_asf->t0_t1_div.low_delta, 0UL, 7UL);
            rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "t0_t1_div->low_strength", p_chroma_asf->t0_t1_div.low_strength, 0, 64);
            rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "t0_t1_div->high_delta", p_chroma_asf->t0_t1_div.high_delta, 0UL, 7UL);
            rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "t0_t1_div->high_strength", p_chroma_asf->t0_t1_div.high_strength, 0, 64);
            rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "t0_t1_div->mid_strength", p_chroma_asf->t0_t1_div.mid_strength, 0, 64);

            rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "max_change", p_chroma_asf->max_change, 0, 255);

            {
                // advanced level control check
                ik_level_t level;

                level.low = p_chroma_asf->level_str_adjust.low;
                level.low_delta = p_chroma_asf->level_str_adjust.low_delta;
                level.high = p_chroma_asf->level_str_adjust.high;
                level.high_delta = p_chroma_asf->level_str_adjust.high_delta;
                rval |= check_advanced_level_control("chroma_advanced_spatial_filter->level_str_adjust", &level);

                level.low = p_chroma_asf->t0_t1_div.low;
                level.low_delta = p_chroma_asf->t0_t1_div.low_delta;
                level.high = p_chroma_asf->t0_t1_div.high;
                level.high_delta = p_chroma_asf->t0_t1_div.high_delta;
                rval |= check_advanced_level_control("chroma_advanced_spatial_filter->t0_t1_div", &level);
            }
        }
    }
    return rval;
}

//SHP
static uint32 ctx_check_hi_high_shpns_noise_valid_range(const ik_hi_high_shpns_noise_t *p_hi_high_shpns_noise, const char *prefix)
{
    uint32 rval = IK_OK;
    uint32  i = 0, j = 0;
    uint32 tmp;
    if (p_hi_high_shpns_noise == NULL) {
        amba_ik_system_print_str_5("[IK] detect p_hi_high_shpns_noise get NULL pointer!", NULL, NULL, NULL, NULL, NULL);
        rval |= IK_ERR_0005;
    } else {
        rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "max_change_up", p_hi_high_shpns_noise->max_change_up, 0U, 255U);
        rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "max_change_down", p_hi_high_shpns_noise->max_change_down, 0U, 255U);
        rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "spatial_fir.specify", p_hi_high_shpns_noise->spatial_fir.specify, 0U, 4U);
        if (p_hi_high_shpns_noise->spatial_fir.specify == 0U) {
            rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "spatial_fir.strength_iso", p_hi_high_shpns_noise->spatial_fir.strength_iso, 0U, 256U);
        } else if (p_hi_high_shpns_noise->spatial_fir.specify == 1U) {
            for (i = 0U; i < 25U; i++) {
                rval |= (uint32)ctx_check_int32_parameter_valid_range(prefix, "spatial_fir.coefs", p_hi_high_shpns_noise->spatial_fir.coefs[0][i], -1024, 1023);
            }
        } else if (p_hi_high_shpns_noise->spatial_fir.specify == 2U) {
            rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "spatial_fir.strength_iso", p_hi_high_shpns_noise->spatial_fir.strength_iso, 0U, 256U);
            rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "spatial_fir.strength_dir", p_hi_high_shpns_noise->spatial_fir.strength_dir, 0U, 256U);
        } else if (p_hi_high_shpns_noise->spatial_fir.specify == 3U) {
            for (i = 0U; i < 9U; i++) {
                rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "spatial_fir.per_dir_fir_dir_amounts", p_hi_high_shpns_noise->spatial_fir.per_dir_fir_dir_amounts[i], 0U, 256U);
                rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "spatial_fir.per_dir_fir_dir_strengths", p_hi_high_shpns_noise->spatial_fir.per_dir_fir_dir_strengths[i], 0U, 256U);
                rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "spatial_fir.per_dir_fir_iso_strengths", p_hi_high_shpns_noise->spatial_fir.per_dir_fir_iso_strengths[i], 0U, 256U);
            }
        } else if (p_hi_high_shpns_noise->spatial_fir.specify == 4U) {
            for (j = 0U; j < 9U; j++) {
                for (i = 0U; i < 25U; i++) {
                    rval |= (uint32)ctx_check_int32_parameter_valid_range(prefix, "spatial_fir.coefs", p_hi_high_shpns_noise->spatial_fir.coefs[j][i], -1024, 1023);
                }
            }
        } else {
            rval |= IK_ERR_0008;
        }
        rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "level_str_adjust.low", p_hi_high_shpns_noise->level_str_adjust.low, 0U, 253U);
        rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "level_str_adjust.low_delta", p_hi_high_shpns_noise->level_str_adjust.low_delta, 0U, 7U);
        rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "level_str_adjust.high", p_hi_high_shpns_noise->level_str_adjust.high, 1, 254U);
        rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "level_str_adjust.high_delta", p_hi_high_shpns_noise->level_str_adjust.high_delta, 0U, 7U);
        rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "level_str_adjust_not_t0_t1_level_based", p_hi_high_shpns_noise->level_str_adjust_not_t0_t1_level_based, 0U, 1U);
        rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "t0", p_hi_high_shpns_noise->t0, 0U, 255U);
        if (p_hi_high_shpns_noise->t0 > p_hi_high_shpns_noise->t1) {
            amba_ik_system_print_uint32_5("[IK] t0 = %d  should not larger than t1 = %d\n", p_hi_high_shpns_noise->t0, p_hi_high_shpns_noise->t1, DC_U, DC_U, DC_U);
            rval |= IK_ERR_0008;
        }
        tmp = p_hi_high_shpns_noise->t1 - p_hi_high_shpns_noise->t0;
        if (tmp > 15U) {
            amba_ik_system_print_uint32_5("[IK] t1 = %d should be larger then t0 = %d at least 15\n", p_hi_high_shpns_noise->t0, p_hi_high_shpns_noise->t1, DC_U, DC_U, DC_U);
            rval |= IK_ERR_0008;
        }
        rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "t1", p_hi_high_shpns_noise->t1, 0U, 255U);
        rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "alpha_min", p_hi_high_shpns_noise->alpha_min, 0U, 255U);
        rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "alpha_max", p_hi_high_shpns_noise->alpha_max, 0U, 255U);
        if ((p_hi_high_shpns_noise->level_str_adjust.method != 0U) &&
            (p_hi_high_shpns_noise->level_str_adjust.method != 1U) &&
            (p_hi_high_shpns_noise->level_str_adjust.method != 2U) &&
            (p_hi_high_shpns_noise->level_str_adjust.method != 6U) &&
            (p_hi_high_shpns_noise->level_str_adjust.method != 7U) &&
            (p_hi_high_shpns_noise->level_str_adjust.method != 12U) &&
            (p_hi_high_shpns_noise->level_str_adjust.method != 14U)) {
            amba_ik_system_print_uint32_5("[IK] p_hi_high_shpns_noise->level_str_adjust.method %d , should be 0,1,6,7", p_hi_high_shpns_noise->level_str_adjust.method, DC_U, DC_U, DC_U, DC_U);
            rval |= IK_ERR_0008;
        }
    }
    return rval;
}

static uint32 ctx_check_hi_shpns_method_level_control_valid_range(const ik_level_method_t *p_level_method, const char *prefix)
{
    uint32 rval = IK_OK;
    if (p_level_method == NULL) {
        amba_ik_system_print_str_5("[IK] detect final_sharpen_coring_idx_scale get NULL pointer!", NULL, NULL, NULL, NULL, NULL);
        rval |= IK_ERR_0005;
    } else {
        rval |= ctx_check_uint32_parameter_valid_range(prefix, "low", p_level_method->low, 0UL, 253UL);
        rval |= ctx_check_uint32_parameter_valid_range(prefix, "low_delta", p_level_method->low_delta, 0UL, 7UL);
        rval |= ctx_check_uint32_parameter_valid_range(prefix, "low_delta", p_level_method->low_strength, 0UL, 255UL);
        rval |= ctx_check_uint32_parameter_valid_range(prefix, "high_delta", p_level_method->mid_strength, 0UL, 255UL);
        rval |= ctx_check_uint32_parameter_valid_range(prefix, "high", p_level_method->high, 1, 254);
        rval |= ctx_check_uint32_parameter_valid_range(prefix, "high_delta", p_level_method->high_delta, 0UL, 7UL);
        rval |= ctx_check_uint32_parameter_valid_range(prefix, "high_delta", p_level_method->high_strength, 0UL, 255UL);
        if ((p_level_method->method != 0U) &&
            (p_level_method->method != 11U)) {
            amba_ik_system_print_uint32_5("[IK] p_level_method->method %d , should be 0,11", p_level_method->method, DC_U, DC_U, DC_U, DC_U);
            rval |= IK_ERR_0008;
        }
    }
    return rval;
}

//li2
static uint32 ctx_check_hi_shpns_both_b_valid_range(const ik_hi_shpns_both_b_t *p_hi_shpns_both_b, const char *prefix)
{
    uint32 rval = IK_OK;
    if (p_hi_shpns_both_b == NULL) {
        amba_ik_system_print_str_5("[IK] detect p_hi_shpns_both_b get NULL pointer!", NULL, NULL, NULL, NULL, NULL);
        rval |= IK_ERR_0005;
    } else {
        rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "enable", p_hi_shpns_both_b->enable, 0, 1);
        rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "mode", p_hi_shpns_both_b->mode, 0, 2);
        rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "edge_thresh", p_hi_shpns_both_b->edge_thresh, 0, 2047);
        rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "wide_edge_detect", p_hi_shpns_both_b->wide_edge_detect, 0, 8);

        rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "max_change.up5x5", p_hi_shpns_both_b->max_change_up5x5, 0, 255);
        rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "max_change.down5x5", p_hi_shpns_both_b->max_change_down5x5, 0, 255);

        rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "max_change.up", p_hi_shpns_both_b->max_change_up, 0, 255);
        rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "max_change.down", p_hi_shpns_both_b->max_change_down, 0, 255);
    }
    return rval;
}

static uint32 ctx_check_hi_shpns_coring_b_valid_range(const ik_hi_shpns_coring_b_t *p_hi_shpns_coring_b, const char *prefix)
{
    uint32 rval = IK_OK;
    uint32 i = 0;
    if (p_hi_shpns_coring_b == NULL) {
        amba_ik_system_print_str_5("[IK] detect p_hi_shpns_coring_b get NULL pointer!", NULL, NULL, NULL, NULL, NULL);
        rval |= IK_ERR_0005;
    } else {
        for (i = 0U; i < 256U; i++) {
            rval |= ctx_check_uint32_parameter_valid_range(prefix, "coring", p_hi_shpns_coring_b->coring[i], 0, 31);
        }
    }
    return rval;
}

static uint32 ctx_check_hi_chroma_filter_high_valid_range(const ik_chroma_filter_t *chroma_filter, const char *prefix)
{
    uint32 rval = IK_OK;
    if (chroma_filter == NULL) {
        rval |= IK_ERR_0005;
    } else {
        rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "enable", chroma_filter->enable, 0, 1);
        rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "noise_level_cb", chroma_filter->noise_level_cb, 0, 255);
        rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "noise_level_cr", chroma_filter->noise_level_cr, 0, 255);
        rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "original_blend_strength_cb", chroma_filter->original_blend_strength_cb, 0, 256);
        rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "original_blend_strength_cr", chroma_filter->original_blend_strength_cr, 0, 256);

        if ((chroma_filter->radius != 32U) && (chroma_filter->radius != 64U) && (chroma_filter->radius != 82U)) {
            amba_ik_system_print_uint32_5("[IK] chroma_filter->radius should be [32, 64, 128], %d is invalid", chroma_filter->radius, DC_U, DC_U, DC_U, DC_U);
            rval |= IK_ERR_0008;
        }
    }

    return rval;
}

static uint32 ctx_check_hi_chroma_filter_b_valid_range(const ik_hi_chroma_filter_b_t *p_hi_chroma_filter_b, const char *prefix)
{
    uint32 rval = IK_OK;
    if (p_hi_chroma_filter_b == NULL) {
        rval |= IK_ERR_0005;
    } else {
        rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "enable", p_hi_chroma_filter_b->enable, 0, 1);
        rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "noise_level_cb", p_hi_chroma_filter_b->noise_level_cb, 0, 255);
        rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "noise_level_cr", p_hi_chroma_filter_b->noise_level_cr, 0, 255);
        rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "original_blend_strength_cb", p_hi_chroma_filter_b->original_blend_strength_cb, 0, 256);
        rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "original_blend_strength_cr", p_hi_chroma_filter_b->original_blend_strength_cr, 0, 256);
    }

    return rval;
}

//combine
static uint32 ctx_check_hi_luma_combine_valid_range(const ik_hi_luma_combine_t *p_hi_luma_combine, const char *prefix)
{
    uint32 rval = IK_OK;
    if (p_hi_luma_combine == NULL) {
        rval |= IK_ERR_0005;
    } else {
        rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "t0", p_hi_luma_combine->t0, 0, 62);
        rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "t1", p_hi_luma_combine->t1, 1, 63);
        rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "alpha_max", p_hi_luma_combine->alpha_max, 0, 255);
        rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "alpha_min", p_hi_luma_combine->alpha_min, 0, 255);
        rval |= (uint32)ctx_check_uint32_parameter_valid_range(prefix, "signal_preserve", p_hi_luma_combine->signal_preserve, 0, 1);

        rval |= ctx_check_uint32_parameter_valid_range(prefix, "t0_t1_add.low", p_hi_luma_combine->t0_t1_add.low, 0UL, 253UL);
        rval |= ctx_check_uint32_parameter_valid_range(prefix, "t0_t1_add.low_delta", p_hi_luma_combine->t0_t1_add.low_delta, 0UL, 7UL);
        rval |= ctx_check_uint32_parameter_valid_range(prefix, "t0_t1_add.low_delta", p_hi_luma_combine->t0_t1_add.low_strength, 0UL, 255UL);
        rval |= ctx_check_uint32_parameter_valid_range(prefix, "t0_t1_add.high_delta", p_hi_luma_combine->t0_t1_add.mid_strength, 0UL, 255UL);
        rval |= ctx_check_uint32_parameter_valid_range(prefix, "t0_t1_add.high", p_hi_luma_combine->t0_t1_add.high, 1, 254);
        rval |= ctx_check_uint32_parameter_valid_range(prefix, "t0_t1_add.high_delta", p_hi_luma_combine->t0_t1_add.high_delta, 0UL, 7UL);
        rval |= ctx_check_uint32_parameter_valid_range(prefix, "t0_t1_add.high_delta", p_hi_luma_combine->t0_t1_add.high_strength, 0UL, 255UL);
        if ((p_hi_luma_combine->t0_t1_add.method != 0U) &&
            (p_hi_luma_combine->t0_t1_add.method != 11U)) {
            amba_ik_system_print_uint32_5("[IK] p_hi_luma_combine->t0_t1_add.method %d , should be 0,11", p_hi_luma_combine->t0_t1_add.method, DC_U, DC_U, DC_U, DC_U);
            rval |= IK_ERR_0008;
        }

        rval |= ctx_check_uint32_parameter_valid_range(prefix, "max_change.low", p_hi_luma_combine->max_change.low, 0UL, 253UL);
        rval |= ctx_check_uint32_parameter_valid_range(prefix, "max_change.low_delta", p_hi_luma_combine->max_change.low_delta, 0UL, 7UL);
        rval |= ctx_check_uint32_parameter_valid_range(prefix, "max_change.low_delta", p_hi_luma_combine->max_change.low_strength, 0UL, 255UL);
        rval |= ctx_check_uint32_parameter_valid_range(prefix, "max_change.high_delta", p_hi_luma_combine->max_change.mid_strength, 0UL, 255UL);
        rval |= ctx_check_uint32_parameter_valid_range(prefix, "max_change.high", p_hi_luma_combine->max_change.high, 1, 254);
        rval |= ctx_check_uint32_parameter_valid_range(prefix, "max_change.high_delta", p_hi_luma_combine->max_change.high_delta, 0UL, 7UL);
        rval |= ctx_check_uint32_parameter_valid_range(prefix, "max_change.high_delta", p_hi_luma_combine->max_change.high_strength, 0UL, 255UL);
        if ((p_hi_luma_combine->max_change.method != 0U) &&
            (p_hi_luma_combine->max_change.method != 11U)) {
            amba_ik_system_print_uint32_5("[IK] p_hi_luma_combine->max_change.method %d , should be 0,11", p_hi_luma_combine->max_change.method, DC_U, DC_U, DC_U, DC_U);
            rval |= IK_ERR_0008;
        }
    }

    return rval;
}

static uint32 ctx_check_hi_chroma_filter_combine_valid_range(const ik_hi_chroma_filter_combine_t *p_hi_chroma_filter_combine, const char *prefix)
{
    uint32 rval = IK_OK;
    if (p_hi_chroma_filter_combine == NULL) {
        amba_ik_system_print_str_5("[IK] detect final_sharpen_coring_idx_scale get NULL pointer!", NULL, NULL, NULL, NULL, NULL);
        rval |= IK_ERR_0005;
    } else {
        rval |= ctx_check_uint32_parameter_valid_range(prefix, "t0_cb", p_hi_chroma_filter_combine->t0_cb, 0, 62);
        rval |= ctx_check_uint32_parameter_valid_range(prefix, "t1_cb", p_hi_chroma_filter_combine->t1_cb, 1, 63);
        rval |= ctx_check_uint32_parameter_valid_range(prefix, "alpha_max_cb", p_hi_chroma_filter_combine->alpha_max_cb, 0, 255);
        rval |= ctx_check_uint32_parameter_valid_range(prefix, "alpha_min_cb", p_hi_chroma_filter_combine->alpha_min_cb, 0, 255);
        rval |= ctx_check_uint32_parameter_valid_range(prefix, "signal_preserve_cb", p_hi_chroma_filter_combine->signal_preserve_cb, 0, 1);

        rval |= ctx_check_uint32_parameter_valid_range(prefix, "max_change_cb.low", p_hi_chroma_filter_combine->max_change_cb.low, 0UL, 253UL);
        rval |= ctx_check_uint32_parameter_valid_range(prefix, "max_change_cb.low_delta", p_hi_chroma_filter_combine->max_change_cb.low_delta, 0UL, 7UL);
        rval |= ctx_check_uint32_parameter_valid_range(prefix, "max_change_cb.low_delta", p_hi_chroma_filter_combine->max_change_cb.low_strength, 0UL, 255UL);
        rval |= ctx_check_uint32_parameter_valid_range(prefix, "max_change_cb.high_delta", p_hi_chroma_filter_combine->max_change_cb.mid_strength, 0UL, 255UL);
        rval |= ctx_check_uint32_parameter_valid_range(prefix, "max_change_cb.high", p_hi_chroma_filter_combine->max_change_cb.high, 1, 254);
        rval |= ctx_check_uint32_parameter_valid_range(prefix, "max_change_cb.high_delta", p_hi_chroma_filter_combine->max_change_cb.high_delta, 0UL, 7UL);
        rval |= ctx_check_uint32_parameter_valid_range(prefix, "max_change_cb.high_delta", p_hi_chroma_filter_combine->max_change_cb.high_strength, 0UL, 255UL);

        rval |= ctx_check_uint32_parameter_valid_range(prefix, "to_t1_add_cb.low", p_hi_chroma_filter_combine->to_t1_add_cb.low, 0UL, 253UL);
        rval |= ctx_check_uint32_parameter_valid_range(prefix, "to_t1_add_cb.low_delta", p_hi_chroma_filter_combine->to_t1_add_cb.low_delta, 0UL, 7UL);
        rval |= ctx_check_uint32_parameter_valid_range(prefix, "to_t1_add_cb.low_delta", p_hi_chroma_filter_combine->to_t1_add_cb.low_strength, 0UL, 255UL);
        rval |= ctx_check_uint32_parameter_valid_range(prefix, "to_t1_add_cb.high_delta", p_hi_chroma_filter_combine->to_t1_add_cb.mid_strength, 0UL, 255UL);
        rval |= ctx_check_uint32_parameter_valid_range(prefix, "to_t1_add_cb.high", p_hi_chroma_filter_combine->to_t1_add_cb.high, 1, 254);
        rval |= ctx_check_uint32_parameter_valid_range(prefix, "to_t1_add_cb.high_delta", p_hi_chroma_filter_combine->to_t1_add_cb.high_delta, 0UL, 7UL);
        rval |= ctx_check_uint32_parameter_valid_range(prefix, "to_t1_add_cb.high_delta", p_hi_chroma_filter_combine->to_t1_add_cb.high_strength, 0UL, 255UL);

        rval |= ctx_check_uint32_parameter_valid_range(prefix, "t0_cr", p_hi_chroma_filter_combine->t0_cr, 0, 62);
        rval |= ctx_check_uint32_parameter_valid_range(prefix, "t1_cr", p_hi_chroma_filter_combine->t1_cr, 1, 63);
        rval |= ctx_check_uint32_parameter_valid_range(prefix, "alpha_max_cr", p_hi_chroma_filter_combine->alpha_max_cr, 0, 255);
        rval |= ctx_check_uint32_parameter_valid_range(prefix, "alpha_min_cr", p_hi_chroma_filter_combine->alpha_min_cr, 0, 255);
        rval |= ctx_check_uint32_parameter_valid_range(prefix, "signal_preserve_cr", p_hi_chroma_filter_combine->signal_preserve_cr, 0, 1);

        rval |= ctx_check_uint32_parameter_valid_range(prefix, "max_change_cr.low", p_hi_chroma_filter_combine->max_change_cr.low, 0UL, 253UL);
        rval |= ctx_check_uint32_parameter_valid_range(prefix, "max_change_cr.low_delta", p_hi_chroma_filter_combine->max_change_cr.low_delta, 0UL, 7UL);
        rval |= ctx_check_uint32_parameter_valid_range(prefix, "max_change_cr.low_delta", p_hi_chroma_filter_combine->max_change_cr.low_strength, 0UL, 255UL);
        rval |= ctx_check_uint32_parameter_valid_range(prefix, "max_change_cr.high_delta", p_hi_chroma_filter_combine->max_change_cr.mid_strength, 0UL, 255UL);
        rval |= ctx_check_uint32_parameter_valid_range(prefix, "max_change_cr.high", p_hi_chroma_filter_combine->max_change_cr.high, 1, 254);
        rval |= ctx_check_uint32_parameter_valid_range(prefix, "max_change_cr.high_delta", p_hi_chroma_filter_combine->max_change_cr.high_delta, 0UL, 7UL);
        rval |= ctx_check_uint32_parameter_valid_range(prefix, "max_change_cr.high_delta", p_hi_chroma_filter_combine->max_change_cr.high_strength, 0UL, 255UL);

        rval |= ctx_check_uint32_parameter_valid_range(prefix, "to_t1_add_cr.low", p_hi_chroma_filter_combine->to_t1_add_cr.low, 0UL, 253UL);
        rval |= ctx_check_uint32_parameter_valid_range(prefix, "to_t1_add_cr.low_delta", p_hi_chroma_filter_combine->to_t1_add_cr.low_delta, 0UL, 7UL);
        rval |= ctx_check_uint32_parameter_valid_range(prefix, "to_t1_add_cr.low_delta", p_hi_chroma_filter_combine->to_t1_add_cr.low_strength, 0UL, 255UL);
        rval |= ctx_check_uint32_parameter_valid_range(prefix, "to_t1_add_cr.high_delta", p_hi_chroma_filter_combine->to_t1_add_cr.mid_strength, 0UL, 255UL);
        rval |= ctx_check_uint32_parameter_valid_range(prefix, "to_t1_add_cr.high", p_hi_chroma_filter_combine->to_t1_add_cr.high, 1, 254);
        rval |= ctx_check_uint32_parameter_valid_range(prefix, "to_t1_add_cr.high_delta", p_hi_chroma_filter_combine->to_t1_add_cr.high_delta, 0UL, 7UL);
        rval |= ctx_check_uint32_parameter_valid_range(prefix, "to_t1_add_cr.high_delta", p_hi_chroma_filter_combine->to_t1_add_cr.high_strength, 0UL, 255UL);

        {
            // advanced level control check
            ik_level_t level;

            level.low = p_hi_chroma_filter_combine->max_change_cb.low;
            level.low_delta = p_hi_chroma_filter_combine->max_change_cb.low_delta;
            level.high = p_hi_chroma_filter_combine->max_change_cb.high;
            level.high_delta = p_hi_chroma_filter_combine->max_change_cb.high_delta;
            rval |= check_advanced_level_control("hi_chroma_filter_med_combine->max_change_cb", &level);

            level.low = p_hi_chroma_filter_combine->max_change_cr.low;
            level.low_delta = p_hi_chroma_filter_combine->max_change_cr.low_delta;
            level.high = p_hi_chroma_filter_combine->max_change_cr.high;
            level.high_delta = p_hi_chroma_filter_combine->max_change_cr.high_delta;
            rval |= check_advanced_level_control("hi_chroma_filter_med_combine->max_change_cr", &level);

            level.low = p_hi_chroma_filter_combine->to_t1_add_cb.low;
            level.low_delta = p_hi_chroma_filter_combine->to_t1_add_cb.low_delta;
            level.high = p_hi_chroma_filter_combine->to_t1_add_cb.high;
            level.high_delta = p_hi_chroma_filter_combine->to_t1_add_cb.high_delta;
            rval |= check_advanced_level_control("hi_chroma_filter_med_combine->to_t1_add_cb", &level);

            level.low = p_hi_chroma_filter_combine->to_t1_add_cr.low;
            level.low_delta = p_hi_chroma_filter_combine->to_t1_add_cr.low_delta;
            level.high = p_hi_chroma_filter_combine->to_t1_add_cr.high;
            level.high_delta = p_hi_chroma_filter_combine->to_t1_add_cr.high_delta;
            rval |= check_advanced_level_control("hi_chroma_filter_med_combine->to_t1_add_cr", &level);
        }
    }
    return rval;
}

static uint32 ctx_check_hili_combine_valid_range(const ik_hili_combine_t *p_hili_combine, const char *prefix)
{
    uint32 rval = IK_OK;
    if (p_hili_combine == NULL) {
        amba_ik_system_print_str_5("[IK] detect p_hili_combine get NULL pointer!", NULL, NULL, NULL, NULL, NULL);
        rval |= IK_ERR_0005;
    } else {
        rval |= ctx_check_uint32_parameter_valid_range(prefix, "t0_y", p_hili_combine->t0_y, 0, 62);
        rval |= ctx_check_uint32_parameter_valid_range(prefix, "t1_y", p_hili_combine->t1_y, 1, 63);
        rval |= ctx_check_uint32_parameter_valid_range(prefix, "alpha_max_y", p_hili_combine->alpha_max_y, 0, 255);
        rval |= ctx_check_uint32_parameter_valid_range(prefix, "alpha_min_y", p_hili_combine->alpha_min_y, 0, 255);
        rval |= ctx_check_uint32_parameter_valid_range(prefix, "signal_preserve_y", p_hili_combine->signal_preserve_y, 0, 1);

        rval |= ctx_check_uint32_parameter_valid_range(prefix, "max_change_y.low", p_hili_combine->max_change_y.low, 0UL, 253UL);
        rval |= ctx_check_uint32_parameter_valid_range(prefix, "max_change_y.low_delta", p_hili_combine->max_change_y.low_delta, 0UL, 7UL);
        rval |= ctx_check_uint32_parameter_valid_range(prefix, "max_change_y.low_delta", p_hili_combine->max_change_y.low_strength, 0UL, 255UL);
        rval |= ctx_check_uint32_parameter_valid_range(prefix, "max_change_y.high_delta", p_hili_combine->max_change_y.mid_strength, 0UL, 255UL);
        rval |= ctx_check_uint32_parameter_valid_range(prefix, "max_change_y.high", p_hili_combine->max_change_y.high, 1, 254);
        rval |= ctx_check_uint32_parameter_valid_range(prefix, "max_change_y.high_delta", p_hili_combine->max_change_y.high_delta, 0UL, 7UL);
        rval |= ctx_check_uint32_parameter_valid_range(prefix, "max_change_y.high_delta", p_hili_combine->max_change_y.high_strength, 0UL, 255UL);

        rval |= ctx_check_uint32_parameter_valid_range(prefix, "to_t1_add_y.low", p_hili_combine->to_t1_add_y.low, 0UL, 253UL);
        rval |= ctx_check_uint32_parameter_valid_range(prefix, "to_t1_add_y.low_delta", p_hili_combine->to_t1_add_y.low_delta, 0UL, 7UL);
        rval |= ctx_check_uint32_parameter_valid_range(prefix, "to_t1_add_y.low_delta", p_hili_combine->to_t1_add_y.low_strength, 0UL, 255UL);
        rval |= ctx_check_uint32_parameter_valid_range(prefix, "to_t1_add_y.high_delta", p_hili_combine->to_t1_add_y.mid_strength, 0UL, 255UL);
        rval |= ctx_check_uint32_parameter_valid_range(prefix, "to_t1_add_y.high", p_hili_combine->to_t1_add_y.high, 1, 254);
        rval |= ctx_check_uint32_parameter_valid_range(prefix, "to_t1_add_y.high_delta", p_hili_combine->to_t1_add_y.high_delta, 0UL, 7UL);
        rval |= ctx_check_uint32_parameter_valid_range(prefix, "to_t1_add_y.high_delta", p_hili_combine->to_t1_add_y.high_strength, 0UL, 255UL);

        rval |= ctx_check_uint32_parameter_valid_range(prefix, "t0_cb", p_hili_combine->t0_cb, 0, 62);
        rval |= ctx_check_uint32_parameter_valid_range(prefix, "t1_cb", p_hili_combine->t1_cb, 1, 63);
        rval |= ctx_check_uint32_parameter_valid_range(prefix, "alpha_max_cb", p_hili_combine->alpha_max_cb, 0, 255);
        rval |= ctx_check_uint32_parameter_valid_range(prefix, "alpha_min_cb", p_hili_combine->alpha_min_cb, 0, 255);
        rval |= ctx_check_uint32_parameter_valid_range(prefix, "signal_preserve_cb", p_hili_combine->signal_preserve_cb, 0, 1);

        rval |= ctx_check_uint32_parameter_valid_range(prefix, "max_change_cb.low", p_hili_combine->max_change_cb.low, 0UL, 253UL);
        rval |= ctx_check_uint32_parameter_valid_range(prefix, "max_change_cb.low_delta", p_hili_combine->max_change_cb.low_delta, 0UL, 7UL);
        rval |= ctx_check_uint32_parameter_valid_range(prefix, "max_change_cb.low_delta", p_hili_combine->max_change_cb.low_strength, 0UL, 255UL);
        rval |= ctx_check_uint32_parameter_valid_range(prefix, "max_change_cb.high_delta", p_hili_combine->max_change_cb.mid_strength, 0UL, 255UL);
        rval |= ctx_check_uint32_parameter_valid_range(prefix, "max_change_cb.high", p_hili_combine->max_change_cb.high, 1, 254);
        rval |= ctx_check_uint32_parameter_valid_range(prefix, "max_change_cb.high_delta", p_hili_combine->max_change_cb.high_delta, 0UL, 7UL);
        rval |= ctx_check_uint32_parameter_valid_range(prefix, "max_change_cb.high_delta", p_hili_combine->max_change_cb.high_strength, 0UL, 255UL);

        rval |= ctx_check_uint32_parameter_valid_range(prefix, "to_t1_add_cb.low", p_hili_combine->to_t1_add_cb.low, 0UL, 253UL);
        rval |= ctx_check_uint32_parameter_valid_range(prefix, "to_t1_add_cb.low_delta", p_hili_combine->to_t1_add_cb.low_delta, 0UL, 7UL);
        rval |= ctx_check_uint32_parameter_valid_range(prefix, "to_t1_add_cb.low_delta", p_hili_combine->to_t1_add_cb.low_strength, 0UL, 255UL);
        rval |= ctx_check_uint32_parameter_valid_range(prefix, "to_t1_add_cb.high_delta", p_hili_combine->to_t1_add_cb.mid_strength, 0UL, 255UL);
        rval |= ctx_check_uint32_parameter_valid_range(prefix, "to_t1_add_cb.high", p_hili_combine->to_t1_add_cb.high, 1, 254);
        rval |= ctx_check_uint32_parameter_valid_range(prefix, "to_t1_add_cb.high_delta", p_hili_combine->to_t1_add_cb.high_delta, 0UL, 7UL);
        rval |= ctx_check_uint32_parameter_valid_range(prefix, "to_t1_add_cb.high_delta", p_hili_combine->to_t1_add_cb.high_strength, 0UL, 255UL);

        rval |= ctx_check_uint32_parameter_valid_range(prefix, "t0_cr", p_hili_combine->t0_cr, 0, 62);
        rval |= ctx_check_uint32_parameter_valid_range(prefix, "t1_cr", p_hili_combine->t1_cr, 1, 63);
        rval |= ctx_check_uint32_parameter_valid_range(prefix, "alpha_max_cr", p_hili_combine->alpha_max_cr, 0, 255);
        rval |= ctx_check_uint32_parameter_valid_range(prefix, "alpha_min_cr", p_hili_combine->alpha_min_cr, 0, 255);
        rval |= ctx_check_uint32_parameter_valid_range(prefix, "signal_preserve_cr", p_hili_combine->signal_preserve_cr, 0, 1);

        rval |= ctx_check_uint32_parameter_valid_range(prefix, "max_change_cr.low", p_hili_combine->max_change_cr.low, 0UL, 253UL);
        rval |= ctx_check_uint32_parameter_valid_range(prefix, "max_change_cr.low_delta", p_hili_combine->max_change_cr.low_delta, 0UL, 7UL);
        rval |= ctx_check_uint32_parameter_valid_range(prefix, "max_change_cr.low_delta", p_hili_combine->max_change_cr.low_strength, 0UL, 255UL);
        rval |= ctx_check_uint32_parameter_valid_range(prefix, "max_change_cr.high_delta", p_hili_combine->max_change_cr.mid_strength, 0UL, 255UL);
        rval |= ctx_check_uint32_parameter_valid_range(prefix, "max_change_cr.high", p_hili_combine->max_change_cr.high, 1, 254);
        rval |= ctx_check_uint32_parameter_valid_range(prefix, "max_change_cr.high_delta", p_hili_combine->max_change_cr.high_delta, 0UL, 7UL);
        rval |= ctx_check_uint32_parameter_valid_range(prefix, "max_change_cr.high_delta", p_hili_combine->max_change_cr.high_strength, 0UL, 255UL);

        rval |= ctx_check_uint32_parameter_valid_range(prefix, "to_t1_add_cr.low", p_hili_combine->to_t1_add_cr.low, 0UL, 253UL);
        rval |= ctx_check_uint32_parameter_valid_range(prefix, "to_t1_add_cr.low_delta", p_hili_combine->to_t1_add_cr.low_delta, 0UL, 7UL);
        rval |= ctx_check_uint32_parameter_valid_range(prefix, "to_t1_add_cr.low_delta", p_hili_combine->to_t1_add_cr.low_strength, 0UL, 255UL);
        rval |= ctx_check_uint32_parameter_valid_range(prefix, "to_t1_add_cr.high_delta", p_hili_combine->to_t1_add_cr.mid_strength, 0UL, 255UL);
        rval |= ctx_check_uint32_parameter_valid_range(prefix, "to_t1_add_cr.high", p_hili_combine->to_t1_add_cr.high, 1, 254);
        rval |= ctx_check_uint32_parameter_valid_range(prefix, "to_t1_add_cr.high_delta", p_hili_combine->to_t1_add_cr.high_delta, 0UL, 7UL);
        rval |= ctx_check_uint32_parameter_valid_range(prefix, "to_t1_add_cr.high_delta", p_hili_combine->to_t1_add_cr.high_strength, 0UL, 255UL);

        {
            // advanced level control check
            ik_level_t level;

            level.low = p_hili_combine->max_change_cb.low;
            level.low_delta = p_hili_combine->max_change_cb.low_delta;
            level.high = p_hili_combine->max_change_cb.high;
            level.high_delta = p_hili_combine->max_change_cb.high_delta;
            rval |= check_advanced_level_control("hili_combine->max_change_cb", &level);

            level.low = p_hili_combine->max_change_cr.low;
            level.low_delta = p_hili_combine->max_change_cr.low_delta;
            level.high = p_hili_combine->max_change_cr.high;
            level.high_delta = p_hili_combine->max_change_cr.high_delta;
            rval |= check_advanced_level_control("hili_combine->max_change_cb", &level);

            level.low = p_hili_combine->to_t1_add_cb.low;
            level.low_delta = p_hili_combine->to_t1_add_cb.low_delta;
            level.high = p_hili_combine->to_t1_add_cb.high;
            level.high_delta = p_hili_combine->to_t1_add_cb.high_delta;
            rval |= check_advanced_level_control("hili_combine->to_t1_add_cb", &level);

            level.low = p_hili_combine->to_t1_add_cr.low;
            level.low_delta = p_hili_combine->to_t1_add_cr.low_delta;
            level.high = p_hili_combine->to_t1_add_cr.high;
            level.high_delta = p_hili_combine->to_t1_add_cr.high_delta;
            rval |= check_advanced_level_control("hili_combine->to_t1_add_cr", &level);
        }
    }
    return rval;
}

static uint32 ctx_check_hi_mid_high_freq_recover_valid_range(const ik_hi_mid_high_freq_recover_t *p_hi_mid_high_freq_recover, const char *prefix)
{
    uint32 rval = IK_OK;
    uint32  i = 0, j = 0;
    if (p_hi_mid_high_freq_recover == NULL) {
        amba_ik_system_print_str_5("[IK] detect p_hi_mid_high_freq_recover get NULL pointer!", NULL, NULL, NULL, NULL, NULL);
        rval |= IK_ERR_0005;
    } else {
        rval |= ctx_check_uint32_parameter_valid_range(prefix, "fir.specify", p_hi_mid_high_freq_recover->fir.specify, 0, 4);
        if (p_hi_mid_high_freq_recover->fir.specify == 0U) {
            rval |= ctx_check_uint32_parameter_valid_range(prefix, "fir.strength_iso", p_hi_mid_high_freq_recover->fir.strength_iso, 0, 256);
        } else if (p_hi_mid_high_freq_recover->fir.specify == 1U) {
            for (j = 0U; j < 9U; j++) {
                for (i = 0U; i < 25U; i++) {
                    rval |= ctx_check_int32_parameter_valid_range(prefix, "fir.coefs", p_hi_mid_high_freq_recover->fir.coefs[j][i], 0, 1023);
                }
            }
        } else if (p_hi_mid_high_freq_recover->fir.specify == 2U) {
            rval |= ctx_check_uint32_parameter_valid_range(prefix, "fir.strength_iso", p_hi_mid_high_freq_recover->fir.strength_iso, 0, 256);
            rval |= ctx_check_uint32_parameter_valid_range(prefix, "fir.strength_dir", p_hi_mid_high_freq_recover->fir.strength_dir, 0, 256);
        } else if (p_hi_mid_high_freq_recover->fir.specify == 3U) {
            for (i = 0U; i < 9U; i++) {
                rval |= ctx_check_uint32_parameter_valid_range(prefix, "fir.per_dir_fir_dir_amounts", p_hi_mid_high_freq_recover->fir.per_dir_fir_dir_amounts[i], 0, 256);
                rval |= ctx_check_uint32_parameter_valid_range(prefix, "fir.per_dir_fir_dir_strengths", p_hi_mid_high_freq_recover->fir.per_dir_fir_dir_strengths[i], 0, 256);
                rval |= ctx_check_uint32_parameter_valid_range(prefix, "fir.per_dir_fir_iso_strengths", p_hi_mid_high_freq_recover->fir.per_dir_fir_iso_strengths[i], 0, 256);
            }
        } else if (p_hi_mid_high_freq_recover->fir.specify == 4U) {
            for (j = 0U; j < 9U; j++) {
                for (i = 0U; i < 25U; i++) {
                    rval |= ctx_check_int32_parameter_valid_range(prefix, "fir.coefs", p_hi_mid_high_freq_recover->fir.coefs[j][i], 0, 1023);
                }
            }
        } else {
            rval |= IK_ERR_0008;
        }
        rval |= ctx_check_uint32_parameter_valid_range(prefix, "max_up", p_hi_mid_high_freq_recover->max_up, 0, 255);
        rval |= ctx_check_uint32_parameter_valid_range(prefix, "max_down", p_hi_mid_high_freq_recover->max_down, 0, 255);

        rval |= ctx_check_uint32_parameter_valid_range(prefix, "scale_level.low", p_hi_mid_high_freq_recover->scale_level.low, 0UL, 253UL);
        rval |= ctx_check_uint32_parameter_valid_range(prefix, "scale_level.low_delta", p_hi_mid_high_freq_recover->scale_level.low_delta, 0UL, 7UL);
        rval |= ctx_check_uint32_parameter_valid_range(prefix, "scale_level.low_delta", p_hi_mid_high_freq_recover->scale_level.low_strength, 0UL, 255UL);
        rval |= ctx_check_uint32_parameter_valid_range(prefix, "scale_level.high_delta", p_hi_mid_high_freq_recover->scale_level.mid_strength, 0UL, 255UL);
        rval |= ctx_check_uint32_parameter_valid_range(prefix, "scale_level.high", p_hi_mid_high_freq_recover->scale_level.high, 1, 254);
        rval |= ctx_check_uint32_parameter_valid_range(prefix, "scale_level.high_delta", p_hi_mid_high_freq_recover->scale_level.high_delta, 0UL, 7UL);
        rval |= ctx_check_uint32_parameter_valid_range(prefix, "scale_level.high_delta", p_hi_mid_high_freq_recover->scale_level.high_strength, 0UL, 255UL);
        if ((p_hi_mid_high_freq_recover->scale_level.method != 0U) &&
            (p_hi_mid_high_freq_recover->scale_level.method != 11U)) {
            amba_ik_system_print_uint32_5("[IK] p_hi_mid_high_freq_recover->scale_level.method %d , should be 0,1", p_hi_mid_high_freq_recover->scale_level.method, DC_U, DC_U, DC_U, DC_U);
            rval |= IK_ERR_0008;
        }
    }
    return rval;
}

static uint32 ctx_check_hi_luma_blend_valid_range(const ik_hi_luma_blend_t *p_hi_luma_blend, const char *prefix)
{
    uint32 rval = IK_OK;
    if (p_hi_luma_blend == NULL) {
        amba_ik_system_print_str_5("[IK] detect p_hi_luma_blend get NULL pointer!", NULL, NULL, NULL, NULL, NULL);
        rval |= IK_ERR_0005;
    } else {
        rval |= ctx_check_uint32_parameter_valid_range(prefix, "luma_level.low", p_hi_luma_blend->luma_level.low, 0UL, 253UL);
        rval |= ctx_check_uint32_parameter_valid_range(prefix, "luma_level.low_delta", p_hi_luma_blend->luma_level.low_delta, 0UL, 7UL);
        rval |= ctx_check_uint32_parameter_valid_range(prefix, "luma_level.low_delta", p_hi_luma_blend->luma_level.low_strength, 0UL, 255UL);
        rval |= ctx_check_uint32_parameter_valid_range(prefix, "luma_level.high_delta", p_hi_luma_blend->luma_level.mid_strength, 0UL, 255UL);
        rval |= ctx_check_uint32_parameter_valid_range(prefix, "luma_level.high", p_hi_luma_blend->luma_level.high, 1, 254);
        rval |= ctx_check_uint32_parameter_valid_range(prefix, "luma_level.high_delta", p_hi_luma_blend->luma_level.high_delta, 0UL, 7UL);
        rval |= ctx_check_uint32_parameter_valid_range(prefix, "luma_level.high_delta", p_hi_luma_blend->luma_level.high_strength, 0UL, 255UL);
        if ((p_hi_luma_blend->luma_level.method != 0U) &&
            (p_hi_luma_blend->luma_level.method != 11U)) {
            amba_ik_system_print_uint32_5("[IK] p_hi_luma_blend->luma_level.method %d , should be 0,11", p_hi_luma_blend->luma_level.method, DC_U, DC_U, DC_U, DC_U);
            rval |= IK_ERR_0008;
        }
    }
    return rval;
}

static uint32 ctx_check_hi_nonsmooth_detect_valid_range(const ik_hi_nonsmooth_detect_t *p_hi_nonsmooth_detect, const char *prefix)
{
    uint32 rval = IK_OK;
    uint32  i = 0;
    if (p_hi_nonsmooth_detect == NULL) {
        amba_ik_system_print_str_5("[IK] detect p_hi_nonsmooth_detect get NULL pointer!", NULL, NULL, NULL, NULL, NULL);
        rval |= IK_ERR_0005;
    } else {
        rval |= ctx_check_uint32_parameter_valid_range(prefix, "smooth", p_hi_nonsmooth_detect->smooth, 0, 443);
        for (i = 0U; i < 16U; i++) {
            rval |= ctx_check_int32_parameter_valid_range(prefix, "lev_adjust", p_hi_nonsmooth_detect->lev_adjust[i], -255, 255);
        }
    }
    return rval;
}

void ik_ctx_hook_debug_check(void)
{
    static ik_debug_check_func_t debug_check_func;

    debug_check_func.vin_sensor_info = ctx_check_vin_sensor_info;
    debug_check_func.fe_tone_curve = ctx_check_fe_tone_curve;
    debug_check_func.fe_blc = ctx_check_fe_static_blc;
    debug_check_func.ae_gain = ctx_check_ae_gain_valid_range;
    debug_check_func.global_dgain = ctx_check_global_dgain_valid_range;
    debug_check_func.dgain_sat = ctx_check_dgain_sat_lvl_valid_range;

    debug_check_func.fe_wb = ctx_check_fe_wb_gain;
    debug_check_func.fe_dgain_sat = ctx_check_dgain_sat_lvl;
    debug_check_func.hdr_blend = ctx_check_hdr_blend;
    debug_check_func.vignette_info = ctx_check_vignette_compensation;
    debug_check_func.deferred_blc = ctx_check_deferred_blc_valid_range;

    debug_check_func.dynamic_bad_pxl_cor = ctx_check_dynamic_bpc;
    debug_check_func.static_bpc = ctx_check_bpc_valid_range;
    debug_check_func.static_bpc_internal = ctx_check_static_bpc_internal_valid_range;
    debug_check_func.cfa_leakage = ctx_check_cfa_leakage_filter_valid_range;
    debug_check_func.anti_aliasing = ctx_check_anti_aliasing_valid_range;

    debug_check_func.grgb_mismatch = ctx_check_grgb_mismatch_valid_range;
    debug_check_func.before_ce_wb = ctx_check_wb_gain_valid_range;
    debug_check_func.warp_info = ctx_check_calib_warp_info_valid_range;
    debug_check_func.warp_info_internal = ctx_check_warp_internal_valid_range;
    debug_check_func.warp_bind_info = ctx_check_warp_bind_info_valid_range;

    debug_check_func.cawarp = ctx_check_calib_ca_warp_info_valid_range;
    debug_check_func.cawarp_internal = ctx_check_ca_warp_internal_valid_range;
    debug_check_func.ce = ctx_check_ce_valid_range;
    debug_check_func.ce_input_tbl = ctx_check_ce_input_table_valid_range;
    debug_check_func.ce_output_tbl = ctx_check_ce_out_table_valid_range;
    debug_check_func.ce_ext_hds = ctx_check_ce_ext_hds_valid_range;
    debug_check_func.after_ce_wb = ctx_check_wb_gain_valid_range;

    debug_check_func.cfa_noise_filter = ctx_check_cfa_noise_filter_valid_range;
    debug_check_func.demosaic = ctx_check_demosaic_valid_range;
    debug_check_func.rgb_ir = ctx_check_rgb_ir_valid_range;
    debug_check_func.pre_cc_gain = ctx_check_pre_cc_gain_valid_range;
    debug_check_func.color_correction_reg = ctx_check_color_correction_reg_valid_range;
    debug_check_func.color_correction = ctx_check_color_correction_valid_range;

    debug_check_func.tone_curve = ctx_check_tone_curve_valid_range;
    debug_check_func.rgb_to_yuv_matrix = ctx_check_rgb_to_yuv_matrix_valid_range;
    debug_check_func.chroma_sacle = ctx_check_chroma_scale_valid_range;
    debug_check_func.chroma_filter = ctx_check_chroma_filter_valid_range;
    debug_check_func.chroma_median_filter = ctx_check_chroma_median_filter_valid_range;

    debug_check_func.first_luma_proc_mode = ctx_check_first_luma_processing_mode_valid_range;
    debug_check_func.adv_spat_fltr = ctx_check_advance_spatial_filter_valid_range;
    debug_check_func.first_sharpen_both = ctx_check_fstshpns_both_valid_range;
    debug_check_func.first_sharpen_fir = ctx_check_fstshpns_fir_valid_range;
    debug_check_func.first_sharpen_noise = ctx_check_fstshpns_noise_valid_range;

    debug_check_func.first_sharpen_coring = ctx_check_fstshpns_coring_valid_range;
    debug_check_func.fstshpns_cor_idx_scl = ctx_check_fstshpns_coring_index_scale_valid_range;
    debug_check_func.fstshpns_min_cor_rst = ctx_check_fstshpns_min_coring_result_valid_range;
    debug_check_func.fstshpns_max_cor_rst = ctx_check_fstshpns_max_coring_result_valid_range;
    debug_check_func.fstshpns_scl_cor = ctx_check_fstshpns_scale_coring_valid_range;

    debug_check_func.video_mctf = ctx_check_video_mctf_valid_range;
    debug_check_func.video_mctf_internal = ctx_check_video_mctf_internal_valid_range;
    debug_check_func.video_mctf_ta = ctx_check_video_mctf_ta_valid_range;
    debug_check_func.video_mctf_and_fnlshp = ctx_check_pos_dep33_valid_range;

    debug_check_func.final_sharpen_both = ctx_check_fnlshpns_both_valid_range;
    debug_check_func.final_sharpen_fir = ctx_check_fnlshpns_fir_valid_range;
    debug_check_func.final_sharpen_noise = ctx_check_fnlshpns_noise_valid_range;
    debug_check_func.final_sharpen_coring = ctx_check_fnlshpns_coring_valid_range;
    debug_check_func.fnlshpns_cor_idx_scl = ctx_check_fnlshpns_coring_index_scale_valid_range;
    debug_check_func.fnlshpns_min_cor_rst = ctx_check_fnlshpns_min_coring_result_valid_range;
    debug_check_func.fnlshpns_max_cor_rst = ctx_check_fnlshpns_max_coring_result_valid_range;
    debug_check_func.fnlshpns_scl_cor = ctx_check_fnlshpns_scale_coring_valid_range;
    debug_check_func.final_sharpen_both_three_d_table = ctx_check_fnlshpns_both_tdt_valid_range;

    debug_check_func.dzoom_info = ctx_check_dzoom_info_valid_range;
    debug_check_func.window_size_info = ctx_check_window_size_info_valid_range;
    debug_check_func.cfa_window_size_info = ctx_check_cfa_window_size_info_valid_range;
    debug_check_func.vin_active_win = ctx_check_vin_active_win_valid_range;
    debug_check_func.dummy_margin_range = ctx_check_set_dummy_win_margin_range_info_valid_range;
    debug_check_func.resamp_strength = ctx_check_resampler_str_valid_range;

    debug_check_func.motion_detect = ctx_check_motion_detect_valid_range;
    debug_check_func.motion_detect_and_mctf = ctx_check_motion_detect_and_mctf_valid_range;
    debug_check_func.motion_detect_pos_dep = ctx_check_pos_dep33_valid_range;

    debug_check_func.histogram_info = amba_ik_debug_check_histogram_info;
    debug_check_func.pseudo_y_info = amba_ik_debug_check_aaa_pseudo_y_info;
    debug_check_func.flip_mode = ctx_check_flip_mode_info_valid_range;
    debug_check_func.aaa_stat = ctx_check_aaa_stat_info_valid_range;
    debug_check_func.af_stat_ex = ctx_check_af_stat_ex_info_valid_range;
    debug_check_func.pg_af_stat_ex = ctx_check_pg_af_stat_ex_info_valid_range;
    debug_check_func.stat_float_tile = ctx_check_aaa_stat_float_tile_valid_range;
    debug_check_func.fusion = ctx_check_fusion;
    debug_check_func.wide_chroma_filter_combine = ctx_check_wide_chroma_filter_combine;

    // hiso
    debug_check_func.hi_anti_aliasing = ctx_check_anti_aliasing_valid_range;
    debug_check_func.hi_cfa_leakage_filter = ctx_check_cfa_leakage_filter_valid_range;
    debug_check_func.hi_dynamic_bpc = ctx_check_dynamic_bpc;
    debug_check_func.hi_grgb_mismatch = ctx_check_grgb_mismatch_valid_range;
    debug_check_func.hi_chroma_median_filter = ctx_check_chroma_median_filter_valid_range;
    debug_check_func.hi_cfa_noise_filter = ctx_check_cfa_noise_filter_valid_range;
    debug_check_func.hi_demosaic = ctx_check_demosaic_valid_range;

    //hiso low2
    debug_check_func.li2_anti_aliasing = ctx_check_anti_aliasing_valid_range;
    debug_check_func.li2_cfa_leakage_filter = ctx_check_cfa_leakage_filter_valid_range;
    debug_check_func.li2_dynamic_bpc = ctx_check_dynamic_bpc;
    debug_check_func.li2_grgb_mismatch = ctx_check_grgb_mismatch_valid_range;
    debug_check_func.li2_cfa_noise_filter = ctx_check_cfa_noise_filter_valid_range;
    debug_check_func.li2_demosaic = ctx_check_demosaic_valid_range;

    //asf
    debug_check_func.hi_asf = ctx_check_hi_asf_valid_range;
    debug_check_func.li2_asf = ctx_check_hi_asf_valid_range;
    debug_check_func.hi_low_asf = ctx_check_hi_asf_b1_valid_range;
    debug_check_func.hi_med1_asf = ctx_check_hi_asf_a_valid_range;
    debug_check_func.hi_med2_asf = ctx_check_hi_asf_b2_valid_range;
    debug_check_func.hi_high_asf = ctx_check_hi_asf_b2_valid_range;
    debug_check_func.hi_high2_asf = ctx_check_hi_asf_a_valid_range;
    debug_check_func.chroma_asf = ctx_check_chroma_asf_valid_range;
    debug_check_func.hi_chroma_asf = ctx_check_chroma_asf_valid_range;
    debug_check_func.hi_low_chroma_asf = ctx_check_chroma_asf_valid_range;

    // sharpen
    // --high--
    debug_check_func.hi_high_shpns_both = ctx_check_fstshpns_both_valid_range;
    debug_check_func.hi_high_shpns_noise = ctx_check_hi_high_shpns_noise_valid_range;
    debug_check_func.hi_high_shpns_coring = ctx_check_fstshpns_coring_valid_range;
    debug_check_func.hi_high_shpns_fir = ctx_check_fstshpns_fir_valid_range;
    debug_check_func.hi_high_shpns_cor_idx_scl = ctx_check_hi_shpns_method_level_control_valid_range;
    debug_check_func.hi_high_shpns_min_cor_rst = ctx_check_hi_shpns_method_level_control_valid_range;
    debug_check_func.hi_high_shpns_max_cor_rst = ctx_check_hi_shpns_method_level_control_valid_range;
    debug_check_func.hi_high_shpns_scl_cor = ctx_check_hi_shpns_method_level_control_valid_range;

    // --med--
    debug_check_func.hi_med_shpns_both = ctx_check_fstshpns_both_valid_range;
    debug_check_func.hi_med_shpns_noise = ctx_check_hi_high_shpns_noise_valid_range;
    debug_check_func.hi_med_shpns_coring = ctx_check_fstshpns_coring_valid_range;
    debug_check_func.hi_med_shpns_fir = ctx_check_fstshpns_fir_valid_range;
    debug_check_func.hi_med_shpns_cor_idx_scl = ctx_check_hi_shpns_method_level_control_valid_range;
    debug_check_func.hi_med_shpns_min_cor_rst = ctx_check_hi_shpns_method_level_control_valid_range;
    debug_check_func.hi_med_shpns_max_cor_rst = ctx_check_hi_shpns_method_level_control_valid_range;
    debug_check_func.hi_med_shpns_scl_cor = ctx_check_hi_shpns_method_level_control_valid_range;

    // --low2--
    debug_check_func.li2_shpns_both = ctx_check_hi_shpns_both_b_valid_range;
    debug_check_func.li2_shpns_noise = ctx_check_hi_high_shpns_noise_valid_range;
    debug_check_func.li2_shpns_coring = ctx_check_hi_shpns_coring_b_valid_range;
    debug_check_func.li2_shpns_fir = ctx_check_fstshpns_fir_valid_range;
    debug_check_func.li2_shpns_cor_idx_scl = ctx_check_hi_shpns_method_level_control_valid_range;
    debug_check_func.li2_shpns_min_cor_rst = ctx_check_hi_shpns_method_level_control_valid_range;
    debug_check_func.li2_shpns_max_cor_rst = ctx_check_hi_shpns_method_level_control_valid_range;
    debug_check_func.li2_shpns_scl_cor = ctx_check_hi_shpns_method_level_control_valid_range;

    // --hili--
    debug_check_func.hili_shpns_both = ctx_check_hi_shpns_both_b_valid_range;
    debug_check_func.hili_shpns_noise = ctx_check_hi_high_shpns_noise_valid_range;
    debug_check_func.hili_shpns_coring = ctx_check_hi_shpns_coring_b_valid_range;
    debug_check_func.hili_shpns_fir = ctx_check_fstshpns_fir_valid_range;
    debug_check_func.hili_shpns_cor_idx_scl = ctx_check_hi_shpns_method_level_control_valid_range;
    debug_check_func.hili_shpns_min_cor_rst = ctx_check_hi_shpns_method_level_control_valid_range;
    debug_check_func.hili_shpns_max_cor_rst = ctx_check_hi_shpns_method_level_control_valid_range;
    debug_check_func.hili_shpns_scl_cor = ctx_check_hi_shpns_method_level_control_valid_range;

    //chroma filter
    debug_check_func.hi_chroma_filter_high = ctx_check_hi_chroma_filter_high_valid_range;
    debug_check_func.hi_chroma_filter_pre = ctx_check_hi_chroma_filter_b_valid_range;
    debug_check_func.hi_chroma_filter_med = ctx_check_hi_chroma_filter_b_valid_range;
    debug_check_func.hi_chroma_filter_low = ctx_check_hi_chroma_filter_b_valid_range;
    debug_check_func.hi_chroma_filter_very_low = ctx_check_hi_chroma_filter_b_valid_range;

    //combine
    debug_check_func.hi_luma_combine = ctx_check_hi_luma_combine_valid_range;
    debug_check_func.hi_low_asf_combine = ctx_check_hi_luma_combine_valid_range;
    debug_check_func.hi_chroma_fltr_med_com = ctx_check_hi_chroma_filter_combine_valid_range;
    debug_check_func.hi_chroma_fltr_low_com = ctx_check_hi_chroma_filter_combine_valid_range;
    debug_check_func.hi_chroma_fltr_very_low_com = ctx_check_hi_chroma_filter_combine_valid_range;
    debug_check_func.hili_combine = ctx_check_hili_combine_valid_range;
    debug_check_func.hi_mid_high_freq_recover = ctx_check_hi_mid_high_freq_recover_valid_range;
    debug_check_func.hi_luma_blend = ctx_check_hi_luma_blend_valid_range;
    debug_check_func.hi_nonsmooth_detect = ctx_check_hi_nonsmooth_detect_valid_range;
    debug_check_func.hi_select = ctx_check_first_luma_processing_mode_valid_range;

    p_debug_check_function = &debug_check_func;
}

const ik_debug_check_func_t* ik_ctx_get_debug_check_func(void)
{
    return p_debug_check_function;
}
