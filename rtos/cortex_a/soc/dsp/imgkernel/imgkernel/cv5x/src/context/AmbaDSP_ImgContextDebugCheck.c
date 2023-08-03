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
static inline uint32 GetU64Msb(uintptr U64Val)
{
    return (uint32)((U64Val >> 32U) & 0xFFFFFFFFU);
}
static inline uint32 GetU64Lsb(uintptr U64Val)
{
    return (uint32)(U64Val & 0xFFFFFFFFU);
}

static uint32 check_advanced_level_control(char const *parameter_name, const ik_level_t *p_level)
{
    uint32 rval = IK_OK;
    uint32 low0, high0;

    low0 = p_level->low + (uint32)(1UL<<p_level->low_delta);
    high0 = p_level->high + (uint32)(1UL<<p_level->high_delta);

    rval |= (uint32)ctx_check_level_control_uint32_parameter_valid_range(parameter_name, "low", p_level->low, 0U, low0-1U);
    rval |= (uint32)ctx_check_level_control_uint32_parameter_valid_range(parameter_name, "low0", low0, p_level->low+1U, p_level->high);
    rval |= (uint32)ctx_check_level_control_uint32_parameter_valid_range(parameter_name, "high", p_level->high, low0, high0-1U);
    //rval |= (uint32)ctx_check_level_control_uint32_parameter_valid_range(parameter_name, "high0", high0, p_level->high+1U, 255U);
    if (rval != IK_OK) {
        amba_ik_system_print_str_5("[IK][ERROR] fail on advanced checker on level control \n", DC_S, DC_S, DC_S, DC_S, DC_S);
        amba_ik_system_print_uint32_5("low:%d, low_delta:%d, high:%d, high_delta:%d\n", p_level->low, p_level->low_delta, p_level->high, p_level->high_delta, DC_U);
        amba_ik_system_print_uint32_5("low0:%d, high0:%d\n", low0, high0, DC_U, DC_U, DC_U);
    } else {
        // misraC
    }

    return rval;
}

static uint32 check_advanced_level_method_control(char const *parameter_name, const ik_level_method_t *p_level)
{
    uint32 rval = IK_OK;
    uint32 low0, high0;

    low0 = p_level->low + (uint32)(1UL<<p_level->low_delta);
    high0 = p_level->high + (uint32)(1UL<<p_level->high_delta);

    rval |= (uint32)ctx_check_level_control_uint32_parameter_valid_range(parameter_name, "low", p_level->low, 0U, low0-1U);
    rval |= (uint32)ctx_check_level_control_uint32_parameter_valid_range(parameter_name, "low0", low0, p_level->low+1U, p_level->high);
    rval |= (uint32)ctx_check_level_control_uint32_parameter_valid_range(parameter_name, "high", p_level->high, low0, high0-1U);
    rval |= (uint32)ctx_check_level_control_uint32_parameter_valid_range(parameter_name, "high0", high0, p_level->high+1U, 255U);
    if (rval != IK_OK) {
        amba_ik_system_print_str_5("[IK][ERROR] fail on advanced checker on level control \n", DC_S, DC_S, DC_S, DC_S, DC_S);
        amba_ik_system_print_uint32_5("low:%d, low_delta:%d, high:%d, high_delta:%d\n", p_level->low, p_level->low_delta, p_level->high, p_level->high_delta, DC_U);
        amba_ik_system_print_uint32_5("low0:%d, high0:%d\n", low0, high0, DC_U, DC_U, DC_U);
    } else {
        // misraC
    }

    return rval;
}

static uint32 ctx_check_vin_sensor_info(const ik_vin_sensor_info_t *p_vin_sensor)
{
    uint32 rval = IK_OK;
    if (p_vin_sensor == NULL) {
        amba_ik_system_print_str_5("[IK][ERROR] amba_ik_debug_check_vin_sensor_info() p_vin_sensor = NULL\n", DC_S, DC_S, DC_S, DC_S, DC_S);
        rval = IK_ERR_0005;
    } else {
        rval |= (uint32)ctx_check_uint32_parameter_valid_range("p_vin_sensor->sensor_pattern", p_vin_sensor->sensor_pattern, 0U, 0x3U);
        rval |= (uint32)ctx_check_uint32_parameter_valid_range("p_vin_sensor->sensor_resolution", p_vin_sensor->sensor_resolution, 0U, 0x10U);
        rval |= (uint32)ctx_check_uint32_parameter_valid_range("p_vin_sensor->sensor_mode", p_vin_sensor->sensor_mode, 0U, 0x3U);
        rval |= (uint32)ctx_check_int32_parameter_valid_range("p_vin_sensor->compression_offset", p_vin_sensor->compression_offset, -16384, 16383);
        rval |= (uint32)ctx_check_uint32_parameter_valid_range("p_vin_sensor->sensor_readout_mode", p_vin_sensor->sensor_readout_mode, 0U, 131071U);
        if ((p_vin_sensor->compression>8U)&&(!(p_vin_sensor->compression>=256U)&&(p_vin_sensor->compression<=259U))) {
            amba_ik_system_print_uint32_5("[IK] parameter, p_vin_sensor->compression = %d, out of range [0:8] or [256:259]", p_vin_sensor->compression, DC_U, DC_U, DC_U, DC_U);
            rval |= IK_ERR_0008;
        }

    }
    return rval;
}

static uint32 ctx_check_fe_tone_curve(const ik_frontend_tone_curve_t *p_fe_tc)
{
    uint32 rval = IK_OK;
    uint32 i;

    if (p_fe_tc==NULL) {
        amba_ik_system_print_str_5("[IK][ERROR] amba_ik_debug_check_fe_tone_curve() p_fe_tc = NULL\n", DC_S, DC_S, DC_S, DC_S, DC_S);
        rval = IK_ERR_0005;
    } else {
        rval |= (uint32)ctx_check_uint32_parameter_valid_range("p_vin_sensor->sensor_pattern", p_fe_tc->decompand_enable, 0U, 0x1U);
        if(p_fe_tc->decompand_enable == 1U) {
            for (i = 0; i < IK_NUM_DECOMPAND_TABLE; i++) {
                rval |= (uint32)ctx_check_uint32_parameter_valid_range("p_vin_sensor->decompand_table", p_fe_tc->decompand_table[i], 0U, 1048575U);
                if(rval != IK_OK) {
                    amba_ik_system_print_uint32_5("[IK][ERROR] amba_ik_debug_check_fe_tone_curve() decompand_table[%d] = %d\n", i, p_fe_tc->decompand_table[i], DC_U, DC_U, DC_U);
                }
            }
        }
    }
    return rval;
}

static uint32 ctx_check_fe_static_blc(const ik_static_blc_level_t *p_fe_blc, uint32 exp_id)
{
    uint32 rval = IK_OK;

    if (p_fe_blc==NULL) {
        amba_ik_system_print_str_5("[IK][ERROR] amba_ik_debug_check_fe_static_blc() p_fe_blc = NULL\n", DC_S, DC_S, DC_S, DC_S, DC_S);
        rval = IK_ERR_0005;
    } else {
        rval |= ctx_check_int32_parameter_valid_range("frontend_static_blc->black_r", p_fe_blc->black_r, -16384, 16383);
        rval |= ctx_check_int32_parameter_valid_range("frontend_static_blc->black_gr", p_fe_blc->black_gr, -16384, 16383);
        rval |= ctx_check_int32_parameter_valid_range("frontend_static_blc->black_gb", p_fe_blc->black_gb, -16384, 16383);
        rval |= ctx_check_int32_parameter_valid_range("frontend_static_blc->black_b", p_fe_blc->black_b, -16384, 16383);
        if(rval != IK_OK) {
            amba_ik_system_print_uint32_5("[IK][ERROR] amba_ik_debug_check_fe_static_blc(exp_id %d) NG.\n", exp_id, DC_U, DC_U, DC_U, DC_U);
        }
    }
    return rval;
}

static uint32 ctx_check_fe_wb_gain(const ik_frontend_wb_gain_t *p_fe_wb_gain, uint32 exp_id)
{
    uint32 rval = IK_OK;

    if (p_fe_wb_gain == NULL) {
        amba_ik_system_print_str_5("[IK][ERROR] amba_ik_debug_check_fe_static_blc() p_fe_blc = NULL\n", DC_S, DC_S, DC_S, DC_S, DC_S);
        rval = IK_ERR_0005;
    } else {
        rval |= ctx_check_uint32_parameter_valid_range("p_fe_wb_gain->r_gain", p_fe_wb_gain->r_gain, 0U, 0x400000U);
        rval |= ctx_check_uint32_parameter_valid_range("p_fe_wb_gain->g_gain", p_fe_wb_gain->g_gain, 0U, 0x400000U);
        rval |= ctx_check_uint32_parameter_valid_range("p_fe_wb_gain->b_gain", p_fe_wb_gain->b_gain, 0U, 0x400000U);
        rval |= ctx_check_uint32_parameter_valid_range("p_fe_wb_gain->shutter_ratio", p_fe_wb_gain->shutter_ratio, 1U, 16383U);
        if (rval != IK_OK) {
            amba_ik_system_print_uint32_5("[IK][ERROR] amba_ik_debug_check_fe_wb_gain(exp_id %d) NG.\n", exp_id, DC_U, DC_U, DC_U, DC_U);
        }
    }
    return rval;
}

static uint32 ctx_check_dgain_sat_lvl(const ik_frontend_dgain_saturation_level_t *p_dgain_sat, uint32 exp_id)
{
    uint32 rval = IK_OK;

    if (p_dgain_sat == NULL) {
        amba_ik_system_print_str_5("[IK][ERROR] ik_frontend_dgain_saturation_level_t() p_dgain_sat = NULL\n", DC_S, DC_S, DC_S, DC_S, DC_S);
        rval = IK_ERR_0005;
    } else {
        rval |= ctx_check_uint32_parameter_valid_range("p_dgain_sat->pre_level_red", p_dgain_sat->pre_level_red, 0, 1048575);
        rval |= ctx_check_uint32_parameter_valid_range("p_dgain_sat->pre_level_green_even", p_dgain_sat->pre_level_green_even, 0, 1048575);
        rval |= ctx_check_uint32_parameter_valid_range("p_dgain_sat->pre_level_green_odd", p_dgain_sat->pre_level_green_odd, 0, 1048575);
        rval |= ctx_check_uint32_parameter_valid_range("p_dgain_sat->pre_level_bule", p_dgain_sat->pre_level_bule, 0, 1048575);
        rval |= ctx_check_uint32_parameter_valid_range("p_dgain_sat->post_level_red", p_dgain_sat->post_level_red, 0, 1048575);
        rval |= ctx_check_uint32_parameter_valid_range("p_dgain_sat->post_level_green_even", p_dgain_sat->post_level_green_even, 0, 1048575);
        rval |= ctx_check_uint32_parameter_valid_range("p_dgain_sat->post_level_green_odd", p_dgain_sat->post_level_green_odd, 0, 1048575);
        rval |= ctx_check_uint32_parameter_valid_range("p_dgain_sat->post_level_bule", p_dgain_sat->post_level_bule, 0, 1048575);
        if (rval != IK_OK) {
            amba_ik_system_print_uint32_5("[IK][ERROR] amba_ik_debug_check_dgain_sat_lvl(exp_id %d) NG.\n", exp_id, DC_U, DC_U, DC_U, DC_U);
        }
    }
    return rval;
}

static uint32 ctx_check_hdr_blend(const ik_hdr_blend_t *p_hdr_blend)
{
    uint32 rval = IK_OK;

    if (p_hdr_blend == NULL) {
        amba_ik_system_print_str_5("[IK][ERROR] amba_ik_debug_check_hdr_blend() p_fe_blc = NULL\n", DC_S, DC_S, DC_S, DC_S, DC_S);
        rval = IK_ERR_0005;
    } else {
        rval |= ctx_check_uint32_parameter_valid_range("p_hdr_blend->output_enable", p_hdr_blend->enable, 0U, 1U);
        rval |= ctx_check_uint32_parameter_valid_range("p_hdr_blend->delta_t0[0]", p_hdr_blend->delta_t0[0], 0U, 19U);
        rval |= ctx_check_uint32_parameter_valid_range("p_hdr_blend->delta_t0[1]", p_hdr_blend->delta_t0[1], 0U, 19U);
        rval |= ctx_check_uint32_parameter_valid_range("p_hdr_blend->delta_t0[2]", p_hdr_blend->delta_t0[2], 0U, 19U);
        rval |= ctx_check_uint32_parameter_valid_range("p_hdr_blend->delta_t0[3]", p_hdr_blend->delta_t0[3], 0U, 19U);
        rval |= ctx_check_uint32_parameter_valid_range("p_hdr_blend->delta_t1[0]", p_hdr_blend->delta_t1[0], 0U, 19U);
        rval |= ctx_check_uint32_parameter_valid_range("p_hdr_blend->delta_t1[1]", p_hdr_blend->delta_t1[1], 0U, 19U);
        rval |= ctx_check_uint32_parameter_valid_range("p_hdr_blend->delta_t1[2]", p_hdr_blend->delta_t1[2], 0U, 19U);
        rval |= ctx_check_uint32_parameter_valid_range("p_hdr_blend->delta_t1[3]", p_hdr_blend->delta_t1[3], 0U, 19U);
        rval |= ctx_check_uint32_parameter_valid_range("p_hdr_blend->flicker_threshold", p_hdr_blend->flicker_threshold, 0U, 256U);
        rval |= ctx_check_uint32_parameter_valid_range("p_hdr_blend->t0_offset[0]", p_hdr_blend->t0_offset[0], 0U, 1048575U/*(1UL<<20U)-1U*/);
        rval |= ctx_check_uint32_parameter_valid_range("p_hdr_blend->t0_offset[1]", p_hdr_blend->t0_offset[1], 0U, 1048575U/*(1UL<<20U)-1U*/);
        rval |= ctx_check_uint32_parameter_valid_range("p_hdr_blend->t0_offset[2]", p_hdr_blend->t0_offset[2], 0U, 1048575U/*(1UL<<20U)-1U*/);
        rval |= ctx_check_uint32_parameter_valid_range("p_hdr_blend->t0_offset[3]", p_hdr_blend->t0_offset[3], 0U, 1048575U/*(1UL<<20U)-1U*/);
        rval |= ctx_check_uint32_parameter_valid_range("p_hdr_blend->t1_offset[0]", p_hdr_blend->t1_offset[0], 0U, 1048575U/*(1UL<<20U)-1U*/);
        rval |= ctx_check_uint32_parameter_valid_range("p_hdr_blend->t1_offset[1]", p_hdr_blend->t1_offset[1], 0U, 1048575U/*(1UL<<20U)-1U*/);
        rval |= ctx_check_uint32_parameter_valid_range("p_hdr_blend->t1_offset[2]", p_hdr_blend->t1_offset[2], 0U, 1048575U/*(1UL<<20U)-1U*/);
        rval |= ctx_check_uint32_parameter_valid_range("p_hdr_blend->t1_offset[3]", p_hdr_blend->t1_offset[3], 0U, 1048575U/*(1UL<<20U)-1U*/);
        rval |= ctx_check_uint32_parameter_valid_range("p_hdr_blend->a0[0]", p_hdr_blend->a0[0], 0U, 256U);
        rval |= ctx_check_uint32_parameter_valid_range("p_hdr_blend->a0[1]", p_hdr_blend->a0[1], 0U, 256U);
        rval |= ctx_check_uint32_parameter_valid_range("p_hdr_blend->a0[2]", p_hdr_blend->a0[2], 0U, 256U);
        rval |= ctx_check_uint32_parameter_valid_range("p_hdr_blend->a0[3]", p_hdr_blend->a0[3], 0U, 256U);
        rval |= ctx_check_uint32_parameter_valid_range("p_hdr_blend->a1[0]", p_hdr_blend->a1[0], 0U, 256U);
        rval |= ctx_check_uint32_parameter_valid_range("p_hdr_blend->a1[1]", p_hdr_blend->a1[1], 0U, 256U);
        rval |= ctx_check_uint32_parameter_valid_range("p_hdr_blend->a1[2]", p_hdr_blend->a1[2], 0U, 256U);
        rval |= ctx_check_uint32_parameter_valid_range("p_hdr_blend->a1[3]", p_hdr_blend->a1[3], 0U, 256U);
        rval |= ctx_check_uint32_parameter_valid_range("p_hdr_blend->b0[0]", p_hdr_blend->b0[0], 0U, 256U);
        rval |= ctx_check_uint32_parameter_valid_range("p_hdr_blend->b0[1]", p_hdr_blend->b0[1], 0U, 256U);
        rval |= ctx_check_uint32_parameter_valid_range("p_hdr_blend->b0[2]", p_hdr_blend->b0[2], 0U, 256U);
        rval |= ctx_check_uint32_parameter_valid_range("p_hdr_blend->b0[3]", p_hdr_blend->b0[3], 0U, 256U);
        rval |= ctx_check_uint32_parameter_valid_range("p_hdr_blend->b1[0]", p_hdr_blend->b1[0], 0U, 256U);
        rval |= ctx_check_uint32_parameter_valid_range("p_hdr_blend->b1[1]", p_hdr_blend->b1[1], 0U, 256U);
        rval |= ctx_check_uint32_parameter_valid_range("p_hdr_blend->b1[2]", p_hdr_blend->b1[2], 0U, 256U);
        rval |= ctx_check_uint32_parameter_valid_range("p_hdr_blend->b1[3]", p_hdr_blend->b1[3], 0U, 256U);
    }
    return rval;
}

static uint32 ctx_check_vignette_compensation(const ik_vignette_t *vignette_compensation)
{
    uint32 rval = IK_OK;
    if(vignette_compensation == NULL) {
        amba_ik_system_print_str_5("[IK][ERROR] amba_ik_debug_check_vignette_compensation() vignette_compensation = NULL\n", DC_S, DC_S, DC_S, DC_S, DC_S);
        rval = IK_ERR_0005;
    } else {
        rval = ctx_check_uint32_parameter_valid_range("vignette_compensation->calib_mode_enable", vignette_compensation->calib_mode_enable, 0, 1);
        if(vignette_compensation->calib_mode_enable == 1U) {
            rval |= ctx_check_uint32_parameter_valid_range("vignette_compensation->vin_sensor_geo.h_sub_sample.factor_den", vignette_compensation->vin_sensor_geo.h_sub_sample.factor_den, 1U, 1U);
            rval |= ctx_check_uint32_parameter_valid_range("vignette_compensation->vin_sensor_geo.h_sub_sample.factor_num", vignette_compensation->vin_sensor_geo.h_sub_sample.factor_num, 1U, 1U);
            rval |= ctx_check_uint32_parameter_valid_range("vignette_compensation->vin_sensor_geo.v_sub_sample.factor_den", vignette_compensation->vin_sensor_geo.v_sub_sample.factor_den, 1U, 1U);
            rval |= ctx_check_uint32_parameter_valid_range("vignette_compensation->vin_sensor_geo.v_sub_sample.factor_num", vignette_compensation->vin_sensor_geo.v_sub_sample.factor_num, 1U, 1U);

            rval |= ctx_check_uint32_parameter_valid_range("vignette_compensation->vin_sensor_geo.start_x", vignette_compensation->vin_sensor_geo.start_x, 0U, 8192U);
            rval |= ctx_check_uint32_parameter_valid_range("vignette_compensation->vin_sensor_geo.start_y", vignette_compensation->vin_sensor_geo.start_y, 0U, 8192U);
            rval |= ctx_check_uint32_parameter_valid_range("vignette_compensation->vin_sensor_geo.width", vignette_compensation->vin_sensor_geo.width, 2U, 15360U);
            rval |= ctx_check_uint32_parameter_valid_range("vignette_compensation->vin_sensor_geo.height", vignette_compensation->vin_sensor_geo.height, 2U, 8640U);
            rval |= ctx_check_uint32_parameter_valid_range("vignette_compensation->calib_vignette_info.version", vignette_compensation->calib_vignette_info.version, IK_VIG_VER, IK_VIG_VER);
        }
        rval |= ctx_check_uint32_parameter_valid_range("vignette_compensation->calib_vignette_info.num_radial_bins_coarse", vignette_compensation->calib_vignette_info.num_radial_bins_coarse, 0U, 127U);
        rval |= ctx_check_uint32_parameter_valid_range("vignette_compensation->calib_vignette_info.num_radial_bins_fine", vignette_compensation->calib_vignette_info.num_radial_bins_fine, 0U, 127U);
        rval |= ctx_check_uint32_parameter_valid_range("vignette_compensation->calib_vignette_info.size_radial_bins_coarse_log", vignette_compensation->calib_vignette_info.size_radial_bins_coarse_log, 0U, 8U);
        rval |= ctx_check_uint32_parameter_valid_range("vignette_compensation->calib_vignette_info.size_radial_bins_fine_log", vignette_compensation->calib_vignette_info.size_radial_bins_fine_log, 0U, 8U);

        rval |= ctx_check_uint32_parameter_valid_range("vignette_compensation->calib_vignette_info.model_center_x_R", vignette_compensation->calib_vignette_info.model_center_x_R, 0U, 8192U);
        rval |= ctx_check_uint32_parameter_valid_range("vignette_compensation->calib_vignette_info.model_center_x_Gr", vignette_compensation->calib_vignette_info.model_center_x_Gr, 0U, 8192U);
        rval |= ctx_check_uint32_parameter_valid_range("vignette_compensation->calib_vignette_info.model_center_x_B", vignette_compensation->calib_vignette_info.model_center_x_B, 0U, 8192U);
        rval |= ctx_check_uint32_parameter_valid_range("vignette_compensation->calib_vignette_info.model_center_x_Gb", vignette_compensation->calib_vignette_info.model_center_x_Gb, 0U, 8192U);

        rval |= ctx_check_uint32_parameter_valid_range("vignette_compensation->calib_vignette_info.model_center_y_R", vignette_compensation->calib_vignette_info.model_center_y_R, 0U, 8192U);
        rval |= ctx_check_uint32_parameter_valid_range("vignette_compensation->calib_vignette_info.model_center_y_Gr", vignette_compensation->calib_vignette_info.model_center_y_Gr, 0U, 8192U);
        rval |= ctx_check_uint32_parameter_valid_range("vignette_compensation->calib_vignette_info.model_center_y_B", vignette_compensation->calib_vignette_info.model_center_y_B, 0U, 8192U);
        rval |= ctx_check_uint32_parameter_valid_range("vignette_compensation->calib_vignette_info.model_center_y_Gb", vignette_compensation->calib_vignette_info.model_center_y_Gb, 0U, 8192U);

    }
    return rval;
}

static uint32 ctx_check_dynamic_bpc(const ik_dynamic_bad_pixel_correction_t *dynamic_bpc)
{
    uint32 rval = IK_OK;
    if (dynamic_bpc == NULL) {
        rval |= IK_ERR_0005;
    } else {
        rval |= (uint32)ctx_check_uint32_parameter_valid_range("dynamic_bpc->enable", dynamic_bpc->enable, 0U, 4U);
        if(dynamic_bpc->enable == 1U) {
            rval |= (uint32)ctx_check_uint32_parameter_valid_range("dynamic_bpc->hot_pixel_strength", dynamic_bpc->hot_pixel_strength, 0U, 10U);
            rval |= (uint32)ctx_check_uint32_parameter_valid_range("dynamic_bpc->dark_pixel_strength", dynamic_bpc->dark_pixel_strength, 0U, 10U);
            rval |= (uint32)ctx_check_uint32_parameter_valid_range("dynamic_bpc->correction_method", dynamic_bpc->correction_method, 0U, 1U);
        }
    }
    return rval;
}

static uint32 amba_ik_debug_check_histogram_info(const ik_histogram_info_t *p_hist_info)
{
    uint32 rval = IK_OK;
    uint32 i;
    if (p_hist_info == NULL) {
        rval |= IK_ERR_0005;
    } else {
        for (i = 0U; i < 8U; i++) {
            rval |= (uint32) ctx_check_uint32_parameter_valid_range("dynamic_bpc->enable", p_hist_info->ae_tile_mask[i], 0U, 65536U);
            if (rval != IK_OK) {
                amba_ik_system_print_uint32_5("[IK][ERROR] amba_ik_debug_check_histogram_info() ae_tile_mask[%d] NG.\n", i, DC_U, DC_U, DC_U, DC_U);
            }
        }
    }
    return rval;
}

static uint32 ctx_check_bpc_valid_range(const ik_static_bad_pxl_cor_t *static_bpc)
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
        rval |= ctx_check_uint32_parameter_valid_range("static_bpc->vin_sensor_geo.h_sub_sample.factor_den", static_bpc->vin_sensor_geo.h_sub_sample.factor_den, 1U, 16U);
        rval |= ctx_check_uint32_parameter_valid_range("static_bpc->vin_sensor_geo.h_sub_sample.factor_num", static_bpc->vin_sensor_geo.h_sub_sample.factor_num, 1U, 16U);
        rval |= ctx_check_uint32_parameter_valid_range("static_bpc->vin_sensor_geo.v_sub_sample.factor_den", static_bpc->vin_sensor_geo.v_sub_sample.factor_den, 1U, 16U);
        rval |= ctx_check_uint32_parameter_valid_range("static_bpc->vin_sensor_geo.v_sub_sample.factor_num", static_bpc->vin_sensor_geo.v_sub_sample.factor_num, 1U, 16U);

        rval |= ctx_check_uint32_parameter_valid_range("static_bpc->vin_sensor_geo.start_x", static_bpc->vin_sensor_geo.start_x, 0U, 8192U);
        rval |= ctx_check_uint32_parameter_valid_range("static_bpc->vin_sensor_geo.start_y", static_bpc->vin_sensor_geo.start_y, 0U, 8192U);
        rval |= ctx_check_uint32_parameter_valid_range("static_bpc->vin_sensor_geo.width", static_bpc->vin_sensor_geo.width, 2U, 15360U);
        rval |= ctx_check_uint32_parameter_valid_range("static_bpc->vin_sensor_geo.height", static_bpc->vin_sensor_geo.height, 2U, 8640U);

        if (static_bpc->calib_sbp_info.version != IK_SBP_VER) {
            amba_ik_system_print_uint32_5("[IK] SBP Correction Version 0x%X != 0x%X", static_bpc->calib_sbp_info.version, IK_SBP_VER, DC_U, DC_U, DC_U);
            rval |= IK_ERR_0008;
        }
        /* Check whether sensor window width is valid*/
        if ((static_bpc->vin_sensor_geo.width % 32U) != 0U) {
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

static uint32 ctx_check_cfa_leakage_filter_valid_range(const ik_cfa_leakage_filter_t *cfa_leakage_filter)
{
    uint32 rval = IK_OK;
    if (cfa_leakage_filter == NULL) {
        rval |= IK_ERR_0005;
    } else {
        rval |= (uint32)ctx_check_uint32_parameter_valid_range("cfa_leakage_filter->enable", cfa_leakage_filter->enable, 0, 1);
        rval |= (uint32)ctx_check_uint32_parameter_valid_range("cfa_leakage_filter->saturation_level", cfa_leakage_filter->saturation_level, 0, 16383);
    }
    return rval;
}

static uint32 ctx_check_anti_aliasing_valid_range(const ik_anti_aliasing_t *anti_aliasing)
{
    uint32 rval = IK_OK;
    if (anti_aliasing == NULL) {
        rval |= IK_ERR_0005;
    } else {
        rval |= (uint32)ctx_check_uint32_parameter_valid_range("anti_aliasing->enable", anti_aliasing->enable, 0, 4);
        if (anti_aliasing->enable == 4U) {
            rval |= (uint32)ctx_check_uint32_parameter_valid_range("anti_aliasing->log_fractional_correct", anti_aliasing->log_fractional_correct, 0, 7);
        }
    }
    return rval;
}

static uint32 ctx_check_grgb_mismatch_valid_range(const ik_grgb_mismatch_t *grgb_mismatch)
{
    uint32 rval = IK_OK;
    if (grgb_mismatch == NULL) {
        rval |= IK_ERR_0005;
    } else {
        rval |= (uint32)ctx_check_uint32_parameter_valid_range("grgb_mismatch->narrow_enable", grgb_mismatch->narrow_enable, 0, 1);
        rval |= (uint32)ctx_check_uint32_parameter_valid_range("grgb_mismatch->wide_enable", grgb_mismatch->wide_enable, 0, 1);
        rval |= (uint32)ctx_check_uint32_parameter_valid_range("grgb_mismatch->wide_safety", grgb_mismatch->wide_safety, 0, 256);
        rval |= (uint32)ctx_check_uint32_parameter_valid_range("grgb_mismatch->wide_thresh", grgb_mismatch->wide_thresh, 0, 256);
    }
    return rval;
}

static uint32 ctx_check_wb_gain_valid_range(const ik_wb_gain_t *wb_gain)
{
    uint32 rval = IK_OK;
    if (wb_gain == NULL) {
        rval |= IK_ERR_0005;
    } else {
        rval |= (uint32)ctx_check_uint32_parameter_valid_range("wb_gain->gain_b", wb_gain->gain_b, 1U, 0xFFFFFF);
        rval |= (uint32)ctx_check_uint32_parameter_valid_range("wb_gain->gain_g", wb_gain->gain_g, 1U, 0xFFFFFF);
        rval |= (uint32)ctx_check_uint32_parameter_valid_range("wb_gain->gain_r", wb_gain->gain_r, 1U, 0xFFFFFF);
    }
    return rval;
}

static uint32 ctx_check_pre_cc_gain_valid_range(const ik_pre_cc_gain_t *pre_cc_gain)
{
    uint32 rval = IK_OK;
    if (pre_cc_gain == NULL) {
        rval |= IK_ERR_0005;
    } else {
        rval |= (uint32)ctx_check_uint32_parameter_valid_range("pre_cc_gain->pre_cc_gain_enable", pre_cc_gain->pre_cc_gain_enable, 0, 1U);
        rval |= (uint32)ctx_check_uint32_parameter_valid_range("pre_cc_gain->pre_cc_gain_r", pre_cc_gain->pre_cc_gain_r, 1U, 0xFFFFFF);
        rval |= (uint32)ctx_check_uint32_parameter_valid_range("pre_cc_gain->pre_cc_gain_g", pre_cc_gain->pre_cc_gain_g, 1U, 0xFFFFFF);
        rval |= (uint32)ctx_check_uint32_parameter_valid_range("pre_cc_gain->pre_cc_gain_b", pre_cc_gain->pre_cc_gain_b, 1U, 0xFFFFFF);
    }
    return rval;
}

static uint32 ctx_check_stored_ir(const ik_stored_ir_t *store_ir)
{
    uint32 rval = IK_OK, i;
    if (store_ir == NULL) {
        rval |= IK_ERR_0005;
    } else {
        rval |= (uint32)ctx_check_uint32_parameter_valid_range("store_ir->ir_shift", store_ir->ir_shift, 0U, 15U);
        for (i = 0U; i < 256U; i++) {
            rval |= (uint32)ctx_check_uint32_parameter_valid_range("store_ir->ir_tone_curve", store_ir->ir_tone_curve[i], 0U, 1023U);
        }
    }
    return rval;
}

static uint32 ctx_check_calib_ca_warp_info_valid_range(const ik_cawarp_info_t *calib_ca_warp_info)
{
    uint32 rval = IK_OK;
    uintptr misra_uintptr;

    rval |= ctx_check_uint32_parameter_valid_range("calib_ca_warp_info->version", calib_ca_warp_info->version, IK_CA_VER, IK_CA_VER);
    rval |= ctx_check_uint32_parameter_valid_range("calib_ca_warp_info->hor_grid_num", calib_ca_warp_info->hor_grid_num, 2U, 64U);
    rval |= ctx_check_uint32_parameter_valid_range("calib_ca_warp_info->ver_grid_num", calib_ca_warp_info->ver_grid_num, 2U, 96U);
    rval |= ctx_check_uint32_parameter_valid_range("calib_ca_warp_info->tile_width_exp", calib_ca_warp_info->tile_width_exp, 0U, 9U);
    rval |= ctx_check_uint32_parameter_valid_range("calib_ca_warp_info->tile_height_exp", calib_ca_warp_info->tile_height_exp, 0U, 9U);
    rval |= ctx_check_uint32_parameter_valid_range("calib_ca_warp_info->vin_sensor_geo.start_x", calib_ca_warp_info->vin_sensor_geo.start_x, 0U, 8192U);
    rval |= ctx_check_uint32_parameter_valid_range("calib_ca_warp_info->vin_sensor_geo.start_y", calib_ca_warp_info->vin_sensor_geo.start_y, 0U, 8192U);
    rval |= ctx_check_uint32_parameter_valid_range("calib_ca_warp_info->vin_sensor_geo.width", calib_ca_warp_info->vin_sensor_geo.width, 2U, 15360U);
    rval |= ctx_check_uint32_parameter_valid_range("calib_ca_warp_info->vin_sensor_geo.height", calib_ca_warp_info->vin_sensor_geo.height, 2U, 8640U);
    rval |= ctx_check_uint32_parameter_valid_range("calib_ca_warp_info->vin_sensor_geo.h_sub_sample.factor_num", calib_ca_warp_info->vin_sensor_geo.h_sub_sample.factor_num, 1U, 16U);
    rval |= ctx_check_uint32_parameter_valid_range("calib_ca_warp_info->vin_sensor_geo.h_sub_sample.factor_den", calib_ca_warp_info->vin_sensor_geo.h_sub_sample.factor_den, 1U, 16U);
    rval |= ctx_check_uint32_parameter_valid_range("calib_ca_warp_info->vin_sensor_geo.v_sub_sample.factor_num", calib_ca_warp_info->vin_sensor_geo.v_sub_sample.factor_num, 1U, 16U);
    rval |= ctx_check_uint32_parameter_valid_range("calib_ca_warp_info->vin_sensor_geo.v_sub_sample.factor_den", calib_ca_warp_info->vin_sensor_geo.v_sub_sample.factor_den, 1U, 16U);
    (void)amba_ik_system_memcpy(&misra_uintptr, &calib_ca_warp_info->p_cawarp_red, sizeof(uint32*));
    if (calib_ca_warp_info->p_cawarp_red == NULL) {
        amba_ik_system_print_uint32_5("[IK] calib_ca_warp_info->p_cawarp_red 0x%x%x", GetU64Msb(misra_uintptr), GetU64Lsb(misra_uintptr), DC_U, DC_U, DC_U);
        rval |= IK_ERR_0005;
    }
    (void)amba_ik_system_memcpy(&misra_uintptr, &calib_ca_warp_info->p_cawarp_blue, sizeof(uint32*));
    if (calib_ca_warp_info->p_cawarp_blue == NULL) {
        amba_ik_system_print_uint32_5("[IK] calib_ca_warp_info->p_cawarp_blue 0x%x%x", GetU64Msb(misra_uintptr), GetU64Lsb(misra_uintptr), DC_U, DC_U, DC_U);
        rval |= IK_ERR_0005;
    }

    if (((calib_ca_warp_info->hor_grid_num - 1U) << calib_ca_warp_info->tile_width_exp) < calib_ca_warp_info->vin_sensor_geo.width) {
        amba_ik_system_print_uint32_5("[IK] (hor_grid_num %d - 1) * tile_width_exp %d < Calib vin width %d", calib_ca_warp_info->hor_grid_num, (uint32)(1UL << calib_ca_warp_info->tile_width_exp), calib_ca_warp_info->vin_sensor_geo.width, DC_U, DC_U);
        rval |= IK_ERR_0008;
    }

    if (((calib_ca_warp_info->ver_grid_num - 1U) << calib_ca_warp_info->tile_height_exp) < calib_ca_warp_info->vin_sensor_geo.height) {
        amba_ik_system_print_uint32_5("[IK] (ver_grid_num %d - 1) * tile_height_exp %d < Calib vin width %d", calib_ca_warp_info->ver_grid_num, (uint32)(1UL << calib_ca_warp_info->tile_height_exp), calib_ca_warp_info->vin_sensor_geo.height, DC_U, DC_U);
        rval |= IK_ERR_0008;
    }
    return rval;
}

static uint32 ctx_check_ce_valid_range(const ik_ce_t *p_ce)
{
    uint32 rval = IK_OK;
    int32 i = 0, sum = 0;
    if (p_ce == NULL) {
        rval |= IK_ERR_0005;
    } else {
        rval |= ctx_check_uint32_parameter_valid_range("contrast_enhance->enable", p_ce->enable, 0U, 1U);
        if (p_ce->enable == 1u) {
            rval |= ctx_check_uint32_parameter_valid_range("contrast_enhance->radius", p_ce->radius, 0U, 64U);
            rval |= ctx_check_uint32_parameter_valid_range("contrast_enhance->luma_avg_method", p_ce->luma_avg_method, 0U, 1U);
            if (rval == IK_OK) {
                // luma_avg_weight check
                if(p_ce->luma_avg_method == 0u) { // avg. mode
                    rval |= ctx_check_int32_parameter_valid_range("contrast_enhance->luma_avg_weight_r", p_ce->luma_avg_weight_r, -128, 127);
                    rval |= ctx_check_int32_parameter_valid_range("contrast_enhance->luma_avg_weight_gr", p_ce->luma_avg_weight_gr, -128, 127);
                    rval |= ctx_check_int32_parameter_valid_range("contrast_enhance->luma_avg_weight_gb", p_ce->luma_avg_weight_gb, -128, 127);
                    rval |= ctx_check_int32_parameter_valid_range("contrast_enhance->luma_avg_weight_b", p_ce->luma_avg_weight_b, -128, 127);

                    sum = (((p_ce->luma_avg_weight_r + p_ce->luma_avg_weight_gr) + p_ce->luma_avg_weight_gb) + p_ce->luma_avg_weight_b);
                    if(sum > 128) {
                        amba_ik_system_print_int32_5("[IK]ERROR! the sum(%d) of luma_avg_weight r:%d gr:%d gb:%d b:%d, should <= 128",
                            sum, p_ce->luma_avg_weight_r, p_ce->luma_avg_weight_gr, p_ce->luma_avg_weight_gb, p_ce->luma_avg_weight_b);
                        rval |= IK_ERR_0008;
                    }
                } else { // max mode
                    rval |= ctx_check_int32_parameter_valid_range("contrast_enhance->luma_avg_weight_r", p_ce->luma_avg_weight_r, 0, 127);
                    rval |= ctx_check_int32_parameter_valid_range("contrast_enhance->luma_avg_weight_gr", p_ce->luma_avg_weight_gr, 0, 127);
                    rval |= ctx_check_int32_parameter_valid_range("contrast_enhance->luma_avg_weight_gb", p_ce->luma_avg_weight_gb, 0, 127);
                    rval |= ctx_check_int32_parameter_valid_range("contrast_enhance->luma_avg_weight_b", p_ce->luma_avg_weight_b, 0, 127);
                }
            }

            rval |= ctx_check_uint32_parameter_valid_range("contrast_enhance->fir_enable", p_ce->fir_enable, 0U, 1U);
            for (i = 0; i < 3; i++) {
                rval |= ctx_check_uint32_parameter_valid_range("contrast_enhance->fir_coeff", p_ce->fir_coeff[i], 0U, 64U);
            }
            rval |= ctx_check_uint32_parameter_valid_range("contrast_enhance->coring_index_scale_shift", p_ce->coring_index_scale_shift, 0U, 24U);
            rval |= ctx_check_uint32_parameter_valid_range("contrast_enhance->coring_index_scale_low", p_ce->coring_index_scale_low, 0U, 255U);
            rval |= ctx_check_uint32_parameter_valid_range("contrast_enhance->coring_index_scale_low_delta", p_ce->coring_index_scale_low_delta, 0U, 7U);
            rval |= ctx_check_uint32_parameter_valid_range("contrast_enhance->coring_index_scale_low_strength", p_ce->coring_index_scale_low_strength, 0U, 255U);
            rval |= ctx_check_uint32_parameter_valid_range("contrast_enhance->coring_index_scale_mid_strength", p_ce->coring_index_scale_mid_strength, 0U, 255U);
            rval |= ctx_check_uint32_parameter_valid_range("contrast_enhance->coring_index_scale_high", p_ce->coring_index_scale_high, 0U, 255U);
            rval |= ctx_check_uint32_parameter_valid_range("contrast_enhance->coring_index_scale_high_delta", p_ce->coring_index_scale_high_delta, 0U, 7U);
            rval |= ctx_check_uint32_parameter_valid_range("contrast_enhance->coring_index_scale_high_strength", p_ce->coring_index_scale_high_strength, 0U, 255U);
            rval |= ctx_check_uint32_parameter_valid_range("contrast_enhance->coring_gain_shift", p_ce->coring_gain_shift, 0U, 20U);
            rval |= ctx_check_uint32_parameter_valid_range("contrast_enhance->coring_gain_low", p_ce->coring_gain_low, 0U, 255U);
            rval |= ctx_check_uint32_parameter_valid_range("contrast_enhance->coring_gain_low_delta", p_ce->coring_gain_low_delta, 0U, 7U);
            rval |= ctx_check_uint32_parameter_valid_range("contrast_enhance->coring_gain_low_strength", p_ce->coring_gain_low_strength, 0U, 255U);
            rval |= ctx_check_uint32_parameter_valid_range("contrast_enhance->coring_gain_mid_strength", p_ce->coring_gain_mid_strength, 0U, 255U);
            rval |= ctx_check_uint32_parameter_valid_range("contrast_enhance->coring_gain_high", p_ce->coring_gain_high, 0U, 255U);
            rval |= ctx_check_uint32_parameter_valid_range("contrast_enhance->coring_gain_high_delta", p_ce->coring_gain_high_delta, 0U, 255U);
            rval |= ctx_check_uint32_parameter_valid_range("contrast_enhance->coring_gain_high_strength", p_ce->coring_gain_high_strength, 0U, 255U);

            rval |= ctx_check_uint32_parameter_valid_range("contrast_enhance->boost_gain_shift", p_ce->boost_gain_shift, 0U, 7U);
            rval |= ctx_check_uint32_parameter_valid_range("contrast_enhance->boost_table_size_exp", p_ce->boost_table_size_exp, 0U, 6U);
            for (i = 0; i < (int32)IK_CE_BOOST_TABLE_SIZE; i++) {
                rval |= ctx_check_int32_parameter_valid_range("contrast_enhance->boost_table", p_ce->boost_table[i], -2048, 2047);
                if (rval != IK_OK) {
                    amba_ik_system_print_uint32_5("[IK]ERROR! boost_table[%d] = %d .boost_table value should be [-2048, 2047]", (uint32)i, (uint32)p_ce->boost_table[i], DC_U, DC_U, DC_U);
                    break;
                }
            }
            rval |= ctx_check_uint32_parameter_valid_range("contrast_enhance->radius", p_ce->radius, 0U, 3U);
            rval |= ctx_check_uint32_parameter_valid_range("contrast_enhance->epsilon", p_ce->epsilon, 0U, 1000U);

            {
                // advanced level control check
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
        } else {
            // misraC
        }
    }
    return rval;
}

static uint32 ctx_check_ce_out_table_valid_range(const ik_ce_output_table_t *p_ce_out_table)
{
    uint32 rval = IK_OK;
    if (p_ce_out_table == NULL) {
        rval |= IK_ERR_0005;
    } else {
        rval |= ctx_check_uint32_parameter_valid_range("p_ce_out_table->output_enable", p_ce_out_table->output_enable, 0U, 1U);
        rval |= ctx_check_uint32_parameter_valid_range("p_ce_out_table->output_shift", p_ce_out_table->output_shift, 0U, 12U);
    }
    return rval;
}

static uint32 ctx_check_ce_input_table_valid_range(const ik_ce_input_table_t *p_ce_input_table)
{
    uint32 rval = IK_OK;
    int32 i;
    if (p_ce_input_table == NULL) {
        rval |= IK_ERR_0005;
    } else {
        rval |= ctx_check_uint32_parameter_valid_range("p_ce_input_table->input_enable", p_ce_input_table->input_enable, 0U, 1U);
        for (i = 0; i < (int32)IK_CE_INPUT_TABLE_SIZE; i++) {
            rval |= ctx_check_uint32_parameter_valid_range("p_ce_input_table->input_table", p_ce_input_table->input_table[i], 0U, 1048575U);
            if (rval != IK_OK) {
                amba_ik_system_print_uint32_5("[IK]ERROR! input_table[%d] = %d .input table value should be < (2^20-1)", (uint32)i, p_ce_input_table->input_table[i], DC_U, DC_U, DC_U);
                break;
            }
        }
    }
    return rval;
}

static uint32 ctx_check_cfa_noise_filter_valid_range(const ik_cfa_noise_filter_t *cfa_noise_filter)
{
    uint32 rval = IK_OK;
    uint32 i;
    uint32 tmp;
    if (cfa_noise_filter == NULL) {
        rval |= IK_ERR_0005;
    } else {
        rval |= (uint32)ctx_check_uint32_parameter_valid_range("cfa_noise_filter->enable", cfa_noise_filter->enable, 0, 1);
#if 1
        for (i = 0U ; i < 3U ; i++) {
            rval |= (uint32)ctx_check_uint32_parameter_valid_range("cfa_noise_filter->extent_fine", cfa_noise_filter->extent_fine[i], 0, 256);
            rval |= (uint32)ctx_check_uint32_parameter_valid_range("cfa_noise_filter->max_noise_level", cfa_noise_filter->max_noise_level[i], 0, 32767);
            rval |= (uint32)ctx_check_uint32_parameter_valid_range("cfa_noise_filter->min_noise_level", cfa_noise_filter->min_noise_level[i], 0, 32767);
            rval |= (uint32)ctx_check_uint32_parameter_valid_range("cfa_noise_filter->original_blend_str", cfa_noise_filter->original_blend_str[i], 0, 256);
            rval |= (uint32)ctx_check_uint32_parameter_valid_range("cfa_noise_filter->extent_regular", cfa_noise_filter->extent_regular[i], 0, 256);
            rval |= (uint32)ctx_check_uint32_parameter_valid_range("cfa_noise_filter->strength_fine", cfa_noise_filter->strength_fine[i], 0, 256);
        }

        rval |= (uint32)ctx_check_uint32_parameter_valid_range("cfa_noise_filter->selectivity_regular", cfa_noise_filter->selectivity_regular, 0, 256);
        rval |= (uint32)ctx_check_uint32_parameter_valid_range("cfa_noise_filter->selectivity_fine", cfa_noise_filter->selectivity_fine, 0, 256);

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
#endif
        rval |= (uint32)ctx_check_uint32_parameter_valid_range("cfa_noise_filter->directional_enable", cfa_noise_filter->directional_enable, 0, 1);
        if(cfa_noise_filter->directional_enable == 1U) {
            rval |= (uint32)ctx_check_uint32_parameter_valid_range("cfa_noise_filter->directional_horvert_edge_strength", cfa_noise_filter->directional_horvert_edge_strength, 8, 255);
            rval |= (uint32)ctx_check_uint32_parameter_valid_range("cfa_noise_filter->directional_horvert_edge_strength_bias", cfa_noise_filter->directional_horvert_strength_bias, 0, 65535);
            rval |= (uint32)ctx_check_uint32_parameter_valid_range("cfa_noise_filter->directional_horvert_original_blend_strength", cfa_noise_filter->directional_horvert_original_blend_strength, 0, 256);
        }
    }
    return rval;
}

static uint32 ctx_check_demosaic_valid_range(const ik_demosaic_t *demosaic)
{
    uint32 rval = IK_OK;
    if (demosaic == NULL) {
        rval |= IK_ERR_0005;
    } else {
        rval |= (uint32)ctx_check_uint32_parameter_valid_range("demosaic->enable", demosaic->enable, 1, 1);
        rval |= (uint32)ctx_check_uint32_parameter_valid_range("demosaic->activity_thresh", demosaic->activity_thresh, 0, 31);
        rval |= (uint32)ctx_check_uint32_parameter_valid_range("demosaic->activity_difference_thresh", demosaic->activity_difference_thresh, 0, 16383);
        rval |= (uint32)ctx_check_uint32_parameter_valid_range("demosaic->grad_clip_thresh", demosaic->grad_clip_thresh, 0, 4095);
        rval |= (uint32)ctx_check_uint32_parameter_valid_range("demosaic->grad_noise_thresh", demosaic->grad_noise_thresh, 0, 32767);
        rval |= (uint32)ctx_check_uint32_parameter_valid_range("demosaic->clamp_directional_candidates", demosaic->alias_interpolation_strength, 0, 38);
    }
    return rval;
}

static uint32 ctx_check_ext_raw_out_mode_valid_range(const uint32 ext_raw_out_mode)
{
    uint32 rval = IK_OK;

    if (ext_raw_out_mode > 8UL) {
        rval |= (uint32)ctx_check_uint32_parameter_valid_range("compr_mode", ext_raw_out_mode, 256, 259);
    } else {
        rval |= (uint32)ctx_check_uint32_parameter_valid_range("compr_mode", ext_raw_out_mode, 0, 8);
    }

    return rval;
}

static uint32 ctx_check_rgb_to_12y_valid_range(const ik_rgb_to_12y_t *rgb_to_12y)
{
    uint32 rval = IK_OK;
    uint32 i;
    if (rgb_to_12y == NULL) {
        rval |= IK_ERR_0005;
    } else {
        rval |= (uint32)ctx_check_uint32_parameter_valid_range("rgb_to_12y->enable", rgb_to_12y->enable, 0, 1);
        for (i = 0U; i < 3U; i++) {
            rval |= (uint32)ctx_check_int32_parameter_valid_range("rgb_to_12y->matrix_values[]", rgb_to_12y->matrix_values[i], -4096, 4095);
        }
        rval |= (uint32)ctx_check_int32_parameter_valid_range("rgb_to_12y->y_offset", rgb_to_12y->y_offset, -32768, 32767);
    }
    return rval;
}

static uint32 ctx_check_luma_noise_reduction_valid_range(const ik_luma_noise_reduction_t *p_luma_noise_reduce)
{
    uint32 rval = IK_OK;
    if (p_luma_noise_reduce == NULL) {
        rval |= IK_ERR_0005;
    } else {
        rval |= (uint32)ctx_check_uint32_parameter_valid_range("p_luma_noise_reduce->enable", p_luma_noise_reduce->enable, 0, 1);
        rval |= (uint32)ctx_check_uint32_parameter_valid_range("p_luma_noise_reduce->sensor_wb_b", p_luma_noise_reduce->sensor_wb_b, 0, 4194303);
        rval |= (uint32)ctx_check_uint32_parameter_valid_range("p_luma_noise_reduce->sensor_wb_g", p_luma_noise_reduce->sensor_wb_g, 0, 4194303);
        rval |= (uint32)ctx_check_uint32_parameter_valid_range("p_luma_noise_reduce->sensor_wb_r", p_luma_noise_reduce->sensor_wb_r, 0, 4194303);
        rval |= (uint32)ctx_check_uint32_parameter_valid_range("p_luma_noise_reduce->strength0", p_luma_noise_reduce->strength0, 0, 127);
        rval |= (uint32)ctx_check_uint32_parameter_valid_range("p_luma_noise_reduce->strength1", p_luma_noise_reduce->strength1, 0, 255);
        rval |= (uint32)ctx_check_uint32_parameter_valid_range("p_luma_noise_reduce->strength2", p_luma_noise_reduce->strength2, 0, 15);
        rval |= (uint32)ctx_check_uint32_parameter_valid_range("p_luma_noise_reduce->strength2_max_change", p_luma_noise_reduce->strength2_max_change, 0, 255);
    }
    return rval;
}

static uint32 ctx_check_color_correction_valid_range(const ik_color_correction_t *color_correction)
{
    uint32 rval = IK_OK;
    uint32 i;

    if (color_correction == NULL) {
        rval = IK_ERR_0005;
    } else {
        rval |= ctx_check_uint32_parameter_valid_range("color_correction->matrix_en", color_correction->matrix_en, 0, 1);
        rval |= ctx_check_int32_parameter_valid_range("color_correction->matrix[0]", (int32)color_correction->matrix[0], 0, 4095);
        rval |= ctx_check_int32_parameter_valid_range("color_correction->matrix[1]", (int32)color_correction->matrix[1], -2048, 2047);
        rval |= ctx_check_int32_parameter_valid_range("color_correction->matrix[2]", (int32)color_correction->matrix[2], -2048, 2047);
        rval |= ctx_check_int32_parameter_valid_range("color_correction->matrix[3]", (int32)color_correction->matrix[3], 0, 4095);
        rval |= ctx_check_int32_parameter_valid_range("color_correction->matrix[4]", (int32)color_correction->matrix[4], -2048, 2047);
        rval |= ctx_check_int32_parameter_valid_range("color_correction->matrix[5]", (int32)color_correction->matrix[5], 0, 4095);
        rval |= ctx_check_uint32_parameter_valid_range("color_correction->matrix_shift_minus_8", color_correction->matrix_shift_minus_8, 0, 2);
        for (i = 0U; i < 4096U; i++) {
            rval |= ctx_check_uint32_parameter_valid_range("color_correction->matrix_three_d_table[]", color_correction->matrix_three_d_table[i], 0, 1073741824U/*(1U<<30U)*/);
        }

    }

    return rval;
}

static uint32 ctx_check_rgb_ir_valid_range(const ik_rgb_ir_t *rgb_ir)
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
        rval |= (uint32)ctx_check_int32_parameter_valid_range("rgb_ir->ircorrect_offset_b", rgb_ir->ircorrect_offset_b, -16384, 16383);
        rval |= (uint32)ctx_check_int32_parameter_valid_range("rgb_ir->ircorrect_offset_gb", rgb_ir->ircorrect_offset_gb, -16384, 16383);
        rval |= (uint32)ctx_check_int32_parameter_valid_range("rgb_ir->ircorrect_offset_gr", rgb_ir->ircorrect_offset_gr, -16384, 16383);
        rval |= (uint32)ctx_check_int32_parameter_valid_range("rgb_ir->ircorrect_offset_r", rgb_ir->ircorrect_offset_r, -16384, 16383);
        rval |= (uint32)ctx_check_uint32_parameter_valid_range("rgb_ir->mul_base_val", rgb_ir->mul_base_val, 0, 511);
        rval |= (uint32)ctx_check_uint32_parameter_valid_range("rgb_ir->mul_delta_high", rgb_ir->mul_delta_high, 0, 14);
        rval |= (uint32)ctx_check_uint32_parameter_valid_range("rgb_ir->mul_high", rgb_ir->mul_high, 0, 16383);
        rval |= (uint32)ctx_check_uint32_parameter_valid_range("rgb_ir->mul_high_val", rgb_ir->mul_high_val, 0, 511);

    }
    return rval;
}

static uint32 ctx_check_tone_curve_valid_range(const ik_tone_curve_t *tone_curve)
{
    uint32 rval = IK_OK, i;
    if (tone_curve == NULL) {
        rval |= IK_ERR_0005;
    } else {
        for (i = 0U; i < 256U; i++) {
            rval |= (uint32)ctx_check_uint32_parameter_valid_range("tone_curve->tone_curve_red", tone_curve->tone_curve_red[i], 0, 1023);
            rval |= (uint32)ctx_check_uint32_parameter_valid_range("tone_curve->tone_curve_green", tone_curve->tone_curve_green[i], 0, 1023);
            rval |= (uint32)ctx_check_uint32_parameter_valid_range("tone_curve->tone_curve_blue", tone_curve->tone_curve_blue[i], 0, 1023);
        }
    }
    return rval;
}

static uint32 ctx_check_rgb_to_yuv_matrix_valid_range(const ik_rgb_to_yuv_matrix_t *rgb_to_yuv_matrix)
{
    uint32 rval = IK_OK, i;
    if (rgb_to_yuv_matrix == NULL) {
        amba_ik_system_print_str_5("[IK] rgb_to_yuv_matrix input address is NULL", NULL, NULL, NULL, NULL, NULL);
        rval |= IK_ERR_0005;
    } else {
        for (i = 0U; i < 9U; i++) {
            rval |= (uint32)ctx_check_int32_parameter_valid_range("rgb_to_yuv_matrix->matrix_values[]", rgb_to_yuv_matrix->matrix_values[i], -4096, 4095);
        }

        rval |= (uint32)ctx_check_int32_parameter_valid_range("rgb_to_yuv_matrix->y_offset", rgb_to_yuv_matrix->y_offset, -1024, 1023);
        rval |= (uint32)ctx_check_int32_parameter_valid_range("rgb_to_yuv_matrix->u_offset", rgb_to_yuv_matrix->u_offset, -256, 255);
        rval |= (uint32)ctx_check_int32_parameter_valid_range("rgb_to_yuv_matrix->v_offset", rgb_to_yuv_matrix->v_offset, -256, 255);
    }
    return rval;
}

static uint32 ctx_check_chroma_scale_valid_range(const ik_chroma_scale_t *chroma_scale)
{
    uint32 rval = IK_OK, i;
    if (chroma_scale == NULL) {
        amba_ik_system_print_str_5("[IK] chroma_scale input address is NULL", NULL, NULL, NULL, NULL, NULL);
        rval |= IK_ERR_0005;
    } else {
        rval |= (uint32)ctx_check_uint32_parameter_valid_range("chroma_scale->enable", chroma_scale->enable, 0, 1);
        for (i = 0U; i < IK_NUM_CHROMA_GAIN_CURVE; i++) {
            rval |= (uint32)ctx_check_uint32_parameter_valid_range("chroma_scale->gain_curve[]", chroma_scale->gain_curve[i], 0, 4095);
        }
    }
    return rval;
}

static uint32 ctx_check_chroma_filter_valid_range(const ik_chroma_filter_t *chroma_filter)
{
    uint32 rval = IK_OK;
    if (chroma_filter == NULL) {
        rval |= IK_ERR_0005;
    } else {
        rval |= (uint32)ctx_check_uint32_parameter_valid_range("chroma_filter->enable", chroma_filter->enable, 0, 1);
        rval |= (uint32)ctx_check_uint32_parameter_valid_range("chroma_filter->noise_level_cb", chroma_filter->noise_level_cb, 0, 255);
        rval |= (uint32)ctx_check_uint32_parameter_valid_range("chroma_filter->noise_level_cr", chroma_filter->noise_level_cr, 0, 255);
        rval |= (uint32)ctx_check_uint32_parameter_valid_range("chroma_filter->original_blend_strength_cb", chroma_filter->original_blend_strength_cb, 0, 256);
        rval |= (uint32)ctx_check_uint32_parameter_valid_range("chroma_filter->original_blend_strength_cr", chroma_filter->original_blend_strength_cr, 0, 256);

        if ((chroma_filter->radius != 32U) && (chroma_filter->radius != 64U) && (chroma_filter->radius != 128U)) {
            amba_ik_system_print_uint32_5("[IK] chroma_filter->radius should be [32, 64, 128], %d is invalid", chroma_filter->radius, DC_U, DC_U, DC_U, DC_U);
            rval |= IK_ERR_0008;
        }
    }

    return rval;
}

static uint32 ctx_check_chroma_median_filter_valid_range(const ik_chroma_median_filter_t *chroma_median_filter)
{
    uint32 rval = IK_OK;
    if (chroma_median_filter == NULL) {
        amba_ik_system_print_str_5("[IK] chroma_median_filter input address is NULL", NULL, NULL, NULL, NULL, NULL);
        rval |= IK_ERR_0005;
    } else {
        rval |= (uint32)ctx_check_uint32_parameter_valid_range("chroma_median_filter->enable", chroma_median_filter->enable, 0, 1);
        rval |= (uint32)ctx_check_uint32_parameter_valid_range("chroma_median_filter->cb_adaptive_strength", chroma_median_filter->cb_adaptive_strength, 0, 256);
        rval |= (uint32)ctx_check_uint32_parameter_valid_range("chroma_median_filter->cr_adaptive_strength", chroma_median_filter->cr_adaptive_strength, 0, 256);
        rval |= (uint32)ctx_check_uint32_parameter_valid_range("chroma_median_filter->cb_non_adaptive_strength", chroma_median_filter->cb_non_adaptive_strength, 0, 31);
        rval |= (uint32)ctx_check_uint32_parameter_valid_range("chroma_median_filter->cr_non_adaptive_strength", chroma_median_filter->cr_non_adaptive_strength, 0, 31);
        rval |= (uint32)ctx_check_uint32_parameter_valid_range("chroma_median_filter->cb_adaptive_amount", chroma_median_filter->cb_adaptive_amount, 0, 256);
        rval |= (uint32)ctx_check_uint32_parameter_valid_range("chroma_median_filter->cr_adaptive_amount", chroma_median_filter->cb_adaptive_amount, 0, 256);
    }
    return rval;
}

static uint32 ctx_check_first_luma_processing_mode_valid_range(const ik_first_luma_process_mode_t *first_luma_process_mode)
{
    uint32 rval = IK_OK;
    if (first_luma_process_mode == NULL) {
        amba_ik_system_print_str_5("[IK] first_luma_process_mode input is NULL address!", NULL, NULL, NULL, NULL, NULL);
        rval |= IK_ERR_0005;
    } else {
        rval |= (uint32)ctx_check_uint32_parameter_valid_range("first_luma_process_mode->use_sharpen_not_asf", first_luma_process_mode->use_sharpen_not_asf, 0, 1);
    }
    return rval;
}

static uint32 ctx_check_advance_spatial_filter_valid_range(const ik_adv_spatial_filter_t *advance_spatial_filter)
{
    uint32 rval = IK_OK, i, j;
    const ik_asf_fir_t *fir;
    if (advance_spatial_filter == NULL) {
        amba_ik_system_print_str_5("[IK] advance_spatial_filter input is NULL address!", NULL, NULL, NULL, NULL, NULL);
        rval |= IK_ERR_0005;
    } else {
        if (advance_spatial_filter->enable!=0U) {
            rval |= (uint32)ctx_check_uint32_parameter_valid_range("advance_spatial_filter->enable", advance_spatial_filter->enable, 0, 1);
            fir = &advance_spatial_filter->fir;
            rval |= (uint32)ctx_check_uint32_parameter_valid_range("advance_spatial_filter->fir->specify", fir->specify, 2, 4);
            rval |= (uint32)ctx_check_uint32_parameter_valid_range("advance_spatial_filter->fir->strength_iso", fir->strength_iso, 0, 256);
            rval |= (uint32)ctx_check_uint32_parameter_valid_range("advance_spatial_filter->fir->strength_dir", fir->strength_dir, 0, 256);
            for (i = 0U; i < 9U; i++) {
                rval |= (uint32)ctx_check_uint32_parameter_valid_range("advance_spatial_filter->fir->per_dir_fir_iso_strengths[]", fir->per_dir_fir_iso_strengths[i], 0, 256);
                rval |= (uint32)ctx_check_uint32_parameter_valid_range("advance_spatial_filter->fir->per_dir_fir_dir_strengths[]", fir->per_dir_fir_dir_strengths[i], 0, 256);
                rval |= (uint32)ctx_check_uint32_parameter_valid_range("advance_spatial_filter->fir->per_dir_fir_dir_amounts[]", fir->per_dir_fir_dir_amounts[i], 0, 256);
            }
            for (i = 0U; i < 9U; i++) {
                for (j = 0U; j < 25U; j++) {
                    rval |= (uint32)ctx_check_int32_parameter_valid_range("advance_spatial_filter->fir->coefs[]", fir->coefs[i][j], -1024, 1023);
                }
            }
            if(advance_spatial_filter->max_change_not_t0_t1_alpha != 0U) {
                rval |= (uint32)ctx_check_uint32_parameter_valid_range("advance_spatial_filter->adapt->alpha_max_down", advance_spatial_filter->adapt.alpha_max_down, 0, 8);
                rval |= (uint32)ctx_check_uint32_parameter_valid_range("advance_spatial_filter->adapt->alpha_max_up", advance_spatial_filter->adapt.alpha_max_up, 0, 8);
                rval |= (uint32)ctx_check_uint32_parameter_valid_range("advance_spatial_filter->adapt->alpha_min_down", advance_spatial_filter->adapt.alpha_min_down, 0, 8);
                rval |= (uint32)ctx_check_uint32_parameter_valid_range("advance_spatial_filter->adapt->alpha_min_up", advance_spatial_filter->adapt.alpha_min_up, 0, 8);

                rval |= (uint32)ctx_check_uint32_parameter_valid_range("advance_spatial_filter->adapt->t0_down", advance_spatial_filter->adapt.t0_down, 0, 252);
                if ((advance_spatial_filter->adapt.t0_down % 2U) != 0U) {
                    amba_ik_system_print_uint32_5("[IK] advance_spatial_filter->adapt.t0_down = %d should be even", advance_spatial_filter->adapt.t0_down, DC_U, DC_U, DC_U, DC_U);
                    rval |= IK_ERR_0008;
                }

                rval |= (uint32)ctx_check_uint32_parameter_valid_range("advance_spatial_filter->adapt->t0_up", advance_spatial_filter->adapt.t0_up, 0U, 254U);
                if ((advance_spatial_filter->adapt.t0_down % 2U) != 0U) {
                    amba_ik_system_print_uint32_5("[IK] advance_spatial_filter->adapt.t0_up = %d should be even", advance_spatial_filter->adapt.t0_up, DC_U, DC_U, DC_U, DC_U);
                    rval |= IK_ERR_0008;
                }

                rval |= (uint32)ctx_check_uint32_parameter_valid_range("advance_spatial_filter->adapt->t1_down", advance_spatial_filter->adapt.t1_down, 2U, 254U);
                if ((advance_spatial_filter->adapt.t1_down % 2U) != 0U) {
                    amba_ik_system_print_uint32_5("[IK] advance_spatial_filter->adapt.t1_down = %d should be even", advance_spatial_filter->adapt.t1_down, DC_U, DC_U, DC_U, DC_U);
                    rval |= IK_ERR_0008;
                }
                rval |= (uint32)ctx_check_uint32_parameter_valid_range("advance_spatial_filter->adapt->t1_up", advance_spatial_filter->adapt.t1_up, 2U, 254U);
                if ((advance_spatial_filter->adapt.t0_down % 2U) != 0U) {
                    amba_ik_system_print_uint32_5("[IK] advance_spatial_filter->adapt.t1_up = %d should be even", advance_spatial_filter->adapt.t1_up, DC_U, DC_U, DC_U, DC_U);
                    rval |= IK_ERR_0008;
                }

                if (advance_spatial_filter->adapt.t0_up > advance_spatial_filter->adapt.t1_up) {
                    amba_ik_system_print_uint32_5("[IK] advance_spatial_filter->adapt->T1_up=%d should larger than T0_up= %d in ", advance_spatial_filter->adapt.t1_up, advance_spatial_filter->adapt.t0_up, DC_U, DC_U, DC_U);
                    rval |= IK_ERR_0008;
                }

                if (advance_spatial_filter->adapt.t0_down > advance_spatial_filter->adapt.t1_down) {
                    amba_ik_system_print_uint32_5("[IK] advance_spatial_filter->adapt->t1_down=%d should larger than t0_down= %d in ", advance_spatial_filter->adapt.t1_down, advance_spatial_filter->adapt.t0_down, DC_U, DC_U, DC_U);
                    rval |= IK_ERR_0008;
                }
            }

            rval |= (uint32)ctx_check_uint32_parameter_valid_range("advance_spatial_filter->level_str_adjust->high", advance_spatial_filter->level_str_adjust.high, 1U, 254U);
            rval |= (uint32)ctx_check_uint32_parameter_valid_range("advance_spatial_filter->level_str_adjust->low", advance_spatial_filter->level_str_adjust.low, 0U, 253U);
            rval |= (uint32)ctx_check_uint32_parameter_valid_range("advance_spatial_filter->level_str_adjust->low_delta", advance_spatial_filter->level_str_adjust.low_delta, 0U, 7U);
            rval |= (uint32)ctx_check_uint32_parameter_valid_range("advance_spatial_filter->level_str_adjust->low_strength", advance_spatial_filter->level_str_adjust.low_strength, 0, 64);
            rval |= (uint32)ctx_check_uint32_parameter_valid_range("advance_spatial_filter->level_str_adjust->high_delta", advance_spatial_filter->level_str_adjust.high_delta, 0U, 7U);
            rval |= (uint32)ctx_check_uint32_parameter_valid_range("advance_spatial_filter->level_str_adjust->high_strength", advance_spatial_filter->level_str_adjust.high_strength, 0, 64);
            rval |= (uint32)ctx_check_uint32_parameter_valid_range("advance_spatial_filter->level_str_adjust->mid_strength", advance_spatial_filter->level_str_adjust.mid_strength, 0, 64);
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

static uint32 ctx_check_fstshpns_both_valid_range(const ik_first_sharpen_both_t *first_sharpen_both)
{
    uint32 rval = IK_OK;
    if (first_sharpen_both == NULL) {
        amba_ik_system_print_str_5("[IK] detect first_sharpen_both get NULL pointer!", NULL, NULL, NULL, NULL, NULL);
        rval |= IK_ERR_0005;
    } else {
        rval |= (uint32)ctx_check_uint32_parameter_valid_range("first_sharpen_both->enable", first_sharpen_both->enable, 0, 1);
        rval |= (uint32)ctx_check_uint32_parameter_valid_range("first_sharpen_both->mode", first_sharpen_both->mode, 0, 2);
        if((first_sharpen_both->mode != 0U)&&(first_sharpen_both->mode != 2U)) {
            amba_ik_system_print_uint32_5("[IKC] Error, parameter first_sharpen_both->mode=%d, should be 0 or 2", first_sharpen_both->mode, DC_U, DC_U, DC_U, DC_U);
            rval |= IK_ERR_0008;
        }

        rval |= (uint32)ctx_check_uint32_parameter_valid_range("first_sharpen_both->edge_thresh", first_sharpen_both->edge_thresh, 0, 2047);
        rval |= (uint32)ctx_check_uint32_parameter_valid_range("first_sharpen_both->wide_edge_detect", first_sharpen_both->wide_edge_detect, 0, 8);

        rval |= (uint32)ctx_check_uint32_parameter_valid_range("first_sharpen_both->max_change.up5x5", first_sharpen_both->max_change_up5x5, 0, 255);
        rval |= (uint32)ctx_check_uint32_parameter_valid_range("first_sharpen_both->max_change.down5x5", first_sharpen_both->max_change_down5x5, 0, 255);
    }
    return rval;

}

static uint32 ctx_check_fstshpns_noise_valid_range(const ik_first_sharpen_noise_t *first_sharpen_noise)
{
    uint32 rval = IK_OK;
    uint32 i = 0, j = 0;
    int32 tmp_int32;
    if (first_sharpen_noise == NULL) {
        amba_ik_system_print_str_5("[IK] detect first_sharpen_noise get NULL pointer!", NULL, NULL, NULL, NULL, NULL);
        rval |= IK_ERR_0005;
    } else {
        rval |= ctx_check_uint32_parameter_valid_range("first_sharpen_noise->max_change_up", first_sharpen_noise->max_change_up, 0, 255);
        rval |= ctx_check_uint32_parameter_valid_range("first_sharpen_noise->max_change_down", first_sharpen_noise->max_change_down, 0, 255);
        rval |= ctx_check_uint32_parameter_valid_range("first_sharpen_noise->spatial_fir.specify", first_sharpen_noise->spatial_fir.specify, 0, 4);
        if (first_sharpen_noise->spatial_fir.specify == 0U) {
            rval |= ctx_check_uint32_parameter_valid_range("first_sharpen_noise->spatial_fir.strength_iso", first_sharpen_noise->spatial_fir.strength_iso, 0, 256);
        } else if (first_sharpen_noise->spatial_fir.specify == 1U) {
            for (i = 0U; i < 25U; i++) {
                rval |= ctx_check_int32_parameter_valid_range("first_sharpen_noise->spatial_fir.coefs", first_sharpen_noise->spatial_fir.coefs[0][i], -1024, 1023);
            }
        } else if (first_sharpen_noise->spatial_fir.specify == 2U) {
            rval |= ctx_check_uint32_parameter_valid_range("first_sharpen_noise->spatial_fir.strength_iso", first_sharpen_noise->spatial_fir.strength_iso, 0, 256);
            rval |= ctx_check_uint32_parameter_valid_range("first_sharpen_noise->spatial_fir.strength_dir", first_sharpen_noise->spatial_fir.strength_dir, 0, 256);
        } else if (first_sharpen_noise->spatial_fir.specify == 3U) {
            for (i = 0U; i < 9U; i++) {
                rval |= ctx_check_uint32_parameter_valid_range("first_sharpen_noise->spatial_fir.per_dir_fir_dir_amounts", first_sharpen_noise->spatial_fir.per_dir_fir_dir_amounts[i], 0, 256);
                rval |= ctx_check_uint32_parameter_valid_range("first_sharpen_noise->spatial_fir.per_dir_fir_dir_strengths", first_sharpen_noise->spatial_fir.per_dir_fir_dir_strengths[i], 0, 256);
                rval |= ctx_check_uint32_parameter_valid_range("first_sharpen_noise->spatial_fir.per_dir_fir_iso_strengths", first_sharpen_noise->spatial_fir.per_dir_fir_iso_strengths[i], 0, 256);
            }
        } else if (first_sharpen_noise->spatial_fir.specify == 4U) {
            for (j = 0U; j < 9U; j++) {
                for (i = 0U; i < 25U; i++) {
                    rval |= ctx_check_int32_parameter_valid_range("first_sharpen_noise->spatial_fir.coefs", first_sharpen_noise->spatial_fir.coefs[j][i], -1024, 1023);
                }
            }
        } else {
            rval |= IK_ERR_0008;
        }
        rval |= ctx_check_uint32_parameter_valid_range("first_sharpen_noise->level_str_adjust.low", first_sharpen_noise->level_str_adjust.low, 0U, 253U);
        rval |= ctx_check_uint32_parameter_valid_range("first_sharpen_noise->level_str_adjust.low_delta", first_sharpen_noise->level_str_adjust.low_delta, 0U, 7U);
        rval |= ctx_check_uint32_parameter_valid_range("first_sharpen_noise->level_str_adjust.high", first_sharpen_noise->level_str_adjust.high, 1U, 254U);
        rval |= ctx_check_uint32_parameter_valid_range("first_sharpen_noise->level_str_adjust.high_delta", first_sharpen_noise->level_str_adjust.high_delta, 0U, 7U);
        rval |= ctx_check_uint32_parameter_valid_range("first_sharpen_noise->level_str_adjust.low_strength", first_sharpen_noise->level_str_adjust.low_strength, 0U, 255U);
        rval |= ctx_check_uint32_parameter_valid_range("first_sharpen_noise->level_str_adjust.mid_strength", first_sharpen_noise->level_str_adjust.mid_strength, 0U, 255U);
        rval |= ctx_check_uint32_parameter_valid_range("first_sharpen_noise->level_str_adjust.high_strength", first_sharpen_noise->level_str_adjust.high_strength, 0U, 255U);
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

static uint32 ctx_check_fstshpns_fir_valid_range(const ik_first_sharpen_fir_t *first_sharpen_fir)
{
    uint32 rval = IK_OK;
    uint32  i = 0, j = 0;
    if (first_sharpen_fir == NULL) {
        amba_ik_system_print_str_5("[IK] detect first_sharpen_fir get NULL pointer!", NULL, NULL, NULL, NULL, NULL);
        rval |= IK_ERR_0005;
    } else {
        rval |= ctx_check_uint32_parameter_valid_range("first_sharpen_fir->specify", first_sharpen_fir->specify, 0, 4);
        if (first_sharpen_fir->specify == 0U) {
            rval |= ctx_check_uint32_parameter_valid_range("first_sharpen_fir->strength_iso", first_sharpen_fir->strength_iso, 0, 256);
        } else if (first_sharpen_fir->specify == 1U) {
            for (j = 0U; j < 9U; j++) {
                for (i = 0U; i < 25U; i++) {
                    rval |= ctx_check_int32_parameter_valid_range("first_sharpen_fir->coefs", first_sharpen_fir->coefs[j][i], -1024, 1023);
                }
            }
        } else if (first_sharpen_fir->specify == 2U) {
            rval |= ctx_check_uint32_parameter_valid_range("first_sharpen_fir->strength_iso", first_sharpen_fir->strength_iso, 0, 256);
            rval |= ctx_check_uint32_parameter_valid_range("first_sharpen_fir->strength_dir", first_sharpen_fir->strength_dir, 0, 256);
        } else if (first_sharpen_fir->specify == 3U) {
            for (i = 0U; i < 9U; i++) {
                rval |= ctx_check_uint32_parameter_valid_range("first_sharpen_fir->per_dir_fir_dir_amounts", first_sharpen_fir->per_dir_fir_dir_amounts[i], 0, 256);
                rval |= ctx_check_uint32_parameter_valid_range("first_sharpen_fir->per_dir_fir_dir_strengths", first_sharpen_fir->per_dir_fir_dir_strengths[i], 0, 256);
                rval |= ctx_check_uint32_parameter_valid_range("first_sharpen_fir->per_dir_fir_iso_strengths", first_sharpen_fir->per_dir_fir_iso_strengths[i], 0, 256);
            }
        } else if (first_sharpen_fir->specify == 4U) {
            for (j = 0U; j < 9U; j++) {
                for (i = 0U; i < 25U; i++) {
                    rval |= ctx_check_int32_parameter_valid_range("first_sharpen_fir->coefs", first_sharpen_fir->coefs[j][i], -1024, 1023);
                }
            }
        } else {
            rval |= IK_ERR_0008;
        }
    }
    return rval;
}

static uint32 ctx_check_fstshpns_coring_valid_range(const ik_first_sharpen_coring_t *first_sharpen_coring)
{
    uint32 rval = IK_OK;
    uint32 i = 0;
    if (first_sharpen_coring == NULL) {
        amba_ik_system_print_str_5("[IK] detect first_sharpen_coring get NULL pointer!", NULL, NULL, NULL, NULL, NULL);
        rval |= IK_ERR_0005;
    } else {
        rval |= ctx_check_uint32_parameter_valid_range("first_sharpen_coring->fractional_bits", first_sharpen_coring->fractional_bits, 1, 3);
        for (i = 0U; i < 256U; i++) {
            rval |= ctx_check_uint32_parameter_valid_range("first_sharpen_coring->coring", first_sharpen_coring->coring[i], 0, 31);
        }
    }
    return rval;
}

static uint32 ctx_check_fstshpns_coring_index_scale_valid_range(const ik_first_sharpen_coring_idx_scale_t *fstshpns_cor_idx_scl)
{
    uint32 rval = IK_OK;
    if (fstshpns_cor_idx_scl == NULL) {
        amba_ik_system_print_str_5("[IK] detect first_sharpen_coring_idx_scale get NULL pointer!", NULL, NULL, NULL, NULL, NULL);
        rval |= IK_ERR_0005;
    } else {
        rval |= ctx_check_uint32_parameter_valid_range("first_sharpen_coring_idx_scale->low", fstshpns_cor_idx_scl->low, 0U, 253U);
        rval |= ctx_check_uint32_parameter_valid_range("first_sharpen_coring_idx_scale->low_delta", fstshpns_cor_idx_scl->low_delta, 0U, 7U);
        rval |= ctx_check_uint32_parameter_valid_range("first_sharpen_coring_idx_scale->high_strength", fstshpns_cor_idx_scl->low_strength, 0U, 255U);
        rval |= ctx_check_uint32_parameter_valid_range("first_sharpen_coring_idx_scale->high_strength", fstshpns_cor_idx_scl->mid_strength, 0U, 255U);
        rval |= ctx_check_uint32_parameter_valid_range("first_sharpen_coring_idx_scale->high", fstshpns_cor_idx_scl->high, 1, 254);
        rval |= ctx_check_uint32_parameter_valid_range("first_sharpen_coring_idx_scale->high_delta", fstshpns_cor_idx_scl->high_delta, 0U, 7U);
        rval |= ctx_check_uint32_parameter_valid_range("first_sharpen_coring_idx_scale->high_strength", fstshpns_cor_idx_scl->high_strength, 0U, 255U);
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

static uint32 ctx_check_fstshpns_min_coring_result_valid_range(const ik_first_sharpen_min_coring_result_t *fstshpns_min_cor_rst)
{
    uint32 rval = IK_OK;
    if (fstshpns_min_cor_rst == NULL) {
        amba_ik_system_print_str_5("[IK] detect first_sharpen_min_coring_result get NULL pointer!", NULL, NULL, NULL, NULL, NULL);
        rval |= IK_ERR_0005;
    } else {
        rval |= ctx_check_uint32_parameter_valid_range("first_sharpen_min_coring_result->low", fstshpns_min_cor_rst->low, 0U, 253U);
        rval |= ctx_check_uint32_parameter_valid_range("first_sharpen_min_coring_result->low_delta", fstshpns_min_cor_rst->low_delta, 0U, 7U);
        rval |= ctx_check_uint32_parameter_valid_range("first_sharpen_min_coring_result->low_strength", fstshpns_min_cor_rst->low_strength, 0U, 255U);
        rval |= ctx_check_uint32_parameter_valid_range("first_sharpen_min_coring_result->mid_strength", fstshpns_min_cor_rst->mid_strength, 0U, 255U);
        rval |= ctx_check_uint32_parameter_valid_range("first_sharpen_min_coring_result->high", fstshpns_min_cor_rst->high, 1, 254);
        rval |= ctx_check_uint32_parameter_valid_range("first_sharpen_min_coring_result->high_delta", fstshpns_min_cor_rst->high_delta, 0U, 7U);
        rval |= ctx_check_uint32_parameter_valid_range("first_sharpen_min_coring_result->high_strength", fstshpns_min_cor_rst->high_strength, 0U, 255U);
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

static uint32 ctx_check_fstshpns_max_coring_result_valid_range(const ik_first_sharpen_max_coring_result_t *fstshpns_max_cor_rst)
{
    uint32 rval = IK_OK;
    if (fstshpns_max_cor_rst == NULL) {
        amba_ik_system_print_str_5("[IK] detect first_sharpen_max_coring_result get NULL pointer!", NULL, NULL, NULL, NULL, NULL);
        rval |= IK_ERR_0005;
    } else {
        rval |= ctx_check_uint32_parameter_valid_range("first_sharpen_max_coring_result->low", fstshpns_max_cor_rst->low, 0U, 253U);
        rval |= ctx_check_uint32_parameter_valid_range("first_sharpen_max_coring_result->low_delta", fstshpns_max_cor_rst->low_delta, 0U, 7U);
        rval |= ctx_check_uint32_parameter_valid_range("first_sharpen_max_coring_result->low_strength", fstshpns_max_cor_rst->low_strength, 0U, 255U);
        rval |= ctx_check_uint32_parameter_valid_range("first_sharpen_max_coring_result->mid_strength", fstshpns_max_cor_rst->mid_strength, 0U, 255U);
        rval |= ctx_check_uint32_parameter_valid_range("first_sharpen_max_coring_result->high", fstshpns_max_cor_rst->high, 1, 254);
        rval |= ctx_check_uint32_parameter_valid_range("first_sharpen_max_coring_result->high_delta", fstshpns_max_cor_rst->high_delta, 0U, 7U);
        rval |= ctx_check_uint32_parameter_valid_range("first_sharpen_max_coring_result->high_strength", fstshpns_max_cor_rst->high_strength, 0U, 255U);
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

static uint32 ctx_check_fstshpns_scale_coring_valid_range(const ik_first_sharpen_scale_coring_t *fstshpns_scl_cor)
{
    uint32 rval = IK_OK;
    if (fstshpns_scl_cor == NULL) {
        amba_ik_system_print_str_5("[IK] detect first_sharpen_scale_coring get NULL pointer!", NULL, NULL, NULL, NULL, NULL);
        rval |= IK_ERR_0005;
    } else {
        rval |= ctx_check_uint32_parameter_valid_range("first_sharpen_scale_coring->low", fstshpns_scl_cor->low, 0U, 253U);
        rval |= ctx_check_uint32_parameter_valid_range("first_sharpen_scale_coring->low_delta", fstshpns_scl_cor->low_delta, 0U, 7U);
        rval |= ctx_check_uint32_parameter_valid_range("first_sharpen_scale_coring->low_strength", fstshpns_scl_cor->low_strength, 0U, 255U);
        rval |= ctx_check_uint32_parameter_valid_range("first_sharpen_scale_coring->mid_strength", fstshpns_scl_cor->mid_strength, 0U, 255U);
        rval |= ctx_check_uint32_parameter_valid_range("first_sharpen_scale_coring->high", fstshpns_scl_cor->high, 1, 254);
        rval |= ctx_check_uint32_parameter_valid_range("first_sharpen_scale_coring->high_delta", fstshpns_scl_cor->high_delta, 0U, 7U);
        rval |= ctx_check_uint32_parameter_valid_range("first_sharpen_scale_coring->high_strength", fstshpns_scl_cor->high_strength, 0U, 255U);
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

static uint32 ctx_check_video_mctf_level_control_valid_range(const ik_level_method_t *p_mctf_level)
{
    uint32 rval = IK_OK;
    if (p_mctf_level == NULL) {
        rval |= IK_ERR_0005;
    } else {
        //method 0,1,2,7,8,9,10,12,14
        if ((p_mctf_level->method != 0U) && (p_mctf_level->method != 1U) && (p_mctf_level->method != 2U) && \
            (p_mctf_level->method != 7U) && (p_mctf_level->method != 8U) && (p_mctf_level->method != 9U) && \
            (p_mctf_level->method != 10U) && (p_mctf_level->method != 12U) && (p_mctf_level->method != 14U)) {
            amba_ik_system_print_uint32_5("p_mctf_level->method %d not valid",p_mctf_level->method, DC_U, DC_U, DC_U, DC_U);
            rval |= IK_ERR_0008;
        }

        rval |= ctx_check_uint32_parameter_valid_range("p_mctf_level->high", p_mctf_level->high, 1U, 254U);
        rval |= ctx_check_uint32_parameter_valid_range("p_mctf_level->low", p_mctf_level->low, 0U, 253U);
        rval |= ctx_check_uint32_parameter_valid_range("p_mctf_level->high_delta", p_mctf_level->high_delta, 0U, 7U);
        rval |= ctx_check_uint32_parameter_valid_range("p_mctf_level->low_delta", p_mctf_level->low_delta, 0U, 7U);
        rval |= ctx_check_uint32_parameter_valid_range("p_mctf_level->low_strength", p_mctf_level->low_strength, 0U, 255U);
        rval |= ctx_check_uint32_parameter_valid_range("p_mctf_level->mid_strength", p_mctf_level->mid_strength, 0U, 255U);
        rval |= ctx_check_uint32_parameter_valid_range("p_mctf_level->high_strength", p_mctf_level->high_strength, 0U, 255U);
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

static uint32 ctx_check_video_mctf_valid_range(const ik_video_mctf_t *p_mctf_level)
{
    uint32 rval = IK_OK;
    uint8 tmp_uint8;
    if (p_mctf_level == NULL) {
        rval |= IK_ERR_0005;
    } else {
        if(p_mctf_level->enable == 1U) {
            rval |= ctx_check_uint32_parameter_valid_range("ik_video_mctf_t.enable", p_mctf_level->enable, 0U, 1U);
            rval |= ctx_check_uint32_parameter_valid_range("ik_video_mctf_t.y_narrow_nonsmooth_detect_shift", p_mctf_level->y_narrow_nonsmooth_detect_shift, 0U, 5U);
            rval |= ctx_check_uint32_parameter_valid_range("ik_video_mctf_t.use_level_based_ta", p_mctf_level->use_level_based_ta, 0U, 1U);
            rval |= ctx_check_uint32_parameter_valid_range("ik_video_mctf_t.compression_enable", p_mctf_level->compression_enable, 0U, 1U);

            rval |= ctx_check_video_mctf_level_control_valid_range(&p_mctf_level->y_3d_maxchange);
            if(rval != IK_OK) {
                amba_ik_system_print_str_5("[IK][ERROR] check ik_video_mctf_t->y_3d_maxchange fail \n", NULL, NULL, NULL, NULL, NULL);
            }
            rval |= ctx_check_video_mctf_level_control_valid_range(&p_mctf_level->cb_3d_maxchange);
            if(rval != IK_OK) {
                amba_ik_system_print_str_5("[IK][ERROR] check ik_video_mctf_t->cb_3d_maxchange fail \n", NULL, NULL, NULL, NULL, NULL);
            }
            rval |= ctx_check_video_mctf_level_control_valid_range(&p_mctf_level->cr_3d_maxchange);
            if(rval != IK_OK) {
                amba_ik_system_print_str_5("[IK][ERROR] check ik_video_mctf_t->cr_3d_maxchange fail \n", NULL, NULL, NULL, NULL, NULL);
            }

            rval |= ctx_check_uint32_parameter_valid_range("ik_video_mctf_t.y_advanced_iso.enable", p_mctf_level->y_advanced_iso.enable, 0U, 1U);
            tmp_uint8 = (uint8)p_mctf_level->y_advanced_iso.max_change_method;
            if ((tmp_uint8 != 0U) && (tmp_uint8 != 1U) && (tmp_uint8 != 2U) && \
                (tmp_uint8 != 7U) && (tmp_uint8 != 8U) && (tmp_uint8 != 9U) && \
                (tmp_uint8 != 10U) && (tmp_uint8 != 12U) && (tmp_uint8 != 14U)) {
                amba_ik_system_print_uint32_5("p_mctf_level->y_advanced_iso.max_change_method %d not valid",tmp_uint8, DC_U, DC_U, DC_U, DC_U);
                rval |= IK_ERR_0008;
            }
            rval |= ctx_check_uint32_parameter_valid_range("ik_video_mctf_t.y_advanced_iso.noise_level", p_mctf_level->y_advanced_iso.noise_level, 0U, 255U);
            rval |= ctx_check_uint32_parameter_valid_range("ik_video_mctf_t.y_advanced_iso.size", p_mctf_level->y_advanced_iso.size, 0U, 1U);
            rval |= ctx_check_uint32_parameter_valid_range("ik_video_mctf_t.cb_advanced_iso.enable", p_mctf_level->cb_advanced_iso.enable, 0U, 1U);
            tmp_uint8 = (uint8)p_mctf_level->cb_advanced_iso.max_change_method;
            if ((tmp_uint8 != 0U) && (tmp_uint8 != 1U) && (tmp_uint8 != 2U) && \
                (tmp_uint8 != 7U) && (tmp_uint8 != 8U) && (tmp_uint8 != 9U) && \
                (tmp_uint8 != 10U) && (tmp_uint8 != 12U) && (tmp_uint8 != 14U)) {
                amba_ik_system_print_uint32_5("p_mctf_level->cb_advanced_iso.max_change_method %d not valid",tmp_uint8, DC_U, DC_U, DC_U, DC_U);
                rval |= IK_ERR_0008;
            }
            rval |= ctx_check_uint32_parameter_valid_range("ik_video_mctf_t.cb_advanced_iso.noise_level", p_mctf_level->cb_advanced_iso.noise_level, 0U, 255U);
            rval |= ctx_check_uint32_parameter_valid_range("ik_video_mctf_t.cb_advanced_iso.size", p_mctf_level->cb_advanced_iso.size, 0U, 1U);
            rval |= ctx_check_uint32_parameter_valid_range("ik_video_mctf_t.cr_advanced_iso.enable", p_mctf_level->cr_advanced_iso.enable, 0U, 1U);
            tmp_uint8 = (uint8)p_mctf_level->cr_advanced_iso.max_change_method;
            if ((tmp_uint8 != 0U) && (tmp_uint8 != 1U) && (tmp_uint8 != 2U) && \
                (tmp_uint8 != 7U) && (tmp_uint8 != 8U) && (tmp_uint8 != 9U) && \
                (tmp_uint8 != 10U) && (tmp_uint8 != 12U) && (tmp_uint8 != 14U)) {
                amba_ik_system_print_uint32_5("p_mctf_level->cr_advanced_iso.max_change_method %d not valid",tmp_uint8, DC_U, DC_U, DC_U, DC_U);
                rval |= IK_ERR_0008;
            }
            rval |= ctx_check_uint32_parameter_valid_range("ik_video_mctf_t.cr_advanced_iso.noise_level", p_mctf_level->cr_advanced_iso.noise_level, 0U, 255U);
            rval |= ctx_check_uint32_parameter_valid_range("ik_video_mctf_t.cr_advanced_iso.size", p_mctf_level->cr_advanced_iso.size, 0U, 1U);

            rval |= ctx_check_video_mctf_level_control_valid_range(&p_mctf_level->y_level_based_ta);
            if(rval != IK_OK) {
                amba_ik_system_print_str_5("[IK][ERROR] check ik_video_mctf_t->y_level_based_ta fail \n", NULL, NULL, NULL, NULL, NULL);
            }
            rval |= ctx_check_video_mctf_level_control_valid_range(&p_mctf_level->cb_level_based_ta);
            if(rval != IK_OK) {
                amba_ik_system_print_str_5("[IK][ERROR] check ik_video_mctf_t->cb_level_based_ta fail \n", NULL, NULL, NULL, NULL, NULL);
            }
            rval |= ctx_check_video_mctf_level_control_valid_range(&p_mctf_level->cr_level_based_ta);
            if(rval != IK_OK) {
                amba_ik_system_print_str_5("[IK][ERROR] check ik_video_mctf_t->cr_level_based_ta fail \n", NULL, NULL, NULL, NULL, NULL);
            }

            rval |= ctx_check_video_mctf_level_control_valid_range(&p_mctf_level->y_overall_max_change);
            if(rval != IK_OK) {
                amba_ik_system_print_str_5("[IK][ERROR] check ik_video_mctf_t->y_overall_max_change fail \n", NULL, NULL, NULL, NULL, NULL);
            }
            rval |= ctx_check_video_mctf_level_control_valid_range(&p_mctf_level->cb_overall_max_change);
            if(rval != IK_OK) {
                amba_ik_system_print_str_5("[IK][ERROR] check ik_video_mctf_t->cb_overall_max_change fail \n", NULL, NULL, NULL, NULL, NULL);
            }
            rval |= ctx_check_video_mctf_level_control_valid_range(&p_mctf_level->cr_overall_max_change);
            if(rval != IK_OK) {
                amba_ik_system_print_str_5("[IK][ERROR] check ik_video_mctf_t->cr_3d_maxchange fail \n", NULL, NULL, NULL, NULL, NULL);
            }

            rval |= ctx_check_video_mctf_level_control_valid_range(&p_mctf_level->y_spat_blend);
            if(rval != IK_OK) {
                amba_ik_system_print_str_5("[IK][ERROR] check ik_video_mctf_t->y_spat_blend fail \n", NULL, NULL, NULL, NULL, NULL);
            }
            rval |= ctx_check_video_mctf_level_control_valid_range(&p_mctf_level->cb_spat_blend);
            if(rval != IK_OK) {
                amba_ik_system_print_str_5("[IK][ERROR] check ik_video_mctf_t->cb_spat_blend fail \n", NULL, NULL, NULL, NULL, NULL);
            }
            rval |= ctx_check_video_mctf_level_control_valid_range(&p_mctf_level->cr_spat_blend);
            if(rval != IK_OK) {
                amba_ik_system_print_str_5("[IK][ERROR] check ik_video_mctf_t->cr_spat_blend fail \n", NULL, NULL, NULL, NULL, NULL);
            }

            rval |= ctx_check_video_mctf_level_control_valid_range(&p_mctf_level->y_spat_filt_max_smth_change);
            if(rval != IK_OK) {
                amba_ik_system_print_str_5("[IK][ERROR] check ik_video_mctf_t->y_spat_filt_max_smth_change fail \n", NULL, NULL, NULL, NULL, NULL);
            }
            rval |= ctx_check_video_mctf_level_control_valid_range(&p_mctf_level->cb_spat_filt_max_smth_change);
            if(rval != IK_OK) {
                amba_ik_system_print_str_5("[IK][ERROR] check ik_video_mctf_t->cb_spat_filt_max_smth_change fail \n", NULL, NULL, NULL, NULL, NULL);
            }
            rval |= ctx_check_video_mctf_level_control_valid_range(&p_mctf_level->cr_spat_filt_max_smth_change);
            if(rval != IK_OK) {
                amba_ik_system_print_str_5("[IK][ERROR] check ik_video_mctf_t->cr_spat_filt_max_smth_change fail \n", NULL, NULL, NULL, NULL, NULL);
            }

            rval |= ctx_check_uint32_parameter_valid_range("ik_video_mctf_t.y_spat_smth_wide_edge_detect", p_mctf_level->y_spat_smth_wide_edge_detect, 0U, 8U);
            rval |= ctx_check_uint32_parameter_valid_range("ik_video_mctf_t.y_spat_smth_edge_thresh", p_mctf_level->y_spat_smth_edge_thresh, 0U, 2047U);
            rval |= ctx_check_video_mctf_level_control_valid_range(&p_mctf_level->y_spat_smth_dir);
            if(rval != IK_OK) {
                amba_ik_system_print_str_5("[IK][ERROR] check ik_video_mctf_t->y_spat_smth_dir fail \n", NULL, NULL, NULL, NULL, NULL);
            }
            rval |= ctx_check_uint32_parameter_valid_range("ik_video_mctf_t.cb_spat_smth_wide_edge_detect", p_mctf_level->cb_spat_smth_wide_edge_detect, 0U, 8U);
            rval |= ctx_check_uint32_parameter_valid_range("ik_video_mctf_t.cb_spat_smth_edge_thresh", p_mctf_level->cb_spat_smth_edge_thresh, 0U, 2047U);
            rval |= ctx_check_video_mctf_level_control_valid_range(&p_mctf_level->cb_spat_smth_dir);
            if(rval != IK_OK) {
                amba_ik_system_print_str_5("[IK][ERROR] check ik_video_mctf_t->cb_spat_smth_dir fail \n", NULL, NULL, NULL, NULL, NULL);
            }
            rval |= ctx_check_uint32_parameter_valid_range("ik_video_mctf_t.cr_spat_smth_wide_edge_detect", p_mctf_level->cr_spat_smth_wide_edge_detect, 0U, 8U);
            rval |= ctx_check_uint32_parameter_valid_range("ik_video_mctf_t.cr_spat_smth_edge_thresh", p_mctf_level->cr_spat_smth_edge_thresh, 0U, 2047U);
            rval |= ctx_check_video_mctf_level_control_valid_range(&p_mctf_level->cr_spat_smth_dir);
            if(rval != IK_OK) {
                amba_ik_system_print_str_5("[IK][ERROR] check ik_video_mctf_t->cr_spat_smth_dir fail \n", NULL, NULL, NULL, NULL, NULL);
            }

            rval |= ctx_check_video_mctf_level_control_valid_range(&p_mctf_level->y_spat_smth_iso);
            if(rval != IK_OK) {
                amba_ik_system_print_str_5("[IK][ERROR] check ik_video_mctf_t->y_spat_smth_iso fail \n", NULL, NULL, NULL, NULL, NULL);
            }
            rval |= ctx_check_video_mctf_level_control_valid_range(&p_mctf_level->cb_spat_smth_iso);
            if(rval != IK_OK) {
                amba_ik_system_print_str_5("[IK][ERROR] check ik_video_mctf_t->cb_spat_smth_iso fail \n", NULL, NULL, NULL, NULL, NULL);
            }
            rval |= ctx_check_video_mctf_level_control_valid_range(&p_mctf_level->cr_spat_smth_iso);
            if(rval != IK_OK) {
                amba_ik_system_print_str_5("[IK][ERROR] check ik_video_mctf_t->cr_spat_smth_iso fail \n", NULL, NULL, NULL, NULL, NULL);
            }

            rval |= ctx_check_uint32_parameter_valid_range("ik_video_mctf_t.y_spat_smth_direct_decide_t0", p_mctf_level->y_spat_smth_direct_decide_t0, 0U, 255U);
            rval |= ctx_check_uint32_parameter_valid_range("ik_video_mctf_t.y_spat_smth_direct_decide_t1", p_mctf_level->y_spat_smth_direct_decide_t1, 0U, 255U);
            rval |= ctx_check_uint32_parameter_valid_range("ik_video_mctf_t.cb_spat_smth_direct_decide_t0", p_mctf_level->cb_spat_smth_direct_decide_t0, 0U, 255U);
            rval |= ctx_check_uint32_parameter_valid_range("ik_video_mctf_t.cb_spat_smth_direct_decide_t1", p_mctf_level->cb_spat_smth_direct_decide_t1, 0U, 255U);
            rval |= ctx_check_uint32_parameter_valid_range("ik_video_mctf_t.cr_spat_smth_direct_decide_t0", p_mctf_level->cr_spat_smth_direct_decide_t0, 0U, 255U);
            rval |= ctx_check_uint32_parameter_valid_range("ik_video_mctf_t.cr_spat_smth_direct_decide_t1", p_mctf_level->cr_spat_smth_direct_decide_t1, 0U, 255U);

            rval |= ctx_check_uint32_parameter_valid_range("ik_video_mctf_t.y_spatial_max_change", p_mctf_level->y_spatial_max_change, 0U, 255U);
            rval |= ctx_check_video_mctf_level_control_valid_range(&p_mctf_level->y_spatial_max_temporal);
            if(rval != IK_OK) {
                amba_ik_system_print_str_5("[IK][ERROR] check ik_video_mctf_t->y_spatial_max_temporal fail \n", NULL, NULL, NULL, NULL, NULL);
            }
            rval |= ctx_check_uint32_parameter_valid_range("ik_video_mctf_t.cb_spatial_max_change", p_mctf_level->cb_spatial_max_change, 0U, 255U);
            rval |= ctx_check_video_mctf_level_control_valid_range(&p_mctf_level->cb_spatial_max_temporal);
            if(rval != IK_OK) {
                amba_ik_system_print_str_5("[IK][ERROR] check ik_video_mctf_t->cb_spatial_max_temporal fail \n", NULL, NULL, NULL, NULL, NULL);
            }
            rval |= ctx_check_uint32_parameter_valid_range("ik_video_mctf_t.cr_spatial_max_change", p_mctf_level->cr_spatial_max_change, 0U, 255U);
            rval |= ctx_check_video_mctf_level_control_valid_range(&p_mctf_level->cr_spatial_max_temporal);
            if(rval != IK_OK) {
                amba_ik_system_print_str_5("[IK][ERROR] check ik_video_mctf_t->cr_spatial_max_temporal fail \n", NULL, NULL, NULL, NULL, NULL);
            }

            rval |= ctx_check_uint32_parameter_valid_range("ik_video_mctf_t.y_curve.temporal_alpha0", p_mctf_level->y_curve.temporal_alpha0, 0U, 255U);
            rval |= ctx_check_uint32_parameter_valid_range("ik_video_mctf_t.y_curve.temporal_alpha1", p_mctf_level->y_curve.temporal_alpha1, 0U, 255U);
            rval |= ctx_check_uint32_parameter_valid_range("ik_video_mctf_t.y_curve.temporal_alpha2", p_mctf_level->y_curve.temporal_alpha2, 0U, 255U);
            rval |= ctx_check_uint32_parameter_valid_range("ik_video_mctf_t.y_curve.temporal_alpha3", p_mctf_level->y_curve.temporal_alpha3, 0U, 255U);
            rval |= ctx_check_uint32_parameter_valid_range("ik_video_mctf_t.y_curve.temporal_t0", p_mctf_level->y_curve.temporal_t0, 0U, 63U);
            rval |= ctx_check_uint32_parameter_valid_range("ik_video_mctf_t.y_curve.temporal_t1", p_mctf_level->y_curve.temporal_t1, 0U, 63U);
            rval |= ctx_check_uint32_parameter_valid_range("ik_video_mctf_t.y_curve.temporal_t2", p_mctf_level->y_curve.temporal_t2, 0U, 63U);
            rval |= ctx_check_uint32_parameter_valid_range("ik_video_mctf_t.y_curve.temporal_t3", p_mctf_level->y_curve.temporal_t3, 0U, 63U);
            rval |= ctx_check_uint32_parameter_valid_range("ik_video_mctf_t.y_temporal_max_change", p_mctf_level->y_temporal_max_change, 0U, 255U);
            rval |= ctx_check_video_mctf_level_control_valid_range(&p_mctf_level->y_temporal_min_target);
            if(rval != IK_OK) {
                amba_ik_system_print_str_5("[IK][ERROR] check ik_video_mctf_t->y_temporal_min_target fail \n", NULL, NULL, NULL, NULL, NULL);
            }

            rval |= ctx_check_uint32_parameter_valid_range("ik_video_mctf_t.cb_curve.temporal_alpha0", p_mctf_level->cb_curve.temporal_alpha0, 0U, 255U);
            rval |= ctx_check_uint32_parameter_valid_range("ik_video_mctf_t.cb_curve.temporal_alpha1", p_mctf_level->cb_curve.temporal_alpha1, 0U, 255U);
            rval |= ctx_check_uint32_parameter_valid_range("ik_video_mctf_t.cb_curve.temporal_alpha2", p_mctf_level->cb_curve.temporal_alpha2, 0U, 255U);
            rval |= ctx_check_uint32_parameter_valid_range("ik_video_mctf_t.cb_curve.temporal_alpha3", p_mctf_level->cb_curve.temporal_alpha3, 0U, 255U);
            rval |= ctx_check_uint32_parameter_valid_range("ik_video_mctf_t.cb_curve.temporal_t0", p_mctf_level->cb_curve.temporal_t0, 0U, 63U);
            rval |= ctx_check_uint32_parameter_valid_range("ik_video_mctf_t.cb_curve.temporal_t1", p_mctf_level->cb_curve.temporal_t1, 0U, 63U);
            rval |= ctx_check_uint32_parameter_valid_range("ik_video_mctf_t.cb_curve.temporal_t2", p_mctf_level->cb_curve.temporal_t2, 0U, 63U);
            rval |= ctx_check_uint32_parameter_valid_range("ik_video_mctf_t.cb_curve.temporal_t3", p_mctf_level->cb_curve.temporal_t3, 0U, 63U);
            rval |= ctx_check_uint32_parameter_valid_range("ik_video_mctf_t.cb_temporal_max_change", p_mctf_level->cb_temporal_max_change, 0U, 255U);
            rval |= ctx_check_video_mctf_level_control_valid_range(&p_mctf_level->cb_temporal_min_target);
            if(rval != IK_OK) {
                amba_ik_system_print_str_5("[IK][ERROR] check ik_video_mctf_t->cb_temporal_min_target fail \n", NULL, NULL, NULL, NULL, NULL);
            }

            rval |= ctx_check_uint32_parameter_valid_range("ik_video_mctf_t.cr_curve.temporal_alpha0", p_mctf_level->cr_curve.temporal_alpha0, 0U, 255U);
            rval |= ctx_check_uint32_parameter_valid_range("ik_video_mctf_t.cr_curve.temporal_alpha1", p_mctf_level->cr_curve.temporal_alpha1, 0U, 255U);
            rval |= ctx_check_uint32_parameter_valid_range("ik_video_mctf_t.cr_curve.temporal_alpha2", p_mctf_level->cr_curve.temporal_alpha2, 0U, 255U);
            rval |= ctx_check_uint32_parameter_valid_range("ik_video_mctf_t.cr_curve.temporal_alpha3", p_mctf_level->cr_curve.temporal_alpha3, 0U, 255U);
            rval |= ctx_check_uint32_parameter_valid_range("ik_video_mctf_t.cr_curve.temporal_t0", p_mctf_level->cr_curve.temporal_t0, 0U, 63U);
            rval |= ctx_check_uint32_parameter_valid_range("ik_video_mctf_t.cr_curve.temporal_t1", p_mctf_level->cr_curve.temporal_t1, 0U, 63U);
            rval |= ctx_check_uint32_parameter_valid_range("ik_video_mctf_t.cr_curve.temporal_t2", p_mctf_level->cr_curve.temporal_t2, 0U, 63U);
            rval |= ctx_check_uint32_parameter_valid_range("ik_video_mctf_t.cr_curve.temporal_t3", p_mctf_level->cr_curve.temporal_t3, 0U, 63U);
            rval |= ctx_check_uint32_parameter_valid_range("ik_video_mctf_t.cr_temporal_max_change", p_mctf_level->cr_temporal_max_change, 0U, 255U);
            rval |= ctx_check_video_mctf_level_control_valid_range(&p_mctf_level->cr_temporal_min_target);
            if(rval != IK_OK) {
                amba_ik_system_print_str_5("[IK][ERROR] check ik_video_mctf_t->cr_temporal_min_target fail \n", NULL, NULL, NULL, NULL, NULL);
            }

            rval |= ctx_check_uint32_parameter_valid_range("ik_video_mctf_t.y_temporal_ghost_prevent", p_mctf_level->y_temporal_ghost_prevent, 0U, 255U);
            rval |= ctx_check_uint32_parameter_valid_range("ik_video_mctf_t.cb_temporal_ghost_prevent", p_mctf_level->cb_temporal_ghost_prevent, 0U, 255U);
            rval |= ctx_check_uint32_parameter_valid_range("ik_video_mctf_t.cr_temporal_ghost_prevent", p_mctf_level->cr_temporal_ghost_prevent, 0U, 255U);

            rval |= ctx_check_uint32_parameter_valid_range("ik_video_mctf_t.y_temporal_artifact_guard", p_mctf_level->y_temporal_artifact_guard, 0U, 255U);
            rval |= ctx_check_uint32_parameter_valid_range("ik_video_mctf_t.cb_temporal_artifact_guard", p_mctf_level->cb_temporal_artifact_guard, 0U, 255U);
            rval |= ctx_check_uint32_parameter_valid_range("ik_video_mctf_t.cr_temporal_artifact_guard", p_mctf_level->cr_temporal_artifact_guard, 0U, 255U);

            rval |= ctx_check_uint32_parameter_valid_range("ik_video_mctf_t.y_combine_strength", p_mctf_level->y_combine_strength, 0U, 256U);
            rval |= ctx_check_uint32_parameter_valid_range("ik_video_mctf_t.y_strength_3d", p_mctf_level->y_strength_3d, 0U, 256U);
            rval |= ctx_check_uint32_parameter_valid_range("ik_video_mctf_t.cb_strength_3d", p_mctf_level->cb_strength_3d, 0U, 256U);
            rval |= ctx_check_uint32_parameter_valid_range("ik_video_mctf_t.cr_strength_3d", p_mctf_level->cr_strength_3d, 0U, 256U);


            rval |= ctx_check_uint32_parameter_valid_range("ik_video_mctf_t.y_temporal_max_change_not_t0_t1_level_based", \
                    p_mctf_level->y_temporal_max_change_not_t0_t1_level_based, 0U, 1U);
            rval |= ctx_check_uint32_parameter_valid_range("ik_video_mctf_t.y_temporal_max_change_not_t0_t1_level_based", \
                    p_mctf_level->cb_temporal_max_change_not_t0_t1_level_based, 0U, 1U);
            rval |= ctx_check_uint32_parameter_valid_range("ik_video_mctf_t.y_temporal_max_change_not_t0_t1_level_based", \
                    p_mctf_level->cr_temporal_max_change_not_t0_t1_level_based, 0U, 1U);

            rval |= ctx_check_video_mctf_level_control_valid_range(&p_mctf_level->y_temporal_either_max_change_or_t0_t1_add);
            if(rval != IK_OK) {
                amba_ik_system_print_str_5("[IK][ERROR] check ik_video_mctf_t->y_temporal_either_max_change_or_t0_t1_add fail \n", NULL, NULL, NULL, NULL, NULL);
            }
            rval |= ctx_check_video_mctf_level_control_valid_range(&p_mctf_level->cb_temporal_either_max_change_or_t0_t1_add);
            if(rval != IK_OK) {
                amba_ik_system_print_str_5("[IK][ERROR] check ik_video_mctf_t->cb_temporal_either_max_change_or_t0_t1_add fail \n", NULL, NULL, NULL, NULL, NULL);
            }
            rval |= ctx_check_video_mctf_level_control_valid_range(&p_mctf_level->cr_temporal_either_max_change_or_t0_t1_add);
            if(rval != IK_OK) {
                amba_ik_system_print_str_5("[IK][ERROR] check ik_video_mctf_t->cr_temporal_either_max_change_or_t0_t1_add fail \n", NULL, NULL, NULL, NULL, NULL);
            }

            if(p_mctf_level->compression_enable == 1U) {
                rval |= ctx_check_uint32_parameter_valid_range("ik_video_mctf_t.compression_bit_rate_luma", p_mctf_level->compression_bit_rate_luma, 32U, 32U);
                rval |= ctx_check_uint32_parameter_valid_range("ik_video_mctf_t.compression_bit_rate_chroma", p_mctf_level->compression_bit_rate_chroma, 20U, 20U);
            }
            rval |= ctx_check_uint32_parameter_valid_range("ik_video_mctf_t.compression_dither_disable", p_mctf_level->compression_dither_disable, 0U, 1U);
        }
    }
    return rval;
}

static uint32 ctx_check_video_mctf_ta_valid_range(const ik_video_mctf_ta_t *video_mctf_ta)
{
    uint32 rval = IK_OK;
    if (video_mctf_ta == NULL) {
        rval |= IK_ERR_0005;
    } else {
        rval |= (uint32)ctx_check_uint32_parameter_valid_range("p_mctf_info_ta->slow_mo_sensitivity", video_mctf_ta->slow_mo_sensitivity, 0, 11);
        rval |= (uint32)ctx_check_uint32_parameter_valid_range("p_mctf_info_ta->score_noise_robust", video_mctf_ta->score_noise_robust, 0, 1);
        rval |= (uint32)ctx_check_uint32_parameter_valid_range("p_mctf_info_ta->lev_adjust_high_delta", video_mctf_ta->lev_adjust_high_delta, 0U, 7U);
        rval |= (uint32)ctx_check_uint32_parameter_valid_range("p_mctf_info_ta->slow_mo_sensitivity", video_mctf_ta->lev_adjust_low_delta, 0U, 7U);
        rval |= (uint32)ctx_check_uint32_parameter_valid_range("p_mctf_info_ta->y_mctf_ta.max", video_mctf_ta->y_mctf_ta.max, 0, 15);
        rval |= (uint32)ctx_check_uint32_parameter_valid_range("p_mctf_info_ta->y_mctf_ta.min", video_mctf_ta->y_mctf_ta.min, 0, 15);
        rval |= (uint32)ctx_check_uint32_parameter_valid_range("p_mctf_info_ta->y_mctf_ta.motion_response", video_mctf_ta->y_mctf_ta.motion_response, 0, 16383);
        rval |= (uint32)ctx_check_uint32_parameter_valid_range("p_mctf_info_ta->y_mctf_ta.noise_base", video_mctf_ta->y_mctf_ta.noise_base, 0, 4095);
        rval |= (uint32)ctx_check_uint32_parameter_valid_range("p_mctf_info_ta->y_mctf_ta.still_thres", video_mctf_ta->y_mctf_ta.still_thresh, 0, 4095);
        rval |= (uint32)ctx_check_uint32_parameter_valid_range("p_mctf_info_ta->cb_mctf_ta.max", video_mctf_ta->cb_mctf_ta.max, 0, 15);
        rval |= (uint32)ctx_check_uint32_parameter_valid_range("p_mctf_info_ta->cb_mctf_ta.min", video_mctf_ta->cb_mctf_ta.min, 0, 15);
        rval |= (uint32)ctx_check_uint32_parameter_valid_range("p_mctf_info_ta->cb_mctf_ta.motion_response", video_mctf_ta->cb_mctf_ta.motion_response, 0, 16383);
        rval |= (uint32)ctx_check_uint32_parameter_valid_range("p_mctf_info_ta->cb_mctf_ta.noise_base", video_mctf_ta->cb_mctf_ta.noise_base, 0, 4095);
        rval |= (uint32)ctx_check_uint32_parameter_valid_range("p_mctf_info_ta->cb_mctf_ta.still_thres", video_mctf_ta->cb_mctf_ta.still_thresh, 0, 4095);
        rval |= (uint32)ctx_check_uint32_parameter_valid_range("p_mctf_info_ta->cr_mctf_ta.max", video_mctf_ta->cr_mctf_ta.max, 0, 15);
        rval |= (uint32)ctx_check_uint32_parameter_valid_range("p_mctf_info_ta->cr_mctf_ta.min", video_mctf_ta->cr_mctf_ta.min, 0, 15);
        rval |= (uint32)ctx_check_uint32_parameter_valid_range("p_mctf_info_ta->cr_mctf_ta.motion_response", video_mctf_ta->cr_mctf_ta.motion_response, 0, 16383);
        rval |= (uint32)ctx_check_uint32_parameter_valid_range("p_mctf_info_ta->cr_mctf_ta.noise_base", video_mctf_ta->cr_mctf_ta.noise_base, 0, 4095);
        rval |= (uint32)ctx_check_uint32_parameter_valid_range("p_mctf_info_ta->cr_mctf_ta.still_thres", video_mctf_ta->cr_mctf_ta.still_thresh, 0, 4095);
        rval |= (uint32)ctx_check_uint32_parameter_valid_range("p_mctf_info_ta->edge_adjust_b", video_mctf_ta->edge_adjust_b, 0, 2047);
        rval |= (uint32)ctx_check_uint32_parameter_valid_range("p_mctf_info_ta->edge_adjust_l", video_mctf_ta->edge_adjust_l, 0, 2047);
        rval |= (uint32)ctx_check_uint32_parameter_valid_range("p_mctf_info_ta->edge_adjust_r", video_mctf_ta->edge_adjust_r, 0, 2047);
        rval |= (uint32)ctx_check_uint32_parameter_valid_range("p_mctf_info_ta->edge_adjust_t", video_mctf_ta->edge_adjust_t, 0, 2047);
    }
    return rval;
}

static uint32 ctx_check_pos_dep33_valid_range(const ik_pos_dep33_t *video_mctf_and_fnlshp)
{
    uint32 rval = IK_OK;
    uint32 i, j;
    if (video_mctf_and_fnlshp == NULL) {
        rval |= IK_ERR_0005;
    } else {
        for (i = 0U; i < 33U; i++) {
            for (j = 0U; j < 33U; j++) {
                rval |= (uint32)ctx_check_uint32_parameter_valid_range("amba_ik_video_mctf_and_final_sharpen_t->pos_dep", video_mctf_and_fnlshp->pos_dep[i][j], 0U, 31U);
            }
        }
    }
    return rval;
}

static uint32 ctx_check_fnlshpns_both_valid_range(const ik_final_sharpen_both_t *final_sharpen_both)
{
    uint32 rval = IK_OK;
    if (final_sharpen_both == NULL) {
        amba_ik_system_print_str_5("[IK] detect final_sharpen_both get NULL pointer!", NULL, NULL, NULL, NULL, NULL);
        rval |= IK_ERR_0005;
    } else {
        rval |= ctx_check_uint32_parameter_valid_range("final_sharpen_both->enable", final_sharpen_both->enable, 0U, 1U);
        rval |= ctx_check_uint32_parameter_valid_range("final_sharpen_both->mode", final_sharpen_both->mode, 0U, 2U);
        if((final_sharpen_both->mode != 0U)&&(final_sharpen_both->mode != 2U)) {
            amba_ik_system_print_uint32_5("[IK] Error, parameter final_sharpen_both->mode=%d, should be 0 or 2", final_sharpen_both->mode, DC_U, DC_U, DC_U, DC_U);
            rval |= IK_ERR_0008;
        }
        rval |= ctx_check_uint32_parameter_valid_range("final_sharpen_both->edge_thresh", final_sharpen_both->edge_thresh, 0U, 2047U);
        rval |= ctx_check_uint32_parameter_valid_range("final_sharpen_both->wide_edge_detect", final_sharpen_both->wide_edge_detect, 0U, 8U);
        rval |= ctx_check_uint32_parameter_valid_range("final_sharpen_both->max_change.up", final_sharpen_both->max_change.up, 0U, 255U);
        rval |= ctx_check_uint32_parameter_valid_range("final_sharpen_both->max_change.up5x5", final_sharpen_both->max_change.up5x5, 0U, 255U);
        rval |= ctx_check_uint32_parameter_valid_range("final_sharpen_both->max_change.down", final_sharpen_both->max_change.down, 0U, 255U);
        rval |= ctx_check_uint32_parameter_valid_range("final_sharpen_both->max_change.down5x5", final_sharpen_both->max_change.down5x5, 0U, 255U);
        //cv2
        rval |= ctx_check_uint32_parameter_valid_range("final_sharpen_both->narrow_nonsmooth_detect_shift", final_sharpen_both->narrow_nonsmooth_detect_shift, 0U, 5U);
    }
    return rval;
}

static uint32 ctx_check_fnlshpns_noise_valid_range(const ik_final_sharpen_noise_t *final_sharpen_noise)
{
    uint32 rval = IK_OK;
    uint32  i = 0, j = 0;
    uint32 tmp;
    if (final_sharpen_noise == NULL) {
        amba_ik_system_print_str_5("[IK] detect final_sharpen_noise get NULL pointer!", NULL, NULL, NULL, NULL, NULL);
        rval |= IK_ERR_0005;
    } else {
        rval |= (uint32)ctx_check_uint32_parameter_valid_range("final_sharpen_noise->max_change_up", final_sharpen_noise->max_change_up, 0U, 255U);
        rval |= (uint32)ctx_check_uint32_parameter_valid_range("final_sharpen_noise->max_change_down", final_sharpen_noise->max_change_down, 0U, 255U);
        rval |= (uint32)ctx_check_uint32_parameter_valid_range("final_sharpen_noise->spatial_fir.specify", final_sharpen_noise->spatial_fir.specify, 0U, 4U);
        if (final_sharpen_noise->spatial_fir.specify == 0U) {
            rval |= (uint32)ctx_check_uint32_parameter_valid_range("final_sharpen_noise->spatial_fir.strength_iso", final_sharpen_noise->spatial_fir.strength_iso, 0U, 256U);
        } else if (final_sharpen_noise->spatial_fir.specify == 1U) {
            for (i = 0U; i < 25U; i++) {
                rval |= (uint32)ctx_check_int32_parameter_valid_range("final_sharpen_noise->spatial_fir.coefs", final_sharpen_noise->spatial_fir.coefs[0][i], -1024, 1023);
            }
        } else if (final_sharpen_noise->spatial_fir.specify == 2U) {
            rval |= (uint32)ctx_check_uint32_parameter_valid_range("final_sharpen_noise->spatial_fir.strength_iso", final_sharpen_noise->spatial_fir.strength_iso, 0U, 256U);
            rval |= (uint32)ctx_check_uint32_parameter_valid_range("final_sharpen_noise->spatial_fir.strength_dir", final_sharpen_noise->spatial_fir.strength_dir, 0U, 256U);
        } else if (final_sharpen_noise->spatial_fir.specify == 3U) {
            for (i = 0U; i < 9U; i++) {
                rval |= (uint32)ctx_check_uint32_parameter_valid_range("final_sharpen_noise->spatial_fir.per_dir_fir_dir_amounts", final_sharpen_noise->spatial_fir.per_dir_fir_dir_amounts[i], 0U, 256U);
                rval |= (uint32)ctx_check_uint32_parameter_valid_range("final_sharpen_noise->spatial_fir.per_dir_fir_dir_strengths", final_sharpen_noise->spatial_fir.per_dir_fir_dir_strengths[i], 0U, 256U);
                rval |= (uint32)ctx_check_uint32_parameter_valid_range("final_sharpen_noise->spatial_fir.per_dir_fir_iso_strengths", final_sharpen_noise->spatial_fir.per_dir_fir_iso_strengths[i], 0U, 256U);
            }
        } else if (final_sharpen_noise->spatial_fir.specify == 4U) {
            for (j = 0U; j < 9U; j++) {
                for (i = 0U; i < 25U; i++) {
                    rval |= (uint32)ctx_check_int32_parameter_valid_range("final_sharpen_noise->spatial_fir.coefs", final_sharpen_noise->spatial_fir.coefs[j][i], -1024, 1023);
                }
            }
        } else {
            rval |= IK_ERR_0008;
        }
        rval |= (uint32)ctx_check_uint32_parameter_valid_range("final_sharpen_noise->level_str_adjust.low", final_sharpen_noise->level_str_adjust.low, 0U, 253U);
        rval |= (uint32)ctx_check_uint32_parameter_valid_range("final_sharpen_noise->level_str_adjust.low_delta", final_sharpen_noise->level_str_adjust.low_delta, 0U, 7U);
        rval |= (uint32)ctx_check_uint32_parameter_valid_range("final_sharpen_noise->level_str_adjust.high", final_sharpen_noise->level_str_adjust.high, 1, 254U);
        rval |= (uint32)ctx_check_uint32_parameter_valid_range("final_sharpen_noise->level_str_adjust.high_delta", final_sharpen_noise->level_str_adjust.high_delta, 0U, 7U);
        rval |= (uint32)ctx_check_uint32_parameter_valid_range("final_sharpen_noise->level_str_adjust_not_t0_t1_level_based", final_sharpen_noise->level_str_adjust_not_t0_t1_level_based, 0U, 1U);
        rval |= (uint32)ctx_check_uint32_parameter_valid_range("final_sharpen_noise->t0", final_sharpen_noise->t0, 0U, 255U);
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
        rval |= (uint32)ctx_check_uint32_parameter_valid_range("final_sharpen_noise->t1", final_sharpen_noise->t1, 0U, 255U);
        rval |= (uint32)ctx_check_uint32_parameter_valid_range("final_sharpen_noise->alpha_min", final_sharpen_noise->alpha_min, 0U, 16U);
        rval |= (uint32)ctx_check_uint32_parameter_valid_range("final_sharpen_noise->alpha_max", final_sharpen_noise->alpha_max, 0U, 16U);
        if ((final_sharpen_noise->level_str_adjust.method != 0U) &&
            (final_sharpen_noise->level_str_adjust.method != 1U) &&
            (final_sharpen_noise->level_str_adjust.method != 2U) &&
            (final_sharpen_noise->level_str_adjust.method != 6U) &&
            (final_sharpen_noise->level_str_adjust.method != 7U) &&
            (final_sharpen_noise->level_str_adjust.method != 12U) &&
            (final_sharpen_noise->level_str_adjust.method != 14U)) {
            amba_ik_system_print_uint32_5("[IK] final_sharpen_noise->level_str_adjust.method %d , should be 0,1,6,7", final_sharpen_noise->level_str_adjust.method, DC_U, DC_U, DC_U, DC_U);
            rval |= IK_ERR_0008;
        }
        //cv2
        rval |= (uint32)ctx_check_uint32_parameter_valid_range("final_sharpen_noise->advanced_iso.enable", final_sharpen_noise->advanced_iso.enable, 0U, 1U);
        rval |= (uint32)ctx_check_uint32_parameter_valid_range("final_sharpen_noise->advanced_iso.enable", final_sharpen_noise->advanced_iso.size, 0U, 2U);
        if ((final_sharpen_noise->advanced_iso.max_change_method != 0U) &&
            (final_sharpen_noise->advanced_iso.max_change_method != 1U) &&
            (final_sharpen_noise->advanced_iso.max_change_method != 2U) &&
            (final_sharpen_noise->advanced_iso.max_change_method != 6U) &&
            (final_sharpen_noise->advanced_iso.max_change_method != 7U) &&
            (final_sharpen_noise->advanced_iso.max_change_method != 12U) &&
            (final_sharpen_noise->advanced_iso.max_change_method != 14U)) {
            amba_ik_system_print_uint32_5("[IK] final_sharpen_noise->advanced_iso.max_change_method %d , should be 0,1,2,6,7,12,14", final_sharpen_noise->advanced_iso.max_change_method, DC_U, DC_U, DC_U, DC_U);
            rval |= IK_ERR_0008;
        }
        if ((final_sharpen_noise->advanced_iso.noise_level_method != 0U) &&
            (final_sharpen_noise->advanced_iso.noise_level_method != 1U) &&
            (final_sharpen_noise->advanced_iso.noise_level_method != 2U) &&
            (final_sharpen_noise->advanced_iso.noise_level_method != 6U) &&
            (final_sharpen_noise->advanced_iso.noise_level_method != 7U) &&
            (final_sharpen_noise->advanced_iso.noise_level_method != 12U) &&
            (final_sharpen_noise->advanced_iso.noise_level_method != 14U)) {
            amba_ik_system_print_uint32_5("[IK] final_sharpen_noise->advanced_iso.noise_level_method %d , should be 0,1,2,6,7,12,14", final_sharpen_noise->advanced_iso.noise_level_method, DC_U, DC_U, DC_U, DC_U);
            rval |= IK_ERR_0008;
        }
        if ((final_sharpen_noise->advanced_iso.str_a_method != 0U) &&
            (final_sharpen_noise->advanced_iso.str_a_method != 1U) &&
            (final_sharpen_noise->advanced_iso.str_a_method != 2U) &&
            (final_sharpen_noise->advanced_iso.str_a_method != 6U) &&
            (final_sharpen_noise->advanced_iso.str_a_method != 7U) &&
            (final_sharpen_noise->advanced_iso.str_a_method != 12U) &&
            (final_sharpen_noise->advanced_iso.str_a_method != 14U)) {
            amba_ik_system_print_uint32_5("[IK] final_sharpen_noise->advanced_iso.str_a_method %d , should be 0,1,2,6,7,12,14", final_sharpen_noise->advanced_iso.str_a_method, DC_U, DC_U, DC_U, DC_U);
            rval |= IK_ERR_0008;
        }
        if ((final_sharpen_noise->advanced_iso.str_b_method != 0U) &&
            (final_sharpen_noise->advanced_iso.str_b_method != 1U) &&
            (final_sharpen_noise->advanced_iso.str_b_method != 2U) &&
            (final_sharpen_noise->advanced_iso.str_b_method != 6U) &&
            (final_sharpen_noise->advanced_iso.str_b_method != 7U) &&
            (final_sharpen_noise->advanced_iso.str_b_method != 12U) &&
            (final_sharpen_noise->advanced_iso.str_b_method != 14U)) {
            amba_ik_system_print_uint32_5("[IK] final_sharpen_noise->advanced_iso.str_b_method %d , should be 0,1,2,6,7,12,14", final_sharpen_noise->advanced_iso.str_b_method, DC_U, DC_U, DC_U, DC_U);
            rval |= IK_ERR_0008;
        }
    }
    return rval;
}

static uint32 ctx_check_fnlshpns_fir_valid_range(const ik_final_sharpen_fir_t *final_sharpen_fir)
{
    uint32 rval = IK_OK;
    uint32  i = 0, j = 0;
    if (final_sharpen_fir == NULL) {
        amba_ik_system_print_str_5("[IK] detect final_sharpen_fir get NULL pointer!", NULL, NULL, NULL, NULL, NULL);
        rval |= IK_ERR_0005;
    } else {
        rval |= ctx_check_uint32_parameter_valid_range("final_sharpen_fir->specify", final_sharpen_fir->specify, 0U, 4U);
        if (final_sharpen_fir->specify == 0U) {
            rval |= ctx_check_uint32_parameter_valid_range("final_sharpen_fir->strength_iso", final_sharpen_fir->strength_iso, 0U, 256U);
        } else if (final_sharpen_fir->specify == 1U) {
            for (j = 0U; j < 25U; j++) {
                rval |= ctx_check_int32_parameter_valid_range("final_sharpen_fir->coefs", final_sharpen_fir->coefs[0][j], -1024, 1023);
            }
        } else if (final_sharpen_fir->specify == 2U) {
            rval |= ctx_check_uint32_parameter_valid_range("final_sharpen_fir->strength_iso", final_sharpen_fir->strength_iso, 0U, 256U);
            rval |= ctx_check_uint32_parameter_valid_range("final_sharpen_fir->strength_dir", final_sharpen_fir->strength_dir, 0U, 256U);
        } else if (final_sharpen_fir->specify == 3U) {
            for (i = 0U; i < 9U; i++) {
                rval |= ctx_check_uint32_parameter_valid_range("final_sharpen_fir->per_dir_fir_dir_amounts", final_sharpen_fir->per_dir_fir_dir_amounts[i],    0U, 256U);
                rval |= ctx_check_uint32_parameter_valid_range("final_sharpen_fir->per_dir_fir_dir_strengths", final_sharpen_fir->per_dir_fir_dir_strengths[i], 0U, 256U);
                rval |= ctx_check_uint32_parameter_valid_range("final_sharpen_fir->per_dir_fir_iso_strengths", final_sharpen_fir->per_dir_fir_iso_strengths[i], 0U, 256U);
            }
        } else if (final_sharpen_fir->specify == 4U) {
            for (j = 0U; j < 9U; j++) {
                for (i = 0U; i < 25U; i++) {
                    rval |= ctx_check_int32_parameter_valid_range("final_sharpen_fir->coefs", final_sharpen_fir->coefs[j][i], -1024, 1023);
                }
            }
        } else {
            // misraC
        }
        //cv2
        rval |= ctx_check_uint32_parameter_valid_range("final_sharpen_fir->pos_fir_artifact_reduce_enable", final_sharpen_fir->pos_fir_artifact_reduce_enable, 0U, 1U);
        rval |= ctx_check_uint32_parameter_valid_range("final_sharpen_fir->pos_fir_artifact_reduce_strength", final_sharpen_fir->pos_fir_artifact_reduce_strength, 0U, 223U);
    }
    return rval;
}

static uint32 ctx_check_fnlshpns_coring_valid_range(const ik_final_sharpen_coring_t *final_sharpen_coring)
{
    uint32 rval = IK_OK;
    uint32  i = 0;
    if (final_sharpen_coring == NULL) {
        amba_ik_system_print_str_5("[IK] detect final_sharpen_coring get NULL pointer!", NULL, NULL, NULL, NULL, NULL);
        rval |= IK_ERR_0005;
    } else {
        rval |= ctx_check_uint32_parameter_valid_range("final_sharpen_coring->fractional_bits", final_sharpen_coring->fractional_bits, 1, 3);
        for (i = 0U; i < 256U; i++) {
            rval |= ctx_check_uint32_parameter_valid_range("final_sharpen_coring->coring", final_sharpen_coring->coring[i], 0, 31);
        }
    }
    return rval;
}

static uint32 ctx_check_fnlshpns_coring_index_scale_valid_range(const ik_final_sharpen_coring_idx_scale_t *fnlshpns_cor_idx_scl)
{
    uint32 rval = IK_OK;
    if (fnlshpns_cor_idx_scl == NULL) {
        amba_ik_system_print_str_5("[IK] detect final_sharpen_coring_idx_scale get NULL pointer!", NULL, NULL, NULL, NULL, NULL);
        rval |= IK_ERR_0005;
    } else {
        rval |= ctx_check_uint32_parameter_valid_range("final_sharpen_coring_idx_scale->low", fnlshpns_cor_idx_scl->low, 0U, 253U);
        rval |= ctx_check_uint32_parameter_valid_range("final_sharpen_coring_idx_scale->low_delta", fnlshpns_cor_idx_scl->low_delta, 0U, 7U);
        rval |= ctx_check_uint32_parameter_valid_range("final_sharpen_coring_idx_scale->high", fnlshpns_cor_idx_scl->high, 1, 254);
        rval |= ctx_check_uint32_parameter_valid_range("final_sharpen_coring_idx_scale->high_delta", fnlshpns_cor_idx_scl->high_delta, 0U, 7U);
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

static uint32 ctx_check_fnlshpns_min_coring_result_valid_range(const ik_final_sharpen_min_coring_result_t *fnlshpns_min_cor_rst)
{
    uint32 rval = IK_OK;
    if (fnlshpns_min_cor_rst == NULL) {
        amba_ik_system_print_str_5("[IK] detect fnlshpns_min_cor_rst get NULL pointer!", NULL, NULL, NULL, NULL, NULL);
        rval |= IK_ERR_0005;
    } else {
        rval |= ctx_check_uint32_parameter_valid_range("fnlshpns_min_cor_rst->low", fnlshpns_min_cor_rst->low, 0U, 253U);
        rval |= ctx_check_uint32_parameter_valid_range("fnlshpns_min_cor_rst->low_delta", fnlshpns_min_cor_rst->low_delta, 0U, 7U);
        rval |= ctx_check_uint32_parameter_valid_range("fnlshpns_min_cor_rst->low_strength", fnlshpns_min_cor_rst->low_strength, 0U, 255U);
        rval |= ctx_check_uint32_parameter_valid_range("fnlshpns_min_cor_rst->mid_strength", fnlshpns_min_cor_rst->mid_strength, 0U, 255U);
        rval |= ctx_check_uint32_parameter_valid_range("fnlshpns_min_cor_rst->high", fnlshpns_min_cor_rst->high, 1, 254);
        rval |= ctx_check_uint32_parameter_valid_range("fnlshpns_min_cor_rst->high_delta", fnlshpns_min_cor_rst->high_delta, 0U, 7U);
        rval |= ctx_check_uint32_parameter_valid_range("fnlshpns_min_cor_rst->high_strength", fnlshpns_min_cor_rst->high_strength, 0U, 255U);
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

static uint32 ctx_check_fnlshpns_max_coring_result_valid_range(const ik_final_sharpen_max_coring_result_t *fnlshpns_max_cor_rst)
{
    uint32 rval = IK_OK;
    if (fnlshpns_max_cor_rst == NULL) {
        amba_ik_system_print_str_5("[IK] detect fnlshpns_max_cor_rst get NULL pointer!", NULL, NULL, NULL, NULL, NULL);
        rval |= IK_ERR_0005;
    } else {
        rval |= ctx_check_uint32_parameter_valid_range("fnlshpns_max_cor_rst->low", fnlshpns_max_cor_rst->low, 0U, 253U);
        rval |= ctx_check_uint32_parameter_valid_range("fnlshpns_max_cor_rst->low_delta", fnlshpns_max_cor_rst->low_delta, 0U, 7U);
        rval |= ctx_check_uint32_parameter_valid_range("fnlshpns_max_cor_rst->low_strength", fnlshpns_max_cor_rst->low_strength, 0U, 255U);
        rval |= ctx_check_uint32_parameter_valid_range("fnlshpns_max_cor_rst->mid_strength", fnlshpns_max_cor_rst->mid_strength, 0U, 255U);
        rval |= ctx_check_uint32_parameter_valid_range("fnlshpns_max_cor_rst->high", fnlshpns_max_cor_rst->high, 1, 254);
        rval |= ctx_check_uint32_parameter_valid_range("fnlshpns_max_cor_rst->high_delta", fnlshpns_max_cor_rst->high_delta, 0U, 7U);
        rval |= ctx_check_uint32_parameter_valid_range("fnlshpns_max_cor_rst->high_strength", fnlshpns_max_cor_rst->high_strength, 0U, 255U);
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

static uint32 ctx_check_fnlshpns_scale_coring_valid_range(const ik_final_sharpen_scale_coring_t *fnlshpns_scl_cor)
{
    uint32 rval = IK_OK;
    if (fnlshpns_scl_cor == NULL) {
        amba_ik_system_print_str_5("[IK] detect fnlshpns_scl_cor get NULL pointer!", NULL, NULL, NULL, NULL, NULL);
        rval |= IK_ERR_0005;
    } else {
        rval |= ctx_check_uint32_parameter_valid_range("fnlshpns_scl_cor->low", fnlshpns_scl_cor->low, 0U, 253U);
        rval |= ctx_check_uint32_parameter_valid_range("fnlshpns_scl_cor->low_delta", fnlshpns_scl_cor->low_delta, 0U, 7U);
        rval |= ctx_check_uint32_parameter_valid_range("fnlshpns_scl_cor->high", fnlshpns_scl_cor->high, 1, 254);
        rval |= ctx_check_uint32_parameter_valid_range("fnlshpns_scl_cor->high_delta", fnlshpns_scl_cor->high_delta, 0U, 7U);
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

static uint32 ctx_check_fnlshpns_both_tdt_valid_range(const ik_final_sharpen_both_three_d_table_t *final_sharpen_both_three_d_table)
{
    uint32 rval = IK_OK;

    if (final_sharpen_both_three_d_table == NULL) {
        amba_ik_system_print_str_5("[IK] detect final_sharpen_both_three_d_table get NULL pointer!", NULL, NULL, NULL, NULL, NULL);
        rval |= IK_ERR_0005;
    } else {
        rval |= (uint32)ctx_check_uint32_parameter_valid_range("final_sharpen_both_three_d_table->y_tone_offset", final_sharpen_both_three_d_table->y_tone_offset, 0U, 255U);
        rval |= (uint32)ctx_check_uint32_parameter_valid_range("final_sharpen_both_three_d_table->y_tone_shift", final_sharpen_both_three_d_table->y_tone_shift, 0U, 7U);
        rval |= (uint32)ctx_check_uint32_parameter_valid_range("final_sharpen_both_three_d_table->u_tone_offset", final_sharpen_both_three_d_table->u_tone_offset, 0U, 255U);
        rval |= (uint32)ctx_check_uint32_parameter_valid_range("final_sharpen_both_three_d_table->u_tone_shift", final_sharpen_both_three_d_table->u_tone_shift, 0U, 7U);
        rval |= (uint32)ctx_check_uint32_parameter_valid_range("final_sharpen_both_three_d_table->u_tone_bits", final_sharpen_both_three_d_table->u_tone_bits, 0U, 8U);
        rval |= (uint32)ctx_check_uint32_parameter_valid_range("final_sharpen_both_three_d_table->v_tone_offset", final_sharpen_both_three_d_table->v_tone_offset, 0U, 255U);
        rval |= (uint32)ctx_check_uint32_parameter_valid_range("final_sharpen_both_three_d_table->v_tone_shift", final_sharpen_both_three_d_table->v_tone_shift, 0U, 7U);
        rval |= (uint32)ctx_check_uint32_parameter_valid_range("final_sharpen_both_three_d_table->v_tone_bits", final_sharpen_both_three_d_table->v_tone_bits, 0U, 8U);
    }

    return rval;
}

static uint32 ctx_check_dzoom_info_valid_range(const ik_dzoom_info_t *dzoom_info)
{
    uint32 rval = IK_OK;
    if (dzoom_info == NULL) {
        rval |= IK_ERR_0005;
    } else {
        if ((dzoom_info->zoom_x < 0x10000U) && (dzoom_info->enable==1U)) {
            amba_ik_system_print_uint32_5("[IK] detect invalid zoom_x 0x%x. ", dzoom_info->zoom_x, DC_U, DC_U, DC_U, DC_U);
            rval |= IK_ERR_0008;
        }
        if ((dzoom_info->zoom_y < 0x10000U) && (dzoom_info->enable==1U)) {
            amba_ik_system_print_uint32_5("[IK] detect invalid zoom_y 0x%x. ", dzoom_info->zoom_y, DC_U, DC_U, DC_U, DC_U);
            rval |= IK_ERR_0008;
        }
    }
    return rval;
}

static uint32 ctx_check_hdr_shift_info_valid_range(const ik_hdr_eis_info_t *hdr_sft)
{
    uint32 rval = IK_OK;
    if (hdr_sft == NULL) {
        rval |= IK_ERR_0005;
    } else {
        ;//shift needn't check, auto clamping by IK.
    }
    return rval;
}

static uint32 ctx_check_flip_mode_info_valid_range(uint32 mode)
{
    uint32 rval = IK_OK;
    if ((mode==0U) || (mode==IK_FLIP_RAW_V) || (mode==IK_FLIP_YUV_H) || (mode==IK_FLIP_RAW_V_YUV_H)) {
        // OK
    } else {
        amba_ik_system_print_uint32_5("[IK] detect invalid flip mode %d. ", mode, DC_U, DC_U, DC_U, DC_U);
        rval |= IK_ERR_0008;
    }
    return rval;
}

static uint32 ctx_check_window_size_info_valid_range(const ik_window_size_info_t *window_size_info, uint32 ability)
{
    uint32 rval = IK_OK;
    uint32 i;
    if (window_size_info == NULL) {
        rval |= IK_ERR_0005;
    } else {
        rval |= ctx_check_uint32_parameter_valid_range("window_size_info->vin_sensor.start_x", window_size_info->vin_sensor.start_x, 0U, 8192U);
        rval |= ctx_check_uint32_parameter_valid_range("window_size_info->vin_sensor.start_y", window_size_info->vin_sensor.start_y, 0U, 8192U);
        if ((window_size_info->vin_sensor.width == 0U) || (window_size_info->vin_sensor.height == 0U) ||
            (window_size_info->vin_sensor.width > 15360U) || (window_size_info->vin_sensor.height > 8640U)) {
            amba_ik_system_print_uint32_5("[IK] vin_sensor(%d, %d) is invalid!", window_size_info->vin_sensor.width, window_size_info->vin_sensor.height, DC_U, DC_U, DC_U);
            rval |= IK_ERR_0008;
        }
        if((ability == AMBA_IK_VIDEO_PIPE_LINEAR) || (ability == AMBA_IK_VIDEO_PIPE_Y2Y) || (ability == AMBA_IK_VIDEO_PIPE_LINEAR_HVH) ||
           (ability == AMBA_IK_VIDEO_PIPE_LINEAR_HHB) || (ability == AMBA_IK_VIDEO_PIPE_Y2Y_HVH) || (ability == AMBA_IK_VIDEO_PIPE_Y2Y_HHB) ||
           (ability == AMBA_IK_VIDEO_PIPE_Y2Y_MD) || (ability == AMBA_IK_VIDEO_PIPE_Y2Y_MD_HHB)) {
            if (((window_size_info->vin_sensor.width&(AMBA_IK_BASIC_ALIGNMENT-1U)) != 0U) || ((window_size_info->vin_sensor.height&0x7U) != 0U)) {
                amba_ik_system_print_uint32_5("[IK] vin_sensor(%d, %d) is invalid! width not 64-aligned or height not 8-aligned, ability = %d.", window_size_info->vin_sensor.width, window_size_info->vin_sensor.height, ability, DC_U, DC_U);
                rval |= IK_ERR_0008;
            }
        } else {
            if ((window_size_info->vin_sensor.height&0x7U) != 0U) {
                amba_ik_system_print_uint32_5("[IK] vin_sensor(%d, %d) is invalid! height not 8-aligned, in CE enabled ability = %d.", window_size_info->vin_sensor.width, window_size_info->vin_sensor.height, ability, DC_U, DC_U);
                rval |= IK_ERR_0008;
            }
        }
        rval |= ctx_check_uint32_parameter_valid_range("window_size_info->vin_sensor.h_sub_sample.factor_num", window_size_info->vin_sensor.h_sub_sample.factor_num, 1U, 16U);
        rval |= ctx_check_uint32_parameter_valid_range("window_size_info->vin_sensor.h_sub_sample.factor_den", window_size_info->vin_sensor.h_sub_sample.factor_den, 1U, 16U);
        rval |= ctx_check_uint32_parameter_valid_range("window_size_info->vin_sensor.v_sub_sample.factor_num", window_size_info->vin_sensor.v_sub_sample.factor_num, 1U, 16U);
        rval |= ctx_check_uint32_parameter_valid_range("window_size_info->vin_sensor.v_sub_sample.factor_den", window_size_info->vin_sensor.v_sub_sample.factor_den, 1U, 16U);

        if ((window_size_info->main_win.width == 0U) || (window_size_info->main_win.height == 0U) ||
            (window_size_info->main_win.width > 15360U) || (window_size_info->main_win.height > 8640U)) {
            amba_ik_system_print_uint32_5("[IK] main_win(%d, %d) is invalid!", window_size_info->main_win.width, window_size_info->main_win.height, DC_U, DC_U, DC_U);
            rval |= IK_ERR_0008;
        }
        if ((window_size_info->main_win.width&(AMBA_IK_BASIC_ALIGNMENT-1U)) != 0U) {
            amba_ik_system_print_uint32_5("[IK] main_win(%d, %d) is invalid! width not 64-aligned.", window_size_info->main_win.width, window_size_info->main_win.height, DC_U, DC_U, DC_U);
            rval |= IK_ERR_0008;
        }
        if(((window_size_info->main_win.width<<3U)<= window_size_info->vin_sensor.width)||
           ((window_size_info->main_win.height<<3U)<= window_size_info->vin_sensor.height)) {
            amba_ik_system_print_uint32_5("[IK] main_win(%d, %d), vin_sensor(%d, %d), over 7.99x down scale ratio",
                                          window_size_info->main_win.width, window_size_info->main_win.height, window_size_info->vin_sensor.width, window_size_info->vin_sensor.height, DC_U);
            rval |= IK_ERR_0008;
        }

        for(i=0; i<3U; i++) {
            if ((window_size_info->prev[i].width > window_size_info->main_win.width) || (window_size_info->prev[i].height > window_size_info->main_win.height)) {
                amba_ik_system_print_uint32_5("[IK] prev[%d](%d, %d) is invalid!should not > main_win(%d, %d)", i, window_size_info->prev[i].width, window_size_info->prev[i].height, window_size_info->main_win.width, window_size_info->main_win.height);
                rval |= IK_ERR_0008;
            }
        }
        if ((window_size_info->thumbnail.width > window_size_info->main_win.width) || (window_size_info->thumbnail.height > window_size_info->main_win.height)) {
            amba_ik_system_print_uint32_5("[IK] thumbnail(%d, %d) is invalid!should not > main_win(%d, %d)", window_size_info->thumbnail.width, window_size_info->thumbnail.height, window_size_info->main_win.width, window_size_info->main_win.height, DC_U);
            rval |= IK_ERR_0008;
        }
        if ((window_size_info->screennail.width > window_size_info->main_win.width) || (window_size_info->screennail.height > window_size_info->main_win.height)) {
            amba_ik_system_print_uint32_5("[IK] screennail(%d, %d) is invalid!should not > main_win(%d, %d)", window_size_info->screennail.width, window_size_info->screennail.height, window_size_info->main_win.width, window_size_info->main_win.height, DC_U);
            rval |= IK_ERR_0008;
        }

    }
    return rval;
}

static uint32 ctx_check_cfa_window_size_info_valid_range(const ik_cfa_window_size_info_t *cfa_window_size_info)
{
    uint32 rval = IK_OK;

    if (cfa_window_size_info == NULL) {
        rval |= IK_ERR_0005;
    } else {
        rval |= ctx_check_uint32_parameter_valid_range("cfa_window_size_info->enable", cfa_window_size_info->enable, 0U, 1U);
        if (((cfa_window_size_info->cfa.width&0x1U) != 0U) || ((cfa_window_size_info->cfa.height&0x1U) != 0U)) {
            amba_ik_system_print_uint32_5("[IK] cfa_window_size_info(%d, %d) is invalid! width not 2-aligned or height not 2-aligned.", cfa_window_size_info->cfa.width, cfa_window_size_info->cfa.height, DC_U, DC_U, DC_U);
            rval |= IK_ERR_0008;
        }
    }
    return rval;
}

static uint32 ctx_check_vin_active_win_valid_range(const ik_vin_active_window_t *p_active_win, uint32 ability)
{
    uint32 rval = IK_OK;

    if (p_active_win == NULL) {
        rval |= IK_ERR_0005;
    } else {
        if(p_active_win->enable == 1U) {
            if ((p_active_win->active_geo.width == 0U) || (p_active_win->active_geo.height == 0U)) {
                amba_ik_system_print_str_5("[IK] active_vin() is invalid!", NULL, NULL, NULL, NULL, NULL);
                rval |= IK_ERR_0008;
            }
            if((ability == AMBA_IK_VIDEO_PIPE_LINEAR) || (ability == AMBA_IK_VIDEO_PIPE_Y2Y) || (ability == AMBA_IK_VIDEO_PIPE_LINEAR_HVH) ||
               (ability == AMBA_IK_VIDEO_PIPE_LINEAR_HHB) || (ability == AMBA_IK_VIDEO_PIPE_Y2Y_HVH) || (ability == AMBA_IK_VIDEO_PIPE_Y2Y_HHB) ||
               (ability == AMBA_IK_VIDEO_PIPE_Y2Y_MD) || (ability == AMBA_IK_VIDEO_PIPE_Y2Y_MD_HHB)) {
                if (((p_active_win->active_geo.width&(AMBA_IK_BASIC_ALIGNMENT-1U)) != 0U) || ((p_active_win->active_geo.height&0x7U) != 0U)) {
                    amba_ik_system_print_uint32_5("[IK] vin_sensor(%d, %d) is invalid! width not 64-aligned or height not 8-aligned, ability = %d.", p_active_win->active_geo.width, p_active_win->active_geo.height, ability, DC_U, DC_U);
                    rval |= IK_ERR_0008;
                }
            } else {
                if ((p_active_win->active_geo.height&0x7U) != 0U) {
                    amba_ik_system_print_uint32_5("[IK] vin_sensor(%d, %d) is invalid! height not 8-aligned, in CE enabled ability = %d.", p_active_win->active_geo.width, p_active_win->active_geo.height, ability, DC_U, DC_U);
                    rval |= IK_ERR_0008;
                }
            }
        }
    }
    return rval;
}

static uint32 ctx_check_set_dummy_win_margin_range_info_valid_range(const ik_dummy_margin_range_t *dmy_range, uint32 stitch_enb)
{
    uint32 rval = IK_OK;
    if (dmy_range == NULL) {
        rval |= IK_ERR_0005;
    } else {
        if (dmy_range->bottom > 0x10000U) {
            amba_ik_system_print_uint32_5("[IK] detect invalid p_user_dmy_range->bottom 0x%x. ", dmy_range->bottom, DC_U, DC_U, DC_U, DC_U);
            rval |= IK_ERR_0008;
        }
        if (dmy_range->top > 0x10000U) {
            amba_ik_system_print_uint32_5("[IK] detect invalid p_user_dmy_range->top 0x%x. ", dmy_range->top, DC_U, DC_U, DC_U, DC_U);
            rval |= IK_ERR_0008;
        }
        if (dmy_range->left > 0x10000U) {
            amba_ik_system_print_uint32_5("[IK] detect invalid p_user_dmy_range->left 0x%x. ", dmy_range->left, DC_U, DC_U, DC_U, DC_U);
            rval |= IK_ERR_0008;
        }
        if (dmy_range->right > 0x10000U) {
            amba_ik_system_print_uint32_5("[IK] detect invalid p_user_dmy_range->right 0x%x. ", dmy_range->right, DC_U, DC_U, DC_U, DC_U);
            rval |= IK_ERR_0008;
        }
        if ((stitch_enb != 0U)&&(dmy_range->left != dmy_range->right)) {
            amba_ik_system_print_uint32_5("[IK] current vin size, detect invalid p_user_dmy_range->left 0x%x != right 0x%x. ", dmy_range->left, dmy_range->right, DC_U, DC_U, DC_U);
            rval |= IK_ERR_0008;
        }
    }
    return rval;
}

static uint32 ctx_check_resampler_str_valid_range(const ik_resampler_strength_t *p_rsmp_str)
{
    uint32 rval = IK_OK;
    if (p_rsmp_str == NULL) {
        rval = IK_ERR_0005;
    } else {
        rval |= (uint32) ctx_check_uint32_parameter_valid_range("p_rsmp_str->cfa_cut_off_freq", p_rsmp_str->cfa_cut_off_freq, 0U, 7U);
        rval |= (uint32) ctx_check_uint32_parameter_valid_range("p_rsmp_str->main_cut_off_freq", p_rsmp_str->main_cut_off_freq, 0U, 7U);
    }

    return rval;
}

static uint32 ctx_check_motion_detect_valid_range(const ik_motion_detect_t *motion_detect)
{
    uint32 rval = IK_OK;
    if (motion_detect == NULL) {
        rval |= IK_ERR_0005;
    } else {
        rval |= (uint32)ctx_check_uint32_parameter_valid_range("motion_detect->enable", motion_detect->enable, 0U, 1U);
        rval |= (uint32)ctx_check_uint32_parameter_valid_range("motion_detect->local_lighting", motion_detect->local_lighting, 0U, 1U);
        rval |= (uint32)ctx_check_uint32_parameter_valid_range("motion_detect->motion_response", motion_detect->motion_response, 1U, 511U);
        rval |= (uint32)ctx_check_uint32_parameter_valid_range("motion_detect->slow_mo_sensitivity", motion_detect->slow_mo_sensitivity, 0U, 19U);
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
    return rval;
}

static uint32 ctx_check_static_bpc_internal_valid_range(const ik_static_bad_pixel_correction_internal_t *static_bpc)
{
    uint32 rval = IK_OK;

    if (static_bpc == NULL) {
        amba_ik_system_print_str_5("[IK] detect static_bpc get NULL pointer!", DC_S, DC_S, DC_S, DC_S, DC_S);
        rval |= IK_ERR_0005;
    } else {
        /* Disable mode, discarding the checks as follow.*/
        /* Check SBP calibration version*/
        rval |= ctx_check_uint32_parameter_valid_range("static_bpc->enable", static_bpc->enable, 0, 3);

        if (static_bpc->p_map == NULL) {
            amba_ik_system_print_str_5("[IK] SBP internal p_map = NULL !\n", DC_S, DC_S, DC_S, DC_S, DC_S);
            rval |= IK_ERR_0008;
        }
    }
    return rval;
}

static uint32 ctx_check_calib_warp_info_valid_range(const ik_warp_info_t *calib_warp_info)
{
    uint32 rval = IK_OK;
    uintptr misra_uintptr;

    if (calib_warp_info == NULL) {
        amba_ik_system_print_str_5("[IK] detect calib_warp_info get NULL pointer!", DC_S, DC_S, DC_S, DC_S, DC_S);
        rval |= IK_ERR_0005;
    } else {
        rval |= ctx_check_uint32_parameter_valid_range("calib_warp_info->version", calib_warp_info->version, IK_WARP_VER, IK_WARP_VER);
        rval |= ctx_check_uint32_parameter_valid_range("calib_warp_info->hor_grid_num", calib_warp_info->hor_grid_num, 2U, 256U);
        rval |= ctx_check_uint32_parameter_valid_range("calib_warp_info->ver_grid_num", calib_warp_info->ver_grid_num, 2U, 192U);
        rval |= ctx_check_uint32_parameter_valid_range("calib_warp_info->tile_width_exp", calib_warp_info->tile_width_exp, 0U, 9U);
        rval |= ctx_check_uint32_parameter_valid_range("calib_warp_info->tile_height_exp", calib_warp_info->tile_height_exp, 0U, 9U);
        rval |= ctx_check_uint32_parameter_valid_range("calib_warp_info->vin_sensor_geo.start_x", calib_warp_info->vin_sensor_geo.start_x, 0U, 8192U);
        rval |= ctx_check_uint32_parameter_valid_range("calib_warp_info->vin_sensor_geo.start_y", calib_warp_info->vin_sensor_geo.start_y, 0U, 8192U);
        rval |= ctx_check_uint32_parameter_valid_range("calib_warp_info->vin_sensor_geo.width", calib_warp_info->vin_sensor_geo.width, 2U, 15360U);
        rval |= ctx_check_uint32_parameter_valid_range("calib_warp_info->vin_sensor_geo.height", calib_warp_info->vin_sensor_geo.height, 2U, 8640U);
        rval |= ctx_check_uint32_parameter_valid_range("calib_warp_info->vin_sensor_geo.h_sub_sample.factor_num", calib_warp_info->vin_sensor_geo.h_sub_sample.factor_num, 1U, 16U);
        rval |= ctx_check_uint32_parameter_valid_range("calib_warp_info->vin_sensor_geo.h_sub_sample.factor_den", calib_warp_info->vin_sensor_geo.h_sub_sample.factor_den, 1U, 16U);
        rval |= ctx_check_uint32_parameter_valid_range("calib_warp_info->vin_sensor_geo.v_sub_sample.factor_num", calib_warp_info->vin_sensor_geo.v_sub_sample.factor_num, 1U, 16U);
        rval |= ctx_check_uint32_parameter_valid_range("calib_warp_info->vin_sensor_geo.v_sub_sample.factor_den", calib_warp_info->vin_sensor_geo.v_sub_sample.factor_den, 1U, 16U);
        (void)amba_ik_system_memcpy(&misra_uintptr, &calib_warp_info->pwarp, sizeof(uint32*));
        if (calib_warp_info->pwarp == NULL) {
            amba_ik_system_print_uint32_5("[IK] calib_warp_info->pwarp 0x%x%x", GetU64Msb(misra_uintptr), GetU64Lsb(misra_uintptr), DC_U, DC_U, DC_U);
            rval |= IK_ERR_0008;
        }

        if (((calib_warp_info->hor_grid_num - 1U) << calib_warp_info->tile_width_exp) < calib_warp_info->vin_sensor_geo.width) {
            amba_ik_system_print_uint32_5("[IK] (hor_grid_num %d - 1) * tile_width_exp %d < Calib vin width %d", calib_warp_info->hor_grid_num, (uint32)(1UL << calib_warp_info->tile_width_exp), calib_warp_info->vin_sensor_geo.width, DC_U, DC_U);
            rval |= IK_ERR_0008;
        }

        if (((calib_warp_info->ver_grid_num - 1U) << calib_warp_info->tile_height_exp) < calib_warp_info->vin_sensor_geo.height) {
            amba_ik_system_print_uint32_5("[IK] (ver_grid_num %d - 1) * tile_height_exp %d < Calib vin width %d", calib_warp_info->ver_grid_num, (uint32)(1UL << calib_warp_info->tile_height_exp), calib_warp_info->vin_sensor_geo.height, DC_U, DC_U);
            rval |= IK_ERR_0008;
        }
    }
    return rval;
}

static uint32 ctx_check_2nd_calib_warp_info_valid_range(const ik_warp_info_t *calib_warp_info_2nd)
{
    uint32 rval = IK_OK;
    uintptr misra_uintptr;

    if (calib_warp_info_2nd == NULL) {
        amba_ik_system_print_str_5("[IK] detect calib_warp_info_2nd get NULL pointer!", DC_S, DC_S, DC_S, DC_S, DC_S);
        rval |= IK_ERR_0005;
    } else {
        rval |= ctx_check_uint32_parameter_valid_range("calib_warp_info_2nd->version", calib_warp_info_2nd->version, IK_WARP_VER, IK_WARP_VER);
        rval |= ctx_check_uint32_parameter_valid_range("calib_warp_info_2nd->hor_grid_num", calib_warp_info_2nd->hor_grid_num, 2U, 256U);
        rval |= ctx_check_uint32_parameter_valid_range("calib_warp_info_2nd->ver_grid_num", calib_warp_info_2nd->ver_grid_num, 2U, 192U);
        rval |= ctx_check_uint32_parameter_valid_range("calib_warp_info_2nd->tile_width_exp", calib_warp_info_2nd->tile_width_exp, 0U, 9U);
        rval |= ctx_check_uint32_parameter_valid_range("calib_warp_info_2nd->tile_height_exp", calib_warp_info_2nd->tile_height_exp, 0U, 9U);
        rval |= ctx_check_uint32_parameter_valid_range("calib_warp_info_2nd->vin_sensor_geo.start_x", calib_warp_info_2nd->vin_sensor_geo.start_x, 0U, 8192U);
        rval |= ctx_check_uint32_parameter_valid_range("calib_warp_info_2nd->vin_sensor_geo.start_y", calib_warp_info_2nd->vin_sensor_geo.start_y, 0U, 8192U);
        rval |= ctx_check_uint32_parameter_valid_range("calib_warp_info_2nd->vin_sensor_geo.width", calib_warp_info_2nd->vin_sensor_geo.width, 2U, 15360U);
        rval |= ctx_check_uint32_parameter_valid_range("calib_warp_info_2nd->vin_sensor_geo.height", calib_warp_info_2nd->vin_sensor_geo.height, 2U, 8640U);
        rval |= ctx_check_uint32_parameter_valid_range("calib_warp_info_2nd->vin_sensor_geo.h_sub_sample.factor_num", calib_warp_info_2nd->vin_sensor_geo.h_sub_sample.factor_num, 1U, 16U);
        rval |= ctx_check_uint32_parameter_valid_range("calib_warp_info_2nd->vin_sensor_geo.h_sub_sample.factor_den", calib_warp_info_2nd->vin_sensor_geo.h_sub_sample.factor_den, 1U, 16U);
        rval |= ctx_check_uint32_parameter_valid_range("calib_warp_info_2nd->vin_sensor_geo.v_sub_sample.factor_num", calib_warp_info_2nd->vin_sensor_geo.v_sub_sample.factor_num, 1U, 16U);
        rval |= ctx_check_uint32_parameter_valid_range("calib_warp_info_2nd->vin_sensor_geo.v_sub_sample.factor_den", calib_warp_info_2nd->vin_sensor_geo.v_sub_sample.factor_den, 1U, 16U);
        (void)amba_ik_system_memcpy(&misra_uintptr, &calib_warp_info_2nd->pwarp, sizeof(uint32*));
        if (calib_warp_info_2nd->pwarp == NULL) {
            amba_ik_system_print_uint32_5("[IK] calib_warp_info_2nd->pwarp 0x%x%x", GetU64Msb(misra_uintptr), GetU64Lsb(misra_uintptr), DC_U, DC_U, DC_U);
            rval |= IK_ERR_0008;
        }

        if (((calib_warp_info_2nd->hor_grid_num - 1U) << calib_warp_info_2nd->tile_width_exp) < calib_warp_info_2nd->vin_sensor_geo.width) {
            amba_ik_system_print_uint32_5("[IK] (hor_grid_num %d - 1) * tile_width_exp %d < Calib vin width %d", calib_warp_info_2nd->hor_grid_num, (uint32)(1UL << calib_warp_info_2nd->tile_width_exp), calib_warp_info_2nd->vin_sensor_geo.width, DC_U, DC_U);
            rval |= IK_ERR_0008;
        }

        if (((calib_warp_info_2nd->ver_grid_num - 1U) << calib_warp_info_2nd->tile_height_exp) < calib_warp_info_2nd->vin_sensor_geo.height) {
            amba_ik_system_print_uint32_5("[IK] (ver_grid_num %d - 1) * tile_height_exp %d < Calib vin width %d", calib_warp_info_2nd->ver_grid_num, (uint32)(1UL << calib_warp_info_2nd->tile_height_exp), calib_warp_info_2nd->vin_sensor_geo.height, DC_U, DC_U);
            rval |= IK_ERR_0008;
        }
    }
    return rval;
}

static uint32 ctx_check_warp_bind_info_valid_range(const ik_bind_info_t *warp_bind_info)
{
    uint32 rval = IK_OK;
    if (warp_bind_info == NULL) {
        rval |= IK_ERR_0005;
    }
    return rval;
}

#define WARP_VIDEO_MAXHORGRID    (128U)
#define WARP_VIDEO_MAXVERGRID    (96U)
#define CAWARP_VIDEO_MAXHORGRID    (32U)
#define CAWARP_VIDEO_MAXVERGRID    (48U)
static uint32 ctx_check_warp_internal_valid_range(const ik_warp_internal_info_t *warp_internal)
{
    uint32 rval = IK_OK;
    uintptr misra_uintptr_01, misra_uintptr_02;
    if (warp_internal == NULL) {
        (void)amba_ik_system_memcpy(&misra_uintptr_01, &warp_internal, sizeof(uint32*));
        amba_ik_system_print_uint32_5("[IK] warp_internal :0x%x%x", GetU64Msb(misra_uintptr_01), GetU64Lsb(misra_uintptr_01), DC_U, DC_U, DC_U);
        rval |= IK_ERR_0005;
    } else if((warp_internal->pwarp_horizontal_table == NULL) || (warp_internal->pwarp_vertical_table == NULL)) {
        (void)amba_ik_system_memcpy(&misra_uintptr_01, &warp_internal->pwarp_horizontal_table, sizeof(uint32*));
        (void)amba_ik_system_memcpy(&misra_uintptr_02, &warp_internal->pwarp_vertical_table, sizeof(uint32*));
        amba_ik_system_print_uint32_5("[IK]pwarp_horizontal_table :0x%x%x, pwarp_vertical_table :0x%x%x  ", GetU64Msb(misra_uintptr_01), GetU64Lsb(misra_uintptr_01), GetU64Msb(misra_uintptr_02), GetU64Lsb(misra_uintptr_02), DC_U);
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

static uint32 ctx_check_warp_2nd_internal_valid_range(const ik_warp_2nd_internal_info_t *warp_2nd_internal)
{
    uint32 rval = IK_OK;
    uintptr misra_uintptr_01;
    if (warp_2nd_internal == NULL) {
        (void)amba_ik_system_memcpy(&misra_uintptr_01, &warp_2nd_internal, sizeof(uint32*));
        amba_ik_system_print_uint32_5("[IK] warp_internal :0x%x%x", GetU64Msb(misra_uintptr_01), GetU64Lsb(misra_uintptr_01), DC_U, DC_U, DC_U);
        rval |= IK_ERR_0005;
    } else if(warp_2nd_internal->pwarp_horizontal_table == NULL) {
        (void)amba_ik_system_memcpy(&misra_uintptr_01, &warp_2nd_internal->pwarp_horizontal_table, sizeof(uint32*));
        amba_ik_system_print_uint32_5("[IK]pwarp_horizontal_table :0x%x%x ", GetU64Msb(misra_uintptr_01), GetU64Lsb(misra_uintptr_01), DC_U, DC_U, DC_U);
        rval |= IK_ERR_0008;
    } else {
        if (warp_2nd_internal->grid_array_width > WARP_VIDEO_MAXHORGRID) {
            amba_ik_system_print_uint32_5("[IK] [Error]: In video mode, warp_2nd_internal->grid_array_width = %d > WARP_VIDEO_MAXHORGRID (%d)"
                                          , warp_2nd_internal->grid_array_width, WARP_VIDEO_MAXHORGRID, DC_U, DC_U, DC_U);
            rval |= IK_ERR_0008;
        }
        if (warp_2nd_internal->grid_array_height> WARP_VIDEO_MAXVERGRID) {
            amba_ik_system_print_uint32_5("[IK] [Error]: In video mode, warp_2nd_internal->grid_array_height = %d > WARP_VIDEO_MAXVERGRID (%d)"
                                          , warp_2nd_internal->grid_array_height, WARP_VIDEO_MAXVERGRID, DC_U, DC_U, DC_U);
            rval |= IK_ERR_0008;
        }
    }
    return rval;
}

static uint32 ctx_check_ca_warp_internal_valid_range(const ik_cawarp_internal_info_t *cawarp_internal)
{
    uint32 rval = IK_OK;
    uintptr misra_uintptr;;
    if (cawarp_internal == NULL) {
        (void)amba_ik_system_memcpy(&misra_uintptr, &cawarp_internal, sizeof(uint32*));
        amba_ik_system_print_uint32_5("[IK] cawarp_internal :0x%x%x", GetU64Msb(misra_uintptr), GetU64Lsb(misra_uintptr), DC_U, DC_U, DC_U);
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

static uint32 ctx_check_video_mctf_internal_valid_range(const ik_mctf_internal_t *video_mctf_internal)
{
    uint32 rval = IK_OK;
    if (video_mctf_internal == NULL) {
        rval |= IK_ERR_0005;
    } else {
        rval |= (uint32)ctx_check_uint32_parameter_valid_range("video_mctf_internal->ta_frames_combine_num1", video_mctf_internal->ta_frames_combine_num1, 0, 6);
        rval |= (uint32)ctx_check_uint32_parameter_valid_range("video_mctf_internal->still_thta_frames_combine_num2resh", video_mctf_internal->ta_frames_combine_num2, 0, 6);
        rval |= (uint32)ctx_check_uint32_parameter_valid_range("video_mctf_internal->y_10", video_mctf_internal->y_10, 0, 1);
    }
    return rval;
}

static uint32 ctx_check_motion_detect_and_mctf_valid_range(const ik_motion_detect_and_mctf_t *p_motion_detect_and_mctf)
{
    uint32 rval = IK_OK;
    if (p_motion_detect_and_mctf == NULL) {
        rval |= IK_ERR_0005;
    } else {
        rval |= (uint32)ctx_check_uint32_parameter_valid_range("motion_detect_and_mctf->local_lighting_radius", p_motion_detect_and_mctf->local_lighting_radius, 0, 68);
    }
    return rval;
}

static uint32 ctx_check_aaa_stat_info_valid_range(const ik_aaa_stat_info_t *p_stat_info)
{
    uint32 rval = IK_OK;

    if (p_stat_info == NULL) {
        rval = IK_ERR_0005;
    } else {
        rval |= (uint32) ctx_check_uint32_parameter_valid_range("stat_info->awb_tile_num_col", p_stat_info->awb_tile_num_col, 2U, 64U);
        rval |= (uint32) ctx_check_uint32_parameter_valid_range("stat_info->awb_tile_num_row", p_stat_info->awb_tile_num_row, 2U, 64U);
        rval |= (uint32) ctx_check_uint32_parameter_valid_range("stat_info->ae_tile_num_col", p_stat_info->ae_tile_num_col, 2U, 24U);
        rval |= (uint32) ctx_check_uint32_parameter_valid_range("stat_info->ae_tile_num_row", p_stat_info->ae_tile_num_row, 2U, 16U);
        rval |= (uint32) ctx_check_uint32_parameter_valid_range("stat_info->af_tile_num_col", p_stat_info->af_tile_num_col, 2U, 24U);
        rval |= (uint32) ctx_check_uint32_parameter_valid_range("stat_info->af_tile_num_row", p_stat_info->af_tile_num_row, 2U, 16U);
        rval |= (uint32) ctx_check_uint32_parameter_valid_range("stat_info->six_minus_cfa_input_shift", p_stat_info->six_minus_cfa_input_shift, 0U, 6U);
        rval |= (uint32) ctx_check_uint32_parameter_valid_range("stat_info->awb_pix_min_value", p_stat_info->awb_pix_min_value, 0U, 16383U);
        rval |= (uint32) ctx_check_uint32_parameter_valid_range("stat_info->awb_pix_max_value", p_stat_info->awb_pix_max_value, 0U, 16383U);
        rval |= (uint32) ctx_check_uint32_parameter_valid_range("stat_info->ae_pix_min_value", p_stat_info->ae_pix_min_value, 0U, 16383U);
        rval |= (uint32) ctx_check_uint32_parameter_valid_range("stat_info->ae_pix_max_value", p_stat_info->ae_pix_max_value, 0U, 16383U);
        if (p_stat_info->awb_pix_min_value > p_stat_info->awb_pix_max_value) {
            amba_ik_system_print_uint32_5("[IK] [Error]: p_stat_info->awb_pix_min_value (%d) > p_stat_info->awb_pix_max_value (%d)"
                                          , p_stat_info->awb_pix_min_value, p_stat_info->awb_pix_max_value, DC_U, DC_U, DC_U);
            rval |= IK_ERR_0008;
        }
        if (p_stat_info->ae_pix_min_value > p_stat_info->ae_pix_max_value) {
            amba_ik_system_print_uint32_5("[IK] [Error]: p_stat_info->ae_pix_min_value (%d) > p_stat_info->ae_pix_max_value (%d)"
                                          , p_stat_info->ae_pix_min_value, p_stat_info->ae_pix_max_value, DC_U, DC_U, DC_U);
            rval |= IK_ERR_0008;
        }
        if ((p_stat_info->awb_tile_col_start+ (p_stat_info->awb_tile_width * p_stat_info->awb_tile_num_col)) > 4096U) {
            amba_ik_system_print_uint32_5("[IK] [Error]: p_stat_info->awb_tile_col_start (%d) + [p_stat_info->awb_tile_width (%d) * p_stat_info->awb_tile_num_col (%d)] > 4096 pseudo domain"
                                          , p_stat_info->awb_tile_col_start, p_stat_info->awb_tile_width, p_stat_info->awb_tile_num_col, DC_U, DC_U);
            rval |= IK_ERR_0008;
        }
        if ((p_stat_info->awb_tile_row_start+ (p_stat_info->awb_tile_height * p_stat_info->awb_tile_num_row)) > 4096U) {
            amba_ik_system_print_uint32_5("[IK] [Error]: p_stat_info->awb_tile_row_start (%d) + [p_stat_info->awb_tile_height (%d) * p_stat_info->awb_tile_num_row (%d)] > 4096 pseudo domain"
                                          , p_stat_info->awb_tile_row_start, p_stat_info->awb_tile_height, p_stat_info->awb_tile_num_row, DC_U, DC_U);
            rval |= IK_ERR_0008;
        }
        if ((p_stat_info->ae_tile_col_start+ (p_stat_info->ae_tile_width * p_stat_info->ae_tile_num_col)) > 4096U) {
            amba_ik_system_print_uint32_5("[IK] [Error]: p_stat_info->ae_tile_col_start (%d) + [p_stat_info->ae_tile_width (%d) * p_stat_info->ae_tile_num_col (%d)] > 4096 pseudo domain"
                                          , p_stat_info->ae_tile_col_start, p_stat_info->ae_tile_width, p_stat_info->ae_tile_num_col, DC_U, DC_U);
            rval |= IK_ERR_0008;
        }
        if ((p_stat_info->ae_tile_row_start+ (p_stat_info->ae_tile_height * p_stat_info->ae_tile_num_row)) > 4096U) {
            amba_ik_system_print_uint32_5("[IK] [Error]: p_stat_info->ae_tile_row_start (%d) + [p_stat_info->ae_tile_height (%d) * p_stat_info->ae_tile_num_row (%d)] > 4096 pseudo domain"
                                          , p_stat_info->ae_tile_row_start, p_stat_info->ae_tile_height, p_stat_info->ae_tile_num_row, DC_U, DC_U);
            rval |= IK_ERR_0008;
        }
        if ((p_stat_info->af_tile_col_start+ (p_stat_info->af_tile_width * p_stat_info->af_tile_num_col)) > 4096U) {
            amba_ik_system_print_uint32_5("[IK] [Error]: p_stat_info->af_tile_col_start (%d) + [p_stat_info->af_tile_width (%d) * p_stat_info->af_tile_num_col (%d)] > 4096 pseudo domain"
                                          , p_stat_info->af_tile_col_start, p_stat_info->af_tile_width, p_stat_info->af_tile_num_col, DC_U, DC_U);
            rval |= IK_ERR_0008;
        }
        if ((p_stat_info->af_tile_row_start+ (p_stat_info->af_tile_height * p_stat_info->af_tile_num_row)) > 4096U) {
            amba_ik_system_print_uint32_5("[IK] [Error]: p_stat_info->af_tile_row_start (%d) + [p_stat_info->af_tile_height (%d) * p_stat_info->af_tile_num_row (%d)] > 4096 pseudo domain"
                                          , p_stat_info->af_tile_row_start, p_stat_info->af_tile_height, p_stat_info->af_tile_num_row, DC_U, DC_U);
            rval |= IK_ERR_0008;
        }
        if (p_stat_info->awb_tile_active_width > p_stat_info->awb_tile_width) {
            amba_ik_system_print_uint32_5("[IK] [Error]: p_stat_info->awb_tile_active_width (%d) > p_stat_info->awb_tile_width (%d)"
                                          , p_stat_info->awb_tile_active_width, p_stat_info->awb_tile_width, DC_U, DC_U, DC_U);
            rval |= IK_ERR_0008;
        }
        if (p_stat_info->awb_tile_active_height > p_stat_info->awb_tile_height) {
            amba_ik_system_print_uint32_5("[IK] [Error]: p_stat_info->awb_tile_active_height (%d) > p_stat_info->awb_tile_height (%d)"
                                          , p_stat_info->awb_tile_active_height, p_stat_info->awb_tile_height, DC_U, DC_U, DC_U);
            rval |= IK_ERR_0008;
        }
        if (p_stat_info->af_tile_active_width > p_stat_info->af_tile_width) {
            amba_ik_system_print_uint32_5("[IK] [Error]: p_stat_info->af_tile_active_width (%d) > p_stat_info->af_tile_width (%d)"
                                          , p_stat_info->af_tile_active_width, p_stat_info->af_tile_width, DC_U, DC_U, DC_U);
            rval |= IK_ERR_0008;
        }
        if (p_stat_info->af_tile_active_height > p_stat_info->af_tile_height) {
            amba_ik_system_print_uint32_5("[IK] [Error]: p_stat_info->af_tile_active_height (%d) > p_stat_info->af_tile_height (%d)"
                                          , p_stat_info->af_tile_active_height, p_stat_info->af_tile_height, DC_U, DC_U, DC_U);
            rval |= IK_ERR_0008;
        }
    }

    return rval;
}

static uint32 ctx_check_aaa_pg_af_info_valid_range(const ik_aaa_pg_af_stat_info_t *p_pg_af_stat_info)
{
    uint32 rval = IK_OK;

    if (p_pg_af_stat_info == NULL) {
        rval = IK_ERR_0005;
    } else {
        rval |= (uint32) ctx_check_uint32_parameter_valid_range("p_pg_af_stat_info->af_tile_num_col", p_pg_af_stat_info->af_tile_num_col, 2U, 32U);
        rval |= (uint32) ctx_check_uint32_parameter_valid_range("p_pg_af_stat_info->af_tile_num_row", p_pg_af_stat_info->af_tile_num_row, 2U, 16U);
        if ((p_pg_af_stat_info->af_tile_col_start+ (p_pg_af_stat_info->af_tile_width * p_pg_af_stat_info->af_tile_num_col)) > 4096U) {
            amba_ik_system_print_uint32_5("[IK] [Error]: p_pg_af_stat_info->af_tile_col_start (%d) + [p_pg_af_stat_info->af_tile_width (%d) * p_pg_af_stat_info->af_tile_num_col (%d)] > 4096 pseudo domain"
                                          , p_pg_af_stat_info->af_tile_col_start, p_pg_af_stat_info->af_tile_width, p_pg_af_stat_info->af_tile_num_col, DC_U, DC_U);
            rval |= IK_ERR_0008;
        }
        if ((p_pg_af_stat_info->af_tile_row_start+ (p_pg_af_stat_info->af_tile_height * p_pg_af_stat_info->af_tile_num_row)) > 4096U) {
            amba_ik_system_print_uint32_5("[IK] [Error]: p_pg_af_stat_info->af_tile_row_start (%d) + [p_pg_af_stat_info->af_tile_height (%d) * p_pg_af_stat_info->af_tile_num_row (%d)] > 4096 pseudo domain"
                                          , p_pg_af_stat_info->af_tile_row_start, p_pg_af_stat_info->af_tile_height, p_pg_af_stat_info->af_tile_num_row, DC_U, DC_U);
            rval |= IK_ERR_0008;
        }
        if (p_pg_af_stat_info->af_tile_active_width > p_pg_af_stat_info->af_tile_width) {
            amba_ik_system_print_uint32_5("[IK] [Error]: p_pg_af_stat_info->af_tile_active_width (%d) > p_pg_af_stat_info->af_tile_width (%d)"
                                          , p_pg_af_stat_info->af_tile_active_width, p_pg_af_stat_info->af_tile_width, DC_U, DC_U, DC_U);
            rval |= IK_ERR_0008;
        }
        if (p_pg_af_stat_info->af_tile_active_height > p_pg_af_stat_info->af_tile_height) {
            amba_ik_system_print_uint32_5("[IK] [Error]: p_pg_af_stat_info->af_tile_active_height (%d) > p_pg_af_stat_info->af_tile_height (%d)"
                                          , p_pg_af_stat_info->af_tile_active_height, p_pg_af_stat_info->af_tile_height, DC_U, DC_U, DC_U);
            rval |= IK_ERR_0008;
        }
    }

    return rval;
}

static uint32 ctx_check_af_stat_ex_info_valid_range(const ik_af_stat_ex_info_t *af_stat_ex_info)
{
    uint32 rval = IK_OK;
    if (af_stat_ex_info == NULL) {
        rval = IK_ERR_0005;
    } else {
        rval |= ctx_check_uint32_parameter_valid_range("af_stat_ex_info->af_horizontal_filter1_mode", af_stat_ex_info->af_horizontal_filter1_mode, 0, 1U);
        rval |= ctx_check_uint32_parameter_valid_range("af_stat_ex_info->af_horizontal_filter1_stage1_enb", af_stat_ex_info->af_horizontal_filter1_stage1_enb, 0, 1U);
        rval |= ctx_check_uint32_parameter_valid_range("af_stat_ex_info->af_horizontal_filter1_stage2_enb", af_stat_ex_info->af_horizontal_filter1_stage2_enb, 0, 1U);
        rval |= ctx_check_uint32_parameter_valid_range("af_stat_ex_info->af_horizontal_filter1_stage3_enb", af_stat_ex_info->af_horizontal_filter1_stage3_enb, 0, 1U);
        rval |= ctx_check_uint32_parameter_valid_range("af_stat_ex_info->af_horizontal_filter1_gain[0]", (uint32)af_stat_ex_info->af_horizontal_filter1_gain[0], 0, 255U);
        rval |= ctx_check_int32_parameter_valid_range("af_stat_ex_info->af_horizontal_filter1_gain[1]", af_stat_ex_info->af_horizontal_filter1_gain[1], -512, 511);
        rval |= ctx_check_int32_parameter_valid_range("af_stat_ex_info->af_horizontal_filter1_gain[2]", af_stat_ex_info->af_horizontal_filter1_gain[2], -512, 511);
        rval |= ctx_check_int32_parameter_valid_range("af_stat_ex_info->af_horizontal_filter1_gain[3]", af_stat_ex_info->af_horizontal_filter1_gain[3], -512, 511);
        rval |= ctx_check_int32_parameter_valid_range("af_stat_ex_info->af_horizontal_filter1_gain[4]", af_stat_ex_info->af_horizontal_filter1_gain[4], -512, 511);
        rval |= ctx_check_int32_parameter_valid_range("af_stat_ex_info->af_horizontal_filter1_gain[5]", af_stat_ex_info->af_horizontal_filter1_gain[5], -512, 511);
        rval |= ctx_check_int32_parameter_valid_range("af_stat_ex_info->af_horizontal_filter1_gain[6]", af_stat_ex_info->af_horizontal_filter1_gain[6], -512, 511);
        rval |= ctx_check_uint32_parameter_valid_range("af_stat_ex_info->af_horizontal_filter1_shift[0]", af_stat_ex_info->af_horizontal_filter1_shift[0], 0, 15U);
        rval |= ctx_check_uint32_parameter_valid_range("af_stat_ex_info->af_horizontal_filter1_shift[1]", af_stat_ex_info->af_horizontal_filter1_shift[1], 0, 15U);
        rval |= ctx_check_uint32_parameter_valid_range("af_stat_ex_info->af_horizontal_filter1_shift[2]", af_stat_ex_info->af_horizontal_filter1_shift[2], 0, 15U);
        rval |= ctx_check_uint32_parameter_valid_range("af_stat_ex_info->af_horizontal_filter1_shift[3]", af_stat_ex_info->af_horizontal_filter1_shift[3], 0, 15U);
        rval |= ctx_check_int32_parameter_valid_range("af_stat_ex_info->af_horizontal_filter1_bias_off", af_stat_ex_info->af_horizontal_filter1_bias_off, -2048, 2047);
        rval |= ctx_check_uint32_parameter_valid_range("af_stat_ex_info->af_horizontal_filter1_thresh", af_stat_ex_info->af_horizontal_filter1_thresh, 0, 2047U);
        rval |= ctx_check_uint32_parameter_valid_range("af_stat_ex_info->af_horizontal_filter2_mode", af_stat_ex_info->af_horizontal_filter2_mode, 0, 1U);
        rval |= ctx_check_uint32_parameter_valid_range("af_stat_ex_info->af_horizontal_filter2_stage1_enb", af_stat_ex_info->af_horizontal_filter2_stage1_enb, 0, 1U);
        rval |= ctx_check_uint32_parameter_valid_range("af_stat_ex_info->af_horizontal_filter2_stage2_enb", af_stat_ex_info->af_horizontal_filter2_stage2_enb, 0, 1U);
        rval |= ctx_check_uint32_parameter_valid_range("af_stat_ex_info->af_horizontal_filter2_stage3_enb", af_stat_ex_info->af_horizontal_filter2_stage3_enb, 0, 1U);
        rval |= ctx_check_uint32_parameter_valid_range("af_stat_ex_info->af_horizontal_filter2_gain[0]", (uint32)af_stat_ex_info->af_horizontal_filter2_gain[0], 0, 255U);
        rval |= ctx_check_int32_parameter_valid_range("af_stat_ex_info->af_horizontal_filter2_gain[1]", af_stat_ex_info->af_horizontal_filter2_gain[1], -512, 511);
        rval |= ctx_check_int32_parameter_valid_range("af_stat_ex_info->af_horizontal_filter2_gain[2]", af_stat_ex_info->af_horizontal_filter2_gain[2], -512, 511);
        rval |= ctx_check_int32_parameter_valid_range("af_stat_ex_info->af_horizontal_filter2_gain[3]", af_stat_ex_info->af_horizontal_filter2_gain[3], -512, 511);
        rval |= ctx_check_int32_parameter_valid_range("af_stat_ex_info->af_horizontal_filter2_gain[4]", af_stat_ex_info->af_horizontal_filter2_gain[4], -512, 511);
        rval |= ctx_check_int32_parameter_valid_range("af_stat_ex_info->af_horizontal_filter2_gain[5]", af_stat_ex_info->af_horizontal_filter2_gain[5], -512, 511);
        rval |= ctx_check_int32_parameter_valid_range("af_stat_ex_info->af_horizontal_filter2_gain[6]", af_stat_ex_info->af_horizontal_filter2_gain[6], -512, 511);
        rval |= ctx_check_uint32_parameter_valid_range("af_stat_ex_info->af_horizontal_filter2_shift[0]", af_stat_ex_info->af_horizontal_filter2_shift[0], 0, 15U);
        rval |= ctx_check_uint32_parameter_valid_range("af_stat_ex_info->af_horizontal_filter2_shift[1]", af_stat_ex_info->af_horizontal_filter2_shift[1], 0, 15U);
        rval |= ctx_check_uint32_parameter_valid_range("af_stat_ex_info->af_horizontal_filter2_shift[2]", af_stat_ex_info->af_horizontal_filter2_shift[2], 0, 15U);
        rval |= ctx_check_uint32_parameter_valid_range("af_stat_ex_info->af_horizontal_filter2_shift[3]", af_stat_ex_info->af_horizontal_filter2_shift[3], 0, 15U);
        rval |= ctx_check_int32_parameter_valid_range("af_stat_ex_info->af_horizontal_filter2_bias_off", af_stat_ex_info->af_horizontal_filter2_bias_off, -2048, 2047);
        rval |= ctx_check_uint32_parameter_valid_range("af_stat_ex_info->af_horizontal_filter2_thresh", af_stat_ex_info->af_horizontal_filter2_thresh, 0, 2047U);
        rval |= ctx_check_uint32_parameter_valid_range("af_stat_ex_info->af_tile_fv1_horizontal_shift", af_stat_ex_info->af_tile_fv1_horizontal_shift, 0, 31U);
        rval |= ctx_check_uint32_parameter_valid_range("af_stat_ex_info->af_tile_fv2_horizontal_shift", af_stat_ex_info->af_tile_fv2_horizontal_shift, 0, 31U);
    }

    return rval;
}

static uint32 ctx_check_pg_af_stat_ex_info_valid_range(const ik_pg_af_stat_ex_info_t *pg_af_stat_ex_info)
{
    uint32 rval = IK_OK;
    if (pg_af_stat_ex_info == NULL) {
        rval = IK_ERR_0005;
    } else {
        rval |= ctx_check_uint32_parameter_valid_range("pg_af_stat_ex_info->af_horizontal_filter1_mode", pg_af_stat_ex_info->af_horizontal_filter1_mode, 0, 1U);
        rval |= ctx_check_uint32_parameter_valid_range("pg_af_stat_ex_info->af_horizontal_filter1_stage1_enb", pg_af_stat_ex_info->af_horizontal_filter1_stage1_enb, 0, 1U);
        rval |= ctx_check_uint32_parameter_valid_range("pg_af_stat_ex_info->af_horizontal_filter1_stage2_enb", pg_af_stat_ex_info->af_horizontal_filter1_stage2_enb, 0, 1U);
        rval |= ctx_check_uint32_parameter_valid_range("pg_af_stat_ex_info->af_horizontal_filter1_stage3_enb", pg_af_stat_ex_info->af_horizontal_filter1_stage3_enb, 0, 1U);
        rval |= ctx_check_uint32_parameter_valid_range("pg_af_stat_ex_info->af_horizontal_filter1_gain[0]", (uint32)pg_af_stat_ex_info->af_horizontal_filter1_gain[0], 0, 255U);
        rval |= ctx_check_int32_parameter_valid_range("pg_af_stat_ex_info->af_horizontal_filter1_gain[1]", pg_af_stat_ex_info->af_horizontal_filter1_gain[1], -512, 511);
        rval |= ctx_check_int32_parameter_valid_range("pg_af_stat_ex_info->af_horizontal_filter1_gain[2]", pg_af_stat_ex_info->af_horizontal_filter1_gain[2], -512, 511);
        rval |= ctx_check_int32_parameter_valid_range("pg_af_stat_ex_info->af_horizontal_filter1_gain[3]", pg_af_stat_ex_info->af_horizontal_filter1_gain[3], -512, 511);
        rval |= ctx_check_int32_parameter_valid_range("pg_af_stat_ex_info->af_horizontal_filter1_gain[4]", pg_af_stat_ex_info->af_horizontal_filter1_gain[4], -512, 511);
        rval |= ctx_check_int32_parameter_valid_range("pg_af_stat_ex_info->af_horizontal_filter1_gain[5]", pg_af_stat_ex_info->af_horizontal_filter1_gain[5], -512, 511);
        rval |= ctx_check_int32_parameter_valid_range("pg_af_stat_ex_info->af_horizontal_filter1_gain[6]", pg_af_stat_ex_info->af_horizontal_filter1_gain[6], -512, 511);
        rval |= ctx_check_uint32_parameter_valid_range("pg_af_stat_ex_info->af_horizontal_filter1_shift[0]", pg_af_stat_ex_info->af_horizontal_filter1_shift[0], 0, 15U);
        rval |= ctx_check_uint32_parameter_valid_range("pg_af_stat_ex_info->af_horizontal_filter1_shift[1]", pg_af_stat_ex_info->af_horizontal_filter1_shift[1], 0, 15U);
        rval |= ctx_check_uint32_parameter_valid_range("pg_af_stat_ex_info->af_horizontal_filter1_shift[2]", pg_af_stat_ex_info->af_horizontal_filter1_shift[2], 0, 15U);
        rval |= ctx_check_uint32_parameter_valid_range("pg_af_stat_ex_info->af_horizontal_filter1_shift[3]", pg_af_stat_ex_info->af_horizontal_filter1_shift[3], 0, 15U);
        rval |= ctx_check_int32_parameter_valid_range("pg_af_stat_ex_info->af_horizontal_filter1_bias_off", pg_af_stat_ex_info->af_horizontal_filter1_bias_off, -2048, 2047);
        rval |= ctx_check_uint32_parameter_valid_range("pg_af_stat_ex_info->af_horizontal_filter1_thresh", pg_af_stat_ex_info->af_horizontal_filter1_thresh, 0, 2047U);
        rval |= ctx_check_uint32_parameter_valid_range("pg_af_stat_ex_info->af_vertical_filter1_thresh", pg_af_stat_ex_info->af_vertical_filter1_thresh, 0, 2047U);
        rval |= ctx_check_uint32_parameter_valid_range("pg_af_stat_ex_info->af_horizontal_filter2_mode", pg_af_stat_ex_info->af_horizontal_filter2_mode, 0, 1U);
        rval |= ctx_check_uint32_parameter_valid_range("pg_af_stat_ex_info->af_horizontal_filter2_stage1_enb", pg_af_stat_ex_info->af_horizontal_filter2_stage1_enb, 0, 1U);
        rval |= ctx_check_uint32_parameter_valid_range("pg_af_stat_ex_info->af_horizontal_filter2_stage2_enb", pg_af_stat_ex_info->af_horizontal_filter2_stage2_enb, 0, 1U);
        rval |= ctx_check_uint32_parameter_valid_range("pg_af_stat_ex_info->af_horizontal_filter2_stage3_enb", pg_af_stat_ex_info->af_horizontal_filter2_stage3_enb, 0, 1U);
        rval |= ctx_check_uint32_parameter_valid_range("pg_af_stat_ex_info->af_horizontal_filter2_gain[0]", (uint32)pg_af_stat_ex_info->af_horizontal_filter2_gain[0], 0, 255U);
        rval |= ctx_check_int32_parameter_valid_range("pg_af_stat_ex_info->af_horizontal_filter2_gain[1]", pg_af_stat_ex_info->af_horizontal_filter2_gain[1], -512, 511);
        rval |= ctx_check_int32_parameter_valid_range("pg_af_stat_ex_info->af_horizontal_filter2_gain[2]", pg_af_stat_ex_info->af_horizontal_filter2_gain[2], -512, 511);
        rval |= ctx_check_int32_parameter_valid_range("pg_af_stat_ex_info->af_horizontal_filter2_gain[3]", pg_af_stat_ex_info->af_horizontal_filter2_gain[3], -512, 511);
        rval |= ctx_check_int32_parameter_valid_range("pg_af_stat_ex_info->af_horizontal_filter2_gain[4]", pg_af_stat_ex_info->af_horizontal_filter2_gain[4], -512, 511);
        rval |= ctx_check_int32_parameter_valid_range("pg_af_stat_ex_info->af_horizontal_filter2_gain[5]", pg_af_stat_ex_info->af_horizontal_filter2_gain[5], -512, 511);
        rval |= ctx_check_int32_parameter_valid_range("pg_af_stat_ex_info->af_horizontal_filter2_gain[6]", pg_af_stat_ex_info->af_horizontal_filter2_gain[6], -512, 511);
        rval |= ctx_check_uint32_parameter_valid_range("pg_af_stat_ex_info->af_horizontal_filter2_shift[0]", pg_af_stat_ex_info->af_horizontal_filter2_shift[0], 0, 15U);
        rval |= ctx_check_uint32_parameter_valid_range("pg_af_stat_ex_info->af_horizontal_filter2_shift[1]", pg_af_stat_ex_info->af_horizontal_filter2_shift[1], 0, 15U);
        rval |= ctx_check_uint32_parameter_valid_range("pg_af_stat_ex_info->af_horizontal_filter2_shift[2]", pg_af_stat_ex_info->af_horizontal_filter2_shift[2], 0, 15U);
        rval |= ctx_check_uint32_parameter_valid_range("pg_af_stat_ex_info->af_horizontal_filter2_shift[3]", pg_af_stat_ex_info->af_horizontal_filter2_shift[3], 0, 15U);
        rval |= ctx_check_int32_parameter_valid_range("pg_af_stat_ex_info->af_horizontal_filter2_bias_off", pg_af_stat_ex_info->af_horizontal_filter2_bias_off, -2048, 2047);
        rval |= ctx_check_uint32_parameter_valid_range("pg_af_stat_ex_info->af_horizontal_filter2_thresh", pg_af_stat_ex_info->af_horizontal_filter2_thresh, 0, 2047U);
        rval |= ctx_check_uint32_parameter_valid_range("pg_af_stat_ex_info->af_vertical_filter2_thresh", pg_af_stat_ex_info->af_vertical_filter2_thresh, 0, 2047U);
        rval |= ctx_check_uint32_parameter_valid_range("pg_af_stat_ex_info->af_tile_fv1_horizontal_shift", pg_af_stat_ex_info->af_tile_fv1_horizontal_shift, 0, 31U);
        rval |= ctx_check_uint32_parameter_valid_range("pg_af_stat_ex_info->af_tile_fv1_vertical_shift", pg_af_stat_ex_info->af_tile_fv1_vertical_shift, 0, 31U);
        rval |= ctx_check_uint32_parameter_valid_range("pg_af_stat_ex_info->af_tile_fv1_horizontal_weight", pg_af_stat_ex_info->af_tile_fv1_horizontal_weight, 0, 255U);
        rval |= ctx_check_uint32_parameter_valid_range("pg_af_stat_ex_info->af_tile_fv1_vertical_weight", pg_af_stat_ex_info->af_tile_fv1_vertical_weight, 0, 255U);
        rval |= ctx_check_uint32_parameter_valid_range("pg_af_stat_ex_info->af_tile_fv2_horizontal_shift", pg_af_stat_ex_info->af_tile_fv2_horizontal_shift, 0, 31U);
        rval |= ctx_check_uint32_parameter_valid_range("pg_af_stat_ex_info->af_tile_fv2_vertical_shift", pg_af_stat_ex_info->af_tile_fv2_vertical_shift, 0, 31U);
        rval |= ctx_check_uint32_parameter_valid_range("pg_af_stat_ex_info->af_tile_fv2_horizontal_weight", pg_af_stat_ex_info->af_tile_fv2_horizontal_weight, 0, 255U);
        rval |= ctx_check_uint32_parameter_valid_range("pg_af_stat_ex_info->af_tile_fv2_vertical_weight", pg_af_stat_ex_info->af_tile_fv2_vertical_weight, 0, 255U);

    }

    return rval;
}

static uint32 ctx_check_wide_chroma_filter_valid_range(const ik_wide_chroma_filter_t *wide_chroma_filter)
{
    uint32 rval = IK_OK;
    if (wide_chroma_filter == NULL) {
        rval |= IK_ERR_0005;
    } else {
        rval |= (uint32)ctx_check_uint32_parameter_valid_range("wide_chroma_filter->enable", wide_chroma_filter->enable, 0, 1);
        rval |= (uint32)ctx_check_uint32_parameter_valid_range("wide_chroma_filter->noise_level_cb", wide_chroma_filter->noise_level_cb, 0, 255);
        rval |= (uint32)ctx_check_uint32_parameter_valid_range("wide_chroma_filter->noise_level_cr", wide_chroma_filter->noise_level_cr, 0, 255);
    }

    return rval;
}

static uint32 ctx_check_wide_chroma_filter_combine(const ik_wide_chroma_filter_combine_t *p_wide_chroma_filter_combine)
{
    uint32 rval = IK_OK;
    if (p_wide_chroma_filter_combine == NULL) {
        rval |= IK_ERR_0005;
    } else {
        rval |= (uint32)ctx_check_uint32_parameter_valid_range("wide_chroma_filter_combine->T0_cb", p_wide_chroma_filter_combine->T0_cb, 0U, 62U);
        rval |= (uint32)ctx_check_uint32_parameter_valid_range("wide_chroma_filter_combine->T0_cr", p_wide_chroma_filter_combine->T0_cr, 0U, 62U);
        rval |= (uint32)ctx_check_uint32_parameter_valid_range("wide_chroma_filter_combine->T1_cb", p_wide_chroma_filter_combine->T1_cb, 1U, 63U);
        rval |= (uint32)ctx_check_uint32_parameter_valid_range("wide_chroma_filter_combine->T1_cr", p_wide_chroma_filter_combine->T1_cr, 1U, 63U);
        if(p_wide_chroma_filter_combine->T0_cb > p_wide_chroma_filter_combine->T1_cb) {
            rval |= IK_ERR_0008;
            amba_ik_system_print_uint32_5("wide_chroma_filter_combine T0_cb=%d  should not larger than T1_cb=%d ", p_wide_chroma_filter_combine->T0_cb, p_wide_chroma_filter_combine->T1_cb, DC_U, DC_U, DC_U);
        }
        if(p_wide_chroma_filter_combine->T0_cr > p_wide_chroma_filter_combine->T1_cr) {
            rval |= IK_ERR_0008;
            amba_ik_system_print_uint32_5("wide_chroma_filter_combine T0_cr=%d  should not larger than T1_cr=%d ", p_wide_chroma_filter_combine->T0_cr, p_wide_chroma_filter_combine->T1_cr, DC_U, DC_U, DC_U);
        }
        rval |= (uint32)ctx_check_uint32_parameter_valid_range("wide_chroma_filter_combine->alpha_max_cb", p_wide_chroma_filter_combine->alpha_max_cb, 0U, 255U);
        rval |= (uint32)ctx_check_uint32_parameter_valid_range("wide_chroma_filter_combine->alpha_max_cr", p_wide_chroma_filter_combine->alpha_max_cr, 0U, 255U);
        rval |= (uint32)ctx_check_uint32_parameter_valid_range("wide_chroma_filter_combine->alpha_min_cb", p_wide_chroma_filter_combine->alpha_min_cb, 0U, 255U);
        rval |= (uint32)ctx_check_uint32_parameter_valid_range("wide_chroma_filter_combine->alpha_min_cr", p_wide_chroma_filter_combine->alpha_min_cr, 0U, 255U);

        rval |= (uint32)ctx_check_uint32_parameter_valid_range("wide_chroma_filter_combine->max_change_cb", \
                p_wide_chroma_filter_combine->max_change_cb, 0U, 255U);
        rval |= (uint32)ctx_check_uint32_parameter_valid_range("wide_chroma_filter_combine->max_change_cr", \
                p_wide_chroma_filter_combine->max_change_cr, 0U, 255U);
    }
    return rval;
}

static uint32 ctx_check_warp_buffer_info(const ik_warp_buffer_info_t *p_warp_buf_info)
{
    uint32 rval = IK_OK;
    if (p_warp_buf_info == NULL) {
        rval |= IK_ERR_0005;
    } else {
        rval |= ctx_check_uint32_parameter_valid_range("p_warp_buf_info->dram_efficiency", p_warp_buf_info->dram_efficiency, 0U, 2U);
        rval |= ctx_check_uint32_parameter_valid_range("p_warp_buf_info->luma_wait_lines", p_warp_buf_info->luma_wait_lines, 8U, 8191U);
        rval |= ctx_check_uint32_parameter_valid_range("p_warp_buf_info->luma_dma_size", p_warp_buf_info->luma_dma_size, 8U, 64U);
    }
    return rval;
}

static uint32 ctx_check_overlap_x(const ik_overlap_x_t *p_overlap_x)
{
    uint32 rval = IK_OK;
    if (p_overlap_x == NULL) {
        rval |= IK_ERR_0005;
    } else {
        rval |= ctx_check_uint32_parameter_valid_range("p_overlap_x->overlap_x", p_overlap_x->overlap_x, 0U, 1024);
    }
    return rval;
}

static uint32 ctx_check_stitching_info(const ik_stitch_info_t *p_stitch_info)
{
    uint32 rval = IK_OK;
    if (p_stitch_info == NULL) {
        rval |= IK_ERR_0005;
    } else {
        rval |= ctx_check_uint32_parameter_valid_range("p_stitch_info->enable", (uint32)p_stitch_info->enable, 0U, 1U);
        if(p_stitch_info->enable == 0) {
            rval |= ctx_check_uint32_parameter_valid_range("p_stitch_info->enable = 0, p_stitch_info->tile_num_x", (uint32)p_stitch_info->tile_num_x, 1U, 1U);
            rval |= ctx_check_uint32_parameter_valid_range("p_stitch_info->enable = 0, p_stitch_info->tile_num_y", (uint32)p_stitch_info->tile_num_y, 1U, 1U);
        } else {
            rval |= ctx_check_uint32_parameter_valid_range("p_stitch_info->tile_num_x", (uint32)p_stitch_info->tile_num_x, 1U, 12U);
            rval |= ctx_check_uint32_parameter_valid_range("p_stitch_info->tile_num_y", (uint32)p_stitch_info->tile_num_y, 1U, 8U);
        }
    }
    return rval;
}

static uint32 ctx_check_burst_tile(const ik_burst_tile_t *p_burst_tile)
{
    uint32 rval = IK_OK;
    if (p_burst_tile == NULL) {
        rval |= IK_ERR_0005;
    } else {
        rval |= ctx_check_uint32_parameter_valid_range("p_burst_tile->enable", (uint32)p_burst_tile->enable, 0U, 1U);
    }
    return rval;
}

static uint32 ctx_check_hi_asf_valid_range(const ik_hi_asf_t *p_hi_asf)
{
    uint32 rval = IK_OK;
    if (p_hi_asf == NULL) {
        amba_ik_system_print_str_5("[IK] p_hi_asf input is NULL address!", NULL, NULL, NULL, NULL, NULL);
        rval |= IK_ERR_0005;
    } else {
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
    return rval;
}

static uint32 ctx_check_li2_asf_valid_range(const ik_li2_asf_t *p_li2_asf)
{
    uint32 rval = IK_OK;
    if (p_li2_asf == NULL) {
        amba_ik_system_print_str_5("[IK] p_li2_asf input is NULL address!", NULL, NULL, NULL, NULL, NULL);
        rval |= IK_ERR_0005;
    } else {
        {
            // advanced level control check
            ik_level_t level;

            level.low = p_li2_asf->level_str_adjust.low;
            level.low_delta = p_li2_asf->level_str_adjust.low_delta;
            level.high = p_li2_asf->level_str_adjust.high;
            level.high_delta = p_li2_asf->level_str_adjust.high_delta;
            rval |= check_advanced_level_control("li2_advanced_spatial_filter->level_str_adjust", &level);

            level.low = p_li2_asf->t0_t1_div.low;
            level.low_delta = p_li2_asf->t0_t1_div.low_delta;
            level.high = p_li2_asf->t0_t1_div.high;
            level.high_delta = p_li2_asf->t0_t1_div.high_delta;
            rval |= check_advanced_level_control("li2_advanced_spatial_filter->t0_t1_div", &level);
        }
    }
    return rval;
}

static uint32 ctx_check_hi_high2_asf_valid_range(const ik_hi_high2_asf_t *p_hi_high2_asf)
{
    uint32 rval = IK_OK;
    if (p_hi_high2_asf == NULL) {
        amba_ik_system_print_str_5("[IK] p_hi_high2_asf input is NULL address!", NULL, NULL, NULL, NULL, NULL);
        rval |= IK_ERR_0005;
    } else {
        {
            // advanced level control check
            ik_level_method_t level;

            level.low = p_hi_high2_asf->level_str_adjust.low;
            level.low_delta = p_hi_high2_asf->level_str_adjust.low_delta;
            level.high = p_hi_high2_asf->level_str_adjust.high;
            level.high_delta = p_hi_high2_asf->level_str_adjust.high_delta;
            rval |= check_advanced_level_method_control("hi_high2_advanced_spatial_filter->level_str_adjust", &level);

            level.low = p_hi_high2_asf->t0_t1_div.low;
            level.low_delta = p_hi_high2_asf->t0_t1_div.low_delta;
            level.high = p_hi_high2_asf->t0_t1_div.high;
            level.high_delta = p_hi_high2_asf->t0_t1_div.high_delta;
            rval |= check_advanced_level_method_control("hi_high2_advanced_spatial_filter->t0_t1_div", &level);
        }
    }
    return rval;
}

static uint32 ctx_check_hi_med1_asf_valid_range(const ik_hi_med1_asf_t *p_hi_med1_asf)
{
    uint32 rval = IK_OK;
    if (p_hi_med1_asf == NULL) {
        amba_ik_system_print_str_5("[IK] p_hi_med1_asf input is NULL address!", NULL, NULL, NULL, NULL, NULL);
        rval |= IK_ERR_0005;
    } else {
        {
            // advanced level control check
            ik_level_method_t level;

            level.low = p_hi_med1_asf->level_str_adjust.low;
            level.low_delta = p_hi_med1_asf->level_str_adjust.low_delta;
            level.high = p_hi_med1_asf->level_str_adjust.high;
            level.high_delta = p_hi_med1_asf->level_str_adjust.high_delta;
            rval |= check_advanced_level_method_control("hi_med1_advanced_spatial_filter->level_str_adjust", &level);

            level.low = p_hi_med1_asf->t0_t1_div.low;
            level.low_delta = p_hi_med1_asf->t0_t1_div.low_delta;
            level.high = p_hi_med1_asf->t0_t1_div.high;
            level.high_delta = p_hi_med1_asf->t0_t1_div.high_delta;
            rval |= check_advanced_level_method_control("hi_med1_advanced_spatial_filter->t0_t1_div", &level);
        }
    }
    return rval;
}

static uint32 ctx_check_hi_low_asf_valid_range(const ik_hi_low_asf_t *p_hi_low_asf)
{
    uint32 rval = IK_OK;
    if (p_hi_low_asf == NULL) {
        amba_ik_system_print_str_5("[IK] ik_hi_low_asf_t input is NULL address!", NULL, NULL, NULL, NULL, NULL);
        rval |= IK_ERR_0005;
    } else {
        {
            // advanced level control check
            ik_level_t level;

            level.low = p_hi_low_asf->level_str_adjust.low;
            level.low_delta = p_hi_low_asf->level_str_adjust.low_delta;
            level.high = p_hi_low_asf->level_str_adjust.high;
            level.high_delta = p_hi_low_asf->level_str_adjust.high_delta;
            rval |= check_advanced_level_control("hi_low_advanced_spatial_filter->level_str_adjust", &level);

            level.low = p_hi_low_asf->t0_t1_div.low;
            level.low_delta = p_hi_low_asf->t0_t1_div.low_delta;
            level.high = p_hi_low_asf->t0_t1_div.high;
            level.high_delta = p_hi_low_asf->t0_t1_div.high_delta;
            rval |= check_advanced_level_control("hi_low_advanced_spatial_filter->t0_t1_div", &level);
        }
    }
    return rval;
}

static uint32 ctx_check_hi_med2_asf_valid_range(const ik_hi_med2_asf_t *p_hi_med2_asf)
{
    uint32 rval = IK_OK;
    if (p_hi_med2_asf == NULL) {
        amba_ik_system_print_str_5("[IK] p_hi_med2_asf input is NULL address!", NULL, NULL, NULL, NULL, NULL);
        rval |= IK_ERR_0005;
    } else {
        {
            // advanced level control check
            ik_level_method_t level;

            level.low = p_hi_med2_asf->level_str_adjust.low;
            level.low_delta = p_hi_med2_asf->level_str_adjust.low_delta;
            level.high = p_hi_med2_asf->level_str_adjust.high;
            level.high_delta = p_hi_med2_asf->level_str_adjust.high_delta;
            rval |= check_advanced_level_method_control("hi_med2_advanced_spatial_filter->level_str_adjust", &level);

            level.low = p_hi_med2_asf->t0_t1_div.low;
            level.low_delta = p_hi_med2_asf->t0_t1_div.low_delta;
            level.high = p_hi_med2_asf->t0_t1_div.high;
            level.high_delta = p_hi_med2_asf->t0_t1_div.high_delta;
            rval |= check_advanced_level_method_control("hi_med2_advanced_spatial_filter->t0_t1_div", &level);
        }
    }
    return rval;
}

static uint32 ctx_check_hi_high_asf_valid_range(const ik_hi_high_asf_t *p_hi_high_asf)
{
    uint32 rval = IK_OK;
    if (p_hi_high_asf == NULL) {
        amba_ik_system_print_str_5("[IK] p_hi_high_asf input is NULL address!", NULL, NULL, NULL, NULL, NULL);
        rval |= IK_ERR_0005;
    } else {
        {
            // advanced level control check
            ik_level_method_t level;

            level.low = p_hi_high_asf->level_str_adjust.low;
            level.low_delta = p_hi_high_asf->level_str_adjust.low_delta;
            level.high = p_hi_high_asf->level_str_adjust.high;
            level.high_delta = p_hi_high_asf->level_str_adjust.high_delta;
            rval |= check_advanced_level_method_control("hi_high_advanced_spatial_filter->level_str_adjust", &level);

            level.low = p_hi_high_asf->t0_t1_div.low;
            level.low_delta = p_hi_high_asf->t0_t1_div.low_delta;
            level.high = p_hi_high_asf->t0_t1_div.high;
            level.high_delta = p_hi_high_asf->t0_t1_div.high_delta;
            rval |= check_advanced_level_method_control("hi_high_advanced_spatial_filter->t0_t1_div", &level);
        }
    }
    return rval;
}

static uint32 ctx_check_chroma_asf_valid_range(const ik_chroma_asf_t *p_chroma_asf)
{
    uint32 rval = IK_OK;
    if (p_chroma_asf == NULL) {
        amba_ik_system_print_str_5("[IK] p_chroma_asf input is NULL address!", NULL, NULL, NULL, NULL, NULL);
        rval |= IK_ERR_0005;
    } else {
        {
            // advanced level control check
            ik_level_t level;

            level.low = p_chroma_asf->level_str_adjust.low;
            level.low_delta = p_chroma_asf->level_str_adjust.low_delta;
            level.high = p_chroma_asf->level_str_adjust.high;
            level.high_delta = p_chroma_asf->level_str_adjust.high_delta;
            rval |= check_advanced_level_control("li_chroma_advanced_spatial_filter->level_str_adjust", &level);

            level.low = p_chroma_asf->t0_t1_div.low;
            level.low_delta = p_chroma_asf->t0_t1_div.low_delta;
            level.high = p_chroma_asf->t0_t1_div.high;
            level.high_delta = p_chroma_asf->t0_t1_div.high_delta;
            rval |= check_advanced_level_control("li_chroma_advanced_spatial_filter->t0_t1_div", &level);
        }
    }
    return rval;
}

static uint32 ctx_check_hi_chroma_asf_valid_range(const ik_hi_chroma_asf_t *p_hi_chroma_asf)
{
    uint32 rval = IK_OK;
    if (p_hi_chroma_asf == NULL) {
        amba_ik_system_print_str_5("[IK] p_hi_chroma_asf input is NULL address!", NULL, NULL, NULL, NULL, NULL);
        rval |= IK_ERR_0005;
    } else {
        {
            // advanced level control check
            ik_level_t level;

            level.low = p_hi_chroma_asf->level_str_adjust.low;
            level.low_delta = p_hi_chroma_asf->level_str_adjust.low_delta;
            level.high = p_hi_chroma_asf->level_str_adjust.high;
            level.high_delta = p_hi_chroma_asf->level_str_adjust.high_delta;
            rval |= check_advanced_level_control("hi_chroma_advanced_spatial_filter->level_str_adjust", &level);

            level.low = p_hi_chroma_asf->t0_t1_div.low;
            level.low_delta = p_hi_chroma_asf->t0_t1_div.low_delta;
            level.high = p_hi_chroma_asf->t0_t1_div.high;
            level.high_delta = p_hi_chroma_asf->t0_t1_div.high_delta;
            rval |= check_advanced_level_control("hi_chroma_advanced_spatial_filter->t0_t1_div", &level);
        }
    }
    return rval;
}

//SHP
static uint32 ctx_check_hi_high_shpns_noise_valid_range(const ik_hi_high_shpns_noise_t *p_hi_high_shpns_noise)
{
    uint32 rval = IK_OK;
    if (p_hi_high_shpns_noise == NULL) {
        amba_ik_system_print_str_5("[IK] detect p_hi_high_shpns_noise get NULL pointer!", NULL, NULL, NULL, NULL, NULL);
        rval |= IK_ERR_0005;
    } else {
        {
            // advanced level control check
            ik_level_method_t level;

            level.low = p_hi_high_shpns_noise->level_str_adjust.low;
            level.low_delta = p_hi_high_shpns_noise->level_str_adjust.low_delta;
            level.high = p_hi_high_shpns_noise->level_str_adjust.high;
            level.high_delta = p_hi_high_shpns_noise->level_str_adjust.high_delta;
            rval |= check_advanced_level_method_control("hi_high_sharpen_noise_filter_noise->level_str_adjust", &level);
        }
    }
    return rval;
}

static uint32 ctx_check_hi_med_shpns_noise_valid_range(const ik_hi_med_shpns_noise_t *p_hi_med_shpns_noise)
{
    uint32 rval = IK_OK;
    if (p_hi_med_shpns_noise == NULL) {
        amba_ik_system_print_str_5("[IK] detect p_hi_med_shpns_noise get NULL pointer!", NULL, NULL, NULL, NULL, NULL);
        rval |= IK_ERR_0005;
    } else {
        {
            // advanced level control check
            ik_level_method_t level;

            level.low = p_hi_med_shpns_noise->level_str_adjust.low;
            level.low_delta = p_hi_med_shpns_noise->level_str_adjust.low_delta;
            level.high = p_hi_med_shpns_noise->level_str_adjust.high;
            level.high_delta = p_hi_med_shpns_noise->level_str_adjust.high_delta;
            rval |= check_advanced_level_method_control("hi_med_sharpen_noise_filter_noise->level_str_adjust", &level);
        }
    }
    return rval;
}

static uint32 ctx_check_li2_shpns_noise_valid_range(const ik_li2_shpns_noise_t *p_li2_shpns_noise)
{
    uint32 rval = IK_OK;
    if (p_li2_shpns_noise == NULL) {
        amba_ik_system_print_str_5("[IK] detect p_li2_shpns_noise get NULL pointer!", NULL, NULL, NULL, NULL, NULL);
        rval |= IK_ERR_0005;
    } else {
        {
            // advanced level control check
            ik_level_method_t level;

            level.low = p_li2_shpns_noise->level_str_adjust.low;
            level.low_delta = p_li2_shpns_noise->level_str_adjust.low_delta;
            level.high = p_li2_shpns_noise->level_str_adjust.high;
            level.high_delta = p_li2_shpns_noise->level_str_adjust.high_delta;
            rval |= check_advanced_level_method_control("li2_sharpen_noise_filter_noise->level_str_adjust", &level);
        }
    }
    return rval;
}

static uint32 ctx_check_hili_shpns_noise_valid_range(const ik_hili_shpns_noise_t *p_hili_shpns_noise)
{
    uint32 rval = IK_OK;
    if (p_hili_shpns_noise == NULL) {
        amba_ik_system_print_str_5("[IK] detect p_hili_shpns_noise get NULL pointer!", NULL, NULL, NULL, NULL, NULL);
        rval |= IK_ERR_0005;
    } else {
        {
            // advanced level control check
            ik_level_method_t level;

            level.low = p_hili_shpns_noise->level_str_adjust.low;
            level.low_delta = p_hili_shpns_noise->level_str_adjust.low_delta;
            level.high = p_hili_shpns_noise->level_str_adjust.high;
            level.high_delta = p_hili_shpns_noise->level_str_adjust.high_delta;
            rval |= check_advanced_level_method_control("hili_sharpen_noise_filter_noise->level_str_adjust", &level);
        }
    }
    return rval;
}

static uint32 ctx_check_hi_high_shpns_coring_valid_range(const ik_hi_high_shpns_coring_t *p_hi_high_shpns_coring)
{
    uint32 rval = IK_OK;
    if (p_hi_high_shpns_coring == NULL) {
        amba_ik_system_print_str_5("[IK] detect p_hi_high_shpns_coring get NULL pointer!", NULL, NULL, NULL, NULL, NULL);
        rval |= IK_ERR_0005;
    } else {

    }
    return rval;
}

static uint32 ctx_check_hi_high_shpns_cor_idx_scl_valid_range(const ik_level_method_t *p_level_method)
{
    uint32 rval = IK_OK;
    if (p_level_method == NULL) {
        amba_ik_system_print_str_5("[IK] detect hi_high_sharpen_noise_filter_sharpen_coring_index_scale get NULL pointer!", NULL, NULL, NULL, NULL, NULL);
        rval |= IK_ERR_0005;
    } else {
        {
            rval |= check_advanced_level_method_control("hi_high_sharpen_noise_filter_sharpen_coring_index_scale", p_level_method);
        }
    }
    return rval;
}

static uint32 ctx_check_hi_high_shpns_min_cor_rst_valid_range(const ik_level_method_t *p_level_method)
{
    uint32 rval = IK_OK;
    if (p_level_method == NULL) {
        amba_ik_system_print_str_5("[IK] detect hi_high_sharpen_noise_filter_sharpen_min_coring_result get NULL pointer!", NULL, NULL, NULL, NULL, NULL);
        rval |= IK_ERR_0005;
    } else {
        {
            rval |= check_advanced_level_method_control("hi_high_sharpen_noise_filter_sharpen_min_coring_result", p_level_method);
        }
    }
    return rval;
}

static uint32 ctx_check_hi_high_shpns_max_cor_rst_valid_range(const ik_level_method_t *p_level_method)
{
    uint32 rval = IK_OK;
    if (p_level_method == NULL) {
        amba_ik_system_print_str_5("[IK] detect hi_high_sharpen_noise_filter_sharpen_max_coring_result get NULL pointer!", NULL, NULL, NULL, NULL, NULL);
        rval |= IK_ERR_0005;
    } else {
        {
            rval |= check_advanced_level_method_control("hi_high_sharpen_noise_filter_sharpen_max_coring_result", p_level_method);
        }
    }
    return rval;
}

static uint32 ctx_check_hi_high_shpns_scl_cor_valid_range(const ik_level_method_t *p_level_method)
{
    uint32 rval = IK_OK;
    if (p_level_method == NULL) {
        amba_ik_system_print_str_5("[IK] detect hi_high_sharpen_noise_filter_sharpen_scale_coring get NULL pointer!", NULL, NULL, NULL, NULL, NULL);
        rval |= IK_ERR_0005;
    } else {
        {
            rval |= check_advanced_level_method_control("hi_high_sharpen_noise_filter_sharpen_scale_coring", p_level_method);
        }
    }
    return rval;
}

static uint32 ctx_check_hi_med_shpns_cor_idx_scl_valid_range(const ik_level_method_t *p_level_method)
{
    uint32 rval = IK_OK;
    if (p_level_method == NULL) {
        amba_ik_system_print_str_5("[IK] detect hi_med_sharpen_noise_filter_sharpen_coring_index_scale get NULL pointer!", NULL, NULL, NULL, NULL, NULL);
        rval |= IK_ERR_0005;
    } else {
        {
            rval |= check_advanced_level_method_control("hi_med_sharpen_noise_filter_sharpen_coring_index_scale", p_level_method);
        }
    }
    return rval;
}

static uint32 ctx_check_hi_med_shpns_min_cor_rst_valid_range(const ik_level_method_t *p_level_method)
{
    uint32 rval = IK_OK;
    if (p_level_method == NULL) {
        amba_ik_system_print_str_5("[IK] detect hi_med_sharpen_noise_filter_sharpen_min_coring_result get NULL pointer!", NULL, NULL, NULL, NULL, NULL);
        rval |= IK_ERR_0005;
    } else {
        {
            rval |= check_advanced_level_method_control("hi_med_sharpen_noise_filter_sharpen_min_coring_result", p_level_method);
        }
    }
    return rval;
}

static uint32 ctx_check_hi_med_shpns_max_cor_rst_valid_range(const ik_level_method_t *p_level_method)
{
    uint32 rval = IK_OK;
    if (p_level_method == NULL) {
        amba_ik_system_print_str_5("[IK] detect hi_med_sharpen_noise_filter_sharpen_max_coring_result get NULL pointer!", NULL, NULL, NULL, NULL, NULL);
        rval |= IK_ERR_0005;
    } else {
        {
            rval |= check_advanced_level_method_control("hi_med_sharpen_noise_filter_sharpen_max_coring_result", p_level_method);
        }
    }
    return rval;
}

static uint32 ctx_check_hi_med_shpns_scl_cor_valid_range(const ik_level_method_t *p_level_method)
{
    uint32 rval = IK_OK;
    if (p_level_method == NULL) {
        amba_ik_system_print_str_5("[IK] detect hi_med_sharpen_noise_filter_sharpen_coring get NULL pointer!", NULL, NULL, NULL, NULL, NULL);
        rval |= IK_ERR_0005;
    } else {
        {
            rval |= check_advanced_level_method_control("hi_med_sharpen_noise_filter_sharpen_coring", p_level_method);
        }
    }
    return rval;
}


static uint32 ctx_check_li2_shpns_cor_idx_scl_valid_range(const ik_level_method_t *p_level_method)
{
    uint32 rval = IK_OK;
    if (p_level_method == NULL) {
        amba_ik_system_print_str_5("[IK] detect li2_sharpen_noise_filter_sharpen_coring_index_scale get NULL pointer!", NULL, NULL, NULL, NULL, NULL);
        rval |= IK_ERR_0005;
    } else {
        {
            rval |= check_advanced_level_method_control("li2_sharpen_noise_filter_sharpen_coring_index_scale", p_level_method);
        }
    }
    return rval;
}

static uint32 ctx_check_li2_shpns_min_cor_rst_valid_range(const ik_level_method_t *p_level_method)
{
    uint32 rval = IK_OK;
    if (p_level_method == NULL) {
        amba_ik_system_print_str_5("[IK] detect li2_sharpen_noise_filter_sharpen_min_coring_result get NULL pointer!", NULL, NULL, NULL, NULL, NULL);
        rval |= IK_ERR_0005;
    } else {
        {
            rval |= check_advanced_level_method_control("li2_sharpen_noise_filter_sharpen_min_coring_result", p_level_method);
        }
    }
    return rval;
}

static uint32 ctx_check_li2_shpns_max_cor_rst_valid_range(const ik_level_method_t *p_level_method)
{
    uint32 rval = IK_OK;
    if (p_level_method == NULL) {
        amba_ik_system_print_str_5("[IK] detect li2_sharpen_noise_filter_sharpen_max_coring_result get NULL pointer!", NULL, NULL, NULL, NULL, NULL);
        rval |= IK_ERR_0005;
    } else {
        {
            rval |= check_advanced_level_method_control("li2_sharpen_noise_filter_sharpen_max_coring_result", p_level_method);
        }
    }
    return rval;
}

static uint32 ctx_check_li2_shpns_scl_cor_valid_range(const ik_level_method_t *p_level_method)
{
    uint32 rval = IK_OK;
    if (p_level_method == NULL) {
        amba_ik_system_print_str_5("[IK] detect li2_sharpen_noise_filter_sharpen_scale_coring get NULL pointer!", NULL, NULL, NULL, NULL, NULL);
        rval |= IK_ERR_0005;
    } else {
        {
            rval |= check_advanced_level_method_control("li2_sharpen_noise_filter_sharpen_scale_coring", p_level_method);
        }
    }
    return rval;
}


static uint32 ctx_check_hili_shpns_cor_idx_scl_valid_range(const ik_level_method_t *p_level_method)
{
    uint32 rval = IK_OK;
    if (p_level_method == NULL) {
        amba_ik_system_print_str_5("[IK] detect hili_sharpen_noise_filter_sharpen_coring_index_scale get NULL pointer!", NULL, NULL, NULL, NULL, NULL);
        rval |= IK_ERR_0005;
    } else {
        {
            rval |= check_advanced_level_method_control("hili_sharpen_noise_filter_sharpen_coring_index_scale", p_level_method);
        }
    }
    return rval;
}

static uint32 ctx_check_hili_shpns_min_cor_rst_valid_range(const ik_level_method_t *p_level_method)
{
    uint32 rval = IK_OK;
    if (p_level_method == NULL) {
        amba_ik_system_print_str_5("[IK] detect hili_sharpen_noise_filter_sharpen_min_coring_result get NULL pointer!", NULL, NULL, NULL, NULL, NULL);
        rval |= IK_ERR_0005;
    } else {
        {
            rval |= check_advanced_level_method_control("hili_sharpen_noise_filter_sharpen_min_coring_result", p_level_method);
        }
    }
    return rval;
}

static uint32 ctx_check_hili_shpns_max_cor_rst_valid_range(const ik_level_method_t *p_level_method)
{
    uint32 rval = IK_OK;
    if (p_level_method == NULL) {
        amba_ik_system_print_str_5("[IK] detect hili_sharpen_noise_filter_sharpen_max_coring_result get NULL pointer!", NULL, NULL, NULL, NULL, NULL);
        rval |= IK_ERR_0005;
    } else {
        {
            rval |= check_advanced_level_method_control("hili_sharpen_noise_filter_sharpen_max_coring_result", p_level_method);
        }
    }
    return rval;
}

static uint32 ctx_check_hili_shpns_scl_cor_valid_range(const ik_level_method_t *p_level_method)
{
    uint32 rval = IK_OK;
    if (p_level_method == NULL) {
        amba_ik_system_print_str_5("[IK] detect hili_sharpen_noise_filter_sharpen_scale_coring get NULL pointer!", NULL, NULL, NULL, NULL, NULL);
        rval |= IK_ERR_0005;
    } else {
        {
            rval |= check_advanced_level_method_control("hili_sharpen_noise_filter_sharpen_scale_coring", p_level_method);
        }
    }
    return rval;
}

//li2
static uint32 ctx_check_hi_shpns_both_b_valid_range(const ik_hi_shpns_both_b_t *p_hi_shpns_both_b)
{
    uint32 rval = IK_OK;
    if (p_hi_shpns_both_b == NULL) {
        amba_ik_system_print_str_5("[IK] detect p_hi_shpns_both_b get NULL pointer!", NULL, NULL, NULL, NULL, NULL);
        rval |= IK_ERR_0005;
    } else {

    }
    return rval;
}

static uint32 ctx_check_hi_shpns_coring_b_valid_range(const ik_hi_shpns_coring_b_t *p_hi_shpns_coring_b)
{
    uint32 rval = IK_OK;
    if (p_hi_shpns_coring_b == NULL) {
        amba_ik_system_print_str_5("[IK] detect p_hi_shpns_coring_b get NULL pointer!", NULL, NULL, NULL, NULL, NULL);
        rval |= IK_ERR_0005;
    } else {

    }
    return rval;
}

static uint32 ctx_check_hi_chroma_filter_high_valid_range(const ik_chroma_filter_t *chroma_filter)
{
    uint32 rval = IK_OK;
    if (chroma_filter == NULL) {
        rval |= IK_ERR_0005;
    } else {

    }

    return rval;
}

static uint32 ctx_check_hi_chroma_filter_b_valid_range(const ik_hi_chroma_filter_b_t *p_hi_chroma_filter_b)
{
    uint32 rval = IK_OK;
    if (p_hi_chroma_filter_b == NULL) {
        rval |= IK_ERR_0005;
    } else {

    }

    return rval;
}

//combine
static uint32 ctx_check_hi_luma_combine_valid_range(const ik_hi_luma_combine_t *p_hi_luma_combine)
{
    uint32 rval = IK_OK;
    if (p_hi_luma_combine == NULL) {
        rval |= IK_ERR_0005;
    } else {

    }

    return rval;
}

static uint32 ctx_check_hi_chroma_filter_med_combine_valid_range(const ik_hi_chroma_filter_combine_t *p_hi_chroma_filter_combine)
{
    uint32 rval = IK_OK;
    if (p_hi_chroma_filter_combine == NULL) {
        amba_ik_system_print_str_5("[IK] detect final_sharpen_coring_idx_scale get NULL pointer!", NULL, NULL, NULL, NULL, NULL);
        rval |= IK_ERR_0005;
    } else {
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

static uint32 ctx_check_hi_chroma_filter_low_combine_valid_range(const ik_hi_chroma_filter_combine_t *p_hi_chroma_filter_combine)
{
    uint32 rval = IK_OK;
    if (p_hi_chroma_filter_combine == NULL) {
        amba_ik_system_print_str_5("[IK] detect final_sharpen_coring_idx_scale get NULL pointer!", NULL, NULL, NULL, NULL, NULL);
        rval |= IK_ERR_0005;
    } else {
        {
            // advanced level control check
            ik_level_t level;

            level.low = p_hi_chroma_filter_combine->max_change_cb.low;
            level.low_delta = p_hi_chroma_filter_combine->max_change_cb.low_delta;
            level.high = p_hi_chroma_filter_combine->max_change_cb.high;
            level.high_delta = p_hi_chroma_filter_combine->max_change_cb.high_delta;
            rval |= check_advanced_level_control("hi_chroma_filter_low_combine->max_change_cb", &level);

            level.low = p_hi_chroma_filter_combine->max_change_cr.low;
            level.low_delta = p_hi_chroma_filter_combine->max_change_cr.low_delta;
            level.high = p_hi_chroma_filter_combine->max_change_cr.high;
            level.high_delta = p_hi_chroma_filter_combine->max_change_cr.high_delta;
            rval |= check_advanced_level_control("hi_chroma_filter_low_combine->max_change_cr", &level);

            level.low = p_hi_chroma_filter_combine->to_t1_add_cb.low;
            level.low_delta = p_hi_chroma_filter_combine->to_t1_add_cb.low_delta;
            level.high = p_hi_chroma_filter_combine->to_t1_add_cb.high;
            level.high_delta = p_hi_chroma_filter_combine->to_t1_add_cb.high_delta;
            rval |= check_advanced_level_control("hi_chroma_filter_low_combine->to_t1_add_cb", &level);

            level.low = p_hi_chroma_filter_combine->to_t1_add_cr.low;
            level.low_delta = p_hi_chroma_filter_combine->to_t1_add_cr.low_delta;
            level.high = p_hi_chroma_filter_combine->to_t1_add_cr.high;
            level.high_delta = p_hi_chroma_filter_combine->to_t1_add_cr.high_delta;
            rval |= check_advanced_level_control("hi_chroma_filter_low_combine->to_t1_add_cr", &level);
        }
    }
    return rval;
}

static uint32 ctx_check_hi_chroma_filter_very_low_combine_valid_range(const ik_hi_chroma_filter_combine_t *p_hi_chroma_filter_combine)
{
    uint32 rval = IK_OK;
    if (p_hi_chroma_filter_combine == NULL) {
        amba_ik_system_print_str_5("[IK] detect final_sharpen_coring_idx_scale get NULL pointer!", NULL, NULL, NULL, NULL, NULL);
        rval |= IK_ERR_0005;
    } else {
        {
            // advanced level control check
            ik_level_t level;

            level.low = p_hi_chroma_filter_combine->max_change_cb.low;
            level.low_delta = p_hi_chroma_filter_combine->max_change_cb.low_delta;
            level.high = p_hi_chroma_filter_combine->max_change_cb.high;
            level.high_delta = p_hi_chroma_filter_combine->max_change_cb.high_delta;
            rval |= check_advanced_level_control("hi_chroma_fltr_very_low_com->max_change_cb", &level);

            level.low = p_hi_chroma_filter_combine->max_change_cr.low;
            level.low_delta = p_hi_chroma_filter_combine->max_change_cr.low_delta;
            level.high = p_hi_chroma_filter_combine->max_change_cr.high;
            level.high_delta = p_hi_chroma_filter_combine->max_change_cr.high_delta;
            rval |= check_advanced_level_control("hi_chroma_fltr_very_low_com->max_change_cr", &level);

            level.low = p_hi_chroma_filter_combine->to_t1_add_cb.low;
            level.low_delta = p_hi_chroma_filter_combine->to_t1_add_cb.low_delta;
            level.high = p_hi_chroma_filter_combine->to_t1_add_cb.high;
            level.high_delta = p_hi_chroma_filter_combine->to_t1_add_cb.high_delta;
            rval |= check_advanced_level_control("hi_chroma_fltr_very_low_com->to_t1_add_cb", &level);

            level.low = p_hi_chroma_filter_combine->to_t1_add_cr.low;
            level.low_delta = p_hi_chroma_filter_combine->to_t1_add_cr.low_delta;
            level.high = p_hi_chroma_filter_combine->to_t1_add_cr.high;
            level.high_delta = p_hi_chroma_filter_combine->to_t1_add_cr.high_delta;
            rval |= check_advanced_level_control("hi_chroma_fltr_very_low_com->to_t1_add_cr", &level);
        }
    }
    return rval;
}


static uint32 ctx_check_hili_combine_valid_range(const ik_hili_combine_t *p_hili_combine)
{
    uint32 rval = IK_OK;
    if (p_hili_combine == NULL) {
        amba_ik_system_print_str_5("[IK] detect p_hili_combine get NULL pointer!", NULL, NULL, NULL, NULL, NULL);
        rval |= IK_ERR_0005;
    } else {
        {
            // advanced level control check
            ik_level_method_t level_method;
            ik_level_t level;

            level_method.low = p_hili_combine->max_change_y.low;
            level_method.low_delta = p_hili_combine->max_change_y.low_delta;
            level_method.high = p_hili_combine->max_change_y.high;
            level_method.high_delta = p_hili_combine->max_change_y.high_delta;
            rval |= check_advanced_level_method_control("hili_combine->max_change_y", &level_method);

            level_method.low = p_hili_combine->to_t1_add_y.low;
            level_method.low_delta = p_hili_combine->to_t1_add_y.low_delta;
            level_method.high = p_hili_combine->to_t1_add_y.high;
            level_method.high_delta = p_hili_combine->to_t1_add_y.high_delta;
            rval |= check_advanced_level_method_control("hili_combine->to_t1_add_y", &level_method);

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

static uint32 ctx_check_hi_mid_high_freq_recover_valid_range(const ik_hi_mid_high_freq_recover_t *p_hi_mid_high_freq_recover)
{
    uint32 rval = IK_OK;
    if (p_hi_mid_high_freq_recover == NULL) {
        amba_ik_system_print_str_5("[IK] detect p_hi_mid_high_freq_recover get NULL pointer!", NULL, NULL, NULL, NULL, NULL);
        rval |= IK_ERR_0005;
    } else {

    }
    return rval;
}

static uint32 ctx_check_hi_luma_blend_valid_range(const ik_hi_luma_blend_t *p_hili2_blend_luma)
{
    uint32 rval = IK_OK;
    if (p_hili2_blend_luma == NULL) {
        amba_ik_system_print_str_5("[IK] detect p_hili2_blend_luma get NULL pointer!", NULL, NULL, NULL, NULL, NULL);
        rval |= IK_ERR_0005;
    } else {
        {
            // advanced level control check
            ik_level_method_t level;

            level.low = p_hili2_blend_luma->luma_level.low;
            level.low_delta = p_hili2_blend_luma->luma_level.low_delta;
            level.high = p_hili2_blend_luma->luma_level.high;
            level.high_delta = p_hili2_blend_luma->luma_level.high_delta;
            rval |= check_advanced_level_method_control("hili2_blend_luma->level_str_adjust", &level);
        }

    }
    return rval;
}

static uint32 ctx_check_hi_nonsmooth_detect_valid_range(const ik_hi_nonsmooth_detect_t *p_hi_nonsmooth_detect)
{
    uint32 rval = IK_OK;
    if (p_hi_nonsmooth_detect == NULL) {
        amba_ik_system_print_str_5("[IK] detect p_hi_nonsmooth_detect get NULL pointer!", NULL, NULL, NULL, NULL, NULL);
        rval |= IK_ERR_0005;
    } else {

    }
    return rval;
}

static uint32 ctx_check_mono_dynamic_bad_pixel_corr_valid_range(const ik_mono_dynamic_bad_pixel_corr_t *p_mono_dynamic_bad_pixel_corr)
{
    uint32 rval = IK_OK;
    if (p_mono_dynamic_bad_pixel_corr == NULL) {
        rval |= IK_ERR_0005;
    } else {
        rval |= (uint32)ctx_check_uint32_parameter_valid_range("mono_dynamic_bad_pixel_corr->enable", p_mono_dynamic_bad_pixel_corr->enable, 0U, 4U);
        if(p_mono_dynamic_bad_pixel_corr->enable == 1U) {
            rval |= (uint32)ctx_check_uint32_parameter_valid_range("mono_dynamic_bad_pixel_corr->hot_pixel_strength", p_mono_dynamic_bad_pixel_corr->hot_pixel_strength, 0U, 10U);
            rval |= (uint32)ctx_check_uint32_parameter_valid_range("mono_dynamic_bad_pixel_corr->dark_pixel_strength", p_mono_dynamic_bad_pixel_corr->dark_pixel_strength, 0U, 10U);
            rval |= (uint32)ctx_check_uint32_parameter_valid_range("mono_dynamic_bad_pixel_corr->correction_method", p_mono_dynamic_bad_pixel_corr->correction_method, 0U, 1U);
        }
    }
    return rval;
}

static uint32 ctx_check_mono_cfa_leakage_filter_valid_range(const ik_mono_cfa_leakage_filter_t *p_mono_cfa_leakage_filter)
{
    uint32 rval = IK_OK;
    if (p_mono_cfa_leakage_filter == NULL) {
        rval |= IK_ERR_0005;
    } else {
        rval |= (uint32)ctx_check_uint32_parameter_valid_range("mono_cfa_leakage_filter->enable", p_mono_cfa_leakage_filter->enable, 0, 1);
        rval |= (uint32)ctx_check_uint32_parameter_valid_range("mono_cfa_leakage_filter->saturation_level", p_mono_cfa_leakage_filter->saturation_level, 0, 16383);
    }
    return rval;
}

static uint32 ctx_check_mono_cfa_noise_filter_valid_range(const ik_mono_cfa_noise_filter_t *p_mono_cfa_noise_filter)
{
    uint32 rval = IK_OK;
    uint32 i;
    uint32 tmp;
    if (p_mono_cfa_noise_filter == NULL) {
        rval |= IK_ERR_0005;
    } else {
        rval |= (uint32)ctx_check_uint32_parameter_valid_range("p_mono_cfa_noise_filter->enable", p_mono_cfa_noise_filter->enable, 0, 1);
#if 1
        for (i = 0U ; i < 3U ; i++) {
            rval |= (uint32)ctx_check_uint32_parameter_valid_range("mono_cfa_noise_filter->extent_fine", p_mono_cfa_noise_filter->extent_fine[i], 0, 256);
            rval |= (uint32)ctx_check_uint32_parameter_valid_range("mono_cfa_noise_filter->max_noise_level", p_mono_cfa_noise_filter->max_noise_level[i], 0, 32767);
            rval |= (uint32)ctx_check_uint32_parameter_valid_range("mono_cfa_noise_filter->min_noise_level", p_mono_cfa_noise_filter->min_noise_level[i], 0, 32767);
            rval |= (uint32)ctx_check_uint32_parameter_valid_range("mono_cfa_noise_filter->original_blend_str", p_mono_cfa_noise_filter->original_blend_str[i], 0, 256);
            rval |= (uint32)ctx_check_uint32_parameter_valid_range("mono_cfa_noise_filter->extent_regular", p_mono_cfa_noise_filter->extent_regular[i], 0, 256);
            rval |= (uint32)ctx_check_uint32_parameter_valid_range("mono_cfa_noise_filter->strength_fine", p_mono_cfa_noise_filter->strength_fine[i], 0, 256);
        }

        rval |= (uint32)ctx_check_uint32_parameter_valid_range("mono_cfa_noise_filter->selectivity_regular", p_mono_cfa_noise_filter->selectivity_regular, 0, 256);
        rval |= (uint32)ctx_check_uint32_parameter_valid_range("mono_cfa_noise_filter->selectivity_fine", p_mono_cfa_noise_filter->selectivity_fine, 0, 256);

        tmp = p_mono_cfa_noise_filter->selectivity_regular % 50U;
        if (tmp > 0U) {
            /*p_mono_cfa_noise_filter->selectivity_regular -= (p_mono_cfa_noise_filter->selectivity_regular%50);*/
            rval |= IK_ERR_0008;
        }
        tmp = p_mono_cfa_noise_filter->selectivity_fine % 50U;
        if (tmp > 0U) {
            /*p_mono_cfa_noise_filter->selectivity_fine -= (p_mono_cfa_noise_filter->selectivity_fine%50);*/
            rval |= IK_ERR_0008;
        }
#endif
        rval |= (uint32)ctx_check_uint32_parameter_valid_range("mono_cfa_noise_filter->directional_enable", p_mono_cfa_noise_filter->directional_enable, 0, 1);
        if(p_mono_cfa_noise_filter->directional_enable == 1U) {
            rval |= (uint32)ctx_check_uint32_parameter_valid_range("mono_cfa_noise_filter->directional_horvert_edge_strength", p_mono_cfa_noise_filter->directional_horvert_edge_strength, 8, 255);
            rval |= (uint32)ctx_check_uint32_parameter_valid_range("mono_cfa_noise_filter->directional_horvert_edge_strength_bias", p_mono_cfa_noise_filter->directional_horvert_strength_bias, 0, 65535);
            rval |= (uint32)ctx_check_uint32_parameter_valid_range("mono_cfa_noise_filter->directional_horvert_original_blend_strength", p_mono_cfa_noise_filter->directional_horvert_original_blend_strength, 0, 256);
        }
    }
    return rval;
}

static uint32 ctx_check_mono_anti_aliasing_valid_range(const ik_mono_anti_aliasing_t *p_mono_anti_aliasing)
{
    uint32 rval = IK_OK;
    if (p_mono_anti_aliasing == NULL) {
        rval |= IK_ERR_0005;
    } else {
        rval |= (uint32)ctx_check_uint32_parameter_valid_range("mono_anti_aliasing->enable", p_mono_anti_aliasing->enable, 0, 4);
        if (p_mono_anti_aliasing->enable == 4U) {
            rval |= (uint32)ctx_check_uint32_parameter_valid_range("mono_anti_aliasing->log_fractional_correct", p_mono_anti_aliasing->log_fractional_correct, 0, 7);
        }
    }
    return rval;
}

static uint32 ctx_check_mono_before_ce_wb_gain_valid_range(const ik_mono_before_ce_wb_gain_t *p_mono_before_ce_wb_gain)
{
    uint32 rval = IK_OK;
    if (p_mono_before_ce_wb_gain == NULL) {
        rval |= IK_ERR_0005;
    } else {
        rval |= (uint32)ctx_check_uint32_parameter_valid_range("mono_before_ce_wb_gain->gain_b", p_mono_before_ce_wb_gain->gain_b, 1U, 0xFFFFFF);
        rval |= (uint32)ctx_check_uint32_parameter_valid_range("mono_before_ce_wb_gain->gain_g", p_mono_before_ce_wb_gain->gain_g, 1U, 0xFFFFFF);
        rval |= (uint32)ctx_check_uint32_parameter_valid_range("mono_before_ce_wb_gain->gain_r", p_mono_before_ce_wb_gain->gain_r, 1U, 0xFFFFFF);
    }
    return rval;
}

static uint32 ctx_check_mono_after_ce_wb_gain_valid_range(const ik_mono_after_ce_wb_gain_t *p_mono_after_ce_wb_gain)
{
    uint32 rval = IK_OK;
    if (p_mono_after_ce_wb_gain == NULL) {
        rval |= IK_ERR_0005;
    } else {
        rval |= (uint32)ctx_check_uint32_parameter_valid_range("mono_after_ce_wb_gain->gain_b", p_mono_after_ce_wb_gain->gain_b, 1U, 0xFFFFFF);
        rval |= (uint32)ctx_check_uint32_parameter_valid_range("mono_after_ce_wb_gain->gain_g", p_mono_after_ce_wb_gain->gain_g, 1U, 0xFFFFFF);
        rval |= (uint32)ctx_check_uint32_parameter_valid_range("mono_after_ce_wb_gain->gain_r", p_mono_after_ce_wb_gain->gain_r, 1U, 0xFFFFFF);
    }
    return rval;
}

static uint32 ctx_check_mono_color_correction_valid_range(const ik_mono_color_correction_t *p_mono_color_correction)
{
    uint32 rval = IK_OK;
    uint32 i;

    if (p_mono_color_correction == NULL) {
        rval = IK_ERR_0005;
    } else {
        rval |= ctx_check_uint32_parameter_valid_range("mono_color_correction->matrix_en", p_mono_color_correction->matrix_en, 0, 1);
        rval |= ctx_check_int32_parameter_valid_range("mono_color_correction->matrix[0]", (int32)p_mono_color_correction->matrix[0], 0, 4095);
        rval |= ctx_check_int32_parameter_valid_range("mono_color_correction->matrix[1]", (int32)p_mono_color_correction->matrix[1], -2048, 2047);
        rval |= ctx_check_int32_parameter_valid_range("mono_color_correction->matrix[2]", (int32)p_mono_color_correction->matrix[2], -2048, 2047);
        rval |= ctx_check_int32_parameter_valid_range("mono_color_correction->matrix[3]", (int32)p_mono_color_correction->matrix[3], 0, 4095);
        rval |= ctx_check_int32_parameter_valid_range("mono_color_correction->matrix[4]", (int32)p_mono_color_correction->matrix[4], -2048, 2047);
        rval |= ctx_check_int32_parameter_valid_range("mono_color_correction->matrix[5]", (int32)p_mono_color_correction->matrix[5], 0, 4095);
        rval |= ctx_check_uint32_parameter_valid_range("mono_color_correction->matrix_shift_minus_8", p_mono_color_correction->matrix_shift_minus_8, 0, 2);
        for (i = 0U; i < 4096U; i++) {
            rval |= ctx_check_uint32_parameter_valid_range("mono_color_correction->matrix_three_d_table[]", p_mono_color_correction->matrix_three_d_table[i], 0, 1073741824U/*(1U<<30U)*/);
        }

    }

    return rval;
}

static uint32 ctx_check_mono_tone_curve_valid_range(const ik_mono_tone_curve_t *p_mono_tone_curve)
{
    uint32 rval = IK_OK, i;
    if (p_mono_tone_curve == NULL) {
        rval |= IK_ERR_0005;
    } else {
        for (i = 0U; i < 256U; i++) {
            rval |= (uint32)ctx_check_uint32_parameter_valid_range("mono_tone_curve->p_mono_tone_curve_red", p_mono_tone_curve->tone_curve_red[i], 0, 1023);
            rval |= (uint32)ctx_check_uint32_parameter_valid_range("mono_tone_curve->p_mono_tone_curve_green", p_mono_tone_curve->tone_curve_green[i], 0, 1023);
            rval |= (uint32)ctx_check_uint32_parameter_valid_range("mono_tone_curve->p_mono_tone_curve_blue", p_mono_tone_curve->tone_curve_blue[i], 0, 1023);
        }
    }
    return rval;
}

static uint32 ctx_check_mono_rgb_to_yuv_matrix_valid_range(const ik_mono_rgb_to_yuv_matrix_t *p_mono_rgb_to_yuv_matrix)
{
    uint32 rval = IK_OK, i;
    if (p_mono_rgb_to_yuv_matrix == NULL) {
        amba_ik_system_print_str_5("[IK] p_mono_rgb_to_yuv_matrix input address is NULL", NULL, NULL, NULL, NULL, NULL);
        rval |= IK_ERR_0005;
    } else {
        for (i = 0U; i < 3U; i++) {
            rval |= (uint32)ctx_check_int32_parameter_valid_range("mono_rgb_to_yuv_matrix->matrix_values[]", p_mono_rgb_to_yuv_matrix->matrix_values[i], -4096, 4095);
        }

        rval |= (uint32)ctx_check_int32_parameter_valid_range("mono_rgb_to_yuv_matrix->y_offset", p_mono_rgb_to_yuv_matrix->y_offset, -1024, 1023);
    }
    return rval;
}

static uint32 ctx_check_mono_demosaic_valid_range(const ik_mono_demosaic_t *p_mono_demosaic)
{
    uint32 rval = IK_OK;
    if (p_mono_demosaic == NULL) {
        rval |= IK_ERR_0005;
    } else {
        rval |= (uint32)ctx_check_uint32_parameter_valid_range("mono_demosaic->enable", p_mono_demosaic->enable, 1, 1);
        rval |= (uint32)ctx_check_uint32_parameter_valid_range("mono_demosaic->activity_thresh", p_mono_demosaic->activity_thresh, 0, 31);
        rval |= (uint32)ctx_check_uint32_parameter_valid_range("mono_demosaic->activity_difference_thresh", p_mono_demosaic->activity_difference_thresh, 0, 16383);
        rval |= (uint32)ctx_check_uint32_parameter_valid_range("mono_demosaic->grad_clip_thresh", p_mono_demosaic->grad_clip_thresh, 0, 4095);
        rval |= (uint32)ctx_check_uint32_parameter_valid_range("mono_demosaic->grad_noise_thresh", p_mono_demosaic->grad_noise_thresh, 0, 32767);
        rval |= (uint32)ctx_check_uint32_parameter_valid_range("mono_demosaic->clamp_directional_candidates", p_mono_demosaic->alias_interpolation_strength, 0, 38);
    }
    return rval;
}

static uint32 ctx_check_mono_grgb_mismatch_valid_range(const ik_mono_grgb_mismatch_t *p_mono_grgb_mismatch)
{
    uint32 rval = IK_OK;
    if (p_mono_grgb_mismatch == NULL) {
        rval |= IK_ERR_0005;
    } else {
        rval |= (uint32)ctx_check_uint32_parameter_valid_range("mono_grgb_mismatch->narrow_enable", p_mono_grgb_mismatch->narrow_enable, 0, 1);
        rval |= (uint32)ctx_check_uint32_parameter_valid_range("mono_grgb_mismatch->wide_enable", p_mono_grgb_mismatch->wide_enable, 0, 1);
        rval |= (uint32)ctx_check_uint32_parameter_valid_range("mono_grgb_mismatch->wide_safety", p_mono_grgb_mismatch->wide_safety, 0, 256);
        rval |= (uint32)ctx_check_uint32_parameter_valid_range("mono_grgb_mismatch->wide_thresh", p_mono_grgb_mismatch->wide_thresh, 0, 256);
    }
    return rval;
}

static uint32 ctx_check_mono_fst_luma_process_mode_valid_range(const ik_mono_fst_luma_process_mode_t *p_mono_fst_luma_process_mode)
{
    uint32 rval = IK_OK;
    if (p_mono_fst_luma_process_mode == NULL) {
        amba_ik_system_print_str_5("[IK] p_mono_fst_luma_process_mode input is NULL address!", NULL, NULL, NULL, NULL, NULL);
        rval |= IK_ERR_0005;
    } else {
        rval |= (uint32)ctx_check_uint32_parameter_valid_range("mono_fst_luma_process_mode->use_sharpen_not_asf", p_mono_fst_luma_process_mode->use_sharpen_not_asf, 0, 1);
    }
    return rval;
}

static uint32 ctx_check_mono_adv_spatial_filter_valid_range(const ik_mono_adv_spatial_filter_t *p_mono_adv_spatial_filter)
{
    uint32 rval = IK_OK, i, j;
    const ik_asf_fir_t *fir;
    if (p_mono_adv_spatial_filter == NULL) {
        amba_ik_system_print_str_5("[IK] p_mono_adv_spatial_filter input is NULL address!", NULL, NULL, NULL, NULL, NULL);
        rval |= IK_ERR_0005;
    } else {
        if (p_mono_adv_spatial_filter->enable!=0U) {
            rval |= (uint32)ctx_check_uint32_parameter_valid_range("mono_adv_spatial_filter->enable", p_mono_adv_spatial_filter->enable, 0, 1);
            fir = &p_mono_adv_spatial_filter->fir;
            rval |= (uint32)ctx_check_uint32_parameter_valid_range("mono_adv_spatial_filter->fir->specify", fir->specify, 2, 4);
            rval |= (uint32)ctx_check_uint32_parameter_valid_range("mono_adv_spatial_filter->fir->strength_iso", fir->strength_iso, 0, 256);
            rval |= (uint32)ctx_check_uint32_parameter_valid_range("mono_adv_spatial_filter->fir->strength_dir", fir->strength_dir, 0, 256);
            for (i = 0U; i < 9U; i++) {
                rval |= (uint32)ctx_check_uint32_parameter_valid_range("mono_adv_spatial_filter->fir->per_dir_fir_iso_strengths[]", fir->per_dir_fir_iso_strengths[i], 0, 256);
                rval |= (uint32)ctx_check_uint32_parameter_valid_range("mono_adv_spatial_filter->fir->per_dir_fir_dir_strengths[]", fir->per_dir_fir_dir_strengths[i], 0, 256);
                rval |= (uint32)ctx_check_uint32_parameter_valid_range("mono_adv_spatial_filter->fir->per_dir_fir_dir_amounts[]", fir->per_dir_fir_dir_amounts[i], 0, 256);
            }
            for (i = 0U; i < 9U; i++) {
                for (j = 0U; j < 25U; j++) {
                    rval |= (uint32)ctx_check_int32_parameter_valid_range("mono_adv_spatial_filter->fir->coefs[]", fir->coefs[i][j], -1024, 1023);
                }
            }
            if(p_mono_adv_spatial_filter->max_change_not_t0_t1_alpha != 0U) {
                rval |= (uint32)ctx_check_uint32_parameter_valid_range("mono_adv_spatial_filter->adapt->alpha_max_down", p_mono_adv_spatial_filter->adapt.alpha_max_down, 0, 8);
                rval |= (uint32)ctx_check_uint32_parameter_valid_range("mono_adv_spatial_filter->adapt->alpha_max_up", p_mono_adv_spatial_filter->adapt.alpha_max_up, 0, 8);
                rval |= (uint32)ctx_check_uint32_parameter_valid_range("mono_adv_spatial_filter->adapt->alpha_min_down", p_mono_adv_spatial_filter->adapt.alpha_min_down, 0, 8);
                rval |= (uint32)ctx_check_uint32_parameter_valid_range("mono_adv_spatial_filter->adapt->alpha_min_up", p_mono_adv_spatial_filter->adapt.alpha_min_up, 0, 8);

                rval |= (uint32)ctx_check_uint32_parameter_valid_range("mono_adv_spatial_filter->adapt->t0_down", p_mono_adv_spatial_filter->adapt.t0_down, 0, 252);
                if ((p_mono_adv_spatial_filter->adapt.t0_down % 2U) != 0U) {
                    amba_ik_system_print_uint32_5("[IK] p_mono_adv_spatial_filter->adapt.t0_down = %d should be even", p_mono_adv_spatial_filter->adapt.t0_down, DC_U, DC_U, DC_U, DC_U);
                    rval |= IK_ERR_0008;
                }

                rval |= (uint32)ctx_check_uint32_parameter_valid_range("mono_adv_spatial_filter->adapt->t0_up", p_mono_adv_spatial_filter->adapt.t0_up, 0U, 254U);
                if ((p_mono_adv_spatial_filter->adapt.t0_down % 2U) != 0U) {
                    amba_ik_system_print_uint32_5("[IK] p_mono_adv_spatial_filter->adapt.t0_up = %d should be even", p_mono_adv_spatial_filter->adapt.t0_up, DC_U, DC_U, DC_U, DC_U);
                    rval |= IK_ERR_0008;
                }

                rval |= (uint32)ctx_check_uint32_parameter_valid_range("mono_adv_spatial_filter->adapt->t1_down", p_mono_adv_spatial_filter->adapt.t1_down, 2U, 254U);
                if ((p_mono_adv_spatial_filter->adapt.t1_down % 2U) != 0U) {
                    amba_ik_system_print_uint32_5("[IK] p_mono_adv_spatial_filter->adapt.t1_down = %d should be even", p_mono_adv_spatial_filter->adapt.t1_down, DC_U, DC_U, DC_U, DC_U);
                    rval |= IK_ERR_0008;
                }
                rval |= (uint32)ctx_check_uint32_parameter_valid_range("mono_adv_spatial_filter->adapt->t1_up", p_mono_adv_spatial_filter->adapt.t1_up, 2U, 254U);
                if ((p_mono_adv_spatial_filter->adapt.t0_down % 2U) != 0U) {
                    amba_ik_system_print_uint32_5("[IK] p_mono_adv_spatial_filter->adapt.t1_up = %d should be even", p_mono_adv_spatial_filter->adapt.t1_up, DC_U, DC_U, DC_U, DC_U);
                    rval |= IK_ERR_0008;
                }

                if (p_mono_adv_spatial_filter->adapt.t0_up > p_mono_adv_spatial_filter->adapt.t1_up) {
                    amba_ik_system_print_uint32_5("[IK] p_mono_adv_spatial_filter->adapt->T1_up=%d should larger than T0_up= %d in ", p_mono_adv_spatial_filter->adapt.t1_up, p_mono_adv_spatial_filter->adapt.t0_up, DC_U, DC_U, DC_U);
                    rval |= IK_ERR_0008;
                }

                if (p_mono_adv_spatial_filter->adapt.t0_down > p_mono_adv_spatial_filter->adapt.t1_down) {
                    amba_ik_system_print_uint32_5("[IK] p_mono_adv_spatial_filter->adapt->t1_down=%d should larger than t0_down= %d in ", p_mono_adv_spatial_filter->adapt.t1_down, p_mono_adv_spatial_filter->adapt.t0_down, DC_U, DC_U, DC_U);
                    rval |= IK_ERR_0008;
                }
            }

            rval |= (uint32)ctx_check_uint32_parameter_valid_range("mono_adv_spatial_filter->level_str_adjust->high", p_mono_adv_spatial_filter->level_str_adjust.high, 1U, 254U);
            rval |= (uint32)ctx_check_uint32_parameter_valid_range("mono_adv_spatial_filter->level_str_adjust->low", p_mono_adv_spatial_filter->level_str_adjust.low, 0U, 253U);
            rval |= (uint32)ctx_check_uint32_parameter_valid_range("mono_adv_spatial_filter->level_str_adjust->low_delta", p_mono_adv_spatial_filter->level_str_adjust.low_delta, 0U, 7U);
            rval |= (uint32)ctx_check_uint32_parameter_valid_range("mono_adv_spatial_filter->level_str_adjust->low_strength", p_mono_adv_spatial_filter->level_str_adjust.low_strength, 0, 64);
            rval |= (uint32)ctx_check_uint32_parameter_valid_range("mono_adv_spatial_filter->level_str_adjust->high_delta", p_mono_adv_spatial_filter->level_str_adjust.high_delta, 0U, 7U);
            rval |= (uint32)ctx_check_uint32_parameter_valid_range("mono_adv_spatial_filter->level_str_adjust->high_strength", p_mono_adv_spatial_filter->level_str_adjust.high_strength, 0, 64);
            rval |= (uint32)ctx_check_uint32_parameter_valid_range("mono_adv_spatial_filter->level_str_adjust->mid_strength", p_mono_adv_spatial_filter->level_str_adjust.mid_strength, 0, 64);
            {
                // advanced level control check
                ik_level_t level;

                level.low = p_mono_adv_spatial_filter->level_str_adjust.low;
                level.low_delta = p_mono_adv_spatial_filter->level_str_adjust.low_delta;
                level.high = p_mono_adv_spatial_filter->level_str_adjust.high;
                level.high_delta = p_mono_adv_spatial_filter->level_str_adjust.high_delta;
                rval |= check_advanced_level_control("mono_adv_spatial_filter->level_str_adjust", &level);

            }
        }
    }
    return rval;
}

static uint32 ctx_check_mono_fst_shp_both_valid_range(const ik_mono_fst_shp_both_t *p_mono_fst_shp_both)
{
    uint32 rval = IK_OK;
    if (p_mono_fst_shp_both == NULL) {
        amba_ik_system_print_str_5("[IK] detect p_mono_fst_shp_both get NULL pointer!", NULL, NULL, NULL, NULL, NULL);
        rval |= IK_ERR_0005;
    } else {
        rval |= (uint32)ctx_check_uint32_parameter_valid_range("mono_fst_shp_both->enable", p_mono_fst_shp_both->enable, 0, 1);
        rval |= (uint32)ctx_check_uint32_parameter_valid_range("mono_fst_shp_both->mode", p_mono_fst_shp_both->mode, 0, 2);
        if((p_mono_fst_shp_both->mode != 0U)&&(p_mono_fst_shp_both->mode != 2U)) {
            amba_ik_system_print_uint32_5("[IKC] Error, parameter p_mono_fst_shp_both->mode=%d, should be 0 or 2", p_mono_fst_shp_both->mode, DC_U, DC_U, DC_U, DC_U);
            rval |= IK_ERR_0008;
        }

        rval |= (uint32)ctx_check_uint32_parameter_valid_range("mono_fst_shp_both->edge_thresh", p_mono_fst_shp_both->edge_thresh, 0, 2047);
        rval |= (uint32)ctx_check_uint32_parameter_valid_range("mono_fst_shp_both->wide_edge_detect", p_mono_fst_shp_both->wide_edge_detect, 0, 8);

        rval |= (uint32)ctx_check_uint32_parameter_valid_range("mono_fst_shp_both->max_change.up5x5", p_mono_fst_shp_both->max_change_up5x5, 0, 255);
        rval |= (uint32)ctx_check_uint32_parameter_valid_range("mono_fst_shp_both->max_change.down5x5", p_mono_fst_shp_both->max_change_down5x5, 0, 255);
    }
    return rval;

}

static uint32 ctx_check_mono_fst_shp_noise_valid_range(const ik_mono_fst_shp_noise_t *p_mono_fst_shp_noise)
{
    uint32 rval = IK_OK;
    uint32 i = 0, j = 0;
    int32 tmp_int32;
    if (p_mono_fst_shp_noise == NULL) {
        amba_ik_system_print_str_5("[IK] detect p_mono_fst_shp_noise get NULL pointer!", NULL, NULL, NULL, NULL, NULL);
        rval |= IK_ERR_0005;
    } else {
        rval |= ctx_check_uint32_parameter_valid_range("mono_fst_shp_noise->max_change_up", p_mono_fst_shp_noise->max_change_up, 0, 255);
        rval |= ctx_check_uint32_parameter_valid_range("mono_fst_shp_noise->max_change_down", p_mono_fst_shp_noise->max_change_down, 0, 255);
        rval |= ctx_check_uint32_parameter_valid_range("mono_fst_shp_noise->spatial_fir.specify", p_mono_fst_shp_noise->spatial_fir.specify, 0, 4);
        if (p_mono_fst_shp_noise->spatial_fir.specify == 0U) {
            rval |= ctx_check_uint32_parameter_valid_range("mono_fst_shp_noise->spatial_fir.strength_iso", p_mono_fst_shp_noise->spatial_fir.strength_iso, 0, 256);
        } else if (p_mono_fst_shp_noise->spatial_fir.specify == 1U) {
            for (i = 0U; i < 25U; i++) {
                rval |= ctx_check_int32_parameter_valid_range("mono_fst_shp_noise->spatial_fir.coefs", p_mono_fst_shp_noise->spatial_fir.coefs[0][i], -1024, 1023);
            }
        } else if (p_mono_fst_shp_noise->spatial_fir.specify == 2U) {
            rval |= ctx_check_uint32_parameter_valid_range("mono_fst_shp_noise->spatial_fir.strength_iso", p_mono_fst_shp_noise->spatial_fir.strength_iso, 0, 256);
            rval |= ctx_check_uint32_parameter_valid_range("mono_fst_shp_noise->spatial_fir.strength_dir", p_mono_fst_shp_noise->spatial_fir.strength_dir, 0, 256);
        } else if (p_mono_fst_shp_noise->spatial_fir.specify == 3U) {
            for (i = 0U; i < 9U; i++) {
                rval |= ctx_check_uint32_parameter_valid_range("mono_fst_shp_noise->spatial_fir.per_dir_fir_dir_amounts", p_mono_fst_shp_noise->spatial_fir.per_dir_fir_dir_amounts[i], 0, 256);
                rval |= ctx_check_uint32_parameter_valid_range("mono_fst_shp_noise->spatial_fir.per_dir_fir_dir_strengths", p_mono_fst_shp_noise->spatial_fir.per_dir_fir_dir_strengths[i], 0, 256);
                rval |= ctx_check_uint32_parameter_valid_range("mono_fst_shp_noise->spatial_fir.per_dir_fir_iso_strengths", p_mono_fst_shp_noise->spatial_fir.per_dir_fir_iso_strengths[i], 0, 256);
            }
        } else if (p_mono_fst_shp_noise->spatial_fir.specify == 4U) {
            for (j = 0U; j < 9U; j++) {
                for (i = 0U; i < 25U; i++) {
                    rval |= ctx_check_int32_parameter_valid_range("mono_fst_shp_noise->spatial_fir.coefs", p_mono_fst_shp_noise->spatial_fir.coefs[j][i], -1024, 1023);
                }
            }
        } else {
            rval |= IK_ERR_0008;
        }
        rval |= ctx_check_uint32_parameter_valid_range("mono_fst_shp_noise->level_str_adjust.low", p_mono_fst_shp_noise->level_str_adjust.low, 0U, 253U);
        rval |= ctx_check_uint32_parameter_valid_range("mono_fst_shp_noise->level_str_adjust.low_delta", p_mono_fst_shp_noise->level_str_adjust.low_delta, 0U, 7U);
        rval |= ctx_check_uint32_parameter_valid_range("mono_fst_shp_noise->level_str_adjust.high", p_mono_fst_shp_noise->level_str_adjust.high, 1U, 254U);
        rval |= ctx_check_uint32_parameter_valid_range("mono_fst_shp_noise->level_str_adjust.high_delta", p_mono_fst_shp_noise->level_str_adjust.high_delta, 0U, 7U);
        rval |= ctx_check_uint32_parameter_valid_range("mono_fst_shp_noise->level_str_adjust.low_strength", p_mono_fst_shp_noise->level_str_adjust.low_strength, 0U, 255U);
        rval |= ctx_check_uint32_parameter_valid_range("mono_fst_shp_noise->level_str_adjust.mid_strength", p_mono_fst_shp_noise->level_str_adjust.mid_strength, 0U, 255U);
        rval |= ctx_check_uint32_parameter_valid_range("mono_fst_shp_noise->level_str_adjust.high_strength", p_mono_fst_shp_noise->level_str_adjust.high_strength, 0U, 255U);
        if (p_mono_fst_shp_noise->t0 > p_mono_fst_shp_noise->t1) {
            amba_ik_system_print_uint32_5("mono_fst_shp_noise T0=%d  should not larger than T1=%d ", p_mono_fst_shp_noise->t0, p_mono_fst_shp_noise->t1, DC_U, DC_U, DC_U);
            rval |= IK_ERR_0008;
        }
        tmp_int32 = (int32)p_mono_fst_shp_noise->t1 - (int32)p_mono_fst_shp_noise->t0;
        if (tmp_int32 > 15) {
            amba_ik_system_print_uint32_5("T1=%d should not larger than T0=%d over 15", p_mono_fst_shp_noise->t1, p_mono_fst_shp_noise->t0, DC_U, DC_U, DC_U);
            rval |= IK_ERR_0008;
        }
        {
            // advanced level control check
            ik_level_t level;

            level.low = p_mono_fst_shp_noise->level_str_adjust.low;
            level.low_delta = p_mono_fst_shp_noise->level_str_adjust.low_delta;
            level.high = p_mono_fst_shp_noise->level_str_adjust.high;
            level.high_delta = p_mono_fst_shp_noise->level_str_adjust.high_delta;
            rval |= check_advanced_level_control("mono_fst_shp_noise->level_str_adjust", &level);

        }
    }
    return rval;
}

static uint32 ctx_check_mono_fst_shp_fir_valid_range(const ik_mono_fst_shp_fir_t *p_mono_fst_shp_fir)
{
    uint32 rval = IK_OK;
    uint32  i = 0, j = 0;
    if (p_mono_fst_shp_fir == NULL) {
        amba_ik_system_print_str_5("[IK] detect p_mono_fst_shp_fir get NULL pointer!", NULL, NULL, NULL, NULL, NULL);
        rval |= IK_ERR_0005;
    } else {
        rval |= ctx_check_uint32_parameter_valid_range("mono_fst_shp_fir->specify", p_mono_fst_shp_fir->specify, 0, 4);
        if (p_mono_fst_shp_fir->specify == 0U) {
            rval |= ctx_check_uint32_parameter_valid_range("mono_fst_shp_fir->strength_iso", p_mono_fst_shp_fir->strength_iso, 0, 256);
        } else if (p_mono_fst_shp_fir->specify == 1U) {
            for (j = 0U; j < 9U; j++) {
                for (i = 0U; i < 25U; i++) {
                    rval |= ctx_check_int32_parameter_valid_range("mono_fst_shp_fir->coefs", p_mono_fst_shp_fir->coefs[j][i], -1024, 1023);
                }
            }
        } else if (p_mono_fst_shp_fir->specify == 2U) {
            rval |= ctx_check_uint32_parameter_valid_range("mono_fst_shp_fir->strength_iso", p_mono_fst_shp_fir->strength_iso, 0, 256);
            rval |= ctx_check_uint32_parameter_valid_range("mono_fst_shp_fir->strength_dir", p_mono_fst_shp_fir->strength_dir, 0, 256);
        } else if (p_mono_fst_shp_fir->specify == 3U) {
            for (i = 0U; i < 9U; i++) {
                rval |= ctx_check_uint32_parameter_valid_range("mono_fst_shp_fir->per_dir_fir_dir_amounts", p_mono_fst_shp_fir->per_dir_fir_dir_amounts[i], 0, 256);
                rval |= ctx_check_uint32_parameter_valid_range("mono_fst_shp_fir->per_dir_fir_dir_strengths", p_mono_fst_shp_fir->per_dir_fir_dir_strengths[i], 0, 256);
                rval |= ctx_check_uint32_parameter_valid_range("mono_fst_shp_fir->per_dir_fir_iso_strengths", p_mono_fst_shp_fir->per_dir_fir_iso_strengths[i], 0, 256);
            }
        } else if (p_mono_fst_shp_fir->specify == 4U) {
            for (j = 0U; j < 9U; j++) {
                for (i = 0U; i < 25U; i++) {
                    rval |= ctx_check_int32_parameter_valid_range("mono_fst_shp_fir->coefs", p_mono_fst_shp_fir->coefs[j][i], -1024, 1023);
                }
            }
        } else {
            rval |= IK_ERR_0008;
        }
    }
    return rval;
}

static uint32 ctx_check_mono_fst_shp_coring_valid_range(const ik_mono_fst_shp_coring_t *p_mono_fst_shp_coring)
{
    uint32 rval = IK_OK;
    uint32 i = 0;
    if (p_mono_fst_shp_coring == NULL) {
        amba_ik_system_print_str_5("[IK] detect p_mono_fst_shp_coring get NULL pointer!", NULL, NULL, NULL, NULL, NULL);
        rval |= IK_ERR_0005;
    } else {
        rval |= ctx_check_uint32_parameter_valid_range("mono_fst_shp_coring->fractional_bits", p_mono_fst_shp_coring->fractional_bits, 1, 3);
        for (i = 0U; i < 256U; i++) {
            rval |= ctx_check_uint32_parameter_valid_range("mono_fst_shp_coring->coring", p_mono_fst_shp_coring->coring[i], 0, 31);
        }
    }
    return rval;
}

static uint32 ctx_check_mono_fst_shp_coring_idx_scale_valid_range(const ik_mono_fst_shp_coring_idx_scale_t *p_mono_fst_shp_coring_idx_scale)
{
    uint32 rval = IK_OK;
    if (p_mono_fst_shp_coring_idx_scale == NULL) {
        amba_ik_system_print_str_5("[IK] detect p_mono_fst_shp_coring_idx_scale get NULL pointer!", NULL, NULL, NULL, NULL, NULL);
        rval |= IK_ERR_0005;
    } else {
        rval |= ctx_check_uint32_parameter_valid_range("mono_fst_shp_coring_idx_scale->low", p_mono_fst_shp_coring_idx_scale->low, 0U, 253U);
        rval |= ctx_check_uint32_parameter_valid_range("mono_fst_shp_coring_idx_scale->low_delta", p_mono_fst_shp_coring_idx_scale->low_delta, 0U, 7U);
        rval |= ctx_check_uint32_parameter_valid_range("mono_fst_shp_coring_idx_scale->high_strength", p_mono_fst_shp_coring_idx_scale->low_strength, 0U, 255U);
        rval |= ctx_check_uint32_parameter_valid_range("mono_fst_shp_coring_idx_scale->high_strength", p_mono_fst_shp_coring_idx_scale->mid_strength, 0U, 255U);
        rval |= ctx_check_uint32_parameter_valid_range("mono_fst_shp_coring_idx_scale->high", p_mono_fst_shp_coring_idx_scale->high, 1, 254);
        rval |= ctx_check_uint32_parameter_valid_range("mono_fst_shp_coring_idx_scale->high_delta", p_mono_fst_shp_coring_idx_scale->high_delta, 0U, 7U);
        rval |= ctx_check_uint32_parameter_valid_range("mono_fst_shp_coring_idx_scale->high_strength", p_mono_fst_shp_coring_idx_scale->high_strength, 0U, 255U);
        {
            // advanced level control check
            ik_level_t level;

            level.low = p_mono_fst_shp_coring_idx_scale->low;
            level.low_delta = p_mono_fst_shp_coring_idx_scale->low_delta;
            level.high = p_mono_fst_shp_coring_idx_scale->high;
            level.high_delta = p_mono_fst_shp_coring_idx_scale->high_delta;
            rval |= check_advanced_level_control("mono_fst_shp_coring_idx_scale", &level);

        }
    }
    return rval;
}

static uint32 ctx_check_mono_fst_shp_min_coring_rslt_valid_range(const ik_mono_fst_shp_min_coring_rslt_t *p_mono_fst_shp_min_coring_rslt)
{
    uint32 rval = IK_OK;
    if (p_mono_fst_shp_min_coring_rslt == NULL) {
        amba_ik_system_print_str_5("[IK] detect p_mono_fst_shp_min_coring_rslt get NULL pointer!", NULL, NULL, NULL, NULL, NULL);
        rval |= IK_ERR_0005;
    } else {
        rval |= ctx_check_uint32_parameter_valid_range("mono_fst_shp_min_coring_rslt->low", p_mono_fst_shp_min_coring_rslt->low, 0U, 253U);
        rval |= ctx_check_uint32_parameter_valid_range("mono_fst_shp_min_coring_rslt->low_delta", p_mono_fst_shp_min_coring_rslt->low_delta, 0U, 7U);
        rval |= ctx_check_uint32_parameter_valid_range("mono_fst_shp_min_coring_rslt->low_strength", p_mono_fst_shp_min_coring_rslt->low_strength, 0U, 255U);
        rval |= ctx_check_uint32_parameter_valid_range("mono_fst_shp_min_coring_rslt->mid_strength", p_mono_fst_shp_min_coring_rslt->mid_strength, 0U, 255U);
        rval |= ctx_check_uint32_parameter_valid_range("mono_fst_shp_min_coring_rslt->high", p_mono_fst_shp_min_coring_rslt->high, 1, 254);
        rval |= ctx_check_uint32_parameter_valid_range("mono_fst_shp_min_coring_rslt->high_delta", p_mono_fst_shp_min_coring_rslt->high_delta, 0U, 7U);
        rval |= ctx_check_uint32_parameter_valid_range("mono_fst_shp_min_coring_rslt->high_strength", p_mono_fst_shp_min_coring_rslt->high_strength, 0U, 255U);
        {
            // advanced level control check
            ik_level_t level;

            level.low = p_mono_fst_shp_min_coring_rslt->low;
            level.low_delta = p_mono_fst_shp_min_coring_rslt->low_delta;
            level.high = p_mono_fst_shp_min_coring_rslt->high;
            level.high_delta = p_mono_fst_shp_min_coring_rslt->high_delta;
            rval |= check_advanced_level_control("mono_fst_shp_min_coring_rslt", &level);

        }
    }
    return rval;
}

static uint32 ctx_check_mono_fst_shp_max_coring_rslt_valid_range(const ik_mono_fst_shp_max_coring_rslt_t *p_mono_fst_shp_max_coring_rslt)
{
    uint32 rval = IK_OK;
    if (p_mono_fst_shp_max_coring_rslt == NULL) {
        amba_ik_system_print_str_5("[IK] detect p_mono_fst_shp_max_coring_rslt get NULL pointer!", NULL, NULL, NULL, NULL, NULL);
        rval |= IK_ERR_0005;
    } else {
        rval |= ctx_check_uint32_parameter_valid_range("mono_fst_shp_max_coring_rslt->low", p_mono_fst_shp_max_coring_rslt->low, 0U, 253U);
        rval |= ctx_check_uint32_parameter_valid_range("mono_fst_shp_max_coring_rslt->low_delta", p_mono_fst_shp_max_coring_rslt->low_delta, 0U, 7U);
        rval |= ctx_check_uint32_parameter_valid_range("mono_fst_shp_max_coring_rslt->low_strength", p_mono_fst_shp_max_coring_rslt->low_strength, 0U, 255U);
        rval |= ctx_check_uint32_parameter_valid_range("mono_fst_shp_max_coring_rslt->mid_strength", p_mono_fst_shp_max_coring_rslt->mid_strength, 0U, 255U);
        rval |= ctx_check_uint32_parameter_valid_range("mono_fst_shp_max_coring_rslt->high", p_mono_fst_shp_max_coring_rslt->high, 1, 254);
        rval |= ctx_check_uint32_parameter_valid_range("mono_fst_shp_max_coring_rslt->high_delta", p_mono_fst_shp_max_coring_rslt->high_delta, 0U, 7U);
        rval |= ctx_check_uint32_parameter_valid_range("mono_fst_shp_max_coring_rslt->high_strength", p_mono_fst_shp_max_coring_rslt->high_strength, 0U, 255U);
        {
            // advanced level control check
            ik_level_t level;

            level.low = p_mono_fst_shp_max_coring_rslt->low;
            level.low_delta = p_mono_fst_shp_max_coring_rslt->low_delta;
            level.high = p_mono_fst_shp_max_coring_rslt->high;
            level.high_delta = p_mono_fst_shp_max_coring_rslt->high_delta;
            rval |= check_advanced_level_control("mono_fst_shp_max_coring_rslt", &level);

        }
    }
    return rval;
}

static uint32 ctx_check_mono_fst_shp_scale_coring_valid_range(const ik_mono_fst_shp_scale_coring_t *p_mono_fst_shp_scale_coring)
{
    uint32 rval = IK_OK;
    if (p_mono_fst_shp_scale_coring == NULL) {
        amba_ik_system_print_str_5("[IK] detect p_mono_fst_shp_scale_coring get NULL pointer!", NULL, NULL, NULL, NULL, NULL);
        rval |= IK_ERR_0005;
    } else {
        rval |= ctx_check_uint32_parameter_valid_range("mono_fst_shp_scale_coring->low", p_mono_fst_shp_scale_coring->low, 0U, 253U);
        rval |= ctx_check_uint32_parameter_valid_range("mono_fst_shp_scale_coring->low_delta", p_mono_fst_shp_scale_coring->low_delta, 0U, 7U);
        rval |= ctx_check_uint32_parameter_valid_range("mono_fst_shp_scale_coring->low_strength", p_mono_fst_shp_scale_coring->low_strength, 0U, 255U);
        rval |= ctx_check_uint32_parameter_valid_range("mono_fst_shp_scale_coring->mid_strength", p_mono_fst_shp_scale_coring->mid_strength, 0U, 255U);
        rval |= ctx_check_uint32_parameter_valid_range("mono_fst_shp_scale_coring->high", p_mono_fst_shp_scale_coring->high, 1, 254);
        rval |= ctx_check_uint32_parameter_valid_range("mono_fst_shp_scale_coring->high_delta", p_mono_fst_shp_scale_coring->high_delta, 0U, 7U);
        rval |= ctx_check_uint32_parameter_valid_range("mono_fst_shp_scale_coring->high_strength", p_mono_fst_shp_scale_coring->high_strength, 0U, 255U);
        {
            // advanced level control check
            ik_level_t level;

            level.low = p_mono_fst_shp_scale_coring->low;
            level.low_delta = p_mono_fst_shp_scale_coring->low_delta;
            level.high = p_mono_fst_shp_scale_coring->high;
            level.high_delta = p_mono_fst_shp_scale_coring->high_delta;
            rval |= check_advanced_level_control("mono_fst_shp_scale_coring", &level);

        }
    }
    return rval;
}

static uint32 ctx_check_mono_exp0_fe_wb_gain_valid_range(const ik_mono_exp0_fe_wb_gain_t *p_mono_exp0_fe_wb_gain)
{
    uint32 rval = IK_OK;

    if (p_mono_exp0_fe_wb_gain == NULL) {
        amba_ik_system_print_str_5("[IK] detect p_mono_exp0_fe_wb_gain get NULL pointer!", NULL, NULL, NULL, NULL, NULL);
        rval = IK_ERR_0005;
    } else {
        rval |= ctx_check_uint32_parameter_valid_range("mono_exp0_fe_wb_gain->r_gain", p_mono_exp0_fe_wb_gain->r_gain, 0U, 0x400000U);
        rval |= ctx_check_uint32_parameter_valid_range("mono_exp0_fe_wb_gain->g_gain", p_mono_exp0_fe_wb_gain->g_gain, 0U, 0x400000U);
        rval |= ctx_check_uint32_parameter_valid_range("mono_exp0_fe_wb_gain->b_gain", p_mono_exp0_fe_wb_gain->b_gain, 0U, 0x400000U);
        rval |= ctx_check_uint32_parameter_valid_range("mono_exp0_fe_wb_gain->shutter_ratio", p_mono_exp0_fe_wb_gain->shutter_ratio, 1U, 16383U);

    }
    return rval;
}

static uint32 ctx_check_mono_exp0_fe_static_blc_level_valid_range(const ik_mono_exp0_fe_static_blc_level_t *p_mono_exp0_fe_static_blc_level)
{
    uint32 rval = IK_OK;

    if (p_mono_exp0_fe_static_blc_level==NULL) {
        amba_ik_system_print_str_5("[IK] detect p_mono_exp0_fe_static_blc_level get NULL pointer!", NULL, NULL, NULL, NULL, NULL);
        rval = IK_ERR_0005;
    } else {
        rval |= ctx_check_int32_parameter_valid_range("mono_exp0_fe_static_blc_level->black_r", p_mono_exp0_fe_static_blc_level->black_r, -16384, 16383);
        rval |= ctx_check_int32_parameter_valid_range("mono_exp0_fe_static_blc_level->black_gr", p_mono_exp0_fe_static_blc_level->black_gr, -16384, 16383);
        rval |= ctx_check_int32_parameter_valid_range("mono_exp0_fe_static_blc_level->black_gb", p_mono_exp0_fe_static_blc_level->black_gb, -16384, 16383);
        rval |= ctx_check_int32_parameter_valid_range("mono_exp0_fe_static_blc_level->black_b", p_mono_exp0_fe_static_blc_level->black_b, -16384, 16383);

    }
    return rval;
}

static uint32 ctx_check_mono_ce_valid_range(const ik_mono_ce_t *p_mono_ce)
{
    uint32 rval = IK_OK;
    int32 i = 0;
    if (p_mono_ce == NULL) {
        rval |= IK_ERR_0005;
    } else {
        rval |= ctx_check_uint32_parameter_valid_range("mono_ce->enable", p_mono_ce->enable, 0U, 1U);
        if (p_mono_ce->enable == 1u) {
            rval |= ctx_check_uint32_parameter_valid_range("mono_ce->radius", p_mono_ce->radius, 0U, 64U);
            rval |= ctx_check_uint32_parameter_valid_range("mono_ce->luma_avg_method", p_mono_ce->luma_avg_method, 0U, 1U);
            rval |= ctx_check_uint32_parameter_valid_range("mono_ce->fir_enable", p_mono_ce->fir_enable, 0U, 1U);
            for (i = 0; i < 3; i++) {
                rval |= ctx_check_uint32_parameter_valid_range("mono_ce->fir_coeff", p_mono_ce->fir_coeff[i], 0U, 64U);
            }
            rval |= ctx_check_uint32_parameter_valid_range("mono_ce->coring_index_scale_shift", p_mono_ce->coring_index_scale_shift, 0U, 24U);
            rval |= ctx_check_uint32_parameter_valid_range("mono_ce->coring_index_scale_low", p_mono_ce->coring_index_scale_low, 0U, 255U);
            rval |= ctx_check_uint32_parameter_valid_range("mono_ce->coring_index_scale_low_delta", p_mono_ce->coring_index_scale_low_delta, 0U, 7U);
            rval |= ctx_check_uint32_parameter_valid_range("mono_ce->coring_index_scale_low_strength", p_mono_ce->coring_index_scale_low_strength, 0U, 255U);
            rval |= ctx_check_uint32_parameter_valid_range("mono_ce->coring_index_scale_mid_strength", p_mono_ce->coring_index_scale_mid_strength, 0U, 255U);
            rval |= ctx_check_uint32_parameter_valid_range("mono_ce->coring_index_scale_high", p_mono_ce->coring_index_scale_high, 0U, 255U);
            rval |= ctx_check_uint32_parameter_valid_range("mono_ce->coring_index_scale_high_delta", p_mono_ce->coring_index_scale_high_delta, 0U, 7U);
            rval |= ctx_check_uint32_parameter_valid_range("mono_ce->coring_index_scale_high_strength", p_mono_ce->coring_index_scale_high_strength, 0U, 255U);
            rval |= ctx_check_uint32_parameter_valid_range("mono_ce->coring_gain_shift", p_mono_ce->coring_gain_shift, 0U, 20U);
            rval |= ctx_check_uint32_parameter_valid_range("mono_ce->coring_gain_low", p_mono_ce->coring_gain_low, 0U, 255U);
            rval |= ctx_check_uint32_parameter_valid_range("mono_ce->coring_gain_low_delta", p_mono_ce->coring_gain_low_delta, 0U, 7U);
            rval |= ctx_check_uint32_parameter_valid_range("mono_ce->coring_gain_low_strength", p_mono_ce->coring_gain_low_strength, 0U, 255U);
            rval |= ctx_check_uint32_parameter_valid_range("mono_ce->coring_gain_mid_strength", p_mono_ce->coring_gain_mid_strength, 0U, 255U);
            rval |= ctx_check_uint32_parameter_valid_range("mono_ce->coring_gain_high", p_mono_ce->coring_gain_high, 0U, 255U);
            rval |= ctx_check_uint32_parameter_valid_range("mono_ce->coring_gain_high_delta", p_mono_ce->coring_gain_high_delta, 0U, 255U);
            rval |= ctx_check_uint32_parameter_valid_range("mono_ce->coring_gain_high_strength", p_mono_ce->coring_gain_high_strength, 0U, 255U);

            rval |= ctx_check_uint32_parameter_valid_range("mono_ce->boost_gain_shift", p_mono_ce->boost_gain_shift, 0U, 7U);
            rval |= ctx_check_uint32_parameter_valid_range("mono_ce->boost_table_size_exp", p_mono_ce->boost_table_size_exp, 0U, 6U);
            for (i = 0; i < (int32)IK_CE_BOOST_TABLE_SIZE; i++) {
                rval |= ctx_check_int32_parameter_valid_range("mono_ce->boost_table", p_mono_ce->boost_table[i], -2048, 2047);
                if (rval != IK_OK) {
                    amba_ik_system_print_uint32_5("[IK]ERROR! boost_table[%d] = %d .boost_table value should be [-2048, 2047]", (uint32)i, (uint32)p_mono_ce->boost_table[i], DC_U, DC_U, DC_U);
                    break;
                }
            }
            rval |= ctx_check_uint32_parameter_valid_range("mono_ce->radius", p_mono_ce->radius, 0U, 3U);
            rval |= ctx_check_uint32_parameter_valid_range("mono_ce->epsilon", p_mono_ce->epsilon, 0U, 1000U);

            {
                // advanced level control check
                ik_level_t level;

                level.low = p_mono_ce->coring_index_scale_low;
                level.low_delta = p_mono_ce->coring_index_scale_low_delta;
                level.high = p_mono_ce->coring_index_scale_high;
                level.high_delta = p_mono_ce->coring_index_scale_high_delta;
                rval |= check_advanced_level_control("mono_ce->coring_index_scale", &level);

                level.low = p_mono_ce->coring_gain_low;
                level.low_delta = p_mono_ce->coring_gain_low_delta;
                level.high = p_mono_ce->coring_gain_high;
                level.high_delta = p_mono_ce->coring_gain_high_delta;
                rval |= check_advanced_level_control("mono_ce->coring_gain", &level);
            }
        } else {
            // misraC
        }
    }
    return rval;
}

static uint32 ctx_check_mono_ce_input_table_valid_range(const ik_mono_ce_input_table_t *p_mono_ce_input_table)
{
    uint32 rval = IK_OK;
    int32 i;
    if (p_mono_ce_input_table == NULL) {
        rval |= IK_ERR_0005;
    } else {
        rval |= ctx_check_uint32_parameter_valid_range("mono_ce_input_table->input_enable", p_mono_ce_input_table->input_enable, 0U, 1U);
        for (i = 0; i < (int32)IK_CE_INPUT_TABLE_SIZE; i++) {
            rval |= ctx_check_uint32_parameter_valid_range("mono_ce_input_table->input_table", p_mono_ce_input_table->input_table[i], 0U, 1048575U);
            if (rval != IK_OK) {
                amba_ik_system_print_uint32_5("[IK]ERROR! input_table[%d] = %d .input table value should be < (2^20-1)", (uint32)i, p_mono_ce_input_table->input_table[i], DC_U, DC_U, DC_U);
                break;
            }
        }
    }
    return rval;
}

static uint32 ctx_check_mono_ce_output_table_valid_range(const ik_mono_ce_output_table_t *p_mono_ce_output_table)
{
    uint32 rval = IK_OK;
    if (p_mono_ce_output_table == NULL) {
        rval |= IK_ERR_0005;
    } else {
        rval |= ctx_check_uint32_parameter_valid_range("mono_ce_output_table->output_enable", p_mono_ce_output_table->output_enable, 0U, 1U);
        rval |= ctx_check_uint32_parameter_valid_range("mono_ce_output_table->output_shift", p_mono_ce_output_table->output_shift, 0U, 12U);
    }
    return rval;
}

static uint32 ctx_check_fusion_valid_range(const ik_fusion_t *p_fusion)
{
    uint32 rval = IK_OK;
    if (p_fusion == NULL) {
        rval |= IK_ERR_0005;
    } else {
        rval |= ctx_check_uint32_parameter_valid_range("fusion->enable", p_fusion->enable, 0U, 1U);
    }
    return rval;
}


void ik_ctx_hook_debug_check(void)
{
    static ik_debug_check_func_t debug_check_func;

    debug_check_func.vin_sensor_info = ctx_check_vin_sensor_info;
    debug_check_func.fe_tone_curve = ctx_check_fe_tone_curve;
    debug_check_func.fe_blc = ctx_check_fe_static_blc;

    debug_check_func.fe_wb = ctx_check_fe_wb_gain;
    debug_check_func.fe_dgain_sat = ctx_check_dgain_sat_lvl;
    debug_check_func.hdr_blend = ctx_check_hdr_blend;
    debug_check_func.vignette_info = ctx_check_vignette_compensation;

    debug_check_func.dynamic_bad_pxl_cor = ctx_check_dynamic_bpc;
    debug_check_func.static_bpc = ctx_check_bpc_valid_range;
    debug_check_func.static_bpc_internal = ctx_check_static_bpc_internal_valid_range;
    debug_check_func.cfa_leakage = ctx_check_cfa_leakage_filter_valid_range;
    debug_check_func.anti_aliasing = ctx_check_anti_aliasing_valid_range;

    debug_check_func.grgb_mismatch = ctx_check_grgb_mismatch_valid_range;
    debug_check_func.before_ce_wb = ctx_check_wb_gain_valid_range;
    debug_check_func.warp_info = ctx_check_calib_warp_info_valid_range;
    debug_check_func.warp_info_2nd = ctx_check_2nd_calib_warp_info_valid_range;
    debug_check_func.warp_info_internal = ctx_check_warp_internal_valid_range;
    debug_check_func.warp_info_2nd_internal = ctx_check_warp_2nd_internal_valid_range;
    debug_check_func.warp_bind_info = ctx_check_warp_bind_info_valid_range;

    debug_check_func.cawarp = ctx_check_calib_ca_warp_info_valid_range;
    debug_check_func.cawarp_internal = ctx_check_ca_warp_internal_valid_range;
    debug_check_func.ce = ctx_check_ce_valid_range;
    debug_check_func.ce_input_tbl = ctx_check_ce_input_table_valid_range;
    debug_check_func.ce_output_tbl = ctx_check_ce_out_table_valid_range;
    debug_check_func.after_ce_wb = ctx_check_wb_gain_valid_range;
    debug_check_func.stored_ir   = ctx_check_stored_ir;

    debug_check_func.cfa_noise_filter = ctx_check_cfa_noise_filter_valid_range;
    debug_check_func.demosaic = ctx_check_demosaic_valid_range;
    debug_check_func.ext_raw_out_mode = ctx_check_ext_raw_out_mode_valid_range;
    debug_check_func.rgb_to_12y = ctx_check_rgb_to_12y_valid_range;
    debug_check_func.luma_noise_reduce = ctx_check_luma_noise_reduction_valid_range;
    debug_check_func.rgb_ir = ctx_check_rgb_ir_valid_range;
    debug_check_func.color_correction = ctx_check_color_correction_valid_range;
    debug_check_func.pre_cc_gain = ctx_check_pre_cc_gain_valid_range;

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
    debug_check_func.hdr_shift = ctx_check_hdr_shift_info_valid_range;
    debug_check_func.window_size_info = ctx_check_window_size_info_valid_range;
    debug_check_func.cfa_window_size_info = ctx_check_cfa_window_size_info_valid_range;
    debug_check_func.vin_active_win = ctx_check_vin_active_win_valid_range;
    debug_check_func.dummy_margin_range = ctx_check_set_dummy_win_margin_range_info_valid_range;
    debug_check_func.resamp_strength = ctx_check_resampler_str_valid_range;

    debug_check_func.motion_detect = ctx_check_motion_detect_valid_range;
    debug_check_func.motion_detect_and_mctf = ctx_check_motion_detect_and_mctf_valid_range;
    debug_check_func.motion_detect_pos_dep = ctx_check_pos_dep33_valid_range;

    debug_check_func.histogram_info = amba_ik_debug_check_histogram_info;
    debug_check_func.flip_mode = ctx_check_flip_mode_info_valid_range;
    debug_check_func.aaa_stat = ctx_check_aaa_stat_info_valid_range;
    debug_check_func.aaa_pg_af_stat = ctx_check_aaa_pg_af_info_valid_range;
    debug_check_func.af_stat_ex = ctx_check_af_stat_ex_info_valid_range;
    debug_check_func.pg_af_stat_ex = ctx_check_pg_af_stat_ex_info_valid_range;
    debug_check_func.wide_chroma_filter = ctx_check_wide_chroma_filter_valid_range;
    debug_check_func.wide_chroma_filter_combine = ctx_check_wide_chroma_filter_combine;
    debug_check_func.warp_buffer_info = ctx_check_warp_buffer_info;
    debug_check_func.overlap_x = ctx_check_overlap_x;
    debug_check_func.stitching_info = ctx_check_stitching_info;
    debug_check_func.burst_tile = ctx_check_burst_tile;

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
    debug_check_func.li2_asf = ctx_check_li2_asf_valid_range;
    debug_check_func.hi_low_asf = ctx_check_hi_low_asf_valid_range;
    debug_check_func.hi_med1_asf = ctx_check_hi_med1_asf_valid_range;
    debug_check_func.hi_med2_asf = ctx_check_hi_med2_asf_valid_range;
    debug_check_func.hi_high_asf = ctx_check_hi_high_asf_valid_range;
    debug_check_func.hi_high2_asf = ctx_check_hi_high2_asf_valid_range;
    debug_check_func.chroma_asf = ctx_check_chroma_asf_valid_range;
    debug_check_func.hi_chroma_asf = ctx_check_hi_chroma_asf_valid_range;
    debug_check_func.hi_low_chroma_asf = ctx_check_chroma_asf_valid_range;

    // sharpen
    // --high--
    debug_check_func.hi_high_shpns_both = ctx_check_fstshpns_both_valid_range;
    debug_check_func.hi_high_shpns_noise = ctx_check_hi_high_shpns_noise_valid_range;
    debug_check_func.hi_high_shpns_coring = ctx_check_hi_high_shpns_coring_valid_range;
    debug_check_func.hi_high_shpns_fir = ctx_check_fstshpns_fir_valid_range;
    debug_check_func.hi_high_shpns_cor_idx_scl = ctx_check_hi_high_shpns_cor_idx_scl_valid_range;
    debug_check_func.hi_high_shpns_min_cor_rst = ctx_check_hi_high_shpns_min_cor_rst_valid_range;
    debug_check_func.hi_high_shpns_max_cor_rst = ctx_check_hi_high_shpns_max_cor_rst_valid_range;
    debug_check_func.hi_high_shpns_scl_cor = ctx_check_hi_high_shpns_scl_cor_valid_range;

    // --med--
    debug_check_func.hi_med_shpns_both = ctx_check_fstshpns_both_valid_range;
    debug_check_func.hi_med_shpns_noise = ctx_check_hi_med_shpns_noise_valid_range;
    debug_check_func.hi_med_shpns_coring = ctx_check_fstshpns_coring_valid_range;
    debug_check_func.hi_med_shpns_fir = ctx_check_fstshpns_fir_valid_range;
    debug_check_func.hi_med_shpns_cor_idx_scl = ctx_check_hi_med_shpns_cor_idx_scl_valid_range;
    debug_check_func.hi_med_shpns_min_cor_rst = ctx_check_hi_med_shpns_min_cor_rst_valid_range;
    debug_check_func.hi_med_shpns_max_cor_rst = ctx_check_hi_med_shpns_max_cor_rst_valid_range;
    debug_check_func.hi_med_shpns_scl_cor = ctx_check_hi_med_shpns_scl_cor_valid_range;

    // --low2--
    debug_check_func.li2_shpns_both = ctx_check_hi_shpns_both_b_valid_range;
    debug_check_func.li2_shpns_noise = ctx_check_li2_shpns_noise_valid_range;
    debug_check_func.li2_shpns_coring = ctx_check_hi_shpns_coring_b_valid_range;
    debug_check_func.li2_shpns_fir = ctx_check_fstshpns_fir_valid_range;
    debug_check_func.li2_shpns_cor_idx_scl = ctx_check_li2_shpns_cor_idx_scl_valid_range;
    debug_check_func.li2_shpns_min_cor_rst = ctx_check_li2_shpns_min_cor_rst_valid_range;
    debug_check_func.li2_shpns_max_cor_rst = ctx_check_li2_shpns_max_cor_rst_valid_range;
    debug_check_func.li2_shpns_scl_cor = ctx_check_li2_shpns_scl_cor_valid_range;

    // --hili--
    debug_check_func.hili_shpns_both = ctx_check_hi_shpns_both_b_valid_range;
    debug_check_func.hili_shpns_noise = ctx_check_hili_shpns_noise_valid_range;
    debug_check_func.hili_shpns_coring = ctx_check_hi_shpns_coring_b_valid_range;
    debug_check_func.hili_shpns_fir = ctx_check_fstshpns_fir_valid_range;
    debug_check_func.hili_shpns_cor_idx_scl = ctx_check_hili_shpns_cor_idx_scl_valid_range;
    debug_check_func.hili_shpns_min_cor_rst = ctx_check_hili_shpns_min_cor_rst_valid_range;
    debug_check_func.hili_shpns_max_cor_rst = ctx_check_hili_shpns_max_cor_rst_valid_range;
    debug_check_func.hili_shpns_scl_cor     = ctx_check_hili_shpns_scl_cor_valid_range;

    //chroma filter
    debug_check_func.hi_chroma_filter_high = ctx_check_hi_chroma_filter_high_valid_range;
    debug_check_func.hi_chroma_filter_pre = ctx_check_hi_chroma_filter_b_valid_range;
    debug_check_func.hi_chroma_filter_med = ctx_check_hi_chroma_filter_b_valid_range;
    debug_check_func.hi_chroma_filter_low = ctx_check_hi_chroma_filter_b_valid_range;
    debug_check_func.hi_chroma_filter_very_low = ctx_check_hi_chroma_filter_b_valid_range;

    //combine
    debug_check_func.hi_luma_combine = ctx_check_hi_luma_combine_valid_range;
    debug_check_func.hi_low_asf_combine = ctx_check_hi_luma_combine_valid_range;
    debug_check_func.hi_chroma_fltr_med_com = ctx_check_hi_chroma_filter_med_combine_valid_range;
    debug_check_func.hi_chroma_fltr_low_com = ctx_check_hi_chroma_filter_low_combine_valid_range;
    debug_check_func.hi_chroma_fltr_very_low_com = ctx_check_hi_chroma_filter_very_low_combine_valid_range;
    debug_check_func.hili_combine = ctx_check_hili_combine_valid_range;
    debug_check_func.hi_mid_high_freq_recover = ctx_check_hi_mid_high_freq_recover_valid_range;
    debug_check_func.hi_luma_blend = ctx_check_hi_luma_blend_valid_range;
    debug_check_func.hi_nonsmooth_detect = ctx_check_hi_nonsmooth_detect_valid_range;
    debug_check_func.hi_select = ctx_check_first_luma_processing_mode_valid_range;

    debug_check_func.hi_wide_chroma_filter = ctx_check_wide_chroma_filter_valid_range;
    debug_check_func.hi_wide_chroma_filter_combine = ctx_check_wide_chroma_filter_combine;
    debug_check_func.li2_wide_chroma_filter = ctx_check_wide_chroma_filter_valid_range;
    debug_check_func.li2_wide_chroma_filter_combine = ctx_check_wide_chroma_filter_combine;

    //mono
    debug_check_func.mono_dynamic_bad_pixel_corr = ctx_check_mono_dynamic_bad_pixel_corr_valid_range;
    debug_check_func.mono_cfa_leakage_filter = ctx_check_mono_cfa_leakage_filter_valid_range;
    debug_check_func.mono_cfa_noise_filter = ctx_check_mono_cfa_noise_filter_valid_range;
    debug_check_func.mono_anti_aliasing = ctx_check_mono_anti_aliasing_valid_range;
    debug_check_func.mono_before_ce_wb_gain = ctx_check_mono_before_ce_wb_gain_valid_range;
    debug_check_func.mono_after_ce_wb_gain = ctx_check_mono_after_ce_wb_gain_valid_range;
    debug_check_func.mono_color_correction = ctx_check_mono_color_correction_valid_range;
    debug_check_func.mono_tone_curve = ctx_check_mono_tone_curve_valid_range;
    debug_check_func.mono_rgb_to_yuv_matrix = ctx_check_mono_rgb_to_yuv_matrix_valid_range;
    debug_check_func.mono_demosaic = ctx_check_mono_demosaic_valid_range;
    debug_check_func.mono_grgb_mismatch = ctx_check_mono_grgb_mismatch_valid_range;
    debug_check_func.mono_fst_luma_process_mode = ctx_check_mono_fst_luma_process_mode_valid_range;
    debug_check_func.mono_adv_spatial_filter = ctx_check_mono_adv_spatial_filter_valid_range;
    debug_check_func.mono_fst_shp_both = ctx_check_mono_fst_shp_both_valid_range;
    debug_check_func.mono_fst_shp_noise = ctx_check_mono_fst_shp_noise_valid_range;
    debug_check_func.mono_fst_shp_fir = ctx_check_mono_fst_shp_fir_valid_range;
    debug_check_func.mono_fst_shp_coring = ctx_check_mono_fst_shp_coring_valid_range;
    debug_check_func.mono_fst_shp_coring_idx_scale = ctx_check_mono_fst_shp_coring_idx_scale_valid_range;
    debug_check_func.mono_fst_shp_min_coring_rslt = ctx_check_mono_fst_shp_min_coring_rslt_valid_range;
    debug_check_func.mono_fst_shp_max_coring_rslt = ctx_check_mono_fst_shp_max_coring_rslt_valid_range;
    debug_check_func.mono_fst_shp_scale_coring = ctx_check_mono_fst_shp_scale_coring_valid_range;
    debug_check_func.mono_exp0_fe_wb_gain = ctx_check_mono_exp0_fe_wb_gain_valid_range;
    debug_check_func.mono_exp0_fe_static_blc_level = ctx_check_mono_exp0_fe_static_blc_level_valid_range;
    debug_check_func.mono_ce = ctx_check_mono_ce_valid_range;
    debug_check_func.mono_ce_input_table = ctx_check_mono_ce_input_table_valid_range;
    debug_check_func.mono_ce_output_table = ctx_check_mono_ce_output_table_valid_range;
    debug_check_func.fusion = ctx_check_fusion_valid_range;

    p_debug_check_function = &debug_check_func;
}

const ik_debug_check_func_t* ik_ctx_get_debug_check_func(void)
{
    return p_debug_check_function;
}
