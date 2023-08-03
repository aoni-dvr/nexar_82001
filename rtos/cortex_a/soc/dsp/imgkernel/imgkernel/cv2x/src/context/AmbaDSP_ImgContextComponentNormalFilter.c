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
#include "AmbaDSP_ImgContextUtility.h"
#include "AmbaDSP_ImgContextDebugUtility.h"
#include "AmbaDSP_ImgArchComponentIF.h"
#include "AmbaDSP_ImgArchSystemAPI.h"

uint32 img_ctx_ivd_set_vin_sensor_info(uint32 context_id, const ik_vin_sensor_info_t *p_sensor_info)
{
    amba_ik_system_print_str_5("[IK] set_vin_sensor_info is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //MISRA
    }
    if (p_sensor_info == NULL) {
        //MISRA
    }
    return IK_ERR_0004;
}

uint32 img_ctx_set_vin_sensor_info(uint32 context_id, const ik_vin_sensor_info_t *p_sensor_info)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;
    const ik_debug_check_func_t* p_check_func = ik_ctx_get_debug_check_func();
    if(p_check_func != NULL) {
        rval = p_check_func->vin_sensor_info(p_sensor_info, "vin_sensor_info");
    }
    if(rval == IK_OK) {
        rval |= img_ctx_get_context(context_id, &p_ctx);
        if((p_sensor_info != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
            (void)amba_ik_system_lock_mutex(context_id, IK_WAIT_FOREVER);
            (void)amba_ik_system_memcpy(&p_ctx->filters.input_param.sensor_info, p_sensor_info, sizeof(ik_vin_sensor_info_t));
            ctx_raise_update_flag(&p_ctx->filters.update_flags.liso.sensor_information_updated);
            (void)amba_ik_system_unlock_mutex(context_id);
        } else {
            if(p_ctx->organization.initial_flag == 0U) {
                amba_ik_system_print_uint32_5("[IK][ERROR] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
                rval = IK_ERR_0002;
            } else {
                amba_ik_system_print_str_5("[IK][ERROR] can't get context\n", NULL, NULL, NULL, NULL, NULL);
            }
        }
    }
    return rval;
}

uint32 img_ctx_ivd_set_global_dgain(uint32 context_id, const ik_global_dgain_t *p_global_dgain)
{
    amba_ik_system_print_str_5("[IK] set_global_dgain is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //MISRA
    }
    if (p_global_dgain == NULL) {
        //MISRA
    }
    return IK_ERR_0004;
}

uint32 img_ctx_set_global_dgain(uint32 context_id, const ik_global_dgain_t *p_global_dgain)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;
    const ik_debug_check_func_t* p_check_func = ik_ctx_get_debug_check_func();
    if(p_check_func != NULL) {
        rval = p_check_func->global_dgain(p_global_dgain, "global_dgain");
    }
    if(rval == IK_OK) {
        rval |= img_ctx_get_context(context_id, &p_ctx);
        if((p_global_dgain != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
            (void)amba_ik_system_lock_mutex(context_id, IK_WAIT_FOREVER);
            (void)amba_ik_system_memcpy(&p_ctx->filters.input_param.global_dgain, p_global_dgain, sizeof(ik_global_dgain_t));
            ctx_raise_update_flag(&p_ctx->filters.update_flags.liso.global_dgain_updated);
            (void)amba_ik_system_unlock_mutex(context_id);
        } else {
            if(p_ctx->organization.initial_flag == 0U) {
                amba_ik_system_print_uint32_5("[IK][ERROR] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
                rval = IK_ERR_0002;
            } else {
                amba_ik_system_print_str_5("[IK][ERROR] can't get context\n", NULL, NULL, NULL, NULL, NULL);
            }
        }
    }
    return rval;
}

uint32 img_ctx_ivd_set_ae_gain(uint32 context_id, const ik_ae_gain_t *p_ae_gain)
{
    amba_ik_system_print_str_5("[IK] set_ae_gain is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //MISRA
    }
    if (p_ae_gain == NULL) {
        //MISRA
    }
    return IK_ERR_0004;
}

uint32 img_ctx_set_ae_gain(uint32 context_id, const ik_ae_gain_t *p_ae_gain)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;
    const ik_debug_check_func_t* p_check_func = ik_ctx_get_debug_check_func();
    if(p_check_func != NULL) {
        rval = p_check_func->ae_gain(p_ae_gain, "ae_gain");
    }
    if(rval == IK_OK) {
        rval |= img_ctx_get_context(context_id, &p_ctx);
        if((p_ae_gain != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
            (void)amba_ik_system_lock_mutex(context_id, IK_WAIT_FOREVER);
            (void)amba_ik_system_memcpy(&p_ctx->filters.input_param.ae_gain, p_ae_gain, sizeof(ik_ae_gain_t));
            ctx_raise_update_flag(&p_ctx->filters.update_flags.liso.ae_gain_updated);
            (void)amba_ik_system_unlock_mutex(context_id);
        } else {
            if(p_ctx->organization.initial_flag == 0U) {
                amba_ik_system_print_uint32_5("[IK][ERROR] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
                rval |= IK_ERR_0002;
            } else {
                amba_ik_system_print_str_5("[IK][ERROR] can't get context\n", NULL, NULL, NULL, NULL, NULL);
            }
        }
    }
    return rval;
}

uint32 img_ctx_ivd_set_before_ce_wb_gain(uint32 context_id, const ik_wb_gain_t *p_before_ce_wb_gain)
{
    amba_ik_system_print_str_5("[IK] set_before_ce_wb_gain is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //MISRA
    }
    if (p_before_ce_wb_gain == NULL) {
        //MISRA
    }
    return IK_ERR_0004;
}

uint32 img_ctx_set_before_ce_wb_gain(uint32 context_id, const ik_wb_gain_t *p_before_ce_wb_gain)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;
    const ik_debug_check_func_t* p_check_func = ik_ctx_get_debug_check_func();
    if(p_check_func != NULL) {
        rval = p_check_func->before_ce_wb(p_before_ce_wb_gain, "before_ce_wb_gain");
    }
    if(rval == IK_OK) {
        rval |= img_ctx_get_context(context_id, &p_ctx);
        if((p_before_ce_wb_gain != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
            (void)amba_ik_system_lock_mutex(context_id, IK_WAIT_FOREVER);
            (void)amba_ik_system_memcpy(&p_ctx->filters.input_param.before_ce_wb_gain, p_before_ce_wb_gain, sizeof(ik_wb_gain_t));
            ctx_raise_update_flag(&p_ctx->filters.update_flags.liso.before_ce_wb_gain_updated);
            (void)amba_ik_system_unlock_mutex(context_id);
        } else {
            if(p_ctx->organization.initial_flag == 0U) {
                amba_ik_system_print_uint32_5("[IK][ERROR] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
                rval |= IK_ERR_0002;
            } else {
                amba_ik_system_print_str_5("[IK][ERROR] can't get context\n", NULL, NULL, NULL, NULL, NULL);
            }
        }
    }
    return rval;
}

uint32 img_ctx_ivd_set_after_ce_wb_gain(uint32 context_id, const ik_wb_gain_t *p_after_ce_wb_gain)
{
    amba_ik_system_print_str_5("[IK] set_after_ce_wb_gain is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //MISRA
    }
    if (p_after_ce_wb_gain == NULL) {
        //MISRA
    }
    return IK_ERR_0004;
}

uint32 img_ctx_set_after_ce_wb_gain(uint32 context_id, const ik_wb_gain_t *p_after_ce_wb_gain)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;
    const ik_debug_check_func_t* p_check_func = ik_ctx_get_debug_check_func();
    if(p_check_func != NULL) {
        rval = p_check_func->after_ce_wb(p_after_ce_wb_gain, "after_ce_wb_gain");
    }
    if(rval == IK_OK) {
        rval |= img_ctx_get_context(context_id, &p_ctx);
        if((p_after_ce_wb_gain != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
            (void)amba_ik_system_lock_mutex(context_id, IK_WAIT_FOREVER);
            (void)amba_ik_system_memcpy(&p_ctx->filters.input_param.after_ce_wb_gain, p_after_ce_wb_gain, sizeof(ik_wb_gain_t));
            ctx_raise_update_flag(&p_ctx->filters.update_flags.liso.after_ce_wb_gain_updated);
            (void)amba_ik_system_unlock_mutex(context_id);
        } else {
            if(p_ctx->organization.initial_flag == 0U) {
                amba_ik_system_print_uint32_5("[IK][ERROR] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
                rval |= IK_ERR_0002;
            } else {
                amba_ik_system_print_str_5("[IK][ERROR] can't get context\n", NULL, NULL, NULL, NULL, NULL);
            }
        }
    }
    return rval;
}

uint32 img_ctx_ivd_set_cfa_leakage_filter(uint32 context_id, const ik_cfa_leakage_filter_t *p_cfa_leakage_filter)
{
    amba_ik_system_print_str_5("[IK] set_cfa_leakage_filter is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //MISRA
    }
    if (p_cfa_leakage_filter == NULL) {
        //MISRA
    }
    return IK_ERR_0004;
}

uint32 img_ctx_set_cfa_leakage_filter(uint32 context_id, const ik_cfa_leakage_filter_t *p_cfa_leakage_filter)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;
    const ik_debug_check_func_t* p_check_func = ik_ctx_get_debug_check_func();
    if(p_check_func != NULL) {
        rval = p_check_func->cfa_leakage(p_cfa_leakage_filter, "cfa_leakage_filter");
    }

    if(rval == IK_OK) {
        rval |= img_ctx_get_context(context_id, &p_ctx);
        if((p_cfa_leakage_filter != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
            (void)amba_ik_system_lock_mutex(context_id, IK_WAIT_FOREVER);
            (void)amba_ik_system_memcpy(&p_ctx->filters.input_param.cfa_leakage_filter, p_cfa_leakage_filter, sizeof(ik_cfa_leakage_filter_t));
            ctx_raise_update_flag(&p_ctx->filters.update_flags.liso.cfa_leakage_filter_updated);
            (void)amba_ik_system_unlock_mutex(context_id);
        } else {
            if(p_ctx->organization.initial_flag == 0U) {
                amba_ik_system_print_uint32_5("[IK][ERROR] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
                rval |= IK_ERR_0002;
            } else {
                amba_ik_system_print_str_5("[IK][ERROR] can't get context\n", NULL, NULL, NULL, NULL, NULL);
            }
        }
    }
    return rval;

}

uint32 img_ctx_ivd_set_anti_aliasing(uint32 context_id, const ik_anti_aliasing_t *p_anti_aliasing)
{
    amba_ik_system_print_str_5("[IK] set_anti_aliasing is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //TBD
    }
    if (p_anti_aliasing == NULL) {
        //TBD
    }
    return IK_ERR_0004;
}

uint32 img_ctx_set_anti_aliasing(uint32 context_id, const ik_anti_aliasing_t *p_anti_aliasing)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;
    const ik_debug_check_func_t* p_check_func = ik_ctx_get_debug_check_func();
    if(p_check_func != NULL) {
        rval = p_check_func->anti_aliasing(p_anti_aliasing, "anti_aliasing");
    }
    if(rval == IK_OK) {
        rval |= img_ctx_get_context(context_id, &p_ctx);
        if((p_anti_aliasing != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
            (void)amba_ik_system_lock_mutex(context_id, IK_WAIT_FOREVER);
            (void)amba_ik_system_memcpy(&p_ctx->filters.input_param.anti_aliasing, p_anti_aliasing, sizeof(ik_anti_aliasing_t));
            ctx_raise_update_flag(&p_ctx->filters.update_flags.liso.anti_aliasing_updated);
            (void)amba_ik_system_unlock_mutex(context_id);
        } else {
            if(p_ctx->organization.initial_flag == 0U) {
                amba_ik_system_print_uint32_5("[IK][ERROR] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
                rval |= IK_ERR_0002;
            } else {
                amba_ik_system_print_str_5("[IK][ERROR] can't get context\n", NULL, NULL, NULL, NULL, NULL);
            }
        }
    }
    return rval;

}

uint32 img_ctx_ivd_set_dynamic_bpc(uint32 context_id, const ik_dynamic_bad_pixel_correction_t *p_dynamic_bpc)
{
    amba_ik_system_print_str_5("[IK] set_dynamic_bpc is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //MISRAC
    }
    if (p_dynamic_bpc==NULL) {
        //MISRAC
    }
    return IK_ERR_0004;
}

uint32 img_ctx_set_dynamic_bpc(uint32 context_id, const ik_dynamic_bad_pixel_correction_t *p_dynamic_bpc)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;
    const ik_debug_check_func_t* p_check_func = ik_ctx_get_debug_check_func();
    if(p_check_func != NULL) {
        rval = p_check_func->dynamic_bad_pxl_cor(p_dynamic_bpc, "dynamic_bpc");
    }
    if(rval == IK_OK) {
        rval |= img_ctx_get_context(context_id, &p_ctx);
        if((p_dynamic_bpc != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
            (void)amba_ik_system_lock_mutex(context_id, IK_WAIT_FOREVER);
            (void)amba_ik_system_memcpy(&p_ctx->filters.input_param.dynamic_bpc, p_dynamic_bpc, sizeof(ik_dynamic_bad_pixel_correction_t));
            ctx_raise_update_flag(&p_ctx->filters.update_flags.liso.dynamic_bpc_updated);
            (void)amba_ik_system_unlock_mutex(context_id);
        } else {
            if(p_ctx->organization.initial_flag == 0U) {
                amba_ik_system_print_uint32_5("[IK][ERROR] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
                rval = IK_ERR_0002;
            } else {
                amba_ik_system_print_str_5("[IK][ERROR] can't get context\n", NULL, NULL, NULL, NULL, NULL);
            }
        }
    }
    return rval;
}

uint32 img_ctx_ivd_set_cfa_noise_filter(uint32 context_id, const ik_cfa_noise_filter_t *p_cfa_noise_filter)
{
    amba_ik_system_print_str_5("[IK] set_cfa_noise_filter is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //TBD
    }
    if (p_cfa_noise_filter == NULL) {
        //TBD
    }
    return IK_ERR_0004;
}

uint32 img_ctx_set_cfa_noise_filter(uint32 context_id, const ik_cfa_noise_filter_t *p_cfa_noise_filter)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;
    const ik_debug_check_func_t* p_check_func = ik_ctx_get_debug_check_func();
    if(p_check_func != NULL) {
        rval = p_check_func->cfa_noise_filter(p_cfa_noise_filter, "cfa_noise_filter");
    }
    if(rval == IK_OK) {
        rval |= img_ctx_get_context(context_id, &p_ctx);
        if((p_cfa_noise_filter != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
            (void)amba_ik_system_lock_mutex(context_id, IK_WAIT_FOREVER);
            (void)amba_ik_system_memcpy(&p_ctx->filters.input_param.cfa_noise_filter, p_cfa_noise_filter, sizeof(ik_cfa_noise_filter_t));
            ctx_raise_update_flag(&p_ctx->filters.update_flags.liso.cfa_noise_filter_updated);
            (void)amba_ik_system_unlock_mutex(context_id);
        } else {
            if(p_ctx->organization.initial_flag == 0U) {
                amba_ik_system_print_uint32_5("[IK][ERROR] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
                rval |= IK_ERR_0002;
            } else {
                amba_ik_system_print_str_5("[IK][ERROR] can't get context\n", NULL, NULL, NULL, NULL, NULL);
            }
        }
    }
    return rval;
}

uint32 img_ctx_ivd_set_dgain_sat_lvl(uint32 context_id, const ik_dgain_saturation_level_t *p_dgain_saturation_level)
{
    amba_ik_system_print_str_5("[IK] set_dgain_sat_lvl is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //TBD
    }
    if (p_dgain_saturation_level == NULL) {
        //TBD
    }
    return IK_ERR_0004;
}

uint32 img_ctx_set_dgain_sat_lvl(uint32 context_id, const ik_dgain_saturation_level_t *p_dgain_saturation_level)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;
    const ik_debug_check_func_t* p_check_func = ik_ctx_get_debug_check_func();
    if(p_check_func != NULL) {
        rval = p_check_func->dgain_sat(p_dgain_saturation_level, "dgain_sat_lvl");
    }
    if(rval == IK_OK) {
        rval |= img_ctx_get_context(context_id, &p_ctx);
        if((p_dgain_saturation_level != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
            (void)amba_ik_system_lock_mutex(context_id, IK_WAIT_FOREVER);
            (void)amba_ik_system_memcpy(&p_ctx->filters.input_param.dgain_sauration_level, p_dgain_saturation_level, sizeof(ik_dgain_saturation_level_t));
            ctx_raise_update_flag(&p_ctx->filters.update_flags.liso.dgain_sat_lvl_updated);
            (void)amba_ik_system_unlock_mutex(context_id);
        } else {
            if(p_ctx->organization.initial_flag == 0U) {
                amba_ik_system_print_uint32_5("[IK][ERROR] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
                rval |= IK_ERR_0002;
            } else {
                amba_ik_system_print_str_5("[IK][ERROR] can't get context\n", NULL, NULL, NULL, NULL, NULL);
            }
        }
    }
    return rval;
}

uint32 img_ctx_ivd_set_demosaic(uint32 context_id, const ik_demosaic_t *p_demosaic)
{
    amba_ik_system_print_str_5("[IK] set_demosaic is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //TBD
    }
    if (p_demosaic == NULL) {
        //TBD
    }
    return IK_ERR_0004;
}

uint32 img_ctx_set_demosaic(uint32 context_id, const ik_demosaic_t *p_demosaic)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;
    const ik_debug_check_func_t* p_check_func = ik_ctx_get_debug_check_func();
    if(p_check_func != NULL) {
        rval = p_check_func->demosaic(p_demosaic, "demosaic");
    }
    if(rval == IK_OK) {
        rval |= img_ctx_get_context(context_id, &p_ctx);
        if((p_demosaic != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
            (void)amba_ik_system_lock_mutex(context_id, IK_WAIT_FOREVER);
            (void)amba_ik_system_memcpy(&p_ctx->filters.input_param.demosaic, p_demosaic, sizeof(ik_demosaic_t));
            ctx_raise_update_flag(&p_ctx->filters.update_flags.liso.demosaic_updated);
            (void)amba_ik_system_unlock_mutex(context_id);
        } else {
            if(p_ctx->organization.initial_flag == 0U) {
                amba_ik_system_print_uint32_5("[IK][ERROR] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
                rval |= IK_ERR_0002;
            } else {
                amba_ik_system_print_str_5("[IK][ERROR] can't get context\n", NULL, NULL, NULL, NULL, NULL);
            }
        }
    }
    return rval;
}

uint32 img_ctx_ivd_set_cc_reg(uint32 context_id, const ik_color_correction_reg_t *p_color_correction_reg)
{
    amba_ik_system_print_str_5("[IK] set_color_correction_reg is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //TBD
    }
    if (p_color_correction_reg == NULL) {
        //TBD
    }
    return IK_ERR_0004;
}

uint32 img_ctx_set_color_correction_reg(uint32 context_id, const ik_color_correction_reg_t *p_color_correction_reg)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;
    const ik_debug_check_func_t* p_check_func = ik_ctx_get_debug_check_func();
    if(p_check_func != NULL) {
        rval = p_check_func->color_correction_reg(p_color_correction_reg, "color_correction_reg");
    }
    if(rval == IK_OK) {
        rval |= img_ctx_get_context(context_id, &p_ctx);
        if((p_color_correction_reg != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
            (void)amba_ik_system_lock_mutex(context_id, IK_WAIT_FOREVER);
            (void)amba_ik_system_memcpy(&p_ctx->filters.input_param.color_correction_reg, p_color_correction_reg, sizeof(ik_color_correction_reg_t));
            p_ctx->filters.input_param.use_cc_reg = 1UL;
            ctx_raise_update_flag(&p_ctx->filters.update_flags.liso.color_correction_reg_updated);
            (void)amba_ik_system_unlock_mutex(context_id);
        } else {
            if(p_ctx->organization.initial_flag == 0U) {
                amba_ik_system_print_uint32_5("[IK][ERROR] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
                rval |= IK_ERR_0002;
            } else {
                amba_ik_system_print_str_5("[IK][ERROR] can't get context\n", NULL, NULL, NULL, NULL, NULL);
            }
        }
    }
    return rval;
}

uint32 img_ctx_ivd_set_color_correction(uint32 context_id, const ik_color_correction_t *p_color_correction)
{
    amba_ik_system_print_str_5("[IK] set_color_correction is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //TBD
    }
    if (p_color_correction==NULL) {
        //TBD
    }
    return IK_ERR_0004;
}

uint32 img_ctx_set_color_correction(uint32 context_id, const ik_color_correction_t *p_color_correction)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;
    const ik_debug_check_func_t* p_check_func = ik_ctx_get_debug_check_func();
    if(p_check_func != NULL) {
        rval = p_check_func->color_correction(p_color_correction, "color_correction");
    }
    if(rval == IK_OK) {
        rval |= img_ctx_get_context(context_id, &p_ctx);
        if((p_color_correction != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
            (void)amba_ik_system_lock_mutex(context_id, IK_WAIT_FOREVER);
            (void)amba_ik_system_memcpy(&p_ctx->filters.input_param.color_correction, p_color_correction, sizeof(ik_color_correction_t));
            p_ctx->filters.input_param.use_cc = 1UL;
            ctx_raise_update_flag(&p_ctx->filters.update_flags.liso.color_correction_updated);
            (void)amba_ik_system_unlock_mutex(context_id);
        } else {
            if(p_ctx->organization.initial_flag == 0U) {
                amba_ik_system_print_uint32_5("[IK][ERROR] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
                rval |= IK_ERR_0002;
            } else {
                amba_ik_system_print_str_5("[IK][ERROR] can't get context\n", NULL, NULL, NULL, NULL, NULL);
            }
        }
    }
    return rval;
}

uint32 img_ctx_ivd_set_pre_cc_gain(uint32 context_id, const ik_pre_cc_gain_t *p_pre_cc_gain)
{
    amba_ik_system_print_str_5("[IK] set_pre_cc_gain is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //TBD
    }
    if (p_pre_cc_gain==NULL) {
        //TBD
    }
    return IK_ERR_0004;
}

uint32 img_ctx_set_pre_cc_gain(uint32 context_id, const ik_pre_cc_gain_t *p_pre_cc_gain)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;
    const ik_debug_check_func_t* p_check_func = ik_ctx_get_debug_check_func();
    if(p_check_func != NULL) {
        rval = p_check_func->pre_cc_gain(p_pre_cc_gain, "pre_cc_gain");
    }
    if(rval == IK_OK) {
        rval |= img_ctx_get_context(context_id, &p_ctx);
        if((p_pre_cc_gain != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
            (void)amba_ik_system_lock_mutex(context_id, IK_WAIT_FOREVER);
            (void)amba_ik_system_memcpy(&p_ctx->filters.input_param.pre_cc_gain, p_pre_cc_gain, sizeof(ik_pre_cc_gain_t));
            ctx_raise_update_flag(&p_ctx->filters.update_flags.liso.pre_cc_gain_updated);
            (void)amba_ik_system_unlock_mutex(context_id);
        } else {
            if(p_ctx->organization.initial_flag == 0U) {
                amba_ik_system_print_uint32_5("[IK][ERROR] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
                rval |= IK_ERR_0002;
            } else {
                amba_ik_system_print_str_5("[IK][ERROR] can't get context\n", NULL, NULL, NULL, NULL, NULL);
            }
        }
    }
    return rval;
}

uint32 img_ctx_ivd_set_tone_curve(uint32 context_id, const ik_tone_curve_t *p_tone_curve)
{
    amba_ik_system_print_str_5("[IK] set_tone_curve is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //TBD
    }
    if (p_tone_curve == NULL) {
        //TBD
    }
    return IK_ERR_0004;
}

uint32 img_ctx_set_tone_curve(uint32 context_id, const ik_tone_curve_t *p_tone_curve)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;
    const ik_debug_check_func_t* p_check_func = ik_ctx_get_debug_check_func();
    if(p_check_func != NULL) {
        rval = p_check_func->tone_curve(p_tone_curve, "tone_curve");
    }
    if(rval == IK_OK) {
        rval |= img_ctx_get_context(context_id, &p_ctx);
        if((p_tone_curve != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
            (void)amba_ik_system_lock_mutex(context_id, IK_WAIT_FOREVER);
            (void)amba_ik_system_memcpy(&p_ctx->filters.input_param.tone_curve, p_tone_curve, sizeof(ik_tone_curve_t));
            p_ctx->filters.input_param.use_tone_curve = 1UL;
            ctx_raise_update_flag(&p_ctx->filters.update_flags.liso.tone_curve_updated);
            (void)amba_ik_system_unlock_mutex(context_id);
        } else {
            if(p_ctx->organization.initial_flag == 0U) {
                amba_ik_system_print_uint32_5("[IK][ERROR] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
                rval |= IK_ERR_0002;
            } else {
                amba_ik_system_print_str_5("[IK][ERROR] can't get context\n", NULL, NULL, NULL, NULL, NULL);
            }
        }
    }
    return rval;
}

uint32 img_ctx_ivd_set_rgb_to_yuv_matrix(uint32 context_id, const ik_rgb_to_yuv_matrix_t *p_rgb_to_yuv_matrix)
{
    amba_ik_system_print_str_5("[IK] set_rgb_to_yuv_matrix is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //TBD
    }
    if (p_rgb_to_yuv_matrix == NULL) {
        //TBD
    }
    return IK_ERR_0004;
}

uint32 img_ctx_set_rgb_to_yuv_matrix(uint32 context_id, const ik_rgb_to_yuv_matrix_t *p_rgb_to_yuv_matrix)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;
    const ik_debug_check_func_t* p_check_func = ik_ctx_get_debug_check_func();
    if(p_check_func != NULL) {
        rval = p_check_func->rgb_to_yuv_matrix(p_rgb_to_yuv_matrix, "rgb_to_yuv_matrix");
    }
    if(rval == IK_OK) {
        rval |= img_ctx_get_context(context_id, &p_ctx);
        if((p_rgb_to_yuv_matrix != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
            (void)amba_ik_system_lock_mutex(context_id, IK_WAIT_FOREVER);
            (void)amba_ik_system_memcpy(&p_ctx->filters.input_param.rgb_to_yuv_matrix, p_rgb_to_yuv_matrix, sizeof(ik_rgb_to_yuv_matrix_t));
            ctx_raise_update_flag(&p_ctx->filters.update_flags.liso.rgb_to_yuv_matrix_updated);
            (void)amba_ik_system_unlock_mutex(context_id);
        } else {
            if(p_ctx->organization.initial_flag == 0U) {
                amba_ik_system_print_uint32_5("[IK][ERROR] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
                rval |= IK_ERR_0002;
            } else {
                amba_ik_system_print_str_5("[IK][ERROR] can't get context\n", NULL, NULL, NULL, NULL, NULL);
            }
        }
    }
    return rval;
}

uint32 img_ctx_ivd_set_rgb_ir(uint32 context_id, const ik_rgb_ir_t *p_rgb_ir)
{
    amba_ik_system_print_str_5("[IK] set_rgb_ir is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //TBD
    }
    if (p_rgb_ir==NULL) {
        //TBD
    }
    return IK_ERR_0004;
}

uint32 img_ctx_set_rgb_ir(uint32 context_id, const ik_rgb_ir_t *p_rgb_ir)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;
    const ik_debug_check_func_t* p_check_func = ik_ctx_get_debug_check_func();
    if(p_check_func != NULL) {
        rval = p_check_func->rgb_ir(p_rgb_ir, "rgb_ir");
    }
    if(rval == IK_OK) {
        rval |= img_ctx_get_context(context_id, &p_ctx);
        if((p_rgb_ir != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
            (void)amba_ik_system_lock_mutex(context_id, IK_WAIT_FOREVER);
            (void)amba_ik_system_memcpy(&p_ctx->filters.input_param.rgb_ir, p_rgb_ir, sizeof(ik_rgb_ir_t));
            p_ctx->filters.input_param.rgb_ir.maintain_color_balance = 256UL;
            ctx_raise_update_flag(&p_ctx->filters.update_flags.liso.rgb_ir_updated);
            (void)amba_ik_system_unlock_mutex(context_id);
        } else {
            if(p_ctx->organization.initial_flag == 0U) {
                amba_ik_system_print_uint32_5("[IK][ERROR] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
                rval |= IK_ERR_0002;
            } else {
                amba_ik_system_print_str_5("[IK][ERROR] can't get context\n", NULL, NULL, NULL, NULL, NULL);
            }
        }
    }
    return rval;
}

uint32 img_ctx_ivd_set_chroma_scale(uint32 context_id, const ik_chroma_scale_t *p_chroma_scale)
{
    amba_ik_system_print_str_5("[IK] set_chroma_scale is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //TBD
    }
    if (p_chroma_scale==NULL) {
        //TBD
    }
    return IK_ERR_0004;
}

uint32 img_ctx_set_chroma_scale(uint32 context_id, const ik_chroma_scale_t *p_chroma_scale)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;
    const ik_debug_check_func_t* p_check_func = ik_ctx_get_debug_check_func();
    if(p_check_func != NULL) {
        rval = p_check_func->chroma_sacle(p_chroma_scale, "chroma_scale");
    }
    if(rval == IK_OK) {
        rval |= img_ctx_get_context(context_id, &p_ctx);
        if((p_chroma_scale != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
            (void)amba_ik_system_lock_mutex(context_id, IK_WAIT_FOREVER);
            (void)amba_ik_system_memcpy(&p_ctx->filters.input_param.chroma_scale, p_chroma_scale, sizeof(ik_chroma_scale_t));
            ctx_raise_update_flag(&p_ctx->filters.update_flags.liso.chroma_scale_updated);
            (void)amba_ik_system_unlock_mutex(context_id);
        } else {
            if(p_ctx->organization.initial_flag == 0U) {
                amba_ik_system_print_uint32_5("[IK][ERROR] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
                rval |= IK_ERR_0002;
            } else {
                amba_ik_system_print_str_5("[IK][ERROR] can't get context\n", NULL, NULL, NULL, NULL, NULL);
            }
        }
    }
    return rval;
}

uint32 img_ctx_ivd_set_chroma_median_filter(uint32 context_id, const ik_chroma_median_filter_t *p_chroma_median_filter)
{
    amba_ik_system_print_str_5("[IK] set_chroma_median_filter is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //TBD
    }
    if (p_chroma_median_filter==NULL) {
        //TBD
    }
    return IK_ERR_0004;
}

uint32 img_ctx_set_chroma_median_filter(uint32 context_id, const ik_chroma_median_filter_t *p_chroma_median_filter)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;
    const ik_debug_check_func_t* p_check_func = ik_ctx_get_debug_check_func();
    if(p_check_func != NULL) {
        rval = p_check_func->chroma_median_filter(p_chroma_median_filter, "chroma_median_filter");
    }
    if(rval == IK_OK) {
        rval |= img_ctx_get_context(context_id, &p_ctx);
        if((p_chroma_median_filter != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
            (void)amba_ik_system_lock_mutex(context_id, IK_WAIT_FOREVER);
            (void)amba_ik_system_memcpy(&p_ctx->filters.input_param.chroma_median_filter, p_chroma_median_filter, sizeof(ik_chroma_median_filter_t));
            ctx_raise_update_flag(&p_ctx->filters.update_flags.liso.chroma_median_filter_updated);
            (void)amba_ik_system_unlock_mutex(context_id);
        } else {
            if(p_ctx->organization.initial_flag == 0U) {
                amba_ik_system_print_uint32_5("[IK][ERROR] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
                rval |= IK_ERR_0002;
            } else {
                amba_ik_system_print_str_5("[IK][ERROR] can't get context\n", NULL, NULL, NULL, NULL, NULL);
            }
        }
    }
    return rval;
}

uint32 img_ctx_ivd_set_first_luma_processing_mode(uint32 context_id, const ik_first_luma_process_mode_t *p_first_luma_process_mode)
{
    amba_ik_system_print_str_5("[IK] set_first_luma_processing_mode is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //TBD
    }
    if (p_first_luma_process_mode==NULL) {
        //TBD
    }
    return IK_ERR_0004;
}

uint32 img_ctx_set_first_luma_processing_mode(uint32 context_id, const ik_first_luma_process_mode_t *p_first_luma_process_mode)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;
    const ik_debug_check_func_t* p_check_func = ik_ctx_get_debug_check_func();
    if(p_check_func != NULL) {
        rval = p_check_func->first_luma_proc_mode(p_first_luma_process_mode, "first_luma_processing_mode");
    }
    if(rval == IK_OK) {
        rval |= img_ctx_get_context(context_id, &p_ctx);
        if((p_first_luma_process_mode != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
            (void)amba_ik_system_lock_mutex(context_id, IK_WAIT_FOREVER);
            (void)amba_ik_system_memcpy(&p_ctx->filters.input_param.first_luma_process_mode, p_first_luma_process_mode, sizeof(ik_first_luma_process_mode_t));
            ctx_raise_update_flag(&p_ctx->filters.update_flags.liso.first_luma_processing_mode_updated);
            (void)amba_ik_system_unlock_mutex(context_id);
        } else {
            if(p_ctx->organization.initial_flag == 0U) {
                amba_ik_system_print_uint32_5("[IK][ERROR] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
                rval |= IK_ERR_0002;
            } else {
                amba_ik_system_print_str_5("[IK][ERROR] can't get context\n", NULL, NULL, NULL, NULL, NULL);
            }
        }
    }
    return rval;
}

uint32 img_ctx_ivd_set_advance_spatial_filter(uint32 context_id, const ik_adv_spatial_filter_t *p_advance_spatial_filter)
{
    amba_ik_system_print_str_5("[IK] set_advance_spatial_filter is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //TBD
    }
    if (p_advance_spatial_filter==NULL) {
        //TBD
    }
    return IK_ERR_0004;
}

uint32 img_ctx_set_advance_spatial_filter(uint32 context_id, const ik_adv_spatial_filter_t *p_advance_spatial_filter)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;
    const ik_debug_check_func_t* p_check_func = ik_ctx_get_debug_check_func();
    if(p_check_func != NULL) {
        rval = p_check_func->adv_spat_fltr(p_advance_spatial_filter, "advance_spatial_filter");
    }
    if(rval == IK_OK) {
        rval |= img_ctx_get_context(context_id, &p_ctx);
        if((p_advance_spatial_filter != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
            (void)amba_ik_system_lock_mutex(context_id, IK_WAIT_FOREVER);
            (void)amba_ik_system_memcpy(&p_ctx->filters.input_param.advance_spatial_filter, p_advance_spatial_filter, sizeof(ik_adv_spatial_filter_t));
            ctx_raise_update_flag(&p_ctx->filters.update_flags.liso.advance_spatial_filter_updated);
            (void)amba_ik_system_unlock_mutex(context_id);
        } else {
            if(p_ctx->organization.initial_flag == 0U) {
                amba_ik_system_print_uint32_5("[IK][ERROR] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
                rval |= IK_ERR_0002;
            } else {
                amba_ik_system_print_str_5("[IK][ERROR] can't get context\n", NULL, NULL, NULL, NULL, NULL);
            }
        }
    }
    return rval;
}

uint32 img_ctx_ivd_set_fstshpns_both(uint32 context_id, const ik_first_sharpen_both_t *p_first_sharpen_both)
{
    amba_ik_system_print_str_5("[IK] set_fstshpns_both is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //TBD
    }
    if (p_first_sharpen_both==NULL) {
        //TBD
    }
    return IK_ERR_0004;
}

uint32 img_ctx_set_fstshpns_both(uint32 context_id, const ik_first_sharpen_both_t *p_first_sharpen_both)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;
    const ik_debug_check_func_t* p_check_func = ik_ctx_get_debug_check_func();
    if(p_check_func != NULL) {
        rval = p_check_func->first_sharpen_both(p_first_sharpen_both, "fstshpns_both");
    }
    if(rval == IK_OK) {
        rval |= img_ctx_get_context(context_id, &p_ctx);
        if((p_first_sharpen_both != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
            (void)amba_ik_system_lock_mutex(context_id, IK_WAIT_FOREVER);
            (void)amba_ik_system_memcpy(&p_ctx->filters.input_param.first_sharpen_both, p_first_sharpen_both, sizeof(ik_first_sharpen_both_t));
            ctx_raise_update_flag(&p_ctx->filters.update_flags.liso.fstshpns_both_updated);
            (void)amba_ik_system_unlock_mutex(context_id);
        } else {
            if(p_ctx->organization.initial_flag == 0U) {
                amba_ik_system_print_uint32_5("[IK][ERROR] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
                rval |= IK_ERR_0002;
            } else {
                amba_ik_system_print_str_5("[IK][ERROR] can't get context\n", NULL, NULL, NULL, NULL, NULL);
            }
        }
    }
    return rval;
}

uint32 img_ctx_ivd_set_fstshpns_noise(uint32 context_id, const ik_first_sharpen_noise_t *p_first_sharpen_noise)
{
    amba_ik_system_print_str_5("[IK] set_fstshpns_noise is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //TBD
    }
    if (p_first_sharpen_noise==NULL) {
        //TBD
    }
    return IK_ERR_0004;
}

uint32 img_ctx_set_fstshpns_noise(uint32 context_id, const ik_first_sharpen_noise_t *p_first_sharpen_noise)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;
    const ik_debug_check_func_t* p_check_func = ik_ctx_get_debug_check_func();
    if(p_check_func != NULL) {
        rval = p_check_func->first_sharpen_noise(p_first_sharpen_noise, "fstshpns_noise");
    }
    if(rval == IK_OK) {
        rval |= img_ctx_get_context(context_id, &p_ctx);
        if((p_first_sharpen_noise != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
            (void)amba_ik_system_lock_mutex(context_id, IK_WAIT_FOREVER);
            (void)amba_ik_system_memcpy(&p_ctx->filters.input_param.first_sharpen_noise, p_first_sharpen_noise, sizeof(ik_first_sharpen_noise_t));
            ctx_raise_update_flag(&p_ctx->filters.update_flags.liso.fstshpns_noise_updated);
            (void)amba_ik_system_unlock_mutex(context_id);
        } else {
            if(p_ctx->organization.initial_flag == 0U) {
                amba_ik_system_print_uint32_5("[IK][ERROR] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
                rval |= IK_ERR_0002;
            } else {
                amba_ik_system_print_str_5("[IK][ERROR] can't get context\n", NULL, NULL, NULL, NULL, NULL);
            }
        }
    }
    return rval;
}

uint32 img_ctx_ivd_set_fstshpns_fir(uint32 context_id, const ik_first_sharpen_fir_t *p_first_sharpen_fir)
{
    amba_ik_system_print_str_5("[IK] first_sharpen_fir is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //TBD
    }
    if (p_first_sharpen_fir==NULL) {
        //TBD
    }
    return IK_ERR_0004;
}

uint32 img_ctx_set_fstshpns_fir(uint32 context_id, const ik_first_sharpen_fir_t *p_first_sharpen_fir)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;
    const ik_debug_check_func_t* p_check_func = ik_ctx_get_debug_check_func();
    if(p_check_func != NULL) {
        rval = p_check_func->first_sharpen_fir(p_first_sharpen_fir, "fstshpns_fir");
    }
    if(rval == IK_OK) {
        rval |= img_ctx_get_context(context_id, &p_ctx);
        if((p_first_sharpen_fir != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
            (void)amba_ik_system_lock_mutex(context_id, IK_WAIT_FOREVER);
            (void)amba_ik_system_memcpy(&p_ctx->filters.input_param.first_sharpen_fir, p_first_sharpen_fir, sizeof(ik_first_sharpen_fir_t));
            ctx_raise_update_flag(&p_ctx->filters.update_flags.liso.fstshpns_fir_updated);
            (void)amba_ik_system_unlock_mutex(context_id);
        } else {
            if(p_ctx->organization.initial_flag == 0U) {
                amba_ik_system_print_uint32_5("[IK][ERROR] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
                rval |= IK_ERR_0002;
            } else {
                amba_ik_system_print_str_5("[IK][ERROR] can't get context\n", NULL, NULL, NULL, NULL, NULL);
            }
        }
    }
    return rval;
}

uint32 img_ctx_ivd_set_fstshpns_coring(uint32 context_id, const ik_first_sharpen_coring_t *p_first_sharpen_coring)
{
    amba_ik_system_print_str_5("[IK] set_fstshpns_coring is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //TBD
    }
    if (p_first_sharpen_coring==NULL) {
        //TBD
    }
    return IK_ERR_0004;
}

uint32 img_ctx_set_fstshpns_coring(uint32 context_id, const ik_first_sharpen_coring_t *p_first_sharpen_coring)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;
    const ik_debug_check_func_t* p_check_func = ik_ctx_get_debug_check_func();
    if(p_check_func != NULL) {
        rval = p_check_func->first_sharpen_coring(p_first_sharpen_coring, "fstshpns_coring");
    }
    if(rval == IK_OK) {
        rval |= img_ctx_get_context(context_id, &p_ctx);
        if((p_first_sharpen_coring != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
            (void)amba_ik_system_lock_mutex(context_id, IK_WAIT_FOREVER);
            (void)amba_ik_system_memcpy(&p_ctx->filters.input_param.first_sharpen_coring, p_first_sharpen_coring, sizeof(ik_first_sharpen_coring_t));
            ctx_raise_update_flag(&p_ctx->filters.update_flags.liso.fstshpns_coring_updated);
            (void)amba_ik_system_unlock_mutex(context_id);
        } else {
            if(p_ctx->organization.initial_flag == 0U) {
                amba_ik_system_print_uint32_5("[IK][ERROR] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
                rval |= IK_ERR_0002;
            } else {
                amba_ik_system_print_str_5("[IK][ERROR] can't get context\n", NULL, NULL, NULL, NULL, NULL);
            }
        }
    }
    return rval;
}

uint32 img_ctx_ivd_set_fstshpns_cor_idx_scl(uint32 context_id, const ik_first_sharpen_coring_idx_scale_t *p_first_sharpen_coring_idx_scale)
{
    amba_ik_system_print_str_5("[IK] set_fstshpns_coring_index_scale is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //TBD
    }
    if (p_first_sharpen_coring_idx_scale==NULL) {
        //TBD
    }
    return IK_ERR_0004;
}

uint32 img_ctx_set_fstshpns_cor_idx_scl(uint32 context_id, const ik_first_sharpen_coring_idx_scale_t *p_first_sharpen_coring_idx_scale)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;
    const ik_debug_check_func_t* p_check_func = ik_ctx_get_debug_check_func();
    if(p_check_func != NULL) {
        rval = p_check_func->fstshpns_cor_idx_scl(p_first_sharpen_coring_idx_scale, "fstshpns_cor_idx_scl");
    }
    if(rval == IK_OK) {
        rval |= img_ctx_get_context(context_id, &p_ctx);
        if((p_first_sharpen_coring_idx_scale != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
            (void)amba_ik_system_lock_mutex(context_id, IK_WAIT_FOREVER);
            (void)amba_ik_system_memcpy(&p_ctx->filters.input_param.first_sharpen_coring_idx_scale, p_first_sharpen_coring_idx_scale, sizeof(ik_first_sharpen_coring_idx_scale_t));
            ctx_raise_update_flag(&p_ctx->filters.update_flags.liso.fstshpns_coring_index_scale_updated);
            (void)amba_ik_system_unlock_mutex(context_id);
        } else {
            if(p_ctx->organization.initial_flag == 0U) {
                amba_ik_system_print_uint32_5("[IK][ERROR] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
                rval |= IK_ERR_0002;
            } else {
                amba_ik_system_print_str_5("[IK][ERROR] can't get context\n", NULL, NULL, NULL, NULL, NULL);
            }
        }
    }
    return rval;
}

uint32 img_ctx_ivd_set_fstshpns_min_coring_result(uint32 context_id, const ik_first_sharpen_min_coring_result_t *p_fstshpns_min_coring_result)
{
    amba_ik_system_print_str_5("[IK] set_fstshpns_min_coring_result is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //TBD
    }
    if (p_fstshpns_min_coring_result==NULL) {
        //TBD
    }
    return IK_ERR_0004;
}

uint32 img_ctx_set_fstshpns_min_coring_result(uint32 context_id, const ik_first_sharpen_min_coring_result_t *p_fstshpns_min_coring_result)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;
    const ik_debug_check_func_t* p_check_func = ik_ctx_get_debug_check_func();
    if(p_check_func != NULL) {
        rval = p_check_func->fstshpns_min_cor_rst(p_fstshpns_min_coring_result, "fstshpns_min_coring_result");
    }
    if(rval == IK_OK) {
        rval |= img_ctx_get_context(context_id, &p_ctx);
        if((p_fstshpns_min_coring_result != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
            (void)amba_ik_system_lock_mutex(context_id, IK_WAIT_FOREVER);
            (void)amba_ik_system_memcpy(&p_ctx->filters.input_param.first_sharpen_min_coring_result, p_fstshpns_min_coring_result, sizeof(ik_first_sharpen_min_coring_result_t));
            ctx_raise_update_flag(&p_ctx->filters.update_flags.liso.fstshpns_min_coring_result_updated);
            (void)amba_ik_system_unlock_mutex(context_id);
        } else {
            if(p_ctx->organization.initial_flag == 0U) {
                amba_ik_system_print_uint32_5("[IK][ERROR] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
                rval |= IK_ERR_0002;
            } else {
                amba_ik_system_print_str_5("[IK][ERROR] can't get context\n", NULL, NULL, NULL, NULL, NULL);
            }
        }
    }
    return rval;
}

uint32 img_ctx_ivd_set_fstshpns_max_coring_result(uint32 context_id, const ik_first_sharpen_max_coring_result_t *p_fstshpns_max_coring_result)
{
    amba_ik_system_print_str_5("[IK] set_fstshpns_max_coring_result is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //TBD
    }
    if (p_fstshpns_max_coring_result == NULL) {
        //TBD
    }
    return IK_ERR_0004;
}

uint32 img_ctx_set_fstshpns_max_coring_result(uint32 context_id, const ik_first_sharpen_max_coring_result_t *p_fstshpns_max_coring_result)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;
    const ik_debug_check_func_t* p_check_func = ik_ctx_get_debug_check_func();
    if(p_check_func != NULL) {
        rval = p_check_func->fstshpns_max_cor_rst(p_fstshpns_max_coring_result, "fstshpns_max_coring_result");
    }
    if(rval == IK_OK) {
        rval |= img_ctx_get_context(context_id, &p_ctx);
        if((p_fstshpns_max_coring_result != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
            (void)amba_ik_system_lock_mutex(context_id, IK_WAIT_FOREVER);
            (void)amba_ik_system_memcpy(&p_ctx->filters.input_param.first_sharpen_max_coring_result, p_fstshpns_max_coring_result, sizeof(ik_first_sharpen_max_coring_result_t));
            ctx_raise_update_flag(&p_ctx->filters.update_flags.liso.fstshpns_max_coring_result_updated);
            (void)amba_ik_system_unlock_mutex(context_id);
        } else {
            if(p_ctx->organization.initial_flag == 0U) {
                amba_ik_system_print_uint32_5("[IK][ERROR] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
                rval |= IK_ERR_0002;
            } else {
                amba_ik_system_print_str_5("[IK][ERROR] can't get context\n", NULL, NULL, NULL, NULL, NULL);
            }
        }
    }
    return rval;
}

uint32 img_ctx_ivd_set_fstshpns_scale_coring(uint32 context_id, const ik_first_sharpen_scale_coring_t *p_fstshpns_scale_coring)
{
    amba_ik_system_print_str_5("[IK] set_fstshpns_scale_coring is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //TBD
    }
    if (p_fstshpns_scale_coring == NULL) {
        //TBD
    }
    return IK_ERR_0004;
}

uint32 img_ctx_set_fstshpns_scale_coring(uint32 context_id, const ik_first_sharpen_scale_coring_t *p_fstshpns_scale_coring)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;
    const ik_debug_check_func_t* p_check_func = ik_ctx_get_debug_check_func();
    if(p_check_func != NULL) {
        rval = p_check_func->fstshpns_scl_cor(p_fstshpns_scale_coring, "fstshpns_scale_coring");
    }
    if(rval == IK_OK) {
        rval |= img_ctx_get_context(context_id, &p_ctx);
        if((p_fstshpns_scale_coring != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
            (void)amba_ik_system_lock_mutex(context_id, IK_WAIT_FOREVER);
            (void)amba_ik_system_memcpy(&p_ctx->filters.input_param.first_sharpen_scale_coring, p_fstshpns_scale_coring, sizeof(ik_first_sharpen_scale_coring_t));
            ctx_raise_update_flag(&p_ctx->filters.update_flags.liso.fstshpns_scale_coring_updated);
            (void)amba_ik_system_unlock_mutex(context_id);
        } else {
            if(p_ctx->organization.initial_flag == 0U) {
                amba_ik_system_print_uint32_5("[IK][ERROR] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
                rval |= IK_ERR_0002;
            } else {
                amba_ik_system_print_str_5("[IK][ERROR] can't get context\n", NULL, NULL, NULL, NULL, NULL);
            }
        }
    }
    return rval;
}

uint32 img_ctx_ivd_set_fnlshpns_both(uint32 context_id, const ik_final_sharpen_both_t *p_final_sharpen_both)
{
    amba_ik_system_print_str_5("[IK] set_fnlshpns_both is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //TBD
    }
    if (p_final_sharpen_both == NULL) {
        //TBD
    }
    return IK_ERR_0004;
}

uint32 img_ctx_set_fnlshpns_both(uint32 context_id, const ik_final_sharpen_both_t *p_final_sharpen_both)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;
    const ik_debug_check_func_t* p_check_func = ik_ctx_get_debug_check_func();
    rval |= img_ctx_get_context(context_id, &p_ctx);
    if((p_final_sharpen_both != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        if(p_check_func != NULL) {
            rval = p_check_func->final_sharpen_both(p_final_sharpen_both, p_ctx->filters.input_param.func_mode, "fnlshpns_both");
        }
        if(rval == IK_OK) {
            (void)amba_ik_system_lock_mutex(context_id, IK_WAIT_FOREVER);
            (void)amba_ik_system_memcpy(&p_ctx->filters.input_param.final_sharpen_both, p_final_sharpen_both, sizeof(ik_final_sharpen_both_t));
            ctx_raise_update_flag(&p_ctx->filters.update_flags.liso.fnlshpns_both_updated);
            (void)amba_ik_system_unlock_mutex(context_id);
        }
    } else {
        if(p_ctx->organization.initial_flag == 0U) {
            amba_ik_system_print_uint32_5("[IK][ERROR] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
            rval |= IK_ERR_0002;
        } else {
            amba_ik_system_print_str_5("[IK][ERROR] can't get context\n", NULL, NULL, NULL, NULL, NULL);
        }
    }
    return rval;
}

uint32 img_ctx_ivd_set_fnlshpns_noise(uint32 context_id, const ik_final_sharpen_noise_t *p_final_sharpen_noise)
{
    amba_ik_system_print_str_5("[IK] set_fnlshpns_noise is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //TBD
    }
    if (p_final_sharpen_noise == NULL) {
        //TBD
    }
    return IK_ERR_0004;
}

uint32 img_ctx_set_fnlshpns_noise(uint32 context_id, const ik_final_sharpen_noise_t *p_final_sharpen_noise)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;
    const ik_debug_check_func_t* p_check_func = ik_ctx_get_debug_check_func();
    if(p_check_func != NULL) {
        rval = p_check_func->final_sharpen_noise(p_final_sharpen_noise, "fnlshpns_noise");
    }
    if(rval == IK_OK) {
        rval |= img_ctx_get_context(context_id, &p_ctx);
        if((p_final_sharpen_noise != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
            (void)amba_ik_system_lock_mutex(context_id, IK_WAIT_FOREVER);
            (void)amba_ik_system_memcpy(&p_ctx->filters.input_param.final_sharpen_noise, p_final_sharpen_noise, sizeof(ik_final_sharpen_noise_t));
            ctx_raise_update_flag(&p_ctx->filters.update_flags.liso.fnlshpns_noise_updated);
            (void)amba_ik_system_unlock_mutex(context_id);
        } else {
            if(p_ctx->organization.initial_flag == 0U) {
                amba_ik_system_print_uint32_5("[IK][ERROR] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
                rval |= IK_ERR_0002;
            } else {
                amba_ik_system_print_str_5("[IK][ERROR] can't get context\n", NULL, NULL, NULL, NULL, NULL);
            }
        }
    }
    return rval;
}

uint32 img_ctx_ivd_set_fnlshpns_fir(uint32 context_id, const ik_final_sharpen_fir_t *p_final_sharpen_fir)
{
    amba_ik_system_print_str_5("[IK] set_fnlshpns_fir is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //TBD
    }
    if (p_final_sharpen_fir == NULL) {
        //TBD
    }
    return IK_ERR_0004;
}

uint32 img_ctx_set_fnlshpns_fir(uint32 context_id, const ik_final_sharpen_fir_t *p_final_sharpen_fir)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;
    const ik_debug_check_func_t* p_check_func = ik_ctx_get_debug_check_func();
    if(p_check_func != NULL) {
        rval = p_check_func->final_sharpen_fir(p_final_sharpen_fir, "fnlshpns_fir");
    }
    if(rval == IK_OK) {
        rval |= img_ctx_get_context(context_id, &p_ctx);
        if((p_final_sharpen_fir != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
            (void)amba_ik_system_lock_mutex(context_id, IK_WAIT_FOREVER);
            (void)amba_ik_system_memcpy(&p_ctx->filters.input_param.final_sharpen_fir, p_final_sharpen_fir, sizeof(ik_final_sharpen_fir_t));
            ctx_raise_update_flag(&p_ctx->filters.update_flags.liso.fnlshpns_fir_updated);
            (void)amba_ik_system_unlock_mutex(context_id);
        } else {
            if(p_ctx->organization.initial_flag == 0U) {
                amba_ik_system_print_uint32_5("[IK][ERROR] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
                rval |= IK_ERR_0002;
            } else {
                amba_ik_system_print_str_5("[IK][ERROR] can't get context\n", NULL, NULL, NULL, NULL, NULL);
            }
        }
    }
    return rval;
}

uint32 img_ctx_ivd_set_fnlshpns_coring(uint32 context_id, const ik_final_sharpen_coring_t *p_final_sharpen_coring)
{
    amba_ik_system_print_str_5("[IK] set_fnlshpns_coring is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //TBD
    }
    if (p_final_sharpen_coring == NULL) {
        //TBD
    }
    return IK_ERR_0004;
}

uint32 img_ctx_set_fnlshpns_coring(uint32 context_id, const ik_final_sharpen_coring_t *p_final_sharpen_coring)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;
    const ik_debug_check_func_t* p_check_func = ik_ctx_get_debug_check_func();
    if(p_check_func != NULL) {
        rval = p_check_func->final_sharpen_coring(p_final_sharpen_coring, "fnlshpns_coring");
    }
    if(rval == IK_OK) {
        rval |= img_ctx_get_context(context_id, &p_ctx);
        if((p_final_sharpen_coring != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
            (void)amba_ik_system_lock_mutex(context_id, IK_WAIT_FOREVER);
            (void)amba_ik_system_memcpy(&p_ctx->filters.input_param.final_sharpen_coring, p_final_sharpen_coring, sizeof(ik_final_sharpen_coring_t));
            ctx_raise_update_flag(&p_ctx->filters.update_flags.liso.fnlshpns_coring_updated);
            (void)amba_ik_system_unlock_mutex(context_id);
        } else {
            if(p_ctx->organization.initial_flag == 0U) {
                amba_ik_system_print_uint32_5("[IK][ERROR] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
                rval |= IK_ERR_0002;
            } else {
                amba_ik_system_print_str_5("[IK][ERROR] can't get context\n", NULL, NULL, NULL, NULL, NULL);
            }
        }
    }
    return rval;

}

uint32 img_ctx_ivd_set_fnlshpns_cor_idx_scl(uint32 context_id, const ik_final_sharpen_coring_idx_scale_t *p_final_sharpen_coring_idx_scale)
{
    amba_ik_system_print_str_5("[IK] set_fnlshpns_coring_index_scale is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //TBD
    }
    if (p_final_sharpen_coring_idx_scale == NULL) {
        //TBD
    }
    return IK_ERR_0004;
}

uint32 img_ctx_set_fnlshpns_cor_idx_scl(uint32 context_id, const ik_final_sharpen_coring_idx_scale_t *p_final_sharpen_coring_idx_scale)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;
    const ik_debug_check_func_t* p_check_func = ik_ctx_get_debug_check_func();
    if(p_check_func != NULL) {
        rval = p_check_func->fnlshpns_cor_idx_scl(p_final_sharpen_coring_idx_scale, "fnlshpns_cor_idx_scl");
    }
    if(rval == IK_OK) {
        rval |= img_ctx_get_context(context_id, &p_ctx);
        if((p_final_sharpen_coring_idx_scale != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
            (void)amba_ik_system_lock_mutex(context_id, IK_WAIT_FOREVER);
            (void)amba_ik_system_memcpy(&p_ctx->filters.input_param.final_sharpen_coring_idx_scale, p_final_sharpen_coring_idx_scale, sizeof(ik_final_sharpen_coring_idx_scale_t));
            ctx_raise_update_flag(&p_ctx->filters.update_flags.liso.fnlshpns_coring_index_scale_updated);
            (void)amba_ik_system_unlock_mutex(context_id);
        } else {
            if(p_ctx->organization.initial_flag == 0U) {
                amba_ik_system_print_uint32_5("[IK][ERROR] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
                rval |= IK_ERR_0002;
            } else {
                amba_ik_system_print_str_5("[IK][ERROR] can't get context\n", NULL, NULL, NULL, NULL, NULL);
            }
        }
    }
    return rval;
}

uint32 img_ctx_ivd_set_fnlshpns_min_coring_result(uint32 context_id, const ik_final_sharpen_min_coring_result_t *p_fnlshpns_min_coring_result)
{
    amba_ik_system_print_str_5("[IK] set_fnlshpns_min_coring_result is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //TBD
    }
    if (p_fnlshpns_min_coring_result == NULL) {
        //TBD
    }
    return IK_ERR_0004;
}

uint32 img_ctx_set_fnlshpns_min_coring_result(uint32 context_id, const ik_final_sharpen_min_coring_result_t *p_fnlshpns_min_coring_result)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;
    const ik_debug_check_func_t* p_check_func = ik_ctx_get_debug_check_func();
    if(p_check_func != NULL) {
        rval = p_check_func->fnlshpns_min_cor_rst(p_fnlshpns_min_coring_result, "fnlshpns_min_coring_result");
    }
    if(rval == IK_OK) {
        rval |= img_ctx_get_context(context_id, &p_ctx);
        if((p_fnlshpns_min_coring_result != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
            (void)amba_ik_system_lock_mutex(context_id, IK_WAIT_FOREVER);
            (void)amba_ik_system_memcpy(&p_ctx->filters.input_param.final_sharpen_min_coring_result, p_fnlshpns_min_coring_result, sizeof(ik_final_sharpen_min_coring_result_t));
            ctx_raise_update_flag(&p_ctx->filters.update_flags.liso.fnlshpns_min_coring_result_updated);
            (void)amba_ik_system_unlock_mutex(context_id);
        } else {
            if(p_ctx->organization.initial_flag == 0U) {
                amba_ik_system_print_uint32_5("[IK][ERROR] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
                rval |= IK_ERR_0002;
            } else {
                amba_ik_system_print_str_5("[IK][ERROR] can't get context\n", NULL, NULL, NULL, NULL, NULL);
            }
        }
    }
    return rval;
}

uint32 img_ctx_ivd_set_fnlshpns_max_coring_result(uint32 context_id, const ik_final_sharpen_max_coring_result_t *p_fnlshpns_max_coring_result)
{
    amba_ik_system_print_str_5("[IK] set_fnlshpns_max_coring_result is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //TBD
    }
    if (p_fnlshpns_max_coring_result == NULL) {
        //TBD
    }
    return IK_ERR_0004;
}

uint32 img_ctx_set_fnlshpns_max_coring_result(uint32 context_id, const ik_final_sharpen_max_coring_result_t *p_fnlshpns_max_coring_result)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;
    const ik_debug_check_func_t* p_check_func = ik_ctx_get_debug_check_func();
    if(p_check_func != NULL) {
        rval = p_check_func->fnlshpns_max_cor_rst(p_fnlshpns_max_coring_result, "fnlshpns_max_coring_result");
    }
    if(rval == IK_OK) {
        rval |= img_ctx_get_context(context_id, &p_ctx);
        if((p_fnlshpns_max_coring_result != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
            (void)amba_ik_system_lock_mutex(context_id, IK_WAIT_FOREVER);
            (void)amba_ik_system_memcpy(&p_ctx->filters.input_param.final_sharpen_max_coring_result, p_fnlshpns_max_coring_result, sizeof(ik_final_sharpen_max_coring_result_t));
            ctx_raise_update_flag(&p_ctx->filters.update_flags.liso.fnlshpns_max_coring_result_updated);
            (void)amba_ik_system_unlock_mutex(context_id);
        } else {
            if(p_ctx->organization.initial_flag == 0U) {
                amba_ik_system_print_uint32_5("[IK][ERROR] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
                rval |= IK_ERR_0002;
            } else {
                amba_ik_system_print_str_5("[IK][ERROR] can't get context\n", NULL, NULL, NULL, NULL, NULL);
            }
        }
    }
    return rval;
}

uint32 img_ctx_ivd_set_fnlshpns_scale_coring(uint32 context_id, const ik_final_sharpen_scale_coring_t *p_fnlshpns_scale_coring)
{
    amba_ik_system_print_str_5("[IK] set_fnlshpns_scale_coring is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //TBD
    }
    if (p_fnlshpns_scale_coring == NULL) {
        //TBD
    }
    return IK_ERR_0004;
}

uint32 img_ctx_set_fnlshpns_scale_coring(uint32 context_id, const ik_final_sharpen_scale_coring_t *p_fnlshpns_scale_coring)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;
    const ik_debug_check_func_t* p_check_func = ik_ctx_get_debug_check_func();
    if(p_check_func != NULL) {
        rval = p_check_func->fnlshpns_scl_cor(p_fnlshpns_scale_coring, "fnlshpns_scale_coring");
    }
    if(rval == IK_OK) {
        if (p_fnlshpns_scale_coring == NULL) {
            rval = IK_ERR_0005;
        } else {
            rval |= img_ctx_get_context(context_id, &p_ctx);
            if (p_ctx->organization.initial_flag == 0U) {
                amba_ik_system_print_uint32_5("[IK][ERROR] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
                rval |= IK_ERR_0002;
            } else if (rval == IK_OK) {
                (void)amba_ik_system_lock_mutex(context_id, IK_WAIT_FOREVER);
                (void)amba_ik_system_memcpy(&p_ctx->filters.input_param.final_sharpen_scale_coring, p_fnlshpns_scale_coring, sizeof(ik_final_sharpen_scale_coring_t));
                ctx_raise_update_flag(&p_ctx->filters.update_flags.liso.fnlshpns_scale_coring_updated);
                (void)amba_ik_system_unlock_mutex(context_id);
            } else {
                // misraC
            }
        }
    }
    return rval;
}

uint32 img_ctx_ivd_set_fnlshpns_both_tdt(uint32 context_id, const ik_final_sharpen_both_three_d_table_t *p_final_sharpen_both_three_d_table)
{
    amba_ik_system_print_str_5("[IK] final_sharpen_both_three_d_table is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //TBD
    }
    if (p_final_sharpen_both_three_d_table == NULL) {
        //TBD
    }
    return IK_ERR_0004;
}

uint32 img_ctx_set_fnlshpns_both_tdt(uint32 context_id, const ik_final_sharpen_both_three_d_table_t *p_final_sharpen_both_three_d_table)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;
    const ik_debug_check_func_t* p_check_func = ik_ctx_get_debug_check_func();
    if(p_check_func != NULL) {
        rval = p_check_func->final_sharpen_both_three_d_table(p_final_sharpen_both_three_d_table, "fnlshpns_both_tdt");
    }
    if(rval == IK_OK) {
        rval |= img_ctx_get_context(context_id, &p_ctx);
        if((p_final_sharpen_both_three_d_table != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
            (void)amba_ik_system_lock_mutex(context_id, IK_WAIT_FOREVER);
            (void)amba_ik_system_memcpy(&p_ctx->filters.input_param.final_sharpen_both_three_d_table, p_final_sharpen_both_three_d_table, sizeof(ik_final_sharpen_both_three_d_table_t));
            ctx_raise_update_flag(&p_ctx->filters.update_flags.liso.fnlshpns_both_tdt_updated);
            p_ctx->filters.input_param.three_d_table_enable = 1U;
            (void)amba_ik_system_unlock_mutex(context_id);
        } else {
            if(p_ctx->organization.initial_flag == 0U) {
                amba_ik_system_print_uint32_5("[IK][ERROR] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
                rval |= IK_ERR_0002;
            } else {
                amba_ik_system_print_str_5("[IK][ERROR] can't get context\n", NULL, NULL, NULL, NULL, NULL);
            }
        }
    }
    return rval;

}

uint32 img_ctx_ivd_set_chroma_filter(uint32 context_id, const ik_chroma_filter_t *p_chroma_filter)
{
    amba_ik_system_print_str_5("[IK] set_chroma_filter is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //TBD
    }
    if (p_chroma_filter==NULL) {
        //TBD
    }
    return IK_ERR_0004;
}

uint32 img_ctx_set_chroma_filter(uint32 context_id, const ik_chroma_filter_t *p_chroma_filter)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;
    const ik_debug_check_func_t* p_check_func = ik_ctx_get_debug_check_func();
    if(p_check_func != NULL) {
        rval = p_check_func->chroma_filter(p_chroma_filter, "chroma_filter");
    }
    if(rval == IK_OK) {
        rval |= img_ctx_get_context(context_id, &p_ctx);
        if((p_chroma_filter != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
            (void)amba_ik_system_lock_mutex(context_id, IK_WAIT_FOREVER);
            (void)amba_ik_system_memcpy(&p_ctx->filters.input_param.chroma_filter, p_chroma_filter, sizeof(ik_chroma_filter_t));
            ctx_raise_update_flag(&p_ctx->filters.update_flags.liso.chroma_filter_updated);
            (void)amba_ik_system_unlock_mutex(context_id);
        } else {
            if(p_ctx->organization.initial_flag == 0U) {
                amba_ik_system_print_uint32_5("[IK][ERROR] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
                rval |= IK_ERR_0002;
            } else {
                amba_ik_system_print_str_5("[IK][ERROR] can't get context\n", NULL, NULL, NULL, NULL, NULL);
            }
        }
    }
    return rval;
}

uint32 img_ctx_ivd_set_grgb_mismatch(uint32 context_id, const ik_grgb_mismatch_t *p_grgb_mismatch)
{
    uint32 rval = IK_ERR_0004;
    amba_ik_system_print_str_5("[IK] set_grgb_mismatch is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id > img_arch_get_context_number()) {
        // misraC
    }
    if (p_grgb_mismatch==NULL) {
        // misraC
    }
    return rval;
}

uint32 img_ctx_set_grgb_mismatch(uint32 context_id, const ik_grgb_mismatch_t *p_grgb_mismatch)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;
    const ik_debug_check_func_t* p_check_func = ik_ctx_get_debug_check_func();
    if(p_check_func != NULL) {
        rval = p_check_func->grgb_mismatch(p_grgb_mismatch, "grgb_mismatch");
    }
    if(rval == IK_OK) {
        rval |= img_ctx_get_context(context_id, &p_ctx);
        if((p_grgb_mismatch != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
            (void)amba_ik_system_lock_mutex(context_id, IK_WAIT_FOREVER);
            (void)amba_ik_system_memcpy(&p_ctx->filters.input_param.grgb_mismatch, p_grgb_mismatch, sizeof(ik_grgb_mismatch_t));
            ctx_raise_update_flag(&p_ctx->filters.update_flags.liso.grgb_mismatch_updated);
            (void)amba_ik_system_unlock_mutex(context_id);
        } else {
            if(p_ctx->organization.initial_flag == 0U) {
                amba_ik_system_print_uint32_5("[IK][ERROR] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
                rval = IK_ERR_0002;
            } else {
                amba_ik_system_print_str_5("[IK][ERROR] can't get context\n", NULL, NULL, NULL, NULL, NULL);
            }
        }
    }
    return rval;
}

uint32 img_ctx_ivd_set_video_mctf(uint32 context_id, const ik_video_mctf_t *p_video_mctf)
{
    amba_ik_system_print_str_5("[IK] set_video_mctf is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //TBD
    }
    if (p_video_mctf==NULL) {
        //TBD
    }
    return IK_ERR_0004;
}


uint32 img_ctx_set_video_mctf(uint32 context_id, const ik_video_mctf_t *p_video_mctf)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;
    rval |= img_ctx_get_context(context_id, &p_ctx);
    if((p_video_mctf != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        const ik_debug_check_func_t* p_check_func = ik_ctx_get_debug_check_func();
        if(p_check_func != NULL) {
            rval = p_check_func->video_mctf(p_video_mctf, p_ctx->filters.input_param.func_mode, "video_mctf");
        }
        if(rval == IK_OK) {
            (void)amba_ik_system_lock_mutex(context_id, IK_WAIT_FOREVER);
            (void)amba_ik_system_memcpy(&p_ctx->filters.input_param.video_mctf, p_video_mctf, sizeof(ik_video_mctf_t));
            ctx_raise_update_flag(&p_ctx->filters.update_flags.liso.video_mctf_updated);
            (void)amba_ik_system_unlock_mutex(context_id);
        }
    } else {
        if(p_ctx->organization.initial_flag == 0U) {
            amba_ik_system_print_uint32_5("[IK][ERROR] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
            rval |= IK_ERR_0002;
        } else {
            amba_ik_system_print_str_5("[IK][ERROR] can't get context\n", NULL, NULL, NULL, NULL, NULL);
        }
    }
    return rval;
}

uint32 img_ctx_ivd_set_mctf_ta(uint32 context_id, const ik_video_mctf_ta_t *p_video_mctf_ta)
{
    amba_ik_system_print_str_5("[IK] set_video_mctf_ta is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //TBD
    }
    if (p_video_mctf_ta==NULL) {
        //TBD
    }
    return IK_ERR_0004;
}


uint32 img_ctx_set_video_mctf_ta(uint32 context_id, const ik_video_mctf_ta_t *p_video_mctf_ta)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;
    const ik_debug_check_func_t* p_check_func = ik_ctx_get_debug_check_func();
    if(p_check_func != NULL) {
        rval = p_check_func->video_mctf_ta(p_video_mctf_ta, "video_mctf_ta");
    }
    if(rval == IK_OK) {
        rval |= img_ctx_get_context(context_id, &p_ctx);
        if((p_video_mctf_ta != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
            (void)amba_ik_system_lock_mutex(context_id, IK_WAIT_FOREVER);
            (void)amba_ik_system_memcpy(&p_ctx->filters.input_param.video_mctf_ta, p_video_mctf_ta, sizeof(ik_video_mctf_ta_t));
            ctx_raise_update_flag(&p_ctx->filters.update_flags.liso.video_mctf_ta_updated);
            (void)amba_ik_system_unlock_mutex(context_id);
        } else {
            if(p_ctx->organization.initial_flag == 0U) {
                amba_ik_system_print_uint32_5("[IK][ERROR] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
                rval |= IK_ERR_0002;
            } else {
                amba_ik_system_print_str_5("[IK][ERROR] can't get context\n", NULL, NULL, NULL, NULL, NULL);
            }
        }
    }
    return rval;
}

uint32 img_ctx_ivd_set_mctf_and_final_sharpen(uint32 context_id, const ik_pos_dep33_t *p_video_mctf_and_final_sharpen)
{
    amba_ik_system_print_str_5("[IK] img_ctx_set_video_mctf_and_final_sharpen is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //TBD
    }
    if (p_video_mctf_and_final_sharpen == NULL) {
        //TBD
    }
    return IK_ERR_0004;
}

uint32 img_ctx_set_video_mctf_and_final_sharpen(uint32 context_id, const ik_pos_dep33_t *p_video_mctf_and_final_sharpen)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;
    const ik_debug_check_func_t* p_check_func = ik_ctx_get_debug_check_func();
    if(p_check_func != NULL) {
        rval = p_check_func->video_mctf_and_fnlshp(p_video_mctf_and_final_sharpen, "video_mctf_and_final_sharpen");
    }
    if(rval == IK_OK) {
        rval |= img_ctx_get_context(context_id, &p_ctx);
        if((p_video_mctf_and_final_sharpen != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
            (void)amba_ik_system_lock_mutex(context_id, IK_WAIT_FOREVER);
            (void)amba_ik_system_memcpy(&p_ctx->filters.input_param.video_mctf_and_final_sharpen, p_video_mctf_and_final_sharpen, sizeof(ik_pos_dep33_t));
            ctx_raise_update_flag(&p_ctx->filters.update_flags.liso.video_mctf_and_final_sharpen_updated);
            (void)amba_ik_system_unlock_mutex(context_id);
        } else {
            if(p_ctx->organization.initial_flag == 0U) {
                amba_ik_system_print_uint32_5("[IK][ERROR] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
                rval |= IK_ERR_0002;
            } else {
                amba_ik_system_print_str_5("[IK][ERROR] can't get context\n", NULL, NULL, NULL, NULL, NULL);
            }
        }
    }
    return rval;
}
#if 0
uint32 img_ctx_ivd_set_idsp_debug_internal(uint32 context_id, const ik_idsp_debug_internal_t *p_idsp_debug_internal)
{
    amba_ik_system_print_str_5("[IK] set_idsp_debug_internal is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //TBD
    }
    if (p_idsp_debug_internal == NULL) {
        //TBD
    }
    return IK_ERR_0004;
}


uint32 img_ctx_set_idsp_debug_internal(uint32 context_id, const ik_idsp_debug_internal_t *p_idsp_debug_internal)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval |= img_ctx_get_context(context_id, &p_ctx);
    if((p_idsp_debug_internal != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        (void)amba_ik_system_lock_mutex(context_id, IK_WAIT_FOREVER);
        (void)amba_ik_system_memcpy(&p_ctx->filters.input_param.internal_idsp_debug, p_idsp_debug_internal, sizeof(ik_idsp_debug_internal_t));
        ctx_raise_update_flag(&p_ctx->filters.update_flags.liso.internal_idsp_debug_updated);
        (void)amba_ik_system_unlock_mutex(context_id);
    } else {
        if(p_ctx->organization.initial_flag == 0U) {
            amba_ik_system_print_uint32_5("[IK][ERROR] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
            rval = IK_ERR_0002;
        } else {
            amba_ik_system_print_str_5("[IK][ERROR] can't get context\n", NULL, NULL, NULL, NULL, NULL);
        }
    }
    return rval;
}
#endif

uint32 img_ctx_ivd_set_mctf_internal(uint32 context_id, const ik_mctf_internal_t *p_video_mctf_internal)
{
    amba_ik_system_print_str_5("[IK] set_video_mctf_internal is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //TBD
    }
    if (p_video_mctf_internal == NULL) {
        //TBD
    }
    return IK_ERR_0004;
}

uint32 img_ctx_set_video_mctf_internal(uint32 context_id, const ik_mctf_internal_t *p_video_mctf_internal)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;
    const ik_debug_check_func_t* p_check_func = ik_ctx_get_debug_check_func();
    if(p_check_func != NULL) {
        rval = p_check_func->video_mctf_internal(p_video_mctf_internal, "video_mctf_internal");
    }
    if(rval == IK_OK) {
        rval |= img_ctx_get_context(context_id, &p_ctx);
        if((p_video_mctf_internal != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
            (void)amba_ik_system_lock_mutex(context_id, IK_WAIT_FOREVER);
            (void)amba_ik_system_memcpy(&p_ctx->filters.input_param.internal_video_mctf, p_video_mctf_internal, sizeof(ik_mctf_internal_t));
            ctx_raise_update_flag(&p_ctx->filters.update_flags.liso.internal_video_mctf_updated);
            (void)amba_ik_system_unlock_mutex(context_id);
        } else {
            if(p_ctx->organization.initial_flag == 0U) {
                amba_ik_system_print_uint32_5("[IK][ERROR] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
                rval |= IK_ERR_0002;
            } else {
                amba_ik_system_print_str_5("[IK][ERROR] can't get context\n", NULL, NULL, NULL, NULL, NULL);
            }
        }
    }
    return rval;
}

uint32 img_ctx_ivd_set_motion_detect(uint32 context_id, const ik_motion_detect_t *p_motion_detect)
{
    amba_ik_system_print_str_5("[IK] img_set_motion_detect is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //TBD
    }
    if (p_motion_detect == NULL) {
        //TBD
    }
    return IK_ERR_0004;
}

uint32 img_ctx_set_motion_detect(uint32 context_id, const ik_motion_detect_t *p_motion_detect)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;
    const ik_debug_check_func_t* p_check_func = ik_ctx_get_debug_check_func();
    if(p_check_func != NULL) {
        rval = p_check_func->motion_detect(p_motion_detect, "motion_detect");
    }
    if(rval == IK_OK) {
        rval |= img_ctx_get_context(context_id, &p_ctx);
        if((p_motion_detect != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
            (void)amba_ik_system_lock_mutex(context_id, IK_WAIT_FOREVER);
            (void)amba_ik_system_memcpy(&p_ctx->filters.input_param.motion_detect, p_motion_detect, sizeof(ik_motion_detect_t));
            ctx_raise_update_flag(&p_ctx->filters.update_flags.liso.motion_detect_updated);
            (void)amba_ik_system_unlock_mutex(context_id);
        } else {
            if(p_ctx->organization.initial_flag == 0U) {
                amba_ik_system_print_uint32_5("[IK][ERROR] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
                rval |= IK_ERR_0002;
            } else {
                amba_ik_system_print_str_5("[IK][ERROR] can't get context\n", NULL, NULL, NULL, NULL, NULL);
            }
        }
    }
    return rval;
}


uint32 img_ctx_ivd_set_motion_detect_pos_dep(uint32 context_id, const ik_pos_dep33_t *p_md_pos_dep)
{
    amba_ik_system_print_str_5("[IK] img_set_motion_detect is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //TBD
    }
    if (p_md_pos_dep == NULL) {
        //TBD
    }
    return IK_ERR_0004;
}

uint32 img_ctx_set_motion_detect_pos_dep(uint32 context_id, const ik_pos_dep33_t *p_md_pos_dep)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;
    const ik_debug_check_func_t* p_check_func = ik_ctx_get_debug_check_func();
    if(p_check_func != NULL) {
        rval = p_check_func->motion_detect_pos_dep(p_md_pos_dep, "motion_detect_pos_dep");
    }
    if(rval == IK_OK) {
        rval |= img_ctx_get_context(context_id, &p_ctx);
        if((p_md_pos_dep != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
            (void)amba_ik_system_lock_mutex(context_id, IK_WAIT_FOREVER);
            (void)amba_ik_system_memcpy(&p_ctx->filters.input_param.motion_detect_pos_dep, p_md_pos_dep, sizeof(ik_pos_dep33_t));
            ctx_raise_update_flag(&p_ctx->filters.update_flags.liso.motion_detect_pos_dep_updated);
            (void)amba_ik_system_unlock_mutex(context_id);
        } else {
            if(p_ctx->organization.initial_flag == 0U) {
                amba_ik_system_print_uint32_5("[IK][ERROR] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
                rval |= IK_ERR_0002;
            } else {
                amba_ik_system_print_str_5("[IK][ERROR] can't get context\n", NULL, NULL, NULL, NULL, NULL);
            }
        }
    }
    return rval;
}

uint32 img_ctx_ivd_set_motion_detect_and_mctf(uint32 context_id, const ik_motion_detect_and_mctf_t *p_motion_detect_and_mctf)
{
    amba_ik_system_print_str_5("[IK] img_set_motion_detect is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //TBD
    }
    if (p_motion_detect_and_mctf == NULL) {
        //TBD
    }
    return IK_ERR_0004;
}

uint32 img_ctx_set_motion_detect_and_mctf(uint32 context_id, const ik_motion_detect_and_mctf_t *p_motion_detect_and_mctf)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;
    const ik_debug_check_func_t* p_check_func = ik_ctx_get_debug_check_func();
    if(p_check_func != NULL) {
        rval = p_check_func->motion_detect_and_mctf(p_motion_detect_and_mctf, "motion_detect_and_mctf");
    }
    if(rval == IK_OK) {
        rval |= img_ctx_get_context(context_id, &p_ctx);
        if((p_motion_detect_and_mctf != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
            (void)amba_ik_system_lock_mutex(context_id, IK_WAIT_FOREVER);
            (void)amba_ik_system_memcpy(&p_ctx->filters.input_param.motion_detect_and_mctf, p_motion_detect_and_mctf, sizeof(ik_motion_detect_and_mctf_t));
            ctx_raise_update_flag(&p_ctx->filters.update_flags.liso.motion_detect_and_mctf_updated);
            (void)amba_ik_system_unlock_mutex(context_id);
        } else {
            if(p_ctx->organization.initial_flag == 0U) {
                amba_ik_system_print_uint32_5("[IK][ERROR] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
                rval = IK_ERR_0002;
            } else {
                amba_ik_system_print_str_5("[IK][ERROR] can't get context\n", NULL, NULL, NULL, NULL, NULL);
            }
        }
    }
    return rval;
}
#if 0
uint32 img_ctx_ivd_set_deferred_blc(uint32 context_id, const ik_deferred_blc_level_t *p_deferred_blc)
{
    amba_ik_system_print_str_5("[IK] set_deferred_blc is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //TBD
    }
    if (p_deferred_blc == NULL) {
        //TBD
    }
    return IK_ERR_0004;
}

uint32 img_ctx_set_deferred_blc(uint32 context_id, const ik_deferred_blc_level_t *p_deferred_blc)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;
    const ik_debug_check_func_t* p_check_func = ik_ctx_get_debug_check_func();
    if(p_check_func != NULL) {
        rval = p_check_func->deferred_blc(p_deferred_blc, "deferred_blc");
    }
    if(rval == IK_OK) {
        rval |= img_ctx_get_context(context_id, &p_ctx);
        if((p_deferred_blc != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
            (void)amba_ik_system_lock_mutex(context_id, IK_WAIT_FOREVER);
            (void)amba_ik_system_memcpy(&p_ctx->filters.input_param.deferred_blc, p_deferred_blc, sizeof(ik_deferred_blc_level_t));
            ctx_raise_update_flag(&p_ctx->filters.update_flags.liso.deferred_blc_updated);
            (void)amba_ik_system_unlock_mutex(context_id);
        } else {
            if(p_ctx->organization.initial_flag == 0U) {
                amba_ik_system_print_uint32_5("[IK][ERROR] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
                rval = IK_ERR_0002;
            } else {
                amba_ik_system_print_str_5("[IK][ERROR] can't get context\n", NULL, NULL, NULL, NULL, NULL);
            }
        }
    }
    return rval;
}
#endif

uint32 img_ctx_ivd_set_aaa_stat_info(uint32 context_id, const ik_aaa_stat_info_t *p_stat_info)
{
    amba_ik_system_print_str_5("[IK] set_aaa_stat_info is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //TBD
    }
    if (p_stat_info == NULL) {
        //TBD
    }
    return IK_ERR_0004;
}

uint32 img_ctx_set_aaa_stat_info(uint32 context_id, const ik_aaa_stat_info_t *p_stat_info)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;
    const ik_debug_check_func_t* p_check_func = ik_ctx_get_debug_check_func();
    if(p_check_func != NULL) {
        rval = p_check_func->aaa_stat(p_stat_info, "aaa_stat_info");
    }
    if(rval == IK_OK) {
        rval |= img_ctx_get_context(context_id, &p_ctx);
        if((p_stat_info != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
            (void)amba_ik_system_lock_mutex(context_id, IK_WAIT_FOREVER);
            (void)amba_ik_system_memcpy(&p_ctx->filters.input_param.aaa_stat_info, p_stat_info, sizeof(ik_aaa_stat_info_t));
            ctx_raise_update_flag(&p_ctx->filters.update_flags.liso.aaa_stat_info_updated);
            (void)amba_ik_system_unlock_mutex(context_id);
        } else {
            if(p_ctx->organization.initial_flag == 0U) {
                amba_ik_system_print_uint32_5("[IK][ERROR] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
                rval = IK_ERR_0002;
            } else {
                amba_ik_system_print_str_5("[IK][ERROR] can't get context\n", NULL, NULL, NULL, NULL, NULL);
            }
        }
    }
    return rval;
}
uint32 img_ctx_ivd_set_af_stat_ex_info(uint32 context_id, const ik_af_stat_ex_info_t *p_af_stat_ex_info)
{
    amba_ik_system_print_str_5("[IK] set_af_ex_info is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //TBD
    }
    if (p_af_stat_ex_info == NULL) {
        //TBD
    }
    return IK_ERR_0004;
}

uint32 img_ctx_set_af_stat_ex_info(uint32 context_id, const ik_af_stat_ex_info_t *p_af_stat_ex_info)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;
    const ik_debug_check_func_t* p_check_func = ik_ctx_get_debug_check_func();
    if(p_check_func != NULL) {
        rval = p_check_func->af_stat_ex(p_af_stat_ex_info, "af_stat_ex_info");
    }
    if(rval == IK_OK) {
        rval |= img_ctx_get_context(context_id, &p_ctx);
        if((p_af_stat_ex_info != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
            (void)amba_ik_system_lock_mutex(context_id, IK_WAIT_FOREVER);
            (void)amba_ik_system_memcpy(&p_ctx->filters.input_param.af_stat_ex_info, p_af_stat_ex_info, sizeof(ik_af_stat_ex_info_t));
            ctx_raise_update_flag(&p_ctx->filters.update_flags.liso.af_stat_ex_info_updated);
            (void)amba_ik_system_unlock_mutex(context_id);
        } else {
            if(p_ctx->organization.initial_flag == 0U) {
                amba_ik_system_print_uint32_5("[IK][ERROR] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
                rval = IK_ERR_0002;
            } else {
                amba_ik_system_print_str_5("[IK][ERROR] can't get context\n", NULL, NULL, NULL, NULL, NULL);
            }
        }
    }
    return rval;
}

uint32 img_ctx_ivd_set_pg_af_stat_ex_info(uint32 context_id, const ik_pg_af_stat_ex_info_t *p_pg_af_stat_ex_info)
{
    amba_ik_system_print_str_5("[IK] set_af_ex_info is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //TBD
    }
    if (p_pg_af_stat_ex_info == NULL) {
        //TBD
    }
    return IK_ERR_0004;
}

uint32 img_ctx_set_pg_af_stat_ex_info(uint32 context_id, const ik_pg_af_stat_ex_info_t *p_pg_af_stat_ex_info)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;
    const ik_debug_check_func_t* p_check_func = ik_ctx_get_debug_check_func();
    if(p_check_func != NULL) {
        rval = p_check_func->pg_af_stat_ex(p_pg_af_stat_ex_info, "pg_af_stat_ex_info");
    }
    if(rval == IK_OK) {
        rval |= img_ctx_get_context(context_id, &p_ctx);
        if((p_pg_af_stat_ex_info != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
            (void)amba_ik_system_lock_mutex(context_id, IK_WAIT_FOREVER);
            (void)amba_ik_system_memcpy(&p_ctx->filters.input_param.pg_af_stat_ex_info, p_pg_af_stat_ex_info, sizeof(ik_pg_af_stat_ex_info_t));
            ctx_raise_update_flag(&p_ctx->filters.update_flags.liso.pg_af_stat_ex_info_updated);
            (void)amba_ik_system_unlock_mutex(context_id);
        } else {
            if(p_ctx->organization.initial_flag == 0U) {
                amba_ik_system_print_uint32_5("[IK][ERROR] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
                rval = IK_ERR_0002;
            } else {
                amba_ik_system_print_str_5("[IK][ERROR] can't get context\n", NULL, NULL, NULL, NULL, NULL);
            }
        }
    }
    return rval;
}

uint32 img_ctx_ivd_set_aaa_stat_float_tile_info(uint32 context_id, const ik_aaa_float_info_t *p_float_tile_info)
{
    amba_ik_system_print_str_5("[IK] set_aaa_stat_float_tile_info is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //TBD
    }
    if (p_float_tile_info == NULL) {
        //TBD
    }
    return IK_ERR_0004;
}

uint32 img_ctx_set_aaa_stat_float_tile_info(uint32 context_id, const ik_aaa_float_info_t *p_float_tile_info)
{
    // TODO:
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;
    const ik_debug_check_func_t* p_check_func = ik_ctx_get_debug_check_func();
    if(p_check_func != NULL) {
        rval = p_check_func->stat_float_tile(p_float_tile_info, "aaa_stat_float_tile_info");
    }
    if(rval == IK_OK) {
        rval |= img_ctx_get_context(context_id, &p_ctx);
        if((p_float_tile_info != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
            (void)amba_ik_system_lock_mutex(context_id, IK_WAIT_FOREVER);
            (void)amba_ik_system_memcpy(&p_ctx->filters.input_param.aaa_float_tile_info, p_float_tile_info, sizeof(ik_aaa_float_info_t));
            ctx_raise_update_flag(&p_ctx->filters.update_flags.liso.aaa_float_tile_info_updated);
            (void)amba_ik_system_unlock_mutex(context_id);
        } else {
            if(p_ctx->organization.initial_flag == 0U) {
                amba_ik_system_print_uint32_5("[IK][ERROR] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
                rval = IK_ERR_0002;
            } else {
                amba_ik_system_print_str_5("[IK][ERROR] can't get context\n", NULL, NULL, NULL, NULL, NULL);
            }
        }
    }
    return rval;
}

uint32 img_ctx_ivd_set_window_size_info(uint32 context_id, const ik_window_size_info_t *p_window_size_info)
{
    amba_ik_system_print_str_5("[IK] set_window_size_info_gain is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //TBD
    }
    if (p_window_size_info == NULL) {
        //TBD
    }
    return IK_ERR_0004;
}

uint32 img_ctx_set_window_size_info(uint32 context_id, const ik_window_size_info_t *p_window_size_info)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;
    const ik_debug_check_func_t* p_check_func = ik_ctx_get_debug_check_func();
    if(p_check_func != NULL) {
        rval = p_check_func->window_size_info(p_window_size_info, "window_size_info");
    }
    if(rval == IK_OK) {
        rval |= img_ctx_get_context(context_id, &p_ctx);
        if((p_window_size_info != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
            (void)amba_ik_system_lock_mutex(context_id, IK_WAIT_FOREVER);
            (void)amba_ik_system_memcpy(&p_ctx->filters.input_param.window_size_info, p_window_size_info, sizeof(ik_window_size_info_t));
            ctx_raise_update_flag(&p_ctx->filters.update_flags.liso.window_size_info_updated);
            (void)amba_ik_system_unlock_mutex(context_id);
        } else {
            if(p_ctx->organization.initial_flag == 0U) {
                amba_ik_system_print_uint32_5("[IK][ERROR] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
                rval = IK_ERR_0002;
            } else {
                amba_ik_system_print_str_5("[IK][ERROR] can't get context\n", NULL, NULL, NULL, NULL, NULL);
            }
        }
    }
    return rval;
}

uint32 img_ctx_ivd_set_cfa_window_size_info(uint32 context_id, const ik_cfa_window_size_info_t *p_cfa_window_size_info)
{
    amba_ik_system_print_str_5("[IK] set_cfa_window_size_info is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //TBD
    }
    if (p_cfa_window_size_info == NULL) {
        //TBD
    }
    return IK_ERR_0004;
}

uint32 img_ctx_set_cfa_window_size_info(uint32 context_id, const ik_cfa_window_size_info_t *p_cfa_window_size_info)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;
    const ik_debug_check_func_t* p_check_func = ik_ctx_get_debug_check_func();
    if(p_check_func != NULL) {
        rval = p_check_func->cfa_window_size_info(p_cfa_window_size_info, "cfa_window_size_info");
    }
    if(rval == IK_OK) {
        rval |= img_ctx_get_context(context_id, &p_ctx);
        if((p_cfa_window_size_info != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
            (void)amba_ik_system_lock_mutex(context_id, IK_WAIT_FOREVER);
            (void)amba_ik_system_memcpy(&p_ctx->filters.input_param.cfa_window_size_info, p_cfa_window_size_info, sizeof(ik_cfa_window_size_info_t));
            ctx_raise_update_flag(&p_ctx->filters.update_flags.liso.cfa_window_size_info_updated);
            (void)amba_ik_system_unlock_mutex(context_id);
        } else {
            if(p_ctx->organization.initial_flag == 0U) {
                amba_ik_system_print_uint32_5("[IK][ERROR] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
                rval = IK_ERR_0002;
            } else {
                amba_ik_system_print_str_5("[IK][ERROR] can't get context\n", NULL, NULL, NULL, NULL, NULL);
            }
        }
    }
    return rval;
}

uint32 img_ctx_ivd_set_fe_tone_curve(uint32 context_id, const ik_frontend_tone_curve_t *p_fe_tc)
{
    amba_ik_system_print_str_5("[IK] img_ctx_ivd_set_fe_tone_curve is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //MISRA
    }
    if (p_fe_tc == NULL) {
        //MISRA
    }
    return IK_ERR_0004;
}

uint32 img_ctx_set_fe_tone_curve(uint32 context_id, const ik_frontend_tone_curve_t *p_fe_tc)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;
    const ik_debug_check_func_t* p_check_func = ik_ctx_get_debug_check_func();
    if(p_check_func != NULL) {
        rval = p_check_func->fe_tone_curve(p_fe_tc, "fe_tone_curve");
    }
    if(rval == IK_OK) {
        rval |= img_ctx_get_context(context_id, &p_ctx);
        if((p_fe_tc != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
            (void)amba_ik_system_lock_mutex(context_id, IK_WAIT_FOREVER);
            (void)amba_ik_system_memcpy(&p_ctx->filters.input_param.fe_tone_curve, p_fe_tc, sizeof(ik_frontend_tone_curve_t));
            ctx_raise_update_flag(&p_ctx->filters.update_flags.liso.fe_tone_curve_updated);
            (void)amba_ik_system_unlock_mutex(context_id);
        } else {
            if(p_ctx->organization.initial_flag == 0U) {
                amba_ik_system_print_uint32_5("[IK][ERROR] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
                rval = IK_ERR_0002;
            } else {
                amba_ik_system_print_str_5("[IK][ERROR] can't get context\n", NULL, NULL, NULL, NULL, NULL);
            }
        }
    }
    return rval;
}

uint32 img_ctx_ivd_set_resampler_str(uint32 context_id, const ik_resampler_strength_t *p_resample_str)
{
    amba_ik_system_print_str_5("[IK] set_resampler_str is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //TBD
    }
    if (p_resample_str == NULL) {
        //TBD
    }
    return IK_ERR_0004;
}

uint32 img_ctx_set_resampler_str(uint32 context_id, const ik_resampler_strength_t *p_resample_str)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;
    const ik_debug_check_func_t* p_check_func = ik_ctx_get_debug_check_func();
    if(p_check_func != NULL) {
        rval = p_check_func->resamp_strength(p_resample_str, "resampler_str");
    }
    if(rval == IK_OK) {
        rval |= img_ctx_get_context(context_id, &p_ctx);
        if((p_resample_str != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
            (void)amba_ik_system_lock_mutex(context_id, IK_WAIT_FOREVER);
            (void)amba_ik_system_memcpy(&p_ctx->filters.input_param.resample_str, p_resample_str, sizeof(ik_resampler_strength_t));
            ctx_raise_update_flag(&p_ctx->filters.update_flags.liso.resample_str_update);
            (void)amba_ik_system_unlock_mutex(context_id);
        } else {
            if(p_ctx->organization.initial_flag == 0U) {
                amba_ik_system_print_uint32_5("[IK][ERROR] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
                rval = IK_ERR_0002;
            } else {
                amba_ik_system_print_str_5("[IK][ERROR] can't get context\n", NULL, NULL, NULL, NULL, NULL);
            }
        }
    }
    return rval;
}

uint32 img_ctx_ivd_set_histogram_info(uint32 context_id, const ik_histogram_info_t *p_hist_info)
{
    uint32 rval = IK_ERR_0004;
    amba_ik_system_print_str_5("[IK] set_histogram_info is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //MISRAC
    }
    if (p_hist_info == NULL) {
        //MISRAC
    }
    return rval;
}

uint32 img_ctx_set_histogram_info(uint32 context_id, const ik_histogram_info_t *p_hist_info)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;
    const ik_debug_check_func_t* p_check_func = ik_ctx_get_debug_check_func();
    if(p_check_func != NULL) {
        rval = p_check_func->histogram_info(p_hist_info, "histogram_info");
    }
    if(rval == IK_OK) {
        rval |= img_ctx_get_context(context_id, &p_ctx);
        if((p_hist_info != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
            (void)amba_ik_system_lock_mutex(context_id, IK_WAIT_FOREVER);
            (void)amba_ik_system_memcpy(&p_ctx->filters.input_param.hist_info, p_hist_info, sizeof(ik_histogram_info_t));
            ctx_raise_update_flag(&p_ctx->filters.update_flags.liso.histogram_info_update);
            (void)amba_ik_system_unlock_mutex(context_id);
        } else {
            if(p_ctx->organization.initial_flag == 0U) {
                amba_ik_system_print_uint32_5("[IK][ERROR] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
                rval = IK_ERR_0002;
            } else {
                amba_ik_system_print_str_5("[IK][ERROR] can't get context\n", NULL, NULL, NULL, NULL, NULL);
            }
        }
    }
    return rval;
}

uint32 img_ctx_ivd_set_pseudo_y_info(uint32 context_id, const ik_aaa_pseudo_y_info_t *p_pseudo_y_info)
{
    uint32 rval = IK_ERR_0004;
    amba_ik_system_print_str_5("[IK] set_pseudo_y_info is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //MISRAC
    }
    if (p_pseudo_y_info == NULL) {
        //MISRAC
    }
    return rval;
}

uint32 img_ctx_set_pseudo_y_info(uint32 context_id, const ik_aaa_pseudo_y_info_t *p_pseudo_y_info)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;
    const ik_debug_check_func_t* p_check_func = ik_ctx_get_debug_check_func();
    if(p_check_func != NULL) {
        rval = p_check_func->pseudo_y_info(p_pseudo_y_info, "pseudo_y_info");
    }
    if(rval == IK_OK) {
        rval |= img_ctx_get_context(context_id, &p_ctx);
        if((p_pseudo_y_info != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
            (void)amba_ik_system_lock_mutex(context_id, IK_WAIT_FOREVER);
            (void)amba_ik_system_memcpy(&p_ctx->filters.input_param.pseudo_y, p_pseudo_y_info, sizeof(ik_aaa_pseudo_y_info_t));
            ctx_raise_update_flag(&p_ctx->filters.update_flags.liso.aaa_pseudo_y_update);
            (void)amba_ik_system_unlock_mutex(context_id);
        } else {
            if(p_ctx->organization.initial_flag == 0U) {
                amba_ik_system_print_uint32_5("[IK][ERROR] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
                rval = IK_ERR_0002;
            } else {
                amba_ik_system_print_str_5("[IK][ERROR] can't get context\n", NULL, NULL, NULL, NULL, NULL);
            }
        }
    }
    return rval;
}

uint32 img_ctx_ivd_set_pg_histogram_info(uint32 context_id, const ik_histogram_info_t *p_hist_info)
{
    amba_ik_system_print_str_5("[IK] set_pg_histogram_info is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //MISRAC
    }
    if (p_hist_info == NULL) {
        //MISRAC
    }
    return IK_ERR_0004;
}

uint32 img_ctx_set_pg_histogram_info(uint32 context_id, const ik_histogram_info_t *p_hist_info)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;
    const ik_debug_check_func_t* p_check_func = ik_ctx_get_debug_check_func();
    if(p_check_func != NULL) {
        rval = p_check_func->histogram_info(p_hist_info, "histogram_info");
    }
    if(rval == IK_OK) {
        rval |= img_ctx_get_context(context_id, &p_ctx);
        if((p_hist_info != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
            (void)amba_ik_system_lock_mutex(context_id, IK_WAIT_FOREVER);
            (void)amba_ik_system_memcpy(&p_ctx->filters.input_param.hist_info_pg, p_hist_info, sizeof(ik_histogram_info_t));
            ctx_raise_update_flag(&p_ctx->filters.update_flags.liso.histogram_info_pg_update);
            (void)amba_ik_system_unlock_mutex(context_id);
        } else {
            if(p_ctx->organization.initial_flag == 0U) {
                amba_ik_system_print_uint32_5("[IK][ERROR] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
                rval = IK_ERR_0002;
            } else {
                amba_ik_system_print_str_5("[IK][ERROR] can't get context\n", NULL, NULL, NULL, NULL, NULL);
            }
        }
    }
    return rval;
}

uint32 img_ctx_ivd_get_vin_sensor_info(uint32 context_id, ik_vin_sensor_info_t *p_sensor_info)
{
    amba_ik_system_print_str_5("[IK] get_vin_sensor_info is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //TBD
    }
    if (p_sensor_info==NULL) {
        //TBD
    } else {
        (void)amba_ik_system_memcpy(p_sensor_info, p_sensor_info, sizeof(uint8));
    }
    return IK_ERR_0004;
}

uint32 img_ctx_get_vin_sensor_info(uint32 context_id, ik_vin_sensor_info_t *p_sensor_info)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval |= img_ctx_get_context(context_id, &p_ctx);
    if((p_sensor_info != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        (void)amba_ik_system_lock_mutex(context_id, IK_WAIT_FOREVER);
        (void)amba_ik_system_memcpy(p_sensor_info, &p_ctx->filters.input_param.sensor_info, sizeof(ik_vin_sensor_info_t));
        (void)amba_ik_system_unlock_mutex(context_id);
    } else {
        if(p_sensor_info == NULL) {
            amba_ik_system_print_str_5("[IK][ERROR] NULL pointer in %s\n", __func__, NULL, NULL, NULL, NULL);
            rval = IK_ERR_0005;
        } else if(p_ctx->organization.initial_flag == 0U) {
            amba_ik_system_print_uint32_5("[IK][ERROR] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
            rval = IK_ERR_0002;
        } else {
            amba_ik_system_print_str_5("[IK][ERROR] can't get context\n", NULL, NULL, NULL, NULL, NULL);
        }
    }

    return rval;

}

uint32 img_ctx_ivd_get_global_dgain(uint32 context_id, ik_global_dgain_t *p_global_dgain)
{
    amba_ik_system_print_str_5("[IK] get_global_dgain is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //TBD
    }
    if (p_global_dgain == NULL) {
        //TBD
    } else {
        (void) amba_ik_system_memcpy(p_global_dgain, p_global_dgain, sizeof(uint8));
    }
    return IK_ERR_0004;
}

uint32 img_ctx_get_global_dgain(uint32 context_id, ik_global_dgain_t *p_global_dgain)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval |= img_ctx_get_context(context_id, &p_ctx);
    if((p_global_dgain != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        (void)amba_ik_system_lock_mutex(context_id, IK_WAIT_FOREVER);
        (void)amba_ik_system_memcpy(p_global_dgain, &p_ctx->filters.input_param.global_dgain, sizeof(ik_global_dgain_t));
        (void)amba_ik_system_unlock_mutex(context_id);
    } else {
        if(p_global_dgain == NULL) {
            amba_ik_system_print_str_5("[IK][ERROR] NULL pointer in %s\n", __func__, NULL, NULL, NULL, NULL);
            rval = IK_ERR_0005;
        } else if(p_ctx->organization.initial_flag == 0U) {
            amba_ik_system_print_uint32_5("[IK][ERROR] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
            rval = IK_ERR_0002;
        } else {
            amba_ik_system_print_str_5("[IK][ERROR] can't get context\n", NULL, NULL, NULL, NULL, NULL);
        }
    }

    return rval;
}

uint32 img_ctx_ivd_get_ae_gain(uint32 context_id, ik_ae_gain_t *p_ae_gain)
{
    amba_ik_system_print_str_5("[IK] get_ae_gain is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //TBD
    }
    if (p_ae_gain == NULL) {
        //TBD
    } else {
        (void)amba_ik_system_memcpy(p_ae_gain, p_ae_gain, sizeof(uint8));
    }
    return IK_ERR_0004;
}

uint32 img_ctx_get_ae_gain(uint32 context_id, ik_ae_gain_t *p_ae_gain)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval |= img_ctx_get_context(context_id, &p_ctx);
    if((p_ae_gain != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        (void)amba_ik_system_lock_mutex(context_id, IK_WAIT_FOREVER);
        (void)amba_ik_system_memcpy(p_ae_gain, &p_ctx->filters.input_param.ae_gain, sizeof(ik_ae_gain_t));
        (void)amba_ik_system_unlock_mutex(context_id);
    } else {
        if(p_ae_gain == NULL) {
            amba_ik_system_print_str_5("[IK][ERROR] NULL pointer in %s\n", __func__, NULL, NULL, NULL, NULL);
            rval = IK_ERR_0005;
        } else if(p_ctx->organization.initial_flag == 0U) {
            amba_ik_system_print_uint32_5("[IK][ERROR] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
            rval = IK_ERR_0002;
        } else {
            amba_ik_system_print_str_5("[IK][ERROR] can't get context\n", NULL, NULL, NULL, NULL, NULL);
        }
    }

    return rval;
}

uint32 img_ctx_ivd_get_before_ce_wb_gain(uint32 context_id, ik_wb_gain_t *p_before_ce_wb_gain)
{
    amba_ik_system_print_str_5("[IK] get_before_ce_wb_gain is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //TBD
    }
    if (p_before_ce_wb_gain == NULL) {
        //TBD
    } else {
        (void)amba_ik_system_memcpy(p_before_ce_wb_gain, p_before_ce_wb_gain, sizeof(uint8));
    }
    return IK_ERR_0004;
}

uint32 img_ctx_get_before_ce_wb_gain(uint32 context_id, ik_wb_gain_t *p_before_ce_wb_gain)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval |= img_ctx_get_context(context_id, &p_ctx);
    if((p_before_ce_wb_gain != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        (void)amba_ik_system_lock_mutex(context_id, IK_WAIT_FOREVER);
        (void)amba_ik_system_memcpy(p_before_ce_wb_gain, &p_ctx->filters.input_param.before_ce_wb_gain, sizeof(ik_wb_gain_t));
        (void)amba_ik_system_unlock_mutex(context_id);
    } else {
        if(p_before_ce_wb_gain == NULL) {
            amba_ik_system_print_str_5("[IK][ERROR] NULL pointer in %s\n", __func__, NULL, NULL, NULL, NULL);
            rval = IK_ERR_0005;
        } else if(p_ctx->organization.initial_flag == 0U) {
            amba_ik_system_print_uint32_5("[IK][ERROR] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
            rval = IK_ERR_0002;
        } else {
            amba_ik_system_print_str_5("[IK][ERROR] can't get context\n", NULL, NULL, NULL, NULL, NULL);
        }
    }

    return rval;
}

uint32 img_ctx_ivd_get_after_ce_wb_gain(uint32 context_id, ik_wb_gain_t *p_after_ce_wb_gain)
{
    amba_ik_system_print_str_5("[IK] get_after_ce_wb_gain is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //TBD
    }
    if (p_after_ce_wb_gain == NULL) {
        //TBD
    } else {
        (void)amba_ik_system_memcpy(p_after_ce_wb_gain, p_after_ce_wb_gain, sizeof(uint8));
    }
    return IK_ERR_0004;
}

uint32 img_ctx_get_after_ce_wb_gain(uint32 context_id, ik_wb_gain_t *p_after_ce_wb_gain)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval |= img_ctx_get_context(context_id, &p_ctx);
    if((p_after_ce_wb_gain != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        (void)amba_ik_system_lock_mutex(context_id, IK_WAIT_FOREVER);
        (void)amba_ik_system_memcpy(p_after_ce_wb_gain, &p_ctx->filters.input_param.after_ce_wb_gain, sizeof(ik_wb_gain_t));
        (void)amba_ik_system_unlock_mutex(context_id);
    } else {
        if(p_after_ce_wb_gain == NULL) {
            amba_ik_system_print_str_5("[IK][ERROR] NULL pointer in %s\n", __func__, NULL, NULL, NULL, NULL);
            rval = IK_ERR_0005;
        } else if(p_ctx->organization.initial_flag == 0U) {
            amba_ik_system_print_uint32_5("[IK][ERROR] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
            rval = IK_ERR_0002;
        } else {
            amba_ik_system_print_str_5("[IK][ERROR] can't get context\n", NULL, NULL, NULL, NULL, NULL);
        }
    }

    return rval;
}

uint32 img_ctx_ivd_get_cfa_leakage_filter(uint32 context_id, ik_cfa_leakage_filter_t *p_cfa_leakage_filter)
{
    amba_ik_system_print_str_5("[IK] get_cfa_leakage_filter is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //TBD
    }
    if (p_cfa_leakage_filter==NULL) {
        //TBD
    } else {
        (void)amba_ik_system_memcpy(p_cfa_leakage_filter, p_cfa_leakage_filter, sizeof(uint8));
    }
    return IK_ERR_0004;
}

uint32 img_ctx_get_cfa_leakage_filter(uint32 context_id, ik_cfa_leakage_filter_t *p_cfa_leakage_filter)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval |= img_ctx_get_context(context_id, &p_ctx);
    if((p_cfa_leakage_filter != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        (void)amba_ik_system_lock_mutex(context_id, IK_WAIT_FOREVER);
        (void)amba_ik_system_memcpy(p_cfa_leakage_filter, &p_ctx->filters.input_param.cfa_leakage_filter, sizeof(ik_cfa_leakage_filter_t));
        (void)amba_ik_system_unlock_mutex(context_id);
    } else {
        if(p_cfa_leakage_filter == NULL) {
            amba_ik_system_print_str_5("[IK][ERROR] NULL pointer in %s\n", __func__, NULL, NULL, NULL, NULL);
            rval = IK_ERR_0005;
        } else if(p_ctx->organization.initial_flag == 0U) {
            amba_ik_system_print_uint32_5("[IK][ERROR] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
            rval = IK_ERR_0002;
        } else {
            amba_ik_system_print_str_5("[IK][ERROR] can't get context\n", NULL, NULL, NULL, NULL, NULL);
        }
    }

    return rval;
}

uint32 img_ctx_ivd_get_anti_aliasing(uint32 context_id, ik_anti_aliasing_t *p_anti_aliasing)
{
    amba_ik_system_print_str_5("[IK] get_anti_aliasing is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //TBD
    }
    if (p_anti_aliasing == NULL) {
        //TBD
    } else {
        (void) amba_ik_system_memcpy(p_anti_aliasing, p_anti_aliasing, sizeof(uint8));
    }
    return IK_ERR_0004;
}

uint32 img_ctx_get_anti_aliasing(uint32 context_id, ik_anti_aliasing_t *p_anti_aliasing)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval |= img_ctx_get_context(context_id, &p_ctx);
    if((p_anti_aliasing != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        (void)amba_ik_system_lock_mutex(context_id, IK_WAIT_FOREVER);
        (void)amba_ik_system_memcpy(p_anti_aliasing, &p_ctx->filters.input_param.anti_aliasing, sizeof(ik_anti_aliasing_t));
        (void)amba_ik_system_unlock_mutex(context_id);
    } else {
        if(p_anti_aliasing == NULL) {
            amba_ik_system_print_str_5("[IK][ERROR] NULL pointer in %s\n", __func__, NULL, NULL, NULL, NULL);
            rval = IK_ERR_0005;
        } else if(p_ctx->organization.initial_flag == 0U) {
            amba_ik_system_print_uint32_5("[IK][ERROR] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
            rval = IK_ERR_0002;
        } else {
            amba_ik_system_print_str_5("[IK][ERROR] can't get context\n", NULL, NULL, NULL, NULL, NULL);
        }
    }

    return rval;
}

uint32 img_ctx_ivd_get_dynamic_bpc(uint32 context_id, ik_dynamic_bad_pixel_correction_t *p_dynamic_bpc)
{
    amba_ik_system_print_str_5("[IK] get_dynamic_bpc is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //TBD
    }
    if (p_dynamic_bpc == NULL) {
        //TBD
    } else {
        (void)amba_ik_system_memcpy(p_dynamic_bpc, p_dynamic_bpc, sizeof(uint8));
    }
    return IK_ERR_0004;
}

uint32 img_ctx_get_dynamic_bpc(uint32 context_id, ik_dynamic_bad_pixel_correction_t *p_dynamic_bpc)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval |= img_ctx_get_context(context_id, &p_ctx);
    if((p_dynamic_bpc != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        (void)amba_ik_system_lock_mutex(context_id, IK_WAIT_FOREVER);
        (void)amba_ik_system_memcpy(p_dynamic_bpc, &p_ctx->filters.input_param.dynamic_bpc, sizeof(ik_dynamic_bad_pixel_correction_t));
        (void)amba_ik_system_unlock_mutex(context_id);
    } else {
        if(p_dynamic_bpc == NULL) {
            amba_ik_system_print_str_5("[IK][ERROR] NULL pointer in %s\n", __func__, NULL, NULL, NULL, NULL);
            rval = IK_ERR_0005;
        } else if(p_ctx->organization.initial_flag == 0U) {
            amba_ik_system_print_uint32_5("[IK][ERROR] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
            rval = IK_ERR_0002;
        } else {
            amba_ik_system_print_str_5("[IK][ERROR] can't get context\n", NULL, NULL, NULL, NULL, NULL);
        }
    }

    return rval;
}

uint32 img_ctx_ivd_get_cfa_noise_filter(uint32 context_id, ik_cfa_noise_filter_t *p_cfa_noise_filter)
{
    amba_ik_system_print_str_5("[IK] get_cfa_noise_filter is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //TBD
    }
    if (p_cfa_noise_filter == NULL) {
        //TBD
    } else {
        (void)amba_ik_system_memcpy(p_cfa_noise_filter, p_cfa_noise_filter, sizeof(uint8));
    }
    return IK_ERR_0004;
}

uint32 img_ctx_get_cfa_noise_filter(uint32 context_id, ik_cfa_noise_filter_t *p_cfa_noise_filter)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval |= img_ctx_get_context(context_id, &p_ctx);
    if((p_cfa_noise_filter != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        (void)amba_ik_system_lock_mutex(context_id, IK_WAIT_FOREVER);
        (void)amba_ik_system_memcpy(p_cfa_noise_filter, &p_ctx->filters.input_param.cfa_noise_filter, sizeof(ik_cfa_noise_filter_t));
        (void)amba_ik_system_unlock_mutex(context_id);
    } else {
        if(p_cfa_noise_filter == NULL) {
            amba_ik_system_print_str_5("[IK][ERROR] NULL pointer in %s\n", __func__, NULL, NULL, NULL, NULL);
            rval = IK_ERR_0005;
        } else if(p_ctx->organization.initial_flag == 0U) {
            amba_ik_system_print_uint32_5("[IK][ERROR] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
            rval = IK_ERR_0002;
        } else {
            amba_ik_system_print_str_5("[IK][ERROR] can't get context\n", NULL, NULL, NULL, NULL, NULL);
        }
    }

    return rval;
}

uint32 img_ctx_ivd_get_dgain_sat_lvl(uint32 context_id, ik_dgain_saturation_level_t *p_dgain_saturation_level)
{
    amba_ik_system_print_str_5("[IK] get_dgain_sat_lvl is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //TBD
    }
    if (p_dgain_saturation_level == NULL) {
        //TBD
    } else {
        (void)amba_ik_system_memcpy(p_dgain_saturation_level, p_dgain_saturation_level, sizeof(uint8));
    }
    return IK_ERR_0004;
}

uint32 img_ctx_get_dgain_sat_lvl(uint32 context_id, ik_dgain_saturation_level_t *p_dgain_saturation_level)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval |= img_ctx_get_context(context_id, &p_ctx);
    if((p_dgain_saturation_level != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        (void)amba_ik_system_lock_mutex(context_id, IK_WAIT_FOREVER);
        (void)amba_ik_system_memcpy(p_dgain_saturation_level, &p_ctx->filters.input_param.dgain_sauration_level, sizeof(ik_dgain_saturation_level_t));
        (void)amba_ik_system_unlock_mutex(context_id);
    } else {
        if(p_dgain_saturation_level == NULL) {
            amba_ik_system_print_str_5("[IK][ERROR] NULL pointer in %s\n", __func__, NULL, NULL, NULL, NULL);
            rval = IK_ERR_0005;
        } else if(p_ctx->organization.initial_flag == 0U) {
            amba_ik_system_print_uint32_5("[IK][ERROR] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
            rval = IK_ERR_0002;
        } else {
            amba_ik_system_print_str_5("[IK][ERROR] can't get context\n", NULL, NULL, NULL, NULL, NULL);
        }
    }

    return rval;
}

uint32 img_ctx_ivd_get_demosaic(uint32 context_id, ik_demosaic_t *p_demosaic)
{
    amba_ik_system_print_str_5("[IK] get_demosaic is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //TBD
    }
    if (p_demosaic == NULL) {
        //TBD
    } else {
        (void)amba_ik_system_memcpy(p_demosaic, p_demosaic, sizeof(uint8));
    }
    return IK_ERR_0004;
}

uint32 img_ctx_get_demosaic(uint32 context_id, ik_demosaic_t *p_demosaic)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval |= img_ctx_get_context(context_id, &p_ctx);
    if((p_demosaic != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        (void)amba_ik_system_lock_mutex(context_id, IK_WAIT_FOREVER);
        (void)amba_ik_system_memcpy(p_demosaic, &p_ctx->filters.input_param.demosaic, sizeof(ik_demosaic_t));
        (void)amba_ik_system_unlock_mutex(context_id);
    } else {
        if(p_demosaic == NULL) {
            amba_ik_system_print_str_5("[IK][ERROR] NULL pointer in %s\n", __func__, NULL, NULL, NULL, NULL);
            rval = IK_ERR_0005;
        } else if(p_ctx->organization.initial_flag == 0U) {
            amba_ik_system_print_uint32_5("[IK][ERROR] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
            rval = IK_ERR_0002;
        } else {
            amba_ik_system_print_str_5("[IK][ERROR] can't get context\n", NULL, NULL, NULL, NULL, NULL);
        }
    }

    return rval;
}

uint32 img_ctx_ivd_get_cc_reg(uint32 context_id, ik_color_correction_reg_t *p_color_correction_reg)
{
    amba_ik_system_print_str_5("[IK] get_color_correction_reg is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //TBD
    }
    if (p_color_correction_reg == NULL) {
        //TBD
    } else {
        (void)amba_ik_system_memcpy(p_color_correction_reg, p_color_correction_reg, sizeof(uint8));
    }
    return IK_ERR_0004;
}

uint32 img_ctx_get_color_correction_reg(uint32 context_id, ik_color_correction_reg_t *p_color_correction_reg)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval |= img_ctx_get_context(context_id, &p_ctx);
    if((p_color_correction_reg != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        (void)amba_ik_system_lock_mutex(context_id, IK_WAIT_FOREVER);
        (void)amba_ik_system_memcpy(p_color_correction_reg, &p_ctx->filters.input_param.color_correction_reg, sizeof(ik_color_correction_reg_t));
        (void)amba_ik_system_unlock_mutex(context_id);
    } else {
        if(p_color_correction_reg == NULL) {
            amba_ik_system_print_str_5("[IK][ERROR] NULL pointer in %s\n", __func__, NULL, NULL, NULL, NULL);
            rval = IK_ERR_0005;
        } else if(p_ctx->organization.initial_flag == 0U) {
            amba_ik_system_print_uint32_5("[IK][ERROR] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
            rval = IK_ERR_0002;
        } else {
            amba_ik_system_print_str_5("[IK][ERROR] can't get context\n", NULL, NULL, NULL, NULL, NULL);
        }
    }

    return rval;
}

uint32 img_ctx_ivd_get_color_correction(uint32 context_id, ik_color_correction_t *p_color_correction)
{
    amba_ik_system_print_str_5("[IK] get_color_correction is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //TBD
    }
    if (p_color_correction == NULL) {
        //TBD
    } else {
        (void)amba_ik_system_memcpy(p_color_correction, p_color_correction, sizeof(uint8));
    }
    return IK_ERR_0004;
}

uint32 img_ctx_get_color_correction(uint32 context_id, ik_color_correction_t *p_color_correction)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval |= img_ctx_get_context(context_id, &p_ctx);
    if((p_color_correction != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        (void)amba_ik_system_lock_mutex(context_id, IK_WAIT_FOREVER);
        (void)amba_ik_system_memcpy(p_color_correction, &p_ctx->filters.input_param.color_correction, sizeof(ik_color_correction_t));
        (void)amba_ik_system_unlock_mutex(context_id);
    } else {
        if(p_color_correction == NULL) {
            amba_ik_system_print_str_5("[IK][ERROR] NULL pointer in %s\n", __func__, NULL, NULL, NULL, NULL);
            rval = IK_ERR_0005;
        } else if(p_ctx->organization.initial_flag == 0U) {
            amba_ik_system_print_uint32_5("[IK][ERROR] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
            rval = IK_ERR_0002;
        } else {
            amba_ik_system_print_str_5("[IK][ERROR] can't get context\n", NULL, NULL, NULL, NULL, NULL);
        }
    }

    return rval;
}

uint32 img_ctx_ivd_get_pre_cc_gain(uint32 context_id, ik_pre_cc_gain_t *p_pre_cc_gain)
{
    amba_ik_system_print_str_5("[IK] get_pre_cc_gain is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //TBD
    }
    if (p_pre_cc_gain == NULL) {
        //TBD
    } else {
        (void)amba_ik_system_memcpy(p_pre_cc_gain, p_pre_cc_gain, sizeof(uint8));
    }
    return IK_ERR_0004;
}

uint32 img_ctx_get_pre_cc_gain(uint32 context_id, ik_pre_cc_gain_t *p_pre_cc_gain)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval |= img_ctx_get_context(context_id, &p_ctx);
    if((p_pre_cc_gain != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        (void)amba_ik_system_lock_mutex(context_id, IK_WAIT_FOREVER);
        (void)amba_ik_system_memcpy(p_pre_cc_gain, &p_ctx->filters.input_param.pre_cc_gain, sizeof(ik_pre_cc_gain_t));
        (void)amba_ik_system_unlock_mutex(context_id);
    } else {
        if(p_pre_cc_gain == NULL) {
            amba_ik_system_print_str_5("[IK][ERROR] NULL pointer in %s\n", __func__, NULL, NULL, NULL, NULL);
            rval = IK_ERR_0005;
        } else if(p_ctx->organization.initial_flag == 0U) {
            amba_ik_system_print_uint32_5("[IK][ERROR] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
            rval = IK_ERR_0002;
        } else {
            amba_ik_system_print_str_5("[IK][ERROR] can't get context\n", NULL, NULL, NULL, NULL, NULL);
        }
    }

    return rval;
}

uint32 img_ctx_ivd_get_tone_curve(uint32 context_id, ik_tone_curve_t *p_tone_curve)
{
    amba_ik_system_print_str_5("[IK] get_tone_curve is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //TBD
    }
    if (p_tone_curve == NULL) {
        //TBD
    } else {
        (void)amba_ik_system_memcpy(p_tone_curve, p_tone_curve, sizeof(uint8));
    }
    return IK_ERR_0004;
}

uint32 img_ctx_get_tone_curve(uint32 context_id, ik_tone_curve_t *p_tone_curve)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval |= img_ctx_get_context(context_id, &p_ctx);
    if((p_tone_curve != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        (void)amba_ik_system_lock_mutex(context_id, IK_WAIT_FOREVER);
        (void)amba_ik_system_memcpy(p_tone_curve, &p_ctx->filters.input_param.tone_curve, sizeof(ik_tone_curve_t));
        (void)amba_ik_system_unlock_mutex(context_id);
    } else {
        if(p_tone_curve == NULL) {
            amba_ik_system_print_str_5("[IK][ERROR] NULL pointer in %s\n", __func__, NULL, NULL, NULL, NULL);
            rval = IK_ERR_0005;
        } else if(p_ctx->organization.initial_flag == 0U) {
            amba_ik_system_print_uint32_5("[IK][ERROR] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
            rval = IK_ERR_0002;
        } else {
            amba_ik_system_print_str_5("[IK][ERROR] can't get context\n", NULL, NULL, NULL, NULL, NULL);
        }
    }

    return rval;
}

uint32 img_ctx_ivd_get_rgb_to_yuv_matrix(uint32 context_id, ik_rgb_to_yuv_matrix_t *p_rgb_to_yuv_matrix)
{
    amba_ik_system_print_str_5("[IK] get_rgb_to_yuv_matrix is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //TBD
    }
    if (p_rgb_to_yuv_matrix == NULL) {
        //TBD
    } else {
        (void)amba_ik_system_memcpy(p_rgb_to_yuv_matrix, p_rgb_to_yuv_matrix, sizeof(uint8));
    }
    return IK_ERR_0004;
}

uint32 img_ctx_get_rgb_to_yuv_matrix(uint32 context_id, ik_rgb_to_yuv_matrix_t *p_rgb_to_yuv_matrix)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval |= img_ctx_get_context(context_id, &p_ctx);
    if((p_rgb_to_yuv_matrix != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        (void)amba_ik_system_lock_mutex(context_id, IK_WAIT_FOREVER);
        (void)amba_ik_system_memcpy(p_rgb_to_yuv_matrix, &p_ctx->filters.input_param.rgb_to_yuv_matrix, sizeof(ik_rgb_to_yuv_matrix_t));
        (void)amba_ik_system_unlock_mutex(context_id);
    } else {
        if(p_rgb_to_yuv_matrix == NULL) {
            amba_ik_system_print_str_5("[IK][ERROR] NULL pointer in %s\n", __func__, NULL, NULL, NULL, NULL);
            rval = IK_ERR_0005;
        } else if(p_ctx->organization.initial_flag == 0U) {
            amba_ik_system_print_uint32_5("[IK][ERROR] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
            rval = IK_ERR_0002;
        } else {
            amba_ik_system_print_str_5("[IK][ERROR] can't get context\n", NULL, NULL, NULL, NULL, NULL);
        }
    }

    return rval;
}


uint32 img_ctx_ivd_get_rgb_ir(uint32 context_id, ik_rgb_ir_t *p_rgb_ir)
{
    amba_ik_system_print_str_5("[IK] get_rgb_ir is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //TBD
    }
    if (p_rgb_ir == NULL) {
        //TBD
    } else {
        (void)amba_ik_system_memcpy(p_rgb_ir, p_rgb_ir, sizeof(uint8));
    }
    return IK_ERR_0004;
}

uint32 img_ctx_get_rgb_ir(uint32 context_id, ik_rgb_ir_t *p_rgb_ir)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval |= img_ctx_get_context(context_id, &p_ctx);
    if((p_rgb_ir != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        (void)amba_ik_system_lock_mutex(context_id, IK_WAIT_FOREVER);
        (void)amba_ik_system_memcpy(p_rgb_ir, &p_ctx->filters.input_param.rgb_ir, sizeof(ik_rgb_ir_t));
        (void)amba_ik_system_unlock_mutex(context_id);
    } else {
        if(p_rgb_ir == NULL) {
            amba_ik_system_print_str_5("[IK][ERROR] NULL pointer in %s\n", __func__, NULL, NULL, NULL, NULL);
            rval = IK_ERR_0005;
        } else if(p_ctx->organization.initial_flag == 0U) {
            amba_ik_system_print_uint32_5("[IK][ERROR] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
            rval = IK_ERR_0002;
        } else {
            amba_ik_system_print_str_5("[IK][ERROR] can't get context\n", NULL, NULL, NULL, NULL, NULL);
        }
    }

    return rval;
}


uint32 img_ctx_ivd_get_chroma_scale(uint32 context_id, ik_chroma_scale_t *p_chroma_scale)
{
    amba_ik_system_print_str_5("[IK] get_chroma_scale is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //TBD
    }
    if (p_chroma_scale == NULL) {
        //TBD
    } else {
        (void)amba_ik_system_memcpy(p_chroma_scale, p_chroma_scale, sizeof(uint8));
    }
    return IK_ERR_0004;
}

uint32 img_ctx_get_chroma_scale(uint32 context_id, ik_chroma_scale_t *p_chroma_scale)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval |= img_ctx_get_context(context_id, &p_ctx);
    if((p_chroma_scale != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        (void)amba_ik_system_lock_mutex(context_id, IK_WAIT_FOREVER);
        (void)amba_ik_system_memcpy(p_chroma_scale, &p_ctx->filters.input_param.chroma_scale, sizeof(ik_chroma_scale_t));
        (void)amba_ik_system_unlock_mutex(context_id);
    } else {
        if(p_chroma_scale == NULL) {
            amba_ik_system_print_str_5("[IK][ERROR] NULL pointer in %s\n", __func__, NULL, NULL, NULL, NULL);
            rval = IK_ERR_0005;
        } else if(p_ctx->organization.initial_flag == 0U) {
            amba_ik_system_print_uint32_5("[IK][ERROR] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
            rval = IK_ERR_0002;
        } else {
            amba_ik_system_print_str_5("[IK][ERROR] can't get context\n", NULL, NULL, NULL, NULL, NULL);
        }
    }

    return rval;
}

uint32 img_ctx_ivd_get_chroma_median_filter(uint32 context_id, ik_chroma_median_filter_t *p_chroma_median_filter)
{
    amba_ik_system_print_str_5("[IK] get_chroma_median_filter is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //TBD
    }
    if (p_chroma_median_filter == NULL) {
        //TBD
    } else {
        (void)amba_ik_system_memcpy(p_chroma_median_filter, p_chroma_median_filter, sizeof(uint8));
    }
    return IK_ERR_0004;
}

uint32 img_ctx_get_chroma_median_filter(uint32 context_id, ik_chroma_median_filter_t *p_chroma_median_filter)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval |= img_ctx_get_context(context_id, &p_ctx);

    if((p_chroma_median_filter != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        (void)amba_ik_system_lock_mutex(context_id, IK_WAIT_FOREVER);
        (void)amba_ik_system_memcpy(p_chroma_median_filter, &p_ctx->filters.input_param.chroma_median_filter, sizeof(ik_chroma_median_filter_t));
        (void)amba_ik_system_unlock_mutex(context_id);
    } else {
        if(p_chroma_median_filter == NULL) {
            amba_ik_system_print_str_5("[IK][ERROR] NULL pointer in %s\n", __func__, NULL, NULL, NULL, NULL);
            rval = IK_ERR_0005;
        } else if(p_ctx->organization.initial_flag == 0U) {
            amba_ik_system_print_uint32_5("[IK][ERROR] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
            rval = IK_ERR_0002;
        } else {
            amba_ik_system_print_str_5("[IK][ERROR] can't get context\n", NULL, NULL, NULL, NULL, NULL);
        }
    }

    return rval;
}

uint32 img_ctx_ivd_get_first_luma_processing_mode(uint32 context_id, ik_first_luma_process_mode_t *p_first_luma_process_mode)
{
    amba_ik_system_print_str_5("[IK] get_first_luma_processing_mode is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //TBD
    }
    if (p_first_luma_process_mode == NULL) {
        //TBD
    } else {
        (void)amba_ik_system_memcpy(p_first_luma_process_mode, p_first_luma_process_mode, sizeof(uint8));
    }
    return IK_ERR_0004;
}

uint32 img_ctx_get_first_luma_processing_mode(uint32 context_id, ik_first_luma_process_mode_t *p_first_luma_process_mode)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval |= img_ctx_get_context(context_id, &p_ctx);
    if((p_first_luma_process_mode != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        (void)amba_ik_system_lock_mutex(context_id, IK_WAIT_FOREVER);
        (void)amba_ik_system_memcpy(p_first_luma_process_mode, &p_ctx->filters.input_param.first_luma_process_mode, sizeof(ik_first_luma_process_mode_t));
        (void)amba_ik_system_unlock_mutex(context_id);
    } else {
        if(p_first_luma_process_mode == NULL) {
            amba_ik_system_print_str_5("[IK][ERROR] NULL pointer in %s\n", __func__, NULL, NULL, NULL, NULL);
            rval = IK_ERR_0005;
        } else if(p_ctx->organization.initial_flag == 0U) {
            amba_ik_system_print_uint32_5("[IK][ERROR] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
            rval = IK_ERR_0002;
        } else {
            amba_ik_system_print_str_5("[IK][ERROR] can't get context\n", NULL, NULL, NULL, NULL, NULL);
        }
    }

    return rval;
}

uint32 img_ctx_ivd_get_advance_spatial_filter(uint32 context_id, ik_adv_spatial_filter_t *p_advance_spatial_filter)
{
    amba_ik_system_print_str_5("[IK] get_advance_spatial_filter is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //TBD
    }
    if (p_advance_spatial_filter == NULL) {
        //TBD
    } else {
        (void)amba_ik_system_memcpy(p_advance_spatial_filter, p_advance_spatial_filter, sizeof(uint8));
    }
    return IK_ERR_0004;
}

uint32 img_ctx_get_advance_spatial_filter(uint32 context_id, ik_adv_spatial_filter_t *p_advance_spatial_filter)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval |= img_ctx_get_context(context_id, &p_ctx);
    if((p_advance_spatial_filter != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        (void)amba_ik_system_lock_mutex(context_id, IK_WAIT_FOREVER);
        (void)amba_ik_system_memcpy(p_advance_spatial_filter, &p_ctx->filters.input_param.advance_spatial_filter, sizeof(ik_adv_spatial_filter_t));
        (void)amba_ik_system_unlock_mutex(context_id);
    } else {
        if(p_advance_spatial_filter == NULL) {
            amba_ik_system_print_str_5("[IK][ERROR] NULL pointer in %s\n", __func__, NULL, NULL, NULL, NULL);
            rval = IK_ERR_0005;
        } else if(p_ctx->organization.initial_flag == 0U) {
            amba_ik_system_print_uint32_5("[IK][ERROR] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
            rval = IK_ERR_0002;
        } else {
            amba_ik_system_print_str_5("[IK][ERROR] can't get context\n", NULL, NULL, NULL, NULL, NULL);
        }
    }

    return rval;
}

uint32 img_ctx_ivd_get_fstshpns_both(uint32 context_id, ik_first_sharpen_both_t *p_first_sharpen_both)
{
    amba_ik_system_print_str_5("[IK] get_fstshpns_both is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //TBD
    }
    if (p_first_sharpen_both == NULL) {
        //TBD
    } else {
        (void)amba_ik_system_memcpy(p_first_sharpen_both, p_first_sharpen_both, sizeof(uint8));
    }
    return IK_ERR_0004;
}

uint32 img_ctx_get_fstshpns_both(uint32 context_id, ik_first_sharpen_both_t *p_first_sharpen_both)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval |= img_ctx_get_context(context_id, &p_ctx);
    if((p_first_sharpen_both != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        (void)amba_ik_system_lock_mutex(context_id, IK_WAIT_FOREVER);
        (void)amba_ik_system_memcpy(p_first_sharpen_both, &p_ctx->filters.input_param.first_sharpen_both, sizeof(ik_first_sharpen_both_t));
        (void)amba_ik_system_unlock_mutex(context_id);
    } else {
        if(p_first_sharpen_both == NULL) {
            amba_ik_system_print_str_5("[IK][ERROR] NULL pointer in %s\n", __func__, NULL, NULL, NULL, NULL);
            rval = IK_ERR_0005;
        } else if(p_ctx->organization.initial_flag == 0U) {
            amba_ik_system_print_uint32_5("[IK][ERROR] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
            rval = IK_ERR_0002;
        } else {
            amba_ik_system_print_str_5("[IK][ERROR] can't get context\n", NULL, NULL, NULL, NULL, NULL);
        }
    }

    return rval;
}

uint32 img_ctx_ivd_get_fstshpns_noise(uint32 context_id, ik_first_sharpen_noise_t *p_first_sharpen_noise)
{
    amba_ik_system_print_str_5("[IK] get_fstshpns_noise is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //TBD
    }
    if (p_first_sharpen_noise == NULL) {
        //TBD
    } else {
        (void)amba_ik_system_memcpy(p_first_sharpen_noise, p_first_sharpen_noise, sizeof(uint8));
    }
    return IK_ERR_0004;
}

uint32 img_ctx_get_fstshpns_noise(uint32 context_id, ik_first_sharpen_noise_t *p_first_sharpen_noise)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval |= img_ctx_get_context(context_id, &p_ctx);
    if((p_first_sharpen_noise != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        (void)amba_ik_system_lock_mutex(context_id, IK_WAIT_FOREVER);
        (void)amba_ik_system_memcpy(p_first_sharpen_noise, &p_ctx->filters.input_param.first_sharpen_noise, sizeof(ik_first_sharpen_noise_t));
        (void)amba_ik_system_unlock_mutex(context_id);
    } else {
        if(p_first_sharpen_noise == NULL) {
            amba_ik_system_print_str_5("[IK][ERROR] NULL pointer in %s\n", __func__, NULL, NULL, NULL, NULL);
            rval = IK_ERR_0005;
        } else if(p_ctx->organization.initial_flag == 0U) {
            amba_ik_system_print_uint32_5("[IK][ERROR] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
            rval = IK_ERR_0002;
        } else {
            amba_ik_system_print_str_5("[IK][ERROR] can't get context\n", NULL, NULL, NULL, NULL, NULL);
        }
    }

    return rval;
}

uint32 img_ctx_ivd_get_fstshpns_fir(uint32 context_id, ik_first_sharpen_fir_t *p_first_sharpen_fir)
{
    amba_ik_system_print_str_5("[IK] get_fstshpns_fir is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //TBD
    }
    if (p_first_sharpen_fir == NULL) {
        //TBD
    } else {
        (void)amba_ik_system_memcpy(p_first_sharpen_fir, p_first_sharpen_fir, sizeof(uint8));
    }
    return IK_ERR_0004;
}

uint32 img_ctx_get_fstshpns_fir(uint32 context_id, ik_first_sharpen_fir_t *p_first_sharpen_fir)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval |= img_ctx_get_context(context_id, &p_ctx);
    if((p_first_sharpen_fir != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        (void)amba_ik_system_lock_mutex(context_id, IK_WAIT_FOREVER);
        (void)amba_ik_system_memcpy(p_first_sharpen_fir, &p_ctx->filters.input_param.first_sharpen_fir, sizeof(ik_first_sharpen_fir_t));
        (void)amba_ik_system_unlock_mutex(context_id);
    } else {
        if(p_first_sharpen_fir == NULL) {
            amba_ik_system_print_str_5("[IK][ERROR] NULL pointer in %s\n", __func__, NULL, NULL, NULL, NULL);
            rval = IK_ERR_0005;
        } else if(p_ctx->organization.initial_flag == 0U) {
            amba_ik_system_print_uint32_5("[IK][ERROR] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
            rval = IK_ERR_0002;
        } else {
            amba_ik_system_print_str_5("[IK][ERROR] can't get context\n", NULL, NULL, NULL, NULL, NULL);
        }
    }

    return rval;
}

uint32 img_ctx_ivd_get_fstshpns_coring(uint32 context_id, ik_first_sharpen_coring_t *p_first_sharpen_coring)
{
    amba_ik_system_print_str_5("[IK] get_fstshpns_coring is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //TBD
    }
    if (p_first_sharpen_coring == NULL) {
        //TBD
    } else {
        (void)amba_ik_system_memcpy(p_first_sharpen_coring, p_first_sharpen_coring, sizeof(uint8));
    }
    return IK_ERR_0004;
}

uint32 img_ctx_get_fstshpns_coring(uint32 context_id, ik_first_sharpen_coring_t *p_first_sharpen_coring)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval |= img_ctx_get_context(context_id, &p_ctx);
    if((p_first_sharpen_coring != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        (void)amba_ik_system_lock_mutex(context_id, IK_WAIT_FOREVER);
        (void)amba_ik_system_memcpy(p_first_sharpen_coring, &p_ctx->filters.input_param.first_sharpen_coring, sizeof(ik_first_sharpen_coring_t));
        (void)amba_ik_system_unlock_mutex(context_id);
    } else {
        if(p_first_sharpen_coring == NULL) {
            amba_ik_system_print_str_5("[IK][ERROR] NULL pointer in %s\n", __func__, NULL, NULL, NULL, NULL);
            rval = IK_ERR_0005;
        } else if(p_ctx->organization.initial_flag == 0U) {
            amba_ik_system_print_uint32_5("[IK][ERROR] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
            rval = IK_ERR_0002;
        } else {
            amba_ik_system_print_str_5("[IK][ERROR] can't get context\n", NULL, NULL, NULL, NULL, NULL);
        }
    }

    return rval;
}

uint32 img_ctx_ivd_get_fstshpns_cor_idx_scl(uint32 context_id, ik_first_sharpen_coring_idx_scale_t *p_fstshpns_coring_index_scale)
{
    amba_ik_system_print_str_5("[IK] get_fstshpns_coring_index_scale is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //TBD
    }
    if (p_fstshpns_coring_index_scale == NULL) {
        //TBD
    } else {
        (void)amba_ik_system_memcpy(p_fstshpns_coring_index_scale, p_fstshpns_coring_index_scale, sizeof(uint8));
    }
    return IK_ERR_0004;
}

uint32 img_ctx_get_fstshpns_cor_idx_scl(uint32 context_id, ik_first_sharpen_coring_idx_scale_t *p_fstshpns_coring_index_scale)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval |= img_ctx_get_context(context_id, &p_ctx);
    if((p_fstshpns_coring_index_scale != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        (void)amba_ik_system_lock_mutex(context_id, IK_WAIT_FOREVER);
        (void)amba_ik_system_memcpy(p_fstshpns_coring_index_scale, &p_ctx->filters.input_param.first_sharpen_coring_idx_scale, sizeof(ik_first_sharpen_coring_idx_scale_t));
        (void)amba_ik_system_unlock_mutex(context_id);
    } else {
        if(p_fstshpns_coring_index_scale == NULL) {
            amba_ik_system_print_str_5("[IK][ERROR] NULL pointer in %s\n", __func__, NULL, NULL, NULL, NULL);
            rval = IK_ERR_0005;
        } else if(p_ctx->organization.initial_flag == 0U) {
            amba_ik_system_print_uint32_5("[IK][ERROR] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
            rval = IK_ERR_0002;
        } else {
            amba_ik_system_print_str_5("[IK][ERROR] can't get context\n", NULL, NULL, NULL, NULL, NULL);
        }
    }

    return rval;
}

uint32 img_ctx_ivd_get_fstshpns_min_coring_result(uint32 context_id, ik_first_sharpen_min_coring_result_t *p_fstshpns_min_coring_result)
{
    amba_ik_system_print_str_5("[IK] get_fstshpns_min_coring_result is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //TBD
    }
    if (p_fstshpns_min_coring_result == NULL) {
        //TBD
    } else {
        (void)amba_ik_system_memcpy(p_fstshpns_min_coring_result, p_fstshpns_min_coring_result, sizeof(uint8));
    }
    return IK_ERR_0004;
}

uint32 img_ctx_get_fstshpns_min_coring_result(uint32 context_id, ik_first_sharpen_min_coring_result_t *p_fstshpns_min_coring_result)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval |= img_ctx_get_context(context_id, &p_ctx);
    if((p_fstshpns_min_coring_result != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        (void)amba_ik_system_lock_mutex(context_id, IK_WAIT_FOREVER);
        (void)amba_ik_system_memcpy(p_fstshpns_min_coring_result, &p_ctx->filters.input_param.first_sharpen_min_coring_result, sizeof(ik_first_sharpen_min_coring_result_t));
        (void)amba_ik_system_unlock_mutex(context_id);
    } else {
        if(p_fstshpns_min_coring_result == NULL) {
            amba_ik_system_print_str_5("[IK][ERROR] NULL pointer in %s\n", __func__, NULL, NULL, NULL, NULL);
            rval = IK_ERR_0005;
        } else if(p_ctx->organization.initial_flag == 0U) {
            amba_ik_system_print_uint32_5("[IK][ERROR] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
            rval = IK_ERR_0002;
        } else {
            amba_ik_system_print_str_5("[IK][ERROR] can't get context\n", NULL, NULL, NULL, NULL, NULL);
        }
    }

    return rval;
}

uint32 img_ctx_ivd_get_fstshpns_max_coring_result(uint32 context_id, ik_first_sharpen_max_coring_result_t *p_fstshpns_max_coring_result)
{
    amba_ik_system_print_str_5("[IK] get_fstshpns_max_coring_result is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //TBD
    }
    if (p_fstshpns_max_coring_result == NULL) {
        //TBD
    } else {
        (void)amba_ik_system_memcpy(p_fstshpns_max_coring_result, p_fstshpns_max_coring_result, sizeof(uint8));
    }
    return IK_ERR_0004;
}

uint32 img_ctx_get_fstshpns_max_coring_result(uint32 context_id, ik_first_sharpen_max_coring_result_t *p_fstshpns_max_coring_result)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval |= img_ctx_get_context(context_id, &p_ctx);
    if((p_fstshpns_max_coring_result != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        (void)amba_ik_system_lock_mutex(context_id, IK_WAIT_FOREVER);
        (void)amba_ik_system_memcpy(p_fstshpns_max_coring_result, &p_ctx->filters.input_param.first_sharpen_max_coring_result, sizeof(ik_first_sharpen_max_coring_result_t));
        (void)amba_ik_system_unlock_mutex(context_id);
    } else {
        if(p_fstshpns_max_coring_result == NULL) {
            amba_ik_system_print_str_5("[IK][ERROR] NULL pointer in %s\n", __func__, NULL, NULL, NULL, NULL);
            rval = IK_ERR_0005;
        } else if(p_ctx->organization.initial_flag == 0U) {
            amba_ik_system_print_uint32_5("[IK][ERROR] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
            rval = IK_ERR_0002;
        } else {
            amba_ik_system_print_str_5("[IK][ERROR] can't get context\n", NULL, NULL, NULL, NULL, NULL);
        }
    }

    return rval;
}

uint32 img_ctx_ivd_get_fstshpns_scale_coring(uint32 context_id, ik_first_sharpen_scale_coring_t *p_fstshpns_scale_coring)
{
    amba_ik_system_print_str_5("[IK] get_fstshpns_scale_coring is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //TBD
    }
    if (p_fstshpns_scale_coring == NULL) {
        //TBD
    } else {
        (void)amba_ik_system_memcpy(p_fstshpns_scale_coring, p_fstshpns_scale_coring, sizeof(uint8));
    }
    return IK_ERR_0004;
}

uint32 img_ctx_get_fstshpns_scale_coring(uint32 context_id, ik_first_sharpen_scale_coring_t *p_fstshpns_scale_coring)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval |= img_ctx_get_context(context_id, &p_ctx);
    if((p_fstshpns_scale_coring != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        (void)amba_ik_system_lock_mutex(context_id, IK_WAIT_FOREVER);
        (void)amba_ik_system_memcpy(p_fstshpns_scale_coring, &p_ctx->filters.input_param.first_sharpen_scale_coring, sizeof(ik_first_sharpen_scale_coring_t));
        (void)amba_ik_system_unlock_mutex(context_id);
    } else {
        if(p_fstshpns_scale_coring == NULL) {
            amba_ik_system_print_str_5("[IK][ERROR] NULL pointer in %s\n", __func__, NULL, NULL, NULL, NULL);
            rval = IK_ERR_0005;
        } else if(p_ctx->organization.initial_flag == 0U) {
            amba_ik_system_print_uint32_5("[IK][ERROR] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
            rval = IK_ERR_0002;
        } else {
            amba_ik_system_print_str_5("[IK][ERROR] can't get context\n", NULL, NULL, NULL, NULL, NULL);
        }
    }

    return rval;
}

uint32 img_ctx_ivd_get_fnlshpns_both(uint32 context_id, ik_final_sharpen_both_t *p_final_sharpen_both)
{
    amba_ik_system_print_str_5("[IK] get_fnlshpns_both is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //TBD
    }
    if (p_final_sharpen_both == NULL) {
        //TBD
    } else {
        (void)amba_ik_system_memcpy(p_final_sharpen_both, p_final_sharpen_both, sizeof(uint8));
    }
    return IK_ERR_0004;
}

uint32 img_ctx_get_fnlshpns_both(uint32 context_id, ik_final_sharpen_both_t *p_final_sharpen_both)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval |= img_ctx_get_context(context_id, &p_ctx);
    if((p_final_sharpen_both != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        (void)amba_ik_system_lock_mutex(context_id, IK_WAIT_FOREVER);
        (void)amba_ik_system_memcpy(p_final_sharpen_both, &p_ctx->filters.input_param.final_sharpen_both, sizeof(ik_final_sharpen_both_t));
        (void)amba_ik_system_unlock_mutex(context_id);
    } else {
        if(p_final_sharpen_both == NULL) {
            amba_ik_system_print_str_5("[IK][ERROR] NULL pointer in %s\n", __func__, NULL, NULL, NULL, NULL);
            rval = IK_ERR_0005;
        } else if(p_ctx->organization.initial_flag == 0U) {
            amba_ik_system_print_uint32_5("[IK][ERROR] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
            rval = IK_ERR_0002;
        } else {
            amba_ik_system_print_str_5("[IK][ERROR] can't get context\n", NULL, NULL, NULL, NULL, NULL);
        }
    }

    return rval;
}

uint32 img_ctx_ivd_get_fnlshpns_noise(uint32 context_id, ik_final_sharpen_noise_t *p_final_sharpen_noise)
{
    amba_ik_system_print_str_5("[IK] get_fnlshpns_noise is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //TBD
    }
    if (p_final_sharpen_noise == NULL) {
        //TBD
    } else {
        (void)amba_ik_system_memcpy(p_final_sharpen_noise, p_final_sharpen_noise, sizeof(uint8));
    }
    return IK_ERR_0004;
}

uint32 img_ctx_get_fnlshpns_noise(uint32 context_id, ik_final_sharpen_noise_t *p_final_sharpen_noise)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval |= img_ctx_get_context(context_id, &p_ctx);
    if((p_final_sharpen_noise != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        (void)amba_ik_system_lock_mutex(context_id, IK_WAIT_FOREVER);
        (void)amba_ik_system_memcpy(p_final_sharpen_noise, &p_ctx->filters.input_param.final_sharpen_noise, sizeof(ik_final_sharpen_noise_t));
        (void)amba_ik_system_unlock_mutex(context_id);
    } else {
        if(p_final_sharpen_noise == NULL) {
            amba_ik_system_print_str_5("[IK][ERROR] NULL pointer in %s\n", __func__, NULL, NULL, NULL, NULL);
            rval = IK_ERR_0005;
        } else if(p_ctx->organization.initial_flag == 0U) {
            amba_ik_system_print_uint32_5("[IK][ERROR] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
            rval = IK_ERR_0002;
        } else {
            amba_ik_system_print_str_5("[IK][ERROR] can't get context\n", NULL, NULL, NULL, NULL, NULL);
        }
    }

    return rval;
}

uint32 img_ctx_ivd_get_fnlshpns_fir(uint32 context_id, ik_final_sharpen_fir_t *p_final_sharpen_fir)
{
    amba_ik_system_print_str_5("[IK] get_fnlshpns_fir is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //TBD
    }
    if (p_final_sharpen_fir == NULL) {
        //TBD
    } else {
        (void)amba_ik_system_memcpy(p_final_sharpen_fir, p_final_sharpen_fir, sizeof(uint8));
    }
    return IK_ERR_0004;
}

uint32 img_ctx_get_fnlshpns_fir(uint32 context_id, ik_final_sharpen_fir_t *p_final_sharpen_fir)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval |= img_ctx_get_context(context_id, &p_ctx);
    if((p_final_sharpen_fir != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        (void)amba_ik_system_lock_mutex(context_id, IK_WAIT_FOREVER);
        (void)amba_ik_system_memcpy(p_final_sharpen_fir, &p_ctx->filters.input_param.final_sharpen_fir, sizeof(ik_final_sharpen_fir_t));
        (void)amba_ik_system_unlock_mutex(context_id);
    } else {
        if(p_final_sharpen_fir == NULL) {
            amba_ik_system_print_str_5("[IK][ERROR] NULL pointer in %s\n", __func__, NULL, NULL, NULL, NULL);
            rval = IK_ERR_0005;
        } else if(p_ctx->organization.initial_flag == 0U) {
            amba_ik_system_print_uint32_5("[IK][ERROR] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
            rval = IK_ERR_0002;
        } else {
            amba_ik_system_print_str_5("[IK][ERROR] can't get context\n", NULL, NULL, NULL, NULL, NULL);
        }
    }

    return rval;
}

uint32 img_ctx_ivd_get_fnlshpns_coring(uint32 context_id, ik_final_sharpen_coring_t *p_final_sharpen_coring)
{
    amba_ik_system_print_str_5("[IK] get_fnlshpns_coring is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //TBD
    }
    if (p_final_sharpen_coring == NULL) {
        //TBD
    } else {
        (void)amba_ik_system_memcpy(p_final_sharpen_coring, p_final_sharpen_coring, sizeof(uint8));
    }
    return IK_ERR_0004;
}

uint32 img_ctx_get_fnlshpns_coring(uint32 context_id, ik_final_sharpen_coring_t *p_final_sharpen_coring)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval |= img_ctx_get_context(context_id, &p_ctx);
    if((p_final_sharpen_coring != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        (void)amba_ik_system_lock_mutex(context_id, IK_WAIT_FOREVER);
        (void)amba_ik_system_memcpy(p_final_sharpen_coring, &p_ctx->filters.input_param.final_sharpen_coring, sizeof(ik_final_sharpen_coring_t));
        (void)amba_ik_system_unlock_mutex(context_id);
    } else {
        if(p_final_sharpen_coring == NULL) {
            amba_ik_system_print_str_5("[IK][ERROR] NULL pointer in %s\n", __func__, NULL, NULL, NULL, NULL);
            rval = IK_ERR_0005;
        } else if(p_ctx->organization.initial_flag == 0U) {
            amba_ik_system_print_uint32_5("[IK][ERROR] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
            rval = IK_ERR_0002;
        } else {
            amba_ik_system_print_str_5("[IK][ERROR] can't get context\n", NULL, NULL, NULL, NULL, NULL);
        }
    }

    return rval;
}

uint32 img_ctx_ivd_get_fnlshpns_cor_idx_scl(uint32 context_id, ik_final_sharpen_coring_idx_scale_t *p_fnlshpns_coring_index_scale)
{
    amba_ik_system_print_str_5("[IK] get_fnlshpns_coring_index_scale is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //TBD
    }
    if (p_fnlshpns_coring_index_scale == NULL) {
        //TBD
    } else {
        (void)amba_ik_system_memcpy(p_fnlshpns_coring_index_scale, p_fnlshpns_coring_index_scale, sizeof(uint8));
    }
    return IK_ERR_0004;
}

uint32 img_ctx_get_fnlshpns_cor_idx_scl(uint32 context_id, ik_final_sharpen_coring_idx_scale_t *p_fnlshpns_coring_index_scale)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval |= img_ctx_get_context(context_id, &p_ctx);
    if((p_fnlshpns_coring_index_scale != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        (void)amba_ik_system_lock_mutex(context_id, IK_WAIT_FOREVER);
        (void)amba_ik_system_memcpy(p_fnlshpns_coring_index_scale, &p_ctx->filters.input_param.final_sharpen_coring_idx_scale, sizeof(ik_final_sharpen_coring_idx_scale_t));
        (void)amba_ik_system_unlock_mutex(context_id);
    } else {
        if(p_fnlshpns_coring_index_scale == NULL) {
            amba_ik_system_print_str_5("[IK][ERROR] NULL pointer in %s\n", __func__, NULL, NULL, NULL, NULL);
            rval = IK_ERR_0005;
        } else if(p_ctx->organization.initial_flag == 0U) {
            amba_ik_system_print_uint32_5("[IK][ERROR] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
            rval = IK_ERR_0002;
        } else {
            amba_ik_system_print_str_5("[IK][ERROR] can't get context\n", NULL, NULL, NULL, NULL, NULL);
        }
    }

    return rval;
}

uint32 img_ctx_ivd_get_fnlshpns_min_coring_result(uint32 context_id, ik_final_sharpen_min_coring_result_t *p_fnlshpns_min_coring_result)
{
    amba_ik_system_print_str_5("[IK] get_fnlshpns_min_coring_result is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //TBD
    }
    if (p_fnlshpns_min_coring_result == NULL) {
        //TBD
    } else {
        (void)amba_ik_system_memcpy(p_fnlshpns_min_coring_result, p_fnlshpns_min_coring_result, sizeof(uint8));
    }
    return IK_ERR_0004;
}

uint32 img_ctx_get_fnlshpns_min_coring_result(uint32 context_id, ik_final_sharpen_min_coring_result_t *p_fnlshpns_min_coring_result)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval |= img_ctx_get_context(context_id, &p_ctx);
    if((p_fnlshpns_min_coring_result != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        (void)amba_ik_system_lock_mutex(context_id, IK_WAIT_FOREVER);
        (void)amba_ik_system_memcpy(p_fnlshpns_min_coring_result, &p_ctx->filters.input_param.final_sharpen_min_coring_result, sizeof(ik_final_sharpen_min_coring_result_t));
        (void)amba_ik_system_unlock_mutex(context_id);
    } else {
        if(p_fnlshpns_min_coring_result == NULL) {
            amba_ik_system_print_str_5("[IK][ERROR] NULL pointer in %s\n", __func__, NULL, NULL, NULL, NULL);
            rval = IK_ERR_0005;
        } else if(p_ctx->organization.initial_flag == 0U) {
            amba_ik_system_print_uint32_5("[IK][ERROR] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
            rval = IK_ERR_0002;
        } else {
            amba_ik_system_print_str_5("[IK][ERROR] can't get context\n", NULL, NULL, NULL, NULL, NULL);
        }
    }

    return rval;
}

uint32 img_ctx_ivd_get_fnlshpns_max_coring_result(uint32 context_id, ik_final_sharpen_max_coring_result_t *p_fnlshpns_max_coring_result)
{
    amba_ik_system_print_str_5("[IK] get_fnlshpns_max_coring_result is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //TBD
    }
    if (p_fnlshpns_max_coring_result == NULL) {
        //TBD
    } else {
        (void)amba_ik_system_memcpy(p_fnlshpns_max_coring_result, p_fnlshpns_max_coring_result, sizeof(uint8));
    }
    return IK_ERR_0004;
}

uint32 img_ctx_get_fnlshpns_max_coring_result(uint32 context_id, ik_final_sharpen_max_coring_result_t *p_fnlshpns_max_coring_result)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval |= img_ctx_get_context(context_id, &p_ctx);
    if((p_fnlshpns_max_coring_result != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        (void)amba_ik_system_lock_mutex(context_id, IK_WAIT_FOREVER);
        (void)amba_ik_system_memcpy(p_fnlshpns_max_coring_result, &p_ctx->filters.input_param.final_sharpen_max_coring_result, sizeof(ik_final_sharpen_max_coring_result_t));
        (void)amba_ik_system_unlock_mutex(context_id);
    } else {
        if(p_fnlshpns_max_coring_result == NULL) {
            amba_ik_system_print_str_5("[IK][ERROR] NULL pointer in %s\n", __func__, NULL, NULL, NULL, NULL);
            rval = IK_ERR_0005;
        } else if(p_ctx->organization.initial_flag == 0U) {
            amba_ik_system_print_uint32_5("[IK][ERROR] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
            rval = IK_ERR_0002;
        } else {
            amba_ik_system_print_str_5("[IK][ERROR] can't get context\n", NULL, NULL, NULL, NULL, NULL);
        }
    }

    return rval;
}

uint32 img_ctx_ivd_get_fnlshpns_scale_coring(uint32 context_id, ik_final_sharpen_scale_coring_t *p_fnlshpns_scale_coring)
{
    amba_ik_system_print_str_5("[IK] get_fnlshpns_scale_coring is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //TBD
    }
    if (p_fnlshpns_scale_coring == NULL) {
        //TBD
    } else {
        (void)amba_ik_system_memcpy(p_fnlshpns_scale_coring, p_fnlshpns_scale_coring, sizeof(uint8));
    }
    return IK_ERR_0004;
}

uint32 img_ctx_get_fnlshpns_scale_coring(uint32 context_id, ik_final_sharpen_scale_coring_t *p_fnlshpns_scale_coring)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval |= img_ctx_get_context(context_id, &p_ctx);
    if((p_fnlshpns_scale_coring != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        (void)amba_ik_system_lock_mutex(context_id, IK_WAIT_FOREVER);
        (void)amba_ik_system_memcpy(p_fnlshpns_scale_coring, &p_ctx->filters.input_param.final_sharpen_scale_coring, sizeof(ik_final_sharpen_scale_coring_t));
        (void)amba_ik_system_unlock_mutex(context_id);
    } else {
        if(p_fnlshpns_scale_coring == NULL) {
            amba_ik_system_print_str_5("[IK][ERROR] NULL pointer in %s\n", __func__, NULL, NULL, NULL, NULL);
            rval = IK_ERR_0005;
        } else if(p_ctx->organization.initial_flag == 0U) {
            amba_ik_system_print_uint32_5("[IK][ERROR] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
            rval = IK_ERR_0002;
        } else {
            amba_ik_system_print_str_5("[IK][ERROR] can't get context\n", NULL, NULL, NULL, NULL, NULL);
        }
    }

    return rval;
}

uint32 img_ctx_ivd_get_fnlshpns_both_tdt(uint32 context_id, ik_final_sharpen_both_three_d_table_t *p_final_sharpen_both_three_d_table)
{
    amba_ik_system_print_str_5("[IK] final_sharpen_both_three_d_table is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //TBD
    }
    if (p_final_sharpen_both_three_d_table == NULL) {
        //TBD
    } else {
        (void)amba_ik_system_memcpy(p_final_sharpen_both_three_d_table, p_final_sharpen_both_three_d_table, sizeof(uint8));
    }
    return IK_ERR_0004;
}

uint32 img_ctx_get_fnlshpns_both_tdt(uint32 context_id, ik_final_sharpen_both_three_d_table_t *p_final_sharpen_both_three_d_table)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval |= img_ctx_get_context(context_id, &p_ctx);
    if((p_final_sharpen_both_three_d_table != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        (void)amba_ik_system_lock_mutex(context_id, IK_WAIT_FOREVER);
        (void)amba_ik_system_memcpy(p_final_sharpen_both_three_d_table, &p_ctx->filters.input_param.final_sharpen_both_three_d_table, sizeof(ik_final_sharpen_both_three_d_table_t));
        (void)amba_ik_system_unlock_mutex(context_id);
    } else {
        if(p_final_sharpen_both_three_d_table == NULL) {
            amba_ik_system_print_str_5("[IK][ERROR] NULL pointer in %s\n", __func__, NULL, NULL, NULL, NULL);
            rval = IK_ERR_0005;
        } else if(p_ctx->organization.initial_flag == 0U) {
            amba_ik_system_print_uint32_5("[IK][ERROR] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
            rval = IK_ERR_0002;
        } else {
            amba_ik_system_print_str_5("[IK][ERROR] can't get context\n", NULL, NULL, NULL, NULL, NULL);
        }
    }

    return rval;
}

uint32 img_ctx_ivd_get_chroma_filter(uint32 context_id, ik_chroma_filter_t *p_chroma_filter)
{
    amba_ik_system_print_str_5("[IK] get_chroma_filter is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //TBD
    }
    if (p_chroma_filter == NULL) {
        //TBD
    } else {
        (void)amba_ik_system_memcpy(p_chroma_filter, p_chroma_filter, sizeof(uint8));
    }
    return IK_ERR_0004;
}

uint32 img_ctx_get_chroma_filter(uint32 context_id, ik_chroma_filter_t *p_chroma_filter)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval |= img_ctx_get_context(context_id, &p_ctx);
    if((p_chroma_filter != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        (void)amba_ik_system_lock_mutex(context_id, IK_WAIT_FOREVER);
        (void)amba_ik_system_memcpy(p_chroma_filter, &p_ctx->filters.input_param.chroma_filter, sizeof(ik_chroma_filter_t));
        (void)amba_ik_system_unlock_mutex(context_id);
    } else {
        if(p_chroma_filter == NULL) {
            amba_ik_system_print_str_5("[IK][ERROR] NULL pointer in %s\n", __func__, NULL, NULL, NULL, NULL);
            rval = IK_ERR_0005;
        } else if(p_ctx->organization.initial_flag == 0U) {
            amba_ik_system_print_uint32_5("[IK][ERROR] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
            rval = IK_ERR_0002;
        } else {
            amba_ik_system_print_str_5("[IK][ERROR] can't get context\n", NULL, NULL, NULL, NULL, NULL);
        }
    }

    return rval;
}

uint32 img_ctx_ivd_get_grgb_mismatch(uint32 context_id, ik_grgb_mismatch_t *p_grgb_mismatch)
{
    amba_ik_system_print_str_5("[IK] grgb_mismatch is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //TBD
    }
    if (p_grgb_mismatch == NULL) {
        //TBD
    } else {
        (void)amba_ik_system_memcpy(p_grgb_mismatch, p_grgb_mismatch, sizeof(uint8));
    }
    return IK_ERR_0004;
}

uint32 img_ctx_get_grgb_mismatch(uint32 context_id, ik_grgb_mismatch_t *p_grgb_mismatch)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval |= img_ctx_get_context(context_id, &p_ctx);
    if((p_grgb_mismatch != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        (void)amba_ik_system_lock_mutex(context_id, IK_WAIT_FOREVER);
        (void)amba_ik_system_memcpy(p_grgb_mismatch, &p_ctx->filters.input_param.grgb_mismatch, sizeof(ik_grgb_mismatch_t));
        (void)amba_ik_system_unlock_mutex(context_id);
    } else {
        if(p_grgb_mismatch == NULL) {
            amba_ik_system_print_str_5("[IK][ERROR] NULL pointer in %s\n", __func__, NULL, NULL, NULL, NULL);
            rval = IK_ERR_0005;
        } else if(p_ctx->organization.initial_flag == 0U) {
            amba_ik_system_print_uint32_5("[IK][ERROR] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
            rval = IK_ERR_0002;
        } else {
            amba_ik_system_print_str_5("[IK][ERROR] can't get context\n", NULL, NULL, NULL, NULL, NULL);
        }
    }

    return rval;
}

uint32 img_ctx_ivd_get_video_mctf(uint32 context_id, ik_video_mctf_t *p_video_mctf)
{
    amba_ik_system_print_str_5("[IK] get_video_mctf is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //TBD
    }
    if (p_video_mctf == NULL) {
        //TBD
    } else {
        (void)amba_ik_system_memcpy(p_video_mctf, p_video_mctf, sizeof(uint8));
    }
    return IK_ERR_0004;
}

uint32 img_ctx_get_video_mctf(uint32 context_id, ik_video_mctf_t *p_video_mctf)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval |= img_ctx_get_context(context_id, &p_ctx);
    if((p_video_mctf != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        (void)amba_ik_system_lock_mutex(context_id, IK_WAIT_FOREVER);
        (void)amba_ik_system_memcpy(p_video_mctf, &p_ctx->filters.input_param.video_mctf, sizeof(ik_video_mctf_t));
        (void)amba_ik_system_unlock_mutex(context_id);
    } else {
        if(p_video_mctf == NULL) {
            amba_ik_system_print_str_5("[IK][ERROR] NULL pointer in %s\n", __func__, NULL, NULL, NULL, NULL);
            rval = IK_ERR_0005;
        } else if(p_ctx->organization.initial_flag == 0U) {
            amba_ik_system_print_uint32_5("[IK][ERROR] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
            rval = IK_ERR_0002;
        } else {
            amba_ik_system_print_str_5("[IK][ERROR] can't get context\n", NULL, NULL, NULL, NULL, NULL);
        }
    }

    return rval;
}
uint32 img_ctx_ivd_get_mctf_ta(uint32 context_id, ik_video_mctf_ta_t *p_video_mctf_ta)
{
    amba_ik_system_print_str_5("[IK] get_video_mctf_ta is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //TBD
    }
    if (p_video_mctf_ta == NULL) {
        //TBD
    } else {
        (void)amba_ik_system_memcpy(p_video_mctf_ta, p_video_mctf_ta, sizeof(uint8));
    }
    return IK_ERR_0004;
}

uint32 img_ctx_get_video_mctf_ta(uint32 context_id, ik_video_mctf_ta_t *p_video_mctf_ta)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval |= img_ctx_get_context(context_id, &p_ctx);
    if((p_video_mctf_ta != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        (void)amba_ik_system_lock_mutex(context_id, IK_WAIT_FOREVER);
        (void)amba_ik_system_memcpy(p_video_mctf_ta, &p_ctx->filters.input_param.video_mctf_ta, sizeof(ik_video_mctf_ta_t));
        (void)amba_ik_system_unlock_mutex(context_id);
    } else {
        if(p_video_mctf_ta == NULL) {
            amba_ik_system_print_str_5("[IK][ERROR] NULL pointer in %s\n", __func__, NULL, NULL, NULL, NULL);
            rval = IK_ERR_0005;
        } else if(p_ctx->organization.initial_flag == 0U) {
            amba_ik_system_print_uint32_5("[IK][ERROR] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
            rval = IK_ERR_0002;
        } else {
            amba_ik_system_print_str_5("[IK][ERROR] can't get context\n", NULL, NULL, NULL, NULL, NULL);
        }
    }

    return rval;
}

uint32 img_ctx_ivd_get_mctf_and_final_sharpen(uint32 context_id, ik_pos_dep33_t *p_video_mctf_and_final_sharpen)
{
    amba_ik_system_print_str_5("[IK] img_ctx_ivd_get_mctf_and_final_sharpen is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //TBD
    }
    if (p_video_mctf_and_final_sharpen == NULL) {
        //TBD
    } else {
        (void)amba_ik_system_memcpy(p_video_mctf_and_final_sharpen, p_video_mctf_and_final_sharpen, sizeof(uint8));
    }
    return IK_ERR_0004;
}

uint32 img_ctx_get_video_mctf_and_final_sharpen(uint32 context_id, ik_pos_dep33_t *p_video_mctf_and_final_sharpen)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval |= img_ctx_get_context(context_id, &p_ctx);
    if((p_video_mctf_and_final_sharpen != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        (void)amba_ik_system_lock_mutex(context_id, IK_WAIT_FOREVER);
        (void)amba_ik_system_memcpy(p_video_mctf_and_final_sharpen, &p_ctx->filters.input_param.video_mctf_and_final_sharpen, sizeof(ik_pos_dep33_t));
        (void)amba_ik_system_unlock_mutex(context_id);
    } else {
        if(p_video_mctf_and_final_sharpen == NULL) {
            amba_ik_system_print_str_5("[IK][ERROR] NULL pointer in %s\n", __func__, NULL, NULL, NULL, NULL);
            rval = IK_ERR_0005;
        } else if(p_ctx->organization.initial_flag == 0U) {
            amba_ik_system_print_uint32_5("[IK][ERROR] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
            rval = IK_ERR_0002;
        } else {
            amba_ik_system_print_str_5("[IK][ERROR] can't get context\n", NULL, NULL, NULL, NULL, NULL);
        }
    }

    return rval;
}
#if 0
uint32 img_ctx_ivd_get_deferred_blc(uint32 context_id, ik_deferred_blc_level_t *p_deferred_blc)
{
    amba_ik_system_print_str_5("[IK] get_deferred_blc is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //TBD
    }
    if (p_deferred_blc == NULL) {
        //TBD
    } else {
        (void)amba_ik_system_memcpy(p_deferred_blc, p_deferred_blc, sizeof(uint8));
    }
    return IK_ERR_0004;
}

uint32 img_ctx_get_deferred_blc(uint32 context_id, ik_deferred_blc_level_t *p_deferred_blc)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval |= img_ctx_get_context(context_id, &p_ctx);
    if((p_deferred_blc != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        (void)amba_ik_system_lock_mutex(context_id, IK_WAIT_FOREVER);
        (void)amba_ik_system_memcpy(p_deferred_blc, &p_ctx->filters.input_param.deferred_blc, sizeof(ik_deferred_blc_level_t));
        (void)amba_ik_system_unlock_mutex(context_id);
    } else {
        if(p_deferred_blc == NULL) {
            amba_ik_system_print_str_5("[IK][ERROR] NULL pointer in %s\n", __func__, NULL, NULL, NULL, NULL);
            rval = IK_ERR_0005;
        } else if(p_ctx->organization.initial_flag == 0U) {
            amba_ik_system_print_uint32_5("[IK][ERROR] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
            rval = IK_ERR_0002;
        } else {
            amba_ik_system_print_str_5("[IK][ERROR] can't get context\n", NULL, NULL, NULL, NULL, NULL);
        }
    }

    return rval;
}
#endif

uint32 img_ctx_ivd_get_aaa_stat_info(uint32 context_id, ik_aaa_stat_info_t *p_stat_info)
{
    amba_ik_system_print_str_5("[IK] get_aaa_stat_info is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //TBD
    }
    if (p_stat_info == NULL) {
        //TBD
    } else {
        (void)amba_ik_system_memcpy(p_stat_info, p_stat_info, sizeof(uint8));
    }
    return IK_ERR_0004;
}

uint32 img_ctx_get_aaa_stat_info(uint32 context_id, ik_aaa_stat_info_t *p_stat_info)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval |= img_ctx_get_context(context_id, &p_ctx);
    if((p_stat_info != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        (void)amba_ik_system_lock_mutex(context_id, IK_WAIT_FOREVER);
        (void)amba_ik_system_memcpy(p_stat_info, &p_ctx->filters.input_param.aaa_stat_info, sizeof(ik_aaa_stat_info_t));
        (void)amba_ik_system_unlock_mutex(context_id);
    } else {
        if(p_stat_info == NULL) {
            amba_ik_system_print_str_5("[IK][ERROR] NULL pointer in %s\n", __func__, NULL, NULL, NULL, NULL);
            rval = IK_ERR_0005;
        } else if(p_ctx->organization.initial_flag == 0U) {
            amba_ik_system_print_uint32_5("[IK][ERROR] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
            rval = IK_ERR_0002;
        } else {
            amba_ik_system_print_str_5("[IK][ERROR] can't get context\n", NULL, NULL, NULL, NULL, NULL);
        }
    }

    return rval;
}
uint32 img_ctx_ivd_get_af_stat_ex_info(uint32 context_id, ik_af_stat_ex_info_t *p_af_stat_ex_info)
{
    amba_ik_system_print_str_5("[IK] get_af_ex_info is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //TBD
    }
    if (p_af_stat_ex_info == NULL) {
        //TBD
    } else {
        (void)amba_ik_system_memcpy(p_af_stat_ex_info, p_af_stat_ex_info, sizeof(uint8));
    }
    return IK_ERR_0004;
}

uint32 img_ctx_get_af_stat_ex_info(uint32 context_id, ik_af_stat_ex_info_t *p_af_stat_ex_info)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval |= img_ctx_get_context(context_id, &p_ctx);
    if((p_af_stat_ex_info != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        (void)amba_ik_system_lock_mutex(context_id, IK_WAIT_FOREVER);
        (void)amba_ik_system_memcpy(p_af_stat_ex_info, &p_ctx->filters.input_param.af_stat_ex_info, sizeof(ik_af_stat_ex_info_t));
        (void)amba_ik_system_unlock_mutex(context_id);
    } else {
        if(p_af_stat_ex_info == NULL) {
            amba_ik_system_print_str_5("[IK][ERROR] NULL pointer in %s\n", __func__, NULL, NULL, NULL, NULL);
            rval = IK_ERR_0005;
        } else if(p_ctx->organization.initial_flag == 0U) {
            amba_ik_system_print_uint32_5("[IK][ERROR] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
            rval = IK_ERR_0002;
        } else {
            amba_ik_system_print_str_5("[IK][ERROR] can't get context\n", NULL, NULL, NULL, NULL, NULL);
        }
    }

    return rval;
}

uint32 img_ctx_ivd_get_pg_af_stat_ex_info(uint32 context_id, ik_pg_af_stat_ex_info_t *p_pg_af_stat_ex_info)
{
    amba_ik_system_print_str_5("[IK] get_af_ex_info is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //TBD
    }
    if (p_pg_af_stat_ex_info == NULL) {
        //TBD
    } else {
        (void)amba_ik_system_memcpy(p_pg_af_stat_ex_info, p_pg_af_stat_ex_info, sizeof(uint8));
    }
    return IK_ERR_0004;
}

uint32 img_ctx_get_pg_af_stat_ex_info(uint32 context_id, ik_pg_af_stat_ex_info_t *p_pg_af_stat_ex_info)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval |= img_ctx_get_context(context_id, &p_ctx);
    if((p_pg_af_stat_ex_info != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        (void)amba_ik_system_lock_mutex(context_id, IK_WAIT_FOREVER);
        (void)amba_ik_system_memcpy(p_pg_af_stat_ex_info, &p_ctx->filters.input_param.pg_af_stat_ex_info, sizeof(ik_pg_af_stat_ex_info_t));
        (void)amba_ik_system_unlock_mutex(context_id);
    } else {
        if(p_pg_af_stat_ex_info == NULL) {
            amba_ik_system_print_str_5("[IK][ERROR] NULL pointer in %s\n", __func__, NULL, NULL, NULL, NULL);
            rval = IK_ERR_0005;
        } else if(p_ctx->organization.initial_flag == 0U) {
            amba_ik_system_print_uint32_5("[IK][ERROR] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
            rval = IK_ERR_0002;
        } else {
            amba_ik_system_print_str_5("[IK][ERROR] can't get context\n", NULL, NULL, NULL, NULL, NULL);
        }
    }

    return rval;
}

uint32 img_ctx_ivd_get_aaa_stat_float_tile_info(uint32 context_id, ik_aaa_float_info_t *p_float_tile_info)
{
    amba_ik_system_print_str_5("[IK] get_aaa_stat_float_tile_info is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //TBD
    }
    if (p_float_tile_info == NULL) {
        //TBD
    } else {
        (void)amba_ik_system_memcpy(p_float_tile_info, p_float_tile_info, sizeof(uint8));
    }
    return IK_ERR_0004;
}

uint32 img_ctx_get_aaa_stat_float_tile_info(uint32 context_id, ik_aaa_float_info_t *p_float_tile_info)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval |= img_ctx_get_context(context_id, &p_ctx);
    if((p_float_tile_info != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        (void)amba_ik_system_lock_mutex(context_id, IK_WAIT_FOREVER);
        (void)amba_ik_system_memcpy(p_float_tile_info, &p_ctx->filters.input_param.aaa_float_tile_info, sizeof(ik_aaa_float_info_t));
        (void)amba_ik_system_unlock_mutex(context_id);
    } else {
        if(p_float_tile_info == NULL) {
            amba_ik_system_print_str_5("[IK][ERROR] NULL pointer in %s\n", __func__, NULL, NULL, NULL, NULL);
            rval = IK_ERR_0005;
        } else if(p_ctx->organization.initial_flag == 0U) {
            amba_ik_system_print_uint32_5("[IK][ERROR] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
            rval = IK_ERR_0002;
        } else {
            amba_ik_system_print_str_5("[IK][ERROR] can't get context\n", NULL, NULL, NULL, NULL, NULL);
        }
    }

    return rval;
}

uint32 img_ctx_ivd_get_window_size_info(uint32 context_id, ik_window_size_info_t *p_window_size_info)
{
    amba_ik_system_print_str_5("[IK] get_window_size_info is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //TBD
    }
    if (p_window_size_info == NULL) {
        //TBD
    } else {
        (void)amba_ik_system_memcpy(p_window_size_info, p_window_size_info, sizeof(uint8));
    }
    return IK_ERR_0004;
}

uint32 img_ctx_get_window_size_info(uint32 context_id, ik_window_size_info_t *p_window_size_info)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval |= img_ctx_get_context(context_id, &p_ctx);
    if((p_window_size_info != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        (void)amba_ik_system_lock_mutex(context_id, IK_WAIT_FOREVER);
        (void)amba_ik_system_memcpy(p_window_size_info, &p_ctx->filters.input_param.window_size_info, sizeof(ik_window_size_info_t));
        (void)amba_ik_system_unlock_mutex(context_id);
    } else {
        if(p_window_size_info == NULL) {
            amba_ik_system_print_str_5("[IK][ERROR] NULL pointer in %s\n", __func__, NULL, NULL, NULL, NULL);
            rval = IK_ERR_0005;
        } else if(p_ctx->organization.initial_flag == 0U) {
            amba_ik_system_print_uint32_5("[IK][ERROR] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
            rval = IK_ERR_0002;
        } else {
            amba_ik_system_print_str_5("[IK][ERROR] can't get context\n", NULL, NULL, NULL, NULL, NULL);
        }
    }

    return rval;
}

uint32 img_ctx_ivd_get_cfa_window_size_info(uint32 context_id, ik_cfa_window_size_info_t *p_cfa_window_size_info)
{
    amba_ik_system_print_str_5("[IK] get_cfa_window_size_info is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //TBD
    }
    if (p_cfa_window_size_info == NULL) {
        //TBD
    } else {
        (void)amba_ik_system_memcpy(p_cfa_window_size_info, p_cfa_window_size_info, sizeof(uint8));
    }
    return IK_ERR_0004;
}

uint32 img_ctx_get_cfa_window_size_info(uint32 context_id, ik_cfa_window_size_info_t *p_cfa_window_size_info)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval |= img_ctx_get_context(context_id, &p_ctx);
    if((p_cfa_window_size_info != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        (void)amba_ik_system_lock_mutex(context_id, IK_WAIT_FOREVER);
        (void)amba_ik_system_memcpy(p_cfa_window_size_info, &p_ctx->filters.input_param.cfa_window_size_info, sizeof(ik_cfa_window_size_info_t));
        (void)amba_ik_system_unlock_mutex(context_id);
    } else {
        if(p_cfa_window_size_info == NULL) {
            amba_ik_system_print_str_5("[IK][ERROR] NULL pointer in %s\n", __func__, NULL, NULL, NULL, NULL);
            rval = IK_ERR_0005;
        } else if(p_ctx->organization.initial_flag == 0U) {
            amba_ik_system_print_uint32_5("[IK][ERROR] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
            rval = IK_ERR_0002;
        } else {
            amba_ik_system_print_str_5("[IK][ERROR] can't get context\n", NULL, NULL, NULL, NULL, NULL);
        }
    }

    return rval;
}
#if 0
uint32 img_ctx_ivd_get_idsp_debug_internal(uint32 context_id, ik_idsp_debug_internal_t *p_idsp_debug_internal)
{
    amba_ik_system_print_str_5("[IK] get_idsp_debug_internal is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //TBD
    }
    if (p_idsp_debug_internal == NULL) {
        //TBD
    } else {
        (void)amba_ik_system_memcpy(p_idsp_debug_internal, p_idsp_debug_internal, sizeof(uint8));
    }
    return IK_ERR_0004;
}

uint32 img_ctx_get_idsp_debug_internal(uint32 context_id, ik_idsp_debug_internal_t *p_idsp_debug_internal)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval |= img_ctx_get_context(context_id, &p_ctx);
    if((p_idsp_debug_internal != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        (void)amba_ik_system_lock_mutex(context_id, IK_WAIT_FOREVER);
        (void)amba_ik_system_memcpy(p_idsp_debug_internal, &p_ctx->filters.input_param.internal_idsp_debug, sizeof(ik_idsp_debug_internal_t));
        (void)amba_ik_system_unlock_mutex(context_id);
    } else {
        if(p_idsp_debug_internal == NULL) {
            amba_ik_system_print_str_5("[IK][ERROR] NULL pointer in %s\n", __func__, NULL, NULL, NULL, NULL);
            rval = IK_ERR_0005;
        } else if(p_ctx->organization.initial_flag == 0U) {
            amba_ik_system_print_uint32_5("[IK][ERROR] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
            rval = IK_ERR_0002;
        } else {
            amba_ik_system_print_str_5("[IK][ERROR] can't get context\n", NULL, NULL, NULL, NULL, NULL);
        }
    }

    return rval;
}
#endif

uint32 img_ctx_ivd_get_mctf_internal(uint32 context_id, ik_mctf_internal_t *p_mctf_internal)
{
    amba_ik_system_print_str_5("[IK] get_mctf_internal is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //TBD
    }
    if (p_mctf_internal == NULL) {
        //TBD
    } else {
        (void)amba_ik_system_memcpy(p_mctf_internal, p_mctf_internal, sizeof(uint8));
    }
    return IK_ERR_0004;
}

uint32 img_ctx_get_video_mctf_internal(uint32 context_id, ik_mctf_internal_t *p_mctf_internal)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval |= img_ctx_get_context(context_id, &p_ctx);
    if((p_mctf_internal != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        (void)amba_ik_system_lock_mutex(context_id, IK_WAIT_FOREVER);
        (void)amba_ik_system_memcpy(p_mctf_internal, &p_ctx->filters.input_param.internal_video_mctf, sizeof(ik_mctf_internal_t));
        (void)amba_ik_system_unlock_mutex(context_id);
    } else {
        if(p_mctf_internal == NULL) {
            amba_ik_system_print_str_5("[IK][ERROR] NULL pointer in %s\n", __func__, NULL, NULL, NULL, NULL);
            rval = IK_ERR_0005;
        } else if(p_ctx->organization.initial_flag == 0U) {
            amba_ik_system_print_uint32_5("[IK][ERROR] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
            rval = IK_ERR_0002;
        } else {
            amba_ik_system_print_str_5("[IK][ERROR] can't get context\n", NULL, NULL, NULL, NULL, NULL);
        }
    }

    return rval;
}

uint32 img_ctx_ivd_get_motion_detect(uint32 context_id, ik_motion_detect_t *p_motion_detect)
{
    amba_ik_system_print_str_5("[IK] get_motion_detect is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //TBD
    }
    if (p_motion_detect == NULL) {
        //TBD
    } else {
        (void)amba_ik_system_memcpy(p_motion_detect, p_motion_detect, sizeof(uint8));
    }
    return IK_ERR_0004;
}

uint32 img_ctx_get_motion_detect(uint32 context_id, ik_motion_detect_t *p_motion_detect)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval |= img_ctx_get_context(context_id, &p_ctx);
    if((p_motion_detect != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        (void)amba_ik_system_lock_mutex(context_id, IK_WAIT_FOREVER);
        (void)amba_ik_system_memcpy(p_motion_detect, &p_ctx->filters.input_param.motion_detect, sizeof(ik_motion_detect_t));
        (void)amba_ik_system_unlock_mutex(context_id);
    } else {
        if(p_motion_detect == NULL) {
            amba_ik_system_print_str_5("[IK][ERROR] NULL pointer in %s\n", __func__, NULL, NULL, NULL, NULL);
            rval = IK_ERR_0005;
        } else if(p_ctx->organization.initial_flag == 0U) {
            amba_ik_system_print_uint32_5("[IK][ERROR] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
            rval = IK_ERR_0002;
        } else {
            amba_ik_system_print_str_5("[IK][ERROR] can't get context\n", NULL, NULL, NULL, NULL, NULL);
        }
    }

    return rval;
}

uint32 img_ctx_ivd_get_motion_detect_pos_dep(uint32 context_id, ik_pos_dep33_t *p_pos_dep)
{
    amba_ik_system_print_str_5("[IK] get_motion_detect is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //TBD
    }
    if (p_pos_dep == NULL) {
        //TBD
    } else {
        (void)amba_ik_system_memcpy(p_pos_dep, p_pos_dep, sizeof(uint8));
    }
    return IK_ERR_0004;
}

uint32 img_ctx_get_motion_detect_pos_dep(uint32 context_id, ik_pos_dep33_t *p_pos_dep)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval |= img_ctx_get_context(context_id, &p_ctx);
    if((p_pos_dep != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        (void)amba_ik_system_lock_mutex(context_id, IK_WAIT_FOREVER);
        (void)amba_ik_system_memcpy(p_pos_dep, &p_ctx->filters.input_param.motion_detect_pos_dep, sizeof(ik_pos_dep33_t));
        (void)amba_ik_system_unlock_mutex(context_id);
    } else {
        if(p_pos_dep == NULL) {
            amba_ik_system_print_str_5("[IK][ERROR] NULL pointer in %s\n", __func__, NULL, NULL, NULL, NULL);
            rval = IK_ERR_0005;
        } else if(p_ctx->organization.initial_flag == 0U) {
            amba_ik_system_print_uint32_5("[IK][ERROR] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
            rval = IK_ERR_0002;
        } else {
            amba_ik_system_print_str_5("[IK][ERROR] can't get context\n", NULL, NULL, NULL, NULL, NULL);
        }
    }

    return rval;
}

uint32 img_ctx_ivd_get_motion_detect_and_mctf(uint32 context_id, ik_motion_detect_and_mctf_t *p_motion_detect_and_mctf)
{
    amba_ik_system_print_str_5("[IK] get_motion_detect_and_mctf is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //TBD
    }
    if (p_motion_detect_and_mctf == NULL) {
        //TBD
    } else {
        (void)amba_ik_system_memcpy(p_motion_detect_and_mctf, p_motion_detect_and_mctf, sizeof(uint8));
    }
    return IK_ERR_0004;
}

uint32 img_ctx_get_motion_detect_and_mctf(uint32 context_id, ik_motion_detect_and_mctf_t *p_motion_detect_and_mctf)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval |= img_ctx_get_context(context_id, &p_ctx);
    if((p_motion_detect_and_mctf != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        (void)amba_ik_system_lock_mutex(context_id, IK_WAIT_FOREVER);
        (void)amba_ik_system_memcpy(p_motion_detect_and_mctf, &p_ctx->filters.input_param.motion_detect_and_mctf, sizeof(ik_motion_detect_and_mctf_t));
        (void)amba_ik_system_unlock_mutex(context_id);
    } else {
        if(p_motion_detect_and_mctf == NULL) {
            amba_ik_system_print_str_5("[IK][ERROR] NULL pointer in %s\n", __func__, NULL, NULL, NULL, NULL);
            rval = IK_ERR_0005;
        } else if(p_ctx->organization.initial_flag == 0U) {
            amba_ik_system_print_uint32_5("[IK][ERROR] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
            rval = IK_ERR_0002;
        } else {
            amba_ik_system_print_str_5("[IK][ERROR] can't get context\n", NULL, NULL, NULL, NULL, NULL);
        }
    }

    return rval;
}

uint32 img_ctx_ivd_get_fe_tone_curve(uint32 context_id, ik_frontend_tone_curve_t *p_fe_tc)
{
    amba_ik_system_print_str_5("[IK] get_motion_detect is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //TBD
    }
    if (p_fe_tc == NULL) {
        //TBD
    } else {
        (void)amba_ik_system_memcpy(p_fe_tc, p_fe_tc, sizeof(uint8));
    }
    return IK_ERR_0004;
}

uint32 img_ctx_get_fe_tone_curve(uint32 context_id, ik_frontend_tone_curve_t *p_fe_tc)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval |= img_ctx_get_context(context_id, &p_ctx);
    if((p_fe_tc != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        (void)amba_ik_system_lock_mutex(context_id, IK_WAIT_FOREVER);
        (void)amba_ik_system_memcpy(p_fe_tc, &p_ctx->filters.input_param.fe_tone_curve, sizeof(ik_frontend_tone_curve_t));
        (void)amba_ik_system_unlock_mutex(context_id);
    } else {
        if(p_fe_tc == NULL) {
            amba_ik_system_print_str_5("[IK][ERROR] NULL pointer in %s\n", __func__, NULL, NULL, NULL, NULL);
            rval = IK_ERR_0005;
        } else if(p_ctx->organization.initial_flag == 0U) {
            amba_ik_system_print_uint32_5("[IK][ERROR] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
            rval = IK_ERR_0002;
        } else {
            amba_ik_system_print_str_5("[IK][ERROR] can't get context\n", NULL, NULL, NULL, NULL, NULL);
        }
    }

    return rval;
}

uint32 img_ctx_ivd_get_resampler_str(uint32 context_id, ik_resampler_strength_t *p_resample_str)
{
    amba_ik_system_print_str_5("[IK] get_resampler_str is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //TBD
    }
    if (p_resample_str == NULL) {
        //TBD
    } else {
        (void)amba_ik_system_memcpy(p_resample_str, p_resample_str, sizeof(uint8));
    }
    return IK_ERR_0004;
}

uint32 img_ctx_get_resampler_str(uint32 context_id, ik_resampler_strength_t *p_resample_str)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval |= img_ctx_get_context(context_id, &p_ctx);
    if((p_resample_str != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        (void)amba_ik_system_lock_mutex(context_id, IK_WAIT_FOREVER);
        (void)amba_ik_system_memcpy(p_resample_str, &p_ctx->filters.input_param.resample_str, sizeof(ik_resampler_strength_t));
        (void)amba_ik_system_unlock_mutex(context_id);
    } else {
        if(p_resample_str == NULL) {
            amba_ik_system_print_str_5("[IK][ERROR] NULL pointer in %s\n", __func__, NULL, NULL, NULL, NULL);
            rval = IK_ERR_0005;
        } else if(p_ctx->organization.initial_flag == 0U) {
            amba_ik_system_print_uint32_5("[IK][ERROR] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
            rval = IK_ERR_0002;
        } else {
            amba_ik_system_print_str_5("[IK][ERROR] can't get context\n", NULL, NULL, NULL, NULL, NULL);
        }
    }

    return rval;
}

uint32 img_ctx_ivd_get_histogram_info(uint32 context_id, ik_histogram_info_t *p_hist_info)
{
    amba_ik_system_print_str_5("[IK] get_histogram_info is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //TBD
    }
    if (p_hist_info == NULL) {
        //TBD
    } else {
        (void)amba_ik_system_memcpy(p_hist_info, p_hist_info, sizeof(uint8));
    }
    return IK_ERR_0004;
}

uint32 img_ctx_get_histogram_info(uint32 context_id, ik_histogram_info_t *p_hist_info)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval |= img_ctx_get_context(context_id, &p_ctx);
    if((p_hist_info != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        (void)amba_ik_system_lock_mutex(context_id, IK_WAIT_FOREVER);
        (void)amba_ik_system_memcpy(p_hist_info, &p_ctx->filters.input_param.hist_info, sizeof(ik_histogram_info_t));
        (void)amba_ik_system_unlock_mutex(context_id);
    } else {
        if(p_hist_info == NULL) {
            amba_ik_system_print_str_5("[IK][ERROR] NULL pointer in %s\n", __func__, NULL, NULL, NULL, NULL);
            rval = IK_ERR_0005;
        } else if(p_ctx->organization.initial_flag == 0U) {
            amba_ik_system_print_uint32_5("[IK][ERROR] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
            rval = IK_ERR_0002;
        } else {
            amba_ik_system_print_str_5("[IK][ERROR] can't get context\n", NULL, NULL, NULL, NULL, NULL);
        }
    }

    return rval;
}

uint32 img_ctx_ivd_get_pseudo_y_info(uint32 context_id, ik_aaa_pseudo_y_info_t *p_pseudo_info)
{
    amba_ik_system_print_str_5("[IK] get_histogram_info is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //TBD
    }
    if (p_pseudo_info == NULL) {
        //TBD
    } else {
        (void)amba_ik_system_memcpy(p_pseudo_info, p_pseudo_info, sizeof(uint8));
    }
    return IK_ERR_0004;
}

uint32 img_ctx_get_pseudo_y_info(uint32 context_id, ik_aaa_pseudo_y_info_t *p_pseudo_info)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval |= img_ctx_get_context(context_id, &p_ctx);
    if((p_pseudo_info != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        (void)amba_ik_system_lock_mutex(context_id, IK_WAIT_FOREVER);
        (void)amba_ik_system_memcpy(p_pseudo_info, &p_ctx->filters.input_param.pseudo_y, sizeof(ik_aaa_pseudo_y_info_t));
        (void)amba_ik_system_unlock_mutex(context_id);
    } else {
        if(p_pseudo_info == NULL) {
            amba_ik_system_print_str_5("[IK][ERROR] NULL pointer in %s\n", __func__, NULL, NULL, NULL, NULL);
            rval = IK_ERR_0005;
        } else if(p_ctx->organization.initial_flag == 0U) {
            amba_ik_system_print_uint32_5("[IK][ERROR] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
            rval = IK_ERR_0002;
        } else {
            amba_ik_system_print_str_5("[IK][ERROR] can't get context\n", NULL, NULL, NULL, NULL, NULL);
        }
    }

    return rval;
}

uint32 img_ctx_ivd_get_pg_histogram_info(uint32 context_id, ik_histogram_info_t *p_hist_info)
{
    amba_ik_system_print_str_5("[IK] get_pg_histogram_info is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //TBD
    }
    if (p_hist_info == NULL) {
        //TBD
    } else {
        (void)amba_ik_system_memcpy(p_hist_info, p_hist_info, sizeof(uint8));
    }
    return IK_ERR_0004;
}

uint32 img_ctx_get_pg_histogram_info(uint32 context_id, ik_histogram_info_t *p_hist_info)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval |= img_ctx_get_context(context_id, &p_ctx);
    if((p_hist_info != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        (void)amba_ik_system_lock_mutex(context_id, IK_WAIT_FOREVER);
        (void)amba_ik_system_memcpy(p_hist_info, &p_ctx->filters.input_param.hist_info_pg, sizeof(ik_histogram_info_t));
        (void)amba_ik_system_unlock_mutex(context_id);
    } else {
        if(p_hist_info == NULL) {
            amba_ik_system_print_str_5("[IK][ERROR] NULL pointer in %s\n", __func__, NULL, NULL, NULL, NULL);
            rval = IK_ERR_0005;
        } else if(p_ctx->organization.initial_flag == 0U) {
            amba_ik_system_print_uint32_5("[IK][ERROR] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
            rval = IK_ERR_0002;
        } else {
            amba_ik_system_print_str_5("[IK][ERROR] can't get context\n", NULL, NULL, NULL, NULL, NULL);
        }
    }

    return rval;
}

uint32 img_ctx_ivd_set_y2y_use_cc_enable_info(uint32 context_id, const uint32 enable)
{
    uint32 rval = IK_ERR_0004;
    amba_ik_system_print_str_5("[IK] set_y2y_use_cc_enable_info is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //MISRA
    }
    if (enable == 0UL) {
        //MISRA
    }
    return rval;
}

uint32 img_ctx_set_y2y_use_cc_enable_info(uint32 context_id, const uint32 enable)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;
    if(enable > 1UL) {
        amba_ik_system_print_str_5("[IK][Error] Invalid data valid, set_pg_histogram_info() enable > 1 !", NULL, NULL, NULL, NULL, NULL);
    }
    rval |= img_ctx_get_context(context_id, &p_ctx);
    if((rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        (void)amba_ik_system_lock_mutex(context_id, IK_WAIT_FOREVER);
        p_ctx->filters.input_param.use_cc_for_yuv2yuv = enable;
        ctx_raise_update_flag(&p_ctx->filters.update_flags.liso.use_y2y_enable_info_updated);
        (void)amba_ik_system_unlock_mutex(context_id);
    } else {
        if(p_ctx->organization.initial_flag == 0U) {
            amba_ik_system_print_uint32_5("[IK][ERROR] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
        } else {
            amba_ik_system_print_str_5("[IK][ERROR] can't get context\n", NULL, NULL, NULL, NULL, NULL);
        }
    }
    return rval;
}

uint32 img_ctx_ivd_get_y2y_use_cc_enable_info(uint32 context_id, uint32 *p_enable)
{
    uint32 rval = IK_ERR_0004;
    amba_ik_system_print_str_5("[IK] get_y2y_use_cc_enable_info is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //TBD
    }
    if (p_enable==NULL) {
        //TBD
    } else {
        (void)amba_ik_system_memcpy(p_enable, p_enable, sizeof(uint8));
    }
    return rval;
}

uint32 img_ctx_get_y2y_use_cc_enable_info(uint32 context_id, uint32 *p_enable)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval |= img_ctx_get_context(context_id, &p_ctx);
    if((rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        (void)amba_ik_system_lock_mutex(context_id, IK_WAIT_FOREVER);
        (void)amba_ik_system_memcpy(p_enable, &p_ctx->filters.input_param.use_cc_for_yuv2yuv, sizeof(uint32));
        (void)amba_ik_system_unlock_mutex(context_id);
    } else {
        if(p_ctx->organization.initial_flag == 0U) {
            amba_ik_system_print_uint32_5("[IK][ERROR] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
        } else {
            amba_ik_system_print_str_5("[IK][ERROR] can't get context\n", NULL, NULL, NULL, NULL, NULL);
        }
    }
    return rval;
}

#if SUPPORT_FUSION
uint32 img_ctx_ivd_set_fusion(uint32 context_id, const ik_fusion_t *p_fusion)
{
    amba_ik_system_print_str_5("[IK] img_ctx_set_fusion() is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //MISRA
    }
    if (p_fusion == NULL) {
        //MISRA
    }
    return IK_ERR_0004;
}

uint32 img_ctx_set_fusion(uint32 context_id, const ik_fusion_t *p_fusion)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;
    const ik_debug_check_func_t* p_check_func = ik_ctx_get_debug_check_func();
    if(p_check_func != NULL) {
        rval = p_check_func->fusion(p_fusion, "fusion");
    }
    if(rval == IK_OK) {
        rval |= img_ctx_get_context(context_id, &p_ctx);
        if((p_fusion != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
            (void)amba_ik_system_lock_mutex(context_id, IK_WAIT_FOREVER);
            (void)amba_ik_system_memcpy(&p_ctx->filters.input_param.fusion, p_fusion, sizeof(ik_fusion_t));
            ctx_raise_update_flag(&p_ctx->filters.update_flags.liso.fusion);
            (void)amba_ik_system_unlock_mutex(context_id);
        } else {
            if(p_ctx->organization.initial_flag == 0U) {
                amba_ik_system_print_uint32_5("[IK][ERROR] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
                rval = IK_ERR_0002;
            } else {
                amba_ik_system_print_str_5("[IK][ERROR] can't get context\n", NULL, NULL, NULL, NULL, NULL);
            }
        }
    }
    return rval;
}

uint32 img_ctx_ivd_get_fusion(uint32 context_id, ik_fusion_t *p_fusion)
{
    amba_ik_system_print_str_5("[IK] get_motion_detect is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //TBD
    }
    if (p_fusion == NULL) {
        //TBD
    } else {
        (void)amba_ik_system_memcpy(p_fusion, p_fusion, sizeof(uint8));
    }
    return IK_ERR_0004;
}

uint32 img_ctx_get_fusion(uint32 context_id, ik_fusion_t *p_fusion)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval |= img_ctx_get_context(context_id, &p_ctx);
    if((p_fusion != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        (void)amba_ik_system_lock_mutex(context_id, IK_WAIT_FOREVER);
        (void)amba_ik_system_memcpy(p_fusion, &p_ctx->filters.input_param.fusion, sizeof(ik_fusion_t));
        (void)amba_ik_system_unlock_mutex(context_id);
    } else {
        if(p_fusion == NULL) {
            amba_ik_system_print_str_5("[IK][ERROR] NULL pointer in %s\n", __func__, NULL, NULL, NULL, NULL);
            rval = IK_ERR_0005;
        } else if(p_ctx->organization.initial_flag == 0U) {
            amba_ik_system_print_uint32_5("[IK][ERROR] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
            rval = IK_ERR_0002;
        } else {
            amba_ik_system_print_str_5("[IK][ERROR] can't get context\n", NULL, NULL, NULL, NULL, NULL);
        }
    }

    return rval;
}

uint32 img_ctx_ivd_set_mono_before_ce_wb_gain(uint32 context_id, const ik_wb_gain_t *p_before_ce_wb_gain)
{
    amba_ik_system_print_str_5("[IK] set_mono_before_ce_wb_gain is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //MISRA
    }
    if (p_before_ce_wb_gain == NULL) {
        //MISRA
    }
    return IK_ERR_0004;
}

uint32 img_ctx_set_mono_before_ce_wb_gain(uint32 context_id, const ik_wb_gain_t *p_before_ce_wb_gain)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;
    const ik_debug_check_func_t* p_check_func = ik_ctx_get_debug_check_func();
    if(p_check_func != NULL) {
        rval = p_check_func->before_ce_wb(p_before_ce_wb_gain, "mono_before_ce_wb_gain");
    }
    if(rval == IK_OK) {
        rval |= img_ctx_get_context(context_id, &p_ctx);
        if((p_before_ce_wb_gain != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
            (void)amba_ik_system_lock_mutex(context_id, IK_WAIT_FOREVER);
            (void)amba_ik_system_memcpy(&p_ctx->filters.input_param.mono_before_ce_wb_gain, p_before_ce_wb_gain, sizeof(ik_wb_gain_t));
            ctx_raise_update_flag(&p_ctx->filters.update_flags.liso.mono_before_ce_wb_gain_updated);
            (void)amba_ik_system_unlock_mutex(context_id);
        } else {
            if(p_ctx->organization.initial_flag == 0U) {
                amba_ik_system_print_uint32_5("[IK][ERROR] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
                rval |= IK_ERR_0002;
            } else {
                amba_ik_system_print_str_5("[IK][ERROR] can't get context\n", NULL, NULL, NULL, NULL, NULL);
            }
        }
    }
    return rval;
}

uint32 img_ctx_ivd_set_mono_after_ce_wb_gain(uint32 context_id, const ik_wb_gain_t *p_after_ce_wb_gain)
{
    amba_ik_system_print_str_5("[IK] set_mono_after_ce_wb_gain is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //MISRA
    }
    if (p_after_ce_wb_gain == NULL) {
        //MISRA
    }
    return IK_ERR_0004;
}

uint32 img_ctx_set_mono_after_ce_wb_gain(uint32 context_id, const ik_wb_gain_t *p_after_ce_wb_gain)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;
    const ik_debug_check_func_t* p_check_func = ik_ctx_get_debug_check_func();
    if(p_check_func != NULL) {
        rval = p_check_func->after_ce_wb(p_after_ce_wb_gain, "mono_after_ce_wb_gain");
    }
    if(rval == IK_OK) {
        rval |= img_ctx_get_context(context_id, &p_ctx);
        if((p_after_ce_wb_gain != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
            (void)amba_ik_system_lock_mutex(context_id, IK_WAIT_FOREVER);
            (void)amba_ik_system_memcpy(&p_ctx->filters.input_param.mono_after_ce_wb_gain, p_after_ce_wb_gain, sizeof(ik_wb_gain_t));
            ctx_raise_update_flag(&p_ctx->filters.update_flags.liso.mono_after_ce_wb_gain_updated);
            (void)amba_ik_system_unlock_mutex(context_id);
        } else {
            if(p_ctx->organization.initial_flag == 0U) {
                amba_ik_system_print_uint32_5("[IK][ERROR] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
                rval |= IK_ERR_0002;
            } else {
                amba_ik_system_print_str_5("[IK][ERROR] can't get context\n", NULL, NULL, NULL, NULL, NULL);
            }
        }
    }
    return rval;
}

uint32 img_ctx_ivd_get_mono_before_ce_wb_gain(uint32 context_id, ik_wb_gain_t *p_before_ce_wb_gain)
{
    amba_ik_system_print_str_5("[IK] get_mono_before_ce_wb_gain is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //TBD
    }
    if (p_before_ce_wb_gain == NULL) {
        //TBD
    } else {
        (void)amba_ik_system_memcpy(p_before_ce_wb_gain, p_before_ce_wb_gain, sizeof(uint8));
    }
    return IK_ERR_0004;
}

uint32 img_ctx_get_mono_before_ce_wb_gain(uint32 context_id, ik_wb_gain_t *p_before_ce_wb_gain)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval |= img_ctx_get_context(context_id, &p_ctx);
    if((p_before_ce_wb_gain != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        (void)amba_ik_system_lock_mutex(context_id, IK_WAIT_FOREVER);
        (void)amba_ik_system_memcpy(p_before_ce_wb_gain, &p_ctx->filters.input_param.mono_before_ce_wb_gain, sizeof(ik_wb_gain_t));
        (void)amba_ik_system_unlock_mutex(context_id);
    } else {
        if(p_before_ce_wb_gain == NULL) {
            amba_ik_system_print_str_5("[IK][ERROR] NULL pointer in %s\n", __func__, NULL, NULL, NULL, NULL);
            rval = IK_ERR_0005;
        } else if(p_ctx->organization.initial_flag == 0U) {
            amba_ik_system_print_uint32_5("[IK][ERROR] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
            rval = IK_ERR_0002;
        } else {
            amba_ik_system_print_str_5("[IK][ERROR] can't get context\n", NULL, NULL, NULL, NULL, NULL);
        }
    }

    return rval;
}

uint32 img_ctx_ivd_get_mono_after_ce_wb_gain(uint32 context_id, ik_wb_gain_t *p_after_ce_wb_gain)
{
    amba_ik_system_print_str_5("[IK] get_mono_after_ce_wb_gain is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //TBD
    }
    if (p_after_ce_wb_gain == NULL) {
        //TBD
    } else {
        (void)amba_ik_system_memcpy(p_after_ce_wb_gain, p_after_ce_wb_gain, sizeof(uint8));
    }
    return IK_ERR_0004;
}

uint32 img_ctx_get_mono_after_ce_wb_gain(uint32 context_id, ik_wb_gain_t *p_after_ce_wb_gain)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval |= img_ctx_get_context(context_id, &p_ctx);
    if((p_after_ce_wb_gain != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        (void)amba_ik_system_lock_mutex(context_id, IK_WAIT_FOREVER);
        (void)amba_ik_system_memcpy(p_after_ce_wb_gain, &p_ctx->filters.input_param.mono_after_ce_wb_gain, sizeof(ik_wb_gain_t));
        (void)amba_ik_system_unlock_mutex(context_id);
    } else {
        if(p_after_ce_wb_gain == NULL) {
            amba_ik_system_print_str_5("[IK][ERROR] NULL pointer in %s\n", __func__, NULL, NULL, NULL, NULL);
            rval = IK_ERR_0005;
        } else if(p_ctx->organization.initial_flag == 0U) {
            amba_ik_system_print_uint32_5("[IK][ERROR] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
            rval = IK_ERR_0002;
        } else {
            amba_ik_system_print_str_5("[IK][ERROR] can't get context\n", NULL, NULL, NULL, NULL, NULL);
        }
    }

    return rval;
}

uint32 img_ctx_ivd_set_mono_cfa_leakage_filter(uint32 context_id, const ik_cfa_leakage_filter_t *p_cfa_leakage_filter)
{
    amba_ik_system_print_str_5("[IK] set_mono_cfa_leakage_filter is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //MISRA
    }
    if (p_cfa_leakage_filter == NULL) {
        //MISRA
    }
    return IK_ERR_0004;
}

uint32 img_ctx_set_mono_cfa_leakage_filter(uint32 context_id, const ik_cfa_leakage_filter_t *p_cfa_leakage_filter)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;
    const ik_debug_check_func_t* p_check_func = ik_ctx_get_debug_check_func();
    if(p_check_func != NULL) {
        rval = p_check_func->cfa_leakage(p_cfa_leakage_filter, "mono_cfa_leakage_filter");
    }

    if(rval == IK_OK) {
        rval |= img_ctx_get_context(context_id, &p_ctx);
        if((p_cfa_leakage_filter != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
            (void)amba_ik_system_lock_mutex(context_id, IK_WAIT_FOREVER);
            (void)amba_ik_system_memcpy(&p_ctx->filters.input_param.mono_cfa_leakage_filter, p_cfa_leakage_filter, sizeof(ik_cfa_leakage_filter_t));
            ctx_raise_update_flag(&p_ctx->filters.update_flags.liso.mono_cfa_leakage_filter_updated);
            (void)amba_ik_system_unlock_mutex(context_id);
        } else {
            if(p_ctx->organization.initial_flag == 0U) {
                amba_ik_system_print_uint32_5("[IK][ERROR] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
                rval |= IK_ERR_0002;
            } else {
                amba_ik_system_print_str_5("[IK][ERROR] can't get context\n", NULL, NULL, NULL, NULL, NULL);
            }
        }
    }
    return rval;

}

uint32 img_ctx_ivd_set_mono_anti_aliasing(uint32 context_id, const ik_anti_aliasing_t *p_anti_aliasing)
{
    amba_ik_system_print_str_5("[IK] set_mono_anti_aliasing is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //TBD
    }
    if (p_anti_aliasing == NULL) {
        //TBD
    }
    return IK_ERR_0004;
}

uint32 img_ctx_set_mono_anti_aliasing(uint32 context_id, const ik_anti_aliasing_t *p_anti_aliasing)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;
    const ik_debug_check_func_t* p_check_func = ik_ctx_get_debug_check_func();
    if(p_check_func != NULL) {
        rval = p_check_func->anti_aliasing(p_anti_aliasing, "mono_anti_aliasing");
    }
    if(rval == IK_OK) {
        rval |= img_ctx_get_context(context_id, &p_ctx);
        if((p_anti_aliasing != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
            (void)amba_ik_system_lock_mutex(context_id, IK_WAIT_FOREVER);
            (void)amba_ik_system_memcpy(&p_ctx->filters.input_param.mono_anti_aliasing, p_anti_aliasing, sizeof(ik_anti_aliasing_t));
            ctx_raise_update_flag(&p_ctx->filters.update_flags.liso.mono_anti_aliasing_updated);
            (void)amba_ik_system_unlock_mutex(context_id);
        } else {
            if(p_ctx->organization.initial_flag == 0U) {
                amba_ik_system_print_uint32_5("[IK][ERROR] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
                rval |= IK_ERR_0002;
            } else {
                amba_ik_system_print_str_5("[IK][ERROR] can't get context\n", NULL, NULL, NULL, NULL, NULL);
            }
        }
    }
    return rval;

}

uint32 img_ctx_ivd_set_mono_dynamic_bpc(uint32 context_id, const ik_dynamic_bad_pixel_correction_t *p_dynamic_bpc)
{
    amba_ik_system_print_str_5("[IK] set_mono_dynamic_bpc is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //MISRAC
    }
    if (p_dynamic_bpc==NULL) {
        //MISRAC
    }
    return IK_ERR_0004;
}

uint32 img_ctx_set_mono_dynamic_bpc(uint32 context_id, const ik_dynamic_bad_pixel_correction_t *p_dynamic_bpc)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;
    const ik_debug_check_func_t* p_check_func = ik_ctx_get_debug_check_func();
    if(p_check_func != NULL) {
        rval = p_check_func->dynamic_bad_pxl_cor(p_dynamic_bpc, "mono_dynamic_bpc");
    }
    if(rval == IK_OK) {
        rval |= img_ctx_get_context(context_id, &p_ctx);
        if((p_dynamic_bpc != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
            (void)amba_ik_system_lock_mutex(context_id, IK_WAIT_FOREVER);
            (void)amba_ik_system_memcpy(&p_ctx->filters.input_param.mono_dynamic_bpc, p_dynamic_bpc, sizeof(ik_dynamic_bad_pixel_correction_t));
            ctx_raise_update_flag(&p_ctx->filters.update_flags.liso.mono_dynamic_bpc_updated);
            (void)amba_ik_system_unlock_mutex(context_id);
        } else {
            if(p_ctx->organization.initial_flag == 0U) {
                amba_ik_system_print_uint32_5("[IK][ERROR] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
                rval = IK_ERR_0002;
            } else {
                amba_ik_system_print_str_5("[IK][ERROR] can't get context\n", NULL, NULL, NULL, NULL, NULL);
            }
        }
    }
    return rval;
}

uint32 img_ctx_ivd_set_mono_cfa_noise_filter(uint32 context_id, const ik_cfa_noise_filter_t *p_cfa_noise_filter)
{
    amba_ik_system_print_str_5("[IK] set_mono_cfa_noise_filter is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //TBD
    }
    if (p_cfa_noise_filter == NULL) {
        //TBD
    }
    return IK_ERR_0004;
}

uint32 img_ctx_set_mono_cfa_noise_filter(uint32 context_id, const ik_cfa_noise_filter_t *p_cfa_noise_filter)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;
    const ik_debug_check_func_t* p_check_func = ik_ctx_get_debug_check_func();
    if(p_check_func != NULL) {
        rval = p_check_func->cfa_noise_filter(p_cfa_noise_filter, "mono_cfa_noise_filter");
    }
    if(rval == IK_OK) {
        rval |= img_ctx_get_context(context_id, &p_ctx);
        if((p_cfa_noise_filter != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
            (void)amba_ik_system_lock_mutex(context_id, IK_WAIT_FOREVER);
            (void)amba_ik_system_memcpy(&p_ctx->filters.input_param.mono_cfa_noise_filter, p_cfa_noise_filter, sizeof(ik_cfa_noise_filter_t));
            ctx_raise_update_flag(&p_ctx->filters.update_flags.liso.mono_cfa_noise_filter_updated);
            (void)amba_ik_system_unlock_mutex(context_id);
        } else {
            if(p_ctx->organization.initial_flag == 0U) {
                amba_ik_system_print_uint32_5("[IK][ERROR] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
                rval |= IK_ERR_0002;
            } else {
                amba_ik_system_print_str_5("[IK][ERROR] can't get context\n", NULL, NULL, NULL, NULL, NULL);
            }
        }
    }
    return rval;
}

uint32 img_ctx_ivd_set_mono_demosaic(uint32 context_id, const ik_demosaic_t *p_demosaic)
{
    amba_ik_system_print_str_5("[IK] set_mono_demosaic is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //TBD
    }
    if (p_demosaic == NULL) {
        //TBD
    }
    return IK_ERR_0004;
}

uint32 img_ctx_set_mono_demosaic(uint32 context_id, const ik_demosaic_t *p_demosaic)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;
    const ik_debug_check_func_t* p_check_func = ik_ctx_get_debug_check_func();
    if(p_check_func != NULL) {
        rval = p_check_func->demosaic(p_demosaic, "mono_demosaic");
    }
    if(rval == IK_OK) {
        rval |= img_ctx_get_context(context_id, &p_ctx);
        if((p_demosaic != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
            (void)amba_ik_system_lock_mutex(context_id, IK_WAIT_FOREVER);
            (void)amba_ik_system_memcpy(&p_ctx->filters.input_param.mono_demosaic, p_demosaic, sizeof(ik_demosaic_t));
            ctx_raise_update_flag(&p_ctx->filters.update_flags.liso.mono_demosaic_updated);
            (void)amba_ik_system_unlock_mutex(context_id);
        } else {
            if(p_ctx->organization.initial_flag == 0U) {
                amba_ik_system_print_uint32_5("[IK][ERROR] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
                rval |= IK_ERR_0002;
            } else {
                amba_ik_system_print_str_5("[IK][ERROR] can't get context\n", NULL, NULL, NULL, NULL, NULL);
            }
        }
    }
    return rval;
}

uint32 img_ctx_ivd_set_mono_cc_reg(uint32 context_id, const ik_color_correction_reg_t *p_color_correction_reg)
{
    amba_ik_system_print_str_5("[IK] set_mono_color_correction_reg is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //TBD
    }
    if (p_color_correction_reg == NULL) {
        //TBD
    }
    return IK_ERR_0004;
}

uint32 img_ctx_set_mono_color_correction_reg(uint32 context_id, const ik_color_correction_reg_t *p_color_correction_reg)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;
    const ik_debug_check_func_t* p_check_func = ik_ctx_get_debug_check_func();
    if(p_check_func != NULL) {
        rval = p_check_func->color_correction_reg(p_color_correction_reg, "mono_color_correction_reg");
    }
    if(rval == IK_OK) {
        rval |= img_ctx_get_context(context_id, &p_ctx);
        if((p_color_correction_reg != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
            (void)amba_ik_system_lock_mutex(context_id, IK_WAIT_FOREVER);
            (void)amba_ik_system_memcpy(&p_ctx->filters.input_param.mono_color_correction_reg, p_color_correction_reg, sizeof(ik_color_correction_reg_t));
            ctx_raise_update_flag(&p_ctx->filters.update_flags.liso.mono_color_correction_reg_updated);
            (void)amba_ik_system_unlock_mutex(context_id);
        } else {
            if(p_ctx->organization.initial_flag == 0U) {
                amba_ik_system_print_uint32_5("[IK][ERROR] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
                rval |= IK_ERR_0002;
            } else {
                amba_ik_system_print_str_5("[IK][ERROR] can't get context\n", NULL, NULL, NULL, NULL, NULL);
            }
        }
    }
    return rval;
}

uint32 img_ctx_ivd_set_mono_color_correction(uint32 context_id, const ik_color_correction_t *p_color_correction)
{
    amba_ik_system_print_str_5("[IK] set_color_correction is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //TBD
    }
    if (p_color_correction==NULL) {
        //TBD
    }
    return IK_ERR_0004;
}

uint32 img_ctx_set_mono_color_correction(uint32 context_id, const ik_color_correction_t *p_color_correction)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;
    const ik_debug_check_func_t* p_check_func = ik_ctx_get_debug_check_func();
    if(p_check_func != NULL) {
        rval = p_check_func->color_correction(p_color_correction, "mono_color_correction");
    }
    if(rval == IK_OK) {
        rval |= img_ctx_get_context(context_id, &p_ctx);
        if((p_color_correction != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
            (void)amba_ik_system_lock_mutex(context_id, IK_WAIT_FOREVER);
            (void)amba_ik_system_memcpy(&p_ctx->filters.input_param.mono_color_correction, p_color_correction, sizeof(ik_color_correction_t));
            ctx_raise_update_flag(&p_ctx->filters.update_flags.liso.mono_color_correction_updated);
            (void)amba_ik_system_unlock_mutex(context_id);
        } else {
            if(p_ctx->organization.initial_flag == 0U) {
                amba_ik_system_print_uint32_5("[IK][ERROR] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
                rval |= IK_ERR_0002;
            } else {
                amba_ik_system_print_str_5("[IK][ERROR] can't get context\n", NULL, NULL, NULL, NULL, NULL);
            }
        }
    }
    return rval;
}

uint32 img_ctx_ivd_set_mono_tone_curve(uint32 context_id, const ik_tone_curve_t *p_tone_curve)
{
    amba_ik_system_print_str_5("[IK] set_mono_tone_curve is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //TBD
    }
    if (p_tone_curve == NULL) {
        //TBD
    }
    return IK_ERR_0004;
}

uint32 img_ctx_set_mono_tone_curve(uint32 context_id, const ik_tone_curve_t *p_tone_curve)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;
    const ik_debug_check_func_t* p_check_func = ik_ctx_get_debug_check_func();
    if(p_check_func != NULL) {
        rval = p_check_func->tone_curve(p_tone_curve, "mono_tone_curve");
    }
    if(rval == IK_OK) {
        rval |= img_ctx_get_context(context_id, &p_ctx);
        if((p_tone_curve != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
            (void)amba_ik_system_lock_mutex(context_id, IK_WAIT_FOREVER);
            (void)amba_ik_system_memcpy(&p_ctx->filters.input_param.mono_tone_curve, p_tone_curve, sizeof(ik_tone_curve_t));
            ctx_raise_update_flag(&p_ctx->filters.update_flags.liso.mono_tone_curve_updated);
            (void)amba_ik_system_unlock_mutex(context_id);
        } else {
            if(p_ctx->organization.initial_flag == 0U) {
                amba_ik_system_print_uint32_5("[IK][ERROR] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
                rval |= IK_ERR_0002;
            } else {
                amba_ik_system_print_str_5("[IK][ERROR] can't get context\n", NULL, NULL, NULL, NULL, NULL);
            }
        }
    }
    return rval;
}

uint32 img_ctx_ivd_set_mono_rgb_to_yuv_matrix(uint32 context_id, const ik_rgb_to_yuv_matrix_t *p_rgb_to_yuv_matrix)
{
    amba_ik_system_print_str_5("[IK] set_mono_rgb_to_yuv_matrix is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //TBD
    }
    if (p_rgb_to_yuv_matrix == NULL) {
        //TBD
    }
    return IK_ERR_0004;
}

uint32 img_ctx_set_mono_rgb_to_yuv_matrix(uint32 context_id, const ik_rgb_to_yuv_matrix_t *p_rgb_to_yuv_matrix)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;
    const ik_debug_check_func_t* p_check_func = ik_ctx_get_debug_check_func();
    if(p_check_func != NULL) {
        rval = p_check_func->rgb_to_yuv_matrix(p_rgb_to_yuv_matrix, "mono_rgb_to_yuv_matrix");
    }
    if(rval == IK_OK) {
        rval |= img_ctx_get_context(context_id, &p_ctx);
        if((p_rgb_to_yuv_matrix != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
            (void)amba_ik_system_lock_mutex(context_id, IK_WAIT_FOREVER);
            (void)amba_ik_system_memcpy(&p_ctx->filters.input_param.mono_rgb_to_yuv_matrix, p_rgb_to_yuv_matrix, sizeof(ik_rgb_to_yuv_matrix_t));
            ctx_raise_update_flag(&p_ctx->filters.update_flags.liso.mono_rgb_to_yuv_matrix_updated);
            (void)amba_ik_system_unlock_mutex(context_id);
        } else {
            if(p_ctx->organization.initial_flag == 0U) {
                amba_ik_system_print_uint32_5("[IK][ERROR] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
                rval |= IK_ERR_0002;
            } else {
                amba_ik_system_print_str_5("[IK][ERROR] can't get context\n", NULL, NULL, NULL, NULL, NULL);
            }
        }
    }
    return rval;
}

uint32 img_ctx_ivd_get_mono_cfa_leakage_filter(uint32 context_id, ik_cfa_leakage_filter_t *p_cfa_leakage_filter)
{
    amba_ik_system_print_str_5("[IK] get_mono_cfa_leakage_filter is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //TBD
    }
    if (p_cfa_leakage_filter==NULL) {
        //TBD
    } else {
        (void)amba_ik_system_memcpy(p_cfa_leakage_filter, p_cfa_leakage_filter, sizeof(uint8));
    }
    return IK_ERR_0004;
}

uint32 img_ctx_get_mono_cfa_leakage_filter(uint32 context_id, ik_cfa_leakage_filter_t *p_cfa_leakage_filter)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval |= img_ctx_get_context(context_id, &p_ctx);
    if((p_cfa_leakage_filter != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        (void)amba_ik_system_lock_mutex(context_id, IK_WAIT_FOREVER);
        (void)amba_ik_system_memcpy(p_cfa_leakage_filter, &p_ctx->filters.input_param.mono_cfa_leakage_filter, sizeof(ik_cfa_leakage_filter_t));
        (void)amba_ik_system_unlock_mutex(context_id);
    } else {
        if(p_cfa_leakage_filter == NULL) {
            amba_ik_system_print_str_5("[IK][ERROR] NULL pointer in %s\n", __func__, NULL, NULL, NULL, NULL);
            rval = IK_ERR_0005;
        } else if(p_ctx->organization.initial_flag == 0U) {
            amba_ik_system_print_uint32_5("[IK][ERROR] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
            rval = IK_ERR_0002;
        } else {
            amba_ik_system_print_str_5("[IK][ERROR] can't get context\n", NULL, NULL, NULL, NULL, NULL);
        }
    }

    return rval;
}

uint32 img_ctx_ivd_get_mono_anti_aliasing(uint32 context_id, ik_anti_aliasing_t *p_anti_aliasing)
{
    amba_ik_system_print_str_5("[IK] get_mono_anti_aliasing is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //TBD
    }
    if (p_anti_aliasing == NULL) {
        //TBD
    } else {
        (void) amba_ik_system_memcpy(p_anti_aliasing, p_anti_aliasing, sizeof(uint8));
    }
    return IK_ERR_0004;
}

uint32 img_ctx_get_mono_anti_aliasing(uint32 context_id, ik_anti_aliasing_t *p_anti_aliasing)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval |= img_ctx_get_context(context_id, &p_ctx);
    if((p_anti_aliasing != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        (void)amba_ik_system_lock_mutex(context_id, IK_WAIT_FOREVER);
        (void)amba_ik_system_memcpy(p_anti_aliasing, &p_ctx->filters.input_param.mono_anti_aliasing, sizeof(ik_anti_aliasing_t));
        (void)amba_ik_system_unlock_mutex(context_id);
    } else {
        if(p_anti_aliasing == NULL) {
            amba_ik_system_print_str_5("[IK][ERROR] NULL pointer in %s\n", __func__, NULL, NULL, NULL, NULL);
            rval = IK_ERR_0005;
        } else if(p_ctx->organization.initial_flag == 0U) {
            amba_ik_system_print_uint32_5("[IK][ERROR] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
            rval = IK_ERR_0002;
        } else {
            amba_ik_system_print_str_5("[IK][ERROR] can't get context\n", NULL, NULL, NULL, NULL, NULL);
        }
    }

    return rval;
}

uint32 img_ctx_ivd_get_mono_dynamic_bpc(uint32 context_id, ik_dynamic_bad_pixel_correction_t *p_dynamic_bpc)
{
    amba_ik_system_print_str_5("[IK] get_mono_dynamic_bpc is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //TBD
    }
    if (p_dynamic_bpc == NULL) {
        //TBD
    } else {
        (void)amba_ik_system_memcpy(p_dynamic_bpc, p_dynamic_bpc, sizeof(uint8));
    }
    return IK_ERR_0004;
}

uint32 img_ctx_get_mono_dynamic_bpc(uint32 context_id, ik_dynamic_bad_pixel_correction_t *p_dynamic_bpc)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval |= img_ctx_get_context(context_id, &p_ctx);
    if((p_dynamic_bpc != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        (void)amba_ik_system_lock_mutex(context_id, IK_WAIT_FOREVER);
        (void)amba_ik_system_memcpy(p_dynamic_bpc, &p_ctx->filters.input_param.mono_dynamic_bpc, sizeof(ik_dynamic_bad_pixel_correction_t));
        (void)amba_ik_system_unlock_mutex(context_id);
    } else {
        if(p_dynamic_bpc == NULL) {
            amba_ik_system_print_str_5("[IK][ERROR] NULL pointer in %s\n", __func__, NULL, NULL, NULL, NULL);
            rval = IK_ERR_0005;
        } else if(p_ctx->organization.initial_flag == 0U) {
            amba_ik_system_print_uint32_5("[IK][ERROR] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
            rval = IK_ERR_0002;
        } else {
            amba_ik_system_print_str_5("[IK][ERROR] can't get context\n", NULL, NULL, NULL, NULL, NULL);
        }
    }

    return rval;
}

uint32 img_ctx_ivd_get_mono_cfa_noise_filter(uint32 context_id, ik_cfa_noise_filter_t *p_cfa_noise_filter)
{
    amba_ik_system_print_str_5("[IK] get_mono_cfa_noise_filter is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //TBD
    }
    if (p_cfa_noise_filter == NULL) {
        //TBD
    } else {
        (void)amba_ik_system_memcpy(p_cfa_noise_filter, p_cfa_noise_filter, sizeof(uint8));
    }
    return IK_ERR_0004;
}

uint32 img_ctx_get_mono_cfa_noise_filter(uint32 context_id, ik_cfa_noise_filter_t *p_cfa_noise_filter)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval |= img_ctx_get_context(context_id, &p_ctx);
    if((p_cfa_noise_filter != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        (void)amba_ik_system_lock_mutex(context_id, IK_WAIT_FOREVER);
        (void)amba_ik_system_memcpy(p_cfa_noise_filter, &p_ctx->filters.input_param.mono_cfa_noise_filter, sizeof(ik_cfa_noise_filter_t));
        (void)amba_ik_system_unlock_mutex(context_id);
    } else {
        if(p_cfa_noise_filter == NULL) {
            amba_ik_system_print_str_5("[IK][ERROR] NULL pointer in %s\n", __func__, NULL, NULL, NULL, NULL);
            rval = IK_ERR_0005;
        } else if(p_ctx->organization.initial_flag == 0U) {
            amba_ik_system_print_uint32_5("[IK][ERROR] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
            rval = IK_ERR_0002;
        } else {
            amba_ik_system_print_str_5("[IK][ERROR] can't get context\n", NULL, NULL, NULL, NULL, NULL);
        }
    }

    return rval;
}

uint32 img_ctx_ivd_get_mono_demosaic(uint32 context_id, ik_demosaic_t *p_demosaic)
{
    amba_ik_system_print_str_5("[IK] get_mono_demosaic is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //TBD
    }
    if (p_demosaic == NULL) {
        //TBD
    } else {
        (void)amba_ik_system_memcpy(p_demosaic, p_demosaic, sizeof(uint8));
    }
    return IK_ERR_0004;
}

uint32 img_ctx_get_mono_demosaic(uint32 context_id, ik_demosaic_t *p_demosaic)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval |= img_ctx_get_context(context_id, &p_ctx);
    if((p_demosaic != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        (void)amba_ik_system_lock_mutex(context_id, IK_WAIT_FOREVER);
        (void)amba_ik_system_memcpy(p_demosaic, &p_ctx->filters.input_param.mono_demosaic, sizeof(ik_demosaic_t));
        (void)amba_ik_system_unlock_mutex(context_id);
    } else {
        if(p_demosaic == NULL) {
            amba_ik_system_print_str_5("[IK][ERROR] NULL pointer in %s\n", __func__, NULL, NULL, NULL, NULL);
            rval = IK_ERR_0005;
        } else if(p_ctx->organization.initial_flag == 0U) {
            amba_ik_system_print_uint32_5("[IK][ERROR] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
            rval = IK_ERR_0002;
        } else {
            amba_ik_system_print_str_5("[IK][ERROR] can't get context\n", NULL, NULL, NULL, NULL, NULL);
        }
    }

    return rval;
}

uint32 img_ctx_ivd_get_mono_cc_reg(uint32 context_id, ik_color_correction_reg_t *p_color_correction_reg)
{
    amba_ik_system_print_str_5("[IK] get_mono_color_correction_reg is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //TBD
    }
    if (p_color_correction_reg == NULL) {
        //TBD
    } else {
        (void)amba_ik_system_memcpy(p_color_correction_reg, p_color_correction_reg, sizeof(uint8));
    }
    return IK_ERR_0004;
}

uint32 img_ctx_get_mono_color_correction_reg(uint32 context_id, ik_color_correction_reg_t *p_color_correction_reg)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval |= img_ctx_get_context(context_id, &p_ctx);
    if((p_color_correction_reg != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        (void)amba_ik_system_lock_mutex(context_id, IK_WAIT_FOREVER);
        (void)amba_ik_system_memcpy(p_color_correction_reg, &p_ctx->filters.input_param.mono_color_correction_reg, sizeof(ik_color_correction_reg_t));
        (void)amba_ik_system_unlock_mutex(context_id);
    } else {
        if(p_color_correction_reg == NULL) {
            amba_ik_system_print_str_5("[IK][ERROR] NULL pointer in %s\n", __func__, NULL, NULL, NULL, NULL);
            rval = IK_ERR_0005;
        } else if(p_ctx->organization.initial_flag == 0U) {
            amba_ik_system_print_uint32_5("[IK][ERROR] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
            rval = IK_ERR_0002;
        } else {
            amba_ik_system_print_str_5("[IK][ERROR] can't get context\n", NULL, NULL, NULL, NULL, NULL);
        }
    }

    return rval;
}

uint32 img_ctx_ivd_get_mono_color_correction(uint32 context_id, ik_color_correction_t *p_color_correction)
{
    amba_ik_system_print_str_5("[IK] get_mono_color_correction is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //TBD
    }
    if (p_color_correction == NULL) {
        //TBD
    } else {
        (void)amba_ik_system_memcpy(p_color_correction, p_color_correction, sizeof(uint8));
    }
    return IK_ERR_0004;
}

uint32 img_ctx_get_mono_color_correction(uint32 context_id, ik_color_correction_t *p_color_correction)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval |= img_ctx_get_context(context_id, &p_ctx);
    if((p_color_correction != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        (void)amba_ik_system_lock_mutex(context_id, IK_WAIT_FOREVER);
        (void)amba_ik_system_memcpy(p_color_correction, &p_ctx->filters.input_param.mono_color_correction, sizeof(ik_color_correction_t));
        (void)amba_ik_system_unlock_mutex(context_id);
    } else {
        if(p_color_correction == NULL) {
            amba_ik_system_print_str_5("[IK][ERROR] NULL pointer in %s\n", __func__, NULL, NULL, NULL, NULL);
            rval = IK_ERR_0005;
        } else if(p_ctx->organization.initial_flag == 0U) {
            amba_ik_system_print_uint32_5("[IK][ERROR] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
            rval = IK_ERR_0002;
        } else {
            amba_ik_system_print_str_5("[IK][ERROR] can't get context\n", NULL, NULL, NULL, NULL, NULL);
        }
    }

    return rval;
}

uint32 img_ctx_ivd_get_mono_tone_curve(uint32 context_id, ik_tone_curve_t *p_tone_curve)
{
    amba_ik_system_print_str_5("[IK] get_tone_curve is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //TBD
    }
    if (p_tone_curve == NULL) {
        //TBD
    } else {
        (void)amba_ik_system_memcpy(p_tone_curve, p_tone_curve, sizeof(uint8));
    }
    return IK_ERR_0004;
}

uint32 img_ctx_get_mono_tone_curve(uint32 context_id, ik_tone_curve_t *p_tone_curve)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval |= img_ctx_get_context(context_id, &p_ctx);
    if((p_tone_curve != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        (void)amba_ik_system_lock_mutex(context_id, IK_WAIT_FOREVER);
        (void)amba_ik_system_memcpy(p_tone_curve, &p_ctx->filters.input_param.mono_tone_curve, sizeof(ik_tone_curve_t));
        (void)amba_ik_system_unlock_mutex(context_id);
    } else {
        if(p_tone_curve == NULL) {
            amba_ik_system_print_str_5("[IK][ERROR] NULL pointer in %s\n", __func__, NULL, NULL, NULL, NULL);
            rval = IK_ERR_0005;
        } else if(p_ctx->organization.initial_flag == 0U) {
            amba_ik_system_print_uint32_5("[IK][ERROR] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
            rval = IK_ERR_0002;
        } else {
            amba_ik_system_print_str_5("[IK][ERROR] can't get context\n", NULL, NULL, NULL, NULL, NULL);
        }
    }

    return rval;
}

uint32 img_ctx_ivd_get_mono_rgb_to_yuv_matrix(uint32 context_id, ik_rgb_to_yuv_matrix_t *p_rgb_to_yuv_matrix)
{
    amba_ik_system_print_str_5("[IK] get_rgb_to_yuv_matrix is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //TBD
    }
    if (p_rgb_to_yuv_matrix == NULL) {
        //TBD
    } else {
        (void)amba_ik_system_memcpy(p_rgb_to_yuv_matrix, p_rgb_to_yuv_matrix, sizeof(uint8));
    }
    return IK_ERR_0004;
}

uint32 img_ctx_get_mono_rgb_to_yuv_matrix(uint32 context_id, ik_rgb_to_yuv_matrix_t *p_rgb_to_yuv_matrix)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval |= img_ctx_get_context(context_id, &p_ctx);
    if((p_rgb_to_yuv_matrix != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        (void)amba_ik_system_lock_mutex(context_id, IK_WAIT_FOREVER);
        (void)amba_ik_system_memcpy(p_rgb_to_yuv_matrix, &p_ctx->filters.input_param.mono_rgb_to_yuv_matrix, sizeof(ik_rgb_to_yuv_matrix_t));
        (void)amba_ik_system_unlock_mutex(context_id);
    } else {
        if(p_rgb_to_yuv_matrix == NULL) {
            amba_ik_system_print_str_5("[IK][ERROR] NULL pointer in %s\n", __func__, NULL, NULL, NULL, NULL);
            rval = IK_ERR_0005;
        } else if(p_ctx->organization.initial_flag == 0U) {
            amba_ik_system_print_uint32_5("[IK][ERROR] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
            rval = IK_ERR_0002;
        } else {
            amba_ik_system_print_str_5("[IK][ERROR] can't get context\n", NULL, NULL, NULL, NULL, NULL);
        }
    }

    return rval;
}

uint32 img_ctx_ivd_set_mono_first_luma_processing_mode(uint32 context_id, const ik_first_luma_process_mode_t *p_first_luma_process_mode)
{
    amba_ik_system_print_str_5("[IK] set_mono_first_luma_processing_mode is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //TBD
    }
    if (p_first_luma_process_mode==NULL) {
        //TBD
    }
    return IK_ERR_0004;
}

uint32 img_ctx_set_mono_first_luma_processing_mode(uint32 context_id, const ik_first_luma_process_mode_t *p_first_luma_process_mode)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;
    const ik_debug_check_func_t* p_check_func = ik_ctx_get_debug_check_func();
    if(p_check_func != NULL) {
        rval = p_check_func->first_luma_proc_mode(p_first_luma_process_mode, "mono_first_luma_processing_mode");
    }
    if(rval == IK_OK) {
        rval |= img_ctx_get_context(context_id, &p_ctx);
        if((p_first_luma_process_mode != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
            (void)amba_ik_system_lock_mutex(context_id, IK_WAIT_FOREVER);
            (void)amba_ik_system_memcpy(&p_ctx->filters.input_param.mono_first_luma_process_mode, p_first_luma_process_mode, sizeof(ik_first_luma_process_mode_t));
            ctx_raise_update_flag(&p_ctx->filters.update_flags.liso.mono_first_luma_processing_mode_updated);
            (void)amba_ik_system_unlock_mutex(context_id);
        } else {
            if(p_ctx->organization.initial_flag == 0U) {
                amba_ik_system_print_uint32_5("[IK][ERROR] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
                rval |= IK_ERR_0002;
            } else {
                amba_ik_system_print_str_5("[IK][ERROR] can't get context\n", NULL, NULL, NULL, NULL, NULL);
            }
        }
    }
    return rval;
}

uint32 img_ctx_ivd_get_mono_first_luma_processing_mode(uint32 context_id, ik_first_luma_process_mode_t *p_first_luma_process_mode)
{
    amba_ik_system_print_str_5("[IK] get_mono_first_luma_processing_mode is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //TBD
    }
    if (p_first_luma_process_mode == NULL) {
        //TBD
    } else {
        (void)amba_ik_system_memcpy(p_first_luma_process_mode, p_first_luma_process_mode, sizeof(uint8));
    }
    return IK_ERR_0004;
}

uint32 img_ctx_get_mono_first_luma_processing_mode(uint32 context_id, ik_first_luma_process_mode_t *p_first_luma_process_mode)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval |= img_ctx_get_context(context_id, &p_ctx);
    if((p_first_luma_process_mode != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        (void)amba_ik_system_lock_mutex(context_id, IK_WAIT_FOREVER);
        (void)amba_ik_system_memcpy(p_first_luma_process_mode, &p_ctx->filters.input_param.mono_first_luma_process_mode, sizeof(ik_first_luma_process_mode_t));
        (void)amba_ik_system_unlock_mutex(context_id);
    } else {
        if(p_first_luma_process_mode == NULL) {
            amba_ik_system_print_str_5("[IK][ERROR] NULL pointer in %s\n", __func__, NULL, NULL, NULL, NULL);
            rval = IK_ERR_0005;
        } else if(p_ctx->organization.initial_flag == 0U) {
            amba_ik_system_print_uint32_5("[IK][ERROR] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
            rval = IK_ERR_0002;
        } else {
            amba_ik_system_print_str_5("[IK][ERROR] can't get context\n", NULL, NULL, NULL, NULL, NULL);
        }
    }

    return rval;
}


uint32 img_ctx_ivd_set_mono_advance_spatial_filter(uint32 context_id, const ik_adv_spatial_filter_t *p_advance_spatial_filter)
{
    amba_ik_system_print_str_5("[IK] set_mono_advance_spatial_filter is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //TBD
    }
    if (p_advance_spatial_filter==NULL) {
        //TBD
    }
    return IK_ERR_0004;
}

uint32 img_ctx_set_mono_advance_spatial_filter(uint32 context_id, const ik_adv_spatial_filter_t *p_advance_spatial_filter)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;
    const ik_debug_check_func_t* p_check_func = ik_ctx_get_debug_check_func();
    if(p_check_func != NULL) {
        rval = p_check_func->adv_spat_fltr(p_advance_spatial_filter, "mono_advance_spatial_filter");
    }
    if(rval == IK_OK) {
        rval |= img_ctx_get_context(context_id, &p_ctx);
        if((p_advance_spatial_filter != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
            (void)amba_ik_system_lock_mutex(context_id, IK_WAIT_FOREVER);
            (void)amba_ik_system_memcpy(&p_ctx->filters.input_param.mono_advance_spatial_filter, p_advance_spatial_filter, sizeof(ik_adv_spatial_filter_t));
            ctx_raise_update_flag(&p_ctx->filters.update_flags.liso.mono_advance_spatial_filter_updated);
            (void)amba_ik_system_unlock_mutex(context_id);
        } else {
            if(p_ctx->organization.initial_flag == 0U) {
                amba_ik_system_print_uint32_5("[IK][ERROR] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
                rval |= IK_ERR_0002;
            } else {
                amba_ik_system_print_str_5("[IK][ERROR] can't get context\n", NULL, NULL, NULL, NULL, NULL);
            }
        }
    }
    return rval;
}

uint32 img_ctx_ivd_get_mono_advance_spatial_filter(uint32 context_id, ik_adv_spatial_filter_t *p_advance_spatial_filter)
{
    amba_ik_system_print_str_5("[IK] get_mono_advance_spatial_filter is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //TBD
    }
    if (p_advance_spatial_filter == NULL) {
        //TBD
    } else {
        (void)amba_ik_system_memcpy(p_advance_spatial_filter, p_advance_spatial_filter, sizeof(uint8));
    }
    return IK_ERR_0004;
}

uint32 img_ctx_get_mono_advance_spatial_filter(uint32 context_id, ik_adv_spatial_filter_t *p_advance_spatial_filter)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval |= img_ctx_get_context(context_id, &p_ctx);
    if((p_advance_spatial_filter != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        (void)amba_ik_system_lock_mutex(context_id, IK_WAIT_FOREVER);
        (void)amba_ik_system_memcpy(p_advance_spatial_filter, &p_ctx->filters.input_param.mono_advance_spatial_filter, sizeof(ik_adv_spatial_filter_t));
        (void)amba_ik_system_unlock_mutex(context_id);
    } else {
        if(p_advance_spatial_filter == NULL) {
            amba_ik_system_print_str_5("[IK][ERROR] NULL pointer in %s\n", __func__, NULL, NULL, NULL, NULL);
            rval = IK_ERR_0005;
        } else if(p_ctx->organization.initial_flag == 0U) {
            amba_ik_system_print_uint32_5("[IK][ERROR] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
            rval = IK_ERR_0002;
        } else {
            amba_ik_system_print_str_5("[IK][ERROR] can't get context\n", NULL, NULL, NULL, NULL, NULL);
        }
    }

    return rval;
}

uint32 img_ctx_ivd_set_mono_fstshpns_both(uint32 context_id, const ik_first_sharpen_both_t *p_first_sharpen_both)
{
    amba_ik_system_print_str_5("[IK] set_mono_fstshpns_both is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //TBD
    }
    if (p_first_sharpen_both==NULL) {
        //TBD
    }
    return IK_ERR_0004;
}

uint32 img_ctx_set_mono_fstshpns_both(uint32 context_id, const ik_first_sharpen_both_t *p_first_sharpen_both)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;
    const ik_debug_check_func_t* p_check_func = ik_ctx_get_debug_check_func();
    if(p_check_func != NULL) {
        rval = p_check_func->first_sharpen_both(p_first_sharpen_both, "mono_fstshpns_both");
    }
    if(rval == IK_OK) {
        rval |= img_ctx_get_context(context_id, &p_ctx);
        if((p_first_sharpen_both != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
            (void)amba_ik_system_lock_mutex(context_id, IK_WAIT_FOREVER);
            (void)amba_ik_system_memcpy(&p_ctx->filters.input_param.mono_first_sharpen_both, p_first_sharpen_both, sizeof(ik_first_sharpen_both_t));
            ctx_raise_update_flag(&p_ctx->filters.update_flags.liso.mono_fstshpns_both_updated);
            (void)amba_ik_system_unlock_mutex(context_id);
        } else {
            if(p_ctx->organization.initial_flag == 0U) {
                amba_ik_system_print_uint32_5("[IK][ERROR] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
                rval |= IK_ERR_0002;
            } else {
                amba_ik_system_print_str_5("[IK][ERROR] can't get context\n", NULL, NULL, NULL, NULL, NULL);
            }
        }
    }
    return rval;
}

uint32 img_ctx_ivd_set_mono_fstshpns_noise(uint32 context_id, const ik_first_sharpen_noise_t *p_first_sharpen_noise)
{
    amba_ik_system_print_str_5("[IK] set_mono_fstshpns_noise is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //TBD
    }
    if (p_first_sharpen_noise==NULL) {
        //TBD
    }
    return IK_ERR_0004;
}

uint32 img_ctx_set_mono_fstshpns_noise(uint32 context_id, const ik_first_sharpen_noise_t *p_first_sharpen_noise)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;
    const ik_debug_check_func_t* p_check_func = ik_ctx_get_debug_check_func();
    if(p_check_func != NULL) {
        rval = p_check_func->first_sharpen_noise(p_first_sharpen_noise, "mono_fstshpns_noise");
    }
    if(rval == IK_OK) {
        rval |= img_ctx_get_context(context_id, &p_ctx);
        if((p_first_sharpen_noise != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
            (void)amba_ik_system_lock_mutex(context_id, IK_WAIT_FOREVER);
            (void)amba_ik_system_memcpy(&p_ctx->filters.input_param.mono_first_sharpen_noise, p_first_sharpen_noise, sizeof(ik_first_sharpen_noise_t));
            ctx_raise_update_flag(&p_ctx->filters.update_flags.liso.mono_fstshpns_noise_updated);
            (void)amba_ik_system_unlock_mutex(context_id);
        } else {
            if(p_ctx->organization.initial_flag == 0U) {
                amba_ik_system_print_uint32_5("[IK][ERROR] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
                rval |= IK_ERR_0002;
            } else {
                amba_ik_system_print_str_5("[IK][ERROR] can't get context\n", NULL, NULL, NULL, NULL, NULL);
            }
        }
    }
    return rval;
}

uint32 img_ctx_ivd_set_mono_fstshpns_fir(uint32 context_id, const ik_first_sharpen_fir_t *p_first_sharpen_fir)
{
    amba_ik_system_print_str_5("[IK] set_mono_first_sharpen_fir is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //TBD
    }
    if (p_first_sharpen_fir==NULL) {
        //TBD
    }
    return IK_ERR_0004;
}

uint32 img_ctx_set_mono_fstshpns_fir(uint32 context_id, const ik_first_sharpen_fir_t *p_first_sharpen_fir)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;
    const ik_debug_check_func_t* p_check_func = ik_ctx_get_debug_check_func();
    if(p_check_func != NULL) {
        rval = p_check_func->first_sharpen_fir(p_first_sharpen_fir, "mono_fstshpns_fir");
    }
    if(rval == IK_OK) {
        rval |= img_ctx_get_context(context_id, &p_ctx);
        if((p_first_sharpen_fir != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
            (void)amba_ik_system_lock_mutex(context_id, IK_WAIT_FOREVER);
            (void)amba_ik_system_memcpy(&p_ctx->filters.input_param.mono_first_sharpen_fir, p_first_sharpen_fir, sizeof(ik_first_sharpen_fir_t));
            ctx_raise_update_flag(&p_ctx->filters.update_flags.liso.mono_fstshpns_fir_updated);
            (void)amba_ik_system_unlock_mutex(context_id);
        } else {
            if(p_ctx->organization.initial_flag == 0U) {
                amba_ik_system_print_uint32_5("[IK][ERROR] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
                rval |= IK_ERR_0002;
            } else {
                amba_ik_system_print_str_5("[IK][ERROR] can't get context\n", NULL, NULL, NULL, NULL, NULL);
            }
        }
    }
    return rval;
}

uint32 img_ctx_ivd_set_mono_fstshpns_coring(uint32 context_id, const ik_first_sharpen_coring_t *p_first_sharpen_coring)
{
    amba_ik_system_print_str_5("[IK] set_mono_fstshpns_coring is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //TBD
    }
    if (p_first_sharpen_coring == NULL) {
        //TBD
    }
    return IK_ERR_0004;
}

uint32 img_ctx_set_mono_fstshpns_coring(uint32 context_id, const ik_first_sharpen_coring_t *p_first_sharpen_coring)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;
    const ik_debug_check_func_t* p_check_func = ik_ctx_get_debug_check_func();
    if(p_check_func != NULL) {
        rval = p_check_func->first_sharpen_coring(p_first_sharpen_coring, "mono_fstshpns_coring");
    }
    if(rval == IK_OK) {
        rval |= img_ctx_get_context(context_id, &p_ctx);
        if((p_first_sharpen_coring != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
            (void)amba_ik_system_lock_mutex(context_id, IK_WAIT_FOREVER);
            (void)amba_ik_system_memcpy(&p_ctx->filters.input_param.mono_first_sharpen_coring, p_first_sharpen_coring, sizeof(ik_first_sharpen_coring_t));
            ctx_raise_update_flag(&p_ctx->filters.update_flags.liso.mono_fstshpns_coring_updated);
            (void)amba_ik_system_unlock_mutex(context_id);
        } else {
            if(p_ctx->organization.initial_flag == 0U) {
                amba_ik_system_print_uint32_5("[IK][ERROR] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
                rval |= IK_ERR_0002;
            } else {
                amba_ik_system_print_str_5("[IK][ERROR] can't get context\n", NULL, NULL, NULL, NULL, NULL);
            }
        }
    }
    return rval;
}

uint32 img_ctx_ivd_set_mono_fstshpns_cor_idx_scl(uint32 context_id, const ik_first_sharpen_coring_idx_scale_t *p_first_sharpen_coring_idx_scale)
{
    amba_ik_system_print_str_5("[IK] set_mono_fstshpns_coring_index_scale is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //TBD
    }
    if (p_first_sharpen_coring_idx_scale==NULL) {
        //TBD
    }
    return IK_ERR_0004;
}

uint32 img_ctx_set_mono_fstshpns_cor_idx_scl(uint32 context_id, const ik_first_sharpen_coring_idx_scale_t *p_first_sharpen_coring_idx_scale)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;
    const ik_debug_check_func_t* p_check_func = ik_ctx_get_debug_check_func();
    if(p_check_func != NULL) {
        rval = p_check_func->fstshpns_cor_idx_scl(p_first_sharpen_coring_idx_scale, "mono_fstshpns_cor_idx_scl");
    }
    if(rval == IK_OK) {
        rval |= img_ctx_get_context(context_id, &p_ctx);
        if((p_first_sharpen_coring_idx_scale != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
            (void)amba_ik_system_lock_mutex(context_id, IK_WAIT_FOREVER);
            (void)amba_ik_system_memcpy(&p_ctx->filters.input_param.mono_first_sharpen_coring_idx_scale, p_first_sharpen_coring_idx_scale, sizeof(ik_first_sharpen_coring_idx_scale_t));
            ctx_raise_update_flag(&p_ctx->filters.update_flags.liso.mono_fstshpns_coring_index_scale_updated);
            (void)amba_ik_system_unlock_mutex(context_id);
        } else {
            if(p_ctx->organization.initial_flag == 0U) {
                amba_ik_system_print_uint32_5("[IK][ERROR] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
                rval |= IK_ERR_0002;
            } else {
                amba_ik_system_print_str_5("[IK][ERROR] can't get context\n", NULL, NULL, NULL, NULL, NULL);
            }
        }
    }
    return rval;
}

uint32 img_ctx_ivd_set_mono_fstshpns_min_coring_result(uint32 context_id, const ik_first_sharpen_min_coring_result_t *p_fstshpns_min_coring_result)
{
    amba_ik_system_print_str_5("[IK] set_mono_fstshpns_min_coring_result is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //TBD
    }
    if (p_fstshpns_min_coring_result==NULL) {
        //TBD
    }
    return IK_ERR_0004;
}

uint32 img_ctx_set_mono_fstshpns_min_coring_result(uint32 context_id, const ik_first_sharpen_min_coring_result_t *p_fstshpns_min_coring_result)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;
    const ik_debug_check_func_t* p_check_func = ik_ctx_get_debug_check_func();
    if(p_check_func != NULL) {
        rval = p_check_func->fstshpns_min_cor_rst(p_fstshpns_min_coring_result, "mono_fstshpns_min_coring_result");
    }
    if(rval == IK_OK) {
        rval |= img_ctx_get_context(context_id, &p_ctx);
        if((p_fstshpns_min_coring_result != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
            (void)amba_ik_system_lock_mutex(context_id, IK_WAIT_FOREVER);
            (void)amba_ik_system_memcpy(&p_ctx->filters.input_param.mono_first_sharpen_min_coring_result, p_fstshpns_min_coring_result, sizeof(ik_first_sharpen_min_coring_result_t));
            ctx_raise_update_flag(&p_ctx->filters.update_flags.liso.mono_fstshpns_min_coring_result_updated);
            (void)amba_ik_system_unlock_mutex(context_id);
        } else {
            if(p_ctx->organization.initial_flag == 0U) {
                amba_ik_system_print_uint32_5("[IK][ERROR] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
                rval |= IK_ERR_0002;
            } else {
                amba_ik_system_print_str_5("[IK][ERROR] can't get context\n", NULL, NULL, NULL, NULL, NULL);
            }
        }
    }
    return rval;
}

uint32 img_ctx_ivd_set_mono_fstshpns_max_coring_result(uint32 context_id, const ik_first_sharpen_max_coring_result_t *p_fstshpns_max_coring_result)
{
    amba_ik_system_print_str_5("[IK] set_fstshpns_max_coring_result is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //TBD
    }
    if (p_fstshpns_max_coring_result == NULL) {
        //TBD
    }
    return IK_ERR_0004;
}

uint32 img_ctx_set_mono_fstshpns_max_coring_result(uint32 context_id, const ik_first_sharpen_max_coring_result_t *p_fstshpns_max_coring_result)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;
    const ik_debug_check_func_t* p_check_func = ik_ctx_get_debug_check_func();
    if(p_check_func != NULL) {
        rval = p_check_func->fstshpns_max_cor_rst(p_fstshpns_max_coring_result, "mono_fstshpns_max_coring_result");
    }
    if(rval == IK_OK) {
        rval |= img_ctx_get_context(context_id, &p_ctx);
        if((p_fstshpns_max_coring_result != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
            (void)amba_ik_system_lock_mutex(context_id, IK_WAIT_FOREVER);
            (void)amba_ik_system_memcpy(&p_ctx->filters.input_param.mono_first_sharpen_max_coring_result, p_fstshpns_max_coring_result, sizeof(ik_first_sharpen_max_coring_result_t));
            ctx_raise_update_flag(&p_ctx->filters.update_flags.liso.mono_fstshpns_max_coring_result_updated);
            (void)amba_ik_system_unlock_mutex(context_id);
        } else {
            if(p_ctx->organization.initial_flag == 0U) {
                amba_ik_system_print_uint32_5("[IK][ERROR] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
                rval |= IK_ERR_0002;
            } else {
                amba_ik_system_print_str_5("[IK][ERROR] can't get context\n", NULL, NULL, NULL, NULL, NULL);
            }
        }
    }
    return rval;
}

uint32 img_ctx_ivd_set_mono_fstshpns_scale_coring(uint32 context_id, const ik_first_sharpen_scale_coring_t *p_fstshpns_scale_coring)
{
    amba_ik_system_print_str_5("[IK] set_mono_fstshpns_scale_coring is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //TBD
    }
    if (p_fstshpns_scale_coring == NULL) {
        //TBD
    }
    return IK_ERR_0004;
}

uint32 img_ctx_set_mono_fstshpns_scale_coring(uint32 context_id, const ik_first_sharpen_scale_coring_t *p_fstshpns_scale_coring)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;
    const ik_debug_check_func_t* p_check_func = ik_ctx_get_debug_check_func();
    if(p_check_func != NULL) {
        rval = p_check_func->fstshpns_scl_cor(p_fstshpns_scale_coring, "mono_fstshpns_scale_coring");
    }
    if(rval == IK_OK) {
        rval |= img_ctx_get_context(context_id, &p_ctx);
        if((p_fstshpns_scale_coring != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
            (void)amba_ik_system_lock_mutex(context_id, IK_WAIT_FOREVER);
            (void)amba_ik_system_memcpy(&p_ctx->filters.input_param.mono_first_sharpen_scale_coring, p_fstshpns_scale_coring, sizeof(ik_first_sharpen_scale_coring_t));
            ctx_raise_update_flag(&p_ctx->filters.update_flags.liso.mono_fstshpns_scale_coring_updated);
            (void)amba_ik_system_unlock_mutex(context_id);
        } else {
            if(p_ctx->organization.initial_flag == 0U) {
                amba_ik_system_print_uint32_5("[IK][ERROR] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
                rval |= IK_ERR_0002;
            } else {
                amba_ik_system_print_str_5("[IK][ERROR] can't get context\n", NULL, NULL, NULL, NULL, NULL);
            }
        }
    }
    return rval;
}


uint32 img_ctx_ivd_get_mono_fstshpns_both(uint32 context_id, ik_first_sharpen_both_t *p_first_sharpen_both)
{
    amba_ik_system_print_str_5("[IK] get_mono_fstshpns_both is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //TBD
    }
    if (p_first_sharpen_both == NULL) {
        //TBD
    } else {
        (void)amba_ik_system_memcpy(p_first_sharpen_both, p_first_sharpen_both, sizeof(uint8));
    }
    return IK_ERR_0004;
}

uint32 img_ctx_get_mono_fstshpns_both(uint32 context_id, ik_first_sharpen_both_t *p_first_sharpen_both)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval |= img_ctx_get_context(context_id, &p_ctx);
    if((p_first_sharpen_both != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        (void)amba_ik_system_lock_mutex(context_id, IK_WAIT_FOREVER);
        (void)amba_ik_system_memcpy(p_first_sharpen_both, &p_ctx->filters.input_param.mono_first_sharpen_both, sizeof(ik_first_sharpen_both_t));
        (void)amba_ik_system_unlock_mutex(context_id);
    } else {
        if(p_first_sharpen_both == NULL) {
            amba_ik_system_print_str_5("[IK][ERROR] NULL pointer in %s\n", __func__, NULL, NULL, NULL, NULL);
            rval = IK_ERR_0005;
        } else if(p_ctx->organization.initial_flag == 0U) {
            amba_ik_system_print_uint32_5("[IK][ERROR] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
            rval = IK_ERR_0002;
        } else {
            amba_ik_system_print_str_5("[IK][ERROR] can't get context\n", NULL, NULL, NULL, NULL, NULL);
        }
    }

    return rval;
}

uint32 img_ctx_ivd_get_mono_fstshpns_noise(uint32 context_id, ik_first_sharpen_noise_t *p_first_sharpen_noise)
{
    amba_ik_system_print_str_5("[IK] get_mono_fstshpns_noise is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //TBD
    }
    if (p_first_sharpen_noise == NULL) {
        //TBD
    } else {
        (void)amba_ik_system_memcpy(p_first_sharpen_noise, p_first_sharpen_noise, sizeof(uint8));
    }
    return IK_ERR_0004;
}

uint32 img_ctx_get_mono_fstshpns_noise(uint32 context_id, ik_first_sharpen_noise_t *p_first_sharpen_noise)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval |= img_ctx_get_context(context_id, &p_ctx);
    if((p_first_sharpen_noise != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        (void)amba_ik_system_lock_mutex(context_id, IK_WAIT_FOREVER);
        (void)amba_ik_system_memcpy(p_first_sharpen_noise, &p_ctx->filters.input_param.mono_first_sharpen_noise, sizeof(ik_first_sharpen_noise_t));
        (void)amba_ik_system_unlock_mutex(context_id);
    } else {
        if(p_first_sharpen_noise == NULL) {
            amba_ik_system_print_str_5("[IK][ERROR] NULL pointer in %s\n", __func__, NULL, NULL, NULL, NULL);
            rval = IK_ERR_0005;
        } else if(p_ctx->organization.initial_flag == 0U) {
            amba_ik_system_print_uint32_5("[IK][ERROR] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
            rval = IK_ERR_0002;
        } else {
            amba_ik_system_print_str_5("[IK][ERROR] can't get context\n", NULL, NULL, NULL, NULL, NULL);
        }
    }

    return rval;
}

uint32 img_ctx_ivd_get_mono_fstshpns_fir(uint32 context_id, ik_first_sharpen_fir_t *p_first_sharpen_fir)
{
    amba_ik_system_print_str_5("[IK] get_mono_fstshpns_fir is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //TBD
    }
    if (p_first_sharpen_fir == NULL) {
        //TBD
    } else {
        (void)amba_ik_system_memcpy(p_first_sharpen_fir, p_first_sharpen_fir, sizeof(uint8));
    }
    return IK_ERR_0004;
}

uint32 img_ctx_get_mono_fstshpns_fir(uint32 context_id, ik_first_sharpen_fir_t *p_first_sharpen_fir)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval |= img_ctx_get_context(context_id, &p_ctx);
    if((p_first_sharpen_fir != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        (void)amba_ik_system_lock_mutex(context_id, IK_WAIT_FOREVER);
        (void)amba_ik_system_memcpy(p_first_sharpen_fir, &p_ctx->filters.input_param.mono_first_sharpen_fir, sizeof(ik_first_sharpen_fir_t));
        (void)amba_ik_system_unlock_mutex(context_id);
    } else {
        if(p_first_sharpen_fir == NULL) {
            amba_ik_system_print_str_5("[IK][ERROR] NULL pointer in %s\n", __func__, NULL, NULL, NULL, NULL);
            rval = IK_ERR_0005;
        } else if(p_ctx->organization.initial_flag == 0U) {
            amba_ik_system_print_uint32_5("[IK][ERROR] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
            rval = IK_ERR_0002;
        } else {
            amba_ik_system_print_str_5("[IK][ERROR] can't get context\n", NULL, NULL, NULL, NULL, NULL);
        }
    }

    return rval;
}

uint32 img_ctx_ivd_get_mono_fstshpns_coring(uint32 context_id, ik_first_sharpen_coring_t *p_first_sharpen_coring)
{
    amba_ik_system_print_str_5("[IK] get_mono_fstshpns_coring is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //TBD
    }
    if (p_first_sharpen_coring == NULL) {
        //TBD
    } else {
        (void)amba_ik_system_memcpy(p_first_sharpen_coring, p_first_sharpen_coring, sizeof(uint8));
    }
    return IK_ERR_0004;
}

uint32 img_ctx_get_mono_fstshpns_coring(uint32 context_id, ik_first_sharpen_coring_t *p_first_sharpen_coring)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval |= img_ctx_get_context(context_id, &p_ctx);
    if((p_first_sharpen_coring != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        (void)amba_ik_system_lock_mutex(context_id, IK_WAIT_FOREVER);
        (void)amba_ik_system_memcpy(p_first_sharpen_coring, &p_ctx->filters.input_param.mono_first_sharpen_coring, sizeof(ik_first_sharpen_coring_t));
        (void)amba_ik_system_unlock_mutex(context_id);
    } else {
        if(p_first_sharpen_coring == NULL) {
            amba_ik_system_print_str_5("[IK][ERROR] NULL pointer in %s\n", __func__, NULL, NULL, NULL, NULL);
            rval = IK_ERR_0005;
        } else if(p_ctx->organization.initial_flag == 0U) {
            amba_ik_system_print_uint32_5("[IK][ERROR] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
            rval = IK_ERR_0002;
        } else {
            amba_ik_system_print_str_5("[IK][ERROR] can't get context\n", NULL, NULL, NULL, NULL, NULL);
        }
    }

    return rval;
}

uint32 img_ctx_ivd_get_mono_fstshpns_cor_idx_scl(uint32 context_id, ik_first_sharpen_coring_idx_scale_t *p_fstshpns_coring_index_scale)
{
    amba_ik_system_print_str_5("[IK] get_mono_fstshpns_coring_index_scale is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //TBD
    }
    if (p_fstshpns_coring_index_scale == NULL) {
        //TBD
    } else {
        (void)amba_ik_system_memcpy(p_fstshpns_coring_index_scale, p_fstshpns_coring_index_scale, sizeof(uint8));
    }
    return IK_ERR_0004;
}

uint32 img_ctx_get_mono_fstshpns_cor_idx_scl(uint32 context_id, ik_first_sharpen_coring_idx_scale_t *p_fstshpns_coring_index_scale)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval |= img_ctx_get_context(context_id, &p_ctx);
    if((p_fstshpns_coring_index_scale != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        (void)amba_ik_system_lock_mutex(context_id, IK_WAIT_FOREVER);
        (void)amba_ik_system_memcpy(p_fstshpns_coring_index_scale, &p_ctx->filters.input_param.mono_first_sharpen_coring_idx_scale, sizeof(ik_first_sharpen_coring_idx_scale_t));
        (void)amba_ik_system_unlock_mutex(context_id);
    } else {
        if(p_fstshpns_coring_index_scale == NULL) {
            amba_ik_system_print_str_5("[IK][ERROR] NULL pointer in %s\n", __func__, NULL, NULL, NULL, NULL);
            rval = IK_ERR_0005;
        } else if(p_ctx->organization.initial_flag == 0U) {
            amba_ik_system_print_uint32_5("[IK][ERROR] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
            rval = IK_ERR_0002;
        } else {
            amba_ik_system_print_str_5("[IK][ERROR] can't get context\n", NULL, NULL, NULL, NULL, NULL);
        }
    }

    return rval;
}

uint32 img_ctx_ivd_get_mono_fstshpns_min_coring_result(uint32 context_id, ik_first_sharpen_min_coring_result_t *p_fstshpns_min_coring_result)
{
    amba_ik_system_print_str_5("[IK] get_mono_fstshpns_min_coring_result is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //TBD
    }
    if (p_fstshpns_min_coring_result == NULL) {
        //TBD
    } else {
        (void)amba_ik_system_memcpy(p_fstshpns_min_coring_result, p_fstshpns_min_coring_result, sizeof(uint8));
    }
    return IK_ERR_0004;
}

uint32 img_ctx_get_mono_fstshpns_min_coring_result(uint32 context_id, ik_first_sharpen_min_coring_result_t *p_fstshpns_min_coring_result)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval |= img_ctx_get_context(context_id, &p_ctx);
    if((p_fstshpns_min_coring_result != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        (void)amba_ik_system_lock_mutex(context_id, IK_WAIT_FOREVER);
        (void)amba_ik_system_memcpy(p_fstshpns_min_coring_result, &p_ctx->filters.input_param.mono_first_sharpen_min_coring_result, sizeof(ik_first_sharpen_min_coring_result_t));
        (void)amba_ik_system_unlock_mutex(context_id);
    } else {
        if(p_fstshpns_min_coring_result == NULL) {
            amba_ik_system_print_str_5("[IK][ERROR] NULL pointer in %s\n", __func__, NULL, NULL, NULL, NULL);
            rval = IK_ERR_0005;
        } else if(p_ctx->organization.initial_flag == 0U) {
            amba_ik_system_print_uint32_5("[IK][ERROR] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
            rval = IK_ERR_0002;
        } else {
            amba_ik_system_print_str_5("[IK][ERROR] can't get context\n", NULL, NULL, NULL, NULL, NULL);
        }
    }

    return rval;
}

uint32 img_ctx_ivd_get_mono_fstshpns_max_coring_result(uint32 context_id, ik_first_sharpen_max_coring_result_t *p_fstshpns_max_coring_result)
{
    amba_ik_system_print_str_5("[IK] get_mono_fstshpns_max_coring_result is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //TBD
    }
    if (p_fstshpns_max_coring_result == NULL) {
        //TBD
    } else {
        (void)amba_ik_system_memcpy(p_fstshpns_max_coring_result, p_fstshpns_max_coring_result, sizeof(uint8));
    }
    return IK_ERR_0004;
}

uint32 img_ctx_get_mono_fstshpns_max_coring_result(uint32 context_id, ik_first_sharpen_max_coring_result_t *p_fstshpns_max_coring_result)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval |= img_ctx_get_context(context_id, &p_ctx);
    if((p_fstshpns_max_coring_result != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        (void)amba_ik_system_lock_mutex(context_id, IK_WAIT_FOREVER);
        (void)amba_ik_system_memcpy(p_fstshpns_max_coring_result, &p_ctx->filters.input_param.mono_first_sharpen_max_coring_result, sizeof(ik_first_sharpen_max_coring_result_t));
        (void)amba_ik_system_unlock_mutex(context_id);
    } else {
        if(p_fstshpns_max_coring_result == NULL) {
            amba_ik_system_print_str_5("[IK][ERROR] NULL pointer in %s\n", __func__, NULL, NULL, NULL, NULL);
            rval = IK_ERR_0005;
        } else if(p_ctx->organization.initial_flag == 0U) {
            amba_ik_system_print_uint32_5("[IK][ERROR] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
            rval = IK_ERR_0002;
        } else {
            amba_ik_system_print_str_5("[IK][ERROR] can't get context\n", NULL, NULL, NULL, NULL, NULL);
        }
    }

    return rval;
}

uint32 img_ctx_ivd_get_mono_fstshpns_scale_coring(uint32 context_id, ik_first_sharpen_scale_coring_t *p_fstshpns_scale_coring)
{
    amba_ik_system_print_str_5("[IK] get_mono_fstshpns_scale_coring is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //TBD
    }
    if (p_fstshpns_scale_coring == NULL) {
        //TBD
    } else {
        (void)amba_ik_system_memcpy(p_fstshpns_scale_coring, p_fstshpns_scale_coring, sizeof(uint8));
    }
    return IK_ERR_0004;
}

uint32 img_ctx_get_mono_fstshpns_scale_coring(uint32 context_id, ik_first_sharpen_scale_coring_t *p_fstshpns_scale_coring)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval |= img_ctx_get_context(context_id, &p_ctx);
    if((p_fstshpns_scale_coring != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        (void)amba_ik_system_lock_mutex(context_id, IK_WAIT_FOREVER);
        (void)amba_ik_system_memcpy(p_fstshpns_scale_coring, &p_ctx->filters.input_param.mono_first_sharpen_scale_coring, sizeof(ik_first_sharpen_scale_coring_t));
        (void)amba_ik_system_unlock_mutex(context_id);
    } else {
        if(p_fstshpns_scale_coring == NULL) {
            amba_ik_system_print_str_5("[IK][ERROR] NULL pointer in %s\n", __func__, NULL, NULL, NULL, NULL);
            rval = IK_ERR_0005;
        } else if(p_ctx->organization.initial_flag == 0U) {
            amba_ik_system_print_uint32_5("[IK][ERROR] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
            rval = IK_ERR_0002;
        } else {
            amba_ik_system_print_str_5("[IK][ERROR] can't get context\n", NULL, NULL, NULL, NULL, NULL);
        }
    }

    return rval;
}


uint32 img_ctx_ivd_set_wide_chroma_filter(uint32 context_id, const ik_chroma_filter_t *p_chroma_filter)
{
    amba_ik_system_print_str_5("[IK] set_wide_chroma_filter is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //TBD
    }
    if (p_chroma_filter==NULL) {
        //TBD
    }
    return IK_ERR_0004;
}

uint32 img_ctx_set_wide_chroma_filter(uint32 context_id, const ik_chroma_filter_t *p_chroma_filter)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;
    const ik_debug_check_func_t* p_check_func = ik_ctx_get_debug_check_func();
    if(p_check_func != NULL) {
        rval = p_check_func->chroma_filter(p_chroma_filter, "wide_chroma_filter");
    }
    if(rval == IK_OK) {
        rval |= img_ctx_get_context(context_id, &p_ctx);
        if((p_chroma_filter != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
            (void)amba_ik_system_lock_mutex(context_id, IK_WAIT_FOREVER);
            (void)amba_ik_system_memcpy(&p_ctx->filters.input_param.wide_chroma_filter, p_chroma_filter, sizeof(ik_chroma_filter_t));
            ctx_raise_update_flag(&p_ctx->filters.update_flags.liso.wide_chroma_filter);
            (void)amba_ik_system_unlock_mutex(context_id);
        } else {
            if(p_ctx->organization.initial_flag == 0U) {
                amba_ik_system_print_uint32_5("[IK][ERROR] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
                rval |= IK_ERR_0002;
            } else {
                amba_ik_system_print_str_5("[IK][ERROR] can't get context\n", NULL, NULL, NULL, NULL, NULL);
            }
        }
    }
    return rval;
}

uint32 img_ctx_ivd_get_wide_chroma_filter(uint32 context_id, ik_chroma_filter_t *p_chroma_filter)
{
    amba_ik_system_print_str_5("[IK] get_wide_chroma_filter is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //TBD
    }
    if (p_chroma_filter == NULL) {
        //TBD
    } else {
        (void)amba_ik_system_memcpy(p_chroma_filter, p_chroma_filter, sizeof(uint8));
    }
    return IK_ERR_0004;
}

uint32 img_ctx_get_wide_chroma_filter(uint32 context_id, ik_chroma_filter_t *p_chroma_filter)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval |= img_ctx_get_context(context_id, &p_ctx);
    if((p_chroma_filter != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        (void)amba_ik_system_lock_mutex(context_id, IK_WAIT_FOREVER);
        (void)amba_ik_system_memcpy(p_chroma_filter, &p_ctx->filters.input_param.wide_chroma_filter, sizeof(ik_chroma_filter_t));
        (void)amba_ik_system_unlock_mutex(context_id);
    } else {
        if(p_chroma_filter == NULL) {
            amba_ik_system_print_str_5("[IK][ERROR] NULL pointer in %s\n", __func__, NULL, NULL, NULL, NULL);
            rval = IK_ERR_0005;
        } else if(p_ctx->organization.initial_flag == 0U) {
            amba_ik_system_print_uint32_5("[IK][ERROR] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
            rval = IK_ERR_0002;
        } else {
            amba_ik_system_print_str_5("[IK][ERROR] can't get context\n", NULL, NULL, NULL, NULL, NULL);
        }
    }

    return rval;
}

uint32 img_ctx_ivd_set_wide_chroma_filter_combine(uint32 context_id, const ik_wide_chroma_filter_combine_t *p_chroma_filter_combine)
{
    amba_ik_system_print_str_5("[IK] set_wide_chroma_filter is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //TBD
    }
    if (p_chroma_filter_combine==NULL) {
        //TBD
    }
    return IK_ERR_0004;
}

uint32 img_ctx_set_wide_chroma_filter_combine(uint32 context_id, const ik_wide_chroma_filter_combine_t *p_chroma_filter_combine)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;
    const ik_debug_check_func_t* p_check_func = ik_ctx_get_debug_check_func();
    if(p_check_func != NULL) {
        rval = p_check_func->wide_chroma_filter_combine(p_chroma_filter_combine, "wide_chroma_filter_combine");
    }
    if(rval == IK_OK) {
        rval |= img_ctx_get_context(context_id, &p_ctx);
        if((p_chroma_filter_combine != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
            (void)amba_ik_system_lock_mutex(context_id, IK_WAIT_FOREVER);
            (void)amba_ik_system_memcpy(&p_ctx->filters.input_param.wide_chroma_combine, p_chroma_filter_combine, sizeof(ik_wide_chroma_filter_combine_t));
            ctx_raise_update_flag(&p_ctx->filters.update_flags.liso.wide_chroma_filter_combine);
            (void)amba_ik_system_unlock_mutex(context_id);
        } else {
            if(p_ctx->organization.initial_flag == 0U) {
                amba_ik_system_print_uint32_5("[IK][ERROR] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
                rval |= IK_ERR_0002;
            } else {
                amba_ik_system_print_str_5("[IK][ERROR] can't get context\n", NULL, NULL, NULL, NULL, NULL);
            }
        }
    }
    return rval;
}

uint32 img_ctx_ivd_get_wide_chroma_filter_combine(uint32 context_id, ik_wide_chroma_filter_combine_t *p_chroma_filter_combine)
{
    amba_ik_system_print_str_5("[IK] get_wide_chroma_filter is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //TBD
    }
    if (p_chroma_filter_combine == NULL) {
        //TBD
    } else {
        (void)amba_ik_system_memcpy(p_chroma_filter_combine, p_chroma_filter_combine, sizeof(uint8));
    }
    return IK_ERR_0004;
}

uint32 img_ctx_get_wide_chroma_filter_combine(uint32 context_id, ik_wide_chroma_filter_combine_t *p_chroma_filter_combine)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval |= img_ctx_get_context(context_id, &p_ctx);
    if((p_chroma_filter_combine != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        (void)amba_ik_system_lock_mutex(context_id, IK_WAIT_FOREVER);
        (void)amba_ik_system_memcpy(p_chroma_filter_combine, &p_ctx->filters.input_param.wide_chroma_combine, sizeof(ik_wide_chroma_filter_combine_t));
        (void)amba_ik_system_unlock_mutex(context_id);
    } else {
        if(p_chroma_filter_combine == NULL) {
            amba_ik_system_print_str_5("[IK][ERROR] NULL pointer in %s\n", __func__, NULL, NULL, NULL, NULL);
            rval = IK_ERR_0005;
        } else if(p_ctx->organization.initial_flag == 0U) {
            amba_ik_system_print_uint32_5("[IK][ERROR] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
            rval = IK_ERR_0002;
        } else {
            amba_ik_system_print_str_5("[IK][ERROR] can't get context\n", NULL, NULL, NULL, NULL, NULL);
        }
    }

    return rval;
}

uint32 img_ctx_ivd_set_mono_exp0_fe_static_blc(uint32 context_id, const ik_static_blc_level_t *p_exp0_frontend_static_blc)
{
    amba_ik_system_print_str_5("[IK] set_mono_exp0_fe_static_blc is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id == 0UL) {
        //TBD
    }
    if (p_exp0_frontend_static_blc == NULL) {
        //TBD
    }
    return IK_ERR_0004;
}

uint32 img_ctx_set_mono_exp0_fe_static_blc(uint32 context_id, const ik_static_blc_level_t *p_exp0_frontend_static_blc)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;
    const ik_debug_check_func_t* p_check_func = ik_ctx_get_debug_check_func();
    if(p_check_func != NULL) {
        rval = p_check_func->fe_blc(p_exp0_frontend_static_blc, 0UL, "mono_exp0_fe_static_blc");
    }

    if(rval == IK_OK) {
        rval |= img_ctx_get_context(context_id, &p_ctx);
        if ((rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
            (void)amba_ik_system_lock_mutex(context_id, IK_WAIT_FOREVER);
            (void)amba_ik_system_memcpy(&p_ctx->filters.input_param.mono_exp0_frontend_static_blc, p_exp0_frontend_static_blc, sizeof(ik_static_blc_level_t));
            ctx_raise_update_flag(&p_ctx->filters.update_flags.liso.mono_exp0_fe_static_blc_updated);
            (void)amba_ik_system_unlock_mutex(context_id);
        } else {
            if(p_ctx->organization.initial_flag == 0U) {
                amba_ik_system_print_uint32_5("[IK][ERROR] context_id = %d is not initial", context_id, DC_U, DC_U, DC_U, DC_U);
                rval = IK_ERR_0002;
            } else {
                amba_ik_system_print_str_5("[IK][ERROR] can't get context", NULL, NULL, NULL, NULL, NULL);
            }
        }
    }
    return rval;
}

uint32 img_ctx_ivd_get_mono_exp0_fe_static_blc(uint32 context_id, ik_static_blc_level_t *p_exp0_frontend_static_blc)
{
    amba_ik_system_print_str_5("[IK] get_mono_exp0_fe_static_blc is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id == 0UL) {
        //TBD
    }
    if (p_exp0_frontend_static_blc != NULL) {
        p_exp0_frontend_static_blc->black_b = p_exp0_frontend_static_blc->black_b;
    }
    return IK_ERR_0004;
}

uint32 img_ctx_get_mono_exp0_fe_static_blc(uint32 context_id, ik_static_blc_level_t *p_exp0_frontend_static_blc)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        (void)amba_ik_system_lock_mutex(context_id, IK_WAIT_FOREVER);
        (void)amba_ik_system_memcpy(p_exp0_frontend_static_blc, &p_ctx->filters.input_param.mono_exp0_frontend_static_blc, sizeof(ik_static_blc_level_t));
        (void)amba_ik_system_unlock_mutex(context_id);
    } else {
        if(p_ctx->organization.initial_flag == 0U) {
            amba_ik_system_print_uint32_5("[IK][ERROR] context_id = %d is not initial", context_id, DC_U, DC_U, DC_U, DC_U);
            rval = IK_ERR_0002;
        } else {
            amba_ik_system_print_str_5("[IK][ERROR] can't get context", NULL, NULL, NULL, NULL, NULL);
        }
    }

    return rval;
}

uint32 img_ctx_ivd_set_mono_exp0_fe_wb_gain(uint32 context_id, const ik_frontend_wb_gain_t *p_exp0_frontend_wb_gain)
{
    amba_ik_system_print_str_5("[IK] set_mono_exp0_fe_wb_gain is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id == 0UL) {
        //TBD
    }
    if (p_exp0_frontend_wb_gain == NULL) {
        //TBD
    }
    return IK_ERR_0004;
}

uint32 img_ctx_set_mono_exp0_fe_wb_gain(uint32 context_id, const ik_frontend_wb_gain_t *p_exp0_frontend_wb_gain)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;
    const ik_debug_check_func_t* p_check_func = ik_ctx_get_debug_check_func();
    if(p_check_func != NULL) {
        rval = p_check_func->fe_wb(p_exp0_frontend_wb_gain, 0UL, "mono_exp0_fe_wb_gain");
    }

    if(rval == IK_OK) {
        rval |= img_ctx_get_context(context_id, &p_ctx);
        if ((rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
            (void)amba_ik_system_lock_mutex(context_id, IK_WAIT_FOREVER);
            (void)amba_ik_system_memcpy(&p_ctx->filters.input_param.mono_exp0_frontend_wb_gain, p_exp0_frontend_wb_gain, sizeof(ik_frontend_wb_gain_t));
            ctx_raise_update_flag(&p_ctx->filters.update_flags.liso.mono_exp0_fe_wb_gain_updated);
            (void)amba_ik_system_unlock_mutex(context_id);
        } else {
            if(p_ctx->organization.initial_flag == 0U) {
                amba_ik_system_print_uint32_5("[IK][ERROR] context_id = %d is not initial", context_id, DC_U, DC_U, DC_U, DC_U);
                rval = IK_ERR_0002;
            } else {
                amba_ik_system_print_str_5("[IK][ERROR] can't get context", NULL, NULL, NULL, NULL, NULL);
            }
        }
    }
    return rval;
}

uint32 img_ctx_ivd_get_mono_exp0_fe_wb_gain(uint32 context_id, ik_frontend_wb_gain_t *p_exp0_frontend_wb_gain)
{
    amba_ik_system_print_str_5("[IK] get_mono_exp0_fe_wb_gain( is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id == 0UL) {
        //TBD
    }
    if (p_exp0_frontend_wb_gain != NULL) {
        p_exp0_frontend_wb_gain->b_gain = p_exp0_frontend_wb_gain->b_gain;
    }
    return IK_ERR_0004;
}

uint32 img_ctx_get_mono_exp0_fe_wb_gain(uint32 context_id, ik_frontend_wb_gain_t *p_exp0_frontend_wb_gain)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        (void)amba_ik_system_lock_mutex(context_id, IK_WAIT_FOREVER);
        (void)amba_ik_system_memcpy(p_exp0_frontend_wb_gain, &p_ctx->filters.input_param.mono_exp0_frontend_wb_gain, sizeof(ik_frontend_wb_gain_t));
        (void)amba_ik_system_unlock_mutex(context_id);
    } else {
        if(p_ctx->organization.initial_flag == 0U) {
            amba_ik_system_print_uint32_5("[IK][ERROR] context_id = %d is not initial", context_id, DC_U, DC_U, DC_U, DC_U);
            rval = IK_ERR_0002;
        } else {
            amba_ik_system_print_str_5("[IK][ERROR] can't get context", NULL, NULL, NULL, NULL, NULL);
        }
    }

    return rval;
}

uint32 img_ctx_ivd_set_mono_ce(uint32 context_id, const ik_ce_t *p_ce)
{
    amba_ik_system_print_str_5("[IK] set_mono_ce is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id == 0UL) {
        //TBD
    }
    if (p_ce == NULL) {
        //TBD
    }
    return IK_ERR_0004;
}

uint32 img_ctx_set_mono_ce(uint32 context_id, const ik_ce_t *p_ce)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;
    const ik_debug_check_func_t* p_check_func = ik_ctx_get_debug_check_func();
    if(p_check_func != NULL) {
        rval = p_check_func->ce(p_ce, "mono_ce");
    }
    if(rval == IK_OK) {
        rval |= img_ctx_get_context(context_id, &p_ctx);
        if ((rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
            (void)amba_ik_system_lock_mutex(context_id, IK_WAIT_FOREVER);
            (void)amba_ik_system_memcpy(&p_ctx->filters.input_param.mono_ce, p_ce, sizeof(ik_ce_t));
            ctx_raise_update_flag(&p_ctx->filters.update_flags.liso.mono_ce_updated);
            (void)amba_ik_system_unlock_mutex(context_id);
        } else {
            if(p_ctx->organization.initial_flag == 0U) {
                amba_ik_system_print_uint32_5("[IK][ERROR] context_id = %d is not initial", context_id, DC_U, DC_U, DC_U, DC_U);
                rval = IK_ERR_0002;
            } else {
                amba_ik_system_print_str_5("[IK][ERROR] can't get context", NULL, NULL, NULL, NULL, NULL);
            }
        }
    }
    return rval;
}

uint32 img_ctx_ivd_set_mono_ce_input_table(uint32 context_id, const ik_ce_input_table_t *p_ce_input_table)
{
    amba_ik_system_print_str_5("[IK] set_mono_ce_input_table is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id == 0UL) {
        //TBD
    }
    if (p_ce_input_table == NULL) {
        //TBD
    }
    return IK_ERR_0004;
}

uint32 img_ctx_set_mono_ce_input_table(uint32 context_id, const ik_ce_input_table_t *p_ce_input_table)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;
    const ik_debug_check_func_t* p_check_func = ik_ctx_get_debug_check_func();
    if(p_check_func != NULL) {
        rval = p_check_func->ce_input_tbl(p_ce_input_table, "mono_ce_input_table");
    }
    if(rval == IK_OK) {
        rval |= img_ctx_get_context(context_id, &p_ctx);
        if ((rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
            (void)amba_ik_system_lock_mutex(context_id, IK_WAIT_FOREVER);
            (void)amba_ik_system_memcpy(&p_ctx->filters.input_param.mono_ce_input_table, p_ce_input_table, sizeof(ik_ce_input_table_t));
            ctx_raise_update_flag(&p_ctx->filters.update_flags.liso.mono_ce_input_table_updated);
            (void)amba_ik_system_unlock_mutex(context_id);
        } else {
            if(p_ctx->organization.initial_flag == 0U) {
                amba_ik_system_print_uint32_5("[IK][ERROR] context_id = %d is not initial", context_id, DC_U, DC_U, DC_U, DC_U);
                rval = IK_ERR_0002;
            } else {
                amba_ik_system_print_str_5("[IK][ERROR] can't get context", NULL, NULL, NULL, NULL, NULL);
            }
        }
    }

    return rval;
}

uint32 img_ctx_ivd_set_mono_ce_out_table(uint32 context_id, const ik_ce_output_table_t *p_ce_out_table)
{
    amba_ik_system_print_str_5("[IK] set_mono_ce_out_table is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id == 0UL) {
        //TBD
    }
    if (p_ce_out_table == NULL) {
        //TBD
    }
    return IK_ERR_0004;
}

uint32 img_ctx_set_mono_ce_out_table(uint32 context_id, const ik_ce_output_table_t *p_ce_out_table)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;
    const ik_debug_check_func_t* p_check_func = ik_ctx_get_debug_check_func();
    if(p_check_func != NULL) {
        rval = p_check_func->ce_output_tbl(p_ce_out_table, "mono_ce_out_table");
    }
    if(rval == IK_OK) {
        rval |= img_ctx_get_context(context_id, &p_ctx);
        if ((rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
            (void)amba_ik_system_lock_mutex(context_id, IK_WAIT_FOREVER);
            (void)amba_ik_system_memcpy(&p_ctx->filters.input_param.mono_ce_out_table, p_ce_out_table, sizeof(ik_ce_output_table_t));
            ctx_raise_update_flag(&p_ctx->filters.update_flags.liso.mono_ce_out_table_updated);
            (void)amba_ik_system_unlock_mutex(context_id);
        } else {
            if(p_ctx->organization.initial_flag == 0U) {
                amba_ik_system_print_uint32_5("[IK][ERROR] context_id = %d is not initial", context_id, DC_U, DC_U, DC_U, DC_U);
                rval = IK_ERR_0002;
            } else {
                amba_ik_system_print_str_5("[IK][ERROR] can't get context", NULL, NULL, NULL, NULL, NULL);
            }
        }
    }

    return rval;
}

uint32 img_ctx_ivd_get_mono_ce(uint32 context_id, ik_ce_t *p_ce)
{
    amba_ik_system_print_str_5("[IK] get_mono_ce is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id == 0UL) {
        //TBD
    }
    if (p_ce != NULL) {
        p_ce->enable = p_ce->enable;
    }
    return IK_ERR_0004;
}

uint32 img_ctx_get_mono_ce(uint32 context_id, ik_ce_t *p_ce)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        (void)amba_ik_system_lock_mutex(context_id, IK_WAIT_FOREVER);
        (void)amba_ik_system_memcpy(p_ce, &p_ctx->filters.input_param.mono_ce, sizeof(ik_ce_t));
        (void)amba_ik_system_unlock_mutex(context_id);
    } else {
        if(p_ctx->organization.initial_flag == 0U) {
            amba_ik_system_print_uint32_5("[IK][ERROR] context_id = %d is not initial", context_id, DC_U, DC_U, DC_U, DC_U);
            rval = IK_ERR_0002;
        } else {
            amba_ik_system_print_str_5("[IK][ERROR] can't get context", NULL, NULL, NULL, NULL, NULL);
        }
    }

    return rval;
}

uint32 img_ctx_ivd_get_mono_ce_input_table(uint32 context_id, ik_ce_input_table_t *p_ce_input_table)
{
    amba_ik_system_print_str_5("[IK] get_mono_ce_input_table is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id == 0UL) {
        //TBD
    }
    if (p_ce_input_table != NULL) {
        p_ce_input_table->input_enable = p_ce_input_table->input_enable;
    }
    return IK_ERR_0004;
}

uint32 img_ctx_get_mono_ce_input_table(uint32 context_id, ik_ce_input_table_t *p_ce_input_table)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        (void)amba_ik_system_lock_mutex(context_id, IK_WAIT_FOREVER);
        (void)amba_ik_system_memcpy(p_ce_input_table, &p_ctx->filters.input_param.mono_ce_input_table, sizeof(ik_ce_input_table_t));
        (void)amba_ik_system_unlock_mutex(context_id);
    } else {
        if(p_ctx->organization.initial_flag == 0U) {
            amba_ik_system_print_uint32_5("[IK][ERROR] context_id = %d is not initial", context_id, DC_U, DC_U, DC_U, DC_U);
            rval = IK_ERR_0002;
        } else {
            amba_ik_system_print_str_5("[IK][ERROR] can't get context", NULL, NULL, NULL, NULL, NULL);
        }
    }

    return rval;
}

uint32 img_ctx_ivd_get_mono_ce_out_table(uint32 context_id, ik_ce_output_table_t *p_ce_out_table)
{
    amba_ik_system_print_str_5("[IK] get_mono_ce_out_table is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id == 0UL) {
        //TBD
    }
    if (p_ce_out_table != NULL) {
        p_ce_out_table->output_enable = p_ce_out_table->output_enable;
    }
    return IK_ERR_0004;
}

uint32 img_ctx_get_mono_ce_out_table(uint32 context_id, ik_ce_output_table_t *p_ce_out_table)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        (void)amba_ik_system_lock_mutex(context_id, IK_WAIT_FOREVER);
        (void)amba_ik_system_memcpy(p_ce_out_table, &p_ctx->filters.input_param.mono_ce_out_table, sizeof(ik_ce_output_table_t));
        (void)amba_ik_system_unlock_mutex(context_id);
    } else {
        if(p_ctx->organization.initial_flag == 0U) {
            amba_ik_system_print_uint32_5("[IK][ERROR] context_id = %d is not initial", context_id, DC_U, DC_U, DC_U, DC_U);
            rval = IK_ERR_0002;
        } else {
            amba_ik_system_print_str_5("[IK][ERROR] can't get context", NULL, NULL, NULL, NULL, NULL);
        }
    }

    return rval;
}

uint32 img_ctx_ivd_set_mono_grgb_mismatch(uint32 context_id, const ik_grgb_mismatch_t *p_grgb_mismatch)
{
    uint32 rval = IK_ERR_0004;
    amba_ik_system_print_str_5("[IK] set_mono_grgb_mismatch is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id > img_arch_get_context_number()) {
        // misraC
    }
    if (p_grgb_mismatch==NULL) {
        // misraC
    }
    return rval;
}

uint32 img_ctx_set_mono_grgb_mismatch(uint32 context_id, const ik_grgb_mismatch_t *p_grgb_mismatch)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;
    const ik_debug_check_func_t* p_check_func = ik_ctx_get_debug_check_func();
    if(p_check_func != NULL) {
        rval = p_check_func->grgb_mismatch(p_grgb_mismatch, "mono_grgb_mismatch");
    }
    if(rval == IK_OK) {
        rval |= img_ctx_get_context(context_id, &p_ctx);
        if((p_grgb_mismatch != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
            (void)amba_ik_system_lock_mutex(context_id, IK_WAIT_FOREVER);
            (void)amba_ik_system_memcpy(&p_ctx->filters.input_param.mono_grgb_mismatch, p_grgb_mismatch, sizeof(ik_grgb_mismatch_t));
            ctx_raise_update_flag(&p_ctx->filters.update_flags.liso.mono_grgb_mismatch_updated);
            (void)amba_ik_system_unlock_mutex(context_id);
        } else {
            if(p_ctx->organization.initial_flag == 0U) {
                amba_ik_system_print_uint32_5("[IK][ERROR] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
                rval = IK_ERR_0002;
            } else {
                amba_ik_system_print_str_5("[IK][ERROR] can't get context\n", NULL, NULL, NULL, NULL, NULL);
            }
        }
    }
    return rval;
}

uint32 img_ctx_ivd_get_mono_grgb_mismatch(uint32 context_id, ik_grgb_mismatch_t *p_grgb_mismatch)
{
    amba_ik_system_print_str_5("[IK] get_mono_grgb_mismatch() is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //TBD
    }
    if (p_grgb_mismatch == NULL) {
        //TBD
    } else {
        (void)amba_ik_system_memcpy(p_grgb_mismatch, p_grgb_mismatch, sizeof(uint8));
    }
    return IK_ERR_0004;
}

uint32 img_ctx_get_mono_grgb_mismatch(uint32 context_id, ik_grgb_mismatch_t *p_grgb_mismatch)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval |= img_ctx_get_context(context_id, &p_ctx);
    if((p_grgb_mismatch != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        (void)amba_ik_system_lock_mutex(context_id, IK_WAIT_FOREVER);
        (void)amba_ik_system_memcpy(p_grgb_mismatch, &p_ctx->filters.input_param.mono_grgb_mismatch, sizeof(ik_grgb_mismatch_t));
        (void)amba_ik_system_unlock_mutex(context_id);
    } else {
        if(p_grgb_mismatch == NULL) {
            amba_ik_system_print_str_5("[IK][ERROR] NULL pointer in %s\n", __func__, NULL, NULL, NULL, NULL);
            rval = IK_ERR_0005;
        } else if(p_ctx->organization.initial_flag == 0U) {
            amba_ik_system_print_uint32_5("[IK][ERROR] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
            rval = IK_ERR_0002;
        } else {
            amba_ik_system_print_str_5("[IK][ERROR] can't get context\n", NULL, NULL, NULL, NULL, NULL);
        }
    }

    return rval;
}

#endif
