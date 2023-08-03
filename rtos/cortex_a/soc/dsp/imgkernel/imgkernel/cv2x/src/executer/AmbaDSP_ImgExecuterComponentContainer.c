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
#include "AmbaDSP_ImgArchSpec.h"
#include "AmbaDSP_ImgExecuterComponentIF.h"
#include "AmbaDSP_ImgContextEntity.h"
#include "AmbaDSP_ImgContextComponentIF.h"

#if 0

static inline uint32 exe_insert_iso_config_in_config_container(const amba_ik_config_containter_handler_t *handler, const amba_ik_cv2_liso_cfg_t *iso_config)
{
    if ((handler==NULL) || (iso_config==NULL)) {
        // fix compile error unused parameter
    }
//    update_iso_config_t update_iso_config = {0};
//    intptr misra_intptr;
//
//    update_iso_config.cmd_code = (uint32)UPDATE_ISO_CONFIG;//0x09060001
//    update_iso_config.iso_config_calib_update = 1;
//    update_iso_config.iso_config_color_update = 1;
//    update_iso_config.iso_config_mctf_update  = 1;
//    update_iso_config.iso_config_step1_update = 1;
//    update_iso_config.aaa_setup_update = 1; // AAA setup update in cmd Q
//    update_iso_config.is_high_iso = 0;
//    (void)amba_ik_system_memcpy(&misra_intptr, &iso_config, sizeof(intptr));
//    update_iso_config.iso_cfg_daddr = (uint32)amba_ik_system_virt_to_phys(misra_intptr);
//
//    (void)amba_ik_system_memcpy(&handler->container->idsp_cmds[handler->idsp_cmd_number][0], &update_iso_config, sizeof(update_iso_config));
//    handler->idsp_cmd_number += 1UL;
    return IK_OK;
}


static inline uint32 exe_insert_hdr_ce_config_in_config_container(amba_ik_config_containter_handler_t *handler, const amba_ik_hdr_ce_cfg_t *hdr_ce_config)
{
    update_hdr_ce_config_t update_hdr_ce_config;
    intptr misra_intptr;

    if((handler->ability->pipe == AMBA_IK_PIPE_VIDEO) && \
       ((handler->ability->video_pipe == AMBA_IK_VIDEO_HDR_EXPO_1)|| \
        (handler->ability->video_pipe == AMBA_IK_VIDEO_HDR_EXPO_2)|| \
        (handler->ability->video_pipe == AMBA_IK_VIDEO_HDR_EXPO_3))) {

        update_hdr_ce_config.cmd_code = (uint32)UPDATE_HDR_CE_CONFIG;//0x09060002
        update_hdr_ce_config.number_of_exposures = hdr_ce_config->exp[0].no_of_exposures;
        update_hdr_ce_config.contrast_enhancement_enable = 1;
        update_hdr_ce_config.exp_0_update = 1;
        update_hdr_ce_config.exp_1_update = 1;
        update_hdr_ce_config.exp_2_update = 1;
        update_hdr_ce_config.blend_0_update = 1;
        update_hdr_ce_config.blend_1_update = 1;
        update_hdr_ce_config.ce_update = 1;
        update_hdr_ce_config.exp_0_start_offset_update = 1;
        update_hdr_ce_config.exp_1_start_offset_update = 1;
        update_hdr_ce_config.exp_2_start_offset_update = 1;
        //update_hdr_ce_config.raw_daddr_start_offset_y[3];
        (void)amba_ik_system_memcpy(&misra_intptr, &hdr_ce_config, sizeof(intptr));
        update_hdr_ce_config.hdr_ce_cfg_daddr = (uint32)amba_ik_system_virt_to_phys(misra_intptr);

        (void)amba_ik_system_memcpy(&handler->container->idsp_cmds[handler->idsp_cmd_number][0], &update_hdr_ce_config, sizeof(amba_ik_hdr_ce_cfg_t));
        handler->idsp_cmd_number += 1UL;

    } else {
        //do nothing
    }
    return IK_OK;
}

static inline uint32 exe_insert_idsp_cmds_in_config_container(amba_ik_config_containter_handler_t *handler, uint32 cmd_number, const amba_ik_setup_cmd_description_t *cmds)
{
    int32 i = 0;
    const void *misra_void;
    if( cmd_number > 0UL ) {
        for( i = 0; i < (int32)cmd_number; i++) {
            (void)amba_ik_system_memcpy(&misra_void, &cmds[i].cmd_address, sizeof(void*));
            (void)amba_ik_system_memcpy(&handler->container->idsp_cmds[handler->idsp_cmd_number][0], misra_void, cmds[i].cmd_size);
            handler->idsp_cmd_number += 1UL;
        }
    } else {
        // misraC fix
    }

    return IK_OK;
}

static inline uint32 exe_insert_vin_cmds_in_config_container(amba_ik_config_containter_handler_t *handler, uint32 cmd_number, const amba_ik_setup_cmd_description_t *cmds)
{
    int32 i = 0;
    const void *misra_void;
    if( cmd_number > 0UL ) {
        for( i = 0; i < (int32)cmd_number; i++) {
            (void)amba_ik_system_memcpy(&misra_void, &cmds[i].cmd_address, sizeof(void*));
            (void)amba_ik_system_memcpy(&handler->container->vin_cmds[handler->vin_cmd_number][0], misra_void, cmds[i].cmd_size);
            handler->vin_cmd_number += 1UL;
        }
    } else {
        // misraC fix
    }

    return IK_OK;
}
#endif

static inline uint32 exe_pack_config_container(const amba_ik_config_containter_handler_t *handler, uint32 config_tag)
{
    handler->container->ik_id = config_tag;
//    handler->container->idsp_cmds[handler->idsp_cmd_number][0] = (uint32)0xDEADBEEFU;
//    handler->container->vin_cmds[handler->vin_cmd_number][0] = (uint32)0xDEADBEEFU;
//
//    (void)amba_ik_system_clean_cache( &handler->container->idsp_cmds[0][0], 128UL * handler->idsp_cmd_number);
    //amba_ik_system_clean_cache( &handler->container->vin_cmds[0][0], 128 * handler->vin_cmd_number);

    return IK_OK;
}

uint32 img_exe_construct_config_container_controller(uint32 context_id, uint32 iso_config_id, amba_ik_config_container_controller_t *p_controller)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;

    if (p_controller == NULL) {
        amba_ik_system_print_str_5("[IK] NULL pointer in %s", __func__, NULL, NULL, NULL, NULL);
        amba_ik_system_print_uint32_5("[IK] iso_config_id = %d ", iso_config_id, DC_U, DC_U, DC_U, DC_U);
        rval = IK_ERR_0005;
    } else {
        rval = img_ctx_get_context(context_id, &p_ctx);
        if (p_ctx->organization.initial_flag == 0U) {
            amba_ik_system_print_uint32_5("[IK] context_id = %d is not initial", context_id, DC_U, DC_U, DC_U, DC_U);
            rval = IK_ERR_0002;
        } else if (rval == IK_OK) {
            p_controller->handler.ability = &p_ctx->organization.attribute.ability;
            (void)amba_ik_system_memcpy(&p_controller->handler.container, &p_ctx->configs.configs_container[iso_config_id], sizeof(amba_ik_config_container_t*));
            p_controller->handler.idsp_cmd_number = 0UL;
            p_controller->handler.vin_cmd_number = 0UL;
            p_controller->pack_container = exe_pack_config_container;
        } else {
            amba_ik_system_print_str_5("[IK] can't get context", NULL, NULL, NULL, NULL, NULL);
        }
    }
    return rval;
}
