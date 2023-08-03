/*
*  @file AmbaDSP_ImgInit.c
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

#include "AmbaDSP_ImgArchSystemAPI.h"
#include "AmbaDSP_ImgArchComponentIF.h"
#include "AmbaDSP_ImgUtility.h"
#include "AmbaDSP_ImgContextEntity.h"
#include "AmbaDSP_ImgConfigEntity.h"
#include "AmbaDSP_ImgContextComponentIF.h"
#include "AmbaDSP_ImgConfigComponentIF.h"
#include "AmbaDSP_ImgExecuterComponentIF.h"
#include "AmbaDSP_ImgDebugPrint.h"
#include "AmbaDSP_ImgProcess.h"
#include "AmbaDSP_imgConfig.h"

static inline uint32 GetU64Msb(uintptr U64Val)
{
    return (uint32)((U64Val >> 32U) & 0xFFFFFFFFU);
}
static inline uint32 GetU64Lsb(uintptr U64Val)
{
    return (uint32)(U64Val & 0xFFFFFFFFU);
}

static uint32 amba_ik_arch_init_check(const ik_context_setting_t *p_setting_of_each_context)
{
    uint32 rval = IK_OK;
    uint32 ctx = 0;

    // first init
    if (p_setting_of_each_context != NULL) {
        if (p_setting_of_each_context->context_number > MAX_CONTEXT_NUM) {
            amba_ik_system_print_uint32_5("[IK] img_arch_init_architecture Fail context_number : %d !!", p_setting_of_each_context->context_number, DC_U, DC_U, DC_U, DC_U);
            rval |= IK_ERR_0008;
        } else {
            for (ctx = 0; ctx < p_setting_of_each_context->context_number; ctx++) {
                if (p_setting_of_each_context->cr_ring_setting[ctx].cr_ring_number > MAX_CR_RING_NUM) {
                    amba_ik_system_print_uint32_5("[IK] Error, p_setting_of_each_context->config_setting[%d].config_number : %d > MAX_CR_RING_NUM : %d", \
                                                  ctx, p_setting_of_each_context->cr_ring_setting[ctx].cr_ring_number, MAX_CR_RING_NUM, DC_U, DC_U);
                    rval |= IK_ERR_0008;
                }
                if (p_setting_of_each_context->cr_ring_setting[ctx].cr_ring_number <= 0u) {
                    amba_ik_system_print_uint32_5("[IK] Error, p_setting_of_each_context->config_setting[%d].config_number : %d <= 0", \
                                                  ctx, p_setting_of_each_context->cr_ring_setting[ctx].cr_ring_number, DC_U, DC_U, DC_U);
                    rval |= IK_ERR_0008;
                }
            }
        }
    } else {
        rval = IK_ERR_0005;
        amba_ik_system_print_str_5("[IK][ERROR] ik_query_arch_memory_size() detect NULL pointer!!", NULL, NULL, NULL, NULL, NULL);
    }

    return rval;
}

uint32 ik_query_arch_memory_size(const ik_context_setting_t *p_setting_of_each_context,
                                 size_t *p_total_ctx_size,
                                 size_t *p_flow_ctrl_size,
                                 size_t *p_flow_tbl_size,
                                 size_t *p_flow_tbl_list_size,
                                 size_t *p_crc_data_size)
{
    size_t ctx_size = 0, flow_ctrl_size = 0, flow_tbl_size = 0, flow_tbl_list_size = 0;
    size_t crc_data_size = 0;
    uint32 i = 0;
    uint32 rval = IK_OK;

    if ((p_total_ctx_size != NULL) && (p_flow_ctrl_size != NULL) && (p_flow_tbl_size != NULL) && (p_flow_tbl_list_size != NULL)) {
        *p_total_ctx_size = IK_BUFFER_HEADER_SIZE;
        for (i = 0u; i < p_setting_of_each_context->context_number; i++) {
            rval |= img_ctx_query_context_memory_size(&ctx_size);
            *p_total_ctx_size += (ctx_size * p_setting_of_each_context->context_number);

            rval |= img_cfg_query_flow_control_memory_size(&flow_ctrl_size);
            rval |= img_cfg_query_flow_tbl_memory_size(p_setting_of_each_context->cr_ring_setting[i].p_ability, &flow_tbl_size);
            rval |= img_cfg_query_flow_tbl_list_memory_size(p_setting_of_each_context->cr_ring_setting[i].p_ability, &flow_tbl_list_size);
            rval |= img_cfg_query_crc_data_memory_size(p_setting_of_each_context->cr_ring_setting[i].p_ability, &crc_data_size);

            *p_flow_ctrl_size += (flow_ctrl_size * p_setting_of_each_context->cr_ring_setting[i].cr_ring_number);
            *p_flow_tbl_size += (flow_tbl_size * p_setting_of_each_context->cr_ring_setting[i].cr_ring_number);
            *p_flow_tbl_list_size += (flow_tbl_list_size * p_setting_of_each_context->cr_ring_setting[i].cr_ring_number);
            *p_crc_data_size += (crc_data_size * p_setting_of_each_context->cr_ring_setting[i].cr_ring_number);
        }
    } else {
        rval = IK_ERR_0005;
        amba_ik_system_print_str_5("[IK][ERROR] ik_query_arch_memory_size() detect NULL pointer!!", NULL, NULL, NULL, NULL, NULL);
    }

    return rval;
}

uint32 ik_query_arch_memory_total_size(const ik_context_setting_t *p_setting_of_each_context, size_t *p_total_size)
{
    size_t ctx_size=0, flow_ctrl_size=0, flow_tbl_size=0, flow_tbl_list_size=0, crc_data_size=0;
    uint32 rval = IK_OK;

    rval = ik_query_arch_memory_size(p_setting_of_each_context, &ctx_size, &flow_ctrl_size, &flow_tbl_size, &flow_tbl_list_size, &crc_data_size);
    if (rval == IK_OK) {
        *p_total_size = ctx_size + flow_ctrl_size + flow_tbl_size + flow_tbl_list_size + crc_data_size;
    } else {
        rval = IK_ERR_0005;
        amba_ik_system_print_str_5("[IK][ERROR] ik_query_arch_memory_size() detect NULL pointer!!", NULL, NULL, NULL, NULL, NULL);
    }

    return rval;
}

uint32 ik_init_arch(const ik_context_setting_t *p_setting_of_each_context, void *p_mem_addr, size_t mem_size, uint32 ik_init_mode)
{
    uint32 rval = IK_OK;
    uint32 ctx = 0, cr_ring = 0;
    amba_ik_context_entity_t *p_ctx;
    void *p_idsp_flow_ctrl;
    void *p_ik_flow_tbl;
    void *p_ik_flow_tbl_list;
    void *p_crc_data;
    uintptr usable_mem_addr;
    size_t usable_mem_size;
    size_t arch_memory = 0;
    size_t ctx_memory = 0;
    size_t flow_ctrl_memory = 0;
    size_t flow_tbl_memory = 0;
    size_t flow_tbl_list_memory = 0;
    size_t crc_data_memory = 0;

    rval |= amba_ik_arch_init_check(p_setting_of_each_context);
    rval |= ikc_check_version(IK_VERSION_MAJOR);
    ik_init_debug_print();

    if (rval == IK_OK) {
        rval = img_arch_init_architecture(p_mem_addr, p_setting_of_each_context->context_number, mem_size, ik_init_mode);
        (void) amba_ik_system_memcpy(&usable_mem_addr, &p_mem_addr, sizeof(uintptr));
        usable_mem_size = mem_size - IK_BUFFER_HEADER_SIZE;
        usable_mem_addr += IK_BUFFER_HEADER_SIZE;

        if (((uintptr)usable_mem_addr & 0x1FU) > 0U) {
            amba_ik_system_print_uint32_5("[IK][ERROR] ik_init_arch() table address 0x%x%x is not 32 alignment", GetU64Msb(usable_mem_addr), GetU64Lsb(usable_mem_addr), DC_U, DC_U, DC_U);
            usable_mem_addr = (uintptr)usable_mem_addr + (32u - ((uintptr)usable_mem_addr & 0x1FU));
            usable_mem_size = usable_mem_size - (32u - ((uintptr)usable_mem_addr & 0x1FU));
            rval = IK_ERR_0006;
        }

        rval |= ik_query_arch_memory_size(p_setting_of_each_context, &ctx_memory, &flow_ctrl_memory, &flow_tbl_memory, &flow_tbl_list_memory, &crc_data_memory);
        arch_memory = ctx_memory + flow_ctrl_memory + flow_tbl_memory + flow_tbl_list_memory + crc_data_memory;

        if (arch_memory > mem_size) {
            img_arch_deinit_architecture();
            amba_ik_system_print_uint32_5("[IK][ERROR] ik_init_arch() required memory %d > usable memory %d ", (uint32)arch_memory, (uint32)mem_size, DC_U, DC_U, DC_U);
            rval |= IK_ERR_0007;
        } else {
            for (ctx = 0; ctx < p_setting_of_each_context->context_number; ctx++) {
                // make it 64 alignment for ctx
                if (((uintptr)usable_mem_addr & 0x3FU) > 0U) {
                    usable_mem_addr += (64u - ((uintptr)usable_mem_addr & 0x3FU));
                    usable_mem_size -= (64u - ((uintptr)usable_mem_addr & 0x3FU));
                }
                // deploy ctx memory
                rval |= img_ctx_prepare_context_memory(ctx, usable_mem_addr, usable_mem_size);
                usable_mem_size -= sizeof(amba_ik_context_entity_t);
                usable_mem_addr += sizeof(amba_ik_context_entity_t);
                rval |= img_ctx_get_context(ctx, &p_ctx);
                if (rval == IK_OK) {
                    // prepare flow_ctrl memory
                    // make it 64 alignment for flow_ctrl
                    if (((uintptr)usable_mem_addr & 0x3FU) > 0U) {
                        usable_mem_addr += (64u - ((uintptr)usable_mem_addr & 0x3FU));
                        usable_mem_size -= (64u - ((uintptr)usable_mem_addr & 0x3FU));
                    }
                    (void)img_cfg_query_flow_control_memory_size(&flow_ctrl_memory);
                    for (cr_ring = 0; cr_ring < p_setting_of_each_context->cr_ring_setting[ctx].cr_ring_number; cr_ring++) {
                        rval |= img_cfg_prepare_flow_control_memory(&p_idsp_flow_ctrl, usable_mem_addr, usable_mem_size, flow_ctrl_memory, ik_init_mode);
                        (void)amba_ik_system_memcpy(&(p_ctx->flow.flow_ctrl[cr_ring]), &p_idsp_flow_ctrl, sizeof(uintptr));
                        usable_mem_size -= flow_ctrl_memory;
                        usable_mem_addr += flow_ctrl_memory;
                    }
                    // prepare flow_tbl memory
                    // make it 128 alignment for flow_tbl
                    if (((uintptr)usable_mem_addr & 0x7FU) > 0U) {
                        usable_mem_addr += (128U - ((uintptr)usable_mem_addr & 0x7FU));
                        usable_mem_size -= (128U - ((uintptr)usable_mem_addr & 0x7FU));
                    }
                    (void)img_cfg_query_flow_tbl_memory_size(p_setting_of_each_context->cr_ring_setting[ctx].p_ability, &flow_tbl_memory);
                    for (cr_ring = 0; cr_ring < p_setting_of_each_context->cr_ring_setting[ctx].cr_ring_number; cr_ring++) {
                        rval |= img_cfg_prepare_flow_tbl_memory(&p_ik_flow_tbl, usable_mem_addr, usable_mem_size, flow_tbl_memory, ik_init_mode);
                        (void)amba_ik_system_memcpy(&(p_ctx->flow.flow_tbl[cr_ring]), &p_ik_flow_tbl, sizeof(uintptr));
                        usable_mem_size -= flow_tbl_memory;
                        usable_mem_addr += flow_tbl_memory;
                    }
                    // prepare flow_tbl_list memory
                    // make it 64 alignment for flow_tbl_list
                    if (((uintptr)usable_mem_addr & 0x3FU) > 0U) {
                        usable_mem_addr += (64u - ((uintptr)usable_mem_addr & 0x3FU));
                        usable_mem_size -= (64u - ((uintptr)usable_mem_addr & 0x3FU));
                    }
                    (void)img_cfg_query_flow_tbl_list_memory_size(p_setting_of_each_context->cr_ring_setting[ctx].p_ability, &flow_tbl_list_memory);
                    for (cr_ring = 0; cr_ring < p_setting_of_each_context->cr_ring_setting[ctx].cr_ring_number; cr_ring++) {
                        rval |= img_cfg_prepare_flow_tbl_list_memory(&p_ik_flow_tbl_list, usable_mem_addr, usable_mem_size, flow_tbl_list_memory, ik_init_mode);
                        (void)amba_ik_system_memcpy(&(p_ctx->flow.flow_tbl_list[cr_ring]), &p_ik_flow_tbl_list, sizeof(uintptr));
                        usable_mem_size -= flow_tbl_list_memory;
                        usable_mem_addr += flow_tbl_list_memory;
                    }
                    // prepare crc memory
                    if(p_setting_of_each_context->cr_ring_setting[ctx].p_ability->pipe == AMBA_IK_VIDEO_PIPE) {
                        // make it 64 alignment for crc
                        if (((uintptr)usable_mem_addr & 0x3FU) > 0U) {
                            usable_mem_addr += (64u - ((uintptr)usable_mem_addr & 0x3FU));
                            usable_mem_size -= (64u - ((uintptr)usable_mem_addr & 0x3FU));
                        }
                        (void)img_cfg_query_crc_data_memory_size(p_setting_of_each_context->cr_ring_setting[ctx].p_ability, &crc_data_memory);
                        for (cr_ring = 0; cr_ring < p_setting_of_each_context->cr_ring_setting[ctx].cr_ring_number; cr_ring++) {
                            rval |= img_cfg_prepare_crc_data_memory(&p_crc_data, usable_mem_addr, usable_mem_size, crc_data_memory, ik_init_mode);
                            (void)amba_ik_system_memcpy(&(p_ctx->flow.crc_data[cr_ring]), &p_crc_data, sizeof(uintptr));
                            usable_mem_size -= crc_data_memory;
                            usable_mem_addr += crc_data_memory;
                        }
                    }

                    //init context is removed from init_arch, but some paramters should be assign in here.
                    if (ik_init_mode == AMBA_IK_ARCH_HARD_INIT) {
                        p_ctx->organization.initial_flag = 0U;
                        p_ctx->organization.attribute.cr_ring_number = p_setting_of_each_context->cr_ring_setting[ctx].cr_ring_number;
                    }
                } else {
                    amba_ik_system_print_uint32_5("[IK][ERROR] ik_init_arch() call img_ctx_get_context Fail, ctx:%d ", ctx, DC_U, DC_U, DC_U, DC_U);
                }
            }
        }
    }

    return rval;
}

uint32 ik_init_default_binary(void *p_bin_data_dram_addr)
{
    uint32 rval = IK_OK;
    ik_buffer_info_t *p_ik_buffer_info = NULL;

    rval |= img_arch_get_ik_working_buffer(&p_ik_buffer_info);
    if (rval == IK_OK) {
        if(p_ik_buffer_info->init_flag == 1u) {
            p_ik_buffer_info->p_bin_data_dram_addr = p_bin_data_dram_addr;
        } else {
            amba_ik_system_print_uint32_5("[IK][ERROR] ik_init_default_binary() should init_arch first ", DC_U, DC_U, DC_U, DC_U, DC_U);
            rval = IK_ERR_0000;
        }
    } else {
        amba_ik_system_print_uint32_5("[IK][ERROR] ik_init_default_binary() call img_arch_get_ik_working_buffer Fail ", DC_U, DC_U, DC_U, DC_U, DC_U);
        rval = IK_ERR_0000;
    }

    return rval;
}

uint32 ik_init_context(uint32 context_id, const ik_ability_t *p_ability)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;
    ik_buffer_info_t *p_ik_buffer_info = NULL;
    uintptr addr = 0;
    idsp_flow_ctrl_t *p_flow = NULL;
    void *p_flow_tbl = NULL;
    void *p_flow_tbl_list = NULL;
    uint32 flow_idx = 0;
    uint32 *p_running_number = NULL;
    uint32 *p_iso_running_number = NULL;

    rval |= img_arch_get_ik_working_buffer(&p_ik_buffer_info);
    rval |= img_ctx_init_context(context_id, p_ability);
    rval |= img_exe_init_executer(context_id);

    rval |= img_ctx_get_context(context_id, &p_ctx);
    if(p_ik_buffer_info->init_mode == AMBA_IK_ARCH_HARD_INIT) {
        flow_idx = 0;
    } else {
        p_ctx->organization.active_cr_state.cr_running_number = p_ctx->organization.active_cr_state.cr_running_number_is_finished;
        p_ctx->organization.active_cr_state.active_flow_idx = ((uint32)p_ctx->organization.active_cr_state.cr_running_number_is_finished-1U) % p_ctx->organization.attribute.cr_ring_number;
        flow_idx = p_ctx->organization.active_cr_state.active_flow_idx;
    }
    if( (rval == IK_OK)) {
        if(p_ctx->organization.p_bin_data_dram_addr != NULL) {
            if(p_ik_buffer_info->init_mode == AMBA_IK_ARCH_HARD_INIT) {
                rval |= img_ctx_get_flow_control(context_id, 0, &addr);
                (void)amba_ik_system_memcpy(&p_flow, &addr, sizeof(void *));
                (void)img_cfg_init_flow_control(context_id, p_flow, p_ability);

                rval |= img_ctx_get_flow_tbl(context_id, 0, &addr);
                (void)amba_ik_system_memcpy(&p_flow_tbl, &addr, sizeof(void *));

                rval |= img_ctx_get_flow_tbl_list(context_id, 0, &addr);
                (void)amba_ik_system_memcpy(&p_flow_tbl_list, &addr, sizeof(void *));
                (void)img_cfg_init_flow_tbl_list(p_flow_tbl, p_flow_tbl_list, p_ability);
                // compose here
                (void)img_cfg_init_flow_tbl(p_flow_tbl, p_flow_tbl_list, p_ability, p_ctx->organization.p_bin_data_dram_addr);
                (void)img_cfg_init_null_flow_tbl_list(p_flow_tbl_list, p_ability);
            } else if(p_ik_buffer_info->init_mode == AMBA_IK_ARCH_SOFT_INIT) {
                rval |= img_ctx_get_flow_tbl_list(context_id, flow_idx, &addr);
                (void)amba_ik_system_memcpy(&p_flow_tbl_list, &addr, sizeof(void *));
                rval |= img_cfg_reinit_flow_tbl_list(p_flow_tbl_list, p_ability);
            } else {
                // misra
            }
        } else {
            amba_ik_system_print_uint32_5("[IK] img_ctx_init_context fail ctx:%d , default binary is NULL", context_id, DC_U, DC_U, DC_U, DC_U);
            rval |= IK_ERR_0009;
        }
    }
    {
        rval |= img_ctx_get_explicit_running_num(context_id, &p_running_number);
        rval |= img_ctx_get_explicit_iso_running_num(context_id, &p_iso_running_number);
        *p_running_number = 0xFFFFFFFFU;
        *p_iso_running_number = 0xFFFFFFFFU;
    }

    if (rval != IK_OK) {
        amba_ik_system_print_uint32_5("[IK] img_ctx_init_context fail ctx:%d ", context_id, DC_U, DC_U, DC_U, DC_U);
    }
    return rval;
}

uint32 ik_get_context_ability(uint32 context_id, ik_ability_t *p_ability)
{
    return img_ctx_get_context_ability(context_id, p_ability);
}

