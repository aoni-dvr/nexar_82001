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
#include "AmbaDSP_ImgConfigEntity.h"
#include "AmbaDSP_ImgConfigComponentIF.h"
#include "AmbaDSP_ImgArchSystemAPI.h"
#include "AmbaDSP_ImgContextEntity.h"
#include "AmbaDSP_ImgContextComponentIF.h"
#include "AmbaDSP_ImgProcess.h"
#include "AmbaDSP_IksUtility.h"

#define DUMP_DEBUG_COMPOSED_CFG 0

#if DUMP_DEBUG_COMPOSED_CFG
#include "stdio.h"
#include "stdlib.h"
#endif

//for misra casting.
static inline uintptr ptr2uintptr(const void *p)
{
    uintptr out;
    (void)amba_ik_system_memcpy(&out, &p, sizeof(void *));
    return out;
}

uint32 img_cfg_query_flow_control_memory_size(size_t *p_mem_size)
{
    uint32 rval = IK_OK;

    if (p_mem_size == NULL) {
        amba_ik_system_print_str_5("[IK] Detect NULL pointer!!", NULL, NULL, NULL, NULL, NULL);
        rval = IK_ERR_0005;
    } else {
        *p_mem_size = sizeof(idsp_ik_flow_ctrl_t) + (64u - (sizeof(idsp_ik_flow_ctrl_t) & 0x3FU));
    }
    return rval;
}

uint32 img_cfg_query_flow_tbl_memory_size(size_t *p_mem_size)
{
    uint32 rval = IK_OK;

    if (p_mem_size == NULL) {
        amba_ik_system_print_str_5("[IK] Detect NULL pointer!!", NULL, NULL, NULL, NULL, NULL);
        rval = IK_ERR_0005;
    } else {
        *p_mem_size = sizeof(amba_ik_flow_tables_t);
    }
    return rval;
}

uint32 img_cfg_query_flow_tbl_list_memory_size(size_t *p_mem_size)
{
    uint32 rval = IK_OK;

    if (p_mem_size == NULL) {
        amba_ik_system_print_str_5("[IK] Detect NULL pointer!!", NULL, NULL, NULL, NULL, NULL);
        rval = IK_ERR_0005;
    } else {
        *p_mem_size = sizeof(amba_ik_flow_tables_list_t);
    }
    return rval;
}

uint32 img_cfg_query_crc_data_memory_size(size_t *p_mem_size)
{
    uint32 rval = IK_OK;

    if (p_mem_size == NULL) {
        amba_ik_system_print_str_5("[IK] Detect NULL pointer!!", NULL, NULL, NULL, NULL, NULL);
        rval = IK_ERR_0005;
    } else {
        *p_mem_size = sizeof(idsp_crc_data_t);
    }
    return rval;
}

uint32 img_cfg_prepare_flow_control_memory(idsp_ik_flow_ctrl_t **p_flow_mem, uintptr mem_addr, size_t size, size_t flow_ctrl_size, uint32 init_mode)
{
    uint32 rval = IK_OK;
    idsp_ik_flow_ctrl_t *tmp_flow;

    if ((p_flow_mem == NULL) || (mem_addr == 0UL) || (size == (size_t)0)) {
        amba_ik_system_print_str_5("[IK] Detect NULL pointer!!", NULL, NULL, NULL, NULL, NULL);
        rval = IK_ERR_0005;
    } else {
        if (flow_ctrl_size < size) {
            (void)amba_ik_system_memcpy(&tmp_flow, &mem_addr, sizeof(idsp_ik_flow_ctrl_t *));
            *p_flow_mem = tmp_flow;
            if (init_mode == AMBA_IK_ARCH_HARD_INIT) {
                (void)amba_ik_system_memset(*p_flow_mem, 0x0, flow_ctrl_size);
            }
        } else {
            amba_ik_system_print_int32_5("[IK] You might allocate enough memory for flow control: allocated:%d, needed:%d!!", (int32)size, (int32)flow_ctrl_size, DC_I, DC_I, DC_I);
            rval = IK_ERR_0007;
        }
    }
    return rval;
}

uint32 img_cfg_prepare_flow_tbl_memory(amba_ik_flow_tables_t **p_flow_tbl_mem, uintptr mem_addr, size_t size, size_t flow_tbl_size, uint32 init_mode)
{
    uint32 rval = IK_OK;
    amba_ik_flow_tables_t *tmp_flow_tbl;

    if ((p_flow_tbl_mem == NULL) || (mem_addr == 0UL) || (size == (size_t)0)) {
        amba_ik_system_print_str_5("[IK] Detect NULL pointer!!", NULL, NULL, NULL, NULL, NULL);
        rval = IK_ERR_0005;
    } else {
        if (flow_tbl_size < size) {
            (void)amba_ik_system_memcpy(&tmp_flow_tbl, &mem_addr, sizeof(amba_ik_flow_tables_t *));
            *p_flow_tbl_mem = tmp_flow_tbl;
            if (init_mode == AMBA_IK_ARCH_HARD_INIT) {
                (void)amba_ik_system_memset(*p_flow_tbl_mem, 0x0, flow_tbl_size); // TBD, do we need this??
            }
        } else {
            amba_ik_system_print_int32_5("[IK] You might allocate enough memory for flow table: allocated:%d, needed:%d!!", (int32)size, (int32)flow_tbl_size, DC_I, DC_I, DC_I);
            rval = IK_ERR_0007;
        }
    }
    return rval;
}

uint32 img_cfg_prepare_flow_tbl_list_memory(amba_ik_flow_tables_list_t **p_flow_tbl_list_mem, uintptr mem_addr, size_t size, size_t flow_tbl_list_size, uint32 init_mode)
{
    uint32 rval = IK_OK;
    amba_ik_flow_tables_list_t *tmp_flow_tbl_list;

    if ((p_flow_tbl_list_mem == NULL) || (mem_addr == 0UL) || (size == (size_t)0)) {
        amba_ik_system_print_str_5("[IK] Detect NULL pointer!!", NULL, NULL, NULL, NULL, NULL);
        rval = IK_ERR_0005;
    } else {
        if (flow_tbl_list_size < size) {
            (void)amba_ik_system_memcpy(&tmp_flow_tbl_list, &mem_addr, sizeof(amba_ik_flow_tables_list_t *));
            *p_flow_tbl_list_mem = tmp_flow_tbl_list;
            if (init_mode == AMBA_IK_ARCH_HARD_INIT) {
                (void)amba_ik_system_memset(*p_flow_tbl_list_mem, 0x0, flow_tbl_list_size);
            }
        } else {
            amba_ik_system_print_int32_5("[IK] You might allocate enough memory for flow table list: allocated:%d, needed:%d!!", (int32)size, (int32)flow_tbl_list_size, DC_I, DC_I, DC_I);
            rval = IK_ERR_0007;
        }
    }
    return rval;
}

uint32 img_cfg_prepare_crc_data_memory(idsp_crc_data_t **p_crc_data_mem, uintptr mem_addr, size_t size, size_t crc_data_size, uint32 init_mode)
{
    uint32 rval = IK_OK;
    idsp_crc_data_t *tmp_crc_data;

    if ((p_crc_data_mem == NULL) || (mem_addr == 0UL) || (size == (size_t)0)) {
        amba_ik_system_print_str_5("[IK] Detect NULL pointer!!", NULL, NULL, NULL, NULL, NULL);
        rval = IK_ERR_0005;
    } else {
        if (crc_data_size < size) {
            (void)amba_ik_system_memcpy(&tmp_crc_data, &mem_addr, sizeof(idsp_crc_data_t *));
            *p_crc_data_mem = tmp_crc_data;
            if (init_mode == AMBA_IK_ARCH_HARD_INIT) {
                (void)amba_ik_system_memset(*p_crc_data_mem, 0x0, crc_data_size); // TBD, do we need this??
            }
        } else {
            amba_ik_system_print_int32_5("[IK] You might allocate enough memory for crc data: allocated:%d, needed:%d!!", (int32)size, (int32)crc_data_size, DC_I, DC_I, DC_I);
            rval = IK_ERR_0007;
        }
    }
    return rval;
}

static uint32 img_cfg_set_idsp_step_info(const ik_ability_t *p_ability, uint32 *p_num_of_steps, uint16 *p_step_cfg_mask)
{
    uint32 rval = IK_OK;
    uint32 i;
    uint32 tmp_uint32 = 0;

    if (p_ability->pipe == AMBA_IK_PIPE_VIDEO) {
        for(i=0; i<(uint32)MAX_IMG_STEP; i++) {
            p_step_cfg_mask[i] = 0;
        }

        switch (p_ability->video_pipe) {
        case AMBA_IK_VIDEO_LINEAR:
        case AMBA_IK_VIDEO_Y2Y:
            *p_num_of_steps = 1;
            tmp_uint32 = (uint32)IK_SECT_CFG_MASK_SEC2;
            tmp_uint32 |= (uint32)IK_SECT_CFG_MASK_SEC3;
            tmp_uint32 |= (uint32)IK_SECT_CFG_MASK_SEC18;
            p_step_cfg_mask[0] = (uint16)tmp_uint32;
            break;
        case AMBA_IK_VIDEO_LINEAR_CE:
        case AMBA_IK_VIDEO_HDR_EXPO_2:
        case AMBA_IK_VIDEO_HDR_EXPO_3:
            *p_num_of_steps = 1;
            tmp_uint32 = (uint32)IK_SECT_CFG_MASK_SEC2;
            tmp_uint32 |= (uint32)IK_SECT_CFG_MASK_SEC3;
            tmp_uint32 |= (uint32)IK_SECT_CFG_MASK_SEC4;
            tmp_uint32 |= (uint32)IK_SECT_CFG_MASK_SEC18;
            p_step_cfg_mask[0] = (uint16)tmp_uint32;
            break;
        default:
            amba_ik_system_print_uint32_5("[IK] Error, video pipe %d is not support now\n", p_ability->video_pipe, DC_U, DC_U, DC_U, DC_U);
            rval = IK_ERR_0008;
            break;
        }
    } else if (p_ability->pipe == AMBA_IK_PIPE_STILL) {
        amba_ik_system_print_uint32_5("[IK] Error, pipe %d is not support now\n", p_ability->pipe, DC_U, DC_U, DC_U, DC_U);
        rval = IK_ERR_0008;
    } else {
        //TBD
    }

    return rval;
}

static uint32 img_cfg_compose_sec2_cfg(void *p_sec2_cr, const void *p_bin_data_dram_addr)
{
    uint32 rval = IK_OK;
    uintptr def_addr = 0;
    const void *p_def_addr = NULL;

    (void)amba_ik_system_memcpy(&def_addr, &p_bin_data_dram_addr, sizeof(void *));
    def_addr += (BIN_CFG_START_OFFSET + SEC_2_BIN_CFG_OFFSET);
    (void)amba_ik_system_memcpy(&p_def_addr, &def_addr, sizeof(void *));

    (void)amba_ik_system_memcpy(p_sec2_cr, p_def_addr, MAX_SEC_2_CFG_SZ);

#if DUMP_DEBUG_COMPOSED_CFG
    {
        FILE *pFileDbg;
        char fname[64];

        sprintf(fname, "_ikc_compose_sec2.bin");

        pFileDbg = fopen(fname, "wb");
        if (pFileDbg == NULL) {
            fputs("File error pFileDbg\n", stderr);
            return 1;
        }

        fwrite(p_sec2_cr, sizeof(uint8), MAX_SEC_2_CFG_SZ, pFileDbg);

        fclose(pFileDbg);
    }
#endif

    return rval;
}

static uint32 img_cfg_compose_sec3_cfg(void *p_sec3_cr, const void *p_bin_data_dram_addr)
{
    uint32 rval = IK_OK;
    uintptr def_addr = 0;
    const void *p_def_addr = NULL;

    (void)amba_ik_system_memcpy(&def_addr, &p_bin_data_dram_addr, sizeof(void *));
    def_addr += (BIN_CFG_START_OFFSET + SEC_3_BIN_CFG_OFFSET);
    (void)amba_ik_system_memcpy(&p_def_addr, &def_addr, sizeof(void *));

    (void)amba_ik_system_memcpy(p_sec3_cr, p_def_addr, MAX_SEC_3_CFG_SZ);

#if DUMP_DEBUG_COMPOSED_CFG
    {
        FILE *pFileDbg;
        char fname[64];

        sprintf(fname, "_ikc_compose_sec3.bin");

        pFileDbg = fopen(fname, "wb");
        if (pFileDbg == NULL) {
            fputs("File error pFileDbg\n", stderr);
            return 1;
        }

        fwrite(p_sec3_cr, sizeof(uint8), MAX_SEC_3_CFG_SZ, pFileDbg);

        fclose(pFileDbg);
    }
#endif

    return rval;
}

static uint32 img_cfg_compose_sec4_cfg(void *p_sec4_cr, const void *p_bin_data_dram_addr)
{
    uint32 rval = IK_OK;
    uintptr def_addr = 0;
    const void *p_def_addr = NULL;

    (void)amba_ik_system_memcpy(&def_addr, &p_bin_data_dram_addr, sizeof(void *));
    def_addr += (BIN_CFG_START_OFFSET + SEC_4_BIN_CFG_OFFSET);
    (void)amba_ik_system_memcpy(&p_def_addr, &def_addr, sizeof(void *));

    (void)amba_ik_system_memcpy(p_sec4_cr, p_def_addr, MAX_SEC_4_CFG_SZ);

#if DUMP_DEBUG_COMPOSED_CFG
    {
        FILE *pFileDbg;
        char fname[64];

        sprintf(fname, "_ikc_compose_sec4.bin");

        pFileDbg = fopen(fname, "wb");
        if (pFileDbg == NULL) {
            fputs("File error pFileDbg\n", stderr);
            return 1;
        }

        fwrite(p_sec4_cr, sizeof(uint8), MAX_SEC_4_CFG_SZ, pFileDbg);

        fclose(pFileDbg);
    }
#endif

    return rval;
}

static uint32 img_cfg_compose_sec18_cfg(void *p_sec18_cr, const void *p_bin_data_dram_addr)
{
    uint32 rval = IK_OK;
    uintptr def_addr = 0;
    const void *p_def_addr = NULL;

    (void)amba_ik_system_memcpy(&def_addr, &p_bin_data_dram_addr, sizeof(void *));
    def_addr += (BIN_CFG_START_OFFSET + SEC_18_BIN_CFG_OFFSET);
    (void)amba_ik_system_memcpy(&p_def_addr, &def_addr, sizeof(void *));

    (void)amba_ik_system_memcpy(p_sec18_cr, p_def_addr, MAX_SEC_18_CFG_SZ);

#if DUMP_DEBUG_COMPOSED_CFG
    {
        FILE *pFileDbg;
        char fname[64];

        sprintf(fname, "_ikc_compose_sec18.bin");

        pFileDbg = fopen(fname, "wb");
        if (pFileDbg == NULL) {
            fputs("File error pFileDbg\n", stderr);
            return 1;
        }

        fwrite(p_sec18_cr, sizeof(uint8), MAX_SEC_18_CFG_SZ, pFileDbg);

        fclose(pFileDbg);
    }
#endif

    return rval;
}

static uint32 img_cfg_populate_flow_tbl_list(const amba_ik_flow_tables_t *p_flow_tbl, amba_ik_flow_tables_list_t *p_flow_tbl_list)
{
    uint32 rval = IK_OK;
    const uint8 *tmp_p_u8;
    const int16 *tmp_p_s16;
    const idsp_ik_aaa_data_t *tmp_p_aaa;
    const ik_query_frame_info_t *tmp_p_frame_info;

    tmp_p_u8 = p_flow_tbl->CR_buf_4;
    (void)amba_ik_system_memcpy(&p_flow_tbl_list->p_CR_buf_4, &tmp_p_u8, sizeof(void*));
    tmp_p_u8 = p_flow_tbl->CR_buf_5;
    (void)amba_ik_system_memcpy(&p_flow_tbl_list->p_CR_buf_5, &tmp_p_u8, sizeof(void*));
    tmp_p_u8 = p_flow_tbl->CR_buf_6;
    (void)amba_ik_system_memcpy(&p_flow_tbl_list->p_CR_buf_6, &tmp_p_u8, sizeof(void*));
    tmp_p_u8 = p_flow_tbl->CR_buf_7;
    (void)amba_ik_system_memcpy(&p_flow_tbl_list->p_CR_buf_7, &tmp_p_u8, sizeof(void*));
    tmp_p_u8 = p_flow_tbl->CR_buf_8;
    (void)amba_ik_system_memcpy(&p_flow_tbl_list->p_CR_buf_8, &tmp_p_u8, sizeof(void*));
    tmp_p_u8 = p_flow_tbl->CR_buf_9;
    (void)amba_ik_system_memcpy(&p_flow_tbl_list->p_CR_buf_9, &tmp_p_u8, sizeof(void*));
    tmp_p_u8 = p_flow_tbl->CR_buf_10;
    (void)amba_ik_system_memcpy(&p_flow_tbl_list->p_CR_buf_10, &tmp_p_u8, sizeof(void*));
    tmp_p_u8 = p_flow_tbl->CR_buf_11;
    (void)amba_ik_system_memcpy(&p_flow_tbl_list->p_CR_buf_11, &tmp_p_u8, sizeof(void*));
    tmp_p_u8 = p_flow_tbl->CR_buf_12;
    (void)amba_ik_system_memcpy(&p_flow_tbl_list->p_CR_buf_12, &tmp_p_u8, sizeof(void*));
    tmp_p_u8 = p_flow_tbl->CR_buf_13;
    (void)amba_ik_system_memcpy(&p_flow_tbl_list->p_CR_buf_13, &tmp_p_u8, sizeof(void*));
    tmp_p_u8 = p_flow_tbl->CR_buf_14;
    (void)amba_ik_system_memcpy(&p_flow_tbl_list->p_CR_buf_14, &tmp_p_u8, sizeof(void*));
    tmp_p_u8 = p_flow_tbl->CR_buf_15;
    (void)amba_ik_system_memcpy(&p_flow_tbl_list->p_CR_buf_15, &tmp_p_u8, sizeof(void*));
    tmp_p_u8 = p_flow_tbl->CR_buf_16;
    (void)amba_ik_system_memcpy(&p_flow_tbl_list->p_CR_buf_16, &tmp_p_u8, sizeof(void*));
    tmp_p_u8 = p_flow_tbl->CR_buf_17;
    (void)amba_ik_system_memcpy(&p_flow_tbl_list->p_CR_buf_17, &tmp_p_u8, sizeof(void*));
    tmp_p_u8 = p_flow_tbl->CR_buf_18;
    (void)amba_ik_system_memcpy(&p_flow_tbl_list->p_CR_buf_18, &tmp_p_u8, sizeof(void*));
    tmp_p_u8 = p_flow_tbl->CR_buf_19;
    (void)amba_ik_system_memcpy(&p_flow_tbl_list->p_CR_buf_19, &tmp_p_u8, sizeof(void*));
    tmp_p_u8 = p_flow_tbl->CR_buf_20;
    (void)amba_ik_system_memcpy(&p_flow_tbl_list->p_CR_buf_20, &tmp_p_u8, sizeof(void*));
    tmp_p_u8 = p_flow_tbl->CR_buf_21;
    (void)amba_ik_system_memcpy(&p_flow_tbl_list->p_CR_buf_21, &tmp_p_u8, sizeof(void*));
    tmp_p_u8 = p_flow_tbl->CR_buf_22;
    (void)amba_ik_system_memcpy(&p_flow_tbl_list->p_CR_buf_22, &tmp_p_u8, sizeof(void*));
    tmp_p_u8 = p_flow_tbl->CR_buf_23;
    (void)amba_ik_system_memcpy(&p_flow_tbl_list->p_CR_buf_23, &tmp_p_u8, sizeof(void*));
    tmp_p_u8 = p_flow_tbl->CR_buf_24;
    (void)amba_ik_system_memcpy(&p_flow_tbl_list->p_CR_buf_24, &tmp_p_u8, sizeof(void*));
    tmp_p_u8 = p_flow_tbl->CR_buf_25;
    (void)amba_ik_system_memcpy(&p_flow_tbl_list->p_CR_buf_25, &tmp_p_u8, sizeof(void*));
    tmp_p_u8 = p_flow_tbl->CR_buf_26;
    (void)amba_ik_system_memcpy(&p_flow_tbl_list->p_CR_buf_26, &tmp_p_u8, sizeof(void*));
    tmp_p_u8 = p_flow_tbl->CR_buf_27;
    (void)amba_ik_system_memcpy(&p_flow_tbl_list->p_CR_buf_27, &tmp_p_u8, sizeof(void*));
    tmp_p_u8 = p_flow_tbl->CR_buf_28;
    (void)amba_ik_system_memcpy(&p_flow_tbl_list->p_CR_buf_28, &tmp_p_u8, sizeof(void*));
    tmp_p_u8 = p_flow_tbl->CR_buf_29;
    (void)amba_ik_system_memcpy(&p_flow_tbl_list->p_CR_buf_29, &tmp_p_u8, sizeof(void*));
    tmp_p_u8 = p_flow_tbl->CR_buf_30;
    (void)amba_ik_system_memcpy(&p_flow_tbl_list->p_CR_buf_30, &tmp_p_u8, sizeof(void*));
    tmp_p_u8 = p_flow_tbl->CR_buf_31;
    (void)amba_ik_system_memcpy(&p_flow_tbl_list->p_CR_buf_31, &tmp_p_u8, sizeof(void*));
    tmp_p_u8 = p_flow_tbl->CR_buf_32;
    (void)amba_ik_system_memcpy(&p_flow_tbl_list->p_CR_buf_32, &tmp_p_u8, sizeof(void*));
    tmp_p_u8 = p_flow_tbl->CR_buf_33;
    (void)amba_ik_system_memcpy(&p_flow_tbl_list->p_CR_buf_33, &tmp_p_u8, sizeof(void*));
    tmp_p_u8 = p_flow_tbl->CR_buf_34;
    (void)amba_ik_system_memcpy(&p_flow_tbl_list->p_CR_buf_34, &tmp_p_u8, sizeof(void*));
    tmp_p_u8 = p_flow_tbl->CR_buf_35;
    (void)amba_ik_system_memcpy(&p_flow_tbl_list->p_CR_buf_35, &tmp_p_u8, sizeof(void*));
    tmp_p_u8 = p_flow_tbl->CR_buf_36;
    (void)amba_ik_system_memcpy(&p_flow_tbl_list->p_CR_buf_36, &tmp_p_u8, sizeof(void*));
    tmp_p_u8 = p_flow_tbl->CR_buf_37;
    (void)amba_ik_system_memcpy(&p_flow_tbl_list->p_CR_buf_37, &tmp_p_u8, sizeof(void*));
    tmp_p_u8 = p_flow_tbl->CR_buf_38;
    (void)amba_ik_system_memcpy(&p_flow_tbl_list->p_CR_buf_38, &tmp_p_u8, sizeof(void*));
    tmp_p_u8 = p_flow_tbl->CR_buf_39;
    (void)amba_ik_system_memcpy(&p_flow_tbl_list->p_CR_buf_39, &tmp_p_u8, sizeof(void*));
    tmp_p_u8 = p_flow_tbl->CR_buf_40;
    (void)amba_ik_system_memcpy(&p_flow_tbl_list->p_CR_buf_40, &tmp_p_u8, sizeof(void*));
    tmp_p_u8 = p_flow_tbl->CR_buf_41;
    (void)amba_ik_system_memcpy(&p_flow_tbl_list->p_CR_buf_41, &tmp_p_u8, sizeof(void*));
    tmp_p_u8 = p_flow_tbl->CR_buf_42;
    (void)amba_ik_system_memcpy(&p_flow_tbl_list->p_CR_buf_42, &tmp_p_u8, sizeof(void*));
    tmp_p_u8 = p_flow_tbl->CR_buf_43;
    (void)amba_ik_system_memcpy(&p_flow_tbl_list->p_CR_buf_43, &tmp_p_u8, sizeof(void*));
    tmp_p_u8 = p_flow_tbl->CR_buf_44;
    (void)amba_ik_system_memcpy(&p_flow_tbl_list->p_CR_buf_44, &tmp_p_u8, sizeof(void*));
    tmp_p_u8 = p_flow_tbl->CR_buf_45;
    (void)amba_ik_system_memcpy(&p_flow_tbl_list->p_CR_buf_45, &tmp_p_u8, sizeof(void*));
    tmp_p_u8 = p_flow_tbl->CR_buf_46;
    (void)amba_ik_system_memcpy(&p_flow_tbl_list->p_CR_buf_46, &tmp_p_u8, sizeof(void*));
    tmp_p_u8 = p_flow_tbl->CR_buf_47;
    (void)amba_ik_system_memcpy(&p_flow_tbl_list->p_CR_buf_47, &tmp_p_u8, sizeof(void*));
    tmp_p_u8 = p_flow_tbl->CR_buf_48;
    (void)amba_ik_system_memcpy(&p_flow_tbl_list->p_CR_buf_48, &tmp_p_u8, sizeof(void*));
    tmp_p_u8 = p_flow_tbl->CR_buf_49;
    (void)amba_ik_system_memcpy(&p_flow_tbl_list->p_CR_buf_49, &tmp_p_u8, sizeof(void*));
    tmp_p_u8 = p_flow_tbl->CR_buf_50;
    (void)amba_ik_system_memcpy(&p_flow_tbl_list->p_CR_buf_50, &tmp_p_u8, sizeof(void*));
    tmp_p_u8 = p_flow_tbl->CR_buf_51;
    (void)amba_ik_system_memcpy(&p_flow_tbl_list->p_CR_buf_51, &tmp_p_u8, sizeof(void*));
    tmp_p_u8 = p_flow_tbl->CR_buf_52;
    (void)amba_ik_system_memcpy(&p_flow_tbl_list->p_CR_buf_52, &tmp_p_u8, sizeof(void*));

    tmp_p_u8 = p_flow_tbl->CR_buf_111;
    (void)amba_ik_system_memcpy(&p_flow_tbl_list->p_CR_buf_111, &tmp_p_u8, sizeof(void*));
    tmp_p_u8 = p_flow_tbl->CR_buf_112;
    (void)amba_ik_system_memcpy(&p_flow_tbl_list->p_CR_buf_112, &tmp_p_u8, sizeof(void*));
    tmp_p_u8 = p_flow_tbl->CR_buf_113;
    (void)amba_ik_system_memcpy(&p_flow_tbl_list->p_CR_buf_113, &tmp_p_u8, sizeof(void*));
    tmp_p_u8 = p_flow_tbl->CR_buf_114;
    (void)amba_ik_system_memcpy(&p_flow_tbl_list->p_CR_buf_114, &tmp_p_u8, sizeof(void*));
    tmp_p_u8 = p_flow_tbl->CR_buf_115;
    (void)amba_ik_system_memcpy(&p_flow_tbl_list->p_CR_buf_115, &tmp_p_u8, sizeof(void*));
    tmp_p_u8 = p_flow_tbl->CR_buf_116;
    (void)amba_ik_system_memcpy(&p_flow_tbl_list->p_CR_buf_116, &tmp_p_u8, sizeof(void*));
    tmp_p_u8 = p_flow_tbl->CR_buf_117;
    (void)amba_ik_system_memcpy(&p_flow_tbl_list->p_CR_buf_117, &tmp_p_u8, sizeof(void*));
    tmp_p_u8 = p_flow_tbl->CR_buf_118;
    (void)amba_ik_system_memcpy(&p_flow_tbl_list->p_CR_buf_118, &tmp_p_u8, sizeof(void*));
    tmp_p_u8 = p_flow_tbl->CR_buf_119;
    (void)amba_ik_system_memcpy(&p_flow_tbl_list->p_CR_buf_119, &tmp_p_u8, sizeof(void*));
    tmp_p_u8 = p_flow_tbl->CR_buf_120;
    (void)amba_ik_system_memcpy(&p_flow_tbl_list->p_CR_buf_120, &tmp_p_u8, sizeof(void*));
    tmp_p_u8 = p_flow_tbl->CR_buf_121;
    (void)amba_ik_system_memcpy(&p_flow_tbl_list->p_CR_buf_121, &tmp_p_u8, sizeof(void*));

    tmp_p_s16 = p_flow_tbl->ca_warp_hor_red;
    (void)amba_ik_system_memcpy(&p_flow_tbl_list->p_ca_warp_hor_red, &tmp_p_s16, sizeof(void*));
    tmp_p_s16 = p_flow_tbl->ca_warp_hor_blue;
    (void)amba_ik_system_memcpy(&p_flow_tbl_list->p_ca_warp_hor_blue, &tmp_p_s16, sizeof(void*));
    tmp_p_s16 = p_flow_tbl->ca_warp_ver_red;
    (void)amba_ik_system_memcpy(&p_flow_tbl_list->p_ca_warp_ver_red, &tmp_p_s16, sizeof(void*));
    tmp_p_s16 = p_flow_tbl->ca_warp_ver_blue;
    (void)amba_ik_system_memcpy(&p_flow_tbl_list->p_ca_warp_ver_blue, &tmp_p_s16, sizeof(void*));
    tmp_p_s16 = p_flow_tbl->warp_hor;
    (void)amba_ik_system_memcpy(&p_flow_tbl_list->p_warp_hor, &tmp_p_s16, sizeof(void*));
    tmp_p_s16 = p_flow_tbl->warp_ver;
    (void)amba_ik_system_memcpy(&p_flow_tbl_list->p_warp_ver, &tmp_p_s16, sizeof(void*));
    tmp_p_aaa = p_flow_tbl->aaa[0];
    (void)amba_ik_system_memcpy(&p_flow_tbl_list->p_aaa, &tmp_p_aaa, sizeof(void*));
    tmp_p_frame_info = &p_flow_tbl->frame_info;
    (void)amba_ik_system_memcpy(&p_flow_tbl_list->p_frame_info, &tmp_p_frame_info, sizeof(void *));

    return rval;
}

static uint32 cfg_init_flow_cr_list(idsp_ik_flow_ctrl_t *p_flow)
{
    uint32 rval = IK_OK;
    uint32 i, j, k;

    for(i=0; i<p_flow->step_info.total_num_of_steps; i++) {
        for(j=(uint32)IK_SECT_CFG_INDEX_SEC2; j<(uint32)IK_SECT_CFG_INDEX_TOTAL; j++) {
            if(((uint32)p_flow->step_info.step_cfg_mask[i] & (1UL << j)) != 0UL) {
                if(j == (uint32)IK_SECT_CFG_INDEX_SEC2) {
                    for(k=0; k<SEC_2_CRS_COUNT; k++) {
                        p_flow->step_crs[i].sec2_crs[k].config_region_enable = 1;
                    }
                }
                if(j == (uint32)IK_SECT_CFG_INDEX_SEC3) {
                    for(k=0; k<SEC_3_CRS_COUNT; k++) {
                        p_flow->step_crs[i].sec3_crs[k].config_region_enable = 1;
                    }
                }
                if(j == (uint32)IK_SECT_CFG_INDEX_SEC4) {
                    for(k=0; k<SEC_4_CRS_COUNT; k++) {
                        p_flow->step_crs[i].sec4_crs[k].config_region_enable = 1;
                    }
                }
                if(j == (uint32)IK_SECT_CFG_INDEX_SEC18) {
                    for(k=0; k<SEC_18_CRS_COUNT; k++) {
                        p_flow->step_crs[i].sec18_crs[k].config_region_enable = 1;
                    }
                }
            }
        }
    }

    return rval;
}

uint32 img_cfg_init_flow_control(uint32 context_id, idsp_ik_flow_ctrl_t *p_flow, const ik_ability_t *p_ability)
{
    struct {
        uint32 ik_major  : 5;
        uint32 ik_minor  : 9;
        uint32 iks_minor : 9;
        uint32 ikc_minor : 9;
    } ver_t;

    uint32 rval = IK_OK;
    uint32 num_of_steps = 0u;
    uint32 ikc_minor = 0u;
    uint32 iks_minor = 0u;

    if ((p_flow == NULL) || (p_ability == NULL)) {
        amba_ik_system_print_str_5("[IK][ERROR] img_cfg_init_flow_control() Detect NULL pointer!!", NULL, NULL, NULL, NULL, NULL);
        rval = IK_ERR_0005;
    } else {
        (void)amba_ik_system_memset(p_flow, 0x0, sizeof(idsp_ik_flow_ctrl_t));
        // set step info here
        rval |= img_cfg_set_idsp_step_info(p_ability, &num_of_steps, p_flow->step_info.step_cfg_mask);
        p_flow->step_info.total_num_of_steps = (uint8)num_of_steps;

        // set header
        p_flow->header.ik_cfg_id.ability = (uint8)p_ability->video_pipe;
        p_flow->header.ik_cfg_id.pipe = p_ability->pipe;
        p_flow->header.ik_cfg_id.ctx_id = (uint8)context_id;
#ifndef EARLYTEST_ENV
        ver_t.ik_major = (uint8)IK_VERSION_MAJOR;
        ver_t.ik_minor = (uint16)IK_VERSION_MINOR;
        (void)iks_get_minor_version(&iks_minor);
        ver_t.iks_minor = (uint16)iks_minor;
        (void)ikc_get_minor_version(&ikc_minor);
        ver_t.ikc_minor = (uint16)ikc_minor;
        (void)amba_ik_system_memcpy(&p_flow->header.struct_version, &ver_t, sizeof(uint32));
#else
        //p_flow->header.struct_version = 0xBABEFACEUL;
        ver_t.ik_major = (uint8)IK_VERSION_MAJOR;
        ver_t.ik_minor = 0u;
        ver_t.ikc_minor = ikc_minor;
        ver_t.iks_minor = iks_minor;
        (void)amba_ik_system_memcpy(&p_flow->header.struct_version, &ver_t, sizeof(uint32));
#endif

        // flow_info
        p_flow->flow_info.system_mode = p_ability->pipe;
        p_flow->flow_info.system_ability = (uint8)p_ability->video_pipe;

        // set cr enable flags
        (void)cfg_init_flow_cr_list(p_flow);
    }

    return rval;
}

uint32 img_cfg_init_flow_tbl(amba_ik_flow_tables_t *p_flow_tbl, const amba_ik_flow_tables_list_t *p_flow_tbl_list,
                             const ik_ability_t *p_ability, const void *p_bin_data_dram_addr)
{
    uint32 rval = IK_OK;

    if ((p_flow_tbl == NULL) || (p_flow_tbl_list == NULL) || (p_ability == NULL)) {
        amba_ik_system_print_str_5("[IK][ERROR] img_cfg_init_flow_tbl() detect NULL pointer", NULL, NULL, NULL, NULL, NULL);
        rval |= IK_ERR_0005;
    } else {
        if (p_ability->pipe == AMBA_IK_PIPE_VIDEO) {
            if(p_ability->video_pipe < AMBA_IK_VIDEO_MAX) {
                if((p_ability->video_pipe == AMBA_IK_VIDEO_LINEAR) || (p_ability->video_pipe == AMBA_IK_VIDEO_Y2Y)) {
                    (void)img_cfg_compose_sec2_cfg(p_flow_tbl_list->p_CR_buf_4, p_bin_data_dram_addr);
                    (void)img_cfg_compose_sec3_cfg(p_flow_tbl_list->p_CR_buf_42, p_bin_data_dram_addr);
                    (void)img_cfg_compose_sec18_cfg(p_flow_tbl_list->p_CR_buf_111, p_bin_data_dram_addr);
                }
                if((p_ability->video_pipe == AMBA_IK_VIDEO_LINEAR_CE) || (p_ability->video_pipe == AMBA_IK_VIDEO_HDR_EXPO_2) ||
                   (p_ability->video_pipe == AMBA_IK_VIDEO_HDR_EXPO_3)) {
                    (void)img_cfg_compose_sec2_cfg(p_flow_tbl_list->p_CR_buf_4, p_bin_data_dram_addr);
                    (void)img_cfg_compose_sec3_cfg(p_flow_tbl_list->p_CR_buf_42, p_bin_data_dram_addr);
                    (void)img_cfg_compose_sec4_cfg(p_flow_tbl_list->p_CR_buf_45, p_bin_data_dram_addr);
                    (void)img_cfg_compose_sec18_cfg(p_flow_tbl_list->p_CR_buf_111, p_bin_data_dram_addr);
                }
                (void)amba_ik_system_memset(p_flow_tbl->ca_warp_hor_red, 0x0, sizeof(p_flow_tbl->ca_warp_hor_red));
                (void)amba_ik_system_memset(p_flow_tbl->ca_warp_hor_blue, 0x0, sizeof(p_flow_tbl->ca_warp_hor_blue));
                (void)amba_ik_system_memset(p_flow_tbl->ca_warp_ver_red, 0x0, sizeof(p_flow_tbl->ca_warp_ver_red));
                (void)amba_ik_system_memset(p_flow_tbl->ca_warp_ver_blue, 0x0, sizeof(p_flow_tbl->ca_warp_ver_blue));
                (void)amba_ik_system_memset(p_flow_tbl->warp_hor, 0x0, sizeof(p_flow_tbl->warp_hor));
                (void)amba_ik_system_memset(p_flow_tbl->warp_ver, 0x0, sizeof(p_flow_tbl->warp_ver));
                (void)amba_ik_system_memset(p_flow_tbl->aaa, 0x0, sizeof(p_flow_tbl->aaa));
                (void)amba_ik_system_memset(&p_flow_tbl->frame_info, 0x0, sizeof(ik_query_frame_info_t));
            } else {
                amba_ik_system_print_uint32_5("[IK] Error, ability %d is not support now\n", p_ability->video_pipe, DC_U, DC_U, DC_U, DC_U);
                rval = IK_ERR_0008;
            }
        } else if (p_ability->pipe == AMBA_IK_PIPE_STILL) {
            amba_ik_system_print_uint32_5("[IK] Error, pipe %d is not support now\n", p_ability->pipe, DC_U, DC_U, DC_U, DC_U);
            rval = IK_ERR_0008;
        } else {
            //TBD
        }
    }

    return rval;
}

uint32 img_cfg_init_flow_tbl_list(const amba_ik_flow_tables_t *p_flow_tbl, amba_ik_flow_tables_list_t *p_flow_tbl_list, const ik_ability_t *p_ability)
{
    uint32 rval = IK_OK;

    if ((p_flow_tbl_list == NULL) || (p_ability == NULL)) {
        amba_ik_system_print_str_5("[IK][ERROR] img_cfg_init_flow_tbl_list() detect NULL pointer", NULL, NULL, NULL, NULL, NULL);
        rval |= IK_ERR_0005;
    } else {
        if (p_ability->pipe == AMBA_IK_PIPE_VIDEO) {
            if(p_ability->video_pipe < AMBA_IK_VIDEO_MAX) {
                (void)img_cfg_populate_flow_tbl_list(p_flow_tbl, p_flow_tbl_list);
                if((p_ability->video_pipe == AMBA_IK_VIDEO_LINEAR) || (p_ability->video_pipe == AMBA_IK_VIDEO_Y2Y)) {
                    p_flow_tbl_list->p_CR_buf_45 = NULL;
                    p_flow_tbl_list->p_CR_buf_46 = NULL;
                    p_flow_tbl_list->p_CR_buf_47 = NULL;
                    p_flow_tbl_list->p_CR_buf_48 = NULL;
                    p_flow_tbl_list->p_CR_buf_49 = NULL;
                    p_flow_tbl_list->p_CR_buf_50 = NULL;
                    p_flow_tbl_list->p_CR_buf_51 = NULL;
                    p_flow_tbl_list->p_CR_buf_52 = NULL;
                }
            } else {
                amba_ik_system_print_uint32_5("[IK] Error, ability %d is not support now\n", p_ability->video_pipe, DC_U, DC_U, DC_U, DC_U);
                rval = IK_ERR_0008;
            }
        } else if (p_ability->pipe == AMBA_IK_PIPE_STILL) {
            amba_ik_system_print_uint32_5("[IK] Error, pipe %d is not support now\n", p_ability->pipe, DC_U, DC_U, DC_U, DC_U);
            rval = IK_ERR_0008;
        } else {
            //TBD
        }
    }

    return rval;
}

static void* INLINE img_cfg_uint32_physical_to_void_virtual(uint32 physical_addr)
{
    void *rval;
    const void *tmp_void_ptr;
    const void *tmp_void_ptr2;
    (void)amba_ik_system_memset(&tmp_void_ptr, 0x0, sizeof(void*));
    (void)amba_ik_system_memcpy(&tmp_void_ptr, &physical_addr, sizeof(uint32));
    tmp_void_ptr2 = amba_ik_system_phys_to_virt(tmp_void_ptr);
    (void)amba_ik_system_memcpy(&rval, &tmp_void_ptr2, sizeof(void*));
    return rval;
}

static inline void img_cfg_amalgam_data_update_calib(const idsp_ik_flow_ctrl_t *p_flow, const amba_ik_flow_tables_list_t *p_flow_tbl_list, amalgam_flow_data_t *p_amalgam_data)
{
    uintptr data_offset_base;
    const void *temp_data_addr = NULL;
    const uint8 *tmp_p_u8;
    const int16 *tmp_p_s16;
    uintptr tmp_uintptr;

    (void)amba_ik_system_memcpy(&data_offset_base, &p_amalgam_data, sizeof(uintptr));

    if (p_flow->flow_info.sbp_map_addr != 0UL) {
        temp_data_addr = img_cfg_uint32_physical_to_void_virtual(p_flow->flow_info.sbp_map_addr);
        (void)amba_ik_system_memcpy(p_amalgam_data->sbp_map, temp_data_addr, IK_AMALGAM_TABLE_SBP_SIZE);
        tmp_p_u8 = p_amalgam_data->sbp_map;
        (void)amba_ik_system_memcpy(&tmp_uintptr, &tmp_p_u8, sizeof(uintptr));
        p_amalgam_data->flow_ctrl.flow_info.sbp_map_addr = tmp_uintptr - data_offset_base;
        amba_ik_system_print_uint32_5("sbp_map_addr 0x%08x, src_addr 0x%08x ",
                                      p_amalgam_data->flow_ctrl.flow_info.sbp_map_addr, ptr2uintptr(temp_data_addr), DC_U, DC_U, DC_U);
    } else {
        amba_ik_system_print_str_5("[IK] SBP is NULL", NULL, NULL, NULL, NULL, NULL);
    }

    if (p_flow_tbl_list->p_ca_warp_hor_red != NULL) {
        (void)amba_ik_system_memcpy(p_amalgam_data->ca_warp_hor_red, p_flow_tbl_list->p_ca_warp_hor_red, sizeof(p_amalgam_data->ca_warp_hor_red));
        tmp_p_s16 = p_amalgam_data->ca_warp_hor_red;
        (void)amba_ik_system_memcpy(&tmp_uintptr, &tmp_p_s16, sizeof(uintptr));
        p_amalgam_data->flow_ctrl.calib.cawarp_horizontal_table_addr_red = tmp_uintptr - data_offset_base;
        amba_ik_system_print_uint32_5("cawarp_horizontal_table_addr_red 0x%08x, src_addr 0x%08x ",
                                      p_amalgam_data->flow_ctrl.calib.cawarp_horizontal_table_addr_red, ptr2uintptr(p_flow_tbl_list->p_ca_warp_hor_red), DC_U, DC_U, DC_U);
    } else {
        amba_ik_system_print_str_5("[IK] cawarp_hor_red is NULL", NULL, NULL, NULL, NULL, NULL);
    }

    if (p_flow_tbl_list->p_ca_warp_hor_blue != NULL) {
        (void)amba_ik_system_memcpy(p_amalgam_data->ca_warp_hor_blue, p_flow_tbl_list->p_ca_warp_hor_blue, sizeof(p_amalgam_data->ca_warp_hor_blue));
        tmp_p_s16 = p_amalgam_data->ca_warp_hor_blue;
        (void)amba_ik_system_memcpy(&tmp_uintptr, &tmp_p_s16, sizeof(uintptr));
        p_amalgam_data->flow_ctrl.calib.cawarp_horizontal_table_addr_blue = tmp_uintptr - data_offset_base;
        amba_ik_system_print_uint32_5("cawarp_horizontal_table_addr_blue 0x%08x, src_addr 0x%08x ",
                                      p_amalgam_data->flow_ctrl.calib.cawarp_horizontal_table_addr_blue, ptr2uintptr(p_flow_tbl_list->p_ca_warp_hor_blue), DC_U, DC_U, DC_U);
    } else {
        amba_ik_system_print_str_5("[IK] cawarp_hor_blue is NULL", NULL, NULL, NULL, NULL, NULL);
    }

    if (p_flow_tbl_list->p_ca_warp_ver_red != NULL) {
        (void)amba_ik_system_memcpy(p_amalgam_data->ca_warp_ver_red, p_flow_tbl_list->p_ca_warp_ver_red, sizeof(p_amalgam_data->ca_warp_ver_red));
        tmp_p_s16 = p_amalgam_data->ca_warp_ver_red;
        (void)amba_ik_system_memcpy(&tmp_uintptr, &tmp_p_s16, sizeof(uintptr));
        p_amalgam_data->flow_ctrl.calib.cawarp_vertical_table_addr_red= tmp_uintptr - data_offset_base;
        amba_ik_system_print_uint32_5("cawarp_vertical_table_addr_red 0x%08x, src_addr 0x%08x ",
                                      p_amalgam_data->flow_ctrl.calib.cawarp_vertical_table_addr_red, ptr2uintptr(p_flow_tbl_list->p_ca_warp_ver_red), DC_U, DC_U, DC_U);
    } else {
        amba_ik_system_print_str_5("[IK] cawarp_ver_red is NULL", NULL, NULL, NULL, NULL, NULL);
    }

    if (p_flow_tbl_list->p_ca_warp_ver_blue != NULL) {
        (void)amba_ik_system_memcpy(p_amalgam_data->ca_warp_ver_blue, p_flow_tbl_list->p_ca_warp_ver_blue, sizeof(p_amalgam_data->ca_warp_ver_blue));
        tmp_p_s16 = p_amalgam_data->ca_warp_ver_blue;
        (void)amba_ik_system_memcpy(&tmp_uintptr, &tmp_p_s16, sizeof(uintptr));
        p_amalgam_data->flow_ctrl.calib.cawarp_vertical_table_addr_blue = tmp_uintptr - data_offset_base;
        amba_ik_system_print_uint32_5("cawarp_vertical_table_addr_blue 0x%08x, src_addr 0x%08x ",
                                      p_amalgam_data->flow_ctrl.calib.cawarp_vertical_table_addr_blue, ptr2uintptr(p_flow_tbl_list->p_ca_warp_ver_blue), DC_U, DC_U, DC_U);
    } else {
        amba_ik_system_print_str_5("[IK] cawarp_ver_blue is NULL", NULL, NULL, NULL, NULL, NULL);
    }

    if (p_flow_tbl_list->p_warp_hor != NULL) {
        (void)amba_ik_system_memcpy(p_amalgam_data->warp_hor, p_flow_tbl_list->p_warp_hor, sizeof(p_amalgam_data->warp_hor));
        tmp_p_s16 = p_amalgam_data->warp_hor;
        (void)amba_ik_system_memcpy(&tmp_uintptr, &tmp_p_s16, sizeof(uintptr));
        p_amalgam_data->flow_ctrl.calib.warp_horizontal_table_address = tmp_uintptr - data_offset_base;
        amba_ik_system_print_uint32_5("warp_horizontal_table_addr 0x%08x, src_addr 0x%08x ",
                                      p_amalgam_data->flow_ctrl.calib.warp_horizontal_table_address, ptr2uintptr(p_flow_tbl_list->p_warp_hor), DC_U, DC_U, DC_U);
    } else {
        amba_ik_system_print_str_5("[IK] warp_hor is NULL", NULL, NULL, NULL, NULL, NULL);
    }

    if (p_flow_tbl_list->p_warp_ver != NULL) {
        (void)amba_ik_system_memcpy(p_amalgam_data->warp_ver, p_flow_tbl_list->p_warp_ver, sizeof(p_amalgam_data->warp_ver));
        tmp_p_s16 = p_amalgam_data->warp_ver;
        (void)amba_ik_system_memcpy(&tmp_uintptr, &tmp_p_s16, sizeof(uintptr));
        p_amalgam_data->flow_ctrl.calib.warp_vertical_table_address = tmp_uintptr - data_offset_base;
        amba_ik_system_print_uint32_5("warp_vertical_table_addr 0x%08x, src_addr 0x%08x ",
                                      p_amalgam_data->flow_ctrl.calib.warp_vertical_table_address, ptr2uintptr(p_flow_tbl_list->p_warp_ver), DC_U, DC_U, DC_U);
    } else {
        amba_ik_system_print_str_5("[IK] warp_ver is NULL", NULL, NULL, NULL, NULL, NULL);
    }

}
uint32 img_cfg_amalgam_data_update(const idsp_ik_flow_ctrl_t *p_flow, const amba_ik_flow_tables_list_t *p_flow_tbl_list, const void *p_user_buffer)
{
    uint32 rval = 0;
    amalgam_flow_data_t *p_amalgam_data;
    uintptr data_offset_base;
    uint32 filter_offset;
    const uint8 *tmp_p_u8;
    uintptr sec2_cfg_buf_offset;
    uintptr sec3_cfg_buf_offset;
    uintptr sec4_cfg_buf_offset;
    uintptr sec18_cfg_buf_offset;
    void* p_sec2_cfg_buf;
    void* p_sec3_cfg_buf;
    void* p_sec4_cfg_buf;
    void* p_sec18_cfg_buf;
    uintptr tmp_uintptr;

    (void)amba_ik_system_memcpy(&p_amalgam_data, &p_user_buffer, sizeof(amalgam_flow_data_t *));
    (void)amba_ik_system_memcpy(&data_offset_base, &p_amalgam_data, sizeof(uintptr));
    (void)amba_ik_system_memset(p_amalgam_data, 0x0, sizeof(amalgam_flow_data_t));
    (void)amba_ik_system_memcpy(&p_amalgam_data->flow_ctrl, p_flow, sizeof(idsp_ik_flow_ctrl_t));

    img_cfg_amalgam_data_update_calib(p_flow, p_flow_tbl_list, p_amalgam_data);//complexity fix.

    if (p_flow_tbl_list->p_aaa != NULL) {
        const idsp_ik_aaa_data_t *p_tmp_aaa;
        (void)amba_ik_system_memcpy(p_amalgam_data->aaa, p_flow_tbl_list->p_aaa, sizeof(p_amalgam_data->aaa));
        p_tmp_aaa = p_amalgam_data->aaa[0];
        (void)amba_ik_system_memcpy(&tmp_uintptr, &p_tmp_aaa, sizeof(uintptr));
        p_amalgam_data->flow_ctrl.stitch.aaa_info_daddr = tmp_uintptr - data_offset_base;
        amba_ik_system_print_uint32_5("aaa_info_daddr 0x%08x, src_addr 0x%08x ",
                                      p_amalgam_data->flow_ctrl.stitch.aaa_info_daddr, ptr2uintptr(p_flow_tbl_list->p_aaa), DC_U, DC_U, DC_U);
    } else {
        amba_ik_system_print_str_5("[IK] AAA is NULL", NULL, NULL, NULL, NULL, NULL);
    }

    // IKC does not use IDSP_CONFIG_HDR_SIZE, however, IKS does, in order to keep it on the same page between IKC and IKS,
    tmp_p_u8 = p_amalgam_data->sec2_cfg_buf;
    (void)amba_ik_system_memcpy(&sec2_cfg_buf_offset, &tmp_p_u8, sizeof(uintptr));
    tmp_p_u8 = p_amalgam_data->sec3_cfg_buf;
    (void)amba_ik_system_memcpy(&sec3_cfg_buf_offset, &tmp_p_u8, sizeof(uintptr));
    tmp_p_u8 = p_amalgam_data->sec4_cfg_buf;
    (void)amba_ik_system_memcpy(&sec4_cfg_buf_offset, &tmp_p_u8, sizeof(uintptr));
    tmp_p_u8 = p_amalgam_data->sec18_cfg_buf;
    (void)amba_ik_system_memcpy(&sec18_cfg_buf_offset, &tmp_p_u8, sizeof(uintptr));
    filter_offset = CR_OFFSET_4;
    tmp_uintptr = sec2_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec2_cfg_buf, &tmp_uintptr, sizeof(void *));
    (void)amba_ik_system_memcpy(p_sec2_cfg_buf, p_flow_tbl_list->p_CR_buf_4, CR_SIZE_4);
    p_amalgam_data->flow_ctrl.step_crs[0].sec2_crs[0].config_region_addr = (tmp_uintptr - data_offset_base) >> 7UL;
    amba_ik_system_print_uint32_5("Sec2 cr[0] addr 0x%08x, offset %d, src_addr 0x%08x ",
                                  p_amalgam_data->flow_ctrl.step_crs[0].sec2_crs[0].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_4), DC_U, DC_U);

    filter_offset = CR_OFFSET_5;
    tmp_uintptr = sec2_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec2_cfg_buf, &tmp_uintptr, sizeof(void *));
    (void)amba_ik_system_memcpy(p_sec2_cfg_buf, p_flow_tbl_list->p_CR_buf_5, CR_SIZE_5);
    p_amalgam_data->flow_ctrl.step_crs[0].sec2_crs[1].config_region_addr = (tmp_uintptr - data_offset_base) >> 7UL;
    amba_ik_system_print_uint32_5("Sec2 cr[1] addr 0x%08x, offset %d, src_addr 0x%08x ",
                                  p_amalgam_data->flow_ctrl.step_crs[0].sec2_crs[1].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_5), DC_U, DC_U);

    filter_offset = CR_OFFSET_6;
    tmp_uintptr = sec2_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec2_cfg_buf, &tmp_uintptr, sizeof(void *));
    (void)amba_ik_system_memcpy(p_sec2_cfg_buf, p_flow_tbl_list->p_CR_buf_6, CR_SIZE_6);
    p_amalgam_data->flow_ctrl.step_crs[0].sec2_crs[2].config_region_addr = (tmp_uintptr - data_offset_base) >> 7UL;
    amba_ik_system_print_uint32_5("Sec2 cr[2] addr 0x%08x, offset %d, src_addr 0x%08x ",
                                  p_amalgam_data->flow_ctrl.step_crs[0].sec2_crs[2].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_6), DC_U, DC_U);

    filter_offset = CR_OFFSET_7;
    tmp_uintptr = sec2_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec2_cfg_buf, &tmp_uintptr, sizeof(void *));
    (void)amba_ik_system_memcpy(p_sec2_cfg_buf, p_flow_tbl_list->p_CR_buf_7, CR_SIZE_7);
    p_amalgam_data->flow_ctrl.step_crs[0].sec2_crs[3].config_region_addr = (tmp_uintptr - data_offset_base) >> 7UL;
    amba_ik_system_print_uint32_5("Sec2 cr[3] addr 0x%08x, offset %d, src_addr 0x%08x ",
                                  p_amalgam_data->flow_ctrl.step_crs[0].sec2_crs[3].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_7), DC_U, DC_U);

    filter_offset = CR_OFFSET_8;
    tmp_uintptr = sec2_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec2_cfg_buf, &tmp_uintptr, sizeof(void *));
    (void)amba_ik_system_memcpy(p_sec2_cfg_buf, p_flow_tbl_list->p_CR_buf_8, CR_SIZE_8);
    p_amalgam_data->flow_ctrl.step_crs[0].sec2_crs[4].config_region_addr = (tmp_uintptr - data_offset_base) >> 7UL;
    amba_ik_system_print_uint32_5("Sec2 cr[4] addr 0x%08x, offset %d, src_addr 0x%08x ",
                                  p_amalgam_data->flow_ctrl.step_crs[0].sec2_crs[4].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_8), DC_U, DC_U);

    filter_offset = CR_OFFSET_9;
    tmp_uintptr = sec2_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec2_cfg_buf, &tmp_uintptr, sizeof(void *));
    (void)amba_ik_system_memcpy(p_sec2_cfg_buf, p_flow_tbl_list->p_CR_buf_9, CR_SIZE_9);
    p_amalgam_data->flow_ctrl.step_crs[0].sec2_crs[5].config_region_addr = (tmp_uintptr - data_offset_base) >> 7UL;
    amba_ik_system_print_uint32_5("Sec2 cr[5] addr 0x%08x, offset %d, src_addr 0x%08x ",
                                  p_amalgam_data->flow_ctrl.step_crs[0].sec2_crs[5].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_9), DC_U, DC_U);

    filter_offset = CR_OFFSET_10;
    tmp_uintptr = sec2_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec2_cfg_buf, &tmp_uintptr, sizeof(void *));
    (void)amba_ik_system_memcpy(p_sec2_cfg_buf, p_flow_tbl_list->p_CR_buf_10, CR_SIZE_10);
    p_amalgam_data->flow_ctrl.step_crs[0].sec2_crs[6].config_region_addr = (tmp_uintptr - data_offset_base) >> 7UL;
    amba_ik_system_print_uint32_5("Sec2 cr[6] addr 0x%08x, offset %d, src_addr 0x%08x ",
                                  p_amalgam_data->flow_ctrl.step_crs[0].sec2_crs[6].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_10), DC_U, DC_U);

    filter_offset = CR_OFFSET_11;
    tmp_uintptr = sec2_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec2_cfg_buf, &tmp_uintptr, sizeof(void *));
    (void)amba_ik_system_memcpy(p_sec2_cfg_buf, p_flow_tbl_list->p_CR_buf_11, CR_SIZE_11);
    p_amalgam_data->flow_ctrl.step_crs[0].sec2_crs[7].config_region_addr = (tmp_uintptr - data_offset_base) >> 7UL;
    amba_ik_system_print_uint32_5("Sec2 cr[7] addr 0x%08x, offset %d, src_addr 0x%08x ",
                                  p_amalgam_data->flow_ctrl.step_crs[0].sec2_crs[7].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_11), DC_U, DC_U);

    filter_offset = CR_OFFSET_12;
    tmp_uintptr = sec2_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec2_cfg_buf, &tmp_uintptr, sizeof(void *));
    (void)amba_ik_system_memcpy(p_sec2_cfg_buf, p_flow_tbl_list->p_CR_buf_12, CR_SIZE_12);
    p_amalgam_data->flow_ctrl.step_crs[0].sec2_crs[8].config_region_addr = (tmp_uintptr - data_offset_base) >> 7UL;
    amba_ik_system_print_uint32_5("Sec2 cr[8] addr 0x%08x, offset %d, src_addr 0x%08x ",
                                  p_amalgam_data->flow_ctrl.step_crs[0].sec2_crs[8].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_12), DC_U, DC_U);

    filter_offset = CR_OFFSET_13;
    tmp_uintptr = sec2_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec2_cfg_buf, &tmp_uintptr, sizeof(void *));
    (void)amba_ik_system_memcpy(p_sec2_cfg_buf, p_flow_tbl_list->p_CR_buf_13, CR_SIZE_13);
    p_amalgam_data->flow_ctrl.step_crs[0].sec2_crs[9].config_region_addr = (tmp_uintptr - data_offset_base) >> 7UL;
    amba_ik_system_print_uint32_5("Sec2 cr[9] addr 0x%08x, offset %d, src_addr 0x%08x ",
                                  p_amalgam_data->flow_ctrl.step_crs[0].sec2_crs[9].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_13), DC_U, DC_U);

    filter_offset = CR_OFFSET_14;
    tmp_uintptr = sec2_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec2_cfg_buf, &tmp_uintptr, sizeof(void *));
    (void)amba_ik_system_memcpy(p_sec2_cfg_buf, p_flow_tbl_list->p_CR_buf_14, CR_SIZE_14);
    p_amalgam_data->flow_ctrl.step_crs[0].sec2_crs[10].config_region_addr = (tmp_uintptr - data_offset_base) >> 7UL;
    amba_ik_system_print_uint32_5("Sec2 cr[10] addr 0x%08x, offset %d, src_addr 0x%08x ",
                                  p_amalgam_data->flow_ctrl.step_crs[0].sec2_crs[10].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_14), DC_U, DC_U);

    filter_offset = CR_OFFSET_15;
    tmp_uintptr = sec2_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec2_cfg_buf, &tmp_uintptr, sizeof(void *));
    (void)amba_ik_system_memcpy(p_sec2_cfg_buf, p_flow_tbl_list->p_CR_buf_15, CR_SIZE_15);
    p_amalgam_data->flow_ctrl.step_crs[0].sec2_crs[11].config_region_addr = (tmp_uintptr - data_offset_base) >> 7UL;
    amba_ik_system_print_uint32_5("Sec2 cr[11] addr 0x%08x, offset %d, src_addr 0x%08x ",
                                  p_amalgam_data->flow_ctrl.step_crs[0].sec2_crs[11].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_15), DC_U, DC_U);

    filter_offset = CR_OFFSET_16;
    tmp_uintptr = sec2_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec2_cfg_buf, &tmp_uintptr, sizeof(void *));
    (void)amba_ik_system_memcpy(p_sec2_cfg_buf, p_flow_tbl_list->p_CR_buf_16, CR_SIZE_16);
    p_amalgam_data->flow_ctrl.step_crs[0].sec2_crs[12].config_region_addr = (tmp_uintptr - data_offset_base) >> 7UL;
    amba_ik_system_print_uint32_5("Sec2 cr[12] addr 0x%08x, offset %d, src_addr 0x%08x ",
                                  p_amalgam_data->flow_ctrl.step_crs[0].sec2_crs[12].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_16), DC_U, DC_U);

    filter_offset = CR_OFFSET_17;
    tmp_uintptr = sec2_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec2_cfg_buf, &tmp_uintptr, sizeof(void *));
    (void)amba_ik_system_memcpy(p_sec2_cfg_buf, p_flow_tbl_list->p_CR_buf_17, CR_SIZE_17);
    p_amalgam_data->flow_ctrl.step_crs[0].sec2_crs[13].config_region_addr = (tmp_uintptr - data_offset_base) >> 7UL;
    amba_ik_system_print_uint32_5("Sec2 cr[13] addr 0x%08x, offset %d, src_addr 0x%08x ",
                                  p_amalgam_data->flow_ctrl.step_crs[0].sec2_crs[13].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_17), DC_U, DC_U);

    filter_offset = CR_OFFSET_18;
    tmp_uintptr = sec2_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec2_cfg_buf, &tmp_uintptr, sizeof(void *));
    (void)amba_ik_system_memcpy(p_sec2_cfg_buf, p_flow_tbl_list->p_CR_buf_18, CR_SIZE_18);
    p_amalgam_data->flow_ctrl.step_crs[0].sec2_crs[14].config_region_addr = (tmp_uintptr - data_offset_base) >> 7UL;
    amba_ik_system_print_uint32_5("Sec2 cr[14] addr 0x%08x, offset %d, src_addr 0x%08x ",
                                  p_amalgam_data->flow_ctrl.step_crs[0].sec2_crs[14].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_18), DC_U, DC_U);

    filter_offset = CR_OFFSET_19;
    tmp_uintptr = sec2_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec2_cfg_buf, &tmp_uintptr, sizeof(void *));
    (void)amba_ik_system_memcpy(p_sec2_cfg_buf, p_flow_tbl_list->p_CR_buf_19, CR_SIZE_19);
    p_amalgam_data->flow_ctrl.step_crs[0].sec2_crs[15].config_region_addr = (tmp_uintptr - data_offset_base) >> 7UL;
    amba_ik_system_print_uint32_5("Sec2 cr[15] addr 0x%08x, offset %d, src_addr 0x%08x ",
                                  p_amalgam_data->flow_ctrl.step_crs[0].sec2_crs[15].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_19), DC_U, DC_U);

    filter_offset = CR_OFFSET_20;
    tmp_uintptr = sec2_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec2_cfg_buf, &tmp_uintptr, sizeof(void *));
    (void)amba_ik_system_memcpy(p_sec2_cfg_buf, p_flow_tbl_list->p_CR_buf_20, CR_SIZE_20);
    p_amalgam_data->flow_ctrl.step_crs[0].sec2_crs[16].config_region_addr = (tmp_uintptr - data_offset_base) >> 7UL;
    amba_ik_system_print_uint32_5("Sec2 cr[16] addr 0x%08x, offset %d, src_addr 0x%08x ",
                                  p_amalgam_data->flow_ctrl.step_crs[0].sec2_crs[16].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_20), DC_U, DC_U);

    filter_offset = CR_OFFSET_21;
    tmp_uintptr = sec2_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec2_cfg_buf, &tmp_uintptr, sizeof(void *));
    (void)amba_ik_system_memcpy(p_sec2_cfg_buf, p_flow_tbl_list->p_CR_buf_21, CR_SIZE_21);
    p_amalgam_data->flow_ctrl.step_crs[0].sec2_crs[17].config_region_addr = (tmp_uintptr - data_offset_base) >> 7UL;
    amba_ik_system_print_uint32_5("Sec2 cr[17] addr 0x%08x, offset %d, src_addr 0x%08x ",
                                  p_amalgam_data->flow_ctrl.step_crs[0].sec2_crs[17].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_21), DC_U, DC_U);

    filter_offset = CR_OFFSET_22;
    tmp_uintptr = sec2_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec2_cfg_buf, &tmp_uintptr, sizeof(void *));
    (void)amba_ik_system_memcpy(p_sec2_cfg_buf, p_flow_tbl_list->p_CR_buf_22, CR_SIZE_22);
    p_amalgam_data->flow_ctrl.step_crs[0].sec2_crs[18].config_region_addr = (tmp_uintptr - data_offset_base) >> 7UL;
    amba_ik_system_print_uint32_5("Sec2 cr[18] addr 0x%08x, offset %d, src_addr 0x%08x ",
                                  p_amalgam_data->flow_ctrl.step_crs[0].sec2_crs[18].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_22), DC_U, DC_U);

    filter_offset = CR_OFFSET_23;
    tmp_uintptr = sec2_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec2_cfg_buf, &tmp_uintptr, sizeof(void *));
    (void)amba_ik_system_memcpy(p_sec2_cfg_buf, p_flow_tbl_list->p_CR_buf_23, CR_SIZE_23);
    p_amalgam_data->flow_ctrl.step_crs[0].sec2_crs[19].config_region_addr = (tmp_uintptr - data_offset_base) >> 7UL;
    amba_ik_system_print_uint32_5("Sec2 cr[19] addr 0x%08x, offset %d, src_addr 0x%08x ",
                                  p_amalgam_data->flow_ctrl.step_crs[0].sec2_crs[19].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_23), DC_U, DC_U);

    filter_offset = CR_OFFSET_24;
    tmp_uintptr = sec2_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec2_cfg_buf, &tmp_uintptr, sizeof(void *));
    (void)amba_ik_system_memcpy(p_sec2_cfg_buf, p_flow_tbl_list->p_CR_buf_24, CR_SIZE_24);
    p_amalgam_data->flow_ctrl.step_crs[0].sec2_crs[20].config_region_addr = (tmp_uintptr - data_offset_base) >> 7UL;
    amba_ik_system_print_uint32_5("Sec2 cr[20] addr 0x%08x, offset %d, src_addr 0x%08x ",
                                  p_amalgam_data->flow_ctrl.step_crs[0].sec2_crs[20].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_24), DC_U, DC_U);

    filter_offset = CR_OFFSET_25;
    tmp_uintptr = sec2_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec2_cfg_buf, &tmp_uintptr, sizeof(void *));
    (void)amba_ik_system_memcpy(p_sec2_cfg_buf, p_flow_tbl_list->p_CR_buf_25, CR_SIZE_25);
    p_amalgam_data->flow_ctrl.step_crs[0].sec2_crs[21].config_region_addr = (tmp_uintptr - data_offset_base) >> 7UL;
    amba_ik_system_print_uint32_5("Sec2 cr[21] addr 0x%08x, offset %d, src_addr 0x%08x ",
                                  p_amalgam_data->flow_ctrl.step_crs[0].sec2_crs[21].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_25), DC_U, DC_U);

    filter_offset = CR_OFFSET_26;
    tmp_uintptr = sec2_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec2_cfg_buf, &tmp_uintptr, sizeof(void *));
    (void)amba_ik_system_memcpy(p_sec2_cfg_buf, p_flow_tbl_list->p_CR_buf_26, CR_SIZE_26);
    p_amalgam_data->flow_ctrl.step_crs[0].sec2_crs[22].config_region_addr = (tmp_uintptr - data_offset_base) >> 7UL;
    amba_ik_system_print_uint32_5("Sec2 cr[22] addr 0x%08x, offset %d, src_addr 0x%08x ",
                                  p_amalgam_data->flow_ctrl.step_crs[0].sec2_crs[22].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_26), DC_U, DC_U);

    filter_offset = CR_OFFSET_27;
    tmp_uintptr = sec2_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec2_cfg_buf, &tmp_uintptr, sizeof(void *));
    (void)amba_ik_system_memcpy(p_sec2_cfg_buf, p_flow_tbl_list->p_CR_buf_27, CR_SIZE_27);
    p_amalgam_data->flow_ctrl.step_crs[0].sec2_crs[23].config_region_addr = (tmp_uintptr - data_offset_base) >> 7UL;
    amba_ik_system_print_uint32_5("Sec2 cr[23] addr 0x%08x, offset %d, src_addr 0x%08x ",
                                  p_amalgam_data->flow_ctrl.step_crs[0].sec2_crs[23].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_27), DC_U, DC_U);

    filter_offset = CR_OFFSET_28;
    tmp_uintptr = sec2_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec2_cfg_buf, &tmp_uintptr, sizeof(void *));
    (void)amba_ik_system_memcpy(p_sec2_cfg_buf, p_flow_tbl_list->p_CR_buf_28, CR_SIZE_28);
    p_amalgam_data->flow_ctrl.step_crs[0].sec2_crs[24].config_region_addr = (tmp_uintptr - data_offset_base) >> 7UL;
    amba_ik_system_print_uint32_5("Sec2 cr[24] addr 0x%08x, offset %d, src_addr 0x%08x ",
                                  p_amalgam_data->flow_ctrl.step_crs[0].sec2_crs[24].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_28), DC_U, DC_U);

    filter_offset = CR_OFFSET_29;
    tmp_uintptr = sec2_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec2_cfg_buf, &tmp_uintptr, sizeof(void *));
    (void)amba_ik_system_memcpy(p_sec2_cfg_buf, p_flow_tbl_list->p_CR_buf_29, CR_SIZE_29);
    p_amalgam_data->flow_ctrl.step_crs[0].sec2_crs[25].config_region_addr = (tmp_uintptr - data_offset_base) >> 7UL;
    amba_ik_system_print_uint32_5("Sec2 cr[25] addr 0x%08x, offset %d, src_addr 0x%08x ",
                                  p_amalgam_data->flow_ctrl.step_crs[0].sec2_crs[25].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_29), DC_U, DC_U);

    filter_offset = CR_OFFSET_30;
    tmp_uintptr = sec2_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec2_cfg_buf, &tmp_uintptr, sizeof(void *));
    (void)amba_ik_system_memcpy(p_sec2_cfg_buf, p_flow_tbl_list->p_CR_buf_30, CR_SIZE_30);
    p_amalgam_data->flow_ctrl.step_crs[0].sec2_crs[26].config_region_addr = (tmp_uintptr - data_offset_base) >> 7UL;
    amba_ik_system_print_uint32_5("Sec2 cr[26] addr 0x%08x, offset %d, src_addr 0x%08x ",
                                  p_amalgam_data->flow_ctrl.step_crs[0].sec2_crs[26].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_30), DC_U, DC_U);

    filter_offset = CR_OFFSET_31;
    tmp_uintptr = sec2_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec2_cfg_buf, &tmp_uintptr, sizeof(void *));
    (void)amba_ik_system_memcpy(p_sec2_cfg_buf, p_flow_tbl_list->p_CR_buf_31, CR_SIZE_31);
    p_amalgam_data->flow_ctrl.step_crs[0].sec2_crs[27].config_region_addr = (tmp_uintptr - data_offset_base) >> 7UL;
    amba_ik_system_print_uint32_5("Sec2 cr[27] addr 0x%08x, offset %d, src_addr 0x%08x ",
                                  p_amalgam_data->flow_ctrl.step_crs[0].sec2_crs[27].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_31), DC_U, DC_U);

    filter_offset = CR_OFFSET_32;
    tmp_uintptr = sec2_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec2_cfg_buf, &tmp_uintptr, sizeof(void *));
    (void)amba_ik_system_memcpy(p_sec2_cfg_buf, p_flow_tbl_list->p_CR_buf_32, CR_SIZE_32);
    p_amalgam_data->flow_ctrl.step_crs[0].sec2_crs[28].config_region_addr = (tmp_uintptr - data_offset_base) >> 7UL;
    amba_ik_system_print_uint32_5("Sec2 cr[28] addr 0x%08x, offset %d, src_addr 0x%08x ",
                                  p_amalgam_data->flow_ctrl.step_crs[0].sec2_crs[28].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_32), DC_U, DC_U);

    filter_offset = CR_OFFSET_33;
    tmp_uintptr = sec2_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec2_cfg_buf, &tmp_uintptr, sizeof(void *));
    (void)amba_ik_system_memcpy(p_sec2_cfg_buf, p_flow_tbl_list->p_CR_buf_33, CR_SIZE_33);
    p_amalgam_data->flow_ctrl.step_crs[0].sec2_crs[29].config_region_addr = (tmp_uintptr - data_offset_base) >> 7UL;
    amba_ik_system_print_uint32_5("Sec2 cr[29] addr 0x%08x, offset %d, src_addr 0x%08x ",
                                  p_amalgam_data->flow_ctrl.step_crs[0].sec2_crs[29].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_33), DC_U, DC_U);

    filter_offset = CR_OFFSET_34;
    tmp_uintptr = sec2_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec2_cfg_buf, &tmp_uintptr, sizeof(void *));
    (void)amba_ik_system_memcpy(p_sec2_cfg_buf, p_flow_tbl_list->p_CR_buf_34, CR_SIZE_34);
    p_amalgam_data->flow_ctrl.step_crs[0].sec2_crs[30].config_region_addr = (tmp_uintptr - data_offset_base) >> 7UL;
    amba_ik_system_print_uint32_5("Sec2 cr[30] addr 0x%08x, offset %d, src_addr 0x%08x ",
                                  p_amalgam_data->flow_ctrl.step_crs[0].sec2_crs[30].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_34), DC_U, DC_U);

    filter_offset = CR_OFFSET_35;
    tmp_uintptr = sec2_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec2_cfg_buf, &tmp_uintptr, sizeof(void *));
    (void)amba_ik_system_memcpy(p_sec2_cfg_buf, p_flow_tbl_list->p_CR_buf_35, CR_SIZE_35);
    p_amalgam_data->flow_ctrl.step_crs[0].sec2_crs[31].config_region_addr = (tmp_uintptr - data_offset_base) >> 7UL;
    amba_ik_system_print_uint32_5("Sec2 cr[31] addr 0x%08x, offset %d, src_addr 0x%08x ",
                                  p_amalgam_data->flow_ctrl.step_crs[0].sec2_crs[31].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_35), DC_U, DC_U);

    filter_offset = CR_OFFSET_40;
    tmp_uintptr = sec2_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec2_cfg_buf, &tmp_uintptr, sizeof(void *));
    (void)amba_ik_system_memcpy(p_sec2_cfg_buf, p_flow_tbl_list->p_CR_buf_40, CR_SIZE_40);
    p_amalgam_data->flow_ctrl.step_crs[0].sec2_crs[32].config_region_addr = (tmp_uintptr - data_offset_base) >> 7UL;
    amba_ik_system_print_uint32_5("Sec2 cr[32] addr 0x%08x, offset %d, src_addr 0x%08x ",
                                  p_amalgam_data->flow_ctrl.step_crs[0].sec2_crs[32].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_40), DC_U, DC_U);

    filter_offset = CR_OFFSET_36;
    tmp_uintptr = sec2_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec2_cfg_buf, &tmp_uintptr, sizeof(void *));
    (void)amba_ik_system_memcpy(p_sec2_cfg_buf, p_flow_tbl_list->p_CR_buf_36, CR_SIZE_36);
    p_amalgam_data->flow_ctrl.step_crs[0].sec2_crs[33].config_region_addr = (tmp_uintptr - data_offset_base) >> 7UL;
    amba_ik_system_print_uint32_5("Sec2 cr[33] addr 0x%08x, offset %d, src_addr 0x%08x ",
                                  p_amalgam_data->flow_ctrl.step_crs[0].sec2_crs[33].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_36), DC_U, DC_U);

    filter_offset = CR_OFFSET_37;
    tmp_uintptr = sec2_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec2_cfg_buf, &tmp_uintptr, sizeof(void *));
    (void)amba_ik_system_memcpy(p_sec2_cfg_buf, p_flow_tbl_list->p_CR_buf_37, CR_SIZE_37);
    p_amalgam_data->flow_ctrl.step_crs[0].sec2_crs[34].config_region_addr = (tmp_uintptr - data_offset_base) >> 7UL;
    amba_ik_system_print_uint32_5("Sec2 cr[34] addr 0x%08x, offset %d, src_addr 0x%08x ",
                                  p_amalgam_data->flow_ctrl.step_crs[0].sec2_crs[34].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_37), DC_U, DC_U);

    filter_offset = CR_OFFSET_38;
    tmp_uintptr = sec2_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec2_cfg_buf, &tmp_uintptr, sizeof(void *));
    (void)amba_ik_system_memcpy(p_sec2_cfg_buf, p_flow_tbl_list->p_CR_buf_38, CR_SIZE_38);
    p_amalgam_data->flow_ctrl.step_crs[0].sec2_crs[35].config_region_addr = (tmp_uintptr - data_offset_base) >> 7UL;
    amba_ik_system_print_uint32_5("Sec2 cr[35] addr 0x%08x, offset %d, src_addr 0x%08x ",
                                  p_amalgam_data->flow_ctrl.step_crs[0].sec2_crs[35].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_38), DC_U, DC_U);

    filter_offset = CR_OFFSET_39;
    tmp_uintptr = sec2_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec2_cfg_buf, &tmp_uintptr, sizeof(void *));
    (void)amba_ik_system_memcpy(p_sec2_cfg_buf, p_flow_tbl_list->p_CR_buf_39, CR_SIZE_39);
    p_amalgam_data->flow_ctrl.step_crs[0].sec2_crs[36].config_region_addr = (tmp_uintptr - data_offset_base) >> 7UL;
    amba_ik_system_print_uint32_5("Sec2 cr[36] addr 0x%08x, offset %d, src_addr 0x%08x ",
                                  p_amalgam_data->flow_ctrl.step_crs[0].sec2_crs[36].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_39), DC_U, DC_U);

    filter_offset = CR_OFFSET_41;
    tmp_uintptr = sec2_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec2_cfg_buf, &tmp_uintptr, sizeof(void *));
    (void)amba_ik_system_memcpy(p_sec2_cfg_buf, p_flow_tbl_list->p_CR_buf_41, CR_SIZE_41);
    p_amalgam_data->flow_ctrl.step_crs[0].sec2_crs[37].config_region_addr = (tmp_uintptr - data_offset_base) >> 7UL;
    amba_ik_system_print_uint32_5("Sec2 cr[37] addr 0x%08x, offset %d, src_addr 0x%08x ",
                                  p_amalgam_data->flow_ctrl.step_crs[0].sec2_crs[37].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_41), DC_U, DC_U);

    filter_offset = CR_OFFSET_42;
    tmp_uintptr = sec3_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec3_cfg_buf, &tmp_uintptr, sizeof(void *));
    (void)amba_ik_system_memcpy(p_sec3_cfg_buf, p_flow_tbl_list->p_CR_buf_42, CR_SIZE_42);
    p_amalgam_data->flow_ctrl.step_crs[0].sec3_crs[0].config_region_addr = (tmp_uintptr - data_offset_base) >> 7UL;
    amba_ik_system_print_uint32_5("Sec3 cr[0] addr 0x%08x, offset %d, src_addr 0x%08x ",
                                  p_amalgam_data->flow_ctrl.step_crs[0].sec3_crs[0].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_42), DC_U, DC_U);

    filter_offset = CR_OFFSET_43;
    tmp_uintptr = sec3_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec3_cfg_buf, &tmp_uintptr, sizeof(void *));
    (void)amba_ik_system_memcpy(p_sec3_cfg_buf, p_flow_tbl_list->p_CR_buf_43, CR_SIZE_43);
    p_amalgam_data->flow_ctrl.step_crs[0].sec3_crs[1].config_region_addr = (tmp_uintptr - data_offset_base) >> 7UL;
    amba_ik_system_print_uint32_5("Sec3 cr[1] addr 0x%08x, offset %d, src_addr 0x%08x ",
                                  p_amalgam_data->flow_ctrl.step_crs[0].sec3_crs[1].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_43), DC_U, DC_U);

    filter_offset = CR_OFFSET_44;
    tmp_uintptr = sec3_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec3_cfg_buf, &tmp_uintptr, sizeof(void *));
    (void)amba_ik_system_memcpy(p_sec3_cfg_buf, p_flow_tbl_list->p_CR_buf_44, CR_SIZE_44);
    p_amalgam_data->flow_ctrl.step_crs[0].sec3_crs[2].config_region_addr = (tmp_uintptr - data_offset_base) >> 7UL;
    amba_ik_system_print_uint32_5("Sec3 cr[2] addr 0x%08x, offset %d, src_addr 0x%08x ",
                                  p_amalgam_data->flow_ctrl.step_crs[0].sec3_crs[2].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_44), DC_U, DC_U);

    if (p_flow_tbl_list->p_CR_buf_45 != NULL) {
        filter_offset = CR_OFFSET_45;
        tmp_uintptr = sec4_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
        (void)amba_ik_system_memcpy(&p_sec4_cfg_buf, &tmp_uintptr, sizeof(void *));
        (void)amba_ik_system_memcpy(p_sec4_cfg_buf, p_flow_tbl_list->p_CR_buf_45, CR_SIZE_45);
        p_amalgam_data->flow_ctrl.step_crs[0].sec4_crs[0].config_region_addr = (tmp_uintptr - data_offset_base) >> 7UL;
        amba_ik_system_print_uint32_5("Sec4 cr[0] addr 0x%08x, offset %d, src_addr 0x%08x ",
                                      p_amalgam_data->flow_ctrl.step_crs[0].sec4_crs[0].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_45), DC_U, DC_U);
    } else {
        amba_ik_system_print_str_5("[IK] CR_buf_45 is NULL", NULL, NULL, NULL, NULL, NULL);
    }

    if (p_flow_tbl_list->p_CR_buf_46 != NULL) {
        filter_offset = CR_OFFSET_46;
        tmp_uintptr = sec4_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
        (void)amba_ik_system_memcpy(&p_sec4_cfg_buf, &tmp_uintptr, sizeof(void *));
        (void)amba_ik_system_memcpy(p_sec4_cfg_buf, p_flow_tbl_list->p_CR_buf_46, CR_SIZE_46);
        p_amalgam_data->flow_ctrl.step_crs[0].sec4_crs[1].config_region_addr = (tmp_uintptr - data_offset_base) >> 7UL;
        amba_ik_system_print_uint32_5("Sec4 cr[1] addr 0x%08x, offset %d, src_addr 0x%08x ",
                                      p_amalgam_data->flow_ctrl.step_crs[0].sec4_crs[1].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_46), DC_U, DC_U);
    } else {
        amba_ik_system_print_str_5("[IK] CR_buf_46 is NULL", NULL, NULL, NULL, NULL, NULL);
    }

    if (p_flow_tbl_list->p_CR_buf_47 != NULL) {
        filter_offset = CR_OFFSET_47;
        tmp_uintptr = sec4_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
        (void)amba_ik_system_memcpy(&p_sec4_cfg_buf, &tmp_uintptr, sizeof(void *));
        (void)amba_ik_system_memcpy(p_sec4_cfg_buf, p_flow_tbl_list->p_CR_buf_47, CR_SIZE_47);
        p_amalgam_data->flow_ctrl.step_crs[0].sec4_crs[2].config_region_addr = (tmp_uintptr - data_offset_base) >> 7UL;
        amba_ik_system_print_uint32_5("Sec4 cr[2] addr 0x%08x, offset %d, src_addr 0x%08x ",
                                      p_amalgam_data->flow_ctrl.step_crs[0].sec4_crs[2].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_47), DC_U, DC_U);
    } else {
        amba_ik_system_print_str_5("[IK] CR_buf_47 is NULL", NULL, NULL, NULL, NULL, NULL);
    }

    if (p_flow_tbl_list->p_CR_buf_48 != NULL) {
        filter_offset = CR_OFFSET_48;
        tmp_uintptr = sec4_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
        (void)amba_ik_system_memcpy(&p_sec4_cfg_buf, &tmp_uintptr, sizeof(void *));
        (void)amba_ik_system_memcpy(p_sec4_cfg_buf, p_flow_tbl_list->p_CR_buf_48, CR_SIZE_48);
        p_amalgam_data->flow_ctrl.step_crs[0].sec4_crs[3].config_region_addr = (tmp_uintptr - data_offset_base) >> 7UL;
        amba_ik_system_print_uint32_5("Sec4 cr[3] addr 0x%08x, offset %d, src_addr 0x%08x ",
                                      p_amalgam_data->flow_ctrl.step_crs[0].sec4_crs[3].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_48), DC_U, DC_U);
    } else {
        amba_ik_system_print_str_5("[IK] CR_buf_48 is NULL", NULL, NULL, NULL, NULL, NULL);
    }

    if (p_flow_tbl_list->p_CR_buf_49 != NULL) {
        filter_offset = CR_OFFSET_49;
        tmp_uintptr = sec4_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
        (void)amba_ik_system_memcpy(&p_sec4_cfg_buf, &tmp_uintptr, sizeof(void *));
        (void)amba_ik_system_memcpy(p_sec4_cfg_buf, p_flow_tbl_list->p_CR_buf_49, CR_SIZE_49);
        p_amalgam_data->flow_ctrl.step_crs[0].sec4_crs[4].config_region_addr = (tmp_uintptr - data_offset_base) >> 7UL;
        amba_ik_system_print_uint32_5("Sec4 cr[4] addr 0x%08x, offset %d, src_addr 0x%08x ",
                                      p_amalgam_data->flow_ctrl.step_crs[0].sec4_crs[4].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_49), DC_U, DC_U);
    } else {
        amba_ik_system_print_str_5("[IK] CR_buf_49 is NULL", NULL, NULL, NULL, NULL, NULL);
    }

    if (p_flow_tbl_list->p_CR_buf_50 != NULL) {
        filter_offset = CR_OFFSET_50;
        tmp_uintptr = sec4_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
        (void)amba_ik_system_memcpy(&p_sec4_cfg_buf, &tmp_uintptr, sizeof(void *));
        (void)amba_ik_system_memcpy(p_sec4_cfg_buf, p_flow_tbl_list->p_CR_buf_50, CR_SIZE_50);
        p_amalgam_data->flow_ctrl.step_crs[0].sec4_crs[5].config_region_addr = (tmp_uintptr - data_offset_base) >> 7UL;
        amba_ik_system_print_uint32_5("Sec4 cr[5] addr 0x%08x, offset %d, src_addr 0x%08x ",
                                      p_amalgam_data->flow_ctrl.step_crs[0].sec4_crs[5].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_50), DC_U, DC_U);
    } else {
        amba_ik_system_print_str_5("[IK] CR_buf_50 is NULL", NULL, NULL, NULL, NULL, NULL);
    }

    if (p_flow_tbl_list->p_CR_buf_51 != NULL) {
        filter_offset = CR_OFFSET_51;
        tmp_uintptr = sec4_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
        (void)amba_ik_system_memcpy(&p_sec4_cfg_buf, &tmp_uintptr, sizeof(void *));
        (void)amba_ik_system_memcpy(p_sec4_cfg_buf, p_flow_tbl_list->p_CR_buf_51, CR_SIZE_51);
        p_amalgam_data->flow_ctrl.step_crs[0].sec4_crs[6].config_region_addr = (tmp_uintptr - data_offset_base) >> 7UL;
        amba_ik_system_print_uint32_5("Sec4 cr[6] addr 0x%08x, offset %d, src_addr 0x%08x ",
                                      p_amalgam_data->flow_ctrl.step_crs[0].sec4_crs[6].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_51), DC_U, DC_U);
    } else {
        amba_ik_system_print_str_5("[IK] CR_buf_51 is NULL", NULL, NULL, NULL, NULL, NULL);
    }

    if (p_flow_tbl_list->p_CR_buf_52 != NULL) {
        filter_offset = CR_OFFSET_52;
        tmp_uintptr = sec4_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
        (void)amba_ik_system_memcpy(&p_sec4_cfg_buf, &tmp_uintptr, sizeof(void *));
        (void)amba_ik_system_memcpy(p_sec4_cfg_buf, p_flow_tbl_list->p_CR_buf_52, CR_SIZE_52);
        p_amalgam_data->flow_ctrl.step_crs[0].sec4_crs[7].config_region_addr = (tmp_uintptr - data_offset_base) >> 7UL;
        amba_ik_system_print_uint32_5("Sec4 cr[7] addr 0x%08x, offset %d, src_addr 0x%08x ",
                                      p_amalgam_data->flow_ctrl.step_crs[0].sec4_crs[7].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_52), DC_U, DC_U);
    } else {
        amba_ik_system_print_str_5("[IK] CR_buf_52 is NULL", NULL, NULL, NULL, NULL, NULL);
    }

    filter_offset = CR_OFFSET_111;
    tmp_uintptr = sec18_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec18_cfg_buf, &tmp_uintptr, sizeof(void *));
    (void)amba_ik_system_memcpy(p_sec18_cfg_buf, p_flow_tbl_list->p_CR_buf_111, CR_SIZE_111);
    p_amalgam_data->flow_ctrl.step_crs[0].sec18_crs[0].config_region_addr = (tmp_uintptr - data_offset_base) >> 7UL;
    amba_ik_system_print_uint32_5("Sec18 cr[0] addr 0x%08x, offset %d, src_addr 0x%08x ",
                                  p_amalgam_data->flow_ctrl.step_crs[0].sec18_crs[0].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_111), DC_U, DC_U);

    filter_offset = CR_OFFSET_112;
    tmp_uintptr = sec18_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec18_cfg_buf, &tmp_uintptr, sizeof(void *));
    (void)amba_ik_system_memcpy(p_sec18_cfg_buf, p_flow_tbl_list->p_CR_buf_112, CR_SIZE_112);
    p_amalgam_data->flow_ctrl.step_crs[0].sec18_crs[1].config_region_addr = (tmp_uintptr - data_offset_base) >> 7UL;
    amba_ik_system_print_uint32_5("Sec18 cr[1] addr 0x%08x, offset %d, src_addr 0x%08x ",
                                  p_amalgam_data->flow_ctrl.step_crs[0].sec18_crs[1].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_112), DC_U, DC_U);

    filter_offset = CR_OFFSET_113;
    tmp_uintptr = sec18_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec18_cfg_buf, &tmp_uintptr, sizeof(void *));
    (void)amba_ik_system_memcpy(p_sec18_cfg_buf, p_flow_tbl_list->p_CR_buf_113, CR_SIZE_113);
    p_amalgam_data->flow_ctrl.step_crs[0].sec18_crs[2].config_region_addr = (tmp_uintptr - data_offset_base) >> 7UL;
    amba_ik_system_print_uint32_5("Sec18 cr[2] addr 0x%08x, offset %d, src_addr 0x%08x ",
                                  p_amalgam_data->flow_ctrl.step_crs[0].sec18_crs[2].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_113), DC_U, DC_U);

    filter_offset = CR_OFFSET_114;
    tmp_uintptr = sec18_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec18_cfg_buf, &tmp_uintptr, sizeof(void *));
    (void)amba_ik_system_memcpy(p_sec18_cfg_buf, p_flow_tbl_list->p_CR_buf_114, CR_SIZE_114);
    p_amalgam_data->flow_ctrl.step_crs[0].sec18_crs[3].config_region_addr = (tmp_uintptr - data_offset_base) >> 7UL;
    amba_ik_system_print_uint32_5("Sec18 cr[3] addr 0x%08x, offset %d, src_addr 0x%08x ",
                                  p_amalgam_data->flow_ctrl.step_crs[0].sec18_crs[3].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_114), DC_U, DC_U);

    filter_offset = CR_OFFSET_115;
    tmp_uintptr = sec18_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec18_cfg_buf, &tmp_uintptr, sizeof(void *));
    (void)amba_ik_system_memcpy(p_sec18_cfg_buf, p_flow_tbl_list->p_CR_buf_115, CR_SIZE_115);
    p_amalgam_data->flow_ctrl.step_crs[0].sec18_crs[4].config_region_addr = (tmp_uintptr - data_offset_base) >> 7UL;
    amba_ik_system_print_uint32_5("Sec18 cr[4] addr 0x%08x, offset %d, src_addr 0x%08x ",
                                  p_amalgam_data->flow_ctrl.step_crs[0].sec18_crs[4].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_115), DC_U, DC_U);

    filter_offset = CR_OFFSET_116;
    tmp_uintptr = sec18_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec18_cfg_buf, &tmp_uintptr, sizeof(void *));
    (void)amba_ik_system_memcpy(p_sec18_cfg_buf, p_flow_tbl_list->p_CR_buf_116, CR_SIZE_116);
    p_amalgam_data->flow_ctrl.step_crs[0].sec18_crs[5].config_region_addr = (tmp_uintptr - data_offset_base) >> 7UL;
    amba_ik_system_print_uint32_5("Sec18 cr[5] addr 0x%08x, offset %d, src_addr 0x%08x ",
                                  p_amalgam_data->flow_ctrl.step_crs[0].sec18_crs[5].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_116), DC_U, DC_U);

    filter_offset = CR_OFFSET_117;
    tmp_uintptr = sec18_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec18_cfg_buf, &tmp_uintptr, sizeof(void *));
    (void)amba_ik_system_memcpy(p_sec18_cfg_buf, p_flow_tbl_list->p_CR_buf_117, CR_SIZE_117);
    p_amalgam_data->flow_ctrl.step_crs[0].sec18_crs[6].config_region_addr = (tmp_uintptr - data_offset_base) >> 7UL;
    amba_ik_system_print_uint32_5("Sec18 cr[6] addr 0x%08x, offset %d, src_addr 0x%08x ",
                                  p_amalgam_data->flow_ctrl.step_crs[0].sec18_crs[6].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_117), DC_U, DC_U);

    filter_offset = CR_OFFSET_118;
    tmp_uintptr = sec18_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec18_cfg_buf, &tmp_uintptr, sizeof(void *));
    (void)amba_ik_system_memcpy(p_sec18_cfg_buf, p_flow_tbl_list->p_CR_buf_118, CR_SIZE_118);
    p_amalgam_data->flow_ctrl.step_crs[0].sec18_crs[7].config_region_addr = (tmp_uintptr - data_offset_base) >> 7UL;
    amba_ik_system_print_uint32_5("Sec18 cr[7] addr 0x%08x, offset %d, src_addr 0x%08x ",
                                  p_amalgam_data->flow_ctrl.step_crs[0].sec18_crs[7].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_118), DC_U, DC_U);

    filter_offset = CR_OFFSET_119;
    tmp_uintptr = sec18_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec18_cfg_buf, &tmp_uintptr, sizeof(void *));
    (void)amba_ik_system_memcpy(p_sec18_cfg_buf, p_flow_tbl_list->p_CR_buf_119, CR_SIZE_119);
    p_amalgam_data->flow_ctrl.step_crs[0].sec18_crs[8].config_region_addr = (tmp_uintptr - data_offset_base) >> 7UL;
    amba_ik_system_print_uint32_5("Sec18 cr[8] addr 0x%08x, offset %d, src_addr 0x%08x ",
                                  p_amalgam_data->flow_ctrl.step_crs[0].sec18_crs[8].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_119), DC_U, DC_U);

    filter_offset = CR_OFFSET_120;
    tmp_uintptr = sec18_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec18_cfg_buf, &tmp_uintptr, sizeof(void *));
    (void)amba_ik_system_memcpy(p_sec18_cfg_buf, p_flow_tbl_list->p_CR_buf_120, CR_SIZE_120);
    p_amalgam_data->flow_ctrl.step_crs[0].sec18_crs[9].config_region_addr = (tmp_uintptr - data_offset_base) >> 7UL;
    amba_ik_system_print_uint32_5("Sec18 cr[9] addr 0x%08x, offset %d, src_addr 0x%08x ",
                                  p_amalgam_data->flow_ctrl.step_crs[0].sec18_crs[9].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_120), DC_U, DC_U);

    filter_offset = CR_OFFSET_121;
    tmp_uintptr = sec18_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec18_cfg_buf, &tmp_uintptr, sizeof(void *));
    (void)amba_ik_system_memcpy(p_sec18_cfg_buf, p_flow_tbl_list->p_CR_buf_121, CR_SIZE_121);
    p_amalgam_data->flow_ctrl.step_crs[0].sec18_crs[10].config_region_addr = (tmp_uintptr - data_offset_base) >> 7UL;
    amba_ik_system_print_uint32_5("Sec18 cr[10] addr 0x%08x, offset %d, src_addr 0x%08x ",
                                  p_amalgam_data->flow_ctrl.step_crs[0].sec18_crs[10].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_121), DC_U, DC_U);

    p_amalgam_data->flow_ctrl.header.idsp_flow_addr = data_offset_base;

    return rval;
}

uint32 img_cfg_amalgam_data_ctx_update(uint32 context_id, const void *p_user_buffer)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx;
    ik_debug_data_t *p_ik_dbg_data;

    rval |= img_ctx_get_context(context_id, &p_ctx);

    if(rval == IK_OK) {
        (void)amba_ik_system_memcpy(&p_ik_dbg_data, &p_user_buffer, sizeof(ik_debug_data_t *));

        //ctx info
        p_ik_dbg_data->ctx_id = (uint16)context_id;
        p_ik_dbg_data->active_flow_idx = p_ctx->organization.active_cr_state.active_flow_idx;

        //3a statistic
        (void)amba_ik_system_memcpy(&p_ik_dbg_data->aaa_stat_info, &p_ctx->filters.input_param.aaa_stat_info, sizeof(ik_aaa_stat_info_t));
        (void)amba_ik_system_memcpy(&p_ik_dbg_data->aaa_pg_stat_info, &p_ctx->filters.input_param.aaa_pg_stat_info, sizeof(ik_aaa_pg_af_stat_info_t));
        (void)amba_ik_system_memcpy(&p_ik_dbg_data->af_stat_ex_info, &p_ctx->filters.input_param.af_stat_ex_info, sizeof(ik_af_stat_ex_info_t));
        (void)amba_ik_system_memcpy(&p_ik_dbg_data->pg_af_stat_ex_info, &p_ctx->filters.input_param.pg_af_stat_ex_info, sizeof(ik_pg_af_stat_ex_info_t));
        (void)amba_ik_system_memcpy(&p_ik_dbg_data->hist_info, &p_ctx->filters.input_param.hist_info, sizeof(ik_histogram_info_t));
        (void)amba_ik_system_memcpy(&p_ik_dbg_data->hist_info_pg, &p_ctx->filters.input_param.hist_info_pg, sizeof(ik_histogram_info_t));

        // window info
        p_ik_dbg_data->flip_mode = p_ctx->filters.input_param.flip_mode;
        (void)amba_ik_system_memcpy(&p_ik_dbg_data->dzoom_info, &p_ctx->filters.input_param.dzoom_info, sizeof(ik_dzoom_info_t));
        (void)amba_ik_system_memcpy(&p_ik_dbg_data->dmy_range, &p_ctx->filters.input_param.dmy_range, sizeof(ik_dummy_margin_range_t));
        (void)amba_ik_system_memcpy(&p_ik_dbg_data->active_window, &p_ctx->filters.input_param.active_window, sizeof(ik_vin_active_window_t));
        (void)amba_ik_system_memcpy(&p_ik_dbg_data->stitching_info, &p_ctx->filters.input_param.stitching_info, sizeof(ik_stitch_info_t));
        (void)amba_ik_system_memcpy(&p_ik_dbg_data->window_size_info, &p_ctx->filters.input_param.window_size_info, sizeof(ik_window_size_info_t));
        (void)amba_ik_system_memcpy(&p_ik_dbg_data->result_win, &p_ctx->filters.input_param.ctx_buf.result_win, sizeof(amba_ik_calc_win_result_t));

        // vig
        p_ik_dbg_data->vig_enable = p_ctx->filters.input_param.vig_enable;
        (void)amba_ik_system_memcpy(&p_ik_dbg_data->vignette_compensation, &p_ctx->filters.input_param.vignette_compensation, sizeof(ik_vignette_t));

        // ca
        p_ik_dbg_data->cawarp_enable = p_ctx->filters.input_param.cawarp_enable;
        (void)amba_ik_system_memcpy(&p_ik_dbg_data->calib_cawarp_info, &p_ctx->filters.input_param.calib_ca_warp_info, sizeof(ik_cawarp_info_t));
        if(p_ctx->filters.input_param.calib_ca_warp_info.p_cawarp_red != NULL) {
            (void)amba_ik_system_memcpy(p_ik_dbg_data->calib_cawarp_red, p_ctx->filters.input_param.calib_ca_warp_info.p_cawarp_red, sizeof(ik_grid_point_t) * CAWARP_VIDEO_MAXHORGRID * CAWARP_VIDEO_MAXVERGRID);
        } else {
            (void)amba_ik_system_memset(p_ik_dbg_data->calib_cawarp_red, 0x0, sizeof(ik_grid_point_t) * CAWARP_VIDEO_MAXHORGRID * CAWARP_VIDEO_MAXVERGRID);
        }
        if(p_ctx->filters.input_param.calib_ca_warp_info.p_cawarp_blue != NULL) {
            (void)amba_ik_system_memcpy(p_ik_dbg_data->calib_cawarp_blue, p_ctx->filters.input_param.calib_ca_warp_info.p_cawarp_blue, sizeof(ik_grid_point_t) * CAWARP_VIDEO_MAXHORGRID * CAWARP_VIDEO_MAXVERGRID);
        } else {
            (void)amba_ik_system_memset(p_ik_dbg_data->calib_cawarp_blue, 0x0, sizeof(ik_grid_point_t) * CAWARP_VIDEO_MAXHORGRID * CAWARP_VIDEO_MAXVERGRID);
        }

        // warp
        p_ik_dbg_data->warp_enable = p_ctx->filters.input_param.warp_enable;
        p_ik_dbg_data->chroma_radius = p_ctx->filters.input_param.chroma_filter.radius;
        p_ik_dbg_data->yuv_mode = p_ctx->filters.input_param.yuv_mode;
        (void)amba_ik_system_memcpy(&p_ik_dbg_data->calib_warp_info, &p_ctx->filters.input_param.calib_warp_info, sizeof(ik_warp_info_t));
        (void)amba_ik_system_memcpy(&p_ik_dbg_data->warp_buf_info, &p_ctx->filters.input_param.warp_buf_info, sizeof(ik_warp_buffer_info_t));
        if(p_ctx->filters.input_param.calib_warp_info.pwarp != NULL) {
            (void)amba_ik_system_memcpy(p_ik_dbg_data->calib_warp, p_ctx->filters.input_param.calib_warp_info.pwarp, sizeof(ik_grid_point_t) * WARP_VIDEO_MAXHORGRID * WARP_VIDEO_MAXVERGRID * 2UL);
        } else {
            (void)amba_ik_system_memset(p_ik_dbg_data->calib_warp, 0x0, sizeof(ik_grid_point_t) * WARP_VIDEO_MAXHORGRID * WARP_VIDEO_MAXVERGRID * 2UL);
        }
    } else {
        amba_ik_system_print_str_5("[IK] can't get context", NULL, NULL, NULL, NULL, NULL);
    }

    return rval;
}

uint32 img_cfg_query_debug_flow_data_size(size_t *p_size)
{
    uint32 rval = IK_OK;

    if(p_size != NULL) {
        *p_size = sizeof(ik_debug_data_t);
    } else {
        amba_ik_system_print_str_5("img_cfg_query_debug_config_size size ptr is NULL", NULL, NULL, NULL, NULL, NULL);
        rval = IK_ERR_0005;
    }
    return rval;
}

#if SUPPORT_CR_MEMORY_FENCE
static void img_cfg_set_sec2_cr_memory_fence(const amba_ik_cr_fence_info_t *p_fence, const amba_ik_flow_tables_t *p_flow_tbl)
{
    const uint8 *tmp_p_u8;
    uint8 *p_tmp_u8 = NULL;

    tmp_p_u8 = p_flow_tbl->CR_buf_4;
    (void)amba_ik_system_memcpy(&p_tmp_u8, &tmp_p_u8, sizeof(void *));
    p_tmp_u8 = &p_tmp_u8[p_fence[0].offset];
    (void)amba_ik_system_memset(p_tmp_u8, 0x58, p_fence[0].length);

    tmp_p_u8 = p_flow_tbl->CR_buf_5;
    (void)amba_ik_system_memcpy(&p_tmp_u8, &tmp_p_u8, sizeof(void *));
    p_tmp_u8 = &p_tmp_u8[p_fence[1].offset];
    (void)amba_ik_system_memset(p_tmp_u8, 0x58, p_fence[1].length);

    tmp_p_u8 = p_flow_tbl->CR_buf_6;
    (void)amba_ik_system_memcpy(&p_tmp_u8, &tmp_p_u8, sizeof(void *));
    p_tmp_u8 = &p_tmp_u8[p_fence[2].offset];
    (void)amba_ik_system_memset(p_tmp_u8, 0x58, p_fence[2].length);

    tmp_p_u8 = p_flow_tbl->CR_buf_7;
    (void)amba_ik_system_memcpy(&p_tmp_u8, &tmp_p_u8, sizeof(void *));
    p_tmp_u8 = &p_tmp_u8[p_fence[3].offset];
    (void)amba_ik_system_memset(p_tmp_u8, 0x58, p_fence[3].length);

    tmp_p_u8 = p_flow_tbl->CR_buf_8;
    (void)amba_ik_system_memcpy(&p_tmp_u8, &tmp_p_u8, sizeof(void *));
    p_tmp_u8 = &p_tmp_u8[p_fence[4].offset];
    (void)amba_ik_system_memset(p_tmp_u8, 0x58, p_fence[4].length);

    tmp_p_u8 = p_flow_tbl->CR_buf_9;
    (void)amba_ik_system_memcpy(&p_tmp_u8, &tmp_p_u8, sizeof(void *));
    p_tmp_u8 = &p_tmp_u8[p_fence[5].offset];
    (void)amba_ik_system_memset(p_tmp_u8, 0x58, p_fence[5].length);

    tmp_p_u8 = p_flow_tbl->CR_buf_12;
    (void)amba_ik_system_memcpy(&p_tmp_u8, &tmp_p_u8, sizeof(void *));
    p_tmp_u8 = &p_tmp_u8[p_fence[8].offset];
    (void)amba_ik_system_memset(p_tmp_u8, 0x58, p_fence[8].length);

    tmp_p_u8 = p_flow_tbl->CR_buf_13;
    (void)amba_ik_system_memcpy(&p_tmp_u8, &tmp_p_u8, sizeof(void *));
    p_tmp_u8 = &p_tmp_u8[p_fence[9].offset];
    (void)amba_ik_system_memset(p_tmp_u8, 0x58, p_fence[9].length);

    tmp_p_u8 = p_flow_tbl->CR_buf_14;
    (void)amba_ik_system_memcpy(&p_tmp_u8, &tmp_p_u8, sizeof(void *));
    p_tmp_u8 = &p_tmp_u8[p_fence[10].offset];
    (void)amba_ik_system_memset(p_tmp_u8, 0x58, p_fence[10].length);

    tmp_p_u8 = p_flow_tbl->CR_buf_15;
    (void)amba_ik_system_memcpy(&p_tmp_u8, &tmp_p_u8, sizeof(void *));
    p_tmp_u8 = &p_tmp_u8[p_fence[11].offset];
    (void)amba_ik_system_memset(p_tmp_u8, 0x58, p_fence[11].length);

    tmp_p_u8 = p_flow_tbl->CR_buf_16;
    (void)amba_ik_system_memcpy(&p_tmp_u8, &tmp_p_u8, sizeof(void *));
    p_tmp_u8 = &p_tmp_u8[p_fence[12].offset];
    (void)amba_ik_system_memset(p_tmp_u8, 0x58, p_fence[12].length);

    tmp_p_u8 = p_flow_tbl->CR_buf_22;
    (void)amba_ik_system_memcpy(&p_tmp_u8, &tmp_p_u8, sizeof(void *));
    p_tmp_u8 = &p_tmp_u8[p_fence[18].offset];
    (void)amba_ik_system_memset(p_tmp_u8, 0x58, p_fence[18].length);

    tmp_p_u8 = p_flow_tbl->CR_buf_23;
    (void)amba_ik_system_memcpy(&p_tmp_u8, &tmp_p_u8, sizeof(void *));
    p_tmp_u8 = &p_tmp_u8[p_fence[19].offset];
    (void)amba_ik_system_memset(p_tmp_u8, 0x58, p_fence[19].length);

    tmp_p_u8 = p_flow_tbl->CR_buf_24;
    (void)amba_ik_system_memcpy(&p_tmp_u8, &tmp_p_u8, sizeof(void *));
    p_tmp_u8 = &p_tmp_u8[p_fence[20].offset];
    (void)amba_ik_system_memset(p_tmp_u8, 0x58, p_fence[20].length);

    tmp_p_u8 = p_flow_tbl->CR_buf_25;
    (void)amba_ik_system_memcpy(&p_tmp_u8, &tmp_p_u8, sizeof(void *));
    p_tmp_u8 = &p_tmp_u8[p_fence[21].offset];
    (void)amba_ik_system_memset(p_tmp_u8, 0x58, p_fence[21].length);

    tmp_p_u8 = p_flow_tbl->CR_buf_26;
    (void)amba_ik_system_memcpy(&p_tmp_u8, &tmp_p_u8, sizeof(void *));
    p_tmp_u8 = &p_tmp_u8[p_fence[22].offset];
    (void)amba_ik_system_memset(p_tmp_u8, 0x58, p_fence[22].length);

    tmp_p_u8 = p_flow_tbl->CR_buf_28;
    (void)amba_ik_system_memcpy(&p_tmp_u8, &tmp_p_u8, sizeof(void *));
    p_tmp_u8 = &p_tmp_u8[p_fence[24].offset];
    (void)amba_ik_system_memset(p_tmp_u8, 0x58, p_fence[24].length);

    tmp_p_u8 = p_flow_tbl->CR_buf_29;
    (void)amba_ik_system_memcpy(&p_tmp_u8, &tmp_p_u8, sizeof(void *));
    p_tmp_u8 = &p_tmp_u8[p_fence[25].offset];
    (void)amba_ik_system_memset(p_tmp_u8, 0x58, p_fence[25].length);

    tmp_p_u8 = p_flow_tbl->CR_buf_31;
    (void)amba_ik_system_memcpy(&p_tmp_u8, &tmp_p_u8, sizeof(void *));
    p_tmp_u8 = &p_tmp_u8[p_fence[27].offset];
    (void)amba_ik_system_memset(p_tmp_u8, 0x58, p_fence[27].length);

    tmp_p_u8 = p_flow_tbl->CR_buf_32;
    (void)amba_ik_system_memcpy(&p_tmp_u8, &tmp_p_u8, sizeof(void *));
    p_tmp_u8 = &p_tmp_u8[p_fence[28].offset];
    (void)amba_ik_system_memset(p_tmp_u8, 0x58, p_fence[28].length);

    tmp_p_u8 = p_flow_tbl->CR_buf_36;
    (void)amba_ik_system_memcpy(&p_tmp_u8, &tmp_p_u8, sizeof(void *));
    p_tmp_u8 = &p_tmp_u8[p_fence[33].offset];
    (void)amba_ik_system_memset(p_tmp_u8, 0x58, p_fence[33].length);

    tmp_p_u8 = p_flow_tbl->CR_buf_37;
    (void)amba_ik_system_memcpy(&p_tmp_u8, &tmp_p_u8, sizeof(void *));
    p_tmp_u8 = &p_tmp_u8[p_fence[34].offset];
    (void)amba_ik_system_memset(p_tmp_u8, 0x58, p_fence[34].length);

    tmp_p_u8 = p_flow_tbl->CR_buf_38;
    (void)amba_ik_system_memcpy(&p_tmp_u8, &tmp_p_u8, sizeof(void *));
    p_tmp_u8 = &p_tmp_u8[p_fence[35].offset];
    (void)amba_ik_system_memset(p_tmp_u8, 0x58, p_fence[35].length);

    tmp_p_u8 = p_flow_tbl->CR_buf_39;
    (void)amba_ik_system_memcpy(&p_tmp_u8, &tmp_p_u8, sizeof(void *));
    p_tmp_u8 = &p_tmp_u8[p_fence[36].offset];
    (void)amba_ik_system_memset(p_tmp_u8, 0x58, p_fence[36].length);

    tmp_p_u8 = p_flow_tbl->CR_buf_40;
    (void)amba_ik_system_memcpy(&p_tmp_u8, &tmp_p_u8, sizeof(void *));
    p_tmp_u8 = &p_tmp_u8[p_fence[32].offset];
    (void)amba_ik_system_memset(p_tmp_u8, 0x58, p_fence[32].length);

    tmp_p_u8 = p_flow_tbl->CR_buf_41;
    (void)amba_ik_system_memcpy(&p_tmp_u8, &tmp_p_u8, sizeof(void *));
    p_tmp_u8 = &p_tmp_u8[p_fence[37].offset];
    (void)amba_ik_system_memset(p_tmp_u8, 0x58, p_fence[37].length);
}

static void img_cfg_set_sec3_cr_memory_fence(const amba_ik_cr_fence_info_t *p_fence, const amba_ik_flow_tables_t *p_flow_tbl)
{
    const uint8 *tmp_p_u8;
    uint8 *p_tmp_u8 = NULL;

    tmp_p_u8 = p_flow_tbl->CR_buf_44;
    (void)amba_ik_system_memcpy(&p_tmp_u8, &tmp_p_u8, sizeof(void *));
    p_tmp_u8 = &p_tmp_u8[p_fence[40].offset];
    (void)amba_ik_system_memset(p_tmp_u8, 0x58, p_fence[40].length);
}

static void img_cfg_set_sec4_cr_memory_fence(const amba_ik_cr_fence_info_t *p_fence, const amba_ik_flow_tables_t *p_flow_tbl)
{
    const uint8 *tmp_p_u8;
    uint8 *p_tmp_u8 = NULL;

    tmp_p_u8 = p_flow_tbl->CR_buf_45;
    (void)amba_ik_system_memcpy(&p_tmp_u8, &tmp_p_u8, sizeof(void *));
    p_tmp_u8 = &p_tmp_u8[p_fence[41].offset];
    (void)amba_ik_system_memset(p_tmp_u8, 0x58, p_fence[41].length);

    tmp_p_u8 = p_flow_tbl->CR_buf_46;
    (void)amba_ik_system_memcpy(&p_tmp_u8, &tmp_p_u8, sizeof(void *));
    p_tmp_u8 = &p_tmp_u8[p_fence[42].offset];
    (void)amba_ik_system_memset(p_tmp_u8, 0x58, p_fence[42].length);

    tmp_p_u8 = p_flow_tbl->CR_buf_47;
    (void)amba_ik_system_memcpy(&p_tmp_u8, &tmp_p_u8, sizeof(void *));
    p_tmp_u8 = &p_tmp_u8[p_fence[43].offset];
    (void)amba_ik_system_memset(p_tmp_u8, 0x58, p_fence[43].length);

    tmp_p_u8 = p_flow_tbl->CR_buf_50;
    (void)amba_ik_system_memcpy(&p_tmp_u8, &tmp_p_u8, sizeof(void *));
    p_tmp_u8 = &p_tmp_u8[p_fence[46].offset];
    (void)amba_ik_system_memset(p_tmp_u8, 0x58, p_fence[46].length);

    tmp_p_u8 = p_flow_tbl->CR_buf_51;
    (void)amba_ik_system_memcpy(&p_tmp_u8, &tmp_p_u8, sizeof(void *));
    p_tmp_u8 = &p_tmp_u8[p_fence[47].offset];
    (void)amba_ik_system_memset(p_tmp_u8, 0x58, p_fence[47].length);

    tmp_p_u8 = p_flow_tbl->CR_buf_52;
    (void)amba_ik_system_memcpy(&p_tmp_u8, &tmp_p_u8, sizeof(void *));
    p_tmp_u8 = &p_tmp_u8[p_fence[48].offset];
    (void)amba_ik_system_memset(p_tmp_u8, 0x58, p_fence[48].length);
}

static void img_cfg_set_sec18_cr_memory_fence(const amba_ik_cr_fence_info_t *p_fence, const amba_ik_flow_tables_t *p_flow_tbl)
{
    const uint8 *tmp_p_u8;
    uint8 *p_tmp_u8 = NULL;

    tmp_p_u8 = p_flow_tbl->CR_buf_111;
    (void)amba_ik_system_memcpy(&p_tmp_u8, &tmp_p_u8, sizeof(void *));
    p_tmp_u8 = &p_tmp_u8[p_fence[49].offset];
    (void)amba_ik_system_memset(p_tmp_u8, 0x58, p_fence[49].length);

    tmp_p_u8 = p_flow_tbl->CR_buf_114;
    (void)amba_ik_system_memcpy(&p_tmp_u8, &tmp_p_u8, sizeof(void *));
    p_tmp_u8 = &p_tmp_u8[p_fence[52].offset];
    (void)amba_ik_system_memset(p_tmp_u8, 0x58, p_fence[52].length);

    tmp_p_u8 = p_flow_tbl->CR_buf_116;
    (void)amba_ik_system_memcpy(&p_tmp_u8, &tmp_p_u8, sizeof(void *));
    p_tmp_u8 = &p_tmp_u8[p_fence[54].offset];
    (void)amba_ik_system_memset(p_tmp_u8, 0x58, p_fence[54].length);

    tmp_p_u8 = p_flow_tbl->CR_buf_117;
    (void)amba_ik_system_memcpy(&p_tmp_u8, &tmp_p_u8, sizeof(void *));
    p_tmp_u8 = &p_tmp_u8[p_fence[55].offset];
    (void)amba_ik_system_memset(p_tmp_u8, 0x58, p_fence[55].length);

    tmp_p_u8 = p_flow_tbl->CR_buf_118;
    (void)amba_ik_system_memcpy(&p_tmp_u8, &tmp_p_u8, sizeof(void *));
    p_tmp_u8 = &p_tmp_u8[p_fence[56].offset];
    (void)amba_ik_system_memset(p_tmp_u8, 0x58, p_fence[56].length);

    tmp_p_u8 = p_flow_tbl->CR_buf_120;
    (void)amba_ik_system_memcpy(&p_tmp_u8, &tmp_p_u8, sizeof(void *));
    p_tmp_u8 = &p_tmp_u8[p_fence[58].offset];
    (void)amba_ik_system_memset(p_tmp_u8, 0x58, p_fence[58].length);

    tmp_p_u8 = p_flow_tbl->CR_buf_121;
    (void)amba_ik_system_memcpy(&p_tmp_u8, &tmp_p_u8, sizeof(void *));
    p_tmp_u8 = &p_tmp_u8[p_fence[59].offset];
    (void)amba_ik_system_memset(p_tmp_u8, 0x58, p_fence[59].length);
}

static void img_cfg_set_flow_tbl_memory_fence(const amba_ik_cr_fence_info_t *p_fence, const amba_ik_flow_tables_t *p_flow_tbl)
{
    const uint32 *tmp_p_u32;
    uint8 *p_tmp_u8 = NULL;

    tmp_p_u32 = p_flow_tbl->fence_0;
    (void)amba_ik_system_memcpy(&p_tmp_u8, &tmp_p_u32, sizeof(void *));
    p_tmp_u8 = &p_tmp_u8[p_fence[60].offset];
    (void)amba_ik_system_memset(p_tmp_u8, 0x58, p_fence[60].length);

    tmp_p_u32 = p_flow_tbl->fence_1;
    (void)amba_ik_system_memcpy(&p_tmp_u8, &tmp_p_u32, sizeof(void *));
    p_tmp_u8 = &p_tmp_u8[p_fence[61].offset];
    (void)amba_ik_system_memset(p_tmp_u8, 0x58, p_fence[61].length);

    tmp_p_u32 = p_flow_tbl->fence_2;
    (void)amba_ik_system_memcpy(&p_tmp_u8, &tmp_p_u32, sizeof(void *));
    p_tmp_u8 = &p_tmp_u8[p_fence[62].offset];
    (void)amba_ik_system_memset(p_tmp_u8, 0x58, p_fence[62].length);

    tmp_p_u32 = p_flow_tbl->fence_3;
    (void)amba_ik_system_memcpy(&p_tmp_u8, &tmp_p_u32, sizeof(void *));
    p_tmp_u8 = &p_tmp_u8[p_fence[63].offset];
    (void)amba_ik_system_memset(p_tmp_u8, 0x58, p_fence[63].length);

    tmp_p_u32 = p_flow_tbl->fence_4;
    (void)amba_ik_system_memcpy(&p_tmp_u8, &tmp_p_u32, sizeof(void *));
    p_tmp_u8 = &p_tmp_u8[p_fence[64].offset];
    (void)amba_ik_system_memset(p_tmp_u8, 0x58, p_fence[64].length);

    tmp_p_u32 = p_flow_tbl->fence_5;
    (void)amba_ik_system_memcpy(&p_tmp_u8, &tmp_p_u32, sizeof(void *));
    p_tmp_u8 = &p_tmp_u8[p_fence[65].offset];
    (void)amba_ik_system_memset(p_tmp_u8, 0x58, p_fence[65].length);

    tmp_p_u32 = p_flow_tbl->fence_6;
    (void)amba_ik_system_memcpy(&p_tmp_u8, &tmp_p_u32, sizeof(void *));
    p_tmp_u8 = &p_tmp_u8[p_fence[66].offset];
    (void)amba_ik_system_memset(p_tmp_u8, 0x58, p_fence[66].length);

    tmp_p_u32 = p_flow_tbl->fence_7;
    (void)amba_ik_system_memcpy(&p_tmp_u8, &tmp_p_u32, sizeof(void *));
    p_tmp_u8 = &p_tmp_u8[p_fence[67].offset];
    (void)amba_ik_system_memset(p_tmp_u8, 0x58, p_fence[67].length);
}

static uint32 img_cfg_set_linear_cr_memory_fence(const amba_ik_cr_fence_info_t *p_fence, const amba_ik_flow_tables_t *p_flow_tbl)
{
    uint32 rval = IK_OK;

    img_cfg_set_sec2_cr_memory_fence(p_fence, p_flow_tbl);
    img_cfg_set_sec3_cr_memory_fence(p_fence, p_flow_tbl);
    img_cfg_set_sec18_cr_memory_fence(p_fence, p_flow_tbl);
    img_cfg_set_flow_tbl_memory_fence(p_fence, p_flow_tbl);

    return rval;
}

static uint32 img_cfg_set_hdr_cr_memory_fence(const amba_ik_cr_fence_info_t *p_fence, const amba_ik_flow_tables_t *p_flow_tbl)
{
    uint32 rval = IK_OK;

    img_cfg_set_sec2_cr_memory_fence(p_fence, p_flow_tbl);
    img_cfg_set_sec3_cr_memory_fence(p_fence, p_flow_tbl);
    img_cfg_set_sec4_cr_memory_fence(p_fence, p_flow_tbl);
    img_cfg_set_sec18_cr_memory_fence(p_fence, p_flow_tbl);
    img_cfg_set_flow_tbl_memory_fence(p_fence, p_flow_tbl);

    return rval;
}

uint32 img_cfg_set_cr_memory_fence(amba_ik_cr_fence_info_t *p_fence, amba_ik_flow_tables_t *p_flow_tbl, const ik_ability_t *p_ability)
{
    uint32 rval = IK_OK;

    if ((p_flow_tbl == NULL) || (p_ability == NULL)) {
        amba_ik_system_print_str_5("[IK][ERROR] img_cfg_set_cr_memory_fence() detect NULL pointer", NULL, NULL, NULL, NULL, NULL);
        rval |= IK_ERR_0005;
    } else {
        (void)amba_ik_system_memcpy(p_fence, p_fence, sizeof(uint8));//misra fix.
        (void)amba_ik_system_memcpy(p_flow_tbl, p_flow_tbl, sizeof(uint8));//misra fix.
        if (p_ability->pipe == AMBA_IK_PIPE_VIDEO) {
            if(p_ability->video_pipe < AMBA_IK_VIDEO_MAX) {
                if((p_ability->video_pipe == AMBA_IK_VIDEO_LINEAR) || (p_ability->video_pipe == AMBA_IK_VIDEO_Y2Y)) {
                    (void)img_cfg_set_linear_cr_memory_fence(p_fence, p_flow_tbl);
                }
                if((p_ability->video_pipe == AMBA_IK_VIDEO_LINEAR_CE) || (p_ability->video_pipe == AMBA_IK_VIDEO_HDR_EXPO_2) ||
                   (p_ability->video_pipe == AMBA_IK_VIDEO_HDR_EXPO_3)) {
                    (void)img_cfg_set_hdr_cr_memory_fence(p_fence, p_flow_tbl);
                }
            } else {
                amba_ik_system_print_uint32_5("[IK] Error, ability %d is not support now\n", p_ability->video_pipe, DC_U, DC_U, DC_U, DC_U);
                rval = IK_ERR_0008;
            }
        } else if (p_ability->pipe == AMBA_IK_PIPE_STILL) {
            amba_ik_system_print_uint32_5("[IK] Error, pipe %d is not support now\n", p_ability->pipe, DC_U, DC_U, DC_U, DC_U);
            rval = IK_ERR_0008;
        } else {
            //TBD
        }
    }

    return rval;
}

uint32 img_cfg_init_cr_memory_fence(amba_ik_cr_fence_info_t *p_fence)
{
    uint32 rval = IK_OK;

    if (p_fence == NULL) {
        amba_ik_system_print_str_5("[IK][ERROR] img_cfg_init_cr_memory_fence() detect NULL pointer", NULL, NULL, NULL, NULL, NULL);
        rval |= IK_ERR_0005;
    } else {
        p_fence[0].offset = 104UL<<2;
        p_fence[0].length = 96;

        p_fence[1].offset = 146UL<<2;
        p_fence[1].length = 56;

        p_fence[2].offset = 8UL<<2;
        p_fence[2].length = 96;

        p_fence[3].offset = 8UL<<2;
        p_fence[3].length = 96;

        p_fence[4].offset = 8UL<<2;
        p_fence[4].length = 96;

        p_fence[5].offset = 27UL<<2;
        p_fence[5].length = 20;

        p_fence[8].offset = 42UL<<2;
        p_fence[8].length = 88;

        p_fence[9].offset = 193UL<<2;
        p_fence[9].length = 124;

        p_fence[10].offset = 449UL<<2;
        p_fence[10].length = 124;

        p_fence[11].offset = 2UL<<1;
        p_fence[11].length = 124;

        p_fence[12].offset = 169UL<<1;
        p_fence[12].length = 46;

        p_fence[18].offset = 14UL<<1;
        p_fence[18].length = 100;

        p_fence[19].offset = 84UL<<1;
        p_fence[19].length = 88;

        p_fence[20].offset = 72UL<<1;
        p_fence[20].length = 112;

        p_fence[21].offset = 4UL<<1;
        p_fence[21].length = 120;

        p_fence[22].offset = 1200UL<<2;
        p_fence[22].length = 64;

        p_fence[24].offset = 44UL<<2;
        p_fence[24].length = 80;

        p_fence[25].offset = 150UL<<1;
        p_fence[25].length = 84;

        p_fence[27].offset = 34UL<<1;
        p_fence[27].length = 60;

        p_fence[28].offset = 2UL<<1;
        p_fence[28].length = 124;

        p_fence[33].offset = 80UL<<1;
        p_fence[33].length = 96;

        p_fence[34].offset = 96UL<<1;
        p_fence[34].length = 64;

        p_fence[35].offset = 8UL<<2;
        p_fence[35].length = 96;

        p_fence[36].offset = 8UL<<2;
        p_fence[36].length = 96;

        p_fence[32].offset = 2UL<<1;
        p_fence[32].length = 124;

        p_fence[37].offset = 2UL<<1;
        p_fence[37].length = 124;

        p_fence[40].offset = 2UL<<1;
        p_fence[40].length = 124;

        p_fence[41].offset = 104UL<<2;
        p_fence[41].length = 96;

        p_fence[42].offset = 146UL<<2;
        p_fence[42].length = 56;

        p_fence[43].offset = 27UL<<2;
        p_fence[43].length = 20;

        p_fence[46].offset = 42UL<<2;
        p_fence[46].length = 88;

        p_fence[47].offset = 58UL<<2;
        p_fence[47].length = 24;

        p_fence[48].offset = 449UL<<2;
        p_fence[48].length = 124;

        p_fence[49].offset = 44UL<<2;
        p_fence[49].length = 80;

        p_fence[52].offset = 182UL<<2;
        p_fence[52].length = 40;

        p_fence[54].offset = 47UL<<2;
        p_fence[54].length = 68;

        p_fence[55].offset = 465UL<<2;
        p_fence[55].length = 60;

        p_fence[56].offset = 182UL<<2;
        p_fence[56].length = 40;

        p_fence[58].offset = 1UL<<2;
        p_fence[58].length = 124;

        p_fence[59].offset = 1UL<<2;
        p_fence[59].length = 124;

        p_fence[60].offset = 0u;
        p_fence[60].length = 64;

        p_fence[61].offset = 0u;
        p_fence[61].length = 64;

        p_fence[62].offset = 0u;
        p_fence[62].length = 64;

        p_fence[63].offset = 0u;
        p_fence[63].length = 64;

        p_fence[64].offset = 0u;
        p_fence[64].length = 64;

        p_fence[65].offset = 0u;
        p_fence[65].length = 64;

        p_fence[66].offset = 0u;
        p_fence[66].length = 64;

        p_fence[67].offset = 0u;
        p_fence[67].length = 64;
    }

    return rval;
}
#endif

