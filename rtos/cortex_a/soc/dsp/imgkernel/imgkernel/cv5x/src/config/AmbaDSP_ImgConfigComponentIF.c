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
#include "AmbaDSP_imgConfig.h"
#include "AmbaDSP_ImgProcess.h"
#include "AmbaDSP_ImgUtility.h"
#include "AmbaDSP_ImgArchComponentIF.h"

#define DUMP_DEBUG_COMPOSED_CFG 0

#if DUMP_DEBUG_COMPOSED_CFG
#include "stdio.h"
#include "stdlib.h"
#endif

//for misra casting.
//#if 0
static inline uintptr ptr2uintptr(const void *p)
{
    uintptr out;
    (void)amba_ik_system_memcpy(&out, &p, sizeof(void *));
    return out;
}
/*#else
static inline uint32 ptr2uintptr(const void *p)
{
    uint32 out;
    (void)amba_ik_system_memcpy(&out, &p, sizeof(uint32));
    return out;
}
#endif
*/


static void* ik_address_add_offset(void* p_addr, int64 offset)
{

    int64 address = 0;
    void* p_address_result;

    (void)amba_ik_system_memcpy(&address, &p_addr, sizeof(void*));
    address = address + offset;
    (void)amba_ik_system_memcpy(&p_address_result, &address, sizeof(void*));

    return  p_address_result;
}

uint32 img_cfg_query_flow_control_memory_size(size_t *p_mem_size)
{
    uint32 rval = IK_OK;

    if (p_mem_size == NULL) {
        amba_ik_system_print_str_5("[IK] Detect NULL pointer!!", NULL, NULL, NULL, NULL, NULL);
        rval = IK_ERR_0005;
    } else {
        *p_mem_size = sizeof(idsp_flow_ctrl_t);
    }
    return rval;
}

uint32 img_cfg_query_flow_tbl_memory_size(const ik_ability_t *p_ability, size_t *p_mem_size)
{
    uint32 rval = IK_OK;

    if (p_mem_size == NULL) {
        amba_ik_system_print_str_5("[IK] Detect NULL pointer!!", NULL, NULL, NULL, NULL, NULL);
        rval = IK_ERR_0005;
    } else {
        if(p_ability->pipe == AMBA_IK_VIDEO_PIPE) {
            if((p_ability->video_pipe == AMBA_IK_VIDEO_PIPE_LINEAR) ||
               (p_ability->video_pipe == AMBA_IK_VIDEO_PIPE_LINEAR_CE) ||
               (p_ability->video_pipe == AMBA_IK_VIDEO_PIPE_HDR_EXPO_2) ||
               (p_ability->video_pipe == AMBA_IK_VIDEO_PIPE_HDR_EXPO_3) ||
               (p_ability->video_pipe == AMBA_IK_VIDEO_PIPE_Y2Y) ||
               (p_ability->video_pipe == AMBA_IK_VIDEO_PIPE_LINEAR_HVH) ||
               (p_ability->video_pipe == AMBA_IK_VIDEO_PIPE_LINEAR_CE_HVH) ||
               (p_ability->video_pipe == AMBA_IK_VIDEO_PIPE_HDR_EXPO_2_HVH) ||
               (p_ability->video_pipe == AMBA_IK_VIDEO_PIPE_HDR_EXPO_3_HVH) ||
               (p_ability->video_pipe == AMBA_IK_VIDEO_PIPE_Y2Y_HVH) ||
               (p_ability->video_pipe == AMBA_IK_VIDEO_PIPE_LINEAR_HHB) ||
               (p_ability->video_pipe == AMBA_IK_VIDEO_PIPE_LINEAR_CE_HHB) ||
               (p_ability->video_pipe == AMBA_IK_VIDEO_PIPE_HDR_EXPO_2_HHB) ||
               (p_ability->video_pipe == AMBA_IK_VIDEO_PIPE_HDR_EXPO_3_HHB) ||
               (p_ability->video_pipe == AMBA_IK_VIDEO_PIPE_Y2Y_HHB)) {
                *p_mem_size = sizeof(amba_ik_flow_tables_t);
            } else if((p_ability->video_pipe == AMBA_IK_VIDEO_PIPE_LINEAR_MD) ||
                      (p_ability->video_pipe == AMBA_IK_VIDEO_PIPE_LINEAR_CE_MD) ||
                      (p_ability->video_pipe == AMBA_IK_VIDEO_PIPE_HDR_EXPO_2_MD) ||
                      (p_ability->video_pipe == AMBA_IK_VIDEO_PIPE_HDR_EXPO_3_MD) ||
                      (p_ability->video_pipe == AMBA_IK_VIDEO_PIPE_FUSION) ||
                      (p_ability->video_pipe == AMBA_IK_VIDEO_PIPE_Y2Y_MD) ||
                      (p_ability->video_pipe == AMBA_IK_VIDEO_PIPE_LINEAR_MD_HHB) ||
                      (p_ability->video_pipe == AMBA_IK_VIDEO_PIPE_LINEAR_CE_MD_HHB) ||
                      (p_ability->video_pipe == AMBA_IK_VIDEO_PIPE_HDR_EXPO_2_MD_HHB) ||
                      (p_ability->video_pipe == AMBA_IK_VIDEO_PIPE_HDR_EXPO_3_MD_HHB) ||
                      (p_ability->video_pipe == AMBA_IK_VIDEO_PIPE_Y2Y_MD_HHB)) {
                *p_mem_size = sizeof(amba_ik_motion_fusion_flow_tables_t);
            } else {
                // misraC
            }
        } else if (p_ability->pipe == AMBA_IK_STILL_PIPE) {
            if (p_ability->still_pipe == AMBA_IK_STILL_PIPE_HISO) {
                *p_mem_size = sizeof(amba_ik_hiso_flow_tables_t);
            } else if (p_ability->still_pipe == AMBA_IK_STILL_PIPE_LISO) {
                *p_mem_size = sizeof(amba_ik_hiso_flow_tables_t);
            } else {
                // misraC
            }
        } else {
            // misraC
        }
    }
    return rval;
}

uint32 img_cfg_query_flow_tbl_list_memory_size(const ik_ability_t *p_ability, size_t *p_mem_size)
{
    uint32 rval = IK_OK;

    if (p_mem_size == NULL) {
        amba_ik_system_print_str_5("[IK] Detect NULL pointer!!", NULL, NULL, NULL, NULL, NULL);
        rval = IK_ERR_0005;
    } else {
        if(p_ability->pipe == AMBA_IK_VIDEO_PIPE) {
            if((p_ability->video_pipe == AMBA_IK_VIDEO_PIPE_LINEAR) ||
               (p_ability->video_pipe == AMBA_IK_VIDEO_PIPE_LINEAR_CE) ||
               (p_ability->video_pipe == AMBA_IK_VIDEO_PIPE_HDR_EXPO_2) ||
               (p_ability->video_pipe == AMBA_IK_VIDEO_PIPE_HDR_EXPO_3) ||
               (p_ability->video_pipe == AMBA_IK_VIDEO_PIPE_Y2Y) ||
               (p_ability->video_pipe == AMBA_IK_VIDEO_PIPE_LINEAR_HVH) ||
               (p_ability->video_pipe == AMBA_IK_VIDEO_PIPE_LINEAR_CE_HVH) ||
               (p_ability->video_pipe == AMBA_IK_VIDEO_PIPE_HDR_EXPO_2_HVH) ||
               (p_ability->video_pipe == AMBA_IK_VIDEO_PIPE_HDR_EXPO_3_HVH) ||
               (p_ability->video_pipe == AMBA_IK_VIDEO_PIPE_Y2Y_HVH) ||
               (p_ability->video_pipe == AMBA_IK_VIDEO_PIPE_LINEAR_HHB) ||
               (p_ability->video_pipe == AMBA_IK_VIDEO_PIPE_LINEAR_CE_HHB) ||
               (p_ability->video_pipe == AMBA_IK_VIDEO_PIPE_HDR_EXPO_2_HHB) ||
               (p_ability->video_pipe == AMBA_IK_VIDEO_PIPE_HDR_EXPO_3_HHB) ||
               (p_ability->video_pipe == AMBA_IK_VIDEO_PIPE_Y2Y_HHB)) {
                *p_mem_size = sizeof(amba_ik_flow_tables_list_t);
            } else if((p_ability->video_pipe == AMBA_IK_VIDEO_PIPE_LINEAR_MD) ||
                      (p_ability->video_pipe == AMBA_IK_VIDEO_PIPE_LINEAR_CE_MD) ||
                      (p_ability->video_pipe == AMBA_IK_VIDEO_PIPE_HDR_EXPO_2_MD) ||
                      (p_ability->video_pipe == AMBA_IK_VIDEO_PIPE_HDR_EXPO_3_MD) ||
                      (p_ability->video_pipe == AMBA_IK_VIDEO_PIPE_FUSION) ||
                      (p_ability->video_pipe == AMBA_IK_VIDEO_PIPE_Y2Y_MD) ||
                      (p_ability->video_pipe == AMBA_IK_VIDEO_PIPE_LINEAR_MD_HHB) ||
                      (p_ability->video_pipe == AMBA_IK_VIDEO_PIPE_LINEAR_CE_MD_HHB) ||
                      (p_ability->video_pipe == AMBA_IK_VIDEO_PIPE_HDR_EXPO_2_MD_HHB) ||
                      (p_ability->video_pipe == AMBA_IK_VIDEO_PIPE_HDR_EXPO_3_MD_HHB) ||
                      (p_ability->video_pipe == AMBA_IK_VIDEO_PIPE_Y2Y_MD_HHB)) {
                *p_mem_size = sizeof(amba_ik_motion_fusion_flow_tables_list_t);
            } else {
                // misraC
            }
        } else if (p_ability->pipe == AMBA_IK_STILL_PIPE) {
            if (p_ability->still_pipe == AMBA_IK_STILL_PIPE_HISO) {
                *p_mem_size = sizeof(amba_ik_hiso_flow_tables_list_t);
            } else if (p_ability->still_pipe == AMBA_IK_STILL_PIPE_LISO) {
                *p_mem_size = sizeof(amba_ik_hiso_flow_tables_list_t);
            } else {
                // misraC
            }
        } else {
            // misraC
        }
    }
    return rval;
}

uint32 img_cfg_query_crc_data_memory_size(const ik_ability_t *p_ability, size_t *p_mem_size)
{
    uint32 rval = IK_OK;

    if (p_mem_size == NULL) {
        amba_ik_system_print_str_5("[IK] Detect NULL pointer!!", NULL, NULL, NULL, NULL, NULL);
        rval = IK_ERR_0005;
    } else {
        if(p_ability->pipe == AMBA_IK_VIDEO_PIPE) {
            *p_mem_size = sizeof(idsp_crc_data_t);
        } else {
            *p_mem_size = 0;
        }
    }
    return rval;
}

uint32 img_cfg_prepare_flow_control_memory(void **p_flow_mem, uintptr mem_addr, size_t size, size_t flow_ctrl_size, uint32 init_mode)
{
    uint32 rval = IK_OK;
    idsp_flow_ctrl_t *tmp_flow;

    if ((p_flow_mem == NULL) || (mem_addr == 0U) || (size == (size_t)0)) {
        amba_ik_system_print_str_5("[IK] Detect NULL pointer!!", NULL, NULL, NULL, NULL, NULL);
        rval = IK_ERR_0005;
    } else {
        if (flow_ctrl_size < size) {
            (void)amba_ik_system_memcpy(&tmp_flow, &mem_addr, sizeof(void *));
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

uint32 img_cfg_prepare_flow_tbl_memory(void **p_flow_tbl_mem, uintptr mem_addr, size_t size, size_t flow_tbl_size, uint32 init_mode)
{
    uint32 rval = IK_OK;
    amba_ik_flow_tables_t *tmp_flow_tbl;

    if ((p_flow_tbl_mem == NULL) || (mem_addr == 0U) || (size == (size_t)0)) {
        amba_ik_system_print_str_5("[IK] Detect NULL pointer!!", NULL, NULL, NULL, NULL, NULL);
        rval = IK_ERR_0005;
    } else {
        if (flow_tbl_size < size) {
            (void)amba_ik_system_memcpy(&tmp_flow_tbl, &mem_addr, sizeof(void *));
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

uint32 img_cfg_prepare_flow_tbl_list_memory(void **p_flow_tbl_list_mem, uintptr mem_addr, size_t size, size_t flow_tbl_list_size, uint32 init_mode)
{
    uint32 rval = IK_OK;
    amba_ik_flow_tables_list_t *tmp_flow_tbl_list;

    if ((p_flow_tbl_list_mem == NULL) || (mem_addr == 0U) || (size == (size_t)0)) {
        amba_ik_system_print_str_5("[IK] Detect NULL pointer!!", NULL, NULL, NULL, NULL, NULL);
        rval = IK_ERR_0005;
    } else {
        if (flow_tbl_list_size < size) {
            (void)amba_ik_system_memcpy(&tmp_flow_tbl_list, &mem_addr, sizeof(void *));
            *p_flow_tbl_list_mem = tmp_flow_tbl_list;
            if (init_mode == AMBA_IK_ARCH_HARD_INIT) {
                (void)amba_ik_system_memset(*p_flow_tbl_list_mem, 0x0, flow_tbl_list_size); // TBD, do we need this??
            }
        } else {
            amba_ik_system_print_int32_5("[IK] You might allocate enough memory for flow table list: allocated:%d, needed:%d!!", (int32)size, (int32)flow_tbl_list_size, DC_I, DC_I, DC_I);
            rval = IK_ERR_0007;
        }
    }
    return rval;
}

uint32 img_cfg_prepare_crc_data_memory(void **p_crc_data_mem, uintptr mem_addr, size_t size, size_t crc_data_size, uint32 init_mode)
{
    uint32 rval = IK_OK;
    idsp_crc_data_t *tmp_crc_data;

    if ((p_crc_data_mem == NULL) || (mem_addr == 0U) || (size == (size_t)0)) {
        amba_ik_system_print_str_5("[IK] Detect NULL pointer!!", NULL, NULL, NULL, NULL, NULL);
        rval = IK_ERR_0005;
    } else {
        if (crc_data_size < size) {
            (void)amba_ik_system_memcpy(&tmp_crc_data, &mem_addr, sizeof(void *));
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

static uint32 img_cfg_set_idsp_step_info(const ik_ability_t *p_ability, uint32 *p_num_of_steps, uint8 *p_step_cfg_mask)
{
    uint32 rval = IK_OK;
    uint32 i;
    uint32 tmp_uint32 = 0;

    if ((p_ability == NULL) || (p_num_of_steps == NULL) || (p_step_cfg_mask == NULL)) {
        amba_ik_system_print_str_5("[IK][ERROR] img_cfg_set_idsp_step_info() detect NULL pointer", NULL, NULL, NULL, NULL, NULL);
        rval |= IK_ERR_0005;
    } else {
        if (p_ability->pipe == AMBA_IK_VIDEO_PIPE) {
            if(p_ability->video_pipe < AMBA_IK_VIDEO_PIPE_MAX) {

                for(i=0; i<(uint32)MAX_IMG_STEP; i++) {
                    p_step_cfg_mask[i] = 0;
                }

                switch (p_ability->video_pipe) {
                case AMBA_IK_VIDEO_PIPE_LINEAR:
                case AMBA_IK_VIDEO_PIPE_Y2Y:
                    *p_num_of_steps = 1;
                    tmp_uint32 = (uint32)SECT_CFG_MASK_SEC2;
                    tmp_uint32 |= (uint32)SECT_CFG_MASK_SEC3;
                    tmp_uint32 |= (uint32)SECT_CFG_MASK_SEC11;
                    tmp_uint32 |= (uint32)SECT_CFG_MASK_SEC18;
                    p_step_cfg_mask[0] = (uint8)tmp_uint32;
                    break;
                case AMBA_IK_VIDEO_PIPE_LINEAR_CE:
                case AMBA_IK_VIDEO_PIPE_HDR_EXPO_2:
                case AMBA_IK_VIDEO_PIPE_HDR_EXPO_3:
                    *p_num_of_steps = 1;
                    tmp_uint32 = (uint32)SECT_CFG_MASK_SEC2;
                    tmp_uint32 |= (uint32)SECT_CFG_MASK_SEC3;
                    tmp_uint32 |= (uint32)SECT_CFG_MASK_SEC4;
                    tmp_uint32 |= (uint32)SECT_CFG_MASK_SEC11;
                    tmp_uint32 |= (uint32)SECT_CFG_MASK_SEC18;
                    p_step_cfg_mask[0] = (uint8)tmp_uint32;
                    break;
                case AMBA_IK_VIDEO_PIPE_LINEAR_MD:
                case AMBA_IK_VIDEO_PIPE_LINEAR_MD_HHB:
                case AMBA_IK_VIDEO_PIPE_Y2Y_MD:
                case AMBA_IK_VIDEO_PIPE_Y2Y_MD_HHB:
                    *p_num_of_steps = 5;
                    tmp_uint32 = (uint32)SECT_CFG_MASK_SEC2;
                    tmp_uint32 |= (uint32)SECT_CFG_MASK_SEC3;
                    tmp_uint32 |= (uint32)SECT_CFG_MASK_SEC11;
                    tmp_uint32 |= (uint32)SECT_CFG_MASK_SEC18;
                    p_step_cfg_mask[0] = (uint8)tmp_uint32;
                    tmp_uint32 = (uint32)SECT_CFG_MASK_SEC2;
                    tmp_uint32 |= (uint32)SECT_CFG_MASK_SEC3;
                    tmp_uint32 |= (uint32)SECT_CFG_MASK_SEC18;
                    p_step_cfg_mask[1] = (uint8)tmp_uint32;
                    tmp_uint32 = (uint32)SECT_CFG_MASK_SEC2;
                    tmp_uint32 |= (uint32)SECT_CFG_MASK_SEC18;
                    p_step_cfg_mask[2] = (uint8)tmp_uint32;
                    tmp_uint32 = (uint32)SECT_CFG_MASK_SEC2;
                    tmp_uint32 |= (uint32)SECT_CFG_MASK_SEC18;
                    p_step_cfg_mask[3] = (uint8)tmp_uint32;
                    tmp_uint32 = (uint32)SECT_CFG_MASK_SEC2;
                    tmp_uint32 |= (uint32)SECT_CFG_MASK_SEC18;
                    p_step_cfg_mask[4] = (uint8)tmp_uint32;
                    break;
                case AMBA_IK_VIDEO_PIPE_LINEAR_CE_MD:
                case AMBA_IK_VIDEO_PIPE_HDR_EXPO_2_MD:
                case AMBA_IK_VIDEO_PIPE_HDR_EXPO_3_MD:
                case AMBA_IK_VIDEO_PIPE_LINEAR_CE_MD_HHB:
                case AMBA_IK_VIDEO_PIPE_HDR_EXPO_2_MD_HHB:
                case AMBA_IK_VIDEO_PIPE_HDR_EXPO_3_MD_HHB:
                    *p_num_of_steps = 5;
                    tmp_uint32 = (uint32)SECT_CFG_MASK_SEC2;
                    tmp_uint32 |= (uint32)SECT_CFG_MASK_SEC3;
                    tmp_uint32 |= (uint32)SECT_CFG_MASK_SEC4;
                    tmp_uint32 |= (uint32)SECT_CFG_MASK_SEC11;
                    tmp_uint32 |= (uint32)SECT_CFG_MASK_SEC18;
                    p_step_cfg_mask[0] = (uint8)tmp_uint32;
                    tmp_uint32 = (uint32)SECT_CFG_MASK_SEC2;
                    tmp_uint32 |= (uint32)SECT_CFG_MASK_SEC3;
                    tmp_uint32 |= (uint32)SECT_CFG_MASK_SEC18;
                    p_step_cfg_mask[1] = (uint8)tmp_uint32;
                    tmp_uint32 = (uint32)SECT_CFG_MASK_SEC2;
                    tmp_uint32 |= (uint32)SECT_CFG_MASK_SEC18;
                    p_step_cfg_mask[2] = (uint8)tmp_uint32;
                    tmp_uint32 = (uint32)SECT_CFG_MASK_SEC2;
                    tmp_uint32 |= (uint32)SECT_CFG_MASK_SEC18;
                    p_step_cfg_mask[3] = (uint8)tmp_uint32;
                    tmp_uint32 = (uint32)SECT_CFG_MASK_SEC2;
                    tmp_uint32 |= (uint32)SECT_CFG_MASK_SEC18;
                    p_step_cfg_mask[4] = (uint8)tmp_uint32;
                    break;
                case AMBA_IK_VIDEO_PIPE_FUSION:
                    *p_num_of_steps = 12;
                    tmp_uint32 = (uint32)SECT_CFG_MASK_SEC2;
                    tmp_uint32 |= (uint32)SECT_CFG_MASK_SEC3;
                    tmp_uint32 |= (uint32)SECT_CFG_MASK_SEC4;
                    tmp_uint32 |= (uint32)SECT_CFG_MASK_SEC11;
                    tmp_uint32 |= (uint32)SECT_CFG_MASK_SEC18;
                    p_step_cfg_mask[0] = (uint8)tmp_uint32;
                    tmp_uint32 = (uint32)SECT_CFG_MASK_SEC2;
                    tmp_uint32 |= (uint32)SECT_CFG_MASK_SEC3;
                    tmp_uint32 |= (uint32)SECT_CFG_MASK_SEC18;
                    p_step_cfg_mask[1] = (uint8)tmp_uint32;
                    tmp_uint32 = (uint32)SECT_CFG_MASK_SEC2;
                    tmp_uint32 |= (uint32)SECT_CFG_MASK_SEC18;
                    p_step_cfg_mask[2] = (uint8)tmp_uint32;
                    tmp_uint32 = (uint32)SECT_CFG_MASK_SEC2;
                    tmp_uint32 |= (uint32)SECT_CFG_MASK_SEC18;
                    p_step_cfg_mask[3] = (uint8)tmp_uint32;
                    tmp_uint32 = (uint32)SECT_CFG_MASK_SEC2;
                    tmp_uint32 |= (uint32)SECT_CFG_MASK_SEC18;
                    p_step_cfg_mask[4] = (uint8)tmp_uint32;
                    tmp_uint32 = (uint32)SECT_CFG_MASK_SEC2;
                    tmp_uint32 |= (uint32)SECT_CFG_MASK_SEC3;
                    tmp_uint32 |= (uint32)SECT_CFG_MASK_SEC4;
                    tmp_uint32 |= (uint32)SECT_CFG_MASK_SEC11;
                    tmp_uint32 |= (uint32)SECT_CFG_MASK_SEC18;
                    p_step_cfg_mask[5] = (uint8)tmp_uint32;
                    tmp_uint32 = (uint32)SECT_CFG_MASK_SEC2;
                    tmp_uint32 |= (uint32)SECT_CFG_MASK_SEC18;
                    p_step_cfg_mask[6] = (uint8)tmp_uint32;
                    tmp_uint32 = (uint32)SECT_CFG_MASK_SEC2;
                    p_step_cfg_mask[7] = (uint8)tmp_uint32;
                    tmp_uint32 = (uint32)SECT_CFG_MASK_SEC2;
                    p_step_cfg_mask[8] = (uint8)tmp_uint32;
                    tmp_uint32 = (uint32)SECT_CFG_MASK_SEC2;
                    tmp_uint32 |= (uint32)SECT_CFG_MASK_SEC18;
                    p_step_cfg_mask[9] = (uint8)tmp_uint32;
                    tmp_uint32 = (uint32)SECT_CFG_MASK_SEC2;
                    tmp_uint32 |= (uint32)SECT_CFG_MASK_SEC18;
                    p_step_cfg_mask[10] = (uint8)tmp_uint32;
                    tmp_uint32 = (uint32)SECT_CFG_MASK_SEC2;
                    tmp_uint32 |= (uint32)SECT_CFG_MASK_SEC3;
                    tmp_uint32 |= (uint32)SECT_CFG_MASK_SEC4;
                    tmp_uint32 |= (uint32)SECT_CFG_MASK_SEC11;
                    tmp_uint32 |= (uint32)SECT_CFG_MASK_SEC18;
                    p_step_cfg_mask[11] = (uint8)tmp_uint32;
                    break;
                case AMBA_IK_VIDEO_PIPE_LINEAR_HVH:
                case AMBA_IK_VIDEO_PIPE_LINEAR_HHB:
                case AMBA_IK_VIDEO_PIPE_Y2Y_HVH:
                case AMBA_IK_VIDEO_PIPE_Y2Y_HHB:
                    *p_num_of_steps = 1;
                    tmp_uint32 = (uint32)SECT_CFG_MASK_SEC2;
                    tmp_uint32 |= (uint32)SECT_CFG_MASK_SEC3;
                    tmp_uint32 |= (uint32)SECT_CFG_MASK_SEC11;
                    tmp_uint32 |= (uint32)SECT_CFG_MASK_SEC18;
                    p_step_cfg_mask[0] = (uint8)tmp_uint32;
                    break;
                case AMBA_IK_VIDEO_PIPE_LINEAR_CE_HVH:
                case AMBA_IK_VIDEO_PIPE_HDR_EXPO_2_HVH:
                case AMBA_IK_VIDEO_PIPE_HDR_EXPO_3_HVH:
                case AMBA_IK_VIDEO_PIPE_LINEAR_CE_HHB:
                case AMBA_IK_VIDEO_PIPE_HDR_EXPO_2_HHB:
                case AMBA_IK_VIDEO_PIPE_HDR_EXPO_3_HHB:
                    *p_num_of_steps = 1;
                    tmp_uint32 = (uint32)SECT_CFG_MASK_SEC2;
                    tmp_uint32 |= (uint32)SECT_CFG_MASK_SEC3;
                    tmp_uint32 |= (uint32)SECT_CFG_MASK_SEC4;
                    tmp_uint32 |= (uint32)SECT_CFG_MASK_SEC11;
                    tmp_uint32 |= (uint32)SECT_CFG_MASK_SEC18;
                    p_step_cfg_mask[0] = (uint8)tmp_uint32;
                    break;
                default:
                    amba_ik_system_print_uint32_5("[IK] Error, video pipe %d is not support now\n", p_ability->video_pipe, DC_U, DC_U, DC_U, DC_U);
                    rval = IK_ERR_0008;
                    break;
                }
            } else {
                amba_ik_system_print_uint32_5("[IK] Error, ability %d is not support now\n", p_ability->video_pipe, DC_U, DC_U, DC_U, DC_U);
                rval = IK_ERR_0008;
            }
        } else if (p_ability->pipe == AMBA_IK_STILL_PIPE) {
            if(p_ability->still_pipe < AMBA_IK_STILL_PIPE_MAX) {

                for(i=0; i<(uint32)MAX_IMG_STEP; i++) {
                    p_step_cfg_mask[i] = 0;
                }

                switch (p_ability->still_pipe) {
                case AMBA_IK_STILL_PIPE_HISO:
#if 0
                    *p_num_of_steps = 14;
                    // step 1
                    tmp_uint32 = (uint32)SECT_CFG_MASK_SEC2;
                    tmp_uint32 |= (uint32)SECT_CFG_MASK_SEC18;
                    p_step_cfg_mask[0] = (uint8)tmp_uint32;
                    // step 2
                    tmp_uint32 = (uint32)SECT_CFG_MASK_SEC2;
                    tmp_uint32 |= (uint32)SECT_CFG_MASK_SEC18;
                    p_step_cfg_mask[1] = (uint8)tmp_uint32;
                    // step 3
                    p_step_cfg_mask[2] = p_step_cfg_mask[1];
                    // step 4
                    p_step_cfg_mask[3] = p_step_cfg_mask[1];
                    // step 4a
                    tmp_uint32 = (uint32)SECT_CFG_MASK_SEC2;
                    p_step_cfg_mask[4] = (uint8)tmp_uint32;
                    // step 5
                    p_step_cfg_mask[5] = p_step_cfg_mask[1];
                    // step 6
                    p_step_cfg_mask[6] = p_step_cfg_mask[1];
                    // step 7
                    p_step_cfg_mask[7] = p_step_cfg_mask[1];
                    // step 8
                    p_step_cfg_mask[8] = p_step_cfg_mask[1];
                    // step 9
                    tmp_uint32 = (uint32)SECT_CFG_MASK_SEC2;
                    p_step_cfg_mask[9] = (uint8)tmp_uint32;
                    // step 10
                    p_step_cfg_mask[10] = p_step_cfg_mask[1];
                    // step 11
                    p_step_cfg_mask[11] = p_step_cfg_mask[0];
                    // step 12
                    p_step_cfg_mask[12] = p_step_cfg_mask[0];
                    // step 13
                    tmp_uint32 = (uint32)SECT_CFG_MASK_SEC2;
                    tmp_uint32 |= (uint32)SECT_CFG_MASK_SEC3;
                    tmp_uint32 |= (uint32)SECT_CFG_MASK_SEC18;
                    p_step_cfg_mask[13] = (uint8)tmp_uint32;
                    break;
                case AMBA_IK_STILL_HISO_CE:
#endif
#ifdef _HISO_SEC3_IN_STEP13
                    *p_num_of_steps = 14;
#else
                    *p_num_of_steps = 15;
#endif
                    // step 1
                    tmp_uint32 = (uint32)SECT_CFG_MASK_SEC2;
                    tmp_uint32 |= (uint32)SECT_CFG_MASK_SEC4;
                    tmp_uint32 |= (uint32)SECT_CFG_MASK_SEC18;
                    p_step_cfg_mask[0] = (uint8)tmp_uint32;
                    // step 2
                    tmp_uint32 = (uint32)SECT_CFG_MASK_SEC2;
                    tmp_uint32 |= (uint32)SECT_CFG_MASK_SEC18;
                    p_step_cfg_mask[1] = (uint8)tmp_uint32;
                    // step 3
                    p_step_cfg_mask[2] = p_step_cfg_mask[1];
                    // step 4
                    p_step_cfg_mask[3] = p_step_cfg_mask[1];
                    // step 4a
                    tmp_uint32 = (uint32)SECT_CFG_MASK_SEC2;
                    p_step_cfg_mask[4] = (uint8)tmp_uint32;
                    // step 5
                    p_step_cfg_mask[5] = p_step_cfg_mask[1];
                    // step 6
                    p_step_cfg_mask[6] = p_step_cfg_mask[1];
                    // step 7
                    p_step_cfg_mask[7] = p_step_cfg_mask[1];
                    // step 8
                    p_step_cfg_mask[8] = p_step_cfg_mask[1];
                    // step 9
                    tmp_uint32 = (uint32)SECT_CFG_MASK_SEC2;
                    p_step_cfg_mask[9] = (uint8)tmp_uint32;
                    // step 10
                    p_step_cfg_mask[10] = p_step_cfg_mask[1];
                    // step 11
                    p_step_cfg_mask[11] = p_step_cfg_mask[0];
                    // step 12
                    p_step_cfg_mask[12] = p_step_cfg_mask[0];
#ifdef _HISO_SEC3_IN_STEP13
                    // step 13
                    tmp_uint32 = (uint32)SECT_CFG_MASK_SEC2;
                    tmp_uint32 |= (uint32)SECT_CFG_MASK_SEC3;
                    tmp_uint32 |= (uint32)SECT_CFG_MASK_SEC18;
                    p_step_cfg_mask[13] = (uint8)tmp_uint32;
                    // step 14
                    tmp_uint32 = 0u;
                    p_step_cfg_mask[14] = (uint8)tmp_uint32;
#else
                    // step 13
                    tmp_uint32 = (uint32)SECT_CFG_MASK_SEC2;
                    tmp_uint32 |= (uint32)SECT_CFG_MASK_SEC18;
                    p_step_cfg_mask[13] = (uint8)tmp_uint32;
                    // step 14
                    tmp_uint32 = (uint32)SECT_CFG_MASK_SEC3;
                    tmp_uint32 |= (uint32)SECT_CFG_MASK_SEC18;
                    p_step_cfg_mask[14] = (uint8)tmp_uint32;
#endif
                    break;
                case AMBA_IK_STILL_PIPE_LISO:
                    // step 1
                    *p_num_of_steps = 1;
                    tmp_uint32 = (uint32)SECT_CFG_MASK_SEC2;
                    tmp_uint32 |= (uint32)SECT_CFG_MASK_SEC3;
                    tmp_uint32 |= (uint32)SECT_CFG_MASK_SEC4;
                    tmp_uint32 |= (uint32)SECT_CFG_MASK_SEC11;
                    tmp_uint32 |= (uint32)SECT_CFG_MASK_SEC18;
                    p_step_cfg_mask[0] = (uint8)tmp_uint32;
                    break;
                default:
                    amba_ik_system_print_uint32_5("[IK] Error, still pipe %d is not support now\n", p_ability->still_pipe, DC_U, DC_U, DC_U, DC_U);
                    rval = IK_ERR_0008;
                    break;
                }
            } else {
                amba_ik_system_print_uint32_5("[IK] Error, ability %d is not support now\n", p_ability->still_pipe, DC_U, DC_U, DC_U, DC_U);
                rval = IK_ERR_0008;
            }
        } else {
            //TBD
        }
    }

    return rval;
}

static uint32 img_cfg_compose_sec2_cfg(void *p_sec2_cr, const void *p_bin_data_dram_addr)
{
    uint32 rval = IK_OK;
    uint64 def_addr = 0;
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

        fwrite(p_sec2_cr, sizeof(uint8_t), MAX_SEC_2_CFG_SZ, pFileDbg);

        fclose(pFileDbg);
    }
#endif

    return rval;
}

static uint32 img_cfg_compose_sec3_cfg(void *p_sec3_cr, const void *p_bin_data_dram_addr)
{
    uint32 rval = IK_OK;
    uint64 def_addr = 0;
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

        fwrite(p_sec3_cr, sizeof(uint8_t), MAX_SEC_3_CFG_SZ, pFileDbg);

        fclose(pFileDbg);
    }
#endif

    return rval;
}

static uint32 img_cfg_compose_sec4_cfg(void *p_sec4_cr, const void *p_bin_data_dram_addr)
{
    uint32 rval = IK_OK;
    uint64 def_addr = 0;
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

        fwrite(p_sec4_cr, sizeof(uint8_t), MAX_SEC_4_CFG_SZ, pFileDbg);

        fclose(pFileDbg);
    }
#endif

    return rval;
}

static uint32 img_cfg_compose_sec11_cfg(void *p_sec11_cr, const void *p_bin_data_dram_addr)
{
    uint32 rval = IK_OK;
    uint64 def_addr = 0;
    const void *p_def_addr = NULL;

    (void)amba_ik_system_memcpy(&def_addr, &p_bin_data_dram_addr, sizeof(void *));
    def_addr += (BIN_CFG_START_OFFSET + SEC_11_BIN_CFG_OFFSET);
    (void)amba_ik_system_memcpy(&p_def_addr, &def_addr, sizeof(void *));

    (void)amba_ik_system_memcpy(p_sec11_cr, p_def_addr, MAX_SEC_11_CFG_SZ);

#if DUMP_DEBUG_COMPOSED_CFG
    {
        FILE *pFileDbg;
        char fname[64];

        sprintf(fname, "_ikc_compose_sec11.bin");

        pFileDbg = fopen(fname, "wb");
        if (pFileDbg == NULL) {
            fputs("File error pFileDbg\n", stderr);
            return 1;
        }

        fwrite(p_sec11_cr, sizeof(uint8_t), MAX_SEC_11_CFG_SZ, pFileDbg);

        fclose(pFileDbg);
    }
#endif

    return rval;
}

static uint32 img_cfg_compose_sec18_cfg(void *p_sec18_cr, const void *p_bin_data_dram_addr)
{
    uint32 rval = IK_OK;
    uint32 tmp_uint32;
    uint64 def_addr = 0;
    const void *p_def_addr = NULL;
    uint32 cfg_size = SEC_18_BIN_CFG_OFFSET;

    (void)amba_ik_system_memcpy(&def_addr, &p_bin_data_dram_addr, sizeof(void *));
    tmp_uint32 = BIN_CFG_START_OFFSET + cfg_size;
    def_addr += (uint64)tmp_uint32;
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

        fwrite(p_sec18_cr, sizeof(uint8_t), MAX_SEC_18_CFG_SZ, pFileDbg);

        fclose(pFileDbg);
    }
#endif

    return rval;
}

static void img_cfg_compose_hiso_r2y_sec2_cfg(const amba_ik_step1_sec2_cr_list_t *p_flow_tbl_list, const void *p_bin_data_dram_addr)
{
    void* tmp_ptr;

    (void)amba_ik_system_memcpy(&tmp_ptr, &p_flow_tbl_list->p_CR_buf_4, sizeof(void*));
    (void)img_cfg_compose_sec2_cfg(tmp_ptr, p_bin_data_dram_addr);
}

static void img_cfg_compose_hiso_r2y_sec4_cfg(const amba_ik_sec4_cr_list_t *p_flow_tbl_list, const void *p_bin_data_dram_addr)
{
    void* tmp_ptr;

    (void)amba_ik_system_memcpy(&tmp_ptr, &p_flow_tbl_list->p_CR_buf_45, sizeof(void*));
    (void)img_cfg_compose_sec4_cfg(tmp_ptr, p_bin_data_dram_addr);
}

static void img_cfg_compose_hiso_y2y_cc_sec2_cfg(const amba_ik_step2_sec2_cr_list_t *p_flow_tbl_list, const void *p_bin_data_dram_addr)
{
    void* tmp_ptr;
    uint64 def_addr = 0;
    const void *p_def_addr = NULL;
    uint32 size = 0;

    (void)amba_ik_system_memcpy(&def_addr, &p_bin_data_dram_addr, sizeof(void *));
    def_addr += (BIN_CFG_START_OFFSET + SEC_2_BIN_CFG_OFFSET + CR_OFFSET_26);
    (void)amba_ik_system_memcpy(&p_def_addr, &def_addr, sizeof(void *));

    size = (CR_SIZE_26 + CR_SIZE_27);
    (void)amba_ik_system_memcpy(&tmp_ptr, &p_flow_tbl_list->p_CR_buf_26, sizeof(void*));
    (void)amba_ik_system_memcpy(tmp_ptr, p_def_addr, size);


    (void)amba_ik_system_memcpy(&def_addr, &p_bin_data_dram_addr, sizeof(void *));
    def_addr += (BIN_CFG_START_OFFSET + SEC_2_BIN_CFG_OFFSET + CR_OFFSET_29);
    (void)amba_ik_system_memcpy(&p_def_addr, &def_addr, sizeof(void *));

    size = (CR_SIZE_29 + CR_SIZE_30 + CR_SIZE_31 + CR_SIZE_32 + CR_SIZE_33);
    (void)amba_ik_system_memcpy(&tmp_ptr, &p_flow_tbl_list->p_CR_buf_29, sizeof(void*));
    (void)amba_ik_system_memcpy(tmp_ptr, p_def_addr, size);


    (void)amba_ik_system_memcpy(&def_addr, &p_bin_data_dram_addr, sizeof(void *));
    def_addr += (BIN_CFG_START_OFFSET + SEC_2_BIN_CFG_OFFSET + CR_OFFSET_35);
    (void)amba_ik_system_memcpy(&p_def_addr, &def_addr, sizeof(void *));

    size = (CR_SIZE_35 + CR_SIZE_36 + CR_SIZE_37 + CR_SIZE_38 + CR_SIZE_39 + CR_SIZE_40 + CR_SIZE_41);
    (void)amba_ik_system_memcpy(&tmp_ptr, &p_flow_tbl_list->p_CR_buf_35, sizeof(void*));
    (void)amba_ik_system_memcpy(tmp_ptr, p_def_addr, size);
}

static void img_cfg_compose_hiso_y2y_sec2_cfg(const amba_ik_step3_sec2_cr_list_t *p_flow_tbl_list, const void *p_bin_data_dram_addr)
{
    void* tmp_ptr;
    uint64 def_addr = 0;
    const void *p_def_addr = NULL;
    uint32 size = 0;

    (void)amba_ik_system_memcpy(&def_addr, &p_bin_data_dram_addr, sizeof(void *));
    def_addr += (BIN_CFG_START_OFFSET + SEC_2_BIN_CFG_OFFSET + CR_OFFSET_26);
    (void)amba_ik_system_memcpy(&p_def_addr, &def_addr, sizeof(void *));

    (void)amba_ik_system_memcpy(&tmp_ptr, &p_flow_tbl_list->p_CR_buf_26, sizeof(void*));
    (void)amba_ik_system_memcpy(tmp_ptr, p_def_addr, CR_SIZE_26);


    (void)amba_ik_system_memcpy(&def_addr, &p_bin_data_dram_addr, sizeof(void *));
    def_addr += (BIN_CFG_START_OFFSET + SEC_2_BIN_CFG_OFFSET + CR_OFFSET_29);
    (void)amba_ik_system_memcpy(&p_def_addr, &def_addr, sizeof(void *));

    size = (CR_SIZE_29 + CR_SIZE_30 + CR_SIZE_31 + CR_SIZE_32 + CR_SIZE_33);
    (void)amba_ik_system_memcpy(&tmp_ptr, &p_flow_tbl_list->p_CR_buf_29, sizeof(void*));
    (void)amba_ik_system_memcpy(tmp_ptr, p_def_addr, size);


    (void)amba_ik_system_memcpy(&def_addr, &p_bin_data_dram_addr, sizeof(void *));
    def_addr += (BIN_CFG_START_OFFSET + SEC_2_BIN_CFG_OFFSET + CR_OFFSET_35);
    (void)amba_ik_system_memcpy(&p_def_addr, &def_addr, sizeof(void *));

    size = (CR_SIZE_35 + CR_SIZE_36 + CR_SIZE_37 + CR_SIZE_38 + CR_SIZE_39 + CR_SIZE_40 + CR_SIZE_41);
    (void)amba_ik_system_memcpy(&tmp_ptr, &p_flow_tbl_list->p_CR_buf_35, sizeof(void*));
    (void)amba_ik_system_memcpy(tmp_ptr, p_def_addr, size);
}

static void img_cfg_compose_hiso_step1_cfg(amba_ik_hiso_step1_cr_list_t *p_flow_tbl_list, const void *p_bin_data_dram_addr)
{
    void* tmp_ptr;
    uint64 def_addr = 0;
    const void *p_def_addr = NULL;
    uint32 size = 0;

    img_cfg_compose_hiso_r2y_sec2_cfg(&p_flow_tbl_list->sec2, p_bin_data_dram_addr);
    img_cfg_compose_hiso_r2y_sec4_cfg(&p_flow_tbl_list->sec4, p_bin_data_dram_addr);

    (void)amba_ik_system_memcpy(&def_addr, &p_bin_data_dram_addr, sizeof(void *));
    def_addr += (BIN_CFG_START_OFFSET + SEC_18_BIN_CFG_OFFSET + CR_OFFSET_112);
    (void)amba_ik_system_memcpy(&p_def_addr, &def_addr, sizeof(void *));

    size = CR_SIZE_112;
    (void)amba_ik_system_memcpy(&tmp_ptr, &p_flow_tbl_list->sec18.p_CR_buf_112, sizeof(void*));
    (void)amba_ik_system_memcpy(tmp_ptr, p_def_addr, size);


    (void)amba_ik_system_memcpy(&def_addr, &p_bin_data_dram_addr, sizeof(void *));
    def_addr += (BIN_CFG_START_OFFSET + SEC_18_BIN_CFG_OFFSET + CR_OFFSET_117);
    (void)amba_ik_system_memcpy(&p_def_addr, &def_addr, sizeof(void *));

    size = (CR_SIZE_117 + CR_SIZE_118);
    (void)amba_ik_system_memcpy(&tmp_ptr, &p_flow_tbl_list->sec18.p_CR_buf_117, sizeof(void*));
    (void)amba_ik_system_memcpy(tmp_ptr, p_def_addr, size);
}

static void img_cfg_compose_hiso_step2_cfg(amba_ik_hiso_step2_cr_list_t *p_flow_tbl_list, const void *p_bin_data_dram_addr)
{
    void* tmp_ptr;
    uint64 def_addr = 0;
    const void *p_def_addr = NULL;
    uint32 size = 0;

    img_cfg_compose_hiso_y2y_cc_sec2_cfg(&p_flow_tbl_list->sec2, p_bin_data_dram_addr);

    (void)amba_ik_system_memcpy(&def_addr, &p_bin_data_dram_addr, sizeof(void *));
    def_addr += (BIN_CFG_START_OFFSET + SEC_18_BIN_CFG_OFFSET + CR_OFFSET_112);
    (void)amba_ik_system_memcpy(&p_def_addr, &def_addr, sizeof(void *));

    size = CR_SIZE_112;
    (void)amba_ik_system_memcpy(&tmp_ptr, &p_flow_tbl_list->sec18.p_CR_buf_112, sizeof(void*));
    (void)amba_ik_system_memcpy(tmp_ptr, p_def_addr, size);


    (void)amba_ik_system_memcpy(&def_addr, &p_bin_data_dram_addr, sizeof(void *));
    def_addr += (BIN_CFG_START_OFFSET + SEC_18_BIN_CFG_OFFSET + CR_OFFSET_117);
    (void)amba_ik_system_memcpy(&p_def_addr, &def_addr, sizeof(void *));

    size = (CR_SIZE_117 + CR_SIZE_118);
    (void)amba_ik_system_memcpy(&tmp_ptr, &p_flow_tbl_list->sec18.p_CR_buf_117, sizeof(void*));
    (void)amba_ik_system_memcpy(tmp_ptr, p_def_addr, size);
}

static void img_cfg_compose_hiso_step3_cfg(amba_ik_hiso_step3_cr_list_t *p_flow_tbl_list, const void *p_bin_data_dram_addr)
{
    void* tmp_ptr;
    uint64 def_addr = 0;
    const void *p_def_addr = NULL;
    uint32 size = 0;

    img_cfg_compose_hiso_y2y_sec2_cfg(&p_flow_tbl_list->sec2, p_bin_data_dram_addr);

    (void)amba_ik_system_memcpy(&def_addr, &p_bin_data_dram_addr, sizeof(void *));
    def_addr += (BIN_CFG_START_OFFSET + SEC_18_BIN_CFG_OFFSET + CR_OFFSET_112);
    (void)amba_ik_system_memcpy(&p_def_addr, &def_addr, sizeof(void *));

    size = CR_SIZE_112;
    (void)amba_ik_system_memcpy(&tmp_ptr, &p_flow_tbl_list->sec18.p_CR_buf_112, sizeof(void*));
    (void)amba_ik_system_memcpy(tmp_ptr, p_def_addr, size);


    (void)amba_ik_system_memcpy(&def_addr, &p_bin_data_dram_addr, sizeof(void *));
    def_addr += (BIN_CFG_START_OFFSET + SEC_18_BIN_CFG_OFFSET + CR_OFFSET_117);
    (void)amba_ik_system_memcpy(&p_def_addr, &def_addr, sizeof(void *));

    size = (CR_SIZE_117 + CR_SIZE_118);
    (void)amba_ik_system_memcpy(&tmp_ptr, &p_flow_tbl_list->sec18.p_CR_buf_117, sizeof(void*));
    (void)amba_ik_system_memcpy(tmp_ptr, p_def_addr, size);
}

static void img_cfg_compose_hiso_step4a_cfg(const amba_ik_hiso_step4a_cr_list_t *p_flow_tbl_list, const void *p_bin_data_dram_addr)
{
    img_cfg_compose_hiso_y2y_sec2_cfg(&p_flow_tbl_list->sec2, p_bin_data_dram_addr);
}

static void img_cfg_compose_hiso_step5_cfg(amba_ik_hiso_step5_cr_list_t *p_flow_tbl_list, const void *p_bin_data_dram_addr)
{
    void* tmp_ptr;
    uint64 def_addr = 0;
    const void *p_def_addr = NULL;
    uint32 size = 0;

    img_cfg_compose_hiso_y2y_sec2_cfg(&p_flow_tbl_list->sec2, p_bin_data_dram_addr);

    (void)amba_ik_system_memcpy(&def_addr, &p_bin_data_dram_addr, sizeof(void *));
    def_addr += (BIN_CFG_START_OFFSET + SEC_18_BIN_CFG_OFFSET + CR_OFFSET_112);
    (void)amba_ik_system_memcpy(&p_def_addr, &def_addr, sizeof(void *));

    size = CR_SIZE_112;
    (void)amba_ik_system_memcpy(&tmp_ptr, &p_flow_tbl_list->sec18.p_CR_buf_112, sizeof(void*));
    (void)amba_ik_system_memcpy(tmp_ptr, p_def_addr, size);


    (void)amba_ik_system_memcpy(&def_addr, &p_bin_data_dram_addr, sizeof(void *));
    def_addr += (BIN_CFG_START_OFFSET + SEC_18_BIN_CFG_OFFSET + CR_OFFSET_114);
    (void)amba_ik_system_memcpy(&p_def_addr, &def_addr, sizeof(void *));

    size = CR_SIZE_114;
    (void)amba_ik_system_memcpy(&tmp_ptr, &p_flow_tbl_list->sec18.p_CR_buf_114, sizeof(void*));
    (void)amba_ik_system_memcpy(tmp_ptr, p_def_addr, size);


    (void)amba_ik_system_memcpy(&def_addr, &p_bin_data_dram_addr, sizeof(void *));
    def_addr += (BIN_CFG_START_OFFSET + SEC_18_BIN_CFG_OFFSET + CR_OFFSET_117);
    (void)amba_ik_system_memcpy(&p_def_addr, &def_addr, sizeof(void *));

    size = (CR_SIZE_117 + CR_SIZE_118);
    (void)amba_ik_system_memcpy(&tmp_ptr, &p_flow_tbl_list->sec18.p_CR_buf_117, sizeof(void*));
    (void)amba_ik_system_memcpy(tmp_ptr, p_def_addr, size);
}

static void img_cfg_compose_hiso_step6_cfg(amba_ik_hiso_step6_cr_list_t *p_flow_tbl_list, const void *p_bin_data_dram_addr)
{
    void* tmp_ptr;
    uint64 def_addr = 0;
    const void *p_def_addr = NULL;
    uint32 size = 0;

    img_cfg_compose_hiso_y2y_sec2_cfg(&p_flow_tbl_list->sec2, p_bin_data_dram_addr);

    (void)amba_ik_system_memcpy(&def_addr, &p_bin_data_dram_addr, sizeof(void *));
    def_addr += (BIN_CFG_START_OFFSET + SEC_18_BIN_CFG_OFFSET + CR_OFFSET_112);
    (void)amba_ik_system_memcpy(&p_def_addr, &def_addr, sizeof(void *));

    size = CR_SIZE_112;
    (void)amba_ik_system_memcpy(&tmp_ptr, &p_flow_tbl_list->sec18.p_CR_buf_112, sizeof(void*));
    (void)amba_ik_system_memcpy(tmp_ptr, p_def_addr, size);


    (void)amba_ik_system_memcpy(&def_addr, &p_bin_data_dram_addr, sizeof(void *));
    def_addr += (BIN_CFG_START_OFFSET + SEC_18_BIN_CFG_OFFSET + CR_OFFSET_114);
    (void)amba_ik_system_memcpy(&p_def_addr, &def_addr, sizeof(void *));

    size = CR_SIZE_114;
    (void)amba_ik_system_memcpy(&tmp_ptr, &p_flow_tbl_list->sec18.p_CR_buf_114, sizeof(void*));
    (void)amba_ik_system_memcpy(tmp_ptr, p_def_addr, size);


    (void)amba_ik_system_memcpy(&def_addr, &p_bin_data_dram_addr, sizeof(void *));
    def_addr += (BIN_CFG_START_OFFSET + SEC_18_BIN_CFG_OFFSET + CR_OFFSET_117);
    (void)amba_ik_system_memcpy(&p_def_addr, &def_addr, sizeof(void *));

    size = CR_SIZE_117;
    (void)amba_ik_system_memcpy(&tmp_ptr, &p_flow_tbl_list->sec18.p_CR_buf_117, sizeof(void*));
    (void)amba_ik_system_memcpy(tmp_ptr, p_def_addr, size);
}

static void img_cfg_compose_hiso_step11_cfg(amba_ik_hiso_step11_cr_list_t *p_flow_tbl_list, const void *p_bin_data_dram_addr)
{
    void* tmp_ptr;
    uint64 def_addr = 0;
    const void *p_def_addr = NULL;
    uint32 size = 0;

    img_cfg_compose_hiso_r2y_sec2_cfg(&p_flow_tbl_list->sec2, p_bin_data_dram_addr);
    img_cfg_compose_hiso_r2y_sec4_cfg(&p_flow_tbl_list->sec4, p_bin_data_dram_addr);

    (void)amba_ik_system_memcpy(&def_addr, &p_bin_data_dram_addr, sizeof(void *));
    def_addr += (BIN_CFG_START_OFFSET + SEC_18_BIN_CFG_OFFSET + CR_OFFSET_112);
    (void)amba_ik_system_memcpy(&p_def_addr, &def_addr, sizeof(void *));

    size = CR_SIZE_112;
    (void)amba_ik_system_memcpy(&tmp_ptr, &p_flow_tbl_list->sec18.p_CR_buf_112, sizeof(void*));
    (void)amba_ik_system_memcpy(tmp_ptr, p_def_addr, size);


    (void)amba_ik_system_memcpy(&def_addr, &p_bin_data_dram_addr, sizeof(void *));
    def_addr += (BIN_CFG_START_OFFSET + SEC_18_BIN_CFG_OFFSET + CR_OFFSET_114);
    (void)amba_ik_system_memcpy(&p_def_addr, &def_addr, sizeof(void *));

    size = CR_SIZE_114;
    (void)amba_ik_system_memcpy(&tmp_ptr, &p_flow_tbl_list->sec18.p_CR_buf_114, sizeof(void*));
    (void)amba_ik_system_memcpy(tmp_ptr, p_def_addr, size);


    (void)amba_ik_system_memcpy(&def_addr, &p_bin_data_dram_addr, sizeof(void *));
    def_addr += (BIN_CFG_START_OFFSET + SEC_18_BIN_CFG_OFFSET + CR_OFFSET_117);
    (void)amba_ik_system_memcpy(&p_def_addr, &def_addr, sizeof(void *));

    size = (CR_SIZE_117 + CR_SIZE_118);
    (void)amba_ik_system_memcpy(&tmp_ptr, &p_flow_tbl_list->sec18.p_CR_buf_117, sizeof(void*));
    (void)amba_ik_system_memcpy(tmp_ptr, p_def_addr, size);
}

static void img_cfg_compose_hiso_step13_cfg(amba_ik_hiso_step13_cr_list_t *p_flow_tbl_list, const void *p_bin_data_dram_addr)
{
    void* tmp_ptr;
    uint64 def_addr = 0;
    const void *p_def_addr = NULL;
    uint32 size = 0;

    img_cfg_compose_hiso_y2y_sec2_cfg(&p_flow_tbl_list->sec2, p_bin_data_dram_addr);

#ifdef _HISO_SEC3_IN_STEP13
    (void)amba_ik_system_memcpy(&tmp_ptr, &p_flow_tbl_list->sec3.p_CR_buf_42, sizeof(void*));
    (void)img_cfg_compose_sec3_cfg(tmp_ptr, p_bin_data_dram_addr);
#endif

    (void)amba_ik_system_memcpy(&def_addr, &p_bin_data_dram_addr, sizeof(void *));
    def_addr += (BIN_CFG_START_OFFSET + SEC_18_BIN_CFG_OFFSET + CR_OFFSET_112);
    (void)amba_ik_system_memcpy(&p_def_addr, &def_addr, sizeof(void *));

    size = CR_SIZE_112;
    (void)amba_ik_system_memcpy(&tmp_ptr, &p_flow_tbl_list->sec18.p_CR_buf_112, sizeof(void*));
    (void)amba_ik_system_memcpy(tmp_ptr, p_def_addr, size);


    (void)amba_ik_system_memcpy(&def_addr, &p_bin_data_dram_addr, sizeof(void *));
    def_addr += (BIN_CFG_START_OFFSET + SEC_18_BIN_CFG_OFFSET + CR_OFFSET_114);
    (void)amba_ik_system_memcpy(&p_def_addr, &def_addr, sizeof(void *));

    size = CR_SIZE_114;
    (void)amba_ik_system_memcpy(&tmp_ptr, &p_flow_tbl_list->sec18.p_CR_buf_114, sizeof(void*));
    (void)amba_ik_system_memcpy(tmp_ptr, p_def_addr, size);

    (void)amba_ik_system_memcpy(&def_addr, &p_bin_data_dram_addr, sizeof(void *));
    def_addr += (BIN_CFG_START_OFFSET + SEC_18_BIN_CFG_OFFSET + CR_OFFSET_117);
    (void)amba_ik_system_memcpy(&p_def_addr, &def_addr, sizeof(void *));

    size = CR_SIZE_117;
    (void)amba_ik_system_memcpy(&tmp_ptr, &p_flow_tbl_list->sec18.p_CR_buf_117, sizeof(void*));
    (void)amba_ik_system_memcpy(tmp_ptr, p_def_addr, size);
}

static void img_cfg_compose_hiso_step14_cfg(const amba_ik_hiso_step14_cr_list_t *p_flow_tbl_list, const void *p_bin_data_dram_addr)
{
    void* tmp_ptr;
    //uint64 def_addr = 0;
    //const void *p_def_addr = NULL;
    //uint32 size = 0;

    (void)amba_ik_system_memcpy(&tmp_ptr, &p_flow_tbl_list->sec3.p_CR_buf_42, sizeof(void*));
    (void)img_cfg_compose_sec3_cfg(tmp_ptr, p_bin_data_dram_addr);

#if 0
    (void)amba_ik_system_memcpy(&def_addr, &p_bin_data_dram_addr, sizeof(void *));
    def_addr += (BIN_CFG_START_OFFSET + SEC_18_BIN_CFG_OFFSET + CR_OFFSET_112);
    (void)amba_ik_system_memcpy(&p_def_addr, &def_addr, sizeof(void *));

    size = CR_SIZE_112;
    tmp_p_u8 = p_flow_tbl->sec18.CR_buf_112;
    (void)amba_ik_system_memcpy(&tmp_ptr, &tmp_p_u8, sizeof(void*));
    (void)amba_ik_system_memcpy(tmp_ptr, p_def_addr, size);


    (void)amba_ik_system_memcpy(&def_addr, &p_bin_data_dram_addr, sizeof(void *));
    def_addr += (BIN_CFG_START_OFFSET + SEC_18_BIN_CFG_OFFSET + CR_OFFSET_117);
    (void)amba_ik_system_memcpy(&p_def_addr, &def_addr, sizeof(void *));

    size = CR_SIZE_117;
    tmp_p_u8 = p_flow_tbl->sec18.CR_buf_117;
    (void)amba_ik_system_memcpy(&tmp_ptr, &tmp_p_u8, sizeof(void*));
    (void)amba_ik_system_memcpy(tmp_ptr, p_def_addr, size);
#endif
}

static uint32 img_cfg_compose_hiso_cfg(amba_ik_hiso_flow_tables_list_t *p_flow_tbl_list, const void *p_bin_data_dram_addr)
{
    static const uint8 step2_COLOR_COR_IN_LOOKUP[] = {
        0x00u, 0x10u, 0x00u, 0x00u, 0x02u, 0x30u, 0x00u, 0x00u, 0x04u, 0x50u, 0x00u, 0x00u, 0x06u, 0x70u, 0x00u, 0x00u,
        0x08u, 0x90u, 0x00u, 0x00u, 0x0au, 0xb0u, 0x00u, 0x00u, 0x0cu, 0xd0u, 0x00u, 0x00u, 0x0eu, 0xf0u, 0x00u, 0x00u,
        0x10u, 0x10u, 0x01u, 0x00u, 0x12u, 0x30u, 0x01u, 0x00u, 0x14u, 0x50u, 0x01u, 0x00u, 0x16u, 0x70u, 0x01u, 0x00u,
        0x18u, 0x90u, 0x01u, 0x00u, 0x1au, 0xb0u, 0x01u, 0x00u, 0x1cu, 0xd0u, 0x01u, 0x00u, 0x1eu, 0xf0u, 0x01u, 0x00u,
        0x20u, 0x10u, 0x02u, 0x00u, 0x22u, 0x30u, 0x02u, 0x00u, 0x24u, 0x50u, 0x02u, 0x00u, 0x26u, 0x70u, 0x02u, 0x00u,
        0x28u, 0x90u, 0x02u, 0x00u, 0x2au, 0xb0u, 0x02u, 0x00u, 0x2cu, 0xd0u, 0x02u, 0x00u, 0x2eu, 0xf0u, 0x02u, 0x00u,
        0x30u, 0x10u, 0x03u, 0x00u, 0x32u, 0x30u, 0x03u, 0x00u, 0x34u, 0x50u, 0x03u, 0x00u, 0x36u, 0x70u, 0x03u, 0x00u,
        0x38u, 0x90u, 0x03u, 0x00u, 0x3au, 0xb0u, 0x03u, 0x00u, 0x3cu, 0xd0u, 0x03u, 0x00u, 0x3eu, 0xf0u, 0x03u, 0x00u,
        0x40u, 0x20u, 0x04u, 0x00u, 0x44u, 0x60u, 0x04u, 0x00u, 0x48u, 0xa0u, 0x04u, 0x00u, 0x4cu, 0xe0u, 0x04u, 0x00u,
        0x50u, 0x20u, 0x05u, 0x00u, 0x54u, 0x60u, 0x05u, 0x00u, 0x58u, 0xa0u, 0x05u, 0x00u, 0x5cu, 0xe0u, 0x05u, 0x00u,
        0x60u, 0x20u, 0x06u, 0x00u, 0x64u, 0x60u, 0x06u, 0x00u, 0x68u, 0xa0u, 0x06u, 0x00u, 0x6cu, 0xe0u, 0x06u, 0x00u,
        0x70u, 0x20u, 0x07u, 0x00u, 0x74u, 0x60u, 0x07u, 0x00u, 0x78u, 0xa0u, 0x07u, 0x00u, 0x7cu, 0xe0u, 0x07u, 0x00u,
        0x80u, 0x40u, 0x08u, 0x00u, 0x88u, 0xc0u, 0x08u, 0x00u, 0x90u, 0x40u, 0x09u, 0x00u, 0x98u, 0xc0u, 0x09u, 0x00u,
        0xa0u, 0x40u, 0x0au, 0x00u, 0xa8u, 0xc0u, 0x0au, 0x00u, 0xb0u, 0x40u, 0x0bu, 0x00u, 0xb8u, 0xc0u, 0x0bu, 0x00u,
        0xc0u, 0x40u, 0x0cu, 0x00u, 0xc8u, 0xc0u, 0x0cu, 0x00u, 0xd0u, 0x40u, 0x0du, 0x00u, 0xd8u, 0xc0u, 0x0du, 0x00u,
        0xe0u, 0x40u, 0x0eu, 0x00u, 0xe8u, 0xc0u, 0x0eu, 0x00u, 0xf0u, 0x40u, 0x0fu, 0x00u, 0xf8u, 0xc0u, 0x0fu, 0x00u,
        0x00u, 0x81u, 0x10u, 0x00u, 0x10u, 0x81u, 0x11u, 0x00u, 0x20u, 0x81u, 0x12u, 0x00u, 0x30u, 0x81u, 0x13u, 0x00u,
        0x40u, 0x81u, 0x14u, 0x00u, 0x50u, 0x81u, 0x15u, 0x00u, 0x60u, 0x81u, 0x16u, 0x00u, 0x70u, 0x81u, 0x17u, 0x00u,
        0x80u, 0x81u, 0x18u, 0x00u, 0x90u, 0x81u, 0x19u, 0x00u, 0xa0u, 0x81u, 0x1au, 0x00u, 0xb0u, 0x81u, 0x1bu, 0x00u,
        0xc0u, 0x81u, 0x1cu, 0x00u, 0xd0u, 0x81u, 0x1du, 0x00u, 0xe0u, 0x81u, 0x1eu, 0x00u, 0xf0u, 0x81u, 0x1fu, 0x00u,
        0x00u, 0x82u, 0x20u, 0x00u, 0x10u, 0x82u, 0x21u, 0x00u, 0x20u, 0x82u, 0x22u, 0x00u, 0x30u, 0x82u, 0x23u, 0x00u,
        0x40u, 0x82u, 0x24u, 0x00u, 0x50u, 0x82u, 0x25u, 0x00u, 0x60u, 0x82u, 0x26u, 0x00u, 0x70u, 0x82u, 0x27u, 0x00u,
        0x80u, 0x82u, 0x28u, 0x00u, 0x90u, 0x82u, 0x29u, 0x00u, 0xa0u, 0x82u, 0x2au, 0x00u, 0xb0u, 0x82u, 0x2bu, 0x00u,
        0xc0u, 0x82u, 0x2cu, 0x00u, 0xd0u, 0x82u, 0x2du, 0x00u, 0xe0u, 0x82u, 0x2eu, 0x00u, 0xf0u, 0x82u, 0x2fu, 0x00u,
        0x00u, 0x83u, 0x30u, 0x00u, 0x10u, 0x83u, 0x31u, 0x00u, 0x20u, 0x83u, 0x32u, 0x00u, 0x30u, 0x83u, 0x33u, 0x00u,
        0x40u, 0x83u, 0x34u, 0x00u, 0x50u, 0x83u, 0x35u, 0x00u, 0x60u, 0x83u, 0x36u, 0x00u, 0x70u, 0x83u, 0x37u, 0x00u,
        0x80u, 0x83u, 0x38u, 0x00u, 0x90u, 0x83u, 0x39u, 0x00u, 0xa0u, 0x83u, 0x3au, 0x00u, 0xb0u, 0x83u, 0x3bu, 0x00u,
        0xc0u, 0x83u, 0x3cu, 0x00u, 0xd0u, 0x83u, 0x3du, 0x00u, 0xe0u, 0x83u, 0x3eu, 0x00u, 0xf0u, 0x83u, 0x3fu, 0x00u,
        0x00u, 0x04u, 0x41u, 0x00u, 0x20u, 0x04u, 0x43u, 0x00u, 0x40u, 0x04u, 0x45u, 0x00u, 0x60u, 0x04u, 0x47u, 0x00u,
        0x80u, 0x04u, 0x49u, 0x00u, 0xa0u, 0x04u, 0x4bu, 0x00u, 0xc0u, 0x04u, 0x4du, 0x00u, 0xe0u, 0x04u, 0x4fu, 0x00u,
        0x00u, 0x05u, 0x51u, 0x00u, 0x20u, 0x05u, 0x53u, 0x00u, 0x40u, 0x05u, 0x55u, 0x00u, 0x60u, 0x05u, 0x57u, 0x00u,
        0x80u, 0x05u, 0x59u, 0x00u, 0xa0u, 0x05u, 0x5bu, 0x00u, 0xc0u, 0x05u, 0x5du, 0x00u, 0xe0u, 0x05u, 0x5fu, 0x00u,
        0x00u, 0x06u, 0x61u, 0x00u, 0x20u, 0x06u, 0x63u, 0x00u, 0x40u, 0x06u, 0x65u, 0x00u, 0x60u, 0x06u, 0x67u, 0x00u,
        0x80u, 0x06u, 0x69u, 0x00u, 0xa0u, 0x06u, 0x6bu, 0x00u, 0xc0u, 0x06u, 0x6du, 0x00u, 0xe0u, 0x06u, 0x6fu, 0x00u,
        0x00u, 0x07u, 0x71u, 0x00u, 0x20u, 0x07u, 0x73u, 0x00u, 0x40u, 0x07u, 0x75u, 0x00u, 0x60u, 0x07u, 0x77u, 0x00u,
        0x80u, 0x07u, 0x79u, 0x00u, 0xa0u, 0x07u, 0x7bu, 0x00u, 0xc0u, 0x07u, 0x7du, 0x00u, 0xe0u, 0x07u, 0x7fu, 0x00u,
        0x00u, 0xf8u, 0x81u, 0x00u, 0x3fu, 0xf8u, 0x85u, 0x00u, 0x7fu, 0xf8u, 0x89u, 0x00u, 0xbfu, 0xf8u, 0x8du, 0x00u,
        0xffu, 0xf8u, 0x91u, 0x00u, 0x3fu, 0xf9u, 0x95u, 0x00u, 0x7fu, 0xf9u, 0x99u, 0x00u, 0xbfu, 0xf9u, 0x9du, 0x00u,
        0xffu, 0xf9u, 0xa1u, 0x00u, 0x3fu, 0xfau, 0xa5u, 0x00u, 0x7fu, 0xfau, 0xa9u, 0x00u, 0xbfu, 0xfau, 0xadu, 0x00u,
        0xffu, 0xfau, 0xb1u, 0x00u, 0x3fu, 0xfbu, 0xb5u, 0x00u, 0x7fu, 0xfbu, 0xb9u, 0x00u, 0xbfu, 0xfbu, 0xbdu, 0x00u,
        0xffu, 0xfbu, 0xc1u, 0x00u, 0x3fu, 0xfcu, 0xc5u, 0x00u, 0x7fu, 0xfcu, 0xc9u, 0x00u, 0xbfu, 0xfcu, 0xcdu, 0x00u,
        0xffu, 0xfcu, 0xd1u, 0x00u, 0x3fu, 0xfdu, 0xd5u, 0x00u, 0x7fu, 0xfdu, 0xd9u, 0x00u, 0xbfu, 0xfdu, 0xddu, 0x00u,
        0xffu, 0xfdu, 0xe1u, 0x00u, 0x3fu, 0xfeu, 0xe5u, 0x00u, 0x7fu, 0xfeu, 0xe9u, 0x00u, 0xbfu, 0xfeu, 0xedu, 0x00u,
        0xffu, 0xfeu, 0xf1u, 0x00u, 0x3fu, 0xffu, 0xf5u, 0x00u, 0x7fu, 0xffu, 0xf9u, 0x00u, 0xbfu, 0xffu, 0xfdu, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x10u, 0x00u, 0x00u, 0x02u, 0x30u, 0x00u, 0x00u, 0x04u, 0x50u, 0x00u, 0x00u, 0x06u, 0x70u, 0x00u, 0x00u,
        0x08u, 0x90u, 0x00u, 0x00u, 0x0au, 0xb0u, 0x00u, 0x00u, 0x0cu, 0xd0u, 0x00u, 0x00u, 0x0eu, 0xf0u, 0x00u, 0x00u,
        0x10u, 0x10u, 0x01u, 0x00u, 0x12u, 0x30u, 0x01u, 0x00u, 0x14u, 0x50u, 0x01u, 0x00u, 0x16u, 0x70u, 0x01u, 0x00u,
        0x18u, 0x90u, 0x01u, 0x00u, 0x1au, 0xb0u, 0x01u, 0x00u, 0x1cu, 0xd0u, 0x01u, 0x00u, 0x1eu, 0xf0u, 0x01u, 0x00u,
        0x20u, 0x10u, 0x02u, 0x00u, 0x22u, 0x30u, 0x02u, 0x00u, 0x24u, 0x50u, 0x02u, 0x00u, 0x26u, 0x70u, 0x02u, 0x00u,
        0x28u, 0x90u, 0x02u, 0x00u, 0x2au, 0xb0u, 0x02u, 0x00u, 0x2cu, 0xd0u, 0x02u, 0x00u, 0x2eu, 0xf0u, 0x02u, 0x00u,
        0x30u, 0x10u, 0x03u, 0x00u, 0x32u, 0x30u, 0x03u, 0x00u, 0x34u, 0x50u, 0x03u, 0x00u, 0x36u, 0x70u, 0x03u, 0x00u,
        0x38u, 0x90u, 0x03u, 0x00u, 0x3au, 0xb0u, 0x03u, 0x00u, 0x3cu, 0xd0u, 0x03u, 0x00u, 0x3eu, 0xf0u, 0x03u, 0x00u,
        0x40u, 0x20u, 0x04u, 0x00u, 0x44u, 0x60u, 0x04u, 0x00u, 0x48u, 0xa0u, 0x04u, 0x00u, 0x4cu, 0xe0u, 0x04u, 0x00u,
        0x50u, 0x20u, 0x05u, 0x00u, 0x54u, 0x60u, 0x05u, 0x00u, 0x58u, 0xa0u, 0x05u, 0x00u, 0x5cu, 0xe0u, 0x05u, 0x00u,
        0x60u, 0x20u, 0x06u, 0x00u, 0x64u, 0x60u, 0x06u, 0x00u, 0x68u, 0xa0u, 0x06u, 0x00u, 0x6cu, 0xe0u, 0x06u, 0x00u,
        0x70u, 0x20u, 0x07u, 0x00u, 0x74u, 0x60u, 0x07u, 0x00u, 0x78u, 0xa0u, 0x07u, 0x00u, 0x7cu, 0xe0u, 0x07u, 0x00u,
        0x80u, 0x40u, 0x08u, 0x00u, 0x88u, 0xc0u, 0x08u, 0x00u, 0x90u, 0x40u, 0x09u, 0x00u, 0x98u, 0xc0u, 0x09u, 0x00u,
        0xa0u, 0x40u, 0x0au, 0x00u, 0xa8u, 0xc0u, 0x0au, 0x00u, 0xb0u, 0x40u, 0x0bu, 0x00u, 0xb8u, 0xc0u, 0x0bu, 0x00u,
        0xc0u, 0x40u, 0x0cu, 0x00u, 0xc8u, 0xc0u, 0x0cu, 0x00u, 0xd0u, 0x40u, 0x0du, 0x00u, 0xd8u, 0xc0u, 0x0du, 0x00u,
        0xe0u, 0x40u, 0x0eu, 0x00u, 0xe8u, 0xc0u, 0x0eu, 0x00u, 0xf0u, 0x40u, 0x0fu, 0x00u, 0xf8u, 0xc0u, 0x0fu, 0x00u,
        0x00u, 0x81u, 0x10u, 0x00u, 0x10u, 0x81u, 0x11u, 0x00u, 0x20u, 0x81u, 0x12u, 0x00u, 0x30u, 0x81u, 0x13u, 0x00u,
        0x40u, 0x81u, 0x14u, 0x00u, 0x50u, 0x81u, 0x15u, 0x00u, 0x60u, 0x81u, 0x16u, 0x00u, 0x70u, 0x81u, 0x17u, 0x00u,
        0x80u, 0x81u, 0x18u, 0x00u, 0x90u, 0x81u, 0x19u, 0x00u, 0xa0u, 0x81u, 0x1au, 0x00u, 0xb0u, 0x81u, 0x1bu, 0x00u,
        0xc0u, 0x81u, 0x1cu, 0x00u, 0xd0u, 0x81u, 0x1du, 0x00u, 0xe0u, 0x81u, 0x1eu, 0x00u, 0xf0u, 0x81u, 0x1fu, 0x00u,
        0x00u, 0x82u, 0x20u, 0x00u, 0x10u, 0x82u, 0x21u, 0x00u, 0x20u, 0x82u, 0x22u, 0x00u, 0x30u, 0x82u, 0x23u, 0x00u,
        0x40u, 0x82u, 0x24u, 0x00u, 0x50u, 0x82u, 0x25u, 0x00u, 0x60u, 0x82u, 0x26u, 0x00u, 0x70u, 0x82u, 0x27u, 0x00u,
        0x80u, 0x82u, 0x28u, 0x00u, 0x90u, 0x82u, 0x29u, 0x00u, 0xa0u, 0x82u, 0x2au, 0x00u, 0xb0u, 0x82u, 0x2bu, 0x00u,
        0xc0u, 0x82u, 0x2cu, 0x00u, 0xd0u, 0x82u, 0x2du, 0x00u, 0xe0u, 0x82u, 0x2eu, 0x00u, 0xf0u, 0x82u, 0x2fu, 0x00u,
        0x00u, 0x83u, 0x30u, 0x00u, 0x10u, 0x83u, 0x31u, 0x00u, 0x20u, 0x83u, 0x32u, 0x00u, 0x30u, 0x83u, 0x33u, 0x00u,
        0x40u, 0x83u, 0x34u, 0x00u, 0x50u, 0x83u, 0x35u, 0x00u, 0x60u, 0x83u, 0x36u, 0x00u, 0x70u, 0x83u, 0x37u, 0x00u,
        0x80u, 0x83u, 0x38u, 0x00u, 0x90u, 0x83u, 0x39u, 0x00u, 0xa0u, 0x83u, 0x3au, 0x00u, 0xb0u, 0x83u, 0x3bu, 0x00u,
        0xc0u, 0x83u, 0x3cu, 0x00u, 0xd0u, 0x83u, 0x3du, 0x00u, 0xe0u, 0x83u, 0x3eu, 0x00u, 0xf0u, 0x83u, 0x3fu, 0x00u,
        0x00u, 0x04u, 0x41u, 0x00u, 0x20u, 0x04u, 0x43u, 0x00u, 0x40u, 0x04u, 0x45u, 0x00u, 0x60u, 0x04u, 0x47u, 0x00u,
        0x80u, 0x04u, 0x49u, 0x00u, 0xa0u, 0x04u, 0x4bu, 0x00u, 0xc0u, 0x04u, 0x4du, 0x00u, 0xe0u, 0x04u, 0x4fu, 0x00u,
        0x00u, 0x05u, 0x51u, 0x00u, 0x20u, 0x05u, 0x53u, 0x00u, 0x40u, 0x05u, 0x55u, 0x00u, 0x60u, 0x05u, 0x57u, 0x00u,
        0x80u, 0x05u, 0x59u, 0x00u, 0xa0u, 0x05u, 0x5bu, 0x00u, 0xc0u, 0x05u, 0x5du, 0x00u, 0xe0u, 0x05u, 0x5fu, 0x00u,
        0x00u, 0x06u, 0x61u, 0x00u, 0x20u, 0x06u, 0x63u, 0x00u, 0x40u, 0x06u, 0x65u, 0x00u, 0x60u, 0x06u, 0x67u, 0x00u,
        0x80u, 0x06u, 0x69u, 0x00u, 0xa0u, 0x06u, 0x6bu, 0x00u, 0xc0u, 0x06u, 0x6du, 0x00u, 0xe0u, 0x06u, 0x6fu, 0x00u,
        0x00u, 0x07u, 0x71u, 0x00u, 0x20u, 0x07u, 0x73u, 0x00u, 0x40u, 0x07u, 0x75u, 0x00u, 0x60u, 0x07u, 0x77u, 0x00u,
        0x80u, 0x07u, 0x79u, 0x00u, 0xa0u, 0x07u, 0x7bu, 0x00u, 0xc0u, 0x07u, 0x7du, 0x00u, 0xe0u, 0x07u, 0x7fu, 0x00u,
        0x00u, 0xf8u, 0x81u, 0x00u, 0x3fu, 0xf8u, 0x85u, 0x00u, 0x7fu, 0xf8u, 0x89u, 0x00u, 0xbfu, 0xf8u, 0x8du, 0x00u,
        0xffu, 0xf8u, 0x91u, 0x00u, 0x3fu, 0xf9u, 0x95u, 0x00u, 0x7fu, 0xf9u, 0x99u, 0x00u, 0xbfu, 0xf9u, 0x9du, 0x00u,
        0xffu, 0xf9u, 0xa1u, 0x00u, 0x3fu, 0xfau, 0xa5u, 0x00u, 0x7fu, 0xfau, 0xa9u, 0x00u, 0xbfu, 0xfau, 0xadu, 0x00u,
        0xffu, 0xfau, 0xb1u, 0x00u, 0x3fu, 0xfbu, 0xb5u, 0x00u, 0x7fu, 0xfbu, 0xb9u, 0x00u, 0xbfu, 0xfbu, 0xbdu, 0x00u,
        0xffu, 0xfbu, 0xc1u, 0x00u, 0x3fu, 0xfcu, 0xc5u, 0x00u, 0x7fu, 0xfcu, 0xc9u, 0x00u, 0xbfu, 0xfcu, 0xcdu, 0x00u,
        0xffu, 0xfcu, 0xd1u, 0x00u, 0x3fu, 0xfdu, 0xd5u, 0x00u, 0x7fu, 0xfdu, 0xd9u, 0x00u, 0xbfu, 0xfdu, 0xddu, 0x00u,
        0xffu, 0xfdu, 0xe1u, 0x00u, 0x3fu, 0xfeu, 0xe5u, 0x00u, 0x7fu, 0xfeu, 0xe9u, 0x00u, 0xbfu, 0xfeu, 0xedu, 0x00u,
        0xffu, 0xfeu, 0xf1u, 0x00u, 0x3fu, 0xffu, 0xf5u, 0x00u, 0x7fu, 0xffu, 0xf9u, 0x00u, 0xbfu, 0xffu, 0xfdu, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x10u, 0x00u, 0x00u, 0x02u, 0x30u, 0x00u, 0x00u, 0x04u, 0x50u, 0x00u, 0x00u, 0x06u, 0x70u, 0x00u, 0x00u,
        0x08u, 0x90u, 0x00u, 0x00u, 0x0au, 0xb0u, 0x00u, 0x00u, 0x0cu, 0xd0u, 0x00u, 0x00u, 0x0eu, 0xf0u, 0x00u, 0x00u,
        0x10u, 0x10u, 0x01u, 0x00u, 0x12u, 0x30u, 0x01u, 0x00u, 0x14u, 0x50u, 0x01u, 0x00u, 0x16u, 0x70u, 0x01u, 0x00u,
        0x18u, 0x90u, 0x01u, 0x00u, 0x1au, 0xb0u, 0x01u, 0x00u, 0x1cu, 0xd0u, 0x01u, 0x00u, 0x1eu, 0xf0u, 0x01u, 0x00u,
        0x20u, 0x10u, 0x02u, 0x00u, 0x22u, 0x30u, 0x02u, 0x00u, 0x24u, 0x50u, 0x02u, 0x00u, 0x26u, 0x70u, 0x02u, 0x00u,
        0x28u, 0x90u, 0x02u, 0x00u, 0x2au, 0xb0u, 0x02u, 0x00u, 0x2cu, 0xd0u, 0x02u, 0x00u, 0x2eu, 0xf0u, 0x02u, 0x00u,
        0x30u, 0x10u, 0x03u, 0x00u, 0x32u, 0x30u, 0x03u, 0x00u, 0x34u, 0x50u, 0x03u, 0x00u, 0x36u, 0x70u, 0x03u, 0x00u,
        0x38u, 0x90u, 0x03u, 0x00u, 0x3au, 0xb0u, 0x03u, 0x00u, 0x3cu, 0xd0u, 0x03u, 0x00u, 0x3eu, 0xf0u, 0x03u, 0x00u,
        0x40u, 0x20u, 0x04u, 0x00u, 0x44u, 0x60u, 0x04u, 0x00u, 0x48u, 0xa0u, 0x04u, 0x00u, 0x4cu, 0xe0u, 0x04u, 0x00u,
        0x50u, 0x20u, 0x05u, 0x00u, 0x54u, 0x60u, 0x05u, 0x00u, 0x58u, 0xa0u, 0x05u, 0x00u, 0x5cu, 0xe0u, 0x05u, 0x00u,
        0x60u, 0x20u, 0x06u, 0x00u, 0x64u, 0x60u, 0x06u, 0x00u, 0x68u, 0xa0u, 0x06u, 0x00u, 0x6cu, 0xe0u, 0x06u, 0x00u,
        0x70u, 0x20u, 0x07u, 0x00u, 0x74u, 0x60u, 0x07u, 0x00u, 0x78u, 0xa0u, 0x07u, 0x00u, 0x7cu, 0xe0u, 0x07u, 0x00u,
        0x80u, 0x40u, 0x08u, 0x00u, 0x88u, 0xc0u, 0x08u, 0x00u, 0x90u, 0x40u, 0x09u, 0x00u, 0x98u, 0xc0u, 0x09u, 0x00u,
        0xa0u, 0x40u, 0x0au, 0x00u, 0xa8u, 0xc0u, 0x0au, 0x00u, 0xb0u, 0x40u, 0x0bu, 0x00u, 0xb8u, 0xc0u, 0x0bu, 0x00u,
        0xc0u, 0x40u, 0x0cu, 0x00u, 0xc8u, 0xc0u, 0x0cu, 0x00u, 0xd0u, 0x40u, 0x0du, 0x00u, 0xd8u, 0xc0u, 0x0du, 0x00u,
        0xe0u, 0x40u, 0x0eu, 0x00u, 0xe8u, 0xc0u, 0x0eu, 0x00u, 0xf0u, 0x40u, 0x0fu, 0x00u, 0xf8u, 0xc0u, 0x0fu, 0x00u,
        0x00u, 0x81u, 0x10u, 0x00u, 0x10u, 0x81u, 0x11u, 0x00u, 0x20u, 0x81u, 0x12u, 0x00u, 0x30u, 0x81u, 0x13u, 0x00u,
        0x40u, 0x81u, 0x14u, 0x00u, 0x50u, 0x81u, 0x15u, 0x00u, 0x60u, 0x81u, 0x16u, 0x00u, 0x70u, 0x81u, 0x17u, 0x00u,
        0x80u, 0x81u, 0x18u, 0x00u, 0x90u, 0x81u, 0x19u, 0x00u, 0xa0u, 0x81u, 0x1au, 0x00u, 0xb0u, 0x81u, 0x1bu, 0x00u,
        0xc0u, 0x81u, 0x1cu, 0x00u, 0xd0u, 0x81u, 0x1du, 0x00u, 0xe0u, 0x81u, 0x1eu, 0x00u, 0xf0u, 0x81u, 0x1fu, 0x00u,
        0x00u, 0x82u, 0x20u, 0x00u, 0x10u, 0x82u, 0x21u, 0x00u, 0x20u, 0x82u, 0x22u, 0x00u, 0x30u, 0x82u, 0x23u, 0x00u,
        0x40u, 0x82u, 0x24u, 0x00u, 0x50u, 0x82u, 0x25u, 0x00u, 0x60u, 0x82u, 0x26u, 0x00u, 0x70u, 0x82u, 0x27u, 0x00u,
        0x80u, 0x82u, 0x28u, 0x00u, 0x90u, 0x82u, 0x29u, 0x00u, 0xa0u, 0x82u, 0x2au, 0x00u, 0xb0u, 0x82u, 0x2bu, 0x00u,
        0xc0u, 0x82u, 0x2cu, 0x00u, 0xd0u, 0x82u, 0x2du, 0x00u, 0xe0u, 0x82u, 0x2eu, 0x00u, 0xf0u, 0x82u, 0x2fu, 0x00u,
        0x00u, 0x83u, 0x30u, 0x00u, 0x10u, 0x83u, 0x31u, 0x00u, 0x20u, 0x83u, 0x32u, 0x00u, 0x30u, 0x83u, 0x33u, 0x00u,
        0x40u, 0x83u, 0x34u, 0x00u, 0x50u, 0x83u, 0x35u, 0x00u, 0x60u, 0x83u, 0x36u, 0x00u, 0x70u, 0x83u, 0x37u, 0x00u,
        0x80u, 0x83u, 0x38u, 0x00u, 0x90u, 0x83u, 0x39u, 0x00u, 0xa0u, 0x83u, 0x3au, 0x00u, 0xb0u, 0x83u, 0x3bu, 0x00u,
        0xc0u, 0x83u, 0x3cu, 0x00u, 0xd0u, 0x83u, 0x3du, 0x00u, 0xe0u, 0x83u, 0x3eu, 0x00u, 0xf0u, 0x83u, 0x3fu, 0x00u,
        0x00u, 0x04u, 0x41u, 0x00u, 0x20u, 0x04u, 0x43u, 0x00u, 0x40u, 0x04u, 0x45u, 0x00u, 0x60u, 0x04u, 0x47u, 0x00u,
        0x80u, 0x04u, 0x49u, 0x00u, 0xa0u, 0x04u, 0x4bu, 0x00u, 0xc0u, 0x04u, 0x4du, 0x00u, 0xe0u, 0x04u, 0x4fu, 0x00u,
        0x00u, 0x05u, 0x51u, 0x00u, 0x20u, 0x05u, 0x53u, 0x00u, 0x40u, 0x05u, 0x55u, 0x00u, 0x60u, 0x05u, 0x57u, 0x00u,
        0x80u, 0x05u, 0x59u, 0x00u, 0xa0u, 0x05u, 0x5bu, 0x00u, 0xc0u, 0x05u, 0x5du, 0x00u, 0xe0u, 0x05u, 0x5fu, 0x00u,
        0x00u, 0x06u, 0x61u, 0x00u, 0x20u, 0x06u, 0x63u, 0x00u, 0x40u, 0x06u, 0x65u, 0x00u, 0x60u, 0x06u, 0x67u, 0x00u,
        0x80u, 0x06u, 0x69u, 0x00u, 0xa0u, 0x06u, 0x6bu, 0x00u, 0xc0u, 0x06u, 0x6du, 0x00u, 0xe0u, 0x06u, 0x6fu, 0x00u,
        0x00u, 0x07u, 0x71u, 0x00u, 0x20u, 0x07u, 0x73u, 0x00u, 0x40u, 0x07u, 0x75u, 0x00u, 0x60u, 0x07u, 0x77u, 0x00u,
        0x80u, 0x07u, 0x79u, 0x00u, 0xa0u, 0x07u, 0x7bu, 0x00u, 0xc0u, 0x07u, 0x7du, 0x00u, 0xe0u, 0x07u, 0x7fu, 0x00u,
        0x00u, 0xf8u, 0x81u, 0x00u, 0x3fu, 0xf8u, 0x85u, 0x00u, 0x7fu, 0xf8u, 0x89u, 0x00u, 0xbfu, 0xf8u, 0x8du, 0x00u,
        0xffu, 0xf8u, 0x91u, 0x00u, 0x3fu, 0xf9u, 0x95u, 0x00u, 0x7fu, 0xf9u, 0x99u, 0x00u, 0xbfu, 0xf9u, 0x9du, 0x00u,
        0xffu, 0xf9u, 0xa1u, 0x00u, 0x3fu, 0xfau, 0xa5u, 0x00u, 0x7fu, 0xfau, 0xa9u, 0x00u, 0xbfu, 0xfau, 0xadu, 0x00u,
        0xffu, 0xfau, 0xb1u, 0x00u, 0x3fu, 0xfbu, 0xb5u, 0x00u, 0x7fu, 0xfbu, 0xb9u, 0x00u, 0xbfu, 0xfbu, 0xbdu, 0x00u,
        0xffu, 0xfbu, 0xc1u, 0x00u, 0x3fu, 0xfcu, 0xc5u, 0x00u, 0x7fu, 0xfcu, 0xc9u, 0x00u, 0xbfu, 0xfcu, 0xcdu, 0x00u,
        0xffu, 0xfcu, 0xd1u, 0x00u, 0x3fu, 0xfdu, 0xd5u, 0x00u, 0x7fu, 0xfdu, 0xd9u, 0x00u, 0xbfu, 0xfdu, 0xddu, 0x00u,
        0xffu, 0xfdu, 0xe1u, 0x00u, 0x3fu, 0xfeu, 0xe5u, 0x00u, 0x7fu, 0xfeu, 0xe9u, 0x00u, 0xbfu, 0xfeu, 0xedu, 0x00u,
        0xffu, 0xfeu, 0xf1u, 0x00u, 0x3fu, 0xffu, 0xf5u, 0x00u, 0x7fu, 0xffu, 0xf9u, 0x00u, 0xbfu, 0xffu, 0xfdu, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u
    };

    static const uint8 step1_mcts_Edge_Detect[] = {
        0x38u, 0x04u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x03u, 0x00u, 0x00u, 0x00u,
        0xffu, 0xffu, 0xffu, 0xffu, 0x00u, 0xffu, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x80u, 0x00u,
        0x00u, 0x40u, 0x00u, 0x00u, 0x00u, 0x08u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x80u, 0x80u, 0x00u, 0x00u, 0xeau, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x0au, 0x00u, 0x00u, 0x00u, 0x0cu, 0x00u, 0x00u, 0x00u, 0x01u, 0x00u, 0x00u, 0x00u, 0x14u, 0x00u, 0x00u, 0x00u,
        0x16u, 0x00u, 0x00u, 0x00u, 0x01u, 0x00u, 0x00u, 0x00u, 0x30u, 0x00u, 0x00u, 0x00u, 0x30u, 0x00u, 0x00u, 0x00u,
        0x30u, 0x00u, 0x00u, 0x00u, 0x06u, 0x00u, 0x00u, 0x00u, 0x0au, 0x00u, 0x00u, 0x00u, 0x0cu, 0x00u, 0x00u, 0x00u,
        0x01u, 0x00u, 0x00u, 0x00u, 0x14u, 0x00u, 0x00u, 0x00u, 0x16u, 0x00u, 0x00u, 0x00u, 0x01u, 0x00u, 0x00u, 0x00u,
        0x20u, 0x00u, 0x00u, 0x00u, 0x20u, 0x00u, 0x00u, 0x00u, 0x20u, 0x00u, 0x00u, 0x00u, 0x06u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x10u, 0x00u, 0x02u, 0xffu, 0xefu, 0xffu, 0x01u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x24u, 0x49u, 0x92u, 0x04u, 0x00u, 0x00u, 0x00u, 0x00u, 0x1cu, 0x1cu, 0x1cu, 0x1cu,
        0x1cu, 0x1cu, 0x1cu, 0x1cu, 0x3cu, 0x3cu, 0x3cu, 0x3cu, 0x3cu, 0x1cu, 0x1cu, 0x3cu, 0x7cu, 0xfcu, 0x7cu, 0x3cu,
        0x1cu, 0x1cu, 0x3cu, 0xfcu, 0xc0u, 0x00u, 0x00u, 0x00u, 0x1cu, 0x1cu, 0x1cu, 0x1cu, 0x1cu, 0x1cu, 0x1cu, 0x1cu,
        0x3cu, 0x3cu, 0x3cu, 0x3cu, 0x3cu, 0x1cu, 0x1cu, 0x3cu, 0x7cu, 0xfcu, 0x7cu, 0x3cu, 0x1cu, 0x1cu, 0x3cu, 0xfcu,
        0xc0u, 0x00u, 0x00u, 0x00u, 0x1cu, 0x1cu, 0x1cu, 0x1cu, 0x1cu, 0x1cu, 0x1cu, 0x1cu, 0x3cu, 0x3cu, 0x3cu, 0x3cu,
        0x3cu, 0x1cu, 0x1cu, 0x3cu, 0x7cu, 0xfcu, 0x7cu, 0x3cu, 0x1cu, 0x1cu, 0x3cu, 0xfcu, 0xc0u, 0x00u, 0x00u, 0x00u,
        0x1cu, 0x1cu, 0x1cu, 0x1cu, 0x1cu, 0x1cu, 0x1cu, 0x1cu, 0x3cu, 0x3cu, 0x3cu, 0x3cu, 0x3cu, 0x1cu, 0x1cu, 0x3cu,
        0x7cu, 0xfcu, 0x7cu, 0x3cu, 0x1cu, 0x1cu, 0x3cu, 0xfcu, 0xc0u, 0x00u, 0x00u, 0x00u, 0x1cu, 0x1cu, 0x1cu, 0x1cu,
        0x1cu, 0x1cu, 0x1cu, 0x1cu, 0x3cu, 0x3cu, 0x3cu, 0x3cu, 0x3cu, 0x1cu, 0x1cu, 0x3cu, 0x7cu, 0xfcu, 0x7cu, 0x3cu,
        0x1cu, 0x1cu, 0x3cu, 0xfcu, 0xc0u, 0x00u, 0x00u, 0x00u, 0x1cu, 0x1cu, 0x1cu, 0x1cu, 0x1cu, 0x1cu, 0x1cu, 0x1cu,
        0x3cu, 0x3cu, 0x3cu, 0x3cu, 0x3cu, 0x1cu, 0x1cu, 0x3cu, 0x7cu, 0xfcu, 0x7cu, 0x3cu, 0x1cu, 0x1cu, 0x3cu, 0xfcu,
        0xc0u, 0x00u, 0x00u, 0x00u, 0x1cu, 0x1cu, 0x1cu, 0x1cu, 0x1cu, 0x1cu, 0x1cu, 0x1cu, 0x3cu, 0x3cu, 0x3cu, 0x3cu,
        0x3cu, 0x1cu, 0x1cu, 0x3cu, 0x7cu, 0xfcu, 0x7cu, 0x3cu, 0x1cu, 0x1cu, 0x3cu, 0xfcu, 0xc0u, 0x00u, 0x00u, 0x00u,
        0x1cu, 0x1cu, 0x1cu, 0x1cu, 0x1cu, 0x1cu, 0x1cu, 0x1cu, 0x3cu, 0x3cu, 0x3cu, 0x3cu, 0x3cu, 0x1cu, 0x1cu, 0x3cu,
        0x7cu, 0xfcu, 0x7cu, 0x3cu, 0x1cu, 0x1cu, 0x3cu, 0xfcu, 0xc0u, 0x00u, 0x00u, 0x00u, 0x1cu, 0x1cu, 0x1cu, 0x1cu,
        0x1cu, 0x1cu, 0x1cu, 0x1cu, 0x3cu, 0x3cu, 0x3cu, 0x3cu, 0x3cu, 0x1cu, 0x1cu, 0x3cu, 0x7cu, 0xfcu, 0x7cu, 0x3cu,
        0x1cu, 0x1cu, 0x3cu, 0xfcu, 0xc0u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x10u, 0x00u, 0x02u, 0xffu, 0xefu, 0xffu, 0x01u, 0x0au, 0x00u, 0x00u, 0x00u,
        0x0cu, 0x00u, 0x00u, 0x00u, 0x01u, 0x00u, 0x00u, 0x00u, 0x14u, 0x00u, 0x00u, 0x00u, 0x16u, 0x00u, 0x00u, 0x00u,
        0x01u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x06u, 0x00u, 0x00u, 0x00u, 0x0au, 0x00u, 0x00u, 0x00u, 0x0cu, 0x00u, 0x00u, 0x00u, 0x01u, 0x00u, 0x00u, 0x00u,
        0x14u, 0x00u, 0x00u, 0x00u, 0x16u, 0x00u, 0x00u, 0x00u, 0x01u, 0x00u, 0x00u, 0x00u, 0x10u, 0x00u, 0x00u, 0x00u,
        0x10u, 0x00u, 0x00u, 0x00u, 0x10u, 0x00u, 0x00u, 0x00u, 0x06u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x42u, 0x08u, 0x01u, 0x00u, 0x42u, 0x08u, 0x01u, 0x00u, 0x42u, 0x08u, 0x01u, 0x00u, 0x42u, 0x08u, 0x01u, 0x00u,
        0x42u, 0x08u, 0x01u, 0x00u, 0x42u, 0x08u, 0x01u, 0x00u, 0x42u, 0x08u, 0x01u, 0x00u, 0x42u, 0x08u, 0x01u, 0x00u,
        0x42u, 0x08u, 0x01u, 0x00u, 0x42u, 0x08u, 0x01u, 0x00u, 0x42u, 0x08u, 0x01u, 0x00u, 0x42u, 0x08u, 0x01u, 0x00u,
        0x42u, 0x08u, 0x01u, 0x00u, 0x42u, 0x08u, 0x01u, 0x00u, 0x42u, 0x08u, 0x01u, 0x00u, 0x42u, 0x08u, 0x01u, 0x00u,
        0x42u, 0x08u, 0x01u, 0x00u, 0x42u, 0x08u, 0x01u, 0x00u, 0x42u, 0x08u, 0x01u, 0x00u, 0x42u, 0x08u, 0x01u, 0x00u,
        0x42u, 0x08u, 0x01u, 0x00u, 0x42u, 0x08u, 0x01u, 0x00u, 0x42u, 0x08u, 0x01u, 0x00u, 0x42u, 0x08u, 0x01u, 0x00u,
        0x42u, 0x08u, 0x01u, 0x00u, 0x42u, 0x08u, 0x01u, 0x00u, 0x42u, 0x08u, 0x01u, 0x00u, 0x42u, 0x08u, 0x01u, 0x00u,
        0x42u, 0x08u, 0x01u, 0x00u, 0x42u, 0x08u, 0x01u, 0x00u, 0x42u, 0x08u, 0x01u, 0x00u, 0x42u, 0x08u, 0x01u, 0x00u,
        0x42u, 0x08u, 0x01u, 0x00u, 0x42u, 0x08u, 0x01u, 0x00u, 0x42u, 0x08u, 0x01u, 0x00u, 0x42u, 0x08u, 0x01u, 0x00u,
        0x42u, 0x08u, 0x01u, 0x00u, 0x42u, 0x08u, 0x01u, 0x00u, 0x42u, 0x08u, 0x01u, 0x00u, 0x42u, 0x08u, 0x01u, 0x00u,
        0x42u, 0x08u, 0x01u, 0x00u, 0x42u, 0x08u, 0x01u, 0x00u, 0x42u, 0x08u, 0x01u, 0x00u, 0x42u, 0x08u, 0x01u, 0x00u,
        0x42u, 0x08u, 0x01u, 0x00u, 0x42u, 0x08u, 0x01u, 0x00u, 0x42u, 0x08u, 0x01u, 0x00u, 0x42u, 0x08u, 0x01u, 0x00u,
        0x42u, 0x08u, 0x01u, 0x00u, 0x42u, 0x08u, 0x01u, 0x00u, 0x42u, 0x08u, 0x01u, 0x00u, 0x42u, 0x08u, 0x01u, 0x00u,
        0x42u, 0x08u, 0x01u, 0x00u, 0x42u, 0x08u, 0x01u, 0x00u, 0x42u, 0x08u, 0x01u, 0x00u, 0x42u, 0x08u, 0x01u, 0x00u,
        0x42u, 0x08u, 0x01u, 0x00u, 0x42u, 0x08u, 0x01u, 0x00u, 0x42u, 0x08u, 0x01u, 0x00u, 0x42u, 0x08u, 0x01u, 0x00u,
        0x42u, 0x08u, 0x01u, 0x00u, 0x42u, 0x08u, 0x01u, 0x00u, 0x42u, 0x08u, 0x01u, 0x00u, 0x42u, 0x08u, 0x01u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x01u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x0au, 0x00u, 0x00u, 0x00u,
        0x0bu, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x80u, 0x00u, 0x00u, 0x00u, 0x80u, 0x00u, 0x00u, 0x00u,
        0x80u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x0au, 0x00u, 0x00u, 0x00u, 0x0cu, 0x00u, 0x00u, 0x00u,
        0x01u, 0x00u, 0x00u, 0x00u, 0x14u, 0x00u, 0x00u, 0x00u, 0x16u, 0x00u, 0x00u, 0x00u, 0x01u, 0x00u, 0x00u, 0x00u,
        0xffu, 0x00u, 0x00u, 0x00u, 0xffu, 0x00u, 0x00u, 0x00u, 0xffu, 0x00u, 0x00u, 0x00u, 0x06u, 0x00u, 0x00u, 0x00u,
        0x0au, 0x00u, 0x00u, 0x00u, 0x0cu, 0x00u, 0x00u, 0x00u, 0x01u, 0x00u, 0x00u, 0x00u, 0x14u, 0x00u, 0x00u, 0x00u,
        0x16u, 0x00u, 0x00u, 0x00u, 0x01u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x06u, 0x00u, 0x00u, 0x00u, 0x0au, 0x00u, 0x00u, 0x00u, 0x0cu, 0x00u, 0x00u, 0x00u,
        0x01u, 0x00u, 0x00u, 0x00u, 0x14u, 0x00u, 0x00u, 0x00u, 0x16u, 0x00u, 0x00u, 0x00u, 0x01u, 0x00u, 0x00u, 0x00u,
        0x10u, 0x00u, 0x00u, 0x00u, 0x10u, 0x00u, 0x00u, 0x00u, 0x10u, 0x00u, 0x00u, 0x00u, 0x06u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x10u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u
    };

    static const uint8 step1_mcts_POS_DEPEND_33x33[] = {
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u
    };

    static const uint8 step2_mcts_Edge_Detect[] = {
        0x38u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x0fu, 0x00u, 0x00u, 0x13u, 0x00u, 0x00u, 0x00u,
        0xffu, 0xffu, 0xffu, 0xffu, 0x00u, 0xffu, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x80u, 0x50u,
        0x00u, 0x40u, 0x00u, 0x00u, 0x00u, 0x08u, 0x00u, 0x00u, 0xa0u, 0x00u, 0x0au, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x80u, 0x80u, 0xcdu, 0x00u, 0xeau, 0x3cu, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x0au, 0x00u, 0x00u, 0x00u, 0x0cu, 0x00u, 0x00u, 0x00u, 0x01u, 0x00u, 0x00u, 0x00u, 0x14u, 0x00u, 0x00u, 0x00u,
        0x16u, 0x00u, 0x00u, 0x00u, 0x01u, 0x00u, 0x00u, 0x00u, 0x30u, 0x00u, 0x00u, 0x00u, 0x30u, 0x00u, 0x00u, 0x00u,
        0x30u, 0x00u, 0x00u, 0x00u, 0x06u, 0x00u, 0x00u, 0x00u, 0x0au, 0x00u, 0x00u, 0x00u, 0x0cu, 0x00u, 0x00u, 0x00u,
        0x01u, 0x00u, 0x00u, 0x00u, 0x14u, 0x00u, 0x00u, 0x00u, 0x16u, 0x00u, 0x00u, 0x00u, 0x01u, 0x00u, 0x00u, 0x00u,
        0x20u, 0x00u, 0x00u, 0x00u, 0x20u, 0x00u, 0x00u, 0x00u, 0x20u, 0x00u, 0x00u, 0x00u, 0x06u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x10u, 0x00u, 0x02u, 0xffu, 0xefu, 0xffu, 0x01u, 0x24u, 0x49u, 0x92u, 0x04u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x24u, 0x49u, 0x92u, 0x04u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x36u, 0x36u, 0x36u, 0x36u, 0x36u, 0x00u, 0x00u, 0x36u, 0x76u, 0xf6u, 0x76u, 0x36u,
        0x00u, 0x00u, 0x36u, 0xf6u, 0xf0u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x36u, 0x36u, 0x36u, 0x36u, 0x36u, 0x00u, 0x00u, 0x36u, 0x76u, 0xf6u, 0x76u, 0x36u, 0x00u, 0x00u, 0x36u, 0xf6u,
        0xf0u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x36u, 0x36u, 0x36u, 0x36u,
        0x36u, 0x00u, 0x00u, 0x36u, 0x76u, 0xf6u, 0x76u, 0x36u, 0x00u, 0x00u, 0x36u, 0xf6u, 0xf0u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x36u, 0x36u, 0x36u, 0x36u, 0x36u, 0x00u, 0x00u, 0x36u,
        0x76u, 0xf6u, 0x76u, 0x36u, 0x00u, 0x00u, 0x36u, 0xf6u, 0xf0u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x36u, 0x36u, 0x36u, 0x36u, 0x36u, 0x00u, 0x00u, 0x36u, 0x76u, 0xf6u, 0x76u, 0x36u,
        0x00u, 0x00u, 0x36u, 0xf6u, 0xf0u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x36u, 0x36u, 0x36u, 0x36u, 0x36u, 0x00u, 0x00u, 0x36u, 0x76u, 0xf6u, 0x76u, 0x36u, 0x00u, 0x00u, 0x36u, 0xf6u,
        0xf0u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x36u, 0x36u, 0x36u, 0x36u,
        0x36u, 0x00u, 0x00u, 0x36u, 0x76u, 0xf6u, 0x76u, 0x36u, 0x00u, 0x00u, 0x36u, 0xf6u, 0xf0u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x36u, 0x36u, 0x36u, 0x36u, 0x36u, 0x00u, 0x00u, 0x36u,
        0x76u, 0xf6u, 0x76u, 0x36u, 0x00u, 0x00u, 0x36u, 0xf6u, 0xf0u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x36u, 0x36u, 0x36u, 0x36u, 0x36u, 0x00u, 0x00u, 0x36u, 0x76u, 0xf6u, 0x76u, 0x36u,
        0x00u, 0x00u, 0x36u, 0xf6u, 0xf0u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x36u, 0x36u, 0x36u, 0x36u, 0x36u, 0x00u, 0x00u, 0x36u, 0x76u, 0xf6u, 0x76u, 0x36u, 0x00u, 0x00u, 0x36u, 0xf6u,
        0xf0u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x15u, 0x34u, 0x74u, 0xf4u, 0x74u, 0x34u, 0x15u, 0x14u, 0x34u, 0xf4u, 0xecu, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x15u, 0x14u, 0x15u, 0x00u, 0x00u, 0x00u, 0x00u, 0x34u, 0x34u, 0x34u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x74u, 0xf4u, 0x74u, 0x00u, 0x00u, 0x00u, 0x00u, 0xf4u, 0xecu, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x14u, 0x14u, 0x00u, 0x00u, 0x00u, 0x00u, 0x34u, 0x34u, 0x14u, 0x00u, 0x00u, 0x7cu, 0xf4u, 0x74u, 0x34u,
        0x00u, 0x00u, 0x00u, 0xf4u, 0xe0u, 0x00u, 0x00u, 0x00u, 0x14u, 0x14u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x14u,
        0x34u, 0x34u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x34u, 0x74u, 0xf4u, 0x7cu, 0x00u, 0x00u, 0x00u, 0x00u, 0xf4u,
        0xe0u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x15u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x34u, 0x14u, 0x00u, 0x00u, 0x7eu, 0xfcu, 0x74u, 0x34u, 0x14u, 0x15u, 0x34u, 0xf4u, 0xe0u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x15u, 0x14u, 0x14u, 0x15u, 0x00u, 0x00u, 0x00u, 0x34u, 0x34u, 0x34u, 0x00u, 0x00u, 0x00u,
        0x7eu, 0xf4u, 0x74u, 0x00u, 0x00u, 0x00u, 0x00u, 0xfcu, 0xe0u, 0x00u, 0x00u, 0x00u, 0x15u, 0x14u, 0x14u, 0x15u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x34u, 0x34u, 0x34u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x74u, 0xf4u, 0x7eu, 0x00u,
        0x00u, 0x00u, 0x00u, 0xfcu, 0xe0u, 0x00u, 0x00u, 0x00u, 0x15u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x14u,
        0x34u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x14u, 0x34u, 0x74u, 0xfcu, 0x7eu, 0x00u, 0x00u, 0x15u, 0x34u, 0xf4u,
        0xe0u, 0x00u, 0x00u, 0x00u, 0x00u, 0x10u, 0x00u, 0x02u, 0xffu, 0xefu, 0xffu, 0x01u, 0x0au, 0x00u, 0x00u, 0x00u,
        0x0cu, 0x00u, 0x00u, 0x00u, 0x01u, 0x00u, 0x00u, 0x00u, 0x14u, 0x00u, 0x00u, 0x00u, 0x16u, 0x00u, 0x00u, 0x00u,
        0x01u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x06u, 0x00u, 0x00u, 0x00u, 0x0au, 0x00u, 0x00u, 0x00u, 0x0cu, 0x00u, 0x00u, 0x00u, 0x01u, 0x00u, 0x00u, 0x00u,
        0x14u, 0x00u, 0x00u, 0x00u, 0x16u, 0x00u, 0x00u, 0x00u, 0x01u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x06u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x0au, 0x00u, 0x00u, 0x00u, 0x0cu, 0x00u, 0x00u, 0x00u, 0x01u, 0x00u, 0x00u, 0x00u, 0x14u, 0x00u, 0x00u, 0x00u,
        0x16u, 0x00u, 0x00u, 0x00u, 0x01u, 0x00u, 0x00u, 0x00u, 0x10u, 0x00u, 0x00u, 0x00u, 0x10u, 0x00u, 0x00u, 0x00u,
        0x10u, 0x00u, 0x00u, 0x00u, 0x06u, 0x00u, 0x00u, 0x00u, 0x0au, 0x00u, 0x00u, 0x00u, 0x0cu, 0x00u, 0x00u, 0x00u,
        0x01u, 0x00u, 0x00u, 0x00u, 0x14u, 0x00u, 0x00u, 0x00u, 0x16u, 0x00u, 0x00u, 0x00u, 0x01u, 0x00u, 0x00u, 0x00u,
        0xffu, 0x00u, 0x00u, 0x00u, 0xffu, 0x00u, 0x00u, 0x00u, 0xffu, 0x00u, 0x00u, 0x00u, 0x06u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x01u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x0au, 0x00u, 0x00u, 0x00u,
        0x0bu, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x01u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x0au, 0x00u, 0x00u, 0x00u, 0x0bu, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x10u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u
    };

    static const uint8 step3_bypass_mcts[] = {
        0x38u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x0fu, 0x00u, 0x00u, 0x13u, 0x00u, 0x00u, 0x00u,
        0xffu, 0x0fu, 0xffu, 0x0fu, 0x00u, 0xffu, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x40u, 0x54u,
        0x00u, 0x40u, 0x00u, 0x00u, 0x00u, 0x08u, 0x00u, 0x00u, 0x60u, 0x00u, 0x06u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x80u, 0x80u, 0xabu, 0x00u, 0xeau, 0x38u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x0au, 0x00u, 0x00u, 0x00u, 0x0cu, 0x00u, 0x00u, 0x00u, 0x01u, 0x00u, 0x00u, 0x00u, 0x14u, 0x00u, 0x00u, 0x00u,
        0x16u, 0x00u, 0x00u, 0x00u, 0x01u, 0x00u, 0x00u, 0x00u, 0x30u, 0x00u, 0x00u, 0x00u, 0x30u, 0x00u, 0x00u, 0x00u,
        0x30u, 0x00u, 0x00u, 0x00u, 0x06u, 0x00u, 0x00u, 0x00u, 0x0au, 0x00u, 0x00u, 0x00u, 0x0cu, 0x00u, 0x00u, 0x00u,
        0x01u, 0x00u, 0x00u, 0x00u, 0x14u, 0x00u, 0x00u, 0x00u, 0x16u, 0x00u, 0x00u, 0x00u, 0x01u, 0x00u, 0x00u, 0x00u,
        0x20u, 0x00u, 0x00u, 0x00u, 0x20u, 0x00u, 0x00u, 0x00u, 0x20u, 0x00u, 0x00u, 0x00u, 0x06u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x10u, 0x00u, 0x02u, 0xffu, 0xefu, 0xffu, 0x01u, 0x24u, 0x49u, 0x92u, 0x04u,
        0x00u, 0x00u, 0x00u, 0x00u, 0xdcu, 0xb6u, 0x6du, 0x03u, 0x00u, 0x00u, 0x00u, 0x00u, 0x1fu, 0x1fu, 0x1fu, 0x1fu,
        0x1fu, 0x1fu, 0x1fu, 0x1fu, 0x37u, 0x37u, 0x37u, 0x37u, 0x37u, 0x1fu, 0x1fu, 0x37u, 0x77u, 0xf6u, 0x77u, 0x37u,
        0x1fu, 0x1fu, 0x37u, 0xf6u, 0xf4u, 0x00u, 0x00u, 0x00u, 0x1fu, 0x1fu, 0x1fu, 0x1fu, 0x1fu, 0x1fu, 0x1fu, 0x1fu,
        0x37u, 0x37u, 0x37u, 0x37u, 0x37u, 0x1fu, 0x1fu, 0x37u, 0x77u, 0xf6u, 0x77u, 0x37u, 0x1fu, 0x1fu, 0x37u, 0xf6u,
        0xf4u, 0x00u, 0x00u, 0x00u, 0x1fu, 0x1fu, 0x1fu, 0x1fu, 0x1fu, 0x1fu, 0x1fu, 0x1fu, 0x37u, 0x37u, 0x37u, 0x37u,
        0x37u, 0x1fu, 0x1fu, 0x37u, 0x77u, 0xf6u, 0x77u, 0x37u, 0x1fu, 0x1fu, 0x37u, 0xf6u, 0xf4u, 0x00u, 0x00u, 0x00u,
        0x1fu, 0x1fu, 0x1fu, 0x1fu, 0x1fu, 0x1fu, 0x1fu, 0x1fu, 0x37u, 0x37u, 0x37u, 0x37u, 0x37u, 0x1fu, 0x1fu, 0x37u,
        0x77u, 0xf6u, 0x77u, 0x37u, 0x1fu, 0x1fu, 0x37u, 0xf6u, 0xf4u, 0x00u, 0x00u, 0x00u, 0x1fu, 0x1fu, 0x1fu, 0x1fu,
        0x1fu, 0x1fu, 0x1fu, 0x1fu, 0x37u, 0x37u, 0x37u, 0x37u, 0x37u, 0x1fu, 0x1fu, 0x37u, 0x77u, 0xf6u, 0x77u, 0x37u,
        0x1fu, 0x1fu, 0x37u, 0xf6u, 0xf4u, 0x00u, 0x00u, 0x00u, 0x1fu, 0x1fu, 0x1fu, 0x1fu, 0x1fu, 0x1fu, 0x1fu, 0x1fu,
        0x37u, 0x37u, 0x37u, 0x37u, 0x37u, 0x1fu, 0x1fu, 0x37u, 0x77u, 0xf6u, 0x77u, 0x37u, 0x1fu, 0x1fu, 0x37u, 0xf6u,
        0xf4u, 0x00u, 0x00u, 0x00u, 0x1fu, 0x1fu, 0x1fu, 0x1fu, 0x1fu, 0x1fu, 0x1fu, 0x1fu, 0x37u, 0x37u, 0x37u, 0x37u,
        0x37u, 0x1fu, 0x1fu, 0x37u, 0x77u, 0xf6u, 0x77u, 0x37u, 0x1fu, 0x1fu, 0x37u, 0xf6u, 0xf4u, 0x00u, 0x00u, 0x00u,
        0x1fu, 0x1fu, 0x1fu, 0x1fu, 0x1fu, 0x1fu, 0x1fu, 0x1fu, 0x37u, 0x37u, 0x37u, 0x37u, 0x37u, 0x1fu, 0x1fu, 0x37u,
        0x77u, 0xf6u, 0x77u, 0x37u, 0x1fu, 0x1fu, 0x37u, 0xf6u, 0xf4u, 0x00u, 0x00u, 0x00u, 0x1fu, 0x1fu, 0x1fu, 0x1fu,
        0x1fu, 0x1fu, 0x1fu, 0x1fu, 0x37u, 0x37u, 0x37u, 0x37u, 0x37u, 0x1fu, 0x1fu, 0x37u, 0x77u, 0xf6u, 0x77u, 0x37u,
        0x1fu, 0x1fu, 0x37u, 0xf6u, 0xf4u, 0x00u, 0x00u, 0x00u, 0x1fu, 0x1fu, 0x1fu, 0x1fu, 0x1fu, 0x1fu, 0x1fu, 0x1fu,
        0x37u, 0x37u, 0x37u, 0x37u, 0x37u, 0x1fu, 0x1fu, 0x37u, 0x77u, 0xf6u, 0x77u, 0x37u, 0x1fu, 0x1fu, 0x37u, 0xf6u,
        0xf4u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x2eu, 0xe4u, 0x5cu, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x2eu, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0xe4u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x5cu, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x2eu, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x66u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x58u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x2eu, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x66u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x58u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x77u, 0x29u, 0x00u, 0x00u, 0x00u, 0xf8u, 0x50u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x29u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0xf8u, 0x77u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x50u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x29u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x77u, 0xf8u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x50u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x29u, 0x77u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0xf8u,
        0x50u, 0x00u, 0x00u, 0x00u, 0x00u, 0x10u, 0x00u, 0x02u, 0xffu, 0xefu, 0xffu, 0x01u, 0x0au, 0x00u, 0x00u, 0x00u,
        0x0cu, 0x00u, 0x00u, 0x00u, 0x01u, 0x00u, 0x00u, 0x00u, 0x14u, 0x00u, 0x00u, 0x00u, 0x16u, 0x00u, 0x00u, 0x00u,
        0x01u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x06u, 0x00u, 0x00u, 0x00u, 0x0au, 0x00u, 0x00u, 0x00u, 0x0cu, 0x00u, 0x00u, 0x00u, 0x01u, 0x00u, 0x00u, 0x00u,
        0x14u, 0x00u, 0x00u, 0x00u, 0x16u, 0x00u, 0x00u, 0x00u, 0x01u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x06u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x0au, 0x00u, 0x00u, 0x00u, 0x0cu, 0x00u, 0x00u, 0x00u, 0x01u, 0x00u, 0x00u, 0x00u, 0x14u, 0x00u, 0x00u, 0x00u,
        0x16u, 0x00u, 0x00u, 0x00u, 0x01u, 0x00u, 0x00u, 0x00u, 0x10u, 0x00u, 0x00u, 0x00u, 0x10u, 0x00u, 0x00u, 0x00u,
        0x10u, 0x00u, 0x00u, 0x00u, 0x06u, 0x00u, 0x00u, 0x00u, 0x0au, 0x00u, 0x00u, 0x00u, 0x0cu, 0x00u, 0x00u, 0x00u,
        0x01u, 0x00u, 0x00u, 0x00u, 0x14u, 0x00u, 0x00u, 0x00u, 0x16u, 0x00u, 0x00u, 0x00u, 0x01u, 0x00u, 0x00u, 0x00u,
        0xffu, 0x00u, 0x00u, 0x00u, 0xffu, 0x00u, 0x00u, 0x00u, 0xffu, 0x00u, 0x00u, 0x00u, 0x06u, 0x00u, 0x00u, 0x00u,
        0xe6u, 0x00u, 0x00u, 0x00u, 0xeau, 0x00u, 0x00u, 0x00u, 0x02u, 0x00u, 0x00u, 0x00u, 0xebu, 0x00u, 0x00u, 0x00u,
        0xefu, 0x00u, 0x00u, 0x00u, 0x02u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x27u, 0x00u, 0x00u, 0x00u,
        0x40u, 0x00u, 0x00u, 0x00u, 0x02u, 0x00u, 0x00u, 0x00u, 0x46u, 0x00u, 0x00u, 0x00u, 0x66u, 0x00u, 0x00u, 0x00u,
        0x05u, 0x00u, 0x00u, 0x00u, 0x9bu, 0x00u, 0x00u, 0x00u, 0xbbu, 0x00u, 0x00u, 0x00u, 0x05u, 0x00u, 0x00u, 0x00u,
        0x10u, 0x00u, 0x00u, 0x00u, 0x10u, 0x00u, 0x00u, 0x00u, 0x10u, 0x00u, 0x00u, 0x00u, 0x06u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x10u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u
    };

    static const uint8 step4_bypass_mcts[] = {
        0x38u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x0fu, 0x00u, 0x00u, 0x13u, 0x00u, 0x00u, 0x00u,
        0xffu, 0xffu, 0xffu, 0xffu, 0x00u, 0xffu, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x80u, 0x50u,
        0x00u, 0x40u, 0x00u, 0x00u, 0x00u, 0x08u, 0x00u, 0x00u, 0x28u, 0x80u, 0x02u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x80u, 0x80u, 0xcdu, 0x00u, 0xeau, 0x34u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x0au, 0x00u, 0x00u, 0x00u, 0x0cu, 0x00u, 0x00u, 0x00u, 0x01u, 0x00u, 0x00u, 0x00u, 0x14u, 0x00u, 0x00u, 0x00u,
        0x16u, 0x00u, 0x00u, 0x00u, 0x01u, 0x00u, 0x00u, 0x00u, 0x30u, 0x00u, 0x00u, 0x00u, 0x30u, 0x00u, 0x00u, 0x00u,
        0x30u, 0x00u, 0x00u, 0x00u, 0x06u, 0x00u, 0x00u, 0x00u, 0x0au, 0x00u, 0x00u, 0x00u, 0x0cu, 0x00u, 0x00u, 0x00u,
        0x01u, 0x00u, 0x00u, 0x00u, 0x14u, 0x00u, 0x00u, 0x00u, 0x16u, 0x00u, 0x00u, 0x00u, 0x01u, 0x00u, 0x00u, 0x00u,
        0x20u, 0x00u, 0x00u, 0x00u, 0x20u, 0x00u, 0x00u, 0x00u, 0x20u, 0x00u, 0x00u, 0x00u, 0x06u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x10u, 0x00u, 0x02u, 0xffu, 0xefu, 0xffu, 0x01u, 0x24u, 0x49u, 0x92u, 0x04u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x24u, 0xc9u, 0x6du, 0x03u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x64u, 0xe4u, 0x64u, 0x00u,
        0x00u, 0x00u, 0x00u, 0xe4u, 0xe0u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x64u, 0xe4u, 0x64u, 0x00u, 0x00u, 0x00u, 0x00u, 0xe4u,
        0xe0u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x64u, 0xe4u, 0x64u, 0x00u, 0x00u, 0x00u, 0x00u, 0xe4u, 0xe0u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x64u, 0xe4u, 0x64u, 0x00u, 0x00u, 0x00u, 0x00u, 0xe4u, 0xe0u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x64u, 0xe4u, 0x64u, 0x00u,
        0x00u, 0x00u, 0x00u, 0xe4u, 0xe0u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x64u, 0xe4u, 0x64u, 0x00u, 0x00u, 0x00u, 0x00u, 0xe4u,
        0xe0u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x64u, 0xe4u, 0x64u, 0x00u, 0x00u, 0x00u, 0x00u, 0xe4u, 0xe0u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x64u, 0xe4u, 0x64u, 0x00u, 0x00u, 0x00u, 0x00u, 0xe4u, 0xe0u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x64u, 0xe4u, 0x64u, 0x00u,
        0x00u, 0x00u, 0x00u, 0xe4u, 0xe0u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x64u, 0xe4u, 0x64u, 0x00u, 0x00u, 0x00u, 0x00u, 0xe4u,
        0xe0u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x25u, 0xc2u, 0xb2u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x25u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0xc2u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0xb2u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x25u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x4bu, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0xa0u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x25u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x4bu, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0xa0u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x79u, 0x2au, 0x00u, 0x00u, 0x00u, 0xf9u, 0x48u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x2au, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0xf9u, 0x79u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x48u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x2au, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x79u, 0xf9u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x48u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x2au, 0x79u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0xf9u,
        0x48u, 0x00u, 0x00u, 0x00u, 0x00u, 0x10u, 0x00u, 0x02u, 0xffu, 0xefu, 0xffu, 0x01u, 0x0au, 0x00u, 0x00u, 0x00u,
        0x0cu, 0x00u, 0x00u, 0x00u, 0x01u, 0x00u, 0x00u, 0x00u, 0x14u, 0x00u, 0x00u, 0x00u, 0x16u, 0x00u, 0x00u, 0x00u,
        0x01u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x06u, 0x00u, 0x00u, 0x00u, 0x0au, 0x00u, 0x00u, 0x00u, 0x0cu, 0x00u, 0x00u, 0x00u, 0x01u, 0x00u, 0x00u, 0x00u,
        0x14u, 0x00u, 0x00u, 0x00u, 0x16u, 0x00u, 0x00u, 0x00u, 0x01u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x06u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x0au, 0x00u, 0x00u, 0x00u, 0x0cu, 0x00u, 0x00u, 0x00u, 0x01u, 0x00u, 0x00u, 0x00u, 0x14u, 0x00u, 0x00u, 0x00u,
        0x16u, 0x00u, 0x00u, 0x00u, 0x01u, 0x00u, 0x00u, 0x00u, 0x10u, 0x00u, 0x00u, 0x00u, 0x10u, 0x00u, 0x00u, 0x00u,
        0x10u, 0x00u, 0x00u, 0x00u, 0x06u, 0x00u, 0x00u, 0x00u, 0x0au, 0x00u, 0x00u, 0x00u, 0x0cu, 0x00u, 0x00u, 0x00u,
        0x01u, 0x00u, 0x00u, 0x00u, 0x14u, 0x00u, 0x00u, 0x00u, 0x16u, 0x00u, 0x00u, 0x00u, 0x01u, 0x00u, 0x00u, 0x00u,
        0xffu, 0x00u, 0x00u, 0x00u, 0xffu, 0x00u, 0x00u, 0x00u, 0xffu, 0x00u, 0x00u, 0x00u, 0x06u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x20u, 0x00u, 0x00u, 0x00u, 0x05u, 0x00u, 0x00u, 0x00u, 0x64u, 0x00u, 0x00u, 0x00u,
        0x74u, 0x00u, 0x00u, 0x00u, 0x04u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x06u, 0x00u, 0x00u, 0x00u, 0x3cu, 0x00u, 0x00u, 0x00u, 0x5cu, 0x00u, 0x00u, 0x00u,
        0x05u, 0x00u, 0x00u, 0x00u, 0x82u, 0x00u, 0x00u, 0x00u, 0xc2u, 0x00u, 0x00u, 0x00u, 0x06u, 0x00u, 0x00u, 0x00u,
        0x10u, 0x00u, 0x00u, 0x00u, 0x10u, 0x00u, 0x00u, 0x00u, 0x10u, 0x00u, 0x00u, 0x00u, 0x06u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x10u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u
    };

    static const uint8 step5_bypass_mcts[] = {
        0x38u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x0fu, 0x00u, 0x00u, 0x13u, 0x00u, 0x00u, 0x00u,
        0xffu, 0x04u, 0xffu, 0x04u, 0x00u, 0xffu, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x80u, 0x50u,
        0x00u, 0x40u, 0x00u, 0x00u, 0x00u, 0x08u, 0x00u, 0x00u, 0x38u, 0xc0u, 0x03u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x80u, 0x80u, 0x80u, 0x00u, 0xeau, 0x34u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x0au, 0x00u, 0x00u, 0x00u, 0x0cu, 0x00u, 0x00u, 0x00u, 0x01u, 0x00u, 0x00u, 0x00u, 0x14u, 0x00u, 0x00u, 0x00u,
        0x16u, 0x00u, 0x00u, 0x00u, 0x01u, 0x00u, 0x00u, 0x00u, 0x30u, 0x00u, 0x00u, 0x00u, 0x30u, 0x00u, 0x00u, 0x00u,
        0x30u, 0x00u, 0x00u, 0x00u, 0x06u, 0x00u, 0x00u, 0x00u, 0x0au, 0x00u, 0x00u, 0x00u, 0x0cu, 0x00u, 0x00u, 0x00u,
        0x01u, 0x00u, 0x00u, 0x00u, 0x14u, 0x00u, 0x00u, 0x00u, 0x16u, 0x00u, 0x00u, 0x00u, 0x01u, 0x00u, 0x00u, 0x00u,
        0x20u, 0x00u, 0x00u, 0x00u, 0x20u, 0x00u, 0x00u, 0x00u, 0x20u, 0x00u, 0x00u, 0x00u, 0x06u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x10u, 0x00u, 0x02u, 0xffu, 0xefu, 0xffu, 0x01u, 0x24u, 0x49u, 0x92u, 0x04u,
        0x00u, 0x00u, 0x00u, 0x00u, 0xdcu, 0x36u, 0x92u, 0x04u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x7du, 0xeeu, 0x7du, 0x00u,
        0x00u, 0x00u, 0x00u, 0xeeu, 0x54u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x7du, 0xeeu, 0x7du, 0x00u, 0x00u, 0x00u, 0x00u, 0xeeu,
        0x54u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x7du, 0xeeu, 0x7du, 0x00u, 0x00u, 0x00u, 0x00u, 0xeeu, 0x54u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x7du, 0xeeu, 0x7du, 0x00u, 0x00u, 0x00u, 0x00u, 0xeeu, 0x54u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x7du, 0xeeu, 0x7du, 0x00u,
        0x00u, 0x00u, 0x00u, 0xeeu, 0x54u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x7du, 0xeeu, 0x7du, 0x00u, 0x00u, 0x00u, 0x00u, 0xeeu,
        0x54u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x7du, 0xeeu, 0x7du, 0x00u, 0x00u, 0x00u, 0x00u, 0xeeu, 0x54u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x7du, 0xeeu, 0x7du, 0x00u, 0x00u, 0x00u, 0x00u, 0xeeu, 0x54u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x7du, 0xeeu, 0x7du, 0x00u,
        0x00u, 0x00u, 0x00u, 0xeeu, 0x54u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x7du, 0xeeu, 0x7du, 0x00u, 0x00u, 0x00u, 0x00u, 0xeeu,
        0x54u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x10u, 0x2eu, 0xeeu, 0x68u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x10u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x2eu, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0xeeu, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x68u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x13u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x31u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x71u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x56u, 0x00u, 0x00u, 0x00u, 0x13u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x31u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x71u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x56u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x19u, 0x00u, 0x00u, 0x00u, 0x00u, 0x76u, 0x28u, 0x19u, 0x00u, 0x00u, 0xf6u, 0x74u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x19u, 0x19u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x28u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0xf6u, 0x76u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x74u, 0x00u, 0x00u, 0x00u, 0x00u, 0x19u, 0x19u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x28u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x76u, 0xf6u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x74u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x19u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x19u, 0x28u, 0x76u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0xf6u,
        0x74u, 0x00u, 0x00u, 0x00u, 0x00u, 0x10u, 0x00u, 0x02u, 0xffu, 0xefu, 0xffu, 0x01u, 0x0au, 0x00u, 0x00u, 0x00u,
        0x0cu, 0x00u, 0x00u, 0x00u, 0x01u, 0x00u, 0x00u, 0x00u, 0x14u, 0x00u, 0x00u, 0x00u, 0x16u, 0x00u, 0x00u, 0x00u,
        0x01u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x06u, 0x00u, 0x00u, 0x00u, 0x0au, 0x00u, 0x00u, 0x00u, 0x0cu, 0x00u, 0x00u, 0x00u, 0x01u, 0x00u, 0x00u, 0x00u,
        0x14u, 0x00u, 0x00u, 0x00u, 0x16u, 0x00u, 0x00u, 0x00u, 0x01u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x06u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x0au, 0x00u, 0x00u, 0x00u, 0x0cu, 0x00u, 0x00u, 0x00u, 0x01u, 0x00u, 0x00u, 0x00u, 0x14u, 0x00u, 0x00u, 0x00u,
        0x16u, 0x00u, 0x00u, 0x00u, 0x01u, 0x00u, 0x00u, 0x00u, 0x10u, 0x00u, 0x00u, 0x00u, 0x10u, 0x00u, 0x00u, 0x00u,
        0x10u, 0x00u, 0x00u, 0x00u, 0x06u, 0x00u, 0x00u, 0x00u, 0x0au, 0x00u, 0x00u, 0x00u, 0x0cu, 0x00u, 0x00u, 0x00u,
        0x01u, 0x00u, 0x00u, 0x00u, 0x14u, 0x00u, 0x00u, 0x00u, 0x16u, 0x00u, 0x00u, 0x00u, 0x01u, 0x00u, 0x00u, 0x00u,
        0xffu, 0x00u, 0x00u, 0x00u, 0xffu, 0x00u, 0x00u, 0x00u, 0xffu, 0x00u, 0x00u, 0x00u, 0x06u, 0x00u, 0x00u, 0x00u,
        0xe6u, 0x00u, 0x00u, 0x00u, 0xeau, 0x00u, 0x00u, 0x00u, 0x02u, 0x00u, 0x00u, 0x00u, 0xebu, 0x00u, 0x00u, 0x00u,
        0xefu, 0x00u, 0x00u, 0x00u, 0x02u, 0x00u, 0x00u, 0x00u, 0x40u, 0x00u, 0x00u, 0x00u, 0x27u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x02u, 0x00u, 0x00u, 0x00u, 0x64u, 0x00u, 0x00u, 0x00u, 0x84u, 0x00u, 0x00u, 0x00u,
        0x05u, 0x00u, 0x00u, 0x00u, 0x9bu, 0x00u, 0x00u, 0x00u, 0xbbu, 0x00u, 0x00u, 0x00u, 0x05u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x06u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x10u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u
    };

    static const uint8 passthrough_mctfa[] = {
        0x14u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0xffu, 0xffu, 0xffu, 0xffu, 0xffu, 0xffu, 0xffu, 0xffu, 0xffu, 0xffu, 0xffu, 0xffu, 0x05u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x80u, 0x88u, 0x00u, 0xffu, 0xffu, 0xffu, 0x00u, 0x01u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x88u, 0x08u, 0x88u, 0x08u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x01u, 0x01u, 0x01u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x01u, 0x01u, 0x01u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x01u, 0x01u, 0x01u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x01u, 0x01u, 0x01u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x0au, 0x0au, 0x00u, 0x01u, 0x0cu, 0x0cu, 0x00u,
        0x48u, 0x00u, 0x00u, 0x00u, 0x0au, 0x14u, 0x14u, 0x00u, 0x0bu, 0x18u, 0x18u, 0x00u, 0x90u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x40u, 0x08u, 0x00u, 0x00u,
        0x00u, 0xe6u, 0xe6u, 0x00u, 0x01u, 0xeau, 0xeau, 0x00u, 0x90u, 0x00u, 0x00u, 0x00u, 0x0au, 0xebu, 0xebu, 0x00u,
        0x0bu, 0xefu, 0xefu, 0x00u, 0x90u, 0x00u, 0x00u, 0x00u, 0xffu, 0xffu, 0xffu, 0x00u, 0xffu, 0xffu, 0xffu, 0x00u,
        0xffu, 0x07u, 0x07u, 0x00u, 0x40u, 0x08u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u
    };

    static const uint8 step10_shpa[] = {
        0x89u, 0x40u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0xffu, 0x07u, 0x00u, 0x04u, 0xffu, 0xffu, 0x00u, 0x00u,
        0x00u, 0xffu, 0x00u, 0x00u, 0x00u, 0xf0u, 0xffu, 0x01u, 0x00u, 0xf0u, 0xffu, 0x01u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0xb6u, 0x0du, 0xc0u, 0x36u, 0x06u, 0x09u, 0x00u, 0x00u, 0x0au, 0x14u, 0x00u, 0x00u,
        0x0cu, 0x16u, 0x00u, 0x00u, 0x09u, 0x00u, 0x00u, 0x00u, 0x20u, 0x20u, 0x20u, 0x00u, 0x0au, 0x14u, 0x00u, 0x00u,
        0x0cu, 0x16u, 0x00u, 0x00u, 0x09u, 0x00u, 0x00u, 0x00u, 0x20u, 0x20u, 0x20u, 0x00u, 0x0au, 0x14u, 0x00u, 0x00u,
        0x0cu, 0x16u, 0x00u, 0x00u, 0x09u, 0x00u, 0xfcu, 0x7fu, 0x00u, 0x00u, 0x00u, 0x00u, 0x0au, 0x14u, 0x00u, 0x00u,
        0x0cu, 0x16u, 0x00u, 0x00u, 0x09u, 0x00u, 0xfcu, 0x7fu, 0x00u, 0x00u, 0x00u, 0x00u, 0x0au, 0x14u, 0x00u, 0x00u,
        0x0cu, 0x16u, 0x00u, 0x00u, 0x09u, 0x00u, 0x00u, 0x00u, 0x10u, 0x10u, 0x10u, 0x00u, 0x0au, 0x14u, 0x00u, 0x00u,
        0x0cu, 0x16u, 0x00u, 0x00u, 0x09u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x0au, 0x14u, 0x00u, 0x00u,
        0x0cu, 0x16u, 0x00u, 0x00u, 0x09u, 0x00u, 0x00u, 0x00u, 0xffu, 0xffu, 0xffu, 0x00u, 0x0au, 0x14u, 0x00u, 0x00u,
        0x0cu, 0x16u, 0x00u, 0x00u, 0x09u, 0x00u, 0x00u, 0x00u, 0x10u, 0x10u, 0x10u, 0x00u, 0x34u, 0x9du, 0x2fu, 0x00u,
        0xffu, 0xffu, 0xffu, 0x3fu, 0xffu, 0x7fu, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x49u, 0x00u, 0x08u, 0x00u, 0x80u, 0x00u, 0x00u, 0x00u, 0x3fu, 0x01u, 0xefu, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0xd0u, 0x00u, 0x00u, 0x00u, 0xf0u, 0xf0u, 0x7bu, 0x7bu, 0x3bu, 0x3bu, 0x30u, 0x3bu, 0x3bu, 0x3bu, 0x3bu, 0x30u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0xd0u, 0x00u, 0x00u, 0x00u,
        0xf0u, 0xf0u, 0x7bu, 0x7bu, 0x3bu, 0x3bu, 0x30u, 0x3bu, 0x3bu, 0x3bu, 0x3bu, 0x30u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0xd0u, 0x00u, 0x00u, 0x00u, 0xf0u, 0xf0u, 0x7bu, 0x7bu,
        0x3bu, 0x3bu, 0x30u, 0x3bu, 0x3bu, 0x3bu, 0x3bu, 0x30u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0xd0u, 0x00u, 0x00u, 0x00u, 0xf0u, 0xf0u, 0x7bu, 0x7bu, 0x3bu, 0x3bu, 0x30u, 0x3bu,
        0x3bu, 0x3bu, 0x3bu, 0x30u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0xd0u, 0x00u, 0x00u, 0x00u, 0xf0u, 0xf0u, 0x7bu, 0x7bu, 0x3bu, 0x3bu, 0x30u, 0x3bu, 0x3bu, 0x3bu, 0x3bu, 0x30u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0xd0u, 0x00u, 0x00u, 0x00u,
        0xf0u, 0xf0u, 0x7bu, 0x7bu, 0x3bu, 0x3bu, 0x30u, 0x3bu, 0x3bu, 0x3bu, 0x3bu, 0x30u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0xd0u, 0x00u, 0x00u, 0x00u, 0xf0u, 0xf0u, 0x7bu, 0x7bu,
        0x3bu, 0x3bu, 0x30u, 0x3bu, 0x3bu, 0x3bu, 0x3bu, 0x30u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0xd0u, 0x00u, 0x00u, 0x00u, 0xf0u, 0xf0u, 0x7bu, 0x7bu, 0x3bu, 0x3bu, 0x30u, 0x3bu,
        0x3bu, 0x3bu, 0x3bu, 0x30u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0xd0u, 0x00u, 0x00u, 0x00u, 0xf0u, 0xf0u, 0x7bu, 0x7bu, 0x3bu, 0x3bu, 0x30u, 0x3bu, 0x3bu, 0x3bu, 0x3bu, 0x30u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x24u, 0x49u, 0x92u, 0x04u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x24u, 0x49u, 0x92u, 0x04u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x01u, 0x03u, 0x05u, 0x07u, 0x09u, 0x0cu, 0x0fu, 0x11u, 0x14u, 0x17u, 0x1bu, 0x1du, 0x21u,
        0x23u, 0x24u, 0x26u, 0x28u, 0x2au, 0x2cu, 0x2eu, 0x2fu, 0x32u, 0x34u, 0x35u, 0x37u, 0x39u, 0x3au, 0x3cu, 0x3eu,
        0x40u, 0x42u, 0x43u, 0x45u, 0x47u, 0x49u, 0x4bu, 0x4cu, 0x4eu, 0x50u, 0x51u, 0x53u, 0x54u, 0x56u, 0x57u, 0x59u,
        0x5au, 0x5cu, 0x5eu, 0x60u, 0x61u, 0x63u, 0x64u, 0x66u, 0x67u, 0x69u, 0x6au, 0x6cu, 0x6du, 0x6fu, 0x70u, 0x71u,
        0x73u, 0x76u, 0x77u, 0x79u, 0x7bu, 0x7cu, 0x7eu, 0x7fu, 0x81u, 0x83u, 0x84u, 0x86u, 0x87u, 0x89u, 0x8bu, 0x8bu,
        0x8cu, 0x8du, 0x8fu, 0x90u, 0x91u, 0x92u, 0x94u, 0x94u, 0x95u, 0x96u, 0x97u, 0x99u, 0x99u, 0x9au, 0x9bu, 0x9cu,
        0x9cu, 0x9eu, 0x9fu, 0x9fu, 0xa0u, 0xa1u, 0xa1u, 0xa3u, 0xa4u, 0xa4u, 0xa5u, 0xa6u, 0xa6u, 0xa8u, 0xa9u, 0xa9u,
        0xaau, 0xabu, 0xabu, 0xadu, 0xadu, 0xaeu, 0xafu, 0xafu, 0xb0u, 0xb0u, 0xb2u, 0xb3u, 0xb3u, 0xb4u, 0xb4u, 0xb5u,
        0xb7u, 0xb7u, 0xb8u, 0xb8u, 0xb9u, 0xb9u, 0xbau, 0xbau, 0xbbu, 0xbbu, 0xbcu, 0xbcu, 0xbeu, 0xbeu, 0xbfu, 0xbfu,
        0xc0u, 0xc0u, 0xc1u, 0xc1u, 0xc2u, 0xc2u, 0xc2u, 0xc3u, 0xc3u, 0xc5u, 0xc5u, 0xc7u, 0xc7u, 0xc7u, 0xc8u, 0xc8u,
        0xcau, 0xcau, 0xcau, 0xccu, 0xccu, 0xcdu, 0xcdu, 0xcfu, 0xcfu, 0xcfu, 0xd1u, 0xd1u, 0xd1u, 0xd3u, 0xd3u, 0xd6u,
        0xd6u, 0xd6u, 0xd8u, 0xd8u, 0xd8u, 0xdau, 0xdau, 0xdau, 0xdau, 0xdcu, 0xdcu, 0xdcu, 0xdfu, 0xdfu, 0xdfu, 0xdfu,
        0xe1u, 0xe1u, 0xe1u, 0xe3u, 0xe3u, 0xe3u, 0xe6u, 0xe6u, 0xe6u, 0xe6u, 0xe8u, 0xe8u, 0xe8u, 0xe8u, 0xeau, 0xeau,
        0xeau, 0xedu, 0xedu, 0xedu, 0xefu, 0xefu, 0xefu, 0xefu, 0xefu, 0xf1u, 0xf1u, 0xf1u, 0xf1u, 0xf2u, 0xf2u, 0xf2u,
        0xf2u, 0xf2u, 0xf4u, 0xf4u, 0xf4u, 0xf4u, 0xf6u, 0xf6u, 0xf6u, 0xf6u, 0xf8u, 0xf8u, 0xf8u, 0xf8u, 0xf8u, 0xfau,
        0xfau, 0xfau, 0xfau, 0xfau, 0xfcu, 0xfcu, 0xfcu, 0xfcu, 0xfeu, 0xfeu, 0xfeu, 0xfeu, 0xfeu, 0xffu, 0xffu, 0xffu
    };

    static const uint8 passthrough_shpb[] = {
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0xffu, 0x07u, 0x00u, 0x15u, 0x00u, 0x00u, 0x00u,
        0xffu, 0xffu, 0xffu, 0xffu, 0x00u, 0xffu, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x80u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x0au, 0x00u, 0x00u, 0x00u, 0x0cu, 0x00u, 0x00u, 0x00u, 0x01u, 0x00u, 0x00u, 0x00u, 0x14u, 0x00u, 0x00u, 0x00u,
        0x16u, 0x00u, 0x00u, 0x00u, 0x01u, 0x00u, 0x00u, 0x00u, 0x20u, 0x00u, 0x00u, 0x00u, 0x20u, 0x00u, 0x00u, 0x00u,
        0x20u, 0x00u, 0x00u, 0x00u, 0x06u, 0x00u, 0x00u, 0x00u, 0x0au, 0x00u, 0x00u, 0x00u, 0x0cu, 0x00u, 0x00u, 0x00u,
        0x01u, 0x00u, 0x00u, 0x00u, 0x14u, 0x00u, 0x00u, 0x00u, 0x16u, 0x00u, 0x00u, 0x00u, 0x01u, 0x00u, 0x00u, 0x00u,
        0x20u, 0x00u, 0x00u, 0x00u, 0x20u, 0x00u, 0x00u, 0x00u, 0x20u, 0x00u, 0x00u, 0x00u, 0x06u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x10u, 0x00u, 0x02u, 0xffu, 0xefu, 0xffu, 0x01u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x10u, 0xe0u, 0x03u, 0xffu, 0xefu, 0x1fu, 0x00u, 0x0au, 0x00u, 0x00u, 0x00u,
        0x0cu, 0x00u, 0x00u, 0x00u, 0x01u, 0x00u, 0x00u, 0x00u, 0x14u, 0x00u, 0x00u, 0x00u, 0x16u, 0x00u, 0x00u, 0x00u,
        0x01u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x06u, 0x00u, 0x00u, 0x00u, 0x0au, 0x00u, 0x00u, 0x00u, 0x0cu, 0x00u, 0x00u, 0x00u, 0x01u, 0x00u, 0x00u, 0x00u,
        0x14u, 0x00u, 0x00u, 0x00u, 0x16u, 0x00u, 0x00u, 0x00u, 0x01u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x06u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x0au, 0x00u, 0x00u, 0x00u, 0x0cu, 0x00u, 0x00u, 0x00u, 0x01u, 0x00u, 0x00u, 0x00u, 0x14u, 0x00u, 0x00u, 0x00u,
        0x16u, 0x00u, 0x00u, 0x00u, 0x01u, 0x00u, 0x00u, 0x00u, 0x10u, 0x00u, 0x00u, 0x00u, 0x10u, 0x00u, 0x00u, 0x00u,
        0x10u, 0x00u, 0x00u, 0x00u, 0x06u, 0x00u, 0x00u, 0x00u, 0x0au, 0x00u, 0x00u, 0x00u, 0x0cu, 0x00u, 0x00u, 0x00u,
        0x01u, 0x00u, 0x00u, 0x00u, 0x14u, 0x00u, 0x00u, 0x00u, 0x16u, 0x00u, 0x00u, 0x00u, 0x01u, 0x00u, 0x00u, 0x00u,
        0xffu, 0x00u, 0x00u, 0x00u, 0xffu, 0x00u, 0x00u, 0x00u, 0xffu, 0x00u, 0x00u, 0x00u, 0x06u, 0x00u, 0x00u, 0x00u,
        0x0au, 0x00u, 0x00u, 0x00u, 0x0cu, 0x00u, 0x00u, 0x00u, 0x01u, 0x00u, 0x00u, 0x00u, 0x14u, 0x00u, 0x00u, 0x00u,
        0x16u, 0x00u, 0x00u, 0x00u, 0x01u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x06u, 0x00u, 0x00u, 0x00u, 0x0au, 0x00u, 0x00u, 0x00u, 0x0cu, 0x00u, 0x00u, 0x00u,
        0x01u, 0x00u, 0x00u, 0x00u, 0x14u, 0x00u, 0x00u, 0x00u, 0x16u, 0x00u, 0x00u, 0x00u, 0x01u, 0x00u, 0x00u, 0x00u,
        0x10u, 0x00u, 0x00u, 0x00u, 0x10u, 0x00u, 0x00u, 0x00u, 0x10u, 0x00u, 0x00u, 0x00u, 0x06u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x10u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u
    };

    static const uint8 step12_li2_hi_combine_mcts_disable[] = {
        0x38u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x13u, 0x00u, 0x00u, 0x00u,
        0xffu, 0xffu, 0xffu, 0xffu, 0x00u, 0xffu, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x80u, 0x00u,
        0x00u, 0x40u, 0x00u, 0x00u, 0x00u, 0x08u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x80u, 0x80u, 0x00u, 0x00u, 0xeau, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x0au, 0x00u, 0x00u, 0x00u, 0x0cu, 0x00u, 0x00u, 0x00u, 0x01u, 0x00u, 0x00u, 0x00u, 0x14u, 0x00u, 0x00u, 0x00u,
        0x16u, 0x00u, 0x00u, 0x00u, 0x01u, 0x00u, 0x00u, 0x00u, 0x30u, 0x00u, 0x00u, 0x00u, 0x30u, 0x00u, 0x00u, 0x00u,
        0x30u, 0x00u, 0x00u, 0x00u, 0x06u, 0x00u, 0x00u, 0x00u, 0x0au, 0x00u, 0x00u, 0x00u, 0x0cu, 0x00u, 0x00u, 0x00u,
        0x01u, 0x00u, 0x00u, 0x00u, 0x14u, 0x00u, 0x00u, 0x00u, 0x16u, 0x00u, 0x00u, 0x00u, 0x01u, 0x00u, 0x00u, 0x00u,
        0x20u, 0x00u, 0x00u, 0x00u, 0x20u, 0x00u, 0x00u, 0x00u, 0x20u, 0x00u, 0x00u, 0x00u, 0x06u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x10u, 0x00u, 0x02u, 0xffu, 0xefu, 0xffu, 0x01u, 0x24u, 0x49u, 0x92u, 0x04u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x24u, 0x49u, 0x92u, 0x04u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x10u, 0x02u, 0x02u, 0xffu, 0x0fu, 0xfeu, 0x01u, 0x0au, 0x00u, 0x00u, 0x00u,
        0x0cu, 0x00u, 0x00u, 0x00u, 0x01u, 0x00u, 0x00u, 0x00u, 0x14u, 0x00u, 0x00u, 0x00u, 0x16u, 0x00u, 0x00u, 0x00u,
        0x01u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x06u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x01u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x01u, 0x00u, 0x00u, 0x00u, 0x02u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x10u, 0x00u, 0x00u, 0x00u,
        0x10u, 0x00u, 0x00u, 0x00u, 0x10u, 0x00u, 0x00u, 0x00u, 0x0bu, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0xffu, 0xffu, 0x0fu, 0x00u, 0xffu, 0xffu, 0x0fu, 0x00u, 0xffu, 0xffu, 0x0fu, 0x00u, 0xffu, 0xffu, 0x0fu, 0x00u,
        0xffu, 0xffu, 0x0fu, 0x00u, 0xffu, 0xffu, 0x0fu, 0x00u, 0xffu, 0xffu, 0x0fu, 0x00u, 0xffu, 0xffu, 0x0fu, 0x00u,
        0xffu, 0xffu, 0x0fu, 0x00u, 0xffu, 0xffu, 0x0fu, 0x00u, 0xffu, 0xffu, 0x0fu, 0x00u, 0xffu, 0xffu, 0x0fu, 0x00u,
        0xffu, 0xffu, 0x0fu, 0x00u, 0xffu, 0xffu, 0x0fu, 0x00u, 0xffu, 0xffu, 0x0fu, 0x00u, 0xffu, 0xffu, 0x0fu, 0x00u,
        0xffu, 0xffu, 0x0fu, 0x00u, 0xffu, 0xffu, 0x0fu, 0x00u, 0xffu, 0xffu, 0x0fu, 0x00u, 0xffu, 0xffu, 0x0fu, 0x00u,
        0xffu, 0xffu, 0x0fu, 0x00u, 0xffu, 0xffu, 0x0fu, 0x00u, 0xffu, 0xffu, 0x0fu, 0x00u, 0xffu, 0xffu, 0x0fu, 0x00u,
        0xffu, 0xffu, 0x0fu, 0x00u, 0xffu, 0xffu, 0x0fu, 0x00u, 0xffu, 0xffu, 0x0fu, 0x00u, 0xffu, 0xffu, 0x0fu, 0x00u,
        0xffu, 0xffu, 0x0fu, 0x00u, 0xffu, 0xffu, 0x0fu, 0x00u, 0xffu, 0xffu, 0x0fu, 0x00u, 0xffu, 0xffu, 0x0fu, 0x00u,
        0xffu, 0xffu, 0x0fu, 0x00u, 0xffu, 0xffu, 0x0fu, 0x00u, 0xffu, 0xffu, 0x0fu, 0x00u, 0xffu, 0xffu, 0x0fu, 0x00u,
        0xffu, 0xffu, 0x0fu, 0x00u, 0xffu, 0xffu, 0x0fu, 0x00u, 0xffu, 0xffu, 0x0fu, 0x00u, 0xffu, 0xffu, 0x0fu, 0x00u,
        0xffu, 0xffu, 0x0fu, 0x00u, 0xffu, 0xffu, 0x0fu, 0x00u, 0xffu, 0xffu, 0x0fu, 0x00u, 0xffu, 0xffu, 0x0fu, 0x00u,
        0xffu, 0xffu, 0x0fu, 0x00u, 0xffu, 0xffu, 0x0fu, 0x00u, 0xffu, 0xffu, 0x0fu, 0x00u, 0xffu, 0xffu, 0x0fu, 0x00u,
        0xffu, 0xffu, 0x0fu, 0x00u, 0xffu, 0xffu, 0x0fu, 0x00u, 0xffu, 0xffu, 0x0fu, 0x00u, 0xffu, 0xffu, 0x0fu, 0x00u,
        0xffu, 0xffu, 0x0fu, 0x00u, 0xffu, 0xffu, 0x0fu, 0x00u, 0xffu, 0xffu, 0x0fu, 0x00u, 0xffu, 0xffu, 0x0fu, 0x00u,
        0xffu, 0xffu, 0x0fu, 0x00u, 0xffu, 0xffu, 0x0fu, 0x00u, 0xffu, 0xffu, 0x0fu, 0x00u, 0xffu, 0xffu, 0x0fu, 0x00u,
        0xffu, 0xffu, 0x0fu, 0x00u, 0xffu, 0xffu, 0x0fu, 0x00u, 0xffu, 0xffu, 0x0fu, 0x00u, 0xffu, 0xffu, 0x0fu, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x03u, 0x00u, 0x00u, 0x00u, 0x04u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0xfeu, 0x00u, 0x00u, 0x00u,
        0xffu, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x10u, 0x00u, 0x00u, 0x00u, 0x10u, 0x00u, 0x00u, 0x00u,
        0x10u, 0x00u, 0x00u, 0x00u, 0x06u, 0x00u, 0x00u, 0x00u, 0x0au, 0x00u, 0x00u, 0x00u, 0x0cu, 0x00u, 0x00u, 0x00u,
        0x01u, 0x00u, 0x00u, 0x00u, 0x14u, 0x00u, 0x00u, 0x00u, 0x16u, 0x00u, 0x00u, 0x00u, 0x01u, 0x00u, 0x00u, 0x00u,
        0xffu, 0x00u, 0x00u, 0x00u, 0xffu, 0x00u, 0x00u, 0x00u, 0xffu, 0x00u, 0x00u, 0x00u, 0x06u, 0x00u, 0x00u, 0x00u,
        0x03u, 0x00u, 0x00u, 0x00u, 0x04u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0xfeu, 0x00u, 0x00u, 0x00u,
        0xffu, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x06u, 0x00u, 0x00u, 0x00u, 0x32u, 0x00u, 0x00u, 0x00u, 0x52u, 0x00u, 0x00u, 0x00u,
        0x05u, 0x00u, 0x00u, 0x00u, 0xfeu, 0x00u, 0x00u, 0x00u, 0xffu, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x10u, 0x00u, 0x00u, 0x00u, 0x10u, 0x00u, 0x00u, 0x00u, 0x10u, 0x00u, 0x00u, 0x00u, 0x06u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x10u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u
    };

    static const uint8 step12_li2_hi_combine_mctf[] = {
        0x08u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0xffu, 0xffu, 0xffu, 0xffu, 0xffu, 0xffu, 0xffu, 0xffu, 0xffu, 0xffu, 0xffu, 0xffu, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0xffu, 0xffu, 0xffu, 0x00u, 0x01u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x88u, 0x08u, 0x88u, 0x08u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x01u, 0x01u, 0x01u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x01u, 0x01u, 0x01u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x01u, 0x01u, 0x01u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x01u, 0x01u, 0x01u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x01u, 0x01u, 0x01u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x01u, 0x01u, 0x01u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x01u, 0x01u, 0x01u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x64u, 0x64u, 0x64u, 0x00u,
        0x65u, 0x65u, 0x65u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0xffu, 0xffu, 0xffu, 0x00u, 0xffu, 0xffu, 0xffu, 0x00u,
        0xffu, 0xffu, 0xffu, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0xe6u, 0x00u, 0x00u, 0x00u, 0xeau, 0x00u, 0x00u, 0x00u,
        0x02u, 0x00u, 0x00u, 0x00u, 0xebu, 0x00u, 0x00u, 0x00u, 0xefu, 0x00u, 0x00u, 0x00u, 0x02u, 0x00u, 0x00u, 0x00u,
        0x30u, 0x00u, 0x00u, 0x00u, 0x37u, 0x00u, 0x00u, 0x00u, 0x50u, 0x00u, 0x00u, 0x00u, 0x02u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u
    };
    uint32 rval = IK_OK;
    uintptr tmp_ptr;
    void*  ptr_sec2_cc_in = NULL;
    img_cfg_compose_hiso_step1_cfg(&p_flow_tbl_list->step1, p_bin_data_dram_addr);
    (void)amba_ik_system_memcpy(p_flow_tbl_list->step1.sec18.p_CR_buf_117, &step1_mcts_Edge_Detect, CR_SIZE_117);
    (void)amba_ik_system_memcpy(p_flow_tbl_list->step1.sec18.p_CR_buf_118, &step1_mcts_POS_DEPEND_33x33, CR_SIZE_118);
    //Step2
    img_cfg_compose_hiso_step2_cfg(&p_flow_tbl_list->step2, p_bin_data_dram_addr);
    //#define OFFSET_REG_COLOR_COR_IN_LOOKUP_RED (512u*2u)
    //ptr_sec2_cc_in = p_flow_tbl->step2.sec2.CR_buf_26 + (OFFSET_REG_COLOR_COR_IN_LOOKUP_RED << 1U);  -->2048
    (void)amba_ik_system_memcpy(&tmp_ptr, &p_flow_tbl_list->step2.sec2.p_CR_buf_26, sizeof(uintptr));
    tmp_ptr += 2048U;
    (void)amba_ik_system_memcpy(&ptr_sec2_cc_in, &tmp_ptr, sizeof(void*));
    (void)amba_ik_system_memcpy(ptr_sec2_cc_in, &step2_COLOR_COR_IN_LOOKUP, 2816);//CR_SIZE_26 - 2048(offset) --> 2816 (COLOR_COR_IN_LOOKUP size)
    (void)amba_ik_system_memcpy(p_flow_tbl_list->step2.sec18.p_CR_buf_117, &step2_mcts_Edge_Detect, CR_SIZE_117);
    (void)amba_ik_system_memcpy(p_flow_tbl_list->step2.sec18.p_CR_buf_118, &step1_mcts_POS_DEPEND_33x33, CR_SIZE_118);
    //Step3
    img_cfg_compose_hiso_step3_cfg(&p_flow_tbl_list->step3, p_bin_data_dram_addr);
    (void)amba_ik_system_memcpy(p_flow_tbl_list->step3.sec18.p_CR_buf_117, &step3_bypass_mcts, CR_SIZE_117);
    (void)amba_ik_system_memcpy(p_flow_tbl_list->step3.sec18.p_CR_buf_118, &step1_mcts_POS_DEPEND_33x33, CR_SIZE_118);
    //Step4
    img_cfg_compose_hiso_step3_cfg(&p_flow_tbl_list->step4, p_bin_data_dram_addr);
    (void)amba_ik_system_memcpy(p_flow_tbl_list->step4.sec18.p_CR_buf_117, &step4_bypass_mcts, CR_SIZE_117);
    (void)amba_ik_system_memcpy(p_flow_tbl_list->step4.sec18.p_CR_buf_118, &step1_mcts_POS_DEPEND_33x33, CR_SIZE_118);
    //Step4a
    img_cfg_compose_hiso_step4a_cfg(&p_flow_tbl_list->step4a, p_bin_data_dram_addr);
    //Step5
    img_cfg_compose_hiso_step5_cfg(&p_flow_tbl_list->step5, p_bin_data_dram_addr);
    (void)amba_ik_system_memcpy(p_flow_tbl_list->step5.sec18.p_CR_buf_117, &step5_bypass_mcts, CR_SIZE_117);
    (void)amba_ik_system_memcpy(p_flow_tbl_list->step5.sec18.p_CR_buf_112, &passthrough_mctfa, CR_SIZE_112);
    //Step6
    img_cfg_compose_hiso_step6_cfg(&p_flow_tbl_list->step6, p_bin_data_dram_addr);
    (void)amba_ik_system_memcpy(p_flow_tbl_list->step6.sec18.p_CR_buf_112, &passthrough_mctfa, CR_SIZE_112);
    //Step7
    img_cfg_compose_hiso_step6_cfg(&p_flow_tbl_list->step7, p_bin_data_dram_addr);
    (void)amba_ik_system_memcpy(p_flow_tbl_list->step7.sec18.p_CR_buf_112, &passthrough_mctfa, CR_SIZE_112);
    //Step8
    img_cfg_compose_hiso_step6_cfg(&p_flow_tbl_list->step8, p_bin_data_dram_addr);
    (void)amba_ik_system_memcpy(p_flow_tbl_list->step8.sec18.p_CR_buf_112, &passthrough_mctfa, CR_SIZE_112);
    //Step9
    img_cfg_compose_hiso_step4a_cfg(&p_flow_tbl_list->step9, p_bin_data_dram_addr);
    //Step10
    img_cfg_compose_hiso_step5_cfg(&p_flow_tbl_list->step10, p_bin_data_dram_addr);
    (void)amba_ik_system_memcpy(p_flow_tbl_list->step10.sec2.p_CR_buf_30, &step10_shpa, CR_SIZE_30);
    //Step11
    img_cfg_compose_hiso_step11_cfg(&p_flow_tbl_list->step11, p_bin_data_dram_addr);
    (void)amba_ik_system_memcpy(p_flow_tbl_list->step11.sec18.p_CR_buf_112, &passthrough_mctfa, CR_SIZE_112);
    (void)amba_ik_system_memcpy(p_flow_tbl_list->step11.sec18.p_CR_buf_117, &passthrough_shpb, CR_SIZE_117);
    (void)amba_ik_system_memcpy(p_flow_tbl_list->step11.sec18.p_CR_buf_118, &step1_mcts_POS_DEPEND_33x33, CR_SIZE_118);
    //Step12
    img_cfg_compose_hiso_step11_cfg(&p_flow_tbl_list->step12, p_bin_data_dram_addr);
    (void)amba_ik_system_memcpy(p_flow_tbl_list->step12.sec18.p_CR_buf_117, &step12_li2_hi_combine_mcts_disable, CR_SIZE_117);
    (void)amba_ik_system_memcpy(p_flow_tbl_list->step12.sec18.p_CR_buf_112, &step12_li2_hi_combine_mctf, CR_SIZE_112);
    (void)amba_ik_system_memcpy(p_flow_tbl_list->step12.sec18.p_CR_buf_118, &step1_mcts_POS_DEPEND_33x33, CR_SIZE_118);
    //Step13
    img_cfg_compose_hiso_step13_cfg(&p_flow_tbl_list->step13, p_bin_data_dram_addr);
    (void)amba_ik_system_memcpy(p_flow_tbl_list->step13.sec18.p_CR_buf_112, &passthrough_mctfa, CR_SIZE_112);
    (void)amba_ik_system_memcpy(p_flow_tbl_list->step13.sec18.p_CR_buf_117, &passthrough_shpb, CR_SIZE_117);
#ifndef _HISO_SEC3_IN_STEP13
    //Step14
    img_cfg_compose_hiso_step14_cfg(&p_flow_tbl_list->step14, p_bin_data_dram_addr);
    (void)amba_ik_system_memcpy(p_flow_tbl_list->step14.sec18.p_CR_buf_117, &passthrough_shpb, CR_SIZE_117);
    (void)amba_ik_system_memcpy(p_flow_tbl_list->step14.sec18.p_CR_buf_112, &passthrough_mctfa, CR_SIZE_112);
#endif

    return rval;
}

static void img_cfg_compose_motion_fusion_step1_cfg(const amba_ik_r2y_flow_tables_list_t *p_flow_tbl_list, const void *p_bin_data_dram_addr)
{
    void* tmp_ptr;

    (void)amba_ik_system_memcpy(&tmp_ptr, &p_flow_tbl_list->p_CR_buf_4, sizeof(void*));
    (void)img_cfg_compose_sec2_cfg(tmp_ptr, p_bin_data_dram_addr);

    (void)amba_ik_system_memcpy(&tmp_ptr, &p_flow_tbl_list->p_CR_buf_42, sizeof(void*));
    (void)img_cfg_compose_sec3_cfg(tmp_ptr, p_bin_data_dram_addr);

    (void)amba_ik_system_memcpy(&tmp_ptr, &p_flow_tbl_list->p_CR_buf_45, sizeof(void*));
    (void)img_cfg_compose_sec4_cfg(tmp_ptr, p_bin_data_dram_addr);

    (void)amba_ik_system_memcpy(&tmp_ptr, &p_flow_tbl_list->p_CR_buf_100, sizeof(void*));
    (void)img_cfg_compose_sec11_cfg(tmp_ptr, p_bin_data_dram_addr);

    (void)amba_ik_system_memcpy(&tmp_ptr, &p_flow_tbl_list->p_CR_buf_111, sizeof(void*));
    (void)img_cfg_compose_sec18_cfg(tmp_ptr, p_bin_data_dram_addr);
}

static void img_cfg_compose_motion_fusion_step2_cfg(const amba_ik_motion_me1_flow_tables_list_t *p_flow_tbl_list, const void *p_bin_data_dram_addr)
{
    void* tmp_ptr;

    (void)amba_ik_system_memcpy(&tmp_ptr, &p_flow_tbl_list->p_CR_buf_4, sizeof(void*));
    (void)img_cfg_compose_sec2_cfg(tmp_ptr, p_bin_data_dram_addr);

    (void)amba_ik_system_memcpy(&tmp_ptr, &p_flow_tbl_list->p_CR_buf_42, sizeof(void*));
    (void)img_cfg_compose_sec3_cfg(tmp_ptr, p_bin_data_dram_addr);

    (void)amba_ik_system_memcpy(&tmp_ptr, &p_flow_tbl_list->p_CR_buf_111, sizeof(void*));
    (void)img_cfg_compose_sec18_cfg(tmp_ptr, p_bin_data_dram_addr);
}

static void img_cfg_compose_motion_fusion_step3_cfg(const amba_ik_motion_a_flow_tables_list_t *p_flow_tbl_list, const void *p_bin_data_dram_addr)
{
    void* tmp_ptr;

    (void)amba_ik_system_memcpy(&tmp_ptr, &p_flow_tbl_list->p_CR_buf_4, sizeof(void*));
    (void)img_cfg_compose_sec2_cfg(tmp_ptr, p_bin_data_dram_addr);

    (void)amba_ik_system_memcpy(&tmp_ptr, &p_flow_tbl_list->p_CR_buf_111, sizeof(void*));
    (void)img_cfg_compose_sec18_cfg(tmp_ptr, p_bin_data_dram_addr);
}


static void img_cfg_compose_motion_fusion_step4_cfg(const amba_ik_motion_b_flow_tables_list_t *p_flow_tbl_list, const void *p_bin_data_dram_addr)
{
    void* tmp_ptr;

    (void)amba_ik_system_memcpy(&tmp_ptr, &p_flow_tbl_list->p_CR_buf_4, sizeof(void*));
    (void)img_cfg_compose_sec2_cfg(tmp_ptr, p_bin_data_dram_addr);

    (void)amba_ik_system_memcpy(&tmp_ptr, &p_flow_tbl_list->p_CR_buf_111, sizeof(void*));
    (void)img_cfg_compose_sec18_cfg(tmp_ptr, p_bin_data_dram_addr);
}

static void img_cfg_compose_motion_fusion_step5_cfg(const amba_ik_motion_c_flow_tables_list_t *p_flow_tbl_list, const void *p_bin_data_dram_addr)
{
    void* tmp_ptr;

    (void)amba_ik_system_memcpy(&tmp_ptr, &p_flow_tbl_list->p_CR_buf_4, sizeof(void*));
    (void)img_cfg_compose_sec2_cfg(tmp_ptr, p_bin_data_dram_addr);

    (void)amba_ik_system_memcpy(&tmp_ptr, &p_flow_tbl_list->p_CR_buf_111, sizeof(void*));
    (void)img_cfg_compose_sec18_cfg(tmp_ptr, p_bin_data_dram_addr);
}
#if SUPPORT_FUSION
static void img_cfg_compose_mono1_8_cfg(amba_ik_r2y_flow_tables_t *p_flow_tbl, const void *p_bin_data_dram_addr)
{
    void* tmp_ptr;

    (void)amba_ik_system_memcpy(&tmp_ptr, &p_flow_tbl->CR_buf_4, sizeof(void*));
    (void)img_cfg_compose_sec2_cfg(tmp_ptr, p_bin_data_dram_addr);

    (void)amba_ik_system_memcpy(&tmp_ptr, &p_flow_tbl->CR_buf_42, sizeof(void*));
    (void)img_cfg_compose_sec3_cfg(tmp_ptr, p_bin_data_dram_addr);

    (void)amba_ik_system_memcpy(&tmp_ptr, &p_flow_tbl->CR_buf_45, sizeof(void*));
    (void)img_cfg_compose_sec4_cfg(tmp_ptr, p_bin_data_dram_addr);

    (void)amba_ik_system_memcpy(&tmp_ptr, &p_flow_tbl->CR_buf_100, sizeof(void*));
    (void)img_cfg_compose_sec11_cfg(tmp_ptr, p_bin_data_dram_addr);

    (void)amba_ik_system_memcpy(&tmp_ptr, &p_flow_tbl->CR_buf_111, sizeof(void*));
    (void)img_cfg_compose_sec18_cfg(tmp_ptr, p_bin_data_dram_addr);
}

static void img_cfg_compose_mono3_cfg(amba_ik_mono3_flow_tables_t *p_flow_tbl, const void *p_bin_data_dram_addr)
{
    void* tmp_ptr;

    (void)amba_ik_system_memcpy(&tmp_ptr, &p_flow_tbl->CR_buf_4, sizeof(void*));
    (void)img_cfg_compose_sec2_cfg(tmp_ptr, p_bin_data_dram_addr);

    (void)amba_ik_system_memcpy(&tmp_ptr, &p_flow_tbl->CR_buf_111, sizeof(void*));
    (void)img_cfg_compose_sec18_cfg(tmp_ptr, p_bin_data_dram_addr);
}

static void img_cfg_compose_mono4_cfg(amba_ik_mono4_flow_tables_t *p_flow_tbl, const void *p_bin_data_dram_addr)
{
    void* tmp_ptr;

    (void)amba_ik_system_memcpy(&tmp_ptr, &p_flow_tbl->CR_buf_4, sizeof(void*));
    (void)img_cfg_compose_sec2_cfg(tmp_ptr, p_bin_data_dram_addr);
}

static void img_cfg_compose_mono5_cfg(amba_ik_mono5_flow_tables_t *p_flow_tbl, const void *p_bin_data_dram_addr)
{
    void* tmp_ptr;

    (void)amba_ik_system_memcpy(&tmp_ptr, &p_flow_tbl->CR_buf_4, sizeof(void*));
    (void)img_cfg_compose_sec2_cfg(tmp_ptr, p_bin_data_dram_addr);
}

static void img_cfg_compose_mono6_cfg(amba_ik_mono6_flow_tables_t *p_flow_tbl, const void *p_bin_data_dram_addr)
{
    void* tmp_ptr;

    (void)amba_ik_system_memcpy(&tmp_ptr, &p_flow_tbl->CR_buf_4, sizeof(void*));
    (void)img_cfg_compose_sec2_cfg(tmp_ptr, p_bin_data_dram_addr);

    (void)amba_ik_system_memcpy(&tmp_ptr, &p_flow_tbl->CR_buf_111, sizeof(void*));
    (void)img_cfg_compose_sec18_cfg(tmp_ptr, p_bin_data_dram_addr);
}

static void img_cfg_compose_mono7_cfg(amba_ik_mono7_flow_tables_t *p_flow_tbl, const void *p_bin_data_dram_addr)
{
    void* tmp_ptr;

    (void)amba_ik_system_memcpy(&tmp_ptr, &p_flow_tbl->CR_buf_4, sizeof(void*));
    (void)img_cfg_compose_sec2_cfg(tmp_ptr, p_bin_data_dram_addr);

    (void)amba_ik_system_memcpy(&tmp_ptr, &p_flow_tbl->CR_buf_111, sizeof(void*));
    (void)img_cfg_compose_sec18_cfg(tmp_ptr, p_bin_data_dram_addr);
}
#endif

static uint32 img_cfg_compose_motion_cfg(amba_ik_motion_fusion_flow_tables_t *p_motion_fusion_flow_tbl, const amba_ik_motion_fusion_flow_tables_list_t *p_motion_fusion_flow_tbl_list,const void *p_bin_data_dram_addr)
{
    uint32 rval = IK_OK;

    img_cfg_compose_motion_fusion_step1_cfg(&p_motion_fusion_flow_tbl_list->r2y, p_bin_data_dram_addr);
    img_cfg_compose_motion_fusion_step2_cfg(&p_motion_fusion_flow_tbl_list->motion_me1, p_bin_data_dram_addr);
    img_cfg_compose_motion_fusion_step3_cfg(&p_motion_fusion_flow_tbl_list->motion_a, p_bin_data_dram_addr);
    img_cfg_compose_motion_fusion_step4_cfg(&p_motion_fusion_flow_tbl_list->motion_b, p_bin_data_dram_addr);
    img_cfg_compose_motion_fusion_step5_cfg(&p_motion_fusion_flow_tbl_list->motion_c, p_bin_data_dram_addr);
#if SUPPORT_FUSION
    img_cfg_compose_mono1_8_cfg(&p_motion_fusion_flow_tbl->mono1, p_bin_data_dram_addr);
    img_cfg_compose_mono3_cfg(&p_motion_fusion_flow_tbl->mono3, p_bin_data_dram_addr);
    img_cfg_compose_mono4_cfg(&p_motion_fusion_flow_tbl->mono4, p_bin_data_dram_addr);
    img_cfg_compose_mono5_cfg(&p_motion_fusion_flow_tbl->mono5, p_bin_data_dram_addr);
    img_cfg_compose_mono6_cfg(&p_motion_fusion_flow_tbl->mono6, p_bin_data_dram_addr);
    img_cfg_compose_mono7_cfg(&p_motion_fusion_flow_tbl->mono7, p_bin_data_dram_addr);
    img_cfg_compose_mono1_8_cfg(&p_motion_fusion_flow_tbl->mono8, p_bin_data_dram_addr);
#endif
    (void)amba_ik_system_memset(p_motion_fusion_flow_tbl->ca_warp_hor_red, 0x0, sizeof(p_motion_fusion_flow_tbl->ca_warp_hor_red));
    (void)amba_ik_system_memset(p_motion_fusion_flow_tbl->ca_warp_hor_blue, 0x0, sizeof(p_motion_fusion_flow_tbl->ca_warp_hor_blue));
    (void)amba_ik_system_memset(p_motion_fusion_flow_tbl->ca_warp_ver_red, 0x0, sizeof(p_motion_fusion_flow_tbl->ca_warp_ver_red));
    (void)amba_ik_system_memset(p_motion_fusion_flow_tbl->ca_warp_ver_blue, 0x0, sizeof(p_motion_fusion_flow_tbl->ca_warp_ver_blue));
    (void)amba_ik_system_memset(p_motion_fusion_flow_tbl->warp_hor, 0x0, sizeof(p_motion_fusion_flow_tbl->warp_hor));
    (void)amba_ik_system_memset(p_motion_fusion_flow_tbl->warp_ver, 0x0, sizeof(p_motion_fusion_flow_tbl->warp_ver));
    (void)amba_ik_system_memset(p_motion_fusion_flow_tbl->warp_hor_b, 0x0, sizeof(p_motion_fusion_flow_tbl->warp_hor_b));
    (void)amba_ik_system_memset(p_motion_fusion_flow_tbl->aaa, 0x0, sizeof(p_motion_fusion_flow_tbl->aaa));

    return rval;
}

static void img_cfg_update_flow_tbl_list(const int64 addr_offset, amba_ik_flow_tables_list_t *p_flow_tbl_list)
{
    p_flow_tbl_list->r2y.p_CR_buf_4  = ik_address_add_offset(p_flow_tbl_list->r2y.p_CR_buf_4, addr_offset);
    p_flow_tbl_list->r2y.p_CR_buf_5  = ik_address_add_offset(p_flow_tbl_list->r2y.p_CR_buf_5, addr_offset);
    p_flow_tbl_list->r2y.p_CR_buf_6  = ik_address_add_offset(p_flow_tbl_list->r2y.p_CR_buf_6, addr_offset);
    p_flow_tbl_list->r2y.p_CR_buf_7  = ik_address_add_offset(p_flow_tbl_list->r2y.p_CR_buf_7, addr_offset);
    p_flow_tbl_list->r2y.p_CR_buf_8  = ik_address_add_offset(p_flow_tbl_list->r2y.p_CR_buf_8, addr_offset);
    p_flow_tbl_list->r2y.p_CR_buf_9  = ik_address_add_offset(p_flow_tbl_list->r2y.p_CR_buf_9, addr_offset);
    p_flow_tbl_list->r2y.p_CR_buf_10 = ik_address_add_offset(p_flow_tbl_list->r2y.p_CR_buf_10, addr_offset);
    p_flow_tbl_list->r2y.p_CR_buf_11 = ik_address_add_offset(p_flow_tbl_list->r2y.p_CR_buf_11, addr_offset);
    p_flow_tbl_list->r2y.p_CR_buf_12 = ik_address_add_offset(p_flow_tbl_list->r2y.p_CR_buf_12, addr_offset);
    p_flow_tbl_list->r2y.p_CR_buf_13 = ik_address_add_offset(p_flow_tbl_list->r2y.p_CR_buf_13, addr_offset);
    p_flow_tbl_list->r2y.p_CR_buf_14 = ik_address_add_offset(p_flow_tbl_list->r2y.p_CR_buf_14, addr_offset);
    p_flow_tbl_list->r2y.p_CR_buf_15 = ik_address_add_offset(p_flow_tbl_list->r2y.p_CR_buf_15, addr_offset);
    p_flow_tbl_list->r2y.p_CR_buf_16 = ik_address_add_offset(p_flow_tbl_list->r2y.p_CR_buf_16, addr_offset);
    p_flow_tbl_list->r2y.p_CR_buf_17 = ik_address_add_offset(p_flow_tbl_list->r2y.p_CR_buf_17, addr_offset);
    p_flow_tbl_list->r2y.p_CR_buf_18 = ik_address_add_offset(p_flow_tbl_list->r2y.p_CR_buf_18, addr_offset);
    p_flow_tbl_list->r2y.p_CR_buf_19 = ik_address_add_offset(p_flow_tbl_list->r2y.p_CR_buf_19, addr_offset);
    p_flow_tbl_list->r2y.p_CR_buf_20 = ik_address_add_offset(p_flow_tbl_list->r2y.p_CR_buf_20, addr_offset);
    p_flow_tbl_list->r2y.p_CR_buf_21 = ik_address_add_offset(p_flow_tbl_list->r2y.p_CR_buf_21, addr_offset);
    p_flow_tbl_list->r2y.p_CR_buf_22 = ik_address_add_offset(p_flow_tbl_list->r2y.p_CR_buf_22, addr_offset);
    p_flow_tbl_list->r2y.p_CR_buf_23 = ik_address_add_offset(p_flow_tbl_list->r2y.p_CR_buf_23, addr_offset);
    p_flow_tbl_list->r2y.p_CR_buf_24 = ik_address_add_offset(p_flow_tbl_list->r2y.p_CR_buf_24, addr_offset);
    p_flow_tbl_list->r2y.p_CR_buf_25 = ik_address_add_offset(p_flow_tbl_list->r2y.p_CR_buf_25, addr_offset);
    p_flow_tbl_list->r2y.p_CR_buf_26 = ik_address_add_offset(p_flow_tbl_list->r2y.p_CR_buf_26, addr_offset);
    p_flow_tbl_list->r2y.p_CR_buf_27 = ik_address_add_offset(p_flow_tbl_list->r2y.p_CR_buf_27, addr_offset);
    p_flow_tbl_list->r2y.p_CR_buf_28 = ik_address_add_offset(p_flow_tbl_list->r2y.p_CR_buf_28, addr_offset);
    p_flow_tbl_list->r2y.p_CR_buf_29 = ik_address_add_offset(p_flow_tbl_list->r2y.p_CR_buf_29, addr_offset);
    p_flow_tbl_list->r2y.p_CR_buf_30 = ik_address_add_offset(p_flow_tbl_list->r2y.p_CR_buf_30, addr_offset);
    p_flow_tbl_list->r2y.p_CR_buf_31 = ik_address_add_offset(p_flow_tbl_list->r2y.p_CR_buf_31, addr_offset);
    p_flow_tbl_list->r2y.p_CR_buf_32 = ik_address_add_offset(p_flow_tbl_list->r2y.p_CR_buf_32, addr_offset);
    p_flow_tbl_list->r2y.p_CR_buf_33 = ik_address_add_offset(p_flow_tbl_list->r2y.p_CR_buf_33, addr_offset);
    p_flow_tbl_list->r2y.p_CR_buf_34 = ik_address_add_offset(p_flow_tbl_list->r2y.p_CR_buf_34, addr_offset);
    p_flow_tbl_list->r2y.p_CR_buf_35 = ik_address_add_offset(p_flow_tbl_list->r2y.p_CR_buf_35, addr_offset);
    p_flow_tbl_list->r2y.p_CR_buf_36 = ik_address_add_offset(p_flow_tbl_list->r2y.p_CR_buf_36, addr_offset);
    p_flow_tbl_list->r2y.p_CR_buf_37 = ik_address_add_offset(p_flow_tbl_list->r2y.p_CR_buf_37, addr_offset);
    p_flow_tbl_list->r2y.p_CR_buf_38 = ik_address_add_offset(p_flow_tbl_list->r2y.p_CR_buf_38, addr_offset);
    p_flow_tbl_list->r2y.p_CR_buf_39 = ik_address_add_offset(p_flow_tbl_list->r2y.p_CR_buf_39, addr_offset);
    p_flow_tbl_list->r2y.p_CR_buf_40 = ik_address_add_offset(p_flow_tbl_list->r2y.p_CR_buf_40, addr_offset);
    p_flow_tbl_list->r2y.p_CR_buf_41 = ik_address_add_offset(p_flow_tbl_list->r2y.p_CR_buf_41, addr_offset);
    p_flow_tbl_list->r2y.p_CR_buf_42 = ik_address_add_offset(p_flow_tbl_list->r2y.p_CR_buf_42, addr_offset);
    p_flow_tbl_list->r2y.p_CR_buf_43 = ik_address_add_offset(p_flow_tbl_list->r2y.p_CR_buf_43, addr_offset);
    p_flow_tbl_list->r2y.p_CR_buf_44 = ik_address_add_offset(p_flow_tbl_list->r2y.p_CR_buf_44, addr_offset);
    p_flow_tbl_list->r2y.p_CR_buf_45 = ik_address_add_offset(p_flow_tbl_list->r2y.p_CR_buf_45, addr_offset);
    p_flow_tbl_list->r2y.p_CR_buf_46 = ik_address_add_offset(p_flow_tbl_list->r2y.p_CR_buf_46, addr_offset);
    p_flow_tbl_list->r2y.p_CR_buf_47 = ik_address_add_offset(p_flow_tbl_list->r2y.p_CR_buf_47, addr_offset);
    p_flow_tbl_list->r2y.p_CR_buf_48 = ik_address_add_offset(p_flow_tbl_list->r2y.p_CR_buf_48, addr_offset);
    p_flow_tbl_list->r2y.p_CR_buf_49 = ik_address_add_offset(p_flow_tbl_list->r2y.p_CR_buf_49, addr_offset);
    p_flow_tbl_list->r2y.p_CR_buf_50 = ik_address_add_offset(p_flow_tbl_list->r2y.p_CR_buf_50, addr_offset);
    p_flow_tbl_list->r2y.p_CR_buf_51 = ik_address_add_offset(p_flow_tbl_list->r2y.p_CR_buf_51, addr_offset);
    p_flow_tbl_list->r2y.p_CR_buf_52 = ik_address_add_offset(p_flow_tbl_list->r2y.p_CR_buf_52, addr_offset);

    p_flow_tbl_list->r2y.p_CR_buf_100= ik_address_add_offset(p_flow_tbl_list->r2y.p_CR_buf_100, addr_offset);
    p_flow_tbl_list->r2y.p_CR_buf_101= ik_address_add_offset(p_flow_tbl_list->r2y.p_CR_buf_101, addr_offset);
    p_flow_tbl_list->r2y.p_CR_buf_102= ik_address_add_offset(p_flow_tbl_list->r2y.p_CR_buf_102, addr_offset);
    p_flow_tbl_list->r2y.p_CR_buf_103= ik_address_add_offset(p_flow_tbl_list->r2y.p_CR_buf_103, addr_offset);
    p_flow_tbl_list->r2y.p_CR_buf_111= ik_address_add_offset(p_flow_tbl_list->r2y.p_CR_buf_111, addr_offset);
    p_flow_tbl_list->r2y.p_CR_buf_112= ik_address_add_offset(p_flow_tbl_list->r2y.p_CR_buf_112, addr_offset);
    p_flow_tbl_list->r2y.p_CR_buf_113= ik_address_add_offset(p_flow_tbl_list->r2y.p_CR_buf_113, addr_offset);
    p_flow_tbl_list->r2y.p_CR_buf_114= ik_address_add_offset(p_flow_tbl_list->r2y.p_CR_buf_114, addr_offset);
    p_flow_tbl_list->r2y.p_CR_buf_115= ik_address_add_offset(p_flow_tbl_list->r2y.p_CR_buf_115, addr_offset);
    p_flow_tbl_list->r2y.p_CR_buf_116= ik_address_add_offset(p_flow_tbl_list->r2y.p_CR_buf_116, addr_offset);
    p_flow_tbl_list->r2y.p_CR_buf_117= ik_address_add_offset(p_flow_tbl_list->r2y.p_CR_buf_117, addr_offset);
    p_flow_tbl_list->r2y.p_CR_buf_118= ik_address_add_offset(p_flow_tbl_list->r2y.p_CR_buf_118, addr_offset);
    p_flow_tbl_list->r2y.p_CR_buf_119= ik_address_add_offset(p_flow_tbl_list->r2y.p_CR_buf_119, addr_offset);
    p_flow_tbl_list->r2y.p_CR_buf_120= ik_address_add_offset(p_flow_tbl_list->r2y.p_CR_buf_120, addr_offset);
    p_flow_tbl_list->r2y.p_CR_buf_121= ik_address_add_offset(p_flow_tbl_list->r2y.p_CR_buf_121, addr_offset);

    p_flow_tbl_list->p_ca_warp_hor_red = ik_address_add_offset(p_flow_tbl_list->p_ca_warp_hor_red,   addr_offset);
    p_flow_tbl_list->p_ca_warp_hor_blue = ik_address_add_offset(p_flow_tbl_list->p_ca_warp_hor_blue,  addr_offset);
    p_flow_tbl_list->p_ca_warp_ver_red = ik_address_add_offset(p_flow_tbl_list->p_ca_warp_ver_red,  addr_offset);
    p_flow_tbl_list->p_ca_warp_ver_blue = ik_address_add_offset(p_flow_tbl_list->p_ca_warp_ver_blue,  addr_offset);
    p_flow_tbl_list->p_warp_hor = ik_address_add_offset(p_flow_tbl_list->p_warp_hor,  addr_offset);
    p_flow_tbl_list->p_warp_ver = ik_address_add_offset(p_flow_tbl_list->p_warp_ver,   addr_offset);
    p_flow_tbl_list->p_aaa = ik_address_add_offset(p_flow_tbl_list->p_aaa,  addr_offset);
    p_flow_tbl_list->p_warp_hor_b = ik_address_add_offset(p_flow_tbl_list->p_warp_hor_b,  addr_offset);
    p_flow_tbl_list->p_extra_window = ik_address_add_offset(p_flow_tbl_list->p_extra_window,  addr_offset);
    p_flow_tbl_list->p_frame_info = ik_address_add_offset(p_flow_tbl_list->p_frame_info,  addr_offset);

}

static uint32 img_cfg_populate_flow_tbl_list(const amba_ik_flow_tables_t *p_flow_tbl, amba_ik_flow_tables_list_t *p_flow_tbl_list)
{
    uint32 rval = IK_OK;
    const uint8 *tmp_p_u8;
    const int16 *tmp_p_s16;
    const idsp_aaa_data_t *tmp_p_aaa;
    const idsp_extra_window_info_t *tmp_p_extra_win;
    const ik_query_frame_info_t *tmp_p_frame_info;

    tmp_p_u8 = p_flow_tbl->r2y.CR_buf_4;
    (void)amba_ik_system_memcpy(&p_flow_tbl_list->r2y.p_CR_buf_4, &tmp_p_u8, sizeof(void*));
    tmp_p_u8 = p_flow_tbl->r2y.CR_buf_5;
    (void)amba_ik_system_memcpy(&p_flow_tbl_list->r2y.p_CR_buf_5, &tmp_p_u8, sizeof(void*));
    tmp_p_u8 = p_flow_tbl->r2y.CR_buf_6;
    (void)amba_ik_system_memcpy(&p_flow_tbl_list->r2y.p_CR_buf_6, &tmp_p_u8, sizeof(void*));
    tmp_p_u8 = p_flow_tbl->r2y.CR_buf_7;
    (void)amba_ik_system_memcpy(&p_flow_tbl_list->r2y.p_CR_buf_7, &tmp_p_u8, sizeof(void*));
    tmp_p_u8 = p_flow_tbl->r2y.CR_buf_8;
    (void)amba_ik_system_memcpy(&p_flow_tbl_list->r2y.p_CR_buf_8, &tmp_p_u8, sizeof(void*));
    tmp_p_u8 = p_flow_tbl->r2y.CR_buf_9;
    (void)amba_ik_system_memcpy(&p_flow_tbl_list->r2y.p_CR_buf_9, &tmp_p_u8, sizeof(void*));
    tmp_p_u8 = p_flow_tbl->r2y.CR_buf_10;
    (void)amba_ik_system_memcpy(&p_flow_tbl_list->r2y.p_CR_buf_10, &tmp_p_u8, sizeof(void*));
    tmp_p_u8 = p_flow_tbl->r2y.CR_buf_11;
    (void)amba_ik_system_memcpy(&p_flow_tbl_list->r2y.p_CR_buf_11, &tmp_p_u8, sizeof(void*));
    tmp_p_u8 = p_flow_tbl->r2y.CR_buf_12;
    (void)amba_ik_system_memcpy(&p_flow_tbl_list->r2y.p_CR_buf_12, &tmp_p_u8, sizeof(void*));
    tmp_p_u8 = p_flow_tbl->r2y.CR_buf_13;
    (void)amba_ik_system_memcpy(&p_flow_tbl_list->r2y.p_CR_buf_13, &tmp_p_u8, sizeof(void*));
    tmp_p_u8 = p_flow_tbl->r2y.CR_buf_14;
    (void)amba_ik_system_memcpy(&p_flow_tbl_list->r2y.p_CR_buf_14, &tmp_p_u8, sizeof(void*));
    tmp_p_u8 = p_flow_tbl->r2y.CR_buf_15;
    (void)amba_ik_system_memcpy(&p_flow_tbl_list->r2y.p_CR_buf_15, &tmp_p_u8, sizeof(void*));
    tmp_p_u8 = p_flow_tbl->r2y.CR_buf_16;
    (void)amba_ik_system_memcpy(&p_flow_tbl_list->r2y.p_CR_buf_16, &tmp_p_u8, sizeof(void*));
    tmp_p_u8 = p_flow_tbl->r2y.CR_buf_17;
    (void)amba_ik_system_memcpy(&p_flow_tbl_list->r2y.p_CR_buf_17, &tmp_p_u8, sizeof(void*));
    tmp_p_u8 = p_flow_tbl->r2y.CR_buf_18;
    (void)amba_ik_system_memcpy(&p_flow_tbl_list->r2y.p_CR_buf_18, &tmp_p_u8, sizeof(void*));
    tmp_p_u8 = p_flow_tbl->r2y.CR_buf_19;
    (void)amba_ik_system_memcpy(&p_flow_tbl_list->r2y.p_CR_buf_19, &tmp_p_u8, sizeof(void*));
    tmp_p_u8 = p_flow_tbl->r2y.CR_buf_20;
    (void)amba_ik_system_memcpy(&p_flow_tbl_list->r2y.p_CR_buf_20, &tmp_p_u8, sizeof(void*));
    tmp_p_u8 = p_flow_tbl->r2y.CR_buf_21;
    (void)amba_ik_system_memcpy(&p_flow_tbl_list->r2y.p_CR_buf_21, &tmp_p_u8, sizeof(void*));
    tmp_p_u8 = p_flow_tbl->r2y.CR_buf_22;
    (void)amba_ik_system_memcpy(&p_flow_tbl_list->r2y.p_CR_buf_22, &tmp_p_u8, sizeof(void*));
    tmp_p_u8 = p_flow_tbl->r2y.CR_buf_23;
    (void)amba_ik_system_memcpy(&p_flow_tbl_list->r2y.p_CR_buf_23, &tmp_p_u8, sizeof(void*));
    tmp_p_u8 = p_flow_tbl->r2y.CR_buf_24;
    (void)amba_ik_system_memcpy(&p_flow_tbl_list->r2y.p_CR_buf_24, &tmp_p_u8, sizeof(void*));
    tmp_p_u8 = p_flow_tbl->r2y.CR_buf_25;
    (void)amba_ik_system_memcpy(&p_flow_tbl_list->r2y.p_CR_buf_25, &tmp_p_u8, sizeof(void*));
    tmp_p_u8 = p_flow_tbl->r2y.CR_buf_26;
    (void)amba_ik_system_memcpy(&p_flow_tbl_list->r2y.p_CR_buf_26, &tmp_p_u8, sizeof(void*));
    tmp_p_u8 = p_flow_tbl->r2y.CR_buf_27;
    (void)amba_ik_system_memcpy(&p_flow_tbl_list->r2y.p_CR_buf_27, &tmp_p_u8, sizeof(void*));
    tmp_p_u8 = p_flow_tbl->r2y.CR_buf_28;
    (void)amba_ik_system_memcpy(&p_flow_tbl_list->r2y.p_CR_buf_28, &tmp_p_u8, sizeof(void*));
    tmp_p_u8 = p_flow_tbl->r2y.CR_buf_29;
    (void)amba_ik_system_memcpy(&p_flow_tbl_list->r2y.p_CR_buf_29, &tmp_p_u8, sizeof(void*));
    tmp_p_u8 = p_flow_tbl->r2y.CR_buf_30;
    (void)amba_ik_system_memcpy(&p_flow_tbl_list->r2y.p_CR_buf_30, &tmp_p_u8, sizeof(void*));
    tmp_p_u8 = p_flow_tbl->r2y.CR_buf_31;
    (void)amba_ik_system_memcpy(&p_flow_tbl_list->r2y.p_CR_buf_31, &tmp_p_u8, sizeof(void*));
    tmp_p_u8 = p_flow_tbl->r2y.CR_buf_32;
    (void)amba_ik_system_memcpy(&p_flow_tbl_list->r2y.p_CR_buf_32, &tmp_p_u8, sizeof(void*));
    tmp_p_u8 = p_flow_tbl->r2y.CR_buf_33;
    (void)amba_ik_system_memcpy(&p_flow_tbl_list->r2y.p_CR_buf_33, &tmp_p_u8, sizeof(void*));
    tmp_p_u8 = p_flow_tbl->r2y.CR_buf_34;
    (void)amba_ik_system_memcpy(&p_flow_tbl_list->r2y.p_CR_buf_34, &tmp_p_u8, sizeof(void*));
    tmp_p_u8 = p_flow_tbl->r2y.CR_buf_35;
    (void)amba_ik_system_memcpy(&p_flow_tbl_list->r2y.p_CR_buf_35, &tmp_p_u8, sizeof(void*));
    tmp_p_u8 = p_flow_tbl->r2y.CR_buf_36;
    (void)amba_ik_system_memcpy(&p_flow_tbl_list->r2y.p_CR_buf_36, &tmp_p_u8, sizeof(void*));
    tmp_p_u8 = p_flow_tbl->r2y.CR_buf_37;
    (void)amba_ik_system_memcpy(&p_flow_tbl_list->r2y.p_CR_buf_37, &tmp_p_u8, sizeof(void*));
    tmp_p_u8 = p_flow_tbl->r2y.CR_buf_38;
    (void)amba_ik_system_memcpy(&p_flow_tbl_list->r2y.p_CR_buf_38, &tmp_p_u8, sizeof(void*));
    tmp_p_u8 = p_flow_tbl->r2y.CR_buf_39;
    (void)amba_ik_system_memcpy(&p_flow_tbl_list->r2y.p_CR_buf_39, &tmp_p_u8, sizeof(void*));
    tmp_p_u8 = p_flow_tbl->r2y.CR_buf_40;
    (void)amba_ik_system_memcpy(&p_flow_tbl_list->r2y.p_CR_buf_40, &tmp_p_u8, sizeof(void*));
    tmp_p_u8 = p_flow_tbl->r2y.CR_buf_41;
    (void)amba_ik_system_memcpy(&p_flow_tbl_list->r2y.p_CR_buf_41, &tmp_p_u8, sizeof(void*));
    tmp_p_u8 = p_flow_tbl->r2y.CR_buf_42;
    (void)amba_ik_system_memcpy(&p_flow_tbl_list->r2y.p_CR_buf_42, &tmp_p_u8, sizeof(void*));
    tmp_p_u8 = p_flow_tbl->r2y.CR_buf_43;
    (void)amba_ik_system_memcpy(&p_flow_tbl_list->r2y.p_CR_buf_43, &tmp_p_u8, sizeof(void*));
    tmp_p_u8 = p_flow_tbl->r2y.CR_buf_44;
    (void)amba_ik_system_memcpy(&p_flow_tbl_list->r2y.p_CR_buf_44, &tmp_p_u8, sizeof(void*));
    tmp_p_u8 = p_flow_tbl->r2y.CR_buf_45;
    (void)amba_ik_system_memcpy(&p_flow_tbl_list->r2y.p_CR_buf_45, &tmp_p_u8, sizeof(void*));
    tmp_p_u8 = p_flow_tbl->r2y.CR_buf_46;
    (void)amba_ik_system_memcpy(&p_flow_tbl_list->r2y.p_CR_buf_46, &tmp_p_u8, sizeof(void*));
    tmp_p_u8 = p_flow_tbl->r2y.CR_buf_47;
    (void)amba_ik_system_memcpy(&p_flow_tbl_list->r2y.p_CR_buf_47, &tmp_p_u8, sizeof(void*));
    tmp_p_u8 = p_flow_tbl->r2y.CR_buf_48;
    (void)amba_ik_system_memcpy(&p_flow_tbl_list->r2y.p_CR_buf_48, &tmp_p_u8, sizeof(void*));
    tmp_p_u8 = p_flow_tbl->r2y.CR_buf_49;
    (void)amba_ik_system_memcpy(&p_flow_tbl_list->r2y.p_CR_buf_49, &tmp_p_u8, sizeof(void*));
    tmp_p_u8 = p_flow_tbl->r2y.CR_buf_50;
    (void)amba_ik_system_memcpy(&p_flow_tbl_list->r2y.p_CR_buf_50, &tmp_p_u8, sizeof(void*));
    tmp_p_u8 = p_flow_tbl->r2y.CR_buf_51;
    (void)amba_ik_system_memcpy(&p_flow_tbl_list->r2y.p_CR_buf_51, &tmp_p_u8, sizeof(void*));
    tmp_p_u8 = p_flow_tbl->r2y.CR_buf_52;
    (void)amba_ik_system_memcpy(&p_flow_tbl_list->r2y.p_CR_buf_52, &tmp_p_u8, sizeof(void*));

    tmp_p_u8 = p_flow_tbl->r2y.CR_buf_100;
    (void)amba_ik_system_memcpy(&p_flow_tbl_list->r2y.p_CR_buf_100, &tmp_p_u8, sizeof(void*));
    tmp_p_u8 = p_flow_tbl->r2y.CR_buf_101;
    (void)amba_ik_system_memcpy(&p_flow_tbl_list->r2y.p_CR_buf_101, &tmp_p_u8, sizeof(void*));
    tmp_p_u8 = p_flow_tbl->r2y.CR_buf_102;
    (void)amba_ik_system_memcpy(&p_flow_tbl_list->r2y.p_CR_buf_102, &tmp_p_u8, sizeof(void*));
    tmp_p_u8 = p_flow_tbl->r2y.CR_buf_103;
    (void)amba_ik_system_memcpy(&p_flow_tbl_list->r2y.p_CR_buf_103, &tmp_p_u8, sizeof(void*));

    tmp_p_u8 = p_flow_tbl->r2y.CR_buf_111;
    (void)amba_ik_system_memcpy(&p_flow_tbl_list->r2y.p_CR_buf_111, &tmp_p_u8, sizeof(void*));
    tmp_p_u8 = p_flow_tbl->r2y.CR_buf_112;
    (void)amba_ik_system_memcpy(&p_flow_tbl_list->r2y.p_CR_buf_112, &tmp_p_u8, sizeof(void*));
    tmp_p_u8 = p_flow_tbl->r2y.CR_buf_113;
    (void)amba_ik_system_memcpy(&p_flow_tbl_list->r2y.p_CR_buf_113, &tmp_p_u8, sizeof(void*));
    tmp_p_u8 = p_flow_tbl->r2y.CR_buf_114;
    (void)amba_ik_system_memcpy(&p_flow_tbl_list->r2y.p_CR_buf_114, &tmp_p_u8, sizeof(void*));
    tmp_p_u8 = p_flow_tbl->r2y.CR_buf_115;
    (void)amba_ik_system_memcpy(&p_flow_tbl_list->r2y.p_CR_buf_115, &tmp_p_u8, sizeof(void*));
    tmp_p_u8 = p_flow_tbl->r2y.CR_buf_116;
    (void)amba_ik_system_memcpy(&p_flow_tbl_list->r2y.p_CR_buf_116, &tmp_p_u8, sizeof(void*));
    tmp_p_u8 = p_flow_tbl->r2y.CR_buf_117;
    (void)amba_ik_system_memcpy(&p_flow_tbl_list->r2y.p_CR_buf_117, &tmp_p_u8, sizeof(void*));
    tmp_p_u8 = p_flow_tbl->r2y.CR_buf_118;
    (void)amba_ik_system_memcpy(&p_flow_tbl_list->r2y.p_CR_buf_118, &tmp_p_u8, sizeof(void*));
    tmp_p_u8 = p_flow_tbl->r2y.CR_buf_119;
    (void)amba_ik_system_memcpy(&p_flow_tbl_list->r2y.p_CR_buf_119, &tmp_p_u8, sizeof(void*));
    tmp_p_u8 = p_flow_tbl->r2y.CR_buf_120;
    (void)amba_ik_system_memcpy(&p_flow_tbl_list->r2y.p_CR_buf_120, &tmp_p_u8, sizeof(void*));
    tmp_p_u8 = p_flow_tbl->r2y.CR_buf_121;
    (void)amba_ik_system_memcpy(&p_flow_tbl_list->r2y.p_CR_buf_121, &tmp_p_u8, sizeof(void*));

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
    tmp_p_s16 = p_flow_tbl->warp_hor_b;
    (void)amba_ik_system_memcpy(&p_flow_tbl_list->p_warp_hor_b, &tmp_p_s16, sizeof(void*));
    tmp_p_extra_win = &p_flow_tbl->extra_window_info;
    (void)amba_ik_system_memcpy(&p_flow_tbl_list->p_extra_window, &tmp_p_extra_win, sizeof(void*));
    tmp_p_frame_info = &p_flow_tbl->frame_info;
    (void)amba_ik_system_memcpy(&p_flow_tbl_list->p_frame_info, &tmp_p_frame_info, sizeof(void *));

    return rval;
}

/****************     init cr lists     ****************/
// common use api
static void img_cfg_populate_hiso_r2y_sec2_flow_tbl_list(const amba_ik_step1_sec2_cr_t *p_flow_tbl, amba_ik_step1_sec2_cr_list_t *p_flow_tbl_list)
{
    const uint8 *tmp_p_u8;

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
}

static void img_cfg_populate_hiso_r2y_sec4_flow_tbl_list(const amba_ik_sec4_cr_t *p_flow_tbl, amba_ik_sec4_cr_list_t *p_flow_tbl_list)
{
    const uint8 *tmp_p_u8;

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
}

static void img_cfg_populate_hiso_y2y_sec2_flow_tbl_list(const amba_ik_step3_sec2_cr_t *p_flow_tbl, amba_ik_step3_sec2_cr_list_t *p_flow_tbl_list)
{
    const uint8 *tmp_p_u8;

    tmp_p_u8 = p_flow_tbl->CR_buf_26;
    (void)amba_ik_system_memcpy(&p_flow_tbl_list->p_CR_buf_26, &tmp_p_u8, sizeof(void*));
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
}

// every steps api
static void img_cfg_populate_hiso_step1_flow_tbl_list(const amba_ik_hiso_step1_cr_t *p_flow_tbl, amba_ik_hiso_step1_cr_list_t *p_flow_tbl_list)
{
    const uint8 *tmp_p_u8;

    img_cfg_populate_hiso_r2y_sec2_flow_tbl_list(&p_flow_tbl->sec2, &p_flow_tbl_list->sec2);
    img_cfg_populate_hiso_r2y_sec4_flow_tbl_list(&p_flow_tbl->sec4, &p_flow_tbl_list->sec4);

    tmp_p_u8 = p_flow_tbl->sec18.CR_buf_112;
    (void)amba_ik_system_memcpy(&p_flow_tbl_list->sec18.p_CR_buf_112, &tmp_p_u8, sizeof(void*));
    tmp_p_u8 = p_flow_tbl->sec18.CR_buf_117;
    (void)amba_ik_system_memcpy(&p_flow_tbl_list->sec18.p_CR_buf_117, &tmp_p_u8, sizeof(void*));
    tmp_p_u8 = p_flow_tbl->sec18.CR_buf_118;
    (void)amba_ik_system_memcpy(&p_flow_tbl_list->sec18.p_CR_buf_118, &tmp_p_u8, sizeof(void*));
}

static void img_cfg_populate_hiso_step2_flow_tbl_list(const amba_ik_hiso_step2_cr_t *p_flow_tbl, amba_ik_hiso_step2_cr_list_t *p_flow_tbl_list)
{
    const uint8 *tmp_p_u8;

    tmp_p_u8 = p_flow_tbl->sec2.CR_buf_26;
    (void)amba_ik_system_memcpy(&p_flow_tbl_list->sec2.p_CR_buf_26, &tmp_p_u8, sizeof(void*));
    tmp_p_u8 = p_flow_tbl->sec2.CR_buf_27;
    (void)amba_ik_system_memcpy(&p_flow_tbl_list->sec2.p_CR_buf_27, &tmp_p_u8, sizeof(void*));
    tmp_p_u8 = p_flow_tbl->sec2.CR_buf_29;
    (void)amba_ik_system_memcpy(&p_flow_tbl_list->sec2.p_CR_buf_29, &tmp_p_u8, sizeof(void*));
    tmp_p_u8 = p_flow_tbl->sec2.CR_buf_30;
    (void)amba_ik_system_memcpy(&p_flow_tbl_list->sec2.p_CR_buf_30, &tmp_p_u8, sizeof(void*));
    tmp_p_u8 = p_flow_tbl->sec2.CR_buf_31;
    (void)amba_ik_system_memcpy(&p_flow_tbl_list->sec2.p_CR_buf_31, &tmp_p_u8, sizeof(void*));
    tmp_p_u8 = p_flow_tbl->sec2.CR_buf_32;
    (void)amba_ik_system_memcpy(&p_flow_tbl_list->sec2.p_CR_buf_32, &tmp_p_u8, sizeof(void*));
    tmp_p_u8 = p_flow_tbl->sec2.CR_buf_33;
    (void)amba_ik_system_memcpy(&p_flow_tbl_list->sec2.p_CR_buf_33, &tmp_p_u8, sizeof(void*));
    tmp_p_u8 = p_flow_tbl->sec2.CR_buf_35;
    (void)amba_ik_system_memcpy(&p_flow_tbl_list->sec2.p_CR_buf_35, &tmp_p_u8, sizeof(void*));
    tmp_p_u8 = p_flow_tbl->sec2.CR_buf_36;
    (void)amba_ik_system_memcpy(&p_flow_tbl_list->sec2.p_CR_buf_36, &tmp_p_u8, sizeof(void*));
    tmp_p_u8 = p_flow_tbl->sec2.CR_buf_37;
    (void)amba_ik_system_memcpy(&p_flow_tbl_list->sec2.p_CR_buf_37, &tmp_p_u8, sizeof(void*));
    tmp_p_u8 = p_flow_tbl->sec2.CR_buf_38;
    (void)amba_ik_system_memcpy(&p_flow_tbl_list->sec2.p_CR_buf_38, &tmp_p_u8, sizeof(void*));
    tmp_p_u8 = p_flow_tbl->sec2.CR_buf_39;
    (void)amba_ik_system_memcpy(&p_flow_tbl_list->sec2.p_CR_buf_39, &tmp_p_u8, sizeof(void*));
    tmp_p_u8 = p_flow_tbl->sec2.CR_buf_40;
    (void)amba_ik_system_memcpy(&p_flow_tbl_list->sec2.p_CR_buf_40, &tmp_p_u8, sizeof(void*));
    tmp_p_u8 = p_flow_tbl->sec2.CR_buf_41;
    (void)amba_ik_system_memcpy(&p_flow_tbl_list->sec2.p_CR_buf_41, &tmp_p_u8, sizeof(void*));

    tmp_p_u8 = p_flow_tbl->sec18.CR_buf_112;
    (void)amba_ik_system_memcpy(&p_flow_tbl_list->sec18.p_CR_buf_112, &tmp_p_u8, sizeof(void*));
    tmp_p_u8 = p_flow_tbl->sec18.CR_buf_117;
    (void)amba_ik_system_memcpy(&p_flow_tbl_list->sec18.p_CR_buf_117, &tmp_p_u8, sizeof(void*));
    tmp_p_u8 = p_flow_tbl->sec18.CR_buf_118;
    (void)amba_ik_system_memcpy(&p_flow_tbl_list->sec18.p_CR_buf_118, &tmp_p_u8, sizeof(void*));
}

static void img_cfg_populate_hiso_step3_flow_tbl_list(const amba_ik_hiso_step3_cr_t *p_flow_tbl, amba_ik_hiso_step3_cr_list_t *p_flow_tbl_list)
{
    const uint8 *tmp_p_u8;

    img_cfg_populate_hiso_y2y_sec2_flow_tbl_list(&p_flow_tbl->sec2, &p_flow_tbl_list->sec2);

    tmp_p_u8 = p_flow_tbl->sec18.CR_buf_112;
    (void)amba_ik_system_memcpy(&p_flow_tbl_list->sec18.p_CR_buf_112, &tmp_p_u8, sizeof(void*));
    tmp_p_u8 = p_flow_tbl->sec18.CR_buf_117;
    (void)amba_ik_system_memcpy(&p_flow_tbl_list->sec18.p_CR_buf_117, &tmp_p_u8, sizeof(void*));
    tmp_p_u8 = p_flow_tbl->sec18.CR_buf_118;
    (void)amba_ik_system_memcpy(&p_flow_tbl_list->sec18.p_CR_buf_118, &tmp_p_u8, sizeof(void*));
}

static void img_cfg_populate_hiso_step4a_flow_tbl_list(const amba_ik_hiso_step4a_cr_t *p_flow_tbl, amba_ik_hiso_step4a_cr_list_t *p_flow_tbl_list)
{
    img_cfg_populate_hiso_y2y_sec2_flow_tbl_list(&p_flow_tbl->sec2, &p_flow_tbl_list->sec2);
}

static void img_cfg_populate_hiso_step5_flow_tbl_list(const amba_ik_hiso_step5_cr_t *p_flow_tbl, amba_ik_hiso_step5_cr_list_t *p_flow_tbl_list)
{
    const uint8 *tmp_p_u8;

    img_cfg_populate_hiso_y2y_sec2_flow_tbl_list(&p_flow_tbl->sec2, &p_flow_tbl_list->sec2);

    tmp_p_u8 = p_flow_tbl->sec18.CR_buf_112;
    (void)amba_ik_system_memcpy(&p_flow_tbl_list->sec18.p_CR_buf_112, &tmp_p_u8, sizeof(void*));
    tmp_p_u8 = p_flow_tbl->sec18.CR_buf_114;
    (void)amba_ik_system_memcpy(&p_flow_tbl_list->sec18.p_CR_buf_114, &tmp_p_u8, sizeof(void*));
    tmp_p_u8 = p_flow_tbl->sec18.CR_buf_117;
    (void)amba_ik_system_memcpy(&p_flow_tbl_list->sec18.p_CR_buf_117, &tmp_p_u8, sizeof(void*));
    tmp_p_u8 = p_flow_tbl->sec18.CR_buf_118;
    (void)amba_ik_system_memcpy(&p_flow_tbl_list->sec18.p_CR_buf_118, &tmp_p_u8, sizeof(void*));
}

static void img_cfg_populate_hiso_step6_flow_tbl_list(const amba_ik_hiso_step6_cr_t *p_flow_tbl, amba_ik_hiso_step6_cr_list_t *p_flow_tbl_list)
{
    const uint8 *tmp_p_u8;

    img_cfg_populate_hiso_y2y_sec2_flow_tbl_list(&p_flow_tbl->sec2, &p_flow_tbl_list->sec2);

    tmp_p_u8 = p_flow_tbl->sec18.CR_buf_112;
    (void)amba_ik_system_memcpy(&p_flow_tbl_list->sec18.p_CR_buf_112, &tmp_p_u8, sizeof(void*));
    tmp_p_u8 = p_flow_tbl->sec18.CR_buf_114;
    (void)amba_ik_system_memcpy(&p_flow_tbl_list->sec18.p_CR_buf_114, &tmp_p_u8, sizeof(void*));
    tmp_p_u8 = p_flow_tbl->sec18.CR_buf_117;
    (void)amba_ik_system_memcpy(&p_flow_tbl_list->sec18.p_CR_buf_117, &tmp_p_u8, sizeof(void*));
}

static void img_cfg_populate_hiso_step11_flow_tbl_list(const amba_ik_hiso_step11_cr_t *p_flow_tbl, amba_ik_hiso_step11_cr_list_t *p_flow_tbl_list)
{
    const uint8 *tmp_p_u8;

    img_cfg_populate_hiso_r2y_sec2_flow_tbl_list(&p_flow_tbl->sec2, &p_flow_tbl_list->sec2);
    img_cfg_populate_hiso_r2y_sec4_flow_tbl_list(&p_flow_tbl->sec4, &p_flow_tbl_list->sec4);

    tmp_p_u8 = p_flow_tbl->sec18.CR_buf_112;
    (void)amba_ik_system_memcpy(&p_flow_tbl_list->sec18.p_CR_buf_112, &tmp_p_u8, sizeof(void*));
    tmp_p_u8 = p_flow_tbl->sec18.CR_buf_114;
    (void)amba_ik_system_memcpy(&p_flow_tbl_list->sec18.p_CR_buf_114, &tmp_p_u8, sizeof(void*));
    tmp_p_u8 = p_flow_tbl->sec18.CR_buf_117;
    (void)amba_ik_system_memcpy(&p_flow_tbl_list->sec18.p_CR_buf_117, &tmp_p_u8, sizeof(void*));
    tmp_p_u8 = p_flow_tbl->sec18.CR_buf_118;
    (void)amba_ik_system_memcpy(&p_flow_tbl_list->sec18.p_CR_buf_118, &tmp_p_u8, sizeof(void*));
}

static void img_cfg_populate_hiso_step13_flow_tbl_list(const amba_ik_hiso_step13_cr_t *p_flow_tbl, amba_ik_hiso_step13_cr_list_t *p_flow_tbl_list)
{
    const uint8 *tmp_p_u8;

    img_cfg_populate_hiso_y2y_sec2_flow_tbl_list(&p_flow_tbl->sec2, &p_flow_tbl_list->sec2);

#ifdef _HISO_SEC3_IN_STEP13
    tmp_p_u8 = p_flow_tbl->sec3.CR_buf_42;
    (void)amba_ik_system_memcpy(&p_flow_tbl_list->sec3.p_CR_buf_42, &tmp_p_u8, sizeof(void*));
    tmp_p_u8 = p_flow_tbl->sec3.CR_buf_43;
    (void)amba_ik_system_memcpy(&p_flow_tbl_list->sec3.p_CR_buf_43, &tmp_p_u8, sizeof(void*));
    tmp_p_u8 = p_flow_tbl->sec3.CR_buf_44;
    (void)amba_ik_system_memcpy(&p_flow_tbl_list->sec3.p_CR_buf_44, &tmp_p_u8, sizeof(void*));
#endif

    tmp_p_u8 = p_flow_tbl->sec18.CR_buf_112;
    (void)amba_ik_system_memcpy(&p_flow_tbl_list->sec18.p_CR_buf_112, &tmp_p_u8, sizeof(void*));
    tmp_p_u8 = p_flow_tbl->sec18.CR_buf_114;
    (void)amba_ik_system_memcpy(&p_flow_tbl_list->sec18.p_CR_buf_114, &tmp_p_u8, sizeof(void*));
    tmp_p_u8 = p_flow_tbl->sec18.CR_buf_117;
    (void)amba_ik_system_memcpy(&p_flow_tbl_list->sec18.p_CR_buf_117, &tmp_p_u8, sizeof(void*));
}

static void img_cfg_populate_hiso_step14_flow_tbl_list(const amba_ik_hiso_step14_cr_t *p_flow_tbl, amba_ik_hiso_step14_cr_list_t *p_flow_tbl_list)
{
    const uint8 *tmp_p_u8;

    tmp_p_u8 = p_flow_tbl->sec3.CR_buf_42;
    (void)amba_ik_system_memcpy(&p_flow_tbl_list->sec3.p_CR_buf_42, &tmp_p_u8, sizeof(void*));
    tmp_p_u8 = p_flow_tbl->sec3.CR_buf_43;
    (void)amba_ik_system_memcpy(&p_flow_tbl_list->sec3.p_CR_buf_43, &tmp_p_u8, sizeof(void*));
    tmp_p_u8 = p_flow_tbl->sec3.CR_buf_44;
    (void)amba_ik_system_memcpy(&p_flow_tbl_list->sec3.p_CR_buf_44, &tmp_p_u8, sizeof(void*));

    tmp_p_u8 = p_flow_tbl->sec18.CR_buf_112;
    (void)amba_ik_system_memcpy(&p_flow_tbl_list->sec18.p_CR_buf_112, &tmp_p_u8, sizeof(void*));
    tmp_p_u8 = p_flow_tbl->sec18.CR_buf_117;
    (void)amba_ik_system_memcpy(&p_flow_tbl_list->sec18.p_CR_buf_117, &tmp_p_u8, sizeof(void*));
}

static uint32 img_cfg_populate_hiso_flow_tbl_list(const amba_ik_hiso_flow_tables_t *p_flow_tbl, amba_ik_hiso_flow_tables_list_t *p_flow_tbl_list)
{
    uint32 rval = IK_OK;
    const int16 *tmp_p_s16;
    const idsp_aaa_data_t *tmp_p_aaa;
    const ik_query_frame_info_t *tmp_p_frame_info;

    img_cfg_populate_hiso_step1_flow_tbl_list(&p_flow_tbl->step1, &p_flow_tbl_list->step1);
    img_cfg_populate_hiso_step2_flow_tbl_list(&p_flow_tbl->step2, &p_flow_tbl_list->step2);
    img_cfg_populate_hiso_step3_flow_tbl_list(&p_flow_tbl->step3, &p_flow_tbl_list->step3);
    img_cfg_populate_hiso_step3_flow_tbl_list(&p_flow_tbl->step4, &p_flow_tbl_list->step4);
    img_cfg_populate_hiso_step4a_flow_tbl_list(&p_flow_tbl->step4a, &p_flow_tbl_list->step4a);
    img_cfg_populate_hiso_step5_flow_tbl_list(&p_flow_tbl->step5, &p_flow_tbl_list->step5);
    img_cfg_populate_hiso_step6_flow_tbl_list(&p_flow_tbl->step6, &p_flow_tbl_list->step6);
    img_cfg_populate_hiso_step6_flow_tbl_list(&p_flow_tbl->step7, &p_flow_tbl_list->step7);
    img_cfg_populate_hiso_step6_flow_tbl_list(&p_flow_tbl->step8, &p_flow_tbl_list->step8);
    img_cfg_populate_hiso_step4a_flow_tbl_list(&p_flow_tbl->step9, &p_flow_tbl_list->step9);
    img_cfg_populate_hiso_step5_flow_tbl_list(&p_flow_tbl->step10, &p_flow_tbl_list->step10);
    img_cfg_populate_hiso_step11_flow_tbl_list(&p_flow_tbl->step11, &p_flow_tbl_list->step11);
    img_cfg_populate_hiso_step11_flow_tbl_list(&p_flow_tbl->step12, &p_flow_tbl_list->step12);
    img_cfg_populate_hiso_step13_flow_tbl_list(&p_flow_tbl->step13, &p_flow_tbl_list->step13);
#ifndef _HISO_SEC3_IN_STEP13
    img_cfg_populate_hiso_step14_flow_tbl_list(&p_flow_tbl->step14, &p_flow_tbl_list->step14);
#endif

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

static void img_cfg_populate_motion_fusion_step1_flow_tbl_list(const amba_ik_r2y_flow_tables_t *p_flow_tbl, amba_ik_r2y_flow_tables_list_t *p_flow_tbl_list)
{
    const uint8 *tmp_p_u8;

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

    tmp_p_u8 = p_flow_tbl->CR_buf_100;
    (void)amba_ik_system_memcpy(&p_flow_tbl_list->p_CR_buf_100, &tmp_p_u8, sizeof(void*));
    tmp_p_u8 = p_flow_tbl->CR_buf_101;
    (void)amba_ik_system_memcpy(&p_flow_tbl_list->p_CR_buf_101, &tmp_p_u8, sizeof(void*));
    tmp_p_u8 = p_flow_tbl->CR_buf_102;
    (void)amba_ik_system_memcpy(&p_flow_tbl_list->p_CR_buf_102, &tmp_p_u8, sizeof(void*));
    tmp_p_u8 = p_flow_tbl->CR_buf_103;
    (void)amba_ik_system_memcpy(&p_flow_tbl_list->p_CR_buf_103, &tmp_p_u8, sizeof(void*));

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
}

static void img_cfg_populate_motion_fusion_step2_flow_tbl_list(const amba_ik_motion_me1_flow_tables_t *p_flow_tbl, amba_ik_motion_me1_flow_tables_list_t *p_flow_tbl_list)
{
    const uint8 *tmp_p_u8;

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
}

static void img_cfg_populate_motion_fusion_step3_flow_tbl_list(const amba_ik_motion_a_flow_tables_t *p_flow_tbl, amba_ik_motion_a_flow_tables_list_t *p_flow_tbl_list)
{
    const uint8 *tmp_p_u8;

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
}

static void img_cfg_populate_motion_fusion_step4_flow_tbl_list(const amba_ik_motion_b_flow_tables_t *p_flow_tbl, amba_ik_motion_b_flow_tables_list_t *p_flow_tbl_list)
{
    const uint8 *tmp_p_u8;

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
}

static void img_cfg_populate_motion_fusion_step5_flow_tbl_list(const amba_ik_motion_c_flow_tables_t *p_flow_tbl, amba_ik_motion_c_flow_tables_list_t *p_flow_tbl_list)
{
    const uint8 *tmp_p_u8;

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
}
#if SUPPORT_FUSION
static void img_cfg_populate_mono1_8_flow_tbl_list(const amba_ik_r2y_flow_tables_t *p_flow_tbl, amba_ik_r2y_flow_tables_list_t *p_flow_tbl_list)
{
    const uint8 *tmp_p_u8;

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

    tmp_p_u8 = p_flow_tbl->CR_buf_100;
    (void)amba_ik_system_memcpy(&p_flow_tbl_list->p_CR_buf_100, &tmp_p_u8, sizeof(void*));
    tmp_p_u8 = p_flow_tbl->CR_buf_101;
    (void)amba_ik_system_memcpy(&p_flow_tbl_list->p_CR_buf_101, &tmp_p_u8, sizeof(void*));
    tmp_p_u8 = p_flow_tbl->CR_buf_102;
    (void)amba_ik_system_memcpy(&p_flow_tbl_list->p_CR_buf_102, &tmp_p_u8, sizeof(void*));
    tmp_p_u8 = p_flow_tbl->CR_buf_103;
    (void)amba_ik_system_memcpy(&p_flow_tbl_list->p_CR_buf_103, &tmp_p_u8, sizeof(void*));

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
}

static void img_cfg_populate_mono3_flow_tbl_list(const amba_ik_mono3_flow_tables_t *p_flow_tbl, amba_ik_mono3_flow_tables_list_t *p_flow_tbl_list)
{
    const uint8 *tmp_p_u8;

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
}

static void img_cfg_populate_mono4_flow_tbl_list(const amba_ik_mono4_flow_tables_t *p_flow_tbl, amba_ik_mono4_flow_tables_list_t *p_flow_tbl_list)
{
    const uint8 *tmp_p_u8;

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
}

static void img_cfg_populate_mono5_flow_tbl_list(const amba_ik_mono5_flow_tables_t *p_flow_tbl, amba_ik_mono5_flow_tables_list_t *p_flow_tbl_list)
{
    const uint8 *tmp_p_u8;

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
}

static void img_cfg_populate_mono6_flow_tbl_list(const amba_ik_mono6_flow_tables_t *p_flow_tbl, amba_ik_mono6_flow_tables_list_t *p_flow_tbl_list)
{
    const uint8 *tmp_p_u8;

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
}

static void img_cfg_populate_mono7_flow_tbl_list(const amba_ik_mono7_flow_tables_t *p_flow_tbl, amba_ik_mono7_flow_tables_list_t *p_flow_tbl_list)
{
    const uint8 *tmp_p_u8;

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
}
#endif
static uint32 img_cfg_populate_motion_flow_tbl_list(const amba_ik_motion_fusion_flow_tables_t *p_flow_tbl, amba_ik_motion_fusion_flow_tables_list_t *p_flow_tbl_list)
{
    uint32 rval = IK_OK;
    const int16 *tmp_p_s16;
    const idsp_aaa_data_t *tmp_p_aaa;
    const idsp_extra_window_info_t *tmp_p_extra_win;
    const ik_query_frame_info_t *tmp_p_frame_info;

    img_cfg_populate_motion_fusion_step1_flow_tbl_list(&p_flow_tbl->r2y, &p_flow_tbl_list->r2y);
    img_cfg_populate_motion_fusion_step2_flow_tbl_list(&p_flow_tbl->motion_me1, &p_flow_tbl_list->motion_me1);
    img_cfg_populate_motion_fusion_step3_flow_tbl_list(&p_flow_tbl->motion_a, &p_flow_tbl_list->motion_a);
    img_cfg_populate_motion_fusion_step4_flow_tbl_list(&p_flow_tbl->motion_b, &p_flow_tbl_list->motion_b);
    img_cfg_populate_motion_fusion_step5_flow_tbl_list(&p_flow_tbl->motion_c, &p_flow_tbl_list->motion_c);
#if SUPPORT_FUSION
    img_cfg_populate_mono1_8_flow_tbl_list(&p_flow_tbl->mono1, &p_flow_tbl_list->mono1);
    img_cfg_populate_mono3_flow_tbl_list(&p_flow_tbl->mono3, &p_flow_tbl_list->mono3);
    img_cfg_populate_mono4_flow_tbl_list(&p_flow_tbl->mono4, &p_flow_tbl_list->mono4);
    img_cfg_populate_mono5_flow_tbl_list(&p_flow_tbl->mono5, &p_flow_tbl_list->mono5);
    img_cfg_populate_mono6_flow_tbl_list(&p_flow_tbl->mono6, &p_flow_tbl_list->mono6);
    img_cfg_populate_mono7_flow_tbl_list(&p_flow_tbl->mono7, &p_flow_tbl_list->mono7);
    img_cfg_populate_mono1_8_flow_tbl_list(&p_flow_tbl->mono8, &p_flow_tbl_list->mono8);
#endif
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
    tmp_p_s16 = p_flow_tbl->warp_hor_b;
    (void)amba_ik_system_memcpy(&p_flow_tbl_list->p_warp_hor_b, &tmp_p_s16, sizeof(void*));
    tmp_p_aaa = p_flow_tbl->aaa[0];
    (void)amba_ik_system_memcpy(&p_flow_tbl_list->p_aaa, &tmp_p_aaa, sizeof(void*));
    tmp_p_extra_win = &p_flow_tbl->extra_window_info;
    (void)amba_ik_system_memcpy(&p_flow_tbl_list->p_extra_window, &tmp_p_extra_win, sizeof(void*));
    tmp_p_frame_info = &p_flow_tbl->frame_info;
    (void)amba_ik_system_memcpy(&p_flow_tbl_list->p_frame_info, &tmp_p_frame_info, sizeof(void *));

    return rval;
}

static void img_cfg_update_motion_fusion_step1_flow_tbl_list(const int64 addr_offset, amba_ik_r2y_flow_tables_list_t *p_flow_tbl_list)
{
    p_flow_tbl_list->p_CR_buf_4  = ik_address_add_offset(p_flow_tbl_list->p_CR_buf_4, addr_offset);
    p_flow_tbl_list->p_CR_buf_5  = ik_address_add_offset(p_flow_tbl_list->p_CR_buf_5, addr_offset);
    p_flow_tbl_list->p_CR_buf_6  = ik_address_add_offset(p_flow_tbl_list->p_CR_buf_6, addr_offset);
    p_flow_tbl_list->p_CR_buf_7  = ik_address_add_offset(p_flow_tbl_list->p_CR_buf_7, addr_offset);
    p_flow_tbl_list->p_CR_buf_8  = ik_address_add_offset(p_flow_tbl_list->p_CR_buf_8, addr_offset);
    p_flow_tbl_list->p_CR_buf_9  = ik_address_add_offset(p_flow_tbl_list->p_CR_buf_9, addr_offset);
    p_flow_tbl_list->p_CR_buf_10 = ik_address_add_offset(p_flow_tbl_list->p_CR_buf_10, addr_offset);
    p_flow_tbl_list->p_CR_buf_11 = ik_address_add_offset(p_flow_tbl_list->p_CR_buf_11, addr_offset);
    p_flow_tbl_list->p_CR_buf_12 = ik_address_add_offset(p_flow_tbl_list->p_CR_buf_12, addr_offset);
    p_flow_tbl_list->p_CR_buf_13 = ik_address_add_offset(p_flow_tbl_list->p_CR_buf_13, addr_offset);
    p_flow_tbl_list->p_CR_buf_14 = ik_address_add_offset(p_flow_tbl_list->p_CR_buf_14, addr_offset);
    p_flow_tbl_list->p_CR_buf_15 = ik_address_add_offset(p_flow_tbl_list->p_CR_buf_15, addr_offset);
    p_flow_tbl_list->p_CR_buf_16 = ik_address_add_offset(p_flow_tbl_list->p_CR_buf_16, addr_offset);
    p_flow_tbl_list->p_CR_buf_17 = ik_address_add_offset(p_flow_tbl_list->p_CR_buf_17, addr_offset);
    p_flow_tbl_list->p_CR_buf_18 = ik_address_add_offset(p_flow_tbl_list->p_CR_buf_18, addr_offset);
    p_flow_tbl_list->p_CR_buf_19 = ik_address_add_offset(p_flow_tbl_list->p_CR_buf_19, addr_offset);
    p_flow_tbl_list->p_CR_buf_20 = ik_address_add_offset(p_flow_tbl_list->p_CR_buf_20, addr_offset);
    p_flow_tbl_list->p_CR_buf_21 = ik_address_add_offset(p_flow_tbl_list->p_CR_buf_21, addr_offset);
    p_flow_tbl_list->p_CR_buf_22 = ik_address_add_offset(p_flow_tbl_list->p_CR_buf_22, addr_offset);
    p_flow_tbl_list->p_CR_buf_23 = ik_address_add_offset(p_flow_tbl_list->p_CR_buf_23, addr_offset);
    p_flow_tbl_list->p_CR_buf_24 = ik_address_add_offset(p_flow_tbl_list->p_CR_buf_24, addr_offset);
    p_flow_tbl_list->p_CR_buf_25 = ik_address_add_offset(p_flow_tbl_list->p_CR_buf_25, addr_offset);
    p_flow_tbl_list->p_CR_buf_26 = ik_address_add_offset(p_flow_tbl_list->p_CR_buf_26, addr_offset);
    p_flow_tbl_list->p_CR_buf_27 = ik_address_add_offset(p_flow_tbl_list->p_CR_buf_27, addr_offset);
    p_flow_tbl_list->p_CR_buf_28 = ik_address_add_offset(p_flow_tbl_list->p_CR_buf_28, addr_offset);
    p_flow_tbl_list->p_CR_buf_29 = ik_address_add_offset(p_flow_tbl_list->p_CR_buf_29, addr_offset);
    p_flow_tbl_list->p_CR_buf_30 = ik_address_add_offset(p_flow_tbl_list->p_CR_buf_30, addr_offset);
    p_flow_tbl_list->p_CR_buf_31 = ik_address_add_offset(p_flow_tbl_list->p_CR_buf_31, addr_offset);
    p_flow_tbl_list->p_CR_buf_32 = ik_address_add_offset(p_flow_tbl_list->p_CR_buf_32, addr_offset);
    p_flow_tbl_list->p_CR_buf_33 = ik_address_add_offset(p_flow_tbl_list->p_CR_buf_33, addr_offset);
    p_flow_tbl_list->p_CR_buf_34 = ik_address_add_offset(p_flow_tbl_list->p_CR_buf_34, addr_offset);
    p_flow_tbl_list->p_CR_buf_35 = ik_address_add_offset(p_flow_tbl_list->p_CR_buf_35, addr_offset);
    p_flow_tbl_list->p_CR_buf_36 = ik_address_add_offset(p_flow_tbl_list->p_CR_buf_36, addr_offset);
    p_flow_tbl_list->p_CR_buf_37 = ik_address_add_offset(p_flow_tbl_list->p_CR_buf_37, addr_offset);
    p_flow_tbl_list->p_CR_buf_38 = ik_address_add_offset(p_flow_tbl_list->p_CR_buf_38, addr_offset);
    p_flow_tbl_list->p_CR_buf_39 = ik_address_add_offset(p_flow_tbl_list->p_CR_buf_39, addr_offset);
    p_flow_tbl_list->p_CR_buf_40 = ik_address_add_offset(p_flow_tbl_list->p_CR_buf_40, addr_offset);
    p_flow_tbl_list->p_CR_buf_41 = ik_address_add_offset(p_flow_tbl_list->p_CR_buf_41, addr_offset);
    p_flow_tbl_list->p_CR_buf_42 = ik_address_add_offset(p_flow_tbl_list->p_CR_buf_42, addr_offset);
    p_flow_tbl_list->p_CR_buf_43 = ik_address_add_offset(p_flow_tbl_list->p_CR_buf_43, addr_offset);
    p_flow_tbl_list->p_CR_buf_44 = ik_address_add_offset(p_flow_tbl_list->p_CR_buf_44, addr_offset);
    p_flow_tbl_list->p_CR_buf_45 = ik_address_add_offset(p_flow_tbl_list->p_CR_buf_45, addr_offset);
    p_flow_tbl_list->p_CR_buf_46 = ik_address_add_offset(p_flow_tbl_list->p_CR_buf_46, addr_offset);
    p_flow_tbl_list->p_CR_buf_47 = ik_address_add_offset(p_flow_tbl_list->p_CR_buf_47, addr_offset);
    p_flow_tbl_list->p_CR_buf_48 = ik_address_add_offset(p_flow_tbl_list->p_CR_buf_48, addr_offset);
    p_flow_tbl_list->p_CR_buf_49 = ik_address_add_offset(p_flow_tbl_list->p_CR_buf_49, addr_offset);
    p_flow_tbl_list->p_CR_buf_50 = ik_address_add_offset(p_flow_tbl_list->p_CR_buf_50, addr_offset);
    p_flow_tbl_list->p_CR_buf_51 = ik_address_add_offset(p_flow_tbl_list->p_CR_buf_51, addr_offset);
    p_flow_tbl_list->p_CR_buf_52 = ik_address_add_offset(p_flow_tbl_list->p_CR_buf_52, addr_offset);

    p_flow_tbl_list->p_CR_buf_100= ik_address_add_offset(p_flow_tbl_list->p_CR_buf_100, addr_offset);
    p_flow_tbl_list->p_CR_buf_101= ik_address_add_offset(p_flow_tbl_list->p_CR_buf_101, addr_offset);
    p_flow_tbl_list->p_CR_buf_102= ik_address_add_offset(p_flow_tbl_list->p_CR_buf_102, addr_offset);
    p_flow_tbl_list->p_CR_buf_103= ik_address_add_offset(p_flow_tbl_list->p_CR_buf_103, addr_offset);
    p_flow_tbl_list->p_CR_buf_111= ik_address_add_offset(p_flow_tbl_list->p_CR_buf_111, addr_offset);
    p_flow_tbl_list->p_CR_buf_112= ik_address_add_offset(p_flow_tbl_list->p_CR_buf_112, addr_offset);
    p_flow_tbl_list->p_CR_buf_113= ik_address_add_offset(p_flow_tbl_list->p_CR_buf_113, addr_offset);
    p_flow_tbl_list->p_CR_buf_114= ik_address_add_offset(p_flow_tbl_list->p_CR_buf_114, addr_offset);
    p_flow_tbl_list->p_CR_buf_115= ik_address_add_offset(p_flow_tbl_list->p_CR_buf_115, addr_offset);
    p_flow_tbl_list->p_CR_buf_116= ik_address_add_offset(p_flow_tbl_list->p_CR_buf_116, addr_offset);
    p_flow_tbl_list->p_CR_buf_117= ik_address_add_offset(p_flow_tbl_list->p_CR_buf_117, addr_offset);
    p_flow_tbl_list->p_CR_buf_118= ik_address_add_offset(p_flow_tbl_list->p_CR_buf_118, addr_offset);
    p_flow_tbl_list->p_CR_buf_119= ik_address_add_offset(p_flow_tbl_list->p_CR_buf_119, addr_offset);
    p_flow_tbl_list->p_CR_buf_120= ik_address_add_offset(p_flow_tbl_list->p_CR_buf_120, addr_offset);
    p_flow_tbl_list->p_CR_buf_121= ik_address_add_offset(p_flow_tbl_list->p_CR_buf_121, addr_offset);

}

static void img_cfg_update_motion_fusion_step2_flow_tbl_list(const int64 addr_offset, amba_ik_motion_me1_flow_tables_list_t *p_flow_tbl_list)
{
    p_flow_tbl_list->p_CR_buf_4  = ik_address_add_offset(p_flow_tbl_list->p_CR_buf_4, addr_offset);
    p_flow_tbl_list->p_CR_buf_5  = ik_address_add_offset(p_flow_tbl_list->p_CR_buf_5, addr_offset);
    p_flow_tbl_list->p_CR_buf_6  = ik_address_add_offset(p_flow_tbl_list->p_CR_buf_6, addr_offset);
    p_flow_tbl_list->p_CR_buf_7  = ik_address_add_offset(p_flow_tbl_list->p_CR_buf_7, addr_offset);
    p_flow_tbl_list->p_CR_buf_8  = ik_address_add_offset(p_flow_tbl_list->p_CR_buf_8, addr_offset);
    p_flow_tbl_list->p_CR_buf_9  = ik_address_add_offset(p_flow_tbl_list->p_CR_buf_9, addr_offset);
    p_flow_tbl_list->p_CR_buf_10 = ik_address_add_offset(p_flow_tbl_list->p_CR_buf_10, addr_offset);
    p_flow_tbl_list->p_CR_buf_11 = ik_address_add_offset(p_flow_tbl_list->p_CR_buf_11, addr_offset);
    p_flow_tbl_list->p_CR_buf_12 = ik_address_add_offset(p_flow_tbl_list->p_CR_buf_12, addr_offset);
    p_flow_tbl_list->p_CR_buf_13 = ik_address_add_offset(p_flow_tbl_list->p_CR_buf_13, addr_offset);
    p_flow_tbl_list->p_CR_buf_14 = ik_address_add_offset(p_flow_tbl_list->p_CR_buf_14, addr_offset);
    p_flow_tbl_list->p_CR_buf_15 = ik_address_add_offset(p_flow_tbl_list->p_CR_buf_15, addr_offset);
    p_flow_tbl_list->p_CR_buf_16 = ik_address_add_offset(p_flow_tbl_list->p_CR_buf_16, addr_offset);
    p_flow_tbl_list->p_CR_buf_17 = ik_address_add_offset(p_flow_tbl_list->p_CR_buf_17, addr_offset);
    p_flow_tbl_list->p_CR_buf_18 = ik_address_add_offset(p_flow_tbl_list->p_CR_buf_18, addr_offset);
    p_flow_tbl_list->p_CR_buf_19 = ik_address_add_offset(p_flow_tbl_list->p_CR_buf_19, addr_offset);
    p_flow_tbl_list->p_CR_buf_20 = ik_address_add_offset(p_flow_tbl_list->p_CR_buf_20, addr_offset);
    p_flow_tbl_list->p_CR_buf_21 = ik_address_add_offset(p_flow_tbl_list->p_CR_buf_21, addr_offset);
    p_flow_tbl_list->p_CR_buf_22 = ik_address_add_offset(p_flow_tbl_list->p_CR_buf_22, addr_offset);
    p_flow_tbl_list->p_CR_buf_23 = ik_address_add_offset(p_flow_tbl_list->p_CR_buf_23, addr_offset);
    p_flow_tbl_list->p_CR_buf_24 = ik_address_add_offset(p_flow_tbl_list->p_CR_buf_24, addr_offset);
    p_flow_tbl_list->p_CR_buf_25 = ik_address_add_offset(p_flow_tbl_list->p_CR_buf_25, addr_offset);
    p_flow_tbl_list->p_CR_buf_26 = ik_address_add_offset(p_flow_tbl_list->p_CR_buf_26, addr_offset);
    p_flow_tbl_list->p_CR_buf_27 = ik_address_add_offset(p_flow_tbl_list->p_CR_buf_27, addr_offset);
    p_flow_tbl_list->p_CR_buf_28 = ik_address_add_offset(p_flow_tbl_list->p_CR_buf_28, addr_offset);
    p_flow_tbl_list->p_CR_buf_29 = ik_address_add_offset(p_flow_tbl_list->p_CR_buf_29, addr_offset);
    p_flow_tbl_list->p_CR_buf_30 = ik_address_add_offset(p_flow_tbl_list->p_CR_buf_30, addr_offset);
    p_flow_tbl_list->p_CR_buf_31 = ik_address_add_offset(p_flow_tbl_list->p_CR_buf_31, addr_offset);
    p_flow_tbl_list->p_CR_buf_32 = ik_address_add_offset(p_flow_tbl_list->p_CR_buf_32, addr_offset);
    p_flow_tbl_list->p_CR_buf_33 = ik_address_add_offset(p_flow_tbl_list->p_CR_buf_33, addr_offset);
    p_flow_tbl_list->p_CR_buf_34 = ik_address_add_offset(p_flow_tbl_list->p_CR_buf_34, addr_offset);
    p_flow_tbl_list->p_CR_buf_35 = ik_address_add_offset(p_flow_tbl_list->p_CR_buf_35, addr_offset);
    p_flow_tbl_list->p_CR_buf_36 = ik_address_add_offset(p_flow_tbl_list->p_CR_buf_36, addr_offset);
    p_flow_tbl_list->p_CR_buf_37 = ik_address_add_offset(p_flow_tbl_list->p_CR_buf_37, addr_offset);
    p_flow_tbl_list->p_CR_buf_38 = ik_address_add_offset(p_flow_tbl_list->p_CR_buf_38, addr_offset);
    p_flow_tbl_list->p_CR_buf_39 = ik_address_add_offset(p_flow_tbl_list->p_CR_buf_39, addr_offset);
    p_flow_tbl_list->p_CR_buf_40 = ik_address_add_offset(p_flow_tbl_list->p_CR_buf_40, addr_offset);
    p_flow_tbl_list->p_CR_buf_41 = ik_address_add_offset(p_flow_tbl_list->p_CR_buf_41, addr_offset);
    p_flow_tbl_list->p_CR_buf_42 = ik_address_add_offset(p_flow_tbl_list->p_CR_buf_42, addr_offset);
    p_flow_tbl_list->p_CR_buf_43 = ik_address_add_offset(p_flow_tbl_list->p_CR_buf_43, addr_offset);
    p_flow_tbl_list->p_CR_buf_44 = ik_address_add_offset(p_flow_tbl_list->p_CR_buf_44, addr_offset);

    p_flow_tbl_list->p_CR_buf_111= ik_address_add_offset(p_flow_tbl_list->p_CR_buf_111, addr_offset);
    p_flow_tbl_list->p_CR_buf_112= ik_address_add_offset(p_flow_tbl_list->p_CR_buf_112, addr_offset);
    p_flow_tbl_list->p_CR_buf_113= ik_address_add_offset(p_flow_tbl_list->p_CR_buf_113, addr_offset);
    p_flow_tbl_list->p_CR_buf_114= ik_address_add_offset(p_flow_tbl_list->p_CR_buf_114, addr_offset);
    p_flow_tbl_list->p_CR_buf_115= ik_address_add_offset(p_flow_tbl_list->p_CR_buf_115, addr_offset);
    p_flow_tbl_list->p_CR_buf_116= ik_address_add_offset(p_flow_tbl_list->p_CR_buf_116, addr_offset);
    p_flow_tbl_list->p_CR_buf_117= ik_address_add_offset(p_flow_tbl_list->p_CR_buf_117, addr_offset);
    p_flow_tbl_list->p_CR_buf_118= ik_address_add_offset(p_flow_tbl_list->p_CR_buf_118, addr_offset);
    p_flow_tbl_list->p_CR_buf_119= ik_address_add_offset(p_flow_tbl_list->p_CR_buf_119, addr_offset);
    p_flow_tbl_list->p_CR_buf_120= ik_address_add_offset(p_flow_tbl_list->p_CR_buf_120, addr_offset);
    p_flow_tbl_list->p_CR_buf_121= ik_address_add_offset(p_flow_tbl_list->p_CR_buf_121, addr_offset);

}

static void img_cfg_update_motion_fusion_step3_flow_tbl_list(const int64 addr_offset, amba_ik_motion_a_flow_tables_list_t *p_flow_tbl_list)
{
    p_flow_tbl_list->p_CR_buf_4  = ik_address_add_offset(p_flow_tbl_list->p_CR_buf_4, addr_offset);
    p_flow_tbl_list->p_CR_buf_5  = ik_address_add_offset(p_flow_tbl_list->p_CR_buf_5, addr_offset);
    p_flow_tbl_list->p_CR_buf_6  = ik_address_add_offset(p_flow_tbl_list->p_CR_buf_6, addr_offset);
    p_flow_tbl_list->p_CR_buf_7  = ik_address_add_offset(p_flow_tbl_list->p_CR_buf_7, addr_offset);
    p_flow_tbl_list->p_CR_buf_8  = ik_address_add_offset(p_flow_tbl_list->p_CR_buf_8, addr_offset);
    p_flow_tbl_list->p_CR_buf_9  = ik_address_add_offset(p_flow_tbl_list->p_CR_buf_9, addr_offset);
    p_flow_tbl_list->p_CR_buf_10 = ik_address_add_offset(p_flow_tbl_list->p_CR_buf_10, addr_offset);
    p_flow_tbl_list->p_CR_buf_11 = ik_address_add_offset(p_flow_tbl_list->p_CR_buf_11, addr_offset);
    p_flow_tbl_list->p_CR_buf_12 = ik_address_add_offset(p_flow_tbl_list->p_CR_buf_12, addr_offset);
    p_flow_tbl_list->p_CR_buf_13 = ik_address_add_offset(p_flow_tbl_list->p_CR_buf_13, addr_offset);
    p_flow_tbl_list->p_CR_buf_14 = ik_address_add_offset(p_flow_tbl_list->p_CR_buf_14, addr_offset);
    p_flow_tbl_list->p_CR_buf_15 = ik_address_add_offset(p_flow_tbl_list->p_CR_buf_15, addr_offset);
    p_flow_tbl_list->p_CR_buf_16 = ik_address_add_offset(p_flow_tbl_list->p_CR_buf_16, addr_offset);
    p_flow_tbl_list->p_CR_buf_17 = ik_address_add_offset(p_flow_tbl_list->p_CR_buf_17, addr_offset);
    p_flow_tbl_list->p_CR_buf_18 = ik_address_add_offset(p_flow_tbl_list->p_CR_buf_18, addr_offset);
    p_flow_tbl_list->p_CR_buf_19 = ik_address_add_offset(p_flow_tbl_list->p_CR_buf_19, addr_offset);
    p_flow_tbl_list->p_CR_buf_20 = ik_address_add_offset(p_flow_tbl_list->p_CR_buf_20, addr_offset);
    p_flow_tbl_list->p_CR_buf_21 = ik_address_add_offset(p_flow_tbl_list->p_CR_buf_21, addr_offset);
    p_flow_tbl_list->p_CR_buf_22 = ik_address_add_offset(p_flow_tbl_list->p_CR_buf_22, addr_offset);
    p_flow_tbl_list->p_CR_buf_23 = ik_address_add_offset(p_flow_tbl_list->p_CR_buf_23, addr_offset);
    p_flow_tbl_list->p_CR_buf_24 = ik_address_add_offset(p_flow_tbl_list->p_CR_buf_24, addr_offset);
    p_flow_tbl_list->p_CR_buf_25 = ik_address_add_offset(p_flow_tbl_list->p_CR_buf_25, addr_offset);
    p_flow_tbl_list->p_CR_buf_26 = ik_address_add_offset(p_flow_tbl_list->p_CR_buf_26, addr_offset);
    p_flow_tbl_list->p_CR_buf_27 = ik_address_add_offset(p_flow_tbl_list->p_CR_buf_27, addr_offset);
    p_flow_tbl_list->p_CR_buf_28 = ik_address_add_offset(p_flow_tbl_list->p_CR_buf_28, addr_offset);
    p_flow_tbl_list->p_CR_buf_29 = ik_address_add_offset(p_flow_tbl_list->p_CR_buf_29, addr_offset);
    p_flow_tbl_list->p_CR_buf_30 = ik_address_add_offset(p_flow_tbl_list->p_CR_buf_30, addr_offset);
    p_flow_tbl_list->p_CR_buf_31 = ik_address_add_offset(p_flow_tbl_list->p_CR_buf_31, addr_offset);
    p_flow_tbl_list->p_CR_buf_32 = ik_address_add_offset(p_flow_tbl_list->p_CR_buf_32, addr_offset);
    p_flow_tbl_list->p_CR_buf_33 = ik_address_add_offset(p_flow_tbl_list->p_CR_buf_33, addr_offset);
    p_flow_tbl_list->p_CR_buf_34 = ik_address_add_offset(p_flow_tbl_list->p_CR_buf_34, addr_offset);
    p_flow_tbl_list->p_CR_buf_35 = ik_address_add_offset(p_flow_tbl_list->p_CR_buf_35, addr_offset);
    p_flow_tbl_list->p_CR_buf_36 = ik_address_add_offset(p_flow_tbl_list->p_CR_buf_36, addr_offset);
    p_flow_tbl_list->p_CR_buf_37 = ik_address_add_offset(p_flow_tbl_list->p_CR_buf_37, addr_offset);
    p_flow_tbl_list->p_CR_buf_38 = ik_address_add_offset(p_flow_tbl_list->p_CR_buf_38, addr_offset);
    p_flow_tbl_list->p_CR_buf_39 = ik_address_add_offset(p_flow_tbl_list->p_CR_buf_39, addr_offset);
    p_flow_tbl_list->p_CR_buf_40 = ik_address_add_offset(p_flow_tbl_list->p_CR_buf_40, addr_offset);
    p_flow_tbl_list->p_CR_buf_41 = ik_address_add_offset(p_flow_tbl_list->p_CR_buf_41, addr_offset);

    p_flow_tbl_list->p_CR_buf_111= ik_address_add_offset(p_flow_tbl_list->p_CR_buf_111, addr_offset);
    p_flow_tbl_list->p_CR_buf_112= ik_address_add_offset(p_flow_tbl_list->p_CR_buf_112, addr_offset);
    p_flow_tbl_list->p_CR_buf_113= ik_address_add_offset(p_flow_tbl_list->p_CR_buf_113, addr_offset);
    p_flow_tbl_list->p_CR_buf_114= ik_address_add_offset(p_flow_tbl_list->p_CR_buf_114, addr_offset);
    p_flow_tbl_list->p_CR_buf_115= ik_address_add_offset(p_flow_tbl_list->p_CR_buf_115, addr_offset);
    p_flow_tbl_list->p_CR_buf_116= ik_address_add_offset(p_flow_tbl_list->p_CR_buf_116, addr_offset);
    p_flow_tbl_list->p_CR_buf_117= ik_address_add_offset(p_flow_tbl_list->p_CR_buf_117, addr_offset);
    p_flow_tbl_list->p_CR_buf_118= ik_address_add_offset(p_flow_tbl_list->p_CR_buf_118, addr_offset);
    p_flow_tbl_list->p_CR_buf_119= ik_address_add_offset(p_flow_tbl_list->p_CR_buf_119, addr_offset);
    p_flow_tbl_list->p_CR_buf_120= ik_address_add_offset(p_flow_tbl_list->p_CR_buf_120, addr_offset);
    p_flow_tbl_list->p_CR_buf_121= ik_address_add_offset(p_flow_tbl_list->p_CR_buf_121, addr_offset);

}

static void img_cfg_update_motion_fusion_step4_flow_tbl_list(const int64 addr_offset, amba_ik_motion_b_flow_tables_list_t *p_flow_tbl_list)
{
    p_flow_tbl_list->p_CR_buf_4  = ik_address_add_offset(p_flow_tbl_list->p_CR_buf_4, addr_offset);
    p_flow_tbl_list->p_CR_buf_5  = ik_address_add_offset(p_flow_tbl_list->p_CR_buf_5, addr_offset);
    p_flow_tbl_list->p_CR_buf_6  = ik_address_add_offset(p_flow_tbl_list->p_CR_buf_6, addr_offset);
    p_flow_tbl_list->p_CR_buf_7  = ik_address_add_offset(p_flow_tbl_list->p_CR_buf_7, addr_offset);
    p_flow_tbl_list->p_CR_buf_8  = ik_address_add_offset(p_flow_tbl_list->p_CR_buf_8, addr_offset);
    p_flow_tbl_list->p_CR_buf_9  = ik_address_add_offset(p_flow_tbl_list->p_CR_buf_9, addr_offset);
    p_flow_tbl_list->p_CR_buf_10 = ik_address_add_offset(p_flow_tbl_list->p_CR_buf_10, addr_offset);
    p_flow_tbl_list->p_CR_buf_11 = ik_address_add_offset(p_flow_tbl_list->p_CR_buf_11, addr_offset);
    p_flow_tbl_list->p_CR_buf_12 = ik_address_add_offset(p_flow_tbl_list->p_CR_buf_12, addr_offset);
    p_flow_tbl_list->p_CR_buf_13 = ik_address_add_offset(p_flow_tbl_list->p_CR_buf_13, addr_offset);
    p_flow_tbl_list->p_CR_buf_14 = ik_address_add_offset(p_flow_tbl_list->p_CR_buf_14, addr_offset);
    p_flow_tbl_list->p_CR_buf_15 = ik_address_add_offset(p_flow_tbl_list->p_CR_buf_15, addr_offset);
    p_flow_tbl_list->p_CR_buf_16 = ik_address_add_offset(p_flow_tbl_list->p_CR_buf_16, addr_offset);
    p_flow_tbl_list->p_CR_buf_17 = ik_address_add_offset(p_flow_tbl_list->p_CR_buf_17, addr_offset);
    p_flow_tbl_list->p_CR_buf_18 = ik_address_add_offset(p_flow_tbl_list->p_CR_buf_18, addr_offset);
    p_flow_tbl_list->p_CR_buf_19 = ik_address_add_offset(p_flow_tbl_list->p_CR_buf_19, addr_offset);
    p_flow_tbl_list->p_CR_buf_20 = ik_address_add_offset(p_flow_tbl_list->p_CR_buf_20, addr_offset);
    p_flow_tbl_list->p_CR_buf_21 = ik_address_add_offset(p_flow_tbl_list->p_CR_buf_21, addr_offset);
    p_flow_tbl_list->p_CR_buf_22 = ik_address_add_offset(p_flow_tbl_list->p_CR_buf_22, addr_offset);
    p_flow_tbl_list->p_CR_buf_23 = ik_address_add_offset(p_flow_tbl_list->p_CR_buf_23, addr_offset);
    p_flow_tbl_list->p_CR_buf_24 = ik_address_add_offset(p_flow_tbl_list->p_CR_buf_24, addr_offset);
    p_flow_tbl_list->p_CR_buf_25 = ik_address_add_offset(p_flow_tbl_list->p_CR_buf_25, addr_offset);
    p_flow_tbl_list->p_CR_buf_26 = ik_address_add_offset(p_flow_tbl_list->p_CR_buf_26, addr_offset);
    p_flow_tbl_list->p_CR_buf_27 = ik_address_add_offset(p_flow_tbl_list->p_CR_buf_27, addr_offset);
    p_flow_tbl_list->p_CR_buf_28 = ik_address_add_offset(p_flow_tbl_list->p_CR_buf_28, addr_offset);
    p_flow_tbl_list->p_CR_buf_29 = ik_address_add_offset(p_flow_tbl_list->p_CR_buf_29, addr_offset);
    p_flow_tbl_list->p_CR_buf_30 = ik_address_add_offset(p_flow_tbl_list->p_CR_buf_30, addr_offset);
    p_flow_tbl_list->p_CR_buf_31 = ik_address_add_offset(p_flow_tbl_list->p_CR_buf_31, addr_offset);
    p_flow_tbl_list->p_CR_buf_32 = ik_address_add_offset(p_flow_tbl_list->p_CR_buf_32, addr_offset);
    p_flow_tbl_list->p_CR_buf_33 = ik_address_add_offset(p_flow_tbl_list->p_CR_buf_33, addr_offset);
    p_flow_tbl_list->p_CR_buf_34 = ik_address_add_offset(p_flow_tbl_list->p_CR_buf_34, addr_offset);
    p_flow_tbl_list->p_CR_buf_35 = ik_address_add_offset(p_flow_tbl_list->p_CR_buf_35, addr_offset);
    p_flow_tbl_list->p_CR_buf_36 = ik_address_add_offset(p_flow_tbl_list->p_CR_buf_36, addr_offset);
    p_flow_tbl_list->p_CR_buf_37 = ik_address_add_offset(p_flow_tbl_list->p_CR_buf_37, addr_offset);
    p_flow_tbl_list->p_CR_buf_38 = ik_address_add_offset(p_flow_tbl_list->p_CR_buf_38, addr_offset);
    p_flow_tbl_list->p_CR_buf_39 = ik_address_add_offset(p_flow_tbl_list->p_CR_buf_39, addr_offset);
    p_flow_tbl_list->p_CR_buf_40 = ik_address_add_offset(p_flow_tbl_list->p_CR_buf_40, addr_offset);
    p_flow_tbl_list->p_CR_buf_41 = ik_address_add_offset(p_flow_tbl_list->p_CR_buf_41, addr_offset);

    p_flow_tbl_list->p_CR_buf_111= ik_address_add_offset(p_flow_tbl_list->p_CR_buf_111, addr_offset);
    p_flow_tbl_list->p_CR_buf_112= ik_address_add_offset(p_flow_tbl_list->p_CR_buf_112, addr_offset);
    p_flow_tbl_list->p_CR_buf_113= ik_address_add_offset(p_flow_tbl_list->p_CR_buf_113, addr_offset);
    p_flow_tbl_list->p_CR_buf_114= ik_address_add_offset(p_flow_tbl_list->p_CR_buf_114, addr_offset);
    p_flow_tbl_list->p_CR_buf_115= ik_address_add_offset(p_flow_tbl_list->p_CR_buf_115, addr_offset);
    p_flow_tbl_list->p_CR_buf_116= ik_address_add_offset(p_flow_tbl_list->p_CR_buf_116, addr_offset);
    p_flow_tbl_list->p_CR_buf_117= ik_address_add_offset(p_flow_tbl_list->p_CR_buf_117, addr_offset);
    p_flow_tbl_list->p_CR_buf_118= ik_address_add_offset(p_flow_tbl_list->p_CR_buf_118, addr_offset);
    p_flow_tbl_list->p_CR_buf_119= ik_address_add_offset(p_flow_tbl_list->p_CR_buf_119, addr_offset);
    p_flow_tbl_list->p_CR_buf_120= ik_address_add_offset(p_flow_tbl_list->p_CR_buf_120, addr_offset);
    p_flow_tbl_list->p_CR_buf_121= ik_address_add_offset(p_flow_tbl_list->p_CR_buf_121, addr_offset);

}

static void img_cfg_update_motion_fusion_step5_flow_tbl_list(const int64 addr_offset, amba_ik_motion_c_flow_tables_list_t *p_flow_tbl_list)
{
    p_flow_tbl_list->p_CR_buf_4  = ik_address_add_offset(p_flow_tbl_list->p_CR_buf_4, addr_offset);
    p_flow_tbl_list->p_CR_buf_5  = ik_address_add_offset(p_flow_tbl_list->p_CR_buf_5, addr_offset);
    p_flow_tbl_list->p_CR_buf_6  = ik_address_add_offset(p_flow_tbl_list->p_CR_buf_6, addr_offset);
    p_flow_tbl_list->p_CR_buf_7  = ik_address_add_offset(p_flow_tbl_list->p_CR_buf_7, addr_offset);
    p_flow_tbl_list->p_CR_buf_8  = ik_address_add_offset(p_flow_tbl_list->p_CR_buf_8, addr_offset);
    p_flow_tbl_list->p_CR_buf_9  = ik_address_add_offset(p_flow_tbl_list->p_CR_buf_9, addr_offset);
    p_flow_tbl_list->p_CR_buf_10 = ik_address_add_offset(p_flow_tbl_list->p_CR_buf_10, addr_offset);
    p_flow_tbl_list->p_CR_buf_11 = ik_address_add_offset(p_flow_tbl_list->p_CR_buf_11, addr_offset);
    p_flow_tbl_list->p_CR_buf_12 = ik_address_add_offset(p_flow_tbl_list->p_CR_buf_12, addr_offset);
    p_flow_tbl_list->p_CR_buf_13 = ik_address_add_offset(p_flow_tbl_list->p_CR_buf_13, addr_offset);
    p_flow_tbl_list->p_CR_buf_14 = ik_address_add_offset(p_flow_tbl_list->p_CR_buf_14, addr_offset);
    p_flow_tbl_list->p_CR_buf_15 = ik_address_add_offset(p_flow_tbl_list->p_CR_buf_15, addr_offset);
    p_flow_tbl_list->p_CR_buf_16 = ik_address_add_offset(p_flow_tbl_list->p_CR_buf_16, addr_offset);
    p_flow_tbl_list->p_CR_buf_17 = ik_address_add_offset(p_flow_tbl_list->p_CR_buf_17, addr_offset);
    p_flow_tbl_list->p_CR_buf_18 = ik_address_add_offset(p_flow_tbl_list->p_CR_buf_18, addr_offset);
    p_flow_tbl_list->p_CR_buf_19 = ik_address_add_offset(p_flow_tbl_list->p_CR_buf_19, addr_offset);
    p_flow_tbl_list->p_CR_buf_20 = ik_address_add_offset(p_flow_tbl_list->p_CR_buf_20, addr_offset);
    p_flow_tbl_list->p_CR_buf_21 = ik_address_add_offset(p_flow_tbl_list->p_CR_buf_21, addr_offset);
    p_flow_tbl_list->p_CR_buf_22 = ik_address_add_offset(p_flow_tbl_list->p_CR_buf_22, addr_offset);
    p_flow_tbl_list->p_CR_buf_23 = ik_address_add_offset(p_flow_tbl_list->p_CR_buf_23, addr_offset);
    p_flow_tbl_list->p_CR_buf_24 = ik_address_add_offset(p_flow_tbl_list->p_CR_buf_24, addr_offset);
    p_flow_tbl_list->p_CR_buf_25 = ik_address_add_offset(p_flow_tbl_list->p_CR_buf_25, addr_offset);
    p_flow_tbl_list->p_CR_buf_26 = ik_address_add_offset(p_flow_tbl_list->p_CR_buf_26, addr_offset);
    p_flow_tbl_list->p_CR_buf_27 = ik_address_add_offset(p_flow_tbl_list->p_CR_buf_27, addr_offset);
    p_flow_tbl_list->p_CR_buf_28 = ik_address_add_offset(p_flow_tbl_list->p_CR_buf_28, addr_offset);
    p_flow_tbl_list->p_CR_buf_29 = ik_address_add_offset(p_flow_tbl_list->p_CR_buf_29, addr_offset);
    p_flow_tbl_list->p_CR_buf_30 = ik_address_add_offset(p_flow_tbl_list->p_CR_buf_30, addr_offset);
    p_flow_tbl_list->p_CR_buf_31 = ik_address_add_offset(p_flow_tbl_list->p_CR_buf_31, addr_offset);
    p_flow_tbl_list->p_CR_buf_32 = ik_address_add_offset(p_flow_tbl_list->p_CR_buf_32, addr_offset);
    p_flow_tbl_list->p_CR_buf_33 = ik_address_add_offset(p_flow_tbl_list->p_CR_buf_33, addr_offset);
    p_flow_tbl_list->p_CR_buf_34 = ik_address_add_offset(p_flow_tbl_list->p_CR_buf_34, addr_offset);
    p_flow_tbl_list->p_CR_buf_35 = ik_address_add_offset(p_flow_tbl_list->p_CR_buf_35, addr_offset);
    p_flow_tbl_list->p_CR_buf_36 = ik_address_add_offset(p_flow_tbl_list->p_CR_buf_36, addr_offset);
    p_flow_tbl_list->p_CR_buf_37 = ik_address_add_offset(p_flow_tbl_list->p_CR_buf_37, addr_offset);
    p_flow_tbl_list->p_CR_buf_38 = ik_address_add_offset(p_flow_tbl_list->p_CR_buf_38, addr_offset);
    p_flow_tbl_list->p_CR_buf_39 = ik_address_add_offset(p_flow_tbl_list->p_CR_buf_39, addr_offset);
    p_flow_tbl_list->p_CR_buf_40 = ik_address_add_offset(p_flow_tbl_list->p_CR_buf_40, addr_offset);
    p_flow_tbl_list->p_CR_buf_41 = ik_address_add_offset(p_flow_tbl_list->p_CR_buf_41, addr_offset);

    p_flow_tbl_list->p_CR_buf_111= ik_address_add_offset(p_flow_tbl_list->p_CR_buf_111, addr_offset);
    p_flow_tbl_list->p_CR_buf_112= ik_address_add_offset(p_flow_tbl_list->p_CR_buf_112, addr_offset);
    p_flow_tbl_list->p_CR_buf_113= ik_address_add_offset(p_flow_tbl_list->p_CR_buf_113, addr_offset);
    p_flow_tbl_list->p_CR_buf_114= ik_address_add_offset(p_flow_tbl_list->p_CR_buf_114, addr_offset);
    p_flow_tbl_list->p_CR_buf_115= ik_address_add_offset(p_flow_tbl_list->p_CR_buf_115, addr_offset);
    p_flow_tbl_list->p_CR_buf_116= ik_address_add_offset(p_flow_tbl_list->p_CR_buf_116, addr_offset);
    p_flow_tbl_list->p_CR_buf_117= ik_address_add_offset(p_flow_tbl_list->p_CR_buf_117, addr_offset);
    p_flow_tbl_list->p_CR_buf_118= ik_address_add_offset(p_flow_tbl_list->p_CR_buf_118, addr_offset);
    p_flow_tbl_list->p_CR_buf_119= ik_address_add_offset(p_flow_tbl_list->p_CR_buf_119, addr_offset);
    p_flow_tbl_list->p_CR_buf_120= ik_address_add_offset(p_flow_tbl_list->p_CR_buf_120, addr_offset);
    p_flow_tbl_list->p_CR_buf_121= ik_address_add_offset(p_flow_tbl_list->p_CR_buf_121, addr_offset);

}



static void img_cfg_update_motion_flow_tbl_list(const int64 addr_offset, amba_ik_motion_fusion_flow_tables_list_t *p_flow_tbl_list)
{

    img_cfg_update_motion_fusion_step1_flow_tbl_list(addr_offset, &p_flow_tbl_list->r2y);
    img_cfg_update_motion_fusion_step2_flow_tbl_list(addr_offset, &p_flow_tbl_list->motion_me1);
    img_cfg_update_motion_fusion_step3_flow_tbl_list(addr_offset, &p_flow_tbl_list->motion_a);
    img_cfg_update_motion_fusion_step4_flow_tbl_list(addr_offset, &p_flow_tbl_list->motion_b);
    img_cfg_update_motion_fusion_step5_flow_tbl_list(addr_offset, &p_flow_tbl_list->motion_c);


    p_flow_tbl_list->p_ca_warp_hor_red = ik_address_add_offset(p_flow_tbl_list->p_ca_warp_hor_red,   addr_offset);
    p_flow_tbl_list->p_ca_warp_hor_blue = ik_address_add_offset(p_flow_tbl_list->p_ca_warp_hor_blue,  addr_offset);
    p_flow_tbl_list->p_ca_warp_ver_red = ik_address_add_offset(p_flow_tbl_list->p_ca_warp_ver_red, addr_offset);
    p_flow_tbl_list->p_ca_warp_ver_blue = ik_address_add_offset(p_flow_tbl_list->p_ca_warp_ver_blue, addr_offset);
    p_flow_tbl_list->p_warp_hor = ik_address_add_offset(p_flow_tbl_list->p_warp_hor,  addr_offset);
    p_flow_tbl_list->p_warp_ver = ik_address_add_offset(p_flow_tbl_list->p_warp_ver,  addr_offset);
    p_flow_tbl_list->p_aaa = ik_address_add_offset(p_flow_tbl_list->p_aaa, addr_offset);
    p_flow_tbl_list->p_warp_hor_b = ik_address_add_offset(p_flow_tbl_list->p_warp_hor_b, addr_offset);
    p_flow_tbl_list->p_extra_window = ik_address_add_offset(p_flow_tbl_list->p_extra_window, addr_offset);
    p_flow_tbl_list->p_frame_info = ik_address_add_offset(p_flow_tbl_list->p_frame_info, addr_offset);
}



/****************     Set un-used CR in idsp_step_crs_t to 0     ****************/
// common use api
static void cfg_init_hiso_y2y_sec2_flow_cr_list(idsp_step_crs_t *p_step_cr)
{
    uint32 i;

    // sec2
    for(i=0u; i<22u; i++) {
        p_step_cr->sec2_crs[i].config_region_enable = 0u;
        p_step_cr->sec2_crs[i].config_region_addr = 0u;
    }

    p_step_cr->sec2_crs[24u].config_region_enable = 0u;
    p_step_cr->sec2_crs[24u].config_region_addr = 0u;
}

static void cfg_init_hiso_step1_sec18_flow_cr_list(idsp_step_crs_t *p_step_cr)
{
    uint32 i;

    // sec18
    p_step_cr->sec18_crs[0u].config_region_enable = 0u;
    p_step_cr->sec18_crs[0u].config_region_addr = 0u;
    for(i=2u; i<6u; i++) {
        p_step_cr->sec18_crs[i].config_region_enable = 0u;
        p_step_cr->sec18_crs[i].config_region_addr = 0u;
    }
    for(i=8u; i<=10u; i++) {
        p_step_cr->sec18_crs[i].config_region_enable = 0u;
        p_step_cr->sec18_crs[i].config_region_addr = 0u;
    }
}

static void cfg_init_hiso_step5_sec18_flow_cr_list(idsp_step_crs_t *p_step_cr)
{
    uint32 i;

    // sec18
    p_step_cr->sec18_crs[0u].config_region_enable = 0u;
    p_step_cr->sec18_crs[0u].config_region_addr = 0u;

    p_step_cr->sec18_crs[2u].config_region_enable = 0u;
    p_step_cr->sec18_crs[2u].config_region_addr = 0u;
    for(i=4u; i<6u; i++) {
        p_step_cr->sec18_crs[i].config_region_enable = 0u;
        p_step_cr->sec18_crs[i].config_region_addr = 0u;
    }
    for(i=8u; i<=10u; i++) {
        p_step_cr->sec18_crs[i].config_region_enable = 0u;
        p_step_cr->sec18_crs[i].config_region_addr = 0u;
    }
}

// every steps api
static void cfg_init_hiso_step1_flow_cr_list(idsp_step_crs_t *p_step_cr)
{
    cfg_init_hiso_step1_sec18_flow_cr_list(p_step_cr);
}

static void cfg_init_hiso_step2_flow_cr_list(idsp_step_crs_t *p_step_cr)
{
    cfg_init_hiso_y2y_sec2_flow_cr_list(p_step_cr);
    p_step_cr->sec2_crs[30u].config_region_enable = 0u;
    p_step_cr->sec2_crs[30u].config_region_addr   = 0u;

    // sec18
    cfg_init_hiso_step1_sec18_flow_cr_list(p_step_cr);
}

static void cfg_init_hiso_step3_flow_cr_list(idsp_step_crs_t *p_step_cr)
{
    // sec2
    cfg_init_hiso_y2y_sec2_flow_cr_list(p_step_cr);
    p_step_cr->sec2_crs[23u].config_region_enable = 0u;
    p_step_cr->sec2_crs[23u].config_region_addr = 0u;

    p_step_cr->sec2_crs[30u].config_region_enable = 0u;
    p_step_cr->sec2_crs[30u].config_region_addr = 0u;

    // sec18
    cfg_init_hiso_step1_sec18_flow_cr_list(p_step_cr);
}

static void cfg_init_hiso_step4a_flow_cr_list(idsp_step_crs_t *p_step_cr)
{
    // sec2
    cfg_init_hiso_y2y_sec2_flow_cr_list(p_step_cr);
    p_step_cr->sec2_crs[23u].config_region_enable = 0u;
    p_step_cr->sec2_crs[23u].config_region_addr = 0u;

    p_step_cr->sec2_crs[30u].config_region_enable = 0u;
    p_step_cr->sec2_crs[30u].config_region_addr = 0u;
}

static void cfg_init_hiso_step5_flow_cr_list(idsp_step_crs_t *p_step_cr)
{
    // sec2
    cfg_init_hiso_y2y_sec2_flow_cr_list(p_step_cr);
    p_step_cr->sec2_crs[23u].config_region_enable = 0u;
    p_step_cr->sec2_crs[23u].config_region_addr = 0u;

    p_step_cr->sec2_crs[30u].config_region_enable = 0u;
    p_step_cr->sec2_crs[30u].config_region_addr = 0u;

    cfg_init_hiso_step5_sec18_flow_cr_list(p_step_cr);
}

static void cfg_init_hiso_step6_flow_cr_list(idsp_step_crs_t *p_step_cr)
{
    // sec2
    cfg_init_hiso_y2y_sec2_flow_cr_list(p_step_cr);
    p_step_cr->sec2_crs[23u].config_region_enable = 0u;
    p_step_cr->sec2_crs[23u].config_region_addr = 0u;

    p_step_cr->sec2_crs[30u].config_region_enable = 0u;
    p_step_cr->sec2_crs[30u].config_region_addr = 0u;

    // sec18
    cfg_init_hiso_step5_sec18_flow_cr_list(p_step_cr);
    p_step_cr->sec18_crs[7u].config_region_enable = 0u;
    p_step_cr->sec18_crs[7u].config_region_addr = 0u;
}

static void cfg_init_hiso_step11_flow_cr_list(idsp_step_crs_t *p_step_cr)
{
    cfg_init_hiso_step5_sec18_flow_cr_list(p_step_cr);
}

static void cfg_init_hiso_step14_flow_cr_list(idsp_step_crs_t *p_step_cr)
{
    // sec18
    cfg_init_hiso_step5_sec18_flow_cr_list(p_step_cr);
    p_step_cr->sec18_crs[3u].config_region_enable = 0u;
    p_step_cr->sec18_crs[3u].config_region_addr = 0u;
    p_step_cr->sec18_crs[7u].config_region_enable = 0u;
    p_step_cr->sec18_crs[7u].config_region_addr = 0u;
}

static uint32 cfg_init_hiso_flow_cr_list(idsp_flow_ctrl_t *p_flow)
{
    uint32 rval = IK_OK;

    cfg_init_hiso_step1_flow_cr_list(&p_flow->step_crs[0]);
    cfg_init_hiso_step2_flow_cr_list(&p_flow->step_crs[1]);
    cfg_init_hiso_step3_flow_cr_list(&p_flow->step_crs[2]);
    cfg_init_hiso_step3_flow_cr_list(&p_flow->step_crs[3]);
    cfg_init_hiso_step4a_flow_cr_list(&p_flow->step_crs[4]);
    cfg_init_hiso_step5_flow_cr_list(&p_flow->step_crs[5]);
    cfg_init_hiso_step6_flow_cr_list(&p_flow->step_crs[6]);
    cfg_init_hiso_step6_flow_cr_list(&p_flow->step_crs[7]);
    cfg_init_hiso_step6_flow_cr_list(&p_flow->step_crs[8]);
    cfg_init_hiso_step4a_flow_cr_list(&p_flow->step_crs[9]);
    cfg_init_hiso_step5_flow_cr_list(&p_flow->step_crs[10]);
    cfg_init_hiso_step11_flow_cr_list(&p_flow->step_crs[11]);
    cfg_init_hiso_step11_flow_cr_list(&p_flow->step_crs[12]);
    cfg_init_hiso_step6_flow_cr_list(&p_flow->step_crs[13]);
#ifndef _HISO_SEC3_IN_STEP13
    cfg_init_hiso_step14_flow_cr_list(&p_flow->step_crs[14]);
#endif

    return rval;
}

static uint32 cfg_init_liso_flow_cr_list(idsp_flow_ctrl_t *p_flow)
{
    uint32 rval = IK_OK;
    uint32 i, j, k;

    for(i=0; i<p_flow->step_info.total_num_of_steps; i++) {
        for(j=(uint32)SECT_CFG_INDEX_SEC2; j<(uint32)SECT_CFG_INDEX_TOTAL; j++) {
            if(((uint32)p_flow->step_info.step_cfg_mask[i] & (1UL << j)) != 0U) {
                switch (j) {
                case (uint32)SECT_CFG_INDEX_SEC2:
                    for(k=0; k<SEC_2_CRS_COUNT; k++) {
                        p_flow->step_crs[i].sec2_crs[k].config_region_enable = 1;
                    }
                    break;
                case (uint32)SECT_CFG_INDEX_SEC3:
                    for(k=0; k<SEC_3_CRS_COUNT; k++) {
                        p_flow->step_crs[i].sec3_crs[k].config_region_enable = 1;
                    }
                    break;
                case (uint32)SECT_CFG_INDEX_SEC4:
                    for(k=0; k<SEC_4_CRS_COUNT; k++) {
                        p_flow->step_crs[i].sec4_crs[k].config_region_enable = 1;
                    }
                    break;
                case (uint32)SECT_CFG_INDEX_SEC11:
                    for(k=0; k<SEC_11_CRS_COUNT; k++) {
                        p_flow->step_crs[i].sec11_crs[k].config_region_enable = 0;  //STILL LISO does not use sec11
                    }
                    break;
                case (uint32)SECT_CFG_INDEX_SEC18:
                default: //misra
                    for(k=0; k<SEC_18_CRS_COUNT; k++) {
                        p_flow->step_crs[i].sec18_crs[k].config_region_enable = 1;
                    }
                    break;
                }
            }
        }
    }

    return rval;

}

static uint32 cfg_init_flow_cr_list(idsp_flow_ctrl_t *p_flow)
{
    uint32 rval = IK_OK;
    uint32 i, j, k;

    for(i=0; i<p_flow->step_info.total_num_of_steps; i++) {
        for(j=(uint32)SECT_CFG_INDEX_SEC2; j<(uint32)SECT_CFG_INDEX_TOTAL; j++) {
            if(((uint32)p_flow->step_info.step_cfg_mask[i] & (1UL << j)) != 0U) {
                switch (j) {
                case (uint32)SECT_CFG_INDEX_SEC2:
                    for(k=0; k<SEC_2_CRS_COUNT; k++) {
                        p_flow->step_crs[i].sec2_crs[k].config_region_enable = 1;
                    }
                    break;
                case (uint32)SECT_CFG_INDEX_SEC3:
                    for(k=0; k<SEC_3_CRS_COUNT; k++) {
                        p_flow->step_crs[i].sec3_crs[k].config_region_enable = 1;
                    }
                    break;
                case (uint32)SECT_CFG_INDEX_SEC4:
                    for(k=0; k<SEC_4_CRS_COUNT; k++) {
                        p_flow->step_crs[i].sec4_crs[k].config_region_enable = 1;
                    }
                    break;
                case (uint32)SECT_CFG_INDEX_SEC11:
                    for(k=0; k<SEC_11_CRS_COUNT; k++) {
                        p_flow->step_crs[i].sec11_crs[k].config_region_enable = 1;
                    }
                    break;
                case (uint32)SECT_CFG_INDEX_SEC18:
                default: //misra
                    for(k=0; k<SEC_18_CRS_COUNT; k++) {
                        p_flow->step_crs[i].sec18_crs[k].config_region_enable = 1;
                    }
                    break;
                }
            }
        }
    }

    return rval;
}

uint32 img_cfg_init_flow_control(uint32 context_id, idsp_flow_ctrl_t *p_flow, const ik_ability_t *p_ability)
{
    /*struct {
        uint32_t ik_major  : 5;
        uint32_t ik_minor  : 9;
        uint32_t iks_minor : 9;
        uint32_t ikc_minor : 9;
    } ver_t;*/

    uint32 rval = IK_OK;
    uint32 num_of_steps = 0;
    //uint32 ikc_minor;
    //uint32 iks_minor;

    if ((p_flow == NULL) || (p_ability == NULL)) {
        amba_ik_system_print_str_5("[IK][ERROR] img_cfg_init_flow_control() Detect NULL pointer!!", NULL, NULL, NULL, NULL, NULL);
        rval = IK_ERR_0005;
    } else {
        (void)amba_ik_system_memset(p_flow, 0x0, sizeof(idsp_flow_ctrl_t));
        // set step info here
        rval |= img_cfg_set_idsp_step_info(p_ability, &num_of_steps, p_flow->step_info.step_cfg_mask);
        p_flow->step_info.total_num_of_steps = (uint8)num_of_steps;

        // set header
        p_flow->header.ik_cfg_id.pipe = p_ability->pipe;
        p_flow->header.ik_cfg_id.ctx_id = (uint8)context_id;

        p_flow->header.struct_version = (uint32)IK_VERSION_MAJOR;


        if (p_ability->pipe == AMBA_IK_VIDEO_PIPE) {
            p_flow->header.ik_cfg_id.ability = (uint8)p_ability->video_pipe;
            // set cr enable flags
            (void)cfg_init_flow_cr_list(p_flow);
        } else if (p_ability->pipe == AMBA_IK_STILL_PIPE) {
            p_flow->header.ik_cfg_id.ability = (uint8)p_ability->still_pipe;
            // set cr enable flags
            if(p_ability->still_pipe == AMBA_IK_STILL_PIPE_HISO) {
                (void)cfg_init_flow_cr_list(p_flow);
                (void)cfg_init_hiso_flow_cr_list(p_flow);
            } else if(p_ability->still_pipe == AMBA_IK_STILL_PIPE_LISO) {
                (void)cfg_init_liso_flow_cr_list(p_flow);
            } else {
                // misra
            }
        } else {
            // misra
        }
    }

    return rval;
}

static uint32 cfg_init_external_tbl(amba_ik_flow_tables_t *p_video_flow_tbl)
{
    uint32 rval = IK_OK;

    (void)amba_ik_system_memset(p_video_flow_tbl->ca_warp_hor_red, 0x0, sizeof(p_video_flow_tbl->ca_warp_hor_red));
    (void)amba_ik_system_memset(p_video_flow_tbl->ca_warp_hor_blue, 0x0, sizeof(p_video_flow_tbl->ca_warp_hor_blue));
    (void)amba_ik_system_memset(p_video_flow_tbl->ca_warp_ver_red, 0x0, sizeof(p_video_flow_tbl->ca_warp_ver_red));
    (void)amba_ik_system_memset(p_video_flow_tbl->ca_warp_ver_blue, 0x0, sizeof(p_video_flow_tbl->ca_warp_ver_blue));
    (void)amba_ik_system_memset(p_video_flow_tbl->warp_hor, 0x0, sizeof(p_video_flow_tbl->warp_hor));
    (void)amba_ik_system_memset(p_video_flow_tbl->warp_ver, 0x0, sizeof(p_video_flow_tbl->warp_ver));
    (void)amba_ik_system_memset(p_video_flow_tbl->warp_hor_b, 0x0, sizeof(p_video_flow_tbl->warp_hor_b));
    (void)amba_ik_system_memset(p_video_flow_tbl->aaa, 0x0, sizeof(p_video_flow_tbl->aaa));

    return rval;
}

uint32 img_cfg_init_flow_tbl(void *p_flow_tbl, const void *p_flow_tbl_list,
                             const ik_ability_t *p_ability, const void *p_bin_data_dram_addr)
{
    uint32 rval = IK_OK;
    void* tmp_ptr;
    amba_ik_flow_tables_t *p_video_flow_tbl = NULL;
    amba_ik_flow_tables_list_t *p_video_flow_tbl_list = NULL;
    amba_ik_flow_tables_t *p_still_flow_tbl = NULL;
    amba_ik_flow_tables_list_t *p_still_flow_tbl_list = NULL;
    amba_ik_motion_fusion_flow_tables_t *p_video_motion_fusion_flow_tbl = NULL;
    amba_ik_motion_fusion_flow_tables_list_t *p_video_motion_fusion_flow_tbl_list = NULL;
    amba_ik_hiso_flow_tables_list_t *p_hiso_flow_tbl_list = NULL;

    if ((p_flow_tbl == NULL) || (p_ability == NULL)) {
        amba_ik_system_print_str_5("[IK][ERROR] img_cfg_init_flow_tbl() detect NULL pointer", NULL, NULL, NULL, NULL, NULL);
        rval |= IK_ERR_0005;
    } else {
        if (p_ability->pipe == AMBA_IK_VIDEO_PIPE) {
            (void)amba_ik_system_memcpy(&p_video_flow_tbl, &p_flow_tbl, sizeof(void *));
            (void)amba_ik_system_memcpy(&p_video_flow_tbl_list, &p_flow_tbl_list, sizeof(void *));
            if((p_ability->video_pipe == AMBA_IK_VIDEO_PIPE_LINEAR) ||
               (p_ability->video_pipe == AMBA_IK_VIDEO_PIPE_Y2Y) ||
               (p_ability->video_pipe == AMBA_IK_VIDEO_PIPE_LINEAR_HVH) ||
               (p_ability->video_pipe == AMBA_IK_VIDEO_PIPE_Y2Y_HVH) ||
               (p_ability->video_pipe == AMBA_IK_VIDEO_PIPE_LINEAR_HHB) ||
               (p_ability->video_pipe == AMBA_IK_VIDEO_PIPE_Y2Y_HHB)) {
                   (void)amba_ik_system_memcpy(&tmp_ptr, &p_video_flow_tbl_list->r2y.p_CR_buf_4, sizeof(void*));
                   (void)img_cfg_compose_sec2_cfg(tmp_ptr, p_bin_data_dram_addr);
                   (void)amba_ik_system_memcpy(&tmp_ptr, &p_video_flow_tbl_list->r2y.p_CR_buf_42, sizeof(void*));
                   (void)img_cfg_compose_sec3_cfg(tmp_ptr, p_bin_data_dram_addr);
                   (void)amba_ik_system_memcpy(&tmp_ptr, &p_video_flow_tbl_list->r2y.p_CR_buf_100, sizeof(void*));
                   (void)img_cfg_compose_sec11_cfg(tmp_ptr, p_bin_data_dram_addr);
                   (void)amba_ik_system_memcpy(&tmp_ptr, &p_video_flow_tbl_list->r2y.p_CR_buf_111, sizeof(void*));
                   (void)img_cfg_compose_sec18_cfg(tmp_ptr, p_bin_data_dram_addr);
               (void)cfg_init_external_tbl(p_video_flow_tbl);
            } else if ((p_ability->video_pipe == AMBA_IK_VIDEO_PIPE_LINEAR_CE) ||
                       (p_ability->video_pipe == AMBA_IK_VIDEO_PIPE_HDR_EXPO_2) ||
                       (p_ability->video_pipe == AMBA_IK_VIDEO_PIPE_HDR_EXPO_3) ||
                       (p_ability->video_pipe == AMBA_IK_VIDEO_PIPE_LINEAR_CE_HVH) ||
                       (p_ability->video_pipe == AMBA_IK_VIDEO_PIPE_HDR_EXPO_2_HVH) ||
                       (p_ability->video_pipe == AMBA_IK_VIDEO_PIPE_HDR_EXPO_3_HVH) ||
                       (p_ability->video_pipe == AMBA_IK_VIDEO_PIPE_LINEAR_CE_HHB) ||
                       (p_ability->video_pipe == AMBA_IK_VIDEO_PIPE_HDR_EXPO_2_HHB) ||
                       (p_ability->video_pipe == AMBA_IK_VIDEO_PIPE_HDR_EXPO_3_HHB)) {
                    (void)amba_ik_system_memcpy(&tmp_ptr, &p_video_flow_tbl_list->r2y.p_CR_buf_4, sizeof(void*));
                    (void)img_cfg_compose_sec2_cfg(tmp_ptr, p_bin_data_dram_addr);
                    (void)amba_ik_system_memcpy(&tmp_ptr, &p_video_flow_tbl_list->r2y.p_CR_buf_42, sizeof(void*));
                    (void)img_cfg_compose_sec3_cfg(tmp_ptr, p_bin_data_dram_addr);
                    (void)amba_ik_system_memcpy(&tmp_ptr, &p_video_flow_tbl_list->r2y.p_CR_buf_45, sizeof(void*));
                    (void)img_cfg_compose_sec4_cfg(tmp_ptr, p_bin_data_dram_addr);
                    (void)amba_ik_system_memcpy(&tmp_ptr, &p_video_flow_tbl_list->r2y.p_CR_buf_100, sizeof(void*));
                    (void)img_cfg_compose_sec11_cfg(tmp_ptr, p_bin_data_dram_addr);
                    (void)amba_ik_system_memcpy(&tmp_ptr, &p_video_flow_tbl_list->r2y.p_CR_buf_111, sizeof(void*));
                    (void)img_cfg_compose_sec18_cfg(tmp_ptr, p_bin_data_dram_addr);
                    (void)cfg_init_external_tbl(p_video_flow_tbl);
            } else if ((p_ability->video_pipe == AMBA_IK_VIDEO_PIPE_LINEAR_MD) ||
                       (p_ability->video_pipe == AMBA_IK_VIDEO_PIPE_LINEAR_CE_MD) ||
                       (p_ability->video_pipe == AMBA_IK_VIDEO_PIPE_HDR_EXPO_2_MD) ||
                       (p_ability->video_pipe == AMBA_IK_VIDEO_PIPE_HDR_EXPO_3_MD) ||
                       (p_ability->video_pipe == AMBA_IK_VIDEO_PIPE_FUSION) ||
                       (p_ability->video_pipe == AMBA_IK_VIDEO_PIPE_Y2Y_MD) ||
                       (p_ability->video_pipe == AMBA_IK_VIDEO_PIPE_LINEAR_MD_HHB) ||
                       (p_ability->video_pipe == AMBA_IK_VIDEO_PIPE_LINEAR_CE_MD_HHB) ||
                       (p_ability->video_pipe == AMBA_IK_VIDEO_PIPE_HDR_EXPO_2_MD_HHB) ||
                       (p_ability->video_pipe == AMBA_IK_VIDEO_PIPE_HDR_EXPO_3_MD_HHB) ||
                       (p_ability->video_pipe == AMBA_IK_VIDEO_PIPE_Y2Y_MD_HHB)) {
                    (void)amba_ik_system_memcpy(&p_video_motion_fusion_flow_tbl, &p_flow_tbl, sizeof(void *));
                    (void)amba_ik_system_memcpy(&p_video_motion_fusion_flow_tbl_list, &p_flow_tbl_list, sizeof(void *));
                    (void)img_cfg_compose_motion_cfg(p_video_motion_fusion_flow_tbl,p_video_motion_fusion_flow_tbl_list, p_bin_data_dram_addr);
            } else {
                amba_ik_system_print_uint32_5("[IK] Error, ability %d is not support now\n", p_ability->video_pipe, DC_U, DC_U, DC_U, DC_U);
                rval = IK_ERR_0008;
            }
        } else if (p_ability->pipe == AMBA_IK_STILL_PIPE) {
            if(p_ability->still_pipe < AMBA_IK_STILL_PIPE_MAX) {
                if(p_ability->still_pipe == AMBA_IK_STILL_PIPE_HISO) {
                    (void)amba_ik_system_memcpy(&p_hiso_flow_tbl_list, &p_flow_tbl_list, sizeof(void *));
                    (void)img_cfg_compose_hiso_cfg(p_hiso_flow_tbl_list, p_bin_data_dram_addr);
                } else if(p_ability->still_pipe == AMBA_IK_STILL_PIPE_LISO) {
                    (void)amba_ik_system_memcpy(&p_still_flow_tbl, &p_flow_tbl, sizeof(void *));
                    (void)amba_ik_system_memcpy(&p_still_flow_tbl_list, &p_flow_tbl_list, sizeof(void *));
                    (void)amba_ik_system_memcpy(&tmp_ptr, &p_still_flow_tbl_list->r2y.p_CR_buf_4, sizeof(void*));
                    (void)img_cfg_compose_sec2_cfg(tmp_ptr, p_bin_data_dram_addr);
                    (void)amba_ik_system_memcpy(&tmp_ptr, &p_still_flow_tbl_list->r2y.p_CR_buf_42, sizeof(void*));
                    (void)img_cfg_compose_sec3_cfg(tmp_ptr, p_bin_data_dram_addr);
                    (void)amba_ik_system_memcpy(&tmp_ptr, &p_still_flow_tbl_list->r2y.p_CR_buf_45, sizeof(void*));
                    (void)img_cfg_compose_sec4_cfg(tmp_ptr, p_bin_data_dram_addr);
                    (void)amba_ik_system_memcpy(&tmp_ptr, &p_still_flow_tbl_list->r2y.p_CR_buf_100, sizeof(void*));
                    (void)img_cfg_compose_sec11_cfg(tmp_ptr, p_bin_data_dram_addr);
                    (void)amba_ik_system_memcpy(&tmp_ptr, &p_still_flow_tbl_list->r2y.p_CR_buf_111, sizeof(void*));
                    (void)img_cfg_compose_sec18_cfg(tmp_ptr, p_bin_data_dram_addr);
                    (void)cfg_init_external_tbl(p_still_flow_tbl);
                } else {
                    //MisraC;
                }
            } else {
                amba_ik_system_print_uint32_5("[IK] Error, ability %d is not support now\n", p_ability->still_pipe, DC_U, DC_U, DC_U, DC_U);
                rval = IK_ERR_0008;
            }
        } else {
            //TBD
        }
    }

    return rval;
}

uint32 img_cfg_reinit_flow_tbl_list(void *p_flow_tbl_list, const ik_ability_t *p_ability)
{
    uint32 rval = IK_OK;
    amba_ik_flow_tables_list_t *p_video_flow_tbl_list = NULL;
    amba_ik_motion_fusion_flow_tables_list_t *p_motion_fusion_flow_tbl_list = NULL;
    ik_buffer_info_t *p_ik_buffer_info = NULL;
    int64 addr_offset;

    rval |= img_arch_get_ik_working_buffer(&p_ik_buffer_info);
    addr_offset = p_ik_buffer_info->working_buffer_addr_offset;

    if ((p_flow_tbl_list == NULL) || (p_ability == NULL)) {
        amba_ik_system_print_str_5("[IK][ERROR] img_cfg_reinit_flow_tbl_list() detect NULL pointer", NULL, NULL, NULL, NULL, NULL);
        rval |= IK_ERR_0005;
    } else {
        if (p_ability->pipe == AMBA_IK_VIDEO_PIPE) {
            (void)amba_ik_system_memcpy(&p_video_flow_tbl_list, &p_flow_tbl_list, sizeof(void *));
            (void)amba_ik_system_memcpy(&p_motion_fusion_flow_tbl_list, &p_flow_tbl_list, sizeof(void *));
            if((p_ability->video_pipe == AMBA_IK_VIDEO_PIPE_LINEAR) ||
               (p_ability->video_pipe == AMBA_IK_VIDEO_PIPE_Y2Y) ||
               (p_ability->video_pipe == AMBA_IK_VIDEO_PIPE_LINEAR_HVH) ||
               (p_ability->video_pipe == AMBA_IK_VIDEO_PIPE_Y2Y_HVH) ||
               (p_ability->video_pipe == AMBA_IK_VIDEO_PIPE_LINEAR_HHB) ||
               (p_ability->video_pipe == AMBA_IK_VIDEO_PIPE_Y2Y_HHB)) {
                (void)img_cfg_update_flow_tbl_list(addr_offset, p_video_flow_tbl_list);
                p_video_flow_tbl_list->r2y.p_CR_buf_45 = NULL;
                p_video_flow_tbl_list->r2y.p_CR_buf_46 = NULL;
                p_video_flow_tbl_list->r2y.p_CR_buf_47 = NULL;
                p_video_flow_tbl_list->r2y.p_CR_buf_48 = NULL;
                p_video_flow_tbl_list->r2y.p_CR_buf_49 = NULL;
                p_video_flow_tbl_list->r2y.p_CR_buf_50 = NULL;
                p_video_flow_tbl_list->r2y.p_CR_buf_51 = NULL;
                p_video_flow_tbl_list->r2y.p_CR_buf_52 = NULL;
            } else if ((p_ability->video_pipe == AMBA_IK_VIDEO_PIPE_LINEAR_CE) ||
                       (p_ability->video_pipe == AMBA_IK_VIDEO_PIPE_HDR_EXPO_2) ||
                       (p_ability->video_pipe == AMBA_IK_VIDEO_PIPE_HDR_EXPO_3) ||
                       (p_ability->video_pipe == AMBA_IK_VIDEO_PIPE_LINEAR_CE_HVH) ||
                       (p_ability->video_pipe == AMBA_IK_VIDEO_PIPE_HDR_EXPO_2_HVH) ||
                       (p_ability->video_pipe == AMBA_IK_VIDEO_PIPE_HDR_EXPO_3_HVH) ||
                       (p_ability->video_pipe == AMBA_IK_VIDEO_PIPE_LINEAR_CE_HHB) ||
                       (p_ability->video_pipe == AMBA_IK_VIDEO_PIPE_HDR_EXPO_2_HHB) ||
                       (p_ability->video_pipe == AMBA_IK_VIDEO_PIPE_HDR_EXPO_3_HHB)) {
                (void)img_cfg_update_flow_tbl_list(addr_offset, p_video_flow_tbl_list);
            } else if ((p_ability->video_pipe == AMBA_IK_VIDEO_PIPE_LINEAR_MD) ||
                       (p_ability->video_pipe == AMBA_IK_VIDEO_PIPE_LINEAR_MD_HHB) ||
                       (p_ability->video_pipe == AMBA_IK_VIDEO_PIPE_Y2Y_MD) ||
                       (p_ability->video_pipe == AMBA_IK_VIDEO_PIPE_Y2Y_MD_HHB)) {
                (void)img_cfg_update_motion_flow_tbl_list(addr_offset, p_motion_fusion_flow_tbl_list);
                //p_motion_fusion_flow_tbl_list->r2y.p_CR_buf_42 = NULL;
                //p_motion_fusion_flow_tbl_list->r2y.p_CR_buf_43 = NULL;
                //p_motion_fusion_flow_tbl_list->r2y.p_CR_buf_44 = NULL;
                p_motion_fusion_flow_tbl_list->r2y.p_CR_buf_45 = NULL;
                p_motion_fusion_flow_tbl_list->r2y.p_CR_buf_46 = NULL;
                p_motion_fusion_flow_tbl_list->r2y.p_CR_buf_47 = NULL;
                p_motion_fusion_flow_tbl_list->r2y.p_CR_buf_48 = NULL;
                p_motion_fusion_flow_tbl_list->r2y.p_CR_buf_49 = NULL;
                p_motion_fusion_flow_tbl_list->r2y.p_CR_buf_50 = NULL;
                p_motion_fusion_flow_tbl_list->r2y.p_CR_buf_51 = NULL;
                p_motion_fusion_flow_tbl_list->r2y.p_CR_buf_52 = NULL;
            } else if ((p_ability->video_pipe == AMBA_IK_VIDEO_PIPE_LINEAR_CE_MD) ||
                       (p_ability->video_pipe == AMBA_IK_VIDEO_PIPE_HDR_EXPO_2_MD) ||
                       (p_ability->video_pipe == AMBA_IK_VIDEO_PIPE_HDR_EXPO_3_MD) ||
                       (p_ability->video_pipe == AMBA_IK_VIDEO_PIPE_LINEAR_CE_MD_HHB) ||
                       (p_ability->video_pipe == AMBA_IK_VIDEO_PIPE_HDR_EXPO_2_MD_HHB) ||
                       (p_ability->video_pipe == AMBA_IK_VIDEO_PIPE_HDR_EXPO_3_MD_HHB)) {
                (void)img_cfg_update_motion_flow_tbl_list(addr_offset, p_motion_fusion_flow_tbl_list);
                //p_motion_fusion_flow_tbl_list->r2y.p_CR_buf_42 = NULL;
                //p_motion_fusion_flow_tbl_list->r2y.p_CR_buf_43 = NULL;
                //p_motion_fusion_flow_tbl_list->r2y.p_CR_buf_44 = NULL;
            } else if (p_ability->video_pipe == AMBA_IK_VIDEO_PIPE_FUSION) {
                //(void)img_cfg_populate_motion_flow_tbl_list(p_motion_fusion_flow_tbl, p_motion_fusion_flow_tbl_list);
            } else {
                amba_ik_system_print_uint32_5("[IK] Error, ability %d is not support now\n", p_ability->video_pipe, DC_U, DC_U, DC_U, DC_U);
                rval = IK_ERR_0008;
            }
        } else if (p_ability->pipe == AMBA_IK_STILL_PIPE) {
            if(p_ability->still_pipe < AMBA_IK_STILL_PIPE_MAX) {
                //TBD
            } else {
                amba_ik_system_print_uint32_5("[IK] Error, ability %d is not support now\n", p_ability->still_pipe, DC_U, DC_U, DC_U, DC_U);
                rval = IK_ERR_0008;
            }
        } else {
            //TBD
        }
    }

    return rval;

}

uint32 img_cfg_init_null_flow_tbl_list(void *p_flow_tbl_list, const ik_ability_t *p_ability)
{
    uint32 rval = IK_OK;
    amba_ik_motion_fusion_flow_tables_list_t *p_motion_fusion_flow_tbl_list = NULL;

    if ((p_flow_tbl_list == NULL) || (p_ability == NULL)) {
        amba_ik_system_print_str_5("[IK][ERROR] img_cfg_init_null_flow_tbl_list() detect NULL pointer", NULL, NULL, NULL, NULL, NULL);
        rval |= IK_ERR_0005;
    } else {
        if ((p_ability->video_pipe == AMBA_IK_VIDEO_PIPE_LINEAR_MD) ||
            (p_ability->video_pipe == AMBA_IK_VIDEO_PIPE_LINEAR_MD_HHB) ||
            (p_ability->video_pipe == AMBA_IK_VIDEO_PIPE_Y2Y_MD) ||
            (p_ability->video_pipe == AMBA_IK_VIDEO_PIPE_Y2Y_MD_HHB)) {
                (void)amba_ik_system_memcpy(&p_motion_fusion_flow_tbl_list, &p_flow_tbl_list, sizeof(void *));
                //p_motion_fusion_flow_tbl_list->r2y.p_CR_buf_42 = NULL;
                //p_motion_fusion_flow_tbl_list->r2y.p_CR_buf_43 = NULL;
                //p_motion_fusion_flow_tbl_list->r2y.p_CR_buf_44 = NULL;
                p_motion_fusion_flow_tbl_list->r2y.p_CR_buf_45 = NULL;
                p_motion_fusion_flow_tbl_list->r2y.p_CR_buf_46 = NULL;
                p_motion_fusion_flow_tbl_list->r2y.p_CR_buf_47 = NULL;
                p_motion_fusion_flow_tbl_list->r2y.p_CR_buf_48 = NULL;
                p_motion_fusion_flow_tbl_list->r2y.p_CR_buf_49 = NULL;
                p_motion_fusion_flow_tbl_list->r2y.p_CR_buf_50 = NULL;
                p_motion_fusion_flow_tbl_list->r2y.p_CR_buf_51 = NULL;
                p_motion_fusion_flow_tbl_list->r2y.p_CR_buf_52 = NULL;
        }
    }

    return rval;
}

uint32 img_cfg_init_flow_tbl_list(const void *p_flow_tbl, void *p_flow_tbl_list, const ik_ability_t *p_ability)
{
    uint32 rval = IK_OK;
    amba_ik_flow_tables_t *p_video_flow_tbl = NULL;
    amba_ik_flow_tables_t *p_still_flow_tbl = NULL;
    amba_ik_flow_tables_list_t *p_video_flow_tbl_list = NULL;
    amba_ik_flow_tables_list_t *p_still_flow_tbl_list = NULL;
    amba_ik_motion_fusion_flow_tables_t *p_motion_fusion_flow_tbl = NULL;
    amba_ik_motion_fusion_flow_tables_list_t *p_motion_fusion_flow_tbl_list = NULL;
    amba_ik_hiso_flow_tables_t *p_hiso_flow_tbl = NULL;
    amba_ik_hiso_flow_tables_list_t *p_hiso_flow_tbl_list = NULL;

    if ((p_flow_tbl_list == NULL) || (p_ability == NULL)) {
        amba_ik_system_print_str_5("[IK][ERROR] img_cfg_init_flow_tbl_list() detect NULL pointer", NULL, NULL, NULL, NULL, NULL);
        rval |= IK_ERR_0005;
    } else {
        if (p_ability->pipe == AMBA_IK_VIDEO_PIPE) {
            (void)amba_ik_system_memcpy(&p_video_flow_tbl, &p_flow_tbl, sizeof(void *));
            (void)amba_ik_system_memcpy(&p_video_flow_tbl_list, &p_flow_tbl_list, sizeof(void *));
            (void)amba_ik_system_memcpy(&p_motion_fusion_flow_tbl, &p_flow_tbl, sizeof(void *));
            (void)amba_ik_system_memcpy(&p_motion_fusion_flow_tbl_list, &p_flow_tbl_list, sizeof(void *));
            if((p_ability->video_pipe == AMBA_IK_VIDEO_PIPE_LINEAR) ||
               (p_ability->video_pipe == AMBA_IK_VIDEO_PIPE_Y2Y) ||
               (p_ability->video_pipe == AMBA_IK_VIDEO_PIPE_LINEAR_HVH) ||
               (p_ability->video_pipe == AMBA_IK_VIDEO_PIPE_Y2Y_HVH) ||
               (p_ability->video_pipe == AMBA_IK_VIDEO_PIPE_LINEAR_HHB) ||
               (p_ability->video_pipe == AMBA_IK_VIDEO_PIPE_Y2Y_HHB)) {
                (void)img_cfg_populate_flow_tbl_list(p_video_flow_tbl, p_video_flow_tbl_list);
                p_video_flow_tbl_list->r2y.p_CR_buf_45 = NULL;
                p_video_flow_tbl_list->r2y.p_CR_buf_46 = NULL;
                p_video_flow_tbl_list->r2y.p_CR_buf_47 = NULL;
                p_video_flow_tbl_list->r2y.p_CR_buf_48 = NULL;
                p_video_flow_tbl_list->r2y.p_CR_buf_49 = NULL;
                p_video_flow_tbl_list->r2y.p_CR_buf_50 = NULL;
                p_video_flow_tbl_list->r2y.p_CR_buf_51 = NULL;
                p_video_flow_tbl_list->r2y.p_CR_buf_52 = NULL;
            } else if ((p_ability->video_pipe == AMBA_IK_VIDEO_PIPE_LINEAR_CE) ||
                       (p_ability->video_pipe == AMBA_IK_VIDEO_PIPE_HDR_EXPO_2) ||
                       (p_ability->video_pipe == AMBA_IK_VIDEO_PIPE_HDR_EXPO_3) ||
                       (p_ability->video_pipe == AMBA_IK_VIDEO_PIPE_LINEAR_CE_HVH) ||
                       (p_ability->video_pipe == AMBA_IK_VIDEO_PIPE_HDR_EXPO_2_HVH) ||
                       (p_ability->video_pipe == AMBA_IK_VIDEO_PIPE_HDR_EXPO_3_HVH) ||
                       (p_ability->video_pipe == AMBA_IK_VIDEO_PIPE_LINEAR_CE_HHB) ||
                       (p_ability->video_pipe == AMBA_IK_VIDEO_PIPE_HDR_EXPO_2_HHB) ||
                       (p_ability->video_pipe == AMBA_IK_VIDEO_PIPE_HDR_EXPO_3_HHB)) {
                (void)img_cfg_populate_flow_tbl_list(p_video_flow_tbl, p_video_flow_tbl_list);
            } else if ((p_ability->video_pipe == AMBA_IK_VIDEO_PIPE_LINEAR_MD) ||
                       (p_ability->video_pipe == AMBA_IK_VIDEO_PIPE_LINEAR_MD_HHB) ||
                       (p_ability->video_pipe == AMBA_IK_VIDEO_PIPE_Y2Y_MD) ||
                       (p_ability->video_pipe == AMBA_IK_VIDEO_PIPE_Y2Y_MD_HHB)) {
                (void)img_cfg_populate_motion_flow_tbl_list(p_motion_fusion_flow_tbl, p_motion_fusion_flow_tbl_list);
            } else if ((p_ability->video_pipe == AMBA_IK_VIDEO_PIPE_LINEAR_CE_MD) ||
                       (p_ability->video_pipe == AMBA_IK_VIDEO_PIPE_HDR_EXPO_2_MD) ||
                       (p_ability->video_pipe == AMBA_IK_VIDEO_PIPE_HDR_EXPO_3_MD) ||
                       (p_ability->video_pipe == AMBA_IK_VIDEO_PIPE_LINEAR_CE_MD_HHB) ||
                       (p_ability->video_pipe == AMBA_IK_VIDEO_PIPE_HDR_EXPO_2_MD_HHB) ||
                       (p_ability->video_pipe == AMBA_IK_VIDEO_PIPE_HDR_EXPO_3_MD_HHB) ||
                       (p_ability->video_pipe == AMBA_IK_VIDEO_PIPE_FUSION)) {
                (void)img_cfg_populate_motion_flow_tbl_list(p_motion_fusion_flow_tbl, p_motion_fusion_flow_tbl_list);
                //p_motion_fusion_flow_tbl_list->r2y.p_CR_buf_42 = NULL;
                //p_motion_fusion_flow_tbl_list->r2y.p_CR_buf_43 = NULL;
                //p_motion_fusion_flow_tbl_list->r2y.p_CR_buf_44 = NULL;
            } else {
                amba_ik_system_print_uint32_5("[IK] Error, ability %d is not support now\n", p_ability->video_pipe, DC_U, DC_U, DC_U, DC_U);
                rval = IK_ERR_0008;
            }
        } else if (p_ability->pipe == AMBA_IK_STILL_PIPE) {
            if(p_ability->still_pipe < AMBA_IK_STILL_PIPE_MAX) {
                if(p_ability->still_pipe == AMBA_IK_STILL_PIPE_HISO) {
                    (void)amba_ik_system_memcpy(&p_hiso_flow_tbl, &p_flow_tbl, sizeof(void *));
                    (void)amba_ik_system_memcpy(&p_hiso_flow_tbl_list, &p_flow_tbl_list, sizeof(void *));
                    (void)img_cfg_populate_hiso_flow_tbl_list(p_hiso_flow_tbl, p_hiso_flow_tbl_list);
                } else if(p_ability->still_pipe == AMBA_IK_STILL_PIPE_LISO) {
                    (void)amba_ik_system_memcpy(&p_still_flow_tbl, &p_flow_tbl, sizeof(void *));
                    (void)amba_ik_system_memcpy(&p_still_flow_tbl_list, &p_flow_tbl_list, sizeof(void *));
                    (void)img_cfg_populate_flow_tbl_list(p_still_flow_tbl, p_still_flow_tbl_list);
                } else {
                    // misra
                }
            } else {
                amba_ik_system_print_uint32_5("[IK] Error, ability %d is not support now\n", p_ability->still_pipe, DC_U, DC_U, DC_U, DC_U);
                rval = IK_ERR_0008;
            }
        } else {
            //TBD
        }
    }

    return rval;
}

static void* INLINE img_cfg_uint32_physical_to_void_virtual(uint32_t physical_addr)
{
    void *rval;
    void *tmp_void_ptr;
    const void *tmp_void_ptr2;
    (void)amba_ik_system_memset(&tmp_void_ptr, 0x0, sizeof(void*));
    (void)amba_ik_system_memcpy(&tmp_void_ptr, &physical_addr, sizeof(uint32));
    tmp_void_ptr2 = amba_ik_system_phys_to_virt(tmp_void_ptr);
    (void)amba_ik_system_memcpy(&rval, &tmp_void_ptr2, sizeof(void*));
    return rval;
}

uint32 img_cfg_amalgam_data_update(const idsp_flow_ctrl_t *p_flow, const amba_ik_flow_tables_list_t *p_flow_tbl_list, const void *p_user_buffer)
{
    uint32 rval = 0;
    amalgam_flow_data_t *p_amalgam_data;
    uintptr data_offset_base;
    const void *temp_data_addr = NULL;
    uint32_t filter_offset;
    const uint8_t *tmp_p_u8;
    const int16_t *tmp_p_s16;
    uintptr sec2_cfg_buf_offset;
    uintptr sec3_cfg_buf_offset;
    uintptr sec4_cfg_buf_offset;
    uintptr sec11_cfg_buf_offset;
    uintptr sec18_cfg_buf_offset;
    void* p_sec2_cfg_buf;
    void* p_sec3_cfg_buf;
    void* p_sec4_cfg_buf;
    void* p_sec11_cfg_buf;
    void* p_sec18_cfg_buf;
    uintptr tmp_uintptr;

    (void)amba_ik_system_memcpy(&p_amalgam_data, &p_user_buffer, sizeof(amalgam_flow_data_t *));
    (void)amba_ik_system_memcpy(&data_offset_base, &p_amalgam_data, sizeof(uintptr));
    (void)amba_ik_system_memset(p_amalgam_data, 0x0, sizeof(amalgam_flow_data_t));
    (void)amba_ik_system_memcpy(&p_amalgam_data->flow_ctrl, p_flow, sizeof(idsp_flow_ctrl_t));

    if (p_flow->flow_info.sbp_map_addr != 0U) {
        temp_data_addr = img_cfg_uint32_physical_to_void_virtual(p_flow->flow_info.sbp_map_addr);
        (void)amba_ik_system_memcpy(p_amalgam_data->sbp_map, temp_data_addr, IK_AMALGAM_TABLE_SBP_SIZE);
        tmp_p_u8 = p_amalgam_data->sbp_map;
        (void)amba_ik_system_memcpy(&tmp_uintptr, &tmp_p_u8, sizeof(uintptr));
        p_amalgam_data->flow_ctrl.flow_info.sbp_map_addr = (uint32_t)(tmp_uintptr - data_offset_base);
        amba_ik_system_print_uint64_5("sbp_map_addr 0x%08x, src_addr %p ",
                                      p_amalgam_data->flow_ctrl.flow_info.sbp_map_addr, ptr2uintptr(temp_data_addr), DC_U, DC_U, DC_U);
    } else {
        amba_ik_system_print_str_5("[IK] SBP is NULL", NULL, NULL, NULL, NULL, NULL);
    }

    if (p_flow_tbl_list->p_ca_warp_hor_red != NULL) {
        (void)amba_ik_system_memcpy(p_amalgam_data->ca_warp_hor_red, p_flow_tbl_list->p_ca_warp_hor_red, sizeof(p_amalgam_data->ca_warp_hor_red));
        tmp_p_s16 = p_amalgam_data->ca_warp_hor_red;
        (void)amba_ik_system_memcpy(&tmp_uintptr, &tmp_p_s16, sizeof(uintptr));
        p_amalgam_data->flow_ctrl.calib.cawarp_horizontal_table_addr_red = (uint32_t)(tmp_uintptr - data_offset_base);
        amba_ik_system_print_uint64_5("cawarp_horizontal_table_addr_red 0x%08x, src_addr %p ",
                                      p_amalgam_data->flow_ctrl.calib.cawarp_horizontal_table_addr_red, ptr2uintptr(p_flow_tbl_list->p_ca_warp_hor_red), DC_U, DC_U, DC_U);
    } else {
        amba_ik_system_print_str_5("[IK] cawarp_hor_red is NULL", NULL, NULL, NULL, NULL, NULL);
    }

    if (p_flow_tbl_list->p_ca_warp_hor_blue != NULL) {
        (void)amba_ik_system_memcpy(p_amalgam_data->ca_warp_hor_blue, p_flow_tbl_list->p_ca_warp_hor_blue, sizeof(p_amalgam_data->ca_warp_hor_blue));
        tmp_p_s16 = p_amalgam_data->ca_warp_hor_blue;
        (void)amba_ik_system_memcpy(&tmp_uintptr, &tmp_p_s16, sizeof(uintptr));
        p_amalgam_data->flow_ctrl.calib.cawarp_horizontal_table_addr_blue = (uint32_t)(tmp_uintptr - data_offset_base);
        amba_ik_system_print_uint64_5("cawarp_horizontal_table_addr_blue 0x%08x, src_addr %p ",
                                      p_amalgam_data->flow_ctrl.calib.cawarp_horizontal_table_addr_blue, ptr2uintptr(p_flow_tbl_list->p_ca_warp_hor_blue), DC_U, DC_U, DC_U);
    } else {
        amba_ik_system_print_str_5("[IK] cawarp_hor_blue is NULL", NULL, NULL, NULL, NULL, NULL);
    }

    if (p_flow_tbl_list->p_ca_warp_ver_red != NULL) {
        (void)amba_ik_system_memcpy(p_amalgam_data->ca_warp_ver_red, p_flow_tbl_list->p_ca_warp_ver_red, sizeof(p_amalgam_data->ca_warp_ver_red));
        tmp_p_s16 = p_amalgam_data->ca_warp_ver_red;
        (void)amba_ik_system_memcpy(&tmp_uintptr, &tmp_p_s16, sizeof(uintptr));
        p_amalgam_data->flow_ctrl.calib.cawarp_vertical_table_addr_red= (uint32_t)(tmp_uintptr - data_offset_base);
        amba_ik_system_print_uint64_5("cawarp_vertical_table_addr_red 0x%08x, src_addr %p ",
                                      p_amalgam_data->flow_ctrl.calib.cawarp_vertical_table_addr_red, ptr2uintptr(p_flow_tbl_list->p_ca_warp_ver_red), DC_U, DC_U, DC_U);
    } else {
        amba_ik_system_print_str_5("[IK] cawarp_ver_red is NULL", NULL, NULL, NULL, NULL, NULL);
    }

    if (p_flow_tbl_list->p_ca_warp_ver_blue != NULL) {
        (void)amba_ik_system_memcpy(p_amalgam_data->ca_warp_ver_blue, p_flow_tbl_list->p_ca_warp_ver_blue, sizeof(p_amalgam_data->ca_warp_ver_blue));
        tmp_p_s16 = p_amalgam_data->ca_warp_ver_blue;
        (void)amba_ik_system_memcpy(&tmp_uintptr, &tmp_p_s16, sizeof(uintptr));
        p_amalgam_data->flow_ctrl.calib.cawarp_vertical_table_addr_blue = (uint32_t)(tmp_uintptr - data_offset_base);
        amba_ik_system_print_uint64_5("cawarp_vertical_table_addr_blue 0x%08x, src_addr %p ",
                                      p_amalgam_data->flow_ctrl.calib.cawarp_vertical_table_addr_blue, ptr2uintptr(p_flow_tbl_list->p_ca_warp_ver_blue), DC_U, DC_U, DC_U);
    } else {
        amba_ik_system_print_str_5("[IK] cawarp_ver_blue is NULL", NULL, NULL, NULL, NULL, NULL);
    }

    if (p_flow_tbl_list->p_warp_hor != NULL) {
        (void)amba_ik_system_memcpy(p_amalgam_data->warp_hor, p_flow_tbl_list->p_warp_hor, sizeof(p_amalgam_data->warp_hor));
        tmp_p_s16 = p_amalgam_data->warp_hor;
        (void)amba_ik_system_memcpy(&tmp_uintptr, &tmp_p_s16, sizeof(uintptr));
        p_amalgam_data->flow_ctrl.calib.warp_horizontal_table_address = (uint32_t)(tmp_uintptr - data_offset_base);
        amba_ik_system_print_uint64_5("warp_horizontal_table_addr 0x%08x, src_addr %p ",
                                      p_amalgam_data->flow_ctrl.calib.warp_horizontal_table_address, ptr2uintptr(p_flow_tbl_list->p_warp_hor), DC_U, DC_U, DC_U);
    } else {
        amba_ik_system_print_str_5("[IK] warp_hor is NULL", NULL, NULL, NULL, NULL, NULL);
    }

    if (p_flow_tbl_list->p_warp_ver != NULL) {
        (void)amba_ik_system_memcpy(p_amalgam_data->warp_ver, p_flow_tbl_list->p_warp_ver, sizeof(p_amalgam_data->warp_ver));
        tmp_p_s16 = p_amalgam_data->warp_ver;
        (void)amba_ik_system_memcpy(&tmp_uintptr, &tmp_p_s16, sizeof(uintptr));
        p_amalgam_data->flow_ctrl.calib.warp_vertical_table_address = (uint32_t)(tmp_uintptr - data_offset_base);
        amba_ik_system_print_uint64_5("warp_vertical_table_addr 0x%08x, src_addr %p ",
                                      p_amalgam_data->flow_ctrl.calib.warp_vertical_table_address, ptr2uintptr(p_flow_tbl_list->p_warp_ver), DC_U, DC_U, DC_U);
    } else {
        amba_ik_system_print_str_5("[IK] warp_ver is NULL", NULL, NULL, NULL, NULL, NULL);
    }

    if (p_flow_tbl_list->p_warp_hor_b != NULL) {
        (void)amba_ik_system_memcpy(p_amalgam_data->warp_hor_b, p_flow_tbl_list->p_warp_hor_b, sizeof(p_amalgam_data->warp_hor_b));
        tmp_p_s16 = p_amalgam_data->warp_hor_b;
        (void)amba_ik_system_memcpy(&tmp_uintptr, &tmp_p_s16, sizeof(uintptr));
        p_amalgam_data->flow_ctrl.calib.warp_horizontal_b_table_address = (uint32_t)(tmp_uintptr - data_offset_base);
        amba_ik_system_print_uint64_5("warp_horizontal_table_addr 0x%08x, src_addr %p ",
                                      p_amalgam_data->flow_ctrl.calib.warp_horizontal_b_table_address, ptr2uintptr(p_flow_tbl_list->p_warp_hor_b), DC_U, DC_U, DC_U);
    } else {
        amba_ik_system_print_str_5("[IK] warp_hor is NULL", NULL, NULL, NULL, NULL, NULL);
    }

    if (p_flow_tbl_list->p_aaa != NULL) {
        const idsp_aaa_data_t *p_tmp_aaa;
        (void)amba_ik_system_memcpy(p_amalgam_data->aaa, p_flow_tbl_list->p_aaa, sizeof(p_amalgam_data->aaa));
        p_tmp_aaa = p_amalgam_data->aaa[0];
        (void)amba_ik_system_memcpy(&tmp_uintptr, &p_tmp_aaa, sizeof(uintptr));
        p_amalgam_data->flow_ctrl.stitch.aaa_info_daddr = (uint32_t)(tmp_uintptr - data_offset_base);
        amba_ik_system_print_uint64_5("aaa_info_daddr 0x%08x, src_addr %p ",
                                      p_amalgam_data->flow_ctrl.stitch.aaa_info_daddr, ptr2uintptr(p_flow_tbl_list->p_aaa), DC_U, DC_U, DC_U);
    } else {
        amba_ik_system_print_str_5("[IK] AAA is NULL", NULL, NULL, NULL, NULL, NULL);
    }

    if (p_flow_tbl_list->p_extra_window != NULL) {
        const idsp_extra_window_info_t *p_tmp_extra_win;
        (void)amba_ik_system_memcpy(&p_amalgam_data->extra_window_info, p_flow_tbl_list->p_extra_window, sizeof(idsp_extra_window_info_t));
        p_tmp_extra_win = &p_amalgam_data->extra_window_info;
        (void)amba_ik_system_memcpy(&tmp_uintptr, &p_tmp_extra_win, sizeof(uintptr));
        p_amalgam_data->flow_ctrl.window.extra_window_info_addr = (uint32_t)(tmp_uintptr - data_offset_base);
        amba_ik_system_print_uint64_5("extra_window_info_addr 0x%08x, src_addr %p ",
                                      p_amalgam_data->flow_ctrl.window.extra_window_info_addr, ptr2uintptr(p_flow_tbl_list->p_extra_window), DC_U, DC_U, DC_U);
    } else {
        amba_ik_system_print_str_5("[IK] extra_window_info_addr is NULL", NULL, NULL, NULL, NULL, NULL);
    }

    // IKC does not use IDSP_CONFIG_HDR_SIZE, however, IKS does, in order to keep it on the same page between IKC and IKS,
    tmp_p_u8 = p_amalgam_data->sec2_cfg_buf;
    (void)amba_ik_system_memcpy(&sec2_cfg_buf_offset, &tmp_p_u8, sizeof(tmp_uintptr));
    tmp_p_u8 = p_amalgam_data->sec3_cfg_buf;
    (void)amba_ik_system_memcpy(&sec3_cfg_buf_offset, &tmp_p_u8, sizeof(tmp_uintptr));
    tmp_p_u8 = p_amalgam_data->sec4_cfg_buf;
    (void)amba_ik_system_memcpy(&sec4_cfg_buf_offset, &tmp_p_u8, sizeof(tmp_uintptr));
    tmp_p_u8 = p_amalgam_data->sec11_cfg_buf;
    (void)amba_ik_system_memcpy(&sec11_cfg_buf_offset, &tmp_p_u8, sizeof(tmp_uintptr));
    tmp_p_u8 = p_amalgam_data->sec18_cfg_buf;
    (void)amba_ik_system_memcpy(&sec18_cfg_buf_offset, &tmp_p_u8, sizeof(tmp_uintptr));

    filter_offset = CR_OFFSET_4;
    tmp_uintptr = sec2_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec2_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec2_cfg_buf, p_flow_tbl_list->r2y.p_CR_buf_4, CR_SIZE_4);
    p_amalgam_data->flow_ctrl.step_crs[0].sec2_crs[0].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec2 cr[0] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_amalgam_data->flow_ctrl.step_crs[0].sec2_crs[0].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->r2y.p_CR_buf_4), DC_U, DC_U);

    filter_offset = CR_OFFSET_5;
    tmp_uintptr = sec2_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec2_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec2_cfg_buf, p_flow_tbl_list->r2y.p_CR_buf_5, CR_SIZE_5);
    p_amalgam_data->flow_ctrl.step_crs[0].sec2_crs[1].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec2 cr[1] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_amalgam_data->flow_ctrl.step_crs[0].sec2_crs[1].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->r2y.p_CR_buf_5), DC_U, DC_U);

    filter_offset = CR_OFFSET_6;
    tmp_uintptr = sec2_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec2_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec2_cfg_buf, p_flow_tbl_list->r2y.p_CR_buf_6, CR_SIZE_6);
    p_amalgam_data->flow_ctrl.step_crs[0].sec2_crs[2].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec2 cr[2] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_amalgam_data->flow_ctrl.step_crs[0].sec2_crs[2].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->r2y.p_CR_buf_6), DC_U, DC_U);

    filter_offset = CR_OFFSET_7;
    tmp_uintptr = sec2_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec2_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec2_cfg_buf, p_flow_tbl_list->r2y.p_CR_buf_7, CR_SIZE_7);
    p_amalgam_data->flow_ctrl.step_crs[0].sec2_crs[3].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec2 cr[3] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_amalgam_data->flow_ctrl.step_crs[0].sec2_crs[3].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->r2y.p_CR_buf_7), DC_U, DC_U);

    filter_offset = CR_OFFSET_8;
    tmp_uintptr = sec2_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec2_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec2_cfg_buf, p_flow_tbl_list->r2y.p_CR_buf_8, CR_SIZE_8);
    p_amalgam_data->flow_ctrl.step_crs[0].sec2_crs[4].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec2 cr[4] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_amalgam_data->flow_ctrl.step_crs[0].sec2_crs[4].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->r2y.p_CR_buf_8), DC_U, DC_U);

    filter_offset = CR_OFFSET_9;
    tmp_uintptr = sec2_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec2_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec2_cfg_buf, p_flow_tbl_list->r2y.p_CR_buf_9, CR_SIZE_9);
    p_amalgam_data->flow_ctrl.step_crs[0].sec2_crs[5].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec2 cr[5] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_amalgam_data->flow_ctrl.step_crs[0].sec2_crs[5].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->r2y.p_CR_buf_9), DC_U, DC_U);

    filter_offset = CR_OFFSET_10;
    tmp_uintptr = sec2_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec2_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec2_cfg_buf, p_flow_tbl_list->r2y.p_CR_buf_10, CR_SIZE_10);
    p_amalgam_data->flow_ctrl.step_crs[0].sec2_crs[6].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec2 cr[6] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_amalgam_data->flow_ctrl.step_crs[0].sec2_crs[6].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->r2y.p_CR_buf_10), DC_U, DC_U);

    filter_offset = CR_OFFSET_11;
    tmp_uintptr = sec2_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec2_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec2_cfg_buf, p_flow_tbl_list->r2y.p_CR_buf_11, CR_SIZE_11);
    p_amalgam_data->flow_ctrl.step_crs[0].sec2_crs[7].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec2 cr[7] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_amalgam_data->flow_ctrl.step_crs[0].sec2_crs[7].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->r2y.p_CR_buf_11), DC_U, DC_U);

    filter_offset = CR_OFFSET_12;
    tmp_uintptr = sec2_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec2_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec2_cfg_buf, p_flow_tbl_list->r2y.p_CR_buf_12, CR_SIZE_12);
    p_amalgam_data->flow_ctrl.step_crs[0].sec2_crs[8].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec2 cr[8] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_amalgam_data->flow_ctrl.step_crs[0].sec2_crs[8].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->r2y.p_CR_buf_12), DC_U, DC_U);

    filter_offset = CR_OFFSET_13;
    tmp_uintptr = sec2_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec2_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec2_cfg_buf, p_flow_tbl_list->r2y.p_CR_buf_13, CR_SIZE_13);
    p_amalgam_data->flow_ctrl.step_crs[0].sec2_crs[9].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec2 cr[9] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_amalgam_data->flow_ctrl.step_crs[0].sec2_crs[9].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->r2y.p_CR_buf_13), DC_U, DC_U);

    filter_offset = CR_OFFSET_14;
    tmp_uintptr = sec2_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec2_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec2_cfg_buf, p_flow_tbl_list->r2y.p_CR_buf_14, CR_SIZE_14);
    p_amalgam_data->flow_ctrl.step_crs[0].sec2_crs[10].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec2 cr[10] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_amalgam_data->flow_ctrl.step_crs[0].sec2_crs[10].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->r2y.p_CR_buf_14), DC_U, DC_U);

    filter_offset = CR_OFFSET_15;
    tmp_uintptr = sec2_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec2_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec2_cfg_buf, p_flow_tbl_list->r2y.p_CR_buf_15, CR_SIZE_15);
    p_amalgam_data->flow_ctrl.step_crs[0].sec2_crs[11].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec2 cr[11] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_amalgam_data->flow_ctrl.step_crs[0].sec2_crs[11].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->r2y.p_CR_buf_15), DC_U, DC_U);

    filter_offset = CR_OFFSET_16;
    tmp_uintptr = sec2_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec2_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec2_cfg_buf, p_flow_tbl_list->r2y.p_CR_buf_16, CR_SIZE_16);
    p_amalgam_data->flow_ctrl.step_crs[0].sec2_crs[12].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec2 cr[12] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_amalgam_data->flow_ctrl.step_crs[0].sec2_crs[12].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->r2y.p_CR_buf_16), DC_U, DC_U);

    filter_offset = CR_OFFSET_17;
    tmp_uintptr = sec2_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec2_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec2_cfg_buf, p_flow_tbl_list->r2y.p_CR_buf_17, CR_SIZE_17);
    p_amalgam_data->flow_ctrl.step_crs[0].sec2_crs[13].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec2 cr[13] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_amalgam_data->flow_ctrl.step_crs[0].sec2_crs[13].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->r2y.p_CR_buf_17), DC_U, DC_U);

    filter_offset = CR_OFFSET_18;
    tmp_uintptr = sec2_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec2_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec2_cfg_buf, p_flow_tbl_list->r2y.p_CR_buf_18, CR_SIZE_18);
    p_amalgam_data->flow_ctrl.step_crs[0].sec2_crs[14].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec2 cr[14] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_amalgam_data->flow_ctrl.step_crs[0].sec2_crs[14].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->r2y.p_CR_buf_18), DC_U, DC_U);

    filter_offset = CR_OFFSET_19;
    tmp_uintptr = sec2_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec2_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec2_cfg_buf, p_flow_tbl_list->r2y.p_CR_buf_19, CR_SIZE_19);
    p_amalgam_data->flow_ctrl.step_crs[0].sec2_crs[15].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec2 cr[15] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_amalgam_data->flow_ctrl.step_crs[0].sec2_crs[15].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->r2y.p_CR_buf_19), DC_U, DC_U);

    filter_offset = CR_OFFSET_20;
    tmp_uintptr = sec2_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec2_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec2_cfg_buf, p_flow_tbl_list->r2y.p_CR_buf_20, CR_SIZE_20);
    p_amalgam_data->flow_ctrl.step_crs[0].sec2_crs[16].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec2 cr[16] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_amalgam_data->flow_ctrl.step_crs[0].sec2_crs[16].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->r2y.p_CR_buf_20), DC_U, DC_U);

    filter_offset = CR_OFFSET_21;
    tmp_uintptr = sec2_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec2_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec2_cfg_buf, p_flow_tbl_list->r2y.p_CR_buf_21, CR_SIZE_21);
    p_amalgam_data->flow_ctrl.step_crs[0].sec2_crs[17].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec2 cr[17] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_amalgam_data->flow_ctrl.step_crs[0].sec2_crs[17].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->r2y.p_CR_buf_21), DC_U, DC_U);

    filter_offset = CR_OFFSET_22;
    tmp_uintptr = sec2_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec2_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec2_cfg_buf, p_flow_tbl_list->r2y.p_CR_buf_22, CR_SIZE_22);
    p_amalgam_data->flow_ctrl.step_crs[0].sec2_crs[18].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec2 cr[18] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_amalgam_data->flow_ctrl.step_crs[0].sec2_crs[18].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->r2y.p_CR_buf_22), DC_U, DC_U);

    filter_offset = CR_OFFSET_23;
    tmp_uintptr = sec2_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec2_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec2_cfg_buf, p_flow_tbl_list->r2y.p_CR_buf_23, CR_SIZE_23);
    p_amalgam_data->flow_ctrl.step_crs[0].sec2_crs[19].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec2 cr[19] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_amalgam_data->flow_ctrl.step_crs[0].sec2_crs[19].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->r2y.p_CR_buf_23), DC_U, DC_U);

    filter_offset = CR_OFFSET_24;
    tmp_uintptr = sec2_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec2_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec2_cfg_buf, p_flow_tbl_list->r2y.p_CR_buf_24, CR_SIZE_24);
    p_amalgam_data->flow_ctrl.step_crs[0].sec2_crs[20].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec2 cr[20] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_amalgam_data->flow_ctrl.step_crs[0].sec2_crs[20].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->r2y.p_CR_buf_24), DC_U, DC_U);

    filter_offset = CR_OFFSET_25;
    tmp_uintptr = sec2_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec2_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec2_cfg_buf, p_flow_tbl_list->r2y.p_CR_buf_25, CR_SIZE_25);
    p_amalgam_data->flow_ctrl.step_crs[0].sec2_crs[21].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec2 cr[21] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_amalgam_data->flow_ctrl.step_crs[0].sec2_crs[21].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->r2y.p_CR_buf_25), DC_U, DC_U);

    filter_offset = CR_OFFSET_26;
    tmp_uintptr = sec2_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec2_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec2_cfg_buf, p_flow_tbl_list->r2y.p_CR_buf_26, CR_SIZE_26);
    p_amalgam_data->flow_ctrl.step_crs[0].sec2_crs[22].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec2 cr[22] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_amalgam_data->flow_ctrl.step_crs[0].sec2_crs[22].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->r2y.p_CR_buf_26), DC_U, DC_U);

    filter_offset = CR_OFFSET_27;
    tmp_uintptr = sec2_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec2_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec2_cfg_buf, p_flow_tbl_list->r2y.p_CR_buf_27, CR_SIZE_27);
    p_amalgam_data->flow_ctrl.step_crs[0].sec2_crs[23].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec2 cr[23] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_amalgam_data->flow_ctrl.step_crs[0].sec2_crs[23].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->r2y.p_CR_buf_27), DC_U, DC_U);

    filter_offset = CR_OFFSET_28;
    tmp_uintptr = sec2_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec2_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec2_cfg_buf, p_flow_tbl_list->r2y.p_CR_buf_28, CR_SIZE_28);
    p_amalgam_data->flow_ctrl.step_crs[0].sec2_crs[24].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec2 cr[24] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_amalgam_data->flow_ctrl.step_crs[0].sec2_crs[24].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->r2y.p_CR_buf_28), DC_U, DC_U);

    filter_offset = CR_OFFSET_29;
    tmp_uintptr = sec2_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec2_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec2_cfg_buf, p_flow_tbl_list->r2y.p_CR_buf_29, CR_SIZE_29);
    p_amalgam_data->flow_ctrl.step_crs[0].sec2_crs[25].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec2 cr[25] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_amalgam_data->flow_ctrl.step_crs[0].sec2_crs[25].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->r2y.p_CR_buf_29), DC_U, DC_U);

    filter_offset = CR_OFFSET_30;
    tmp_uintptr = sec2_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec2_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec2_cfg_buf, p_flow_tbl_list->r2y.p_CR_buf_30, CR_SIZE_30);
    p_amalgam_data->flow_ctrl.step_crs[0].sec2_crs[26].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec2 cr[26] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_amalgam_data->flow_ctrl.step_crs[0].sec2_crs[26].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->r2y.p_CR_buf_30), DC_U, DC_U);

    filter_offset = CR_OFFSET_31;
    tmp_uintptr = sec2_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec2_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec2_cfg_buf, p_flow_tbl_list->r2y.p_CR_buf_31, CR_SIZE_31);
    p_amalgam_data->flow_ctrl.step_crs[0].sec2_crs[27].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec2 cr[27] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_amalgam_data->flow_ctrl.step_crs[0].sec2_crs[27].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->r2y.p_CR_buf_31), DC_U, DC_U);

    filter_offset = CR_OFFSET_32;
    tmp_uintptr = sec2_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec2_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec2_cfg_buf, p_flow_tbl_list->r2y.p_CR_buf_32, CR_SIZE_32);
    p_amalgam_data->flow_ctrl.step_crs[0].sec2_crs[28].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec2 cr[28] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_amalgam_data->flow_ctrl.step_crs[0].sec2_crs[28].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->r2y.p_CR_buf_32), DC_U, DC_U);

    filter_offset = CR_OFFSET_33;
    tmp_uintptr = sec2_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec2_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec2_cfg_buf, p_flow_tbl_list->r2y.p_CR_buf_33, CR_SIZE_33);
    p_amalgam_data->flow_ctrl.step_crs[0].sec2_crs[29].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec2 cr[29] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_amalgam_data->flow_ctrl.step_crs[0].sec2_crs[29].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->r2y.p_CR_buf_33), DC_U, DC_U);

    filter_offset = CR_OFFSET_34;
    tmp_uintptr = sec2_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec2_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec2_cfg_buf, p_flow_tbl_list->r2y.p_CR_buf_34, CR_SIZE_34);
    p_amalgam_data->flow_ctrl.step_crs[0].sec2_crs[30].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec2 cr[30] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_amalgam_data->flow_ctrl.step_crs[0].sec2_crs[30].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->r2y.p_CR_buf_34), DC_U, DC_U);

    filter_offset = CR_OFFSET_35;
    tmp_uintptr = sec2_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec2_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec2_cfg_buf, p_flow_tbl_list->r2y.p_CR_buf_35, CR_SIZE_35);
    p_amalgam_data->flow_ctrl.step_crs[0].sec2_crs[31].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec2 cr[31] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_amalgam_data->flow_ctrl.step_crs[0].sec2_crs[31].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->r2y.p_CR_buf_35), DC_U, DC_U);

    filter_offset = CR_OFFSET_40;
    tmp_uintptr = sec2_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec2_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec2_cfg_buf, p_flow_tbl_list->r2y.p_CR_buf_40, CR_SIZE_40);
    p_amalgam_data->flow_ctrl.step_crs[0].sec2_crs[32].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec2 cr[32] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_amalgam_data->flow_ctrl.step_crs[0].sec2_crs[32].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->r2y.p_CR_buf_40), DC_U, DC_U);

    filter_offset = CR_OFFSET_36;
    tmp_uintptr = sec2_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec2_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec2_cfg_buf, p_flow_tbl_list->r2y.p_CR_buf_36, CR_SIZE_36);
    p_amalgam_data->flow_ctrl.step_crs[0].sec2_crs[33].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec2 cr[33] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_amalgam_data->flow_ctrl.step_crs[0].sec2_crs[33].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->r2y.p_CR_buf_36), DC_U, DC_U);

    filter_offset = CR_OFFSET_37;
    tmp_uintptr = sec2_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec2_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec2_cfg_buf, p_flow_tbl_list->r2y.p_CR_buf_37, CR_SIZE_37);
    p_amalgam_data->flow_ctrl.step_crs[0].sec2_crs[34].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec2 cr[34] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_amalgam_data->flow_ctrl.step_crs[0].sec2_crs[34].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->r2y.p_CR_buf_37), DC_U, DC_U);

    filter_offset = CR_OFFSET_38;
    tmp_uintptr = sec2_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec2_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec2_cfg_buf, p_flow_tbl_list->r2y.p_CR_buf_38, CR_SIZE_38);
    p_amalgam_data->flow_ctrl.step_crs[0].sec2_crs[35].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec2 cr[35] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_amalgam_data->flow_ctrl.step_crs[0].sec2_crs[35].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->r2y.p_CR_buf_38), DC_U, DC_U);

    filter_offset = CR_OFFSET_39;
    tmp_uintptr = sec2_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec2_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec2_cfg_buf, p_flow_tbl_list->r2y.p_CR_buf_39, CR_SIZE_39);
    p_amalgam_data->flow_ctrl.step_crs[0].sec2_crs[36].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec2 cr[36] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_amalgam_data->flow_ctrl.step_crs[0].sec2_crs[36].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->r2y.p_CR_buf_39), DC_U, DC_U);

    filter_offset = CR_OFFSET_41;
    tmp_uintptr = sec2_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec2_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec2_cfg_buf, p_flow_tbl_list->r2y.p_CR_buf_41, CR_SIZE_41);
    p_amalgam_data->flow_ctrl.step_crs[0].sec2_crs[37].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec2 cr[37] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_amalgam_data->flow_ctrl.step_crs[0].sec2_crs[37].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->r2y.p_CR_buf_41), DC_U, DC_U);

    filter_offset = CR_OFFSET_42;
    tmp_uintptr = sec3_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec3_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec3_cfg_buf, p_flow_tbl_list->r2y.p_CR_buf_42, CR_SIZE_42);
    p_amalgam_data->flow_ctrl.step_crs[0].sec3_crs[0].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec3 cr[0] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_amalgam_data->flow_ctrl.step_crs[0].sec3_crs[0].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->r2y.p_CR_buf_42), DC_U, DC_U);

    filter_offset = CR_OFFSET_43;
    tmp_uintptr = sec3_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec3_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec3_cfg_buf, p_flow_tbl_list->r2y.p_CR_buf_43, CR_SIZE_43);
    p_amalgam_data->flow_ctrl.step_crs[0].sec3_crs[1].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec3 cr[1] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_amalgam_data->flow_ctrl.step_crs[0].sec3_crs[1].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->r2y.p_CR_buf_43), DC_U, DC_U);

    filter_offset = CR_OFFSET_44;
    tmp_uintptr = sec3_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec3_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec3_cfg_buf, p_flow_tbl_list->r2y.p_CR_buf_44, CR_SIZE_44);
    p_amalgam_data->flow_ctrl.step_crs[0].sec3_crs[2].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec3 cr[2] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_amalgam_data->flow_ctrl.step_crs[0].sec3_crs[2].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->r2y.p_CR_buf_44), DC_U, DC_U);

    if (p_flow_tbl_list->r2y.p_CR_buf_45 != NULL) {
        filter_offset = CR_OFFSET_45;
        tmp_uintptr = sec4_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
        (void)amba_ik_system_memcpy(&p_sec4_cfg_buf, &tmp_uintptr, sizeof(void*));
        (void)amba_ik_system_memcpy(p_sec4_cfg_buf, p_flow_tbl_list->r2y.p_CR_buf_45, CR_SIZE_45);
        p_amalgam_data->flow_ctrl.step_crs[0].sec4_crs[0].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
        amba_ik_system_print_uint64_5("Sec4 cr[0] addr 0x%08x, offset %d, src_addr %p ",
                                      (uint64)p_amalgam_data->flow_ctrl.step_crs[0].sec4_crs[0].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->r2y.p_CR_buf_45), DC_U, DC_U);
    } else {
        amba_ik_system_print_str_5("[IK] CR_buf_45 is NULL", NULL, NULL, NULL, NULL, NULL);
    }

    if (p_flow_tbl_list->r2y.p_CR_buf_46 != NULL) {
        filter_offset = CR_OFFSET_46;
        tmp_uintptr = sec4_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
        (void)amba_ik_system_memcpy(&p_sec4_cfg_buf, &tmp_uintptr, sizeof(void*));
        (void)amba_ik_system_memcpy(p_sec4_cfg_buf, p_flow_tbl_list->r2y.p_CR_buf_46, CR_SIZE_46);
        p_amalgam_data->flow_ctrl.step_crs[0].sec4_crs[1].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
        amba_ik_system_print_uint64_5("Sec4 cr[1] addr 0x%08x, offset %d, src_addr %p ",
                                      (uint64)p_amalgam_data->flow_ctrl.step_crs[0].sec4_crs[1].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->r2y.p_CR_buf_46), DC_U, DC_U);
    } else {
        amba_ik_system_print_str_5("[IK] CR_buf_46 is NULL", NULL, NULL, NULL, NULL, NULL);
    }

    if (p_flow_tbl_list->r2y.p_CR_buf_47 != NULL) {
        filter_offset = CR_OFFSET_47;
        tmp_uintptr = sec4_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
        (void)amba_ik_system_memcpy(&p_sec4_cfg_buf, &tmp_uintptr, sizeof(void*));
        (void)amba_ik_system_memcpy(p_sec4_cfg_buf, p_flow_tbl_list->r2y.p_CR_buf_47, CR_SIZE_47);
        p_amalgam_data->flow_ctrl.step_crs[0].sec4_crs[2].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
        amba_ik_system_print_uint64_5("Sec4 cr[2] addr 0x%08x, offset %d, src_addr %p ",
                                      (uint64)p_amalgam_data->flow_ctrl.step_crs[0].sec4_crs[2].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->r2y.p_CR_buf_47), DC_U, DC_U);
    } else {
        amba_ik_system_print_str_5("[IK] CR_buf_47 is NULL", NULL, NULL, NULL, NULL, NULL);
    }

    if (p_flow_tbl_list->r2y.p_CR_buf_48 != NULL) {
        filter_offset = CR_OFFSET_48;
        tmp_uintptr = sec4_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
        (void)amba_ik_system_memcpy(&p_sec4_cfg_buf, &tmp_uintptr, sizeof(void*));
        (void)amba_ik_system_memcpy(p_sec4_cfg_buf, p_flow_tbl_list->r2y.p_CR_buf_48, CR_SIZE_48);
        p_amalgam_data->flow_ctrl.step_crs[0].sec4_crs[3].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
        amba_ik_system_print_uint64_5("Sec4 cr[3] addr 0x%08x, offset %d, src_addr %p ",
                                      (uint64)p_amalgam_data->flow_ctrl.step_crs[0].sec4_crs[3].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->r2y.p_CR_buf_48), DC_U, DC_U);
    } else {
        amba_ik_system_print_str_5("[IK] CR_buf_48 is NULL", NULL, NULL, NULL, NULL, NULL);
    }

    if (p_flow_tbl_list->r2y.p_CR_buf_49 != NULL) {
        filter_offset = CR_OFFSET_49;
        tmp_uintptr = sec4_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
        (void)amba_ik_system_memcpy(&p_sec4_cfg_buf, &tmp_uintptr, sizeof(void*));
        (void)amba_ik_system_memcpy(p_sec4_cfg_buf, p_flow_tbl_list->r2y.p_CR_buf_49, CR_SIZE_49);
        p_amalgam_data->flow_ctrl.step_crs[0].sec4_crs[4].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
        amba_ik_system_print_uint64_5("Sec4 cr[4] addr 0x%08x, offset %d, src_addr %p ",
                                      (uint64)p_amalgam_data->flow_ctrl.step_crs[0].sec4_crs[4].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->r2y.p_CR_buf_49), DC_U, DC_U);
    } else {
        amba_ik_system_print_str_5("[IK] CR_buf_49 is NULL", NULL, NULL, NULL, NULL, NULL);
    }

    if (p_flow_tbl_list->r2y.p_CR_buf_50 != NULL) {
        filter_offset = CR_OFFSET_50;
        tmp_uintptr = sec4_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
        (void)amba_ik_system_memcpy(&p_sec4_cfg_buf, &tmp_uintptr, sizeof(void*));
        (void)amba_ik_system_memcpy(p_sec4_cfg_buf, p_flow_tbl_list->r2y.p_CR_buf_50, CR_SIZE_50);
        p_amalgam_data->flow_ctrl.step_crs[0].sec4_crs[5].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
        amba_ik_system_print_uint64_5("Sec4 cr[5] addr 0x%08x, offset %d, src_addr %p ",
                                      (uint64)p_amalgam_data->flow_ctrl.step_crs[0].sec4_crs[5].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->r2y.p_CR_buf_50), DC_U, DC_U);
    } else {
        amba_ik_system_print_str_5("[IK] CR_buf_50 is NULL", NULL, NULL, NULL, NULL, NULL);
    }

    if (p_flow_tbl_list->r2y.p_CR_buf_51 != NULL) {
        filter_offset = CR_OFFSET_51;
        tmp_uintptr = sec4_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
        (void)amba_ik_system_memcpy(&p_sec4_cfg_buf, &tmp_uintptr, sizeof(void*));
        (void)amba_ik_system_memcpy(p_sec4_cfg_buf, p_flow_tbl_list->r2y.p_CR_buf_51, CR_SIZE_51);
        p_amalgam_data->flow_ctrl.step_crs[0].sec4_crs[6].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
        amba_ik_system_print_uint64_5("Sec4 cr[6] addr 0x%08x, offset %d, src_addr %p ",
                                      (uint64)p_amalgam_data->flow_ctrl.step_crs[0].sec4_crs[6].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->r2y.p_CR_buf_51), DC_U, DC_U);
    } else {
        amba_ik_system_print_str_5("[IK] CR_buf_51 is NULL", NULL, NULL, NULL, NULL, NULL);
    }

    if (p_flow_tbl_list->r2y.p_CR_buf_52 != NULL) {
        filter_offset = CR_OFFSET_52;
        tmp_uintptr = sec4_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
        (void)amba_ik_system_memcpy(&p_sec4_cfg_buf, &tmp_uintptr, sizeof(void*));
        (void)amba_ik_system_memcpy(p_sec4_cfg_buf, p_flow_tbl_list->r2y.p_CR_buf_52, CR_SIZE_52);
        p_amalgam_data->flow_ctrl.step_crs[0].sec4_crs[7].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
        amba_ik_system_print_uint64_5("Sec4 cr[7] addr 0x%08x, offset %d, src_addr %p ",
                                      (uint64)p_amalgam_data->flow_ctrl.step_crs[0].sec4_crs[7].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->r2y.p_CR_buf_52), DC_U, DC_U);
    } else {
        amba_ik_system_print_str_5("[IK] CR_buf_52 is NULL", NULL, NULL, NULL, NULL, NULL);
    }

    if (p_flow_tbl_list->r2y.p_CR_buf_100 != NULL) {
        filter_offset = CR_OFFSET_100;
        tmp_uintptr = sec11_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
        (void)amba_ik_system_memcpy(&p_sec11_cfg_buf, &tmp_uintptr, sizeof(void*));
        (void)amba_ik_system_memcpy(p_sec11_cfg_buf, p_flow_tbl_list->r2y.p_CR_buf_100, CR_SIZE_100);
        p_amalgam_data->flow_ctrl.step_crs[0].sec11_crs[0].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
        amba_ik_system_print_uint64_5("Sec11 cr[0] addr 0x%08x, offset %d, src_addr %p ",
                                      (uint64)p_amalgam_data->flow_ctrl.step_crs[0].sec11_crs[0].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->r2y.p_CR_buf_100), DC_U, DC_U);
    } else {
        amba_ik_system_print_str_5("[IK] CR_buf_100 is NULL", NULL, NULL, NULL, NULL, NULL);
    }

    if (p_flow_tbl_list->r2y.p_CR_buf_101 != NULL) {
        filter_offset = CR_OFFSET_101;
        tmp_uintptr = sec11_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
        (void)amba_ik_system_memcpy(&p_sec11_cfg_buf, &tmp_uintptr, sizeof(void*));
        (void)amba_ik_system_memcpy(p_sec11_cfg_buf, p_flow_tbl_list->r2y.p_CR_buf_101, CR_SIZE_101);
        p_amalgam_data->flow_ctrl.step_crs[0].sec11_crs[1].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
        amba_ik_system_print_uint64_5("Sec11 cr[1] addr 0x%08x, offset %d, src_addr %p ",
                                      (uint64)p_amalgam_data->flow_ctrl.step_crs[0].sec11_crs[1].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->r2y.p_CR_buf_101), DC_U, DC_U);
    } else {
        amba_ik_system_print_str_5("[IK] CR_buf_101 is NULL", NULL, NULL, NULL, NULL, NULL);
    }

    if (p_flow_tbl_list->r2y.p_CR_buf_102 != NULL) {
        filter_offset = CR_OFFSET_102;
        tmp_uintptr = sec11_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
        (void)amba_ik_system_memcpy(&p_sec11_cfg_buf, &tmp_uintptr, sizeof(void*));
        (void)amba_ik_system_memcpy(p_sec11_cfg_buf, p_flow_tbl_list->r2y.p_CR_buf_102, CR_SIZE_102);
        p_amalgam_data->flow_ctrl.step_crs[0].sec11_crs[2].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
        amba_ik_system_print_uint64_5("Sec11 cr[2] addr 0x%08x, offset %d, src_addr %p ",
                                      (uint64)p_amalgam_data->flow_ctrl.step_crs[0].sec11_crs[2].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->r2y.p_CR_buf_102), DC_U, DC_U);
    } else {
        amba_ik_system_print_str_5("[IK] CR_buf_102 is NULL", NULL, NULL, NULL, NULL, NULL);
    }

    if (p_flow_tbl_list->r2y.p_CR_buf_103 != NULL) {
        filter_offset = CR_OFFSET_103;
        tmp_uintptr = sec11_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
        (void)amba_ik_system_memcpy(&p_sec11_cfg_buf, &tmp_uintptr, sizeof(void*));
        (void)amba_ik_system_memcpy(p_sec11_cfg_buf, p_flow_tbl_list->r2y.p_CR_buf_103, CR_SIZE_103);
        p_amalgam_data->flow_ctrl.step_crs[0].sec11_crs[3].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
        amba_ik_system_print_uint64_5("Sec11 cr[3] addr 0x%08x, offset %d, src_addr %p ",
                                      (uint64)p_amalgam_data->flow_ctrl.step_crs[0].sec11_crs[3].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->r2y.p_CR_buf_103), DC_U, DC_U);
    } else {
        amba_ik_system_print_str_5("[IK] CR_buf_100 is NULL", NULL, NULL, NULL, NULL, NULL);
    }

    filter_offset = CR_OFFSET_111;
    tmp_uintptr = sec18_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec18_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec18_cfg_buf, p_flow_tbl_list->r2y.p_CR_buf_111, CR_SIZE_111);
    p_amalgam_data->flow_ctrl.step_crs[0].sec18_crs[0].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec18 cr[0] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_amalgam_data->flow_ctrl.step_crs[0].sec18_crs[0].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->r2y.p_CR_buf_111), DC_U, DC_U);

    filter_offset = CR_OFFSET_112;
    tmp_uintptr = sec18_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec18_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec18_cfg_buf, p_flow_tbl_list->r2y.p_CR_buf_112, CR_SIZE_112);
    p_amalgam_data->flow_ctrl.step_crs[0].sec18_crs[1].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec18 cr[1] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_amalgam_data->flow_ctrl.step_crs[0].sec18_crs[1].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->r2y.p_CR_buf_112), DC_U, DC_U);

    filter_offset = CR_OFFSET_113;
    tmp_uintptr = sec18_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec18_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec18_cfg_buf, p_flow_tbl_list->r2y.p_CR_buf_113, CR_SIZE_113);
    p_amalgam_data->flow_ctrl.step_crs[0].sec18_crs[2].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec18 cr[2] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_amalgam_data->flow_ctrl.step_crs[0].sec18_crs[2].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->r2y.p_CR_buf_113), DC_U, DC_U);

    filter_offset = CR_OFFSET_114;
    tmp_uintptr = sec18_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec18_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec18_cfg_buf, p_flow_tbl_list->r2y.p_CR_buf_114, CR_SIZE_114);
    p_amalgam_data->flow_ctrl.step_crs[0].sec18_crs[3].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec18 cr[3] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_amalgam_data->flow_ctrl.step_crs[0].sec18_crs[3].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->r2y.p_CR_buf_114), DC_U, DC_U);

    filter_offset = CR_OFFSET_115;
    tmp_uintptr = sec18_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec18_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec18_cfg_buf, p_flow_tbl_list->r2y.p_CR_buf_115, CR_SIZE_115);
    p_amalgam_data->flow_ctrl.step_crs[0].sec18_crs[4].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec18 cr[4] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_amalgam_data->flow_ctrl.step_crs[0].sec18_crs[4].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->r2y.p_CR_buf_115), DC_U, DC_U);

    filter_offset = CR_OFFSET_116;
    tmp_uintptr = sec18_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec18_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec18_cfg_buf, p_flow_tbl_list->r2y.p_CR_buf_116, CR_SIZE_116);
    p_amalgam_data->flow_ctrl.step_crs[0].sec18_crs[5].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec18 cr[5] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_amalgam_data->flow_ctrl.step_crs[0].sec18_crs[5].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->r2y.p_CR_buf_116), DC_U, DC_U);

    filter_offset = CR_OFFSET_117;
    tmp_uintptr = sec18_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec18_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec18_cfg_buf, p_flow_tbl_list->r2y.p_CR_buf_117, CR_SIZE_117);
    p_amalgam_data->flow_ctrl.step_crs[0].sec18_crs[6].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec18 cr[6] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_amalgam_data->flow_ctrl.step_crs[0].sec18_crs[6].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->r2y.p_CR_buf_117), DC_U, DC_U);

    filter_offset = CR_OFFSET_118;
    tmp_uintptr = sec18_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec18_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec18_cfg_buf, p_flow_tbl_list->r2y.p_CR_buf_118, CR_SIZE_118);
    p_amalgam_data->flow_ctrl.step_crs[0].sec18_crs[7].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec18 cr[7] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_amalgam_data->flow_ctrl.step_crs[0].sec18_crs[7].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->r2y.p_CR_buf_118), DC_U, DC_U);

    filter_offset = CR_OFFSET_119;
    tmp_uintptr = sec18_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec18_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec18_cfg_buf, p_flow_tbl_list->r2y.p_CR_buf_119, CR_SIZE_119);
    p_amalgam_data->flow_ctrl.step_crs[0].sec18_crs[8].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec18 cr[8] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_amalgam_data->flow_ctrl.step_crs[0].sec18_crs[8].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->r2y.p_CR_buf_119), DC_U, DC_U);

    filter_offset = CR_OFFSET_120;
    tmp_uintptr = sec18_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec18_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec18_cfg_buf, p_flow_tbl_list->r2y.p_CR_buf_120, CR_SIZE_120);
    p_amalgam_data->flow_ctrl.step_crs[0].sec18_crs[9].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec18 cr[9] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_amalgam_data->flow_ctrl.step_crs[0].sec18_crs[9].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->r2y.p_CR_buf_120), DC_U, DC_U);

    filter_offset = CR_OFFSET_121;
    tmp_uintptr = sec18_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec18_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec18_cfg_buf, p_flow_tbl_list->r2y.p_CR_buf_121, CR_SIZE_121);
    p_amalgam_data->flow_ctrl.step_crs[0].sec18_crs[10].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec18 cr[10] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_amalgam_data->flow_ctrl.step_crs[0].sec18_crs[10].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->r2y.p_CR_buf_121), DC_U, DC_U);

    //p_amalgam_data->flow_ctrl.header.idsp_flow_addr = data_offset_base;
    (void)amba_ik_system_memcpy(&p_amalgam_data->flow_ctrl.header.idsp_flow_addr, &data_offset_base, sizeof(uint32));

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
        p_ik_dbg_data->ctx_debug_data.ctx_id = (uint16)context_id;
        p_ik_dbg_data->ctx_debug_data.active_flow_idx = p_ctx->organization.active_cr_state.active_flow_idx;

        //3a statistic
        (void)amba_ik_system_memcpy(&p_ik_dbg_data->ctx_debug_data.aaa_stat_info, &p_ctx->filters.input_param.aaa_stat_info, sizeof(ik_aaa_stat_info_t));
        (void)amba_ik_system_memcpy(&p_ik_dbg_data->ctx_debug_data.aaa_pg_stat_info, &p_ctx->filters.input_param.aaa_pg_stat_info, sizeof(ik_aaa_pg_af_stat_info_t));
        (void)amba_ik_system_memcpy(&p_ik_dbg_data->ctx_debug_data.af_stat_ex_info, &p_ctx->filters.input_param.af_stat_ex_info, sizeof(ik_af_stat_ex_info_t));
        (void)amba_ik_system_memcpy(&p_ik_dbg_data->ctx_debug_data.pg_af_stat_ex_info, &p_ctx->filters.input_param.pg_af_stat_ex_info, sizeof(ik_pg_af_stat_ex_info_t));
        (void)amba_ik_system_memcpy(&p_ik_dbg_data->ctx_debug_data.hist_info, &p_ctx->filters.input_param.hist_info, sizeof(ik_histogram_info_t));
        (void)amba_ik_system_memcpy(&p_ik_dbg_data->ctx_debug_data.hist_info_pg, &p_ctx->filters.input_param.hist_info_pg, sizeof(ik_histogram_info_t));

        // window info
        p_ik_dbg_data->ctx_debug_data.flip_mode = p_ctx->filters.input_param.flip_mode;
        (void)amba_ik_system_memcpy(&p_ik_dbg_data->ctx_debug_data.dzoom_info, &p_ctx->filters.input_param.dzoom_info, sizeof(ik_dzoom_info_t));
        (void)amba_ik_system_memcpy(&p_ik_dbg_data->ctx_debug_data.dmy_range, &p_ctx->filters.input_param.dmy_range, sizeof(ik_dummy_margin_range_t));
        (void)amba_ik_system_memcpy(&p_ik_dbg_data->ctx_debug_data.active_window, &p_ctx->filters.input_param.active_window, sizeof(ik_vin_active_window_t));
        (void)amba_ik_system_memcpy(&p_ik_dbg_data->ctx_debug_data.stitching_info, &p_ctx->filters.input_param.stitching_info, sizeof(ik_stitch_info_t));
        (void)amba_ik_system_memcpy(&p_ik_dbg_data->ctx_debug_data.window_size_info, &p_ctx->filters.input_param.window_size_info, sizeof(ik_window_size_info_t));
        (void)amba_ik_system_memcpy(&p_ik_dbg_data->ctx_debug_data.result_win, &p_ctx->filters.input_param.ctx_buf.result_win, sizeof(amba_ik_calc_win_result_t));

        // vig
        p_ik_dbg_data->ctx_debug_data.vig_enable = p_ctx->filters.input_param.vig_enable;
        (void)amba_ik_system_memcpy(&p_ik_dbg_data->ctx_debug_data.vignette_compensation, &p_ctx->filters.input_param.vignette_compensation, sizeof(ik_vignette_t));

        // ca
        p_ik_dbg_data->ctx_debug_data.cawarp_enable = p_ctx->filters.input_param.cawarp_enable;
        (void)amba_ik_system_memcpy(&p_ik_dbg_data->ctx_debug_data.calib_cawarp_info, &p_ctx->filters.input_param.calib_ca_warp_info, sizeof(ik_cawarp_info_t));
        if(p_ctx->filters.input_param.calib_ca_warp_info.p_cawarp_red != NULL) {
            (void)amba_ik_system_memcpy(p_ik_dbg_data->ctx_debug_data.calib_cawarp_red, p_ctx->filters.input_param.calib_ca_warp_info.p_cawarp_red, sizeof(ik_grid_point_t) * CAWARP_VIDEO_MAXHORGRID * CAWARP_VIDEO_MAXVERGRID);
        } else {
            (void)amba_ik_system_memset(p_ik_dbg_data->ctx_debug_data.calib_cawarp_red, 0x0, sizeof(ik_grid_point_t) * CAWARP_VIDEO_MAXHORGRID * CAWARP_VIDEO_MAXVERGRID);
        }
        if(p_ctx->filters.input_param.calib_ca_warp_info.p_cawarp_blue != NULL) {
            (void)amba_ik_system_memcpy(p_ik_dbg_data->ctx_debug_data.calib_cawarp_blue, p_ctx->filters.input_param.calib_ca_warp_info.p_cawarp_blue, sizeof(ik_grid_point_t) * CAWARP_VIDEO_MAXHORGRID * CAWARP_VIDEO_MAXVERGRID);
        } else {
            (void)amba_ik_system_memset(p_ik_dbg_data->ctx_debug_data.calib_cawarp_blue, 0x0, sizeof(ik_grid_point_t) * CAWARP_VIDEO_MAXHORGRID * CAWARP_VIDEO_MAXVERGRID);
        }

        // warp
        p_ik_dbg_data->ctx_debug_data.warp_enable = p_ctx->filters.input_param.warp_enable;
        p_ik_dbg_data->ctx_debug_data.chroma_radius = p_ctx->filters.input_param.chroma_filter.radius;
        p_ik_dbg_data->ctx_debug_data.yuv_mode = p_ctx->filters.input_param.yuv_mode;
        (void)amba_ik_system_memcpy(&p_ik_dbg_data->ctx_debug_data.calib_warp_info, &p_ctx->filters.input_param.calib_warp_info, sizeof(ik_warp_info_t));
        (void)amba_ik_system_memcpy(&p_ik_dbg_data->ctx_debug_data.warp_buf_info, &p_ctx->filters.input_param.warp_buf_info, sizeof(ik_warp_buffer_info_t));
        if(p_ctx->filters.input_param.calib_warp_info.pwarp != NULL) {
            (void)amba_ik_system_memcpy(p_ik_dbg_data->ctx_debug_data.calib_warp, p_ctx->filters.input_param.calib_warp_info.pwarp, sizeof(ik_grid_point_t) * WARP_VIDEO_MAXHORGRID * WARP_VIDEO_MAXVERGRID * 2U);
        } else {
            (void)amba_ik_system_memset(p_ik_dbg_data->ctx_debug_data.calib_warp, 0x0, sizeof(ik_grid_point_t) * WARP_VIDEO_MAXHORGRID * WARP_VIDEO_MAXVERGRID * 2U);
        }

        {
            // version
            uint32 ikc_version_major, ikc_version_minor;
            rval |= ikc_get_version(&ikc_version_major, &ikc_version_minor);
            p_ik_dbg_data->ctx_debug_data.ik_version_major = (uint32)IK_VERSION_MAJOR;
            p_ik_dbg_data->ctx_debug_data.ik_version_minor = (uint32)IK_VERSION_MINOR;
            p_ik_dbg_data->ctx_debug_data.ikc_version_major = ikc_version_major;
            p_ik_dbg_data->ctx_debug_data.ikc_version_minor = ikc_version_minor;
        }
    } else {
        amba_ik_system_print_str_5("[IK] can't get context", NULL, NULL, NULL, NULL, NULL);
    }

    return rval;
}

uint32 img_cfg_query_debug_flow_data_size(uint32 context_id, size_t *p_size)
{
    uint32_t rval = IK_OK;
    ik_ability_t ability;

    rval |= ik_get_context_ability(context_id, &ability);

    if(p_size != NULL) {
        if ((ability.video_pipe == AMBA_IK_VIDEO_PIPE_LINEAR) ||
            (ability.video_pipe == AMBA_IK_VIDEO_PIPE_LINEAR_CE) ||
            (ability.video_pipe == AMBA_IK_VIDEO_PIPE_HDR_EXPO_2) ||
            (ability.video_pipe == AMBA_IK_VIDEO_PIPE_HDR_EXPO_3) ||
            (ability.video_pipe == AMBA_IK_VIDEO_PIPE_Y2Y) ||
            (ability.video_pipe == AMBA_IK_VIDEO_PIPE_LINEAR_HVH) ||
            (ability.video_pipe == AMBA_IK_VIDEO_PIPE_LINEAR_CE_HVH) ||
            (ability.video_pipe == AMBA_IK_VIDEO_PIPE_HDR_EXPO_2_HVH) ||
            (ability.video_pipe == AMBA_IK_VIDEO_PIPE_HDR_EXPO_3_HVH) ||
            (ability.video_pipe == AMBA_IK_VIDEO_PIPE_Y2Y_HVH) ||
            (ability.video_pipe == AMBA_IK_VIDEO_PIPE_LINEAR_HHB) ||
            (ability.video_pipe == AMBA_IK_VIDEO_PIPE_LINEAR_CE_HHB) ||
            (ability.video_pipe == AMBA_IK_VIDEO_PIPE_HDR_EXPO_2_HHB) ||
            (ability.video_pipe == AMBA_IK_VIDEO_PIPE_HDR_EXPO_3_HHB) ||
            (ability.video_pipe == AMBA_IK_VIDEO_PIPE_Y2Y_HHB)) {
            *p_size = sizeof(ik_debug_data_t);
        } else if ((ability.video_pipe == AMBA_IK_VIDEO_PIPE_LINEAR_MD) ||
                   (ability.video_pipe == AMBA_IK_VIDEO_PIPE_LINEAR_CE_MD) ||
                   (ability.video_pipe == AMBA_IK_VIDEO_PIPE_HDR_EXPO_2_MD) ||
                   (ability.video_pipe == AMBA_IK_VIDEO_PIPE_HDR_EXPO_3_MD) ||
                   (ability.video_pipe == AMBA_IK_VIDEO_PIPE_FUSION) ||
                   (ability.video_pipe == AMBA_IK_VIDEO_PIPE_Y2Y_MD) ||
                   (ability.video_pipe == AMBA_IK_VIDEO_PIPE_LINEAR_MD_HHB) ||
                   (ability.video_pipe == AMBA_IK_VIDEO_PIPE_LINEAR_CE_MD_HHB) ||
                   (ability.video_pipe == AMBA_IK_VIDEO_PIPE_HDR_EXPO_2_MD_HHB) ||
                   (ability.video_pipe == AMBA_IK_VIDEO_PIPE_HDR_EXPO_3_MD_HHB) ||
                   (ability.video_pipe == AMBA_IK_VIDEO_PIPE_Y2Y_MD_HHB)) {
            *p_size = sizeof(ik_motion_fusion_debug_data_t);
        } else {
            // misraC
        }
    } else {
        amba_ik_system_print_str_5("img_cfg_query_debug_config_size size ptr is NULL", NULL, NULL, NULL, NULL, NULL);
        rval = IK_ERR_0005;
    }
    return rval;
}

/****************     debug dump     ****************/
// common use api
static void img_cfg_hiso_r2y_sec2_amalgam_data_update(uintptr data_offset_base,
        idsp_step_crs_t *p_step_cr,
        uint8_t *p_cr,
        const amba_ik_step1_sec2_cr_list_t *p_flow_tbl_list)
{
    uint32_t filter_offset;
    uintptr sec2_cfg_buf_offset;
    void* p_sec2_cfg_buf;
    uintptr tmp_uintptr;

    (void)amba_ik_system_memcpy(&sec2_cfg_buf_offset, &p_cr, sizeof(tmp_uintptr));

    filter_offset = CR_OFFSET_4;
    tmp_uintptr = sec2_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec2_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec2_cfg_buf, p_flow_tbl_list->p_CR_buf_4, CR_SIZE_4);
    p_step_cr->sec2_crs[0].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec2 cr[0] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_step_cr->sec2_crs[0].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_4), DC_U, DC_U);

    filter_offset = CR_OFFSET_5;
    tmp_uintptr = sec2_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec2_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec2_cfg_buf, p_flow_tbl_list->p_CR_buf_5, CR_SIZE_5);
    p_step_cr->sec2_crs[1].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec2 cr[1] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_step_cr->sec2_crs[1].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_5), DC_U, DC_U);

    filter_offset = CR_OFFSET_6;
    tmp_uintptr = sec2_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec2_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec2_cfg_buf, p_flow_tbl_list->p_CR_buf_6, CR_SIZE_6);
    p_step_cr->sec2_crs[2].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec2 cr[2] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_step_cr->sec2_crs[2].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_6), DC_U, DC_U);

    filter_offset = CR_OFFSET_7;
    tmp_uintptr = sec2_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec2_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec2_cfg_buf, p_flow_tbl_list->p_CR_buf_7, CR_SIZE_7);
    p_step_cr->sec2_crs[3].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec2 cr[3] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_step_cr->sec2_crs[3].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_7), DC_U, DC_U);

    filter_offset = CR_OFFSET_8;
    tmp_uintptr = sec2_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec2_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec2_cfg_buf, p_flow_tbl_list->p_CR_buf_8, CR_SIZE_8);
    p_step_cr->sec2_crs[4].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec2 cr[4] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_step_cr->sec2_crs[4].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_8), DC_U, DC_U);

    filter_offset = CR_OFFSET_9;
    tmp_uintptr = sec2_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec2_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec2_cfg_buf, p_flow_tbl_list->p_CR_buf_9, CR_SIZE_9);
    p_step_cr->sec2_crs[5].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec2 cr[5] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_step_cr->sec2_crs[5].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_9), DC_U, DC_U);

    filter_offset = CR_OFFSET_10;
    tmp_uintptr = sec2_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec2_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec2_cfg_buf, p_flow_tbl_list->p_CR_buf_10, CR_SIZE_10);
    p_step_cr->sec2_crs[6].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec2 cr[6] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_step_cr->sec2_crs[6].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_10), DC_U, DC_U);

    filter_offset = CR_OFFSET_11;
    tmp_uintptr = sec2_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec2_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec2_cfg_buf, p_flow_tbl_list->p_CR_buf_11, CR_SIZE_11);
    p_step_cr->sec2_crs[7].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec2 cr[7] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_step_cr->sec2_crs[7].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_11), DC_U, DC_U);

    filter_offset = CR_OFFSET_12;
    tmp_uintptr = sec2_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec2_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec2_cfg_buf, p_flow_tbl_list->p_CR_buf_12, CR_SIZE_12);
    p_step_cr->sec2_crs[8].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec2 cr[8] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_step_cr->sec2_crs[8].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_12), DC_U, DC_U);

    filter_offset = CR_OFFSET_13;
    tmp_uintptr = sec2_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec2_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec2_cfg_buf, p_flow_tbl_list->p_CR_buf_13, CR_SIZE_13);
    p_step_cr->sec2_crs[9].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec2 cr[9] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_step_cr->sec2_crs[9].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_13), DC_U, DC_U);

    filter_offset = CR_OFFSET_14;
    tmp_uintptr = sec2_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec2_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec2_cfg_buf, p_flow_tbl_list->p_CR_buf_14, CR_SIZE_14);
    p_step_cr->sec2_crs[10].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec2 cr[10] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_step_cr->sec2_crs[10].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_14), DC_U, DC_U);

    filter_offset = CR_OFFSET_15;
    tmp_uintptr = sec2_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec2_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec2_cfg_buf, p_flow_tbl_list->p_CR_buf_15, CR_SIZE_15);
    p_step_cr->sec2_crs[11].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec2 cr[11] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_step_cr->sec2_crs[11].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_15), DC_U, DC_U);

    filter_offset = CR_OFFSET_16;
    tmp_uintptr = sec2_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec2_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec2_cfg_buf, p_flow_tbl_list->p_CR_buf_16, CR_SIZE_16);
    p_step_cr->sec2_crs[12].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec2 cr[12] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_step_cr->sec2_crs[12].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_16), DC_U, DC_U);

    filter_offset = CR_OFFSET_17;
    tmp_uintptr = sec2_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec2_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec2_cfg_buf, p_flow_tbl_list->p_CR_buf_17, CR_SIZE_17);
    p_step_cr->sec2_crs[13].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec2 cr[13] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_step_cr->sec2_crs[13].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_17), DC_U, DC_U);

    filter_offset = CR_OFFSET_18;
    tmp_uintptr = sec2_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec2_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec2_cfg_buf, p_flow_tbl_list->p_CR_buf_18, CR_SIZE_18);
    p_step_cr->sec2_crs[14].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec2 cr[14] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_step_cr->sec2_crs[14].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_18), DC_U, DC_U);

    filter_offset = CR_OFFSET_19;
    tmp_uintptr = sec2_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec2_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec2_cfg_buf, p_flow_tbl_list->p_CR_buf_19, CR_SIZE_19);
    p_step_cr->sec2_crs[15].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec2 cr[15] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_step_cr->sec2_crs[15].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_19), DC_U, DC_U);

    filter_offset = CR_OFFSET_20;
    tmp_uintptr = sec2_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec2_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec2_cfg_buf, p_flow_tbl_list->p_CR_buf_20, CR_SIZE_20);
    p_step_cr->sec2_crs[16].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec2 cr[16] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_step_cr->sec2_crs[16].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_20), DC_U, DC_U);

    filter_offset = CR_OFFSET_21;
    tmp_uintptr = sec2_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec2_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec2_cfg_buf, p_flow_tbl_list->p_CR_buf_21, CR_SIZE_21);
    p_step_cr->sec2_crs[17].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec2 cr[17] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_step_cr->sec2_crs[17].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_21), DC_U, DC_U);

    filter_offset = CR_OFFSET_22;
    tmp_uintptr = sec2_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec2_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec2_cfg_buf, p_flow_tbl_list->p_CR_buf_22, CR_SIZE_22);
    p_step_cr->sec2_crs[18].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec2 cr[18] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_step_cr->sec2_crs[18].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_22), DC_U, DC_U);

    filter_offset = CR_OFFSET_23;
    tmp_uintptr = sec2_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec2_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec2_cfg_buf, p_flow_tbl_list->p_CR_buf_23, CR_SIZE_23);
    p_step_cr->sec2_crs[19].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec2 cr[19] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_step_cr->sec2_crs[19].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_23), DC_U, DC_U);

    filter_offset = CR_OFFSET_24;
    tmp_uintptr = sec2_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec2_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec2_cfg_buf, p_flow_tbl_list->p_CR_buf_24, CR_SIZE_24);
    p_step_cr->sec2_crs[20].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec2 cr[20] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_step_cr->sec2_crs[20].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_24), DC_U, DC_U);

    filter_offset = CR_OFFSET_25;
    tmp_uintptr = sec2_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec2_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec2_cfg_buf, p_flow_tbl_list->p_CR_buf_25, CR_SIZE_25);
    p_step_cr->sec2_crs[21].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec2 cr[21] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_step_cr->sec2_crs[21].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_25), DC_U, DC_U);

    filter_offset = CR_OFFSET_26;
    tmp_uintptr = sec2_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec2_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec2_cfg_buf, p_flow_tbl_list->p_CR_buf_26, CR_SIZE_26);
    p_step_cr->sec2_crs[22].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec2 cr[22] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_step_cr->sec2_crs[22].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_26), DC_U, DC_U);

    filter_offset = CR_OFFSET_27;
    tmp_uintptr = sec2_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec2_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec2_cfg_buf, p_flow_tbl_list->p_CR_buf_27, CR_SIZE_27);
    p_step_cr->sec2_crs[23].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec2 cr[23] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_step_cr->sec2_crs[23].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_27), DC_U, DC_U);

    filter_offset = CR_OFFSET_28;
    tmp_uintptr = sec2_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec2_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec2_cfg_buf, p_flow_tbl_list->p_CR_buf_28, CR_SIZE_28);
    p_step_cr->sec2_crs[24].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec2 cr[24] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_step_cr->sec2_crs[24].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_28), DC_U, DC_U);

    filter_offset = CR_OFFSET_29;
    tmp_uintptr = sec2_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec2_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec2_cfg_buf, p_flow_tbl_list->p_CR_buf_29, CR_SIZE_29);
    p_step_cr->sec2_crs[25].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec2 cr[25] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_step_cr->sec2_crs[25].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_29), DC_U, DC_U);

    filter_offset = CR_OFFSET_30;
    tmp_uintptr = sec2_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec2_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec2_cfg_buf, p_flow_tbl_list->p_CR_buf_30, CR_SIZE_30);
    p_step_cr->sec2_crs[26].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec2 cr[26] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_step_cr->sec2_crs[26].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_30), DC_U, DC_U);

    filter_offset = CR_OFFSET_31;
    tmp_uintptr = sec2_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec2_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec2_cfg_buf, p_flow_tbl_list->p_CR_buf_31, CR_SIZE_31);
    p_step_cr->sec2_crs[27].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec2 cr[27] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_step_cr->sec2_crs[27].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_31), DC_U, DC_U);

    filter_offset = CR_OFFSET_32;
    tmp_uintptr = sec2_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec2_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec2_cfg_buf, p_flow_tbl_list->p_CR_buf_32, CR_SIZE_32);
    p_step_cr->sec2_crs[28].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec2 cr[28] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_step_cr->sec2_crs[28].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_32), DC_U, DC_U);

    filter_offset = CR_OFFSET_33;
    tmp_uintptr = sec2_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec2_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec2_cfg_buf, p_flow_tbl_list->p_CR_buf_33, CR_SIZE_33);
    p_step_cr->sec2_crs[29].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec2 cr[29] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_step_cr->sec2_crs[29].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_33), DC_U, DC_U);

    filter_offset = CR_OFFSET_34;
    tmp_uintptr = sec2_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec2_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec2_cfg_buf, p_flow_tbl_list->p_CR_buf_34, CR_SIZE_34);
    p_step_cr->sec2_crs[30].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec2 cr[30] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_step_cr->sec2_crs[30].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_34), DC_U, DC_U);

    filter_offset = CR_OFFSET_35;
    tmp_uintptr = sec2_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec2_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec2_cfg_buf, p_flow_tbl_list->p_CR_buf_35, CR_SIZE_35);
    p_step_cr->sec2_crs[31].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec2 cr[31] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_step_cr->sec2_crs[31].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_35), DC_U, DC_U);

    filter_offset = CR_OFFSET_40;
    tmp_uintptr = sec2_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec2_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec2_cfg_buf, p_flow_tbl_list->p_CR_buf_40, CR_SIZE_40);
    p_step_cr->sec2_crs[32].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec2 cr[32] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_step_cr->sec2_crs[32].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_40), DC_U, DC_U);

    filter_offset = CR_OFFSET_36;
    tmp_uintptr = sec2_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec2_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec2_cfg_buf, p_flow_tbl_list->p_CR_buf_36, CR_SIZE_36);
    p_step_cr->sec2_crs[33].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec2 cr[33] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_step_cr->sec2_crs[33].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_36), DC_U, DC_U);

    filter_offset = CR_OFFSET_37;
    tmp_uintptr = sec2_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec2_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec2_cfg_buf, p_flow_tbl_list->p_CR_buf_37, CR_SIZE_37);
    p_step_cr->sec2_crs[34].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec2 cr[34] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_step_cr->sec2_crs[34].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_37), DC_U, DC_U);

    filter_offset = CR_OFFSET_38;
    tmp_uintptr = sec2_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec2_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec2_cfg_buf, p_flow_tbl_list->p_CR_buf_38, CR_SIZE_38);
    p_step_cr->sec2_crs[35].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec2 cr[35] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_step_cr->sec2_crs[35].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_38), DC_U, DC_U);

    filter_offset = CR_OFFSET_39;
    tmp_uintptr = sec2_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec2_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec2_cfg_buf, p_flow_tbl_list->p_CR_buf_39, CR_SIZE_39);
    p_step_cr->sec2_crs[36].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec2 cr[36] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_step_cr->sec2_crs[36].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_39), DC_U, DC_U);

    filter_offset = CR_OFFSET_41;
    tmp_uintptr = sec2_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec2_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec2_cfg_buf, p_flow_tbl_list->p_CR_buf_41, CR_SIZE_41);
    p_step_cr->sec2_crs[37].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec2 cr[37] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_step_cr->sec2_crs[37].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_41), DC_U, DC_U);
}

static void img_cfg_hiso_r2y_sec4_amalgam_data_update(uintptr data_offset_base,
        idsp_step_crs_t *p_step_cr,
        uint8_t *p_cr,
        const amba_ik_sec4_cr_list_t *p_flow_tbl_list)
{
    uint32_t filter_offset;
    uintptr sec4_cfg_buf_offset;
    void* p_sec4_cfg_buf;
    uintptr tmp_uintptr;

    (void)amba_ik_system_memcpy(&sec4_cfg_buf_offset, &p_cr, sizeof(tmp_uintptr));

    filter_offset = CR_OFFSET_45;
    tmp_uintptr = sec4_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec4_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec4_cfg_buf, p_flow_tbl_list->p_CR_buf_45, CR_SIZE_45);
    p_step_cr->sec4_crs[0].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec4 cr[0] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_step_cr->sec4_crs[0].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_45), DC_U, DC_U);

    filter_offset = CR_OFFSET_46;
    tmp_uintptr = sec4_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec4_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec4_cfg_buf, p_flow_tbl_list->p_CR_buf_46, CR_SIZE_46);
    p_step_cr->sec4_crs[1].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec4 cr[1] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_step_cr->sec4_crs[1].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_46), DC_U, DC_U);

    filter_offset = CR_OFFSET_47;
    tmp_uintptr = sec4_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec4_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec4_cfg_buf, p_flow_tbl_list->p_CR_buf_47, CR_SIZE_47);
    p_step_cr->sec4_crs[2].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec4 cr[2] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_step_cr->sec4_crs[2].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_47), DC_U, DC_U);

    filter_offset = CR_OFFSET_48;
    tmp_uintptr = sec4_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec4_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec4_cfg_buf, p_flow_tbl_list->p_CR_buf_48, CR_SIZE_48);
    p_step_cr->sec4_crs[3].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec4 cr[3] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_step_cr->sec4_crs[3].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_48), DC_U, DC_U);

    filter_offset = CR_OFFSET_49;
    tmp_uintptr = sec4_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec4_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec4_cfg_buf, p_flow_tbl_list->p_CR_buf_49, CR_SIZE_49);
    p_step_cr->sec4_crs[4].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec4 cr[4] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_step_cr->sec4_crs[4].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_49), DC_U, DC_U);

    filter_offset = CR_OFFSET_50;
    tmp_uintptr = sec4_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec4_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec4_cfg_buf, p_flow_tbl_list->p_CR_buf_50, CR_SIZE_50);
    p_step_cr->sec4_crs[5].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec4 cr[5] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_step_cr->sec4_crs[5].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_50), DC_U, DC_U);

    filter_offset = CR_OFFSET_51;
    tmp_uintptr = sec4_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec4_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec4_cfg_buf, p_flow_tbl_list->p_CR_buf_51, CR_SIZE_51);
    p_step_cr->sec4_crs[6].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec4 cr[6] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_step_cr->sec4_crs[6].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_51), DC_U, DC_U);

    filter_offset = CR_OFFSET_52;
    tmp_uintptr = sec4_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec4_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec4_cfg_buf, p_flow_tbl_list->p_CR_buf_52, CR_SIZE_52);
    p_step_cr->sec4_crs[7].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec4 cr[7] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_step_cr->sec4_crs[7].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_52), DC_U, DC_U);
}

static void img_cfg_hiso_y2y_sec2_amalgam_data_update(uintptr data_offset_base,
        idsp_step_crs_t *p_step_cr,
        uint8_t *p_cr,
        const amba_ik_step3_sec2_cr_list_t *p_flow_tbl_list)
{
    uint32_t filter_offset;
    uintptr sec2_cfg_buf_offset;
    void* p_sec2_cfg_buf;
    uintptr tmp_uintptr;

    (void)amba_ik_system_memcpy(&sec2_cfg_buf_offset, &p_cr, sizeof(tmp_uintptr));

    filter_offset = MODE_8_SEC_2_CR_OFFSET_26;
    tmp_uintptr = sec2_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec2_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec2_cfg_buf, p_flow_tbl_list->p_CR_buf_26, CR_SIZE_26);
    p_step_cr->sec2_crs[22].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec2 cr[22] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_step_cr->sec2_crs[22].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_26), DC_U, DC_U);

    filter_offset = MODE_8_SEC_2_CR_OFFSET_29;
    tmp_uintptr = sec2_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec2_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec2_cfg_buf, p_flow_tbl_list->p_CR_buf_29, CR_SIZE_29);
    p_step_cr->sec2_crs[25].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec2 cr[25] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_step_cr->sec2_crs[25].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_29), DC_U, DC_U);

    filter_offset = MODE_8_SEC_2_CR_OFFSET_30;
    tmp_uintptr = sec2_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec2_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec2_cfg_buf, p_flow_tbl_list->p_CR_buf_30, CR_SIZE_30);
    p_step_cr->sec2_crs[26].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec2 cr[26] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_step_cr->sec2_crs[26].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_30), DC_U, DC_U);

    filter_offset = MODE_8_SEC_2_CR_OFFSET_31;
    tmp_uintptr = sec2_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec2_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec2_cfg_buf, p_flow_tbl_list->p_CR_buf_31, CR_SIZE_31);
    p_step_cr->sec2_crs[27].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec2 cr[27] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_step_cr->sec2_crs[27].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_31), DC_U, DC_U);

    filter_offset = MODE_8_SEC_2_CR_OFFSET_32;
    tmp_uintptr = sec2_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec2_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec2_cfg_buf, p_flow_tbl_list->p_CR_buf_32, CR_SIZE_32);
    p_step_cr->sec2_crs[28].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec2 cr[28] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_step_cr->sec2_crs[28].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_32), DC_U, DC_U);

    filter_offset = MODE_8_SEC_2_CR_OFFSET_33;
    tmp_uintptr = sec2_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec2_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec2_cfg_buf, p_flow_tbl_list->p_CR_buf_33, CR_SIZE_33);
    p_step_cr->sec2_crs[29].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec2 cr[29] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_step_cr->sec2_crs[29].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_33), DC_U, DC_U);

    filter_offset = MODE_8_SEC_2_CR_OFFSET_34;
    tmp_uintptr = sec2_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec2_cfg_buf, &tmp_uintptr, sizeof(void*));
    //(void)amba_ik_system_memcpy(p_sec2_cfg_buf, p_flow_tbl_list->p_CR_buf_34, CR_SIZE_34);
    p_step_cr->sec2_crs[30].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec2 cr[30] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_step_cr->sec2_crs[30].config_region_addr << 7, filter_offset, DC_U, DC_U, DC_U);

    filter_offset = MODE_8_SEC_2_CR_OFFSET_35;
    tmp_uintptr = sec2_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec2_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec2_cfg_buf, p_flow_tbl_list->p_CR_buf_35, CR_SIZE_35);
    p_step_cr->sec2_crs[31].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec2 cr[31] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_step_cr->sec2_crs[31].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_35), DC_U, DC_U);

    filter_offset = MODE_8_SEC_2_CR_OFFSET_40;
    tmp_uintptr = sec2_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec2_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec2_cfg_buf, p_flow_tbl_list->p_CR_buf_40, CR_SIZE_40);
    p_step_cr->sec2_crs[32].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec2 cr[32] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_step_cr->sec2_crs[32].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_40), DC_U, DC_U);

    filter_offset = MODE_8_SEC_2_CR_OFFSET_36;
    tmp_uintptr = sec2_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec2_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec2_cfg_buf, p_flow_tbl_list->p_CR_buf_36, CR_SIZE_36);
    p_step_cr->sec2_crs[33].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec2 cr[33] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_step_cr->sec2_crs[33].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_36), DC_U, DC_U);

    filter_offset = MODE_8_SEC_2_CR_OFFSET_37;
    tmp_uintptr = sec2_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec2_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec2_cfg_buf, p_flow_tbl_list->p_CR_buf_37, CR_SIZE_37);
    p_step_cr->sec2_crs[34].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec2 cr[34] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_step_cr->sec2_crs[34].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_37), DC_U, DC_U);

    filter_offset = MODE_8_SEC_2_CR_OFFSET_38;
    tmp_uintptr = sec2_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec2_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec2_cfg_buf, p_flow_tbl_list->p_CR_buf_38, CR_SIZE_38);
    p_step_cr->sec2_crs[35].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec2 cr[35] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_step_cr->sec2_crs[35].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_38), DC_U, DC_U);

    filter_offset = MODE_8_SEC_2_CR_OFFSET_39;
    tmp_uintptr = sec2_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec2_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec2_cfg_buf, p_flow_tbl_list->p_CR_buf_39, CR_SIZE_39);
    p_step_cr->sec2_crs[36].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec2 cr[36] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_step_cr->sec2_crs[36].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_39), DC_U, DC_U);

    filter_offset = MODE_8_SEC_2_CR_OFFSET_41;
    tmp_uintptr = sec2_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec2_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec2_cfg_buf, p_flow_tbl_list->p_CR_buf_41, CR_SIZE_41);
    p_step_cr->sec2_crs[37].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec2 cr[37] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_step_cr->sec2_crs[37].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_41), DC_U, DC_U);
}

// every steps api
static void img_cfg_hiso_step1_amalgam_data_update(uintptr data_offset_base,
        idsp_step_crs_t *p_step_cr,
        amalgam_hiso_step1_flow_data_t *p_cr,
        const amba_ik_hiso_step1_cr_list_t *p_flow_tbl_list)
{
    uint32_t filter_offset;
    const uint8_t *tmp_p_u8;
    uintptr sec18_cfg_buf_offset;
    void* p_sec18_cfg_buf;
    uintptr tmp_uintptr;

    // IKC does not use IDSP_CONFIG_HDR_SIZE, however, IKS does, in order to keep it on the same page between IKC and IKS,
    tmp_p_u8 = p_cr->sec18_cfg_buf;
    (void)amba_ik_system_memcpy(&sec18_cfg_buf_offset, &tmp_p_u8, sizeof(tmp_uintptr));

    img_cfg_hiso_r2y_sec2_amalgam_data_update(data_offset_base, p_step_cr, p_cr->sec2_cfg_buf, &p_flow_tbl_list->sec2);
    img_cfg_hiso_r2y_sec4_amalgam_data_update(data_offset_base, p_step_cr, p_cr->sec4_cfg_buf, &p_flow_tbl_list->sec4);

    filter_offset = MODE_4_SEC_18_CR_OFFSET_112;
    tmp_uintptr = sec18_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec18_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec18_cfg_buf, p_flow_tbl_list->sec18.p_CR_buf_112, CR_SIZE_112);
    p_step_cr->sec18_crs[1].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec18 cr[1] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_step_cr->sec18_crs[1].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->sec18.p_CR_buf_112), DC_U, DC_U);

    filter_offset = MODE_4_SEC_18_CR_OFFSET_113;
    tmp_uintptr = sec18_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec18_cfg_buf, &tmp_uintptr, sizeof(void*));
    //(void)amba_ik_system_memcpy(p_sec18_cfg_buf, p_flow_tbl_list->sec18.p_CR_buf_113, CR_SIZE_113);
    //p_step_cr->sec18_crs[2].config_region_addr = (tmp_uintptr - data_offset_base) >> 7U;
    amba_ik_system_print_uint32_5("Sec18 cr[2] addr 0x%08x, offset %d, src_addr 0x%08x ",
                                  p_step_cr->sec18_crs[2].config_region_addr << 7, filter_offset, DC_U, DC_U, DC_U);

    filter_offset = MODE_4_SEC_18_CR_OFFSET_114;
    tmp_uintptr = sec18_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec18_cfg_buf, &tmp_uintptr, sizeof(void*));
    //(void)amba_ik_system_memcpy(p_sec18_cfg_buf, p_flow_tbl_list->sec18.p_CR_buf_114, CR_SIZE_114);
    //p_step_cr->sec18_crs[3].config_region_addr = (tmp_uintptr - data_offset_base) >> 7U;
    amba_ik_system_print_uint32_5("Sec18 cr[3] addr 0x%08x, offset %d, src_addr 0x%08x ",
                                  p_step_cr->sec18_crs[3].config_region_addr << 7, filter_offset, DC_U, DC_U, DC_U);

    filter_offset = MODE_4_SEC_18_CR_OFFSET_115;
    tmp_uintptr = sec18_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec18_cfg_buf, &tmp_uintptr, sizeof(void*));
    //(void)amba_ik_system_memcpy(p_sec18_cfg_buf, p_flow_tbl_list->sec18.p_CR_buf_115, CR_SIZE_115);
    //p_step_cr->sec18_crs[4].config_region_addr = (tmp_uintptr - data_offset_base) >> 7U;
    amba_ik_system_print_uint32_5("Sec18 cr[4] addr 0x%08x, offset %d, src_addr 0x%08x ",
                                  p_step_cr->sec18_crs[4].config_region_addr << 7, filter_offset, DC_U, DC_U, DC_U);

    filter_offset = MODE_4_SEC_18_CR_OFFSET_117;
    tmp_uintptr = sec18_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec18_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec18_cfg_buf, p_flow_tbl_list->sec18.p_CR_buf_117, CR_SIZE_117);
    p_step_cr->sec18_crs[6].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec18 cr[6] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_step_cr->sec18_crs[6].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->sec18.p_CR_buf_117), DC_U, DC_U);

    filter_offset = MODE_4_SEC_18_CR_OFFSET_118;
    tmp_uintptr = sec18_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec18_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec18_cfg_buf, p_flow_tbl_list->sec18.p_CR_buf_118, CR_SIZE_118);
    p_step_cr->sec18_crs[7].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec18 cr[7] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_step_cr->sec18_crs[7].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->sec18.p_CR_buf_118), DC_U, DC_U);

    filter_offset = MODE_4_SEC_18_CR_OFFSET_119;
    tmp_uintptr = sec18_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec18_cfg_buf, &tmp_uintptr, sizeof(void*));
    //(void)amba_ik_system_memcpy(p_sec18_cfg_buf, p_flow_tbl_list->sec18.p_CR_buf_119, CR_SIZE_119);
    //p_step_cr->sec18_crs[8].config_region_addr = (tmp_uintptr - data_offset_base) >> 7U;
    amba_ik_system_print_uint32_5("Sec18 cr[8] addr 0x%08x, offset %d, src_addr 0x%08x ",
                                  p_step_cr->sec18_crs[8].config_region_addr << 7, filter_offset, DC_U, DC_U, DC_U);
}

static void img_cfg_hiso_step2_amalgam_data_update(uintptr data_offset_base,
        idsp_step_crs_t *p_step_cr,
        const amalgam_hiso_step2_flow_data_t *p_cr,
        const amba_ik_hiso_step2_cr_list_t *p_flow_tbl_list)
{
    uint32_t filter_offset;
    const uint8_t *tmp_p_u8;
    uintptr sec2_cfg_buf_offset;
    uintptr sec18_cfg_buf_offset;
    void* p_sec2_cfg_buf;
    void* p_sec18_cfg_buf;
    uintptr tmp_uintptr;

    // IKC does not use IDSP_CONFIG_HDR_SIZE, however, IKS does, in order to keep it on the same page between IKC and IKS,
    tmp_p_u8 = p_cr->sec2_cfg_buf;
    (void)amba_ik_system_memcpy(&sec2_cfg_buf_offset, &tmp_p_u8, sizeof(tmp_uintptr));
    tmp_p_u8 = p_cr->sec18_cfg_buf;
    (void)amba_ik_system_memcpy(&sec18_cfg_buf_offset, &tmp_p_u8, sizeof(tmp_uintptr));

    filter_offset = MODE_8_SEC_2_CR_OFFSET_26;
    tmp_uintptr = sec2_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec2_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec2_cfg_buf, p_flow_tbl_list->sec2.p_CR_buf_26, CR_SIZE_26);
    p_step_cr->sec2_crs[22].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec2 cr[22] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_step_cr->sec2_crs[22].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->sec2.p_CR_buf_26), DC_U, DC_U);

    filter_offset = MODE_8_SEC_2_CR_OFFSET_27;
    tmp_uintptr = sec2_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec2_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec2_cfg_buf, p_flow_tbl_list->sec2.p_CR_buf_27, CR_SIZE_27);
    p_step_cr->sec2_crs[23].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec2 cr[23] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_step_cr->sec2_crs[23].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->sec2.p_CR_buf_27), DC_U, DC_U);

    filter_offset = MODE_8_SEC_2_CR_OFFSET_29;
    tmp_uintptr = sec2_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec2_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec2_cfg_buf, p_flow_tbl_list->sec2.p_CR_buf_29, CR_SIZE_29);
    p_step_cr->sec2_crs[25].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec2 cr[25] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_step_cr->sec2_crs[25].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->sec2.p_CR_buf_29), DC_U, DC_U);

    filter_offset = MODE_8_SEC_2_CR_OFFSET_30;
    tmp_uintptr = sec2_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec2_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec2_cfg_buf, p_flow_tbl_list->sec2.p_CR_buf_30, CR_SIZE_30);
    p_step_cr->sec2_crs[26].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec2 cr[26] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_step_cr->sec2_crs[26].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->sec2.p_CR_buf_30), DC_U, DC_U);

    filter_offset = MODE_8_SEC_2_CR_OFFSET_31;
    tmp_uintptr = sec2_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec2_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec2_cfg_buf, p_flow_tbl_list->sec2.p_CR_buf_31, CR_SIZE_31);
    p_step_cr->sec2_crs[27].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec2 cr[27] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_step_cr->sec2_crs[27].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->sec2.p_CR_buf_31), DC_U, DC_U);

    filter_offset = MODE_8_SEC_2_CR_OFFSET_32;
    tmp_uintptr = sec2_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec2_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec2_cfg_buf, p_flow_tbl_list->sec2.p_CR_buf_32, CR_SIZE_32);
    p_step_cr->sec2_crs[28].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec2 cr[28] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_step_cr->sec2_crs[28].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->sec2.p_CR_buf_32), DC_U, DC_U);

    filter_offset = MODE_8_SEC_2_CR_OFFSET_33;
    tmp_uintptr = sec2_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec2_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec2_cfg_buf, p_flow_tbl_list->sec2.p_CR_buf_33, CR_SIZE_33);
    p_step_cr->sec2_crs[29].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec2 cr[29] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_step_cr->sec2_crs[29].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->sec2.p_CR_buf_33), DC_U, DC_U);

    filter_offset = MODE_8_SEC_2_CR_OFFSET_34;
    tmp_uintptr = sec2_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec2_cfg_buf, &tmp_uintptr, sizeof(void*));
    //(void)amba_ik_system_memcpy(p_sec2_cfg_buf, p_flow_tbl_list->sec2.p_CR_buf_34, CR_SIZE_34);
    p_step_cr->sec2_crs[30].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint32_5("Sec2 cr[30] addr 0x%08x, offset %d, src_addr 0x%08x ",
                                  p_step_cr->sec2_crs[30].config_region_addr << 7, filter_offset, DC_U, DC_U, DC_U);

    filter_offset = MODE_8_SEC_2_CR_OFFSET_35;
    tmp_uintptr = sec2_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec2_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec2_cfg_buf, p_flow_tbl_list->sec2.p_CR_buf_35, CR_SIZE_35);
    p_step_cr->sec2_crs[31].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec2 cr[31] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_step_cr->sec2_crs[31].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->sec2.p_CR_buf_35), DC_U, DC_U);

    filter_offset = MODE_8_SEC_2_CR_OFFSET_40;
    tmp_uintptr = sec2_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec2_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec2_cfg_buf, p_flow_tbl_list->sec2.p_CR_buf_40, CR_SIZE_40);
    p_step_cr->sec2_crs[32].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec2 cr[32] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_step_cr->sec2_crs[32].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->sec2.p_CR_buf_40), DC_U, DC_U);

    filter_offset = MODE_8_SEC_2_CR_OFFSET_36;
    tmp_uintptr = sec2_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec2_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec2_cfg_buf, p_flow_tbl_list->sec2.p_CR_buf_36, CR_SIZE_36);
    p_step_cr->sec2_crs[33].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec2 cr[33] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_step_cr->sec2_crs[33].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->sec2.p_CR_buf_36), DC_U, DC_U);

    filter_offset = MODE_8_SEC_2_CR_OFFSET_37;
    tmp_uintptr = sec2_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec2_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec2_cfg_buf, p_flow_tbl_list->sec2.p_CR_buf_37, CR_SIZE_37);
    p_step_cr->sec2_crs[34].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec2 cr[34] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_step_cr->sec2_crs[34].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->sec2.p_CR_buf_37), DC_U, DC_U);

    filter_offset = MODE_8_SEC_2_CR_OFFSET_38;
    tmp_uintptr = sec2_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec2_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec2_cfg_buf, p_flow_tbl_list->sec2.p_CR_buf_38, CR_SIZE_38);
    p_step_cr->sec2_crs[35].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec2 cr[35] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_step_cr->sec2_crs[35].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->sec2.p_CR_buf_38), DC_U, DC_U);

    filter_offset = MODE_8_SEC_2_CR_OFFSET_39;
    tmp_uintptr = sec2_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec2_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec2_cfg_buf, p_flow_tbl_list->sec2.p_CR_buf_39, CR_SIZE_39);
    p_step_cr->sec2_crs[36].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec2 cr[36] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_step_cr->sec2_crs[36].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->sec2.p_CR_buf_39), DC_U, DC_U);

    filter_offset = MODE_8_SEC_2_CR_OFFSET_41;
    tmp_uintptr = sec2_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec2_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec2_cfg_buf, p_flow_tbl_list->sec2.p_CR_buf_41, CR_SIZE_41);
    p_step_cr->sec2_crs[37].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec2 cr[37] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_step_cr->sec2_crs[37].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->sec2.p_CR_buf_41), DC_U, DC_U);

    filter_offset = MODE_4_SEC_18_CR_OFFSET_112;
    tmp_uintptr = sec18_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec18_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec18_cfg_buf, p_flow_tbl_list->sec18.p_CR_buf_112, CR_SIZE_112);
    p_step_cr->sec18_crs[1].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec18 cr[1] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_step_cr->sec18_crs[1].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->sec18.p_CR_buf_112), DC_U, DC_U);

    filter_offset = MODE_4_SEC_18_CR_OFFSET_113;
    tmp_uintptr = sec18_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec18_cfg_buf, &tmp_uintptr, sizeof(void*));
    //(void)amba_ik_system_memcpy(p_sec18_cfg_buf, p_flow_tbl_list->sec18.p_CR_buf_113, CR_SIZE_113);
    //p_step_cr->sec18_crs[2].config_region_addr = (tmp_uintptr - data_offset_base) >> 7U;
    amba_ik_system_print_uint32_5("Sec18 cr[2] addr 0x%08x, offset %d, src_addr 0x%08x ",
                                  p_step_cr->sec18_crs[2].config_region_addr << 7, filter_offset, DC_U, DC_U, DC_U);

    filter_offset = MODE_4_SEC_18_CR_OFFSET_114;
    tmp_uintptr = sec18_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec18_cfg_buf, &tmp_uintptr, sizeof(void*));
    //(void)amba_ik_system_memcpy(p_sec18_cfg_buf, p_flow_tbl_list->sec18.p_CR_buf_114, CR_SIZE_114);
    //p_step_cr->sec18_crs[3].config_region_addr = (tmp_uintptr - data_offset_base) >> 7U;
    amba_ik_system_print_uint32_5("Sec18 cr[3] addr 0x%08x, offset %d, src_addr 0x%08x ",
                                  p_step_cr->sec18_crs[3].config_region_addr << 7, filter_offset, DC_U, DC_U, DC_U);

    filter_offset = MODE_4_SEC_18_CR_OFFSET_115;
    tmp_uintptr = sec18_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec18_cfg_buf, &tmp_uintptr, sizeof(void*));
    //(void)amba_ik_system_memcpy(p_sec18_cfg_buf, p_flow_tbl_list->sec18.p_CR_buf_115, CR_SIZE_115);
    //p_step_cr->sec18_crs[4].config_region_addr = (tmp_uintptr - data_offset_base) >> 7U;
    amba_ik_system_print_uint32_5("Sec18 cr[4] addr 0x%08x, offset %d, src_addr 0x%08x ",
                                  p_step_cr->sec18_crs[4].config_region_addr << 7, filter_offset, DC_U, DC_U, DC_U);

    filter_offset = MODE_4_SEC_18_CR_OFFSET_117;
    tmp_uintptr = sec18_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec18_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec18_cfg_buf, p_flow_tbl_list->sec18.p_CR_buf_117, CR_SIZE_117);
    p_step_cr->sec18_crs[6].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec18 cr[6] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_step_cr->sec18_crs[6].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->sec18.p_CR_buf_117), DC_U, DC_U);

    filter_offset = MODE_4_SEC_18_CR_OFFSET_118;
    tmp_uintptr = sec18_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec18_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec18_cfg_buf, p_flow_tbl_list->sec18.p_CR_buf_118, CR_SIZE_118);
    p_step_cr->sec18_crs[7].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec18 cr[7] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_step_cr->sec18_crs[7].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->sec18.p_CR_buf_118), DC_U, DC_U);

    filter_offset = MODE_4_SEC_18_CR_OFFSET_119;
    tmp_uintptr = sec18_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec18_cfg_buf, &tmp_uintptr, sizeof(void*));
    //(void)amba_ik_system_memcpy(p_sec18_cfg_buf, p_flow_tbl_list->sec18.p_CR_buf_119, CR_SIZE_119);
    //p_step_cr->sec18_crs[8].config_region_addr = (tmp_uintptr - data_offset_base) >> 7U;
    amba_ik_system_print_uint32_5("Sec18 cr[8] addr 0x%08x, offset %d, src_addr 0x%08x ",
                                  p_step_cr->sec18_crs[8].config_region_addr << 7, filter_offset, DC_U, DC_U, DC_U);
}

static void img_cfg_hiso_step3_amalgam_data_update(uintptr data_offset_base,
        idsp_step_crs_t *p_step_cr,
        amalgam_hiso_step3_flow_data_t *p_cr,
        const amba_ik_hiso_step3_cr_list_t *p_flow_tbl_list)
{
    uint32_t filter_offset;
    const uint8_t *tmp_p_u8;
    uintptr sec2_cfg_buf_offset;
    uintptr sec18_cfg_buf_offset;
    void* p_sec2_cfg_buf;
    void* p_sec18_cfg_buf;
    uintptr tmp_uintptr;

    // IKC does not use IDSP_CONFIG_HDR_SIZE, however, IKS does, in order to keep it on the same page between IKC and IKS,
    tmp_p_u8 = p_cr->sec2_cfg_buf;
    (void)amba_ik_system_memcpy(&sec2_cfg_buf_offset, &tmp_p_u8, sizeof(tmp_uintptr));
    tmp_p_u8 = p_cr->sec18_cfg_buf;
    (void)amba_ik_system_memcpy(&sec18_cfg_buf_offset, &tmp_p_u8, sizeof(tmp_uintptr));

    img_cfg_hiso_y2y_sec2_amalgam_data_update(data_offset_base, p_step_cr, p_cr->sec2_cfg_buf, &p_flow_tbl_list->sec2);

    filter_offset = MODE_8_SEC_2_CR_OFFSET_27;
    tmp_uintptr = sec2_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec2_cfg_buf, &tmp_uintptr, sizeof(void*));
    //(void)amba_ik_system_memcpy(p_sec2_cfg_buf, p_flow_tbl_list->sec2.p_CR_buf_27, CR_SIZE_27);
    p_step_cr->sec2_crs[23].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint32_5("Sec2 cr[23] addr 0x%08x, offset %d, src_addr 0x%08x ",
                                  p_step_cr->sec2_crs[23].config_region_addr << 7, filter_offset, DC_U, DC_U, DC_U);


    filter_offset = MODE_4_SEC_18_CR_OFFSET_112;
    tmp_uintptr = sec18_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec18_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec18_cfg_buf, p_flow_tbl_list->sec18.p_CR_buf_112, CR_SIZE_112);
    p_step_cr->sec18_crs[1].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec18 cr[1] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_step_cr->sec18_crs[1].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->sec18.p_CR_buf_112), DC_U, DC_U);

    filter_offset = MODE_4_SEC_18_CR_OFFSET_113;
    tmp_uintptr = sec18_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec18_cfg_buf, &tmp_uintptr, sizeof(void*));
    //(void)amba_ik_system_memcpy(p_sec18_cfg_buf, p_flow_tbl_list->sec18.p_CR_buf_113, CR_SIZE_113);
    //p_step_cr->sec18_crs[2].config_region_addr = (tmp_uintptr - data_offset_base) >> 7U;
    amba_ik_system_print_uint32_5("Sec18 cr[2] addr 0x%08x, offset %d, src_addr 0x%08x ",
                                  p_step_cr->sec18_crs[2].config_region_addr << 7, filter_offset, DC_U, DC_U, DC_U);

    filter_offset = MODE_4_SEC_18_CR_OFFSET_114;
    tmp_uintptr = sec18_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec18_cfg_buf, &tmp_uintptr, sizeof(void*));
    //(void)amba_ik_system_memcpy(p_sec18_cfg_buf, p_flow_tbl_list->sec18.p_CR_buf_114, CR_SIZE_114);
    //p_step_cr->sec18_crs[3].config_region_addr = (tmp_uintptr - data_offset_base) >> 7U;
    amba_ik_system_print_uint32_5("Sec18 cr[3] addr 0x%08x, offset %d, src_addr 0x%08x ",
                                  p_step_cr->sec18_crs[3].config_region_addr << 7, filter_offset, DC_U, DC_U, DC_U);

    filter_offset = MODE_4_SEC_18_CR_OFFSET_115;
    tmp_uintptr = sec18_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec18_cfg_buf, &tmp_uintptr, sizeof(void*));
    //(void)amba_ik_system_memcpy(p_sec18_cfg_buf, p_flow_tbl_list->sec18.p_CR_buf_115, CR_SIZE_115);
    //p_step_cr->sec18_crs[4].config_region_addr = (tmp_uintptr - data_offset_base) >> 7U;
    amba_ik_system_print_uint32_5("Sec18 cr[4] addr 0x%08x, offset %d, src_addr 0x%08x ",
                                  p_step_cr->sec18_crs[4].config_region_addr << 7, filter_offset, DC_U, DC_U, DC_U);

    filter_offset = MODE_4_SEC_18_CR_OFFSET_117;
    tmp_uintptr = sec18_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec18_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec18_cfg_buf, p_flow_tbl_list->sec18.p_CR_buf_117, CR_SIZE_117);
    p_step_cr->sec18_crs[6].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec18 cr[6] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_step_cr->sec18_crs[6].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->sec18.p_CR_buf_117), DC_U, DC_U);

    filter_offset = MODE_4_SEC_18_CR_OFFSET_118;
    tmp_uintptr = sec18_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec18_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec18_cfg_buf, p_flow_tbl_list->sec18.p_CR_buf_118, CR_SIZE_118);
    p_step_cr->sec18_crs[7].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec18 cr[7] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_step_cr->sec18_crs[7].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->sec18.p_CR_buf_118), DC_U, DC_U);

    filter_offset = MODE_4_SEC_18_CR_OFFSET_119;
    tmp_uintptr = sec18_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec18_cfg_buf, &tmp_uintptr, sizeof(void*));
    //(void)amba_ik_system_memcpy(p_sec18_cfg_buf, p_flow_tbl_list->sec18.p_CR_buf_119, CR_SIZE_119);
    //p_step_cr->sec18_crs[8].config_region_addr = (tmp_uintptr - data_offset_base) >> 7U;
    amba_ik_system_print_uint32_5("Sec18 cr[8] addr 0x%08x, offset %d, src_addr 0x%08x ",
                                  p_step_cr->sec18_crs[8].config_region_addr << 7, filter_offset, DC_U, DC_U, DC_U);
}

static void img_cfg_hiso_step4a_amalgam_data_update(uintptr data_offset_base,
        idsp_step_crs_t *p_step_cr,
        amalgam_hiso_step4a_flow_data_t *p_cr,
        const amba_ik_hiso_step4a_cr_list_t *p_flow_tbl_list)
{
    uint32_t filter_offset;
    const uint8_t *tmp_p_u8;
    uintptr sec2_cfg_buf_offset;
    void* p_sec2_cfg_buf;
    uintptr tmp_uintptr;

    // IKC does not use IDSP_CONFIG_HDR_SIZE, however, IKS does, in order to keep it on the same page between IKC and IKS,
    tmp_p_u8 = p_cr->sec2_cfg_buf;
    (void)amba_ik_system_memcpy(&sec2_cfg_buf_offset, &tmp_p_u8, sizeof(tmp_uintptr));

    img_cfg_hiso_y2y_sec2_amalgam_data_update(data_offset_base, p_step_cr, p_cr->sec2_cfg_buf, &p_flow_tbl_list->sec2);

    filter_offset = MODE_8_SEC_2_CR_OFFSET_27;
    tmp_uintptr = sec2_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec2_cfg_buf, &tmp_uintptr, sizeof(void*));
    //(void)amba_ik_system_memcpy(p_sec2_cfg_buf, p_flow_tbl_list->sec2.p_CR_buf_27, CR_SIZE_27);
    p_step_cr->sec2_crs[23].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint32_5("Sec2 cr[23] addr 0x%08x, offset %d, src_addr 0x%08x ",
                                  p_step_cr->sec2_crs[23].config_region_addr << 7, filter_offset, DC_U, DC_U, DC_U);
}

static void img_cfg_hiso_step5_amalgam_data_update(uintptr data_offset_base,
        idsp_step_crs_t *p_step_cr,
        amalgam_hiso_step5_flow_data_t *p_cr,
        const amba_ik_hiso_step5_cr_list_t *p_flow_tbl_list)
{
    uint32_t filter_offset;
    const uint8_t *tmp_p_u8;
    uintptr sec2_cfg_buf_offset;
    uintptr sec18_cfg_buf_offset;
    void* p_sec2_cfg_buf;
    void* p_sec18_cfg_buf;
    uintptr tmp_uintptr;

    // IKC does not use IDSP_CONFIG_HDR_SIZE, however, IKS does, in order to keep it on the same page between IKC and IKS,
    tmp_p_u8 = p_cr->sec2_cfg_buf;
    (void)amba_ik_system_memcpy(&sec2_cfg_buf_offset, &tmp_p_u8, sizeof(tmp_uintptr));
    tmp_p_u8 = p_cr->sec18_cfg_buf;
    (void)amba_ik_system_memcpy(&sec18_cfg_buf_offset, &tmp_p_u8, sizeof(tmp_uintptr));

    img_cfg_hiso_y2y_sec2_amalgam_data_update(data_offset_base, p_step_cr, p_cr->sec2_cfg_buf, &p_flow_tbl_list->sec2);

    filter_offset = MODE_8_SEC_2_CR_OFFSET_27;
    tmp_uintptr = sec2_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec2_cfg_buf, &tmp_uintptr, sizeof(void*));
    //(void)amba_ik_system_memcpy(p_sec2_cfg_buf, p_flow_tbl_list->sec2.p_CR_buf_27, CR_SIZE_27);
    p_step_cr->sec2_crs[23].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint32_5("Sec2 cr[23] addr 0x%08x, offset %d, src_addr 0x%08x ",
                                  p_step_cr->sec2_crs[23].config_region_addr << 7, filter_offset, DC_U, DC_U, DC_U);


    filter_offset = MODE_4_SEC_18_CR_OFFSET_112;
    tmp_uintptr = sec18_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec18_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec18_cfg_buf, p_flow_tbl_list->sec18.p_CR_buf_112, CR_SIZE_112);
    p_step_cr->sec18_crs[1].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec18 cr[1] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_step_cr->sec18_crs[1].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->sec18.p_CR_buf_112), DC_U, DC_U);

    filter_offset = MODE_4_SEC_18_CR_OFFSET_113;
    tmp_uintptr = sec18_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec18_cfg_buf, &tmp_uintptr, sizeof(void*));
    //(void)amba_ik_system_memcpy(p_sec18_cfg_buf, p_flow_tbl_list->sec18.p_CR_buf_113, CR_SIZE_113);
    //p_step_cr->sec18_crs[2].config_region_addr = (tmp_uintptr - data_offset_base) >> 7U;
    amba_ik_system_print_uint32_5("Sec18 cr[2] addr 0x%08x, offset %d, src_addr 0x%08x ",
                                  p_step_cr->sec18_crs[2].config_region_addr << 7, filter_offset, DC_U, DC_U, DC_U);

    filter_offset = MODE_4_SEC_18_CR_OFFSET_114;
    tmp_uintptr = sec18_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec18_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec18_cfg_buf, p_flow_tbl_list->sec18.p_CR_buf_114, CR_SIZE_114);
    p_step_cr->sec18_crs[3].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint32_5("Sec18 cr[3] addr 0x%08x, offset %d, src_addr 0x%08x ",
                                  p_step_cr->sec18_crs[3].config_region_addr << 7, filter_offset, DC_U, DC_U, DC_U);

    filter_offset = MODE_4_SEC_18_CR_OFFSET_115;
    tmp_uintptr = sec18_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec18_cfg_buf, &tmp_uintptr, sizeof(void*));
    //(void)amba_ik_system_memcpy(p_sec18_cfg_buf, p_flow_tbl_list->sec18.p_CR_buf_115, CR_SIZE_115);
    //p_step_cr->sec18_crs[4].config_region_addr = (tmp_uintptr - data_offset_base) >> 7U;
    amba_ik_system_print_uint32_5("Sec18 cr[4] addr 0x%08x, offset %d, src_addr 0x%08x ",
                                  p_step_cr->sec18_crs[4].config_region_addr << 7, filter_offset, DC_U, DC_U, DC_U);

    filter_offset = MODE_4_SEC_18_CR_OFFSET_117;
    tmp_uintptr = sec18_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec18_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec18_cfg_buf, p_flow_tbl_list->sec18.p_CR_buf_117, CR_SIZE_117);
    p_step_cr->sec18_crs[6].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec18 cr[6] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_step_cr->sec18_crs[6].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->sec18.p_CR_buf_117), DC_U, DC_U);

    filter_offset = MODE_4_SEC_18_CR_OFFSET_118;
    tmp_uintptr = sec18_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec18_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec18_cfg_buf, p_flow_tbl_list->sec18.p_CR_buf_118, CR_SIZE_118);
    p_step_cr->sec18_crs[7].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec18 cr[7] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_step_cr->sec18_crs[7].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->sec18.p_CR_buf_118), DC_U, DC_U);

    filter_offset = MODE_4_SEC_18_CR_OFFSET_119;
    tmp_uintptr = sec18_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec18_cfg_buf, &tmp_uintptr, sizeof(void*));
    //(void)amba_ik_system_memcpy(p_sec18_cfg_buf, p_flow_tbl_list->sec18.p_CR_buf_119, CR_SIZE_119);
    //p_step_cr->sec18_crs[8].config_region_addr = (tmp_uintptr - data_offset_base) >> 7U;
    amba_ik_system_print_uint32_5("Sec18 cr[8] addr 0x%08x, offset %d, src_addr 0x%08x ",
                                  p_step_cr->sec18_crs[8].config_region_addr << 7, filter_offset, DC_U, DC_U, DC_U);
}

static void img_cfg_hiso_step6_amalgam_data_update(uintptr data_offset_base,
        idsp_step_crs_t *p_step_cr,
        amalgam_hiso_step6_flow_data_t *p_cr,
        const amba_ik_hiso_step6_cr_list_t *p_flow_tbl_list)
{
    uint32_t filter_offset;
    const uint8_t *tmp_p_u8;
    uintptr sec2_cfg_buf_offset;
    uintptr sec18_cfg_buf_offset;
    void* p_sec2_cfg_buf;
    void* p_sec18_cfg_buf;
    uintptr tmp_uintptr;

    // IKC does not use IDSP_CONFIG_HDR_SIZE, however, IKS does, in order to keep it on the same page between IKC and IKS,
    tmp_p_u8 = p_cr->sec2_cfg_buf;
    (void)amba_ik_system_memcpy(&sec2_cfg_buf_offset, &tmp_p_u8, sizeof(tmp_uintptr));
    tmp_p_u8 = p_cr->sec18_cfg_buf;
    (void)amba_ik_system_memcpy(&sec18_cfg_buf_offset, &tmp_p_u8, sizeof(tmp_uintptr));

    img_cfg_hiso_y2y_sec2_amalgam_data_update(data_offset_base, p_step_cr, p_cr->sec2_cfg_buf, &p_flow_tbl_list->sec2);

    filter_offset = MODE_8_SEC_2_CR_OFFSET_27;
    tmp_uintptr = sec2_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec2_cfg_buf, &tmp_uintptr, sizeof(void*));
    //(void)amba_ik_system_memcpy(p_sec2_cfg_buf, p_flow_tbl_list->sec2.p_CR_buf_27, CR_SIZE_27);
    p_step_cr->sec2_crs[23].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint32_5("Sec2 cr[23] addr 0x%08x, offset %d, src_addr 0x%08x ",
                                  p_step_cr->sec2_crs[23].config_region_addr << 7, filter_offset, DC_U, DC_U, DC_U);


    filter_offset = MODE_4_SEC_18_CR_OFFSET_112;
    tmp_uintptr = sec18_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec18_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec18_cfg_buf, p_flow_tbl_list->sec18.p_CR_buf_112, CR_SIZE_112);
    p_step_cr->sec18_crs[1].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec18 cr[1] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_step_cr->sec18_crs[1].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->sec18.p_CR_buf_112), DC_U, DC_U);

    filter_offset = MODE_4_SEC_18_CR_OFFSET_113;
    tmp_uintptr = sec18_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec18_cfg_buf, &tmp_uintptr, sizeof(void*));
    //(void)amba_ik_system_memcpy(p_sec18_cfg_buf, p_flow_tbl_list->sec18.p_CR_buf_113, CR_SIZE_113);
    //p_step_cr->sec18_crs[2].config_region_addr = (tmp_uintptr - data_offset_base) >> 7U;
    amba_ik_system_print_uint32_5("Sec18 cr[2] addr 0x%08x, offset %d, src_addr 0x%08x ",
                                  p_step_cr->sec18_crs[2].config_region_addr << 7, filter_offset, DC_U, DC_U, DC_U);

    filter_offset = MODE_4_SEC_18_CR_OFFSET_114;
    tmp_uintptr = sec18_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec18_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec18_cfg_buf, p_flow_tbl_list->sec18.p_CR_buf_114, CR_SIZE_114);
    p_step_cr->sec18_crs[3].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec18 cr[3] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_step_cr->sec18_crs[3].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->sec18.p_CR_buf_114), DC_U, DC_U);

    filter_offset = MODE_4_SEC_18_CR_OFFSET_115;
    tmp_uintptr = sec18_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec18_cfg_buf, &tmp_uintptr, sizeof(void*));
    //(void)amba_ik_system_memcpy(p_sec18_cfg_buf, p_flow_tbl_list->sec18.p_CR_buf_115, CR_SIZE_115);
    //p_step_cr->sec18_crs[4].config_region_addr = (tmp_uintptr - data_offset_base) >> 7U;
    amba_ik_system_print_uint32_5("Sec18 cr[4] addr 0x%08x, offset %d, src_addr 0x%08x ",
                                  p_step_cr->sec18_crs[4].config_region_addr << 7, filter_offset, DC_U, DC_U, DC_U);

    filter_offset = MODE_4_SEC_18_CR_OFFSET_117;
    tmp_uintptr = sec18_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec18_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec18_cfg_buf, p_flow_tbl_list->sec18.p_CR_buf_117, CR_SIZE_117);
    p_step_cr->sec18_crs[6].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec18 cr[6] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_step_cr->sec18_crs[6].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->sec18.p_CR_buf_117), DC_U, DC_U);

    filter_offset = MODE_4_SEC_18_CR_OFFSET_118;
    tmp_uintptr = sec18_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec18_cfg_buf, &tmp_uintptr, sizeof(void*));
    //(void)amba_ik_system_memcpy(p_sec18_cfg_buf, p_flow_tbl_list->sec18.p_CR_buf_118, CR_SIZE_118);
    //p_step_cr->sec18_crs[7].config_region_addr = (tmp_uintptr - data_offset_base) >> 7U;
    amba_ik_system_print_uint32_5("Sec18 cr[7] addr 0x%08x, offset %d, src_addr 0x%08x ",
                                  p_step_cr->sec18_crs[7].config_region_addr << 7, filter_offset, DC_U, DC_U, DC_U);

    filter_offset = MODE_4_SEC_18_CR_OFFSET_119;
    tmp_uintptr = sec18_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec18_cfg_buf, &tmp_uintptr, sizeof(void*));
    //(void)amba_ik_system_memcpy(p_sec18_cfg_buf, p_flow_tbl_list->sec18.p_CR_buf_119, CR_SIZE_119);
    //p_step_cr->sec18_crs[8].config_region_addr = (tmp_uintptr - data_offset_base) >> 7U;
    amba_ik_system_print_uint32_5("Sec18 cr[8] addr 0x%08x, offset %d, src_addr 0x%08x ",
                                  p_step_cr->sec18_crs[8].config_region_addr << 7, filter_offset, DC_U, DC_U, DC_U);
}

static void img_cfg_hiso_step11_amalgam_data_update(uintptr data_offset_base,
        idsp_step_crs_t *p_step_cr,
        amalgam_hiso_step11_flow_data_t *p_cr,
        const amba_ik_hiso_step11_cr_list_t *p_flow_tbl_list)
{
    uint32_t filter_offset;
    const uint8_t *tmp_p_u8;
    uintptr sec18_cfg_buf_offset;
    void* p_sec18_cfg_buf;
    uintptr tmp_uintptr;

    // IKC does not use IDSP_CONFIG_HDR_SIZE, however, IKS does, in order to keep it on the same page between IKC and IKS,
    tmp_p_u8 = p_cr->sec18_cfg_buf;
    (void)amba_ik_system_memcpy(&sec18_cfg_buf_offset, &tmp_p_u8, sizeof(tmp_uintptr));

    img_cfg_hiso_r2y_sec2_amalgam_data_update(data_offset_base, p_step_cr, p_cr->sec2_cfg_buf, &p_flow_tbl_list->sec2);
    img_cfg_hiso_r2y_sec4_amalgam_data_update(data_offset_base, p_step_cr, p_cr->sec4_cfg_buf, &p_flow_tbl_list->sec4);

    filter_offset = MODE_4_SEC_18_CR_OFFSET_112;
    tmp_uintptr = sec18_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec18_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec18_cfg_buf, p_flow_tbl_list->sec18.p_CR_buf_112, CR_SIZE_112);
    p_step_cr->sec18_crs[1].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec18 cr[1] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_step_cr->sec18_crs[1].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->sec18.p_CR_buf_112), DC_U, DC_U);

    filter_offset = MODE_4_SEC_18_CR_OFFSET_113;
    tmp_uintptr = sec18_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec18_cfg_buf, &tmp_uintptr, sizeof(void*));
    //(void)amba_ik_system_memcpy(p_sec18_cfg_buf, p_flow_tbl_list->sec18.p_CR_buf_113, CR_SIZE_113);
    //p_step_cr->sec18_crs[2].config_region_addr = (tmp_uintptr - data_offset_base) >> 7U;
    amba_ik_system_print_uint32_5("Sec18 cr[2] addr 0x%08x, offset %d, src_addr 0x%08x ",
                                  p_step_cr->sec18_crs[2].config_region_addr << 7, filter_offset, DC_U, DC_U, DC_U);

    filter_offset = MODE_4_SEC_18_CR_OFFSET_114;
    tmp_uintptr = sec18_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec18_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec18_cfg_buf, p_flow_tbl_list->sec18.p_CR_buf_114, CR_SIZE_114);
    p_step_cr->sec18_crs[3].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint32_5("Sec18 cr[3] addr 0x%08x, offset %d, src_addr 0x%08x ",
                                  p_step_cr->sec18_crs[3].config_region_addr << 7, filter_offset, DC_U, DC_U, DC_U);

    filter_offset = MODE_4_SEC_18_CR_OFFSET_115;
    tmp_uintptr = sec18_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec18_cfg_buf, &tmp_uintptr, sizeof(void*));
    //(void)amba_ik_system_memcpy(p_sec18_cfg_buf, p_flow_tbl_list->sec18.p_CR_buf_115, CR_SIZE_115);
    //p_step_cr->sec18_crs[4].config_region_addr = (tmp_uintptr - data_offset_base) >> 7U;
    amba_ik_system_print_uint32_5("Sec18 cr[4] addr 0x%08x, offset %d, src_addr 0x%08x ",
                                  p_step_cr->sec18_crs[4].config_region_addr << 7, filter_offset, DC_U, DC_U, DC_U);

    filter_offset = MODE_4_SEC_18_CR_OFFSET_117;
    tmp_uintptr = sec18_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec18_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec18_cfg_buf, p_flow_tbl_list->sec18.p_CR_buf_117, CR_SIZE_117);
    p_step_cr->sec18_crs[6].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec18 cr[6] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_step_cr->sec18_crs[6].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->sec18.p_CR_buf_117), DC_U, DC_U);

    filter_offset = MODE_4_SEC_18_CR_OFFSET_118;
    tmp_uintptr = sec18_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec18_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec18_cfg_buf, p_flow_tbl_list->sec18.p_CR_buf_118, CR_SIZE_118);
    p_step_cr->sec18_crs[7].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec18 cr[7] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_step_cr->sec18_crs[7].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->sec18.p_CR_buf_118), DC_U, DC_U);

    filter_offset = MODE_4_SEC_18_CR_OFFSET_119;
    tmp_uintptr = sec18_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec18_cfg_buf, &tmp_uintptr, sizeof(void*));
    //(void)amba_ik_system_memcpy(p_sec18_cfg_buf, p_flow_tbl_list->sec18.p_CR_buf_119, CR_SIZE_119);
    //p_step_cr->sec18_crs[8].config_region_addr = (tmp_uintptr - data_offset_base) >> 7U;
    amba_ik_system_print_uint32_5("Sec18 cr[8] addr 0x%08x, offset %d, src_addr 0x%08x ",
                                  p_step_cr->sec18_crs[8].config_region_addr << 7, filter_offset, DC_U, DC_U, DC_U);
}

static void img_cfg_hiso_step13_amalgam_data_update(uintptr data_offset_base,
        idsp_step_crs_t *p_step_cr,
        amalgam_hiso_step13_flow_data_t *p_cr,
        const amba_ik_hiso_step13_cr_list_t *p_flow_tbl_list)
{
    uint32_t filter_offset;
    const uint8_t *tmp_p_u8;
    uintptr sec2_cfg_buf_offset;
    uintptr sec18_cfg_buf_offset;
    void* p_sec2_cfg_buf;
    void* p_sec18_cfg_buf;
#ifdef _HISO_SEC3_IN_STEP13
    uintptr sec3_cfg_buf_offset;
    void* p_sec3_cfg_buf;
#endif
    uintptr tmp_uintptr;

    // IKC does not use IDSP_CONFIG_HDR_SIZE, however, IKS does, in order to keep it on the same page between IKC and IKS,
    tmp_p_u8 = p_cr->sec2_cfg_buf;
    (void)amba_ik_system_memcpy(&sec2_cfg_buf_offset, &tmp_p_u8, sizeof(tmp_uintptr));
#ifdef _HISO_SEC3_IN_STEP13
    tmp_p_u8 = p_cr->sec3_cfg_buf;
    (void)amba_ik_system_memcpy(&sec3_cfg_buf_offset, &tmp_p_u8, sizeof(tmp_uintptr));
#endif
    tmp_p_u8 = p_cr->sec18_cfg_buf;
    (void)amba_ik_system_memcpy(&sec18_cfg_buf_offset, &tmp_p_u8, sizeof(tmp_uintptr));

    img_cfg_hiso_y2y_sec2_amalgam_data_update(data_offset_base, p_step_cr, p_cr->sec2_cfg_buf, &p_flow_tbl_list->sec2);

    filter_offset = MODE_8_SEC_2_CR_OFFSET_27;
    tmp_uintptr = sec2_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec2_cfg_buf, &tmp_uintptr, sizeof(void*));
    //(void)amba_ik_system_memcpy(p_sec2_cfg_buf, p_flow_tbl_list->sec2.p_CR_buf_27, CR_SIZE_27);
    p_step_cr->sec2_crs[23].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint32_5("Sec2 cr[23] addr 0x%08x, offset %d, src_addr 0x%08x ",
                                  p_step_cr->sec2_crs[23].config_region_addr << 7, filter_offset, DC_U, DC_U, DC_U);

#ifdef _HISO_SEC3_IN_STEP13
    filter_offset = CR_OFFSET_42;
    tmp_uintptr = sec3_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec3_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec3_cfg_buf, p_flow_tbl_list->sec3.p_CR_buf_42, CR_SIZE_42);
    p_step_cr->sec3_crs[0].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec3 cr[0] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_step_cr->sec3_crs[0].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->sec3.p_CR_buf_42), DC_U, DC_U);

    filter_offset = CR_OFFSET_43;
    tmp_uintptr = sec3_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec3_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec3_cfg_buf, p_flow_tbl_list->sec3.p_CR_buf_43, CR_SIZE_43);
    p_step_cr->sec3_crs[1].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec3 cr[1] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_step_cr->sec3_crs[1].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->sec3.p_CR_buf_43), DC_U, DC_U);

    filter_offset = CR_OFFSET_44;
    tmp_uintptr = sec3_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec3_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec3_cfg_buf, p_flow_tbl_list->sec3.p_CR_buf_44, CR_SIZE_44);
    p_step_cr->sec3_crs[2].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec3 cr[2] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_step_cr->sec3_crs[2].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->sec3.p_CR_buf_44), DC_U, DC_U);
#endif

    filter_offset = MODE_4_SEC_18_CR_OFFSET_112;
    tmp_uintptr = sec18_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec18_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec18_cfg_buf, p_flow_tbl_list->sec18.p_CR_buf_112, CR_SIZE_112);
    p_step_cr->sec18_crs[1].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec18 cr[1] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_step_cr->sec18_crs[1].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->sec18.p_CR_buf_112), DC_U, DC_U);

    filter_offset = MODE_4_SEC_18_CR_OFFSET_113;
    tmp_uintptr = sec18_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec18_cfg_buf, &tmp_uintptr, sizeof(void*));
    //(void)amba_ik_system_memcpy(p_sec18_cfg_buf, p_flow_tbl_list->sec18.p_CR_buf_113, CR_SIZE_113);
    //p_step_cr->sec18_crs[2].config_region_addr = (tmp_uintptr - data_offset_base) >> 7U;
    amba_ik_system_print_uint32_5("Sec18 cr[2] addr 0x%08x, offset %d, src_addr 0x%08x ",
                                  p_step_cr->sec18_crs[2].config_region_addr << 7, filter_offset, DC_U, DC_U, DC_U);

    filter_offset = MODE_4_SEC_18_CR_OFFSET_114;
    tmp_uintptr = sec18_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec18_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec18_cfg_buf, p_flow_tbl_list->sec18.p_CR_buf_114, CR_SIZE_114);
    p_step_cr->sec18_crs[3].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec18 cr[3] addr 0x%08x, offset %d, src_addr 0x%08x ",
                                  (uint64)p_step_cr->sec18_crs[3].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->sec18.p_CR_buf_114), DC_U, DC_U);

    filter_offset = MODE_4_SEC_18_CR_OFFSET_115;
    tmp_uintptr = sec18_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec18_cfg_buf, &tmp_uintptr, sizeof(void*));
    //(void)amba_ik_system_memcpy(p_sec18_cfg_buf, p_flow_tbl_list->sec18.p_CR_buf_115, CR_SIZE_115);
    //p_step_cr->sec18_crs[4].config_region_addr = (tmp_uintptr - data_offset_base) >> 7U;
    amba_ik_system_print_uint32_5("Sec18 cr[4] addr 0x%08x, offset %d, src_addr 0x%08x ",
                                  p_step_cr->sec18_crs[4].config_region_addr << 7, filter_offset, DC_U, DC_U, DC_U);

    filter_offset = MODE_4_SEC_18_CR_OFFSET_117;
    tmp_uintptr = sec18_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec18_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec18_cfg_buf, p_flow_tbl_list->sec18.p_CR_buf_117, CR_SIZE_117);
    p_step_cr->sec18_crs[6].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec18 cr[6] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_step_cr->sec18_crs[6].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->sec18.p_CR_buf_117), DC_U, DC_U);

    filter_offset = MODE_4_SEC_18_CR_OFFSET_118;
    tmp_uintptr = sec18_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec18_cfg_buf, &tmp_uintptr, sizeof(void*));
    //(void)amba_ik_system_memcpy(p_sec18_cfg_buf, p_flow_tbl_list->sec18.p_CR_buf_118, CR_SIZE_118);
    //p_step_cr->sec18_crs[7].config_region_addr = (tmp_uintptr - data_offset_base) >> 7U;
    amba_ik_system_print_uint32_5("Sec18 cr[7] addr 0x%08x, offset %d, src_addr 0x%08x ",
                                  p_step_cr->sec18_crs[7].config_region_addr << 7, filter_offset, DC_U, DC_U, DC_U);

    filter_offset = MODE_4_SEC_18_CR_OFFSET_119;
    tmp_uintptr = sec18_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec18_cfg_buf, &tmp_uintptr, sizeof(void*));
    //(void)amba_ik_system_memcpy(p_sec18_cfg_buf, p_flow_tbl_list->sec18.p_CR_buf_119, CR_SIZE_119);
    //p_step_cr->sec18_crs[8].config_region_addr = (tmp_uintptr - data_offset_base) >> 7U;
    amba_ik_system_print_uint32_5("Sec18 cr[8] addr 0x%08x, offset %d, src_addr 0x%08x ",
                                  p_step_cr->sec18_crs[8].config_region_addr << 7, filter_offset, DC_U, DC_U, DC_U);
}

static void img_cfg_hiso_step14_amalgam_data_update(uintptr data_offset_base,
        idsp_step_crs_t *p_step_cr,
        const amalgam_hiso_step14_flow_data_t *p_cr,
        const amba_ik_hiso_step14_cr_list_t *p_flow_tbl_list)
{
    uint32_t filter_offset;
    const uint8_t *tmp_p_u8;
    uintptr sec18_cfg_buf_offset;
    void* p_sec18_cfg_buf;
    uintptr sec3_cfg_buf_offset;
    void* p_sec3_cfg_buf;
    uintptr tmp_uintptr;

    // IKC does not use IDSP_CONFIG_HDR_SIZE, however, IKS does, in order to keep it on the same page between IKC and IKS,
    tmp_p_u8 = p_cr->sec3_cfg_buf;
    (void)amba_ik_system_memcpy(&sec3_cfg_buf_offset, &tmp_p_u8, sizeof(tmp_uintptr));
    tmp_p_u8 = p_cr->sec18_cfg_buf;
    (void)amba_ik_system_memcpy(&sec18_cfg_buf_offset, &tmp_p_u8, sizeof(tmp_uintptr));


    filter_offset = CR_OFFSET_42;
    tmp_uintptr = sec3_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec3_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec3_cfg_buf, p_flow_tbl_list->sec3.p_CR_buf_42, CR_SIZE_42);
    p_step_cr->sec3_crs[0].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec3 cr[0] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_step_cr->sec3_crs[0].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->sec3.p_CR_buf_42), DC_U, DC_U);

    filter_offset = CR_OFFSET_43;
    tmp_uintptr = sec3_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec3_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec3_cfg_buf, p_flow_tbl_list->sec3.p_CR_buf_43, CR_SIZE_43);
    p_step_cr->sec3_crs[1].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec3 cr[1] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_step_cr->sec3_crs[1].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->sec3.p_CR_buf_43), DC_U, DC_U);

    filter_offset = CR_OFFSET_44;
    tmp_uintptr = sec3_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec3_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec3_cfg_buf, p_flow_tbl_list->sec3.p_CR_buf_44, CR_SIZE_44);
    p_step_cr->sec3_crs[2].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec3 cr[2] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_step_cr->sec3_crs[2].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->sec3.p_CR_buf_44), DC_U, DC_U);


    filter_offset = MODE_4_SEC_18_CR_OFFSET_112;
    tmp_uintptr = sec18_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec18_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec18_cfg_buf, p_flow_tbl_list->sec18.p_CR_buf_112, CR_SIZE_112);
    p_step_cr->sec18_crs[1].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec18 cr[1] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_step_cr->sec18_crs[1].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->sec18.p_CR_buf_112), DC_U, DC_U);

    filter_offset = MODE_4_SEC_18_CR_OFFSET_113;
    tmp_uintptr = sec18_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec18_cfg_buf, &tmp_uintptr, sizeof(void*));
    //(void)amba_ik_system_memcpy(p_sec18_cfg_buf, p_flow_tbl_list->sec18.p_CR_buf_113, CR_SIZE_113);
    //p_step_cr->sec18_crs[2].config_region_addr = (tmp_uintptr - data_offset_base) >> 7U;
    amba_ik_system_print_uint32_5("Sec18 cr[2] addr 0x%08x, offset %d, src_addr 0x%08x ",
                                  p_step_cr->sec18_crs[2].config_region_addr << 7, filter_offset, DC_U, DC_U, DC_U);

    filter_offset = MODE_4_SEC_18_CR_OFFSET_114;
    tmp_uintptr = sec18_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec18_cfg_buf, &tmp_uintptr, sizeof(void*));
    //(void)amba_ik_system_memcpy(p_sec18_cfg_buf, p_flow_tbl_list->sec18.p_CR_buf_114, CR_SIZE_114);
    //p_step_cr->sec18_crs[3].config_region_addr = (tmp_uintptr - data_offset_base) >> 7U;
    amba_ik_system_print_uint32_5("Sec18 cr[3] addr 0x%08x, offset %d, src_addr 0x%08x ",
                                  p_step_cr->sec18_crs[3].config_region_addr << 7, filter_offset, DC_U, DC_U, DC_U);

    filter_offset = MODE_4_SEC_18_CR_OFFSET_115;
    tmp_uintptr = sec18_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec18_cfg_buf, &tmp_uintptr, sizeof(void*));
    //(void)amba_ik_system_memcpy(p_sec18_cfg_buf, p_flow_tbl_list->sec18.p_CR_buf_115, CR_SIZE_115);
    //p_step_cr->sec18_crs[4].config_region_addr = (tmp_uintptr - data_offset_base) >> 7U;
    amba_ik_system_print_uint32_5("Sec18 cr[4] addr 0x%08x, offset %d, src_addr 0x%08x ",
                                  p_step_cr->sec18_crs[4].config_region_addr << 7, filter_offset, DC_U, DC_U, DC_U);

    filter_offset = MODE_4_SEC_18_CR_OFFSET_117;
    tmp_uintptr = sec18_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec18_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec18_cfg_buf, p_flow_tbl_list->sec18.p_CR_buf_117, CR_SIZE_117);
    p_step_cr->sec18_crs[6].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec18 cr[6] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_step_cr->sec18_crs[6].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->sec18.p_CR_buf_117), DC_U, DC_U);

    filter_offset = MODE_4_SEC_18_CR_OFFSET_118;
    tmp_uintptr = sec18_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec18_cfg_buf, &tmp_uintptr, sizeof(void*));
    //(void)amba_ik_system_memcpy(p_sec18_cfg_buf, p_flow_tbl_list->sec18.p_CR_buf_118, CR_SIZE_118);
    //p_step_cr->sec18_crs[7].config_region_addr = (tmp_uintptr - data_offset_base) >> 7U;
    amba_ik_system_print_uint32_5("Sec18 cr[7] addr 0x%08x, offset %d, src_addr 0x%08x ",
                                  p_step_cr->sec18_crs[7].config_region_addr << 7, filter_offset, DC_U, DC_U, DC_U);

    filter_offset = MODE_4_SEC_18_CR_OFFSET_119;
    tmp_uintptr = sec18_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec18_cfg_buf, &tmp_uintptr, sizeof(void*));
    //(void)amba_ik_system_memcpy(p_sec18_cfg_buf, p_flow_tbl_list->sec18.p_CR_buf_119, CR_SIZE_119);
    //p_step_cr->sec18_crs[8].config_region_addr = (tmp_uintptr - data_offset_base) >> 7U;
    amba_ik_system_print_uint32_5("Sec18 cr[8] addr 0x%08x, offset %d, src_addr 0x%08x ",
                                  p_step_cr->sec18_crs[8].config_region_addr << 7, filter_offset, DC_U, DC_U, DC_U);
}

uint32 img_cfg_hiso_amalgam_data_update(const idsp_flow_ctrl_t *p_flow, const amba_ik_hiso_flow_tables_list_t *p_flow_tbl_list, const void *p_user_buffer)
{
    uint32 rval = 0;
    amalgam_hiso_flow_data_t *p_amalgam_data;
    uintptr data_offset_base;
    const void *temp_data_addr = NULL;
    const uint8_t *tmp_p_u8;
    const int16_t *tmp_p_s16;
    uintptr tmp_uintptr;


    (void)amba_ik_system_memcpy(&p_amalgam_data, &p_user_buffer, sizeof(amalgam_hiso_flow_data_t *));
    (void)amba_ik_system_memcpy(&data_offset_base, &p_amalgam_data, sizeof(uintptr));
    (void)amba_ik_system_memset(p_amalgam_data, 0x0, sizeof(amalgam_hiso_flow_data_t));
    (void)amba_ik_system_memcpy(&p_amalgam_data->flow_ctrl, p_flow, sizeof(idsp_flow_ctrl_t));

    if (p_flow->flow_info.sbp_map_addr != 0U) {
        temp_data_addr = img_cfg_uint32_physical_to_void_virtual(p_flow->flow_info.sbp_map_addr);
        (void)amba_ik_system_memcpy(p_amalgam_data->sbp_map, temp_data_addr, IK_AMALGAM_TABLE_SBP_SIZE);
        tmp_p_u8 = p_amalgam_data->sbp_map;
        (void)amba_ik_system_memcpy(&tmp_uintptr, &tmp_p_u8, sizeof(tmp_uintptr));
        p_amalgam_data->flow_ctrl.flow_info.sbp_map_addr = (uint32_t)(tmp_uintptr - data_offset_base);
        amba_ik_system_print_uint64_5("sbp_map_addr 0x%08x, src_addr %p ",
                                      p_amalgam_data->flow_ctrl.flow_info.sbp_map_addr, ptr2uintptr(temp_data_addr), DC_U, DC_U, DC_U);
    } else {
        amba_ik_system_print_str_5("[IK] SBP is NULL", NULL, NULL, NULL, NULL, NULL);
    }

    if (p_flow_tbl_list->p_ca_warp_hor_red != NULL) {
        (void)amba_ik_system_memcpy(p_amalgam_data->ca_warp_hor_red, p_flow_tbl_list->p_ca_warp_hor_red, sizeof(p_amalgam_data->ca_warp_hor_red));
        tmp_p_s16 = p_amalgam_data->ca_warp_hor_red;
        (void)amba_ik_system_memcpy(&tmp_uintptr, &tmp_p_s16, sizeof(tmp_uintptr));
        p_amalgam_data->flow_ctrl.calib.cawarp_horizontal_table_addr_red = (uint32_t)(tmp_uintptr - data_offset_base);
        amba_ik_system_print_uint64_5("cawarp_horizontal_table_addr_red 0x%08x, src_addr %p ",
                                      p_amalgam_data->flow_ctrl.calib.cawarp_horizontal_table_addr_red, ptr2uintptr(p_flow_tbl_list->p_ca_warp_hor_red), DC_U, DC_U, DC_U);
    } else {
        amba_ik_system_print_str_5("[IK] cawarp_hor_red is NULL", NULL, NULL, NULL, NULL, NULL);
    }

    if (p_flow_tbl_list->p_ca_warp_hor_blue != NULL) {
        (void)amba_ik_system_memcpy(p_amalgam_data->ca_warp_hor_blue, p_flow_tbl_list->p_ca_warp_hor_blue, sizeof(p_amalgam_data->ca_warp_hor_blue));
        tmp_p_s16 = p_amalgam_data->ca_warp_hor_blue;
        (void)amba_ik_system_memcpy(&tmp_uintptr, &tmp_p_s16, sizeof(tmp_uintptr));
        p_amalgam_data->flow_ctrl.calib.cawarp_horizontal_table_addr_blue = (uint32_t)(tmp_uintptr - data_offset_base);
        amba_ik_system_print_uint64_5("cawarp_horizontal_table_addr_blue 0x%08x, src_addr %p ",
                                      p_amalgam_data->flow_ctrl.calib.cawarp_horizontal_table_addr_blue, ptr2uintptr(p_flow_tbl_list->p_ca_warp_hor_blue), DC_U, DC_U, DC_U);
    } else {
        amba_ik_system_print_str_5("[IK] cawarp_hor_blue is NULL", NULL, NULL, NULL, NULL, NULL);
    }

    if (p_flow_tbl_list->p_ca_warp_ver_red != NULL) {
        (void)amba_ik_system_memcpy(p_amalgam_data->ca_warp_ver_red, p_flow_tbl_list->p_ca_warp_ver_red, sizeof(p_amalgam_data->ca_warp_ver_red));
        tmp_p_s16 = p_amalgam_data->ca_warp_ver_red;
        (void)amba_ik_system_memcpy(&tmp_uintptr, &tmp_p_s16, sizeof(tmp_uintptr));
        p_amalgam_data->flow_ctrl.calib.cawarp_vertical_table_addr_red= (uint32_t)(tmp_uintptr - data_offset_base);
        amba_ik_system_print_uint64_5("cawarp_vertical_table_addr_red 0x%08x, src_addr %p ",
                                      p_amalgam_data->flow_ctrl.calib.cawarp_vertical_table_addr_red, ptr2uintptr(p_flow_tbl_list->p_ca_warp_ver_red), DC_U, DC_U, DC_U);
    } else {
        amba_ik_system_print_str_5("[IK] cawarp_ver_red is NULL", NULL, NULL, NULL, NULL, NULL);
    }

    if (p_flow_tbl_list->p_ca_warp_ver_blue != NULL) {
        (void)amba_ik_system_memcpy(p_amalgam_data->ca_warp_ver_blue, p_flow_tbl_list->p_ca_warp_ver_blue, sizeof(p_amalgam_data->ca_warp_ver_blue));
        tmp_p_s16 = p_amalgam_data->ca_warp_ver_blue;
        (void)amba_ik_system_memcpy(&tmp_uintptr, &tmp_p_s16, sizeof(tmp_uintptr));
        p_amalgam_data->flow_ctrl.calib.cawarp_vertical_table_addr_blue = (uint32_t)(tmp_uintptr - data_offset_base);
        amba_ik_system_print_uint64_5("cawarp_vertical_table_addr_blue 0x%08x, src_addr %p ",
                                      p_amalgam_data->flow_ctrl.calib.cawarp_vertical_table_addr_blue, ptr2uintptr(p_flow_tbl_list->p_ca_warp_ver_blue), DC_U, DC_U, DC_U);
    } else {
        amba_ik_system_print_str_5("[IK] cawarp_ver_blue is NULL", NULL, NULL, NULL, NULL, NULL);
    }

    if (p_flow_tbl_list->p_warp_hor != NULL) {
        (void)amba_ik_system_memcpy(p_amalgam_data->warp_hor, p_flow_tbl_list->p_warp_hor, sizeof(p_amalgam_data->warp_hor));
        tmp_p_s16 = p_amalgam_data->warp_hor;
        (void)amba_ik_system_memcpy(&tmp_uintptr, &tmp_p_s16, sizeof(tmp_uintptr));
        p_amalgam_data->flow_ctrl.calib.warp_horizontal_table_address = (uint32_t)(tmp_uintptr - data_offset_base);
        amba_ik_system_print_uint64_5("warp_horizontal_table_addr 0x%08x, src_addr %p ",
                                      p_amalgam_data->flow_ctrl.calib.warp_horizontal_table_address, ptr2uintptr(p_flow_tbl_list->p_warp_hor), DC_U, DC_U, DC_U);
    } else {
        amba_ik_system_print_str_5("[IK] warp_hor is NULL", NULL, NULL, NULL, NULL, NULL);
    }

    if (p_flow_tbl_list->p_warp_ver != NULL) {
        (void)amba_ik_system_memcpy(p_amalgam_data->warp_ver, p_flow_tbl_list->p_warp_ver, sizeof(p_amalgam_data->warp_ver));
        tmp_p_s16 = p_amalgam_data->warp_ver;
        (void)amba_ik_system_memcpy(&tmp_uintptr, &tmp_p_s16, sizeof(tmp_uintptr));
        p_amalgam_data->flow_ctrl.calib.warp_vertical_table_address = (uint32_t)(tmp_uintptr - data_offset_base);
        amba_ik_system_print_uint64_5("warp_vertical_table_addr 0x%08x, src_addr 0x%08x ",
                                      p_amalgam_data->flow_ctrl.calib.warp_vertical_table_address, ptr2uintptr(p_flow_tbl_list->p_warp_ver), DC_U, DC_U, DC_U);
    } else {
        amba_ik_system_print_str_5("[IK] warp_ver is NULL", NULL, NULL, NULL, NULL, NULL);
    }

    if (p_flow_tbl_list->p_aaa != NULL) {
        const idsp_aaa_data_t *p_tmp_aaa;
        (void)amba_ik_system_memcpy(p_amalgam_data->aaa, p_flow_tbl_list->p_aaa, sizeof(p_amalgam_data->aaa));
        p_tmp_aaa = p_amalgam_data->aaa[0];
        (void)amba_ik_system_memcpy(&tmp_uintptr, &p_tmp_aaa, sizeof(tmp_uintptr));
        p_amalgam_data->flow_ctrl.stitch.aaa_info_daddr = (uint32_t)(tmp_uintptr - data_offset_base);
        amba_ik_system_print_uint64_5("aaa_info_daddr 0x%08x, src_addr %p ",
                                      p_amalgam_data->flow_ctrl.stitch.aaa_info_daddr, ptr2uintptr(p_flow_tbl_list->p_aaa), DC_U, DC_U, DC_U);
    } else {
        amba_ik_system_print_str_5("[IK] AAA is NULL", NULL, NULL, NULL, NULL, NULL);
    }

    amba_ik_system_print_str_5("Step 1 :", DC_S, DC_S, DC_S, DC_S, DC_S);
    img_cfg_hiso_step1_amalgam_data_update(data_offset_base, &p_amalgam_data->flow_ctrl.step_crs[0], &p_amalgam_data->step1_cfg_buf, &p_flow_tbl_list->step1);
    amba_ik_system_print_str_5("Step 2 :", DC_S, DC_S, DC_S, DC_S, DC_S);
    img_cfg_hiso_step2_amalgam_data_update(data_offset_base, &p_amalgam_data->flow_ctrl.step_crs[1], &p_amalgam_data->step2_cfg_buf, &p_flow_tbl_list->step2);
    amba_ik_system_print_str_5("Step 3 :", DC_S, DC_S, DC_S, DC_S, DC_S);
    img_cfg_hiso_step3_amalgam_data_update(data_offset_base, &p_amalgam_data->flow_ctrl.step_crs[2], &p_amalgam_data->step3_cfg_buf, &p_flow_tbl_list->step3);
    amba_ik_system_print_str_5("Step 4 :", DC_S, DC_S, DC_S, DC_S, DC_S);
    img_cfg_hiso_step3_amalgam_data_update(data_offset_base, &p_amalgam_data->flow_ctrl.step_crs[3], &p_amalgam_data->step4_cfg_buf, &p_flow_tbl_list->step4);
    amba_ik_system_print_str_5("Step 4a :", DC_S, DC_S, DC_S, DC_S, DC_S);
    img_cfg_hiso_step4a_amalgam_data_update(data_offset_base, &p_amalgam_data->flow_ctrl.step_crs[4], &p_amalgam_data->step4a_cfg_buf, &p_flow_tbl_list->step4a);
    amba_ik_system_print_str_5("Step 5 :", DC_S, DC_S, DC_S, DC_S, DC_S);
    img_cfg_hiso_step5_amalgam_data_update(data_offset_base, &p_amalgam_data->flow_ctrl.step_crs[5], &p_amalgam_data->step5_cfg_buf, &p_flow_tbl_list->step5);
    amba_ik_system_print_str_5("Step 6 :", DC_S, DC_S, DC_S, DC_S, DC_S);
    img_cfg_hiso_step6_amalgam_data_update(data_offset_base, &p_amalgam_data->flow_ctrl.step_crs[6], &p_amalgam_data->step6_cfg_buf, &p_flow_tbl_list->step6);
    amba_ik_system_print_str_5("Step 7 :", DC_S, DC_S, DC_S, DC_S, DC_S);
    img_cfg_hiso_step6_amalgam_data_update(data_offset_base, &p_amalgam_data->flow_ctrl.step_crs[7], &p_amalgam_data->step7_cfg_buf, &p_flow_tbl_list->step7);
    amba_ik_system_print_str_5("Step 8 :", DC_S, DC_S, DC_S, DC_S, DC_S);
    img_cfg_hiso_step6_amalgam_data_update(data_offset_base, &p_amalgam_data->flow_ctrl.step_crs[8], &p_amalgam_data->step8_cfg_buf, &p_flow_tbl_list->step8);
    amba_ik_system_print_str_5("Step 9 :", DC_S, DC_S, DC_S, DC_S, DC_S);
    img_cfg_hiso_step4a_amalgam_data_update(data_offset_base, &p_amalgam_data->flow_ctrl.step_crs[9], &p_amalgam_data->step9_cfg_buf, &p_flow_tbl_list->step9);
    amba_ik_system_print_str_5("Step 10 :", DC_S, DC_S, DC_S, DC_S, DC_S);
    img_cfg_hiso_step5_amalgam_data_update(data_offset_base, &p_amalgam_data->flow_ctrl.step_crs[10], &p_amalgam_data->step10_cfg_buf, &p_flow_tbl_list->step10);
    amba_ik_system_print_str_5("Step 11 :", DC_S, DC_S, DC_S, DC_S, DC_S);
    img_cfg_hiso_step11_amalgam_data_update(data_offset_base, &p_amalgam_data->flow_ctrl.step_crs[11], &p_amalgam_data->step11_cfg_buf, &p_flow_tbl_list->step11);
    amba_ik_system_print_str_5("Step 12 :", DC_S, DC_S, DC_S, DC_S, DC_S);
    img_cfg_hiso_step11_amalgam_data_update(data_offset_base, &p_amalgam_data->flow_ctrl.step_crs[12], &p_amalgam_data->step12_cfg_buf, &p_flow_tbl_list->step12);
    amba_ik_system_print_str_5("Step 13 :", DC_S, DC_S, DC_S, DC_S, DC_S);
    img_cfg_hiso_step13_amalgam_data_update(data_offset_base, &p_amalgam_data->flow_ctrl.step_crs[13], &p_amalgam_data->step13_cfg_buf, &p_flow_tbl_list->step13);
#ifndef _HISO_SEC3_IN_STEP13
    amba_ik_system_print_str_5("Step 14 :", DC_S, DC_S, DC_S, DC_S, DC_S);
    img_cfg_hiso_step14_amalgam_data_update(data_offset_base, &p_amalgam_data->flow_ctrl.step_crs[14], &p_amalgam_data->step14_cfg_buf, &p_flow_tbl_list->step14);
#endif

    //p_amalgam_data->flow_ctrl.header.idsp_flow_addr = data_offset_base;
    (void)amba_ik_system_memcpy(&p_amalgam_data->flow_ctrl.header.idsp_flow_addr, &data_offset_base, sizeof(uint32));

    return rval;
}


uint32 img_cfg_hiso_query_debug_flow_data_size(size_t *p_size)
{
    uint32_t rval = IK_OK;

    if(p_size != NULL) {
        *p_size = sizeof(amalgam_hiso_flow_data_t);
    } else {
        amba_ik_system_print_str_5("img_cfg_hiso_query_debug_flow_data_size size ptr is NULL", NULL, NULL, NULL, NULL, NULL);
        rval = IK_ERR_0005;
    }
    return rval;
}

static uint32 img_cfg_r2y_amalgam_data_update(uintptr data_offset_base,
        idsp_step_crs_t *p_step_cr,
        const amalgam_r2y_flow_data_t *p_amalgam_data,
        const amba_ik_r2y_flow_tables_list_t *p_flow_tbl_list)
{
    uint32 rval = 0;
    uint32_t filter_offset;
    const uint8_t *tmp_p_u8;
    uintptr sec2_cfg_buf_offset;
    uintptr sec3_cfg_buf_offset;
    uintptr sec4_cfg_buf_offset;
    uintptr sec11_cfg_buf_offset;
    uintptr sec18_cfg_buf_offset;
    void* p_sec2_cfg_buf;
    void* p_sec3_cfg_buf;
    void* p_sec4_cfg_buf;
    void* p_sec11_cfg_buf;
    void* p_sec18_cfg_buf;
    uintptr tmp_uintptr;

    // IKC does not use IDSP_CONFIG_HDR_SIZE, however, IKS does, in order to keep it on the same page between IKC and IKS,
    tmp_p_u8 = p_amalgam_data->sec2_cfg_buf;
    (void)amba_ik_system_memcpy(&sec2_cfg_buf_offset, &tmp_p_u8, sizeof(tmp_uintptr));
    tmp_p_u8 = p_amalgam_data->sec3_cfg_buf;
    (void)amba_ik_system_memcpy(&sec3_cfg_buf_offset, &tmp_p_u8, sizeof(tmp_uintptr));
    tmp_p_u8 = p_amalgam_data->sec4_cfg_buf;
    (void)amba_ik_system_memcpy(&sec4_cfg_buf_offset, &tmp_p_u8, sizeof(tmp_uintptr));
    tmp_p_u8 = p_amalgam_data->sec11_cfg_buf;
    (void)amba_ik_system_memcpy(&sec11_cfg_buf_offset, &tmp_p_u8, sizeof(tmp_uintptr));
    tmp_p_u8 = p_amalgam_data->sec18_cfg_buf;
    (void)amba_ik_system_memcpy(&sec18_cfg_buf_offset, &tmp_p_u8, sizeof(tmp_uintptr));

    filter_offset = CR_OFFSET_4;
    tmp_uintptr = sec2_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec2_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec2_cfg_buf, p_flow_tbl_list->p_CR_buf_4, CR_SIZE_4);
    p_step_cr->sec2_crs[0].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec2 cr[0] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_step_cr->sec2_crs[0].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_4), DC_U, DC_U);

    filter_offset = CR_OFFSET_5;
    tmp_uintptr = sec2_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec2_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec2_cfg_buf, p_flow_tbl_list->p_CR_buf_5, CR_SIZE_5);
    p_step_cr->sec2_crs[1].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec2 cr[1] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_step_cr->sec2_crs[1].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_5), DC_U, DC_U);

    filter_offset = CR_OFFSET_6;
    tmp_uintptr = sec2_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec2_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec2_cfg_buf, p_flow_tbl_list->p_CR_buf_6, CR_SIZE_6);
    p_step_cr->sec2_crs[2].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec2 cr[2] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_step_cr->sec2_crs[2].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_6), DC_U, DC_U);

    filter_offset = CR_OFFSET_7;
    tmp_uintptr = sec2_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec2_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec2_cfg_buf, p_flow_tbl_list->p_CR_buf_7, CR_SIZE_7);
    p_step_cr->sec2_crs[3].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec2 cr[3] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_step_cr->sec2_crs[3].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_7), DC_U, DC_U);

    filter_offset = CR_OFFSET_8;
    tmp_uintptr = sec2_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec2_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec2_cfg_buf, p_flow_tbl_list->p_CR_buf_8, CR_SIZE_8);
    p_step_cr->sec2_crs[4].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec2 cr[4] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_step_cr->sec2_crs[4].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_8), DC_U, DC_U);

    filter_offset = CR_OFFSET_9;
    tmp_uintptr = sec2_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec2_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec2_cfg_buf, p_flow_tbl_list->p_CR_buf_9, CR_SIZE_9);
    p_step_cr->sec2_crs[5].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec2 cr[5] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_step_cr->sec2_crs[5].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_9), DC_U, DC_U);

    filter_offset = CR_OFFSET_10;
    tmp_uintptr = sec2_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec2_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec2_cfg_buf, p_flow_tbl_list->p_CR_buf_10, CR_SIZE_10);
    p_step_cr->sec2_crs[6].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec2 cr[6] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_step_cr->sec2_crs[6].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_10), DC_U, DC_U);

    filter_offset = CR_OFFSET_11;
    tmp_uintptr = sec2_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec2_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec2_cfg_buf, p_flow_tbl_list->p_CR_buf_11, CR_SIZE_11);
    p_step_cr->sec2_crs[7].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec2 cr[7] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_step_cr->sec2_crs[7].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_11), DC_U, DC_U);

    filter_offset = CR_OFFSET_12;
    tmp_uintptr = sec2_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec2_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec2_cfg_buf, p_flow_tbl_list->p_CR_buf_12, CR_SIZE_12);
    p_step_cr->sec2_crs[8].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec2 cr[8] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_step_cr->sec2_crs[8].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_12), DC_U, DC_U);

    filter_offset = CR_OFFSET_13;
    tmp_uintptr = sec2_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec2_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec2_cfg_buf, p_flow_tbl_list->p_CR_buf_13, CR_SIZE_13);
    p_step_cr->sec2_crs[9].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec2 cr[9] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_step_cr->sec2_crs[9].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_13), DC_U, DC_U);

    filter_offset = CR_OFFSET_14;
    tmp_uintptr = sec2_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec2_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec2_cfg_buf, p_flow_tbl_list->p_CR_buf_14, CR_SIZE_14);
    p_step_cr->sec2_crs[10].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec2 cr[10] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_step_cr->sec2_crs[10].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_14), DC_U, DC_U);

    filter_offset = CR_OFFSET_15;
    tmp_uintptr = sec2_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec2_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec2_cfg_buf, p_flow_tbl_list->p_CR_buf_15, CR_SIZE_15);
    p_step_cr->sec2_crs[11].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec2 cr[11] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_step_cr->sec2_crs[11].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_15), DC_U, DC_U);

    filter_offset = CR_OFFSET_16;
    tmp_uintptr = sec2_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec2_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec2_cfg_buf, p_flow_tbl_list->p_CR_buf_16, CR_SIZE_16);
    p_step_cr->sec2_crs[12].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec2 cr[12] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_step_cr->sec2_crs[12].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_16), DC_U, DC_U);

    filter_offset = CR_OFFSET_17;
    tmp_uintptr = sec2_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec2_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec2_cfg_buf, p_flow_tbl_list->p_CR_buf_17, CR_SIZE_17);
    p_step_cr->sec2_crs[13].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec2 cr[13] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_step_cr->sec2_crs[13].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_17), DC_U, DC_U);

    filter_offset = CR_OFFSET_18;
    tmp_uintptr = sec2_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec2_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec2_cfg_buf, p_flow_tbl_list->p_CR_buf_18, CR_SIZE_18);
    p_step_cr->sec2_crs[14].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec2 cr[14] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_step_cr->sec2_crs[14].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_18), DC_U, DC_U);

    filter_offset = CR_OFFSET_19;
    tmp_uintptr = sec2_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec2_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec2_cfg_buf, p_flow_tbl_list->p_CR_buf_19, CR_SIZE_19);
    p_step_cr->sec2_crs[15].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec2 cr[15] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_step_cr->sec2_crs[15].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_19), DC_U, DC_U);

    filter_offset = CR_OFFSET_20;
    tmp_uintptr = sec2_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec2_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec2_cfg_buf, p_flow_tbl_list->p_CR_buf_20, CR_SIZE_20);
    p_step_cr->sec2_crs[16].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec2 cr[16] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_step_cr->sec2_crs[16].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_20), DC_U, DC_U);

    filter_offset = CR_OFFSET_21;
    tmp_uintptr = sec2_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec2_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec2_cfg_buf, p_flow_tbl_list->p_CR_buf_21, CR_SIZE_21);
    p_step_cr->sec2_crs[17].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec2 cr[17] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_step_cr->sec2_crs[17].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_21), DC_U, DC_U);

    filter_offset = CR_OFFSET_22;
    tmp_uintptr = sec2_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec2_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec2_cfg_buf, p_flow_tbl_list->p_CR_buf_22, CR_SIZE_22);
    p_step_cr->sec2_crs[18].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec2 cr[18] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_step_cr->sec2_crs[18].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_22), DC_U, DC_U);

    filter_offset = CR_OFFSET_23;
    tmp_uintptr = sec2_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec2_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec2_cfg_buf, p_flow_tbl_list->p_CR_buf_23, CR_SIZE_23);
    p_step_cr->sec2_crs[19].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec2 cr[19] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_step_cr->sec2_crs[19].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_23), DC_U, DC_U);

    filter_offset = CR_OFFSET_24;
    tmp_uintptr = sec2_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec2_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec2_cfg_buf, p_flow_tbl_list->p_CR_buf_24, CR_SIZE_24);
    p_step_cr->sec2_crs[20].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec2 cr[20] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_step_cr->sec2_crs[20].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_24), DC_U, DC_U);

    filter_offset = CR_OFFSET_25;
    tmp_uintptr = sec2_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec2_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec2_cfg_buf, p_flow_tbl_list->p_CR_buf_25, CR_SIZE_25);
    p_step_cr->sec2_crs[21].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec2 cr[21] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_step_cr->sec2_crs[21].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_25), DC_U, DC_U);

    filter_offset = CR_OFFSET_26;
    tmp_uintptr = sec2_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec2_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec2_cfg_buf, p_flow_tbl_list->p_CR_buf_26, CR_SIZE_26);
    p_step_cr->sec2_crs[22].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec2 cr[22] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_step_cr->sec2_crs[22].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_26), DC_U, DC_U);

    filter_offset = CR_OFFSET_27;
    tmp_uintptr = sec2_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec2_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec2_cfg_buf, p_flow_tbl_list->p_CR_buf_27, CR_SIZE_27);
    p_step_cr->sec2_crs[23].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec2 cr[23] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_step_cr->sec2_crs[23].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_27), DC_U, DC_U);

    filter_offset = CR_OFFSET_28;
    tmp_uintptr = sec2_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec2_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec2_cfg_buf, p_flow_tbl_list->p_CR_buf_28, CR_SIZE_28);
    p_step_cr->sec2_crs[24].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec2 cr[24] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_step_cr->sec2_crs[24].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_28), DC_U, DC_U);

    filter_offset = CR_OFFSET_29;
    tmp_uintptr = sec2_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec2_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec2_cfg_buf, p_flow_tbl_list->p_CR_buf_29, CR_SIZE_29);
    p_step_cr->sec2_crs[25].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec2 cr[25] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_step_cr->sec2_crs[25].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_29), DC_U, DC_U);

    filter_offset = CR_OFFSET_30;
    tmp_uintptr = sec2_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec2_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec2_cfg_buf, p_flow_tbl_list->p_CR_buf_30, CR_SIZE_30);
    p_step_cr->sec2_crs[26].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec2 cr[26] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_step_cr->sec2_crs[26].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_30), DC_U, DC_U);

    filter_offset = CR_OFFSET_31;
    tmp_uintptr = sec2_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec2_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec2_cfg_buf, p_flow_tbl_list->p_CR_buf_31, CR_SIZE_31);
    p_step_cr->sec2_crs[27].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec2 cr[27] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_step_cr->sec2_crs[27].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_31), DC_U, DC_U);

    filter_offset = CR_OFFSET_32;
    tmp_uintptr = sec2_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec2_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec2_cfg_buf, p_flow_tbl_list->p_CR_buf_32, CR_SIZE_32);
    p_step_cr->sec2_crs[28].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec2 cr[28] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_step_cr->sec2_crs[28].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_32), DC_U, DC_U);

    filter_offset = CR_OFFSET_33;
    tmp_uintptr = sec2_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec2_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec2_cfg_buf, p_flow_tbl_list->p_CR_buf_33, CR_SIZE_33);
    p_step_cr->sec2_crs[29].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec2 cr[29] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_step_cr->sec2_crs[29].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_33), DC_U, DC_U);

    filter_offset = CR_OFFSET_34;
    tmp_uintptr = sec2_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec2_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec2_cfg_buf, p_flow_tbl_list->p_CR_buf_34, CR_SIZE_34);
    p_step_cr->sec2_crs[30].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec2 cr[30] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_step_cr->sec2_crs[30].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_34), DC_U, DC_U);

    filter_offset = CR_OFFSET_35;
    tmp_uintptr = sec2_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec2_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec2_cfg_buf, p_flow_tbl_list->p_CR_buf_35, CR_SIZE_35);
    p_step_cr->sec2_crs[31].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec2 cr[31] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_step_cr->sec2_crs[31].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_35), DC_U, DC_U);

    filter_offset = CR_OFFSET_40;
    tmp_uintptr = sec2_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec2_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec2_cfg_buf, p_flow_tbl_list->p_CR_buf_40, CR_SIZE_40);
    p_step_cr->sec2_crs[32].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec2 cr[32] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_step_cr->sec2_crs[32].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_40), DC_U, DC_U);

    filter_offset = CR_OFFSET_36;
    tmp_uintptr = sec2_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec2_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec2_cfg_buf, p_flow_tbl_list->p_CR_buf_36, CR_SIZE_36);
    p_step_cr->sec2_crs[33].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec2 cr[33] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_step_cr->sec2_crs[33].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_36), DC_U, DC_U);

    filter_offset = CR_OFFSET_37;
    tmp_uintptr = sec2_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec2_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec2_cfg_buf, p_flow_tbl_list->p_CR_buf_37, CR_SIZE_37);
    p_step_cr->sec2_crs[34].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec2 cr[34] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_step_cr->sec2_crs[34].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_37), DC_U, DC_U);

    filter_offset = CR_OFFSET_38;
    tmp_uintptr = sec2_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec2_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec2_cfg_buf, p_flow_tbl_list->p_CR_buf_38, CR_SIZE_38);
    p_step_cr->sec2_crs[35].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec2 cr[35] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_step_cr->sec2_crs[35].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_38), DC_U, DC_U);

    filter_offset = CR_OFFSET_39;
    tmp_uintptr = sec2_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec2_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec2_cfg_buf, p_flow_tbl_list->p_CR_buf_39, CR_SIZE_39);
    p_step_cr->sec2_crs[36].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec2 cr[36] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_step_cr->sec2_crs[36].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_39), DC_U, DC_U);

    filter_offset = CR_OFFSET_41;
    tmp_uintptr = sec2_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec2_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec2_cfg_buf, p_flow_tbl_list->p_CR_buf_41, CR_SIZE_41);
    p_step_cr->sec2_crs[37].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec2 cr[37] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_step_cr->sec2_crs[37].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_41), DC_U, DC_U);

    filter_offset = CR_OFFSET_42;
    tmp_uintptr = sec3_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec3_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec3_cfg_buf, p_flow_tbl_list->p_CR_buf_42, CR_SIZE_42);
    p_step_cr->sec3_crs[0].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec3 cr[0] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_step_cr->sec3_crs[0].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_42), DC_U, DC_U);

    filter_offset = CR_OFFSET_43;
    tmp_uintptr = sec3_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec3_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec3_cfg_buf, p_flow_tbl_list->p_CR_buf_43, CR_SIZE_43);
    p_step_cr->sec3_crs[1].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec3 cr[1] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_step_cr->sec3_crs[1].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_43), DC_U, DC_U);

    filter_offset = CR_OFFSET_44;
    tmp_uintptr = sec3_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec3_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec3_cfg_buf, p_flow_tbl_list->p_CR_buf_44, CR_SIZE_44);
    p_step_cr->sec3_crs[2].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec3 cr[2] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_step_cr->sec3_crs[2].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_44), DC_U, DC_U);

    if (p_flow_tbl_list->p_CR_buf_45 != NULL) {
        filter_offset = CR_OFFSET_45;
        tmp_uintptr = sec4_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
        (void)amba_ik_system_memcpy(&p_sec4_cfg_buf, &tmp_uintptr, sizeof(void*));
        (void)amba_ik_system_memcpy(p_sec4_cfg_buf, p_flow_tbl_list->p_CR_buf_45, CR_SIZE_45);
        p_step_cr->sec4_crs[0].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
        amba_ik_system_print_uint64_5("Sec4 cr[0] addr 0x%08x, offset %d, src_addr %p ",
                                      (uint64)p_step_cr->sec4_crs[0].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_45), DC_U, DC_U);
    } else {
        amba_ik_system_print_str_5("[IK] CR_buf_45 is NULL", NULL, NULL, NULL, NULL, NULL);
    }

    if (p_flow_tbl_list->p_CR_buf_46 != NULL) {
        filter_offset = CR_OFFSET_46;
        tmp_uintptr = sec4_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
        (void)amba_ik_system_memcpy(&p_sec4_cfg_buf, &tmp_uintptr, sizeof(void*));
        (void)amba_ik_system_memcpy(p_sec4_cfg_buf, p_flow_tbl_list->p_CR_buf_46, CR_SIZE_46);
        p_step_cr->sec4_crs[1].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
        amba_ik_system_print_uint64_5("Sec4 cr[1] addr 0x%08x, offset %d, src_addr %p ",
                                      (uint64)p_step_cr->sec4_crs[1].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_46), DC_U, DC_U);
    } else {
        amba_ik_system_print_str_5("[IK] CR_buf_46 is NULL", NULL, NULL, NULL, NULL, NULL);
    }

    if (p_flow_tbl_list->p_CR_buf_47 != NULL) {
        filter_offset = CR_OFFSET_47;
        tmp_uintptr = sec4_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
        (void)amba_ik_system_memcpy(&p_sec4_cfg_buf, &tmp_uintptr, sizeof(void*));
        (void)amba_ik_system_memcpy(p_sec4_cfg_buf, p_flow_tbl_list->p_CR_buf_47, CR_SIZE_47);
        p_step_cr->sec4_crs[2].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
        amba_ik_system_print_uint64_5("Sec4 cr[2] addr 0x%08x, offset %d, src_addr %p ",
                                      (uint64)p_step_cr->sec4_crs[2].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_47), DC_U, DC_U);
    } else {
        amba_ik_system_print_str_5("[IK] CR_buf_47 is NULL", NULL, NULL, NULL, NULL, NULL);
    }

    if (p_flow_tbl_list->p_CR_buf_48 != NULL) {
        filter_offset = CR_OFFSET_48;
        tmp_uintptr = sec4_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
        (void)amba_ik_system_memcpy(&p_sec4_cfg_buf, &tmp_uintptr, sizeof(void*));
        (void)amba_ik_system_memcpy(p_sec4_cfg_buf, p_flow_tbl_list->p_CR_buf_48, CR_SIZE_48);
        p_step_cr->sec4_crs[3].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
        amba_ik_system_print_uint64_5("Sec4 cr[3] addr 0x%08x, offset %d, src_addr %p ",
                                      (uint64)p_step_cr->sec4_crs[3].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_48), DC_U, DC_U);
    } else {
        amba_ik_system_print_str_5("[IK] CR_buf_48 is NULL", NULL, NULL, NULL, NULL, NULL);
    }

    if (p_flow_tbl_list->p_CR_buf_49 != NULL) {
        filter_offset = CR_OFFSET_49;
        tmp_uintptr = sec4_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
        (void)amba_ik_system_memcpy(&p_sec4_cfg_buf, &tmp_uintptr, sizeof(void*));
        (void)amba_ik_system_memcpy(p_sec4_cfg_buf, p_flow_tbl_list->p_CR_buf_49, CR_SIZE_49);
        p_step_cr->sec4_crs[4].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
        amba_ik_system_print_uint64_5("Sec4 cr[4] addr 0x%08x, offset %d, src_addr %p ",
                                      (uint64)p_step_cr->sec4_crs[4].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_49), DC_U, DC_U);
    } else {
        amba_ik_system_print_str_5("[IK] CR_buf_49 is NULL", NULL, NULL, NULL, NULL, NULL);
    }

    if (p_flow_tbl_list->p_CR_buf_50 != NULL) {
        filter_offset = CR_OFFSET_50;
        tmp_uintptr = sec4_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
        (void)amba_ik_system_memcpy(&p_sec4_cfg_buf, &tmp_uintptr, sizeof(void*));
        (void)amba_ik_system_memcpy(p_sec4_cfg_buf, p_flow_tbl_list->p_CR_buf_50, CR_SIZE_50);
        p_step_cr->sec4_crs[5].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
        amba_ik_system_print_uint64_5("Sec4 cr[5] addr 0x%08x, offset %d, src_addr %p ",
                                      (uint64)p_step_cr->sec4_crs[5].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_50), DC_U, DC_U);
    } else {
        amba_ik_system_print_str_5("[IK] CR_buf_50 is NULL", NULL, NULL, NULL, NULL, NULL);
    }

    if (p_flow_tbl_list->p_CR_buf_51 != NULL) {
        filter_offset = CR_OFFSET_51;
        tmp_uintptr = sec4_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
        (void)amba_ik_system_memcpy(&p_sec4_cfg_buf, &tmp_uintptr, sizeof(void*));
        (void)amba_ik_system_memcpy(p_sec4_cfg_buf, p_flow_tbl_list->p_CR_buf_51, CR_SIZE_51);
        p_step_cr->sec4_crs[6].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
        amba_ik_system_print_uint64_5("Sec4 cr[6] addr 0x%08x, offset %d, src_addr %p ",
                                      (uint64)p_step_cr->sec4_crs[6].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_51), DC_U, DC_U);
    } else {
        amba_ik_system_print_str_5("[IK] CR_buf_51 is NULL", NULL, NULL, NULL, NULL, NULL);
    }

    if (p_flow_tbl_list->p_CR_buf_52 != NULL) {
        filter_offset = CR_OFFSET_52;
        tmp_uintptr = sec4_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
        (void)amba_ik_system_memcpy(&p_sec4_cfg_buf, &tmp_uintptr, sizeof(void*));
        (void)amba_ik_system_memcpy(p_sec4_cfg_buf, p_flow_tbl_list->p_CR_buf_52, CR_SIZE_52);
        p_step_cr->sec4_crs[7].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
        amba_ik_system_print_uint64_5("Sec4 cr[7] addr 0x%08x, offset %d, src_addr %p ",
                                      (uint64)p_step_cr->sec4_crs[7].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_52), DC_U, DC_U);
    } else {
        amba_ik_system_print_str_5("[IK] CR_buf_52 is NULL", NULL, NULL, NULL, NULL, NULL);
    }

    if (p_flow_tbl_list->p_CR_buf_100 != NULL) {
        filter_offset = CR_OFFSET_100;
        tmp_uintptr = sec11_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
        (void)amba_ik_system_memcpy(&p_sec11_cfg_buf, &tmp_uintptr, sizeof(void*));
        (void)amba_ik_system_memcpy(p_sec11_cfg_buf, p_flow_tbl_list->p_CR_buf_100, CR_SIZE_100);
        p_step_cr->sec11_crs[0].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
        amba_ik_system_print_uint64_5("Sec11 cr[0] addr 0x%08x, offset %d, src_addr %p ",
                                      (uint64)p_step_cr->sec11_crs[0].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_100), DC_U, DC_U);
    } else {
        amba_ik_system_print_str_5("[IK] CR_buf_100 is NULL", NULL, NULL, NULL, NULL, NULL);
    }

    if (p_flow_tbl_list->p_CR_buf_101 != NULL) {
        filter_offset = CR_OFFSET_101;
        tmp_uintptr = sec11_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
        (void)amba_ik_system_memcpy(&p_sec11_cfg_buf, &tmp_uintptr, sizeof(void*));
        (void)amba_ik_system_memcpy(p_sec11_cfg_buf, p_flow_tbl_list->p_CR_buf_101, CR_SIZE_101);
        p_step_cr->sec11_crs[1].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
        amba_ik_system_print_uint64_5("Sec11 cr[1] addr 0x%08x, offset %d, src_addr %p ",
                                     (uint64)p_step_cr->sec11_crs[1].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_101), DC_U, DC_U);
    } else {
        amba_ik_system_print_str_5("[IK] CR_buf_101 is NULL", NULL, NULL, NULL, NULL, NULL);
    }

    if (p_flow_tbl_list->p_CR_buf_102 != NULL) {
        filter_offset = CR_OFFSET_102;
        tmp_uintptr = sec11_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
        (void)amba_ik_system_memcpy(&p_sec11_cfg_buf, &tmp_uintptr, sizeof(void*));
        (void)amba_ik_system_memcpy(p_sec11_cfg_buf, p_flow_tbl_list->p_CR_buf_102, CR_SIZE_102);
        p_step_cr->sec11_crs[2].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
        amba_ik_system_print_uint64_5("Sec11 cr[2] addr 0x%08x, offset %d, src_addr %p ",
                                      (uint64)p_step_cr->sec11_crs[2].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_102), DC_U, DC_U);
    } else {
        amba_ik_system_print_str_5("[IK] CR_buf_102 is NULL", NULL, NULL, NULL, NULL, NULL);
    }

    if (p_flow_tbl_list->p_CR_buf_103 != NULL) {
        filter_offset = CR_OFFSET_103;
        tmp_uintptr = sec11_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
        (void)amba_ik_system_memcpy(&p_sec11_cfg_buf, &tmp_uintptr, sizeof(void*));
        (void)amba_ik_system_memcpy(p_sec11_cfg_buf, p_flow_tbl_list->p_CR_buf_103, CR_SIZE_103);
        p_step_cr->sec11_crs[3].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
        amba_ik_system_print_uint64_5("Sec11 cr[3] addr 0x%08x, offset %d, src_addr %p ",
                                      (uint64)p_step_cr->sec11_crs[3].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_103), DC_U, DC_U);
    } else {
        amba_ik_system_print_str_5("[IK] CR_buf_100 is NULL", NULL, NULL, NULL, NULL, NULL);
    }

    filter_offset = CR_OFFSET_111;
    tmp_uintptr = sec18_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec18_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec18_cfg_buf, p_flow_tbl_list->p_CR_buf_111, CR_SIZE_111);
    p_step_cr->sec18_crs[0].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec18 cr[0] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_step_cr->sec18_crs[0].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_111), DC_U, DC_U);

    filter_offset = CR_OFFSET_112;
    tmp_uintptr = sec18_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec18_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec18_cfg_buf, p_flow_tbl_list->p_CR_buf_112, CR_SIZE_112);
    p_step_cr->sec18_crs[1].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec18 cr[1] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_step_cr->sec18_crs[1].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_112), DC_U, DC_U);

    filter_offset = CR_OFFSET_113;
    tmp_uintptr = sec18_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec18_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec18_cfg_buf, p_flow_tbl_list->p_CR_buf_113, CR_SIZE_113);
    p_step_cr->sec18_crs[2].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec18 cr[2] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_step_cr->sec18_crs[2].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_113), DC_U, DC_U);

    filter_offset = CR_OFFSET_114;
    tmp_uintptr = sec18_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec18_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec18_cfg_buf, p_flow_tbl_list->p_CR_buf_114, CR_SIZE_114);
    p_step_cr->sec18_crs[3].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec18 cr[3] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_step_cr->sec18_crs[3].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_114), DC_U, DC_U);

    filter_offset = CR_OFFSET_115;
    tmp_uintptr = sec18_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec18_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec18_cfg_buf, p_flow_tbl_list->p_CR_buf_115, CR_SIZE_115);
    p_step_cr->sec18_crs[4].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec18 cr[4] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_step_cr->sec18_crs[4].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_115), DC_U, DC_U);

    filter_offset = CR_OFFSET_116;
    tmp_uintptr = sec18_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec18_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec18_cfg_buf, p_flow_tbl_list->p_CR_buf_116, CR_SIZE_116);
    p_step_cr->sec18_crs[5].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec18 cr[5] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_step_cr->sec18_crs[5].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_116), DC_U, DC_U);

    filter_offset = CR_OFFSET_117;
    tmp_uintptr = sec18_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec18_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec18_cfg_buf, p_flow_tbl_list->p_CR_buf_117, CR_SIZE_117);
    p_step_cr->sec18_crs[6].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec18 cr[6] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_step_cr->sec18_crs[6].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_117), DC_U, DC_U);

    filter_offset = CR_OFFSET_118;
    tmp_uintptr = sec18_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec18_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec18_cfg_buf, p_flow_tbl_list->p_CR_buf_118, CR_SIZE_118);
    p_step_cr->sec18_crs[7].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec18 cr[7] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_step_cr->sec18_crs[7].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_118), DC_U, DC_U);

    filter_offset = CR_OFFSET_119;
    tmp_uintptr = sec18_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec18_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec18_cfg_buf, p_flow_tbl_list->p_CR_buf_119, CR_SIZE_119);
    p_step_cr->sec18_crs[8].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec18 cr[8] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_step_cr->sec18_crs[8].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_119), DC_U, DC_U);

    filter_offset = CR_OFFSET_120;
    tmp_uintptr = sec18_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec18_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec18_cfg_buf, p_flow_tbl_list->p_CR_buf_120, CR_SIZE_120);
    p_step_cr->sec18_crs[9].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec18 cr[9] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_step_cr->sec18_crs[9].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_120), DC_U, DC_U);

    filter_offset = CR_OFFSET_121;
    tmp_uintptr = sec18_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec18_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec18_cfg_buf, p_flow_tbl_list->p_CR_buf_121, CR_SIZE_121);
    p_step_cr->sec18_crs[10].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec18 cr[10] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_step_cr->sec18_crs[10].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_121), DC_U, DC_U);

    return rval;
}

static uint32 img_cfg_motion_me1_amalgam_data_update(uintptr data_offset_base,
        idsp_step_crs_t *p_step_cr,
        const amalgam_motion_me1_flow_data_t *p_amalgam_data,
        const amba_ik_motion_me1_flow_tables_list_t *p_flow_tbl_list)
{
    uint32 rval = 0;
    uint32_t filter_offset;
    const uint8_t *tmp_p_u8;
    uintptr sec2_cfg_buf_offset;
    uintptr sec3_cfg_buf_offset;
    uintptr sec18_cfg_buf_offset;
    void* p_sec2_cfg_buf;
    void* p_sec3_cfg_buf;
    void* p_sec18_cfg_buf;
    uintptr tmp_uintptr;
    uint32 i;

    // IKC does not use IDSP_CONFIG_HDR_SIZE, however, IKS does, in order to keep it on the same page between IKC and IKS,
    tmp_p_u8 = p_amalgam_data->sec2_cfg_buf;
    (void)amba_ik_system_memcpy(&sec2_cfg_buf_offset, &tmp_p_u8, sizeof(tmp_uintptr));
    tmp_p_u8 = p_amalgam_data->sec3_cfg_buf;
    (void)amba_ik_system_memcpy(&sec3_cfg_buf_offset, &tmp_p_u8, sizeof(tmp_uintptr));
    tmp_p_u8 = p_amalgam_data->sec18_cfg_buf;
    (void)amba_ik_system_memcpy(&sec18_cfg_buf_offset, &tmp_p_u8, sizeof(tmp_uintptr));

    filter_offset = MODE_8_SEC_2_CR_OFFSET_26;
    tmp_uintptr = sec2_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec2_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec2_cfg_buf, p_flow_tbl_list->p_CR_buf_26, CR_SIZE_26);
    p_step_cr->sec2_crs[22].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec2 cr[22] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_step_cr->sec2_crs[22].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_26), DC_U, DC_U);

    filter_offset = MODE_8_SEC_2_CR_OFFSET_27;
    tmp_uintptr = sec2_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec2_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec2_cfg_buf, p_flow_tbl_list->p_CR_buf_27, CR_SIZE_27);
    p_step_cr->sec2_crs[23].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec2 cr[23] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_step_cr->sec2_crs[23].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_27), DC_U, DC_U);

    filter_offset = MODE_8_SEC_2_CR_OFFSET_29;
    tmp_uintptr = sec2_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec2_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec2_cfg_buf, p_flow_tbl_list->p_CR_buf_29, CR_SIZE_29);
    p_step_cr->sec2_crs[25].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec2 cr[25] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_step_cr->sec2_crs[25].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_29), DC_U, DC_U);

    filter_offset = MODE_8_SEC_2_CR_OFFSET_30;
    tmp_uintptr = sec2_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec2_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec2_cfg_buf, p_flow_tbl_list->p_CR_buf_30, CR_SIZE_30);
    p_step_cr->sec2_crs[26].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec2 cr[26] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_step_cr->sec2_crs[26].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_30), DC_U, DC_U);

    filter_offset = MODE_8_SEC_2_CR_OFFSET_31;
    tmp_uintptr = sec2_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec2_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec2_cfg_buf, p_flow_tbl_list->p_CR_buf_31, CR_SIZE_31);
    p_step_cr->sec2_crs[27].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec2 cr[27] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_step_cr->sec2_crs[27].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_31), DC_U, DC_U);

    filter_offset = MODE_8_SEC_2_CR_OFFSET_32;
    tmp_uintptr = sec2_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec2_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec2_cfg_buf, p_flow_tbl_list->p_CR_buf_32, CR_SIZE_32);
    p_step_cr->sec2_crs[28].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec2 cr[28] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_step_cr->sec2_crs[28].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_32), DC_U, DC_U);

    filter_offset = MODE_8_SEC_2_CR_OFFSET_33;
    tmp_uintptr = sec2_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec2_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec2_cfg_buf, p_flow_tbl_list->p_CR_buf_33, CR_SIZE_33);
    p_step_cr->sec2_crs[29].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec2 cr[29] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_step_cr->sec2_crs[29].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_33), DC_U, DC_U);

    filter_offset = MODE_8_SEC_2_CR_OFFSET_34;
    tmp_uintptr = sec2_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec2_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec2_cfg_buf, p_flow_tbl_list->p_CR_buf_34, CR_SIZE_34);
    p_step_cr->sec2_crs[30].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec2 cr[30] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_step_cr->sec2_crs[30].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_34), DC_U, DC_U);

    filter_offset = MODE_8_SEC_2_CR_OFFSET_35;
    tmp_uintptr = sec2_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec2_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec2_cfg_buf, p_flow_tbl_list->p_CR_buf_35, CR_SIZE_35);
    p_step_cr->sec2_crs[31].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec2 cr[31] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_step_cr->sec2_crs[31].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_35), DC_U, DC_U);

    filter_offset = MODE_8_SEC_2_CR_OFFSET_40;
    tmp_uintptr = sec2_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec2_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec2_cfg_buf, p_flow_tbl_list->p_CR_buf_40, CR_SIZE_40);
    p_step_cr->sec2_crs[32].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec2 cr[32] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_step_cr->sec2_crs[32].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_40), DC_U, DC_U);

    filter_offset = MODE_8_SEC_2_CR_OFFSET_36;
    tmp_uintptr = sec2_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec2_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec2_cfg_buf, p_flow_tbl_list->p_CR_buf_36, CR_SIZE_36);
    p_step_cr->sec2_crs[33].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec2 cr[33] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_step_cr->sec2_crs[33].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_36), DC_U, DC_U);

    filter_offset = MODE_8_SEC_2_CR_OFFSET_37;
    tmp_uintptr = sec2_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec2_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec2_cfg_buf, p_flow_tbl_list->p_CR_buf_37, CR_SIZE_37);
    p_step_cr->sec2_crs[34].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec2 cr[34] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_step_cr->sec2_crs[34].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_37), DC_U, DC_U);

    filter_offset = MODE_8_SEC_2_CR_OFFSET_38;
    tmp_uintptr = sec2_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec2_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec2_cfg_buf, p_flow_tbl_list->p_CR_buf_38, CR_SIZE_38);
    p_step_cr->sec2_crs[35].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec2 cr[35] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_step_cr->sec2_crs[35].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_38), DC_U, DC_U);

    filter_offset = MODE_8_SEC_2_CR_OFFSET_39;
    tmp_uintptr = sec2_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec2_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec2_cfg_buf, p_flow_tbl_list->p_CR_buf_39, CR_SIZE_39);
    p_step_cr->sec2_crs[36].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec2 cr[36] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_step_cr->sec2_crs[36].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_39), DC_U, DC_U);

    filter_offset = MODE_8_SEC_2_CR_OFFSET_41;
    tmp_uintptr = sec2_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec2_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec2_cfg_buf, p_flow_tbl_list->p_CR_buf_41, CR_SIZE_41);
    p_step_cr->sec2_crs[37].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec2 cr[37] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_step_cr->sec2_crs[37].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_41), DC_U, DC_U);

    filter_offset = CR_OFFSET_42;
    tmp_uintptr = sec3_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec3_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec3_cfg_buf, p_flow_tbl_list->p_CR_buf_42, CR_SIZE_42);
    p_step_cr->sec3_crs[0].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec3 cr[0] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_step_cr->sec3_crs[0].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_42), DC_U, DC_U);

    filter_offset = CR_OFFSET_43;
    tmp_uintptr = sec3_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec3_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec3_cfg_buf, p_flow_tbl_list->p_CR_buf_43, CR_SIZE_43);
    p_step_cr->sec3_crs[1].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec3 cr[1] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_step_cr->sec3_crs[1].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_43), DC_U, DC_U);

    filter_offset = CR_OFFSET_44;
    tmp_uintptr = sec3_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec3_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec3_cfg_buf, p_flow_tbl_list->p_CR_buf_44, CR_SIZE_44);
    p_step_cr->sec3_crs[2].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec3 cr[2] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_step_cr->sec3_crs[2].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_44), DC_U, DC_U);

    filter_offset = MODE_4_SEC_18_CR_OFFSET_112;
    tmp_uintptr = sec18_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec18_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec18_cfg_buf, p_flow_tbl_list->p_CR_buf_112, CR_SIZE_112);
    p_step_cr->sec18_crs[1].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec18 cr[1] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_step_cr->sec18_crs[1].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_112), DC_U, DC_U);

    filter_offset = MODE_4_SEC_18_CR_OFFSET_113;
    tmp_uintptr = sec18_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec18_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec18_cfg_buf, p_flow_tbl_list->p_CR_buf_113, CR_SIZE_113);
    p_step_cr->sec18_crs[2].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint32_5("Sec18 cr[2] addr 0x%08x, offset %d, src_addr 0x%08x ",
                                  p_step_cr->sec18_crs[2].config_region_addr << 7, filter_offset, DC_U, DC_U, DC_U);

    filter_offset = MODE_4_SEC_18_CR_OFFSET_114;
    tmp_uintptr = sec18_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec18_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec18_cfg_buf, p_flow_tbl_list->p_CR_buf_114, CR_SIZE_114);
    p_step_cr->sec18_crs[3].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint32_5("Sec18 cr[3] addr 0x%08x, offset %d, src_addr 0x%08x ",
                                  p_step_cr->sec18_crs[3].config_region_addr << 7, filter_offset, DC_U, DC_U, DC_U);

    filter_offset = MODE_4_SEC_18_CR_OFFSET_115;
    tmp_uintptr = sec18_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec18_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec18_cfg_buf, p_flow_tbl_list->p_CR_buf_115, CR_SIZE_115);
    p_step_cr->sec18_crs[4].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint32_5("Sec18 cr[4] addr 0x%08x, offset %d, src_addr 0x%08x ",
                                  p_step_cr->sec18_crs[4].config_region_addr << 7, filter_offset, DC_U, DC_U, DC_U);

    filter_offset = MODE_4_SEC_18_CR_OFFSET_117;
    tmp_uintptr = sec18_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec18_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec18_cfg_buf, p_flow_tbl_list->p_CR_buf_117, CR_SIZE_117);
    p_step_cr->sec18_crs[6].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec18 cr[6] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_step_cr->sec18_crs[6].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_117), DC_U, DC_U);

    filter_offset = MODE_4_SEC_18_CR_OFFSET_118;
    tmp_uintptr = sec18_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec18_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec18_cfg_buf, p_flow_tbl_list->p_CR_buf_118, CR_SIZE_118);
    p_step_cr->sec18_crs[7].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec18 cr[7] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_step_cr->sec18_crs[7].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_118), DC_U, DC_U);

    filter_offset = MODE_4_SEC_18_CR_OFFSET_119;
    tmp_uintptr = sec18_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec18_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec18_cfg_buf, p_flow_tbl_list->p_CR_buf_119, CR_SIZE_119);
    p_step_cr->sec18_crs[8].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint32_5("Sec18 cr[8] addr 0x%08x, offset %d, src_addr 0x%08x ",
                                  p_step_cr->sec18_crs[8].config_region_addr << 7, filter_offset, DC_U, DC_U, DC_U);

    {
        for (i=0U; i<22U; i++) {
            p_step_cr->sec2_crs[i].config_region_addr = 0;
        }
        p_step_cr->sec2_crs[24].config_region_addr = 0;

        p_step_cr->sec18_crs[0].config_region_addr = 0;
        p_step_cr->sec18_crs[5].config_region_addr = 0;
        p_step_cr->sec18_crs[9].config_region_addr = 0;
        p_step_cr->sec18_crs[10].config_region_addr = 0;
    }

    return rval;
}

static uint32 img_cfg_motion_a_amalgam_data_update(uintptr data_offset_base,
        idsp_step_crs_t *p_step_cr,
        const amalgam_motion_a_flow_data_t *p_amalgam_data,
        const amba_ik_motion_a_flow_tables_list_t *p_flow_tbl_list)
{
    uint32 rval = 0;
    uint32_t filter_offset;
    const uint8_t *tmp_p_u8;
    uintptr sec2_cfg_buf_offset;
    uintptr sec18_cfg_buf_offset;
    void* p_sec2_cfg_buf;
    void* p_sec18_cfg_buf;
    uintptr tmp_uintptr;
    uint32 i;

    // IKC does not use IDSP_CONFIG_HDR_SIZE, however, IKS does, in order to keep it on the same page between IKC and IKS,
    tmp_p_u8 = p_amalgam_data->sec2_cfg_buf;
    (void)amba_ik_system_memcpy(&sec2_cfg_buf_offset, &tmp_p_u8, sizeof(tmp_uintptr));
    tmp_p_u8 = p_amalgam_data->sec18_cfg_buf;
    (void)amba_ik_system_memcpy(&sec18_cfg_buf_offset, &tmp_p_u8, sizeof(tmp_uintptr));

    filter_offset = MODE_8_SEC_2_CR_OFFSET_26;
    tmp_uintptr = sec2_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec2_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec2_cfg_buf, p_flow_tbl_list->p_CR_buf_26, CR_SIZE_26);
    p_step_cr->sec2_crs[22].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec2 cr[22] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_step_cr->sec2_crs[22].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_26), DC_U, DC_U);

    filter_offset = MODE_8_SEC_2_CR_OFFSET_27;
    tmp_uintptr = sec2_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec2_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec2_cfg_buf, p_flow_tbl_list->p_CR_buf_27, CR_SIZE_27);
    p_step_cr->sec2_crs[23].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec2 cr[23] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_step_cr->sec2_crs[23].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_27), DC_U, DC_U);

    filter_offset = MODE_8_SEC_2_CR_OFFSET_29;
    tmp_uintptr = sec2_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec2_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec2_cfg_buf, p_flow_tbl_list->p_CR_buf_29, CR_SIZE_29);
    p_step_cr->sec2_crs[25].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec2 cr[25] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_step_cr->sec2_crs[25].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_29), DC_U, DC_U);

    filter_offset = MODE_8_SEC_2_CR_OFFSET_30;
    tmp_uintptr = sec2_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec2_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec2_cfg_buf, p_flow_tbl_list->p_CR_buf_30, CR_SIZE_30);
    p_step_cr->sec2_crs[26].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec2 cr[26] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_step_cr->sec2_crs[26].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_30), DC_U, DC_U);

    filter_offset = MODE_8_SEC_2_CR_OFFSET_31;
    tmp_uintptr = sec2_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec2_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec2_cfg_buf, p_flow_tbl_list->p_CR_buf_31, CR_SIZE_31);
    p_step_cr->sec2_crs[27].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec2 cr[27] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_step_cr->sec2_crs[27].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_31), DC_U, DC_U);

    filter_offset = MODE_8_SEC_2_CR_OFFSET_32;
    tmp_uintptr = sec2_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec2_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec2_cfg_buf, p_flow_tbl_list->p_CR_buf_32, CR_SIZE_32);
    p_step_cr->sec2_crs[28].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec2 cr[28] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_step_cr->sec2_crs[28].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_32), DC_U, DC_U);

    filter_offset = MODE_8_SEC_2_CR_OFFSET_33;
    tmp_uintptr = sec2_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec2_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec2_cfg_buf, p_flow_tbl_list->p_CR_buf_33, CR_SIZE_33);
    p_step_cr->sec2_crs[29].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec2 cr[29] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_step_cr->sec2_crs[29].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_33), DC_U, DC_U);

    filter_offset = MODE_8_SEC_2_CR_OFFSET_34;
    tmp_uintptr = sec2_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec2_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec2_cfg_buf, p_flow_tbl_list->p_CR_buf_34, CR_SIZE_34);
    p_step_cr->sec2_crs[30].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec2 cr[30] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_step_cr->sec2_crs[30].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_34), DC_U, DC_U);

    filter_offset = MODE_8_SEC_2_CR_OFFSET_35;
    tmp_uintptr = sec2_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec2_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec2_cfg_buf, p_flow_tbl_list->p_CR_buf_35, CR_SIZE_35);
    p_step_cr->sec2_crs[31].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec2 cr[31] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_step_cr->sec2_crs[31].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_35), DC_U, DC_U);

    filter_offset = MODE_8_SEC_2_CR_OFFSET_40;
    tmp_uintptr = sec2_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec2_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec2_cfg_buf, p_flow_tbl_list->p_CR_buf_40, CR_SIZE_40);
    p_step_cr->sec2_crs[32].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec2 cr[32] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_step_cr->sec2_crs[32].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_40), DC_U, DC_U);

    filter_offset = MODE_8_SEC_2_CR_OFFSET_36;
    tmp_uintptr = sec2_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec2_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec2_cfg_buf, p_flow_tbl_list->p_CR_buf_36, CR_SIZE_36);
    p_step_cr->sec2_crs[33].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec2 cr[33] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_step_cr->sec2_crs[33].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_36), DC_U, DC_U);

    filter_offset = MODE_8_SEC_2_CR_OFFSET_37;
    tmp_uintptr = sec2_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec2_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec2_cfg_buf, p_flow_tbl_list->p_CR_buf_37, CR_SIZE_37);
    p_step_cr->sec2_crs[34].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec2 cr[34] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_step_cr->sec2_crs[34].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_37), DC_U, DC_U);

    filter_offset = MODE_8_SEC_2_CR_OFFSET_38;
    tmp_uintptr = sec2_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec2_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec2_cfg_buf, p_flow_tbl_list->p_CR_buf_38, CR_SIZE_38);
    p_step_cr->sec2_crs[35].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec2 cr[35] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_step_cr->sec2_crs[35].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_38), DC_U, DC_U);

    filter_offset = MODE_8_SEC_2_CR_OFFSET_39;
    tmp_uintptr = sec2_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec2_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec2_cfg_buf, p_flow_tbl_list->p_CR_buf_39, CR_SIZE_39);
    p_step_cr->sec2_crs[36].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec2 cr[36] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_step_cr->sec2_crs[36].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_39), DC_U, DC_U);

    filter_offset = MODE_8_SEC_2_CR_OFFSET_41;
    tmp_uintptr = sec2_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec2_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec2_cfg_buf, p_flow_tbl_list->p_CR_buf_41, CR_SIZE_41);
    p_step_cr->sec2_crs[37].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec2 cr[37] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_step_cr->sec2_crs[37].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_41), DC_U, DC_U);

    filter_offset = MODE_4_SEC_18_CR_OFFSET_112;
    tmp_uintptr = sec18_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec18_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec18_cfg_buf, p_flow_tbl_list->p_CR_buf_112, CR_SIZE_112);
    p_step_cr->sec18_crs[1].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec18 cr[1] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_step_cr->sec18_crs[1].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_112), DC_U, DC_U);

    filter_offset = MODE_4_SEC_18_CR_OFFSET_113;
    tmp_uintptr = sec18_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec18_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec18_cfg_buf, p_flow_tbl_list->p_CR_buf_113, CR_SIZE_113);
    p_step_cr->sec18_crs[2].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint32_5("Sec18 cr[2] addr 0x%08x, offset %d, src_addr 0x%08x ",
                                  p_step_cr->sec18_crs[2].config_region_addr << 7, filter_offset, DC_U, DC_U, DC_U);

    filter_offset = MODE_4_SEC_18_CR_OFFSET_114;
    tmp_uintptr = sec18_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec18_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec18_cfg_buf, p_flow_tbl_list->p_CR_buf_114, CR_SIZE_114);
    p_step_cr->sec18_crs[3].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint32_5("Sec18 cr[3] addr 0x%08x, offset %d, src_addr 0x%08x ",
                                  p_step_cr->sec18_crs[3].config_region_addr << 7, filter_offset, DC_U, DC_U, DC_U);

    filter_offset = MODE_4_SEC_18_CR_OFFSET_115;
    tmp_uintptr = sec18_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec18_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec18_cfg_buf, p_flow_tbl_list->p_CR_buf_115, CR_SIZE_115);
    p_step_cr->sec18_crs[4].config_region_addr = (uint32_t)(tmp_uintptr - data_offset_base) >> 7U;
    amba_ik_system_print_uint32_5("Sec18 cr[4] addr 0x%08x, offset %d, src_addr 0x%08x ",
                                  p_step_cr->sec18_crs[4].config_region_addr << 7, filter_offset, DC_U, DC_U, DC_U);

    filter_offset = MODE_4_SEC_18_CR_OFFSET_117;
    tmp_uintptr = sec18_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec18_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec18_cfg_buf, p_flow_tbl_list->p_CR_buf_117, CR_SIZE_117);
    p_step_cr->sec18_crs[6].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec18 cr[6] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_step_cr->sec18_crs[6].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_117), DC_U, DC_U);

    filter_offset = MODE_4_SEC_18_CR_OFFSET_118;
    tmp_uintptr = sec18_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec18_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec18_cfg_buf, p_flow_tbl_list->p_CR_buf_118, CR_SIZE_118);
    p_step_cr->sec18_crs[7].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec18 cr[7] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_step_cr->sec18_crs[7].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_118), DC_U, DC_U);

    filter_offset = MODE_4_SEC_18_CR_OFFSET_119;
    tmp_uintptr = sec18_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec18_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec18_cfg_buf, p_flow_tbl_list->p_CR_buf_119, CR_SIZE_119);
    p_step_cr->sec18_crs[8].config_region_addr = (uint32_t)(tmp_uintptr - data_offset_base) >> 7U;
    amba_ik_system_print_uint32_5("Sec18 cr[8] addr 0x%08x, offset %d, src_addr 0x%08x ",
                                  p_step_cr->sec18_crs[8].config_region_addr << 7, filter_offset, DC_U, DC_U, DC_U);

    {
        for (i=0U; i<22U; i++) {
            p_step_cr->sec2_crs[i].config_region_addr = 0;
        }
        p_step_cr->sec2_crs[24].config_region_addr = 0;

        p_step_cr->sec18_crs[0].config_region_addr = 0;
        p_step_cr->sec18_crs[5].config_region_addr = 0;
        p_step_cr->sec18_crs[9].config_region_addr = 0;
        p_step_cr->sec18_crs[10].config_region_addr = 0;
    }

    return rval;
}

static uint32 img_cfg_motion_b_amalgam_data_update(uintptr data_offset_base,
        idsp_step_crs_t *p_step_cr,
        const amalgam_motion_b_flow_data_t *p_amalgam_data,
        const amba_ik_motion_b_flow_tables_list_t *p_flow_tbl_list)
{
    uint32 rval = 0;
    uint32_t filter_offset;
    const uint8_t *tmp_p_u8;
    uintptr sec2_cfg_buf_offset;
    uintptr sec18_cfg_buf_offset;
    void* p_sec2_cfg_buf;
    void* p_sec18_cfg_buf;
    uintptr tmp_uintptr;
    uint32 i;

    // IKC does not use IDSP_CONFIG_HDR_SIZE, however, IKS does, in order to keep it on the same page between IKC and IKS,
    tmp_p_u8 = p_amalgam_data->sec2_cfg_buf;
    (void)amba_ik_system_memcpy(&sec2_cfg_buf_offset, &tmp_p_u8, sizeof(tmp_uintptr));
    tmp_p_u8 = p_amalgam_data->sec18_cfg_buf;
    (void)amba_ik_system_memcpy(&sec18_cfg_buf_offset, &tmp_p_u8, sizeof(tmp_uintptr));

    filter_offset = MODE_8_SEC_2_CR_OFFSET_26;
    tmp_uintptr = sec2_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec2_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec2_cfg_buf, p_flow_tbl_list->p_CR_buf_26, CR_SIZE_26);
    p_step_cr->sec2_crs[22].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec2 cr[22] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_step_cr->sec2_crs[22].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_26), DC_U, DC_U);

    filter_offset = MODE_8_SEC_2_CR_OFFSET_27;
    tmp_uintptr = sec2_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec2_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec2_cfg_buf, p_flow_tbl_list->p_CR_buf_27, CR_SIZE_27);
    p_step_cr->sec2_crs[23].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec2 cr[23] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_step_cr->sec2_crs[23].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_27), DC_U, DC_U);

    filter_offset = MODE_8_SEC_2_CR_OFFSET_29;
    tmp_uintptr = sec2_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec2_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec2_cfg_buf, p_flow_tbl_list->p_CR_buf_29, CR_SIZE_29);
    p_step_cr->sec2_crs[25].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec2 cr[25] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_step_cr->sec2_crs[25].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_29), DC_U, DC_U);

    filter_offset = MODE_8_SEC_2_CR_OFFSET_30;
    tmp_uintptr = sec2_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec2_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec2_cfg_buf, p_flow_tbl_list->p_CR_buf_30, CR_SIZE_30);
    p_step_cr->sec2_crs[26].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec2 cr[26] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_step_cr->sec2_crs[26].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_30), DC_U, DC_U);

    filter_offset = MODE_8_SEC_2_CR_OFFSET_31;
    tmp_uintptr = sec2_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec2_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec2_cfg_buf, p_flow_tbl_list->p_CR_buf_31, CR_SIZE_31);
    p_step_cr->sec2_crs[27].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec2 cr[27] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_step_cr->sec2_crs[27].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_31), DC_U, DC_U);

    filter_offset = MODE_8_SEC_2_CR_OFFSET_32;
    tmp_uintptr = sec2_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec2_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec2_cfg_buf, p_flow_tbl_list->p_CR_buf_32, CR_SIZE_32);
    p_step_cr->sec2_crs[28].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec2 cr[28] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_step_cr->sec2_crs[28].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_32), DC_U, DC_U);

    filter_offset = MODE_8_SEC_2_CR_OFFSET_33;
    tmp_uintptr = sec2_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec2_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec2_cfg_buf, p_flow_tbl_list->p_CR_buf_33, CR_SIZE_33);
    p_step_cr->sec2_crs[29].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec2 cr[29] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_step_cr->sec2_crs[29].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_33), DC_U, DC_U);

    filter_offset = MODE_8_SEC_2_CR_OFFSET_34;
    tmp_uintptr = sec2_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec2_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec2_cfg_buf, p_flow_tbl_list->p_CR_buf_34, CR_SIZE_34);
    p_step_cr->sec2_crs[30].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec2 cr[30] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_step_cr->sec2_crs[30].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_34), DC_U, DC_U);

    filter_offset = MODE_8_SEC_2_CR_OFFSET_35;
    tmp_uintptr = sec2_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec2_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec2_cfg_buf, p_flow_tbl_list->p_CR_buf_35, CR_SIZE_35);
    p_step_cr->sec2_crs[31].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec2 cr[31] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_step_cr->sec2_crs[31].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_35), DC_U, DC_U);

    filter_offset = MODE_8_SEC_2_CR_OFFSET_40;
    tmp_uintptr = sec2_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec2_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec2_cfg_buf, p_flow_tbl_list->p_CR_buf_40, CR_SIZE_40);
    p_step_cr->sec2_crs[32].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec2 cr[32] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_step_cr->sec2_crs[32].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_40), DC_U, DC_U);

    filter_offset = MODE_8_SEC_2_CR_OFFSET_36;
    tmp_uintptr = sec2_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec2_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec2_cfg_buf, p_flow_tbl_list->p_CR_buf_36, CR_SIZE_36);
    p_step_cr->sec2_crs[33].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec2 cr[33] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_step_cr->sec2_crs[33].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_36), DC_U, DC_U);

    filter_offset = MODE_8_SEC_2_CR_OFFSET_37;
    tmp_uintptr = sec2_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec2_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec2_cfg_buf, p_flow_tbl_list->p_CR_buf_37, CR_SIZE_37);
    p_step_cr->sec2_crs[34].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec2 cr[34] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_step_cr->sec2_crs[34].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_37), DC_U, DC_U);

    filter_offset = MODE_8_SEC_2_CR_OFFSET_38;
    tmp_uintptr = sec2_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec2_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec2_cfg_buf, p_flow_tbl_list->p_CR_buf_38, CR_SIZE_38);
    p_step_cr->sec2_crs[35].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec2 cr[35] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_step_cr->sec2_crs[35].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_38), DC_U, DC_U);

    filter_offset = MODE_8_SEC_2_CR_OFFSET_39;
    tmp_uintptr = sec2_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec2_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec2_cfg_buf, p_flow_tbl_list->p_CR_buf_39, CR_SIZE_39);
    p_step_cr->sec2_crs[36].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec2 cr[36] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_step_cr->sec2_crs[36].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_39), DC_U, DC_U);

    filter_offset = MODE_8_SEC_2_CR_OFFSET_41;
    tmp_uintptr = sec2_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec2_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec2_cfg_buf, p_flow_tbl_list->p_CR_buf_41, CR_SIZE_41);
    p_step_cr->sec2_crs[37].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec2 cr[37] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_step_cr->sec2_crs[37].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_41), DC_U, DC_U);

    filter_offset = MODE_4_SEC_18_CR_OFFSET_112;
    tmp_uintptr = sec18_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec18_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec18_cfg_buf, p_flow_tbl_list->p_CR_buf_112, CR_SIZE_112);
    p_step_cr->sec18_crs[1].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec18 cr[1] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_step_cr->sec18_crs[1].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_112), DC_U, DC_U);

    filter_offset = MODE_4_SEC_18_CR_OFFSET_113;
    tmp_uintptr = sec18_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec18_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec18_cfg_buf, p_flow_tbl_list->p_CR_buf_113, CR_SIZE_113);
    p_step_cr->sec18_crs[2].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint32_5("Sec18 cr[2] addr 0x%08x, offset %d, src_addr 0x%08x ",
                                  p_step_cr->sec18_crs[2].config_region_addr << 7, filter_offset, DC_U, DC_U, DC_U);

    filter_offset = MODE_4_SEC_18_CR_OFFSET_114;
    tmp_uintptr = sec18_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec18_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec18_cfg_buf, p_flow_tbl_list->p_CR_buf_114, CR_SIZE_114);
    p_step_cr->sec18_crs[3].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint32_5("Sec18 cr[3] addr 0x%08x, offset %d, src_addr 0x%08x ",
                                  p_step_cr->sec18_crs[3].config_region_addr << 7, filter_offset, DC_U, DC_U, DC_U);

    filter_offset = MODE_4_SEC_18_CR_OFFSET_115;
    tmp_uintptr = sec18_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec18_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec18_cfg_buf, p_flow_tbl_list->p_CR_buf_115, CR_SIZE_115);
    p_step_cr->sec18_crs[4].config_region_addr = (uint32_t)(tmp_uintptr - data_offset_base) >> 7U;
    amba_ik_system_print_uint32_5("Sec18 cr[4] addr 0x%08x, offset %d, src_addr 0x%08x ",
                                  p_step_cr->sec18_crs[4].config_region_addr << 7, filter_offset, DC_U, DC_U, DC_U);

    filter_offset = MODE_4_SEC_18_CR_OFFSET_117;
    tmp_uintptr = sec18_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec18_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec18_cfg_buf, p_flow_tbl_list->p_CR_buf_117, CR_SIZE_117);
    p_step_cr->sec18_crs[6].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec18 cr[6] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_step_cr->sec18_crs[6].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_117), DC_U, DC_U);

    filter_offset = MODE_4_SEC_18_CR_OFFSET_118;
    tmp_uintptr = sec18_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec18_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec18_cfg_buf, p_flow_tbl_list->p_CR_buf_118, CR_SIZE_118);
    p_step_cr->sec18_crs[7].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec18 cr[7] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_step_cr->sec18_crs[7].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_118), DC_U, DC_U);

    filter_offset = MODE_4_SEC_18_CR_OFFSET_119;
    tmp_uintptr = sec18_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec18_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec18_cfg_buf, p_flow_tbl_list->p_CR_buf_119, CR_SIZE_119);
    p_step_cr->sec18_crs[8].config_region_addr = (uint32_t)(tmp_uintptr - data_offset_base) >> 7U;
    amba_ik_system_print_uint32_5("Sec18 cr[8] addr 0x%08x, offset %d, src_addr 0x%08x ",
                                  p_step_cr->sec18_crs[8].config_region_addr << 7, filter_offset, DC_U, DC_U, DC_U);

    {
        for (i=0U; i<22U; i++) {
            p_step_cr->sec2_crs[i].config_region_addr = 0;
        }
        p_step_cr->sec2_crs[24].config_region_addr = 0;

        p_step_cr->sec18_crs[0].config_region_addr = 0;
        p_step_cr->sec18_crs[5].config_region_addr = 0;
        p_step_cr->sec18_crs[9].config_region_addr = 0;
        p_step_cr->sec18_crs[10].config_region_addr = 0;
    }

    return rval;
}

static uint32 img_cfg_motion_c_amalgam_data_update(uintptr data_offset_base,
        idsp_step_crs_t *p_step_cr,
        const amalgam_motion_c_flow_data_t *p_amalgam_data,
        const amba_ik_motion_c_flow_tables_list_t *p_flow_tbl_list)
{
    uint32 rval = 0;
    uint32_t filter_offset;
    const uint8_t *tmp_p_u8;
    uintptr sec2_cfg_buf_offset;
    uintptr sec18_cfg_buf_offset;
    void* p_sec2_cfg_buf;
    void* p_sec18_cfg_buf;
    uintptr tmp_uintptr;
    uint32 i;

    // IKC does not use IDSP_CONFIG_HDR_SIZE, however, IKS does, in order to keep it on the same page between IKC and IKS,
    tmp_p_u8 = p_amalgam_data->sec2_cfg_buf;
    (void)amba_ik_system_memcpy(&sec2_cfg_buf_offset, &tmp_p_u8, sizeof(tmp_uintptr));
    tmp_p_u8 = p_amalgam_data->sec18_cfg_buf;
    (void)amba_ik_system_memcpy(&sec18_cfg_buf_offset, &tmp_p_u8, sizeof(tmp_uintptr));

    filter_offset = MODE_8_SEC_2_CR_OFFSET_26;
    tmp_uintptr = sec2_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec2_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec2_cfg_buf, p_flow_tbl_list->p_CR_buf_26, CR_SIZE_26);
    p_step_cr->sec2_crs[22].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec2 cr[22] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_step_cr->sec2_crs[22].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_26), DC_U, DC_U);

    filter_offset = MODE_8_SEC_2_CR_OFFSET_27;
    tmp_uintptr = sec2_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec2_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec2_cfg_buf, p_flow_tbl_list->p_CR_buf_27, CR_SIZE_27);
    p_step_cr->sec2_crs[23].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec2 cr[23] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_step_cr->sec2_crs[23].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_27), DC_U, DC_U);

    filter_offset = MODE_8_SEC_2_CR_OFFSET_29;
    tmp_uintptr = sec2_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec2_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec2_cfg_buf, p_flow_tbl_list->p_CR_buf_29, CR_SIZE_29);
    p_step_cr->sec2_crs[25].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec2 cr[25] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_step_cr->sec2_crs[25].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_29), DC_U, DC_U);

    filter_offset = MODE_8_SEC_2_CR_OFFSET_30;
    tmp_uintptr = sec2_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec2_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec2_cfg_buf, p_flow_tbl_list->p_CR_buf_30, CR_SIZE_30);
    p_step_cr->sec2_crs[26].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec2 cr[26] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_step_cr->sec2_crs[26].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_30), DC_U, DC_U);

    filter_offset = MODE_8_SEC_2_CR_OFFSET_31;
    tmp_uintptr = sec2_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec2_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec2_cfg_buf, p_flow_tbl_list->p_CR_buf_31, CR_SIZE_31);
    p_step_cr->sec2_crs[27].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec2 cr[27] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_step_cr->sec2_crs[27].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_31), DC_U, DC_U);

    filter_offset = MODE_8_SEC_2_CR_OFFSET_32;
    tmp_uintptr = sec2_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec2_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec2_cfg_buf, p_flow_tbl_list->p_CR_buf_32, CR_SIZE_32);
    p_step_cr->sec2_crs[28].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec2 cr[28] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_step_cr->sec2_crs[28].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_32), DC_U, DC_U);

    filter_offset = MODE_8_SEC_2_CR_OFFSET_33;
    tmp_uintptr = sec2_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec2_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec2_cfg_buf, p_flow_tbl_list->p_CR_buf_33, CR_SIZE_33);
    p_step_cr->sec2_crs[29].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec2 cr[29] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_step_cr->sec2_crs[29].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_33), DC_U, DC_U);

    filter_offset = MODE_8_SEC_2_CR_OFFSET_34;
    tmp_uintptr = sec2_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec2_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec2_cfg_buf, p_flow_tbl_list->p_CR_buf_34, CR_SIZE_34);
    p_step_cr->sec2_crs[30].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec2 cr[30] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_step_cr->sec2_crs[30].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_34), DC_U, DC_U);

    filter_offset = MODE_8_SEC_2_CR_OFFSET_35;
    tmp_uintptr = sec2_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec2_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec2_cfg_buf, p_flow_tbl_list->p_CR_buf_35, CR_SIZE_35);
    p_step_cr->sec2_crs[31].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec2 cr[31] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_step_cr->sec2_crs[31].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_35), DC_U, DC_U);

    filter_offset = MODE_8_SEC_2_CR_OFFSET_40;
    tmp_uintptr = sec2_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec2_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec2_cfg_buf, p_flow_tbl_list->p_CR_buf_40, CR_SIZE_40);
    p_step_cr->sec2_crs[32].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec2 cr[32] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_step_cr->sec2_crs[32].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_40), DC_U, DC_U);

    filter_offset = MODE_8_SEC_2_CR_OFFSET_36;
    tmp_uintptr = sec2_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec2_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec2_cfg_buf, p_flow_tbl_list->p_CR_buf_36, CR_SIZE_36);
    p_step_cr->sec2_crs[33].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec2 cr[33] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_step_cr->sec2_crs[33].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_36), DC_U, DC_U);

    filter_offset = MODE_8_SEC_2_CR_OFFSET_37;
    tmp_uintptr = sec2_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec2_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec2_cfg_buf, p_flow_tbl_list->p_CR_buf_37, CR_SIZE_37);
    p_step_cr->sec2_crs[34].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec2 cr[34] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_step_cr->sec2_crs[34].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_37), DC_U, DC_U);

    filter_offset = MODE_8_SEC_2_CR_OFFSET_38;
    tmp_uintptr = sec2_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec2_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec2_cfg_buf, p_flow_tbl_list->p_CR_buf_38, CR_SIZE_38);
    p_step_cr->sec2_crs[35].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec2 cr[35] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_step_cr->sec2_crs[35].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_38), DC_U, DC_U);

    filter_offset = MODE_8_SEC_2_CR_OFFSET_39;
    tmp_uintptr = sec2_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec2_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec2_cfg_buf, p_flow_tbl_list->p_CR_buf_39, CR_SIZE_39);
    p_step_cr->sec2_crs[36].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec2 cr[36] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_step_cr->sec2_crs[36].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_39), DC_U, DC_U);

    filter_offset = MODE_8_SEC_2_CR_OFFSET_41;
    tmp_uintptr = sec2_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec2_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec2_cfg_buf, p_flow_tbl_list->p_CR_buf_41, CR_SIZE_41);
    p_step_cr->sec2_crs[37].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec2 cr[37] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_step_cr->sec2_crs[37].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_41), DC_U, DC_U);

    filter_offset = MODE_4_SEC_18_CR_OFFSET_112;
    tmp_uintptr = sec18_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec18_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec18_cfg_buf, p_flow_tbl_list->p_CR_buf_112, CR_SIZE_112);
    p_step_cr->sec18_crs[1].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec18 cr[1] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_step_cr->sec18_crs[1].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_112), DC_U, DC_U);

    filter_offset = MODE_4_SEC_18_CR_OFFSET_113;
    tmp_uintptr = sec18_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec18_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec18_cfg_buf, p_flow_tbl_list->p_CR_buf_113, CR_SIZE_113);
    p_step_cr->sec18_crs[2].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint32_5("Sec18 cr[2] addr 0x%08x, offset %d, src_addr 0x%08x ",
                                  p_step_cr->sec18_crs[2].config_region_addr << 7, filter_offset, DC_U, DC_U, DC_U);

    filter_offset = MODE_4_SEC_18_CR_OFFSET_114;
    tmp_uintptr = sec18_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec18_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec18_cfg_buf, p_flow_tbl_list->p_CR_buf_114, CR_SIZE_114);
    p_step_cr->sec18_crs[3].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint32_5("Sec18 cr[3] addr 0x%08x, offset %d, src_addr 0x%08x ",
                                  p_step_cr->sec18_crs[3].config_region_addr << 7, filter_offset, DC_U, DC_U, DC_U);

    filter_offset = MODE_4_SEC_18_CR_OFFSET_115;
    tmp_uintptr = sec18_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec18_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec18_cfg_buf, p_flow_tbl_list->p_CR_buf_115, CR_SIZE_115);
    p_step_cr->sec18_crs[4].config_region_addr = (uint32_t)(tmp_uintptr - data_offset_base) >> 7U;
    amba_ik_system_print_uint32_5("Sec18 cr[4] addr 0x%08x, offset %d, src_addr 0x%08x ",
                                  p_step_cr->sec18_crs[4].config_region_addr << 7, filter_offset, DC_U, DC_U, DC_U);

    filter_offset = MODE_4_SEC_18_CR_OFFSET_117;
    tmp_uintptr = sec18_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec18_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec18_cfg_buf, p_flow_tbl_list->p_CR_buf_117, CR_SIZE_117);
    p_step_cr->sec18_crs[6].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec18 cr[6] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_step_cr->sec18_crs[6].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_117), DC_U, DC_U);

    filter_offset = MODE_4_SEC_18_CR_OFFSET_118;
    tmp_uintptr = sec18_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec18_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec18_cfg_buf, p_flow_tbl_list->p_CR_buf_118, CR_SIZE_118);
    p_step_cr->sec18_crs[7].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec18 cr[7] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_step_cr->sec18_crs[7].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_118), DC_U, DC_U);

    filter_offset = MODE_4_SEC_18_CR_OFFSET_119;
    tmp_uintptr = sec18_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec18_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec18_cfg_buf, p_flow_tbl_list->p_CR_buf_119, CR_SIZE_119);
    p_step_cr->sec18_crs[8].config_region_addr = (uint32_t)(tmp_uintptr - data_offset_base) >> 7U;
    amba_ik_system_print_uint32_5("Sec18 cr[8] addr 0x%08x, offset %d, src_addr 0x%08x ",
                                  p_step_cr->sec18_crs[8].config_region_addr << 7, filter_offset, DC_U, DC_U, DC_U);

    {
        for (i=0U; i<22U; i++) {
            p_step_cr->sec2_crs[i].config_region_addr = 0;
        }
        p_step_cr->sec2_crs[24].config_region_addr = 0;

        p_step_cr->sec18_crs[0].config_region_addr = 0;
        p_step_cr->sec18_crs[5].config_region_addr = 0;
        p_step_cr->sec18_crs[9].config_region_addr = 0;
        p_step_cr->sec18_crs[10].config_region_addr = 0;
    }

    return rval;
}
#if SUPPORT_FUSION
static uint32 img_cfg_mono1_8_amalgam_data_update(uintptr data_offset_base,
        idsp_step_crs_t *p_step_cr,
        const amalgam_r2y_flow_data_t *p_amalgam_data,
        const amba_ik_r2y_flow_tables_list_t *p_flow_tbl_list)
{
    uint32 rval = 0;
    uint32_t filter_offset;
    const uint8_t *tmp_p_u8;
    uintptr sec2_cfg_buf_offset;
    uintptr sec3_cfg_buf_offset;
    uintptr sec4_cfg_buf_offset;
    uintptr sec11_cfg_buf_offset;
    uintptr sec18_cfg_buf_offset;
    void* p_sec2_cfg_buf;
    void* p_sec3_cfg_buf;
    void* p_sec4_cfg_buf;
    void* p_sec11_cfg_buf;
    void* p_sec18_cfg_buf;
    uintptr tmp_uintptr;

    // IKC does not use IDSP_CONFIG_HDR_SIZE, however, IKS does, in order to keep it on the same page between IKC and IKS,
    tmp_p_u8 = p_amalgam_data->sec2_cfg_buf;
    (void)amba_ik_system_memcpy(&sec2_cfg_buf_offset, &tmp_p_u8, sizeof(tmp_uintptr));
    tmp_p_u8 = p_amalgam_data->sec3_cfg_buf;
    (void)amba_ik_system_memcpy(&sec3_cfg_buf_offset, &tmp_p_u8, sizeof(tmp_uintptr));
    tmp_p_u8 = p_amalgam_data->sec4_cfg_buf;
    (void)amba_ik_system_memcpy(&sec4_cfg_buf_offset, &tmp_p_u8, sizeof(tmp_uintptr));
    tmp_p_u8 = p_amalgam_data->sec11_cfg_buf;
    (void)amba_ik_system_memcpy(&sec11_cfg_buf_offset, &tmp_p_u8, sizeof(tmp_uintptr));
    tmp_p_u8 = p_amalgam_data->sec18_cfg_buf;
    (void)amba_ik_system_memcpy(&sec18_cfg_buf_offset, &tmp_p_u8, sizeof(tmp_uintptr));

    filter_offset = CR_OFFSET_4;
    tmp_uintptr = sec2_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec2_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec2_cfg_buf, p_flow_tbl_list->p_CR_buf_4, CR_SIZE_4);
    p_step_cr->sec2_crs[0].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec2 cr[0] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_step_cr->sec2_crs[0].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_4), DC_U, DC_U);

    filter_offset = CR_OFFSET_5;
    tmp_uintptr = sec2_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec2_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec2_cfg_buf, p_flow_tbl_list->p_CR_buf_5, CR_SIZE_5);
    p_step_cr->sec2_crs[1].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec2 cr[1] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_step_cr->sec2_crs[1].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_5), DC_U, DC_U);

    filter_offset = CR_OFFSET_6;
    tmp_uintptr = sec2_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec2_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec2_cfg_buf, p_flow_tbl_list->p_CR_buf_6, CR_SIZE_6);
    p_step_cr->sec2_crs[2].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec2 cr[2] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_step_cr->sec2_crs[2].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_6), DC_U, DC_U);

    filter_offset = CR_OFFSET_7;
    tmp_uintptr = sec2_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec2_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec2_cfg_buf, p_flow_tbl_list->p_CR_buf_7, CR_SIZE_7);
    p_step_cr->sec2_crs[3].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec2 cr[3] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_step_cr->sec2_crs[3].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_7), DC_U, DC_U);

    filter_offset = CR_OFFSET_8;
    tmp_uintptr = sec2_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec2_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec2_cfg_buf, p_flow_tbl_list->p_CR_buf_8, CR_SIZE_8);
    p_step_cr->sec2_crs[4].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec2 cr[4] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_step_cr->sec2_crs[4].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_8), DC_U, DC_U);

    filter_offset = CR_OFFSET_9;
    tmp_uintptr = sec2_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec2_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec2_cfg_buf, p_flow_tbl_list->p_CR_buf_9, CR_SIZE_9);
    p_step_cr->sec2_crs[5].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec2 cr[5] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_step_cr->sec2_crs[5].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_9), DC_U, DC_U);

    filter_offset = CR_OFFSET_10;
    tmp_uintptr = sec2_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec2_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec2_cfg_buf, p_flow_tbl_list->p_CR_buf_10, CR_SIZE_10);
    p_step_cr->sec2_crs[6].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec2 cr[6] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_step_cr->sec2_crs[6].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_10), DC_U, DC_U);

    filter_offset = CR_OFFSET_11;
    tmp_uintptr = sec2_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec2_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec2_cfg_buf, p_flow_tbl_list->p_CR_buf_11, CR_SIZE_11);
    p_step_cr->sec2_crs[7].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec2 cr[7] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_step_cr->sec2_crs[7].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_11), DC_U, DC_U);

    filter_offset = CR_OFFSET_12;
    tmp_uintptr = sec2_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec2_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec2_cfg_buf, p_flow_tbl_list->p_CR_buf_12, CR_SIZE_12);
    p_step_cr->sec2_crs[8].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec2 cr[8] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_step_cr->sec2_crs[8].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_12), DC_U, DC_U);

    filter_offset = CR_OFFSET_13;
    tmp_uintptr = sec2_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec2_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec2_cfg_buf, p_flow_tbl_list->p_CR_buf_13, CR_SIZE_13);
    p_step_cr->sec2_crs[9].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec2 cr[9] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_step_cr->sec2_crs[9].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_13), DC_U, DC_U);

    filter_offset = CR_OFFSET_14;
    tmp_uintptr = sec2_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec2_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec2_cfg_buf, p_flow_tbl_list->p_CR_buf_14, CR_SIZE_14);
    p_step_cr->sec2_crs[10].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec2 cr[10] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_step_cr->sec2_crs[10].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_14), DC_U, DC_U);

    filter_offset = CR_OFFSET_15;
    tmp_uintptr = sec2_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec2_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec2_cfg_buf, p_flow_tbl_list->p_CR_buf_15, CR_SIZE_15);
    p_step_cr->sec2_crs[11].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec2 cr[11] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_step_cr->sec2_crs[11].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_15), DC_U, DC_U);

    filter_offset = CR_OFFSET_16;
    tmp_uintptr = sec2_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec2_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec2_cfg_buf, p_flow_tbl_list->p_CR_buf_16, CR_SIZE_16);
    p_step_cr->sec2_crs[12].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec2 cr[12] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_step_cr->sec2_crs[12].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_16), DC_U, DC_U);

    filter_offset = CR_OFFSET_17;
    tmp_uintptr = sec2_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec2_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec2_cfg_buf, p_flow_tbl_list->p_CR_buf_17, CR_SIZE_17);
    p_step_cr->sec2_crs[13].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec2 cr[13] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_step_cr->sec2_crs[13].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_17), DC_U, DC_U);

    filter_offset = CR_OFFSET_18;
    tmp_uintptr = sec2_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec2_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec2_cfg_buf, p_flow_tbl_list->p_CR_buf_18, CR_SIZE_18);
    p_step_cr->sec2_crs[14].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec2 cr[14] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_step_cr->sec2_crs[14].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_18), DC_U, DC_U);

    filter_offset = CR_OFFSET_19;
    tmp_uintptr = sec2_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec2_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec2_cfg_buf, p_flow_tbl_list->p_CR_buf_19, CR_SIZE_19);
    p_step_cr->sec2_crs[15].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec2 cr[15] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_step_cr->sec2_crs[15].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_19), DC_U, DC_U);

    filter_offset = CR_OFFSET_20;
    tmp_uintptr = sec2_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec2_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec2_cfg_buf, p_flow_tbl_list->p_CR_buf_20, CR_SIZE_20);
    p_step_cr->sec2_crs[16].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec2 cr[16] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_step_cr->sec2_crs[16].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_20), DC_U, DC_U);

    filter_offset = CR_OFFSET_21;
    tmp_uintptr = sec2_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec2_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec2_cfg_buf, p_flow_tbl_list->p_CR_buf_21, CR_SIZE_21);
    p_step_cr->sec2_crs[17].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec2 cr[17] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_step_cr->sec2_crs[17].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_21), DC_U, DC_U);

    filter_offset = CR_OFFSET_22;
    tmp_uintptr = sec2_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec2_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec2_cfg_buf, p_flow_tbl_list->p_CR_buf_22, CR_SIZE_22);
    p_step_cr->sec2_crs[18].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec2 cr[18] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_step_cr->sec2_crs[18].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_22), DC_U, DC_U);

    filter_offset = CR_OFFSET_23;
    tmp_uintptr = sec2_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec2_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec2_cfg_buf, p_flow_tbl_list->p_CR_buf_23, CR_SIZE_23);
    p_step_cr->sec2_crs[19].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec2 cr[19] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_step_cr->sec2_crs[19].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_23), DC_U, DC_U);

    filter_offset = CR_OFFSET_24;
    tmp_uintptr = sec2_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec2_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec2_cfg_buf, p_flow_tbl_list->p_CR_buf_24, CR_SIZE_24);
    p_step_cr->sec2_crs[20].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec2 cr[20] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_step_cr->sec2_crs[20].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_24), DC_U, DC_U);

    filter_offset = CR_OFFSET_25;
    tmp_uintptr = sec2_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec2_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec2_cfg_buf, p_flow_tbl_list->p_CR_buf_25, CR_SIZE_25);
    p_step_cr->sec2_crs[21].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec2 cr[21] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_step_cr->sec2_crs[21].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_25), DC_U, DC_U);

    filter_offset = CR_OFFSET_26;
    tmp_uintptr = sec2_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec2_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec2_cfg_buf, p_flow_tbl_list->p_CR_buf_26, CR_SIZE_26);
    p_step_cr->sec2_crs[22].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec2 cr[22] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_step_cr->sec2_crs[22].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_26), DC_U, DC_U);

    filter_offset = CR_OFFSET_27;
    tmp_uintptr = sec2_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec2_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec2_cfg_buf, p_flow_tbl_list->p_CR_buf_27, CR_SIZE_27);
    p_step_cr->sec2_crs[23].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec2 cr[23] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_step_cr->sec2_crs[23].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_27), DC_U, DC_U);

    filter_offset = CR_OFFSET_28;
    tmp_uintptr = sec2_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec2_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec2_cfg_buf, p_flow_tbl_list->p_CR_buf_28, CR_SIZE_28);
    p_step_cr->sec2_crs[24].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec2 cr[24] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_step_cr->sec2_crs[24].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_28), DC_U, DC_U);

    filter_offset = CR_OFFSET_29;
    tmp_uintptr = sec2_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec2_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec2_cfg_buf, p_flow_tbl_list->p_CR_buf_29, CR_SIZE_29);
    p_step_cr->sec2_crs[25].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec2 cr[25] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_step_cr->sec2_crs[25].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_29), DC_U, DC_U);

    filter_offset = CR_OFFSET_30;
    tmp_uintptr = sec2_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec2_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec2_cfg_buf, p_flow_tbl_list->p_CR_buf_30, CR_SIZE_30);
    p_step_cr->sec2_crs[26].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec2 cr[26] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_step_cr->sec2_crs[26].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_30), DC_U, DC_U);

    filter_offset = CR_OFFSET_31;
    tmp_uintptr = sec2_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec2_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec2_cfg_buf, p_flow_tbl_list->p_CR_buf_31, CR_SIZE_31);
    p_step_cr->sec2_crs[27].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec2 cr[27] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_step_cr->sec2_crs[27].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_31), DC_U, DC_U);

    filter_offset = CR_OFFSET_32;
    tmp_uintptr = sec2_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec2_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec2_cfg_buf, p_flow_tbl_list->p_CR_buf_32, CR_SIZE_32);
    p_step_cr->sec2_crs[28].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec2 cr[28] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_step_cr->sec2_crs[28].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_32), DC_U, DC_U);

    filter_offset = CR_OFFSET_33;
    tmp_uintptr = sec2_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec2_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec2_cfg_buf, p_flow_tbl_list->p_CR_buf_33, CR_SIZE_33);
    p_step_cr->sec2_crs[29].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec2 cr[29] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_step_cr->sec2_crs[29].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_33), DC_U, DC_U);

    filter_offset = CR_OFFSET_34;
    tmp_uintptr = sec2_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec2_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec2_cfg_buf, p_flow_tbl_list->p_CR_buf_34, CR_SIZE_34);
    p_step_cr->sec2_crs[30].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec2 cr[30] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_step_cr->sec2_crs[30].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_34), DC_U, DC_U);

    filter_offset = CR_OFFSET_35;
    tmp_uintptr = sec2_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec2_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec2_cfg_buf, p_flow_tbl_list->p_CR_buf_35, CR_SIZE_35);
    p_step_cr->sec2_crs[31].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec2 cr[31] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_step_cr->sec2_crs[31].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_35), DC_U, DC_U);

    filter_offset = CR_OFFSET_40;
    tmp_uintptr = sec2_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec2_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec2_cfg_buf, p_flow_tbl_list->p_CR_buf_40, CR_SIZE_40);
    p_step_cr->sec2_crs[32].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec2 cr[32] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_step_cr->sec2_crs[32].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_40), DC_U, DC_U);

    filter_offset = CR_OFFSET_36;
    tmp_uintptr = sec2_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec2_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec2_cfg_buf, p_flow_tbl_list->p_CR_buf_36, CR_SIZE_36);
    p_step_cr->sec2_crs[33].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec2 cr[33] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_step_cr->sec2_crs[33].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_36), DC_U, DC_U);

    filter_offset = CR_OFFSET_37;
    tmp_uintptr = sec2_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec2_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec2_cfg_buf, p_flow_tbl_list->p_CR_buf_37, CR_SIZE_37);
    p_step_cr->sec2_crs[34].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec2 cr[34] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_step_cr->sec2_crs[34].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_37), DC_U, DC_U);

    filter_offset = CR_OFFSET_38;
    tmp_uintptr = sec2_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec2_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec2_cfg_buf, p_flow_tbl_list->p_CR_buf_38, CR_SIZE_38);
    p_step_cr->sec2_crs[35].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec2 cr[35] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_step_cr->sec2_crs[35].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_38), DC_U, DC_U);

    filter_offset = CR_OFFSET_39;
    tmp_uintptr = sec2_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec2_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec2_cfg_buf, p_flow_tbl_list->p_CR_buf_39, CR_SIZE_39);
    p_step_cr->sec2_crs[36].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec2 cr[36] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_step_cr->sec2_crs[36].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_39), DC_U, DC_U);

    filter_offset = CR_OFFSET_41;
    tmp_uintptr = sec2_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec2_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec2_cfg_buf, p_flow_tbl_list->p_CR_buf_41, CR_SIZE_41);
    p_step_cr->sec2_crs[37].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec2 cr[37] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_step_cr->sec2_crs[37].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_41), DC_U, DC_U);

    filter_offset = CR_OFFSET_42;
    tmp_uintptr = sec3_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec3_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec3_cfg_buf, p_flow_tbl_list->p_CR_buf_42, CR_SIZE_42);
    p_step_cr->sec3_crs[0].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec3 cr[0] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_step_cr->sec3_crs[0].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_42), DC_U, DC_U);

    filter_offset = CR_OFFSET_43;
    tmp_uintptr = sec3_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec3_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec3_cfg_buf, p_flow_tbl_list->p_CR_buf_43, CR_SIZE_43);
    p_step_cr->sec3_crs[1].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec3 cr[1] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_step_cr->sec3_crs[1].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_43), DC_U, DC_U);

    filter_offset = CR_OFFSET_44;
    tmp_uintptr = sec3_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec3_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec3_cfg_buf, p_flow_tbl_list->p_CR_buf_44, CR_SIZE_44);
    p_step_cr->sec3_crs[2].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec3 cr[2] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_step_cr->sec3_crs[2].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_44), DC_U, DC_U);

    if (p_flow_tbl_list->p_CR_buf_45 != NULL) {
        filter_offset = CR_OFFSET_45;
        tmp_uintptr = sec4_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
        (void)amba_ik_system_memcpy(&p_sec4_cfg_buf, &tmp_uintptr, sizeof(void*));
        (void)amba_ik_system_memcpy(p_sec4_cfg_buf, p_flow_tbl_list->p_CR_buf_45, CR_SIZE_45);
        p_step_cr->sec4_crs[0].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
        amba_ik_system_print_uint64_5("Sec4 cr[0] addr 0x%08x, offset %d, src_addr %p ",
                                      (uint64)p_step_cr->sec4_crs[0].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_45), DC_U, DC_U);
    } else {
        amba_ik_system_print_str_5("[IK] CR_buf_45 is NULL", NULL, NULL, NULL, NULL, NULL);
    }

    if (p_flow_tbl_list->p_CR_buf_46 != NULL) {
        filter_offset = CR_OFFSET_46;
        tmp_uintptr = sec4_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
        (void)amba_ik_system_memcpy(&p_sec4_cfg_buf, &tmp_uintptr, sizeof(void*));
        (void)amba_ik_system_memcpy(p_sec4_cfg_buf, p_flow_tbl_list->p_CR_buf_46, CR_SIZE_46);
        p_step_cr->sec4_crs[1].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
        amba_ik_system_print_uint64_5("Sec4 cr[1] addr 0x%08x, offset %d, src_addr %p ",
                                      (uint64)p_step_cr->sec4_crs[1].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_46), DC_U, DC_U);
    } else {
        amba_ik_system_print_str_5("[IK] CR_buf_46 is NULL", NULL, NULL, NULL, NULL, NULL);
    }

    if (p_flow_tbl_list->p_CR_buf_47 != NULL) {
        filter_offset = CR_OFFSET_47;
        tmp_uintptr = sec4_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
        (void)amba_ik_system_memcpy(&p_sec4_cfg_buf, &tmp_uintptr, sizeof(void*));
        (void)amba_ik_system_memcpy(p_sec4_cfg_buf, p_flow_tbl_list->p_CR_buf_47, CR_SIZE_47);
        p_step_cr->sec4_crs[2].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
        amba_ik_system_print_uint64_5("Sec4 cr[2] addr 0x%08x, offset %d, src_addr %p ",
                                      (uint64)p_step_cr->sec4_crs[2].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_47), DC_U, DC_U);
    } else {
        amba_ik_system_print_str_5("[IK] CR_buf_47 is NULL", NULL, NULL, NULL, NULL, NULL);
    }

    if (p_flow_tbl_list->p_CR_buf_48 != NULL) {
        filter_offset = CR_OFFSET_48;
        tmp_uintptr = sec4_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
        (void)amba_ik_system_memcpy(&p_sec4_cfg_buf, &tmp_uintptr, sizeof(void*));
        (void)amba_ik_system_memcpy(p_sec4_cfg_buf, p_flow_tbl_list->p_CR_buf_48, CR_SIZE_48);
        p_step_cr->sec4_crs[3].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
        amba_ik_system_print_uint64_5("Sec4 cr[3] addr 0x%08x, offset %d, src_addr %p ",
                                      (uint64)p_step_cr->sec4_crs[3].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_48), DC_U, DC_U);
    } else {
        amba_ik_system_print_str_5("[IK] CR_buf_48 is NULL", NULL, NULL, NULL, NULL, NULL);
    }

    if (p_flow_tbl_list->p_CR_buf_49 != NULL) {
        filter_offset = CR_OFFSET_49;
        tmp_uintptr = sec4_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
        (void)amba_ik_system_memcpy(&p_sec4_cfg_buf, &tmp_uintptr, sizeof(void*));
        (void)amba_ik_system_memcpy(p_sec4_cfg_buf, p_flow_tbl_list->p_CR_buf_49, CR_SIZE_49);
        p_step_cr->sec4_crs[4].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
        amba_ik_system_print_uint64_5("Sec4 cr[4] addr 0x%08x, offset %d, src_addr %p ",
                                      (uint64)p_step_cr->sec4_crs[4].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_49), DC_U, DC_U);
    } else {
        amba_ik_system_print_str_5("[IK] CR_buf_49 is NULL", NULL, NULL, NULL, NULL, NULL);
    }

    if (p_flow_tbl_list->p_CR_buf_50 != NULL) {
        filter_offset = CR_OFFSET_50;
        tmp_uintptr = sec4_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
        (void)amba_ik_system_memcpy(&p_sec4_cfg_buf, &tmp_uintptr, sizeof(void*));
        (void)amba_ik_system_memcpy(p_sec4_cfg_buf, p_flow_tbl_list->p_CR_buf_50, CR_SIZE_50);
        p_step_cr->sec4_crs[5].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
        amba_ik_system_print_uint64_5("Sec4 cr[5] addr 0x%08x, offset %d, src_addr %p ",
                                      (uint64)p_step_cr->sec4_crs[5].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_50), DC_U, DC_U);
    } else {
        amba_ik_system_print_str_5("[IK] CR_buf_50 is NULL", NULL, NULL, NULL, NULL, NULL);
    }

    if (p_flow_tbl_list->p_CR_buf_51 != NULL) {
        filter_offset = CR_OFFSET_51;
        tmp_uintptr = sec4_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
        (void)amba_ik_system_memcpy(&p_sec4_cfg_buf, &tmp_uintptr, sizeof(void*));
        (void)amba_ik_system_memcpy(p_sec4_cfg_buf, p_flow_tbl_list->p_CR_buf_51, CR_SIZE_51);
        p_step_cr->sec4_crs[6].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
        amba_ik_system_print_uint64_5("Sec4 cr[6] addr 0x%08x, offset %d, src_addr %p ",
                                      (uint64)p_step_cr->sec4_crs[6].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_51), DC_U, DC_U);
    } else {
        amba_ik_system_print_str_5("[IK] CR_buf_51 is NULL", NULL, NULL, NULL, NULL, NULL);
    }

    if (p_flow_tbl_list->p_CR_buf_52 != NULL) {
        filter_offset = CR_OFFSET_52;
        tmp_uintptr = sec4_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
        (void)amba_ik_system_memcpy(&p_sec4_cfg_buf, &tmp_uintptr, sizeof(void*));
        (void)amba_ik_system_memcpy(p_sec4_cfg_buf, p_flow_tbl_list->p_CR_buf_52, CR_SIZE_52);
        p_step_cr->sec4_crs[7].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
        amba_ik_system_print_uint64_5("Sec4 cr[7] addr 0x%08x, offset %d, src_addr %p ",
                                      (uint64)p_step_cr->sec4_crs[7].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_52), DC_U, DC_U);
    } else {
        amba_ik_system_print_str_5("[IK] CR_buf_52 is NULL", NULL, NULL, NULL, NULL, NULL);
    }

    if (p_flow_tbl_list->p_CR_buf_100 != NULL) {
        filter_offset = CR_OFFSET_100;
        tmp_uintptr = sec11_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
        (void)amba_ik_system_memcpy(&p_sec11_cfg_buf, &tmp_uintptr, sizeof(void*));
        (void)amba_ik_system_memcpy(p_sec11_cfg_buf, p_flow_tbl_list->p_CR_buf_100, CR_SIZE_100);
        p_step_cr->sec11_crs[0].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
        amba_ik_system_print_uint64_5("Sec11 cr[0] addr 0x%08x, offset %d, src_addr %p ",
                                      (uint64)p_step_cr->sec11_crs[0].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_100), DC_U, DC_U);
    } else {
        amba_ik_system_print_str_5("[IK] CR_buf_100 is NULL", NULL, NULL, NULL, NULL, NULL);
    }

    if (p_flow_tbl_list->p_CR_buf_101 != NULL) {
        filter_offset = CR_OFFSET_101;
        tmp_uintptr = sec11_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
        (void)amba_ik_system_memcpy(&p_sec11_cfg_buf, &tmp_uintptr, sizeof(void*));
        (void)amba_ik_system_memcpy(p_sec11_cfg_buf, p_flow_tbl_list->p_CR_buf_101, CR_SIZE_101);
        p_step_cr->sec11_crs[1].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
        amba_ik_system_print_uint64_5("Sec11 cr[1] addr 0x%08x, offset %d, src_addr %p ",
                                     (uint64)p_step_cr->sec11_crs[1].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_101), DC_U, DC_U);
    } else {
        amba_ik_system_print_str_5("[IK] CR_buf_101 is NULL", NULL, NULL, NULL, NULL, NULL);
    }

    if (p_flow_tbl_list->p_CR_buf_102 != NULL) {
        filter_offset = CR_OFFSET_102;
        tmp_uintptr = sec11_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
        (void)amba_ik_system_memcpy(&p_sec11_cfg_buf, &tmp_uintptr, sizeof(void*));
        (void)amba_ik_system_memcpy(p_sec11_cfg_buf, p_flow_tbl_list->p_CR_buf_102, CR_SIZE_102);
        p_step_cr->sec11_crs[2].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
        amba_ik_system_print_uint64_5("Sec11 cr[2] addr 0x%08x, offset %d, src_addr %p ",
                                      (uint64)p_step_cr->sec11_crs[2].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_102), DC_U, DC_U);
    } else {
        amba_ik_system_print_str_5("[IK] CR_buf_102 is NULL", NULL, NULL, NULL, NULL, NULL);
    }

    if (p_flow_tbl_list->p_CR_buf_103 != NULL) {
        filter_offset = CR_OFFSET_103;
        tmp_uintptr = sec11_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
        (void)amba_ik_system_memcpy(&p_sec11_cfg_buf, &tmp_uintptr, sizeof(void*));
        (void)amba_ik_system_memcpy(p_sec11_cfg_buf, p_flow_tbl_list->p_CR_buf_103, CR_SIZE_103);
        p_step_cr->sec11_crs[3].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
        amba_ik_system_print_uint64_5("Sec11 cr[3] addr 0x%08x, offset %d, src_addr %p ",
                                      (uint64)p_step_cr->sec11_crs[3].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_103), DC_U, DC_U);
    } else {
        amba_ik_system_print_str_5("[IK] CR_buf_100 is NULL", NULL, NULL, NULL, NULL, NULL);
    }

    filter_offset = CR_OFFSET_111;
    tmp_uintptr = sec18_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec18_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec18_cfg_buf, p_flow_tbl_list->p_CR_buf_111, CR_SIZE_111);
    p_step_cr->sec18_crs[0].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec18 cr[0] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_step_cr->sec18_crs[0].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_111), DC_U, DC_U);

    filter_offset = CR_OFFSET_112;
    tmp_uintptr = sec18_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec18_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec18_cfg_buf, p_flow_tbl_list->p_CR_buf_112, CR_SIZE_112);
    p_step_cr->sec18_crs[1].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec18 cr[1] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_step_cr->sec18_crs[1].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_112), DC_U, DC_U);

    filter_offset = CR_OFFSET_113;
    tmp_uintptr = sec18_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec18_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec18_cfg_buf, p_flow_tbl_list->p_CR_buf_113, CR_SIZE_113);
    p_step_cr->sec18_crs[2].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec18 cr[2] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_step_cr->sec18_crs[2].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_113), DC_U, DC_U);

    filter_offset = CR_OFFSET_114;
    tmp_uintptr = sec18_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec18_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec18_cfg_buf, p_flow_tbl_list->p_CR_buf_114, CR_SIZE_114);
    p_step_cr->sec18_crs[3].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec18 cr[3] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_step_cr->sec18_crs[3].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_114), DC_U, DC_U);

    filter_offset = CR_OFFSET_115;
    tmp_uintptr = sec18_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec18_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec18_cfg_buf, p_flow_tbl_list->p_CR_buf_115, CR_SIZE_115);
    p_step_cr->sec18_crs[4].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec18 cr[4] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_step_cr->sec18_crs[4].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_115), DC_U, DC_U);

    filter_offset = CR_OFFSET_116;
    tmp_uintptr = sec18_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec18_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec18_cfg_buf, p_flow_tbl_list->p_CR_buf_116, CR_SIZE_116);
    p_step_cr->sec18_crs[5].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec18 cr[5] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_step_cr->sec18_crs[5].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_116), DC_U, DC_U);

    filter_offset = CR_OFFSET_117;
    tmp_uintptr = sec18_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec18_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec18_cfg_buf, p_flow_tbl_list->p_CR_buf_117, CR_SIZE_117);
    p_step_cr->sec18_crs[6].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec18 cr[6] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_step_cr->sec18_crs[6].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_117), DC_U, DC_U);

    filter_offset = CR_OFFSET_118;
    tmp_uintptr = sec18_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec18_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec18_cfg_buf, p_flow_tbl_list->p_CR_buf_118, CR_SIZE_118);
    p_step_cr->sec18_crs[7].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec18 cr[7] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_step_cr->sec18_crs[7].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_118), DC_U, DC_U);

    filter_offset = CR_OFFSET_119;
    tmp_uintptr = sec18_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec18_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec18_cfg_buf, p_flow_tbl_list->p_CR_buf_119, CR_SIZE_119);
    p_step_cr->sec18_crs[8].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec18 cr[8] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_step_cr->sec18_crs[8].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_119), DC_U, DC_U);

    filter_offset = CR_OFFSET_120;
    tmp_uintptr = sec18_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec18_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec18_cfg_buf, p_flow_tbl_list->p_CR_buf_120, CR_SIZE_120);
    p_step_cr->sec18_crs[9].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec18 cr[9] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_step_cr->sec18_crs[9].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_120), DC_U, DC_U);

    filter_offset = CR_OFFSET_121;
    tmp_uintptr = sec18_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec18_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec18_cfg_buf, p_flow_tbl_list->p_CR_buf_121, CR_SIZE_121);
    p_step_cr->sec18_crs[10].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec18 cr[10] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_step_cr->sec18_crs[10].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_121), DC_U, DC_U);

    return rval;
}

static uint32 img_cfg_mono3_amalgam_data_update(uintptr data_offset_base,
        idsp_step_crs_t *p_step_cr,
        const amalgam_mono3_flow_data_t *p_amalgam_data,
        const amba_ik_mono3_flow_tables_list_t *p_flow_tbl_list)
{
    uint32 rval = 0;
    uint32_t filter_offset;
    const uint8_t *tmp_p_u8;
    uintptr sec2_cfg_buf_offset;
    uintptr sec18_cfg_buf_offset;
    void* p_sec2_cfg_buf;
    void* p_sec18_cfg_buf;
    uintptr tmp_uintptr;
    uint32 i;

    // IKC does not use IDSP_CONFIG_HDR_SIZE, however, IKS does, in order to keep it on the same page between IKC and IKS,
    tmp_p_u8 = p_amalgam_data->sec2_cfg_buf;
    (void)amba_ik_system_memcpy(&sec2_cfg_buf_offset, &tmp_p_u8, sizeof(tmp_uintptr));
    tmp_p_u8 = p_amalgam_data->sec18_cfg_buf;
    (void)amba_ik_system_memcpy(&sec18_cfg_buf_offset, &tmp_p_u8, sizeof(tmp_uintptr));

    filter_offset = MODE_8_SEC_2_CR_OFFSET_26;
    tmp_uintptr = sec2_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec2_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec2_cfg_buf, p_flow_tbl_list->p_CR_buf_26, CR_SIZE_26);
    p_step_cr->sec2_crs[22].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec2 cr[22] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_step_cr->sec2_crs[22].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_26), DC_U, DC_U);

    filter_offset = MODE_8_SEC_2_CR_OFFSET_27;
    tmp_uintptr = sec2_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec2_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec2_cfg_buf, p_flow_tbl_list->p_CR_buf_27, CR_SIZE_27);
    p_step_cr->sec2_crs[23].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec2 cr[23] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_step_cr->sec2_crs[23].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_27), DC_U, DC_U);

    filter_offset = MODE_8_SEC_2_CR_OFFSET_29;
    tmp_uintptr = sec2_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec2_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec2_cfg_buf, p_flow_tbl_list->p_CR_buf_29, CR_SIZE_29);
    p_step_cr->sec2_crs[25].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec2 cr[25] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_step_cr->sec2_crs[25].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_29), DC_U, DC_U);

    filter_offset = MODE_8_SEC_2_CR_OFFSET_30;
    tmp_uintptr = sec2_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec2_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec2_cfg_buf, p_flow_tbl_list->p_CR_buf_30, CR_SIZE_30);
    p_step_cr->sec2_crs[26].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec2 cr[26] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_step_cr->sec2_crs[26].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_30), DC_U, DC_U);

    filter_offset = MODE_8_SEC_2_CR_OFFSET_31;
    tmp_uintptr = sec2_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec2_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec2_cfg_buf, p_flow_tbl_list->p_CR_buf_31, CR_SIZE_31);
    p_step_cr->sec2_crs[27].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec2 cr[27] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_step_cr->sec2_crs[27].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_31), DC_U, DC_U);

    filter_offset = MODE_8_SEC_2_CR_OFFSET_32;
    tmp_uintptr = sec2_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec2_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec2_cfg_buf, p_flow_tbl_list->p_CR_buf_32, CR_SIZE_32);
    p_step_cr->sec2_crs[28].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec2 cr[28] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_step_cr->sec2_crs[28].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_32), DC_U, DC_U);

    filter_offset = MODE_8_SEC_2_CR_OFFSET_33;
    tmp_uintptr = sec2_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec2_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec2_cfg_buf, p_flow_tbl_list->p_CR_buf_33, CR_SIZE_33);
    p_step_cr->sec2_crs[29].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec2 cr[29] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_step_cr->sec2_crs[29].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_33), DC_U, DC_U);

    filter_offset = MODE_8_SEC_2_CR_OFFSET_34;
    tmp_uintptr = sec2_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec2_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec2_cfg_buf, p_flow_tbl_list->p_CR_buf_34, CR_SIZE_34);
    p_step_cr->sec2_crs[30].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec2 cr[30] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_step_cr->sec2_crs[30].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_34), DC_U, DC_U);

    filter_offset = MODE_8_SEC_2_CR_OFFSET_35;
    tmp_uintptr = sec2_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec2_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec2_cfg_buf, p_flow_tbl_list->p_CR_buf_35, CR_SIZE_35);
    p_step_cr->sec2_crs[31].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec2 cr[31] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_step_cr->sec2_crs[31].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_35), DC_U, DC_U);

    filter_offset = MODE_8_SEC_2_CR_OFFSET_40;
    tmp_uintptr = sec2_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec2_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec2_cfg_buf, p_flow_tbl_list->p_CR_buf_40, CR_SIZE_40);
    p_step_cr->sec2_crs[32].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec2 cr[32] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_step_cr->sec2_crs[32].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_40), DC_U, DC_U);

    filter_offset = MODE_8_SEC_2_CR_OFFSET_36;
    tmp_uintptr = sec2_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec2_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec2_cfg_buf, p_flow_tbl_list->p_CR_buf_36, CR_SIZE_36);
    p_step_cr->sec2_crs[33].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec2 cr[33] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_step_cr->sec2_crs[33].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_36), DC_U, DC_U);

    filter_offset = MODE_8_SEC_2_CR_OFFSET_37;
    tmp_uintptr = sec2_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec2_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec2_cfg_buf, p_flow_tbl_list->p_CR_buf_37, CR_SIZE_37);
    p_step_cr->sec2_crs[34].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec2 cr[34] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_step_cr->sec2_crs[34].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_37), DC_U, DC_U);

    filter_offset = MODE_8_SEC_2_CR_OFFSET_38;
    tmp_uintptr = sec2_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec2_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec2_cfg_buf, p_flow_tbl_list->p_CR_buf_38, CR_SIZE_38);
    p_step_cr->sec2_crs[35].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec2 cr[35] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_step_cr->sec2_crs[35].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_38), DC_U, DC_U);

    filter_offset = MODE_8_SEC_2_CR_OFFSET_39;
    tmp_uintptr = sec2_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec2_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec2_cfg_buf, p_flow_tbl_list->p_CR_buf_39, CR_SIZE_39);
    p_step_cr->sec2_crs[36].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec2 cr[36] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_step_cr->sec2_crs[36].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_39), DC_U, DC_U);

    filter_offset = MODE_8_SEC_2_CR_OFFSET_41;
    tmp_uintptr = sec2_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec2_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec2_cfg_buf, p_flow_tbl_list->p_CR_buf_41, CR_SIZE_41);
    p_step_cr->sec2_crs[37].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec2 cr[37] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_step_cr->sec2_crs[37].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_41), DC_U, DC_U);

    filter_offset = MODE_4_SEC_18_CR_OFFSET_112;
    tmp_uintptr = sec18_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec18_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec18_cfg_buf, p_flow_tbl_list->p_CR_buf_112, CR_SIZE_112);
    p_step_cr->sec18_crs[1].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec18 cr[1] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_step_cr->sec18_crs[1].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_112), DC_U, DC_U);

    filter_offset = MODE_4_SEC_18_CR_OFFSET_113;
    tmp_uintptr = sec18_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec18_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec18_cfg_buf, p_flow_tbl_list->p_CR_buf_113, CR_SIZE_113);
    p_step_cr->sec18_crs[2].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint32_5("Sec18 cr[2] addr 0x%08x, offset %d, src_addr 0x%08x ",
                                  p_step_cr->sec18_crs[2].config_region_addr << 7, filter_offset, DC_U, DC_U, DC_U);

    filter_offset = MODE_4_SEC_18_CR_OFFSET_114;
    tmp_uintptr = sec18_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec18_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec18_cfg_buf, p_flow_tbl_list->p_CR_buf_114, CR_SIZE_114);
    p_step_cr->sec18_crs[3].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint32_5("Sec18 cr[3] addr 0x%08x, offset %d, src_addr 0x%08x ",
                                  p_step_cr->sec18_crs[3].config_region_addr << 7, filter_offset, DC_U, DC_U, DC_U);

    filter_offset = MODE_4_SEC_18_CR_OFFSET_115;
    tmp_uintptr = sec18_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec18_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec18_cfg_buf, p_flow_tbl_list->p_CR_buf_115, CR_SIZE_115);
    p_step_cr->sec18_crs[4].config_region_addr = (uint32_t)(tmp_uintptr - data_offset_base) >> 7U;
    amba_ik_system_print_uint32_5("Sec18 cr[4] addr 0x%08x, offset %d, src_addr 0x%08x ",
                                  p_step_cr->sec18_crs[4].config_region_addr << 7, filter_offset, DC_U, DC_U, DC_U);

    filter_offset = MODE_4_SEC_18_CR_OFFSET_117;
    tmp_uintptr = sec18_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec18_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec18_cfg_buf, p_flow_tbl_list->p_CR_buf_117, CR_SIZE_117);
    p_step_cr->sec18_crs[6].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec18 cr[6] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_step_cr->sec18_crs[6].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_117), DC_U, DC_U);

    filter_offset = MODE_4_SEC_18_CR_OFFSET_118;
    tmp_uintptr = sec18_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec18_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec18_cfg_buf, p_flow_tbl_list->p_CR_buf_118, CR_SIZE_118);
    p_step_cr->sec18_crs[7].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec18 cr[7] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_step_cr->sec18_crs[7].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_118), DC_U, DC_U);

    filter_offset = MODE_4_SEC_18_CR_OFFSET_119;
    tmp_uintptr = sec18_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec18_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec18_cfg_buf, p_flow_tbl_list->p_CR_buf_119, CR_SIZE_119);
    p_step_cr->sec18_crs[8].config_region_addr = (uint32_t)(tmp_uintptr - data_offset_base) >> 7U;
    amba_ik_system_print_uint32_5("Sec18 cr[8] addr 0x%08x, offset %d, src_addr 0x%08x ",
                                  p_step_cr->sec18_crs[8].config_region_addr << 7, filter_offset, DC_U, DC_U, DC_U);

    {
        for (i=0U; i<22U; i++) {
            p_step_cr->sec2_crs[i].config_region_addr = 0;
        }
        p_step_cr->sec2_crs[24].config_region_addr = 0;

        p_step_cr->sec18_crs[0].config_region_addr = 0;
        p_step_cr->sec18_crs[5].config_region_addr = 0;
        p_step_cr->sec18_crs[9].config_region_addr = 0;
        p_step_cr->sec18_crs[10].config_region_addr = 0;
    }

    return rval;
}

static uint32 img_cfg_mono4_amalgam_data_update(uintptr data_offset_base,
        idsp_step_crs_t *p_step_cr,
        const amalgam_mono4_flow_data_t *p_amalgam_data,
        const amba_ik_mono4_flow_tables_list_t *p_flow_tbl_list)
{
    uint32 rval = 0;
    uint32_t filter_offset;
    const uint8_t *tmp_p_u8;
    uintptr sec2_cfg_buf_offset;
    void* p_sec2_cfg_buf;
    uintptr tmp_uintptr;
    uint32 i;

    // IKC does not use IDSP_CONFIG_HDR_SIZE, however, IKS does, in order to keep it on the same page between IKC and IKS,
    tmp_p_u8 = p_amalgam_data->sec2_cfg_buf;
    (void)amba_ik_system_memcpy(&sec2_cfg_buf_offset, &tmp_p_u8, sizeof(tmp_uintptr));

    filter_offset = MODE_8_SEC_2_CR_OFFSET_26;
    tmp_uintptr = sec2_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec2_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec2_cfg_buf, p_flow_tbl_list->p_CR_buf_26, CR_SIZE_26);
    p_step_cr->sec2_crs[22].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec2 cr[22] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_step_cr->sec2_crs[22].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_26), DC_U, DC_U);

    filter_offset = MODE_8_SEC_2_CR_OFFSET_27;
    tmp_uintptr = sec2_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec2_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec2_cfg_buf, p_flow_tbl_list->p_CR_buf_27, CR_SIZE_27);
    p_step_cr->sec2_crs[23].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec2 cr[23] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_step_cr->sec2_crs[23].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_27), DC_U, DC_U);

    filter_offset = MODE_8_SEC_2_CR_OFFSET_29;
    tmp_uintptr = sec2_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec2_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec2_cfg_buf, p_flow_tbl_list->p_CR_buf_29, CR_SIZE_29);
    p_step_cr->sec2_crs[25].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec2 cr[25] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_step_cr->sec2_crs[25].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_29), DC_U, DC_U);

    filter_offset = MODE_8_SEC_2_CR_OFFSET_30;
    tmp_uintptr = sec2_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec2_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec2_cfg_buf, p_flow_tbl_list->p_CR_buf_30, CR_SIZE_30);
    p_step_cr->sec2_crs[26].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec2 cr[26] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_step_cr->sec2_crs[26].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_30), DC_U, DC_U);

    filter_offset = MODE_8_SEC_2_CR_OFFSET_31;
    tmp_uintptr = sec2_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec2_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec2_cfg_buf, p_flow_tbl_list->p_CR_buf_31, CR_SIZE_31);
    p_step_cr->sec2_crs[27].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec2 cr[27] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_step_cr->sec2_crs[27].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_31), DC_U, DC_U);

    filter_offset = MODE_8_SEC_2_CR_OFFSET_32;
    tmp_uintptr = sec2_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec2_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec2_cfg_buf, p_flow_tbl_list->p_CR_buf_32, CR_SIZE_32);
    p_step_cr->sec2_crs[28].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec2 cr[28] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_step_cr->sec2_crs[28].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_32), DC_U, DC_U);

    filter_offset = MODE_8_SEC_2_CR_OFFSET_33;
    tmp_uintptr = sec2_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec2_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec2_cfg_buf, p_flow_tbl_list->p_CR_buf_33, CR_SIZE_33);
    p_step_cr->sec2_crs[29].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec2 cr[29] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_step_cr->sec2_crs[29].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_33), DC_U, DC_U);

    filter_offset = MODE_8_SEC_2_CR_OFFSET_34;
    tmp_uintptr = sec2_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec2_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec2_cfg_buf, p_flow_tbl_list->p_CR_buf_34, CR_SIZE_34);
    p_step_cr->sec2_crs[30].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec2 cr[30] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_step_cr->sec2_crs[30].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_34), DC_U, DC_U);

    filter_offset = MODE_8_SEC_2_CR_OFFSET_35;
    tmp_uintptr = sec2_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec2_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec2_cfg_buf, p_flow_tbl_list->p_CR_buf_35, CR_SIZE_35);
    p_step_cr->sec2_crs[31].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec2 cr[31] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_step_cr->sec2_crs[31].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_35), DC_U, DC_U);

    filter_offset = MODE_8_SEC_2_CR_OFFSET_40;
    tmp_uintptr = sec2_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec2_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec2_cfg_buf, p_flow_tbl_list->p_CR_buf_40, CR_SIZE_40);
    p_step_cr->sec2_crs[32].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec2 cr[32] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_step_cr->sec2_crs[32].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_40), DC_U, DC_U);

    filter_offset = MODE_8_SEC_2_CR_OFFSET_36;
    tmp_uintptr = sec2_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec2_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec2_cfg_buf, p_flow_tbl_list->p_CR_buf_36, CR_SIZE_36);
    p_step_cr->sec2_crs[33].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec2 cr[33] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_step_cr->sec2_crs[33].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_36), DC_U, DC_U);

    filter_offset = MODE_8_SEC_2_CR_OFFSET_37;
    tmp_uintptr = sec2_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec2_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec2_cfg_buf, p_flow_tbl_list->p_CR_buf_37, CR_SIZE_37);
    p_step_cr->sec2_crs[34].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec2 cr[34] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_step_cr->sec2_crs[34].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_37), DC_U, DC_U);

    filter_offset = MODE_8_SEC_2_CR_OFFSET_38;
    tmp_uintptr = sec2_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec2_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec2_cfg_buf, p_flow_tbl_list->p_CR_buf_38, CR_SIZE_38);
    p_step_cr->sec2_crs[35].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec2 cr[35] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_step_cr->sec2_crs[35].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_38), DC_U, DC_U);

    filter_offset = MODE_8_SEC_2_CR_OFFSET_39;
    tmp_uintptr = sec2_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec2_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec2_cfg_buf, p_flow_tbl_list->p_CR_buf_39, CR_SIZE_39);
    p_step_cr->sec2_crs[36].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec2 cr[36] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_step_cr->sec2_crs[36].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_39), DC_U, DC_U);

    filter_offset = MODE_8_SEC_2_CR_OFFSET_41;
    tmp_uintptr = sec2_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec2_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec2_cfg_buf, p_flow_tbl_list->p_CR_buf_41, CR_SIZE_41);
    p_step_cr->sec2_crs[37].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec2 cr[37] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_step_cr->sec2_crs[37].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_41), DC_U, DC_U);

    {
        for (i=0U; i<22U; i++) {
            p_step_cr->sec2_crs[i].config_region_addr = 0;
        }
        p_step_cr->sec2_crs[24].config_region_addr = 0;
    }

    return rval;
}

static uint32 img_cfg_mono5_amalgam_data_update(uintptr data_offset_base,
        idsp_step_crs_t *p_step_cr,
        const amalgam_mono5_flow_data_t *p_amalgam_data,
        const amba_ik_mono5_flow_tables_list_t *p_flow_tbl_list)
{
    uint32 rval = 0;
    uint32_t filter_offset;
    const uint8_t *tmp_p_u8;
    uintptr sec2_cfg_buf_offset;
    void* p_sec2_cfg_buf;
    uintptr tmp_uintptr;
    uint32 i;

    // IKC does not use IDSP_CONFIG_HDR_SIZE, however, IKS does, in order to keep it on the same page between IKC and IKS,
    tmp_p_u8 = p_amalgam_data->sec2_cfg_buf;
    (void)amba_ik_system_memcpy(&sec2_cfg_buf_offset, &tmp_p_u8, sizeof(tmp_uintptr));

    filter_offset = MODE_8_SEC_2_CR_OFFSET_26;
    tmp_uintptr = sec2_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec2_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec2_cfg_buf, p_flow_tbl_list->p_CR_buf_26, CR_SIZE_26);
    p_step_cr->sec2_crs[22].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec2 cr[22] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_step_cr->sec2_crs[22].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_26), DC_U, DC_U);

    filter_offset = MODE_8_SEC_2_CR_OFFSET_27;
    tmp_uintptr = sec2_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec2_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec2_cfg_buf, p_flow_tbl_list->p_CR_buf_27, CR_SIZE_27);
    p_step_cr->sec2_crs[23].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec2 cr[23] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_step_cr->sec2_crs[23].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_27), DC_U, DC_U);

    filter_offset = MODE_8_SEC_2_CR_OFFSET_29;
    tmp_uintptr = sec2_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec2_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec2_cfg_buf, p_flow_tbl_list->p_CR_buf_29, CR_SIZE_29);
    p_step_cr->sec2_crs[25].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec2 cr[25] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_step_cr->sec2_crs[25].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_29), DC_U, DC_U);

    filter_offset = MODE_8_SEC_2_CR_OFFSET_30;
    tmp_uintptr = sec2_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec2_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec2_cfg_buf, p_flow_tbl_list->p_CR_buf_30, CR_SIZE_30);
    p_step_cr->sec2_crs[26].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec2 cr[26] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_step_cr->sec2_crs[26].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_30), DC_U, DC_U);

    filter_offset = MODE_8_SEC_2_CR_OFFSET_31;
    tmp_uintptr = sec2_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec2_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec2_cfg_buf, p_flow_tbl_list->p_CR_buf_31, CR_SIZE_31);
    p_step_cr->sec2_crs[27].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec2 cr[27] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_step_cr->sec2_crs[27].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_31), DC_U, DC_U);

    filter_offset = MODE_8_SEC_2_CR_OFFSET_32;
    tmp_uintptr = sec2_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec2_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec2_cfg_buf, p_flow_tbl_list->p_CR_buf_32, CR_SIZE_32);
    p_step_cr->sec2_crs[28].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec2 cr[28] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_step_cr->sec2_crs[28].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_32), DC_U, DC_U);

    filter_offset = MODE_8_SEC_2_CR_OFFSET_33;
    tmp_uintptr = sec2_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec2_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec2_cfg_buf, p_flow_tbl_list->p_CR_buf_33, CR_SIZE_33);
    p_step_cr->sec2_crs[29].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec2 cr[29] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_step_cr->sec2_crs[29].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_33), DC_U, DC_U);

    filter_offset = MODE_8_SEC_2_CR_OFFSET_34;
    tmp_uintptr = sec2_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec2_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec2_cfg_buf, p_flow_tbl_list->p_CR_buf_34, CR_SIZE_34);
    p_step_cr->sec2_crs[30].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec2 cr[30] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_step_cr->sec2_crs[30].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_34), DC_U, DC_U);

    filter_offset = MODE_8_SEC_2_CR_OFFSET_35;
    tmp_uintptr = sec2_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec2_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec2_cfg_buf, p_flow_tbl_list->p_CR_buf_35, CR_SIZE_35);
    p_step_cr->sec2_crs[31].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec2 cr[31] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_step_cr->sec2_crs[31].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_35), DC_U, DC_U);

    filter_offset = MODE_8_SEC_2_CR_OFFSET_40;
    tmp_uintptr = sec2_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec2_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec2_cfg_buf, p_flow_tbl_list->p_CR_buf_40, CR_SIZE_40);
    p_step_cr->sec2_crs[32].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec2 cr[32] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_step_cr->sec2_crs[32].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_40), DC_U, DC_U);

    filter_offset = MODE_8_SEC_2_CR_OFFSET_36;
    tmp_uintptr = sec2_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec2_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec2_cfg_buf, p_flow_tbl_list->p_CR_buf_36, CR_SIZE_36);
    p_step_cr->sec2_crs[33].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec2 cr[33] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_step_cr->sec2_crs[33].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_36), DC_U, DC_U);

    filter_offset = MODE_8_SEC_2_CR_OFFSET_37;
    tmp_uintptr = sec2_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec2_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec2_cfg_buf, p_flow_tbl_list->p_CR_buf_37, CR_SIZE_37);
    p_step_cr->sec2_crs[34].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec2 cr[34] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_step_cr->sec2_crs[34].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_37), DC_U, DC_U);

    filter_offset = MODE_8_SEC_2_CR_OFFSET_38;
    tmp_uintptr = sec2_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec2_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec2_cfg_buf, p_flow_tbl_list->p_CR_buf_38, CR_SIZE_38);
    p_step_cr->sec2_crs[35].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec2 cr[35] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_step_cr->sec2_crs[35].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_38), DC_U, DC_U);

    filter_offset = MODE_8_SEC_2_CR_OFFSET_39;
    tmp_uintptr = sec2_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec2_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec2_cfg_buf, p_flow_tbl_list->p_CR_buf_39, CR_SIZE_39);
    p_step_cr->sec2_crs[36].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec2 cr[36] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_step_cr->sec2_crs[36].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_39), DC_U, DC_U);

    filter_offset = MODE_8_SEC_2_CR_OFFSET_41;
    tmp_uintptr = sec2_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec2_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec2_cfg_buf, p_flow_tbl_list->p_CR_buf_41, CR_SIZE_41);
    p_step_cr->sec2_crs[37].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec2 cr[37] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_step_cr->sec2_crs[37].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_41), DC_U, DC_U);

    {
        for (i=0U; i<22U; i++) {
            p_step_cr->sec2_crs[i].config_region_addr = 0;
        }
        p_step_cr->sec2_crs[24].config_region_addr = 0;
    }

    return rval;
}

static uint32 img_cfg_mono6_amalgam_data_update(uintptr data_offset_base,
        idsp_step_crs_t *p_step_cr,
        const amalgam_mono6_flow_data_t *p_amalgam_data,
        const amba_ik_mono6_flow_tables_list_t *p_flow_tbl_list)
{
    uint32 rval = 0;
    uint32_t filter_offset;
    const uint8_t *tmp_p_u8;
    uintptr sec2_cfg_buf_offset;
    uintptr sec18_cfg_buf_offset;
    void* p_sec2_cfg_buf;
    void* p_sec18_cfg_buf;
    uintptr tmp_uintptr;
    uint32 i;

    // IKC does not use IDSP_CONFIG_HDR_SIZE, however, IKS does, in order to keep it on the same page between IKC and IKS,
    tmp_p_u8 = p_amalgam_data->sec2_cfg_buf;
    (void)amba_ik_system_memcpy(&sec2_cfg_buf_offset, &tmp_p_u8, sizeof(tmp_uintptr));
    tmp_p_u8 = p_amalgam_data->sec18_cfg_buf;
    (void)amba_ik_system_memcpy(&sec18_cfg_buf_offset, &tmp_p_u8, sizeof(tmp_uintptr));

    filter_offset = MODE_8_SEC_2_CR_OFFSET_26;
    tmp_uintptr = sec2_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec2_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec2_cfg_buf, p_flow_tbl_list->p_CR_buf_26, CR_SIZE_26);
    p_step_cr->sec2_crs[22].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec2 cr[22] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_step_cr->sec2_crs[22].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_26), DC_U, DC_U);

    filter_offset = MODE_8_SEC_2_CR_OFFSET_27;
    tmp_uintptr = sec2_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec2_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec2_cfg_buf, p_flow_tbl_list->p_CR_buf_27, CR_SIZE_27);
    p_step_cr->sec2_crs[23].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec2 cr[23] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_step_cr->sec2_crs[23].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_27), DC_U, DC_U);

    filter_offset = MODE_8_SEC_2_CR_OFFSET_29;
    tmp_uintptr = sec2_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec2_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec2_cfg_buf, p_flow_tbl_list->p_CR_buf_29, CR_SIZE_29);
    p_step_cr->sec2_crs[25].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec2 cr[25] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_step_cr->sec2_crs[25].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_29), DC_U, DC_U);

    filter_offset = MODE_8_SEC_2_CR_OFFSET_30;
    tmp_uintptr = sec2_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec2_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec2_cfg_buf, p_flow_tbl_list->p_CR_buf_30, CR_SIZE_30);
    p_step_cr->sec2_crs[26].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec2 cr[26] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_step_cr->sec2_crs[26].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_30), DC_U, DC_U);

    filter_offset = MODE_8_SEC_2_CR_OFFSET_31;
    tmp_uintptr = sec2_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec2_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec2_cfg_buf, p_flow_tbl_list->p_CR_buf_31, CR_SIZE_31);
    p_step_cr->sec2_crs[27].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec2 cr[27] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_step_cr->sec2_crs[27].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_31), DC_U, DC_U);

    filter_offset = MODE_8_SEC_2_CR_OFFSET_32;
    tmp_uintptr = sec2_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec2_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec2_cfg_buf, p_flow_tbl_list->p_CR_buf_32, CR_SIZE_32);
    p_step_cr->sec2_crs[28].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec2 cr[28] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_step_cr->sec2_crs[28].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_32), DC_U, DC_U);

    filter_offset = MODE_8_SEC_2_CR_OFFSET_33;
    tmp_uintptr = sec2_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec2_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec2_cfg_buf, p_flow_tbl_list->p_CR_buf_33, CR_SIZE_33);
    p_step_cr->sec2_crs[29].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec2 cr[29] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_step_cr->sec2_crs[29].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_33), DC_U, DC_U);

    filter_offset = MODE_8_SEC_2_CR_OFFSET_34;
    tmp_uintptr = sec2_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec2_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec2_cfg_buf, p_flow_tbl_list->p_CR_buf_34, CR_SIZE_34);
    p_step_cr->sec2_crs[30].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec2 cr[30] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_step_cr->sec2_crs[30].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_34), DC_U, DC_U);

    filter_offset = MODE_8_SEC_2_CR_OFFSET_35;
    tmp_uintptr = sec2_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec2_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec2_cfg_buf, p_flow_tbl_list->p_CR_buf_35, CR_SIZE_35);
    p_step_cr->sec2_crs[31].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec2 cr[31] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_step_cr->sec2_crs[31].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_35), DC_U, DC_U);

    filter_offset = MODE_8_SEC_2_CR_OFFSET_40;
    tmp_uintptr = sec2_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec2_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec2_cfg_buf, p_flow_tbl_list->p_CR_buf_40, CR_SIZE_40);
    p_step_cr->sec2_crs[32].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec2 cr[32] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_step_cr->sec2_crs[32].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_40), DC_U, DC_U);

    filter_offset = MODE_8_SEC_2_CR_OFFSET_36;
    tmp_uintptr = sec2_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec2_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec2_cfg_buf, p_flow_tbl_list->p_CR_buf_36, CR_SIZE_36);
    p_step_cr->sec2_crs[33].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec2 cr[33] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_step_cr->sec2_crs[33].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_36), DC_U, DC_U);

    filter_offset = MODE_8_SEC_2_CR_OFFSET_37;
    tmp_uintptr = sec2_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec2_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec2_cfg_buf, p_flow_tbl_list->p_CR_buf_37, CR_SIZE_37);
    p_step_cr->sec2_crs[34].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec2 cr[34] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_step_cr->sec2_crs[34].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_37), DC_U, DC_U);

    filter_offset = MODE_8_SEC_2_CR_OFFSET_38;
    tmp_uintptr = sec2_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec2_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec2_cfg_buf, p_flow_tbl_list->p_CR_buf_38, CR_SIZE_38);
    p_step_cr->sec2_crs[35].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec2 cr[35] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_step_cr->sec2_crs[35].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_38), DC_U, DC_U);

    filter_offset = MODE_8_SEC_2_CR_OFFSET_39;
    tmp_uintptr = sec2_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec2_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec2_cfg_buf, p_flow_tbl_list->p_CR_buf_39, CR_SIZE_39);
    p_step_cr->sec2_crs[36].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec2 cr[36] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_step_cr->sec2_crs[36].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_39), DC_U, DC_U);

    filter_offset = MODE_8_SEC_2_CR_OFFSET_41;
    tmp_uintptr = sec2_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec2_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec2_cfg_buf, p_flow_tbl_list->p_CR_buf_41, CR_SIZE_41);
    p_step_cr->sec2_crs[37].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec2 cr[37] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_step_cr->sec2_crs[37].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_41), DC_U, DC_U);

    filter_offset = MODE_4_SEC_18_CR_OFFSET_112;
    tmp_uintptr = sec18_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec18_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec18_cfg_buf, p_flow_tbl_list->p_CR_buf_112, CR_SIZE_112);
    p_step_cr->sec18_crs[1].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec18 cr[1] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_step_cr->sec18_crs[1].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_112), DC_U, DC_U);

    filter_offset = MODE_4_SEC_18_CR_OFFSET_113;
    tmp_uintptr = sec18_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec18_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec18_cfg_buf, p_flow_tbl_list->p_CR_buf_113, CR_SIZE_113);
    p_step_cr->sec18_crs[2].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint32_5("Sec18 cr[2] addr 0x%08x, offset %d, src_addr 0x%08x ",
                                  p_step_cr->sec18_crs[2].config_region_addr << 7, filter_offset, DC_U, DC_U, DC_U);

    filter_offset = MODE_4_SEC_18_CR_OFFSET_114;
    tmp_uintptr = sec18_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec18_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec18_cfg_buf, p_flow_tbl_list->p_CR_buf_114, CR_SIZE_114);
    p_step_cr->sec18_crs[3].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint32_5("Sec18 cr[3] addr 0x%08x, offset %d, src_addr 0x%08x ",
                                  p_step_cr->sec18_crs[3].config_region_addr << 7, filter_offset, DC_U, DC_U, DC_U);

    filter_offset = MODE_4_SEC_18_CR_OFFSET_115;
    tmp_uintptr = sec18_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec18_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec18_cfg_buf, p_flow_tbl_list->p_CR_buf_115, CR_SIZE_115);
    p_step_cr->sec18_crs[4].config_region_addr = (uint32_t)(tmp_uintptr - data_offset_base) >> 7U;
    amba_ik_system_print_uint32_5("Sec18 cr[4] addr 0x%08x, offset %d, src_addr 0x%08x ",
                                  p_step_cr->sec18_crs[4].config_region_addr << 7, filter_offset, DC_U, DC_U, DC_U);

    filter_offset = MODE_4_SEC_18_CR_OFFSET_117;
    tmp_uintptr = sec18_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec18_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec18_cfg_buf, p_flow_tbl_list->p_CR_buf_117, CR_SIZE_117);
    p_step_cr->sec18_crs[6].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec18 cr[6] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_step_cr->sec18_crs[6].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_117), DC_U, DC_U);

    filter_offset = MODE_4_SEC_18_CR_OFFSET_118;
    tmp_uintptr = sec18_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec18_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec18_cfg_buf, p_flow_tbl_list->p_CR_buf_118, CR_SIZE_118);
    p_step_cr->sec18_crs[7].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec18 cr[7] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_step_cr->sec18_crs[7].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_118), DC_U, DC_U);

    filter_offset = MODE_4_SEC_18_CR_OFFSET_119;
    tmp_uintptr = sec18_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec18_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec18_cfg_buf, p_flow_tbl_list->p_CR_buf_119, CR_SIZE_119);
    p_step_cr->sec18_crs[8].config_region_addr = (uint32_t)(tmp_uintptr - data_offset_base) >> 7U;
    amba_ik_system_print_uint32_5("Sec18 cr[8] addr 0x%08x, offset %d, src_addr 0x%08x ",
                                  p_step_cr->sec18_crs[8].config_region_addr << 7, filter_offset, DC_U, DC_U, DC_U);

    {
        for (i=0U; i<22U; i++) {
            p_step_cr->sec2_crs[i].config_region_addr = 0;
        }
        p_step_cr->sec2_crs[24].config_region_addr = 0;

        p_step_cr->sec18_crs[0].config_region_addr = 0;
        p_step_cr->sec18_crs[5].config_region_addr = 0;
        p_step_cr->sec18_crs[9].config_region_addr = 0;
        p_step_cr->sec18_crs[10].config_region_addr = 0;
    }

    return rval;
}

static uint32 img_cfg_mono7_amalgam_data_update(uintptr data_offset_base,
        idsp_step_crs_t *p_step_cr,
        const amalgam_mono7_flow_data_t *p_amalgam_data,
        const amba_ik_mono7_flow_tables_list_t *p_flow_tbl_list)
{
    uint32 rval = 0;
    uint32_t filter_offset;
    const uint8_t *tmp_p_u8;
    uintptr sec2_cfg_buf_offset;
    uintptr sec18_cfg_buf_offset;
    void* p_sec2_cfg_buf;
    void* p_sec18_cfg_buf;
    uintptr tmp_uintptr;
    uint32 i;

    // IKC does not use IDSP_CONFIG_HDR_SIZE, however, IKS does, in order to keep it on the same page between IKC and IKS,
    tmp_p_u8 = p_amalgam_data->sec2_cfg_buf;
    (void)amba_ik_system_memcpy(&sec2_cfg_buf_offset, &tmp_p_u8, sizeof(tmp_uintptr));
    tmp_p_u8 = p_amalgam_data->sec18_cfg_buf;
    (void)amba_ik_system_memcpy(&sec18_cfg_buf_offset, &tmp_p_u8, sizeof(tmp_uintptr));

    filter_offset = MODE_8_SEC_2_CR_OFFSET_26;
    tmp_uintptr = sec2_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec2_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec2_cfg_buf, p_flow_tbl_list->p_CR_buf_26, CR_SIZE_26);
    p_step_cr->sec2_crs[22].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec2 cr[22] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_step_cr->sec2_crs[22].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_26), DC_U, DC_U);

    filter_offset = MODE_8_SEC_2_CR_OFFSET_27;
    tmp_uintptr = sec2_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec2_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec2_cfg_buf, p_flow_tbl_list->p_CR_buf_27, CR_SIZE_27);
    p_step_cr->sec2_crs[23].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec2 cr[23] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_step_cr->sec2_crs[23].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_27), DC_U, DC_U);

    filter_offset = MODE_8_SEC_2_CR_OFFSET_29;
    tmp_uintptr = sec2_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec2_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec2_cfg_buf, p_flow_tbl_list->p_CR_buf_29, CR_SIZE_29);
    p_step_cr->sec2_crs[25].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec2 cr[25] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_step_cr->sec2_crs[25].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_29), DC_U, DC_U);

    filter_offset = MODE_8_SEC_2_CR_OFFSET_30;
    tmp_uintptr = sec2_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec2_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec2_cfg_buf, p_flow_tbl_list->p_CR_buf_30, CR_SIZE_30);
    p_step_cr->sec2_crs[26].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec2 cr[26] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_step_cr->sec2_crs[26].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_30), DC_U, DC_U);

    filter_offset = MODE_8_SEC_2_CR_OFFSET_31;
    tmp_uintptr = sec2_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec2_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec2_cfg_buf, p_flow_tbl_list->p_CR_buf_31, CR_SIZE_31);
    p_step_cr->sec2_crs[27].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec2 cr[27] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_step_cr->sec2_crs[27].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_31), DC_U, DC_U);

    filter_offset = MODE_8_SEC_2_CR_OFFSET_32;
    tmp_uintptr = sec2_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec2_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec2_cfg_buf, p_flow_tbl_list->p_CR_buf_32, CR_SIZE_32);
    p_step_cr->sec2_crs[28].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec2 cr[28] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_step_cr->sec2_crs[28].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_32), DC_U, DC_U);

    filter_offset = MODE_8_SEC_2_CR_OFFSET_33;
    tmp_uintptr = sec2_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec2_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec2_cfg_buf, p_flow_tbl_list->p_CR_buf_33, CR_SIZE_33);
    p_step_cr->sec2_crs[29].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec2 cr[29] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_step_cr->sec2_crs[29].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_33), DC_U, DC_U);

    filter_offset = MODE_8_SEC_2_CR_OFFSET_34;
    tmp_uintptr = sec2_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec2_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec2_cfg_buf, p_flow_tbl_list->p_CR_buf_34, CR_SIZE_34);
    p_step_cr->sec2_crs[30].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec2 cr[30] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_step_cr->sec2_crs[30].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_34), DC_U, DC_U);

    filter_offset = MODE_8_SEC_2_CR_OFFSET_35;
    tmp_uintptr = sec2_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec2_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec2_cfg_buf, p_flow_tbl_list->p_CR_buf_35, CR_SIZE_35);
    p_step_cr->sec2_crs[31].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec2 cr[31] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_step_cr->sec2_crs[31].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_35), DC_U, DC_U);

    filter_offset = MODE_8_SEC_2_CR_OFFSET_40;
    tmp_uintptr = sec2_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec2_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec2_cfg_buf, p_flow_tbl_list->p_CR_buf_40, CR_SIZE_40);
    p_step_cr->sec2_crs[32].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec2 cr[32] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_step_cr->sec2_crs[32].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_40), DC_U, DC_U);

    filter_offset = MODE_8_SEC_2_CR_OFFSET_36;
    tmp_uintptr = sec2_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec2_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec2_cfg_buf, p_flow_tbl_list->p_CR_buf_36, CR_SIZE_36);
    p_step_cr->sec2_crs[33].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec2 cr[33] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_step_cr->sec2_crs[33].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_36), DC_U, DC_U);

    filter_offset = MODE_8_SEC_2_CR_OFFSET_37;
    tmp_uintptr = sec2_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec2_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec2_cfg_buf, p_flow_tbl_list->p_CR_buf_37, CR_SIZE_37);
    p_step_cr->sec2_crs[34].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec2 cr[34] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_step_cr->sec2_crs[34].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_37), DC_U, DC_U);

    filter_offset = MODE_8_SEC_2_CR_OFFSET_38;
    tmp_uintptr = sec2_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec2_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec2_cfg_buf, p_flow_tbl_list->p_CR_buf_38, CR_SIZE_38);
    p_step_cr->sec2_crs[35].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec2 cr[35] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_step_cr->sec2_crs[35].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_38), DC_U, DC_U);

    filter_offset = MODE_8_SEC_2_CR_OFFSET_39;
    tmp_uintptr = sec2_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec2_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec2_cfg_buf, p_flow_tbl_list->p_CR_buf_39, CR_SIZE_39);
    p_step_cr->sec2_crs[36].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec2 cr[36] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_step_cr->sec2_crs[36].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_39), DC_U, DC_U);

    filter_offset = MODE_8_SEC_2_CR_OFFSET_41;
    tmp_uintptr = sec2_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec2_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec2_cfg_buf, p_flow_tbl_list->p_CR_buf_41, CR_SIZE_41);
    p_step_cr->sec2_crs[37].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec2 cr[37] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_step_cr->sec2_crs[37].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_41), DC_U, DC_U);

    filter_offset = MODE_4_SEC_18_CR_OFFSET_112;
    tmp_uintptr = sec18_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec18_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec18_cfg_buf, p_flow_tbl_list->p_CR_buf_112, CR_SIZE_112);
    p_step_cr->sec18_crs[1].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec18 cr[1] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_step_cr->sec18_crs[1].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_112), DC_U, DC_U);

    filter_offset = MODE_4_SEC_18_CR_OFFSET_113;
    tmp_uintptr = sec18_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec18_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec18_cfg_buf, p_flow_tbl_list->p_CR_buf_113, CR_SIZE_113);
    p_step_cr->sec18_crs[2].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint32_5("Sec18 cr[2] addr 0x%08x, offset %d, src_addr 0x%08x ",
                                  p_step_cr->sec18_crs[2].config_region_addr << 7, filter_offset, DC_U, DC_U, DC_U);

    filter_offset = MODE_4_SEC_18_CR_OFFSET_114;
    tmp_uintptr = sec18_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec18_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec18_cfg_buf, p_flow_tbl_list->p_CR_buf_114, CR_SIZE_114);
    p_step_cr->sec18_crs[3].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint32_5("Sec18 cr[3] addr 0x%08x, offset %d, src_addr 0x%08x ",
                                  p_step_cr->sec18_crs[3].config_region_addr << 7, filter_offset, DC_U, DC_U, DC_U);

    filter_offset = MODE_4_SEC_18_CR_OFFSET_115;
    tmp_uintptr = sec18_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec18_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec18_cfg_buf, p_flow_tbl_list->p_CR_buf_115, CR_SIZE_115);
    p_step_cr->sec18_crs[4].config_region_addr = (uint32_t)(tmp_uintptr - data_offset_base) >> 7U;
    amba_ik_system_print_uint32_5("Sec18 cr[4] addr 0x%08x, offset %d, src_addr 0x%08x ",
                                  p_step_cr->sec18_crs[4].config_region_addr << 7, filter_offset, DC_U, DC_U, DC_U);

    filter_offset = MODE_4_SEC_18_CR_OFFSET_117;
    tmp_uintptr = sec18_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec18_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec18_cfg_buf, p_flow_tbl_list->p_CR_buf_117, CR_SIZE_117);
    p_step_cr->sec18_crs[6].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec18 cr[6] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_step_cr->sec18_crs[6].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_117), DC_U, DC_U);

    filter_offset = MODE_4_SEC_18_CR_OFFSET_118;
    tmp_uintptr = sec18_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec18_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec18_cfg_buf, p_flow_tbl_list->p_CR_buf_118, CR_SIZE_118);
    p_step_cr->sec18_crs[7].config_region_addr = (uint32_t)((tmp_uintptr - data_offset_base) >> 7U);
    amba_ik_system_print_uint64_5("Sec18 cr[7] addr 0x%08x, offset %d, src_addr %p ",
                                  (uint64)p_step_cr->sec18_crs[7].config_region_addr << 7, filter_offset, ptr2uintptr(p_flow_tbl_list->p_CR_buf_118), DC_U, DC_U);

    filter_offset = MODE_4_SEC_18_CR_OFFSET_119;
    tmp_uintptr = sec18_cfg_buf_offset + IDSP_CONFIG_HDR_SIZE + filter_offset;
    (void)amba_ik_system_memcpy(&p_sec18_cfg_buf, &tmp_uintptr, sizeof(void*));
    (void)amba_ik_system_memcpy(p_sec18_cfg_buf, p_flow_tbl_list->p_CR_buf_119, CR_SIZE_119);
    p_step_cr->sec18_crs[8].config_region_addr = (uint32_t)(tmp_uintptr - data_offset_base) >> 7U;
    amba_ik_system_print_uint32_5("Sec18 cr[8] addr 0x%08x, offset %d, src_addr 0x%08x ",
                                  p_step_cr->sec18_crs[8].config_region_addr << 7, filter_offset, DC_U, DC_U, DC_U);

    {
        for (i=0U; i<22U; i++) {
            p_step_cr->sec2_crs[i].config_region_addr = 0;
        }
        p_step_cr->sec2_crs[24].config_region_addr = 0;

        p_step_cr->sec18_crs[0].config_region_addr = 0;
        p_step_cr->sec18_crs[5].config_region_addr = 0;
        p_step_cr->sec18_crs[9].config_region_addr = 0;
        p_step_cr->sec18_crs[10].config_region_addr = 0;
    }

    return rval;
}
#endif
uint32 img_cfg_motion_fusion_amalgam_data_update(const idsp_flow_ctrl_t *p_flow, const amba_ik_motion_fusion_flow_tables_list_t *p_flow_tbl_list, const void *p_user_buffer)
{
    uint32 rval = 0;
    amalgam_motion_fusion_flow_data_t *p_amalgam_data;
    uintptr data_offset_base;
    const void *temp_data_addr = NULL;
    const uint8_t *tmp_p_u8;
    const int16_t *tmp_p_s16;
    uintptr tmp_uintptr;


    (void)amba_ik_system_memcpy(&p_amalgam_data, &p_user_buffer, sizeof(amalgam_motion_fusion_flow_data_t *));
    (void)amba_ik_system_memcpy(&data_offset_base, &p_amalgam_data, sizeof(uintptr));
    (void)amba_ik_system_memset(p_amalgam_data, 0x0, sizeof(amalgam_motion_fusion_flow_data_t));
    (void)amba_ik_system_memcpy(&p_amalgam_data->flow_ctrl, p_flow, sizeof(idsp_flow_ctrl_t));

    if (p_flow->flow_info.sbp_map_addr != 0U) {
        temp_data_addr = img_cfg_uint32_physical_to_void_virtual(p_flow->flow_info.sbp_map_addr);
        (void)amba_ik_system_memcpy(p_amalgam_data->sbp_map, temp_data_addr, IK_AMALGAM_TABLE_SBP_SIZE);
        tmp_p_u8 = p_amalgam_data->sbp_map;
        (void)amba_ik_system_memcpy(&tmp_uintptr, &tmp_p_u8, sizeof(tmp_uintptr));
        p_amalgam_data->flow_ctrl.flow_info.sbp_map_addr = (uint32_t)(tmp_uintptr - data_offset_base);
        amba_ik_system_print_uint64_5("sbp_map_addr 0x%08x, src_addr %p ",
                                      p_amalgam_data->flow_ctrl.flow_info.sbp_map_addr, ptr2uintptr(temp_data_addr), DC_U, DC_U, DC_U);
    } else {
        amba_ik_system_print_str_5("[IK] SBP is NULL", NULL, NULL, NULL, NULL, NULL);
    }

    if (p_flow_tbl_list->p_ca_warp_hor_red != NULL) {
        (void)amba_ik_system_memcpy(p_amalgam_data->ca_warp_hor_red, p_flow_tbl_list->p_ca_warp_hor_red, sizeof(p_amalgam_data->ca_warp_hor_red));
        tmp_p_s16 = p_amalgam_data->ca_warp_hor_red;
        (void)amba_ik_system_memcpy(&tmp_uintptr, &tmp_p_s16, sizeof(tmp_uintptr));
        p_amalgam_data->flow_ctrl.calib.cawarp_horizontal_table_addr_red = (uint32_t)(tmp_uintptr - data_offset_base);
        amba_ik_system_print_uint64_5("cawarp_horizontal_table_addr_red 0x%08x, src_addr %p ",
                                      p_amalgam_data->flow_ctrl.calib.cawarp_horizontal_table_addr_red, ptr2uintptr(p_flow_tbl_list->p_ca_warp_hor_red), DC_U, DC_U, DC_U);
    } else {
        amba_ik_system_print_str_5("[IK] cawarp_hor_red is NULL", NULL, NULL, NULL, NULL, NULL);
    }

    if (p_flow_tbl_list->p_ca_warp_hor_blue != NULL) {
        (void)amba_ik_system_memcpy(p_amalgam_data->ca_warp_hor_blue, p_flow_tbl_list->p_ca_warp_hor_blue, sizeof(p_amalgam_data->ca_warp_hor_blue));
        tmp_p_s16 = p_amalgam_data->ca_warp_hor_blue;
        (void)amba_ik_system_memcpy(&tmp_uintptr, &tmp_p_s16, sizeof(tmp_uintptr));
        p_amalgam_data->flow_ctrl.calib.cawarp_horizontal_table_addr_blue = (uint32_t)(tmp_uintptr - data_offset_base);
        amba_ik_system_print_uint64_5("cawarp_horizontal_table_addr_blue 0x%08x, src_addr %p ",
                                      p_amalgam_data->flow_ctrl.calib.cawarp_horizontal_table_addr_blue, ptr2uintptr(p_flow_tbl_list->p_ca_warp_hor_blue), DC_U, DC_U, DC_U);
    } else {
        amba_ik_system_print_str_5("[IK] cawarp_hor_blue is NULL", NULL, NULL, NULL, NULL, NULL);
    }

    if (p_flow_tbl_list->p_ca_warp_ver_red != NULL) {
        (void)amba_ik_system_memcpy(p_amalgam_data->ca_warp_ver_red, p_flow_tbl_list->p_ca_warp_ver_red, sizeof(p_amalgam_data->ca_warp_ver_red));
        tmp_p_s16 = p_amalgam_data->ca_warp_ver_red;
        (void)amba_ik_system_memcpy(&tmp_uintptr, &tmp_p_s16, sizeof(tmp_uintptr));
        p_amalgam_data->flow_ctrl.calib.cawarp_vertical_table_addr_red= (uint32_t)(tmp_uintptr - data_offset_base);
        amba_ik_system_print_uint64_5("cawarp_vertical_table_addr_red 0x%08x, src_addr %p ",
                                      p_amalgam_data->flow_ctrl.calib.cawarp_vertical_table_addr_red, ptr2uintptr(p_flow_tbl_list->p_ca_warp_ver_red), DC_U, DC_U, DC_U);
    } else {
        amba_ik_system_print_str_5("[IK] cawarp_ver_red is NULL", NULL, NULL, NULL, NULL, NULL);
    }

    if (p_flow_tbl_list->p_ca_warp_ver_blue != NULL) {
        (void)amba_ik_system_memcpy(p_amalgam_data->ca_warp_ver_blue, p_flow_tbl_list->p_ca_warp_ver_blue, sizeof(p_amalgam_data->ca_warp_ver_blue));
        tmp_p_s16 = p_amalgam_data->ca_warp_ver_blue;
        (void)amba_ik_system_memcpy(&tmp_uintptr, &tmp_p_s16, sizeof(tmp_uintptr));
        p_amalgam_data->flow_ctrl.calib.cawarp_vertical_table_addr_blue = (uint32_t)(tmp_uintptr - data_offset_base);
        amba_ik_system_print_uint64_5("cawarp_vertical_table_addr_blue 0x%08x, src_addr %p ",
                                      p_amalgam_data->flow_ctrl.calib.cawarp_vertical_table_addr_blue, ptr2uintptr(p_flow_tbl_list->p_ca_warp_ver_blue), DC_U, DC_U, DC_U);
    } else {
        amba_ik_system_print_str_5("[IK] cawarp_ver_blue is NULL", NULL, NULL, NULL, NULL, NULL);
    }

    if (p_flow_tbl_list->p_warp_hor != NULL) {
        (void)amba_ik_system_memcpy(p_amalgam_data->warp_hor, p_flow_tbl_list->p_warp_hor, sizeof(p_amalgam_data->warp_hor));
        tmp_p_s16 = p_amalgam_data->warp_hor;
        (void)amba_ik_system_memcpy(&tmp_uintptr, &tmp_p_s16, sizeof(tmp_uintptr));
        p_amalgam_data->flow_ctrl.calib.warp_horizontal_table_address = (uint32_t)(tmp_uintptr - data_offset_base);
        amba_ik_system_print_uint64_5("warp_horizontal_table_addr 0x%08x, src_addr %p ",
                                      p_amalgam_data->flow_ctrl.calib.warp_horizontal_table_address, ptr2uintptr(p_flow_tbl_list->p_warp_hor), DC_U, DC_U, DC_U);
    } else {
        amba_ik_system_print_str_5("[IK] warp_hor is NULL", NULL, NULL, NULL, NULL, NULL);
    }

    if (p_flow_tbl_list->p_warp_ver != NULL) {
        (void)amba_ik_system_memcpy(p_amalgam_data->warp_ver, p_flow_tbl_list->p_warp_ver, sizeof(p_amalgam_data->warp_ver));
        tmp_p_s16 = p_amalgam_data->warp_ver;
        (void)amba_ik_system_memcpy(&tmp_uintptr, &tmp_p_s16, sizeof(tmp_uintptr));
        p_amalgam_data->flow_ctrl.calib.warp_vertical_table_address = (uint32_t)(tmp_uintptr - data_offset_base);
        amba_ik_system_print_uint64_5("warp_vertical_table_addr 0x%08x, src_addr %p ",
                                      p_amalgam_data->flow_ctrl.calib.warp_vertical_table_address, ptr2uintptr(p_flow_tbl_list->p_warp_ver), DC_U, DC_U, DC_U);
    } else {
        amba_ik_system_print_str_5("[IK] warp_ver is NULL", NULL, NULL, NULL, NULL, NULL);
    }

    if (p_flow_tbl_list->p_warp_hor_b != NULL) {
        (void)amba_ik_system_memcpy(p_amalgam_data->warp_hor_b, p_flow_tbl_list->p_warp_hor_b, sizeof(p_amalgam_data->warp_hor_b));
        tmp_p_s16 = p_amalgam_data->warp_hor_b;
        (void)amba_ik_system_memcpy(&tmp_uintptr, &tmp_p_s16, sizeof(tmp_uintptr));
        p_amalgam_data->flow_ctrl.calib.warp_horizontal_b_table_address = (uint32_t)(tmp_uintptr - data_offset_base);
        amba_ik_system_print_uint64_5("warp_horizontal_b_table_addr 0x%08x, src_addr %p ",
                                      p_amalgam_data->flow_ctrl.calib.warp_horizontal_b_table_address, ptr2uintptr(p_flow_tbl_list->p_warp_hor_b), DC_U, DC_U, DC_U);
    } else {
        amba_ik_system_print_str_5("[IK] warp_hor is NULL", NULL, NULL, NULL, NULL, NULL);
    }

    if (p_flow_tbl_list->p_aaa != NULL) {
        const idsp_aaa_data_t *p_tmp_aaa;
        (void)amba_ik_system_memcpy(p_amalgam_data->aaa, p_flow_tbl_list->p_aaa, sizeof(p_amalgam_data->aaa));
        p_tmp_aaa = p_amalgam_data->aaa[0];
        (void)amba_ik_system_memcpy(&tmp_uintptr, &p_tmp_aaa, sizeof(tmp_uintptr));
        p_amalgam_data->flow_ctrl.stitch.aaa_info_daddr = (uint32_t)(tmp_uintptr - data_offset_base);
        amba_ik_system_print_uint64_5("aaa_info_daddr 0x%08x, src_addr %p ",
                                      p_amalgam_data->flow_ctrl.stitch.aaa_info_daddr, ptr2uintptr(p_flow_tbl_list->p_aaa), DC_U, DC_U, DC_U);
    } else {
        amba_ik_system_print_str_5("[IK] AAA is NULL", NULL, NULL, NULL, NULL, NULL);
    }

    if (p_flow_tbl_list->p_extra_window != NULL) {
        const idsp_extra_window_info_t *p_tmp_extra_win;
        (void)amba_ik_system_memcpy(&p_amalgam_data->extra_window_info, p_flow_tbl_list->p_extra_window, sizeof(idsp_extra_window_info_t));
        p_tmp_extra_win = &p_amalgam_data->extra_window_info;
        (void)amba_ik_system_memcpy(&tmp_uintptr, &p_tmp_extra_win, sizeof(uintptr));
        p_amalgam_data->flow_ctrl.window.extra_window_info_addr = (uint32_t)(tmp_uintptr - data_offset_base);
        amba_ik_system_print_uint64_5("extra_window_info_addr 0x%08x, src_addr %p ",
                                      p_amalgam_data->flow_ctrl.window.extra_window_info_addr, ptr2uintptr(p_flow_tbl_list->p_extra_window), DC_U, DC_U, DC_U);
    } else {
        amba_ik_system_print_str_5("[IK] extra_window_info_addr is NULL", NULL, NULL, NULL, NULL, NULL);
    }

    amba_ik_system_print_str_5("Step 0 r2y:", DC_S, DC_S, DC_S, DC_S, DC_S);
    (void)img_cfg_r2y_amalgam_data_update(data_offset_base, &p_amalgam_data->flow_ctrl.step_crs[0], &p_amalgam_data->r2y_cfg_buf, &p_flow_tbl_list->r2y);
    amba_ik_system_print_str_5("Step 1 motion_me1:", DC_S, DC_S, DC_S, DC_S, DC_S);
    (void)img_cfg_motion_me1_amalgam_data_update(data_offset_base, &p_amalgam_data->flow_ctrl.step_crs[1], &p_amalgam_data->motion_me1_cfg_buf, &p_flow_tbl_list->motion_me1);
    amba_ik_system_print_str_5("Step 2 motion_a:", DC_S, DC_S, DC_S, DC_S, DC_S);
    (void)img_cfg_motion_a_amalgam_data_update(data_offset_base, &p_amalgam_data->flow_ctrl.step_crs[2], &p_amalgam_data->motion_a_cfg_buf, &p_flow_tbl_list->motion_a);
    amba_ik_system_print_str_5("Step 3 motion_b:", DC_S, DC_S, DC_S, DC_S, DC_S);
    (void)img_cfg_motion_b_amalgam_data_update(data_offset_base, &p_amalgam_data->flow_ctrl.step_crs[3], &p_amalgam_data->motion_b_cfg_buf, &p_flow_tbl_list->motion_b);
    amba_ik_system_print_str_5("Step 4 motion_c:", DC_S, DC_S, DC_S, DC_S, DC_S);
    (void)img_cfg_motion_c_amalgam_data_update(data_offset_base, &p_amalgam_data->flow_ctrl.step_crs[4], &p_amalgam_data->motion_c_cfg_buf, &p_flow_tbl_list->motion_c);
#if SUPPORT_FUSION
    amba_ik_system_print_str_5("Step 5 mono1:", DC_S, DC_S, DC_S, DC_S, DC_S);
    (void)img_cfg_mono1_8_amalgam_data_update(data_offset_base, &p_amalgam_data->flow_ctrl.step_crs[5], &p_amalgam_data->mono1_cfg_buf, &p_flow_tbl_list->mono1);
    amba_ik_system_print_str_5("Step 6 mono3:", DC_S, DC_S, DC_S, DC_S, DC_S);
    (void)img_cfg_mono3_amalgam_data_update(data_offset_base, &p_amalgam_data->flow_ctrl.step_crs[6], &p_amalgam_data->mono3_cfg_buf, &p_flow_tbl_list->mono3);
    amba_ik_system_print_str_5("Step 7 mono4:", DC_S, DC_S, DC_S, DC_S, DC_S);
    (void)img_cfg_mono4_amalgam_data_update(data_offset_base, &p_amalgam_data->flow_ctrl.step_crs[7], &p_amalgam_data->mono4_cfg_buf, &p_flow_tbl_list->mono4);
    amba_ik_system_print_str_5("Step 8 mono5:", DC_S, DC_S, DC_S, DC_S, DC_S);
    (void)img_cfg_mono5_amalgam_data_update(data_offset_base, &p_amalgam_data->flow_ctrl.step_crs[8], &p_amalgam_data->mono5_cfg_buf, &p_flow_tbl_list->mono5);
    amba_ik_system_print_str_5("Step 9 mono6:", DC_S, DC_S, DC_S, DC_S, DC_S);
    (void)img_cfg_mono6_amalgam_data_update(data_offset_base, &p_amalgam_data->flow_ctrl.step_crs[9], &p_amalgam_data->mono6_cfg_buf, &p_flow_tbl_list->mono6);
    amba_ik_system_print_str_5("Step 10 mono7:", DC_S, DC_S, DC_S, DC_S, DC_S);
    (void)img_cfg_mono7_amalgam_data_update(data_offset_base, &p_amalgam_data->flow_ctrl.step_crs[10], &p_amalgam_data->mono7_cfg_buf, &p_flow_tbl_list->mono7);
    amba_ik_system_print_str_5("Step 11 mono8:", DC_S, DC_S, DC_S, DC_S, DC_S);
    (void)img_cfg_mono1_8_amalgam_data_update(data_offset_base, &p_amalgam_data->flow_ctrl.step_crs[11], &p_amalgam_data->mono8_cfg_buf, &p_flow_tbl_list->mono8);
#endif
    //p_amalgam_data->flow_ctrl.header.idsp_flow_addr = data_offset_base;
    (void)amba_ik_system_memcpy(&p_amalgam_data->flow_ctrl.header.idsp_flow_addr, &data_offset_base, sizeof(uint32));

    return rval;
}

uint32 img_cfg_motion_fusion_amalgam_data_ctx_update(uint32 context_id, const void *p_user_buffer)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx;
    ik_motion_fusion_debug_data_t *p_ik_dbg_data;

    rval |= img_ctx_get_context(context_id, &p_ctx);

    if(rval == IK_OK) {
        (void)amba_ik_system_memcpy(&p_ik_dbg_data, &p_user_buffer, sizeof(ik_motion_fusion_debug_data_t *));

        //ctx info
        p_ik_dbg_data->ctx_debug_data.ctx_id = (uint16)context_id;
        p_ik_dbg_data->ctx_debug_data.active_flow_idx = p_ctx->organization.active_cr_state.active_flow_idx;

        //3a statistic
        (void)amba_ik_system_memcpy(&p_ik_dbg_data->ctx_debug_data.aaa_stat_info, &p_ctx->filters.input_param.aaa_stat_info, sizeof(ik_aaa_stat_info_t));
        (void)amba_ik_system_memcpy(&p_ik_dbg_data->ctx_debug_data.aaa_pg_stat_info, &p_ctx->filters.input_param.aaa_pg_stat_info, sizeof(ik_aaa_pg_af_stat_info_t));
        (void)amba_ik_system_memcpy(&p_ik_dbg_data->ctx_debug_data.af_stat_ex_info, &p_ctx->filters.input_param.af_stat_ex_info, sizeof(ik_af_stat_ex_info_t));
        (void)amba_ik_system_memcpy(&p_ik_dbg_data->ctx_debug_data.pg_af_stat_ex_info, &p_ctx->filters.input_param.pg_af_stat_ex_info, sizeof(ik_pg_af_stat_ex_info_t));
        (void)amba_ik_system_memcpy(&p_ik_dbg_data->ctx_debug_data.hist_info, &p_ctx->filters.input_param.hist_info, sizeof(ik_histogram_info_t));
        (void)amba_ik_system_memcpy(&p_ik_dbg_data->ctx_debug_data.hist_info_pg, &p_ctx->filters.input_param.hist_info_pg, sizeof(ik_histogram_info_t));

        // window info
        p_ik_dbg_data->ctx_debug_data.flip_mode = p_ctx->filters.input_param.flip_mode;
        (void)amba_ik_system_memcpy(&p_ik_dbg_data->ctx_debug_data.dzoom_info, &p_ctx->filters.input_param.dzoom_info, sizeof(ik_dzoom_info_t));
        (void)amba_ik_system_memcpy(&p_ik_dbg_data->ctx_debug_data.dmy_range, &p_ctx->filters.input_param.dmy_range, sizeof(ik_dummy_margin_range_t));
        (void)amba_ik_system_memcpy(&p_ik_dbg_data->ctx_debug_data.active_window, &p_ctx->filters.input_param.active_window, sizeof(ik_vin_active_window_t));
        (void)amba_ik_system_memcpy(&p_ik_dbg_data->ctx_debug_data.stitching_info, &p_ctx->filters.input_param.stitching_info, sizeof(ik_stitch_info_t));
        (void)amba_ik_system_memcpy(&p_ik_dbg_data->ctx_debug_data.window_size_info, &p_ctx->filters.input_param.window_size_info, sizeof(ik_window_size_info_t));
        (void)amba_ik_system_memcpy(&p_ik_dbg_data->ctx_debug_data.result_win, &p_ctx->filters.input_param.ctx_buf.result_win, sizeof(amba_ik_calc_win_result_t));

        // vig
        p_ik_dbg_data->ctx_debug_data.vig_enable = p_ctx->filters.input_param.vig_enable;
        (void)amba_ik_system_memcpy(&p_ik_dbg_data->ctx_debug_data.vignette_compensation, &p_ctx->filters.input_param.vignette_compensation, sizeof(ik_vignette_t));

        // ca
        p_ik_dbg_data->ctx_debug_data.cawarp_enable = p_ctx->filters.input_param.cawarp_enable;
        (void)amba_ik_system_memcpy(&p_ik_dbg_data->ctx_debug_data.calib_cawarp_info, &p_ctx->filters.input_param.calib_ca_warp_info, sizeof(ik_cawarp_info_t));
        if(p_ctx->filters.input_param.calib_ca_warp_info.p_cawarp_red != NULL) {
            (void)amba_ik_system_memcpy(p_ik_dbg_data->ctx_debug_data.calib_cawarp_red, p_ctx->filters.input_param.calib_ca_warp_info.p_cawarp_red, sizeof(ik_grid_point_t) * CAWARP_VIDEO_MAXHORGRID * CAWARP_VIDEO_MAXVERGRID);
        } else {
            (void)amba_ik_system_memset(p_ik_dbg_data->ctx_debug_data.calib_cawarp_red, 0x0, sizeof(ik_grid_point_t) * CAWARP_VIDEO_MAXHORGRID * CAWARP_VIDEO_MAXVERGRID);
        }
        if(p_ctx->filters.input_param.calib_ca_warp_info.p_cawarp_blue != NULL) {
            (void)amba_ik_system_memcpy(p_ik_dbg_data->ctx_debug_data.calib_cawarp_blue, p_ctx->filters.input_param.calib_ca_warp_info.p_cawarp_blue, sizeof(ik_grid_point_t) * CAWARP_VIDEO_MAXHORGRID * CAWARP_VIDEO_MAXVERGRID);
        } else {
            (void)amba_ik_system_memset(p_ik_dbg_data->ctx_debug_data.calib_cawarp_blue, 0x0, sizeof(ik_grid_point_t) * CAWARP_VIDEO_MAXHORGRID * CAWARP_VIDEO_MAXVERGRID);
        }

        // warp
        p_ik_dbg_data->ctx_debug_data.warp_enable = p_ctx->filters.input_param.warp_enable;
        p_ik_dbg_data->ctx_debug_data.chroma_radius = p_ctx->filters.input_param.chroma_filter.radius;
        p_ik_dbg_data->ctx_debug_data.yuv_mode = p_ctx->filters.input_param.yuv_mode;
        (void)amba_ik_system_memcpy(&p_ik_dbg_data->ctx_debug_data.calib_warp_info, &p_ctx->filters.input_param.calib_warp_info, sizeof(ik_warp_info_t));
        (void)amba_ik_system_memcpy(&p_ik_dbg_data->ctx_debug_data.warp_buf_info, &p_ctx->filters.input_param.warp_buf_info, sizeof(ik_warp_buffer_info_t));
        if(p_ctx->filters.input_param.calib_warp_info.pwarp != NULL) {
            (void)amba_ik_system_memcpy(p_ik_dbg_data->ctx_debug_data.calib_warp, p_ctx->filters.input_param.calib_warp_info.pwarp, sizeof(ik_grid_point_t) * WARP_VIDEO_MAXHORGRID * WARP_VIDEO_MAXVERGRID * 2U);
        } else {
            (void)amba_ik_system_memset(p_ik_dbg_data->ctx_debug_data.calib_warp, 0x0, sizeof(ik_grid_point_t) * WARP_VIDEO_MAXHORGRID * WARP_VIDEO_MAXVERGRID * 2U);
        }

        {
            // version
            uint32 ikc_version_major, ikc_version_minor;
            rval |= ikc_get_version(&ikc_version_major, &ikc_version_minor);
            p_ik_dbg_data->ctx_debug_data.ik_version_major = (uint32)IK_VERSION_MAJOR;
            p_ik_dbg_data->ctx_debug_data.ik_version_minor = (uint32)IK_VERSION_MINOR;
            p_ik_dbg_data->ctx_debug_data.ikc_version_major = ikc_version_major;
            p_ik_dbg_data->ctx_debug_data.ikc_version_minor = ikc_version_minor;
        }
    } else {
        amba_ik_system_print_str_5("[IK] can't get context", NULL, NULL, NULL, NULL, NULL);
    }

    return rval;
}

