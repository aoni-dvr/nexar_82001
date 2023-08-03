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

#ifndef CONFIG_COMPONENT_IF_H
#define CONFIG_COMPONENT_IF_H
#include "ik_data_type.h"
#include "AmbaDSP_ImgContextEntity.h"
#include "AmbaDSP_ImgConfigEntity.h"


uint32 img_cfg_query_flow_control_memory_size(size_t *p_mem_size);
uint32 img_cfg_query_flow_tbl_memory_size(const ik_ability_t *p_ability, size_t *p_mem_size);
uint32 img_cfg_query_flow_tbl_list_memory_size(const ik_ability_t *p_ability, size_t *p_mem_size);
uint32 img_cfg_query_crc_data_memory_size(const ik_ability_t *p_ability, size_t *p_mem_size);
uint32 img_cfg_prepare_flow_control_memory(void **p_flow_mem, uintptr mem_addr, size_t size, size_t flow_ctrl_size, uint32 init_mode);
uint32 img_cfg_prepare_flow_tbl_memory(void **p_flow_tbl_mem, uintptr mem_addr, size_t size, size_t flow_tbl_size, uint32 init_mode);
uint32 img_cfg_prepare_flow_tbl_list_memory(void **p_flow_tbl_list_mem, uintptr mem_addr, size_t size, size_t flow_tbl_list_size, uint32 init_mode);
uint32 img_cfg_prepare_crc_data_memory(void **p_crc_data_mem, uintptr mem_addr, size_t size, size_t crc_data_size, uint32 init_mode);
uint32 img_cfg_init_flow_control(uint32 context_id, idsp_flow_ctrl_t *p_flow, const ik_ability_t *p_ability);
uint32 img_cfg_init_flow_tbl(void *p_flow_tbl, const void *p_flow_tbl_list, const ik_ability_t *p_ability, const void *p_bin_data_dram_addr);
uint32 img_cfg_init_flow_tbl_list(const void *p_flow_tbl, void *p_flow_tbl_list, const ik_ability_t *p_ability);
uint32 img_cfg_reinit_flow_tbl_list(void *p_flow_tbl_list, const ik_ability_t *p_ability);
uint32 img_cfg_init_null_flow_tbl_list(void *p_flow_tbl_list, const ik_ability_t *p_ability);
uint32 img_cfg_query_debug_flow_data_size(uint32 context_id, size_t *p_size);
uint32 img_cfg_amalgam_data_update(const idsp_flow_ctrl_t *p_flow, const amba_ik_flow_tables_list_t *p_flow_tbl_list, const void *p_user_buffer);
uint32 img_cfg_amalgam_data_ctx_update(uint32 context_id, const void *p_user_buffer);
uint32 img_cfg_hiso_query_debug_flow_data_size(size_t *p_size);
uint32 img_cfg_hiso_amalgam_data_update(const idsp_flow_ctrl_t *p_flow, const amba_ik_hiso_flow_tables_list_t *p_flow_tbl_list, const void *p_user_buffer);
uint32 img_cfg_motion_fusion_amalgam_data_update(const idsp_flow_ctrl_t *p_flow, const amba_ik_motion_fusion_flow_tables_list_t *p_flow_tbl_list, const void *p_user_buffer);
uint32 img_cfg_motion_fusion_amalgam_data_ctx_update(uint32 context_id, const void *p_user_buffer);

#endif

