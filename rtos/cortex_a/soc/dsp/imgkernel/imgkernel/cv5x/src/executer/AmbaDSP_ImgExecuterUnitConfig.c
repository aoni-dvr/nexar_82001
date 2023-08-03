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

static inline uint8 equal_op_u32(uint32 compare1, uint32 compare2)
{
    return (compare1 == compare2) ? 1U : 0U;
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
        if (p_filters->input_param.rgb_ir.mode != 0U) {
            amba_ik_system_print_uint32_5("[IK][ERROR] Incorrect sensor_mode %d or rgb_ir.mode %d !", \
                                          p_sensor_info->sensor_mode, p_filters->input_param.rgb_ir.mode, DC_U, DC_U, DC_U);
            rval |= IK_ERR_0008;
        }
    } else if (p_sensor_info->sensor_mode == IK_SENSOR_MODE_RGBIR) {
        if (p_filters->input_param.rgb_ir.mode == 0U) {
            amba_ik_system_print_uint32_5("[IK][ERROR] Incorrect sensor_mode %d or rgb_ir.mode %d !", \
                                          p_sensor_info->sensor_mode, p_filters->input_param.rgb_ir.mode, DC_U, DC_U, DC_U);
            rval |= IK_ERR_0008;
        }
    } else {
        if (p_filters->input_param.rgb_ir.mode != 0U) {
            amba_ik_system_print_uint32_5("[IK][ERROR] Incorrect sensor_mode %d or rgb_ir.mode %d !", \
                                          p_sensor_info->sensor_mode, p_filters->input_param.rgb_ir.mode, DC_U, DC_U, DC_U);
            rval |= IK_ERR_0008;
        }
    }

    return rval;
}

uint32 exe_process_calib_cr(idsp_flow_ctrl_t *p_flow, void *p_flow_tbl_list, amba_ik_filter_t *p_filters)
{
    uint32 rval = IK_OK;
    uint32 window_calculate_updated = 0U;
    amba_ik_flow_tables_list_t *p_flow_tables_list = NULL;
    amba_ik_motion_fusion_flow_tables_list_t *p_motion_fusion_flow_tables_list = NULL;
    const amba_ik_r2y_flow_tables_list_t *p_r2y = NULL;
    uint32 video_pipe = p_filters->input_param.ability;
    void *p_ca_warp_hor_red = NULL;
    void *p_ca_warp_hor_blue = NULL;
    void *p_ca_warp_ver_red = NULL;
    void *p_ca_warp_ver_blue = NULL;
    void *p_warp_hor = NULL;
    void *p_warp_ver = NULL;
    void *p_warp_hor_b = NULL;

    if ((video_pipe == AMBA_IK_VIDEO_PIPE_LINEAR) ||
        (video_pipe == AMBA_IK_VIDEO_PIPE_LINEAR_CE) ||
        (video_pipe == AMBA_IK_VIDEO_PIPE_HDR_EXPO_2) ||
        (video_pipe == AMBA_IK_VIDEO_PIPE_HDR_EXPO_3) ||
        (video_pipe == AMBA_IK_VIDEO_PIPE_Y2Y) ||
        (video_pipe == AMBA_IK_VIDEO_PIPE_LINEAR_HVH)||
        (video_pipe == AMBA_IK_VIDEO_PIPE_LINEAR_CE_HVH) ||
        (video_pipe == AMBA_IK_VIDEO_PIPE_HDR_EXPO_2_HVH) ||
        (video_pipe == AMBA_IK_VIDEO_PIPE_HDR_EXPO_3_HVH) ||
        (video_pipe == AMBA_IK_VIDEO_PIPE_Y2Y_HVH) ||
        (video_pipe == AMBA_IK_VIDEO_PIPE_LINEAR_HHB)||
        (video_pipe == AMBA_IK_VIDEO_PIPE_LINEAR_CE_HHB) ||
        (video_pipe == AMBA_IK_VIDEO_PIPE_HDR_EXPO_2_HHB) ||
        (video_pipe == AMBA_IK_VIDEO_PIPE_HDR_EXPO_3_HHB) ||
        (video_pipe == AMBA_IK_VIDEO_PIPE_Y2Y_HHB) ||
        (video_pipe == AMBA_IK_STILL_PIPE_LISO)) {
        (void)amba_ik_system_memcpy(&p_flow_tables_list, &p_flow_tbl_list, sizeof(amba_ik_flow_tables_list_t*));
        p_r2y = &p_flow_tables_list->r2y;
        p_ca_warp_hor_red = p_flow_tables_list->p_ca_warp_hor_red;
        p_ca_warp_hor_blue = p_flow_tables_list->p_ca_warp_hor_blue;
        p_ca_warp_ver_red = p_flow_tables_list->p_ca_warp_ver_red;
        p_ca_warp_ver_blue = p_flow_tables_list->p_ca_warp_ver_blue ;
        p_warp_hor = p_flow_tables_list->p_warp_hor;
        p_warp_ver = p_flow_tables_list->p_warp_ver;
        p_warp_hor_b = p_flow_tables_list->p_warp_hor_b;
    } else if ((video_pipe == AMBA_IK_VIDEO_PIPE_LINEAR_MD) ||
               (video_pipe == AMBA_IK_VIDEO_PIPE_LINEAR_CE_MD) ||
               (video_pipe == AMBA_IK_VIDEO_PIPE_HDR_EXPO_2_MD) ||
               (video_pipe == AMBA_IK_VIDEO_PIPE_HDR_EXPO_3_MD) ||
               (video_pipe == AMBA_IK_VIDEO_PIPE_FUSION) ||
               (video_pipe == AMBA_IK_VIDEO_PIPE_Y2Y_MD) ||
               (video_pipe == AMBA_IK_VIDEO_PIPE_LINEAR_MD_HHB) ||
               (video_pipe == AMBA_IK_VIDEO_PIPE_LINEAR_CE_MD_HHB) ||
               (video_pipe == AMBA_IK_VIDEO_PIPE_HDR_EXPO_2_MD_HHB) ||
               (video_pipe == AMBA_IK_VIDEO_PIPE_HDR_EXPO_3_MD_HHB) ||
               (video_pipe == AMBA_IK_VIDEO_PIPE_Y2Y_MD_HHB)) {
        (void)amba_ik_system_memcpy(&p_motion_fusion_flow_tables_list, &p_flow_tbl_list, sizeof(amba_ik_motion_fusion_flow_tables_list_t*));
        p_r2y = &p_motion_fusion_flow_tables_list->r2y;
        p_ca_warp_hor_red = p_motion_fusion_flow_tables_list->p_ca_warp_hor_red;
        p_ca_warp_hor_blue = p_motion_fusion_flow_tables_list->p_ca_warp_hor_blue;
        p_ca_warp_ver_red = p_motion_fusion_flow_tables_list->p_ca_warp_ver_red;
        p_ca_warp_ver_blue = p_motion_fusion_flow_tables_list->p_ca_warp_ver_blue ;
        p_warp_hor = p_motion_fusion_flow_tables_list->p_warp_hor;
        p_warp_ver = p_motion_fusion_flow_tables_list->p_warp_ver;
        p_warp_hor_b = p_motion_fusion_flow_tables_list->p_warp_hor_b;
    } else {
        // misraC
    }

    (void)amba_ik_system_memcpy(p_flow, p_flow, sizeof(uint8));//misra fix.

    if ((p_filters->update_flags.iso.window_size_info_updated == 1U) ||
        (p_filters->update_flags.iso.cfa_window_size_info_updated == 1U) ||
        (p_filters->update_flags.iso.dmy_range_updated == 1U) ||
        (p_filters->update_flags.iso.dzoom_info_updated == 1U) ||
        (p_filters->update_flags.iso.vin_active_win_updated == 1U) ||
        (p_filters->update_flags.iso.stitching_info_updated == 1U) ||
        (p_filters->update_flags.iso.overlap_x_updated == 1U) ||
        (p_filters->update_flags.iso.flip_mode_updated == 1U) ||
        (p_filters->update_flags.iso.warp_enable_updated == 1U) ||
        (p_filters->update_flags.iso.warp_enable_2nd_updated == 1U)) {
        window_calculate_updated = 1U;
    }

    // input mode
    if(p_filters->update_flags.iso.is_1st_frame == 1U) { //should only configure once at 1st time.
        ikc_in_input_mode_t in_input_mode;
        ikc_out_input_mode_t out_input_mode;

        rval |= exe_check_vin_sensor(p_filters);
        if((p_filters->update_flags.cr.r2y.cr_4_update == 0U)||
           (p_filters->update_flags.cr.r2y.cr_6_update == 0U)||
           (p_filters->update_flags.cr.r2y.cr_7_update == 0U)||
           (p_filters->update_flags.cr.r2y.cr_8_update == 0U)||
           (p_filters->update_flags.cr.r2y.cr_9_update == 0U)||
           (p_filters->update_flags.cr.r2y.cr_11_update == 0U)||
           (p_filters->update_flags.cr.r2y.cr_12_update == 0U)||
           (p_filters->update_flags.cr.r2y.cr_13_update == 0U)||
           (p_filters->update_flags.cr.r2y.cr_16_update == 0U)||
           (p_filters->update_flags.cr.r2y.cr_21_update == 0U)||
           (p_filters->update_flags.cr.r2y.cr_22_update == 0U)||
           (p_filters->update_flags.cr.r2y.cr_23_update == 0U)||
           (p_filters->update_flags.cr.r2y.cr_24_update == 0U)||
           (p_filters->update_flags.cr.r2y.cr_26_update == 0U)||
           (p_filters->update_flags.cr.r2y.cr_29_update == 0U)||
           (p_filters->update_flags.cr.r2y.cr_30_update == 0U)||
           (p_filters->update_flags.cr.r2y.cr_31_update == 0U)||
           (p_filters->update_flags.cr.r2y.cr_32_update == 0U)||
           (p_filters->update_flags.cr.r2y.cr_117_update == 0U)) {
            amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_input_mode !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
            rval |= IK_ERR_0000;
        } else {
            in_input_mode.flip_mode = p_filters->input_param.flip_mode;
            in_input_mode.p_sensor_info = &p_filters->input_param.sensor_info;
            out_input_mode.p_cr_4 = p_r2y->p_CR_buf_4;
            out_input_mode.cr_4_size = CR_SIZE_4;
            out_input_mode.p_cr_6 = p_r2y->p_CR_buf_6;
            out_input_mode.cr_6_size = CR_SIZE_6;
            out_input_mode.p_cr_7 = p_r2y->p_CR_buf_7;
            out_input_mode.cr_7_size = CR_SIZE_7;
            out_input_mode.p_cr_8 = p_r2y->p_CR_buf_8;
            out_input_mode.cr_8_size = CR_SIZE_8;
            out_input_mode.p_cr_9 = p_r2y->p_CR_buf_9;
            out_input_mode.cr_9_size = CR_SIZE_9;
            out_input_mode.p_cr_11 = p_r2y->p_CR_buf_11;
            out_input_mode.cr_11_size = CR_SIZE_11;
            out_input_mode.p_cr_12 = p_r2y->p_CR_buf_12;
            out_input_mode.cr_12_size = CR_SIZE_12;
            out_input_mode.p_cr_13 = p_r2y->p_CR_buf_13;
            out_input_mode.cr_13_size = CR_SIZE_13;
            out_input_mode.p_cr_16 = p_r2y->p_CR_buf_16;
            out_input_mode.cr_16_size = CR_SIZE_16;
            out_input_mode.p_cr_21 = p_r2y->p_CR_buf_21;
            out_input_mode.cr_21_size = CR_SIZE_21;
            out_input_mode.p_cr_22 = p_r2y->p_CR_buf_22;
            out_input_mode.cr_22_size = CR_SIZE_22;
            out_input_mode.p_cr_23 = p_r2y->p_CR_buf_23;
            out_input_mode.cr_23_size = CR_SIZE_23;
            out_input_mode.p_cr_24 = p_r2y->p_CR_buf_24;
            out_input_mode.cr_24_size = CR_SIZE_24;
            out_input_mode.p_cr_26 = p_r2y->p_CR_buf_26;
            out_input_mode.cr_26_size = CR_SIZE_26;
            out_input_mode.p_cr_29 = p_r2y->p_CR_buf_29;
            out_input_mode.cr_29_size = CR_SIZE_29;
            out_input_mode.p_cr_30 = p_r2y->p_CR_buf_30;
            out_input_mode.cr_30_size = CR_SIZE_30;
            out_input_mode.p_cr_31 = p_r2y->p_CR_buf_31;
            out_input_mode.cr_31_size = CR_SIZE_31;
            out_input_mode.p_cr_32 = p_r2y->p_CR_buf_32;
            out_input_mode.cr_32_size = CR_SIZE_32;
            out_input_mode.p_cr_117 = p_r2y->p_CR_buf_117;
            out_input_mode.cr_117_size = CR_SIZE_117;
            out_input_mode.p_flow = &p_flow->flow_info;
            out_input_mode.p_calib = &p_flow->calib;

            if ((p_filters->input_param.ability == AMBA_IK_VIDEO_PIPE_Y2Y) ||
                (p_filters->input_param.ability == AMBA_IK_VIDEO_PIPE_Y2Y_HVH) ||
                (p_filters->input_param.ability == AMBA_IK_VIDEO_PIPE_Y2Y_HHB) ||
                (p_filters->input_param.ability == AMBA_IK_VIDEO_PIPE_Y2Y_MD) ||
                (p_filters->input_param.ability == AMBA_IK_VIDEO_PIPE_Y2Y_MD_HHB)) {
                in_input_mode.is_yuv_mode = 1U;
            } else {
                in_input_mode.is_yuv_mode = 0U;
            }
            rval |= ikc_input_mode(&in_input_mode, &out_input_mode);
        }
    }

    if (window_calculate_updated == 1U) {
        rval |= exe_win_calc_wrapper(p_filters);
    }

    if ((p_filters->input_param.ability != AMBA_IK_VIDEO_PIPE_Y2Y) &&
        (p_filters->input_param.ability != AMBA_IK_VIDEO_PIPE_Y2Y_HVH) &&
        (p_filters->input_param.ability != AMBA_IK_VIDEO_PIPE_Y2Y_HHB) &&
        (p_filters->input_param.ability != AMBA_IK_VIDEO_PIPE_Y2Y_MD) &&
        (p_filters->input_param.ability != AMBA_IK_VIDEO_PIPE_Y2Y_MD_HHB)) {
        // sbp
        if((p_filters->update_flags.iso.static_bpc_updated == 1u) || (p_filters->update_flags.iso.static_bpc_internal_updated == 1u) ||
           (p_filters->update_flags.iso.dynamic_bpc_updated == 1u) || (p_filters->update_flags.iso.window_size_info_updated == 1U)) {
            if(p_filters->input_param.ctx_buf.sbp_internal_mode_flag == 1u) {
                ikc_in_static_bad_pixel_internal_t in_sbp_internal;
                ikc_out_static_bad_pixel_internal_t out_sbp;
                ik_static_bad_pixel_correction_internal_t sbp_internal_tmp;
                const void *void_ptr;

                if(p_filters->update_flags.cr.r2y.cr_12_update == 0U) {
                    amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_static_bad_pixel_internal !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
                    rval |= IK_ERR_0000;
                } else {
                    //consider 64 bit compiler, flow data sbp addr(u32) goes on physical addr + offset.
                    (void)amba_ik_system_memcpy(&sbp_internal_tmp,&p_filters->input_param.sbp_internal,sizeof(ik_static_bad_pixel_correction_internal_t));
                    void_ptr = amba_ik_system_virt_to_phys(sbp_internal_tmp.p_map);
                    (void)amba_ik_system_memcpy(&sbp_internal_tmp.p_map, &void_ptr, sizeof(void*));
                    in_sbp_internal.dbp_enable = p_filters->input_param.dynamic_bpc.enable;
                    in_sbp_internal.p_sbp_internal = &sbp_internal_tmp;
                    out_sbp.p_cr_12 = p_r2y->p_CR_buf_12;
                    out_sbp.cr_12_size = CR_SIZE_12;
                    out_sbp.p_flow = &p_flow->flow_info;
                    rval |= ikc_static_bad_pixel_internal(&in_sbp_internal, &out_sbp);
                }
            } else {
                ikc_in_static_bad_pixel_t in_sbp;
                ikc_out_static_bad_pixel_t out_sbp;
                ik_static_bad_pxl_cor_t static_bpc_tmp;
                const void *void_ptr;

                if(p_filters->update_flags.cr.r2y.cr_12_update == 0U) {
                    amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_static_bad_pixel !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
                    rval |= IK_ERR_0000;
                } else {
                    //consider 64 bit compiler, flow data sbp addr(u32) goes on physical addr + offset.
                    (void)amba_ik_system_memcpy(&static_bpc_tmp,&p_filters->input_param.static_bpc,sizeof(ik_static_bad_pxl_cor_t));
                    void_ptr = amba_ik_system_virt_to_phys(static_bpc_tmp.calib_sbp_info.sbp_buffer);
                    (void)amba_ik_system_memcpy(&static_bpc_tmp.calib_sbp_info.sbp_buffer, &void_ptr, sizeof(void*));
                    in_sbp.dbp_enable = p_filters->input_param.dynamic_bpc.enable;
                    in_sbp.sbp_enable = p_filters->input_param.sbp_enable;
                    in_sbp.p_static_bpc = &static_bpc_tmp;
                    in_sbp.p_vin_sensor = &p_filters->input_param.window_size_info.vin_sensor;
                    out_sbp.p_cr_12 = p_r2y->p_CR_buf_12;
                    out_sbp.cr_12_size = CR_SIZE_12;
                    out_sbp.p_flow = &p_flow->flow_info;
                    rval |= ikc_static_bad_pixel(&in_sbp, &out_sbp);
                }
            }
            p_flow->flow_info.update_sbp = 1U;
        } else {
            p_flow->flow_info.update_sbp = 0U;
        }

        // vig
        if ((p_filters->update_flags.iso.vignette_compensation_updated == 1U)||
            (p_filters->update_flags.iso.window_size_info_updated == 1U) || (window_calculate_updated == 1U)) {
            ikc_in_vignette_t in_vig;
            ikc_out_vignette_t out_vig;

            if((p_filters->update_flags.cr.r2y.cr_9_update == 0U)||
               (p_filters->update_flags.cr.r2y.cr_10_update == 0U)||
               (p_filters->update_flags.cr.r2y.cr_11_update == 0U)) {
                amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_vignette !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
                rval |= IK_ERR_0000;
            } else {
                in_vig.vig_enable = p_filters->input_param.vig_enable;
                in_vig.flip_mode = p_filters->input_param.flip_mode;
                in_vig.p_vignette_compensation = &p_filters->input_param.vignette_compensation;
                in_vig.p_vin_sensor = &p_filters->input_param.window_size_info.vin_sensor;
                in_vig.p_dmy_win_geo = &p_filters->input_param.ctx_buf.result_win.dmy_win_geo;
                in_vig.p_active_window = &p_filters->input_param.active_window;
                out_vig.p_cr_9 = p_r2y->p_CR_buf_9;
                out_vig.cr_9_size = CR_SIZE_9;
                out_vig.p_cr_10 = p_r2y->p_CR_buf_10;
                out_vig.cr_10_size = CR_SIZE_10;
                out_vig.p_cr_11 = p_r2y->p_CR_buf_11;
                out_vig.cr_11_size = CR_SIZE_11;
                out_vig.p_flow = &p_flow->flow_info;
                rval |= ikc_vignette(&in_vig, &out_vig);
            }
            p_flow->flow_info.update_vignette = 1U;
        } else {
            p_flow->flow_info.update_vignette = 0U;
        }

        // ca
        if ((p_filters->update_flags.iso.calib_ca_warp_info_updated == 1U) || (p_filters->update_flags.iso.cawarp_enable_updated == 1U) ||
            (p_filters->update_flags.iso.cawarp_internal_updated == 1U) || (p_filters->update_flags.iso.window_size_info_updated == 1U) ||
            (window_calculate_updated == 1U)) {
            if (p_filters->input_param.ctx_buf.cawarp_internal_mode_flag == 1U) {
                ikc_in_cawarp_internal_t in_ca;
                ikc_out_cawarp_t out_ca;

                if((p_filters->update_flags.cr.cawarp == 0U)||
                   (p_filters->update_flags.cr.r2y.cr_16_update == 0U)||
                   (p_filters->update_flags.cr.r2y.cr_17_update == 0U)) {
                    amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_cawarp_internal !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
                    rval |= IK_ERR_0000;
                } else {
                    in_ca.p_stitching_info = &p_filters->input_param.stitching_info;
                    in_ca.p_ca_warp_internal = &p_filters->input_param.ca_warp_internal;
                    out_ca.p_cr_16 = p_r2y->p_CR_buf_16;
                    out_ca.cr_16_size = CR_SIZE_16;
                    out_ca.p_cr_17 = p_r2y->p_CR_buf_17;
                    out_ca.cr_17_size = CR_SIZE_17;
                    out_ca.p_cr_18 = p_r2y->p_CR_buf_18;
                    out_ca.cr_18_size = CR_SIZE_18;
                    out_ca.p_cr_19 = p_r2y->p_CR_buf_19;
                    out_ca.cr_19_size = CR_SIZE_19;
                    out_ca.p_cr_20 = p_r2y->p_CR_buf_20;
                    out_ca.cr_20_size = CR_SIZE_20;
                    out_ca.p_calib = &p_flow->calib;
                    out_ca.p_cawarp_hor_table_addr_red = p_ca_warp_hor_red;
                    out_ca.h_red_size = CAWARP_VIDEO_MAXSIZE;
                    out_ca.p_cawarp_vertical_table_addr_red = p_ca_warp_ver_red;
                    out_ca.v_red_size = CAWARP_VIDEO_MAXSIZE;
                    out_ca.p_cawarp_hor_table_addr_blue = p_ca_warp_hor_blue;
                    out_ca.h_blue_size = CAWARP_VIDEO_MAXSIZE;
                    out_ca.p_cawarp_vertical_table_addr_blue = p_ca_warp_ver_blue;
                    out_ca.v_blue_size = CAWARP_VIDEO_MAXSIZE;
                    rval |= ikc_cawarp_internal(&in_ca, &out_ca);

                    p_flow->calib.cawarp_horizontal_table_addr_red = physical2u32(p_ca_warp_hor_red);
                    p_flow->calib.cawarp_horizontal_table_addr_blue = physical2u32(p_ca_warp_hor_blue);
                    p_flow->calib.cawarp_vertical_table_addr_red = physical2u32(p_ca_warp_ver_red);
                    p_flow->calib.cawarp_vertical_table_addr_blue = physical2u32(p_ca_warp_ver_blue);
                }
            } else {
                ikc_in_cawarp_t in_ca;
                ikc_out_cawarp_t out_ca;

                if((p_filters->update_flags.cr.cawarp == 0U)||
                   (p_filters->update_flags.cr.r2y.cr_16_update == 0U)||
                   (p_filters->update_flags.cr.r2y.cr_17_update == 0U)) {
                    amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_cawarp !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
                    rval |= IK_ERR_0000;
                } else {
                    in_ca.is_group_cmd = 0u;
                    in_ca.is_hiso      = 0u;
                    in_ca.cawarp_enable = (uint32)p_filters->input_param.cawarp_enable;
                    in_ca.flip_mode = p_filters->input_param.flip_mode;
                    in_ca.p_stitching_info = &p_filters->input_param.stitching_info;
                    in_ca.p_window_size_info = &p_filters->input_param.window_size_info;
                    in_ca.p_result_win = &p_filters->input_param.ctx_buf.result_win;
                    in_ca.p_calib_ca_warp_info = &p_filters->input_param.calib_ca_warp_info;
                    out_ca.p_cr_16 = p_r2y->p_CR_buf_16;
                    out_ca.cr_16_size = CR_SIZE_16;
                    out_ca.p_cr_17 = p_r2y->p_CR_buf_17;
                    out_ca.cr_17_size = CR_SIZE_17;
                    out_ca.p_cr_18 = p_r2y->p_CR_buf_18;
                    out_ca.cr_18_size = CR_SIZE_18;
                    out_ca.p_cr_19 = p_r2y->p_CR_buf_19;
                    out_ca.cr_19_size = CR_SIZE_19;
                    out_ca.p_cr_20 = p_r2y->p_CR_buf_20;
                    out_ca.cr_20_size = CR_SIZE_20;
                    out_ca.p_calib = &p_flow->calib;
                    out_ca.p_cawarp_hor_table_addr_red = p_ca_warp_hor_red;
                    out_ca.h_red_size = CAWARP_VIDEO_MAXSIZE;
                    out_ca.p_cawarp_vertical_table_addr_red = p_ca_warp_ver_red;
                    out_ca.v_red_size = CAWARP_VIDEO_MAXSIZE;
                    out_ca.p_cawarp_hor_table_addr_blue = p_ca_warp_hor_blue;
                    out_ca.h_blue_size = CAWARP_VIDEO_MAXSIZE;
                    out_ca.p_cawarp_vertical_table_addr_blue = p_ca_warp_ver_blue;
                    out_ca.v_blue_size = CAWARP_VIDEO_MAXSIZE;

                    rval |= ikc_cawarp(&in_ca, &out_ca);

                    p_flow->calib.cawarp_horizontal_table_addr_red = physical2u32(p_ca_warp_hor_red);
                    p_flow->calib.cawarp_horizontal_table_addr_blue = physical2u32(p_ca_warp_hor_blue);
                    p_flow->calib.cawarp_vertical_table_addr_red = physical2u32(p_ca_warp_ver_red);
                    p_flow->calib.cawarp_vertical_table_addr_blue = physical2u32(p_ca_warp_ver_blue);
                }
            }
            p_flow->calib.update_CA_warp = 1U;
        } else {
            p_flow->calib.update_CA_warp = 0U;
        }
    } else {
        p_flow->flow_info.update_sbp = 0U;
        p_flow->flow_info.update_vignette = 0U;
        p_flow->calib.update_CA_warp = 0U;
        p_flow->calib.update_lens_warp = 1U;
    }

    if((p_filters->update_flags.iso.chroma_filter_updated == 1U)||
       (p_filters->update_flags.iso.warp_enable_updated == 1U)||
       (p_filters->update_flags.iso.calib_warp_info_updated == 1U)||
       (p_filters->update_flags.iso.warp_internal_updated == 1U)||
       (p_filters->update_flags.iso.window_size_info_updated == 1U)||
       (window_calculate_updated == 1U)) {
        if(p_filters->input_param.ctx_buf.warp_internal_mode_flag == 1U) { //internal warp
            ikc_in_warp_internal_t in_warp_internal;
            ikc_out_hwarp_t out_hwarp;
            ikc_out_vwarp_t out_vwarp;

            if((p_filters->update_flags.cr.warp == 0U)||
               (p_filters->update_flags.cr.r2y.cr_33_update == 0U)||
               (p_filters->update_flags.cr.r2y.cr_34_update == 0U)||
               (p_filters->update_flags.cr.r2y.cr_35_update == 0U)||
               (p_filters->update_flags.cr.r2y.cr_42_update == 0U)||
               (p_filters->update_flags.cr.r2y.cr_43_update == 0U)||
               (p_filters->update_flags.cr.r2y.cr_44_update == 0U)) {
                amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_warp_internal !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
                rval |= IK_ERR_0000;
            } else {
                {
                    in_warp_internal.flip_mode = p_filters->input_param.flip_mode;
                    in_warp_internal.p_window_size_info = &p_filters->input_param.window_size_info;
                    in_warp_internal.p_act_win_crop = &p_filters->input_param.ctx_buf.result_win.act_win_crop;
                    in_warp_internal.p_dmy_win_geo = &p_filters->input_param.ctx_buf.result_win.dmy_win_geo;
                    in_warp_internal.p_cfa_win_dim = &p_filters->input_param.ctx_buf.result_win.cfa_win_dim;
                    in_warp_internal.p_dmy_range = &p_filters->input_param.dmy_range;
                    in_warp_internal.chroma_radius = p_filters->input_param.chroma_filter.radius;
                    in_warp_internal.p_stitching_info = &p_filters->input_param.stitching_info;
                    in_warp_internal.p_warp_internal = &p_filters->input_param.warp_internal;
                    in_warp_internal.dram_efficiency = p_filters->input_param.warp_buf_info.dram_efficiency;
                    in_warp_internal.luma_wait_lines = p_filters->input_param.warp_buf_info.luma_wait_lines;
                    in_warp_internal.luma_dma_size = p_filters->input_param.warp_buf_info.luma_dma_size;
                    out_hwarp.p_cr_33 = p_r2y->p_CR_buf_33;
                    out_hwarp.cr_33_size = CR_SIZE_33;
                    out_hwarp.p_cr_34 = p_r2y->p_CR_buf_34;
                    out_hwarp.cr_34_size = CR_SIZE_34;
                    out_hwarp.p_cr_35 = p_r2y->p_CR_buf_35;
                    out_hwarp.cr_35_size = CR_SIZE_35;
                    out_hwarp.p_flow_info = &p_flow->flow_info;
                    out_hwarp.p_window = &p_flow->window;
                    out_hwarp.p_phase = &p_flow->phase;
                    out_hwarp.p_calib = &p_flow->calib;
                    out_hwarp.p_stitch = &p_flow->stitch;
                    out_hwarp.p_warp_horizontal_table_address = p_warp_hor;
                    out_hwarp.h_size = WARP_VIDEO_MAXHORGRID * WARP_VIDEO_MAXVERGRID * 2U * 2U;// multi_slice buffer.
                    rval |= ikc_hwarp_internal(&in_warp_internal, &out_hwarp);
                }
                {
                    in_warp_internal.flip_mode = p_filters->input_param.flip_mode;
                    in_warp_internal.p_window_size_info = &p_filters->input_param.window_size_info;
                    in_warp_internal.p_act_win_crop = &p_filters->input_param.ctx_buf.result_win.act_win_crop;
                    in_warp_internal.p_dmy_win_geo = &p_filters->input_param.ctx_buf.result_win.dmy_win_geo;
                    in_warp_internal.p_cfa_win_dim = &p_filters->input_param.ctx_buf.result_win.cfa_win_dim;
                    in_warp_internal.p_dmy_range = &p_filters->input_param.dmy_range;
                    in_warp_internal.chroma_radius = p_filters->input_param.chroma_filter.radius;
                    in_warp_internal.p_stitching_info = &p_filters->input_param.stitching_info;
                    in_warp_internal.p_warp_internal = &p_filters->input_param.warp_internal;
                    in_warp_internal.dram_efficiency = p_filters->input_param.warp_buf_info.dram_efficiency;
                    in_warp_internal.luma_wait_lines = p_filters->input_param.warp_buf_info.luma_wait_lines;
                    in_warp_internal.luma_dma_size = p_filters->input_param.warp_buf_info.luma_dma_size;
                    out_vwarp.p_cr_42 = p_r2y->p_CR_buf_42;
                    out_vwarp.cr_42_size = CR_SIZE_42;
                    out_vwarp.p_cr_43 = p_r2y->p_CR_buf_43;
                    out_vwarp.cr_43_size = CR_SIZE_43;
                    out_vwarp.p_cr_44 = p_r2y->p_CR_buf_44;
                    out_vwarp.cr_44_size = CR_SIZE_44;
                    //out_vwarp.p_flow_info = &p_flow->flow_info;
                    out_vwarp.p_window = &p_flow->window;
                    //out_vwarp.p_phase = &p_flow->phase;
                    out_vwarp.p_calib = &p_flow->calib;
                    out_vwarp.p_warp_vertical_table_address = p_warp_ver;
                    out_vwarp.v_size = WARP_VIDEO_MAXHORGRID * WARP_VIDEO_MAXVERGRID * 2U * 2U;// multi_slice buffer.
                    rval |= ikc_vwarp_internal(&in_warp_internal, &out_vwarp);
                }
            }
        } else {
            ikc_in_hwarp_t in_hwarp;
            ikc_out_hwarp_t out_hwarp;
            ikc_in_vwarp_t in_vwarp;
            ikc_out_vwarp_t out_vwarp;

            if((p_filters->update_flags.cr.warp == 0U)||
               (p_filters->update_flags.cr.r2y.cr_33_update == 0U)||
               (p_filters->update_flags.cr.r2y.cr_34_update == 0U)||
               (p_filters->update_flags.cr.r2y.cr_35_update == 0U)||
               (p_filters->update_flags.cr.r2y.cr_42_update == 0U)||
               (p_filters->update_flags.cr.r2y.cr_43_update == 0U)||
               (p_filters->update_flags.cr.r2y.cr_44_update == 0U)) {
                amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_hwarp !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
                rval |= IK_ERR_0000;
            } else {
                {
                    in_hwarp.is_group_cmd = 0u;
                    in_hwarp.is_hiso      = 0u;
                    in_hwarp.warp_enable = (uint32)p_filters->input_param.warp_enable;
                    in_hwarp.flip_mode = p_filters->input_param.flip_mode;
                    in_hwarp.p_window_size_info = &p_filters->input_param.window_size_info;
                    in_hwarp.p_act_win_crop = &p_filters->input_param.ctx_buf.result_win.act_win_crop;
                    in_hwarp.p_dmy_win_geo = &p_filters->input_param.ctx_buf.result_win.dmy_win_geo;
                    in_hwarp.p_cfa_win_dim = &p_filters->input_param.ctx_buf.result_win.cfa_win_dim;
                    in_hwarp.p_dmy_range = &p_filters->input_param.dmy_range;
                    in_hwarp.chroma_radius = p_filters->input_param.chroma_filter.radius;
                    in_hwarp.is_y2y_420input = (p_filters->input_param.yuv_mode == AMBA_IK_YUV420) ? 1U : 0U;
                    in_hwarp.p_stitching_info = &p_filters->input_param.stitching_info;
                    in_hwarp.p_calib_warp_info = &p_filters->input_param.calib_warp_info;
                    in_hwarp.dram_efficiency = p_filters->input_param.warp_buf_info.dram_efficiency;
                    in_hwarp.luma_wait_lines = p_filters->input_param.warp_buf_info.luma_wait_lines;
                    in_hwarp.luma_dma_size = p_filters->input_param.warp_buf_info.luma_dma_size;
                    in_hwarp.is_still_422 = 0;
                    out_hwarp.p_cr_33 = p_r2y->p_CR_buf_33;
                    out_hwarp.cr_33_size = CR_SIZE_33;
                    out_hwarp.p_cr_34 = p_r2y->p_CR_buf_34;
                    out_hwarp.cr_34_size = CR_SIZE_34;
                    out_hwarp.p_cr_35 = p_r2y->p_CR_buf_35;
                    out_hwarp.cr_35_size = CR_SIZE_35;
                    out_hwarp.p_flow_info = &p_flow->flow_info;
                    out_hwarp.p_window = &p_flow->window;
                    out_hwarp.p_phase = &p_flow->phase;
                    out_hwarp.p_calib = &p_flow->calib;
                    out_hwarp.p_stitch = &p_flow->stitch;
                    out_hwarp.p_warp_horizontal_table_address = p_warp_hor;
                    out_hwarp.h_size = WARP_VIDEO_MAXHORGRID * WARP_VIDEO_MAXVERGRID * 2U * 2U;// multi_slice buffer.

                    rval |= ikc_hwarp(&in_hwarp, &out_hwarp);
                }
                {
                    if ((video_pipe == AMBA_IK_VIDEO_PIPE_LINEAR) ||
                        (video_pipe == AMBA_IK_VIDEO_PIPE_LINEAR_CE) ||
                        (video_pipe == AMBA_IK_VIDEO_PIPE_HDR_EXPO_2) ||
                        (video_pipe == AMBA_IK_VIDEO_PIPE_HDR_EXPO_3) ||
                        (video_pipe == AMBA_IK_VIDEO_PIPE_Y2Y) ||
                        (video_pipe == AMBA_IK_VIDEO_PIPE_LINEAR_HVH)||
                        (video_pipe == AMBA_IK_VIDEO_PIPE_LINEAR_CE_HVH) ||
                        (video_pipe == AMBA_IK_VIDEO_PIPE_HDR_EXPO_2_HVH) ||
                        (video_pipe == AMBA_IK_VIDEO_PIPE_HDR_EXPO_3_HVH) ||
                        (video_pipe == AMBA_IK_VIDEO_PIPE_Y2Y_HVH) ||
                        (video_pipe == AMBA_IK_VIDEO_PIPE_LINEAR_HHB)||
                        (video_pipe == AMBA_IK_VIDEO_PIPE_LINEAR_CE_HHB) ||
                        (video_pipe == AMBA_IK_VIDEO_PIPE_HDR_EXPO_2_HHB) ||
                        (video_pipe == AMBA_IK_VIDEO_PIPE_HDR_EXPO_3_HHB) ||
                        (video_pipe == AMBA_IK_VIDEO_PIPE_Y2Y_HHB) ||
                        (video_pipe == AMBA_IK_STILL_PIPE_LISO) ||
                        (video_pipe == AMBA_IK_VIDEO_PIPE_FUSION)) {
                        in_vwarp.is_multi_pass = 0u;
                    } else if ((video_pipe == AMBA_IK_VIDEO_PIPE_LINEAR_MD) ||
                               (video_pipe == AMBA_IK_VIDEO_PIPE_LINEAR_CE_MD) ||
                               (video_pipe == AMBA_IK_VIDEO_PIPE_HDR_EXPO_2_MD) ||
                               (video_pipe == AMBA_IK_VIDEO_PIPE_HDR_EXPO_3_MD) ||
                               (video_pipe == AMBA_IK_VIDEO_PIPE_Y2Y_MD) ||
                               (video_pipe == AMBA_IK_VIDEO_PIPE_LINEAR_MD_HHB) ||
                               (video_pipe == AMBA_IK_VIDEO_PIPE_LINEAR_CE_MD_HHB) ||
                               (video_pipe == AMBA_IK_VIDEO_PIPE_HDR_EXPO_2_MD_HHB) ||
                               (video_pipe == AMBA_IK_VIDEO_PIPE_HDR_EXPO_3_MD_HHB) ||
                               (video_pipe == AMBA_IK_VIDEO_PIPE_Y2Y_MD_HHB)) {
                        in_vwarp.is_multi_pass = 1u;
                    } else {
                        // misraC
                    }
                    if ((video_pipe == AMBA_IK_VIDEO_PIPE_LINEAR_HVH)||
                        (video_pipe == AMBA_IK_VIDEO_PIPE_LINEAR_CE_HVH) ||
                        (video_pipe == AMBA_IK_VIDEO_PIPE_HDR_EXPO_2_HVH) ||
                        (video_pipe == AMBA_IK_VIDEO_PIPE_HDR_EXPO_3_HVH) ||
                        (video_pipe == AMBA_IK_VIDEO_PIPE_Y2Y_HVH)) {
                        in_vwarp.is_hvh_enabled = 1u;
                        in_vwarp.is_hhb_enabled = 0u;
                    } else if ((video_pipe == AMBA_IK_VIDEO_PIPE_LINEAR_HHB) ||
                               (video_pipe == AMBA_IK_VIDEO_PIPE_LINEAR_CE_HHB) ||
                               (video_pipe == AMBA_IK_VIDEO_PIPE_HDR_EXPO_2_HHB) ||
                               (video_pipe == AMBA_IK_VIDEO_PIPE_HDR_EXPO_3_HHB) ||
                               (video_pipe == AMBA_IK_VIDEO_PIPE_LINEAR_MD_HHB) ||
                               (video_pipe == AMBA_IK_VIDEO_PIPE_LINEAR_CE_MD_HHB) ||
                               (video_pipe == AMBA_IK_VIDEO_PIPE_HDR_EXPO_2_MD_HHB) ||
                               (video_pipe == AMBA_IK_VIDEO_PIPE_HDR_EXPO_3_MD_HHB) ||
                               (video_pipe == AMBA_IK_VIDEO_PIPE_Y2Y_HHB) ||
                               (video_pipe == AMBA_IK_VIDEO_PIPE_Y2Y_MD_HHB)) {
                        in_vwarp.is_hvh_enabled = 0u;
                        in_vwarp.is_hhb_enabled = 1u;
                    } else {
                        in_vwarp.is_hvh_enabled = 0u;
                        in_vwarp.is_hhb_enabled = 0u;
                    }

                    in_vwarp.is_group_cmd = 0u;
                    in_vwarp.warp_enable = (uint32)p_filters->input_param.warp_enable;
                    in_vwarp.flip_mode = p_filters->input_param.flip_mode;
                    in_vwarp.p_window_size_info = &p_filters->input_param.window_size_info;
                    in_vwarp.p_act_win_crop = &p_filters->input_param.ctx_buf.result_win.act_win_crop;
                    in_vwarp.p_dmy_win_geo = &p_filters->input_param.ctx_buf.result_win.dmy_win_geo;
                    in_vwarp.p_cfa_win_dim = &p_filters->input_param.ctx_buf.result_win.cfa_win_dim;
                    in_vwarp.p_dmy_range = &p_filters->input_param.dmy_range;
                    in_vwarp.chroma_radius = p_filters->input_param.chroma_filter.radius;
                    in_vwarp.is_y2y_420input = (p_filters->input_param.yuv_mode == AMBA_IK_YUV420) ? 1U : 0U;
                    in_vwarp.p_stitching_info = &p_filters->input_param.stitching_info;
                    in_vwarp.p_calib_warp_info = &p_filters->input_param.calib_warp_info;
                    in_vwarp.dram_efficiency = p_filters->input_param.warp_buf_info.dram_efficiency;
                    in_vwarp.luma_wait_lines = p_filters->input_param.warp_buf_info.luma_wait_lines;
                    in_vwarp.luma_dma_size = p_filters->input_param.warp_buf_info.luma_dma_size;
                    in_vwarp.p_burst_tile = &p_filters->input_param.burst_tile;
                    out_vwarp.p_cr_42 = p_r2y->p_CR_buf_42;
                    out_vwarp.cr_42_size = CR_SIZE_42;
                    out_vwarp.p_cr_43 = p_r2y->p_CR_buf_43;
                    out_vwarp.cr_43_size = CR_SIZE_43;
                    out_vwarp.p_cr_44 = p_r2y->p_CR_buf_44;
                    out_vwarp.cr_44_size = CR_SIZE_44;
                    out_vwarp.p_window = &p_flow->window;
                    out_vwarp.p_calib = &p_flow->calib;
                    out_vwarp.p_warp_vertical_table_address = p_warp_ver;
                    out_vwarp.v_size = WARP_VIDEO_MAXHORGRID * WARP_VIDEO_MAXVERGRID * 2U * 2U;// multi_slice buffer.

                    rval |= ikc_vwarp(&in_vwarp, &out_vwarp);
                }
            }
        }
        p_flow->calib.update_lens_warp = 1U;
    } else {
        p_flow->calib.update_lens_warp = 0U;
    }

    if((p_filters->update_flags.iso.warp_enable_2nd_updated == 1U)||
       (p_filters->update_flags.iso.calib_warp_2nd_info_updated == 1U)||
       (p_filters->update_flags.iso.warp_internal_2nd_updated == 1U)||
       (p_filters->update_flags.iso.window_size_info_updated == 1U)||
       (p_filters->update_flags.iso.chroma_filter_updated == 1U)||
       (window_calculate_updated == 1U)) {
        if(p_filters->input_param.ctx_buf.warp_2nd_internal_mode_flag == 1U) { //internal warpl;

            if((p_filters->update_flags.cr.warp_2nd == 0U)||
               (p_filters->update_flags.cr.r2y.cr_100_update == 0U)||
               (p_filters->update_flags.cr.r2y.cr_101_update == 0U)) {
                amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_secondary_hwarp_internal !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
                rval |= IK_ERR_0000;
            } else {
                {
                    ikc_in_2nd_hwarp_internal_t in_2nd_hwarp_internal;
                    ikc_out_2nd_hwarp_t out_2nd_hwarp;

                    in_2nd_hwarp_internal.p_window_size_info = &p_filters->input_param.window_size_info;
                    in_2nd_hwarp_internal.p_stitching_info = &p_filters->input_param.stitching_info;
                    in_2nd_hwarp_internal.p_warp_2nd_internal = &p_filters->input_param.warp_2nd_internal;
                    out_2nd_hwarp.p_cr_100 = p_r2y->p_CR_buf_100;
                    out_2nd_hwarp.cr_100_size = CR_SIZE_100;
                    out_2nd_hwarp.p_cr_101 = p_r2y->p_CR_buf_101;
                    out_2nd_hwarp.cr_101_size = CR_SIZE_101;
                    out_2nd_hwarp.p_flow_info = &p_flow->flow_info;
                    out_2nd_hwarp.p_phase = &p_flow->phase;
                    out_2nd_hwarp.p_calib = &p_flow->calib;
                    out_2nd_hwarp.p_warp_horizontal_b_table_address = p_warp_hor_b;
                    out_2nd_hwarp.h_size = WARP_VIDEO_MAXHORGRID * WARP_VIDEO_MAXVERGRID * 2U * 2U;// multi_slice buffer.

                    rval |= ikc_secondary_hwarp_internal(&in_2nd_hwarp_internal, &out_2nd_hwarp);
                }
            }
        } else {
            if((p_filters->update_flags.cr.warp_2nd == 0U)||
               (p_filters->update_flags.cr.r2y.cr_100_update == 0U)||
               (p_filters->update_flags.cr.r2y.cr_101_update == 0U)) {
                amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_secondary_hwarp !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
                rval |= IK_ERR_0000;
            } else {
                {
                    ikc_in_2nd_hwarp_t in_2nd_hwarp;
                    ikc_out_2nd_hwarp_t out_2nd_hwarp;

                    if ((video_pipe == AMBA_IK_VIDEO_PIPE_LINEAR_HVH)||
                        (video_pipe == AMBA_IK_VIDEO_PIPE_LINEAR_CE_HVH) ||
                        (video_pipe == AMBA_IK_VIDEO_PIPE_HDR_EXPO_2_HVH) ||
                        (video_pipe == AMBA_IK_VIDEO_PIPE_HDR_EXPO_3_HVH) ||
                        (video_pipe == AMBA_IK_VIDEO_PIPE_Y2Y_HVH)) {
                        in_2nd_hwarp.is_hvh_enabled = 1U;
                        in_2nd_hwarp.is_hhb_enabled = 0U;
                    } else if ((video_pipe == AMBA_IK_VIDEO_PIPE_LINEAR_HHB) ||
                               (video_pipe == AMBA_IK_VIDEO_PIPE_LINEAR_CE_HHB) ||
                               (video_pipe == AMBA_IK_VIDEO_PIPE_HDR_EXPO_2_HHB) ||
                               (video_pipe == AMBA_IK_VIDEO_PIPE_HDR_EXPO_3_HHB) ||
                               (video_pipe == AMBA_IK_VIDEO_PIPE_LINEAR_MD_HHB) ||
                               (video_pipe == AMBA_IK_VIDEO_PIPE_LINEAR_CE_MD_HHB) ||
                               (video_pipe == AMBA_IK_VIDEO_PIPE_HDR_EXPO_2_MD_HHB) ||
                               (video_pipe == AMBA_IK_VIDEO_PIPE_HDR_EXPO_3_MD_HHB) ||
                               (video_pipe == AMBA_IK_VIDEO_PIPE_Y2Y_HHB) ||
                               (video_pipe == AMBA_IK_VIDEO_PIPE_Y2Y_MD_HHB)) {
                        in_2nd_hwarp.is_hvh_enabled = 0U;
                        in_2nd_hwarp.is_hhb_enabled = 1U;
                    } else {
                        in_2nd_hwarp.is_hvh_enabled = 0U;
                        in_2nd_hwarp.is_hhb_enabled = 0U;
                    }
                    in_2nd_hwarp.warp_enable = (uint32)p_filters->input_param.warp_enable_2nd;
                    in_2nd_hwarp.is_group_cmd = 0u;
                    in_2nd_hwarp.flip_mode = p_filters->input_param.flip_mode;
                    in_2nd_hwarp.chroma_radius = p_filters->input_param.chroma_filter.radius;
                    {
                        in_2nd_hwarp.p_window_size_info = &p_filters->input_param.window_size_info;
                        in_2nd_hwarp.p_act_win_crop = &p_filters->input_param.ctx_buf.result_win.act_win_crop;
                        in_2nd_hwarp.p_dmy_win_geo = &p_filters->input_param.ctx_buf.result_win.dmy_win_geo;
                        in_2nd_hwarp.p_cfa_win_dim = &p_filters->input_param.ctx_buf.result_win.cfa_win_dim;
                        in_2nd_hwarp.p_stitching_info = &p_filters->input_param.stitching_info;
                    }
                    in_2nd_hwarp.p_calib_warp_info = &p_filters->input_param.calib_warp_info_2nd;
                    out_2nd_hwarp.p_cr_100 = p_r2y->p_CR_buf_100;
                    out_2nd_hwarp.cr_100_size = CR_SIZE_100;
                    out_2nd_hwarp.p_cr_101 = p_r2y->p_CR_buf_101;
                    out_2nd_hwarp.cr_101_size = CR_SIZE_101;
                    out_2nd_hwarp.p_flow_info = &p_flow->flow_info;
                    out_2nd_hwarp.p_phase = &p_flow->phase;
                    out_2nd_hwarp.p_calib = &p_flow->calib;
                    out_2nd_hwarp.p_warp_horizontal_b_table_address = p_warp_hor_b;
                    out_2nd_hwarp.h_size = WARP_VIDEO_MAXHORGRID * WARP_VIDEO_MAXVERGRID * 2U * 2U;// multi_slice buffer.

                    rval |= ikc_secondary_hwarp(&in_2nd_hwarp, &out_2nd_hwarp);
                }
            }
        }
        p_flow->calib.update_lens_warp_b = 1U; //TRUE;
    } else {
        p_flow->calib.update_lens_warp_b = 0U;
    }
#if 0
    amba_ik_system_print_uint32_5("####### ik_cfg_id_t:%d", sizeof(ik_cfg_id_t));
    amba_ik_system_print_uint32_5("####### idsp_flow_header_t:%d", sizeof(idsp_flow_header_t));
    amba_ik_system_print_uint32_5("####### idsp_calib_data_t:%d", sizeof(idsp_calib_data_t));
    amba_ik_system_print_uint32_5("####### idsp_aaa_data_t:%d", sizeof(idsp_aaa_data_t));
    amba_ik_system_print_uint32_5("####### idsp_flow_info_t:%d", sizeof(idsp_flow_info_t));
    amba_ik_system_print_uint32_5("####### idsp_window_info_t:%d", sizeof(idsp_window_info_t));
    amba_ik_system_print_uint32_5("####### idsp_phase_info_t:%d", sizeof(idsp_phase_info_t));
    amba_ik_system_print_uint32_5("####### idsp_stitch_data_t:%d", sizeof(idsp_stitch_data_t));
    amba_ik_system_print_uint32_5("####### idsp_hiso_data_t:%d", sizeof(idsp_hiso_data_t));
    amba_ik_system_print_uint32_5("####### idsp_motion_detect_t:%d", sizeof(idsp_motion_detect_t));
    amba_ik_system_print_uint32_5("####### idsp_fusion_data_t:%d", sizeof(idsp_fusion_data_t));
    amba_ik_system_print_uint32_5("####### idsp_multi_pass_data_t:%d", sizeof(idsp_multi_pass_data_t));
    amba_ik_system_print_uint32_5("####### idsp_step_info_t:%d", sizeof(idsp_step_info_t));
    amba_ik_system_print_uint32_5("####### config_region_t:%d", sizeof(config_region_t));
    amba_ik_system_print_uint32_5("####### idsp_step_crs_t:%d", sizeof(idsp_step_crs_t));
    amba_ik_system_print_uint32_5("####### idsp_flow_ctrl_t:%d", sizeof(idsp_flow_ctrl_t));
    amba_ik_system_print_uint32_5("####### idsp_group_update_info_t:%d", sizeof(idsp_group_update_info_t));
#endif
    return rval;
}

uint32 exe_process_hdr_ce_cr(idsp_flow_ctrl_t *p_flow, void *p_flow_tbl_list, amba_ik_filter_t *p_filters, uint32 exp_num)
{
    uint32 rval = IK_OK;
    uint32 window_calculate_updated = 0U;
    amba_ik_flow_tables_list_t *p_flow_tables_list;
    amba_ik_motion_fusion_flow_tables_list_t *p_motion_fusion_flow_tables_list;
    const amba_ik_r2y_flow_tables_list_t *p_r2y = NULL;
    uint32 video_pipe = p_filters->input_param.ability;
    void *p_extra_window = NULL;

    if ((video_pipe == AMBA_IK_VIDEO_PIPE_LINEAR) ||
        (video_pipe == AMBA_IK_VIDEO_PIPE_LINEAR_CE) ||
        (video_pipe == AMBA_IK_VIDEO_PIPE_HDR_EXPO_2) ||
        (video_pipe == AMBA_IK_VIDEO_PIPE_HDR_EXPO_3) ||
        (video_pipe == AMBA_IK_VIDEO_PIPE_Y2Y) ||
        (video_pipe == AMBA_IK_VIDEO_PIPE_LINEAR_HVH)||
        (video_pipe == AMBA_IK_VIDEO_PIPE_LINEAR_CE_HVH) ||
        (video_pipe == AMBA_IK_VIDEO_PIPE_HDR_EXPO_2_HVH) ||
        (video_pipe == AMBA_IK_VIDEO_PIPE_HDR_EXPO_3_HVH) ||
        (video_pipe == AMBA_IK_VIDEO_PIPE_Y2Y_HVH) ||
        (video_pipe == AMBA_IK_VIDEO_PIPE_LINEAR_HHB)||
        (video_pipe == AMBA_IK_VIDEO_PIPE_LINEAR_CE_HHB) ||
        (video_pipe == AMBA_IK_VIDEO_PIPE_HDR_EXPO_2_HHB) ||
        (video_pipe == AMBA_IK_VIDEO_PIPE_HDR_EXPO_3_HHB) ||
        (video_pipe == AMBA_IK_VIDEO_PIPE_Y2Y_HHB) ||
        (video_pipe == AMBA_IK_STILL_PIPE_LISO)) {
        (void)amba_ik_system_memcpy(&p_flow_tables_list, &p_flow_tbl_list, sizeof(amba_ik_flow_tables_list_t*));
        p_r2y = &p_flow_tables_list->r2y;
        p_extra_window = p_flow_tables_list->p_extra_window;
    } else if ((video_pipe == AMBA_IK_VIDEO_PIPE_LINEAR_MD) ||
               (video_pipe == AMBA_IK_VIDEO_PIPE_LINEAR_CE_MD) ||
               (video_pipe == AMBA_IK_VIDEO_PIPE_HDR_EXPO_2_MD) ||
               (video_pipe == AMBA_IK_VIDEO_PIPE_HDR_EXPO_3_MD) ||
               (video_pipe == AMBA_IK_VIDEO_PIPE_FUSION) ||
               (video_pipe == AMBA_IK_VIDEO_PIPE_Y2Y_MD) ||
               (video_pipe == AMBA_IK_VIDEO_PIPE_LINEAR_MD_HHB) ||
               (video_pipe == AMBA_IK_VIDEO_PIPE_LINEAR_CE_MD_HHB) ||
               (video_pipe == AMBA_IK_VIDEO_PIPE_HDR_EXPO_2_MD_HHB) ||
               (video_pipe == AMBA_IK_VIDEO_PIPE_HDR_EXPO_3_MD_HHB) ||
               (video_pipe == AMBA_IK_VIDEO_PIPE_Y2Y_MD_HHB)) {
        (void)amba_ik_system_memcpy(&p_motion_fusion_flow_tables_list, &p_flow_tbl_list, sizeof(amba_ik_motion_fusion_flow_tables_list_t*));
        p_r2y = &p_motion_fusion_flow_tables_list->r2y;
        p_extra_window = p_motion_fusion_flow_tables_list->p_extra_window;
    } else {
        // misraC
    }

    (void)amba_ik_system_memcpy(p_flow, p_flow, sizeof(uint8));//misra fix.

    if ((p_filters->update_flags.iso.window_size_info_updated == 1U) ||
        (p_filters->update_flags.iso.cfa_window_size_info_updated == 1U) ||
        (p_filters->update_flags.iso.dmy_range_updated == 1U) ||
        (p_filters->update_flags.iso.dzoom_info_updated == 1U) ||
        (p_filters->update_flags.iso.vin_active_win_updated == 1U) ||
        (p_filters->update_flags.iso.stitching_info_updated == 1U) ||
        (p_filters->update_flags.iso.overlap_x_updated == 1U) ||
        (p_filters->update_flags.iso.flip_mode_updated == 1U) ||
        (p_filters->update_flags.iso.warp_enable_updated == 1U) ||
        (p_filters->update_flags.iso.warp_enable_2nd_updated == 1U)) {
        window_calculate_updated = 1U;
    }

    if (p_r2y != NULL) {
        if ((p_filters->input_param.ability != AMBA_IK_VIDEO_PIPE_Y2Y) &&
            (p_filters->input_param.ability != AMBA_IK_VIDEO_PIPE_Y2Y_HVH) &&
            (p_filters->input_param.ability != AMBA_IK_VIDEO_PIPE_Y2Y_HHB) &&
            (p_filters->input_param.ability != AMBA_IK_VIDEO_PIPE_Y2Y_MD) &&
            (p_filters->input_param.ability != AMBA_IK_VIDEO_PIPE_Y2Y_MD_HHB)) {
            // before_ce_gain
            if(p_filters->update_flags.iso.before_ce_wb_gain_updated == 1u) {
                ikc_in_before_ce_gain_t in_before_ce_gain;
                ikc_out_before_ce_gain_t out_before_ce_gain;

                if(p_filters->update_flags.cr.r2y.cr_13_update == 0U) {
                    amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_before_ce_gain !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
                    rval |= IK_ERR_0000;
                } else {
                    in_before_ce_gain.is_ir_only = ((p_filters->input_param.sensor_info.sensor_mode == 1U) && (p_filters->input_param.rgb_ir.ir_only == 1U))?1U:0U;
                    in_before_ce_gain.p_before_ce_wb_gain = &p_filters->input_param.before_ce_wb_gain;
                    out_before_ce_gain.p_cr_13 = p_r2y->p_CR_buf_13;
                    out_before_ce_gain.cr_13_size = CR_SIZE_13;
                    rval |= ikc_before_ce_gain(&in_before_ce_gain, &out_before_ce_gain);
                }
            }

            if(exp_num < 4U) {
                // hdr_blend and hdr_alpha
                if(p_filters->update_flags.iso.hdr_blend_updated == 1u) {
                    ikc_in_hdr_blend_t in_hdr_blend;
                    ikc_out_hdr_blend_t out_hdr_blend;

                    if(p_filters->update_flags.cr.r2y.cr_4_update == 0U) {
                        amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_hdr_blend !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
                        rval |= IK_ERR_0000;
                    } else {
                        in_hdr_blend.exp_num = exp_num;
                        in_hdr_blend.p_hdr_raw_info = &p_filters->input_param.hdr_raw_info;
                        in_hdr_blend.p_hdr_blend = &p_filters->input_param.hdr_blend;
                        out_hdr_blend.p_cr_4 = p_r2y->p_CR_buf_4;
                        out_hdr_blend.cr_4_size = CR_SIZE_4;
                        out_hdr_blend.p_flow = &p_flow->flow_info;
                        rval |= ikc_hdr_blend(&in_hdr_blend, &out_hdr_blend);
                    }
                }

                // hdr_tone_curve
                if(p_filters->update_flags.iso.fe_tone_curve_updated == 1u) {
                    ikc_in_hdr_tone_curve_t in_hdr_tc;
                    ikc_out_hdr_tone_curve_t out_hdr_tc;

                    if((p_filters->update_flags.cr.r2y.cr_4_update == 0U)||
                       (p_filters->update_flags.cr.r2y.cr_5_update == 0U)) {
                        amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_front_end_tone_curve !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
                        rval |= IK_ERR_0000;
                    } else {
                        in_hdr_tc.p_fe_tone_curve = &p_filters->input_param.fe_tone_curve;
                        out_hdr_tc.p_cr_4 = p_r2y->p_CR_buf_4;
                        out_hdr_tc.cr_4_size = CR_SIZE_4;
                        out_hdr_tc.p_cr_5 = p_r2y->p_CR_buf_5;
                        out_hdr_tc.cr_5_size = CR_SIZE_5;
                        rval |= ikc_front_end_tone_curve(&in_hdr_tc, &out_hdr_tc);
                    }
                }

                // losy decompress
                if((p_filters->update_flags.iso.sensor_information_updated == 1u)||(window_calculate_updated == 1U)) {
                    ikc_in_decompress_t in_decompress;
                    ikc_out_decompress_t out_decompress;

                    if(p_filters->update_flags.cr.r2y.cr_4_update == 0U) {
                        amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_decompression !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
                        rval |= IK_ERR_0000;
                    } else {
                        in_decompress.exp_num = exp_num;
                        in_decompress.compress_mode = p_filters->input_param.sensor_info.compression;////0~8, 256~259.
                        in_decompress.p_dmy_win_geo = &p_filters->input_param.ctx_buf.result_win.dmy_win_geo;
                        out_decompress.p_cr_4 = p_r2y->p_CR_buf_4;
                        out_decompress.cr_4_size = CR_SIZE_4;
                        rval |= ikc_decompression(&in_decompress, &out_decompress);
                        if(p_filters->update_flags.iso.is_1st_frame == 1u) {
                            p_filters->input_param.ctx_buf.first_compression_offset = p_filters->input_param.sensor_info.compression_offset;
                        }
                    }
                }
                //sanity check, mutually exclusive between TC / compression_offset.
                if((p_filters->input_param.fe_tone_curve.decompand_enable != 0U) && (p_filters->input_param.ctx_buf.first_compression_offset != 0)) {
                    amba_ik_system_print_int32_5("[IK] exe_process_hdr_ce_cr fail, invalid decompand_enable : %d, first_compression_offset : %d", (int32)p_filters->input_param.fe_tone_curve.decompand_enable, p_filters->input_param.ctx_buf.first_compression_offset, DC_I, DC_I, DC_I);
                    rval |= IK_ERR_0008;
                }

                // hdr exp
                if((p_filters->update_flags.iso.exp0_fe_static_blc_updated == 1u) || (p_filters->update_flags.iso.exp1_fe_static_blc_updated == 1u) ||
                   (p_filters->update_flags.iso.exp2_fe_static_blc_updated == 1u)) {
                    ikc_in_hdr_blc_t in_hdr_blc;
                    ikc_out_hdr_blc_t out_hdr_blc;

                    if((p_filters->update_flags.cr.r2y.cr_4_update == 0U)||
                       (p_filters->update_flags.cr.r2y.cr_6_update == 0U)||
                       (p_filters->update_flags.cr.r2y.cr_7_update == 0U)||
                       (p_filters->update_flags.cr.r2y.cr_8_update == 0U)) {
                        amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_hdr_black_level !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
                        rval |= IK_ERR_0000;
                    } else {
                        in_hdr_blc.exp_num = exp_num;
                        in_hdr_blc.rgb_ir_mode = p_filters->input_param.rgb_ir.mode;
                        in_hdr_blc.compression_offset = ((p_filters->input_param.sensor_info.compression > IK_RAW_COMPRESS_OFF)&&(p_filters->input_param.sensor_info.compression <= IK_RAW_COMPRESS_10p5)) ? p_filters->input_param.ctx_buf.first_compression_offset : 0;
                        (void)amba_ik_system_memcpy(&p_filters->input_param.ctx_buf.ir_only_trick_buffer.exp0_frontend_static_blc, &p_filters->input_param.exp0_frontend_static_blc, sizeof(ik_static_blc_level_t));
                        (void)amba_ik_system_memcpy(&p_filters->input_param.ctx_buf.ir_only_trick_buffer.exp1_frontend_static_blc, &p_filters->input_param.exp1_frontend_static_blc, sizeof(ik_static_blc_level_t));
                        (void)amba_ik_system_memcpy(&p_filters->input_param.ctx_buf.ir_only_trick_buffer.exp2_frontend_static_blc, &p_filters->input_param.exp2_frontend_static_blc, sizeof(ik_static_blc_level_t));
                        in_hdr_blc.p_exp0_frontend_static_blc = &p_filters->input_param.ctx_buf.ir_only_trick_buffer.exp0_frontend_static_blc;
                        in_hdr_blc.p_exp1_frontend_static_blc = &p_filters->input_param.ctx_buf.ir_only_trick_buffer.exp1_frontend_static_blc;
                        in_hdr_blc.p_exp2_frontend_static_blc = &p_filters->input_param.ctx_buf.ir_only_trick_buffer.exp2_frontend_static_blc;
                        out_hdr_blc.p_cr_4 = p_r2y->p_CR_buf_4;
                        out_hdr_blc.cr_4_size = CR_SIZE_4;
                        out_hdr_blc.p_cr_6 = p_r2y->p_CR_buf_6;
                        out_hdr_blc.cr_6_size = CR_SIZE_6;
                        out_hdr_blc.p_cr_7 = p_r2y->p_CR_buf_7;
                        out_hdr_blc.cr_7_size = CR_SIZE_7;
                        out_hdr_blc.p_cr_8 = p_r2y->p_CR_buf_8;
                        out_hdr_blc.cr_8_size = CR_SIZE_8;
                        if((p_filters->input_param.rgb_ir.ir_only==1U)&&(p_filters->input_param.sensor_info.sensor_mode==1U)) { //ir_only, runing on RGB bayer pattern, carry IR on B channel.
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

                if((p_filters->update_flags.iso.exp0_fe_wb_gain_updated == 1u) || (p_filters->update_flags.iso.exp1_fe_wb_gain_updated == 1u) ||
                   (p_filters->update_flags.iso.exp2_fe_wb_gain_updated == 1u)) {
                    ikc_in_hdr_dgain_t in_hdr_dgain = {0};
                    ikc_out_hdr_dgain_t out_hdr_dgain;

                    if(p_filters->update_flags.cr.r2y.cr_4_update == 0U) {
                        amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_hdr_dgain !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
                        rval |= IK_ERR_0000;
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

                        out_hdr_dgain.p_cr_4 = p_r2y->p_CR_buf_4;
                        out_hdr_dgain.cr_4_size = CR_SIZE_4;
                        if((p_filters->input_param.rgb_ir.ir_only==1U)&&(p_filters->input_param.sensor_info.sensor_mode==1U)) { //ir_only, runing on RGB bayer pattern, carry IR on B channel.
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

                // hdr eis
                if((p_filters->input_param.use_hdr_eis == 1U)&&
                   ((p_filters->update_flags.iso.hdr_eis_shift_info_updated == 1u)||
                    (p_filters->update_flags.iso.sensor_information_updated == 1u)||(window_calculate_updated == 1U))) {
                    ikc_in_hdr_eis_t in_hdr_eis;
                    ikc_out_hdr_eis_t out_hdr_eis;

                    if(p_filters->update_flags.cr.r2y.cr_4_update == 0U) {
                        amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_hdr_eis !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
                        rval |= IK_ERR_0000;
                    } else {
                        in_hdr_eis.exp_num = exp_num;
                        in_hdr_eis.p_dmy_win_geo = &p_filters->input_param.ctx_buf.result_win.dmy_win_geo;
                        in_hdr_eis.p_vin_sensor = &p_filters->input_param.window_size_info.vin_sensor;
                        in_hdr_eis.horizontal_shift_exp1 = p_filters->input_param.ctx_buf.result_win.hdr_eis_sft.horizontal_shift_exp1;
                        in_hdr_eis.vertical_shift_exp1 = p_filters->input_param.ctx_buf.result_win.hdr_eis_sft.vertical_shift_exp1;
                        in_hdr_eis.horizontal_shift_exp2 = p_filters->input_param.ctx_buf.result_win.hdr_eis_sft.horizontal_shift_exp2;
                        in_hdr_eis.vertical_shift_exp2 = p_filters->input_param.ctx_buf.result_win.hdr_eis_sft.vertical_shift_exp2;
                        out_hdr_eis.p_cr_4 = p_r2y->p_CR_buf_4;
                        out_hdr_eis.cr_4_size = CR_SIZE_4;
                        out_hdr_eis.p_extra_window_info = p_extra_window;
                        out_hdr_eis.p_window = &p_flow->window;
                        rval |= ikc_hdr_eis(&in_hdr_eis, &out_hdr_eis);
                    }
                }

                // ce
                if(exp_num > 0U) {
                    if((p_filters->update_flags.iso.ce_updated == 1u)||(window_calculate_updated == 1U)) {
                        ikc_in_ce_t in_ce;
                        ikc_out_ce_t out_ce;

                        if(p_filters->update_flags.cr.r2y.cr_13_update == 0U) {
                            amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_contrast_enhancement !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
                            rval |= IK_ERR_0000;
                        } else {
                            in_ce.exp_num = exp_num;
                            in_ce.sensor_pattern = p_filters->input_param.sensor_info.sensor_pattern;
                            in_ce.p_dmy_win_geo = &p_filters->input_param.ctx_buf.result_win.dmy_win_geo;
                            in_ce.p_ce = &p_filters->input_param.ce;
                            out_ce.p_cr_13 = p_r2y->p_CR_buf_13;
                            out_ce.cr_13_size = CR_SIZE_13;
                            out_ce.p_flow = &p_flow->flow_info;
                            rval |= ikc_contrast_enhancement(&in_ce, &out_ce);
                        }
                    }
                    if(p_filters->update_flags.iso.ce_input_table_updated == 1u) {
                        ikc_in_ce_input_t in_ce_input;
                        ikc_out_ce_input_t out_ce_input;

                        if((p_filters->update_flags.cr.r2y.cr_13_update == 0U)||
                           (p_filters->update_flags.cr.r2y.cr_14_update == 0U)) {
                            amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_contrast_enhancement_input !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
                            rval |= IK_ERR_0000;
                        } else {
                            in_ce_input.ce_enable = p_filters->input_param.ce.enable;
                            in_ce_input.p_ce_input_table = &p_filters->input_param.ce_input_table;
                            out_ce_input.p_cr_13 = p_r2y->p_CR_buf_13;
                            out_ce_input.cr_13_size = CR_SIZE_13;
                            out_ce_input.p_cr_14 = p_r2y->p_CR_buf_14;
                            out_ce_input.cr_14_size = CR_SIZE_14;
                            rval |= ikc_contrast_enhancement_input(&in_ce_input, &out_ce_input);
                        }
                    }
                    if(p_filters->update_flags.iso.ce_out_table_updated == 1u) {
                        ikc_in_ce_output_t in_ce_output;
                        ikc_out_ce_t out_ce;

                        if(p_filters->update_flags.cr.r2y.cr_13_update == 0U) {
                            amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_contrast_enhancement_output !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
                            rval |= IK_ERR_0000;
                        } else {
                            in_ce_output.ce_enable = p_filters->input_param.ce.enable;
                            in_ce_output.p_ce_out_table = &p_filters->input_param.ce_out_table;
                            out_ce.p_cr_13 = p_r2y->p_CR_buf_13;
                            out_ce.cr_13_size = CR_SIZE_13;
                            rval |= ikc_contrast_enhancement_output(&in_ce_output, &out_ce);
                        }
                    }
                }
            } else {
                amba_ik_system_print_uint32_5("[IK] exe_process_hdr_ce_cr fail, incorrect exp_num %d", exp_num, DC_U, DC_U, DC_U, DC_U);
                rval = IK_ERR_0000;
            }
        } else {
            amba_ik_system_print_uint32_5("[IK] exe_process_hdr_ce_cr fail, do not support y2y mode!", DC_U, DC_U, DC_U, DC_U, DC_U);
            rval = IK_ERR_0004;
        }
    }

    return rval;
}

uint32 exe_process_cfa_cr(idsp_flow_ctrl_t *p_flow, void *p_flow_tbl_list, amba_ik_filter_t *p_filters)
{
    uint32 rval = IK_OK;
    uint32 window_calculate_updated = 0U;
    amba_ik_flow_tables_list_t *p_flow_tables_list;
    amba_ik_motion_fusion_flow_tables_list_t *p_motion_fusion_flow_tables_list;
    const amba_ik_r2y_flow_tables_list_t *p_r2y = NULL;
    uint32 video_pipe = p_filters->input_param.ability;
    void *p_aaa = NULL;

    if ((video_pipe == AMBA_IK_VIDEO_PIPE_LINEAR) ||
        (video_pipe == AMBA_IK_VIDEO_PIPE_LINEAR_CE) ||
        (video_pipe == AMBA_IK_VIDEO_PIPE_HDR_EXPO_2) ||
        (video_pipe == AMBA_IK_VIDEO_PIPE_HDR_EXPO_3) ||
        (video_pipe == AMBA_IK_VIDEO_PIPE_Y2Y) ||
        (video_pipe == AMBA_IK_VIDEO_PIPE_LINEAR_HVH)||
        (video_pipe == AMBA_IK_VIDEO_PIPE_LINEAR_CE_HVH) ||
        (video_pipe == AMBA_IK_VIDEO_PIPE_HDR_EXPO_2_HVH) ||
        (video_pipe == AMBA_IK_VIDEO_PIPE_HDR_EXPO_3_HVH) ||
        (video_pipe == AMBA_IK_VIDEO_PIPE_Y2Y_HVH) ||
        (video_pipe == AMBA_IK_VIDEO_PIPE_LINEAR_HHB)||
        (video_pipe == AMBA_IK_VIDEO_PIPE_LINEAR_CE_HHB) ||
        (video_pipe == AMBA_IK_VIDEO_PIPE_HDR_EXPO_2_HHB) ||
        (video_pipe == AMBA_IK_VIDEO_PIPE_HDR_EXPO_3_HHB) ||
        (video_pipe == AMBA_IK_VIDEO_PIPE_Y2Y_HHB) ||
        (video_pipe == AMBA_IK_STILL_PIPE_LISO)) {
        (void)amba_ik_system_memcpy(&p_flow_tables_list, &p_flow_tbl_list, sizeof(amba_ik_flow_tables_list_t*));
        p_r2y = &p_flow_tables_list->r2y;
        p_aaa = p_flow_tables_list->p_aaa;
    } else if ((video_pipe == AMBA_IK_VIDEO_PIPE_LINEAR_MD) ||
               (video_pipe == AMBA_IK_VIDEO_PIPE_LINEAR_CE_MD) ||
               (video_pipe == AMBA_IK_VIDEO_PIPE_HDR_EXPO_2_MD) ||
               (video_pipe == AMBA_IK_VIDEO_PIPE_HDR_EXPO_3_MD) ||
               (video_pipe == AMBA_IK_VIDEO_PIPE_FUSION) ||
               (video_pipe == AMBA_IK_VIDEO_PIPE_Y2Y_MD) ||
               (video_pipe == AMBA_IK_VIDEO_PIPE_LINEAR_MD_HHB) ||
               (video_pipe == AMBA_IK_VIDEO_PIPE_LINEAR_CE_MD_HHB) ||
               (video_pipe == AMBA_IK_VIDEO_PIPE_HDR_EXPO_2_MD_HHB) ||
               (video_pipe == AMBA_IK_VIDEO_PIPE_HDR_EXPO_3_MD_HHB) ||
               (video_pipe == AMBA_IK_VIDEO_PIPE_Y2Y_MD_HHB)) {
        (void)amba_ik_system_memcpy(&p_motion_fusion_flow_tables_list, &p_flow_tbl_list, sizeof(amba_ik_motion_fusion_flow_tables_list_t*));
        p_r2y = &p_motion_fusion_flow_tables_list->r2y;
        p_aaa = p_motion_fusion_flow_tables_list->p_aaa;
    } else {
        // misraC
    }

    (void)amba_ik_system_memcpy(p_flow, p_flow, sizeof(uint8));//misra fix.

    if ((p_filters->update_flags.iso.window_size_info_updated == 1U) ||
        (p_filters->update_flags.iso.cfa_window_size_info_updated == 1U) ||
        (p_filters->update_flags.iso.dmy_range_updated == 1U) ||
        (p_filters->update_flags.iso.dzoom_info_updated == 1U) ||
        (p_filters->update_flags.iso.vin_active_win_updated == 1U) ||
        (p_filters->update_flags.iso.stitching_info_updated == 1U) ||
        (p_filters->update_flags.iso.overlap_x_updated == 1U) ||
        (p_filters->update_flags.iso.flip_mode_updated == 1U) ||
        (p_filters->update_flags.iso.warp_enable_updated == 1U) ||
        (p_filters->update_flags.iso.warp_enable_2nd_updated == 1U)) {
        window_calculate_updated = 1U;
    }

    if (p_r2y != NULL) {
        if ((p_filters->input_param.ability != AMBA_IK_VIDEO_PIPE_Y2Y) &&
            (p_filters->input_param.ability != AMBA_IK_VIDEO_PIPE_Y2Y_HVH) &&
            (p_filters->input_param.ability != AMBA_IK_VIDEO_PIPE_Y2Y_HHB) &&
            (p_filters->input_param.ability != AMBA_IK_VIDEO_PIPE_Y2Y_MD) &&
            (p_filters->input_param.ability != AMBA_IK_VIDEO_PIPE_Y2Y_MD_HHB)) {
            // dbp
            if(p_filters->update_flags.iso.dynamic_bpc_updated == 1u) {
                ikc_in_dynamic_bad_pixel_t in_dbp;
                ikc_out_dynamic_bad_pixel_t out_dbp;

                if((p_filters->update_flags.cr.r2y.cr_11_update == 0U)||
                   (p_filters->update_flags.cr.r2y.cr_12_update == 0U)) {
                    amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_dynamic_bad_pixel !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
                    rval |= IK_ERR_0000;
                } else {
                    if(p_filters->input_param.ctx_buf.sbp_internal_mode_flag == 1u) {
                        in_dbp.sbp_enable = p_filters->input_param.sbp_internal.enable;
                    } else {
                        in_dbp.sbp_enable = p_filters->input_param.sbp_enable;
                    }
                    if((p_filters->input_param.rgb_ir.ir_only==1U)&&(p_filters->input_param.sensor_info.sensor_mode==1U)) { //ir_only, runing on RGB bayer pattern, carry IR on B channel.
                        in_dbp.rgb_ir_mode = 0u;
                    } else {
                        in_dbp.rgb_ir_mode = p_filters->input_param.rgb_ir.mode;
                    }
                    in_dbp.p_dbp = &p_filters->input_param.dynamic_bpc;
                    out_dbp.p_cr_11 = p_r2y->p_CR_buf_11;
                    out_dbp.cr_11_size = CR_SIZE_11;
                    out_dbp.p_cr_12 = p_r2y->p_CR_buf_12;
                    out_dbp.cr_12_size = CR_SIZE_12;
                    rval |= ikc_dynamic_bad_pixel(&in_dbp, &out_dbp);
                }
            }
            // stored ir
            if(p_filters->update_flags.iso.stored_ir_updated == 1u) {
                ikc_in_stored_ir_t in_stored_ir;
                ikc_out_stored_ir_t out_stored_ir;

                if(p_filters->update_flags.cr.r2y.cr_12_update == 0U) {
                    amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_stored_ir !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
                    rval |= IK_ERR_0000;
                } else {
                    in_stored_ir.p_stored_ir = &p_filters->input_param.stored_ir;
                    out_stored_ir.p_cr_12 = p_r2y->p_CR_buf_12;
                    out_stored_ir.cr_12_size = CR_SIZE_12;
                    rval |= ikc_stored_ir(&in_stored_ir, &out_stored_ir);
                }
            }

            // grgb_mismatch
            if(p_filters->update_flags.iso.grgb_mismatch_updated == 1u) {
                ikc_in_grgb_mismatch_t in_grgb;
                ikc_out_grgb_mismatch_t out_grgb;

                if(p_filters->update_flags.cr.r2y.cr_23_update == 0U) {
                    amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_grgb_mismatch !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
                    rval |= IK_ERR_0000;
                } else {
                    in_grgb.p_grgb_mismatch = &p_filters->input_param.grgb_mismatch;
                    out_grgb.p_cr_23 = p_r2y->p_CR_buf_23;
                    out_grgb.cr_23_size = CR_SIZE_23;
                    rval |= ikc_grgb_mismatch(&in_grgb, &out_grgb);
                }
            }

            // cfa_leakage and anti_aliasing
            if((p_filters->update_flags.iso.cfa_leakage_filter_updated == 1u) || (p_filters->update_flags.iso.anti_aliasing_updated == 1u)) {
                ikc_in_cfa_leak_anti_alias_t in_cfa_leak_anti_alias;
                ikc_out_cfa_leak_anti_alias_t out_cfa_leak_anti_alias;

                if(p_filters->update_flags.cr.r2y.cr_12_update == 0U) {
                    amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_cfa_leakage_anti_aliasing !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
                    rval |= IK_ERR_0000;
                } else {
                    in_cfa_leak_anti_alias.p_cfa_leakage_filter = &p_filters->input_param.cfa_leakage_filter;
                    in_cfa_leak_anti_alias.p_anti_aliasing = &p_filters->input_param.anti_aliasing;
                    out_cfa_leak_anti_alias.p_cr_12 = p_r2y->p_CR_buf_12;
                    out_cfa_leak_anti_alias.cr_12_size = CR_SIZE_12;
                    rval |= ikc_cfa_leakage_anti_aliasing(&in_cfa_leak_anti_alias, &out_cfa_leak_anti_alias);
                }
            }

            // cfa_lossy_compression
            if(p_filters->update_flags.iso.ext_raw_out_mode_updated == 1u) {
                ikc_in_cfa_lossy_compression_t in_cfa_lossy_compr;
                ikc_out_cfa_lossy_compression_t out_cfa_lossy_compr;

                if(p_filters->update_flags.cr.r2y.cr_15_update == 0U) {
                    amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_cfa_lossy_compression !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
                    rval |= IK_ERR_0000;
                } else {
                    in_cfa_lossy_compr.ext_raw_out_mode = p_filters->input_param.ext_raw_out_mode; //0~8, 256~259.
                    out_cfa_lossy_compr.p_cr_15 = p_r2y->p_CR_buf_15;
                    out_cfa_lossy_compr.cr_15_size = CR_SIZE_15;
                    out_cfa_lossy_compr.p_flow_info = &p_flow->flow_info;
                    rval |= ikc_cfa_lossy_compression(&in_cfa_lossy_compr, &out_cfa_lossy_compr);
                }
            }

            // cfa noise
            if(p_filters->update_flags.iso.cfa_noise_filter_updated == 1u) {
                ikc_in_cfa_noise_t in_cfa_noise;
                ikc_out_cfa_noise_t out_cfa_noise;

                if(p_filters->update_flags.cr.r2y.cr_23_update == 0U) {
                    amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_cfa_noise !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
                    rval |= IK_ERR_0000;
                } else {
                    in_cfa_noise.p_cfa_noise_filter = &p_filters->input_param.cfa_noise_filter;
                    out_cfa_noise.p_cr_23 = p_r2y->p_CR_buf_23;
                    out_cfa_noise.cr_23_size = CR_SIZE_23;
                    rval |= ikc_cfa_noise(&in_cfa_noise, &out_cfa_noise);
                }
            }

            // wb_gain
            if(p_filters->update_flags.iso.after_ce_wb_gain_updated == 1u) {
                ikc_in_after_ce_gain_t in_after_ce_gain;
                ikc_out_after_ce_gain_t out_after_ce_gain;

                if(p_filters->update_flags.cr.r2y.cr_22_update == 0U) {
                    amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_after_ce_gain !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
                    rval |= IK_ERR_0000;
                } else {
                    in_after_ce_gain.p_after_ce_wb_gain = &p_filters->input_param.after_ce_wb_gain;
                    out_after_ce_gain.p_cr_22 = p_r2y->p_CR_buf_22;
                    out_after_ce_gain.cr_22_size = CR_SIZE_22;
                    rval |= ikc_after_ce_gain(&in_after_ce_gain, &out_after_ce_gain);
                }
            }

            // rgbir
            if(p_filters->update_flags.iso.rgb_ir_updated == 1u) {
                ikc_in_rgb_ir_t in_rgb_ir;
                ikc_out_rgb_ir_t out_rgb_ir;

                if((p_filters->update_flags.cr.r2y.cr_4_update == 0U)||
                   (p_filters->update_flags.cr.r2y.cr_6_update == 0U)||
                   (p_filters->update_flags.cr.r2y.cr_7_update == 0U)||
                   (p_filters->update_flags.cr.r2y.cr_8_update == 0U)||
                   (p_filters->update_flags.cr.r2y.cr_9_update == 0U)||
                   (p_filters->update_flags.cr.r2y.cr_11_update == 0U)||
                   (p_filters->update_flags.cr.r2y.cr_12_update == 0U)||
                   (p_filters->update_flags.cr.r2y.cr_21_update == 0U)) {
                    amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_rgb_ir !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
                    rval |= IK_ERR_0000;
                } else {
                    in_rgb_ir.flip_mode = p_filters->input_param.flip_mode;
                    (void)amba_ik_system_memcpy(&p_filters->input_param.ctx_buf.ir_only_trick_buffer.rgb_ir, &p_filters->input_param.rgb_ir, sizeof(ik_rgb_ir_t));
                    in_rgb_ir.p_rgb_ir = &p_filters->input_param.ctx_buf.ir_only_trick_buffer.rgb_ir;
                    in_rgb_ir.p_before_ce_wb_gain = &p_filters->input_param.before_ce_wb_gain;
                    in_rgb_ir.p_after_ce_wb_gain = &p_filters->input_param.after_ce_wb_gain;
                    (void)amba_ik_system_memcpy(&p_filters->input_param.ctx_buf.ir_only_trick_buffer.exp0_frontend_wb_gain, &p_filters->input_param.exp0_frontend_wb_gain, sizeof(ik_frontend_wb_gain_t));
                    in_rgb_ir.p_exp0_frontend_wb_gain = &p_filters->input_param.ctx_buf.ir_only_trick_buffer.exp0_frontend_wb_gain;
                    out_rgb_ir.p_cr_4 = p_r2y->p_CR_buf_4;
                    out_rgb_ir.cr_4_size = CR_SIZE_4;
                    out_rgb_ir.p_cr_6 = p_r2y->p_CR_buf_6;
                    out_rgb_ir.cr_6_size = CR_SIZE_6;
                    out_rgb_ir.p_cr_7 = p_r2y->p_CR_buf_7;
                    out_rgb_ir.cr_7_size = CR_SIZE_7;
                    out_rgb_ir.p_cr_8 = p_r2y->p_CR_buf_8;
                    out_rgb_ir.cr_8_size = CR_SIZE_8;
                    out_rgb_ir.p_cr_9 = p_r2y->p_CR_buf_9;
                    out_rgb_ir.cr_9_size = CR_SIZE_9;
                    out_rgb_ir.p_cr_11 = p_r2y->p_CR_buf_11;
                    out_rgb_ir.cr_11_size = CR_SIZE_11;
                    out_rgb_ir.p_cr_12 = p_r2y->p_CR_buf_12;
                    out_rgb_ir.cr_12_size = CR_SIZE_12;
                    out_rgb_ir.p_cr_21 = p_r2y->p_CR_buf_21;
                    out_rgb_ir.cr_21_size = CR_SIZE_21;
                    out_rgb_ir.p_flow = &p_flow->flow_info;
                    if((p_filters->input_param.rgb_ir.ir_only==1U)&&(p_filters->input_param.sensor_info.sensor_mode==1U)) { //ir_only, runing on RGB bayer pattern, carry IR on B channel.
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
            if ((p_filters->update_flags.iso.resample_str_update== 1U) ||
                (p_filters->update_flags.iso.cawarp_enable_updated == 1U)||
                (window_calculate_updated == 1U)) {
                ikc_in_cfa_prescale_t in_cfa_prescale;
                ikc_out_cfa_prescale_t out_cfa_prescale;

                if(p_filters->update_flags.cr.r2y.cr_16_update == 0U) {
                    amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_cfa_prescale !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
                    rval |= IK_ERR_0000;
                } else {
                    in_cfa_prescale.p_flow = &p_flow->flow_info;
                    in_cfa_prescale.cfa_cut_off_freq = p_filters->input_param.resample_str.cfa_cut_off_freq;
                    in_cfa_prescale.p_dmy_win_geo = &p_filters->input_param.ctx_buf.result_win.dmy_win_geo;
                    in_cfa_prescale.p_cfa_win_dim = &p_filters->input_param.ctx_buf.result_win.cfa_win_dim;
                    in_cfa_prescale.cawarp_enable = (uint32)p_filters->input_param.cawarp_enable;
                    out_cfa_prescale.p_cr_16 = p_r2y->p_CR_buf_16;
                    out_cfa_prescale.cr_16_size = CR_SIZE_16;
                    rval |= ikc_cfa_prescale(&in_cfa_prescale, &out_cfa_prescale);
                }
            }
        }

        // aaa_cfa
        if((p_filters->update_flags.iso.aaa_stat_info_updated == 1u) ||
           (p_filters->update_flags.iso.af_stat_ex_info_updated == 1u) ||
           (p_filters->update_flags.iso.pg_af_stat_ex_info_updated == 1u) ||
           (p_filters->update_flags.iso.histogram_info_update == 1u) ||
           (p_filters->update_flags.iso.histogram_info_pg_update == 1u) ||
           (p_filters->update_flags.iso.window_size_info_updated == 1U)||
           (window_calculate_updated == 1U)) {
            ikc_in_aaa_t in_aaa;
            ikc_out_aaa_t out_aaa;

            if((p_filters->update_flags.cr.aaa_data == 0U)||
               (p_filters->update_flags.cr.r2y.cr_9_update == 0U)||
               (p_filters->update_flags.cr.r2y.cr_12_update == 0U)||
               (p_filters->update_flags.cr.r2y.cr_21_update == 0U)||
               (p_filters->update_flags.cr.r2y.cr_28_update == 0U)) {
                amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_aaa !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
                rval |= IK_ERR_0000;
            } else {
                in_aaa.p_sensor_info = &p_filters->input_param.sensor_info;//determine RGB-IR.
                in_aaa.p_rgb_ir = &p_filters->input_param.rgb_ir;
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
                out_aaa.p_cr_9 = p_r2y->p_CR_buf_9;
                out_aaa.cr_9_size = CR_SIZE_9;
                out_aaa.p_cr_12 = p_r2y->p_CR_buf_12;
                out_aaa.cr_12_size = CR_SIZE_12;
                out_aaa.p_cr_21 = p_r2y->p_CR_buf_21;
                out_aaa.cr_21_size = CR_SIZE_21;
                out_aaa.p_cr_28 = p_r2y->p_CR_buf_28;
                out_aaa.cr_28_size = CR_SIZE_28;
                out_aaa.p_stitching_aaa = p_aaa;

                if ((p_filters->input_param.ability == AMBA_IK_VIDEO_PIPE_Y2Y) ||
                    (p_filters->input_param.ability == AMBA_IK_VIDEO_PIPE_Y2Y_HVH) ||
                    (p_filters->input_param.ability == AMBA_IK_VIDEO_PIPE_Y2Y_HHB) ||
                    (p_filters->input_param.ability == AMBA_IK_VIDEO_PIPE_Y2Y_MD) ||
                    (p_filters->input_param.ability == AMBA_IK_VIDEO_PIPE_Y2Y_MD_HHB)) {
                    in_aaa.is_yuv_mode = 1U;
                } else {
                    in_aaa.is_yuv_mode = 0U;
                }
                rval |= ikc_aaa(&in_aaa, &out_aaa);
            }
        }
    }

    return rval;
}

uint32 exe_process_rgb_cr(void *p_flow_tbl_list, amba_ik_filter_t *p_filters)
{
    uint32 rval = IK_OK;
    uint32 window_calculate_updated = 0U;
    amba_ik_flow_tables_list_t *p_flow_tables_list;
    amba_ik_motion_fusion_flow_tables_list_t *p_motion_fusion_flow_tables_list;
    const amba_ik_r2y_flow_tables_list_t *p_r2y = NULL;
    uint32 video_pipe = p_filters->input_param.ability;

    if ((video_pipe == AMBA_IK_VIDEO_PIPE_LINEAR) ||
        (video_pipe == AMBA_IK_VIDEO_PIPE_LINEAR_CE) ||
        (video_pipe == AMBA_IK_VIDEO_PIPE_HDR_EXPO_2) ||
        (video_pipe == AMBA_IK_VIDEO_PIPE_HDR_EXPO_3) ||
        (video_pipe == AMBA_IK_VIDEO_PIPE_Y2Y) ||
        (video_pipe == AMBA_IK_VIDEO_PIPE_LINEAR_HVH)||
        (video_pipe == AMBA_IK_VIDEO_PIPE_LINEAR_CE_HVH) ||
        (video_pipe == AMBA_IK_VIDEO_PIPE_HDR_EXPO_2_HVH) ||
        (video_pipe == AMBA_IK_VIDEO_PIPE_HDR_EXPO_3_HVH) ||
        (video_pipe == AMBA_IK_VIDEO_PIPE_Y2Y_HVH) ||
        (video_pipe == AMBA_IK_VIDEO_PIPE_LINEAR_HHB)||
        (video_pipe == AMBA_IK_VIDEO_PIPE_LINEAR_CE_HHB) ||
        (video_pipe == AMBA_IK_VIDEO_PIPE_HDR_EXPO_2_HHB) ||
        (video_pipe == AMBA_IK_VIDEO_PIPE_HDR_EXPO_3_HHB) ||
        (video_pipe == AMBA_IK_VIDEO_PIPE_Y2Y_HHB) ||
        (video_pipe == AMBA_IK_STILL_PIPE_LISO)) {
        (void)amba_ik_system_memcpy(&p_flow_tables_list, &p_flow_tbl_list, sizeof(amba_ik_flow_tables_list_t*));
        p_r2y = &p_flow_tables_list->r2y;
    } else if ((video_pipe == AMBA_IK_VIDEO_PIPE_LINEAR_MD) ||
               (video_pipe == AMBA_IK_VIDEO_PIPE_LINEAR_CE_MD) ||
               (video_pipe == AMBA_IK_VIDEO_PIPE_HDR_EXPO_2_MD) ||
               (video_pipe == AMBA_IK_VIDEO_PIPE_HDR_EXPO_3_MD) ||
               (video_pipe == AMBA_IK_VIDEO_PIPE_FUSION) ||
               (video_pipe == AMBA_IK_VIDEO_PIPE_Y2Y_MD) ||
               (video_pipe == AMBA_IK_VIDEO_PIPE_LINEAR_MD_HHB) ||
               (video_pipe == AMBA_IK_VIDEO_PIPE_LINEAR_CE_MD_HHB) ||
               (video_pipe == AMBA_IK_VIDEO_PIPE_HDR_EXPO_2_MD_HHB) ||
               (video_pipe == AMBA_IK_VIDEO_PIPE_HDR_EXPO_3_MD_HHB) ||
               (video_pipe == AMBA_IK_VIDEO_PIPE_Y2Y_MD_HHB)) {
        (void)amba_ik_system_memcpy(&p_motion_fusion_flow_tables_list, &p_flow_tbl_list, sizeof(amba_ik_motion_fusion_flow_tables_list_t*));
        p_r2y = &p_motion_fusion_flow_tables_list->r2y;
    } else {
        // misraC
    }

    if ((p_filters->update_flags.iso.window_size_info_updated == 1U) ||
        (p_filters->update_flags.iso.cfa_window_size_info_updated == 1U) ||
        (p_filters->update_flags.iso.dmy_range_updated == 1U) ||
        (p_filters->update_flags.iso.dzoom_info_updated == 1U) ||
        (p_filters->update_flags.iso.vin_active_win_updated == 1U) ||
        (p_filters->update_flags.iso.stitching_info_updated == 1U) ||
        (p_filters->update_flags.iso.overlap_x_updated == 1U) ||
        (p_filters->update_flags.iso.flip_mode_updated == 1U) ||
        (p_filters->update_flags.iso.warp_enable_updated == 1U) ||
        (p_filters->update_flags.iso.warp_enable_2nd_updated == 1U)) {
        window_calculate_updated = 1U;
    }

    if (window_calculate_updated==1U) {;} //misra

    if (p_r2y != NULL) {
        if ((p_filters->input_param.ability != AMBA_IK_VIDEO_PIPE_Y2Y) &&
            (p_filters->input_param.ability != AMBA_IK_VIDEO_PIPE_Y2Y_HVH) &&
            (p_filters->input_param.ability != AMBA_IK_VIDEO_PIPE_Y2Y_HHB) &&
            (p_filters->input_param.ability != AMBA_IK_VIDEO_PIPE_Y2Y_MD) &&
            (p_filters->input_param.ability != AMBA_IK_VIDEO_PIPE_Y2Y_MD_HHB)) {
            // demosaic
            if(p_filters->update_flags.iso.demosaic_updated == 1U) {
                ikc_in_demosaic_t in_demosaic;
                ikc_out_demosaic_t out_demosaic;

                if((p_filters->update_flags.cr.r2y.cr_12_update == 0U)||
                   (p_filters->update_flags.cr.r2y.cr_23_update == 0U)||
                   (p_filters->update_flags.cr.r2y.cr_24_update == 0U)) {
                    amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_demosaic_filter !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
                    rval |= IK_ERR_0000;
                } else {
                    in_demosaic.p_demosaic = &p_filters->input_param.demosaic;
                    out_demosaic.p_cr_12 = p_r2y->p_CR_buf_12;
                    out_demosaic.cr_12_size = CR_SIZE_12;
                    out_demosaic.p_cr_23 = p_r2y->p_CR_buf_23;
                    out_demosaic.cr_23_size = CR_SIZE_23;
                    out_demosaic.p_cr_24 = p_r2y->p_CR_buf_24;
                    out_demosaic.cr_24_size = CR_SIZE_24;
                    rval |= ikc_demosaic_filter(&in_demosaic, &out_demosaic);
                }
            }

            // rgb_to_12y
            if(p_filters->update_flags.iso.rgb_to_12y_updated == 1U) {
                ikc_in_rgb12y_t in_rgb12y;
                ikc_out_rgb12y_t out_rgb12y;

                if(p_filters->update_flags.cr.r2y.cr_25_update == 0U) {
                    amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_rgb12y !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
                    rval |= IK_ERR_0000;
                } else {
                    in_rgb12y.p_rgb_to_12y = &p_filters->input_param.rgb_to_12y;
                    out_rgb12y.p_cr_25 = p_r2y->p_CR_buf_25;
                    out_rgb12y.cr_25_size = CR_SIZE_25;
                    rval |= ikc_rgb12y(&in_rgb12y, &out_rgb12y);
                }
            }

            // cc
            if(p_filters->update_flags.iso.is_1st_frame == 1U) {//should only configure once in 1st time, due to R2Y always turn on.
                ikc_in_cc_en_t in_cc_enb;
                ikc_out_cc_en_t out_cc_enb;

                if(p_filters->update_flags.cr.r2y.cr_26_update == 0U) {
                    amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_color_correction_enable !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
                    rval |= IK_ERR_0000;
                } else {
                    in_cc_enb.is_yuv_mode = 0;
                    in_cc_enb.use_cc_for_yuv2yuv = 0;
                    out_cc_enb.p_cr_26 = p_r2y->p_CR_buf_26;
                    out_cc_enb.cr_26_size = CR_SIZE_26;
                    rval |= ikc_color_correction_enable(&in_cc_enb, &out_cc_enb);
                }
            }

            // cc_3d
            if(p_filters->update_flags.iso.color_correction_updated == 1U) {
                ikc_in_cc_t in_cc_3d;
                ikc_out_cc_t out_cc_3d;

                if((p_filters->update_flags.cr.r2y.cr_26_update == 0U)||
                   (p_filters->update_flags.cr.r2y.cr_27_update == 0U)) {
                    amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_color_correction !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
                    rval |= IK_ERR_0000;
                } else {
                    in_cc_3d.p_color_correction = &p_filters->input_param.color_correction;
                    in_cc_3d.is_ir_only = ((p_filters->input_param.sensor_info.sensor_mode == 1U) && (p_filters->input_param.rgb_ir.ir_only == 1U))?1U:0U;
                    out_cc_3d.p_cr_26 = p_r2y->p_CR_buf_26;
                    out_cc_3d.cr_26_size = CR_SIZE_26;
                    out_cc_3d.p_cr_27 = p_r2y->p_CR_buf_27;
                    out_cc_3d.cr_27_size = CR_SIZE_27;
                    rval |= ikc_color_correction(&in_cc_3d, &out_cc_3d);
                }
            }

            // cc_out
            if(p_filters->update_flags.iso.tone_curve_updated == 1U) {
                ikc_in_cc_out_t in_cc_out;
                ikc_out_cc_out_t out_cc_out;

                if(p_filters->update_flags.cr.r2y.cr_26_update == 0U) {
                    amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_color_correction_out !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
                    rval |= IK_ERR_0000;
                } else {
                    in_cc_out.p_tone_curve = &p_filters->input_param.tone_curve;
                    out_cc_out.p_cr_26 = p_r2y->p_CR_buf_26;
                    out_cc_out.cr_26_size = CR_SIZE_26;
                    rval |= ikc_color_correction_out(&in_cc_out, &out_cc_out);
                }
            }
        } else {
            //YUV input mode.
            //No longer support CC under Y2Y pipeline, since cc_reg has already been hacked as R2Y curve in low-level dsp.
            if(p_filters->update_flags.iso.is_1st_frame == 1U) {
                ikc_in_cc_en_t in_cc_enb;
                ikc_out_cc_en_t out_cc_enb;

                if(p_filters->update_flags.cr.r2y.cr_26_update == 0U) {
                    amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_color_correction_enable !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
                    rval |= IK_ERR_0000;
                } else {
                    in_cc_enb.is_yuv_mode = 1U;
                    in_cc_enb.use_cc_for_yuv2yuv = 0;
                    out_cc_enb.p_cr_26 = p_r2y->p_CR_buf_26;
                    out_cc_enb.cr_26_size = CR_SIZE_26;
                    rval |= ikc_color_correction_enable(&in_cc_enb, &out_cc_enb);
                }
            }
        }

        // rgb2yuv
        if((p_filters->update_flags.iso.rgb_to_yuv_matrix_updated == 1U) ||
           (p_filters->update_flags.iso.is_1st_frame == 1U)) {
            ikc_in_rgb2yuv_t in_rgb2yuv;
            ikc_out_rgb2yuv_t out_rgb2yuv;

            if(p_filters->update_flags.cr.r2y.cr_29_update == 0U) {
                amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_rgb2yuv !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
                rval |= IK_ERR_0000;
            } else {
                in_rgb2yuv.p_rgb_to_yuv_matrix = &p_filters->input_param.rgb_to_yuv_matrix;
                in_rgb2yuv.is_ir_only = ((p_filters->input_param.sensor_info.sensor_mode == 1U) && (p_filters->input_param.rgb_ir.ir_only == 1U))?1U:0U;
                out_rgb2yuv.p_cr_29 = p_r2y->p_CR_buf_29;
                out_rgb2yuv.cr_29_size = CR_SIZE_29;
                rval |= ikc_rgb2yuv(&in_rgb2yuv, &out_rgb2yuv);
            }
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

uint32 exe_process_yuv_cr(idsp_flow_ctrl_t *p_flow, void *p_flow_tbl_list, amba_ik_filter_t *p_filters)
{
    uint32 rval = IK_OK;
    uint32 window_calculate_updated = 0U;
    amba_ik_flow_tables_list_t *p_flow_tables_list;
    amba_ik_motion_fusion_flow_tables_list_t *p_motion_fusion_flow_tables_list;
    const amba_ik_r2y_flow_tables_list_t *p_r2y = NULL;
    uint32 video_pipe = p_filters->input_param.ability;

    if ((video_pipe == AMBA_IK_VIDEO_PIPE_LINEAR) ||
        (video_pipe == AMBA_IK_VIDEO_PIPE_LINEAR_CE) ||
        (video_pipe == AMBA_IK_VIDEO_PIPE_HDR_EXPO_2) ||
        (video_pipe == AMBA_IK_VIDEO_PIPE_HDR_EXPO_3) ||
        (video_pipe == AMBA_IK_VIDEO_PIPE_Y2Y) ||
        (video_pipe == AMBA_IK_VIDEO_PIPE_LINEAR_HVH)||
        (video_pipe == AMBA_IK_VIDEO_PIPE_LINEAR_CE_HVH) ||
        (video_pipe == AMBA_IK_VIDEO_PIPE_HDR_EXPO_2_HVH) ||
        (video_pipe == AMBA_IK_VIDEO_PIPE_HDR_EXPO_3_HVH) ||
        (video_pipe == AMBA_IK_VIDEO_PIPE_Y2Y_HVH) ||
        (video_pipe == AMBA_IK_VIDEO_PIPE_LINEAR_HHB)||
        (video_pipe == AMBA_IK_VIDEO_PIPE_LINEAR_CE_HHB) ||
        (video_pipe == AMBA_IK_VIDEO_PIPE_HDR_EXPO_2_HHB) ||
        (video_pipe == AMBA_IK_VIDEO_PIPE_HDR_EXPO_3_HHB) ||
        (video_pipe == AMBA_IK_VIDEO_PIPE_Y2Y_HHB) ||
        (video_pipe == AMBA_IK_STILL_PIPE_LISO)) {
        (void)amba_ik_system_memcpy(&p_flow_tables_list, &p_flow_tbl_list, sizeof(amba_ik_flow_tables_list_t*));
        p_r2y = &p_flow_tables_list->r2y;
    } else if ((video_pipe == AMBA_IK_VIDEO_PIPE_LINEAR_MD) ||
               (video_pipe == AMBA_IK_VIDEO_PIPE_LINEAR_CE_MD) ||
               (video_pipe == AMBA_IK_VIDEO_PIPE_HDR_EXPO_2_MD) ||
               (video_pipe == AMBA_IK_VIDEO_PIPE_HDR_EXPO_3_MD) ||
               (video_pipe == AMBA_IK_VIDEO_PIPE_FUSION) ||
               (video_pipe == AMBA_IK_VIDEO_PIPE_Y2Y_MD) ||
               (video_pipe == AMBA_IK_VIDEO_PIPE_LINEAR_MD_HHB) ||
               (video_pipe == AMBA_IK_VIDEO_PIPE_LINEAR_CE_MD_HHB) ||
               (video_pipe == AMBA_IK_VIDEO_PIPE_HDR_EXPO_2_MD_HHB) ||
               (video_pipe == AMBA_IK_VIDEO_PIPE_HDR_EXPO_3_MD_HHB) ||
               (video_pipe == AMBA_IK_VIDEO_PIPE_Y2Y_MD_HHB)) {
        (void)amba_ik_system_memcpy(&p_motion_fusion_flow_tables_list, &p_flow_tbl_list, sizeof(amba_ik_motion_fusion_flow_tables_list_t*));
        p_r2y = &p_motion_fusion_flow_tables_list->r2y;
    } else {
        // misraC
    }

    (void)amba_ik_system_memcpy(p_flow, p_flow, sizeof(uint8));//misra fix.

    if ((p_filters->update_flags.iso.window_size_info_updated == 1U) ||
        (p_filters->update_flags.iso.cfa_window_size_info_updated == 1U) ||
        (p_filters->update_flags.iso.dmy_range_updated == 1U) ||
        (p_filters->update_flags.iso.dzoom_info_updated == 1U) ||
        (p_filters->update_flags.iso.vin_active_win_updated == 1U) ||
        (p_filters->update_flags.iso.stitching_info_updated == 1U) ||
        (p_filters->update_flags.iso.overlap_x_updated == 1U) ||
        (p_filters->update_flags.iso.flip_mode_updated == 1U) ||
        (p_filters->update_flags.iso.warp_enable_updated == 1U) ||
        (p_filters->update_flags.iso.warp_enable_2nd_updated == 1U)) {
        window_calculate_updated = 1U;
    }

    if (p_r2y != NULL) {
        // chroma scale
        if(p_filters->update_flags.iso.chroma_scale_updated == 1U) {
            ikc_in_chroma_scale_t in_chroma_scale;
            ikc_out_chroma_scale_t out_chroma_scale;

            if(p_filters->update_flags.cr.r2y.cr_29_update == 0U) {
                amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_chroma_scale !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
                rval |= IK_ERR_0000;
            } else {
                if ((p_filters->input_param.ability == AMBA_IK_VIDEO_PIPE_Y2Y) ||
                    (p_filters->input_param.ability == AMBA_IK_VIDEO_PIPE_Y2Y_HVH) ||
                    (p_filters->input_param.ability == AMBA_IK_VIDEO_PIPE_Y2Y_HHB) ||
                    (p_filters->input_param.ability == AMBA_IK_VIDEO_PIPE_Y2Y_MD) ||
                    (p_filters->input_param.ability == AMBA_IK_VIDEO_PIPE_Y2Y_MD_HHB)) {
                    in_chroma_scale.is_yuv_mode = 1U;
                } else {
                    in_chroma_scale.is_yuv_mode = 0U;
                }
                in_chroma_scale.p_chroma_scale = &p_filters->input_param.chroma_scale;
                out_chroma_scale.p_cr_29 = p_r2y->p_CR_buf_29;
                out_chroma_scale.cr_29_size = CR_SIZE_29;
                rval |= ikc_chroma_scale(&in_chroma_scale, &out_chroma_scale);
            }
        }
        // chroma_median_filter
        if(p_filters->update_flags.iso.chroma_median_filter_updated == 1U) {
            ikc_in_chroma_median_t in_chroma_median;
            ikc_out_chroma_median_t out_chroma_median;

            if((p_filters->update_flags.cr.r2y.cr_30_update == 0U)||
               (p_filters->update_flags.cr.r2y.cr_31_update == 0U)) {
                amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_chroma_median !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
                rval |= IK_ERR_0000;
            } else {
                if ((p_filters->input_param.ability == AMBA_IK_VIDEO_PIPE_Y2Y) ||
                    (p_filters->input_param.ability == AMBA_IK_VIDEO_PIPE_Y2Y_HVH) ||
                    (p_filters->input_param.ability == AMBA_IK_VIDEO_PIPE_Y2Y_HHB) ||
                    (p_filters->input_param.ability == AMBA_IK_VIDEO_PIPE_Y2Y_MD) ||
                    (p_filters->input_param.ability == AMBA_IK_VIDEO_PIPE_Y2Y_MD_HHB)) {
                    in_chroma_median.is_yuv_mode = 1U;
                } else {
                    in_chroma_median.is_yuv_mode = 0U;
                }
                in_chroma_median.p_chroma_median_filter = &p_filters->input_param.chroma_median_filter;
                out_chroma_median.p_cr_30 = p_r2y->p_CR_buf_30;
                out_chroma_median.cr_30_size = CR_SIZE_30;
                out_chroma_median.p_cr_31 = p_r2y->p_CR_buf_31;
                out_chroma_median.cr_31_size = CR_SIZE_31;
                rval |= ikc_chroma_median(&in_chroma_median, &out_chroma_median);
            }
        }

        // chroma_noise_filter
        if((p_filters->update_flags.iso.chroma_filter_updated == 1U) || (p_filters->update_flags.iso.wide_chroma_filter == 1U) ||
           (p_filters->update_flags.iso.wide_chroma_filter_combine == 1U) || (window_calculate_updated == 1U)) {
            ikc_in_chroma_noise_t in_chroma_noise;
            ikc_out_chroma_noise_t out_chroma_noise;
            uint64 sec2_luma_hscale_phase_inc = p_filters->input_param.ctx_buf.result_win.warp_phase.sec2_hscale_phase_inc;
            uint64 sec2_luma_vscale_phase_inc = p_filters->input_param.ctx_buf.result_win.warp_phase.sec2_vscale_phase_inc;
            uint32 chroma_radius = p_filters->input_param.chroma_filter.radius;
            uint64 sec2_chroma_hscale_phase_inc;
            uint64 sec2_chroma_vscale_phase_inc;

            if(p_filters->update_flags.cr.r2y.cr_36_update == 0U) {
                amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_chroma_noise !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
                rval |= IK_ERR_0000;
            } else {
                in_chroma_noise.p_chroma_filter = &p_filters->input_param.chroma_filter;
                in_chroma_noise.p_wide_chroma_filter = &p_filters->input_param.wide_chroma_filter;
                in_chroma_noise.p_wide_chroma_combine = &p_filters->input_param.wide_chroma_combine;
                if ((in_chroma_noise.p_chroma_filter->radius==32u) && (in_chroma_noise.p_wide_chroma_filter->enable==1u)) {
                    in_chroma_noise.is_wide_chroma_invalid = 1u;
                } else {
                    in_chroma_noise.is_wide_chroma_invalid = 0u;
                }
                out_chroma_noise.p_cr_36 = p_r2y->p_CR_buf_36;
                out_chroma_noise.cr_36_size = CR_SIZE_36;
                out_chroma_noise.p_window = &p_flow->window;
                rval |= ikc_chroma_noise(&in_chroma_noise, &out_chroma_noise);
                p_flow->window.update_chroma_radius = 1U;
                //check radius causes phase_inc overflow.
                sec2_chroma_hscale_phase_inc = (sec2_luma_hscale_phase_inc * (uint64)chroma_radius) >> 5UL;
                sec2_chroma_vscale_phase_inc = (sec2_luma_vscale_phase_inc * (uint64)chroma_radius) >> 5UL;
                if ((sec2_chroma_hscale_phase_inc >= (8UL<<13UL)) || (sec2_chroma_vscale_phase_inc >= (8UL<<13UL))) {
                    amba_ik_system_print_uint32_5("[IK][ERROR] exe_iso_cfg_check_raw2yuv_chroma_filter() fail. One or more downscaling_factor is out of range(>=8<<13), please check chroma radius or window setting!", DC_U, DC_U, DC_U, DC_U, DC_U);
                    amba_ik_system_print_uint32_5("[IK][ERROR] sec2_chroma_hscale_phase_inc:%d, sec2_chroma_vscale_phase_inc:%d, chroma_radius:%d", (uint32)sec2_chroma_hscale_phase_inc, (uint32)sec2_chroma_vscale_phase_inc, chroma_radius, DC_U, DC_U);
                    rval = IK_ERR_0101;
                }
            }
        } else {
            p_flow->window.update_chroma_radius = 0U;
        }

        // luma_processing
        if(p_filters->input_param.first_luma_process_mode.use_sharpen_not_asf == 0U) {//asf
            ikc_out_asf_t out_asf;
            out_asf.p_cr_30 = p_r2y->p_CR_buf_30;
            out_asf.cr_30_size = CR_SIZE_30;
            if(p_filters->update_flags.iso.advance_spatial_filter_updated == 1U) {
                ikc_in_asf_t in_asf;

                if(p_filters->update_flags.cr.r2y.cr_30_update == 0U) {
                    amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_asf !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
                    rval |= IK_ERR_0000;
                } else {
                    in_asf.p_advance_spatial_filter = &p_filters->input_param.advance_spatial_filter;
                    in_asf.first_luma_processing_mode_updated = (uint32)p_filters->update_flags.iso.first_luma_processing_mode_updated;
                    rval |= ikc_asf(&in_asf, &out_asf);
                }
            }
        } else { //shpA
            ikc_out_sharpen_t out_sharpen;
            out_sharpen.p_cr_30 = p_r2y->p_CR_buf_30;
            out_sharpen.cr_30_size = CR_SIZE_30;

            if((p_filters->update_flags.iso.fstshpns_noise_updated == 1U)||(p_filters->update_flags.iso.fstshpns_fir_updated == 1U)) {
                ikc_in_sharpen_t in_sharpen;

                if(p_filters->update_flags.cr.r2y.cr_30_update == 0U) {
                    amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_sharpen !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
                    rval |= IK_ERR_0000;
                } else {
                    in_sharpen.sharpen_mode = p_filters->input_param.first_sharpen_both.mode;
                    in_sharpen.p_first_sharpen_noise = &p_filters->input_param.first_sharpen_noise;
                    in_sharpen.p_first_sharpen_fir = &p_filters->input_param.first_sharpen_fir;
                    in_sharpen.p_working_buffer = &p_filters->input_param.ctx_buf.first_sharpen_working_buffer[0];
                    in_sharpen.working_buffer_size = (uint32)sizeof(p_filters->input_param.ctx_buf.first_sharpen_working_buffer);
                    rval |= ikc_sharpen(&in_sharpen, &out_sharpen);
                }
            }

            if(p_filters->update_flags.iso.fstshpns_both_updated == 1U) {
                ikc_in_sharpen_both_t in_sharpen_both;

                if(p_filters->update_flags.cr.r2y.cr_30_update == 0U) {
                    amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_sharpen_both !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
                    rval |= IK_ERR_0000;
                } else {
                    in_sharpen_both.p_first_sharpen_both = &p_filters->input_param.first_sharpen_both;
                    rval |= ikc_sharpen_both(&in_sharpen_both, &out_sharpen);
                }
            }

            if(p_filters->update_flags.iso.fstshpns_coring_updated == 1U) {
                ikc_in_sharpen_coring_t in_sharpen_coring;

                if(p_filters->update_flags.cr.r2y.cr_30_update == 0U) {
                    amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_sharpen_coring !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
                    rval |= IK_ERR_0000;
                } else {
                    in_sharpen_coring.p_first_sharpen_coring = &p_filters->input_param.first_sharpen_coring;
                    rval |= ikc_sharpen_coring(&in_sharpen_coring, &out_sharpen);
                }
            }

            if(p_filters->update_flags.iso.fstshpns_coring_index_scale_updated == 1U) {
                ikc_in_sharpen_coring_idx_scale_t in_sharpen_coring_idx_scale;

                if(p_filters->update_flags.cr.r2y.cr_30_update == 0U) {
                    amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_sharpen_coring_idx_scale !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
                    rval |= IK_ERR_0000;
                } else {
                    in_sharpen_coring_idx_scale.p_first_sharpen_coring_idx_scale = &p_filters->input_param.first_sharpen_coring_idx_scale;
                    rval |= ikc_sharpen_coring_idx_scale(&in_sharpen_coring_idx_scale, &out_sharpen);
                }
            }

            if(p_filters->update_flags.iso.fstshpns_min_coring_result_updated == 1U) {
                ikc_in_sharpen_min_coring_t in_sharpen_min_coring;

                if(p_filters->update_flags.cr.r2y.cr_30_update == 0U) {
                    amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_sharpen_min_coring !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
                    rval |= IK_ERR_0000;
                } else {
                    in_sharpen_min_coring.p_first_sharpen_min_coring_result = &p_filters->input_param.first_sharpen_min_coring_result;
                    rval |= ikc_sharpen_min_coring(&in_sharpen_min_coring, &out_sharpen);
                }
            }

            if(p_filters->update_flags.iso.fstshpns_max_coring_result_updated == 1U) {
                ikc_in_sharpen_max_coring_t in_sharpen_max_coring;

                if(p_filters->update_flags.cr.r2y.cr_30_update == 0U) {
                    amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_sharpen_max_coring !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
                    rval |= IK_ERR_0000;
                } else {
                    in_sharpen_max_coring.p_first_sharpen_max_coring_result = &p_filters->input_param.first_sharpen_max_coring_result;
                    rval |= ikc_sharpen_max_coring(&in_sharpen_max_coring, &out_sharpen);
                }
            }

            if(p_filters->update_flags.iso.fstshpns_scale_coring_updated == 1U) {
                ikc_in_sharpen_scale_coring_t in_sharpen_scale_coring;

                if(p_filters->update_flags.cr.r2y.cr_30_update == 0U) {
                    amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_sharpen_scale_coring !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
                    rval |= IK_ERR_0000;
                } else {
                    in_sharpen_scale_coring.p_first_sharpen_scale_coring = &p_filters->input_param.first_sharpen_scale_coring;
                    rval |= ikc_sharpen_scale_coring(&in_sharpen_scale_coring, &out_sharpen);
                }
            }
        }

        // LNL
        if(((p_filters->update_flags.iso.after_ce_wb_gain_updated == 1U)&&(p_filters->input_param.ability != AMBA_IK_VIDEO_PIPE_Y2Y)&&(p_filters->input_param.ability != AMBA_IK_VIDEO_PIPE_Y2Y_HVH)&&(p_filters->input_param.ability != AMBA_IK_VIDEO_PIPE_Y2Y_HHB)&&(p_filters->input_param.ability != AMBA_IK_VIDEO_PIPE_Y2Y_MD)&&(p_filters->input_param.ability != AMBA_IK_VIDEO_PIPE_Y2Y_MD_HHB)) ||
           ((p_filters->update_flags.iso.rgb_to_12y_updated == 1U)&&(p_filters->input_param.ability != AMBA_IK_VIDEO_PIPE_Y2Y)&&(p_filters->input_param.ability != AMBA_IK_VIDEO_PIPE_Y2Y_HVH)&&(p_filters->input_param.ability != AMBA_IK_VIDEO_PIPE_Y2Y_HHB)&&(p_filters->input_param.ability != AMBA_IK_VIDEO_PIPE_Y2Y_MD)&&(p_filters->input_param.ability != AMBA_IK_VIDEO_PIPE_Y2Y_MD_HHB))||
           (p_filters->update_flags.iso.lnl_updated == 1U)||
           (window_calculate_updated == 1U)) {
            ikc_in_lnl_t in_lnl;
            ikc_out_lnl_t out_lnl;

            if(p_filters->update_flags.cr.r2y.cr_30_update == 0U) {
                amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_lnl !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
                rval |= IK_ERR_0000;
            } else {
                in_lnl.p_luma_noise_reduce = &p_filters->input_param.luma_noise_reduce;
                in_lnl.p_after_ce_wb_gain = &p_filters->input_param.after_ce_wb_gain;
                in_lnl.p_rgb_to_12y = &p_filters->input_param.rgb_to_12y;
                in_lnl.p_cfa_win = &p_filters->input_param.ctx_buf.result_win.cfa_win_dim;
                in_lnl.use_sharpen_not_asf = p_filters->input_param.first_luma_process_mode.use_sharpen_not_asf;
                in_lnl.p_advance_spatial_filter = &p_filters->input_param.advance_spatial_filter;
                in_lnl.p_first_sharpen_both = &p_filters->input_param.first_sharpen_both;
                out_lnl.p_cr_30 = p_r2y->p_CR_buf_30;
                out_lnl.cr_30_size = CR_SIZE_30;
                out_lnl.p_flow_info = &p_flow->flow_info;
                rval |= ikc_lnl(&in_lnl, &out_lnl);
            }
        }
        // LNL table
        if((p_filters->update_flags.iso.color_correction_updated == 1U)||
           (p_filters->update_flags.iso.tone_curve_updated == 1U)) {
            ikc_in_lnl_tbl_t in_lnl_tbl;
            ikc_out_lnl_t out_lnl;

            if(p_filters->update_flags.cr.r2y.cr_30_update == 0U) {
                amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_lnl_tbl !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
                rval |= IK_ERR_0000;
            } else {
                in_lnl_tbl.p_color_correction = &p_filters->input_param.color_correction;
                in_lnl_tbl.p_tone_curve = &p_filters->input_param.tone_curve;
                out_lnl.p_cr_30 = p_r2y->p_CR_buf_30;
                out_lnl.cr_30_size = CR_SIZE_30;
                rval |= ikc_lnl_tbl(&in_lnl_tbl, &out_lnl);
            }
        }

        if((p_filters->update_flags.iso.chroma_filter_updated == 1U)||
           (p_filters->update_flags.iso.resample_str_update == 1U)||
           (p_filters->update_flags.iso.window_size_info_updated == 1U)||
           (window_calculate_updated == 1U)) {
            if((p_filters->update_flags.cr.r2y.cr_33_update == 0U)||
               (p_filters->update_flags.cr.r2y.cr_35_update == 0U)||
               (p_filters->update_flags.cr.r2y.cr_37_update == 0U)||
               (p_filters->update_flags.cr.r2y.cr_42_update == 0U)) {
                amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_main_resampler !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
                rval |= IK_ERR_0000;
            } else {
                if ((video_pipe == AMBA_IK_VIDEO_PIPE_LINEAR) ||
                    (video_pipe == AMBA_IK_VIDEO_PIPE_LINEAR_CE) ||
                    (video_pipe == AMBA_IK_VIDEO_PIPE_HDR_EXPO_2) ||
                    (video_pipe == AMBA_IK_VIDEO_PIPE_HDR_EXPO_3) ||
                    (video_pipe == AMBA_IK_VIDEO_PIPE_Y2Y) ||
                    (video_pipe == AMBA_IK_VIDEO_PIPE_LINEAR_HVH)||
                    (video_pipe == AMBA_IK_VIDEO_PIPE_LINEAR_CE_HVH) ||
                    (video_pipe == AMBA_IK_VIDEO_PIPE_HDR_EXPO_2_HVH) ||
                    (video_pipe == AMBA_IK_VIDEO_PIPE_HDR_EXPO_3_HVH) ||
                    (video_pipe == AMBA_IK_VIDEO_PIPE_Y2Y_HVH) ||
                    (video_pipe == AMBA_IK_VIDEO_PIPE_LINEAR_HHB)||
                    (video_pipe == AMBA_IK_VIDEO_PIPE_LINEAR_CE_HHB) ||
                    (video_pipe == AMBA_IK_VIDEO_PIPE_HDR_EXPO_2_HHB) ||
                    (video_pipe == AMBA_IK_VIDEO_PIPE_HDR_EXPO_3_HHB) ||
                    (video_pipe == AMBA_IK_VIDEO_PIPE_Y2Y_HHB) ||
                    (video_pipe == AMBA_IK_STILL_PIPE_LISO) ||
                    (video_pipe == AMBA_IK_VIDEO_PIPE_FUSION)) {
                    ikc_in_sec2_main_resampler_t in_sec2_main_resampler;
                    ikc_out_sec2_main_resampler_t out_sec2_main_resampler;
                    ikc_in_sec3_vertical_resampler_t in_sec3_vertical_resampler;
                    ikc_out_sec3_vertical_resampler_t out_sec3_vertical_resampler;

                    in_sec2_main_resampler.main_cut_off_freq = p_filters->input_param.resample_str.main_cut_off_freq;
                    in_sec2_main_resampler.p_act_win_crop = &p_filters->input_param.ctx_buf.result_win.act_win_crop;
                    in_sec2_main_resampler.p_dmy_win_geo = &p_filters->input_param.ctx_buf.result_win.dmy_win_geo;
                    in_sec2_main_resampler.p_cfa_win_dim = &p_filters->input_param.ctx_buf.result_win.cfa_win_dim;
                    in_sec2_main_resampler.p_main = &p_filters->input_param.window_size_info.main_win;
                    in_sec2_main_resampler.p_dmy_range = &p_filters->input_param.dmy_range;
                    in_sec2_main_resampler.chroma_radius = p_filters->input_param.chroma_filter.radius;
                    in_sec2_main_resampler.is_y2y_420input = (p_filters->input_param.yuv_mode == AMBA_IK_YUV420) ? 1U : 0U;
                    in_sec2_main_resampler.flip_mode = p_filters->input_param.flip_mode;
                    in_sec2_main_resampler.luma_dma_size = p_filters->input_param.warp_buf_info.luma_dma_size;
                    in_sec2_main_resampler.p_burst_tile = &p_filters->input_param.burst_tile;
                    out_sec2_main_resampler.p_cr_33 = p_r2y->p_CR_buf_33;
                    out_sec2_main_resampler.cr_33_size = CR_SIZE_33;
                    out_sec2_main_resampler.p_cr_35 = p_r2y->p_CR_buf_35;
                    out_sec2_main_resampler.cr_35_size = CR_SIZE_35;
                    out_sec2_main_resampler.p_cr_37 = p_r2y->p_CR_buf_37;
                    out_sec2_main_resampler.cr_37_size = CR_SIZE_37;
                    rval |= ikc_sec2_main_resampler(&in_sec2_main_resampler, &out_sec2_main_resampler);

                    in_sec3_vertical_resampler.p_act_win_crop = &p_filters->input_param.ctx_buf.result_win.act_win_crop;
                    in_sec3_vertical_resampler.p_dmy_win_geo = &p_filters->input_param.ctx_buf.result_win.dmy_win_geo;
                    in_sec3_vertical_resampler.p_cfa_win_dim = &p_filters->input_param.ctx_buf.result_win.cfa_win_dim;
                    in_sec3_vertical_resampler.p_main = &p_filters->input_param.window_size_info.main_win;
                    in_sec3_vertical_resampler.p_dmy_range = &p_filters->input_param.dmy_range;
                    in_sec3_vertical_resampler.chroma_radius = p_filters->input_param.chroma_filter.radius;
                    in_sec3_vertical_resampler.is_y2y_420input = (p_filters->input_param.yuv_mode == AMBA_IK_YUV420) ? 1U : 0U;
                    in_sec3_vertical_resampler.flip_mode = p_filters->input_param.flip_mode;
                    out_sec3_vertical_resampler.p_cr_42 = p_r2y->p_CR_buf_42;
                    out_sec3_vertical_resampler.cr_42_size = CR_SIZE_42;
                    rval |= ikc_sec3_vertical_resampler(&in_sec3_vertical_resampler, &out_sec3_vertical_resampler);
                } else if ((video_pipe == AMBA_IK_VIDEO_PIPE_LINEAR_MD) ||
                           (video_pipe == AMBA_IK_VIDEO_PIPE_LINEAR_CE_MD) ||
                           (video_pipe == AMBA_IK_VIDEO_PIPE_HDR_EXPO_2_MD) ||
                           (video_pipe == AMBA_IK_VIDEO_PIPE_HDR_EXPO_3_MD) ||
                           (video_pipe == AMBA_IK_VIDEO_PIPE_Y2Y_MD) ||
                           (video_pipe == AMBA_IK_VIDEO_PIPE_LINEAR_MD_HHB) ||
                           (video_pipe == AMBA_IK_VIDEO_PIPE_LINEAR_CE_MD_HHB) ||
                           (video_pipe == AMBA_IK_VIDEO_PIPE_HDR_EXPO_2_MD_HHB) ||
                           (video_pipe == AMBA_IK_VIDEO_PIPE_HDR_EXPO_3_MD_HHB) ||
                           (video_pipe == AMBA_IK_VIDEO_PIPE_Y2Y_MD_HHB)) {
                    ikc_in_sec2_optc_main_resampler_t in_sec2_optc_main_resampler;
                    ikc_out_sec2_optc_main_resampler_t out_sec2_optc_main_resampler;
                    ikc_in_sec3_optc_vertical_resampler_t in_sec3_optc_vertical_resampler;
                    ikc_out_sec3_optc_vertical_resampler_t out_sec3_optc_vertical_resampler;

                    in_sec2_optc_main_resampler.main_cut_off_freq = p_filters->input_param.resample_str.main_cut_off_freq;
                    in_sec2_optc_main_resampler.p_act_win_crop = &p_filters->input_param.ctx_buf.result_win.act_win_crop;
                    in_sec2_optc_main_resampler.p_dmy_win_geo = &p_filters->input_param.ctx_buf.result_win.dmy_win_geo;
                    in_sec2_optc_main_resampler.p_cfa_win_dim = &p_filters->input_param.ctx_buf.result_win.cfa_win_dim;
                    in_sec2_optc_main_resampler.p_main = &p_filters->input_param.window_size_info.main_win;
                    in_sec2_optc_main_resampler.p_dmy_range = &p_filters->input_param.dmy_range;
                    in_sec2_optc_main_resampler.chroma_radius = p_filters->input_param.chroma_filter.radius;
                    in_sec2_optc_main_resampler.is_y2y_420input = (p_filters->input_param.yuv_mode == AMBA_IK_YUV420) ? 1U : 0U;
                    in_sec2_optc_main_resampler.flip_mode = p_filters->input_param.flip_mode;
                    in_sec2_optc_main_resampler.luma_dma_size = p_filters->input_param.warp_buf_info.luma_dma_size;
                    in_sec2_optc_main_resampler.p_burst_tile= &p_filters->input_param.burst_tile;
                    out_sec2_optc_main_resampler.p_cr_33 = p_r2y->p_CR_buf_33;
                    out_sec2_optc_main_resampler.cr_33_size = CR_SIZE_33;
                    out_sec2_optc_main_resampler.p_cr_35 = p_r2y->p_CR_buf_35;
                    out_sec2_optc_main_resampler.cr_35_size = CR_SIZE_35;
                    out_sec2_optc_main_resampler.p_cr_37 = p_r2y->p_CR_buf_37;
                    out_sec2_optc_main_resampler.cr_37_size = CR_SIZE_37;
                    rval |= ikc_sec2_optc_main_resampler(&in_sec2_optc_main_resampler, &out_sec2_optc_main_resampler);

                    in_sec3_optc_vertical_resampler.p_act_win_crop = &p_filters->input_param.ctx_buf.result_win.act_win_crop;
                    in_sec3_optc_vertical_resampler.p_dmy_win_geo = &p_filters->input_param.ctx_buf.result_win.dmy_win_geo;
                    in_sec3_optc_vertical_resampler.p_cfa_win_dim = &p_filters->input_param.ctx_buf.result_win.cfa_win_dim;
                    in_sec3_optc_vertical_resampler.p_main = &p_filters->input_param.window_size_info.main_win;
                    in_sec3_optc_vertical_resampler.p_dmy_range = &p_filters->input_param.dmy_range;
                    in_sec3_optc_vertical_resampler.chroma_radius = p_filters->input_param.chroma_filter.radius;
                    in_sec3_optc_vertical_resampler.is_y2y_420input = (p_filters->input_param.yuv_mode == AMBA_IK_YUV420) ? 1U : 0U;
                    in_sec3_optc_vertical_resampler.flip_mode = p_filters->input_param.flip_mode;
                    out_sec3_optc_vertical_resampler.p_cr_42 = p_r2y->p_CR_buf_42;
                    out_sec3_optc_vertical_resampler.cr_42_size = CR_SIZE_42;
                    rval |= ikc_sec3_optc_vertical_resampler(&in_sec3_optc_vertical_resampler, &out_sec3_optc_vertical_resampler);
                } else {
                    // default
                }
            }
        }
#if 0
        if((p_filters->update_flags.iso.warp_enable_2nd_updated == 1U)||
           (p_filters->update_flags.iso.calib_warp_2nd_info_updated == 1U)||
           (p_filters->update_flags.iso.warp_internal_2nd_updated == 1U)||
           (p_filters->update_flags.iso.window_size_info_updated == 1U)||
           (p_filters->update_flags.iso.chroma_filter_updated == 1U)||
           (window_calculate_updated == 1U)) {
            if((p_filters->update_flags.cr.warp_2nd == 0U)||
               (p_filters->update_flags.cr.r2y.cr_100_update == 0U)||
               (p_filters->update_flags.cr.r2y.cr_101_update == 0U)) {
                amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_secondary_hwarp !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
                rval |= IK_ERR_0000;
            } else {
                ikc_in_sec11_main_resampler_t in_sec11_main_resampler;
                ikc_out_sec11_main_resampler_t out_sec11_main_resampler;

                if ((video_pipe == AMBA_IK_VIDEO_PIPE_LINEAR_HHB) ||
                    (video_pipe == AMBA_IK_VIDEO_PIPE_LINEAR_CE_HHB) ||
                    (video_pipe == AMBA_IK_VIDEO_PIPE_HDR_EXPO_2_HHB) ||
                    (video_pipe == AMBA_IK_VIDEO_PIPE_HDR_EXPO_3_HHB) ||
                    (video_pipe == AMBA_IK_VIDEO_PIPE_LINEAR_MD_HHB) ||
                    (video_pipe == AMBA_IK_VIDEO_PIPE_LINEAR_CE_MD_HHB) ||
                    (video_pipe == AMBA_IK_VIDEO_PIPE_HDR_EXPO_2_MD_HHB) ||
                    (video_pipe == AMBA_IK_VIDEO_PIPE_HDR_EXPO_3_MD_HHB) ||
                    (video_pipe == AMBA_IK_VIDEO_PIPE_Y2Y_HHB) ||
                    (video_pipe == AMBA_IK_VIDEO_PIPE_Y2Y_MD_HHB)) {
                    in_sec11_main_resampler.is_hhb_enabled = 1u;
                } else {
                    in_sec11_main_resampler.is_hhb_enabled = 0u;
                }
                in_sec11_main_resampler.main_cut_off_freq = p_filters->input_param.resample_str.main_cut_off_freq;
                in_sec11_main_resampler.p_act_win_crop = &p_filters->input_param.ctx_buf.result_win.act_win_crop;
                in_sec11_main_resampler.p_dmy_win_geo = &p_filters->input_param.ctx_buf.result_win.dmy_win_geo;
                in_sec11_main_resampler.p_cfa_win_dim = &p_filters->input_param.ctx_buf.result_win.cfa_win_dim;
                in_sec11_main_resampler.p_main = &p_filters->input_param.window_size_info.main_win;
                in_sec11_main_resampler.p_dmy_range = &p_filters->input_param.dmy_range;
                in_sec11_main_resampler.chroma_radius = p_filters->input_param.chroma_filter.radius;
                in_sec11_main_resampler.flip_mode = p_filters->input_param.flip_mode;

                out_sec11_main_resampler.p_cr_100 = p_r2y->p_CR_buf_100;
                out_sec11_main_resampler.cr_100_size = CR_SIZE_100;

                rval |= ikc_sec11_main_resampler(&in_sec11_main_resampler, &out_sec11_main_resampler);
            }
       }
#endif
    }

    rval |= exe_check_burst_tile_mode(p_filters);

    return rval;
}

uint32 exe_process_hdr_ce_sub_cr(void *p_flow_tbl_list, amba_ik_filter_t *p_filters, uint32 exp_num)
{
    uint32 rval = IK_OK;
    uint32 window_calculate_updated = 0U;
    amba_ik_flow_tables_list_t *p_flow_tables_list;
    amba_ik_motion_fusion_flow_tables_list_t *p_motion_fusion_flow_tables_list;
    const amba_ik_r2y_flow_tables_list_t *p_r2y = NULL;
    uint32 video_pipe = p_filters->input_param.ability;

    if ((video_pipe == AMBA_IK_VIDEO_PIPE_LINEAR) ||
        (video_pipe == AMBA_IK_VIDEO_PIPE_LINEAR_CE) ||
        (video_pipe == AMBA_IK_VIDEO_PIPE_HDR_EXPO_2) ||
        (video_pipe == AMBA_IK_VIDEO_PIPE_HDR_EXPO_3) ||
        (video_pipe == AMBA_IK_VIDEO_PIPE_Y2Y) ||
        (video_pipe == AMBA_IK_VIDEO_PIPE_LINEAR_HVH)||
        (video_pipe == AMBA_IK_VIDEO_PIPE_LINEAR_CE_HVH) ||
        (video_pipe == AMBA_IK_VIDEO_PIPE_HDR_EXPO_2_HVH) ||
        (video_pipe == AMBA_IK_VIDEO_PIPE_HDR_EXPO_3_HVH) ||
        (video_pipe == AMBA_IK_VIDEO_PIPE_Y2Y_HVH) ||
        (video_pipe == AMBA_IK_VIDEO_PIPE_LINEAR_HHB)||
        (video_pipe == AMBA_IK_VIDEO_PIPE_LINEAR_CE_HHB) ||
        (video_pipe == AMBA_IK_VIDEO_PIPE_HDR_EXPO_2_HHB) ||
        (video_pipe == AMBA_IK_VIDEO_PIPE_HDR_EXPO_3_HHB) ||
        (video_pipe == AMBA_IK_VIDEO_PIPE_Y2Y_HHB) ||
        (video_pipe == AMBA_IK_STILL_PIPE_LISO)) {
        (void)amba_ik_system_memcpy(&p_flow_tables_list, &p_flow_tbl_list, sizeof(amba_ik_flow_tables_list_t*));
        p_r2y = &p_flow_tables_list->r2y;
    } else if ((video_pipe == AMBA_IK_VIDEO_PIPE_LINEAR_MD) ||
               (video_pipe == AMBA_IK_VIDEO_PIPE_LINEAR_CE_MD) ||
               (video_pipe == AMBA_IK_VIDEO_PIPE_HDR_EXPO_2_MD) ||
               (video_pipe == AMBA_IK_VIDEO_PIPE_HDR_EXPO_3_MD) ||
               (video_pipe == AMBA_IK_VIDEO_PIPE_FUSION) ||
               (video_pipe == AMBA_IK_VIDEO_PIPE_Y2Y_MD) ||
               (video_pipe == AMBA_IK_VIDEO_PIPE_LINEAR_MD_HHB) ||
               (video_pipe == AMBA_IK_VIDEO_PIPE_LINEAR_CE_MD_HHB) ||
               (video_pipe == AMBA_IK_VIDEO_PIPE_HDR_EXPO_2_MD_HHB) ||
               (video_pipe == AMBA_IK_VIDEO_PIPE_HDR_EXPO_3_MD_HHB) ||
               (video_pipe == AMBA_IK_VIDEO_PIPE_Y2Y_MD_HHB)) {
        (void)amba_ik_system_memcpy(&p_motion_fusion_flow_tables_list, &p_flow_tbl_list, sizeof(amba_ik_motion_fusion_flow_tables_list_t*));
        p_r2y = &p_motion_fusion_flow_tables_list->r2y;
    } else {
        // misraC
    }

    if ((p_filters->update_flags.iso.window_size_info_updated == 1U) ||
        (p_filters->update_flags.iso.cfa_window_size_info_updated == 1U) ||
        (p_filters->update_flags.iso.dmy_range_updated == 1U) ||
        (p_filters->update_flags.iso.dzoom_info_updated == 1U) ||
        (p_filters->update_flags.iso.vin_active_win_updated == 1U) ||
        (p_filters->update_flags.iso.stitching_info_updated == 1U) ||
        (p_filters->update_flags.iso.overlap_x_updated == 1U) ||
        (p_filters->update_flags.iso.flip_mode_updated == 1U) ||
        (p_filters->update_flags.iso.warp_enable_updated == 1U) ||
        (p_filters->update_flags.iso.warp_enable_2nd_updated == 1U)) {
        window_calculate_updated = 1U;
    }

    if (p_r2y != NULL) {
        if ((p_filters->input_param.ability != AMBA_IK_VIDEO_PIPE_Y2Y) &&
            (p_filters->input_param.ability != AMBA_IK_VIDEO_PIPE_Y2Y_HVH) &&
            (p_filters->input_param.ability != AMBA_IK_VIDEO_PIPE_Y2Y_HHB) &&
            (p_filters->input_param.ability != AMBA_IK_VIDEO_PIPE_Y2Y_MD) &&
            (p_filters->input_param.ability != AMBA_IK_VIDEO_PIPE_Y2Y_MD_HHB)) {
            // input mode sub
            if((p_filters->update_flags.iso.is_1st_frame == 1U) && (p_filters->update_flags.iso.sensor_information_updated == 1u)) { //should only configure once at 1st time.
                ikc_in_input_mode_sub_t in_input_mode_sub;
                ikc_out_input_mode_sub_t out_input_mode_sub;

                if((p_filters->update_flags.cr.r2y.cr_45_update == 0U)||
                   (p_filters->update_flags.cr.r2y.cr_47_update == 0U)||
                   (p_filters->update_flags.cr.r2y.cr_49_update == 0U)||
                   (p_filters->update_flags.cr.r2y.cr_50_update == 0U)||
                   (p_filters->update_flags.cr.r2y.cr_51_update == 0U)) {
                    amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_input_mode_sub !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
                    rval |= IK_ERR_0000;
                } else {
                    in_input_mode_sub.flip_mode = p_filters->input_param.flip_mode;
                    in_input_mode_sub.is_yuv_mode = 0u;
                    in_input_mode_sub.sensor_pattern = p_filters->input_param.sensor_info.sensor_pattern;
                    out_input_mode_sub.p_cr_45 = p_r2y->p_CR_buf_45;
                    out_input_mode_sub.cr_45_size = CR_SIZE_45;
                    out_input_mode_sub.p_cr_47 = p_r2y->p_CR_buf_47;
                    out_input_mode_sub.cr_47_size = CR_SIZE_47;
                    out_input_mode_sub.p_cr_49 = p_r2y->p_CR_buf_49;
                    out_input_mode_sub.cr_49_size = CR_SIZE_49;
                    out_input_mode_sub.p_cr_50 = p_r2y->p_CR_buf_50;
                    out_input_mode_sub.cr_50_size = CR_SIZE_50;
                    out_input_mode_sub.p_cr_51 = p_r2y->p_CR_buf_51;
                    out_input_mode_sub.cr_51_size = CR_SIZE_51;

                    rval |= ikc_input_mode_sub(&in_input_mode_sub, &out_input_mode_sub);
                }
            }

            // before_ce_gain sub
            if(p_filters->update_flags.iso.before_ce_wb_gain_updated == 1u) {
                ikc_in_before_ce_gain_t in_before_ce_gain;
                ikc_out_before_ce_gain_sub_t out_before_ce_gain_sub;

                if(p_filters->update_flags.cr.r2y.cr_51_update == 0U) {
                    amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_before_ce_gain_sub !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
                    rval |= IK_ERR_0000;
                } else {
                    in_before_ce_gain.p_before_ce_wb_gain = &p_filters->input_param.before_ce_wb_gain;
                    out_before_ce_gain_sub.p_cr_51 = p_r2y->p_CR_buf_51;
                    out_before_ce_gain_sub.cr_51_size = CR_SIZE_51;
                    rval |= ikc_before_ce_gain_sub(&in_before_ce_gain, &out_before_ce_gain_sub);
                }
            }

            if(exp_num < 4U) {
                // hdr_blend sub and hdr_alpha sub
                if(p_filters->update_flags.iso.hdr_blend_updated == 1u) {
                    ikc_in_hdr_blend_t in_hdr_blend;
                    ikc_out_hdr_blend_sub_t out_hdr_blend_sub;

                    if(p_filters->update_flags.cr.r2y.cr_45_update == 0U) {
                        amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_hdr_blend_sub !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
                        rval |= IK_ERR_0000;
                    } else {
                        in_hdr_blend.exp_num = exp_num;
                        in_hdr_blend.p_hdr_raw_info = &p_filters->input_param.hdr_raw_info;
                        in_hdr_blend.p_hdr_blend = &p_filters->input_param.hdr_blend;
                        out_hdr_blend_sub.p_cr_45 = p_r2y->p_CR_buf_45;
                        out_hdr_blend_sub.cr_45_size = CR_SIZE_45;
                        rval |= ikc_hdr_blend_sub(&in_hdr_blend, &out_hdr_blend_sub);
                    }
                }

                // hdr_tone_curve sub
                if(p_filters->update_flags.iso.fe_tone_curve_updated == 1u) {
                    ikc_in_hdr_tone_curve_t in_hdr_tc;
                    ikc_out_hdr_tone_curve_sub_t out_hdr_tc_sub;

                    if((p_filters->update_flags.cr.r2y.cr_45_update == 0U)||
                       (p_filters->update_flags.cr.r2y.cr_46_update == 0U)) {
                        amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_front_end_tone_curve_sub !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
                        rval |= IK_ERR_0000;
                    } else {
                        in_hdr_tc.p_fe_tone_curve = &p_filters->input_param.fe_tone_curve;
                        out_hdr_tc_sub.p_cr_45 = p_r2y->p_CR_buf_45;
                        out_hdr_tc_sub.cr_45_size = CR_SIZE_45;
                        out_hdr_tc_sub.p_cr_46 = p_r2y->p_CR_buf_46;
                        out_hdr_tc_sub.cr_46_size = CR_SIZE_46;
                        rval |= ikc_front_end_tone_curve_sub(&in_hdr_tc, &out_hdr_tc_sub);
                    }
                }

                // losy decompress sub
                if((p_filters->update_flags.iso.sensor_information_updated == 1u) || (window_calculate_updated == 1U)) {
                    ikc_in_decompress_t in_decompress;
                    ikc_out_decompress_sub_t out_decompress_sub;

                    if(p_filters->update_flags.cr.r2y.cr_45_update == 0U) {
                        amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_decompression_sub !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
                        rval |= IK_ERR_0000;
                    } else {
                        in_decompress.exp_num = exp_num;
                        in_decompress.compress_mode = p_filters->input_param.sensor_info.compression;//0~8, 256~259.
                        in_decompress.p_dmy_win_geo = &p_filters->input_param.ctx_buf.result_win.dmy_win_geo;
                        out_decompress_sub.p_cr_45 = p_r2y->p_CR_buf_45;
                        out_decompress_sub.cr_45_size = CR_SIZE_45;
                        rval |= ikc_decompression_sub(&in_decompress, &out_decompress_sub);
                    }
                }

                // hdr exp sub
                if((p_filters->update_flags.iso.exp0_fe_static_blc_updated == 1u) || (p_filters->update_flags.iso.exp1_fe_static_blc_updated == 1u) ||
                   (p_filters->update_flags.iso.exp2_fe_static_blc_updated == 1u)) {
                    ikc_in_hdr_blc_t in_hdr_blc;
                    ikc_out_hdr_blc_sub_t out_hdr_blc_sub;

                    if(p_filters->update_flags.cr.r2y.cr_45_update == 0U) {
                        amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_hdr_black_level_sub !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
                        rval |= IK_ERR_0000;
                    } else {
                        in_hdr_blc.exp_num = exp_num;
                        in_hdr_blc.rgb_ir_mode = p_filters->input_param.rgb_ir.mode;
                        in_hdr_blc.compression_offset = ((p_filters->input_param.sensor_info.compression > IK_RAW_COMPRESS_OFF)&&(p_filters->input_param.sensor_info.compression <= IK_RAW_COMPRESS_10p5)) ? p_filters->input_param.ctx_buf.first_compression_offset : 0;
                        (void)amba_ik_system_memcpy(&p_filters->input_param.ctx_buf.ir_only_trick_buffer.exp0_frontend_static_blc, &p_filters->input_param.exp0_frontend_static_blc, sizeof(ik_static_blc_level_t));
                        (void)amba_ik_system_memcpy(&p_filters->input_param.ctx_buf.ir_only_trick_buffer.exp1_frontend_static_blc, &p_filters->input_param.exp1_frontend_static_blc, sizeof(ik_static_blc_level_t));
                        (void)amba_ik_system_memcpy(&p_filters->input_param.ctx_buf.ir_only_trick_buffer.exp2_frontend_static_blc, &p_filters->input_param.exp2_frontend_static_blc, sizeof(ik_static_blc_level_t));
                        in_hdr_blc.p_exp0_frontend_static_blc = &p_filters->input_param.ctx_buf.ir_only_trick_buffer.exp0_frontend_static_blc;
                        in_hdr_blc.p_exp1_frontend_static_blc = &p_filters->input_param.ctx_buf.ir_only_trick_buffer.exp1_frontend_static_blc;
                        in_hdr_blc.p_exp2_frontend_static_blc = &p_filters->input_param.ctx_buf.ir_only_trick_buffer.exp2_frontend_static_blc;
                        out_hdr_blc_sub.p_cr_45 = p_r2y->p_CR_buf_45;
                        out_hdr_blc_sub.cr_45_size = CR_SIZE_45;
                        if((p_filters->input_param.rgb_ir.ir_only==1U)&&(p_filters->input_param.sensor_info.sensor_mode==1U)) { //ir_only, runing on RGB bayer pattern, carry IR on B channel.
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

                if((p_filters->update_flags.iso.exp0_fe_wb_gain_updated == 1u) || (p_filters->update_flags.iso.exp1_fe_wb_gain_updated == 1u) ||
                   (p_filters->update_flags.iso.exp2_fe_wb_gain_updated == 1u)) {
                    ikc_in_hdr_dgain_t in_hdr_dgain = {0};
                    ikc_out_hdr_dgain_sub_t out_hdr_dgain_sub;

                    if(p_filters->update_flags.cr.r2y.cr_45_update == 0U) {
                        amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_hdr_dgain_sub !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
                        rval |= IK_ERR_0000;
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
                        out_hdr_dgain_sub.p_cr_45 = p_r2y->p_CR_buf_45;
                        out_hdr_dgain_sub.cr_45_size = CR_SIZE_45;
                        if((p_filters->input_param.rgb_ir.ir_only==1U)&&(p_filters->input_param.sensor_info.sensor_mode==1U)) { //ir_only, runing on RGB bayer pattern, carry IR on B channel.
                            in_hdr_dgain.rgb_ir_mode = 0;
                            in_hdr_dgain.p_exp0_frontend_wb_gain->b_gain = in_hdr_dgain.p_exp0_frontend_wb_gain->ir_gain;
                            in_hdr_dgain.p_exp1_frontend_wb_gain->b_gain = in_hdr_dgain.p_exp1_frontend_wb_gain->ir_gain;
                            in_hdr_dgain.p_exp2_frontend_wb_gain->b_gain = in_hdr_dgain.p_exp2_frontend_wb_gain->ir_gain;
                        }
                        rval |= ikc_hdr_dgain_sub(&in_hdr_dgain, &out_hdr_dgain_sub);
                    }
                }

                // hdr eis sub
                if((p_filters->input_param.use_hdr_eis == 1U)&&
                   ((p_filters->update_flags.iso.hdr_eis_shift_info_updated == 1u)||
                    (p_filters->update_flags.iso.sensor_information_updated == 1u) || (window_calculate_updated == 1U))) {
                    ikc_in_hdr_eis_t in_hdr_eis_sub;
                    ikc_out_hdr_eis_sub_t out_hdr_eis_sub;

                    if(p_filters->update_flags.cr.r2y.cr_45_update == 0U) {
                        amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_decompression_sub !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
                        rval |= IK_ERR_0000;
                    } else {
                        in_hdr_eis_sub.exp_num = exp_num;
                        in_hdr_eis_sub.p_dmy_win_geo = &p_filters->input_param.ctx_buf.result_win.dmy_win_geo;
                        in_hdr_eis_sub.p_vin_sensor = &p_filters->input_param.window_size_info.vin_sensor;
                        in_hdr_eis_sub.horizontal_shift_exp1 = p_filters->input_param.ctx_buf.result_win.hdr_eis_sft.horizontal_shift_exp1;
                        in_hdr_eis_sub.vertical_shift_exp1 = p_filters->input_param.ctx_buf.result_win.hdr_eis_sft.vertical_shift_exp1;
                        in_hdr_eis_sub.horizontal_shift_exp2 = p_filters->input_param.ctx_buf.result_win.hdr_eis_sft.horizontal_shift_exp2;
                        in_hdr_eis_sub.vertical_shift_exp2 = p_filters->input_param.ctx_buf.result_win.hdr_eis_sft.vertical_shift_exp2;
                        out_hdr_eis_sub.p_cr_45 = p_r2y->p_CR_buf_45;
                        out_hdr_eis_sub.cr_45_size = CR_SIZE_45;
                        rval |= ikc_hdr_eis_sub(&in_hdr_eis_sub, &out_hdr_eis_sub);
                    }
                }

                // ce sub
                if(exp_num > 0U) {
                    if((p_filters->update_flags.iso.ce_updated == 1u) || (window_calculate_updated == 1U)) {
                        ikc_in_ce_t in_ce;
                        ikc_out_ce_sub_t out_ce_sub;

                        if((p_filters->update_flags.cr.r2y.cr_49_update == 0U)||
                           (p_filters->update_flags.cr.r2y.cr_50_update == 0U)||
                           (p_filters->update_flags.cr.r2y.cr_51_update == 0U)) {
                            amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_contrast_enhancement_sub !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
                            rval |= IK_ERR_0000;
                        } else {
                            in_ce.sensor_pattern = p_filters->input_param.sensor_info.sensor_pattern;
                            in_ce.p_dmy_win_geo = &p_filters->input_param.ctx_buf.result_win.dmy_win_geo;
                            in_ce.p_ce = &p_filters->input_param.ce;
                            out_ce_sub.p_cr_49 = p_r2y->p_CR_buf_49;
                            out_ce_sub.cr_49_size = CR_SIZE_49;
                            out_ce_sub.p_cr_50 = p_r2y->p_CR_buf_50;
                            out_ce_sub.cr_50_size = CR_SIZE_50;
                            out_ce_sub.p_cr_51 = p_r2y->p_CR_buf_51;
                            out_ce_sub.cr_51_size = CR_SIZE_51;
                            rval |= ikc_contrast_enhancement_sub(&in_ce, &out_ce_sub);
                        }
                    }
                    if(p_filters->update_flags.iso.ce_input_table_updated == 1u) {
                        ikc_in_ce_input_sub_t in_ce_input_sub;
                        ikc_out_ce_input_sub_t out_ce_input_sub;

                        if((p_filters->update_flags.cr.r2y.cr_51_update == 0U)||
                           (p_filters->update_flags.cr.r2y.cr_52_update == 0U)) {
                            amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_contrast_enhancement_input_sub !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
                            rval |= IK_ERR_0000;
                        } else {
                            in_ce_input_sub.ce_enable = p_filters->input_param.ce.enable;
                            in_ce_input_sub.radius = p_filters->input_param.ce.radius;
                            in_ce_input_sub.epsilon = p_filters->input_param.ce.epsilon;
                            in_ce_input_sub.p_ce_input_table = &p_filters->input_param.ce_input_table;
                            out_ce_input_sub.p_cr_51 = p_r2y->p_CR_buf_51;
                            out_ce_input_sub.cr_51_size = CR_SIZE_51;
                            out_ce_input_sub.p_cr_52 = p_r2y->p_CR_buf_52;
                            out_ce_input_sub.cr_52_size = CR_SIZE_52;
                            rval |= ikc_contrast_enhancement_input_sub(&in_ce_input_sub, &out_ce_input_sub);
                        }
                    }
                    // vig sub
                    if ((p_filters->update_flags.iso.vignette_compensation_updated == 1u) || (p_filters->update_flags.iso.window_size_info_updated == 1U) ||
                        (window_calculate_updated == 1U)) {
                        ikc_in_vignette_t in_vig;
                        ikc_out_vignette_sub_t out_vig_sub;

                        if((p_filters->update_flags.cr.r2y.cr_47_update == 0U)||
                           (p_filters->update_flags.cr.r2y.cr_48_update == 0U)||
                           (p_filters->update_flags.cr.r2y.cr_49_update == 0U)) {
                            amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_vignette_sub !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
                            rval |= IK_ERR_0000;
                        } else {
                            in_vig.vig_enable = p_filters->input_param.vig_enable;
                            in_vig.flip_mode = p_filters->input_param.flip_mode;
                            in_vig.p_vignette_compensation = &p_filters->input_param.vignette_compensation;
                            in_vig.p_vin_sensor = &p_filters->input_param.window_size_info.vin_sensor;
                            in_vig.p_dmy_win_geo = &p_filters->input_param.ctx_buf.result_win.dmy_win_geo;
                            in_vig.p_active_window = &p_filters->input_param.active_window;
                            out_vig_sub.p_cr_47 = p_r2y->p_CR_buf_47;
                            out_vig_sub.cr_47_size = CR_SIZE_47;
                            out_vig_sub.p_cr_48 = p_r2y->p_CR_buf_48;
                            out_vig_sub.cr_48_size = CR_SIZE_48;
                            out_vig_sub.p_cr_49 = p_r2y->p_CR_buf_49;
                            out_vig_sub.cr_49_size = CR_SIZE_49;
                            rval |= ikc_vignette_sub(&in_vig, &out_vig_sub);
                        }
                    }
                    // rgbir sub
                    if(p_filters->update_flags.iso.rgb_ir_updated == 1u) {
                        ikc_in_rgb_ir_t in_rgb_ir;
                        ikc_out_rgb_ir_sub_t out_rgb_ir_sub;

                        if((p_filters->update_flags.cr.r2y.cr_45_update == 0U)||
                           (p_filters->update_flags.cr.r2y.cr_47_update == 0U)||
                           (p_filters->update_flags.cr.r2y.cr_49_update == 0U)||
                           (p_filters->update_flags.cr.r2y.cr_50_update == 0U)) {
                            amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_rgb_ir_sub !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
                            rval |= IK_ERR_0000;
                        } else {
                            in_rgb_ir.p_rgb_ir = &p_filters->input_param.rgb_ir;
                            in_rgb_ir.p_before_ce_wb_gain = &p_filters->input_param.before_ce_wb_gain;
                            in_rgb_ir.p_after_ce_wb_gain = &p_filters->input_param.after_ce_wb_gain;
                            in_rgb_ir.p_exp0_frontend_wb_gain = &p_filters->input_param.exp0_frontend_wb_gain;
                            out_rgb_ir_sub.p_cr_45 = p_r2y->p_CR_buf_45;
                            out_rgb_ir_sub.cr_45_size = CR_SIZE_45;
                            out_rgb_ir_sub.p_cr_47 = p_r2y->p_CR_buf_47;
                            out_rgb_ir_sub.cr_47_size = CR_SIZE_47;
                            out_rgb_ir_sub.p_cr_49 = p_r2y->p_CR_buf_49;
                            out_rgb_ir_sub.cr_49_size = CR_SIZE_49;
                            out_rgb_ir_sub.p_cr_50 = p_r2y->p_CR_buf_50;
                            out_rgb_ir_sub.cr_50_size = CR_SIZE_50;
                            rval |= ikc_rgb_ir_sub(&in_rgb_ir, &out_rgb_ir_sub);
                        }
                    }
                }
            } else {
                amba_ik_system_print_uint32_5("[IK] exe_process_hdr_ce_sub_cr fail, incorrect exp_num %d", exp_num, DC_U, DC_U, DC_U, DC_U);
                rval = IK_ERR_0000;
            }
        } else {
            amba_ik_system_print_uint32_5("[IK] exe_process_hdr_ce_sub_cr fail, do not support y2y mode!", DC_U, DC_U, DC_U, DC_U, DC_U);
            rval = IK_ERR_0004;
        }
    }

    return rval;
}

static INLINE uint32 ik_check_mctf_mcts_three_d_table(const amba_ik_filter_t *p_filters)
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

    if(p_filters->input_param.video_mctf.y_advanced_iso.max_change_method == 1U) {
        amba_ik_system_print_str_5("[IK][ERROR] three_d_table is disable. video_mctf.y_advanced_iso.max_change_method can not be 1\n", DC_S, DC_S, DC_S, DC_S, DC_S);
        rval |= IK_ERR_0008;
    }
    if(p_filters->input_param.video_mctf.cb_advanced_iso.max_change_method == 1U) {
        amba_ik_system_print_str_5("[IK][ERROR] three_d_table is disable. video_mctf.cb_advanced_iso.max_change_method can not be 1\n", DC_S, DC_S, DC_S, DC_S, DC_S);
        rval |= IK_ERR_0008;
    }
    if(p_filters->input_param.video_mctf.cr_advanced_iso.max_change_method == 1U) {
        amba_ik_system_print_str_5("[IK][ERROR] three_d_table is disable. video_mctf.cr_advanced_iso.max_change_method can not be 1\n", DC_S, DC_S, DC_S, DC_S, DC_S);
        rval |= IK_ERR_0008;
    }

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

    if (rval != IK_OK) {
        amba_ik_system_print_str_5("[IK][ERROR] Please check ik_set_fnl_shp_three_d_table() or ik_set_video_mctf() \n", DC_S, DC_S, DC_S, DC_S, DC_S);

    }

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
        rval |= ik_check_mctf_mcts_three_d_table(p_filters);
    }

    return rval;
}

uint32 exe_process_mctf_mcts_still_liso_cr(idsp_flow_ctrl_t *p_flow, void *p_flow_tbl_list, amba_ik_filter_t *p_filters)
{
    uint32 rval = IK_OK;
    amba_ik_flow_tables_list_t *p_flow_tables_list;
    const amba_ik_r2y_flow_tables_list_t *p_r2y = NULL;
    uint32 video_pipe = p_filters->input_param.ability;

    if (video_pipe == AMBA_IK_STILL_PIPE_LISO) {
        (void)amba_ik_system_memcpy(&p_flow_tables_list, &p_flow_tbl_list, sizeof(amba_ik_flow_tables_list_t*));
        p_r2y = &p_flow_tables_list->r2y;
    } else {
        // misraC
    }

    (void)amba_ik_system_memcpy(p_flow, p_flow, sizeof(uint8));//misra fix.

    if (p_r2y != NULL) {
        {
            ikc_out_sec18_mctf_shpb_t out_mctf;
            ikc_in_mctf_pass_through_t in_mctf_pass_through;

            in_mctf_pass_through.is_yuv_422 = 0u;
            in_mctf_pass_through.step       = 0u;
            out_mctf.p_cr_112 = p_r2y->p_CR_buf_112;
            out_mctf.cr_112_size = CR_SIZE_112;
            out_mctf.p_cr_117 = p_r2y->p_CR_buf_117;
            out_mctf.cr_117_size = CR_SIZE_117;
            rval |= ikc_hiso_mctf_pass_through(&in_mctf_pass_through, &out_mctf);
        }
        //Shpb
        {
            ikc_out_final_sharpen_t out_fnl_shp;
            out_fnl_shp.p_cr_117 = p_r2y->p_CR_buf_117;
            out_fnl_shp.cr_117_size = CR_SIZE_117;
            if((p_filters->update_flags.iso.fnlshpns_both_updated == 1U)||
               (p_filters->update_flags.iso.fnlshpns_noise_updated == 1U)||
               (p_filters->update_flags.iso.fnlshpns_fir_updated == 1U)||
               (p_filters->update_flags.iso.window_size_info_updated == 1U)) {
                ikc_in_final_sharpen_t in_fnl_shp;

                if(p_filters->update_flags.cr.r2y.cr_117_update == 0U) {
                    amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_fnlshp !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
                    rval |= IK_ERR_0000;
                } else {
                    in_fnl_shp.is_hhb_enabled = 0U;
                    in_fnl_shp.sharpen_mode = p_filters->input_param.final_sharpen_both.mode;
                    in_fnl_shp.sharpen_both_enable = p_filters->input_param.final_sharpen_both.enable;
                    in_fnl_shp.p_final_sharpen_noise = &p_filters->input_param.final_sharpen_noise;
                    in_fnl_shp.p_final_sharpen_fir = &p_filters->input_param.final_sharpen_fir;
                    in_fnl_shp.p_main = &p_filters->input_param.window_size_info.main_win;
                    rval |= ikc_fnlshp(&in_fnl_shp, &out_fnl_shp);
                }
            }
            if((p_filters->update_flags.iso.fnlshpns_both_updated == 1U)||
               (p_filters->update_flags.iso.video_mctf_updated == 1U)) {
                ikc_in_final_sharpen_both_t in_fnl_shp_both;

                if(p_filters->update_flags.cr.r2y.cr_117_update == 0U) {
                    amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_fnlshp_both !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
                    rval |= IK_ERR_0000;
                } else {
                    in_fnl_shp_both.p_final_sharpen_both = &p_filters->input_param.final_sharpen_both;
                    in_fnl_shp_both.mctf_y_combine_strength = p_filters->input_param.video_mctf.y_combine_strength;
                    rval |= ikc_fnlshp_both(&in_fnl_shp_both, &out_fnl_shp);
                }
            }
            if(p_filters->update_flags.iso.fnlshpns_coring_updated == 1U) {
                ikc_in_final_sharpen_coring_t in_fnl_shp_coring;

                if(p_filters->update_flags.cr.r2y.cr_117_update == 0U) {
                    amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_fnlshp_coring !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
                    rval |= IK_ERR_0000;
                } else {
                    in_fnl_shp_coring.p_final_sharpen_coring = &p_filters->input_param.final_sharpen_coring;
                    rval |= ikc_fnlshp_coring(&in_fnl_shp_coring, &out_fnl_shp);
                }
            }
            if(p_filters->update_flags.iso.fnlshpns_coring_index_scale_updated == 1U) {
                ikc_in_final_sharpen_coring_idx_scale_t in_fnl_shp_coring_idx_scale;

                if(p_filters->update_flags.cr.r2y.cr_117_update == 0U) {
                    amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_fnlshp_coring_idx_scale !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
                    rval |= IK_ERR_0000;
                } else {
                    in_fnl_shp_coring_idx_scale.p_final_sharpen_coring_idx_scale = &p_filters->input_param.final_sharpen_coring_idx_scale;
                    rval |= ikc_fnlshp_coring_idx_scale(&in_fnl_shp_coring_idx_scale, &out_fnl_shp);
                }
            }
            if(p_filters->update_flags.iso.fnlshpns_min_coring_result_updated == 1U) {
                ikc_in_final_sharpen_min_coring_t in_fnl_shp_min_coring;

                if(p_filters->update_flags.cr.r2y.cr_117_update == 0U) {
                    amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_fnlshp_min_coring !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
                    rval |= IK_ERR_0000;
                } else {
                    in_fnl_shp_min_coring.p_final_sharpen_min_coring_result = &p_filters->input_param.final_sharpen_min_coring_result;
                    rval |= ikc_fnlshp_min_coring(&in_fnl_shp_min_coring, &out_fnl_shp);
                }
            }
            if(p_filters->update_flags.iso.fnlshpns_max_coring_result_updated == 1U) {
                ikc_in_final_sharpen_max_coring_t in_fnl_shp_max_coring;

                if(p_filters->update_flags.cr.r2y.cr_117_update == 0U) {
                    amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_fnlshp_max_coring !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
                    rval |= IK_ERR_0000;
                } else {
                    in_fnl_shp_max_coring.p_final_sharpen_max_coring_result = &p_filters->input_param.final_sharpen_max_coring_result;
                    rval |= ikc_fnlshp_max_coring(&in_fnl_shp_max_coring, &out_fnl_shp);
                }
            }
            if(p_filters->update_flags.iso.fnlshpns_scale_coring_updated == 1U) {
                ikc_in_final_sharpen_scale_coring_t in_fnl_shp_scale_coring;

                if(p_filters->update_flags.cr.r2y.cr_117_update == 0U) {
                    amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_fnlshp_scale_coring !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
                    rval |= IK_ERR_0000;
                } else {
                    in_fnl_shp_scale_coring.p_final_sharpen_scale_coring = &p_filters->input_param.final_sharpen_scale_coring;
                    rval |= ikc_fnlshp_scale_coring(&in_fnl_shp_scale_coring, &out_fnl_shp);
                }
            }
            if(p_filters->update_flags.iso.fnlshpns_both_tdt_updated == 1U) {
                ikc_in_final_sharpen_both_3d_t in_fnl_shp_both_3d;
                ikc_out_final_sharpen_both_3d_t out_fnl_shp_both_3d;

                if((p_filters->update_flags.cr.r2y.cr_117_update == 0U)||
                   (p_filters->update_flags.cr.r2y.cr_119_update == 0U)) {
                    amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_fnlshp_both_3d !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
                    rval |= IK_ERR_0000;
                } else {
                    in_fnl_shp_both_3d.p_final_sharpen_both_three_d_table = &p_filters->input_param.final_sharpen_both_three_d_table;
                    out_fnl_shp_both_3d.p_cr_117 = p_r2y->p_CR_buf_117;
                    out_fnl_shp_both_3d.cr_117_size = CR_SIZE_117;
                    out_fnl_shp_both_3d.p_cr_119 = p_r2y->p_CR_buf_119;
                    out_fnl_shp_both_3d.cr_119_size = CR_SIZE_119;
                    rval |= ikc_fnlshp_both_3d(&in_fnl_shp_both_3d, &out_fnl_shp_both_3d);
                }
            }
        }
    }
    return rval;
}

uint32 exe_process_mctf_mcts_cr(idsp_flow_ctrl_t *p_flow, void *p_flow_tbl_list, amba_ik_filter_t *p_filters)
{
    uint32 rval = IK_OK;
    amba_ik_flow_tables_list_t *p_flow_tables_list;
    amba_ik_motion_fusion_flow_tables_list_t *p_motion_fusion_flow_tables_list;
    const amba_ik_r2y_flow_tables_list_t *p_r2y = NULL;
    uint32 video_pipe = p_filters->input_param.ability;

    if ((video_pipe == AMBA_IK_VIDEO_PIPE_LINEAR) ||
        (video_pipe == AMBA_IK_VIDEO_PIPE_LINEAR_CE) ||
        (video_pipe == AMBA_IK_VIDEO_PIPE_HDR_EXPO_2) ||
        (video_pipe == AMBA_IK_VIDEO_PIPE_HDR_EXPO_3) ||
        (video_pipe == AMBA_IK_VIDEO_PIPE_Y2Y) ||
        (video_pipe == AMBA_IK_VIDEO_PIPE_LINEAR_HVH)||
        (video_pipe == AMBA_IK_VIDEO_PIPE_LINEAR_CE_HVH) ||
        (video_pipe == AMBA_IK_VIDEO_PIPE_HDR_EXPO_2_HVH) ||
        (video_pipe == AMBA_IK_VIDEO_PIPE_HDR_EXPO_3_HVH) ||
        (video_pipe == AMBA_IK_VIDEO_PIPE_Y2Y_HVH) ||
        (video_pipe == AMBA_IK_VIDEO_PIPE_LINEAR_HHB) ||
        (video_pipe == AMBA_IK_VIDEO_PIPE_LINEAR_CE_HHB) ||
        (video_pipe == AMBA_IK_VIDEO_PIPE_HDR_EXPO_2_HHB) ||
        (video_pipe == AMBA_IK_VIDEO_PIPE_HDR_EXPO_3_HHB) ||
        (video_pipe == AMBA_IK_VIDEO_PIPE_Y2Y_HHB)) {
        (void)amba_ik_system_memcpy(&p_flow_tables_list, &p_flow_tbl_list, sizeof(amba_ik_flow_tables_list_t*));
        p_r2y = &p_flow_tables_list->r2y;
    } else if ((video_pipe == AMBA_IK_VIDEO_PIPE_LINEAR_MD) ||
               (video_pipe == AMBA_IK_VIDEO_PIPE_LINEAR_CE_MD) ||
               (video_pipe == AMBA_IK_VIDEO_PIPE_HDR_EXPO_2_MD) ||
               (video_pipe == AMBA_IK_VIDEO_PIPE_HDR_EXPO_3_MD) ||
               (video_pipe == AMBA_IK_VIDEO_PIPE_FUSION) ||
               (video_pipe == AMBA_IK_VIDEO_PIPE_Y2Y_MD) ||
               (video_pipe == AMBA_IK_VIDEO_PIPE_LINEAR_MD_HHB) ||
               (video_pipe == AMBA_IK_VIDEO_PIPE_LINEAR_CE_MD_HHB) ||
               (video_pipe == AMBA_IK_VIDEO_PIPE_HDR_EXPO_2_MD_HHB) ||
               (video_pipe == AMBA_IK_VIDEO_PIPE_HDR_EXPO_3_MD_HHB) ||
               (video_pipe == AMBA_IK_VIDEO_PIPE_Y2Y_MD_HHB)) {
        (void)amba_ik_system_memcpy(&p_motion_fusion_flow_tables_list, &p_flow_tbl_list, sizeof(amba_ik_motion_fusion_flow_tables_list_t*));
        p_r2y = &p_motion_fusion_flow_tables_list->r2y;
    } else {
        // misraC
    }

    (void)amba_ik_system_memcpy(p_flow, p_flow, sizeof(uint8));//misra fix.

    rval |= ik_check_mctf_mcts(p_filters);

    if(rval == IK_OK) {
        if (p_r2y != NULL) {
            //mctf
            if((p_filters->update_flags.iso.video_mctf_updated == 1U)||
               (p_filters->update_flags.iso.internal_video_mctf_updated == 1U)||
               (p_filters->update_flags.iso.video_mctf_ta_updated == 1U)||
               (p_filters->update_flags.iso.fnlshpns_both_updated == 1U)||
               (p_filters->update_flags.iso.window_size_info_updated == 1U)) {
                ikc_in_mctf_t in_mctf;
                ikc_out_mctf_t out_mctf;
#if 0 // SH usage
                ikc_in_mctf_compression_t in_mctf_compression;
                ikc_out_mctf_compression_t out_mctf_compression;
#endif
                if((p_filters->update_flags.cr.r2y.cr_111_update == 0U)||
                   (p_filters->update_flags.cr.r2y.cr_112_update == 0U)||
                   (p_filters->update_flags.cr.r2y.cr_113_update == 0U)||
                   (p_filters->update_flags.cr.r2y.cr_116_update == 0U)) {
                    amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_mctf !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
                    rval |= IK_ERR_0000;
                } else {
                    if ((video_pipe == AMBA_IK_VIDEO_PIPE_LINEAR_HHB) ||
                        (video_pipe == AMBA_IK_VIDEO_PIPE_LINEAR_CE_HHB) ||
                        (video_pipe == AMBA_IK_VIDEO_PIPE_HDR_EXPO_2_HHB) ||
                        (video_pipe == AMBA_IK_VIDEO_PIPE_HDR_EXPO_3_HHB) ||
                        (video_pipe == AMBA_IK_VIDEO_PIPE_LINEAR_MD_HHB) ||
                        (video_pipe == AMBA_IK_VIDEO_PIPE_LINEAR_CE_MD_HHB) ||
                        (video_pipe == AMBA_IK_VIDEO_PIPE_HDR_EXPO_2_MD_HHB) ||
                        (video_pipe == AMBA_IK_VIDEO_PIPE_HDR_EXPO_3_MD_HHB) ||
                        (video_pipe == AMBA_IK_VIDEO_PIPE_Y2Y_HHB) ||
                        (video_pipe == AMBA_IK_VIDEO_PIPE_Y2Y_MD_HHB)) {
                        in_mctf.is_hhb_enabled = 1U;
                    } else {
                        in_mctf.is_hhb_enabled = 0U;
                    }
                    {
                        in_mctf.p_video_mctf = &p_filters->input_param.video_mctf;
                        in_mctf.p_internal_video_mctf = &p_filters->input_param.internal_video_mctf;
                        in_mctf.p_video_mctf_ta = &p_filters->input_param.video_mctf_ta;
                        in_mctf.p_final_sharpen_both = &p_filters->input_param.final_sharpen_both;
                        in_mctf.p_main = &p_filters->input_param.window_size_info.main_win;
                        out_mctf.p_cr_111 = p_r2y->p_CR_buf_111;
                        out_mctf.cr_111_size = CR_SIZE_111;
                        out_mctf.p_cr_112 = p_r2y->p_CR_buf_112;
                        out_mctf.cr_112_size = CR_SIZE_112;
                        out_mctf.p_cr_113 = p_r2y->p_CR_buf_113;
                        out_mctf.cr_113_size = CR_SIZE_113;
                        out_mctf.p_cr_116 = p_r2y->p_CR_buf_116;
                        out_mctf.cr_116_size = CR_SIZE_116;
                        out_mctf.p_flow_info = &p_flow->flow_info;
                        out_mctf.p_phase = &p_flow->phase;
                        rval |= ikc_mctf(&in_mctf, &out_mctf);
                    }
#if 0 // SH usage
                    { // overwrite mctf compression to support runtime change
                        in_mctf_compression.p_video_mctf = &p_filters->input_param.video_mctf;
                        out_mctf_compression.p_cr_111 = p_r2y->p_CR_buf_111;
                        out_mctf_compression.cr_111_size = CR_SIZE_111;
                        out_mctf_compression.p_cr_116 = p_r2y->p_CR_buf_116;
                        out_mctf_compression.cr_116_size = CR_SIZE_116;
                        out_mctf_compression.p_flow_info = &p_flow->flow_info;
                        rval |= ikc_mctf_compression(&in_mctf_compression, &out_mctf_compression);
                    }
#endif
                }
            }

            //shpB
            {
                ikc_out_final_sharpen_t out_fnl_shp;
                out_fnl_shp.p_cr_117 = p_r2y->p_CR_buf_117;
                out_fnl_shp.cr_117_size = CR_SIZE_117;
                if((p_filters->update_flags.iso.fnlshpns_both_updated == 1U)||
                   (p_filters->update_flags.iso.fnlshpns_noise_updated == 1U)||
                   (p_filters->update_flags.iso.fnlshpns_fir_updated == 1U)||
                   (p_filters->update_flags.iso.window_size_info_updated == 1U)) {
                    ikc_in_final_sharpen_t in_fnl_shp;

                    if(p_filters->update_flags.cr.r2y.cr_117_update == 0U) {
                        amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_fnlshp !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
                        rval |= IK_ERR_0000;
                    } else {
                        if ((video_pipe == AMBA_IK_VIDEO_PIPE_LINEAR_HHB) ||
                            (video_pipe == AMBA_IK_VIDEO_PIPE_LINEAR_CE_HHB) ||
                            (video_pipe == AMBA_IK_VIDEO_PIPE_HDR_EXPO_2_HHB) ||
                            (video_pipe == AMBA_IK_VIDEO_PIPE_HDR_EXPO_3_HHB) ||
                            (video_pipe == AMBA_IK_VIDEO_PIPE_LINEAR_MD_HHB) ||
                            (video_pipe == AMBA_IK_VIDEO_PIPE_LINEAR_CE_MD_HHB) ||
                            (video_pipe == AMBA_IK_VIDEO_PIPE_HDR_EXPO_2_MD_HHB) ||
                            (video_pipe == AMBA_IK_VIDEO_PIPE_HDR_EXPO_3_MD_HHB) ||
                            (video_pipe == AMBA_IK_VIDEO_PIPE_Y2Y_HHB) ||
                            (video_pipe == AMBA_IK_VIDEO_PIPE_Y2Y_MD_HHB)) {
                            in_fnl_shp.is_hhb_enabled = 1U;
                        } else {
                            in_fnl_shp.is_hhb_enabled = 0U;
                        }
                        in_fnl_shp.sharpen_mode = p_filters->input_param.final_sharpen_both.mode;
                        in_fnl_shp.sharpen_both_enable = p_filters->input_param.final_sharpen_both.enable;
                        in_fnl_shp.p_final_sharpen_noise = &p_filters->input_param.final_sharpen_noise;
                        in_fnl_shp.p_final_sharpen_fir = &p_filters->input_param.final_sharpen_fir;
                        in_fnl_shp.p_main = &p_filters->input_param.window_size_info.main_win;
                        rval |= ikc_fnlshp(&in_fnl_shp, &out_fnl_shp);
                    }
                }
                if((p_filters->update_flags.iso.fnlshpns_both_updated == 1U)||
                   (p_filters->update_flags.iso.video_mctf_updated == 1U)) {
                    ikc_in_final_sharpen_both_t in_fnl_shp_both;

                    if(p_filters->update_flags.cr.r2y.cr_117_update == 0U) {
                        amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_fnlshp_both !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
                        rval |= IK_ERR_0000;
                    } else {
                        in_fnl_shp_both.p_final_sharpen_both = &p_filters->input_param.final_sharpen_both;
                        in_fnl_shp_both.mctf_y_combine_strength = p_filters->input_param.video_mctf.y_combine_strength;
                        rval |= ikc_fnlshp_both(&in_fnl_shp_both, &out_fnl_shp);
                    }
                }
                if(p_filters->update_flags.iso.fnlshpns_coring_updated == 1U) {
                    ikc_in_final_sharpen_coring_t in_fnl_shp_coring;

                    if(p_filters->update_flags.cr.r2y.cr_117_update == 0U) {
                        amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_fnlshp_coring !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
                        rval |= IK_ERR_0000;
                    } else {
                        in_fnl_shp_coring.p_final_sharpen_coring = &p_filters->input_param.final_sharpen_coring;
                        rval |= ikc_fnlshp_coring(&in_fnl_shp_coring, &out_fnl_shp);
                    }
                }
                if(p_filters->update_flags.iso.fnlshpns_coring_index_scale_updated == 1U) {
                    ikc_in_final_sharpen_coring_idx_scale_t in_fnl_shp_coring_idx_scale;

                    if(p_filters->update_flags.cr.r2y.cr_117_update == 0U) {
                        amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_fnlshp_coring_idx_scale !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
                        rval |= IK_ERR_0000;
                    } else {
                        in_fnl_shp_coring_idx_scale.p_final_sharpen_coring_idx_scale = &p_filters->input_param.final_sharpen_coring_idx_scale;
                        rval |= ikc_fnlshp_coring_idx_scale(&in_fnl_shp_coring_idx_scale, &out_fnl_shp);
                    }
                }
                if(p_filters->update_flags.iso.fnlshpns_min_coring_result_updated == 1U) {
                    ikc_in_final_sharpen_min_coring_t in_fnl_shp_min_coring;

                    if(p_filters->update_flags.cr.r2y.cr_117_update == 0U) {
                        amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_fnlshp_min_coring !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
                        rval |= IK_ERR_0000;
                    } else {
                        in_fnl_shp_min_coring.p_final_sharpen_min_coring_result = &p_filters->input_param.final_sharpen_min_coring_result;
                        rval |= ikc_fnlshp_min_coring(&in_fnl_shp_min_coring, &out_fnl_shp);
                    }
                }
                if(p_filters->update_flags.iso.fnlshpns_max_coring_result_updated == 1U) {
                    ikc_in_final_sharpen_max_coring_t in_fnl_shp_max_coring;

                    if(p_filters->update_flags.cr.r2y.cr_117_update == 0U) {
                        amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_fnlshp_max_coring !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
                        rval |= IK_ERR_0000;
                    } else {
                        in_fnl_shp_max_coring.p_final_sharpen_max_coring_result = &p_filters->input_param.final_sharpen_max_coring_result;
                        rval |= ikc_fnlshp_max_coring(&in_fnl_shp_max_coring, &out_fnl_shp);
                    }
                }
                if(p_filters->update_flags.iso.fnlshpns_scale_coring_updated == 1U) {
                    ikc_in_final_sharpen_scale_coring_t in_fnl_shp_scale_coring;

                    if(p_filters->update_flags.cr.r2y.cr_117_update == 0U) {
                        amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_fnlshp_scale_coring !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
                        rval |= IK_ERR_0000;
                    } else {
                        in_fnl_shp_scale_coring.p_final_sharpen_scale_coring = &p_filters->input_param.final_sharpen_scale_coring;
                        rval |= ikc_fnlshp_scale_coring(&in_fnl_shp_scale_coring, &out_fnl_shp);
                    }
                }
                if(p_filters->update_flags.iso.fnlshpns_both_tdt_updated == 1U) {
                    ikc_in_final_sharpen_both_3d_t in_fnl_shp_both_3d;
                    ikc_out_final_sharpen_both_3d_t out_fnl_shp_both_3d;

                    if((p_filters->update_flags.cr.r2y.cr_117_update == 0U)||
                       (p_filters->update_flags.cr.r2y.cr_119_update == 0U)) {
                        amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_fnlshp_both_3d !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
                        rval |= IK_ERR_0000;
                    } else {
                        in_fnl_shp_both_3d.p_final_sharpen_both_three_d_table = &p_filters->input_param.final_sharpen_both_three_d_table;
                        out_fnl_shp_both_3d.p_cr_117 = p_r2y->p_CR_buf_117;
                        out_fnl_shp_both_3d.cr_117_size = CR_SIZE_117;
                        out_fnl_shp_both_3d.p_cr_119 = p_r2y->p_CR_buf_119;
                        out_fnl_shp_both_3d.cr_119_size = CR_SIZE_119;
                        rval |= ikc_fnlshp_both_3d(&in_fnl_shp_both_3d, &out_fnl_shp_both_3d);
                    }
                }
            }

            if(p_filters->update_flags.iso.video_mctf_and_final_sharpen_updated == 1U) {
                ikc_in_mctf_pos_dep33_t in_mctf_pos;
                ikc_out_mctf_pos_dep33_t out_mctf_pos;

                if((p_filters->update_flags.cr.r2y.cr_114_update == 0U)||
                   (p_filters->update_flags.cr.r2y.cr_118_update == 0U)) {
                    amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_mctf_pos_dep33 !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
                    rval |= IK_ERR_0000;
                } else {
                    in_mctf_pos.p_video_mctf_and_final_sharpen = &p_filters->input_param.video_mctf_and_final_sharpen;
                    out_mctf_pos.p_cr_114 = p_r2y->p_CR_buf_114;
                    out_mctf_pos.cr_114_size = CR_SIZE_114;
                    out_mctf_pos.p_cr_118 = p_r2y->p_CR_buf_118;
                    out_mctf_pos.cr_118_size = CR_SIZE_118;
                    rval |= ikc_mctf_pos_dep33(&in_mctf_pos, &out_mctf_pos);
                }
            }
        }
    }
    return rval;
}

uint32 exe_process_optc_r2y_default_cr(idsp_flow_ctrl_t *p_flow, void *p_flow_tbl_list, const amba_ik_filter_t *p_filters)
{
    uint32 rval = IK_OK;
    amba_ik_motion_fusion_flow_tables_list_t *p_motion_fusion_flow_tables_list;
    const amba_ik_r2y_flow_tables_list_t *p_r2y;

    (void)amba_ik_system_memcpy(&p_motion_fusion_flow_tables_list, &p_flow_tbl_list, sizeof(amba_ik_motion_fusion_flow_tables_list_t*));
    p_r2y = &p_motion_fusion_flow_tables_list->r2y;

    (void)amba_ik_system_memcpy(p_flow, p_flow, sizeof(uint8));//misra fix.

    // default value
    if(p_filters->update_flags.iso.is_1st_frame == 1U) {
        ikc_in_r2y_default_tbl_t in_r2y_default_tbl;
        ikc_out_r2y_default_tbl_t out_r2y_default_tbl;
        out_r2y_default_tbl.p_cr_112 = p_r2y->p_CR_buf_112;
        out_r2y_default_tbl.cr_112_size = CR_SIZE_112;
        rval |= ikc_r2y_default_tbl(&in_r2y_default_tbl, &out_r2y_default_tbl);
    }

    // start exe
    // TBD

    return rval;
}

uint32 exe_process_optc_r2y_cr(idsp_flow_ctrl_t *p_flow, void *p_flow_tbl_list, amba_ik_filter_t *p_filters)
{
    uint32 rval = IK_OK;
    amba_ik_motion_fusion_flow_tables_list_t *p_motion_fusion_flow_tables_list;
    const amba_ik_r2y_flow_tables_list_t *p_r2y;

    (void)amba_ik_system_memcpy(&p_motion_fusion_flow_tables_list, &p_flow_tbl_list, sizeof(amba_ik_motion_fusion_flow_tables_list_t*));
    p_r2y = &p_motion_fusion_flow_tables_list->r2y;

    rval |= ik_check_mctf_mcts(p_filters);

    (void)amba_ik_system_memcpy(p_flow, p_flow, sizeof(uint8));//misra fix.

    // default value
    if(rval == IK_OK) {
        if((p_filters->update_flags.iso.video_mctf_updated == 1U)||
           (p_filters->update_flags.iso.internal_video_mctf_updated == 1U)||
           (p_filters->update_flags.iso.video_mctf_ta_updated == 1U)) {
            if(p_filters->update_flags.cr.r2y.cr_112_update == 0U) {
                amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_mctf !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
                rval |= IK_ERR_0000;
            } else {
                ikc_in_optc_r2y_t in_optc_r2y;
                ikc_out_optc_r2y_t out_optc_r2y;
                in_optc_r2y.p_motion_detect = &p_filters->input_param.motion_detect;
                in_optc_r2y.p_video_mctf = &p_filters->input_param.video_mctf;
                out_optc_r2y.p_cr_112 = p_r2y->p_CR_buf_112;
                out_optc_r2y.cr_112_size = CR_SIZE_112;
                out_optc_r2y.p_cr_117 = p_r2y->p_CR_buf_117;
                out_optc_r2y.cr_117_size = CR_SIZE_117;
                rval |= ikc_optc_r2y(&in_optc_r2y, &out_optc_r2y);
            }
        }
    }

    // start exe

    return rval;
}

uint32 exe_process_optc_motion_me1_cr(idsp_flow_ctrl_t *p_flow, void *p_flow_tbl_list, amba_ik_filter_t *p_filters)
{
    uint32 rval = IK_OK;
    amba_ik_motion_fusion_flow_tables_list_t *p_motion_fusion_flow_tables_list;
    const amba_ik_motion_me1_flow_tables_list_t *p_motion_me1;
    uint32 window_calculate_updated = 0U;
    void *p_warp_ver;
    uint32 video_pipe = p_filters->input_param.ability;

    (void)amba_ik_system_memcpy(&p_motion_fusion_flow_tables_list, &p_flow_tbl_list, sizeof(amba_ik_motion_fusion_flow_tables_list_t*));
    p_motion_me1 = &p_motion_fusion_flow_tables_list->motion_me1;
    p_warp_ver = p_motion_fusion_flow_tables_list->p_warp_ver;

    (void)amba_ik_system_memcpy(p_flow, p_flow, sizeof(uint8));//misra fix.

    if ((p_filters->update_flags.iso.window_size_info_updated == 1U) ||
        (p_filters->update_flags.iso.cfa_window_size_info_updated == 1U) ||
        (p_filters->update_flags.iso.dmy_range_updated == 1U) ||
        (p_filters->update_flags.iso.dzoom_info_updated == 1U) ||
        (p_filters->update_flags.iso.vin_active_win_updated == 1U) ||
        (p_filters->update_flags.iso.stitching_info_updated == 1U) ||
        (p_filters->update_flags.iso.overlap_x_updated == 1U) ||
        (p_filters->update_flags.iso.flip_mode_updated == 1U) ||
        (p_filters->update_flags.iso.warp_enable_updated == 1U) ||
        (p_filters->update_flags.iso.warp_enable_2nd_updated == 1U)) {
        window_calculate_updated = 1U;
    }

    // default value
    if(p_filters->update_flags.iso.is_1st_frame == 1U) {
        ikc_in_motion_me1_default_tbl_t in_motion_me1_default_tbl;
        ikc_out_motion_me1_default_tbl_t out_motion_me1_default_tbl;
        out_motion_me1_default_tbl.p_cr_26 = p_motion_me1->p_CR_buf_26;
        out_motion_me1_default_tbl.cr_26_size = CR_SIZE_26;
        out_motion_me1_default_tbl.p_cr_29 = p_motion_me1->p_CR_buf_29;
        out_motion_me1_default_tbl.cr_29_size = CR_SIZE_29;
        out_motion_me1_default_tbl.p_cr_30 = p_motion_me1->p_CR_buf_30;
        out_motion_me1_default_tbl.cr_30_size = CR_SIZE_30;
        out_motion_me1_default_tbl.p_cr_31 = p_motion_me1->p_CR_buf_31;
        out_motion_me1_default_tbl.cr_31_size = CR_SIZE_31;
        out_motion_me1_default_tbl.p_cr_32 = p_motion_me1->p_CR_buf_32;
        out_motion_me1_default_tbl.cr_32_size = CR_SIZE_32;
        out_motion_me1_default_tbl.p_cr_36 = p_motion_me1->p_CR_buf_36;
        out_motion_me1_default_tbl.cr_36_size = CR_SIZE_36;
        out_motion_me1_default_tbl.p_cr_112 = p_motion_me1->p_CR_buf_112;
        out_motion_me1_default_tbl.cr_112_size = CR_SIZE_112;
        out_motion_me1_default_tbl.p_cr_117 = p_motion_me1->p_CR_buf_117;
        out_motion_me1_default_tbl.cr_117_size = CR_SIZE_117;
        rval |= ikc_motion_me1_default_tbl(&in_motion_me1_default_tbl, &out_motion_me1_default_tbl);
    }

    // start exe
    if((p_filters->update_flags.iso.chroma_filter_updated == 1U)||
       (p_filters->update_flags.iso.resample_str_update == 1U)||
       (p_filters->update_flags.iso.window_size_info_updated == 1U)||
       (window_calculate_updated == 1U)) {
        if((p_filters->update_flags.cr.motion_me1.cr_33_update == 0U)||
           (p_filters->update_flags.cr.motion_me1.cr_35_update == 0U)||
           (p_filters->update_flags.cr.motion_me1.cr_37_update == 0U) ||
           (p_filters->update_flags.cr.motion_me1.cr_42_update == 0U)) {
            amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_sec2_motion_me1_resampler !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
            rval |= IK_ERR_0000;
        } else {
            {
                ikc_in_sec2_motion_me1_resampler_t in_sec2_motion_me1_resampler;
                ikc_out_sec2_motion_me1_resampler_t out_sec2_motion_me1_resampler;

                in_sec2_motion_me1_resampler.main_cut_off_freq = p_filters->input_param.resample_str.main_cut_off_freq;
                in_sec2_motion_me1_resampler.p_main = &p_filters->input_param.window_size_info.main_win;
                in_sec2_motion_me1_resampler.chroma_radius = 32U;
                in_sec2_motion_me1_resampler.is_y2y_420input = 0U;//p_filters->input_param.yuv_mode;
                in_sec2_motion_me1_resampler.luma_dma_size = p_filters->input_param.warp_buf_info.luma_dma_size;
                out_sec2_motion_me1_resampler.p_cr_33 = p_motion_me1->p_CR_buf_33;
                out_sec2_motion_me1_resampler.cr_33_size = CR_SIZE_33;
                out_sec2_motion_me1_resampler.p_cr_35 = p_motion_me1->p_CR_buf_35;
                out_sec2_motion_me1_resampler.cr_35_size = CR_SIZE_35;
                out_sec2_motion_me1_resampler.p_cr_37 = p_motion_me1->p_CR_buf_37;
                out_sec2_motion_me1_resampler.cr_37_size = CR_SIZE_37;

                if ((video_pipe == AMBA_IK_VIDEO_PIPE_LINEAR_MD_HHB) ||
                    (video_pipe == AMBA_IK_VIDEO_PIPE_LINEAR_CE_MD_HHB) ||
                    (video_pipe == AMBA_IK_VIDEO_PIPE_HDR_EXPO_2_MD_HHB) ||
                    (video_pipe == AMBA_IK_VIDEO_PIPE_HDR_EXPO_3_MD_HHB) ||
                    (video_pipe == AMBA_IK_VIDEO_PIPE_Y2Y_MD_HHB)) {
                    in_sec2_motion_me1_resampler.is_hhb_enabled = 1u;
                } else {
                    in_sec2_motion_me1_resampler.is_hhb_enabled = 0u;
                }

                rval |= ikc_sec2_motion_me1_resampler(&in_sec2_motion_me1_resampler, &out_sec2_motion_me1_resampler);
            }
            {
                ikc_in_sec3_motion_me1_vertical_resampler_t in_sec3_motion_me1_vertical_resampler;
                ikc_out_sec3_motion_me1_vertical_resampler_t out_sec3_motion_me1_vertical_resampler;

                in_sec3_motion_me1_vertical_resampler.p_act_win_crop = &p_filters->input_param.ctx_buf.result_win.act_win_crop;
                in_sec3_motion_me1_vertical_resampler.p_dmy_win_geo = &p_filters->input_param.ctx_buf.result_win.dmy_win_geo;
                in_sec3_motion_me1_vertical_resampler.p_cfa_win_dim = &p_filters->input_param.ctx_buf.result_win.cfa_win_dim;
                in_sec3_motion_me1_vertical_resampler.p_main = &p_filters->input_param.window_size_info.main_win;
                in_sec3_motion_me1_vertical_resampler.p_dmy_range = &p_filters->input_param.dmy_range;
                in_sec3_motion_me1_vertical_resampler.chroma_radius = p_filters->input_param.chroma_filter.radius;
                in_sec3_motion_me1_vertical_resampler.is_y2y_420input = 0U;//(p_filters->input_param.yuv_mode == AMBA_IK_YUV420) ? 1U : 0U;
                in_sec3_motion_me1_vertical_resampler.flip_mode = p_filters->input_param.flip_mode;
                out_sec3_motion_me1_vertical_resampler.p_cr_42 = p_motion_me1->p_CR_buf_42;
                out_sec3_motion_me1_vertical_resampler.cr_42_size = CR_SIZE_42;
                rval |= ikc_sec3_motion_me1_vertical_resampler(&in_sec3_motion_me1_vertical_resampler, &out_sec3_motion_me1_vertical_resampler);
            }
        }
    }

#if 1
    if((p_filters->update_flags.iso.chroma_filter_updated == 1U)||
       (p_filters->update_flags.iso.warp_enable_updated == 1U)||
       (p_filters->update_flags.iso.calib_warp_info_updated == 1U)||
       (p_filters->update_flags.iso.warp_internal_updated == 1U)||
       (p_filters->update_flags.iso.window_size_info_updated == 1U)||
       (window_calculate_updated == 1U)) {
        if(p_filters->input_param.ctx_buf.warp_internal_mode_flag == 1U) { //internal warp
            //ikc_in_warp_internal_t in_warp_internal;
            //ikc_out_hwarp_t out_hwarp;

            if((p_filters->update_flags.cr.warp == 0U)||
               (p_filters->update_flags.cr.motion_me1.cr_42_update == 0U)||
               (p_filters->update_flags.cr.motion_me1.cr_43_update == 0U)||
               (p_filters->update_flags.cr.motion_me1.cr_44_update == 0U)) {
                amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_warp_internal !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
                rval |= IK_ERR_0000;
            } else {
#if 0
                in_warp_internal.flip_mode = p_filters->input_param.flip_mode;
                in_warp_internal.p_window_size_info = &p_filters->input_param.window_size_info;
                in_warp_internal.p_act_win_crop = &p_filters->input_param.ctx_buf.result_win.act_win_crop;
                in_warp_internal.p_dmy_win_geo = &p_filters->input_param.ctx_buf.result_win.dmy_win_geo;
                in_warp_internal.p_cfa_win_dim = &p_filters->input_param.ctx_buf.result_win.cfa_win_dim;
                in_warp_internal.p_dmy_range = &p_filters->input_param.dmy_range;
                in_warp_internal.chroma_radius = p_filters->input_param.chroma_filter.radius;
                in_warp_internal.p_stitching_info = &p_filters->input_param.stitching_info;
                in_warp_internal.p_warp_internal = &p_filters->input_param.warp_internal;
                in_warp_internal.dram_efficiency = p_filters->input_param.warp_buf_info.dram_efficiency;
                in_warp_internal.luma_wait_lines = p_filters->input_param.warp_buf_info.luma_wait_lines;
                in_warp_internal.luma_dma_size = p_filters->input_param.warp_buf_info.luma_dma_size;
                out_hwarp.p_cr_33 = p_flow_tbl_list->r2y.p_CR_buf_33;
                out_hwarp.cr_33_size = CR_SIZE_33;
                out_hwarp.p_cr_34 = p_flow_tbl_list->r2y.p_CR_buf_34;
                out_hwarp.cr_34_size = CR_SIZE_34;
                out_hwarp.p_cr_35 = p_flow_tbl_list->r2y.p_CR_buf_35;
                out_hwarp.cr_35_size = CR_SIZE_35;
                out_hwarp.p_cr_42 = p_flow_tbl_list->r2y.p_CR_buf_42;
                out_hwarp.cr_42_size = CR_SIZE_42;
                out_hwarp.p_cr_43 = p_flow_tbl_list->r2y.p_CR_buf_43;
                out_hwarp.cr_43_size = CR_SIZE_43;
                out_hwarp.p_cr_44 = p_flow_tbl_list->r2y.p_CR_buf_44;
                out_hwarp.cr_44_size = CR_SIZE_44;
                out_hwarp.p_flow_info = &p_flow->flow_info;
                out_hwarp.p_window = &p_flow->window;
                out_hwarp.p_phase = &p_flow->phase;
                out_hwarp.p_calib = &p_flow->calib;
                out_hwarp.p_stitch = &p_flow->stitch;
                out_hwarp.p_warp_horizontal_table_address = p_flow_tbl_list->r2y.p_warp_hor;
                out_hwarp.h_size = WARP_VIDEO_MAXHORGRID * WARP_VIDEO_MAXVERGRID * 2U * 2U;// multi_slice buffer.
                out_hwarp.p_warp_vertical_table_address = p_flow_tbl_list->r2y.p_warp_ver;
                out_hwarp.v_size = WARP_VIDEO_MAXHORGRID * WARP_VIDEO_MAXVERGRID * 2U * 2U;// multi_slice buffer.
                rval |= ikc_warp_internal(&in_warp_internal, &out_hwarp);
#endif
            }
        } else {
            ikc_in_vwarp_t in_vwarp;
            ikc_out_vwarp_t out_vwarp;

            if((p_filters->update_flags.cr.warp == 0U)||
               (p_filters->update_flags.cr.motion_me1.cr_42_update == 0U)||
               (p_filters->update_flags.cr.motion_me1.cr_43_update == 0U)||
               (p_filters->update_flags.cr.motion_me1.cr_44_update == 0U)) {
                amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_hwarp !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
                rval |= IK_ERR_0000;
            } else {
                {
                    in_vwarp.is_hvh_enabled = 0;
                    in_vwarp.is_hhb_enabled = 0;
                    in_vwarp.is_multi_pass = 2u;
                    in_vwarp.is_group_cmd = 0u;
                    in_vwarp.warp_enable = (uint32)p_filters->input_param.warp_enable;
                    in_vwarp.flip_mode = p_filters->input_param.flip_mode;
                    in_vwarp.p_window_size_info = &p_filters->input_param.window_size_info;
                    in_vwarp.p_act_win_crop = &p_filters->input_param.ctx_buf.result_win.act_win_crop;
                    in_vwarp.p_dmy_win_geo = &p_filters->input_param.ctx_buf.result_win.dmy_win_geo;
                    in_vwarp.p_cfa_win_dim = &p_filters->input_param.ctx_buf.result_win.cfa_win_dim;
                    in_vwarp.p_dmy_range = &p_filters->input_param.dmy_range;
                    in_vwarp.chroma_radius = p_filters->input_param.chroma_filter.radius;
                    in_vwarp.is_y2y_420input = (p_filters->input_param.yuv_mode == AMBA_IK_YUV420) ? 1U : 0U;
                    in_vwarp.p_stitching_info = &p_filters->input_param.stitching_info;
                    in_vwarp.p_calib_warp_info = &p_filters->input_param.calib_warp_info;
                    in_vwarp.dram_efficiency = p_filters->input_param.warp_buf_info.dram_efficiency;
                    in_vwarp.luma_wait_lines = p_filters->input_param.warp_buf_info.luma_wait_lines;
                    in_vwarp.luma_dma_size = p_filters->input_param.warp_buf_info.luma_dma_size;
                    in_vwarp.p_burst_tile = &p_filters->input_param.burst_tile;
                    out_vwarp.p_cr_42 = p_motion_me1->p_CR_buf_42;
                    out_vwarp.cr_42_size = CR_SIZE_42;
                    out_vwarp.p_cr_43 = p_motion_me1->p_CR_buf_43;
                    out_vwarp.cr_43_size = CR_SIZE_43;
                    out_vwarp.p_cr_44 = p_motion_me1->p_CR_buf_44;
                    out_vwarp.cr_44_size = CR_SIZE_44;
                    out_vwarp.p_window = &p_flow->window;
                    out_vwarp.p_calib = &p_flow->calib;
                    out_vwarp.p_warp_vertical_table_address = p_warp_ver;
                    out_vwarp.v_size = WARP_VIDEO_MAXHORGRID * WARP_VIDEO_MAXVERGRID * 2U * 2U;// multi_slice buffer.

                    rval |= ikc_vwarp(&in_vwarp, &out_vwarp);
                }
            }
        }
        p_flow->calib.update_lens_warp = 1U;
    } else {
        p_flow->calib.update_lens_warp = 0U;
    }
#endif
    if((window_calculate_updated == 1U)) {
        if((p_filters->update_flags.cr.motion_me1.cr_30_update == 0U)||
           (p_filters->update_flags.cr.motion_me1.cr_112_update == 0U)||
           (p_filters->update_flags.cr.motion_me1.cr_117_update == 0U)) {
            amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_motion_me1_input_size !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
            rval |= IK_ERR_0000;
        } else {
            ikc_in_motion_me1_input_size_t in_motion_me1_input_size;
            ikc_out_motion_me1_input_size_t out_motion_me1_input_size;

            in_motion_me1_input_size.p_main = &p_filters->input_param.window_size_info.main_win;
            out_motion_me1_input_size.p_cr_30 = p_motion_me1->p_CR_buf_30;
            out_motion_me1_input_size.cr_30_size = CR_SIZE_30;
            out_motion_me1_input_size.p_cr_112 = p_motion_me1->p_CR_buf_112;
            out_motion_me1_input_size.cr_112_size = CR_SIZE_112;
            out_motion_me1_input_size.p_cr_117 = p_motion_me1->p_CR_buf_117;
            out_motion_me1_input_size.cr_117_size = CR_SIZE_117;

            if ((video_pipe == AMBA_IK_VIDEO_PIPE_LINEAR_MD_HHB) ||
                (video_pipe == AMBA_IK_VIDEO_PIPE_LINEAR_CE_MD_HHB) ||
                (video_pipe == AMBA_IK_VIDEO_PIPE_HDR_EXPO_2_MD_HHB) ||
                (video_pipe == AMBA_IK_VIDEO_PIPE_HDR_EXPO_3_MD_HHB) ||
                (video_pipe == AMBA_IK_VIDEO_PIPE_Y2Y_MD_HHB)) {
                in_motion_me1_input_size.is_hhb_enabled = 1u;
            } else {
                in_motion_me1_input_size.is_hhb_enabled = 0u;
            }

            rval |= ikc_motion_me1_input_size(&in_motion_me1_input_size, &out_motion_me1_input_size);
        }
    }
    return rval;
}

uint32 exe_process_optc_motion_a_cr(idsp_flow_ctrl_t *p_flow, void *p_flow_tbl_list, amba_ik_filter_t *p_filters)
{
    uint32 rval = IK_OK;
    amba_ik_motion_fusion_flow_tables_list_t *p_motion_fusion_flow_tables_list;
    const amba_ik_motion_a_flow_tables_list_t *p_motion_a;
    uint32 window_calculate_updated = 0U;
    uint32 video_pipe = p_filters->input_param.ability;

    (void)amba_ik_system_memcpy(&p_motion_fusion_flow_tables_list, &p_flow_tbl_list, sizeof(amba_ik_motion_fusion_flow_tables_list_t*));
    p_motion_a = &p_motion_fusion_flow_tables_list->motion_a;

    (void)amba_ik_system_memcpy(p_flow, p_flow, sizeof(uint8));//misra fix.

    if ((p_filters->update_flags.iso.window_size_info_updated == 1U) ||
        (p_filters->update_flags.iso.cfa_window_size_info_updated == 1U) ||
        (p_filters->update_flags.iso.dmy_range_updated == 1U) ||
        (p_filters->update_flags.iso.dzoom_info_updated == 1U) ||
        (p_filters->update_flags.iso.vin_active_win_updated == 1U) ||
        (p_filters->update_flags.iso.stitching_info_updated == 1U) ||
        (p_filters->update_flags.iso.overlap_x_updated == 1U) ||
        (p_filters->update_flags.iso.flip_mode_updated == 1U) ||
        (p_filters->update_flags.iso.warp_enable_updated == 1U) ||
        (p_filters->update_flags.iso.warp_enable_2nd_updated == 1U)) {
        window_calculate_updated = 1U;
    }

    // default value
    if(p_filters->update_flags.iso.is_1st_frame == 1U) {
        ikc_in_motion_a_default_tbl_t in_motion_a_default_tbl;
        ikc_out_motion_a_default_tbl_t out_motion_a_default_tbl;
        out_motion_a_default_tbl.p_cr_26 = p_motion_a->p_CR_buf_26;
        out_motion_a_default_tbl.cr_26_size = CR_SIZE_26;
        out_motion_a_default_tbl.p_cr_29 = p_motion_a->p_CR_buf_29;
        out_motion_a_default_tbl.cr_29_size = CR_SIZE_29;
        out_motion_a_default_tbl.p_cr_30 = p_motion_a->p_CR_buf_30;
        out_motion_a_default_tbl.cr_30_size = CR_SIZE_30;
        out_motion_a_default_tbl.p_cr_31 = p_motion_a->p_CR_buf_31;
        out_motion_a_default_tbl.cr_31_size = CR_SIZE_31;
        out_motion_a_default_tbl.p_cr_32 = p_motion_a->p_CR_buf_32;
        out_motion_a_default_tbl.cr_32_size = CR_SIZE_32;
        out_motion_a_default_tbl.p_cr_36 = p_motion_a->p_CR_buf_36;
        out_motion_a_default_tbl.cr_36_size = CR_SIZE_36;
        out_motion_a_default_tbl.p_cr_112 = p_motion_a->p_CR_buf_112;
        out_motion_a_default_tbl.cr_112_size = CR_SIZE_112;
        out_motion_a_default_tbl.p_cr_113 = p_motion_a->p_CR_buf_113;
        out_motion_a_default_tbl.cr_113_size = CR_SIZE_113;
        out_motion_a_default_tbl.p_cr_115 = p_motion_a->p_CR_buf_115;
        out_motion_a_default_tbl.cr_115_size = CR_SIZE_115;
        out_motion_a_default_tbl.p_cr_117 = p_motion_a->p_CR_buf_117;
        out_motion_a_default_tbl.cr_117_size = CR_SIZE_117;
        out_motion_a_default_tbl.p_cr_119 = p_motion_a->p_CR_buf_119;
        out_motion_a_default_tbl.cr_119_size = CR_SIZE_119;
        rval |= ikc_motion_a_default_tbl(&in_motion_a_default_tbl, &out_motion_a_default_tbl);
    }

    // start exe
    if(p_filters->update_flags.iso.motion_detect_updated == 1U) {
        ikc_in_motion_detect_a_t in_motion_detect_a;
        ikc_out_motion_detect_a_t out_motion_detect_a;

        if(p_filters->update_flags.cr.motion_a.cr_30_update == 0U) {
            amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_motion_detect_a !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
            rval |= IK_ERR_0000;
        } else {
            in_motion_detect_a.p_motion_detect = &p_filters->input_param.motion_detect;
            out_motion_detect_a.p_cr_30 = p_motion_a->p_CR_buf_30;
            out_motion_detect_a.cr_30_size = CR_SIZE_30;
            rval |= ikc_motion_detect_a(&in_motion_detect_a, &out_motion_detect_a);
        }
    }

    if((p_filters->update_flags.iso.motion_detect_updated == 1U) || (p_filters->update_flags.iso.motion_detect_pos_dep_updated == 1U)) {
        ikc_in_motion_detect_a_mctf_t in_motion_detect_a_mctf;
        ikc_out_motion_detect_a_mctf_t out_motion_detect_a_mctf;

        if((p_filters->update_flags.cr.motion_a.cr_112_update == 0U) ||
           (p_filters->update_flags.cr.motion_a.cr_114_update == 0U)) {
            amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_motion_detect_a_mctf !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
            rval |= IK_ERR_0000;
        } else {
            in_motion_detect_a_mctf.p_motion_detect = &p_filters->input_param.motion_detect;
            in_motion_detect_a_mctf.p_pos_dep33 = &p_filters->input_param.motion_detect_pos_dep;
            in_motion_detect_a_mctf.p_motion_detect_and_mctf = &p_filters->input_param.motion_detect_and_mctf;
            in_motion_detect_a_mctf.p_main = &p_filters->input_param.window_size_info.main_win;
            out_motion_detect_a_mctf.p_cr_112 = p_motion_a->p_CR_buf_112;
            out_motion_detect_a_mctf.cr_112_size = CR_SIZE_112;
            out_motion_detect_a_mctf.p_cr_114 = p_motion_a->p_CR_buf_114;
            out_motion_detect_a_mctf.cr_114_size = CR_SIZE_114;
            out_motion_detect_a_mctf.p_step_info = &p_flow->step_info;
            rval |= ikc_motion_detect_a_mctf(&in_motion_detect_a_mctf, &out_motion_detect_a_mctf);
        }
        p_flow->step_info.multi_pass.fusion.opt_c.update_local_lighting = 1u;
        p_flow->step_info.multi_pass.fusion.opt_c.update_slow_mo_sensitivity = 1u;
    } else {
        p_flow->step_info.multi_pass.fusion.opt_c.update_local_lighting = 0;
        p_flow->step_info.multi_pass.fusion.opt_c.update_slow_mo_sensitivity = 0;
    }

    if((p_filters->update_flags.iso.chroma_filter_updated == 1U)||
       (p_filters->update_flags.iso.resample_str_update == 1U)||
       (p_filters->update_flags.iso.window_size_info_updated == 1U)||
       (window_calculate_updated == 1U)) {
        if((p_filters->update_flags.cr.motion_a.cr_33_update == 0U)||
           (p_filters->update_flags.cr.motion_a.cr_35_update == 0U)||
           (p_filters->update_flags.cr.motion_a.cr_37_update == 0U)) {
            amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_sec2_motion_abc_resampler !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
            rval |= IK_ERR_0000;
        } else {
            {
                ikc_in_sec2_motion_abc_resampler_t in_sec2_motion_abc_resampler;
                ikc_out_sec2_motion_abc_resampler_t out_sec2_motion_abc_resampler;

                in_sec2_motion_abc_resampler.main_cut_off_freq = p_filters->input_param.resample_str.main_cut_off_freq;
                in_sec2_motion_abc_resampler.p_main = &p_filters->input_param.window_size_info.main_win;
                in_sec2_motion_abc_resampler.chroma_radius = 32U;
                in_sec2_motion_abc_resampler.is_y2y_420input = 0U;//p_filters->input_param.yuv_mode;
                in_sec2_motion_abc_resampler.luma_dma_size = p_filters->input_param.warp_buf_info.luma_dma_size;
                out_sec2_motion_abc_resampler.p_cr_33 = p_motion_a->p_CR_buf_33;
                out_sec2_motion_abc_resampler.cr_33_size = CR_SIZE_33;
                out_sec2_motion_abc_resampler.p_cr_35 = p_motion_a->p_CR_buf_35;
                out_sec2_motion_abc_resampler.cr_35_size = CR_SIZE_35;
                out_sec2_motion_abc_resampler.p_cr_37 = p_motion_a->p_CR_buf_37;
                out_sec2_motion_abc_resampler.cr_37_size = CR_SIZE_37;

                if ((video_pipe == AMBA_IK_VIDEO_PIPE_LINEAR_MD_HHB) ||
                    (video_pipe == AMBA_IK_VIDEO_PIPE_LINEAR_CE_MD_HHB) ||
                    (video_pipe == AMBA_IK_VIDEO_PIPE_HDR_EXPO_2_MD_HHB) ||
                    (video_pipe == AMBA_IK_VIDEO_PIPE_HDR_EXPO_3_MD_HHB) ||
                    (video_pipe == AMBA_IK_VIDEO_PIPE_Y2Y_MD_HHB)) {
                    in_sec2_motion_abc_resampler.is_hhb_enabled = 1u;
                } else {
                    in_sec2_motion_abc_resampler.is_hhb_enabled = 0u;
                }

                rval |= ikc_sec2_motion_abc_resampler(&in_sec2_motion_abc_resampler, &out_sec2_motion_abc_resampler);
            }
        }
    }

    if((window_calculate_updated == 1U)) {
        if((p_filters->update_flags.cr.motion_a.cr_30_update == 0U)||
           (p_filters->update_flags.cr.motion_a.cr_112_update == 0U)||
           (p_filters->update_flags.cr.motion_a.cr_117_update == 0U)) {
            amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_motion_a_input_size !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
            rval |= IK_ERR_0000;
        } else {
            ikc_in_motion_a_input_size_t in_motion_a_input_size;
            ikc_out_motion_a_input_size_t out_motion_a_input_size;

            in_motion_a_input_size.p_main = &p_filters->input_param.window_size_info.main_win;
            in_motion_a_input_size.motion_detect_smooth = 0; // new motion detect hacked
            out_motion_a_input_size.p_cr_30 = p_motion_a->p_CR_buf_30;
            out_motion_a_input_size.cr_30_size = CR_SIZE_30;
            out_motion_a_input_size.p_cr_112 = p_motion_a->p_CR_buf_112;
            out_motion_a_input_size.cr_112_size = CR_SIZE_112;
            out_motion_a_input_size.p_cr_117 = p_motion_a->p_CR_buf_117;
            out_motion_a_input_size.cr_117_size = CR_SIZE_117;

            if ((video_pipe == AMBA_IK_VIDEO_PIPE_LINEAR_MD_HHB) ||
                (video_pipe == AMBA_IK_VIDEO_PIPE_LINEAR_CE_MD_HHB) ||
                (video_pipe == AMBA_IK_VIDEO_PIPE_HDR_EXPO_2_MD_HHB) ||
                (video_pipe == AMBA_IK_VIDEO_PIPE_HDR_EXPO_3_MD_HHB) ||
                (video_pipe == AMBA_IK_VIDEO_PIPE_Y2Y_MD_HHB)) {
                in_motion_a_input_size.is_hhb_enabled = 1u;
            } else {
                in_motion_a_input_size.is_hhb_enabled = 0u;
            }

            rval |= ikc_motion_a_input_size(&in_motion_a_input_size, &out_motion_a_input_size);
        }
    }

    return rval;
}

uint32 exe_process_optc_motion_b_cr(idsp_flow_ctrl_t *p_flow, void *p_flow_tbl_list, amba_ik_filter_t *p_filters)
{
    uint32 rval = IK_OK;
    amba_ik_motion_fusion_flow_tables_list_t *p_motion_fusion_flow_tables_list;
    const amba_ik_motion_b_flow_tables_list_t *p_motion_b;
    uint32 window_calculate_updated = 0U;
    uint32 video_pipe = p_filters->input_param.ability;

    (void)amba_ik_system_memcpy(&p_motion_fusion_flow_tables_list, &p_flow_tbl_list, sizeof(amba_ik_motion_fusion_flow_tables_list_t*));
    p_motion_b = &p_motion_fusion_flow_tables_list->motion_b;

    (void)amba_ik_system_memcpy(p_flow, p_flow, sizeof(uint8));//misra fix.

    if ((p_filters->update_flags.iso.window_size_info_updated == 1U) ||
        (p_filters->update_flags.iso.cfa_window_size_info_updated == 1U) ||
        (p_filters->update_flags.iso.dmy_range_updated == 1U) ||
        (p_filters->update_flags.iso.dzoom_info_updated == 1U) ||
        (p_filters->update_flags.iso.vin_active_win_updated == 1U) ||
        (p_filters->update_flags.iso.stitching_info_updated == 1U) ||
        (p_filters->update_flags.iso.overlap_x_updated == 1U) ||
        (p_filters->update_flags.iso.flip_mode_updated == 1U) ||
        (p_filters->update_flags.iso.warp_enable_updated == 1U) ||
        (p_filters->update_flags.iso.warp_enable_2nd_updated == 1U)) {
        window_calculate_updated = 1U;
    }

    // default value
    if(p_filters->update_flags.iso.is_1st_frame == 1U) {
        ikc_in_motion_b_default_tbl_t in_motion_b_default_tbl;
        ikc_out_motion_b_default_tbl_t out_motion_b_default_tbl;
        out_motion_b_default_tbl.p_cr_26 = p_motion_b->p_CR_buf_26;
        out_motion_b_default_tbl.cr_26_size = CR_SIZE_26;
        out_motion_b_default_tbl.p_cr_29 = p_motion_b->p_CR_buf_29;
        out_motion_b_default_tbl.cr_29_size = CR_SIZE_29;
        out_motion_b_default_tbl.p_cr_30 = p_motion_b->p_CR_buf_30;
        out_motion_b_default_tbl.cr_30_size = CR_SIZE_30;
        out_motion_b_default_tbl.p_cr_31 = p_motion_b->p_CR_buf_31;
        out_motion_b_default_tbl.cr_31_size = CR_SIZE_31;
        out_motion_b_default_tbl.p_cr_32 = p_motion_b->p_CR_buf_32;
        out_motion_b_default_tbl.cr_32_size = CR_SIZE_32;
        out_motion_b_default_tbl.p_cr_36 = p_motion_b->p_CR_buf_36;
        out_motion_b_default_tbl.cr_36_size = CR_SIZE_36;
        out_motion_b_default_tbl.p_cr_112 = p_motion_b->p_CR_buf_112;
        out_motion_b_default_tbl.cr_112_size = CR_SIZE_112;
        out_motion_b_default_tbl.p_cr_113 = p_motion_b->p_CR_buf_113;
        out_motion_b_default_tbl.cr_113_size = CR_SIZE_113;
        out_motion_b_default_tbl.p_cr_115 = p_motion_b->p_CR_buf_115;
        out_motion_b_default_tbl.cr_115_size = CR_SIZE_115;
        out_motion_b_default_tbl.p_cr_117 = p_motion_b->p_CR_buf_117;
        out_motion_b_default_tbl.cr_117_size = CR_SIZE_117;
        out_motion_b_default_tbl.p_cr_119 = p_motion_b->p_CR_buf_119;
        out_motion_b_default_tbl.cr_119_size = CR_SIZE_119;
        rval |= ikc_motion_b_default_tbl(&in_motion_b_default_tbl, &out_motion_b_default_tbl);
    }

    // start exe
    if(p_filters->update_flags.iso.motion_detect_updated == 1U) {
        ikc_in_motion_detect_b_t in_motion_detect_b;
        ikc_out_motion_detect_b_t out_motion_detect_b;

        if(p_filters->update_flags.cr.motion_b.cr_30_update == 0U) {
            amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_motion_detect_b !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
            rval |= IK_ERR_0000;
        } else {
            in_motion_detect_b.p_motion_detect = &p_filters->input_param.motion_detect;
            out_motion_detect_b.p_cr_30 = p_motion_b->p_CR_buf_30;
            out_motion_detect_b.cr_30_size = CR_SIZE_30;
            rval |= ikc_motion_detect_b(&in_motion_detect_b, &out_motion_detect_b);
        }
    }

    if((p_filters->update_flags.iso.motion_detect_updated == 1U) || (p_filters->update_flags.iso.motion_detect_pos_dep_updated == 1U)) {
        ikc_in_motion_detect_b_mctf_t in_motion_detect_b_mctf;
        ikc_out_motion_detect_b_mctf_t out_motion_detect_b_mctf;

        if((p_filters->update_flags.cr.motion_b.cr_112_update == 0U) ||
           (p_filters->update_flags.cr.motion_b.cr_114_update == 0U)) {
            amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_motion_detect_b_mctf !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
            rval |= IK_ERR_0000;
        } else {
            in_motion_detect_b_mctf.p_motion_detect = &p_filters->input_param.motion_detect;
            in_motion_detect_b_mctf.p_pos_dep33 = &p_filters->input_param.motion_detect_pos_dep;
            in_motion_detect_b_mctf.p_motion_detect_and_mctf = &p_filters->input_param.motion_detect_and_mctf;
            in_motion_detect_b_mctf.p_main = &p_filters->input_param.window_size_info.main_win;
            out_motion_detect_b_mctf.p_cr_112 = p_motion_b->p_CR_buf_112;
            out_motion_detect_b_mctf.cr_112_size = CR_SIZE_112;
            out_motion_detect_b_mctf.p_cr_114 = p_motion_b->p_CR_buf_114;
            out_motion_detect_b_mctf.cr_114_size = CR_SIZE_114;
            rval |= ikc_motion_detect_b_mctf(&in_motion_detect_b_mctf, &out_motion_detect_b_mctf);
        }
    }

    if((p_filters->update_flags.iso.chroma_filter_updated == 1U)||
       (p_filters->update_flags.iso.resample_str_update == 1U)||
       (p_filters->update_flags.iso.window_size_info_updated == 1U)||
       (window_calculate_updated == 1U)) {
        if((p_filters->update_flags.cr.motion_b.cr_33_update == 0U)||
           (p_filters->update_flags.cr.motion_b.cr_35_update == 0U)||
           (p_filters->update_flags.cr.motion_b.cr_37_update == 0U)) {
            amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_sec2_main_resampler !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
            rval |= IK_ERR_0000;
        } else {
            {
                ikc_in_sec2_motion_abc_resampler_t in_sec2_motion_abc_resampler;
                ikc_out_sec2_motion_abc_resampler_t out_sec2_motion_abc_resampler;

                in_sec2_motion_abc_resampler.main_cut_off_freq = p_filters->input_param.resample_str.main_cut_off_freq;
                in_sec2_motion_abc_resampler.p_main = &p_filters->input_param.window_size_info.main_win;
                in_sec2_motion_abc_resampler.chroma_radius = 32U;
                in_sec2_motion_abc_resampler.is_y2y_420input = 0U;//p_filters->input_param.yuv_mode;
                in_sec2_motion_abc_resampler.luma_dma_size = p_filters->input_param.warp_buf_info.luma_dma_size;
                out_sec2_motion_abc_resampler.p_cr_33 = p_motion_b->p_CR_buf_33;
                out_sec2_motion_abc_resampler.cr_33_size = CR_SIZE_33;
                out_sec2_motion_abc_resampler.p_cr_35 = p_motion_b->p_CR_buf_35;
                out_sec2_motion_abc_resampler.cr_35_size = CR_SIZE_35;
                out_sec2_motion_abc_resampler.p_cr_37 = p_motion_b->p_CR_buf_37;
                out_sec2_motion_abc_resampler.cr_37_size = CR_SIZE_37;

                if ((video_pipe == AMBA_IK_VIDEO_PIPE_LINEAR_MD_HHB) ||
                    (video_pipe == AMBA_IK_VIDEO_PIPE_LINEAR_CE_MD_HHB) ||
                    (video_pipe == AMBA_IK_VIDEO_PIPE_HDR_EXPO_2_MD_HHB) ||
                    (video_pipe == AMBA_IK_VIDEO_PIPE_HDR_EXPO_3_MD_HHB) ||
                    (video_pipe == AMBA_IK_VIDEO_PIPE_Y2Y_MD_HHB)) {
                    in_sec2_motion_abc_resampler.is_hhb_enabled = 1u;
                } else {
                    in_sec2_motion_abc_resampler.is_hhb_enabled = 0u;
                }

                rval |= ikc_sec2_motion_abc_resampler(&in_sec2_motion_abc_resampler, &out_sec2_motion_abc_resampler);
            }
        }
    }

    if((window_calculate_updated == 1U)) {
        if((p_filters->update_flags.cr.motion_b.cr_30_update == 0U)||
           (p_filters->update_flags.cr.motion_b.cr_112_update == 0U)||
           (p_filters->update_flags.cr.motion_b.cr_117_update == 0U)) {
            amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_motion_b_input_size !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
            rval |= IK_ERR_0000;
        } else {
            ikc_in_motion_b_input_size_t in_motion_b_input_size;
            ikc_out_motion_b_input_size_t out_motion_b_input_size;

            in_motion_b_input_size.p_main = &p_filters->input_param.window_size_info.main_win;
            in_motion_b_input_size.motion_detect_smooth = 0; // new motion detect hacked
            out_motion_b_input_size.p_cr_30 = p_motion_b->p_CR_buf_30;
            out_motion_b_input_size.cr_30_size = CR_SIZE_30;
            out_motion_b_input_size.p_cr_112 = p_motion_b->p_CR_buf_112;
            out_motion_b_input_size.cr_112_size = CR_SIZE_112;
            out_motion_b_input_size.p_cr_117 = p_motion_b->p_CR_buf_117;
            out_motion_b_input_size.cr_117_size = CR_SIZE_117;

            if ((video_pipe == AMBA_IK_VIDEO_PIPE_LINEAR_MD_HHB) ||
                (video_pipe == AMBA_IK_VIDEO_PIPE_LINEAR_CE_MD_HHB) ||
                (video_pipe == AMBA_IK_VIDEO_PIPE_HDR_EXPO_2_MD_HHB) ||
                (video_pipe == AMBA_IK_VIDEO_PIPE_HDR_EXPO_3_MD_HHB) ||
                (video_pipe == AMBA_IK_VIDEO_PIPE_Y2Y_MD_HHB)) {
                in_motion_b_input_size.is_hhb_enabled = 1u;
            } else {
                in_motion_b_input_size.is_hhb_enabled = 0u;
            }

            rval |= ikc_motion_b_input_size(&in_motion_b_input_size, &out_motion_b_input_size);
        }
    }

    return rval;
}

uint32 exe_process_optc_motion_c_cr(idsp_flow_ctrl_t *p_flow, void *p_flow_tbl_list, amba_ik_filter_t *p_filters)
{
    uint32 rval = IK_OK;
    amba_ik_motion_fusion_flow_tables_list_t *p_motion_fusion_flow_tables_list;
    const amba_ik_motion_c_flow_tables_list_t *p_motion_c;
    uint32 window_calculate_updated = 0U;
    uint32 video_pipe = p_filters->input_param.ability;

    (void)amba_ik_system_memcpy(&p_motion_fusion_flow_tables_list, &p_flow_tbl_list, sizeof(amba_ik_motion_fusion_flow_tables_list_t*));
    p_motion_c = &p_motion_fusion_flow_tables_list->motion_c;

    (void)amba_ik_system_memcpy(p_flow, p_flow, sizeof(uint8));//misra fix.

    if ((p_filters->update_flags.iso.window_size_info_updated == 1U) ||
        (p_filters->update_flags.iso.cfa_window_size_info_updated == 1U) ||
        (p_filters->update_flags.iso.dmy_range_updated == 1U) ||
        (p_filters->update_flags.iso.dzoom_info_updated == 1U) ||
        (p_filters->update_flags.iso.vin_active_win_updated == 1U) ||
        (p_filters->update_flags.iso.stitching_info_updated == 1U) ||
        (p_filters->update_flags.iso.overlap_x_updated == 1U) ||
        (p_filters->update_flags.iso.flip_mode_updated == 1U) ||
        (p_filters->update_flags.iso.warp_enable_updated == 1U) ||
        (p_filters->update_flags.iso.warp_enable_2nd_updated == 1U)) {
        window_calculate_updated = 1U;
    }

    // default value
    if(p_filters->update_flags.iso.is_1st_frame == 1U) {
        ikc_in_motion_c_default_tbl_t in_motion_c_default_tbl;
        ikc_out_motion_c_default_tbl_t out_motion_c_default_tbl;
        out_motion_c_default_tbl.p_cr_26 = p_motion_c->p_CR_buf_26;
        out_motion_c_default_tbl.cr_26_size = CR_SIZE_26;
        out_motion_c_default_tbl.p_cr_29 = p_motion_c->p_CR_buf_29;
        out_motion_c_default_tbl.cr_29_size = CR_SIZE_29;
        out_motion_c_default_tbl.p_cr_30 = p_motion_c->p_CR_buf_30;
        out_motion_c_default_tbl.cr_30_size = CR_SIZE_30;
        out_motion_c_default_tbl.p_cr_31 = p_motion_c->p_CR_buf_31;
        out_motion_c_default_tbl.cr_31_size = CR_SIZE_31;
        out_motion_c_default_tbl.p_cr_32 = p_motion_c->p_CR_buf_32;
        out_motion_c_default_tbl.cr_32_size = CR_SIZE_32;
        out_motion_c_default_tbl.p_cr_36 = p_motion_c->p_CR_buf_36;
        out_motion_c_default_tbl.cr_36_size = CR_SIZE_36;
        out_motion_c_default_tbl.p_cr_112 = p_motion_c->p_CR_buf_112;
        out_motion_c_default_tbl.cr_112_size = CR_SIZE_112;
        out_motion_c_default_tbl.p_cr_113 = p_motion_c->p_CR_buf_113;
        out_motion_c_default_tbl.cr_113_size = CR_SIZE_113;
        out_motion_c_default_tbl.p_cr_115 = p_motion_c->p_CR_buf_115;
        out_motion_c_default_tbl.cr_115_size = CR_SIZE_115;
        out_motion_c_default_tbl.p_cr_117 = p_motion_c->p_CR_buf_117;
        out_motion_c_default_tbl.cr_117_size = CR_SIZE_117;
        out_motion_c_default_tbl.p_cr_119 = p_motion_c->p_CR_buf_119;
        out_motion_c_default_tbl.cr_119_size = CR_SIZE_119;
        rval |= ikc_motion_c_default_tbl(&in_motion_c_default_tbl, &out_motion_c_default_tbl);
    }

    // start exe
    if((p_filters->update_flags.iso.motion_detect_updated== 1U) ||
       (p_filters->update_flags.iso.motion_detect_and_mctf_updated== 1U) ||
       (p_filters->update_flags.iso.motion_detect_pos_dep_updated== 1U)) {
        ikc_in_motion_detect_c_t in_motion_detect_c;
        ikc_out_motion_detect_c_t out_motion_detect_c;

        if(p_filters->update_flags.cr.motion_c.cr_30_update == 0U) {
            amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_motion_detect_c !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
            rval |= IK_ERR_0000;
        } else {
            in_motion_detect_c.p_motion_detect = &p_filters->input_param.motion_detect;
            out_motion_detect_c.p_cr_30 = p_motion_c->p_CR_buf_30;
            out_motion_detect_c.cr_30_size = CR_SIZE_30;
            rval |= ikc_motion_detect_c(&in_motion_detect_c, &out_motion_detect_c);
        }
    }

    if((p_filters->update_flags.iso.motion_detect_updated == 1U) || (p_filters->update_flags.iso.motion_detect_pos_dep_updated == 1U)) {
        ikc_in_motion_detect_c_mctf_t in_motion_detect_c_mctf;
        ikc_out_motion_detect_c_mctf_t out_motion_detect_c_mctf;

        if((p_filters->update_flags.cr.motion_c.cr_112_update == 0U) ||
           (p_filters->update_flags.cr.motion_c.cr_114_update == 0U)) {
            amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_motion_detect_c_mctf !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
            rval |= IK_ERR_0000;
        } else {
            in_motion_detect_c_mctf.p_motion_detect = &p_filters->input_param.motion_detect;
            in_motion_detect_c_mctf.p_pos_dep33 = &p_filters->input_param.motion_detect_pos_dep;
            in_motion_detect_c_mctf.p_motion_detect_and_mctf = &p_filters->input_param.motion_detect_and_mctf;
            in_motion_detect_c_mctf.p_main = &p_filters->input_param.window_size_info.main_win;
            out_motion_detect_c_mctf.p_cr_112 = p_motion_c->p_CR_buf_112;
            out_motion_detect_c_mctf.cr_112_size = CR_SIZE_112;
            out_motion_detect_c_mctf.p_cr_114 = p_motion_c->p_CR_buf_114;
            out_motion_detect_c_mctf.cr_114_size = CR_SIZE_114;
            rval |= ikc_motion_detect_c_mctf(&in_motion_detect_c_mctf, &out_motion_detect_c_mctf);
        }
    }

    if((p_filters->update_flags.iso.chroma_filter_updated == 1U)||
       (p_filters->update_flags.iso.resample_str_update == 1U)||
       (p_filters->update_flags.iso.window_size_info_updated == 1U)||
       (window_calculate_updated == 1U)) {
        if((p_filters->update_flags.cr.motion_c.cr_33_update == 0U)||
           (p_filters->update_flags.cr.motion_c.cr_35_update == 0U)||
           (p_filters->update_flags.cr.motion_c.cr_37_update == 0U)) {
            amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_sec2_main_resampler !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
            rval |= IK_ERR_0000;
        } else {
            {
                ikc_in_sec2_motion_abc_resampler_t in_sec2_motion_abc_resampler;
                ikc_out_sec2_motion_abc_resampler_t out_sec2_motion_abc_resampler;

                in_sec2_motion_abc_resampler.main_cut_off_freq = p_filters->input_param.resample_str.main_cut_off_freq;
                in_sec2_motion_abc_resampler.p_main = &p_filters->input_param.window_size_info.main_win;
                in_sec2_motion_abc_resampler.chroma_radius = 32U;
                in_sec2_motion_abc_resampler.is_y2y_420input = 0U;//p_filters->input_param.yuv_mode;
                in_sec2_motion_abc_resampler.luma_dma_size = p_filters->input_param.warp_buf_info.luma_dma_size;
                out_sec2_motion_abc_resampler.p_cr_33 = p_motion_c->p_CR_buf_33;
                out_sec2_motion_abc_resampler.cr_33_size = CR_SIZE_33;
                out_sec2_motion_abc_resampler.p_cr_35 = p_motion_c->p_CR_buf_35;
                out_sec2_motion_abc_resampler.cr_35_size = CR_SIZE_35;
                out_sec2_motion_abc_resampler.p_cr_37 = p_motion_c->p_CR_buf_37;
                out_sec2_motion_abc_resampler.cr_37_size = CR_SIZE_37;

                if ((video_pipe == AMBA_IK_VIDEO_PIPE_LINEAR_MD_HHB) ||
                    (video_pipe == AMBA_IK_VIDEO_PIPE_LINEAR_CE_MD_HHB) ||
                    (video_pipe == AMBA_IK_VIDEO_PIPE_HDR_EXPO_2_MD_HHB) ||
                    (video_pipe == AMBA_IK_VIDEO_PIPE_HDR_EXPO_3_MD_HHB) ||
                    (video_pipe == AMBA_IK_VIDEO_PIPE_Y2Y_MD_HHB)) {
                    in_sec2_motion_abc_resampler.is_hhb_enabled = 1u;
                } else {
                    in_sec2_motion_abc_resampler.is_hhb_enabled = 0u;
                }

                rval |= ikc_sec2_motion_abc_resampler(&in_sec2_motion_abc_resampler, &out_sec2_motion_abc_resampler);
            }
        }
    }

    if((window_calculate_updated == 1U)) {
        if((p_filters->update_flags.cr.motion_c.cr_30_update == 0U)||
           (p_filters->update_flags.cr.motion_c.cr_112_update == 0U)||
           (p_filters->update_flags.cr.motion_c.cr_117_update == 0U)) {
            amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_motion_c_input_size !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
            rval |= IK_ERR_0000;
        } else {
            ikc_in_motion_c_input_size_t in_motion_c_input_size;
            ikc_out_motion_c_input_size_t out_motion_c_input_size;

            in_motion_c_input_size.p_main = &p_filters->input_param.window_size_info.main_win;
            in_motion_c_input_size.motion_detect_smooth = 0; // new motion detect hacked
            out_motion_c_input_size.p_cr_30 = p_motion_c->p_CR_buf_30;
            out_motion_c_input_size.cr_30_size = CR_SIZE_30;
            out_motion_c_input_size.p_cr_112 = p_motion_c->p_CR_buf_112;
            out_motion_c_input_size.cr_112_size = CR_SIZE_112;
            out_motion_c_input_size.p_cr_117 = p_motion_c->p_CR_buf_117;
            out_motion_c_input_size.cr_117_size = CR_SIZE_117;

            if ((video_pipe == AMBA_IK_VIDEO_PIPE_LINEAR_MD_HHB) ||
                (video_pipe == AMBA_IK_VIDEO_PIPE_LINEAR_CE_MD_HHB) ||
                (video_pipe == AMBA_IK_VIDEO_PIPE_HDR_EXPO_2_MD_HHB) ||
                (video_pipe == AMBA_IK_VIDEO_PIPE_HDR_EXPO_3_MD_HHB) ||
                (video_pipe == AMBA_IK_VIDEO_PIPE_Y2Y_MD_HHB)) {
                in_motion_c_input_size.is_hhb_enabled = 1u;
            } else {
                in_motion_c_input_size.is_hhb_enabled = 0u;
            }

            rval |= ikc_motion_c_input_size(&in_motion_c_input_size, &out_motion_c_input_size);
        }
    }

    return rval;
}
#if SUPPORT_FUSION
uint32 exe_process_mono1_8_cr(idsp_flow_ctrl_t *p_flow, void *p_flow_tbl_list, amba_ik_filter_t *p_filters, uint32 pass)
{
    uint32 rval = IK_OK;
    amba_ik_motion_fusion_flow_tables_list_t *p_motion_fusion_flow_tables_list;
    const amba_ik_r2y_flow_tables_list_t *p_r2y;
    const amba_ik_r2y_cr_id_list_t *p_r2y_cr_id;
    uint32 window_calculate_updated = 0U;
    void *p_extra_window = NULL;

    (void)amba_ik_system_memcpy(&p_motion_fusion_flow_tables_list, &p_flow_tbl_list, sizeof(amba_ik_motion_fusion_flow_tables_list_t*));
    if (pass==1u) {
        p_r2y = &p_motion_fusion_flow_tables_list->mono1;
        p_r2y_cr_id = &p_filters->update_flags.cr.mono1;
    } else if (pass==8u) {
        p_r2y = &p_motion_fusion_flow_tables_list->mono8;
        p_r2y_cr_id = &p_filters->update_flags.cr.mono8;
    } else {
        rval = IK_ERR_0004;
    }
    p_extra_window = p_motion_fusion_flow_tables_list->p_extra_window;

    // start exe
    if (rval == IK_OK) {
        (void)amba_ik_system_memcpy(p_flow, p_flow, sizeof(uint8));//misra fix.
        if ((p_filters->update_flags.iso.window_size_info_updated == 1U) ||
            (p_filters->update_flags.iso.cfa_window_size_info_updated == 1U) ||
            (p_filters->update_flags.iso.dmy_range_updated == 1U) ||
            (p_filters->update_flags.iso.dzoom_info_updated == 1U) ||
            (p_filters->update_flags.iso.vin_active_win_updated == 1U) ||
            (p_filters->update_flags.iso.stitching_info_updated == 1U) ||
            (p_filters->update_flags.iso.overlap_x_updated == 1U) ||
            (p_filters->update_flags.iso.flip_mode_updated == 1U) ||
            (p_filters->update_flags.iso.warp_enable_updated == 1U) ||
            (p_filters->update_flags.iso.warp_enable_2nd_updated == 1U)) {
            window_calculate_updated = 1U;
        }

        // default value
        if(p_filters->update_flags.iso.is_1st_frame == 1U) {
            ikc_in_r2y_default_tbl_t in_r2y_default_tbl;
            ikc_out_r2y_default_tbl_t out_r2y_default_tbl;
            out_r2y_default_tbl.p_cr_112 = p_r2y->p_CR_buf_112;
            out_r2y_default_tbl.cr_112_size = CR_SIZE_112;
            rval |= ikc_r2y_default_tbl(&in_r2y_default_tbl, &out_r2y_default_tbl);
        }
        ///////////// calib
        // input mode
        if(p_filters->update_flags.iso.is_1st_frame == 1U) { //should only configure once at 1st time.
            ikc_in_input_mode_t in_input_mode;
            ikc_out_input_mode_t out_input_mode;

            rval |= exe_check_vin_sensor(p_filters);
            if((p_r2y_cr_id->cr_4_update == 0U)||
               (p_r2y_cr_id->cr_6_update == 0U)||
               (p_r2y_cr_id->cr_7_update == 0U)||
               (p_r2y_cr_id->cr_8_update == 0U)||
               (p_r2y_cr_id->cr_9_update == 0U)||
               (p_r2y_cr_id->cr_11_update == 0U)||
               (p_r2y_cr_id->cr_12_update == 0U)||
               (p_r2y_cr_id->cr_13_update == 0U)||
               (p_r2y_cr_id->cr_16_update == 0U)||
               (p_r2y_cr_id->cr_21_update == 0U)||
               (p_r2y_cr_id->cr_22_update == 0U)||
               (p_r2y_cr_id->cr_23_update == 0U)||
               (p_r2y_cr_id->cr_24_update == 0U)||
               (p_r2y_cr_id->cr_26_update == 0U)||
               (p_r2y_cr_id->cr_29_update == 0U)||
               (p_r2y_cr_id->cr_30_update == 0U)||
               (p_r2y_cr_id->cr_31_update == 0U)||
               (p_r2y_cr_id->cr_32_update == 0U)||
               (p_r2y_cr_id->cr_117_update == 0U)) {
                amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_input_mode !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
                rval |= IK_ERR_0000;
            } else {
                in_input_mode.flip_mode = p_filters->input_param.flip_mode;
                in_input_mode.p_sensor_info = &p_filters->input_param.sensor_info;
                in_input_mode.is_yuv_mode = 0U;
                out_input_mode.p_cr_4 = p_r2y->p_CR_buf_4;
                out_input_mode.cr_4_size = CR_SIZE_4;
                out_input_mode.p_cr_6 = p_r2y->p_CR_buf_6;
                out_input_mode.cr_6_size = CR_SIZE_6;
                out_input_mode.p_cr_7 = p_r2y->p_CR_buf_7;
                out_input_mode.cr_7_size = CR_SIZE_7;
                out_input_mode.p_cr_8 = p_r2y->p_CR_buf_8;
                out_input_mode.cr_8_size = CR_SIZE_8;
                out_input_mode.p_cr_9 = p_r2y->p_CR_buf_9;
                out_input_mode.cr_9_size = CR_SIZE_9;
                out_input_mode.p_cr_11 = p_r2y->p_CR_buf_11;
                out_input_mode.cr_11_size = CR_SIZE_11;
                out_input_mode.p_cr_12 = p_r2y->p_CR_buf_12;
                out_input_mode.cr_12_size = CR_SIZE_12;
                out_input_mode.p_cr_13 = p_r2y->p_CR_buf_13;
                out_input_mode.cr_13_size = CR_SIZE_13;
                out_input_mode.p_cr_16 = p_r2y->p_CR_buf_16;
                out_input_mode.cr_16_size = CR_SIZE_16;
                out_input_mode.p_cr_21 = p_r2y->p_CR_buf_21;
                out_input_mode.cr_21_size = CR_SIZE_21;
                out_input_mode.p_cr_22 = p_r2y->p_CR_buf_22;
                out_input_mode.cr_22_size = CR_SIZE_22;
                out_input_mode.p_cr_23 = p_r2y->p_CR_buf_23;
                out_input_mode.cr_23_size = CR_SIZE_23;
                out_input_mode.p_cr_24 = p_r2y->p_CR_buf_24;
                out_input_mode.cr_24_size = CR_SIZE_24;
                out_input_mode.p_cr_26 = p_r2y->p_CR_buf_26;
                out_input_mode.cr_26_size = CR_SIZE_26;
                out_input_mode.p_cr_29 = p_r2y->p_CR_buf_29;
                out_input_mode.cr_29_size = CR_SIZE_29;
                out_input_mode.p_cr_30 = p_r2y->p_CR_buf_30;
                out_input_mode.cr_30_size = CR_SIZE_30;
                out_input_mode.p_cr_31 = p_r2y->p_CR_buf_31;
                out_input_mode.cr_31_size = CR_SIZE_31;
                out_input_mode.p_cr_32 = p_r2y->p_CR_buf_32;
                out_input_mode.cr_32_size = CR_SIZE_32;
                out_input_mode.p_cr_117 = p_r2y->p_CR_buf_117;
                out_input_mode.cr_117_size = CR_SIZE_117;
                out_input_mode.p_flow = &p_flow->flow_info;
                out_input_mode.p_calib = &p_flow->calib;
                rval |= ikc_input_mode(&in_input_mode, &out_input_mode);
            }
        }
        // sbp
        if((p_filters->update_flags.iso.static_bpc_updated == 1u) || (p_filters->update_flags.iso.static_bpc_internal_updated == 1u) ||
           (p_filters->update_flags.iso.dynamic_bpc_updated == 1u) || (p_filters->update_flags.iso.window_size_info_updated == 1U)) {
            if(p_filters->input_param.ctx_buf.sbp_internal_mode_flag == 1u) {
                ikc_in_static_bad_pixel_internal_t in_sbp_internal;
                ikc_out_static_bad_pixel_internal_t out_sbp;
                ik_static_bad_pixel_correction_internal_t sbp_internal_tmp;
                const void *void_ptr;

                if(p_r2y_cr_id->cr_12_update == 0U) {
                    amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_static_bad_pixel_internal !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
                    rval |= IK_ERR_0000;
                } else {
                    //consider 64 bit compiler, flow data sbp addr(u32) goes on physical addr + offset.
                    (void)amba_ik_system_memcpy(&sbp_internal_tmp,&p_filters->input_param.sbp_internal,sizeof(ik_static_bad_pixel_correction_internal_t));
                    void_ptr = amba_ik_system_virt_to_phys(sbp_internal_tmp.p_map);
                    (void)amba_ik_system_memcpy(&sbp_internal_tmp.p_map, &void_ptr, sizeof(void*));
                    in_sbp_internal.dbp_enable = p_filters->input_param.dynamic_bpc.enable;
                    in_sbp_internal.p_sbp_internal = &sbp_internal_tmp;
                    out_sbp.p_cr_12 = p_r2y->p_CR_buf_12;
                    out_sbp.cr_12_size = CR_SIZE_12;
                    out_sbp.p_flow = &p_flow->flow_info;
                    rval |= ikc_static_bad_pixel_internal(&in_sbp_internal, &out_sbp);
                }
            } else {
                ikc_in_static_bad_pixel_t in_sbp;
                ikc_out_static_bad_pixel_t out_sbp;
                ik_static_bad_pxl_cor_t static_bpc_tmp;
                const void *void_ptr;

                if(p_r2y_cr_id->cr_12_update == 0U) {
                    amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_static_bad_pixel !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
                    rval |= IK_ERR_0000;
                } else {
                    //consider 64 bit compiler, flow data sbp addr(u32) goes on physical addr + offset.
                    (void)amba_ik_system_memcpy(&static_bpc_tmp,&p_filters->input_param.static_bpc,sizeof(ik_static_bad_pxl_cor_t));
                    void_ptr = amba_ik_system_virt_to_phys(static_bpc_tmp.calib_sbp_info.sbp_buffer);
                    (void)amba_ik_system_memcpy(&static_bpc_tmp.calib_sbp_info.sbp_buffer, &void_ptr, sizeof(void*));
                    in_sbp.dbp_enable = p_filters->input_param.dynamic_bpc.enable;
                    in_sbp.sbp_enable = p_filters->input_param.sbp_enable;
                    in_sbp.p_static_bpc = &static_bpc_tmp;
                    in_sbp.p_vin_sensor = &p_filters->input_param.window_size_info.vin_sensor;
                    out_sbp.p_cr_12 = p_r2y->p_CR_buf_12;
                    out_sbp.cr_12_size = CR_SIZE_12;
                    out_sbp.p_flow = &p_flow->flow_info;
                    rval |= ikc_static_bad_pixel(&in_sbp, &out_sbp);
                }
            }
            p_flow->flow_info.update_sbp = 1U;
        } else {
            p_flow->flow_info.update_sbp = 0U;
        }
        // vig
        if ((p_filters->update_flags.iso.vignette_compensation_updated == 1U)||
            (p_filters->update_flags.iso.window_size_info_updated == 1U) || (window_calculate_updated == 1U)) {
            ikc_in_vignette_t in_vig;
            ikc_out_vignette_t out_vig;

            if((p_r2y_cr_id->cr_9_update == 0U)||
               (p_r2y_cr_id->cr_10_update == 0U)||
               (p_r2y_cr_id->cr_11_update == 0U)) {
                amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_vignette !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
                rval |= IK_ERR_0000;
            } else {
                in_vig.vig_enable = p_filters->input_param.vig_enable;
                in_vig.flip_mode = p_filters->input_param.flip_mode;
                in_vig.p_vignette_compensation = &p_filters->input_param.vignette_compensation;
                in_vig.p_vin_sensor = &p_filters->input_param.window_size_info.vin_sensor;
                in_vig.p_dmy_win_geo = &p_filters->input_param.ctx_buf.result_win.dmy_win_geo;
                in_vig.p_active_window = &p_filters->input_param.active_window;
                out_vig.p_cr_9 = p_r2y->p_CR_buf_9;
                out_vig.cr_9_size = CR_SIZE_9;
                out_vig.p_cr_10 = p_r2y->p_CR_buf_10;
                out_vig.cr_10_size = CR_SIZE_10;
                out_vig.p_cr_11 = p_r2y->p_CR_buf_11;
                out_vig.cr_11_size = CR_SIZE_11;
                out_vig.p_flow = &p_flow->flow_info;
                rval |= ikc_vignette(&in_vig, &out_vig);
            }
            p_flow->flow_info.update_vignette = 1U;
        } else {
            p_flow->flow_info.update_vignette = 0U;
        }
        ////////////// hdr_ce_cr
        // before_ce_gain
        if(p_filters->update_flags.iso.mono_before_ce_wb_gain_updated == 1u) {
            ikc_in_before_ce_gain_t in_before_ce_gain;
            ikc_out_before_ce_gain_t out_before_ce_gain;

            if(p_r2y_cr_id->cr_13_update == 0U) {
                amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_before_ce_gain !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
                rval |= IK_ERR_0000;
            } else {
                in_before_ce_gain.is_ir_only = 0u;//((p_filters->input_param.sensor_info.sensor_mode == 1U) && (p_filters->input_param.rgb_ir.ir_only == 1U))?1U:0U;
                in_before_ce_gain.p_before_ce_wb_gain = &p_filters->input_param.mono_before_ce_wb_gain;
                out_before_ce_gain.p_cr_13 = p_r2y->p_CR_buf_13;
                out_before_ce_gain.cr_13_size = CR_SIZE_13;
                rval |= ikc_before_ce_gain(&in_before_ce_gain, &out_before_ce_gain);
            }
        }
        // hdr_blend and hdr_alpha
        if(p_filters->update_flags.iso.hdr_blend_updated == 1u) {
            ikc_in_hdr_blend_t in_hdr_blend;
            ikc_out_hdr_blend_t out_hdr_blend;

            if(p_r2y_cr_id->cr_4_update == 0U) {
                amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_hdr_blend !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
                rval |= IK_ERR_0000;
            } else {
                in_hdr_blend.exp_num = 1u;
                in_hdr_blend.p_hdr_raw_info = &p_filters->input_param.hdr_raw_info;
                in_hdr_blend.p_hdr_blend = &p_filters->input_param.hdr_blend;
                out_hdr_blend.p_cr_4 = p_r2y->p_CR_buf_4;
                out_hdr_blend.cr_4_size = CR_SIZE_4;
                out_hdr_blend.p_flow = &p_flow->flow_info;
                rval |= ikc_hdr_blend(&in_hdr_blend, &out_hdr_blend);
            }
        }

        // hdr_tone_curve
        if(p_filters->update_flags.iso.fe_tone_curve_updated == 1u) {
            ikc_in_hdr_tone_curve_t in_hdr_tc;
            ikc_out_hdr_tone_curve_t out_hdr_tc;

            if((p_r2y_cr_id->cr_4_update == 0U)||
               (p_r2y_cr_id->cr_5_update == 0U)) {
                amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_front_end_tone_curve !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
                rval |= IK_ERR_0000;
            } else {
                in_hdr_tc.p_fe_tone_curve = &p_filters->input_param.fe_tone_curve;
                out_hdr_tc.p_cr_4 = p_r2y->p_CR_buf_4;
                out_hdr_tc.cr_4_size = CR_SIZE_4;
                out_hdr_tc.p_cr_5 = p_r2y->p_CR_buf_5;
                out_hdr_tc.cr_5_size = CR_SIZE_5;
                rval |= ikc_front_end_tone_curve(&in_hdr_tc, &out_hdr_tc);
            }
        }

        // losy decompress
        if((p_filters->update_flags.iso.sensor_information_updated == 1u)||(window_calculate_updated == 1U)) {
            ikc_in_decompress_t in_decompress;
            ikc_out_decompress_t out_decompress;

            if(p_r2y_cr_id->cr_4_update == 0U) {
                amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_decompression !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
                rval |= IK_ERR_0000;
            } else {
                in_decompress.exp_num = 1u;
                in_decompress.compress_mode = p_filters->input_param.sensor_info.compression;////0~8, 256~259.
                in_decompress.p_dmy_win_geo = &p_filters->input_param.ctx_buf.result_win.dmy_win_geo;
                out_decompress.p_cr_4 = p_r2y->p_CR_buf_4;
                out_decompress.cr_4_size = CR_SIZE_4;
                rval |= ikc_decompression(&in_decompress, &out_decompress);
                if(p_filters->update_flags.iso.is_1st_frame == 1u) {
                    p_filters->input_param.ctx_buf.first_compression_offset = p_filters->input_param.sensor_info.compression_offset;
                }
            }
        }
        //sanity check, mutually exclusive between TC / compression_offset.
        if((p_filters->input_param.fe_tone_curve.decompand_enable != 0U) && (p_filters->input_param.ctx_buf.first_compression_offset != 0)) {
            amba_ik_system_print_int32_5("[IK] exe_process_hdr_ce_cr fail, invalid decompand_enable : %d, first_compression_offset : %d", (int32)p_filters->input_param.fe_tone_curve.decompand_enable, p_filters->input_param.ctx_buf.first_compression_offset, DC_I, DC_I, DC_I);
            rval |= IK_ERR_0008;
        }

        // hdr exp
        if(p_filters->update_flags.iso.mono_exp0_fe_static_blc_level_updated == 1u) {
            ikc_in_hdr_blc_t in_hdr_blc;
            ikc_out_hdr_blc_t out_hdr_blc;

            if((p_r2y_cr_id->cr_4_update == 0U)||
               (p_r2y_cr_id->cr_6_update == 0U)||
               (p_r2y_cr_id->cr_7_update == 0U)||
               (p_r2y_cr_id->cr_8_update == 0U)) {
                amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_hdr_black_level !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
                rval |= IK_ERR_0000;
            } else {
                in_hdr_blc.exp_num = 1u;
                in_hdr_blc.rgb_ir_mode = p_filters->input_param.rgb_ir.mode;
                in_hdr_blc.compression_offset = ((p_filters->input_param.sensor_info.compression > IK_RAW_COMPRESS_OFF)&&(p_filters->input_param.sensor_info.compression <= IK_RAW_COMPRESS_10p5)) ? p_filters->input_param.ctx_buf.first_compression_offset : 0;
                (void)amba_ik_system_memcpy(&p_filters->input_param.ctx_buf.ir_only_trick_buffer.mono_exp0_fe_static_blc_level, &p_filters->input_param.mono_exp0_fe_static_blc_level, sizeof(ik_mono_exp0_fe_static_blc_level_t));
                in_hdr_blc.p_exp0_frontend_static_blc = &p_filters->input_param.ctx_buf.ir_only_trick_buffer.mono_exp0_fe_static_blc_level;
                in_hdr_blc.p_exp1_frontend_static_blc = &p_filters->input_param.ctx_buf.ir_only_trick_buffer.exp1_frontend_static_blc;
                in_hdr_blc.p_exp2_frontend_static_blc = &p_filters->input_param.ctx_buf.ir_only_trick_buffer.exp2_frontend_static_blc;
                out_hdr_blc.p_cr_4 = p_r2y->p_CR_buf_4;
                out_hdr_blc.cr_4_size = CR_SIZE_4;
                out_hdr_blc.p_cr_6 = p_r2y->p_CR_buf_6;
                out_hdr_blc.cr_6_size = CR_SIZE_6;
                out_hdr_blc.p_cr_7 = p_r2y->p_CR_buf_7;
                out_hdr_blc.cr_7_size = CR_SIZE_7;
                out_hdr_blc.p_cr_8 = p_r2y->p_CR_buf_8;
                out_hdr_blc.cr_8_size = CR_SIZE_8;
                rval |= ikc_hdr_black_level(&in_hdr_blc, &out_hdr_blc);
            }
        }

        if(p_filters->update_flags.iso.mono_exp0_fe_wb_gain_updated == 1u) {
            ikc_in_hdr_dgain_t in_hdr_dgain = {0};
            ikc_out_hdr_dgain_t out_hdr_dgain;

            if(p_r2y_cr_id->cr_4_update == 0U) {
                amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_hdr_dgain !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
                rval |= IK_ERR_0000;
            } else {
                in_hdr_dgain.exp_num = 1u;
                in_hdr_dgain.rgb_ir_mode = p_filters->input_param.rgb_ir.mode;
                (void)amba_ik_system_memcpy(&p_filters->input_param.ctx_buf.ir_only_trick_buffer.mono_exp0_fe_wb_gain, &p_filters->input_param.mono_exp0_fe_wb_gain, sizeof(ik_mono_exp0_fe_wb_gain_t));
                in_hdr_dgain.p_exp0_frontend_wb_gain = &p_filters->input_param.ctx_buf.ir_only_trick_buffer.mono_exp0_fe_wb_gain;
                in_hdr_dgain.p_exp1_frontend_wb_gain = &p_filters->input_param.ctx_buf.ir_only_trick_buffer.exp1_frontend_wb_gain;
                in_hdr_dgain.p_exp2_frontend_wb_gain = &p_filters->input_param.ctx_buf.ir_only_trick_buffer.exp2_frontend_wb_gain;

                (void)amba_ik_system_memcpy(&p_filters->input_param.ctx_buf.ir_only_trick_buffer.mono_exp0_fe_static_blc_level, &p_filters->input_param.mono_exp0_fe_static_blc_level, sizeof(ik_mono_exp0_fe_static_blc_level_t));
                in_hdr_dgain.p_exp0_frontend_static_blc = &p_filters->input_param.ctx_buf.ir_only_trick_buffer.mono_exp0_fe_static_blc_level;

                out_hdr_dgain.p_cr_4 = p_r2y->p_CR_buf_4;
                out_hdr_dgain.cr_4_size = CR_SIZE_4;
                rval |= ikc_hdr_dgain(&in_hdr_dgain, &out_hdr_dgain);
            }
        }
        // hdr eis
        if((p_filters->input_param.use_hdr_eis == 1U)&&
           ((p_filters->update_flags.iso.hdr_eis_shift_info_updated == 1u)||
            (p_filters->update_flags.iso.sensor_information_updated == 1u)||(window_calculate_updated == 1U))) {
            ikc_in_hdr_eis_t in_hdr_eis;
            ikc_out_hdr_eis_t out_hdr_eis;

            if(p_r2y_cr_id->cr_4_update == 0U) {
                amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_hdr_eis !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
                rval |= IK_ERR_0000;
            } else {
                in_hdr_eis.exp_num = 1u;
                in_hdr_eis.p_dmy_win_geo = &p_filters->input_param.ctx_buf.result_win.dmy_win_geo;
                in_hdr_eis.p_vin_sensor = &p_filters->input_param.window_size_info.vin_sensor;
                in_hdr_eis.horizontal_shift_exp1 = p_filters->input_param.ctx_buf.result_win.hdr_eis_sft.horizontal_shift_exp1;
                in_hdr_eis.vertical_shift_exp1 = p_filters->input_param.ctx_buf.result_win.hdr_eis_sft.vertical_shift_exp1;
                in_hdr_eis.horizontal_shift_exp2 = p_filters->input_param.ctx_buf.result_win.hdr_eis_sft.horizontal_shift_exp2;
                in_hdr_eis.vertical_shift_exp2 = p_filters->input_param.ctx_buf.result_win.hdr_eis_sft.vertical_shift_exp2;
                out_hdr_eis.p_cr_4 = p_r2y->p_CR_buf_4;
                out_hdr_eis.cr_4_size = CR_SIZE_4;
                out_hdr_eis.p_extra_window_info = p_extra_window;
                out_hdr_eis.p_window = &p_flow->window;
                rval |= ikc_hdr_eis(&in_hdr_eis, &out_hdr_eis);
            }
        }
        if((p_filters->update_flags.iso.mono_ce_updated == 1u)||(window_calculate_updated == 1U)) {
            ikc_in_ce_t in_ce;
            ikc_out_ce_t out_ce;

            if(p_r2y_cr_id->cr_13_update == 0U) {
                amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_contrast_enhancement !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
                rval |= IK_ERR_0000;
            } else {
                in_ce.exp_num = 1u;
                in_ce.sensor_pattern = p_filters->input_param.sensor_info.sensor_pattern;
                in_ce.p_dmy_win_geo = &p_filters->input_param.ctx_buf.result_win.dmy_win_geo;
                in_ce.p_ce = &p_filters->input_param.mono_ce;
                out_ce.p_cr_13 = p_r2y->p_CR_buf_13;
                out_ce.cr_13_size = CR_SIZE_13;
                out_ce.p_flow = &p_flow->flow_info;
                rval |= ikc_contrast_enhancement(&in_ce, &out_ce);
            }
        }
        if(p_filters->update_flags.iso.mono_ce_input_table_updated == 1u) {
            ikc_in_ce_input_t in_ce_input;
            ikc_out_ce_input_t out_ce_input;

            if((p_r2y_cr_id->cr_13_update == 0U)||
               (p_r2y_cr_id->cr_14_update == 0U)) {
                amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_contrast_enhancement_input !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
                rval |= IK_ERR_0000;
            } else {
                in_ce_input.ce_enable = p_filters->input_param.mono_ce.enable;
                in_ce_input.p_ce_input_table = &p_filters->input_param.mono_ce_input_table;
                out_ce_input.p_cr_13 = p_r2y->p_CR_buf_13;
                out_ce_input.cr_13_size = CR_SIZE_13;
                out_ce_input.p_cr_14 = p_r2y->p_CR_buf_14;
                out_ce_input.cr_14_size = CR_SIZE_14;
                rval |= ikc_contrast_enhancement_input(&in_ce_input, &out_ce_input);
            }
        }
        if(p_filters->update_flags.iso.mono_ce_output_table_updated == 1u) {
            ikc_in_ce_output_t in_ce_output;
            ikc_out_ce_t out_ce;

            if(p_r2y_cr_id->cr_13_update == 0U) {
                amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_contrast_enhancement_output !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
                rval |= IK_ERR_0000;
            } else {
                in_ce_output.ce_enable = p_filters->input_param.mono_ce.enable;
                in_ce_output.p_ce_out_table = &p_filters->input_param.mono_ce_output_table;
                out_ce.p_cr_13 = p_r2y->p_CR_buf_13;
                out_ce.cr_13_size = CR_SIZE_13;
                rval |= ikc_contrast_enhancement_output(&in_ce_output, &out_ce);
            }
        }
        ///////////////// hdr_ce_sub_cr
        // input mode sub
        if((p_filters->update_flags.iso.is_1st_frame == 1U) && (p_filters->update_flags.iso.sensor_information_updated == 1u)) { //should only configure once at 1st time.
            ikc_in_input_mode_sub_t in_input_mode_sub;
            ikc_out_input_mode_sub_t out_input_mode_sub;

            if((p_r2y_cr_id->cr_45_update == 0U)||
               (p_r2y_cr_id->cr_47_update == 0U)||
               (p_r2y_cr_id->cr_49_update == 0U)||
               (p_r2y_cr_id->cr_50_update == 0U)||
               (p_r2y_cr_id->cr_51_update == 0U)) {
                amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_input_mode_sub !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
                rval |= IK_ERR_0000;
            } else {
                in_input_mode_sub.flip_mode = p_filters->input_param.flip_mode;
                in_input_mode_sub.is_yuv_mode = 0u;
                in_input_mode_sub.sensor_pattern = p_filters->input_param.sensor_info.sensor_pattern;
                out_input_mode_sub.p_cr_45 = p_r2y->p_CR_buf_45;
                out_input_mode_sub.cr_45_size = CR_SIZE_45;
                out_input_mode_sub.p_cr_47 = p_r2y->p_CR_buf_47;
                out_input_mode_sub.cr_47_size = CR_SIZE_47;
                out_input_mode_sub.p_cr_49 = p_r2y->p_CR_buf_49;
                out_input_mode_sub.cr_49_size = CR_SIZE_49;
                out_input_mode_sub.p_cr_50 = p_r2y->p_CR_buf_50;
                out_input_mode_sub.cr_50_size = CR_SIZE_50;
                out_input_mode_sub.p_cr_51 = p_r2y->p_CR_buf_51;
                out_input_mode_sub.cr_51_size = CR_SIZE_51;

                rval |= ikc_input_mode_sub(&in_input_mode_sub, &out_input_mode_sub);
            }
        }

        // before_ce_gain sub
        if(p_filters->update_flags.iso.mono_before_ce_wb_gain_updated == 1u) {
            ikc_in_before_ce_gain_t in_before_ce_gain;
            ikc_out_before_ce_gain_sub_t out_before_ce_gain_sub;

            if(p_r2y_cr_id->cr_51_update == 0U) {
                amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_before_ce_gain_sub !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
                rval |= IK_ERR_0000;
            } else {
                in_before_ce_gain.p_before_ce_wb_gain = &p_filters->input_param.mono_before_ce_wb_gain;
                out_before_ce_gain_sub.p_cr_51 = p_r2y->p_CR_buf_51;
                out_before_ce_gain_sub.cr_51_size = CR_SIZE_51;
                rval |= ikc_before_ce_gain_sub(&in_before_ce_gain, &out_before_ce_gain_sub);
            }
        }
        // hdr_blend sub and hdr_alpha sub
        if(p_filters->update_flags.iso.hdr_blend_updated == 1u) {
            ikc_in_hdr_blend_t in_hdr_blend;
            ikc_out_hdr_blend_sub_t out_hdr_blend_sub;

            if(p_r2y_cr_id->cr_45_update == 0U) {
                amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_hdr_blend_sub !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
                rval |= IK_ERR_0000;
            } else {
                in_hdr_blend.exp_num = 1u;
                in_hdr_blend.p_hdr_raw_info = &p_filters->input_param.hdr_raw_info;
                in_hdr_blend.p_hdr_blend = &p_filters->input_param.hdr_blend;
                out_hdr_blend_sub.p_cr_45 = p_r2y->p_CR_buf_45;
                out_hdr_blend_sub.cr_45_size = CR_SIZE_45;
                rval |= ikc_hdr_blend_sub(&in_hdr_blend, &out_hdr_blend_sub);
            }
        }

        // hdr_tone_curve sub
        if(p_filters->update_flags.iso.fe_tone_curve_updated == 1u) {
            ikc_in_hdr_tone_curve_t in_hdr_tc;
            ikc_out_hdr_tone_curve_sub_t out_hdr_tc_sub;

            if((p_r2y_cr_id->cr_45_update == 0U)||
               (p_r2y_cr_id->cr_46_update == 0U)) {
                amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_front_end_tone_curve_sub !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
                rval |= IK_ERR_0000;
            } else {
                in_hdr_tc.p_fe_tone_curve = &p_filters->input_param.fe_tone_curve;
                out_hdr_tc_sub.p_cr_45 = p_r2y->p_CR_buf_45;
                out_hdr_tc_sub.cr_45_size = CR_SIZE_45;
                out_hdr_tc_sub.p_cr_46 = p_r2y->p_CR_buf_46;
                out_hdr_tc_sub.cr_46_size = CR_SIZE_46;
                rval |= ikc_front_end_tone_curve_sub(&in_hdr_tc, &out_hdr_tc_sub);
            }
        }

        // losy decompress sub
        if((p_filters->update_flags.iso.sensor_information_updated == 1u) || (window_calculate_updated == 1U)) {
            ikc_in_decompress_t in_decompress;
            ikc_out_decompress_sub_t out_decompress_sub;

            if(p_r2y_cr_id->cr_45_update == 0U) {
                amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_decompression_sub !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
                rval |= IK_ERR_0000;
            } else {
                in_decompress.exp_num = 1u;
                in_decompress.compress_mode = p_filters->input_param.sensor_info.compression;//0~8, 256~259.
                in_decompress.p_dmy_win_geo = &p_filters->input_param.ctx_buf.result_win.dmy_win_geo;
                out_decompress_sub.p_cr_45 = p_r2y->p_CR_buf_45;
                out_decompress_sub.cr_45_size = CR_SIZE_45;
                rval |= ikc_decompression_sub(&in_decompress, &out_decompress_sub);
            }
        }

        // hdr exp sub
        if(p_filters->update_flags.iso.mono_exp0_fe_static_blc_level_updated == 1u) {
            ikc_in_hdr_blc_t in_hdr_blc;
            ikc_out_hdr_blc_sub_t out_hdr_blc_sub;

            if(p_r2y_cr_id->cr_45_update == 0U) {
                amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_hdr_black_level_sub !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
                rval |= IK_ERR_0000;
            } else {
                in_hdr_blc.exp_num = 1u;
                in_hdr_blc.rgb_ir_mode = p_filters->input_param.rgb_ir.mode;
                in_hdr_blc.compression_offset = ((p_filters->input_param.sensor_info.compression > IK_RAW_COMPRESS_OFF)&&(p_filters->input_param.sensor_info.compression <= IK_RAW_COMPRESS_10p5)) ? p_filters->input_param.ctx_buf.first_compression_offset : 0;
                (void)amba_ik_system_memcpy(&p_filters->input_param.ctx_buf.ir_only_trick_buffer.mono_exp0_fe_static_blc_level, &p_filters->input_param.mono_exp0_fe_static_blc_level, sizeof(ik_mono_exp0_fe_static_blc_level_t));
                in_hdr_blc.p_exp0_frontend_static_blc = &p_filters->input_param.ctx_buf.ir_only_trick_buffer.mono_exp0_fe_static_blc_level;
                in_hdr_blc.p_exp1_frontend_static_blc = &p_filters->input_param.ctx_buf.ir_only_trick_buffer.exp1_frontend_static_blc;
                in_hdr_blc.p_exp2_frontend_static_blc = &p_filters->input_param.ctx_buf.ir_only_trick_buffer.exp2_frontend_static_blc;
                out_hdr_blc_sub.p_cr_45 = p_r2y->p_CR_buf_45;
                out_hdr_blc_sub.cr_45_size = CR_SIZE_45;
                rval |= ikc_hdr_black_level_sub(&in_hdr_blc, &out_hdr_blc_sub);
            }
        }

        if(p_filters->update_flags.iso.mono_exp0_fe_wb_gain_updated == 1u) {
            ikc_in_hdr_dgain_t in_hdr_dgain = {0};
            ikc_out_hdr_dgain_sub_t out_hdr_dgain_sub;

            if(p_r2y_cr_id->cr_45_update == 0U) {
                amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_hdr_dgain_sub !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
                rval |= IK_ERR_0000;
            } else {
                in_hdr_dgain.exp_num = 1u;
                in_hdr_dgain.rgb_ir_mode = p_filters->input_param.rgb_ir.mode;
                (void)amba_ik_system_memcpy(&p_filters->input_param.ctx_buf.ir_only_trick_buffer.mono_exp0_fe_wb_gain, &p_filters->input_param.mono_exp0_fe_wb_gain, sizeof(ik_mono_exp0_fe_wb_gain_t));
                in_hdr_dgain.p_exp0_frontend_wb_gain = &p_filters->input_param.ctx_buf.ir_only_trick_buffer.mono_exp0_fe_wb_gain;
                in_hdr_dgain.p_exp1_frontend_wb_gain = &p_filters->input_param.ctx_buf.ir_only_trick_buffer.exp1_frontend_wb_gain;
                in_hdr_dgain.p_exp2_frontend_wb_gain = &p_filters->input_param.ctx_buf.ir_only_trick_buffer.exp2_frontend_wb_gain;
                in_hdr_dgain.p_exp0_frontend_static_blc = &p_filters->input_param.mono_exp0_fe_static_blc_level;
                in_hdr_dgain.p_exp1_frontend_static_blc = &p_filters->input_param.exp1_frontend_static_blc;
                in_hdr_dgain.p_exp2_frontend_static_blc = &p_filters->input_param.exp2_frontend_static_blc;
                out_hdr_dgain_sub.p_cr_45 = p_r2y->p_CR_buf_45;
                out_hdr_dgain_sub.cr_45_size = CR_SIZE_45;
                rval |= ikc_hdr_dgain_sub(&in_hdr_dgain, &out_hdr_dgain_sub);
            }
        }

        // hdr eis sub
        if((p_filters->input_param.use_hdr_eis == 1U)&&
           ((p_filters->update_flags.iso.hdr_eis_shift_info_updated == 1u)||
            (p_filters->update_flags.iso.sensor_information_updated == 1u) || (window_calculate_updated == 1U))) {
            ikc_in_hdr_eis_t in_hdr_eis_sub;
            ikc_out_hdr_eis_sub_t out_hdr_eis_sub;

            if(p_r2y_cr_id->cr_45_update == 0U) {
                amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_decompression_sub !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
                rval |= IK_ERR_0000;
            } else {
                in_hdr_eis_sub.exp_num = 1u;
                in_hdr_eis_sub.p_dmy_win_geo = &p_filters->input_param.ctx_buf.result_win.dmy_win_geo;
                in_hdr_eis_sub.p_vin_sensor = &p_filters->input_param.window_size_info.vin_sensor;
                in_hdr_eis_sub.horizontal_shift_exp1 = p_filters->input_param.ctx_buf.result_win.hdr_eis_sft.horizontal_shift_exp1;
                in_hdr_eis_sub.vertical_shift_exp1 = p_filters->input_param.ctx_buf.result_win.hdr_eis_sft.vertical_shift_exp1;
                in_hdr_eis_sub.horizontal_shift_exp2 = p_filters->input_param.ctx_buf.result_win.hdr_eis_sft.horizontal_shift_exp2;
                in_hdr_eis_sub.vertical_shift_exp2 = p_filters->input_param.ctx_buf.result_win.hdr_eis_sft.vertical_shift_exp2;
                out_hdr_eis_sub.p_cr_45 = p_r2y->p_CR_buf_45;
                out_hdr_eis_sub.cr_45_size = CR_SIZE_45;
                rval |= ikc_hdr_eis_sub(&in_hdr_eis_sub, &out_hdr_eis_sub);
            }
        }
        if((p_filters->update_flags.iso.mono_ce_updated == 1u) || (window_calculate_updated == 1U)) {
            ikc_in_ce_t in_ce;
            ikc_out_ce_sub_t out_ce_sub;

            if((p_r2y_cr_id->cr_49_update == 0U)||
               (p_r2y_cr_id->cr_50_update == 0U)||
               (p_r2y_cr_id->cr_51_update == 0U)) {
                amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_contrast_enhancement_sub !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
                rval |= IK_ERR_0000;
            } else {
                in_ce.sensor_pattern = p_filters->input_param.sensor_info.sensor_pattern;
                in_ce.p_dmy_win_geo = &p_filters->input_param.ctx_buf.result_win.dmy_win_geo;
                in_ce.p_ce = &p_filters->input_param.mono_ce;
                out_ce_sub.p_cr_49 = p_r2y->p_CR_buf_49;
                out_ce_sub.cr_49_size = CR_SIZE_49;
                out_ce_sub.p_cr_50 = p_r2y->p_CR_buf_50;
                out_ce_sub.cr_50_size = CR_SIZE_50;
                out_ce_sub.p_cr_51 = p_r2y->p_CR_buf_51;
                out_ce_sub.cr_51_size = CR_SIZE_51;
                rval |= ikc_contrast_enhancement_sub(&in_ce, &out_ce_sub);
            }
        }
        if(p_filters->update_flags.iso.mono_ce_input_table_updated == 1u) {
            ikc_in_ce_input_sub_t in_ce_input_sub;
            ikc_out_ce_input_sub_t out_ce_input_sub;

            if((p_r2y_cr_id->cr_51_update == 0U)||
               (p_r2y_cr_id->cr_52_update == 0U)) {
                amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_contrast_enhancement_input_sub !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
                rval |= IK_ERR_0000;
            } else {
                in_ce_input_sub.ce_enable = p_filters->input_param.mono_ce.enable;
                in_ce_input_sub.radius = p_filters->input_param.mono_ce.radius;
                in_ce_input_sub.epsilon = p_filters->input_param.mono_ce.epsilon;
                in_ce_input_sub.p_ce_input_table = &p_filters->input_param.mono_ce_input_table;
                out_ce_input_sub.p_cr_51 = p_r2y->p_CR_buf_51;
                out_ce_input_sub.cr_51_size = CR_SIZE_51;
                out_ce_input_sub.p_cr_52 = p_r2y->p_CR_buf_52;
                out_ce_input_sub.cr_52_size = CR_SIZE_52;
                rval |= ikc_contrast_enhancement_input_sub(&in_ce_input_sub, &out_ce_input_sub);
            }
        }
        // vig sub
        if ((p_filters->update_flags.iso.vignette_compensation_updated == 1u) || (p_filters->update_flags.iso.window_size_info_updated == 1U) ||
            (window_calculate_updated == 1U)) {
            ikc_in_vignette_t in_vig;
            ikc_out_vignette_sub_t out_vig_sub;

            if((p_r2y_cr_id->cr_47_update == 0U)||
               (p_r2y_cr_id->cr_48_update == 0U)||
               (p_r2y_cr_id->cr_49_update == 0U)) {
                amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_vignette_sub !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
                rval |= IK_ERR_0000;
            } else {
                in_vig.vig_enable = p_filters->input_param.vig_enable;
                in_vig.flip_mode = p_filters->input_param.flip_mode;
                in_vig.p_vignette_compensation = &p_filters->input_param.vignette_compensation;
                in_vig.p_vin_sensor = &p_filters->input_param.window_size_info.vin_sensor;
                in_vig.p_dmy_win_geo = &p_filters->input_param.ctx_buf.result_win.dmy_win_geo;
                in_vig.p_active_window = &p_filters->input_param.active_window;
                out_vig_sub.p_cr_47 = p_r2y->p_CR_buf_47;
                out_vig_sub.cr_47_size = CR_SIZE_47;
                out_vig_sub.p_cr_48 = p_r2y->p_CR_buf_48;
                out_vig_sub.cr_48_size = CR_SIZE_48;
                out_vig_sub.p_cr_49 = p_r2y->p_CR_buf_49;
                out_vig_sub.cr_49_size = CR_SIZE_49;
                rval |= ikc_vignette_sub(&in_vig, &out_vig_sub);
            }
        }
        // rgbir sub
        if(p_filters->update_flags.iso.rgb_ir_updated == 1u) {
            ikc_in_rgb_ir_t in_rgb_ir;
            ikc_out_rgb_ir_sub_t out_rgb_ir_sub;

            if((p_r2y_cr_id->cr_45_update == 0U)||
               (p_r2y_cr_id->cr_47_update == 0U)||
               (p_r2y_cr_id->cr_49_update == 0U)||
               (p_r2y_cr_id->cr_50_update == 0U)) {
                amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_rgb_ir_sub !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
                rval |= IK_ERR_0000;
            } else {
                in_rgb_ir.p_rgb_ir = &p_filters->input_param.rgb_ir;
                in_rgb_ir.p_before_ce_wb_gain = &p_filters->input_param.mono_before_ce_wb_gain;
                in_rgb_ir.p_after_ce_wb_gain = &p_filters->input_param.mono_after_ce_wb_gain;
                in_rgb_ir.p_exp0_frontend_wb_gain = &p_filters->input_param.mono_exp0_fe_wb_gain;
                out_rgb_ir_sub.p_cr_45 = p_r2y->p_CR_buf_45;
                out_rgb_ir_sub.cr_45_size = CR_SIZE_45;
                out_rgb_ir_sub.p_cr_47 = p_r2y->p_CR_buf_47;
                out_rgb_ir_sub.cr_47_size = CR_SIZE_47;
                out_rgb_ir_sub.p_cr_49 = p_r2y->p_CR_buf_49;
                out_rgb_ir_sub.cr_49_size = CR_SIZE_49;
                out_rgb_ir_sub.p_cr_50 = p_r2y->p_CR_buf_50;
                out_rgb_ir_sub.cr_50_size = CR_SIZE_50;
                rval |= ikc_rgb_ir_sub(&in_rgb_ir, &out_rgb_ir_sub);
            }
        }
        ////////////////// cfa
        //dbp
        if(p_filters->update_flags.iso.mono_dynamic_bad_pixel_corr_updated == 1u) {
            ikc_in_dynamic_bad_pixel_t in_dbp;
            ikc_out_dynamic_bad_pixel_t out_dbp;

            if((p_r2y_cr_id->cr_11_update == 0U)||
               (p_r2y_cr_id->cr_12_update == 0U)) {
                amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_dynamic_bad_pixel !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
                rval |= IK_ERR_0000;
            } else {
                if(p_filters->input_param.ctx_buf.sbp_internal_mode_flag == 1u) {
                    in_dbp.sbp_enable = p_filters->input_param.sbp_internal.enable;
                } else {
                    in_dbp.sbp_enable = p_filters->input_param.sbp_enable;
                }
                in_dbp.rgb_ir_mode = 0u;
                in_dbp.p_dbp = &p_filters->input_param.mono_dynamic_bad_pixel_corr;
                out_dbp.p_cr_11 = p_r2y->p_CR_buf_11;
                out_dbp.cr_11_size = CR_SIZE_11;
                out_dbp.p_cr_12 = p_r2y->p_CR_buf_12;
                out_dbp.cr_12_size = CR_SIZE_12;
                rval |= ikc_dynamic_bad_pixel(&in_dbp, &out_dbp);
            }
        }
        // stored ir
        if(p_filters->update_flags.iso.stored_ir_updated == 1u) {
            ikc_in_stored_ir_t in_stored_ir;
            ikc_out_stored_ir_t out_stored_ir;

            if(p_r2y_cr_id->cr_12_update == 0U) {
                amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_stored_ir !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
                rval |= IK_ERR_0000;
            } else {
                in_stored_ir.p_stored_ir = &p_filters->input_param.stored_ir;
                out_stored_ir.p_cr_12 = p_r2y->p_CR_buf_12;
                out_stored_ir.cr_12_size = CR_SIZE_12;
                rval |= ikc_stored_ir(&in_stored_ir, &out_stored_ir);
            }
        }
        // grgb_mismatch
        if(p_filters->update_flags.iso.mono_grgb_mismatch_updated == 1u) {
            ikc_in_grgb_mismatch_t in_grgb;
            ikc_out_grgb_mismatch_t out_grgb;

            if(p_r2y_cr_id->cr_23_update == 0U) {
                amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_grgb_mismatch !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
                rval |= IK_ERR_0000;
            } else {
                in_grgb.p_grgb_mismatch = &p_filters->input_param.mono_grgb_mismatch;
                out_grgb.p_cr_23 = p_r2y->p_CR_buf_23;
                out_grgb.cr_23_size = CR_SIZE_23;
                rval |= ikc_grgb_mismatch(&in_grgb, &out_grgb);
            }
        }

        // cfa_leakage and anti_aliasing
        if((p_filters->update_flags.iso.mono_cfa_leakage_filter_updated == 1u) || (p_filters->update_flags.iso.mono_anti_aliasing_updated == 1u)) {
            ikc_in_cfa_leak_anti_alias_t in_cfa_leak_anti_alias;
            ikc_out_cfa_leak_anti_alias_t out_cfa_leak_anti_alias;

            if(p_r2y_cr_id->cr_12_update == 0U) {
                amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_cfa_leakage_anti_aliasing !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
                rval |= IK_ERR_0000;
            } else {
                in_cfa_leak_anti_alias.p_cfa_leakage_filter = &p_filters->input_param.mono_cfa_leakage_filter;
                in_cfa_leak_anti_alias.p_anti_aliasing = &p_filters->input_param.mono_anti_aliasing;
                out_cfa_leak_anti_alias.p_cr_12 = p_r2y->p_CR_buf_12;
                out_cfa_leak_anti_alias.cr_12_size = CR_SIZE_12;
                rval |= ikc_cfa_leakage_anti_aliasing(&in_cfa_leak_anti_alias, &out_cfa_leak_anti_alias);
            }
        }

        // cfa noise
        if(p_filters->update_flags.iso.mono_cfa_noise_filter_updated == 1u) {
            ikc_in_cfa_noise_t in_cfa_noise;
            ikc_out_cfa_noise_t out_cfa_noise;

            if(p_r2y_cr_id->cr_23_update == 0U) {
                amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_cfa_noise !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
                rval |= IK_ERR_0000;
            } else {
                in_cfa_noise.p_cfa_noise_filter = &p_filters->input_param.mono_cfa_noise_filter;
                out_cfa_noise.p_cr_23 = p_r2y->p_CR_buf_23;
                out_cfa_noise.cr_23_size = CR_SIZE_23;
                rval |= ikc_cfa_noise(&in_cfa_noise, &out_cfa_noise);
            }
        }

        // wb_gain
        if(p_filters->update_flags.iso.mono_after_ce_wb_gain_updated == 1u) {
            ikc_in_after_ce_gain_t in_after_ce_gain;
            ikc_out_after_ce_gain_t out_after_ce_gain;

            if(p_r2y_cr_id->cr_22_update == 0U) {
                amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_after_ce_gain !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
                rval |= IK_ERR_0000;
            } else {
                in_after_ce_gain.p_after_ce_wb_gain = &p_filters->input_param.mono_after_ce_wb_gain;
                out_after_ce_gain.p_cr_22 = p_r2y->p_CR_buf_22;
                out_after_ce_gain.cr_22_size = CR_SIZE_22;
                rval |= ikc_after_ce_gain(&in_after_ce_gain, &out_after_ce_gain);
            }
        }
        // rgbir
        if(p_filters->update_flags.iso.rgb_ir_updated == 1u) {
            ikc_in_rgb_ir_t in_rgb_ir;
            ikc_out_rgb_ir_t out_rgb_ir;

            if((p_r2y_cr_id->cr_4_update == 0U)||
               (p_r2y_cr_id->cr_6_update == 0U)||
               (p_r2y_cr_id->cr_7_update == 0U)||
               (p_r2y_cr_id->cr_8_update == 0U)||
               (p_r2y_cr_id->cr_9_update == 0U)||
               (p_r2y_cr_id->cr_11_update == 0U)||
               (p_r2y_cr_id->cr_12_update == 0U)||
               (p_r2y_cr_id->cr_21_update == 0U)) {
                amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_rgb_ir !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
                rval |= IK_ERR_0000;
            } else {
                in_rgb_ir.flip_mode = p_filters->input_param.flip_mode;
                (void)amba_ik_system_memcpy(&p_filters->input_param.ctx_buf.ir_only_trick_buffer.rgb_ir, &p_filters->input_param.rgb_ir, sizeof(ik_rgb_ir_t));
                in_rgb_ir.p_rgb_ir = &p_filters->input_param.ctx_buf.ir_only_trick_buffer.rgb_ir;
                in_rgb_ir.p_before_ce_wb_gain = &p_filters->input_param.mono_before_ce_wb_gain;
                in_rgb_ir.p_after_ce_wb_gain = &p_filters->input_param.mono_after_ce_wb_gain;
                (void)amba_ik_system_memcpy(&p_filters->input_param.ctx_buf.ir_only_trick_buffer.mono_exp0_fe_wb_gain, &p_filters->input_param.mono_exp0_fe_wb_gain, sizeof(ik_mono_exp0_fe_wb_gain_t));
                in_rgb_ir.p_exp0_frontend_wb_gain = &p_filters->input_param.ctx_buf.ir_only_trick_buffer.mono_exp0_fe_wb_gain;
                out_rgb_ir.p_cr_4 = p_r2y->p_CR_buf_4;
                out_rgb_ir.cr_4_size = CR_SIZE_4;
                out_rgb_ir.p_cr_6 = p_r2y->p_CR_buf_6;
                out_rgb_ir.cr_6_size = CR_SIZE_6;
                out_rgb_ir.p_cr_7 = p_r2y->p_CR_buf_7;
                out_rgb_ir.cr_7_size = CR_SIZE_7;
                out_rgb_ir.p_cr_8 = p_r2y->p_CR_buf_8;
                out_rgb_ir.cr_8_size = CR_SIZE_8;
                out_rgb_ir.p_cr_9 = p_r2y->p_CR_buf_9;
                out_rgb_ir.cr_9_size = CR_SIZE_9;
                out_rgb_ir.p_cr_11 = p_r2y->p_CR_buf_11;
                out_rgb_ir.cr_11_size = CR_SIZE_11;
                out_rgb_ir.p_cr_12 = p_r2y->p_CR_buf_12;
                out_rgb_ir.cr_12_size = CR_SIZE_12;
                out_rgb_ir.p_cr_21 = p_r2y->p_CR_buf_21;
                out_rgb_ir.cr_21_size = CR_SIZE_21;
                out_rgb_ir.p_flow = &p_flow->flow_info;
                if((p_filters->input_param.rgb_ir.ir_only==1U)&&(p_filters->input_param.sensor_info.sensor_mode==1U)) { //ir_only, runing on RGB bayer pattern, carry IR on B channel.
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
        if ((p_filters->update_flags.iso.resample_str_update== 1U) ||
            (p_filters->update_flags.iso.cawarp_enable_updated == 1U)||
            (window_calculate_updated == 1U)) {
            ikc_in_cfa_prescale_t in_cfa_prescale;
            ikc_out_cfa_prescale_t out_cfa_prescale;

            if(p_r2y_cr_id->cr_16_update == 0U) {
                amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_cfa_prescale !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
                rval |= IK_ERR_0000;
            } else {
                in_cfa_prescale.p_flow = &p_flow->flow_info;
                in_cfa_prescale.cfa_cut_off_freq = p_filters->input_param.resample_str.cfa_cut_off_freq;
                in_cfa_prescale.p_dmy_win_geo = &p_filters->input_param.ctx_buf.result_win.dmy_win_geo;
                in_cfa_prescale.p_cfa_win_dim = &p_filters->input_param.ctx_buf.result_win.cfa_win_dim;
                in_cfa_prescale.cawarp_enable = (uint32)p_filters->input_param.cawarp_enable;
                out_cfa_prescale.p_cr_16 = p_r2y->p_CR_buf_16;
                out_cfa_prescale.cr_16_size = CR_SIZE_16;
                rval |= ikc_cfa_prescale(&in_cfa_prescale, &out_cfa_prescale);
            }
        }
        ///////////////// rgb
        // demosaic
        if(p_filters->update_flags.iso.mono_demosaic_updated == 1U) {
            ikc_in_demosaic_t in_demosaic;
            ikc_out_demosaic_t out_demosaic;

            if((p_r2y_cr_id->cr_12_update == 0U)||
               (p_r2y_cr_id->cr_23_update == 0U)||
               (p_r2y_cr_id->cr_24_update == 0U)) {
                amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_demosaic_filter !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
                rval |= IK_ERR_0000;
            } else {
                in_demosaic.p_demosaic = &p_filters->input_param.mono_demosaic;
                out_demosaic.p_cr_12 = p_r2y->p_CR_buf_12;
                out_demosaic.cr_12_size = CR_SIZE_12;
                out_demosaic.p_cr_23 = p_r2y->p_CR_buf_23;
                out_demosaic.cr_23_size = CR_SIZE_23;
                out_demosaic.p_cr_24 = p_r2y->p_CR_buf_24;
                out_demosaic.cr_24_size = CR_SIZE_24;
                rval |= ikc_demosaic_filter(&in_demosaic, &out_demosaic);
            }
        }
        // cc
        if(p_filters->update_flags.iso.is_1st_frame == 1U) {//should only configure once in 1st time, due to R2Y always turn on.
            ikc_in_cc_en_t in_cc_enb;
            ikc_out_cc_en_t out_cc_enb;

            if(p_r2y_cr_id->cr_26_update == 0U) {
                amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_color_correction_enable !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
                rval |= IK_ERR_0000;
            } else {
                in_cc_enb.is_yuv_mode = 0;
                in_cc_enb.use_cc_for_yuv2yuv = 0;
                out_cc_enb.p_cr_26 = p_r2y->p_CR_buf_26;
                out_cc_enb.cr_26_size = CR_SIZE_26;
                rval |= ikc_color_correction_enable(&in_cc_enb, &out_cc_enb);
            }
        }

        // cc_3d
        if(p_filters->update_flags.iso.mono_color_correction_updated == 1U) {
            ikc_in_cc_t in_cc_3d;
            ikc_out_cc_t out_cc_3d;

            if((p_r2y_cr_id->cr_26_update == 0U)||
               (p_r2y_cr_id->cr_27_update == 0U)) {
                amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_color_correction !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
                rval |= IK_ERR_0000;
            } else {
                in_cc_3d.p_color_correction = &p_filters->input_param.mono_color_correction;
                in_cc_3d.is_ir_only = 0u;//((p_filters->input_param.sensor_info.sensor_mode == 1U) && (p_filters->input_param.rgb_ir.ir_only == 1U))?1U:0U;
                out_cc_3d.p_cr_26 = p_r2y->p_CR_buf_26;
                out_cc_3d.cr_26_size = CR_SIZE_26;
                out_cc_3d.p_cr_27 = p_r2y->p_CR_buf_27;
                out_cc_3d.cr_27_size = CR_SIZE_27;
                rval |= ikc_color_correction(&in_cc_3d, &out_cc_3d);
            }
        }

        // cc_out
        if(p_filters->update_flags.iso.mono_tone_curve_updated == 1U) {
            ikc_in_cc_out_t in_cc_out;
            ikc_out_cc_out_t out_cc_out;

            if(p_r2y_cr_id->cr_26_update == 0U) {
                amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_color_correction_out !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
                rval |= IK_ERR_0000;
            } else {
                in_cc_out.p_tone_curve = &p_filters->input_param.mono_tone_curve;
                out_cc_out.p_cr_26 = p_r2y->p_CR_buf_26;
                out_cc_out.cr_26_size = CR_SIZE_26;
                rval |= ikc_color_correction_out(&in_cc_out, &out_cc_out);
            }
        }

        // rgb2yuv
        if((p_filters->update_flags.iso.mono_rgb_to_yuv_matrix_updated == 1U) ||
           (p_filters->update_flags.iso.is_1st_frame == 1U)) {
            ikc_in_rgb2yuv_t in_rgb2yuv;
            ikc_out_rgb2yuv_t out_rgb2yuv;
            ik_rgb_to_yuv_matrix_t rgb_to_yuv_matrix={0};

            if(p_r2y_cr_id->cr_29_update == 0U) {
                amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_rgb2yuv !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
                rval |= IK_ERR_0000;
            } else {
                {
                    rgb_to_yuv_matrix.matrix_values[0] = p_filters->input_param.mono_rgb_to_yuv_matrix.matrix_values[0];
                    rgb_to_yuv_matrix.matrix_values[1] = p_filters->input_param.mono_rgb_to_yuv_matrix.matrix_values[1];
                    rgb_to_yuv_matrix.matrix_values[2] = p_filters->input_param.mono_rgb_to_yuv_matrix.matrix_values[2];
                    rgb_to_yuv_matrix.y_offset = p_filters->input_param.mono_rgb_to_yuv_matrix.y_offset;
                    rgb_to_yuv_matrix.u_offset = 128;
                    rgb_to_yuv_matrix.v_offset = 128;
                }
                in_rgb2yuv.p_rgb_to_yuv_matrix = &rgb_to_yuv_matrix;
                in_rgb2yuv.is_ir_only = 0u;//((p_filters->input_param.sensor_info.sensor_mode == 1U) && (p_filters->input_param.rgb_ir.ir_only == 1U))?1U:0U;
                out_rgb2yuv.p_cr_29 = p_r2y->p_CR_buf_29;
                out_rgb2yuv.cr_29_size = CR_SIZE_29;
                rval |= ikc_rgb2yuv(&in_rgb2yuv, &out_rgb2yuv);
            }
        }
        //////////////// yuv
        if(p_filters->update_flags.iso.chroma_scale_updated == 1U) {
            ikc_in_chroma_scale_t in_chroma_scale;
            ikc_out_chroma_scale_t out_chroma_scale;

            if(p_r2y_cr_id->cr_29_update == 0U) {
                amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_chroma_scale !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
                rval |= IK_ERR_0000;
            } else {
                if ((p_filters->input_param.ability == AMBA_IK_VIDEO_PIPE_Y2Y) ||
                    (p_filters->input_param.ability == AMBA_IK_VIDEO_PIPE_Y2Y_HVH) ||
                    (p_filters->input_param.ability == AMBA_IK_VIDEO_PIPE_Y2Y_HHB) ||
                    (p_filters->input_param.ability == AMBA_IK_VIDEO_PIPE_Y2Y_MD) ||
                    (p_filters->input_param.ability == AMBA_IK_VIDEO_PIPE_Y2Y_MD_HHB)) {
                    in_chroma_scale.is_yuv_mode = 1U;
                } else {
                    in_chroma_scale.is_yuv_mode = 0U;
                }
                in_chroma_scale.p_chroma_scale = &p_filters->input_param.chroma_scale;
                out_chroma_scale.p_cr_29 = p_r2y->p_CR_buf_29;
                out_chroma_scale.cr_29_size = CR_SIZE_29;
                rval |= ikc_chroma_scale(&in_chroma_scale, &out_chroma_scale);
            }
        }
        // chroma_median_filter
        if(p_filters->update_flags.iso.chroma_median_filter_updated == 1U) {
            ikc_in_chroma_median_t in_chroma_median;
            ikc_out_chroma_median_t out_chroma_median;

            if((p_r2y_cr_id->cr_30_update == 0U)||
               (p_r2y_cr_id->cr_31_update == 0U)) {
                amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_chroma_median !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
                rval |= IK_ERR_0000;
            } else {
                in_chroma_median.is_yuv_mode = 1U; // disable chroma median filter
                in_chroma_median.p_chroma_median_filter = &p_filters->input_param.chroma_median_filter;
                out_chroma_median.p_cr_30 = p_r2y->p_CR_buf_30;
                out_chroma_median.cr_30_size = CR_SIZE_30;
                out_chroma_median.p_cr_31 = p_r2y->p_CR_buf_31;
                out_chroma_median.cr_31_size = CR_SIZE_31;
                rval |= ikc_chroma_median(&in_chroma_median, &out_chroma_median);
            }
        }

        // chroma_noise_filter
        if((p_filters->update_flags.iso.chroma_filter_updated == 1U) || (p_filters->update_flags.iso.wide_chroma_filter == 1U) ||
           (p_filters->update_flags.iso.wide_chroma_filter_combine == 1U) || (window_calculate_updated == 1U)) {
            ikc_in_chroma_noise_t in_chroma_noise;
            ikc_out_chroma_noise_t out_chroma_noise;
            uint64 sec2_luma_hscale_phase_inc = p_filters->input_param.ctx_buf.result_win.warp_phase.sec2_hscale_phase_inc;
            uint64 sec2_luma_vscale_phase_inc = p_filters->input_param.ctx_buf.result_win.warp_phase.sec2_vscale_phase_inc;
            uint32 chroma_radius = p_filters->input_param.chroma_filter.radius;
            uint64 sec2_chroma_hscale_phase_inc;
            uint64 sec2_chroma_vscale_phase_inc;

            if(p_r2y_cr_id->cr_36_update == 0U) {
                amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_chroma_noise !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
                rval |= IK_ERR_0000;
            } else {
                in_chroma_noise.p_chroma_filter = &p_filters->input_param.chroma_filter;
                in_chroma_noise.p_wide_chroma_filter = &p_filters->input_param.wide_chroma_filter;
                in_chroma_noise.p_wide_chroma_combine = &p_filters->input_param.wide_chroma_combine;
                if ((in_chroma_noise.p_chroma_filter->radius==32u) && (in_chroma_noise.p_wide_chroma_filter->enable==1u)) {
                    in_chroma_noise.is_wide_chroma_invalid = 1u;
                } else {
                    in_chroma_noise.is_wide_chroma_invalid = 0u;
                }
                out_chroma_noise.p_cr_36 = p_r2y->p_CR_buf_36;
                out_chroma_noise.cr_36_size = CR_SIZE_36;
                out_chroma_noise.p_window = &p_flow->window;
                rval |= ikc_chroma_noise(&in_chroma_noise, &out_chroma_noise);
                p_flow->window.update_chroma_radius = 1U;
                //check radius causes phase_inc overflow.
                sec2_chroma_hscale_phase_inc = (sec2_luma_hscale_phase_inc * (uint64)chroma_radius) >> 5UL;
                sec2_chroma_vscale_phase_inc = (sec2_luma_vscale_phase_inc * (uint64)chroma_radius) >> 5UL;
                if ((sec2_chroma_hscale_phase_inc >= (8UL<<13UL)) || (sec2_chroma_vscale_phase_inc >= (8UL<<13UL))) {
                    amba_ik_system_print_uint32_5("[IK][ERROR] exe_iso_cfg_check_raw2yuv_chroma_filter() fail. One or more downscaling_factor is out of range(>=8<<13), please check chroma radius or window setting!", DC_U, DC_U, DC_U, DC_U, DC_U);
                    amba_ik_system_print_uint32_5("[IK][ERROR] sec2_chroma_hscale_phase_inc:%d, sec2_chroma_vscale_phase_inc:%d, chroma_radius:%d", (uint32)sec2_chroma_hscale_phase_inc, (uint32)sec2_chroma_vscale_phase_inc, chroma_radius, DC_U, DC_U);
                    rval = IK_ERR_0101;
                }
            }
        }
        // luma_processing
        if(p_filters->input_param.mono_fst_luma_process_mode.use_sharpen_not_asf == 0U) {//asf
            ikc_out_asf_t out_asf;
            out_asf.p_cr_30 = p_r2y->p_CR_buf_30;
            out_asf.cr_30_size = CR_SIZE_30;
            if(p_filters->update_flags.iso.mono_adv_spatial_filter_updated == 1U) {
                ikc_in_asf_t in_asf;

                if(p_r2y_cr_id->cr_30_update == 0U) {
                    amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_asf !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
                    rval |= IK_ERR_0000;
                } else {
                    in_asf.p_advance_spatial_filter = &p_filters->input_param.mono_adv_spatial_filter;
                    in_asf.first_luma_processing_mode_updated = (uint32)p_filters->update_flags.iso.mono_fst_luma_process_mode_updated;
                    rval |= ikc_asf(&in_asf, &out_asf);
                }
            }
        } else { //shpA
            ikc_out_sharpen_t out_sharpen;
            out_sharpen.p_cr_30 = p_r2y->p_CR_buf_30;
            out_sharpen.cr_30_size = CR_SIZE_30;

            if((p_filters->update_flags.iso.mono_fst_shp_noise_updated == 1U)||(p_filters->update_flags.iso.mono_fst_shp_fir_updated == 1U)) {
                ikc_in_sharpen_t in_sharpen;

                if(p_r2y_cr_id->cr_30_update == 0U) {
                    amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_sharpen !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
                    rval |= IK_ERR_0000;
                } else {
                    in_sharpen.sharpen_mode = p_filters->input_param.mono_fst_shp_both.mode;
                    in_sharpen.p_first_sharpen_noise = &p_filters->input_param.mono_fst_shp_noise;
                    in_sharpen.p_first_sharpen_fir = &p_filters->input_param.mono_fst_shp_fir;
                    in_sharpen.p_working_buffer = &p_filters->input_param.ctx_buf.mono_fst_shp_working_buffer[0];
                    in_sharpen.working_buffer_size = (uint32)sizeof(p_filters->input_param.ctx_buf.mono_fst_shp_working_buffer);
                    rval |= ikc_sharpen(&in_sharpen, &out_sharpen);
                }
            }

            if(p_filters->update_flags.iso.mono_fst_shp_both_updated == 1U) {
                ikc_in_sharpen_both_t in_sharpen_both;

                if(p_r2y_cr_id->cr_30_update == 0U) {
                    amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_sharpen_both !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
                    rval |= IK_ERR_0000;
                } else {
                    in_sharpen_both.p_first_sharpen_both = &p_filters->input_param.mono_fst_shp_both;
                    rval |= ikc_sharpen_both(&in_sharpen_both, &out_sharpen);
                }
            }

            if(p_filters->update_flags.iso.mono_fst_shp_coring_updated == 1U) {
                ikc_in_sharpen_coring_t in_sharpen_coring;

                if(p_r2y_cr_id->cr_30_update == 0U) {
                    amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_sharpen_coring !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
                    rval |= IK_ERR_0000;
                } else {
                    in_sharpen_coring.p_first_sharpen_coring = &p_filters->input_param.mono_fst_shp_coring;
                    rval |= ikc_sharpen_coring(&in_sharpen_coring, &out_sharpen);
                }
            }

            if(p_filters->update_flags.iso.mono_fst_shp_coring_idx_scale_updated == 1U) {
                ikc_in_sharpen_coring_idx_scale_t in_sharpen_coring_idx_scale;

                if(p_r2y_cr_id->cr_30_update == 0U) {
                    amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_sharpen_coring_idx_scale !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
                    rval |= IK_ERR_0000;
                } else {
                    in_sharpen_coring_idx_scale.p_first_sharpen_coring_idx_scale = &p_filters->input_param.mono_fst_shp_coring_idx_scale;
                    rval |= ikc_sharpen_coring_idx_scale(&in_sharpen_coring_idx_scale, &out_sharpen);
                }
            }

            if(p_filters->update_flags.iso.mono_fst_shp_min_coring_rslt_updated == 1U) {
                ikc_in_sharpen_min_coring_t in_sharpen_min_coring;

                if(p_r2y_cr_id->cr_30_update == 0U) {
                    amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_sharpen_min_coring !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
                    rval |= IK_ERR_0000;
                } else {
                    in_sharpen_min_coring.p_first_sharpen_min_coring_result = &p_filters->input_param.mono_fst_shp_min_coring_rslt;
                    rval |= ikc_sharpen_min_coring(&in_sharpen_min_coring, &out_sharpen);
                }
            }

            if(p_filters->update_flags.iso.mono_fst_shp_max_coring_rslt_updated == 1U) {
                ikc_in_sharpen_max_coring_t in_sharpen_max_coring;

                if(p_r2y_cr_id->cr_30_update == 0U) {
                    amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_sharpen_max_coring !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
                    rval |= IK_ERR_0000;
                } else {
                    in_sharpen_max_coring.p_first_sharpen_max_coring_result = &p_filters->input_param.mono_fst_shp_max_coring_rslt;
                    rval |= ikc_sharpen_max_coring(&in_sharpen_max_coring, &out_sharpen);
                }
            }

            if(p_filters->update_flags.iso.mono_fst_shp_scale_coring_updated == 1U) {
                ikc_in_sharpen_scale_coring_t in_sharpen_scale_coring;

                if(p_r2y_cr_id->cr_30_update == 0U) {
                    amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_sharpen_scale_coring !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
                    rval |= IK_ERR_0000;
                } else {
                    in_sharpen_scale_coring.p_first_sharpen_scale_coring = &p_filters->input_param.mono_fst_shp_scale_coring;
                    rval |= ikc_sharpen_scale_coring(&in_sharpen_scale_coring, &out_sharpen);
                }
            }
        }
        // LNL
        if((p_filters->update_flags.iso.mono_after_ce_wb_gain_updated == 1U) ||
           (p_filters->update_flags.iso.rgb_to_12y_updated == 1U)||
           (p_filters->update_flags.iso.lnl_updated == 1U)||
           (window_calculate_updated == 1U)) {
            ikc_in_lnl_t in_lnl;
            ikc_out_lnl_t out_lnl;

            if(p_r2y_cr_id->cr_30_update == 0U) {
                amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_lnl !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
                rval |= IK_ERR_0000;
            } else {
                in_lnl.p_luma_noise_reduce = &p_filters->input_param.luma_noise_reduce;
                in_lnl.p_after_ce_wb_gain = &p_filters->input_param.mono_after_ce_wb_gain;
                in_lnl.p_rgb_to_12y = &p_filters->input_param.rgb_to_12y;
                in_lnl.p_cfa_win = &p_filters->input_param.ctx_buf.result_win.cfa_win_dim;
                in_lnl.use_sharpen_not_asf = p_filters->input_param.mono_fst_luma_process_mode.use_sharpen_not_asf;
                in_lnl.p_advance_spatial_filter = &p_filters->input_param.mono_adv_spatial_filter;
                in_lnl.p_first_sharpen_both = &p_filters->input_param.mono_fst_shp_both;
                out_lnl.p_cr_30 = p_r2y->p_CR_buf_30;
                out_lnl.cr_30_size = CR_SIZE_30;
                out_lnl.p_flow_info = &p_flow->flow_info;
                rval |= ikc_lnl(&in_lnl, &out_lnl);
            }
        }
        // LNL table
        if((p_filters->update_flags.iso.mono_color_correction_updated == 1U)||
           (p_filters->update_flags.iso.mono_tone_curve_updated == 1U)) {
            ikc_in_lnl_tbl_t in_lnl_tbl;
            ikc_out_lnl_t out_lnl;

            if(p_r2y_cr_id->cr_30_update == 0U) {
                amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_lnl_tbl !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
                rval |= IK_ERR_0000;
            } else {
                in_lnl_tbl.p_color_correction = &p_filters->input_param.mono_color_correction;
                in_lnl_tbl.p_tone_curve = &p_filters->input_param.mono_tone_curve;
                out_lnl.p_cr_30 = p_r2y->p_CR_buf_30;
                out_lnl.cr_30_size = CR_SIZE_30;
                rval |= ikc_lnl_tbl(&in_lnl_tbl, &out_lnl);
            }
        }
        if((p_filters->update_flags.iso.chroma_filter_updated == 1U)||
           (p_filters->update_flags.iso.resample_str_update == 1U)||
           (p_filters->update_flags.iso.window_size_info_updated == 1U)||
           (window_calculate_updated == 1U)) {
            if((p_r2y_cr_id->cr_33_update == 0U)||
               (p_r2y_cr_id->cr_35_update == 0U)||
               (p_r2y_cr_id->cr_37_update == 0U)) {
                amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_main_resampler !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
                rval |= IK_ERR_0000;
            } else {
                ikc_in_sec2_mono1_8_resampler_t in_sec2_mono1_8_resampler;
                ikc_out_sec2_mono1_8_resampler_t out_sec2_mono1_8_resampler;

                in_sec2_mono1_8_resampler.main_cut_off_freq = p_filters->input_param.resample_str.main_cut_off_freq;
                in_sec2_mono1_8_resampler.p_act_win_crop = &p_filters->input_param.ctx_buf.result_win.act_win_crop;
                in_sec2_mono1_8_resampler.p_dmy_win_geo = &p_filters->input_param.ctx_buf.result_win.dmy_win_geo;
                in_sec2_mono1_8_resampler.p_cfa_win_dim = &p_filters->input_param.ctx_buf.result_win.cfa_win_dim;
                in_sec2_mono1_8_resampler.p_main = &p_filters->input_param.window_size_info.main_win;
                in_sec2_mono1_8_resampler.p_dmy_range = &p_filters->input_param.dmy_range;
                in_sec2_mono1_8_resampler.chroma_radius = p_filters->input_param.chroma_filter.radius;
                in_sec2_mono1_8_resampler.is_y2y_420input = (p_filters->input_param.yuv_mode == AMBA_IK_YUV420) ? 1U : 0U;
                in_sec2_mono1_8_resampler.flip_mode = p_filters->input_param.flip_mode;
                in_sec2_mono1_8_resampler.luma_dma_size = p_filters->input_param.warp_buf_info.luma_dma_size;
                in_sec2_mono1_8_resampler.p_burst_tile = &p_filters->input_param.burst_tile;
                out_sec2_mono1_8_resampler.p_cr_33 = p_r2y->p_CR_buf_33;
                out_sec2_mono1_8_resampler.cr_33_size = CR_SIZE_33;
                out_sec2_mono1_8_resampler.p_cr_35 = p_r2y->p_CR_buf_35;
                out_sec2_mono1_8_resampler.cr_35_size = CR_SIZE_35;
                out_sec2_mono1_8_resampler.p_cr_37 = p_r2y->p_CR_buf_37;
                out_sec2_mono1_8_resampler.cr_37_size = CR_SIZE_37;
                rval |= ikc_sec2_mono1_8_resampler(&in_sec2_mono1_8_resampler, &out_sec2_mono1_8_resampler);
            }
        }
    }
    return rval;
}

uint32 exe_process_mono3_cr(idsp_flow_ctrl_t *p_flow, void *p_flow_tbl_list, amba_ik_filter_t *p_filters)
{
    uint32 rval = IK_OK;
    amba_ik_motion_fusion_flow_tables_list_t *p_motion_fusion_flow_tables_list;
    const amba_ik_mono3_flow_tables_list_t *p_mono3;
    const amba_ik_mono3_cr_id_list_t *p_mono3_cr_id;
    uint32 window_calculate_updated = 0U;

    (void)amba_ik_system_memcpy(&p_motion_fusion_flow_tables_list, &p_flow_tbl_list, sizeof(amba_ik_motion_fusion_flow_tables_list_t*));
    p_mono3 = &p_motion_fusion_flow_tables_list->mono3;
    p_mono3_cr_id = &p_filters->update_flags.cr.mono3;

    (void)amba_ik_system_memcpy(p_flow, p_flow, sizeof(uint8));//misra fix.

    if ((p_filters->update_flags.iso.window_size_info_updated == 1U) ||
        (p_filters->update_flags.iso.cfa_window_size_info_updated == 1U) ||
        (p_filters->update_flags.iso.dmy_range_updated == 1U) ||
        (p_filters->update_flags.iso.dzoom_info_updated == 1U) ||
        (p_filters->update_flags.iso.vin_active_win_updated == 1U) ||
        (p_filters->update_flags.iso.stitching_info_updated == 1U) ||
        (p_filters->update_flags.iso.overlap_x_updated == 1U) ||
        (p_filters->update_flags.iso.flip_mode_updated == 1U) ||
        (p_filters->update_flags.iso.warp_enable_updated == 1U) ||
        (p_filters->update_flags.iso.warp_enable_2nd_updated == 1U)) {
        window_calculate_updated = 1U;
    }

    if (window_calculate_updated==1U) {;} //misra

    // default value
    if(p_filters->update_flags.iso.is_1st_frame == 1U) {
        ikc_in_mono3_default_tbl_t in_mono3_default_tbl;
        ikc_out_mono3_default_tbl_t out_mono3_default_tbl;
        out_mono3_default_tbl.p_cr_26 = p_mono3->p_CR_buf_26;
        out_mono3_default_tbl.cr_26_size = CR_SIZE_26;
        rval |= ikc_mono3_default_tbl(&in_mono3_default_tbl, &out_mono3_default_tbl);
    }

    // start exe
    // mono_cc
    if(p_filters->update_flags.iso.fusion_updated == 1U) {
        if((p_mono3_cr_id->cr_26_update == 0U)||
           (p_mono3_cr_id->cr_27_update == 0U)) {
            amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_mono3 !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
            rval |= IK_ERR_0000;
        } else {
            ikc_in_mono3_t in_mono3;
            ikc_out_mono3_t out_mono3;

            in_mono3.p_fusion = &p_filters->input_param.fusion;
            out_mono3.p_cr_26 = p_mono3->p_CR_buf_26;
            out_mono3.cr_26_size = CR_SIZE_26;
            out_mono3.p_cr_27 = p_mono3->p_CR_buf_27;
            out_mono3.cr_27_size = CR_SIZE_27;
            rval |= ikc_mono3(&in_mono3, &out_mono3);
        }
    }
    // resampler
    if((p_filters->update_flags.iso.chroma_filter_updated == 1U)||
       (p_filters->update_flags.iso.resample_str_update == 1U)||
       (p_filters->update_flags.iso.window_size_info_updated == 1U)||
       (window_calculate_updated == 1U)) {
        if((p_mono3_cr_id->cr_33_update == 0U)||
           (p_mono3_cr_id->cr_35_update == 0U)||
           (p_mono3_cr_id->cr_37_update == 0U)) {
            amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_main_resampler !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
            rval |= IK_ERR_0000;
        } else {
            ikc_in_sec2_mono3_resampler_t in_sec2_mono3_resampler;
            ikc_out_sec2_mono3_resampler_t out_sec2_mono3_resampler;

            in_sec2_mono3_resampler.main_cut_off_freq = p_filters->input_param.resample_str.main_cut_off_freq;
            in_sec2_mono3_resampler.p_act_win_crop = &p_filters->input_param.ctx_buf.result_win.act_win_crop;
            in_sec2_mono3_resampler.p_dmy_win_geo = &p_filters->input_param.ctx_buf.result_win.dmy_win_geo;
            in_sec2_mono3_resampler.p_cfa_win_dim = &p_filters->input_param.ctx_buf.result_win.cfa_win_dim;
            in_sec2_mono3_resampler.p_main = &p_filters->input_param.window_size_info.main_win;
            in_sec2_mono3_resampler.p_dmy_range = &p_filters->input_param.dmy_range;
            in_sec2_mono3_resampler.chroma_radius = p_filters->input_param.chroma_filter.radius;
            in_sec2_mono3_resampler.is_y2y_420input = (p_filters->input_param.yuv_mode == AMBA_IK_YUV420) ? 1U : 0U;
            in_sec2_mono3_resampler.flip_mode = p_filters->input_param.flip_mode;
            in_sec2_mono3_resampler.luma_dma_size = p_filters->input_param.warp_buf_info.luma_dma_size;
            in_sec2_mono3_resampler.p_burst_tile = &p_filters->input_param.burst_tile;
            out_sec2_mono3_resampler.p_cr_33 = p_mono3->p_CR_buf_33;
            out_sec2_mono3_resampler.cr_33_size = CR_SIZE_33;
            out_sec2_mono3_resampler.p_cr_35 = p_mono3->p_CR_buf_35;
            out_sec2_mono3_resampler.cr_35_size = CR_SIZE_35;
            out_sec2_mono3_resampler.p_cr_37 = p_mono3->p_CR_buf_37;
            out_sec2_mono3_resampler.cr_37_size = CR_SIZE_37;
            rval |= ikc_sec2_mono3_resampler(&in_sec2_mono3_resampler, &out_sec2_mono3_resampler);
        }
    }

    return rval;
}

uint32 exe_process_mono4_cr(idsp_flow_ctrl_t *p_flow, void *p_flow_tbl_list, amba_ik_filter_t *p_filters)
{
    uint32 rval = IK_OK;
    amba_ik_motion_fusion_flow_tables_list_t *p_motion_fusion_flow_tables_list;
    const amba_ik_mono4_flow_tables_list_t *p_mono4;
    const amba_ik_mono4_cr_id_list_t *p_mono4_cr_id;
    uint32 window_calculate_updated = 0U;

    (void)amba_ik_system_memcpy(&p_motion_fusion_flow_tables_list, &p_flow_tbl_list, sizeof(amba_ik_motion_fusion_flow_tables_list_t*));
    p_mono4 = &p_motion_fusion_flow_tables_list->mono4;
    p_mono4_cr_id = &p_filters->update_flags.cr.mono4;

    (void)amba_ik_system_memcpy(p_flow, p_flow, sizeof(uint8));//misra fix.

    if ((p_filters->update_flags.iso.window_size_info_updated == 1U) ||
        (p_filters->update_flags.iso.cfa_window_size_info_updated == 1U) ||
        (p_filters->update_flags.iso.dmy_range_updated == 1U) ||
        (p_filters->update_flags.iso.dzoom_info_updated == 1U) ||
        (p_filters->update_flags.iso.vin_active_win_updated == 1U) ||
        (p_filters->update_flags.iso.stitching_info_updated == 1U) ||
        (p_filters->update_flags.iso.overlap_x_updated == 1U) ||
        (p_filters->update_flags.iso.flip_mode_updated == 1U) ||
        (p_filters->update_flags.iso.warp_enable_updated == 1U) ||
        (p_filters->update_flags.iso.warp_enable_2nd_updated == 1U)) {
        window_calculate_updated = 1U;
    }

    if (window_calculate_updated==1U) {;} //misra

    // default value
    if(p_filters->update_flags.iso.is_1st_frame == 1U) {
        ikc_in_mono4_default_tbl_t in_mono4_default_tbl;
        ikc_out_mono4_default_tbl_t out_mono4_default_tbl;
        out_mono4_default_tbl.p_cr_26 = p_mono4->p_CR_buf_26;
        out_mono4_default_tbl.cr_26_size = CR_SIZE_26;
        rval |= ikc_mono4_default_tbl(&in_mono4_default_tbl, &out_mono4_default_tbl);
    }

    // start exe
    if((p_filters->update_flags.iso.chroma_filter_updated == 1U)||
       (p_filters->update_flags.iso.resample_str_update == 1U)||
       (p_filters->update_flags.iso.window_size_info_updated == 1U)||
       (window_calculate_updated == 1U)) {
        if((p_mono4_cr_id->cr_33_update == 0U)||
           (p_mono4_cr_id->cr_35_update == 0U)||
           (p_mono4_cr_id->cr_37_update == 0U)) {
            amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_main_resampler !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
            rval |= IK_ERR_0000;
        } else {
            ikc_in_sec2_mono4_resampler_t in_sec2_mono4_resampler;
            ikc_out_sec2_mono4_resampler_t out_sec2_mono4_resampler;

            in_sec2_mono4_resampler.main_cut_off_freq = p_filters->input_param.resample_str.main_cut_off_freq;
            in_sec2_mono4_resampler.p_act_win_crop = &p_filters->input_param.ctx_buf.result_win.act_win_crop;
            in_sec2_mono4_resampler.p_dmy_win_geo = &p_filters->input_param.ctx_buf.result_win.dmy_win_geo;
            in_sec2_mono4_resampler.p_cfa_win_dim = &p_filters->input_param.ctx_buf.result_win.cfa_win_dim;
            in_sec2_mono4_resampler.p_main = &p_filters->input_param.window_size_info.main_win;
            in_sec2_mono4_resampler.p_dmy_range = &p_filters->input_param.dmy_range;
            in_sec2_mono4_resampler.chroma_radius = p_filters->input_param.chroma_filter.radius;
            in_sec2_mono4_resampler.is_y2y_420input = (p_filters->input_param.yuv_mode == AMBA_IK_YUV420) ? 1U : 0U;
            in_sec2_mono4_resampler.flip_mode = p_filters->input_param.flip_mode;
            in_sec2_mono4_resampler.luma_dma_size = p_filters->input_param.warp_buf_info.luma_dma_size;
            in_sec2_mono4_resampler.p_burst_tile = &p_filters->input_param.burst_tile;
            out_sec2_mono4_resampler.p_cr_33 = p_mono4->p_CR_buf_33;
            out_sec2_mono4_resampler.cr_33_size = CR_SIZE_33;
            out_sec2_mono4_resampler.p_cr_35 = p_mono4->p_CR_buf_35;
            out_sec2_mono4_resampler.cr_35_size = CR_SIZE_35;
            out_sec2_mono4_resampler.p_cr_37 = p_mono4->p_CR_buf_37;
            out_sec2_mono4_resampler.cr_37_size = CR_SIZE_37;
            rval |= ikc_sec2_mono4_resampler(&in_sec2_mono4_resampler, &out_sec2_mono4_resampler);
        }
    }


    return rval;
}

uint32 exe_process_mono5_cr(idsp_flow_ctrl_t *p_flow, void *p_flow_tbl_list, amba_ik_filter_t *p_filters)
{
    uint32 rval = IK_OK;
    amba_ik_motion_fusion_flow_tables_list_t *p_motion_fusion_flow_tables_list;
    const amba_ik_mono5_flow_tables_list_t *p_mono5;
    const amba_ik_mono5_cr_id_list_t *p_mono5_cr_id;
    uint32 window_calculate_updated = 0U;

    (void)amba_ik_system_memcpy(&p_motion_fusion_flow_tables_list, &p_flow_tbl_list, sizeof(amba_ik_motion_fusion_flow_tables_list_t*));
    p_mono5 = &p_motion_fusion_flow_tables_list->mono5;
    p_mono5_cr_id = &p_filters->update_flags.cr.mono5;

    (void)amba_ik_system_memcpy(p_flow, p_flow, sizeof(uint8));//misra fix.

    if ((p_filters->update_flags.iso.window_size_info_updated == 1U) ||
        (p_filters->update_flags.iso.cfa_window_size_info_updated == 1U) ||
        (p_filters->update_flags.iso.dmy_range_updated == 1U) ||
        (p_filters->update_flags.iso.dzoom_info_updated == 1U) ||
        (p_filters->update_flags.iso.vin_active_win_updated == 1U) ||
        (p_filters->update_flags.iso.stitching_info_updated == 1U) ||
        (p_filters->update_flags.iso.overlap_x_updated == 1U) ||
        (p_filters->update_flags.iso.flip_mode_updated == 1U) ||
        (p_filters->update_flags.iso.warp_enable_updated == 1U) ||
        (p_filters->update_flags.iso.warp_enable_2nd_updated == 1U)) {
        window_calculate_updated = 1U;
    }

    if (window_calculate_updated==1U) {;} //misra

    // default value
    if(p_filters->update_flags.iso.is_1st_frame == 1U) {
        ikc_in_mono5_default_tbl_t in_mono5_default_tbl;
        ikc_out_mono5_default_tbl_t out_mono5_default_tbl;
        out_mono5_default_tbl.p_cr_26 = p_mono5->p_CR_buf_26;
        out_mono5_default_tbl.cr_26_size = CR_SIZE_26;
        rval |= ikc_mono5_default_tbl(&in_mono5_default_tbl, &out_mono5_default_tbl);
    }

    // start exe
    // mono_cc
    if(p_filters->update_flags.iso.fusion_updated == 1U) {
        if((p_mono5_cr_id->cr_26_update == 0U)||
           (p_mono5_cr_id->cr_27_update == 0U)) {
            amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_mono5 !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
            rval |= IK_ERR_0000;
        } else {
            ikc_in_mono5_t in_mono5;
            ikc_out_mono5_t out_mono5;

            in_mono5.p_fusion = &p_filters->input_param.fusion;
            out_mono5.p_cr_26 = p_mono5->p_CR_buf_26;
            out_mono5.cr_26_size = CR_SIZE_26;
            out_mono5.p_cr_27 = p_mono5->p_CR_buf_27;
            out_mono5.cr_27_size = CR_SIZE_27;
            rval |= ikc_mono5(&in_mono5, &out_mono5);
        }
    }
    // resampler
    if((p_filters->update_flags.iso.chroma_filter_updated == 1U)||
       (p_filters->update_flags.iso.resample_str_update == 1U)||
       (p_filters->update_flags.iso.window_size_info_updated == 1U)||
       (window_calculate_updated == 1U)) {
        if((p_mono5_cr_id->cr_33_update == 0U)||
           (p_mono5_cr_id->cr_35_update == 0U)||
           (p_mono5_cr_id->cr_37_update == 0U)) {
            amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_main_resampler !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
            rval |= IK_ERR_0000;
        } else {
            ikc_in_sec2_mono5_resampler_t in_sec2_mono5_resampler;
            ikc_out_sec2_mono5_resampler_t out_sec2_mono5_resampler;

            in_sec2_mono5_resampler.main_cut_off_freq = p_filters->input_param.resample_str.main_cut_off_freq;
            in_sec2_mono5_resampler.p_act_win_crop = &p_filters->input_param.ctx_buf.result_win.act_win_crop;
            in_sec2_mono5_resampler.p_dmy_win_geo = &p_filters->input_param.ctx_buf.result_win.dmy_win_geo;
            in_sec2_mono5_resampler.p_cfa_win_dim = &p_filters->input_param.ctx_buf.result_win.cfa_win_dim;
            in_sec2_mono5_resampler.p_main = &p_filters->input_param.window_size_info.main_win;
            in_sec2_mono5_resampler.p_dmy_range = &p_filters->input_param.dmy_range;
            in_sec2_mono5_resampler.chroma_radius = p_filters->input_param.chroma_filter.radius;
            in_sec2_mono5_resampler.is_y2y_420input = (p_filters->input_param.yuv_mode == AMBA_IK_YUV420) ? 1U : 0U;
            in_sec2_mono5_resampler.flip_mode = p_filters->input_param.flip_mode;
            in_sec2_mono5_resampler.luma_dma_size = p_filters->input_param.warp_buf_info.luma_dma_size;
            in_sec2_mono5_resampler.p_burst_tile = &p_filters->input_param.burst_tile;
            out_sec2_mono5_resampler.p_cr_33 = p_mono5->p_CR_buf_33;
            out_sec2_mono5_resampler.cr_33_size = CR_SIZE_33;
            out_sec2_mono5_resampler.p_cr_35 = p_mono5->p_CR_buf_35;
            out_sec2_mono5_resampler.cr_35_size = CR_SIZE_35;
            out_sec2_mono5_resampler.p_cr_37 = p_mono5->p_CR_buf_37;
            out_sec2_mono5_resampler.cr_37_size = CR_SIZE_37;
            rval |= ikc_sec2_mono5_resampler(&in_sec2_mono5_resampler, &out_sec2_mono5_resampler);
        }
    }

    return rval;
}

uint32 exe_process_mono6_cr(idsp_flow_ctrl_t *p_flow, void *p_flow_tbl_list, amba_ik_filter_t *p_filters)
{
    uint32 rval = IK_OK;
    amba_ik_motion_fusion_flow_tables_list_t *p_motion_fusion_flow_tables_list;
    const amba_ik_mono6_flow_tables_list_t *p_mono6;
    const amba_ik_mono6_cr_id_list_t *p_mono6_cr_id;
    uint32 window_calculate_updated = 0U;

    (void)amba_ik_system_memcpy(&p_motion_fusion_flow_tables_list, &p_flow_tbl_list, sizeof(amba_ik_motion_fusion_flow_tables_list_t*));
    p_mono6 = &p_motion_fusion_flow_tables_list->mono6;
    p_mono6_cr_id = &p_filters->update_flags.cr.mono6;

    (void)amba_ik_system_memcpy(p_flow, p_flow, sizeof(uint8));//misra fix.

    if ((p_filters->update_flags.iso.window_size_info_updated == 1U) ||
        (p_filters->update_flags.iso.cfa_window_size_info_updated == 1U) ||
        (p_filters->update_flags.iso.dmy_range_updated == 1U) ||
        (p_filters->update_flags.iso.dzoom_info_updated == 1U) ||
        (p_filters->update_flags.iso.vin_active_win_updated == 1U) ||
        (p_filters->update_flags.iso.stitching_info_updated == 1U) ||
        (p_filters->update_flags.iso.overlap_x_updated == 1U) ||
        (p_filters->update_flags.iso.flip_mode_updated == 1U) ||
        (p_filters->update_flags.iso.warp_enable_updated == 1U) ||
        (p_filters->update_flags.iso.warp_enable_2nd_updated == 1U)) {
        window_calculate_updated = 1U;
    }

    if (window_calculate_updated==1U) {;} //misra

    // default value
    if(p_filters->update_flags.iso.is_1st_frame == 1U) {
        ikc_in_mono6_default_tbl_t in_mono6_default_tbl;
        ikc_out_mono6_default_tbl_t out_mono6_default_tbl;
        out_mono6_default_tbl.p_cr_26 = p_mono6->p_CR_buf_26;
        out_mono6_default_tbl.cr_26_size = CR_SIZE_26;
        rval |= ikc_mono6_default_tbl(&in_mono6_default_tbl, &out_mono6_default_tbl);
    }

    // start exe
    if((p_filters->update_flags.iso.chroma_filter_updated == 1U)||
       (p_filters->update_flags.iso.resample_str_update == 1U)||
       (p_filters->update_flags.iso.window_size_info_updated == 1U)||
       (window_calculate_updated == 1U)) {
        if((p_mono6_cr_id->cr_33_update == 0U)||
           (p_mono6_cr_id->cr_35_update == 0U)||
           (p_mono6_cr_id->cr_37_update == 0U)) {
            amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_main_resampler !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
            rval |= IK_ERR_0000;
        } else {
            ikc_in_sec2_mono6_resampler_t in_sec2_mono6_resampler;
            ikc_out_sec2_mono6_resampler_t out_sec2_mono6_resampler;

            in_sec2_mono6_resampler.main_cut_off_freq = p_filters->input_param.resample_str.main_cut_off_freq;
            in_sec2_mono6_resampler.p_act_win_crop = &p_filters->input_param.ctx_buf.result_win.act_win_crop;
            in_sec2_mono6_resampler.p_dmy_win_geo = &p_filters->input_param.ctx_buf.result_win.dmy_win_geo;
            in_sec2_mono6_resampler.p_cfa_win_dim = &p_filters->input_param.ctx_buf.result_win.cfa_win_dim;
            in_sec2_mono6_resampler.p_main = &p_filters->input_param.window_size_info.main_win;
            in_sec2_mono6_resampler.p_dmy_range = &p_filters->input_param.dmy_range;
            in_sec2_mono6_resampler.chroma_radius = p_filters->input_param.chroma_filter.radius;
            in_sec2_mono6_resampler.is_y2y_420input = (p_filters->input_param.yuv_mode == AMBA_IK_YUV420) ? 1U : 0U;
            in_sec2_mono6_resampler.flip_mode = p_filters->input_param.flip_mode;
            in_sec2_mono6_resampler.luma_dma_size = p_filters->input_param.warp_buf_info.luma_dma_size;
            in_sec2_mono6_resampler.p_burst_tile = &p_filters->input_param.burst_tile;
            out_sec2_mono6_resampler.p_cr_33 = p_mono6->p_CR_buf_33;
            out_sec2_mono6_resampler.cr_33_size = CR_SIZE_33;
            out_sec2_mono6_resampler.p_cr_35 = p_mono6->p_CR_buf_35;
            out_sec2_mono6_resampler.cr_35_size = CR_SIZE_35;
            out_sec2_mono6_resampler.p_cr_37 = p_mono6->p_CR_buf_37;
            out_sec2_mono6_resampler.cr_37_size = CR_SIZE_37;
            rval |= ikc_sec2_mono6_resampler(&in_sec2_mono6_resampler, &out_sec2_mono6_resampler);
        }
    }

    return rval;
}

uint32 exe_process_mono7_cr(idsp_flow_ctrl_t *p_flow, void *p_flow_tbl_list, amba_ik_filter_t *p_filters)
{
    uint32 rval = IK_OK;
    amba_ik_motion_fusion_flow_tables_list_t *p_motion_fusion_flow_tables_list;
    const amba_ik_mono7_flow_tables_list_t *p_mono7;
    const amba_ik_mono7_cr_id_list_t *p_mono7_cr_id;
    uint32 window_calculate_updated = 0U;

    (void)amba_ik_system_memcpy(&p_motion_fusion_flow_tables_list, &p_flow_tbl_list, sizeof(amba_ik_motion_fusion_flow_tables_list_t*));
    p_mono7 = &p_motion_fusion_flow_tables_list->mono7;
    p_mono7_cr_id = &p_filters->update_flags.cr.mono7;

    (void)amba_ik_system_memcpy(p_flow, p_flow, sizeof(uint8));//misra fix.

    if ((p_filters->update_flags.iso.window_size_info_updated == 1U) ||
        (p_filters->update_flags.iso.cfa_window_size_info_updated == 1U) ||
        (p_filters->update_flags.iso.dmy_range_updated == 1U) ||
        (p_filters->update_flags.iso.dzoom_info_updated == 1U) ||
        (p_filters->update_flags.iso.vin_active_win_updated == 1U) ||
        (p_filters->update_flags.iso.stitching_info_updated == 1U) ||
        (p_filters->update_flags.iso.overlap_x_updated == 1U) ||
        (p_filters->update_flags.iso.flip_mode_updated == 1U) ||
        (p_filters->update_flags.iso.warp_enable_updated == 1U) ||
        (p_filters->update_flags.iso.warp_enable_2nd_updated == 1U)) {
        window_calculate_updated = 1U;
    }

    if (window_calculate_updated==1U) {;} //misra

    // default value
    if(p_filters->update_flags.iso.is_1st_frame == 1U) {
        ikc_in_mono7_default_tbl_t in_mono7_default_tbl;
        ikc_out_mono7_default_tbl_t out_mono7_default_tbl;
        out_mono7_default_tbl.p_cr_26 = p_mono7->p_CR_buf_26;
        out_mono7_default_tbl.cr_26_size = CR_SIZE_26;
        rval |= ikc_mono7_default_tbl(&in_mono7_default_tbl, &out_mono7_default_tbl);
    }

    // start exe
    // mono_cc
    if(p_filters->update_flags.iso.fusion_updated == 1U) {
        if((p_mono7_cr_id->cr_26_update == 0U)||
           (p_mono7_cr_id->cr_27_update == 0U)) {
            amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_mono7 !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
            rval |= IK_ERR_0000;
        } else {
            ikc_in_mono7_t in_mono7;
            ikc_out_mono7_t out_mono7;

            in_mono7.p_fusion = &p_filters->input_param.fusion;
            out_mono7.p_cr_26 = p_mono7->p_CR_buf_26;
            out_mono7.cr_26_size = CR_SIZE_26;
            out_mono7.p_cr_27 = p_mono7->p_CR_buf_27;
            out_mono7.cr_27_size = CR_SIZE_27;
            rval |= ikc_mono7(&in_mono7, &out_mono7);
        }
    }
    // resampler
    if((p_filters->update_flags.iso.chroma_filter_updated == 1U)||
       (p_filters->update_flags.iso.resample_str_update == 1U)||
       (p_filters->update_flags.iso.window_size_info_updated == 1U)||
       (window_calculate_updated == 1U)) {
        if((p_mono7_cr_id->cr_33_update == 0U)||
           (p_mono7_cr_id->cr_35_update == 0U)||
           (p_mono7_cr_id->cr_37_update == 0U)) {
            amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_main_resampler !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
            rval |= IK_ERR_0000;
        } else {
            ikc_in_sec2_mono7_resampler_t in_sec2_mono7_resampler;
            ikc_out_sec2_mono7_resampler_t out_sec2_mono7_resampler;

            in_sec2_mono7_resampler.main_cut_off_freq = p_filters->input_param.resample_str.main_cut_off_freq;
            in_sec2_mono7_resampler.p_act_win_crop = &p_filters->input_param.ctx_buf.result_win.act_win_crop;
            in_sec2_mono7_resampler.p_dmy_win_geo = &p_filters->input_param.ctx_buf.result_win.dmy_win_geo;
            in_sec2_mono7_resampler.p_cfa_win_dim = &p_filters->input_param.ctx_buf.result_win.cfa_win_dim;
            in_sec2_mono7_resampler.p_main = &p_filters->input_param.window_size_info.main_win;
            in_sec2_mono7_resampler.p_dmy_range = &p_filters->input_param.dmy_range;
            in_sec2_mono7_resampler.chroma_radius = p_filters->input_param.chroma_filter.radius;
            in_sec2_mono7_resampler.is_y2y_420input = (p_filters->input_param.yuv_mode == AMBA_IK_YUV420) ? 1U : 0U;
            in_sec2_mono7_resampler.flip_mode = p_filters->input_param.flip_mode;
            in_sec2_mono7_resampler.luma_dma_size = p_filters->input_param.warp_buf_info.luma_dma_size;
            in_sec2_mono7_resampler.p_burst_tile = &p_filters->input_param.burst_tile;
            out_sec2_mono7_resampler.p_cr_33 = p_mono7->p_CR_buf_33;
            out_sec2_mono7_resampler.cr_33_size = CR_SIZE_33;
            out_sec2_mono7_resampler.p_cr_35 = p_mono7->p_CR_buf_35;
            out_sec2_mono7_resampler.cr_35_size = CR_SIZE_35;
            out_sec2_mono7_resampler.p_cr_37 = p_mono7->p_CR_buf_37;
            out_sec2_mono7_resampler.cr_37_size = CR_SIZE_37;
            rval |= ikc_sec2_mono7_resampler(&in_sec2_mono7_resampler, &out_sec2_mono7_resampler);
        }
    }

    return rval;
}
#endif
uint32 exe_query_calc_geo_buf_size(ik_query_calc_geo_buf_size_t *p_buf_size_info)
{
    uint32 rval = IK_OK;

    if (p_buf_size_info == NULL) {
        rval = IK_ERR_0005;
        amba_ik_system_print_str_5("[IK][ERROR] p_buf_size_info == NULL\n", DC_S, DC_S, DC_S, DC_S, DC_S);
    } else {
        p_buf_size_info->work_buf_size = (uint32)sizeof(amba_ik_calc_geo_work_buf_t);
        p_buf_size_info->out_buf_size = (uint32)sizeof(amalgam_group_update_data_t);
    }

    return rval;
}

static uint32 exe_calc_geo_settings_basic_check(uint32 is_hvh_pipeline, const ik_calc_geo_settings_t *p_info, const ik_calc_geo_result_t *p_result)
{
    uint32 rval = IK_OK;
    uint32 misra_u32;

    if (p_info->p_in->p_window_size_info == NULL) {
        rval = IK_ERR_0005;
        amba_ik_system_print_str_5("[IK][ERROR] p_info->p_in->p_window_size_info == NULL\n", DC_S, DC_S, DC_S, DC_S, DC_S);
    } else {
        if ((p_info->p_in->p_window_size_info->vin_sensor.width == 0U) || (p_info->p_in->p_window_size_info->vin_sensor.height == 0U) ||
            (p_info->p_in->p_window_size_info->vin_sensor.h_sub_sample.factor_num == 0U) || (p_info->p_in->p_window_size_info->vin_sensor.h_sub_sample.factor_den == 0U) ||
            (p_info->p_in->p_window_size_info->vin_sensor.v_sub_sample.factor_num == 0U) || (p_info->p_in->p_window_size_info->vin_sensor.v_sub_sample.factor_den == 0U) ||
            (p_info->p_in->p_window_size_info->main_win.width == 0U) || (p_info->p_in->p_window_size_info->main_win.height == 0U)) {
            rval = IK_ERR_0008;
            amba_ik_system_print_uint32_5("[IK][ERROR] vin w %d h %d main_win w %d h %d \n",
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

        if(is_hvh_pipeline != 0U) {
            if (p_info->p_in->p_calib_warp_info_2nd == NULL) {
                rval = IK_ERR_0005;
                amba_ik_system_print_int32_5("[IK][ERROR] hvh pipeline, warp_enable %d but p_calib_warp_info_2nd is NULL\n", p_info->p_in->warp_enable, DC_I, DC_I, DC_I, DC_I);
            } else {
                if (p_info->p_in->p_calib_warp_info_2nd->pwarp == NULL) {
                    rval = IK_ERR_0005;
                    amba_ik_system_print_int32_5("[IK][ERROR] hvh pipeline, warp_enable %d but p_calib_warp_info_2nd->pwarp is NULL\n", p_info->p_in->warp_enable, DC_I, DC_I, DC_I, DC_I);
                }
            }
        }

    }
    if ((p_info->p_in->use_cawarp == 1) && (p_info->p_in->cawarp_enable == 1)) {
        if (p_info->p_in->p_calib_ca_warp_info == NULL) {
            rval = IK_ERR_0005;
            amba_ik_system_print_int32_5("[IK][ERROR] cawarp_enable %d but p_calib_ca_warp_info is NULL\n", p_info->p_in->cawarp_enable, DC_I, DC_I, DC_I, DC_I);
        } else {
            if ((p_info->p_in->p_calib_ca_warp_info->p_cawarp_red == NULL) || (p_info->p_in->p_calib_ca_warp_info->p_cawarp_blue == NULL)) {
                rval = IK_ERR_0005;
                amba_ik_system_print_int32_5("[IK][ERROR] cawarp_enable %d but p_calib_ca_warp_info->p_cawarp_red or p_calib_ca_warp_info->p_cawarp_blue is NULL\n",
                                             p_info->p_in->cawarp_enable, DC_I, DC_I, DC_I, DC_I);
            }
        }
    }

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

    return rval;
}

static inline uint32 exe_calc_geo_settings_check(const ik_ability_t *p_ability, const ik_calc_geo_settings_t *p_info, const ik_calc_geo_result_t *p_result, const ik_query_calc_geo_buf_size_t *p_exptected_buf_size)
{
    uint32 rval = IK_OK;
    uint32 is_hvh_pipeline;

    if ((p_ability->video_pipe == AMBA_IK_VIDEO_PIPE_LINEAR_HVH)||
        (p_ability->video_pipe == AMBA_IK_VIDEO_PIPE_LINEAR_CE_HVH) ||
        (p_ability->video_pipe == AMBA_IK_VIDEO_PIPE_HDR_EXPO_2_HVH) ||
        (p_ability->video_pipe == AMBA_IK_VIDEO_PIPE_HDR_EXPO_3_HVH) ||
        (p_ability->video_pipe == AMBA_IK_VIDEO_PIPE_LINEAR_HHB) ||
        (p_ability->video_pipe == AMBA_IK_VIDEO_PIPE_LINEAR_CE_HHB) ||
        (p_ability->video_pipe == AMBA_IK_VIDEO_PIPE_HDR_EXPO_2_HHB) ||
        (p_ability->video_pipe == AMBA_IK_VIDEO_PIPE_HDR_EXPO_3_HHB) ||
        (p_ability->video_pipe == AMBA_IK_VIDEO_PIPE_LINEAR_MD_HHB) ||
        (p_ability->video_pipe == AMBA_IK_VIDEO_PIPE_LINEAR_CE_MD_HHB) ||
        (p_ability->video_pipe == AMBA_IK_VIDEO_PIPE_HDR_EXPO_2_MD_HHB) ||
        (p_ability->video_pipe == AMBA_IK_VIDEO_PIPE_HDR_EXPO_3_MD_HHB) ||
        (p_ability->video_pipe == AMBA_IK_VIDEO_PIPE_Y2Y_HVH)||
        (p_ability->video_pipe == AMBA_IK_VIDEO_PIPE_Y2Y_HHB)||
        (p_ability->video_pipe == AMBA_IK_VIDEO_PIPE_Y2Y_MD_HHB)) {
        is_hvh_pipeline = 1u;
    } else {
        is_hvh_pipeline = 0;
    }

    if ((p_info == NULL)||(p_result == NULL)) {
        rval = IK_ERR_0005;
        amba_ik_system_print_str_5("[IK][ERROR] p_info or p_result == NULL\n", DC_S, DC_S, DC_S, DC_S, DC_S);
    } else {

        if ((p_info->p_in == NULL) || (p_info->p_work_buf == NULL) || (p_result->p_out == NULL)) {
            rval = IK_ERR_0005;
            amba_ik_system_print_uint32_5("[IK][ERROR] p_info->p_in or p_info->p_work_buf or p_info->p_out is NULL\n", DC_U, DC_U, DC_U, DC_U, DC_U);
        } else {
            if (p_ability->pipe != AMBA_IK_VIDEO_PIPE) {
                amba_ik_system_print_uint32_5("[IK] Error, pipe %d is not support now\n", p_ability->pipe, DC_U, DC_U, DC_U, DC_U);
                rval = IK_ERR_0008;
            } else {
                rval |= exe_calc_geo_settings_basic_check(is_hvh_pipeline, p_info, p_result);
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
        amba_ik_calc_geo_work_buf_t *p_work_buf,
        ik_vin_active_window_t *p_act_win,
        ik_dummy_margin_range_t *p_dmy_range,
        ik_dzoom_info_t *p_dz_info,
        ik_stitch_info_t *p_stitch_info,
        ik_overlap_x_t *p_overlap_x,
        ik_hdr_eis_info_t *p_hdr_eis_sft_exp1,
        ik_hdr_eis_info_t *p_hdr_eis_sft_exp2,
        ik_cfa_window_size_info_t *p_cfa_window_size_info)
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
    if(p_info->p_in->p_stitch_info!= NULL) {
        p_win_info->p_stitching_info = p_info->p_in->p_stitch_info;
    } else {
        p_win_info->p_stitching_info = p_stitch_info;
    }
    p_win_info->p_overlap_x = p_overlap_x;
    p_win_info->p_cfa_window_size_info = p_cfa_window_size_info;
    p_win_info->p_hdr_eis_sft_exp1 = p_hdr_eis_sft_exp1;
    p_win_info->p_hdr_eis_sft_exp2 = p_hdr_eis_sft_exp2;

    p_win_info->p_result_win = &p_work_buf->result_win;
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
    if(p_info->p_in->p_sensor_info != NULL) {
        p_in_aaa->p_sensor_info = p_info->p_in->p_sensor_info;//determine RGB-IR.
    }
    if(p_info->p_in->p_rgb_ir != NULL) {
        p_in_aaa->p_rgb_ir = p_info->p_in->p_rgb_ir;
    }
    if(p_info->p_in->p_aaa_stat_info != NULL) {
        p_in_aaa->p_aaa_stat_info = p_info->p_in->p_aaa_stat_info;
    }
    if(p_info->p_in->p_aaa_pg_af_stat_info != NULL) {
        p_in_aaa->p_aaa_pg_af_stat_info = p_info->p_in->p_aaa_pg_af_stat_info;
    }
    if(p_info->p_in->p_af_stat_ex_info != NULL) {
        p_in_aaa->p_af_stat_ex_info= p_info->p_in->p_af_stat_ex_info;
    }
    if(p_info->p_in->p_pg_af_stat_ex_info != NULL) {
        p_in_aaa->p_pg_af_stat_ex_info = p_info->p_in->p_pg_af_stat_ex_info;
    }
    if(p_info->p_in->p_hist_info != NULL) {
        p_in_aaa->p_hist_info = p_info->p_in->p_hist_info;
    }
    if(p_info->p_in->p_hist_info_pg != NULL) {
        p_in_aaa->p_hist_info_pg = p_info->p_in->p_hist_info_pg;
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
    p_out_aaa->p_stitching_aaa = p_out->aaa[0];

    if ((p_ability->video_pipe != AMBA_IK_VIDEO_PIPE_Y2Y) &&
        (p_ability->video_pipe != AMBA_IK_VIDEO_PIPE_Y2Y_HVH) &&
        (p_ability->video_pipe != AMBA_IK_VIDEO_PIPE_Y2Y_HHB) &&
        (p_ability->video_pipe != AMBA_IK_VIDEO_PIPE_Y2Y_MD) &&
        (p_ability->video_pipe != AMBA_IK_VIDEO_PIPE_Y2Y_MD_HHB)) {
        p_in_aaa->is_yuv_mode = 0U;
    } else {
        p_in_aaa->is_yuv_mode = 1U;
    }
}

uint32 exe_calc_geo_settings(const ik_ability_t *p_ability, const ik_calc_geo_settings_t *p_info, const ik_calc_geo_result_t *p_result)
{
    uint32 rval = IK_OK;
    ik_query_calc_geo_buf_size_t exptected_buf_size;
    amba_ik_win_calc_t win_info = {0};
    ik_stitch_info_t stitching_info = {0};
    ik_warp_buffer_info_t warp_buf_info = {0};
    amba_ik_calc_geo_work_buf_t *p_work_buf = NULL;
    amalgam_group_update_data_t *p_out = NULL;
    void *p_work_cr = NULL;
    uint32 data_offset_base = 0;
    ik_vin_active_window_t act_win = {0};
    ik_cfa_window_size_info_t cfa_window_size_info = {0};
    ik_dummy_margin_range_t dmy_range = {0};
    ik_dzoom_info_t dz_info = {0};
    ik_stitch_info_t stitch_info = {0};
    ik_overlap_x_t overlap_x = {0};
    ik_hdr_eis_info_t hdr_eis_sft_exp1 = {0};
    ik_hdr_eis_info_t hdr_eis_sft_exp2 = {0};

    (void)amba_ik_system_memset(&exptected_buf_size, 0, sizeof(ik_query_calc_geo_buf_size_t));//misra

    (void)exe_query_calc_geo_buf_size(&exptected_buf_size);
    rval |= exe_calc_geo_settings_check(p_ability, p_info, p_result, &exptected_buf_size);

    /* Parameters Check */
    if (rval == IK_OK) {
        /* Do things */
        (void)amba_ik_system_memcpy(&p_work_buf, &p_info->p_work_buf, sizeof(void *));
        (void)amba_ik_system_memcpy(&p_out, &p_result->p_out, sizeof(void *));
        p_work_cr = ptr2ptr(p_work_buf->CR_buf_101);
        (void)amba_ik_system_memcpy(&data_offset_base, &p_result->p_out, sizeof(uint32));

        exe_calc_geo_settings_set_win_info(p_ability, p_info, &win_info, p_work_buf, &act_win, &dmy_range, &dz_info, &stitch_info, &overlap_x, &hdr_eis_sft_exp1, &hdr_eis_sft_exp2, &cfa_window_size_info);
        rval |= exe_win_calc(&win_info);

        if (rval == IK_OK) {
            {
                // default value
                warp_buf_info.luma_wait_lines = 8u;
                warp_buf_info.luma_dma_size = 32u;
                if(p_info->p_in->p_dmy_range != NULL) {
                    (void)amba_ik_system_memcpy(&dmy_range, p_info->p_in->p_dmy_range, sizeof(ik_dummy_margin_range_t));
                } else {
                    dmy_range.enable = 0;
                }
                if (p_info->p_in->p_stitch_info!=NULL) {
                    (void)amba_ik_system_memcpy(&stitching_info, p_info->p_in->p_stitch_info, sizeof(ik_stitch_info_t));
                } else {
                    stitching_info.enable = 0;
                }
            }
            if ((p_ability->video_pipe != AMBA_IK_VIDEO_PIPE_Y2Y) &&
                (p_ability->video_pipe != AMBA_IK_VIDEO_PIPE_Y2Y_HVH) &&
                (p_ability->video_pipe != AMBA_IK_VIDEO_PIPE_Y2Y_HHB) &&
                (p_ability->video_pipe != AMBA_IK_VIDEO_PIPE_Y2Y_MD) &&
                (p_ability->video_pipe != AMBA_IK_VIDEO_PIPE_Y2Y_MD_HHB)) {
                // ca
                if (p_info->p_in->use_cawarp == 1) {
                    ikc_in_cawarp_t in_ca;
                    ikc_out_cawarp_t out_ca;

                    in_ca.is_group_cmd = 1u;
                    in_ca.is_hiso      = 0u;
                    in_ca.cawarp_enable = (uint32)p_info->p_in->cawarp_enable;
                    in_ca.flip_mode = p_info->p_in->flip_mode;
                    in_ca.p_stitching_info = &stitching_info;
                    in_ca.p_window_size_info = p_info->p_in->p_window_size_info;
                    in_ca.p_result_win = &p_work_buf->result_win;
                    if(p_info->p_in->cawarp_enable == 1) {
                        in_ca.p_calib_ca_warp_info = p_info->p_in->p_calib_ca_warp_info;
                    }

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
                }
                p_out->group_update_info.calib.update_CA_warp = 1U;
            } else {
                p_out->group_update_info.calib.update_CA_warp = 0U;
            }
            {
                ikc_in_hwarp_t in_hwarp;
                ikc_out_hwarp_t out_hwarp;

                in_hwarp.is_group_cmd = 1u;
                in_hwarp.is_hiso      = 0u;
                in_hwarp.warp_enable = (uint32)p_info->p_in->warp_enable;
                in_hwarp.flip_mode = p_info->p_in->flip_mode;
                in_hwarp.p_window_size_info = p_info->p_in->p_window_size_info;
                in_hwarp.p_act_win_crop = &p_work_buf->result_win.act_win_crop;
                in_hwarp.p_dmy_win_geo = &p_work_buf->result_win.dmy_win_geo;
                in_hwarp.p_cfa_win_dim = &p_work_buf->result_win.cfa_win_dim;
                in_hwarp.p_dmy_range = &dmy_range;
                in_hwarp.p_stitching_info = &stitching_info;
                in_hwarp.chroma_radius = p_info->p_in->chroma_filter_radius;
                if ((p_ability->video_pipe == AMBA_IK_VIDEO_PIPE_Y2Y) ||
                    (p_ability->video_pipe == AMBA_IK_VIDEO_PIPE_Y2Y_HVH) ||
                    (p_ability->video_pipe == AMBA_IK_VIDEO_PIPE_Y2Y_HHB) ||
                    (p_ability->video_pipe == AMBA_IK_VIDEO_PIPE_Y2Y_MD) ||
                    (p_ability->video_pipe == AMBA_IK_VIDEO_PIPE_Y2Y_MD_HHB)) {
                    in_hwarp.is_y2y_420input = (p_info->p_in->yuv_mode == AMBA_IK_YUV420) ? 1U : 0U;
                } else {
                    in_hwarp.is_y2y_420input = 0;
                }
                if(p_info->p_in->warp_enable == 1) {
                    in_hwarp.p_calib_warp_info = p_info->p_in->p_calib_warp_info;
                }
                in_hwarp.dram_efficiency = warp_buf_info.dram_efficiency;
                in_hwarp.luma_wait_lines = warp_buf_info.luma_wait_lines;
                in_hwarp.luma_dma_size = warp_buf_info.luma_dma_size;
                in_hwarp.is_still_422 = 0;

                out_hwarp.p_cr_33 = p_work_cr;
                out_hwarp.cr_33_size = CR_SIZE_33;
                out_hwarp.p_cr_34 = p_work_cr;
                out_hwarp.cr_34_size = CR_SIZE_34;
                out_hwarp.p_cr_35 = p_work_cr;
                out_hwarp.cr_35_size = CR_SIZE_35;
                out_hwarp.p_flow_info = &p_work_buf->flow_info;
                out_hwarp.p_window = &p_out->group_update_info.window;
                out_hwarp.p_phase = &p_out->group_update_info.phase;
                out_hwarp.p_calib = &p_out->group_update_info.calib;
                out_hwarp.p_stitch = &p_out->group_update_info.stitch;
                out_hwarp.p_warp_horizontal_table_address = ptr2ptr(p_out->warp_hor);
                out_hwarp.h_size = WARP_VIDEO_MAXHORGRID * WARP_VIDEO_MAXVERGRID * 2U * 2U;// multi_slice buffer.

                // I suppose user calib table already remapped by comsvc.
                rval |= ikc_hwarp(&in_hwarp, &out_hwarp);
            }
            {
                ikc_in_vwarp_t in_vwarp;
                ikc_out_vwarp_t out_vwarp;
                ik_burst_tile_t burst_tile= {0};
                if ((p_ability->video_pipe == AMBA_IK_VIDEO_PIPE_LINEAR_HVH)||
                    (p_ability->video_pipe == AMBA_IK_VIDEO_PIPE_LINEAR_CE_HVH) ||
                    (p_ability->video_pipe == AMBA_IK_VIDEO_PIPE_HDR_EXPO_2_HVH) ||
                    (p_ability->video_pipe == AMBA_IK_VIDEO_PIPE_HDR_EXPO_3_HVH) ||
                    (p_ability->video_pipe == AMBA_IK_VIDEO_PIPE_Y2Y_HVH)) {
                    in_vwarp.is_hvh_enabled = 1u;
                    in_vwarp.is_hhb_enabled = 0u;
                } else if ((p_ability->video_pipe == AMBA_IK_VIDEO_PIPE_LINEAR_HHB)||
                           (p_ability->video_pipe == AMBA_IK_VIDEO_PIPE_LINEAR_CE_HHB) ||
                           (p_ability->video_pipe == AMBA_IK_VIDEO_PIPE_HDR_EXPO_2_HHB) ||
                           (p_ability->video_pipe == AMBA_IK_VIDEO_PIPE_HDR_EXPO_3_HHB) ||
                           (p_ability->video_pipe == AMBA_IK_VIDEO_PIPE_LINEAR_MD_HHB) ||
                           (p_ability->video_pipe == AMBA_IK_VIDEO_PIPE_LINEAR_CE_MD_HHB) ||
                           (p_ability->video_pipe == AMBA_IK_VIDEO_PIPE_HDR_EXPO_2_MD_HHB) ||
                           (p_ability->video_pipe == AMBA_IK_VIDEO_PIPE_HDR_EXPO_3_MD_HHB) ||
                           (p_ability->video_pipe == AMBA_IK_VIDEO_PIPE_Y2Y_HHB) ||
                           (p_ability->video_pipe == AMBA_IK_VIDEO_PIPE_Y2Y_MD_HHB)) {
                    in_vwarp.is_hvh_enabled = 0u;
                    in_vwarp.is_hhb_enabled = 1u;
                } else {
                    in_vwarp.is_hvh_enabled = 0u;
                    in_vwarp.is_hhb_enabled = 0u;
                }
                in_vwarp.is_multi_pass = 0u;
                in_vwarp.is_group_cmd = 1u;
                in_vwarp.warp_enable = (uint32)p_info->p_in->warp_enable;
                in_vwarp.flip_mode = p_info->p_in->flip_mode;
                in_vwarp.p_window_size_info = p_info->p_in->p_window_size_info;
                in_vwarp.p_act_win_crop = &p_work_buf->result_win.act_win_crop;
                in_vwarp.p_dmy_win_geo = &p_work_buf->result_win.dmy_win_geo;
                in_vwarp.p_cfa_win_dim = &p_work_buf->result_win.cfa_win_dim;
                in_vwarp.p_dmy_range = &dmy_range;
                in_vwarp.p_stitching_info = &stitching_info;
                in_vwarp.chroma_radius = p_info->p_in->chroma_filter_radius;
                if ((p_ability->video_pipe == AMBA_IK_VIDEO_PIPE_Y2Y) ||
                    (p_ability->video_pipe == AMBA_IK_VIDEO_PIPE_Y2Y_HVH) ||
                    (p_ability->video_pipe == AMBA_IK_VIDEO_PIPE_Y2Y_HHB) ||
                    (p_ability->video_pipe == AMBA_IK_VIDEO_PIPE_Y2Y_MD) ||
                    (p_ability->video_pipe == AMBA_IK_VIDEO_PIPE_Y2Y_MD_HHB)) {
                    in_vwarp.is_y2y_420input = (p_info->p_in->yuv_mode == AMBA_IK_YUV420) ? 1U : 0U;
                } else {
                    in_vwarp.is_y2y_420input = 0;
                }
                if(p_info->p_in->warp_enable == 1) {
                    in_vwarp.p_calib_warp_info = p_info->p_in->p_calib_warp_info;
                }
                in_vwarp.dram_efficiency = warp_buf_info.dram_efficiency;
                in_vwarp.luma_wait_lines = warp_buf_info.luma_wait_lines;
                in_vwarp.luma_dma_size = warp_buf_info.luma_dma_size;
                in_vwarp.p_burst_tile = &burst_tile;
                out_vwarp.p_cr_42 = p_work_cr;
                out_vwarp.cr_42_size = CR_SIZE_42;
                out_vwarp.p_cr_43 = p_work_cr;
                out_vwarp.cr_43_size = CR_SIZE_43;
                out_vwarp.p_cr_44 = p_work_cr;
                out_vwarp.cr_44_size = CR_SIZE_44;
                out_vwarp.p_window = &p_out->group_update_info.window;
                out_vwarp.p_calib = &p_out->group_update_info.calib;
                out_vwarp.p_warp_vertical_table_address = ptr2ptr(p_out->warp_ver);
                out_vwarp.v_size = WARP_VIDEO_MAXHORGRID * WARP_VIDEO_MAXVERGRID * 2U * 2U;// multi_slice buffer.

                rval |= ikc_vwarp(&in_vwarp, &out_vwarp);
            }
            {
                ikc_in_2nd_hwarp_t in_hwarp_2nd;
                ikc_out_2nd_hwarp_t out_hwarp_2nd;
                ik_warp_info_t fake_calib_info;

                if ((p_ability->video_pipe == AMBA_IK_VIDEO_PIPE_LINEAR_HVH) ||
                    (p_ability->video_pipe == AMBA_IK_VIDEO_PIPE_LINEAR_CE_HVH) ||
                    (p_ability->video_pipe == AMBA_IK_VIDEO_PIPE_HDR_EXPO_2_HVH) ||
                    (p_ability->video_pipe == AMBA_IK_VIDEO_PIPE_HDR_EXPO_3_HVH) ||
                    (p_ability->video_pipe == AMBA_IK_VIDEO_PIPE_Y2Y_HVH)) {
                    in_hwarp_2nd.is_hvh_enabled = 1U;
                    in_hwarp_2nd.is_hhb_enabled = 0U;
                } else if ((p_ability->video_pipe == AMBA_IK_VIDEO_PIPE_LINEAR_HHB) ||
                           (p_ability->video_pipe == AMBA_IK_VIDEO_PIPE_LINEAR_CE_HHB) ||
                           (p_ability->video_pipe == AMBA_IK_VIDEO_PIPE_HDR_EXPO_2_HHB) ||
                           (p_ability->video_pipe == AMBA_IK_VIDEO_PIPE_HDR_EXPO_3_HHB) ||
                           (p_ability->video_pipe == AMBA_IK_VIDEO_PIPE_LINEAR_MD_HHB) ||
                           (p_ability->video_pipe == AMBA_IK_VIDEO_PIPE_LINEAR_CE_MD_HHB) ||
                           (p_ability->video_pipe == AMBA_IK_VIDEO_PIPE_HDR_EXPO_2_MD_HHB) ||
                           (p_ability->video_pipe == AMBA_IK_VIDEO_PIPE_HDR_EXPO_3_MD_HHB) ||
                           (p_ability->video_pipe == AMBA_IK_VIDEO_PIPE_Y2Y_HHB) ||
                           (p_ability->video_pipe == AMBA_IK_VIDEO_PIPE_Y2Y_MD_HHB)) {
                    in_hwarp_2nd.is_hvh_enabled = 0U;
                    in_hwarp_2nd.is_hhb_enabled = 1U;
                } else {
                    in_hwarp_2nd.is_hvh_enabled = 0U;
                    in_hwarp_2nd.is_hhb_enabled = 0U;
                }

                (void)amba_ik_system_memcpy(&fake_calib_info, p_info->p_in->p_calib_warp_info, sizeof(ik_warp_info_t));
                fake_calib_info.pwarp = NULL;
                in_hwarp_2nd.is_group_cmd = 1u;
                in_hwarp_2nd.chroma_radius = p_info->p_in->chroma_filter_radius;
                if ((in_hwarp_2nd.is_hvh_enabled==1u) || (in_hwarp_2nd.is_hhb_enabled==1u)) {
                    in_hwarp_2nd.warp_enable = (uint32)p_info->p_in->warp_2nd_enable;
                    in_hwarp_2nd.p_calib_warp_info = p_info->p_in->p_calib_warp_info_2nd;
                } else {
                    in_hwarp_2nd.warp_enable = 0;
                    in_hwarp_2nd.p_calib_warp_info = &fake_calib_info;
                }
                in_hwarp_2nd.flip_mode = p_info->p_in->flip_mode;
                {
                    in_hwarp_2nd.p_window_size_info = p_info->p_in->p_window_size_info;
                    in_hwarp_2nd.p_act_win_crop = &p_work_buf->result_win.act_win_crop;
                    in_hwarp_2nd.p_dmy_win_geo = &p_work_buf->result_win.dmy_win_geo;
                    in_hwarp_2nd.p_cfa_win_dim = &p_work_buf->result_win.cfa_win_dim;
                    in_hwarp_2nd.p_stitching_info = &stitching_info;
                }

                out_hwarp_2nd.p_cr_100 = p_work_cr;
                out_hwarp_2nd.cr_100_size = CR_SIZE_100;
                out_hwarp_2nd.p_cr_101 = p_work_cr;
                out_hwarp_2nd.cr_101_size = CR_SIZE_101;
                out_hwarp_2nd.p_flow_info = &p_work_buf->flow_info;
                out_hwarp_2nd.p_phase = &p_out->group_update_info.phase;
                out_hwarp_2nd.p_calib = &p_out->group_update_info.calib;
                out_hwarp_2nd.p_warp_horizontal_b_table_address = ptr2ptr(p_out->warp_hor_b);
                out_hwarp_2nd.h_size = WARP_VIDEO_MAXHORGRID * WARP_VIDEO_MAXVERGRID * 2U * 2U;// multi_slice buffer.

                rval |= ikc_secondary_hwarp(&in_hwarp_2nd, &out_hwarp_2nd);
            }
            {
                ikc_in_chroma_noise_t in_chroma_noise = {0};
                ikc_out_chroma_noise_t out_chroma_noise = {0};

                ik_chroma_filter_t chroma_filter = {0};
                ik_wide_chroma_filter_t wide_chroma_filter = {0};
                ik_wide_chroma_filter_combine_t wide_chroma_combine = {0};
                chroma_filter.radius = p_info->p_in->chroma_filter_radius;

                in_chroma_noise.p_chroma_filter = &chroma_filter;
                in_chroma_noise.p_wide_chroma_filter = &wide_chroma_filter;
                in_chroma_noise.p_wide_chroma_combine = &wide_chroma_combine;
                if ((in_chroma_noise.p_chroma_filter->radius==32u) && (in_chroma_noise.p_wide_chroma_filter->enable==1u)) {
                    in_chroma_noise.is_wide_chroma_invalid = 1u;
                } else {
                    in_chroma_noise.is_wide_chroma_invalid = 0u;
                }
                out_chroma_noise.p_cr_36 = p_work_cr;
                out_chroma_noise.cr_36_size = CR_SIZE_36;
                out_chroma_noise.p_window = &p_out->group_update_info.window;

                rval |= ikc_chroma_noise(&in_chroma_noise, &out_chroma_noise);
            }
            {
                // aaa_cfa
                ikc_in_aaa_t in_aaa;
                ikc_out_aaa_t out_aaa;

                exe_calc_geo_settings_set_aaa_info(p_ability, p_info, &in_aaa, &out_aaa, &stitching_info, p_work_buf, p_out, p_work_cr);
                rval |= ikc_aaa(&in_aaa, &out_aaa);
            }
            {
                // value assign
                p_out->group_update_info.calib.update_lens_warp = 1U;
                p_out->group_update_info.calib.update_lens_warp_b = 1U;
                p_out->group_update_info.calib_update = 1U;
                p_out->group_update_info.phase.prescale_update = 1U;
                p_out->group_update_info.phase.main_update = 1U;
                p_out->group_update_info.phase.vwarp_update = 1U;
                p_out->group_update_info.phase.hwarp_b_update = 1U;
                p_out->group_update_info.phase.pos_dep_update = 1U;
                p_out->group_update_info.window.update_chroma_radius = 1U;
                p_out->group_update_info.window.use_hdr_dummy_crop_window = 0U;
                p_out->group_update_info.window_info_update = 1U;
                p_out->group_update_info.phase_info_update = 1U;
                p_out->group_update_info.stitch_data_update = 1U;
                p_out->group_update_info.aaa_stats_update = 1U;

                p_out->group_update_info.calib.cawarp_horizontal_table_addr_red = physical2u32(amba_ik_system_virt_to_phys(p_out->ca_warp_hor_red));
                p_out->group_update_info.calib.cawarp_horizontal_table_addr_blue = physical2u32(amba_ik_system_virt_to_phys(p_out->ca_warp_hor_blue));
                p_out->group_update_info.calib.cawarp_vertical_table_addr_red = physical2u32(amba_ik_system_virt_to_phys(p_out->ca_warp_ver_red));
                p_out->group_update_info.calib.cawarp_vertical_table_addr_blue = physical2u32(amba_ik_system_virt_to_phys(p_out->ca_warp_ver_blue));
                p_out->group_update_info.calib.warp_horizontal_table_address = physical2u32(amba_ik_system_virt_to_phys(p_out->warp_hor));
                p_out->group_update_info.calib.warp_vertical_table_address = physical2u32(amba_ik_system_virt_to_phys(p_out->warp_ver));
                p_out->group_update_info.calib.warp_horizontal_b_table_address = physical2u32(amba_ik_system_virt_to_phys(p_out->warp_hor_b));
                p_out->group_update_info.stitch.aaa_info_daddr = physical2u32(amba_ik_system_virt_to_phys(p_out->aaa[0]));
                p_out->group_update_info.window.extra_window_info_addr = physical2u32(NULL);
            }
        }

#ifdef EARLYTEST_ENV
        /* amalgam relevant address */
        if (p_out->group_update_info.calib.cawarp_horizontal_table_addr_red != 0U) {
            p_out->group_update_info.calib.cawarp_horizontal_table_addr_red -= data_offset_base;
        } else {
            amba_ik_system_print_str_5("[IK] cawarp_horizontal_table_addr_red is NULL", NULL, NULL, NULL, NULL, NULL);
        }

        if (p_out->group_update_info.calib.cawarp_horizontal_table_addr_blue != 0U) {
            p_out->group_update_info.calib.cawarp_horizontal_table_addr_blue -= data_offset_base;
        } else {
            amba_ik_system_print_str_5("[IK] cawarp_horizontal_table_addr_blue is NULL", NULL, NULL, NULL, NULL, NULL);
        }

        if (p_out->group_update_info.calib.cawarp_vertical_table_addr_red != 0U) {
            p_out->group_update_info.calib.cawarp_vertical_table_addr_red -= data_offset_base;
        } else {
            amba_ik_system_print_str_5("[IK] cawarp_vertical_table_addr_red is NULL", NULL, NULL, NULL, NULL, NULL);
        }

        if (p_out->group_update_info.calib.cawarp_vertical_table_addr_blue != 0U) {
            p_out->group_update_info.calib.cawarp_vertical_table_addr_blue -= data_offset_base;
        } else {
            amba_ik_system_print_str_5("[IK] cawarp_vertical_table_addr_blue is NULL", NULL, NULL, NULL, NULL, NULL);
        }

        if (p_out->group_update_info.calib.warp_horizontal_table_address != 0U) {
            p_out->group_update_info.calib.warp_horizontal_table_address -= data_offset_base;
        } else {
            amba_ik_system_print_str_5("[IK] warp_horizontal_table_address is NULL", NULL, NULL, NULL, NULL, NULL);
        }

        if (p_out->group_update_info.calib.warp_vertical_table_address != 0U) {
            p_out->group_update_info.calib.warp_vertical_table_address -= data_offset_base;
        } else {
            amba_ik_system_print_str_5("[IK] warp_vertical_table_address is NULL", NULL, NULL, NULL, NULL, NULL);
        }

        if (p_out->group_update_info.calib.warp_horizontal_b_table_address != 0U) {
            p_out->group_update_info.calib.warp_horizontal_b_table_address -= data_offset_base;
        } else {
            amba_ik_system_print_str_5("[IK] warp_horizontal_b_table_address is NULL", NULL, NULL, NULL, NULL, NULL);
        }

        if (p_out->group_update_info.stitch.aaa_info_daddr != 0U) {
            p_out->group_update_info.stitch.aaa_info_daddr -= data_offset_base;
        } else {
            amba_ik_system_print_str_5("[IK] stitch.aaa_info_daddr is NULL", NULL, NULL, NULL, NULL, NULL);
        }
#endif

        /* Clean out cache */
        if (rval == IK_OK) {
            amba_ik_system_clean_cache(p_result->p_out, exptected_buf_size.out_buf_size);
        }
    }

#if 0 // debug
    {
        uint32 flip_h, flip_v;
        flip_h = p_info->p_in->flip_mode & IK_FLIP_YUV_H;
        flip_v = p_info->p_in->flip_mode & IK_FLIP_RAW_V;

        amba_ik_system_print_uint32_5("system.tuning_mode IMG_MODE_PREVIEW", DC_U, DC_U, DC_U, DC_U, DC_U);
        amba_ik_system_print_uint32_5("system.ability %d", p_ability->video_pipe, DC_U, DC_U, DC_U, DC_U);
        amba_ik_system_print_uint32_5("system.flip_h %d", flip_h, DC_U, DC_U, DC_U, DC_U);
        amba_ik_system_print_uint32_5("system.flip_v %d", flip_v, DC_U, DC_U, DC_U, DC_U);
        amba_ik_system_print_uint32_5("system.yuv_mode %d", p_info->p_in->yuv_mode, DC_U, DC_U, DC_U, DC_U);
        amba_ik_system_print_uint32_5("system.raw_bayer %d", p_info->p_in->p_sensor_info->sensor_pattern, DC_U, DC_U, DC_U, DC_U);
    }
    if (p_info->p_in->p_rgb_ir!=NULL) {
        amba_ik_system_print_uint32_5("rgb_ir.mode %d", p_info->p_in->p_rgb_ir->mode, DC_U, DC_U, DC_U, DC_U);
        amba_ik_system_print_uint32_5("rgb_ir.ircorrect_offset_b %d", p_info->p_in->p_rgb_ir->ircorrect_offset_b, DC_U, DC_U, DC_U, DC_U);
        amba_ik_system_print_uint32_5("rgb_ir.ircorrect_offset_gb %d", p_info->p_in->p_rgb_ir->ircorrect_offset_gb, DC_U, DC_U, DC_U, DC_U);
        amba_ik_system_print_uint32_5("rgb_ir.ircorrect_offset_gr %d", p_info->p_in->p_rgb_ir->ircorrect_offset_gr, DC_U, DC_U, DC_U, DC_U);
        amba_ik_system_print_uint32_5("rgb_ir.ircorrect_offset_r %d", p_info->p_in->p_rgb_ir->ircorrect_offset_r, DC_U, DC_U, DC_U, DC_U);
        amba_ik_system_print_uint32_5("rgb_ir.mul_base_val %d", p_info->p_in->p_rgb_ir->mul_base_val, DC_U, DC_U, DC_U, DC_U);
        amba_ik_system_print_uint32_5("rgb_ir.mul_delta_high %d", p_info->p_in->p_rgb_ir->mul_delta_high, DC_U, DC_U, DC_U, DC_U);
        amba_ik_system_print_uint32_5("rgb_ir.mul_high %d", p_info->p_in->p_rgb_ir->mul_high, DC_U, DC_U, DC_U, DC_U);
        amba_ik_system_print_uint32_5("rgb_ir.mul_high_val %d", p_info->p_in->p_rgb_ir->mul_high_val, DC_U, DC_U, DC_U, DC_U);
        amba_ik_system_print_uint32_5("rgb_ir.ir_only %d", p_info->p_in->p_rgb_ir->ir_only, DC_U, DC_U, DC_U, DC_U);
    } else {
        amba_ik_system_print_uint32_5("#rgb_ir is NULL", DC_U, DC_U, DC_U, DC_U, DC_U);
    }
    {
        amba_ik_system_print_uint32_5("warp_compensation.enable %d", p_info->p_in->warp_enable, DC_U, DC_U, DC_U, DC_U);
        if (p_info->p_in->warp_enable==1) {
            uintptr address;
            amba_ik_system_print_uint32_5("warp_compensation.calib_version %d", p_info->p_in->p_calib_warp_info->version, DC_U, DC_U, DC_U, DC_U);
            amba_ik_system_print_uint32_5("warp_compensation.horizontal_grid_number %d", p_info->p_in->p_calib_warp_info->hor_grid_num, DC_U, DC_U, DC_U, DC_U);
            amba_ik_system_print_uint32_5("warp_compensation.vertical_grid_number %d", p_info->p_in->p_calib_warp_info->ver_grid_num, DC_U, DC_U, DC_U, DC_U);
            amba_ik_system_print_uint32_5("warp_compensation.tile_width_exponent %d", p_info->p_in->p_calib_warp_info->tile_width_exp, DC_U, DC_U, DC_U, DC_U);
            amba_ik_system_print_uint32_5("warp_compensation.tile_height_exponent %d", p_info->p_in->p_calib_warp_info->tile_height_exp, DC_U, DC_U, DC_U, DC_U);
            amba_ik_system_print_uint32_5("warp_compensation.vin_sensor_start_x %d", p_info->p_in->p_calib_warp_info->vin_sensor_geo.start_x, DC_U, DC_U, DC_U, DC_U);
            amba_ik_system_print_uint32_5("warp_compensation.vin_sensor_start_y %d", p_info->p_in->p_calib_warp_info->vin_sensor_geo.start_y, DC_U, DC_U, DC_U, DC_U);
            amba_ik_system_print_uint32_5("warp_compensation.vin_sensor_width %d", p_info->p_in->p_calib_warp_info->vin_sensor_geo.width, DC_U, DC_U, DC_U, DC_U);
            amba_ik_system_print_uint32_5("warp_compensation.vin_sensor_height %d", p_info->p_in->p_calib_warp_info->vin_sensor_geo.height, DC_U, DC_U, DC_U, DC_U);
            amba_ik_system_print_uint32_5("warp_compensation.vin_sensor_h_subsample_factor_num %d", p_info->p_in->p_calib_warp_info->vin_sensor_geo.h_sub_sample.factor_num, DC_U, DC_U, DC_U, DC_U);
            amba_ik_system_print_uint32_5("warp_compensation.vin_sensor_h_subsample_factor_den %d", p_info->p_in->p_calib_warp_info->vin_sensor_geo.h_sub_sample.factor_den, DC_U, DC_U, DC_U, DC_U);
            amba_ik_system_print_uint32_5("warp_compensation.vin_sensor_v_subsample_factor_num %d", p_info->p_in->p_calib_warp_info->vin_sensor_geo.v_sub_sample.factor_num, DC_U, DC_U, DC_U, DC_U);
            amba_ik_system_print_uint32_5("warp_compensation.vin_sensor_v_subsample_factor_den %d", p_info->p_in->p_calib_warp_info->vin_sensor_geo.v_sub_sample.factor_den, DC_U, DC_U, DC_U, DC_U);
            amba_ik_system_print_uint32_5("warp_compensation.enb_2stage_compensation %d", p_info->p_in->p_calib_warp_info->enb_2_stage_compensation, DC_U, DC_U, DC_U, DC_U);
            amba_ik_system_memcpy(&address, &p_info->p_in->p_calib_warp_info->pwarp, sizeof(uintptr));
            amba_ik_system_print_uint32_5("warp_compensation.warp_grid_table_path 0x%x", address, DC_U, DC_U, DC_U, DC_U);
        }
    }
    {
        amba_ik_system_print_uint32_5("warp_compensation_2nd.enable %d", p_info->p_in->warp_2nd_enable, DC_U, DC_U, DC_U, DC_U);
        if (p_info->p_in->warp_2nd_enable==1) {
            uintptr address;
            amba_ik_system_print_uint32_5("warp_compensation_2nd.calib_version %d", p_info->p_in->p_calib_warp_info_2nd->version, DC_U, DC_U, DC_U, DC_U);
            amba_ik_system_print_uint32_5("warp_compensation_2nd.horizontal_grid_number %d", p_info->p_in->p_calib_warp_info_2nd->hor_grid_num, DC_U, DC_U, DC_U, DC_U);
            amba_ik_system_print_uint32_5("warp_compensation_2nd.vertical_grid_number %d", p_info->p_in->p_calib_warp_info_2nd->ver_grid_num, DC_U, DC_U, DC_U, DC_U);
            amba_ik_system_print_uint32_5("warp_compensation_2nd.tile_width_exponent %d", p_info->p_in->p_calib_warp_info_2nd->tile_width_exp, DC_U, DC_U, DC_U, DC_U);
            amba_ik_system_print_uint32_5("warp_compensation_2nd.tile_height_exponent %d", p_info->p_in->p_calib_warp_info_2nd->tile_height_exp, DC_U, DC_U, DC_U, DC_U);
            amba_ik_system_print_uint32_5("warp_compensation_2nd.vin_sensor_start_x %d", p_info->p_in->p_calib_warp_info_2nd->vin_sensor_geo.start_x, DC_U, DC_U, DC_U, DC_U);
            amba_ik_system_print_uint32_5("warp_compensation_2nd.vin_sensor_start_y %d", p_info->p_in->p_calib_warp_info_2nd->vin_sensor_geo.start_y, DC_U, DC_U, DC_U, DC_U);
            amba_ik_system_print_uint32_5("warp_compensation_2nd.vin_sensor_width %d", p_info->p_in->p_calib_warp_info_2nd->vin_sensor_geo.width, DC_U, DC_U, DC_U, DC_U);
            amba_ik_system_print_uint32_5("warp_compensation_2nd.vin_sensor_height %d", p_info->p_in->p_calib_warp_info_2nd->vin_sensor_geo.height, DC_U, DC_U, DC_U, DC_U);
            amba_ik_system_print_uint32_5("warp_compensation_2nd.vin_sensor_h_subsample_factor_num %d", p_info->p_in->p_calib_warp_info_2nd->vin_sensor_geo.h_sub_sample.factor_num, DC_U, DC_U, DC_U, DC_U);
            amba_ik_system_print_uint32_5("warp_compensation_2nd.vin_sensor_h_subsample_factor_den %d", p_info->p_in->p_calib_warp_info_2nd->vin_sensor_geo.h_sub_sample.factor_den, DC_U, DC_U, DC_U, DC_U);
            amba_ik_system_print_uint32_5("warp_compensation_2nd.vin_sensor_v_subsample_factor_num %d", p_info->p_in->p_calib_warp_info_2nd->vin_sensor_geo.v_sub_sample.factor_num, DC_U, DC_U, DC_U, DC_U);
            amba_ik_system_print_uint32_5("warp_compensation_2nd.vin_sensor_v_subsample_factor_den %d", p_info->p_in->p_calib_warp_info_2nd->vin_sensor_geo.v_sub_sample.factor_den, DC_U, DC_U, DC_U, DC_U);
            amba_ik_system_print_uint32_5("warp_compensation_2nd.enb_2stage_compensation %d", p_info->p_in->p_calib_warp_info_2nd->enb_2_stage_compensation, DC_U, DC_U, DC_U, DC_U);
            amba_ik_system_memcpy(&address, &p_info->p_in->p_calib_warp_info_2nd->pwarp, sizeof(uintptr));
            amba_ik_system_print_uint32_5("warp_compensation_2nd.warp_grid_table_path 0x%x", address, DC_U, DC_U, DC_U, DC_U);
        }
    }
    {
        amba_ik_system_print_uint32_5("chromatic_aberration_correction.enable %d", p_info->p_in->cawarp_enable, DC_U, DC_U, DC_U, DC_U);
        if (p_info->p_in->cawarp_enable==1) {
            uintptr address;
            amba_ik_system_print_uint32_5("chromatic_aberration_correction.calib_version %d", p_info->p_in->p_calib_ca_warp_info->version, DC_U, DC_U, DC_U, DC_U);
            amba_ik_system_print_uint32_5("chromatic_aberration_correction.horizontal_grid_number %d", p_info->p_in->p_calib_ca_warp_info->hor_grid_num, DC_U, DC_U, DC_U, DC_U);
            amba_ik_system_print_uint32_5("chromatic_aberration_correction.vertical_grid_number %d", p_info->p_in->p_calib_ca_warp_info->ver_grid_num, DC_U, DC_U, DC_U, DC_U);
            amba_ik_system_print_uint32_5("chromatic_aberration_correction.tile_width_exponent %d", p_info->p_in->p_calib_ca_warp_info->tile_width_exp, DC_U, DC_U, DC_U, DC_U);
            amba_ik_system_print_uint32_5("chromatic_aberration_correction.tile_height_exponent %d", p_info->p_in->p_calib_ca_warp_info->tile_height_exp, DC_U, DC_U, DC_U, DC_U);
            amba_ik_system_print_uint32_5("chromatic_aberration_correction.vin_sensor_start_x %d", p_info->p_in->p_calib_ca_warp_info->vin_sensor_geo.start_x, DC_U, DC_U, DC_U, DC_U);
            amba_ik_system_print_uint32_5("chromatic_aberration_correction.vin_sensor_start_y %d", p_info->p_in->p_calib_ca_warp_info->vin_sensor_geo.start_y, DC_U, DC_U, DC_U, DC_U);
            amba_ik_system_print_uint32_5("chromatic_aberration_correction.vin_sensor_width %d", p_info->p_in->p_calib_ca_warp_info->vin_sensor_geo.width, DC_U, DC_U, DC_U, DC_U);
            amba_ik_system_print_uint32_5("chromatic_aberration_correction.vin_sensor_height %d", p_info->p_in->p_calib_ca_warp_info->vin_sensor_geo.height, DC_U, DC_U, DC_U, DC_U);
            amba_ik_system_print_uint32_5("chromatic_aberration_correction.vin_sensor_h_subsample_factor_num %d", p_info->p_in->p_calib_ca_warp_info->vin_sensor_geo.h_sub_sample.factor_num, DC_U, DC_U, DC_U, DC_U);
            amba_ik_system_print_uint32_5("chromatic_aberration_correction.vin_sensor_h_subsample_factor_den %d", p_info->p_in->p_calib_ca_warp_info->vin_sensor_geo.h_sub_sample.factor_den, DC_U, DC_U, DC_U, DC_U);
            amba_ik_system_print_uint32_5("chromatic_aberration_correction.vin_sensor_v_subsample_factor_num %d", p_info->p_in->p_calib_ca_warp_info->vin_sensor_geo.v_sub_sample.factor_num, DC_U, DC_U, DC_U, DC_U);
            amba_ik_system_print_uint32_5("chromatic_aberration_correction.vin_sensor_v_subsample_factor_den %d", p_info->p_in->p_calib_ca_warp_info->vin_sensor_geo.v_sub_sample.factor_den, DC_U, DC_U, DC_U, DC_U);
            amba_ik_system_print_uint32_5("chromatic_aberration_correction.enb_2stage_compensation %d", p_info->p_in->p_calib_ca_warp_info->enb_2_stage_compensation, DC_U, DC_U, DC_U, DC_U);
            amba_ik_system_memcpy(&address, &p_info->p_in->p_calib_ca_warp_info->p_cawarp_red, sizeof(uintptr));
            amba_ik_system_print_uint32_5("chromatic_aberration_correction.red_ca_grid_table_path 0x%x", address, DC_U, DC_U, DC_U, DC_U);
            amba_ik_system_memcpy(&address, &p_info->p_in->p_calib_ca_warp_info->p_cawarp_blue, sizeof(uintptr));
            amba_ik_system_print_uint32_5("chromatic_aberration_correction.blue_ca_grid_table_path 0x%x", address, DC_U, DC_U, DC_U, DC_U);
        }
    }
    if (p_info->p_in->p_window_size_info!=NULL) {
        amba_ik_system_print_uint32_5("system.raw_start_x %d", p_info->p_in->p_window_size_info->vin_sensor.start_x, DC_U, DC_U, DC_U, DC_U);
        amba_ik_system_print_uint32_5("system.raw_start_y %d", p_info->p_in->p_window_size_info->vin_sensor.start_y, DC_U, DC_U, DC_U, DC_U);
        amba_ik_system_print_uint32_5("system.raw_pitch %d", p_info->p_in->p_window_size_info->vin_sensor.width, DC_U, DC_U, DC_U, DC_U);
        amba_ik_system_print_uint32_5("system.raw_width %d", p_info->p_in->p_window_size_info->vin_sensor.width, DC_U, DC_U, DC_U, DC_U);
        amba_ik_system_print_uint32_5("system.raw_height %d", p_info->p_in->p_window_size_info->vin_sensor.height, DC_U, DC_U, DC_U, DC_U);
        amba_ik_system_print_uint32_5("system.h_subsample_factor_num %d", p_info->p_in->p_window_size_info->vin_sensor.h_sub_sample.factor_num, DC_U, DC_U, DC_U, DC_U);
        amba_ik_system_print_uint32_5("system.h_subsample_factor_den %d", p_info->p_in->p_window_size_info->vin_sensor.h_sub_sample.factor_den, DC_U, DC_U, DC_U, DC_U);
        amba_ik_system_print_uint32_5("system.v_subsample_factor_num %d", p_info->p_in->p_window_size_info->vin_sensor.v_sub_sample.factor_num, DC_U, DC_U, DC_U, DC_U);
        amba_ik_system_print_uint32_5("system.v_subsample_factor_den %d", p_info->p_in->p_window_size_info->vin_sensor.v_sub_sample.factor_den, DC_U, DC_U, DC_U, DC_U);
        amba_ik_system_print_uint32_5("system.main_width %d", p_info->p_in->p_window_size_info->main_win.width, DC_U, DC_U, DC_U, DC_U);
        amba_ik_system_print_uint32_5("system.main_height %d", p_info->p_in->p_window_size_info->main_win.height, DC_U, DC_U, DC_U, DC_U);
    }
    if (p_info->p_in->p_active_window!=NULL) {
        amba_ik_system_print_uint32_5("active.enable %d", p_info->p_in->p_active_window->enable, DC_U, DC_U, DC_U, DC_U);
        amba_ik_system_print_uint32_5("active.startx %d", p_info->p_in->p_active_window->active_geo.start_x, DC_U, DC_U, DC_U, DC_U);
        amba_ik_system_print_uint32_5("active.starty %d", p_info->p_in->p_active_window->active_geo.start_y, DC_U, DC_U, DC_U, DC_U);
        amba_ik_system_print_uint32_5("active.width %d", p_info->p_in->p_active_window->active_geo.width, DC_U, DC_U, DC_U, DC_U);
        amba_ik_system_print_uint32_5("active.height %d", p_info->p_in->p_active_window->active_geo.height, DC_U, DC_U, DC_U, DC_U);
    } else {
        amba_ik_system_print_uint32_5("#active is NULL", DC_U, DC_U, DC_U, DC_U, DC_U);
    }
    if (p_info->p_in->p_dmy_range!=NULL) {
        amba_ik_system_print_uint32_5("dummy.enable %d", p_info->p_in->p_dmy_range->enable, DC_U, DC_U, DC_U, DC_U);
        amba_ik_system_print_uint32_5("dummy.top %d", p_info->p_in->p_dmy_range->top, DC_U, DC_U, DC_U, DC_U);
        amba_ik_system_print_uint32_5("dummy.bottom %d", p_info->p_in->p_dmy_range->bottom, DC_U, DC_U, DC_U, DC_U);
        amba_ik_system_print_uint32_5("dummy.left %d", p_info->p_in->p_dmy_range->left, DC_U, DC_U, DC_U, DC_U);
        amba_ik_system_print_uint32_5("dummy.right %d", p_info->p_in->p_dmy_range->right, DC_U, DC_U, DC_U, DC_U);
    } else {
        amba_ik_system_print_uint32_5("#dummy is NULL", DC_U, DC_U, DC_U, DC_U, DC_U);
    }
    if (p_info->p_in->p_dzoom_info!=NULL) {
        amba_ik_system_print_uint32_5("dzoom.enable %d", p_info->p_in->p_dzoom_info->enable, DC_U, DC_U, DC_U, DC_U);
        amba_ik_system_print_uint32_5("dzoom.shiftx %d", p_info->p_in->p_dzoom_info->shift_x, DC_U, DC_U, DC_U, DC_U);
        amba_ik_system_print_uint32_5("dzoom.shifty %d", p_info->p_in->p_dzoom_info->shift_y, DC_U, DC_U, DC_U, DC_U);
        amba_ik_system_print_uint32_5("dzoom.zoomx %d", p_info->p_in->p_dzoom_info->zoom_x, DC_U, DC_U, DC_U, DC_U);
        amba_ik_system_print_uint32_5("dzoom.zoomy %d", p_info->p_in->p_dzoom_info->zoom_y, DC_U, DC_U, DC_U, DC_U);
    } else {
        amba_ik_system_print_uint32_5("#dzoom is NULL", DC_U, DC_U, DC_U, DC_U, DC_U);
    }
    {
        amba_ik_system_print_uint32_5("li_chroma_filter.enable 1", DC_U, DC_U, DC_U, DC_U, DC_U);
        amba_ik_system_print_uint32_5("li_chroma_filter.noise_level_cb 10", DC_U, DC_U, DC_U, DC_U, DC_U);
        amba_ik_system_print_uint32_5("li_chroma_filter.noise_level_cr 10", DC_U, DC_U, DC_U, DC_U, DC_U);
        amba_ik_system_print_uint32_5("li_chroma_filter.original_blend_strength_cb 0", DC_U, DC_U, DC_U, DC_U, DC_U);
        amba_ik_system_print_uint32_5("li_chroma_filter.original_blend_strength_cr 0", DC_U, DC_U, DC_U, DC_U, DC_U);
        amba_ik_system_print_uint32_5("li_chroma_filter.radius %d", p_info->p_in->chroma_filter_radius, DC_U, DC_U, DC_U, DC_U);
    }
    {
        uint32 j;

        amba_ik_system_print_uint32_5("H warp table content", DC_U, DC_U, DC_U, DC_U, DC_U);
        for (j=0; j<WARP_VIDEO_MAXVERGRID; j++) {
            amba_ik_system_print_uint32_5("%d %d %d %d %d ", p_out->warp_hor[j*WARP_VIDEO_MAXVERGRID], p_out->warp_hor[j*WARP_VIDEO_MAXVERGRID+1], p_out->warp_hor[j*WARP_VIDEO_MAXVERGRID+2], p_out->warp_hor[j*WARP_VIDEO_MAXVERGRID+3], p_out->warp_hor[j*WARP_VIDEO_MAXVERGRID+4]);
        }

        amba_ik_system_print_uint32_5("V warp table content", DC_U, DC_U, DC_U, DC_U, DC_U);
        for (j=0; j<WARP_VIDEO_MAXVERGRID; j++) {
            amba_ik_system_print_uint32_5("%d %d %d %d %d ", p_out->warp_ver[j*WARP_VIDEO_MAXVERGRID], p_out->warp_ver[j*WARP_VIDEO_MAXVERGRID+1], p_out->warp_ver[j*WARP_VIDEO_MAXVERGRID+2], p_out->warp_ver[j*WARP_VIDEO_MAXVERGRID+3], p_out->warp_ver[j*WARP_VIDEO_MAXVERGRID+4]);
        }
    }
#endif
    return rval;

}

uint32 exe_warp_remap(const ik_in_warp_remap_t *p_warp_remap_in, ik_out_warp_remap_t *p_warp_remap_out)
{
    const ikc_in_warp_remap_t *p_ikc_warp_remap_in = p_warp_remap_in;
    ikc_out_warp_remap_t *p_ikc_warp_remap_out = p_warp_remap_out;

    return ikc_warp_remap(p_ikc_warp_remap_in, p_ikc_warp_remap_out);
}

uint32 exe_warp_remap_hvh_vr(const ik_in_warp_remap_t *p_warp_remap_in, ik_out_warp_remap_t *p_warp_remap_out_1st, ik_out_warp_remap_t *p_warp_remap_out_2nd)
{
    const ikc_in_warp_remap_t *p_ikc_warp_remap_in = p_warp_remap_in;
    ikc_out_warp_remap_t *p_ikc_warp_remap_out_1st = p_warp_remap_out_1st;
    ikc_out_warp_remap_t *p_ikc_warp_remap_out_2nd = p_warp_remap_out_2nd;

    return ikc_warp_remap_hvh_vr(p_ikc_warp_remap_in, p_ikc_warp_remap_out_1st, p_ikc_warp_remap_out_2nd);
}

uint32 exe_cawarp_remap(const ik_in_cawarp_remap_t *p_cawarp_remap_in, ik_out_cawarp_remap_t *p_cawarp_remap_out)
{
    const ikc_in_cawarp_remap_t *p_ikc_cawarp_remap_in = p_cawarp_remap_in;
    ikc_out_cawarp_remap_t *p_ikc_cawarp_remap_out = p_cawarp_remap_out;

    return ikc_cawarp_remap(p_ikc_cawarp_remap_in, p_ikc_cawarp_remap_out);
}

uint32 exe_process_temp_hack_hiso_data(idsp_flow_ctrl_t *p_flow, amba_ik_filter_t *p_filters)
{
    uint32 rval = IK_OK;
    ikc_in_temp_hack_hiso_data_t in_hiso;
    ikc_out_temp_hack_hiso_data_t out_hiso;

    if (p_flow == NULL) {
        /*misra fix*/
    } else {
        in_hiso.exp_num = p_filters->input_param.num_of_exposures;
        in_hiso.p_hdr_raw_info = &p_filters->input_param.hdr_raw_info;
        in_hiso.p_main                = &p_filters->input_param.window_size_info.main_win;
        in_hiso.hi_nonsmooth_detect   = &p_filters->hiso_input_param.hi_nonsmooth_detect;
        in_hiso.hi_chroma_filter_high = &p_filters->hiso_input_param.hi_chroma_filter_high;
        out_hiso.p_flow = p_flow;
        rval |= ikc_temp_hack_hiso_data(&in_hiso, &out_hiso);
    }
    return rval;
}

static inline void exe_iso_cfg_high_iso_chroma_asf_to_asf(ik_adv_spatial_filter_t *p_asf, const ik_hi_chroma_asf_t *p_chroma_asf)
{
    p_asf->max_change_not_t0_t1_alpha = p_chroma_asf->max_change_not_t0_t1_alpha;
    p_asf->enable = p_chroma_asf->enable;
    (void)amba_ik_system_memcpy(&p_asf->fir,&p_chroma_asf->fir,sizeof(ik_asf_fir_t));
    p_asf->directional_decide_t0 = p_chroma_asf->directional_decide_t0;
    p_asf->directional_decide_t1 = p_chroma_asf->directional_decide_t1;
    p_asf->adapt.alpha_min_up = p_chroma_asf->alpha_min;
    p_asf->adapt.alpha_min_down = p_chroma_asf->alpha_min;
    p_asf->adapt.alpha_max_up = p_chroma_asf->alpha_max;
    p_asf->adapt.alpha_max_down = p_chroma_asf->alpha_max;
    p_asf->adapt.t0_up = p_chroma_asf->t0;
    p_asf->adapt.t0_down = p_chroma_asf->t0;
    p_asf->adapt.t1_up = p_chroma_asf->t1;
    p_asf->adapt.t1_down = p_chroma_asf->t1;
    (void)amba_ik_system_memcpy(&p_asf->level_str_adjust,&p_chroma_asf->level_str_adjust,sizeof(ik_level_t));
    (void)amba_ik_system_memcpy(&p_asf->t0_t1_div,&p_chroma_asf->t0_t1_div,sizeof(ik_level_t));

    //2017.03.15 ref: [H22][issue]Hi/Li_chroma_advanced_spatial_filter.enable=0 has image broken problem
    if (p_chroma_asf->enable == 0U) {
        p_asf->enable = 1U;
        p_asf->max_change_up = 0U;
        p_asf->max_change_down = 0U;
    } else {
        p_asf->max_change_up = p_chroma_asf->max_change;
        p_asf->max_change_down = p_chroma_asf->max_change;
    }
}

static uint32 exe_process_init_y2y_rgb2yuv(const amba_ik_step3_sec2_cr_list_t *sec2, const amba_ik_filter_t *p_filters)
{
    uint32 rval = IK_OK;
    ik_rgb_to_yuv_matrix_t rgb_to_yuv_matrix;
    ikc_in_rgb2yuv_t in_rgb2yuv = {0};
    ikc_out_rgb2yuv_t out_rgb2yuv;
    rgb_to_yuv_matrix.matrix_values[0] = 1024;
    rgb_to_yuv_matrix.matrix_values[1] = 0;
    rgb_to_yuv_matrix.matrix_values[2] = 0;
    rgb_to_yuv_matrix.matrix_values[3] = 0;
    rgb_to_yuv_matrix.matrix_values[4] = 1024;
    rgb_to_yuv_matrix.matrix_values[5] = 0;
    rgb_to_yuv_matrix.matrix_values[6] = 0;
    rgb_to_yuv_matrix.matrix_values[7] = 0;
    rgb_to_yuv_matrix.matrix_values[8] = 1024;
    rgb_to_yuv_matrix.y_offset = 0;
    rgb_to_yuv_matrix.u_offset = 128;
    rgb_to_yuv_matrix.v_offset= 128;
    if ((p_filters->input_param.sensor_info.sensor_mode == 1U)/* && (p_filters->input_param.rgb_ir.ir_only == 1U)*/) {
        (void)amba_ik_system_memset(&rgb_to_yuv_matrix, 0x0, sizeof(ik_rgb_to_yuv_matrix_t));
        rgb_to_yuv_matrix.matrix_values[2] = 1024; //m13
        rgb_to_yuv_matrix.u_offset = 128;
        rgb_to_yuv_matrix.v_offset = 128;
    }

    if(p_filters->update_flags.cr.r2y.cr_29_update == 0U) {
        amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on exe_process_init_y2y_rgb2yuv!!!",DC_U,DC_U,DC_U,DC_U,DC_U);
        rval |= IK_ERR_0000;
    } else {
        in_rgb2yuv.p_rgb_to_yuv_matrix = &rgb_to_yuv_matrix;
        out_rgb2yuv.p_cr_29 =sec2->p_CR_buf_29;
        out_rgb2yuv.cr_29_size = CR_SIZE_29;
        rval |= ikc_rgb2yuv(&in_rgb2yuv, &out_rgb2yuv);
    }
    return rval;
}

static uint32 exe_process_input_mode_y2y_step2(idsp_flow_ctrl_t *p_flow, const amba_ik_step2_sec2_cr_list_t *sec2, amba_ik_filter_t *p_filters)
{
    uint32 rval = IK_OK;
    // exe_process_calib_cr
    ikc_in_input_mode_t in_input_mode;
    ikc_out_input_mode_y2y_t out_input_mode;

    rval |= exe_check_vin_sensor(p_filters);
    if((p_filters->update_flags.cr.r2y.cr_26_update == 0U)||
       (p_filters->update_flags.cr.r2y.cr_29_update == 0U)||
       (p_filters->update_flags.cr.r2y.cr_30_update == 0U)||
       (p_filters->update_flags.cr.r2y.cr_31_update == 0U)||
       (p_filters->update_flags.cr.r2y.cr_32_update == 0U)) {
        amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on exe_process_input_mode_y2y_step2 !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
        rval |= IK_ERR_0000;
    } else {
        in_input_mode.flip_mode = p_filters->input_param.flip_mode;
        in_input_mode.p_sensor_info = &p_filters->input_param.sensor_info;
        out_input_mode.p_cr_26 = sec2->p_CR_buf_26;
        out_input_mode.cr_26_size = CR_SIZE_26;
        out_input_mode.p_cr_29 = sec2->p_CR_buf_29;
        out_input_mode.cr_29_size = CR_SIZE_29;
        out_input_mode.p_cr_30 = sec2->p_CR_buf_30;
        out_input_mode.cr_30_size = CR_SIZE_30;
        out_input_mode.p_cr_31 = sec2->p_CR_buf_31;
        out_input_mode.cr_31_size = CR_SIZE_31;
        out_input_mode.p_cr_32 = sec2->p_CR_buf_32;
        out_input_mode.cr_32_size = CR_SIZE_32;
        out_input_mode.p_flow = &p_flow->flow_info;
        out_input_mode.p_calib = &p_flow->calib;
        in_input_mode.is_yuv_mode = 1U;
        rval |= ikc_input_mode_y2y_step2(&in_input_mode, &out_input_mode);
    }
    return rval;
}


//For step 3 5 6 7 8 9 10 13
static uint32 exe_process_input_mode_y2y_step3(idsp_flow_ctrl_t *p_flow, const amba_ik_step3_sec2_cr_list_t *sec2, const amba_ik_filter_t *p_filters, uint32 step)
{
    uint32 rval = IK_OK;
    // exe_process_calib_cr
    ikc_in_input_mode_y2y_t in_input_mode;
    ikc_out_input_mode_y2y_t out_input_mode;

    rval |= exe_check_vin_sensor(p_filters);
    if((p_filters->update_flags.cr.r2y.cr_26_update == 0U)||
       (p_filters->update_flags.cr.r2y.cr_29_update == 0U)||
       (p_filters->update_flags.cr.r2y.cr_30_update == 0U)||
       (p_filters->update_flags.cr.r2y.cr_31_update == 0U)||
       (p_filters->update_flags.cr.r2y.cr_32_update == 0U)) {
        amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on exe_process_input_mode_y2y_step3 !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
        rval |= IK_ERR_0000;
    } else {
        out_input_mode.p_cr_26 = sec2->p_CR_buf_26;
        out_input_mode.cr_26_size = CR_SIZE_26;
        out_input_mode.p_cr_29 = sec2->p_CR_buf_29;
        out_input_mode.cr_29_size = CR_SIZE_29;
        out_input_mode.p_cr_30 = sec2->p_CR_buf_30;
        out_input_mode.cr_30_size = CR_SIZE_30;
        out_input_mode.p_cr_31 = sec2->p_CR_buf_31;
        out_input_mode.cr_31_size = CR_SIZE_31;
        out_input_mode.p_cr_32 = sec2->p_CR_buf_32;
        out_input_mode.cr_32_size = CR_SIZE_32;
        out_input_mode.p_flow = &p_flow->flow_info;
        out_input_mode.p_calib = &p_flow->calib;
        in_input_mode.step = step;
        rval |= ikc_input_mode_y2y_step3(&in_input_mode, &out_input_mode);
    }
    return rval;
}

static uint32 exe_process_input_mode_y2y_step4(idsp_flow_ctrl_t *p_flow, const amba_ik_step3_sec2_cr_list_t *sec2, const amba_ik_filter_t *p_filters)
{
    uint32 rval = IK_OK;
    // exe_process_calib_cr
    ikc_in_input_mode_y2y_t in_input_mode;
    ikc_out_input_mode_y2y_t out_input_mode;

    rval |= exe_check_vin_sensor(p_filters);
    {
        out_input_mode.p_cr_26 = sec2->p_CR_buf_26;
        out_input_mode.cr_26_size = CR_SIZE_26;
        out_input_mode.p_cr_29 = sec2->p_CR_buf_29;
        out_input_mode.cr_29_size = CR_SIZE_29;
        out_input_mode.p_cr_30 = sec2->p_CR_buf_30;
        out_input_mode.cr_30_size = CR_SIZE_30;
        out_input_mode.p_cr_31 = sec2->p_CR_buf_31;
        out_input_mode.cr_31_size = CR_SIZE_31;
        out_input_mode.p_cr_32 = sec2->p_CR_buf_32;
        out_input_mode.cr_32_size = CR_SIZE_32;
        out_input_mode.p_flow = &p_flow->flow_info;
        out_input_mode.p_calib = &p_flow->calib;
        in_input_mode.step = 4U;
        rval |= ikc_input_mode_y2y_step4(&in_input_mode, &out_input_mode);
    }
    return rval;
}

static uint32 exe_chroma_noise_def_value_step1(const amba_ik_step1_sec2_cr_list_t *sec2, const amba_ik_filter_t *p_filters)
{
    uint32 rval = IK_OK;
    ikc_in_chroma_noise_default_val_t  in_chroma_noise;
    ikc_out_chroma_noise_default_val_t out_chroma_noise;

    if(p_filters->update_flags.cr.r2y.cr_36_update == 0U) {
        amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on exe_process_chroma_noise_default_value !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
        rval |= IK_ERR_0000;
    } else {
        in_chroma_noise.is_first_frame = p_filters->update_flags.iso.is_1st_frame;
        out_chroma_noise.p_cr_36 = sec2->p_CR_buf_36;
        out_chroma_noise.cr_36_size = CR_SIZE_36;
        rval |= ikc_chroma_noise_default(&in_chroma_noise, &out_chroma_noise);
    }
    return rval;
}

static uint32 exe_chroma_noise_def_value_step2(const amba_ik_step2_sec2_cr_list_t *sec2, const amba_ik_filter_t *p_filters)
{
    uint32 rval = IK_OK;
    ikc_in_chroma_noise_default_val_t  in_chroma_noise;
    ikc_out_chroma_noise_default_val_t out_chroma_noise;

    if(p_filters->update_flags.cr.r2y.cr_36_update == 0U) {
        amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on exe_process_chroma_noise_default_value !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
        rval |= IK_ERR_0000;
    } else {
        in_chroma_noise.is_first_frame = p_filters->update_flags.iso.is_1st_frame;
        out_chroma_noise.p_cr_36 = sec2->p_CR_buf_36;
        out_chroma_noise.cr_36_size = CR_SIZE_36;
        rval |= ikc_chroma_noise_default(&in_chroma_noise, &out_chroma_noise);
    }
    return rval;
}

static uint32 exe_chroma_noise_def_value(const amba_ik_step3_sec2_cr_list_t *sec2, const amba_ik_filter_t *p_filters)
{
    uint32 rval = IK_OK;
    ikc_in_chroma_noise_default_val_t  in_chroma_noise;
    ikc_out_chroma_noise_default_val_t out_chroma_noise;

    if(p_filters->update_flags.cr.r2y.cr_36_update == 0U) {
        amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on exe_process_chroma_noise_default_value !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
        rval |= IK_ERR_0000;
    } else {
        in_chroma_noise.is_first_frame = p_filters->update_flags.iso.is_1st_frame;
        out_chroma_noise.p_cr_36 = sec2->p_CR_buf_36;
        out_chroma_noise.cr_36_size = CR_SIZE_36;
        rval |= ikc_chroma_noise_default(&in_chroma_noise, &out_chroma_noise);
    }
    return rval;
}


static uint32 exe_process_hiso_step1_cr(idsp_flow_ctrl_t *p_flow, const amba_ik_hiso_flow_tables_list_t *p_cr, amba_ik_filter_t *p_filters, uint32 num_of_exp)
{
    uint32 rval = IK_OK;
    uint32 window_calculate_updated = 0U;

    if (num_of_exp == 0U) {
        // fix compile error
    } else {
        // default
    }

    rval |= exe_chroma_noise_def_value_step1(&p_cr->step1.sec2, p_filters);

    if ((p_filters->update_flags.iso.window_size_info_updated == 1U) ||
        (p_filters->update_flags.iso.cfa_window_size_info_updated == 1U) ||
        (p_filters->update_flags.iso.dmy_range_updated == 1U) ||
        (p_filters->update_flags.iso.dzoom_info_updated == 1U) ||
        (p_filters->update_flags.iso.vin_active_win_updated == 1U) ||
        (p_filters->update_flags.iso.stitching_info_updated == 1U) ||
        (p_filters->update_flags.iso.overlap_x_updated == 1U) ||
        (p_filters->update_flags.iso.flip_mode_updated == 1U) ||
        (p_filters->update_flags.iso.warp_enable_updated == 1U) ||
        (p_filters->update_flags.iso.warp_enable_2nd_updated == 1U)) {
        window_calculate_updated = 1U;
    }

    // exe_process_calib_cr
    if(p_filters->update_flags.iso.is_1st_frame == 1U) { //should only configure once at 1st time.
        ikc_in_input_mode_t in_input_mode;
        ikc_out_input_mode_t out_input_mode;

        rval |= exe_check_vin_sensor(p_filters);
        if((p_filters->update_flags.cr.r2y.cr_4_update == 0U)||
           (p_filters->update_flags.cr.r2y.cr_6_update == 0U)||
           (p_filters->update_flags.cr.r2y.cr_7_update == 0U)||
           (p_filters->update_flags.cr.r2y.cr_8_update == 0U)||
           (p_filters->update_flags.cr.r2y.cr_9_update == 0U)||
           (p_filters->update_flags.cr.r2y.cr_11_update == 0U)||
           (p_filters->update_flags.cr.r2y.cr_12_update == 0U)||
           (p_filters->update_flags.cr.r2y.cr_13_update == 0U)||
           (p_filters->update_flags.cr.r2y.cr_16_update == 0U)||
           (p_filters->update_flags.cr.r2y.cr_21_update == 0U)||
           (p_filters->update_flags.cr.r2y.cr_22_update == 0U)||
           (p_filters->update_flags.cr.r2y.cr_23_update == 0U)||
           (p_filters->update_flags.cr.r2y.cr_24_update == 0U)||
           (p_filters->update_flags.cr.r2y.cr_26_update == 0U)||
           (p_filters->update_flags.cr.r2y.cr_29_update == 0U)||
           (p_filters->update_flags.cr.r2y.cr_30_update == 0U)||
           (p_filters->update_flags.cr.r2y.cr_31_update == 0U)||
           (p_filters->update_flags.cr.r2y.cr_32_update == 0U)||
           (p_filters->update_flags.cr.r2y.cr_117_update == 0U)) {
            amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_input_mode !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
            rval |= IK_ERR_0000;
        } else {
            in_input_mode.flip_mode = p_filters->input_param.flip_mode;
            in_input_mode.p_sensor_info = &p_filters->input_param.sensor_info;
            out_input_mode.p_cr_4 = p_cr->step1.sec2.p_CR_buf_4;
            out_input_mode.cr_4_size = CR_SIZE_4;
            out_input_mode.p_cr_6 = p_cr->step1.sec2.p_CR_buf_6;
            out_input_mode.cr_6_size = CR_SIZE_6;
            out_input_mode.p_cr_7 = p_cr->step1.sec2.p_CR_buf_7;
            out_input_mode.cr_7_size = CR_SIZE_7;
            out_input_mode.p_cr_8 = p_cr->step1.sec2.p_CR_buf_8;
            out_input_mode.cr_8_size = CR_SIZE_8;
            out_input_mode.p_cr_9 = p_cr->step1.sec2.p_CR_buf_9;
            out_input_mode.cr_9_size = CR_SIZE_9;
            out_input_mode.p_cr_11 = p_cr->step1.sec2.p_CR_buf_11;
            out_input_mode.cr_11_size = CR_SIZE_11;
            out_input_mode.p_cr_12 = p_cr->step1.sec2.p_CR_buf_12;
            out_input_mode.cr_12_size = CR_SIZE_12;
            out_input_mode.p_cr_13 = p_cr->step1.sec2.p_CR_buf_13;
            out_input_mode.cr_13_size = CR_SIZE_13;
            out_input_mode.p_cr_16 = p_cr->step1.sec2.p_CR_buf_16;
            out_input_mode.cr_16_size = CR_SIZE_16;
            out_input_mode.p_cr_21 = p_cr->step1.sec2.p_CR_buf_21;
            out_input_mode.cr_21_size = CR_SIZE_21;
            out_input_mode.p_cr_22 = p_cr->step1.sec2.p_CR_buf_22;
            out_input_mode.cr_22_size = CR_SIZE_22;
            out_input_mode.p_cr_23 = p_cr->step1.sec2.p_CR_buf_23;
            out_input_mode.cr_23_size = CR_SIZE_23;
            out_input_mode.p_cr_24 = p_cr->step1.sec2.p_CR_buf_24;
            out_input_mode.cr_24_size = CR_SIZE_24;
            out_input_mode.p_cr_26 = p_cr->step1.sec2.p_CR_buf_26;
            out_input_mode.cr_26_size = CR_SIZE_26;
            out_input_mode.p_cr_29 = p_cr->step1.sec2.p_CR_buf_29;
            out_input_mode.cr_29_size = CR_SIZE_29;
            out_input_mode.p_cr_30 = p_cr->step1.sec2.p_CR_buf_30;
            out_input_mode.cr_30_size = CR_SIZE_30;
            out_input_mode.p_cr_31 = p_cr->step1.sec2.p_CR_buf_31;
            out_input_mode.cr_31_size = CR_SIZE_31;
            out_input_mode.p_cr_32 = p_cr->step1.sec2.p_CR_buf_32;
            out_input_mode.cr_32_size = CR_SIZE_32;
            out_input_mode.p_cr_117 = p_cr->step1.sec18.p_CR_buf_117;
            out_input_mode.cr_117_size = CR_SIZE_117;
            out_input_mode.p_flow = &p_flow->flow_info;
            out_input_mode.p_calib = &p_flow->calib;

            if ((p_filters->input_param.ability == AMBA_IK_VIDEO_PIPE_Y2Y) ||
                (p_filters->input_param.ability == AMBA_IK_VIDEO_PIPE_Y2Y_HVH) ||
                (p_filters->input_param.ability == AMBA_IK_VIDEO_PIPE_Y2Y_HHB) ||
                (p_filters->input_param.ability == AMBA_IK_VIDEO_PIPE_Y2Y_MD) ||
                (p_filters->input_param.ability == AMBA_IK_VIDEO_PIPE_Y2Y_MD_HHB)) {
                in_input_mode.is_yuv_mode = 1U;
            } else {
                in_input_mode.is_yuv_mode = 0U;
            }
            rval |= ikc_input_mode(&in_input_mode, &out_input_mode);
        }
    }
    if (window_calculate_updated == 1U) {
        rval |= exe_win_calc_wrapper(p_filters);
    }
    if((p_filters->update_flags.iso.static_bpc_updated == 1u) || (p_filters->update_flags.iso.static_bpc_internal_updated == 1u) ||
       (p_filters->update_flags.iso.dynamic_bpc_updated == 1u) || (p_filters->update_flags.iso.window_size_info_updated == 1U)) {
        if(p_filters->input_param.ctx_buf.sbp_internal_mode_flag == 1u) {
            ikc_in_static_bad_pixel_internal_t in_sbp_internal;
            ikc_out_static_bad_pixel_internal_t out_sbp;
            ik_static_bad_pixel_correction_internal_t sbp_internal_tmp;
            const void *void_ptr;

            if(p_filters->update_flags.cr.r2y.cr_12_update == 0U) {
                amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_static_bad_pixel_internal !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
                rval |= IK_ERR_0000;
            } else {
                //consider 64 bit compiler, flow data sbp addr(u32) goes on physical addr + offset.
                (void)amba_ik_system_memcpy(&sbp_internal_tmp,&p_filters->input_param.sbp_internal,sizeof(ik_static_bad_pixel_correction_internal_t));
                void_ptr = amba_ik_system_virt_to_phys(sbp_internal_tmp.p_map);
                (void)amba_ik_system_memcpy(&sbp_internal_tmp.p_map, &void_ptr, sizeof(void*));
                in_sbp_internal.dbp_enable = p_filters->input_param.dynamic_bpc.enable;
                in_sbp_internal.p_sbp_internal = &sbp_internal_tmp;
                out_sbp.p_cr_12 = p_cr->step1.sec2.p_CR_buf_12;
                out_sbp.cr_12_size = CR_SIZE_12;
                out_sbp.p_flow = &p_flow->flow_info;
                rval |= ikc_static_bad_pixel_internal(&in_sbp_internal, &out_sbp);
            }
        } else {
            ikc_in_static_bad_pixel_t in_sbp;
            ikc_out_static_bad_pixel_t out_sbp;
            ik_static_bad_pxl_cor_t static_bpc_tmp;
            const void *void_ptr;

            if(p_filters->update_flags.cr.r2y.cr_12_update == 0U) {
                amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_static_bad_pixel !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
                rval |= IK_ERR_0000;
            } else {
                //consider 64 bit compiler, flow data sbp addr(u32) goes on physical addr + offset.
                (void)amba_ik_system_memcpy(&static_bpc_tmp,&p_filters->input_param.static_bpc,sizeof(ik_static_bad_pxl_cor_t));
                void_ptr = amba_ik_system_virt_to_phys(static_bpc_tmp.calib_sbp_info.sbp_buffer);
                (void)amba_ik_system_memcpy(&static_bpc_tmp.calib_sbp_info.sbp_buffer, &void_ptr, sizeof(void*));
                in_sbp.dbp_enable = p_filters->input_param.dynamic_bpc.enable;
                in_sbp.sbp_enable = p_filters->input_param.sbp_enable;
                in_sbp.p_static_bpc = &static_bpc_tmp;
                in_sbp.p_vin_sensor = &p_filters->input_param.window_size_info.vin_sensor;
                out_sbp.p_cr_12 = p_cr->step1.sec2.p_CR_buf_12;
                out_sbp.cr_12_size = CR_SIZE_12;
                out_sbp.p_flow = &p_flow->flow_info;
                rval |= ikc_static_bad_pixel(&in_sbp, &out_sbp);
            }
        }
        p_flow->flow_info.update_sbp = 1U;
    } else {
        p_flow->flow_info.update_sbp = 0U;
    }
    if ((p_filters->update_flags.iso.vignette_compensation_updated == 1U)||
        (p_filters->update_flags.iso.window_size_info_updated == 1U) || (window_calculate_updated == 1U)) {
        ikc_in_vignette_t in_vig;
        ikc_out_vignette_t out_vig;

        if((p_filters->update_flags.cr.r2y.cr_9_update == 0U)||
           (p_filters->update_flags.cr.r2y.cr_10_update == 0U)||
           (p_filters->update_flags.cr.r2y.cr_11_update == 0U)) {
            amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_vignette !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
            rval |= IK_ERR_0000;
        } else {
            in_vig.vig_enable = p_filters->input_param.vig_enable;
            in_vig.flip_mode = p_filters->input_param.flip_mode;
            in_vig.p_vignette_compensation = &p_filters->input_param.vignette_compensation;
            in_vig.p_vin_sensor = &p_filters->input_param.window_size_info.vin_sensor;
            in_vig.p_dmy_win_geo = &p_filters->input_param.ctx_buf.result_win.dmy_win_geo;
            in_vig.p_active_window = &p_filters->input_param.active_window;
            out_vig.p_cr_9 = p_cr->step1.sec2.p_CR_buf_9;
            out_vig.cr_9_size = CR_SIZE_9;
            out_vig.p_cr_10 = p_cr->step1.sec2.p_CR_buf_10;
            out_vig.cr_10_size = CR_SIZE_10;
            out_vig.p_cr_11 = p_cr->step1.sec2.p_CR_buf_11;
            out_vig.cr_11_size = CR_SIZE_11;
            out_vig.p_flow = &p_flow->flow_info;
            rval |= ikc_vignette(&in_vig, &out_vig);
        }
        p_flow->flow_info.update_vignette = 1U;
    } else {
        p_flow->flow_info.update_vignette = 0U;
    }
    if ((p_filters->update_flags.iso.calib_ca_warp_info_updated == 1U) || (p_filters->update_flags.iso.cawarp_enable_updated == 1U) ||
        (p_filters->update_flags.iso.cawarp_internal_updated == 1U) || (p_filters->update_flags.iso.window_size_info_updated == 1U) ||
        (window_calculate_updated == 1U)) {
        if (p_filters->input_param.ctx_buf.cawarp_internal_mode_flag == 1U) {
            ikc_in_cawarp_internal_t in_ca;
            ikc_out_cawarp_t out_ca;

            if((p_filters->update_flags.cr.cawarp == 0U)||
               (p_filters->update_flags.cr.r2y.cr_16_update == 0U)||
               (p_filters->update_flags.cr.r2y.cr_17_update == 0U)) {
                amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_cawarp_internal !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
                rval |= IK_ERR_0000;
            } else {
                in_ca.p_stitching_info = &p_filters->input_param.stitching_info;
                in_ca.p_ca_warp_internal = &p_filters->input_param.ca_warp_internal;
                out_ca.p_cr_16 = p_cr->step1.sec2.p_CR_buf_16;
                out_ca.cr_16_size = CR_SIZE_16;
                out_ca.p_cr_17 = p_cr->step1.sec2.p_CR_buf_17;
                out_ca.cr_17_size = CR_SIZE_17;
                out_ca.p_cr_18 = p_cr->step1.sec2.p_CR_buf_18;
                out_ca.cr_18_size = CR_SIZE_18;
                out_ca.p_cr_19 = p_cr->step1.sec2.p_CR_buf_19;
                out_ca.cr_19_size = CR_SIZE_19;
                out_ca.p_cr_20 = p_cr->step1.sec2.p_CR_buf_20;
                out_ca.cr_20_size = CR_SIZE_20;
                out_ca.p_calib = &p_flow->calib;
                out_ca.p_cawarp_hor_table_addr_red = p_cr->p_ca_warp_hor_red;
                out_ca.h_red_size = CAWARP_VIDEO_MAXSIZE;
                out_ca.p_cawarp_vertical_table_addr_red = p_cr->p_ca_warp_ver_red;
                out_ca.v_red_size = CAWARP_VIDEO_MAXSIZE;
                out_ca.p_cawarp_hor_table_addr_blue = p_cr->p_ca_warp_hor_blue;
                out_ca.h_blue_size = CAWARP_VIDEO_MAXSIZE;
                out_ca.p_cawarp_vertical_table_addr_blue = p_cr->p_ca_warp_ver_blue;
                out_ca.v_blue_size = CAWARP_VIDEO_MAXSIZE;
                rval |= ikc_cawarp_internal(&in_ca, &out_ca);

                p_flow->calib.cawarp_horizontal_table_addr_red = physical2u32(p_cr->p_ca_warp_hor_red);
                p_flow->calib.cawarp_horizontal_table_addr_blue = physical2u32(p_cr->p_ca_warp_hor_blue);
                p_flow->calib.cawarp_vertical_table_addr_red = physical2u32(p_cr->p_ca_warp_ver_red);
                p_flow->calib.cawarp_vertical_table_addr_blue = physical2u32(p_cr->p_ca_warp_ver_blue);
            }
        } else {
            ikc_in_cawarp_t in_ca;
            ikc_out_cawarp_t out_ca;

            if((p_filters->update_flags.cr.cawarp == 0U)||
               (p_filters->update_flags.cr.r2y.cr_16_update == 0U)||
               (p_filters->update_flags.cr.r2y.cr_17_update == 0U)) {
                amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_cawarp !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
                rval |= IK_ERR_0000;
            } else {
                in_ca.is_group_cmd = 0u;
                in_ca.is_hiso      = 1u;
                in_ca.cawarp_enable = (uint32)p_filters->input_param.cawarp_enable;
                in_ca.flip_mode = p_filters->input_param.flip_mode;
                in_ca.p_stitching_info = &p_filters->input_param.stitching_info;
                in_ca.p_window_size_info = &p_filters->input_param.window_size_info;
                in_ca.p_result_win = &p_filters->input_param.ctx_buf.result_win;
                in_ca.p_calib_ca_warp_info = &p_filters->input_param.calib_ca_warp_info;
                out_ca.p_cr_16 = p_cr->step1.sec2.p_CR_buf_16;
                out_ca.cr_16_size = CR_SIZE_16;
                out_ca.p_cr_17 = p_cr->step1.sec2.p_CR_buf_17;
                out_ca.cr_17_size = CR_SIZE_17;
                out_ca.p_cr_18 = p_cr->step1.sec2.p_CR_buf_18;
                out_ca.cr_18_size = CR_SIZE_18;
                out_ca.p_cr_19 = p_cr->step1.sec2.p_CR_buf_19;
                out_ca.cr_19_size = CR_SIZE_19;
                out_ca.p_cr_20 = p_cr->step1.sec2.p_CR_buf_20;
                out_ca.cr_20_size = CR_SIZE_20;
                out_ca.p_calib = &p_flow->calib;
                out_ca.p_cawarp_hor_table_addr_red = p_cr->p_ca_warp_hor_red;
                out_ca.h_red_size = CAWARP_VIDEO_MAXSIZE;
                out_ca.p_cawarp_vertical_table_addr_red = p_cr->p_ca_warp_ver_red;
                out_ca.v_red_size = CAWARP_VIDEO_MAXSIZE;
                out_ca.p_cawarp_hor_table_addr_blue = p_cr->p_ca_warp_hor_blue;
                out_ca.h_blue_size = CAWARP_VIDEO_MAXSIZE;
                out_ca.p_cawarp_vertical_table_addr_blue = p_cr->p_ca_warp_ver_blue;
                out_ca.v_blue_size = CAWARP_VIDEO_MAXSIZE;

                rval |= ikc_cawarp(&in_ca, &out_ca);

                p_flow->calib.cawarp_horizontal_table_addr_red = physical2u32(p_cr->p_ca_warp_hor_red);
                p_flow->calib.cawarp_horizontal_table_addr_blue = physical2u32(p_cr->p_ca_warp_hor_blue);
                p_flow->calib.cawarp_vertical_table_addr_red = physical2u32(p_cr->p_ca_warp_ver_red);
                p_flow->calib.cawarp_vertical_table_addr_blue = physical2u32(p_cr->p_ca_warp_ver_blue);
            }
        }
        p_flow->calib.update_CA_warp = 1U;
    } else {
        p_flow->calib.update_CA_warp = 0U;
    }
    if((p_filters->update_flags.iso.chroma_filter_updated == 1U)||
       (p_filters->update_flags.iso.warp_enable_updated == 1U)||
       (p_filters->update_flags.iso.calib_warp_info_updated == 1U)||
       (p_filters->update_flags.iso.warp_internal_updated == 1U)||
       (p_filters->update_flags.iso.window_size_info_updated == 1U)||
       (window_calculate_updated == 1U)) {
        if(p_filters->input_param.ctx_buf.warp_internal_mode_flag == 1U) { //internal warp
#if 0
            ikc_in_warp_internal_t in_warp_internal;
            ikc_out_hwarp_t out_hwarp;

            if((p_filters->update_flags.cr.warp == 0U)||
               (p_filters->update_flags.cr.r2y.cr_33_update == 0U)||
               (p_filters->update_flags.cr.r2y.cr_34_update == 0U)||
               (p_filters->update_flags.cr.r2y.cr_35_update == 0U)||
               (p_filters->update_flags.cr.r2y.cr_42_update == 0U)||
               (p_filters->update_flags.cr.r2y.cr_43_update == 0U)||
               (p_filters->update_flags.cr.r2y.cr_44_update == 0U)) {
                amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_warp_internal !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
                rval |= IK_ERR_0000;
            } else {
                in_warp_internal.flip_mode = p_filters->input_param.flip_mode;
                in_warp_internal.p_window_size_info = &p_filters->input_param.window_size_info;
                in_warp_internal.p_act_win_crop = &p_filters->input_param.ctx_buf.result_win.act_win_crop;
                in_warp_internal.p_dmy_win_geo = &p_filters->input_param.ctx_buf.result_win.dmy_win_geo;
                in_warp_internal.p_cfa_win_dim = &p_filters->input_param.ctx_buf.result_win.cfa_win_dim;
                in_warp_internal.p_dmy_range = &p_filters->input_param.dmy_range;
                in_warp_internal.chroma_radius = p_filters->input_param.chroma_filter.radius;
                in_warp_internal.p_stitching_info = &p_filters->input_param.stitching_info;
                in_warp_internal.p_warp_internal = &p_filters->input_param.warp_internal;
                in_warp_internal.dram_efficiency = p_filters->input_param.warp_buf_info.dram_efficiency;
                in_warp_internal.luma_wait_lines = p_filters->input_param.warp_buf_info.luma_wait_lines;
                in_warp_internal.luma_dma_size = p_filters->input_param.warp_buf_info.luma_dma_size;
                out_hwarp.p_cr_33 = p_flow_tbl_list->r2y.p_CR_buf_33;
                out_hwarp.cr_33_size = CR_SIZE_33;
                out_hwarp.p_cr_34 = p_flow_tbl_list->r2y.p_CR_buf_34;
                out_hwarp.cr_34_size = CR_SIZE_34;
                out_hwarp.p_cr_35 = p_flow_tbl_list->r2y.p_CR_buf_35;
                out_hwarp.cr_35_size = CR_SIZE_35;
                out_hwarp.p_cr_42 = p_flow_tbl_list->r2y.p_CR_buf_42;
                out_hwarp.cr_42_size = CR_SIZE_42;
                out_hwarp.p_cr_43 = p_flow_tbl_list->r2y.p_CR_buf_43;
                out_hwarp.cr_43_size = CR_SIZE_43;
                out_hwarp.p_cr_44 = p_flow_tbl_list->r2y.p_CR_buf_44;
                out_hwarp.cr_44_size = CR_SIZE_44;
                out_hwarp.p_flow_info = &p_flow->flow_info;
                out_hwarp.p_window = &p_flow->window;
                out_hwarp.p_phase = &p_flow->phase;
                out_hwarp.p_calib = &p_flow->calib;
                out_hwarp.p_stitch = &p_flow->stitch;
                out_hwarp.p_warp_horizontal_table_address = p_flow_tbl_list->r2y.p_warp_hor;
                out_hwarp.h_size = WARP_VIDEO_MAXHORGRID * WARP_VIDEO_MAXVERGRID * 2U * 2U;// multi_slice buffer.
                out_hwarp.p_warp_vertical_table_address = p_flow_tbl_list->r2y.p_warp_ver;
                out_hwarp.v_size = WARP_VIDEO_MAXHORGRID * WARP_VIDEO_MAXVERGRID * 2U * 2U;// multi_slice buffer.
                rval |= ikc_warp_internal(&in_warp_internal, &out_hwarp);
            }
#endif
        } else {
            ikc_in_hwarp_t in_hwarp;
            ikc_out_hwarp_t out_hwarp;
            //ikc_in_vwarp_t in_vwarp;
            //ikc_out_vwarp_t out_vwarp;

            if((p_filters->update_flags.cr.warp == 0U)||
               (p_filters->update_flags.cr.r2y.cr_33_update == 0U)||
               (p_filters->update_flags.cr.r2y.cr_34_update == 0U)||
               (p_filters->update_flags.cr.r2y.cr_35_update == 0U)||
               (p_filters->update_flags.cr.r2y.cr_42_update == 0U)||
               (p_filters->update_flags.cr.r2y.cr_43_update == 0U)||
               (p_filters->update_flags.cr.r2y.cr_44_update == 0U)) {
                amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_hwarp !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
                rval |= IK_ERR_0000;
            } else {
                {
                    in_hwarp.is_group_cmd = 0u;
                    in_hwarp.is_hiso      = 1u;
                    in_hwarp.warp_enable = (uint32)p_filters->input_param.warp_enable;
                    in_hwarp.flip_mode = p_filters->input_param.flip_mode;
                    in_hwarp.p_window_size_info = &p_filters->input_param.window_size_info;
                    in_hwarp.p_act_win_crop = &p_filters->input_param.ctx_buf.result_win.act_win_crop;
                    in_hwarp.p_dmy_win_geo = &p_filters->input_param.ctx_buf.result_win.dmy_win_geo;
                    in_hwarp.p_cfa_win_dim = &p_filters->input_param.ctx_buf.result_win.cfa_win_dim;
                    in_hwarp.p_dmy_range = &p_filters->input_param.dmy_range;
                    in_hwarp.chroma_radius = p_filters->input_param.chroma_filter.radius;
                    in_hwarp.is_y2y_420input = (p_filters->input_param.yuv_mode == AMBA_IK_YUV420) ? 1U : 0U;
                    in_hwarp.p_stitching_info = &p_filters->input_param.stitching_info;
                    in_hwarp.p_calib_warp_info = &p_filters->input_param.calib_warp_info;
                    in_hwarp.dram_efficiency = p_filters->input_param.warp_buf_info.dram_efficiency;
                    in_hwarp.luma_wait_lines = p_filters->input_param.warp_buf_info.luma_wait_lines;
                    in_hwarp.luma_dma_size = p_filters->input_param.warp_buf_info.luma_dma_size;
                    in_hwarp.is_still_422 = 0;
                    out_hwarp.p_cr_33 = p_cr->step1.sec2.p_CR_buf_33;
                    out_hwarp.cr_33_size = CR_SIZE_33;
                    out_hwarp.p_cr_34 = p_cr->step1.sec2.p_CR_buf_34;
                    out_hwarp.cr_34_size = CR_SIZE_34;
                    out_hwarp.p_cr_35 = p_cr->step1.sec2.p_CR_buf_35;
                    out_hwarp.cr_35_size = CR_SIZE_35;
                    out_hwarp.p_flow_info = &p_flow->flow_info;
                    out_hwarp.p_window = &p_flow->window;
                    out_hwarp.p_phase = &p_flow->phase;
                    out_hwarp.p_calib = &p_flow->calib;
                    out_hwarp.p_stitch = &p_flow->stitch;
                    out_hwarp.p_warp_horizontal_table_address = p_cr->p_warp_hor;
                    out_hwarp.h_size = WARP_VIDEO_MAXHORGRID * WARP_VIDEO_MAXVERGRID * 2U * 2U;// multi_slice buffer.

                    rval |= ikc_hwarp(&in_hwarp, &out_hwarp);
                }
            }
        }
        p_flow->calib.update_lens_warp = 1U;
    } else {
        p_flow->calib.update_lens_warp = 0U;
    }

    // exe_process_hdr_ce_cr
    if(p_filters->update_flags.iso.before_ce_wb_gain_updated == 1u) {
        ikc_in_before_ce_gain_t in_before_ce_gain = {0};
        ikc_out_before_ce_gain_t out_before_ce_gain;

        if(p_filters->update_flags.cr.r2y.cr_13_update == 0U) {
            amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_before_ce_gain !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
            rval |= IK_ERR_0000;
        } else {
            in_before_ce_gain.p_before_ce_wb_gain = &p_filters->input_param.before_ce_wb_gain;
            out_before_ce_gain.p_cr_13 = p_cr->step1.sec2.p_CR_buf_13;
            out_before_ce_gain.cr_13_size = CR_SIZE_13;
            rval |= ikc_before_ce_gain(&in_before_ce_gain, &out_before_ce_gain);
        }
    }
    if(p_filters->update_flags.iso.hdr_blend_updated == 1u) {
        ikc_in_hdr_blend_t in_hdr_blend;
        ikc_out_hdr_blend_t out_hdr_blend;

        if(p_filters->update_flags.cr.r2y.cr_4_update == 0U) {
            amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_hdr_blend !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
            rval |= IK_ERR_0000;
        } else {
            in_hdr_blend.exp_num = p_filters->input_param.num_of_exposures;
            in_hdr_blend.p_hdr_raw_info = &p_filters->input_param.hdr_raw_info;
            in_hdr_blend.p_hdr_blend = &p_filters->input_param.hdr_blend;
            out_hdr_blend.p_cr_4 = p_cr->step1.sec2.p_CR_buf_4;
            out_hdr_blend.cr_4_size = CR_SIZE_4;
            out_hdr_blend.p_flow = &p_flow->flow_info;
            rval |= ikc_hdr_blend(&in_hdr_blend, &out_hdr_blend);
        }
    }
    if(p_filters->update_flags.iso.fe_tone_curve_updated == 1u) {
        ikc_in_hdr_tone_curve_t in_hdr_tc;
        ikc_out_hdr_tone_curve_t out_hdr_tc;

        if((p_filters->update_flags.cr.r2y.cr_4_update == 0U)||
           (p_filters->update_flags.cr.r2y.cr_5_update == 0U)) {
            amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_front_end_tone_curve !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
            rval |= IK_ERR_0000;
        } else {
            in_hdr_tc.p_fe_tone_curve = &p_filters->input_param.fe_tone_curve;
            out_hdr_tc.p_cr_4 = p_cr->step1.sec2.p_CR_buf_4;
            out_hdr_tc.cr_4_size = CR_SIZE_4;
            out_hdr_tc.p_cr_5 = p_cr->step1.sec2.p_CR_buf_5;
            out_hdr_tc.cr_5_size = CR_SIZE_5;
            rval |= ikc_front_end_tone_curve(&in_hdr_tc, &out_hdr_tc);
        }
    }
    if((p_filters->update_flags.iso.sensor_information_updated == 1u)||(window_calculate_updated == 1U)) {
        ikc_in_decompress_t in_decompress;
        ikc_out_decompress_t out_decompress;

        if(p_filters->update_flags.cr.r2y.cr_4_update == 0U) {
            amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_decompression !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
            rval |= IK_ERR_0000;
        } else {
            in_decompress.exp_num = 1U;
            in_decompress.compress_mode = p_filters->input_param.sensor_info.compression;////0~8, 256~259.
            in_decompress.p_dmy_win_geo = &p_filters->input_param.ctx_buf.result_win.dmy_win_geo;
            out_decompress.p_cr_4 = p_cr->step1.sec2.p_CR_buf_4;
            out_decompress.cr_4_size = CR_SIZE_4;
            rval |= ikc_decompression(&in_decompress, &out_decompress);
            if(p_filters->update_flags.iso.is_1st_frame == 1u) {
                p_filters->input_param.ctx_buf.first_compression_offset = p_filters->input_param.sensor_info.compression_offset;
            }
        }
    }
    if((p_filters->update_flags.iso.exp0_fe_static_blc_updated == 1u) ||
       (p_filters->update_flags.iso.exp1_fe_static_blc_updated == 1u) ||
       (p_filters->update_flags.iso.exp2_fe_static_blc_updated == 1u)) {
        ikc_in_hdr_blc_t in_hdr_blc;
        ikc_out_hdr_blc_t out_hdr_blc;

        if((p_filters->update_flags.cr.r2y.cr_4_update == 0U)||
           (p_filters->update_flags.cr.r2y.cr_6_update == 0U)||
           (p_filters->update_flags.cr.r2y.cr_7_update == 0U)||
           (p_filters->update_flags.cr.r2y.cr_8_update == 0U)) {
            amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_hdr_black_level !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
            rval |= IK_ERR_0000;
        } else {
            in_hdr_blc.exp_num = p_filters->input_param.num_of_exposures;
            in_hdr_blc.rgb_ir_mode = p_filters->input_param.rgb_ir.mode;
            in_hdr_blc.compression_offset = ((p_filters->input_param.sensor_info.compression > IK_RAW_COMPRESS_OFF)&&(p_filters->input_param.sensor_info.compression <= IK_RAW_COMPRESS_10p5)) ? p_filters->input_param.ctx_buf.first_compression_offset : 0;
            in_hdr_blc.p_exp0_frontend_static_blc = &p_filters->input_param.exp0_frontend_static_blc;
            in_hdr_blc.p_exp1_frontend_static_blc = &p_filters->input_param.exp1_frontend_static_blc;
            in_hdr_blc.p_exp2_frontend_static_blc = &p_filters->input_param.exp2_frontend_static_blc;
            out_hdr_blc.p_cr_4 = p_cr->step1.sec2.p_CR_buf_4;
            out_hdr_blc.cr_4_size = CR_SIZE_4;
            out_hdr_blc.p_cr_6 = p_cr->step1.sec2.p_CR_buf_6;
            out_hdr_blc.cr_6_size = CR_SIZE_6;
            out_hdr_blc.p_cr_7 = p_cr->step1.sec2.p_CR_buf_7;
            out_hdr_blc.cr_7_size = CR_SIZE_7;
            out_hdr_blc.p_cr_8 = p_cr->step1.sec2.p_CR_buf_8;
            out_hdr_blc.cr_8_size = CR_SIZE_8;
            rval |= ikc_hdr_black_level(&in_hdr_blc, &out_hdr_blc);
        }
    }
    if((p_filters->update_flags.iso.exp0_fe_wb_gain_updated == 1u) ||
       (p_filters->update_flags.iso.exp1_fe_wb_gain_updated == 1u) ||
       (p_filters->update_flags.iso.exp2_fe_wb_gain_updated == 1u)) {
        ikc_in_hdr_dgain_t in_hdr_dgain = {0};
        ikc_out_hdr_dgain_t out_hdr_dgain;

        if(p_filters->update_flags.cr.r2y.cr_4_update == 0U) {
            amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_hdr_dgain !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
            rval |= IK_ERR_0000;
        } else {
            in_hdr_dgain.exp_num = p_filters->input_param.num_of_exposures;
            in_hdr_dgain.rgb_ir_mode = p_filters->input_param.rgb_ir.mode;
            in_hdr_dgain.p_exp0_frontend_wb_gain = &p_filters->input_param.exp0_frontend_wb_gain;
            in_hdr_dgain.p_exp1_frontend_wb_gain = &p_filters->input_param.exp1_frontend_wb_gain;
            in_hdr_dgain.p_exp2_frontend_wb_gain = &p_filters->input_param.exp2_frontend_wb_gain;
            in_hdr_dgain.p_exp0_frontend_static_blc = &p_filters->input_param.exp0_frontend_static_blc;
            in_hdr_dgain.p_exp1_frontend_static_blc = &p_filters->input_param.exp1_frontend_static_blc;
            in_hdr_dgain.p_exp2_frontend_static_blc = &p_filters->input_param.exp2_frontend_static_blc;
            out_hdr_dgain.p_cr_4 = p_cr->step1.sec2.p_CR_buf_4;
            out_hdr_dgain.cr_4_size = CR_SIZE_4;
            rval |= ikc_hdr_dgain(&in_hdr_dgain, &out_hdr_dgain);
        }
    }
    if((p_filters->update_flags.iso.ce_updated == 1u)||(window_calculate_updated == 1U)) {
        ikc_in_ce_t in_ce;
        ikc_out_ce_t out_ce;

        if(p_filters->update_flags.cr.r2y.cr_13_update == 0U) {
            amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_contrast_enhancement !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
            rval |= IK_ERR_0000;
        } else {
            in_ce.exp_num = p_filters->input_param.num_of_exposures;
            in_ce.sensor_pattern = p_filters->input_param.sensor_info.sensor_pattern;
            in_ce.p_dmy_win_geo = &p_filters->input_param.ctx_buf.result_win.dmy_win_geo;
            in_ce.p_ce = &p_filters->input_param.ce;
            out_ce.p_cr_13 = p_cr->step1.sec2.p_CR_buf_13;
            out_ce.cr_13_size = CR_SIZE_13;
            out_ce.p_flow = &p_flow->flow_info;
            rval |= ikc_contrast_enhancement(&in_ce, &out_ce);
        }
    }
    if(p_filters->update_flags.iso.ce_input_table_updated == 1u) {
        ikc_in_ce_input_t in_ce_input;
        ikc_out_ce_input_t out_ce_input;

        if((p_filters->update_flags.cr.r2y.cr_13_update == 0U)||
           (p_filters->update_flags.cr.r2y.cr_14_update == 0U)) {
            amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_contrast_enhancement_input !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
            rval |= IK_ERR_0000;
        } else {
            in_ce_input.ce_enable = p_filters->input_param.ce.enable;
            in_ce_input.p_ce_input_table = &p_filters->input_param.ce_input_table;
            out_ce_input.p_cr_13 = p_cr->step1.sec2.p_CR_buf_13;
            out_ce_input.cr_13_size = CR_SIZE_13;
            out_ce_input.p_cr_14 = p_cr->step1.sec2.p_CR_buf_14;
            out_ce_input.cr_14_size = CR_SIZE_14;
            rval |= ikc_contrast_enhancement_input(&in_ce_input, &out_ce_input);
        }
    }
    if(p_filters->update_flags.iso.ce_out_table_updated == 1u) {
        ikc_in_ce_output_t in_ce_output;
        ikc_out_ce_t out_ce;

        if(p_filters->update_flags.cr.r2y.cr_13_update == 0U) {
            amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_contrast_enhancement_output !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
            rval |= IK_ERR_0000;
        } else {
            in_ce_output.ce_enable = p_filters->input_param.ce.enable;
            in_ce_output.p_ce_out_table = &p_filters->input_param.ce_out_table;
            out_ce.p_cr_13 = p_cr->step1.sec2.p_CR_buf_13;
            out_ce.cr_13_size = CR_SIZE_13;
            rval |= ikc_contrast_enhancement_output(&in_ce_output, &out_ce);
        }
    }

    // exe_process_cfa_cr
    if(p_filters->update_flags.iso.hi_dynamic_bpc_updated == 1u) {
        ikc_in_dynamic_bad_pixel_t in_dbp;
        ikc_out_dynamic_bad_pixel_t out_dbp;

        if((p_filters->update_flags.cr.r2y.cr_11_update == 0U)||
           (p_filters->update_flags.cr.r2y.cr_12_update == 0U)) {
            amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_dynamic_bad_pixel !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
            rval |= IK_ERR_0000;
        } else {
            if(p_filters->input_param.ctx_buf.sbp_internal_mode_flag == 1u) {
                in_dbp.sbp_enable = p_filters->input_param.sbp_internal.enable;
            } else {
                in_dbp.sbp_enable = p_filters->input_param.sbp_enable;
            }
            if((p_filters->input_param.rgb_ir.ir_only==1U)&&(p_filters->input_param.sensor_info.sensor_mode==1U)) { //ir_only, runing on RGB bayer pattern, carry IR on B channel.
                in_dbp.rgb_ir_mode = 0u;
            } else {
                in_dbp.rgb_ir_mode = p_filters->input_param.rgb_ir.mode;
            }
            in_dbp.p_dbp = &p_filters->hiso_input_param.hi_dynamic_bpc;
            out_dbp.p_cr_11 = p_cr->step1.sec2.p_CR_buf_11;
            out_dbp.cr_11_size = CR_SIZE_11;
            out_dbp.p_cr_12 = p_cr->step1.sec2.p_CR_buf_12;
            out_dbp.cr_12_size = CR_SIZE_12;
            rval |= ikc_dynamic_bad_pixel(&in_dbp, &out_dbp);
        }
    }
    if(p_filters->update_flags.iso.stored_ir_updated == 1u) {
        ikc_in_stored_ir_t in_stored_ir;
        ikc_out_stored_ir_t out_stored_ir;

        if(p_filters->update_flags.cr.r2y.cr_12_update == 0U) {
            amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_stored_ir !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
            rval |= IK_ERR_0000;
        } else {
            in_stored_ir.p_stored_ir = &p_filters->input_param.stored_ir;
            out_stored_ir.p_cr_12 = p_cr->step1.sec2.p_CR_buf_12;
            out_stored_ir.cr_12_size = CR_SIZE_12;
            rval |= ikc_stored_ir(&in_stored_ir, &out_stored_ir);
        }
    }
    if(p_filters->update_flags.iso.hi_grgb_mismatch_updated == 1u) {
        ikc_in_grgb_mismatch_t in_grgb;
        ikc_out_grgb_mismatch_t out_grgb;

        if(p_filters->update_flags.cr.r2y.cr_23_update == 0U) {
            amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_grgb_mismatch !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
            rval |= IK_ERR_0000;
        } else {
            in_grgb.p_grgb_mismatch = &p_filters->hiso_input_param.hi_grgb_mismatch;
            out_grgb.p_cr_23 = p_cr->step1.sec2.p_CR_buf_23;
            out_grgb.cr_23_size = CR_SIZE_23;
            rval |= ikc_grgb_mismatch(&in_grgb, &out_grgb);
        }
    }
    if((p_filters->update_flags.iso.hi_cfa_leakage_filter_updated == 1u) || (p_filters->update_flags.iso.hi_anti_aliasing_updated == 1u)) {
        ikc_in_cfa_leak_anti_alias_t in_cfa_leak_anti_alias;
        ikc_out_cfa_leak_anti_alias_t out_cfa_leak_anti_alias;

        if(p_filters->update_flags.cr.r2y.cr_12_update == 0U) {
            amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_cfa_leakage_anti_aliasing !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
            rval |= IK_ERR_0000;
        } else {
            in_cfa_leak_anti_alias.p_cfa_leakage_filter = &p_filters->hiso_input_param.hi_cfa_leakage_filter;
            in_cfa_leak_anti_alias.p_anti_aliasing = &p_filters->hiso_input_param.hi_anti_aliasing;
            out_cfa_leak_anti_alias.p_cr_12 = p_cr->step1.sec2.p_CR_buf_12;
            out_cfa_leak_anti_alias.cr_12_size = CR_SIZE_12;
            rval |= ikc_cfa_leakage_anti_aliasing(&in_cfa_leak_anti_alias, &out_cfa_leak_anti_alias);
        }
    }
    if(p_filters->update_flags.iso.hi_cfa_noise_filter_updated == 1u) {
        ikc_in_cfa_noise_t in_cfa_noise;
        ikc_out_cfa_noise_t out_cfa_noise;

        if(p_filters->update_flags.cr.r2y.cr_23_update == 0U) {
            amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_cfa_noise !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
            rval |= IK_ERR_0000;
        } else {
            in_cfa_noise.p_cfa_noise_filter = &p_filters->hiso_input_param.hi_cfa_noise_filter;
            out_cfa_noise.p_cr_23 = p_cr->step1.sec2.p_CR_buf_23;
            out_cfa_noise.cr_23_size = CR_SIZE_23;
            rval |= ikc_cfa_noise(&in_cfa_noise, &out_cfa_noise);
        }
    }
    if(p_filters->update_flags.iso.after_ce_wb_gain_updated == 1u) {
        ikc_in_after_ce_gain_t in_after_ce_gain;
        ikc_out_after_ce_gain_t out_after_ce_gain;

        if(p_filters->update_flags.cr.r2y.cr_22_update == 0U) {
            amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_after_ce_gain !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
            rval |= IK_ERR_0000;
        } else {
            in_after_ce_gain.p_after_ce_wb_gain = &p_filters->input_param.after_ce_wb_gain;
            out_after_ce_gain.p_cr_22 = p_cr->step1.sec2.p_CR_buf_22;
            out_after_ce_gain.cr_22_size = CR_SIZE_22;
            rval |= ikc_after_ce_gain(&in_after_ce_gain, &out_after_ce_gain);
        }
    }
    if(p_filters->update_flags.iso.rgb_ir_updated == 1u) {
        ikc_in_rgb_ir_t in_rgb_ir;
        ikc_out_rgb_ir_t out_rgb_ir;

        if((p_filters->update_flags.cr.r2y.cr_4_update == 0U)||
           (p_filters->update_flags.cr.r2y.cr_6_update == 0U)||
           (p_filters->update_flags.cr.r2y.cr_7_update == 0U)||
           (p_filters->update_flags.cr.r2y.cr_8_update == 0U)||
           (p_filters->update_flags.cr.r2y.cr_9_update == 0U)||
           (p_filters->update_flags.cr.r2y.cr_11_update == 0U)||
           (p_filters->update_flags.cr.r2y.cr_12_update == 0U)||
           (p_filters->update_flags.cr.r2y.cr_21_update == 0U)) {
            amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_rgb_ir !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
            rval |= IK_ERR_0000;
        } else {
            in_rgb_ir.p_rgb_ir = &p_filters->input_param.rgb_ir;
            in_rgb_ir.p_before_ce_wb_gain = &p_filters->input_param.before_ce_wb_gain;
            in_rgb_ir.p_after_ce_wb_gain = &p_filters->input_param.after_ce_wb_gain;
            in_rgb_ir.p_exp0_frontend_wb_gain = &p_filters->input_param.exp0_frontend_wb_gain;
            out_rgb_ir.p_cr_4 = p_cr->step1.sec2.p_CR_buf_4;
            out_rgb_ir.cr_4_size = CR_SIZE_4;
            out_rgb_ir.p_cr_6 = p_cr->step1.sec2.p_CR_buf_6;
            out_rgb_ir.cr_6_size = CR_SIZE_6;
            out_rgb_ir.p_cr_7 = p_cr->step1.sec2.p_CR_buf_7;
            out_rgb_ir.cr_7_size = CR_SIZE_7;
            out_rgb_ir.p_cr_8 = p_cr->step1.sec2.p_CR_buf_8;
            out_rgb_ir.cr_8_size = CR_SIZE_8;
            out_rgb_ir.p_cr_9 = p_cr->step1.sec2.p_CR_buf_9;
            out_rgb_ir.cr_9_size = CR_SIZE_9;
            out_rgb_ir.p_cr_11 = p_cr->step1.sec2.p_CR_buf_11;
            out_rgb_ir.cr_11_size = CR_SIZE_11;
            out_rgb_ir.p_cr_12 = p_cr->step1.sec2.p_CR_buf_12;
            out_rgb_ir.cr_12_size = CR_SIZE_12;
            out_rgb_ir.p_cr_21 = p_cr->step1.sec2.p_CR_buf_21;
            out_rgb_ir.cr_21_size = CR_SIZE_21;
            out_rgb_ir.p_flow = &p_flow->flow_info;
            rval |= ikc_rgb_ir(&in_rgb_ir, &out_rgb_ir);
        }
    }
    if ((p_filters->update_flags.iso.resample_str_update== 1U) ||
        (p_filters->update_flags.iso.cawarp_enable_updated == 1U)||
        (window_calculate_updated == 1U)) {
        ikc_in_cfa_prescale_t in_cfa_prescale;
        ikc_out_cfa_prescale_t out_cfa_prescale;

        if(p_filters->update_flags.cr.r2y.cr_16_update == 0U) {
            amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_cfa_prescale !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
            rval |= IK_ERR_0000;
        } else {
            in_cfa_prescale.p_flow = &p_flow->flow_info;
            in_cfa_prescale.cfa_cut_off_freq = p_filters->input_param.resample_str.cfa_cut_off_freq;
            in_cfa_prescale.p_dmy_win_geo = &p_filters->input_param.ctx_buf.result_win.dmy_win_geo;
            in_cfa_prescale.p_cfa_win_dim = &p_filters->input_param.ctx_buf.result_win.cfa_win_dim;
            in_cfa_prescale.cawarp_enable = (uint32)p_filters->input_param.cawarp_enable;
            out_cfa_prescale.p_cr_16 = p_cr->step1.sec2.p_CR_buf_16;
            out_cfa_prescale.cr_16_size = CR_SIZE_16;
            rval |= ikc_cfa_prescale(&in_cfa_prescale, &out_cfa_prescale);
        }
    }
    if((p_filters->update_flags.iso.aaa_stat_info_updated == 1u) ||
       (p_filters->update_flags.iso.af_stat_ex_info_updated == 1u) ||
       (p_filters->update_flags.iso.pg_af_stat_ex_info_updated == 1u) ||
       (p_filters->update_flags.iso.histogram_info_update == 1u) ||
       (p_filters->update_flags.iso.histogram_info_pg_update == 1u) ||
       (p_filters->update_flags.iso.window_size_info_updated == 1U)||
       (window_calculate_updated == 1U)) {
        ikc_in_aaa_t in_aaa;
        ikc_out_aaa_t out_aaa;

        if((p_filters->update_flags.cr.aaa_data == 0U)||
           (p_filters->update_flags.cr.r2y.cr_9_update == 0U)||
           (p_filters->update_flags.cr.r2y.cr_12_update == 0U)||
           (p_filters->update_flags.cr.r2y.cr_21_update == 0U)||
           (p_filters->update_flags.cr.r2y.cr_28_update == 0U)) {
            amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_aaa !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
            rval |= IK_ERR_0000;
        } else {
            in_aaa.p_sensor_info = &p_filters->input_param.sensor_info;//determine RGB-IR.
            in_aaa.p_rgb_ir = &p_filters->input_param.rgb_ir;
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
            out_aaa.p_cr_9 = p_cr->step1.sec2.p_CR_buf_9;
            out_aaa.cr_9_size = CR_SIZE_9;
            out_aaa.p_cr_12 = p_cr->step1.sec2.p_CR_buf_12;
            out_aaa.cr_12_size = CR_SIZE_12;
            out_aaa.p_cr_21 = p_cr->step1.sec2.p_CR_buf_21;
            out_aaa.cr_21_size = CR_SIZE_21;
            out_aaa.p_cr_28 = p_cr->step1.sec2.p_CR_buf_28;
            out_aaa.cr_28_size = CR_SIZE_28;
            out_aaa.p_stitching_aaa = p_cr->p_aaa;

            if ((p_filters->input_param.ability == AMBA_IK_VIDEO_PIPE_Y2Y) ||
                (p_filters->input_param.ability == AMBA_IK_VIDEO_PIPE_Y2Y_HVH) ||
                (p_filters->input_param.ability == AMBA_IK_VIDEO_PIPE_Y2Y_HHB) ||
                (p_filters->input_param.ability == AMBA_IK_VIDEO_PIPE_Y2Y_MD) ||
                (p_filters->input_param.ability == AMBA_IK_VIDEO_PIPE_Y2Y_MD_HHB)) {
                in_aaa.is_yuv_mode = 1U;
            } else {
                in_aaa.is_yuv_mode = 0U;
            }
            rval |= ikc_aaa(&in_aaa, &out_aaa);
        }
    }

    // exe_process_rgb_cr
    if(p_filters->update_flags.iso.hi_demosaic_updated == 1U) {
        ikc_in_demosaic_t in_demosaic;
        ikc_out_demosaic_t out_demosaic;

        if((p_filters->update_flags.cr.r2y.cr_12_update == 0U)||
           (p_filters->update_flags.cr.r2y.cr_23_update == 0U)||
           (p_filters->update_flags.cr.r2y.cr_24_update == 0U)) {
            amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_demosaic_filter !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
            rval |= IK_ERR_0000;
        } else {
            in_demosaic.p_demosaic = &p_filters->hiso_input_param.hi_demosaic;
            out_demosaic.p_cr_12 = p_cr->step1.sec2.p_CR_buf_12;
            out_demosaic.cr_12_size = CR_SIZE_12;
            out_demosaic.p_cr_23 = p_cr->step1.sec2.p_CR_buf_23;
            out_demosaic.cr_23_size = CR_SIZE_23;
            out_demosaic.p_cr_24 = p_cr->step1.sec2.p_CR_buf_24;
            out_demosaic.cr_24_size = CR_SIZE_24;
            rval |= ikc_demosaic_filter(&in_demosaic, &out_demosaic);
        }
    }
    if(p_filters->update_flags.iso.rgb_to_12y_updated == 1U) {
        ikc_in_rgb12y_t in_rgb12y;
        ikc_out_rgb12y_t out_rgb12y;

        if(p_filters->update_flags.cr.r2y.cr_25_update == 0U) {
            amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_rgb12y !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
            rval |= IK_ERR_0000;
        } else {
            in_rgb12y.p_rgb_to_12y = &p_filters->input_param.rgb_to_12y;
            out_rgb12y.p_cr_25 = p_cr->step1.sec2.p_CR_buf_25;
            out_rgb12y.cr_25_size = CR_SIZE_25;
            rval |= ikc_rgb12y(&in_rgb12y, &out_rgb12y);
        }
    }
    if(p_filters->update_flags.iso.is_1st_frame == 1U) {//should only configure once in 1st time, due to R2Y always turn on.
        ikc_in_cc_en_t in_cc_enb;
        ikc_out_cc_en_t out_cc_enb;

        if(p_filters->update_flags.cr.r2y.cr_26_update == 0U) {
            amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_color_correction_enable !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
            rval |= IK_ERR_0000;
        } else {
            in_cc_enb.is_yuv_mode = 0;
            in_cc_enb.use_cc_for_yuv2yuv = 0;
            out_cc_enb.p_cr_26 = p_cr->step1.sec2.p_CR_buf_26;
            out_cc_enb.cr_26_size = CR_SIZE_26;
            rval |= ikc_color_correction_enable(&in_cc_enb, &out_cc_enb);
        }
    }
    if(p_filters->update_flags.iso.color_correction_updated == 1U) {
        ikc_in_cc_t in_cc_3d;
        ikc_out_cc_t out_cc_3d;

        if((p_filters->update_flags.cr.r2y.cr_26_update == 0U)||
           (p_filters->update_flags.cr.r2y.cr_27_update == 0U)) {
            amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_color_correction !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
            rval |= IK_ERR_0000;
        } else {
            in_cc_3d.p_color_correction = &p_filters->input_param.color_correction;
            out_cc_3d.p_cr_26 = p_cr->step1.sec2.p_CR_buf_26;
            out_cc_3d.cr_26_size = CR_SIZE_26;
            out_cc_3d.p_cr_27 = p_cr->step1.sec2.p_CR_buf_27;
            out_cc_3d.cr_27_size = CR_SIZE_27;
            rval |= ikc_color_correction(&in_cc_3d, &out_cc_3d);
        }
    }
    if(p_filters->update_flags.iso.tone_curve_updated == 1U) {
        ikc_in_cc_out_t in_cc_out;
        ikc_out_cc_out_t out_cc_out;

        if(p_filters->update_flags.cr.r2y.cr_26_update == 0U) {
            amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_color_correction_out !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
            rval |= IK_ERR_0000;
        } else {
            in_cc_out.p_tone_curve = &p_filters->input_param.tone_curve;
            out_cc_out.p_cr_26 = p_cr->step1.sec2.p_CR_buf_26;
            out_cc_out.cr_26_size = CR_SIZE_26;
            rval |= ikc_color_correction_out(&in_cc_out, &out_cc_out);
        }
    }
    if((p_filters->update_flags.iso.rgb_to_yuv_matrix_updated == 1U) ||
       (p_filters->update_flags.iso.is_1st_frame == 1U)) {
        ikc_in_rgb2yuv_t in_rgb2yuv = {0};
        ikc_out_rgb2yuv_t out_rgb2yuv;

        if(p_filters->update_flags.cr.r2y.cr_29_update == 0U) {
            amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_rgb2yuv !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
            rval |= IK_ERR_0000;
        } else {
            in_rgb2yuv.p_rgb_to_yuv_matrix = &p_filters->input_param.rgb_to_yuv_matrix;
            out_rgb2yuv.p_cr_29 = p_cr->step1.sec2.p_CR_buf_29;
            out_rgb2yuv.cr_29_size = CR_SIZE_29;
            rval |= ikc_rgb2yuv(&in_rgb2yuv, &out_rgb2yuv);
        }
    }

    // exe_process_yuv_cr
    if(p_filters->update_flags.iso.chroma_scale_updated == 1U) {
        ikc_in_chroma_scale_t in_chroma_scale;
        ikc_out_chroma_scale_t out_chroma_scale;

        if(p_filters->update_flags.cr.r2y.cr_29_update == 0U) {
            amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_chroma_scale !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
            rval |= IK_ERR_0000;
        } else {
            if ((p_filters->input_param.ability == AMBA_IK_VIDEO_PIPE_Y2Y) ||
                (p_filters->input_param.ability == AMBA_IK_VIDEO_PIPE_Y2Y_HVH) ||
                (p_filters->input_param.ability == AMBA_IK_VIDEO_PIPE_Y2Y_HHB) ||
                (p_filters->input_param.ability == AMBA_IK_VIDEO_PIPE_Y2Y_MD) ||
                (p_filters->input_param.ability == AMBA_IK_VIDEO_PIPE_Y2Y_MD_HHB)) {
                in_chroma_scale.is_yuv_mode = 1U;
            } else {
                in_chroma_scale.is_yuv_mode = 0U;
            }
            in_chroma_scale.p_chroma_scale = &p_filters->input_param.chroma_scale;
            out_chroma_scale.p_cr_29 = p_cr->step1.sec2.p_CR_buf_29;
            out_chroma_scale.cr_29_size = CR_SIZE_29;
            rval |= ikc_chroma_scale(&in_chroma_scale, &out_chroma_scale);
        }
    }
    if(p_filters->update_flags.iso.hi_chroma_median_filter_updated == 1U) {
        ikc_in_chroma_median_t in_chroma_median;
        ikc_out_chroma_median_t out_chroma_median;

        if((p_filters->update_flags.cr.r2y.cr_30_update == 0U)||
           (p_filters->update_flags.cr.r2y.cr_31_update == 0U)) {
            amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_chroma_median !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
            rval |= IK_ERR_0000;
        } else {
            if ((p_filters->input_param.ability == AMBA_IK_VIDEO_PIPE_Y2Y) ||
                (p_filters->input_param.ability == AMBA_IK_VIDEO_PIPE_Y2Y_HVH) ||
                (p_filters->input_param.ability == AMBA_IK_VIDEO_PIPE_Y2Y_HHB) ||
                (p_filters->input_param.ability == AMBA_IK_VIDEO_PIPE_Y2Y_MD) ||
                (p_filters->input_param.ability == AMBA_IK_VIDEO_PIPE_Y2Y_MD_HHB)) {
                in_chroma_median.is_yuv_mode = 1U;
            } else {
                in_chroma_median.is_yuv_mode = 0U;
            }
            in_chroma_median.p_chroma_median_filter = &p_filters->hiso_input_param.hi_chroma_median_filter;
            out_chroma_median.p_cr_30 = p_cr->step1.sec2.p_CR_buf_30;
            out_chroma_median.cr_30_size = CR_SIZE_30;
            out_chroma_median.p_cr_31 = p_cr->step1.sec2.p_CR_buf_31;
            out_chroma_median.cr_31_size = CR_SIZE_31;
            rval |= ikc_chroma_median(&in_chroma_median, &out_chroma_median);
        }
    }
    if((p_filters->update_flags.iso.chroma_filter_updated == 1U) || (p_filters->update_flags.iso.wide_chroma_filter == 1U) ||
       (p_filters->update_flags.iso.wide_chroma_filter_combine == 1U) || (window_calculate_updated == 1U)) {
        ikc_in_chroma_noise_t in_chroma_noise;
        ikc_out_chroma_noise_t out_chroma_noise;
        ik_chroma_filter_t chroma_filter;
        uint64 sec2_luma_hscale_phase_inc = p_filters->input_param.ctx_buf.result_win.warp_phase.sec2_hscale_phase_inc;
        uint64 sec2_luma_vscale_phase_inc = p_filters->input_param.ctx_buf.result_win.warp_phase.sec2_vscale_phase_inc;
        uint32 chroma_radius = p_filters->input_param.chroma_filter.radius;
        uint64 sec2_chroma_hscale_phase_inc;
        uint64 sec2_chroma_vscale_phase_inc;

        if(p_filters->update_flags.cr.r2y.cr_36_update == 0U) {
            amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_chroma_noise !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
            rval |= IK_ERR_0000;
        } else {
            amba_ik_system_memcpy(&chroma_filter, &p_filters->hiso_input_param.hi_chroma_filter_pre, sizeof(ik_hi_chroma_filter_pre_t));
            chroma_filter.radius = p_filters->hiso_input_param.hi_chroma_filter_high.radius;
            in_chroma_noise.p_chroma_filter = &chroma_filter;
            in_chroma_noise.p_wide_chroma_filter  = &p_filters->hiso_input_param.hi_wide_chroma_filter_pre;
            in_chroma_noise.p_wide_chroma_combine = &p_filters->hiso_input_param.hi_wide_chroma_filter_pre_combine;
            if ((in_chroma_noise.p_chroma_filter->radius==32u) && (in_chroma_noise.p_wide_chroma_filter->enable==1u)) {
                in_chroma_noise.is_wide_chroma_invalid = 1u;
            } else {
                in_chroma_noise.is_wide_chroma_invalid = 0u;
            }
            out_chroma_noise.p_cr_36 = p_cr->step1.sec2.p_CR_buf_36;
            out_chroma_noise.cr_36_size = CR_SIZE_36;
            out_chroma_noise.p_window = &p_flow->window;
            rval |= ikc_chroma_noise(&in_chroma_noise, &out_chroma_noise);
            p_flow->window.update_chroma_radius = 1U;
            //check radius causes phase_inc overflow.
            sec2_chroma_hscale_phase_inc = (sec2_luma_hscale_phase_inc * (uint64)chroma_radius) >> 5UL;
            sec2_chroma_vscale_phase_inc = (sec2_luma_vscale_phase_inc * (uint64)chroma_radius) >> 5UL;
            if ((sec2_chroma_hscale_phase_inc >= (8UL<<13UL)) || (sec2_chroma_vscale_phase_inc >= (8UL<<13UL))) {
                amba_ik_system_print_uint32_5("[IK][ERROR] exe_iso_cfg_check_raw2yuv_chroma_filter() fail. One or more downscaling_factor is out of range(>=8<<13), please check chroma radius or window setting!", DC_U, DC_U, DC_U, DC_U, DC_U);
                amba_ik_system_print_uint32_5("[IK][ERROR] sec2_chroma_hscale_phase_inc:%d, sec2_chroma_vscale_phase_inc:%d, chroma_radius:%d", (uint32)sec2_chroma_hscale_phase_inc, (uint32)sec2_chroma_vscale_phase_inc, chroma_radius, DC_U, DC_U);
                rval = IK_ERR_0101;
            }
        }
    } else {
        p_flow->window.update_chroma_radius = 0U;
    }
    if((p_filters->update_flags.iso.chroma_filter_updated == 1U)||
       (p_filters->update_flags.iso.resample_str_update == 1U)||
       (p_filters->update_flags.iso.window_size_info_updated == 1U)||
       (window_calculate_updated == 1U)) {
        if((p_filters->update_flags.cr.r2y.cr_33_update == 0U)||
           (p_filters->update_flags.cr.r2y.cr_35_update == 0U)||
           (p_filters->update_flags.cr.r2y.cr_37_update == 0U)) {
            amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_sec2_main_resampler !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
            rval |= IK_ERR_0000;
        } else {
            {
                ikc_in_sec2_main_resampler_t in_sec2_main_resampler;
                ikc_out_sec2_main_resampler_t out_sec2_main_resampler;

                in_sec2_main_resampler.main_cut_off_freq = p_filters->input_param.resample_str.main_cut_off_freq;
                in_sec2_main_resampler.p_act_win_crop = &p_filters->input_param.ctx_buf.result_win.act_win_crop;
                in_sec2_main_resampler.p_dmy_win_geo = &p_filters->input_param.ctx_buf.result_win.dmy_win_geo;
                in_sec2_main_resampler.p_cfa_win_dim = &p_filters->input_param.ctx_buf.result_win.cfa_win_dim;
                in_sec2_main_resampler.p_main = &p_filters->input_param.window_size_info.main_win;
                in_sec2_main_resampler.p_dmy_range = &p_filters->input_param.dmy_range;
                in_sec2_main_resampler.chroma_radius = 32;//p_filters->input_param.chroma_filter.radius;
                in_sec2_main_resampler.is_y2y_420input = (p_filters->input_param.yuv_mode == AMBA_IK_YUV420) ? 1U : 0U;
                in_sec2_main_resampler.flip_mode = p_filters->input_param.flip_mode;
                in_sec2_main_resampler.luma_dma_size = p_filters->input_param.warp_buf_info.luma_dma_size;
                out_sec2_main_resampler.p_cr_33 = p_cr->step1.sec2.p_CR_buf_33;
                out_sec2_main_resampler.cr_33_size = CR_SIZE_33;
                out_sec2_main_resampler.p_cr_35 = p_cr->step1.sec2.p_CR_buf_35;
                out_sec2_main_resampler.cr_35_size = CR_SIZE_35;
                out_sec2_main_resampler.p_cr_37 = p_cr->step1.sec2.p_CR_buf_37;
                out_sec2_main_resampler.cr_37_size = CR_SIZE_37;
                rval |= ikc_sec2_main_resampler(&in_sec2_main_resampler, &out_sec2_main_resampler);
            }
        }
    }

    // exe_process_hdr_ce_sub_cr
    if((p_filters->update_flags.iso.is_1st_frame == 1U) && (p_filters->update_flags.iso.sensor_information_updated == 1u)) { //should only configure once at 1st time.
        ikc_in_input_mode_sub_t in_input_mode_sub;
        ikc_out_input_mode_sub_t out_input_mode_sub;

        if((p_filters->update_flags.cr.r2y.cr_45_update == 0U)||
           (p_filters->update_flags.cr.r2y.cr_47_update == 0U)||
           (p_filters->update_flags.cr.r2y.cr_49_update == 0U)||
           (p_filters->update_flags.cr.r2y.cr_50_update == 0U)||
           (p_filters->update_flags.cr.r2y.cr_51_update == 0U)) {
            amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_input_mode_sub !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
            rval |= IK_ERR_0000;
        } else {
            in_input_mode_sub.flip_mode = p_filters->input_param.flip_mode;
            in_input_mode_sub.is_yuv_mode = 0u;
            in_input_mode_sub.sensor_pattern = p_filters->input_param.sensor_info.sensor_pattern;
            out_input_mode_sub.p_cr_45 = p_cr->step1.sec4.p_CR_buf_45;
            out_input_mode_sub.cr_45_size = CR_SIZE_45;
            out_input_mode_sub.p_cr_47 = p_cr->step1.sec4.p_CR_buf_47;
            out_input_mode_sub.cr_47_size = CR_SIZE_47;
            out_input_mode_sub.p_cr_49 = p_cr->step1.sec4.p_CR_buf_49;
            out_input_mode_sub.cr_49_size = CR_SIZE_49;
            out_input_mode_sub.p_cr_50 = p_cr->step1.sec4.p_CR_buf_50;
            out_input_mode_sub.cr_50_size = CR_SIZE_50;
            out_input_mode_sub.p_cr_51 = p_cr->step1.sec4.p_CR_buf_51;
            out_input_mode_sub.cr_51_size = CR_SIZE_51;

            rval |= ikc_input_mode_sub(&in_input_mode_sub, &out_input_mode_sub);
        }
    }
    if(p_filters->update_flags.iso.before_ce_wb_gain_updated == 1u) {
        ikc_in_before_ce_gain_t in_before_ce_gain;
        ikc_out_before_ce_gain_sub_t out_before_ce_gain_sub;

        if(p_filters->update_flags.cr.r2y.cr_51_update == 0U) {
            amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_before_ce_gain_sub !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
            rval |= IK_ERR_0000;
        } else {
            in_before_ce_gain.p_before_ce_wb_gain = &p_filters->input_param.before_ce_wb_gain;
            out_before_ce_gain_sub.p_cr_51 = p_cr->step1.sec4.p_CR_buf_51;
            out_before_ce_gain_sub.cr_51_size = CR_SIZE_51;
            rval |= ikc_before_ce_gain_sub(&in_before_ce_gain, &out_before_ce_gain_sub);
        }
    }
    if(p_filters->update_flags.iso.hdr_blend_updated == 1u) {
        ikc_in_hdr_blend_t in_hdr_blend;
        ikc_out_hdr_blend_sub_t out_hdr_blend_sub;

        if(p_filters->update_flags.cr.r2y.cr_45_update == 0U) {
            amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_hdr_blend_sub !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
            rval |= IK_ERR_0000;
        } else {
            in_hdr_blend.exp_num = p_filters->input_param.num_of_exposures;
            in_hdr_blend.p_hdr_raw_info = &p_filters->input_param.hdr_raw_info;
            in_hdr_blend.p_hdr_blend = &p_filters->input_param.hdr_blend;
            out_hdr_blend_sub.p_cr_45 = p_cr->step1.sec4.p_CR_buf_45;
            out_hdr_blend_sub.cr_45_size = CR_SIZE_45;
            rval |= ikc_hdr_blend_sub(&in_hdr_blend, &out_hdr_blend_sub);
        }
    }
    if(p_filters->update_flags.iso.fe_tone_curve_updated == 1u) {
        ikc_in_hdr_tone_curve_t in_hdr_tc;
        ikc_out_hdr_tone_curve_sub_t out_hdr_tc_sub;

        if((p_filters->update_flags.cr.r2y.cr_45_update == 0U)||
           (p_filters->update_flags.cr.r2y.cr_46_update == 0U)) {
            amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_front_end_tone_curve_sub !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
            rval |= IK_ERR_0000;
        } else {
            in_hdr_tc.p_fe_tone_curve = &p_filters->input_param.fe_tone_curve;
            out_hdr_tc_sub.p_cr_45 = p_cr->step1.sec4.p_CR_buf_45;
            out_hdr_tc_sub.cr_45_size = CR_SIZE_45;
            out_hdr_tc_sub.p_cr_46 = p_cr->step1.sec4.p_CR_buf_46;
            out_hdr_tc_sub.cr_46_size = CR_SIZE_46;
            rval |= ikc_front_end_tone_curve_sub(&in_hdr_tc, &out_hdr_tc_sub);
        }
    }
    if((p_filters->update_flags.iso.sensor_information_updated == 1u) || (window_calculate_updated == 1U)) {
        ikc_in_decompress_t in_decompress;
        ikc_out_decompress_sub_t out_decompress_sub;

        if(p_filters->update_flags.cr.r2y.cr_45_update == 0U) {
            amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_decompression_sub !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
            rval |= IK_ERR_0000;
        } else {
            in_decompress.exp_num = p_filters->input_param.num_of_exposures;
            in_decompress.compress_mode = p_filters->input_param.sensor_info.compression;//0~8, 256~259.
            in_decompress.p_dmy_win_geo = &p_filters->input_param.ctx_buf.result_win.dmy_win_geo;
            out_decompress_sub.p_cr_45 = p_cr->step1.sec4.p_CR_buf_45;
            out_decompress_sub.cr_45_size = CR_SIZE_45;
            rval |= ikc_decompression_sub(&in_decompress, &out_decompress_sub);
        }
    }
    if((p_filters->update_flags.iso.exp0_fe_static_blc_updated == 1u) ||
       (p_filters->update_flags.iso.exp1_fe_static_blc_updated == 1u) ||
       (p_filters->update_flags.iso.exp2_fe_static_blc_updated == 1u)) {
        ikc_in_hdr_blc_t in_hdr_blc;
        ikc_out_hdr_blc_sub_t out_hdr_blc_sub;

        if(p_filters->update_flags.cr.r2y.cr_45_update == 0U) {
            amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_hdr_black_level_sub !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
            rval |= IK_ERR_0000;
        } else {
            in_hdr_blc.exp_num = p_filters->input_param.num_of_exposures;
            in_hdr_blc.rgb_ir_mode = p_filters->input_param.rgb_ir.mode;
            in_hdr_blc.compression_offset = ((p_filters->input_param.sensor_info.compression > IK_RAW_COMPRESS_OFF)&&(p_filters->input_param.sensor_info.compression <= IK_RAW_COMPRESS_10p5)) ? p_filters->input_param.ctx_buf.first_compression_offset : 0;
            in_hdr_blc.p_exp0_frontend_static_blc = &p_filters->input_param.exp0_frontend_static_blc;
            in_hdr_blc.p_exp1_frontend_static_blc = &p_filters->input_param.exp1_frontend_static_blc;
            in_hdr_blc.p_exp2_frontend_static_blc = &p_filters->input_param.exp2_frontend_static_blc;
            out_hdr_blc_sub.p_cr_45 = p_cr->step1.sec4.p_CR_buf_45;
            out_hdr_blc_sub.cr_45_size = CR_SIZE_45;
            rval |= ikc_hdr_black_level_sub(&in_hdr_blc, &out_hdr_blc_sub);
        }
    }
    if((p_filters->update_flags.iso.exp0_fe_wb_gain_updated == 1u) ||
       (p_filters->update_flags.iso.exp1_fe_wb_gain_updated == 1u) ||
       (p_filters->update_flags.iso.exp2_fe_wb_gain_updated == 1u)) {
        ikc_in_hdr_dgain_t in_hdr_dgain = {0};
        ikc_out_hdr_dgain_sub_t out_hdr_dgain_sub;

        if(p_filters->update_flags.cr.r2y.cr_45_update == 0U) {
            amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_hdr_dgain_sub !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
            rval |= IK_ERR_0000;
        } else {
            in_hdr_dgain.exp_num = p_filters->input_param.num_of_exposures;
            in_hdr_dgain.rgb_ir_mode = p_filters->input_param.rgb_ir.mode;
            in_hdr_dgain.p_exp0_frontend_wb_gain = &p_filters->input_param.exp0_frontend_wb_gain;
            in_hdr_dgain.p_exp1_frontend_wb_gain = &p_filters->input_param.exp1_frontend_wb_gain;
            in_hdr_dgain.p_exp2_frontend_wb_gain = &p_filters->input_param.exp2_frontend_wb_gain;
            in_hdr_dgain.p_exp0_frontend_static_blc = &p_filters->input_param.exp0_frontend_static_blc;
            in_hdr_dgain.p_exp1_frontend_static_blc = &p_filters->input_param.exp1_frontend_static_blc;
            in_hdr_dgain.p_exp2_frontend_static_blc = &p_filters->input_param.exp2_frontend_static_blc;
            out_hdr_dgain_sub.p_cr_45 = p_cr->step1.sec4.p_CR_buf_45;
            out_hdr_dgain_sub.cr_45_size = CR_SIZE_45;
            rval |= ikc_hdr_dgain_sub(&in_hdr_dgain, &out_hdr_dgain_sub);
        }
    }
    if((p_filters->update_flags.iso.ce_updated == 1u) || (window_calculate_updated == 1U)) {
        ikc_in_ce_t in_ce;
        ikc_out_ce_sub_t out_ce_sub;

        if((p_filters->update_flags.cr.r2y.cr_49_update == 0U)||
           (p_filters->update_flags.cr.r2y.cr_50_update == 0U)||
           (p_filters->update_flags.cr.r2y.cr_51_update == 0U)) {
            amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_contrast_enhancement_sub !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
            rval |= IK_ERR_0000;
        } else {
            in_ce.sensor_pattern = p_filters->input_param.sensor_info.sensor_pattern;
            in_ce.p_dmy_win_geo = &p_filters->input_param.ctx_buf.result_win.dmy_win_geo;
            in_ce.p_ce = &p_filters->input_param.ce;
            out_ce_sub.p_cr_49 = p_cr->step1.sec4.p_CR_buf_49;
            out_ce_sub.cr_49_size = CR_SIZE_49;
            out_ce_sub.p_cr_50 = p_cr->step1.sec4.p_CR_buf_50;
            out_ce_sub.cr_50_size = CR_SIZE_50;
            out_ce_sub.p_cr_51 = p_cr->step1.sec4.p_CR_buf_51;
            out_ce_sub.cr_51_size = CR_SIZE_51;
            rval |= ikc_contrast_enhancement_sub(&in_ce, &out_ce_sub);
        }
    }
    if(p_filters->update_flags.iso.ce_input_table_updated == 1u) {
        ikc_in_ce_input_sub_t in_ce_input_sub;
        ikc_out_ce_input_sub_t out_ce_input_sub;

        if((p_filters->update_flags.cr.r2y.cr_51_update == 0U)||
           (p_filters->update_flags.cr.r2y.cr_52_update == 0U)) {
            amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_contrast_enhancement_input_sub !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
            rval |= IK_ERR_0000;
        } else {
            in_ce_input_sub.ce_enable = p_filters->input_param.ce.enable;
            in_ce_input_sub.radius = p_filters->input_param.ce.radius;
            in_ce_input_sub.epsilon = p_filters->input_param.ce.epsilon;
            in_ce_input_sub.p_ce_input_table = &p_filters->input_param.ce_input_table;
            out_ce_input_sub.p_cr_51 = p_cr->step1.sec4.p_CR_buf_51;
            out_ce_input_sub.cr_51_size = CR_SIZE_51;
            out_ce_input_sub.p_cr_52 = p_cr->step1.sec4.p_CR_buf_52;
            out_ce_input_sub.cr_52_size = CR_SIZE_52;
            rval |= ikc_contrast_enhancement_input_sub(&in_ce_input_sub, &out_ce_input_sub);
        }
    }
    if ((p_filters->update_flags.iso.vignette_compensation_updated == 1u) || (p_filters->update_flags.iso.window_size_info_updated == 1U) ||
        (window_calculate_updated == 1U)) {
        ikc_in_vignette_t in_vig;
        ikc_out_vignette_sub_t out_vig_sub;

        if((p_filters->update_flags.cr.r2y.cr_47_update == 0U)||
           (p_filters->update_flags.cr.r2y.cr_48_update == 0U)||
           (p_filters->update_flags.cr.r2y.cr_49_update == 0U)) {
            amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_vignette_sub !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
            rval |= IK_ERR_0000;
        } else {
            in_vig.vig_enable = p_filters->input_param.vig_enable;
            in_vig.flip_mode = p_filters->input_param.flip_mode;
            in_vig.p_vignette_compensation = &p_filters->input_param.vignette_compensation;
            in_vig.p_vin_sensor = &p_filters->input_param.window_size_info.vin_sensor;
            in_vig.p_dmy_win_geo = &p_filters->input_param.ctx_buf.result_win.dmy_win_geo;
            in_vig.p_active_window = &p_filters->input_param.active_window;
            out_vig_sub.p_cr_47 = p_cr->step1.sec4.p_CR_buf_47;
            out_vig_sub.cr_47_size = CR_SIZE_47;
            out_vig_sub.p_cr_48 = p_cr->step1.sec4.p_CR_buf_48;
            out_vig_sub.cr_48_size = CR_SIZE_48;
            out_vig_sub.p_cr_49 = p_cr->step1.sec4.p_CR_buf_49;
            out_vig_sub.cr_49_size = CR_SIZE_49;
            rval |= ikc_vignette_sub(&in_vig, &out_vig_sub);
        }
    }
    if(p_filters->update_flags.iso.rgb_ir_updated == 1u) {
        ikc_in_rgb_ir_t in_rgb_ir;
        ikc_out_rgb_ir_sub_t out_rgb_ir_sub;

        if((p_filters->update_flags.cr.r2y.cr_45_update == 0U)||
           (p_filters->update_flags.cr.r2y.cr_47_update == 0U)||
           (p_filters->update_flags.cr.r2y.cr_49_update == 0U)||
           (p_filters->update_flags.cr.r2y.cr_50_update == 0U)) {
            amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_rgb_ir_sub !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
            rval |= IK_ERR_0000;
        } else {
            in_rgb_ir.p_rgb_ir = &p_filters->input_param.rgb_ir;
            in_rgb_ir.p_before_ce_wb_gain = &p_filters->input_param.before_ce_wb_gain;
            in_rgb_ir.p_after_ce_wb_gain = &p_filters->input_param.after_ce_wb_gain;
            in_rgb_ir.p_exp0_frontend_wb_gain = &p_filters->input_param.exp0_frontend_wb_gain;
            out_rgb_ir_sub.p_cr_45 = p_cr->step1.sec4.p_CR_buf_45;
            out_rgb_ir_sub.cr_45_size = CR_SIZE_45;
            out_rgb_ir_sub.p_cr_47 = p_cr->step1.sec4.p_CR_buf_47;
            out_rgb_ir_sub.cr_47_size = CR_SIZE_47;
            out_rgb_ir_sub.p_cr_49 = p_cr->step1.sec4.p_CR_buf_49;
            out_rgb_ir_sub.cr_49_size = CR_SIZE_49;
            out_rgb_ir_sub.p_cr_50 = p_cr->step1.sec4.p_CR_buf_50;
            out_rgb_ir_sub.cr_50_size = CR_SIZE_50;
            rval |= ikc_rgb_ir_sub(&in_rgb_ir, &out_rgb_ir_sub);
        }
    }

    if (p_filters->update_flags.iso.hi_asf_updated == 1U) {
        ikc_in_asf_t  in_asf;
        ikc_out_asf_t out_asf;

        if(p_filters->update_flags.cr.r2y.cr_30_update == 0U) {
            amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on step1 ikc_asf !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
            rval |= IK_ERR_0000;
        } else {
            in_asf.p_advance_spatial_filter           = &p_filters->hiso_input_param.hi_asf;
            in_asf.first_luma_processing_mode_updated = (uint32)p_filters->update_flags.iso.first_luma_processing_mode_updated;
            out_asf.p_cr_30    = p_cr->step1.sec2.p_CR_buf_30;
            out_asf.cr_30_size = CR_SIZE_30;
            rval |= ikc_asf(&in_asf, &out_asf);
        }
    }

    if(((p_filters->update_flags.iso.after_ce_wb_gain_updated == 1U)&&(p_filters->input_param.ability != AMBA_IK_VIDEO_PIPE_Y2Y)) ||
       ((p_filters->update_flags.iso.rgb_to_12y_updated == 1U)&&(p_filters->input_param.ability != AMBA_IK_VIDEO_PIPE_Y2Y))||
       (p_filters->update_flags.iso.lnl_updated == 1U)||
       (window_calculate_updated == 1U)) {
        ikc_in_lnl_t in_lnl;
        ikc_out_lnl_t out_lnl;

        if(p_filters->update_flags.cr.r2y.cr_30_update == 0U) {
            amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on Hiso step1 ikc_lnl !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
            rval |= IK_ERR_0000;
        } else {
            if(p_filters->input_param.hi_luma_noise_reduce.enable == 1U) {
                if(p_filters->hiso_input_param.hi_asf.enable != 1U) {
                    amba_ik_system_print_uint32_5("[IK] [ERROR] IF [hi_color_dependent_luma_noise_reduction.enable] = 1, then hi_advanced_spatial_filter.enable (=%d) should be enabled !!!",p_filters->hiso_input_param.hi_asf.enable,DC_U,DC_U,DC_U,DC_U);
                }
            }
            in_lnl.p_luma_noise_reduce = &p_filters->input_param.hi_luma_noise_reduce;
            in_lnl.p_after_ce_wb_gain = &p_filters->input_param.after_ce_wb_gain;
            in_lnl.p_rgb_to_12y = &p_filters->input_param.rgb_to_12y;
            in_lnl.p_cfa_win = &p_filters->input_param.ctx_buf.result_win.cfa_win_dim;
            in_lnl.use_sharpen_not_asf = 0;
            in_lnl.p_advance_spatial_filter = &p_filters->hiso_input_param.hi_asf;
            out_lnl.p_cr_30 = p_cr->step1.sec2.p_CR_buf_30;
            out_lnl.cr_30_size = CR_SIZE_30;
            out_lnl.p_flow_info = &p_flow->flow_info;
            rval |= ikc_lnl(&in_lnl, &out_lnl);
        }
    }
    if((p_filters->update_flags.iso.color_correction_updated == 1U)||
       (p_filters->update_flags.iso.tone_curve_updated == 1U)) {
        ikc_in_lnl_tbl_t in_lnl_tbl;
        ikc_out_lnl_t out_lnl;

        if(p_filters->update_flags.cr.r2y.cr_30_update == 0U) {
            amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_lnl_tbl !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
            rval |= IK_ERR_0000;
        } else {
            in_lnl_tbl.p_color_correction = &p_filters->input_param.color_correction;
            in_lnl_tbl.p_tone_curve = &p_filters->input_param.tone_curve;
            out_lnl.p_cr_30 = p_cr->step1.sec2.p_CR_buf_30;
            out_lnl.cr_30_size = CR_SIZE_30;
            rval |= ikc_lnl_tbl(&in_lnl_tbl, &out_lnl);
        }
    }
    {
        ikc_in_hiso_frame_size_t in_hiso_frame_size;
        ikc_out_asf_t out_shpa_frame_size;
        ikc_out_sec18_mctf_shpb_t out_frame_size_div16_m1;
        in_hiso_frame_size.step = 0;  //step1
        in_hiso_frame_size.chroma_filter         = &p_filters->input_param.chroma_filter;
        in_hiso_frame_size.hi_nonsmooth_detect   = &p_filters->hiso_input_param.hi_nonsmooth_detect;
        in_hiso_frame_size.hi_chroma_filter_high = &p_filters->hiso_input_param.hi_chroma_filter_high;
        in_hiso_frame_size.p_main                = &p_filters->input_param.window_size_info.main_win;
        out_shpa_frame_size.p_cr_30    = p_cr->step1.sec2.p_CR_buf_30;
        out_shpa_frame_size.cr_30_size = CR_SIZE_30;
        rval |= ikc_hiso_shpa_frame_size(&in_hiso_frame_size, &out_shpa_frame_size);

        out_frame_size_div16_m1.p_cr_112    = p_cr->step1.sec18.p_CR_buf_112;
        out_frame_size_div16_m1.p_cr_117    = p_cr->step1.sec18.p_CR_buf_117;
        out_frame_size_div16_m1.cr_112_size = CR_SIZE_112;
        out_frame_size_div16_m1.cr_117_size = CR_SIZE_117;
        rval |= ikc_hiso_sec18_frame_size_div16_m1(&in_hiso_frame_size, &out_frame_size_div16_m1);
    }
    {
        ikc_out_sec18_mctf_shpb_t out_mctf;
        ikc_in_mctf_pass_through_t in_mctf_pass_through;

        in_mctf_pass_through.is_yuv_422 = 0u;
        in_mctf_pass_through.step       = 0u;
        out_mctf.p_cr_112 = p_cr->step1.sec18.p_CR_buf_112;
        out_mctf.cr_112_size = CR_SIZE_112;
        out_mctf.p_cr_117 = p_cr->step1.sec18.p_CR_buf_117;
        out_mctf.cr_117_size = CR_SIZE_117;
        rval |= ikc_hiso_mctf_pass_through(&in_mctf_pass_through, &out_mctf);
    }
    {
        ikc_out_sec18_mctf_shpb_t out_mctf;
        out_mctf.p_cr_112 = p_cr->step1.sec18.p_CR_buf_112;
        out_mctf.cr_112_size = CR_SIZE_112;
        out_mctf.p_cr_117 = p_cr->step1.sec18.p_CR_buf_117;
        out_mctf.cr_117_size = CR_SIZE_117;
        rval |= ikc_hiso_mctf_ta_constraints(&out_mctf);
    }
    return rval;
}

static uint32 exe_process_hiso_step2_cr(idsp_flow_ctrl_t *p_flow, const amba_ik_hiso_flow_tables_list_t *p_cr, amba_ik_filter_t *p_filters)
{
    uint32 rval = IK_OK;
    uint32 window_calculate_updated = 0U;

    rval |= exe_chroma_noise_def_value_step2(&p_cr->step2.sec2, p_filters);

    if((p_filters->update_flags.iso.rgb_to_yuv_matrix_updated == 1U) ||
       (p_filters->update_flags.iso.is_1st_frame == 1U)) {
        ik_rgb_to_yuv_matrix_t rgb_to_yuv_matrix;
        ikc_in_rgb2yuv_t in_rgb2yuv = {0};
        ikc_out_rgb2yuv_t out_rgb2yuv;
        rgb_to_yuv_matrix.matrix_values[0] = 1024;
        rgb_to_yuv_matrix.matrix_values[1] = 0;
        rgb_to_yuv_matrix.matrix_values[2] = 0;
        rgb_to_yuv_matrix.matrix_values[3] = 0;
        rgb_to_yuv_matrix.matrix_values[4] = 1024;
        rgb_to_yuv_matrix.matrix_values[5] = 0;
        rgb_to_yuv_matrix.matrix_values[6] = 0;
        rgb_to_yuv_matrix.matrix_values[7] = 0;
        rgb_to_yuv_matrix.matrix_values[8] = 1024;
        rgb_to_yuv_matrix.y_offset = 0;
        rgb_to_yuv_matrix.u_offset = 128;
        rgb_to_yuv_matrix.v_offset= 128;
        if ((p_filters->input_param.sensor_info.sensor_mode == 1U)/* && (p_filters->input_param.rgb_ir.ir_only == 1U)*/) {
            (void)amba_ik_system_memset(&rgb_to_yuv_matrix, 0x0, sizeof(ik_rgb_to_yuv_matrix_t));
            rgb_to_yuv_matrix.matrix_values[2] = 1024; //m13
            rgb_to_yuv_matrix.u_offset = 128;
            rgb_to_yuv_matrix.v_offset = 128;
        }

        if(p_filters->update_flags.cr.r2y.cr_29_update == 0U) {
            amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_rgb2yuv step2!!!",DC_U,DC_U,DC_U,DC_U,DC_U);
            rval |= IK_ERR_0000;
        } else {
            in_rgb2yuv.p_rgb_to_yuv_matrix = &rgb_to_yuv_matrix;
            out_rgb2yuv.p_cr_29 = p_cr->step2.sec2.p_CR_buf_29;
            out_rgb2yuv.cr_29_size = CR_SIZE_29;
            rval |= ikc_rgb2yuv(&in_rgb2yuv, &out_rgb2yuv);
        }
    }

    if((p_filters->update_flags.iso.is_1st_frame == 1U)) {
        ikc_in_cc_premul_t in_cc_premul;
        ikc_out_cc_out_t   out_cc_premul;

        if((p_filters->update_flags.cr.r2y.cr_26_update == 0U)||
           (p_filters->update_flags.cr.r2y.cr_27_update == 0U)) {
            amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_color_correction !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
            rval |= IK_ERR_0000;
        } else {
            in_cc_premul.is_first_frame = 1U;
            in_cc_premul.p_pre_cc_gain = &p_filters->input_param.pre_cc_gain;
            out_cc_premul.p_cr_26 = p_cr->step2.sec2.p_CR_buf_26;
            out_cc_premul.cr_26_size = CR_SIZE_26;
            rval |= ikc_color_correction_premul(&in_cc_premul, &out_cc_premul);
        }
    }

    if(p_filters->update_flags.iso.is_1st_frame == 1U) {
        rval |= exe_process_input_mode_y2y_step2(p_flow, &p_cr->step2.sec2, p_filters);
    }

    if(p_filters->update_flags.iso.hi_nonsmooth_detect_updated == 1U) {
        ikc_in_cc_edge_score_t in_cc_edge_score;
        ikc_out_cc_edge_score_t out_cc_edge_score;

        if(p_filters->update_flags.cr.r2y.cr_27_update == 0U) {
            amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_color_correction_edge_score !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
            rval |= IK_ERR_0000;
        } else {
            in_cc_edge_score.p_hi_nonsmooth_detect = &p_filters->hiso_input_param.hi_nonsmooth_detect;
            out_cc_edge_score.p_cr_27 = p_cr->step2.sec2.p_CR_buf_27;
            out_cc_edge_score.cr_27_size = CR_SIZE_27;
            rval |= ikc_color_correction_edge_score(&in_cc_edge_score, &out_cc_edge_score);
        }
    }

    if(p_filters->update_flags.iso.hi_nonsmooth_detect_updated  == 1U) {
        ikc_in_step2_asf_mcts   in_step2_asf_mcts;
        ikc_out_sec18_sharpen_t out_sharpen;

        if(p_filters->update_flags.cr.r2y.cr_117_update == 0U) {
            amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_sec18_step2_asf_mcts !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
            rval |= IK_ERR_0000;
        } else {
            in_step2_asf_mcts.p_in_hi_nonsmooth_detect = &p_filters->hiso_input_param.hi_nonsmooth_detect;
            out_sharpen.p_cr_117    = p_cr->step2.sec18.p_CR_buf_117;
            out_sharpen.cr_117_size = CR_SIZE_117;
            rval |= ikc_sec18_step2_asf_mcts(&in_step2_asf_mcts, &out_sharpen);
        }
    }

    if ((p_filters->update_flags.iso.window_size_info_updated == 1U) ||
        (p_filters->update_flags.iso.cfa_window_size_info_updated == 1U) ||
        (p_filters->update_flags.iso.dmy_range_updated == 1U) ||
        (p_filters->update_flags.iso.dzoom_info_updated == 1U) ||
        (p_filters->update_flags.iso.vin_active_win_updated == 1U) ||
        (p_filters->update_flags.iso.stitching_info_updated == 1U) ||
        (p_filters->update_flags.iso.overlap_x_updated == 1U) ||
        (p_filters->update_flags.iso.flip_mode_updated == 1U) ||
        (p_filters->update_flags.iso.warp_enable_updated == 1U) ||
        (p_filters->update_flags.iso.warp_enable_2nd_updated == 1U)) {
        window_calculate_updated = 1U;
    }

    if((p_filters->update_flags.iso.chroma_filter_updated == 1U)||
       (p_filters->update_flags.iso.resample_str_update == 1U)||
       (p_filters->update_flags.iso.window_size_info_updated == 1U)||
       (window_calculate_updated == 1U)) {
        ikc_in_sec2_hiso_resampler_t in_sec2_hiso_resampler;
        ikc_out_sec2_main_resampler_t out_sec2_main_resampler;
        ikc_in_hiso_resampler_size_t  in_hiso_resampler;
        ikc_out_hiso_resampler_size_t out_hiso_resampler;

        in_hiso_resampler.step   = 1;//step2
        in_hiso_resampler.p_main = &p_filters->input_param.window_size_info.main_win;
        in_hiso_resampler.chroma_filter         = &p_filters->input_param.chroma_filter;
        in_hiso_resampler.hi_nonsmooth_detect   = &p_filters->hiso_input_param.hi_nonsmooth_detect;
        in_hiso_resampler.hi_chroma_filter_high = &p_filters->hiso_input_param.hi_chroma_filter_high;

        rval |= ikc_hiso_resampler_size(&in_hiso_resampler, &out_hiso_resampler);
        in_sec2_hiso_resampler.step   = 1u;
        in_sec2_hiso_resampler.main_cut_off_freq = p_filters->input_param.resample_str.main_cut_off_freq;
        in_sec2_hiso_resampler.p_main = &out_hiso_resampler.output;
        in_sec2_hiso_resampler.chroma_radius = p_filters->input_param.chroma_filter.radius;
        out_sec2_main_resampler.p_cr_33 = p_cr->step2.sec2.p_CR_buf_33;
        out_sec2_main_resampler.cr_33_size = CR_SIZE_33;
        out_sec2_main_resampler.p_cr_35 = p_cr->step2.sec2.p_CR_buf_35;
        out_sec2_main_resampler.cr_35_size = CR_SIZE_35;
        out_sec2_main_resampler.p_cr_37 = p_cr->step2.sec2.p_CR_buf_37;
        out_sec2_main_resampler.cr_37_size = CR_SIZE_37;
        rval |= ikc_sec2_hiso_resampler(&in_sec2_hiso_resampler, &out_sec2_main_resampler);
    }

    {
        ikc_in_hiso_frame_size_t in_hiso_frame_size;
        ikc_out_asf_t out_shpa_frame_size;
        ikc_out_sec18_mctf_shpb_t out_frame_size_div16_m1;
        in_hiso_frame_size.step = 1;  //step2
        in_hiso_frame_size.chroma_filter         = &p_filters->input_param.chroma_filter;
        in_hiso_frame_size.hi_nonsmooth_detect   = &p_filters->hiso_input_param.hi_nonsmooth_detect;
        in_hiso_frame_size.hi_chroma_filter_high = &p_filters->hiso_input_param.hi_chroma_filter_high;
        in_hiso_frame_size.p_main                = &p_filters->input_param.window_size_info.main_win;
        out_shpa_frame_size.p_cr_30    = p_cr->step2.sec2.p_CR_buf_30;
        out_shpa_frame_size.cr_30_size = CR_SIZE_30;
        rval |= ikc_hiso_shpa_frame_size(&in_hiso_frame_size, &out_shpa_frame_size);
        out_frame_size_div16_m1.p_cr_112    = p_cr->step2.sec18.p_CR_buf_112;
        out_frame_size_div16_m1.p_cr_117    = p_cr->step2.sec18.p_CR_buf_117;
        out_frame_size_div16_m1.cr_112_size = CR_SIZE_112;
        out_frame_size_div16_m1.cr_117_size = CR_SIZE_117;
        rval |= ikc_hiso_sec18_frame_size_div16_m1(&in_hiso_frame_size, &out_frame_size_div16_m1);
    }
    {
        ikc_out_sec18_mctf_shpb_t out_mctf;
        ikc_in_mctf_pass_through_t in_mctf_pass_through;

        in_mctf_pass_through.is_yuv_422 = 0u;
        in_mctf_pass_through.step       = 1u;
        out_mctf.p_cr_112 = p_cr->step2.sec18.p_CR_buf_112;
        out_mctf.cr_112_size = CR_SIZE_112;
        out_mctf.p_cr_117 = p_cr->step2.sec18.p_CR_buf_117;
        out_mctf.cr_117_size = CR_SIZE_117;
        rval |= ikc_hiso_mctf_pass_through(&in_mctf_pass_through, &out_mctf);
    }
    {
        ikc_out_sec18_mctf_shpb_t out_mctf;
        out_mctf.p_cr_112 = p_cr->step2.sec18.p_CR_buf_112;
        out_mctf.cr_112_size = CR_SIZE_112;
        out_mctf.p_cr_117 = p_cr->step2.sec18.p_CR_buf_117;
        out_mctf.cr_117_size = CR_SIZE_117;
        rval |= ikc_hiso_mctf_ta_constraints(&out_mctf);
    }
    return rval;
}

static uint32 exe_process_hiso_step3_cr(idsp_flow_ctrl_t *p_flow, const amba_ik_hiso_flow_tables_list_t *p_cr, amba_ik_filter_t *p_filters)
{
    uint32 rval = IK_OK;
    ik_adv_spatial_filter_t hiso_chroma_asf;
    ikc_in_asf_t  in_asf;
    ikc_out_asf_t out_asf;
    uint32 window_calculate_updated = 0U;

    rval |= exe_chroma_noise_def_value(&p_cr->step3.sec2, p_filters);

    if((p_filters->update_flags.iso.rgb_to_yuv_matrix_updated == 1U) ||
       (p_filters->update_flags.iso.is_1st_frame == 1U)) {
        rval |= exe_process_init_y2y_rgb2yuv(&p_cr->step3.sec2, p_filters);
    }

    if(p_filters->update_flags.iso.is_1st_frame == 1U) {
        rval |= exe_process_input_mode_y2y_step3(p_flow, &p_cr->step3.sec2, p_filters, 2u);  // index 2 step3
    }

    exe_iso_cfg_high_iso_chroma_asf_to_asf(&hiso_chroma_asf, &p_filters->hiso_input_param.hi_chroma_asf);

    if(p_filters->update_flags.cr.r2y.cr_30_update == 0U) {
        amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_hiso_asf !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
        rval |= IK_ERR_0000;
    } else {
        in_asf.p_advance_spatial_filter           = &hiso_chroma_asf;
        in_asf.first_luma_processing_mode_updated = (uint32)p_filters->update_flags.iso.first_luma_processing_mode_updated;
        out_asf.p_cr_30    = p_cr->step3.sec2.p_CR_buf_30;
        out_asf.cr_30_size = CR_SIZE_30;
        rval |= ikc_hiso_asf(&in_asf, &out_asf);
    }

    if(p_filters->update_flags.iso.hi_high_asf_updated == 1U) {
        ikc_in_step3_asf_mcts   in_step3_asf_mcts;
        ikc_out_sec18_sharpen_t out_sharpen;

        if(p_filters->update_flags.cr.r2y.cr_117_update == 0U) {
            amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_sec18_step3_asf_mcts !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
            rval |= IK_ERR_0000;
        } else {
            if(p_filters->hiso_input_param.hi_high_asf.enable == 1U) {
                in_step3_asf_mcts.p_in_hi_high_asf = &p_filters->hiso_input_param.hi_high_asf;
                out_sharpen.p_cr_117    = p_cr->step3.sec18.p_CR_buf_117;
                out_sharpen.cr_117_size = CR_SIZE_117;
                rval |= ikc_sec18_step3_asf_mcts(&in_step3_asf_mcts, &out_sharpen);
            } else {
                ikc_out_sec18_mctf_shpb_t out_shpb;
                ikc_in_shpb_pass_through_t  in_shpb_pass_through;
                in_shpb_pass_through.is_yuv_422 = 0u;
                in_shpb_pass_through.step       = 2u;//step3
                out_shpb.p_cr_112 = p_cr->step3.sec18.p_CR_buf_112;
                out_shpb.cr_112_size = CR_SIZE_112;
                out_shpb.p_cr_117 = p_cr->step3.sec18.p_CR_buf_117;
                out_shpb.cr_117_size = CR_SIZE_117;
                rval |= ikc_hiso_shpb_pass_through(&in_shpb_pass_through, &out_shpb);
            }
        }
    }

    if ((p_filters->update_flags.iso.window_size_info_updated == 1U) ||
        (p_filters->update_flags.iso.cfa_window_size_info_updated == 1U) ||
        (p_filters->update_flags.iso.dmy_range_updated == 1U) ||
        (p_filters->update_flags.iso.dzoom_info_updated == 1U) ||
        (p_filters->update_flags.iso.vin_active_win_updated == 1U) ||
        (p_filters->update_flags.iso.stitching_info_updated == 1U) ||
        (p_filters->update_flags.iso.overlap_x_updated == 1U) ||
        (p_filters->update_flags.iso.flip_mode_updated == 1U) ||
        (p_filters->update_flags.iso.warp_enable_updated == 1U) ||
        (p_filters->update_flags.iso.warp_enable_2nd_updated == 1U)) {
        window_calculate_updated = 1U;
    }

    if((p_filters->update_flags.iso.chroma_filter_updated == 1U)||
       (p_filters->update_flags.iso.resample_str_update == 1U)||
       (p_filters->update_flags.iso.window_size_info_updated == 1U)||
       (window_calculate_updated == 1U)) {
        ikc_in_sec2_hiso_resampler_t in_sec2_hiso_resampler;
        ikc_out_sec2_main_resampler_t out_sec2_main_resampler;
        ikc_in_hiso_resampler_size_t  in_hiso_resampler;
        ikc_out_hiso_resampler_size_t out_hiso_resampler;

        in_hiso_resampler.step   = 2;//step3
        in_hiso_resampler.p_main = &p_filters->input_param.window_size_info.main_win;
        in_hiso_resampler.chroma_filter         = &p_filters->input_param.chroma_filter;
        in_hiso_resampler.hi_nonsmooth_detect   = &p_filters->hiso_input_param.hi_nonsmooth_detect;
        in_hiso_resampler.hi_chroma_filter_high = &p_filters->hiso_input_param.hi_chroma_filter_high;

        rval |= ikc_hiso_resampler_size(&in_hiso_resampler, &out_hiso_resampler);

        in_sec2_hiso_resampler.step   = 2U;
        in_sec2_hiso_resampler.main_cut_off_freq = p_filters->input_param.resample_str.main_cut_off_freq;
        in_sec2_hiso_resampler.p_main = &out_hiso_resampler.output;
        in_sec2_hiso_resampler.chroma_radius = p_filters->input_param.chroma_filter.radius;
        out_sec2_main_resampler.p_cr_33 = p_cr->step3.sec2.p_CR_buf_33;
        out_sec2_main_resampler.cr_33_size = CR_SIZE_33;
        out_sec2_main_resampler.p_cr_35 = p_cr->step3.sec2.p_CR_buf_35;
        out_sec2_main_resampler.cr_35_size = CR_SIZE_35;
        out_sec2_main_resampler.p_cr_37 = p_cr->step3.sec2.p_CR_buf_37;
        out_sec2_main_resampler.cr_37_size = CR_SIZE_37;
        rval |= ikc_sec2_hiso_resampler(&in_sec2_hiso_resampler, &out_sec2_main_resampler);
    }

    {
        ikc_in_hiso_frame_size_t in_hiso_frame_size;
        ikc_out_asf_t out_shpa_frame_size;
        ikc_out_sec18_mctf_shpb_t out_frame_size_div16_m1;
        in_hiso_frame_size.step = 2;  //step3
        in_hiso_frame_size.chroma_filter         = &p_filters->input_param.chroma_filter;
        in_hiso_frame_size.hi_nonsmooth_detect   = &p_filters->hiso_input_param.hi_nonsmooth_detect;
        in_hiso_frame_size.hi_chroma_filter_high = &p_filters->hiso_input_param.hi_chroma_filter_high;
        in_hiso_frame_size.p_main                = &p_filters->input_param.window_size_info.main_win;
        out_shpa_frame_size.p_cr_30    = p_cr->step3.sec2.p_CR_buf_30;
        out_shpa_frame_size.cr_30_size = CR_SIZE_30;
        rval |= ikc_hiso_shpa_frame_size(&in_hiso_frame_size, &out_shpa_frame_size);
        out_frame_size_div16_m1.p_cr_112    = p_cr->step3.sec18.p_CR_buf_112;
        out_frame_size_div16_m1.p_cr_117    = p_cr->step3.sec18.p_CR_buf_117;
        out_frame_size_div16_m1.cr_112_size = CR_SIZE_112;
        out_frame_size_div16_m1.cr_117_size = CR_SIZE_117;
        rval |= ikc_hiso_sec18_frame_size_div16_m1(&in_hiso_frame_size, &out_frame_size_div16_m1);
    }
    {
        ikc_out_sec18_mctf_shpb_t out_mctf;
        ikc_in_mctf_pass_through_t in_mctf_pass_through;

        in_mctf_pass_through.is_yuv_422 = 0u;
        in_mctf_pass_through.step       = 2u; //step3
        out_mctf.p_cr_112 = p_cr->step3.sec18.p_CR_buf_112;
        out_mctf.cr_112_size = CR_SIZE_112;
        out_mctf.p_cr_117 = p_cr->step3.sec18.p_CR_buf_117;
        out_mctf.cr_117_size = CR_SIZE_117;
        rval |= ikc_hiso_mctf_pass_through(&in_mctf_pass_through, &out_mctf);
    }
    {
        ikc_out_sec18_mctf_shpb_t out_mctf;
        out_mctf.p_cr_112 = p_cr->step3.sec18.p_CR_buf_112;
        out_mctf.cr_112_size = CR_SIZE_112;
        out_mctf.p_cr_117 = p_cr->step3.sec18.p_CR_buf_117;
        out_mctf.cr_117_size = CR_SIZE_117;
        rval |= ikc_hiso_mctf_ta_constraints(&out_mctf);
    }
    return rval;
}

static uint32 exe_process_hiso_step4_cr(idsp_flow_ctrl_t *p_flow, const amba_ik_hiso_flow_tables_list_t *p_cr, amba_ik_filter_t *p_filters)
{
    uint32 rval = IK_OK;
    ikc_in_asf_a_t  in_asf;
    ikc_out_asf_t   out_asf;
    uint32 window_calculate_updated = 0U;

    rval |= exe_chroma_noise_def_value(&p_cr->step4.sec2, p_filters);

    if((p_filters->update_flags.iso.rgb_to_yuv_matrix_updated == 1U) ||
       (p_filters->update_flags.iso.is_1st_frame == 1U)) {
        rval |= exe_process_init_y2y_rgb2yuv(&p_cr->step4.sec2, p_filters);
    }

    if(p_filters->update_flags.iso.is_1st_frame == 1U) { //should only configure once at 1st time.
        rval |= exe_process_input_mode_y2y_step4(p_flow, &p_cr->step4.sec2, p_filters);
    }

    if(p_filters->update_flags.cr.r2y.cr_30_update == 0U) {
        amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_hiso_asf_a !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
        rval |= IK_ERR_0000;
    } else {
        in_asf.p_advance_spatial_filter = &p_filters->hiso_input_param.hi_med1_asf;
        in_asf.first_luma_processing_mode_updated = (uint32)p_filters->update_flags.iso.first_luma_processing_mode_updated;
        out_asf.p_cr_30    = p_cr->step4.sec2.p_CR_buf_30;
        out_asf.cr_30_size = CR_SIZE_30;
        rval |= ikc_hiso_asf_a(&in_asf, &out_asf);
    }

    if(p_filters->update_flags.iso.hi_low_asf_updated == 1U) {
        ikc_in_step4_asf_mcts   in_step4_asf_mcts;
        ikc_out_sec18_sharpen_t out_sharpen;

        if(p_filters->update_flags.cr.r2y.cr_117_update == 0U) {
            amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_sec18_step4_asf_mcts !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
            rval |= IK_ERR_0000;
        } else {
            if(p_filters->hiso_input_param.hi_low_asf.enable == 1U) {
                in_step4_asf_mcts.p_in_hi_low_asf = &p_filters->hiso_input_param.hi_low_asf;
                out_sharpen.p_cr_117    = p_cr->step4.sec18.p_CR_buf_117;
                out_sharpen.cr_117_size = CR_SIZE_117;
                rval |= ikc_sec18_step4_asf_mcts(&in_step4_asf_mcts, &out_sharpen);
            } else {
                ikc_out_sec18_mctf_shpb_t out_shpb;
                ikc_in_shpb_pass_through_t  in_shpb_pass_through;
                in_shpb_pass_through.is_yuv_422 = 0u;
                in_shpb_pass_through.step       = 3u; //step4
                out_shpb.p_cr_112 = p_cr->step4.sec18.p_CR_buf_112;
                out_shpb.cr_112_size = CR_SIZE_112;
                out_shpb.p_cr_117 = p_cr->step4.sec18.p_CR_buf_117;
                out_shpb.cr_117_size = CR_SIZE_117;
                rval |= ikc_hiso_shpb_pass_through(&in_shpb_pass_through, &out_shpb);
            }
        }
    }

    if ((p_filters->update_flags.iso.window_size_info_updated == 1U) ||
        (p_filters->update_flags.iso.cfa_window_size_info_updated == 1U) ||
        (p_filters->update_flags.iso.dmy_range_updated == 1U) ||
        (p_filters->update_flags.iso.dzoom_info_updated == 1U) ||
        (p_filters->update_flags.iso.vin_active_win_updated == 1U) ||
        (p_filters->update_flags.iso.stitching_info_updated == 1U) ||
        (p_filters->update_flags.iso.overlap_x_updated == 1U) ||
        (p_filters->update_flags.iso.flip_mode_updated == 1U) ||
        (p_filters->update_flags.iso.warp_enable_updated == 1U) ||
        (p_filters->update_flags.iso.warp_enable_2nd_updated == 1U)) {
        window_calculate_updated = 1U;
    }

    if((p_filters->update_flags.iso.chroma_filter_updated == 1U)||
       (p_filters->update_flags.iso.resample_str_update == 1U)||
       (p_filters->update_flags.iso.window_size_info_updated == 1U)||
       (window_calculate_updated == 1U)) {
        ikc_in_sec2_hiso_resampler_t in_sec2_hiso_resampler;
        ikc_out_sec2_main_resampler_t out_sec2_main_resampler;
        ikc_in_hiso_resampler_size_t  in_hiso_resampler;
        ikc_out_hiso_resampler_size_t out_hiso_resampler;

        in_hiso_resampler.step   = 3;//step4
        in_hiso_resampler.p_main = &p_filters->input_param.window_size_info.main_win;
        in_hiso_resampler.chroma_filter         = &p_filters->input_param.chroma_filter;
        in_hiso_resampler.hi_nonsmooth_detect   = &p_filters->hiso_input_param.hi_nonsmooth_detect;
        in_hiso_resampler.hi_chroma_filter_high = &p_filters->hiso_input_param.hi_chroma_filter_high;

        rval |= ikc_hiso_resampler_size(&in_hiso_resampler, &out_hiso_resampler);

        in_sec2_hiso_resampler.step   = 3U;
        in_sec2_hiso_resampler.main_cut_off_freq = p_filters->input_param.resample_str.main_cut_off_freq;
        in_sec2_hiso_resampler.p_main = &out_hiso_resampler.output;
        in_sec2_hiso_resampler.chroma_radius = p_filters->input_param.chroma_filter.radius;
        out_sec2_main_resampler.p_cr_33 = p_cr->step4.sec2.p_CR_buf_33;
        out_sec2_main_resampler.cr_33_size = CR_SIZE_33;
        out_sec2_main_resampler.p_cr_35 = p_cr->step4.sec2.p_CR_buf_35;
        out_sec2_main_resampler.cr_35_size = CR_SIZE_35;
        out_sec2_main_resampler.p_cr_37 = p_cr->step4.sec2.p_CR_buf_37;
        out_sec2_main_resampler.cr_37_size = CR_SIZE_37;
        rval |= ikc_sec2_hiso_resampler(&in_sec2_hiso_resampler, &out_sec2_main_resampler);
    }

    {
        ikc_in_hiso_frame_size_t in_hiso_frame_size;
        ikc_out_asf_t out_shpa_frame_size;
        ikc_out_sec18_mctf_shpb_t out_frame_size_div16_m1;
        in_hiso_frame_size.step = 3;  //step4
        in_hiso_frame_size.chroma_filter         = &p_filters->input_param.chroma_filter;
        in_hiso_frame_size.hi_nonsmooth_detect   = &p_filters->hiso_input_param.hi_nonsmooth_detect;
        in_hiso_frame_size.hi_chroma_filter_high = &p_filters->hiso_input_param.hi_chroma_filter_high;
        in_hiso_frame_size.p_main                = &p_filters->input_param.window_size_info.main_win;
        out_shpa_frame_size.p_cr_30    = p_cr->step4.sec2.p_CR_buf_30;
        out_shpa_frame_size.cr_30_size = CR_SIZE_30;
        rval |= ikc_hiso_shpa_frame_size(&in_hiso_frame_size, &out_shpa_frame_size);
        out_frame_size_div16_m1.p_cr_112    = p_cr->step4.sec18.p_CR_buf_112;
        out_frame_size_div16_m1.p_cr_117    = p_cr->step4.sec18.p_CR_buf_117;
        out_frame_size_div16_m1.cr_112_size = CR_SIZE_112;
        out_frame_size_div16_m1.cr_117_size = CR_SIZE_117;
        rval |= ikc_hiso_sec18_frame_size_div16_m1(&in_hiso_frame_size, &out_frame_size_div16_m1);
    }

    {
        ikc_out_sec18_mctf_shpb_t out_mctf;
        ikc_in_mctf_pass_through_t in_mctf_pass_through;

        in_mctf_pass_through.is_yuv_422 = 0u;
        in_mctf_pass_through.step       = 3u;//step4
        out_mctf.p_cr_112 = p_cr->step4.sec18.p_CR_buf_112;
        out_mctf.cr_112_size = CR_SIZE_112;
        out_mctf.p_cr_117 = p_cr->step4.sec18.p_CR_buf_117;
        out_mctf.cr_117_size = CR_SIZE_117;
        rval |= ikc_hiso_mctf_pass_through(&in_mctf_pass_through, &out_mctf);
    }
    {
        ikc_out_sec18_mctf_shpb_t out_mctf;
        out_mctf.p_cr_112 = p_cr->step4.sec18.p_CR_buf_112;
        out_mctf.cr_112_size = CR_SIZE_112;
        out_mctf.p_cr_117 = p_cr->step4.sec18.p_CR_buf_117;
        out_mctf.cr_117_size = CR_SIZE_117;
        rval |= ikc_hiso_mctf_ta_constraints(&out_mctf);
    }
    return rval;
}

static uint32 exe_process_hiso_step4a_cr(idsp_flow_ctrl_t *p_flow, const amba_ik_hiso_flow_tables_list_t *p_cr, amba_ik_filter_t *p_filters)
{
    uint32 rval = IK_OK;
    uint32 window_calculate_updated = 0U;

    if((p_filters->update_flags.iso.rgb_to_yuv_matrix_updated == 1U) ||
       (p_filters->update_flags.iso.is_1st_frame == 1U)) {
        rval |= exe_process_init_y2y_rgb2yuv(&p_cr->step4a.sec2, p_filters);
    }

    if(p_filters->update_flags.iso.is_1st_frame == 1U) {
        rval |= exe_process_input_mode_y2y_step4(p_flow, &p_cr->step4a.sec2, p_filters);
        {
            ikc_out_step4a_def_value_t out_step4a_def_value;
            ikc_in_def_t  in_hiso_def;
            out_step4a_def_value.p_cr_30 = p_cr->step4a.sec2.p_CR_buf_30;
            out_step4a_def_value.cr_30_size = CR_SIZE_30;

            out_step4a_def_value.p_cr_32 = p_cr->step4a.sec2.p_CR_buf_32;
            out_step4a_def_value.cr_32_size = CR_SIZE_32;

            in_hiso_def.step = 4;
            rval |= ikc_hiso_sec2_hardcode(&in_hiso_def, &out_step4a_def_value);
        }
    }

    if ((p_filters->update_flags.iso.window_size_info_updated == 1U) ||
        (p_filters->update_flags.iso.cfa_window_size_info_updated == 1U) ||
        (p_filters->update_flags.iso.dmy_range_updated == 1U) ||
        (p_filters->update_flags.iso.dzoom_info_updated == 1U) ||
        (p_filters->update_flags.iso.vin_active_win_updated == 1U) ||
        (p_filters->update_flags.iso.stitching_info_updated == 1U) ||
        (p_filters->update_flags.iso.overlap_x_updated == 1U) ||
        (p_filters->update_flags.iso.flip_mode_updated == 1U) ||
        (p_filters->update_flags.iso.warp_enable_updated == 1U) ||
        (p_filters->update_flags.iso.warp_enable_2nd_updated == 1U)) {
        window_calculate_updated = 1U;
    }

    if((p_filters->update_flags.iso.hi_chroma_filter_low_updated == 1U) || (p_filters->update_flags.iso.hi_wide_chroma_filter_low == 1U) ||
       (p_filters->update_flags.iso.hi_wide_chroma_filter_combine_low == 1U) || (window_calculate_updated == 1U)) {
        ikc_in_chroma_noise_t in_chroma_noise;
        ikc_out_chroma_noise_t out_chroma_noise;
        ik_chroma_filter_t chroma_filter;
        uint64 sec2_luma_hscale_phase_inc = p_filters->input_param.ctx_buf.result_win.warp_phase.sec2_hscale_phase_inc;
        uint64 sec2_luma_vscale_phase_inc = p_filters->input_param.ctx_buf.result_win.warp_phase.sec2_vscale_phase_inc;
        uint32 chroma_radius = p_filters->input_param.chroma_filter.radius;
        uint64 sec2_chroma_hscale_phase_inc;
        uint64 sec2_chroma_vscale_phase_inc;

        if(p_filters->update_flags.cr.r2y.cr_36_update == 0U) {
            amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_chroma_noise in !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
            rval |= IK_ERR_0000;
        } else {
            amba_ik_system_memcpy(&chroma_filter, &p_filters->hiso_input_param.hi_chroma_filter_low, sizeof(ik_hi_chroma_filter_pre_t));
            chroma_filter.radius = p_filters->hiso_input_param.hi_chroma_filter_high.radius;
            in_chroma_noise.p_chroma_filter = &chroma_filter;
            in_chroma_noise.p_wide_chroma_filter = &p_filters->hiso_input_param.hi_wide_chroma_filter_low;
            in_chroma_noise.p_wide_chroma_combine = &p_filters->hiso_input_param.hi_wide_chroma_filter_combine_low;
            if ((in_chroma_noise.p_chroma_filter->radius==32u) && (in_chroma_noise.p_wide_chroma_filter->enable==1u)) {
                in_chroma_noise.is_wide_chroma_invalid = 1u;
            } else {
                in_chroma_noise.is_wide_chroma_invalid = 0u;
            }
            out_chroma_noise.p_cr_36 = p_cr->step4a.sec2.p_CR_buf_36;
            out_chroma_noise.cr_36_size = CR_SIZE_36;
            out_chroma_noise.p_window = &p_flow->window;
            rval |= ikc_chroma_noise(&in_chroma_noise, &out_chroma_noise);
            p_flow->window.update_chroma_radius = 1U;
            //check radius causes phase_inc overflow.
            sec2_chroma_hscale_phase_inc = (sec2_luma_hscale_phase_inc * (uint64)chroma_radius) >> 5UL;
            sec2_chroma_vscale_phase_inc = (sec2_luma_vscale_phase_inc * (uint64)chroma_radius) >> 5UL;
            if ((sec2_chroma_hscale_phase_inc >= (8UL<<13UL)) || (sec2_chroma_vscale_phase_inc >= (8UL<<13UL))) {
                amba_ik_system_print_uint32_5("[IK][ERROR] exe_iso_cfg_check_raw2yuv_chroma_filter() fail. One or more downscaling_factor is out of range(>=8<<13), please check chroma radius or window setting!", DC_U, DC_U, DC_U, DC_U, DC_U);
                amba_ik_system_print_uint32_5("[IK][ERROR] sec2_chroma_hscale_phase_inc:%d, sec2_chroma_vscale_phase_inc:%d, chroma_radius:%d", (uint32)sec2_chroma_hscale_phase_inc, (uint32)sec2_chroma_vscale_phase_inc, chroma_radius, DC_U, DC_U);
                rval = IK_ERR_0101;
            }
        }
    } else {
        p_flow->window.update_chroma_radius = 0U;
    }

    if((p_filters->update_flags.iso.chroma_filter_updated == 1U)||
       (p_filters->update_flags.iso.resample_str_update == 1U)||
       (p_filters->update_flags.iso.window_size_info_updated == 1U)||
       (window_calculate_updated == 1U)) {
        ikc_in_sec2_hiso_resampler_t in_sec2_hiso_resampler;
        ikc_out_sec2_main_resampler_t out_sec2_main_resampler;
        ikc_in_hiso_resampler_size_t  in_hiso_resampler;
        ikc_out_hiso_resampler_size_t out_hiso_resampler;

        in_hiso_resampler.step   = 4;//step4a
        in_hiso_resampler.p_main = &p_filters->input_param.window_size_info.main_win;
        in_hiso_resampler.chroma_filter         = &p_filters->input_param.chroma_filter;
        in_hiso_resampler.hi_nonsmooth_detect   = &p_filters->hiso_input_param.hi_nonsmooth_detect;
        in_hiso_resampler.hi_chroma_filter_high = &p_filters->hiso_input_param.hi_chroma_filter_high;

        rval |= ikc_hiso_resampler_size(&in_hiso_resampler, &out_hiso_resampler);

        in_sec2_hiso_resampler.step   = 4U;
        in_sec2_hiso_resampler.p_main = &out_hiso_resampler.output;
        in_sec2_hiso_resampler.main_cut_off_freq = p_filters->input_param.resample_str.main_cut_off_freq;
        in_sec2_hiso_resampler.chroma_radius = p_filters->input_param.chroma_filter.radius;
        out_sec2_main_resampler.p_cr_33 = p_cr->step4a.sec2.p_CR_buf_33;
        out_sec2_main_resampler.cr_33_size = CR_SIZE_33;
        out_sec2_main_resampler.p_cr_35 = p_cr->step4a.sec2.p_CR_buf_35;
        out_sec2_main_resampler.cr_35_size = CR_SIZE_35;
        out_sec2_main_resampler.p_cr_37 = p_cr->step4a.sec2.p_CR_buf_37;
        out_sec2_main_resampler.cr_37_size = CR_SIZE_37;
        rval |= ikc_sec2_hiso_resampler(&in_sec2_hiso_resampler, &out_sec2_main_resampler);
    }

    {
        ikc_in_hiso_frame_size_t in_hiso_frame_size;
        ikc_out_asf_t out_shpa_frame_size;
        in_hiso_frame_size.step = 4;  //step4a
        in_hiso_frame_size.chroma_filter         = &p_filters->input_param.chroma_filter;
        in_hiso_frame_size.hi_nonsmooth_detect   = &p_filters->hiso_input_param.hi_nonsmooth_detect;
        in_hiso_frame_size.hi_chroma_filter_high = &p_filters->hiso_input_param.hi_chroma_filter_high;
        in_hiso_frame_size.p_main                = &p_filters->input_param.window_size_info.main_win;
        out_shpa_frame_size.p_cr_30    = p_cr->step4a.sec2.p_CR_buf_30;
        out_shpa_frame_size.cr_30_size = CR_SIZE_30;
        rval |= ikc_hiso_shpa_frame_size(&in_hiso_frame_size, &out_shpa_frame_size);
    }

    return rval;
}

static uint32 exe_process_hiso_step5_cr(idsp_flow_ctrl_t *p_flow, const amba_ik_hiso_flow_tables_list_t *p_cr, amba_ik_filter_t *p_filters)
{
    uint32 rval = IK_OK;
    ikc_out_sharpen_t sec2_out_sharpen;
    ikc_in_hiso_sharpen_t  in_hiso_sharpen;
    uint32 window_calculate_updated = 0U;

    rval |= exe_chroma_noise_def_value(&p_cr->step5.sec2, p_filters);

    if((p_filters->update_flags.iso.rgb_to_yuv_matrix_updated == 1U) ||
       (p_filters->update_flags.iso.is_1st_frame == 1U)) {
        rval |= exe_process_init_y2y_rgb2yuv(&p_cr->step5.sec2, p_filters);
    }

    // exe_process_calib_cr
    if(p_filters->update_flags.iso.is_1st_frame == 1U) { //should only configure once at 1st time.
        rval |= exe_process_input_mode_y2y_step3(p_flow, &p_cr->step5.sec2, p_filters, 5u);
    }

    //======Noise======//
    //if(p_filters->hiso_input_param.hi_med_shpns_both.enable== 1U)
    {
        in_hiso_sharpen.sharpen_mode          = p_filters->hiso_input_param.hi_med_shpns_both.mode;
        in_hiso_sharpen.p_first_sharpen_noise = &p_filters->hiso_input_param.hi_med_shpns_noise;
        in_hiso_sharpen.p_first_sharpen_fir   = &p_filters->hiso_input_param.hi_med_shpns_fir;
        in_hiso_sharpen.p_working_buffer      = &p_filters->hiso_input_param.ctx_buf.first_sharpen_working_buffer[0];
        in_hiso_sharpen.working_buffer_size   = (uint32)sizeof(p_filters->hiso_input_param.ctx_buf.first_sharpen_working_buffer);

        sec2_out_sharpen.p_cr_30 = p_cr->step5.sec2.p_CR_buf_30;
        sec2_out_sharpen.cr_30_size = CR_SIZE_30;

        if(p_filters->update_flags.cr.r2y.cr_30_update == 0U) {
            amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_hiso_sharpen !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
            rval |= IK_ERR_0000;
        } else {
            rval |= ikc_hiso_sharpen(&in_hiso_sharpen, &sec2_out_sharpen);
        }

        //======both======//
        if(p_filters->update_flags.iso.hi_med_shpns_both_updated == 1U) {
            ikc_in_sharpen_both_t in_sharpen_both;

            if(p_filters->update_flags.cr.r2y.cr_30_update == 0U) {
                amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_sharpen_both !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
                rval |= IK_ERR_0000;
            } else {
                ik_hi_med_shpns_both_t *p_hi_med_shpns_both = &p_filters->hiso_input_param.hi_med_shpns_both;
                (void)amba_ik_system_memcpy(&in_sharpen_both.p_first_sharpen_both, &p_hi_med_shpns_both, sizeof(void*));
                rval |= ikc_sharpen_both(&in_sharpen_both, &sec2_out_sharpen);
                if(p_filters->hiso_input_param.hi_med_shpns_both.enable == 0U) {
                    rval |= ikc_sharpen_both_disable(&sec2_out_sharpen);
                }
            }
        }

        //======coring======//
        if(p_filters->update_flags.iso.hi_med_shpns_coring_updated == 1U) {
            ikc_in_sharpen_coring_t in_sharpen_coring;

            if(p_filters->update_flags.cr.r2y.cr_30_update == 0U) {
                amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_sharpen_coring !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
                rval |= IK_ERR_0000;
            } else {
                ik_hi_med_shpns_coring_t *p_hi_med_shpns_coring = &p_filters->hiso_input_param.hi_med_shpns_coring;
                (void)amba_ik_system_memcpy(&in_sharpen_coring.p_first_sharpen_coring, &p_hi_med_shpns_coring, sizeof(void*));
                rval |= ikc_sharpen_coring(&in_sharpen_coring, &sec2_out_sharpen);
            }
        }

        if(p_filters->update_flags.iso.hi_med_shpns_cor_idx_scl_updated == 1U) {
            ikc_in_sharpen_coring_idx_scale_t in_sharpen_coring_idx_scale;

            if(p_filters->update_flags.cr.r2y.cr_30_update == 0U) {
                amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_sharpen_coring_idx_scale !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
                rval |= IK_ERR_0000;
            } else {
                ik_hi_med_shpns_cor_idx_scl_t *p_hi_med_shpns_cor_idx_scl = &p_filters->hiso_input_param.hi_med_shpns_cor_idx_scl;
                (void)amba_ik_system_memcpy(&in_sharpen_coring_idx_scale.p_first_sharpen_coring_idx_scale, &p_hi_med_shpns_cor_idx_scl, sizeof(void*));
                rval |= ikc_sharpen_coring_idx_scale(&in_sharpen_coring_idx_scale, &sec2_out_sharpen);
            }
        }

        if(p_filters->update_flags.iso.hi_med_shpns_min_cor_rst_updated == 1U) {
            ikc_in_sharpen_min_coring_t in_sharpen_min_coring_result;

            if(p_filters->update_flags.cr.r2y.cr_30_update == 0U) {
                amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_sharpen_min_coring !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
                rval |= IK_ERR_0000;
            } else {
                ik_hi_med_shpns_min_cor_rst_t *p_hi_med_shpns_min_cor_rst = &p_filters->hiso_input_param.hi_med_shpns_min_cor_rst;
                (void)amba_ik_system_memcpy(&in_sharpen_min_coring_result.p_first_sharpen_min_coring_result, &p_hi_med_shpns_min_cor_rst, sizeof(void*));
                rval |= ikc_sharpen_min_coring(&in_sharpen_min_coring_result, &sec2_out_sharpen);
            }
        }

        if(p_filters->update_flags.iso.hi_med_shpns_max_cor_rst_updated == 1U) {
            ikc_in_sharpen_max_coring_t in_sharpen_max_coring_result;

            if(p_filters->update_flags.cr.r2y.cr_30_update == 0U) {
                amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_sharpen_max_coring !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
                rval |= IK_ERR_0000;
            } else {
                ik_hi_med_shpns_max_cor_rst_t *p_hi_med_shpns_max_cor_rst = &p_filters->hiso_input_param.hi_med_shpns_max_cor_rst;
                (void)amba_ik_system_memcpy(&in_sharpen_max_coring_result.p_first_sharpen_max_coring_result, &p_hi_med_shpns_max_cor_rst, sizeof(void*));
                rval |= ikc_sharpen_max_coring(&in_sharpen_max_coring_result, &sec2_out_sharpen);
            }
        }

        if(p_filters->update_flags.iso.hi_med_shpns_scl_cor_updated == 1U) {
            ikc_in_sharpen_scale_coring_t in_sharpen_scale_coring;

            if(p_filters->update_flags.cr.r2y.cr_30_update == 0U) {
                amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_sharpen_scale_coring !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
                rval |= IK_ERR_0000;
            } else {
                ik_hi_med_shpns_scl_cor_t *p_hi_med_shpns_scl_cor = &p_filters->hiso_input_param.hi_med_shpns_scl_cor;
                (void)amba_ik_system_memcpy(&in_sharpen_scale_coring.p_first_sharpen_scale_coring, &p_hi_med_shpns_scl_cor, sizeof(void*));
                rval |= ikc_sharpen_scale_coring(&in_sharpen_scale_coring, &sec2_out_sharpen);
            }
        }
    }

    if(p_filters->update_flags.iso.hi_low_asf_combine_updated == 1U) {
        ikc_in_luma_combine_2_mctf_t in_hi_low_asf_combine;
        ikc_out_mctf_t out_hi_low_asf_combine;
        in_hi_low_asf_combine.step = 5U;
        in_hi_low_asf_combine.p_hi_luma_filter_combine = &p_filters->hiso_input_param.hi_low_asf_combine;
        out_hi_low_asf_combine.p_cr_112 = p_cr->step5.sec18.p_CR_buf_112;
        out_hi_low_asf_combine.cr_112_size = CR_SIZE_112;
        rval |= ikc_luma_combine_2_mctf(&in_hi_low_asf_combine, &out_hi_low_asf_combine);
    }

    if(p_filters->update_flags.iso.hi_med2_asf_updated == 1U) {
        ikc_in_step5_asf_mcts   in_step5_asf_mcts;
        ikc_out_sec18_sharpen_t sec18_out_sharpen;

        if(p_filters->update_flags.cr.r2y.cr_117_update == 0U) {
            amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_sec18_step5_asf_mcts !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
            rval |= IK_ERR_0000;
        } else {
            if(p_filters->hiso_input_param.hi_med2_asf.enable == 1U) {
                in_step5_asf_mcts.p_in_hi_med2_asf = &p_filters->hiso_input_param.hi_med2_asf;
                sec18_out_sharpen.p_cr_117    = p_cr->step5.sec18.p_CR_buf_117;
                sec18_out_sharpen.cr_117_size = CR_SIZE_117;
                rval |= ikc_sec18_step5_asf_mcts(&in_step5_asf_mcts, &sec18_out_sharpen);
            } else {
                ikc_out_sec18_mctf_shpb_t out_shpb;
                ikc_in_shpb_pass_through_t  in_shpb_pass_through;
                in_shpb_pass_through.is_yuv_422 = 0u;
                in_shpb_pass_through.step       = 5u; //step5
                out_shpb.p_cr_112 = p_cr->step5.sec18.p_CR_buf_112;
                out_shpb.cr_112_size = CR_SIZE_112;
                out_shpb.p_cr_117 = p_cr->step5.sec18.p_CR_buf_117;
                out_shpb.cr_117_size = CR_SIZE_117;
                rval |= ikc_hiso_shpb_pass_through(&in_shpb_pass_through, &out_shpb);
            }
        }
    }

    if ((p_filters->update_flags.iso.window_size_info_updated == 1U) ||
        (p_filters->update_flags.iso.cfa_window_size_info_updated == 1U) ||
        (p_filters->update_flags.iso.dmy_range_updated == 1U) ||
        (p_filters->update_flags.iso.dzoom_info_updated == 1U) ||
        (p_filters->update_flags.iso.vin_active_win_updated == 1U) ||
        (p_filters->update_flags.iso.stitching_info_updated == 1U) ||
        (p_filters->update_flags.iso.overlap_x_updated == 1U) ||
        (p_filters->update_flags.iso.flip_mode_updated == 1U) ||
        (p_filters->update_flags.iso.warp_enable_updated == 1U) ||
        (p_filters->update_flags.iso.warp_enable_2nd_updated == 1U)) {
        window_calculate_updated = 1U;
    }

    if((p_filters->update_flags.iso.chroma_filter_updated == 1U)||
       (p_filters->update_flags.iso.resample_str_update == 1U)||
       (p_filters->update_flags.iso.window_size_info_updated == 1U)||
       (window_calculate_updated == 1U)) {
        ikc_in_sec2_hiso_resampler_t in_sec2_hiso_resampler;
        ikc_out_sec2_main_resampler_t out_sec2_main_resampler;
        ikc_in_hiso_resampler_size_t  in_hiso_resampler;
        ikc_out_hiso_resampler_size_t out_hiso_resampler;

        in_hiso_resampler.step   = 5;//step5
        in_hiso_resampler.p_main = &p_filters->input_param.window_size_info.main_win;
        in_hiso_resampler.chroma_filter         = &p_filters->input_param.chroma_filter;
        in_hiso_resampler.hi_nonsmooth_detect   = &p_filters->hiso_input_param.hi_nonsmooth_detect;
        in_hiso_resampler.hi_chroma_filter_high = &p_filters->hiso_input_param.hi_chroma_filter_high;

        rval |= ikc_hiso_resampler_size(&in_hiso_resampler, &out_hiso_resampler);

        in_sec2_hiso_resampler.step   = 5U;
        in_sec2_hiso_resampler.main_cut_off_freq = p_filters->input_param.resample_str.main_cut_off_freq;
        in_sec2_hiso_resampler.p_main = &out_hiso_resampler.output;
        in_sec2_hiso_resampler.chroma_radius = p_filters->input_param.chroma_filter.radius;
        out_sec2_main_resampler.p_cr_33 = p_cr->step5.sec2.p_CR_buf_33;
        out_sec2_main_resampler.cr_33_size = CR_SIZE_33;
        out_sec2_main_resampler.p_cr_35 = p_cr->step5.sec2.p_CR_buf_35;
        out_sec2_main_resampler.cr_35_size = CR_SIZE_35;
        out_sec2_main_resampler.p_cr_37 = p_cr->step5.sec2.p_CR_buf_37;
        out_sec2_main_resampler.cr_37_size = CR_SIZE_37;
        rval |= ikc_sec2_hiso_resampler(&in_sec2_hiso_resampler, &out_sec2_main_resampler);
    }

    {
        ikc_in_hiso_frame_size_t in_hiso_frame_size;
        ikc_out_asf_t out_shpa_frame_size;
        ikc_out_sec18_mctf_shpb_t out_frame_size_div16_m1;
        in_hiso_frame_size.step = 5;  //step5
        in_hiso_frame_size.chroma_filter         = &p_filters->input_param.chroma_filter;
        in_hiso_frame_size.hi_nonsmooth_detect   = &p_filters->hiso_input_param.hi_nonsmooth_detect;
        in_hiso_frame_size.hi_chroma_filter_high = &p_filters->hiso_input_param.hi_chroma_filter_high;
        in_hiso_frame_size.p_main                = &p_filters->input_param.window_size_info.main_win;
        out_shpa_frame_size.p_cr_30    = p_cr->step5.sec2.p_CR_buf_30;
        out_shpa_frame_size.cr_30_size = CR_SIZE_30;
        rval |= ikc_hiso_shpa_frame_size(&in_hiso_frame_size, &out_shpa_frame_size);
        out_frame_size_div16_m1.p_cr_112    = p_cr->step5.sec18.p_CR_buf_112;
        out_frame_size_div16_m1.p_cr_117    = p_cr->step5.sec18.p_CR_buf_117;
        out_frame_size_div16_m1.cr_112_size = CR_SIZE_112;
        out_frame_size_div16_m1.cr_117_size = CR_SIZE_117;
        rval |= ikc_hiso_sec18_frame_size_div16_m1(&in_hiso_frame_size, &out_frame_size_div16_m1);
    }
    {
        ikc_out_sec18_mctf_shpb_t out_mctf;
        ikc_in_sec18_chroma_format_t in_sec18_chroma_format;

        in_sec18_chroma_format.step = 5u; //step5
        out_mctf.p_cr_112 = p_cr->step5.sec18.p_CR_buf_112;
        out_mctf.cr_112_size = CR_SIZE_112;
        out_mctf.p_cr_117 = p_cr->step5.sec18.p_CR_buf_117;
        out_mctf.cr_117_size = CR_SIZE_117;
        rval |= ikc_hiso_sec18_chroma_format(&in_sec18_chroma_format, &out_mctf);
    }
    {
        ikc_out_sec18_mctf_shpb_t out_mctf;
        out_mctf.p_cr_112 = p_cr->step5.sec18.p_CR_buf_112;
        out_mctf.cr_112_size = CR_SIZE_112;
        out_mctf.p_cr_117 = p_cr->step5.sec18.p_CR_buf_117;
        out_mctf.cr_117_size = CR_SIZE_117;
        rval |= ikc_hiso_mctf_ta_constraints(&out_mctf);
    }
    {
        ikc_in_def_t in_input;
        ikc_out_sec18_mctf_shpb_t out_mctf;
        in_input.step = 5U;
        out_mctf.p_cr_112 = p_cr->step5.sec18.p_CR_buf_112;
        out_mctf.cr_112_size = CR_SIZE_112;
        out_mctf.p_cr_117 = p_cr->step5.sec18.p_CR_buf_117;
        out_mctf.cr_117_size = CR_SIZE_117;
        rval |= ikc_hiso_set_regs_that_mctf_rtl_needs(&in_input, &out_mctf);
    }
    return rval;
}

static uint32 exe_process_hiso_step6_cr(idsp_flow_ctrl_t *p_flow, const amba_ik_hiso_flow_tables_list_t *p_cr, amba_ik_filter_t *p_filters)
{
    uint32 rval = IK_OK;
    uint32 window_calculate_updated = 0U;

    rval |= exe_chroma_noise_def_value(&p_cr->step6.sec2, p_filters);

    if((p_filters->update_flags.iso.rgb_to_yuv_matrix_updated == 1U) ||
       (p_filters->update_flags.iso.is_1st_frame == 1U)) {
        rval |= exe_process_init_y2y_rgb2yuv(&p_cr->step6.sec2, p_filters);
    }

    // exe_process_calib_cr
    if(p_filters->update_flags.iso.is_1st_frame == 1U) { //should only configure once at 1st time.
        rval |= exe_process_input_mode_y2y_step3(p_flow, &p_cr->step6.sec2, p_filters, 6u);
    }

    if ((p_filters->update_flags.iso.window_size_info_updated == 1U) ||
        (p_filters->update_flags.iso.cfa_window_size_info_updated == 1U) ||
        (p_filters->update_flags.iso.dmy_range_updated == 1U) ||
        (p_filters->update_flags.iso.dzoom_info_updated == 1U) ||
        (p_filters->update_flags.iso.vin_active_win_updated == 1U) ||
        (p_filters->update_flags.iso.stitching_info_updated == 1U) ||
        (p_filters->update_flags.iso.overlap_x_updated == 1U) ||
        (p_filters->update_flags.iso.flip_mode_updated == 1U) ||
        (p_filters->update_flags.iso.warp_enable_updated == 1U) ||
        (p_filters->update_flags.iso.warp_enable_2nd_updated == 1U)) {
        window_calculate_updated = 1U;
    }

    if((p_filters->update_flags.iso.hi_chroma_filter_very_low_updated == 1U) || (p_filters->update_flags.iso.hi_wide_chroma_filter_very_low == 1U) ||
       (p_filters->update_flags.iso.hi_wide_chroma_filter_combine_very_low == 1U) || (window_calculate_updated == 1U)) {
        ikc_in_chroma_noise_t in_chroma_noise;
        ikc_out_chroma_noise_t out_chroma_noise;
        ik_chroma_filter_t chroma_filter;
        uint64 sec2_luma_hscale_phase_inc = p_filters->input_param.ctx_buf.result_win.warp_phase.sec2_hscale_phase_inc;
        uint64 sec2_luma_vscale_phase_inc = p_filters->input_param.ctx_buf.result_win.warp_phase.sec2_vscale_phase_inc;
        uint32 chroma_radius = p_filters->input_param.chroma_filter.radius;
        uint64 sec2_chroma_hscale_phase_inc;
        uint64 sec2_chroma_vscale_phase_inc;

        if(p_filters->update_flags.cr.r2y.cr_36_update == 0U) {
            amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_chroma_noise !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
            rval |= IK_ERR_0000;
        } else {
            amba_ik_system_memcpy(&chroma_filter, &p_filters->hiso_input_param.hi_chroma_filter_very_low, sizeof(ik_hi_chroma_filter_pre_t));
            chroma_filter.radius = p_filters->hiso_input_param.hi_chroma_filter_high.radius;
            in_chroma_noise.p_chroma_filter = &chroma_filter;
            in_chroma_noise.p_wide_chroma_filter = &p_filters->hiso_input_param.hi_wide_chroma_filter_very_low;
            in_chroma_noise.p_wide_chroma_combine = &p_filters->hiso_input_param.hi_wide_chroma_filter_combine_very_low;
            if ((in_chroma_noise.p_chroma_filter->radius==32u) && (in_chroma_noise.p_wide_chroma_filter->enable==1u)) {
                in_chroma_noise.is_wide_chroma_invalid = 1u;
            } else {
                in_chroma_noise.is_wide_chroma_invalid = 0u;
            }
            out_chroma_noise.p_cr_36 = p_cr->step6.sec2.p_CR_buf_36;
            out_chroma_noise.cr_36_size = CR_SIZE_36;
            out_chroma_noise.p_window = &p_flow->window;
            rval |= ikc_chroma_noise(&in_chroma_noise, &out_chroma_noise);
            p_flow->window.update_chroma_radius = 1U;
            //check radius causes phase_inc overflow.
            sec2_chroma_hscale_phase_inc = (sec2_luma_hscale_phase_inc * (uint64)chroma_radius) >> 5UL;
            sec2_chroma_vscale_phase_inc = (sec2_luma_vscale_phase_inc * (uint64)chroma_radius) >> 5UL;
            if ((sec2_chroma_hscale_phase_inc >= (8UL<<13UL)) || (sec2_chroma_vscale_phase_inc >= (8UL<<13UL))) {
                amba_ik_system_print_uint32_5("[IK][ERROR] exe_iso_cfg_check_raw2yuv_chroma_filter() fail. One or more downscaling_factor is out of range(>=8<<13), please check chroma radius or window setting!", DC_U, DC_U, DC_U, DC_U, DC_U);
                amba_ik_system_print_uint32_5("[IK][ERROR] sec2_chroma_hscale_phase_inc:%d, sec2_chroma_vscale_phase_inc:%d, chroma_radius:%d", (uint32)sec2_chroma_hscale_phase_inc, (uint32)sec2_chroma_vscale_phase_inc, chroma_radius, DC_U, DC_U);
                rval = IK_ERR_0101;
            }
        }
    } else {
        p_flow->window.update_chroma_radius = 0U;
    }

    if(p_filters->update_flags.iso.hi_chroma_fltr_very_low_com_updated == 1U) {
        ikc_in_chroma_combine_2_mctf_t in_hi_chroma_filter_combine;
        ikc_out_mctf_t out_hi_chroma_filter_combine;

        if(p_filters->update_flags.cr.r2y.cr_112_update == 0U) {
            amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on step6 ikc_chroma_combine_2_mctf !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
            rval |= IK_ERR_0000;
        } else {
            in_hi_chroma_filter_combine.p_hi_chroma_filter_combine = &p_filters->hiso_input_param.hi_chroma_fltr_very_low_com;
            out_hi_chroma_filter_combine.p_cr_112 = p_cr->step6.sec18.p_CR_buf_112;
            out_hi_chroma_filter_combine.cr_112_size = CR_SIZE_112;
            rval |= ikc_chroma_combine_2_mctf(&in_hi_chroma_filter_combine, &out_hi_chroma_filter_combine);
        }
    }

    if((p_filters->update_flags.iso.chroma_filter_updated == 1U)||
       (p_filters->update_flags.iso.resample_str_update == 1U)||
       (p_filters->update_flags.iso.window_size_info_updated == 1U)||
       (window_calculate_updated == 1U)) {
        ikc_in_sec2_hiso_resampler_t in_sec2_hiso_resampler;
        ikc_out_sec2_main_resampler_t out_sec2_main_resampler;
        ikc_in_hiso_resampler_size_t  in_hiso_resampler;
        ikc_out_hiso_resampler_size_t out_hiso_resampler;

        in_hiso_resampler.step   = 6;//step6
        in_hiso_resampler.p_main = &p_filters->input_param.window_size_info.main_win;
        in_hiso_resampler.chroma_filter         = &p_filters->input_param.chroma_filter;
        in_hiso_resampler.hi_nonsmooth_detect   = &p_filters->hiso_input_param.hi_nonsmooth_detect;
        in_hiso_resampler.hi_chroma_filter_high = &p_filters->hiso_input_param.hi_chroma_filter_high;

        rval |= ikc_hiso_resampler_size(&in_hiso_resampler, &out_hiso_resampler);

        in_sec2_hiso_resampler.step   = 6U;
        in_sec2_hiso_resampler.main_cut_off_freq = p_filters->input_param.resample_str.main_cut_off_freq;
        in_sec2_hiso_resampler.p_main = &out_hiso_resampler.output;
        in_sec2_hiso_resampler.chroma_radius = p_filters->input_param.chroma_filter.radius;
        out_sec2_main_resampler.p_cr_33 = p_cr->step6.sec2.p_CR_buf_33;
        out_sec2_main_resampler.cr_33_size = CR_SIZE_33;
        out_sec2_main_resampler.p_cr_35 = p_cr->step6.sec2.p_CR_buf_35;
        out_sec2_main_resampler.cr_35_size = CR_SIZE_35;
        out_sec2_main_resampler.p_cr_37 = p_cr->step6.sec2.p_CR_buf_37;
        out_sec2_main_resampler.cr_37_size = CR_SIZE_37;
        rval |= ikc_sec2_hiso_resampler(&in_sec2_hiso_resampler, &out_sec2_main_resampler);
    }

    {
        ikc_out_sec18_mctf_shpb_t out_shpb;
        ikc_in_shpb_pass_through_t  in_shpb_pass_through;
        in_shpb_pass_through.is_yuv_422 = 1u;
        in_shpb_pass_through.step       = 6u;
        out_shpb.p_cr_112 = p_cr->step6.sec18.p_CR_buf_112;
        out_shpb.cr_112_size = CR_SIZE_112;
        out_shpb.p_cr_117 = p_cr->step6.sec18.p_CR_buf_117;
        out_shpb.cr_117_size = CR_SIZE_117;
        rval |= ikc_hiso_shpb_pass_through(&in_shpb_pass_through, &out_shpb);
    }
    {
        ikc_in_hiso_frame_size_t in_hiso_frame_size;
        ikc_out_asf_t out_shpa_frame_size;
        ikc_out_sec18_mctf_shpb_t out_frame_size_div16_m1;
        in_hiso_frame_size.step = 6;  //step6
        in_hiso_frame_size.chroma_filter         = &p_filters->input_param.chroma_filter;
        in_hiso_frame_size.hi_nonsmooth_detect   = &p_filters->hiso_input_param.hi_nonsmooth_detect;
        in_hiso_frame_size.hi_chroma_filter_high = &p_filters->hiso_input_param.hi_chroma_filter_high;
        in_hiso_frame_size.p_main                = &p_filters->input_param.window_size_info.main_win;
        out_shpa_frame_size.p_cr_30    = p_cr->step6.sec2.p_CR_buf_30;
        out_shpa_frame_size.cr_30_size = CR_SIZE_30;
        rval |= ikc_hiso_shpa_frame_size(&in_hiso_frame_size, &out_shpa_frame_size);
        out_frame_size_div16_m1.p_cr_112    = p_cr->step6.sec18.p_CR_buf_112;
        out_frame_size_div16_m1.p_cr_117    = p_cr->step6.sec18.p_CR_buf_117;
        out_frame_size_div16_m1.cr_112_size = CR_SIZE_112;
        out_frame_size_div16_m1.cr_117_size = CR_SIZE_117;
        rval |= ikc_hiso_sec18_frame_size_div16_m1(&in_hiso_frame_size, &out_frame_size_div16_m1);
    }
    {
        ikc_out_sec18_mctf_shpb_t out_mctf;
        out_mctf.p_cr_112 = p_cr->step6.sec18.p_CR_buf_112;
        out_mctf.cr_112_size = CR_SIZE_112;
        out_mctf.p_cr_117 = p_cr->step6.sec18.p_CR_buf_117;
        out_mctf.cr_117_size = CR_SIZE_117;
        rval |= ikc_hiso_mctf_ta_constraints(&out_mctf);
    }
    return rval;

}

static uint32 exe_process_hiso_step7_cr(idsp_flow_ctrl_t *p_flow, const amba_ik_hiso_flow_tables_list_t *p_cr, amba_ik_filter_t *p_filters)
{
    uint32 rval = IK_OK;
    uint32 window_calculate_updated = 0U;

    rval |= exe_chroma_noise_def_value(&p_cr->step7.sec2, p_filters);

    if((p_filters->update_flags.iso.rgb_to_yuv_matrix_updated == 1U) ||
       (p_filters->update_flags.iso.is_1st_frame == 1U)) {
        rval |= exe_process_init_y2y_rgb2yuv(&p_cr->step7.sec2, p_filters);
    }

    // exe_process_calib_cr
    if(p_filters->update_flags.iso.is_1st_frame == 1U) { //should only configure once at 1st time.
        rval |= exe_process_input_mode_y2y_step3(p_flow, &p_cr->step7.sec2, p_filters, 7u);
    }

    if ((p_filters->update_flags.iso.window_size_info_updated == 1U) ||
        (p_filters->update_flags.iso.cfa_window_size_info_updated == 1U) ||
        (p_filters->update_flags.iso.dmy_range_updated == 1U) ||
        (p_filters->update_flags.iso.dzoom_info_updated == 1U) ||
        (p_filters->update_flags.iso.vin_active_win_updated == 1U) ||
        (p_filters->update_flags.iso.stitching_info_updated == 1U) ||
        (p_filters->update_flags.iso.overlap_x_updated == 1U) ||
        (p_filters->update_flags.iso.flip_mode_updated == 1U) ||
        (p_filters->update_flags.iso.warp_enable_updated == 1U) ||
        (p_filters->update_flags.iso.warp_enable_2nd_updated == 1U)) {
        window_calculate_updated = 1U;
    }

    if((p_filters->update_flags.iso.hi_chroma_filter_med_updated == 1U) || (p_filters->update_flags.iso.hi_wide_chroma_filter_med == 1U) ||
       (p_filters->update_flags.iso.hi_wide_chroma_filter_combine_med == 1U) || (window_calculate_updated == 1U)) {
        ikc_in_chroma_noise_t in_chroma_noise;
        ikc_out_chroma_noise_t out_chroma_noise;
        ik_chroma_filter_t chroma_filter;
        uint64 sec2_luma_hscale_phase_inc = p_filters->input_param.ctx_buf.result_win.warp_phase.sec2_hscale_phase_inc;
        uint64 sec2_luma_vscale_phase_inc = p_filters->input_param.ctx_buf.result_win.warp_phase.sec2_vscale_phase_inc;
        uint32 chroma_radius = p_filters->input_param.chroma_filter.radius;
        uint64 sec2_chroma_hscale_phase_inc;
        uint64 sec2_chroma_vscale_phase_inc;

        if(p_filters->update_flags.cr.r2y.cr_36_update == 0U) {
            amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_chroma_noise !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
            rval |= IK_ERR_0000;
        } else {
            amba_ik_system_memcpy(&chroma_filter, &p_filters->hiso_input_param.hi_chroma_filter_med, sizeof(ik_hi_chroma_filter_pre_t));
            chroma_filter.radius = p_filters->hiso_input_param.hi_chroma_filter_high.radius;
            in_chroma_noise.p_chroma_filter = &chroma_filter;
            in_chroma_noise.p_wide_chroma_filter = &p_filters->hiso_input_param.hi_wide_chroma_filter_med;
            in_chroma_noise.p_wide_chroma_combine = &p_filters->hiso_input_param.hi_wide_chroma_filter_combine_med;
            if ((in_chroma_noise.p_chroma_filter->radius==32u) && (in_chroma_noise.p_wide_chroma_filter->enable==1u)) {
                in_chroma_noise.is_wide_chroma_invalid = 1u;
            } else {
                in_chroma_noise.is_wide_chroma_invalid = 0u;
            }
            out_chroma_noise.p_cr_36 = p_cr->step7.sec2.p_CR_buf_36;
            out_chroma_noise.cr_36_size = CR_SIZE_36;
            out_chroma_noise.p_window = &p_flow->window;
            rval |= ikc_chroma_noise(&in_chroma_noise, &out_chroma_noise);
            p_flow->window.update_chroma_radius = 1U;
            //check radius causes phase_inc overflow.
            sec2_chroma_hscale_phase_inc = (sec2_luma_hscale_phase_inc * (uint64)chroma_radius) >> 5UL;
            sec2_chroma_vscale_phase_inc = (sec2_luma_vscale_phase_inc * (uint64)chroma_radius) >> 5UL;
            if ((sec2_chroma_hscale_phase_inc >= (8UL<<13UL)) || (sec2_chroma_vscale_phase_inc >= (8UL<<13UL))) {
                amba_ik_system_print_uint32_5("[IK][ERROR] exe_iso_cfg_check_raw2yuv_chroma_filter() fail. One or more downscaling_factor is out of range(>=8<<13), please check chroma radius or window setting!", DC_U, DC_U, DC_U, DC_U, DC_U);
                amba_ik_system_print_uint32_5("[IK][ERROR] sec2_chroma_hscale_phase_inc:%d, sec2_chroma_vscale_phase_inc:%d, chroma_radius:%d", (uint32)sec2_chroma_hscale_phase_inc, (uint32)sec2_chroma_vscale_phase_inc, chroma_radius, DC_U, DC_U);
                rval = IK_ERR_0101;
            }
        }
    } else {
        p_flow->window.update_chroma_radius = 0U;
    }

    if(p_filters->update_flags.iso.hi_chroma_fltr_low_com_updated == 1U) {
        ikc_in_chroma_combine_2_mctf_t in_hi_chroma_filter_combine;
        ikc_out_mctf_t out_hi_chroma_filter_combine;

        if(p_filters->update_flags.cr.r2y.cr_112_update == 0U) {
            amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on step7 ikc_chroma_combine_2_mctf !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
            rval |= IK_ERR_0000;
        } else {
            in_hi_chroma_filter_combine.p_hi_chroma_filter_combine = &p_filters->hiso_input_param.hi_chroma_fltr_low_com;
            out_hi_chroma_filter_combine.p_cr_112 = p_cr->step7.sec18.p_CR_buf_112;
            out_hi_chroma_filter_combine.cr_112_size = CR_SIZE_112;
            rval |= ikc_chroma_combine_2_mctf(&in_hi_chroma_filter_combine, &out_hi_chroma_filter_combine);
        }
    }

    if((p_filters->update_flags.iso.chroma_filter_updated == 1U)||
       (p_filters->update_flags.iso.resample_str_update == 1U)||
       (p_filters->update_flags.iso.window_size_info_updated == 1U)||
       (window_calculate_updated == 1U)) {
        ikc_in_sec2_hiso_resampler_t in_sec2_hiso_resampler;
        ikc_out_sec2_main_resampler_t out_sec2_main_resampler;
        ikc_in_hiso_resampler_size_t  in_hiso_resampler;
        ikc_out_hiso_resampler_size_t out_hiso_resampler;

        in_hiso_resampler.step   = 7;//step7
        in_hiso_resampler.p_main = &p_filters->input_param.window_size_info.main_win;
        in_hiso_resampler.chroma_filter         = &p_filters->input_param.chroma_filter;
        in_hiso_resampler.hi_nonsmooth_detect   = &p_filters->hiso_input_param.hi_nonsmooth_detect;
        in_hiso_resampler.hi_chroma_filter_high = &p_filters->hiso_input_param.hi_chroma_filter_high;

        rval |= ikc_hiso_resampler_size(&in_hiso_resampler, &out_hiso_resampler);

        in_sec2_hiso_resampler.step   = 7U;
        in_sec2_hiso_resampler.main_cut_off_freq = p_filters->input_param.resample_str.main_cut_off_freq;
        in_sec2_hiso_resampler.p_main = &out_hiso_resampler.output;
        in_sec2_hiso_resampler.chroma_radius = p_filters->input_param.chroma_filter.radius;
        out_sec2_main_resampler.p_cr_33 = p_cr->step7.sec2.p_CR_buf_33;
        out_sec2_main_resampler.cr_33_size = CR_SIZE_33;
        out_sec2_main_resampler.p_cr_35 = p_cr->step7.sec2.p_CR_buf_35;
        out_sec2_main_resampler.cr_35_size = CR_SIZE_35;
        out_sec2_main_resampler.p_cr_37 = p_cr->step7.sec2.p_CR_buf_37;
        out_sec2_main_resampler.cr_37_size = CR_SIZE_37;
        rval |= ikc_sec2_hiso_resampler(&in_sec2_hiso_resampler, &out_sec2_main_resampler);
    }
    {
        ikc_in_hiso_frame_size_t in_hiso_frame_size;
        ikc_out_asf_t out_shpa_frame_size;
        ikc_out_sec18_mctf_shpb_t out_frame_size_div16_m1;
        in_hiso_frame_size.step = 7;  //step7
        in_hiso_frame_size.chroma_filter         = &p_filters->input_param.chroma_filter;
        in_hiso_frame_size.hi_nonsmooth_detect   = &p_filters->hiso_input_param.hi_nonsmooth_detect;
        in_hiso_frame_size.hi_chroma_filter_high = &p_filters->hiso_input_param.hi_chroma_filter_high;
        in_hiso_frame_size.p_main                = &p_filters->input_param.window_size_info.main_win;
        out_shpa_frame_size.p_cr_30    = p_cr->step7.sec2.p_CR_buf_30;
        out_shpa_frame_size.cr_30_size = CR_SIZE_30;
        rval |= ikc_hiso_shpa_frame_size(&in_hiso_frame_size, &out_shpa_frame_size);
        out_frame_size_div16_m1.p_cr_112    = p_cr->step7.sec18.p_CR_buf_112;
        out_frame_size_div16_m1.p_cr_117    = p_cr->step7.sec18.p_CR_buf_117;
        out_frame_size_div16_m1.cr_112_size = CR_SIZE_112;
        out_frame_size_div16_m1.cr_117_size = CR_SIZE_117;
        rval |= ikc_hiso_sec18_frame_size_div16_m1(&in_hiso_frame_size, &out_frame_size_div16_m1);
    }
    {
        ikc_out_sec18_mctf_shpb_t out_shpb;
        ikc_in_shpb_pass_through_t  in_shpb_pass_through;
        in_shpb_pass_through.is_yuv_422 = 1u;
        in_shpb_pass_through.step       = 7u;
        out_shpb.p_cr_112 = p_cr->step7.sec18.p_CR_buf_112;
        out_shpb.cr_112_size = CR_SIZE_112;
        out_shpb.p_cr_117 = p_cr->step7.sec18.p_CR_buf_117;
        out_shpb.cr_117_size = CR_SIZE_117;
        rval |= ikc_hiso_shpb_pass_through(&in_shpb_pass_through, &out_shpb);
    }
    {
        ikc_out_sec18_mctf_shpb_t out_mctf;
        out_mctf.p_cr_112 = p_cr->step7.sec18.p_CR_buf_112;
        out_mctf.cr_112_size = CR_SIZE_112;
        out_mctf.p_cr_117 = p_cr->step7.sec18.p_CR_buf_117;
        out_mctf.cr_117_size = CR_SIZE_117;
        rval |= ikc_hiso_mctf_ta_constraints(&out_mctf);
    }
    return rval;
}

static uint32 exe_process_hiso_step8_cr(idsp_flow_ctrl_t *p_flow, const amba_ik_hiso_flow_tables_list_t *p_cr, amba_ik_filter_t *p_filters)
{
    uint32 rval = IK_OK;
    uint32 window_calculate_updated = 0U;

    rval |= exe_chroma_noise_def_value(&p_cr->step8.sec2, p_filters);

    if((p_filters->update_flags.iso.rgb_to_yuv_matrix_updated == 1U) ||
       (p_filters->update_flags.iso.is_1st_frame == 1U)) {
        rval |= exe_process_init_y2y_rgb2yuv(&p_cr->step8.sec2, p_filters);
    }

    // exe_process_calib_cr
    if(p_filters->update_flags.iso.is_1st_frame == 1U) { //should only configure once at 1st time.
        rval |= exe_process_input_mode_y2y_step3(p_flow, &p_cr->step8.sec2, p_filters, 8u);
    }

    if ((p_filters->update_flags.iso.window_size_info_updated == 1U) ||
        (p_filters->update_flags.iso.cfa_window_size_info_updated == 1U) ||
        (p_filters->update_flags.iso.dmy_range_updated == 1U) ||
        (p_filters->update_flags.iso.dzoom_info_updated == 1U) ||
        (p_filters->update_flags.iso.vin_active_win_updated == 1U) ||
        (p_filters->update_flags.iso.stitching_info_updated == 1U) ||
        (p_filters->update_flags.iso.overlap_x_updated == 1U) ||
        (p_filters->update_flags.iso.flip_mode_updated == 1U) ||
        (p_filters->update_flags.iso.warp_enable_updated == 1U) ||
        (p_filters->update_flags.iso.warp_enable_2nd_updated == 1U)) {
        window_calculate_updated = 1U;
    }

    if((p_filters->update_flags.iso.hi_chroma_filter_high_updated == 1U) || (p_filters->update_flags.iso.hi_wide_chroma_filter_high == 1U) ||
       (p_filters->update_flags.iso.hi_wide_chroma_filter_combine_high == 1U) || (window_calculate_updated == 1U)) {
        ikc_in_chroma_noise_t in_chroma_noise;
        ikc_out_chroma_noise_t out_chroma_noise;
        ik_chroma_filter_t chroma_filter;
        uint64 sec2_luma_hscale_phase_inc = p_filters->input_param.ctx_buf.result_win.warp_phase.sec2_hscale_phase_inc;
        uint64 sec2_luma_vscale_phase_inc = p_filters->input_param.ctx_buf.result_win.warp_phase.sec2_vscale_phase_inc;
        uint32 chroma_radius = p_filters->input_param.chroma_filter.radius;
        uint64 sec2_chroma_hscale_phase_inc;
        uint64 sec2_chroma_vscale_phase_inc;

        if(p_filters->update_flags.cr.r2y.cr_36_update == 0U) {
            amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on step8 ikc_chroma_noise !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
            rval |= IK_ERR_0000;
        } else {
            amba_ik_system_memcpy(&chroma_filter, &p_filters->hiso_input_param.hi_chroma_filter_high, sizeof(ik_hi_chroma_filter_pre_t));
            chroma_filter.radius = p_filters->hiso_input_param.hi_chroma_filter_high.radius;
            in_chroma_noise.p_chroma_filter = &chroma_filter;
            in_chroma_noise.p_wide_chroma_filter = &p_filters->hiso_input_param.hi_wide_chroma_filter_high;
            in_chroma_noise.p_wide_chroma_combine = &p_filters->hiso_input_param.hi_wide_chroma_filter_combine_high;
            if ((in_chroma_noise.p_chroma_filter->radius==32u) && (in_chroma_noise.p_wide_chroma_filter->enable==1u)) {
                in_chroma_noise.is_wide_chroma_invalid = 1u;
            } else {
                in_chroma_noise.is_wide_chroma_invalid = 0u;
            }
            out_chroma_noise.p_cr_36 = p_cr->step8.sec2.p_CR_buf_36;
            out_chroma_noise.cr_36_size = CR_SIZE_36;
            out_chroma_noise.p_window = &p_flow->window;
            rval |= ikc_chroma_noise(&in_chroma_noise, &out_chroma_noise);
            p_flow->window.update_chroma_radius = 1U;
            //check radius causes phase_inc overflow.
            sec2_chroma_hscale_phase_inc = (sec2_luma_hscale_phase_inc * (uint64)chroma_radius) >> 5UL;
            sec2_chroma_vscale_phase_inc = (sec2_luma_vscale_phase_inc * (uint64)chroma_radius) >> 5UL;
            if ((sec2_chroma_hscale_phase_inc >= (8UL<<13UL)) || (sec2_chroma_vscale_phase_inc >= (8UL<<13UL))) {
                amba_ik_system_print_uint32_5("[IK][ERROR] exe_iso_cfg_check_raw2yuv_chroma_filter() fail. One or more downscaling_factor is out of range(>=8<<13), please check chroma radius or window setting!", DC_U, DC_U, DC_U, DC_U, DC_U);
                amba_ik_system_print_uint32_5("[IK][ERROR] sec2_chroma_hscale_phase_inc:%d, sec2_chroma_vscale_phase_inc:%d, chroma_radius:%d", (uint32)sec2_chroma_hscale_phase_inc, (uint32)sec2_chroma_vscale_phase_inc, chroma_radius, DC_U, DC_U);
                rval = IK_ERR_0101;
            }
        }
    } else {
        p_flow->window.update_chroma_radius = 0U;
    }

    if(p_filters->update_flags.iso.hi_chroma_fltr_med_com_updated == 1U) {
        ikc_in_chroma_combine_2_mctf_t in_hi_chroma_filter_combine;
        ikc_out_mctf_t out_hi_chroma_filter_combine;

        if(p_filters->update_flags.cr.r2y.cr_112_update == 0U) {
            amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on step8 ikc_chroma_combine_2_mctf !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
            rval |= IK_ERR_0000;
        } else {
            in_hi_chroma_filter_combine.p_hi_chroma_filter_combine = &p_filters->hiso_input_param.hi_chroma_fltr_med_com;
            out_hi_chroma_filter_combine.p_cr_112 = p_cr->step8.sec18.p_CR_buf_112;
            out_hi_chroma_filter_combine.cr_112_size = CR_SIZE_112;
            rval |= ikc_chroma_combine_2_mctf(&in_hi_chroma_filter_combine, &out_hi_chroma_filter_combine);
        }
    }
    if((p_filters->update_flags.iso.chroma_filter_updated == 1U)||
       (p_filters->update_flags.iso.resample_str_update == 1U)||
       (p_filters->update_flags.iso.window_size_info_updated == 1U)||
       (window_calculate_updated == 1U)) {
        ikc_in_sec2_hiso_resampler_t in_sec2_hiso_resampler;
        ikc_out_sec2_main_resampler_t out_sec2_main_resampler;
        ikc_in_hiso_resampler_size_t  in_hiso_resampler;
        ikc_out_hiso_resampler_size_t out_hiso_resampler;

        in_hiso_resampler.step   = 8;//step8
        in_hiso_resampler.p_main = &p_filters->input_param.window_size_info.main_win;
        in_hiso_resampler.chroma_filter         = &p_filters->input_param.chroma_filter;
        in_hiso_resampler.hi_nonsmooth_detect   = &p_filters->hiso_input_param.hi_nonsmooth_detect;
        in_hiso_resampler.hi_chroma_filter_high = &p_filters->hiso_input_param.hi_chroma_filter_high;

        rval |= ikc_hiso_resampler_size(&in_hiso_resampler, &out_hiso_resampler);

        in_sec2_hiso_resampler.step   = 8U;
        in_sec2_hiso_resampler.main_cut_off_freq = p_filters->input_param.resample_str.main_cut_off_freq;
        in_sec2_hiso_resampler.p_main = &out_hiso_resampler.output;
        in_sec2_hiso_resampler.chroma_radius = p_filters->input_param.chroma_filter.radius;
        out_sec2_main_resampler.p_cr_33 = p_cr->step8.sec2.p_CR_buf_33;
        out_sec2_main_resampler.cr_33_size = CR_SIZE_33;
        out_sec2_main_resampler.p_cr_35 = p_cr->step8.sec2.p_CR_buf_35;
        out_sec2_main_resampler.cr_35_size = CR_SIZE_35;
        out_sec2_main_resampler.p_cr_37 = p_cr->step8.sec2.p_CR_buf_37;
        out_sec2_main_resampler.cr_37_size = CR_SIZE_37;
        rval |= ikc_sec2_hiso_resampler(&in_sec2_hiso_resampler, &out_sec2_main_resampler);
    }
    {
        ikc_in_hiso_frame_size_t in_hiso_frame_size;
        ikc_out_asf_t out_shpa_frame_size;
        ikc_out_sec18_mctf_shpb_t out_frame_size_div16_m1;
        in_hiso_frame_size.step = 8;  //step8
        in_hiso_frame_size.chroma_filter         = &p_filters->input_param.chroma_filter;
        in_hiso_frame_size.hi_nonsmooth_detect   = &p_filters->hiso_input_param.hi_nonsmooth_detect;
        in_hiso_frame_size.hi_chroma_filter_high = &p_filters->hiso_input_param.hi_chroma_filter_high;
        in_hiso_frame_size.p_main                = &p_filters->input_param.window_size_info.main_win;
        out_shpa_frame_size.p_cr_30    = p_cr->step8.sec2.p_CR_buf_30;
        out_shpa_frame_size.cr_30_size = CR_SIZE_30;
        rval |= ikc_hiso_shpa_frame_size(&in_hiso_frame_size, &out_shpa_frame_size);
        out_frame_size_div16_m1.p_cr_112    = p_cr->step8.sec18.p_CR_buf_112;
        out_frame_size_div16_m1.p_cr_117    = p_cr->step8.sec18.p_CR_buf_117;
        out_frame_size_div16_m1.cr_112_size = CR_SIZE_112;
        out_frame_size_div16_m1.cr_117_size = CR_SIZE_117;
        rval |= ikc_hiso_sec18_frame_size_div16_m1(&in_hiso_frame_size, &out_frame_size_div16_m1);
    }
    {
        ikc_out_sec18_mctf_shpb_t out_shpb;
        ikc_in_shpb_pass_through_t  in_shpb_pass_through;
        in_shpb_pass_through.is_yuv_422 = 1u;
        in_shpb_pass_through.step       = 8u;
        out_shpb.p_cr_112 = p_cr->step8.sec18.p_CR_buf_112;
        out_shpb.cr_112_size = CR_SIZE_112;
        out_shpb.p_cr_117 = p_cr->step8.sec18.p_CR_buf_117;
        out_shpb.cr_117_size = CR_SIZE_117;
        rval |= ikc_hiso_shpb_pass_through(&in_shpb_pass_through, &out_shpb);
    }
    {
        ikc_out_sec18_mctf_shpb_t out_mctf;
        out_mctf.p_cr_112 = p_cr->step8.sec18.p_CR_buf_112;
        out_mctf.cr_112_size = CR_SIZE_112;
        out_mctf.p_cr_117 = p_cr->step8.sec18.p_CR_buf_117;
        out_mctf.cr_117_size = CR_SIZE_117;
        rval |= ikc_hiso_mctf_ta_constraints(&out_mctf);
    }
    return rval;
}

static uint32 exe_process_hiso_step9_cr(idsp_flow_ctrl_t *p_flow, const amba_ik_hiso_flow_tables_list_t *p_cr, amba_ik_filter_t *p_filters)
{
    uint32 rval = IK_OK;
    ikc_in_asf_a_t  in_asf;
    ikc_out_asf_t   out_asf;
    uint32 window_calculate_updated = 0U;

    rval |= exe_chroma_noise_def_value(&p_cr->step9.sec2, p_filters);

    if((p_filters->update_flags.iso.rgb_to_yuv_matrix_updated == 1U) ||
       (p_filters->update_flags.iso.is_1st_frame == 1U)) {
        rval |= exe_process_init_y2y_rgb2yuv(&p_cr->step9.sec2, p_filters);
    }

    // exe_process_calib_cr
    if(p_filters->update_flags.iso.is_1st_frame == 1U) { //should only configure once at 1st time.
        rval |= exe_process_input_mode_y2y_step3(p_flow, &p_cr->step9.sec2, p_filters, 9u);
    }

    if ((p_filters->update_flags.iso.window_size_info_updated == 1U) ||
        (p_filters->update_flags.iso.cfa_window_size_info_updated == 1U) ||
        (p_filters->update_flags.iso.dmy_range_updated == 1U) ||
        (p_filters->update_flags.iso.dzoom_info_updated == 1U) ||
        (p_filters->update_flags.iso.vin_active_win_updated == 1U) ||
        (p_filters->update_flags.iso.stitching_info_updated == 1U) ||
        (p_filters->update_flags.iso.overlap_x_updated == 1U) ||
        (p_filters->update_flags.iso.flip_mode_updated == 1U) ||
        (p_filters->update_flags.iso.warp_enable_updated == 1U) ||
        (p_filters->update_flags.iso.warp_enable_2nd_updated == 1U)) {
        window_calculate_updated = 1U;
    }

    if(p_filters->update_flags.cr.r2y.cr_30_update == 0U) {
        amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_hiso_asf_a !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
        rval |= IK_ERR_0000;
    } else {
        if(p_filters->hiso_input_param.hi_select.use_sharpen_not_asf == 1U) {
            ikc_in_step9_shpa_noise_t in_step9_shpa_noise;
            ikc_out_sharpen_t         out_sharpen;
            in_step9_shpa_noise.hi_high_shpns_both        = &p_filters->hiso_input_param.hi_high_shpns_both;
            in_step9_shpa_noise.hi_high_shpns_noise       = &p_filters->hiso_input_param.hi_high_shpns_noise;
            in_step9_shpa_noise.hi_high_shpns_coring      = &p_filters->hiso_input_param.hi_high_shpns_coring;
            in_step9_shpa_noise.hi_high_shpns_fir         = &p_filters->hiso_input_param.hi_high_shpns_fir;
            in_step9_shpa_noise.hi_high_shpns_cor_idx_scl = &p_filters->hiso_input_param.hi_high_shpns_cor_idx_scl;
            in_step9_shpa_noise.hi_high_shpns_min_cor_rst = &p_filters->hiso_input_param.hi_high_shpns_min_cor_rst;
            in_step9_shpa_noise.hi_high_shpns_max_cor_rst = &p_filters->hiso_input_param.hi_high_shpns_max_cor_rst;
            in_step9_shpa_noise.hi_high_shpns_scl_cor     = &p_filters->hiso_input_param.hi_high_shpns_scl_cor;
            out_sharpen.p_cr_30 = p_cr->step9.sec2.p_CR_buf_30;
            out_sharpen.cr_30_size = CR_SIZE_30;
            rval |= ikc_hiso_step9_shpa_nosie_filter(&in_step9_shpa_noise, &out_sharpen);
        } else {
            in_asf.p_advance_spatial_filter = &p_filters->hiso_input_param.hi_high2_asf;
            in_asf.first_luma_processing_mode_updated = (uint32)p_filters->update_flags.iso.first_luma_processing_mode_updated;
            out_asf.p_cr_30    = p_cr->step9.sec2.p_CR_buf_30;
            out_asf.cr_30_size = CR_SIZE_30;
            rval |= ikc_hiso_asf_a(&in_asf, &out_asf);
        }
    }
    if((p_filters->update_flags.iso.chroma_filter_updated == 1U)||
       (p_filters->update_flags.iso.resample_str_update == 1U)||
       (p_filters->update_flags.iso.window_size_info_updated == 1U)||
       (window_calculate_updated == 1U)) {
        ikc_in_sec2_hiso_resampler_t in_sec2_hiso_resampler;
        ikc_out_sec2_main_resampler_t out_sec2_main_resampler;
        ikc_in_hiso_resampler_size_t  in_hiso_resampler;
        ikc_out_hiso_resampler_size_t out_hiso_resampler;

        in_hiso_resampler.step   = 9;//step9
        in_hiso_resampler.p_main = &p_filters->input_param.window_size_info.main_win;
        in_hiso_resampler.chroma_filter         = &p_filters->input_param.chroma_filter;
        in_hiso_resampler.hi_nonsmooth_detect   = &p_filters->hiso_input_param.hi_nonsmooth_detect;
        in_hiso_resampler.hi_chroma_filter_high = &p_filters->hiso_input_param.hi_chroma_filter_high;

        rval |= ikc_hiso_resampler_size(&in_hiso_resampler, &out_hiso_resampler);

        in_sec2_hiso_resampler.step   = 9U;
        in_sec2_hiso_resampler.main_cut_off_freq = p_filters->input_param.resample_str.main_cut_off_freq;
        in_sec2_hiso_resampler.p_main = &out_hiso_resampler.output;
        in_sec2_hiso_resampler.chroma_radius = p_filters->input_param.chroma_filter.radius;
        out_sec2_main_resampler.p_cr_33 = p_cr->step9.sec2.p_CR_buf_33;
        out_sec2_main_resampler.cr_33_size = CR_SIZE_33;
        out_sec2_main_resampler.p_cr_35 = p_cr->step9.sec2.p_CR_buf_35;
        out_sec2_main_resampler.cr_35_size = CR_SIZE_35;
        out_sec2_main_resampler.p_cr_37 = p_cr->step9.sec2.p_CR_buf_37;
        out_sec2_main_resampler.cr_37_size = CR_SIZE_37;
        rval |= ikc_sec2_hiso_resampler(&in_sec2_hiso_resampler, &out_sec2_main_resampler);
    }
    {
        ikc_in_hiso_frame_size_t in_hiso_frame_size;
        ikc_out_asf_t out_shpa_frame_size;
        in_hiso_frame_size.step = 9;  //step9
        in_hiso_frame_size.chroma_filter         = &p_filters->input_param.chroma_filter;
        in_hiso_frame_size.hi_nonsmooth_detect   = &p_filters->hiso_input_param.hi_nonsmooth_detect;
        in_hiso_frame_size.hi_chroma_filter_high = &p_filters->hiso_input_param.hi_chroma_filter_high;
        in_hiso_frame_size.p_main                = &p_filters->input_param.window_size_info.main_win;
        out_shpa_frame_size.p_cr_30    = p_cr->step9.sec2.p_CR_buf_30;
        out_shpa_frame_size.cr_30_size = CR_SIZE_30;
        rval |= ikc_hiso_shpa_frame_size(&in_hiso_frame_size, &out_shpa_frame_size);
    }
    return rval;
}

static uint32 exe_process_hiso_step10_cr(idsp_flow_ctrl_t *p_flow, const amba_ik_hiso_flow_tables_list_t *p_cr, amba_ik_filter_t *p_filters)
{
    uint32 rval = IK_OK;
    uint32 window_calculate_updated = 0U;

    rval |= exe_chroma_noise_def_value(&p_cr->step10.sec2, p_filters);

    if((p_filters->update_flags.iso.rgb_to_yuv_matrix_updated == 1U) ||
       (p_filters->update_flags.iso.is_1st_frame == 1U)) {
        rval |= exe_process_init_y2y_rgb2yuv(&p_cr->step10.sec2, p_filters);
    }

    // exe_process_calib_cr
    if(p_filters->update_flags.iso.is_1st_frame == 1U) { //should only configure once at 1st time.
        rval |= exe_process_input_mode_y2y_step3(p_flow, &p_cr->step10.sec2, p_filters, 10u);
    }

    if(p_filters->update_flags.iso.hi_luma_combine_updated == 1U) {
        ikc_in_luma_combine_2_mctf_t in_hi_luma_combine;
        ikc_out_mctf_t out_hi_luma_combine;
        in_hi_luma_combine.step = 10U;
        in_hi_luma_combine.p_hi_luma_filter_combine = &p_filters->hiso_input_param.hi_luma_combine;
        out_hi_luma_combine.p_cr_112 = p_cr->step10.sec18.p_CR_buf_112;
        out_hi_luma_combine.cr_112_size = CR_SIZE_112;
        rval |= ikc_luma_combine_2_mctf(&in_hi_luma_combine, &out_hi_luma_combine);
    }

    if ((p_filters->update_flags.iso.window_size_info_updated == 1U) ||
        (p_filters->update_flags.iso.cfa_window_size_info_updated == 1U) ||
        (p_filters->update_flags.iso.dmy_range_updated == 1U) ||
        (p_filters->update_flags.iso.dzoom_info_updated == 1U) ||
        (p_filters->update_flags.iso.vin_active_win_updated == 1U) ||
        (p_filters->update_flags.iso.stitching_info_updated == 1U) ||
        (p_filters->update_flags.iso.overlap_x_updated == 1U) ||
        (p_filters->update_flags.iso.flip_mode_updated == 1U) ||
        (p_filters->update_flags.iso.warp_enable_updated == 1U) ||
        (p_filters->update_flags.iso.warp_enable_2nd_updated == 1U)) {
        window_calculate_updated = 1U;
    }

    if((p_filters->update_flags.iso.chroma_filter_updated == 1U)||
       (p_filters->update_flags.iso.resample_str_update == 1U)||
       (p_filters->update_flags.iso.window_size_info_updated == 1U)||
       (window_calculate_updated == 1U)) {
        ikc_in_sec2_hiso_resampler_t in_sec2_hiso_resampler;
        ikc_out_sec2_main_resampler_t out_sec2_main_resampler;
        ikc_in_hiso_resampler_size_t  in_hiso_resampler;
        ikc_out_hiso_resampler_size_t out_hiso_resampler;

        in_hiso_resampler.step   = 10;//step10
        in_hiso_resampler.p_main = &p_filters->input_param.window_size_info.main_win;
        in_hiso_resampler.chroma_filter         = &p_filters->input_param.chroma_filter;
        in_hiso_resampler.hi_nonsmooth_detect   = &p_filters->hiso_input_param.hi_nonsmooth_detect;
        in_hiso_resampler.hi_chroma_filter_high = &p_filters->hiso_input_param.hi_chroma_filter_high;

        rval |= ikc_hiso_resampler_size(&in_hiso_resampler, &out_hiso_resampler);

        in_sec2_hiso_resampler.step   = 10U;
        in_sec2_hiso_resampler.main_cut_off_freq = p_filters->input_param.resample_str.main_cut_off_freq;
        in_sec2_hiso_resampler.p_main = &out_hiso_resampler.output;
        in_sec2_hiso_resampler.chroma_radius = p_filters->input_param.chroma_filter.radius;
        out_sec2_main_resampler.p_cr_33 = p_cr->step10.sec2.p_CR_buf_33;
        out_sec2_main_resampler.cr_33_size = CR_SIZE_33;
        out_sec2_main_resampler.p_cr_35 = p_cr->step10.sec2.p_CR_buf_35;
        out_sec2_main_resampler.cr_35_size = CR_SIZE_35;
        out_sec2_main_resampler.p_cr_37 = p_cr->step10.sec2.p_CR_buf_37;
        out_sec2_main_resampler.cr_37_size = CR_SIZE_37;
        rval |= ikc_sec2_hiso_resampler(&in_sec2_hiso_resampler, &out_sec2_main_resampler);
    }

    {
        ikc_in_hiso_frame_size_t in_hiso_frame_size;
        ikc_out_asf_t out_shpa_frame_size;
        ikc_out_sec18_mctf_shpb_t out_frame_size_div16_m1;
        in_hiso_frame_size.step = 10;  //step10
        in_hiso_frame_size.chroma_filter         = &p_filters->input_param.chroma_filter;
        in_hiso_frame_size.hi_nonsmooth_detect   = &p_filters->hiso_input_param.hi_nonsmooth_detect;
        in_hiso_frame_size.hi_chroma_filter_high = &p_filters->hiso_input_param.hi_chroma_filter_high;
        in_hiso_frame_size.p_main                = &p_filters->input_param.window_size_info.main_win;
        out_shpa_frame_size.p_cr_30    = p_cr->step10.sec2.p_CR_buf_30;
        out_shpa_frame_size.cr_30_size = CR_SIZE_30;
        rval |= ikc_hiso_shpa_frame_size(&in_hiso_frame_size, &out_shpa_frame_size);
        out_frame_size_div16_m1.p_cr_112    = p_cr->step10.sec18.p_CR_buf_112;
        out_frame_size_div16_m1.p_cr_117    = p_cr->step10.sec18.p_CR_buf_117;
        out_frame_size_div16_m1.cr_112_size = CR_SIZE_112;
        out_frame_size_div16_m1.cr_117_size = CR_SIZE_117;
        rval |= ikc_hiso_sec18_frame_size_div16_m1(&in_hiso_frame_size, &out_frame_size_div16_m1);
    }
    {
        ikc_out_sec18_mctf_shpb_t out_shpb;
        ikc_in_shpb_pass_through_t  in_shpb_pass_through;
        in_shpb_pass_through.is_yuv_422 = 0u;
        in_shpb_pass_through.step       = 10u;
        out_shpb.p_cr_112 = p_cr->step10.sec18.p_CR_buf_112;
        out_shpb.cr_112_size = CR_SIZE_112;
        out_shpb.p_cr_117 = p_cr->step10.sec18.p_CR_buf_117;
        out_shpb.cr_117_size = CR_SIZE_117;
        rval |= ikc_hiso_shpb_pass_through(&in_shpb_pass_through, &out_shpb);
    }
    {
        ikc_out_sec18_mctf_shpb_t out_mctf;
        out_mctf.p_cr_112 = p_cr->step10.sec18.p_CR_buf_112;
        out_mctf.cr_112_size = CR_SIZE_112;
        out_mctf.p_cr_117 = p_cr->step10.sec18.p_CR_buf_117;
        out_mctf.cr_117_size = CR_SIZE_117;
        rval |= ikc_hiso_mctf_ta_constraints(&out_mctf);
    }
    return rval;
}

static uint32 exe_process_hiso_step11_cr(idsp_flow_ctrl_t *p_flow, const amba_ik_hiso_flow_tables_list_t *p_cr, amba_ik_filter_t *p_filters, uint32 num_of_exp)
{
    uint32 rval = IK_OK;
    uint32 window_calculate_updated = 0U;

    if (num_of_exp == 0U) {
        // fix compile error
    } else {
        // default
    }

    rval |= exe_chroma_noise_def_value_step1(&p_cr->step11.sec2, p_filters);

    if ((p_filters->update_flags.iso.window_size_info_updated == 1U) ||
        (p_filters->update_flags.iso.cfa_window_size_info_updated == 1U) ||
        (p_filters->update_flags.iso.dmy_range_updated == 1U) ||
        (p_filters->update_flags.iso.dzoom_info_updated == 1U) ||
        (p_filters->update_flags.iso.vin_active_win_updated == 1U) ||
        (p_filters->update_flags.iso.stitching_info_updated == 1U) ||
        (p_filters->update_flags.iso.overlap_x_updated == 1U) ||
        (p_filters->update_flags.iso.flip_mode_updated == 1U) ||
        (p_filters->update_flags.iso.warp_enable_updated == 1U) ||
        (p_filters->update_flags.iso.warp_enable_2nd_updated == 1U)) {
        window_calculate_updated = 1U;
    }

    // exe_process_calib_cr
    if(p_filters->update_flags.iso.is_1st_frame == 1U) { //should only configure once at 1st time.
        ikc_in_input_mode_t in_input_mode;
        ikc_out_input_mode_t out_input_mode;

        rval |= exe_check_vin_sensor(p_filters);
        if((p_filters->update_flags.cr.r2y.cr_4_update == 0U)||
           (p_filters->update_flags.cr.r2y.cr_6_update == 0U)||
           (p_filters->update_flags.cr.r2y.cr_7_update == 0U)||
           (p_filters->update_flags.cr.r2y.cr_8_update == 0U)||
           (p_filters->update_flags.cr.r2y.cr_9_update == 0U)||
           (p_filters->update_flags.cr.r2y.cr_11_update == 0U)||
           (p_filters->update_flags.cr.r2y.cr_12_update == 0U)||
           (p_filters->update_flags.cr.r2y.cr_13_update == 0U)||
           (p_filters->update_flags.cr.r2y.cr_16_update == 0U)||
           (p_filters->update_flags.cr.r2y.cr_21_update == 0U)||
           (p_filters->update_flags.cr.r2y.cr_22_update == 0U)||
           (p_filters->update_flags.cr.r2y.cr_23_update == 0U)||
           (p_filters->update_flags.cr.r2y.cr_24_update == 0U)||
           (p_filters->update_flags.cr.r2y.cr_26_update == 0U)||
           (p_filters->update_flags.cr.r2y.cr_29_update == 0U)||
           (p_filters->update_flags.cr.r2y.cr_30_update == 0U)||
           (p_filters->update_flags.cr.r2y.cr_31_update == 0U)||
           (p_filters->update_flags.cr.r2y.cr_32_update == 0U)||
           (p_filters->update_flags.cr.r2y.cr_117_update == 0U)) {
            amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_input_mode !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
            rval |= IK_ERR_0000;
        } else {
            in_input_mode.flip_mode = p_filters->input_param.flip_mode;
            in_input_mode.p_sensor_info = &p_filters->input_param.sensor_info;
            out_input_mode.p_cr_4 = p_cr->step11.sec2.p_CR_buf_4;
            out_input_mode.cr_4_size = CR_SIZE_4;
            out_input_mode.p_cr_6 = p_cr->step11.sec2.p_CR_buf_6;
            out_input_mode.cr_6_size = CR_SIZE_6;
            out_input_mode.p_cr_7 = p_cr->step11.sec2.p_CR_buf_7;
            out_input_mode.cr_7_size = CR_SIZE_7;
            out_input_mode.p_cr_8 = p_cr->step11.sec2.p_CR_buf_8;
            out_input_mode.cr_8_size = CR_SIZE_8;
            out_input_mode.p_cr_9 = p_cr->step11.sec2.p_CR_buf_9;
            out_input_mode.cr_9_size = CR_SIZE_9;
            out_input_mode.p_cr_11 = p_cr->step11.sec2.p_CR_buf_11;
            out_input_mode.cr_11_size = CR_SIZE_11;
            out_input_mode.p_cr_12 = p_cr->step11.sec2.p_CR_buf_12;
            out_input_mode.cr_12_size = CR_SIZE_12;
            out_input_mode.p_cr_13 = p_cr->step11.sec2.p_CR_buf_13;
            out_input_mode.cr_13_size = CR_SIZE_13;
            out_input_mode.p_cr_16 = p_cr->step11.sec2.p_CR_buf_16;
            out_input_mode.cr_16_size = CR_SIZE_16;
            out_input_mode.p_cr_21 = p_cr->step11.sec2.p_CR_buf_21;
            out_input_mode.cr_21_size = CR_SIZE_21;
            out_input_mode.p_cr_22 = p_cr->step11.sec2.p_CR_buf_22;
            out_input_mode.cr_22_size = CR_SIZE_22;
            out_input_mode.p_cr_23 = p_cr->step11.sec2.p_CR_buf_23;
            out_input_mode.cr_23_size = CR_SIZE_23;
            out_input_mode.p_cr_24 = p_cr->step11.sec2.p_CR_buf_24;
            out_input_mode.cr_24_size = CR_SIZE_24;
            out_input_mode.p_cr_26 = p_cr->step11.sec2.p_CR_buf_26;
            out_input_mode.cr_26_size = CR_SIZE_26;
            out_input_mode.p_cr_29 = p_cr->step11.sec2.p_CR_buf_29;
            out_input_mode.cr_29_size = CR_SIZE_29;
            out_input_mode.p_cr_30 = p_cr->step11.sec2.p_CR_buf_30;
            out_input_mode.cr_30_size = CR_SIZE_30;
            out_input_mode.p_cr_31 = p_cr->step11.sec2.p_CR_buf_31;
            out_input_mode.cr_31_size = CR_SIZE_31;
            out_input_mode.p_cr_32 = p_cr->step11.sec2.p_CR_buf_32;
            out_input_mode.cr_32_size = CR_SIZE_32;
            out_input_mode.p_cr_117 = p_cr->step11.sec18.p_CR_buf_117;
            out_input_mode.cr_117_size = CR_SIZE_117;
            out_input_mode.p_flow = &p_flow->flow_info;
            out_input_mode.p_calib = &p_flow->calib;

            if (p_filters->input_param.ability == AMBA_IK_VIDEO_PIPE_Y2Y) {
                in_input_mode.is_yuv_mode = 1U;
            } else {
                in_input_mode.is_yuv_mode = 0U;
            }
            rval |= ikc_input_mode(&in_input_mode, &out_input_mode);
        }
    }
    if (window_calculate_updated == 1U) {
        rval |= exe_win_calc_wrapper(p_filters);
    }
    if((p_filters->update_flags.iso.static_bpc_updated == 1u) || (p_filters->update_flags.iso.static_bpc_internal_updated == 1u) ||
       (p_filters->update_flags.iso.dynamic_bpc_updated == 1u) || (p_filters->update_flags.iso.window_size_info_updated == 1U)) {
        if(p_filters->input_param.ctx_buf.sbp_internal_mode_flag == 1u) {
            ikc_in_static_bad_pixel_internal_t in_sbp_internal;
            ikc_out_static_bad_pixel_internal_t out_sbp;
            ik_static_bad_pixel_correction_internal_t sbp_internal_tmp;
            const void *void_ptr;

            if(p_filters->update_flags.cr.r2y.cr_12_update == 0U) {
                amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_static_bad_pixel_internal !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
                rval |= IK_ERR_0000;
            } else {
                //consider 64 bit compiler, flow data sbp addr(u32) goes on physical addr + offset.
                (void)amba_ik_system_memcpy(&sbp_internal_tmp,&p_filters->input_param.sbp_internal,sizeof(ik_static_bad_pixel_correction_internal_t));
                void_ptr = amba_ik_system_virt_to_phys(sbp_internal_tmp.p_map);
                (void)amba_ik_system_memcpy(&sbp_internal_tmp.p_map, &void_ptr, sizeof(void*));
                in_sbp_internal.dbp_enable = p_filters->input_param.dynamic_bpc.enable;
                in_sbp_internal.p_sbp_internal = &sbp_internal_tmp;
                out_sbp.p_cr_12 = p_cr->step11.sec2.p_CR_buf_12;
                out_sbp.cr_12_size = CR_SIZE_12;
                out_sbp.p_flow = &p_flow->flow_info;
                rval |= ikc_static_bad_pixel_internal(&in_sbp_internal, &out_sbp);
            }
        } else {
            ikc_in_static_bad_pixel_t in_sbp;
            ikc_out_static_bad_pixel_t out_sbp;
            ik_static_bad_pxl_cor_t static_bpc_tmp;
            const void *void_ptr;

            if(p_filters->update_flags.cr.r2y.cr_12_update == 0U) {
                amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_static_bad_pixel !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
                rval |= IK_ERR_0000;
            } else {
                //consider 64 bit compiler, flow data sbp addr(u32) goes on physical addr + offset.
                (void)amba_ik_system_memcpy(&static_bpc_tmp,&p_filters->input_param.static_bpc,sizeof(ik_static_bad_pxl_cor_t));
                void_ptr = amba_ik_system_virt_to_phys(static_bpc_tmp.calib_sbp_info.sbp_buffer);
                (void)amba_ik_system_memcpy(&static_bpc_tmp.calib_sbp_info.sbp_buffer, &void_ptr, sizeof(void*));
                in_sbp.dbp_enable = p_filters->input_param.dynamic_bpc.enable;
                in_sbp.sbp_enable = p_filters->input_param.sbp_enable;
                in_sbp.p_static_bpc = &static_bpc_tmp;
                in_sbp.p_vin_sensor = &p_filters->input_param.window_size_info.vin_sensor;
                out_sbp.p_cr_12 = p_cr->step11.sec2.p_CR_buf_12;
                out_sbp.cr_12_size = CR_SIZE_12;
                out_sbp.p_flow = &p_flow->flow_info;
                rval |= ikc_static_bad_pixel(&in_sbp, &out_sbp);
            }
        }
        p_flow->flow_info.update_sbp = 1U;
    } else {
        p_flow->flow_info.update_sbp = 0U;
    }
    if ((p_filters->update_flags.iso.vignette_compensation_updated == 1U)||
        (p_filters->update_flags.iso.window_size_info_updated == 1U) || (window_calculate_updated == 1U)) {
        ikc_in_vignette_t in_vig;
        ikc_out_vignette_t out_vig;

        if((p_filters->update_flags.cr.r2y.cr_9_update == 0U)||
           (p_filters->update_flags.cr.r2y.cr_10_update == 0U)||
           (p_filters->update_flags.cr.r2y.cr_11_update == 0U)) {
            amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_vignette !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
            rval |= IK_ERR_0000;
        } else {
            in_vig.vig_enable = p_filters->input_param.vig_enable;
            in_vig.flip_mode = p_filters->input_param.flip_mode;
            in_vig.p_vignette_compensation = &p_filters->input_param.vignette_compensation;
            in_vig.p_vin_sensor = &p_filters->input_param.window_size_info.vin_sensor;
            in_vig.p_dmy_win_geo = &p_filters->input_param.ctx_buf.result_win.dmy_win_geo;
            in_vig.p_active_window = &p_filters->input_param.active_window;
            out_vig.p_cr_9 = p_cr->step11.sec2.p_CR_buf_9;
            out_vig.cr_9_size = CR_SIZE_9;
            out_vig.p_cr_10 = p_cr->step11.sec2.p_CR_buf_10;
            out_vig.cr_10_size = CR_SIZE_10;
            out_vig.p_cr_11 = p_cr->step11.sec2.p_CR_buf_11;
            out_vig.cr_11_size = CR_SIZE_11;
            out_vig.p_flow = &p_flow->flow_info;
            rval |= ikc_vignette(&in_vig, &out_vig);
        }
        p_flow->flow_info.update_vignette = 1U;
    } else {
        p_flow->flow_info.update_vignette = 0U;
    }
    if ((p_filters->update_flags.iso.calib_ca_warp_info_updated == 1U) || (p_filters->update_flags.iso.cawarp_enable_updated == 1U) ||
        (p_filters->update_flags.iso.cawarp_internal_updated == 1U) || (p_filters->update_flags.iso.window_size_info_updated == 1U) ||
        (window_calculate_updated == 1U)) {
        if (p_filters->input_param.ctx_buf.cawarp_internal_mode_flag == 1U) {
            ikc_in_cawarp_internal_t in_ca;
            ikc_out_cawarp_t out_ca;

            if((p_filters->update_flags.cr.cawarp == 0U)||
               (p_filters->update_flags.cr.r2y.cr_16_update == 0U)||
               (p_filters->update_flags.cr.r2y.cr_17_update == 0U)) {
                amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_cawarp_internal !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
                rval |= IK_ERR_0000;
            } else {
                in_ca.p_stitching_info = &p_filters->input_param.stitching_info;
                in_ca.p_ca_warp_internal = &p_filters->input_param.ca_warp_internal;
                out_ca.p_cr_16 = p_cr->step11.sec2.p_CR_buf_16;
                out_ca.cr_16_size = CR_SIZE_16;
                out_ca.p_cr_17 = p_cr->step11.sec2.p_CR_buf_17;
                out_ca.cr_17_size = CR_SIZE_17;
                out_ca.p_cr_18 = p_cr->step11.sec2.p_CR_buf_18;
                out_ca.cr_18_size = CR_SIZE_18;
                out_ca.p_cr_19 = p_cr->step11.sec2.p_CR_buf_19;
                out_ca.cr_19_size = CR_SIZE_19;
                out_ca.p_cr_20 = p_cr->step11.sec2.p_CR_buf_20;
                out_ca.cr_20_size = CR_SIZE_20;
                out_ca.p_calib = &p_flow->calib;
                out_ca.p_cawarp_hor_table_addr_red = p_cr->p_ca_warp_hor_red;
                out_ca.h_red_size = CAWARP_VIDEO_MAXSIZE;
                out_ca.p_cawarp_vertical_table_addr_red = p_cr->p_ca_warp_ver_red;
                out_ca.v_red_size = CAWARP_VIDEO_MAXSIZE;
                out_ca.p_cawarp_hor_table_addr_blue = p_cr->p_ca_warp_hor_blue;
                out_ca.h_blue_size = CAWARP_VIDEO_MAXSIZE;
                out_ca.p_cawarp_vertical_table_addr_blue = p_cr->p_ca_warp_ver_blue;
                out_ca.v_blue_size = CAWARP_VIDEO_MAXSIZE;
                rval |= ikc_cawarp_internal(&in_ca, &out_ca);

                p_flow->calib.cawarp_horizontal_table_addr_red = physical2u32(p_cr->p_ca_warp_hor_red);
                p_flow->calib.cawarp_horizontal_table_addr_blue = physical2u32(p_cr->p_ca_warp_hor_blue);
                p_flow->calib.cawarp_vertical_table_addr_red = physical2u32(p_cr->p_ca_warp_ver_red);
                p_flow->calib.cawarp_vertical_table_addr_blue = physical2u32(p_cr->p_ca_warp_ver_blue);
            }
        } else {
            ikc_in_cawarp_t in_ca;
            ikc_out_cawarp_t out_ca;

            if((p_filters->update_flags.cr.cawarp == 0U)||
               (p_filters->update_flags.cr.r2y.cr_16_update == 0U)||
               (p_filters->update_flags.cr.r2y.cr_17_update == 0U)) {
                amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_cawarp !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
                rval |= IK_ERR_0000;
            } else {
                in_ca.is_group_cmd = 0u;
                in_ca.is_hiso      = 1u;
                in_ca.cawarp_enable = (uint32)p_filters->input_param.cawarp_enable;
                in_ca.flip_mode = p_filters->input_param.flip_mode;
                in_ca.p_stitching_info = &p_filters->input_param.stitching_info;
                in_ca.p_window_size_info = &p_filters->input_param.window_size_info;
                in_ca.p_result_win = &p_filters->input_param.ctx_buf.result_win;
                in_ca.p_calib_ca_warp_info = &p_filters->input_param.calib_ca_warp_info;
                out_ca.p_cr_16 = p_cr->step11.sec2.p_CR_buf_16;
                out_ca.cr_16_size = CR_SIZE_16;
                out_ca.p_cr_17 = p_cr->step11.sec2.p_CR_buf_17;
                out_ca.cr_17_size = CR_SIZE_17;
                out_ca.p_cr_18 = p_cr->step11.sec2.p_CR_buf_18;
                out_ca.cr_18_size = CR_SIZE_18;
                out_ca.p_cr_19 = p_cr->step11.sec2.p_CR_buf_19;
                out_ca.cr_19_size = CR_SIZE_19;
                out_ca.p_cr_20 = p_cr->step11.sec2.p_CR_buf_20;
                out_ca.cr_20_size = CR_SIZE_20;
                out_ca.p_calib = &p_flow->calib;
                out_ca.p_cawarp_hor_table_addr_red = p_cr->p_ca_warp_hor_red;
                out_ca.h_red_size = CAWARP_VIDEO_MAXSIZE;
                out_ca.p_cawarp_vertical_table_addr_red = p_cr->p_ca_warp_ver_red;
                out_ca.v_red_size = CAWARP_VIDEO_MAXSIZE;
                out_ca.p_cawarp_hor_table_addr_blue = p_cr->p_ca_warp_hor_blue;
                out_ca.h_blue_size = CAWARP_VIDEO_MAXSIZE;
                out_ca.p_cawarp_vertical_table_addr_blue = p_cr->p_ca_warp_ver_blue;
                out_ca.v_blue_size = CAWARP_VIDEO_MAXSIZE;

                rval |= ikc_cawarp(&in_ca, &out_ca);

                p_flow->calib.cawarp_horizontal_table_addr_red = physical2u32(p_cr->p_ca_warp_hor_red);
                p_flow->calib.cawarp_horizontal_table_addr_blue = physical2u32(p_cr->p_ca_warp_hor_blue);
                p_flow->calib.cawarp_vertical_table_addr_red = physical2u32(p_cr->p_ca_warp_ver_red);
                p_flow->calib.cawarp_vertical_table_addr_blue = physical2u32(p_cr->p_ca_warp_ver_blue);
            }
        }
        p_flow->calib.update_CA_warp = 1U;
    } else {
        p_flow->calib.update_CA_warp = 0U;
    }
    if((p_filters->update_flags.iso.chroma_filter_updated == 1U)||
       (p_filters->update_flags.iso.warp_enable_updated == 1U)||
       (p_filters->update_flags.iso.calib_warp_info_updated == 1U)||
       (p_filters->update_flags.iso.warp_internal_updated == 1U)||
       (p_filters->update_flags.iso.window_size_info_updated == 1U)||
       (window_calculate_updated == 1U)) {
        if(p_filters->input_param.ctx_buf.warp_internal_mode_flag == 1U) { //internal warp
#if 0
            ikc_in_warp_internal_t in_warp_internal;
            ikc_out_hwarp_t out_hwarp;

            if((p_filters->update_flags.cr.warp == 0U)||
               (p_filters->update_flags.cr.r2y.cr_33_update == 0U)||
               (p_filters->update_flags.cr.r2y.cr_34_update == 0U)||
               (p_filters->update_flags.cr.r2y.cr_35_update == 0U)||
               (p_filters->update_flags.cr.r2y.cr_42_update == 0U)||
               (p_filters->update_flags.cr.r2y.cr_43_update == 0U)||
               (p_filters->update_flags.cr.r2y.cr_44_update == 0U)) {
                amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_warp_internal !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
                rval |= IK_ERR_0000;
            } else {
                in_warp_internal.flip_mode = p_filters->input_param.flip_mode;
                in_warp_internal.p_window_size_info = &p_filters->input_param.window_size_info;
                in_warp_internal.p_act_win_crop = &p_filters->input_param.ctx_buf.result_win.act_win_crop;
                in_warp_internal.p_dmy_win_geo = &p_filters->input_param.ctx_buf.result_win.dmy_win_geo;
                in_warp_internal.p_cfa_win_dim = &p_filters->input_param.ctx_buf.result_win.cfa_win_dim;
                in_warp_internal.p_dmy_range = &p_filters->input_param.dmy_range;
                in_warp_internal.chroma_radius = p_filters->input_param.chroma_filter.radius;
                in_warp_internal.p_stitching_info = &p_filters->input_param.stitching_info;
                in_warp_internal.p_warp_internal = &p_filters->input_param.warp_internal;
                in_warp_internal.dram_efficiency = p_filters->input_param.warp_buf_info.dram_efficiency;
                in_warp_internal.luma_wait_lines = p_filters->input_param.warp_buf_info.luma_wait_lines;
                in_warp_internal.luma_dma_size = p_filters->input_param.warp_buf_info.luma_dma_size;
                out_hwarp.p_cr_33 = p_flow_tbl_list->r2y.p_CR_buf_33;
                out_hwarp.cr_33_size = CR_SIZE_33;
                out_hwarp.p_cr_34 = p_flow_tbl_list->r2y.p_CR_buf_34;
                out_hwarp.cr_34_size = CR_SIZE_34;
                out_hwarp.p_cr_35 = p_flow_tbl_list->r2y.p_CR_buf_35;
                out_hwarp.cr_35_size = CR_SIZE_35;
                out_hwarp.p_cr_42 = p_flow_tbl_list->r2y.p_CR_buf_42;
                out_hwarp.cr_42_size = CR_SIZE_42;
                out_hwarp.p_cr_43 = p_flow_tbl_list->r2y.p_CR_buf_43;
                out_hwarp.cr_43_size = CR_SIZE_43;
                out_hwarp.p_cr_44 = p_flow_tbl_list->r2y.p_CR_buf_44;
                out_hwarp.cr_44_size = CR_SIZE_44;
                out_hwarp.p_flow_info = &p_flow->flow_info;
                out_hwarp.p_window = &p_flow->window;
                out_hwarp.p_phase = &p_flow->phase;
                out_hwarp.p_calib = &p_flow->calib;
                out_hwarp.p_stitch = &p_flow->stitch;
                out_hwarp.p_warp_horizontal_table_address = p_flow_tbl_list->r2y.p_warp_hor;
                out_hwarp.h_size = WARP_VIDEO_MAXHORGRID * WARP_VIDEO_MAXVERGRID * 2U * 2U;// multi_slice buffer.
                out_hwarp.p_warp_vertical_table_address = p_flow_tbl_list->r2y.p_warp_ver;
                out_hwarp.v_size = WARP_VIDEO_MAXHORGRID * WARP_VIDEO_MAXVERGRID * 2U * 2U;// multi_slice buffer.
                rval |= ikc_warp_internal(&in_warp_internal, &out_hwarp);
            }
#endif
        } else {
            ikc_in_hwarp_t in_hwarp;
            ikc_out_hwarp_t out_hwarp;
            //ikc_in_vwarp_t in_vwarp;
            //ikc_out_vwarp_t out_vwarp;

            if((p_filters->update_flags.cr.warp == 0U)||
               (p_filters->update_flags.cr.r2y.cr_33_update == 0U)||
               (p_filters->update_flags.cr.r2y.cr_34_update == 0U)||
               (p_filters->update_flags.cr.r2y.cr_35_update == 0U)||
               (p_filters->update_flags.cr.r2y.cr_42_update == 0U)||
               (p_filters->update_flags.cr.r2y.cr_43_update == 0U)||
               (p_filters->update_flags.cr.r2y.cr_44_update == 0U)) {
                amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_hwarp !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
                rval |= IK_ERR_0000;
            } else {
                {
                    in_hwarp.is_group_cmd = 0u;
                    in_hwarp.is_hiso      = 1u;
                    in_hwarp.warp_enable = (uint32)p_filters->input_param.warp_enable;
                    in_hwarp.flip_mode = p_filters->input_param.flip_mode;
                    in_hwarp.p_window_size_info = &p_filters->input_param.window_size_info;
                    in_hwarp.p_act_win_crop = &p_filters->input_param.ctx_buf.result_win.act_win_crop;
                    in_hwarp.p_dmy_win_geo = &p_filters->input_param.ctx_buf.result_win.dmy_win_geo;
                    in_hwarp.p_cfa_win_dim = &p_filters->input_param.ctx_buf.result_win.cfa_win_dim;
                    in_hwarp.p_dmy_range = &p_filters->input_param.dmy_range;
                    in_hwarp.chroma_radius = p_filters->input_param.chroma_filter.radius;
                    in_hwarp.is_y2y_420input = (p_filters->input_param.yuv_mode == AMBA_IK_YUV420) ? 1U : 0U;
                    in_hwarp.p_stitching_info = &p_filters->input_param.stitching_info;
                    in_hwarp.p_calib_warp_info = &p_filters->input_param.calib_warp_info;
                    in_hwarp.dram_efficiency = p_filters->input_param.warp_buf_info.dram_efficiency;
                    in_hwarp.luma_wait_lines = p_filters->input_param.warp_buf_info.luma_wait_lines;
                    in_hwarp.luma_dma_size = p_filters->input_param.warp_buf_info.luma_dma_size;
                    in_hwarp.is_still_422 = 0;
                    out_hwarp.p_cr_33 = p_cr->step11.sec2.p_CR_buf_33;
                    out_hwarp.cr_33_size = CR_SIZE_33;
                    out_hwarp.p_cr_34 = p_cr->step11.sec2.p_CR_buf_34;
                    out_hwarp.cr_34_size = CR_SIZE_34;
                    out_hwarp.p_cr_35 = p_cr->step11.sec2.p_CR_buf_35;
                    out_hwarp.cr_35_size = CR_SIZE_35;
                    out_hwarp.p_flow_info = &p_flow->flow_info;
                    out_hwarp.p_window = &p_flow->window;
                    out_hwarp.p_phase = &p_flow->phase;
                    out_hwarp.p_calib = &p_flow->calib;
                    out_hwarp.p_stitch = &p_flow->stitch;
                    out_hwarp.p_warp_horizontal_table_address = p_cr->p_warp_hor;
                    out_hwarp.h_size = WARP_VIDEO_MAXHORGRID * WARP_VIDEO_MAXVERGRID * 2U * 2U;// multi_slice buffer.

                    rval |= ikc_hwarp(&in_hwarp, &out_hwarp);
                }
            }
        }
        p_flow->calib.update_lens_warp = 1U;
    } else {
        p_flow->calib.update_lens_warp = 0U;
    }

    // exe_process_hdr_ce_cr
    if(p_filters->update_flags.iso.before_ce_wb_gain_updated == 1u) {
        ikc_in_before_ce_gain_t in_before_ce_gain={0};
        ikc_out_before_ce_gain_t out_before_ce_gain;

        if(p_filters->update_flags.cr.r2y.cr_13_update == 0U) {
            amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_before_ce_gain !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
            rval |= IK_ERR_0000;
        } else {
            in_before_ce_gain.p_before_ce_wb_gain = &p_filters->input_param.before_ce_wb_gain;
            out_before_ce_gain.p_cr_13 = p_cr->step11.sec2.p_CR_buf_13;
            out_before_ce_gain.cr_13_size = CR_SIZE_13;
            rval |= ikc_before_ce_gain(&in_before_ce_gain, &out_before_ce_gain);
        }
    }
    if(p_filters->update_flags.iso.hdr_blend_updated == 1u) {
        ikc_in_hdr_blend_t in_hdr_blend;
        ikc_out_hdr_blend_t out_hdr_blend;

        if(p_filters->update_flags.cr.r2y.cr_4_update == 0U) {
            amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_hdr_blend !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
            rval |= IK_ERR_0000;
        } else {
            in_hdr_blend.exp_num = p_filters->input_param.num_of_exposures;
            in_hdr_blend.p_hdr_raw_info = &p_filters->input_param.hdr_raw_info;
            in_hdr_blend.p_hdr_blend = &p_filters->input_param.hdr_blend;
            out_hdr_blend.p_cr_4 = p_cr->step11.sec2.p_CR_buf_4;
            out_hdr_blend.cr_4_size = CR_SIZE_4;
            out_hdr_blend.p_flow = &p_flow->flow_info;
            rval |= ikc_hdr_blend(&in_hdr_blend, &out_hdr_blend);
        }
    }
    if(p_filters->update_flags.iso.fe_tone_curve_updated == 1u) {
        ikc_in_hdr_tone_curve_t in_hdr_tc;
        ikc_out_hdr_tone_curve_t out_hdr_tc;

        if((p_filters->update_flags.cr.r2y.cr_4_update == 0U)||
           (p_filters->update_flags.cr.r2y.cr_5_update == 0U)) {
            amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_front_end_tone_curve !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
            rval |= IK_ERR_0000;
        } else {
            in_hdr_tc.p_fe_tone_curve = &p_filters->input_param.fe_tone_curve;
            out_hdr_tc.p_cr_4 = p_cr->step11.sec2.p_CR_buf_4;
            out_hdr_tc.cr_4_size = CR_SIZE_4;
            out_hdr_tc.p_cr_5 = p_cr->step11.sec2.p_CR_buf_5;
            out_hdr_tc.cr_5_size = CR_SIZE_5;
            rval |= ikc_front_end_tone_curve(&in_hdr_tc, &out_hdr_tc);
        }
    }
    if((p_filters->update_flags.iso.sensor_information_updated == 1u)||(window_calculate_updated == 1U)) {
        ikc_in_decompress_t in_decompress;
        ikc_out_decompress_t out_decompress;

        if(p_filters->update_flags.cr.r2y.cr_4_update == 0U) {
            amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_decompression !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
            rval |= IK_ERR_0000;
        } else {
            in_decompress.exp_num = 1U;
            in_decompress.compress_mode = p_filters->input_param.sensor_info.compression;////0~8, 256~259.
            in_decompress.p_dmy_win_geo = &p_filters->input_param.ctx_buf.result_win.dmy_win_geo;
            out_decompress.p_cr_4 = p_cr->step11.sec2.p_CR_buf_4;
            out_decompress.cr_4_size = CR_SIZE_4;
            rval |= ikc_decompression(&in_decompress, &out_decompress);
            if(p_filters->update_flags.iso.is_1st_frame == 1u) {
                p_filters->input_param.ctx_buf.first_compression_offset = p_filters->input_param.sensor_info.compression_offset;
            }
        }
    }
    if((p_filters->update_flags.iso.exp0_fe_static_blc_updated == 1u) ||
       (p_filters->update_flags.iso.exp1_fe_static_blc_updated == 1u) ||
       (p_filters->update_flags.iso.exp2_fe_static_blc_updated == 1u)) {
        ikc_in_hdr_blc_t in_hdr_blc;
        ikc_out_hdr_blc_t out_hdr_blc;

        if((p_filters->update_flags.cr.r2y.cr_4_update == 0U)||
           (p_filters->update_flags.cr.r2y.cr_6_update == 0U)||
           (p_filters->update_flags.cr.r2y.cr_7_update == 0U)||
           (p_filters->update_flags.cr.r2y.cr_8_update == 0U)) {
            amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_hdr_black_level !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
            rval |= IK_ERR_0000;
        } else {
            in_hdr_blc.exp_num = p_filters->input_param.num_of_exposures;
            in_hdr_blc.rgb_ir_mode = p_filters->input_param.rgb_ir.mode;
            in_hdr_blc.compression_offset = ((p_filters->input_param.sensor_info.compression > IK_RAW_COMPRESS_OFF)&&(p_filters->input_param.sensor_info.compression <= IK_RAW_COMPRESS_10p5)) ? p_filters->input_param.ctx_buf.first_compression_offset : 0;
            in_hdr_blc.p_exp0_frontend_static_blc = &p_filters->input_param.exp0_frontend_static_blc;
            in_hdr_blc.p_exp1_frontend_static_blc = &p_filters->input_param.exp1_frontend_static_blc;
            in_hdr_blc.p_exp2_frontend_static_blc = &p_filters->input_param.exp2_frontend_static_blc;
            out_hdr_blc.p_cr_4 = p_cr->step11.sec2.p_CR_buf_4;
            out_hdr_blc.cr_4_size = CR_SIZE_4;
            out_hdr_blc.p_cr_6 = p_cr->step11.sec2.p_CR_buf_6;
            out_hdr_blc.cr_6_size = CR_SIZE_6;
            out_hdr_blc.p_cr_7 = p_cr->step11.sec2.p_CR_buf_7;
            out_hdr_blc.cr_7_size = CR_SIZE_7;
            out_hdr_blc.p_cr_8 = p_cr->step11.sec2.p_CR_buf_8;
            out_hdr_blc.cr_8_size = CR_SIZE_8;
            rval |= ikc_hdr_black_level(&in_hdr_blc, &out_hdr_blc);
        }
    }
    if((p_filters->update_flags.iso.exp0_fe_wb_gain_updated == 1u) ||
       (p_filters->update_flags.iso.exp1_fe_wb_gain_updated == 1u) ||
       (p_filters->update_flags.iso.exp2_fe_wb_gain_updated == 1u)) {
        ikc_in_hdr_dgain_t in_hdr_dgain = {0};
        ikc_out_hdr_dgain_t out_hdr_dgain;

        if(p_filters->update_flags.cr.r2y.cr_4_update == 0U) {
            amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_hdr_dgain !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
            rval |= IK_ERR_0000;
        } else {
            in_hdr_dgain.exp_num = p_filters->input_param.num_of_exposures;
            in_hdr_dgain.rgb_ir_mode = p_filters->input_param.rgb_ir.mode;
            in_hdr_dgain.p_exp0_frontend_wb_gain = &p_filters->input_param.exp0_frontend_wb_gain;
            in_hdr_dgain.p_exp1_frontend_wb_gain = &p_filters->input_param.exp1_frontend_wb_gain;
            in_hdr_dgain.p_exp2_frontend_wb_gain = &p_filters->input_param.exp2_frontend_wb_gain;
            in_hdr_dgain.p_exp0_frontend_static_blc = &p_filters->input_param.exp0_frontend_static_blc;
            in_hdr_dgain.p_exp1_frontend_static_blc = &p_filters->input_param.exp1_frontend_static_blc;
            in_hdr_dgain.p_exp2_frontend_static_blc = &p_filters->input_param.exp2_frontend_static_blc;
            out_hdr_dgain.p_cr_4 = p_cr->step11.sec2.p_CR_buf_4;
            out_hdr_dgain.cr_4_size = CR_SIZE_4;
            rval |= ikc_hdr_dgain(&in_hdr_dgain, &out_hdr_dgain);
        }
    }
    if((p_filters->update_flags.iso.ce_updated == 1u)||(window_calculate_updated == 1U)) {
        ikc_in_ce_t in_ce;
        ikc_out_ce_t out_ce;

        if(p_filters->update_flags.cr.r2y.cr_13_update == 0U) {
            amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_contrast_enhancement !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
            rval |= IK_ERR_0000;
        } else {
            in_ce.exp_num = p_filters->input_param.num_of_exposures;
            in_ce.sensor_pattern = p_filters->input_param.sensor_info.sensor_pattern;
            in_ce.p_dmy_win_geo = &p_filters->input_param.ctx_buf.result_win.dmy_win_geo;
            in_ce.p_ce = &p_filters->input_param.ce;
            out_ce.p_cr_13 = p_cr->step11.sec2.p_CR_buf_13;
            out_ce.cr_13_size = CR_SIZE_13;
            out_ce.p_flow = &p_flow->flow_info;
            rval |= ikc_contrast_enhancement(&in_ce, &out_ce);
        }
    }
    if(p_filters->update_flags.iso.ce_input_table_updated == 1u) {
        ikc_in_ce_input_t in_ce_input;
        ikc_out_ce_input_t out_ce_input;

        if((p_filters->update_flags.cr.r2y.cr_13_update == 0U)||
           (p_filters->update_flags.cr.r2y.cr_14_update == 0U)) {
            amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_contrast_enhancement_input !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
            rval |= IK_ERR_0000;
        } else {
            in_ce_input.ce_enable = p_filters->input_param.ce.enable;
            in_ce_input.p_ce_input_table = &p_filters->input_param.ce_input_table;
            out_ce_input.p_cr_13 = p_cr->step11.sec2.p_CR_buf_13;
            out_ce_input.cr_13_size = CR_SIZE_13;
            out_ce_input.p_cr_14 = p_cr->step11.sec2.p_CR_buf_14;
            out_ce_input.cr_14_size = CR_SIZE_14;
            rval |= ikc_contrast_enhancement_input(&in_ce_input, &out_ce_input);
        }
    }
    if(p_filters->update_flags.iso.ce_out_table_updated == 1u) {
        ikc_in_ce_output_t in_ce_output;
        ikc_out_ce_t out_ce;

        if(p_filters->update_flags.cr.r2y.cr_13_update == 0U) {
            amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_contrast_enhancement_output !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
            rval |= IK_ERR_0000;
        } else {
            in_ce_output.ce_enable = p_filters->input_param.ce.enable;
            in_ce_output.p_ce_out_table = &p_filters->input_param.ce_out_table;
            out_ce.p_cr_13 = p_cr->step11.sec2.p_CR_buf_13;
            out_ce.cr_13_size = CR_SIZE_13;
            rval |= ikc_contrast_enhancement_output(&in_ce_output, &out_ce);
        }
    }

    // exe_process_cfa_cr
    if(p_filters->update_flags.iso.dynamic_bpc_updated == 1u) {
        ikc_in_dynamic_bad_pixel_t in_dbp;
        ikc_out_dynamic_bad_pixel_t out_dbp;

        if((p_filters->update_flags.cr.r2y.cr_11_update == 0U)||
           (p_filters->update_flags.cr.r2y.cr_12_update == 0U)) {
            amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_dynamic_bad_pixel !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
            rval |= IK_ERR_0000;
        } else {
            if(p_filters->input_param.ctx_buf.sbp_internal_mode_flag == 1u) {
                in_dbp.sbp_enable = p_filters->input_param.sbp_internal.enable;
            } else {
                in_dbp.sbp_enable = p_filters->input_param.sbp_enable;
            }
            if((p_filters->input_param.rgb_ir.ir_only==1U)&&(p_filters->input_param.sensor_info.sensor_mode==1U)) { //ir_only, runing on RGB bayer pattern, carry IR on B channel.
                in_dbp.rgb_ir_mode = 0u;
            } else {
                in_dbp.rgb_ir_mode = p_filters->input_param.rgb_ir.mode;
            }
            in_dbp.p_dbp = &p_filters->input_param.dynamic_bpc;
            out_dbp.p_cr_11 = p_cr->step11.sec2.p_CR_buf_11;
            out_dbp.cr_11_size = CR_SIZE_11;
            out_dbp.p_cr_12 = p_cr->step11.sec2.p_CR_buf_12;
            out_dbp.cr_12_size = CR_SIZE_12;
            rval |= ikc_dynamic_bad_pixel(&in_dbp, &out_dbp);
        }
    }
    if(p_filters->update_flags.iso.stored_ir_updated == 1u) {
        ikc_in_stored_ir_t in_stored_ir;
        ikc_out_stored_ir_t out_stored_ir;

        if(p_filters->update_flags.cr.r2y.cr_12_update == 0U) {
            amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_stored_ir !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
            rval |= IK_ERR_0000;
        } else {
            in_stored_ir.p_stored_ir = &p_filters->input_param.stored_ir;
            out_stored_ir.p_cr_12 = p_cr->step11.sec2.p_CR_buf_12;
            out_stored_ir.cr_12_size = CR_SIZE_12;
            rval |= ikc_stored_ir(&in_stored_ir, &out_stored_ir);
        }
    }
    if(p_filters->update_flags.iso.grgb_mismatch_updated == 1u) {
        ikc_in_grgb_mismatch_t in_grgb;
        ikc_out_grgb_mismatch_t out_grgb;

        if(p_filters->update_flags.cr.r2y.cr_23_update == 0U) {
            amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_grgb_mismatch !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
            rval |= IK_ERR_0000;
        } else {
            in_grgb.p_grgb_mismatch = &p_filters->input_param.grgb_mismatch;
            out_grgb.p_cr_23 = p_cr->step11.sec2.p_CR_buf_23;
            out_grgb.cr_23_size = CR_SIZE_23;
            rval |= ikc_grgb_mismatch(&in_grgb, &out_grgb);
        }
    }
    if((p_filters->update_flags.iso.cfa_leakage_filter_updated == 1u) || (p_filters->update_flags.iso.anti_aliasing_updated == 1u)) {
        ikc_in_cfa_leak_anti_alias_t in_cfa_leak_anti_alias;
        ikc_out_cfa_leak_anti_alias_t out_cfa_leak_anti_alias;

        if(p_filters->update_flags.cr.r2y.cr_12_update == 0U) {
            amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_cfa_leakage_anti_aliasing !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
            rval |= IK_ERR_0000;
        } else {
            in_cfa_leak_anti_alias.p_cfa_leakage_filter = &p_filters->input_param.cfa_leakage_filter;
            in_cfa_leak_anti_alias.p_anti_aliasing = &p_filters->input_param.anti_aliasing;
            out_cfa_leak_anti_alias.p_cr_12 = p_cr->step11.sec2.p_CR_buf_12;
            out_cfa_leak_anti_alias.cr_12_size = CR_SIZE_12;
            rval |= ikc_cfa_leakage_anti_aliasing(&in_cfa_leak_anti_alias, &out_cfa_leak_anti_alias);
        }
    }
    if(p_filters->update_flags.iso.cfa_noise_filter_updated == 1u) {
        ikc_in_cfa_noise_t in_cfa_noise;
        ikc_out_cfa_noise_t out_cfa_noise;

        if(p_filters->update_flags.cr.r2y.cr_23_update == 0U) {
            amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_cfa_noise !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
            rval |= IK_ERR_0000;
        } else {
            in_cfa_noise.p_cfa_noise_filter = &p_filters->input_param.cfa_noise_filter;
            out_cfa_noise.p_cr_23 = p_cr->step11.sec2.p_CR_buf_23;
            out_cfa_noise.cr_23_size = CR_SIZE_23;
            rval |= ikc_cfa_noise(&in_cfa_noise, &out_cfa_noise);
        }
    }
    if(p_filters->update_flags.iso.after_ce_wb_gain_updated == 1u) {
        ikc_in_after_ce_gain_t in_after_ce_gain;
        ikc_out_after_ce_gain_t out_after_ce_gain;

        if(p_filters->update_flags.cr.r2y.cr_22_update == 0U) {
            amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_after_ce_gain !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
            rval |= IK_ERR_0000;
        } else {
            in_after_ce_gain.p_after_ce_wb_gain = &p_filters->input_param.after_ce_wb_gain;
            out_after_ce_gain.p_cr_22 = p_cr->step11.sec2.p_CR_buf_22;
            out_after_ce_gain.cr_22_size = CR_SIZE_22;
            rval |= ikc_after_ce_gain(&in_after_ce_gain, &out_after_ce_gain);
        }
    }
    if(p_filters->update_flags.iso.rgb_ir_updated == 1u) {
        ikc_in_rgb_ir_t in_rgb_ir;
        ikc_out_rgb_ir_t out_rgb_ir;

        if((p_filters->update_flags.cr.r2y.cr_4_update == 0U)||
           (p_filters->update_flags.cr.r2y.cr_6_update == 0U)||
           (p_filters->update_flags.cr.r2y.cr_7_update == 0U)||
           (p_filters->update_flags.cr.r2y.cr_8_update == 0U)||
           (p_filters->update_flags.cr.r2y.cr_9_update == 0U)||
           (p_filters->update_flags.cr.r2y.cr_11_update == 0U)||
           (p_filters->update_flags.cr.r2y.cr_12_update == 0U)||
           (p_filters->update_flags.cr.r2y.cr_21_update == 0U)) {
            amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_rgb_ir !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
            rval |= IK_ERR_0000;
        } else {
            in_rgb_ir.p_rgb_ir = &p_filters->input_param.rgb_ir;
            in_rgb_ir.p_before_ce_wb_gain = &p_filters->input_param.before_ce_wb_gain;
            in_rgb_ir.p_after_ce_wb_gain = &p_filters->input_param.after_ce_wb_gain;
            in_rgb_ir.p_exp0_frontend_wb_gain = &p_filters->input_param.exp0_frontend_wb_gain;
            out_rgb_ir.p_cr_4 = p_cr->step11.sec2.p_CR_buf_4;
            out_rgb_ir.cr_4_size = CR_SIZE_4;
            out_rgb_ir.p_cr_6 = p_cr->step11.sec2.p_CR_buf_6;
            out_rgb_ir.cr_6_size = CR_SIZE_6;
            out_rgb_ir.p_cr_7 = p_cr->step11.sec2.p_CR_buf_7;
            out_rgb_ir.cr_7_size = CR_SIZE_7;
            out_rgb_ir.p_cr_8 = p_cr->step11.sec2.p_CR_buf_8;
            out_rgb_ir.cr_8_size = CR_SIZE_8;
            out_rgb_ir.p_cr_9 = p_cr->step11.sec2.p_CR_buf_9;
            out_rgb_ir.cr_9_size = CR_SIZE_9;
            out_rgb_ir.p_cr_11 = p_cr->step11.sec2.p_CR_buf_11;
            out_rgb_ir.cr_11_size = CR_SIZE_11;
            out_rgb_ir.p_cr_12 = p_cr->step11.sec2.p_CR_buf_12;
            out_rgb_ir.cr_12_size = CR_SIZE_12;
            out_rgb_ir.p_cr_21 = p_cr->step11.sec2.p_CR_buf_21;
            out_rgb_ir.cr_21_size = CR_SIZE_21;
            out_rgb_ir.p_flow = &p_flow->flow_info;
            rval |= ikc_rgb_ir(&in_rgb_ir, &out_rgb_ir);
        }
    }
    if ((p_filters->update_flags.iso.resample_str_update== 1U) ||
        (p_filters->update_flags.iso.cawarp_enable_updated == 1U)||
        (window_calculate_updated == 1U)) {
        ikc_in_cfa_prescale_t in_cfa_prescale;
        ikc_out_cfa_prescale_t out_cfa_prescale;

        if(p_filters->update_flags.cr.r2y.cr_16_update == 0U) {
            amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_cfa_prescale !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
            rval |= IK_ERR_0000;
        } else {
            in_cfa_prescale.p_flow = &p_flow->flow_info;
            in_cfa_prescale.cfa_cut_off_freq = p_filters->input_param.resample_str.cfa_cut_off_freq;
            in_cfa_prescale.p_dmy_win_geo = &p_filters->input_param.ctx_buf.result_win.dmy_win_geo;
            in_cfa_prescale.p_cfa_win_dim = &p_filters->input_param.ctx_buf.result_win.cfa_win_dim;
            in_cfa_prescale.cawarp_enable = (uint32)p_filters->input_param.cawarp_enable;
            out_cfa_prescale.p_cr_16 = p_cr->step11.sec2.p_CR_buf_16;
            out_cfa_prescale.cr_16_size = CR_SIZE_16;
            rval |= ikc_cfa_prescale(&in_cfa_prescale, &out_cfa_prescale);
        }
    }
    if((p_filters->update_flags.iso.aaa_stat_info_updated == 1u) ||
       (p_filters->update_flags.iso.af_stat_ex_info_updated == 1u) ||
       (p_filters->update_flags.iso.pg_af_stat_ex_info_updated == 1u) ||
       (p_filters->update_flags.iso.histogram_info_update == 1u) ||
       (p_filters->update_flags.iso.histogram_info_pg_update == 1u) ||
       (p_filters->update_flags.iso.window_size_info_updated == 1U)||
       (window_calculate_updated == 1U)) {
        ikc_in_aaa_t in_aaa;
        ikc_out_aaa_t out_aaa;

        if((p_filters->update_flags.cr.aaa_data == 0U)||
           (p_filters->update_flags.cr.r2y.cr_9_update == 0U)||
           (p_filters->update_flags.cr.r2y.cr_12_update == 0U)||
           (p_filters->update_flags.cr.r2y.cr_21_update == 0U)||
           (p_filters->update_flags.cr.r2y.cr_28_update == 0U)) {
            amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_aaa !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
            rval |= IK_ERR_0000;
        } else {
            in_aaa.p_sensor_info = &p_filters->input_param.sensor_info;//determine RGB-IR.
            in_aaa.p_rgb_ir = &p_filters->input_param.rgb_ir;
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
            out_aaa.p_cr_9 = p_cr->step11.sec2.p_CR_buf_9;
            out_aaa.cr_9_size = CR_SIZE_9;
            out_aaa.p_cr_12 = p_cr->step11.sec2.p_CR_buf_12;
            out_aaa.cr_12_size = CR_SIZE_12;
            out_aaa.p_cr_21 = p_cr->step11.sec2.p_CR_buf_21;
            out_aaa.cr_21_size = CR_SIZE_21;
            out_aaa.p_cr_28 = p_cr->step11.sec2.p_CR_buf_28;
            out_aaa.cr_28_size = CR_SIZE_28;
            out_aaa.p_stitching_aaa = p_cr->p_aaa;

            if (p_filters->input_param.ability == AMBA_IK_VIDEO_PIPE_Y2Y) {
                in_aaa.is_yuv_mode = 1U;
            } else {
                in_aaa.is_yuv_mode = 0U;
            }
            rval |= ikc_aaa(&in_aaa, &out_aaa);
        }
    }

    // exe_process_rgb_cr
    if(p_filters->update_flags.iso.demosaic_updated == 1U) {
        ikc_in_demosaic_t in_demosaic;
        ikc_out_demosaic_t out_demosaic;

        if((p_filters->update_flags.cr.r2y.cr_12_update == 0U)||
           (p_filters->update_flags.cr.r2y.cr_23_update == 0U)||
           (p_filters->update_flags.cr.r2y.cr_24_update == 0U)) {
            amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_demosaic_filter !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
            rval |= IK_ERR_0000;
        } else {
            in_demosaic.p_demosaic = &p_filters->input_param.demosaic;
            out_demosaic.p_cr_12 = p_cr->step11.sec2.p_CR_buf_12;
            out_demosaic.cr_12_size = CR_SIZE_12;
            out_demosaic.p_cr_23 = p_cr->step11.sec2.p_CR_buf_23;
            out_demosaic.cr_23_size = CR_SIZE_23;
            out_demosaic.p_cr_24 = p_cr->step11.sec2.p_CR_buf_24;
            out_demosaic.cr_24_size = CR_SIZE_24;
            rval |= ikc_demosaic_filter(&in_demosaic, &out_demosaic);
        }
    }
    if(p_filters->update_flags.iso.rgb_to_12y_updated == 1U) {
        ikc_in_rgb12y_t in_rgb12y;
        ikc_out_rgb12y_t out_rgb12y;

        if(p_filters->update_flags.cr.r2y.cr_25_update == 0U) {
            amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_rgb12y !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
            rval |= IK_ERR_0000;
        } else {
            in_rgb12y.p_rgb_to_12y = &p_filters->input_param.rgb_to_12y;
            out_rgb12y.p_cr_25 = p_cr->step11.sec2.p_CR_buf_25;
            out_rgb12y.cr_25_size = CR_SIZE_25;
            rval |= ikc_rgb12y(&in_rgb12y, &out_rgb12y);
        }
    }
    if(p_filters->update_flags.iso.is_1st_frame == 1U) {//should only configure once in 1st time, due to R2Y always turn on.
        ikc_in_cc_en_t in_cc_enb;
        ikc_out_cc_en_t out_cc_enb;

        if(p_filters->update_flags.cr.r2y.cr_26_update == 0U) {
            amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_color_correction_enable !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
            rval |= IK_ERR_0000;
        } else {
            in_cc_enb.is_yuv_mode = 0;
            in_cc_enb.use_cc_for_yuv2yuv = 0;
            out_cc_enb.p_cr_26 = p_cr->step11.sec2.p_CR_buf_26;
            out_cc_enb.cr_26_size = CR_SIZE_26;
            rval |= ikc_color_correction_enable(&in_cc_enb, &out_cc_enb);
        }
    }
    if(p_filters->update_flags.iso.color_correction_updated == 1U) {
        ikc_in_cc_t in_cc_3d;
        ikc_out_cc_t out_cc_3d;

        if((p_filters->update_flags.cr.r2y.cr_26_update == 0U)||
           (p_filters->update_flags.cr.r2y.cr_27_update == 0U)) {
            amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_color_correction !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
            rval |= IK_ERR_0000;
        } else {
            in_cc_3d.p_color_correction = &p_filters->input_param.color_correction;
            out_cc_3d.p_cr_26 = p_cr->step11.sec2.p_CR_buf_26;
            out_cc_3d.cr_26_size = CR_SIZE_26;
            out_cc_3d.p_cr_27 = p_cr->step11.sec2.p_CR_buf_27;
            out_cc_3d.cr_27_size = CR_SIZE_27;
            rval |= ikc_color_correction(&in_cc_3d, &out_cc_3d);
        }
    }
    if(p_filters->update_flags.iso.tone_curve_updated == 1U) {
        ikc_in_cc_out_t in_cc_out;
        ikc_out_cc_out_t out_cc_out;

        if(p_filters->update_flags.cr.r2y.cr_26_update == 0U) {
            amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_color_correction_out !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
            rval |= IK_ERR_0000;
        } else {
            in_cc_out.p_tone_curve = &p_filters->input_param.tone_curve;
            out_cc_out.p_cr_26 = p_cr->step11.sec2.p_CR_buf_26;
            out_cc_out.cr_26_size = CR_SIZE_26;
            rval |= ikc_color_correction_out(&in_cc_out, &out_cc_out);
        }
    }
    if((p_filters->update_flags.iso.rgb_to_yuv_matrix_updated == 1U) ||
       (p_filters->update_flags.iso.is_1st_frame == 1U)) {
        ikc_in_rgb2yuv_t in_rgb2yuv = {0};
        ikc_out_rgb2yuv_t out_rgb2yuv;

        if(p_filters->update_flags.cr.r2y.cr_29_update == 0U) {
            amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_rgb2yuv !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
            rval |= IK_ERR_0000;
        } else {
            in_rgb2yuv.p_rgb_to_yuv_matrix = &p_filters->input_param.rgb_to_yuv_matrix;
            out_rgb2yuv.p_cr_29 = p_cr->step11.sec2.p_CR_buf_29;
            out_rgb2yuv.cr_29_size = CR_SIZE_29;
            rval |= ikc_rgb2yuv(&in_rgb2yuv, &out_rgb2yuv);
        }
    }

    // exe_process_yuv_cr
    if(p_filters->update_flags.iso.chroma_scale_updated == 1U) {
        ikc_in_chroma_scale_t in_chroma_scale;
        ikc_out_chroma_scale_t out_chroma_scale;

        if(p_filters->update_flags.cr.r2y.cr_29_update == 0U) {
            amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_chroma_scale !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
            rval |= IK_ERR_0000;
        } else {
            if (p_filters->input_param.ability == AMBA_IK_VIDEO_PIPE_Y2Y) {
                in_chroma_scale.is_yuv_mode = 1U;
            } else {
                in_chroma_scale.is_yuv_mode = 0U;
            }
            in_chroma_scale.p_chroma_scale = &p_filters->input_param.chroma_scale;
            out_chroma_scale.p_cr_29 = p_cr->step11.sec2.p_CR_buf_29;
            out_chroma_scale.cr_29_size = CR_SIZE_29;
            rval |= ikc_chroma_scale(&in_chroma_scale, &out_chroma_scale);
        }
    }
    if(p_filters->update_flags.iso.chroma_median_filter_updated == 1U) {
        ikc_in_chroma_median_t in_chroma_median;
        ikc_out_chroma_median_t out_chroma_median;

        if((p_filters->update_flags.cr.r2y.cr_30_update == 0U)||
           (p_filters->update_flags.cr.r2y.cr_31_update == 0U)) {
            amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_chroma_median !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
            rval |= IK_ERR_0000;
        } else {
            if (p_filters->input_param.ability == AMBA_IK_VIDEO_PIPE_Y2Y) {
                in_chroma_median.is_yuv_mode = 1U;
            } else {
                in_chroma_median.is_yuv_mode = 0U;
            }
            in_chroma_median.p_chroma_median_filter = &p_filters->input_param.chroma_median_filter;
            out_chroma_median.p_cr_30 = p_cr->step11.sec2.p_CR_buf_30;
            out_chroma_median.cr_30_size = CR_SIZE_30;
            out_chroma_median.p_cr_31 = p_cr->step11.sec2.p_CR_buf_31;
            out_chroma_median.cr_31_size = CR_SIZE_31;
            rval |= ikc_chroma_median(&in_chroma_median, &out_chroma_median);
        }
    }
    if((p_filters->update_flags.iso.chroma_filter_updated == 1U) || (p_filters->update_flags.iso.wide_chroma_filter == 1U) ||
       (p_filters->update_flags.iso.wide_chroma_filter_combine == 1U) || (window_calculate_updated == 1U)) {
        ikc_in_chroma_noise_t in_chroma_noise;
        ikc_out_chroma_noise_t out_chroma_noise;
        uint64 sec2_luma_hscale_phase_inc = p_filters->input_param.ctx_buf.result_win.warp_phase.sec2_hscale_phase_inc;
        uint64 sec2_luma_vscale_phase_inc = p_filters->input_param.ctx_buf.result_win.warp_phase.sec2_vscale_phase_inc;
        uint32 chroma_radius = p_filters->input_param.chroma_filter.radius;
        uint64 sec2_chroma_hscale_phase_inc;
        uint64 sec2_chroma_vscale_phase_inc;

        if(p_filters->update_flags.cr.r2y.cr_36_update == 0U) {
            amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_chroma_noise !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
            rval |= IK_ERR_0000;
        } else {
            if(p_filters->input_param.chroma_filter.radius == 32U) {
                if(p_filters->input_param.wide_chroma_filter.enable != 0U) {
                    amba_ik_system_print_uint32_5("[IK] [ERROR] li_wide_chroma_filter.enable (= %d) should be disabled if li_chroma_filter.radius=32!!!",p_filters->input_param.wide_chroma_filter.enable,DC_U,DC_U,DC_U,DC_U);
                    rval |= IK_ERR_0008;
                }
            }
            in_chroma_noise.p_chroma_filter = &p_filters->input_param.chroma_filter;
            in_chroma_noise.p_wide_chroma_filter = &p_filters->input_param.wide_chroma_filter;
            in_chroma_noise.p_wide_chroma_combine = &p_filters->input_param.wide_chroma_combine;
            if ((in_chroma_noise.p_chroma_filter->radius==32u) && (in_chroma_noise.p_wide_chroma_filter->enable==1u)) {
                in_chroma_noise.is_wide_chroma_invalid = 1u;
            } else {
                in_chroma_noise.is_wide_chroma_invalid = 0u;
            }
            out_chroma_noise.p_cr_36 = p_cr->step11.sec2.p_CR_buf_36;
            out_chroma_noise.cr_36_size = CR_SIZE_36;
            out_chroma_noise.p_window = &p_flow->window;
            rval |= ikc_chroma_noise(&in_chroma_noise, &out_chroma_noise);
            p_flow->window.update_chroma_radius = 1U;
            //check radius causes phase_inc overflow.
            sec2_chroma_hscale_phase_inc = (sec2_luma_hscale_phase_inc * (uint64)chroma_radius) >> 5UL;
            sec2_chroma_vscale_phase_inc = (sec2_luma_vscale_phase_inc * (uint64)chroma_radius) >> 5UL;
            if ((sec2_chroma_hscale_phase_inc >= (8UL<<13UL)) || (sec2_chroma_vscale_phase_inc >= (8UL<<13UL))) {
                amba_ik_system_print_uint32_5("[IK][ERROR] exe_iso_cfg_check_raw2yuv_chroma_filter() fail. One or more downscaling_factor is out of range(>=8<<13), please check chroma radius or window setting!", DC_U, DC_U, DC_U, DC_U, DC_U);
                amba_ik_system_print_uint32_5("[IK][ERROR] sec2_chroma_hscale_phase_inc:%d, sec2_chroma_vscale_phase_inc:%d, chroma_radius:%d", (uint32)sec2_chroma_hscale_phase_inc, (uint32)sec2_chroma_vscale_phase_inc, chroma_radius, DC_U, DC_U);
                rval = IK_ERR_0101;
            }
        }
    } else {
        p_flow->window.update_chroma_radius = 0U;
    }

    if((p_filters->update_flags.iso.chroma_filter_updated == 1U)||
       (p_filters->update_flags.iso.resample_str_update == 1U)||
       (p_filters->update_flags.iso.window_size_info_updated == 1U)||
       (window_calculate_updated == 1U)) {
        ikc_in_sec2_hiso_resampler_t in_sec2_hiso_resampler;
        ikc_out_sec2_main_resampler_t out_sec2_main_resampler;
        ikc_in_hiso_resampler_size_t  in_hiso_resampler;
        ikc_out_hiso_resampler_size_t out_hiso_resampler;

        in_hiso_resampler.step   = 11;//step11
        in_hiso_resampler.p_main = &p_filters->input_param.window_size_info.main_win;
        in_hiso_resampler.chroma_filter         = &p_filters->input_param.chroma_filter;
        in_hiso_resampler.hi_nonsmooth_detect   = &p_filters->hiso_input_param.hi_nonsmooth_detect;
        in_hiso_resampler.hi_chroma_filter_high = &p_filters->hiso_input_param.hi_chroma_filter_high;

        rval |= ikc_hiso_resampler_size(&in_hiso_resampler, &out_hiso_resampler);

        in_sec2_hiso_resampler.step   = 11U;
        in_sec2_hiso_resampler.main_cut_off_freq = p_filters->input_param.resample_str.main_cut_off_freq;
        in_sec2_hiso_resampler.p_main = &out_hiso_resampler.output;
        in_sec2_hiso_resampler.chroma_radius = p_filters->input_param.chroma_filter.radius;
        out_sec2_main_resampler.p_cr_33 = p_cr->step11.sec2.p_CR_buf_33;
        out_sec2_main_resampler.cr_33_size = CR_SIZE_33;
        out_sec2_main_resampler.p_cr_35 = p_cr->step11.sec2.p_CR_buf_35;
        out_sec2_main_resampler.cr_35_size = CR_SIZE_35;
        out_sec2_main_resampler.p_cr_37 = p_cr->step11.sec2.p_CR_buf_37;
        out_sec2_main_resampler.cr_37_size = CR_SIZE_37;
        rval |= ikc_sec2_hiso_resampler(&in_sec2_hiso_resampler, &out_sec2_main_resampler);
    }
    if (p_filters->update_flags.iso.advance_spatial_filter_updated == 1U) {
        ikc_in_asf_t  in_asf;
        ikc_out_asf_t out_asf;

        if(p_filters->update_flags.cr.r2y.cr_30_update == 0U) {
            amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on step11 ikc_asf !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
            rval |= IK_ERR_0000;
        } else {
            in_asf.p_advance_spatial_filter           = &p_filters->input_param.advance_spatial_filter;
            in_asf.first_luma_processing_mode_updated = (uint32)p_filters->update_flags.iso.first_luma_processing_mode_updated;
            out_asf.p_cr_30    = p_cr->step11.sec2.p_CR_buf_30;
            out_asf.cr_30_size = CR_SIZE_30;
            rval |= ikc_asf(&in_asf, &out_asf);
        }
    }

    if(((p_filters->update_flags.iso.after_ce_wb_gain_updated == 1U)&&(p_filters->input_param.ability != AMBA_IK_VIDEO_PIPE_Y2Y)) ||
       ((p_filters->update_flags.iso.rgb_to_12y_updated == 1U)&&(p_filters->input_param.ability != AMBA_IK_VIDEO_PIPE_Y2Y))||
       (p_filters->update_flags.iso.lnl_updated == 1U)||
       (window_calculate_updated == 1U)) {
        ikc_in_lnl_t in_lnl;
        ikc_out_lnl_t out_lnl;

        if(p_filters->update_flags.cr.r2y.cr_30_update == 0U) {
            amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on Hiso step11 ikc_lnl !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
            rval |= IK_ERR_0000;
        } else {
            if(p_filters->input_param.luma_noise_reduce.enable == 1U) {
                if(p_filters->input_param.advance_spatial_filter.enable != 1U) {
                    amba_ik_system_print_uint32_5("[IK] [ERROR] IF [li_color_dependent_luma_noise_reduction.enable] = 1, then li_advanced_spatial_filter.enable (=%d) should be enabled !!!",p_filters->input_param.advance_spatial_filter.enable,DC_U,DC_U,DC_U,DC_U);
                }
            }
            in_lnl.p_luma_noise_reduce = &p_filters->input_param.luma_noise_reduce;
            in_lnl.p_after_ce_wb_gain = &p_filters->input_param.after_ce_wb_gain;
            in_lnl.p_rgb_to_12y = &p_filters->input_param.rgb_to_12y;
            in_lnl.p_cfa_win = &p_filters->input_param.ctx_buf.result_win.cfa_win_dim;
            in_lnl.use_sharpen_not_asf = 0;
            in_lnl.p_advance_spatial_filter = &p_filters->input_param.advance_spatial_filter;
            out_lnl.p_cr_30 = p_cr->step11.sec2.p_CR_buf_30;
            out_lnl.cr_30_size = CR_SIZE_30;
            out_lnl.p_flow_info = &p_flow->flow_info;
            rval |= ikc_lnl(&in_lnl, &out_lnl);
        }
    }
    if((p_filters->update_flags.iso.color_correction_updated == 1U)||
       (p_filters->update_flags.iso.tone_curve_updated == 1U)) {
        ikc_in_lnl_tbl_t in_lnl_tbl;
        ikc_out_lnl_t out_lnl;

        if(p_filters->update_flags.cr.r2y.cr_30_update == 0U) {
            amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_lnl_tbl !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
            rval |= IK_ERR_0000;
        } else {
            in_lnl_tbl.p_color_correction = &p_filters->input_param.color_correction;
            in_lnl_tbl.p_tone_curve = &p_filters->input_param.tone_curve;
            out_lnl.p_cr_30 = p_cr->step11.sec2.p_CR_buf_30;
            out_lnl.cr_30_size = CR_SIZE_30;
            rval |= ikc_lnl_tbl(&in_lnl_tbl, &out_lnl);
        }
    }
    // exe_process_hdr_ce_sub_cr
    if((p_filters->update_flags.iso.is_1st_frame == 1U) && (p_filters->update_flags.iso.sensor_information_updated == 1u)) { //should only configure once at 1st time.
        ikc_in_input_mode_sub_t in_input_mode_sub;
        ikc_out_input_mode_sub_t out_input_mode_sub;

        if((p_filters->update_flags.cr.r2y.cr_45_update == 0U)||
           (p_filters->update_flags.cr.r2y.cr_47_update == 0U)||
           (p_filters->update_flags.cr.r2y.cr_49_update == 0U)||
           (p_filters->update_flags.cr.r2y.cr_50_update == 0U)||
           (p_filters->update_flags.cr.r2y.cr_51_update == 0U)) {
            amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_input_mode_sub !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
            rval |= IK_ERR_0000;
        } else {
            in_input_mode_sub.flip_mode = p_filters->input_param.flip_mode;
            in_input_mode_sub.is_yuv_mode = 0u;
            in_input_mode_sub.sensor_pattern = p_filters->input_param.sensor_info.sensor_pattern;
            out_input_mode_sub.p_cr_45 = p_cr->step11.sec4.p_CR_buf_45;
            out_input_mode_sub.cr_45_size = CR_SIZE_45;
            out_input_mode_sub.p_cr_47 = p_cr->step11.sec4.p_CR_buf_47;
            out_input_mode_sub.cr_47_size = CR_SIZE_47;
            out_input_mode_sub.p_cr_49 = p_cr->step11.sec4.p_CR_buf_49;
            out_input_mode_sub.cr_49_size = CR_SIZE_49;
            out_input_mode_sub.p_cr_50 = p_cr->step11.sec4.p_CR_buf_50;
            out_input_mode_sub.cr_50_size = CR_SIZE_50;
            out_input_mode_sub.p_cr_51 = p_cr->step11.sec4.p_CR_buf_51;
            out_input_mode_sub.cr_51_size = CR_SIZE_51;

            rval |= ikc_input_mode_sub(&in_input_mode_sub, &out_input_mode_sub);
        }
    }
    if(p_filters->update_flags.iso.before_ce_wb_gain_updated == 1u) {
        ikc_in_before_ce_gain_t in_before_ce_gain;
        ikc_out_before_ce_gain_sub_t out_before_ce_gain_sub;

        if(p_filters->update_flags.cr.r2y.cr_51_update == 0U) {
            amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_before_ce_gain_sub !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
            rval |= IK_ERR_0000;
        } else {
            in_before_ce_gain.p_before_ce_wb_gain = &p_filters->input_param.before_ce_wb_gain;
            out_before_ce_gain_sub.p_cr_51 = p_cr->step11.sec4.p_CR_buf_51;
            out_before_ce_gain_sub.cr_51_size = CR_SIZE_51;
            rval |= ikc_before_ce_gain_sub(&in_before_ce_gain, &out_before_ce_gain_sub);
        }
    }
    if(p_filters->update_flags.iso.hdr_blend_updated == 1u) {
        ikc_in_hdr_blend_t in_hdr_blend;
        ikc_out_hdr_blend_sub_t out_hdr_blend_sub;

        if(p_filters->update_flags.cr.r2y.cr_45_update == 0U) {
            amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_hdr_blend_sub !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
            rval |= IK_ERR_0000;
        } else {
            in_hdr_blend.exp_num = p_filters->input_param.num_of_exposures;
            in_hdr_blend.p_hdr_raw_info = &p_filters->input_param.hdr_raw_info;
            in_hdr_blend.p_hdr_blend = &p_filters->input_param.hdr_blend;
            out_hdr_blend_sub.p_cr_45 = p_cr->step11.sec4.p_CR_buf_45;
            out_hdr_blend_sub.cr_45_size = CR_SIZE_45;
            rval |= ikc_hdr_blend_sub(&in_hdr_blend, &out_hdr_blend_sub);
        }
    }
    if(p_filters->update_flags.iso.fe_tone_curve_updated == 1u) {
        ikc_in_hdr_tone_curve_t in_hdr_tc;
        ikc_out_hdr_tone_curve_sub_t out_hdr_tc_sub;

        if((p_filters->update_flags.cr.r2y.cr_45_update == 0U)||
           (p_filters->update_flags.cr.r2y.cr_46_update == 0U)) {
            amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_front_end_tone_curve_sub !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
            rval |= IK_ERR_0000;
        } else {
            in_hdr_tc.p_fe_tone_curve = &p_filters->input_param.fe_tone_curve;
            out_hdr_tc_sub.p_cr_45 = p_cr->step11.sec4.p_CR_buf_45;
            out_hdr_tc_sub.cr_45_size = CR_SIZE_45;
            out_hdr_tc_sub.p_cr_46 = p_cr->step11.sec4.p_CR_buf_46;
            out_hdr_tc_sub.cr_46_size = CR_SIZE_46;
            rval |= ikc_front_end_tone_curve_sub(&in_hdr_tc, &out_hdr_tc_sub);
        }
    }
    if((p_filters->update_flags.iso.sensor_information_updated == 1u) || (window_calculate_updated == 1U)) {
        ikc_in_decompress_t in_decompress;
        ikc_out_decompress_sub_t out_decompress_sub;

        if(p_filters->update_flags.cr.r2y.cr_45_update == 0U) {
            amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_decompression_sub !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
            rval |= IK_ERR_0000;
        } else {
            in_decompress.exp_num = p_filters->input_param.num_of_exposures;
            in_decompress.compress_mode = p_filters->input_param.sensor_info.compression;//0~8, 256~259.
            in_decompress.p_dmy_win_geo = &p_filters->input_param.ctx_buf.result_win.dmy_win_geo;
            out_decompress_sub.p_cr_45 = p_cr->step11.sec4.p_CR_buf_45;
            out_decompress_sub.cr_45_size = CR_SIZE_45;
            rval |= ikc_decompression_sub(&in_decompress, &out_decompress_sub);
        }
    }
    if((p_filters->update_flags.iso.exp0_fe_static_blc_updated == 1u) ||
       (p_filters->update_flags.iso.exp1_fe_static_blc_updated == 1u) ||
       (p_filters->update_flags.iso.exp2_fe_static_blc_updated == 1u)) {
        ikc_in_hdr_blc_t in_hdr_blc;
        ikc_out_hdr_blc_sub_t out_hdr_blc_sub;

        if(p_filters->update_flags.cr.r2y.cr_45_update == 0U) {
            amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_hdr_black_level_sub !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
            rval |= IK_ERR_0000;
        } else {
            in_hdr_blc.exp_num = p_filters->input_param.num_of_exposures;
            in_hdr_blc.rgb_ir_mode = p_filters->input_param.rgb_ir.mode;
            in_hdr_blc.compression_offset = ((p_filters->input_param.sensor_info.compression > IK_RAW_COMPRESS_OFF)&&(p_filters->input_param.sensor_info.compression <= IK_RAW_COMPRESS_10p5)) ? p_filters->input_param.ctx_buf.first_compression_offset : 0;
            in_hdr_blc.p_exp0_frontend_static_blc = &p_filters->input_param.exp0_frontend_static_blc;
            in_hdr_blc.p_exp1_frontend_static_blc = &p_filters->input_param.exp1_frontend_static_blc;
            in_hdr_blc.p_exp2_frontend_static_blc = &p_filters->input_param.exp2_frontend_static_blc;
            out_hdr_blc_sub.p_cr_45 = p_cr->step11.sec4.p_CR_buf_45;
            out_hdr_blc_sub.cr_45_size = CR_SIZE_45;
            rval |= ikc_hdr_black_level_sub(&in_hdr_blc, &out_hdr_blc_sub);
        }
    }
    if((p_filters->update_flags.iso.exp0_fe_wb_gain_updated == 1u) ||
       (p_filters->update_flags.iso.exp1_fe_wb_gain_updated == 1u) ||
       (p_filters->update_flags.iso.exp2_fe_wb_gain_updated == 1u)) {
        ikc_in_hdr_dgain_t in_hdr_dgain = {0};
        ikc_out_hdr_dgain_sub_t out_hdr_dgain_sub;

        if(p_filters->update_flags.cr.r2y.cr_45_update == 0U) {
            amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_hdr_dgain_sub !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
            rval |= IK_ERR_0000;
        } else {
            in_hdr_dgain.exp_num = p_filters->input_param.num_of_exposures;
            in_hdr_dgain.rgb_ir_mode = p_filters->input_param.rgb_ir.mode;
            in_hdr_dgain.p_exp0_frontend_wb_gain = &p_filters->input_param.exp0_frontend_wb_gain;
            in_hdr_dgain.p_exp1_frontend_wb_gain = &p_filters->input_param.exp1_frontend_wb_gain;
            in_hdr_dgain.p_exp2_frontend_wb_gain = &p_filters->input_param.exp2_frontend_wb_gain;
            in_hdr_dgain.p_exp0_frontend_static_blc = &p_filters->input_param.exp0_frontend_static_blc;
            in_hdr_dgain.p_exp1_frontend_static_blc = &p_filters->input_param.exp1_frontend_static_blc;
            in_hdr_dgain.p_exp2_frontend_static_blc = &p_filters->input_param.exp2_frontend_static_blc;
            out_hdr_dgain_sub.p_cr_45 = p_cr->step11.sec4.p_CR_buf_45;
            out_hdr_dgain_sub.cr_45_size = CR_SIZE_45;
            rval |= ikc_hdr_dgain_sub(&in_hdr_dgain, &out_hdr_dgain_sub);
        }
    }
    if((p_filters->update_flags.iso.ce_updated == 1u) || (window_calculate_updated == 1U)) {
        ikc_in_ce_t in_ce;
        ikc_out_ce_sub_t out_ce_sub;

        if((p_filters->update_flags.cr.r2y.cr_49_update == 0U)||
           (p_filters->update_flags.cr.r2y.cr_50_update == 0U)||
           (p_filters->update_flags.cr.r2y.cr_51_update == 0U)) {
            amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_contrast_enhancement_sub !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
            rval |= IK_ERR_0000;
        } else {
            in_ce.sensor_pattern = p_filters->input_param.sensor_info.sensor_pattern;
            in_ce.p_dmy_win_geo = &p_filters->input_param.ctx_buf.result_win.dmy_win_geo;
            in_ce.p_ce = &p_filters->input_param.ce;
            out_ce_sub.p_cr_49 = p_cr->step11.sec4.p_CR_buf_49;
            out_ce_sub.cr_49_size = CR_SIZE_49;
            out_ce_sub.p_cr_50 = p_cr->step11.sec4.p_CR_buf_50;
            out_ce_sub.cr_50_size = CR_SIZE_50;
            out_ce_sub.p_cr_51 = p_cr->step11.sec4.p_CR_buf_51;
            out_ce_sub.cr_51_size = CR_SIZE_51;
            rval |= ikc_contrast_enhancement_sub(&in_ce, &out_ce_sub);
        }
    }
    if(p_filters->update_flags.iso.ce_input_table_updated == 1u) {
        ikc_in_ce_input_sub_t in_ce_input_sub;
        ikc_out_ce_input_sub_t out_ce_input_sub;

        if((p_filters->update_flags.cr.r2y.cr_51_update == 0U)||
           (p_filters->update_flags.cr.r2y.cr_52_update == 0U)) {
            amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_contrast_enhancement_input_sub !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
            rval |= IK_ERR_0000;
        } else {
            in_ce_input_sub.ce_enable = p_filters->input_param.ce.enable;
            in_ce_input_sub.radius = p_filters->input_param.ce.radius;
            in_ce_input_sub.epsilon = p_filters->input_param.ce.epsilon;
            in_ce_input_sub.p_ce_input_table = &p_filters->input_param.ce_input_table;
            out_ce_input_sub.p_cr_51 = p_cr->step11.sec4.p_CR_buf_51;
            out_ce_input_sub.cr_51_size = CR_SIZE_51;
            out_ce_input_sub.p_cr_52 = p_cr->step11.sec4.p_CR_buf_52;
            out_ce_input_sub.cr_52_size = CR_SIZE_52;
            rval |= ikc_contrast_enhancement_input_sub(&in_ce_input_sub, &out_ce_input_sub);
        }
    }
    if ((p_filters->update_flags.iso.vignette_compensation_updated == 1u) || (p_filters->update_flags.iso.window_size_info_updated == 1U) ||
        (window_calculate_updated == 1U)) {
        ikc_in_vignette_t in_vig;
        ikc_out_vignette_sub_t out_vig_sub;

        if((p_filters->update_flags.cr.r2y.cr_47_update == 0U)||
           (p_filters->update_flags.cr.r2y.cr_48_update == 0U)||
           (p_filters->update_flags.cr.r2y.cr_49_update == 0U)) {
            amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_vignette_sub !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
            rval |= IK_ERR_0000;
        } else {
            in_vig.vig_enable = p_filters->input_param.vig_enable;
            in_vig.flip_mode = p_filters->input_param.flip_mode;
            in_vig.p_vignette_compensation = &p_filters->input_param.vignette_compensation;
            in_vig.p_vin_sensor = &p_filters->input_param.window_size_info.vin_sensor;
            in_vig.p_dmy_win_geo = &p_filters->input_param.ctx_buf.result_win.dmy_win_geo;
            in_vig.p_active_window = &p_filters->input_param.active_window;
            out_vig_sub.p_cr_47 = p_cr->step11.sec4.p_CR_buf_47;
            out_vig_sub.cr_47_size = CR_SIZE_47;
            out_vig_sub.p_cr_48 = p_cr->step11.sec4.p_CR_buf_48;
            out_vig_sub.cr_48_size = CR_SIZE_48;
            out_vig_sub.p_cr_49 = p_cr->step11.sec4.p_CR_buf_49;
            out_vig_sub.cr_49_size = CR_SIZE_49;
            rval |= ikc_vignette_sub(&in_vig, &out_vig_sub);
        }
    }
    if(p_filters->update_flags.iso.rgb_ir_updated == 1u) {
        ikc_in_rgb_ir_t in_rgb_ir;
        ikc_out_rgb_ir_sub_t out_rgb_ir_sub;

        if((p_filters->update_flags.cr.r2y.cr_45_update == 0U)||
           (p_filters->update_flags.cr.r2y.cr_47_update == 0U)||
           (p_filters->update_flags.cr.r2y.cr_49_update == 0U)||
           (p_filters->update_flags.cr.r2y.cr_50_update == 0U)) {
            amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_rgb_ir_sub !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
            rval |= IK_ERR_0000;
        } else {
            in_rgb_ir.p_rgb_ir = &p_filters->input_param.rgb_ir;
            in_rgb_ir.p_before_ce_wb_gain = &p_filters->input_param.before_ce_wb_gain;
            in_rgb_ir.p_after_ce_wb_gain = &p_filters->input_param.after_ce_wb_gain;
            in_rgb_ir.p_exp0_frontend_wb_gain = &p_filters->input_param.exp0_frontend_wb_gain;
            out_rgb_ir_sub.p_cr_45 = p_cr->step11.sec4.p_CR_buf_45;
            out_rgb_ir_sub.cr_45_size = CR_SIZE_45;
            out_rgb_ir_sub.p_cr_47 = p_cr->step11.sec4.p_CR_buf_47;
            out_rgb_ir_sub.cr_47_size = CR_SIZE_47;
            out_rgb_ir_sub.p_cr_49 = p_cr->step11.sec4.p_CR_buf_49;
            out_rgb_ir_sub.cr_49_size = CR_SIZE_49;
            out_rgb_ir_sub.p_cr_50 = p_cr->step11.sec4.p_CR_buf_50;
            out_rgb_ir_sub.cr_50_size = CR_SIZE_50;
            rval |= ikc_rgb_ir_sub(&in_rgb_ir, &out_rgb_ir_sub);
        }
    }

    //step11_sec18
    {
        low_high_shp_t LowHighShp;
        ikc_in_sec18_step11_shp_mcts_t in_sec18_step11_shp_mcts;
        ikc_out_sec18_sharpen_t        out_sharpen;
        out_sharpen.p_cr_117= p_cr->step11.sec18.p_CR_buf_117;
        out_sharpen.cr_117_size = CR_SIZE_117;

        (void)amba_ik_system_memcpy(&LowHighShp.hili_shpns_both, &p_filters->hiso_input_param.hili_shpns_both, sizeof(ik_hili_shpns_both_t));
        (void)amba_ik_system_memcpy(&LowHighShp.hili_shpns_coring, &p_filters->hiso_input_param.hili_shpns_coring, sizeof(ik_hili_shpns_coring_t));
        (void)amba_ik_system_memcpy(&LowHighShp.hili_shpns_cor_idx_scl, &p_filters->hiso_input_param.hili_shpns_cor_idx_scl, sizeof(ik_hili_shpns_cor_idx_scl_t));
        (void)amba_ik_system_memcpy(&LowHighShp.hili_shpns_min_cor_rst, &p_filters->hiso_input_param.hili_shpns_min_cor_rst, sizeof(ik_hili_shpns_min_cor_rst_t));
        (void)amba_ik_system_memcpy(&LowHighShp.hili_shpns_max_cor_rst, &p_filters->hiso_input_param.hili_shpns_max_cor_rst, sizeof(ik_hili_shpns_max_cor_rst_t));
        (void)amba_ik_system_memcpy(&LowHighShp.hili_shpns_scl_cor, &p_filters->hiso_input_param.hili_shpns_scl_cor, sizeof(ik_hili_shpns_scl_cor_t));

        if (p_filters->hiso_input_param.hili_shpns_both.enable == 0U) { //ref: must be enabled but set to passthrousrgh by making FIRs 0
            LowHighShp.hili_shpns_both.enable = 1U;
            (void)amba_ik_system_memset(&LowHighShp.hili_shpns_noise, 0x0, sizeof(ik_hili_shpns_noise_t));
            (void)amba_ik_system_memset(&LowHighShp.hili_shpns_fir, 0x0, sizeof(ik_hili_shpns_fir_t));
        } else {
            (void)amba_ik_system_memcpy(&LowHighShp.hili_shpns_noise, &p_filters->hiso_input_param.hili_shpns_noise, sizeof(ik_hili_shpns_noise_t));
            (void)amba_ik_system_memcpy(&LowHighShp.hili_shpns_fir, &p_filters->hiso_input_param.hili_shpns_fir, sizeof(ik_hili_shpns_fir_t));
        }

        in_sec18_step11_shp_mcts.p_hili_shpns_both          = &LowHighShp.hili_shpns_both;
        in_sec18_step11_shp_mcts.p_hili_shpns_noise         = &LowHighShp.hili_shpns_noise;
        in_sec18_step11_shp_mcts.p_hili_shpns_coring        = &LowHighShp.hili_shpns_coring;
        in_sec18_step11_shp_mcts.p_hili_shpns_fir           = &LowHighShp.hili_shpns_fir;
        in_sec18_step11_shp_mcts.p_hili_shpns_cor_idx_scl   = &LowHighShp.hili_shpns_cor_idx_scl;
        in_sec18_step11_shp_mcts.p_hili_shpns_min_cor_rst   = &LowHighShp.hili_shpns_min_cor_rst;
        in_sec18_step11_shp_mcts.p_hili_shpns_max_cor_rst   = &LowHighShp.hili_shpns_max_cor_rst;
        in_sec18_step11_shp_mcts.p_hili_shpns_scl_cor       = &LowHighShp.hili_shpns_scl_cor;
        in_sec18_step11_shp_mcts.p_hi_mid_high_freq_recover = &p_filters->hiso_input_param.hi_mid_high_freq_recover;

        rval |= ikc_sec18_step11_shp_mcts(&in_sec18_step11_shp_mcts, &out_sharpen);
    }
    {
        if(p_filters->update_flags.iso.hili_combine_updated == 1U) {
            ikc_in_luma_combine_2_mctf_t      in_hili_combine;
            ikc_in_luma_combine_2_mctf_cbcr_t in_hili_combine_cbcr;
            ikc_out_mctf_t out_hili_combine;
            ik_hi_luma_combine_t hili_combine_y;
            ik_hi_chroma_filter_combine_t hili_combine_cbcr;
            hili_combine_y.t0 = p_filters->hiso_input_param.hili_combine.t0_y;
            hili_combine_y.t1 = p_filters->hiso_input_param.hili_combine.t1_y;
            hili_combine_y.alpha_min = p_filters->hiso_input_param.hili_combine.alpha_min_y;
            hili_combine_y.alpha_max = p_filters->hiso_input_param.hili_combine.alpha_max_y;
            hili_combine_y.signal_preserve = p_filters->hiso_input_param.hili_combine.signal_preserve_y;
            (void)amba_ik_system_memcpy(&hili_combine_y.max_change, &p_filters->hiso_input_param.hili_combine.max_change_y, sizeof(ik_level_method_t));
            (void)amba_ik_system_memcpy(&hili_combine_y.t0_t1_add, &p_filters->hiso_input_param.hili_combine.to_t1_add_y, sizeof(ik_level_method_t));
            (void)amba_ik_system_memcpy(&hili_combine_y.min_tar, &p_filters->hiso_input_param.hili_combine.min_tar_y, sizeof(ik_level_method_t));
            (void)amba_ik_system_memcpy(&hili_combine_y.min_tar_cb, &p_filters->hiso_input_param.hili_combine.min_tar_cb, sizeof(ik_level_t));
            (void)amba_ik_system_memcpy(&hili_combine_y.min_tar_cr, &p_filters->hiso_input_param.hili_combine.min_tar_cr, sizeof(ik_level_t));
            in_hili_combine.step = 11U;
            in_hili_combine.p_hi_luma_filter_combine = &hili_combine_y;
            out_hili_combine.p_cr_112 = p_cr->step11.sec18.p_CR_buf_112;
            out_hili_combine.cr_112_size = CR_SIZE_112;
            rval |= ikc_luma_combine_2_mctf(&in_hili_combine, &out_hili_combine);

            (void)amba_ik_system_memcpy(&hili_combine_cbcr.max_change_cb, &p_filters->hiso_input_param.hili_combine.max_change_cb, sizeof(ik_level_t));
            (void)amba_ik_system_memcpy(&hili_combine_cbcr.to_t1_add_cb, &p_filters->hiso_input_param.hili_combine.to_t1_add_cb, sizeof(ik_level_t));
            (void)amba_ik_system_memcpy(&hili_combine_cbcr.max_change_cr, &p_filters->hiso_input_param.hili_combine.max_change_cr, sizeof(ik_level_t));
            (void)amba_ik_system_memcpy(&hili_combine_cbcr.to_t1_add_cr, &p_filters->hiso_input_param.hili_combine.to_t1_add_cr, sizeof(ik_level_t));
            hili_combine_cbcr.signal_preserve_cb = p_filters->hiso_input_param.hili_combine.signal_preserve_cb;
            hili_combine_cbcr.signal_preserve_cr = p_filters->hiso_input_param.hili_combine.signal_preserve_cr;

            hili_combine_cbcr.t0_cb = p_filters->hiso_input_param.hili_combine.t0_cb;
            hili_combine_cbcr.t0_cr = p_filters->hiso_input_param.hili_combine.t0_cr;
            hili_combine_cbcr.t1_cb = p_filters->hiso_input_param.hili_combine.t1_cb;
            hili_combine_cbcr.t1_cr = p_filters->hiso_input_param.hili_combine.t1_cr;
            hili_combine_cbcr.alpha_max_cb = p_filters->hiso_input_param.hili_combine.alpha_max_cb;
            hili_combine_cbcr.alpha_max_cr = p_filters->hiso_input_param.hili_combine.alpha_max_cr;
            hili_combine_cbcr.alpha_min_cb = p_filters->hiso_input_param.hili_combine.alpha_min_cb;
            hili_combine_cbcr.alpha_min_cr = p_filters->hiso_input_param.hili_combine.alpha_min_cr;
            in_hili_combine_cbcr.p_hili_combine_cbcr = &hili_combine_cbcr;
            rval |= ikc_luma_combine_2_mctf_cbcr(&in_hili_combine_cbcr, &out_hili_combine);

        }
    }
    {
        ikc_in_hiso_frame_size_t in_hiso_frame_size;
        ikc_out_asf_t out_shpa_frame_size;
        ikc_out_sec18_mctf_shpb_t out_frame_size_div16_m1;
        in_hiso_frame_size.step = 11;  //step11
        in_hiso_frame_size.chroma_filter         = &p_filters->input_param.chroma_filter;
        in_hiso_frame_size.hi_nonsmooth_detect   = &p_filters->hiso_input_param.hi_nonsmooth_detect;
        in_hiso_frame_size.hi_chroma_filter_high = &p_filters->hiso_input_param.hi_chroma_filter_high;
        in_hiso_frame_size.p_main                = &p_filters->input_param.window_size_info.main_win;
        out_shpa_frame_size.p_cr_30    = p_cr->step11.sec2.p_CR_buf_30;
        out_shpa_frame_size.cr_30_size = CR_SIZE_30;
        rval |= ikc_hiso_shpa_frame_size(&in_hiso_frame_size, &out_shpa_frame_size);
        out_frame_size_div16_m1.p_cr_112    = p_cr->step11.sec18.p_CR_buf_112;
        out_frame_size_div16_m1.p_cr_117    = p_cr->step11.sec18.p_CR_buf_117;
        out_frame_size_div16_m1.cr_112_size = CR_SIZE_112;
        out_frame_size_div16_m1.cr_117_size = CR_SIZE_117;
        rval |= ikc_hiso_sec18_frame_size_div16_m1(&in_hiso_frame_size, &out_frame_size_div16_m1);
    }
    {
        ikc_out_sec18_mctf_shpb_t out_mctf;
        ikc_in_sec18_chroma_format_t in_sec18_chroma_format;

        in_sec18_chroma_format.step = 11u; //step11
        out_mctf.p_cr_112 = p_cr->step11.sec18.p_CR_buf_112;
        out_mctf.cr_112_size = CR_SIZE_112;
        out_mctf.p_cr_117 = p_cr->step11.sec18.p_CR_buf_117;
        out_mctf.cr_117_size = CR_SIZE_117;
        rval |= ikc_hiso_sec18_chroma_format(&in_sec18_chroma_format, &out_mctf);
    }
    {
        ikc_out_sec18_mctf_shpb_t out_mctf;
        out_mctf.p_cr_112 = p_cr->step11.sec18.p_CR_buf_112;
        out_mctf.cr_112_size = CR_SIZE_112;
        out_mctf.p_cr_117 = p_cr->step11.sec18.p_CR_buf_117;
        out_mctf.cr_117_size = CR_SIZE_117;
        rval |= ikc_hiso_mctf_ta_constraints(&out_mctf);
    }
    {
        ikc_in_def_t in_input;
        ikc_out_sec18_mctf_shpb_t out_mctf;
        in_input.step = 11U;
        out_mctf.p_cr_112 = p_cr->step11.sec18.p_CR_buf_112;
        out_mctf.cr_112_size = CR_SIZE_112;
        out_mctf.p_cr_117 = p_cr->step11.sec18.p_CR_buf_117;
        out_mctf.cr_117_size = CR_SIZE_117;
        rval |= ikc_hiso_set_regs_that_mctf_rtl_needs(&in_input, &out_mctf);
    }

    return rval;
}

static uint32 exe_process_hiso_step12_cr(idsp_flow_ctrl_t *p_flow, const amba_ik_hiso_flow_tables_list_t *p_cr, amba_ik_filter_t *p_filters, uint32 num_of_exp)
{
    uint32 rval = IK_OK;
    uint32 window_calculate_updated = 0U;

    if (num_of_exp == 0U) {
        // fix compile error
    } else {
        // default
    }

    {
        ikc_in_def_t in_input;
        ikc_out_sec18_mctf_shpb_t out_mctf;
        in_input.step = 12U;
        out_mctf.p_cr_112 = p_cr->step12.sec18.p_CR_buf_112;
        out_mctf.cr_112_size = CR_SIZE_112;
        out_mctf.p_cr_117 = p_cr->step12.sec18.p_CR_buf_117;
        out_mctf.cr_117_size = CR_SIZE_117;
        rval |= ikc_hiso_set_regs_that_mctf_rtl_needs(&in_input, &out_mctf);
    }

    rval |= exe_chroma_noise_def_value_step1(&p_cr->step12.sec2, p_filters);

    if ((p_filters->update_flags.iso.window_size_info_updated == 1U) ||
        (p_filters->update_flags.iso.cfa_window_size_info_updated == 1U) ||
        (p_filters->update_flags.iso.dmy_range_updated == 1U) ||
        (p_filters->update_flags.iso.dzoom_info_updated == 1U) ||
        (p_filters->update_flags.iso.vin_active_win_updated == 1U) ||
        (p_filters->update_flags.iso.stitching_info_updated == 1U) ||
        (p_filters->update_flags.iso.overlap_x_updated == 1U) ||
        (p_filters->update_flags.iso.flip_mode_updated == 1U) ||
        (p_filters->update_flags.iso.warp_enable_updated == 1U) ||
        (p_filters->update_flags.iso.warp_enable_2nd_updated == 1U)) {
        window_calculate_updated = 1U;
    }

    // exe_process_calib_cr
    if(p_filters->update_flags.iso.is_1st_frame == 1U) { //should only configure once at 1st time.
        ikc_in_input_mode_t in_input_mode;
        ikc_out_input_mode_t out_input_mode;

        rval |= exe_check_vin_sensor(p_filters);
        if((p_filters->update_flags.cr.r2y.cr_4_update == 0U)||
           (p_filters->update_flags.cr.r2y.cr_6_update == 0U)||
           (p_filters->update_flags.cr.r2y.cr_7_update == 0U)||
           (p_filters->update_flags.cr.r2y.cr_8_update == 0U)||
           (p_filters->update_flags.cr.r2y.cr_9_update == 0U)||
           (p_filters->update_flags.cr.r2y.cr_11_update == 0U)||
           (p_filters->update_flags.cr.r2y.cr_12_update == 0U)||
           (p_filters->update_flags.cr.r2y.cr_13_update == 0U)||
           (p_filters->update_flags.cr.r2y.cr_16_update == 0U)||
           (p_filters->update_flags.cr.r2y.cr_21_update == 0U)||
           (p_filters->update_flags.cr.r2y.cr_22_update == 0U)||
           (p_filters->update_flags.cr.r2y.cr_23_update == 0U)||
           (p_filters->update_flags.cr.r2y.cr_24_update == 0U)||
           (p_filters->update_flags.cr.r2y.cr_26_update == 0U)||
           (p_filters->update_flags.cr.r2y.cr_29_update == 0U)||
           (p_filters->update_flags.cr.r2y.cr_30_update == 0U)||
           (p_filters->update_flags.cr.r2y.cr_31_update == 0U)||
           (p_filters->update_flags.cr.r2y.cr_32_update == 0U)||
           (p_filters->update_flags.cr.r2y.cr_117_update == 0U)) {
            amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_input_mode !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
            rval |= IK_ERR_0000;
        } else {
            in_input_mode.flip_mode = p_filters->input_param.flip_mode;
            in_input_mode.p_sensor_info = &p_filters->input_param.sensor_info;
            out_input_mode.p_cr_4 = p_cr->step12.sec2.p_CR_buf_4;
            out_input_mode.cr_4_size = CR_SIZE_4;
            out_input_mode.p_cr_6 = p_cr->step12.sec2.p_CR_buf_6;
            out_input_mode.cr_6_size = CR_SIZE_6;
            out_input_mode.p_cr_7 = p_cr->step12.sec2.p_CR_buf_7;
            out_input_mode.cr_7_size = CR_SIZE_7;
            out_input_mode.p_cr_8 = p_cr->step12.sec2.p_CR_buf_8;
            out_input_mode.cr_8_size = CR_SIZE_8;
            out_input_mode.p_cr_9 = p_cr->step12.sec2.p_CR_buf_9;
            out_input_mode.cr_9_size = CR_SIZE_9;
            out_input_mode.p_cr_11 = p_cr->step12.sec2.p_CR_buf_11;
            out_input_mode.cr_11_size = CR_SIZE_11;
            out_input_mode.p_cr_12 = p_cr->step12.sec2.p_CR_buf_12;
            out_input_mode.cr_12_size = CR_SIZE_12;
            out_input_mode.p_cr_13 = p_cr->step12.sec2.p_CR_buf_13;
            out_input_mode.cr_13_size = CR_SIZE_13;
            out_input_mode.p_cr_16 = p_cr->step12.sec2.p_CR_buf_16;
            out_input_mode.cr_16_size = CR_SIZE_16;
            out_input_mode.p_cr_21 = p_cr->step12.sec2.p_CR_buf_21;
            out_input_mode.cr_21_size = CR_SIZE_21;
            out_input_mode.p_cr_22 = p_cr->step12.sec2.p_CR_buf_22;
            out_input_mode.cr_22_size = CR_SIZE_22;
            out_input_mode.p_cr_23 = p_cr->step12.sec2.p_CR_buf_23;
            out_input_mode.cr_23_size = CR_SIZE_23;
            out_input_mode.p_cr_24 = p_cr->step12.sec2.p_CR_buf_24;
            out_input_mode.cr_24_size = CR_SIZE_24;
            out_input_mode.p_cr_26 = p_cr->step12.sec2.p_CR_buf_26;
            out_input_mode.cr_26_size = CR_SIZE_26;
            out_input_mode.p_cr_29 = p_cr->step12.sec2.p_CR_buf_29;
            out_input_mode.cr_29_size = CR_SIZE_29;
            out_input_mode.p_cr_30 = p_cr->step12.sec2.p_CR_buf_30;
            out_input_mode.cr_30_size = CR_SIZE_30;
            out_input_mode.p_cr_31 = p_cr->step12.sec2.p_CR_buf_31;
            out_input_mode.cr_31_size = CR_SIZE_31;
            out_input_mode.p_cr_32 = p_cr->step12.sec2.p_CR_buf_32;
            out_input_mode.cr_32_size = CR_SIZE_32;
            out_input_mode.p_cr_117 = p_cr->step12.sec18.p_CR_buf_117;
            out_input_mode.cr_117_size = CR_SIZE_117;
            out_input_mode.p_flow = &p_flow->flow_info;
            out_input_mode.p_calib = &p_flow->calib;

            if (p_filters->input_param.ability == AMBA_IK_VIDEO_PIPE_Y2Y) {
                in_input_mode.is_yuv_mode = 1U;
            } else {
                in_input_mode.is_yuv_mode = 0U;
            }
            rval |= ikc_input_mode(&in_input_mode, &out_input_mode);
        }
    }
    if (window_calculate_updated == 1U) {
        rval |= exe_win_calc_wrapper(p_filters);
    }
    if((p_filters->update_flags.iso.static_bpc_updated == 1u) || (p_filters->update_flags.iso.static_bpc_internal_updated == 1u) ||
       (p_filters->update_flags.iso.dynamic_bpc_updated == 1u) || (p_filters->update_flags.iso.window_size_info_updated == 1U)) {
        if(p_filters->input_param.ctx_buf.sbp_internal_mode_flag == 1u) {
            ikc_in_static_bad_pixel_internal_t in_sbp_internal;
            ikc_out_static_bad_pixel_internal_t out_sbp;
            ik_static_bad_pixel_correction_internal_t sbp_internal_tmp;
            const void *void_ptr;

            if(p_filters->update_flags.cr.r2y.cr_12_update == 0U) {
                amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_static_bad_pixel_internal !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
                rval |= IK_ERR_0000;
            } else {
                //consider 64 bit compiler, flow data sbp addr(u32) goes on physical addr + offset.
                (void)amba_ik_system_memcpy(&sbp_internal_tmp,&p_filters->input_param.sbp_internal,sizeof(ik_static_bad_pixel_correction_internal_t));
                void_ptr = amba_ik_system_virt_to_phys(sbp_internal_tmp.p_map);
                (void)amba_ik_system_memcpy(&sbp_internal_tmp.p_map, &void_ptr, sizeof(void*));
                in_sbp_internal.dbp_enable = p_filters->input_param.dynamic_bpc.enable;
                in_sbp_internal.p_sbp_internal = &sbp_internal_tmp;
                out_sbp.p_cr_12 = p_cr->step12.sec2.p_CR_buf_12;
                out_sbp.cr_12_size = CR_SIZE_12;
                out_sbp.p_flow = &p_flow->flow_info;
                rval |= ikc_static_bad_pixel_internal(&in_sbp_internal, &out_sbp);
            }
        } else {
            ikc_in_static_bad_pixel_t in_sbp;
            ikc_out_static_bad_pixel_t out_sbp;
            ik_static_bad_pxl_cor_t static_bpc_tmp;
            const void *void_ptr;

            if(p_filters->update_flags.cr.r2y.cr_12_update == 0U) {
                amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_static_bad_pixel !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
                rval |= IK_ERR_0000;
            } else {
                //consider 64 bit compiler, flow data sbp addr(u32) goes on physical addr + offset.
                (void)amba_ik_system_memcpy(&static_bpc_tmp,&p_filters->input_param.static_bpc,sizeof(ik_static_bad_pxl_cor_t));
                void_ptr = amba_ik_system_virt_to_phys(static_bpc_tmp.calib_sbp_info.sbp_buffer);
                (void)amba_ik_system_memcpy(&static_bpc_tmp.calib_sbp_info.sbp_buffer, &void_ptr, sizeof(void*));
                in_sbp.dbp_enable = p_filters->input_param.dynamic_bpc.enable;
                in_sbp.sbp_enable = p_filters->input_param.sbp_enable;
                in_sbp.p_static_bpc = &static_bpc_tmp;
                in_sbp.p_vin_sensor = &p_filters->input_param.window_size_info.vin_sensor;
                out_sbp.p_cr_12 = p_cr->step12.sec2.p_CR_buf_12;
                out_sbp.cr_12_size = CR_SIZE_12;
                out_sbp.p_flow = &p_flow->flow_info;
                rval |= ikc_static_bad_pixel(&in_sbp, &out_sbp);
            }
        }
        p_flow->flow_info.update_sbp = 1U;
    } else {
        p_flow->flow_info.update_sbp = 0U;
    }
    if ((p_filters->update_flags.iso.vignette_compensation_updated == 1U)||
        (p_filters->update_flags.iso.window_size_info_updated == 1U) || (window_calculate_updated == 1U)) {
        ikc_in_vignette_t in_vig;
        ikc_out_vignette_t out_vig;

        if((p_filters->update_flags.cr.r2y.cr_9_update == 0U)||
           (p_filters->update_flags.cr.r2y.cr_10_update == 0U)||
           (p_filters->update_flags.cr.r2y.cr_11_update == 0U)) {
            amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_vignette !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
            rval |= IK_ERR_0000;
        } else {
            in_vig.vig_enable = p_filters->input_param.vig_enable;
            in_vig.flip_mode = p_filters->input_param.flip_mode;
            in_vig.p_vignette_compensation = &p_filters->input_param.vignette_compensation;
            in_vig.p_vin_sensor = &p_filters->input_param.window_size_info.vin_sensor;
            in_vig.p_dmy_win_geo = &p_filters->input_param.ctx_buf.result_win.dmy_win_geo;
            in_vig.p_active_window = &p_filters->input_param.active_window;
            out_vig.p_cr_9 = p_cr->step12.sec2.p_CR_buf_9;
            out_vig.cr_9_size = CR_SIZE_9;
            out_vig.p_cr_10 = p_cr->step12.sec2.p_CR_buf_10;
            out_vig.cr_10_size = CR_SIZE_10;
            out_vig.p_cr_11 = p_cr->step12.sec2.p_CR_buf_11;
            out_vig.cr_11_size = CR_SIZE_11;
            out_vig.p_flow = &p_flow->flow_info;
            rval |= ikc_vignette(&in_vig, &out_vig);
        }
        p_flow->flow_info.update_vignette = 1U;
    } else {
        p_flow->flow_info.update_vignette = 0U;
    }
    if ((p_filters->update_flags.iso.calib_ca_warp_info_updated == 1U) || (p_filters->update_flags.iso.cawarp_enable_updated == 1U) ||
        (p_filters->update_flags.iso.cawarp_internal_updated == 1U) || (p_filters->update_flags.iso.window_size_info_updated == 1U) ||
        (window_calculate_updated == 1U)) {
        if (p_filters->input_param.ctx_buf.cawarp_internal_mode_flag == 1U) {
            ikc_in_cawarp_internal_t in_ca;
            ikc_out_cawarp_t out_ca;

            if((p_filters->update_flags.cr.cawarp == 0U)||
               (p_filters->update_flags.cr.r2y.cr_16_update == 0U)||
               (p_filters->update_flags.cr.r2y.cr_17_update == 0U)) {
                amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_cawarp_internal !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
                rval |= IK_ERR_0000;
            } else {
                in_ca.p_stitching_info = &p_filters->input_param.stitching_info;
                in_ca.p_ca_warp_internal = &p_filters->input_param.ca_warp_internal;
                out_ca.p_cr_16 = p_cr->step12.sec2.p_CR_buf_16;
                out_ca.cr_16_size = CR_SIZE_16;
                out_ca.p_cr_17 = p_cr->step12.sec2.p_CR_buf_17;
                out_ca.cr_17_size = CR_SIZE_17;
                out_ca.p_cr_18 = p_cr->step12.sec2.p_CR_buf_18;
                out_ca.cr_18_size = CR_SIZE_18;
                out_ca.p_cr_19 = p_cr->step12.sec2.p_CR_buf_19;
                out_ca.cr_19_size = CR_SIZE_19;
                out_ca.p_cr_20 = p_cr->step12.sec2.p_CR_buf_20;
                out_ca.cr_20_size = CR_SIZE_20;
                out_ca.p_calib = &p_flow->calib;
                out_ca.p_cawarp_hor_table_addr_red = p_cr->p_ca_warp_hor_red;
                out_ca.h_red_size = CAWARP_VIDEO_MAXSIZE;
                out_ca.p_cawarp_vertical_table_addr_red = p_cr->p_ca_warp_ver_red;
                out_ca.v_red_size = CAWARP_VIDEO_MAXSIZE;
                out_ca.p_cawarp_hor_table_addr_blue = p_cr->p_ca_warp_hor_blue;
                out_ca.h_blue_size = CAWARP_VIDEO_MAXSIZE;
                out_ca.p_cawarp_vertical_table_addr_blue = p_cr->p_ca_warp_ver_blue;
                out_ca.v_blue_size = CAWARP_VIDEO_MAXSIZE;
                rval |= ikc_cawarp_internal(&in_ca, &out_ca);

                p_flow->calib.cawarp_horizontal_table_addr_red = physical2u32(p_cr->p_ca_warp_hor_red);
                p_flow->calib.cawarp_horizontal_table_addr_blue = physical2u32(p_cr->p_ca_warp_hor_blue);
                p_flow->calib.cawarp_vertical_table_addr_red = physical2u32(p_cr->p_ca_warp_ver_red);
                p_flow->calib.cawarp_vertical_table_addr_blue = physical2u32(p_cr->p_ca_warp_ver_blue);
            }
        } else {
            ikc_in_cawarp_t in_ca;
            ikc_out_cawarp_t out_ca;

            if((p_filters->update_flags.cr.cawarp == 0U)||
               (p_filters->update_flags.cr.r2y.cr_16_update == 0U)||
               (p_filters->update_flags.cr.r2y.cr_17_update == 0U)) {
                amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_cawarp !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
                rval |= IK_ERR_0000;
            } else {
                in_ca.is_group_cmd = 0u;
                in_ca.is_hiso      = 1u;
                in_ca.cawarp_enable = (uint32)p_filters->input_param.cawarp_enable;
                in_ca.flip_mode = p_filters->input_param.flip_mode;
                in_ca.p_stitching_info = &p_filters->input_param.stitching_info;
                in_ca.p_window_size_info = &p_filters->input_param.window_size_info;
                in_ca.p_result_win = &p_filters->input_param.ctx_buf.result_win;
                in_ca.p_calib_ca_warp_info = &p_filters->input_param.calib_ca_warp_info;
                out_ca.p_cr_16 = p_cr->step12.sec2.p_CR_buf_16;
                out_ca.cr_16_size = CR_SIZE_16;
                out_ca.p_cr_17 = p_cr->step12.sec2.p_CR_buf_17;
                out_ca.cr_17_size = CR_SIZE_17;
                out_ca.p_cr_18 = p_cr->step12.sec2.p_CR_buf_18;
                out_ca.cr_18_size = CR_SIZE_18;
                out_ca.p_cr_19 = p_cr->step12.sec2.p_CR_buf_19;
                out_ca.cr_19_size = CR_SIZE_19;
                out_ca.p_cr_20 = p_cr->step12.sec2.p_CR_buf_20;
                out_ca.cr_20_size = CR_SIZE_20;
                out_ca.p_calib = &p_flow->calib;
                out_ca.p_cawarp_hor_table_addr_red = p_cr->p_ca_warp_hor_red;
                out_ca.h_red_size = CAWARP_VIDEO_MAXSIZE;
                out_ca.p_cawarp_vertical_table_addr_red = p_cr->p_ca_warp_ver_red;
                out_ca.v_red_size = CAWARP_VIDEO_MAXSIZE;
                out_ca.p_cawarp_hor_table_addr_blue = p_cr->p_ca_warp_hor_blue;
                out_ca.h_blue_size = CAWARP_VIDEO_MAXSIZE;
                out_ca.p_cawarp_vertical_table_addr_blue = p_cr->p_ca_warp_ver_blue;
                out_ca.v_blue_size = CAWARP_VIDEO_MAXSIZE;

                rval |= ikc_cawarp(&in_ca, &out_ca);

                p_flow->calib.cawarp_horizontal_table_addr_red = physical2u32(p_cr->p_ca_warp_hor_red);
                p_flow->calib.cawarp_horizontal_table_addr_blue = physical2u32(p_cr->p_ca_warp_hor_blue);
                p_flow->calib.cawarp_vertical_table_addr_red = physical2u32(p_cr->p_ca_warp_ver_red);
                p_flow->calib.cawarp_vertical_table_addr_blue = physical2u32(p_cr->p_ca_warp_ver_blue);
            }
        }
        p_flow->calib.update_CA_warp = 1U;
    } else {
        p_flow->calib.update_CA_warp = 0U;
    }
    if((p_filters->update_flags.iso.chroma_filter_updated == 1U)||
       (p_filters->update_flags.iso.warp_enable_updated == 1U)||
       (p_filters->update_flags.iso.calib_warp_info_updated == 1U)||
       (p_filters->update_flags.iso.warp_internal_updated == 1U)||
       (p_filters->update_flags.iso.window_size_info_updated == 1U)||
       (window_calculate_updated == 1U)) {
        if(p_filters->input_param.ctx_buf.warp_internal_mode_flag == 1U) { //internal warp
#if 0
            ikc_in_warp_internal_t in_warp_internal;
            ikc_out_hwarp_t out_hwarp;

            if((p_filters->update_flags.cr.warp == 0U)||
               (p_filters->update_flags.cr.r2y.cr_33_update == 0U)||
               (p_filters->update_flags.cr.r2y.cr_34_update == 0U)||
               (p_filters->update_flags.cr.r2y.cr_35_update == 0U)||
               (p_filters->update_flags.cr.r2y.cr_42_update == 0U)||
               (p_filters->update_flags.cr.r2y.cr_43_update == 0U)||
               (p_filters->update_flags.cr.r2y.cr_44_update == 0U)) {
                amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_warp_internal !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
                rval |= IK_ERR_0000;
            } else {
                in_warp_internal.flip_mode = p_filters->input_param.flip_mode;
                in_warp_internal.p_window_size_info = &p_filters->input_param.window_size_info;
                in_warp_internal.p_act_win_crop = &p_filters->input_param.ctx_buf.result_win.act_win_crop;
                in_warp_internal.p_dmy_win_geo = &p_filters->input_param.ctx_buf.result_win.dmy_win_geo;
                in_warp_internal.p_cfa_win_dim = &p_filters->input_param.ctx_buf.result_win.cfa_win_dim;
                in_warp_internal.p_dmy_range = &p_filters->input_param.dmy_range;
                in_warp_internal.chroma_radius = p_filters->input_param.chroma_filter.radius;
                in_warp_internal.p_stitching_info = &p_filters->input_param.stitching_info;
                in_warp_internal.p_warp_internal = &p_filters->input_param.warp_internal;
                in_warp_internal.dram_efficiency = p_filters->input_param.warp_buf_info.dram_efficiency;
                in_warp_internal.luma_wait_lines = p_filters->input_param.warp_buf_info.luma_wait_lines;
                in_warp_internal.luma_dma_size = p_filters->input_param.warp_buf_info.luma_dma_size;
                out_hwarp.p_cr_33 = p_flow_tbl_list->r2y.p_CR_buf_33;
                out_hwarp.cr_33_size = CR_SIZE_33;
                out_hwarp.p_cr_34 = p_flow_tbl_list->r2y.p_CR_buf_34;
                out_hwarp.cr_34_size = CR_SIZE_34;
                out_hwarp.p_cr_35 = p_flow_tbl_list->r2y.p_CR_buf_35;
                out_hwarp.cr_35_size = CR_SIZE_35;
                out_hwarp.p_cr_42 = p_flow_tbl_list->r2y.p_CR_buf_42;
                out_hwarp.cr_42_size = CR_SIZE_42;
                out_hwarp.p_cr_43 = p_flow_tbl_list->r2y.p_CR_buf_43;
                out_hwarp.cr_43_size = CR_SIZE_43;
                out_hwarp.p_cr_44 = p_flow_tbl_list->r2y.p_CR_buf_44;
                out_hwarp.cr_44_size = CR_SIZE_44;
                out_hwarp.p_flow_info = &p_flow->flow_info;
                out_hwarp.p_window = &p_flow->window;
                out_hwarp.p_phase = &p_flow->phase;
                out_hwarp.p_calib = &p_flow->calib;
                out_hwarp.p_stitch = &p_flow->stitch;
                out_hwarp.p_warp_horizontal_table_address = p_flow_tbl_list->r2y.p_warp_hor;
                out_hwarp.h_size = WARP_VIDEO_MAXHORGRID * WARP_VIDEO_MAXVERGRID * 2U * 2U;// multi_slice buffer.
                out_hwarp.p_warp_vertical_table_address = p_flow_tbl_list->r2y.p_warp_ver;
                out_hwarp.v_size = WARP_VIDEO_MAXHORGRID * WARP_VIDEO_MAXVERGRID * 2U * 2U;// multi_slice buffer.
                rval |= ikc_warp_internal(&in_warp_internal, &out_hwarp);
            }
#endif
        } else {
            ikc_in_hwarp_t in_hwarp;
            ikc_out_hwarp_t out_hwarp;
            //ikc_in_vwarp_t in_vwarp;
            //ikc_out_vwarp_t out_vwarp;

            if((p_filters->update_flags.cr.warp == 0U)||
               (p_filters->update_flags.cr.r2y.cr_33_update == 0U)||
               (p_filters->update_flags.cr.r2y.cr_34_update == 0U)||
               (p_filters->update_flags.cr.r2y.cr_35_update == 0U)||
               (p_filters->update_flags.cr.r2y.cr_42_update == 0U)||
               (p_filters->update_flags.cr.r2y.cr_43_update == 0U)||
               (p_filters->update_flags.cr.r2y.cr_44_update == 0U)) {
                amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_hwarp !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
                rval |= IK_ERR_0000;
            } else {
                {
                    in_hwarp.is_group_cmd = 0u;
                    in_hwarp.is_hiso      = 1u;
                    in_hwarp.warp_enable = (uint32)p_filters->input_param.warp_enable;
                    in_hwarp.flip_mode = p_filters->input_param.flip_mode;
                    in_hwarp.p_window_size_info = &p_filters->input_param.window_size_info;
                    in_hwarp.p_act_win_crop = &p_filters->input_param.ctx_buf.result_win.act_win_crop;
                    in_hwarp.p_dmy_win_geo = &p_filters->input_param.ctx_buf.result_win.dmy_win_geo;
                    in_hwarp.p_cfa_win_dim = &p_filters->input_param.ctx_buf.result_win.cfa_win_dim;
                    in_hwarp.p_dmy_range = &p_filters->input_param.dmy_range;
                    in_hwarp.chroma_radius = p_filters->input_param.chroma_filter.radius;
                    in_hwarp.is_y2y_420input = (p_filters->input_param.yuv_mode == AMBA_IK_YUV420) ? 1U : 0U;
                    in_hwarp.p_stitching_info = &p_filters->input_param.stitching_info;
                    in_hwarp.p_calib_warp_info = &p_filters->input_param.calib_warp_info;
                    in_hwarp.dram_efficiency = p_filters->input_param.warp_buf_info.dram_efficiency;
                    in_hwarp.luma_wait_lines = p_filters->input_param.warp_buf_info.luma_wait_lines;
                    in_hwarp.luma_dma_size = p_filters->input_param.warp_buf_info.luma_dma_size;
                    in_hwarp.is_still_422 = 0;
                    out_hwarp.p_cr_33 = p_cr->step12.sec2.p_CR_buf_33;
                    out_hwarp.cr_33_size = CR_SIZE_33;
                    out_hwarp.p_cr_34 = p_cr->step12.sec2.p_CR_buf_34;
                    out_hwarp.cr_34_size = CR_SIZE_34;
                    out_hwarp.p_cr_35 = p_cr->step12.sec2.p_CR_buf_35;
                    out_hwarp.cr_35_size = CR_SIZE_35;
                    out_hwarp.p_flow_info = &p_flow->flow_info;
                    out_hwarp.p_window = &p_flow->window;
                    out_hwarp.p_phase = &p_flow->phase;
                    out_hwarp.p_calib = &p_flow->calib;
                    out_hwarp.p_stitch = &p_flow->stitch;
                    out_hwarp.p_warp_horizontal_table_address = p_cr->p_warp_hor;
                    out_hwarp.h_size = WARP_VIDEO_MAXHORGRID * WARP_VIDEO_MAXVERGRID * 2U * 2U;// multi_slice buffer.

                    rval |= ikc_hwarp(&in_hwarp, &out_hwarp);
                }
            }
        }
        p_flow->calib.update_lens_warp = 1U;
    } else {
        p_flow->calib.update_lens_warp = 0U;
    }

    // exe_process_hdr_ce_cr
    if(p_filters->update_flags.iso.before_ce_wb_gain_updated == 1u) {
        ikc_in_before_ce_gain_t in_before_ce_gain={0};
        ikc_out_before_ce_gain_t out_before_ce_gain;

        if(p_filters->update_flags.cr.r2y.cr_13_update == 0U) {
            amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_before_ce_gain !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
            rval |= IK_ERR_0000;
        } else {
            in_before_ce_gain.p_before_ce_wb_gain = &p_filters->input_param.before_ce_wb_gain;
            out_before_ce_gain.p_cr_13 = p_cr->step12.sec2.p_CR_buf_13;
            out_before_ce_gain.cr_13_size = CR_SIZE_13;
            rval |= ikc_before_ce_gain(&in_before_ce_gain, &out_before_ce_gain);
        }
    }
    if(p_filters->update_flags.iso.hdr_blend_updated == 1u) {
        ikc_in_hdr_blend_t in_hdr_blend;
        ikc_out_hdr_blend_t out_hdr_blend;

        if(p_filters->update_flags.cr.r2y.cr_4_update == 0U) {
            amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_hdr_blend !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
            rval |= IK_ERR_0000;
        } else {
            in_hdr_blend.exp_num = p_filters->input_param.num_of_exposures;
            in_hdr_blend.p_hdr_raw_info = &p_filters->input_param.hdr_raw_info;
            in_hdr_blend.p_hdr_blend = &p_filters->input_param.hdr_blend;
            out_hdr_blend.p_cr_4 = p_cr->step12.sec2.p_CR_buf_4;
            out_hdr_blend.cr_4_size = CR_SIZE_4;
            out_hdr_blend.p_flow = &p_flow->flow_info;
            rval |= ikc_hdr_blend(&in_hdr_blend, &out_hdr_blend);
        }
    }
    if(p_filters->update_flags.iso.fe_tone_curve_updated == 1u) {
        ikc_in_hdr_tone_curve_t in_hdr_tc;
        ikc_out_hdr_tone_curve_t out_hdr_tc;

        if((p_filters->update_flags.cr.r2y.cr_4_update == 0U)||
           (p_filters->update_flags.cr.r2y.cr_5_update == 0U)) {
            amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_front_end_tone_curve !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
            rval |= IK_ERR_0000;
        } else {
            in_hdr_tc.p_fe_tone_curve = &p_filters->input_param.fe_tone_curve;
            out_hdr_tc.p_cr_4 = p_cr->step12.sec2.p_CR_buf_4;
            out_hdr_tc.cr_4_size = CR_SIZE_4;
            out_hdr_tc.p_cr_5 = p_cr->step12.sec2.p_CR_buf_5;
            out_hdr_tc.cr_5_size = CR_SIZE_5;
            rval |= ikc_front_end_tone_curve(&in_hdr_tc, &out_hdr_tc);
        }
    }
    if((p_filters->update_flags.iso.sensor_information_updated == 1u)||(window_calculate_updated == 1U)) {
        ikc_in_decompress_t in_decompress;
        ikc_out_decompress_t out_decompress;

        if(p_filters->update_flags.cr.r2y.cr_4_update == 0U) {
            amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_decompression !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
            rval |= IK_ERR_0000;
        } else {
            in_decompress.exp_num = 1U;
            in_decompress.compress_mode = p_filters->input_param.sensor_info.compression;////0~8, 256~259.
            in_decompress.p_dmy_win_geo = &p_filters->input_param.ctx_buf.result_win.dmy_win_geo;
            out_decompress.p_cr_4 = p_cr->step12.sec2.p_CR_buf_4;
            out_decompress.cr_4_size = CR_SIZE_4;
            rval |= ikc_decompression(&in_decompress, &out_decompress);
            if(p_filters->update_flags.iso.is_1st_frame == 1u) {
                p_filters->input_param.ctx_buf.first_compression_offset = p_filters->input_param.sensor_info.compression_offset;
            }
        }
    }
    if((p_filters->update_flags.iso.exp0_fe_static_blc_updated == 1u) ||
       (p_filters->update_flags.iso.exp1_fe_static_blc_updated == 1u) ||
       (p_filters->update_flags.iso.exp2_fe_static_blc_updated == 1u)) {
        ikc_in_hdr_blc_t in_hdr_blc;
        ikc_out_hdr_blc_t out_hdr_blc;

        if((p_filters->update_flags.cr.r2y.cr_4_update == 0U)||
           (p_filters->update_flags.cr.r2y.cr_6_update == 0U)||
           (p_filters->update_flags.cr.r2y.cr_7_update == 0U)||
           (p_filters->update_flags.cr.r2y.cr_8_update == 0U)) {
            amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_hdr_black_level !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
            rval |= IK_ERR_0000;
        } else {
            in_hdr_blc.exp_num = p_filters->input_param.num_of_exposures;
            in_hdr_blc.rgb_ir_mode = p_filters->input_param.rgb_ir.mode;
            in_hdr_blc.compression_offset = ((p_filters->input_param.sensor_info.compression > IK_RAW_COMPRESS_OFF)&&(p_filters->input_param.sensor_info.compression <= IK_RAW_COMPRESS_10p5)) ? p_filters->input_param.ctx_buf.first_compression_offset : 0;
            in_hdr_blc.p_exp0_frontend_static_blc = &p_filters->input_param.exp0_frontend_static_blc;
            in_hdr_blc.p_exp1_frontend_static_blc = &p_filters->input_param.exp1_frontend_static_blc;
            in_hdr_blc.p_exp2_frontend_static_blc = &p_filters->input_param.exp2_frontend_static_blc;
            out_hdr_blc.p_cr_4 = p_cr->step12.sec2.p_CR_buf_4;
            out_hdr_blc.cr_4_size = CR_SIZE_4;
            out_hdr_blc.p_cr_6 = p_cr->step12.sec2.p_CR_buf_6;
            out_hdr_blc.cr_6_size = CR_SIZE_6;
            out_hdr_blc.p_cr_7 = p_cr->step12.sec2.p_CR_buf_7;
            out_hdr_blc.cr_7_size = CR_SIZE_7;
            out_hdr_blc.p_cr_8 = p_cr->step12.sec2.p_CR_buf_8;
            out_hdr_blc.cr_8_size = CR_SIZE_8;
            rval |= ikc_hdr_black_level(&in_hdr_blc, &out_hdr_blc);
        }
    }
    if((p_filters->update_flags.iso.exp0_fe_wb_gain_updated == 1u) ||
       (p_filters->update_flags.iso.exp1_fe_wb_gain_updated == 1u) ||
       (p_filters->update_flags.iso.exp2_fe_wb_gain_updated == 1u)) {
        ikc_in_hdr_dgain_t in_hdr_dgain = {0};
        ikc_out_hdr_dgain_t out_hdr_dgain;

        if(p_filters->update_flags.cr.r2y.cr_4_update == 0U) {
            amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_hdr_dgain !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
            rval |= IK_ERR_0000;
        } else {
            in_hdr_dgain.exp_num = p_filters->input_param.num_of_exposures;
            in_hdr_dgain.rgb_ir_mode = p_filters->input_param.rgb_ir.mode;
            in_hdr_dgain.p_exp0_frontend_wb_gain = &p_filters->input_param.exp0_frontend_wb_gain;
            in_hdr_dgain.p_exp1_frontend_wb_gain = &p_filters->input_param.exp1_frontend_wb_gain;
            in_hdr_dgain.p_exp2_frontend_wb_gain = &p_filters->input_param.exp2_frontend_wb_gain;
            in_hdr_dgain.p_exp0_frontend_static_blc = &p_filters->input_param.exp0_frontend_static_blc;
            in_hdr_dgain.p_exp1_frontend_static_blc = &p_filters->input_param.exp1_frontend_static_blc;
            in_hdr_dgain.p_exp2_frontend_static_blc = &p_filters->input_param.exp2_frontend_static_blc;
            out_hdr_dgain.p_cr_4 = p_cr->step12.sec2.p_CR_buf_4;
            out_hdr_dgain.cr_4_size = CR_SIZE_4;
            rval |= ikc_hdr_dgain(&in_hdr_dgain, &out_hdr_dgain);
        }
    }
    if((p_filters->update_flags.iso.ce_updated == 1u)||(window_calculate_updated == 1U)) {
        ikc_in_ce_t in_ce;
        ikc_out_ce_t out_ce;

        if(p_filters->update_flags.cr.r2y.cr_13_update == 0U) {
            amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_contrast_enhancement !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
            rval |= IK_ERR_0000;
        } else {
            in_ce.exp_num = p_filters->input_param.num_of_exposures;
            in_ce.sensor_pattern = p_filters->input_param.sensor_info.sensor_pattern;
            in_ce.p_dmy_win_geo = &p_filters->input_param.ctx_buf.result_win.dmy_win_geo;
            in_ce.p_ce = &p_filters->input_param.ce;
            out_ce.p_cr_13 = p_cr->step12.sec2.p_CR_buf_13;
            out_ce.cr_13_size = CR_SIZE_13;
            out_ce.p_flow = &p_flow->flow_info;
            rval |= ikc_contrast_enhancement(&in_ce, &out_ce);
        }
    }
    if(p_filters->update_flags.iso.ce_input_table_updated == 1u) {
        ikc_in_ce_input_t in_ce_input;
        ikc_out_ce_input_t out_ce_input;

        if((p_filters->update_flags.cr.r2y.cr_13_update == 0U)||
           (p_filters->update_flags.cr.r2y.cr_14_update == 0U)) {
            amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_contrast_enhancement_input !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
            rval |= IK_ERR_0000;
        } else {
            in_ce_input.ce_enable = p_filters->input_param.ce.enable;
            in_ce_input.p_ce_input_table = &p_filters->input_param.ce_input_table;
            out_ce_input.p_cr_13 = p_cr->step12.sec2.p_CR_buf_13;
            out_ce_input.cr_13_size = CR_SIZE_13;
            out_ce_input.p_cr_14 = p_cr->step12.sec2.p_CR_buf_14;
            out_ce_input.cr_14_size = CR_SIZE_14;
            rval |= ikc_contrast_enhancement_input(&in_ce_input, &out_ce_input);
        }
    }
    if(p_filters->update_flags.iso.ce_out_table_updated == 1u) {
        ikc_in_ce_output_t in_ce_output;
        ikc_out_ce_t out_ce;

        if(p_filters->update_flags.cr.r2y.cr_13_update == 0U) {
            amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_contrast_enhancement_output !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
            rval |= IK_ERR_0000;
        } else {
            in_ce_output.ce_enable = p_filters->input_param.ce.enable;
            in_ce_output.p_ce_out_table = &p_filters->input_param.ce_out_table;
            out_ce.p_cr_13 = p_cr->step12.sec2.p_CR_buf_13;
            out_ce.cr_13_size = CR_SIZE_13;
            rval |= ikc_contrast_enhancement_output(&in_ce_output, &out_ce);
        }
    }

    // exe_process_cfa_cr
    if(p_filters->update_flags.iso.li2_dynamic_bpc_updated == 1u) {
        ikc_in_dynamic_bad_pixel_t in_dbp;
        ikc_out_dynamic_bad_pixel_t out_dbp;

        if((p_filters->update_flags.cr.r2y.cr_11_update == 0U)||
           (p_filters->update_flags.cr.r2y.cr_12_update == 0U)) {
            amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_dynamic_bad_pixel !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
            rval |= IK_ERR_0000;
        } else {
            if(p_filters->input_param.ctx_buf.sbp_internal_mode_flag == 1u) {
                in_dbp.sbp_enable = p_filters->input_param.sbp_internal.enable;
            } else {
                in_dbp.sbp_enable = p_filters->input_param.sbp_enable;
            }
            if((p_filters->input_param.rgb_ir.ir_only==1U)&&(p_filters->input_param.sensor_info.sensor_mode==1U)) { //ir_only, runing on RGB bayer pattern, carry IR on B channel.
                in_dbp.rgb_ir_mode = 0u;
            } else {
                in_dbp.rgb_ir_mode = p_filters->input_param.rgb_ir.mode;
            }
            in_dbp.p_dbp = &p_filters->hiso_input_param.li2_dynamic_bpc;
            out_dbp.p_cr_11 = p_cr->step12.sec2.p_CR_buf_11;
            out_dbp.cr_11_size = CR_SIZE_11;
            out_dbp.p_cr_12 = p_cr->step12.sec2.p_CR_buf_12;
            out_dbp.cr_12_size = CR_SIZE_12;
            rval |= ikc_dynamic_bad_pixel(&in_dbp, &out_dbp);
        }
    }
    if(p_filters->update_flags.iso.stored_ir_updated == 1u) {
        ikc_in_stored_ir_t in_stored_ir;
        ikc_out_stored_ir_t out_stored_ir;

        if(p_filters->update_flags.cr.r2y.cr_12_update == 0U) {
            amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_stored_ir !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
            rval |= IK_ERR_0000;
        } else {
            in_stored_ir.p_stored_ir = &p_filters->input_param.stored_ir;
            out_stored_ir.p_cr_12 = p_cr->step12.sec2.p_CR_buf_12;
            out_stored_ir.cr_12_size = CR_SIZE_12;
            rval |= ikc_stored_ir(&in_stored_ir, &out_stored_ir);
        }
    }
    if(p_filters->update_flags.iso.li2_grgb_mismatch_updated == 1u) {
        ikc_in_grgb_mismatch_t in_grgb;
        ikc_out_grgb_mismatch_t out_grgb;

        if(p_filters->update_flags.cr.r2y.cr_23_update == 0U) {
            amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_grgb_mismatch !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
            rval |= IK_ERR_0000;
        } else {
            in_grgb.p_grgb_mismatch = &p_filters->hiso_input_param.li2_grgb_mismatch;
            out_grgb.p_cr_23 = p_cr->step12.sec2.p_CR_buf_23;
            out_grgb.cr_23_size = CR_SIZE_23;
            rval |= ikc_grgb_mismatch(&in_grgb, &out_grgb);
        }
    }
    if((p_filters->update_flags.iso.li2_cfa_leakage_filter_updated == 1u) || (p_filters->update_flags.iso.li2_anti_aliasing_updated == 1u)) {
        ikc_in_cfa_leak_anti_alias_t in_cfa_leak_anti_alias;
        ikc_out_cfa_leak_anti_alias_t out_cfa_leak_anti_alias;

        if(p_filters->update_flags.cr.r2y.cr_12_update == 0U) {
            amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_cfa_leakage_anti_aliasing !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
            rval |= IK_ERR_0000;
        } else {
            in_cfa_leak_anti_alias.p_cfa_leakage_filter = &p_filters->hiso_input_param.li2_cfa_leakage_filter;
            in_cfa_leak_anti_alias.p_anti_aliasing = &p_filters->hiso_input_param.li2_anti_aliasing;
            out_cfa_leak_anti_alias.p_cr_12 = p_cr->step12.sec2.p_CR_buf_12;
            out_cfa_leak_anti_alias.cr_12_size = CR_SIZE_12;
            rval |= ikc_cfa_leakage_anti_aliasing(&in_cfa_leak_anti_alias, &out_cfa_leak_anti_alias);
        }
    }
    if(p_filters->update_flags.iso.li2_cfa_noise_filter_updated == 1u) {
        ikc_in_cfa_noise_t in_cfa_noise;
        ikc_out_cfa_noise_t out_cfa_noise;

        if(p_filters->update_flags.cr.r2y.cr_23_update == 0U) {
            amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_cfa_noise !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
            rval |= IK_ERR_0000;
        } else {
            in_cfa_noise.p_cfa_noise_filter = &p_filters->hiso_input_param.li2_cfa_noise_filter;
            out_cfa_noise.p_cr_23 = p_cr->step12.sec2.p_CR_buf_23;
            out_cfa_noise.cr_23_size = CR_SIZE_23;
            rval |= ikc_cfa_noise(&in_cfa_noise, &out_cfa_noise);
        }
    }
    if(p_filters->update_flags.iso.after_ce_wb_gain_updated == 1u) {
        ikc_in_after_ce_gain_t in_after_ce_gain;
        ikc_out_after_ce_gain_t out_after_ce_gain;

        if(p_filters->update_flags.cr.r2y.cr_22_update == 0U) {
            amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_after_ce_gain !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
            rval |= IK_ERR_0000;
        } else {
            in_after_ce_gain.p_after_ce_wb_gain = &p_filters->input_param.after_ce_wb_gain;
            out_after_ce_gain.p_cr_22 = p_cr->step12.sec2.p_CR_buf_22;
            out_after_ce_gain.cr_22_size = CR_SIZE_22;
            rval |= ikc_after_ce_gain(&in_after_ce_gain, &out_after_ce_gain);
        }
    }
    if(p_filters->update_flags.iso.rgb_ir_updated == 1u) {
        ikc_in_rgb_ir_t in_rgb_ir;
        ikc_out_rgb_ir_t out_rgb_ir;

        if((p_filters->update_flags.cr.r2y.cr_4_update == 0U)||
           (p_filters->update_flags.cr.r2y.cr_6_update == 0U)||
           (p_filters->update_flags.cr.r2y.cr_7_update == 0U)||
           (p_filters->update_flags.cr.r2y.cr_8_update == 0U)||
           (p_filters->update_flags.cr.r2y.cr_9_update == 0U)||
           (p_filters->update_flags.cr.r2y.cr_11_update == 0U)||
           (p_filters->update_flags.cr.r2y.cr_12_update == 0U)||
           (p_filters->update_flags.cr.r2y.cr_21_update == 0U)) {
            amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_rgb_ir !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
            rval |= IK_ERR_0000;
        } else {
            in_rgb_ir.p_rgb_ir = &p_filters->input_param.rgb_ir;
            in_rgb_ir.p_before_ce_wb_gain = &p_filters->input_param.before_ce_wb_gain;
            in_rgb_ir.p_after_ce_wb_gain = &p_filters->input_param.after_ce_wb_gain;
            in_rgb_ir.p_exp0_frontend_wb_gain = &p_filters->input_param.exp0_frontend_wb_gain;
            out_rgb_ir.p_cr_4 = p_cr->step12.sec2.p_CR_buf_4;
            out_rgb_ir.cr_4_size = CR_SIZE_4;
            out_rgb_ir.p_cr_6 = p_cr->step12.sec2.p_CR_buf_6;
            out_rgb_ir.cr_6_size = CR_SIZE_6;
            out_rgb_ir.p_cr_7 = p_cr->step12.sec2.p_CR_buf_7;
            out_rgb_ir.cr_7_size = CR_SIZE_7;
            out_rgb_ir.p_cr_8 = p_cr->step12.sec2.p_CR_buf_8;
            out_rgb_ir.cr_8_size = CR_SIZE_8;
            out_rgb_ir.p_cr_9 = p_cr->step12.sec2.p_CR_buf_9;
            out_rgb_ir.cr_9_size = CR_SIZE_9;
            out_rgb_ir.p_cr_11 = p_cr->step12.sec2.p_CR_buf_11;
            out_rgb_ir.cr_11_size = CR_SIZE_11;
            out_rgb_ir.p_cr_12 = p_cr->step12.sec2.p_CR_buf_12;
            out_rgb_ir.cr_12_size = CR_SIZE_12;
            out_rgb_ir.p_cr_21 = p_cr->step12.sec2.p_CR_buf_21;
            out_rgb_ir.cr_21_size = CR_SIZE_21;
            out_rgb_ir.p_flow = &p_flow->flow_info;
            rval |= ikc_rgb_ir(&in_rgb_ir, &out_rgb_ir);
        }
    }
    if ((p_filters->update_flags.iso.resample_str_update== 1U) ||
        (p_filters->update_flags.iso.cawarp_enable_updated == 1U)||
        (window_calculate_updated == 1U)) {
        ikc_in_cfa_prescale_t in_cfa_prescale;
        ikc_out_cfa_prescale_t out_cfa_prescale;

        if(p_filters->update_flags.cr.r2y.cr_16_update == 0U) {
            amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_cfa_prescale !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
            rval |= IK_ERR_0000;
        } else {
            in_cfa_prescale.p_flow = &p_flow->flow_info;
            in_cfa_prescale.cfa_cut_off_freq = p_filters->input_param.resample_str.cfa_cut_off_freq;
            in_cfa_prescale.p_dmy_win_geo = &p_filters->input_param.ctx_buf.result_win.dmy_win_geo;
            in_cfa_prescale.p_cfa_win_dim = &p_filters->input_param.ctx_buf.result_win.cfa_win_dim;
            in_cfa_prescale.cawarp_enable = (uint32)p_filters->input_param.cawarp_enable;
            out_cfa_prescale.p_cr_16 = p_cr->step12.sec2.p_CR_buf_16;
            out_cfa_prescale.cr_16_size = CR_SIZE_16;
            rval |= ikc_cfa_prescale(&in_cfa_prescale, &out_cfa_prescale);
        }
    }
    if((p_filters->update_flags.iso.aaa_stat_info_updated == 1u) ||
       (p_filters->update_flags.iso.af_stat_ex_info_updated == 1u) ||
       (p_filters->update_flags.iso.pg_af_stat_ex_info_updated == 1u) ||
       (p_filters->update_flags.iso.histogram_info_update == 1u) ||
       (p_filters->update_flags.iso.histogram_info_pg_update == 1u) ||
       (p_filters->update_flags.iso.window_size_info_updated == 1U)||
       (window_calculate_updated == 1U)) {
        ikc_in_aaa_t in_aaa;
        ikc_out_aaa_t out_aaa;

        if((p_filters->update_flags.cr.aaa_data == 0U)||
           (p_filters->update_flags.cr.r2y.cr_9_update == 0U)||
           (p_filters->update_flags.cr.r2y.cr_12_update == 0U)||
           (p_filters->update_flags.cr.r2y.cr_21_update == 0U)||
           (p_filters->update_flags.cr.r2y.cr_28_update == 0U)) {
            amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_aaa !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
            rval |= IK_ERR_0000;
        } else {
            in_aaa.p_sensor_info = &p_filters->input_param.sensor_info;//determine RGB-IR.
            in_aaa.p_rgb_ir = &p_filters->input_param.rgb_ir;
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
            out_aaa.p_cr_9 = p_cr->step12.sec2.p_CR_buf_9;
            out_aaa.cr_9_size = CR_SIZE_9;
            out_aaa.p_cr_12 = p_cr->step12.sec2.p_CR_buf_12;
            out_aaa.cr_12_size = CR_SIZE_12;
            out_aaa.p_cr_21 = p_cr->step12.sec2.p_CR_buf_21;
            out_aaa.cr_21_size = CR_SIZE_21;
            out_aaa.p_cr_28 = p_cr->step12.sec2.p_CR_buf_28;
            out_aaa.cr_28_size = CR_SIZE_28;
            out_aaa.p_stitching_aaa = p_cr->p_aaa;

            if (p_filters->input_param.ability == AMBA_IK_VIDEO_PIPE_Y2Y) {
                in_aaa.is_yuv_mode = 1U;
            } else {
                in_aaa.is_yuv_mode = 0U;
            }
            rval |= ikc_aaa(&in_aaa, &out_aaa);
        }
    }

    // exe_process_rgb_cr
    if(p_filters->update_flags.iso.li2_demosaic_updated == 1U) {
        ikc_in_demosaic_t in_demosaic;
        ikc_out_demosaic_t out_demosaic;

        if((p_filters->update_flags.cr.r2y.cr_12_update == 0U)||
           (p_filters->update_flags.cr.r2y.cr_23_update == 0U)||
           (p_filters->update_flags.cr.r2y.cr_24_update == 0U)) {
            amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_demosaic_filter !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
            rval |= IK_ERR_0000;
        } else {
            in_demosaic.p_demosaic = &p_filters->hiso_input_param.li2_demosaic;
            out_demosaic.p_cr_12 = p_cr->step12.sec2.p_CR_buf_12;
            out_demosaic.cr_12_size = CR_SIZE_12;
            out_demosaic.p_cr_23 = p_cr->step12.sec2.p_CR_buf_23;
            out_demosaic.cr_23_size = CR_SIZE_23;
            out_demosaic.p_cr_24 = p_cr->step12.sec2.p_CR_buf_24;
            out_demosaic.cr_24_size = CR_SIZE_24;
            rval |= ikc_demosaic_filter(&in_demosaic, &out_demosaic);
        }
    }
    if(p_filters->update_flags.iso.rgb_to_12y_updated == 1U) {
        ikc_in_rgb12y_t in_rgb12y;
        ikc_out_rgb12y_t out_rgb12y;

        if(p_filters->update_flags.cr.r2y.cr_25_update == 0U) {
            amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_rgb12y !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
            rval |= IK_ERR_0000;
        } else {
            in_rgb12y.p_rgb_to_12y = &p_filters->input_param.rgb_to_12y;
            out_rgb12y.p_cr_25 = p_cr->step12.sec2.p_CR_buf_25;
            out_rgb12y.cr_25_size = CR_SIZE_25;
            rval |= ikc_rgb12y(&in_rgb12y, &out_rgb12y);
        }
    }
    if(p_filters->update_flags.iso.is_1st_frame == 1U) {//should only configure once in 1st time, due to R2Y always turn on.
        ikc_in_cc_en_t in_cc_enb;
        ikc_out_cc_en_t out_cc_enb;

        if(p_filters->update_flags.cr.r2y.cr_26_update == 0U) {
            amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_color_correction_enable !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
            rval |= IK_ERR_0000;
        } else {
            in_cc_enb.is_yuv_mode = 0;
            in_cc_enb.use_cc_for_yuv2yuv = 0;
            out_cc_enb.p_cr_26 = p_cr->step12.sec2.p_CR_buf_26;
            out_cc_enb.cr_26_size = CR_SIZE_26;
            rval |= ikc_color_correction_enable(&in_cc_enb, &out_cc_enb);
        }
    }
    if(p_filters->update_flags.iso.color_correction_updated == 1U) {
        ikc_in_cc_t in_cc_3d;
        ikc_out_cc_t out_cc_3d;

        if((p_filters->update_flags.cr.r2y.cr_26_update == 0U)||
           (p_filters->update_flags.cr.r2y.cr_27_update == 0U)) {
            amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_color_correction !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
            rval |= IK_ERR_0000;
        } else {
            in_cc_3d.p_color_correction = &p_filters->input_param.color_correction;
            out_cc_3d.p_cr_26 = p_cr->step12.sec2.p_CR_buf_26;
            out_cc_3d.cr_26_size = CR_SIZE_26;
            out_cc_3d.p_cr_27 = p_cr->step12.sec2.p_CR_buf_27;
            out_cc_3d.cr_27_size = CR_SIZE_27;
            rval |= ikc_color_correction(&in_cc_3d, &out_cc_3d);
        }
    }
    if(p_filters->update_flags.iso.tone_curve_updated == 1U) {
        ikc_in_cc_out_t in_cc_out;
        ikc_out_cc_out_t out_cc_out;

        if(p_filters->update_flags.cr.r2y.cr_26_update == 0U) {
            amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_color_correction_out !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
            rval |= IK_ERR_0000;
        } else {
            in_cc_out.p_tone_curve = &p_filters->input_param.tone_curve;
            out_cc_out.p_cr_26 = p_cr->step12.sec2.p_CR_buf_26;
            out_cc_out.cr_26_size = CR_SIZE_26;
            rval |= ikc_color_correction_out(&in_cc_out, &out_cc_out);
        }
    }
    if((p_filters->update_flags.iso.rgb_to_yuv_matrix_updated == 1U) ||
       (p_filters->update_flags.iso.is_1st_frame == 1U)) {
        ikc_in_rgb2yuv_t in_rgb2yuv={0};
        ikc_out_rgb2yuv_t out_rgb2yuv;

        if(p_filters->update_flags.cr.r2y.cr_29_update == 0U) {
            amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_rgb2yuv !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
            rval |= IK_ERR_0000;
        } else {
            in_rgb2yuv.p_rgb_to_yuv_matrix = &p_filters->input_param.rgb_to_yuv_matrix;
            out_rgb2yuv.p_cr_29 = p_cr->step12.sec2.p_CR_buf_29;
            out_rgb2yuv.cr_29_size = CR_SIZE_29;
            rval |= ikc_rgb2yuv(&in_rgb2yuv, &out_rgb2yuv);
        }
    }

    // exe_process_yuv_cr
    if(p_filters->update_flags.iso.chroma_scale_updated == 1U) {
        ikc_in_chroma_scale_t in_chroma_scale;
        ikc_out_chroma_scale_t out_chroma_scale;

        if(p_filters->update_flags.cr.r2y.cr_29_update == 0U) {
            amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_chroma_scale !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
            rval |= IK_ERR_0000;
        } else {
            if (p_filters->input_param.ability == AMBA_IK_VIDEO_PIPE_Y2Y) {
                in_chroma_scale.is_yuv_mode = 1U;
            } else {
                in_chroma_scale.is_yuv_mode = 0U;
            }
            in_chroma_scale.p_chroma_scale = &p_filters->input_param.chroma_scale;
            out_chroma_scale.p_cr_29 = p_cr->step12.sec2.p_CR_buf_29;
            out_chroma_scale.cr_29_size = CR_SIZE_29;
            rval |= ikc_chroma_scale(&in_chroma_scale, &out_chroma_scale);
        }
    }
    if((p_filters->update_flags.iso.chroma_filter_updated == 1U) || (p_filters->update_flags.iso.li2_wide_chroma_filter == 1U) ||
       (p_filters->update_flags.iso.li2_wide_chroma_filter_combine== 1U) || (window_calculate_updated == 1U)) {
        ikc_in_chroma_noise_t in_chroma_noise;
        ikc_out_chroma_noise_t out_chroma_noise;
        uint64 sec2_luma_hscale_phase_inc = p_filters->input_param.ctx_buf.result_win.warp_phase.sec2_hscale_phase_inc;
        uint64 sec2_luma_vscale_phase_inc = p_filters->input_param.ctx_buf.result_win.warp_phase.sec2_vscale_phase_inc;
        uint32 chroma_radius = p_filters->input_param.chroma_filter.radius;
        uint64 sec2_chroma_hscale_phase_inc;
        uint64 sec2_chroma_vscale_phase_inc;

        if(p_filters->update_flags.cr.r2y.cr_36_update == 0U) {
            amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_chroma_noise !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
            rval |= IK_ERR_0000;
        } else {
            in_chroma_noise.p_chroma_filter = &p_filters->input_param.chroma_filter;
            in_chroma_noise.p_wide_chroma_filter = &p_filters->hiso_input_param.li2_wide_chroma_filter;
            in_chroma_noise.p_wide_chroma_combine = &p_filters->hiso_input_param.li2_wide_chroma_filter_combine;
            if ((in_chroma_noise.p_chroma_filter->radius==32u) && (in_chroma_noise.p_wide_chroma_filter->enable==1u)) {
                in_chroma_noise.is_wide_chroma_invalid = 1u;
            } else {
                in_chroma_noise.is_wide_chroma_invalid = 0u;
            }
            out_chroma_noise.p_cr_36 = p_cr->step12.sec2.p_CR_buf_36;
            out_chroma_noise.cr_36_size = CR_SIZE_36;
            out_chroma_noise.p_window = &p_flow->window;
            rval |= ikc_chroma_noise(&in_chroma_noise, &out_chroma_noise);
            p_flow->window.update_chroma_radius = 1U;
            //check radius causes phase_inc overflow.
            sec2_chroma_hscale_phase_inc = (sec2_luma_hscale_phase_inc * (uint64)chroma_radius) >> 5UL;
            sec2_chroma_vscale_phase_inc = (sec2_luma_vscale_phase_inc * (uint64)chroma_radius) >> 5UL;
            if ((sec2_chroma_hscale_phase_inc >= (8UL<<13UL)) || (sec2_chroma_vscale_phase_inc >= (8UL<<13UL))) {
                amba_ik_system_print_uint32_5("[IK][ERROR] exe_iso_cfg_check_raw2yuv_chroma_filter() fail. One or more downscaling_factor is out of range(>=8<<13), please check chroma radius or window setting!", DC_U, DC_U, DC_U, DC_U, DC_U);
                amba_ik_system_print_uint32_5("[IK][ERROR] sec2_chroma_hscale_phase_inc:%d, sec2_chroma_vscale_phase_inc:%d, chroma_radius:%d", (uint32)sec2_chroma_hscale_phase_inc, (uint32)sec2_chroma_vscale_phase_inc, chroma_radius, DC_U, DC_U);
                rval = IK_ERR_0101;
            }
        }
    } else {
        p_flow->window.update_chroma_radius = 0U;
    }

    if((p_filters->update_flags.iso.chroma_filter_updated == 1U)||
       (p_filters->update_flags.iso.resample_str_update == 1U)||
       (p_filters->update_flags.iso.window_size_info_updated == 1U)||
       (window_calculate_updated == 1U)) {
        if((p_filters->update_flags.cr.r2y.cr_33_update == 0U)||
           (p_filters->update_flags.cr.r2y.cr_35_update == 0U)||
           (p_filters->update_flags.cr.r2y.cr_37_update == 0U)||
           (p_filters->update_flags.cr.r2y.cr_42_update == 0U)) {
            amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_main_resampler !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
            rval |= IK_ERR_0000;
        } else {
            {
                ikc_in_sec2_main_resampler_t in_sec2_main_resampler;
                ikc_out_sec2_main_resampler_t out_sec2_main_resampler;

                in_sec2_main_resampler.main_cut_off_freq = p_filters->input_param.resample_str.main_cut_off_freq;
                in_sec2_main_resampler.p_act_win_crop = &p_filters->input_param.ctx_buf.result_win.act_win_crop;
                in_sec2_main_resampler.p_dmy_win_geo = &p_filters->input_param.ctx_buf.result_win.dmy_win_geo;
                in_sec2_main_resampler.p_cfa_win_dim = &p_filters->input_param.ctx_buf.result_win.cfa_win_dim;
                in_sec2_main_resampler.p_main = &p_filters->input_param.window_size_info.main_win;
                in_sec2_main_resampler.p_dmy_range = &p_filters->input_param.dmy_range;
                in_sec2_main_resampler.chroma_radius = 32;//p_filters->input_param.chroma_filter.radius;
                in_sec2_main_resampler.is_y2y_420input = (p_filters->input_param.yuv_mode == AMBA_IK_YUV420) ? 1U : 0U;
                in_sec2_main_resampler.flip_mode = p_filters->input_param.flip_mode;
                in_sec2_main_resampler.luma_dma_size = p_filters->input_param.warp_buf_info.luma_dma_size;
                out_sec2_main_resampler.p_cr_33 = p_cr->step12.sec2.p_CR_buf_33;
                out_sec2_main_resampler.cr_33_size = CR_SIZE_33;
                out_sec2_main_resampler.p_cr_35 = p_cr->step12.sec2.p_CR_buf_35;
                out_sec2_main_resampler.cr_35_size = CR_SIZE_35;
                out_sec2_main_resampler.p_cr_37 = p_cr->step12.sec2.p_CR_buf_37;
                out_sec2_main_resampler.cr_37_size = CR_SIZE_37;
                rval |= ikc_sec2_main_resampler(&in_sec2_main_resampler, &out_sec2_main_resampler);
            }
        }
    }
    if (p_filters->update_flags.iso.li2_asf_updated == 1U) {
        ikc_in_asf_t  in_asf;
        ikc_out_asf_t out_asf;

        if(p_filters->update_flags.cr.r2y.cr_30_update == 0U) {
            amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on step12 ikc_asf !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
            rval |= IK_ERR_0000;
        } else {
            in_asf.p_advance_spatial_filter           = &p_filters->hiso_input_param.li2_asf;
            in_asf.first_luma_processing_mode_updated = (uint32)p_filters->update_flags.iso.first_luma_processing_mode_updated;
            out_asf.p_cr_30    = p_cr->step12.sec2.p_CR_buf_30;
            out_asf.cr_30_size = CR_SIZE_30;
            rval |= ikc_asf(&in_asf, &out_asf);
        }
    }

    if(((p_filters->update_flags.iso.after_ce_wb_gain_updated == 1U)&&(p_filters->input_param.ability != AMBA_IK_VIDEO_PIPE_Y2Y)) ||
       ((p_filters->update_flags.iso.rgb_to_12y_updated == 1U)&&(p_filters->input_param.ability != AMBA_IK_VIDEO_PIPE_Y2Y))||
       (p_filters->update_flags.iso.lnl_updated == 1U)||
       (window_calculate_updated == 1U)) {
        ikc_in_lnl_t in_lnl;
        ikc_out_lnl_t out_lnl;

        if(p_filters->update_flags.cr.r2y.cr_30_update == 0U) {
            amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on Hiso step12 ikc_lnl !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
            rval |= IK_ERR_0000;
        } else {
            if(p_filters->input_param.li2_luma_noise_reduce.enable == 1U) {
                if(p_filters->hiso_input_param.li2_asf.enable != 1U) {
                    amba_ik_system_print_uint32_5("[IK] [ERROR] IF [li2_color_dependent_luma_noise_reduction.enable] = 1, then li2_advanced_spatial_filter.enable (=%d) should be enabled !!!",p_filters->hiso_input_param.li2_asf.enable,DC_U,DC_U,DC_U,DC_U);
                }
            }
            in_lnl.p_luma_noise_reduce = &p_filters->input_param.li2_luma_noise_reduce;
            in_lnl.p_after_ce_wb_gain = &p_filters->input_param.after_ce_wb_gain;
            in_lnl.p_rgb_to_12y = &p_filters->input_param.rgb_to_12y;
            in_lnl.p_cfa_win = &p_filters->input_param.ctx_buf.result_win.cfa_win_dim;
            in_lnl.use_sharpen_not_asf = 0;
            in_lnl.p_advance_spatial_filter = &p_filters->hiso_input_param.li2_asf;
            out_lnl.p_cr_30 = p_cr->step12.sec2.p_CR_buf_30;
            out_lnl.cr_30_size = CR_SIZE_30;
            out_lnl.p_flow_info = &p_flow->flow_info;
            rval |= ikc_lnl(&in_lnl, &out_lnl);
        }
    }
    if((p_filters->update_flags.iso.color_correction_updated == 1U)||
       (p_filters->update_flags.iso.tone_curve_updated == 1U)) {
        ikc_in_lnl_tbl_t in_lnl_tbl;
        ikc_out_lnl_t out_lnl;

        if(p_filters->update_flags.cr.r2y.cr_30_update == 0U) {
            amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_lnl_tbl !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
            rval |= IK_ERR_0000;
        } else {
            in_lnl_tbl.p_color_correction = &p_filters->input_param.color_correction;
            in_lnl_tbl.p_tone_curve = &p_filters->input_param.tone_curve;
            out_lnl.p_cr_30 = p_cr->step12.sec2.p_CR_buf_30;
            out_lnl.cr_30_size = CR_SIZE_30;
            rval |= ikc_lnl_tbl(&in_lnl_tbl, &out_lnl);
        }
    }
    // exe_process_hdr_ce_sub_cr
    if((p_filters->update_flags.iso.is_1st_frame == 1U) && (p_filters->update_flags.iso.sensor_information_updated == 1u)) { //should only configure once at 1st time.
        ikc_in_input_mode_sub_t in_input_mode_sub;
        ikc_out_input_mode_sub_t out_input_mode_sub;

        if((p_filters->update_flags.cr.r2y.cr_45_update == 0U)||
           (p_filters->update_flags.cr.r2y.cr_47_update == 0U)||
           (p_filters->update_flags.cr.r2y.cr_49_update == 0U)||
           (p_filters->update_flags.cr.r2y.cr_50_update == 0U)||
           (p_filters->update_flags.cr.r2y.cr_51_update == 0U)) {
            amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_input_mode_sub !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
            rval |= IK_ERR_0000;
        } else {
            in_input_mode_sub.flip_mode = p_filters->input_param.flip_mode;
            in_input_mode_sub.is_yuv_mode = 0u;
            in_input_mode_sub.sensor_pattern = p_filters->input_param.sensor_info.sensor_pattern;
            out_input_mode_sub.p_cr_45 = p_cr->step12.sec4.p_CR_buf_45;
            out_input_mode_sub.cr_45_size = CR_SIZE_45;
            out_input_mode_sub.p_cr_47 = p_cr->step12.sec4.p_CR_buf_47;
            out_input_mode_sub.cr_47_size = CR_SIZE_47;
            out_input_mode_sub.p_cr_49 = p_cr->step12.sec4.p_CR_buf_49;
            out_input_mode_sub.cr_49_size = CR_SIZE_49;
            out_input_mode_sub.p_cr_50 = p_cr->step12.sec4.p_CR_buf_50;
            out_input_mode_sub.cr_50_size = CR_SIZE_50;
            out_input_mode_sub.p_cr_51 = p_cr->step12.sec4.p_CR_buf_51;
            out_input_mode_sub.cr_51_size = CR_SIZE_51;

            rval |= ikc_input_mode_sub(&in_input_mode_sub, &out_input_mode_sub);
        }
    }
    if(p_filters->update_flags.iso.before_ce_wb_gain_updated == 1u) {
        ikc_in_before_ce_gain_t in_before_ce_gain;
        ikc_out_before_ce_gain_sub_t out_before_ce_gain_sub;

        if(p_filters->update_flags.cr.r2y.cr_51_update == 0U) {
            amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_before_ce_gain_sub !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
            rval |= IK_ERR_0000;
        } else {
            in_before_ce_gain.p_before_ce_wb_gain = &p_filters->input_param.before_ce_wb_gain;
            out_before_ce_gain_sub.p_cr_51 = p_cr->step12.sec4.p_CR_buf_51;
            out_before_ce_gain_sub.cr_51_size = CR_SIZE_51;
            rval |= ikc_before_ce_gain_sub(&in_before_ce_gain, &out_before_ce_gain_sub);
        }
    }
    if(p_filters->update_flags.iso.hdr_blend_updated == 1u) {
        ikc_in_hdr_blend_t in_hdr_blend;
        ikc_out_hdr_blend_sub_t out_hdr_blend_sub;

        if(p_filters->update_flags.cr.r2y.cr_45_update == 0U) {
            amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_hdr_blend_sub !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
            rval |= IK_ERR_0000;
        } else {
            in_hdr_blend.exp_num = p_filters->input_param.num_of_exposures;
            in_hdr_blend.p_hdr_raw_info = &p_filters->input_param.hdr_raw_info;
            in_hdr_blend.p_hdr_blend = &p_filters->input_param.hdr_blend;
            out_hdr_blend_sub.p_cr_45 = p_cr->step12.sec4.p_CR_buf_45;
            out_hdr_blend_sub.cr_45_size = CR_SIZE_45;
            rval |= ikc_hdr_blend_sub(&in_hdr_blend, &out_hdr_blend_sub);
        }
    }
    if(p_filters->update_flags.iso.fe_tone_curve_updated == 1u) {
        ikc_in_hdr_tone_curve_t in_hdr_tc;
        ikc_out_hdr_tone_curve_sub_t out_hdr_tc_sub;

        if((p_filters->update_flags.cr.r2y.cr_45_update == 0U)||
           (p_filters->update_flags.cr.r2y.cr_46_update == 0U)) {
            amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_front_end_tone_curve_sub !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
            rval |= IK_ERR_0000;
        } else {
            in_hdr_tc.p_fe_tone_curve = &p_filters->input_param.fe_tone_curve;
            out_hdr_tc_sub.p_cr_45 = p_cr->step12.sec4.p_CR_buf_45;
            out_hdr_tc_sub.cr_45_size = CR_SIZE_45;
            out_hdr_tc_sub.p_cr_46 = p_cr->step12.sec4.p_CR_buf_46;
            out_hdr_tc_sub.cr_46_size = CR_SIZE_46;
            rval |= ikc_front_end_tone_curve_sub(&in_hdr_tc, &out_hdr_tc_sub);
        }
    }
    if((p_filters->update_flags.iso.sensor_information_updated == 1u) || (window_calculate_updated == 1U)) {
        ikc_in_decompress_t in_decompress;
        ikc_out_decompress_sub_t out_decompress_sub;

        if(p_filters->update_flags.cr.r2y.cr_45_update == 0U) {
            amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_decompression_sub !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
            rval |= IK_ERR_0000;
        } else {
            in_decompress.exp_num = p_filters->input_param.num_of_exposures;
            in_decompress.compress_mode = p_filters->input_param.sensor_info.compression;//0~8, 256~259.
            in_decompress.p_dmy_win_geo = &p_filters->input_param.ctx_buf.result_win.dmy_win_geo;
            out_decompress_sub.p_cr_45 = p_cr->step12.sec4.p_CR_buf_45;
            out_decompress_sub.cr_45_size = CR_SIZE_45;
            rval |= ikc_decompression_sub(&in_decompress, &out_decompress_sub);
        }
    }
    if((p_filters->update_flags.iso.exp0_fe_static_blc_updated == 1u) ||
       (p_filters->update_flags.iso.exp1_fe_static_blc_updated == 1u) ||
       (p_filters->update_flags.iso.exp2_fe_static_blc_updated == 1u)) {
        ikc_in_hdr_blc_t in_hdr_blc;
        ikc_out_hdr_blc_sub_t out_hdr_blc_sub;

        if(p_filters->update_flags.cr.r2y.cr_45_update == 0U) {
            amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_hdr_black_level_sub !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
            rval |= IK_ERR_0000;
        } else {
            in_hdr_blc.exp_num = p_filters->input_param.num_of_exposures;
            in_hdr_blc.rgb_ir_mode = p_filters->input_param.rgb_ir.mode;
            in_hdr_blc.compression_offset = ((p_filters->input_param.sensor_info.compression > IK_RAW_COMPRESS_OFF)&&(p_filters->input_param.sensor_info.compression <= IK_RAW_COMPRESS_10p5)) ? p_filters->input_param.ctx_buf.first_compression_offset : 0;
            in_hdr_blc.p_exp0_frontend_static_blc = &p_filters->input_param.exp0_frontend_static_blc;
            in_hdr_blc.p_exp1_frontend_static_blc = &p_filters->input_param.exp1_frontend_static_blc;
            in_hdr_blc.p_exp2_frontend_static_blc = &p_filters->input_param.exp2_frontend_static_blc;
            out_hdr_blc_sub.p_cr_45 = p_cr->step12.sec4.p_CR_buf_45;
            out_hdr_blc_sub.cr_45_size = CR_SIZE_45;
            rval |= ikc_hdr_black_level_sub(&in_hdr_blc, &out_hdr_blc_sub);
        }
    }
    if((p_filters->update_flags.iso.exp0_fe_wb_gain_updated == 1u) ||
       (p_filters->update_flags.iso.exp1_fe_wb_gain_updated == 1u) ||
       (p_filters->update_flags.iso.exp2_fe_wb_gain_updated == 1u)) {
        ikc_in_hdr_dgain_t in_hdr_dgain = {0};
        ikc_out_hdr_dgain_sub_t out_hdr_dgain_sub;

        if(p_filters->update_flags.cr.r2y.cr_45_update == 0U) {
            amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_hdr_dgain_sub !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
            rval |= IK_ERR_0000;
        } else {
            in_hdr_dgain.exp_num = p_filters->input_param.num_of_exposures;
            in_hdr_dgain.rgb_ir_mode = p_filters->input_param.rgb_ir.mode;
            in_hdr_dgain.p_exp0_frontend_wb_gain = &p_filters->input_param.exp0_frontend_wb_gain;
            in_hdr_dgain.p_exp1_frontend_wb_gain = &p_filters->input_param.exp1_frontend_wb_gain;
            in_hdr_dgain.p_exp2_frontend_wb_gain = &p_filters->input_param.exp2_frontend_wb_gain;
            in_hdr_dgain.p_exp0_frontend_static_blc = &p_filters->input_param.exp0_frontend_static_blc;
            in_hdr_dgain.p_exp1_frontend_static_blc = &p_filters->input_param.exp1_frontend_static_blc;
            in_hdr_dgain.p_exp2_frontend_static_blc = &p_filters->input_param.exp2_frontend_static_blc;
            out_hdr_dgain_sub.p_cr_45 = p_cr->step12.sec4.p_CR_buf_45;
            out_hdr_dgain_sub.cr_45_size = CR_SIZE_45;
            rval |= ikc_hdr_dgain_sub(&in_hdr_dgain, &out_hdr_dgain_sub);
        }
    }
    if((p_filters->update_flags.iso.ce_updated == 1u) || (window_calculate_updated == 1U)) {
        ikc_in_ce_t in_ce;
        ikc_out_ce_sub_t out_ce_sub;

        if((p_filters->update_flags.cr.r2y.cr_49_update == 0U)||
           (p_filters->update_flags.cr.r2y.cr_50_update == 0U)||
           (p_filters->update_flags.cr.r2y.cr_51_update == 0U)) {
            amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_contrast_enhancement_sub !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
            rval |= IK_ERR_0000;
        } else {
            in_ce.sensor_pattern = p_filters->input_param.sensor_info.sensor_pattern;
            in_ce.p_dmy_win_geo = &p_filters->input_param.ctx_buf.result_win.dmy_win_geo;
            in_ce.p_ce = &p_filters->input_param.ce;
            out_ce_sub.p_cr_49 = p_cr->step12.sec4.p_CR_buf_49;
            out_ce_sub.cr_49_size = CR_SIZE_49;
            out_ce_sub.p_cr_50 = p_cr->step12.sec4.p_CR_buf_50;
            out_ce_sub.cr_50_size = CR_SIZE_50;
            out_ce_sub.p_cr_51 = p_cr->step12.sec4.p_CR_buf_51;
            out_ce_sub.cr_51_size = CR_SIZE_51;
            rval |= ikc_contrast_enhancement_sub(&in_ce, &out_ce_sub);
        }
    }
    if(p_filters->update_flags.iso.ce_input_table_updated == 1u) {
        ikc_in_ce_input_sub_t in_ce_input_sub;
        ikc_out_ce_input_sub_t out_ce_input_sub;

        if((p_filters->update_flags.cr.r2y.cr_51_update == 0U)||
           (p_filters->update_flags.cr.r2y.cr_52_update == 0U)) {
            amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_contrast_enhancement_input_sub !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
            rval |= IK_ERR_0000;
        } else {
            in_ce_input_sub.ce_enable = p_filters->input_param.ce.enable;
            in_ce_input_sub.radius = p_filters->input_param.ce.radius;
            in_ce_input_sub.epsilon = p_filters->input_param.ce.epsilon;
            in_ce_input_sub.p_ce_input_table = &p_filters->input_param.ce_input_table;
            out_ce_input_sub.p_cr_51 = p_cr->step12.sec4.p_CR_buf_51;
            out_ce_input_sub.cr_51_size = CR_SIZE_51;
            out_ce_input_sub.p_cr_52 = p_cr->step12.sec4.p_CR_buf_52;
            out_ce_input_sub.cr_52_size = CR_SIZE_52;
            rval |= ikc_contrast_enhancement_input_sub(&in_ce_input_sub, &out_ce_input_sub);
        }
    }
    if ((p_filters->update_flags.iso.vignette_compensation_updated == 1u) || (p_filters->update_flags.iso.window_size_info_updated == 1U) ||
        (window_calculate_updated == 1U)) {
        ikc_in_vignette_t in_vig;
        ikc_out_vignette_sub_t out_vig_sub;

        if((p_filters->update_flags.cr.r2y.cr_47_update == 0U)||
           (p_filters->update_flags.cr.r2y.cr_48_update == 0U)||
           (p_filters->update_flags.cr.r2y.cr_49_update == 0U)) {
            amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_vignette_sub !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
            rval |= IK_ERR_0000;
        } else {
            in_vig.vig_enable = p_filters->input_param.vig_enable;
            in_vig.flip_mode = p_filters->input_param.flip_mode;
            in_vig.p_vignette_compensation = &p_filters->input_param.vignette_compensation;
            in_vig.p_vin_sensor = &p_filters->input_param.window_size_info.vin_sensor;
            in_vig.p_dmy_win_geo = &p_filters->input_param.ctx_buf.result_win.dmy_win_geo;
            in_vig.p_active_window = &p_filters->input_param.active_window;
            out_vig_sub.p_cr_47 = p_cr->step12.sec4.p_CR_buf_47;
            out_vig_sub.cr_47_size = CR_SIZE_47;
            out_vig_sub.p_cr_48 = p_cr->step12.sec4.p_CR_buf_48;
            out_vig_sub.cr_48_size = CR_SIZE_48;
            out_vig_sub.p_cr_49 = p_cr->step12.sec4.p_CR_buf_49;
            out_vig_sub.cr_49_size = CR_SIZE_49;
            rval |= ikc_vignette_sub(&in_vig, &out_vig_sub);
        }
    }
    if(p_filters->update_flags.iso.rgb_ir_updated == 1u) {
        ikc_in_rgb_ir_t in_rgb_ir;
        ikc_out_rgb_ir_sub_t out_rgb_ir_sub;

        if((p_filters->update_flags.cr.r2y.cr_45_update == 0U)||
           (p_filters->update_flags.cr.r2y.cr_47_update == 0U)||
           (p_filters->update_flags.cr.r2y.cr_49_update == 0U)||
           (p_filters->update_flags.cr.r2y.cr_50_update == 0U)) {
            amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_rgb_ir_sub !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
            rval |= IK_ERR_0000;
        } else {
            in_rgb_ir.p_rgb_ir = &p_filters->input_param.rgb_ir;
            in_rgb_ir.p_before_ce_wb_gain = &p_filters->input_param.before_ce_wb_gain;
            in_rgb_ir.p_after_ce_wb_gain = &p_filters->input_param.after_ce_wb_gain;
            in_rgb_ir.p_exp0_frontend_wb_gain = &p_filters->input_param.exp0_frontend_wb_gain;
            out_rgb_ir_sub.p_cr_45 = p_cr->step12.sec4.p_CR_buf_45;
            out_rgb_ir_sub.cr_45_size = CR_SIZE_45;
            out_rgb_ir_sub.p_cr_47 = p_cr->step12.sec4.p_CR_buf_47;
            out_rgb_ir_sub.cr_47_size = CR_SIZE_47;
            out_rgb_ir_sub.p_cr_49 = p_cr->step12.sec4.p_CR_buf_49;
            out_rgb_ir_sub.cr_49_size = CR_SIZE_49;
            out_rgb_ir_sub.p_cr_50 = p_cr->step12.sec4.p_CR_buf_50;
            out_rgb_ir_sub.cr_50_size = CR_SIZE_50;
            rval |= ikc_rgb_ir_sub(&in_rgb_ir, &out_rgb_ir_sub);
        }
    }

//    if(p_filters->hiso_input_param.li2_shpns_both.enable == 1U)
    {
        li2_shp_t Li2Shp;
        ikc_in_sec18_step12_shp_mcts_t in_sec18_step12_shp_mcts;
        ikc_out_sec18_sharpen_t        out_sharpen;
        out_sharpen.p_cr_117= p_cr->step12.sec18.p_CR_buf_117;
        out_sharpen.cr_117_size = CR_SIZE_117;

        (void)amba_ik_system_memcpy(&Li2Shp.li2_shpns_both, &p_filters->hiso_input_param.li2_shpns_both, sizeof(ik_li2_shpns_both_t));
        (void)amba_ik_system_memcpy(&Li2Shp.li2_shpns_coring, &p_filters->hiso_input_param.li2_shpns_coring, sizeof(ik_li2_shpns_coring_t));
        (void)amba_ik_system_memcpy(&Li2Shp.li2_shpns_cor_idx_scl, &p_filters->hiso_input_param.li2_shpns_cor_idx_scl, sizeof(ik_li2_shpns_cor_idx_scl_t));
        (void)amba_ik_system_memcpy(&Li2Shp.li2_shpns_min_cor_rst, &p_filters->hiso_input_param.li2_shpns_min_cor_rst, sizeof(ik_li2_shpns_min_cor_rst_t));
        (void)amba_ik_system_memcpy(&Li2Shp.li2_shpns_max_cor_rst, &p_filters->hiso_input_param.li2_shpns_max_cor_rst, sizeof(ik_li2_shpns_max_cor_rst_t));
        (void)amba_ik_system_memcpy(&Li2Shp.li2_shpns_scl_cor, &p_filters->hiso_input_param.li2_shpns_scl_cor, sizeof(ik_li2_shpns_scl_cor_t));


        if (p_filters->hiso_input_param.li2_shpns_both.enable == 0U) { //ref: must be enabled but set to passthrousrgh by making FIRs 0
            Li2Shp.li2_shpns_both.enable = 1U;
            (void)amba_ik_system_memset(&Li2Shp.li2_shpns_noise, 0x0, sizeof(ik_li2_shpns_noise_t));
            (void)amba_ik_system_memset(&Li2Shp.li2_shpns_fir, 0x0, sizeof(ik_li2_shpns_fir_t));
        } else {
            (void)amba_ik_system_memcpy(&Li2Shp.li2_shpns_noise, &p_filters->hiso_input_param.li2_shpns_noise, sizeof(ik_li2_shpns_noise_t));
            (void)amba_ik_system_memcpy(&Li2Shp.li2_shpns_fir, &p_filters->hiso_input_param.li2_shpns_fir, sizeof(ik_li2_shpns_fir_t));
        }

        in_sec18_step12_shp_mcts.p_li2_shpns_both        = &Li2Shp.li2_shpns_both;
        in_sec18_step12_shp_mcts.p_li2_shpns_noise       = &Li2Shp.li2_shpns_noise;
        in_sec18_step12_shp_mcts.p_li2_shpns_coring      = &Li2Shp.li2_shpns_coring;
        in_sec18_step12_shp_mcts.p_li2_shpns_fir         = &Li2Shp.li2_shpns_fir;
        in_sec18_step12_shp_mcts.p_li2_shpns_cor_idx_scl = &Li2Shp.li2_shpns_cor_idx_scl;
        in_sec18_step12_shp_mcts.p_li2_shpns_min_cor_rst = &Li2Shp.li2_shpns_min_cor_rst;
        in_sec18_step12_shp_mcts.p_li2_shpns_max_cor_rst = &Li2Shp.li2_shpns_max_cor_rst;
        in_sec18_step12_shp_mcts.p_li2_shpns_scl_cor     = &Li2Shp.li2_shpns_scl_cor;

        rval |= ikc_sec18_step12_shp_mcts(&in_sec18_step12_shp_mcts, &out_sharpen);
    }

    {
        ikc_in_hili2_luma_blend_mctf_t in_sec18_step12_hili2_luma_blend;
        ikc_out_sec18_mctf_shpb_t      out_mctf_shpb;
        out_mctf_shpb.p_cr_112= p_cr->step12.sec18.p_CR_buf_112;
        out_mctf_shpb.cr_112_size = CR_SIZE_112;
        out_mctf_shpb.p_cr_117= p_cr->step12.sec18.p_CR_buf_117;
        out_mctf_shpb.cr_117_size = CR_SIZE_117;
        in_sec18_step12_hili2_luma_blend.p_hi_luma_blend = &p_filters->hiso_input_param.hi_luma_blend;
        rval |= ikc_hili2_luma_blend_mctf( &in_sec18_step12_hili2_luma_blend, &out_mctf_shpb);
    }

    {
        ikc_in_hiso_frame_size_t in_hiso_frame_size;
        ikc_out_asf_t out_shpa_frame_size;
        ikc_out_sec18_mctf_shpb_t out_frame_size_div16_m1;
        in_hiso_frame_size.step = 12;  //step12
        in_hiso_frame_size.chroma_filter         = &p_filters->input_param.chroma_filter;
        in_hiso_frame_size.hi_nonsmooth_detect   = &p_filters->hiso_input_param.hi_nonsmooth_detect;
        in_hiso_frame_size.hi_chroma_filter_high = &p_filters->hiso_input_param.hi_chroma_filter_high;
        in_hiso_frame_size.p_main                = &p_filters->input_param.window_size_info.main_win;
        out_shpa_frame_size.p_cr_30    = p_cr->step12.sec2.p_CR_buf_30;
        out_shpa_frame_size.cr_30_size = CR_SIZE_30;
        rval |= ikc_hiso_shpa_frame_size(&in_hiso_frame_size, &out_shpa_frame_size);
        out_frame_size_div16_m1.p_cr_112    = p_cr->step12.sec18.p_CR_buf_112;
        out_frame_size_div16_m1.p_cr_117    = p_cr->step12.sec18.p_CR_buf_117;
        out_frame_size_div16_m1.cr_112_size = CR_SIZE_112;
        out_frame_size_div16_m1.cr_117_size = CR_SIZE_117;
        rval |= ikc_hiso_sec18_frame_size_div16_m1(&in_hiso_frame_size, &out_frame_size_div16_m1);
    }
    {
        ikc_out_sec18_mctf_shpb_t out_mctf;
        ikc_in_sec18_chroma_format_t in_sec18_chroma_format;

        in_sec18_chroma_format.step = 12u; //step12
        out_mctf.p_cr_112 = p_cr->step12.sec18.p_CR_buf_112;
        out_mctf.cr_112_size = CR_SIZE_112;
        out_mctf.p_cr_117 = p_cr->step12.sec18.p_CR_buf_117;
        out_mctf.cr_117_size = CR_SIZE_117;
        rval |= ikc_hiso_sec18_chroma_format(&in_sec18_chroma_format, &out_mctf);
    }
    {
        ikc_out_sec18_mctf_shpb_t out_mctf;
        out_mctf.p_cr_112 = p_cr->step12.sec18.p_CR_buf_112;
        out_mctf.cr_112_size = CR_SIZE_112;
        out_mctf.p_cr_117 = p_cr->step12.sec18.p_CR_buf_117;
        out_mctf.cr_117_size = CR_SIZE_117;
        rval |= ikc_hiso_mctf_ta_constraints(&out_mctf);
    }
    return rval;
}

static uint32 exe_process_hiso_step13_cr(idsp_flow_ctrl_t *p_flow, const amba_ik_hiso_flow_tables_list_t *p_cr, amba_ik_filter_t *p_filters)
{
    uint32 rval = IK_OK;
    uint32 window_calculate_updated = 0U;

    rval |= exe_chroma_noise_def_value(&p_cr->step13.sec2, p_filters);

    if((p_filters->update_flags.iso.rgb_to_yuv_matrix_updated == 1U) ||
       (p_filters->update_flags.iso.is_1st_frame == 1U)) {
        rval |= exe_process_init_y2y_rgb2yuv(&p_cr->step13.sec2, p_filters);
    }

    // exe_process_calib_cr
    if(p_filters->update_flags.iso.is_1st_frame == 1U) { //should only configure once at 1st time.
        rval |= exe_process_input_mode_y2y_step3(p_flow, &p_cr->step13.sec2, p_filters, 13u);
    }
    {
        ik_adv_spatial_filter_t hiso_chroma_asf;
        ikc_in_asf_t  in_asf;
        ikc_out_asf_t out_asf;

        exe_iso_cfg_high_iso_chroma_asf_to_asf(&hiso_chroma_asf, &p_filters->hiso_input_param.chroma_asf);

        if(p_filters->update_flags.cr.r2y.cr_30_update == 0U) {
            amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_hiso_asf !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
            rval |= IK_ERR_0000;
        } else {
            in_asf.p_advance_spatial_filter           = &hiso_chroma_asf;
            in_asf.first_luma_processing_mode_updated = (uint32)p_filters->update_flags.iso.first_luma_processing_mode_updated;
            out_asf.p_cr_30    = p_cr->step13.sec2.p_CR_buf_30;
            out_asf.cr_30_size = CR_SIZE_30;
            rval |= ikc_hiso_asf(&in_asf, &out_asf);
        }
    }

    if(p_filters->update_flags.iso.hili_combine_updated == 1U) {
        ikc_in_chroma_combine_2_mctf_t in_hi_chroma_filter_combine;
        ikc_out_mctf_t out_hi_chroma_filter_combine;
        {
            // hili_combine.*cb, hili_combine.*cr
            ik_hi_chroma_filter_combine_t hili_combine_cbcr;
            hili_combine_cbcr.t0_cb = p_filters->hiso_input_param.hili_combine.t0_cb;
            hili_combine_cbcr.t1_cb = p_filters->hiso_input_param.hili_combine.t1_cb;
            hili_combine_cbcr.alpha_min_cb = p_filters->hiso_input_param.hili_combine.alpha_min_cb;
            hili_combine_cbcr.alpha_max_cb = p_filters->hiso_input_param.hili_combine.alpha_max_cb;
            hili_combine_cbcr.signal_preserve_cb = p_filters->hiso_input_param.hili_combine.signal_preserve_cb;
            (void)amba_ik_system_memcpy(&hili_combine_cbcr.max_change_cb, &p_filters->hiso_input_param.hili_combine.max_change_cb, sizeof(ik_level_t));
            (void)amba_ik_system_memcpy(&hili_combine_cbcr.to_t1_add_cb, &p_filters->hiso_input_param.hili_combine.to_t1_add_cb, sizeof(ik_level_t));
            hili_combine_cbcr.t0_cr = p_filters->hiso_input_param.hili_combine.t0_cr;
            hili_combine_cbcr.t1_cr = p_filters->hiso_input_param.hili_combine.t1_cr;
            hili_combine_cbcr.alpha_min_cr = p_filters->hiso_input_param.hili_combine.alpha_min_cr;
            hili_combine_cbcr.alpha_max_cr = p_filters->hiso_input_param.hili_combine.alpha_max_cr;
            hili_combine_cbcr.signal_preserve_cr = p_filters->hiso_input_param.hili_combine.signal_preserve_cr;
            (void)amba_ik_system_memcpy(&hili_combine_cbcr.max_change_cr, &p_filters->hiso_input_param.hili_combine.max_change_cr, sizeof(ik_level_t));
            (void)amba_ik_system_memcpy(&hili_combine_cbcr.to_t1_add_cr, &p_filters->hiso_input_param.hili_combine.to_t1_add_cr, sizeof(ik_level_t));
            if(p_filters->update_flags.cr.r2y.cr_112_update == 0U) {
                amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on step13 ikc_chroma_combine_2_mctf !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
                rval |= IK_ERR_0000;
            } else {
                in_hi_chroma_filter_combine.p_hi_chroma_filter_combine = &hili_combine_cbcr;
                out_hi_chroma_filter_combine.p_cr_112 = p_cr->step13.sec18.p_CR_buf_112;
                out_hi_chroma_filter_combine.cr_112_size = CR_SIZE_112;
                rval |= ikc_chroma_combine_2_mctf(&in_hi_chroma_filter_combine, &out_hi_chroma_filter_combine);
            }
        }
    }

    if ((p_filters->update_flags.iso.window_size_info_updated == 1U) ||
        (p_filters->update_flags.iso.cfa_window_size_info_updated == 1U) ||
        (p_filters->update_flags.iso.dmy_range_updated == 1U) ||
        (p_filters->update_flags.iso.dzoom_info_updated == 1U) ||
        (p_filters->update_flags.iso.vin_active_win_updated == 1U) ||
        (p_filters->update_flags.iso.stitching_info_updated == 1U) ||
        (p_filters->update_flags.iso.overlap_x_updated == 1U) ||
        (p_filters->update_flags.iso.flip_mode_updated == 1U) ||
        (p_filters->update_flags.iso.warp_enable_updated == 1U) ||
        (p_filters->update_flags.iso.warp_enable_2nd_updated == 1U)) {
        window_calculate_updated = 1U;
    }
    if((p_filters->update_flags.iso.chroma_filter_updated == 1U)||
       (p_filters->update_flags.iso.resample_str_update == 1U)||
       (p_filters->update_flags.iso.window_size_info_updated == 1U)||
       (window_calculate_updated == 1U)) {
        ikc_in_sec2_hiso_resampler_t in_sec2_hiso_resampler;
        ikc_out_sec2_main_resampler_t out_sec2_main_resampler;
        ikc_in_hiso_resampler_size_t  in_hiso_resampler;
        ikc_out_hiso_resampler_size_t out_hiso_resampler;

        in_hiso_resampler.step   = 13U;//step13
        in_hiso_resampler.p_main = &p_filters->input_param.window_size_info.main_win;
        in_hiso_resampler.chroma_filter         = &p_filters->input_param.chroma_filter;
        in_hiso_resampler.hi_nonsmooth_detect   = &p_filters->hiso_input_param.hi_nonsmooth_detect;
        in_hiso_resampler.hi_chroma_filter_high = &p_filters->hiso_input_param.hi_chroma_filter_high;

        rval |= ikc_hiso_resampler_size(&in_hiso_resampler, &out_hiso_resampler);

        in_sec2_hiso_resampler.step   = 13U;
        in_sec2_hiso_resampler.main_cut_off_freq = p_filters->input_param.resample_str.main_cut_off_freq;
        in_sec2_hiso_resampler.p_main = &out_hiso_resampler.output;
        in_sec2_hiso_resampler.chroma_radius = p_filters->input_param.chroma_filter.radius;
        out_sec2_main_resampler.p_cr_33 = p_cr->step13.sec2.p_CR_buf_33;
        out_sec2_main_resampler.cr_33_size = CR_SIZE_33;
        out_sec2_main_resampler.p_cr_35 = p_cr->step13.sec2.p_CR_buf_35;
        out_sec2_main_resampler.cr_35_size = CR_SIZE_35;
        out_sec2_main_resampler.p_cr_37 = p_cr->step13.sec2.p_CR_buf_37;
        out_sec2_main_resampler.cr_37_size = CR_SIZE_37;
        rval |= ikc_sec2_hiso_resampler(&in_sec2_hiso_resampler, &out_sec2_main_resampler);
    }

    {
        ikc_in_hiso_frame_size_t in_hiso_frame_size;
        ikc_out_asf_t out_shpa_frame_size;
        ikc_out_sec18_mctf_shpb_t out_frame_size_div16_m1;
        in_hiso_frame_size.step = 13;  //step13
        in_hiso_frame_size.chroma_filter         = &p_filters->input_param.chroma_filter;
        in_hiso_frame_size.hi_nonsmooth_detect   = &p_filters->hiso_input_param.hi_nonsmooth_detect;
        in_hiso_frame_size.hi_chroma_filter_high = &p_filters->hiso_input_param.hi_chroma_filter_high;
        in_hiso_frame_size.p_main                = &p_filters->input_param.window_size_info.main_win;
        out_shpa_frame_size.p_cr_30    = p_cr->step13.sec2.p_CR_buf_30;
        out_shpa_frame_size.cr_30_size = CR_SIZE_30;
        rval |= ikc_hiso_shpa_frame_size(&in_hiso_frame_size, &out_shpa_frame_size);
        out_frame_size_div16_m1.p_cr_112    = p_cr->step13.sec18.p_CR_buf_112;
        out_frame_size_div16_m1.p_cr_117    = p_cr->step13.sec18.p_CR_buf_117;
        out_frame_size_div16_m1.cr_112_size = CR_SIZE_112;
        out_frame_size_div16_m1.cr_117_size = CR_SIZE_117;
        rval |= ikc_hiso_sec18_frame_size_div16_m1(&in_hiso_frame_size, &out_frame_size_div16_m1);
    }

    {
        ikc_out_sec18_mctf_shpb_t   out_shpb;
        ikc_in_shpb_pass_through_t  in_shpb_pass_through;
        in_shpb_pass_through.is_yuv_422 = 1u;
        in_shpb_pass_through.step       = 13u;
        out_shpb.p_cr_112 = p_cr->step13.sec18.p_CR_buf_112;
        out_shpb.cr_112_size = CR_SIZE_112;
        out_shpb.p_cr_117 = p_cr->step13.sec18.p_CR_buf_117;
        out_shpb.cr_117_size = CR_SIZE_117;
        rval |= ikc_hiso_shpb_pass_through(&in_shpb_pass_through, &out_shpb);
    }
    {
        ikc_out_sec18_mctf_shpb_t out_mctf;
        out_mctf.p_cr_112 = p_cr->step13.sec18.p_CR_buf_112;
        out_mctf.cr_112_size = CR_SIZE_112;
        out_mctf.p_cr_117 = p_cr->step13.sec18.p_CR_buf_117;
        out_mctf.cr_117_size = CR_SIZE_117;
        rval |= ikc_hiso_mctf_ta_constraints(&out_mctf);
    }
    return rval;
}


static uint32 exe_process_hiso_step14_cr(idsp_flow_ctrl_t *p_flow, const amba_ik_hiso_flow_tables_list_t *p_cr, amba_ik_filter_t *p_filters)
{
    uint32 rval = IK_OK;
    uint32 window_calculate_updated = 0U;

    if ((p_filters->update_flags.iso.window_size_info_updated == 1U) ||
        (p_filters->update_flags.iso.cfa_window_size_info_updated == 1U) ||
        (p_filters->update_flags.iso.dmy_range_updated == 1U) ||
        (p_filters->update_flags.iso.dzoom_info_updated == 1U) ||
        (p_filters->update_flags.iso.vin_active_win_updated == 1U) ||
        (p_filters->update_flags.iso.stitching_info_updated == 1U) ||
        (p_filters->update_flags.iso.overlap_x_updated == 1U) ||
        (p_filters->update_flags.iso.flip_mode_updated == 1U) ||
        (p_filters->update_flags.iso.warp_enable_updated == 1U) ||
        (p_filters->update_flags.iso.warp_enable_2nd_updated == 1U)) {
        window_calculate_updated = 1U;
    }

    if((p_filters->update_flags.iso.chroma_filter_updated == 1U)||
       (p_filters->update_flags.iso.warp_enable_updated == 1U)||
       (p_filters->update_flags.iso.calib_warp_info_updated == 1U)||
       (p_filters->update_flags.iso.warp_internal_updated == 1U)||
       (p_filters->update_flags.iso.window_size_info_updated == 1U)||
       (window_calculate_updated == 1U)) {
        if(p_filters->input_param.ctx_buf.warp_internal_mode_flag == 1U) { //internal warp
            //ikc_in_warp_internal_t in_warp_internal;
            //ikc_out_hwarp_t out_hwarp;

            if((p_filters->update_flags.cr.warp == 0U)||
               (p_filters->update_flags.cr.r2y.cr_33_update == 0U)||
               (p_filters->update_flags.cr.r2y.cr_34_update == 0U)||
               (p_filters->update_flags.cr.r2y.cr_35_update == 0U)||
               (p_filters->update_flags.cr.r2y.cr_42_update == 0U)||
               (p_filters->update_flags.cr.r2y.cr_43_update == 0U)||
               (p_filters->update_flags.cr.r2y.cr_44_update == 0U)) {
                amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_warp_internal !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
                rval |= IK_ERR_0000;
            } else {
#if 0
                in_warp_internal.flip_mode = p_filters->input_param.flip_mode;
                in_warp_internal.p_window_size_info = &p_filters->input_param.window_size_info;
                in_warp_internal.p_act_win_crop = &p_filters->input_param.ctx_buf.result_win.act_win_crop;
                in_warp_internal.p_dmy_win_geo = &p_filters->input_param.ctx_buf.result_win.dmy_win_geo;
                in_warp_internal.p_cfa_win_dim = &p_filters->input_param.ctx_buf.result_win.cfa_win_dim;
                in_warp_internal.p_dmy_range = &p_filters->input_param.dmy_range;
                in_warp_internal.chroma_radius = p_filters->input_param.chroma_filter.radius;
                in_warp_internal.p_stitching_info = &p_filters->input_param.stitching_info;
                in_warp_internal.p_warp_internal = &p_filters->input_param.warp_internal;
                in_warp_internal.dram_efficiency = p_filters->input_param.warp_buf_info.dram_efficiency;
                in_warp_internal.luma_wait_lines = p_filters->input_param.warp_buf_info.luma_wait_lines;
                in_warp_internal.luma_dma_size = p_filters->input_param.warp_buf_info.luma_dma_size;
                out_hwarp.p_cr_33 = p_flow_tbl_list->r2y.p_CR_buf_33;
                out_hwarp.cr_33_size = CR_SIZE_33;
                out_hwarp.p_cr_34 = p_flow_tbl_list->r2y.p_CR_buf_34;
                out_hwarp.cr_34_size = CR_SIZE_34;
                out_hwarp.p_cr_35 = p_flow_tbl_list->r2y.p_CR_buf_35;
                out_hwarp.cr_35_size = CR_SIZE_35;
                out_hwarp.p_cr_42 = p_flow_tbl_list->r2y.p_CR_buf_42;
                out_hwarp.cr_42_size = CR_SIZE_42;
                out_hwarp.p_cr_43 = p_flow_tbl_list->r2y.p_CR_buf_43;
                out_hwarp.cr_43_size = CR_SIZE_43;
                out_hwarp.p_cr_44 = p_flow_tbl_list->r2y.p_CR_buf_44;
                out_hwarp.cr_44_size = CR_SIZE_44;
                out_hwarp.p_flow_info = &p_flow->flow_info;
                out_hwarp.p_window = &p_flow->window;
                out_hwarp.p_phase = &p_flow->phase;
                out_hwarp.p_calib = &p_flow->calib;
                out_hwarp.p_stitch = &p_flow->stitch;
                out_hwarp.p_warp_horizontal_table_address = p_flow_tbl_list->r2y.p_warp_hor;
                out_hwarp.h_size = WARP_VIDEO_MAXHORGRID * WARP_VIDEO_MAXVERGRID * 2U * 2U;// multi_slice buffer.
                out_hwarp.p_warp_vertical_table_address = p_flow_tbl_list->r2y.p_warp_ver;
                out_hwarp.v_size = WARP_VIDEO_MAXHORGRID * WARP_VIDEO_MAXVERGRID * 2U * 2U;// multi_slice buffer.
                rval |= ikc_warp_internal(&in_warp_internal, &out_hwarp);
#endif
            }
        } else {
            ikc_in_vwarp_t in_vwarp;
            ikc_out_vwarp_t out_vwarp;

            if((p_filters->update_flags.cr.warp == 0U)||
               (p_filters->update_flags.cr.r2y.cr_33_update == 0U)||
               (p_filters->update_flags.cr.r2y.cr_34_update == 0U)||
               (p_filters->update_flags.cr.r2y.cr_35_update == 0U)||
               (p_filters->update_flags.cr.r2y.cr_42_update == 0U)||
               (p_filters->update_flags.cr.r2y.cr_43_update == 0U)||
               (p_filters->update_flags.cr.r2y.cr_44_update == 0U)) {
                amba_ik_system_print_uint32_5("[IK] [ERROR] Detect fatally concurrent access on ikc_vwarp !!!",DC_U,DC_U,DC_U,DC_U,DC_U);
                rval |= IK_ERR_0000;
            } else {
                {
                    in_vwarp.is_hvh_enabled = 0;
                    in_vwarp.is_hhb_enabled = 0;
                    in_vwarp.is_multi_pass = 0u;
                    in_vwarp.is_group_cmd = 0u;
                    in_vwarp.warp_enable = (uint32)p_filters->input_param.warp_enable;
                    in_vwarp.flip_mode = p_filters->input_param.flip_mode;
                    in_vwarp.p_window_size_info = &p_filters->input_param.window_size_info;
                    in_vwarp.p_act_win_crop = &p_filters->input_param.ctx_buf.result_win.act_win_crop;
                    in_vwarp.p_dmy_win_geo = &p_filters->input_param.ctx_buf.result_win.dmy_win_geo;
                    in_vwarp.p_cfa_win_dim = &p_filters->input_param.ctx_buf.result_win.cfa_win_dim;
                    in_vwarp.p_dmy_range = &p_filters->input_param.dmy_range;
                    in_vwarp.chroma_radius = p_filters->input_param.chroma_filter.radius;
                    in_vwarp.is_y2y_420input = (p_filters->input_param.yuv_mode == AMBA_IK_YUV420) ? 1U : 0U;
                    in_vwarp.p_stitching_info = &p_filters->input_param.stitching_info;
                    in_vwarp.p_calib_warp_info = &p_filters->input_param.calib_warp_info;
                    in_vwarp.dram_efficiency = p_filters->input_param.warp_buf_info.dram_efficiency;
                    in_vwarp.luma_wait_lines = p_filters->input_param.warp_buf_info.luma_wait_lines;
                    in_vwarp.luma_dma_size = p_filters->input_param.warp_buf_info.luma_dma_size;
                    in_vwarp.p_burst_tile = &p_filters->input_param.burst_tile;
                    out_vwarp.p_cr_42 = p_cr->step14.sec3.p_CR_buf_42;
                    out_vwarp.cr_42_size = CR_SIZE_42;
                    out_vwarp.p_cr_43 = p_cr->step14.sec3.p_CR_buf_43;
                    out_vwarp.cr_43_size = CR_SIZE_43;
                    out_vwarp.p_cr_44 = p_cr->step14.sec3.p_CR_buf_44;
                    out_vwarp.cr_44_size = CR_SIZE_44;
                    out_vwarp.p_window = &p_flow->window;
                    out_vwarp.p_calib = &p_flow->calib;
                    out_vwarp.p_warp_vertical_table_address = p_cr->p_warp_ver;
                    out_vwarp.v_size = WARP_VIDEO_MAXHORGRID * WARP_VIDEO_MAXVERGRID * 2U * 2U;// multi_slice buffer.

                    rval |= ikc_vwarp(&in_vwarp, &out_vwarp);
                }
            }
            {
                ikc_in_sec3_vertical_resampler_t in_sec3_vertical_resampler;
                ikc_out_sec3_vertical_resampler_t out_sec3_vertical_resampler;

                in_sec3_vertical_resampler.p_act_win_crop = &p_filters->input_param.ctx_buf.result_win.act_win_crop;
                in_sec3_vertical_resampler.p_dmy_win_geo = &p_filters->input_param.ctx_buf.result_win.dmy_win_geo;
                in_sec3_vertical_resampler.p_cfa_win_dim = &p_filters->input_param.ctx_buf.result_win.cfa_win_dim;
                in_sec3_vertical_resampler.p_main = &p_filters->input_param.window_size_info.main_win;
                in_sec3_vertical_resampler.p_dmy_range = &p_filters->input_param.dmy_range;
                in_sec3_vertical_resampler.chroma_radius = p_filters->input_param.chroma_filter.radius;
                in_sec3_vertical_resampler.is_y2y_420input = (p_filters->input_param.yuv_mode == AMBA_IK_YUV420) ? 1U : 0U;
                in_sec3_vertical_resampler.flip_mode = p_filters->input_param.flip_mode;
                out_sec3_vertical_resampler.p_cr_42 = p_cr->step14.sec3.p_CR_buf_42;
                out_sec3_vertical_resampler.cr_42_size = CR_SIZE_42;
                rval |= ikc_sec3_vertical_resampler_still(&in_sec3_vertical_resampler, &out_sec3_vertical_resampler);
            }
        }
        p_flow->calib.update_lens_warp = 1U;
    } else {
        p_flow->calib.update_lens_warp = 0U;
    }

    {
        ikc_out_sec18_mctf_shpb_t out_mctf;
        ikc_in_mctf_pass_through_t in_mctf_pass_through;

        in_mctf_pass_through.is_yuv_422 = 0u;
        in_mctf_pass_through.step       = 14u; //step14
        out_mctf.p_cr_112 = p_cr->step14.sec18.p_CR_buf_112;
        out_mctf.cr_112_size = CR_SIZE_112;
        out_mctf.p_cr_117 = p_cr->step14.sec18.p_CR_buf_117;
        out_mctf.cr_117_size = CR_SIZE_117;
        rval |= ikc_hiso_mctf_pass_through(&in_mctf_pass_through, &out_mctf);
    }
    {
        ikc_out_sec18_mctf_shpb_t out_shpb;
        ikc_in_shpb_pass_through_t  in_shpb_pass_through;
        in_shpb_pass_through.is_yuv_422 = 0u;
        in_shpb_pass_through.step       = 14u;
        out_shpb.p_cr_112 = p_cr->step14.sec18.p_CR_buf_112;
        out_shpb.cr_112_size = CR_SIZE_112;
        out_shpb.p_cr_117 = p_cr->step14.sec18.p_CR_buf_117;
        out_shpb.cr_117_size = CR_SIZE_117;
        rval |= ikc_hiso_shpb_pass_through(&in_shpb_pass_through, &out_shpb);
    }
    /*{
        ikc_out_sec18_mctf_shpb_t out_mctf;
        out_mctf.p_cr_112 = p_cr->step14.sec18.p_CR_buf_112;
        out_mctf.cr_112_size = CR_SIZE_112;
        out_mctf.p_cr_117 = p_cr->step14.sec18.p_CR_buf_117;
        out_mctf.cr_117_size = CR_SIZE_117;
        rval |= ikc_hiso_mctf_ta_constraints(&out_mctf);
    }*/
    {
        if((p_filters->update_flags.iso.chroma_filter_updated == 1U)||
           (p_filters->update_flags.iso.resample_str_update == 1U)||
           (p_filters->update_flags.iso.window_size_info_updated == 1U)||
           (window_calculate_updated == 1U)) {
            ikc_in_sec2_hiso_resampler_t in_sec2_hiso_resampler;
            ikc_out_sec2_main_resampler_t out_sec2_main_resampler;
            ikc_in_hiso_resampler_size_t  in_hiso_resampler;
            ikc_out_hiso_resampler_size_t out_hiso_resampler;

            in_hiso_resampler.step   = 14U;//step14
            in_hiso_resampler.p_main = &p_filters->input_param.window_size_info.main_win;
            in_hiso_resampler.chroma_filter         = &p_filters->input_param.chroma_filter;
            in_hiso_resampler.hi_nonsmooth_detect   = &p_filters->hiso_input_param.hi_nonsmooth_detect;
            in_hiso_resampler.hi_chroma_filter_high = &p_filters->hiso_input_param.hi_chroma_filter_high;
            out_hiso_resampler.p_flow               = p_flow;
            rval |= ikc_hiso_resampler_size(&in_hiso_resampler, &out_hiso_resampler);
            in_sec2_hiso_resampler.step   = 14U;
            in_sec2_hiso_resampler.main_cut_off_freq = p_filters->input_param.resample_str.main_cut_off_freq;
            in_sec2_hiso_resampler.p_main = &out_hiso_resampler.output;
            in_sec2_hiso_resampler.chroma_radius = p_filters->input_param.chroma_filter.radius;
            out_sec2_main_resampler.p_cr_33 = p_cr->step13.sec2.p_CR_buf_33;
            out_sec2_main_resampler.cr_33_size = CR_SIZE_33;
            out_sec2_main_resampler.p_cr_35 = p_cr->step13.sec2.p_CR_buf_35;
            out_sec2_main_resampler.cr_35_size = CR_SIZE_35;
            out_sec2_main_resampler.p_cr_37 = p_cr->step13.sec2.p_CR_buf_37;
            out_sec2_main_resampler.cr_37_size = CR_SIZE_37;
            rval |= ikc_sec2_hiso_resampler(&in_sec2_hiso_resampler, &out_sec2_main_resampler);
        }
    }
    {
        ikc_in_hiso_frame_size_t in_hiso_frame_size;
        ikc_out_sec18_mctf_shpb_t out_frame_size_div16_m1;
        in_hiso_frame_size.step = 14;  //step13
        in_hiso_frame_size.chroma_filter         = &p_filters->input_param.chroma_filter;
        in_hiso_frame_size.hi_nonsmooth_detect   = &p_filters->hiso_input_param.hi_nonsmooth_detect;
        in_hiso_frame_size.hi_chroma_filter_high = &p_filters->hiso_input_param.hi_chroma_filter_high;
        in_hiso_frame_size.p_main                = &p_filters->input_param.window_size_info.main_win;
        out_frame_size_div16_m1.p_cr_112    = p_cr->step14.sec18.p_CR_buf_112;
        out_frame_size_div16_m1.p_cr_117    = p_cr->step14.sec18.p_CR_buf_117;
        out_frame_size_div16_m1.cr_112_size = CR_SIZE_112;
        out_frame_size_div16_m1.cr_117_size = CR_SIZE_117;
        rval |= ikc_hiso_sec18_frame_size_div16_m1(&in_hiso_frame_size, &out_frame_size_div16_m1);
    }
    return rval;
}


uint32 exe_process_hiso_cr(idsp_flow_ctrl_t *p_flow, amba_ik_hiso_flow_tables_list_t *p_flow_tbl_list, amba_ik_filter_t *p_filters)
{
    uint32 rval = IK_OK;
    //ikc_in_temp_hack_hiso_data_t in_hiso;
    //ikc_out_temp_hack_hiso_data_t out_hiso;

    (void)amba_ik_system_memcpy(p_flow, p_flow, sizeof(uint8));//misra fix.
    (void)amba_ik_system_memcpy(p_flow_tbl_list, p_flow_tbl_list, sizeof(uint8));//misra fix.

    if (p_flow == NULL) {
        /*misra fix*/
    } else {
        //in_hiso.exp_num = p_filters->input_param.num_of_exposures;
        //in_hiso.p_hdr_raw_info = &p_filters->input_param.hdr_raw_info;
        //out_hiso.p_flow = p_flow;
        //rval |= ikc_temp_hack_hiso_data(&in_hiso, &out_hiso);


        rval |= exe_process_hiso_step1_cr(p_flow, p_flow_tbl_list, p_filters, p_filters->input_param.num_of_exposures);
        rval |= exe_process_hiso_step2_cr(p_flow, p_flow_tbl_list, p_filters);
        rval |= exe_process_hiso_step3_cr(p_flow, p_flow_tbl_list, p_filters);
        rval |= exe_process_hiso_step4_cr(p_flow, p_flow_tbl_list, p_filters);
        rval |= exe_process_hiso_step4a_cr(p_flow, p_flow_tbl_list, p_filters);
        rval |= exe_process_hiso_step5_cr(p_flow, p_flow_tbl_list, p_filters);
        rval |= exe_process_hiso_step6_cr(p_flow, p_flow_tbl_list, p_filters);
        rval |= exe_process_hiso_step7_cr(p_flow, p_flow_tbl_list, p_filters);
        rval |= exe_process_hiso_step8_cr(p_flow, p_flow_tbl_list, p_filters);
        rval |= exe_process_hiso_step9_cr(p_flow, p_flow_tbl_list, p_filters);
        rval |= exe_process_hiso_step10_cr(p_flow, p_flow_tbl_list, p_filters );
        rval |= exe_process_hiso_step11_cr(p_flow, p_flow_tbl_list, p_filters, p_filters->input_param.num_of_exposures);
        rval |= exe_process_hiso_step12_cr(p_flow, p_flow_tbl_list, p_filters, p_filters->input_param.num_of_exposures);
        rval |= exe_process_hiso_step13_cr(p_flow, p_flow_tbl_list, p_filters);
#ifndef _HISO_SEC3_IN_STEP13
        rval |= exe_process_hiso_step14_cr(p_flow, p_flow_tbl_list, p_filters);
#endif
    }
    return rval;
}
