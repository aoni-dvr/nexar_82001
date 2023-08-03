/*
*  @file AmbaDSP_ImgExecuter.c
*
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

#include "AmbaDSP_ImgExecuterComponentIF.h"
#include "AmbaDSP_ImgUtility.h"
#include "AmbaDSP_ImgContextComponentIF.h"
#include "AmbaDSP_ImgDebugUtility.h"
#include "AmbaDSP_ImgArchComponentIF.h"

#define IK_DBG_PROFILE_EXE 0

uint32 ik_execute(uint32 context_id, ik_execute_container_t *p_execute_container)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;
    uint32 flow_idx = 0;
    uintptr addr = 0;
    ik_buffer_info_t *p_ik_buffer_info = NULL;
    const uint32 *p_ik_cfg_id;

#if IK_DBG_PROFILE_EXE
    uint32 start_tick = 0;
    uint32 end_tick = 0;
    uint32 start_sys_time = 0;
    uint32 end_sys_time = 0;

    extern UINT32 AmbaKAL_GetSysTickCount(UINT32 * pSysTickCount);
    extern UINT32 AmbaRTSL_GetOrcTimer(void);

    (void)AmbaKAL_GetSysTickCount(&start_sys_time);
    start_tick = AmbaRTSL_GetOrcTimer();
#endif

    rval = img_ctx_get_context(context_id, &p_ctx);
    if (rval == IK_OK) {
        (void)amba_ik_system_lock_mutex(context_id, IK_WAIT_FOREVER, 0u);

        if ((p_execute_container != NULL) && (p_ctx->organization.initial_flag != 0U)) {
            if(p_ctx->organization.active_cr_state.cr_running_number == p_ctx->organization.active_cr_state.max_running_number) {
                p_ctx->organization.active_cr_state.cr_running_number = 1u;
            } else {
                p_ctx->organization.active_cr_state.cr_running_number++; //ucode 0 is used for some case. start from 1
            }
            rval |= p_ctx->organization.executer_method.execute_cr(context_id);
            flow_idx = p_ctx->organization.active_cr_state.active_flow_idx;

            rval |= img_ctx_get_flow_control(context_id, flow_idx, &addr);
            (void)amba_ik_system_memcpy(&p_execute_container->p_flow_ctrl, &addr, sizeof(idsp_ik_flow_ctrl_t *));
            (void)amba_ik_system_memcpy(&p_ik_cfg_id, &addr, sizeof(uint32 *));
            //p_execute_container->ik_id = flow_idx;
            p_execute_container->ik_id = *p_ik_cfg_id;

            rval |= img_arch_get_ik_working_buffer(&p_ik_buffer_info);
            if(p_ik_buffer_info->safety_crc_enable == 1u) {
#ifndef EARLYTEST_ENV
                const idsp_crc_data_t *p_crc = NULL;
                rval |= img_ctx_get_crc_data(context_id, flow_idx, &addr);
                (void)amba_ik_system_memcpy(&p_crc, &addr, sizeof(void *));
                if((p_ctx->filters.input_param.safety_info.update_freq == 1u) ||
                   ((((uint32)p_ctx->organization.active_cr_state.cr_running_number - 1u) % p_ctx->filters.input_param.safety_info.update_freq) == 0u)) {
                    p_execute_container->ik_crc = p_crc->frame_crc32;
                } else {
                    p_execute_container->ik_crc = 0u;
                }
#endif
            } else {
                p_execute_container->ik_crc = 0u;
            }
        } else {
            if(p_execute_container == NULL) {
                amba_ik_system_print_str_5("[IK] NULL pointer in %s", __func__, DC_S, DC_S, DC_S, DC_S);
                rval |= IK_ERR_0005;
            } else {
                amba_ik_system_print_uint32_5("[IK] context_id = %d is not initial", context_id, DC_U, DC_U, DC_U, DC_U);
                rval |= IK_ERR_0002;
            }
        }

        (void)amba_ik_system_unlock_mutex(context_id, 0u);
    } else {
        amba_ik_system_print_uint32_5("[IK] can't get context", DC_U, DC_U, DC_U, DC_U, DC_U);
    }

#if IK_DBG_PROFILE_EXE
    end_tick = AmbaRTSL_GetOrcTimer();
    (void)AmbaKAL_GetSysTickCount(&end_sys_time);
    amba_ik_system_print_uint32_5("[IK][Profile ik_execute()] flow_idx %d, sys_time = %d ms, tick = %d", flow_idx, (end_sys_time - start_sys_time), (end_tick - start_tick), DC_U, DC_U);
#endif

    return rval;
}

uint32 ik_query_calc_geo_buf_size(ik_query_calc_geo_buf_size_t *p_buf_size_info)
{
    return img_exe_query_calc_geo_buf_size(p_buf_size_info);
}

uint32 ik_calc_geo_settings(const ik_ability_t *p_ability, const ik_calc_geo_settings_t *p_info, const ik_calc_geo_result_t *p_result)
{
    return img_exe_calc_geo_settings(p_ability, p_info, p_result);
}

uint32 ik_warp_remap(const ik_in_warp_remap_t *p_warp_remap_in, ik_out_warp_remap_t *p_warp_remap_out)
{
    return img_exe_warp_remap(p_warp_remap_in, p_warp_remap_out);
}

uint32 ik_cawarp_remap(const ik_in_cawarp_remap_t *p_cawarp_remap_in, ik_out_cawarp_remap_t *p_cawarp_remap_out)
{
    return img_exe_cawarp_remap(p_cawarp_remap_in, p_cawarp_remap_out);
}

uint32 ik_query_frame_info(uint32 context_id, const uint32 ik_id, ik_query_frame_info_t *p_frame_info)
{
    return img_exe_query_frame_info(context_id, ik_id, p_frame_info);
}

uint32 ik_inject_safety_error(uint32 context_id, const uint32 selection)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval = img_ctx_get_context(context_id, &p_ctx);
    if (rval == IK_OK) {
        (void)amba_ik_system_lock_mutex(context_id, IK_WAIT_FOREVER, 0u);
        rval |= img_exe_inject_safety_error(context_id, selection);
        (void)amba_ik_system_unlock_mutex(context_id, 0u);
    } else {
        amba_ik_system_print_uint32_5("[IK] can't get context", DC_U, DC_U, DC_U, DC_U, DC_U);
    }

    return rval;
}

