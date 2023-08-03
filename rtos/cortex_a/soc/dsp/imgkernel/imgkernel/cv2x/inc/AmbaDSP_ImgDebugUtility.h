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

#ifndef IMG_DEBUG_UTILITY_H
#define IMG_DEBUG_UTILITY_H
#include "ik_data_type.h"

//uint32 amba_ik_set_debug_status_by_id(uint32 id, uint8 enable_func_name, uint8 enable_func_param, uint8 enable_record);
//void amba_ik_register_filter_log(uint32 (*func)(void *data, size_t size, uint32 context_id));
uint32 ik_set_diag_case_id(uint32 context_id, uint8 diag_case_id);

uint32 ik_set_debug_level(uint32 debug_level);
uint32 ik_get_debug_level(uint32 *debug_level);

uint32 ik_set_warp_debug_level(uint32 warp_debug_level);
uint32 ik_get_warp_debug_level(uint32 *warp_debug_level);

uint32 ik_set_static_bpc_highlight(uint32 context_id, uint32 high_light_mode);
uint32 ik_get_static_bpc_highlight(uint32 context_id, uint32 *p_high_light_mode);

void ik_hook_debug_check(void);

#endif
