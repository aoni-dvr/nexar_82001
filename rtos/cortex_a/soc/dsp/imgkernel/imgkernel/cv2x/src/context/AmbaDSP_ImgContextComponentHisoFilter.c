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

// Hiso
uint32 img_ctx_ivd_set_hi_anti_aliasing(uint32 context_id, const ik_hi_anti_aliasing_t *p_hi_anti_aliasing)
{
    amba_ik_system_print_str_5("[IK] set_hi_anti_aliasing is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //TBD
    }
    if (p_hi_anti_aliasing == NULL) {
        //TBD
    }
    return IK_ERR_0004;
}

uint32 img_ctx_set_hi_anti_aliasing(uint32 context_id, const ik_hi_anti_aliasing_t *p_hi_anti_aliasing)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;
    const ik_debug_check_func_t* p_check_func = ik_ctx_get_debug_check_func();
    if(p_check_func != NULL) {
        rval = p_check_func->hi_anti_aliasing(p_hi_anti_aliasing, "hi_anti_aliasing");
    }
    if(rval == IK_OK) {
        rval |= img_ctx_get_context(context_id, &p_ctx);
        if((p_hi_anti_aliasing != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
            (void)amba_ik_system_lock_mutex(context_id, IK_WAIT_FOREVER);
            (void)amba_ik_system_memcpy(&p_ctx->filters.p_hiso_input_param->hi_anti_aliasing, p_hi_anti_aliasing, sizeof(ik_hi_anti_aliasing_t));
            ctx_raise_update_flag(&p_ctx->filters.update_flags.liso.hi_anti_aliasing_updated);
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

uint32 img_ctx_ivd_get_hi_anti_aliasing(uint32 context_id, ik_hi_anti_aliasing_t *p_hi_anti_aliasing)
{
    amba_ik_system_print_str_5("[IK] get_hi_anti_aliasing is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //TBD
    }
    if (p_hi_anti_aliasing == NULL) {
        //TBD
    } else {
        (void) amba_ik_system_memcpy(p_hi_anti_aliasing, p_hi_anti_aliasing, sizeof(uint8));
    }
    return IK_ERR_0004;
}

uint32 img_ctx_get_hi_anti_aliasing(uint32 context_id, ik_hi_anti_aliasing_t *p_hi_anti_aliasing)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval |= img_ctx_get_context(context_id, &p_ctx);
    if((p_hi_anti_aliasing != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        (void)amba_ik_system_lock_mutex(context_id, IK_WAIT_FOREVER);
        (void)amba_ik_system_memcpy(p_hi_anti_aliasing, &p_ctx->filters.p_hiso_input_param->hi_anti_aliasing, sizeof(ik_hi_anti_aliasing_t));
        (void)amba_ik_system_unlock_mutex(context_id);
    } else {
        if(p_hi_anti_aliasing == NULL) {
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

uint32 img_ctx_ivd_set_hi_cfa_leakage_filter(uint32 context_id, const ik_hi_cfa_leakage_filter_t *p_hi_cfa_leakage_filter)
{
    amba_ik_system_print_str_5("[IK] set_hi_cfa_leakage_filter is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //MISRA
    }
    if (p_hi_cfa_leakage_filter == NULL) {
        //MISRA
    }
    return IK_ERR_0004;
}

uint32 img_ctx_set_hi_cfa_leakage_filter(uint32 context_id, const ik_hi_cfa_leakage_filter_t *p_hi_cfa_leakage_filter)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;
    const ik_debug_check_func_t* p_check_func = ik_ctx_get_debug_check_func();
    if(p_check_func != NULL) {
        rval = p_check_func->hi_cfa_leakage_filter(p_hi_cfa_leakage_filter, "hi_cfa_leakage_filter");
    }

    if(rval == IK_OK) {
        rval |= img_ctx_get_context(context_id, &p_ctx);
        if((p_hi_cfa_leakage_filter != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
            (void)amba_ik_system_lock_mutex(context_id, IK_WAIT_FOREVER);
            (void)amba_ik_system_memcpy(&p_ctx->filters.p_hiso_input_param->hi_cfa_leakage_filter, p_hi_cfa_leakage_filter, sizeof(ik_hi_cfa_leakage_filter_t));
            ctx_raise_update_flag(&p_ctx->filters.update_flags.liso.hi_cfa_leakage_filter_updated);
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

uint32 img_ctx_ivd_get_hi_cfa_leakage_filter(uint32 context_id, ik_hi_cfa_leakage_filter_t *p_hi_cfa_leakage_filter)
{
    amba_ik_system_print_str_5("[IK] get_hi_cfa_leakage_filter is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //TBD
    }
    if (p_hi_cfa_leakage_filter==NULL) {
        //TBD
    } else {
        (void)amba_ik_system_memcpy(p_hi_cfa_leakage_filter, p_hi_cfa_leakage_filter, sizeof(uint8));
    }
    return IK_ERR_0004;
}

uint32 img_ctx_get_hi_cfa_leakage_filter(uint32 context_id, ik_hi_cfa_leakage_filter_t *p_hi_cfa_leakage_filter)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval |= img_ctx_get_context(context_id, &p_ctx);
    if((p_hi_cfa_leakage_filter != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        (void)amba_ik_system_lock_mutex(context_id, IK_WAIT_FOREVER);
        (void)amba_ik_system_memcpy(p_hi_cfa_leakage_filter, &p_ctx->filters.p_hiso_input_param->hi_cfa_leakage_filter, sizeof(ik_hi_cfa_leakage_filter_t));
        (void)amba_ik_system_unlock_mutex(context_id);
    } else {
        if(p_hi_cfa_leakage_filter == NULL) {
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

uint32 img_ctx_ivd_set_hi_dynamic_bpc(uint32 context_id, const ik_hi_dynamic_bpc_t *p_hi_dynamic_bpc)
{
    amba_ik_system_print_str_5("[IK] set_hi_dynamic_bpc is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //MISRAC
    }
    if (p_hi_dynamic_bpc==NULL) {
        //MISRAC
    }
    return IK_ERR_0004;
}

uint32 img_ctx_set_hi_dynamic_bpc(uint32 context_id, const ik_hi_dynamic_bpc_t *p_hi_dynamic_bpc)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;
    const ik_debug_check_func_t* p_check_func = ik_ctx_get_debug_check_func();
    if(p_check_func != NULL) {
        rval = p_check_func->hi_dynamic_bpc(p_hi_dynamic_bpc, "hi_dynamic_bpc");
    }
    if(rval == IK_OK) {
        rval |= img_ctx_get_context(context_id, &p_ctx);
        if((p_hi_dynamic_bpc != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
            (void)amba_ik_system_lock_mutex(context_id, IK_WAIT_FOREVER);
            (void)amba_ik_system_memcpy(&p_ctx->filters.p_hiso_input_param->hi_dynamic_bpc, p_hi_dynamic_bpc, sizeof(ik_hi_dynamic_bpc_t));
            ctx_raise_update_flag(&p_ctx->filters.update_flags.liso.hi_dynamic_bpc_updated);
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

uint32 img_ctx_ivd_get_hi_dynamic_bpc(uint32 context_id, ik_hi_dynamic_bpc_t *p_hi_dynamic_bpc)
{
    amba_ik_system_print_str_5("[IK] get_hi_dynamic_bpc is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //TBD
    }
    if (p_hi_dynamic_bpc == NULL) {
        //TBD
    } else {
        (void)amba_ik_system_memcpy(p_hi_dynamic_bpc, p_hi_dynamic_bpc, sizeof(uint8));
    }
    return IK_ERR_0004;
}

uint32 img_ctx_get_hi_dynamic_bpc(uint32 context_id, ik_hi_dynamic_bpc_t *p_hi_dynamic_bpc)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval |= img_ctx_get_context(context_id, &p_ctx);
    if((p_hi_dynamic_bpc != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        (void)amba_ik_system_lock_mutex(context_id, IK_WAIT_FOREVER);
        (void)amba_ik_system_memcpy(p_hi_dynamic_bpc, &p_ctx->filters.p_hiso_input_param->hi_dynamic_bpc, sizeof(ik_hi_dynamic_bpc_t));
        (void)amba_ik_system_unlock_mutex(context_id);
    } else {
        if(p_hi_dynamic_bpc == NULL) {
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

uint32 img_ctx_ivd_set_hi_grgb_mismatch(uint32 context_id, const ik_hi_grgb_mismatch_t *p_hi_grgb_mismatch)
{
    uint32 rval = IK_ERR_0004;
    amba_ik_system_print_str_5("[IK] set_hi_grgb_mismatch is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id > img_arch_get_context_number()) {
        // misraC
    }
    if (p_hi_grgb_mismatch==NULL) {
        // misraC
    }
    return rval;
}

uint32 img_ctx_set_hi_grgb_mismatch(uint32 context_id, const ik_hi_grgb_mismatch_t *p_hi_grgb_mismatch)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;
    const ik_debug_check_func_t* p_check_func = ik_ctx_get_debug_check_func();
    if(p_check_func != NULL) {
        rval = p_check_func->hi_grgb_mismatch(p_hi_grgb_mismatch, "hi_grgb_mismatch");
    }
    if(rval == IK_OK) {
        rval |= img_ctx_get_context(context_id, &p_ctx);
        if((p_hi_grgb_mismatch != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
            (void)amba_ik_system_lock_mutex(context_id, IK_WAIT_FOREVER);
            (void)amba_ik_system_memcpy(&p_ctx->filters.p_hiso_input_param->hi_grgb_mismatch, p_hi_grgb_mismatch, sizeof(ik_hi_grgb_mismatch_t));
            ctx_raise_update_flag(&p_ctx->filters.update_flags.liso.hi_grgb_mismatch_updated);
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

uint32 img_ctx_ivd_get_hi_grgb_mismatch(uint32 context_id, ik_hi_grgb_mismatch_t *p_hi_grgb_mismatch)
{
    amba_ik_system_print_str_5("[IK] grgb_mismatch is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //TBD
    }
    if (p_hi_grgb_mismatch == NULL) {
        //TBD
    } else {
        (void)amba_ik_system_memcpy(p_hi_grgb_mismatch, p_hi_grgb_mismatch, sizeof(uint8));
    }
    return IK_ERR_0004;
}

uint32 img_ctx_get_hi_grgb_mismatch(uint32 context_id, ik_hi_grgb_mismatch_t *p_hi_grgb_mismatch)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval |= img_ctx_get_context(context_id, &p_ctx);
    if((p_hi_grgb_mismatch != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        (void)amba_ik_system_lock_mutex(context_id, IK_WAIT_FOREVER);
        (void)amba_ik_system_memcpy(p_hi_grgb_mismatch, &p_ctx->filters.p_hiso_input_param->hi_grgb_mismatch, sizeof(ik_hi_grgb_mismatch_t));
        (void)amba_ik_system_unlock_mutex(context_id);
    } else {
        if(p_hi_grgb_mismatch == NULL) {
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

uint32 img_ctx_ivd_set_hi_chroma_median_filter(uint32 context_id, const ik_hi_chroma_median_filter_t *p_hi_chroma_median_filter)
{
    amba_ik_system_print_str_5("[IK] set_hi_chroma_median_filter is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //TBD
    }
    if (p_hi_chroma_median_filter==NULL) {
        //TBD
    }
    return IK_ERR_0004;
}

uint32 img_ctx_set_hi_chroma_median_filter(uint32 context_id, const ik_hi_chroma_median_filter_t *p_hi_chroma_median_filter)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;
    const ik_debug_check_func_t* p_check_func = ik_ctx_get_debug_check_func();
    if(p_check_func != NULL) {
        rval = p_check_func->hi_chroma_median_filter(p_hi_chroma_median_filter, "hi_chroma_median_filter");
    }
    if(rval == IK_OK) {
        rval |= img_ctx_get_context(context_id, &p_ctx);
        if((p_hi_chroma_median_filter != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
            (void)amba_ik_system_lock_mutex(context_id, IK_WAIT_FOREVER);
            (void)amba_ik_system_memcpy(&p_ctx->filters.p_hiso_input_param->hi_chroma_median_filter, p_hi_chroma_median_filter, sizeof(ik_hi_chroma_median_filter_t));
            ctx_raise_update_flag(&p_ctx->filters.update_flags.liso.hi_chroma_median_filter_updated);
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

uint32 img_ctx_ivd_get_hi_chroma_median_filter(uint32 context_id, ik_hi_chroma_median_filter_t *p_hi_chroma_median_filter)
{
    amba_ik_system_print_str_5("[IK] get_hi_chroma_median_filter is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //TBD
    }
    if (p_hi_chroma_median_filter == NULL) {
        //TBD
    } else {
        (void)amba_ik_system_memcpy(p_hi_chroma_median_filter, p_hi_chroma_median_filter, sizeof(uint8));
    }
    return IK_ERR_0004;
}

uint32 img_ctx_get_hi_chroma_median_filter(uint32 context_id, ik_hi_chroma_median_filter_t *p_hi_chroma_median_filter)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval |= img_ctx_get_context(context_id, &p_ctx);

    if((p_hi_chroma_median_filter != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        (void)amba_ik_system_lock_mutex(context_id, IK_WAIT_FOREVER);
        (void)amba_ik_system_memcpy(p_hi_chroma_median_filter, &p_ctx->filters.p_hiso_input_param->hi_chroma_median_filter, sizeof(ik_hi_chroma_median_filter_t));
        (void)amba_ik_system_unlock_mutex(context_id);
    } else {
        if(p_hi_chroma_median_filter == NULL) {
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

uint32 img_ctx_ivd_set_hi_cfa_noise_filter(uint32 context_id, const ik_hi_cfa_noise_filter_t *p_hi_cfa_noise_filter)
{
    amba_ik_system_print_str_5("[IK] set_hi_cfa_noise_filter is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //TBD
    }
    if (p_hi_cfa_noise_filter == NULL) {
        //TBD
    }
    return IK_ERR_0004;
}

uint32 img_ctx_set_hi_cfa_noise_filter(uint32 context_id, const ik_hi_cfa_noise_filter_t *p_hi_cfa_noise_filter)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;
    const ik_debug_check_func_t* p_check_func = ik_ctx_get_debug_check_func();
    if(p_check_func != NULL) {
        rval = p_check_func->hi_cfa_noise_filter(p_hi_cfa_noise_filter, "hi_cfa_noise_filter");
    }
    if(rval == IK_OK) {
        rval |= img_ctx_get_context(context_id, &p_ctx);
        if((p_hi_cfa_noise_filter != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
            (void)amba_ik_system_lock_mutex(context_id, IK_WAIT_FOREVER);
            (void)amba_ik_system_memcpy(&p_ctx->filters.p_hiso_input_param->hi_cfa_noise_filter, p_hi_cfa_noise_filter, sizeof(ik_hi_cfa_noise_filter_t));
            ctx_raise_update_flag(&p_ctx->filters.update_flags.liso.hi_cfa_noise_filter_updated);
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

uint32 img_ctx_ivd_get_hi_cfa_noise_filter(uint32 context_id, ik_hi_cfa_noise_filter_t *p_hi_cfa_noise_filter)
{
    amba_ik_system_print_str_5("[IK] get_hi_cfa_noise_filter is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //TBD
    }
    if (p_hi_cfa_noise_filter == NULL) {
        //TBD
    } else {
        (void)amba_ik_system_memcpy(p_hi_cfa_noise_filter, p_hi_cfa_noise_filter, sizeof(uint8));
    }
    return IK_ERR_0004;
}

uint32 img_ctx_get_hi_cfa_noise_filter(uint32 context_id, ik_hi_cfa_noise_filter_t *p_hi_cfa_noise_filter)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval |= img_ctx_get_context(context_id, &p_ctx);
    if((p_hi_cfa_noise_filter != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        (void)amba_ik_system_lock_mutex(context_id, IK_WAIT_FOREVER);
        (void)amba_ik_system_memcpy(p_hi_cfa_noise_filter, &p_ctx->filters.p_hiso_input_param->hi_cfa_noise_filter, sizeof(ik_hi_cfa_noise_filter_t));
        (void)amba_ik_system_unlock_mutex(context_id);
    } else {
        if(p_hi_cfa_noise_filter == NULL) {
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

uint32 img_ctx_ivd_set_hi_demosaic(uint32 context_id, const ik_hi_demosaic_t *p_hi_demosaic)
{
    amba_ik_system_print_str_5("[IK] set_hi_demosaic is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //TBD
    }
    if (p_hi_demosaic == NULL) {
        //TBD
    }
    return IK_ERR_0004;
}

uint32 img_ctx_set_hi_demosaic(uint32 context_id, const ik_hi_demosaic_t *p_hi_demosaic)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;
    const ik_debug_check_func_t* p_check_func = ik_ctx_get_debug_check_func();
    if(p_check_func != NULL) {
        rval = p_check_func->hi_demosaic(p_hi_demosaic, "hi_demosaic");
    }
    if(rval == IK_OK) {
        rval |= img_ctx_get_context(context_id, &p_ctx);
        if((p_hi_demosaic != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
            (void)amba_ik_system_lock_mutex(context_id, IK_WAIT_FOREVER);
            (void)amba_ik_system_memcpy(&p_ctx->filters.p_hiso_input_param->hi_demosaic, p_hi_demosaic, sizeof(ik_hi_demosaic_t));
            ctx_raise_update_flag(&p_ctx->filters.update_flags.liso.hi_demosaic_updated);
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

uint32 img_ctx_ivd_get_hi_demosaic(uint32 context_id, ik_hi_demosaic_t *p_hi_demosaic)
{
    amba_ik_system_print_str_5("[IK] get_hi_demosaic is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //TBD
    }
    if (p_hi_demosaic == NULL) {
        //TBD
    } else {
        (void)amba_ik_system_memcpy(p_hi_demosaic, p_hi_demosaic, sizeof(uint8));
    }
    return IK_ERR_0004;
}

uint32 img_ctx_get_hi_demosaic(uint32 context_id, ik_hi_demosaic_t *p_hi_demosaic)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval |= img_ctx_get_context(context_id, &p_ctx);
    if((p_hi_demosaic != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        (void)amba_ik_system_lock_mutex(context_id, IK_WAIT_FOREVER);
        (void)amba_ik_system_memcpy(p_hi_demosaic, &p_ctx->filters.p_hiso_input_param->hi_demosaic, sizeof(ik_hi_demosaic_t));
        (void)amba_ik_system_unlock_mutex(context_id);
    } else {
        if(p_hi_demosaic == NULL) {
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

// Hiso Low2
uint32 img_ctx_ivd_set_li2_anti_aliasing(uint32 context_id, const ik_li2_anti_aliasing_t *p_li2_anti_aliasing)
{
    amba_ik_system_print_str_5("[IK] set_li2_anti_aliasing is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //TBD
    }
    if (p_li2_anti_aliasing == NULL) {
        //TBD
    }
    return IK_ERR_0004;
}

uint32 img_ctx_set_li2_anti_aliasing(uint32 context_id, const ik_li2_anti_aliasing_t *p_li2_anti_aliasing)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;
    const ik_debug_check_func_t* p_check_func = ik_ctx_get_debug_check_func();
    if(p_check_func != NULL) {
        rval = p_check_func->li2_anti_aliasing(p_li2_anti_aliasing, "li2_anti_aliasing");
    }
    if(rval == IK_OK) {
        rval |= img_ctx_get_context(context_id, &p_ctx);
        if((p_li2_anti_aliasing != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
            (void)amba_ik_system_lock_mutex(context_id, IK_WAIT_FOREVER);
            (void)amba_ik_system_memcpy(&p_ctx->filters.p_hiso_input_param->li2_anti_aliasing, p_li2_anti_aliasing, sizeof(ik_li2_anti_aliasing_t));
            ctx_raise_update_flag(&p_ctx->filters.update_flags.liso.li2_anti_aliasing_updated);
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

uint32 img_ctx_ivd_get_li2_anti_aliasing(uint32 context_id, ik_li2_anti_aliasing_t *p_li2_anti_aliasing)
{
    amba_ik_system_print_str_5("[IK] get_li2_anti_aliasing is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //TBD
    }
    if (p_li2_anti_aliasing == NULL) {
        //TBD
    } else {
        (void) amba_ik_system_memcpy(p_li2_anti_aliasing, p_li2_anti_aliasing, sizeof(uint8));
    }
    return IK_ERR_0004;
}

uint32 img_ctx_get_li2_anti_aliasing(uint32 context_id, ik_li2_anti_aliasing_t *p_li2_anti_aliasing)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval |= img_ctx_get_context(context_id, &p_ctx);
    if((p_li2_anti_aliasing != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        (void)amba_ik_system_lock_mutex(context_id, IK_WAIT_FOREVER);
        (void)amba_ik_system_memcpy(p_li2_anti_aliasing, &p_ctx->filters.p_hiso_input_param->li2_anti_aliasing, sizeof(ik_li2_anti_aliasing_t));
        (void)amba_ik_system_unlock_mutex(context_id);
    } else {
        if(p_li2_anti_aliasing == NULL) {
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

uint32 img_ctx_ivd_set_li2_cfa_leakage_filter(uint32 context_id, const ik_li2_cfa_leakage_filter_t *p_li2_cfa_leakage_filter)
{
    amba_ik_system_print_str_5("[IK] set_li2_cfa_leakage_filter is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //MISRA
    }
    if (p_li2_cfa_leakage_filter == NULL) {
        //MISRA
    }
    return IK_ERR_0004;
}

uint32 img_ctx_set_li2_cfa_leakage_filter(uint32 context_id, const ik_li2_cfa_leakage_filter_t *p_li2_cfa_leakage_filter)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;
    const ik_debug_check_func_t* p_check_func = ik_ctx_get_debug_check_func();
    if(p_check_func != NULL) {
        rval = p_check_func->li2_cfa_leakage_filter(p_li2_cfa_leakage_filter, "li2_cfa_leakage_filter");
    }

    if(rval == IK_OK) {
        rval |= img_ctx_get_context(context_id, &p_ctx);
        if((p_li2_cfa_leakage_filter != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
            (void)amba_ik_system_lock_mutex(context_id, IK_WAIT_FOREVER);
            (void)amba_ik_system_memcpy(&p_ctx->filters.p_hiso_input_param->li2_cfa_leakage_filter, p_li2_cfa_leakage_filter, sizeof(ik_li2_cfa_leakage_filter_t));
            ctx_raise_update_flag(&p_ctx->filters.update_flags.liso.li2_cfa_leakage_filter_updated);
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

uint32 img_ctx_ivd_get_li2_cfa_leakage_filter(uint32 context_id, ik_li2_cfa_leakage_filter_t *p_li2_cfa_leakage_filter)
{
    amba_ik_system_print_str_5("[IK] get_li2_cfa_leakage_filter is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //TBD
    }
    if (p_li2_cfa_leakage_filter==NULL) {
        //TBD
    } else {
        (void)amba_ik_system_memcpy(p_li2_cfa_leakage_filter, p_li2_cfa_leakage_filter, sizeof(uint8));
    }
    return IK_ERR_0004;
}

uint32 img_ctx_get_li2_cfa_leakage_filter(uint32 context_id, ik_li2_cfa_leakage_filter_t *p_li2_cfa_leakage_filter)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval |= img_ctx_get_context(context_id, &p_ctx);
    if((p_li2_cfa_leakage_filter != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        (void)amba_ik_system_lock_mutex(context_id, IK_WAIT_FOREVER);
        (void)amba_ik_system_memcpy(p_li2_cfa_leakage_filter, &p_ctx->filters.p_hiso_input_param->li2_cfa_leakage_filter, sizeof(ik_li2_cfa_leakage_filter_t));
        (void)amba_ik_system_unlock_mutex(context_id);
    } else {
        if(p_li2_cfa_leakage_filter == NULL) {
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

uint32 img_ctx_ivd_set_li2_dynamic_bpc(uint32 context_id, const ik_li2_dynamic_bpc_t *p_li2_dynamic_bpc)
{
    amba_ik_system_print_str_5("[IK] set_li2_dynamic_bpc is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //MISRAC
    }
    if (p_li2_dynamic_bpc==NULL) {
        //MISRAC
    }
    return IK_ERR_0004;
}

uint32 img_ctx_set_li2_dynamic_bpc(uint32 context_id, const ik_li2_dynamic_bpc_t *p_li2_dynamic_bpc)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;
    const ik_debug_check_func_t* p_check_func = ik_ctx_get_debug_check_func();
    if(p_check_func != NULL) {
        rval = p_check_func->li2_dynamic_bpc(p_li2_dynamic_bpc, "li2_dynamic_bpc");
    }
    if(rval == IK_OK) {
        rval |= img_ctx_get_context(context_id, &p_ctx);
        if((p_li2_dynamic_bpc != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
            (void)amba_ik_system_lock_mutex(context_id, IK_WAIT_FOREVER);
            (void)amba_ik_system_memcpy(&p_ctx->filters.p_hiso_input_param->li2_dynamic_bpc, p_li2_dynamic_bpc, sizeof(ik_li2_dynamic_bpc_t));
            ctx_raise_update_flag(&p_ctx->filters.update_flags.liso.li2_dynamic_bpc_updated);
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

uint32 img_ctx_ivd_get_li2_dynamic_bpc(uint32 context_id, ik_li2_dynamic_bpc_t *p_li2_dynamic_bpc)
{
    amba_ik_system_print_str_5("[IK] get_li2_dynamic_bpc is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //TBD
    }
    if (p_li2_dynamic_bpc == NULL) {
        //TBD
    } else {
        (void)amba_ik_system_memcpy(p_li2_dynamic_bpc, p_li2_dynamic_bpc, sizeof(uint8));
    }
    return IK_ERR_0004;
}

uint32 img_ctx_get_li2_dynamic_bpc(uint32 context_id, ik_li2_dynamic_bpc_t *p_li2_dynamic_bpc)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval |= img_ctx_get_context(context_id, &p_ctx);
    if((p_li2_dynamic_bpc != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        (void)amba_ik_system_lock_mutex(context_id, IK_WAIT_FOREVER);
        (void)amba_ik_system_memcpy(p_li2_dynamic_bpc, &p_ctx->filters.p_hiso_input_param->li2_dynamic_bpc, sizeof(ik_li2_dynamic_bpc_t));
        (void)amba_ik_system_unlock_mutex(context_id);
    } else {
        if(p_li2_dynamic_bpc == NULL) {
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

uint32 img_ctx_ivd_set_li2_grgb_mismatch(uint32 context_id, const ik_li2_grgb_mismatch_t *p_li2_grgb_mismatch)
{
    uint32 rval = IK_ERR_0004;
    amba_ik_system_print_str_5("[IK] set_li2_grgb_mismatch is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id > img_arch_get_context_number()) {
        // misraC
    }
    if (p_li2_grgb_mismatch==NULL) {
        // misraC
    }
    return rval;
}

uint32 img_ctx_set_li2_grgb_mismatch(uint32 context_id, const ik_li2_grgb_mismatch_t *p_li2_grgb_mismatch)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;
    const ik_debug_check_func_t* p_check_func = ik_ctx_get_debug_check_func();
    if(p_check_func != NULL) {
        rval = p_check_func->li2_grgb_mismatch(p_li2_grgb_mismatch, "li2_grgb_mismatch");
    }
    if(rval == IK_OK) {
        rval |= img_ctx_get_context(context_id, &p_ctx);
        if((p_li2_grgb_mismatch != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
            (void)amba_ik_system_lock_mutex(context_id, IK_WAIT_FOREVER);
            (void)amba_ik_system_memcpy(&p_ctx->filters.p_hiso_input_param->li2_grgb_mismatch, p_li2_grgb_mismatch, sizeof(ik_li2_grgb_mismatch_t));
            ctx_raise_update_flag(&p_ctx->filters.update_flags.liso.li2_grgb_mismatch_updated);
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

uint32 img_ctx_ivd_get_li2_grgb_mismatch(uint32 context_id, ik_li2_grgb_mismatch_t *p_li2_grgb_mismatch)
{
    amba_ik_system_print_str_5("[IK] grgb_mismatch is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //TBD
    }
    if (p_li2_grgb_mismatch == NULL) {
        //TBD
    } else {
        (void)amba_ik_system_memcpy(p_li2_grgb_mismatch, p_li2_grgb_mismatch, sizeof(uint8));
    }
    return IK_ERR_0004;
}

uint32 img_ctx_get_li2_grgb_mismatch(uint32 context_id, ik_li2_grgb_mismatch_t *p_li2_grgb_mismatch)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval |= img_ctx_get_context(context_id, &p_ctx);
    if((p_li2_grgb_mismatch != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        (void)amba_ik_system_lock_mutex(context_id, IK_WAIT_FOREVER);
        (void)amba_ik_system_memcpy(p_li2_grgb_mismatch, &p_ctx->filters.p_hiso_input_param->li2_grgb_mismatch, sizeof(ik_li2_grgb_mismatch_t));
        (void)amba_ik_system_unlock_mutex(context_id);
    } else {
        if(p_li2_grgb_mismatch == NULL) {
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

uint32 img_ctx_ivd_set_li2_cfa_noise_filter(uint32 context_id, const ik_li2_cfa_noise_filter_t *p_li2_cfa_noise_filter)
{
    amba_ik_system_print_str_5("[IK] set_li2_cfa_noise_filter is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //TBD
    }
    if (p_li2_cfa_noise_filter == NULL) {
        //TBD
    }
    return IK_ERR_0004;
}

uint32 img_ctx_set_li2_cfa_noise_filter(uint32 context_id, const ik_li2_cfa_noise_filter_t *p_li2_cfa_noise_filter)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;
    const ik_debug_check_func_t* p_check_func = ik_ctx_get_debug_check_func();
    if(p_check_func != NULL) {
        rval = p_check_func->li2_cfa_noise_filter(p_li2_cfa_noise_filter, "li2_cfa_noise_filter");
    }
    if(rval == IK_OK) {
        rval |= img_ctx_get_context(context_id, &p_ctx);
        if((p_li2_cfa_noise_filter != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
            (void)amba_ik_system_lock_mutex(context_id, IK_WAIT_FOREVER);
            (void)amba_ik_system_memcpy(&p_ctx->filters.p_hiso_input_param->li2_cfa_noise_filter, p_li2_cfa_noise_filter, sizeof(ik_li2_cfa_noise_filter_t));
            ctx_raise_update_flag(&p_ctx->filters.update_flags.liso.li2_cfa_noise_filter_updated);
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

uint32 img_ctx_ivd_get_li2_cfa_noise_filter(uint32 context_id, ik_li2_cfa_noise_filter_t *p_li2_cfa_noise_filter)
{
    amba_ik_system_print_str_5("[IK] get_li2_cfa_noise_filter is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //TBD
    }
    if (p_li2_cfa_noise_filter == NULL) {
        //TBD
    } else {
        (void)amba_ik_system_memcpy(p_li2_cfa_noise_filter, p_li2_cfa_noise_filter, sizeof(uint8));
    }
    return IK_ERR_0004;
}

uint32 img_ctx_get_li2_cfa_noise_filter(uint32 context_id, ik_li2_cfa_noise_filter_t *p_li2_cfa_noise_filter)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval |= img_ctx_get_context(context_id, &p_ctx);
    if((p_li2_cfa_noise_filter != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        (void)amba_ik_system_lock_mutex(context_id, IK_WAIT_FOREVER);
        (void)amba_ik_system_memcpy(p_li2_cfa_noise_filter, &p_ctx->filters.p_hiso_input_param->li2_cfa_noise_filter, sizeof(ik_li2_cfa_noise_filter_t));
        (void)amba_ik_system_unlock_mutex(context_id);
    } else {
        if(p_li2_cfa_noise_filter == NULL) {
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

uint32 img_ctx_ivd_set_li2_demosaic(uint32 context_id, const ik_li2_demosaic_t *p_li2_demosaic)
{
    amba_ik_system_print_str_5("[IK] set_li2_demosaic is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //TBD
    }
    if (p_li2_demosaic == NULL) {
        //TBD
    }
    return IK_ERR_0004;
}

uint32 img_ctx_set_li2_demosaic(uint32 context_id, const ik_li2_demosaic_t *p_li2_demosaic)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;
    const ik_debug_check_func_t* p_check_func = ik_ctx_get_debug_check_func();
    if(p_check_func != NULL) {
        rval = p_check_func->li2_demosaic(p_li2_demosaic, "li2_demosaic");
    }
    if(rval == IK_OK) {
        rval |= img_ctx_get_context(context_id, &p_ctx);
        if((p_li2_demosaic != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
            (void)amba_ik_system_lock_mutex(context_id, IK_WAIT_FOREVER);
            (void)amba_ik_system_memcpy(&p_ctx->filters.p_hiso_input_param->li2_demosaic, p_li2_demosaic, sizeof(ik_li2_demosaic_t));
            ctx_raise_update_flag(&p_ctx->filters.update_flags.liso.li2_demosaic_updated);
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

uint32 img_ctx_ivd_get_li2_demosaic(uint32 context_id, ik_li2_demosaic_t *p_li2_demosaic)
{
    amba_ik_system_print_str_5("[IK] get_li2_demosaic is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //TBD
    }
    if (p_li2_demosaic == NULL) {
        //TBD
    } else {
        (void)amba_ik_system_memcpy(p_li2_demosaic, p_li2_demosaic, sizeof(uint8));
    }
    return IK_ERR_0004;
}

uint32 img_ctx_get_li2_demosaic(uint32 context_id, ik_li2_demosaic_t *p_li2_demosaic)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval |= img_ctx_get_context(context_id, &p_ctx);
    if((p_li2_demosaic != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        (void)amba_ik_system_lock_mutex(context_id, IK_WAIT_FOREVER);
        (void)amba_ik_system_memcpy(p_li2_demosaic, &p_ctx->filters.p_hiso_input_param->li2_demosaic, sizeof(ik_li2_demosaic_t));
        (void)amba_ik_system_unlock_mutex(context_id);
    } else {
        if(p_li2_demosaic == NULL) {
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

uint32 img_ctx_ivd_set_hi_asf(uint32 context_id, const ik_hi_asf_t *p_hi_asf)
{
    amba_ik_system_print_str_5("[IK] set_hi_asf is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //TBD
    }
    if (p_hi_asf==NULL) {
        //TBD
    }
    return IK_ERR_0004;
}

uint32 img_ctx_set_hi_asf(uint32 context_id, const ik_hi_asf_t *p_hi_asf)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;
    const ik_debug_check_func_t* p_check_func = ik_ctx_get_debug_check_func();
    if(p_check_func != NULL) {
        rval = p_check_func->hi_asf(p_hi_asf, "hi_asf");
    }
    if(rval == IK_OK) {
        rval |= img_ctx_get_context(context_id, &p_ctx);
        if((p_hi_asf != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
            (void)amba_ik_system_lock_mutex(context_id, IK_WAIT_FOREVER);
            (void)amba_ik_system_memcpy(&p_ctx->filters.p_hiso_input_param->hi_asf, p_hi_asf, sizeof(ik_hi_asf_t));
            ctx_raise_update_flag(&p_ctx->filters.update_flags.liso.hi_asf_updated);
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

uint32 img_ctx_ivd_get_hi_asf(uint32 context_id, ik_hi_asf_t *p_hi_asf)
{
    amba_ik_system_print_str_5("[IK] get_hi_asf is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //TBD
    }
    if (p_hi_asf == NULL) {
        //TBD
    } else {
        (void)amba_ik_system_memcpy(p_hi_asf, p_hi_asf, sizeof(uint8));
    }
    return IK_ERR_0004;
}

uint32 img_ctx_get_hi_asf(uint32 context_id, ik_hi_asf_t *p_hi_asf)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval |= img_ctx_get_context(context_id, &p_ctx);
    if((p_hi_asf != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        (void)amba_ik_system_lock_mutex(context_id, IK_WAIT_FOREVER);
        (void)amba_ik_system_memcpy(p_hi_asf, &p_ctx->filters.p_hiso_input_param->hi_asf, sizeof(ik_hi_asf_t));
        (void)amba_ik_system_unlock_mutex(context_id);
    } else {
        if(p_hi_asf == NULL) {
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

uint32 img_ctx_ivd_set_li2_asf(uint32 context_id, const ik_li2_asf_t *p_li2_asf)
{
    amba_ik_system_print_str_5("[IK] set_li2_asf is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //TBD
    }
    if (p_li2_asf==NULL) {
        //TBD
    }
    return IK_ERR_0004;
}

uint32 img_ctx_set_li2_asf(uint32 context_id, const ik_li2_asf_t *p_li2_asf)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;
    const ik_debug_check_func_t* p_check_func = ik_ctx_get_debug_check_func();
    if(p_check_func != NULL) {
        rval = p_check_func->li2_asf(p_li2_asf, "li2_asf");
    }
    if(rval == IK_OK) {
        rval |= img_ctx_get_context(context_id, &p_ctx);
        if((p_li2_asf != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
            (void)amba_ik_system_lock_mutex(context_id, IK_WAIT_FOREVER);
            (void)amba_ik_system_memcpy(&p_ctx->filters.p_hiso_input_param->li2_asf, p_li2_asf, sizeof(ik_li2_asf_t));
            ctx_raise_update_flag(&p_ctx->filters.update_flags.liso.li2_asf_updated);
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

uint32 img_ctx_ivd_get_li2_asf(uint32 context_id, ik_li2_asf_t *p_li2_asf)
{
    amba_ik_system_print_str_5("[IK] get_li2_asf is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //TBD
    }
    if (p_li2_asf == NULL) {
        //TBD
    } else {
        (void)amba_ik_system_memcpy(p_li2_asf, p_li2_asf, sizeof(uint8));
    }
    return IK_ERR_0004;
}

uint32 img_ctx_get_li2_asf(uint32 context_id, ik_li2_asf_t *p_li2_asf)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval |= img_ctx_get_context(context_id, &p_ctx);
    if((p_li2_asf != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        (void)amba_ik_system_lock_mutex(context_id, IK_WAIT_FOREVER);
        (void)amba_ik_system_memcpy(p_li2_asf, &p_ctx->filters.p_hiso_input_param->li2_asf, sizeof(ik_li2_asf_t));
        (void)amba_ik_system_unlock_mutex(context_id);
    } else {
        if(p_li2_asf == NULL) {
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

uint32 img_ctx_ivd_set_hi_low_asf(uint32 context_id, const ik_hi_low_asf_t *p_hi_low_asf)
{
    amba_ik_system_print_str_5("[IK] set_hi_low_asf is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //TBD
    }
    if (p_hi_low_asf==NULL) {
        //TBD
    }
    return IK_ERR_0004;
}

uint32 img_ctx_set_hi_low_asf(uint32 context_id, const ik_hi_low_asf_t *p_hi_low_asf)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;
    const ik_debug_check_func_t* p_check_func = ik_ctx_get_debug_check_func();
    if(p_check_func != NULL) {
        rval = p_check_func->hi_low_asf(p_hi_low_asf, "hi_low_asf");
    }
    if(rval == IK_OK) {
        rval |= img_ctx_get_context(context_id, &p_ctx);
        if((p_hi_low_asf != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
            (void)amba_ik_system_lock_mutex(context_id, IK_WAIT_FOREVER);
            (void)amba_ik_system_memcpy(&p_ctx->filters.p_hiso_input_param->hi_low_asf, p_hi_low_asf, sizeof(ik_hi_low_asf_t));
            ctx_raise_update_flag(&p_ctx->filters.update_flags.liso.hi_low_asf_updated);
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

uint32 img_ctx_ivd_get_hi_low_asf(uint32 context_id, ik_hi_low_asf_t *p_hi_low_asf)
{
    amba_ik_system_print_str_5("[IK] get_hi_low_asf is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //TBD
    }
    if (p_hi_low_asf == NULL) {
        //TBD
    } else {
        (void)amba_ik_system_memcpy(p_hi_low_asf, p_hi_low_asf, sizeof(uint8));
    }
    return IK_ERR_0004;
}

uint32 img_ctx_get_hi_low_asf(uint32 context_id, ik_hi_low_asf_t *p_hi_low_asf)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval |= img_ctx_get_context(context_id, &p_ctx);
    if((p_hi_low_asf != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        (void)amba_ik_system_lock_mutex(context_id, IK_WAIT_FOREVER);
        (void)amba_ik_system_memcpy(p_hi_low_asf, &p_ctx->filters.p_hiso_input_param->hi_low_asf, sizeof(ik_hi_low_asf_t));
        (void)amba_ik_system_unlock_mutex(context_id);
    } else {
        if(p_hi_low_asf == NULL) {
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

uint32 img_ctx_ivd_set_hi_med1_asf(uint32 context_id, const ik_hi_med1_asf_t *p_hi_med1_asf)
{
    amba_ik_system_print_str_5("[IK] set_hi_med1_asf is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //TBD
    }
    if (p_hi_med1_asf==NULL) {
        //TBD
    }
    return IK_ERR_0004;
}

uint32 img_ctx_set_hi_med1_asf(uint32 context_id, const ik_hi_med1_asf_t *p_hi_med1_asf)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;
    const ik_debug_check_func_t* p_check_func = ik_ctx_get_debug_check_func();
    if(p_check_func != NULL) {
        rval = p_check_func->hi_med1_asf(p_hi_med1_asf, "hi_med1_asf");
    }
    if(rval == IK_OK) {
        rval |= img_ctx_get_context(context_id, &p_ctx);
        if((p_hi_med1_asf != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
            (void)amba_ik_system_lock_mutex(context_id, IK_WAIT_FOREVER);
            (void)amba_ik_system_memcpy(&p_ctx->filters.p_hiso_input_param->hi_med1_asf, p_hi_med1_asf, sizeof(ik_hi_med1_asf_t));
            ctx_raise_update_flag(&p_ctx->filters.update_flags.liso.hi_med1_asf_updated);
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

uint32 img_ctx_ivd_get_hi_med1_asf(uint32 context_id, ik_hi_med1_asf_t *p_hi_med1_asf)
{
    amba_ik_system_print_str_5("[IK] get_hi_med1_asf is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //TBD
    }
    if (p_hi_med1_asf == NULL) {
        //TBD
    } else {
        (void)amba_ik_system_memcpy(p_hi_med1_asf, p_hi_med1_asf, sizeof(uint8));
    }
    return IK_ERR_0004;
}

uint32 img_ctx_get_hi_med1_asf(uint32 context_id, ik_hi_med1_asf_t *p_hi_med1_asf)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval |= img_ctx_get_context(context_id, &p_ctx);
    if((p_hi_med1_asf != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        (void)amba_ik_system_lock_mutex(context_id, IK_WAIT_FOREVER);
        (void)amba_ik_system_memcpy(p_hi_med1_asf, &p_ctx->filters.p_hiso_input_param->hi_med1_asf, sizeof(ik_hi_med1_asf_t));
        (void)amba_ik_system_unlock_mutex(context_id);
    } else {
        if(p_hi_med1_asf == NULL) {
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

uint32 img_ctx_ivd_set_hi_med2_asf(uint32 context_id, const ik_hi_med2_asf_t *p_hi_med2_asf)
{
    amba_ik_system_print_str_5("[IK] set_hi_med2_asf is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //TBD
    }
    if (p_hi_med2_asf==NULL) {
        //TBD
    }
    return IK_ERR_0004;
}

uint32 img_ctx_set_hi_med2_asf(uint32 context_id, const ik_hi_med2_asf_t *p_hi_med2_asf)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;
    const ik_debug_check_func_t* p_check_func = ik_ctx_get_debug_check_func();
    if(p_check_func != NULL) {
        rval = p_check_func->hi_med2_asf(p_hi_med2_asf, "hi_med2_asf");
    }
    if(rval == IK_OK) {
        rval |= img_ctx_get_context(context_id, &p_ctx);
        if((p_hi_med2_asf != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
            (void)amba_ik_system_lock_mutex(context_id, IK_WAIT_FOREVER);
            (void)amba_ik_system_memcpy(&p_ctx->filters.p_hiso_input_param->hi_med2_asf, p_hi_med2_asf, sizeof(ik_hi_med2_asf_t));
            ctx_raise_update_flag(&p_ctx->filters.update_flags.liso.hi_med2_asf_updated);
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

uint32 img_ctx_ivd_get_hi_med2_asf(uint32 context_id, ik_hi_med2_asf_t *p_hi_med2_asf)
{
    amba_ik_system_print_str_5("[IK] get_hi_med2_asf is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //TBD
    }
    if (p_hi_med2_asf == NULL) {
        //TBD
    } else {
        (void)amba_ik_system_memcpy(p_hi_med2_asf, p_hi_med2_asf, sizeof(uint8));
    }
    return IK_ERR_0004;
}

uint32 img_ctx_get_hi_med2_asf(uint32 context_id, ik_hi_med2_asf_t *p_hi_med2_asf)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval |= img_ctx_get_context(context_id, &p_ctx);
    if((p_hi_med2_asf != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        (void)amba_ik_system_lock_mutex(context_id, IK_WAIT_FOREVER);
        (void)amba_ik_system_memcpy(p_hi_med2_asf, &p_ctx->filters.p_hiso_input_param->hi_med2_asf, sizeof(ik_hi_med2_asf_t));
        (void)amba_ik_system_unlock_mutex(context_id);
    } else {
        if(p_hi_med2_asf == NULL) {
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

uint32 img_ctx_ivd_set_hi_high_asf(uint32 context_id, const ik_hi_high_asf_t *p_hi_high_asf)
{
    amba_ik_system_print_str_5("[IK] set_hi_high_asf is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //TBD
    }
    if (p_hi_high_asf==NULL) {
        //TBD
    }
    return IK_ERR_0004;
}

uint32 img_ctx_set_hi_high_asf(uint32 context_id, const ik_hi_high_asf_t *p_hi_high_asf)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;
    const ik_debug_check_func_t* p_check_func = ik_ctx_get_debug_check_func();
    if(p_check_func != NULL) {
        rval = p_check_func->hi_high_asf(p_hi_high_asf, "hi_high_asf");
    }
    if(rval == IK_OK) {
        rval |= img_ctx_get_context(context_id, &p_ctx);
        if((p_hi_high_asf != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
            (void)amba_ik_system_lock_mutex(context_id, IK_WAIT_FOREVER);
            (void)amba_ik_system_memcpy(&p_ctx->filters.p_hiso_input_param->hi_high_asf, p_hi_high_asf, sizeof(ik_hi_high_asf_t));
            ctx_raise_update_flag(&p_ctx->filters.update_flags.liso.hi_high_asf_updated);
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

uint32 img_ctx_ivd_get_hi_high_asf(uint32 context_id, ik_hi_high_asf_t *p_hi_high_asf)
{
    amba_ik_system_print_str_5("[IK] get_hi_high_asf is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //TBD
    }
    if (p_hi_high_asf == NULL) {
        //TBD
    } else {
        (void)amba_ik_system_memcpy(p_hi_high_asf, p_hi_high_asf, sizeof(uint8));
    }
    return IK_ERR_0004;
}

uint32 img_ctx_get_hi_high_asf(uint32 context_id, ik_hi_high_asf_t *p_hi_high_asf)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval |= img_ctx_get_context(context_id, &p_ctx);
    if((p_hi_high_asf != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        (void)amba_ik_system_lock_mutex(context_id, IK_WAIT_FOREVER);
        (void)amba_ik_system_memcpy(p_hi_high_asf, &p_ctx->filters.p_hiso_input_param->hi_high_asf, sizeof(ik_hi_high_asf_t));
        (void)amba_ik_system_unlock_mutex(context_id);
    } else {
        if(p_hi_high_asf == NULL) {
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

uint32 img_ctx_ivd_set_hi_high2_asf(uint32 context_id, const ik_hi_high2_asf_t *p_hi_high2_asf)
{
    amba_ik_system_print_str_5("[IK] set_hi_high2_asf is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //TBD
    }
    if (p_hi_high2_asf==NULL) {
        //TBD
    }
    return IK_ERR_0004;
}

uint32 img_ctx_set_hi_high2_asf(uint32 context_id, const ik_hi_high2_asf_t *p_hi_high2_asf)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;
    const ik_debug_check_func_t* p_check_func = ik_ctx_get_debug_check_func();
    if(p_check_func != NULL) {
        rval = p_check_func->hi_high2_asf(p_hi_high2_asf, "hi_high2_asf");
    }
    if(rval == IK_OK) {
        rval |= img_ctx_get_context(context_id, &p_ctx);
        if((p_hi_high2_asf != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
            (void)amba_ik_system_lock_mutex(context_id, IK_WAIT_FOREVER);
            (void)amba_ik_system_memcpy(&p_ctx->filters.p_hiso_input_param->hi_high2_asf, p_hi_high2_asf, sizeof(ik_hi_high2_asf_t));
            ctx_raise_update_flag(&p_ctx->filters.update_flags.liso.hi_high2_asf_updated);
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

uint32 img_ctx_ivd_get_hi_high2_asf(uint32 context_id, ik_hi_high2_asf_t *p_hi_high2_asf)
{
    amba_ik_system_print_str_5("[IK] get_hi_high2_asf is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //TBD
    }
    if (p_hi_high2_asf == NULL) {
        //TBD
    } else {
        (void)amba_ik_system_memcpy(p_hi_high2_asf, p_hi_high2_asf, sizeof(uint8));
    }
    return IK_ERR_0004;
}

uint32 img_ctx_get_hi_high2_asf(uint32 context_id, ik_hi_high2_asf_t *p_hi_high2_asf)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval |= img_ctx_get_context(context_id, &p_ctx);
    if((p_hi_high2_asf != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        (void)amba_ik_system_lock_mutex(context_id, IK_WAIT_FOREVER);
        (void)amba_ik_system_memcpy(p_hi_high2_asf, &p_ctx->filters.p_hiso_input_param->hi_high2_asf, sizeof(ik_hi_high2_asf_t));
        (void)amba_ik_system_unlock_mutex(context_id);
    } else {
        if(p_hi_high2_asf == NULL) {
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

uint32 img_ctx_ivd_set_chroma_asf(uint32 context_id, const ik_chroma_asf_t *p_chroma_asf)
{
    amba_ik_system_print_str_5("[IK] set_chroma_asf is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //TBD
    }
    if (p_chroma_asf==NULL) {
        //TBD
    }
    return IK_ERR_0004;
}

uint32 img_ctx_set_chroma_asf(uint32 context_id, const ik_chroma_asf_t *p_chroma_asf)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;
    const ik_debug_check_func_t* p_check_func = ik_ctx_get_debug_check_func();
    if(p_check_func != NULL) {
        rval = p_check_func->chroma_asf(p_chroma_asf, "chroma_asf");
    }
    if(rval == IK_OK) {
        rval |= img_ctx_get_context(context_id, &p_ctx);
        if((p_chroma_asf != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
            (void)amba_ik_system_lock_mutex(context_id, IK_WAIT_FOREVER);
            (void)amba_ik_system_memcpy(&p_ctx->filters.p_hiso_input_param->chroma_asf, p_chroma_asf, sizeof(ik_chroma_asf_t));
            ctx_raise_update_flag(&p_ctx->filters.update_flags.liso.chroma_asf_updated);
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

uint32 img_ctx_ivd_get_chroma_asf(uint32 context_id, ik_chroma_asf_t *p_chroma_asf)
{
    amba_ik_system_print_str_5("[IK] get_chroma_asf is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //TBD
    }
    if (p_chroma_asf == NULL) {
        //TBD
    } else {
        (void)amba_ik_system_memcpy(p_chroma_asf, p_chroma_asf, sizeof(uint8));
    }
    return IK_ERR_0004;
}

uint32 img_ctx_get_chroma_asf(uint32 context_id, ik_chroma_asf_t *p_chroma_asf)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval |= img_ctx_get_context(context_id, &p_ctx);
    if((p_chroma_asf != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        (void)amba_ik_system_lock_mutex(context_id, IK_WAIT_FOREVER);
        (void)amba_ik_system_memcpy(p_chroma_asf, &p_ctx->filters.p_hiso_input_param->chroma_asf, sizeof(ik_chroma_asf_t));
        (void)amba_ik_system_unlock_mutex(context_id);
    } else {
        if(p_chroma_asf == NULL) {
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

uint32 img_ctx_ivd_set_hi_chroma_asf(uint32 context_id, const ik_hi_chroma_asf_t *p_hi_chroma_asf)
{
    amba_ik_system_print_str_5("[IK] set_hi_chroma_asf is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //TBD
    }
    if (p_hi_chroma_asf==NULL) {
        //TBD
    }
    return IK_ERR_0004;
}

uint32 img_ctx_set_hi_chroma_asf(uint32 context_id, const ik_hi_chroma_asf_t *p_hi_chroma_asf)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;
    const ik_debug_check_func_t* p_check_func = ik_ctx_get_debug_check_func();
    if(p_check_func != NULL) {
        rval = p_check_func->hi_chroma_asf(p_hi_chroma_asf, "hi_chroma_asf");
    }
    if(rval == IK_OK) {
        rval |= img_ctx_get_context(context_id, &p_ctx);
        if((p_hi_chroma_asf != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
            (void)amba_ik_system_lock_mutex(context_id, IK_WAIT_FOREVER);
            (void)amba_ik_system_memcpy(&p_ctx->filters.p_hiso_input_param->hi_chroma_asf, p_hi_chroma_asf, sizeof(ik_hi_chroma_asf_t));
            ctx_raise_update_flag(&p_ctx->filters.update_flags.liso.hi_chroma_asf_updated);
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

uint32 img_ctx_ivd_get_hi_chroma_asf(uint32 context_id, ik_hi_chroma_asf_t *p_hi_chroma_asf)
{
    amba_ik_system_print_str_5("[IK] get_hi_chroma_asf is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //TBD
    }
    if (p_hi_chroma_asf == NULL) {
        //TBD
    } else {
        (void)amba_ik_system_memcpy(p_hi_chroma_asf, p_hi_chroma_asf, sizeof(uint8));
    }
    return IK_ERR_0004;
}

uint32 img_ctx_get_hi_chroma_asf(uint32 context_id, ik_hi_chroma_asf_t *p_hi_chroma_asf)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval |= img_ctx_get_context(context_id, &p_ctx);
    if((p_hi_chroma_asf != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        (void)amba_ik_system_lock_mutex(context_id, IK_WAIT_FOREVER);
        (void)amba_ik_system_memcpy(p_hi_chroma_asf, &p_ctx->filters.p_hiso_input_param->hi_chroma_asf, sizeof(ik_hi_chroma_asf_t));
        (void)amba_ik_system_unlock_mutex(context_id);
    } else {
        if(p_hi_chroma_asf == NULL) {
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

uint32 img_ctx_ivd_set_hi_low_chroma_asf(uint32 context_id, const ik_hi_low_chroma_asf_t *p_hi_low_chroma_asf)
{
    amba_ik_system_print_str_5("[IK] set_hi_low_chroma_asf is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //TBD
    }
    if (p_hi_low_chroma_asf==NULL) {
        //TBD
    }
    return IK_ERR_0004;
}

uint32 img_ctx_set_hi_low_chroma_asf(uint32 context_id, const ik_hi_low_chroma_asf_t *p_hi_low_chroma_asf)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;
    const ik_debug_check_func_t* p_check_func = ik_ctx_get_debug_check_func();
    if(p_check_func != NULL) {
        rval = p_check_func->hi_low_chroma_asf(p_hi_low_chroma_asf, "hi_low_chroma_asf");
    }
    if(rval == IK_OK) {
        rval |= img_ctx_get_context(context_id, &p_ctx);
        if((p_hi_low_chroma_asf != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
            (void)amba_ik_system_lock_mutex(context_id, IK_WAIT_FOREVER);
            (void)amba_ik_system_memcpy(&p_ctx->filters.p_hiso_input_param->hi_chroma_asf, p_hi_low_chroma_asf, sizeof(ik_hi_low_chroma_asf_t));
            ctx_raise_update_flag(&p_ctx->filters.update_flags.liso.hi_low_chroma_asf_updated);
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

uint32 img_ctx_ivd_get_hi_low_chroma_asf(uint32 context_id, ik_hi_low_chroma_asf_t *p_hi_low_chroma_asf)
{
    amba_ik_system_print_str_5("[IK] get_hi_low_chroma_asf is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //TBD
    }
    if (p_hi_low_chroma_asf == NULL) {
        //TBD
    } else {
        (void)amba_ik_system_memcpy(p_hi_low_chroma_asf, p_hi_low_chroma_asf, sizeof(uint8));
    }
    return IK_ERR_0004;
}

uint32 img_ctx_get_hi_low_chroma_asf(uint32 context_id, ik_hi_low_chroma_asf_t *p_hi_low_chroma_asf)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval |= img_ctx_get_context(context_id, &p_ctx);
    if((p_hi_low_chroma_asf != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        (void)amba_ik_system_lock_mutex(context_id, IK_WAIT_FOREVER);
        (void)amba_ik_system_memcpy(p_hi_low_chroma_asf, &p_ctx->filters.p_hiso_input_param->hi_chroma_asf, sizeof(ik_hi_low_chroma_asf_t));
        (void)amba_ik_system_unlock_mutex(context_id);
    } else {
        if(p_hi_low_chroma_asf == NULL) {
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

uint32 img_ctx_ivd_set_hi_high_shpns_both(uint32 context_id, const ik_hi_high_shpns_both_t *p_hi_high_shpns_both)
{
    amba_ik_system_print_str_5("[IK] set_hi_high_shpns_both is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //TBD
    }
    if (p_hi_high_shpns_both==NULL) {
        //TBD
    }
    return IK_ERR_0004;
}

uint32 img_ctx_set_hi_high_shpns_both(uint32 context_id, const ik_hi_high_shpns_both_t *p_hi_high_shpns_both)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;
    const ik_debug_check_func_t* p_check_func = ik_ctx_get_debug_check_func();
    if(p_check_func != NULL) {
        rval = p_check_func->hi_high_shpns_both(p_hi_high_shpns_both, "hi_high_shpns_both");
    }
    if(rval == IK_OK) {
        rval |= img_ctx_get_context(context_id, &p_ctx);
        if((p_hi_high_shpns_both != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
            (void)amba_ik_system_lock_mutex(context_id, IK_WAIT_FOREVER);
            (void)amba_ik_system_memcpy(&p_ctx->filters.p_hiso_input_param->hi_high_shpns_both, p_hi_high_shpns_both, sizeof(ik_hi_high_shpns_both_t));
            ctx_raise_update_flag(&p_ctx->filters.update_flags.liso.hi_high_shpns_both_updated);
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

uint32 img_ctx_ivd_get_hi_high_shpns_both(uint32 context_id, ik_hi_high_shpns_both_t *p_hi_high_shpns_both)
{
    amba_ik_system_print_str_5("[IK] get_hi_high_shpns_both is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //TBD
    }
    if (p_hi_high_shpns_both == NULL) {
        //TBD
    } else {
        (void)amba_ik_system_memcpy(p_hi_high_shpns_both, p_hi_high_shpns_both, sizeof(uint8));
    }
    return IK_ERR_0004;
}

uint32 img_ctx_get_hi_high_shpns_both(uint32 context_id, ik_hi_high_shpns_both_t *p_hi_high_shpns_both)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval |= img_ctx_get_context(context_id, &p_ctx);
    if((p_hi_high_shpns_both != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        (void)amba_ik_system_lock_mutex(context_id, IK_WAIT_FOREVER);
        (void)amba_ik_system_memcpy(p_hi_high_shpns_both, &p_ctx->filters.p_hiso_input_param->hi_high_shpns_both, sizeof(ik_hi_high_shpns_both_t));
        (void)amba_ik_system_unlock_mutex(context_id);
    } else {
        if(p_hi_high_shpns_both == NULL) {
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

uint32 img_ctx_ivd_set_hi_high_shpns_noise(uint32 context_id, const ik_hi_high_shpns_noise_t *p_hi_high_shpns_noise)
{
    amba_ik_system_print_str_5("[IK] set_hi_high_shpns_noise is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //TBD
    }
    if (p_hi_high_shpns_noise==NULL) {
        //TBD
    }
    return IK_ERR_0004;
}

uint32 img_ctx_set_hi_high_shpns_noise(uint32 context_id, const ik_hi_high_shpns_noise_t *p_hi_high_shpns_noise)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;
    const ik_debug_check_func_t* p_check_func = ik_ctx_get_debug_check_func();
    if(p_check_func != NULL) {
        rval = p_check_func->hi_high_shpns_noise(p_hi_high_shpns_noise, "hi_high_shpns_noise");
    }
    if(rval == IK_OK) {
        rval |= img_ctx_get_context(context_id, &p_ctx);
        if((p_hi_high_shpns_noise != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
            (void)amba_ik_system_lock_mutex(context_id, IK_WAIT_FOREVER);
            (void)amba_ik_system_memcpy(&p_ctx->filters.p_hiso_input_param->hi_high_shpns_noise, p_hi_high_shpns_noise, sizeof(ik_hi_high_shpns_noise_t));
            ctx_raise_update_flag(&p_ctx->filters.update_flags.liso.hi_high_shpns_noise_updated);
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

uint32 img_ctx_ivd_get_hi_high_shpns_noise(uint32 context_id, ik_hi_high_shpns_noise_t *p_hi_high_shpns_noise)
{
    amba_ik_system_print_str_5("[IK] get_hi_high_shpns_noise is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //TBD
    }
    if (p_hi_high_shpns_noise == NULL) {
        //TBD
    } else {
        (void)amba_ik_system_memcpy(p_hi_high_shpns_noise, p_hi_high_shpns_noise, sizeof(uint8));
    }
    return IK_ERR_0004;
}

uint32 img_ctx_get_hi_high_shpns_noise(uint32 context_id, ik_hi_high_shpns_noise_t *p_hi_high_shpns_noise)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval |= img_ctx_get_context(context_id, &p_ctx);
    if((p_hi_high_shpns_noise != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        (void)amba_ik_system_lock_mutex(context_id, IK_WAIT_FOREVER);
        (void)amba_ik_system_memcpy(p_hi_high_shpns_noise, &p_ctx->filters.p_hiso_input_param->hi_high_shpns_noise, sizeof(ik_hi_high_shpns_noise_t));
        (void)amba_ik_system_unlock_mutex(context_id);
    } else {
        if(p_hi_high_shpns_noise == NULL) {
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

uint32 img_ctx_ivd_set_hi_high_shpns_coring(uint32 context_id, const ik_hi_high_shpns_coring_t *p_hi_high_shpns_coring)
{
    amba_ik_system_print_str_5("[IK] set_hi_high_shpns_coring is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //TBD
    }
    if (p_hi_high_shpns_coring==NULL) {
        //TBD
    }
    return IK_ERR_0004;
}

uint32 img_ctx_set_hi_high_shpns_coring(uint32 context_id, const ik_hi_high_shpns_coring_t *p_hi_high_shpns_coring)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;
    const ik_debug_check_func_t* p_check_func = ik_ctx_get_debug_check_func();
    if(p_check_func != NULL) {
        rval = p_check_func->hi_high_shpns_coring(p_hi_high_shpns_coring, "hi_high_shpns_coring");
    }
    if(rval == IK_OK) {
        rval |= img_ctx_get_context(context_id, &p_ctx);
        if((p_hi_high_shpns_coring != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
            (void)amba_ik_system_lock_mutex(context_id, IK_WAIT_FOREVER);
            (void)amba_ik_system_memcpy(&p_ctx->filters.p_hiso_input_param->hi_high_shpns_coring, p_hi_high_shpns_coring, sizeof(ik_hi_high_shpns_coring_t));
            ctx_raise_update_flag(&p_ctx->filters.update_flags.liso.hi_high_shpns_coring_updated);
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

uint32 img_ctx_ivd_get_hi_high_shpns_coring(uint32 context_id, ik_hi_high_shpns_coring_t *p_hi_high_shpns_coring)
{
    amba_ik_system_print_str_5("[IK] get_hi_high_shpns_coring is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //TBD
    }
    if (p_hi_high_shpns_coring == NULL) {
        //TBD
    } else {
        (void)amba_ik_system_memcpy(p_hi_high_shpns_coring, p_hi_high_shpns_coring, sizeof(uint8));
    }
    return IK_ERR_0004;
}

uint32 img_ctx_get_hi_high_shpns_coring(uint32 context_id, ik_hi_high_shpns_coring_t *p_hi_high_shpns_coring)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval |= img_ctx_get_context(context_id, &p_ctx);
    if((p_hi_high_shpns_coring != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        (void)amba_ik_system_lock_mutex(context_id, IK_WAIT_FOREVER);
        (void)amba_ik_system_memcpy(p_hi_high_shpns_coring, &p_ctx->filters.p_hiso_input_param->hi_high_shpns_coring, sizeof(ik_hi_high_shpns_coring_t));
        (void)amba_ik_system_unlock_mutex(context_id);
    } else {
        if(p_hi_high_shpns_coring == NULL) {
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

uint32 img_ctx_ivd_set_hi_high_shpns_fir(uint32 context_id, const ik_hi_high_shpns_fir_t *p_hi_high_shpns_fir)
{
    amba_ik_system_print_str_5("[IK] set_hi_high_shpns_fir is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //TBD
    }
    if (p_hi_high_shpns_fir==NULL) {
        //TBD
    }
    return IK_ERR_0004;
}

uint32 img_ctx_set_hi_high_shpns_fir(uint32 context_id, const ik_hi_high_shpns_fir_t *p_hi_high_shpns_fir)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;
    const ik_debug_check_func_t* p_check_func = ik_ctx_get_debug_check_func();
    if(p_check_func != NULL) {
        rval = p_check_func->hi_high_shpns_fir(p_hi_high_shpns_fir, "hi_high_shpns_fir");
    }
    if(rval == IK_OK) {
        rval |= img_ctx_get_context(context_id, &p_ctx);
        if((p_hi_high_shpns_fir != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
            (void)amba_ik_system_lock_mutex(context_id, IK_WAIT_FOREVER);
            (void)amba_ik_system_memcpy(&p_ctx->filters.p_hiso_input_param->hi_high_shpns_fir, p_hi_high_shpns_fir, sizeof(ik_hi_high_shpns_fir_t));
            ctx_raise_update_flag(&p_ctx->filters.update_flags.liso.hi_high_shpns_fir_updated);
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

uint32 img_ctx_ivd_get_hi_high_shpns_fir(uint32 context_id, ik_hi_high_shpns_fir_t *p_hi_high_shpns_fir)
{
    amba_ik_system_print_str_5("[IK] get_hi_high_shpns_fir is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //TBD
    }
    if (p_hi_high_shpns_fir == NULL) {
        //TBD
    } else {
        (void)amba_ik_system_memcpy(p_hi_high_shpns_fir, p_hi_high_shpns_fir, sizeof(uint8));
    }
    return IK_ERR_0004;
}

uint32 img_ctx_get_hi_high_shpns_fir(uint32 context_id, ik_hi_high_shpns_fir_t *p_hi_high_shpns_fir)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval |= img_ctx_get_context(context_id, &p_ctx);
    if((p_hi_high_shpns_fir != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        (void)amba_ik_system_lock_mutex(context_id, IK_WAIT_FOREVER);
        (void)amba_ik_system_memcpy(p_hi_high_shpns_fir, &p_ctx->filters.p_hiso_input_param->hi_high_shpns_fir, sizeof(ik_hi_high_shpns_fir_t));
        (void)amba_ik_system_unlock_mutex(context_id);
    } else {
        if(p_hi_high_shpns_fir == NULL) {
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

uint32 img_ctx_ivd_set_hi_high_shp_cor_idx_scl(uint32 context_id, const ik_hi_high_shpns_cor_idx_scl_t *p_hi_high_shpns_cor_idx_scl)
{
    amba_ik_system_print_str_5("[IK] set_hi_high_shpns_cor_idx_scl is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //TBD
    }
    if (p_hi_high_shpns_cor_idx_scl==NULL) {
        //TBD
    }
    return IK_ERR_0004;
}

uint32 img_ctx_set_hi_high_shpns_cor_idx_scl(uint32 context_id, const ik_hi_high_shpns_cor_idx_scl_t *p_hi_high_shpns_cor_idx_scl)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;
    const ik_debug_check_func_t* p_check_func = ik_ctx_get_debug_check_func();
    if(p_check_func != NULL) {
        rval = p_check_func->hi_high_shpns_cor_idx_scl(p_hi_high_shpns_cor_idx_scl, "hi_high_shpns_cor_idx_scl");
    }
    if(rval == IK_OK) {
        rval |= img_ctx_get_context(context_id, &p_ctx);
        if((p_hi_high_shpns_cor_idx_scl != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
            (void)amba_ik_system_lock_mutex(context_id, IK_WAIT_FOREVER);
            (void)amba_ik_system_memcpy(&p_ctx->filters.p_hiso_input_param->hi_high_shpns_cor_idx_scl, p_hi_high_shpns_cor_idx_scl, sizeof(ik_hi_high_shpns_cor_idx_scl_t));
            ctx_raise_update_flag(&p_ctx->filters.update_flags.liso.hi_high_shpns_cor_idx_scl_updated);
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

uint32 img_ctx_ivd_get_hi_high_shp_cor_idx_scl(uint32 context_id, ik_hi_high_shpns_cor_idx_scl_t *p_hi_high_shpns_cor_idx_scl)
{
    amba_ik_system_print_str_5("[IK] get_hi_high_shpns_cor_idx_scl is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //TBD
    }
    if (p_hi_high_shpns_cor_idx_scl == NULL) {
        //TBD
    } else {
        (void)amba_ik_system_memcpy(p_hi_high_shpns_cor_idx_scl, p_hi_high_shpns_cor_idx_scl, sizeof(uint8));
    }
    return IK_ERR_0004;
}

uint32 img_ctx_get_hi_high_shpns_cor_idx_scl(uint32 context_id, ik_hi_high_shpns_cor_idx_scl_t *p_hi_high_shpns_cor_idx_scl)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval |= img_ctx_get_context(context_id, &p_ctx);
    if((p_hi_high_shpns_cor_idx_scl != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        (void)amba_ik_system_lock_mutex(context_id, IK_WAIT_FOREVER);
        (void)amba_ik_system_memcpy(p_hi_high_shpns_cor_idx_scl, &p_ctx->filters.p_hiso_input_param->hi_high_shpns_cor_idx_scl, sizeof(ik_hi_high_shpns_cor_idx_scl_t));
        (void)amba_ik_system_unlock_mutex(context_id);
    } else {
        if(p_hi_high_shpns_cor_idx_scl == NULL) {
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

uint32 img_ctx_ivd_set_hi_high_shp_min_cor_rst(uint32 context_id, const ik_hi_high_shpns_min_cor_rst_t *p_hi_high_shpns_min_cor_rst)
{
    amba_ik_system_print_str_5("[IK] set_hi_high_shpns_min_cor_rst is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //TBD
    }
    if (p_hi_high_shpns_min_cor_rst==NULL) {
        //TBD
    }
    return IK_ERR_0004;
}

uint32 img_ctx_set_hi_high_shpns_min_cor_rst(uint32 context_id, const ik_hi_high_shpns_min_cor_rst_t *p_hi_high_shpns_min_cor_rst)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;
    const ik_debug_check_func_t* p_check_func = ik_ctx_get_debug_check_func();
    if(p_check_func != NULL) {
        rval = p_check_func->hi_high_shpns_min_cor_rst(p_hi_high_shpns_min_cor_rst, "hi_high_shpns_min_cor_rst");
    }
    if(rval == IK_OK) {
        rval |= img_ctx_get_context(context_id, &p_ctx);
        if((p_hi_high_shpns_min_cor_rst != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
            (void)amba_ik_system_lock_mutex(context_id, IK_WAIT_FOREVER);
            (void)amba_ik_system_memcpy(&p_ctx->filters.p_hiso_input_param->hi_high_shpns_min_cor_rst, p_hi_high_shpns_min_cor_rst, sizeof(ik_hi_high_shpns_min_cor_rst_t));
            ctx_raise_update_flag(&p_ctx->filters.update_flags.liso.hi_high_shpns_min_cor_rst_updated);
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

uint32 img_ctx_ivd_get_hi_high_shp_min_cor_rst(uint32 context_id, ik_hi_high_shpns_min_cor_rst_t *p_hi_high_shpns_min_cor_rst)
{
    amba_ik_system_print_str_5("[IK] get_hi_high_shpns_min_cor_rst is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //TBD
    }
    if (p_hi_high_shpns_min_cor_rst == NULL) {
        //TBD
    } else {
        (void)amba_ik_system_memcpy(p_hi_high_shpns_min_cor_rst, p_hi_high_shpns_min_cor_rst, sizeof(uint8));
    }
    return IK_ERR_0004;
}

uint32 img_ctx_get_hi_high_shpns_min_cor_rst(uint32 context_id, ik_hi_high_shpns_min_cor_rst_t *p_hi_high_shpns_min_cor_rst)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval |= img_ctx_get_context(context_id, &p_ctx);
    if((p_hi_high_shpns_min_cor_rst != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        (void)amba_ik_system_lock_mutex(context_id, IK_WAIT_FOREVER);
        (void)amba_ik_system_memcpy(p_hi_high_shpns_min_cor_rst, &p_ctx->filters.p_hiso_input_param->hi_high_shpns_min_cor_rst, sizeof(ik_hi_high_shpns_min_cor_rst_t));
        (void)amba_ik_system_unlock_mutex(context_id);
    } else {
        if(p_hi_high_shpns_min_cor_rst == NULL) {
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

uint32 img_ctx_ivd_set_hi_high_shp_max_cor_rst(uint32 context_id, const ik_hi_high_shpns_max_cor_rst_t *p_hi_high_shpns_max_cor_rst)
{
    amba_ik_system_print_str_5("[IK] set_hi_high_shpns_max_cor_rst is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //TBD
    }
    if (p_hi_high_shpns_max_cor_rst==NULL) {
        //TBD
    }
    return IK_ERR_0004;
}

uint32 img_ctx_set_hi_high_shpns_max_cor_rst(uint32 context_id, const ik_hi_high_shpns_max_cor_rst_t *p_hi_high_shpns_max_cor_rst)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;
    const ik_debug_check_func_t* p_check_func = ik_ctx_get_debug_check_func();
    if(p_check_func != NULL) {
        rval = p_check_func->hi_high_shpns_max_cor_rst(p_hi_high_shpns_max_cor_rst, "hi_high_shpns_max_cor_rst");
    }
    if(rval == IK_OK) {
        rval |= img_ctx_get_context(context_id, &p_ctx);
        if((p_hi_high_shpns_max_cor_rst != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
            (void)amba_ik_system_lock_mutex(context_id, IK_WAIT_FOREVER);
            (void)amba_ik_system_memcpy(&p_ctx->filters.p_hiso_input_param->hi_high_shpns_max_cor_rst, p_hi_high_shpns_max_cor_rst, sizeof(ik_hi_high_shpns_max_cor_rst_t));
            ctx_raise_update_flag(&p_ctx->filters.update_flags.liso.hi_high_shpns_max_cor_rst_updated);
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

uint32 img_ctx_ivd_get_hi_high_shp_max_cor_rst(uint32 context_id, ik_hi_high_shpns_max_cor_rst_t *p_hi_high_shpns_max_cor_rst)
{
    amba_ik_system_print_str_5("[IK] get_hi_high_shpns_max_cor_rst is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //TBD
    }
    if (p_hi_high_shpns_max_cor_rst == NULL) {
        //TBD
    } else {
        (void)amba_ik_system_memcpy(p_hi_high_shpns_max_cor_rst, p_hi_high_shpns_max_cor_rst, sizeof(uint8));
    }
    return IK_ERR_0004;
}

uint32 img_ctx_get_hi_high_shpns_max_cor_rst(uint32 context_id, ik_hi_high_shpns_max_cor_rst_t *p_hi_high_shpns_max_cor_rst)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval |= img_ctx_get_context(context_id, &p_ctx);
    if((p_hi_high_shpns_max_cor_rst != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        (void)amba_ik_system_lock_mutex(context_id, IK_WAIT_FOREVER);
        (void)amba_ik_system_memcpy(p_hi_high_shpns_max_cor_rst, &p_ctx->filters.p_hiso_input_param->hi_high_shpns_max_cor_rst, sizeof(ik_hi_high_shpns_max_cor_rst_t));
        (void)amba_ik_system_unlock_mutex(context_id);
    } else {
        if(p_hi_high_shpns_max_cor_rst == NULL) {
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

uint32 img_ctx_ivd_set_hi_high_shpns_scl_cor(uint32 context_id, const ik_hi_high_shpns_scl_cor_t *p_hi_high_shpns_scl_cor)
{
    amba_ik_system_print_str_5("[IK] set_hi_high_shpns_scl_cor is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //TBD
    }
    if (p_hi_high_shpns_scl_cor==NULL) {
        //TBD
    }
    return IK_ERR_0004;
}

uint32 img_ctx_set_hi_high_shpns_scl_cor(uint32 context_id, const ik_hi_high_shpns_scl_cor_t *p_hi_high_shpns_scl_cor)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;
    const ik_debug_check_func_t* p_check_func = ik_ctx_get_debug_check_func();
    if(p_check_func != NULL) {
        rval = p_check_func->hi_high_shpns_scl_cor(p_hi_high_shpns_scl_cor, "hi_high_shpns_scl_cor");
    }
    if(rval == IK_OK) {
        rval |= img_ctx_get_context(context_id, &p_ctx);
        if((p_hi_high_shpns_scl_cor != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
            (void)amba_ik_system_lock_mutex(context_id, IK_WAIT_FOREVER);
            (void)amba_ik_system_memcpy(&p_ctx->filters.p_hiso_input_param->hi_high_shpns_scl_cor, p_hi_high_shpns_scl_cor, sizeof(ik_hi_high_shpns_scl_cor_t));
            ctx_raise_update_flag(&p_ctx->filters.update_flags.liso.hi_high_shpns_scl_cor_updated);
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

uint32 img_ctx_ivd_get_hi_high_shpns_scl_cor(uint32 context_id, ik_hi_high_shpns_scl_cor_t *p_hi_high_shpns_scl_cor)
{
    amba_ik_system_print_str_5("[IK] get_hi_high_shpns_scl_cor is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //TBD
    }
    if (p_hi_high_shpns_scl_cor == NULL) {
        //TBD
    } else {
        (void)amba_ik_system_memcpy(p_hi_high_shpns_scl_cor, p_hi_high_shpns_scl_cor, sizeof(uint8));
    }
    return IK_ERR_0004;
}

uint32 img_ctx_get_hi_high_shpns_scl_cor(uint32 context_id, ik_hi_high_shpns_scl_cor_t *p_hi_high_shpns_scl_cor)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval |= img_ctx_get_context(context_id, &p_ctx);
    if((p_hi_high_shpns_scl_cor != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        (void)amba_ik_system_lock_mutex(context_id, IK_WAIT_FOREVER);
        (void)amba_ik_system_memcpy(p_hi_high_shpns_scl_cor, &p_ctx->filters.p_hiso_input_param->hi_high_shpns_scl_cor, sizeof(ik_hi_high_shpns_scl_cor_t));
        (void)amba_ik_system_unlock_mutex(context_id);
    } else {
        if(p_hi_high_shpns_scl_cor == NULL) {
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

uint32 img_ctx_ivd_set_hi_med_shpns_both(uint32 context_id, const ik_hi_med_shpns_both_t *p_hi_med_shpns_both)
{
    amba_ik_system_print_str_5("[IK] set_hi_med_shpns_both is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //TBD
    }
    if (p_hi_med_shpns_both==NULL) {
        //TBD
    }
    return IK_ERR_0004;
}

uint32 img_ctx_set_hi_med_shpns_both(uint32 context_id, const ik_hi_med_shpns_both_t *p_hi_med_shpns_both)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;
    const ik_debug_check_func_t* p_check_func = ik_ctx_get_debug_check_func();
    if(p_check_func != NULL) {
        rval = p_check_func->hi_med_shpns_both(p_hi_med_shpns_both, "hi_med_shpns_both");
    }
    if(rval == IK_OK) {
        rval |= img_ctx_get_context(context_id, &p_ctx);
        if((p_hi_med_shpns_both != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
            (void)amba_ik_system_lock_mutex(context_id, IK_WAIT_FOREVER);
            (void)amba_ik_system_memcpy(&p_ctx->filters.p_hiso_input_param->hi_med_shpns_both, p_hi_med_shpns_both, sizeof(ik_hi_med_shpns_both_t));
            ctx_raise_update_flag(&p_ctx->filters.update_flags.liso.hi_med_shpns_both_updated);
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

uint32 img_ctx_ivd_get_hi_med_shpns_both(uint32 context_id, ik_hi_med_shpns_both_t *p_hi_med_shpns_both)
{
    amba_ik_system_print_str_5("[IK] get_hi_med_shpns_both is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //TBD
    }
    if (p_hi_med_shpns_both == NULL) {
        //TBD
    } else {
        (void)amba_ik_system_memcpy(p_hi_med_shpns_both, p_hi_med_shpns_both, sizeof(uint8));
    }
    return IK_ERR_0004;
}

uint32 img_ctx_get_hi_med_shpns_both(uint32 context_id, ik_hi_med_shpns_both_t *p_hi_med_shpns_both)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval |= img_ctx_get_context(context_id, &p_ctx);
    if((p_hi_med_shpns_both != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        (void)amba_ik_system_lock_mutex(context_id, IK_WAIT_FOREVER);
        (void)amba_ik_system_memcpy(p_hi_med_shpns_both, &p_ctx->filters.p_hiso_input_param->hi_med_shpns_both, sizeof(ik_hi_med_shpns_both_t));
        (void)amba_ik_system_unlock_mutex(context_id);
    } else {
        if(p_hi_med_shpns_both == NULL) {
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

uint32 img_ctx_ivd_set_hi_med_shpns_noise(uint32 context_id, const ik_hi_med_shpns_noise_t *p_hi_med_shpns_noise)
{
    amba_ik_system_print_str_5("[IK] set_hi_med_shpns_noise is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //TBD
    }
    if (p_hi_med_shpns_noise==NULL) {
        //TBD
    }
    return IK_ERR_0004;
}

uint32 img_ctx_set_hi_med_shpns_noise(uint32 context_id, const ik_hi_med_shpns_noise_t *p_hi_med_shpns_noise)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;
    const ik_debug_check_func_t* p_check_func = ik_ctx_get_debug_check_func();
    if(p_check_func != NULL) {
        rval = p_check_func->hi_med_shpns_noise(p_hi_med_shpns_noise, "hi_med_shpns_noise");
    }
    if(rval == IK_OK) {
        rval |= img_ctx_get_context(context_id, &p_ctx);
        if((p_hi_med_shpns_noise != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
            (void)amba_ik_system_lock_mutex(context_id, IK_WAIT_FOREVER);
            (void)amba_ik_system_memcpy(&p_ctx->filters.p_hiso_input_param->hi_med_shpns_noise, p_hi_med_shpns_noise, sizeof(ik_hi_med_shpns_noise_t));
            ctx_raise_update_flag(&p_ctx->filters.update_flags.liso.hi_med_shpns_noise_updated);
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

uint32 img_ctx_ivd_get_hi_med_shpns_noise(uint32 context_id, ik_hi_med_shpns_noise_t *p_hi_med_shpns_noise)
{
    amba_ik_system_print_str_5("[IK] get_hi_med_shpns_noise is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //TBD
    }
    if (p_hi_med_shpns_noise == NULL) {
        //TBD
    } else {
        (void)amba_ik_system_memcpy(p_hi_med_shpns_noise, p_hi_med_shpns_noise, sizeof(uint8));
    }
    return IK_ERR_0004;
}

uint32 img_ctx_get_hi_med_shpns_noise(uint32 context_id, ik_hi_med_shpns_noise_t *p_hi_med_shpns_noise)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval |= img_ctx_get_context(context_id, &p_ctx);
    if((p_hi_med_shpns_noise != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        (void)amba_ik_system_lock_mutex(context_id, IK_WAIT_FOREVER);
        (void)amba_ik_system_memcpy(p_hi_med_shpns_noise, &p_ctx->filters.p_hiso_input_param->hi_med_shpns_noise, sizeof(ik_hi_med_shpns_noise_t));
        (void)amba_ik_system_unlock_mutex(context_id);
    } else {
        if(p_hi_med_shpns_noise == NULL) {
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

uint32 img_ctx_ivd_set_hi_med_shpns_coring(uint32 context_id, const ik_hi_med_shpns_coring_t *p_hi_med_shpns_coring)
{
    amba_ik_system_print_str_5("[IK] set_hi_med_shpns_coring is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //TBD
    }
    if (p_hi_med_shpns_coring==NULL) {
        //TBD
    }
    return IK_ERR_0004;
}

uint32 img_ctx_set_hi_med_shpns_coring(uint32 context_id, const ik_hi_med_shpns_coring_t *p_hi_med_shpns_coring)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;
    const ik_debug_check_func_t* p_check_func = ik_ctx_get_debug_check_func();
    if(p_check_func != NULL) {
        rval = p_check_func->hi_med_shpns_coring(p_hi_med_shpns_coring, "hi_med_shpns_coring");
    }
    if(rval == IK_OK) {
        rval |= img_ctx_get_context(context_id, &p_ctx);
        if((p_hi_med_shpns_coring != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
            (void)amba_ik_system_lock_mutex(context_id, IK_WAIT_FOREVER);
            (void)amba_ik_system_memcpy(&p_ctx->filters.p_hiso_input_param->hi_med_shpns_coring, p_hi_med_shpns_coring, sizeof(ik_hi_med_shpns_coring_t));
            ctx_raise_update_flag(&p_ctx->filters.update_flags.liso.hi_med_shpns_coring_updated);
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

uint32 img_ctx_ivd_get_hi_med_shpns_coring(uint32 context_id, ik_hi_med_shpns_coring_t *p_hi_med_shpns_coring)
{
    amba_ik_system_print_str_5("[IK] get_hi_med_shpns_coring is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //TBD
    }
    if (p_hi_med_shpns_coring == NULL) {
        //TBD
    } else {
        (void)amba_ik_system_memcpy(p_hi_med_shpns_coring, p_hi_med_shpns_coring, sizeof(uint8));
    }
    return IK_ERR_0004;
}

uint32 img_ctx_get_hi_med_shpns_coring(uint32 context_id, ik_hi_med_shpns_coring_t *p_hi_med_shpns_coring)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval |= img_ctx_get_context(context_id, &p_ctx);
    if((p_hi_med_shpns_coring != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        (void)amba_ik_system_lock_mutex(context_id, IK_WAIT_FOREVER);
        (void)amba_ik_system_memcpy(p_hi_med_shpns_coring, &p_ctx->filters.p_hiso_input_param->hi_med_shpns_coring, sizeof(ik_hi_med_shpns_coring_t));
        (void)amba_ik_system_unlock_mutex(context_id);
    } else {
        if(p_hi_med_shpns_coring == NULL) {
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

uint32 img_ctx_ivd_set_hi_med_shpns_fir(uint32 context_id, const ik_hi_med_shpns_fir_t *p_hi_med_shpns_fir)
{
    amba_ik_system_print_str_5("[IK] set_hi_med_shpns_fir is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //TBD
    }
    if (p_hi_med_shpns_fir==NULL) {
        //TBD
    }
    return IK_ERR_0004;
}

uint32 img_ctx_set_hi_med_shpns_fir(uint32 context_id, const ik_hi_med_shpns_fir_t *p_hi_med_shpns_fir)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;
    const ik_debug_check_func_t* p_check_func = ik_ctx_get_debug_check_func();
    if(p_check_func != NULL) {
        rval = p_check_func->hi_med_shpns_fir(p_hi_med_shpns_fir, "hi_med_shpns_fir");
    }
    if(rval == IK_OK) {
        rval |= img_ctx_get_context(context_id, &p_ctx);
        if((p_hi_med_shpns_fir != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
            (void)amba_ik_system_lock_mutex(context_id, IK_WAIT_FOREVER);
            (void)amba_ik_system_memcpy(&p_ctx->filters.p_hiso_input_param->hi_med_shpns_fir, p_hi_med_shpns_fir, sizeof(ik_hi_med_shpns_fir_t));
            ctx_raise_update_flag(&p_ctx->filters.update_flags.liso.hi_med_shpns_fir_updated);
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

uint32 img_ctx_ivd_get_hi_med_shpns_fir(uint32 context_id, ik_hi_med_shpns_fir_t *p_hi_med_shpns_fir)
{
    amba_ik_system_print_str_5("[IK] get_hi_med_shpns_fir is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //TBD
    }
    if (p_hi_med_shpns_fir == NULL) {
        //TBD
    } else {
        (void)amba_ik_system_memcpy(p_hi_med_shpns_fir, p_hi_med_shpns_fir, sizeof(uint8));
    }
    return IK_ERR_0004;
}

uint32 img_ctx_get_hi_med_shpns_fir(uint32 context_id, ik_hi_med_shpns_fir_t *p_hi_med_shpns_fir)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval |= img_ctx_get_context(context_id, &p_ctx);
    if((p_hi_med_shpns_fir != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        (void)amba_ik_system_lock_mutex(context_id, IK_WAIT_FOREVER);
        (void)amba_ik_system_memcpy(p_hi_med_shpns_fir, &p_ctx->filters.p_hiso_input_param->hi_med_shpns_fir, sizeof(ik_hi_med_shpns_fir_t));
        (void)amba_ik_system_unlock_mutex(context_id);
    } else {
        if(p_hi_med_shpns_fir == NULL) {
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

uint32 img_ctx_ivd_set_hi_med_shp_cor_idx_scl(uint32 context_id, const ik_hi_med_shpns_cor_idx_scl_t *p_hi_med_shpns_cor_idx_scl)
{
    amba_ik_system_print_str_5("[IK] set_hi_med_shpns_cor_idx_scl is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //TBD
    }
    if (p_hi_med_shpns_cor_idx_scl==NULL) {
        //TBD
    }
    return IK_ERR_0004;
}

uint32 img_ctx_set_hi_med_shpns_cor_idx_scl(uint32 context_id, const ik_hi_med_shpns_cor_idx_scl_t *p_hi_med_shpns_cor_idx_scl)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;
    const ik_debug_check_func_t* p_check_func = ik_ctx_get_debug_check_func();
    if(p_check_func != NULL) {
        rval = p_check_func->hi_med_shpns_cor_idx_scl(p_hi_med_shpns_cor_idx_scl, "hi_med_shpns_cor_idx_scl");
    }
    if(rval == IK_OK) {
        rval |= img_ctx_get_context(context_id, &p_ctx);
        if((p_hi_med_shpns_cor_idx_scl != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
            (void)amba_ik_system_lock_mutex(context_id, IK_WAIT_FOREVER);
            (void)amba_ik_system_memcpy(&p_ctx->filters.p_hiso_input_param->hi_med_shpns_cor_idx_scl, p_hi_med_shpns_cor_idx_scl, sizeof(ik_hi_med_shpns_cor_idx_scl_t));
            ctx_raise_update_flag(&p_ctx->filters.update_flags.liso.hi_med_shpns_cor_idx_scl_updated);
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

uint32 img_ctx_ivd_get_hi_med_shp_cor_idx_scl(uint32 context_id, ik_hi_med_shpns_cor_idx_scl_t *p_hi_med_shpns_cor_idx_scl)
{
    amba_ik_system_print_str_5("[IK] get_hi_med_shpns_cor_idx_scl is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //TBD
    }
    if (p_hi_med_shpns_cor_idx_scl == NULL) {
        //TBD
    } else {
        (void)amba_ik_system_memcpy(p_hi_med_shpns_cor_idx_scl, p_hi_med_shpns_cor_idx_scl, sizeof(uint8));
    }
    return IK_ERR_0004;
}

uint32 img_ctx_get_hi_med_shpns_cor_idx_scl(uint32 context_id, ik_hi_med_shpns_cor_idx_scl_t *p_hi_med_shpns_cor_idx_scl)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval |= img_ctx_get_context(context_id, &p_ctx);
    if((p_hi_med_shpns_cor_idx_scl != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        (void)amba_ik_system_lock_mutex(context_id, IK_WAIT_FOREVER);
        (void)amba_ik_system_memcpy(p_hi_med_shpns_cor_idx_scl, &p_ctx->filters.p_hiso_input_param->hi_med_shpns_cor_idx_scl, sizeof(ik_hi_med_shpns_cor_idx_scl_t));
        (void)amba_ik_system_unlock_mutex(context_id);
    } else {
        if(p_hi_med_shpns_cor_idx_scl == NULL) {
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

uint32 img_ctx_ivd_set_hi_med_shpns_min_cor_rst(uint32 context_id, const ik_hi_med_shpns_min_cor_rst_t *p_hi_med_shpns_min_cor_rst)
{
    amba_ik_system_print_str_5("[IK] set_hi_med_shpns_min_cor_rst is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //TBD
    }
    if (p_hi_med_shpns_min_cor_rst==NULL) {
        //TBD
    }
    return IK_ERR_0004;
}

uint32 img_ctx_set_hi_med_shpns_min_cor_rst(uint32 context_id, const ik_hi_med_shpns_min_cor_rst_t *p_hi_med_shpns_min_cor_rst)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;
    const ik_debug_check_func_t* p_check_func = ik_ctx_get_debug_check_func();
    if(p_check_func != NULL) {
        rval = p_check_func->hi_med_shpns_min_cor_rst(p_hi_med_shpns_min_cor_rst, "hi_med_shpns_min_cor_rst");
    }
    if(rval == IK_OK) {
        rval |= img_ctx_get_context(context_id, &p_ctx);
        if((p_hi_med_shpns_min_cor_rst != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
            (void)amba_ik_system_lock_mutex(context_id, IK_WAIT_FOREVER);
            (void)amba_ik_system_memcpy(&p_ctx->filters.p_hiso_input_param->hi_med_shpns_min_cor_rst, p_hi_med_shpns_min_cor_rst, sizeof(ik_hi_med_shpns_min_cor_rst_t));
            ctx_raise_update_flag(&p_ctx->filters.update_flags.liso.hi_med_shpns_min_cor_rst_updated);
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

uint32 img_ctx_ivd_get_hi_med_shpns_min_cor_rst(uint32 context_id, ik_hi_med_shpns_min_cor_rst_t *p_hi_med_shpns_min_cor_rst)
{
    amba_ik_system_print_str_5("[IK] get_hi_med_shpns_min_cor_rst is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //TBD
    }
    if (p_hi_med_shpns_min_cor_rst == NULL) {
        //TBD
    } else {
        (void)amba_ik_system_memcpy(p_hi_med_shpns_min_cor_rst, p_hi_med_shpns_min_cor_rst, sizeof(uint8));
    }
    return IK_ERR_0004;
}

uint32 img_ctx_get_hi_med_shpns_min_cor_rst(uint32 context_id, ik_hi_med_shpns_min_cor_rst_t *p_hi_med_shpns_min_cor_rst)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval |= img_ctx_get_context(context_id, &p_ctx);
    if((p_hi_med_shpns_min_cor_rst != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        (void)amba_ik_system_lock_mutex(context_id, IK_WAIT_FOREVER);
        (void)amba_ik_system_memcpy(p_hi_med_shpns_min_cor_rst, &p_ctx->filters.p_hiso_input_param->hi_med_shpns_min_cor_rst, sizeof(ik_hi_med_shpns_min_cor_rst_t));
        (void)amba_ik_system_unlock_mutex(context_id);
    } else {
        if(p_hi_med_shpns_min_cor_rst == NULL) {
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

uint32 img_ctx_ivd_set_hi_med_shpns_max_cor_rst(uint32 context_id, const ik_hi_med_shpns_max_cor_rst_t *p_hi_med_shpns_max_cor_rst)
{
    amba_ik_system_print_str_5("[IK] set_hi_med_shpns_max_cor_rst is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //TBD
    }
    if (p_hi_med_shpns_max_cor_rst==NULL) {
        //TBD
    }
    return IK_ERR_0004;
}

uint32 img_ctx_set_hi_med_shpns_max_cor_rst(uint32 context_id, const ik_hi_med_shpns_max_cor_rst_t *p_hi_med_shpns_max_cor_rst)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;
    const ik_debug_check_func_t* p_check_func = ik_ctx_get_debug_check_func();
    if(p_check_func != NULL) {
        rval = p_check_func->hi_med_shpns_max_cor_rst(p_hi_med_shpns_max_cor_rst, "hi_med_shpns_max_cor_rst");
    }
    if(rval == IK_OK) {
        rval |= img_ctx_get_context(context_id, &p_ctx);
        if((p_hi_med_shpns_max_cor_rst != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
            (void)amba_ik_system_lock_mutex(context_id, IK_WAIT_FOREVER);
            (void)amba_ik_system_memcpy(&p_ctx->filters.p_hiso_input_param->hi_med_shpns_max_cor_rst, p_hi_med_shpns_max_cor_rst, sizeof(ik_hi_med_shpns_max_cor_rst_t));
            ctx_raise_update_flag(&p_ctx->filters.update_flags.liso.hi_med_shpns_max_cor_rst_updated);
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

uint32 img_ctx_ivd_get_hi_med_shpns_max_cor_rst(uint32 context_id, ik_hi_med_shpns_max_cor_rst_t *p_hi_med_shpns_max_cor_rst)
{
    amba_ik_system_print_str_5("[IK] get_hi_med_shpns_max_cor_rst is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //TBD
    }
    if (p_hi_med_shpns_max_cor_rst == NULL) {
        //TBD
    } else {
        (void)amba_ik_system_memcpy(p_hi_med_shpns_max_cor_rst, p_hi_med_shpns_max_cor_rst, sizeof(uint8));
    }
    return IK_ERR_0004;
}

uint32 img_ctx_get_hi_med_shpns_max_cor_rst(uint32 context_id, ik_hi_med_shpns_max_cor_rst_t *p_hi_med_shpns_max_cor_rst)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval |= img_ctx_get_context(context_id, &p_ctx);
    if((p_hi_med_shpns_max_cor_rst != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        (void)amba_ik_system_lock_mutex(context_id, IK_WAIT_FOREVER);
        (void)amba_ik_system_memcpy(p_hi_med_shpns_max_cor_rst, &p_ctx->filters.p_hiso_input_param->hi_med_shpns_max_cor_rst, sizeof(ik_hi_med_shpns_max_cor_rst_t));
        (void)amba_ik_system_unlock_mutex(context_id);
    } else {
        if(p_hi_med_shpns_max_cor_rst == NULL) {
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

uint32 img_ctx_ivd_set_hi_med_shpns_scl_cor(uint32 context_id, const ik_hi_med_shpns_scl_cor_t *p_hi_med_shpns_scl_cor)
{
    amba_ik_system_print_str_5("[IK] set_hi_med_shpns_scl_cor is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //TBD
    }
    if (p_hi_med_shpns_scl_cor==NULL) {
        //TBD
    }
    return IK_ERR_0004;
}

uint32 img_ctx_set_hi_med_shpns_scl_cor(uint32 context_id, const ik_hi_med_shpns_scl_cor_t *p_hi_med_shpns_scl_cor)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;
    const ik_debug_check_func_t* p_check_func = ik_ctx_get_debug_check_func();
    if(p_check_func != NULL) {
        rval = p_check_func->hi_med_shpns_scl_cor(p_hi_med_shpns_scl_cor, "hi_med_shpns_scl_cor");
    }
    if(rval == IK_OK) {
        rval |= img_ctx_get_context(context_id, &p_ctx);
        if((p_hi_med_shpns_scl_cor != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
            (void)amba_ik_system_lock_mutex(context_id, IK_WAIT_FOREVER);
            (void)amba_ik_system_memcpy(&p_ctx->filters.p_hiso_input_param->hi_med_shpns_scl_cor, p_hi_med_shpns_scl_cor, sizeof(ik_hi_med_shpns_scl_cor_t));
            ctx_raise_update_flag(&p_ctx->filters.update_flags.liso.hi_med_shpns_scl_cor_updated);
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

uint32 img_ctx_ivd_get_hi_med_shpns_scl_cor(uint32 context_id, ik_hi_med_shpns_scl_cor_t *p_hi_med_shpns_scl_cor)
{
    amba_ik_system_print_str_5("[IK] get_hi_med_shpns_scl_cor is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //TBD
    }
    if (p_hi_med_shpns_scl_cor == NULL) {
        //TBD
    } else {
        (void)amba_ik_system_memcpy(p_hi_med_shpns_scl_cor, p_hi_med_shpns_scl_cor, sizeof(uint8));
    }
    return IK_ERR_0004;
}

uint32 img_ctx_get_hi_med_shpns_scl_cor(uint32 context_id, ik_hi_med_shpns_scl_cor_t *p_hi_med_shpns_scl_cor)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval |= img_ctx_get_context(context_id, &p_ctx);
    if((p_hi_med_shpns_scl_cor != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        (void)amba_ik_system_lock_mutex(context_id, IK_WAIT_FOREVER);
        (void)amba_ik_system_memcpy(p_hi_med_shpns_scl_cor, &p_ctx->filters.p_hiso_input_param->hi_med_shpns_scl_cor, sizeof(ik_hi_med_shpns_scl_cor_t));
        (void)amba_ik_system_unlock_mutex(context_id);
    } else {
        if(p_hi_med_shpns_scl_cor == NULL) {
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

uint32 img_ctx_ivd_set_li2_shpns_both(uint32 context_id, const ik_li2_shpns_both_t *p_li2_shpns_both)
{
    amba_ik_system_print_str_5("[IK] set_li2_shpns_both is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //TBD
    }
    if (p_li2_shpns_both==NULL) {
        //TBD
    }
    return IK_ERR_0004;
}

uint32 img_ctx_set_li2_shpns_both(uint32 context_id, const ik_li2_shpns_both_t *p_li2_shpns_both)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;
    const ik_debug_check_func_t* p_check_func = ik_ctx_get_debug_check_func();
    if(p_check_func != NULL) {
        rval = p_check_func->li2_shpns_both(p_li2_shpns_both, "li2_shpns_both");
    }
    if(rval == IK_OK) {
        rval |= img_ctx_get_context(context_id, &p_ctx);
        if((p_li2_shpns_both != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
            (void)amba_ik_system_lock_mutex(context_id, IK_WAIT_FOREVER);
            (void)amba_ik_system_memcpy(&p_ctx->filters.p_hiso_input_param->li2_shpns_both, p_li2_shpns_both, sizeof(ik_li2_shpns_both_t));
            ctx_raise_update_flag(&p_ctx->filters.update_flags.liso.li2_shpns_both_updated);
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

uint32 img_ctx_ivd_get_li2_shpns_both(uint32 context_id, ik_li2_shpns_both_t *p_li2_shpns_both)
{
    amba_ik_system_print_str_5("[IK] get_li2_shpns_both is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //TBD
    }
    if (p_li2_shpns_both == NULL) {
        //TBD
    } else {
        (void)amba_ik_system_memcpy(p_li2_shpns_both, p_li2_shpns_both, sizeof(uint8));
    }
    return IK_ERR_0004;
}

uint32 img_ctx_get_li2_shpns_both(uint32 context_id, ik_li2_shpns_both_t *p_li2_shpns_both)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval |= img_ctx_get_context(context_id, &p_ctx);
    if((p_li2_shpns_both != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        (void)amba_ik_system_lock_mutex(context_id, IK_WAIT_FOREVER);
        (void)amba_ik_system_memcpy(p_li2_shpns_both, &p_ctx->filters.p_hiso_input_param->li2_shpns_both, sizeof(ik_li2_shpns_both_t));
        (void)amba_ik_system_unlock_mutex(context_id);
    } else {
        if(p_li2_shpns_both == NULL) {
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

uint32 img_ctx_ivd_set_li2_shpns_noise(uint32 context_id, const ik_li2_shpns_noise_t *p_li2_shpns_noise)
{
    amba_ik_system_print_str_5("[IK] set_li2_shpns_noise is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //TBD
    }
    if (p_li2_shpns_noise==NULL) {
        //TBD
    }
    return IK_ERR_0004;
}

uint32 img_ctx_set_li2_shpns_noise(uint32 context_id, const ik_li2_shpns_noise_t *p_li2_shpns_noise)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;
    const ik_debug_check_func_t* p_check_func = ik_ctx_get_debug_check_func();
    if(p_check_func != NULL) {
        rval = p_check_func->li2_shpns_noise(p_li2_shpns_noise, "li2_shpns_noise");
    }
    if(rval == IK_OK) {
        rval |= img_ctx_get_context(context_id, &p_ctx);
        if((p_li2_shpns_noise != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
            (void)amba_ik_system_lock_mutex(context_id, IK_WAIT_FOREVER);
            (void)amba_ik_system_memcpy(&p_ctx->filters.p_hiso_input_param->li2_shpns_noise, p_li2_shpns_noise, sizeof(ik_li2_shpns_noise_t));
            ctx_raise_update_flag(&p_ctx->filters.update_flags.liso.li2_shpns_noise_updated);
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

uint32 img_ctx_ivd_get_li2_shpns_noise(uint32 context_id, ik_li2_shpns_noise_t *p_li2_shpns_noise)
{
    amba_ik_system_print_str_5("[IK] get_li2_shpns_noise is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //TBD
    }
    if (p_li2_shpns_noise == NULL) {
        //TBD
    } else {
        (void)amba_ik_system_memcpy(p_li2_shpns_noise, p_li2_shpns_noise, sizeof(uint8));
    }
    return IK_ERR_0004;
}

uint32 img_ctx_get_li2_shpns_noise(uint32 context_id, ik_li2_shpns_noise_t *p_li2_shpns_noise)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval |= img_ctx_get_context(context_id, &p_ctx);
    if((p_li2_shpns_noise != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        (void)amba_ik_system_lock_mutex(context_id, IK_WAIT_FOREVER);
        (void)amba_ik_system_memcpy(p_li2_shpns_noise, &p_ctx->filters.p_hiso_input_param->li2_shpns_noise, sizeof(ik_li2_shpns_noise_t));
        (void)amba_ik_system_unlock_mutex(context_id);
    } else {
        if(p_li2_shpns_noise == NULL) {
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

uint32 img_ctx_ivd_set_li2_shpns_coring(uint32 context_id, const ik_li2_shpns_coring_t *p_li2_shpns_coring)
{
    amba_ik_system_print_str_5("[IK] set_li2_shpns_coring is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //TBD
    }
    if (p_li2_shpns_coring==NULL) {
        //TBD
    }
    return IK_ERR_0004;
}

uint32 img_ctx_set_li2_shpns_coring(uint32 context_id, const ik_li2_shpns_coring_t *p_li2_shpns_coring)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;
    const ik_debug_check_func_t* p_check_func = ik_ctx_get_debug_check_func();
    if(p_check_func != NULL) {
        rval = p_check_func->li2_shpns_coring(p_li2_shpns_coring, "li2_shpns_coring");
    }
    if(rval == IK_OK) {
        rval |= img_ctx_get_context(context_id, &p_ctx);
        if((p_li2_shpns_coring != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
            (void)amba_ik_system_lock_mutex(context_id, IK_WAIT_FOREVER);
            (void)amba_ik_system_memcpy(&p_ctx->filters.p_hiso_input_param->li2_shpns_coring, p_li2_shpns_coring, sizeof(ik_li2_shpns_coring_t));
            ctx_raise_update_flag(&p_ctx->filters.update_flags.liso.li2_shpns_coring_updated);
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

uint32 img_ctx_ivd_get_li2_shpns_coring(uint32 context_id, ik_li2_shpns_coring_t *p_li2_shpns_coring)
{
    amba_ik_system_print_str_5("[IK] get_li2_shpns_coring is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //TBD
    }
    if (p_li2_shpns_coring == NULL) {
        //TBD
    } else {
        (void)amba_ik_system_memcpy(p_li2_shpns_coring, p_li2_shpns_coring, sizeof(uint8));
    }
    return IK_ERR_0004;
}

uint32 img_ctx_get_li2_shpns_coring(uint32 context_id, ik_li2_shpns_coring_t *p_li2_shpns_coring)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval |= img_ctx_get_context(context_id, &p_ctx);
    if((p_li2_shpns_coring != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        (void)amba_ik_system_lock_mutex(context_id, IK_WAIT_FOREVER);
        (void)amba_ik_system_memcpy(p_li2_shpns_coring, &p_ctx->filters.p_hiso_input_param->li2_shpns_coring, sizeof(ik_li2_shpns_coring_t));
        (void)amba_ik_system_unlock_mutex(context_id);
    } else {
        if(p_li2_shpns_coring == NULL) {
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

uint32 img_ctx_ivd_set_li2_shpns_fir(uint32 context_id, const ik_li2_shpns_fir_t *p_li2_shpns_fir)
{
    amba_ik_system_print_str_5("[IK] set_li2_shpns_fir is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //TBD
    }
    if (p_li2_shpns_fir==NULL) {
        //TBD
    }
    return IK_ERR_0004;
}

uint32 img_ctx_set_li2_shpns_fir(uint32 context_id, const ik_li2_shpns_fir_t *p_li2_shpns_fir)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;
    const ik_debug_check_func_t* p_check_func = ik_ctx_get_debug_check_func();
    if(p_check_func != NULL) {
        rval = p_check_func->li2_shpns_fir(p_li2_shpns_fir, "li2_shpns_fir");
    }
    if(rval == IK_OK) {
        rval |= img_ctx_get_context(context_id, &p_ctx);
        if((p_li2_shpns_fir != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
            (void)amba_ik_system_lock_mutex(context_id, IK_WAIT_FOREVER);
            (void)amba_ik_system_memcpy(&p_ctx->filters.p_hiso_input_param->li2_shpns_fir, p_li2_shpns_fir, sizeof(ik_li2_shpns_fir_t));
            ctx_raise_update_flag(&p_ctx->filters.update_flags.liso.li2_shpns_fir_updated);
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

uint32 img_ctx_ivd_get_li2_shpns_fir(uint32 context_id, ik_li2_shpns_fir_t *p_li2_shpns_fir)
{
    amba_ik_system_print_str_5("[IK] get_li2_shpns_fir is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //TBD
    }
    if (p_li2_shpns_fir == NULL) {
        //TBD
    } else {
        (void)amba_ik_system_memcpy(p_li2_shpns_fir, p_li2_shpns_fir, sizeof(uint8));
    }
    return IK_ERR_0004;
}

uint32 img_ctx_get_li2_shpns_fir(uint32 context_id, ik_li2_shpns_fir_t *p_li2_shpns_fir)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval |= img_ctx_get_context(context_id, &p_ctx);
    if((p_li2_shpns_fir != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        (void)amba_ik_system_lock_mutex(context_id, IK_WAIT_FOREVER);
        (void)amba_ik_system_memcpy(p_li2_shpns_fir, &p_ctx->filters.p_hiso_input_param->li2_shpns_fir, sizeof(ik_li2_shpns_fir_t));
        (void)amba_ik_system_unlock_mutex(context_id);
    } else {
        if(p_li2_shpns_fir == NULL) {
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

uint32 img_ctx_ivd_set_li2_shpns_cor_idx_scl(uint32 context_id, const ik_li2_shpns_cor_idx_scl_t *p_li2_shpns_cor_idx_scl)
{
    amba_ik_system_print_str_5("[IK] set_li2_shpns_cor_idx_scl is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //TBD
    }
    if (p_li2_shpns_cor_idx_scl==NULL) {
        //TBD
    }
    return IK_ERR_0004;
}

uint32 img_ctx_set_li2_shpns_cor_idx_scl(uint32 context_id, const ik_li2_shpns_cor_idx_scl_t *p_li2_shpns_cor_idx_scl)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;
    const ik_debug_check_func_t* p_check_func = ik_ctx_get_debug_check_func();
    if(p_check_func != NULL) {
        rval = p_check_func->li2_shpns_cor_idx_scl(p_li2_shpns_cor_idx_scl, "li2_shpns_cor_idx_scl");
    }
    if(rval == IK_OK) {
        rval |= img_ctx_get_context(context_id, &p_ctx);
        if((p_li2_shpns_cor_idx_scl != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
            (void)amba_ik_system_lock_mutex(context_id, IK_WAIT_FOREVER);
            (void)amba_ik_system_memcpy(&p_ctx->filters.p_hiso_input_param->li2_shpns_cor_idx_scl, p_li2_shpns_cor_idx_scl, sizeof(ik_li2_shpns_cor_idx_scl_t));
            ctx_raise_update_flag(&p_ctx->filters.update_flags.liso.li2_shpns_cor_idx_scl_updated);
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

uint32 img_ctx_ivd_get_li2_shpns_cor_idx_scl(uint32 context_id, ik_li2_shpns_cor_idx_scl_t *p_li2_shpns_cor_idx_scl)
{
    amba_ik_system_print_str_5("[IK] get_li2_shpns_cor_idx_scl is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //TBD
    }
    if (p_li2_shpns_cor_idx_scl == NULL) {
        //TBD
    } else {
        (void)amba_ik_system_memcpy(p_li2_shpns_cor_idx_scl, p_li2_shpns_cor_idx_scl, sizeof(uint8));
    }
    return IK_ERR_0004;
}

uint32 img_ctx_get_li2_shpns_cor_idx_scl(uint32 context_id, ik_li2_shpns_cor_idx_scl_t *p_li2_shpns_cor_idx_scl)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval |= img_ctx_get_context(context_id, &p_ctx);
    if((p_li2_shpns_cor_idx_scl != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        (void)amba_ik_system_lock_mutex(context_id, IK_WAIT_FOREVER);
        (void)amba_ik_system_memcpy(p_li2_shpns_cor_idx_scl, &p_ctx->filters.p_hiso_input_param->li2_shpns_cor_idx_scl, sizeof(ik_li2_shpns_cor_idx_scl_t));
        (void)amba_ik_system_unlock_mutex(context_id);
    } else {
        if(p_li2_shpns_cor_idx_scl == NULL) {
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

uint32 img_ctx_ivd_set_li2_shpns_min_cor_rst(uint32 context_id, const ik_li2_shpns_min_cor_rst_t *p_li2_shpns_min_cor_rst)
{
    amba_ik_system_print_str_5("[IK] set_li2_shpns_min_cor_rst is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //TBD
    }
    if (p_li2_shpns_min_cor_rst==NULL) {
        //TBD
    }
    return IK_ERR_0004;
}

uint32 img_ctx_set_li2_shpns_min_cor_rst(uint32 context_id, const ik_li2_shpns_min_cor_rst_t *p_li2_shpns_min_cor_rst)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;
    const ik_debug_check_func_t* p_check_func = ik_ctx_get_debug_check_func();
    if(p_check_func != NULL) {
        rval = p_check_func->li2_shpns_min_cor_rst(p_li2_shpns_min_cor_rst, "li2_shpns_min_cor_rst");
    }
    if(rval == IK_OK) {
        rval |= img_ctx_get_context(context_id, &p_ctx);
        if((p_li2_shpns_min_cor_rst != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
            (void)amba_ik_system_lock_mutex(context_id, IK_WAIT_FOREVER);
            (void)amba_ik_system_memcpy(&p_ctx->filters.p_hiso_input_param->li2_shpns_min_cor_rst, p_li2_shpns_min_cor_rst, sizeof(ik_li2_shpns_min_cor_rst_t));
            ctx_raise_update_flag(&p_ctx->filters.update_flags.liso.li2_shpns_min_cor_rst_updated);
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

uint32 img_ctx_ivd_get_li2_shpns_min_cor_rst(uint32 context_id, ik_li2_shpns_min_cor_rst_t *p_li2_shpns_min_cor_rst)
{
    amba_ik_system_print_str_5("[IK] get_li2_shpns_min_cor_rst is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //TBD
    }
    if (p_li2_shpns_min_cor_rst == NULL) {
        //TBD
    } else {
        (void)amba_ik_system_memcpy(p_li2_shpns_min_cor_rst, p_li2_shpns_min_cor_rst, sizeof(uint8));
    }
    return IK_ERR_0004;
}

uint32 img_ctx_get_li2_shpns_min_cor_rst(uint32 context_id, ik_li2_shpns_min_cor_rst_t *p_li2_shpns_min_cor_rst)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval |= img_ctx_get_context(context_id, &p_ctx);
    if((p_li2_shpns_min_cor_rst != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        (void)amba_ik_system_lock_mutex(context_id, IK_WAIT_FOREVER);
        (void)amba_ik_system_memcpy(p_li2_shpns_min_cor_rst, &p_ctx->filters.p_hiso_input_param->li2_shpns_min_cor_rst, sizeof(ik_li2_shpns_min_cor_rst_t));
        (void)amba_ik_system_unlock_mutex(context_id);
    } else {
        if(p_li2_shpns_min_cor_rst == NULL) {
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

uint32 img_ctx_ivd_set_li2_shpns_max_cor_rst(uint32 context_id, const ik_li2_shpns_max_cor_rst_t *p_li2_shpns_max_cor_rst)
{
    amba_ik_system_print_str_5("[IK] set_li2_shpns_max_cor_rst is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //TBD
    }
    if (p_li2_shpns_max_cor_rst==NULL) {
        //TBD
    }
    return IK_ERR_0004;
}

uint32 img_ctx_set_li2_shpns_max_cor_rst(uint32 context_id, const ik_li2_shpns_max_cor_rst_t *p_li2_shpns_max_cor_rst)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;
    const ik_debug_check_func_t* p_check_func = ik_ctx_get_debug_check_func();
    if(p_check_func != NULL) {
        rval = p_check_func->li2_shpns_max_cor_rst(p_li2_shpns_max_cor_rst, "li2_shpns_max_cor_rst");
    }
    if(rval == IK_OK) {
        rval |= img_ctx_get_context(context_id, &p_ctx);
        if((p_li2_shpns_max_cor_rst != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
            (void)amba_ik_system_lock_mutex(context_id, IK_WAIT_FOREVER);
            (void)amba_ik_system_memcpy(&p_ctx->filters.p_hiso_input_param->li2_shpns_max_cor_rst, p_li2_shpns_max_cor_rst, sizeof(ik_li2_shpns_max_cor_rst_t));
            ctx_raise_update_flag(&p_ctx->filters.update_flags.liso.li2_shpns_max_cor_rst_updated);
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

uint32 img_ctx_ivd_get_li2_shpns_max_cor_rst(uint32 context_id, ik_li2_shpns_max_cor_rst_t *p_li2_shpns_max_cor_rst)
{
    amba_ik_system_print_str_5("[IK] get_li2_shpns_max_cor_rst is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //TBD
    }
    if (p_li2_shpns_max_cor_rst == NULL) {
        //TBD
    } else {
        (void)amba_ik_system_memcpy(p_li2_shpns_max_cor_rst, p_li2_shpns_max_cor_rst, sizeof(uint8));
    }
    return IK_ERR_0004;
}

uint32 img_ctx_get_li2_shpns_max_cor_rst(uint32 context_id, ik_li2_shpns_max_cor_rst_t *p_li2_shpns_max_cor_rst)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval |= img_ctx_get_context(context_id, &p_ctx);
    if((p_li2_shpns_max_cor_rst != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        (void)amba_ik_system_lock_mutex(context_id, IK_WAIT_FOREVER);
        (void)amba_ik_system_memcpy(p_li2_shpns_max_cor_rst, &p_ctx->filters.p_hiso_input_param->li2_shpns_max_cor_rst, sizeof(ik_li2_shpns_max_cor_rst_t));
        (void)amba_ik_system_unlock_mutex(context_id);
    } else {
        if(p_li2_shpns_max_cor_rst == NULL) {
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

uint32 img_ctx_ivd_set_li2_shpns_scl_cor(uint32 context_id, const ik_li2_shpns_scl_cor_t *p_li2_shpns_scl_cor)
{
    amba_ik_system_print_str_5("[IK] set_li2_shpns_scl_cor is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //TBD
    }
    if (p_li2_shpns_scl_cor==NULL) {
        //TBD
    }
    return IK_ERR_0004;
}

uint32 img_ctx_set_li2_shpns_scl_cor(uint32 context_id, const ik_li2_shpns_scl_cor_t *p_li2_shpns_scl_cor)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;
    const ik_debug_check_func_t* p_check_func = ik_ctx_get_debug_check_func();
    if(p_check_func != NULL) {
        rval = p_check_func->li2_shpns_scl_cor(p_li2_shpns_scl_cor, "li2_shpns_scl_cor");
    }
    if(rval == IK_OK) {
        rval |= img_ctx_get_context(context_id, &p_ctx);
        if((p_li2_shpns_scl_cor != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
            (void)amba_ik_system_lock_mutex(context_id, IK_WAIT_FOREVER);
            (void)amba_ik_system_memcpy(&p_ctx->filters.p_hiso_input_param->li2_shpns_scl_cor, p_li2_shpns_scl_cor, sizeof(ik_li2_shpns_scl_cor_t));
            ctx_raise_update_flag(&p_ctx->filters.update_flags.liso.li2_shpns_scl_cor_updated);
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

uint32 img_ctx_ivd_get_li2_shpns_scl_cor(uint32 context_id, ik_li2_shpns_scl_cor_t *p_li2_shpns_scl_cor)
{
    amba_ik_system_print_str_5("[IK] get_li2_shpns_scl_cor is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //TBD
    }
    if (p_li2_shpns_scl_cor == NULL) {
        //TBD
    } else {
        (void)amba_ik_system_memcpy(p_li2_shpns_scl_cor, p_li2_shpns_scl_cor, sizeof(uint8));
    }
    return IK_ERR_0004;
}

uint32 img_ctx_get_li2_shpns_scl_cor(uint32 context_id, ik_li2_shpns_scl_cor_t *p_li2_shpns_scl_cor)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval |= img_ctx_get_context(context_id, &p_ctx);
    if((p_li2_shpns_scl_cor != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        (void)amba_ik_system_lock_mutex(context_id, IK_WAIT_FOREVER);
        (void)amba_ik_system_memcpy(p_li2_shpns_scl_cor, &p_ctx->filters.p_hiso_input_param->li2_shpns_scl_cor, sizeof(ik_li2_shpns_scl_cor_t));
        (void)amba_ik_system_unlock_mutex(context_id);
    } else {
        if(p_li2_shpns_scl_cor == NULL) {
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

uint32 img_ctx_ivd_set_hili_shpns_both(uint32 context_id, const ik_hili_shpns_both_t *p_hili_shpns_both)
{
    amba_ik_system_print_str_5("[IK] set_hili_shpns_both is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //TBD
    }
    if (p_hili_shpns_both==NULL) {
        //TBD
    }
    return IK_ERR_0004;
}

uint32 img_ctx_set_hili_shpns_both(uint32 context_id, const ik_hili_shpns_both_t *p_hili_shpns_both)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;
    const ik_debug_check_func_t* p_check_func = ik_ctx_get_debug_check_func();
    if(p_check_func != NULL) {
        rval = p_check_func->hili_shpns_both(p_hili_shpns_both, "hili_shpns_both");
    }
    if(rval == IK_OK) {
        rval |= img_ctx_get_context(context_id, &p_ctx);
        if((p_hili_shpns_both != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
            (void)amba_ik_system_lock_mutex(context_id, IK_WAIT_FOREVER);
            (void)amba_ik_system_memcpy(&p_ctx->filters.p_hiso_input_param->hili_shpns_both, p_hili_shpns_both, sizeof(ik_hili_shpns_both_t));
            ctx_raise_update_flag(&p_ctx->filters.update_flags.liso.hili_shpns_both_updated);
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

uint32 img_ctx_ivd_get_hili_shpns_both(uint32 context_id, ik_hili_shpns_both_t *p_hili_shpns_both)
{
    amba_ik_system_print_str_5("[IK] get_hili_shpns_both is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //TBD
    }
    if (p_hili_shpns_both == NULL) {
        //TBD
    } else {
        (void)amba_ik_system_memcpy(p_hili_shpns_both, p_hili_shpns_both, sizeof(uint8));
    }
    return IK_ERR_0004;
}

uint32 img_ctx_get_hili_shpns_both(uint32 context_id, ik_hili_shpns_both_t *p_hili_shpns_both)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval |= img_ctx_get_context(context_id, &p_ctx);
    if((p_hili_shpns_both != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        (void)amba_ik_system_lock_mutex(context_id, IK_WAIT_FOREVER);
        (void)amba_ik_system_memcpy(p_hili_shpns_both, &p_ctx->filters.p_hiso_input_param->hili_shpns_both, sizeof(ik_hili_shpns_both_t));
        (void)amba_ik_system_unlock_mutex(context_id);
    } else {
        if(p_hili_shpns_both == NULL) {
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

uint32 img_ctx_ivd_set_hili_shpns_noise(uint32 context_id, const ik_hili_shpns_noise_t *p_hili_shpns_noise)
{
    amba_ik_system_print_str_5("[IK] set_hili_shpns_noise is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //TBD
    }
    if (p_hili_shpns_noise==NULL) {
        //TBD
    }
    return IK_ERR_0004;
}

uint32 img_ctx_set_hili_shpns_noise(uint32 context_id, const ik_hili_shpns_noise_t *p_hili_shpns_noise)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;
    const ik_debug_check_func_t* p_check_func = ik_ctx_get_debug_check_func();
    if(p_check_func != NULL) {
        rval = p_check_func->hili_shpns_noise(p_hili_shpns_noise, "hili_shpns_noise");
    }
    if(rval == IK_OK) {
        rval |= img_ctx_get_context(context_id, &p_ctx);
        if((p_hili_shpns_noise != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
            (void)amba_ik_system_lock_mutex(context_id, IK_WAIT_FOREVER);
            (void)amba_ik_system_memcpy(&p_ctx->filters.p_hiso_input_param->hili_shpns_noise, p_hili_shpns_noise, sizeof(ik_hili_shpns_noise_t));
            ctx_raise_update_flag(&p_ctx->filters.update_flags.liso.hili_shpns_noise_updated);
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

uint32 img_ctx_ivd_get_hili_shpns_noise(uint32 context_id, ik_hili_shpns_noise_t *p_hili_shpns_noise)
{
    amba_ik_system_print_str_5("[IK] get_hili_shpns_noise is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //TBD
    }
    if (p_hili_shpns_noise == NULL) {
        //TBD
    } else {
        (void)amba_ik_system_memcpy(p_hili_shpns_noise, p_hili_shpns_noise, sizeof(uint8));
    }
    return IK_ERR_0004;
}

uint32 img_ctx_get_hili_shpns_noise(uint32 context_id, ik_hili_shpns_noise_t *p_hili_shpns_noise)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval |= img_ctx_get_context(context_id, &p_ctx);
    if((p_hili_shpns_noise != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        (void)amba_ik_system_lock_mutex(context_id, IK_WAIT_FOREVER);
        (void)amba_ik_system_memcpy(p_hili_shpns_noise, &p_ctx->filters.p_hiso_input_param->hili_shpns_noise, sizeof(ik_hili_shpns_noise_t));
        (void)amba_ik_system_unlock_mutex(context_id);
    } else {
        if(p_hili_shpns_noise == NULL) {
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

uint32 img_ctx_ivd_set_hili_shpns_coring(uint32 context_id, const ik_hili_shpns_coring_t *p_hili_shpns_coring)
{
    amba_ik_system_print_str_5("[IK] set_hili_shpns_coring is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //TBD
    }
    if (p_hili_shpns_coring==NULL) {
        //TBD
    }
    return IK_ERR_0004;
}

uint32 img_ctx_set_hili_shpns_coring(uint32 context_id, const ik_hili_shpns_coring_t *p_hili_shpns_coring)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;
    const ik_debug_check_func_t* p_check_func = ik_ctx_get_debug_check_func();
    if(p_check_func != NULL) {
        rval = p_check_func->hili_shpns_coring(p_hili_shpns_coring, "hili_shpns_coring");
    }
    if(rval == IK_OK) {
        rval |= img_ctx_get_context(context_id, &p_ctx);
        if((p_hili_shpns_coring != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
            (void)amba_ik_system_lock_mutex(context_id, IK_WAIT_FOREVER);
            (void)amba_ik_system_memcpy(&p_ctx->filters.p_hiso_input_param->hili_shpns_coring, p_hili_shpns_coring, sizeof(ik_hili_shpns_coring_t));
            ctx_raise_update_flag(&p_ctx->filters.update_flags.liso.hili_shpns_coring_updated);
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

uint32 img_ctx_ivd_get_hili_shpns_coring(uint32 context_id, ik_hili_shpns_coring_t *p_hili_shpns_coring)
{
    amba_ik_system_print_str_5("[IK] get_hili_shpns_coring is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //TBD
    }
    if (p_hili_shpns_coring == NULL) {
        //TBD
    } else {
        (void)amba_ik_system_memcpy(p_hili_shpns_coring, p_hili_shpns_coring, sizeof(uint8));
    }
    return IK_ERR_0004;
}

uint32 img_ctx_get_hili_shpns_coring(uint32 context_id, ik_hili_shpns_coring_t *p_hili_shpns_coring)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval |= img_ctx_get_context(context_id, &p_ctx);
    if((p_hili_shpns_coring != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        (void)amba_ik_system_lock_mutex(context_id, IK_WAIT_FOREVER);
        (void)amba_ik_system_memcpy(p_hili_shpns_coring, &p_ctx->filters.p_hiso_input_param->hili_shpns_coring, sizeof(ik_hili_shpns_coring_t));
        (void)amba_ik_system_unlock_mutex(context_id);
    } else {
        if(p_hili_shpns_coring == NULL) {
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

uint32 img_ctx_ivd_set_hili_shpns_fir(uint32 context_id, const ik_hili_shpns_fir_t *p_hili_shpns_fir)
{
    amba_ik_system_print_str_5("[IK] set_hili_shpns_fir is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //TBD
    }
    if (p_hili_shpns_fir==NULL) {
        //TBD
    }
    return IK_ERR_0004;
}

uint32 img_ctx_set_hili_shpns_fir(uint32 context_id, const ik_hili_shpns_fir_t *p_hili_shpns_fir)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;
    const ik_debug_check_func_t* p_check_func = ik_ctx_get_debug_check_func();
    if(p_check_func != NULL) {
        rval = p_check_func->hili_shpns_fir(p_hili_shpns_fir, "hili_shpns_fir");
    }
    if(rval == IK_OK) {
        rval |= img_ctx_get_context(context_id, &p_ctx);
        if((p_hili_shpns_fir != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
            (void)amba_ik_system_lock_mutex(context_id, IK_WAIT_FOREVER);
            (void)amba_ik_system_memcpy(&p_ctx->filters.p_hiso_input_param->hili_shpns_fir, p_hili_shpns_fir, sizeof(ik_hili_shpns_fir_t));
            ctx_raise_update_flag(&p_ctx->filters.update_flags.liso.hili_shpns_fir_updated);
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

uint32 img_ctx_ivd_get_hili_shpns_fir(uint32 context_id, ik_hili_shpns_fir_t *p_hili_shpns_fir)
{
    amba_ik_system_print_str_5("[IK] get_hili_shpns_fir is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //TBD
    }
    if (p_hili_shpns_fir == NULL) {
        //TBD
    } else {
        (void)amba_ik_system_memcpy(p_hili_shpns_fir, p_hili_shpns_fir, sizeof(uint8));
    }
    return IK_ERR_0004;
}

uint32 img_ctx_get_hili_shpns_fir(uint32 context_id, ik_hili_shpns_fir_t *p_hili_shpns_fir)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval |= img_ctx_get_context(context_id, &p_ctx);
    if((p_hili_shpns_fir != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        (void)amba_ik_system_lock_mutex(context_id, IK_WAIT_FOREVER);
        (void)amba_ik_system_memcpy(p_hili_shpns_fir, &p_ctx->filters.p_hiso_input_param->hili_shpns_fir, sizeof(ik_hili_shpns_fir_t));
        (void)amba_ik_system_unlock_mutex(context_id);
    } else {
        if(p_hili_shpns_fir == NULL) {
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

uint32 img_ctx_ivd_set_hili_shpns_cor_idx_scl(uint32 context_id, const ik_hili_shpns_cor_idx_scl_t *p_hili_shpns_cor_idx_scl)
{
    amba_ik_system_print_str_5("[IK] set_hili_shpns_cor_idx_scl is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //TBD
    }
    if (p_hili_shpns_cor_idx_scl==NULL) {
        //TBD
    }
    return IK_ERR_0004;
}

uint32 img_ctx_set_hili_shpns_cor_idx_scl(uint32 context_id, const ik_hili_shpns_cor_idx_scl_t *p_hili_shpns_cor_idx_scl)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;
    const ik_debug_check_func_t* p_check_func = ik_ctx_get_debug_check_func();
    if(p_check_func != NULL) {
        rval = p_check_func->hili_shpns_cor_idx_scl(p_hili_shpns_cor_idx_scl, "hili_shpns_cor_idx_scl");
    }
    if(rval == IK_OK) {
        rval |= img_ctx_get_context(context_id, &p_ctx);
        if((p_hili_shpns_cor_idx_scl != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
            (void)amba_ik_system_lock_mutex(context_id, IK_WAIT_FOREVER);
            (void)amba_ik_system_memcpy(&p_ctx->filters.p_hiso_input_param->hili_shpns_cor_idx_scl, p_hili_shpns_cor_idx_scl, sizeof(ik_hili_shpns_cor_idx_scl_t));
            ctx_raise_update_flag(&p_ctx->filters.update_flags.liso.hili_shpns_cor_idx_scl_updated);
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

uint32 img_ctx_ivd_get_hili_shpns_cor_idx_scl(uint32 context_id, ik_hili_shpns_cor_idx_scl_t *p_hili_shpns_cor_idx_scl)
{
    amba_ik_system_print_str_5("[IK] get_hili_shpns_cor_idx_scl is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //TBD
    }
    if (p_hili_shpns_cor_idx_scl == NULL) {
        //TBD
    } else {
        (void)amba_ik_system_memcpy(p_hili_shpns_cor_idx_scl, p_hili_shpns_cor_idx_scl, sizeof(uint8));
    }
    return IK_ERR_0004;
}

uint32 img_ctx_get_hili_shpns_cor_idx_scl(uint32 context_id, ik_hili_shpns_cor_idx_scl_t *p_hili_shpns_cor_idx_scl)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval |= img_ctx_get_context(context_id, &p_ctx);
    if((p_hili_shpns_cor_idx_scl != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        (void)amba_ik_system_lock_mutex(context_id, IK_WAIT_FOREVER);
        (void)amba_ik_system_memcpy(p_hili_shpns_cor_idx_scl, &p_ctx->filters.p_hiso_input_param->hili_shpns_cor_idx_scl, sizeof(ik_hili_shpns_cor_idx_scl_t));
        (void)amba_ik_system_unlock_mutex(context_id);
    } else {
        if(p_hili_shpns_cor_idx_scl == NULL) {
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

uint32 img_ctx_ivd_set_hili_shpns_min_cor_rst(uint32 context_id, const ik_hili_shpns_min_cor_rst_t *p_hili_shpns_min_cor_rst)
{
    amba_ik_system_print_str_5("[IK] set_hili_shpns_min_cor_rst is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //TBD
    }
    if (p_hili_shpns_min_cor_rst==NULL) {
        //TBD
    }
    return IK_ERR_0004;
}

uint32 img_ctx_set_hili_shpns_min_cor_rst(uint32 context_id, const ik_hili_shpns_min_cor_rst_t *p_hili_shpns_min_cor_rst)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;
    const ik_debug_check_func_t* p_check_func = ik_ctx_get_debug_check_func();
    if(p_check_func != NULL) {
        rval = p_check_func->hili_shpns_min_cor_rst(p_hili_shpns_min_cor_rst, "hili_shpns_min_cor_rst");
    }
    if(rval == IK_OK) {
        rval |= img_ctx_get_context(context_id, &p_ctx);
        if((p_hili_shpns_min_cor_rst != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
            (void)amba_ik_system_lock_mutex(context_id, IK_WAIT_FOREVER);
            (void)amba_ik_system_memcpy(&p_ctx->filters.p_hiso_input_param->hili_shpns_min_cor_rst, p_hili_shpns_min_cor_rst, sizeof(ik_hili_shpns_min_cor_rst_t));
            ctx_raise_update_flag(&p_ctx->filters.update_flags.liso.hili_shpns_min_cor_rst_updated);
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

uint32 img_ctx_ivd_get_hili_shpns_min_cor_rst(uint32 context_id, ik_hili_shpns_min_cor_rst_t *p_hili_shpns_min_cor_rst)
{
    amba_ik_system_print_str_5("[IK] get_hili_shpns_min_cor_rst is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //TBD
    }
    if (p_hili_shpns_min_cor_rst == NULL) {
        //TBD
    } else {
        (void)amba_ik_system_memcpy(p_hili_shpns_min_cor_rst, p_hili_shpns_min_cor_rst, sizeof(uint8));
    }
    return IK_ERR_0004;
}

uint32 img_ctx_get_hili_shpns_min_cor_rst(uint32 context_id, ik_hili_shpns_min_cor_rst_t *p_hili_shpns_min_cor_rst)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval |= img_ctx_get_context(context_id, &p_ctx);
    if((p_hili_shpns_min_cor_rst != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        (void)amba_ik_system_lock_mutex(context_id, IK_WAIT_FOREVER);
        (void)amba_ik_system_memcpy(p_hili_shpns_min_cor_rst, &p_ctx->filters.p_hiso_input_param->hili_shpns_min_cor_rst, sizeof(ik_hili_shpns_min_cor_rst_t));
        (void)amba_ik_system_unlock_mutex(context_id);
    } else {
        if(p_hili_shpns_min_cor_rst == NULL) {
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

uint32 img_ctx_ivd_set_hili_shpns_max_cor_rst(uint32 context_id, const ik_hili_shpns_max_cor_rst_t *p_hili_shpns_max_cor_rst)
{
    amba_ik_system_print_str_5("[IK] set_hili_shpns_max_cor_rst is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //TBD
    }
    if (p_hili_shpns_max_cor_rst==NULL) {
        //TBD
    }
    return IK_ERR_0004;
}

uint32 img_ctx_set_hili_shpns_max_cor_rst(uint32 context_id, const ik_hili_shpns_max_cor_rst_t *p_hili_shpns_max_cor_rst)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;
    const ik_debug_check_func_t* p_check_func = ik_ctx_get_debug_check_func();
    if(p_check_func != NULL) {
        rval = p_check_func->hili_shpns_max_cor_rst(p_hili_shpns_max_cor_rst, "hili_shpns_max_cor_rst");
    }
    if(rval == IK_OK) {
        rval |= img_ctx_get_context(context_id, &p_ctx);
        if((p_hili_shpns_max_cor_rst != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
            (void)amba_ik_system_lock_mutex(context_id, IK_WAIT_FOREVER);
            (void)amba_ik_system_memcpy(&p_ctx->filters.p_hiso_input_param->hili_shpns_max_cor_rst, p_hili_shpns_max_cor_rst, sizeof(ik_hili_shpns_max_cor_rst_t));
            ctx_raise_update_flag(&p_ctx->filters.update_flags.liso.hili_shpns_max_cor_rst_updated);
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

uint32 img_ctx_ivd_get_hili_shpns_max_cor_rst(uint32 context_id, ik_hili_shpns_max_cor_rst_t *p_hili_shpns_max_cor_rst)
{
    amba_ik_system_print_str_5("[IK] get_hili_shpns_max_cor_rst is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //TBD
    }
    if (p_hili_shpns_max_cor_rst == NULL) {
        //TBD
    } else {
        (void)amba_ik_system_memcpy(p_hili_shpns_max_cor_rst, p_hili_shpns_max_cor_rst, sizeof(uint8));
    }
    return IK_ERR_0004;
}

uint32 img_ctx_get_hili_shpns_max_cor_rst(uint32 context_id, ik_hili_shpns_max_cor_rst_t *p_hili_shpns_max_cor_rst)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval |= img_ctx_get_context(context_id, &p_ctx);
    if((p_hili_shpns_max_cor_rst != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        (void)amba_ik_system_lock_mutex(context_id, IK_WAIT_FOREVER);
        (void)amba_ik_system_memcpy(p_hili_shpns_max_cor_rst, &p_ctx->filters.p_hiso_input_param->hili_shpns_max_cor_rst, sizeof(ik_hili_shpns_max_cor_rst_t));
        (void)amba_ik_system_unlock_mutex(context_id);
    } else {
        if(p_hili_shpns_max_cor_rst == NULL) {
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

uint32 img_ctx_ivd_set_hili_shpns_scl_cor(uint32 context_id, const ik_hili_shpns_scl_cor_t *p_hili_shpns_scl_cor)
{
    amba_ik_system_print_str_5("[IK] set_hili_shpns_scl_cor is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //TBD
    }
    if (p_hili_shpns_scl_cor==NULL) {
        //TBD
    }
    return IK_ERR_0004;
}

uint32 img_ctx_set_hili_shpns_scl_cor(uint32 context_id, const ik_hili_shpns_scl_cor_t *p_hili_shpns_scl_cor)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;
    const ik_debug_check_func_t* p_check_func = ik_ctx_get_debug_check_func();
    if(p_check_func != NULL) {
        rval = p_check_func->hili_shpns_scl_cor(p_hili_shpns_scl_cor, "hili_shpns_scl_cor");
    }
    if(rval == IK_OK) {
        rval |= img_ctx_get_context(context_id, &p_ctx);
        if((p_hili_shpns_scl_cor != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
            (void)amba_ik_system_lock_mutex(context_id, IK_WAIT_FOREVER);
            (void)amba_ik_system_memcpy(&p_ctx->filters.p_hiso_input_param->hili_shpns_scl_cor, p_hili_shpns_scl_cor, sizeof(ik_hili_shpns_scl_cor_t));
            ctx_raise_update_flag(&p_ctx->filters.update_flags.liso.hili_shpns_scl_cor_updated);
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

uint32 img_ctx_ivd_get_hili_shpns_scl_cor(uint32 context_id, ik_hili_shpns_scl_cor_t *p_hili_shpns_scl_cor)
{
    amba_ik_system_print_str_5("[IK] get_hili_shpns_scl_cor is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //TBD
    }
    if (p_hili_shpns_scl_cor == NULL) {
        //TBD
    } else {
        (void)amba_ik_system_memcpy(p_hili_shpns_scl_cor, p_hili_shpns_scl_cor, sizeof(uint8));
    }
    return IK_ERR_0004;
}

uint32 img_ctx_get_hili_shpns_scl_cor(uint32 context_id, ik_hili_shpns_scl_cor_t *p_hili_shpns_scl_cor)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval |= img_ctx_get_context(context_id, &p_ctx);
    if((p_hili_shpns_scl_cor != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        (void)amba_ik_system_lock_mutex(context_id, IK_WAIT_FOREVER);
        (void)amba_ik_system_memcpy(p_hili_shpns_scl_cor, &p_ctx->filters.p_hiso_input_param->hili_shpns_scl_cor, sizeof(ik_hili_shpns_scl_cor_t));
        (void)amba_ik_system_unlock_mutex(context_id);
    } else {
        if(p_hili_shpns_scl_cor == NULL) {
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

uint32 img_ctx_ivd_set_hi_chrm_fltr_high(uint32 context_id, const ik_hi_chroma_filter_high_t *p_hi_chroma_filter_high)
{
    amba_ik_system_print_str_5("[IK] set_hi_chroma_filter_high is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //TBD
    }
    if (p_hi_chroma_filter_high==NULL) {
        //TBD
    }
    return IK_ERR_0004;
}

uint32 img_ctx_set_hi_chroma_filter_high(uint32 context_id, const ik_hi_chroma_filter_high_t *p_hi_chroma_filter_high)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;
    const ik_debug_check_func_t* p_check_func = ik_ctx_get_debug_check_func();
    if(p_check_func != NULL) {
        rval = p_check_func->hi_chroma_filter_high(p_hi_chroma_filter_high, "hi_chroma_filter_high");
    }
    if(rval == IK_OK) {
        rval |= img_ctx_get_context(context_id, &p_ctx);
        if((p_hi_chroma_filter_high != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
            (void)amba_ik_system_lock_mutex(context_id, IK_WAIT_FOREVER);
            (void)amba_ik_system_memcpy(&p_ctx->filters.p_hiso_input_param->hi_chroma_filter_high, p_hi_chroma_filter_high, sizeof(ik_hi_chroma_filter_high_t));
            ctx_raise_update_flag(&p_ctx->filters.update_flags.liso.hi_chroma_filter_high_updated);
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

uint32 img_ctx_ivd_get_hi_chrm_fltr_high(uint32 context_id, ik_hi_chroma_filter_high_t *p_hi_chroma_filter_high)
{
    amba_ik_system_print_str_5("[IK] get_hi_chroma_filter_high is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //TBD
    }
    if (p_hi_chroma_filter_high == NULL) {
        //TBD
    } else {
        (void)amba_ik_system_memcpy(p_hi_chroma_filter_high, p_hi_chroma_filter_high, sizeof(uint8));
    }
    return IK_ERR_0004;
}

uint32 img_ctx_get_hi_chroma_filter_high(uint32 context_id, ik_hi_chroma_filter_high_t *p_hi_chroma_filter_high)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval |= img_ctx_get_context(context_id, &p_ctx);
    if((p_hi_chroma_filter_high != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        (void)amba_ik_system_lock_mutex(context_id, IK_WAIT_FOREVER);
        (void)amba_ik_system_memcpy(p_hi_chroma_filter_high, &p_ctx->filters.p_hiso_input_param->hi_chroma_filter_high, sizeof(ik_hi_chroma_filter_high_t));
        (void)amba_ik_system_unlock_mutex(context_id);
    } else {
        if(p_hi_chroma_filter_high == NULL) {
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

uint32 img_ctx_ivd_set_hi_chrm_fltr_pre(uint32 context_id, const ik_hi_chroma_filter_pre_t *p_hi_chroma_filter_pre)
{
    amba_ik_system_print_str_5("[IK] set_hi_chroma_filter_pre is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //TBD
    }
    if (p_hi_chroma_filter_pre==NULL) {
        //TBD
    }
    return IK_ERR_0004;
}

uint32 img_ctx_set_hi_chroma_filter_pre(uint32 context_id, const ik_hi_chroma_filter_pre_t *p_hi_chroma_filter_pre)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;
    const ik_debug_check_func_t* p_check_func = ik_ctx_get_debug_check_func();
    if(p_check_func != NULL) {
        rval = p_check_func->hi_chroma_filter_pre(p_hi_chroma_filter_pre, "hi_chroma_filter_pre");
    }
    if(rval == IK_OK) {
        rval |= img_ctx_get_context(context_id, &p_ctx);
        if((p_hi_chroma_filter_pre != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
            (void)amba_ik_system_lock_mutex(context_id, IK_WAIT_FOREVER);
            (void)amba_ik_system_memcpy(&p_ctx->filters.p_hiso_input_param->hi_chroma_filter_pre, p_hi_chroma_filter_pre, sizeof(ik_hi_chroma_filter_pre_t));
            ctx_raise_update_flag(&p_ctx->filters.update_flags.liso.hi_chroma_filter_pre_updated);
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

uint32 img_ctx_ivd_get_hi_chrm_fltr_pre(uint32 context_id, ik_hi_chroma_filter_pre_t *p_hi_chroma_filter_pre)
{
    amba_ik_system_print_str_5("[IK] get_hi_chroma_filter_pre is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //TBD
    }
    if (p_hi_chroma_filter_pre == NULL) {
        //TBD
    } else {
        (void)amba_ik_system_memcpy(p_hi_chroma_filter_pre, p_hi_chroma_filter_pre, sizeof(uint8));
    }
    return IK_ERR_0004;
}

uint32 img_ctx_get_hi_chroma_filter_pre(uint32 context_id, ik_hi_chroma_filter_pre_t *p_hi_chroma_filter_pre)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval |= img_ctx_get_context(context_id, &p_ctx);
    if((p_hi_chroma_filter_pre != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        (void)amba_ik_system_lock_mutex(context_id, IK_WAIT_FOREVER);
        (void)amba_ik_system_memcpy(p_hi_chroma_filter_pre, &p_ctx->filters.p_hiso_input_param->hi_chroma_filter_pre, sizeof(ik_hi_chroma_filter_pre_t));
        (void)amba_ik_system_unlock_mutex(context_id);
    } else {
        if(p_hi_chroma_filter_pre == NULL) {
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

uint32 img_ctx_ivd_set_hi_chrm_fltr_med(uint32 context_id, const ik_hi_chroma_filter_med_t *p_hi_chroma_filter_med)
{
    amba_ik_system_print_str_5("[IK] set_hi_chroma_filter_med is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //TBD
    }
    if (p_hi_chroma_filter_med==NULL) {
        //TBD
    }
    return IK_ERR_0004;
}

uint32 img_ctx_set_hi_chroma_filter_med(uint32 context_id, const ik_hi_chroma_filter_med_t *p_hi_chroma_filter_med)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;
    const ik_debug_check_func_t* p_check_func = ik_ctx_get_debug_check_func();
    if(p_check_func != NULL) {
        rval = p_check_func->hi_chroma_filter_med(p_hi_chroma_filter_med, "hi_chroma_filter_med");
    }
    if(rval == IK_OK) {
        rval |= img_ctx_get_context(context_id, &p_ctx);
        if((p_hi_chroma_filter_med != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
            (void)amba_ik_system_lock_mutex(context_id, IK_WAIT_FOREVER);
            (void)amba_ik_system_memcpy(&p_ctx->filters.p_hiso_input_param->hi_chroma_filter_med, p_hi_chroma_filter_med, sizeof(ik_hi_chroma_filter_med_t));
            ctx_raise_update_flag(&p_ctx->filters.update_flags.liso.hi_chroma_filter_med_updated);
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

uint32 img_ctx_ivd_get_hi_chrm_fltr_med(uint32 context_id, ik_hi_chroma_filter_med_t *p_hi_chroma_filter_med)
{
    amba_ik_system_print_str_5("[IK] get_hi_chroma_filter_med is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //TBD
    }
    if (p_hi_chroma_filter_med == NULL) {
        //TBD
    } else {
        (void)amba_ik_system_memcpy(p_hi_chroma_filter_med, p_hi_chroma_filter_med, sizeof(uint8));
    }
    return IK_ERR_0004;
}

uint32 img_ctx_get_hi_chroma_filter_med(uint32 context_id, ik_hi_chroma_filter_med_t *p_hi_chroma_filter_med)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval |= img_ctx_get_context(context_id, &p_ctx);
    if((p_hi_chroma_filter_med != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        (void)amba_ik_system_lock_mutex(context_id, IK_WAIT_FOREVER);
        (void)amba_ik_system_memcpy(p_hi_chroma_filter_med, &p_ctx->filters.p_hiso_input_param->hi_chroma_filter_med, sizeof(ik_hi_chroma_filter_med_t));
        (void)amba_ik_system_unlock_mutex(context_id);
    } else {
        if(p_hi_chroma_filter_med == NULL) {
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

uint32 img_ctx_ivd_set_hi_chrm_filter_low(uint32 context_id, const ik_hi_chroma_filter_low_t *p_hi_chroma_filter_low)
{
    amba_ik_system_print_str_5("[IK] set_hi_chroma_filter_low is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //TBD
    }
    if (p_hi_chroma_filter_low==NULL) {
        //TBD
    }
    return IK_ERR_0004;
}

uint32 img_ctx_set_hi_chroma_filter_low(uint32 context_id, const ik_hi_chroma_filter_low_t *p_hi_chroma_filter_low)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;
    const ik_debug_check_func_t* p_check_func = ik_ctx_get_debug_check_func();
    if(p_check_func != NULL) {
        rval = p_check_func->hi_chroma_filter_low(p_hi_chroma_filter_low, "hi_chroma_filter_low");
    }
    if(rval == IK_OK) {
        rval |= img_ctx_get_context(context_id, &p_ctx);
        if((p_hi_chroma_filter_low != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
            (void)amba_ik_system_lock_mutex(context_id, IK_WAIT_FOREVER);
            (void)amba_ik_system_memcpy(&p_ctx->filters.p_hiso_input_param->hi_chroma_filter_low, p_hi_chroma_filter_low, sizeof(ik_hi_chroma_filter_low_t));
            ctx_raise_update_flag(&p_ctx->filters.update_flags.liso.hi_chroma_filter_low_updated);
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

uint32 img_ctx_ivd_get_hi_chrm_filter_low(uint32 context_id, ik_hi_chroma_filter_low_t *p_hi_chroma_filter_low)
{
    amba_ik_system_print_str_5("[IK] get_hi_chroma_filter_low is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //TBD
    }
    if (p_hi_chroma_filter_low == NULL) {
        //TBD
    } else {
        (void)amba_ik_system_memcpy(p_hi_chroma_filter_low, p_hi_chroma_filter_low, sizeof(uint8));
    }
    return IK_ERR_0004;
}

uint32 img_ctx_get_hi_chroma_filter_low(uint32 context_id, ik_hi_chroma_filter_low_t *p_hi_chroma_filter_low)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval |= img_ctx_get_context(context_id, &p_ctx);
    if((p_hi_chroma_filter_low != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        (void)amba_ik_system_lock_mutex(context_id, IK_WAIT_FOREVER);
        (void)amba_ik_system_memcpy(p_hi_chroma_filter_low, &p_ctx->filters.p_hiso_input_param->hi_chroma_filter_low, sizeof(ik_hi_chroma_filter_low_t));
        (void)amba_ik_system_unlock_mutex(context_id);
    } else {
        if(p_hi_chroma_filter_low == NULL) {
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

uint32 img_ctx_ivd_set_hi_chrm_fltr_very_low(uint32 context_id, const ik_hi_chroma_filter_very_low_t *p_hi_chroma_filter_very_low)
{
    amba_ik_system_print_str_5("[IK] set_hi_chroma_filter_very_low is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //TBD
    }
    if (p_hi_chroma_filter_very_low==NULL) {
        //TBD
    }
    return IK_ERR_0004;
}

uint32 img_ctx_set_hi_chroma_filter_very_low(uint32 context_id, const ik_hi_chroma_filter_very_low_t *p_hi_chroma_filter_very_low)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;
    const ik_debug_check_func_t* p_check_func = ik_ctx_get_debug_check_func();
    if(p_check_func != NULL) {
        rval = p_check_func->hi_chroma_filter_very_low(p_hi_chroma_filter_very_low, "hi_chroma_filter_very_low");
    }
    if(rval == IK_OK) {
        rval |= img_ctx_get_context(context_id, &p_ctx);
        if((p_hi_chroma_filter_very_low != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
            (void)amba_ik_system_lock_mutex(context_id, IK_WAIT_FOREVER);
            (void)amba_ik_system_memcpy(&p_ctx->filters.p_hiso_input_param->hi_chroma_filter_very_low, p_hi_chroma_filter_very_low, sizeof(ik_hi_chroma_filter_very_low_t));
            ctx_raise_update_flag(&p_ctx->filters.update_flags.liso.hi_chroma_filter_very_low_updated);
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

uint32 img_ctx_ivd_get_hi_chrm_fltr_very_low(uint32 context_id, ik_hi_chroma_filter_very_low_t *p_hi_chroma_filter_very_low)
{
    amba_ik_system_print_str_5("[IK] get_hi_chroma_filter_very_low is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //TBD
    }
    if (p_hi_chroma_filter_very_low == NULL) {
        //TBD
    } else {
        (void)amba_ik_system_memcpy(p_hi_chroma_filter_very_low, p_hi_chroma_filter_very_low, sizeof(uint8));
    }
    return IK_ERR_0004;
}

uint32 img_ctx_get_hi_chroma_filter_very_low(uint32 context_id, ik_hi_chroma_filter_very_low_t *p_hi_chroma_filter_very_low)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval |= img_ctx_get_context(context_id, &p_ctx);
    if((p_hi_chroma_filter_very_low != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        (void)amba_ik_system_lock_mutex(context_id, IK_WAIT_FOREVER);
        (void)amba_ik_system_memcpy(p_hi_chroma_filter_very_low, &p_ctx->filters.p_hiso_input_param->hi_chroma_filter_very_low, sizeof(ik_hi_chroma_filter_very_low_t));
        (void)amba_ik_system_unlock_mutex(context_id);
    } else {
        if(p_hi_chroma_filter_very_low == NULL) {
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

uint32 img_ctx_ivd_set_hi_luma_combine(uint32 context_id, const ik_hi_luma_combine_t *p_hi_luma_combine)
{
    amba_ik_system_print_str_5("[IK] set_hi_luma_combine is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //TBD
    }
    if (p_hi_luma_combine==NULL) {
        //TBD
    }
    return IK_ERR_0004;
}

uint32 img_ctx_set_hi_luma_combine(uint32 context_id, const ik_hi_luma_combine_t *p_hi_luma_combine)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;
    const ik_debug_check_func_t* p_check_func = ik_ctx_get_debug_check_func();
    if(p_check_func != NULL) {
        rval = p_check_func->hi_luma_combine(p_hi_luma_combine, "hi_luma_combine");
    }
    if(rval == IK_OK) {
        rval |= img_ctx_get_context(context_id, &p_ctx);
        if((p_hi_luma_combine != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
            (void)amba_ik_system_lock_mutex(context_id, IK_WAIT_FOREVER);
            (void)amba_ik_system_memcpy(&p_ctx->filters.p_hiso_input_param->hi_luma_combine, p_hi_luma_combine, sizeof(ik_hi_luma_combine_t));
            ctx_raise_update_flag(&p_ctx->filters.update_flags.liso.hi_luma_combine_updated);
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

uint32 img_ctx_ivd_get_hi_luma_combine(uint32 context_id, ik_hi_luma_combine_t *p_hi_luma_combine)
{
    amba_ik_system_print_str_5("[IK] get_hi_luma_combine is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //TBD
    }
    if (p_hi_luma_combine == NULL) {
        //TBD
    } else {
        (void)amba_ik_system_memcpy(p_hi_luma_combine, p_hi_luma_combine, sizeof(uint8));
    }
    return IK_ERR_0004;
}

uint32 img_ctx_get_hi_luma_combine(uint32 context_id, ik_hi_luma_combine_t *p_hi_luma_combine)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval |= img_ctx_get_context(context_id, &p_ctx);
    if((p_hi_luma_combine != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        (void)amba_ik_system_lock_mutex(context_id, IK_WAIT_FOREVER);
        (void)amba_ik_system_memcpy(p_hi_luma_combine, &p_ctx->filters.p_hiso_input_param->hi_luma_combine, sizeof(ik_hi_luma_combine_t));
        (void)amba_ik_system_unlock_mutex(context_id);
    } else {
        if(p_hi_luma_combine == NULL) {
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

uint32 img_ctx_ivd_set_hi_low_asf_combine(uint32 context_id, const ik_hi_low_asf_combine_t *p_hi_low_asf_combine)
{
    amba_ik_system_print_str_5("[IK] set_hi_low_asf_combine is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //TBD
    }
    if (p_hi_low_asf_combine==NULL) {
        //TBD
    }
    return IK_ERR_0004;
}

uint32 img_ctx_set_hi_low_asf_combine(uint32 context_id, const ik_hi_low_asf_combine_t *p_hi_low_asf_combine)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;
    const ik_debug_check_func_t* p_check_func = ik_ctx_get_debug_check_func();
    if(p_check_func != NULL) {
        rval = p_check_func->hi_low_asf_combine(p_hi_low_asf_combine, "hi_low_asf_combine");
    }
    if(rval == IK_OK) {
        rval |= img_ctx_get_context(context_id, &p_ctx);
        if((p_hi_low_asf_combine != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
            (void)amba_ik_system_lock_mutex(context_id, IK_WAIT_FOREVER);
            (void)amba_ik_system_memcpy(&p_ctx->filters.p_hiso_input_param->hi_low_asf_combine, p_hi_low_asf_combine, sizeof(ik_hi_low_asf_combine_t));
            ctx_raise_update_flag(&p_ctx->filters.update_flags.liso.hi_low_asf_combine_updated);
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

uint32 img_ctx_ivd_get_hi_low_asf_combine(uint32 context_id, ik_hi_low_asf_combine_t *p_hi_low_asf_combine)
{
    amba_ik_system_print_str_5("[IK] get_hi_low_asf_combine is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //TBD
    }
    if (p_hi_low_asf_combine == NULL) {
        //TBD
    } else {
        (void)amba_ik_system_memcpy(p_hi_low_asf_combine, p_hi_low_asf_combine, sizeof(uint8));
    }
    return IK_ERR_0004;
}

uint32 img_ctx_get_hi_low_asf_combine(uint32 context_id, ik_hi_low_asf_combine_t *p_hi_low_asf_combine)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval |= img_ctx_get_context(context_id, &p_ctx);
    if((p_hi_low_asf_combine != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        (void)amba_ik_system_lock_mutex(context_id, IK_WAIT_FOREVER);
        (void)amba_ik_system_memcpy(p_hi_low_asf_combine, &p_ctx->filters.p_hiso_input_param->hi_low_asf_combine, sizeof(ik_hi_low_asf_combine_t));
        (void)amba_ik_system_unlock_mutex(context_id);
    } else {
        if(p_hi_low_asf_combine == NULL) {
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

uint32 img_ctx_ivd_set_hi_chroma_fltr_med_com(uint32 context_id, const ik_hi_chroma_fltr_med_com_t *p_hi_chroma_fltr_med_com)
{
    amba_ik_system_print_str_5("[IK] set_hi_chroma_fltr_med_com is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //TBD
    }
    if (p_hi_chroma_fltr_med_com==NULL) {
        //TBD
    }
    return IK_ERR_0004;
}

uint32 img_ctx_set_hi_chroma_fltr_med_com(uint32 context_id, const ik_hi_chroma_fltr_med_com_t *p_hi_chroma_fltr_med_com)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;
    const ik_debug_check_func_t* p_check_func = ik_ctx_get_debug_check_func();
    if(p_check_func != NULL) {
        rval = p_check_func->hi_chroma_fltr_med_com(p_hi_chroma_fltr_med_com, "hi_chroma_fltr_med_com");
    }
    if(rval == IK_OK) {
        rval |= img_ctx_get_context(context_id, &p_ctx);
        if((p_hi_chroma_fltr_med_com != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
            (void)amba_ik_system_lock_mutex(context_id, IK_WAIT_FOREVER);
            (void)amba_ik_system_memcpy(&p_ctx->filters.p_hiso_input_param->hi_chroma_fltr_med_com, p_hi_chroma_fltr_med_com, sizeof(ik_hi_chroma_fltr_med_com_t));
            ctx_raise_update_flag(&p_ctx->filters.update_flags.liso.hi_chroma_fltr_med_com_updated);
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

uint32 img_ctx_ivd_get_hi_chroma_fltr_med_com(uint32 context_id, ik_hi_chroma_fltr_med_com_t *p_hi_chroma_fltr_med_com)
{
    amba_ik_system_print_str_5("[IK] get_hi_chroma_fltr_med_com is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //TBD
    }
    if (p_hi_chroma_fltr_med_com == NULL) {
        //TBD
    } else {
        (void)amba_ik_system_memcpy(p_hi_chroma_fltr_med_com, p_hi_chroma_fltr_med_com, sizeof(uint8));
    }
    return IK_ERR_0004;
}

uint32 img_ctx_get_hi_chroma_fltr_med_com(uint32 context_id, ik_hi_chroma_fltr_med_com_t *p_hi_chroma_fltr_med_com)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval |= img_ctx_get_context(context_id, &p_ctx);
    if((p_hi_chroma_fltr_med_com != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        (void)amba_ik_system_lock_mutex(context_id, IK_WAIT_FOREVER);
        (void)amba_ik_system_memcpy(p_hi_chroma_fltr_med_com, &p_ctx->filters.p_hiso_input_param->hi_chroma_fltr_med_com, sizeof(ik_hi_chroma_fltr_med_com_t));
        (void)amba_ik_system_unlock_mutex(context_id);
    } else {
        if(p_hi_chroma_fltr_med_com == NULL) {
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

uint32 img_ctx_ivd_set_hi_chrm_fltr_low_com(uint32 context_id, const ik_hi_chroma_fltr_low_com_t *p_hi_chroma_fltr_low_com)
{
    amba_ik_system_print_str_5("[IK] set_hi_chroma_fltr_low_com is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //TBD
    }
    if (p_hi_chroma_fltr_low_com==NULL) {
        //TBD
    }
    return IK_ERR_0004;
}

uint32 img_ctx_set_hi_chroma_fltr_low_com(uint32 context_id, const ik_hi_chroma_fltr_low_com_t *p_hi_chroma_fltr_low_com)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;
    const ik_debug_check_func_t* p_check_func = ik_ctx_get_debug_check_func();
    if(p_check_func != NULL) {
        rval = p_check_func->hi_chroma_fltr_low_com(p_hi_chroma_fltr_low_com, "hi_chroma_fltr_low_com");
    }
    if(rval == IK_OK) {
        rval |= img_ctx_get_context(context_id, &p_ctx);
        if((p_hi_chroma_fltr_low_com != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
            (void)amba_ik_system_lock_mutex(context_id, IK_WAIT_FOREVER);
            (void)amba_ik_system_memcpy(&p_ctx->filters.p_hiso_input_param->hi_chroma_fltr_low_com, p_hi_chroma_fltr_low_com, sizeof(ik_hi_chroma_fltr_low_com_t));
            ctx_raise_update_flag(&p_ctx->filters.update_flags.liso.hi_chroma_fltr_low_com_updated);
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

uint32 img_ctx_ivd_get_hi_chrm_fltr_low_com(uint32 context_id, ik_hi_chroma_fltr_low_com_t *p_hi_chroma_fltr_low_com)
{
    amba_ik_system_print_str_5("[IK] get_hi_chroma_fltr_low_com is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //TBD
    }
    if (p_hi_chroma_fltr_low_com == NULL) {
        //TBD
    } else {
        (void)amba_ik_system_memcpy(p_hi_chroma_fltr_low_com, p_hi_chroma_fltr_low_com, sizeof(uint8));
    }
    return IK_ERR_0004;
}

uint32 img_ctx_get_hi_chroma_fltr_low_com(uint32 context_id, ik_hi_chroma_fltr_low_com_t *p_hi_chroma_fltr_low_com)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval |= img_ctx_get_context(context_id, &p_ctx);
    if((p_hi_chroma_fltr_low_com != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        (void)amba_ik_system_lock_mutex(context_id, IK_WAIT_FOREVER);
        (void)amba_ik_system_memcpy(p_hi_chroma_fltr_low_com, &p_ctx->filters.p_hiso_input_param->hi_chroma_fltr_low_com, sizeof(ik_hi_chroma_fltr_low_com_t));
        (void)amba_ik_system_unlock_mutex(context_id);
    } else {
        if(p_hi_chroma_fltr_low_com == NULL) {
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

uint32 img_ctx_ivd_set_hi_chm_fltr_very_low_com(uint32 context_id, const ik_hi_chroma_fltr_very_low_com_t *p_hi_chroma_fltr_very_low_com)
{
    amba_ik_system_print_str_5("[IK] set_hi_chroma_fltr_very_low_com is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //TBD
    }
    if (p_hi_chroma_fltr_very_low_com==NULL) {
        //TBD
    }
    return IK_ERR_0004;
}

uint32 img_ctx_set_hi_chroma_fltr_very_low_com(uint32 context_id, const ik_hi_chroma_fltr_very_low_com_t *p_hi_chroma_fltr_very_low_com)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;
    const ik_debug_check_func_t* p_check_func = ik_ctx_get_debug_check_func();
    if(p_check_func != NULL) {
        rval = p_check_func->hi_chroma_fltr_very_low_com(p_hi_chroma_fltr_very_low_com, "hi_chroma_fltr_very_low_com");
    }
    if(rval == IK_OK) {
        rval |= img_ctx_get_context(context_id, &p_ctx);
        if((p_hi_chroma_fltr_very_low_com != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
            (void)amba_ik_system_lock_mutex(context_id, IK_WAIT_FOREVER);
            (void)amba_ik_system_memcpy(&p_ctx->filters.p_hiso_input_param->hi_chroma_fltr_very_low_com, p_hi_chroma_fltr_very_low_com, sizeof(ik_hi_chroma_fltr_very_low_com_t));
            ctx_raise_update_flag(&p_ctx->filters.update_flags.liso.hi_chroma_fltr_very_low_com_updated);
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

uint32 img_ctx_ivd_get_hi_chm_fltr_very_low_com(uint32 context_id, ik_hi_chroma_fltr_very_low_com_t *p_hi_chroma_fltr_very_low_com)
{
    amba_ik_system_print_str_5("[IK] get_hi_chroma_fltr_very_low_com is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //TBD
    }
    if (p_hi_chroma_fltr_very_low_com == NULL) {
        //TBD
    } else {
        (void)amba_ik_system_memcpy(p_hi_chroma_fltr_very_low_com, p_hi_chroma_fltr_very_low_com, sizeof(uint8));
    }
    return IK_ERR_0004;
}

uint32 img_ctx_get_hi_chroma_fltr_very_low_com(uint32 context_id, ik_hi_chroma_fltr_very_low_com_t *p_hi_chroma_fltr_very_low_com)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval |= img_ctx_get_context(context_id, &p_ctx);
    if((p_hi_chroma_fltr_very_low_com != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        (void)amba_ik_system_lock_mutex(context_id, IK_WAIT_FOREVER);
        (void)amba_ik_system_memcpy(p_hi_chroma_fltr_very_low_com, &p_ctx->filters.p_hiso_input_param->hi_chroma_fltr_very_low_com, sizeof(ik_hi_chroma_fltr_very_low_com_t));
        (void)amba_ik_system_unlock_mutex(context_id);
    } else {
        if(p_hi_chroma_fltr_very_low_com == NULL) {
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

uint32 img_ctx_ivd_set_hili_combine(uint32 context_id, const ik_hili_combine_t *p_hili_combine)
{
    amba_ik_system_print_str_5("[IK] set_hili_combine is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //TBD
    }
    if (p_hili_combine==NULL) {
        //TBD
    }
    return IK_ERR_0004;
}

uint32 img_ctx_set_hili_combine(uint32 context_id, const ik_hili_combine_t *p_hili_combine)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;
    const ik_debug_check_func_t* p_check_func = ik_ctx_get_debug_check_func();
    if(p_check_func != NULL) {
        rval = p_check_func->hili_combine(p_hili_combine, "hili_combine");
    }
    if(rval == IK_OK) {
        rval |= img_ctx_get_context(context_id, &p_ctx);
        if((p_hili_combine != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
            (void)amba_ik_system_lock_mutex(context_id, IK_WAIT_FOREVER);
            (void)amba_ik_system_memcpy(&p_ctx->filters.p_hiso_input_param->hili_combine, p_hili_combine, sizeof(ik_hili_combine_t));
            ctx_raise_update_flag(&p_ctx->filters.update_flags.liso.hili_combine_updated);
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

uint32 img_ctx_ivd_get_hili_combine(uint32 context_id, ik_hili_combine_t *p_hili_combine)
{
    amba_ik_system_print_str_5("[IK] get_hili_combine is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //TBD
    }
    if (p_hili_combine == NULL) {
        //TBD
    } else {
        (void)amba_ik_system_memcpy(p_hili_combine, p_hili_combine, sizeof(uint8));
    }
    return IK_ERR_0004;
}

uint32 img_ctx_get_hili_combine(uint32 context_id, ik_hili_combine_t *p_hili_combine)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval |= img_ctx_get_context(context_id, &p_ctx);
    if((p_hili_combine != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        (void)amba_ik_system_lock_mutex(context_id, IK_WAIT_FOREVER);
        (void)amba_ik_system_memcpy(p_hili_combine, &p_ctx->filters.p_hiso_input_param->hili_combine, sizeof(ik_hili_combine_t));
        (void)amba_ik_system_unlock_mutex(context_id);
    } else {
        if(p_hili_combine == NULL) {
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

uint32 img_ctx_ivd_set_hi_mid_high_freq_recover(uint32 context_id, const ik_hi_mid_high_freq_recover_t *p_hi_mid_high_freq_recover)
{
    amba_ik_system_print_str_5("[IK] set_hi_mid_high_freq_recover is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //TBD
    }
    if (p_hi_mid_high_freq_recover==NULL) {
        //TBD
    }
    return IK_ERR_0004;
}

uint32 img_ctx_set_hi_mid_high_freq_recover(uint32 context_id, const ik_hi_mid_high_freq_recover_t *p_hi_mid_high_freq_recover)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;
    const ik_debug_check_func_t* p_check_func = ik_ctx_get_debug_check_func();
    if(p_check_func != NULL) {
        rval = p_check_func->hi_mid_high_freq_recover(p_hi_mid_high_freq_recover, "hi_mid_high_freq_recover");
    }
    if(rval == IK_OK) {
        rval |= img_ctx_get_context(context_id, &p_ctx);
        if((p_hi_mid_high_freq_recover != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
            (void)amba_ik_system_lock_mutex(context_id, IK_WAIT_FOREVER);
            (void)amba_ik_system_memcpy(&p_ctx->filters.p_hiso_input_param->hi_mid_high_freq_recover, p_hi_mid_high_freq_recover, sizeof(ik_hi_mid_high_freq_recover_t));
            ctx_raise_update_flag(&p_ctx->filters.update_flags.liso.hi_mid_high_freq_recover_updated);
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

uint32 img_ctx_ivd_get_hi_mid_high_freq_recover(uint32 context_id, ik_hi_mid_high_freq_recover_t *p_hi_mid_high_freq_recover)
{
    amba_ik_system_print_str_5("[IK] get_hi_mid_high_freq_recover is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //TBD
    }
    if (p_hi_mid_high_freq_recover == NULL) {
        //TBD
    } else {
        (void)amba_ik_system_memcpy(p_hi_mid_high_freq_recover, p_hi_mid_high_freq_recover, sizeof(uint8));
    }
    return IK_ERR_0004;
}

uint32 img_ctx_get_hi_mid_high_freq_recover(uint32 context_id, ik_hi_mid_high_freq_recover_t *p_hi_mid_high_freq_recover)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval |= img_ctx_get_context(context_id, &p_ctx);
    if((p_hi_mid_high_freq_recover != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        (void)amba_ik_system_lock_mutex(context_id, IK_WAIT_FOREVER);
        (void)amba_ik_system_memcpy(p_hi_mid_high_freq_recover, &p_ctx->filters.p_hiso_input_param->hi_mid_high_freq_recover, sizeof(ik_hi_mid_high_freq_recover_t));
        (void)amba_ik_system_unlock_mutex(context_id);
    } else {
        if(p_hi_mid_high_freq_recover == NULL) {
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

uint32 img_ctx_ivd_set_hi_luma_blend(uint32 context_id, const ik_hi_luma_blend_t *p_hi_luma_blend)
{
    amba_ik_system_print_str_5("[IK] set_hi_luma_blend is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //TBD
    }
    if (p_hi_luma_blend==NULL) {
        //TBD
    }
    return IK_ERR_0004;
}

uint32 img_ctx_set_hi_luma_blend(uint32 context_id, const ik_hi_luma_blend_t *p_hi_luma_blend)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;
    const ik_debug_check_func_t* p_check_func = ik_ctx_get_debug_check_func();
    if(p_check_func != NULL) {
        rval = p_check_func->hi_luma_blend(p_hi_luma_blend, "hi_luma_blend");
    }
    if(rval == IK_OK) {
        rval |= img_ctx_get_context(context_id, &p_ctx);
        if((p_hi_luma_blend != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
            (void)amba_ik_system_lock_mutex(context_id, IK_WAIT_FOREVER);
            (void)amba_ik_system_memcpy(&p_ctx->filters.p_hiso_input_param->hi_luma_blend, p_hi_luma_blend, sizeof(ik_hi_luma_blend_t));
            ctx_raise_update_flag(&p_ctx->filters.update_flags.liso.hi_luma_blend_updated);
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

uint32 img_ctx_ivd_get_hi_luma_blend(uint32 context_id, ik_hi_luma_blend_t *p_hi_luma_blend)
{
    amba_ik_system_print_str_5("[IK] get_hi_luma_blend is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //TBD
    }
    if (p_hi_luma_blend == NULL) {
        //TBD
    } else {
        (void)amba_ik_system_memcpy(p_hi_luma_blend, p_hi_luma_blend, sizeof(uint8));
    }
    return IK_ERR_0004;
}

uint32 img_ctx_get_hi_luma_blend(uint32 context_id, ik_hi_luma_blend_t *p_hi_luma_blend)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval |= img_ctx_get_context(context_id, &p_ctx);
    if((p_hi_luma_blend != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        (void)amba_ik_system_lock_mutex(context_id, IK_WAIT_FOREVER);
        (void)amba_ik_system_memcpy(p_hi_luma_blend, &p_ctx->filters.p_hiso_input_param->hi_luma_blend, sizeof(ik_hi_luma_blend_t));
        (void)amba_ik_system_unlock_mutex(context_id);
    } else {
        if(p_hi_luma_blend == NULL) {
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

uint32 img_ctx_ivd_set_hi_nonsmooth_detect(uint32 context_id, const ik_hi_nonsmooth_detect_t *p_hi_nonsmooth_detect)
{
    amba_ik_system_print_str_5("[IK] set_hi_nonsmooth_detect is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //TBD
    }
    if (p_hi_nonsmooth_detect==NULL) {
        //TBD
    }
    return IK_ERR_0004;
}

uint32 img_ctx_set_hi_nonsmooth_detect(uint32 context_id, const ik_hi_nonsmooth_detect_t *p_hi_nonsmooth_detect)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;
    const ik_debug_check_func_t* p_check_func = ik_ctx_get_debug_check_func();
    if(p_check_func != NULL) {
        rval = p_check_func->hi_nonsmooth_detect(p_hi_nonsmooth_detect, "hi_nonsmooth_detect");
    }
    if(rval == IK_OK) {
        rval |= img_ctx_get_context(context_id, &p_ctx);
        if((p_hi_nonsmooth_detect != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
            (void)amba_ik_system_lock_mutex(context_id, IK_WAIT_FOREVER);
            (void)amba_ik_system_memcpy(&p_ctx->filters.p_hiso_input_param->hi_nonsmooth_detect, p_hi_nonsmooth_detect, sizeof(ik_hi_nonsmooth_detect_t));
            ctx_raise_update_flag(&p_ctx->filters.update_flags.liso.hi_nonsmooth_detect_updated);
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

uint32 img_ctx_ivd_get_hi_nonsmooth_detect(uint32 context_id, ik_hi_nonsmooth_detect_t *p_hi_nonsmooth_detect)
{
    amba_ik_system_print_str_5("[IK] get_hi_nonsmooth_detect is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //TBD
    }
    if (p_hi_nonsmooth_detect == NULL) {
        //TBD
    } else {
        (void)amba_ik_system_memcpy(p_hi_nonsmooth_detect, p_hi_nonsmooth_detect, sizeof(uint8));
    }
    return IK_ERR_0004;
}

uint32 img_ctx_get_hi_nonsmooth_detect(uint32 context_id, ik_hi_nonsmooth_detect_t *p_hi_nonsmooth_detect)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval |= img_ctx_get_context(context_id, &p_ctx);
    if((p_hi_nonsmooth_detect != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        (void)amba_ik_system_lock_mutex(context_id, IK_WAIT_FOREVER);
        (void)amba_ik_system_memcpy(p_hi_nonsmooth_detect, &p_ctx->filters.p_hiso_input_param->hi_nonsmooth_detect, sizeof(ik_hi_nonsmooth_detect_t));
        (void)amba_ik_system_unlock_mutex(context_id);
    } else {
        if(p_hi_nonsmooth_detect == NULL) {
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

uint32 img_ctx_ivd_set_hi_select(uint32 context_id, const ik_hi_select_t *p_hi_select)
{
    amba_ik_system_print_str_5("[IK] set_hi_select is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //TBD
    }
    if (p_hi_select==NULL) {
        //TBD
    }
    return IK_ERR_0004;
}

uint32 img_ctx_set_hi_select(uint32 context_id, const ik_hi_select_t *p_hi_select)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;
    const ik_debug_check_func_t* p_check_func = ik_ctx_get_debug_check_func();
    if(p_check_func != NULL) {
        rval = p_check_func->hi_select(p_hi_select, "hi_select");
    }
    if(rval == IK_OK) {
        rval |= img_ctx_get_context(context_id, &p_ctx);
        if((p_hi_select != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
            (void)amba_ik_system_lock_mutex(context_id, IK_WAIT_FOREVER);
            (void)amba_ik_system_memcpy(&p_ctx->filters.p_hiso_input_param->hi_select, p_hi_select, sizeof(ik_hi_select_t));
            ctx_raise_update_flag(&p_ctx->filters.update_flags.liso.hi_select_updated);
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

uint32 img_ctx_ivd_get_hi_select(uint32 context_id, ik_hi_select_t *p_hi_select)
{
    amba_ik_system_print_str_5("[IK] get_hi_select is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //TBD
    }
    if (p_hi_select == NULL) {
        //TBD
    } else {
        (void)amba_ik_system_memcpy(p_hi_select, p_hi_select, sizeof(uint8));
    }
    return IK_ERR_0004;
}

uint32 img_ctx_get_hi_select(uint32 context_id, ik_hi_select_t *p_hi_select)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval |= img_ctx_get_context(context_id, &p_ctx);
    if((p_hi_select != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        (void)amba_ik_system_lock_mutex(context_id, IK_WAIT_FOREVER);
        (void)amba_ik_system_memcpy(p_hi_select, &p_ctx->filters.p_hiso_input_param->hi_select, sizeof(ik_hi_select_t));
        (void)amba_ik_system_unlock_mutex(context_id);
    } else {
        if(p_hi_select == NULL) {
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

