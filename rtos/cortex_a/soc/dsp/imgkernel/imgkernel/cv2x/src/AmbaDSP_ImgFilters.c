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

#include "AmbaDSP_ImgFilter.h"
#include "AmbaDSP_ImgDebugPrint.h"
#include "AmbaDSP_ImgContextEntity.h"
#include "AmbaDSP_ImgContextComponentIF.h"
#include "AmbaDSP_ImgArchSystemAPI.h"


uint32 ik_set_vin_sensor_info(uint32 context_id, const ik_vin_sensor_info_t *p_sensor_info)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;
    const amba_img_dsp_debug_print_func_t *p_print_fun = ik_get_debug_print_func();
    if(p_print_fun != NULL) {
        p_print_fun->vin_sensor_info(context_id, p_sensor_info);
    }

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((p_sensor_info != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        rval = p_ctx->organization.filter_methods.set_vin_sensor_info(context_id, p_sensor_info);
    } else {
        if(p_sensor_info == NULL) {
            amba_ik_system_print_str_5("[IK] NULL pointer in %s\n", __func__, DC_S, DC_S, DC_S, DC_S);
            rval = IK_ERR_0005;
        } else if(p_ctx->organization.initial_flag == 0U) {
            amba_ik_system_print_uint32_5("[IK] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
            rval = IK_ERR_0002;
        } else {
            amba_ik_system_print_uint32_5("[IK] can't get context\n", DC_U, DC_U, DC_U, DC_U, DC_U);
        }
    }

    return rval;
}

uint32 ik_get_vin_sensor_info(uint32 context_id, ik_vin_sensor_info_t *p_sensor_info)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((p_sensor_info != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        rval = p_ctx->organization.filter_methods.get_vin_sensor_info(context_id, p_sensor_info);
    } else {
        if(p_sensor_info == NULL) {
            amba_ik_system_print_str_5("[IK] NULL pointer in %s\n", __func__, DC_S, DC_S, DC_S, DC_S);
            rval = IK_ERR_0005;
        } else if(p_ctx->organization.initial_flag == 0U) {
            amba_ik_system_print_uint32_5("[IK] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
            rval = IK_ERR_0002;
        } else {
            amba_ik_system_print_uint32_5("[IK] can't get context\n", DC_U, DC_U, DC_U, DC_U, DC_U);
        }
    }
    return rval;
}

uint32 ik_set_before_ce_wb_gain(uint32 context_id, const ik_wb_gain_t *p_before_ce_wb_gain)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;
    const amba_img_dsp_debug_print_func_t *p_print_fun = ik_get_debug_print_func();
    if(p_print_fun != NULL) {
        p_print_fun->before_ce_wb(context_id, p_before_ce_wb_gain);
    }

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((p_before_ce_wb_gain != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        rval = p_ctx->organization.filter_methods.set_before_ce_wb_gain(context_id, p_before_ce_wb_gain);
    } else {
        if(p_before_ce_wb_gain == NULL) {
            amba_ik_system_print_str_5("[IK] NULL pointer in %s\n", __func__, DC_S, DC_S, DC_S, DC_S);
            rval = IK_ERR_0005;
        } else if(p_ctx->organization.initial_flag == 0U) {
            amba_ik_system_print_uint32_5("[IK] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
            rval = IK_ERR_0002;
        } else {
            amba_ik_system_print_uint32_5("[IK] can't get context\n", DC_U, DC_U, DC_U, DC_U, DC_U);
        }
    }
    return rval;
}

uint32 ik_get_before_ce_wb_gain(uint32 context_id, ik_wb_gain_t *p_before_ce_wb_gain)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((p_before_ce_wb_gain != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        rval = p_ctx->organization.filter_methods.get_before_ce_wb_gain(context_id, p_before_ce_wb_gain);
    } else {
        if(p_before_ce_wb_gain == NULL) {
            amba_ik_system_print_str_5("[IK] NULL pointer in %s\n", __func__, DC_S, DC_S, DC_S, DC_S);
            rval = IK_ERR_0005;
        } else if(p_ctx->organization.initial_flag == 0U) {
            amba_ik_system_print_uint32_5("[IK] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
            rval = IK_ERR_0002;
        } else {
            amba_ik_system_print_uint32_5("[IK] can't get context\n", DC_U, DC_U, DC_U, DC_U, DC_U);
        }
    }
    return rval;
}

uint32 ik_set_after_ce_wb_gain(uint32 context_id, const ik_wb_gain_t *p_after_ce_wb_gain)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;
    const amba_img_dsp_debug_print_func_t *p_print_fun = ik_get_debug_print_func();
    if(p_print_fun != NULL) {
        p_print_fun->after_ce_wb(context_id, p_after_ce_wb_gain);
    }

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((p_after_ce_wb_gain != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        rval = p_ctx->organization.filter_methods.set_after_ce_wb_gain(context_id, p_after_ce_wb_gain);
    } else {
        if(p_after_ce_wb_gain == NULL) {
            amba_ik_system_print_str_5("[IK] NULL pointer in %s\n", __func__, DC_S, DC_S, DC_S, DC_S);
            rval = IK_ERR_0005;
        } else if(p_ctx->organization.initial_flag == 0U) {
            amba_ik_system_print_uint32_5("[IK] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
            rval = IK_ERR_0002;
        } else {
            amba_ik_system_print_uint32_5("[IK] can't get context\n", DC_U, DC_U, DC_U, DC_U, DC_U);
        }
    }
    return rval;
}

uint32 ik_get_after_ce_wb_gain(uint32 context_id, ik_wb_gain_t *p_after_ce_wb_gain)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((p_after_ce_wb_gain != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        rval = p_ctx->organization.filter_methods.get_after_ce_wb_gain(context_id, p_after_ce_wb_gain);
    } else {
        if(p_after_ce_wb_gain == NULL) {
            amba_ik_system_print_str_5("[IK] NULL pointer in %s\n", __func__, DC_S, DC_S, DC_S, DC_S);
            rval = IK_ERR_0005;
        } else if(p_ctx->organization.initial_flag == 0U) {
            amba_ik_system_print_uint32_5("[IK] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
            rval = IK_ERR_0002;
        } else {
            amba_ik_system_print_uint32_5("[IK] can't get context\n", DC_U, DC_U, DC_U, DC_U, DC_U);
        }
    }
    return rval;
}

uint32 ik_set_cfa_leakage_filter(uint32 context_id, const ik_cfa_leakage_filter_t *p_cfa_leakage_filter)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;
    const amba_img_dsp_debug_print_func_t *p_print_fun = ik_get_debug_print_func();
    if(p_print_fun != NULL) {
        p_print_fun->cfa_leakage(context_id, p_cfa_leakage_filter);
    }

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((p_cfa_leakage_filter != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        rval = p_ctx->organization.filter_methods.set_cfa_leakage_filter(context_id, p_cfa_leakage_filter);
    } else {
        if(p_cfa_leakage_filter == NULL) {
            amba_ik_system_print_str_5("[IK] NULL pointer in %s\n", __func__, DC_S, DC_S, DC_S, DC_S);
            rval = IK_ERR_0005;
        } else if(p_ctx->organization.initial_flag == 0U) {
            amba_ik_system_print_uint32_5("[IK] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
            rval = IK_ERR_0002;
        } else {
            amba_ik_system_print_uint32_5("[IK] can't get context\n", DC_U, DC_U, DC_U, DC_U, DC_U);
        }
    }
    return rval;
}

uint32 ik_get_cfa_leakage_filter(uint32 context_id, ik_cfa_leakage_filter_t *p_cfa_leakage_filter)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((p_cfa_leakage_filter != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        rval = p_ctx->organization.filter_methods.get_cfa_leakage_filter(context_id, p_cfa_leakage_filter);
    } else {
        if(p_cfa_leakage_filter == NULL) {
            amba_ik_system_print_str_5("[IK] NULL pointer in %s\n", __func__, DC_S, DC_S, DC_S, DC_S);
            rval = IK_ERR_0005;
        } else if(p_ctx->organization.initial_flag == 0U) {
            amba_ik_system_print_uint32_5("[IK] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
            rval = IK_ERR_0002;
        } else {
            amba_ik_system_print_uint32_5("[IK] can't get context\n", DC_U, DC_U, DC_U, DC_U, DC_U);
        }
    }
    return rval;
}

uint32 ik_set_anti_aliasing(uint32 context_id, const ik_anti_aliasing_t *p_anti_aliasing)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;
    const amba_img_dsp_debug_print_func_t *p_print_fun = ik_get_debug_print_func();
    if(p_print_fun != NULL) {
        p_print_fun->anti_aliasing(context_id, p_anti_aliasing);
    }

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((p_anti_aliasing != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        rval = p_ctx->organization.filter_methods.set_anti_aliasing(context_id, p_anti_aliasing);
    } else {
        if(p_anti_aliasing == NULL) {
            amba_ik_system_print_str_5("[IK] NULL pointer in %s\n", __func__, DC_S, DC_S, DC_S, DC_S);
            rval = IK_ERR_0005;
        } else if(p_ctx->organization.initial_flag == 0U) {
            amba_ik_system_print_uint32_5("[IK] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
            rval = IK_ERR_0002;
        } else {
            amba_ik_system_print_uint32_5("[IK] can't get context\n", DC_U, DC_U, DC_U, DC_U, DC_U);
        }
    }
    return rval;
}

uint32 ik_get_anti_aliasing(uint32 context_id, ik_anti_aliasing_t *p_anti_aliasing)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((p_anti_aliasing != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        rval = p_ctx->organization.filter_methods.get_anti_aliasing(context_id, p_anti_aliasing);
    } else {
        if(p_anti_aliasing == NULL) {
            amba_ik_system_print_str_5("[IK] NULL pointer in %s\n", __func__, DC_S, DC_S, DC_S, DC_S);
            rval = IK_ERR_0005;
        } else if(p_ctx->organization.initial_flag == 0U) {
            amba_ik_system_print_uint32_5("[IK] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
            rval = IK_ERR_0002;
        } else {
            amba_ik_system_print_uint32_5("[IK] can't get context\n", DC_U, DC_U, DC_U, DC_U, DC_U);
        }
    }
    return rval;
}

uint32 ik_set_dynamic_bad_pixel_corr(uint32 context_id, const ik_dynamic_bad_pixel_correction_t *p_dynamic_bpc)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;
    const amba_img_dsp_debug_print_func_t *p_print_fun = ik_get_debug_print_func();
    if(p_print_fun != NULL) {
        p_print_fun->dynamic_bad_pxl_cor(context_id, p_dynamic_bpc);
    }

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((p_dynamic_bpc != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        rval = p_ctx->organization.filter_methods.set_dynamic_bpc(context_id, p_dynamic_bpc);
    } else {
        if(p_dynamic_bpc == NULL) {
            amba_ik_system_print_str_5("[IK] NULL pointer in %s\n", __func__, DC_S, DC_S, DC_S, DC_S);
            rval = IK_ERR_0005;
        } else if(p_ctx->organization.initial_flag == 0U) {
            amba_ik_system_print_uint32_5("[IK] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
            rval = IK_ERR_0002;
        } else {
            amba_ik_system_print_uint32_5("[IK] can't get context\n", DC_U, DC_U, DC_U, DC_U, DC_U);
        }
    }
    return rval;
}

uint32 ik_get_dynamic_bad_pixel_corr(uint32 context_id, ik_dynamic_bad_pixel_correction_t *p_dynamic_bpc)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((p_dynamic_bpc != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        rval = p_ctx->organization.filter_methods.get_dynamic_bpc(context_id, p_dynamic_bpc);
    } else {
        if(p_dynamic_bpc == NULL) {
            amba_ik_system_print_str_5("[IK] NULL pointer in %s\n", __func__, DC_S, DC_S, DC_S, DC_S);
            rval = IK_ERR_0005;
        } else if(p_ctx->organization.initial_flag == 0U) {
            amba_ik_system_print_uint32_5("[IK] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
            rval = IK_ERR_0002;
        } else {
            amba_ik_system_print_uint32_5("[IK] can't get context\n", DC_U, DC_U, DC_U, DC_U, DC_U);
        }
    }
    return rval;
}

uint32 ik_set_static_bad_pxl_corr(uint32 context_id, const ik_static_bad_pxl_cor_t *p_static_bpc)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;
    const amba_img_dsp_debug_print_func_t *p_print_fun = ik_get_debug_print_func();
    if(p_print_fun != NULL) {
        p_print_fun->static_bpc(context_id, p_static_bpc);
    }

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((p_static_bpc != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        rval = p_ctx->organization.filter_methods.set_static_bpc(context_id, p_static_bpc);
    } else {
        if(p_static_bpc == NULL) {
            amba_ik_system_print_str_5("[IK] NULL pointer in %s\n", __func__, DC_S, DC_S, DC_S, DC_S);
            rval = IK_ERR_0005;
        } else if(p_ctx->organization.initial_flag == 0U) {
            amba_ik_system_print_uint32_5("[IK] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
            rval = IK_ERR_0002;
        } else {
            amba_ik_system_print_uint32_5("[IK] can't get context\n", DC_U, DC_U, DC_U, DC_U, DC_U);
        }
    }
    return rval;
}

uint32 ik_get_static_bad_pxl_corr(uint32 context_id, ik_static_bad_pxl_cor_t *p_static_bpc)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((p_static_bpc != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        rval = p_ctx->organization.filter_methods.get_static_bpc(context_id, p_static_bpc);
    } else {
        if(p_static_bpc == NULL) {
            amba_ik_system_print_str_5("[IK] NULL pointer in %s\n", __func__, DC_S, DC_S, DC_S, DC_S);
            rval = IK_ERR_0005;
        } else if(p_ctx->organization.initial_flag == 0U) {
            amba_ik_system_print_uint32_5("[IK] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
            rval = IK_ERR_0002;
        } else {
            amba_ik_system_print_uint32_5("[IK] can't get context\n", DC_U, DC_U, DC_U, DC_U, DC_U);
        }
    }
    return rval;
}

uint32 ik_set_static_bad_pxl_corr_itnl(uint32 context_id, const ik_static_bad_pixel_correction_internal_t *p_static_bpc_internal)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;
    const amba_img_dsp_debug_print_func_t *p_print_fun = ik_get_debug_print_func();
    if(p_print_fun != NULL) {
//        p_print_fun->static_bpc(context_id, p_static_bpc_internal);
        amba_ik_system_print_uint32_5("[IK] ik_set_static_bad_pxl_corr_itnl() update \n", DC_U, DC_U, DC_U, DC_U, DC_U);
    }

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((p_static_bpc_internal != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        rval = p_ctx->organization.filter_methods.set_static_bpc_internal(context_id, p_static_bpc_internal);
    } else {
        if(p_static_bpc_internal == NULL) {
            amba_ik_system_print_str_5("[IK] NULL pointer in %s\n", __func__, DC_S, DC_S, DC_S, DC_S);
            rval = IK_ERR_0005;
        } else if(p_ctx->organization.initial_flag == 0U) {
            amba_ik_system_print_uint32_5("[IK] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
            rval = IK_ERR_0002;
        } else {
            amba_ik_system_print_uint32_5("[IK] can't get context\n", DC_U, DC_U, DC_U, DC_U, DC_U);
        }
    }
    return rval;
}

uint32 ik_get_static_bad_pxl_corr_itnl(uint32 context_id, ik_static_bad_pixel_correction_internal_t *p_static_bpc_internal)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((p_static_bpc_internal != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        rval = p_ctx->organization.filter_methods.get_static_bpc_internal(context_id, p_static_bpc_internal);
    } else {
        if(p_static_bpc_internal == NULL) {
            amba_ik_system_print_str_5("[IK] NULL pointer in %s\n", __func__, DC_S, DC_S, DC_S, DC_S);
            rval = IK_ERR_0005;
        } else if(p_ctx->organization.initial_flag == 0U) {
            amba_ik_system_print_uint32_5("[IK] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
            rval = IK_ERR_0002;
        } else {
            amba_ik_system_print_uint32_5("[IK] can't get context\n", DC_U, DC_U, DC_U, DC_U, DC_U);
        }
    }
    return rval;
}

uint32 ik_set_cfa_noise_filter(uint32 context_id, const ik_cfa_noise_filter_t *p_cfa_noise_filter)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;
    const amba_img_dsp_debug_print_func_t *p_print_fun = ik_get_debug_print_func();
    if(p_print_fun != NULL) {
        p_print_fun->cfa_noise_filter(context_id, p_cfa_noise_filter);
    }

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((p_cfa_noise_filter != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        rval = p_ctx->organization.filter_methods.set_cfa_noise_filter(context_id, p_cfa_noise_filter);
    } else {
        if(p_cfa_noise_filter == NULL) {
            amba_ik_system_print_str_5("[IK] NULL pointer in %s\n", __func__, DC_S, DC_S, DC_S, DC_S);
            rval = IK_ERR_0005;
        } else if(p_ctx->organization.initial_flag == 0U) {
            amba_ik_system_print_uint32_5("[IK] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
            rval = IK_ERR_0002;
        } else {
            amba_ik_system_print_uint32_5("[IK] can't get context\n", DC_U, DC_U, DC_U, DC_U, DC_U);
        }
    }
    return rval;
}

uint32 ik_get_cfa_noise_filter(uint32 context_id, ik_cfa_noise_filter_t *p_cfa_noise_filter)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((p_cfa_noise_filter != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        rval = p_ctx->organization.filter_methods.get_cfa_noise_filter(context_id, p_cfa_noise_filter);
    } else {
        if(p_cfa_noise_filter == NULL) {
            amba_ik_system_print_str_5("[IK] NULL pointer in %s\n", __func__, DC_S, DC_S, DC_S, DC_S);
            rval = IK_ERR_0005;
        } else if(p_ctx->organization.initial_flag == 0U) {
            amba_ik_system_print_uint32_5("[IK] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
            rval = IK_ERR_0002;
        } else {
            amba_ik_system_print_uint32_5("[IK] can't get context\n", DC_U, DC_U, DC_U, DC_U, DC_U);
        }
    }
    return rval;
}

uint32 ik_set_demosaic(uint32 context_id, const ik_demosaic_t *p_demosaic)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;
    const amba_img_dsp_debug_print_func_t *p_print_fun = ik_get_debug_print_func();
    if(p_print_fun != NULL) {
        p_print_fun->demosaic(context_id, p_demosaic);
    }

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((p_demosaic != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        rval = p_ctx->organization.filter_methods.set_demosaic(context_id, p_demosaic);
    } else {
        if(p_demosaic == NULL) {
            amba_ik_system_print_str_5("[IK] NULL pointer in %s\n", __func__, DC_S, DC_S, DC_S, DC_S);
            rval = IK_ERR_0005;
        } else if(p_ctx->organization.initial_flag == 0U) {
            amba_ik_system_print_uint32_5("[IK] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
            rval = IK_ERR_0002;
        } else {
            amba_ik_system_print_uint32_5("[IK] can't get context\n", DC_U, DC_U, DC_U, DC_U, DC_U);
        }
    }
    return rval;
}

uint32 ik_get_demosaic(uint32 context_id, ik_demosaic_t *p_demosaic)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((p_demosaic != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        rval = p_ctx->organization.filter_methods.get_demosaic(context_id, p_demosaic);
    } else {
        if(p_demosaic == NULL) {
            amba_ik_system_print_str_5("[IK] NULL pointer in %s\n", __func__, DC_S, DC_S, DC_S, DC_S);
            rval = IK_ERR_0005;
        } else if(p_ctx->organization.initial_flag == 0U) {
            amba_ik_system_print_uint32_5("[IK] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
            rval = IK_ERR_0002;
        } else {
            amba_ik_system_print_uint32_5("[IK] can't get context\n", DC_U, DC_U, DC_U, DC_U, DC_U);
        }
    }
    return rval;
}

uint32 ik_set_color_correction_reg(uint32 context_id, const ik_color_correction_reg_t *p_color_correction_reg)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;
    const amba_img_dsp_debug_print_func_t *p_print_fun = ik_get_debug_print_func();
    if(p_print_fun != NULL) {
        p_print_fun->color_correction_reg(context_id, p_color_correction_reg);
    }

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((p_color_correction_reg != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        rval = p_ctx->organization.filter_methods.set_color_correction_reg(context_id, p_color_correction_reg);
    } else {
        if(p_color_correction_reg == NULL) {
            amba_ik_system_print_str_5("[IK] NULL pointer in %s\n", __func__, DC_S, DC_S, DC_S, DC_S);
            rval = IK_ERR_0005;
        } else if(p_ctx->organization.initial_flag == 0U) {
            amba_ik_system_print_uint32_5("[IK] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
            rval = IK_ERR_0002;
        } else {
            amba_ik_system_print_uint32_5("[IK] can't get context\n", DC_U, DC_U, DC_U, DC_U, DC_U);
        }
    }
    return rval;
}

uint32 ik_get_color_correction_reg(uint32 context_id, ik_color_correction_reg_t *p_color_correction_reg)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((p_color_correction_reg != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        rval = p_ctx->organization.filter_methods.get_color_correction_reg(context_id, p_color_correction_reg);
    } else {
        if(p_color_correction_reg == NULL) {
            amba_ik_system_print_str_5("[IK] NULL pointer in %s\n", __func__, DC_S, DC_S, DC_S, DC_S);
            rval = IK_ERR_0005;
        } else if(p_ctx->organization.initial_flag == 0U) {
            amba_ik_system_print_uint32_5("[IK] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
            rval = IK_ERR_0002;
        } else {
            amba_ik_system_print_uint32_5("[IK] can't get context\n", DC_U, DC_U, DC_U, DC_U, DC_U);
        }
    }
    return rval;
}

uint32 ik_set_color_correction(uint32 context_id, const ik_color_correction_t *p_color_correction)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;
    const amba_img_dsp_debug_print_func_t *p_print_fun = ik_get_debug_print_func();
    if(p_print_fun != NULL) {
        p_print_fun->color_correction(context_id, p_color_correction);
    }

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((p_color_correction != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        rval = p_ctx->organization.filter_methods.set_color_correction(context_id, p_color_correction);
    } else {
        if(p_color_correction == NULL) {
            amba_ik_system_print_str_5("[IK] NULL pointer in %s\n", __func__, DC_S, DC_S, DC_S, DC_S);
            rval = IK_ERR_0005;
        } else if(p_ctx->organization.initial_flag == 0U) {
            amba_ik_system_print_uint32_5("[IK] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
            rval = IK_ERR_0002;
        } else {
            amba_ik_system_print_uint32_5("[IK] can't get context\n", DC_U, DC_U, DC_U, DC_U, DC_U);
        }
    }
    return rval;
}

uint32 ik_get_color_correction(uint32 context_id, ik_color_correction_t *p_color_correction)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((p_color_correction != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        rval = p_ctx->organization.filter_methods.get_color_correction(context_id, p_color_correction);
    } else {
        if(p_color_correction == NULL) {
            amba_ik_system_print_str_5("[IK] NULL pointer in %s\n", __func__, DC_S, DC_S, DC_S, DC_S);
            rval = IK_ERR_0005;
        } else if(p_ctx->organization.initial_flag == 0U) {
            amba_ik_system_print_uint32_5("[IK] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
            rval = IK_ERR_0002;
        } else {
            amba_ik_system_print_uint32_5("[IK] can't get context\n", DC_U, DC_U, DC_U, DC_U, DC_U);
        }
    }
    return rval;
}

uint32 ik_set_pre_cc_gain(uint32 context_id, const ik_pre_cc_gain_t *p_pre_cc_gain)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;
    const amba_img_dsp_debug_print_func_t *p_print_fun = ik_get_debug_print_func();
    if(p_print_fun != NULL) {
        p_print_fun->pre_cc_gain(context_id, p_pre_cc_gain);
    }

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((p_pre_cc_gain != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        rval = p_ctx->organization.filter_methods.set_pre_cc_gain(context_id, p_pre_cc_gain);
    } else {
        if(p_pre_cc_gain == NULL) {
            amba_ik_system_print_str_5("[IK] NULL pointer in %s\n", __func__, DC_S, DC_S, DC_S, DC_S);
            rval = IK_ERR_0005;
        } else if(p_ctx->organization.initial_flag == 0U) {
            amba_ik_system_print_uint32_5("[IK] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
            rval = IK_ERR_0002;
        } else {
            amba_ik_system_print_uint32_5("[IK] can't get context\n", DC_U, DC_U, DC_U, DC_U, DC_U);
        }
    }
    return rval;
}

uint32 ik_get_pre_cc_gain(uint32 context_id, ik_pre_cc_gain_t *p_pre_cc_gain)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((p_pre_cc_gain != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        rval = p_ctx->organization.filter_methods.get_pre_cc_gain(context_id, p_pre_cc_gain);
    } else {
        if(p_pre_cc_gain == NULL) {
            amba_ik_system_print_str_5("[IK] NULL pointer in %s\n", __func__, DC_S, DC_S, DC_S, DC_S);
            rval = IK_ERR_0005;
        } else if(p_ctx->organization.initial_flag == 0U) {
            amba_ik_system_print_uint32_5("[IK] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
            rval = IK_ERR_0002;
        } else {
            amba_ik_system_print_uint32_5("[IK] can't get context\n", DC_U, DC_U, DC_U, DC_U, DC_U);
        }
    }
    return rval;
}

uint32 ik_set_tone_curve(uint32 context_id, const ik_tone_curve_t *p_tone_curve)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;
    const amba_img_dsp_debug_print_func_t *p_print_fun = ik_get_debug_print_func();
    if(p_print_fun != NULL) {
        p_print_fun->tone_curve(context_id, p_tone_curve);
    }

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((p_tone_curve != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        rval = p_ctx->organization.filter_methods.set_tone_curve(context_id, p_tone_curve);
    } else {
        if(p_tone_curve == NULL) {
            amba_ik_system_print_str_5("[IK] NULL pointer in %s\n", __func__, DC_S, DC_S, DC_S, DC_S);
            rval = IK_ERR_0005;
        } else if(p_ctx->organization.initial_flag == 0U) {
            amba_ik_system_print_uint32_5("[IK] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
            rval = IK_ERR_0002;
        } else {
            amba_ik_system_print_uint32_5("[IK] can't get context\n", DC_U, DC_U, DC_U, DC_U, DC_U);
        }
    }
    return rval;
}

uint32 ik_get_tone_curve(uint32 context_id, ik_tone_curve_t *p_tone_curve)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((p_tone_curve != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        rval = p_ctx->organization.filter_methods.get_tone_curve(context_id, p_tone_curve);
    } else {
        if(p_tone_curve == NULL) {
            amba_ik_system_print_str_5("[IK] NULL pointer in %s\n", __func__, DC_S, DC_S, DC_S, DC_S);
            rval = IK_ERR_0005;
        } else if(p_ctx->organization.initial_flag == 0U) {
            amba_ik_system_print_uint32_5("[IK] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
            rval = IK_ERR_0002;
        } else {
            amba_ik_system_print_uint32_5("[IK] can't get context\n", DC_U, DC_U, DC_U, DC_U, DC_U);
        }
    }
    return rval;
}

uint32 ik_set_rgb_to_yuv_matrix(uint32 context_id, const ik_rgb_to_yuv_matrix_t *p_rgb_to_yuv_matrix)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;
    const amba_img_dsp_debug_print_func_t *p_print_fun = ik_get_debug_print_func();
    if(p_print_fun != NULL) {
        p_print_fun->rgb_to_yuv_matrix(context_id, p_rgb_to_yuv_matrix);
    }

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((p_rgb_to_yuv_matrix != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        rval = p_ctx->organization.filter_methods.set_rgb_to_yuv_matrix(context_id, p_rgb_to_yuv_matrix);
    } else {
        if(p_rgb_to_yuv_matrix == NULL) {
            amba_ik_system_print_str_5("[IK] NULL pointer in %s\n", __func__, DC_S, DC_S, DC_S, DC_S);
            rval = IK_ERR_0005;
        } else if(p_ctx->organization.initial_flag == 0U) {
            amba_ik_system_print_uint32_5("[IK] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
            rval = IK_ERR_0002;
        } else {
            amba_ik_system_print_uint32_5("[IK] can't get context\n", DC_U, DC_U, DC_U, DC_U, DC_U);
        }
    }
    return rval;
}

uint32 ik_get_rgb_to_yuv_matrix(uint32 context_id, ik_rgb_to_yuv_matrix_t *p_rgb_to_yuv_matrix)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((p_rgb_to_yuv_matrix != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        rval = p_ctx->organization.filter_methods.get_rgb_to_yuv_matrix(context_id, p_rgb_to_yuv_matrix);
    } else {
        if(p_rgb_to_yuv_matrix == NULL) {
            amba_ik_system_print_str_5("[IK] NULL pointer in %s\n", __func__, DC_S, DC_S, DC_S, DC_S);
            rval = IK_ERR_0005;
        } else if(p_ctx->organization.initial_flag == 0U) {
            amba_ik_system_print_uint32_5("[IK] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
            rval = IK_ERR_0002;
        } else {
            amba_ik_system_print_uint32_5("[IK] can't get context\n", DC_U, DC_U, DC_U, DC_U, DC_U);
        }
    }
    return rval;
}

uint32 ik_set_rgb_ir(uint32 context_id, const ik_rgb_ir_t *p_rgb_ir)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;
    const amba_img_dsp_debug_print_func_t *p_print_fun = ik_get_debug_print_func();
    if(p_print_fun != NULL) {
        p_print_fun->rgb_ir(context_id, p_rgb_ir);
    }

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((p_rgb_ir != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        rval = p_ctx->organization.filter_methods.set_rgb_ir(context_id, p_rgb_ir);
    } else {
        if(p_rgb_ir == NULL) {
            amba_ik_system_print_str_5("[IK] NULL pointer in %s\n", __func__, DC_S, DC_S, DC_S, DC_S);
            rval = IK_ERR_0005;
        } else if(p_ctx->organization.initial_flag == 0U) {
            amba_ik_system_print_uint32_5("[IK] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
            rval = IK_ERR_0002;
        } else {
            amba_ik_system_print_uint32_5("[IK] can't get context\n", DC_U, DC_U, DC_U, DC_U, DC_U);
        }
    }
    return rval;
}

uint32 ik_get_rgb_ir(uint32 context_id, ik_rgb_ir_t *p_rgb_ir)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((p_rgb_ir != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        rval = p_ctx->organization.filter_methods.get_rgb_ir(context_id, p_rgb_ir);
    } else {
        if(p_rgb_ir == NULL) {
            amba_ik_system_print_str_5("[IK] NULL pointer in %s\n", __func__, DC_S, DC_S, DC_S, DC_S);
            rval = IK_ERR_0005;
        } else if(p_ctx->organization.initial_flag == 0U) {
            amba_ik_system_print_uint32_5("[IK] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
            rval = IK_ERR_0002;
        } else {
            amba_ik_system_print_uint32_5("[IK] can't get context\n", DC_U, DC_U, DC_U, DC_U, DC_U);
        }
    }
    return rval;
}

uint32 ik_set_chroma_scale(uint32 context_id, const ik_chroma_scale_t *p_chroma_scale)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;
    const amba_img_dsp_debug_print_func_t *p_print_fun = ik_get_debug_print_func();
    if(p_print_fun != NULL) {
        p_print_fun->chroma_sacle(context_id, p_chroma_scale);
    }

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((p_chroma_scale != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        rval = p_ctx->organization.filter_methods.set_chroma_scale(context_id, p_chroma_scale);
    } else {
        if(p_chroma_scale == NULL) {
            amba_ik_system_print_str_5("[IK] NULL pointer in %s\n", __func__, DC_S, DC_S, DC_S, DC_S);
            rval = IK_ERR_0005;
        } else if(p_ctx->organization.initial_flag == 0U) {
            amba_ik_system_print_uint32_5("[IK] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
            rval = IK_ERR_0002;
        } else {
            amba_ik_system_print_uint32_5("[IK] can't get context\n", DC_U, DC_U, DC_U, DC_U, DC_U);
        }
    }
    return rval;
}

uint32 ik_get_chroma_scale(uint32 context_id, ik_chroma_scale_t *p_chroma_scale)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((p_chroma_scale != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        rval = p_ctx->organization.filter_methods.get_chroma_scale(context_id, p_chroma_scale);
    } else {
        if(p_chroma_scale == NULL) {
            amba_ik_system_print_str_5("[IK] NULL pointer in %s\n", __func__, DC_S, DC_S, DC_S, DC_S);
            rval = IK_ERR_0005;
        } else if(p_ctx->organization.initial_flag == 0U) {
            amba_ik_system_print_uint32_5("[IK] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
            rval = IK_ERR_0002;
        } else {
            amba_ik_system_print_uint32_5("[IK] can't get context\n", DC_U, DC_U, DC_U, DC_U, DC_U);
        }
    }
    return rval;
}

uint32 ik_set_chroma_median_filter(uint32 context_id, const ik_chroma_median_filter_t *p_chroma_median_filter)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;
    const amba_img_dsp_debug_print_func_t *p_print_fun = ik_get_debug_print_func();
    if(p_print_fun != NULL) {
        p_print_fun->chroma_median_filter(context_id, p_chroma_median_filter);
    }

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((p_chroma_median_filter != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        rval = p_ctx->organization.filter_methods.set_chroma_median_filter(context_id, p_chroma_median_filter);
    } else {
        if(p_chroma_median_filter == NULL) {
            amba_ik_system_print_str_5("[IK] NULL pointer in %s\n", __func__, DC_S, DC_S, DC_S, DC_S);
            rval = IK_ERR_0005;
        } else if(p_ctx->organization.initial_flag == 0U) {
            amba_ik_system_print_uint32_5("[IK] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
            rval = IK_ERR_0002;
        } else {
            amba_ik_system_print_uint32_5("[IK] can't get context\n", DC_U, DC_U, DC_U, DC_U, DC_U);
        }
    }
    return rval;
}

uint32 ik_get_chroma_median_filter(uint32 context_id, ik_chroma_median_filter_t *p_chroma_median_filter)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((p_chroma_median_filter != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        rval = p_ctx->organization.filter_methods.get_chroma_median_filter(context_id, p_chroma_median_filter);
    } else {
        if(p_chroma_median_filter == NULL) {
            amba_ik_system_print_str_5("[IK] NULL pointer in %s\n", __func__, DC_S, DC_S, DC_S, DC_S);
            rval = IK_ERR_0005;
        } else if(p_ctx->organization.initial_flag == 0U) {
            amba_ik_system_print_uint32_5("[IK] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
            rval = IK_ERR_0002;
        } else {
            amba_ik_system_print_uint32_5("[IK] can't get context\n", DC_U, DC_U, DC_U, DC_U, DC_U);
        }
    }
    return rval;
}

uint32 ik_set_fst_luma_process_mode(uint32 context_id, const ik_first_luma_process_mode_t *p_first_luma_process_mode)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;
    const amba_img_dsp_debug_print_func_t *p_print_fun = ik_get_debug_print_func();
    if(p_print_fun != NULL) {
        p_print_fun->first_luma_proc_mode(context_id, p_first_luma_process_mode);
    }

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((p_first_luma_process_mode != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        rval = p_ctx->organization.filter_methods.set_first_luma_processing_mode(context_id, p_first_luma_process_mode);
    } else {
        if(p_first_luma_process_mode == NULL) {
            amba_ik_system_print_str_5("[IK] NULL pointer in %s\n", __func__, DC_S, DC_S, DC_S, DC_S);
            rval = IK_ERR_0005;
        } else if(p_ctx->organization.initial_flag == 0U) {
            amba_ik_system_print_uint32_5("[IK] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
            rval = IK_ERR_0002;
        } else {
            amba_ik_system_print_uint32_5("[IK] can't get context\n", DC_U, DC_U, DC_U, DC_U, DC_U);
        }
    }
    return rval;
}

uint32 ik_get_fst_luma_process_mode(uint32 context_id, ik_first_luma_process_mode_t *p_first_luma_process_mode)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((p_first_luma_process_mode != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        rval = p_ctx->organization.filter_methods.get_first_luma_processing_mode(context_id, p_first_luma_process_mode);
    } else {
        if(p_first_luma_process_mode == NULL) {
            amba_ik_system_print_str_5("[IK] NULL pointer in %s\n", __func__, DC_S, DC_S, DC_S, DC_S);
            rval = IK_ERR_0005;
        } else if(p_ctx->organization.initial_flag == 0U) {
            amba_ik_system_print_uint32_5("[IK] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
            rval = IK_ERR_0002;
        } else {
            amba_ik_system_print_uint32_5("[IK] can't get context\n", DC_U, DC_U, DC_U, DC_U, DC_U);
        }
    }
    return rval;
}

uint32 ik_set_adv_spatial_filter(uint32 context_id, const ik_adv_spatial_filter_t *p_advance_spatial_filter)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;
    const amba_img_dsp_debug_print_func_t *p_print_fun = ik_get_debug_print_func();
    if(p_print_fun != NULL) {
        p_print_fun->adv_spat_fltr(context_id, p_advance_spatial_filter);
    }

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((p_advance_spatial_filter != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        rval = p_ctx->organization.filter_methods.set_advance_spatial_filter(context_id, p_advance_spatial_filter);
    } else {
        if(p_advance_spatial_filter == NULL) {
            amba_ik_system_print_str_5("[IK] NULL pointer in %s\n", __func__, DC_S, DC_S, DC_S, DC_S);
            rval = IK_ERR_0005;
        } else if(p_ctx->organization.initial_flag == 0U) {
            amba_ik_system_print_uint32_5("[IK] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
            rval = IK_ERR_0002;
        } else {
            amba_ik_system_print_uint32_5("[IK] can't get context\n", DC_U, DC_U, DC_U, DC_U, DC_U);
        }
    }
    return rval;
}

uint32 ik_get_adv_spatial_filter(uint32 context_id, ik_adv_spatial_filter_t *p_advance_spatial_filter)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((p_advance_spatial_filter != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        rval = p_ctx->organization.filter_methods.get_advance_spatial_filter(context_id, p_advance_spatial_filter);
    } else {
        if(p_advance_spatial_filter == NULL) {
            amba_ik_system_print_str_5("[IK] NULL pointer in %s\n", __func__, DC_S, DC_S, DC_S, DC_S);
            rval = IK_ERR_0005;
        } else if(p_ctx->organization.initial_flag == 0U) {
            amba_ik_system_print_uint32_5("[IK] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
            rval = IK_ERR_0002;
        } else {
            amba_ik_system_print_uint32_5("[IK] can't get context\n", DC_U, DC_U, DC_U, DC_U, DC_U);
        }
    }
    return rval;
}

uint32 ik_set_fst_shp_both(uint32 context_id, const ik_first_sharpen_both_t *p_first_sharpen_both)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;
    const amba_img_dsp_debug_print_func_t *p_print_fun = ik_get_debug_print_func();
    if(p_print_fun != NULL) {
        p_print_fun->first_sharpen_both(context_id, p_first_sharpen_both);
    }

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((p_first_sharpen_both != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        rval = p_ctx->organization.filter_methods.set_fstshpns_both(context_id, p_first_sharpen_both);
    } else {
        if(p_first_sharpen_both == NULL) {
            amba_ik_system_print_str_5("[IK] NULL pointer in %s\n", __func__, DC_S, DC_S, DC_S, DC_S);
            rval = IK_ERR_0005;
        } else if(p_ctx->organization.initial_flag == 0U) {
            amba_ik_system_print_uint32_5("[IK] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
            rval = IK_ERR_0002;
        } else {
            amba_ik_system_print_uint32_5("[IK] can't get context\n", DC_U, DC_U, DC_U, DC_U, DC_U);
        }
    }
    return rval;
}

uint32 ik_get_fst_shp_both(uint32 context_id, ik_first_sharpen_both_t *p_first_sharpen_both)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((p_first_sharpen_both != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        rval = p_ctx->organization.filter_methods.get_fstshpns_both(context_id, p_first_sharpen_both);
    } else {
        if(p_first_sharpen_both == NULL) {
            amba_ik_system_print_str_5("[IK] NULL pointer in %s\n", __func__, DC_S, DC_S, DC_S, DC_S);
            rval = IK_ERR_0005;
        } else if(p_ctx->organization.initial_flag == 0U) {
            amba_ik_system_print_uint32_5("[IK] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
            rval = IK_ERR_0002;
        } else {
            amba_ik_system_print_uint32_5("[IK] can't get context\n", DC_U, DC_U, DC_U, DC_U, DC_U);
        }
    }
    return rval;
}

uint32 ik_set_fst_shp_noise(uint32 context_id, const ik_first_sharpen_noise_t *p_first_sharpen_noise)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;
    const amba_img_dsp_debug_print_func_t *p_print_fun = ik_get_debug_print_func();
    if(p_print_fun != NULL) {
        p_print_fun->first_sharpen_noise(context_id, p_first_sharpen_noise);
    }

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((p_first_sharpen_noise != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        rval = p_ctx->organization.filter_methods.set_fstshpns_noise(context_id, p_first_sharpen_noise);
    } else {
        if(p_first_sharpen_noise == NULL) {
            amba_ik_system_print_str_5("[IK] NULL pointer in %s\n", __func__, DC_S, DC_S, DC_S, DC_S);
            rval = IK_ERR_0005;
        } else if(p_ctx->organization.initial_flag == 0U) {
            amba_ik_system_print_uint32_5("[IK] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
            rval = IK_ERR_0002;
        } else {
            amba_ik_system_print_uint32_5("[IK] can't get context\n", DC_U, DC_U, DC_U, DC_U, DC_U);
        }
    }
    return rval;
}

uint32 ik_get_fst_shp_noise(uint32 context_id, ik_first_sharpen_noise_t *p_first_sharpen_noise)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((p_first_sharpen_noise != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        rval = p_ctx->organization.filter_methods.get_fstshpns_noise(context_id, p_first_sharpen_noise);
    } else {
        if(p_first_sharpen_noise == NULL) {
            amba_ik_system_print_str_5("[IK] NULL pointer in %s\n", __func__, DC_S, DC_S, DC_S, DC_S);
            rval = IK_ERR_0005;
        } else if(p_ctx->organization.initial_flag == 0U) {
            amba_ik_system_print_uint32_5("[IK] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
            rval = IK_ERR_0002;
        } else {
            amba_ik_system_print_uint32_5("[IK] can't get context\n", DC_U, DC_U, DC_U, DC_U, DC_U);
        }
    }
    return rval;
}

uint32 ik_set_fst_shp_fir(uint32 context_id, const ik_first_sharpen_fir_t *p_first_sharpen_fir)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;
    const amba_img_dsp_debug_print_func_t *p_print_fun = ik_get_debug_print_func();
    if(p_print_fun != NULL) {
        p_print_fun->first_sharpen_fir(context_id, p_first_sharpen_fir);
    }

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((p_first_sharpen_fir != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        rval = p_ctx->organization.filter_methods.set_fstshpns_fir(context_id, p_first_sharpen_fir);
    } else {
        if(p_first_sharpen_fir == NULL) {
            amba_ik_system_print_str_5("[IK] NULL pointer in %s\n", __func__, DC_S, DC_S, DC_S, DC_S);
            rval = IK_ERR_0005;
        } else if(p_ctx->organization.initial_flag == 0U) {
            amba_ik_system_print_uint32_5("[IK] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
            rval = IK_ERR_0002;
        } else {
            amba_ik_system_print_uint32_5("[IK] can't get context\n", DC_U, DC_U, DC_U, DC_U, DC_U);
        }
    }
    return rval;
}

uint32 ik_get_fst_shp_fir(uint32 context_id, ik_first_sharpen_fir_t *p_first_sharpen_fir)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((p_first_sharpen_fir != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        rval = p_ctx->organization.filter_methods.get_fstshpns_fir(context_id, p_first_sharpen_fir);
    } else {
        if(p_first_sharpen_fir == NULL) {
            amba_ik_system_print_str_5("[IK] NULL pointer in %s\n", __func__, DC_S, DC_S, DC_S, DC_S);
            rval = IK_ERR_0005;
        } else if(p_ctx->organization.initial_flag == 0U) {
            amba_ik_system_print_uint32_5("[IK] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
            rval = IK_ERR_0002;
        } else {
            amba_ik_system_print_uint32_5("[IK] can't get context\n", DC_U, DC_U, DC_U, DC_U, DC_U);
        }
    }
    return rval;
}

uint32 ik_set_fst_shp_coring(uint32 context_id, const ik_first_sharpen_coring_t *p_first_sharpen_coring)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;
    const amba_img_dsp_debug_print_func_t *p_print_fun = ik_get_debug_print_func();
    if(p_print_fun != NULL) {
        p_print_fun->first_sharpen_coring(context_id, p_first_sharpen_coring);
    }

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((p_first_sharpen_coring != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        rval = p_ctx->organization.filter_methods.set_fstshpns_coring(context_id, p_first_sharpen_coring);
    } else {
        if(p_first_sharpen_coring == NULL) {
            amba_ik_system_print_str_5("[IK] NULL pointer in %s\n", __func__, DC_S, DC_S, DC_S, DC_S);
            rval = IK_ERR_0005;
        } else if(p_ctx->organization.initial_flag == 0U) {
            amba_ik_system_print_uint32_5("[IK] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
            rval = IK_ERR_0002;
        } else {
            amba_ik_system_print_uint32_5("[IK] can't get context\n", DC_U, DC_U, DC_U, DC_U, DC_U);
        }
    }
    return rval;
}

uint32 ik_get_fst_shp_coring(uint32 context_id, ik_first_sharpen_coring_t *p_first_sharpen_coring)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((p_first_sharpen_coring != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        rval = p_ctx->organization.filter_methods.get_fstshpns_coring(context_id, p_first_sharpen_coring);
    } else {
        if(p_first_sharpen_coring == NULL) {
            amba_ik_system_print_str_5("[IK] NULL pointer in %s\n", __func__, DC_S, DC_S, DC_S, DC_S);
            rval = IK_ERR_0005;
        } else if(p_ctx->organization.initial_flag == 0U) {
            amba_ik_system_print_uint32_5("[IK] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
            rval = IK_ERR_0002;
        } else {
            amba_ik_system_print_uint32_5("[IK] can't get context\n", DC_U, DC_U, DC_U, DC_U, DC_U);
        }
    }
    return rval;
}

uint32 ik_set_fst_shp_coring_idx_scale(uint32 context_id, const ik_first_sharpen_coring_idx_scale_t *p_first_sharpen_coring_idx_scale)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;
    const amba_img_dsp_debug_print_func_t *p_print_fun = ik_get_debug_print_func();
    if(p_print_fun != NULL) {
        p_print_fun->fstshpns_cor_idx_scl(context_id, p_first_sharpen_coring_idx_scale);
    }

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((p_first_sharpen_coring_idx_scale != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        rval = p_ctx->organization.filter_methods.set_fstshpns_coring_index_scale(context_id, p_first_sharpen_coring_idx_scale);
    } else {
        if(p_first_sharpen_coring_idx_scale == NULL) {
            amba_ik_system_print_str_5("[IK] NULL pointer in %s\n", __func__, DC_S, DC_S, DC_S, DC_S);
            rval = IK_ERR_0005;
        } else if(p_ctx->organization.initial_flag == 0U) {
            amba_ik_system_print_uint32_5("[IK] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
            rval = IK_ERR_0002;
        } else {
            amba_ik_system_print_uint32_5("[IK] can't get context\n", DC_U, DC_U, DC_U, DC_U, DC_U);
        }
    }
    return rval;
}

uint32 ik_get_fst_shp_coring_idx_scale(uint32 context_id, ik_first_sharpen_coring_idx_scale_t *p_first_sharpen_coring_idx_scale)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((p_first_sharpen_coring_idx_scale != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        rval = p_ctx->organization.filter_methods.get_fstshpns_coring_index_scale(context_id, p_first_sharpen_coring_idx_scale);
    } else {
        if(p_first_sharpen_coring_idx_scale == NULL) {
            amba_ik_system_print_str_5("[IK] NULL pointer in %s\n", __func__, DC_S, DC_S, DC_S, DC_S);
            rval = IK_ERR_0005;
        } else if(p_ctx->organization.initial_flag == 0U) {
            amba_ik_system_print_uint32_5("[IK] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
            rval = IK_ERR_0002;
        } else {
            amba_ik_system_print_uint32_5("[IK] can't get context\n", DC_U, DC_U, DC_U, DC_U, DC_U);
        }
    }
    return rval;
}

uint32 ik_set_fst_shp_min_coring_rslt(uint32 context_id, const ik_first_sharpen_min_coring_result_t *p_first_sharpen_min_coring_result)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;
    const amba_img_dsp_debug_print_func_t *p_print_fun = ik_get_debug_print_func();
    if(p_print_fun != NULL) {
        p_print_fun->fstshpns_min_cor_rst(context_id, p_first_sharpen_min_coring_result);
    }

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((p_first_sharpen_min_coring_result != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        rval = p_ctx->organization.filter_methods.set_fstshpns_min_coring_result(context_id, p_first_sharpen_min_coring_result);
    } else {
        if(p_first_sharpen_min_coring_result == NULL) {
            amba_ik_system_print_str_5("[IK] NULL pointer in %s\n", __func__, DC_S, DC_S, DC_S, DC_S);
            rval = IK_ERR_0005;
        } else if(p_ctx->organization.initial_flag == 0U) {
            amba_ik_system_print_uint32_5("[IK] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
            rval = IK_ERR_0002;
        } else {
            amba_ik_system_print_uint32_5("[IK] can't get context\n", DC_U, DC_U, DC_U, DC_U, DC_U);
        }
    }
    return rval;
}

uint32 ik_get_fst_shp_min_coring_rslt(uint32 context_id, ik_first_sharpen_min_coring_result_t *p_first_sharpen_min_coring_result)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((p_first_sharpen_min_coring_result != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        rval = p_ctx->organization.filter_methods.get_fstshpns_min_coring_result(context_id, p_first_sharpen_min_coring_result);
    } else {
        if(p_first_sharpen_min_coring_result == NULL) {
            amba_ik_system_print_str_5("[IK] NULL pointer in %s\n", __func__, DC_S, DC_S, DC_S, DC_S);
            rval = IK_ERR_0005;
        } else if(p_ctx->organization.initial_flag == 0U) {
            amba_ik_system_print_uint32_5("[IK] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
            rval = IK_ERR_0002;
        } else {
            amba_ik_system_print_uint32_5("[IK] can't get context\n", DC_U, DC_U, DC_U, DC_U, DC_U);
        }
    }
    return rval;
}

uint32 ik_set_fst_shp_max_coring_rslt(uint32 context_id, const ik_first_sharpen_max_coring_result_t *p_first_sharpen_max_coring_result)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;
    const amba_img_dsp_debug_print_func_t *p_print_fun = ik_get_debug_print_func();
    if(p_print_fun != NULL) {
        p_print_fun->fstshpns_max_cor_rst(context_id, p_first_sharpen_max_coring_result);
    }

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((p_first_sharpen_max_coring_result != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        rval = p_ctx->organization.filter_methods.set_fstshpns_max_coring_result(context_id, p_first_sharpen_max_coring_result);
    } else {
        if(p_first_sharpen_max_coring_result == NULL) {
            amba_ik_system_print_str_5("[IK] NULL pointer in %s\n", __func__, DC_S, DC_S, DC_S, DC_S);
            rval = IK_ERR_0005;
        } else if(p_ctx->organization.initial_flag == 0U) {
            amba_ik_system_print_uint32_5("[IK] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
            rval = IK_ERR_0002;
        } else {
            amba_ik_system_print_uint32_5("[IK] can't get context\n", DC_U, DC_U, DC_U, DC_U, DC_U);
        }
    }
    return rval;
}

uint32 ik_get_fst_shp_max_coring_rslt(uint32 context_id, ik_first_sharpen_max_coring_result_t *p_first_sharpen_max_coring_result)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((p_first_sharpen_max_coring_result != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        rval = p_ctx->organization.filter_methods.get_fstshpns_max_coring_result(context_id, p_first_sharpen_max_coring_result);
    } else {
        if(p_first_sharpen_max_coring_result == NULL) {
            amba_ik_system_print_str_5("[IK] NULL pointer in %s\n", __func__, DC_S, DC_S, DC_S, DC_S);
            rval = IK_ERR_0005;
        } else if(p_ctx->organization.initial_flag == 0U) {
            amba_ik_system_print_uint32_5("[IK] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
            rval = IK_ERR_0002;
        } else {
            amba_ik_system_print_uint32_5("[IK] can't get context\n", DC_U, DC_U, DC_U, DC_U, DC_U);
        }
    }
    return rval;
}

uint32 ik_set_fst_shp_scale_coring(uint32 context_id, const ik_first_sharpen_scale_coring_t *p_first_sharpen_scale_coring)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;
    const amba_img_dsp_debug_print_func_t *p_print_fun = ik_get_debug_print_func();
    if(p_print_fun != NULL) {
        p_print_fun->fstshpns_scl_cor(context_id, p_first_sharpen_scale_coring);
    }

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((p_first_sharpen_scale_coring != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        rval = p_ctx->organization.filter_methods.set_fstshpns_scale_coring(context_id, p_first_sharpen_scale_coring);
    } else {
        if(p_first_sharpen_scale_coring == NULL) {
            amba_ik_system_print_str_5("[IK] NULL pointer in %s\n", __func__, DC_S, DC_S, DC_S, DC_S);
            rval = IK_ERR_0005;
        } else if(p_ctx->organization.initial_flag == 0U) {
            amba_ik_system_print_uint32_5("[IK] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
            rval = IK_ERR_0002;
        } else {
            amba_ik_system_print_uint32_5("[IK] can't get context\n", DC_U, DC_U, DC_U, DC_U, DC_U);
        }
    }
    return rval;
}

uint32 ik_get_fst_shp_scale_coring(uint32 context_id, ik_first_sharpen_scale_coring_t *p_first_sharpen_scale_coring)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((p_first_sharpen_scale_coring != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        rval = p_ctx->organization.filter_methods.get_fstshpns_scale_coring(context_id, p_first_sharpen_scale_coring);
    } else {
        if(p_first_sharpen_scale_coring == NULL) {
            amba_ik_system_print_str_5("[IK] NULL pointer in %s\n", __func__, DC_S, DC_S, DC_S, DC_S);
            rval = IK_ERR_0005;
        } else if(p_ctx->organization.initial_flag == 0U) {
            amba_ik_system_print_uint32_5("[IK] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
            rval = IK_ERR_0002;
        } else {
            amba_ik_system_print_uint32_5("[IK] can't get context\n", DC_U, DC_U, DC_U, DC_U, DC_U);
        }
    }
    return rval;
}

uint32 ik_set_fnl_shp_both(uint32 context_id, const ik_final_sharpen_both_t *p_final_sharpen_both)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;
    const amba_img_dsp_debug_print_func_t *p_print_fun = ik_get_debug_print_func();
    if(p_print_fun != NULL) {
        p_print_fun->final_sharpen_both(context_id, p_final_sharpen_both);
    }

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((p_final_sharpen_both != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        rval = p_ctx->organization.filter_methods.set_fnlshpns_both(context_id, p_final_sharpen_both);
    } else {
        if(p_final_sharpen_both == NULL) {
            amba_ik_system_print_str_5("[IK] NULL pointer in %s\n", __func__, DC_S, DC_S, DC_S, DC_S);
            rval = IK_ERR_0005;
        } else if(p_ctx->organization.initial_flag == 0U) {
            amba_ik_system_print_uint32_5("[IK] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
            rval = IK_ERR_0002;
        } else {
            amba_ik_system_print_uint32_5("[IK] can't get context\n", DC_U, DC_U, DC_U, DC_U, DC_U);
        }
    }
    return rval;
}

uint32 ik_get_fnl_shp_both(uint32 context_id, ik_final_sharpen_both_t *p_final_sharpen_both)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((p_final_sharpen_both != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        rval = p_ctx->organization.filter_methods.get_fnlshpns_both(context_id, p_final_sharpen_both);
    } else {
        if(p_final_sharpen_both == NULL) {
            amba_ik_system_print_str_5("[IK] NULL pointer in %s\n", __func__, DC_S, DC_S, DC_S, DC_S);
            rval = IK_ERR_0005;
        } else if(p_ctx->organization.initial_flag == 0U) {
            amba_ik_system_print_uint32_5("[IK] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
            rval = IK_ERR_0002;
        } else {
            amba_ik_system_print_uint32_5("[IK] can't get context\n", DC_U, DC_U, DC_U, DC_U, DC_U);
        }
    }
    return rval;
}

uint32 ik_set_fnl_shp_noise(uint32 context_id, const ik_final_sharpen_noise_t *p_final_sharpen_noise)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;
    const amba_img_dsp_debug_print_func_t *p_print_fun = ik_get_debug_print_func();
    if(p_print_fun != NULL) {
        p_print_fun->final_sharpen_noise(context_id, p_final_sharpen_noise);
    }

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((p_final_sharpen_noise != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        rval = p_ctx->organization.filter_methods.set_fnlshpns_noise(context_id, p_final_sharpen_noise);
    } else {
        if(p_final_sharpen_noise == NULL) {
            amba_ik_system_print_str_5("[IK] NULL pointer in %s\n", __func__, DC_S, DC_S, DC_S, DC_S);
            rval = IK_ERR_0005;
        } else if(p_ctx->organization.initial_flag == 0U) {
            amba_ik_system_print_uint32_5("[IK] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
            rval = IK_ERR_0002;
        } else {
            amba_ik_system_print_uint32_5("[IK] can't get context\n", DC_U, DC_U, DC_U, DC_U, DC_U);
        }
    }
    return rval;
}

uint32 ik_get_fnl_shp_noise(uint32 context_id, ik_final_sharpen_noise_t *p_final_sharpen_noise)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((p_final_sharpen_noise != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        rval = p_ctx->organization.filter_methods.get_fnlshpns_noise(context_id, p_final_sharpen_noise);
    } else {
        if(p_final_sharpen_noise == NULL) {
            amba_ik_system_print_str_5("[IK] NULL pointer in %s\n", __func__, DC_S, DC_S, DC_S, DC_S);
            rval = IK_ERR_0005;
        } else if(p_ctx->organization.initial_flag == 0U) {
            amba_ik_system_print_uint32_5("[IK] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
            rval = IK_ERR_0002;
        } else {
            amba_ik_system_print_uint32_5("[IK] can't get context\n", DC_U, DC_U, DC_U, DC_U, DC_U);
        }
    }
    return rval;
}

uint32 ik_set_fnl_shp_fir(uint32 context_id, const ik_final_sharpen_fir_t *p_final_sharpen_fir)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;
    const amba_img_dsp_debug_print_func_t *p_print_fun = ik_get_debug_print_func();
    if(p_print_fun != NULL) {
        p_print_fun->final_sharpen_fir(context_id, p_final_sharpen_fir);
    }

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((p_final_sharpen_fir != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        rval = p_ctx->organization.filter_methods.set_fnlshpns_fir(context_id, p_final_sharpen_fir);
    } else {
        if(p_final_sharpen_fir == NULL) {
            amba_ik_system_print_str_5("[IK] NULL pointer in %s\n", __func__, DC_S, DC_S, DC_S, DC_S);
            rval = IK_ERR_0005;
        } else if(p_ctx->organization.initial_flag == 0U) {
            amba_ik_system_print_uint32_5("[IK] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
            rval = IK_ERR_0002;
        } else {
            amba_ik_system_print_uint32_5("[IK] can't get context\n", DC_U, DC_U, DC_U, DC_U, DC_U);
        }
    }
    return rval;
}

uint32 ik_get_fnl_shp_fir(uint32 context_id, ik_final_sharpen_fir_t *p_final_sharpen_fir)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((p_final_sharpen_fir != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        rval = p_ctx->organization.filter_methods.get_fnlshpns_fir(context_id, p_final_sharpen_fir);
    } else {
        if(p_final_sharpen_fir == NULL) {
            amba_ik_system_print_str_5("[IK] NULL pointer in %s\n", __func__, DC_S, DC_S, DC_S, DC_S);
            rval = IK_ERR_0005;
        } else if(p_ctx->organization.initial_flag == 0U) {
            amba_ik_system_print_uint32_5("[IK] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
            rval = IK_ERR_0002;
        } else {
            amba_ik_system_print_uint32_5("[IK] can't get context\n", DC_U, DC_U, DC_U, DC_U, DC_U);
        }
    }
    return rval;
}

uint32 ik_set_fnl_shp_coring(uint32 context_id, const ik_final_sharpen_coring_t *p_final_sharpen_coring)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;
    const amba_img_dsp_debug_print_func_t *p_print_fun = ik_get_debug_print_func();
    if(p_print_fun != NULL) {
        p_print_fun->final_sharpen_coring(context_id, p_final_sharpen_coring);
    }

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((p_final_sharpen_coring != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        rval = p_ctx->organization.filter_methods.set_fnlshpns_coring(context_id, p_final_sharpen_coring);
    } else {
        if(p_final_sharpen_coring == NULL) {
            amba_ik_system_print_str_5("[IK] NULL pointer in %s\n", __func__, DC_S, DC_S, DC_S, DC_S);
            rval = IK_ERR_0005;
        } else if(p_ctx->organization.initial_flag == 0U) {
            amba_ik_system_print_uint32_5("[IK] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
            rval = IK_ERR_0002;
        } else {
            amba_ik_system_print_uint32_5("[IK] can't get context\n", DC_U, DC_U, DC_U, DC_U, DC_U);
        }
    }
    return rval;
}

uint32 ik_get_fnl_shp_coring(uint32 context_id, ik_final_sharpen_coring_t *p_final_sharpen_coring)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((p_final_sharpen_coring != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        rval = p_ctx->organization.filter_methods.get_fnlshpns_coring(context_id, p_final_sharpen_coring);
    } else {
        if(p_final_sharpen_coring == NULL) {
            amba_ik_system_print_str_5("[IK] NULL pointer in %s\n", __func__, DC_S, DC_S, DC_S, DC_S);
            rval = IK_ERR_0005;
        } else if(p_ctx->organization.initial_flag == 0U) {
            amba_ik_system_print_uint32_5("[IK] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
            rval = IK_ERR_0002;
        } else {
            amba_ik_system_print_uint32_5("[IK] can't get context\n", DC_U, DC_U, DC_U, DC_U, DC_U);
        }
    }
    return rval;
}

uint32 ik_set_fnl_shp_coring_idx_scale(uint32 context_id, const ik_final_sharpen_coring_idx_scale_t *p_final_sharpen_coring_idx_scale)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;
    const amba_img_dsp_debug_print_func_t *p_print_fun = ik_get_debug_print_func();
    if(p_print_fun != NULL) {
        p_print_fun->fnlshpns_cor_idx_scl(context_id, p_final_sharpen_coring_idx_scale);
    }

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((p_final_sharpen_coring_idx_scale != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        rval = p_ctx->organization.filter_methods.set_fnlshpns_coring_index_scale(context_id, p_final_sharpen_coring_idx_scale);
    } else {
        if(p_final_sharpen_coring_idx_scale == NULL) {
            amba_ik_system_print_str_5("[IK] NULL pointer in %s\n", __func__, DC_S, DC_S, DC_S, DC_S);
            rval = IK_ERR_0005;
        } else if(p_ctx->organization.initial_flag == 0U) {
            amba_ik_system_print_uint32_5("[IK] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
            rval = IK_ERR_0002;
        } else {
            amba_ik_system_print_uint32_5("[IK] can't get context\n", DC_U, DC_U, DC_U, DC_U, DC_U);
        }
    }
    return rval;
}

uint32 ik_get_fnl_shp_coring_idx_scale(uint32 context_id, ik_final_sharpen_coring_idx_scale_t *p_final_sharpen_coring_idx_scale)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((p_final_sharpen_coring_idx_scale != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        rval = p_ctx->organization.filter_methods.get_fnlshpns_coring_index_scale(context_id, p_final_sharpen_coring_idx_scale);
    } else {
        if(p_final_sharpen_coring_idx_scale == NULL) {
            amba_ik_system_print_str_5("[IK] NULL pointer in %s\n", __func__, DC_S, DC_S, DC_S, DC_S);
            rval = IK_ERR_0005;
        } else if(p_ctx->organization.initial_flag == 0U) {
            amba_ik_system_print_uint32_5("[IK] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
            rval = IK_ERR_0002;
        } else {
            amba_ik_system_print_uint32_5("[IK] can't get context\n", DC_U, DC_U, DC_U, DC_U, DC_U);
        }
    }
    return rval;
}

uint32 ik_set_fnl_shp_min_coring_rslt(uint32 context_id, const ik_final_sharpen_min_coring_result_t *p_final_sharpen_min_coring_result)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;
    const amba_img_dsp_debug_print_func_t *p_print_fun = ik_get_debug_print_func();
    if(p_print_fun != NULL) {
        p_print_fun->fnlshpns_min_cor_rst(context_id, p_final_sharpen_min_coring_result);
    }

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((p_final_sharpen_min_coring_result != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        rval = p_ctx->organization.filter_methods.set_fnlshpns_min_coring_result(context_id, p_final_sharpen_min_coring_result);
    } else {
        if(p_final_sharpen_min_coring_result == NULL) {
            amba_ik_system_print_str_5("[IK] NULL pointer in %s\n", __func__, DC_S, DC_S, DC_S, DC_S);
            rval = IK_ERR_0005;
        } else if(p_ctx->organization.initial_flag == 0U) {
            amba_ik_system_print_uint32_5("[IK] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
            rval = IK_ERR_0002;
        } else {
            amba_ik_system_print_uint32_5("[IK] can't get context\n", DC_U, DC_U, DC_U, DC_U, DC_U);
        }
    }
    return rval;
}

uint32 ik_get_fnl_shp_min_coring_rslt(uint32 context_id, ik_final_sharpen_min_coring_result_t *p_final_sharpen_min_coring_result)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((p_final_sharpen_min_coring_result != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        rval = p_ctx->organization.filter_methods.get_fnlshpns_min_coring_result(context_id, p_final_sharpen_min_coring_result);
    } else {
        if(p_final_sharpen_min_coring_result == NULL) {
            amba_ik_system_print_str_5("[IK] NULL pointer in %s\n", __func__, DC_S, DC_S, DC_S, DC_S);
            rval = IK_ERR_0005;
        } else if(p_ctx->organization.initial_flag == 0U) {
            amba_ik_system_print_uint32_5("[IK] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
            rval = IK_ERR_0002;
        } else {
            amba_ik_system_print_uint32_5("[IK] can't get context\n", DC_U, DC_U, DC_U, DC_U, DC_U);
        }
    }
    return rval;
}

uint32 ik_set_fnl_shp_max_coring_rslt(uint32 context_id, const ik_final_sharpen_max_coring_result_t *p_final_sharpen_max_coring_result)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;
    const amba_img_dsp_debug_print_func_t *p_print_fun = ik_get_debug_print_func();
    if(p_print_fun != NULL) {
        p_print_fun->fnlshpns_max_cor_rst(context_id, p_final_sharpen_max_coring_result);
    }

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((p_final_sharpen_max_coring_result != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        rval = p_ctx->organization.filter_methods.set_fnlshpns_max_coring_result(context_id, p_final_sharpen_max_coring_result);
    } else {
        if(p_final_sharpen_max_coring_result == NULL) {
            amba_ik_system_print_str_5("[IK] NULL pointer in %s\n", __func__, DC_S, DC_S, DC_S, DC_S);
            rval = IK_ERR_0005;
        } else if(p_ctx->organization.initial_flag == 0U) {
            amba_ik_system_print_uint32_5("[IK] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
            rval = IK_ERR_0002;
        } else {
            amba_ik_system_print_uint32_5("[IK] can't get context\n", DC_U, DC_U, DC_U, DC_U, DC_U);
        }
    }
    return rval;
}

uint32 ik_get_fnl_shp_max_coring_rslt(uint32 context_id, ik_final_sharpen_max_coring_result_t *p_final_sharpen_max_coring_result)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((p_final_sharpen_max_coring_result != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        rval = p_ctx->organization.filter_methods.get_fnlshpns_max_coring_result(context_id, p_final_sharpen_max_coring_result);
    } else {
        if(p_final_sharpen_max_coring_result == NULL) {
            amba_ik_system_print_str_5("[IK] NULL pointer in %s\n", __func__, DC_S, DC_S, DC_S, DC_S);
            rval = IK_ERR_0005;
        } else if(p_ctx->organization.initial_flag == 0U) {
            amba_ik_system_print_uint32_5("[IK] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
            rval = IK_ERR_0002;
        } else {
            amba_ik_system_print_uint32_5("[IK] can't get context\n", DC_U, DC_U, DC_U, DC_U, DC_U);
        }
    }
    return rval;
}

uint32 ik_set_fnl_shp_scale_coring(uint32 context_id, const ik_final_sharpen_scale_coring_t *p_final_sharpen_scale_coring)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;
    const amba_img_dsp_debug_print_func_t *p_print_fun = ik_get_debug_print_func();
    if(p_print_fun != NULL) {
        p_print_fun->fnlshpns_scl_cor(context_id, p_final_sharpen_scale_coring);
    }

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((p_final_sharpen_scale_coring != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        rval = p_ctx->organization.filter_methods.set_fnlshpns_scale_coring(context_id, p_final_sharpen_scale_coring);
    } else {
        if(p_final_sharpen_scale_coring == NULL) {
            amba_ik_system_print_str_5("[IK] NULL pointer in %s\n", __func__, DC_S, DC_S, DC_S, DC_S);
            rval = IK_ERR_0005;
        } else if(p_ctx->organization.initial_flag == 0U) {
            amba_ik_system_print_uint32_5("[IK] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
            rval = IK_ERR_0002;
        } else {
            amba_ik_system_print_uint32_5("[IK] can't get context\n", DC_U, DC_U, DC_U, DC_U, DC_U);
        }
    }
    return rval;
}

uint32 ik_get_fnl_shp_scale_coring(uint32 context_id, ik_final_sharpen_scale_coring_t *p_final_sharpen_scale_coring)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((p_final_sharpen_scale_coring != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        rval = p_ctx->organization.filter_methods.get_fnlshpns_scale_coring(context_id, p_final_sharpen_scale_coring);
    } else {
        if(p_final_sharpen_scale_coring == NULL) {
            amba_ik_system_print_str_5("[IK] NULL pointer in %s\n", __func__, DC_S, DC_S, DC_S, DC_S);
            rval = IK_ERR_0005;
        } else if(p_ctx->organization.initial_flag == 0U) {
            amba_ik_system_print_uint32_5("[IK] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
            rval = IK_ERR_0002;
        } else {
            amba_ik_system_print_uint32_5("[IK] can't get context\n", DC_U, DC_U, DC_U, DC_U, DC_U);
        }
    }
    return rval;
}

uint32 ik_set_fnl_shp_three_d_table(uint32 context_id, const ik_final_sharpen_both_three_d_table_t *p_final_sharpen_both_three_d_table)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;
    const amba_img_dsp_debug_print_func_t *p_print_fun = ik_get_debug_print_func();
    if(p_print_fun != NULL) {
        p_print_fun->final_sharpen_both_three_d_table(context_id, p_final_sharpen_both_three_d_table);
    }

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((p_final_sharpen_both_three_d_table != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        rval = p_ctx->organization.filter_methods.set_fnlshpns_both_tdt(context_id, p_final_sharpen_both_three_d_table);
    } else {
        if(p_final_sharpen_both_three_d_table == NULL) {
            amba_ik_system_print_str_5("[IK] NULL pointer in %s\n", __func__, DC_S, DC_S, DC_S, DC_S);
            rval = IK_ERR_0005;
        } else if(p_ctx->organization.initial_flag == 0U) {
            amba_ik_system_print_uint32_5("[IK] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
            rval = IK_ERR_0002;
        } else {
            amba_ik_system_print_uint32_5("[IK] can't get context\n", DC_U, DC_U, DC_U, DC_U, DC_U);
        }
    }
    return rval;
}

uint32 ik_get_fnl_shp_three_d_table(uint32 context_id, ik_final_sharpen_both_three_d_table_t *p_final_sharpen_both_three_d_table)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((p_final_sharpen_both_three_d_table != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        rval = p_ctx->organization.filter_methods.get_fnlshpns_both_tdt(context_id, p_final_sharpen_both_three_d_table);
    } else {
        if(p_final_sharpen_both_three_d_table == NULL) {
            amba_ik_system_print_str_5("[IK] NULL pointer in %s\n", __func__, DC_S, DC_S, DC_S, DC_S);
            rval = IK_ERR_0005;
        } else if(p_ctx->organization.initial_flag == 0U) {
            amba_ik_system_print_uint32_5("[IK] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
            rval = IK_ERR_0002;
        } else {
            amba_ik_system_print_uint32_5("[IK] can't get context\n", DC_U, DC_U, DC_U, DC_U, DC_U);
        }
    }
    return rval;
}

uint32 ik_set_chroma_filter(uint32 context_id, const ik_chroma_filter_t *p_chroma_filter)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;
    const amba_img_dsp_debug_print_func_t *p_print_fun = ik_get_debug_print_func();
    if(p_print_fun != NULL) {
        p_print_fun->chroma_filter(context_id, p_chroma_filter);
    }

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((p_chroma_filter != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        rval = p_ctx->organization.filter_methods.set_chroma_filter(context_id, p_chroma_filter);
    } else {
        if(p_chroma_filter == NULL) {
            amba_ik_system_print_str_5("[IK] NULL pointer in %s\n", __func__, DC_S, DC_S, DC_S, DC_S);
            rval = IK_ERR_0005;
        } else if(p_ctx->organization.initial_flag == 0U) {
            amba_ik_system_print_uint32_5("[IK] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
            rval = IK_ERR_0002;
        } else {
            amba_ik_system_print_uint32_5("[IK] can't get context\n", DC_U, DC_U, DC_U, DC_U, DC_U);
        }
    }
    return rval;
}

uint32 ik_get_chroma_filter(uint32 context_id, ik_chroma_filter_t *p_chroma_filter)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((p_chroma_filter != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        rval = p_ctx->organization.filter_methods.get_chroma_filter(context_id, p_chroma_filter);
    } else {
        if(p_chroma_filter == NULL) {
            amba_ik_system_print_str_5("[IK] NULL pointer in %s\n", __func__, DC_S, DC_S, DC_S, DC_S);
            rval = IK_ERR_0005;
        } else if(p_ctx->organization.initial_flag == 0U) {
            amba_ik_system_print_uint32_5("[IK] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
            rval = IK_ERR_0002;
        } else {
            amba_ik_system_print_uint32_5("[IK] can't get context\n", DC_U, DC_U, DC_U, DC_U, DC_U);
        }
    }
    return rval;
}

uint32 ik_set_grgb_mismatch(uint32 context_id, const ik_grgb_mismatch_t *p_grgb_mismatch)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;
    const amba_img_dsp_debug_print_func_t *p_print_fun = ik_get_debug_print_func();
    if(p_print_fun != NULL) {
        p_print_fun->grgb_mismatch(context_id, p_grgb_mismatch);
    }

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((p_grgb_mismatch != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        rval = p_ctx->organization.filter_methods.set_grgb_mismatch(context_id, p_grgb_mismatch);
    } else {
        if(p_grgb_mismatch == NULL) {
            amba_ik_system_print_str_5("[IK] NULL pointer in %s\n", __func__, DC_S, DC_S, DC_S, DC_S);
            rval = IK_ERR_0005;
        } else if(p_ctx->organization.initial_flag == 0U) {
            amba_ik_system_print_uint32_5("[IK] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
            rval = IK_ERR_0002;
        } else {
            amba_ik_system_print_uint32_5("[IK] can't get context\n", DC_U, DC_U, DC_U, DC_U, DC_U);
        }
    }
    return rval;
}

uint32 ik_get_grgb_mismatch(uint32 context_id, ik_grgb_mismatch_t *p_grgb_mismatch)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((p_grgb_mismatch != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        rval = p_ctx->organization.filter_methods.get_grgb_mismatch(context_id, p_grgb_mismatch);
    } else {
        if(p_grgb_mismatch == NULL) {
            amba_ik_system_print_str_5("[IK] NULL pointer in %s\n", __func__, DC_S, DC_S, DC_S, DC_S);
            rval = IK_ERR_0005;
        } else if(p_ctx->organization.initial_flag == 0U) {
            amba_ik_system_print_uint32_5("[IK] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
            rval = IK_ERR_0002;
        } else {
            amba_ik_system_print_uint32_5("[IK] can't get context\n", DC_U, DC_U, DC_U, DC_U, DC_U);
        }
    }
    return rval;
}

uint32 ik_set_video_mctf(uint32 context_id, const ik_video_mctf_t *p_video_mctf)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;
    const amba_img_dsp_debug_print_func_t *p_print_fun = ik_get_debug_print_func();
    if(p_print_fun != NULL) {
        p_print_fun->video_mctf(context_id, p_video_mctf);
    }

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((p_video_mctf != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        rval = p_ctx->organization.filter_methods.set_video_mctf(context_id, p_video_mctf);
    } else {
        if(p_video_mctf == NULL) {
            amba_ik_system_print_str_5("[IK] NULL pointer in %s\n", __func__, DC_S, DC_S, DC_S, DC_S);
            rval = IK_ERR_0005;
        } else if(p_ctx->organization.initial_flag == 0U) {
            amba_ik_system_print_uint32_5("[IK] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
            rval = IK_ERR_0002;
        } else {
            amba_ik_system_print_uint32_5("[IK] can't get context\n", DC_U, DC_U, DC_U, DC_U, DC_U);
        }
    }
    return rval;
}

uint32 ik_get_video_mctf(uint32 context_id, ik_video_mctf_t *p_video_mctf)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((p_video_mctf != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        rval = p_ctx->organization.filter_methods.get_video_mctf(context_id, p_video_mctf);
    } else {
        if(p_video_mctf == NULL) {
            amba_ik_system_print_str_5("[IK] NULL pointer in %s\n", __func__, DC_S, DC_S, DC_S, DC_S);
            rval = IK_ERR_0005;
        } else if(p_ctx->organization.initial_flag == 0U) {
            amba_ik_system_print_uint32_5("[IK] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
            rval = IK_ERR_0002;
        } else {
            amba_ik_system_print_uint32_5("[IK] can't get context\n", DC_U, DC_U, DC_U, DC_U, DC_U);
        }
    }
    return rval;
}

uint32 ik_set_video_mctf_ta(uint32 context_id, const ik_video_mctf_ta_t *p_video_mctf_ta)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;
    const amba_img_dsp_debug_print_func_t *p_print_fun = ik_get_debug_print_func();
    if(p_print_fun != NULL) {
        p_print_fun->video_mctf_ta(context_id, p_video_mctf_ta);
    }

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((p_video_mctf_ta != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        rval = p_ctx->organization.filter_methods.set_video_mctf_ta(context_id, p_video_mctf_ta);
    } else {
        if(p_video_mctf_ta == NULL) {
            amba_ik_system_print_str_5("[IK] NULL pointer in %s\n", __func__, DC_S, DC_S, DC_S, DC_S);
            rval = IK_ERR_0005;
        } else if(p_ctx->organization.initial_flag == 0U) {
            amba_ik_system_print_uint32_5("[IK] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
            rval = IK_ERR_0002;
        } else {
            amba_ik_system_print_uint32_5("[IK] can't get context\n", DC_U, DC_U, DC_U, DC_U, DC_U);
        }
    }
    return rval;
}

uint32 ik_get_video_mctf_ta(uint32 context_id, ik_video_mctf_ta_t *p_video_mctf_ta)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((p_video_mctf_ta != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        rval = p_ctx->organization.filter_methods.get_video_mctf_ta(context_id, p_video_mctf_ta);
    } else {
        if(p_video_mctf_ta == NULL) {
            amba_ik_system_print_str_5("[IK] NULL pointer in %s\n", __func__, DC_S, DC_S, DC_S, DC_S);
            rval = IK_ERR_0005;
        } else if(p_ctx->organization.initial_flag == 0U) {
            amba_ik_system_print_uint32_5("[IK] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
            rval = IK_ERR_0002;
        } else {
            amba_ik_system_print_uint32_5("[IK] can't get context\n", DC_U, DC_U, DC_U, DC_U, DC_U);
        }
    }
    return rval;
}

uint32 ik_set_video_mctf_and_fnl_shp(uint32 context_id, const ik_pos_dep33_t *p_video_mctf_and_final_sharpen)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;
    const amba_img_dsp_debug_print_func_t *p_print_fun = ik_get_debug_print_func();
    if(p_print_fun != NULL) {
        p_print_fun->video_mctf_and_fnlshp(context_id, p_video_mctf_and_final_sharpen);
    }

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((p_video_mctf_and_final_sharpen != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        rval = p_ctx->organization.filter_methods.set_video_mctf_and_final_sharpen(context_id, p_video_mctf_and_final_sharpen);
    } else {
        if(p_video_mctf_and_final_sharpen == NULL) {
            amba_ik_system_print_str_5("[IK] NULL pointer in %s\n", __func__, DC_S, DC_S, DC_S, DC_S);
            rval = IK_ERR_0005;
        } else if(p_ctx->organization.initial_flag == 0U) {
            amba_ik_system_print_uint32_5("[IK] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
            rval = IK_ERR_0002;
        } else {
            amba_ik_system_print_uint32_5("[IK] can't get context\n", DC_U, DC_U, DC_U, DC_U, DC_U);
        }
    }
    return rval;
}

uint32 ik_get_video_mctf_and_fnl_shp(uint32 context_id, ik_pos_dep33_t *p_video_mctf_and_final_sharpen)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((p_video_mctf_and_final_sharpen != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        rval = p_ctx->organization.filter_methods.get_video_mctf_and_final_sharpen(context_id, p_video_mctf_and_final_sharpen);
    } else {
        if(p_video_mctf_and_final_sharpen == NULL) {
            amba_ik_system_print_str_5("[IK] NULL pointer in %s\n", __func__, DC_S, DC_S, DC_S, DC_S);
            rval = IK_ERR_0005;
        } else if(p_ctx->organization.initial_flag == 0U) {
            amba_ik_system_print_uint32_5("[IK] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
            rval = IK_ERR_0002;
        } else {
            amba_ik_system_print_uint32_5("[IK] can't get context\n", DC_U, DC_U, DC_U, DC_U, DC_U);
        }
    }
    return rval;
}

uint32 ik_set_motion_detect(uint32 context_id, const ik_motion_detect_t *p_motion_detect)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;
    const amba_img_dsp_debug_print_func_t *p_print_fun = ik_get_debug_print_func();
    if(p_print_fun != NULL) {
        p_print_fun->motion_detect(context_id, p_motion_detect);
    }

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((p_motion_detect != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        rval = p_ctx->organization.filter_methods.set_motion_detect(context_id, p_motion_detect);
    } else {
        if(p_motion_detect == NULL) {
            amba_ik_system_print_str_5("[IK] NULL pointer in %s\n", __func__, DC_S, DC_S, DC_S, DC_S);
            rval = IK_ERR_0005;
        } else if(p_ctx->organization.initial_flag == 0U) {
            amba_ik_system_print_uint32_5("[IK] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
            rval = IK_ERR_0002;
        } else {
            amba_ik_system_print_uint32_5("[IK] can't get context\n", DC_U, DC_U, DC_U, DC_U, DC_U);
        }
    }
    return rval;
}

uint32 ik_get_motion_detect(uint32 context_id, ik_motion_detect_t *p_motion_detect)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((p_motion_detect != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        rval = p_ctx->organization.filter_methods.get_motion_detect(context_id, p_motion_detect);
    } else {
        if(p_motion_detect == NULL) {
            amba_ik_system_print_str_5("[IK] NULL pointer in %s\n", __func__, DC_S, DC_S, DC_S, DC_S);
            rval = IK_ERR_0005;
        } else if(p_ctx->organization.initial_flag == 0U) {
            amba_ik_system_print_uint32_5("[IK] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
            rval = IK_ERR_0002;
        } else {
            amba_ik_system_print_uint32_5("[IK] can't get context\n", DC_U, DC_U, DC_U, DC_U, DC_U);
        }
    }
    return rval;
}

uint32 ik_set_motion_detect_pos_dep(uint32 context_id, const ik_pos_dep33_t *p_motion_detect_pos_dep)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;
    const amba_img_dsp_debug_print_func_t *p_print_fun = ik_get_debug_print_func();
    if(p_print_fun != NULL) {
        p_print_fun->motion_detect_pos_dep(context_id, p_motion_detect_pos_dep);
    }

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((p_motion_detect_pos_dep != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        rval = p_ctx->organization.filter_methods.set_motion_detect_pos_dep(context_id, p_motion_detect_pos_dep);
    } else {
        if(p_motion_detect_pos_dep == NULL) {
            amba_ik_system_print_str_5("[IK] NULL pointer in %s\n", __func__, DC_S, DC_S, DC_S, DC_S);
            rval = IK_ERR_0005;
        } else if(p_ctx->organization.initial_flag == 0U) {
            amba_ik_system_print_uint32_5("[IK] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
            rval = IK_ERR_0002;
        } else {
            amba_ik_system_print_uint32_5("[IK] can't get context\n", DC_U, DC_U, DC_U, DC_U, DC_U);
        }
    }
    return rval;
}

uint32 ik_get_motion_detect_pos_dep(uint32 context_id, ik_pos_dep33_t *p_motion_detect_pos_dep)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((p_motion_detect_pos_dep != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        rval = p_ctx->organization.filter_methods.get_motion_detect_pos_dep(context_id, p_motion_detect_pos_dep);
    } else {
        if(p_motion_detect_pos_dep == NULL) {
            amba_ik_system_print_str_5("[IK] NULL pointer in %s\n", __func__, DC_S, DC_S, DC_S, DC_S);
            rval = IK_ERR_0005;
        } else if(p_ctx->organization.initial_flag == 0U) {
            amba_ik_system_print_uint32_5("[IK] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
            rval = IK_ERR_0002;
        } else {
            amba_ik_system_print_uint32_5("[IK] can't get context\n", DC_U, DC_U, DC_U, DC_U, DC_U);
        }
    }
    return rval;
}

uint32 ik_set_motion_detect_and_mctf(uint32 context_id, const ik_motion_detect_and_mctf_t *p_motion_detect_and_mctf)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((p_motion_detect_and_mctf != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        rval = p_ctx->organization.filter_methods.set_motion_detect_and_mctf(context_id, p_motion_detect_and_mctf);
    } else {
        if(p_motion_detect_and_mctf == NULL) {
            amba_ik_system_print_str_5("[IK] NULL pointer in %s\n", __func__, DC_S, DC_S, DC_S, DC_S);
            rval = IK_ERR_0005;
        } else if(p_ctx->organization.initial_flag == 0U) {
            amba_ik_system_print_uint32_5("[IK] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
            rval = IK_ERR_0002;
        } else {
            amba_ik_system_print_uint32_5("[IK] can't get context\n", DC_U, DC_U, DC_U, DC_U, DC_U);
        }
    }
    return rval;
}

uint32 ik_get_motion_detect_and_mctf(uint32 context_id, ik_motion_detect_and_mctf_t *p_motion_detect_and_mctf)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((p_motion_detect_and_mctf != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        rval = p_ctx->organization.filter_methods.get_motion_detect_and_mctf(context_id, p_motion_detect_and_mctf);
    } else {
        if(p_motion_detect_and_mctf == NULL) {
            amba_ik_system_print_str_5("[IK] NULL pointer in %s\n", __func__, DC_S, DC_S, DC_S, DC_S);
            rval = IK_ERR_0005;
        } else if(p_ctx->organization.initial_flag == 0U) {
            amba_ik_system_print_uint32_5("[IK] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
            rval = IK_ERR_0002;
        } else {
            amba_ik_system_print_uint32_5("[IK] can't get context\n", DC_U, DC_U, DC_U, DC_U, DC_U);
        }
    }
    return rval;
}

uint32 ik_set_vignette(uint32 context_id, const ik_vignette_t *p_vignette)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;
    const amba_img_dsp_debug_print_func_t *p_print_fun = ik_get_debug_print_func();
    if(p_print_fun != NULL) {
        p_print_fun->vignette_info(context_id, p_vignette);
    }

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((p_vignette != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        rval = p_ctx->organization.filter_methods.set_vignette_compensation(context_id, p_vignette);
    } else {
        if(p_vignette == NULL) {
            amba_ik_system_print_str_5("[IK] NULL pointer in %s\n", __func__, DC_S, DC_S, DC_S, DC_S);
            rval = IK_ERR_0005;
        } else if(p_ctx->organization.initial_flag == 0U) {
            amba_ik_system_print_uint32_5("[IK] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
            rval = IK_ERR_0002;
        } else {
            amba_ik_system_print_uint32_5("[IK] can't get context\n", DC_U, DC_U, DC_U, DC_U, DC_U);
        }
    }
    return rval;
}

uint32 ik_get_vignette(uint32 context_id, ik_vignette_t *p_vignette)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((p_vignette != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        rval = p_ctx->organization.filter_methods.get_vignette_compensation(context_id, p_vignette);
    } else {
        if(p_vignette == NULL) {
            amba_ik_system_print_str_5("[IK] NULL pointer in %s\n", __func__, DC_S, DC_S, DC_S, DC_S);
            rval = IK_ERR_0005;
        } else if(p_ctx->organization.initial_flag == 0U) {
            amba_ik_system_print_uint32_5("[IK] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
            rval = IK_ERR_0002;
        } else {
            amba_ik_system_print_uint32_5("[IK] can't get context\n", DC_U, DC_U, DC_U, DC_U, DC_U);
        }
    }
    return rval;
}

uint32 ik_set_exp0_frontend_static_blc(uint32 context_id, const ik_static_blc_level_t *p_exp0_frontend_static_blc)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;
    const amba_img_dsp_debug_print_func_t *p_print_fun = ik_get_debug_print_func();
    if(p_print_fun != NULL) {
        p_print_fun->fe_blc(context_id, p_exp0_frontend_static_blc, 0U);
    }

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((p_exp0_frontend_static_blc != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        rval = p_ctx->organization.filter_methods.set_exp0_fe_static_blc(context_id, p_exp0_frontend_static_blc);
    } else {
        if(p_exp0_frontend_static_blc == NULL) {
            amba_ik_system_print_str_5("[IK] NULL pointer in %s\n", __func__, DC_S, DC_S, DC_S, DC_S);
            rval = IK_ERR_0005;
        } else if(p_ctx->organization.initial_flag == 0U) {
            amba_ik_system_print_uint32_5("[IK] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
            rval = IK_ERR_0002;
        } else {
            amba_ik_system_print_uint32_5("[IK] can't get context\n", DC_U, DC_U, DC_U, DC_U, DC_U);
        }
    }
    return rval;
}

uint32 ik_get_exp0_frontend_static_blc(uint32 context_id, ik_static_blc_level_t *p_exp0_frontend_static_blc)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((p_exp0_frontend_static_blc != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        rval = p_ctx->organization.filter_methods.get_exp0_fe_static_blc(context_id, p_exp0_frontend_static_blc);
    } else {
        if(p_exp0_frontend_static_blc == NULL) {
            amba_ik_system_print_str_5("[IK] NULL pointer in %s\n", __func__, DC_S, DC_S, DC_S, DC_S);
            rval = IK_ERR_0005;
        } else if(p_ctx->organization.initial_flag == 0U) {
            amba_ik_system_print_uint32_5("[IK] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
            rval = IK_ERR_0002;
        } else {
            amba_ik_system_print_uint32_5("[IK] can't get context\n", DC_U, DC_U, DC_U, DC_U, DC_U);
        }
    }
    return rval;
}

uint32 ik_set_exp1_frontend_static_blc(uint32 context_id, const ik_static_blc_level_t *p_exp1_frontend_static_blc)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;
    const amba_img_dsp_debug_print_func_t *p_print_fun = ik_get_debug_print_func();
    if(p_print_fun != NULL) {
        p_print_fun->fe_blc(context_id, p_exp1_frontend_static_blc, 1U);
    }

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((p_exp1_frontend_static_blc != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        rval = p_ctx->organization.filter_methods.set_exp1_fe_static_blc(context_id, p_exp1_frontend_static_blc);
    } else {
        if(p_exp1_frontend_static_blc == NULL) {
            amba_ik_system_print_str_5("[IK] NULL pointer in %s\n", __func__, DC_S, DC_S, DC_S, DC_S);
            rval = IK_ERR_0005;
        } else if(p_ctx->organization.initial_flag == 0U) {
            amba_ik_system_print_uint32_5("[IK] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
            rval = IK_ERR_0002;
        } else {
            amba_ik_system_print_uint32_5("[IK] can't get context\n", DC_U, DC_U, DC_U, DC_U, DC_U);
        }
    }
    return rval;
}

uint32 ik_get_exp1_frontend_static_blc(uint32 context_id, ik_static_blc_level_t *p_exp1_frontend_static_blc)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((p_exp1_frontend_static_blc != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        rval = p_ctx->organization.filter_methods.get_exp1_fe_static_blc(context_id, p_exp1_frontend_static_blc);
    } else {
        if(p_exp1_frontend_static_blc == NULL) {
            amba_ik_system_print_str_5("[IK] NULL pointer in %s\n", __func__, DC_S, DC_S, DC_S, DC_S);
            rval = IK_ERR_0005;
        } else if(p_ctx->organization.initial_flag == 0U) {
            amba_ik_system_print_uint32_5("[IK] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
            rval = IK_ERR_0002;
        } else {
            amba_ik_system_print_uint32_5("[IK] can't get context\n", DC_U, DC_U, DC_U, DC_U, DC_U);
        }
    }
    return rval;
}

uint32 ik_set_exp2_frontend_static_blc(uint32 context_id, const ik_static_blc_level_t *p_exp2_frontend_static_blc)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;
    const amba_img_dsp_debug_print_func_t *p_print_fun = ik_get_debug_print_func();
    if(p_print_fun != NULL) {
        p_print_fun->fe_blc(context_id, p_exp2_frontend_static_blc, 2U);
    }

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((p_exp2_frontend_static_blc != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        rval = p_ctx->organization.filter_methods.set_exp2_fe_static_blc(context_id, p_exp2_frontend_static_blc);
    } else {
        if(p_exp2_frontend_static_blc == NULL) {
            amba_ik_system_print_str_5("[IK] NULL pointer in %s\n", __func__, DC_S, DC_S, DC_S, DC_S);
            rval = IK_ERR_0005;
        } else if(p_ctx->organization.initial_flag == 0U) {
            amba_ik_system_print_uint32_5("[IK] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
            rval = IK_ERR_0002;
        } else {
            amba_ik_system_print_uint32_5("[IK] can't get context\n", DC_U, DC_U, DC_U, DC_U, DC_U);
        }
    }
    return rval;
}

uint32 ik_get_exp2_frontend_static_blc(uint32 context_id, ik_static_blc_level_t *p_exp2_frontend_static_blc)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((p_exp2_frontend_static_blc != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        rval = p_ctx->organization.filter_methods.get_exp2_fe_static_blc(context_id, p_exp2_frontend_static_blc);
    } else {
        if(p_exp2_frontend_static_blc == NULL) {
            amba_ik_system_print_str_5("[IK] NULL pointer in %s\n", __func__, DC_S, DC_S, DC_S, DC_S);
            rval = IK_ERR_0005;
        } else if(p_ctx->organization.initial_flag == 0U) {
            amba_ik_system_print_uint32_5("[IK] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
            rval = IK_ERR_0002;
        } else {
            amba_ik_system_print_uint32_5("[IK] can't get context\n", DC_U, DC_U, DC_U, DC_U, DC_U);
        }
    }
    return rval;
}

uint32 ik_set_exp0_frontend_wb_gain(uint32 context_id, const ik_frontend_wb_gain_t *p_exp0_frontend_wb_gain)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;
    const amba_img_dsp_debug_print_func_t *p_print_fun = ik_get_debug_print_func();
    if(p_print_fun != NULL) {
        p_print_fun->fe_wb(context_id, p_exp0_frontend_wb_gain, 0);
    }

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((p_exp0_frontend_wb_gain != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        rval = p_ctx->organization.filter_methods.set_exp0_fe_wb_gain(context_id, p_exp0_frontend_wb_gain);
    } else {
        if(p_exp0_frontend_wb_gain == NULL) {
            amba_ik_system_print_str_5("[IK] NULL pointer in %s\n", __func__, DC_S, DC_S, DC_S, DC_S);
            rval = IK_ERR_0005;
        } else if(p_ctx->organization.initial_flag == 0U) {
            amba_ik_system_print_uint32_5("[IK] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
            rval = IK_ERR_0002;
        } else {
            amba_ik_system_print_uint32_5("[IK] can't get context\n", DC_U, DC_U, DC_U, DC_U, DC_U);
        }
    }
    return rval;
}

uint32 ik_get_exp0_frontend_wb_gain(uint32 context_id, ik_frontend_wb_gain_t *p_exp0_frontend_wb_gain)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((p_exp0_frontend_wb_gain != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        rval = p_ctx->organization.filter_methods.get_exp0_fe_wb_gain(context_id, p_exp0_frontend_wb_gain);
    } else {
        if(p_exp0_frontend_wb_gain == NULL) {
            amba_ik_system_print_str_5("[IK] NULL pointer in %s\n", __func__, DC_S, DC_S, DC_S, DC_S);
            rval = IK_ERR_0005;
        } else if(p_ctx->organization.initial_flag == 0U) {
            amba_ik_system_print_uint32_5("[IK] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
            rval = IK_ERR_0002;
        } else {
            amba_ik_system_print_uint32_5("[IK] can't get context\n", DC_U, DC_U, DC_U, DC_U, DC_U);
        }
    }
    return rval;
}

uint32 ik_set_exp1_frontend_wb_gain(uint32 context_id, const ik_frontend_wb_gain_t *p_exp1_frontend_wb_gain)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;
    const amba_img_dsp_debug_print_func_t *p_print_fun = ik_get_debug_print_func();
    if(p_print_fun != NULL) {
        p_print_fun->fe_wb(context_id, p_exp1_frontend_wb_gain, 1);
    }

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((p_exp1_frontend_wb_gain != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        rval = p_ctx->organization.filter_methods.set_exp1_fe_wb_gain(context_id, p_exp1_frontend_wb_gain);
    } else {
        if(p_exp1_frontend_wb_gain == NULL) {
            amba_ik_system_print_str_5("[IK] NULL pointer in %s\n", __func__, DC_S, DC_S, DC_S, DC_S);
            rval = IK_ERR_0005;
        } else if(p_ctx->organization.initial_flag == 0U) {
            amba_ik_system_print_uint32_5("[IK] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
            rval = IK_ERR_0002;
        } else {
            amba_ik_system_print_uint32_5("[IK] can't get context\n", DC_U, DC_U, DC_U, DC_U, DC_U);
        }
    }
    return rval;
}

uint32 ik_get_exp1_frontend_wb_gain(uint32 context_id, ik_frontend_wb_gain_t *p_exp1_frontend_wb_gain)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((p_exp1_frontend_wb_gain != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        rval = p_ctx->organization.filter_methods.get_exp1_fe_wb_gain(context_id, p_exp1_frontend_wb_gain);
    } else {
        if(p_exp1_frontend_wb_gain == NULL) {
            amba_ik_system_print_str_5("[IK] NULL pointer in %s\n", __func__, DC_S, DC_S, DC_S, DC_S);
            rval = IK_ERR_0005;
        } else if(p_ctx->organization.initial_flag == 0U) {
            amba_ik_system_print_uint32_5("[IK] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
            rval = IK_ERR_0002;
        } else {
            amba_ik_system_print_uint32_5("[IK] can't get context\n", DC_U, DC_U, DC_U, DC_U, DC_U);
        }
    }
    return rval;
}

uint32 ik_set_exp2_frontend_wb_gain(uint32 context_id, const ik_frontend_wb_gain_t *p_exp2_frontend_wb_gain)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;
    const amba_img_dsp_debug_print_func_t *p_print_fun = ik_get_debug_print_func();
    if(p_print_fun != NULL) {
        p_print_fun->fe_wb(context_id, p_exp2_frontend_wb_gain, 2);
    }

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((p_exp2_frontend_wb_gain != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        rval = p_ctx->organization.filter_methods.set_exp2_fe_wb_gain(context_id, p_exp2_frontend_wb_gain);
    } else {
        if(p_exp2_frontend_wb_gain == NULL) {
            amba_ik_system_print_str_5("[IK] NULL pointer in %s\n", __func__, DC_S, DC_S, DC_S, DC_S);
            rval = IK_ERR_0005;
        } else if(p_ctx->organization.initial_flag == 0U) {
            amba_ik_system_print_uint32_5("[IK] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
            rval = IK_ERR_0002;
        } else {
            amba_ik_system_print_uint32_5("[IK] can't get context\n", DC_U, DC_U, DC_U, DC_U, DC_U);
        }
    }
    return rval;
}

uint32 ik_get_exp2_frontend_wb_gain(uint32 context_id, ik_frontend_wb_gain_t *p_exp2_frontend_wb_gain)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((p_exp2_frontend_wb_gain != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        rval = p_ctx->organization.filter_methods.get_exp2_fe_wb_gain(context_id, p_exp2_frontend_wb_gain);
    } else {
        if(p_exp2_frontend_wb_gain == NULL) {
            amba_ik_system_print_str_5("[IK] NULL pointer in %s\n", __func__, DC_S, DC_S, DC_S, DC_S);
            rval = IK_ERR_0005;
        } else if(p_ctx->organization.initial_flag == 0U) {
            amba_ik_system_print_uint32_5("[IK] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
            rval = IK_ERR_0002;
        } else {
            amba_ik_system_print_uint32_5("[IK] can't get context\n", DC_U, DC_U, DC_U, DC_U, DC_U);
        }
    }
    return rval;
}

uint32 ik_set_exp0_frontend_dgain_sat_lvl(uint32 context_id, const ik_frontend_dgain_saturation_level_t *p_exp0_frontend_dgain_sat_lvl)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;
    const amba_img_dsp_debug_print_func_t *p_print_fun = ik_get_debug_print_func();
    if(p_print_fun != NULL) {
        p_print_fun->fe_dgain_saturation_level(context_id, p_exp0_frontend_dgain_sat_lvl);
    }

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((p_exp0_frontend_dgain_sat_lvl != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        rval = p_ctx->organization.filter_methods.set_exp0_fe_dgain_sat_lvl(context_id, p_exp0_frontend_dgain_sat_lvl);
    } else {
        if(p_exp0_frontend_dgain_sat_lvl == NULL) {
            amba_ik_system_print_str_5("[IK] NULL pointer in %s\n", __func__, DC_S, DC_S, DC_S, DC_S);
            rval = IK_ERR_0005;
        } else if(p_ctx->organization.initial_flag == 0U) {
            amba_ik_system_print_uint32_5("[IK] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
            rval = IK_ERR_0002;
        } else {
            amba_ik_system_print_uint32_5("[IK] can't get context\n", DC_U, DC_U, DC_U, DC_U, DC_U);
        }
    }
    return rval;
}

uint32 ik_get_exp0_frontend_dgain_sat_lvl(uint32 context_id, ik_frontend_dgain_saturation_level_t *p_exp0_frontend_dgain_sat_lvl)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((p_exp0_frontend_dgain_sat_lvl != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        rval = p_ctx->organization.filter_methods.get_exp0_fe_dgain_sat_lvl(context_id, p_exp0_frontend_dgain_sat_lvl);
    } else {
        if(p_exp0_frontend_dgain_sat_lvl == NULL) {
            amba_ik_system_print_str_5("[IK] NULL pointer in %s\n", __func__, DC_S, DC_S, DC_S, DC_S);
            rval = IK_ERR_0005;
        } else if(p_ctx->organization.initial_flag == 0U) {
            amba_ik_system_print_uint32_5("[IK] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
            rval = IK_ERR_0002;
        } else {
            amba_ik_system_print_uint32_5("[IK] can't get context\n", DC_U, DC_U, DC_U, DC_U, DC_U);
        }
    }
    return rval;
}

uint32 ik_set_exp1_frontend_dgain_sat_lvl(uint32 context_id, const ik_frontend_dgain_saturation_level_t *p_exp1_frontend_dgain_sat_lvl)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;
    const amba_img_dsp_debug_print_func_t *p_print_fun = ik_get_debug_print_func();
    if(p_print_fun != NULL) {
        p_print_fun->fe_dgain_saturation_level(context_id, p_exp1_frontend_dgain_sat_lvl);
    }

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((p_exp1_frontend_dgain_sat_lvl != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        rval = p_ctx->organization.filter_methods.set_exp1_fe_dgain_sat_lvl(context_id, p_exp1_frontend_dgain_sat_lvl);
    } else {
        if(p_exp1_frontend_dgain_sat_lvl == NULL) {
            amba_ik_system_print_str_5("[IK] NULL pointer in %s\n", __func__, DC_S, DC_S, DC_S, DC_S);
            rval = IK_ERR_0005;
        } else if(p_ctx->organization.initial_flag == 0U) {
            amba_ik_system_print_uint32_5("[IK] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
            rval = IK_ERR_0002;
        } else {
            amba_ik_system_print_uint32_5("[IK] can't get context\n", DC_U, DC_U, DC_U, DC_U, DC_U);
        }
    }
    return rval;
}

uint32 ik_get_exp1_frontend_dgain_sat_lvl(uint32 context_id, ik_frontend_dgain_saturation_level_t *p_exp1_frontend_dgain_sat_lvl)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((p_exp1_frontend_dgain_sat_lvl != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        rval = p_ctx->organization.filter_methods.get_exp1_fe_dgain_sat_lvl(context_id, p_exp1_frontend_dgain_sat_lvl);
    } else {
        if(p_exp1_frontend_dgain_sat_lvl == NULL) {
            amba_ik_system_print_str_5("[IK] NULL pointer in %s\n", __func__, DC_S, DC_S, DC_S, DC_S);
            rval = IK_ERR_0005;
        } else if(p_ctx->organization.initial_flag == 0U) {
            amba_ik_system_print_uint32_5("[IK] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
            rval = IK_ERR_0002;
        } else {
            amba_ik_system_print_uint32_5("[IK] can't get context\n", DC_U, DC_U, DC_U, DC_U, DC_U);
        }
    }
    return rval;
}

uint32 ik_set_exp2_frontend_dgain_sat_lvl(uint32 context_id, const ik_frontend_dgain_saturation_level_t *p_exp2_frontend_dgain_sat_lvl)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;
    const amba_img_dsp_debug_print_func_t *p_print_fun = ik_get_debug_print_func();
    if(p_print_fun != NULL) {
        p_print_fun->fe_dgain_saturation_level(context_id, p_exp2_frontend_dgain_sat_lvl);
    }

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((p_exp2_frontend_dgain_sat_lvl != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        rval = p_ctx->organization.filter_methods.set_exp2_fe_dgain_sat_lvl(context_id, p_exp2_frontend_dgain_sat_lvl);
    } else {
        if(p_exp2_frontend_dgain_sat_lvl == NULL) {
            amba_ik_system_print_str_5("[IK] NULL pointer in %s\n", __func__, DC_S, DC_S, DC_S, DC_S);
            rval = IK_ERR_0005;
        } else if(p_ctx->organization.initial_flag == 0U) {
            amba_ik_system_print_uint32_5("[IK] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
            rval = IK_ERR_0002;
        } else {
            amba_ik_system_print_uint32_5("[IK] can't get context\n", DC_U, DC_U, DC_U, DC_U, DC_U);
        }
    }
    return rval;
}

uint32 ik_get_exp2_frontend_dgain_sat_lvl(uint32 context_id, ik_frontend_dgain_saturation_level_t *p_exp2_frontend_dgain_sat_lvl)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((p_exp2_frontend_dgain_sat_lvl != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        rval = p_ctx->organization.filter_methods.get_exp2_fe_dgain_sat_lvl(context_id, p_exp2_frontend_dgain_sat_lvl);
    } else {
        if(p_exp2_frontend_dgain_sat_lvl == NULL) {
            amba_ik_system_print_str_5("[IK] NULL pointer in %s\n", __func__, DC_S, DC_S, DC_S, DC_S);
            rval = IK_ERR_0005;
        } else if(p_ctx->organization.initial_flag == 0U) {
            amba_ik_system_print_uint32_5("[IK] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
            rval = IK_ERR_0002;
        } else {
            amba_ik_system_print_uint32_5("[IK] can't get context\n", DC_U, DC_U, DC_U, DC_U, DC_U);
        }
    }
    return rval;
}

uint32 ik_set_ce(uint32 context_id, const ik_ce_t *p_ce)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;
    const amba_img_dsp_debug_print_func_t *p_print_fun = ik_get_debug_print_func();
    if(p_print_fun != NULL) {
        p_print_fun->ce(context_id, p_ce);
    }

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((p_ce != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        rval = p_ctx->organization.filter_methods.set_ce(context_id, p_ce);
    } else {
        if(p_ce == NULL) {
            amba_ik_system_print_str_5("[IK] NULL pointer in %s\n", __func__, DC_S, DC_S, DC_S, DC_S);
            rval = IK_ERR_0005;
        } else if(p_ctx->organization.initial_flag == 0U) {
            amba_ik_system_print_uint32_5("[IK] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
            rval = IK_ERR_0002;
        } else {
            amba_ik_system_print_uint32_5("[IK] can't get context\n", DC_U, DC_U, DC_U, DC_U, DC_U);
        }
    }
    return rval;
}

uint32 ik_get_ce(uint32 context_id, ik_ce_t *p_ce)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((p_ce != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        rval = p_ctx->organization.filter_methods.get_ce(context_id, p_ce);
    } else {
        if(p_ce == NULL) {
            amba_ik_system_print_str_5("[IK] NULL pointer in %s\n", __func__, DC_S, DC_S, DC_S, DC_S);
            rval = IK_ERR_0005;
        } else if(p_ctx->organization.initial_flag == 0U) {
            amba_ik_system_print_uint32_5("[IK] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
            rval = IK_ERR_0002;
        } else {
            amba_ik_system_print_uint32_5("[IK] can't get context\n", DC_U, DC_U, DC_U, DC_U, DC_U);
        }
    }
    return rval;
}

uint32 ik_set_ce_input_table(uint32 context_id, const ik_ce_input_table_t *p_ce_input_table)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;
    const amba_img_dsp_debug_print_func_t *p_print_fun = ik_get_debug_print_func();
    if(p_print_fun != NULL) {
        p_print_fun->ce_input_tbl(context_id, p_ce_input_table);
    }

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((p_ce_input_table != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        rval = p_ctx->organization.filter_methods.set_ce_input_table(context_id, p_ce_input_table);
    } else {
        if(p_ce_input_table == NULL) {
            amba_ik_system_print_str_5("[IK] NULL pointer in %s\n", __func__, DC_S, DC_S, DC_S, DC_S);
            rval = IK_ERR_0005;
        } else if(p_ctx->organization.initial_flag == 0U) {
            amba_ik_system_print_uint32_5("[IK] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
            rval = IK_ERR_0002;
        } else {
            amba_ik_system_print_uint32_5("[IK] can't get context\n", DC_U, DC_U, DC_U, DC_U, DC_U);
        }
    }
    return rval;
}

uint32 ik_get_ce_input_table(uint32 context_id, ik_ce_input_table_t *p_ce_input_table)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((p_ce_input_table != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        rval = p_ctx->organization.filter_methods.get_ce_input_table(context_id, p_ce_input_table);
    } else {
        if(p_ce_input_table == NULL) {
            amba_ik_system_print_str_5("[IK] NULL pointer in %s\n", __func__, DC_S, DC_S, DC_S, DC_S);
            rval = IK_ERR_0005;
        } else if(p_ctx->organization.initial_flag == 0U) {
            amba_ik_system_print_uint32_5("[IK] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
            rval = IK_ERR_0002;
        } else {
            amba_ik_system_print_uint32_5("[IK] can't get context\n", DC_U, DC_U, DC_U, DC_U, DC_U);
        }
    }
    return rval;
}


uint32 ik_set_ce_out_table(uint32 context_id, const ik_ce_output_table_t *p_ce_out_table)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;
    const amba_img_dsp_debug_print_func_t *p_print_fun = ik_get_debug_print_func();
    if(p_print_fun != NULL) {
        p_print_fun->ce_output_tbl(context_id, p_ce_out_table);
    }

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((p_ce_out_table != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        rval = p_ctx->organization.filter_methods.set_ce_out_table(context_id, p_ce_out_table);
    } else {
        if(p_ce_out_table == NULL) {
            amba_ik_system_print_str_5("[IK] NULL pointer in %s\n", __func__, DC_S, DC_S, DC_S, DC_S);
            rval = IK_ERR_0005;
        } else if(p_ctx->organization.initial_flag == 0U) {
            amba_ik_system_print_uint32_5("[IK] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
            rval = IK_ERR_0002;
        } else {
            amba_ik_system_print_uint32_5("[IK] can't get context\n", DC_U, DC_U, DC_U, DC_U, DC_U);
        }
    }
    return rval;
}

uint32 ik_get_ce_out_table(uint32 context_id, ik_ce_output_table_t *p_ce_out_table)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((p_ce_out_table != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        rval = p_ctx->organization.filter_methods.get_ce_out_table(context_id, p_ce_out_table);
    } else {
        if(p_ce_out_table == NULL) {
            amba_ik_system_print_str_5("[IK] NULL pointer in %s\n", __func__, DC_S, DC_S, DC_S, DC_S);
            rval = IK_ERR_0005;
        } else if(p_ctx->organization.initial_flag == 0U) {
            amba_ik_system_print_uint32_5("[IK] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
            rval = IK_ERR_0002;
        } else {
            amba_ik_system_print_uint32_5("[IK] can't get context\n", DC_U, DC_U, DC_U, DC_U, DC_U);
        }
    }
    return rval;
}

uint32 ik_set_ce_ext_hds(uint32 context_id, const ik_ce_external_hds_t *p_ce_ext_hds)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        rval = p_ctx->organization.filter_methods.set_ce_ext_hds(context_id, p_ce_ext_hds);
    } else {
        if(p_ctx->organization.initial_flag == 0U) {
            amba_ik_system_print_uint32_5("[IK] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
            rval = IK_ERR_0002;
        } else {
            amba_ik_system_print_uint32_5("[IK] can't get context\n", DC_U, DC_U, DC_U, DC_U, DC_U);
        }
    }
    return rval;
}

uint32 ik_get_ce_ext_hds(uint32 context_id, ik_ce_external_hds_t *p_ce_ext_hds)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((p_ce_ext_hds != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        rval = p_ctx->organization.filter_methods.get_ce_ext_hds(context_id, p_ce_ext_hds);
    } else {
        if(p_ce_ext_hds == NULL) {
            amba_ik_system_print_str_5("[IK] NULL pointer in %s\n", __func__, DC_S, DC_S, DC_S, DC_S);
            rval = IK_ERR_0005;
        } else if(p_ctx->organization.initial_flag == 0U) {
            amba_ik_system_print_uint32_5("[IK] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
            rval = IK_ERR_0002;
        } else {
            amba_ik_system_print_uint32_5("[IK] can't get context\n", DC_U, DC_U, DC_U, DC_U, DC_U);
        }
    }
    return rval;
}

uint32 ik_set_hdr_blend(uint32 context_id, const ik_hdr_blend_t *p_hdr_blend)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;
    const amba_img_dsp_debug_print_func_t *p_print_fun = ik_get_debug_print_func();
    if(p_print_fun != NULL) {
        p_print_fun->hdr_blend(context_id, p_hdr_blend);
    }

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((p_hdr_blend != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        rval = p_ctx->organization.filter_methods.set_hdr_blend(context_id, p_hdr_blend);
    } else {
        if(p_hdr_blend == NULL) {
            amba_ik_system_print_str_5("[IK] NULL pointer in %s\n", __func__, DC_S, DC_S, DC_S, DC_S);
            rval = IK_ERR_0005;
        } else if(p_ctx->organization.initial_flag == 0U) {
            amba_ik_system_print_uint32_5("[IK] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
            rval = IK_ERR_0002;
        } else {
            amba_ik_system_print_uint32_5("[IK] can't get context\n", DC_U, DC_U, DC_U, DC_U, DC_U);
        }
    }
    return rval;
}

uint32 ik_get_hdr_blend(uint32 context_id, ik_hdr_blend_t *p_hdr_blend)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((p_hdr_blend != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        rval = p_ctx->organization.filter_methods.get_hdr_blend(context_id, p_hdr_blend);
    } else {
        if(p_hdr_blend == NULL) {
            amba_ik_system_print_str_5("[IK] NULL pointer in %s\n", __func__, DC_S, DC_S, DC_S, DC_S);
            rval = IK_ERR_0005;
        } else if(p_ctx->organization.initial_flag == 0U) {
            amba_ik_system_print_uint32_5("[IK] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
            rval = IK_ERR_0002;
        } else {
            amba_ik_system_print_uint32_5("[IK] can't get context\n", DC_U, DC_U, DC_U, DC_U, DC_U);
        }
    }
    return rval;
}

uint32 ik_set_frontend_tone_curve(uint32 context_id, const ik_frontend_tone_curve_t *p_fe_tc)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;
    const amba_img_dsp_debug_print_func_t *p_print_fun = ik_get_debug_print_func();
    if(p_print_fun != NULL) {
        p_print_fun->fe_tone_curve(context_id, p_fe_tc);
    }

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((p_fe_tc != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        rval = p_ctx->organization.filter_methods.set_front_end_tone_curve(context_id, p_fe_tc);
    } else {
        if(p_fe_tc == NULL) {
            amba_ik_system_print_str_5("[IK] NULL pointer in %s\n", __func__, DC_S, DC_S, DC_S, DC_S);
            rval = IK_ERR_0005;
        } else if(p_ctx->organization.initial_flag == 0U) {
            amba_ik_system_print_uint32_5("[IK] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
            rval = IK_ERR_0002;
        } else {
            amba_ik_system_print_uint32_5("[IK] can't get context\n", DC_U, DC_U, DC_U, DC_U, DC_U);
        }
    }
    return rval;
}

uint32 ik_get_frontend_tone_curve(uint32 context_id, ik_frontend_tone_curve_t *p_fe_tc)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((p_fe_tc != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        rval = p_ctx->organization.filter_methods.get_front_end_tone_curve(context_id, p_fe_tc);
    } else {
        if(p_fe_tc == NULL) {
            amba_ik_system_print_str_5("[IK] NULL pointer in %s\n", __func__, DC_S, DC_S, DC_S, DC_S);
            rval = IK_ERR_0005;
        } else if(p_ctx->organization.initial_flag == 0U) {
            amba_ik_system_print_uint32_5("[IK] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
            rval = IK_ERR_0002;
        } else {
            amba_ik_system_print_uint32_5("[IK] can't get context\n", DC_U, DC_U, DC_U, DC_U, DC_U);
        }
    }
    return rval;
}

uint32 ik_set_resampler_strength(uint32 context_id, const ik_resampler_strength_t *p_resampler_strength)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;
    const amba_img_dsp_debug_print_func_t *p_print_fun = ik_get_debug_print_func();
    if(p_print_fun != NULL) {
        p_print_fun->resamp_strength(context_id, p_resampler_strength);
    }

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((p_resampler_strength != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        rval = p_ctx->organization.filter_methods.set_resampler_str(context_id, p_resampler_strength);
    } else {
        if(p_resampler_strength == NULL) {
            amba_ik_system_print_str_5("[IK] NULL pointer in %s\n", __func__, DC_S, DC_S, DC_S, DC_S);
            rval = IK_ERR_0005;
        } else if(p_ctx->organization.initial_flag == 0U) {
            amba_ik_system_print_uint32_5("[IK] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
            rval = IK_ERR_0002;
        } else {
            amba_ik_system_print_uint32_5("[IK] can't get context\n", DC_U, DC_U, DC_U, DC_U, DC_U);
        }
    }
    return rval;
}

uint32 ik_get_resampler_strength(uint32 context_id, ik_resampler_strength_t *p_resampler_strength)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((p_resampler_strength != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        rval = p_ctx->organization.filter_methods.get_resampler_str(context_id, p_resampler_strength);
    } else {
        if(p_resampler_strength == NULL) {
            amba_ik_system_print_str_5("[IK] NULL pointer in %s\n", __func__, DC_S, DC_S, DC_S, DC_S);
            rval = IK_ERR_0005;
        } else if(p_ctx->organization.initial_flag == 0U) {
            amba_ik_system_print_uint32_5("[IK] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
            rval = IK_ERR_0002;
        } else {
            amba_ik_system_print_uint32_5("[IK] can't get context\n", DC_U, DC_U, DC_U, DC_U, DC_U);
        }
    }
    return rval;
}

uint32 ik_set_aaa_stat_info(uint32 context_id, const ik_aaa_stat_info_t *p_stat_info)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((p_stat_info != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        rval = p_ctx->organization.filter_methods.set_aaa_stat_info(context_id, p_stat_info);
    } else {
        if(p_stat_info == NULL) {
            amba_ik_system_print_str_5("[IK] NULL pointer in %s\n", __func__, DC_S, DC_S, DC_S, DC_S);
            rval = IK_ERR_0005;
        } else if(p_ctx->organization.initial_flag == 0U) {
            amba_ik_system_print_uint32_5("[IK] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
            rval = IK_ERR_0002;
        } else {
            amba_ik_system_print_uint32_5("[IK] can't get context\n", DC_U, DC_U, DC_U, DC_U, DC_U);
        }
    }
    return rval;
}

uint32 ik_get_aaa_stat_info(uint32 context_id, ik_aaa_stat_info_t *p_stat_info)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((p_stat_info != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        rval = p_ctx->organization.filter_methods.get_aaa_stat_info(context_id, p_stat_info);
    } else {
        if(p_stat_info == NULL) {
            amba_ik_system_print_str_5("[IK] NULL pointer in %s\n", __func__, DC_S, DC_S, DC_S, DC_S);
            rval = IK_ERR_0005;
        } else if(p_ctx->organization.initial_flag == 0U) {
            amba_ik_system_print_uint32_5("[IK] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
            rval = IK_ERR_0002;
        } else {
            amba_ik_system_print_uint32_5("[IK] can't get context\n", DC_U, DC_U, DC_U, DC_U, DC_U);
        }
    }
    return rval;
}

uint32 ik_set_af_stat_ex_info(uint32 context_id, const ik_af_stat_ex_info_t *p_af_stat_ex_info)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((p_af_stat_ex_info != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        rval = p_ctx->organization.filter_methods.set_af_stat_ex_info(context_id, p_af_stat_ex_info);
    } else {
        if(p_af_stat_ex_info == NULL) {
            amba_ik_system_print_str_5("[IK] NULL pointer in %s\n", __func__, DC_S, DC_S, DC_S, DC_S);
            rval = IK_ERR_0005;
        } else if(p_ctx->organization.initial_flag == 0U) {
            amba_ik_system_print_uint32_5("[IK] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
            rval = IK_ERR_0002;
        } else {
            amba_ik_system_print_uint32_5("[IK] can't get context\n", DC_U, DC_U, DC_U, DC_U, DC_U);
        }
    }
    return rval;
}

uint32 ik_get_af_stat_ex_info(uint32 context_id, ik_af_stat_ex_info_t *p_af_stat_ex_info)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((p_af_stat_ex_info != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        rval = p_ctx->organization.filter_methods.get_af_stat_ex_info(context_id, p_af_stat_ex_info);
    } else {
        if(p_af_stat_ex_info == NULL) {
            amba_ik_system_print_str_5("[IK] NULL pointer in %s\n", __func__, DC_S, DC_S, DC_S, DC_S);
            rval = IK_ERR_0005;
        } else if(p_ctx->organization.initial_flag == 0U) {
            amba_ik_system_print_uint32_5("[IK] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
            rval = IK_ERR_0002;
        } else {
            amba_ik_system_print_uint32_5("[IK] can't get context\n", DC_U, DC_U, DC_U, DC_U, DC_U);
        }
    }
    return rval;
}

uint32 ik_set_pg_af_stat_ex_info(uint32 context_id, const ik_pg_af_stat_ex_info_t *p_pg_af_stat_ex_info)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((p_pg_af_stat_ex_info != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        rval = p_ctx->organization.filter_methods.set_pg_af_stat_ex_info(context_id, p_pg_af_stat_ex_info);
    } else {
        if(p_pg_af_stat_ex_info == NULL) {
            amba_ik_system_print_str_5("[IK] NULL pointer in %s\n", __func__, DC_S, DC_S, DC_S, DC_S);
            rval = IK_ERR_0005;
        } else if(p_ctx->organization.initial_flag == 0U) {
            amba_ik_system_print_uint32_5("[IK] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
            rval = IK_ERR_0002;
        } else {
            amba_ik_system_print_uint32_5("[IK] can't get context\n", DC_U, DC_U, DC_U, DC_U, DC_U);
        }
    }
    return rval;
}

uint32 ik_get_pg_af_stat_ex_info(uint32 context_id, ik_pg_af_stat_ex_info_t *p_pg_af_stat_ex_info)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((p_pg_af_stat_ex_info != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        rval = p_ctx->organization.filter_methods.get_pg_af_stat_ex_info(context_id, p_pg_af_stat_ex_info);
    } else {
        if(p_pg_af_stat_ex_info == NULL) {
            amba_ik_system_print_str_5("[IK] NULL pointer in %s\n", __func__, DC_S, DC_S, DC_S, DC_S);
            rval = IK_ERR_0005;
        } else if(p_ctx->organization.initial_flag == 0U) {
            amba_ik_system_print_uint32_5("[IK] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
            rval = IK_ERR_0002;
        } else {
            amba_ik_system_print_uint32_5("[IK] can't get context\n", DC_U, DC_U, DC_U, DC_U, DC_U);
        }
    }
    return rval;
}

uint32 ik_set_histogram_info(uint32 context_id, const ik_histogram_info_t *p_hist_info)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((p_hist_info != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        rval = p_ctx->organization.filter_methods.set_histogram_info(context_id, p_hist_info);
    } else {
        if(p_hist_info == NULL) {
            amba_ik_system_print_str_5("[IK] NULL pointer in %s\n", __func__, DC_S, DC_S, DC_S, DC_S);
            rval = IK_ERR_0005;
        } else if(p_ctx->organization.initial_flag == 0U) {
            amba_ik_system_print_uint32_5("[IK] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
            rval = IK_ERR_0002;
        } else {
            amba_ik_system_print_uint32_5("[IK] can't get context\n", DC_U, DC_U, DC_U, DC_U, DC_U);
        }
    }
    return rval;
}

uint32 ik_get_histogram_info(uint32 context_id, ik_histogram_info_t *p_hist_info)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((p_hist_info != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        rval = p_ctx->organization.filter_methods.get_histogram_info(context_id, p_hist_info);
    } else {
        if(p_hist_info == NULL) {
            amba_ik_system_print_str_5("[IK] NULL pointer in %s\n", __func__, DC_S, DC_S, DC_S, DC_S);
            rval = IK_ERR_0005;
        } else if(p_ctx->organization.initial_flag == 0U) {
            amba_ik_system_print_uint32_5("[IK] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
            rval = IK_ERR_0002;
        } else {
            amba_ik_system_print_uint32_5("[IK] can't get context\n", DC_U, DC_U, DC_U, DC_U, DC_U);
        }
    }
    return rval;
}

uint32 ik_set_pseudo_y_info(uint32 context_id, const ik_aaa_pseudo_y_info_t *p_pseudo_y_info)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((p_pseudo_y_info != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        rval = p_ctx->organization.filter_methods.set_pseudo_y_info(context_id, p_pseudo_y_info);
    } else {
        if(p_pseudo_y_info == NULL) {
            amba_ik_system_print_str_5("[IK] NULL pointer in %s\n", __func__, DC_S, DC_S, DC_S, DC_S);
            rval = IK_ERR_0005;
        } else if(p_ctx->organization.initial_flag == 0U) {
            amba_ik_system_print_uint32_5("[IK] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
            rval = IK_ERR_0002;
        } else {
            amba_ik_system_print_uint32_5("[IK] can't get context\n", DC_U, DC_U, DC_U, DC_U, DC_U);
        }
    }
    return rval;
}

uint32 ik_get_pseudo_y_info(uint32 context_id, ik_aaa_pseudo_y_info_t *p_pseudo_y_info)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((p_pseudo_y_info != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        rval = p_ctx->organization.filter_methods.get_pseudo_y_info(context_id, p_pseudo_y_info);
    } else {
        if(p_pseudo_y_info == NULL) {
            amba_ik_system_print_str_5("[IK] NULL pointer in %s\n", __func__, DC_S, DC_S, DC_S, DC_S);
            rval = IK_ERR_0005;
        } else if(p_ctx->organization.initial_flag == 0U) {
            amba_ik_system_print_uint32_5("[IK] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
            rval = IK_ERR_0002;
        } else {
            amba_ik_system_print_uint32_5("[IK] can't get context\n", DC_U, DC_U, DC_U, DC_U, DC_U);
        }
    }
    return rval;
}

uint32 ik_set_pg_histogram_info(uint32 context_id, const ik_histogram_info_t *p_hist_info)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((p_hist_info != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        rval = p_ctx->organization.filter_methods.set_pg_histogram_info(context_id, p_hist_info);
    } else {
        if(p_hist_info == NULL) {
            amba_ik_system_print_str_5("[IK] NULL pointer in %s\n", __func__, DC_S, DC_S, DC_S, DC_S);
            rval = IK_ERR_0005;
        } else if(p_ctx->organization.initial_flag == 0U) {
            amba_ik_system_print_uint32_5("[IK] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
            rval = IK_ERR_0002;
        } else {
            amba_ik_system_print_uint32_5("[IK] can't get context\n", DC_U, DC_U, DC_U, DC_U, DC_U);
        }
    }
    return rval;
}

uint32 ik_get_pg_histogram_info(uint32 context_id, ik_histogram_info_t *p_hist_info)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((p_hist_info != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        rval = p_ctx->organization.filter_methods.get_pg_histogram_info(context_id, p_hist_info);
    } else {
        if(p_hist_info == NULL) {
            amba_ik_system_print_str_5("[IK] NULL pointer in %s\n", __func__, DC_S, DC_S, DC_S, DC_S);
            rval = IK_ERR_0005;
        } else if(p_ctx->organization.initial_flag == 0U) {
            amba_ik_system_print_uint32_5("[IK] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
            rval = IK_ERR_0002;
        } else {
            amba_ik_system_print_uint32_5("[IK] can't get context\n", DC_U, DC_U, DC_U, DC_U, DC_U);
        }
    }
    return rval;
}

uint32 ik_set_window_size_info(uint32 context_id, const ik_window_size_info_t *p_window_size_info)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;
    const amba_img_dsp_debug_print_func_t *p_print_fun = ik_get_debug_print_func();
    if(p_print_fun != NULL) {
        p_print_fun->window_size_info(context_id, p_window_size_info);
    }

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((p_window_size_info != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        rval = p_ctx->organization.filter_methods.set_window_size_info(context_id, p_window_size_info);
    } else {
        if(p_window_size_info == NULL) {
            amba_ik_system_print_str_5("[IK] NULL pointer in %s\n", __func__, DC_S, DC_S, DC_S, DC_S);
            rval = IK_ERR_0005;
        } else if(p_ctx->organization.initial_flag == 0U) {
            amba_ik_system_print_uint32_5("[IK] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
            rval = IK_ERR_0002;
        } else {
            amba_ik_system_print_uint32_5("[IK] can't get context\n", DC_U, DC_U, DC_U, DC_U, DC_U);
        }
    }
    return rval;
}

uint32 ik_get_window_size_info(uint32 context_id, ik_window_size_info_t *p_window_size_info)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((p_window_size_info != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        rval = p_ctx->organization.filter_methods.get_window_size_info(context_id, p_window_size_info);
    } else {
        if(p_window_size_info == NULL) {
            amba_ik_system_print_str_5("[IK] NULL pointer in %s\n", __func__, DC_S, DC_S, DC_S, DC_S);
            rval = IK_ERR_0005;
        } else if(p_ctx->organization.initial_flag == 0U) {
            amba_ik_system_print_uint32_5("[IK] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
            rval = IK_ERR_0002;
        } else {
            amba_ik_system_print_uint32_5("[IK] can't get context\n", DC_U, DC_U, DC_U, DC_U, DC_U);
        }
    }
    return rval;
}

uint32 ik_set_cfa_window_size_info(uint32 context_id, const ik_cfa_window_size_info_t *p_cfa_window_size_info)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;
    const amba_img_dsp_debug_print_func_t *p_print_fun = ik_get_debug_print_func();
    if(p_print_fun != NULL) {
        p_print_fun->cfa_window_size_info(context_id, p_cfa_window_size_info);
    }

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((p_cfa_window_size_info != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        rval = p_ctx->organization.filter_methods.set_cfa_window_size_info(context_id, p_cfa_window_size_info);
    } else {
        if(p_cfa_window_size_info == NULL) {
            amba_ik_system_print_str_5("[IK] NULL pointer in %s\n", __func__, DC_S, DC_S, DC_S, DC_S);
            rval = IK_ERR_0005;
        } else if(p_ctx->organization.initial_flag == 0U) {
            amba_ik_system_print_uint32_5("[IK] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
            rval = IK_ERR_0002;
        } else {
            amba_ik_system_print_uint32_5("[IK] can't get context\n", DC_U, DC_U, DC_U, DC_U, DC_U);
        }
    }
    return rval;
}

uint32 ik_get_cfa_window_size_info(uint32 context_id, ik_cfa_window_size_info_t *p_cfa_window_size_info)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((p_cfa_window_size_info != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        rval = p_ctx->organization.filter_methods.get_cfa_window_size_info(context_id, p_cfa_window_size_info);
    } else {
        if(p_cfa_window_size_info == NULL) {
            amba_ik_system_print_str_5("[IK] NULL pointer in %s\n", __func__, DC_S, DC_S, DC_S, DC_S);
            rval = IK_ERR_0005;
        } else if(p_ctx->organization.initial_flag == 0U) {
            amba_ik_system_print_uint32_5("[IK] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
            rval = IK_ERR_0002;
        } else {
            amba_ik_system_print_uint32_5("[IK] can't get context\n", DC_U, DC_U, DC_U, DC_U, DC_U);
        }
    }
    return rval;
}

uint32 ik_set_warp_info(uint32 context_id, const ik_warp_info_t *p_calib_warp_info)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((p_calib_warp_info != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        rval = p_ctx->organization.filter_methods.set_calib_warp_info(context_id, p_calib_warp_info);
    } else {
        if(p_calib_warp_info == NULL) {
            amba_ik_system_print_str_5("[IK] NULL pointer in %s\n", __func__, DC_S, DC_S, DC_S, DC_S);
            rval = IK_ERR_0005;
        } else if(p_ctx->organization.initial_flag == 0U) {
            amba_ik_system_print_uint32_5("[IK] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
            rval = IK_ERR_0002;
        } else {
            amba_ik_system_print_uint32_5("[IK] can't get context\n", DC_U, DC_U, DC_U, DC_U, DC_U);
        }
    }
    return rval;
}

uint32 ik_get_warp_info(uint32 context_id, ik_warp_info_t *p_calib_warp_info)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((p_calib_warp_info != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        rval = p_ctx->organization.filter_methods.get_calib_warp_info(context_id, p_calib_warp_info);
    } else {
        if(p_calib_warp_info == NULL) {
            amba_ik_system_print_str_5("[IK] NULL pointer in %s\n", __func__, DC_S, DC_S, DC_S, DC_S);
            rval = IK_ERR_0005;
        } else if(p_ctx->organization.initial_flag == 0U) {
            amba_ik_system_print_uint32_5("[IK] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
            rval = IK_ERR_0002;
        } else {
            amba_ik_system_print_uint32_5("[IK] can't get context\n", DC_U, DC_U, DC_U, DC_U, DC_U);
        }
    }
    return rval;
}

uint32 ik_set_cawarp_info(uint32 context_id, const ik_cawarp_info_t *p_calib_ca_warp_info)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((p_calib_ca_warp_info != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        rval = p_ctx->organization.filter_methods.set_calib_ca_warp_info(context_id, p_calib_ca_warp_info);
    } else {
        if(p_calib_ca_warp_info == NULL) {
            amba_ik_system_print_str_5("[IK] NULL pointer in %s\n", __func__, DC_S, DC_S, DC_S, DC_S);
            rval = IK_ERR_0005;
        } else if(p_ctx->organization.initial_flag == 0U) {
            amba_ik_system_print_uint32_5("[IK] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
            rval = IK_ERR_0002;
        } else {
            amba_ik_system_print_uint32_5("[IK] can't get context\n", DC_U, DC_U, DC_U, DC_U, DC_U);
        }
    }
    return rval;
}

uint32 ik_get_cawarp_info(uint32 context_id, ik_cawarp_info_t *p_calib_ca_warp_info)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((p_calib_ca_warp_info != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        rval = p_ctx->organization.filter_methods.get_calib_ca_warp_info(context_id, p_calib_ca_warp_info);
    } else {
        if(p_calib_ca_warp_info == NULL) {
            amba_ik_system_print_str_5("[IK] NULL pointer in %s\n", __func__, DC_S, DC_S, DC_S, DC_S);
            rval = IK_ERR_0005;
        } else if(p_ctx->organization.initial_flag == 0U) {
            amba_ik_system_print_uint32_5("[IK] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
            rval = IK_ERR_0002;
        } else {
            amba_ik_system_print_uint32_5("[IK] can't get context\n", DC_U, DC_U, DC_U, DC_U, DC_U);
        }
    }
    return rval;
}

uint32 ik_set_dzoom_info(uint32 context_id, const ik_dzoom_info_t *p_dzoom_info)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;
    const amba_img_dsp_debug_print_func_t *p_print_fun = ik_get_debug_print_func();
    if(p_print_fun != NULL) {
        p_print_fun->dzoom_info(context_id, p_dzoom_info);
    }

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((p_dzoom_info != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        rval = p_ctx->organization.filter_methods.set_dzoom_info(context_id, p_dzoom_info);
    } else {
        if(p_dzoom_info == NULL) {
            amba_ik_system_print_str_5("[IK] NULL pointer in %s\n", __func__, DC_S, DC_S, DC_S, DC_S);
            rval = IK_ERR_0005;
        } else if(p_ctx->organization.initial_flag == 0U) {
            amba_ik_system_print_uint32_5("[IK] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
            rval = IK_ERR_0002;
        } else {
            amba_ik_system_print_uint32_5("[IK] can't get context\n", DC_U, DC_U, DC_U, DC_U, DC_U);
        }
    }
    return rval;
}

uint32 ik_get_dzoom_info(uint32 context_id, ik_dzoom_info_t *p_dzoom_info)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((p_dzoom_info != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        rval = p_ctx->organization.filter_methods.get_dzoom_info(context_id, p_dzoom_info);
    } else {
        if(p_dzoom_info == NULL) {
            amba_ik_system_print_str_5("[IK] NULL pointer in %s\n", __func__, DC_S, DC_S, DC_S, DC_S);
            rval = IK_ERR_0005;
        } else if(p_ctx->organization.initial_flag == 0U) {
            amba_ik_system_print_uint32_5("[IK] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
            rval = IK_ERR_0002;
        } else {
            amba_ik_system_print_uint32_5("[IK] can't get context\n", DC_U, DC_U, DC_U, DC_U, DC_U);
        }
    }
    return rval;
}

uint32 ik_set_static_bad_pxl_corr_enb(uint32 context_id, const uint32 enb)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;
    const amba_img_dsp_debug_print_func_t *p_print_fun = ik_get_debug_print_func();
    if(p_print_fun != NULL) {
        p_print_fun->static_bpc_enable(context_id, enb);
    }

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        rval = p_ctx->organization.filter_methods.set_static_bpc_enable_info(context_id, enb);
    } else {
        if(p_ctx->organization.initial_flag == 0U) {
            amba_ik_system_print_uint32_5("[IK] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
            rval = IK_ERR_0002;
        } else {
            amba_ik_system_print_uint32_5("[IK] can't get context\n", DC_U, DC_U, DC_U, DC_U, DC_U);
        }
    }
    return rval;
}

uint32 ik_get_static_bad_pxl_corr_enb(uint32 context_id, uint32 *p_enb)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((p_enb != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        rval = p_ctx->organization.filter_methods.get_static_bpc_enable_info(context_id, p_enb);
    } else {
        if(p_enb == NULL) {
            amba_ik_system_print_str_5("[IK] NULL pointer in %s\n", __func__, DC_S, DC_S, DC_S, DC_S);
            rval = IK_ERR_0005;
        } else if(p_ctx->organization.initial_flag == 0U) {
            amba_ik_system_print_uint32_5("[IK] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
            rval = IK_ERR_0002;
        } else {
            amba_ik_system_print_uint32_5("[IK] can't get context\n", DC_U, DC_U, DC_U, DC_U, DC_U);
        }
    }
    return rval;
}

uint32 ik_set_vignette_enb(uint32 context_id, const uint32 enb)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;
    const amba_img_dsp_debug_print_func_t *p_print_fun = ik_get_debug_print_func();
    if(p_print_fun != NULL) {
        p_print_fun->vignette_enable(context_id, enb);
    }

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        rval = p_ctx->organization.filter_methods.set_vignette_enable_info(context_id, enb);
    } else {
        if(p_ctx->organization.initial_flag == 0U) {
            amba_ik_system_print_uint32_5("[IK] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
            rval = IK_ERR_0002;
        } else {
            amba_ik_system_print_uint32_5("[IK] can't get context\n", DC_U, DC_U, DC_U, DC_U, DC_U);
        }
    }
    return rval;
}

uint32 ik_get_vignette_enb(uint32 context_id, uint32 *p_enb)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((p_enb != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        rval = p_ctx->organization.filter_methods.get_vignette_enable_info(context_id, p_enb);
    } else {
        if(p_enb == NULL) {
            amba_ik_system_print_str_5("[IK] NULL pointer in %s\n", __func__, DC_S, DC_S, DC_S, DC_S);
            rval = IK_ERR_0005;
        } else if(p_ctx->organization.initial_flag == 0U) {
            amba_ik_system_print_uint32_5("[IK] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
            rval = IK_ERR_0002;
        } else {
            amba_ik_system_print_uint32_5("[IK] can't get context\n", DC_U, DC_U, DC_U, DC_U, DC_U);
        }
    }
    return rval;
}

uint32 ik_set_warp_enb(uint32 context_id, const uint32 enb)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;
    const amba_img_dsp_debug_print_func_t *p_print_fun = ik_get_debug_print_func();
    if(p_print_fun != NULL) {
        p_print_fun->warp_enable(context_id, enb);
    }

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        rval = p_ctx->organization.filter_methods.set_warp_enable_info(context_id, enb);
    } else {
        if(p_ctx->organization.initial_flag == 0U) {
            amba_ik_system_print_uint32_5("[IK] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
            rval = IK_ERR_0002;
        } else {
            amba_ik_system_print_uint32_5("[IK] can't get context\n", DC_U, DC_U, DC_U, DC_U, DC_U);
        }
    }
    return rval;
}

uint32 ik_get_warp_enb(uint32 context_id, uint32 *p_enb)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((p_enb != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        rval = p_ctx->organization.filter_methods.get_warp_enable_info(context_id, p_enb);
    } else {
        if(p_enb == NULL) {
            amba_ik_system_print_str_5("[IK] NULL pointer in %s\n", __func__, DC_S, DC_S, DC_S, DC_S);
            rval = IK_ERR_0005;
        } else if(p_ctx->organization.initial_flag == 0U) {
            amba_ik_system_print_uint32_5("[IK] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
            rval = IK_ERR_0002;
        } else {
            amba_ik_system_print_uint32_5("[IK] can't get context\n", DC_U, DC_U, DC_U, DC_U, DC_U);
        }
    }
    return rval;
}

uint32 ik_set_cawarp_enb(uint32 context_id, const uint32 enb)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;
    const amba_img_dsp_debug_print_func_t *p_print_fun = ik_get_debug_print_func();
    if(p_print_fun != NULL) {
        p_print_fun->cawarp_enable(context_id, enb);
    }

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        rval = p_ctx->organization.filter_methods.set_cawarp_enable_info(context_id, enb);
    } else {
        if(p_ctx->organization.initial_flag == 0U) {
            amba_ik_system_print_uint32_5("[IK] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
            rval = IK_ERR_0002;
        } else {
            amba_ik_system_print_uint32_5("[IK] can't get context\n", DC_U, DC_U, DC_U, DC_U, DC_U);
        }
    }
    return rval;
}

uint32 ik_get_cawarp_enb(uint32 context_id, uint32 *p_enb)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((p_enb != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        rval = p_ctx->organization.filter_methods.get_cawarp_enable_info(context_id, p_enb);
    } else {
        if(p_enb == NULL) {
            amba_ik_system_print_str_5("[IK] NULL pointer in %s\n", __func__, DC_S, DC_S, DC_S, DC_S);
            rval = IK_ERR_0005;
        } else if(p_ctx->organization.initial_flag == 0U) {
            amba_ik_system_print_uint32_5("[IK] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
            rval = IK_ERR_0002;
        } else {
            amba_ik_system_print_uint32_5("[IK] can't get context\n", DC_U, DC_U, DC_U, DC_U, DC_U);
        }
    }
    return rval;
}

uint32 ik_set_dummy_margin_range(uint32 context_id, const ik_dummy_margin_range_t *p_dmy_margin_range)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;
    const amba_img_dsp_debug_print_func_t *p_print_fun = ik_get_debug_print_func();
    if(p_print_fun != NULL) {
        p_print_fun->dummy_margin_range(context_id, p_dmy_margin_range);
    }

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        rval = p_ctx->organization.filter_methods.set_dmy_range_info(context_id, p_dmy_margin_range);
    } else {
        if(p_ctx->organization.initial_flag == 0U) {
            amba_ik_system_print_uint32_5("[IK] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
            rval = IK_ERR_0002;
        } else {
            amba_ik_system_print_uint32_5("[IK] can't get context\n", DC_U, DC_U, DC_U, DC_U, DC_U);
        }
    }
    return rval;
}

uint32 ik_get_dummy_margin_range(uint32 context_id, ik_dummy_margin_range_t *p_dmy_margin_range)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((p_dmy_margin_range != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        rval = p_ctx->organization.filter_methods.get_dmy_range_info(context_id, p_dmy_margin_range);
    } else {
        if(p_dmy_margin_range == NULL) {
            amba_ik_system_print_str_5("[IK] NULL pointer in %s\n", __func__, DC_S, DC_S, DC_S, DC_S);
            rval = IK_ERR_0005;
        } else if(p_ctx->organization.initial_flag == 0U) {
            amba_ik_system_print_uint32_5("[IK] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
            rval = IK_ERR_0002;
        } else {
            amba_ik_system_print_uint32_5("[IK] can't get context\n", DC_U, DC_U, DC_U, DC_U, DC_U);
        }
    }
    return rval;
}
/*
uint32 amba_ik_set_flip_mode(uint32 context_id, const int32 mode)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        rval = p_ctx->organization.filter_methods.set_flip_mode(context_id, mode);
    } else {
        if(p_ctx->organization.initial_flag == 0U){
            amba_ik_system_print_uint32_5("[IK] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
        } else {
            amba_ik_system_print_uint32_5("[IK] can't get context\n", DC_U, DC_U, DC_U, DC_U, DC_U);
        }
    }
    return rval;
}
*/
uint32 ik_set_vin_active_win(uint32 context_id, const ik_vin_active_window_t *p_vin_active_win)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;
    const amba_img_dsp_debug_print_func_t *p_print_fun = ik_get_debug_print_func();
    if(p_print_fun != NULL) {
        p_print_fun->vin_active_win(context_id, p_vin_active_win);
    }

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((p_vin_active_win != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        rval = p_ctx->organization.filter_methods.set_active_win(context_id, p_vin_active_win);
    } else {
        if(p_vin_active_win == NULL) {
            amba_ik_system_print_str_5("[IK] NULL pointer in %s\n", __func__, DC_S, DC_S, DC_S, DC_S);
            rval = IK_ERR_0005;
        } else if(p_ctx->organization.initial_flag == 0U) {
            amba_ik_system_print_uint32_5("[IK] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
            rval = IK_ERR_0002;
        } else {
            amba_ik_system_print_uint32_5("[IK] can't get context\n", DC_U, DC_U, DC_U, DC_U, DC_U);
        }
    }
    return rval;
}

uint32 ik_get_vin_active_win(uint32 context_id, ik_vin_active_window_t *p_vin_active_win)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((p_vin_active_win != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        rval = p_ctx->organization.filter_methods.get_active_win(context_id, p_vin_active_win);
    } else {
        if(p_vin_active_win == NULL) {
            amba_ik_system_print_str_5("[IK] NULL pointer in %s\n", __func__, DC_S, DC_S, DC_S, DC_S);
            rval = IK_ERR_0005;
        } else if(p_ctx->organization.initial_flag == 0U) {
            amba_ik_system_print_uint32_5("[IK] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
            rval = IK_ERR_0002;
        } else {
            amba_ik_system_print_uint32_5("[IK] can't get context\n", DC_U, DC_U, DC_U, DC_U, DC_U);
        }
    }
    return rval;
}

uint32 ik_set_warp_internal(uint32 context_id, const ik_warp_internal_info_t *p_warp_dzoom_internal)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((p_warp_dzoom_internal != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        rval = p_ctx->organization.filter_methods.set_warp_internal(context_id, p_warp_dzoom_internal);
    } else {
        if(p_warp_dzoom_internal == NULL) {
            amba_ik_system_print_str_5("[IK] NULL pointer in %s\n", __func__, DC_S, DC_S, DC_S, DC_S);
            rval = IK_ERR_0005;
        } else if(p_ctx->organization.initial_flag == 0U) {
            amba_ik_system_print_uint32_5("[IK] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
            rval = IK_ERR_0002;
        } else {
            amba_ik_system_print_uint32_5("[IK] can't get context\n", DC_U, DC_U, DC_U, DC_U, DC_U);
        }
    }
    return rval;
}

uint32 ik_get_warp_internal(uint32 context_id, ik_warp_internal_info_t *p_warp_dzoom_internal)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((p_warp_dzoom_internal != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        rval = p_ctx->organization.filter_methods.get_warp_internal(context_id, p_warp_dzoom_internal);
    } else {
        if(p_warp_dzoom_internal == NULL) {
            amba_ik_system_print_str_5("[IK] NULL pointer in %s\n", __func__, DC_S, DC_S, DC_S, DC_S);
            rval = IK_ERR_0005;
        } else if(p_ctx->organization.initial_flag == 0U) {
            amba_ik_system_print_uint32_5("[IK] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
            rval = IK_ERR_0002;
        } else {
            amba_ik_system_print_uint32_5("[IK] can't get context\n", DC_U, DC_U, DC_U, DC_U, DC_U);
        }
    }
    return rval;
}

uint32 ik_set_cawarp_internal(uint32 context_id, const ik_cawarp_internal_info_t *p_cawarp_internal)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((p_cawarp_internal != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        rval = p_ctx->organization.filter_methods.set_ca_warp_internal(context_id, p_cawarp_internal);
    } else {
        if(p_cawarp_internal == NULL) {
            amba_ik_system_print_str_5("[IK] NULL pointer in %s\n", __func__, DC_S, DC_S, DC_S, DC_S);
            rval = IK_ERR_0005;
        } else if(p_ctx->organization.initial_flag == 0U) {
            amba_ik_system_print_uint32_5("[IK] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
            rval = IK_ERR_0002;
        } else {
            amba_ik_system_print_uint32_5("[IK] can't get context\n", DC_U, DC_U, DC_U, DC_U, DC_U);
        }
    }
    return rval;
}

uint32 ik_get_cawarp_internal(uint32 context_id, ik_cawarp_internal_info_t *p_cawarp_internal)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((p_cawarp_internal != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        rval = p_ctx->organization.filter_methods.get_ca_warp_internal(context_id, p_cawarp_internal);
    } else {
        if(p_cawarp_internal == NULL) {
            amba_ik_system_print_str_5("[IK] NULL pointer in %s\n", __func__, DC_S, DC_S, DC_S, DC_S);
            rval = IK_ERR_0005;
        } else if(p_ctx->organization.initial_flag == 0U) {
            amba_ik_system_print_uint32_5("[IK] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
            rval = IK_ERR_0002;
        } else {
            amba_ik_system_print_uint32_5("[IK] can't get context\n", DC_U, DC_U, DC_U, DC_U, DC_U);
        }
    }
    return rval;
}

uint32 amba_ik_set_mctf_internal(uint32 context_id, const ik_mctf_internal_t *p_mctf_internal)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((p_mctf_internal != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        rval = p_ctx->organization.filter_methods.set_mctf_internal(context_id, p_mctf_internal);
    } else {
        if(p_mctf_internal == NULL) {
            amba_ik_system_print_str_5("[IK] NULL pointer in %s\n", __func__, DC_S, DC_S, DC_S, DC_S);
            rval = IK_ERR_0005;
        } else if(p_ctx->organization.initial_flag == 0U) {
            amba_ik_system_print_uint32_5("[IK] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
            rval = IK_ERR_0002;
        } else {
            amba_ik_system_print_uint32_5("[IK] can't get context\n", DC_U, DC_U, DC_U, DC_U, DC_U);
        }
    }
    return rval;
}

uint32 amba_ik_get_mctf_internal(uint32 context_id, ik_mctf_internal_t *p_mctf_internal)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((p_mctf_internal != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        rval = p_ctx->organization.filter_methods.get_mctf_internal(context_id, p_mctf_internal);
    } else {
        if(p_mctf_internal == NULL) {
            amba_ik_system_print_str_5("[IK] NULL pointer in %s\n", __func__, DC_S, DC_S, DC_S, DC_S);
            rval = IK_ERR_0005;
        } else if(p_ctx->organization.initial_flag == 0U) {
            amba_ik_system_print_uint32_5("[IK] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
            rval = IK_ERR_0002;
        } else {
            amba_ik_system_print_uint32_5("[IK] can't get context\n", DC_U, DC_U, DC_U, DC_U, DC_U);
        }
    }
    return rval;
}

uint32 ik_set_hdr_raw_offset(uint32 context_id, const ik_hdr_raw_info_t *p_hdr_raw_info)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((p_hdr_raw_info != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        rval = p_ctx->organization.filter_methods.set_hdr_raw_offset(context_id, p_hdr_raw_info);
    } else {
        if(p_hdr_raw_info == NULL) {
            amba_ik_system_print_str_5("[IK] NULL pointer in %s\n", __func__, DC_S, DC_S, DC_S, DC_S);
            rval = IK_ERR_0005;
        } else if(p_ctx->organization.initial_flag == 0U) {
            amba_ik_system_print_uint32_5("[IK] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
            rval = IK_ERR_0002;
        } else {
            amba_ik_system_print_uint32_5("[IK] can't get context\n", DC_U, DC_U, DC_U, DC_U, DC_U);
        }
    }
    return rval;
}

uint32 ik_get_hdr_raw_offset(uint32 context_id, ik_hdr_raw_info_t *p_hdr_raw_info)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((p_hdr_raw_info != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        rval = p_ctx->organization.filter_methods.get_hdr_raw_offset(context_id, p_hdr_raw_info);
    } else {
        if(p_hdr_raw_info == NULL) {
            amba_ik_system_print_str_5("[IK] NULL pointer in %s\n", __func__, DC_S, DC_S, DC_S, DC_S);
            rval = IK_ERR_0005;
        } else if(p_ctx->organization.initial_flag == 0U) {
            amba_ik_system_print_uint32_5("[IK] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
            rval = IK_ERR_0002;
        } else {
            amba_ik_system_print_uint32_5("[IK] can't get context\n", DC_U, DC_U, DC_U, DC_U, DC_U);
        }
    }
    return rval;
}

uint32 ik_set_flip_mode(uint32 context_id, const uint32 mode)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        rval = p_ctx->organization.filter_methods.set_flip_mode(context_id, mode);
    } else {
        if(p_ctx->organization.initial_flag == 0U) {
            amba_ik_system_print_uint32_5("[IK] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
            rval = IK_ERR_0002;
        } else {
            amba_ik_system_print_uint32_5("[IK] can't get context\n", DC_U, DC_U, DC_U, DC_U, DC_U);
        }
    }
    return rval;
}

uint32 ik_get_flip_mode(uint32 context_id, uint32 *p_mode)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((p_mode != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        rval = p_ctx->organization.filter_methods.get_flip_mode(context_id, p_mode);
    } else {
        if(p_mode == NULL) {
            amba_ik_system_print_str_5("[IK] NULL pointer in %s\n", __func__, DC_S, DC_S, DC_S, DC_S);
            rval = IK_ERR_0005;
        } else if(p_ctx->organization.initial_flag == 0U) {
            amba_ik_system_print_uint32_5("[IK] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
            rval = IK_ERR_0002;
        } else {
            amba_ik_system_print_uint32_5("[IK] can't get context\n", DC_U, DC_U, DC_U, DC_U, DC_U);
        }
    }
    return rval;
}

uint32 ik_set_y2y_use_cc_enable_info(uint32 context_id, const uint32 enable)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        rval = p_ctx->organization.filter_methods.set_y2y_use_cc_enable_info(context_id, enable);
    } else {
        if(p_ctx->organization.initial_flag == 0U) {
            amba_ik_system_print_uint32_5("[IK] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
            rval = IK_ERR_0002;
        } else {
            amba_ik_system_print_uint32_5("[IK] can't get context\n", DC_U, DC_U, DC_U, DC_U, DC_U);
        }
    }
    return rval;
}

uint32 ik_get_y2y_use_cc_enable_info(uint32 context_id, uint32 *enable)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        rval = p_ctx->organization.filter_methods.get_y2y_use_cc_enable_info(context_id, enable);
    } else {
        if(p_ctx->organization.initial_flag == 0U) {
            amba_ik_system_print_uint32_5("[IK] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
            rval = IK_ERR_0002;
        } else {
            amba_ik_system_print_uint32_5("[IK] can't get context\n", DC_U, DC_U, DC_U, DC_U, DC_U);
        }
    }
    return rval;
}

#if SUPPORT_FUSION
uint32 ik_set_fusion(uint32 context_id, const ik_fusion_t *p_fusion)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((p_fusion != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        rval = p_ctx->organization.filter_methods.set_fusion(context_id, p_fusion);
    } else {
        if(p_fusion == NULL) {
            amba_ik_system_print_str_5("[IK] NULL pointer in %s\n", __func__, DC_S, DC_S, DC_S, DC_S);
            rval = IK_ERR_0005;
        } else if(p_ctx->organization.initial_flag == 0U) {
            amba_ik_system_print_uint32_5("[IK] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
            rval = IK_ERR_0002;
        } else {
            amba_ik_system_print_uint32_5("[IK] can't get context\n", DC_U, DC_U, DC_U, DC_U, DC_U);
        }
    }
    return rval;
}
uint32 ik_get_fusion(uint32 context_id, ik_fusion_t *p_fusion)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((p_fusion != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        rval = p_ctx->organization.filter_methods.get_fusion(context_id, p_fusion);
    } else {
        if(p_fusion == NULL) {
            amba_ik_system_print_str_5("[IK] NULL pointer in %s\n", __func__, DC_S, DC_S, DC_S, DC_S);
            rval = IK_ERR_0005;
        } else if(p_ctx->organization.initial_flag == 0U) {
            amba_ik_system_print_uint32_5("[IK] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
            rval = IK_ERR_0002;
        } else {
            amba_ik_system_print_uint32_5("[IK] can't get context\n", DC_U, DC_U, DC_U, DC_U, DC_U);
        }
    }
    return rval;
}

uint32 ik_set_mono_static_bad_pxl_corr(uint32 context_id, const ik_static_bad_pxl_cor_t *p_static_bpc)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;
    const amba_img_dsp_debug_print_func_t *p_print_fun = ik_get_debug_print_func();
    if(p_print_fun != NULL) {
        p_print_fun->static_bpc(context_id, p_static_bpc);
    }

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((p_static_bpc != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        rval = p_ctx->organization.filter_methods.set_mono_static_bpc(context_id, p_static_bpc);
    } else {
        if(p_static_bpc == NULL) {
            amba_ik_system_print_str_5("[IK] NULL pointer in %s\n", __func__, DC_S, DC_S, DC_S, DC_S);
            rval = IK_ERR_0005;
        } else if(p_ctx->organization.initial_flag == 0U) {
            amba_ik_system_print_uint32_5("[IK] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
            rval = IK_ERR_0002;
        } else {
            amba_ik_system_print_uint32_5("[IK] can't get context\n", DC_U, DC_U, DC_U, DC_U, DC_U);
        }
    }
    return rval;
}

uint32 ik_get_mono_static_bad_pxl_corr(uint32 context_id, ik_static_bad_pxl_cor_t *p_static_bpc)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((p_static_bpc != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        rval = p_ctx->organization.filter_methods.get_mono_static_bpc(context_id, p_static_bpc);
    } else {
        if(p_static_bpc == NULL) {
            amba_ik_system_print_str_5("[IK] NULL pointer in %s\n", __func__, DC_S, DC_S, DC_S, DC_S);
            rval = IK_ERR_0005;
        } else if(p_ctx->organization.initial_flag == 0U) {
            amba_ik_system_print_uint32_5("[IK] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
            rval = IK_ERR_0002;
        } else {
            amba_ik_system_print_uint32_5("[IK] can't get context\n", DC_U, DC_U, DC_U, DC_U, DC_U);
        }
    }
    return rval;
}

uint32 ik_set_mono_vignette(uint32 context_id, const ik_vignette_t *p_vignette)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;
    const amba_img_dsp_debug_print_func_t *p_print_fun = ik_get_debug_print_func();
    if(p_print_fun != NULL) {
        p_print_fun->vignette_info(context_id, p_vignette);
    }

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((p_vignette != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        rval = p_ctx->organization.filter_methods.set_mono_vignette_compensation(context_id, p_vignette);
    } else {
        if(p_vignette == NULL) {
            amba_ik_system_print_str_5("[IK] NULL pointer in %s\n", __func__, DC_S, DC_S, DC_S, DC_S);
            rval = IK_ERR_0005;
        } else if(p_ctx->organization.initial_flag == 0U) {
            amba_ik_system_print_uint32_5("[IK] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
            rval = IK_ERR_0002;
        } else {
            amba_ik_system_print_uint32_5("[IK] can't get context\n", DC_U, DC_U, DC_U, DC_U, DC_U);
        }
    }
    return rval;
}

uint32 ik_get_mono_vignette(uint32 context_id, ik_vignette_t *p_vignette)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((p_vignette != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        rval = p_ctx->organization.filter_methods.get_mono_vignette_compensation(context_id, p_vignette);
    } else {
        if(p_vignette == NULL) {
            amba_ik_system_print_str_5("[IK] NULL pointer in %s\n", __func__, DC_S, DC_S, DC_S, DC_S);
            rval = IK_ERR_0005;
        } else if(p_ctx->organization.initial_flag == 0U) {
            amba_ik_system_print_uint32_5("[IK] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
            rval = IK_ERR_0002;
        } else {
            amba_ik_system_print_uint32_5("[IK] can't get context\n", DC_U, DC_U, DC_U, DC_U, DC_U);
        }
    }
    return rval;
}

uint32 ik_set_mono_before_ce_wb_gain(uint32 context_id, const ik_wb_gain_t *p_before_ce_wb_gain)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;
    const amba_img_dsp_debug_print_func_t *p_print_fun = ik_get_debug_print_func();
    if(p_print_fun != NULL) {
        p_print_fun->before_ce_wb(context_id, p_before_ce_wb_gain);
    }

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((p_before_ce_wb_gain != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        rval = p_ctx->organization.filter_methods.set_mono_before_ce_wb_gain(context_id, p_before_ce_wb_gain);
    } else {
        if(p_before_ce_wb_gain == NULL) {
            amba_ik_system_print_str_5("[IK] NULL pointer in %s\n", __func__, DC_S, DC_S, DC_S, DC_S);
            rval = IK_ERR_0005;
        } else if(p_ctx->organization.initial_flag == 0U) {
            amba_ik_system_print_uint32_5("[IK] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
            rval = IK_ERR_0002;
        } else {
            amba_ik_system_print_uint32_5("[IK] can't get context\n", DC_U, DC_U, DC_U, DC_U, DC_U);
        }
    }
    return rval;
}

uint32 ik_get_mono_before_ce_wb_gain(uint32 context_id, ik_wb_gain_t *p_before_ce_wb_gain)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((p_before_ce_wb_gain != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        rval = p_ctx->organization.filter_methods.get_mono_before_ce_wb_gain(context_id, p_before_ce_wb_gain);
    } else {
        if(p_before_ce_wb_gain == NULL) {
            amba_ik_system_print_str_5("[IK] NULL pointer in %s\n", __func__, DC_S, DC_S, DC_S, DC_S);
            rval = IK_ERR_0005;
        } else if(p_ctx->organization.initial_flag == 0U) {
            amba_ik_system_print_uint32_5("[IK] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
            rval = IK_ERR_0002;
        } else {
            amba_ik_system_print_uint32_5("[IK] can't get context\n", DC_U, DC_U, DC_U, DC_U, DC_U);
        }
    }
    return rval;
}

uint32 ik_set_mono_after_ce_wb_gain(uint32 context_id, const ik_wb_gain_t *p_after_ce_wb_gain)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;
    const amba_img_dsp_debug_print_func_t *p_print_fun = ik_get_debug_print_func();
    if(p_print_fun != NULL) {
        p_print_fun->after_ce_wb(context_id, p_after_ce_wb_gain);
    }

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((p_after_ce_wb_gain != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        rval = p_ctx->organization.filter_methods.set_mono_after_ce_wb_gain(context_id, p_after_ce_wb_gain);
    } else {
        if(p_after_ce_wb_gain == NULL) {
            amba_ik_system_print_str_5("[IK] NULL pointer in %s\n", __func__, DC_S, DC_S, DC_S, DC_S);
            rval = IK_ERR_0005;
        } else if(p_ctx->organization.initial_flag == 0U) {
            amba_ik_system_print_uint32_5("[IK] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
            rval = IK_ERR_0002;
        } else {
            amba_ik_system_print_uint32_5("[IK] can't get context\n", DC_U, DC_U, DC_U, DC_U, DC_U);
        }
    }
    return rval;
}

uint32 ik_get_mono_after_ce_wb_gain(uint32 context_id, ik_wb_gain_t *p_after_ce_wb_gain)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((p_after_ce_wb_gain != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        rval = p_ctx->organization.filter_methods.get_mono_after_ce_wb_gain(context_id, p_after_ce_wb_gain);
    } else {
        if(p_after_ce_wb_gain == NULL) {
            amba_ik_system_print_str_5("[IK] NULL pointer in %s\n", __func__, DC_S, DC_S, DC_S, DC_S);
            rval = IK_ERR_0005;
        } else if(p_ctx->organization.initial_flag == 0U) {
            amba_ik_system_print_uint32_5("[IK] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
            rval = IK_ERR_0002;
        } else {
            amba_ik_system_print_uint32_5("[IK] can't get context\n", DC_U, DC_U, DC_U, DC_U, DC_U);
        }
    }
    return rval;
}

uint32 ik_set_mono_cfa_leakage_filter(uint32 context_id, const ik_cfa_leakage_filter_t *p_cfa_leakage_filter)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;
    const amba_img_dsp_debug_print_func_t *p_print_fun = ik_get_debug_print_func();
    if(p_print_fun != NULL) {
        p_print_fun->cfa_leakage(context_id, p_cfa_leakage_filter);
    }

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((p_cfa_leakage_filter != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        rval = p_ctx->organization.filter_methods.set_mono_cfa_leakage_filter(context_id, p_cfa_leakage_filter);
    } else {
        if(p_cfa_leakage_filter == NULL) {
            amba_ik_system_print_str_5("[IK] NULL pointer in %s\n", __func__, DC_S, DC_S, DC_S, DC_S);
            rval = IK_ERR_0005;
        } else if(p_ctx->organization.initial_flag == 0U) {
            amba_ik_system_print_uint32_5("[IK] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
            rval = IK_ERR_0002;
        } else {
            amba_ik_system_print_uint32_5("[IK] can't get context\n", DC_U, DC_U, DC_U, DC_U, DC_U);
        }
    }
    return rval;
}

uint32 ik_get_mono_cfa_leakage_filter(uint32 context_id, ik_cfa_leakage_filter_t *p_cfa_leakage_filter)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((p_cfa_leakage_filter != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        rval = p_ctx->organization.filter_methods.get_mono_cfa_leakage_filter(context_id, p_cfa_leakage_filter);
    } else {
        if(p_cfa_leakage_filter == NULL) {
            amba_ik_system_print_str_5("[IK] NULL pointer in %s\n", __func__, DC_S, DC_S, DC_S, DC_S);
            rval = IK_ERR_0005;
        } else if(p_ctx->organization.initial_flag == 0U) {
            amba_ik_system_print_uint32_5("[IK] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
            rval = IK_ERR_0002;
        } else {
            amba_ik_system_print_uint32_5("[IK] can't get context\n", DC_U, DC_U, DC_U, DC_U, DC_U);
        }
    }
    return rval;
}

uint32 ik_set_mono_anti_aliasing(uint32 context_id, const ik_anti_aliasing_t *p_anti_aliasing)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;
    const amba_img_dsp_debug_print_func_t *p_print_fun = ik_get_debug_print_func();
    if(p_print_fun != NULL) {
        p_print_fun->anti_aliasing(context_id, p_anti_aliasing);
    }

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((p_anti_aliasing != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        rval = p_ctx->organization.filter_methods.set_mono_anti_aliasing(context_id, p_anti_aliasing);
    } else {
        if(p_anti_aliasing == NULL) {
            amba_ik_system_print_str_5("[IK] NULL pointer in %s\n", __func__, DC_S, DC_S, DC_S, DC_S);
            rval = IK_ERR_0005;
        } else if(p_ctx->organization.initial_flag == 0U) {
            amba_ik_system_print_uint32_5("[IK] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
            rval = IK_ERR_0002;
        } else {
            amba_ik_system_print_uint32_5("[IK] can't get context\n", DC_U, DC_U, DC_U, DC_U, DC_U);
        }
    }
    return rval;
}

uint32 ik_get_mono_anti_aliasing(uint32 context_id, ik_anti_aliasing_t *p_anti_aliasing)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((p_anti_aliasing != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        rval = p_ctx->organization.filter_methods.get_mono_anti_aliasing(context_id, p_anti_aliasing);
    } else {
        if(p_anti_aliasing == NULL) {
            amba_ik_system_print_str_5("[IK] NULL pointer in %s\n", __func__, DC_S, DC_S, DC_S, DC_S);
            rval = IK_ERR_0005;
        } else if(p_ctx->organization.initial_flag == 0U) {
            amba_ik_system_print_uint32_5("[IK] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
            rval = IK_ERR_0002;
        } else {
            amba_ik_system_print_uint32_5("[IK] can't get context\n", DC_U, DC_U, DC_U, DC_U, DC_U);
        }
    }
    return rval;
}

uint32 ik_set_mono_dynamic_bad_pixel_corr(uint32 context_id, const ik_dynamic_bad_pixel_correction_t *p_dynamic_bpc)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;
    const amba_img_dsp_debug_print_func_t *p_print_fun = ik_get_debug_print_func();
    if(p_print_fun != NULL) {
        p_print_fun->dynamic_bad_pxl_cor(context_id, p_dynamic_bpc);
    }

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((p_dynamic_bpc != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        rval = p_ctx->organization.filter_methods.set_mono_dynamic_bpc(context_id, p_dynamic_bpc);
    } else {
        if(p_dynamic_bpc == NULL) {
            amba_ik_system_print_str_5("[IK] NULL pointer in %s\n", __func__, DC_S, DC_S, DC_S, DC_S);
            rval = IK_ERR_0005;
        } else if(p_ctx->organization.initial_flag == 0U) {
            amba_ik_system_print_uint32_5("[IK] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
            rval = IK_ERR_0002;
        } else {
            amba_ik_system_print_uint32_5("[IK] can't get context\n", DC_U, DC_U, DC_U, DC_U, DC_U);
        }
    }
    return rval;
}

uint32 ik_get_mono_dynamic_bad_pixel_corr(uint32 context_id, ik_dynamic_bad_pixel_correction_t *p_dynamic_bpc)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((p_dynamic_bpc != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        rval = p_ctx->organization.filter_methods.get_mono_dynamic_bpc(context_id, p_dynamic_bpc);
    } else {
        if(p_dynamic_bpc == NULL) {
            amba_ik_system_print_str_5("[IK] NULL pointer in %s\n", __func__, DC_S, DC_S, DC_S, DC_S);
            rval = IK_ERR_0005;
        } else if(p_ctx->organization.initial_flag == 0U) {
            amba_ik_system_print_uint32_5("[IK] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
            rval = IK_ERR_0002;
        } else {
            amba_ik_system_print_uint32_5("[IK] can't get context\n", DC_U, DC_U, DC_U, DC_U, DC_U);
        }
    }
    return rval;
}

uint32 ik_set_mono_grgb_mismatch(uint32 context_id, const ik_grgb_mismatch_t *p_grgb_mismatch)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;
    const amba_img_dsp_debug_print_func_t *p_print_fun = ik_get_debug_print_func();
    if(p_print_fun != NULL) {
        p_print_fun->grgb_mismatch(context_id, p_grgb_mismatch);
    }

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((p_grgb_mismatch != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        rval = p_ctx->organization.filter_methods.set_mono_grgb_mismatch(context_id, p_grgb_mismatch);
    } else {
        if(p_grgb_mismatch == NULL) {
            amba_ik_system_print_str_5("[IK] NULL pointer in %s\n", __func__, DC_S, DC_S, DC_S, DC_S);
            rval = IK_ERR_0005;
        } else if(p_ctx->organization.initial_flag == 0U) {
            amba_ik_system_print_uint32_5("[IK] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
            rval = IK_ERR_0002;
        } else {
            amba_ik_system_print_uint32_5("[IK] can't get context\n", DC_U, DC_U, DC_U, DC_U, DC_U);
        }
    }
    return rval;
}

uint32 ik_get_mono_grgb_mismatch(uint32 context_id, ik_grgb_mismatch_t *p_grgb_mismatch)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((p_grgb_mismatch != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        rval = p_ctx->organization.filter_methods.get_mono_grgb_mismatch(context_id, p_grgb_mismatch);
    } else {
        if(p_grgb_mismatch == NULL) {
            amba_ik_system_print_str_5("[IK] NULL pointer in %s\n", __func__, DC_S, DC_S, DC_S, DC_S);
            rval = IK_ERR_0005;
        } else if(p_ctx->organization.initial_flag == 0U) {
            amba_ik_system_print_uint32_5("[IK] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
            rval = IK_ERR_0002;
        } else {
            amba_ik_system_print_uint32_5("[IK] can't get context\n", DC_U, DC_U, DC_U, DC_U, DC_U);
        }
    }
    return rval;
}

uint32 ik_set_mono_cfa_noise_filter(uint32 context_id, const ik_cfa_noise_filter_t *p_cfa_noise_filter)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;
    const amba_img_dsp_debug_print_func_t *p_print_fun = ik_get_debug_print_func();
    if(p_print_fun != NULL) {
        p_print_fun->cfa_noise_filter(context_id, p_cfa_noise_filter);
    }

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((p_cfa_noise_filter != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        rval = p_ctx->organization.filter_methods.set_mono_cfa_noise_filter(context_id, p_cfa_noise_filter);
    } else {
        if(p_cfa_noise_filter == NULL) {
            amba_ik_system_print_str_5("[IK] NULL pointer in %s\n", __func__, DC_S, DC_S, DC_S, DC_S);
            rval = IK_ERR_0005;
        } else if(p_ctx->organization.initial_flag == 0U) {
            amba_ik_system_print_uint32_5("[IK] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
            rval = IK_ERR_0002;
        } else {
            amba_ik_system_print_uint32_5("[IK] can't get context\n", DC_U, DC_U, DC_U, DC_U, DC_U);
        }
    }
    return rval;
}

uint32 ik_get_mono_cfa_noise_filter(uint32 context_id, ik_cfa_noise_filter_t *p_cfa_noise_filter)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((p_cfa_noise_filter != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        rval = p_ctx->organization.filter_methods.get_mono_cfa_noise_filter(context_id, p_cfa_noise_filter);
    } else {
        if(p_cfa_noise_filter == NULL) {
            amba_ik_system_print_str_5("[IK] NULL pointer in %s\n", __func__, DC_S, DC_S, DC_S, DC_S);
            rval = IK_ERR_0005;
        } else if(p_ctx->organization.initial_flag == 0U) {
            amba_ik_system_print_uint32_5("[IK] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
            rval = IK_ERR_0002;
        } else {
            amba_ik_system_print_uint32_5("[IK] can't get context\n", DC_U, DC_U, DC_U, DC_U, DC_U);
        }
    }
    return rval;
}

uint32 ik_set_mono_demosaic(uint32 context_id, const ik_demosaic_t *p_demosaic)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;
    const amba_img_dsp_debug_print_func_t *p_print_fun = ik_get_debug_print_func();
    if(p_print_fun != NULL) {
        p_print_fun->demosaic(context_id, p_demosaic);
    }

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((p_demosaic != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        rval = p_ctx->organization.filter_methods.set_mono_demosaic(context_id, p_demosaic);
    } else {
        if(p_demosaic == NULL) {
            amba_ik_system_print_str_5("[IK] NULL pointer in %s\n", __func__, DC_S, DC_S, DC_S, DC_S);
            rval = IK_ERR_0005;
        } else if(p_ctx->organization.initial_flag == 0U) {
            amba_ik_system_print_uint32_5("[IK] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
            rval = IK_ERR_0002;
        } else {
            amba_ik_system_print_uint32_5("[IK] can't get context\n", DC_U, DC_U, DC_U, DC_U, DC_U);
        }
    }
    return rval;
}

uint32 ik_get_mono_demosaic(uint32 context_id, ik_demosaic_t *p_demosaic)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((p_demosaic != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        rval = p_ctx->organization.filter_methods.get_mono_demosaic(context_id, p_demosaic);
    } else {
        if(p_demosaic == NULL) {
            amba_ik_system_print_str_5("[IK] NULL pointer in %s\n", __func__, DC_S, DC_S, DC_S, DC_S);
            rval = IK_ERR_0005;
        } else if(p_ctx->organization.initial_flag == 0U) {
            amba_ik_system_print_uint32_5("[IK] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
            rval = IK_ERR_0002;
        } else {
            amba_ik_system_print_uint32_5("[IK] can't get context\n", DC_U, DC_U, DC_U, DC_U, DC_U);
        }
    }
    return rval;
}

uint32 ik_set_mono_color_correction_reg(uint32 context_id, const ik_color_correction_reg_t *p_color_correction_reg)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;
    const amba_img_dsp_debug_print_func_t *p_print_fun = ik_get_debug_print_func();
    if(p_print_fun != NULL) {
        p_print_fun->color_correction_reg(context_id, p_color_correction_reg);
    }

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((p_color_correction_reg != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        rval = p_ctx->organization.filter_methods.set_mono_color_correction_reg(context_id, p_color_correction_reg);
    } else {
        if(p_color_correction_reg == NULL) {
            amba_ik_system_print_str_5("[IK] NULL pointer in %s\n", __func__, DC_S, DC_S, DC_S, DC_S);
            rval = IK_ERR_0005;
        } else if(p_ctx->organization.initial_flag == 0U) {
            amba_ik_system_print_uint32_5("[IK] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
            rval = IK_ERR_0002;
        } else {
            amba_ik_system_print_uint32_5("[IK] can't get context\n", DC_U, DC_U, DC_U, DC_U, DC_U);
        }
    }
    return rval;
}

uint32 ik_get_mono_color_correction_reg(uint32 context_id, ik_color_correction_reg_t *p_color_correction_reg)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((p_color_correction_reg != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        rval = p_ctx->organization.filter_methods.get_mono_color_correction_reg(context_id, p_color_correction_reg);
    } else {
        if(p_color_correction_reg == NULL) {
            amba_ik_system_print_str_5("[IK] NULL pointer in %s\n", __func__, DC_S, DC_S, DC_S, DC_S);
            rval = IK_ERR_0005;
        } else if(p_ctx->organization.initial_flag == 0U) {
            amba_ik_system_print_uint32_5("[IK] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
            rval = IK_ERR_0002;
        } else {
            amba_ik_system_print_uint32_5("[IK] can't get context\n", DC_U, DC_U, DC_U, DC_U, DC_U);
        }
    }
    return rval;
}

uint32 ik_set_mono_color_correction(uint32 context_id, const ik_color_correction_t *p_color_correction)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;
    const amba_img_dsp_debug_print_func_t *p_print_fun = ik_get_debug_print_func();
    if(p_print_fun != NULL) {
        p_print_fun->color_correction(context_id, p_color_correction);
    }

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((p_color_correction != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        rval = p_ctx->organization.filter_methods.set_mono_color_correction(context_id, p_color_correction);
    } else {
        if(p_color_correction == NULL) {
            amba_ik_system_print_str_5("[IK] NULL pointer in %s\n", __func__, DC_S, DC_S, DC_S, DC_S);
            rval = IK_ERR_0005;
        } else if(p_ctx->organization.initial_flag == 0U) {
            amba_ik_system_print_uint32_5("[IK] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
            rval = IK_ERR_0002;
        } else {
            amba_ik_system_print_uint32_5("[IK] can't get context\n", DC_U, DC_U, DC_U, DC_U, DC_U);
        }
    }
    return rval;
}

uint32 ik_get_mono_color_correction(uint32 context_id, ik_color_correction_t *p_color_correction)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((p_color_correction != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        rval = p_ctx->organization.filter_methods.get_mono_color_correction(context_id, p_color_correction);
    } else {
        if(p_color_correction == NULL) {
            amba_ik_system_print_str_5("[IK] NULL pointer in %s\n", __func__, DC_S, DC_S, DC_S, DC_S);
            rval = IK_ERR_0005;
        } else if(p_ctx->organization.initial_flag == 0U) {
            amba_ik_system_print_uint32_5("[IK] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
            rval = IK_ERR_0002;
        } else {
            amba_ik_system_print_uint32_5("[IK] can't get context\n", DC_U, DC_U, DC_U, DC_U, DC_U);
        }
    }
    return rval;
}

uint32 ik_set_mono_tone_curve(uint32 context_id, const ik_tone_curve_t *p_tone_curve)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;
    const amba_img_dsp_debug_print_func_t *p_print_fun = ik_get_debug_print_func();
    if(p_print_fun != NULL) {
        p_print_fun->tone_curve(context_id, p_tone_curve);
    }

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((p_tone_curve != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        rval = p_ctx->organization.filter_methods.set_mono_tone_curve(context_id, p_tone_curve);
    } else {
        if(p_tone_curve == NULL) {
            amba_ik_system_print_str_5("[IK] NULL pointer in %s\n", __func__, DC_S, DC_S, DC_S, DC_S);
            rval = IK_ERR_0005;
        } else if(p_ctx->organization.initial_flag == 0U) {
            amba_ik_system_print_uint32_5("[IK] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
            rval = IK_ERR_0002;
        } else {
            amba_ik_system_print_uint32_5("[IK] can't get context\n", DC_U, DC_U, DC_U, DC_U, DC_U);
        }
    }
    return rval;
}

uint32 ik_get_mono_tone_curve(uint32 context_id, ik_tone_curve_t *p_tone_curve)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((p_tone_curve != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        rval = p_ctx->organization.filter_methods.get_mono_tone_curve(context_id, p_tone_curve);
    } else {
        if(p_tone_curve == NULL) {
            amba_ik_system_print_str_5("[IK] NULL pointer in %s\n", __func__, DC_S, DC_S, DC_S, DC_S);
            rval = IK_ERR_0005;
        } else if(p_ctx->organization.initial_flag == 0U) {
            amba_ik_system_print_uint32_5("[IK] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
            rval = IK_ERR_0002;
        } else {
            amba_ik_system_print_uint32_5("[IK] can't get context\n", DC_U, DC_U, DC_U, DC_U, DC_U);
        }
    }
    return rval;
}

uint32 ik_set_mono_rgb_to_yuv_matrix(uint32 context_id, const ik_rgb_to_yuv_matrix_t *p_rgb_to_yuv_matrix)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;
    const amba_img_dsp_debug_print_func_t *p_print_fun = ik_get_debug_print_func();
    if(p_print_fun != NULL) {
        p_print_fun->rgb_to_yuv_matrix(context_id, p_rgb_to_yuv_matrix);
    }

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((p_rgb_to_yuv_matrix != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        rval = p_ctx->organization.filter_methods.set_mono_rgb_to_yuv_matrix(context_id, p_rgb_to_yuv_matrix);
    } else {
        if(p_rgb_to_yuv_matrix == NULL) {
            amba_ik_system_print_str_5("[IK] NULL pointer in %s\n", __func__, DC_S, DC_S, DC_S, DC_S);
            rval = IK_ERR_0005;
        } else if(p_ctx->organization.initial_flag == 0U) {
            amba_ik_system_print_uint32_5("[IK] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
            rval = IK_ERR_0002;
        } else {
            amba_ik_system_print_uint32_5("[IK] can't get context\n", DC_U, DC_U, DC_U, DC_U, DC_U);
        }
    }
    return rval;
}

uint32 ik_get_mono_rgb_to_yuv_matrix(uint32 context_id, ik_rgb_to_yuv_matrix_t *p_rgb_to_yuv_matrix)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((p_rgb_to_yuv_matrix != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        rval = p_ctx->organization.filter_methods.get_mono_rgb_to_yuv_matrix(context_id, p_rgb_to_yuv_matrix);
    } else {
        if(p_rgb_to_yuv_matrix == NULL) {
            amba_ik_system_print_str_5("[IK] NULL pointer in %s\n", __func__, DC_S, DC_S, DC_S, DC_S);
            rval = IK_ERR_0005;
        } else if(p_ctx->organization.initial_flag == 0U) {
            amba_ik_system_print_uint32_5("[IK] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
            rval = IK_ERR_0002;
        } else {
            amba_ik_system_print_uint32_5("[IK] can't get context\n", DC_U, DC_U, DC_U, DC_U, DC_U);
        }
    }
    return rval;
}

uint32 ik_set_mono_fst_luma_process_mode(uint32 context_id, const ik_first_luma_process_mode_t *p_first_luma_process_mode)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;
    const amba_img_dsp_debug_print_func_t *p_print_fun = ik_get_debug_print_func();
    if(p_print_fun != NULL) {
        p_print_fun->first_luma_proc_mode(context_id, p_first_luma_process_mode);
    }

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((p_first_luma_process_mode != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        rval = p_ctx->organization.filter_methods.set_mono_first_luma_processing_mode(context_id, p_first_luma_process_mode);
    } else {
        if(p_first_luma_process_mode == NULL) {
            amba_ik_system_print_str_5("[IK] NULL pointer in %s\n", __func__, DC_S, DC_S, DC_S, DC_S);
            rval = IK_ERR_0005;
        } else if(p_ctx->organization.initial_flag == 0U) {
            amba_ik_system_print_uint32_5("[IK] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
            rval = IK_ERR_0002;
        } else {
            amba_ik_system_print_uint32_5("[IK] can't get context\n", DC_U, DC_U, DC_U, DC_U, DC_U);
        }
    }
    return rval;
}

uint32 ik_get_mono_fst_luma_process_mode(uint32 context_id, ik_first_luma_process_mode_t *p_first_luma_process_mode)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((p_first_luma_process_mode != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        rval = p_ctx->organization.filter_methods.get_mono_first_luma_processing_mode(context_id, p_first_luma_process_mode);
    } else {
        if(p_first_luma_process_mode == NULL) {
            amba_ik_system_print_str_5("[IK] NULL pointer in %s\n", __func__, DC_S, DC_S, DC_S, DC_S);
            rval = IK_ERR_0005;
        } else if(p_ctx->organization.initial_flag == 0U) {
            amba_ik_system_print_uint32_5("[IK] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
            rval = IK_ERR_0002;
        } else {
            amba_ik_system_print_uint32_5("[IK] can't get context\n", DC_U, DC_U, DC_U, DC_U, DC_U);
        }
    }
    return rval;
}

uint32 ik_set_mono_adv_spatial_filter(uint32 context_id, const ik_adv_spatial_filter_t *p_advance_spatial_filter)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;
    const amba_img_dsp_debug_print_func_t *p_print_fun = ik_get_debug_print_func();
    if(p_print_fun != NULL) {
        p_print_fun->adv_spat_fltr(context_id, p_advance_spatial_filter);
    }

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((p_advance_spatial_filter != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        rval = p_ctx->organization.filter_methods.set_mono_advance_spatial_filter(context_id, p_advance_spatial_filter);
    } else {
        if(p_advance_spatial_filter == NULL) {
            amba_ik_system_print_str_5("[IK] NULL pointer in %s\n", __func__, DC_S, DC_S, DC_S, DC_S);
            rval = IK_ERR_0005;
        } else if(p_ctx->organization.initial_flag == 0U) {
            amba_ik_system_print_uint32_5("[IK] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
            rval = IK_ERR_0002;
        } else {
            amba_ik_system_print_uint32_5("[IK] can't get context\n", DC_U, DC_U, DC_U, DC_U, DC_U);
        }
    }
    return rval;
}

uint32 ik_get_mono_adv_spatial_filter(uint32 context_id, ik_adv_spatial_filter_t *p_advance_spatial_filter)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((p_advance_spatial_filter != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        rval = p_ctx->organization.filter_methods.get_mono_advance_spatial_filter(context_id, p_advance_spatial_filter);
    } else {
        if(p_advance_spatial_filter == NULL) {
            amba_ik_system_print_str_5("[IK] NULL pointer in %s\n", __func__, DC_S, DC_S, DC_S, DC_S);
            rval = IK_ERR_0005;
        } else if(p_ctx->organization.initial_flag == 0U) {
            amba_ik_system_print_uint32_5("[IK] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
            rval = IK_ERR_0002;
        } else {
            amba_ik_system_print_uint32_5("[IK] can't get context\n", DC_U, DC_U, DC_U, DC_U, DC_U);
        }
    }
    return rval;
}

uint32 ik_set_mono_fst_shp_both(uint32 context_id, const ik_first_sharpen_both_t *p_first_sharpen_both)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;
    const amba_img_dsp_debug_print_func_t *p_print_fun = ik_get_debug_print_func();
    if(p_print_fun != NULL) {
        p_print_fun->first_sharpen_both(context_id, p_first_sharpen_both);
    }

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((p_first_sharpen_both != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        rval = p_ctx->organization.filter_methods.set_mono_fstshpns_both(context_id, p_first_sharpen_both);
    } else {
        if(p_first_sharpen_both == NULL) {
            amba_ik_system_print_str_5("[IK] NULL pointer in %s\n", __func__, DC_S, DC_S, DC_S, DC_S);
            rval = IK_ERR_0005;
        } else if(p_ctx->organization.initial_flag == 0U) {
            amba_ik_system_print_uint32_5("[IK] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
            rval = IK_ERR_0002;
        } else {
            amba_ik_system_print_uint32_5("[IK] can't get context\n", DC_U, DC_U, DC_U, DC_U, DC_U);
        }
    }
    return rval;
}

uint32 ik_get_mono_fst_shp_both(uint32 context_id, ik_first_sharpen_both_t *p_first_sharpen_both)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((p_first_sharpen_both != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        rval = p_ctx->organization.filter_methods.get_mono_fstshpns_both(context_id, p_first_sharpen_both);
    } else {
        if(p_first_sharpen_both == NULL) {
            amba_ik_system_print_str_5("[IK] NULL pointer in %s\n", __func__, DC_S, DC_S, DC_S, DC_S);
            rval = IK_ERR_0005;
        } else if(p_ctx->organization.initial_flag == 0U) {
            amba_ik_system_print_uint32_5("[IK] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
            rval = IK_ERR_0002;
        } else {
            amba_ik_system_print_uint32_5("[IK] can't get context\n", DC_U, DC_U, DC_U, DC_U, DC_U);
        }
    }
    return rval;
}

uint32 ik_set_mono_fst_shp_noise(uint32 context_id, const ik_first_sharpen_noise_t *p_first_sharpen_noise)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;
    const amba_img_dsp_debug_print_func_t *p_print_fun = ik_get_debug_print_func();
    if(p_print_fun != NULL) {
        p_print_fun->first_sharpen_noise(context_id, p_first_sharpen_noise);
    }

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((p_first_sharpen_noise != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        rval = p_ctx->organization.filter_methods.set_mono_fstshpns_noise(context_id, p_first_sharpen_noise);
    } else {
        if(p_first_sharpen_noise == NULL) {
            amba_ik_system_print_str_5("[IK] NULL pointer in %s\n", __func__, DC_S, DC_S, DC_S, DC_S);
            rval = IK_ERR_0005;
        } else if(p_ctx->organization.initial_flag == 0U) {
            amba_ik_system_print_uint32_5("[IK] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
            rval = IK_ERR_0002;
        } else {
            amba_ik_system_print_uint32_5("[IK] can't get context\n", DC_U, DC_U, DC_U, DC_U, DC_U);
        }
    }
    return rval;
}

uint32 ik_get_mono_fst_shp_noise(uint32 context_id, ik_first_sharpen_noise_t *p_first_sharpen_noise)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((p_first_sharpen_noise != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        rval = p_ctx->organization.filter_methods.get_mono_fstshpns_noise(context_id, p_first_sharpen_noise);
    } else {
        if(p_first_sharpen_noise == NULL) {
            amba_ik_system_print_str_5("[IK] NULL pointer in %s\n", __func__, DC_S, DC_S, DC_S, DC_S);
            rval = IK_ERR_0005;
        } else if(p_ctx->organization.initial_flag == 0U) {
            amba_ik_system_print_uint32_5("[IK] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
            rval = IK_ERR_0002;
        } else {
            amba_ik_system_print_uint32_5("[IK] can't get context\n", DC_U, DC_U, DC_U, DC_U, DC_U);
        }
    }
    return rval;
}

uint32 ik_set_mono_fst_shp_fir(uint32 context_id, const ik_first_sharpen_fir_t *p_first_sharpen_fir)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;
    const amba_img_dsp_debug_print_func_t *p_print_fun = ik_get_debug_print_func();
    if(p_print_fun != NULL) {
        p_print_fun->first_sharpen_fir(context_id, p_first_sharpen_fir);
    }

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((p_first_sharpen_fir != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        rval = p_ctx->organization.filter_methods.set_mono_fstshpns_fir(context_id, p_first_sharpen_fir);
    } else {
        if(p_first_sharpen_fir == NULL) {
            amba_ik_system_print_str_5("[IK] NULL pointer in %s\n", __func__, DC_S, DC_S, DC_S, DC_S);
            rval = IK_ERR_0005;
        } else if(p_ctx->organization.initial_flag == 0U) {
            amba_ik_system_print_uint32_5("[IK] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
            rval = IK_ERR_0002;
        } else {
            amba_ik_system_print_uint32_5("[IK] can't get context\n", DC_U, DC_U, DC_U, DC_U, DC_U);
        }
    }
    return rval;
}

uint32 ik_get_mono_fst_shp_fir(uint32 context_id, ik_first_sharpen_fir_t *p_first_sharpen_fir)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((p_first_sharpen_fir != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        rval = p_ctx->organization.filter_methods.get_mono_fstshpns_fir(context_id, p_first_sharpen_fir);
    } else {
        if(p_first_sharpen_fir == NULL) {
            amba_ik_system_print_str_5("[IK] NULL pointer in %s\n", __func__, DC_S, DC_S, DC_S, DC_S);
            rval = IK_ERR_0005;
        } else if(p_ctx->organization.initial_flag == 0U) {
            amba_ik_system_print_uint32_5("[IK] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
            rval = IK_ERR_0002;
        } else {
            amba_ik_system_print_uint32_5("[IK] can't get context\n", DC_U, DC_U, DC_U, DC_U, DC_U);
        }
    }
    return rval;
}

uint32 ik_set_mono_fst_shp_coring(uint32 context_id, const ik_first_sharpen_coring_t *p_first_sharpen_coring)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;
    const amba_img_dsp_debug_print_func_t *p_print_fun = ik_get_debug_print_func();
    if(p_print_fun != NULL) {
        p_print_fun->first_sharpen_coring(context_id, p_first_sharpen_coring);
    }

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((p_first_sharpen_coring != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        rval = p_ctx->organization.filter_methods.set_mono_fstshpns_coring(context_id, p_first_sharpen_coring);
    } else {
        if(p_first_sharpen_coring == NULL) {
            amba_ik_system_print_str_5("[IK] NULL pointer in %s\n", __func__, DC_S, DC_S, DC_S, DC_S);
            rval = IK_ERR_0005;
        } else if(p_ctx->organization.initial_flag == 0U) {
            amba_ik_system_print_uint32_5("[IK] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
            rval = IK_ERR_0002;
        } else {
            amba_ik_system_print_uint32_5("[IK] can't get context\n", DC_U, DC_U, DC_U, DC_U, DC_U);
        }
    }
    return rval;
}

uint32 ik_get_mono_fst_shp_coring(uint32 context_id, ik_first_sharpen_coring_t *p_first_sharpen_coring)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((p_first_sharpen_coring != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        rval = p_ctx->organization.filter_methods.get_mono_fstshpns_coring(context_id, p_first_sharpen_coring);
    } else {
        if(p_first_sharpen_coring == NULL) {
            amba_ik_system_print_str_5("[IK] NULL pointer in %s\n", __func__, DC_S, DC_S, DC_S, DC_S);
            rval = IK_ERR_0005;
        } else if(p_ctx->organization.initial_flag == 0U) {
            amba_ik_system_print_uint32_5("[IK] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
            rval = IK_ERR_0002;
        } else {
            amba_ik_system_print_uint32_5("[IK] can't get context\n", DC_U, DC_U, DC_U, DC_U, DC_U);
        }
    }
    return rval;
}

uint32 ik_set_mono_fst_shp_coring_idx_scale(uint32 context_id, const ik_first_sharpen_coring_idx_scale_t *p_first_sharpen_coring_idx_scale)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;
    const amba_img_dsp_debug_print_func_t *p_print_fun = ik_get_debug_print_func();
    if(p_print_fun != NULL) {
        p_print_fun->fstshpns_cor_idx_scl(context_id, p_first_sharpen_coring_idx_scale);
    }

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((p_first_sharpen_coring_idx_scale != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        rval = p_ctx->organization.filter_methods.set_mono_fstshpns_coring_index_scale(context_id, p_first_sharpen_coring_idx_scale);
    } else {
        if(p_first_sharpen_coring_idx_scale == NULL) {
            amba_ik_system_print_str_5("[IK] NULL pointer in %s\n", __func__, DC_S, DC_S, DC_S, DC_S);
            rval = IK_ERR_0005;
        } else if(p_ctx->organization.initial_flag == 0U) {
            amba_ik_system_print_uint32_5("[IK] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
            rval = IK_ERR_0002;
        } else {
            amba_ik_system_print_uint32_5("[IK] can't get context\n", DC_U, DC_U, DC_U, DC_U, DC_U);
        }
    }
    return rval;
}

uint32 ik_get_mono_fst_shp_coring_idx_scale(uint32 context_id, ik_first_sharpen_coring_idx_scale_t *p_first_sharpen_coring_idx_scale)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((p_first_sharpen_coring_idx_scale != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        rval = p_ctx->organization.filter_methods.get_mono_fstshpns_coring_index_scale(context_id, p_first_sharpen_coring_idx_scale);
    } else {
        if(p_first_sharpen_coring_idx_scale == NULL) {
            amba_ik_system_print_str_5("[IK] NULL pointer in %s\n", __func__, DC_S, DC_S, DC_S, DC_S);
            rval = IK_ERR_0005;
        } else if(p_ctx->organization.initial_flag == 0U) {
            amba_ik_system_print_uint32_5("[IK] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
            rval = IK_ERR_0002;
        } else {
            amba_ik_system_print_uint32_5("[IK] can't get context\n", DC_U, DC_U, DC_U, DC_U, DC_U);
        }
    }
    return rval;
}

uint32 ik_set_mono_fst_shp_min_coring_rslt(uint32 context_id, const ik_first_sharpen_min_coring_result_t *p_first_sharpen_min_coring_result)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;
    const amba_img_dsp_debug_print_func_t *p_print_fun = ik_get_debug_print_func();
    if(p_print_fun != NULL) {
        p_print_fun->fstshpns_min_cor_rst(context_id, p_first_sharpen_min_coring_result);
    }

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((p_first_sharpen_min_coring_result != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        rval = p_ctx->organization.filter_methods.set_mono_fstshpns_min_coring_result(context_id, p_first_sharpen_min_coring_result);
    } else {
        if(p_first_sharpen_min_coring_result == NULL) {
            amba_ik_system_print_str_5("[IK] NULL pointer in %s\n", __func__, DC_S, DC_S, DC_S, DC_S);
            rval = IK_ERR_0005;
        } else if(p_ctx->organization.initial_flag == 0U) {
            amba_ik_system_print_uint32_5("[IK] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
            rval = IK_ERR_0002;
        } else {
            amba_ik_system_print_uint32_5("[IK] can't get context\n", DC_U, DC_U, DC_U, DC_U, DC_U);
        }
    }
    return rval;
}

uint32 ik_get_mono_fst_shp_min_coring_rslt(uint32 context_id, ik_first_sharpen_min_coring_result_t *p_first_sharpen_min_coring_result)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((p_first_sharpen_min_coring_result != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        rval = p_ctx->organization.filter_methods.get_mono_fstshpns_min_coring_result(context_id, p_first_sharpen_min_coring_result);
    } else {
        if(p_first_sharpen_min_coring_result == NULL) {
            amba_ik_system_print_str_5("[IK] NULL pointer in %s\n", __func__, DC_S, DC_S, DC_S, DC_S);
            rval = IK_ERR_0005;
        } else if(p_ctx->organization.initial_flag == 0U) {
            amba_ik_system_print_uint32_5("[IK] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
            rval = IK_ERR_0002;
        } else {
            amba_ik_system_print_uint32_5("[IK] can't get context\n", DC_U, DC_U, DC_U, DC_U, DC_U);
        }
    }
    return rval;
}

uint32 ik_set_mono_fst_shp_max_coring_rslt(uint32 context_id, const ik_first_sharpen_max_coring_result_t *p_first_sharpen_max_coring_result)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;
    const amba_img_dsp_debug_print_func_t *p_print_fun = ik_get_debug_print_func();
    if(p_print_fun != NULL) {
        p_print_fun->fstshpns_max_cor_rst(context_id, p_first_sharpen_max_coring_result);
    }

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((p_first_sharpen_max_coring_result != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        rval = p_ctx->organization.filter_methods.set_mono_fstshpns_max_coring_result(context_id, p_first_sharpen_max_coring_result);
    } else {
        if(p_first_sharpen_max_coring_result == NULL) {
            amba_ik_system_print_str_5("[IK] NULL pointer in %s\n", __func__, DC_S, DC_S, DC_S, DC_S);
            rval = IK_ERR_0005;
        } else if(p_ctx->organization.initial_flag == 0U) {
            amba_ik_system_print_uint32_5("[IK] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
            rval = IK_ERR_0002;
        } else {
            amba_ik_system_print_uint32_5("[IK] can't get context\n", DC_U, DC_U, DC_U, DC_U, DC_U);
        }
    }
    return rval;
}

uint32 ik_get_mono_fst_shp_max_coring_rslt(uint32 context_id, ik_first_sharpen_max_coring_result_t *p_first_sharpen_max_coring_result)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((p_first_sharpen_max_coring_result != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        rval = p_ctx->organization.filter_methods.get_mono_fstshpns_max_coring_result(context_id, p_first_sharpen_max_coring_result);
    } else {
        if(p_first_sharpen_max_coring_result == NULL) {
            amba_ik_system_print_str_5("[IK] NULL pointer in %s\n", __func__, DC_S, DC_S, DC_S, DC_S);
            rval = IK_ERR_0005;
        } else if(p_ctx->organization.initial_flag == 0U) {
            amba_ik_system_print_uint32_5("[IK] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
            rval = IK_ERR_0002;
        } else {
            amba_ik_system_print_uint32_5("[IK] can't get context\n", DC_U, DC_U, DC_U, DC_U, DC_U);
        }
    }
    return rval;
}

uint32 ik_set_mono_fst_shp_scale_coring(uint32 context_id, const ik_first_sharpen_scale_coring_t *p_first_sharpen_scale_coring)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;
    const amba_img_dsp_debug_print_func_t *p_print_fun = ik_get_debug_print_func();
    if(p_print_fun != NULL) {
        p_print_fun->fstshpns_scl_cor(context_id, p_first_sharpen_scale_coring);
    }

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((p_first_sharpen_scale_coring != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        rval = p_ctx->organization.filter_methods.set_mono_fstshpns_scale_coring(context_id, p_first_sharpen_scale_coring);
    } else {
        if(p_first_sharpen_scale_coring == NULL) {
            amba_ik_system_print_str_5("[IK] NULL pointer in %s\n", __func__, DC_S, DC_S, DC_S, DC_S);
            rval = IK_ERR_0005;
        } else if(p_ctx->organization.initial_flag == 0U) {
            amba_ik_system_print_uint32_5("[IK] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
            rval = IK_ERR_0002;
        } else {
            amba_ik_system_print_uint32_5("[IK] can't get context\n", DC_U, DC_U, DC_U, DC_U, DC_U);
        }
    }
    return rval;
}

uint32 ik_get_mono_fst_shp_scale_coring(uint32 context_id, ik_first_sharpen_scale_coring_t *p_first_sharpen_scale_coring)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((p_first_sharpen_scale_coring != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        rval = p_ctx->organization.filter_methods.get_mono_fstshpns_scale_coring(context_id, p_first_sharpen_scale_coring);
    } else {
        if(p_first_sharpen_scale_coring == NULL) {
            amba_ik_system_print_str_5("[IK] NULL pointer in %s\n", __func__, DC_S, DC_S, DC_S, DC_S);
            rval = IK_ERR_0005;
        } else if(p_ctx->organization.initial_flag == 0U) {
            amba_ik_system_print_uint32_5("[IK] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
            rval = IK_ERR_0002;
        } else {
            amba_ik_system_print_uint32_5("[IK] can't get context\n", DC_U, DC_U, DC_U, DC_U, DC_U);
        }
    }
    return rval;
}

uint32 ik_set_wide_chroma_filter(uint32 context_id, const ik_chroma_filter_t *p_chroma_filter)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;
    const amba_img_dsp_debug_print_func_t *p_print_fun = ik_get_debug_print_func();
    if(p_print_fun != NULL) {
        p_print_fun->chroma_filter(context_id, p_chroma_filter);
    }

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((p_chroma_filter != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        rval = p_ctx->organization.filter_methods.set_wide_chroma_filter(context_id, p_chroma_filter);
    } else {
        if(p_chroma_filter == NULL) {
            amba_ik_system_print_str_5("[IK] NULL pointer in %s\n", __func__, DC_S, DC_S, DC_S, DC_S);
            rval = IK_ERR_0005;
        } else if(p_ctx->organization.initial_flag == 0U) {
            amba_ik_system_print_uint32_5("[IK] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
            rval = IK_ERR_0002;
        } else {
            amba_ik_system_print_uint32_5("[IK] can't get context\n", DC_U, DC_U, DC_U, DC_U, DC_U);
        }
    }
    return rval;
}

uint32 ik_get_wide_chroma_filter(uint32 context_id, ik_chroma_filter_t *p_chroma_filter)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((p_chroma_filter != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        rval = p_ctx->organization.filter_methods.get_wide_chroma_filter(context_id, p_chroma_filter);
    } else {
        if(p_chroma_filter == NULL) {
            amba_ik_system_print_str_5("[IK] NULL pointer in %s\n", __func__, DC_S, DC_S, DC_S, DC_S);
            rval = IK_ERR_0005;
        } else if(p_ctx->organization.initial_flag == 0U) {
            amba_ik_system_print_uint32_5("[IK] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
            rval = IK_ERR_0002;
        } else {
            amba_ik_system_print_uint32_5("[IK] can't get context\n", DC_U, DC_U, DC_U, DC_U, DC_U);
        }
    }
    return rval;
}

uint32 ik_set_wide_chroma_filter_combine(uint32 context_id, const ik_wide_chroma_filter_combine_t *p_chroma_filter_combine)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;
//    const amba_img_dsp_debug_print_func_t *p_print_fun = ik_get_debug_print_func();
//    if(p_print_fun != NULL){
//        p_print_fun->chroma_filter(context_id, p_chroma_filter_combine);
//    }

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((p_chroma_filter_combine != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        rval = p_ctx->organization.filter_methods.set_wide_chroma_combine(context_id, p_chroma_filter_combine);
    } else {
        if(p_chroma_filter_combine == NULL) {
            amba_ik_system_print_str_5("[IK] NULL pointer in %s\n", __func__, DC_S, DC_S, DC_S, DC_S);
            rval = IK_ERR_0005;
        } else if(p_ctx->organization.initial_flag == 0U) {
            amba_ik_system_print_uint32_5("[IK] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
            rval = IK_ERR_0002;
        } else {
            amba_ik_system_print_uint32_5("[IK] can't get context\n", DC_U, DC_U, DC_U, DC_U, DC_U);
        }
    }
    return rval;
}

uint32 ik_get_wide_chroma_filter_combine(uint32 context_id, ik_wide_chroma_filter_combine_t *p_chroma_filter_combine)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((p_chroma_filter_combine != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        rval = p_ctx->organization.filter_methods.get_wide_chroma_combine(context_id, p_chroma_filter_combine);
    } else {
        if(p_chroma_filter_combine == NULL) {
            amba_ik_system_print_str_5("[IK] NULL pointer in %s\n", __func__, DC_S, DC_S, DC_S, DC_S);
            rval = IK_ERR_0005;
        } else if(p_ctx->organization.initial_flag == 0U) {
            amba_ik_system_print_uint32_5("[IK] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
            rval = IK_ERR_0002;
        } else {
            amba_ik_system_print_uint32_5("[IK] can't get context\n", DC_U, DC_U, DC_U, DC_U, DC_U);
        }
    }
    return rval;
}

uint32 ik_set_mono_exp0_frontend_static_blc(uint32 context_id, const ik_static_blc_level_t *p_exp0_frontend_static_blc)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;
    const amba_img_dsp_debug_print_func_t *p_print_fun = ik_get_debug_print_func();
    if(p_print_fun != NULL) {
        p_print_fun->fe_blc(context_id, p_exp0_frontend_static_blc, 0U);
    }

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((p_exp0_frontend_static_blc != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        rval = p_ctx->organization.filter_methods.set_mono_exp0_fe_static_blc(context_id, p_exp0_frontend_static_blc);
    } else {
        if(p_exp0_frontend_static_blc == NULL) {
            amba_ik_system_print_str_5("[IK] NULL pointer in %s\n", __func__, DC_S, DC_S, DC_S, DC_S);
            rval = IK_ERR_0005;
        } else if(p_ctx->organization.initial_flag == 0U) {
            amba_ik_system_print_uint32_5("[IK] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
            rval = IK_ERR_0002;
        } else {
            amba_ik_system_print_uint32_5("[IK] can't get context\n", DC_U, DC_U, DC_U, DC_U, DC_U);
        }
    }
    return rval;
}

uint32 ik_get_mono_exp0_frontend_static_blc(uint32 context_id, ik_static_blc_level_t *p_exp0_frontend_static_blc)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((p_exp0_frontend_static_blc != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        rval = p_ctx->organization.filter_methods.get_mono_exp0_fe_static_blc(context_id, p_exp0_frontend_static_blc);
    } else {
        if(p_exp0_frontend_static_blc == NULL) {
            amba_ik_system_print_str_5("[IK] NULL pointer in %s\n", __func__, DC_S, DC_S, DC_S, DC_S);
            rval = IK_ERR_0005;
        } else if(p_ctx->organization.initial_flag == 0U) {
            amba_ik_system_print_uint32_5("[IK] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
            rval = IK_ERR_0002;
        } else {
            amba_ik_system_print_uint32_5("[IK] can't get context\n", DC_U, DC_U, DC_U, DC_U, DC_U);
        }
    }
    return rval;
}

uint32 ik_set_mono_exp0_frontend_wb_gain(uint32 context_id, const ik_frontend_wb_gain_t *p_exp0_frontend_wb_gain)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;
    const amba_img_dsp_debug_print_func_t *p_print_fun = ik_get_debug_print_func();
    if(p_print_fun != NULL) {
        p_print_fun->fe_wb(context_id, p_exp0_frontend_wb_gain, 0);
    }

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((p_exp0_frontend_wb_gain != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        rval = p_ctx->organization.filter_methods.set_mono_exp0_fe_wb_gain(context_id, p_exp0_frontend_wb_gain);
    } else {
        if(p_exp0_frontend_wb_gain == NULL) {
            amba_ik_system_print_str_5("[IK] NULL pointer in %s\n", __func__, DC_S, DC_S, DC_S, DC_S);
            rval = IK_ERR_0005;
        } else if(p_ctx->organization.initial_flag == 0U) {
            amba_ik_system_print_uint32_5("[IK] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
            rval = IK_ERR_0002;
        } else {
            amba_ik_system_print_uint32_5("[IK] can't get context\n", DC_U, DC_U, DC_U, DC_U, DC_U);
        }
    }
    return rval;
}

uint32 ik_get_mono_exp0_frontend_wb_gain(uint32 context_id, ik_frontend_wb_gain_t *p_exp0_frontend_wb_gain)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((p_exp0_frontend_wb_gain != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        rval = p_ctx->organization.filter_methods.get_mono_exp0_fe_wb_gain(context_id, p_exp0_frontend_wb_gain);
    } else {
        if(p_exp0_frontend_wb_gain == NULL) {
            amba_ik_system_print_str_5("[IK] NULL pointer in %s\n", __func__, DC_S, DC_S, DC_S, DC_S);
            rval = IK_ERR_0005;
        } else if(p_ctx->organization.initial_flag == 0U) {
            amba_ik_system_print_uint32_5("[IK] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
            rval = IK_ERR_0002;
        } else {
            amba_ik_system_print_uint32_5("[IK] can't get context\n", DC_U, DC_U, DC_U, DC_U, DC_U);
        }
    }
    return rval;
}

uint32 ik_set_mono_ce(uint32 context_id, const ik_ce_t *p_ce)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;
    const amba_img_dsp_debug_print_func_t *p_print_fun = ik_get_debug_print_func();
    if(p_print_fun != NULL) {
        p_print_fun->ce(context_id, p_ce);
    }

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((p_ce != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        rval = p_ctx->organization.filter_methods.set_mono_ce(context_id, p_ce);
    } else {
        if(p_ce == NULL) {
            amba_ik_system_print_str_5("[IK] NULL pointer in %s\n", __func__, DC_S, DC_S, DC_S, DC_S);
            rval = IK_ERR_0005;
        } else if(p_ctx->organization.initial_flag == 0U) {
            amba_ik_system_print_uint32_5("[IK] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
            rval = IK_ERR_0002;
        } else {
            amba_ik_system_print_uint32_5("[IK] can't get context\n", DC_U, DC_U, DC_U, DC_U, DC_U);
        }
    }
    return rval;
}

uint32 ik_get_mono_ce(uint32 context_id, ik_ce_t *p_ce)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((p_ce != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        rval = p_ctx->organization.filter_methods.get_mono_ce(context_id, p_ce);
    } else {
        if(p_ce == NULL) {
            amba_ik_system_print_str_5("[IK] NULL pointer in %s\n", __func__, DC_S, DC_S, DC_S, DC_S);
            rval = IK_ERR_0005;
        } else if(p_ctx->organization.initial_flag == 0U) {
            amba_ik_system_print_uint32_5("[IK] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
            rval = IK_ERR_0002;
        } else {
            amba_ik_system_print_uint32_5("[IK] can't get context\n", DC_U, DC_U, DC_U, DC_U, DC_U);
        }
    }
    return rval;
}

uint32 ik_set_mono_ce_input_table(uint32 context_id, const ik_ce_input_table_t *p_ce_input_table)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;
    const amba_img_dsp_debug_print_func_t *p_print_fun = ik_get_debug_print_func();
    if(p_print_fun != NULL) {
        p_print_fun->ce_input_tbl(context_id, p_ce_input_table);
    }

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((p_ce_input_table != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        rval = p_ctx->organization.filter_methods.set_mono_ce_input_table(context_id, p_ce_input_table);
    } else {
        if(p_ce_input_table == NULL) {
            amba_ik_system_print_str_5("[IK] NULL pointer in %s\n", __func__, DC_S, DC_S, DC_S, DC_S);
            rval = IK_ERR_0005;
        } else if(p_ctx->organization.initial_flag == 0U) {
            amba_ik_system_print_uint32_5("[IK] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
            rval = IK_ERR_0002;
        } else {
            amba_ik_system_print_uint32_5("[IK] can't get context\n", DC_U, DC_U, DC_U, DC_U, DC_U);
        }
    }
    return rval;
}

uint32 ik_get_mono_ce_input_table(uint32 context_id, ik_ce_input_table_t *p_ce_input_table)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((p_ce_input_table != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        rval = p_ctx->organization.filter_methods.get_mono_ce_input_table(context_id, p_ce_input_table);
    } else {
        if(p_ce_input_table == NULL) {
            amba_ik_system_print_str_5("[IK] NULL pointer in %s\n", __func__, DC_S, DC_S, DC_S, DC_S);
            rval = IK_ERR_0005;
        } else if(p_ctx->organization.initial_flag == 0U) {
            amba_ik_system_print_uint32_5("[IK] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
            rval = IK_ERR_0002;
        } else {
            amba_ik_system_print_uint32_5("[IK] can't get context\n", DC_U, DC_U, DC_U, DC_U, DC_U);
        }
    }
    return rval;
}


uint32 ik_set_mono_ce_out_table(uint32 context_id, const ik_ce_output_table_t *p_ce_out_table)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;
    const amba_img_dsp_debug_print_func_t *p_print_fun = ik_get_debug_print_func();
    if(p_print_fun != NULL) {
        p_print_fun->ce_output_tbl(context_id, p_ce_out_table);
    }

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((p_ce_out_table != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        rval = p_ctx->organization.filter_methods.set_mono_ce_out_table(context_id, p_ce_out_table);
    } else {
        if(p_ce_out_table == NULL) {
            amba_ik_system_print_str_5("[IK] NULL pointer in %s\n", __func__, DC_S, DC_S, DC_S, DC_S);
            rval = IK_ERR_0005;
        } else if(p_ctx->organization.initial_flag == 0U) {
            amba_ik_system_print_uint32_5("[IK] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
            rval = IK_ERR_0002;
        } else {
            amba_ik_system_print_uint32_5("[IK] can't get context\n", DC_U, DC_U, DC_U, DC_U, DC_U);
        }
    }
    return rval;
}

uint32 ik_get_mono_ce_out_table(uint32 context_id, ik_ce_output_table_t *p_ce_out_table)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((p_ce_out_table != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        rval = p_ctx->organization.filter_methods.get_mono_ce_out_table(context_id, p_ce_out_table);
    } else {
        if(p_ce_out_table == NULL) {
            amba_ik_system_print_str_5("[IK] NULL pointer in %s\n", __func__, DC_S, DC_S, DC_S, DC_S);
            rval = IK_ERR_0005;
        } else if(p_ctx->organization.initial_flag == 0U) {
            amba_ik_system_print_uint32_5("[IK] context_id = %d is not initial\n", context_id, DC_U, DC_U, DC_U, DC_U);
            rval = IK_ERR_0002;
        } else {
            amba_ik_system_print_uint32_5("[IK] can't get context\n", DC_U, DC_U, DC_U, DC_U, DC_U);
        }
    }
    return rval;
}

#endif
