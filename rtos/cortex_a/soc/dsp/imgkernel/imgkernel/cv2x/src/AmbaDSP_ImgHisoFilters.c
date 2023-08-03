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

uint32 ik_set_hi_anti_aliasing(uint32 context_id, const ik_hi_anti_aliasing_t *p_hi_anti_aliasing)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;
    const amba_img_dsp_debug_print_func_t *p_print_fun = ik_get_debug_print_func();
    if(p_print_fun != NULL) {
        //p_print_fun->hi_anti_aliasing(context_id, p_hi_anti_aliasing);
    }

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((p_hi_anti_aliasing != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        rval = p_ctx->organization.filter_methods.set_hi_anti_aliasing(context_id, p_hi_anti_aliasing);
    } else {
        if(p_hi_anti_aliasing == NULL) {
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

uint32 ik_get_hi_anti_aliasing(uint32 context_id, ik_hi_anti_aliasing_t *p_hi_anti_aliasing)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((p_hi_anti_aliasing != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        rval = p_ctx->organization.filter_methods.get_hi_anti_aliasing(context_id, p_hi_anti_aliasing);
    } else {
        if(p_hi_anti_aliasing == NULL) {
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

uint32 ik_set_hi_cfa_leakage_filter(uint32 context_id, const ik_hi_cfa_leakage_filter_t *p_hi_cfa_leakage_filter)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;
    const amba_img_dsp_debug_print_func_t *p_print_fun = ik_get_debug_print_func();
    if(p_print_fun != NULL) {
        //p_print_fun->hi_cfa_leakage_filter(context_id, p_hi_cfa_leakage_filter);
    }

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((p_hi_cfa_leakage_filter != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        rval = p_ctx->organization.filter_methods.set_hi_cfa_leakage_filter(context_id, p_hi_cfa_leakage_filter);
    } else {
        if(p_hi_cfa_leakage_filter == NULL) {
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

uint32 ik_get_hi_cfa_leakage_filter(uint32 context_id, ik_hi_cfa_leakage_filter_t *p_hi_cfa_leakage_filter)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((p_hi_cfa_leakage_filter != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        rval = p_ctx->organization.filter_methods.get_hi_cfa_leakage_filter(context_id, p_hi_cfa_leakage_filter);
    } else {
        if(p_hi_cfa_leakage_filter == NULL) {
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

uint32 ik_set_hi_dynamic_bpc(uint32 context_id, const ik_hi_dynamic_bpc_t *p_hi_dynamic_bpc)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;
    const amba_img_dsp_debug_print_func_t *p_print_fun = ik_get_debug_print_func();
    if(p_print_fun != NULL) {
        //p_print_fun->hi_dynamic_bpc(context_id, p_hi_dynamic_bpc);
    }

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((p_hi_dynamic_bpc != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        rval = p_ctx->organization.filter_methods.set_hi_dynamic_bpc(context_id, p_hi_dynamic_bpc);
    } else {
        if(p_hi_dynamic_bpc == NULL) {
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

uint32 ik_get_hi_dynamic_bpc(uint32 context_id, ik_hi_dynamic_bpc_t *p_hi_dynamic_bpc)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((p_hi_dynamic_bpc != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        rval = p_ctx->organization.filter_methods.get_hi_dynamic_bpc(context_id, p_hi_dynamic_bpc);
    } else {
        if(p_hi_dynamic_bpc == NULL) {
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

uint32 ik_set_hi_grgb_mismatch(uint32 context_id, const ik_hi_grgb_mismatch_t *p_hi_grgb_mismatch)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;
    const amba_img_dsp_debug_print_func_t *p_print_fun = ik_get_debug_print_func();
    if(p_print_fun != NULL) {
        //p_print_fun->hi_grgb_mismatch(context_id, p_hi_grgb_mismatch);
    }

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((p_hi_grgb_mismatch != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        rval = p_ctx->organization.filter_methods.set_hi_grgb_mismatch(context_id, p_hi_grgb_mismatch);
    } else {
        if(p_hi_grgb_mismatch == NULL) {
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

uint32 ik_get_hi_grgb_mismatch(uint32 context_id, ik_hi_grgb_mismatch_t *p_hi_grgb_mismatch)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((p_hi_grgb_mismatch != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        rval = p_ctx->organization.filter_methods.get_hi_grgb_mismatch(context_id, p_hi_grgb_mismatch);
    } else {
        if(p_hi_grgb_mismatch == NULL) {
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

uint32 ik_set_hi_chroma_median_filter(uint32 context_id, const ik_hi_chroma_median_filter_t *p_hi_chroma_median_filter)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;
    const amba_img_dsp_debug_print_func_t *p_print_fun = ik_get_debug_print_func();
    if(p_print_fun != NULL) {
        //p_print_fun->hi_chroma_median_filter(context_id, p_hi_chroma_median_filter);
    }

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((p_hi_chroma_median_filter != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        rval = p_ctx->organization.filter_methods.set_hi_chroma_median_filter(context_id, p_hi_chroma_median_filter);
    } else {
        if(p_hi_chroma_median_filter == NULL) {
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

uint32 ik_get_hi_chroma_median_filter(uint32 context_id, ik_hi_chroma_median_filter_t *p_hi_chroma_median_filter)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((p_hi_chroma_median_filter != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        rval = p_ctx->organization.filter_methods.get_hi_chroma_median_filter(context_id, p_hi_chroma_median_filter);
    } else {
        if(p_hi_chroma_median_filter == NULL) {
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

uint32 ik_set_hi_cfa_noise_filter(uint32 context_id, const ik_hi_cfa_noise_filter_t *p_hi_cfa_noise_filter)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;
    const amba_img_dsp_debug_print_func_t *p_print_fun = ik_get_debug_print_func();
    if(p_print_fun != NULL) {
        //p_print_fun->hi_cfa_noise_filter(context_id, p_hi_cfa_noise_filter);
    }

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((p_hi_cfa_noise_filter != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        rval = p_ctx->organization.filter_methods.set_hi_cfa_noise_filter(context_id, p_hi_cfa_noise_filter);
    } else {
        if(p_hi_cfa_noise_filter == NULL) {
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

uint32 ik_get_hi_cfa_noise_filter(uint32 context_id, ik_hi_cfa_noise_filter_t *p_hi_cfa_noise_filter)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((p_hi_cfa_noise_filter != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        rval = p_ctx->organization.filter_methods.get_hi_cfa_noise_filter(context_id, p_hi_cfa_noise_filter);
    } else {
        if(p_hi_cfa_noise_filter == NULL) {
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

uint32 ik_set_hi_demosaic(uint32 context_id, const ik_hi_demosaic_t *p_hi_demosaic)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;
    const amba_img_dsp_debug_print_func_t *p_print_fun = ik_get_debug_print_func();
    if(p_print_fun != NULL) {
        //p_print_fun->hi_demosaic(context_id, p_hi_demosaic);
    }

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((p_hi_demosaic != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        rval = p_ctx->organization.filter_methods.set_hi_demosaic(context_id, p_hi_demosaic);
    } else {
        if(p_hi_demosaic == NULL) {
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

uint32 ik_get_hi_demosaic(uint32 context_id, ik_hi_demosaic_t *p_hi_demosaic)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((p_hi_demosaic != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        rval = p_ctx->organization.filter_methods.get_hi_demosaic(context_id, p_hi_demosaic);
    } else {
        if(p_hi_demosaic == NULL) {
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

uint32 ik_set_li2_anti_aliasing(uint32 context_id, const ik_li2_anti_aliasing_t *p_li2_anti_aliasing)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;
    const amba_img_dsp_debug_print_func_t *p_print_fun = ik_get_debug_print_func();
    if(p_print_fun != NULL) {
        //p_print_fun->li2_anti_aliasing(context_id, p_li2_anti_aliasing);
    }

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((p_li2_anti_aliasing != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        rval = p_ctx->organization.filter_methods.set_li2_anti_aliasing(context_id, p_li2_anti_aliasing);
    } else {
        if(p_li2_anti_aliasing == NULL) {
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

uint32 ik_get_li2_anti_aliasing(uint32 context_id, ik_li2_anti_aliasing_t *p_li2_anti_aliasing)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((p_li2_anti_aliasing != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        rval = p_ctx->organization.filter_methods.get_li2_anti_aliasing(context_id, p_li2_anti_aliasing);
    } else {
        if(p_li2_anti_aliasing == NULL) {
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

uint32 ik_set_li2_cfa_leakage_filter(uint32 context_id, const ik_li2_cfa_leakage_filter_t *p_li2_cfa_leakage_filter)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;
    const amba_img_dsp_debug_print_func_t *p_print_fun = ik_get_debug_print_func();
    if(p_print_fun != NULL) {
        //p_print_fun->li2_cfa_leakage_filter(context_id, p_li2_cfa_leakage_filter);
    }

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((p_li2_cfa_leakage_filter != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        rval = p_ctx->organization.filter_methods.set_li2_cfa_leakage_filter(context_id, p_li2_cfa_leakage_filter);
    } else {
        if(p_li2_cfa_leakage_filter == NULL) {
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

uint32 ik_get_li2_cfa_leakage_filter(uint32 context_id, ik_li2_cfa_leakage_filter_t *p_li2_cfa_leakage_filter)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((p_li2_cfa_leakage_filter != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        rval = p_ctx->organization.filter_methods.get_li2_cfa_leakage_filter(context_id, p_li2_cfa_leakage_filter);
    } else {
        if(p_li2_cfa_leakage_filter == NULL) {
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

uint32 ik_set_li2_dynamic_bpc(uint32 context_id, const ik_li2_dynamic_bpc_t *p_li2_dynamic_bpc)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;
    const amba_img_dsp_debug_print_func_t *p_print_fun = ik_get_debug_print_func();
    if(p_print_fun != NULL) {
        //p_print_fun->li2_dynamic_bpc(context_id, p_li2_dynamic_bpc);
    }

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((p_li2_dynamic_bpc != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        rval = p_ctx->organization.filter_methods.set_li2_dynamic_bpc(context_id, p_li2_dynamic_bpc);
    } else {
        if(p_li2_dynamic_bpc == NULL) {
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

uint32 ik_get_li2_dynamic_bpc(uint32 context_id, ik_li2_dynamic_bpc_t *p_li2_dynamic_bpc)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((p_li2_dynamic_bpc != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        rval = p_ctx->organization.filter_methods.get_li2_dynamic_bpc(context_id, p_li2_dynamic_bpc);
    } else {
        if(p_li2_dynamic_bpc == NULL) {
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

uint32 ik_set_li2_grgb_mismatch(uint32 context_id, const ik_li2_grgb_mismatch_t *p_li2_grgb_mismatch)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;
    const amba_img_dsp_debug_print_func_t *p_print_fun = ik_get_debug_print_func();
    if(p_print_fun != NULL) {
        //p_print_fun->li2_grgb_mismatch(context_id, p_li2_grgb_mismatch);
    }

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((p_li2_grgb_mismatch != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        rval = p_ctx->organization.filter_methods.set_li2_grgb_mismatch(context_id, p_li2_grgb_mismatch);
    } else {
        if(p_li2_grgb_mismatch == NULL) {
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

uint32 ik_get_li2_grgb_mismatch(uint32 context_id, ik_li2_grgb_mismatch_t *p_li2_grgb_mismatch)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((p_li2_grgb_mismatch != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        rval = p_ctx->organization.filter_methods.get_li2_grgb_mismatch(context_id, p_li2_grgb_mismatch);
    } else {
        if(p_li2_grgb_mismatch == NULL) {
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

uint32 ik_set_li2_cfa_noise_filter(uint32 context_id, const ik_li2_cfa_noise_filter_t *p_li2_cfa_noise_filter)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;
    const amba_img_dsp_debug_print_func_t *p_print_fun = ik_get_debug_print_func();
    if(p_print_fun != NULL) {
        //p_print_fun->li2_cfa_noise_filter(context_id, p_li2_cfa_noise_filter);
    }

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((p_li2_cfa_noise_filter != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        rval = p_ctx->organization.filter_methods.set_li2_cfa_noise_filter(context_id, p_li2_cfa_noise_filter);
    } else {
        if(p_li2_cfa_noise_filter == NULL) {
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

uint32 ik_get_li2_cfa_noise_filter(uint32 context_id, ik_li2_cfa_noise_filter_t *p_li2_cfa_noise_filter)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((p_li2_cfa_noise_filter != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        rval = p_ctx->organization.filter_methods.get_li2_cfa_noise_filter(context_id, p_li2_cfa_noise_filter);
    } else {
        if(p_li2_cfa_noise_filter == NULL) {
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

uint32 ik_set_li2_demosaic(uint32 context_id, const ik_li2_demosaic_t *p_li2_demosaic)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;
    const amba_img_dsp_debug_print_func_t *p_print_fun = ik_get_debug_print_func();
    if(p_print_fun != NULL) {
        //p_print_fun->li2_demosaic(context_id, p_li2_demosaic);
    }

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((p_li2_demosaic != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        rval = p_ctx->organization.filter_methods.set_li2_demosaic(context_id, p_li2_demosaic);
    } else {
        if(p_li2_demosaic == NULL) {
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

uint32 ik_get_li2_demosaic(uint32 context_id, ik_li2_demosaic_t *p_li2_demosaic)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((p_li2_demosaic != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        rval = p_ctx->organization.filter_methods.get_li2_demosaic(context_id, p_li2_demosaic);
    } else {
        if(p_li2_demosaic == NULL) {
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

uint32 ik_set_hi_asf(uint32 context_id, const ik_hi_asf_t *p_hi_asf)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;
    const amba_img_dsp_debug_print_func_t *p_print_fun = ik_get_debug_print_func();
    if(p_print_fun != NULL) {
        //p_print_fun->hi_asf(context_id, p_hi_asf);
    }

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((p_hi_asf != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        rval = p_ctx->organization.filter_methods.set_hi_asf(context_id, p_hi_asf);
    } else {
        if(p_hi_asf == NULL) {
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

uint32 ik_get_hi_asf(uint32 context_id, ik_hi_asf_t *p_hi_asf)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((p_hi_asf != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        rval = p_ctx->organization.filter_methods.get_hi_asf(context_id, p_hi_asf);
    } else {
        if(p_hi_asf == NULL) {
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

uint32 ik_set_li2_asf(uint32 context_id, const ik_li2_asf_t *p_li2_asf)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;
    const amba_img_dsp_debug_print_func_t *p_print_fun = ik_get_debug_print_func();
    if(p_print_fun != NULL) {
        //p_print_fun->li2_asf(context_id, p_li2_asf);
    }

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((p_li2_asf != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        rval = p_ctx->organization.filter_methods.set_li2_asf(context_id, p_li2_asf);
    } else {
        if(p_li2_asf == NULL) {
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

uint32 ik_get_li2_asf(uint32 context_id, ik_li2_asf_t *p_li2_asf)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((p_li2_asf != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        rval = p_ctx->organization.filter_methods.get_li2_asf(context_id, p_li2_asf);
    } else {
        if(p_li2_asf == NULL) {
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

uint32 ik_set_hi_low_asf(uint32 context_id, const ik_hi_low_asf_t *p_hi_low_asf)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;
    const amba_img_dsp_debug_print_func_t *p_print_fun = ik_get_debug_print_func();
    if(p_print_fun != NULL) {
        //p_print_fun->hi_low_asf(context_id, p_hi_low_asf);
    }

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((p_hi_low_asf != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        rval = p_ctx->organization.filter_methods.set_hi_low_asf(context_id, p_hi_low_asf);
    } else {
        if(p_hi_low_asf == NULL) {
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

uint32 ik_get_hi_low_asf(uint32 context_id, ik_hi_low_asf_t *p_hi_low_asf)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((p_hi_low_asf != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        rval = p_ctx->organization.filter_methods.get_hi_low_asf(context_id, p_hi_low_asf);
    } else {
        if(p_hi_low_asf == NULL) {
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

uint32 ik_set_hi_med1_asf(uint32 context_id, const ik_hi_med1_asf_t *p_hi_med1_asf)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;
    const amba_img_dsp_debug_print_func_t *p_print_fun = ik_get_debug_print_func();
    if(p_print_fun != NULL) {
        //p_print_fun->hi_med1_asf(context_id, p_hi_med1_asf);
    }

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((p_hi_med1_asf != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        rval = p_ctx->organization.filter_methods.set_hi_med1_asf(context_id, p_hi_med1_asf);
    } else {
        if(p_hi_med1_asf == NULL) {
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

uint32 ik_get_hi_med1_asf(uint32 context_id, ik_hi_med1_asf_t *p_hi_med1_asf)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((p_hi_med1_asf != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        rval = p_ctx->organization.filter_methods.get_hi_med1_asf(context_id, p_hi_med1_asf);
    } else {
        if(p_hi_med1_asf == NULL) {
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

uint32 ik_set_hi_med2_asf(uint32 context_id, const ik_hi_med2_asf_t *p_hi_med2_asf)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;
    const amba_img_dsp_debug_print_func_t *p_print_fun = ik_get_debug_print_func();
    if(p_print_fun != NULL) {
        //p_print_fun->hi_med2_asf(context_id, p_hi_med2_asf);
    }

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((p_hi_med2_asf != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        rval = p_ctx->organization.filter_methods.set_hi_med2_asf(context_id, p_hi_med2_asf);
    } else {
        if(p_hi_med2_asf == NULL) {
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

uint32 ik_get_hi_med2_asf(uint32 context_id, ik_hi_med2_asf_t *p_hi_med2_asf)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((p_hi_med2_asf != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        rval = p_ctx->organization.filter_methods.get_hi_med2_asf(context_id, p_hi_med2_asf);
    } else {
        if(p_hi_med2_asf == NULL) {
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

uint32 ik_set_hi_high_asf(uint32 context_id, const ik_hi_high_asf_t *p_hi_high_asf)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;
    const amba_img_dsp_debug_print_func_t *p_print_fun = ik_get_debug_print_func();
    if(p_print_fun != NULL) {
        //p_print_fun->hi_high_asf(context_id, p_hi_high_asf);
    }

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((p_hi_high_asf != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        rval = p_ctx->organization.filter_methods.set_hi_high_asf(context_id, p_hi_high_asf);
    } else {
        if(p_hi_high_asf == NULL) {
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

uint32 ik_get_hi_high_asf(uint32 context_id, ik_hi_high_asf_t *p_hi_high_asf)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((p_hi_high_asf != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        rval = p_ctx->organization.filter_methods.get_hi_high_asf(context_id, p_hi_high_asf);
    } else {
        if(p_hi_high_asf == NULL) {
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

uint32 ik_set_hi_high2_asf(uint32 context_id, const ik_hi_high2_asf_t *p_hi_high2_asf)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;
    const amba_img_dsp_debug_print_func_t *p_print_fun = ik_get_debug_print_func();
    if(p_print_fun != NULL) {
        //p_print_fun->hi_high2_asf(context_id, p_hi_high2_asf);
    }

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((p_hi_high2_asf != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        rval = p_ctx->organization.filter_methods.set_hi_high2_asf(context_id, p_hi_high2_asf);
    } else {
        if(p_hi_high2_asf == NULL) {
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

uint32 ik_get_hi_high2_asf(uint32 context_id, ik_hi_high2_asf_t *p_hi_high2_asf)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((p_hi_high2_asf != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        rval = p_ctx->organization.filter_methods.get_hi_high2_asf(context_id, p_hi_high2_asf);
    } else {
        if(p_hi_high2_asf == NULL) {
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

uint32 ik_set_chroma_asf(uint32 context_id, const ik_chroma_asf_t *p_chroma_asf)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;
    const amba_img_dsp_debug_print_func_t *p_print_fun = ik_get_debug_print_func();
    if(p_print_fun != NULL) {
        //p_print_fun->chroma_asf(context_id, p_chroma_asf);
    }

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((p_chroma_asf != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        rval = p_ctx->organization.filter_methods.set_chroma_asf(context_id, p_chroma_asf);
    } else {
        if(p_chroma_asf == NULL) {
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

uint32 ik_get_chroma_asf(uint32 context_id, ik_chroma_asf_t *p_chroma_asf)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((p_chroma_asf != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        rval = p_ctx->organization.filter_methods.get_chroma_asf(context_id, p_chroma_asf);
    } else {
        if(p_chroma_asf == NULL) {
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

uint32 ik_set_hi_chroma_asf(uint32 context_id, const ik_hi_chroma_asf_t *p_hi_chroma_asf)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;
    const amba_img_dsp_debug_print_func_t *p_print_fun = ik_get_debug_print_func();
    if(p_print_fun != NULL) {
        //p_print_fun->hi_chroma_asf(context_id, p_hi_chroma_asf);
    }

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((p_hi_chroma_asf != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        rval = p_ctx->organization.filter_methods.set_hi_chroma_asf(context_id, p_hi_chroma_asf);
    } else {
        if(p_hi_chroma_asf == NULL) {
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

uint32 ik_get_hi_chroma_asf(uint32 context_id, ik_hi_chroma_asf_t *p_hi_chroma_asf)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((p_hi_chroma_asf != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        rval = p_ctx->organization.filter_methods.get_hi_chroma_asf(context_id, p_hi_chroma_asf);
    } else {
        if(p_hi_chroma_asf == NULL) {
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

uint32 ik_set_hi_low_chroma_asf(uint32 context_id, const ik_hi_low_chroma_asf_t *p_hi_low_chroma_asf)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;
    const amba_img_dsp_debug_print_func_t *p_print_fun = ik_get_debug_print_func();
    if(p_print_fun != NULL) {
        //p_print_fun->hi_low_chroma_asf(context_id, p_hi_low_chroma_asf);
    }

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((p_hi_low_chroma_asf != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        rval = p_ctx->organization.filter_methods.set_hi_low_chroma_asf(context_id, p_hi_low_chroma_asf);
    } else {
        if(p_hi_low_chroma_asf == NULL) {
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

uint32 ik_get_hi_low_chroma_asf(uint32 context_id, ik_hi_low_chroma_asf_t *p_hi_low_chroma_asf)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((p_hi_low_chroma_asf != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        rval = p_ctx->organization.filter_methods.get_hi_low_chroma_asf(context_id, p_hi_low_chroma_asf);
    } else {
        if(p_hi_low_chroma_asf == NULL) {
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

uint32 ik_set_hi_high_shpns_both(uint32 context_id, const ik_hi_high_shpns_both_t *p_hi_high_shpns_both)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;
    const amba_img_dsp_debug_print_func_t *p_print_fun = ik_get_debug_print_func();
    if(p_print_fun != NULL) {
        //p_print_fun->hi_high_shpns_both(context_id, p_hi_high_shpns_both);
    }

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((p_hi_high_shpns_both != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        rval = p_ctx->organization.filter_methods.set_hi_high_shpns_both(context_id, p_hi_high_shpns_both);
    } else {
        if(p_hi_high_shpns_both == NULL) {
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

uint32 ik_get_hi_high_shpns_both(uint32 context_id, ik_hi_high_shpns_both_t *p_hi_high_shpns_both)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((p_hi_high_shpns_both != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        rval = p_ctx->organization.filter_methods.get_hi_high_shpns_both(context_id, p_hi_high_shpns_both);
    } else {
        if(p_hi_high_shpns_both == NULL) {
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

uint32 ik_set_hi_high_shpns_noise(uint32 context_id, const ik_hi_high_shpns_noise_t *p_hi_high_shpns_noise)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;
    const amba_img_dsp_debug_print_func_t *p_print_fun = ik_get_debug_print_func();
    if(p_print_fun != NULL) {
        //p_print_fun->hi_high_shpns_noise(context_id, p_hi_high_shpns_noise);
    }

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((p_hi_high_shpns_noise != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        rval = p_ctx->organization.filter_methods.set_hi_high_shpns_noise(context_id, p_hi_high_shpns_noise);
    } else {
        if(p_hi_high_shpns_noise == NULL) {
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

uint32 ik_get_hi_high_shpns_noise(uint32 context_id, ik_hi_high_shpns_noise_t *p_hi_high_shpns_noise)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((p_hi_high_shpns_noise != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        rval = p_ctx->organization.filter_methods.get_hi_high_shpns_noise(context_id, p_hi_high_shpns_noise);
    } else {
        if(p_hi_high_shpns_noise == NULL) {
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

uint32 ik_set_hi_high_shpns_coring(uint32 context_id, const ik_hi_high_shpns_coring_t *p_hi_high_shpns_coring)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;
    const amba_img_dsp_debug_print_func_t *p_print_fun = ik_get_debug_print_func();
    if(p_print_fun != NULL) {
        //p_print_fun->hi_high_shpns_coring(context_id, p_hi_high_shpns_coring);
    }

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((p_hi_high_shpns_coring != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        rval = p_ctx->organization.filter_methods.set_hi_high_shpns_coring(context_id, p_hi_high_shpns_coring);
    } else {
        if(p_hi_high_shpns_coring == NULL) {
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

uint32 ik_get_hi_high_shpns_coring(uint32 context_id, ik_hi_high_shpns_coring_t *p_hi_high_shpns_coring)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((p_hi_high_shpns_coring != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        rval = p_ctx->organization.filter_methods.get_hi_high_shpns_coring(context_id, p_hi_high_shpns_coring);
    } else {
        if(p_hi_high_shpns_coring == NULL) {
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

uint32 ik_set_hi_high_shpns_fir(uint32 context_id, const ik_hi_high_shpns_fir_t *p_hi_high_shpns_fir)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;
    const amba_img_dsp_debug_print_func_t *p_print_fun = ik_get_debug_print_func();
    if(p_print_fun != NULL) {
        //p_print_fun->hi_high_shpns_fir(context_id, p_hi_high_shpns_fir);
    }

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((p_hi_high_shpns_fir != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        rval = p_ctx->organization.filter_methods.set_hi_high_shpns_fir(context_id, p_hi_high_shpns_fir);
    } else {
        if(p_hi_high_shpns_fir == NULL) {
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

uint32 ik_get_hi_high_shpns_fir(uint32 context_id, ik_hi_high_shpns_fir_t *p_hi_high_shpns_fir)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((p_hi_high_shpns_fir != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        rval = p_ctx->organization.filter_methods.get_hi_high_shpns_fir(context_id, p_hi_high_shpns_fir);
    } else {
        if(p_hi_high_shpns_fir == NULL) {
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

uint32 ik_set_hi_high_shpns_cor_idx_scl(uint32 context_id, const ik_hi_high_shpns_cor_idx_scl_t *p_hi_high_shpns_cor_idx_scl)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;
    const amba_img_dsp_debug_print_func_t *p_print_fun = ik_get_debug_print_func();
    if(p_print_fun != NULL) {
        //p_print_fun->hi_high_shpns_cor_idx_scl(context_id, p_hi_high_shpns_cor_idx_scl);
    }

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((p_hi_high_shpns_cor_idx_scl != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        rval = p_ctx->organization.filter_methods.set_hi_high_shpns_cor_idx_scl(context_id, p_hi_high_shpns_cor_idx_scl);
    } else {
        if(p_hi_high_shpns_cor_idx_scl == NULL) {
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

uint32 ik_get_hi_high_shpns_cor_idx_scl(uint32 context_id, ik_hi_high_shpns_cor_idx_scl_t *p_hi_high_shpns_cor_idx_scl)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((p_hi_high_shpns_cor_idx_scl != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        rval = p_ctx->organization.filter_methods.get_hi_high_shpns_cor_idx_scl(context_id, p_hi_high_shpns_cor_idx_scl);
    } else {
        if(p_hi_high_shpns_cor_idx_scl == NULL) {
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

uint32 ik_set_hi_high_shpns_min_cor_rst(uint32 context_id, const ik_hi_high_shpns_min_cor_rst_t *p_hi_high_shpns_min_cor_rst)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;
    const amba_img_dsp_debug_print_func_t *p_print_fun = ik_get_debug_print_func();
    if(p_print_fun != NULL) {
        //p_print_fun->hi_high_shpns_min_cor_rst(context_id, p_hi_high_shpns_min_cor_rst);
    }

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((p_hi_high_shpns_min_cor_rst != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        rval = p_ctx->organization.filter_methods.set_hi_high_shpns_min_cor_rst(context_id, p_hi_high_shpns_min_cor_rst);
    } else {
        if(p_hi_high_shpns_min_cor_rst == NULL) {
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

uint32 ik_get_hi_high_shpns_min_cor_rst(uint32 context_id, ik_hi_high_shpns_min_cor_rst_t *p_hi_high_shpns_min_cor_rst)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((p_hi_high_shpns_min_cor_rst != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        rval = p_ctx->organization.filter_methods.get_hi_high_shpns_min_cor_rst(context_id, p_hi_high_shpns_min_cor_rst);
    } else {
        if(p_hi_high_shpns_min_cor_rst == NULL) {
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

uint32 ik_set_hi_high_shpns_max_cor_rst(uint32 context_id, const ik_hi_high_shpns_max_cor_rst_t *p_hi_high_shpns_max_cor_rst)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;
    const amba_img_dsp_debug_print_func_t *p_print_fun = ik_get_debug_print_func();
    if(p_print_fun != NULL) {
        //p_print_fun->hi_high_shpns_max_cor_rst(context_id, p_hi_high_shpns_max_cor_rst);
    }

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((p_hi_high_shpns_max_cor_rst != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        rval = p_ctx->organization.filter_methods.set_hi_high_shpns_max_cor_rst(context_id, p_hi_high_shpns_max_cor_rst);
    } else {
        if(p_hi_high_shpns_max_cor_rst == NULL) {
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

uint32 ik_get_hi_high_shpns_max_cor_rst(uint32 context_id, ik_hi_high_shpns_max_cor_rst_t *p_hi_high_shpns_max_cor_rst)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((p_hi_high_shpns_max_cor_rst != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        rval = p_ctx->organization.filter_methods.get_hi_high_shpns_max_cor_rst(context_id, p_hi_high_shpns_max_cor_rst);
    } else {
        if(p_hi_high_shpns_max_cor_rst == NULL) {
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

uint32 ik_set_hi_high_shpns_scl_cor(uint32 context_id, const ik_hi_high_shpns_scl_cor_t *p_hi_high_shpns_scl_cor)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;
    const amba_img_dsp_debug_print_func_t *p_print_fun = ik_get_debug_print_func();
    if(p_print_fun != NULL) {
        //p_print_fun->hi_high_shpns_scl_cor(context_id, p_hi_high_shpns_scl_cor);
    }

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((p_hi_high_shpns_scl_cor != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        rval = p_ctx->organization.filter_methods.set_hi_high_shpns_scl_cor(context_id, p_hi_high_shpns_scl_cor);
    } else {
        if(p_hi_high_shpns_scl_cor == NULL) {
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

uint32 ik_get_hi_high_shpns_scl_cor(uint32 context_id, ik_hi_high_shpns_scl_cor_t *p_hi_high_shpns_scl_cor)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((p_hi_high_shpns_scl_cor != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        rval = p_ctx->organization.filter_methods.get_hi_high_shpns_scl_cor(context_id, p_hi_high_shpns_scl_cor);
    } else {
        if(p_hi_high_shpns_scl_cor == NULL) {
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

uint32 ik_set_hi_med_shpns_both(uint32 context_id, const ik_hi_med_shpns_both_t *p_hi_med_shpns_both)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;
    const amba_img_dsp_debug_print_func_t *p_print_fun = ik_get_debug_print_func();
    if(p_print_fun != NULL) {
        //p_print_fun->hi_med_shpns_both(context_id, p_hi_med_shpns_both);
    }

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((p_hi_med_shpns_both != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        rval = p_ctx->organization.filter_methods.set_hi_med_shpns_both(context_id, p_hi_med_shpns_both);
    } else {
        if(p_hi_med_shpns_both == NULL) {
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

uint32 ik_get_hi_med_shpns_both(uint32 context_id, ik_hi_med_shpns_both_t *p_hi_med_shpns_both)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((p_hi_med_shpns_both != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        rval = p_ctx->organization.filter_methods.get_hi_med_shpns_both(context_id, p_hi_med_shpns_both);
    } else {
        if(p_hi_med_shpns_both == NULL) {
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

uint32 ik_set_hi_med_shpns_noise(uint32 context_id, const ik_hi_med_shpns_noise_t *p_hi_med_shpns_noise)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;
    const amba_img_dsp_debug_print_func_t *p_print_fun = ik_get_debug_print_func();
    if(p_print_fun != NULL) {
        //p_print_fun->hi_med_shpns_noise(context_id, p_hi_med_shpns_noise);
    }

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((p_hi_med_shpns_noise != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        rval = p_ctx->organization.filter_methods.set_hi_med_shpns_noise(context_id, p_hi_med_shpns_noise);
    } else {
        if(p_hi_med_shpns_noise == NULL) {
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

uint32 ik_get_hi_med_shpns_noise(uint32 context_id, ik_hi_med_shpns_noise_t *p_hi_med_shpns_noise)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((p_hi_med_shpns_noise != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        rval = p_ctx->organization.filter_methods.get_hi_med_shpns_noise(context_id, p_hi_med_shpns_noise);
    } else {
        if(p_hi_med_shpns_noise == NULL) {
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

uint32 ik_set_hi_med_shpns_coring(uint32 context_id, const ik_hi_med_shpns_coring_t *p_hi_med_shpns_coring)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;
    const amba_img_dsp_debug_print_func_t *p_print_fun = ik_get_debug_print_func();
    if(p_print_fun != NULL) {
        //p_print_fun->hi_med_shpns_coring(context_id, p_hi_med_shpns_coring);
    }

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((p_hi_med_shpns_coring != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        rval = p_ctx->organization.filter_methods.set_hi_med_shpns_coring(context_id, p_hi_med_shpns_coring);
    } else {
        if(p_hi_med_shpns_coring == NULL) {
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

uint32 ik_get_hi_med_shpns_coring(uint32 context_id, ik_hi_med_shpns_coring_t *p_hi_med_shpns_coring)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((p_hi_med_shpns_coring != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        rval = p_ctx->organization.filter_methods.get_hi_med_shpns_coring(context_id, p_hi_med_shpns_coring);
    } else {
        if(p_hi_med_shpns_coring == NULL) {
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

uint32 ik_set_hi_med_shpns_fir(uint32 context_id, const ik_hi_med_shpns_fir_t *p_hi_med_shpns_fir)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;
    const amba_img_dsp_debug_print_func_t *p_print_fun = ik_get_debug_print_func();
    if(p_print_fun != NULL) {
        //p_print_fun->hi_med_shpns_fir(context_id, p_hi_med_shpns_fir);
    }

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((p_hi_med_shpns_fir != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        rval = p_ctx->organization.filter_methods.set_hi_med_shpns_fir(context_id, p_hi_med_shpns_fir);
    } else {
        if(p_hi_med_shpns_fir == NULL) {
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

uint32 ik_get_hi_med_shpns_fir(uint32 context_id, ik_hi_med_shpns_fir_t *p_hi_med_shpns_fir)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((p_hi_med_shpns_fir != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        rval = p_ctx->organization.filter_methods.get_hi_med_shpns_fir(context_id, p_hi_med_shpns_fir);
    } else {
        if(p_hi_med_shpns_fir == NULL) {
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

uint32 ik_set_hi_med_shpns_cor_idx_scl(uint32 context_id, const ik_hi_med_shpns_cor_idx_scl_t *p_hi_med_shpns_cor_idx_scl)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;
    const amba_img_dsp_debug_print_func_t *p_print_fun = ik_get_debug_print_func();
    if(p_print_fun != NULL) {
        //p_print_fun->hi_med_shpns_cor_idx_scl(context_id, p_hi_med_shpns_cor_idx_scl);
    }

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((p_hi_med_shpns_cor_idx_scl != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        rval = p_ctx->organization.filter_methods.set_hi_med_shpns_cor_idx_scl(context_id, p_hi_med_shpns_cor_idx_scl);
    } else {
        if(p_hi_med_shpns_cor_idx_scl == NULL) {
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

uint32 ik_get_hi_med_shpns_cor_idx_scl(uint32 context_id, ik_hi_med_shpns_cor_idx_scl_t *p_hi_med_shpns_cor_idx_scl)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((p_hi_med_shpns_cor_idx_scl != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        rval = p_ctx->organization.filter_methods.get_hi_med_shpns_cor_idx_scl(context_id, p_hi_med_shpns_cor_idx_scl);
    } else {
        if(p_hi_med_shpns_cor_idx_scl == NULL) {
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

uint32 ik_set_hi_med_shpns_min_cor_rst(uint32 context_id, const ik_hi_med_shpns_min_cor_rst_t *p_hi_med_shpns_min_cor_rst)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;
    const amba_img_dsp_debug_print_func_t *p_print_fun = ik_get_debug_print_func();
    if(p_print_fun != NULL) {
        //p_print_fun->hi_med_shpns_min_cor_rst(context_id, p_hi_med_shpns_min_cor_rst);
    }

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((p_hi_med_shpns_min_cor_rst != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        rval = p_ctx->organization.filter_methods.set_hi_med_shpns_min_cor_rst(context_id, p_hi_med_shpns_min_cor_rst);
    } else {
        if(p_hi_med_shpns_min_cor_rst == NULL) {
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

uint32 ik_get_hi_med_shpns_min_cor_rst(uint32 context_id, ik_hi_med_shpns_min_cor_rst_t *p_hi_med_shpns_min_cor_rst)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((p_hi_med_shpns_min_cor_rst != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        rval = p_ctx->organization.filter_methods.get_hi_med_shpns_min_cor_rst(context_id, p_hi_med_shpns_min_cor_rst);
    } else {
        if(p_hi_med_shpns_min_cor_rst == NULL) {
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

uint32 ik_set_hi_med_shpns_max_cor_rst(uint32 context_id, const ik_hi_med_shpns_max_cor_rst_t *p_hi_med_shpns_max_cor_rst)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;
    const amba_img_dsp_debug_print_func_t *p_print_fun = ik_get_debug_print_func();
    if(p_print_fun != NULL) {
        //p_print_fun->hi_med_shpns_max_cor_rst(context_id, p_hi_med_shpns_max_cor_rst);
    }

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((p_hi_med_shpns_max_cor_rst != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        rval = p_ctx->organization.filter_methods.set_hi_med_shpns_max_cor_rst(context_id, p_hi_med_shpns_max_cor_rst);
    } else {
        if(p_hi_med_shpns_max_cor_rst == NULL) {
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

uint32 ik_get_hi_med_shpns_max_cor_rst(uint32 context_id, ik_hi_med_shpns_max_cor_rst_t *p_hi_med_shpns_max_cor_rst)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((p_hi_med_shpns_max_cor_rst != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        rval = p_ctx->organization.filter_methods.get_hi_med_shpns_max_cor_rst(context_id, p_hi_med_shpns_max_cor_rst);
    } else {
        if(p_hi_med_shpns_max_cor_rst == NULL) {
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

uint32 ik_set_hi_med_shpns_scl_cor(uint32 context_id, const ik_hi_med_shpns_scl_cor_t *p_hi_med_shpns_scl_cor)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;
    const amba_img_dsp_debug_print_func_t *p_print_fun = ik_get_debug_print_func();
    if(p_print_fun != NULL) {
        //p_print_fun->hi_med_shpns_scl_cor(context_id, p_hi_med_shpns_scl_cor);
    }

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((p_hi_med_shpns_scl_cor != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        rval = p_ctx->organization.filter_methods.set_hi_med_shpns_scl_cor(context_id, p_hi_med_shpns_scl_cor);
    } else {
        if(p_hi_med_shpns_scl_cor == NULL) {
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

uint32 ik_get_hi_med_shpns_scl_cor(uint32 context_id, ik_hi_med_shpns_scl_cor_t *p_hi_med_shpns_scl_cor)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((p_hi_med_shpns_scl_cor != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        rval = p_ctx->organization.filter_methods.get_hi_med_shpns_scl_cor(context_id, p_hi_med_shpns_scl_cor);
    } else {
        if(p_hi_med_shpns_scl_cor == NULL) {
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

uint32 ik_set_li2_shpns_both(uint32 context_id, const ik_li2_shpns_both_t *p_li2_shpns_both)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;
    const amba_img_dsp_debug_print_func_t *p_print_fun = ik_get_debug_print_func();
    if(p_print_fun != NULL) {
        //p_print_fun->li2_shpns_both(context_id, p_li2_shpns_both);
    }

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((p_li2_shpns_both != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        rval = p_ctx->organization.filter_methods.set_li2_shpns_both(context_id, p_li2_shpns_both);
    } else {
        if(p_li2_shpns_both == NULL) {
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

uint32 ik_get_li2_shpns_both(uint32 context_id, ik_li2_shpns_both_t *p_li2_shpns_both)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((p_li2_shpns_both != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        rval = p_ctx->organization.filter_methods.get_li2_shpns_both(context_id, p_li2_shpns_both);
    } else {
        if(p_li2_shpns_both == NULL) {
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

uint32 ik_set_li2_shpns_noise(uint32 context_id, const ik_li2_shpns_noise_t *p_li2_shpns_noise)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;
    const amba_img_dsp_debug_print_func_t *p_print_fun = ik_get_debug_print_func();
    if(p_print_fun != NULL) {
        //p_print_fun->li2_shpns_noise(context_id, p_li2_shpns_noise);
    }

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((p_li2_shpns_noise != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        rval = p_ctx->organization.filter_methods.set_li2_shpns_noise(context_id, p_li2_shpns_noise);
    } else {
        if(p_li2_shpns_noise == NULL) {
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

uint32 ik_get_li2_shpns_noise(uint32 context_id, ik_li2_shpns_noise_t *p_li2_shpns_noise)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((p_li2_shpns_noise != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        rval = p_ctx->organization.filter_methods.get_li2_shpns_noise(context_id, p_li2_shpns_noise);
    } else {
        if(p_li2_shpns_noise == NULL) {
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

uint32 ik_set_li2_shpns_coring(uint32 context_id, const ik_li2_shpns_coring_t *p_li2_shpns_coring)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;
    const amba_img_dsp_debug_print_func_t *p_print_fun = ik_get_debug_print_func();
    if(p_print_fun != NULL) {
        //p_print_fun->li2_shpns_coring(context_id, p_li2_shpns_coring);
    }

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((p_li2_shpns_coring != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        rval = p_ctx->organization.filter_methods.set_li2_shpns_coring(context_id, p_li2_shpns_coring);
    } else {
        if(p_li2_shpns_coring == NULL) {
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

uint32 ik_get_li2_shpns_coring(uint32 context_id, ik_li2_shpns_coring_t *p_li2_shpns_coring)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((p_li2_shpns_coring != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        rval = p_ctx->organization.filter_methods.get_li2_shpns_coring(context_id, p_li2_shpns_coring);
    } else {
        if(p_li2_shpns_coring == NULL) {
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

uint32 ik_set_li2_shpns_fir(uint32 context_id, const ik_li2_shpns_fir_t *p_li2_shpns_fir)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;
    const amba_img_dsp_debug_print_func_t *p_print_fun = ik_get_debug_print_func();
    if(p_print_fun != NULL) {
        //p_print_fun->li2_shpns_fir(context_id, p_li2_shpns_fir);
    }

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((p_li2_shpns_fir != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        rval = p_ctx->organization.filter_methods.set_li2_shpns_fir(context_id, p_li2_shpns_fir);
    } else {
        if(p_li2_shpns_fir == NULL) {
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

uint32 ik_get_li2_shpns_fir(uint32 context_id, ik_li2_shpns_fir_t *p_li2_shpns_fir)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((p_li2_shpns_fir != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        rval = p_ctx->organization.filter_methods.get_li2_shpns_fir(context_id, p_li2_shpns_fir);
    } else {
        if(p_li2_shpns_fir == NULL) {
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

uint32 ik_set_li2_shpns_cor_idx_scl(uint32 context_id, const ik_li2_shpns_cor_idx_scl_t *p_li2_shpns_cor_idx_scl)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;
    const amba_img_dsp_debug_print_func_t *p_print_fun = ik_get_debug_print_func();
    if(p_print_fun != NULL) {
        //p_print_fun->li2_shpns_cor_idx_scl(context_id, p_li2_shpns_cor_idx_scl);
    }

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((p_li2_shpns_cor_idx_scl != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        rval = p_ctx->organization.filter_methods.set_li2_shpns_cor_idx_scl(context_id, p_li2_shpns_cor_idx_scl);
    } else {
        if(p_li2_shpns_cor_idx_scl == NULL) {
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

uint32 ik_get_li2_shpns_cor_idx_scl(uint32 context_id, ik_li2_shpns_cor_idx_scl_t *p_li2_shpns_cor_idx_scl)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((p_li2_shpns_cor_idx_scl != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        rval = p_ctx->organization.filter_methods.get_li2_shpns_cor_idx_scl(context_id, p_li2_shpns_cor_idx_scl);
    } else {
        if(p_li2_shpns_cor_idx_scl == NULL) {
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

uint32 ik_set_li2_shpns_min_cor_rst(uint32 context_id, const ik_li2_shpns_min_cor_rst_t *p_li2_shpns_min_cor_rst)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;
    const amba_img_dsp_debug_print_func_t *p_print_fun = ik_get_debug_print_func();
    if(p_print_fun != NULL) {
        //p_print_fun->li2_shpns_min_cor_rst(context_id, p_li2_shpns_min_cor_rst);
    }

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((p_li2_shpns_min_cor_rst != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        rval = p_ctx->organization.filter_methods.set_li2_shpns_min_cor_rst(context_id, p_li2_shpns_min_cor_rst);
    } else {
        if(p_li2_shpns_min_cor_rst == NULL) {
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

uint32 ik_get_li2_shpns_min_cor_rst(uint32 context_id, ik_li2_shpns_min_cor_rst_t *p_li2_shpns_min_cor_rst)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((p_li2_shpns_min_cor_rst != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        rval = p_ctx->organization.filter_methods.get_li2_shpns_min_cor_rst(context_id, p_li2_shpns_min_cor_rst);
    } else {
        if(p_li2_shpns_min_cor_rst == NULL) {
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

uint32 ik_set_li2_shpns_max_cor_rst(uint32 context_id, const ik_li2_shpns_max_cor_rst_t *p_li2_shpns_max_cor_rst)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;
    const amba_img_dsp_debug_print_func_t *p_print_fun = ik_get_debug_print_func();
    if(p_print_fun != NULL) {
        //p_print_fun->li2_shpns_max_cor_rst(context_id, p_li2_shpns_max_cor_rst);
    }

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((p_li2_shpns_max_cor_rst != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        rval = p_ctx->organization.filter_methods.set_li2_shpns_max_cor_rst(context_id, p_li2_shpns_max_cor_rst);
    } else {
        if(p_li2_shpns_max_cor_rst == NULL) {
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

uint32 ik_get_li2_shpns_max_cor_rst(uint32 context_id, ik_li2_shpns_max_cor_rst_t *p_li2_shpns_max_cor_rst)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((p_li2_shpns_max_cor_rst != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        rval = p_ctx->organization.filter_methods.get_li2_shpns_max_cor_rst(context_id, p_li2_shpns_max_cor_rst);
    } else {
        if(p_li2_shpns_max_cor_rst == NULL) {
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

uint32 ik_set_li2_shpns_scl_cor(uint32 context_id, const ik_li2_shpns_scl_cor_t *p_li2_shpns_scl_cor)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;
    const amba_img_dsp_debug_print_func_t *p_print_fun = ik_get_debug_print_func();
    if(p_print_fun != NULL) {
        //p_print_fun->li2_shpns_scl_cor(context_id, p_li2_shpns_scl_cor);
    }

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((p_li2_shpns_scl_cor != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        rval = p_ctx->organization.filter_methods.set_li2_shpns_scl_cor(context_id, p_li2_shpns_scl_cor);
    } else {
        if(p_li2_shpns_scl_cor == NULL) {
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

uint32 ik_get_li2_shpns_scl_cor(uint32 context_id, ik_li2_shpns_scl_cor_t *p_li2_shpns_scl_cor)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((p_li2_shpns_scl_cor != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        rval = p_ctx->organization.filter_methods.get_li2_shpns_scl_cor(context_id, p_li2_shpns_scl_cor);
    } else {
        if(p_li2_shpns_scl_cor == NULL) {
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

uint32 ik_set_hili_shpns_both(uint32 context_id, const ik_hili_shpns_both_t *p_hili_shpns_both)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;
    const amba_img_dsp_debug_print_func_t *p_print_fun = ik_get_debug_print_func();
    if(p_print_fun != NULL) {
        //p_print_fun->hili_shpns_both(context_id, p_hili_shpns_both);
    }

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((p_hili_shpns_both != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        rval = p_ctx->organization.filter_methods.set_hili_shpns_both(context_id, p_hili_shpns_both);
    } else {
        if(p_hili_shpns_both == NULL) {
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

uint32 ik_get_hili_shpns_both(uint32 context_id, ik_hili_shpns_both_t *p_hili_shpns_both)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((p_hili_shpns_both != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        rval = p_ctx->organization.filter_methods.get_hili_shpns_both(context_id, p_hili_shpns_both);
    } else {
        if(p_hili_shpns_both == NULL) {
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

uint32 ik_set_hili_shpns_noise(uint32 context_id, const ik_hili_shpns_noise_t *p_hili_shpns_noise)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;
    const amba_img_dsp_debug_print_func_t *p_print_fun = ik_get_debug_print_func();
    if(p_print_fun != NULL) {
        //p_print_fun->hili_shpns_noise(context_id, p_hili_shpns_noise);
    }

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((p_hili_shpns_noise != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        rval = p_ctx->organization.filter_methods.set_hili_shpns_noise(context_id, p_hili_shpns_noise);
    } else {
        if(p_hili_shpns_noise == NULL) {
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

uint32 ik_get_hili_shpns_noise(uint32 context_id, ik_hili_shpns_noise_t *p_hili_shpns_noise)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((p_hili_shpns_noise != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        rval = p_ctx->organization.filter_methods.get_hili_shpns_noise(context_id, p_hili_shpns_noise);
    } else {
        if(p_hili_shpns_noise == NULL) {
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

uint32 ik_set_hili_shpns_coring(uint32 context_id, const ik_hili_shpns_coring_t *p_hili_shpns_coring)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;
    const amba_img_dsp_debug_print_func_t *p_print_fun = ik_get_debug_print_func();
    if(p_print_fun != NULL) {
        //p_print_fun->hili_shpns_coring(context_id, p_hili_shpns_coring);
    }

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((p_hili_shpns_coring != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        rval = p_ctx->organization.filter_methods.set_hili_shpns_coring(context_id, p_hili_shpns_coring);
    } else {
        if(p_hili_shpns_coring == NULL) {
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

uint32 ik_get_hili_shpns_coring(uint32 context_id, ik_hili_shpns_coring_t *p_hili_shpns_coring)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((p_hili_shpns_coring != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        rval = p_ctx->organization.filter_methods.get_hili_shpns_coring(context_id, p_hili_shpns_coring);
    } else {
        if(p_hili_shpns_coring == NULL) {
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

uint32 ik_set_hili_shpns_fir(uint32 context_id, const ik_hili_shpns_fir_t *p_hili_shpns_fir)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;
    const amba_img_dsp_debug_print_func_t *p_print_fun = ik_get_debug_print_func();
    if(p_print_fun != NULL) {
        //p_print_fun->hili_shpns_fir(context_id, p_hili_shpns_fir);
    }

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((p_hili_shpns_fir != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        rval = p_ctx->organization.filter_methods.set_hili_shpns_fir(context_id, p_hili_shpns_fir);
    } else {
        if(p_hili_shpns_fir == NULL) {
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

uint32 ik_get_hili_shpns_fir(uint32 context_id, ik_hili_shpns_fir_t *p_hili_shpns_fir)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((p_hili_shpns_fir != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        rval = p_ctx->organization.filter_methods.get_hili_shpns_fir(context_id, p_hili_shpns_fir);
    } else {
        if(p_hili_shpns_fir == NULL) {
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

uint32 ik_set_hili_shpns_cor_idx_scl(uint32 context_id, const ik_hili_shpns_cor_idx_scl_t *p_hili_shpns_cor_idx_scl)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;
    const amba_img_dsp_debug_print_func_t *p_print_fun = ik_get_debug_print_func();
    if(p_print_fun != NULL) {
        //p_print_fun->hili_shpns_cor_idx_scl(context_id, p_hili_shpns_cor_idx_scl);
    }

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((p_hili_shpns_cor_idx_scl != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        rval = p_ctx->organization.filter_methods.set_hili_shpns_cor_idx_scl(context_id, p_hili_shpns_cor_idx_scl);
    } else {
        if(p_hili_shpns_cor_idx_scl == NULL) {
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

uint32 ik_get_hili_shpns_cor_idx_scl(uint32 context_id, ik_hili_shpns_cor_idx_scl_t *p_hili_shpns_cor_idx_scl)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((p_hili_shpns_cor_idx_scl != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        rval = p_ctx->organization.filter_methods.get_hili_shpns_cor_idx_scl(context_id, p_hili_shpns_cor_idx_scl);
    } else {
        if(p_hili_shpns_cor_idx_scl == NULL) {
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

uint32 ik_set_hili_shpns_min_cor_rst(uint32 context_id, const ik_hili_shpns_min_cor_rst_t *p_hili_shpns_min_cor_rst)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;
    const amba_img_dsp_debug_print_func_t *p_print_fun = ik_get_debug_print_func();
    if(p_print_fun != NULL) {
        //p_print_fun->hili_shpns_min_cor_rst(context_id, p_hili_shpns_min_cor_rst);
    }

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((p_hili_shpns_min_cor_rst != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        rval = p_ctx->organization.filter_methods.set_hili_shpns_min_cor_rst(context_id, p_hili_shpns_min_cor_rst);
    } else {
        if(p_hili_shpns_min_cor_rst == NULL) {
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

uint32 ik_get_hili_shpns_min_cor_rst(uint32 context_id, ik_hili_shpns_min_cor_rst_t *p_hili_shpns_min_cor_rst)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((p_hili_shpns_min_cor_rst != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        rval = p_ctx->organization.filter_methods.get_hili_shpns_min_cor_rst(context_id, p_hili_shpns_min_cor_rst);
    } else {
        if(p_hili_shpns_min_cor_rst == NULL) {
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

uint32 ik_set_hili_shpns_max_cor_rst(uint32 context_id, const ik_hili_shpns_max_cor_rst_t *p_hili_shpns_max_cor_rst)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;
    const amba_img_dsp_debug_print_func_t *p_print_fun = ik_get_debug_print_func();
    if(p_print_fun != NULL) {
        //p_print_fun->hili_shpns_max_cor_rst(context_id, p_hili_shpns_max_cor_rst);
    }

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((p_hili_shpns_max_cor_rst != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        rval = p_ctx->organization.filter_methods.set_hili_shpns_max_cor_rst(context_id, p_hili_shpns_max_cor_rst);
    } else {
        if(p_hili_shpns_max_cor_rst == NULL) {
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

uint32 ik_get_hili_shpns_max_cor_rst(uint32 context_id, ik_hili_shpns_max_cor_rst_t *p_hili_shpns_max_cor_rst)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((p_hili_shpns_max_cor_rst != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        rval = p_ctx->organization.filter_methods.get_hili_shpns_max_cor_rst(context_id, p_hili_shpns_max_cor_rst);
    } else {
        if(p_hili_shpns_max_cor_rst == NULL) {
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

uint32 ik_set_hili_shpns_scl_cor(uint32 context_id, const ik_hili_shpns_scl_cor_t *p_hili_shpns_scl_cor)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;
    const amba_img_dsp_debug_print_func_t *p_print_fun = ik_get_debug_print_func();
    if(p_print_fun != NULL) {
        //p_print_fun->hili_shpns_scl_cor(context_id, p_hili_shpns_scl_cor);
    }

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((p_hili_shpns_scl_cor != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        rval = p_ctx->organization.filter_methods.set_hili_shpns_scl_cor(context_id, p_hili_shpns_scl_cor);
    } else {
        if(p_hili_shpns_scl_cor == NULL) {
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

uint32 ik_get_hili_shpns_scl_cor(uint32 context_id, ik_hili_shpns_scl_cor_t *p_hili_shpns_scl_cor)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((p_hili_shpns_scl_cor != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        rval = p_ctx->organization.filter_methods.get_hili_shpns_scl_cor(context_id, p_hili_shpns_scl_cor);
    } else {
        if(p_hili_shpns_scl_cor == NULL) {
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

uint32 ik_set_hi_chroma_filter_high(uint32 context_id, const ik_hi_chroma_filter_high_t *p_hi_chroma_filter_high)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;
    const amba_img_dsp_debug_print_func_t *p_print_fun = ik_get_debug_print_func();
    if(p_print_fun != NULL) {
        //p_print_fun->hi_chroma_filter_high(context_id, p_hi_chroma_filter_high);
    }

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((p_hi_chroma_filter_high != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        rval = p_ctx->organization.filter_methods.set_hi_chroma_filter_high(context_id, p_hi_chroma_filter_high);
    } else {
        if(p_hi_chroma_filter_high == NULL) {
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

uint32 ik_get_hi_chroma_filter_high(uint32 context_id, ik_hi_chroma_filter_high_t *p_hi_chroma_filter_high)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((p_hi_chroma_filter_high != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        rval = p_ctx->organization.filter_methods.get_hi_chroma_filter_high(context_id, p_hi_chroma_filter_high);
    } else {
        if(p_hi_chroma_filter_high == NULL) {
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

uint32 ik_set_hi_chroma_filter_pre(uint32 context_id, const ik_hi_chroma_filter_pre_t *p_hi_chroma_filter_pre)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;
    const amba_img_dsp_debug_print_func_t *p_print_fun = ik_get_debug_print_func();
    if(p_print_fun != NULL) {
        //p_print_fun->hi_chroma_filter_pre(context_id, p_hi_chroma_filter_pre);
    }

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((p_hi_chroma_filter_pre != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        rval = p_ctx->organization.filter_methods.set_hi_chroma_filter_pre(context_id, p_hi_chroma_filter_pre);
    } else {
        if(p_hi_chroma_filter_pre == NULL) {
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

uint32 ik_get_hi_chroma_filter_pre(uint32 context_id, ik_hi_chroma_filter_pre_t *p_hi_chroma_filter_pre)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((p_hi_chroma_filter_pre != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        rval = p_ctx->organization.filter_methods.get_hi_chroma_filter_pre(context_id, p_hi_chroma_filter_pre);
    } else {
        if(p_hi_chroma_filter_pre == NULL) {
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

uint32 ik_set_hi_chroma_filter_med(uint32 context_id, const ik_hi_chroma_filter_med_t *p_hi_chroma_filter_med)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;
    const amba_img_dsp_debug_print_func_t *p_print_fun = ik_get_debug_print_func();
    if(p_print_fun != NULL) {
        //p_print_fun->hi_chroma_filter_med(context_id, p_hi_chroma_filter_med);
    }

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((p_hi_chroma_filter_med != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        rval = p_ctx->organization.filter_methods.set_hi_chroma_filter_med(context_id, p_hi_chroma_filter_med);
    } else {
        if(p_hi_chroma_filter_med == NULL) {
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

uint32 ik_get_hi_chroma_filter_med(uint32 context_id, ik_hi_chroma_filter_med_t *p_hi_chroma_filter_med)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((p_hi_chroma_filter_med != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        rval = p_ctx->organization.filter_methods.get_hi_chroma_filter_med(context_id, p_hi_chroma_filter_med);
    } else {
        if(p_hi_chroma_filter_med == NULL) {
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

uint32 ik_set_hi_chroma_filter_low(uint32 context_id, const ik_hi_chroma_filter_low_t *p_hi_chroma_filter_low)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;
    const amba_img_dsp_debug_print_func_t *p_print_fun = ik_get_debug_print_func();
    if(p_print_fun != NULL) {
        //p_print_fun->hi_chroma_filter_low(context_id, p_hi_chroma_filter_low);
    }

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((p_hi_chroma_filter_low != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        rval = p_ctx->organization.filter_methods.set_hi_chroma_filter_low(context_id, p_hi_chroma_filter_low);
    } else {
        if(p_hi_chroma_filter_low == NULL) {
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

uint32 ik_get_hi_chroma_filter_low(uint32 context_id, ik_hi_chroma_filter_low_t *p_hi_chroma_filter_low)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((p_hi_chroma_filter_low != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        rval = p_ctx->organization.filter_methods.get_hi_chroma_filter_low(context_id, p_hi_chroma_filter_low);
    } else {
        if(p_hi_chroma_filter_low == NULL) {
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

uint32 ik_set_hi_chroma_filter_very_low(uint32 context_id, const ik_hi_chroma_filter_very_low_t *p_hi_chroma_filter_very_low)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;
    const amba_img_dsp_debug_print_func_t *p_print_fun = ik_get_debug_print_func();
    if(p_print_fun != NULL) {
        //p_print_fun->hi_chroma_filter_very_low(context_id, p_hi_chroma_filter_very_low);
    }

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((p_hi_chroma_filter_very_low != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        rval = p_ctx->organization.filter_methods.set_hi_chroma_filter_very_low(context_id, p_hi_chroma_filter_very_low);
    } else {
        if(p_hi_chroma_filter_very_low == NULL) {
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

uint32 ik_get_hi_chroma_filter_very_low(uint32 context_id, ik_hi_chroma_filter_very_low_t *p_hi_chroma_filter_very_low)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((p_hi_chroma_filter_very_low != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        rval = p_ctx->organization.filter_methods.get_hi_chroma_filter_very_low(context_id, p_hi_chroma_filter_very_low);
    } else {
        if(p_hi_chroma_filter_very_low == NULL) {
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

uint32 ik_set_hi_luma_combine(uint32 context_id, const ik_hi_luma_combine_t *p_hi_luma_combine)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;
    const amba_img_dsp_debug_print_func_t *p_print_fun = ik_get_debug_print_func();
    if(p_print_fun != NULL) {
        //p_print_fun->hi_luma_combine(context_id, p_hi_luma_combine);
    }

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((p_hi_luma_combine != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        rval = p_ctx->organization.filter_methods.set_hi_luma_combine(context_id, p_hi_luma_combine);
    } else {
        if(p_hi_luma_combine == NULL) {
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

uint32 ik_get_hi_luma_combine(uint32 context_id, ik_hi_luma_combine_t *p_hi_luma_combine)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((p_hi_luma_combine != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        rval = p_ctx->organization.filter_methods.get_hi_luma_combine(context_id, p_hi_luma_combine);
    } else {
        if(p_hi_luma_combine == NULL) {
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

uint32 ik_set_hi_low_asf_combine(uint32 context_id, const ik_hi_low_asf_combine_t *p_hi_low_asf_combine)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;
    const amba_img_dsp_debug_print_func_t *p_print_fun = ik_get_debug_print_func();
    if(p_print_fun != NULL) {
        //p_print_fun->hi_low_asf_combine(context_id, p_hi_low_asf_combine);
    }

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((p_hi_low_asf_combine != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        rval = p_ctx->organization.filter_methods.set_hi_low_asf_combine(context_id, p_hi_low_asf_combine);
    } else {
        if(p_hi_low_asf_combine == NULL) {
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

uint32 ik_get_hi_low_asf_combine(uint32 context_id, ik_hi_low_asf_combine_t *p_hi_low_asf_combine)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((p_hi_low_asf_combine != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        rval = p_ctx->organization.filter_methods.get_hi_low_asf_combine(context_id, p_hi_low_asf_combine);
    } else {
        if(p_hi_low_asf_combine == NULL) {
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

uint32 ik_set_hi_chroma_fltr_med_com(uint32 context_id, const ik_hi_chroma_fltr_med_com_t *p_hi_chroma_fltr_med_com)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;
    const amba_img_dsp_debug_print_func_t *p_print_fun = ik_get_debug_print_func();
    if(p_print_fun != NULL) {
        //p_print_fun->hi_chroma_fltr_med_com(context_id, p_hi_chroma_fltr_med_com);
    }

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((p_hi_chroma_fltr_med_com != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        rval = p_ctx->organization.filter_methods.set_hi_chroma_fltr_med_com(context_id, p_hi_chroma_fltr_med_com);
    } else {
        if(p_hi_chroma_fltr_med_com == NULL) {
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

uint32 ik_get_hi_chroma_fltr_med_com(uint32 context_id, ik_hi_chroma_fltr_med_com_t *p_hi_chroma_fltr_med_com)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((p_hi_chroma_fltr_med_com != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        rval = p_ctx->organization.filter_methods.get_hi_chroma_fltr_med_com(context_id, p_hi_chroma_fltr_med_com);
    } else {
        if(p_hi_chroma_fltr_med_com == NULL) {
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

uint32 ik_set_hi_chroma_fltr_low_com(uint32 context_id, const ik_hi_chroma_fltr_low_com_t *p_hi_chroma_fltr_low_com)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;
    const amba_img_dsp_debug_print_func_t *p_print_fun = ik_get_debug_print_func();
    if(p_print_fun != NULL) {
        //p_print_fun->hi_chroma_fltr_low_com(context_id, p_hi_chroma_fltr_low_com);
    }

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((p_hi_chroma_fltr_low_com != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        rval = p_ctx->organization.filter_methods.set_hi_chroma_fltr_low_com(context_id, p_hi_chroma_fltr_low_com);
    } else {
        if(p_hi_chroma_fltr_low_com == NULL) {
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

uint32 ik_get_hi_chroma_fltr_low_com(uint32 context_id, ik_hi_chroma_fltr_low_com_t *p_hi_chroma_fltr_low_com)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((p_hi_chroma_fltr_low_com != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        rval = p_ctx->organization.filter_methods.get_hi_chroma_fltr_low_com(context_id, p_hi_chroma_fltr_low_com);
    } else {
        if(p_hi_chroma_fltr_low_com == NULL) {
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

uint32 ik_set_hi_chroma_fltr_very_low_com(uint32 context_id, const ik_hi_chroma_fltr_very_low_com_t *p_hi_chroma_fltr_very_low_com)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;
    const amba_img_dsp_debug_print_func_t *p_print_fun = ik_get_debug_print_func();
    if(p_print_fun != NULL) {
        //p_print_fun->hi_chroma_fltr_very_low_com(context_id, p_hi_chroma_fltr_very_low_com);
    }

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((p_hi_chroma_fltr_very_low_com != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        rval = p_ctx->organization.filter_methods.set_hi_chroma_fltr_very_low_com(context_id, p_hi_chroma_fltr_very_low_com);
    } else {
        if(p_hi_chroma_fltr_very_low_com == NULL) {
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

uint32 ik_get_hi_chroma_fltr_very_low_com(uint32 context_id, ik_hi_chroma_fltr_very_low_com_t *p_hi_chroma_fltr_very_low_com)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((p_hi_chroma_fltr_very_low_com != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        rval = p_ctx->organization.filter_methods.get_hi_chroma_fltr_very_low_com(context_id, p_hi_chroma_fltr_very_low_com);
    } else {
        if(p_hi_chroma_fltr_very_low_com == NULL) {
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

uint32 ik_set_hili_combine(uint32 context_id, const ik_hili_combine_t *p_hili_combine)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;
    const amba_img_dsp_debug_print_func_t *p_print_fun = ik_get_debug_print_func();
    if(p_print_fun != NULL) {
        //p_print_fun->hili_combine(context_id, p_hili_combine);
    }

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((p_hili_combine != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        rval = p_ctx->organization.filter_methods.set_hili_combine(context_id, p_hili_combine);
    } else {
        if(p_hili_combine == NULL) {
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

uint32 ik_get_hili_combine(uint32 context_id, ik_hili_combine_t *p_hili_combine)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((p_hili_combine != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        rval = p_ctx->organization.filter_methods.get_hili_combine(context_id, p_hili_combine);
    } else {
        if(p_hili_combine == NULL) {
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

uint32 ik_set_hi_mid_high_freq_recover(uint32 context_id, const ik_hi_mid_high_freq_recover_t *p_hi_mid_high_freq_recover)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;
    const amba_img_dsp_debug_print_func_t *p_print_fun = ik_get_debug_print_func();
    if(p_print_fun != NULL) {
        //p_print_fun->hi_mid_high_freq_recover(context_id, p_hi_mid_high_freq_recover);
    }

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((p_hi_mid_high_freq_recover != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        rval = p_ctx->organization.filter_methods.set_hi_mid_high_freq_recover(context_id, p_hi_mid_high_freq_recover);
    } else {
        if(p_hi_mid_high_freq_recover == NULL) {
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

uint32 ik_get_hi_mid_high_freq_recover(uint32 context_id, ik_hi_mid_high_freq_recover_t *p_hi_mid_high_freq_recover)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((p_hi_mid_high_freq_recover != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        rval = p_ctx->organization.filter_methods.get_hi_mid_high_freq_recover(context_id, p_hi_mid_high_freq_recover);
    } else {
        if(p_hi_mid_high_freq_recover == NULL) {
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

uint32 ik_set_hi_luma_blend(uint32 context_id, const ik_hi_luma_blend_t *p_hi_luma_blend)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;
    const amba_img_dsp_debug_print_func_t *p_print_fun = ik_get_debug_print_func();
    if(p_print_fun != NULL) {
        //p_print_fun->hi_luma_blend(context_id, p_hi_luma_blend);
    }

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((p_hi_luma_blend != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        rval = p_ctx->organization.filter_methods.set_hi_luma_blend(context_id, p_hi_luma_blend);
    } else {
        if(p_hi_luma_blend == NULL) {
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

uint32 ik_get_hi_luma_blend(uint32 context_id, ik_hi_luma_blend_t *p_hi_luma_blend)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((p_hi_luma_blend != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        rval = p_ctx->organization.filter_methods.get_hi_luma_blend(context_id, p_hi_luma_blend);
    } else {
        if(p_hi_luma_blend == NULL) {
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

uint32 ik_set_hi_nonsmooth_detect(uint32 context_id, const ik_hi_nonsmooth_detect_t *p_hi_nonsmooth_detect)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;
    const amba_img_dsp_debug_print_func_t *p_print_fun = ik_get_debug_print_func();
    if(p_print_fun != NULL) {
        //p_print_fun->hi_nonsmooth_detect(context_id, p_hi_nonsmooth_detect);
    }

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((p_hi_nonsmooth_detect != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        rval = p_ctx->organization.filter_methods.set_hi_nonsmooth_detect(context_id, p_hi_nonsmooth_detect);
    } else {
        if(p_hi_nonsmooth_detect == NULL) {
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

uint32 ik_get_hi_nonsmooth_detect(uint32 context_id, ik_hi_nonsmooth_detect_t *p_hi_nonsmooth_detect)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((p_hi_nonsmooth_detect != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        rval = p_ctx->organization.filter_methods.get_hi_nonsmooth_detect(context_id, p_hi_nonsmooth_detect);
    } else {
        if(p_hi_nonsmooth_detect == NULL) {
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

uint32 ik_set_hi_select(uint32 context_id, const ik_hi_select_t *p_hi_select)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;
    const amba_img_dsp_debug_print_func_t *p_print_fun = ik_get_debug_print_func();
    if(p_print_fun != NULL) {
        //p_print_fun->hi_select(context_id, p_hi_select);
    }

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((p_hi_select != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        rval = p_ctx->organization.filter_methods.set_hi_select(context_id, p_hi_select);
    } else {
        if(p_hi_select == NULL) {
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

uint32 ik_get_hi_select(uint32 context_id, ik_hi_select_t *p_hi_select)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((p_hi_select != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        rval = p_ctx->organization.filter_methods.get_hi_select(context_id, p_hi_select);
    } else {
        if(p_hi_select == NULL) {
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

