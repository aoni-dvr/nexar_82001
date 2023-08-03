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
#include "AmbaDSP_ImgContextComponentIF.h"


uint32 img_exe_init_executer(uint32 context_id);
uint32 img_exe_init_safety_executer(uint32 context_id);

uint32 img_exe_query_calc_geo_buf_size(ik_query_calc_geo_buf_size_t *p_buf_size_info);
uint32 img_exe_calc_geo_settings(const ik_ability_t *p_ability, const ik_calc_geo_settings_t *p_info, const ik_calc_geo_result_t *p_result);

uint32 img_exe_warp_remap(const ik_in_warp_remap_t *p_warp_remap_in, ik_out_warp_remap_t *p_warp_remap_out);
uint32 img_exe_cawarp_remap(const ik_in_cawarp_remap_t *p_cawarp_remap_in, ik_out_cawarp_remap_t *p_cawarp_remap_out);
uint32 img_exe_query_frame_info(uint32 context_id, const uint32 ik_id, ik_query_frame_info_t *p_frame_info);
uint32 img_exe_inject_safety_error(uint32 context_id, const uint32 selection);
#endif
