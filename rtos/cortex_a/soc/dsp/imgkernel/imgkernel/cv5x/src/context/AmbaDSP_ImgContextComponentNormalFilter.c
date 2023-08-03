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

uint32 img_ctx_ivd_set_vin_sensor_info(uint32 context_id, const ik_vin_sensor_info_t *p_sensor_info)
{
    amba_ik_system_print_str_5("[IK] set_vin_sensor_info is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0U) {
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
        rval = p_check_func->vin_sensor_info(p_sensor_info);
    }
    if(rval == IK_OK) {
        rval |= img_ctx_get_context(context_id, &p_ctx);
        if(p_ctx->filters.update_flags.iso.is_1st_frame == 0U) {
            amba_ik_system_print_uint32_5("[IK][ERROR] Invalid ik_vin_sensor_info_t run-time update, context_id = %d\n", context_id, DC_U, DC_U, DC_U, DC_U);
            rval |= IK_ERR_0008;
        } else {
            (void)amba_ik_system_lock_mutex(&p_ctx->organization.mutex, IK_WAIT_FOREVER);
            (void)amba_ik_system_memcpy(&p_ctx->filters.input_param.sensor_info, p_sensor_info, sizeof(ik_vin_sensor_info_t));
            ctx_raise_update_flag(&p_ctx->filters.update_flags.iso.sensor_information_updated);
            (void)amba_ik_system_unlock_mutex(&p_ctx->organization.mutex);
        }
    }
    return rval;
}

uint32 img_ctx_ivd_set_yuv_mode(const uint32 context_id, const uint32 yuv_mode)
{
    amba_ik_system_print_str_5("[IK] set_yuv_mode is invalid!", NULL, NULL, NULL, NULL, NULL);
    if ((context_id != 0U) || (yuv_mode != 0xFFFFFFFFU)) {
        //MISRA
    }
    return IK_ERR_0004;
}

uint32 img_ctx_set_yuv_mode(const uint32 context_id, const uint32 yuv_mode)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;
    if(yuv_mode > 1U) {
        amba_ik_system_print_str_5("[IK][ERROR] set_yuv_mode() yuv_mode > 1 !", DC_S, DC_S, DC_S, DC_S, DC_S);
        rval |= IK_ERR_0008;
    }
    if(rval == IK_OK) {
        rval |= img_ctx_get_context(context_id, &p_ctx);
        if((rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
            (void)amba_ik_system_lock_mutex(&p_ctx->organization.mutex, IK_WAIT_FOREVER);
            (void)amba_ik_system_memcpy(&p_ctx->filters.input_param.yuv_mode, &yuv_mode, sizeof(uint32));
            //ctx_raise_update_flag(&p_ctx->filters.update_flags.iso.yuv_mode_updated);
            (void)amba_ik_system_unlock_mutex(&p_ctx->organization.mutex);
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

uint32 img_ctx_ivd_set_before_ce_wb_gain(uint32 context_id, const ik_wb_gain_t *p_before_ce_wb_gain)
{
    amba_ik_system_print_str_5("[IK] set_before_ce_wb_gain is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0U) {
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
        rval = p_check_func->before_ce_wb(p_before_ce_wb_gain);
    }
    if(rval == IK_OK) {
        rval |= img_ctx_get_context(context_id, &p_ctx);
        if((p_before_ce_wb_gain != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
            (void)amba_ik_system_lock_mutex(&p_ctx->organization.mutex, IK_WAIT_FOREVER);
            (void)amba_ik_system_memcpy(&p_ctx->filters.input_param.before_ce_wb_gain, p_before_ce_wb_gain, sizeof(ik_wb_gain_t));
            ctx_raise_update_flag(&p_ctx->filters.update_flags.iso.before_ce_wb_gain_updated);
            (void)amba_ik_system_unlock_mutex(&p_ctx->organization.mutex);
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
    if (context_id != 0U) {
        //MISRA
    }
    if (p_pre_cc_gain == NULL) {
        //MISRA
    }
    return IK_ERR_0004;
}

uint32 img_ctx_set_pre_cc_gain(uint32 context_id, const ik_pre_cc_gain_t *p_pre_cc_gain)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;
    const ik_debug_check_func_t* p_check_func = ik_ctx_get_debug_check_func();
    if(p_check_func != NULL) {
        rval = p_check_func->pre_cc_gain(p_pre_cc_gain);
    }
    if(rval == IK_OK) {
        rval |= img_ctx_get_context(context_id, &p_ctx);
        if((p_pre_cc_gain != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
            (void)amba_ik_system_lock_mutex(&p_ctx->organization.mutex, IK_WAIT_FOREVER);
            (void)amba_ik_system_memcpy(&p_ctx->filters.input_param.pre_cc_gain, p_pre_cc_gain, sizeof(ik_pre_cc_gain_t));
            ctx_raise_update_flag(&p_ctx->filters.update_flags.iso.pre_cc_gain_updated);
            (void)amba_ik_system_unlock_mutex(&p_ctx->organization.mutex);
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
    if (context_id != 0U) {
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
        rval = p_check_func->after_ce_wb(p_after_ce_wb_gain);
    }
    if(rval == IK_OK) {
        rval |= img_ctx_get_context(context_id, &p_ctx);
        if((p_after_ce_wb_gain != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
            (void)amba_ik_system_lock_mutex(&p_ctx->organization.mutex, IK_WAIT_FOREVER);
            (void)amba_ik_system_memcpy(&p_ctx->filters.input_param.after_ce_wb_gain, p_after_ce_wb_gain, sizeof(ik_wb_gain_t));
            ctx_raise_update_flag(&p_ctx->filters.update_flags.iso.after_ce_wb_gain_updated);
            (void)amba_ik_system_unlock_mutex(&p_ctx->organization.mutex);
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

uint32 img_ctx_ivd_set_stored_ir(uint32 context_id, const ik_stored_ir_t *p_stored_ir)
{
    amba_ik_system_print_str_5("[IK] set_stored_ir is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0U) {
        //MISRA
    }
    if (p_stored_ir == NULL) {
        //MISRA
    }
    return IK_ERR_0004;
}

uint32 img_ctx_set_stored_ir(uint32 context_id, const ik_stored_ir_t *p_stored_ir)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;
    const ik_debug_check_func_t* p_check_func = ik_ctx_get_debug_check_func();
    if(p_check_func != NULL) {
        rval = p_check_func->stored_ir(p_stored_ir);
    }
    if(rval == IK_OK) {
        rval |= img_ctx_get_context(context_id, &p_ctx);
        if((p_stored_ir != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
            (void)amba_ik_system_lock_mutex(&p_ctx->organization.mutex, IK_WAIT_FOREVER);
            (void)amba_ik_system_memcpy(&p_ctx->filters.input_param.stored_ir, p_stored_ir, sizeof(ik_stored_ir_t));
            ctx_raise_update_flag(&p_ctx->filters.update_flags.iso.stored_ir_updated);
            (void)amba_ik_system_unlock_mutex(&p_ctx->organization.mutex);
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
    if (context_id != 0U) {
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
        rval = p_check_func->cfa_leakage(p_cfa_leakage_filter);
    }

    if(rval == IK_OK) {
        rval |= img_ctx_get_context(context_id, &p_ctx);
        if((p_cfa_leakage_filter != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
            (void)amba_ik_system_lock_mutex(&p_ctx->organization.mutex, IK_WAIT_FOREVER);
            (void)amba_ik_system_memcpy(&p_ctx->filters.input_param.cfa_leakage_filter, p_cfa_leakage_filter, sizeof(ik_cfa_leakage_filter_t));
            ctx_raise_update_flag(&p_ctx->filters.update_flags.iso.cfa_leakage_filter_updated);
            (void)amba_ik_system_unlock_mutex(&p_ctx->organization.mutex);
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
    if (context_id != 0U) {
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
        rval = p_check_func->anti_aliasing(p_anti_aliasing);
    }
    if(rval == IK_OK) {
        rval |= img_ctx_get_context(context_id, &p_ctx);
        if((p_anti_aliasing != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
            (void)amba_ik_system_lock_mutex(&p_ctx->organization.mutex, IK_WAIT_FOREVER);
            (void)amba_ik_system_memcpy(&p_ctx->filters.input_param.anti_aliasing, p_anti_aliasing, sizeof(ik_anti_aliasing_t));
            ctx_raise_update_flag(&p_ctx->filters.update_flags.iso.anti_aliasing_updated);
            (void)amba_ik_system_unlock_mutex(&p_ctx->organization.mutex);
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
    if (context_id != 0U) {
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
        rval = p_check_func->dynamic_bad_pxl_cor(p_dynamic_bpc);
    }
    if(rval == IK_OK) {
        rval |= img_ctx_get_context(context_id, &p_ctx);
        if((p_dynamic_bpc != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
            (void)amba_ik_system_lock_mutex(&p_ctx->organization.mutex, IK_WAIT_FOREVER);
            (void)amba_ik_system_memcpy(&p_ctx->filters.input_param.dynamic_bpc, p_dynamic_bpc, sizeof(ik_dynamic_bad_pixel_correction_t));
            ctx_raise_update_flag(&p_ctx->filters.update_flags.iso.dynamic_bpc_updated);
            (void)amba_ik_system_unlock_mutex(&p_ctx->organization.mutex);
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
    if (context_id != 0U) {
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
        rval = p_check_func->cfa_noise_filter(p_cfa_noise_filter);
    }
    if(rval == IK_OK) {
        rval |= img_ctx_get_context(context_id, &p_ctx);
        if((p_cfa_noise_filter != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
            (void)amba_ik_system_lock_mutex(&p_ctx->organization.mutex, IK_WAIT_FOREVER);
            (void)amba_ik_system_memcpy(&p_ctx->filters.input_param.cfa_noise_filter, p_cfa_noise_filter, sizeof(ik_cfa_noise_filter_t));
            ctx_raise_update_flag(&p_ctx->filters.update_flags.iso.cfa_noise_filter_updated);
            (void)amba_ik_system_unlock_mutex(&p_ctx->organization.mutex);
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
    if (context_id != 0U) {
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
        rval = p_check_func->demosaic(p_demosaic);
    }
    if(rval == IK_OK) {
        rval |= img_ctx_get_context(context_id, &p_ctx);
        if((p_demosaic != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
            (void)amba_ik_system_lock_mutex(&p_ctx->organization.mutex, IK_WAIT_FOREVER);
            (void)amba_ik_system_memcpy(&p_ctx->filters.input_param.demosaic, p_demosaic, sizeof(ik_demosaic_t));
            ctx_raise_update_flag(&p_ctx->filters.update_flags.iso.demosaic_updated);
            (void)amba_ik_system_unlock_mutex(&p_ctx->organization.mutex);
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

uint32 img_ctx_ivd_set_ext_raw_out_mode(uint32 context_id, const uint32 ext_raw_out_mode)
{
    amba_ik_system_print_str_5("[IK] compr_mode is invalid!", NULL, NULL, NULL, NULL, NULL);
    if ((context_id != 0UL) || (ext_raw_out_mode != 0xFFFFFFFFUL)) {
        //MISRA
    }
    return IK_ERR_0004;
}

uint32 img_ctx_set_ext_raw_out_mode(uint32 context_id, const uint32 ext_raw_out_mode)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;
    const ik_debug_check_func_t* p_check_func = ik_ctx_get_debug_check_func();
    if(p_check_func != NULL) {
        rval = p_check_func->ext_raw_out_mode(ext_raw_out_mode);
    }
    if(rval == IK_OK) {
        rval |= img_ctx_get_context(context_id, &p_ctx);
        (void)amba_ik_system_lock_mutex(&p_ctx->organization.mutex, IK_WAIT_FOREVER);
        (void)amba_ik_system_memcpy(&p_ctx->filters.input_param.ext_raw_out_mode, &ext_raw_out_mode, sizeof(uint32));
        ctx_raise_update_flag(&p_ctx->filters.update_flags.iso.ext_raw_out_mode_updated);
        (void)amba_ik_system_unlock_mutex(&p_ctx->organization.mutex);
    }
    return rval;
}

uint32 img_ctx_ivd_set_rgb_to_12y(uint32 context_id, const ik_rgb_to_12y_t *p_rgb_to_12y)
{
    amba_ik_system_print_str_5("[IK] set_rgb_to_12y is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0U) {
        //TBD
    }
    if (p_rgb_to_12y == NULL) {
        //TBD
    }
    return IK_ERR_0004;
}

uint32 img_ctx_set_rgb_to_12y(uint32 context_id, const ik_rgb_to_12y_t *p_rgb_to_12y)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;
    const ik_debug_check_func_t* p_check_func = ik_ctx_get_debug_check_func();
    if(p_check_func != NULL) {
        rval = p_check_func->rgb_to_12y(p_rgb_to_12y);
    }
    if(rval == IK_OK) {
        rval |= img_ctx_get_context(context_id, &p_ctx);
        if((p_rgb_to_12y != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
            (void)amba_ik_system_lock_mutex(&p_ctx->organization.mutex, IK_WAIT_FOREVER);
            (void)amba_ik_system_memcpy(&p_ctx->filters.input_param.rgb_to_12y, p_rgb_to_12y, sizeof(ik_rgb_to_12y_t));
            ctx_raise_update_flag(&p_ctx->filters.update_flags.iso.rgb_to_12y_updated);
            (void)amba_ik_system_unlock_mutex(&p_ctx->organization.mutex);
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

uint32 img_ctx_ivd_set_luma_noise_reduction(uint32 context_id, const ik_luma_noise_reduction_t *p_luma_noise_reduce)
{
    amba_ik_system_print_str_5("[IK] set_luma_noise_reduction is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0U) {
        //TBD
    }
    if (p_luma_noise_reduce == NULL) {
        //TBD
    }
    return IK_ERR_0004;
}

uint32 img_ctx_set_luma_noise_reduction(uint32 context_id, const ik_luma_noise_reduction_t *p_luma_noise_reduce)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;
    const ik_debug_check_func_t* p_check_func = ik_ctx_get_debug_check_func();
    if(p_check_func != NULL) {
        rval = p_check_func->luma_noise_reduce(p_luma_noise_reduce);
    }
    if(rval == IK_OK) {
        rval |= img_ctx_get_context(context_id, &p_ctx);
        if((p_luma_noise_reduce != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
            (void)amba_ik_system_lock_mutex(&p_ctx->organization.mutex, IK_WAIT_FOREVER);
            (void)amba_ik_system_memcpy(&p_ctx->filters.input_param.luma_noise_reduce, p_luma_noise_reduce, sizeof(ik_luma_noise_reduction_t));
            ctx_raise_update_flag(&p_ctx->filters.update_flags.iso.lnl_updated);
            (void)amba_ik_system_unlock_mutex(&p_ctx->organization.mutex);
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

uint32 img_ctx_ivd_set_hi_luma_noise_reduction(uint32 context_id, const ik_luma_noise_reduction_t *p_hi_luma_noise_reduce)
{
    amba_ik_system_print_str_5("[IK] set_hi_luma_noise_reduction is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0U) {
        //TBD
    }
    if (p_hi_luma_noise_reduce == NULL) {
        //TBD
    }
    return IK_ERR_0004;
}

uint32 img_ctx_set_hi_luma_noise_reduction(uint32 context_id, const ik_luma_noise_reduction_t *p_hi_luma_noise_reduce)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;
    const ik_debug_check_func_t* p_check_func = ik_ctx_get_debug_check_func();
    if(p_check_func != NULL) {
        rval = p_check_func->luma_noise_reduce(p_hi_luma_noise_reduce);
    }
    if(rval == IK_OK) {
        rval |= img_ctx_get_context(context_id, &p_ctx);
        if((p_hi_luma_noise_reduce != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
            (void)amba_ik_system_lock_mutex(&p_ctx->organization.mutex, IK_WAIT_FOREVER);
            (void)amba_ik_system_memcpy(&p_ctx->filters.input_param.hi_luma_noise_reduce, p_hi_luma_noise_reduce, sizeof(ik_luma_noise_reduction_t));
            ctx_raise_update_flag(&p_ctx->filters.update_flags.iso.hi_lnl_updated);
            (void)amba_ik_system_unlock_mutex(&p_ctx->organization.mutex);
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

uint32 img_ctx_ivd_set_li2_luma_noise_reduction(uint32 context_id, const ik_luma_noise_reduction_t *p_li2_luma_noise_reduce)
{
    amba_ik_system_print_str_5("[IK] set_li_luma_noise_reduction is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0U) {
        //TBD
    }
    if (p_li2_luma_noise_reduce == NULL) {
        //TBD
    }
    return IK_ERR_0004;
}

uint32 img_ctx_set_li2_luma_noise_reduction(uint32 context_id, const ik_luma_noise_reduction_t *p_li2_luma_noise_reduce)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;
    const ik_debug_check_func_t* p_check_func = ik_ctx_get_debug_check_func();
    if(p_check_func != NULL) {
        rval = p_check_func->luma_noise_reduce(p_li2_luma_noise_reduce);
    }
    if(rval == IK_OK) {
        rval |= img_ctx_get_context(context_id, &p_ctx);
        if((p_li2_luma_noise_reduce != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
            (void)amba_ik_system_lock_mutex(&p_ctx->organization.mutex, IK_WAIT_FOREVER);
            (void)amba_ik_system_memcpy(&p_ctx->filters.input_param.li2_luma_noise_reduce, p_li2_luma_noise_reduce, sizeof(ik_luma_noise_reduction_t));
            ctx_raise_update_flag(&p_ctx->filters.update_flags.iso.li2_lnl_updated);
            (void)amba_ik_system_unlock_mutex(&p_ctx->organization.mutex);
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
    if (context_id != 0U) {
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
        rval = p_check_func->color_correction(p_color_correction);
    }
    if(rval == IK_OK) {
        rval |= img_ctx_get_context(context_id, &p_ctx);
        if((p_color_correction != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
            (void)amba_ik_system_lock_mutex(&p_ctx->organization.mutex, IK_WAIT_FOREVER);
            (void)amba_ik_system_memcpy(&p_ctx->filters.input_param.color_correction, p_color_correction, sizeof(ik_color_correction_t));
            p_ctx->filters.input_param.use_cc = 1U;
            ctx_raise_update_flag(&p_ctx->filters.update_flags.iso.color_correction_updated);
            (void)amba_ik_system_unlock_mutex(&p_ctx->organization.mutex);
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
    if (context_id != 0U) {
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
        rval = p_check_func->tone_curve(p_tone_curve);
    }
    if(rval == IK_OK) {
        rval |= img_ctx_get_context(context_id, &p_ctx);
        if((p_tone_curve != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
            (void)amba_ik_system_lock_mutex(&p_ctx->organization.mutex, IK_WAIT_FOREVER);
            (void)amba_ik_system_memcpy(&p_ctx->filters.input_param.tone_curve, p_tone_curve, sizeof(ik_tone_curve_t));
            p_ctx->filters.input_param.use_tone_curve = 1U;
            ctx_raise_update_flag(&p_ctx->filters.update_flags.iso.tone_curve_updated);
            (void)amba_ik_system_unlock_mutex(&p_ctx->organization.mutex);
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
    if (context_id != 0U) {
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
        rval = p_check_func->rgb_to_yuv_matrix(p_rgb_to_yuv_matrix);
    }
    if(rval == IK_OK) {
        rval |= img_ctx_get_context(context_id, &p_ctx);
        if((p_rgb_to_yuv_matrix != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
            (void)amba_ik_system_lock_mutex(&p_ctx->organization.mutex, IK_WAIT_FOREVER);
            (void)amba_ik_system_memcpy(&p_ctx->filters.input_param.rgb_to_yuv_matrix, p_rgb_to_yuv_matrix, sizeof(ik_rgb_to_yuv_matrix_t));
            ctx_raise_update_flag(&p_ctx->filters.update_flags.iso.rgb_to_yuv_matrix_updated);
            (void)amba_ik_system_unlock_mutex(&p_ctx->organization.mutex);
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
    if (context_id != 0U) {
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
        rval = p_check_func->rgb_ir(p_rgb_ir);
    }
    if(rval == IK_OK) {
        rval |= img_ctx_get_context(context_id, &p_ctx);
        if((p_rgb_ir != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
            (void)amba_ik_system_lock_mutex(&p_ctx->organization.mutex, IK_WAIT_FOREVER);
            (void)amba_ik_system_memcpy(&p_ctx->filters.input_param.rgb_ir, p_rgb_ir, sizeof(ik_rgb_ir_t));
            ctx_raise_update_flag(&p_ctx->filters.update_flags.iso.rgb_ir_updated);
            (void)amba_ik_system_unlock_mutex(&p_ctx->organization.mutex);
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
    if (context_id != 0U) {
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
        rval = p_check_func->chroma_sacle(p_chroma_scale);
    }
    if(rval == IK_OK) {
        rval |= img_ctx_get_context(context_id, &p_ctx);
        if((p_chroma_scale != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
            (void)amba_ik_system_lock_mutex(&p_ctx->organization.mutex, IK_WAIT_FOREVER);
            (void)amba_ik_system_memcpy(&p_ctx->filters.input_param.chroma_scale, p_chroma_scale, sizeof(ik_chroma_scale_t));
            ctx_raise_update_flag(&p_ctx->filters.update_flags.iso.chroma_scale_updated);
            (void)amba_ik_system_unlock_mutex(&p_ctx->organization.mutex);
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
    if (context_id != 0U) {
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
        rval = p_check_func->chroma_median_filter(p_chroma_median_filter);
    }
    if(rval == IK_OK) {
        rval |= img_ctx_get_context(context_id, &p_ctx);
        if((p_chroma_median_filter != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
            (void)amba_ik_system_lock_mutex(&p_ctx->organization.mutex, IK_WAIT_FOREVER);
            (void)amba_ik_system_memcpy(&p_ctx->filters.input_param.chroma_median_filter, p_chroma_median_filter, sizeof(ik_chroma_median_filter_t));
            ctx_raise_update_flag(&p_ctx->filters.update_flags.iso.chroma_median_filter_updated);
            (void)amba_ik_system_unlock_mutex(&p_ctx->organization.mutex);
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
    if (context_id != 0U) {
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
        rval = p_check_func->first_luma_proc_mode(p_first_luma_process_mode);
    }
    if(rval == IK_OK) {
        rval |= img_ctx_get_context(context_id, &p_ctx);
        if((p_first_luma_process_mode != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
            (void)amba_ik_system_lock_mutex(&p_ctx->organization.mutex, IK_WAIT_FOREVER);
            (void)amba_ik_system_memcpy(&p_ctx->filters.input_param.first_luma_process_mode, p_first_luma_process_mode, sizeof(ik_first_luma_process_mode_t));
            ctx_raise_update_flag(&p_ctx->filters.update_flags.iso.first_luma_processing_mode_updated);
            (void)amba_ik_system_unlock_mutex(&p_ctx->organization.mutex);
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
    if (context_id != 0U) {
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
        rval = p_check_func->adv_spat_fltr(p_advance_spatial_filter);
    }
    if(rval == IK_OK) {
        rval |= img_ctx_get_context(context_id, &p_ctx);
        if((p_advance_spatial_filter != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
            (void)amba_ik_system_lock_mutex(&p_ctx->organization.mutex, IK_WAIT_FOREVER);
            (void)amba_ik_system_memcpy(&p_ctx->filters.input_param.advance_spatial_filter, p_advance_spatial_filter, sizeof(ik_adv_spatial_filter_t));
            ctx_raise_update_flag(&p_ctx->filters.update_flags.iso.advance_spatial_filter_updated);
            (void)amba_ik_system_unlock_mutex(&p_ctx->organization.mutex);
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
    if (context_id != 0U) {
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
        rval = p_check_func->first_sharpen_both(p_first_sharpen_both);
    }
    if(rval == IK_OK) {
        rval |= img_ctx_get_context(context_id, &p_ctx);
        if((p_first_sharpen_both != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
            (void)amba_ik_system_lock_mutex(&p_ctx->organization.mutex, IK_WAIT_FOREVER);
            (void)amba_ik_system_memcpy(&p_ctx->filters.input_param.first_sharpen_both, p_first_sharpen_both, sizeof(ik_first_sharpen_both_t));
            ctx_raise_update_flag(&p_ctx->filters.update_flags.iso.fstshpns_both_updated);
            (void)amba_ik_system_unlock_mutex(&p_ctx->organization.mutex);
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
    if (context_id != 0U) {
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
        rval = p_check_func->first_sharpen_noise(p_first_sharpen_noise);
    }
    if(rval == IK_OK) {
        rval |= img_ctx_get_context(context_id, &p_ctx);
        if((p_first_sharpen_noise != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
            (void)amba_ik_system_lock_mutex(&p_ctx->organization.mutex, IK_WAIT_FOREVER);
            (void)amba_ik_system_memcpy(&p_ctx->filters.input_param.first_sharpen_noise, p_first_sharpen_noise, sizeof(ik_first_sharpen_noise_t));
            ctx_raise_update_flag(&p_ctx->filters.update_flags.iso.fstshpns_noise_updated);
            (void)amba_ik_system_unlock_mutex(&p_ctx->organization.mutex);
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
    if (context_id != 0U) {
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
        rval = p_check_func->first_sharpen_fir(p_first_sharpen_fir);
    }
    if(rval == IK_OK) {
        rval |= img_ctx_get_context(context_id, &p_ctx);
        if((p_first_sharpen_fir != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
            (void)amba_ik_system_lock_mutex(&p_ctx->organization.mutex, IK_WAIT_FOREVER);
            (void)amba_ik_system_memcpy(&p_ctx->filters.input_param.first_sharpen_fir, p_first_sharpen_fir, sizeof(ik_first_sharpen_fir_t));
            ctx_raise_update_flag(&p_ctx->filters.update_flags.iso.fstshpns_fir_updated);
            (void)amba_ik_system_unlock_mutex(&p_ctx->organization.mutex);
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
    if (context_id != 0U) {
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
        rval = p_check_func->first_sharpen_coring(p_first_sharpen_coring);
    }
    if(rval == IK_OK) {
        rval |= img_ctx_get_context(context_id, &p_ctx);
        if((p_first_sharpen_coring != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
            (void)amba_ik_system_lock_mutex(&p_ctx->organization.mutex, IK_WAIT_FOREVER);
            (void)amba_ik_system_memcpy(&p_ctx->filters.input_param.first_sharpen_coring, p_first_sharpen_coring, sizeof(ik_first_sharpen_coring_t));
            ctx_raise_update_flag(&p_ctx->filters.update_flags.iso.fstshpns_coring_updated);
            (void)amba_ik_system_unlock_mutex(&p_ctx->organization.mutex);
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
    if (context_id != 0U) {
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
        rval = p_check_func->fstshpns_cor_idx_scl(p_first_sharpen_coring_idx_scale);
    }
    if(rval == IK_OK) {
        rval |= img_ctx_get_context(context_id, &p_ctx);
        if((p_first_sharpen_coring_idx_scale != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
            (void)amba_ik_system_lock_mutex(&p_ctx->organization.mutex, IK_WAIT_FOREVER);
            (void)amba_ik_system_memcpy(&p_ctx->filters.input_param.first_sharpen_coring_idx_scale, p_first_sharpen_coring_idx_scale, sizeof(ik_first_sharpen_coring_idx_scale_t));
            ctx_raise_update_flag(&p_ctx->filters.update_flags.iso.fstshpns_coring_index_scale_updated);
            (void)amba_ik_system_unlock_mutex(&p_ctx->organization.mutex);
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
    if (context_id != 0U) {
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
        rval = p_check_func->fstshpns_min_cor_rst(p_fstshpns_min_coring_result);
    }
    if(rval == IK_OK) {
        rval |= img_ctx_get_context(context_id, &p_ctx);
        if((p_fstshpns_min_coring_result != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
            (void)amba_ik_system_lock_mutex(&p_ctx->organization.mutex, IK_WAIT_FOREVER);
            (void)amba_ik_system_memcpy(&p_ctx->filters.input_param.first_sharpen_min_coring_result, p_fstshpns_min_coring_result, sizeof(ik_first_sharpen_min_coring_result_t));
            ctx_raise_update_flag(&p_ctx->filters.update_flags.iso.fstshpns_min_coring_result_updated);
            (void)amba_ik_system_unlock_mutex(&p_ctx->organization.mutex);
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
    if (context_id != 0U) {
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
        rval = p_check_func->fstshpns_max_cor_rst(p_fstshpns_max_coring_result);
    }
    if(rval == IK_OK) {
        rval |= img_ctx_get_context(context_id, &p_ctx);
        if((p_fstshpns_max_coring_result != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
            (void)amba_ik_system_lock_mutex(&p_ctx->organization.mutex, IK_WAIT_FOREVER);
            (void)amba_ik_system_memcpy(&p_ctx->filters.input_param.first_sharpen_max_coring_result, p_fstshpns_max_coring_result, sizeof(ik_first_sharpen_max_coring_result_t));
            ctx_raise_update_flag(&p_ctx->filters.update_flags.iso.fstshpns_max_coring_result_updated);
            (void)amba_ik_system_unlock_mutex(&p_ctx->organization.mutex);
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
    if (context_id != 0U) {
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
        rval = p_check_func->fstshpns_scl_cor(p_fstshpns_scale_coring);
    }
    if(rval == IK_OK) {
        rval |= img_ctx_get_context(context_id, &p_ctx);
        if((p_fstshpns_scale_coring != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
            (void)amba_ik_system_lock_mutex(&p_ctx->organization.mutex, IK_WAIT_FOREVER);
            (void)amba_ik_system_memcpy(&p_ctx->filters.input_param.first_sharpen_scale_coring, p_fstshpns_scale_coring, sizeof(ik_first_sharpen_scale_coring_t));
            ctx_raise_update_flag(&p_ctx->filters.update_flags.iso.fstshpns_scale_coring_updated);
            (void)amba_ik_system_unlock_mutex(&p_ctx->organization.mutex);
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
    if (context_id != 0U) {
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
            rval = p_check_func->final_sharpen_both(p_final_sharpen_both);
        }
        if(rval == IK_OK) {
            (void)amba_ik_system_lock_mutex(&p_ctx->organization.mutex, IK_WAIT_FOREVER);
            (void)amba_ik_system_memcpy(&p_ctx->filters.input_param.final_sharpen_both, p_final_sharpen_both, sizeof(ik_final_sharpen_both_t));
            ctx_raise_update_flag(&p_ctx->filters.update_flags.iso.fnlshpns_both_updated);
            (void)amba_ik_system_unlock_mutex(&p_ctx->organization.mutex);
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
    if (context_id != 0U) {
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
        rval = p_check_func->final_sharpen_noise(p_final_sharpen_noise);
    }
    if(rval == IK_OK) {
        rval |= img_ctx_get_context(context_id, &p_ctx);
        if((p_final_sharpen_noise != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
            (void)amba_ik_system_lock_mutex(&p_ctx->organization.mutex, IK_WAIT_FOREVER);
            (void)amba_ik_system_memcpy(&p_ctx->filters.input_param.final_sharpen_noise, p_final_sharpen_noise, sizeof(ik_final_sharpen_noise_t));
            ctx_raise_update_flag(&p_ctx->filters.update_flags.iso.fnlshpns_noise_updated);
            (void)amba_ik_system_unlock_mutex(&p_ctx->organization.mutex);
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
    if (context_id != 0U) {
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
        rval = p_check_func->final_sharpen_fir(p_final_sharpen_fir);
    }
    if(rval == IK_OK) {
        rval |= img_ctx_get_context(context_id, &p_ctx);
        if((p_final_sharpen_fir != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
            (void)amba_ik_system_lock_mutex(&p_ctx->organization.mutex, IK_WAIT_FOREVER);
            (void)amba_ik_system_memcpy(&p_ctx->filters.input_param.final_sharpen_fir, p_final_sharpen_fir, sizeof(ik_final_sharpen_fir_t));
            ctx_raise_update_flag(&p_ctx->filters.update_flags.iso.fnlshpns_fir_updated);
            (void)amba_ik_system_unlock_mutex(&p_ctx->organization.mutex);
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
    if (context_id != 0U) {
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
        rval = p_check_func->final_sharpen_coring(p_final_sharpen_coring);
    }
    if(rval == IK_OK) {
        rval |= img_ctx_get_context(context_id, &p_ctx);
        if((p_final_sharpen_coring != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
            (void)amba_ik_system_lock_mutex(&p_ctx->organization.mutex, IK_WAIT_FOREVER);
            (void)amba_ik_system_memcpy(&p_ctx->filters.input_param.final_sharpen_coring, p_final_sharpen_coring, sizeof(ik_final_sharpen_coring_t));
            ctx_raise_update_flag(&p_ctx->filters.update_flags.iso.fnlshpns_coring_updated);
            (void)amba_ik_system_unlock_mutex(&p_ctx->organization.mutex);
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
    if (context_id != 0U) {
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
        rval = p_check_func->fnlshpns_cor_idx_scl(p_final_sharpen_coring_idx_scale);
    }
    if(rval == IK_OK) {
        rval |= img_ctx_get_context(context_id, &p_ctx);
        if((p_final_sharpen_coring_idx_scale != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
            (void)amba_ik_system_lock_mutex(&p_ctx->organization.mutex, IK_WAIT_FOREVER);
            (void)amba_ik_system_memcpy(&p_ctx->filters.input_param.final_sharpen_coring_idx_scale, p_final_sharpen_coring_idx_scale, sizeof(ik_final_sharpen_coring_idx_scale_t));
            ctx_raise_update_flag(&p_ctx->filters.update_flags.iso.fnlshpns_coring_index_scale_updated);
            (void)amba_ik_system_unlock_mutex(&p_ctx->organization.mutex);
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
    if (context_id != 0U) {
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
        rval = p_check_func->fnlshpns_min_cor_rst(p_fnlshpns_min_coring_result);
    }
    if(rval == IK_OK) {
        rval |= img_ctx_get_context(context_id, &p_ctx);
        if((p_fnlshpns_min_coring_result != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
            (void)amba_ik_system_lock_mutex(&p_ctx->organization.mutex, IK_WAIT_FOREVER);
            (void)amba_ik_system_memcpy(&p_ctx->filters.input_param.final_sharpen_min_coring_result, p_fnlshpns_min_coring_result, sizeof(ik_final_sharpen_min_coring_result_t));
            ctx_raise_update_flag(&p_ctx->filters.update_flags.iso.fnlshpns_min_coring_result_updated);
            (void)amba_ik_system_unlock_mutex(&p_ctx->organization.mutex);
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
    if (context_id != 0U) {
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
        rval = p_check_func->fnlshpns_max_cor_rst(p_fnlshpns_max_coring_result);
    }
    if(rval == IK_OK) {
        rval |= img_ctx_get_context(context_id, &p_ctx);
        if((p_fnlshpns_max_coring_result != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
            (void)amba_ik_system_lock_mutex(&p_ctx->organization.mutex, IK_WAIT_FOREVER);
            (void)amba_ik_system_memcpy(&p_ctx->filters.input_param.final_sharpen_max_coring_result, p_fnlshpns_max_coring_result, sizeof(ik_final_sharpen_max_coring_result_t));
            ctx_raise_update_flag(&p_ctx->filters.update_flags.iso.fnlshpns_max_coring_result_updated);
            (void)amba_ik_system_unlock_mutex(&p_ctx->organization.mutex);
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
    if (context_id != 0U) {
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
        rval = p_check_func->fnlshpns_scl_cor(p_fnlshpns_scale_coring);
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
                (void)amba_ik_system_lock_mutex(&p_ctx->organization.mutex, IK_WAIT_FOREVER);
                (void)amba_ik_system_memcpy(&p_ctx->filters.input_param.final_sharpen_scale_coring, p_fnlshpns_scale_coring, sizeof(ik_final_sharpen_scale_coring_t));
                ctx_raise_update_flag(&p_ctx->filters.update_flags.iso.fnlshpns_scale_coring_updated);
                (void)amba_ik_system_unlock_mutex(&p_ctx->organization.mutex);
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
    if (context_id != 0U) {
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
        rval = p_check_func->final_sharpen_both_three_d_table(p_final_sharpen_both_three_d_table);
    }
    if(rval == IK_OK) {
        rval |= img_ctx_get_context(context_id, &p_ctx);
        if((p_final_sharpen_both_three_d_table != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
            (void)amba_ik_system_lock_mutex(&p_ctx->organization.mutex, IK_WAIT_FOREVER);
            (void)amba_ik_system_memcpy(&p_ctx->filters.input_param.final_sharpen_both_three_d_table, p_final_sharpen_both_three_d_table, sizeof(ik_final_sharpen_both_three_d_table_t));
            ctx_raise_update_flag(&p_ctx->filters.update_flags.iso.fnlshpns_both_tdt_updated);
            p_ctx->filters.input_param.three_d_table_enable = 1U;
            (void)amba_ik_system_unlock_mutex(&p_ctx->organization.mutex);
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
    if (context_id != 0U) {
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
        rval = p_check_func->chroma_filter(p_chroma_filter);
    }
    if(rval == IK_OK) {
        rval |= img_ctx_get_context(context_id, &p_ctx);
        if((p_chroma_filter != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
            (void)amba_ik_system_lock_mutex(&p_ctx->organization.mutex, IK_WAIT_FOREVER);
            (void)amba_ik_system_memcpy(&p_ctx->filters.input_param.chroma_filter, p_chroma_filter, sizeof(ik_chroma_filter_t));
            ctx_raise_update_flag(&p_ctx->filters.update_flags.iso.chroma_filter_updated);
            (void)amba_ik_system_unlock_mutex(&p_ctx->organization.mutex);
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

uint32 img_ctx_ivd_set_wide_chroma_ft(uint32 context_id, const ik_wide_chroma_filter_t *p_wide_chroma_filter)
{
    amba_ik_system_print_str_5("[IK] set_wide_chroma_filter is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0U) {
        //TBD
    }
    if (p_wide_chroma_filter==NULL) {
        //TBD
    }
    return IK_ERR_0004;
}

uint32 img_ctx_set_wide_chroma_filter(uint32 context_id, const ik_wide_chroma_filter_t *p_wide_chroma_filter)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;
    const ik_debug_check_func_t* p_check_func = ik_ctx_get_debug_check_func();
    if(p_check_func != NULL) {
        rval = p_check_func->wide_chroma_filter(p_wide_chroma_filter);
    }
    if(rval == IK_OK) {
        rval |= img_ctx_get_context(context_id, &p_ctx);
        if((p_wide_chroma_filter != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
            (void)amba_ik_system_lock_mutex(&p_ctx->organization.mutex, IK_WAIT_FOREVER);
            (void)amba_ik_system_memcpy(&p_ctx->filters.input_param.wide_chroma_filter, p_wide_chroma_filter, sizeof(ik_wide_chroma_filter_t));
            ctx_raise_update_flag(&p_ctx->filters.update_flags.iso.wide_chroma_filter);
            (void)amba_ik_system_unlock_mutex(&p_ctx->organization.mutex);
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

uint32 img_ctx_ivd_set_wide_chroma_ft_combine(uint32 context_id, const ik_wide_chroma_filter_combine_t *p_chroma_filter_combine)
{
    amba_ik_system_print_str_5("[IK] set_wide_chroma_filter is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0U) {
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
        rval = p_check_func->wide_chroma_filter_combine(p_chroma_filter_combine);
    }
    if(rval == IK_OK) {
        rval |= img_ctx_get_context(context_id, &p_ctx);
        if((p_chroma_filter_combine != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
            (void)amba_ik_system_lock_mutex(&p_ctx->organization.mutex, IK_WAIT_FOREVER);
            (void)amba_ik_system_memcpy(&p_ctx->filters.input_param.wide_chroma_combine, p_chroma_filter_combine, sizeof(ik_wide_chroma_filter_combine_t));
            ctx_raise_update_flag(&p_ctx->filters.update_flags.iso.wide_chroma_filter_combine);
            (void)amba_ik_system_unlock_mutex(&p_ctx->organization.mutex);
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
        rval = p_check_func->grgb_mismatch(p_grgb_mismatch);
    }
    if(rval == IK_OK) {
        rval |= img_ctx_get_context(context_id, &p_ctx);
        if((p_grgb_mismatch != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
            (void)amba_ik_system_lock_mutex(&p_ctx->organization.mutex, IK_WAIT_FOREVER);
            (void)amba_ik_system_memcpy(&p_ctx->filters.input_param.grgb_mismatch, p_grgb_mismatch, sizeof(ik_grgb_mismatch_t));
            ctx_raise_update_flag(&p_ctx->filters.update_flags.iso.grgb_mismatch_updated);
            (void)amba_ik_system_unlock_mutex(&p_ctx->organization.mutex);
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
    if (context_id != 0U) {
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
            rval = p_check_func->video_mctf(p_video_mctf);
        }
        if(rval == IK_OK) {
            if((p_ctx->filters.update_flags.iso.is_1st_frame == 0U)&&
               ((p_ctx->filters.input_param.video_mctf.compression_enable != p_video_mctf->compression_enable)||
                (p_ctx->filters.input_param.video_mctf.compression_bit_rate_luma != p_video_mctf->compression_bit_rate_luma)||
                (p_ctx->filters.input_param.video_mctf.compression_bit_rate_chroma != p_video_mctf->compression_bit_rate_chroma)||
                (p_ctx->filters.input_param.video_mctf.compression_dither_disable != p_video_mctf->compression_dither_disable))) {
                amba_ik_system_print_uint32_5("[IK][ERROR] Invalid mctf compression run-time change, context_id = %d\n", context_id, DC_U, DC_U, DC_U, DC_U);
                amba_ik_system_print_uint32_5("[IK][ERROR] video_mctf.compression_enable, first = %d, update = %d\n", p_ctx->filters.input_param.video_mctf.compression_enable, p_video_mctf->compression_enable, DC_U, DC_U, DC_U);
                amba_ik_system_print_uint32_5("[IK][ERROR] video_mctf.compression_bit_rate_luma, first = %d, update = %d\n", p_ctx->filters.input_param.video_mctf.compression_bit_rate_luma, p_video_mctf->compression_bit_rate_luma, DC_U, DC_U, DC_U);
                amba_ik_system_print_uint32_5("[IK][ERROR] video_mctf.compression_bit_rate_chroma, first = %d, update = %d\n", p_ctx->filters.input_param.video_mctf.compression_bit_rate_chroma, p_video_mctf->compression_bit_rate_chroma, DC_U, DC_U, DC_U);
                amba_ik_system_print_uint32_5("[IK][ERROR] video_mctf.compression_dither_disable, first = %d, update = %d\n", p_ctx->filters.input_param.video_mctf.compression_dither_disable, p_video_mctf->compression_dither_disable, DC_U, DC_U, DC_U);
                rval |= IK_ERR_0008;
            } else {
                (void)amba_ik_system_lock_mutex(&p_ctx->organization.mutex, IK_WAIT_FOREVER);
                (void)amba_ik_system_memcpy(&p_ctx->filters.input_param.video_mctf, p_video_mctf, sizeof(ik_video_mctf_t));
                ctx_raise_update_flag(&p_ctx->filters.update_flags.iso.video_mctf_updated);
                (void)amba_ik_system_unlock_mutex(&p_ctx->organization.mutex);
            }
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
    if (context_id != 0U) {
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
        rval = p_check_func->video_mctf_ta(p_video_mctf_ta);
    }
    if(rval == IK_OK) {
        rval |= img_ctx_get_context(context_id, &p_ctx);
        if((p_video_mctf_ta != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
            (void)amba_ik_system_lock_mutex(&p_ctx->organization.mutex, IK_WAIT_FOREVER);
            (void)amba_ik_system_memcpy(&p_ctx->filters.input_param.video_mctf_ta, p_video_mctf_ta, sizeof(ik_video_mctf_ta_t));
            ctx_raise_update_flag(&p_ctx->filters.update_flags.iso.video_mctf_ta_updated);
            (void)amba_ik_system_unlock_mutex(&p_ctx->organization.mutex);
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
    if (context_id != 0U) {
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
        rval = p_check_func->video_mctf_and_fnlshp(p_video_mctf_and_final_sharpen);
    }
    if(rval == IK_OK) {
        rval |= img_ctx_get_context(context_id, &p_ctx);
        if((p_video_mctf_and_final_sharpen != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
            (void)amba_ik_system_lock_mutex(&p_ctx->organization.mutex, IK_WAIT_FOREVER);
            (void)amba_ik_system_memcpy(&p_ctx->filters.input_param.video_mctf_and_final_sharpen, p_video_mctf_and_final_sharpen, sizeof(ik_pos_dep33_t));
            ctx_raise_update_flag(&p_ctx->filters.update_flags.iso.video_mctf_and_final_sharpen_updated);
            (void)amba_ik_system_unlock_mutex(&p_ctx->organization.mutex);
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

uint32 img_ctx_ivd_set_mctf_internal(uint32 context_id, const ik_mctf_internal_t *p_video_mctf_internal)
{
    amba_ik_system_print_str_5("[IK] set_video_mctf_internal is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0U) {
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
        rval = p_check_func->video_mctf_internal(p_video_mctf_internal);
    }
    if(rval == IK_OK) {
        rval |= img_ctx_get_context(context_id, &p_ctx);
        if((p_video_mctf_internal != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
            (void)amba_ik_system_lock_mutex(&p_ctx->organization.mutex, IK_WAIT_FOREVER);
            (void)amba_ik_system_memcpy(&p_ctx->filters.input_param.internal_video_mctf, p_video_mctf_internal, sizeof(ik_mctf_internal_t));
            ctx_raise_update_flag(&p_ctx->filters.update_flags.iso.internal_video_mctf_updated);
            (void)amba_ik_system_unlock_mutex(&p_ctx->organization.mutex);
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
    if (context_id != 0U) {
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
        rval = p_check_func->motion_detect(p_motion_detect);
    }
    if(rval == IK_OK) {
        rval |= img_ctx_get_context(context_id, &p_ctx);
        if((p_motion_detect != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
            (void)amba_ik_system_lock_mutex(&p_ctx->organization.mutex, IK_WAIT_FOREVER);
            (void)amba_ik_system_memcpy(&p_ctx->filters.input_param.motion_detect, p_motion_detect, sizeof(ik_motion_detect_t));
            ctx_raise_update_flag(&p_ctx->filters.update_flags.iso.motion_detect_updated);
            (void)amba_ik_system_unlock_mutex(&p_ctx->organization.mutex);
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
    if (context_id != 0U) {
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
        rval = p_check_func->motion_detect_pos_dep(p_md_pos_dep);
    }
    if(rval == IK_OK) {
        rval |= img_ctx_get_context(context_id, &p_ctx);
        if((p_md_pos_dep != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
            (void)amba_ik_system_lock_mutex(&p_ctx->organization.mutex, IK_WAIT_FOREVER);
            (void)amba_ik_system_memcpy(&p_ctx->filters.input_param.motion_detect_pos_dep, p_md_pos_dep, sizeof(ik_pos_dep33_t));
            ctx_raise_update_flag(&p_ctx->filters.update_flags.iso.motion_detect_pos_dep_updated);
            (void)amba_ik_system_unlock_mutex(&p_ctx->organization.mutex);
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
    if (context_id != 0U) {
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
        rval = p_check_func->motion_detect_and_mctf(p_motion_detect_and_mctf);
    }
    if(rval == IK_OK) {
        rval |= img_ctx_get_context(context_id, &p_ctx);
        if((p_motion_detect_and_mctf != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
            (void)amba_ik_system_lock_mutex(&p_ctx->organization.mutex, IK_WAIT_FOREVER);
            (void)amba_ik_system_memcpy(&p_ctx->filters.input_param.motion_detect_and_mctf, p_motion_detect_and_mctf, sizeof(ik_motion_detect_and_mctf_t));
            ctx_raise_update_flag(&p_ctx->filters.update_flags.iso.motion_detect_and_mctf_updated);
            (void)amba_ik_system_unlock_mutex(&p_ctx->organization.mutex);
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

uint32 img_ctx_ivd_set_aaa_stat_info(uint32 context_id, const ik_aaa_stat_info_t *p_stat_info)
{
    amba_ik_system_print_str_5("[IK] set_aaa_stat_info is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0U) {
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
        rval = p_check_func->aaa_stat(p_stat_info);
    }
    if(rval == IK_OK) {
        rval |= img_ctx_get_context(context_id, &p_ctx);
        if((p_stat_info != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
            (void)amba_ik_system_lock_mutex(&p_ctx->organization.mutex, IK_WAIT_FOREVER);
            (void)amba_ik_system_memcpy(&p_ctx->filters.input_param.aaa_stat_info, p_stat_info, sizeof(ik_aaa_stat_info_t));
            ctx_raise_update_flag(&p_ctx->filters.update_flags.iso.aaa_stat_info_updated);
            (void)amba_ik_system_unlock_mutex(&p_ctx->organization.mutex);
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

uint32 img_ctx_ivd_set_aaa_pg_af_info(uint32 context_id, const ik_aaa_pg_af_stat_info_t *p_pg_af_stat_info)
{
    amba_ik_system_print_str_5("[IK] set_aaa_pg_af_info is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0U) {
        //TBD
    }
    if (p_pg_af_stat_info == NULL) {
        //TBD
    }
    return IK_ERR_0004;
}

uint32 img_ctx_set_aaa_pg_af_info(uint32 context_id, const ik_aaa_pg_af_stat_info_t *p_pg_af_stat_info)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;
    const ik_debug_check_func_t* p_check_func = ik_ctx_get_debug_check_func();
    if(p_check_func != NULL) {
        rval = p_check_func->aaa_pg_af_stat(p_pg_af_stat_info);
    }
    if(rval == IK_OK) {
        rval |= img_ctx_get_context(context_id, &p_ctx);
        if((p_pg_af_stat_info != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
            (void)amba_ik_system_lock_mutex(&p_ctx->organization.mutex, IK_WAIT_FOREVER);
            //due to pg_af support up to 32x16, thus it should be saparately stored in context.
            (void)amba_ik_system_memcpy(&p_ctx->filters.input_param.aaa_pg_stat_info, p_pg_af_stat_info, sizeof(ik_aaa_pg_af_stat_info_t));
            ctx_raise_update_flag(&p_ctx->filters.update_flags.iso.aaa_stat_info_updated);
            (void)amba_ik_system_unlock_mutex(&p_ctx->organization.mutex);
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
    if (context_id != 0U) {
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
        rval = p_check_func->af_stat_ex(p_af_stat_ex_info);
    }
    if(rval == IK_OK) {
        rval |= img_ctx_get_context(context_id, &p_ctx);
        if((p_af_stat_ex_info != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
            (void)amba_ik_system_lock_mutex(&p_ctx->organization.mutex, IK_WAIT_FOREVER);
            (void)amba_ik_system_memcpy(&p_ctx->filters.input_param.af_stat_ex_info, p_af_stat_ex_info, sizeof(ik_af_stat_ex_info_t));
            ctx_raise_update_flag(&p_ctx->filters.update_flags.iso.af_stat_ex_info_updated);
            (void)amba_ik_system_unlock_mutex(&p_ctx->organization.mutex);
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
    if (context_id != 0U) {
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
        rval = p_check_func->pg_af_stat_ex(p_pg_af_stat_ex_info);
    }
    if(rval == IK_OK) {
        rval |= img_ctx_get_context(context_id, &p_ctx);
        if((p_pg_af_stat_ex_info != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
            (void)amba_ik_system_lock_mutex(&p_ctx->organization.mutex, IK_WAIT_FOREVER);
            (void)amba_ik_system_memcpy(&p_ctx->filters.input_param.pg_af_stat_ex_info, p_pg_af_stat_ex_info, sizeof(ik_pg_af_stat_ex_info_t));
            ctx_raise_update_flag(&p_ctx->filters.update_flags.iso.pg_af_stat_ex_info_updated);
            (void)amba_ik_system_unlock_mutex(&p_ctx->organization.mutex);
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
    if (context_id != 0U) {
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

    rval |= img_ctx_get_context(context_id, &p_ctx);
    if((p_check_func != NULL)&&(rval == IK_OK)) {
        rval = p_check_func->window_size_info(p_window_size_info, p_ctx->filters.input_param.ability);
    }
    if((p_window_size_info != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        (void)amba_ik_system_lock_mutex(&p_ctx->organization.mutex, IK_WAIT_FOREVER);
        (void)amba_ik_system_memcpy(&p_ctx->filters.input_param.window_size_info, p_window_size_info, sizeof(ik_window_size_info_t));
        ctx_raise_update_flag(&p_ctx->filters.update_flags.iso.window_size_info_updated);
        (void)amba_ik_system_unlock_mutex(&p_ctx->organization.mutex);
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

uint32 img_ctx_ivd_set_cfa_window_size_info(uint32 context_id, const ik_cfa_window_size_info_t *p_cfa_window_size_info)
{
    amba_ik_system_print_str_5("[IK] set_cfa_window_size_info_gain is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0U) {
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

    rval |= img_ctx_get_context(context_id, &p_ctx);
    if((p_check_func != NULL)&&(rval == IK_OK)) {
        rval = p_check_func->cfa_window_size_info(p_cfa_window_size_info);
    }
    if((p_cfa_window_size_info != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        (void)amba_ik_system_lock_mutex(&p_ctx->organization.mutex, IK_WAIT_FOREVER);
        (void)amba_ik_system_memcpy(&p_ctx->filters.input_param.cfa_window_size_info, p_cfa_window_size_info, sizeof(ik_cfa_window_size_info_t));
        ctx_raise_update_flag(&p_ctx->filters.update_flags.iso.cfa_window_size_info_updated);
        (void)amba_ik_system_unlock_mutex(&p_ctx->organization.mutex);
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

uint32 img_ctx_ivd_set_fe_tone_curve(uint32 context_id, const ik_frontend_tone_curve_t *p_fe_tc)
{
    amba_ik_system_print_str_5("[IK] img_ctx_ivd_set_fe_tone_curve is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0U) {
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
        rval = p_check_func->fe_tone_curve(p_fe_tc);
    }
    if(rval == IK_OK) {
        rval |= img_ctx_get_context(context_id, &p_ctx);
        if((p_fe_tc != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
            (void)amba_ik_system_lock_mutex(&p_ctx->organization.mutex, IK_WAIT_FOREVER);
            (void)amba_ik_system_memcpy(&p_ctx->filters.input_param.fe_tone_curve, p_fe_tc, sizeof(ik_frontend_tone_curve_t));
            ctx_raise_update_flag(&p_ctx->filters.update_flags.iso.fe_tone_curve_updated);
            (void)amba_ik_system_unlock_mutex(&p_ctx->organization.mutex);
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
    if (context_id != 0U) {
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
        rval = p_check_func->resamp_strength(p_resample_str);
    }
    if(rval == IK_OK) {
        rval |= img_ctx_get_context(context_id, &p_ctx);
        if((p_resample_str != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
            (void)amba_ik_system_lock_mutex(&p_ctx->organization.mutex, IK_WAIT_FOREVER);
            (void)amba_ik_system_memcpy(&p_ctx->filters.input_param.resample_str, p_resample_str, sizeof(ik_resampler_strength_t));
            ctx_raise_update_flag(&p_ctx->filters.update_flags.iso.resample_str_update);
            (void)amba_ik_system_unlock_mutex(&p_ctx->organization.mutex);
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
    if (context_id != 0U) {
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
        rval = p_check_func->histogram_info(p_hist_info);
    }
    if(rval == IK_OK) {
        rval |= img_ctx_get_context(context_id, &p_ctx);
        if((p_hist_info != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
            (void)amba_ik_system_lock_mutex(&p_ctx->organization.mutex, IK_WAIT_FOREVER);
            (void)amba_ik_system_memcpy(&p_ctx->filters.input_param.hist_info, p_hist_info, sizeof(ik_histogram_info_t));
            ctx_raise_update_flag(&p_ctx->filters.update_flags.iso.histogram_info_update);
            (void)amba_ik_system_unlock_mutex(&p_ctx->organization.mutex);
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
    if (context_id != 0U) {
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
        rval = p_check_func->histogram_info(p_hist_info);
    }
    if(rval == IK_OK) {
        rval |= img_ctx_get_context(context_id, &p_ctx);
        if((p_hist_info != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
            (void)amba_ik_system_lock_mutex(&p_ctx->organization.mutex, IK_WAIT_FOREVER);
            (void)amba_ik_system_memcpy(&p_ctx->filters.input_param.hist_info_pg, p_hist_info, sizeof(ik_histogram_info_t));
            ctx_raise_update_flag(&p_ctx->filters.update_flags.iso.histogram_info_pg_update);
            (void)amba_ik_system_unlock_mutex(&p_ctx->organization.mutex);
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

//mono
uint32 img_ctx_ivd_set_mono_dynamic_bad_pixel_corr(uint32 context_id, const ik_mono_dynamic_bad_pixel_corr_t *p_mono_dynamic_bad_pixel_corr)
{
    amba_ik_system_print_str_5("[IK] set_mono_dynamic_bad_pixel_corr is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0U) {
        //MISRAC
    }
    if (p_mono_dynamic_bad_pixel_corr==NULL) {
        //MISRAC
    }
    return IK_ERR_0004;
}

uint32 img_ctx_set_mono_dynamic_bad_pixel_corr(uint32 context_id, const ik_mono_dynamic_bad_pixel_corr_t *p_mono_dynamic_bad_pixel_corr)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;
    const ik_debug_check_func_t* p_check_func = ik_ctx_get_debug_check_func();
    if(p_check_func != NULL) {
        rval = p_check_func->mono_dynamic_bad_pixel_corr(p_mono_dynamic_bad_pixel_corr);
    }
    if(rval == IK_OK) {
        rval |= img_ctx_get_context(context_id, &p_ctx);
        if((p_mono_dynamic_bad_pixel_corr != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
            (void)amba_ik_system_lock_mutex(&p_ctx->organization.mutex, IK_WAIT_FOREVER);
            (void)amba_ik_system_memcpy(&p_ctx->filters.input_param.mono_dynamic_bad_pixel_corr, p_mono_dynamic_bad_pixel_corr, sizeof(ik_mono_dynamic_bad_pixel_corr_t));
            ctx_raise_update_flag(&p_ctx->filters.update_flags.iso.mono_dynamic_bad_pixel_corr_updated);
            (void)amba_ik_system_unlock_mutex(&p_ctx->organization.mutex);
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

uint32 img_ctx_ivd_set_mono_cfa_leakage_filter(uint32 context_id, const ik_mono_cfa_leakage_filter_t *p_mono_cfa_leakage_filter)
{
    amba_ik_system_print_str_5("[IK] set_mono_cfa_leakage_filter is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0U) {
        //MISRAC
    }
    if (p_mono_cfa_leakage_filter==NULL) {
        //MISRAC
    }
    return IK_ERR_0004;
}

uint32 img_ctx_set_mono_cfa_leakage_filter(uint32 context_id, const ik_mono_cfa_leakage_filter_t *p_mono_cfa_leakage_filter)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;
    const ik_debug_check_func_t* p_check_func = ik_ctx_get_debug_check_func();
    if(p_check_func != NULL) {
        rval = p_check_func->mono_cfa_leakage_filter(p_mono_cfa_leakage_filter);
    }
    if(rval == IK_OK) {
        rval |= img_ctx_get_context(context_id, &p_ctx);
        if((p_mono_cfa_leakage_filter != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
            (void)amba_ik_system_lock_mutex(&p_ctx->organization.mutex, IK_WAIT_FOREVER);
            (void)amba_ik_system_memcpy(&p_ctx->filters.input_param.mono_cfa_leakage_filter, p_mono_cfa_leakage_filter, sizeof(ik_mono_cfa_leakage_filter_t));
            ctx_raise_update_flag(&p_ctx->filters.update_flags.iso.mono_cfa_leakage_filter_updated);
            (void)amba_ik_system_unlock_mutex(&p_ctx->organization.mutex);
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

uint32 img_ctx_ivd_set_mono_cfa_noise_filter(uint32 context_id, const ik_mono_cfa_noise_filter_t *p_mono_cfa_noise_filter)
{
    amba_ik_system_print_str_5("[IK] set_mono_cfa_noise_filter is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0U) {
        //MISRAC
    }
    if (p_mono_cfa_noise_filter==NULL) {
        //MISRAC
    }
    return IK_ERR_0004;
}

uint32 img_ctx_set_mono_cfa_noise_filter(uint32 context_id, const ik_mono_cfa_noise_filter_t *p_mono_cfa_noise_filter)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;
    const ik_debug_check_func_t* p_check_func = ik_ctx_get_debug_check_func();
    if(p_check_func != NULL) {
        rval = p_check_func->mono_cfa_noise_filter(p_mono_cfa_noise_filter);
    }
    if(rval == IK_OK) {
        rval |= img_ctx_get_context(context_id, &p_ctx);
        if((p_mono_cfa_noise_filter != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
            (void)amba_ik_system_lock_mutex(&p_ctx->organization.mutex, IK_WAIT_FOREVER);
            (void)amba_ik_system_memcpy(&p_ctx->filters.input_param.mono_cfa_noise_filter, p_mono_cfa_noise_filter, sizeof(ik_mono_cfa_noise_filter_t));
            ctx_raise_update_flag(&p_ctx->filters.update_flags.iso.mono_cfa_noise_filter_updated);
            (void)amba_ik_system_unlock_mutex(&p_ctx->organization.mutex);
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

uint32 img_ctx_ivd_set_mono_anti_aliasing(uint32 context_id, const ik_mono_anti_aliasing_t *p_mono_anti_aliasing)
{
    amba_ik_system_print_str_5("[IK] set_mono_anti_aliasing is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0U) {
        //MISRAC
    }
    if (p_mono_anti_aliasing==NULL) {
        //MISRAC
    }
    return IK_ERR_0004;
}

uint32 img_ctx_set_mono_anti_aliasing(uint32 context_id, const ik_mono_anti_aliasing_t *p_mono_anti_aliasing)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;
    const ik_debug_check_func_t* p_check_func = ik_ctx_get_debug_check_func();
    if(p_check_func != NULL) {
        rval = p_check_func->mono_anti_aliasing(p_mono_anti_aliasing);
    }
    if(rval == IK_OK) {
        rval |= img_ctx_get_context(context_id, &p_ctx);
        if((p_mono_anti_aliasing != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
            (void)amba_ik_system_lock_mutex(&p_ctx->organization.mutex, IK_WAIT_FOREVER);
            (void)amba_ik_system_memcpy(&p_ctx->filters.input_param.mono_anti_aliasing, p_mono_anti_aliasing, sizeof(ik_mono_anti_aliasing_t));
            ctx_raise_update_flag(&p_ctx->filters.update_flags.iso.mono_anti_aliasing_updated);
            (void)amba_ik_system_unlock_mutex(&p_ctx->organization.mutex);
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

uint32 img_ctx_ivd_set_mono_before_ce_wb_gain(uint32 context_id, const ik_mono_before_ce_wb_gain_t *p_mono_before_ce_wb_gain)
{
    amba_ik_system_print_str_5("[IK] set_mono_before_ce_wb_gain is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0U) {
        //MISRAC
    }
    if (p_mono_before_ce_wb_gain==NULL) {
        //MISRAC
    }
    return IK_ERR_0004;
}

uint32 img_ctx_set_mono_before_ce_wb_gain(uint32 context_id, const ik_mono_before_ce_wb_gain_t *p_mono_before_ce_wb_gain)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;
    const ik_debug_check_func_t* p_check_func = ik_ctx_get_debug_check_func();
    if(p_check_func != NULL) {
        rval = p_check_func->mono_before_ce_wb_gain(p_mono_before_ce_wb_gain);
    }
    if(rval == IK_OK) {
        rval |= img_ctx_get_context(context_id, &p_ctx);
        if((p_mono_before_ce_wb_gain != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
            (void)amba_ik_system_lock_mutex(&p_ctx->organization.mutex, IK_WAIT_FOREVER);
            (void)amba_ik_system_memcpy(&p_ctx->filters.input_param.mono_before_ce_wb_gain, p_mono_before_ce_wb_gain, sizeof(ik_mono_before_ce_wb_gain_t));
            ctx_raise_update_flag(&p_ctx->filters.update_flags.iso.mono_before_ce_wb_gain_updated);
            (void)amba_ik_system_unlock_mutex(&p_ctx->organization.mutex);
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

uint32 img_ctx_ivd_set_mono_after_ce_wb_gain(uint32 context_id, const ik_mono_after_ce_wb_gain_t *p_mono_after_ce_wb_gain)
{
    amba_ik_system_print_str_5("[IK] set_mono_after_ce_wb_gain is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0U) {
        //MISRAC
    }
    if (p_mono_after_ce_wb_gain==NULL) {
        //MISRAC
    }
    return IK_ERR_0004;
}

uint32 img_ctx_set_mono_after_ce_wb_gain(uint32 context_id, const ik_mono_after_ce_wb_gain_t *p_mono_after_ce_wb_gain)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;
    const ik_debug_check_func_t* p_check_func = ik_ctx_get_debug_check_func();
    if(p_check_func != NULL) {
        rval = p_check_func->mono_after_ce_wb_gain(p_mono_after_ce_wb_gain);
    }
    if(rval == IK_OK) {
        rval |= img_ctx_get_context(context_id, &p_ctx);
        if((p_mono_after_ce_wb_gain != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
            (void)amba_ik_system_lock_mutex(&p_ctx->organization.mutex, IK_WAIT_FOREVER);
            (void)amba_ik_system_memcpy(&p_ctx->filters.input_param.mono_after_ce_wb_gain, p_mono_after_ce_wb_gain, sizeof(ik_mono_after_ce_wb_gain_t));
            ctx_raise_update_flag(&p_ctx->filters.update_flags.iso.mono_after_ce_wb_gain_updated);
            (void)amba_ik_system_unlock_mutex(&p_ctx->organization.mutex);
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

uint32 img_ctx_ivd_set_mono_color_correction(uint32 context_id, const ik_mono_color_correction_t *p_mono_color_correction)
{
    amba_ik_system_print_str_5("[IK] set_mono_color_correction is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0U) {
        //MISRAC
    }
    if (p_mono_color_correction==NULL) {
        //MISRAC
    }
    return IK_ERR_0004;
}

uint32 img_ctx_set_mono_color_correction(uint32 context_id, const ik_mono_color_correction_t *p_mono_color_correction)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;
    const ik_debug_check_func_t* p_check_func = ik_ctx_get_debug_check_func();
    if(p_check_func != NULL) {
        rval = p_check_func->mono_color_correction(p_mono_color_correction);
    }
    if(rval == IK_OK) {
        rval |= img_ctx_get_context(context_id, &p_ctx);
        if((p_mono_color_correction != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
            (void)amba_ik_system_lock_mutex(&p_ctx->organization.mutex, IK_WAIT_FOREVER);
            (void)amba_ik_system_memcpy(&p_ctx->filters.input_param.mono_color_correction, p_mono_color_correction, sizeof(ik_mono_color_correction_t));
            ctx_raise_update_flag(&p_ctx->filters.update_flags.iso.mono_color_correction_updated);
            (void)amba_ik_system_unlock_mutex(&p_ctx->organization.mutex);
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

uint32 img_ctx_ivd_set_mono_tone_curve(uint32 context_id, const ik_mono_tone_curve_t *p_mono_tone_curve)
{
    amba_ik_system_print_str_5("[IK] set_mono_tone_curve is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0U) {
        //MISRAC
    }
    if (p_mono_tone_curve==NULL) {
        //MISRAC
    }
    return IK_ERR_0004;
}

uint32 img_ctx_set_mono_tone_curve(uint32 context_id, const ik_mono_tone_curve_t *p_mono_tone_curve)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;
    const ik_debug_check_func_t* p_check_func = ik_ctx_get_debug_check_func();
    if(p_check_func != NULL) {
        rval = p_check_func->mono_tone_curve(p_mono_tone_curve);
    }
    if(rval == IK_OK) {
        rval |= img_ctx_get_context(context_id, &p_ctx);
        if((p_mono_tone_curve != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
            (void)amba_ik_system_lock_mutex(&p_ctx->organization.mutex, IK_WAIT_FOREVER);
            (void)amba_ik_system_memcpy(&p_ctx->filters.input_param.mono_tone_curve, p_mono_tone_curve, sizeof(ik_mono_tone_curve_t));
            ctx_raise_update_flag(&p_ctx->filters.update_flags.iso.mono_tone_curve_updated);
            (void)amba_ik_system_unlock_mutex(&p_ctx->organization.mutex);
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

uint32 img_ctx_ivd_set_mono_rgb_to_yuv_matrix(uint32 context_id, const ik_mono_rgb_to_yuv_matrix_t *p_mono_rgb_to_yuv_matrix)
{
    amba_ik_system_print_str_5("[IK] set_mono_rgb_to_yuv_matrix is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0U) {
        //MISRAC
    }
    if (p_mono_rgb_to_yuv_matrix==NULL) {
        //MISRAC
    }
    return IK_ERR_0004;
}

uint32 img_ctx_set_mono_rgb_to_yuv_matrix(uint32 context_id, const ik_mono_rgb_to_yuv_matrix_t *p_mono_rgb_to_yuv_matrix)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;
    const ik_debug_check_func_t* p_check_func = ik_ctx_get_debug_check_func();
    if(p_check_func != NULL) {
        rval = p_check_func->mono_rgb_to_yuv_matrix(p_mono_rgb_to_yuv_matrix);
    }
    if(rval == IK_OK) {
        rval |= img_ctx_get_context(context_id, &p_ctx);
        if((p_mono_rgb_to_yuv_matrix != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
            (void)amba_ik_system_lock_mutex(&p_ctx->organization.mutex, IK_WAIT_FOREVER);
            (void)amba_ik_system_memcpy(&p_ctx->filters.input_param.mono_rgb_to_yuv_matrix, p_mono_rgb_to_yuv_matrix, sizeof(ik_mono_rgb_to_yuv_matrix_t));
            ctx_raise_update_flag(&p_ctx->filters.update_flags.iso.mono_rgb_to_yuv_matrix_updated);
            (void)amba_ik_system_unlock_mutex(&p_ctx->organization.mutex);
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

uint32 img_ctx_ivd_set_mono_demosaic(uint32 context_id, const ik_mono_demosaic_t *p_mono_demosaic)
{
    amba_ik_system_print_str_5("[IK] set_mono_demosaic is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0U) {
        //MISRAC
    }
    if (p_mono_demosaic==NULL) {
        //MISRAC
    }
    return IK_ERR_0004;
}

uint32 img_ctx_set_mono_demosaic(uint32 context_id, const ik_mono_demosaic_t *p_mono_demosaic)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;
    const ik_debug_check_func_t* p_check_func = ik_ctx_get_debug_check_func();
    if(p_check_func != NULL) {
        rval = p_check_func->mono_demosaic(p_mono_demosaic);
    }
    if(rval == IK_OK) {
        rval |= img_ctx_get_context(context_id, &p_ctx);
        if((p_mono_demosaic != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
            (void)amba_ik_system_lock_mutex(&p_ctx->organization.mutex, IK_WAIT_FOREVER);
            (void)amba_ik_system_memcpy(&p_ctx->filters.input_param.mono_demosaic, p_mono_demosaic, sizeof(ik_mono_demosaic_t));
            ctx_raise_update_flag(&p_ctx->filters.update_flags.iso.mono_demosaic_updated);
            (void)amba_ik_system_unlock_mutex(&p_ctx->organization.mutex);
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

uint32 img_ctx_ivd_set_mono_grgb_mismatch(uint32 context_id, const ik_mono_grgb_mismatch_t *p_mono_grgb_mismatch)
{
    amba_ik_system_print_str_5("[IK] set_mono_grgb_mismatch is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0U) {
        //MISRAC
    }
    if (p_mono_grgb_mismatch==NULL) {
        //MISRAC
    }
    return IK_ERR_0004;
}

uint32 img_ctx_set_mono_grgb_mismatch(uint32 context_id, const ik_mono_grgb_mismatch_t *p_mono_grgb_mismatch)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;
    const ik_debug_check_func_t* p_check_func = ik_ctx_get_debug_check_func();
    if(p_check_func != NULL) {
        rval = p_check_func->mono_grgb_mismatch(p_mono_grgb_mismatch);
    }
    if(rval == IK_OK) {
        rval |= img_ctx_get_context(context_id, &p_ctx);
        if((p_mono_grgb_mismatch != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
            (void)amba_ik_system_lock_mutex(&p_ctx->organization.mutex, IK_WAIT_FOREVER);
            (void)amba_ik_system_memcpy(&p_ctx->filters.input_param.mono_grgb_mismatch, p_mono_grgb_mismatch, sizeof(ik_mono_grgb_mismatch_t));
            ctx_raise_update_flag(&p_ctx->filters.update_flags.iso.mono_grgb_mismatch_updated);
            (void)amba_ik_system_unlock_mutex(&p_ctx->organization.mutex);
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

uint32 img_ctx_ivd_set_mono_fst_luma_process_mode(uint32 context_id, const ik_mono_fst_luma_process_mode_t *p_mono_fst_luma_process_mode)
{
    amba_ik_system_print_str_5("[IK] set_mono_fst_luma_process_mode is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0U) {
        //MISRAC
    }
    if (p_mono_fst_luma_process_mode==NULL) {
        //MISRAC
    }
    return IK_ERR_0004;
}

uint32 img_ctx_set_mono_fst_luma_process_mode(uint32 context_id, const ik_mono_fst_luma_process_mode_t *p_mono_fst_luma_process_mode)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;
    const ik_debug_check_func_t* p_check_func = ik_ctx_get_debug_check_func();
    if(p_check_func != NULL) {
        rval = p_check_func->mono_fst_luma_process_mode(p_mono_fst_luma_process_mode);
    }
    if(rval == IK_OK) {
        rval |= img_ctx_get_context(context_id, &p_ctx);
        if((p_mono_fst_luma_process_mode != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
            (void)amba_ik_system_lock_mutex(&p_ctx->organization.mutex, IK_WAIT_FOREVER);
            (void)amba_ik_system_memcpy(&p_ctx->filters.input_param.mono_fst_luma_process_mode, p_mono_fst_luma_process_mode, sizeof(ik_mono_fst_luma_process_mode_t));
            ctx_raise_update_flag(&p_ctx->filters.update_flags.iso.mono_fst_luma_process_mode_updated);
            (void)amba_ik_system_unlock_mutex(&p_ctx->organization.mutex);
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

uint32 img_ctx_ivd_set_mono_adv_spatial_filter(uint32 context_id, const ik_mono_adv_spatial_filter_t *p_mono_adv_spatial_filter)
{
    amba_ik_system_print_str_5("[IK] set_mono_adv_spatial_filter is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0U) {
        //MISRAC
    }
    if (p_mono_adv_spatial_filter==NULL) {
        //MISRAC
    }
    return IK_ERR_0004;
}

uint32 img_ctx_set_mono_adv_spatial_filter(uint32 context_id, const ik_mono_adv_spatial_filter_t *p_mono_adv_spatial_filter)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;
    const ik_debug_check_func_t* p_check_func = ik_ctx_get_debug_check_func();
    if(p_check_func != NULL) {
        rval = p_check_func->mono_adv_spatial_filter(p_mono_adv_spatial_filter);
    }
    if(rval == IK_OK) {
        rval |= img_ctx_get_context(context_id, &p_ctx);
        if((p_mono_adv_spatial_filter != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
            (void)amba_ik_system_lock_mutex(&p_ctx->organization.mutex, IK_WAIT_FOREVER);
            (void)amba_ik_system_memcpy(&p_ctx->filters.input_param.mono_adv_spatial_filter, p_mono_adv_spatial_filter, sizeof(ik_mono_adv_spatial_filter_t));
            ctx_raise_update_flag(&p_ctx->filters.update_flags.iso.mono_adv_spatial_filter_updated);
            (void)amba_ik_system_unlock_mutex(&p_ctx->organization.mutex);
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

uint32 img_ctx_ivd_set_mono_fst_shp_both(uint32 context_id, const ik_mono_fst_shp_both_t *p_mono_fst_shp_both)
{
    amba_ik_system_print_str_5("[IK] set_mono_fst_shp_both is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0U) {
        //MISRAC
    }
    if (p_mono_fst_shp_both==NULL) {
        //MISRAC
    }
    return IK_ERR_0004;
}

uint32 img_ctx_set_mono_fst_shp_both(uint32 context_id, const ik_mono_fst_shp_both_t *p_mono_fst_shp_both)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;
    const ik_debug_check_func_t* p_check_func = ik_ctx_get_debug_check_func();
    if(p_check_func != NULL) {
        rval = p_check_func->mono_fst_shp_both(p_mono_fst_shp_both);
    }
    if(rval == IK_OK) {
        rval |= img_ctx_get_context(context_id, &p_ctx);
        if((p_mono_fst_shp_both != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
            (void)amba_ik_system_lock_mutex(&p_ctx->organization.mutex, IK_WAIT_FOREVER);
            (void)amba_ik_system_memcpy(&p_ctx->filters.input_param.mono_fst_shp_both, p_mono_fst_shp_both, sizeof(ik_mono_fst_shp_both_t));
            ctx_raise_update_flag(&p_ctx->filters.update_flags.iso.mono_fst_shp_both_updated);
            (void)amba_ik_system_unlock_mutex(&p_ctx->organization.mutex);
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

uint32 img_ctx_ivd_set_mono_fst_shp_noise(uint32 context_id, const ik_mono_fst_shp_noise_t *p_mono_fst_shp_noise)
{
    amba_ik_system_print_str_5("[IK] set_mono_fst_shp_noise is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0U) {
        //MISRAC
    }
    if (p_mono_fst_shp_noise==NULL) {
        //MISRAC
    }
    return IK_ERR_0004;
}

uint32 img_ctx_set_mono_fst_shp_noise(uint32 context_id, const ik_mono_fst_shp_noise_t *p_mono_fst_shp_noise)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;
    const ik_debug_check_func_t* p_check_func = ik_ctx_get_debug_check_func();
    if(p_check_func != NULL) {
        rval = p_check_func->mono_fst_shp_noise(p_mono_fst_shp_noise);
    }
    if(rval == IK_OK) {
        rval |= img_ctx_get_context(context_id, &p_ctx);
        if((p_mono_fst_shp_noise != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
            (void)amba_ik_system_lock_mutex(&p_ctx->organization.mutex, IK_WAIT_FOREVER);
            (void)amba_ik_system_memcpy(&p_ctx->filters.input_param.mono_fst_shp_noise, p_mono_fst_shp_noise, sizeof(ik_mono_fst_shp_noise_t));
            ctx_raise_update_flag(&p_ctx->filters.update_flags.iso.mono_fst_shp_noise_updated);
            (void)amba_ik_system_unlock_mutex(&p_ctx->organization.mutex);
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

uint32 img_ctx_ivd_set_mono_fst_shp_fir(uint32 context_id, const ik_mono_fst_shp_fir_t *p_mono_fst_shp_fir)
{
    amba_ik_system_print_str_5("[IK] set_mono_fst_shp_fir is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0U) {
        //MISRAC
    }
    if (p_mono_fst_shp_fir==NULL) {
        //MISRAC
    }
    return IK_ERR_0004;
}

uint32 img_ctx_set_mono_fst_shp_fir(uint32 context_id, const ik_mono_fst_shp_fir_t *p_mono_fst_shp_fir)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;
    const ik_debug_check_func_t* p_check_func = ik_ctx_get_debug_check_func();
    if(p_check_func != NULL) {
        rval = p_check_func->mono_fst_shp_fir(p_mono_fst_shp_fir);
    }
    if(rval == IK_OK) {
        rval |= img_ctx_get_context(context_id, &p_ctx);
        if((p_mono_fst_shp_fir != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
            (void)amba_ik_system_lock_mutex(&p_ctx->organization.mutex, IK_WAIT_FOREVER);
            (void)amba_ik_system_memcpy(&p_ctx->filters.input_param.mono_fst_shp_fir, p_mono_fst_shp_fir, sizeof(ik_mono_fst_shp_fir_t));
            ctx_raise_update_flag(&p_ctx->filters.update_flags.iso.mono_fst_shp_fir_updated);
            (void)amba_ik_system_unlock_mutex(&p_ctx->organization.mutex);
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

uint32 img_ctx_ivd_set_mono_fst_shp_coring(uint32 context_id, const ik_mono_fst_shp_coring_t *p_mono_fst_shp_coring)
{
    amba_ik_system_print_str_5("[IK] set_mono_fst_shp_coring is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0U) {
        //MISRAC
    }
    if (p_mono_fst_shp_coring==NULL) {
        //MISRAC
    }
    return IK_ERR_0004;
}

uint32 img_ctx_set_mono_fst_shp_coring(uint32 context_id, const ik_mono_fst_shp_coring_t *p_mono_fst_shp_coring)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;
    const ik_debug_check_func_t* p_check_func = ik_ctx_get_debug_check_func();
    if(p_check_func != NULL) {
        rval = p_check_func->mono_fst_shp_coring(p_mono_fst_shp_coring);
    }
    if(rval == IK_OK) {
        rval |= img_ctx_get_context(context_id, &p_ctx);
        if((p_mono_fst_shp_coring != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
            (void)amba_ik_system_lock_mutex(&p_ctx->organization.mutex, IK_WAIT_FOREVER);
            (void)amba_ik_system_memcpy(&p_ctx->filters.input_param.mono_fst_shp_coring, p_mono_fst_shp_coring, sizeof(ik_mono_fst_shp_coring_t));
            ctx_raise_update_flag(&p_ctx->filters.update_flags.iso.mono_fst_shp_coring_updated);
            (void)amba_ik_system_unlock_mutex(&p_ctx->organization.mutex);
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

uint32 img_ctx_ivd_set_mono_fst_shp_coring_idx_scale(uint32 context_id, const ik_mono_fst_shp_coring_idx_scale_t *p_mono_fst_shp_coring_idx_scale)
{
    amba_ik_system_print_str_5("[IK] set_mono_fst_shp_coring_idx_scale is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0U) {
        //MISRAC
    }
    if (p_mono_fst_shp_coring_idx_scale==NULL) {
        //MISRAC
    }
    return IK_ERR_0004;
}

uint32 img_ctx_set_mono_fst_shp_coring_idx_scale(uint32 context_id, const ik_mono_fst_shp_coring_idx_scale_t *p_mono_fst_shp_coring_idx_scale)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;
    const ik_debug_check_func_t* p_check_func = ik_ctx_get_debug_check_func();
    if(p_check_func != NULL) {
        rval = p_check_func->mono_fst_shp_coring_idx_scale(p_mono_fst_shp_coring_idx_scale);
    }
    if(rval == IK_OK) {
        rval |= img_ctx_get_context(context_id, &p_ctx);
        if((p_mono_fst_shp_coring_idx_scale != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
            (void)amba_ik_system_lock_mutex(&p_ctx->organization.mutex, IK_WAIT_FOREVER);
            (void)amba_ik_system_memcpy(&p_ctx->filters.input_param.mono_fst_shp_coring_idx_scale, p_mono_fst_shp_coring_idx_scale, sizeof(ik_mono_fst_shp_coring_idx_scale_t));
            ctx_raise_update_flag(&p_ctx->filters.update_flags.iso.mono_fst_shp_coring_idx_scale_updated);
            (void)amba_ik_system_unlock_mutex(&p_ctx->organization.mutex);
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

uint32 img_ctx_ivd_set_mono_fst_shp_min_coring_rslt(uint32 context_id, const ik_mono_fst_shp_min_coring_rslt_t *p_mono_fst_shp_min_coring_rslt)
{
    amba_ik_system_print_str_5("[IK] set_mono_fst_shp_min_coring_rslt is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0U) {
        //MISRAC
    }
    if (p_mono_fst_shp_min_coring_rslt==NULL) {
        //MISRAC
    }
    return IK_ERR_0004;
}

uint32 img_ctx_set_mono_fst_shp_min_coring_rslt(uint32 context_id, const ik_mono_fst_shp_min_coring_rslt_t *p_mono_fst_shp_min_coring_rslt)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;
    const ik_debug_check_func_t* p_check_func = ik_ctx_get_debug_check_func();
    if(p_check_func != NULL) {
        rval = p_check_func->mono_fst_shp_min_coring_rslt(p_mono_fst_shp_min_coring_rslt);
    }
    if(rval == IK_OK) {
        rval |= img_ctx_get_context(context_id, &p_ctx);
        if((p_mono_fst_shp_min_coring_rslt != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
            (void)amba_ik_system_lock_mutex(&p_ctx->organization.mutex, IK_WAIT_FOREVER);
            (void)amba_ik_system_memcpy(&p_ctx->filters.input_param.mono_fst_shp_min_coring_rslt, p_mono_fst_shp_min_coring_rslt, sizeof(ik_mono_fst_shp_min_coring_rslt_t));
            ctx_raise_update_flag(&p_ctx->filters.update_flags.iso.mono_fst_shp_min_coring_rslt_updated);
            (void)amba_ik_system_unlock_mutex(&p_ctx->organization.mutex);
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

uint32 img_ctx_ivd_set_mono_fst_shp_max_coring_rslt(uint32 context_id, const ik_mono_fst_shp_max_coring_rslt_t *p_mono_fst_shp_max_coring_rslt)
{
    amba_ik_system_print_str_5("[IK] set_mono_fst_shp_max_coring_rslt is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0U) {
        //MISRAC
    }
    if (p_mono_fst_shp_max_coring_rslt==NULL) {
        //MISRAC
    }
    return IK_ERR_0004;
}

uint32 img_ctx_set_mono_fst_shp_max_coring_rslt(uint32 context_id, const ik_mono_fst_shp_max_coring_rslt_t *p_mono_fst_shp_max_coring_rslt)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;
    const ik_debug_check_func_t* p_check_func = ik_ctx_get_debug_check_func();
    if(p_check_func != NULL) {
        rval = p_check_func->mono_fst_shp_max_coring_rslt(p_mono_fst_shp_max_coring_rslt);
    }
    if(rval == IK_OK) {
        rval |= img_ctx_get_context(context_id, &p_ctx);
        if((p_mono_fst_shp_max_coring_rslt != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
            (void)amba_ik_system_lock_mutex(&p_ctx->organization.mutex, IK_WAIT_FOREVER);
            (void)amba_ik_system_memcpy(&p_ctx->filters.input_param.mono_fst_shp_max_coring_rslt, p_mono_fst_shp_max_coring_rslt, sizeof(ik_mono_fst_shp_max_coring_rslt_t));
            ctx_raise_update_flag(&p_ctx->filters.update_flags.iso.mono_fst_shp_max_coring_rslt_updated);
            (void)amba_ik_system_unlock_mutex(&p_ctx->organization.mutex);
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

uint32 img_ctx_ivd_set_mono_fst_shp_scale_coring(uint32 context_id, const ik_mono_fst_shp_scale_coring_t *p_mono_fst_shp_scale_coring)
{
    amba_ik_system_print_str_5("[IK] set_mono_fst_shp_scale_coring is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0U) {
        //MISRAC
    }
    if (p_mono_fst_shp_scale_coring==NULL) {
        //MISRAC
    }
    return IK_ERR_0004;
}

uint32 img_ctx_set_mono_fst_shp_scale_coring(uint32 context_id, const ik_mono_fst_shp_scale_coring_t *p_mono_fst_shp_scale_coring)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;
    const ik_debug_check_func_t* p_check_func = ik_ctx_get_debug_check_func();
    if(p_check_func != NULL) {
        rval = p_check_func->mono_fst_shp_scale_coring(p_mono_fst_shp_scale_coring);
    }
    if(rval == IK_OK) {
        rval |= img_ctx_get_context(context_id, &p_ctx);
        if((p_mono_fst_shp_scale_coring != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
            (void)amba_ik_system_lock_mutex(&p_ctx->organization.mutex, IK_WAIT_FOREVER);
            (void)amba_ik_system_memcpy(&p_ctx->filters.input_param.mono_fst_shp_scale_coring, p_mono_fst_shp_scale_coring, sizeof(ik_mono_fst_shp_scale_coring_t));
            ctx_raise_update_flag(&p_ctx->filters.update_flags.iso.mono_fst_shp_scale_coring_updated);
            (void)amba_ik_system_unlock_mutex(&p_ctx->organization.mutex);
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

uint32 img_ctx_ivd_set_mono_exp0_fe_wb_gain(uint32 context_id, const ik_mono_exp0_fe_wb_gain_t *p_mono_exp0_fe_wb_gain)
{
    amba_ik_system_print_str_5("[IK] set_mono_exp0_fe_wb_gain is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0U) {
        //MISRAC
    }
    if (p_mono_exp0_fe_wb_gain==NULL) {
        //MISRAC
    }
    return IK_ERR_0004;
}

uint32 img_ctx_set_mono_exp0_fe_wb_gain(uint32 context_id, const ik_mono_exp0_fe_wb_gain_t *p_mono_exp0_fe_wb_gain)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;
    const ik_debug_check_func_t* p_check_func = ik_ctx_get_debug_check_func();
    if(p_check_func != NULL) {
        rval = p_check_func->mono_exp0_fe_wb_gain(p_mono_exp0_fe_wb_gain);
    }
    if(rval == IK_OK) {
        rval |= img_ctx_get_context(context_id, &p_ctx);
        if((p_mono_exp0_fe_wb_gain != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
            (void)amba_ik_system_lock_mutex(&p_ctx->organization.mutex, IK_WAIT_FOREVER);
            (void)amba_ik_system_memcpy(&p_ctx->filters.input_param.mono_exp0_fe_wb_gain, p_mono_exp0_fe_wb_gain, sizeof(ik_mono_exp0_fe_wb_gain_t));
            ctx_raise_update_flag(&p_ctx->filters.update_flags.iso.mono_exp0_fe_wb_gain_updated);
            (void)amba_ik_system_unlock_mutex(&p_ctx->organization.mutex);
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

uint32 img_ctx_ivd_set_mono_exp0_fe_static_blc_level(uint32 context_id, const ik_mono_exp0_fe_static_blc_level_t *p_mono_exp0_fe_static_blc_level)
{
    amba_ik_system_print_str_5("[IK] set_mono_exp0_fe_static_blc_level is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0U) {
        //MISRAC
    }
    if (p_mono_exp0_fe_static_blc_level==NULL) {
        //MISRAC
    }
    return IK_ERR_0004;
}

uint32 img_ctx_set_mono_exp0_fe_static_blc_level(uint32 context_id, const ik_mono_exp0_fe_static_blc_level_t *p_mono_exp0_fe_static_blc_level)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;
    const ik_debug_check_func_t* p_check_func = ik_ctx_get_debug_check_func();
    if(p_check_func != NULL) {
        rval = p_check_func->mono_exp0_fe_static_blc_level(p_mono_exp0_fe_static_blc_level);
    }
    if(rval == IK_OK) {
        rval |= img_ctx_get_context(context_id, &p_ctx);
        if((p_mono_exp0_fe_static_blc_level != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
            (void)amba_ik_system_lock_mutex(&p_ctx->organization.mutex, IK_WAIT_FOREVER);
            (void)amba_ik_system_memcpy(&p_ctx->filters.input_param.mono_exp0_fe_static_blc_level, p_mono_exp0_fe_static_blc_level, sizeof(ik_mono_exp0_fe_static_blc_level_t));
            ctx_raise_update_flag(&p_ctx->filters.update_flags.iso.mono_exp0_fe_static_blc_level_updated);
            (void)amba_ik_system_unlock_mutex(&p_ctx->organization.mutex);
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

uint32 img_ctx_ivd_set_mono_ce(uint32 context_id, const ik_mono_ce_t *p_mono_ce)
{
    amba_ik_system_print_str_5("[IK] set_mono_ce is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0U) {
        //MISRAC
    }
    if (p_mono_ce==NULL) {
        //MISRAC
    }
    return IK_ERR_0004;
}

uint32 img_ctx_set_mono_ce(uint32 context_id, const ik_mono_ce_t *p_mono_ce)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;
    const ik_debug_check_func_t* p_check_func = ik_ctx_get_debug_check_func();
    if(p_check_func != NULL) {
        rval = p_check_func->mono_ce(p_mono_ce);
    }
    if(rval == IK_OK) {
        rval |= img_ctx_get_context(context_id, &p_ctx);
        if((p_mono_ce != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
            (void)amba_ik_system_lock_mutex(&p_ctx->organization.mutex, IK_WAIT_FOREVER);
            (void)amba_ik_system_memcpy(&p_ctx->filters.input_param.mono_ce, p_mono_ce, sizeof(ik_mono_ce_t));
            ctx_raise_update_flag(&p_ctx->filters.update_flags.iso.mono_ce_updated);
            (void)amba_ik_system_unlock_mutex(&p_ctx->organization.mutex);
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

uint32 img_ctx_ivd_set_mono_ce_input_table(uint32 context_id, const ik_mono_ce_input_table_t *p_mono_ce_input_table)
{
    amba_ik_system_print_str_5("[IK] set_mono_ce_input_table is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0U) {
        //MISRAC
    }
    if (p_mono_ce_input_table==NULL) {
        //MISRAC
    }
    return IK_ERR_0004;
}

uint32 img_ctx_set_mono_ce_input_table(uint32 context_id, const ik_mono_ce_input_table_t *p_mono_ce_input_table)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;
    const ik_debug_check_func_t* p_check_func = ik_ctx_get_debug_check_func();
    if(p_check_func != NULL) {
        rval = p_check_func->mono_ce_input_table(p_mono_ce_input_table);
    }
    if(rval == IK_OK) {
        rval |= img_ctx_get_context(context_id, &p_ctx);
        if((p_mono_ce_input_table != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
            (void)amba_ik_system_lock_mutex(&p_ctx->organization.mutex, IK_WAIT_FOREVER);
            (void)amba_ik_system_memcpy(&p_ctx->filters.input_param.mono_ce_input_table, p_mono_ce_input_table, sizeof(ik_mono_ce_input_table_t));
            ctx_raise_update_flag(&p_ctx->filters.update_flags.iso.mono_ce_input_table_updated);
            (void)amba_ik_system_unlock_mutex(&p_ctx->organization.mutex);
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

uint32 img_ctx_ivd_set_mono_ce_output_table(uint32 context_id, const ik_mono_ce_output_table_t *p_mono_ce_output_table)
{
    amba_ik_system_print_str_5("[IK] set_mono_ce_output_table is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0U) {
        //MISRAC
    }
    if (p_mono_ce_output_table==NULL) {
        //MISRAC
    }
    return IK_ERR_0004;
}

uint32 img_ctx_set_mono_ce_output_table(uint32 context_id, const ik_mono_ce_output_table_t *p_mono_ce_output_table)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;
    const ik_debug_check_func_t* p_check_func = ik_ctx_get_debug_check_func();
    if(p_check_func != NULL) {
        rval = p_check_func->mono_ce_output_table(p_mono_ce_output_table);
    }
    if(rval == IK_OK) {
        rval |= img_ctx_get_context(context_id, &p_ctx);
        if((p_mono_ce_output_table != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
            (void)amba_ik_system_lock_mutex(&p_ctx->organization.mutex, IK_WAIT_FOREVER);
            (void)amba_ik_system_memcpy(&p_ctx->filters.input_param.mono_ce_output_table, p_mono_ce_output_table, sizeof(ik_mono_ce_output_table_t));
            ctx_raise_update_flag(&p_ctx->filters.update_flags.iso.mono_ce_output_table_updated);
            (void)amba_ik_system_unlock_mutex(&p_ctx->organization.mutex);
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

uint32 img_ctx_ivd_set_fusion(uint32 context_id, const ik_fusion_t *p_fusion)
{
    amba_ik_system_print_str_5("[IK] set_fusion is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0U) {
        //MISRAC
    }
    if (p_fusion==NULL) {
        //MISRAC
    }
    return IK_ERR_0004;
}

uint32 img_ctx_set_fusion(uint32 context_id, const ik_fusion_t *p_fusion)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;
    const ik_debug_check_func_t* p_check_func = ik_ctx_get_debug_check_func();
    if(p_check_func != NULL) {
        rval = p_check_func->fusion(p_fusion);
    }
    if(rval == IK_OK) {
        rval |= img_ctx_get_context(context_id, &p_ctx);
        if((p_fusion != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
            (void)amba_ik_system_lock_mutex(&p_ctx->organization.mutex, IK_WAIT_FOREVER);
            (void)amba_ik_system_memcpy(&p_ctx->filters.input_param.fusion, p_fusion, sizeof(ik_fusion_t));
            ctx_raise_update_flag(&p_ctx->filters.update_flags.iso.fusion_updated);
            (void)amba_ik_system_unlock_mutex(&p_ctx->organization.mutex);
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
    if (context_id != 0U) {
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
        (void)amba_ik_system_lock_mutex(&p_ctx->organization.mutex, IK_WAIT_FOREVER);
        (void)amba_ik_system_memcpy(p_sensor_info, &p_ctx->filters.input_param.sensor_info, sizeof(ik_vin_sensor_info_t));
        (void)amba_ik_system_unlock_mutex(&p_ctx->organization.mutex);
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

uint32 img_ctx_ivd_get_yuv_mode(const uint32 context_id, uint32 *p_yuv_mode)
{
    amba_ik_system_print_str_5("[IK] get_yuv_mode() is invalid!", DC_S, DC_S, DC_S, DC_S, DC_S);
    if (context_id == 0U) {
        //TBD
    }
    if (p_yuv_mode != NULL) {
        //TBD
        *p_yuv_mode = 0U;
    }
    return IK_ERR_0004;
}

uint32 img_ctx_get_yuv_mode(const uint32 context_id, uint32 *p_yuv_mode)
{
    amba_ik_context_entity_t *p_ctx = NULL;
    uint32 rval;

    if (p_yuv_mode == NULL) {
        rval = IK_ERR_0005;
    } else {
        rval = img_ctx_get_context(context_id, &p_ctx);
        if (rval == IK_OK) {
            (void)amba_ik_system_lock_mutex(&p_ctx->organization.mutex, IK_WAIT_FOREVER);
            (void)amba_ik_system_memcpy(p_yuv_mode, &p_ctx->filters.input_param.yuv_mode, sizeof(uint32));
            (void)amba_ik_system_unlock_mutex(&p_ctx->organization.mutex);
        }
    }
    return rval;
}

uint32 img_ctx_ivd_get_before_ce_wb_gain(uint32 context_id, ik_wb_gain_t *p_before_ce_wb_gain)
{
    amba_ik_system_print_str_5("[IK] get_before_ce_wb_gain is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0U) {
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
        (void)amba_ik_system_lock_mutex(&p_ctx->organization.mutex, IK_WAIT_FOREVER);
        (void)amba_ik_system_memcpy(p_before_ce_wb_gain, &p_ctx->filters.input_param.before_ce_wb_gain, sizeof(ik_wb_gain_t));
        (void)amba_ik_system_unlock_mutex(&p_ctx->organization.mutex);
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

uint32 img_ctx_ivd_get_pre_cc_gain(uint32 context_id, ik_pre_cc_gain_t *p_pre_cc_gain)
{
    amba_ik_system_print_str_5("[IK] get_pre_cc_gain is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0U) {
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
        (void)amba_ik_system_lock_mutex(&p_ctx->organization.mutex, IK_WAIT_FOREVER);
        (void)amba_ik_system_memcpy(p_pre_cc_gain, &p_ctx->filters.input_param.pre_cc_gain, sizeof(ik_pre_cc_gain_t));
        (void)amba_ik_system_unlock_mutex(&p_ctx->organization.mutex);
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

uint32 img_ctx_ivd_get_after_ce_wb_gain(uint32 context_id, ik_wb_gain_t *p_after_ce_wb_gain)
{
    amba_ik_system_print_str_5("[IK] get_after_ce_wb_gain is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0U) {
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
        (void)amba_ik_system_lock_mutex(&p_ctx->organization.mutex, IK_WAIT_FOREVER);
        (void)amba_ik_system_memcpy(p_after_ce_wb_gain, &p_ctx->filters.input_param.after_ce_wb_gain, sizeof(ik_wb_gain_t));
        (void)amba_ik_system_unlock_mutex(&p_ctx->organization.mutex);
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

uint32 img_ctx_ivd_get_stored_ir(uint32 context_id, ik_stored_ir_t *p_stored_ir)
{
    amba_ik_system_print_str_5("[IK] get_stored_ir is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0U) {
        //TBD
    }
    if (p_stored_ir == NULL) {
        //TBD
    } else {
        (void)amba_ik_system_memcpy(p_stored_ir, p_stored_ir, sizeof(uint8));
    }
    return IK_ERR_0004;
}

uint32 img_ctx_get_stored_ir(uint32 context_id, ik_stored_ir_t *p_stored_ir)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval |= img_ctx_get_context(context_id, &p_ctx);
    if((p_stored_ir != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        (void)amba_ik_system_lock_mutex(&p_ctx->organization.mutex, IK_WAIT_FOREVER);
        (void)amba_ik_system_memcpy(p_stored_ir, &p_ctx->filters.input_param.stored_ir, sizeof(ik_stored_ir_t));
        (void)amba_ik_system_unlock_mutex(&p_ctx->organization.mutex);
    } else {
        if(p_stored_ir == NULL) {
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
    if (context_id != 0U) {
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
        (void)amba_ik_system_lock_mutex(&p_ctx->organization.mutex, IK_WAIT_FOREVER);
        (void)amba_ik_system_memcpy(p_cfa_leakage_filter, &p_ctx->filters.input_param.cfa_leakage_filter, sizeof(ik_cfa_leakage_filter_t));
        (void)amba_ik_system_unlock_mutex(&p_ctx->organization.mutex);
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
    if (context_id != 0U) {
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
        (void)amba_ik_system_lock_mutex(&p_ctx->organization.mutex, IK_WAIT_FOREVER);
        (void)amba_ik_system_memcpy(p_anti_aliasing, &p_ctx->filters.input_param.anti_aliasing, sizeof(ik_anti_aliasing_t));
        (void)amba_ik_system_unlock_mutex(&p_ctx->organization.mutex);
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
    if (context_id != 0U) {
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
        (void)amba_ik_system_lock_mutex(&p_ctx->organization.mutex, IK_WAIT_FOREVER);
        (void)amba_ik_system_memcpy(p_dynamic_bpc, &p_ctx->filters.input_param.dynamic_bpc, sizeof(ik_dynamic_bad_pixel_correction_t));
        (void)amba_ik_system_unlock_mutex(&p_ctx->organization.mutex);
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
    if (context_id != 0U) {
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
        (void)amba_ik_system_lock_mutex(&p_ctx->organization.mutex, IK_WAIT_FOREVER);
        (void)amba_ik_system_memcpy(p_cfa_noise_filter, &p_ctx->filters.input_param.cfa_noise_filter, sizeof(ik_cfa_noise_filter_t));
        (void)amba_ik_system_unlock_mutex(&p_ctx->organization.mutex);
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

uint32 img_ctx_ivd_get_demosaic(uint32 context_id, ik_demosaic_t *p_demosaic)
{
    amba_ik_system_print_str_5("[IK] get_demosaic is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0U) {
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
        (void)amba_ik_system_lock_mutex(&p_ctx->organization.mutex, IK_WAIT_FOREVER);
        (void)amba_ik_system_memcpy(p_demosaic, &p_ctx->filters.input_param.demosaic, sizeof(ik_demosaic_t));
        (void)amba_ik_system_unlock_mutex(&p_ctx->organization.mutex);
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

uint32 img_ctx_ivd_get_ext_raw_out_mode(uint32 context_id, uint32 *p_ext_raw_out_mode)
{
    amba_ik_system_print_str_5("[IK] get_ext_raw_out_mode() is invalid!", DC_S, DC_S, DC_S, DC_S, DC_S);
    if (context_id == 0UL) {
        //TBD
    }
    if (p_ext_raw_out_mode != NULL) {
        //TBD
        *p_ext_raw_out_mode = 0U;
    }
    return IK_ERR_0004;
}

uint32 img_ctx_get_ext_raw_out_mode(uint32 context_id, uint32 *p_ext_raw_out_mode)
{
    amba_ik_context_entity_t *p_ctx = NULL;
    uint32 rval = IK_OK;

    rval = img_ctx_get_context(context_id, &p_ctx);
    (void)amba_ik_system_lock_mutex(&p_ctx->organization.mutex, IK_WAIT_FOREVER);
    (void)amba_ik_system_memcpy(p_ext_raw_out_mode, &p_ctx->filters.input_param.ext_raw_out_mode, sizeof(uint32));
    (void)amba_ik_system_unlock_mutex(&p_ctx->organization.mutex);

    return rval;
}

uint32 img_ctx_ivd_get_rgb_to_12y(uint32 context_id, ik_rgb_to_12y_t *p_rgb_to_12y)
{
    amba_ik_system_print_str_5("[IK] get_rgb_to_12y is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0U) {
        //TBD
    }
    if (p_rgb_to_12y == NULL) {
        //TBD
    } else {
        (void)amba_ik_system_memcpy(p_rgb_to_12y, p_rgb_to_12y, sizeof(uint8));
    }
    return IK_ERR_0004;
}

uint32 img_ctx_get_rgb_to_12y(uint32 context_id, ik_rgb_to_12y_t *p_rgb_to_12y)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval |= img_ctx_get_context(context_id, &p_ctx);
    if((p_rgb_to_12y != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        (void)amba_ik_system_lock_mutex(&p_ctx->organization.mutex, IK_WAIT_FOREVER);
        (void)amba_ik_system_memcpy(p_rgb_to_12y, &p_ctx->filters.input_param.rgb_to_12y, sizeof(ik_rgb_to_12y_t));
        (void)amba_ik_system_unlock_mutex(&p_ctx->organization.mutex);
    } else {
        if(p_rgb_to_12y == NULL) {
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

uint32 img_ctx_ivd_get_luma_noise_reduction(uint32 context_id, ik_luma_noise_reduction_t *p_luma_noise_reduce)
{
    amba_ik_system_print_str_5("[IK] get_luma_noise_reduction is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0U) {
        //TBD
    }
    if (p_luma_noise_reduce == NULL) {
        //TBD
    } else {
        (void)amba_ik_system_memcpy(p_luma_noise_reduce, p_luma_noise_reduce, sizeof(uint8));
    }
    return IK_ERR_0004;
}

uint32 img_ctx_get_luma_noise_reduction(uint32 context_id, ik_luma_noise_reduction_t *p_luma_noise_reduce)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval |= img_ctx_get_context(context_id, &p_ctx);
    if((p_luma_noise_reduce != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        (void)amba_ik_system_lock_mutex(&p_ctx->organization.mutex, IK_WAIT_FOREVER);
        (void)amba_ik_system_memcpy(p_luma_noise_reduce, &p_ctx->filters.input_param.luma_noise_reduce, sizeof(ik_luma_noise_reduction_t));
        (void)amba_ik_system_unlock_mutex(&p_ctx->organization.mutex);
    } else {
        if(p_luma_noise_reduce == NULL) {
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

uint32 img_ctx_ivd_get_hi_luma_noise_reduction(uint32 context_id, ik_luma_noise_reduction_t *p_hi_luma_noise_reduce)
{
    amba_ik_system_print_str_5("[IK] get_hi_luma_noise_reduction is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0U) {
        //TBD
    }
    if (p_hi_luma_noise_reduce == NULL) {
        //TBD
    } else {
        (void)amba_ik_system_memcpy(p_hi_luma_noise_reduce, p_hi_luma_noise_reduce, sizeof(uint8));
    }
    return IK_ERR_0004;
}

uint32 img_ctx_get_hi_luma_noise_reduction(uint32 context_id, ik_luma_noise_reduction_t *p_hi_luma_noise_reduce)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval |= img_ctx_get_context(context_id, &p_ctx);
    if((p_hi_luma_noise_reduce != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        (void)amba_ik_system_lock_mutex(&p_ctx->organization.mutex, IK_WAIT_FOREVER);
        (void)amba_ik_system_memcpy(p_hi_luma_noise_reduce, &p_ctx->filters.input_param.hi_luma_noise_reduce, sizeof(ik_luma_noise_reduction_t));
        (void)amba_ik_system_unlock_mutex(&p_ctx->organization.mutex);
    } else {
        if(p_hi_luma_noise_reduce == NULL) {
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

uint32 img_ctx_ivd_get_li2_luma_noise_reduction(uint32 context_id, ik_luma_noise_reduction_t *p_li2_luma_noise_reduce)
{
    amba_ik_system_print_str_5("[IK] get_li2_luma_noise_reduction is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0U) {
        //TBD
    }
    if (p_li2_luma_noise_reduce == NULL) {
        //TBD
    } else {
        (void)amba_ik_system_memcpy(p_li2_luma_noise_reduce, p_li2_luma_noise_reduce, sizeof(uint8));
    }
    return IK_ERR_0004;
}

uint32 img_ctx_get_li2_luma_noise_reduction(uint32 context_id, ik_luma_noise_reduction_t *p_li2_luma_noise_reduce)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval |= img_ctx_get_context(context_id, &p_ctx);
    if((p_li2_luma_noise_reduce != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        (void)amba_ik_system_lock_mutex(&p_ctx->organization.mutex, IK_WAIT_FOREVER);
        (void)amba_ik_system_memcpy(p_li2_luma_noise_reduce, &p_ctx->filters.input_param.li2_luma_noise_reduce, sizeof(ik_luma_noise_reduction_t));
        (void)amba_ik_system_unlock_mutex(&p_ctx->organization.mutex);
    } else {
        if(p_li2_luma_noise_reduce == NULL) {
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
    if (context_id != 0U) {
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
        (void)amba_ik_system_lock_mutex(&p_ctx->organization.mutex, IK_WAIT_FOREVER);
        (void)amba_ik_system_memcpy(p_color_correction, &p_ctx->filters.input_param.color_correction, sizeof(ik_color_correction_t));
        (void)amba_ik_system_unlock_mutex(&p_ctx->organization.mutex);
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

uint32 img_ctx_ivd_get_tone_curve(uint32 context_id, ik_tone_curve_t *p_tone_curve)
{
    amba_ik_system_print_str_5("[IK] get_tone_curve is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0U) {
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
        (void)amba_ik_system_lock_mutex(&p_ctx->organization.mutex, IK_WAIT_FOREVER);
        (void)amba_ik_system_memcpy(p_tone_curve, &p_ctx->filters.input_param.tone_curve, sizeof(ik_tone_curve_t));
        (void)amba_ik_system_unlock_mutex(&p_ctx->organization.mutex);
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
    if (context_id != 0U) {
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
        (void)amba_ik_system_lock_mutex(&p_ctx->organization.mutex, IK_WAIT_FOREVER);
        (void)amba_ik_system_memcpy(p_rgb_to_yuv_matrix, &p_ctx->filters.input_param.rgb_to_yuv_matrix, sizeof(ik_rgb_to_yuv_matrix_t));
        (void)amba_ik_system_unlock_mutex(&p_ctx->organization.mutex);
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
    if (context_id != 0U) {
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
        (void)amba_ik_system_lock_mutex(&p_ctx->organization.mutex, IK_WAIT_FOREVER);
        (void)amba_ik_system_memcpy(p_rgb_ir, &p_ctx->filters.input_param.rgb_ir, sizeof(ik_rgb_ir_t));
        (void)amba_ik_system_unlock_mutex(&p_ctx->organization.mutex);
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
    if (context_id != 0U) {
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
        (void)amba_ik_system_lock_mutex(&p_ctx->organization.mutex, IK_WAIT_FOREVER);
        (void)amba_ik_system_memcpy(p_chroma_scale, &p_ctx->filters.input_param.chroma_scale, sizeof(ik_chroma_scale_t));
        (void)amba_ik_system_unlock_mutex(&p_ctx->organization.mutex);
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
    if (context_id != 0U) {
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
        (void)amba_ik_system_lock_mutex(&p_ctx->organization.mutex, IK_WAIT_FOREVER);
        (void)amba_ik_system_memcpy(p_chroma_median_filter, &p_ctx->filters.input_param.chroma_median_filter, sizeof(ik_chroma_median_filter_t));
        (void)amba_ik_system_unlock_mutex(&p_ctx->organization.mutex);
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
    if (context_id != 0U) {
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
        (void)amba_ik_system_lock_mutex(&p_ctx->organization.mutex, IK_WAIT_FOREVER);
        (void)amba_ik_system_memcpy(p_first_luma_process_mode, &p_ctx->filters.input_param.first_luma_process_mode, sizeof(ik_first_luma_process_mode_t));
        (void)amba_ik_system_unlock_mutex(&p_ctx->organization.mutex);
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
    if (context_id != 0U) {
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
        (void)amba_ik_system_lock_mutex(&p_ctx->organization.mutex, IK_WAIT_FOREVER);
        (void)amba_ik_system_memcpy(p_advance_spatial_filter, &p_ctx->filters.input_param.advance_spatial_filter, sizeof(ik_adv_spatial_filter_t));
        (void)amba_ik_system_unlock_mutex(&p_ctx->organization.mutex);
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
    if (context_id != 0U) {
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
        (void)amba_ik_system_lock_mutex(&p_ctx->organization.mutex, IK_WAIT_FOREVER);
        (void)amba_ik_system_memcpy(p_first_sharpen_both, &p_ctx->filters.input_param.first_sharpen_both, sizeof(ik_first_sharpen_both_t));
        (void)amba_ik_system_unlock_mutex(&p_ctx->organization.mutex);
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
    if (context_id != 0U) {
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
        (void)amba_ik_system_lock_mutex(&p_ctx->organization.mutex, IK_WAIT_FOREVER);
        (void)amba_ik_system_memcpy(p_first_sharpen_noise, &p_ctx->filters.input_param.first_sharpen_noise, sizeof(ik_first_sharpen_noise_t));
        (void)amba_ik_system_unlock_mutex(&p_ctx->organization.mutex);
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
    if (context_id != 0U) {
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
        (void)amba_ik_system_lock_mutex(&p_ctx->organization.mutex, IK_WAIT_FOREVER);
        (void)amba_ik_system_memcpy(p_first_sharpen_fir, &p_ctx->filters.input_param.first_sharpen_fir, sizeof(ik_first_sharpen_fir_t));
        (void)amba_ik_system_unlock_mutex(&p_ctx->organization.mutex);
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
    if (context_id != 0U) {
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
        (void)amba_ik_system_lock_mutex(&p_ctx->organization.mutex, IK_WAIT_FOREVER);
        (void)amba_ik_system_memcpy(p_first_sharpen_coring, &p_ctx->filters.input_param.first_sharpen_coring, sizeof(ik_first_sharpen_coring_t));
        (void)amba_ik_system_unlock_mutex(&p_ctx->organization.mutex);
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
    if (context_id != 0U) {
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
        (void)amba_ik_system_lock_mutex(&p_ctx->organization.mutex, IK_WAIT_FOREVER);
        (void)amba_ik_system_memcpy(p_fstshpns_coring_index_scale, &p_ctx->filters.input_param.first_sharpen_coring_idx_scale, sizeof(ik_first_sharpen_coring_idx_scale_t));
        (void)amba_ik_system_unlock_mutex(&p_ctx->organization.mutex);
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
    if (context_id != 0U) {
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
        (void)amba_ik_system_lock_mutex(&p_ctx->organization.mutex, IK_WAIT_FOREVER);
        (void)amba_ik_system_memcpy(p_fstshpns_min_coring_result, &p_ctx->filters.input_param.first_sharpen_min_coring_result, sizeof(ik_first_sharpen_min_coring_result_t));
        (void)amba_ik_system_unlock_mutex(&p_ctx->organization.mutex);
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
    if (context_id != 0U) {
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
        (void)amba_ik_system_lock_mutex(&p_ctx->organization.mutex, IK_WAIT_FOREVER);
        (void)amba_ik_system_memcpy(p_fstshpns_max_coring_result, &p_ctx->filters.input_param.first_sharpen_max_coring_result, sizeof(ik_first_sharpen_max_coring_result_t));
        (void)amba_ik_system_unlock_mutex(&p_ctx->organization.mutex);
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
    if (context_id != 0U) {
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
        (void)amba_ik_system_lock_mutex(&p_ctx->organization.mutex, IK_WAIT_FOREVER);
        (void)amba_ik_system_memcpy(p_fstshpns_scale_coring, &p_ctx->filters.input_param.first_sharpen_scale_coring, sizeof(ik_first_sharpen_scale_coring_t));
        (void)amba_ik_system_unlock_mutex(&p_ctx->organization.mutex);
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
    if (context_id != 0U) {
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
        (void)amba_ik_system_lock_mutex(&p_ctx->organization.mutex, IK_WAIT_FOREVER);
        (void)amba_ik_system_memcpy(p_final_sharpen_both, &p_ctx->filters.input_param.final_sharpen_both, sizeof(ik_final_sharpen_both_t));
        (void)amba_ik_system_unlock_mutex(&p_ctx->organization.mutex);
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
    if (context_id != 0U) {
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
        (void)amba_ik_system_lock_mutex(&p_ctx->organization.mutex, IK_WAIT_FOREVER);
        (void)amba_ik_system_memcpy(p_final_sharpen_noise, &p_ctx->filters.input_param.final_sharpen_noise, sizeof(ik_final_sharpen_noise_t));
        (void)amba_ik_system_unlock_mutex(&p_ctx->organization.mutex);
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
    if (context_id != 0U) {
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
        (void)amba_ik_system_lock_mutex(&p_ctx->organization.mutex, IK_WAIT_FOREVER);
        (void)amba_ik_system_memcpy(p_final_sharpen_fir, &p_ctx->filters.input_param.final_sharpen_fir, sizeof(ik_final_sharpen_fir_t));
        (void)amba_ik_system_unlock_mutex(&p_ctx->organization.mutex);
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
    if (context_id != 0U) {
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
        (void)amba_ik_system_lock_mutex(&p_ctx->organization.mutex, IK_WAIT_FOREVER);
        (void)amba_ik_system_memcpy(p_final_sharpen_coring, &p_ctx->filters.input_param.final_sharpen_coring, sizeof(ik_final_sharpen_coring_t));
        (void)amba_ik_system_unlock_mutex(&p_ctx->organization.mutex);
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
    if (context_id != 0U) {
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
        (void)amba_ik_system_lock_mutex(&p_ctx->organization.mutex, IK_WAIT_FOREVER);
        (void)amba_ik_system_memcpy(p_fnlshpns_coring_index_scale, &p_ctx->filters.input_param.final_sharpen_coring_idx_scale, sizeof(ik_final_sharpen_coring_idx_scale_t));
        (void)amba_ik_system_unlock_mutex(&p_ctx->organization.mutex);
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
    if (context_id != 0U) {
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
        (void)amba_ik_system_lock_mutex(&p_ctx->organization.mutex, IK_WAIT_FOREVER);
        (void)amba_ik_system_memcpy(p_fnlshpns_min_coring_result, &p_ctx->filters.input_param.final_sharpen_min_coring_result, sizeof(ik_final_sharpen_min_coring_result_t));
        (void)amba_ik_system_unlock_mutex(&p_ctx->organization.mutex);
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
    if (context_id != 0U) {
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
        (void)amba_ik_system_lock_mutex(&p_ctx->organization.mutex, IK_WAIT_FOREVER);
        (void)amba_ik_system_memcpy(p_fnlshpns_max_coring_result, &p_ctx->filters.input_param.final_sharpen_max_coring_result, sizeof(ik_final_sharpen_max_coring_result_t));
        (void)amba_ik_system_unlock_mutex(&p_ctx->organization.mutex);
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
    if (context_id != 0U) {
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
        (void)amba_ik_system_lock_mutex(&p_ctx->organization.mutex, IK_WAIT_FOREVER);
        (void)amba_ik_system_memcpy(p_fnlshpns_scale_coring, &p_ctx->filters.input_param.final_sharpen_scale_coring, sizeof(ik_final_sharpen_scale_coring_t));
        (void)amba_ik_system_unlock_mutex(&p_ctx->organization.mutex);
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
    if (context_id != 0U) {
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
        (void)amba_ik_system_lock_mutex(&p_ctx->organization.mutex, IK_WAIT_FOREVER);
        (void)amba_ik_system_memcpy(p_final_sharpen_both_three_d_table, &p_ctx->filters.input_param.final_sharpen_both_three_d_table, sizeof(ik_final_sharpen_both_three_d_table_t));
        (void)amba_ik_system_unlock_mutex(&p_ctx->organization.mutex);
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
    if (context_id != 0U) {
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
        (void)amba_ik_system_lock_mutex(&p_ctx->organization.mutex, IK_WAIT_FOREVER);
        (void)amba_ik_system_memcpy(p_chroma_filter, &p_ctx->filters.input_param.chroma_filter, sizeof(ik_chroma_filter_t));
        (void)amba_ik_system_unlock_mutex(&p_ctx->organization.mutex);
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

uint32 img_ctx_ivd_get_wide_chroma_ft(uint32 context_id, ik_wide_chroma_filter_t *p_wide_chroma_filter)
{
    amba_ik_system_print_str_5("[IK] get_wide_chroma_filter is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0U) {
        //TBD
    }
    if (p_wide_chroma_filter == NULL) {
        //TBD
    } else {
        (void)amba_ik_system_memcpy(p_wide_chroma_filter, p_wide_chroma_filter, sizeof(uint8));
    }
    return IK_ERR_0004;
}

uint32 img_ctx_get_wide_chroma_filter(uint32 context_id, ik_wide_chroma_filter_t *p_wide_chroma_filter)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval |= img_ctx_get_context(context_id, &p_ctx);
    if((p_wide_chroma_filter != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        (void)amba_ik_system_lock_mutex(&p_ctx->organization.mutex, IK_WAIT_FOREVER);
        (void)amba_ik_system_memcpy(p_wide_chroma_filter, &p_ctx->filters.input_param.wide_chroma_filter, sizeof(ik_wide_chroma_filter_t));
        (void)amba_ik_system_unlock_mutex(&p_ctx->organization.mutex);
    } else {
        if(p_wide_chroma_filter == NULL) {
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

uint32 img_ctx_ivd_get_wide_chroma_ft_combine(uint32 context_id, ik_wide_chroma_filter_combine_t *p_chroma_filter_combine)
{
    amba_ik_system_print_str_5("[IK] get_wide_chroma_filter is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0U) {
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
        (void)amba_ik_system_lock_mutex(&p_ctx->organization.mutex, IK_WAIT_FOREVER);
        (void)amba_ik_system_memcpy(p_chroma_filter_combine, &p_ctx->filters.input_param.wide_chroma_combine, sizeof(ik_wide_chroma_filter_combine_t));
        (void)amba_ik_system_unlock_mutex(&p_ctx->organization.mutex);
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

uint32 img_ctx_ivd_get_grgb_mismatch(uint32 context_id, ik_grgb_mismatch_t *p_grgb_mismatch)
{
    amba_ik_system_print_str_5("[IK] grgb_mismatch is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0U) {
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
        (void)amba_ik_system_lock_mutex(&p_ctx->organization.mutex, IK_WAIT_FOREVER);
        (void)amba_ik_system_memcpy(p_grgb_mismatch, &p_ctx->filters.input_param.grgb_mismatch, sizeof(ik_grgb_mismatch_t));
        (void)amba_ik_system_unlock_mutex(&p_ctx->organization.mutex);
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
    if (context_id != 0U) {
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
        (void)amba_ik_system_lock_mutex(&p_ctx->organization.mutex, IK_WAIT_FOREVER);
        (void)amba_ik_system_memcpy(p_video_mctf, &p_ctx->filters.input_param.video_mctf, sizeof(ik_video_mctf_t));
        (void)amba_ik_system_unlock_mutex(&p_ctx->organization.mutex);
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
    if (context_id != 0U) {
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
        (void)amba_ik_system_lock_mutex(&p_ctx->organization.mutex, IK_WAIT_FOREVER);
        (void)amba_ik_system_memcpy(p_video_mctf_ta, &p_ctx->filters.input_param.video_mctf_ta, sizeof(ik_video_mctf_ta_t));
        (void)amba_ik_system_unlock_mutex(&p_ctx->organization.mutex);
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
    if (context_id != 0U) {
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
        (void)amba_ik_system_lock_mutex(&p_ctx->organization.mutex, IK_WAIT_FOREVER);
        (void)amba_ik_system_memcpy(p_video_mctf_and_final_sharpen, &p_ctx->filters.input_param.video_mctf_and_final_sharpen, sizeof(ik_pos_dep33_t));
        (void)amba_ik_system_unlock_mutex(&p_ctx->organization.mutex);
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

uint32 img_ctx_ivd_get_aaa_stat_info(uint32 context_id, ik_aaa_stat_info_t *p_stat_info)
{
    amba_ik_system_print_str_5("[IK] get_aaa_stat_info is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0U) {
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
        (void)amba_ik_system_lock_mutex(&p_ctx->organization.mutex, IK_WAIT_FOREVER);
        (void)amba_ik_system_memcpy(p_stat_info, &p_ctx->filters.input_param.aaa_stat_info, sizeof(ik_aaa_stat_info_t));
        (void)amba_ik_system_unlock_mutex(&p_ctx->organization.mutex);
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

uint32 img_ctx_ivd_get_aaa_pg_af_info(uint32 context_id, ik_aaa_pg_af_stat_info_t *p_pg_af_stat_info)
{
    amba_ik_system_print_str_5("[IK] get_aaa_pg_af_info is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0U) {
        //TBD
    }
    if (p_pg_af_stat_info == NULL) {
        //TBD
    } else {
        (void)amba_ik_system_memcpy(p_pg_af_stat_info, p_pg_af_stat_info, sizeof(uint8));
    }
    return IK_ERR_0004;
}

uint32 img_ctx_get_aaa_pg_af_info(uint32 context_id, ik_aaa_pg_af_stat_info_t *p_pg_af_stat_info)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval |= img_ctx_get_context(context_id, &p_ctx);
    if((p_pg_af_stat_info != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        (void)amba_ik_system_lock_mutex(&p_ctx->organization.mutex, IK_WAIT_FOREVER);
        (void)amba_ik_system_memcpy(p_pg_af_stat_info, &p_ctx->filters.input_param.aaa_pg_stat_info, sizeof(ik_aaa_pg_af_stat_info_t));
        (void)amba_ik_system_unlock_mutex(&p_ctx->organization.mutex);
    } else {
        if(p_pg_af_stat_info == NULL) {
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
    if (context_id != 0U) {
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
        (void)amba_ik_system_lock_mutex(&p_ctx->organization.mutex, IK_WAIT_FOREVER);
        (void)amba_ik_system_memcpy(p_af_stat_ex_info, &p_ctx->filters.input_param.af_stat_ex_info, sizeof(ik_af_stat_ex_info_t));
        (void)amba_ik_system_unlock_mutex(&p_ctx->organization.mutex);
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
    if (context_id != 0U) {
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
        (void)amba_ik_system_lock_mutex(&p_ctx->organization.mutex, IK_WAIT_FOREVER);
        (void)amba_ik_system_memcpy(p_pg_af_stat_ex_info, &p_ctx->filters.input_param.pg_af_stat_ex_info, sizeof(ik_pg_af_stat_ex_info_t));
        (void)amba_ik_system_unlock_mutex(&p_ctx->organization.mutex);
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

uint32 img_ctx_ivd_get_window_size_info(uint32 context_id, ik_window_size_info_t *p_window_size_info)
{
    amba_ik_system_print_str_5("[IK] get_window_size_info is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0U) {
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
        (void)amba_ik_system_lock_mutex(&p_ctx->organization.mutex, IK_WAIT_FOREVER);
        (void)amba_ik_system_memcpy(p_window_size_info, &p_ctx->filters.input_param.window_size_info, sizeof(ik_window_size_info_t));
        (void)amba_ik_system_unlock_mutex(&p_ctx->organization.mutex);
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
    if (context_id != 0U) {
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
        (void)amba_ik_system_lock_mutex(&p_ctx->organization.mutex, IK_WAIT_FOREVER);
        (void)amba_ik_system_memcpy(p_cfa_window_size_info, &p_ctx->filters.input_param.cfa_window_size_info, sizeof(ik_cfa_window_size_info_t));
        (void)amba_ik_system_unlock_mutex(&p_ctx->organization.mutex);
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

uint32 img_ctx_ivd_get_mctf_internal(uint32 context_id, ik_mctf_internal_t *p_mctf_internal)
{
    amba_ik_system_print_str_5("[IK] get_mctf_internal is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0U) {
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
        (void)amba_ik_system_lock_mutex(&p_ctx->organization.mutex, IK_WAIT_FOREVER);
        (void)amba_ik_system_memcpy(p_mctf_internal, &p_ctx->filters.input_param.internal_video_mctf, sizeof(ik_mctf_internal_t));
        (void)amba_ik_system_unlock_mutex(&p_ctx->organization.mutex);
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
    if (context_id != 0U) {
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
        (void)amba_ik_system_lock_mutex(&p_ctx->organization.mutex, IK_WAIT_FOREVER);
        (void)amba_ik_system_memcpy(p_motion_detect, &p_ctx->filters.input_param.motion_detect, sizeof(ik_motion_detect_t));
        (void)amba_ik_system_unlock_mutex(&p_ctx->organization.mutex);
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
    if (context_id != 0U) {
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
        (void)amba_ik_system_lock_mutex(&p_ctx->organization.mutex, IK_WAIT_FOREVER);
        (void)amba_ik_system_memcpy(p_pos_dep, &p_ctx->filters.input_param.motion_detect_pos_dep, sizeof(ik_pos_dep33_t));
        (void)amba_ik_system_unlock_mutex(&p_ctx->organization.mutex);
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
    if (context_id != 0U) {
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
        (void)amba_ik_system_lock_mutex(&p_ctx->organization.mutex, IK_WAIT_FOREVER);
        (void)amba_ik_system_memcpy(p_motion_detect_and_mctf, &p_ctx->filters.input_param.motion_detect_and_mctf, sizeof(ik_motion_detect_and_mctf_t));
        (void)amba_ik_system_unlock_mutex(&p_ctx->organization.mutex);
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
    amba_ik_system_print_str_5("[IK] get_fe_tone_curve is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0U) {
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
        (void)amba_ik_system_lock_mutex(&p_ctx->organization.mutex, IK_WAIT_FOREVER);
        (void)amba_ik_system_memcpy(p_fe_tc, &p_ctx->filters.input_param.fe_tone_curve, sizeof(ik_frontend_tone_curve_t));
        (void)amba_ik_system_unlock_mutex(&p_ctx->organization.mutex);
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
    if (context_id != 0U) {
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
        (void)amba_ik_system_lock_mutex(&p_ctx->organization.mutex, IK_WAIT_FOREVER);
        (void)amba_ik_system_memcpy(p_resample_str, &p_ctx->filters.input_param.resample_str, sizeof(ik_resampler_strength_t));
        (void)amba_ik_system_unlock_mutex(&p_ctx->organization.mutex);
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
    if (context_id != 0U) {
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
        (void)amba_ik_system_lock_mutex(&p_ctx->organization.mutex, IK_WAIT_FOREVER);
        (void)amba_ik_system_memcpy(p_hist_info, &p_ctx->filters.input_param.hist_info, sizeof(ik_histogram_info_t));
        (void)amba_ik_system_unlock_mutex(&p_ctx->organization.mutex);
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

uint32 img_ctx_ivd_get_pg_histogram_info(uint32 context_id, ik_histogram_info_t *p_hist_info)
{
    amba_ik_system_print_str_5("[IK] get_pg_histogram_info is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0U) {
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
        (void)amba_ik_system_lock_mutex(&p_ctx->organization.mutex, IK_WAIT_FOREVER);
        (void)amba_ik_system_memcpy(p_hist_info, &p_ctx->filters.input_param.hist_info_pg, sizeof(ik_histogram_info_t));
        (void)amba_ik_system_unlock_mutex(&p_ctx->organization.mutex);
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
    if (context_id != 0U) {
        //MISRA
    }
    if (enable == 0U) {
        //MISRA
    }
    return rval;
}

uint32 img_ctx_set_y2y_use_cc_enable_info(uint32 context_id, const uint32 enable)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;
    if(enable > 1U) {
        amba_ik_system_print_str_5("[IK][Error] Invalid data valid, set_pg_histogram_info() enable > 1 !", NULL, NULL, NULL, NULL, NULL);
    }
    rval |= img_ctx_get_context(context_id, &p_ctx);
    if((rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        (void)amba_ik_system_lock_mutex(&p_ctx->organization.mutex, IK_WAIT_FOREVER);
        p_ctx->filters.input_param.use_cc_for_yuv2yuv = enable;
        ctx_raise_update_flag(&p_ctx->filters.update_flags.iso.use_y2y_enable_info_updated);
        (void)amba_ik_system_unlock_mutex(&p_ctx->organization.mutex);
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
    if (context_id != 0U) {
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
        (void)amba_ik_system_lock_mutex(&p_ctx->organization.mutex, IK_WAIT_FOREVER);
        (void)amba_ik_system_memcpy(p_enable, &p_ctx->filters.input_param.use_cc_for_yuv2yuv, sizeof(uint32));
        (void)amba_ik_system_unlock_mutex(&p_ctx->organization.mutex);
    } else {
        if(p_ctx->organization.initial_flag == 0U) {
            amba_ik_system_print_uint32_5("[IK][ERROR] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
        } else {
            amba_ik_system_print_str_5("[IK][ERROR] can't get context\n", NULL, NULL, NULL, NULL, NULL);
        }
    }
    return rval;
}

//mono
uint32 img_ctx_ivd_get_mono_dynamic_bad_pixel_corr(uint32 context_id, ik_mono_dynamic_bad_pixel_corr_t *p_mono_dynamic_bad_pixel_corr)
{
    amba_ik_system_print_str_5("[IK] get_mono_dynamic_bad_pixel_corr is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0U) {
        //TBD
    }
    if (p_mono_dynamic_bad_pixel_corr == NULL) {
        //TBD
    } else {
        (void)amba_ik_system_memcpy(p_mono_dynamic_bad_pixel_corr, p_mono_dynamic_bad_pixel_corr, sizeof(uint8));
    }
    return IK_ERR_0004;
}

uint32 img_ctx_get_mono_dynamic_bad_pixel_corr(uint32 context_id, ik_mono_dynamic_bad_pixel_corr_t *p_mono_dynamic_bad_pixel_corr)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval |= img_ctx_get_context(context_id, &p_ctx);
    if((p_mono_dynamic_bad_pixel_corr != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        (void)amba_ik_system_lock_mutex(&p_ctx->organization.mutex, IK_WAIT_FOREVER);
        (void)amba_ik_system_memcpy(p_mono_dynamic_bad_pixel_corr, &p_ctx->filters.input_param.mono_dynamic_bad_pixel_corr, sizeof(ik_mono_dynamic_bad_pixel_corr_t));
        (void)amba_ik_system_unlock_mutex(&p_ctx->organization.mutex);
    } else {
        if(p_mono_dynamic_bad_pixel_corr == NULL) {
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

uint32 img_ctx_ivd_get_mono_cfa_leakage_filter(uint32 context_id, ik_mono_cfa_leakage_filter_t *p_mono_cfa_leakage_filter)
{
    amba_ik_system_print_str_5("[IK] get_mono_cfa_leakage_filter is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0U) {
        //TBD
    }
    if (p_mono_cfa_leakage_filter == NULL) {
        //TBD
    } else {
        (void)amba_ik_system_memcpy(p_mono_cfa_leakage_filter, p_mono_cfa_leakage_filter, sizeof(uint8));
    }
    return IK_ERR_0004;
}

uint32 img_ctx_get_mono_cfa_leakage_filter(uint32 context_id, ik_mono_cfa_leakage_filter_t *p_mono_cfa_leakage_filter)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval |= img_ctx_get_context(context_id, &p_ctx);
    if((p_mono_cfa_leakage_filter != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        (void)amba_ik_system_lock_mutex(&p_ctx->organization.mutex, IK_WAIT_FOREVER);
        (void)amba_ik_system_memcpy(p_mono_cfa_leakage_filter, &p_ctx->filters.input_param.mono_cfa_leakage_filter, sizeof(ik_mono_cfa_leakage_filter_t));
        (void)amba_ik_system_unlock_mutex(&p_ctx->organization.mutex);
    } else {
        if(p_mono_cfa_leakage_filter == NULL) {
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

uint32 img_ctx_ivd_get_mono_cfa_noise_filter(uint32 context_id, ik_mono_cfa_noise_filter_t *p_mono_cfa_noise_filter)
{
    amba_ik_system_print_str_5("[IK] get_mono_cfa_noise_filter is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0U) {
        //TBD
    }
    if (p_mono_cfa_noise_filter == NULL) {
        //TBD
    } else {
        (void)amba_ik_system_memcpy(p_mono_cfa_noise_filter, p_mono_cfa_noise_filter, sizeof(uint8));
    }
    return IK_ERR_0004;
}

uint32 img_ctx_get_mono_cfa_noise_filter(uint32 context_id, ik_mono_cfa_noise_filter_t *p_mono_cfa_noise_filter)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval |= img_ctx_get_context(context_id, &p_ctx);
    if((p_mono_cfa_noise_filter != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        (void)amba_ik_system_lock_mutex(&p_ctx->organization.mutex, IK_WAIT_FOREVER);
        (void)amba_ik_system_memcpy(p_mono_cfa_noise_filter, &p_ctx->filters.input_param.mono_cfa_noise_filter, sizeof(ik_mono_cfa_noise_filter_t));
        (void)amba_ik_system_unlock_mutex(&p_ctx->organization.mutex);
    } else {
        if(p_mono_cfa_noise_filter == NULL) {
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

uint32 img_ctx_ivd_get_mono_anti_aliasing(uint32 context_id, ik_mono_anti_aliasing_t *p_mono_anti_aliasing)
{
    amba_ik_system_print_str_5("[IK] get_mono_anti_aliasing is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0U) {
        //TBD
    }
    if (p_mono_anti_aliasing == NULL) {
        //TBD
    } else {
        (void)amba_ik_system_memcpy(p_mono_anti_aliasing, p_mono_anti_aliasing, sizeof(uint8));
    }
    return IK_ERR_0004;
}

uint32 img_ctx_get_mono_anti_aliasing(uint32 context_id, ik_mono_anti_aliasing_t *p_mono_anti_aliasing)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval |= img_ctx_get_context(context_id, &p_ctx);
    if((p_mono_anti_aliasing != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        (void)amba_ik_system_lock_mutex(&p_ctx->organization.mutex, IK_WAIT_FOREVER);
        (void)amba_ik_system_memcpy(p_mono_anti_aliasing, &p_ctx->filters.input_param.mono_anti_aliasing, sizeof(ik_mono_anti_aliasing_t));
        (void)amba_ik_system_unlock_mutex(&p_ctx->organization.mutex);
    } else {
        if(p_mono_anti_aliasing == NULL) {
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

uint32 img_ctx_ivd_get_mono_before_ce_wb_gain(uint32 context_id, ik_mono_before_ce_wb_gain_t *p_mono_before_ce_wb_gain)
{
    amba_ik_system_print_str_5("[IK] get_mono_before_ce_wb_gain is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0U) {
        //TBD
    }
    if (p_mono_before_ce_wb_gain == NULL) {
        //TBD
    } else {
        (void)amba_ik_system_memcpy(p_mono_before_ce_wb_gain, p_mono_before_ce_wb_gain, sizeof(uint8));
    }
    return IK_ERR_0004;
}

uint32 img_ctx_get_mono_before_ce_wb_gain(uint32 context_id, ik_mono_before_ce_wb_gain_t *p_mono_before_ce_wb_gain)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval |= img_ctx_get_context(context_id, &p_ctx);
    if((p_mono_before_ce_wb_gain != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        (void)amba_ik_system_lock_mutex(&p_ctx->organization.mutex, IK_WAIT_FOREVER);
        (void)amba_ik_system_memcpy(p_mono_before_ce_wb_gain, &p_ctx->filters.input_param.mono_before_ce_wb_gain, sizeof(ik_mono_before_ce_wb_gain_t));
        (void)amba_ik_system_unlock_mutex(&p_ctx->organization.mutex);
    } else {
        if(p_mono_before_ce_wb_gain == NULL) {
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

uint32 img_ctx_ivd_get_mono_after_ce_wb_gain(uint32 context_id, ik_mono_after_ce_wb_gain_t *p_mono_after_ce_wb_gain)
{
    amba_ik_system_print_str_5("[IK] get_mono_after_ce_wb_gain is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0U) {
        //TBD
    }
    if (p_mono_after_ce_wb_gain == NULL) {
        //TBD
    } else {
        (void)amba_ik_system_memcpy(p_mono_after_ce_wb_gain, p_mono_after_ce_wb_gain, sizeof(uint8));
    }
    return IK_ERR_0004;
}

uint32 img_ctx_get_mono_after_ce_wb_gain(uint32 context_id, ik_mono_after_ce_wb_gain_t *p_mono_after_ce_wb_gain)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval |= img_ctx_get_context(context_id, &p_ctx);
    if((p_mono_after_ce_wb_gain != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        (void)amba_ik_system_lock_mutex(&p_ctx->organization.mutex, IK_WAIT_FOREVER);
        (void)amba_ik_system_memcpy(p_mono_after_ce_wb_gain, &p_ctx->filters.input_param.mono_after_ce_wb_gain, sizeof(ik_mono_after_ce_wb_gain_t));
        (void)amba_ik_system_unlock_mutex(&p_ctx->organization.mutex);
    } else {
        if(p_mono_after_ce_wb_gain == NULL) {
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

uint32 img_ctx_ivd_get_mono_color_correction(uint32 context_id, ik_mono_color_correction_t *p_mono_color_correction)
{
    amba_ik_system_print_str_5("[IK] get_mono_color_correction is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0U) {
        //TBD
    }
    if (p_mono_color_correction == NULL) {
        //TBD
    } else {
        (void)amba_ik_system_memcpy(p_mono_color_correction, p_mono_color_correction, sizeof(uint8));
    }
    return IK_ERR_0004;
}

uint32 img_ctx_get_mono_color_correction(uint32 context_id, ik_mono_color_correction_t *p_mono_color_correction)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval |= img_ctx_get_context(context_id, &p_ctx);
    if((p_mono_color_correction != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        (void)amba_ik_system_lock_mutex(&p_ctx->organization.mutex, IK_WAIT_FOREVER);
        (void)amba_ik_system_memcpy(p_mono_color_correction, &p_ctx->filters.input_param.mono_color_correction, sizeof(ik_mono_color_correction_t));
        (void)amba_ik_system_unlock_mutex(&p_ctx->organization.mutex);
    } else {
        if(p_mono_color_correction == NULL) {
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

uint32 img_ctx_ivd_get_mono_tone_curve(uint32 context_id, ik_mono_tone_curve_t *p_mono_tone_curve)
{
    amba_ik_system_print_str_5("[IK] get_mono_tone_curve is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0U) {
        //TBD
    }
    if (p_mono_tone_curve == NULL) {
        //TBD
    } else {
        (void)amba_ik_system_memcpy(p_mono_tone_curve, p_mono_tone_curve, sizeof(uint8));
    }
    return IK_ERR_0004;
}

uint32 img_ctx_get_mono_tone_curve(uint32 context_id, ik_mono_tone_curve_t *p_mono_tone_curve)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval |= img_ctx_get_context(context_id, &p_ctx);
    if((p_mono_tone_curve != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        (void)amba_ik_system_lock_mutex(&p_ctx->organization.mutex, IK_WAIT_FOREVER);
        (void)amba_ik_system_memcpy(p_mono_tone_curve, &p_ctx->filters.input_param.mono_tone_curve, sizeof(ik_mono_tone_curve_t));
        (void)amba_ik_system_unlock_mutex(&p_ctx->organization.mutex);
    } else {
        if(p_mono_tone_curve == NULL) {
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

uint32 img_ctx_ivd_get_mono_rgb_to_yuv_matrix(uint32 context_id, ik_mono_rgb_to_yuv_matrix_t *p_mono_rgb_to_yuv_matrix)
{
    amba_ik_system_print_str_5("[IK] get_mono_rgb_to_yuv_matrix is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0U) {
        //TBD
    }
    if (p_mono_rgb_to_yuv_matrix == NULL) {
        //TBD
    } else {
        (void)amba_ik_system_memcpy(p_mono_rgb_to_yuv_matrix, p_mono_rgb_to_yuv_matrix, sizeof(uint8));
    }
    return IK_ERR_0004;
}

uint32 img_ctx_get_mono_rgb_to_yuv_matrix(uint32 context_id, ik_mono_rgb_to_yuv_matrix_t *p_mono_rgb_to_yuv_matrix)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval |= img_ctx_get_context(context_id, &p_ctx);
    if((p_mono_rgb_to_yuv_matrix != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        (void)amba_ik_system_lock_mutex(&p_ctx->organization.mutex, IK_WAIT_FOREVER);
        (void)amba_ik_system_memcpy(p_mono_rgb_to_yuv_matrix, &p_ctx->filters.input_param.mono_rgb_to_yuv_matrix, sizeof(ik_mono_rgb_to_yuv_matrix_t));
        (void)amba_ik_system_unlock_mutex(&p_ctx->organization.mutex);
    } else {
        if(p_mono_rgb_to_yuv_matrix == NULL) {
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

uint32 img_ctx_ivd_get_mono_demosaic(uint32 context_id, ik_mono_demosaic_t *p_mono_demosaic)
{
    amba_ik_system_print_str_5("[IK] get_mono_demosaic is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0U) {
        //TBD
    }
    if (p_mono_demosaic == NULL) {
        //TBD
    } else {
        (void)amba_ik_system_memcpy(p_mono_demosaic, p_mono_demosaic, sizeof(uint8));
    }
    return IK_ERR_0004;
}

uint32 img_ctx_get_mono_demosaic(uint32 context_id, ik_mono_demosaic_t *p_mono_demosaic)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval |= img_ctx_get_context(context_id, &p_ctx);
    if((p_mono_demosaic != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        (void)amba_ik_system_lock_mutex(&p_ctx->organization.mutex, IK_WAIT_FOREVER);
        (void)amba_ik_system_memcpy(p_mono_demosaic, &p_ctx->filters.input_param.mono_demosaic, sizeof(ik_mono_demosaic_t));
        (void)amba_ik_system_unlock_mutex(&p_ctx->organization.mutex);
    } else {
        if(p_mono_demosaic == NULL) {
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

uint32 img_ctx_ivd_get_mono_grgb_mismatch(uint32 context_id, ik_mono_grgb_mismatch_t *p_mono_grgb_mismatch)
{
    amba_ik_system_print_str_5("[IK] get_mono_grgb_mismatch is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0U) {
        //TBD
    }
    if (p_mono_grgb_mismatch == NULL) {
        //TBD
    } else {
        (void)amba_ik_system_memcpy(p_mono_grgb_mismatch, p_mono_grgb_mismatch, sizeof(uint8));
    }
    return IK_ERR_0004;
}

uint32 img_ctx_get_mono_grgb_mismatch(uint32 context_id, ik_mono_grgb_mismatch_t *p_mono_grgb_mismatch)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval |= img_ctx_get_context(context_id, &p_ctx);
    if((p_mono_grgb_mismatch != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        (void)amba_ik_system_lock_mutex(&p_ctx->organization.mutex, IK_WAIT_FOREVER);
        (void)amba_ik_system_memcpy(p_mono_grgb_mismatch, &p_ctx->filters.input_param.mono_grgb_mismatch, sizeof(ik_mono_grgb_mismatch_t));
        (void)amba_ik_system_unlock_mutex(&p_ctx->organization.mutex);
    } else {
        if(p_mono_grgb_mismatch == NULL) {
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

uint32 img_ctx_ivd_get_mono_fst_luma_process_mode(uint32 context_id, ik_mono_fst_luma_process_mode_t *p_mono_fst_luma_process_mode)
{
    amba_ik_system_print_str_5("[IK] get_mono_fst_luma_process_mode is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0U) {
        //TBD
    }
    if (p_mono_fst_luma_process_mode == NULL) {
        //TBD
    } else {
        (void)amba_ik_system_memcpy(p_mono_fst_luma_process_mode, p_mono_fst_luma_process_mode, sizeof(uint8));
    }
    return IK_ERR_0004;
}

uint32 img_ctx_get_mono_fst_luma_process_mode(uint32 context_id, ik_mono_fst_luma_process_mode_t *p_mono_fst_luma_process_mode)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval |= img_ctx_get_context(context_id, &p_ctx);
    if((p_mono_fst_luma_process_mode != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        (void)amba_ik_system_lock_mutex(&p_ctx->organization.mutex, IK_WAIT_FOREVER);
        (void)amba_ik_system_memcpy(p_mono_fst_luma_process_mode, &p_ctx->filters.input_param.mono_fst_luma_process_mode, sizeof(ik_mono_fst_luma_process_mode_t));
        (void)amba_ik_system_unlock_mutex(&p_ctx->organization.mutex);
    } else {
        if(p_mono_fst_luma_process_mode == NULL) {
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

uint32 img_ctx_ivd_get_mono_adv_spatial_filter(uint32 context_id, ik_mono_adv_spatial_filter_t *p_mono_adv_spatial_filter)
{
    amba_ik_system_print_str_5("[IK] get_mono_adv_spatial_filter is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0U) {
        //TBD
    }
    if (p_mono_adv_spatial_filter == NULL) {
        //TBD
    } else {
        (void)amba_ik_system_memcpy(p_mono_adv_spatial_filter, p_mono_adv_spatial_filter, sizeof(uint8));
    }
    return IK_ERR_0004;
}

uint32 img_ctx_get_mono_adv_spatial_filter(uint32 context_id, ik_mono_adv_spatial_filter_t *p_mono_adv_spatial_filter)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval |= img_ctx_get_context(context_id, &p_ctx);
    if((p_mono_adv_spatial_filter != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        (void)amba_ik_system_lock_mutex(&p_ctx->organization.mutex, IK_WAIT_FOREVER);
        (void)amba_ik_system_memcpy(p_mono_adv_spatial_filter, &p_ctx->filters.input_param.mono_adv_spatial_filter, sizeof(ik_mono_adv_spatial_filter_t));
        (void)amba_ik_system_unlock_mutex(&p_ctx->organization.mutex);
    } else {
        if(p_mono_adv_spatial_filter == NULL) {
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

uint32 img_ctx_ivd_get_mono_fst_shp_both(uint32 context_id, ik_mono_fst_shp_both_t *p_mono_fst_shp_both)
{
    amba_ik_system_print_str_5("[IK] get_mono_fst_shp_both is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0U) {
        //TBD
    }
    if (p_mono_fst_shp_both == NULL) {
        //TBD
    } else {
        (void)amba_ik_system_memcpy(p_mono_fst_shp_both, p_mono_fst_shp_both, sizeof(uint8));
    }
    return IK_ERR_0004;
}

uint32 img_ctx_get_mono_fst_shp_both(uint32 context_id, ik_mono_fst_shp_both_t *p_mono_fst_shp_both)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval |= img_ctx_get_context(context_id, &p_ctx);
    if((p_mono_fst_shp_both != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        (void)amba_ik_system_lock_mutex(&p_ctx->organization.mutex, IK_WAIT_FOREVER);
        (void)amba_ik_system_memcpy(p_mono_fst_shp_both, &p_ctx->filters.input_param.mono_fst_shp_both, sizeof(ik_mono_fst_shp_both_t));
        (void)amba_ik_system_unlock_mutex(&p_ctx->organization.mutex);
    } else {
        if(p_mono_fst_shp_both == NULL) {
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

uint32 img_ctx_ivd_get_mono_fst_shp_noise(uint32 context_id, ik_mono_fst_shp_noise_t *p_mono_fst_shp_noise)
{
    amba_ik_system_print_str_5("[IK] get_mono_fst_shp_noise is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0U) {
        //TBD
    }
    if (p_mono_fst_shp_noise == NULL) {
        //TBD
    } else {
        (void)amba_ik_system_memcpy(p_mono_fst_shp_noise, p_mono_fst_shp_noise, sizeof(uint8));
    }
    return IK_ERR_0004;
}

uint32 img_ctx_get_mono_fst_shp_noise(uint32 context_id, ik_mono_fst_shp_noise_t *p_mono_fst_shp_noise)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval |= img_ctx_get_context(context_id, &p_ctx);
    if((p_mono_fst_shp_noise != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        (void)amba_ik_system_lock_mutex(&p_ctx->organization.mutex, IK_WAIT_FOREVER);
        (void)amba_ik_system_memcpy(p_mono_fst_shp_noise, &p_ctx->filters.input_param.mono_fst_shp_noise, sizeof(ik_mono_fst_shp_noise_t));
        (void)amba_ik_system_unlock_mutex(&p_ctx->organization.mutex);
    } else {
        if(p_mono_fst_shp_noise == NULL) {
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

uint32 img_ctx_ivd_get_mono_fst_shp_fir(uint32 context_id, ik_mono_fst_shp_fir_t *p_mono_fst_shp_fir)
{
    amba_ik_system_print_str_5("[IK] get_mono_fst_shp_fir is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0U) {
        //TBD
    }
    if (p_mono_fst_shp_fir == NULL) {
        //TBD
    } else {
        (void)amba_ik_system_memcpy(p_mono_fst_shp_fir, p_mono_fst_shp_fir, sizeof(uint8));
    }
    return IK_ERR_0004;
}

uint32 img_ctx_get_mono_fst_shp_fir(uint32 context_id, ik_mono_fst_shp_fir_t *p_mono_fst_shp_fir)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval |= img_ctx_get_context(context_id, &p_ctx);
    if((p_mono_fst_shp_fir != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        (void)amba_ik_system_lock_mutex(&p_ctx->organization.mutex, IK_WAIT_FOREVER);
        (void)amba_ik_system_memcpy(p_mono_fst_shp_fir, &p_ctx->filters.input_param.mono_fst_shp_fir, sizeof(ik_mono_fst_shp_fir_t));
        (void)amba_ik_system_unlock_mutex(&p_ctx->organization.mutex);
    } else {
        if(p_mono_fst_shp_fir == NULL) {
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

uint32 img_ctx_ivd_get_mono_fst_shp_coring(uint32 context_id, ik_mono_fst_shp_coring_t *p_mono_fst_shp_coring)
{
    amba_ik_system_print_str_5("[IK] get_mono_fst_shp_coring is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0U) {
        //TBD
    }
    if (p_mono_fst_shp_coring == NULL) {
        //TBD
    } else {
        (void)amba_ik_system_memcpy(p_mono_fst_shp_coring, p_mono_fst_shp_coring, sizeof(uint8));
    }
    return IK_ERR_0004;
}

uint32 img_ctx_get_mono_fst_shp_coring(uint32 context_id, ik_mono_fst_shp_coring_t *p_mono_fst_shp_coring)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval |= img_ctx_get_context(context_id, &p_ctx);
    if((p_mono_fst_shp_coring != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        (void)amba_ik_system_lock_mutex(&p_ctx->organization.mutex, IK_WAIT_FOREVER);
        (void)amba_ik_system_memcpy(p_mono_fst_shp_coring, &p_ctx->filters.input_param.mono_fst_shp_coring, sizeof(ik_mono_fst_shp_coring_t));
        (void)amba_ik_system_unlock_mutex(&p_ctx->organization.mutex);
    } else {
        if(p_mono_fst_shp_coring == NULL) {
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

uint32 img_ctx_ivd_get_mono_fst_shp_coring_idx_scale(uint32 context_id, ik_mono_fst_shp_coring_idx_scale_t *p_mono_fst_shp_coring_idx_scale)
{
    amba_ik_system_print_str_5("[IK] get_mono_fst_shp_coring_idx_scale is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0U) {
        //TBD
    }
    if (p_mono_fst_shp_coring_idx_scale == NULL) {
        //TBD
    } else {
        (void)amba_ik_system_memcpy(p_mono_fst_shp_coring_idx_scale, p_mono_fst_shp_coring_idx_scale, sizeof(uint8));
    }
    return IK_ERR_0004;
}

uint32 img_ctx_get_mono_fst_shp_coring_idx_scale(uint32 context_id, ik_mono_fst_shp_coring_idx_scale_t *p_mono_fst_shp_coring_idx_scale)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval |= img_ctx_get_context(context_id, &p_ctx);
    if((p_mono_fst_shp_coring_idx_scale != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        (void)amba_ik_system_lock_mutex(&p_ctx->organization.mutex, IK_WAIT_FOREVER);
        (void)amba_ik_system_memcpy(p_mono_fst_shp_coring_idx_scale, &p_ctx->filters.input_param.mono_fst_shp_coring_idx_scale, sizeof(ik_mono_fst_shp_coring_idx_scale_t));
        (void)amba_ik_system_unlock_mutex(&p_ctx->organization.mutex);
    } else {
        if(p_mono_fst_shp_coring_idx_scale == NULL) {
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

uint32 img_ctx_ivd_get_mono_fst_shp_min_coring_rslt(uint32 context_id, ik_mono_fst_shp_min_coring_rslt_t *p_mono_fst_shp_min_coring_rslt)
{
    amba_ik_system_print_str_5("[IK] get_mono_fst_shp_min_coring_rslt is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0U) {
        //TBD
    }
    if (p_mono_fst_shp_min_coring_rslt == NULL) {
        //TBD
    } else {
        (void)amba_ik_system_memcpy(p_mono_fst_shp_min_coring_rslt, p_mono_fst_shp_min_coring_rslt, sizeof(uint8));
    }
    return IK_ERR_0004;
}

uint32 img_ctx_get_mono_fst_shp_min_coring_rslt(uint32 context_id, ik_mono_fst_shp_min_coring_rslt_t *p_mono_fst_shp_min_coring_rslt)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval |= img_ctx_get_context(context_id, &p_ctx);
    if((p_mono_fst_shp_min_coring_rslt != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        (void)amba_ik_system_lock_mutex(&p_ctx->organization.mutex, IK_WAIT_FOREVER);
        (void)amba_ik_system_memcpy(p_mono_fst_shp_min_coring_rslt, &p_ctx->filters.input_param.mono_fst_shp_min_coring_rslt, sizeof(ik_mono_fst_shp_min_coring_rslt_t));
        (void)amba_ik_system_unlock_mutex(&p_ctx->organization.mutex);
    } else {
        if(p_mono_fst_shp_min_coring_rslt == NULL) {
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

uint32 img_ctx_ivd_get_mono_fst_shp_max_coring_rslt(uint32 context_id, ik_mono_fst_shp_max_coring_rslt_t *p_mono_fst_shp_max_coring_rslt)
{
    amba_ik_system_print_str_5("[IK] get_mono_fst_shp_max_coring_rslt is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0U) {
        //TBD
    }
    if (p_mono_fst_shp_max_coring_rslt == NULL) {
        //TBD
    } else {
        (void)amba_ik_system_memcpy(p_mono_fst_shp_max_coring_rslt, p_mono_fst_shp_max_coring_rslt, sizeof(uint8));
    }
    return IK_ERR_0004;
}

uint32 img_ctx_get_mono_fst_shp_max_coring_rslt(uint32 context_id, ik_mono_fst_shp_max_coring_rslt_t *p_mono_fst_shp_max_coring_rslt)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval |= img_ctx_get_context(context_id, &p_ctx);
    if((p_mono_fst_shp_max_coring_rslt != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        (void)amba_ik_system_lock_mutex(&p_ctx->organization.mutex, IK_WAIT_FOREVER);
        (void)amba_ik_system_memcpy(p_mono_fst_shp_max_coring_rslt, &p_ctx->filters.input_param.mono_fst_shp_max_coring_rslt, sizeof(ik_mono_fst_shp_max_coring_rslt_t));
        (void)amba_ik_system_unlock_mutex(&p_ctx->organization.mutex);
    } else {
        if(p_mono_fst_shp_max_coring_rslt == NULL) {
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

uint32 img_ctx_ivd_get_mono_fst_shp_scale_coring(uint32 context_id, ik_mono_fst_shp_scale_coring_t *p_mono_fst_shp_scale_coring)
{
    amba_ik_system_print_str_5("[IK] get_mono_fst_shp_scale_coring is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0U) {
        //TBD
    }
    if (p_mono_fst_shp_scale_coring == NULL) {
        //TBD
    } else {
        (void)amba_ik_system_memcpy(p_mono_fst_shp_scale_coring, p_mono_fst_shp_scale_coring, sizeof(uint8));
    }
    return IK_ERR_0004;
}

uint32 img_ctx_get_mono_fst_shp_scale_coring(uint32 context_id, ik_mono_fst_shp_scale_coring_t *p_mono_fst_shp_scale_coring)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval |= img_ctx_get_context(context_id, &p_ctx);
    if((p_mono_fst_shp_scale_coring != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        (void)amba_ik_system_lock_mutex(&p_ctx->organization.mutex, IK_WAIT_FOREVER);
        (void)amba_ik_system_memcpy(p_mono_fst_shp_scale_coring, &p_ctx->filters.input_param.mono_fst_shp_scale_coring, sizeof(ik_mono_fst_shp_scale_coring_t));
        (void)amba_ik_system_unlock_mutex(&p_ctx->organization.mutex);
    } else {
        if(p_mono_fst_shp_scale_coring == NULL) {
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

uint32 img_ctx_ivd_get_mono_exp0_fe_wb_gain(uint32 context_id, ik_mono_exp0_fe_wb_gain_t *p_mono_exp0_fe_wb_gain)
{
    amba_ik_system_print_str_5("[IK] get_mono_exp0_fe_wb_gain is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0U) {
        //TBD
    }
    if (p_mono_exp0_fe_wb_gain == NULL) {
        //TBD
    } else {
        (void)amba_ik_system_memcpy(p_mono_exp0_fe_wb_gain, p_mono_exp0_fe_wb_gain, sizeof(uint8));
    }
    return IK_ERR_0004;
}

uint32 img_ctx_get_mono_exp0_fe_wb_gain(uint32 context_id, ik_mono_exp0_fe_wb_gain_t *p_mono_exp0_fe_wb_gain)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval |= img_ctx_get_context(context_id, &p_ctx);
    if((p_mono_exp0_fe_wb_gain != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        (void)amba_ik_system_lock_mutex(&p_ctx->organization.mutex, IK_WAIT_FOREVER);
        (void)amba_ik_system_memcpy(p_mono_exp0_fe_wb_gain, &p_ctx->filters.input_param.mono_exp0_fe_wb_gain, sizeof(ik_mono_exp0_fe_wb_gain_t));
        (void)amba_ik_system_unlock_mutex(&p_ctx->organization.mutex);
    } else {
        if(p_mono_exp0_fe_wb_gain == NULL) {
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

uint32 img_ctx_ivd_get_mono_exp0_fe_static_blc_level(uint32 context_id, ik_mono_exp0_fe_static_blc_level_t *p_mono_exp0_fe_static_blc_level)
{
    amba_ik_system_print_str_5("[IK] get_mono_exp0_fe_static_blc_level is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0U) {
        //TBD
    }
    if (p_mono_exp0_fe_static_blc_level == NULL) {
        //TBD
    } else {
        (void)amba_ik_system_memcpy(p_mono_exp0_fe_static_blc_level, p_mono_exp0_fe_static_blc_level, sizeof(uint8));
    }
    return IK_ERR_0004;
}

uint32 img_ctx_get_mono_exp0_fe_static_blc_level(uint32 context_id, ik_mono_exp0_fe_static_blc_level_t *p_mono_exp0_fe_static_blc_level)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval |= img_ctx_get_context(context_id, &p_ctx);
    if((p_mono_exp0_fe_static_blc_level != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        (void)amba_ik_system_lock_mutex(&p_ctx->organization.mutex, IK_WAIT_FOREVER);
        (void)amba_ik_system_memcpy(p_mono_exp0_fe_static_blc_level, &p_ctx->filters.input_param.mono_exp0_fe_static_blc_level, sizeof(ik_mono_exp0_fe_static_blc_level_t));
        (void)amba_ik_system_unlock_mutex(&p_ctx->organization.mutex);
    } else {
        if(p_mono_exp0_fe_static_blc_level == NULL) {
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

uint32 img_ctx_ivd_get_mono_ce(uint32 context_id, ik_mono_ce_t *p_mono_ce)
{
    amba_ik_system_print_str_5("[IK] get_mono_ce is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0U) {
        //TBD
    }
    if (p_mono_ce == NULL) {
        //TBD
    } else {
        (void)amba_ik_system_memcpy(p_mono_ce, p_mono_ce, sizeof(uint8));
    }
    return IK_ERR_0004;
}

uint32 img_ctx_get_mono_ce(uint32 context_id, ik_mono_ce_t *p_mono_ce)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval |= img_ctx_get_context(context_id, &p_ctx);
    if((p_mono_ce != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        (void)amba_ik_system_lock_mutex(&p_ctx->organization.mutex, IK_WAIT_FOREVER);
        (void)amba_ik_system_memcpy(p_mono_ce, &p_ctx->filters.input_param.mono_ce, sizeof(ik_mono_ce_t));
        (void)amba_ik_system_unlock_mutex(&p_ctx->organization.mutex);
    } else {
        if(p_mono_ce == NULL) {
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

uint32 img_ctx_ivd_get_mono_ce_input_table(uint32 context_id, ik_mono_ce_input_table_t *p_mono_ce_input_table)
{
    amba_ik_system_print_str_5("[IK] get_mono_ce_input_table is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0U) {
        //TBD
    }
    if (p_mono_ce_input_table == NULL) {
        //TBD
    } else {
        (void)amba_ik_system_memcpy(p_mono_ce_input_table, p_mono_ce_input_table, sizeof(uint8));
    }
    return IK_ERR_0004;
}

uint32 img_ctx_get_mono_ce_input_table(uint32 context_id, ik_mono_ce_input_table_t *p_mono_ce_input_table)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval |= img_ctx_get_context(context_id, &p_ctx);
    if((p_mono_ce_input_table != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        (void)amba_ik_system_lock_mutex(&p_ctx->organization.mutex, IK_WAIT_FOREVER);
        (void)amba_ik_system_memcpy(p_mono_ce_input_table, &p_ctx->filters.input_param.mono_ce_input_table, sizeof(ik_mono_ce_input_table_t));
        (void)amba_ik_system_unlock_mutex(&p_ctx->organization.mutex);
    } else {
        if(p_mono_ce_input_table == NULL) {
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

uint32 img_ctx_ivd_get_mono_ce_output_table(uint32 context_id, ik_mono_ce_output_table_t *p_mono_ce_output_table)
{
    amba_ik_system_print_str_5("[IK] get_mono_ce_output_table is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0U) {
        //TBD
    }
    if (p_mono_ce_output_table == NULL) {
        //TBD
    } else {
        (void)amba_ik_system_memcpy(p_mono_ce_output_table, p_mono_ce_output_table, sizeof(uint8));
    }
    return IK_ERR_0004;
}

uint32 img_ctx_get_mono_ce_output_table(uint32 context_id, ik_mono_ce_output_table_t *p_mono_ce_output_table)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval |= img_ctx_get_context(context_id, &p_ctx);
    if((p_mono_ce_output_table != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        (void)amba_ik_system_lock_mutex(&p_ctx->organization.mutex, IK_WAIT_FOREVER);
        (void)amba_ik_system_memcpy(p_mono_ce_output_table, &p_ctx->filters.input_param.mono_ce_output_table, sizeof(ik_mono_ce_output_table_t));
        (void)amba_ik_system_unlock_mutex(&p_ctx->organization.mutex);
    } else {
        if(p_mono_ce_output_table == NULL) {
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

uint32 img_ctx_ivd_get_fusion(uint32 context_id, ik_fusion_t *p_fusion)
{
    amba_ik_system_print_str_5("[IK] get_fusion is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0U) {
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
        (void)amba_ik_system_lock_mutex(&p_ctx->organization.mutex, IK_WAIT_FOREVER);
        (void)amba_ik_system_memcpy(p_fusion, &p_ctx->filters.input_param.fusion, sizeof(ik_fusion_t));
        (void)amba_ik_system_unlock_mutex(&p_ctx->organization.mutex);
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

