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

#ifndef IMG_ADVANCED_FILTERS_IF_H
#define IMG_ADVANCED_FILTERS_IF_H

#include "ik_data_type.h"
#include "AmbaDSP_ImgCommon.h"

typedef struct {
    ik_hdr_raw_info_t hdr_raw_info;
    uint32 reserved[26];
} ik_query_frame_info_t; // make it 128 alignment

typedef struct {
    int32       shift_x;     /* 16.16 format, unit in pixel in VinSensorGeo domain.*/
    int32       shift_y;     /* 16.16 format, unit in pixel in VinSensorGeo domain.*/
} ik_hdr_eis_info_t;

uint32 ik_query_arch_memory_size(
    const ik_context_setting_t *p_setting_of_each_context,
    size_t *p_total_ctx_size,
    size_t *p_flow_ctrl_size,
    size_t *p_flow_tbl_size,
    size_t *p_flow_tbl_list_size,
    size_t *p_crc_data_size);

typedef struct {
    uint32 enable;
    ik_window_dimension_t cfa;
} ik_cfa_window_size_info_t;

typedef struct {
    uint32 overlap_x;
} ik_overlap_x_t;

uint32 ik_set_hdr_exp_num(uint32 context_id, const uint32 exp_num);
uint32 ik_get_hdr_exp_num(uint32 context_id, uint32 *p_exp_num);

uint32 ik_set_hdr_shift_info_exp1(uint32 context_id, const ik_hdr_eis_info_t *p_hdr_sft_info);
uint32 ik_get_hdr_shift_info_exp1(uint32 context_id, ik_hdr_eis_info_t *p_hdr_sft_info);

uint32 ik_set_hdr_shift_info_exp2(uint32 context_id, const ik_hdr_eis_info_t *p_hdr_sft_info);
uint32 ik_get_hdr_shift_info_exp2(uint32 context_id, ik_hdr_eis_info_t *p_hdr_sft_info);

uint32 ik_query_frame_info(uint32 context_id, const uint32 ik_id, ik_query_frame_info_t *p_frame_info);

uint32 ik_set_cfa_window_size_info(uint32 context_id, const ik_cfa_window_size_info_t *p_cfa_window_size_info);
uint32 ik_get_cfa_window_size_info(uint32 context_id, ik_cfa_window_size_info_t *p_cfa_window_size_info);

uint32 ik_set_overlap_x(uint32 context_id, const ik_overlap_x_t *p_overlap_x);
uint32 ik_get_overlap_x(uint32 context_id, ik_overlap_x_t *p_overlap_x);

uint32 ik_execute_eis_1st(uint32 context_id, ik_execute_container_t *p_execute_container);
uint32 ik_execute_eis_2nd(uint32 context_id, uint32 explicit_eis_ik_id, ik_execute_container_t *p_execute_container);

uint32 ik_set_ext_raw_out_mode(uint32 context_id, const uint32 ext_raw_out_mode);
uint32 ik_get_ext_raw_out_mode(uint32 context_id, uint32 *p_ext_raw_out_mode);

#endif
