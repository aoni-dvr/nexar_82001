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

#include "ik_data_type.h"
#include "AmbaDSP_ImgContextEntity.h"
#include "AmbaDSP_ImgConfigEntity.h"


typedef struct {
    uint32 (*compute_cr)(idsp_flow_ctrl_t *p_flow, void *p_list, amba_ik_filter_t *p_filters);
} amba_ik_exe_cr_utilities_t;

uint32 exe_process_calib_cr(idsp_flow_ctrl_t *p_flow, void *p_flow_tbl_list, amba_ik_filter_t *p_filters);
uint32 exe_process_hdr_ce_cr(idsp_flow_ctrl_t *p_flow, void *p_flow_tbl_list, amba_ik_filter_t *p_filters, uint32 exp_num);
uint32 exe_process_cfa_cr(idsp_flow_ctrl_t *p_flow, void *p_flow_tbl_list, amba_ik_filter_t *p_filters);
uint32 exe_process_rgb_cr(void *p_flow_tbl_list, amba_ik_filter_t *p_filters);
uint32 exe_process_yuv_cr(idsp_flow_ctrl_t *p_flow, void *p_flow_tbl_list, amba_ik_filter_t *p_filters);
uint32 exe_process_hdr_ce_sub_cr(void *p_flow_tbl_list, amba_ik_filter_t *p_filters, uint32 exp_num);
uint32 exe_process_mctf_mcts_cr(idsp_flow_ctrl_t *p_flow, void *p_flow_tbl_list, amba_ik_filter_t *p_filters);
uint32 exe_process_mctf_mcts_still_liso_cr(idsp_flow_ctrl_t *p_flow, void *p_flow_tbl_list, amba_ik_filter_t *p_filters);
uint32 exe_process_optc_r2y_default_cr(idsp_flow_ctrl_t *p_flow, void *p_flow_tbl_list, const amba_ik_filter_t *p_filters);
uint32 exe_process_optc_r2y_cr(idsp_flow_ctrl_t *p_flow, void *p_flow_tbl_list, amba_ik_filter_t *p_filters);
uint32 exe_process_optc_motion_me1_cr(idsp_flow_ctrl_t *p_flow, void *p_flow_tbl_list, amba_ik_filter_t *p_filters);
uint32 exe_process_optc_motion_a_cr(idsp_flow_ctrl_t *p_flow, void *p_flow_tbl_list, amba_ik_filter_t *p_filters);
uint32 exe_process_optc_motion_b_cr(idsp_flow_ctrl_t *p_flow, void *p_flow_tbl_list, amba_ik_filter_t *p_filters);
uint32 exe_process_optc_motion_c_cr(idsp_flow_ctrl_t *p_flow, void *p_flow_tbl_list, amba_ik_filter_t *p_filters);
uint32 exe_process_mono1_8_cr(idsp_flow_ctrl_t *p_flow, void *p_flow_tbl_list, amba_ik_filter_t *p_filters, uint32 pass);
uint32 exe_process_mono3_cr(idsp_flow_ctrl_t *p_flow, void *p_flow_tbl_list, amba_ik_filter_t *p_filters);
uint32 exe_process_mono4_cr(idsp_flow_ctrl_t *p_flow, void *p_flow_tbl_list, amba_ik_filter_t *p_filters);
uint32 exe_process_mono5_cr(idsp_flow_ctrl_t *p_flow, void *p_flow_tbl_list, amba_ik_filter_t *p_filters);
uint32 exe_process_mono6_cr(idsp_flow_ctrl_t *p_flow, void *p_flow_tbl_list, amba_ik_filter_t *p_filters);
uint32 exe_process_mono7_cr(idsp_flow_ctrl_t *p_flow, void *p_flow_tbl_list, amba_ik_filter_t *p_filters);

uint32 exe_process_temp_hack_hiso_data(idsp_flow_ctrl_t *p_flow, amba_ik_filter_t *p_filters);
uint32 exe_process_hiso_cr(idsp_flow_ctrl_t *p_flow, amba_ik_hiso_flow_tables_list_t *p_flow_tbl_list, amba_ik_filter_t *p_filters);

#endif
