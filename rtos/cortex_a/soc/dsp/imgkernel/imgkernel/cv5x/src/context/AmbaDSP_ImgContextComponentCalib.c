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

#include "AmbaDSP_ImgContextEntity.h"
#include "AmbaDSP_ImgContextComponentIF.h"
#include "AmbaDSP_ImgContextDebugUtility.h"
#include "AmbaDSP_ImgContextUtility.h"

uint32 img_ctx_ivd_set_static_bpc(const uint32 context_id, const ik_static_bad_pxl_cor_t *p_static_bpc)
{
    amba_ik_system_print_str_5("[IK][ERROR] set_static_bpc is invalid !", DC_S, DC_S, DC_S, DC_S, DC_S);
    if (context_id == 0U) {
        //TBD
    }
    if (p_static_bpc == NULL) {
        //TBD
    }
    return IK_ERR_0004;
}

uint32 img_ctx_ivd_set_static_bpc_internal(const uint32 context_id, const ik_static_bad_pixel_correction_internal_t *static_bpc_internal)
{
    amba_ik_system_print_str_5("[IK][ERROR] set_static_bpc_internal is invalid !", DC_S, DC_S, DC_S, DC_S, DC_S);
    if (context_id == 0U) {
        //TBD
    }
    if (static_bpc_internal == NULL) {
        //TBD
    }
    return IK_ERR_0004;
}

uint32 img_ctx_ivd_set_static_bpc_enable_info(const uint32 context_id, const uint32 enable)
{
    amba_ik_system_print_str_5("[IK][ERROR] set_static_bpc_enable_info is invalid !", DC_S, DC_S, DC_S, DC_S, DC_S);
    if (context_id == 0U) {
        //TBD
    }
    if (enable == 0U) {
        //TBD
    }
    return IK_ERR_0004;
}

uint32 img_ctx_ivd_get_static_bpc_enable_info(const uint32 context_id, uint32 *p_enable)
{
    amba_ik_system_print_str_5("[IK] get_static_bpc_enable_info() is invalid!", DC_S, DC_S, DC_S, DC_S, DC_S);
    if (context_id == 0U) {
        //TBD
    }
    if (p_enable != NULL) {
        //TBD
        *p_enable = 0;
    }
    return IK_ERR_0004;
}

uint32 img_ctx_set_static_bpc(const uint32 context_id, const ik_static_bad_pxl_cor_t *p_static_bpc)
{
    uint32 rval = IK_OK;
    uint32 sbp_pitch;
    amba_ik_context_entity_t *p_ctx = NULL;
    const ik_debug_check_func_t* p_check_func = ik_ctx_get_debug_check_func();
    if(p_check_func != NULL) {
        rval = p_check_func->static_bpc(p_static_bpc);
    }
    rval |= img_ctx_get_context(context_id, &p_ctx);
    if ((rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        (void)amba_ik_system_lock_mutex(&p_ctx->organization.mutex, IK_WAIT_FOREVER);
        (void)amba_ik_system_memcpy(&p_ctx->filters.input_param.static_bpc, p_static_bpc, sizeof(ik_static_bad_pxl_cor_t));
        ctx_raise_update_flag(&p_ctx->filters.update_flags.iso.static_bpc_updated);
        p_ctx->filters.input_param.ctx_buf.sbp_internal_mode_flag = 0;
        // user SBP table clean cache at Set period.
        sbp_pitch = (((p_ctx->filters.input_param.static_bpc.vin_sensor_geo.width + 255U) >>8U) <<5U);
        amba_ik_system_clean_cache(p_ctx->filters.input_param.static_bpc.calib_sbp_info.sbp_buffer, sbp_pitch*p_ctx->filters.input_param.static_bpc.vin_sensor_geo.height);
        (void)amba_ik_system_unlock_mutex(&p_ctx->organization.mutex);
    } else {
        if(p_ctx->organization.initial_flag == 0U) {
            amba_ik_system_print_uint32_5("[IK] context_id = %d was not initial", context_id, DC_U, DC_U, DC_U, DC_U);
            rval = IK_ERR_0002;
        } else {
            amba_ik_system_print_str_5("[IK] can't get context", DC_S, DC_S, DC_S, DC_S, DC_S);
        }
    }
    return rval;
}

uint32 img_ctx_set_static_bpc_internal(const uint32 context_id, const ik_static_bad_pixel_correction_internal_t *static_bpc_internal)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;
    const ik_debug_check_func_t* p_check_func = ik_ctx_get_debug_check_func();
    if(p_check_func != NULL) {
        rval = p_check_func->static_bpc_internal(static_bpc_internal);
    }
    rval |= img_ctx_get_context(context_id, &p_ctx);
    if ((rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        (void)amba_ik_system_lock_mutex(&p_ctx->organization.mutex, IK_WAIT_FOREVER);
        (void)amba_ik_system_memcpy(&p_ctx->filters.input_param.sbp_internal, static_bpc_internal, sizeof(ik_static_bad_pixel_correction_internal_t));
        ctx_raise_update_flag(&p_ctx->filters.update_flags.iso.static_bpc_internal_updated);
        p_ctx->filters.input_param.ctx_buf.sbp_internal_mode_flag = 1;
        // user SBP table clean cache at Set period.
        amba_ik_system_clean_cache(p_ctx->filters.input_param.sbp_internal.p_map, p_ctx->filters.input_param.sbp_internal.pixel_map_pitch*p_ctx->filters.input_param.sbp_internal.pixel_map_height);
        (void)amba_ik_system_unlock_mutex(&p_ctx->organization.mutex);
    } else {
        if(p_ctx->organization.initial_flag == 0U) {
            amba_ik_system_print_uint32_5("[IK] context_id = %d was not initial", context_id, DC_U, DC_U, DC_U, DC_U);
            rval = IK_ERR_0002;
        } else {
            amba_ik_system_print_str_5("[IK] can't get context", DC_S, DC_S, DC_S, DC_S, DC_S);
        }
    }
    return rval;
}

uint32 img_ctx_set_static_bpc_enable_info(const uint32 context_id, const uint32 enable)
{
    amba_ik_context_entity_t *p_ctx = NULL;
    uint32 rval = img_ctx_get_context(context_id, &p_ctx);
    if(enable > 1U) {
        rval = IK_ERR_0008;
    }
    if ((rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        (void)amba_ik_system_lock_mutex(&p_ctx->organization.mutex, IK_WAIT_FOREVER);
        (void)amba_ik_system_memcpy(&p_ctx->filters.input_param.sbp_enable, &enable, sizeof(int32));
        ctx_raise_update_flag(&p_ctx->filters.update_flags.iso.static_bpc_updated);
        (void)amba_ik_system_unlock_mutex(&p_ctx->organization.mutex);
    } else {
        if(p_ctx->organization.initial_flag == 0U) {
            amba_ik_system_print_uint32_5("[IK] context_id = %d was not initial", context_id, DC_U, DC_U, DC_U, DC_U);
            rval = IK_ERR_0002;
        } else {
            amba_ik_system_print_str_5("[IK] can't get context", DC_S, DC_S, DC_S, DC_S, DC_S);
        }
    }
    return rval;
}

uint32 img_ctx_get_static_bpc_enable_info(const uint32 context_id, uint32 *p_enable)
{
    amba_ik_context_entity_t *p_ctx = NULL;
    uint32 rval = img_ctx_get_context(context_id, &p_ctx);
    if ((rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        (void)amba_ik_system_lock_mutex(&p_ctx->organization.mutex, IK_WAIT_FOREVER);
        (void)amba_ik_system_memcpy(p_enable,  &p_ctx->filters.input_param.sbp_enable, sizeof(int32));
        (void)amba_ik_system_unlock_mutex(&p_ctx->organization.mutex);
    } else {
        if(p_ctx->organization.initial_flag == 0U) {
            amba_ik_system_print_uint32_5("[IK] context_id = %d was not initial", context_id, DC_U, DC_U, DC_U, DC_U);
            rval = IK_ERR_0002;
        } else {
            amba_ik_system_print_str_5("[IK] can't get context", DC_S, DC_S, DC_S, DC_S, DC_S);
        }
    }
    return rval;
}

uint32 img_ctx_ivd_set_vignette(const uint32 context_id, const ik_vignette_t *p_vignette)
{
    amba_ik_system_print_str_5("[IK] please set correct set_vignette_compensation api", DC_S, DC_S, DC_S, DC_S, DC_S);
    if(context_id == 0U) {
        //TBD
    }
    if(p_vignette == NULL) {
        //TBD
    }
    return IK_ERR_0004;
}

uint32 img_ctx_ivd_set_vignette_enable_info(const uint32 context_id, const uint32 enable)
{
    amba_ik_system_print_str_5("[IK] set_vignette_compensation_enable_info() is invalid!", DC_S, DC_S, DC_S, DC_S, DC_S);
    if(context_id == 0U) {
        //TBD
    }
    if(enable == 0U) {
        //TBD
    }
    return IK_ERR_0004;
}

uint32 img_ctx_ivd_get_vignette_enable_info(const uint32 context_id, uint32 *p_enable)
{
    amba_ik_system_print_str_5("[IK] get_vignette_compensation_enable_info() is invalid!", DC_S, DC_S, DC_S, DC_S, DC_S);
    if(context_id == 0U) {
        //TBD
    }
    if(p_enable != NULL) {
        //TBD
        *p_enable = 0U;
    }
    return IK_ERR_0004;
}

uint32 img_ctx_set_vignette(const uint32 context_id, const ik_vignette_t *p_vignette)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;
    const ik_debug_check_func_t* p_check_func = ik_ctx_get_debug_check_func();
    if(p_check_func != NULL) {
        rval = p_check_func->vignette_info(p_vignette);
    }
    if(rval == IK_OK) {
        rval |= img_ctx_get_context(context_id, &p_ctx);
        if ((rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
            (void)amba_ik_system_lock_mutex(&p_ctx->organization.mutex, IK_WAIT_FOREVER);
            (void)amba_ik_system_memcpy(&p_ctx->filters.input_param.vignette_compensation, p_vignette, sizeof(ik_vignette_t));
            ctx_raise_update_flag(&p_ctx->filters.update_flags.iso.vignette_compensation_updated);
            (void)amba_ik_system_unlock_mutex(&p_ctx->organization.mutex);
        } else {
            if(p_ctx->organization.initial_flag == 0U) {
                amba_ik_system_print_uint32_5("[IK] context_id = %d was not initial", context_id, DC_U, DC_U, DC_U, DC_U);
                rval = IK_ERR_0002;
            } else {
                amba_ik_system_print_str_5("[IK] can't get context", DC_S, DC_S, DC_S, DC_S, DC_S);
            }
        }
    }
    return rval;
}

uint32 img_ctx_set_vignette_enable_info(const uint32 context_id, const uint32 enable)
{
    amba_ik_context_entity_t *p_ctx = NULL;
    uint32 rval = img_ctx_get_context(context_id, &p_ctx);
    if(enable > 1U) {
        amba_ik_system_print_uint32_5("[IK][ERROR] img_ctx_set_vignette_enable_info enable %d > 1\n",enable, DC_U, DC_U, DC_U, DC_U);
        rval = IK_ERR_0008;
    }
    if ((rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        (void)amba_ik_system_lock_mutex(&p_ctx->organization.mutex, IK_WAIT_FOREVER);
        (void)amba_ik_system_memcpy(&p_ctx->filters.input_param.vig_enable, &enable, sizeof(uint32));
        ctx_raise_update_flag(&p_ctx->filters.update_flags.iso.vignette_compensation_updated);
        (void)amba_ik_system_unlock_mutex(&p_ctx->organization.mutex);
    } else {
        if(p_ctx->organization.initial_flag == 0U) {
            amba_ik_system_print_uint32_5("[IK] context_id = %d was not initial", context_id, DC_U, DC_U, DC_U, DC_U);
            rval = IK_ERR_0002;
        } else {
            amba_ik_system_print_str_5("[IK] can't get context", DC_S, DC_S, DC_S, DC_S, DC_S);
        }
    }
    return rval;
}

uint32 img_ctx_get_vignette_enable_info(const uint32 context_id, uint32 *p_enable)
{
    amba_ik_context_entity_t *p_ctx = NULL;
    uint32 rval = img_ctx_get_context(context_id, &p_ctx);
    if ((rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        (void)amba_ik_system_lock_mutex(&p_ctx->organization.mutex, IK_WAIT_FOREVER);
        (void)amba_ik_system_memcpy(p_enable, &p_ctx->filters.input_param.vig_enable, sizeof(uint32));
        (void)amba_ik_system_unlock_mutex(&p_ctx->organization.mutex);
    } else {
        if(p_ctx->organization.initial_flag == 0U) {
            amba_ik_system_print_uint32_5("[IK] context_id = %d was not initial", context_id, DC_U, DC_U, DC_U, DC_U);
            rval = IK_ERR_0002;
        } else {
            amba_ik_system_print_str_5("[IK] can't get context", DC_S, DC_S, DC_S, DC_S, DC_S);
        }
    }
    return rval;
}

uint32 img_ctx_ivd_set_calib_warp_info(const uint32 context_id, const ik_warp_info_t *p_calib_warp_info)
{
    amba_ik_system_print_str_5("[IK] set_calib_warp_info() is invalid!", DC_S, DC_S, DC_S, DC_S, DC_S);
    if (context_id == 0U) {
        //TBD
    }
    if (p_calib_warp_info == NULL) {
        //TBD
    }
    return IK_ERR_0004;
}

uint32 img_ctx_set_calib_warp_info(const uint32 context_id, const ik_warp_info_t *p_calib_warp_info)
{
    amba_ik_context_entity_t *p_ctx = NULL;
    uint32 rval = IK_OK;
    const ik_debug_check_func_t* p_check_func = ik_ctx_get_debug_check_func();
    if(p_check_func != NULL) {
        rval = p_check_func->warp_info(p_calib_warp_info);
    }
    rval |= img_ctx_get_context(context_id, &p_ctx);
    if ((rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        (void)amba_ik_system_lock_mutex(&p_ctx->organization.mutex, IK_WAIT_FOREVER);
        (void)amba_ik_system_memcpy(&p_ctx->filters.input_param.calib_warp_info, p_calib_warp_info, sizeof(ik_warp_info_t));
        ctx_raise_update_flag(&p_ctx->filters.update_flags.iso.calib_warp_info_updated);
        p_ctx->filters.input_param.ctx_buf.warp_internal_mode_flag = 0;
        (void)amba_ik_system_unlock_mutex(&p_ctx->organization.mutex);
    } else {
        if(p_ctx->organization.initial_flag == 0U) {
            amba_ik_system_print_uint32_5("[IK] context_id = %d was not initial", context_id, DC_U, DC_U, DC_U, DC_U);
            rval = IK_ERR_0002;
        } else {
            amba_ik_system_print_str_5("[IK] can't get context", DC_S, DC_S, DC_S, DC_S, DC_S);
        }
    }
    return rval;
}

uint32 img_ctx_ivd_set_2nd_calib_warp_info(const uint32 context_id, const ik_warp_info_t *p_calib_warp_info_2nd)
{
    amba_ik_system_print_str_5("[IK] set_2nd_calib_warp_info() is invalid!", DC_S, DC_S, DC_S, DC_S, DC_S);
    if (context_id == 0U) {
        //TBD
    }
    if (p_calib_warp_info_2nd == NULL) {
        //TBD
    }
    return IK_ERR_0004;
}

uint32 img_ctx_set_2nd_calib_warp_info(const uint32 context_id, const ik_warp_info_t *p_calib_warp_info_2nd)
{
    amba_ik_context_entity_t *p_ctx = NULL;
    uint32 rval = IK_OK;
    const ik_debug_check_func_t* p_check_func = ik_ctx_get_debug_check_func();
    if(p_check_func != NULL) {
        rval = p_check_func->warp_info_2nd(p_calib_warp_info_2nd);
    }
    rval |= img_ctx_get_context(context_id, &p_ctx);
    if ((rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        (void)amba_ik_system_lock_mutex(&p_ctx->organization.mutex, IK_WAIT_FOREVER);
        (void)amba_ik_system_memcpy(&p_ctx->filters.input_param.calib_warp_info_2nd, p_calib_warp_info_2nd, sizeof(ik_warp_info_t));
        ctx_raise_update_flag(&p_ctx->filters.update_flags.iso.calib_warp_2nd_info_updated);
        p_ctx->filters.input_param.ctx_buf.warp_2nd_internal_mode_flag = 0;
        (void)amba_ik_system_unlock_mutex(&p_ctx->organization.mutex);
    } else {
        if(p_ctx->organization.initial_flag == 0U) {
            amba_ik_system_print_uint32_5("[IK] context_id = %d was not initial", context_id, DC_U, DC_U, DC_U, DC_U);
            rval = IK_ERR_0002;
        } else {
            amba_ik_system_print_str_5("[IK] can't get context", DC_S, DC_S, DC_S, DC_S, DC_S);
        }
    }
    return rval;
}

uint32 img_ctx_set_warp_bind_info(const uint32 context_id, const ik_bind_info_t *p_warp_bind_info)
{
    amba_ik_context_entity_t *p_ctx = NULL;
    const ik_debug_check_func_t* p_check_func = ik_ctx_get_debug_check_func();
    uint32 rval = IK_OK;
    if(p_check_func != NULL) {
        rval = p_check_func->warp_bind_info(p_warp_bind_info);
    }
    rval |= img_ctx_get_context(context_id, &p_ctx);
    if ((rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        (void)amba_ik_system_lock_mutex(&p_ctx->organization.mutex, IK_WAIT_FOREVER);
        (void)amba_ik_system_memcpy(&p_ctx->filters.input_param.warp_bind_info, p_warp_bind_info, sizeof(ik_bind_info_t));
        ctx_raise_update_flag(&p_ctx->filters.update_flags.iso.warp_bind_info_updated);
        (void)amba_ik_system_unlock_mutex(&p_ctx->organization.mutex);
    } else {
        if(p_ctx->organization.initial_flag == 0U) {
            amba_ik_system_print_uint32_5("[IK] context_id = %d was not initial", context_id, DC_U, DC_U, DC_U, DC_U);
            rval = IK_ERR_0002;
        } else {
            amba_ik_system_print_str_5("[IK] can't get context", DC_S, DC_S, DC_S, DC_S, DC_S);
        }
    }
    return rval;
}

uint32 img_ctx_ivd_set_calib_ca_warp_info(const uint32 context_id, const ik_cawarp_info_t *p_calib_ca_warp_info)
{
    amba_ik_system_print_str_5("[IK] set_calib_ca_warp_info() is invalid!", DC_S, DC_S, DC_S, DC_S, DC_S);
    if (context_id == 0U) {
        //TBD
    }
    if (p_calib_ca_warp_info == NULL) {
        //TBD
    }
    return IK_ERR_0004;
}

uint32 img_ctx_set_calib_ca_warp_info(const uint32 context_id, const ik_cawarp_info_t *p_calib_ca_warp_info)
{
    amba_ik_context_entity_t *p_ctx = NULL;
    uint32 rval = IK_OK;
    const ik_debug_check_func_t* p_check_func = ik_ctx_get_debug_check_func();
    if(p_check_func != NULL) {
        rval = p_check_func->cawarp(p_calib_ca_warp_info);
    }
    rval |= img_ctx_get_context(context_id, &p_ctx);
    if ((rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        (void)amba_ik_system_lock_mutex(&p_ctx->organization.mutex, IK_WAIT_FOREVER);
        (void)amba_ik_system_memcpy(&p_ctx->filters.input_param.calib_ca_warp_info, p_calib_ca_warp_info, sizeof(ik_cawarp_info_t));
#if SUPPORT_EXTEND_TABLE
        p_ctx->filters.input_param.calib_ca_warp_info.p_cawarp_red = &p_ctx->filters.input_param.ca_red_tbl[0];
        p_ctx->filters.input_param.calib_ca_warp_info.p_cawarp_blue = &p_ctx->filters.input_param.ca_blue_tbl[0];
        (void)amba_ik_system_memcpy(p_ctx->filters.input_param.calib_ca_warp_info.p_cawarp_red, p_calib_ca_warp_info->p_cawarp_red, sizeof(ik_grid_point_t)*2*2*32*48);
        (void)amba_ik_system_memcpy(p_ctx->filters.input_param.calib_ca_warp_info.p_cawarp_blue , p_calib_ca_warp_info->p_cawarp_blue, sizeof(ik_grid_point_t)*2*2*32*48);
#endif
        ctx_raise_update_flag(&p_ctx->filters.update_flags.iso.calib_ca_warp_info_updated);
        p_ctx->filters.input_param.ctx_buf.cawarp_internal_mode_flag = 0;
        (void)amba_ik_system_unlock_mutex(&p_ctx->organization.mutex);
    } else {
        if(p_ctx->organization.initial_flag == 0U) {
            amba_ik_system_print_uint32_5("[IK] context_id = %d was not initial", context_id, DC_U, DC_U, DC_U, DC_U);
            rval = IK_ERR_0002;
        } else {
            amba_ik_system_print_str_5("[IK] can't get context", DC_S, DC_S, DC_S, DC_S, DC_S);
        }
    }

    return rval;
}

uint32 img_ctx_ivd_set_dzoom_info(const uint32 context_id, const ik_dzoom_info_t *p_dzoom_info)
{
    amba_ik_system_print_str_5("[IK] set_dzoom_info() is invalid!", DC_S, DC_S, DC_S, DC_S, DC_S);
    if (context_id == 0U) {
        //TBD
    }
    if (p_dzoom_info == NULL) {
        //TBD
    }
    return IK_ERR_0004;
}

uint32 img_ctx_set_dzoom_info(const uint32 context_id, const ik_dzoom_info_t *p_dzoom_info)
{
    amba_ik_context_entity_t *p_ctx = NULL;
    uint32 rval = IK_OK;
    const ik_debug_check_func_t* p_check_func = ik_ctx_get_debug_check_func();
    if(p_check_func != NULL) {
        rval = p_check_func->dzoom_info(p_dzoom_info);
    }
    rval |= img_ctx_get_context(context_id, &p_ctx);
    if ((rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        (void)amba_ik_system_lock_mutex(&p_ctx->organization.mutex, IK_WAIT_FOREVER);
        (void)amba_ik_system_memcpy(&p_ctx->filters.input_param.dzoom_info, p_dzoom_info, sizeof(ik_dzoom_info_t));
        ctx_raise_update_flag(&p_ctx->filters.update_flags.iso.dzoom_info_updated);
        p_ctx->filters.input_param.ctx_buf.dzoom_internal_mode_flag = 0;
        (void)amba_ik_system_unlock_mutex(&p_ctx->organization.mutex);
    } else {
        if(p_ctx->organization.initial_flag == 0U) {
            amba_ik_system_print_uint32_5("[IK] context_id = %d was not initial", context_id, DC_U, DC_U, DC_U, DC_U);
            rval = IK_ERR_0002;
        } else {
            amba_ik_system_print_str_5("[IK] can't get context", DC_S, DC_S, DC_S, DC_S, DC_S);
        }
    }
    return rval;
}

uint32 img_ctx_ivd_set_hdr_shift_info_exp1(const uint32 context_id, const ik_hdr_eis_info_t *p_hdr_sft_info)
{
    amba_ik_system_print_str_5("[IK] set_hdr_shift_info_exp1() is invalid!", DC_S, DC_S, DC_S, DC_S, DC_S);
    if (context_id == 0U) {
        //TBD
    }
    if (p_hdr_sft_info == NULL) {
        //TBD
    }
    return IK_ERR_0004;
}

uint32 img_ctx_set_hdr_shift_info_exp1(const uint32 context_id, const ik_hdr_eis_info_t *p_hdr_sft_info)
{
    amba_ik_context_entity_t *p_ctx = NULL;
    uint32 rval = IK_OK;
    const ik_debug_check_func_t* p_check_func = ik_ctx_get_debug_check_func();
    if(p_check_func != NULL) {
        rval = p_check_func->hdr_shift(p_hdr_sft_info);
    }
    rval |= img_ctx_get_context(context_id, &p_ctx);
    if ((rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        (void)amba_ik_system_lock_mutex(&p_ctx->organization.mutex, IK_WAIT_FOREVER);
        (void)amba_ik_system_memcpy(&p_ctx->filters.input_param.hdr_eis_sft_exp1, p_hdr_sft_info, sizeof(ik_hdr_eis_info_t));
        ctx_raise_update_flag(&p_ctx->filters.update_flags.iso.hdr_eis_shift_info_updated);
        (void)amba_ik_system_unlock_mutex(&p_ctx->organization.mutex);
    } else {
        if(p_ctx->organization.initial_flag == 0U) {
            amba_ik_system_print_uint32_5("[IK] context_id = %d was not initial", context_id, DC_U, DC_U, DC_U, DC_U);
            rval = IK_ERR_0002;
        } else {
            amba_ik_system_print_str_5("[IK] can't get context", DC_S, DC_S, DC_S, DC_S, DC_S);
        }
    }
    return rval;
}

uint32 img_ctx_ivd_set_hdr_shift_info_exp2(const uint32 context_id, const ik_hdr_eis_info_t *p_hdr_sft_info)
{
    amba_ik_system_print_str_5("[IK] set_hdr_shift_info_exp2() is invalid!", DC_S, DC_S, DC_S, DC_S, DC_S);
    if (context_id == 0U) {
        //TBD
    }
    if (p_hdr_sft_info == NULL) {
        //TBD
    }
    return IK_ERR_0004;
}

uint32 img_ctx_set_hdr_shift_info_exp2(const uint32 context_id, const ik_hdr_eis_info_t *p_hdr_sft_info)
{
    amba_ik_context_entity_t *p_ctx = NULL;
    uint32 rval = IK_OK;
    const ik_debug_check_func_t* p_check_func = ik_ctx_get_debug_check_func();
    if(p_check_func != NULL) {
        rval = p_check_func->hdr_shift(p_hdr_sft_info);
    }
    rval |= img_ctx_get_context(context_id, &p_ctx);
    if ((rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        (void)amba_ik_system_lock_mutex(&p_ctx->organization.mutex, IK_WAIT_FOREVER);
        (void)amba_ik_system_memcpy(&p_ctx->filters.input_param.hdr_eis_sft_exp2, p_hdr_sft_info, sizeof(ik_hdr_eis_info_t));
        ctx_raise_update_flag(&p_ctx->filters.update_flags.iso.hdr_eis_shift_info_updated);
        (void)amba_ik_system_unlock_mutex(&p_ctx->organization.mutex);
    } else {
        if(p_ctx->organization.initial_flag == 0U) {
            amba_ik_system_print_uint32_5("[IK] context_id = %d was not initial", context_id, DC_U, DC_U, DC_U, DC_U);
            rval = IK_ERR_0002;
        } else {
            amba_ik_system_print_str_5("[IK] can't get context", DC_S, DC_S, DC_S, DC_S, DC_S);
        }
    }
    return rval;
}

uint32 img_ctx_ivd_set_warp_enable_info(const uint32 context_id, const uint32 enable)
{
    amba_ik_system_print_str_5("[IK] set_warp_enable_info() is invalid!", DC_S, DC_S, DC_S, DC_S, DC_S);
    if (context_id == 0U) {
        //TBD
    }
    if (enable == 0U) {
        //TBD
    }
    return IK_ERR_0004;
}

uint32 img_ctx_set_warp_enable_info(const uint32 context_id, const uint32 enable)
{
    amba_ik_context_entity_t *p_ctx = NULL;
    uint32 rval = IK_OK;
    if(enable > 1U) {
        amba_ik_system_print_str_5("[IK][ERROR] set_warp_enable_info() enable > 1 !", DC_S, DC_S, DC_S, DC_S, DC_S);
        rval |= IK_ERR_0008;
    }
    rval |= img_ctx_get_context(context_id, &p_ctx);
    if ((rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        (void)amba_ik_system_lock_mutex(&p_ctx->organization.mutex, IK_WAIT_FOREVER);
        (void)amba_ik_system_memcpy(&p_ctx->filters.input_param.warp_enable, &enable, sizeof(int32));
        ctx_raise_update_flag(&p_ctx->filters.update_flags.iso.warp_enable_updated);
        (void)amba_ik_system_unlock_mutex(&p_ctx->organization.mutex);
    } else {
        if(p_ctx->organization.initial_flag == 0U) {
            amba_ik_system_print_uint32_5("[IK] context_id = %d was not initial", context_id, DC_U, DC_U, DC_U, DC_U);
            rval = IK_ERR_0002;
        } else {
            amba_ik_system_print_str_5("[IK] can't get context", DC_S, DC_S, DC_S, DC_S, DC_S);
        }
    }

    return rval;
}

uint32 img_ctx_ivd_set_warp_2nd_enable_info(const uint32 context_id, const uint32 enable)
{
    amba_ik_system_print_str_5("[IK] set_warp_2nd_enable_info() is invalid!", DC_S, DC_S, DC_S, DC_S, DC_S);
    if (context_id == 0U) {
        //TBD
    }
    if (enable == 0U) {
        //TBD
    }
    return IK_ERR_0004;
}

uint32 img_ctx_set_warp_2nd_enable_info(const uint32 context_id, const uint32 enable)
{
    amba_ik_context_entity_t *p_ctx = NULL;
    uint32 rval = IK_OK;
    if(enable > 1U) {
        amba_ik_system_print_str_5("[IK][ERROR] set_warp_2nd_enable_info() enable > 1 !", DC_S, DC_S, DC_S, DC_S, DC_S);
        rval |= IK_ERR_0008;
    }
    rval |= img_ctx_get_context(context_id, &p_ctx);
    if ((rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        (void)amba_ik_system_lock_mutex(&p_ctx->organization.mutex, IK_WAIT_FOREVER);
        (void)amba_ik_system_memcpy(&p_ctx->filters.input_param.warp_enable_2nd, &enable, sizeof(int32));
        ctx_raise_update_flag(&p_ctx->filters.update_flags.iso.warp_enable_2nd_updated);
        (void)amba_ik_system_unlock_mutex(&p_ctx->organization.mutex);
    } else {
        if(p_ctx->organization.initial_flag == 0U) {
            amba_ik_system_print_uint32_5("[IK] context_id = %d was not initial", context_id, DC_U, DC_U, DC_U, DC_U);
            rval = IK_ERR_0002;
        } else {
            amba_ik_system_print_str_5("[IK] can't get context", DC_S, DC_S, DC_S, DC_S, DC_S);
        }
    }

    return rval;
}

uint32 img_ctx_ivd_set_cawarp_enable_info(const uint32 context_id, const uint32 enable)
{
    amba_ik_system_print_str_5("[IK] set_cawarp_enable_info() is invalid!", DC_S, DC_S, DC_S, DC_S, DC_S);
    if (context_id == 0U) {
        //TBD
    }
    if (enable == 0U) {
        //TBD
    }
    return IK_ERR_0004;
}

uint32 img_ctx_set_cawarp_enable_info(const uint32 context_id, const uint32 enable)
{
    amba_ik_context_entity_t *p_ctx = NULL;
    uint32 rval = IK_OK;
    if(enable > 1U) {
        amba_ik_system_print_str_5("[IK][ERROR] set_cawarp_enable_info() enable > 1 !", DC_S, DC_S, DC_S, DC_S, DC_S);
        rval |= IK_ERR_0008;
    }
    rval |= img_ctx_get_context(context_id, &p_ctx);
    if ((rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        (void)amba_ik_system_lock_mutex(&p_ctx->organization.mutex, IK_WAIT_FOREVER);
        (void)amba_ik_system_memcpy(&p_ctx->filters.input_param.cawarp_enable, &enable, sizeof(int32));
        ctx_raise_update_flag(&p_ctx->filters.update_flags.iso.cawarp_enable_updated);
        (void)amba_ik_system_unlock_mutex(&p_ctx->organization.mutex);
    } else {
        if(p_ctx->organization.initial_flag == 0U) {
            amba_ik_system_print_uint32_5("[IK] context_id = %d was not initial", context_id, DC_U, DC_U, DC_U, DC_U);
            rval = IK_ERR_0002;
        } else {
            amba_ik_system_print_str_5("[IK] can't get context", DC_S, DC_S, DC_S, DC_S, DC_S);
        }
    }

    return rval;
}

uint32 img_ctx_ivd_set_dummy_win_margin_range_info(const uint32 context_id, const ik_dummy_margin_range_t *p_dmy_range)
{
    amba_ik_system_print_str_5("[IK] set_dummy_win_margin_rangee_info() is invalid!", DC_S, DC_S, DC_S, DC_S, DC_S);
    if (context_id == 0U) {
        //TBD
    }
    if (p_dmy_range == NULL) {
        //TBD
    }
    return IK_ERR_0004;
}

uint32 img_ctx_set_dummy_win_margin_range_info(const uint32 context_id, const ik_dummy_margin_range_t *p_dmy_range)
{
    amba_ik_context_entity_t *p_ctx = NULL;
    const ik_debug_check_func_t* p_check_func = ik_ctx_get_debug_check_func();
    uint32 rval = IK_OK;
    rval |= img_ctx_get_context(context_id, &p_ctx);
    if((rval == IK_OK)&&(p_check_func != NULL)) {
        rval = p_check_func->dummy_margin_range(
                   p_dmy_range,
                   ((p_ctx->filters.input_param.stitching_info.enable != 0)&&(p_ctx->filters.input_param.stitching_info.tile_num_x > 1)) ? 1U : 0U);
    }
    if ((rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        (void)amba_ik_system_lock_mutex(&p_ctx->organization.mutex, IK_WAIT_FOREVER);
        (void)amba_ik_system_memcpy(&p_ctx->filters.input_param.dmy_range, p_dmy_range, sizeof(ik_dummy_margin_range_t));
        ctx_raise_update_flag(&p_ctx->filters.update_flags.iso.dmy_range_updated);
        (void)amba_ik_system_unlock_mutex(&p_ctx->organization.mutex);
    } else {
        if(p_ctx->organization.initial_flag == 0U) {
            amba_ik_system_print_uint32_5("[IK] context_id = %d was not initial", context_id, DC_U, DC_U, DC_U, DC_U);
            rval = IK_ERR_0002;
        } else {
            amba_ik_system_print_str_5("[IK] can't get context", DC_S, DC_S, DC_S, DC_S, DC_S);
        }
    }

    return rval;
}

uint32 img_ctx_ivd_set_dmy_margin_enable_info(const uint32 context_id, const uint32 enable)
{
    amba_ik_system_print_str_5("[IK] set_dmy_margin_enable_info() is invalid!", DC_S, DC_S, DC_S, DC_S, DC_S);
    if (context_id == 0U) {
        //TBD
    }
    if (enable == 0U) {
        //TBD
    }
    return IK_ERR_0004;
}

uint32 img_ctx_ivd_set_flip_mode(const uint32 context_id, const uint32 mode)
{
    amba_ik_system_print_str_5("[IK] set_flip_input_mode_info() is invalid!", DC_S, DC_S, DC_S, DC_S, DC_S);
    if (context_id == 0U) {
        //TBD
    }
    if (mode == 0U) {
        //TBD
    }
    return IK_ERR_0004;
}

uint32 img_ctx_set_flip_mode(const uint32 context_id, const uint32 mode)
{
    amba_ik_context_entity_t *p_ctx = NULL;
    uint32 rval = IK_OK;
    const ik_debug_check_func_t* p_check_func = ik_ctx_get_debug_check_func();
    if(p_check_func != NULL) {
        rval = p_check_func->flip_mode(mode);
    }
    rval |= img_ctx_get_context(context_id, &p_ctx);
    if ((rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        (void)amba_ik_system_lock_mutex(&p_ctx->organization.mutex, IK_WAIT_FOREVER);
        (void)amba_ik_system_memcpy(&p_ctx->filters.input_param.flip_mode, &mode, sizeof(int32));
        ctx_raise_update_flag(&p_ctx->filters.update_flags.iso.flip_mode_updated);
        (void)amba_ik_system_unlock_mutex(&p_ctx->organization.mutex);
    } else {
        if(p_ctx->organization.initial_flag == 0U) {
            amba_ik_system_print_uint32_5("[IK] context_id = %d was not initial", context_id, DC_U, DC_U, DC_U, DC_U);
            rval = IK_ERR_0002;
        } else {
            amba_ik_system_print_str_5("[IK] can't get context", DC_S, DC_S, DC_S, DC_S, DC_S);
        }
    }

    return rval;
}

uint32 img_ctx_ivd_set_warp_buffer_info(const uint32 context_id, const ik_warp_buffer_info_t *p_warp_buf_info)
{
    amba_ik_system_print_str_5("[IK] set_warp_buffer_info() is invalid!", DC_S, DC_S, DC_S, DC_S, DC_S);
    if (context_id == 0U) {
        //TBD
    }
    if (p_warp_buf_info == NULL) {
        //TBD
    }
    return IK_ERR_0004;
}

uint32 img_ctx_set_warp_buffer_info(const uint32 context_id, const ik_warp_buffer_info_t *p_warp_buf_info)
{
    amba_ik_context_entity_t *p_ctx = NULL;
    uint32 rval = IK_OK;
    const ik_debug_check_func_t* p_check_func = ik_ctx_get_debug_check_func();
    if(p_check_func != NULL) {
        rval = p_check_func->warp_buffer_info(p_warp_buf_info);
    }
    rval |= img_ctx_get_context(context_id, &p_ctx);
    if ((rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        (void)amba_ik_system_lock_mutex(&p_ctx->organization.mutex, IK_WAIT_FOREVER);
        (void)amba_ik_system_memcpy(&p_ctx->filters.input_param.warp_buf_info, p_warp_buf_info, sizeof(ik_warp_buffer_info_t));
        ctx_raise_update_flag(&p_ctx->filters.update_flags.iso.warp_buf_info_updated);
        (void)amba_ik_system_unlock_mutex(&p_ctx->organization.mutex);
    } else {
        if(p_ctx->organization.initial_flag == 0U) {
            amba_ik_system_print_uint32_5("[IK] context_id = %d was not initial", context_id, DC_U, DC_U, DC_U, DC_U);
            rval = IK_ERR_0002;
        } else {
            amba_ik_system_print_str_5("[IK] can't get context", DC_S, DC_S, DC_S, DC_S, DC_S);
        }
    }

    return rval;
}

uint32 img_ctx_ivd_set_overlap_x(const uint32 context_id, const ik_overlap_x_t *p_overlap_x)
{
    amba_ik_system_print_str_5("[IK] set_overlap_x() is invalid!", DC_S, DC_S, DC_S, DC_S, DC_S);
    if (context_id == 0U) {
        //TBD
    }
    if (p_overlap_x == NULL) {
        //TBD
    }
    return IK_ERR_0004;
}

uint32 img_ctx_set_overlap_x(const uint32 context_id, const ik_overlap_x_t *p_overlap_x)
{
    amba_ik_context_entity_t *p_ctx = NULL;
    uint32 rval = IK_OK;
    const ik_debug_check_func_t* p_check_func = ik_ctx_get_debug_check_func();
    if(p_check_func != NULL) {
        rval = p_check_func->overlap_x(p_overlap_x);
    }
    rval |= img_ctx_get_context(context_id, &p_ctx);
    if ((rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        (void)amba_ik_system_lock_mutex(&p_ctx->organization.mutex, IK_WAIT_FOREVER);
        (void)amba_ik_system_memcpy(&p_ctx->filters.input_param.overlap_x, p_overlap_x, sizeof(ik_overlap_x_t));
        ctx_raise_update_flag(&p_ctx->filters.update_flags.iso.overlap_x_updated);
        (void)amba_ik_system_unlock_mutex(&p_ctx->organization.mutex);
    } else {
        if(p_ctx->organization.initial_flag == 0U) {
            amba_ik_system_print_uint32_5("[IK] context_id = %d was not initial", context_id, DC_U, DC_U, DC_U, DC_U);
            rval = IK_ERR_0002;
        } else {
            amba_ik_system_print_str_5("[IK] can't get context", DC_S, DC_S, DC_S, DC_S, DC_S);
        }
    }

    return rval;
}

uint32 img_ctx_ivd_set_stitching_info(const uint32 context_id, const ik_stitch_info_t *p_stitch_info)
{
    amba_ik_system_print_str_5("[IK] set_stitching_info() is invalid!", DC_S, DC_S, DC_S, DC_S, DC_S);
    if (context_id == 0U) {
        //TBD
    }
    if (p_stitch_info == NULL) {
        //TBD
    }
    return IK_ERR_0004;
}

uint32 img_ctx_set_stitching_info(const uint32 context_id, const ik_stitch_info_t *p_stitch_info)
{
    amba_ik_context_entity_t *p_ctx = NULL;
    uint32 rval = IK_OK;
    const ik_debug_check_func_t* p_check_func = ik_ctx_get_debug_check_func();
    if(p_check_func != NULL) {
        rval = p_check_func->stitching_info(p_stitch_info);
    }
    if(rval == IK_OK) {
        rval |= img_ctx_get_context(context_id, &p_ctx);
        if(p_ctx->filters.update_flags.iso.is_1st_frame == 0U) {
            amba_ik_system_print_uint32_5("[IK][ERROR] Invalid ik_set_stitching_info run-time update, context_id = %d\n", context_id, DC_U, DC_U, DC_U, DC_U);
            rval |= IK_ERR_0008;
        } else {
            (void)amba_ik_system_lock_mutex(&p_ctx->organization.mutex, IK_WAIT_FOREVER);
            (void)amba_ik_system_memcpy(&p_ctx->filters.input_param.stitching_info, p_stitch_info, sizeof(ik_stitch_info_t));
            ctx_raise_update_flag(&p_ctx->filters.update_flags.iso.stitching_info_updated);
            (void)amba_ik_system_unlock_mutex(&p_ctx->organization.mutex);
        }
    }
    return rval;
}

uint32 img_ctx_ivd_set_burst_tile(const uint32 context_id, const ik_burst_tile_t *p_burst_tile)
{
    amba_ik_system_print_str_5("[IK] set_burst_tile() is invalid!", DC_S, DC_S, DC_S, DC_S, DC_S);
    if (context_id == 0U) {
        //TBD
    }
    if (p_burst_tile == NULL) {
        //TBD
    }
    return IK_ERR_0004;
}

uint32 img_ctx_set_burst_tile(const uint32 context_id, const ik_burst_tile_t *p_burst_tile)
{
    amba_ik_context_entity_t *p_ctx = NULL;
    uint32 rval = IK_OK;
    const ik_debug_check_func_t* p_check_func = ik_ctx_get_debug_check_func();
    if(p_check_func != NULL) {
        rval = p_check_func->burst_tile(p_burst_tile);
    }
    if(rval == IK_OK) {
        rval |= img_ctx_get_context(context_id, &p_ctx);
        if(p_ctx->filters.update_flags.iso.is_1st_frame == 0U) {
            amba_ik_system_print_uint32_5("[IK][ERROR] Invalid img_ctx_set_burst_tile run-time update, context_id = %d\n", context_id, DC_U, DC_U, DC_U, DC_U);
            rval |= IK_ERR_0008;
        } else {
            (void)amba_ik_system_lock_mutex(&p_ctx->organization.mutex, IK_WAIT_FOREVER);
            (void)amba_ik_system_memcpy(&p_ctx->filters.input_param.burst_tile, p_burst_tile, sizeof(ik_burst_tile_t));
            ctx_raise_update_flag(&p_ctx->filters.update_flags.iso.burst_tile_updated);
            (void)amba_ik_system_unlock_mutex(&p_ctx->organization.mutex);
        }
    }
    return rval;
}

uint32 img_ctx_ivd_get_static_bpc(const uint32 context_id, ik_static_bad_pxl_cor_t *p_static_bpc)
{
    amba_ik_system_print_str_5("[IK][ERROR] get static_bpc API is invalid!", DC_S, DC_S, DC_S, DC_S, DC_S);
    if (context_id == 0U) {
        //MISRAC
    }
    if (p_static_bpc != NULL) {
        (void)amba_ik_system_memcpy(p_static_bpc, p_static_bpc, sizeof(uint8));
    }
    return IK_ERR_0004;
}

uint32 img_ctx_ivd_get_static_bpc_internal(const uint32 context_id, ik_static_bad_pixel_correction_internal_t *p_static_bpc_internal)
{
    amba_ik_system_print_str_5("[IK] please set correct get_static_bpc_internal api", DC_S, DC_S, DC_S, DC_S, DC_S);
    if (context_id == 0U) {
        //MISRAC
    }
    if (p_static_bpc_internal != NULL) {
        (void)amba_ik_system_memcpy(p_static_bpc_internal, p_static_bpc_internal, sizeof(uint8));
    }
    return IK_ERR_0004;
}

uint32 img_ctx_get_static_bpc(const uint32 context_id, ik_static_bad_pxl_cor_t *p_static_bpc)
{
    amba_ik_context_entity_t *p_ctx = NULL;
    uint32 rval = img_ctx_get_context(context_id, &p_ctx);
    if ((rval == IK_OK) && (p_static_bpc != NULL)) {
        (void)amba_ik_system_lock_mutex(&p_ctx->organization.mutex, IK_WAIT_FOREVER);
        (void)amba_ik_system_memcpy(p_static_bpc, &p_ctx->filters.input_param.static_bpc, sizeof(ik_static_bad_pxl_cor_t));
        (void)amba_ik_system_unlock_mutex(&p_ctx->organization.mutex);
    }
    return rval;
}

uint32 img_ctx_get_static_bpc_internal(const uint32 context_id, ik_static_bad_pixel_correction_internal_t *p_static_bpc_internal)
{
    amba_ik_context_entity_t *p_ctx = NULL;
    uint32 rval;

    if (p_static_bpc_internal == NULL) {
        rval = IK_ERR_0005;
    } else {
        rval = img_ctx_get_context(context_id, &p_ctx);
        if (rval == IK_OK) {
            (void)amba_ik_system_lock_mutex(&p_ctx->organization.mutex, IK_WAIT_FOREVER);
            (void)amba_ik_system_memcpy(p_static_bpc_internal, &p_ctx->filters.input_param.sbp_internal, sizeof(ik_static_bad_pixel_correction_internal_t));
            (void)amba_ik_system_unlock_mutex(&p_ctx->organization.mutex);
        }
    }
    return rval;
}

uint32 img_ctx_ivd_get_vignette(const uint32 context_id, ik_vignette_t *p_vignette)
{
    amba_ik_system_print_str_5("[IK]  please set correct get_vignette_compensation api", DC_S, DC_S, DC_S, DC_S, DC_S);
    if (context_id == 0U) {
        //TBD
    }
    if (p_vignette != NULL) {
        (void)amba_ik_system_memcpy(p_vignette, p_vignette, sizeof(ik_vignette_t));
    }
    return IK_ERR_0004;
}

uint32 img_ctx_get_vignette(const uint32 context_id, ik_vignette_t *p_vignette)
{
    amba_ik_context_entity_t *p_ctx = NULL;
    uint32 rval;

    if (p_vignette == NULL) {
        rval = IK_ERR_0005;
    } else {
        rval = img_ctx_get_context(context_id, &p_ctx);
        if (rval == IK_OK) {
            (void)amba_ik_system_lock_mutex(&p_ctx->organization.mutex, IK_WAIT_FOREVER);
            (void)amba_ik_system_memcpy(p_vignette, &p_ctx->filters.input_param.vignette_compensation, sizeof(ik_vignette_t));
            (void)amba_ik_system_unlock_mutex(&p_ctx->organization.mutex);
        }
    }
    return rval;
}

uint32 img_ctx_ivd_get_calib_warp_info(const uint32 context_id, ik_warp_info_t *p_calib_warp_info)
{
    amba_ik_system_print_str_5("[IK] get_calib_warp_info() is invalid!", DC_S, DC_S, DC_S, DC_S, DC_S);
    if (context_id == 0U) {
        //TBD
    }
    if (p_calib_warp_info != NULL) {
        //TBD
        p_calib_warp_info->reserved = 0U;
    }
    return IK_ERR_0004;
}

uint32 img_ctx_get_calib_warp_info(const uint32 context_id, ik_warp_info_t *p_calib_warp_info)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;

    if (p_calib_warp_info == NULL) {
        rval = IK_ERR_0005;
    } else {
        rval = img_ctx_get_context(context_id, &p_ctx);
        if (rval == IK_OK) {
            (void)amba_ik_system_lock_mutex(&p_ctx->organization.mutex, IK_WAIT_FOREVER);
            (void)amba_ik_system_memcpy(p_calib_warp_info, &p_ctx->filters.input_param.calib_warp_info, sizeof(ik_warp_info_t));
            (void)amba_ik_system_unlock_mutex(&p_ctx->organization.mutex);
        }
    }
    return rval;
}

uint32 img_ctx_ivd_get_2nd_calib_warp_info(const uint32 context_id, ik_warp_info_t *p_calib_warp_info_2nd)
{
    amba_ik_system_print_str_5("[IK] get_calib_warp_info() is invalid!", DC_S, DC_S, DC_S, DC_S, DC_S);
    if (context_id == 0U) {
        //TBD
    }
    if (p_calib_warp_info_2nd != NULL) {
        //TBD
        p_calib_warp_info_2nd->reserved = 0U;
    }
    return IK_ERR_0004;
}

uint32 img_ctx_get_2nd_calib_warp_info(const uint32 context_id, ik_warp_info_t *p_calib_warp_info_2nd)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;

    if (p_calib_warp_info_2nd == NULL) {
        rval = IK_ERR_0005;
    } else {
        rval = img_ctx_get_context(context_id, &p_ctx);
        if (rval == IK_OK) {
            (void)amba_ik_system_lock_mutex(&p_ctx->organization.mutex, IK_WAIT_FOREVER);
            (void)amba_ik_system_memcpy(p_calib_warp_info_2nd, &p_ctx->filters.input_param.calib_warp_info_2nd, sizeof(ik_warp_info_t));
            (void)amba_ik_system_unlock_mutex(&p_ctx->organization.mutex);
        }
    }
    return rval;
}

uint32 img_ctx_get_warp_bind_info(const uint32 context_id, ik_bind_info_t *p_warp_bind_info)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;

    if (p_warp_bind_info == NULL) {
        rval = IK_ERR_0005;
    } else {
        rval = img_ctx_get_context(context_id, &p_ctx);
        if (rval == IK_OK) {
            (void)amba_ik_system_lock_mutex(&p_ctx->organization.mutex, IK_WAIT_FOREVER);
            (void)amba_ik_system_memcpy(p_warp_bind_info, &p_ctx->filters.input_param.warp_bind_info, sizeof(ik_bind_info_t));
            (void)amba_ik_system_unlock_mutex(&p_ctx->organization.mutex);
        }
    }
    return rval;
}

uint32 img_ctx_ivd_get_calib_ca_warp_info(const uint32 context_id, ik_cawarp_info_t *p_calib_ca_warp_info)
{
    amba_ik_system_print_str_5("[IK] get_calib_ca_warp_info() is invalid!", DC_S, DC_S, DC_S, DC_S, DC_S);
    if (context_id == 0U) {
        //TBD
    }
    if (p_calib_ca_warp_info != NULL) {
        //TBD
        p_calib_ca_warp_info->Reserved = 0U;
    }
    return IK_ERR_0004;
}

uint32 img_ctx_get_calib_ca_warp_info(const uint32 context_id, ik_cawarp_info_t *p_calib_ca_warp_info)
{
    amba_ik_context_entity_t *p_ctx = NULL;
    uint32 rval;

    if (p_calib_ca_warp_info == NULL) {
        rval = IK_ERR_0005;
    } else {
        rval = img_ctx_get_context(context_id, &p_ctx);
        if (rval == IK_OK) {
            (void)amba_ik_system_lock_mutex(&p_ctx->organization.mutex, IK_WAIT_FOREVER);
            (void)amba_ik_system_memcpy(p_calib_ca_warp_info, &p_ctx->filters.input_param.calib_ca_warp_info, sizeof(ik_cawarp_info_t));
            (void)amba_ik_system_unlock_mutex(&p_ctx->organization.mutex);
        }
    }
    return rval;
}

uint32 img_ctx_ivd_get_dzoom_info(const uint32 context_id, ik_dzoom_info_t *p_dzoom_info)
{
    amba_ik_system_print_str_5("[IK] get_dzoom_info() is invalid!", DC_S, DC_S, DC_S, DC_S, DC_S);
    if (context_id == 0U) {
        //TBD
    }
    if (p_dzoom_info != NULL) {
        //TBD
        p_dzoom_info->zoom_x = 0U;
    }
    return IK_ERR_0004;
}

uint32 img_ctx_get_dzoom_info(const uint32 context_id, ik_dzoom_info_t *p_dzoom_info)
{
    amba_ik_context_entity_t *p_ctx = NULL;
    uint32 rval;

    if (p_dzoom_info == NULL) {
        rval = IK_ERR_0005;
    } else {
        rval = img_ctx_get_context(context_id, &p_ctx);
        if (rval == IK_OK) {
            (void)amba_ik_system_lock_mutex(&p_ctx->organization.mutex, IK_WAIT_FOREVER);
            (void)amba_ik_system_memcpy(p_dzoom_info, &p_ctx->filters.input_param.dzoom_info, sizeof(ik_dzoom_info_t));
            (void)amba_ik_system_unlock_mutex(&p_ctx->organization.mutex);
        }
    }
    return rval;
}

uint32 img_ctx_ivd_get_hdr_shift_info_exp1(const uint32 context_id, ik_hdr_eis_info_t *p_hdr_sft_info)
{
    amba_ik_system_print_str_5("[IK] get_hdr_shift_info_exp1() is invalid!", DC_S, DC_S, DC_S, DC_S, DC_S);
    if (context_id == 0U) {
        //TBD
    }
    if (p_hdr_sft_info != NULL) {
        //TBD
        p_hdr_sft_info->shift_x = 0;
        p_hdr_sft_info->shift_y = 0;
    }
    return IK_ERR_0004;
}

uint32 img_ctx_get_hdr_shift_info_exp1(const uint32 context_id, ik_hdr_eis_info_t *p_hdr_sft_info)
{
    amba_ik_context_entity_t *p_ctx = NULL;
    uint32 rval;

    if (p_hdr_sft_info == NULL) {
        rval = IK_ERR_0005;
    } else {
        rval = img_ctx_get_context(context_id, &p_ctx);
        if (rval == IK_OK) {
            (void)amba_ik_system_lock_mutex(&p_ctx->organization.mutex, IK_WAIT_FOREVER);
            (void)amba_ik_system_memcpy(p_hdr_sft_info, &p_ctx->filters.input_param.hdr_eis_sft_exp1, sizeof(ik_hdr_eis_info_t));
            (void)amba_ik_system_unlock_mutex(&p_ctx->organization.mutex);
        }
    }
    return rval;
}

uint32 img_ctx_ivd_get_hdr_shift_info_exp2(const uint32 context_id, ik_hdr_eis_info_t *p_hdr_sft_info)
{
    amba_ik_system_print_str_5("[IK] get_hdr_shift_info_exp2() is invalid!", DC_S, DC_S, DC_S, DC_S, DC_S);
    if (context_id == 0U) {
        //TBD
    }
    if (p_hdr_sft_info != NULL) {
        //TBD
        p_hdr_sft_info->shift_x = 0;
        p_hdr_sft_info->shift_y = 0;
    }
    return IK_ERR_0004;
}

uint32 img_ctx_get_hdr_shift_info_exp2(const uint32 context_id, ik_hdr_eis_info_t *p_hdr_sft_info)
{
    amba_ik_context_entity_t *p_ctx = NULL;
    uint32 rval;

    if (p_hdr_sft_info == NULL) {
        rval = IK_ERR_0005;
    } else {
        rval = img_ctx_get_context(context_id, &p_ctx);
        if (rval == IK_OK) {
            (void)amba_ik_system_lock_mutex(&p_ctx->organization.mutex, IK_WAIT_FOREVER);
            (void)amba_ik_system_memcpy(p_hdr_sft_info, &p_ctx->filters.input_param.hdr_eis_sft_exp2, sizeof(ik_hdr_eis_info_t));
            (void)amba_ik_system_unlock_mutex(&p_ctx->organization.mutex);
        }
    }
    return rval;
}

uint32 img_ctx_ivd_get_warp_enable_info(const uint32 context_id, uint32 *p_enable)
{
    amba_ik_system_print_str_5("[IK] get_warp_enable_info() is invalid!", DC_S, DC_S, DC_S, DC_S, DC_S);
    if (context_id == 0U) {
        //TBD
    }
    if (p_enable != NULL) {
        //TBD
        *p_enable = 0U;
    }
    return IK_ERR_0004;
}

uint32 img_ctx_get_warp_enable_info(const uint32 context_id, uint32 *p_enable)
{
    amba_ik_context_entity_t *p_ctx = NULL;
    uint32 rval;

    if (p_enable == NULL) {
        rval = IK_ERR_0005;
    } else {
        rval = img_ctx_get_context(context_id, &p_ctx);
        if (rval == IK_OK) {
            (void)amba_ik_system_lock_mutex(&p_ctx->organization.mutex, IK_WAIT_FOREVER);
            (void)amba_ik_system_memcpy(p_enable, &p_ctx->filters.input_param.warp_enable, sizeof(int32));
            (void)amba_ik_system_unlock_mutex(&p_ctx->organization.mutex);
        }
    }
    return rval;
}

uint32 img_ctx_ivd_get_warp_2nd_enable_info(const uint32 context_id, uint32 *p_enable)
{
    amba_ik_system_print_str_5("[IK] get_warp_2nd_enable_info() is invalid!", DC_S, DC_S, DC_S, DC_S, DC_S);
    if (context_id == 0U) {
        //TBD
    }
    if (p_enable != NULL) {
        //TBD
        *p_enable = 0U;
    }
    return IK_ERR_0004;
}

uint32 img_ctx_get_warp_2nd_enable_info(const uint32 context_id, uint32 *p_enable)
{
    amba_ik_context_entity_t *p_ctx = NULL;
    uint32 rval;

    if (p_enable == NULL) {
        rval = IK_ERR_0005;
    } else {
        rval = img_ctx_get_context(context_id, &p_ctx);
        if (rval == IK_OK) {
            (void)amba_ik_system_lock_mutex(&p_ctx->organization.mutex, IK_WAIT_FOREVER);
            (void)amba_ik_system_memcpy(p_enable, &p_ctx->filters.input_param.warp_enable_2nd, sizeof(int32));
            (void)amba_ik_system_unlock_mutex(&p_ctx->organization.mutex);
        }
    }
    return rval;
}

uint32 img_ctx_ivd_get_cawarp_enable_info(const uint32 context_id, uint32 *p_enable)
{
    amba_ik_system_print_str_5("[IK] get_cawarp_enable_info() is invalid!", DC_S, DC_S, DC_S, DC_S, DC_S);
    if (context_id == 0U) {
        //TBD
    }
    if (p_enable != NULL) {
        //TBD
        *p_enable = 0U;
    }
    return IK_ERR_0004;
}

uint32 img_ctx_get_cawarp_enable_info(const uint32 context_id, uint32 *p_enable)
{
    amba_ik_context_entity_t *p_ctx = NULL;
    uint32 rval;

    if (p_enable == NULL) {
        rval = IK_ERR_0005;
    } else {
        rval = img_ctx_get_context(context_id, &p_ctx);
        if (rval == IK_OK) {
            (void)amba_ik_system_lock_mutex(&p_ctx->organization.mutex, IK_WAIT_FOREVER);
            (void)amba_ik_system_memcpy(p_enable, &p_ctx->filters.input_param.cawarp_enable, sizeof(int32));
            (void)amba_ik_system_unlock_mutex(&p_ctx->organization.mutex);
        }
    }
    return rval;
}

uint32 img_ctx_ivd_get_dummy_win_margin_range_info(const uint32 context_id, ik_dummy_margin_range_t *p_dmy_range)
{
    amba_ik_system_print_str_5("[IK] get_dummy_win_margin_range_info() is invalid!", DC_S, DC_S, DC_S, DC_S, DC_S);
    if (context_id == 0U) {
        //TBD
    }
    if (p_dmy_range != NULL) {
        //TBD
        p_dmy_range->top = 0U;
    }
    return IK_ERR_0004;
}

uint32 img_ctx_get_dummy_win_margin_range_info(const uint32 context_id, ik_dummy_margin_range_t *p_dmy_range)
{
    amba_ik_context_entity_t *p_ctx = NULL;
    uint32 rval;

    if (p_dmy_range == NULL) {
        rval = IK_ERR_0005;
    } else {
        rval = img_ctx_get_context(context_id, &p_ctx);
        if (rval == IK_OK) {
            (void)amba_ik_system_lock_mutex(&p_ctx->organization.mutex, IK_WAIT_FOREVER);
            (void)amba_ik_system_memcpy(p_dmy_range, &p_ctx->filters.input_param.dmy_range, sizeof(ik_dummy_margin_range_t));
            (void)amba_ik_system_unlock_mutex(&p_ctx->organization.mutex);
        }
    }
    return rval;
}

uint32 img_ctx_ivd_set_vin_active_window(const uint32 context_id, const ik_vin_active_window_t *p_active_win)
{
    amba_ik_system_print_str_5("[IK] vin_active_window() is invalid!", DC_S, DC_S, DC_S, DC_S, DC_S);
    if (context_id == 0U) {
        //TBD
    }
    if (p_active_win != NULL) {
        //TBD
    }
    return IK_ERR_0004;
}

uint32 img_ctx_set_vin_active_window(const uint32 context_id, const ik_vin_active_window_t *p_active_win)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;
    const ik_debug_check_func_t* p_check_func = ik_ctx_get_debug_check_func();

    rval = img_ctx_get_context(context_id, &p_ctx);
    if((p_check_func != NULL)&&(rval == IK_OK)) {
        rval = p_check_func->vin_active_win(p_active_win, p_ctx->filters.input_param.ability);
    }
    if ((rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        (void)amba_ik_system_lock_mutex(&p_ctx->organization.mutex, IK_WAIT_FOREVER);
        (void)amba_ik_system_memcpy(&p_ctx->filters.input_param.active_window, p_active_win, sizeof(ik_vin_active_window_t));
        ctx_raise_update_flag(&p_ctx->filters.update_flags.iso.vin_active_win_updated);
        (void)amba_ik_system_unlock_mutex(&p_ctx->organization.mutex);
    } else {
        if(p_ctx->organization.initial_flag == 0U) {
            amba_ik_system_print_uint32_5("[IK] context_id = %d was not initial", context_id, DC_U, DC_U, DC_U, DC_U);
            rval = IK_ERR_0002;
        } else {
            amba_ik_system_print_str_5("[IK] can't get context", DC_S, DC_S, DC_S, DC_S, DC_S);
        }
    }
    return rval;
}

uint32 img_ctx_ivd_get_vin_active_window(const uint32 context_id, ik_vin_active_window_t *p_active_win)
{
    amba_ik_system_print_str_5("[IK] get_vin_active_window() is invalid!", DC_S, DC_S, DC_S, DC_S, DC_S);
    if (context_id == 0U) {
        //TBD
    }
    if (p_active_win != NULL) {
        //TBD
        p_active_win->active_geo.start_x= 0U;
    }
    return IK_ERR_0004;
}

uint32 img_ctx_get_vin_active_window(const uint32 context_id, ik_vin_active_window_t *p_active_win)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;

    if (p_active_win == NULL) {
        rval = IK_ERR_0005;
    } else {
        rval = img_ctx_get_context(context_id, &p_ctx);
        if (rval == IK_OK) {
            (void)amba_ik_system_lock_mutex(&p_ctx->organization.mutex, IK_WAIT_FOREVER);
            (void)amba_ik_system_memcpy(p_active_win, &p_ctx->filters.input_param.active_window, sizeof(ik_vin_active_window_t));
            (void)amba_ik_system_unlock_mutex(&p_ctx->organization.mutex);
        }
    }
    return rval;
}

uint32 img_ctx_ivd_set_warp_internal(const uint32 context_id, const ik_warp_internal_info_t *p_warp_internal)
{
    amba_ik_system_print_str_5("[IK] set warp_internal() is invalid!", DC_S, DC_S, DC_S, DC_S, DC_S);
    if (context_id == 0U) {
        //TBD
    }
    if (p_warp_internal != NULL) {
        //TBD
    }
    return IK_ERR_0004;
}

uint32 img_ctx_set_warp_internal(const uint32 context_id, const ik_warp_internal_info_t *p_warp_internal)
{
    amba_ik_context_entity_t *p_ctx = NULL;
    uint32 rval = IK_OK;
    const ik_debug_check_func_t* p_check_func = ik_ctx_get_debug_check_func();
    if(p_check_func != NULL) {
        rval = p_check_func->warp_info_internal(p_warp_internal);
    }
    rval |= img_ctx_get_context(context_id, &p_ctx);
    if ((rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        (void)amba_ik_system_lock_mutex(&p_ctx->organization.mutex, IK_WAIT_FOREVER);
        (void)amba_ik_system_memcpy(&p_ctx->filters.input_param.warp_internal, p_warp_internal, sizeof(ik_warp_internal_info_t));
        ctx_raise_update_flag(&p_ctx->filters.update_flags.iso.warp_internal_updated);
        p_ctx->filters.input_param.ctx_buf.warp_internal_mode_flag = 1;
        (void)amba_ik_system_unlock_mutex(&p_ctx->organization.mutex);
    } else {
        if(p_ctx->organization.initial_flag == 0U) {
            amba_ik_system_print_uint32_5("[IK] context_id = %d was not initial", context_id, DC_U, DC_U, DC_U, DC_U);
        } else {
            amba_ik_system_print_str_5("[IK] can't get context", DC_S, DC_S, DC_S, DC_S, DC_S);
        }
        rval = IK_ERR_0000; // TBD
    }

    return rval;
}

uint32 img_ctx_ivd_set_warp_2nd_internal(const uint32 context_id, const ik_warp_2nd_internal_info_t *p_warp_2nd_internal)
{
    amba_ik_system_print_str_5("[IK] set warp_2nd_internal() is invalid!", DC_S, DC_S, DC_S, DC_S, DC_S);
    if (context_id == 0U) {
        //TBD
    }
    if (p_warp_2nd_internal != NULL) {
        //TBD
    }
    return IK_ERR_0004;
}

uint32 img_ctx_set_warp_2nd_internal(const uint32 context_id, const ik_warp_2nd_internal_info_t *p_warp_2nd_internal)
{
    amba_ik_context_entity_t *p_ctx = NULL;
    uint32 rval = IK_OK;
    const ik_debug_check_func_t* p_check_func = ik_ctx_get_debug_check_func();
    if(p_check_func != NULL) {
        rval = p_check_func->warp_info_2nd_internal(p_warp_2nd_internal);
    }
    rval |= img_ctx_get_context(context_id, &p_ctx);
    if ((rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        (void)amba_ik_system_lock_mutex(&p_ctx->organization.mutex, IK_WAIT_FOREVER);
        (void)amba_ik_system_memcpy(&p_ctx->filters.input_param.warp_2nd_internal, p_warp_2nd_internal, sizeof(ik_warp_2nd_internal_info_t));
        ctx_raise_update_flag(&p_ctx->filters.update_flags.iso.warp_internal_2nd_updated);
        p_ctx->filters.input_param.ctx_buf.warp_2nd_internal_mode_flag = 1;
        (void)amba_ik_system_unlock_mutex(&p_ctx->organization.mutex);
    } else {
        if(p_ctx->organization.initial_flag == 0U) {
            amba_ik_system_print_uint32_5("[IK] context_id = %d was not initial", context_id, DC_U, DC_U, DC_U, DC_U);
        } else {
            amba_ik_system_print_str_5("[IK] can't get context", DC_S, DC_S, DC_S, DC_S, DC_S);
        }
        rval = IK_ERR_0000; // TBD
    }

    return rval;
}

uint32 img_ctx_ivd_set_cawarp_internal(const uint32 context_id, const ik_cawarp_internal_info_t *p_cawarp_internal)
{
    amba_ik_system_print_str_5("[IK] set cawarp_internal() is invalid!", DC_S, DC_S, DC_S, DC_S, DC_S);
    if (context_id == 0U) {
        //TBD
    }
    if (p_cawarp_internal != NULL) {
        //TBD
    }
    return IK_ERR_0004;
}
// TBD
uint32 img_ctx_set_cawarp_internal(const uint32 context_id, const ik_cawarp_internal_info_t *p_cawarp_internal)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;
    const ik_debug_check_func_t* p_check_func = ik_ctx_get_debug_check_func();
    if(p_check_func != NULL) {
        rval = p_check_func->cawarp_internal(p_cawarp_internal);
    }
    rval |= img_ctx_get_context(context_id, &p_ctx);
    if ((rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        (void)amba_ik_system_lock_mutex(&p_ctx->organization.mutex, IK_WAIT_FOREVER);
        (void)amba_ik_system_memcpy(&p_ctx->filters.input_param.ca_warp_internal, p_cawarp_internal, sizeof(ik_cawarp_internal_info_t));
        ctx_raise_update_flag(&p_ctx->filters.update_flags.iso.cawarp_internal_updated);
        p_ctx->filters.input_param.ctx_buf.cawarp_internal_mode_flag = 1;
        (void)amba_ik_system_unlock_mutex(&p_ctx->organization.mutex);
    } else {
        if(p_ctx->organization.initial_flag == 0U) {
            amba_ik_system_print_uint32_5("[IK] context_id = %d was not initial", context_id, DC_U, DC_U, DC_U, DC_U);
            rval = IK_ERR_0002;
        } else {
            amba_ik_system_print_str_5("[IK] can't get context", DC_S, DC_S, DC_S, DC_S, DC_S);
        }
    }

    return rval;
}

uint32 img_ctx_ivd_get_warp_internal(const uint32 context_id, ik_warp_internal_info_t *p_warp_internal)
{
    amba_ik_system_print_str_5("[IK] get warp_internal() is invalid!", DC_S, DC_S, DC_S, DC_S, DC_S);
    if (context_id == 0U) {
        //TBD
    }
    if (p_warp_internal != NULL) {
        //TBD
        p_warp_internal->enable = 0U;
    }
    return IK_ERR_0004;
}

uint32 img_ctx_get_warp_internal(const uint32 context_id, ik_warp_internal_info_t *p_warp_internal)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        (void)amba_ik_system_lock_mutex(&p_ctx->organization.mutex, IK_WAIT_FOREVER);
        (void)amba_ik_system_memcpy(p_warp_internal, &p_ctx->filters.input_param.warp_internal, sizeof(ik_warp_internal_info_t));
        (void)amba_ik_system_unlock_mutex(&p_ctx->organization.mutex);
    } else {
        rval = IK_ERR_0000; // TBD
    }
    return rval;
}

uint32 img_ctx_ivd_get_warp_2nd_internal(const uint32 context_id, ik_warp_2nd_internal_info_t *p_warp_2nd_internal)
{
    amba_ik_system_print_str_5("[IK] get warp_2nd_internal() is invalid!", DC_S, DC_S, DC_S, DC_S, DC_S);
    if (context_id == 0U) {
        //TBD
    }
    if (p_warp_2nd_internal != NULL) {
        //TBD
        p_warp_2nd_internal->enable = 0U;
    }
    return IK_ERR_0004;
}

uint32 img_ctx_get_warp_2nd_internal(const uint32 context_id, ik_warp_2nd_internal_info_t *p_warp_2nd_internal)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        (void)amba_ik_system_lock_mutex(&p_ctx->organization.mutex, IK_WAIT_FOREVER);
        (void)amba_ik_system_memcpy(p_warp_2nd_internal, &p_ctx->filters.input_param.warp_2nd_internal, sizeof(ik_warp_2nd_internal_info_t));
        (void)amba_ik_system_unlock_mutex(&p_ctx->organization.mutex);
    } else {
        rval = IK_ERR_0000; // TBD
    }
    return rval;
}

uint32 img_ctx_ivd_get_cawarp_internal(const uint32 context_id, ik_cawarp_internal_info_t *p_cawarp_internal)
{
    amba_ik_system_print_str_5("[IK] get cawarp_internal() is invalid!", DC_S, DC_S, DC_S, DC_S, DC_S);
    if (context_id == 0U) {
        //TBD
    }
    if (p_cawarp_internal != NULL) {
        //TBD
        p_cawarp_internal->vert_warp_enable = 0U;
    }
    return IK_ERR_0004;
}

uint32 img_ctx_get_cawarp_internal(const uint32 context_id, ik_cawarp_internal_info_t *p_cawarp_internal)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        (void)amba_ik_system_lock_mutex(&p_ctx->organization.mutex, IK_WAIT_FOREVER);
        (void)amba_ik_system_memcpy(p_cawarp_internal, &p_ctx->filters.input_param.ca_warp_internal, sizeof(ik_cawarp_internal_info_t));
        (void)amba_ik_system_unlock_mutex(&p_ctx->organization.mutex);
    } else {
        rval = IK_ERR_0000; // TBD
    }
    return rval;
}

uint32 img_ctx_get_win_result(const uint32 context_id, amba_ik_calc_win_result_t *p_win_result)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        (void)amba_ik_system_lock_mutex(&p_ctx->organization.mutex, IK_WAIT_FOREVER);
        (void)amba_ik_system_memcpy(p_win_result, &p_ctx->filters.input_param.ctx_buf.result_win, sizeof(amba_ik_calc_win_result_t));
        (void)amba_ik_system_unlock_mutex(&p_ctx->organization.mutex);
    } else {
        rval = IK_ERR_0000; // TBD
    }
    return rval;
}

uint32 img_ctx_ivd_get_flip_mode(const uint32 context_id, uint32 *mode)
{
    amba_ik_system_print_str_5("[IK] get_flip_mode is invalid!", DC_S, DC_S, DC_S, DC_S, DC_S);
    if (context_id == 0U) {
        //MISRAC
    }
    if (mode != NULL) {
        (void)amba_ik_system_memcpy(mode, mode, sizeof(uint8));
    }
    return IK_ERR_0004;
}

uint32 img_ctx_get_flip_mode(const uint32 context_id, uint32 *mode)
{
    amba_ik_context_entity_t *p_ctx = NULL;
    uint32 rval;

    if (mode == NULL) {
        rval = IK_ERR_0005;
    } else {
        rval = img_ctx_get_context(context_id, &p_ctx);
        if (rval == IK_OK) {
            (void)amba_ik_system_lock_mutex(&p_ctx->organization.mutex, IK_WAIT_FOREVER);
            (void)amba_ik_system_memcpy(mode, &p_ctx->filters.input_param.flip_mode, sizeof(uint32));
            (void)amba_ik_system_unlock_mutex(&p_ctx->organization.mutex);
        }
    }
    return rval;
}

uint32 img_ctx_ivd_get_warp_buffer_info(const uint32 context_id, ik_warp_buffer_info_t *p_warp_buf_info)
{
    amba_ik_system_print_str_5("[IK] get_warp_buffer_info is invalid!", DC_S, DC_S, DC_S, DC_S, DC_S);
    if (context_id == 0U) {
        //MISRAC
    }
    if (p_warp_buf_info != NULL) {
        (void)amba_ik_system_memcpy(p_warp_buf_info, p_warp_buf_info, sizeof(uint8));
    }
    return IK_ERR_0004;
}

uint32 img_ctx_get_warp_buffer_info(const uint32 context_id, ik_warp_buffer_info_t *p_warp_buf_info)
{
    amba_ik_context_entity_t *p_ctx = NULL;
    uint32 rval;

    if (p_warp_buf_info == NULL) {
        rval = IK_ERR_0005;
    } else {
        rval = img_ctx_get_context(context_id, &p_ctx);
        if (rval == IK_OK) {
            (void)amba_ik_system_lock_mutex(&p_ctx->organization.mutex, IK_WAIT_FOREVER);
            (void)amba_ik_system_memcpy(p_warp_buf_info, &p_ctx->filters.input_param.warp_buf_info, sizeof(ik_warp_buffer_info_t));
            (void)amba_ik_system_unlock_mutex(&p_ctx->organization.mutex);
        }
    }
    return rval;
}

uint32 img_ctx_ivd_get_overlap_x(const uint32 context_id, ik_overlap_x_t *p_overlap_x)
{
    amba_ik_system_print_str_5("[IK] get_overlap_x is invalid!", DC_S, DC_S, DC_S, DC_S, DC_S);
    if (context_id == 0U) {
        //MISRAC
    }
    if (p_overlap_x != NULL) {
        (void)amba_ik_system_memcpy(p_overlap_x, p_overlap_x, sizeof(uint8));
    }
    return IK_ERR_0004;
}

uint32 img_ctx_get_overlap_x(const uint32 context_id, ik_overlap_x_t *p_overlap_x)
{
    amba_ik_context_entity_t *p_ctx = NULL;
    uint32 rval;

    if (p_overlap_x == NULL) {
        rval = IK_ERR_0005;
    } else {
        rval = img_ctx_get_context(context_id, &p_ctx);
        if (rval == IK_OK) {
            (void)amba_ik_system_lock_mutex(&p_ctx->organization.mutex, IK_WAIT_FOREVER);
            (void)amba_ik_system_memcpy(p_overlap_x, &p_ctx->filters.input_param.overlap_x, sizeof(ik_overlap_x_t));
            (void)amba_ik_system_unlock_mutex(&p_ctx->organization.mutex);
        }
    }
    return rval;
}

uint32 img_ctx_ivd_get_stitching_info(const uint32 context_id, ik_stitch_info_t *p_stitch_info)
{
    amba_ik_system_print_str_5("[IK] get_stitching_info is invalid!", DC_S, DC_S, DC_S, DC_S, DC_S);
    if (context_id == 0U) {
        //MISRAC
    }
    if (p_stitch_info != NULL) {
        (void)amba_ik_system_memcpy(p_stitch_info, p_stitch_info, sizeof(uint8));
    }
    return IK_ERR_0004;
}

uint32 img_ctx_get_stitching_info(const uint32 context_id, ik_stitch_info_t *p_stitch_info)
{
    amba_ik_context_entity_t *p_ctx = NULL;
    uint32 rval;

    if (p_stitch_info == NULL) {
        rval = IK_ERR_0005;
    } else {
        rval = img_ctx_get_context(context_id, &p_ctx);
        if (rval == IK_OK) {
            (void)amba_ik_system_lock_mutex(&p_ctx->organization.mutex, IK_WAIT_FOREVER);
            (void)amba_ik_system_memcpy(p_stitch_info, &p_ctx->filters.input_param.stitching_info, sizeof(ik_stitch_info_t));
            (void)amba_ik_system_unlock_mutex(&p_ctx->organization.mutex);
        }
    }
    return rval;
}

uint32 img_ctx_ivd_get_burst_tile(const uint32 context_id, ik_burst_tile_t *p_burst_tile)
{
    amba_ik_system_print_str_5("[IK] get_burst_tile is invalid!", DC_S, DC_S, DC_S, DC_S, DC_S);
    if (context_id == 0U) {
        //MISRAC
    }
    if (p_burst_tile != NULL) {
        (void)amba_ik_system_memcpy(p_burst_tile, p_burst_tile, sizeof(uint8));
    }
    return IK_ERR_0004;
}

uint32 img_ctx_get_burst_tile(const uint32 context_id, ik_burst_tile_t *p_burst_tile)
{
    amba_ik_context_entity_t *p_ctx = NULL;
    uint32 rval;

    if (p_burst_tile == NULL) {
        rval = IK_ERR_0005;
    } else {
        rval = img_ctx_get_context(context_id, &p_ctx);
        if (rval == IK_OK) {
            (void)amba_ik_system_lock_mutex(&p_ctx->organization.mutex, IK_WAIT_FOREVER);
            (void)amba_ik_system_memcpy(p_burst_tile, &p_ctx->filters.input_param.burst_tile, sizeof(ik_burst_tile_t));
            (void)amba_ik_system_unlock_mutex(&p_ctx->organization.mutex);
        }
    }
    return rval;
}

