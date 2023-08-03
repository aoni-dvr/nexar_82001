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

#ifndef IMG_EXECUTER_UTILTIES_H
#define IMG_EXECUTER_UTILTIES_H

#include "AmbaDSP_ImgControlState.h"
#include "AmbaDSP_ImgContextEntity.h"
#include "AmbaDSP_ImgConfigEntity.h"

typedef struct {
    //int32 (*hook_iso_config_tables)(amba_ik_cv1_liso_cfg_t *, amba_ik_setup_cmds_t *, amba_ik_iso_config_table_index_t *, intptr *);
    uint32 (*compute_first_liso_config_and_tables)(const ik_cv2_liso_cfg_t *iso_config, amba_ik_setup_cmds_t *cmds, amba_ik_filter_t *filters);
    uint32 (*compute_first_hiso_config_and_tables)(const ik_cv2_hiso_cfg_t *iso_config, const amba_ik_setup_cmds_t *cmds, amba_ik_filter_t *filters);
} amba_ik_exe_first_iso_utilities_t;

typedef struct {
    //int32 (*hook_iso_config_tables)(amba_ik_cv1_liso_cfg_t *, amba_ik_setup_cmds_t *, amba_ik_iso_config_table_index_t *, intptr *);
    uint32 (*compute_updated_iso_config_and_tables)(const ik_cv2_liso_cfg_t *iso_config, amba_ik_setup_cmds_t *cmds, amba_ik_filter_t *filters);
    //int32 (*clone_iso_config)(amba_ik_configs_entity_t *, amba_ik_attribute_t *, uint32, uint32);
    //int32 (*get_next_iso_config_index)(amba_ik_active_config_state_t *, uint32 *);
} amba_ik_exe_update_iso_utilities_t;

uint32 ikc_exe_iso_cfg_share_first(const amba_ik_filter_t *p_filters, const iso_meta_header_t *p_iso_share, const amba_ik_setup_cmds_t *p_cmds);
uint32 ikc_exe_iso_cfg_calib_first(amba_ik_filter_t *p_filters, const iso_calib_data_t *p_iso_calib, const amba_ik_setup_cmds_t *p_cmds);
uint32 ikc_exe_iso_cfg_calib_mipi_first(amba_ik_filter_t *p_filters, const iso_calib_data_t *p_iso_calib, const amba_ik_setup_cmds_t *p_cmds);
uint32 ikc_exe_iso_cfg_color_first(amba_ik_filter_t *p_filters, const iso_color_data_t *p_iso_color, const amba_ik_setup_cmds_t *p_cmds);
uint32 ikc_exe_iso_cfg_liso_first(amba_ik_filter_t *p_filters, const iso_raw2yuv_data_t *p_iso_liso, amba_ik_setup_cmds_t *p_cmds);
uint32 ikc_exe_iso_cfg_flow_first(const amba_ik_filter_t *p_filters, const iso_flow_data_t *p_iso_flow, amba_ik_setup_cmds_t *p_cmds);
uint32 ikc_exe_iso_cfg_extra_first(amba_ik_filter_t *p_filters, const iso_extra_data_t *p_iso_extra, int32 exp_num);
uint32 ikc_exe_iso_cfg_motion_first(const amba_ik_filter_t *p_filters, const iso_motion_detect_data_t *p_iso_motion, const amba_ik_setup_cmds_t *p_cmds);
uint32 ikc_exe_iso_cfg_single_exp_first(amba_ik_filter_t *p_filters, const iso_single_exposure_data_t *p_iso_single_exp, const amba_ik_setup_cmds_t *p_cmds, int32 exp_num, uint32 exp_idx);
uint32 ikc_exe_iso_cfg_color_mipi_first(amba_ik_filter_t *p_filters, const iso_color_data_t *p_iso_color, const amba_ik_setup_cmds_t *p_cmds);
uint32 ikc_exe_iso_cfg_extra2_first(const amba_ik_filter_t *p_filters, const iso_extra2_data_t *p_iso_extra2);

uint32 ikc_exe_iso_cfg_share_update(const amba_ik_filter_t *p_filters, const iso_meta_header_t *p_iso_share, const amba_ik_setup_cmds_t *p_cmds);
uint32 ikc_exe_iso_cfg_calib_update(amba_ik_filter_t *p_filters, const iso_calib_data_t *p_iso_calib,const amba_ik_setup_cmds_t *p_cmds, int32 exp_num);
uint32 ikc_exe_iso_cfg_color_update(amba_ik_filter_t *p_filters, const iso_color_data_t *p_iso_color, const amba_ik_setup_cmds_t *p_cmds);
uint32 ikc_exe_iso_cfg_liso_update(amba_ik_filter_t *p_filters, const iso_raw2yuv_data_t *p_iso_liso, amba_ik_setup_cmds_t *p_cmds);
uint32 ikc_exe_iso_cfg_flow_update(const amba_ik_filter_t *p_filters, const iso_flow_data_t *p_iso_flow, amba_ik_setup_cmds_t *p_cmds);
uint32 ikc_exe_iso_cfg_extra_update(amba_ik_filter_t *p_filters, const iso_extra_data_t *p_iso_extra, int32 exp_num);
uint32 ikc_exe_iso_cfg_motion_update(const amba_ik_filter_t *p_filters, const iso_motion_detect_data_t *p_iso_motion, const amba_ik_setup_cmds_t *p_cmds);
uint32 ikc_exe_iso_cfg_single_exp_update(const amba_ik_filter_t *p_filters, const iso_single_exposure_data_t *p_iso_single_exp, const amba_ik_setup_cmds_t *p_cmds, int32 exp_num, uint32 exp_idx);
uint32 ikc_exe_iso_cfg_extra2_update(const amba_ik_filter_t *p_filters, const iso_extra2_data_t *p_iso_extra2);

#if SUPPORT_FUSION
uint32 ikc_exe_iso_cfg_calib_mono_first(amba_ik_filter_t *p_filters, iso_calib_data_t *p_iso_calib, amba_ik_setup_cmds_t *p_cmds);
uint32 ikc_exe_iso_cfg_color_mono_first(amba_ik_filter_t *p_filters, iso_color_data_t *p_iso_color);
uint32 ikc_exe_iso_cfg_liso_mono_first(amba_ik_filter_t *p_filters, iso_raw2yuv_data_t *p_iso_liso, amba_ik_setup_cmds_t *p_cmds);
uint32 ikc_exe_iso_cfg_extra_mono_first(amba_ik_filter_t *p_filters, iso_extra_data_t *p_iso_extra, amba_ik_setup_cmds_t *p_cmds, int32 exp_num);
uint32 ikc_exe_iso_cfg_single_exp_mono_first(amba_ik_filter_t *p_filters, iso_single_exposure_data_t *p_iso_single_exp, amba_ik_setup_cmds_t *p_cmds);
uint32 ikc_exe_iso_cfg_yuv_mono_first(amba_ik_filter_t *p_filters, iso_mono_yuv_t *p_iso_yuv);
uint32 ikc_exe_iso_cfg_mctf_mono_first(amba_ik_filter_t *p_filters, iso_mono_mctf_t *p_iso_mono_mctf, iso_flow_data_t *p_iso_flow);
uint32 ikc_exe_iso_cfg_step4_mono_first(amba_ik_filter_t *p_filters, iso_sharpen_data_t *p_iso_shp);

uint32 ikc_exe_iso_cfg_calib_mono_update(amba_ik_filter_t *p_filters, iso_calib_data_t *p_iso_calib,amba_ik_setup_cmds_t *p_cmds);
uint32 ikc_exe_iso_cfg_color_mono_update(amba_ik_filter_t *p_filters, iso_color_data_t *p_iso_color, amba_ik_setup_cmds_t *p_cmds);
uint32 ikc_exe_iso_cfg_liso_mono_update(amba_ik_filter_t *p_filters, iso_raw2yuv_data_t *p_iso_liso, amba_ik_setup_cmds_t *p_cmds);
uint32 ikc_exe_iso_cfg_extra_mono_update(amba_ik_filter_t *p_filters, iso_extra_data_t *p_iso_extra);
uint32 ikc_exe_iso_cfg_single_exp_mono_update(amba_ik_filter_t *p_filters, iso_single_exposure_data_t *p_iso_single_exp);
uint32 ikc_exe_iso_cfg_yuv_mono_update(amba_ik_filter_t *p_filters, iso_mono_yuv_t *p_iso_yuv);
uint32 ikc_exe_iso_cfg_mctf_mono_update(amba_ik_filter_t *p_filters, iso_mono_mctf_t *p_iso_mono_mctf, iso_flow_data_t *p_iso_flow);
#endif

#if PRE_NN_PROCESS
uint32 ikc_exe_iso_cfg_pre_nn_first(amba_ik_filter_t *p_filters, const iso_nn_param_t *p_iso_pre_nn);
uint32 ikc_exe_iso_cfg_pre_nn_update(amba_ik_filter_t *p_filters, const iso_nn_param_t *p_iso_pre_nn);
#endif

uint32 ikc_exe_iso_cfg_hiso_share(const amba_ik_filter_t *p_filters, const iso_meta_header_t *p_cfg_header, const amba_ik_setup_cmds_t *p_cmds);
uint32 ikc_exe_iso_cfg_hiso_calib(amba_ik_filter_t *p_filters, const iso_calib_data_t *p_cfg_calib, const amba_ik_setup_cmds_t *p_cmds);
uint32 ikc_exe_iso_cfg_hiso_color(amba_ik_filter_t *p_filters, const iso_color_data_t *p_cfg_color, const amba_ik_setup_cmds_t *p_cmds);
uint32 ikc_exe_iso_cfg_hiso_step1(amba_ik_filter_t *p_filters, const iso_still_hiso_param01_t *p_iso_step1);
uint32 ikc_exe_iso_cfg_hiso_step2(const amba_ik_filter_t *p_filters, const iso_still_hiso_param02_t *p_iso_step2);
uint32 ikc_exe_iso_cfg_hiso_step3(const amba_ik_filter_t *p_filters, const iso_still_hiso_param03_t *p_iso_step3);
uint32 ikc_exe_iso_cfg_hiso_step4(const amba_ik_filter_t *p_filters, const iso_still_hiso_param04_t *p_iso_step4);
uint32 ikc_exe_iso_cfg_hiso_step5(const amba_ik_filter_t *p_filters, const iso_still_hiso_param05_t *p_iso_step5);
uint32 ikc_exe_iso_cfg_hiso_step6(const amba_ik_filter_t *p_filters, const iso_still_hiso_param06_t *p_iso_step6);
uint32 ikc_exe_iso_cfg_hiso_step7(const amba_ik_filter_t *p_filters, const iso_still_hiso_param07_t *p_iso_step7);
uint32 ikc_exe_iso_cfg_hiso_step8(const amba_ik_filter_t *p_filters, const iso_still_hiso_param08_t *p_iso_step8);
uint32 ikc_exe_iso_cfg_hiso_step9(const amba_ik_filter_t *p_filters, const iso_still_hiso_param09_t *p_iso_step9);
uint32 ikc_exe_iso_cfg_hiso_step11(amba_ik_filter_t *p_filters, const iso_still_hiso_param11_t *p_iso_step11);
uint32 ikc_exe_iso_cfg_hiso_step12(amba_ik_filter_t *p_filters, const iso_still_hiso_param12_t *p_iso_step12);
uint32 ikc_exe_iso_cfg_hiso_step13(const amba_ik_filter_t *p_filters, const iso_still_hiso_param13_t *p_iso_step13);
uint32 ikc_exe_iso_cfg_hiso_flow(const amba_ik_filter_t *p_filters, const iso_still_hiso_param_flow_t *p_iso_flow);
uint32 ikc_exe_iso_cfg_hiso_mctf(const amba_ik_filter_t *p_filters, const iso_still_hiso_param_mctf_t *p_iso_mctf);
//uint32 ikc_exe_iso_cfg_hiso_vwarp(amba_ik_filter_t *p_filters, iso_liso_param_vwarp_t *p_cfg_vwarp);
#endif
