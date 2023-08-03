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

#ifndef IMG_EXECUTER_COMPONENT_IF_H
#define IMG_EXECUTER_COMPONENT_IF_H

#include "ik_data_type.h"
#include "AmbaDSP_ImgArchSpec.h"
#include "AmbaDSP_ImgConfigEntity.h"
#include "AmbaDSP_ImgContextEntity.h"


typedef struct {
    ik_ability_t *ability;
    amba_ik_config_container_t *container;
    uint32 idsp_cmd_number;
    uint32 vin_cmd_number;
} amba_ik_config_containter_handler_t;

typedef struct {
    amba_ik_config_containter_handler_t handler;
    uint32 (*pack_container)(const amba_ik_config_containter_handler_t *handler, uint32 config_tag);
} amba_ik_config_container_controller_t;


uint32 img_exe_init_executer(uint32 context_id);
//uint32 img_exe_execute_invalid_config(uint32 context_id);
//uint32 img_exe_execute_first_iso_config(uint32 context_id);
//uint32 img_exe_execute_update_iso_config(uint32 context_id);
//uint32 img_exe_execute_still_hiso_config(uint32 context_id);

uint32 img_exe_construct_config_container_controller(uint32 context_id, uint32 iso_config_id, amba_ik_config_container_controller_t *p_controller);

uint32 img_exe_query_calc_geo_buf_size(ik_query_calc_geo_buf_size_t *p_buf_size_info);
uint32 img_exe_calc_geo_settings(const ik_calc_geo_settings_t *p_info, const ik_calc_geo_result_t *p_result);
uint32 img_exe_calc_warp_resources(const ik_query_warp_t *p_info, ik_query_warp_result_t *p_result);

#endif
