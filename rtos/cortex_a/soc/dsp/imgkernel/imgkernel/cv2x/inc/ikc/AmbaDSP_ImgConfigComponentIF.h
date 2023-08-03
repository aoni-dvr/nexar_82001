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
#include "AmbaDSP_ImgConfigEntity.h"

uint32 img_cfg_query_iso_config_memory_size(const ikc_ability_t *ability, size_t *p_mem_size);
uint32 img_cfg_query_iso_config_tables_memory_size(const ikc_ability_t *ability, size_t *p_mem_size);
uint32 img_cfg_query_container_memory_size(size_t *p_mem_size);
uint32 img_cfg_prepare_iso_config_memory(const ikc_ability_t *ability, amba_ik_iso_config_and_state_t **p_cfg_mem, uintptr mem_addr, size_t size, uint32 init_mode);
uint32 img_cfg_prepare_iso_config_table_memory(const ikc_ability_t *ability, amba_ik_iso_config_tables_t **p_cfg_tbl_mem, uintptr mem_addr, size_t size, uint32 init_mode);
uint32 img_cfg_prepare_container_memory(amba_ik_config_container_t **p_container_mem, uintptr mem_addr, size_t size, uint32 init_mode);
uint32 img_cfg_init_iso_config(amba_ik_iso_config_and_state_t *p_cfg_and_state);
uint32 img_cfg_init_iso_config_tables(amba_ik_iso_config_tables_t *p_tables);
uint32 img_cfg_find_iso_config_tables(const ik_cv2_liso_cfg_t *p_iso, amba_ik_cfg_tbl_description_list_t *p_tables_info, uint32 eis_mode);
uint32 img_cfg_clean_iso_config_and_tables_cache(const ik_cv2_liso_cfg_t *p_iso, const amba_ik_cfg_tbl_description_list_t *p_table_info);
uint32 img_cfg_init_hiso_config(amba_ik_hiso_config_and_state_t *p_cfg_and_state);
uint32 img_cfg_init_hiso_config_tables(amba_ik_hiso_config_tables_t *p_tables);
uint32 img_cfg_find_hiso_config_tables(const ik_cv2_hiso_cfg_t *p_iso, amba_ik_cfg_tbl_description_list_t *p_tables_info);
uint32 img_cfg_clean_hiso_config_and_tables_cache(const ik_cv2_hiso_cfg_t *p_iso, const amba_ik_cfg_tbl_description_list_t *p_table_info);
uint32 img_cfg_amalgam_data_hiso_update(const void *p_user_buffer, const void *p_iso_cfg_addr);

uint32 img_cfg_query_debug_config_size(const ikc_ability_t *p_ability, size_t *p_size);
uint32 img_cfg_amalgam_data_update(const void *p_user_buffer, const void *p_iso_cfg_addr);
uint32 img_cfg_update_config_flags(const ik_cv2_liso_cfg_t *p_iso, ikc_iso_config_update *p_iso_cfg_update);
uint32 img_cfg_size_check(void);

#endif
