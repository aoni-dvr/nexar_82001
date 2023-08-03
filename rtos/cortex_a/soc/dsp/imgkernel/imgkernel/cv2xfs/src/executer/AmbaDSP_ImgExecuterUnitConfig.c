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

#include "AmbaDSP_ImgConfigComponentIF.h"
#include "AmbaDSP_ImgContextComponentIF.h"
#include "AmbaDSP_ImgExecuterUtilities.h"
#include "AmbaDSP_ImgContextUtility.h"
#include "AmbaDSP_ImgProcess.h"
#include "AmbaDSP_ImgExecuterCalib.h"


//for misra casting.
static inline uint32 physical2u32(const void *p)
{
    uint32 out;
    (void)amba_ik_system_memcpy(&out, &p, sizeof(uint32));
    return out;
}

static inline void* ptr2ptr(void *p)
{
    return p;
}

static inline uint8 greater_op_u32(uint32 compare1, uint32 compare2)
{
    return (compare1 > compare2) ? 1U : 0U;
}

static inline uint8 less_equal_op_u32(uint32 compare1, uint32 compare2)
{
    return (compare1 <= compare2) ? 1U : 0U;
}

static inline uint8 equal_op_u32(uint32 compare1, uint32 compare2)
{
    return (compare1 == compare2) ? 1U : 0U;
}

static inline uint8 non_equal_op_u32(uint32 compare1, uint32 compare2)
{
    return (compare1 != compare2) ? 1U : 0U;
}

static inline uint8 lnl_op_u32(uint32 compare1, uint32 compare2)
{
    return ((equal_op_u32(compare1, 1U) + non_equal_op_u32(compare2, AMBA_IK_VIDEO_Y2Y))==2U) ? 1U : 0U;
}

static inline uint8 lnl_table_op_u32_1(uint32 compare1, uint32 compare2)
{
    return ((compare1 == 1U) || (compare2 == 1U)) ? 1U : 0U;
}

static inline uint8 lnl_table_op_u32_2(uint32 compare1, uint32 compare2)
{
    return (!((compare1 == 0U) && (compare2 == AMBA_IK_VIDEO_Y2Y))) ? 1U : 0U;
}

static inline uint8 null_check_op(const void *p)
{
    return (p == NULL) ? 1U : 0U;
}

static inline uint8 warp_flow_table_op_u32(uint32 compare1, uint32 compare2)
{
    return ((compare1 == 0U) && (compare2 == 1U)) ? 1u : 0u;
}

static uint32 exe_check_vin_sensor(const amba_ik_filter_t *p_filters)
{
    uint32 rval = IK_OK;

    const ik_vin_sensor_info_t *p_sensor_info = &p_filters->input_param.sensor_info;

    /*
     *    sensor mode check
     *    a) Normal sensor
     *    system.sensor_mode 0
     *    rgb_ir.mode 0
     *    system.raw_bayer 0 = RG, 1 = BG, 2 = GR, 3 = GB
     *
     *    b) RGBIR
     *    system.sensor_mode 1
     *    rgb_ir.mode 1:2
     *    system.raw_bayer 0 = RG, 1 = BG, 2 = GR, 3 = GB
     *
     *    c) RCCC/CCCR/CRCC/CCRC
     *    system.sensor_mode 2
     *    rgb_ir.mode 0
     *    system.raw_bayer 0 = RG, 1 = BG, 2 = GR, 3 = GB
     */

    if (p_sensor_info->sensor_mode == IK_SENSOR_MODE_NORMAL) {
        if (p_filters->input_param.rgb_ir.mode != 0UL) {
            amba_ik_system_print_uint32_5("[IK][ERROR] Incorrect sensor_mode %d or rgb_ir.mode %d !", \
                                          p_sensor_info->sensor_mode, p_filters->input_param.rgb_ir.mode, DC_U, DC_U, DC_U);
            rval |= IK_ERR_0008;
        }
    } else if (p_sensor_info->sensor_mode == IK_SENSOR_MODE_RGBIR) {
        if (p_filters->input_param.rgb_ir.mode == 0UL) {
            amba_ik_system_print_uint32_5("[IK][ERROR] Incorrect sensor_mode %d or rgb_ir.mode %d !", \
                                          p_sensor_info->sensor_mode, p_filters->input_param.rgb_ir.mode, DC_U, DC_U, DC_U);
            rval |= IK_ERR_0008;
        }
    } else {
        if (p_filters->input_param.rgb_ir.mode != 0UL) {
            amba_ik_system_print_uint32_5("[IK][ERROR] Incorrect sensor_mode %d or rgb_ir.mode %d !", \
                                          p_sensor_info->sensor_mode, p_filters->input_param.rgb_ir.mode, DC_U, DC_U, DC_U);
            rval |= IK_ERR_0008;
        }
    }

    return rval;
}

static inline uint32 process_calib_cr_non_y2y_part1(idsp_ik_flow_ctrl_t *p_flow, amba_ik_flow_tables_list_t *p_flow_tbl_list, amba_ik_filter_t *p_filters, uint32 window_calculate_updated)
{
    uint32 rval = IK_OK;
    uint8 bool_result;

    (void)amba_ik_system_memcpy(p_flow, p_flow, sizeof(uint8));//misra fix.
    (void)amba_ik_system_memcpy(p_flow_tbl_list, p_flow_tbl_list, sizeof(uint8));//misra fix.

    // sbp
    bool_result = equal_op_u32(p_filters->update_flags.iso.static_bpc_updated, 1u) + equal_op_u32(p_filters->update_flags.iso.static_bpc_internal_updated, 1u) +
                  equal_op_u32(p_filters->update_flags.iso.dynamic_bpc_updated, 1u) + equal_op_u32(p_filters->update_flags.iso.window_size_info_updated, 1u);
    if(bool_result != 0u) {
        if(p_filters->input_param.ctx_buf.sbp_internal_mode_flag == 1u) {
            ikc_in_static_bad_pixel_internal_t in_sbp_internal;
            ikc_out_static_bad_pixel_internal_t out_sbp;
            ik_static_bad_pixel_correction_internal_t sbp_internal_tmp;
            const void *void_ptr;

            if(p_filters->update_flags.cr.cr_12_update == 0U) {
                amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_static_bad_pixel_internal !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
                rval |= IK_ERR_0205;//Ring buffer Eorror
            } else {
                //consider 64 bit compiler, flow data sbp addr(u32) goes on physical addr + offset.
                (void)amba_ik_system_memcpy(&sbp_internal_tmp,&p_filters->input_param.sbp_internal,sizeof(ik_static_bad_pixel_correction_internal_t));
                void_ptr = amba_ik_system_virt_to_phys(sbp_internal_tmp.p_map);
                (void)amba_ik_system_memcpy(&sbp_internal_tmp.p_map, &void_ptr, sizeof(void*));
                in_sbp_internal.dbp_enable = p_filters->input_param.dynamic_bpc.enable;
                in_sbp_internal.p_sbp_internal = &sbp_internal_tmp;
                out_sbp.p_cr_12 = p_flow_tbl_list->p_CR_buf_12;
                out_sbp.cr_12_size = CR_SIZE_12;
                out_sbp.p_flow = &p_flow->flow_info;
                rval |= ikc_static_bad_pixel_internal(&in_sbp_internal, &out_sbp);
            }
        } else {
            ikc_in_static_bad_pixel_t in_sbp;
            ikc_out_static_bad_pixel_t out_sbp;
            ik_static_bad_pxl_cor_t static_bpc_tmp;
            const void *void_ptr;

            if(p_filters->update_flags.cr.cr_12_update == 0U) {
                amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_static_bad_pixel !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
                rval |= IK_ERR_0205;//Ring buffer Eorror
            } else {
                //consider 64 bit compiler, flow data sbp addr(u32) goes on physical addr + offset.
                (void)amba_ik_system_memcpy(&static_bpc_tmp,&p_filters->input_param.static_bpc,sizeof(ik_static_bad_pxl_cor_t));
                void_ptr = amba_ik_system_virt_to_phys(static_bpc_tmp.calib_sbp_info.sbp_buffer);
                (void)amba_ik_system_memcpy(&static_bpc_tmp.calib_sbp_info.sbp_buffer, &void_ptr, sizeof(void*));
                in_sbp.dbp_enable = p_filters->input_param.dynamic_bpc.enable;
                in_sbp.sbp_enable = p_filters->input_param.sbp_enable;
                in_sbp.p_static_bpc = &static_bpc_tmp;
                in_sbp.p_vin_sensor = &p_filters->input_param.window_size_info.vin_sensor;
                out_sbp.p_cr_12 = p_flow_tbl_list->p_CR_buf_12;
                out_sbp.cr_12_size = CR_SIZE_12;
                out_sbp.p_flow = &p_flow->flow_info;
                rval |= ikc_static_bad_pixel(&in_sbp, &out_sbp);
            }
        }
        p_flow->flow_info.update_sbp = 1u;
    } else {
        p_flow->flow_info.update_sbp = 0u;
    }

    // vig
    bool_result = equal_op_u32(p_filters->update_flags.iso.vignette_compensation_updated, 1U) + equal_op_u32(p_filters->update_flags.iso.window_size_info_updated, 1U) +
                  equal_op_u32(window_calculate_updated, 1UL);
    if (bool_result != 0u) {
        ikc_in_vignette_t in_vig;
        ikc_out_vignette_t out_vig;

        bool_result = equal_op_u32(p_filters->update_flags.cr.cr_9_update, 0U) + equal_op_u32(p_filters->update_flags.cr.cr_10_update, 0U) +
                      equal_op_u32(p_filters->update_flags.cr.cr_11_update, 0U);
        if(bool_result != 0u) {
            amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_vignette !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
            rval |= IK_ERR_0205;//Ring buffer Eorror
        } else {
            in_vig.vig_enable = p_filters->input_param.vig_enable;
            in_vig.flip_mode = p_filters->input_param.flip_mode;
            in_vig.p_vignette_compensation = &p_filters->input_param.vignette_compensation;
            in_vig.p_vin_sensor = &p_filters->input_param.window_size_info.vin_sensor;
            in_vig.p_dmy_win_geo = &p_filters->input_param.ctx_buf.result_win.dmy_win_geo;
            out_vig.p_cr_9 = p_flow_tbl_list->p_CR_buf_9;
            out_vig.cr_9_size = CR_SIZE_9;
            out_vig.p_cr_10 = p_flow_tbl_list->p_CR_buf_10;
            out_vig.cr_10_size = CR_SIZE_10;
            out_vig.p_cr_11 = p_flow_tbl_list->p_CR_buf_11;
            out_vig.cr_11_size = CR_SIZE_11;
            out_vig.p_flow = &p_flow->flow_info;
            rval |= ikc_vignette(&in_vig, &out_vig);
        }
        p_flow->flow_info.update_vignette = 1u;
    } else {
        p_flow->flow_info.update_vignette = 0u;
    }

    return rval;
}

static inline uint32 process_calib_cr_non_y2y_part2(idsp_ik_flow_ctrl_t *p_flow, amba_ik_flow_tables_list_t *p_flow_tbl_list, amba_ik_filter_t *p_filters, uint32 window_calculate_updated)
{
    uint32 rval = IK_OK;
    uint8 bool_result;

    (void)amba_ik_system_memcpy(p_flow, p_flow, sizeof(uint8));//misra fix.
    (void)amba_ik_system_memcpy(p_flow_tbl_list, p_flow_tbl_list, sizeof(uint8));//misra fix.

    // ca
    bool_result = equal_op_u32(p_filters->update_flags.iso.calib_ca_warp_info_updated, 1U) + equal_op_u32(p_filters->update_flags.iso.cawarp_enable_updated, 1U) +
                  equal_op_u32(p_filters->update_flags.iso.cawarp_internal_updated, 1U) + equal_op_u32(p_filters->update_flags.iso.window_size_info_updated, 1U) +
                  equal_op_u32(window_calculate_updated, 1UL);
    if (bool_result != 0u) {
        if (p_filters->input_param.ctx_buf.cawarp_internal_mode_flag == 1U) {
            ikc_in_cawarp_internal_t in_ca;
            ikc_out_cawarp_t out_ca;

            bool_result = equal_op_u32(p_filters->update_flags.cr.cawarp, 0U) + equal_op_u32(p_filters->update_flags.cr.cr_16_update, 0U) +
                          equal_op_u32(p_filters->update_flags.cr.cr_17_update, 0U);
            if(bool_result != 0u) {
                amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_cawarp_internal !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
                rval |= IK_ERR_0205;//Ring buffer Eorror
            } else {
                in_ca.p_stitching_info = &p_filters->input_param.stitching_info;
                in_ca.p_ca_warp_internal = &p_filters->input_param.ca_warp_internal;
                out_ca.p_cr_16 = p_flow_tbl_list->p_CR_buf_16;
                out_ca.cr_16_size = CR_SIZE_16;
                out_ca.p_cr_17 = p_flow_tbl_list->p_CR_buf_17;
                out_ca.cr_17_size = CR_SIZE_17;
                out_ca.p_cr_18 = p_flow_tbl_list->p_CR_buf_18;
                out_ca.cr_18_size = CR_SIZE_18;
                out_ca.p_cr_19 = p_flow_tbl_list->p_CR_buf_19;
                out_ca.cr_19_size = CR_SIZE_19;
                out_ca.p_cr_20 = p_flow_tbl_list->p_CR_buf_20;
                out_ca.cr_20_size = CR_SIZE_20;
                out_ca.p_calib = &p_flow->calib;
                out_ca.p_cawarp_hor_table_addr_red = p_flow_tbl_list->p_ca_warp_hor_red;
                out_ca.h_red_size = CAWARP_VIDEO_MAXSIZE;
                out_ca.p_cawarp_vertical_table_addr_red = p_flow_tbl_list->p_ca_warp_ver_red;
                out_ca.v_red_size = CAWARP_VIDEO_MAXSIZE;
                out_ca.p_cawarp_hor_table_addr_blue = p_flow_tbl_list->p_ca_warp_hor_blue;
                out_ca.h_blue_size = CAWARP_VIDEO_MAXSIZE;
                out_ca.p_cawarp_vertical_table_addr_blue = p_flow_tbl_list->p_ca_warp_ver_blue;
                out_ca.v_blue_size = CAWARP_VIDEO_MAXSIZE;
                rval |= ikc_cawarp_internal(&in_ca, &out_ca);
            }
        } else {
            ikc_in_cawarp_t in_ca;
            ikc_out_cawarp_t out_ca;

            bool_result = equal_op_u32(p_filters->update_flags.cr.cawarp, 0U) + equal_op_u32(p_filters->update_flags.cr.cr_16_update, 0U) +
                          equal_op_u32(p_filters->update_flags.cr.cr_17_update, 0U);
            if(bool_result != 0u) {
                amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_cawarp !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
                rval |= IK_ERR_0205;//Ring buffer Eorror
            } else {
                in_ca.is_group_cmd = 0u;
                in_ca.cawarp_enable = (uint32)p_filters->input_param.cawarp_enable;
                in_ca.flip_mode = p_filters->input_param.flip_mode;
                in_ca.p_stitching_info = &p_filters->input_param.stitching_info;
                in_ca.p_window_size_info = &p_filters->input_param.window_size_info;
                in_ca.p_result_win = &p_filters->input_param.ctx_buf.result_win;
                in_ca.p_calib_ca_warp_info = &p_filters->input_param.calib_ca_warp_info;
                out_ca.p_cr_16 = p_flow_tbl_list->p_CR_buf_16;
                out_ca.cr_16_size = CR_SIZE_16;
                out_ca.p_cr_17 = p_flow_tbl_list->p_CR_buf_17;
                out_ca.cr_17_size = CR_SIZE_17;
                out_ca.p_cr_18 = p_flow_tbl_list->p_CR_buf_18;
                out_ca.cr_18_size = CR_SIZE_18;
                out_ca.p_cr_19 = p_flow_tbl_list->p_CR_buf_19;
                out_ca.cr_19_size = CR_SIZE_19;
                out_ca.p_cr_20 = p_flow_tbl_list->p_CR_buf_20;
                out_ca.cr_20_size = CR_SIZE_20;
                out_ca.p_calib = &p_flow->calib;
                out_ca.p_cawarp_hor_table_addr_red = p_flow_tbl_list->p_ca_warp_hor_red;
                out_ca.h_red_size = CAWARP_VIDEO_MAXSIZE;
                out_ca.p_cawarp_vertical_table_addr_red = p_flow_tbl_list->p_ca_warp_ver_red;
                out_ca.v_red_size = CAWARP_VIDEO_MAXSIZE;
                out_ca.p_cawarp_hor_table_addr_blue = p_flow_tbl_list->p_ca_warp_hor_blue;
                out_ca.h_blue_size = CAWARP_VIDEO_MAXSIZE;
                out_ca.p_cawarp_vertical_table_addr_blue = p_flow_tbl_list->p_ca_warp_ver_blue;
                out_ca.v_blue_size = CAWARP_VIDEO_MAXSIZE;

                rval |= ikc_cawarp(&in_ca, &out_ca);
            }
        }
        p_flow->calib.update_CA_warp = 1u;
    } else {
        p_flow->calib.update_CA_warp = 0u;
    }

    return rval;
}
static inline uint32 process_calib_cr_warp_part1(idsp_ik_flow_ctrl_t *p_flow, amba_ik_flow_tables_list_t *p_flow_tbl_list, amba_ik_filter_t *p_filters, uint32 window_calculate_updated)
{
    uint32 rval = IK_OK;
    uint8 bool_result;

    (void)amba_ik_system_memcpy(p_flow, p_flow, sizeof(uint8));//misra fix.
    (void)amba_ik_system_memcpy(p_flow_tbl_list, p_flow_tbl_list, sizeof(uint8));//misra fix.
    (void)amba_ik_system_memcpy(p_filters, p_filters, sizeof(uint8));//misra fix.

    bool_result = equal_op_u32(p_filters->update_flags.iso.warp_enable_updated, 1U) +
                  equal_op_u32(p_filters->update_flags.iso.calib_warp_info_updated, 1U) +
                  equal_op_u32(p_filters->update_flags.iso.warp_internal_updated, 1U) +
                  equal_op_u32(p_filters->update_flags.iso.window_size_info_updated, 1U) +
                  equal_op_u32(p_filters->update_flags.iso.warp_buf_info_updated, 1U) +
                  equal_op_u32(window_calculate_updated, 1UL);
    if(bool_result != 0u) {
        if(p_filters->input_param.ctx_buf.warp_internal_mode_flag == 1U) { //internal warp
            ikc_in_warp_internal_t in_warp_internal;
            ikc_out_warp_t out_warp;

            bool_result = warp_flow_table_op_u32(p_filters->update_flags.cr.warp, p_filters->update_flags.iso.warp_internal_updated) +
                          equal_op_u32(p_filters->update_flags.cr.cr_33_update, 0U) +
                          equal_op_u32(p_filters->update_flags.cr.cr_34_update, 0U) +
                          equal_op_u32(p_filters->update_flags.cr.cr_35_update, 0U) +
                          equal_op_u32(p_filters->update_flags.cr.cr_42_update, 0U) +
                          equal_op_u32(p_filters->update_flags.cr.cr_43_update, 0U) +
                          equal_op_u32(p_filters->update_flags.cr.cr_44_update, 0U);
            if(bool_result != 0u) {
                amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_warp_internal !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
                rval |= IK_ERR_0205;//Ring buffer Eorror
            } else {
                in_warp_internal.flip_mode = p_filters->input_param.flip_mode;
                in_warp_internal.p_window_size_info = &p_filters->input_param.window_size_info;
                in_warp_internal.p_act_win_crop = &p_filters->input_param.ctx_buf.result_win.act_win_crop;
                in_warp_internal.p_dmy_win_geo = &p_filters->input_param.ctx_buf.result_win.dmy_win_geo;
                in_warp_internal.p_cfa_win_dim = &p_filters->input_param.ctx_buf.result_win.cfa_win_dim;
                in_warp_internal.p_dmy_range = &p_filters->input_param.dmy_range;
                in_warp_internal.p_stitching_info = &p_filters->input_param.stitching_info;
                in_warp_internal.p_warp_internal = &p_filters->input_param.warp_internal;
                in_warp_internal.dram_efficiency = p_filters->input_param.warp_buf_info.dram_efficiency;
                in_warp_internal.force_vscale_sec_n = 0U;

                out_warp.p_cr_33 = p_flow_tbl_list->p_CR_buf_33;
                out_warp.cr_33_size = CR_SIZE_33;
                out_warp.p_cr_34 = p_flow_tbl_list->p_CR_buf_34;
                out_warp.cr_34_size = CR_SIZE_34;
                out_warp.p_cr_35 = p_flow_tbl_list->p_CR_buf_35;
                out_warp.cr_35_size = CR_SIZE_35;
                out_warp.p_cr_42 = p_flow_tbl_list->p_CR_buf_42;
                out_warp.cr_42_size = CR_SIZE_42;
                out_warp.p_cr_43 = p_flow_tbl_list->p_CR_buf_43;
                out_warp.cr_43_size = CR_SIZE_43;
                out_warp.p_cr_44 = p_flow_tbl_list->p_CR_buf_44;
                out_warp.cr_44_size = CR_SIZE_44;
                out_warp.p_flow_info = &p_flow->flow_info;
                out_warp.p_window = &p_flow->window;
                out_warp.p_phase = &p_flow->phase;
                out_warp.p_calib = &p_flow->calib;
                out_warp.p_stitch = &p_flow->stitch;
                out_warp.p_warp_horizontal_table_address = p_flow_tbl_list->p_warp_hor;
                out_warp.h_size = WARP_VIDEO_MAXHORGRID * WARP_VIDEO_MAXVERGRID * 2U * 2U;// multi_slice buffer.
                out_warp.p_warp_vertical_table_address = p_flow_tbl_list->p_warp_ver;
                out_warp.v_size = WARP_VIDEO_MAXHORGRID * WARP_VIDEO_MAXVERGRID * 2U * 2U;// multi_slice buffer.
                rval |= ikc_warp_internal(&in_warp_internal, &out_warp);
            }
        } else {
            ikc_in_warp_t in_warp;
            ikc_out_warp_t out_warp;

            bool_result = warp_flow_table_op_u32(p_filters->update_flags.cr.warp, p_filters->update_flags.iso.calib_warp_info_updated) +
                          equal_op_u32(p_filters->update_flags.cr.cr_33_update, 0U) +
                          equal_op_u32(p_filters->update_flags.cr.cr_34_update, 0U) +
                          equal_op_u32(p_filters->update_flags.cr.cr_35_update, 0U) +
                          equal_op_u32(p_filters->update_flags.cr.cr_42_update, 0U) +
                          equal_op_u32(p_filters->update_flags.cr.cr_43_update, 0U) +
                          equal_op_u32(p_filters->update_flags.cr.cr_44_update, 0U);
            if(bool_result != 0u) {
                amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_warp !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
                rval |= IK_ERR_0205;//Ring buffer Eorror
            } else {
                in_warp.is_group_cmd = 0u;
                in_warp.warp_enable = (uint32)p_filters->input_param.warp_enable;
                in_warp.flip_mode = p_filters->input_param.flip_mode;
                in_warp.p_window_size_info = &p_filters->input_param.window_size_info;
                in_warp.p_act_win_crop = &p_filters->input_param.ctx_buf.result_win.act_win_crop;
                in_warp.p_dmy_win_geo = &p_filters->input_param.ctx_buf.result_win.dmy_win_geo;
                in_warp.p_cfa_win_dim = &p_filters->input_param.ctx_buf.result_win.cfa_win_dim;
                in_warp.p_dmy_range = &p_filters->input_param.dmy_range;
                in_warp.p_stitching_info = &p_filters->input_param.stitching_info;
                in_warp.p_calib_warp_info = &p_filters->input_param.calib_warp_info;
                in_warp.dram_efficiency = p_filters->input_param.warp_buf_info.dram_efficiency;
                in_warp.force_vscale_sec_n = 0U;

                out_warp.p_cr_33 = p_flow_tbl_list->p_CR_buf_33;
                out_warp.cr_33_size = CR_SIZE_33;
                out_warp.p_cr_34 = p_flow_tbl_list->p_CR_buf_34;
                out_warp.cr_34_size = CR_SIZE_34;
                out_warp.p_cr_35 = p_flow_tbl_list->p_CR_buf_35;
                out_warp.cr_35_size = CR_SIZE_35;
                out_warp.p_cr_42 = p_flow_tbl_list->p_CR_buf_42;
                out_warp.cr_42_size = CR_SIZE_42;
                out_warp.p_cr_43 = p_flow_tbl_list->p_CR_buf_43;
                out_warp.cr_43_size = CR_SIZE_43;
                out_warp.p_cr_44 = p_flow_tbl_list->p_CR_buf_44;
                out_warp.cr_44_size = CR_SIZE_44;
                out_warp.p_flow_info = &p_flow->flow_info;
                out_warp.p_window = &p_flow->window;
                out_warp.p_phase = &p_flow->phase;
                out_warp.p_calib = &p_flow->calib;
                out_warp.p_stitch = &p_flow->stitch;
                out_warp.p_warp_horizontal_table_address = p_flow_tbl_list->p_warp_hor;
                out_warp.h_size = WARP_VIDEO_MAXHORGRID * WARP_VIDEO_MAXVERGRID * 2U * 2U;// multi_slice buffer.
                out_warp.p_warp_vertical_table_address = p_flow_tbl_list->p_warp_ver;
                out_warp.v_size = WARP_VIDEO_MAXHORGRID * WARP_VIDEO_MAXVERGRID * 2U * 2U;// multi_slice buffer.

                rval |= ikc_warp(&in_warp, &out_warp);
            }
        }
        p_flow->calib.update_lens_warp = 1u;
    } else {
        p_flow->calib.update_lens_warp = 0u;
    }

    return rval;
}
static inline uint32 process_calib_cr_warp_part2(idsp_ik_flow_ctrl_t *p_flow, amba_ik_flow_tables_list_t *p_flow_tbl_list, amba_ik_filter_t *p_filters, uint32 window_calculate_updated)
{
    uint32 rval = IK_OK;
    uint8 bool_result;
    uint32 is_y2y_420input = (p_filters->input_param.yuv_mode == AMBA_IK_YUV420) ? 1UL : 0UL;

    (void)amba_ik_system_memcpy(p_flow, p_flow, sizeof(uint8));//misra fix.
    (void)amba_ik_system_memcpy(p_flow_tbl_list, p_flow_tbl_list, sizeof(uint8));//misra fix.
    (void)amba_ik_system_memcpy(p_filters, p_filters, sizeof(uint8));//misra fix.

    // avoid warp interpolation when chroma radius update, to reduce performance overhead.
    // MUST use this after ikc_warp or ikc_warp_internal, due to sec2_output_size
    bool_result = equal_op_u32(p_filters->update_flags.iso.chroma_filter_updated, 1U) +
                  equal_op_u32(p_filters->update_flags.iso.warp_enable_updated, 1U) +
                  equal_op_u32(p_filters->update_flags.iso.calib_warp_info_updated, 1U) +
                  equal_op_u32(p_filters->update_flags.iso.warp_internal_updated, 1U) +
                  equal_op_u32(p_filters->update_flags.iso.window_size_info_updated, 1U) +
                  equal_op_u32(p_filters->update_flags.iso.warp_buf_info_updated, 1U) +
                  equal_op_u32(window_calculate_updated, 1UL);

    if(bool_result != 0u) {
        ikc_in_warp_radius_t in_warp_radius;
        ikc_out_warp_radius_t out_warp_radius;
        if(p_filters->input_param.ctx_buf.warp_internal_mode_flag == 1U) {//internal warp
            // we don't need warp table if only radius update
            bool_result = //equal_op_u32(p_filters->update_flags.cr.warp, 0U) +
                equal_op_u32(p_filters->update_flags.cr.cr_33_update, 0U) +
                equal_op_u32(p_filters->update_flags.cr.cr_34_update, 0U) +
                equal_op_u32(p_filters->update_flags.cr.cr_35_update, 0U) +
                equal_op_u32(p_filters->update_flags.cr.cr_42_update, 0U) +
                equal_op_u32(p_filters->update_flags.cr.cr_43_update, 0U) +
                equal_op_u32(p_filters->update_flags.cr.cr_44_update, 0U);
            if(bool_result != 0u) {
                amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_warp_internal !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
                rval |= IK_ERR_0205;//Ring buffer Eorror
            } else {
                in_warp_radius.chroma_radius = p_filters->input_param.chroma_filter.radius;
                in_warp_radius.is_y2y_420input = is_y2y_420input;
                in_warp_radius.luma_wait_lines = p_filters->input_param.warp_buf_info.luma_wait_lines;
                in_warp_radius.luma_dma_size = p_filters->input_param.warp_buf_info.luma_dma_size;
#if defined(EARLYTEST_ENV)
                in_warp_radius.chroma_dma_size = p_filters->input_param.warp_buf_info.chroma_dma_size;
#endif
                out_warp_radius.p_cr_35 = p_flow_tbl_list->p_CR_buf_35;
                out_warp_radius.cr_35_size = CR_SIZE_35;
                out_warp_radius.p_cr_42 = p_flow_tbl_list->p_CR_buf_42;
                out_warp_radius.cr_42_size = CR_SIZE_42;
                out_warp_radius.p_window = &p_flow->window;
                out_warp_radius.p_calib = &p_flow->calib;

                rval |= ikc_warp_radius_internal(&in_warp_radius, &out_warp_radius);
            }
        } else {
            // we don't need warp table if only radius update
            bool_result = //equal_op_u32(p_filters->update_flags.cr.warp, 0U) +
                equal_op_u32(p_filters->update_flags.cr.cr_33_update, 0U) +
                equal_op_u32(p_filters->update_flags.cr.cr_34_update, 0U) +
                equal_op_u32(p_filters->update_flags.cr.cr_35_update, 0U) +
                equal_op_u32(p_filters->update_flags.cr.cr_42_update, 0U) +
                equal_op_u32(p_filters->update_flags.cr.cr_43_update, 0U) +
                equal_op_u32(p_filters->update_flags.cr.cr_44_update, 0U);
            if(bool_result != 0u) {
                amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_warp_radius !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
                rval |= IK_ERR_0205;//Ring buffer Eorror
            } else {
                in_warp_radius.is_group_cmd = 0u;
                in_warp_radius.chroma_radius = p_filters->input_param.chroma_filter.radius;
                in_warp_radius.is_y2y_420input = is_y2y_420input;
                in_warp_radius.dram_efficiency = p_filters->input_param.warp_buf_info.dram_efficiency;
                in_warp_radius.luma_wait_lines = p_filters->input_param.warp_buf_info.luma_wait_lines;
                in_warp_radius.luma_dma_size = p_filters->input_param.warp_buf_info.luma_dma_size;
#if defined(EARLYTEST_ENV)
                in_warp_radius.chroma_dma_size = p_filters->input_param.warp_buf_info.chroma_dma_size;
#endif
                out_warp_radius.p_cr_35 = p_flow_tbl_list->p_CR_buf_35;
                out_warp_radius.cr_35_size = CR_SIZE_35;
                out_warp_radius.p_cr_42 = p_flow_tbl_list->p_CR_buf_42;
                out_warp_radius.cr_42_size = CR_SIZE_42;
                out_warp_radius.p_window = &p_flow->window;
                out_warp_radius.p_calib = &p_flow->calib;

                rval |= ikc_warp_radius(&in_warp_radius, &out_warp_radius);
            }
        }
        p_flow->calib.update_lens_warp = 1u;
    } else {
        p_flow->calib.update_lens_warp = 0u;
    }

    return rval;
}

uint32 exe_process_calib_cr(idsp_ik_flow_ctrl_t *p_flow, amba_ik_flow_tables_list_t *p_flow_tbl_list, amba_ik_filter_t *p_filters)
{
    uint32 rval = IK_OK;
    uint32 window_calculate_updated = 0UL;
    uint8 bool_result;

    bool_result = equal_op_u32(p_filters->update_flags.iso.window_size_info_updated, 1U) +
                  equal_op_u32(p_filters->update_flags.iso.dmy_range_updated, 1U) +
                  equal_op_u32(p_filters->update_flags.iso.dzoom_info_updated, 1U) +
                  equal_op_u32(p_filters->update_flags.iso.vin_active_win_updated, 1U) +
                  equal_op_u32(p_filters->update_flags.iso.stitching_info_updated, 1U) +
                  equal_op_u32(p_filters->update_flags.iso.flip_mode_updated, 1U) +
                  equal_op_u32(p_filters->update_flags.iso.warp_enable_updated, 1U);
    if (bool_result != 0u) {
        window_calculate_updated = 1UL;
    }

    // input mode
    if(p_filters->update_flags.iso.is_1st_frame == 1U) { //should only configure once at 1st time.
        ikc_in_input_mode_t in_input_mode;
        ikc_out_input_mode_t out_input_mode;

        rval |= exe_check_vin_sensor(p_filters);
        bool_result = equal_op_u32(p_filters->update_flags.cr.cr_4_update, 0U) +
                      equal_op_u32(p_filters->update_flags.cr.cr_6_update, 0U) +
                      equal_op_u32(p_filters->update_flags.cr.cr_7_update, 0U) +
                      equal_op_u32(p_filters->update_flags.cr.cr_8_update, 0U) +
                      equal_op_u32(p_filters->update_flags.cr.cr_9_update, 0U) +
                      equal_op_u32(p_filters->update_flags.cr.cr_11_update, 0U) +
                      equal_op_u32(p_filters->update_flags.cr.cr_12_update, 0U) +
                      equal_op_u32(p_filters->update_flags.cr.cr_13_update, 0U) +
                      equal_op_u32(p_filters->update_flags.cr.cr_16_update, 0U) +
                      equal_op_u32(p_filters->update_flags.cr.cr_21_update, 0U) +
                      equal_op_u32(p_filters->update_flags.cr.cr_22_update, 0U) +
                      equal_op_u32(p_filters->update_flags.cr.cr_23_update, 0U) +
                      equal_op_u32(p_filters->update_flags.cr.cr_24_update, 0U) +
                      equal_op_u32(p_filters->update_flags.cr.cr_26_update, 0U) +
                      equal_op_u32(p_filters->update_flags.cr.cr_29_update, 0U) +
                      equal_op_u32(p_filters->update_flags.cr.cr_30_update, 0U) +
                      equal_op_u32(p_filters->update_flags.cr.cr_31_update, 0U) +
                      equal_op_u32(p_filters->update_flags.cr.cr_32_update, 0U) +
                      equal_op_u32(p_filters->update_flags.cr.cr_117_update, 0U);
        if(bool_result != 0u) {
            amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_input_mode !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
            rval |= IK_ERR_0205;//Ring buffer Eorror
        } else {
            in_input_mode.flip_mode = p_filters->input_param.flip_mode;
            in_input_mode.p_sensor_info = &p_filters->input_param.sensor_info;
            out_input_mode.p_cr_4 = p_flow_tbl_list->p_CR_buf_4;
            out_input_mode.cr_4_size = CR_SIZE_4;
            out_input_mode.p_cr_6 = p_flow_tbl_list->p_CR_buf_6;
            out_input_mode.cr_6_size = CR_SIZE_6;
            out_input_mode.p_cr_7 = p_flow_tbl_list->p_CR_buf_7;
            out_input_mode.cr_7_size = CR_SIZE_7;
            out_input_mode.p_cr_8 = p_flow_tbl_list->p_CR_buf_8;
            out_input_mode.cr_8_size = CR_SIZE_8;
            out_input_mode.p_cr_9 = p_flow_tbl_list->p_CR_buf_9;
            out_input_mode.cr_9_size = CR_SIZE_9;
            out_input_mode.p_cr_11 = p_flow_tbl_list->p_CR_buf_11;
            out_input_mode.cr_11_size = CR_SIZE_11;
            out_input_mode.p_cr_12 = p_flow_tbl_list->p_CR_buf_12;
            out_input_mode.cr_12_size = CR_SIZE_12;
            out_input_mode.p_cr_13 = p_flow_tbl_list->p_CR_buf_13;
            out_input_mode.cr_13_size = CR_SIZE_13;
            out_input_mode.p_cr_16 = p_flow_tbl_list->p_CR_buf_16;
            out_input_mode.cr_16_size = CR_SIZE_16;
            out_input_mode.p_cr_21 = p_flow_tbl_list->p_CR_buf_21;
            out_input_mode.cr_21_size = CR_SIZE_21;
            out_input_mode.p_cr_22 = p_flow_tbl_list->p_CR_buf_22;
            out_input_mode.cr_22_size = CR_SIZE_22;
            out_input_mode.p_cr_23 = p_flow_tbl_list->p_CR_buf_23;
            out_input_mode.cr_23_size = CR_SIZE_23;
            out_input_mode.p_cr_24 = p_flow_tbl_list->p_CR_buf_24;
            out_input_mode.cr_24_size = CR_SIZE_24;
            out_input_mode.p_cr_26 = p_flow_tbl_list->p_CR_buf_26;
            out_input_mode.cr_26_size = CR_SIZE_26;
            out_input_mode.p_cr_29 = p_flow_tbl_list->p_CR_buf_29;
            out_input_mode.cr_29_size = CR_SIZE_29;
            out_input_mode.p_cr_30 = p_flow_tbl_list->p_CR_buf_30;
            out_input_mode.cr_30_size = CR_SIZE_30;
            out_input_mode.p_cr_31 = p_flow_tbl_list->p_CR_buf_31;
            out_input_mode.cr_31_size = CR_SIZE_31;
            out_input_mode.p_cr_32 = p_flow_tbl_list->p_CR_buf_32;
            out_input_mode.cr_32_size = CR_SIZE_32;
            out_input_mode.p_cr_117 = p_flow_tbl_list->p_CR_buf_117;
            out_input_mode.cr_117_size = CR_SIZE_117;
            out_input_mode.p_flow = &p_flow->flow_info;
            out_input_mode.p_calib = &p_flow->calib;

            if (p_filters->input_param.ability == AMBA_IK_VIDEO_Y2Y) {
                in_input_mode.is_yuv_mode = 1UL;
            } else {
                in_input_mode.is_yuv_mode = 0UL;
            }
            rval |= ikc_input_mode(&in_input_mode, &out_input_mode);
        }
    }

    if (window_calculate_updated == 1UL) {
        rval |= exe_win_calc_wrapper(p_filters);
    }

    if (p_filters->input_param.ability != AMBA_IK_VIDEO_Y2Y) {
        //complexity refine.
        rval |= process_calib_cr_non_y2y_part1(p_flow, p_flow_tbl_list, p_filters, window_calculate_updated);
        rval |= process_calib_cr_non_y2y_part2(p_flow, p_flow_tbl_list, p_filters, window_calculate_updated);
    } else {
        p_flow->flow_info.update_sbp = 0u;
        p_flow->flow_info.update_vignette = 0u;
        p_flow->calib.update_CA_warp = 0u;
        p_flow->calib.update_lens_warp = 1u;
    }

    //complexity refine.
    rval |= process_calib_cr_warp_part1(p_flow, p_flow_tbl_list, p_filters, window_calculate_updated);
    rval |= process_calib_cr_warp_part2(p_flow, p_flow_tbl_list, p_filters, window_calculate_updated);

    return rval;
}

static inline uint32 process_hdr_ce_cr_part1(idsp_ik_flow_ctrl_t *p_flow, amba_ik_flow_tables_list_t *p_flow_tbl_list, amba_ik_filter_t *p_filters, uint32 exp_num, uint32 window_calculate_updated)
{
    uint32 rval = IK_OK;
    uint8 bool_result;

    (void)amba_ik_system_memcpy(p_flow, p_flow, sizeof(uint8));//misra fix.
    (void)amba_ik_system_memcpy(p_flow_tbl_list, p_flow_tbl_list, sizeof(uint8));//misra fix.

    // hdr_blend and hdr_alpha
    if(p_filters->update_flags.iso.hdr_blend_updated == 1u) {
        ikc_in_hdr_blend_t in_hdr_blend;
        ikc_out_hdr_blend_t out_hdr_blend;

        if(p_filters->update_flags.cr.cr_4_update == 0U) {
            amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_hdr_blend !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
            rval |= IK_ERR_0205;//Ring buffer Eorror
        } else {
            in_hdr_blend.exp_num = exp_num;
            in_hdr_blend.p_hdr_raw_info = &p_filters->input_param.hdr_raw_info;
            in_hdr_blend.p_hdr_blend = &p_filters->input_param.hdr_blend;
            out_hdr_blend.p_cr_4 = p_flow_tbl_list->p_CR_buf_4;
            out_hdr_blend.cr_4_size = CR_SIZE_4;
            out_hdr_blend.p_flow = &p_flow->flow_info;
            rval |= ikc_hdr_blend(&in_hdr_blend, &out_hdr_blend);
        }
    }

    // hdr_tone_curve
    if(p_filters->update_flags.iso.fe_tone_curve_updated == 1u) {
        ikc_in_hdr_tone_curve_t in_hdr_tc;
        ikc_out_hdr_tone_curve_t out_hdr_tc;

        bool_result = equal_op_u32(p_filters->update_flags.cr.cr_4_update, 0U) + equal_op_u32(p_filters->update_flags.cr.cr_5_update, 0U);
        if(bool_result != 0u) {
            amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_front_end_tone_curve !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
            rval |= IK_ERR_0205;//Ring buffer Eorror
        } else {
            in_hdr_tc.p_fe_tone_curve = &p_filters->input_param.fe_tone_curve;
            out_hdr_tc.p_cr_4 = p_flow_tbl_list->p_CR_buf_4;
            out_hdr_tc.cr_4_size = CR_SIZE_4;
            out_hdr_tc.p_cr_5 = p_flow_tbl_list->p_CR_buf_5;
            out_hdr_tc.cr_5_size = CR_SIZE_5;
            rval |= ikc_front_end_tone_curve(&in_hdr_tc, &out_hdr_tc);
        }
    }

    // losy decompress
    bool_result = equal_op_u32(p_filters->update_flags.iso.sensor_information_updated, 1u) + equal_op_u32(window_calculate_updated, 1UL);
    if(bool_result != 0u) {
        ikc_in_decompress_t in_decompress;
        ikc_out_decompress_t out_decompress;
        //save the 1st frame compression offset.
        if(p_filters->update_flags.iso.is_1st_frame == 1u) {
            p_filters->input_param.ctx_buf.first_compression_offset = p_filters->input_param.sensor_info.compression_offset;
        }

        if(p_filters->update_flags.cr.cr_4_update == 0U) {
            amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_decompression !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
            rval |= IK_ERR_0205;//Ring buffer Eorror
        } else {
            in_decompress.exp_num = exp_num;
            in_decompress.compress_mode = p_filters->input_param.sensor_info.compression;////0~8, 256~259.
            in_decompress.p_dmy_win_geo = &p_filters->input_param.ctx_buf.result_win.dmy_win_geo;
            out_decompress.p_cr_4 = p_flow_tbl_list->p_CR_buf_4;
            out_decompress.cr_4_size = CR_SIZE_4;
            rval |= ikc_decompression(&in_decompress, &out_decompress);
        }
    }
    //sanity check, mutually exclusive between TC / compression_offset.
    bool_result = non_equal_op_u32(p_filters->input_param.fe_tone_curve.decompand_enable, 0U) +
                  non_equal_op_u32((uint32)p_filters->input_param.ctx_buf.first_compression_offset, 0u);
    if(bool_result == 2u) {
        amba_ik_system_print_int32_5("[IK] exe_process_hdr_ce_cr fail, invalid decompand_enable : %d, first_compression_offset : %d", (int32)p_filters->input_param.fe_tone_curve.decompand_enable, p_filters->input_param.ctx_buf.first_compression_offset, DC_I, DC_I, DC_I);
        rval |= IK_ERR_0008;
    }

    return rval;
}

static inline uint32 process_hdr_ce_cr_part2_a(idsp_ik_flow_ctrl_t *p_flow, amba_ik_flow_tables_list_t *p_flow_tbl_list, amba_ik_filter_t *p_filters, uint32 exp_num, uint32 window_calculate_updated)
{
    uint32 rval = IK_OK;
    uint8 bool_result;

    (void)amba_ik_system_memcpy(p_flow, p_flow, sizeof(uint8));//misra fix.
    (void)amba_ik_system_memcpy(p_flow_tbl_list, p_flow_tbl_list, sizeof(uint8));//misra fix.
    if (window_calculate_updated == 1UL) {;}

    // hdr exp
    bool_result = equal_op_u32(p_filters->update_flags.iso.exp0_fe_static_blc_updated, 1u) + equal_op_u32(p_filters->update_flags.iso.exp1_fe_static_blc_updated, 1u) +
                  equal_op_u32(p_filters->update_flags.iso.exp2_fe_static_blc_updated, 1u) + equal_op_u32(p_filters->update_flags.iso.sensor_information_updated, 1u) +
                  equal_op_u32(p_filters->update_flags.iso.rgb_ir_updated, 1u);
    if(bool_result != 0u) {
        ikc_in_hdr_blc_t in_hdr_blc;
        ikc_out_hdr_blc_t out_hdr_blc;
        int32 compression_offset = ((greater_op_u32(p_filters->input_param.sensor_info.compression, IK_RAW_COMPRESS_OFF)+less_equal_op_u32(p_filters->input_param.sensor_info.compression, IK_RAW_COMPRESS_10p5))==2U) ?
                                   p_filters->input_param.ctx_buf.first_compression_offset : 0;

        bool_result = equal_op_u32(p_filters->update_flags.cr.cr_4_update, 0U) + equal_op_u32(p_filters->update_flags.cr.cr_6_update, 0U) +
                      equal_op_u32(p_filters->update_flags.cr.cr_7_update, 0U) + equal_op_u32(p_filters->update_flags.cr.cr_8_update, 0U);
        if(bool_result != 0u) {
            amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_hdr_black_level !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
            rval |= IK_ERR_0205;//Ring buffer Eorror
        } else {
            in_hdr_blc.exp_num = exp_num;
            in_hdr_blc.rgb_ir_mode = p_filters->input_param.rgb_ir.mode;
            in_hdr_blc.compression_offset = compression_offset;
            (void)amba_ik_system_memcpy(&p_filters->input_param.ctx_buf.ir_only_trick_buffer.exp0_frontend_static_blc, &p_filters->input_param.exp0_frontend_static_blc, sizeof(ik_static_blc_level_t));
            (void)amba_ik_system_memcpy(&p_filters->input_param.ctx_buf.ir_only_trick_buffer.exp1_frontend_static_blc, &p_filters->input_param.exp1_frontend_static_blc, sizeof(ik_static_blc_level_t));
            (void)amba_ik_system_memcpy(&p_filters->input_param.ctx_buf.ir_only_trick_buffer.exp2_frontend_static_blc, &p_filters->input_param.exp2_frontend_static_blc, sizeof(ik_static_blc_level_t));
            in_hdr_blc.p_exp0_frontend_static_blc = &p_filters->input_param.ctx_buf.ir_only_trick_buffer.exp0_frontend_static_blc;
            in_hdr_blc.p_exp1_frontend_static_blc = &p_filters->input_param.ctx_buf.ir_only_trick_buffer.exp1_frontend_static_blc;
            in_hdr_blc.p_exp2_frontend_static_blc = &p_filters->input_param.ctx_buf.ir_only_trick_buffer.exp2_frontend_static_blc;
            out_hdr_blc.p_cr_4 = p_flow_tbl_list->p_CR_buf_4;
            out_hdr_blc.cr_4_size = CR_SIZE_4;
            out_hdr_blc.p_cr_6 = p_flow_tbl_list->p_CR_buf_6;
            out_hdr_blc.cr_6_size = CR_SIZE_6;
            out_hdr_blc.p_cr_7 = p_flow_tbl_list->p_CR_buf_7;
            out_hdr_blc.cr_7_size = CR_SIZE_7;
            out_hdr_blc.p_cr_8 = p_flow_tbl_list->p_CR_buf_8;
            out_hdr_blc.cr_8_size = CR_SIZE_8;
            if((equal_op_u32(p_filters->input_param.rgb_ir.ir_only, 1U)+equal_op_u32(p_filters->input_param.sensor_info.sensor_mode, 1U))==2U) { //ir_only, runing on RGB bayer pattern, carry IR on B channel.
                in_hdr_blc.rgb_ir_mode = 0;
                in_hdr_blc.p_exp0_frontend_static_blc->black_r = -16383;
                in_hdr_blc.p_exp1_frontend_static_blc->black_r = -16383;
                in_hdr_blc.p_exp2_frontend_static_blc->black_r = -16383;
                in_hdr_blc.p_exp0_frontend_static_blc->black_b = in_hdr_blc.p_exp0_frontend_static_blc->black_ir;
                in_hdr_blc.p_exp1_frontend_static_blc->black_b = in_hdr_blc.p_exp1_frontend_static_blc->black_ir;
                in_hdr_blc.p_exp2_frontend_static_blc->black_b = in_hdr_blc.p_exp2_frontend_static_blc->black_ir;
            }
            rval |= ikc_hdr_black_level(&in_hdr_blc, &out_hdr_blc);
        }
    }

    return rval;
}

static inline uint32 process_hdr_ce_cr_part2_b(idsp_ik_flow_ctrl_t *p_flow, amba_ik_flow_tables_list_t *p_flow_tbl_list, amba_ik_filter_t *p_filters, uint32 exp_num, uint32 window_calculate_updated)
{
    uint32 rval = IK_OK;
    uint8 bool_result;

    (void)amba_ik_system_memcpy(p_flow, p_flow, sizeof(uint8));//misra fix.
    (void)amba_ik_system_memcpy(p_flow_tbl_list, p_flow_tbl_list, sizeof(uint8));//misra fix.
    if (window_calculate_updated == 1UL) {;}

    // hdr exp
    bool_result = equal_op_u32(p_filters->update_flags.iso.exp0_fe_wb_gain_updated, 1u) + equal_op_u32(p_filters->update_flags.iso.exp1_fe_wb_gain_updated, 1u) +
                  equal_op_u32(p_filters->update_flags.iso.exp2_fe_wb_gain_updated, 1u) + equal_op_u32(p_filters->update_flags.iso.exp0_fe_static_blc_updated, 1u) +
                  equal_op_u32(p_filters->update_flags.iso.exp1_fe_static_blc_updated, 1u) + equal_op_u32(p_filters->update_flags.iso.exp2_fe_static_blc_updated, 1u) +
                  equal_op_u32(p_filters->update_flags.iso.sensor_information_updated, 1u) + equal_op_u32(p_filters->update_flags.iso.rgb_ir_updated, 1u);
    if(bool_result != 0u) {
        ikc_in_hdr_dgain_t in_hdr_dgain;
        ikc_out_hdr_dgain_t out_hdr_dgain;

        if(p_filters->update_flags.cr.cr_4_update == 0U) {
            amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_hdr_dgain !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
            rval |= IK_ERR_0205;//Ring buffer Eorror
        } else {
            in_hdr_dgain.exp_num = exp_num;
            in_hdr_dgain.rgb_ir_mode = p_filters->input_param.rgb_ir.mode;
            (void)amba_ik_system_memcpy(&p_filters->input_param.ctx_buf.ir_only_trick_buffer.exp0_frontend_wb_gain, &p_filters->input_param.exp0_frontend_wb_gain, sizeof(ik_frontend_wb_gain_t));
            (void)amba_ik_system_memcpy(&p_filters->input_param.ctx_buf.ir_only_trick_buffer.exp1_frontend_wb_gain, &p_filters->input_param.exp1_frontend_wb_gain, sizeof(ik_frontend_wb_gain_t));
            (void)amba_ik_system_memcpy(&p_filters->input_param.ctx_buf.ir_only_trick_buffer.exp2_frontend_wb_gain, &p_filters->input_param.exp2_frontend_wb_gain, sizeof(ik_frontend_wb_gain_t));
            in_hdr_dgain.p_exp0_frontend_wb_gain = &p_filters->input_param.ctx_buf.ir_only_trick_buffer.exp0_frontend_wb_gain;
            in_hdr_dgain.p_exp1_frontend_wb_gain = &p_filters->input_param.ctx_buf.ir_only_trick_buffer.exp1_frontend_wb_gain;
            in_hdr_dgain.p_exp2_frontend_wb_gain = &p_filters->input_param.ctx_buf.ir_only_trick_buffer.exp2_frontend_wb_gain;

            (void)amba_ik_system_memcpy(&p_filters->input_param.ctx_buf.ir_only_trick_buffer.exp0_frontend_static_blc, &p_filters->input_param.exp0_frontend_static_blc, sizeof(ik_static_blc_level_t));
            (void)amba_ik_system_memcpy(&p_filters->input_param.ctx_buf.ir_only_trick_buffer.exp1_frontend_static_blc, &p_filters->input_param.exp1_frontend_static_blc, sizeof(ik_static_blc_level_t));
            (void)amba_ik_system_memcpy(&p_filters->input_param.ctx_buf.ir_only_trick_buffer.exp2_frontend_static_blc, &p_filters->input_param.exp2_frontend_static_blc, sizeof(ik_static_blc_level_t));
            in_hdr_dgain.p_exp0_frontend_static_blc = &p_filters->input_param.ctx_buf.ir_only_trick_buffer.exp0_frontend_static_blc;
            in_hdr_dgain.p_exp1_frontend_static_blc = &p_filters->input_param.ctx_buf.ir_only_trick_buffer.exp1_frontend_static_blc;
            in_hdr_dgain.p_exp2_frontend_static_blc = &p_filters->input_param.ctx_buf.ir_only_trick_buffer.exp2_frontend_static_blc;

            out_hdr_dgain.p_cr_4 = p_flow_tbl_list->p_CR_buf_4;
            out_hdr_dgain.cr_4_size = CR_SIZE_4;
            if((equal_op_u32(p_filters->input_param.rgb_ir.ir_only, 1U)+equal_op_u32(p_filters->input_param.sensor_info.sensor_mode, 1U))==2U) { //ir_only, runing on RGB bayer pattern, carry IR on B channel.
                in_hdr_dgain.rgb_ir_mode = 0;
                in_hdr_dgain.p_exp0_frontend_wb_gain->b_gain = in_hdr_dgain.p_exp0_frontend_wb_gain->ir_gain;
                in_hdr_dgain.p_exp1_frontend_wb_gain->b_gain = in_hdr_dgain.p_exp1_frontend_wb_gain->ir_gain;
                in_hdr_dgain.p_exp2_frontend_wb_gain->b_gain = in_hdr_dgain.p_exp2_frontend_wb_gain->ir_gain;

                in_hdr_dgain.p_exp0_frontend_static_blc->black_r = -16383;
                in_hdr_dgain.p_exp1_frontend_static_blc->black_r = -16383;
                in_hdr_dgain.p_exp2_frontend_static_blc->black_r = -16383;
                in_hdr_dgain.p_exp0_frontend_static_blc->black_b = in_hdr_dgain.p_exp0_frontend_static_blc->black_ir;
                in_hdr_dgain.p_exp1_frontend_static_blc->black_b = in_hdr_dgain.p_exp1_frontend_static_blc->black_ir;
                in_hdr_dgain.p_exp2_frontend_static_blc->black_b = in_hdr_dgain.p_exp2_frontend_static_blc->black_ir;
            }
            rval |= ikc_hdr_dgain(&in_hdr_dgain, &out_hdr_dgain);
        }
    }

    return rval;
}

static inline uint32 process_hdr_ce_cr_part2(idsp_ik_flow_ctrl_t *p_flow, amba_ik_flow_tables_list_t *p_flow_tbl_list, amba_ik_filter_t *p_filters, uint32 exp_num, uint32 window_calculate_updated)
{
    uint32 rval = IK_OK;

    (void)amba_ik_system_memcpy(p_flow, p_flow, sizeof(uint8));//misra fix.
    (void)amba_ik_system_memcpy(p_flow_tbl_list, p_flow_tbl_list, sizeof(uint8));//misra fix.
    if (window_calculate_updated == 1UL) {;}

    // hdr exp
    //complexity refine.
    rval |= process_hdr_ce_cr_part2_a(p_flow, p_flow_tbl_list, p_filters, exp_num, window_calculate_updated);
    rval |= process_hdr_ce_cr_part2_b(p_flow, p_flow_tbl_list, p_filters, exp_num, window_calculate_updated);

    return rval;
}

static inline uint32 process_hdr_ce_cr_part3(idsp_ik_flow_ctrl_t *p_flow, amba_ik_flow_tables_list_t *p_flow_tbl_list, amba_ik_filter_t *p_filters, uint32 exp_num, uint32 window_calculate_updated)
{
    uint32 rval = IK_OK;
    uint8 bool_result;

    (void)amba_ik_system_memcpy(p_flow, p_flow, sizeof(uint8));//misra fix.
    (void)amba_ik_system_memcpy(p_flow_tbl_list, p_flow_tbl_list, sizeof(uint8));//misra fix.

    // ce
    if(exp_num > 0UL) {
        bool_result = equal_op_u32(p_filters->update_flags.iso.ce_updated, 1u) + equal_op_u32(window_calculate_updated, 1UL);
        if(bool_result != 0u) {
            ikc_in_ce_t in_ce;
            ikc_out_ce_t out_ce;

            if(p_filters->update_flags.cr.cr_13_update == 0U) {
                amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_contrast_enhancement !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
                rval |= IK_ERR_0205;//Ring buffer Eorror
            } else {
                in_ce.exp_num = exp_num;
                in_ce.sensor_pattern = p_filters->input_param.sensor_info.sensor_pattern;
                in_ce.p_dmy_win_geo = &p_filters->input_param.ctx_buf.result_win.dmy_win_geo;
                in_ce.p_ce = &p_filters->input_param.ce;
                out_ce.p_cr_13 = p_flow_tbl_list->p_CR_buf_13;
                out_ce.cr_13_size = CR_SIZE_13;
                out_ce.p_flow = &p_flow->flow_info;
                rval |= ikc_contrast_enhancement(&in_ce, &out_ce);
            }
        }
        if(p_filters->update_flags.iso.ce_input_table_updated == 1u) {
            ikc_in_ce_input_t in_ce_input;
            ikc_out_ce_input_t out_ce_input;

            bool_result = equal_op_u32(p_filters->update_flags.cr.cr_13_update, 0u) + equal_op_u32(p_filters->update_flags.cr.cr_14_update, 0u);
            if(bool_result != 0u) {
                amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_contrast_enhancement_input !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
                rval |= IK_ERR_0205;//Ring buffer Eorror
            } else {
                in_ce_input.ce_enable = p_filters->input_param.ce.enable;
                in_ce_input.p_ce_input_table = &p_filters->input_param.ce_input_table;
                out_ce_input.p_cr_13 = p_flow_tbl_list->p_CR_buf_13;
                out_ce_input.cr_13_size = CR_SIZE_13;
                out_ce_input.p_cr_14 = p_flow_tbl_list->p_CR_buf_14;
                out_ce_input.cr_14_size = CR_SIZE_14;
                rval |= ikc_contrast_enhancement_input(&in_ce_input, &out_ce_input);
            }
        }
        if(p_filters->update_flags.iso.ce_out_table_updated == 1u) {
            ikc_in_ce_output_t in_ce_output;
            ikc_out_ce_t out_ce;

            if(p_filters->update_flags.cr.cr_13_update == 0U) {
                amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_contrast_enhancement_output !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
                rval |= IK_ERR_0205;//Ring buffer Eorror
            } else {
                in_ce_output.ce_enable = p_filters->input_param.ce.enable;
                in_ce_output.p_ce_out_table = &p_filters->input_param.ce_out_table;
                out_ce.p_cr_13 = p_flow_tbl_list->p_CR_buf_13;
                out_ce.cr_13_size = CR_SIZE_13;
                rval |= ikc_contrast_enhancement_output(&in_ce_output, &out_ce);
            }
        }
    }

    return rval;
}

uint32 exe_process_hdr_ce_cr(idsp_ik_flow_ctrl_t *p_flow, amba_ik_flow_tables_list_t *p_flow_tbl_list, amba_ik_filter_t *p_filters, uint32 exp_num)
{
    uint32 rval = IK_OK;
    uint32 window_calculate_updated = 0UL;
    uint8 bool_result;

    bool_result = equal_op_u32(p_filters->update_flags.iso.window_size_info_updated, 1U) + equal_op_u32(p_filters->update_flags.iso.dmy_range_updated, 1U) +
                  equal_op_u32(p_filters->update_flags.iso.dzoom_info_updated, 1U) + equal_op_u32(p_filters->update_flags.iso.vin_active_win_updated, 1U) +
                  equal_op_u32(p_filters->update_flags.iso.stitching_info_updated, 1U) + equal_op_u32(p_filters->update_flags.iso.flip_mode_updated, 1U) +
                  equal_op_u32(p_filters->update_flags.iso.warp_enable_updated, 1U);
    if (bool_result != 0u) {
        window_calculate_updated = 1UL;
    }

    if (p_filters->input_param.ability != AMBA_IK_VIDEO_Y2Y) {
        // before_ce_gain
        if(p_filters->update_flags.iso.before_ce_wb_gain_updated == 1u) {
            ikc_in_before_ce_gain_t in_before_ce_gain;
            ikc_out_before_ce_gain_t out_before_ce_gain;

            if(p_filters->update_flags.cr.cr_13_update == 0U) {
                amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_before_ce_gain !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
                rval |= IK_ERR_0205;//Ring buffer Eorror
            } else {
                in_before_ce_gain.p_before_ce_wb_gain = &p_filters->input_param.before_ce_wb_gain;
                out_before_ce_gain.p_cr_13 = p_flow_tbl_list->p_CR_buf_13;
                out_before_ce_gain.cr_13_size = CR_SIZE_13;
                rval |= ikc_before_ce_gain(&in_before_ce_gain, &out_before_ce_gain);
            }
        }

        if(exp_num < 4UL) {
            //complexity refine.
            //hdr related.
            rval |= process_hdr_ce_cr_part1(p_flow, p_flow_tbl_list, p_filters, exp_num, window_calculate_updated);
            rval |= process_hdr_ce_cr_part2(p_flow, p_flow_tbl_list, p_filters, exp_num, window_calculate_updated);
            //ce related
            rval |= process_hdr_ce_cr_part3(p_flow, p_flow_tbl_list, p_filters, exp_num, window_calculate_updated);

        } else {
            amba_ik_system_print_uint32_5("[IK] exe_process_hdr_ce_cr fail, incorrect exp_num %d", exp_num, DC_U, DC_U, DC_U, DC_U);
            rval = IK_ERR_0000;
        }
    } else {
        amba_ik_system_print_uint32_5("[IK] exe_process_hdr_ce_cr fail, do not support y2y mode!", DC_U, DC_U, DC_U, DC_U, DC_U);
        rval = IK_ERR_0004;
    }

    return rval;
}

static inline uint32 process_cfa_cr_non_y2y_part1_a(idsp_ik_flow_ctrl_t *p_flow, amba_ik_flow_tables_list_t *p_flow_tbl_list, amba_ik_filter_t *p_filters, uint32 window_calculate_updated)
{
    uint32 rval = IK_OK;
    uint8 bool_result;
    (void)amba_ik_system_memcpy(p_flow, p_flow, sizeof(uint8));//misra fix.
    (void)amba_ik_system_memcpy(p_flow_tbl_list, p_flow_tbl_list, sizeof(uint8));//misra fix.
    if (window_calculate_updated==1UL) {;} //misra

    // dbp
    bool_result = equal_op_u32(p_filters->update_flags.iso.dynamic_bpc_updated, 1u) + equal_op_u32(p_filters->update_flags.iso.sensor_information_updated, 1u) +
                  equal_op_u32(p_filters->update_flags.iso.rgb_ir_updated, 1u);
    if(bool_result != 0u) {
        ikc_in_dynamic_bad_pixel_t in_dbp;
        ikc_out_dynamic_bad_pixel_t out_dbp;

        bool_result = equal_op_u32(p_filters->update_flags.cr.cr_11_update, 0U) + equal_op_u32(p_filters->update_flags.cr.cr_12_update, 0U);
        if(bool_result != 0U) {
            amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_dynamic_bad_pixel !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
            rval |= IK_ERR_0205;//Ring buffer Eorror
        } else {
            if(p_filters->input_param.ctx_buf.sbp_internal_mode_flag == 1u) {
                in_dbp.sbp_enable = p_filters->input_param.sbp_internal.enable;
            } else {
                in_dbp.sbp_enable = p_filters->input_param.sbp_enable;
            }
            if((equal_op_u32(p_filters->input_param.rgb_ir.ir_only, 1U)+equal_op_u32(p_filters->input_param.sensor_info.sensor_mode, 1U))==2U) { //ir_only, runing on RGB bayer pattern, carry IR on B channel.
                in_dbp.rgb_ir_mode = 0u;
            } else {
                in_dbp.rgb_ir_mode = p_filters->input_param.rgb_ir.mode;
            }
            in_dbp.p_dbp = &p_filters->input_param.dynamic_bpc;
            out_dbp.p_cr_11 = p_flow_tbl_list->p_CR_buf_11;
            out_dbp.cr_11_size = CR_SIZE_11;
            out_dbp.p_cr_12 = p_flow_tbl_list->p_CR_buf_12;
            out_dbp.cr_12_size = CR_SIZE_12;
            rval |= ikc_dynamic_bad_pixel(&in_dbp, &out_dbp);
        }
    }

    return rval;
}

static inline uint32 process_cfa_cr_non_y2y_part1(idsp_ik_flow_ctrl_t *p_flow, amba_ik_flow_tables_list_t *p_flow_tbl_list, amba_ik_filter_t *p_filters, uint32 window_calculate_updated)
{
    uint32 rval = IK_OK;
    uint8 bool_result;
    (void)amba_ik_system_memcpy(p_flow, p_flow, sizeof(uint8));//misra fix.
    (void)amba_ik_system_memcpy(p_flow_tbl_list, p_flow_tbl_list, sizeof(uint8));//misra fix.
    if (window_calculate_updated==1UL) {;} //misra

    // dbp
    //complexity refine.
    rval |= process_cfa_cr_non_y2y_part1_a(p_flow, p_flow_tbl_list, p_filters, window_calculate_updated);

    // grgb_mismatch
    if(p_filters->update_flags.iso.grgb_mismatch_updated == 1u) {
        ikc_in_grgb_mismatch_t in_grgb;
        ikc_out_grgb_mismatch_t out_grgb;

        if(p_filters->update_flags.cr.cr_23_update == 0U) {
            amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_grgb_mismatch !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
            rval |= IK_ERR_0205;//Ring buffer Eorror
        } else {
            in_grgb.p_grgb_mismatch = &p_filters->input_param.grgb_mismatch;
            out_grgb.p_cr_23 = p_flow_tbl_list->p_CR_buf_23;
            out_grgb.cr_23_size = CR_SIZE_23;
            rval |= ikc_grgb_mismatch(&in_grgb, &out_grgb);
        }
    }

    // cfa_leakage and anti_aliasing
    bool_result = equal_op_u32(p_filters->update_flags.iso.cfa_leakage_filter_updated, 1U) + equal_op_u32(p_filters->update_flags.iso.anti_aliasing_updated, 1U);
    if(bool_result != 0U) {
        ikc_in_cfa_leak_anti_alias_t in_cfa_leak_anti_alias;
        ikc_out_cfa_leak_anti_alias_t out_cfa_leak_anti_alias;

        if(p_filters->update_flags.cr.cr_12_update == 0U) {
            amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_cfa_leakage_anti_aliasing !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
            rval |= IK_ERR_0205;//Ring buffer Eorror
        } else {
            in_cfa_leak_anti_alias.p_cfa_leakage_filter = &p_filters->input_param.cfa_leakage_filter;
            in_cfa_leak_anti_alias.p_anti_aliasing = &p_filters->input_param.anti_aliasing;
            out_cfa_leak_anti_alias.p_cr_12 = p_flow_tbl_list->p_CR_buf_12;
            out_cfa_leak_anti_alias.cr_12_size = CR_SIZE_12;
            rval |= ikc_cfa_leakage_anti_aliasing(&in_cfa_leak_anti_alias, &out_cfa_leak_anti_alias);
        }
    }

    // cfa noise
    if(p_filters->update_flags.iso.cfa_noise_filter_updated == 1u) {
        ikc_in_cfa_noise_t in_cfa_noise;
        ikc_out_cfa_noise_t out_cfa_noise;

        if(p_filters->update_flags.cr.cr_23_update == 0U) {
            amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_cfa_noise !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
            rval |= IK_ERR_0205;//Ring buffer Eorror
        } else {
            in_cfa_noise.p_cfa_noise_filter = &p_filters->input_param.cfa_noise_filter;
            out_cfa_noise.p_cr_23 = p_flow_tbl_list->p_CR_buf_23;
            out_cfa_noise.cr_23_size = CR_SIZE_23;
            rval |= ikc_cfa_noise(&in_cfa_noise, &out_cfa_noise);
        }
    }
    return rval;
}

static inline uint32 process_cfa_cr_non_y2y_part2(idsp_ik_flow_ctrl_t *p_flow, amba_ik_flow_tables_list_t *p_flow_tbl_list, amba_ik_filter_t *p_filters, uint32 window_calculate_updated)
{
    uint32 rval = IK_OK;
    uint8 bool_result;
    (void)amba_ik_system_memcpy(p_flow, p_flow, sizeof(uint8));//misra fix.
    (void)amba_ik_system_memcpy(p_flow_tbl_list, p_flow_tbl_list, sizeof(uint8));//misra fix.

    // wb_gain
    if(p_filters->update_flags.iso.after_ce_wb_gain_updated == 1u) {
        ikc_in_after_ce_gain_t in_after_ce_gain;
        ikc_out_after_ce_gain_t out_after_ce_gain;

        if(p_filters->update_flags.cr.cr_22_update == 0U) {
            amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_after_ce_gain !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
            rval |= IK_ERR_0205;//Ring buffer Eorror
        } else {
            in_after_ce_gain.p_after_ce_wb_gain = &p_filters->input_param.after_ce_wb_gain;
            out_after_ce_gain.p_cr_22 = p_flow_tbl_list->p_CR_buf_22;
            out_after_ce_gain.cr_22_size = CR_SIZE_22;
            rval |= ikc_after_ce_gain(&in_after_ce_gain, &out_after_ce_gain);
        }
    }

    // rgbir
    bool_result = equal_op_u32(p_filters->update_flags.iso.rgb_ir_updated, 1U) + equal_op_u32(p_filters->update_flags.iso.sensor_information_updated, 1U);
    if(bool_result != 0U) {
        ikc_in_rgb_ir_t in_rgb_ir;
        ikc_out_rgb_ir_t out_rgb_ir;

        bool_result = equal_op_u32(p_filters->update_flags.cr.cr_4_update, 0U) + equal_op_u32(p_filters->update_flags.cr.cr_6_update, 0U) +
                      equal_op_u32(p_filters->update_flags.cr.cr_7_update, 0U) + equal_op_u32(p_filters->update_flags.cr.cr_8_update, 0U) +
                      equal_op_u32(p_filters->update_flags.cr.cr_9_update, 0U) + equal_op_u32(p_filters->update_flags.cr.cr_11_update, 0U) +
                      equal_op_u32(p_filters->update_flags.cr.cr_12_update, 0U) + equal_op_u32(p_filters->update_flags.cr.cr_21_update, 0U);
        if(bool_result != 0U) {
            amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_rgb_ir !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
            rval |= IK_ERR_0205;//Ring buffer Eorror
        } else {
            in_rgb_ir.flip_mode = p_filters->input_param.flip_mode;
            (void)amba_ik_system_memcpy(&p_filters->input_param.ctx_buf.ir_only_trick_buffer.rgb_ir, &p_filters->input_param.rgb_ir, sizeof(ik_rgb_ir_t));
            in_rgb_ir.p_rgb_ir = &p_filters->input_param.ctx_buf.ir_only_trick_buffer.rgb_ir;
            in_rgb_ir.p_before_ce_wb_gain = &p_filters->input_param.before_ce_wb_gain;
            in_rgb_ir.p_after_ce_wb_gain = &p_filters->input_param.after_ce_wb_gain;
            (void)amba_ik_system_memcpy(&p_filters->input_param.ctx_buf.ir_only_trick_buffer.exp0_frontend_wb_gain, &p_filters->input_param.exp0_frontend_wb_gain, sizeof(ik_frontend_wb_gain_t));
            in_rgb_ir.p_exp0_frontend_wb_gain = &p_filters->input_param.ctx_buf.ir_only_trick_buffer.exp0_frontend_wb_gain;
            out_rgb_ir.p_cr_4 = p_flow_tbl_list->p_CR_buf_4;
            out_rgb_ir.cr_4_size = CR_SIZE_4;
            out_rgb_ir.p_cr_6 = p_flow_tbl_list->p_CR_buf_6;
            out_rgb_ir.cr_6_size = CR_SIZE_6;
            out_rgb_ir.p_cr_7 = p_flow_tbl_list->p_CR_buf_7;
            out_rgb_ir.cr_7_size = CR_SIZE_7;
            out_rgb_ir.p_cr_8 = p_flow_tbl_list->p_CR_buf_8;
            out_rgb_ir.cr_8_size = CR_SIZE_8;
            out_rgb_ir.p_cr_9 = p_flow_tbl_list->p_CR_buf_9;
            out_rgb_ir.cr_9_size = CR_SIZE_9;
            out_rgb_ir.p_cr_11 = p_flow_tbl_list->p_CR_buf_11;
            out_rgb_ir.cr_11_size = CR_SIZE_11;
            out_rgb_ir.p_cr_12 = p_flow_tbl_list->p_CR_buf_12;
            out_rgb_ir.cr_12_size = CR_SIZE_12;
            out_rgb_ir.p_cr_21 = p_flow_tbl_list->p_CR_buf_21;
            out_rgb_ir.cr_21_size = CR_SIZE_21;
            out_rgb_ir.p_flow = &p_flow->flow_info;
            if((equal_op_u32(p_filters->input_param.rgb_ir.ir_only, 1U)+equal_op_u32(p_filters->input_param.sensor_info.sensor_mode, 1U))==2U) { //ir_only, runing on RGB bayer pattern, carry IR on B channel.
                in_rgb_ir.p_rgb_ir->mode = 0;
                in_rgb_ir.p_rgb_ir->ir_only = 1u;
                in_rgb_ir.p_exp0_frontend_wb_gain->b_gain = in_rgb_ir.p_exp0_frontend_wb_gain->ir_gain;
            } else {
                in_rgb_ir.p_rgb_ir->ir_only = 0u;
            }
            rval |= ikc_rgb_ir(&in_rgb_ir, &out_rgb_ir);
        }
    }

    // CFA prescaler...
    bool_result = equal_op_u32(p_filters->update_flags.iso.resample_str_update, 1U) + equal_op_u32(p_filters->update_flags.iso.cawarp_enable_updated, 1U) +
                  equal_op_u32(window_calculate_updated, 1U);
    if (bool_result != 0U) {
        ikc_in_cfa_prescale_t in_cfa_prescale;
        ikc_out_cfa_prescale_t out_cfa_prescale;

        if(p_filters->update_flags.cr.cr_16_update == 0U) {
            amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_cfa_prescale !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
            rval |= IK_ERR_0205;//Ring buffer Eorror
        } else {
            in_cfa_prescale.sensor_readout_mode = p_filters->input_param.sensor_info.sensor_readout_mode;
            in_cfa_prescale.cfa_cut_off_freq = p_filters->input_param.resample_str.cfa_cut_off_freq;
            in_cfa_prescale.p_dmy_win_geo = &p_filters->input_param.ctx_buf.result_win.dmy_win_geo;
            in_cfa_prescale.p_cfa_win_dim = &p_filters->input_param.ctx_buf.result_win.cfa_win_dim;
            in_cfa_prescale.cawarp_enable = (uint32)p_filters->input_param.cawarp_enable;
            out_cfa_prescale.p_cr_16 = p_flow_tbl_list->p_CR_buf_16;
            out_cfa_prescale.cr_16_size = CR_SIZE_16;
            rval |= ikc_cfa_prescale(&in_cfa_prescale, &out_cfa_prescale);
        }
    }

    return rval;
}

uint32 exe_process_cfa_cr(idsp_ik_flow_ctrl_t *p_flow, amba_ik_flow_tables_list_t *p_flow_tbl_list, amba_ik_filter_t *p_filters)
{
    uint32 rval = IK_OK;
    uint32 window_calculate_updated = 0UL;
    uint8 bool_result;

    bool_result = equal_op_u32(p_filters->update_flags.iso.window_size_info_updated, 1U) + equal_op_u32(p_filters->update_flags.iso.dmy_range_updated, 1U) +
                  equal_op_u32(p_filters->update_flags.iso.dzoom_info_updated, 1U) + equal_op_u32(p_filters->update_flags.iso.vin_active_win_updated, 1U) +
                  equal_op_u32(p_filters->update_flags.iso.stitching_info_updated, 1U) + equal_op_u32(p_filters->update_flags.iso.flip_mode_updated, 1U) +
                  equal_op_u32(p_filters->update_flags.iso.warp_enable_updated, 1U);
    if (bool_result != 0u) {
        window_calculate_updated = 1UL;
    }

    if (p_flow == NULL) {/*misra fix*/}
    if (p_filters->input_param.ability != AMBA_IK_VIDEO_Y2Y) {
        //complexity refine.
        rval |= process_cfa_cr_non_y2y_part1(p_flow, p_flow_tbl_list, p_filters, window_calculate_updated);
        rval |= process_cfa_cr_non_y2y_part2(p_flow, p_flow_tbl_list, p_filters, window_calculate_updated);
    }

    // aaa_cfa
    bool_result = equal_op_u32(p_filters->update_flags.iso.aaa_stat_info_updated, 1u) + equal_op_u32(p_filters->update_flags.iso.af_stat_ex_info_updated, 1u) +
                  equal_op_u32(p_filters->update_flags.iso.pg_af_stat_ex_info_updated, 1u) + equal_op_u32(p_filters->update_flags.iso.histogram_info_update, 1u) +
                  equal_op_u32(p_filters->update_flags.iso.histogram_info_pg_update, 1u) + equal_op_u32(p_filters->update_flags.iso.window_size_info_updated, 1U) +
                  equal_op_u32(p_filters->update_flags.iso.sensor_information_updated, 1u) + equal_op_u32(p_filters->update_flags.iso.rgb_ir_updated, 1u) +
                  equal_op_u32(window_calculate_updated, 1UL);
    if(bool_result != 0u) {
        ikc_in_aaa_t in_aaa;
        ikc_out_aaa_t out_aaa;

        bool_result = equal_op_u32(p_filters->update_flags.cr.aaa_data, 0U) + equal_op_u32(p_filters->update_flags.cr.cr_9_update, 0U) +
                      equal_op_u32(p_filters->update_flags.cr.cr_12_update, 0U) + equal_op_u32(p_filters->update_flags.cr.cr_21_update, 0U) +
                      equal_op_u32(p_filters->update_flags.cr.cr_28_update, 0U);
        if(bool_result != 0u) {
            amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_aaa !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
            rval |= IK_ERR_0205;//Ring buffer Eorror
        } else {
            in_aaa.is_group_cmd = 0u;
            (void)amba_ik_system_memcpy(&p_filters->input_param.ctx_buf.ir_only_trick_buffer.sensor_info, &p_filters->input_param.sensor_info, sizeof(ik_vin_sensor_info_t));
            (void)amba_ik_system_memcpy(&p_filters->input_param.ctx_buf.ir_only_trick_buffer.rgb_ir, &p_filters->input_param.rgb_ir, sizeof(ik_rgb_ir_t));
            in_aaa.p_sensor_info = &p_filters->input_param.ctx_buf.ir_only_trick_buffer.sensor_info;//determine RGB-IR.
            in_aaa.p_rgb_ir = &p_filters->input_param.ctx_buf.ir_only_trick_buffer.rgb_ir;
            in_aaa.p_aaa_stat_info = &p_filters->input_param.aaa_stat_info;
            in_aaa.p_aaa_pg_af_stat_info = &p_filters->input_param.aaa_pg_stat_info;
            in_aaa.p_af_stat_ex_info= &p_filters->input_param.af_stat_ex_info;
            in_aaa.p_pg_af_stat_ex_info = &p_filters->input_param.pg_af_stat_ex_info;
            in_aaa.p_hist_info = &p_filters->input_param.hist_info;
            in_aaa.p_hist_info_pg = &p_filters->input_param.hist_info_pg;
            in_aaa.p_logical_dmy_win_geo = &p_filters->input_param.ctx_buf.result_win.logical_dmy_win_geo;
            in_aaa.p_cfa_win_dim = &p_filters->input_param.ctx_buf.result_win.cfa_win_dim;
            in_aaa.p_main = &p_filters->input_param.window_size_info.main_win;
            in_aaa.p_stitching_info = &p_filters->input_param.stitching_info;
            out_aaa.p_cr_9 = p_flow_tbl_list->p_CR_buf_9;
            out_aaa.cr_9_size = CR_SIZE_9;
            out_aaa.p_cr_12 = p_flow_tbl_list->p_CR_buf_12;
            out_aaa.cr_12_size = CR_SIZE_12;
            out_aaa.p_cr_21 = p_flow_tbl_list->p_CR_buf_21;
            out_aaa.cr_21_size = CR_SIZE_21;
            out_aaa.p_cr_28 = p_flow_tbl_list->p_CR_buf_28;
            out_aaa.cr_28_size = CR_SIZE_28;
            out_aaa.p_stitching_aaa = p_flow_tbl_list->p_aaa;

            if (p_filters->input_param.ability == AMBA_IK_VIDEO_Y2Y) {
                in_aaa.is_yuv_mode = 1UL;
            } else {
                in_aaa.is_yuv_mode = 0UL;
            }
            if((equal_op_u32(p_filters->input_param.rgb_ir.ir_only, 1U)+equal_op_u32(p_filters->input_param.sensor_info.sensor_mode, 1U))==2U) { //ir_only, runing on RGB bayer pattern, carry IR on B channel.
                in_aaa.p_sensor_info->sensor_mode = 0;
                in_aaa.p_rgb_ir->mode = 0;
            }
            rval |= ikc_aaa(&in_aaa, &out_aaa);
        }
    }

    return rval;
}

static inline uint32 process_rgb_cr_non_y2y_part1(amba_ik_flow_tables_list_t *p_flow_tbl_list, amba_ik_filter_t *p_filters)
{
    uint32 rval = IK_OK;
    uint8 bool_result;
    (void)amba_ik_system_memcpy(p_flow_tbl_list, p_flow_tbl_list, sizeof(uint8));//misra fix.

    // demosaic
    if(p_filters->update_flags.iso.demosaic_updated == 1U) {
        ikc_in_demosaic_t in_demosaic;
        ikc_out_demosaic_t out_demosaic;

        bool_result = equal_op_u32(p_filters->update_flags.cr.cr_12_update, 0U) + equal_op_u32(p_filters->update_flags.cr.cr_23_update, 0U) +
                      equal_op_u32(p_filters->update_flags.cr.cr_24_update, 0U);
        if(bool_result != 0U) {
            amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_demosaic_filter !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
            rval |= IK_ERR_0205;//Ring buffer Eorror
        } else {
            in_demosaic.p_demosaic = &p_filters->input_param.demosaic;
            out_demosaic.p_cr_12 = p_flow_tbl_list->p_CR_buf_12;
            out_demosaic.cr_12_size = CR_SIZE_12;
            out_demosaic.p_cr_23 = p_flow_tbl_list->p_CR_buf_23;
            out_demosaic.cr_23_size = CR_SIZE_23;
            out_demosaic.p_cr_24 = p_flow_tbl_list->p_CR_buf_24;
            out_demosaic.cr_24_size = CR_SIZE_24;
            rval |= ikc_demosaic_filter(&in_demosaic, &out_demosaic);
        }
    }

    // rgb_to_12y
    if(p_filters->update_flags.iso.rgb_to_12y_updated == 1U) {
        ikc_in_rgb12y_t in_rgb12y;
        ikc_out_rgb12y_t out_rgb12y;

        if(p_filters->update_flags.cr.cr_25_update == 0U) {
            amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_rgb12y !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
            rval |= IK_ERR_0205;//Ring buffer Eorror
        } else {
            in_rgb12y.p_rgb_to_12y = &p_filters->input_param.rgb_to_12y;
            out_rgb12y.p_cr_25 = p_flow_tbl_list->p_CR_buf_25;
            out_rgb12y.cr_25_size = CR_SIZE_25;
            rval |= ikc_rgb12y(&in_rgb12y, &out_rgb12y);
        }
    }

    return rval;
}

static inline uint32 process_rgb_cr_non_y2y_part2(amba_ik_flow_tables_list_t *p_flow_tbl_list, amba_ik_filter_t *p_filters)
{
    uint32 rval = IK_OK;
    uint8 bool_result;
    (void)amba_ik_system_memcpy(p_flow_tbl_list, p_flow_tbl_list, sizeof(uint8));//misra fix.

    // cc
    if(p_filters->update_flags.iso.is_1st_frame == 1U) {//should only configure once in 1st time, due to R2Y always turn on.
        ikc_in_cc_en_t in_cc_enb;
        ikc_out_cc_en_t out_cc_enb;

        if(p_filters->update_flags.cr.cr_26_update == 0U) {
            amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_color_correction_enable !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
            rval |= IK_ERR_0205;//Ring buffer Eorror
        } else {
            in_cc_enb.is_yuv_mode = 0;
            in_cc_enb.use_cc_for_yuv2yuv = 0;
            out_cc_enb.p_cr_26 = p_flow_tbl_list->p_CR_buf_26;
            out_cc_enb.cr_26_size = CR_SIZE_26;
            rval |= ikc_color_correction_enable(&in_cc_enb, &out_cc_enb);
        }
    }

    // cc_3d
    bool_result = equal_op_u32(p_filters->update_flags.iso.color_correction_updated, 1u) + equal_op_u32(p_filters->update_flags.iso.sensor_information_updated, 1u) +
                  equal_op_u32(p_filters->update_flags.iso.rgb_ir_updated, 1u);
    if(bool_result != 0u) {
        ikc_in_cc_t in_cc_3d;
        ikc_out_cc_t out_cc_3d;

        bool_result = equal_op_u32(p_filters->update_flags.cr.cr_26_update, 0U) + equal_op_u32(p_filters->update_flags.cr.cr_27_update, 0U);
        if(bool_result != 0u) {
            amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_color_correction !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
            rval |= IK_ERR_0205;//Ring buffer Eorror
        } else {
            in_cc_3d.p_color_correction = &p_filters->input_param.color_correction;
            if((equal_op_u32(p_filters->input_param.rgb_ir.ir_only, 1U)+equal_op_u32(p_filters->input_param.sensor_info.sensor_mode, 1U))==2U) { //ir_only, runing on RGB bayer pattern, carry IR on B channel.
                in_cc_3d.is_ir_only = 1u;
            } else {
                in_cc_3d.is_ir_only = 0u;
            }
            out_cc_3d.p_cr_26 = p_flow_tbl_list->p_CR_buf_26;
            out_cc_3d.cr_26_size = CR_SIZE_26;
            out_cc_3d.p_cr_27 = p_flow_tbl_list->p_CR_buf_27;
            out_cc_3d.cr_27_size = CR_SIZE_27;
            rval |= ikc_color_correction(&in_cc_3d, &out_cc_3d);
        }
    }

    // cc_out
    if(p_filters->update_flags.iso.tone_curve_updated == 1U) {
        ikc_in_cc_out_t in_cc_out;
        ikc_out_cc_out_t out_cc_out;

        if(p_filters->update_flags.cr.cr_26_update == 0U) {
            amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_color_correction_out !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
            rval |= IK_ERR_0205;//Ring buffer Eorror
        } else {
            in_cc_out.p_tone_curve = &p_filters->input_param.tone_curve;
            out_cc_out.p_cr_26 = p_flow_tbl_list->p_CR_buf_26;
            out_cc_out.cr_26_size = CR_SIZE_26;
            rval |= ikc_color_correction_out(&in_cc_out, &out_cc_out);
        }
    }

    return rval;
}

static inline uint32 process_rgb_cr_y2y(amba_ik_flow_tables_list_t *p_flow_tbl_list, amba_ik_filter_t *p_filters)
{
    uint32 rval = IK_OK;
    uint32 is_critical_frame_enable_cc = 0;
    uint8 bool_result;
    (void)amba_ik_system_memcpy(p_flow_tbl_list, p_flow_tbl_list, sizeof(uint8));//misra fix.

    //YUV input mode.
    //Since cc_reg has already been hacked as R2Y curve in default binary.
    //IK need to set bypass cc_reg curve if cc_3d + cc_out both enabled.
    //Otherwise disable CCs in Y2Y.
    bool_result = equal_op_u32(p_filters->input_param.use_cc_for_yuv2yuv, 0U) + equal_op_u32(p_filters->input_param.use_cc, 1U) +
                  equal_op_u32(p_filters->input_param.use_tone_curve, 1U);
    if (bool_result == 3U) {
        //enable Y2Y CC.
        p_filters->input_param.use_cc_for_yuv2yuv = 1U;
        is_critical_frame_enable_cc = 1U;
    }

    //Only configure when 1st frame or CCs activating frame.
    bool_result = equal_op_u32(p_filters->update_flags.iso.is_1st_frame, 1U) + equal_op_u32(is_critical_frame_enable_cc, 1U);
    if(bool_result != 0U) {
        ikc_in_cc_en_t in_cc_enb;
        ikc_out_cc_en_t out_cc_enb;

        if(p_filters->update_flags.cr.cr_26_update == 0U) {
            amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_color_correction_enable !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
            rval |= IK_ERR_0205;//Ring buffer Eorror
        } else {
            in_cc_enb.is_yuv_mode = 1UL;
            in_cc_enb.use_cc_for_yuv2yuv = p_filters->input_param.use_cc_for_yuv2yuv;
            out_cc_enb.p_cr_26 = p_flow_tbl_list->p_CR_buf_26;
            out_cc_enb.cr_26_size = CR_SIZE_26;
            rval |= ikc_color_correction_enable(&in_cc_enb, &out_cc_enb);
        }
    }

    //Y2Y cc_reg, make bypass curve.
    if(is_critical_frame_enable_cc == 1U) {
        ikc_out_cc_reg_t out_cc_reg;

        if(p_filters->update_flags.cr.cr_26_update == 0U) {
            amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_color_correction_reg_y2y !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
            rval |= IK_ERR_0205;//Ring buffer Eorror
        } else {
            out_cc_reg.p_cr_26 = p_flow_tbl_list->p_CR_buf_26;
            out_cc_reg.cr_26_size = CR_SIZE_26;
            rval |= ikc_color_correction_reg_y2y(&out_cc_reg);
        }
    }

    //Y2Y cc_3d
    bool_result = equal_op_u32(p_filters->input_param.use_cc_for_yuv2yuv, 1U) + equal_op_u32(p_filters->update_flags.iso.color_correction_updated, 1U);
    if(bool_result == 2U) {
        ikc_in_cc_t in_cc_3d;
        ikc_out_cc_t out_cc_3d;

        bool_result = equal_op_u32(p_filters->update_flags.cr.cr_26_update, 0U) + equal_op_u32(p_filters->update_flags.cr.cr_27_update, 0U);
        if(bool_result != 0U) {
            amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_color_correction !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
            rval |= IK_ERR_0205;//Ring buffer Eorror
        } else {
            in_cc_3d.p_color_correction = &p_filters->input_param.color_correction;
            in_cc_3d.is_ir_only = 0u;
            out_cc_3d.p_cr_26 = p_flow_tbl_list->p_CR_buf_26;
            out_cc_3d.cr_26_size = CR_SIZE_26;
            out_cc_3d.p_cr_27 = p_flow_tbl_list->p_CR_buf_27;
            out_cc_3d.cr_27_size = CR_SIZE_27;
            rval |= ikc_color_correction(&in_cc_3d, &out_cc_3d);
        }
    }

    //Y2Y cc_out
    bool_result = equal_op_u32(p_filters->input_param.use_cc_for_yuv2yuv, 1U) + equal_op_u32(p_filters->update_flags.iso.tone_curve_updated, 1U);
    if(bool_result == 2U) {
        ikc_in_cc_out_t in_cc_out;
        ikc_out_cc_out_t out_cc_out;

        if(p_filters->update_flags.cr.cr_26_update == 0U) {
            amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_color_correction_out !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
            rval |= IK_ERR_0205;//Ring buffer Eorror
        } else {
            in_cc_out.p_tone_curve = &p_filters->input_param.tone_curve;
            out_cc_out.p_cr_26 = p_flow_tbl_list->p_CR_buf_26;
            out_cc_out.cr_26_size = CR_SIZE_26;
            rval |= ikc_color_correction_out(&in_cc_out, &out_cc_out);
        }
    }

    return rval;
}

uint32 exe_process_rgb_cr(amba_ik_flow_tables_list_t *p_flow_tbl_list, amba_ik_filter_t *p_filters)
{
    uint32 rval = IK_OK;
    uint32 window_calculate_updated = 0UL;
    uint8 bool_result;

    bool_result = equal_op_u32(p_filters->update_flags.iso.window_size_info_updated, 1U) + equal_op_u32(p_filters->update_flags.iso.dmy_range_updated, 1U) +
                  equal_op_u32(p_filters->update_flags.iso.dzoom_info_updated, 1U) + equal_op_u32(p_filters->update_flags.iso.vin_active_win_updated, 1U) +
                  equal_op_u32(p_filters->update_flags.iso.stitching_info_updated, 1U) + equal_op_u32(p_filters->update_flags.iso.flip_mode_updated, 1U) +
                  equal_op_u32(p_filters->update_flags.iso.warp_enable_updated, 1U);
    if (bool_result != 0u) {
        window_calculate_updated = 1UL;
    }

    if (window_calculate_updated==1UL) {;} //misra

    if (p_filters->input_param.ability != AMBA_IK_VIDEO_Y2Y) {
        //complexity refine.
        rval |= process_rgb_cr_non_y2y_part1(p_flow_tbl_list, p_filters);
        rval |= process_rgb_cr_non_y2y_part2(p_flow_tbl_list, p_filters);
    } else {
        //complexity refine.
        rval |= process_rgb_cr_y2y(p_flow_tbl_list, p_filters);
    }

    // pre cc gain
    if(p_filters->update_flags.iso.pre_cc_gain_updated == 1U) {
        ikc_in_pre_cc_gain_t in_pre_cc_gain;
        ikc_out_pre_cc_gain_t out_pre_cc_gain;

        if(p_filters->update_flags.cr.cr_26_update == 0U) {
            amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_pre_cc_out !!!", DC_U,DC_U,DC_U,DC_U,DC_U);
            rval |= IK_ERR_0205;//Ring buffer Eorror
        } else {
            in_pre_cc_gain.p_pre_cc_gain = &p_filters->input_param.pre_cc_gain;
            out_pre_cc_gain.p_cr_26 = p_flow_tbl_list->p_CR_buf_26;
            out_pre_cc_gain.cr_26_size = CR_SIZE_26;
            rval |= ikc_pre_cc_gain(&in_pre_cc_gain, &out_pre_cc_gain);
        }
    }

    // rgb2yuv
    bool_result = equal_op_u32(p_filters->update_flags.iso.rgb_to_yuv_matrix_updated, 1U) + equal_op_u32(p_filters->update_flags.iso.is_1st_frame, 1U) +
                  equal_op_u32(p_filters->update_flags.iso.rgb_ir_updated, 1U) + equal_op_u32(p_filters->update_flags.iso.sensor_information_updated, 1U);
    if(bool_result != 0U) {
        ikc_in_rgb2yuv_t in_rgb2yuv;
        ikc_out_rgb2yuv_t out_rgb2yuv;

        if(p_filters->update_flags.cr.cr_29_update == 0U) {
            amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_rgb2yuv !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
            rval |= IK_ERR_0205;//Ring buffer Eorror
        } else {
            in_rgb2yuv.p_rgb_to_yuv_matrix = &p_filters->input_param.rgb_to_yuv_matrix;
            if((equal_op_u32(p_filters->input_param.rgb_ir.ir_only, 1U)+equal_op_u32(p_filters->input_param.sensor_info.sensor_mode, 1U))==2U) { //ir_only, runing on RGB bayer pattern, carry IR on B channel.
                in_rgb2yuv.is_ir_only = 1u;
            } else {
                in_rgb2yuv.is_ir_only = 0u;
            }
            out_rgb2yuv.p_cr_29 = p_flow_tbl_list->p_CR_buf_29;
            out_rgb2yuv.cr_29_size = CR_SIZE_29;
            rval |= ikc_rgb2yuv(&in_rgb2yuv, &out_rgb2yuv);
        }
    }
    return rval;
}

static inline uint32 process_yuv_cr_chroma(idsp_ik_flow_ctrl_t *p_flow, amba_ik_flow_tables_list_t *p_flow_tbl_list, amba_ik_filter_t *p_filters, uint32 window_calculate_updated)
{
    uint32 rval = IK_OK;
    uint8 bool_result;
    (void)amba_ik_system_memcpy(p_flow, p_flow, sizeof(uint8));//misra fix.
    (void)amba_ik_system_memcpy(p_flow_tbl_list, p_flow_tbl_list, sizeof(uint8));//misra fix.

    // chroma scale
    if(p_filters->update_flags.iso.chroma_scale_updated == 1U) {
        ikc_in_chroma_scale_t in_chroma_scale;
        ikc_out_chroma_scale_t out_chroma_scale;

        if(p_filters->update_flags.cr.cr_29_update == 0U) {
            amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_chroma_scale !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
            rval |= IK_ERR_0205;//Ring buffer Eorror
        } else {
            if (p_filters->input_param.ability == AMBA_IK_VIDEO_Y2Y) {
                in_chroma_scale.is_yuv_mode = 1UL;
            } else {
                in_chroma_scale.is_yuv_mode = 0UL;
            }
            in_chroma_scale.p_chroma_scale = &p_filters->input_param.chroma_scale;
            out_chroma_scale.p_cr_29 = p_flow_tbl_list->p_CR_buf_29;
            out_chroma_scale.cr_29_size = CR_SIZE_29;
            rval |= ikc_chroma_scale(&in_chroma_scale, &out_chroma_scale);
        }
    }
    // chroma_median_filter
    if(p_filters->update_flags.iso.chroma_median_filter_updated == 1U) {
        ikc_in_chroma_median_t in_chroma_median;
        ikc_out_chroma_median_t out_chroma_median;

        bool_result = equal_op_u32(p_filters->update_flags.cr.cr_30_update, 0U) + equal_op_u32(p_filters->update_flags.cr.cr_31_update, 0U);
        if(bool_result != 0U) {
            amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_chroma_median !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
            rval |= IK_ERR_0205;//Ring buffer Eorror
        } else {
            in_chroma_median.is_yuv_mode = equal_op_u32(p_filters->input_param.ability, AMBA_IK_VIDEO_Y2Y);
            in_chroma_median.p_chroma_median_filter = &p_filters->input_param.chroma_median_filter;
            out_chroma_median.p_cr_30 = p_flow_tbl_list->p_CR_buf_30;
            out_chroma_median.cr_30_size = CR_SIZE_30;
            out_chroma_median.p_cr_31 = p_flow_tbl_list->p_CR_buf_31;
            out_chroma_median.cr_31_size = CR_SIZE_31;
            rval |= ikc_chroma_median(&in_chroma_median, &out_chroma_median);
        }
    }

    // chroma_noise_filter
    bool_result = equal_op_u32(p_filters->update_flags.iso.chroma_filter_updated, 1U) + equal_op_u32(window_calculate_updated, 1U);
    if(bool_result != 0U) {
        ikc_in_chroma_noise_t in_chroma_noise;
        ikc_out_chroma_noise_t out_chroma_noise;

        if(p_filters->update_flags.cr.cr_36_update == 0U) {
            amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_chroma_noise !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
            rval |= IK_ERR_0205;//Ring buffer Eorror
        } else {
            in_chroma_noise.is_group_cmd = 0u;
            in_chroma_noise.p_chroma_filter = &p_filters->input_param.chroma_filter;
            in_chroma_noise.p_wide_chroma_filter = &p_filters->input_param.wide_chroma_filter;
            in_chroma_noise.p_wide_chroma_combine = &p_filters->input_param.wide_chroma_combine;
            out_chroma_noise.p_cr_36 = p_flow_tbl_list->p_CR_buf_36;
            out_chroma_noise.cr_36_size = CR_SIZE_36;
            out_chroma_noise.p_window = &p_flow->window;
            rval |= ikc_chroma_noise(&in_chroma_noise, &out_chroma_noise);
            p_flow->window.update_chroma_radius = 1u;
        }
    } else {
        p_flow->window.update_chroma_radius = 0u;
    }

    return rval;
}

static inline uint32 process_yuv_cr_fst_shp_part1(idsp_ik_flow_ctrl_t *p_flow, amba_ik_flow_tables_list_t *p_flow_tbl_list, amba_ik_filter_t *p_filters)
{
    uint32 rval = IK_OK;
    ikc_out_sharpen_t out_sharpen;
    uint8 bool_result;

    (void)amba_ik_system_memcpy(p_flow, p_flow, sizeof(uint8));//misra fix.
    (void)amba_ik_system_memcpy(p_flow_tbl_list, p_flow_tbl_list, sizeof(uint8));//misra fix.

    out_sharpen.p_cr_30 = p_flow_tbl_list->p_CR_buf_30;
    out_sharpen.cr_30_size = CR_SIZE_30;

    bool_result = equal_op_u32(p_filters->update_flags.iso.fstshpns_noise_updated, 1U) + equal_op_u32(p_filters->update_flags.iso.fstshpns_fir_updated, 1U);
    if(bool_result != 0U) {
        ikc_in_sharpen_t in_sharpen;

        if(p_filters->update_flags.cr.cr_30_update == 0U) {
            amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_sharpen !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
            rval |= IK_ERR_0205;//Ring buffer Eorror
        } else {
            in_sharpen.sharpen_mode = p_filters->input_param.first_sharpen_both.mode;
            in_sharpen.p_first_sharpen_noise = &p_filters->input_param.first_sharpen_noise;
            in_sharpen.p_first_sharpen_fir = &p_filters->input_param.first_sharpen_fir;
            in_sharpen.p_working_buffer = &p_filters->input_param.ctx_buf.first_sharpen_working_buffer[0];
            in_sharpen.working_buffer_size = sizeof(p_filters->input_param.ctx_buf.first_sharpen_working_buffer);
            rval |= ikc_sharpen(&in_sharpen, &out_sharpen);
        }
    }

    if(p_filters->update_flags.iso.fstshpns_both_updated == 1U) {
        ikc_in_sharpen_both_t in_sharpen_both;

        if(p_filters->update_flags.cr.cr_30_update == 0U) {
            amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_sharpen_both !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
            rval |= IK_ERR_0205;//Ring buffer Eorror
        } else {
            in_sharpen_both.p_first_sharpen_both = &p_filters->input_param.first_sharpen_both;
            rval |= ikc_sharpen_both(&in_sharpen_both, &out_sharpen);
        }
    }

    if(p_filters->update_flags.iso.fstshpns_coring_updated == 1U) {
        ikc_in_sharpen_coring_t in_sharpen_coring;

        if(p_filters->update_flags.cr.cr_30_update == 0U) {
            amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_sharpen_coring !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
            rval |= IK_ERR_0205;//Ring buffer Eorror
        } else {
            in_sharpen_coring.p_first_sharpen_coring = &p_filters->input_param.first_sharpen_coring;
            rval |= ikc_sharpen_coring(&in_sharpen_coring, &out_sharpen);
        }
    }

    if(p_filters->update_flags.iso.fstshpns_coring_index_scale_updated == 1U) {
        ikc_in_sharpen_coring_idx_scale_t in_sharpen_coring_idx_scale;

        if(p_filters->update_flags.cr.cr_30_update == 0U) {
            amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_sharpen_coring_idx_scale !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
            rval |= IK_ERR_0205;//Ring buffer Eorror
        } else {
            in_sharpen_coring_idx_scale.p_first_sharpen_coring_idx_scale = &p_filters->input_param.first_sharpen_coring_idx_scale;
            rval |= ikc_sharpen_coring_idx_scale(&in_sharpen_coring_idx_scale, &out_sharpen);
        }
    }

    return rval;
}

static inline uint32 process_yuv_cr_fst_shp_part2(idsp_ik_flow_ctrl_t *p_flow, amba_ik_flow_tables_list_t *p_flow_tbl_list, amba_ik_filter_t *p_filters)
{
    uint32 rval = IK_OK;
    ikc_out_sharpen_t out_sharpen;

    (void)amba_ik_system_memcpy(p_flow, p_flow, sizeof(uint8));//misra fix.
    (void)amba_ik_system_memcpy(p_flow_tbl_list, p_flow_tbl_list, sizeof(uint8));//misra fix.

    out_sharpen.p_cr_30 = p_flow_tbl_list->p_CR_buf_30;
    out_sharpen.cr_30_size = CR_SIZE_30;

    if(p_filters->update_flags.iso.fstshpns_min_coring_result_updated == 1U) {
        ikc_in_sharpen_min_coring_t in_sharpen_min_coring;

        if(p_filters->update_flags.cr.cr_30_update == 0U) {
            amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_sharpen_min_coring !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
            rval |= IK_ERR_0205;//Ring buffer Eorror
        } else {
            in_sharpen_min_coring.p_first_sharpen_min_coring_result = &p_filters->input_param.first_sharpen_min_coring_result;
            rval |= ikc_sharpen_min_coring(&in_sharpen_min_coring, &out_sharpen);
        }
    }

    if(p_filters->update_flags.iso.fstshpns_max_coring_result_updated == 1U) {
        ikc_in_sharpen_max_coring_t in_sharpen_max_coring;

        if(p_filters->update_flags.cr.cr_30_update == 0U) {
            amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_sharpen_max_coring !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
            rval |= IK_ERR_0205;//Ring buffer Eorror
        } else {
            in_sharpen_max_coring.p_first_sharpen_max_coring_result = &p_filters->input_param.first_sharpen_max_coring_result;
            rval |= ikc_sharpen_max_coring(&in_sharpen_max_coring, &out_sharpen);
        }
    }

    if(p_filters->update_flags.iso.fstshpns_scale_coring_updated == 1U) {
        ikc_in_sharpen_scale_coring_t in_sharpen_scale_coring;

        if(p_filters->update_flags.cr.cr_30_update == 0U) {
            amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_sharpen_scale_coring !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
            rval |= IK_ERR_0205;//Ring buffer Eorror
        } else {
            in_sharpen_scale_coring.p_first_sharpen_scale_coring = &p_filters->input_param.first_sharpen_scale_coring;
            rval |= ikc_sharpen_scale_coring(&in_sharpen_scale_coring, &out_sharpen);
        }
    }

    return rval;
}

static inline uint32 process_yuv_cr_lnl(idsp_ik_flow_ctrl_t *p_flow, amba_ik_flow_tables_list_t *p_flow_tbl_list, amba_ik_filter_t *p_filters, uint32 window_calculate_updated)
{
    uint32 rval = IK_OK;
    uint8 bool_result;

    (void)amba_ik_system_memcpy(p_flow, p_flow, sizeof(uint8));//misra fix.
    (void)amba_ik_system_memcpy(p_flow_tbl_list, p_flow_tbl_list, sizeof(uint8));//misra fix.
    (void)amba_ik_system_memcpy(p_filters, p_filters, sizeof(uint8));//misra fix.

    // LNL
    bool_result = lnl_op_u32(p_filters->update_flags.iso.after_ce_wb_gain_updated, p_filters->input_param.ability) +
                  lnl_op_u32(p_filters->update_flags.iso.rgb_to_12y_updated, p_filters->input_param.ability) +
                  equal_op_u32(p_filters->update_flags.iso.lnl_updated, 1U) + equal_op_u32(window_calculate_updated, 1UL);
    if(bool_result != 0u) {
        ikc_in_lnl_t in_lnl;
        ikc_out_lnl_t out_lnl;
        ik_luma_noise_reduction_t lnl_disable;

        if(p_filters->update_flags.cr.cr_30_update == 0U) {
            amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_lnl !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
            rval |= IK_ERR_0205;//Ring buffer Eorror
        } else {
            if((p_filters->input_param.use_cc_for_yuv2yuv == 0U) && (p_filters->input_param.ability == AMBA_IK_VIDEO_Y2Y)) {
                //Y2Y pipeline, if CCs not enabled then LNL must be disabled.
                lnl_disable.enable = 0;
                lnl_disable.sensor_wb_b = 0;
                lnl_disable.sensor_wb_g = 0;
                lnl_disable.sensor_wb_r = 0;
                lnl_disable.strength0 = 100UL;
                lnl_disable.strength1 = 128UL;
                lnl_disable.strength2 = 0;
                lnl_disable.strength2_max_change = 0;
                in_lnl.p_luma_noise_reduce = &lnl_disable;
            } else {
                in_lnl.p_luma_noise_reduce = &p_filters->input_param.luma_noise_reduce;
            }
            in_lnl.p_after_ce_wb_gain = &p_filters->input_param.after_ce_wb_gain;
            in_lnl.p_rgb_to_12y = &p_filters->input_param.rgb_to_12y;
            in_lnl.p_cfa_win = &p_filters->input_param.ctx_buf.result_win.cfa_win_dim;
            out_lnl.p_cr_30 = p_flow_tbl_list->p_CR_buf_30;
            out_lnl.cr_30_size = CR_SIZE_30;
            out_lnl.p_flow_info = &p_flow->flow_info;
            rval |= ikc_lnl(&in_lnl, &out_lnl);
        }
    }
    // LNL table
    bool_result = lnl_table_op_u32_1(p_filters->update_flags.iso.color_correction_updated, p_filters->update_flags.iso.tone_curve_updated) +
                  lnl_table_op_u32_2(p_filters->input_param.use_cc_for_yuv2yuv, p_filters->input_param.ability);
    if(bool_result == 2u) {
        ikc_in_lnl_tbl_t in_lnl_tbl;
        ikc_out_lnl_t out_lnl;

        if(p_filters->update_flags.cr.cr_30_update == 0U) {
            amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_lnl_tbl !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
            rval |= IK_ERR_0205;//Ring buffer Eorror
        } else {
            in_lnl_tbl.p_color_correction = &p_filters->input_param.color_correction;
            in_lnl_tbl.p_tone_curve = &p_filters->input_param.tone_curve;
            out_lnl.p_cr_30 = p_flow_tbl_list->p_CR_buf_30;
            out_lnl.cr_30_size = CR_SIZE_30;
            rval |= ikc_lnl_tbl(&in_lnl_tbl, &out_lnl);
        }
    }

    return rval;
}

static uint32 exe_check_burst_tile_mode(const amba_ik_filter_t *p_filters)
{
    uint32 rval = IK_OK;
    const ik_burst_tile_t *p_burst_tile = &p_filters->input_param.burst_tile;
    const ik_warp_buffer_info_t *p_warp_buf_info = &p_filters->input_param.warp_buf_info;

    if((equal_op_u32(p_burst_tile->enable, 1U)+equal_op_u32(p_warp_buf_info->dram_efficiency, 2U))==2U) {
        amba_ik_system_print_uint32_5("[IK][ERROR] p_burst_tile->enable %d + p_warp_buf_info->dram_efficiency %d !, smem to warp not supports burst tile mode", \
                                      p_burst_tile->enable, p_warp_buf_info->dram_efficiency, DC_U, DC_U, DC_U);
        rval |= IK_ERR_0008;
    }

    return rval;
}

uint32 exe_process_yuv_cr(idsp_ik_flow_ctrl_t *p_flow, amba_ik_flow_tables_list_t *p_flow_tbl_list, amba_ik_filter_t *p_filters)
{
    uint32 rval = IK_OK;
    uint32 window_calculate_updated = 0UL;
    uint8 bool_result;

    bool_result = equal_op_u32(p_filters->update_flags.iso.window_size_info_updated, 1U) + equal_op_u32(p_filters->update_flags.iso.dmy_range_updated, 1U) +
                  equal_op_u32(p_filters->update_flags.iso.dzoom_info_updated, 1U) + equal_op_u32(p_filters->update_flags.iso.vin_active_win_updated, 1U) +
                  equal_op_u32(p_filters->update_flags.iso.stitching_info_updated, 1U) + equal_op_u32(p_filters->update_flags.iso.flip_mode_updated, 1U) +
                  equal_op_u32(p_filters->update_flags.iso.warp_enable_updated, 1U);
    if (bool_result != 0u) {
        window_calculate_updated = 1UL;
    }

    //complexity refine.
    //uv filters.
    rval |= process_yuv_cr_chroma(p_flow, p_flow_tbl_list, p_filters, window_calculate_updated);

    // luma_processing
    if(p_filters->input_param.first_luma_process_mode.use_sharpen_not_asf == 0U) {//asf
        ikc_out_asf_t out_asf;
        out_asf.p_cr_30 = p_flow_tbl_list->p_CR_buf_30;
        out_asf.cr_30_size = CR_SIZE_30;
        if(p_filters->update_flags.iso.advance_spatial_filter_updated == 1U) {
            ikc_in_asf_t in_asf;

            if(p_filters->update_flags.cr.cr_30_update == 0U) {
                amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_asf !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
                rval |= IK_ERR_0205;//Ring buffer Eorror
            } else {
                in_asf.p_advance_spatial_filter = &p_filters->input_param.advance_spatial_filter;
                in_asf.first_luma_processing_mode_updated = (uint32)p_filters->update_flags.iso.first_luma_processing_mode_updated;
                rval |= ikc_asf(&in_asf, &out_asf);
            }
        }
    } else { //shpA
        //complexity refine.
        rval |= process_yuv_cr_fst_shp_part1(p_flow, p_flow_tbl_list, p_filters);
        rval |= process_yuv_cr_fst_shp_part2(p_flow, p_flow_tbl_list, p_filters);
    }

    //LNL
    //complexity refine.
    rval |= process_yuv_cr_lnl(p_flow, p_flow_tbl_list, p_filters,window_calculate_updated);

    bool_result = equal_op_u32(p_filters->update_flags.iso.chroma_filter_updated, 1U) + equal_op_u32(p_filters->update_flags.iso.resample_str_update, 1U) +
                  equal_op_u32(p_filters->update_flags.iso.window_size_info_updated, 1U) + equal_op_u32(window_calculate_updated, 1UL) +
                  equal_op_u32(p_filters->update_flags.iso.burst_tile_updated, 1U);
    if(bool_result != 0u) {
        ikc_in_main_resamp_t in_main_resamp;
        ikc_out_main_resamp_t out_main_resamp;

        bool_result = equal_op_u32(p_filters->update_flags.cr.cr_33_update, 0U) + equal_op_u32(p_filters->update_flags.cr.cr_35_update, 0U) +
                      equal_op_u32(p_filters->update_flags.cr.cr_37_update, 0U) + equal_op_u32(p_filters->update_flags.cr.cr_42_update, 0U);
        if(bool_result != 0u) {
            amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_main_resampler !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
            rval |= IK_ERR_0205;//Ring buffer Eorror
        } else {
            in_main_resamp.main_cut_off_freq = p_filters->input_param.resample_str.main_cut_off_freq;
            in_main_resamp.p_act_win_crop = &p_filters->input_param.ctx_buf.result_win.act_win_crop;
            in_main_resamp.p_dmy_win_geo = &p_filters->input_param.ctx_buf.result_win.dmy_win_geo;
            in_main_resamp.p_cfa_win_dim = &p_filters->input_param.ctx_buf.result_win.cfa_win_dim;
            in_main_resamp.p_main = &p_filters->input_param.window_size_info.main_win;
            in_main_resamp.p_dmy_range = &p_filters->input_param.dmy_range;
            in_main_resamp.chroma_radius = p_filters->input_param.chroma_filter.radius;
            in_main_resamp.is_y2y_420input = (p_filters->input_param.yuv_mode == AMBA_IK_YUV420) ? 1UL : 0UL;
            in_main_resamp.flip_mode = p_filters->input_param.flip_mode;
            in_main_resamp.p_burst_tile = &p_filters->input_param.burst_tile;
            in_main_resamp.force_vscale_sec_n = 0U;
            out_main_resamp.p_cr_33 = p_flow_tbl_list->p_CR_buf_33;
            out_main_resamp.cr_33_size = CR_SIZE_33;
            out_main_resamp.p_cr_35 = p_flow_tbl_list->p_CR_buf_35;
            out_main_resamp.cr_35_size = CR_SIZE_35;
            out_main_resamp.p_cr_37 = p_flow_tbl_list->p_CR_buf_37;
            out_main_resamp.cr_37_size = CR_SIZE_37;
            out_main_resamp.p_cr_42 = p_flow_tbl_list->p_CR_buf_42;
            out_main_resamp.cr_42_size = CR_SIZE_42;
            rval |= ikc_main_resampler(&in_main_resamp, &out_main_resamp);
        }
    }

    rval |= exe_check_burst_tile_mode(p_filters);
    return rval;
}

static inline uint32 process_hdr_ce_sub_cr_patr1_a(amba_ik_flow_tables_list_t *p_flow_tbl_list, amba_ik_filter_t *p_filters, uint32 exp_num, uint32 window_calculate_updated)
{
    uint32 rval = IK_OK;
    uint8 bool_result;
    (void)amba_ik_system_memcpy(p_flow_tbl_list, p_flow_tbl_list, sizeof(uint8));//misra fix.
    if (window_calculate_updated == 1UL) {;}

    // hdr exp sub
    bool_result = equal_op_u32(p_filters->update_flags.iso.exp0_fe_static_blc_updated, 1u) + equal_op_u32(p_filters->update_flags.iso.exp1_fe_static_blc_updated, 1u) +
                  equal_op_u32(p_filters->update_flags.iso.exp2_fe_static_blc_updated, 1u) + equal_op_u32(p_filters->update_flags.iso.sensor_information_updated, 1u) +
                  equal_op_u32(p_filters->update_flags.iso.rgb_ir_updated, 1u);
    if(bool_result != 0u) {
        ikc_in_hdr_blc_t in_hdr_blc;
        ikc_out_hdr_blc_sub_t out_hdr_blc_sub;
        int32 compression_offset = ((greater_op_u32(p_filters->input_param.sensor_info.compression, IK_RAW_COMPRESS_OFF)+less_equal_op_u32(p_filters->input_param.sensor_info.compression, IK_RAW_COMPRESS_10p5))==2U) ?
                                   p_filters->input_param.ctx_buf.first_compression_offset : 0;

        if(p_filters->update_flags.cr.cr_45_update == 0U) {
            amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_hdr_black_level_sub !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
            rval |= IK_ERR_0205;//Ring buffer Eorror
        } else {
            in_hdr_blc.exp_num = exp_num;
            in_hdr_blc.rgb_ir_mode = p_filters->input_param.rgb_ir.mode;
            in_hdr_blc.compression_offset = compression_offset;
            (void)amba_ik_system_memcpy(&p_filters->input_param.ctx_buf.ir_only_trick_buffer.exp0_frontend_static_blc, &p_filters->input_param.exp0_frontend_static_blc, sizeof(ik_static_blc_level_t));
            (void)amba_ik_system_memcpy(&p_filters->input_param.ctx_buf.ir_only_trick_buffer.exp1_frontend_static_blc, &p_filters->input_param.exp1_frontend_static_blc, sizeof(ik_static_blc_level_t));
            (void)amba_ik_system_memcpy(&p_filters->input_param.ctx_buf.ir_only_trick_buffer.exp2_frontend_static_blc, &p_filters->input_param.exp2_frontend_static_blc, sizeof(ik_static_blc_level_t));
            in_hdr_blc.p_exp0_frontend_static_blc = &p_filters->input_param.ctx_buf.ir_only_trick_buffer.exp0_frontend_static_blc;
            in_hdr_blc.p_exp1_frontend_static_blc = &p_filters->input_param.ctx_buf.ir_only_trick_buffer.exp1_frontend_static_blc;
            in_hdr_blc.p_exp2_frontend_static_blc = &p_filters->input_param.ctx_buf.ir_only_trick_buffer.exp2_frontend_static_blc;
            out_hdr_blc_sub.p_cr_45 = p_flow_tbl_list->p_CR_buf_45;
            out_hdr_blc_sub.cr_45_size = CR_SIZE_45;
            if((equal_op_u32(p_filters->input_param.rgb_ir.ir_only, 1U)+equal_op_u32(p_filters->input_param.sensor_info.sensor_mode, 1U))==2U) { //ir_only, runing on RGB bayer pattern, carry IR on B channel.
                in_hdr_blc.rgb_ir_mode = 0;
                in_hdr_blc.p_exp0_frontend_static_blc->black_r = -16383;
                in_hdr_blc.p_exp1_frontend_static_blc->black_r = -16383;
                in_hdr_blc.p_exp2_frontend_static_blc->black_r = -16383;
                in_hdr_blc.p_exp0_frontend_static_blc->black_b = in_hdr_blc.p_exp0_frontend_static_blc->black_ir;
                in_hdr_blc.p_exp1_frontend_static_blc->black_b = in_hdr_blc.p_exp1_frontend_static_blc->black_ir;
                in_hdr_blc.p_exp2_frontend_static_blc->black_b = in_hdr_blc.p_exp2_frontend_static_blc->black_ir;
            }
            rval |= ikc_hdr_black_level_sub(&in_hdr_blc, &out_hdr_blc_sub);
        }
    }

    return rval;
}

static inline uint32 process_hdr_ce_sub_cr_patr1_b(amba_ik_flow_tables_list_t *p_flow_tbl_list, amba_ik_filter_t *p_filters, uint32 exp_num, uint32 window_calculate_updated)
{
    uint32 rval = IK_OK;
    uint8 bool_result;
    (void)amba_ik_system_memcpy(p_flow_tbl_list, p_flow_tbl_list, sizeof(uint8));//misra fix.
    if (window_calculate_updated == 1UL) {;}

    // hdr exp sub
    bool_result = equal_op_u32(p_filters->update_flags.iso.exp0_fe_wb_gain_updated, 1u) + equal_op_u32(p_filters->update_flags.iso.exp1_fe_wb_gain_updated, 1u) +
                  equal_op_u32(p_filters->update_flags.iso.exp2_fe_wb_gain_updated, 1u) + equal_op_u32(p_filters->update_flags.iso.exp0_fe_static_blc_updated, 1u) +
                  equal_op_u32(p_filters->update_flags.iso.exp1_fe_static_blc_updated, 1u) + equal_op_u32(p_filters->update_flags.iso.exp2_fe_static_blc_updated, 1u) +
                  equal_op_u32(p_filters->update_flags.iso.sensor_information_updated, 1u) + equal_op_u32(p_filters->update_flags.iso.rgb_ir_updated, 1u);
    if(bool_result != 0u) {
        ikc_in_hdr_dgain_t in_hdr_dgain;
        ikc_out_hdr_dgain_sub_t out_hdr_dgain_sub;

        if(p_filters->update_flags.cr.cr_45_update == 0U) {
            amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_hdr_dgain_sub !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
            rval |= IK_ERR_0205;//Ring buffer Eorror
        } else {
            in_hdr_dgain.exp_num = exp_num;
            in_hdr_dgain.rgb_ir_mode = p_filters->input_param.rgb_ir.mode;
            (void)amba_ik_system_memcpy(&p_filters->input_param.ctx_buf.ir_only_trick_buffer.exp0_frontend_wb_gain, &p_filters->input_param.exp0_frontend_wb_gain, sizeof(ik_frontend_wb_gain_t));
            (void)amba_ik_system_memcpy(&p_filters->input_param.ctx_buf.ir_only_trick_buffer.exp1_frontend_wb_gain, &p_filters->input_param.exp1_frontend_wb_gain, sizeof(ik_frontend_wb_gain_t));
            (void)amba_ik_system_memcpy(&p_filters->input_param.ctx_buf.ir_only_trick_buffer.exp2_frontend_wb_gain, &p_filters->input_param.exp2_frontend_wb_gain, sizeof(ik_frontend_wb_gain_t));
            in_hdr_dgain.p_exp0_frontend_wb_gain = &p_filters->input_param.ctx_buf.ir_only_trick_buffer.exp0_frontend_wb_gain;
            in_hdr_dgain.p_exp1_frontend_wb_gain = &p_filters->input_param.ctx_buf.ir_only_trick_buffer.exp1_frontend_wb_gain;
            in_hdr_dgain.p_exp2_frontend_wb_gain = &p_filters->input_param.ctx_buf.ir_only_trick_buffer.exp2_frontend_wb_gain;
            in_hdr_dgain.p_exp0_frontend_static_blc = &p_filters->input_param.exp0_frontend_static_blc;
            in_hdr_dgain.p_exp1_frontend_static_blc = &p_filters->input_param.exp1_frontend_static_blc;
            in_hdr_dgain.p_exp2_frontend_static_blc = &p_filters->input_param.exp2_frontend_static_blc;
            out_hdr_dgain_sub.p_cr_45 = p_flow_tbl_list->p_CR_buf_45;
            out_hdr_dgain_sub.cr_45_size = CR_SIZE_45;
            if((equal_op_u32(p_filters->input_param.rgb_ir.ir_only, 1U)+equal_op_u32(p_filters->input_param.sensor_info.sensor_mode, 1U))==2U) { //ir_only, runing on RGB bayer pattern, carry IR on B channel.
                in_hdr_dgain.rgb_ir_mode = 0;
                in_hdr_dgain.p_exp0_frontend_wb_gain->b_gain = in_hdr_dgain.p_exp0_frontend_wb_gain->ir_gain;
                in_hdr_dgain.p_exp1_frontend_wb_gain->b_gain = in_hdr_dgain.p_exp1_frontend_wb_gain->ir_gain;
                in_hdr_dgain.p_exp2_frontend_wb_gain->b_gain = in_hdr_dgain.p_exp2_frontend_wb_gain->ir_gain;
            }
            rval |= ikc_hdr_dgain_sub(&in_hdr_dgain, &out_hdr_dgain_sub);
        }
    }

    return rval;
}

static inline uint32 process_hdr_ce_sub_cr_patr1(amba_ik_flow_tables_list_t *p_flow_tbl_list, amba_ik_filter_t *p_filters, uint32 exp_num, uint32 window_calculate_updated)
{
    uint32 rval = IK_OK;
    uint8 bool_result;
    (void)amba_ik_system_memcpy(p_flow_tbl_list, p_flow_tbl_list, sizeof(uint8));//misra fix.

    // hdr_blend sub and hdr_alpha sub
    if(p_filters->update_flags.iso.hdr_blend_updated == 1u) {
        ikc_in_hdr_blend_t in_hdr_blend;
        ikc_out_hdr_blend_sub_t out_hdr_blend_sub;

        if(p_filters->update_flags.cr.cr_45_update == 0U) {
            amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_hdr_blend_sub !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
            rval |= IK_ERR_0205;//Ring buffer Eorror
        } else {
            in_hdr_blend.exp_num = exp_num;
            in_hdr_blend.p_hdr_raw_info = &p_filters->input_param.hdr_raw_info;
            in_hdr_blend.p_hdr_blend = &p_filters->input_param.hdr_blend;
            out_hdr_blend_sub.p_cr_45 = p_flow_tbl_list->p_CR_buf_45;
            out_hdr_blend_sub.cr_45_size = CR_SIZE_45;
            rval |= ikc_hdr_blend_sub(&in_hdr_blend, &out_hdr_blend_sub);
        }
    }

    // hdr_tone_curve sub
    if(p_filters->update_flags.iso.fe_tone_curve_updated == 1u) {
        ikc_in_hdr_tone_curve_t in_hdr_tc;
        ikc_out_hdr_tone_curve_sub_t out_hdr_tc_sub;

        bool_result = equal_op_u32(p_filters->update_flags.cr.cr_45_update, 0U) + equal_op_u32(p_filters->update_flags.cr.cr_46_update, 0U);
        if(bool_result != 0U) {
            amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_front_end_tone_curve_sub !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
            rval |= IK_ERR_0205;//Ring buffer Eorror
        } else {
            in_hdr_tc.p_fe_tone_curve = &p_filters->input_param.fe_tone_curve;
            out_hdr_tc_sub.p_cr_45 = p_flow_tbl_list->p_CR_buf_45;
            out_hdr_tc_sub.cr_45_size = CR_SIZE_45;
            out_hdr_tc_sub.p_cr_46 = p_flow_tbl_list->p_CR_buf_46;
            out_hdr_tc_sub.cr_46_size = CR_SIZE_46;
            rval |= ikc_front_end_tone_curve_sub(&in_hdr_tc, &out_hdr_tc_sub);
        }
    }

    // losy decompress sub
    bool_result = equal_op_u32(p_filters->update_flags.iso.sensor_information_updated, 1U) + equal_op_u32(window_calculate_updated, 1U);
    if(bool_result != 0U) {
        ikc_in_decompress_t in_decompress;
        ikc_out_decompress_sub_t out_decompress_sub;

        if(p_filters->update_flags.cr.cr_45_update == 0U) {
            amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_decompression_sub !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
            rval |= IK_ERR_0205;//Ring buffer Eorror
        } else {
            in_decompress.exp_num = exp_num;
            in_decompress.compress_mode = p_filters->input_param.sensor_info.compression;//0~8, 256~259.
            in_decompress.p_dmy_win_geo = &p_filters->input_param.ctx_buf.result_win.dmy_win_geo;
            out_decompress_sub.p_cr_45 = p_flow_tbl_list->p_CR_buf_45;
            out_decompress_sub.cr_45_size = CR_SIZE_45;
            rval |= ikc_decompression_sub(&in_decompress, &out_decompress_sub);
        }
    }
    //complexity refine.
    rval |= process_hdr_ce_sub_cr_patr1_a(p_flow_tbl_list, p_filters, exp_num, window_calculate_updated);
    rval |= process_hdr_ce_sub_cr_patr1_b(p_flow_tbl_list, p_filters, exp_num, window_calculate_updated);

    return rval;
}

static inline uint32 process_hdr_ce_sub_cr_patr2_a(amba_ik_flow_tables_list_t *p_flow_tbl_list, amba_ik_filter_t *p_filters, uint32 exp_num, uint32 window_calculate_updated)
{
    uint32 rval = IK_OK;
    uint8 bool_result;
    (void)amba_ik_system_memcpy(p_flow_tbl_list, p_flow_tbl_list, sizeof(uint8));//misra fix.

    if (exp_num == 0UL) {;} //misra fix.
    if (window_calculate_updated==1UL) {;} //misra

    // rgbir sub
    bool_result = equal_op_u32(p_filters->update_flags.iso.rgb_ir_updated, 1u) + equal_op_u32(p_filters->update_flags.iso.sensor_information_updated, 1u);
    if(bool_result != 0U) {
        ikc_in_rgb_ir_t in_rgb_ir;
        ikc_out_rgb_ir_sub_t out_rgb_ir_sub;

        bool_result = equal_op_u32(p_filters->update_flags.cr.cr_45_update, 0u) + equal_op_u32(p_filters->update_flags.cr.cr_47_update, 0u) +
                      equal_op_u32(p_filters->update_flags.cr.cr_49_update, 0u) + equal_op_u32(p_filters->update_flags.cr.cr_50_update, 0u);
        if(bool_result != 0U) {
            amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_rgb_ir_sub !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
            rval |= IK_ERR_0205;//Ring buffer Eorror
        } else {
            in_rgb_ir.flip_mode = p_filters->input_param.flip_mode;
            (void)amba_ik_system_memcpy(&p_filters->input_param.ctx_buf.ir_only_trick_buffer.rgb_ir, &p_filters->input_param.rgb_ir, sizeof(ik_rgb_ir_t));
            in_rgb_ir.p_rgb_ir = &p_filters->input_param.ctx_buf.ir_only_trick_buffer.rgb_ir;
            in_rgb_ir.p_before_ce_wb_gain = &p_filters->input_param.before_ce_wb_gain;
            in_rgb_ir.p_after_ce_wb_gain = &p_filters->input_param.after_ce_wb_gain;
            (void)amba_ik_system_memcpy(&p_filters->input_param.ctx_buf.ir_only_trick_buffer.exp0_frontend_wb_gain, &p_filters->input_param.exp0_frontend_wb_gain, sizeof(ik_frontend_wb_gain_t));
            in_rgb_ir.p_exp0_frontend_wb_gain = &p_filters->input_param.ctx_buf.ir_only_trick_buffer.exp0_frontend_wb_gain;
            out_rgb_ir_sub.p_cr_45 = p_flow_tbl_list->p_CR_buf_45;
            out_rgb_ir_sub.cr_45_size = CR_SIZE_45;
            out_rgb_ir_sub.p_cr_47 = p_flow_tbl_list->p_CR_buf_47;
            out_rgb_ir_sub.cr_47_size = CR_SIZE_47;
            out_rgb_ir_sub.p_cr_49 = p_flow_tbl_list->p_CR_buf_49;
            out_rgb_ir_sub.cr_49_size = CR_SIZE_49;
            out_rgb_ir_sub.p_cr_50 = p_flow_tbl_list->p_CR_buf_50;
            out_rgb_ir_sub.cr_50_size = CR_SIZE_50;
            if((equal_op_u32(p_filters->input_param.rgb_ir.ir_only, 1U)+equal_op_u32(p_filters->input_param.sensor_info.sensor_mode, 1U))==2U) { //ir_only, runing on RGB bayer pattern, carry IR on B channel.
                in_rgb_ir.p_rgb_ir->mode = 0;
                in_rgb_ir.p_rgb_ir->ir_only = 1u;
                in_rgb_ir.p_exp0_frontend_wb_gain->b_gain = in_rgb_ir.p_exp0_frontend_wb_gain->ir_gain;
            } else {
                in_rgb_ir.p_rgb_ir->ir_only = 0u;
            }
            rval |= ikc_rgb_ir_sub(&in_rgb_ir, &out_rgb_ir_sub);
        }
    }

    return rval;
}

static inline uint32 process_hdr_ce_sub_cr_patr2(amba_ik_flow_tables_list_t *p_flow_tbl_list, amba_ik_filter_t *p_filters, uint32 exp_num, uint32 window_calculate_updated)
{
    uint32 rval = IK_OK;
    uint8 bool_result;
    (void)amba_ik_system_memcpy(p_flow_tbl_list, p_flow_tbl_list, sizeof(uint8));//misra fix.

    if (exp_num == 0UL) {;} //misra fix.

    bool_result = equal_op_u32(p_filters->update_flags.iso.ce_updated, 1u) + equal_op_u32(window_calculate_updated, 1u);
    if(bool_result != 0U) {
        ikc_in_ce_t in_ce;
        ikc_out_ce_sub_t out_ce_sub;

        bool_result = equal_op_u32(p_filters->update_flags.cr.cr_49_update, 0u) + equal_op_u32(p_filters->update_flags.cr.cr_50_update, 0u)+
                      equal_op_u32(p_filters->update_flags.cr.cr_51_update, 0u);
        if(bool_result != 0U) {
            amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_contrast_enhancement_sub !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
            rval |= IK_ERR_0205;//Ring buffer Eorror
        } else {
            in_ce.sensor_pattern = p_filters->input_param.sensor_info.sensor_pattern;
            in_ce.p_dmy_win_geo = &p_filters->input_param.ctx_buf.result_win.dmy_win_geo;
            in_ce.p_ce = &p_filters->input_param.ce;
            out_ce_sub.p_cr_49 = p_flow_tbl_list->p_CR_buf_49;
            out_ce_sub.cr_49_size = CR_SIZE_49;
            out_ce_sub.p_cr_50 = p_flow_tbl_list->p_CR_buf_50;
            out_ce_sub.cr_50_size = CR_SIZE_50;
            out_ce_sub.p_cr_51 = p_flow_tbl_list->p_CR_buf_51;
            out_ce_sub.cr_51_size = CR_SIZE_51;
            rval |= ikc_contrast_enhancement_sub(&in_ce, &out_ce_sub);
        }
    }
    if(p_filters->update_flags.iso.ce_input_table_updated == 1u) {
        ikc_in_ce_input_sub_t in_ce_input_sub;
        ikc_out_ce_input_sub_t out_ce_input_sub;

        bool_result = equal_op_u32(p_filters->update_flags.cr.cr_51_update, 0u) + equal_op_u32(p_filters->update_flags.cr.cr_52_update, 0u);
        if(bool_result != 0U) {
            amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_contrast_enhancement_input_sub !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
            rval |= IK_ERR_0205;//Ring buffer Eorror
        } else {
            in_ce_input_sub.ce_enable = p_filters->input_param.ce.enable;
            in_ce_input_sub.radius = p_filters->input_param.ce.radius;
            in_ce_input_sub.epsilon = p_filters->input_param.ce.epsilon;
            in_ce_input_sub.p_ce_input_table = &p_filters->input_param.ce_input_table;
            out_ce_input_sub.p_cr_51 = p_flow_tbl_list->p_CR_buf_51;
            out_ce_input_sub.cr_51_size = CR_SIZE_51;
            out_ce_input_sub.p_cr_52 = p_flow_tbl_list->p_CR_buf_52;
            out_ce_input_sub.cr_52_size = CR_SIZE_52;
            rval |= ikc_contrast_enhancement_input_sub(&in_ce_input_sub, &out_ce_input_sub);
        }
    }
    // vig sub
    bool_result = equal_op_u32(p_filters->update_flags.iso.vignette_compensation_updated, 1u) + equal_op_u32(p_filters->update_flags.iso.window_size_info_updated, 1u) +
                  equal_op_u32(window_calculate_updated, 1u);
    if (bool_result != 0U) {
        ikc_in_vignette_t in_vig;
        ikc_out_vignette_sub_t out_vig_sub;

        bool_result = equal_op_u32(p_filters->update_flags.cr.cr_47_update, 0u) + equal_op_u32(p_filters->update_flags.cr.cr_48_update, 0u) +
                      equal_op_u32(p_filters->update_flags.cr.cr_49_update, 0u);
        if(bool_result != 0U) {
            amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_vignette_sub !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
            rval |= IK_ERR_0205;//Ring buffer Eorror
        } else {
            in_vig.vig_enable = p_filters->input_param.vig_enable;
            in_vig.flip_mode = p_filters->input_param.flip_mode;
            in_vig.p_vignette_compensation = &p_filters->input_param.vignette_compensation;
            in_vig.p_vin_sensor = &p_filters->input_param.window_size_info.vin_sensor;
            in_vig.p_dmy_win_geo = &p_filters->input_param.ctx_buf.result_win.dmy_win_geo;
            out_vig_sub.p_cr_47 = p_flow_tbl_list->p_CR_buf_47;
            out_vig_sub.cr_47_size = CR_SIZE_47;
            out_vig_sub.p_cr_48 = p_flow_tbl_list->p_CR_buf_48;
            out_vig_sub.cr_48_size = CR_SIZE_48;
            out_vig_sub.p_cr_49 = p_flow_tbl_list->p_CR_buf_49;
            out_vig_sub.cr_49_size = CR_SIZE_49;
            rval |= ikc_vignette_sub(&in_vig, &out_vig_sub);
        }
    }
    //complexity refine.
    rval |= process_hdr_ce_sub_cr_patr2_a(p_flow_tbl_list, p_filters, exp_num, window_calculate_updated);

    return rval;
}

uint32 exe_process_hdr_ce_sub_cr(amba_ik_flow_tables_list_t *p_flow_tbl_list, amba_ik_filter_t *p_filters, uint32 exp_num)
{
    uint32 rval = IK_OK;
    uint32 window_calculate_updated = 0UL;
    uint8 bool_result;

    bool_result = equal_op_u32(p_filters->update_flags.iso.window_size_info_updated, 1U) +
                  equal_op_u32(p_filters->update_flags.iso.dmy_range_updated, 1U) +
                  equal_op_u32(p_filters->update_flags.iso.dzoom_info_updated, 1U) +
                  equal_op_u32(p_filters->update_flags.iso.vin_active_win_updated, 1U) +
                  equal_op_u32(p_filters->update_flags.iso.stitching_info_updated, 1U) +
                  equal_op_u32(p_filters->update_flags.iso.flip_mode_updated, 1U) +
                  equal_op_u32(p_filters->update_flags.iso.warp_enable_updated, 1U);
    if (bool_result != 0u) {
        window_calculate_updated = 1UL;
    }

    if (p_filters->input_param.ability != AMBA_IK_VIDEO_Y2Y) {
        // input mode sub
        bool_result = equal_op_u32(p_filters->update_flags.iso.is_1st_frame, 1U) + equal_op_u32(p_filters->update_flags.iso.sensor_information_updated, 1U);
        if(bool_result == 2U) { //should only configure once at 1st time.
            ikc_in_input_mode_sub_t in_input_mode_sub;
            ikc_out_input_mode_sub_t out_input_mode_sub;

            bool_result = equal_op_u32(p_filters->update_flags.cr.cr_45_update, 0u) + equal_op_u32(p_filters->update_flags.cr.cr_47_update, 0u)+
                          equal_op_u32(p_filters->update_flags.cr.cr_49_update, 0u) + equal_op_u32(p_filters->update_flags.cr.cr_50_update, 0u)+
                          equal_op_u32(p_filters->update_flags.cr.cr_51_update, 0u);
            if(bool_result != 0U) {
                amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_input_mode_sub !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
                rval |= IK_ERR_0205;//Ring buffer Eorror
            } else {
                in_input_mode_sub.flip_mode = p_filters->input_param.flip_mode;
                in_input_mode_sub.is_yuv_mode = 0u;
                in_input_mode_sub.sensor_pattern = p_filters->input_param.sensor_info.sensor_pattern;
                out_input_mode_sub.p_cr_45 = p_flow_tbl_list->p_CR_buf_45;
                out_input_mode_sub.cr_45_size = CR_SIZE_45;
                out_input_mode_sub.p_cr_47 = p_flow_tbl_list->p_CR_buf_47;
                out_input_mode_sub.cr_47_size = CR_SIZE_47;
                out_input_mode_sub.p_cr_49 = p_flow_tbl_list->p_CR_buf_49;
                out_input_mode_sub.cr_49_size = CR_SIZE_49;
                out_input_mode_sub.p_cr_50 = p_flow_tbl_list->p_CR_buf_50;
                out_input_mode_sub.cr_50_size = CR_SIZE_50;
                out_input_mode_sub.p_cr_51 = p_flow_tbl_list->p_CR_buf_51;
                out_input_mode_sub.cr_51_size = CR_SIZE_51;

                rval |= ikc_input_mode_sub(&in_input_mode_sub, &out_input_mode_sub);
            }
        }

        // before_ce_gain sub
        if(p_filters->update_flags.iso.before_ce_wb_gain_updated == 1u) {
            ikc_in_before_ce_gain_t in_before_ce_gain;
            ikc_out_before_ce_gain_sub_t out_before_ce_gain_sub;

            if(p_filters->update_flags.cr.cr_51_update == 0U) {
                amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_before_ce_gain_sub !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
                rval |= IK_ERR_0205;//Ring buffer Eorror
            } else {
                in_before_ce_gain.p_before_ce_wb_gain = &p_filters->input_param.before_ce_wb_gain;
                out_before_ce_gain_sub.p_cr_51 = p_flow_tbl_list->p_CR_buf_51;
                out_before_ce_gain_sub.cr_51_size = CR_SIZE_51;
                rval |= ikc_before_ce_gain_sub(&in_before_ce_gain, &out_before_ce_gain_sub);
            }
        }

        if(exp_num < 4UL) {
            //complexity refine.
            //hdr sub related.
            rval |= process_hdr_ce_sub_cr_patr1(p_flow_tbl_list, p_filters, exp_num, window_calculate_updated);

            // ce sub
            if(exp_num > 0UL) {
                //complexity refine.
                //ce sub related.
                rval |= process_hdr_ce_sub_cr_patr2(p_flow_tbl_list, p_filters, exp_num, window_calculate_updated);
            }
        } else {
            amba_ik_system_print_uint32_5("[IK] exe_process_hdr_ce_sub_cr fail, incorrect exp_num %d", exp_num, DC_U, DC_U, DC_U, DC_U);
            rval = IK_ERR_0000;
        }
    } else {
        amba_ik_system_print_uint32_5("[IK] exe_process_hdr_ce_sub_cr fail, do not support y2y mode!", DC_U, DC_U, DC_U, DC_U, DC_U);
        rval = IK_ERR_0004;
    }

    return rval;
}

static INLINE uint32 ik_check_mctf_mcts_3d_tbl_1(const amba_ik_filter_t *p_filters)
{
    uint32 rval = IK_OK;
    // MCTF
    if(p_filters->input_param.video_mctf.y_temporal_either_max_change_or_t0_t1_add.method == 1U) {
        amba_ik_system_print_str_5("[IK][ERROR] three_d_table is disable. video_mctf.y_temporal_either_max_change_or_t0_t1_add.method can not be 1\n", DC_S, DC_S, DC_S, DC_S, DC_S);
        rval |= IK_ERR_0008;
    }
    if(p_filters->input_param.video_mctf.cb_temporal_either_max_change_or_t0_t1_add.method == 1U) {
        amba_ik_system_print_str_5("[IK][ERROR] three_d_table is disable. video_mctf.cb_temporal_either_max_change_or_t0_t1_add.method can not be 1\n", DC_S, DC_S, DC_S, DC_S, DC_S);
        rval |= IK_ERR_0008;
    }
    if(p_filters->input_param.video_mctf.cr_temporal_either_max_change_or_t0_t1_add.method == 1U) {
        amba_ik_system_print_str_5("[IK][ERROR] three_d_table is disable. video_mctf.cr_temporal_either_max_change_or_t0_t1_add.method can not be 1\n", DC_S, DC_S, DC_S, DC_S, DC_S);
        rval |= IK_ERR_0008;
    }

    if(p_filters->input_param.video_mctf.y_spat_blend.method == 1U) {
        amba_ik_system_print_str_5("[IK][ERROR] three_d_table is disable. video_mctf.y_spat_blend.method can not be 1\n", DC_S, DC_S, DC_S, DC_S, DC_S);
        rval |= IK_ERR_0008;
    }
    if(p_filters->input_param.video_mctf.cb_spat_blend.method == 1U) {
        amba_ik_system_print_str_5("[IK][ERROR] three_d_table is disable. video_mctf.cb_spat_blend.method can not be 1\n", DC_S, DC_S, DC_S, DC_S, DC_S);
        rval |= IK_ERR_0008;
    }
    if(p_filters->input_param.video_mctf.cr_spat_blend.method == 1U) {
        amba_ik_system_print_str_5("[IK][ERROR] three_d_table is disable. video_mctf.cr_spat_blend.method can not be 1\n", DC_S, DC_S, DC_S, DC_S, DC_S);
        rval |= IK_ERR_0008;
    }

    if(p_filters->input_param.video_mctf.y_advanced_iso_max_change_method == 1U) {
        amba_ik_system_print_str_5("[IK][ERROR] three_d_table is disable. video_mctf.y_advanced_iso_max_change_method can not be 1\n", DC_S, DC_S, DC_S, DC_S, DC_S);
        rval |= IK_ERR_0008;
    }
    if(p_filters->input_param.video_mctf.cb_advanced_iso_max_change_method == 1U) {
        amba_ik_system_print_str_5("[IK][ERROR] three_d_table is disable. video_mctf.cb_advanced_iso_max_change_method can not be 1\n", DC_S, DC_S, DC_S, DC_S, DC_S);
        rval |= IK_ERR_0008;
    }
    if(p_filters->input_param.video_mctf.cr_advanced_iso_max_change_method == 1U) {
        amba_ik_system_print_str_5("[IK][ERROR] three_d_table is disable. video_mctf.cr_advanced_iso_max_change_method can not be 1\n", DC_S, DC_S, DC_S, DC_S, DC_S);
        rval |= IK_ERR_0008;
    }

    return rval;
}

static INLINE uint32 ik_check_mctf_mcts_3d_tbl_2(const amba_ik_filter_t *p_filters)
{
    uint32 rval = IK_OK;
    // MCTF
    if(p_filters->input_param.video_mctf.y_3d_maxchange.method == 1U) {
        amba_ik_system_print_str_5("[IK][ERROR] three_d_table is disable. video_mctf.y_3d_maxchange.method can not be 1\n", DC_S, DC_S, DC_S, DC_S, DC_S);
        rval |= IK_ERR_0008;
    }
    if(p_filters->input_param.video_mctf.cb_3d_maxchange.method == 1U) {
        amba_ik_system_print_str_5("[IK][ERROR] three_d_table is disable. video_mctf.cb_3d_maxchange.method can not be 1\n", DC_S, DC_S, DC_S, DC_S, DC_S);
        rval |= IK_ERR_0008;
    }
    if(p_filters->input_param.video_mctf.cr_3d_maxchange.method == 1U) {
        amba_ik_system_print_str_5("[IK][ERROR] three_d_table is disable. video_mctf.cr_3d_maxchange.method can not be 1\n", DC_S, DC_S, DC_S, DC_S, DC_S);
        rval |= IK_ERR_0008;
    }

    if(p_filters->input_param.video_mctf.y_overall_max_change.method == 1U) {
        amba_ik_system_print_str_5("[IK][ERROR] three_d_table is disable. video_mctf.y_overall_max_change.method can not be 1\n", DC_S, DC_S, DC_S, DC_S, DC_S);
        rval |= IK_ERR_0008;
    }
    if(p_filters->input_param.video_mctf.cb_overall_max_change.method == 1U) {
        amba_ik_system_print_str_5("[IK][ERROR] three_d_table is disable. video_mctf.cb_overall_max_change.method can not be 1\n", DC_S, DC_S, DC_S, DC_S, DC_S);
        rval |= IK_ERR_0008;
    }
    if(p_filters->input_param.video_mctf.cr_overall_max_change.method == 1U) {
        amba_ik_system_print_str_5("[IK][ERROR] three_d_table is disable. video_mctf.cr_overall_max_change.method can not be 1\n", DC_S, DC_S, DC_S, DC_S, DC_S);
        rval |= IK_ERR_0008;
    }


    if(p_filters->input_param.video_mctf.y_spat_filt_max_smth_change.method == 1U) {
        amba_ik_system_print_str_5("[IK][ERROR] three_d_table is disable. video_mctf.y_spat_filt_max_smth_change.method can not be 1\n", DC_S, DC_S, DC_S, DC_S, DC_S);
        rval |= IK_ERR_0008;
    }
    if(p_filters->input_param.video_mctf.cb_spat_filt_max_smth_change.method == 1U) {
        amba_ik_system_print_str_5("[IK][ERROR] three_d_table is disable. video_mctf.cb_spat_filt_max_smth_change.method can not be 1\n", DC_S, DC_S, DC_S, DC_S, DC_S);
        rval |= IK_ERR_0008;
    }
    if(p_filters->input_param.video_mctf.cr_spat_filt_max_smth_change.method == 1U) {
        amba_ik_system_print_str_5("[IK][ERROR] three_d_table is disable. video_mctf.cr_spat_filt_max_smth_change.method can not be 1\n", DC_S, DC_S, DC_S, DC_S, DC_S);
        rval |= IK_ERR_0008;
    }

    return rval;
}

static INLINE uint32 ik_check_mctf_mcts_3d_tbl_3(const amba_ik_filter_t *p_filters)
{
    uint32 rval = IK_OK;
    // MCTF
    if(p_filters->input_param.video_mctf.y_spat_smth_dir.method == 1U) {
        amba_ik_system_print_str_5("[IK][ERROR] three_d_table is disable. video_mctf.y_spat_smth_dir.method can not be 1\n", DC_S, DC_S, DC_S, DC_S, DC_S);
        rval |= IK_ERR_0008;
    }
    if(p_filters->input_param.video_mctf.cb_spat_smth_dir.method == 1U) {
        amba_ik_system_print_str_5("[IK][ERROR] three_d_table is disable. video_mctf.cb_spat_smth_dir.method can not be 1\n", DC_S, DC_S, DC_S, DC_S, DC_S);
        rval |= IK_ERR_0008;
    }
    if(p_filters->input_param.video_mctf.cr_spat_smth_dir.method == 1U) {
        amba_ik_system_print_str_5("[IK][ERROR] three_d_table is disable. video_mctf.cr_spat_smth_dir.method can not be 1\n", DC_S, DC_S, DC_S, DC_S, DC_S);
        rval |= IK_ERR_0008;
    }

    if(p_filters->input_param.video_mctf.y_spat_smth_iso.method == 1U) {
        amba_ik_system_print_str_5("[IK][ERROR] three_d_table is disable. video_mctf.y_spat_smth_iso.method can not be 1\n", DC_S, DC_S, DC_S, DC_S, DC_S);
        rval |= IK_ERR_0008;
    }
    if(p_filters->input_param.video_mctf.cb_spat_smth_iso.method == 1U) {
        amba_ik_system_print_str_5("[IK][ERROR] three_d_table is disable. video_mctf.cb_spat_smth_iso.method can not be 1\n", DC_S, DC_S, DC_S, DC_S, DC_S);
        rval |= IK_ERR_0008;
    }
    if(p_filters->input_param.video_mctf.cr_spat_smth_iso.method == 1U) {
        amba_ik_system_print_str_5("[IK][ERROR] three_d_table is disable. video_mctf.cr_spat_smth_iso.method can not be 1\n", DC_S, DC_S, DC_S, DC_S, DC_S);
        rval |= IK_ERR_0008;
    }

    if(p_filters->input_param.video_mctf.y_level_based_ta.method == 1U) {
        amba_ik_system_print_str_5("[IK][ERROR] three_d_table is disable. video_mctf.y_level_based_ta.method can not be 1\n", DC_S, DC_S, DC_S, DC_S, DC_S);
        rval |= IK_ERR_0008;
    }
    if(p_filters->input_param.video_mctf.cb_level_based_ta.method == 1U) {
        amba_ik_system_print_str_5("[IK][ERROR] three_d_table is disable. video_mctf.cb_level_based_ta.method can not be 1\n", DC_S, DC_S, DC_S, DC_S, DC_S);
        rval |= IK_ERR_0008;
    }
    if(p_filters->input_param.video_mctf.cr_level_based_ta.method == 1U) {
        amba_ik_system_print_str_5("[IK][ERROR] three_d_table is disable. video_mctf.cr_level_based_ta.method can not be 1\n", DC_S, DC_S, DC_S, DC_S, DC_S);
        rval |= IK_ERR_0008;
    }

    return rval;
}

static INLINE uint32 ik_check_mctf_mcts_3d_tbl_4(const amba_ik_filter_t *p_filters)
{
    uint32 rval = IK_OK;
    // MCTF
    if(p_filters->input_param.video_mctf.y_temporal_min_target.method == 1U) {
        amba_ik_system_print_str_5("[IK][ERROR] three_d_table is disable. video_mctf.y_temporal_min_target.method can not be 1\n", DC_S, DC_S, DC_S, DC_S, DC_S);
        rval |= IK_ERR_0008;
    }
    if(p_filters->input_param.video_mctf.cb_temporal_min_target.method == 1U) {
        amba_ik_system_print_str_5("[IK][ERROR] three_d_table is disable. video_mctf.cb_temporal_min_target.method can not be 1\n", DC_S, DC_S, DC_S, DC_S, DC_S);
        rval |= IK_ERR_0008;
    }
    if(p_filters->input_param.video_mctf.cr_temporal_min_target.method == 1U) {
        amba_ik_system_print_str_5("[IK][ERROR] three_d_table is disable. video_mctf.cr_temporal_min_target.method can not be 1\n", DC_S, DC_S, DC_S, DC_S, DC_S);
        rval |= IK_ERR_0008;
    }

    if(p_filters->input_param.video_mctf.y_spatial_max_temporal.method == 1U) {
        amba_ik_system_print_str_5("[IK][ERROR] three_d_table is disable. video_mctf.y_spatial_max_temporal.method can not be 1\n", DC_S, DC_S, DC_S, DC_S, DC_S);
        rval |= IK_ERR_0008;
    }
    if(p_filters->input_param.video_mctf.cb_spatial_max_temporal.method == 1U) {
        amba_ik_system_print_str_5("[IK][ERROR] three_d_table is disable. video_mctf.cb_spatial_max_temporal.method can not be 1\n", DC_S, DC_S, DC_S, DC_S, DC_S);
        rval |= IK_ERR_0008;
    }
    if(p_filters->input_param.video_mctf.cr_spatial_max_temporal.method == 1U) {
        amba_ik_system_print_str_5("[IK][ERROR] three_d_table is disable. video_mctf.cr_spatial_max_temporal.method can not be 1\n", DC_S, DC_S, DC_S, DC_S, DC_S);
        rval |= IK_ERR_0008;
    }

    return rval;
}

static INLINE uint32 ik_check_mctf_mcts_3d_tbl_5(const amba_ik_filter_t *p_filters)
{
    uint32 rval = IK_OK;
    //MCTS
    if(p_filters->input_param.final_sharpen_noise.level_str_adjust.method == 1U) {
        amba_ik_system_print_str_5("[IK][ERROR] three_d_table is disable. final_sharpen_both.level_str_adjust.method can not be 1\n", DC_S, DC_S, DC_S, DC_S, DC_S);
        amba_ik_system_print_str_5("[IK][ERROR] Please check ik_set_fnl_shp_three_d_table() or ik_set_fnl_shp_noise() \n", DC_S, DC_S, DC_S, DC_S, DC_S);
        rval |= IK_ERR_0008;
    }
    if(p_filters->input_param.final_sharpen_noise.advanced_iso.max_change_method == 1U) {
        amba_ik_system_print_str_5("[IK][ERROR] three_d_table is disable. final_sharpen_both.advanced_iso.max_change_method can not be 1\n", DC_S, DC_S, DC_S, DC_S, DC_S);
        amba_ik_system_print_str_5("[IK][ERROR] Please check ik_set_fnl_shp_three_d_table() or ik_set_fnl_shp_noise() \n", DC_S, DC_S, DC_S, DC_S, DC_S);
        rval |= IK_ERR_0008;
    }
    if(p_filters->input_param.final_sharpen_noise.advanced_iso.noise_level_method == 1U) {
        amba_ik_system_print_str_5("[IK][ERROR] three_d_table is disable. final_sharpen_both.advanced_iso.noise_level_method can not be 1\n", DC_S, DC_S, DC_S, DC_S, DC_S);
        amba_ik_system_print_str_5("[IK][ERROR] Please check ik_set_fnl_shp_three_d_table() or ik_set_fnl_shp_noise() \n", DC_S, DC_S, DC_S, DC_S, DC_S);
        rval |= IK_ERR_0008;
    }
    if(p_filters->input_param.final_sharpen_noise.advanced_iso.str_a_method == 1U) {
        amba_ik_system_print_str_5("[IK][ERROR] three_d_table is disable. final_sharpen_both.advanced_iso.str_a_method can not be 1\n", DC_S, DC_S, DC_S, DC_S, DC_S);
        amba_ik_system_print_str_5("[IK][ERROR] Please check ik_set_fnl_shp_three_d_table() or ik_set_fnl_shp_noise() \n", DC_S, DC_S, DC_S, DC_S, DC_S);
        rval |= IK_ERR_0008;
    }
    if(p_filters->input_param.final_sharpen_noise.advanced_iso.str_b_method == 1U) {
        amba_ik_system_print_str_5("[IK][ERROR] three_d_table is disable. final_sharpen_both.advanced_iso.str_b_method can not be 1\n", DC_S, DC_S, DC_S, DC_S, DC_S);
        amba_ik_system_print_str_5("[IK][ERROR] Please check ik_set_fnl_shp_three_d_table() or ik_set_fnl_shp_noise() \n", DC_S, DC_S, DC_S, DC_S, DC_S);
        rval |= IK_ERR_0008;
    }

    return rval;
}

static INLINE uint32 ik_check_mctf_mcts_3d_tbl_6(const amba_ik_filter_t *p_filters)
{
    uint32 rval = IK_OK;
    //MCTS
    if(p_filters->input_param.final_sharpen_coring_idx_scale.method == 1U) {
        amba_ik_system_print_str_5("[IK][ERROR] three_d_table is disable. final_sharpen_coring_idx_scale.method can not be 1\n", DC_S, DC_S, DC_S, DC_S, DC_S);
        amba_ik_system_print_str_5("[IK][ERROR] Please check ik_set_fnl_shp_three_d_table() or ik_set_fnl_shp_coring_idx_scale() \n", DC_S, DC_S, DC_S, DC_S, DC_S);
        rval |= IK_ERR_0008;
    }
    if(p_filters->input_param.final_sharpen_min_coring_result.method == 1U) {
        amba_ik_system_print_str_5("[IK][ERROR] three_d_table is disable. final_sharpen_min_coring_result.method can not be 1\n", DC_S, DC_S, DC_S, DC_S, DC_S);
        amba_ik_system_print_str_5("[IK][ERROR] Please check ik_set_fnl_shp_three_d_table() or ik_set_fnl_shp_min_coring_rslt() \n", DC_S, DC_S, DC_S, DC_S, DC_S);
        rval |= IK_ERR_0008;
    }
    if(p_filters->input_param.final_sharpen_max_coring_result.method == 1U) {
        amba_ik_system_print_str_5("[IK][ERROR] three_d_table is disable. final_sharpen_coring_idx_scale.method can not be 1\n", DC_S, DC_S, DC_S, DC_S, DC_S);
        amba_ik_system_print_str_5("[IK][ERROR] Please check ik_set_fnl_shp_three_d_table() or ik_set_fnl_shp_max_coring_rslt() \n", DC_S, DC_S, DC_S, DC_S, DC_S);
        rval |= IK_ERR_0008;
    }
    if(p_filters->input_param.final_sharpen_scale_coring.method == 1U) {
        amba_ik_system_print_str_5("[IK][ERROR] three_d_table is disable. final_sharpen_scale_coring.method can not be 1\n", DC_S, DC_S, DC_S, DC_S, DC_S);
        amba_ik_system_print_str_5("[IK][ERROR] Please check ik_set_fnl_shp_three_d_table() or ik_set_fnl_shp_scale_coring() \n", DC_S, DC_S, DC_S, DC_S, DC_S);
        rval |= IK_ERR_0008;
    }
    if(p_filters->input_param.final_sharpen_coring_idx_scale.method == 1U) {
        amba_ik_system_print_str_5("[IK][ERROR] three_d_table is disable. final_sharpen_coring_idx_scale.method can not be 1\n", DC_S, DC_S, DC_S, DC_S, DC_S);
        amba_ik_system_print_str_5("[IK][ERROR] Please check ik_set_fnl_shp_three_d_table() or ik_set_fnl_shp_coring_idx_scale() \n", DC_S, DC_S, DC_S, DC_S, DC_S);
        rval |= IK_ERR_0008;
    }

    return rval;
}

static uint32 ik_check_mctf_mcts(const amba_ik_filter_t *p_filters)
{
    uint32 rval = IK_OK;
    if(p_filters->input_param.three_d_table_enable == 0U) {
        //complexity refine.
        rval |= ik_check_mctf_mcts_3d_tbl_1(p_filters);
        rval |= ik_check_mctf_mcts_3d_tbl_2(p_filters);
        rval |= ik_check_mctf_mcts_3d_tbl_3(p_filters);
        rval |= ik_check_mctf_mcts_3d_tbl_4(p_filters);
        if (rval != IK_OK) {
            amba_ik_system_print_str_5("[IK][ERROR] Please check ik_set_fnl_shp_three_d_table() or ik_set_video_mctf() \n", DC_S, DC_S, DC_S, DC_S, DC_S);
        }
        rval |= ik_check_mctf_mcts_3d_tbl_5(p_filters);
        rval |= ik_check_mctf_mcts_3d_tbl_6(p_filters);
    }

    return rval;
}

static inline uint32 process_mctf_mcts_cr_fnl_shp_p1(idsp_ik_flow_ctrl_t *p_flow, amba_ik_flow_tables_list_t *p_flow_tbl_list, amba_ik_filter_t *p_filters)
{
    uint32 rval = IK_OK;
    ikc_out_final_sharpen_t out_fnl_shp;
    uint8 bool_result;

    (void)amba_ik_system_memcpy(p_flow, p_flow, sizeof(uint8));//misra fix.
    (void)amba_ik_system_memcpy(p_flow_tbl_list, p_flow_tbl_list, sizeof(uint8));//misra fix.

    out_fnl_shp.p_cr_117 = p_flow_tbl_list->p_CR_buf_117;
    out_fnl_shp.cr_117_size = CR_SIZE_117;
    bool_result = equal_op_u32(p_filters->update_flags.iso.fnlshpns_both_updated, 1U) + equal_op_u32(p_filters->update_flags.iso.fnlshpns_noise_updated, 1U)+
                  equal_op_u32(p_filters->update_flags.iso.fnlshpns_fir_updated, 1U) + equal_op_u32(p_filters->update_flags.iso.window_size_info_updated, 1U);
    if(bool_result != 0U) {
        ikc_in_final_sharpen_t in_fnl_shp;

        if(p_filters->update_flags.cr.cr_117_update == 0U) {
            amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_fnlshp !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
            rval |= IK_ERR_0205;//Ring buffer Eorror
        } else {
            in_fnl_shp.sharpen_mode = p_filters->input_param.final_sharpen_both.mode;
            in_fnl_shp.sharpen_both_enable = p_filters->input_param.final_sharpen_both.enable;
            in_fnl_shp.p_final_sharpen_noise = &p_filters->input_param.final_sharpen_noise;
            in_fnl_shp.p_final_sharpen_fir = &p_filters->input_param.final_sharpen_fir;
            in_fnl_shp.p_main = &p_filters->input_param.window_size_info.main_win;
            rval |= ikc_fnlshp(&in_fnl_shp, &out_fnl_shp);
        }
    }
    bool_result = equal_op_u32(p_filters->update_flags.iso.fnlshpns_both_updated, 1U) + equal_op_u32(p_filters->update_flags.iso.video_mctf_updated, 1U);
    if(bool_result != 0U) {
        ikc_in_final_sharpen_both_t in_fnl_shp_both;

        if(p_filters->update_flags.cr.cr_117_update == 0U) {
            amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_fnlshp_both !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
            rval |= IK_ERR_0205;//Ring buffer Eorror
        } else {
            in_fnl_shp_both.p_final_sharpen_both = &p_filters->input_param.final_sharpen_both;
            in_fnl_shp_both.mctf_y_combine_strength = p_filters->input_param.video_mctf.y_combine_strength;
            rval |= ikc_fnlshp_both(&in_fnl_shp_both, &out_fnl_shp);
        }
    }
    if(p_filters->update_flags.iso.fnlshpns_coring_updated == 1U) {
        ikc_in_final_sharpen_coring_t in_fnl_shp_coring;

        if(p_filters->update_flags.cr.cr_117_update == 0U) {
            amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_fnlshp_coring !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
            rval |= IK_ERR_0205;//Ring buffer Eorror
        } else {
            in_fnl_shp_coring.p_final_sharpen_coring = &p_filters->input_param.final_sharpen_coring;
            rval |= ikc_fnlshp_coring(&in_fnl_shp_coring, &out_fnl_shp);
        }
    }
    if(p_filters->update_flags.iso.fnlshpns_coring_index_scale_updated == 1U) {
        ikc_in_final_sharpen_coring_idx_scale_t in_fnl_shp_coring_idx_scale;

        if(p_filters->update_flags.cr.cr_117_update == 0U) {
            amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_fnlshp_coring_idx_scale !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
            rval |= IK_ERR_0205;//Ring buffer Eorror
        } else {
            in_fnl_shp_coring_idx_scale.p_final_sharpen_coring_idx_scale = &p_filters->input_param.final_sharpen_coring_idx_scale;
            rval |= ikc_fnlshp_coring_idx_scale(&in_fnl_shp_coring_idx_scale, &out_fnl_shp);
        }
    }

    return rval;
}

static inline uint32 process_mctf_mcts_cr_fnl_shp_p2(idsp_ik_flow_ctrl_t *p_flow, amba_ik_flow_tables_list_t *p_flow_tbl_list, amba_ik_filter_t *p_filters)
{
    uint32 rval = IK_OK;
    ikc_out_final_sharpen_t out_fnl_shp;

    (void)amba_ik_system_memcpy(p_flow, p_flow, sizeof(uint8));//misra fix.
    (void)amba_ik_system_memcpy(p_flow_tbl_list, p_flow_tbl_list, sizeof(uint8));//misra fix.

    out_fnl_shp.p_cr_117 = p_flow_tbl_list->p_CR_buf_117;
    out_fnl_shp.cr_117_size = CR_SIZE_117;

    if(p_filters->update_flags.iso.fnlshpns_min_coring_result_updated == 1U) {
        ikc_in_final_sharpen_min_coring_t in_fnl_shp_min_coring;

        if(p_filters->update_flags.cr.cr_117_update == 0U) {
            amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_fnlshp_min_coring !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
            rval |= IK_ERR_0205;//Ring buffer Eorror
        } else {
            in_fnl_shp_min_coring.p_final_sharpen_min_coring_result = &p_filters->input_param.final_sharpen_min_coring_result;
            rval |= ikc_fnlshp_min_coring(&in_fnl_shp_min_coring, &out_fnl_shp);
        }
    }
    if(p_filters->update_flags.iso.fnlshpns_max_coring_result_updated == 1U) {
        ikc_in_final_sharpen_max_coring_t in_fnl_shp_max_coring;

        if(p_filters->update_flags.cr.cr_117_update == 0U) {
            amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_fnlshp_max_coring !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
            rval |= IK_ERR_0205;//Ring buffer Eorror
        } else {
            in_fnl_shp_max_coring.p_final_sharpen_max_coring_result = &p_filters->input_param.final_sharpen_max_coring_result;
            rval |= ikc_fnlshp_max_coring(&in_fnl_shp_max_coring, &out_fnl_shp);
        }
    }
    if(p_filters->update_flags.iso.fnlshpns_scale_coring_updated == 1U) {
        ikc_in_final_sharpen_scale_coring_t in_fnl_shp_scale_coring;

        if(p_filters->update_flags.cr.cr_117_update == 0U) {
            amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_fnlshp_scale_coring !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
            rval |= IK_ERR_0205;//Ring buffer Eorror
        } else {
            in_fnl_shp_scale_coring.p_final_sharpen_scale_coring = &p_filters->input_param.final_sharpen_scale_coring;
            rval |= ikc_fnlshp_scale_coring(&in_fnl_shp_scale_coring, &out_fnl_shp);
        }
    }

    return rval;
}

uint32 exe_process_mctf_mcts_cr(idsp_ik_flow_ctrl_t *p_flow, amba_ik_flow_tables_list_t *p_flow_tbl_list, amba_ik_filter_t *p_filters)
{
    uint32 rval = IK_OK;
    uint8 bool_result;
    (void)amba_ik_system_memcpy(p_flow, p_flow, sizeof(uint8));//misra fix.
    (void)amba_ik_system_memcpy(p_flow_tbl_list, p_flow_tbl_list, sizeof(uint8));//misra fix.

    rval |= ik_check_mctf_mcts(p_filters);

    if(rval == IK_OK) {
        //mctf
        bool_result = equal_op_u32(p_filters->update_flags.iso.video_mctf_updated, 1U) + equal_op_u32(p_filters->update_flags.iso.internal_video_mctf_updated, 1U)+
                      equal_op_u32(p_filters->update_flags.iso.video_mctf_ta_updated, 1U) + equal_op_u32(p_filters->update_flags.iso.fnlshpns_both_updated, 1U)+
                      equal_op_u32(p_filters->update_flags.iso.window_size_info_updated, 1U);
        if(bool_result != 0U) {
            ikc_in_mctf_t in_mctf;
            ikc_out_mctf_t out_mctf;

            bool_result = equal_op_u32(p_filters->update_flags.cr.cr_111_update, 0U) + equal_op_u32(p_filters->update_flags.cr.cr_112_update, 0U) +
                          equal_op_u32(p_filters->update_flags.cr.cr_113_update, 0U) + equal_op_u32(p_filters->update_flags.cr.cr_116_update, 0U);
            if(bool_result != 0U) {
                amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_mctf !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
                rval |= IK_ERR_0205;//Ring buffer Eorror
            } else {
                in_mctf.p_video_mctf = &p_filters->input_param.video_mctf;
                in_mctf.p_internal_video_mctf = &p_filters->input_param.internal_video_mctf;
                in_mctf.p_video_mctf_ta = &p_filters->input_param.video_mctf_ta;
                in_mctf.p_final_sharpen_both = &p_filters->input_param.final_sharpen_both;
                in_mctf.p_main = &p_filters->input_param.window_size_info.main_win;
                out_mctf.p_cr_111 = p_flow_tbl_list->p_CR_buf_111;
                out_mctf.cr_111_size = CR_SIZE_111;
                out_mctf.p_cr_112 = p_flow_tbl_list->p_CR_buf_112;
                out_mctf.cr_112_size = CR_SIZE_112;
                out_mctf.p_cr_113 = p_flow_tbl_list->p_CR_buf_113;
                out_mctf.cr_113_size = CR_SIZE_113;
                out_mctf.p_cr_116 = p_flow_tbl_list->p_CR_buf_116;
                out_mctf.cr_116_size = CR_SIZE_116;
                out_mctf.p_flow_info = &p_flow->flow_info;
                out_mctf.p_phase = &p_flow->phase;
                rval |= ikc_mctf(&in_mctf, &out_mctf);
            }
        }

        //shpB
        //complexity refine.
        rval |= process_mctf_mcts_cr_fnl_shp_p1(p_flow, p_flow_tbl_list, p_filters);
        rval |= process_mctf_mcts_cr_fnl_shp_p2(p_flow, p_flow_tbl_list, p_filters);
        //shpB 3D table.
        if(p_filters->update_flags.iso.fnlshpns_both_tdt_updated == 1U) {
            ikc_in_final_sharpen_both_3d_t in_fnl_shp_both_3d;
            ikc_out_final_sharpen_both_3d_t out_fnl_shp_both_3d;

            bool_result = equal_op_u32(p_filters->update_flags.cr.cr_117_update, 0U) + equal_op_u32(p_filters->update_flags.cr.cr_119_update, 0U);
            if(bool_result != 0U) {
                amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_fnlshp_both_3d !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
                rval |= IK_ERR_0205;//Ring buffer Eorror
            } else {
                in_fnl_shp_both_3d.p_final_sharpen_both_three_d_table = &p_filters->input_param.final_sharpen_both_three_d_table;
                out_fnl_shp_both_3d.p_cr_117 = p_flow_tbl_list->p_CR_buf_117;
                out_fnl_shp_both_3d.cr_117_size = CR_SIZE_117;
                out_fnl_shp_both_3d.p_cr_119 = p_flow_tbl_list->p_CR_buf_119;
                out_fnl_shp_both_3d.cr_119_size = CR_SIZE_119;
                rval |= ikc_fnlshp_both_3d(&in_fnl_shp_both_3d, &out_fnl_shp_both_3d);
            }
        }

        if(p_filters->update_flags.iso.video_mctf_and_final_sharpen_updated == 1U) {
            ikc_in_mctf_pos_dep33_t in_mctf_pos;
            ikc_out_mctf_pos_dep33_t out_mctf_pos;

            bool_result = equal_op_u32(p_filters->update_flags.cr.cr_114_update, 0U) + equal_op_u32(p_filters->update_flags.cr.cr_118_update, 0U);
            if(bool_result != 0U) {
                amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_mctf_pos_dep33 !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
                rval |= IK_ERR_0205;//Ring buffer Eorror
            } else {
                in_mctf_pos.p_video_mctf_and_final_sharpen = &p_filters->input_param.video_mctf_and_final_sharpen;
                out_mctf_pos.p_cr_114 = p_flow_tbl_list->p_CR_buf_114;
                out_mctf_pos.cr_114_size = CR_SIZE_114;
                out_mctf_pos.p_cr_118 = p_flow_tbl_list->p_CR_buf_118;
                out_mctf_pos.cr_118_size = CR_SIZE_118;
                rval |= ikc_mctf_pos_dep33(&in_mctf_pos, &out_mctf_pos);
            }
        }
    }
    return rval;
}

uint32 exe_query_calc_geo_buf_size(ik_query_calc_geo_buf_size_t *p_buf_size_info)
{
    uint32 rval = IK_OK;

    if (p_buf_size_info == NULL) {
        rval = IK_ERR_0005;
        amba_ik_system_print_str_5("[IK][ERROR] p_buf_size_info == NULL\n", DC_S, DC_S, DC_S, DC_S, DC_S);
    } else {
        p_buf_size_info->work_buf_size = sizeof(amba_ik_calc_geo_work_buf_t);
        p_buf_size_info->out_buf_size = sizeof(amalgam_group_update_data_t);
    }

    return rval;
}

static inline uint32 exe_calc_geo_settings_basic0_check(const ik_calc_geo_settings_t *p_info, const ik_calc_geo_result_t *p_result)
{
    //complexity refine.
    uint32 rval = IK_OK;
    uint32 misra_u32;

    (void)amba_ik_system_memcpy(&misra_u32, &p_result->p_out, sizeof(uint32));
    if ((misra_u32 & 0x1FU) != 0U) {
        rval = IK_ERR_0006;
        amba_ik_system_print_uint32_5("[IK][ERROR] p_out %p must be 32 alignment\n", misra_u32, DC_U, DC_U, DC_U, DC_U);
    }

    (void)amba_ik_system_memcpy(&misra_u32, &p_info->p_work_buf, sizeof(uint32));
    if ((misra_u32 & 0x7FU) != 0U) {
        rval = IK_ERR_0006;
        amba_ik_system_print_uint32_5("[IK][ERROR] p_work_buf %p must be 128 alignment\n", misra_u32, DC_U, DC_U, DC_U, DC_U);
    }
    if((equal_op_u32(p_info->p_in->chroma_filter_radius,32u)+
        equal_op_u32(p_info->p_in->chroma_filter_radius,64u)+
        equal_op_u32(p_info->p_in->chroma_filter_radius,128u))== 0u) {
        rval = IK_ERR_0008;
        amba_ik_system_print_uint32_5("[IK][ERROR] p_info->p_in->chroma_filter_radius %d must be 32,64,128\n", p_info->p_in->chroma_filter_radius, DC_U, DC_U, DC_U, DC_U);
    }
    return rval;
}
static uint32 exe_calc_geo_settings_basic_check(const ik_calc_geo_settings_t *p_info, const ik_calc_geo_result_t *p_result)
{
    uint32 rval = IK_OK;
    uint8 bool_result;

    if (p_info->p_in->p_window_size_info == NULL) {
        rval = IK_ERR_0005;
        amba_ik_system_print_str_5("[IK][ERROR] p_info->p_in->p_window_size_info == NULL\n", DC_S, DC_S, DC_S, DC_S, DC_S);
    } else {
        bool_result = equal_op_u32(p_info->p_in->p_window_size_info->vin_sensor.width, 0U) + equal_op_u32(p_info->p_in->p_window_size_info->vin_sensor.height, 0U) +
                      equal_op_u32(p_info->p_in->p_window_size_info->vin_sensor.h_sub_sample.factor_num, 0U) + equal_op_u32(p_info->p_in->p_window_size_info->vin_sensor.h_sub_sample.factor_den, 0U) +
                      equal_op_u32(p_info->p_in->p_window_size_info->vin_sensor.v_sub_sample.factor_num, 0U) + equal_op_u32(p_info->p_in->p_window_size_info->vin_sensor.v_sub_sample.factor_den, 0U) +
                      equal_op_u32(p_info->p_in->p_window_size_info->main_win.width, 0U) + equal_op_u32(p_info->p_in->p_window_size_info->main_win.height, 0U);
        if (bool_result != 0U) {
            rval = IK_ERR_0008;
            amba_ik_system_print_uint32_5("[IK][ERROR] vin w %d h %d main w %d h %d \n",
                                          p_info->p_in->p_window_size_info->vin_sensor.width, p_info->p_in->p_window_size_info->vin_sensor.height,
                                          p_info->p_in->p_window_size_info->main_win.width, p_info->p_in->p_window_size_info->main_win.height, DC_U);
            amba_ik_system_print_uint32_5("[IK][ERROR] vin h_sub %d/%d v_sub %d/%d \n",
                                          p_info->p_in->p_window_size_info->vin_sensor.h_sub_sample.factor_num, p_info->p_in->p_window_size_info->vin_sensor.h_sub_sample.factor_den,
                                          p_info->p_in->p_window_size_info->vin_sensor.v_sub_sample.factor_num, p_info->p_in->p_window_size_info->vin_sensor.v_sub_sample.factor_den, DC_U);
        }
    }

    if (p_info->p_in->warp_enable == 1) {
        if (p_info->p_in->p_calib_warp_info == NULL) {
            rval = IK_ERR_0005;
            amba_ik_system_print_int32_5("[IK][ERROR] warp_enable %d but p_calib_warp_info is NULL\n", p_info->p_in->warp_enable, DC_I, DC_I, DC_I, DC_I);
        } else {
            if (p_info->p_in->p_calib_warp_info->pwarp == NULL) {
                rval = IK_ERR_0005;
                amba_ik_system_print_int32_5("[IK][ERROR] warp_enable %d but p_calib_warp_info->pwarp is NULL\n", p_info->p_in->warp_enable, DC_I, DC_I, DC_I, DC_I);
            }
        }
    }
    bool_result = equal_op_u32((uint32)p_info->p_in->use_cawarp, 1U) + equal_op_u32((uint32)p_info->p_in->cawarp_enable, 1U);
    if (bool_result == 2U) {
        if (p_info->p_in->p_calib_ca_warp_info == NULL) {
            rval = IK_ERR_0005;
            amba_ik_system_print_int32_5("[IK][ERROR] cawarp_enable %d but p_calib_ca_warp_info is NULL\n", p_info->p_in->cawarp_enable, DC_I, DC_I, DC_I, DC_I);
        } else {
            bool_result = null_check_op(p_info->p_in->p_calib_ca_warp_info->p_cawarp_red)+null_check_op(p_info->p_in->p_calib_ca_warp_info->p_cawarp_blue);
            if (bool_result != 0U) {
                rval = IK_ERR_0005;
                amba_ik_system_print_int32_5("[IK][ERROR] cawarp_enable %d but p_calib_ca_warp_info->p_cawarp_red or p_calib_ca_warp_info->p_cawarp_blue is NULL\n",
                                             p_info->p_in->cawarp_enable, DC_I, DC_I, DC_I, DC_I);
            }
        }
    }

    rval |= exe_calc_geo_settings_basic0_check(p_info, p_result);
    return rval;
}

static inline uint32 exe_calc_geo_settings_check(const ik_ability_t *p_ability, const ik_calc_geo_settings_t *p_info, const ik_calc_geo_result_t *p_result, const ik_query_calc_geo_buf_size_t *p_exptected_buf_size)
{
    uint32 rval = IK_OK;

    if ((null_check_op(p_ability)+null_check_op(p_info)+null_check_op(p_result)) != 0u) {
        rval = IK_ERR_0005;
        amba_ik_system_print_str_5("[IK][ERROR] p_ability or p_info or p_result == NULL\n", DC_S, DC_S, DC_S, DC_S, DC_S);
    } else {

        if ((null_check_op(p_info->p_in)+null_check_op(p_info->p_work_buf)+null_check_op(p_result->p_out)) != 0u) {
            rval = IK_ERR_0005;
            amba_ik_system_print_uint32_5("[IK][ERROR] p_info->p_in or p_info->p_work_buf or p_info->p_out is NULL\n", DC_U, DC_U, DC_U, DC_U, DC_U);
        } else {
            if (p_ability->pipe != AMBA_IK_PIPE_VIDEO) {
                amba_ik_system_print_uint32_5("[IK] Error, pipe %d is not support now\n", p_ability->pipe, DC_U, DC_U, DC_U, DC_U);
                rval = IK_ERR_0008;
            } else {
                rval |= exe_calc_geo_settings_basic_check(p_info, p_result);
                if (p_info->work_buf_size < p_exptected_buf_size->work_buf_size) {
                    rval = IK_ERR_0007;
                    amba_ik_system_print_uint32_5("[IK][ERROR] work_buf_size %d smaller than expected_work_buf_size %d\n", p_info->work_buf_size, p_exptected_buf_size->work_buf_size, DC_U, DC_U, DC_U);
                }

                if ((p_info->work_buf_size & 0x7FU) != 0u) {
                    rval = IK_ERR_0007;
                    amba_ik_system_print_uint32_5("[IK][ERROR] work_buf_size %d must be 128 alignment\n", p_info->work_buf_size, DC_U, DC_U, DC_U, DC_U);
                }

                if (p_result->out_buf_size < p_exptected_buf_size->out_buf_size) {
                    rval = IK_ERR_0007;
                    amba_ik_system_print_uint32_5("[IK][ERROR] out_buf_size %d smaller than expected_out_buf_size %d\n", p_result->out_buf_size, p_exptected_buf_size->out_buf_size, DC_U, DC_U, DC_U);
                }
            }
        }

    }

    return rval;
}

static inline void exe_calc_geo_settings_set_win_info(const ik_ability_t *p_ability,
        const ik_calc_geo_settings_t *p_info,
        amba_ik_win_calc_t *p_win_info,
        ik_stitch_info_t *p_stitching_info,
        amba_ik_calc_geo_work_buf_t *p_work_buf,
        ik_vin_active_window_t *p_act_win,
        ik_dummy_margin_range_t *p_dmy_range,
        ik_dzoom_info_t *p_dz_info)
{
    p_win_info->warp_enable = p_info->p_in->warp_enable;
    p_win_info->ability = p_ability->video_pipe;
    p_win_info->p_window_size_info = p_info->p_in->p_window_size_info;
    if(p_info->p_in->p_active_window != NULL) {
        p_win_info->p_active_window = p_info->p_in->p_active_window;
    } else {
        p_win_info->p_active_window = p_act_win;
    }
    if(p_info->p_in->p_dmy_range != NULL) {
        p_win_info->p_dmy_range = p_info->p_in->p_dmy_range;
    } else {
        p_win_info->p_dmy_range = p_dmy_range;
    }
    if(p_info->p_in->p_dzoom_info != NULL) {
        p_win_info->p_dzoom_info = p_info->p_in->p_dzoom_info;
    } else {
        p_win_info->p_dzoom_info = p_dz_info;
    }
    p_win_info->p_result_win = &p_work_buf->result_win;
    p_win_info->p_stitching_info = p_stitching_info;
}

static inline void exe_calc_geo_settings_set_aaa_info(const ik_ability_t *p_ability,
        const ik_calc_geo_settings_t *p_info,
        ikc_in_aaa_t *p_in_aaa,
        ikc_out_aaa_t *p_out_aaa,
        ik_stitch_info_t *p_stitching_info,
        amba_ik_calc_geo_work_buf_t *p_work_buf,
        amalgam_group_update_data_t *p_out,
        void *p_work_cr)
{
    static ik_vin_sensor_info_t      sensor_info = {0};
    static ik_rgb_ir_t rgb_ir = {0};
    static ik_aaa_stat_info_t        aaa_stat_info = {0};
    static ik_aaa_pg_af_stat_info_t  aaa_pg_af_stat_info = {0};
    static ik_af_stat_ex_info_t      af_stat_ex_info = {0};
    static ik_pg_af_stat_ex_info_t   pg_af_stat_ex_info = {0};
    static ik_histogram_info_t       hist_info = {0};
    static ik_histogram_info_t       hist_info_pg = {0};

    sensor_info.sensor_mode = 0;

    aaa_stat_info.awb_tile_num_col = 64UL;
    aaa_stat_info.awb_tile_num_row = 64UL;
    aaa_stat_info.awb_tile_col_start = 0UL;
    aaa_stat_info.awb_tile_row_start = 0UL;
    aaa_stat_info.awb_tile_width = 64UL;
    aaa_stat_info.awb_tile_height = 64UL;
    aaa_stat_info.awb_tile_active_width = 64UL;
    aaa_stat_info.awb_tile_active_height = 64UL;
    aaa_stat_info.awb_pix_min_value = 0UL;
    aaa_stat_info.awb_pix_max_value = 16383UL;

    aaa_stat_info.ae_tile_num_col = 24UL;
    aaa_stat_info.ae_tile_num_row = 16UL;
    aaa_stat_info.ae_tile_col_start = 0UL;
    aaa_stat_info.ae_tile_row_start = 0UL;
    aaa_stat_info.ae_tile_width = 171UL;
    aaa_stat_info.ae_tile_height = 256UL;
    aaa_stat_info.ae_pix_min_value = 0UL;
    aaa_stat_info.ae_pix_max_value = 16383UL;

    aaa_stat_info.af_tile_num_col = 24UL;
    aaa_stat_info.af_tile_num_row = 16UL;
    aaa_stat_info.af_tile_col_start = 0UL;
    aaa_stat_info.af_tile_row_start = 0UL;
    aaa_stat_info.af_tile_width = 171UL;
    aaa_stat_info.af_tile_height = 256UL;
    aaa_stat_info.af_tile_active_width = 171UL;
    aaa_stat_info.af_tile_active_height = 256UL;

    aaa_pg_af_stat_info.af_tile_num_col = 32UL;
    aaa_pg_af_stat_info.af_tile_num_row = 16UL;
    aaa_pg_af_stat_info.af_tile_col_start = 0UL;
    aaa_pg_af_stat_info.af_tile_row_start = 0UL;
    aaa_pg_af_stat_info.af_tile_width = 128UL;
    aaa_pg_af_stat_info.af_tile_height = 256UL;
    aaa_pg_af_stat_info.af_tile_active_width = 128UL;
    aaa_pg_af_stat_info.af_tile_active_height = 256UL;

    af_stat_ex_info.af_horizontal_filter1_stage1_enb = 1UL;
    af_stat_ex_info.af_horizontal_filter1_stage2_enb = 1UL;
    af_stat_ex_info.af_horizontal_filter1_stage3_enb = 1UL;
    af_stat_ex_info.af_horizontal_filter2_stage1_enb = 1UL;
    af_stat_ex_info.af_horizontal_filter2_stage2_enb = 1UL;
    af_stat_ex_info.af_horizontal_filter2_stage3_enb = 1UL;
    af_stat_ex_info.af_horizontal_filter2_thresh = 50UL;
    af_stat_ex_info.af_tile_fv1_horizontal_shift = 8UL;
    af_stat_ex_info.af_tile_fv2_horizontal_shift = 8UL;
    af_stat_ex_info.af_horizontal_filter1_gain[0] = 188;
    af_stat_ex_info.af_horizontal_filter1_gain[1] = 467;
    af_stat_ex_info.af_horizontal_filter1_gain[2] = -235;
    af_stat_ex_info.af_horizontal_filter1_gain[3] = 375;
    af_stat_ex_info.af_horizontal_filter1_gain[4] = -184;
    af_stat_ex_info.af_horizontal_filter1_gain[5] = 276;
    af_stat_ex_info.af_horizontal_filter1_gain[6] = -206;

    af_stat_ex_info.af_horizontal_filter1_shift[0] = 7;
    af_stat_ex_info.af_horizontal_filter1_shift[1] = 2;
    af_stat_ex_info.af_horizontal_filter1_shift[2] = 2;
    af_stat_ex_info.af_horizontal_filter1_shift[3] = 0;

    af_stat_ex_info.af_horizontal_filter2_gain[0] = 188;
    af_stat_ex_info.af_horizontal_filter2_gain[1] = 467;
    af_stat_ex_info.af_horizontal_filter2_gain[2] = -235;
    af_stat_ex_info.af_horizontal_filter2_gain[3] = 375;
    af_stat_ex_info.af_horizontal_filter2_gain[4] = -184;
    af_stat_ex_info.af_horizontal_filter2_gain[5] = 276;
    af_stat_ex_info.af_horizontal_filter2_gain[6] = -206;

    af_stat_ex_info.af_horizontal_filter2_shift[0] = 7;
    af_stat_ex_info.af_horizontal_filter2_shift[1] = 2;
    af_stat_ex_info.af_horizontal_filter2_shift[2] = 2;
    af_stat_ex_info.af_horizontal_filter2_shift[3] = 0;

    pg_af_stat_ex_info.af_horizontal_filter1_stage1_enb = 1UL;
    pg_af_stat_ex_info.af_horizontal_filter1_stage2_enb = 1UL;
    pg_af_stat_ex_info.af_horizontal_filter1_stage3_enb = 1UL;
    pg_af_stat_ex_info.af_horizontal_filter2_stage1_enb = 1UL;
    pg_af_stat_ex_info.af_horizontal_filter2_stage2_enb = 1UL;
    pg_af_stat_ex_info.af_horizontal_filter2_stage3_enb = 1UL;
    pg_af_stat_ex_info.af_vertical_filter1_thresh = 100UL;
    pg_af_stat_ex_info.af_tile_fv1_horizontal_shift = 7UL;
    pg_af_stat_ex_info.af_tile_fv1_horizontal_weight = 255UL;
    pg_af_stat_ex_info.af_vertical_filter2_thresh = 100UL;
    pg_af_stat_ex_info.af_tile_fv2_horizontal_shift = 7UL;
    pg_af_stat_ex_info.af_tile_fv2_horizontal_weight = 255UL;
    pg_af_stat_ex_info.af_horizontal_filter1_gain[0] = 188;
    pg_af_stat_ex_info.af_horizontal_filter1_gain[1] = 467;
    pg_af_stat_ex_info.af_horizontal_filter1_gain[2] = -235;
    pg_af_stat_ex_info.af_horizontal_filter1_gain[3] = 375;
    pg_af_stat_ex_info.af_horizontal_filter1_gain[4] = -184;
    pg_af_stat_ex_info.af_horizontal_filter1_gain[5] = 276;
    pg_af_stat_ex_info.af_horizontal_filter1_gain[6] = -206;

    pg_af_stat_ex_info.af_horizontal_filter1_shift[0] = 7;
    pg_af_stat_ex_info.af_horizontal_filter1_shift[1] = 2;
    pg_af_stat_ex_info.af_horizontal_filter1_shift[2] = 2;
    pg_af_stat_ex_info.af_horizontal_filter1_shift[3] = 0;

    pg_af_stat_ex_info.af_horizontal_filter2_gain[0] = 188;
    pg_af_stat_ex_info.af_horizontal_filter2_gain[1] = 467;
    pg_af_stat_ex_info.af_horizontal_filter2_gain[2] = -235;
    pg_af_stat_ex_info.af_horizontal_filter2_gain[3] = 375;
    pg_af_stat_ex_info.af_horizontal_filter2_gain[4] = -184;
    pg_af_stat_ex_info.af_horizontal_filter2_gain[5] = 276;
    pg_af_stat_ex_info.af_horizontal_filter2_gain[6] = -206;

    pg_af_stat_ex_info.af_horizontal_filter2_shift[0] = 7;
    pg_af_stat_ex_info.af_horizontal_filter2_shift[1] = 2;
    pg_af_stat_ex_info.af_horizontal_filter2_shift[2] = 2;
    pg_af_stat_ex_info.af_horizontal_filter2_shift[3] = 0;

    hist_info.ae_tile_mask[0] = 0xFFFFFF;
    hist_info.ae_tile_mask[1] = 0xFFFFFF;
    hist_info.ae_tile_mask[2] = 0xFFFFFF;
    hist_info.ae_tile_mask[3] = 0xFFFFFF;
    hist_info.ae_tile_mask[4] = 0xFFFFFF;
    hist_info.ae_tile_mask[5] = 0xFFFFFF;
    hist_info.ae_tile_mask[6] = 0xFFFFFF;
    hist_info.ae_tile_mask[7] = 0xFFFFFF;
    hist_info.ae_tile_mask[8] = 0xFFFFFF;
    hist_info.ae_tile_mask[9] = 0xFFFFFF;
    hist_info.ae_tile_mask[10] = 0xFFFFFF;
    hist_info.ae_tile_mask[11] = 0xFFFFFF;
    hist_info.ae_tile_mask[12] = 0xFFFFFF;
    hist_info.ae_tile_mask[13] = 0xFFFFFF;
    hist_info.ae_tile_mask[14] = 0xFFFFFF;
    hist_info.ae_tile_mask[15] = 0xFFFFFF;

    hist_info_pg.ae_tile_mask[0] = 0xFFFFFF;
    hist_info_pg.ae_tile_mask[1] = 0xFFFFFF;
    hist_info_pg.ae_tile_mask[2] = 0xFFFFFF;
    hist_info_pg.ae_tile_mask[3] = 0xFFFFFF;
    hist_info_pg.ae_tile_mask[4] = 0xFFFFFF;
    hist_info_pg.ae_tile_mask[5] = 0xFFFFFF;
    hist_info_pg.ae_tile_mask[6] = 0xFFFFFF;
    hist_info_pg.ae_tile_mask[7] = 0xFFFFFF;
    hist_info_pg.ae_tile_mask[8] = 0xFFFFFF;
    hist_info_pg.ae_tile_mask[9] = 0xFFFFFF;
    hist_info_pg.ae_tile_mask[10] = 0xFFFFFF;
    hist_info_pg.ae_tile_mask[11] = 0xFFFFFF;
    hist_info_pg.ae_tile_mask[12] = 0xFFFFFF;
    hist_info_pg.ae_tile_mask[13] = 0xFFFFFF;
    hist_info_pg.ae_tile_mask[14] = 0xFFFFFF;
    hist_info_pg.ae_tile_mask[15] = 0xFFFFFF;

    p_in_aaa->is_group_cmd = 1u;
    if(p_info->p_in->p_sensor_info != NULL) {
        p_in_aaa->p_sensor_info = p_info->p_in->p_sensor_info;//determine RGB-IR.
    } else {
        p_in_aaa->p_sensor_info = &sensor_info;
    }
    if(p_info->p_in->p_rgb_ir != NULL) {
        (void)amba_ik_system_memcpy(&rgb_ir, p_info->p_in->p_rgb_ir, sizeof(ik_rgb_ir_t));
        p_in_aaa->p_rgb_ir = &rgb_ir;
        if(p_in_aaa->p_rgb_ir->ir_only != 0U) {
            p_in_aaa->p_rgb_ir->mode = 0U;
        }
    } else {
        p_in_aaa->p_rgb_ir = &rgb_ir;
    }
    if(p_info->p_in->p_aaa_stat_info != NULL) {
        p_in_aaa->p_aaa_stat_info = p_info->p_in->p_aaa_stat_info;
    } else {
        p_in_aaa->p_aaa_stat_info = &aaa_stat_info;
    }
    if(p_info->p_in->p_aaa_pg_af_stat_info != NULL) {
        p_in_aaa->p_aaa_pg_af_stat_info = p_info->p_in->p_aaa_pg_af_stat_info;
    } else {
        p_in_aaa->p_aaa_pg_af_stat_info = &aaa_pg_af_stat_info;
    }
    if(p_info->p_in->p_af_stat_ex_info != NULL) {
        p_in_aaa->p_af_stat_ex_info= p_info->p_in->p_af_stat_ex_info;
    } else {
        p_in_aaa->p_af_stat_ex_info = &af_stat_ex_info;
    }
    if(p_info->p_in->p_pg_af_stat_ex_info != NULL) {
        p_in_aaa->p_pg_af_stat_ex_info = p_info->p_in->p_pg_af_stat_ex_info;
    } else {
        p_in_aaa->p_pg_af_stat_ex_info = &pg_af_stat_ex_info;
    }
    if(p_info->p_in->p_hist_info != NULL) {
        p_in_aaa->p_hist_info = p_info->p_in->p_hist_info;
    } else {
        p_in_aaa->p_hist_info = &hist_info;
    }
    if(p_info->p_in->p_hist_info_pg != NULL) {
        p_in_aaa->p_hist_info_pg = p_info->p_in->p_hist_info_pg;
    } else {
        p_in_aaa->p_hist_info_pg = &hist_info_pg;
    }
    p_in_aaa->p_logical_dmy_win_geo = &p_work_buf->result_win.logical_dmy_win_geo;
    p_in_aaa->p_cfa_win_dim = &p_work_buf->result_win.cfa_win_dim;
    p_in_aaa->p_main = &p_info->p_in->p_window_size_info->main_win;
    p_in_aaa->p_stitching_info = p_stitching_info;
    p_out_aaa->p_cr_9 = p_work_cr;
    p_out_aaa->cr_9_size = CR_SIZE_9;
    p_out_aaa->p_cr_12 = p_work_cr;
    p_out_aaa->cr_12_size = CR_SIZE_12;
    p_out_aaa->p_cr_21 = p_work_cr;
    p_out_aaa->cr_21_size = CR_SIZE_21;
    p_out_aaa->p_cr_28 = p_work_cr;
    p_out_aaa->cr_28_size = CR_SIZE_28;
    p_out_aaa->p_stitching_aaa = &p_out->group_update_info.aaa;

    if (p_ability->video_pipe != AMBA_IK_VIDEO_Y2Y) {
        p_in_aaa->is_yuv_mode = 0UL;
    } else {
        p_in_aaa->is_yuv_mode = 1UL;
    }
}

uint32 exe_calc_geo_settings(const ik_ability_t *p_ability, const ik_calc_geo_settings_t *p_info, const ik_calc_geo_result_t *p_result)
{
    uint32 rval = IK_OK;
    ik_query_calc_geo_buf_size_t exptected_buf_size;
    amba_ik_win_calc_t win_info = {0};
    ik_stitch_info_t stitching_info = {0};
    amba_ik_calc_geo_work_buf_t *p_work_buf = NULL;
    amalgam_group_update_data_t *p_out = NULL;
    void *p_work_cr = NULL;
    uint32 data_offset_base = 0;
    ik_vin_active_window_t act_win = {0};
    ik_dummy_margin_range_t dmy_range = {0};
    ik_dzoom_info_t dz_info = {0};

    (void)amba_ik_system_memset(&exptected_buf_size, 0, sizeof(ik_query_calc_geo_buf_size_t));//misra

    (void)exe_query_calc_geo_buf_size(&exptected_buf_size);
    rval |= exe_calc_geo_settings_check(p_ability, p_info, p_result, &exptected_buf_size);

    /* Parameters Check */
    if (rval == IK_OK) {
        /* Do things */
        (void)amba_ik_system_memcpy(&p_work_buf, &p_info->p_work_buf, sizeof(void *));
        (void)amba_ik_system_memcpy(&p_out, &p_result->p_out, sizeof(void *));
        p_work_cr = ptr2ptr(p_work_buf->CR_buf_42);
        (void)amba_ik_system_memcpy(&data_offset_base, &p_result->p_out, sizeof(uint32));

        exe_calc_geo_settings_set_win_info(p_ability, p_info, &win_info, &stitching_info, p_work_buf, &act_win, &dmy_range, &dz_info);
        rval |= exe_win_calc(&win_info);

        if (rval == IK_OK) {
            // default value
            stitching_info.tile_num_x = 1; // only support group cmd for frame mode
            stitching_info.tile_num_y = 1; // only support group cmd for frame mode

            if (p_ability->video_pipe != AMBA_IK_VIDEO_Y2Y) {
                // ca
                if (p_info->p_in->use_cawarp == 1) {
                    ikc_in_cawarp_t in_ca;
                    ikc_out_cawarp_t out_ca;

                    in_ca.is_group_cmd = 1u;
                    in_ca.cawarp_enable = (uint32)p_info->p_in->cawarp_enable;
                    in_ca.flip_mode = p_info->p_in->flip_mode;
                    in_ca.p_stitching_info = &stitching_info;
                    in_ca.p_window_size_info = p_info->p_in->p_window_size_info;
                    in_ca.p_result_win = &p_work_buf->result_win;
                    in_ca.p_calib_ca_warp_info = p_info->p_in->p_calib_ca_warp_info;

                    out_ca.p_cr_16 = p_work_cr;
                    out_ca.cr_16_size = CR_SIZE_16;
                    out_ca.p_cr_17 = p_work_cr;
                    out_ca.cr_17_size = CR_SIZE_17;
                    out_ca.p_cr_18 = p_work_cr;
                    out_ca.cr_18_size = CR_SIZE_18;
                    out_ca.p_cr_19 = p_work_cr;
                    out_ca.cr_19_size = CR_SIZE_19;
                    out_ca.p_cr_20 = p_work_cr;
                    out_ca.cr_20_size = CR_SIZE_20;
                    out_ca.p_calib = &p_out->group_update_info.calib;
                    out_ca.p_cawarp_hor_table_addr_red = ptr2ptr(p_out->ca_warp_hor_red);
                    out_ca.h_red_size = CAWARP_VIDEO_MAXSIZE;
                    out_ca.p_cawarp_vertical_table_addr_red = ptr2ptr(p_out->ca_warp_ver_red);
                    out_ca.v_red_size = CAWARP_VIDEO_MAXSIZE;
                    out_ca.p_cawarp_hor_table_addr_blue = ptr2ptr(p_out->ca_warp_hor_blue);
                    out_ca.h_blue_size = CAWARP_VIDEO_MAXSIZE;
                    out_ca.p_cawarp_vertical_table_addr_blue = ptr2ptr(p_out->ca_warp_ver_blue);
                    out_ca.v_blue_size = CAWARP_VIDEO_MAXSIZE;

                    // I suppose user calib table already remapped by comsvc.
                    rval |= ikc_cawarp(&in_ca, &out_ca);

                    p_out->group_update_info.calib.cawarp_horizontal_table_addr_red = physical2u32(amba_ik_system_virt_to_phys(p_out->ca_warp_hor_red));
                    p_out->group_update_info.calib.cawarp_horizontal_table_addr_blue = physical2u32(amba_ik_system_virt_to_phys(p_out->ca_warp_hor_blue));
                    p_out->group_update_info.calib.cawarp_vertical_table_addr_red = physical2u32(amba_ik_system_virt_to_phys(p_out->ca_warp_ver_red));
                    p_out->group_update_info.calib.cawarp_vertical_table_addr_blue = physical2u32(amba_ik_system_virt_to_phys(p_out->ca_warp_ver_blue));
                }
                p_out->group_update_info.calib.update_CA_warp = 1u;
            } else {
                p_out->group_update_info.calib.update_CA_warp = 0u;
            }
            {
                ikc_in_warp_t in_warp;
                ikc_out_warp_t out_warp;

                in_warp.is_group_cmd = 1u;
                in_warp.warp_enable = (uint32)p_info->p_in->warp_enable;
                in_warp.flip_mode = p_info->p_in->flip_mode;
                in_warp.p_window_size_info = p_info->p_in->p_window_size_info;
                in_warp.p_act_win_crop = &p_work_buf->result_win.act_win_crop;
                in_warp.p_dmy_win_geo = &p_work_buf->result_win.dmy_win_geo;
                in_warp.p_cfa_win_dim = &p_work_buf->result_win.cfa_win_dim;
                if(p_info->p_in->p_dmy_range != NULL) {
                    in_warp.p_dmy_range = p_info->p_in->p_dmy_range;
                } else {
                    in_warp.p_dmy_range = &dmy_range;
                }
                in_warp.p_stitching_info = &stitching_info;
                in_warp.p_calib_warp_info = p_info->p_in->p_calib_warp_info;
                if(p_info->p_in->p_warp_buf_info != NULL) {
                    in_warp.dram_efficiency = p_info->p_in->p_warp_buf_info->dram_efficiency;
                } else {
                    in_warp.dram_efficiency = 0;
                }
                in_warp.force_vscale_sec_n = 0;

                out_warp.p_cr_33 = p_work_cr;
                out_warp.cr_33_size = CR_SIZE_33;
                out_warp.p_cr_34 = p_work_cr;
                out_warp.cr_34_size = CR_SIZE_34;
                out_warp.p_cr_35 = p_work_cr;
                out_warp.cr_35_size = CR_SIZE_35;
                out_warp.p_cr_42 = p_work_cr;
                out_warp.cr_42_size = CR_SIZE_42;
                out_warp.p_cr_43 = p_work_cr;
                out_warp.cr_43_size = CR_SIZE_43;
                out_warp.p_cr_44 = p_work_cr;
                out_warp.cr_44_size = CR_SIZE_44;
                out_warp.p_flow_info = &p_work_buf->flow_info;
                out_warp.p_window = &p_out->group_update_info.window;
                out_warp.p_phase = &p_out->group_update_info.phase;
                out_warp.p_calib = &p_out->group_update_info.calib;
                out_warp.p_stitch = &p_work_buf->stitch;
                out_warp.p_warp_horizontal_table_address = ptr2ptr(p_out->warp_hor);
                out_warp.h_size = WARP_VIDEO_MAXHORGRID * WARP_VIDEO_MAXVERGRID * 2U * 2U;// multi_slice buffer.
                out_warp.p_warp_vertical_table_address = ptr2ptr(p_out->warp_ver);
                out_warp.v_size = WARP_VIDEO_MAXHORGRID * WARP_VIDEO_MAXVERGRID * 2U * 2U;// multi_slice buffer.

                // I suppose user calib table already remapped by comsvc.
                rval |= ikc_warp(&in_warp, &out_warp);
            }
            {
                ikc_in_warp_radius_t in_warp_radius;
                ikc_out_warp_radius_t out_warp_radius;

                in_warp_radius.is_group_cmd = 1u;
                in_warp_radius.chroma_radius = p_info->p_in->chroma_filter_radius;
                in_warp_radius.is_y2y_420input = equal_op_u32(p_info->p_in->yuv_mode, AMBA_IK_YUV420);
                //warp_buf_info.luma_wait_lines = 8u;
                //warp_buf_info.luma_dma_size = 32u;
                if(p_info->p_in->p_warp_buf_info != NULL) {
                    in_warp_radius.dram_efficiency = p_info->p_in->p_warp_buf_info->dram_efficiency;
                    in_warp_radius.luma_wait_lines = p_info->p_in->p_warp_buf_info->luma_wait_lines;
                    in_warp_radius.luma_dma_size = p_info->p_in->p_warp_buf_info->luma_dma_size;
#if defined(EARLYTEST_ENV)
                    in_warp_radius.chroma_dma_size = p_info->p_in->p_warp_buf_info->chroma_dma_size;
#endif
                } else {
                    in_warp_radius.dram_efficiency = 0;
                    in_warp_radius.luma_wait_lines = 8;
                    in_warp_radius.luma_dma_size = 32;
                }

                out_warp_radius.p_cr_35 = p_work_cr;
                out_warp_radius.cr_35_size = CR_SIZE_35;
                out_warp_radius.p_cr_42 = p_work_cr;
                out_warp_radius.cr_42_size = CR_SIZE_42;
                out_warp_radius.p_window = &p_out->group_update_info.window;
                out_warp_radius.p_calib = &p_out->group_update_info.calib;

                rval |= ikc_warp_radius(&in_warp_radius, &out_warp_radius);
            }
            {
                ikc_in_chroma_noise_t in_chroma_noise = {0};
                ikc_out_chroma_noise_t out_chroma_noise = {0};

                ik_chroma_filter_t chroma_filter = {0};
                ik_wide_chroma_filter_t wide_chroma_filter = {0};
                ik_wide_chroma_filter_combine_t wide_chroma_combine = {0};
                chroma_filter.radius = p_info->p_in->chroma_filter_radius;

                in_chroma_noise.is_group_cmd = 1u;
                in_chroma_noise.p_chroma_filter = &chroma_filter;
                in_chroma_noise.p_wide_chroma_filter = &wide_chroma_filter;
                in_chroma_noise.p_wide_chroma_combine = &wide_chroma_combine;
                out_chroma_noise.p_cr_36 = p_work_cr;
                out_chroma_noise.cr_36_size = CR_SIZE_36;
                out_chroma_noise.p_window = &p_out->group_update_info.window;

                rval |= ikc_chroma_noise(&in_chroma_noise, &out_chroma_noise);
            }

            p_out->group_update_info.calib.warp_horizontal_table_address = physical2u32(amba_ik_system_virt_to_phys(p_out->warp_hor));
            p_out->group_update_info.calib.warp_vertical_table_address = physical2u32(amba_ik_system_virt_to_phys(p_out->warp_ver));

            p_out->group_update_info.calib.update_lens_warp = 1u;
            p_out->group_update_info.calib_update = 1u;
            p_out->group_update_info.window.update_chroma_radius = 1u;
            p_out->group_update_info.window_info_update = 1u;
            p_out->group_update_info.phase_info_update = 1u;

            // aaa_cfa
            {
                ikc_in_aaa_t in_aaa;
                ikc_out_aaa_t out_aaa;

                exe_calc_geo_settings_set_aaa_info(p_ability, p_info, &in_aaa, &out_aaa, &stitching_info, p_work_buf, p_out, p_work_cr);
                rval |= ikc_aaa(&in_aaa, &out_aaa);
            }

            p_out->group_update_info.aaa_stats_update = 1u;
        }

#ifdef EARLYTEST_ENV
        /* amalgam relevant address */
        if (p_out->group_update_info.calib.cawarp_horizontal_table_addr_red != 0UL) {
            p_out->group_update_info.calib.cawarp_horizontal_table_addr_red -= data_offset_base;
        } else {
            amba_ik_system_print_str_5("[IK] cawarp_horizontal_table_addr_red is NULL", NULL, NULL, NULL, NULL, NULL);
        }

        if (p_out->group_update_info.calib.cawarp_horizontal_table_addr_blue != 0UL) {
            p_out->group_update_info.calib.cawarp_horizontal_table_addr_blue -= data_offset_base;
        } else {
            amba_ik_system_print_str_5("[IK] cawarp_horizontal_table_addr_blue is NULL", NULL, NULL, NULL, NULL, NULL);
        }

        if (p_out->group_update_info.calib.cawarp_vertical_table_addr_red != 0UL) {
            p_out->group_update_info.calib.cawarp_vertical_table_addr_red -= data_offset_base;
        } else {
            amba_ik_system_print_str_5("[IK] cawarp_vertical_table_addr_red is NULL", NULL, NULL, NULL, NULL, NULL);
        }

        if (p_out->group_update_info.calib.cawarp_vertical_table_addr_blue != 0UL) {
            p_out->group_update_info.calib.cawarp_vertical_table_addr_blue -= data_offset_base;
        } else {
            amba_ik_system_print_str_5("[IK] cawarp_vertical_table_addr_blue is NULL", NULL, NULL, NULL, NULL, NULL);
        }

        p_out->group_update_info.calib.warp_horizontal_table_address -= data_offset_base;
        p_out->group_update_info.calib.warp_vertical_table_address -= data_offset_base;

        //never happens below...
        /*if (p_out->group_update_info.calib.warp_horizontal_table_address != 0UL) {
            p_out->group_update_info.calib.warp_horizontal_table_address -= data_offset_base;
        } else {
            amba_ik_system_print_str_5("[IK] warp_horizontal_table_address is NULL", NULL, NULL, NULL, NULL, NULL);
        }

        if (p_out->group_update_info.calib.warp_vertical_table_address != 0UL) {
            p_out->group_update_info.calib.warp_vertical_table_address -= data_offset_base;
        } else {
            amba_ik_system_print_str_5("[IK] warp_vertical_table_address is NULL", NULL, NULL, NULL, NULL, NULL);
        }*/
#endif

        /* Clean out cache */
        if (rval == IK_OK) {
            amba_ik_system_clean_cache(p_result->p_out, exptected_buf_size.out_buf_size);
        }
    }

    return rval;

}

uint32 exe_warp_remap(const ik_in_warp_remap_t *p_warp_remap_in, ik_out_warp_remap_t *p_warp_remap_out)
{
    const ikc_in_warp_remap_t *p_ikc_warp_remap_in = p_warp_remap_in;
    ikc_out_warp_remap_t *p_ikc_warp_remap_out = p_warp_remap_out;

    return ikc_warp_remap(p_ikc_warp_remap_in, p_ikc_warp_remap_out);
}

uint32 exe_cawarp_remap(const ik_in_cawarp_remap_t *p_cawarp_remap_in, ik_out_cawarp_remap_t *p_cawarp_remap_out)
{
    const ikc_in_cawarp_remap_t *p_ikc_cawarp_remap_in = p_cawarp_remap_in;
    ikc_out_cawarp_remap_t *p_ikc_cawarp_remap_out = p_cawarp_remap_out;

    return ikc_cawarp_remap(p_ikc_cawarp_remap_in, p_ikc_cawarp_remap_out);
}

