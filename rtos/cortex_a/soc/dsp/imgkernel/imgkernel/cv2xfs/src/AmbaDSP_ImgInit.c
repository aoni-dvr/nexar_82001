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
#ifndef EARLYTEST_ENV
#include "AmbaDSP_Monitor.h"
#endif


static inline uint32 get_round_up_diff(uintptr x, uint32 n)
{
    uintptr diff = x;
    uintptr val;
    val = ((x)+(n)-1u) & ~((n)-1u);
    return val-diff;
}

static uint32 amba_ik_arch_init_check(const ik_context_setting_t *p_setting_of_each_context, const uint32 ik_init_mode)
{
    uint32 rval = IK_OK;
    uint32 ctx = 0;

    rval |= img_arch_check_ik_working_buffer(ik_init_mode);
    if(rval == IK_OK) { // first init
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
                }
            }
        } else {
            rval = IK_ERR_0005;
            amba_ik_system_print_str_5("[IK][ERROR] ik_query_arch_memory_size() detect NULL pointer!!", NULL, NULL, NULL, NULL, NULL);
        }
    } else {
        amba_ik_system_print_str_5("[IK][ERROR] amba_ik_arch_init_check() detect re-init!!", NULL, NULL, NULL, NULL, NULL);
    }

    return rval;
}

static inline uint32 ik_default_binary_mem_fence_check(uint32 cr_idx, uint32 length, const uint8 *ptr)
{
    uint32 rval = IK_OK;
    uint32 golden = 0x58585858UL;
    uint32 size = length;
    const uint32 *ptr32 = NULL; // please do not change this to uint64 *, due to we compare 4 bytes at one time.

    (void)amba_ik_system_memcpy(&ptr32, &ptr, sizeof(void *));

    while(size >= 4u) {
        if(golden != *ptr32) {
            amba_ik_system_print_uint32_5("[IK] detect default binary corruption at CR_%d", cr_idx, DC_U, DC_U, DC_U, DC_U);
            rval |= IK_ERR_0206;
        }
        ptr32++;
        size -= 4u;
    }

#if 0 // default bin mem fence are all 4 alignment I think.
    {
        uint8 golden_8 = 0x58u;
        uint32 size_remain = length & 0x3u;
        const uint8 *ptr8 = NULL; // please do not change this to uint64 *, due to we compare 1 bytes at one time.

        (void)amba_ik_system_memcpy(&ptr8, &ptr32, sizeof(void *));

        while(size_remain > 0u) {
            if(golden_8 != *ptr8) {
                amba_ik_system_print_uint32_5("[IK] detect default binary corruption at CR_%d", cr_idx, DC_U, DC_U, DC_U, DC_U);
                rval |= IK_ERR_0206;
            }
            ptr8++;
            size_remain--;
        }
    }
#endif

    return rval;
}

static uint32 ik_default_binary_init_check(const amba_ik_context_entity_t *p_ctx)
{
    uint32 rval = IK_OK;
    const amba_ik_cr_fence_info_t *p_fence;
    uint64 def_addr = 0;
    const uint8 *p_def_sec2_addr = NULL;
    const uint8 *p_def_sec3_addr = NULL;
    const uint8 *p_def_sec4_addr = NULL;
    const uint8 *p_def_sec18_addr = NULL;
    const uint8 *p_tmp_u8 = NULL;

    p_fence = p_ctx->fence;
    (void)amba_ik_system_memcpy(&def_addr, &p_ctx->organization.p_bin_data_dram_addr, sizeof(void *));
    def_addr += (BIN_CFG_START_OFFSET + SEC_2_BIN_CFG_OFFSET);
    (void)amba_ik_system_memcpy(&p_def_sec2_addr, &def_addr, sizeof(void *));

    (void)amba_ik_system_memcpy(&def_addr, &p_ctx->organization.p_bin_data_dram_addr, sizeof(void *));
    def_addr += (BIN_CFG_START_OFFSET + SEC_3_BIN_CFG_OFFSET);
    (void)amba_ik_system_memcpy(&p_def_sec3_addr, &def_addr, sizeof(void *));

    (void)amba_ik_system_memcpy(&def_addr, &p_ctx->organization.p_bin_data_dram_addr, sizeof(void *));
    def_addr += (BIN_CFG_START_OFFSET + SEC_4_BIN_CFG_OFFSET);
    (void)amba_ik_system_memcpy(&p_def_sec4_addr, &def_addr, sizeof(void *));

    (void)amba_ik_system_memcpy(&def_addr, &p_ctx->organization.p_bin_data_dram_addr, sizeof(void *));
    def_addr += (BIN_CFG_START_OFFSET + SEC_18_BIN_CFG_OFFSET);
    (void)amba_ik_system_memcpy(&p_def_sec18_addr, &def_addr, sizeof(void *));

    p_tmp_u8 = &p_def_sec2_addr[p_fence[1].offset + CR_OFFSET_5];
    rval |= ik_default_binary_mem_fence_check(5U, p_fence[1].length, p_tmp_u8);

    p_tmp_u8 = &p_def_sec2_addr[p_fence[2].offset + CR_OFFSET_6];
    rval |= ik_default_binary_mem_fence_check(6U, p_fence[2].length, p_tmp_u8);

    p_tmp_u8 = &p_def_sec2_addr[p_fence[3].offset + CR_OFFSET_7];
    rval |= ik_default_binary_mem_fence_check(7U, p_fence[3].length, p_tmp_u8);

    p_tmp_u8 = &p_def_sec2_addr[p_fence[4].offset + CR_OFFSET_8];
    rval |= ik_default_binary_mem_fence_check(8U, p_fence[4].length, p_tmp_u8);

    p_tmp_u8 = &p_def_sec2_addr[p_fence[8].offset + CR_OFFSET_12];
    rval |= ik_default_binary_mem_fence_check(12U, p_fence[8].length, p_tmp_u8);

    p_tmp_u8 = &p_def_sec2_addr[p_fence[10].offset + CR_OFFSET_14];
    rval |= ik_default_binary_mem_fence_check(14U, p_fence[10].length, p_tmp_u8);

    p_tmp_u8 = &p_def_sec2_addr[p_fence[11].offset + CR_OFFSET_15];
    rval |= ik_default_binary_mem_fence_check(15U, p_fence[11].length, p_tmp_u8);

    p_tmp_u8 = &p_def_sec2_addr[p_fence[18].offset + CR_OFFSET_22];
    rval |= ik_default_binary_mem_fence_check(22U, p_fence[18].length, p_tmp_u8);

    p_tmp_u8 = &p_def_sec2_addr[p_fence[19].offset + CR_OFFSET_23];
    rval |= ik_default_binary_mem_fence_check(23U, p_fence[19].length, p_tmp_u8);

    p_tmp_u8 = &p_def_sec2_addr[p_fence[20].offset + CR_OFFSET_24];
    rval |= ik_default_binary_mem_fence_check(24U, p_fence[20].length, p_tmp_u8);

    p_tmp_u8 = &p_def_sec2_addr[p_fence[21].offset + CR_OFFSET_25];
    rval |= ik_default_binary_mem_fence_check(25U, p_fence[21].length, p_tmp_u8);

    p_tmp_u8 = &p_def_sec2_addr[p_fence[24].offset + CR_OFFSET_28];
    rval |= ik_default_binary_mem_fence_check(28U, p_fence[24].length, p_tmp_u8);

    p_tmp_u8 = &p_def_sec2_addr[p_fence[25].offset + CR_OFFSET_29];
    rval |= ik_default_binary_mem_fence_check(29U, p_fence[25].length, p_tmp_u8);

    p_tmp_u8 = &p_def_sec2_addr[p_fence[27].offset + CR_OFFSET_31];
    rval |= ik_default_binary_mem_fence_check(31U, p_fence[27].length, p_tmp_u8);

    p_tmp_u8 = &p_def_sec2_addr[p_fence[28].offset + CR_OFFSET_32];
    rval |= ik_default_binary_mem_fence_check(32U, p_fence[28].length, p_tmp_u8);

    p_tmp_u8 = &p_def_sec2_addr[p_fence[33].offset + CR_OFFSET_36];
    rval |= ik_default_binary_mem_fence_check(36U, p_fence[33].length, p_tmp_u8);

    p_tmp_u8 = &p_def_sec2_addr[p_fence[34].offset + CR_OFFSET_37];
    rval |= ik_default_binary_mem_fence_check(37U, p_fence[34].length, p_tmp_u8);

    p_tmp_u8 = &p_def_sec2_addr[p_fence[35].offset + CR_OFFSET_38];
    rval |= ik_default_binary_mem_fence_check(38U, p_fence[35].length, p_tmp_u8);

    p_tmp_u8 = &p_def_sec2_addr[p_fence[36].offset + CR_OFFSET_39];
    rval |= ik_default_binary_mem_fence_check(39U, p_fence[36].length, p_tmp_u8);

    p_tmp_u8 = &p_def_sec2_addr[p_fence[32].offset + CR_OFFSET_40];
    rval |= ik_default_binary_mem_fence_check(40U, p_fence[32].length, p_tmp_u8);

    p_tmp_u8 = &p_def_sec2_addr[p_fence[37].offset + CR_OFFSET_41];
    rval |= ik_default_binary_mem_fence_check(41U, p_fence[37].length, p_tmp_u8);

    p_tmp_u8 = &p_def_sec3_addr[p_fence[40].offset + CR_OFFSET_44];
    rval |= ik_default_binary_mem_fence_check(44U, p_fence[40].length, p_tmp_u8);

    p_tmp_u8 = &p_def_sec4_addr[p_fence[42].offset + CR_OFFSET_46];
    rval |= ik_default_binary_mem_fence_check(46U, p_fence[42].length, p_tmp_u8);

    p_tmp_u8 = &p_def_sec4_addr[p_fence[46].offset + CR_OFFSET_50];
    rval |= ik_default_binary_mem_fence_check(50U, p_fence[46].length, p_tmp_u8);

    p_tmp_u8 = &p_def_sec4_addr[p_fence[48].offset + CR_OFFSET_52];
    rval |= ik_default_binary_mem_fence_check(52U, p_fence[48].length, p_tmp_u8);

    p_tmp_u8 = &p_def_sec18_addr[p_fence[49].offset + CR_OFFSET_111];
    rval |= ik_default_binary_mem_fence_check(111U, p_fence[49].length, p_tmp_u8);

    p_tmp_u8 = &p_def_sec18_addr[p_fence[54].offset + CR_OFFSET_116];
    rval |= ik_default_binary_mem_fence_check(116U, p_fence[54].length, p_tmp_u8);

    p_tmp_u8 = &p_def_sec18_addr[p_fence[58].offset + CR_OFFSET_120];
    rval |= ik_default_binary_mem_fence_check(120U, p_fence[58].length, p_tmp_u8);

    p_tmp_u8 = &p_def_sec18_addr[p_fence[59].offset + CR_OFFSET_121];
    rval |= ik_default_binary_mem_fence_check(121U, p_fence[59].length, p_tmp_u8);

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

    rval |= img_ctx_query_context_memory_size(&ctx_size);
    rval |= img_cfg_query_flow_control_memory_size(&flow_ctrl_size);
    rval |= img_cfg_query_flow_tbl_memory_size(&flow_tbl_size);
    rval |= img_cfg_query_flow_tbl_list_memory_size(&flow_tbl_list_size);
    rval |= img_cfg_query_crc_data_memory_size(&crc_data_size);

    if ((p_total_ctx_size != NULL) && (p_flow_ctrl_size != NULL) && (p_flow_tbl_size != NULL) &&
        (p_flow_tbl_list_size != NULL) && (p_crc_data_size != NULL)) {
        *p_total_ctx_size = (ctx_size * p_setting_of_each_context->context_number) + IK_BUFFER_HEADER_SIZE;

        for (i = 0u; i < p_setting_of_each_context->context_number; i++) {
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

uint32 ik_init_arch(const ik_context_setting_t *p_setting_of_each_context, void *p_mem_addr, size_t mem_size, uint32 ik_init_mode)
{
    uint32 rval = IK_OK;
    uint32 ctx = 0, cr_ring = 0;
    amba_ik_context_entity_t *p_ctx;
    idsp_ik_flow_ctrl_t *p_idsp_flow_ctrl;
    amba_ik_flow_tables_t *p_ik_flow_tbl;
    amba_ik_flow_tables_list_t *p_ik_flow_tbl_list;
    idsp_crc_data_t *p_crc_data;
    uintptr usable_mem_addr;
    size_t usable_mem_size;
    size_t arch_memory = 0;
    size_t ctx_memory = 0;
    size_t flow_ctrl_memory = 0;
    size_t flow_tbl_memory = 0;
    size_t flow_tbl_list_memory = 0;
    size_t crc_data_memory = 0;
    uint32 diff;

    rval |= amba_ik_arch_init_check(p_setting_of_each_context, ik_init_mode);
    rval |= ikc_check_version(IK_VERSION_MAJOR, IKC_VERSION_MAJOR);
    ik_init_debug_print();

#if (defined(__unix__) && defined(__QNX__))
    //QNX OS needs IK implemts physical2virtual, record the virtual2physical offset in intial time.
    (void)amba_ik_system_virt_to_phys(p_mem_addr);
#endif

    if (rval == IK_OK) {
        rval = img_arch_init_architecture(p_mem_addr, p_setting_of_each_context->context_number, mem_size, ik_init_mode);
        (void) amba_ik_system_memcpy(&usable_mem_addr, &p_mem_addr, sizeof(uintptr));
        usable_mem_size = mem_size - IK_BUFFER_HEADER_SIZE;
        usable_mem_addr += IK_BUFFER_HEADER_SIZE;

        if ((usable_mem_addr & 0x1FU) > 0U) {
            amba_ik_system_print_uint32_5("[IK][ERROR] ik_init_arch() table address 0x%x is not 32 alignment", usable_mem_addr, DC_U, DC_U, DC_U, DC_U);
            diff = get_round_up_diff(usable_mem_addr, 32);
            usable_mem_addr += diff;
            usable_mem_size -= diff;
            rval = IK_ERR_0006;
        } else {
            if(ikc_register_valid_mem_range(usable_mem_addr, usable_mem_addr + usable_mem_size) != IK_OK) {
                //something wrong here, should never go to this scope!!
                amba_ik_system_print_uint32_5("[IK][ERROR] calls ikc_register_valid_mem_range(0x%x, 0x%x) fail", usable_mem_addr, usable_mem_addr + usable_mem_size, DC_U, DC_U, DC_U);
                rval = IK_ERR_0006;
            }
        }

        rval |= ik_query_arch_memory_size(p_setting_of_each_context, &ctx_memory, &flow_ctrl_memory, &flow_tbl_memory, &flow_tbl_list_memory, &crc_data_memory);
        arch_memory = ctx_memory + flow_ctrl_memory + flow_tbl_memory + flow_tbl_list_memory + crc_data_memory;

        if (arch_memory > mem_size) {
            img_arch_deinit_architecture();
            amba_ik_system_print_uint32_5("[IK][ERROR] ik_init_arch() required memory %d > usable memory %d ", arch_memory, mem_size, DC_U, DC_U, DC_U);
            rval |= IK_ERR_0007;
        } else {
            for (ctx = 0; ctx < p_setting_of_each_context->context_number; ctx++) {
                // make it 64 alignment for ctx
                diff = get_round_up_diff(usable_mem_addr, 64);
                usable_mem_addr += diff;
                usable_mem_size -= diff;
                // deploy ctx memory
                rval |= img_ctx_prepare_context_memory(ctx, usable_mem_addr, usable_mem_size);
                usable_mem_size -= sizeof(amba_ik_context_entity_t);
                usable_mem_addr += sizeof(amba_ik_context_entity_t);
                rval |= img_ctx_get_context(ctx, &p_ctx);
                if (rval == IK_OK) {
                    // prepare flow_ctrl memory
                    // make it 64 alignment for flow_ctrl
                    diff = get_round_up_diff(usable_mem_addr, 64);
                    usable_mem_addr += diff;
                    usable_mem_size -= diff;
                    (void)img_cfg_query_flow_control_memory_size(&flow_ctrl_memory);
                    for (cr_ring = 0; cr_ring < p_setting_of_each_context->cr_ring_setting[ctx].cr_ring_number; cr_ring++) {
                        rval |= img_cfg_prepare_flow_control_memory(&p_idsp_flow_ctrl, usable_mem_addr, usable_mem_size, flow_ctrl_memory, ik_init_mode);
                        (void)amba_ik_system_memcpy(&(p_ctx->flow.flow_ctrl[cr_ring]), &p_idsp_flow_ctrl, sizeof(idsp_ik_flow_ctrl_t *));
                        usable_mem_size -= flow_ctrl_memory;
                        usable_mem_addr += flow_ctrl_memory;
                    }
                    // prepare flow_tbl memory
                    // make it 128 alignment for flow_tbl
                    diff = get_round_up_diff(usable_mem_addr, 128);
                    usable_mem_addr += diff;
                    usable_mem_size -= diff;
                    (void)img_cfg_query_flow_tbl_memory_size(&flow_tbl_memory);
                    for (cr_ring = 0; cr_ring < p_setting_of_each_context->cr_ring_setting[ctx].cr_ring_number; cr_ring++) {
                        rval |= img_cfg_prepare_flow_tbl_memory(&p_ik_flow_tbl, usable_mem_addr, usable_mem_size, flow_tbl_memory, ik_init_mode);
                        (void)amba_ik_system_memcpy(&(p_ctx->flow.flow_tbl[cr_ring]), &p_ik_flow_tbl, sizeof(amba_ik_flow_tables_t *));
                        usable_mem_size -= flow_tbl_memory;
                        usable_mem_addr += flow_tbl_memory;
                    }
                    // prepare flow_tbl_list memory
                    // make it 64 alignment for flow_tbl_list
                    diff = get_round_up_diff(usable_mem_addr, 64);
                    usable_mem_addr += diff;
                    usable_mem_size -= diff;
                    (void)img_cfg_query_flow_tbl_list_memory_size(&flow_tbl_list_memory);
                    for (cr_ring = 0; cr_ring < p_setting_of_each_context->cr_ring_setting[ctx].cr_ring_number; cr_ring++) {
                        rval |= img_cfg_prepare_flow_tbl_list_memory(&p_ik_flow_tbl_list, usable_mem_addr, usable_mem_size, flow_tbl_list_memory, ik_init_mode);
                        (void)amba_ik_system_memcpy(&(p_ctx->flow.flow_tbl_list[cr_ring]), &p_ik_flow_tbl_list, sizeof(amba_ik_flow_tables_list_t *));
                        usable_mem_size -= flow_tbl_list_memory;
                        usable_mem_addr += flow_tbl_list_memory;
                    }
                    // prepare crc memory
                    // make it 64 alignment for crc
                    diff = get_round_up_diff(usable_mem_addr, 64);
                    usable_mem_addr += diff;
                    usable_mem_size -= diff;
                    (void)img_cfg_query_crc_data_memory_size(&crc_data_memory);
                    for (cr_ring = 0; cr_ring < p_setting_of_each_context->cr_ring_setting[ctx].cr_ring_number; cr_ring++) {
                        rval |= img_cfg_prepare_crc_data_memory(&p_crc_data, usable_mem_addr, usable_mem_size, crc_data_memory, ik_init_mode);
                        (void)amba_ik_system_memcpy(&(p_ctx->flow.crc_data[cr_ring]), &p_crc_data, sizeof(idsp_crc_data_t *));
                        usable_mem_size -= crc_data_memory;
                        usable_mem_addr += crc_data_memory;
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
        if(p_ik_buffer_info->safety_state == IK_SAFETY_STATE_OK) {
            if(p_ik_buffer_info->init_flag == 1u) {
                if(p_ik_buffer_info->p_bin_data_dram_addr == NULL) {
                    p_ik_buffer_info->p_bin_data_dram_addr = p_bin_data_dram_addr;
                } else {
                    amba_ik_system_print_uint32_5("[IK][ERROR] ik_init_default_binary() detect re-init!!", DC_U, DC_U, DC_U, DC_U, DC_U);
                    rval = IK_ERR_0000;
                }
            } else {
                amba_ik_system_print_uint32_5("[IK][ERROR] ik_init_default_binary() should init_arch first ", DC_U, DC_U, DC_U, DC_U, DC_U);
                rval = IK_ERR_0000;
            }
        } else {
            amba_ik_system_print_uint32_5("[IK][ERROR] ik_init_default_binary() Detect Safety ERROR... ", DC_U, DC_U, DC_U, DC_U, DC_U);
            rval = IK_ERR_0200;
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
    amba_ik_context_entity_t *p_ctx;
    ik_buffer_info_t *p_ik_buffer_info = NULL;
    uintptr addr = 0;
    idsp_ik_flow_ctrl_t *p_flow = NULL;
    amba_ik_flow_tables_t *p_flow_tbl = NULL;
    amba_ik_flow_tables_list_t *p_flow_tbl_list = NULL;
    uint32 i;

    rval |= img_arch_get_ik_working_buffer(&p_ik_buffer_info);
    if(p_ik_buffer_info->safety_state == IK_SAFETY_STATE_OK) {
        rval |= img_ctx_init_context(context_id, p_ability);
        rval |= img_exe_init_executer(context_id);

        rval |= img_ctx_get_context(context_id, &p_ctx);
        if((p_ik_buffer_info->init_mode == AMBA_IK_ARCH_HARD_INIT) && (rval == IK_OK)) {
            // cr memory fence
#if SUPPORT_CR_MEMORY_FENCE
            //init flow fence here
            (void)img_cfg_init_cr_memory_fence(p_ctx->fence);
            rval |= ik_default_binary_init_check(p_ctx);
#endif

            if(rval == IK_OK) {
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

                // cr memory fence
#if SUPPORT_CR_MEMORY_FENCE
                for(i=0u; i<p_ctx->organization.attribute.cr_ring_number; i++) {
                    rval |= img_ctx_get_flow_tbl(context_id, i, &addr);
                    (void)amba_ik_system_memcpy(&p_flow_tbl, &addr, sizeof(void *));
                    (void)img_cfg_set_cr_memory_fence(p_ctx->fence, p_flow_tbl, p_ability);
                }
#endif
            } else {
                amba_ik_system_print_uint32_5("[IK] ik_default_binary_init_check fail ctx:%d ", context_id, DC_U, DC_U, DC_U, DC_U);
                p_ik_buffer_info->safety_state = IK_SAFETY_STATE_ERROR;
                (void)img_exe_init_safety_executer(context_id);
                img_ctx_hook_safety_filter_method(&p_ctx->organization.filter_methods);

#if defined(CONFIG_ENABLE_DSP_MONITOR) && !defined(EARLYTEST_ENV)
                if(p_ik_buffer_info->safety_logic_enable == 1u) {
                    AMBA_DSP_MONITOR_ERR_NOTIFY_s ErrNotify = {0};
                    ErrNotify.ModuleId = 0x110u;
                    ErrNotify.ErrorId = rval;
                    rval |= AmbaDSP_MonitorErrorNotify(&ErrNotify);
                }
#endif
            }
        }
    } else {
        amba_ik_system_print_uint32_5("[IK][ERROR] ik_init_context() Detect Safety ERROR... ", DC_U, DC_U, DC_U, DC_U, DC_U);
        rval |= IK_ERR_0200;
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

