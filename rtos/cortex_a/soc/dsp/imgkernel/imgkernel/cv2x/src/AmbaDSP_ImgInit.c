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

#include "AmbaDSP_ImgArchSystemAPI.h"
#include "AmbaDSP_ImgArchComponentIF.h"
//#include "AmbaDSP_ImgArchSpec.h"
#include "AmbaDSP_ImgUtility.h"
#include "AmbaDSP_ImgContextEntity.h"
#include "AmbaDSP_ImgConfigEntity.h"
#include "AmbaDSP_ImgContextComponentIF.h"
#include "AmbaDSP_ImgConfigComponentIF.h"
#include "AmbaDSP_ImgExecuterComponentIF.h"
#include "AmbaDSP_ImgDebugPrint.h"


static uint32 amba_ik_arch_init_check(const ik_context_setting_t *p_setting_of_each_context)
{
    uint32 rval = IK_OK;
    uint32 ctx = 0;

    rval |= img_cfg_size_check();

    if (p_setting_of_each_context != NULL) {
        if ((p_setting_of_each_context->context_number > (uint32) MAX_CONTEXT_NUM) || (p_setting_of_each_context->context_number == 0UL)) {
            amba_ik_system_print_uint32_5("[IK] img_arch_init_architecture Fail context_number : %d !!", p_setting_of_each_context->context_number, DC_U, DC_U, DC_U, DC_U);
            rval |= IK_ERR_0008;
        } else {
            for (ctx = 0; ctx < p_setting_of_each_context->context_number; ctx++) {
                if ((p_setting_of_each_context->config_setting[ctx].config_number > MAX_CONFIG_NUM) || (p_setting_of_each_context->config_setting[ctx].config_number == 0UL)) {
                    amba_ik_system_print_uint32_5("[IK] Error, p_setting_of_each_context->config_setting[%d].config_number : %d", \
                                                  ctx, p_setting_of_each_context->config_setting[ctx].config_number, MAX_CONFIG_NUM, DC_U, DC_U);
                    rval |= IK_ERR_0008;
                }
                if (p_setting_of_each_context->config_setting[ctx].ability == NULL) {
                    amba_ik_system_print_uint32_5("[IK] Error, p_setting_of_each_context->config_setting[%d].ability : NULL", ctx, DC_U, DC_U, DC_U, DC_U);
                    rval |= IK_ERR_0008;
                } else {
                    const ik_ability_t *p_ability = p_setting_of_each_context->config_setting[ctx].ability;
                    if ((p_ability->pipe != AMBA_IK_PIPE_VIDEO) && (p_ability->pipe != AMBA_IK_PIPE_STILL)) {
                        amba_ik_system_print_uint32_5("[IK] Error, ctx_id:%d not support p_ability->pipe : %d", ctx, p_ability->pipe, DC_U, DC_U, DC_U);
                        rval |= IK_ERR_0008;
                    }
                    if(p_ability->pipe == AMBA_IK_PIPE_VIDEO) {
                        if (p_ability->video_pipe >= AMBA_IK_VIDEO_MAX) {
                            amba_ik_system_print_uint32_5("[IK] Error, ctx_id:%d not support p_ability->video_pipe : %d", ctx, p_ability->video_pipe, DC_U, DC_U, DC_U);
                            rval |= IK_ERR_0008;
                        }
                    } else {
                        if ((p_ability->still_pipe <= AMBA_IK_STILL_BASE)||(p_ability->still_pipe >= AMBA_IK_STILL_MAX)) {
                            amba_ik_system_print_uint32_5("[IK] Error, ctx_id:%d not support p_ability->still_pipe : %d", ctx, p_ability->still_pipe, DC_U, DC_U, DC_U);
                            rval |= IK_ERR_0008;
                        }
                    }
                }
            }
        }
    } else {
        rval = IK_ERR_0005;
        amba_ik_system_print_str_5("[IK][ERROR] ik_query_arch_memory_size() detect NULL pointer!!", NULL, NULL, NULL, NULL, NULL);
    }
    return rval;
}

uint32 ik_query_arch_memory_size(const ik_context_setting_t *p_setting_of_each_context, size_t *p_total_ctx_size)
{
    size_t ctx_size = 0, iso_cfg_mem_size = 0, iso_cfg_tbl_size = 0, container_mem_size = 0;
    uint32  i = 0;
    uint32 rval = IK_OK;
    uint32 total_size;

    rval |= amba_ik_arch_init_check(p_setting_of_each_context);

    if ((rval == IK_OK) && (p_total_ctx_size != NULL)) {
        rval |= img_cfg_query_container_memory_size(&container_mem_size);

        if (rval == IK_OK) {
//            single_cfg_size = iso_cfg_mem_size + iso_cfg_tbl_size + hdr_ce_cfg_mem_size + hdr_ce_cfg_tbl_size + container_mem_size;
//            for (i = 0U; i < p_setting_of_each_context->context_number; i++) {
//                *p_total_ctx_size += (single_cfg_size * p_setting_of_each_context->config_setting[i].config_number);
//            }

            total_size = IK_BUFFER_HEADER_SIZE;
            for (i = 0U; i < p_setting_of_each_context->context_number; i++) {
                (void)img_ctx_query_context_memory_size(p_setting_of_each_context->config_setting[i].ability, &ctx_size);
                (void)img_cfg_query_iso_config_memory_size(p_setting_of_each_context->config_setting[i].ability, &iso_cfg_mem_size);
                (void)img_cfg_query_iso_config_tables_memory_size(p_setting_of_each_context->config_setting[i].ability, &iso_cfg_tbl_size);
                total_size += ctx_size;
                total_size += (iso_cfg_mem_size + iso_cfg_tbl_size + container_mem_size)*p_setting_of_each_context->config_setting[i].config_number;
            }
            total_size += 512U; /* reserve additional buffer  */
            *p_total_ctx_size = total_size;
        } else {
            // misraC
        }
    } else {
        rval = IK_ERR_0005;
        amba_ik_system_print_str_5("[IK][ERROR] ik_query_arch_memory_size() detect NULL pointer!!", NULL, NULL, NULL, NULL, NULL);
    }

    return rval;
}
static uint32 sub_ik_init_arch(const ik_context_setting_t *p_setting_of_each_context, uint32 ik_init_mode, uint32 rval)
{
    uint32 ctx = 0;
    uint32 ret_value = 0;
    amba_ik_context_entity_t *p_ctx = NULL;
    ret_value = rval;
    if (ret_value == IK_OK) {
        for (ctx = 0 ; ctx < p_setting_of_each_context->context_number; ctx++) {
            ret_value |= img_ctx_get_context(ctx, &p_ctx);

            //init context init is removed from init_arch, but some paramters should be assign in here.
            if (ik_init_mode == AMBA_IK_ARCH_HARD_INIT) {
                p_ctx->organization.initial_flag = 0U;
                p_ctx->organization.attribute.config_number = p_setting_of_each_context->config_setting[ctx].config_number;
            }
        }
    }
    return ret_value;
}

uint32 ik_init_arch(const ik_context_setting_t *p_setting_of_each_context, void* p_mem_addr, size_t mem_size, uint32 ik_init_mode)
{
    uint32 rval = IK_OK;
    uint32 ctx = 0, cfg = 0;
    //ik_ability_t ability;
    amba_ik_context_entity_t *p_ctx = NULL;
    uintptr usable_mem_addr;
    size_t usable_mem_size;
    amba_ik_iso_config_and_state_t *p_amba_ik_iso_config_and_state;
    amba_ik_iso_config_tables_t *p_amba_ik_iso_config_tables;
    amba_ik_config_container_t *p_amba_ik_config_container;
    size_t arch_memory = 0;

    rval |= amba_ik_arch_init_check(p_setting_of_each_context);
    rval |= img_arch_check_ik_working_buffer();
    ik_init_debug_print();

    // To record the offset of phy2vir
    (void)amba_ikc_system_virt_to_phys(p_mem_addr);

    if (rval == IK_OK) {
        rval = img_arch_init_architecture(p_mem_addr, p_setting_of_each_context->context_number, mem_size, ik_init_mode);
        (void) amba_ik_system_memcpy(&usable_mem_addr, &p_mem_addr, sizeof(uintptr));
        usable_mem_size = mem_size - IK_BUFFER_HEADER_SIZE;
        usable_mem_addr += IK_BUFFER_HEADER_SIZE;

        if (((uintptr)usable_mem_addr & 0x1FU) > 0U) {
            amba_ik_system_print_uint32_5("[IK][ERROR] ik_init_arch() table address 0x%x is not 32 alignment", usable_mem_addr, DC_U, DC_U, DC_U, DC_U);
            usable_mem_addr = (uintptr)usable_mem_addr + (32U - ((uintptr)usable_mem_addr & 0x1FU));//(((uintptr)mem_addr + 31UL) & (UINTMAX_MAX - 0x1FUL));
            rval = IK_ERR_0006;
        }
        rval |= ik_query_arch_memory_size(p_setting_of_each_context, &arch_memory);

        if (arch_memory > mem_size) {
            img_arch_deinit_architecture();
            amba_ik_system_print_uint32_5("[IK][ERROR] ik_init_arch() required memory %d > usable memory %d ", arch_memory, mem_size, DC_U, DC_U, DC_U);
            rval |= IK_ERR_0007;
        } else {
            /*if(((uint32)sizeof(amba_ik_context_entity_t) & 0x1FU) > 0U){
                amba_ik_system_print_uint32_5("[IK] amba_ik_context_entity_t : %d \n", (uint32)sizeof(amba_ik_context_entity_t) & 0x1F, DC_U, DC_U, DC_U, DC_U);
            }
            if(((uint32)sizeof(amba_ik_iso_config_and_state_t) & 0x1FU) > 0U){
                amba_ik_system_print_uint32_5("[IK] amba_ik_iso_config_and_state_t : %d \n", (uint32)sizeof(amba_ik_iso_config_and_state_t) & 0x1F, DC_U, DC_U, DC_U, DC_U);
            }
            if(((uint32)sizeof(amba_ik_hdr_ce_config_and_state_t) & 0x1FU) > 0U){
                amba_ik_system_print_uint32_5("[IK] amba_ik_hdr_ce_config_and_state_t : %d \n", (uint32)sizeof(amba_ik_hdr_ce_config_and_state_t) & 0x1F, DC_U, DC_U, DC_U, DC_U);
            }
            if(((uint32)sizeof(amba_ik_iso_config_tables_t) & 0x1FU) > 0U){
                amba_ik_system_print_uint32_5("[IK] amba_ik_iso_config_tables_t : %d \n", (uint32)sizeof(amba_ik_iso_config_tables_t) & 0x1F, DC_U, DC_U, DC_U, DC_U);
            }
            if(((uint32)sizeof(amba_ik_hdr_ce_config_tables_t) & 0x1FU) > 0U){
                amba_ik_system_print_uint32_5("[IK] amba_ik_hdr_ce_config_tables_t : %d \n", (uint32)sizeof(amba_ik_hdr_ce_config_tables_t) & 0x1F, DC_U, DC_U, DC_U, DC_U);
            }
            if(((uint32)sizeof(amba_ik_config_container_t) & 0x1FU) > 0U){
                amba_ik_system_print_uint32_5("[IK] amba_ik_config_container_t : %d \n", (uint32)sizeof(amba_ik_config_container_t) & 0x1F, DC_U, DC_U, DC_U, DC_U);
            }*/

            for (ctx = 0 ; ctx < p_setting_of_each_context->context_number; ctx++) {
                if (usable_mem_size > sizeof(amba_ik_context_entity_t)) {
                    rval |= img_ctx_prepare_context_memory(p_setting_of_each_context->config_setting[ctx].ability, ctx, usable_mem_addr, usable_mem_size);
                    if (p_setting_of_each_context->config_setting[ctx].ability->pipe == AMBA_IK_PIPE_STILL) {
                        usable_mem_size -= sizeof(amba_ik_context_entity_t) + sizeof(amba_ik_hiso_input_parameters_t);
                        usable_mem_addr += sizeof(amba_ik_context_entity_t) + sizeof(amba_ik_hiso_input_parameters_t);
                    } else {
                        usable_mem_size -= sizeof(amba_ik_context_entity_t);
                        usable_mem_addr += sizeof(amba_ik_context_entity_t);
                    }
                    rval |= img_ctx_get_context(ctx, &p_ctx);
                    if (rval == IK_OK) {
                        for (cfg = 0 ; cfg < p_setting_of_each_context->config_setting[ctx].config_number; cfg++) {
                            rval |= img_cfg_prepare_iso_config_memory(p_setting_of_each_context->config_setting[ctx].ability, &p_amba_ik_iso_config_and_state, usable_mem_addr, usable_mem_size, ik_init_mode);
                            (void)amba_ik_system_memcpy(&(p_ctx->configs.iso_configs_and_states[cfg]),&p_amba_ik_iso_config_and_state, sizeof(amba_ik_iso_config_and_state_t *));
                            if (p_setting_of_each_context->config_setting[ctx].ability->pipe == AMBA_IK_PIPE_STILL) {
                                usable_mem_size -= sizeof(amba_ik_hiso_config_and_state_t);
                                usable_mem_addr += sizeof(amba_ik_hiso_config_and_state_t);
                            } else {
                                usable_mem_size -= sizeof(amba_ik_iso_config_and_state_t);
                                usable_mem_addr += sizeof(amba_ik_iso_config_and_state_t);
                            }
                        }
                        for (cfg = 0 ; cfg < p_setting_of_each_context->config_setting[ctx].config_number; cfg++) {
                            rval |= img_cfg_prepare_iso_config_table_memory(p_setting_of_each_context->config_setting[ctx].ability, &p_amba_ik_iso_config_tables, usable_mem_addr, usable_mem_size, ik_init_mode);
                            (void)amba_ik_system_memcpy(&p_ctx->configs.iso_tables[cfg],&p_amba_ik_iso_config_tables, sizeof(amba_ik_iso_config_tables_t *));
                            if (p_setting_of_each_context->config_setting[ctx].ability->pipe == AMBA_IK_PIPE_STILL) {
                                usable_mem_size -= sizeof(amba_ik_hiso_config_tables_t);
                                usable_mem_addr += sizeof(amba_ik_hiso_config_tables_t);
                            } else {
                                usable_mem_size -= sizeof(amba_ik_iso_config_tables_t);
                                usable_mem_addr += sizeof(amba_ik_iso_config_tables_t);
                            }
                        }
                        for (cfg = 0 ; cfg < p_setting_of_each_context->config_setting[ctx].config_number; cfg++) {
                            rval |= img_cfg_prepare_container_memory(&p_amba_ik_config_container, usable_mem_addr, usable_mem_size, ik_init_mode);
                            (void)amba_ik_system_memcpy(&p_ctx->configs.configs_container[cfg],&p_amba_ik_config_container, sizeof(amba_ik_config_container_t *));
                            usable_mem_size -= sizeof(amba_ik_config_container_t);
                            usable_mem_addr += sizeof(amba_ik_config_container_t);
                        }

                    } else {
                        amba_ik_system_print_uint32_5("[IK][ERROR] ik_init_arch() call img_ctx_get_context Fail, ctx:%d ", ctx, DC_U, DC_U, DC_U, DC_U);
                    }
                } else {
                    amba_ik_system_print_uint32_5("[IK][ERROR] usable_mem_size not enough : %d ", usable_mem_size, DC_U, DC_U, DC_U, DC_U);
                    rval |= IK_ERR_0007;
                }
            }
            rval |= sub_ik_init_arch(p_setting_of_each_context, ik_init_mode, rval);
#if 0
            if (rval == IK_OK) {
                for (cfg = 0 ; cfg < setting_of_each_context->config_setting[ctx].config_number; cfg++) {
                    rval |= img_cfg_init_iso_config_tables((amba_ik_iso_config_tables_t *)p_ctx[ctx].configs.iso_tables[cfg]);
                    rval |= img_cfg_init_hdr_ce_config_tables((amba_ik_hdr_ce_config_tables_t *)p_ctx[ctx].configs.hdr_ce_tables[cfg]);
                }
            }
#endif

        }

    }

    return rval;
}

uint32 ik_get_filter_default_values(ik_user_parameters_t *p_default_values)
{
    // TODO:
    if(p_default_values != NULL) {
        (void)amba_ik_system_memcpy(p_default_values, p_default_values, sizeof(uint8));
    }
    return IK_OK;
}

uint32 ik_init_context(uint32 context_id, const ik_ability_t *p_ability, const ik_user_parameters_t *p_default_values)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;
    amba_ik_iso_config_and_state_t *cfg_and_state;
    amba_ik_iso_config_tables_t *cfg_tbl;
    uint32 i;
    ik_buffer_info_t *p_ik_buffer_info = NULL;
    uint32 *p_running_number = NULL;
    uint32 *p_iso_running_number = NULL;

    rval = img_arch_get_ik_working_buffer(&p_ik_buffer_info);

    rval = img_ctx_init_context(context_id, p_ability, p_default_values, p_ik_buffer_info->init_mode);
    rval |= img_exe_init_executer(context_id);
    rval |= img_ctx_get_context(context_id, &p_ctx);
    if(p_ik_buffer_info->init_mode == AMBA_IK_ARCH_HARD_INIT) {
        // clean cfg and cfg tables.
        if ((p_ability->pipe == AMBA_IK_PIPE_STILL) && (p_ability->still_pipe == AMBA_IK_STILL_HISO)) {
            amba_ik_hiso_config_and_state_t *hiso_cfg_and_state;
            amba_ik_hiso_config_tables_t *hiso_cfg_tbl;
            (void)amba_ik_system_memcpy(&hiso_cfg_and_state, &p_ctx->configs.iso_configs_and_states[0], sizeof(amba_ik_hiso_config_and_state_t*));
            rval |= img_cfg_init_hiso_config(hiso_cfg_and_state);
            for (i = 0; i < p_ctx->organization.attribute.config_number; i++) {
                (void) amba_ik_system_memcpy(&hiso_cfg_tbl, &p_ctx->configs.iso_tables[i], sizeof(amba_ik_hiso_config_tables_t*));
                rval |= img_cfg_init_hiso_config_tables(hiso_cfg_tbl);
            }
        } else {
            (void)amba_ik_system_memcpy(&cfg_and_state, &p_ctx->configs.iso_configs_and_states[0], sizeof(amba_ik_iso_config_and_state_t*));
            rval |= img_cfg_init_iso_config(cfg_and_state);
            for (i = 0; i < p_ctx->organization.attribute.config_number; i++) {
                (void) amba_ik_system_memcpy(&cfg_tbl, &p_ctx->configs.iso_tables[i], sizeof(amba_ik_iso_config_tables_t*));
                rval |= img_cfg_init_iso_config_tables(cfg_tbl);
            }
        }
    }
    rval |= img_ctx_get_explicit_running_num(context_id, &p_running_number);
    rval |= img_ctx_get_explicit_iso_running_num(context_id, &p_iso_running_number);
    *p_running_number = 0xFFFFFFFFUL;
    *p_iso_running_number = 0xFFFFFFFFUL;
    if (rval != IK_OK) {
        amba_ik_system_print_uint32_5("[IK] img_ctx_init_context fail ctx:%d ", context_id, DC_U, DC_U, DC_U, DC_U);
    }

    return rval;
}

uint32 ik_get_context_ability(uint32 context_id, ik_ability_t *p_ability)
{
    return img_ctx_get_context_ability(context_id, p_ability);
}
