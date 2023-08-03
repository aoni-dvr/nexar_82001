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
//#include "AmbaDSP_ImgArchSystemAPI.h"
#include "AmbaDSP_ImgContextComponentIF.h"
#include "AmbaDSP_ImgContextDebugUtility.h"
#include "AmbaDSP_ImgContextUtility.h"

uint32 img_ctx_ivd_set_exp1_fe_static_blc(uint32 context_id, const ik_static_blc_level_t *p_exp1_frontend_static_blc)
{
    amba_ik_system_print_str_5("[IK] set_exp1_fe_static_blc is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id == 0U) {
        //TBD
    }
    if (p_exp1_frontend_static_blc == NULL) {
        //TBD
    }
    return IK_ERR_0004;
}

uint32 img_ctx_set_exp1_fe_static_blc(uint32 context_id, const ik_static_blc_level_t *p_exp1_frontend_static_blc)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;
    const ik_debug_check_func_t* p_check_func = ik_ctx_get_debug_check_func();
    if(p_check_func != NULL) {
        rval = p_check_func->fe_blc(p_exp1_frontend_static_blc, 1U);
    }

    if(rval == IK_OK) {
        rval |= img_ctx_get_context(context_id, &p_ctx);
        if ((rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
            (void)amba_ik_system_lock_mutex(&p_ctx->organization.mutex, IK_WAIT_FOREVER);
            (void)amba_ik_system_memcpy(&p_ctx->filters.input_param.exp1_frontend_static_blc, p_exp1_frontend_static_blc, sizeof(ik_static_blc_level_t));
            ctx_raise_update_flag(&p_ctx->filters.update_flags.iso.exp1_fe_static_blc_updated);
            (void)amba_ik_system_unlock_mutex(&p_ctx->organization.mutex);
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

uint32 img_ctx_ivd_set_exp2_fe_static_blc(uint32 context_id, const ik_static_blc_level_t *p_exp2_frontend_static_blc)
{
    amba_ik_system_print_str_5("[IK] set_exp2_fe_static_blc is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id == 0U) {
        //TBD
    }
    if (p_exp2_frontend_static_blc == NULL) {
        //TBD
    }
    return IK_ERR_0004;
}

uint32 img_ctx_set_exp2_fe_static_blc(uint32 context_id, const ik_static_blc_level_t *p_exp2_frontend_static_blc)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;
    const ik_debug_check_func_t* p_check_func = ik_ctx_get_debug_check_func();
    if(p_check_func != NULL) {
        rval = p_check_func->fe_blc(p_exp2_frontend_static_blc, 2U);
    }

    if(rval == IK_OK) {
        rval |= img_ctx_get_context(context_id, &p_ctx);
        if ((rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
            (void)amba_ik_system_lock_mutex(&p_ctx->organization.mutex, IK_WAIT_FOREVER);
            (void)amba_ik_system_memcpy(&p_ctx->filters.input_param.exp2_frontend_static_blc, p_exp2_frontend_static_blc, sizeof(ik_static_blc_level_t));
            ctx_raise_update_flag(&p_ctx->filters.update_flags.iso.exp2_fe_static_blc_updated);
            (void)amba_ik_system_unlock_mutex(&p_ctx->organization.mutex);
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

uint32 img_ctx_ivd_set_exp1_fe_wb_gain(uint32 context_id, const ik_frontend_wb_gain_t *p_exp1_frontend_wb_gain)
{
    amba_ik_system_print_str_5("[IK] set_exp1_fe_wb_gain is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id == 0U) {
        //TBD
    }
    if (p_exp1_frontend_wb_gain == NULL) {
        //TBD
    }
    return IK_ERR_0004;
}

uint32 img_ctx_set_exp1_fe_wb_gain(uint32 context_id, const ik_frontend_wb_gain_t *p_exp1_frontend_wb_gain)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;
    const ik_debug_check_func_t* p_check_func = ik_ctx_get_debug_check_func();
    if(p_check_func != NULL) {
        rval = p_check_func->fe_wb(p_exp1_frontend_wb_gain, 1U);
    }

    if(rval == IK_OK) {
        rval |= img_ctx_get_context(context_id, &p_ctx);
        if ((rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
            (void)amba_ik_system_lock_mutex(&p_ctx->organization.mutex, IK_WAIT_FOREVER);
            (void)amba_ik_system_memcpy(&p_ctx->filters.input_param.exp1_frontend_wb_gain, p_exp1_frontend_wb_gain, sizeof(ik_frontend_wb_gain_t));
            ctx_raise_update_flag(&p_ctx->filters.update_flags.iso.exp1_fe_wb_gain_updated);
            (void)amba_ik_system_unlock_mutex(&p_ctx->organization.mutex);
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

uint32 img_ctx_ivd_set_exp2_fe_wb_gain(uint32 context_id, const ik_frontend_wb_gain_t *p_exp2_frontend_wb_gain)
{
    amba_ik_system_print_str_5("[IK] set_exp2_fe_wb_gain is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id == 0U) {
        //TBD
    }
    if (p_exp2_frontend_wb_gain == NULL) {
        //TBD
    }
    return IK_ERR_0004;
}

uint32 img_ctx_set_exp2_fe_wb_gain(uint32 context_id, const ik_frontend_wb_gain_t *p_exp2_frontend_wb_gain)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;
    const ik_debug_check_func_t* p_check_func = ik_ctx_get_debug_check_func();
    if(p_check_func != NULL) {
        rval = p_check_func->fe_wb(p_exp2_frontend_wb_gain, 2U);
    }
    if(rval == IK_OK) {
        rval |= img_ctx_get_context(context_id, &p_ctx);
        if ((rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
            (void)amba_ik_system_lock_mutex(&p_ctx->organization.mutex, IK_WAIT_FOREVER);
            (void)amba_ik_system_memcpy(&p_ctx->filters.input_param.exp2_frontend_wb_gain, p_exp2_frontend_wb_gain, sizeof(ik_frontend_wb_gain_t));
            ctx_raise_update_flag(&p_ctx->filters.update_flags.iso.exp2_fe_wb_gain_updated);
            (void)amba_ik_system_unlock_mutex(&p_ctx->organization.mutex);
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

uint32 img_ctx_ivd_set_hdr_raw_offset(uint32 context_id, const ik_hdr_raw_info_t *p_raw_info)
{
    amba_ik_system_print_str_5("[IK] set_hdr_raw_offset is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id == 0U) {
        //TBD
    }
    if (p_raw_info == NULL) {
        //TBD
    }
    return IK_ERR_0004;
}

uint32 img_ctx_set_hdr_raw_offset(uint32 context_id, const ik_hdr_raw_info_t *p_raw_info)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        (void)amba_ik_system_lock_mutex(&p_ctx->organization.mutex, IK_WAIT_FOREVER);
        (void)amba_ik_system_memcpy(&p_ctx->filters.input_param.hdr_raw_info, p_raw_info, sizeof(ik_hdr_raw_info_t));
        ctx_raise_update_flag(&p_ctx->filters.update_flags.iso.hdr_raw_info_updated);
        (void)amba_ik_system_unlock_mutex(&p_ctx->organization.mutex);
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

uint32 img_ctx_ivd_set_ce(uint32 context_id, const ik_ce_t *p_ce)
{
    amba_ik_system_print_str_5("[IK] set_ce is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id == 0U) {
        //TBD
    }
    if (p_ce == NULL) {
        //TBD
    }
    return IK_ERR_0004;
}

uint32 img_ctx_set_ce(uint32 context_id, const ik_ce_t *p_ce)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;
    const ik_debug_check_func_t* p_check_func = ik_ctx_get_debug_check_func();
    if(p_check_func != NULL) {
        rval = p_check_func->ce(p_ce);
    }
    if(rval == IK_OK) {
        rval |= img_ctx_get_context(context_id, &p_ctx);
        if ((rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
            (void)amba_ik_system_lock_mutex(&p_ctx->organization.mutex, IK_WAIT_FOREVER);
            (void)amba_ik_system_memcpy(&p_ctx->filters.input_param.ce, p_ce, sizeof(ik_ce_t));
            ctx_raise_update_flag(&p_ctx->filters.update_flags.iso.ce_updated);
            (void)amba_ik_system_unlock_mutex(&p_ctx->organization.mutex);
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

uint32 img_ctx_ivd_set_ce_input_table(uint32 context_id, const ik_ce_input_table_t *p_ce_input_table)
{
    amba_ik_system_print_str_5("[IK] set_ce_input_table is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id == 0U) {
        //TBD
    }
    if (p_ce_input_table == NULL) {
        //TBD
    }
    return IK_ERR_0004;
}

uint32 img_ctx_set_ce_input_table(uint32 context_id, const ik_ce_input_table_t *p_ce_input_table)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;
    const ik_debug_check_func_t* p_check_func = ik_ctx_get_debug_check_func();
    if(p_check_func != NULL) {
        rval = p_check_func->ce_input_tbl(p_ce_input_table);
    }
    if(rval == IK_OK) {
        rval |= img_ctx_get_context(context_id, &p_ctx);
        if ((rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
            (void)amba_ik_system_lock_mutex(&p_ctx->organization.mutex, IK_WAIT_FOREVER);
            (void)amba_ik_system_memcpy(&p_ctx->filters.input_param.ce_input_table, p_ce_input_table, sizeof(ik_ce_input_table_t));
            ctx_raise_update_flag(&p_ctx->filters.update_flags.iso.ce_input_table_updated);
            (void)amba_ik_system_unlock_mutex(&p_ctx->organization.mutex);
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

uint32 img_ctx_ivd_set_ce_out_table(uint32 context_id, const ik_ce_output_table_t *p_ce_out_table)
{
    amba_ik_system_print_str_5("[IK] set_ce_out_table is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id == 0U) {
        //TBD
    }
    if (p_ce_out_table == NULL) {
        //TBD
    }
    return IK_ERR_0004;
}

uint32 img_ctx_set_ce_out_table(uint32 context_id, const ik_ce_output_table_t *p_ce_out_table)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;
    const ik_debug_check_func_t* p_check_func = ik_ctx_get_debug_check_func();
    if(p_check_func != NULL) {
        rval = p_check_func->ce_output_tbl(p_ce_out_table);
    }
    if(rval == IK_OK) {
        rval |= img_ctx_get_context(context_id, &p_ctx);
        if ((rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
            (void)amba_ik_system_lock_mutex(&p_ctx->organization.mutex, IK_WAIT_FOREVER);
            (void)amba_ik_system_memcpy(&p_ctx->filters.input_param.ce_out_table, p_ce_out_table, sizeof(ik_ce_output_table_t));
            ctx_raise_update_flag(&p_ctx->filters.update_flags.iso.ce_out_table_updated);
            (void)amba_ik_system_unlock_mutex(&p_ctx->organization.mutex);
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

uint32 img_ctx_ivd_set_hdr_exp_num(const uint32 context_id, const uint32 exp_num)
{
    amba_ik_system_print_str_5("[IK] set_hdr_exp_num is invalid!", NULL, NULL, NULL, NULL, NULL);
    if ((context_id != 0U) || (exp_num != 0xFFFFFFFFU)) {
        //MISRA
    }
    return IK_ERR_0004;
}

uint32 img_ctx_set_hdr_exp_num(const uint32 context_id, const uint32 exp_num)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;
    if(exp_num > 3U) {
        amba_ik_system_print_str_5("[IK][ERROR] set_hdr_exp_num() exp_num > 3 !", DC_S, DC_S, DC_S, DC_S, DC_S);
        rval |= IK_ERR_0008;
    }
    if(rval == IK_OK) {
        rval |= img_ctx_get_context(context_id, &p_ctx);
        if((rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
            (void)amba_ik_system_lock_mutex(&p_ctx->organization.mutex, IK_WAIT_FOREVER);
            (void)amba_ik_system_memcpy(&p_ctx->filters.input_param.num_of_exposures, &exp_num, sizeof(uint32));
            //ctx_raise_update_flag(&p_ctx->filters.update_flags.iso.hdr_exp_num_updated);
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

uint32 img_ctx_ivd_set_hdr_blend(uint32 context_id, const ik_hdr_blend_t *p_hdr_blend)
{
    amba_ik_system_print_str_5("[IK] set_hdr_blend is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id == 0U) {
        //TBD
    }
    if (p_hdr_blend == NULL) {
        //TBD
    }
    return IK_ERR_0004;
}

uint32 img_ctx_set_hdr_blend(uint32 context_id, const ik_hdr_blend_t *p_hdr_blend)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;
    const ik_debug_check_func_t* p_check_func = ik_ctx_get_debug_check_func();
    if(p_check_func != NULL) {
        rval = p_check_func->hdr_blend(p_hdr_blend);
    }
    if(rval == IK_OK) {
        rval |= img_ctx_get_context(context_id, &p_ctx);
        if ((rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
            (void)amba_ik_system_lock_mutex(&p_ctx->organization.mutex, IK_WAIT_FOREVER);
            (void)amba_ik_system_memcpy(&p_ctx->filters.input_param.hdr_blend, p_hdr_blend, sizeof(ik_hdr_blend_t));
            ctx_raise_update_flag(&p_ctx->filters.update_flags.iso.hdr_blend_updated);
            (void)amba_ik_system_unlock_mutex(&p_ctx->organization.mutex);
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

uint32 img_ctx_ivd_set_exp0_fe_static_blc(uint32 context_id, const ik_static_blc_level_t *p_exp0_frontend_static_blc)
{
    amba_ik_system_print_str_5("[IK] set_exp0_fe_static_blc is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id == 0U) {
        //TBD
    }
    if (p_exp0_frontend_static_blc == NULL) {
        //TBD
    }
    return IK_ERR_0004;
}

uint32 img_ctx_set_exp0_fe_static_blc(uint32 context_id, const ik_static_blc_level_t *p_exp0_frontend_static_blc)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;
    const ik_debug_check_func_t* p_check_func = ik_ctx_get_debug_check_func();
    if(p_check_func != NULL) {
        rval = p_check_func->fe_blc(p_exp0_frontend_static_blc, 0U);
    }

    if(rval == IK_OK) {
        rval |= img_ctx_get_context(context_id, &p_ctx);
        if ((rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
            (void)amba_ik_system_lock_mutex(&p_ctx->organization.mutex, IK_WAIT_FOREVER);
            (void)amba_ik_system_memcpy(&p_ctx->filters.input_param.exp0_frontend_static_blc, p_exp0_frontend_static_blc, sizeof(ik_static_blc_level_t));
            ctx_raise_update_flag(&p_ctx->filters.update_flags.iso.exp0_fe_static_blc_updated);
            (void)amba_ik_system_unlock_mutex(&p_ctx->organization.mutex);
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

uint32 img_ctx_ivd_set_exp0_fe_wb_gain(uint32 context_id, const ik_frontend_wb_gain_t *p_exp0_frontend_wb_gain)
{
    amba_ik_system_print_str_5("[IK] set_exp0_fe_wb_gain is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id == 0U) {
        //TBD
    }
    if (p_exp0_frontend_wb_gain == NULL) {
        //TBD
    }
    return IK_ERR_0004;
}

uint32 img_ctx_set_exp0_fe_wb_gain(uint32 context_id, const ik_frontend_wb_gain_t *p_exp0_frontend_wb_gain)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;
    const ik_debug_check_func_t* p_check_func = ik_ctx_get_debug_check_func();
    if(p_check_func != NULL) {
        rval = p_check_func->fe_wb(p_exp0_frontend_wb_gain, 0U);
    }

    if(rval == IK_OK) {
        rval |= img_ctx_get_context(context_id, &p_ctx);
        if ((rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
            (void)amba_ik_system_lock_mutex(&p_ctx->organization.mutex, IK_WAIT_FOREVER);
            (void)amba_ik_system_memcpy(&p_ctx->filters.input_param.exp0_frontend_wb_gain, p_exp0_frontend_wb_gain, sizeof(ik_frontend_wb_gain_t));
            ctx_raise_update_flag(&p_ctx->filters.update_flags.iso.exp0_fe_wb_gain_updated);
            (void)amba_ik_system_unlock_mutex(&p_ctx->organization.mutex);
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

uint32 img_ctx_ivd_get_exp1_fe_static_blc(uint32 context_id, ik_static_blc_level_t *p_exp1_frontend_static_blc)
{
    amba_ik_system_print_str_5("[IK] get_exp1_fe_static_blc is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id == 0U) {
        //TBD
    }
    if (p_exp1_frontend_static_blc != NULL) {
        //TBD
        int32 tmp = p_exp1_frontend_static_blc->black_r;
        p_exp1_frontend_static_blc->black_r = tmp;
    }
    return IK_ERR_0004;
}

uint32 img_ctx_get_exp1_fe_static_blc(uint32 context_id, ik_static_blc_level_t *p_exp1_frontend_static_blc)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        (void)amba_ik_system_lock_mutex(&p_ctx->organization.mutex, IK_WAIT_FOREVER);
        (void)amba_ik_system_memcpy(p_exp1_frontend_static_blc, &p_ctx->filters.input_param.exp1_frontend_static_blc, sizeof(ik_static_blc_level_t));
        (void)amba_ik_system_unlock_mutex(&p_ctx->organization.mutex);
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

uint32 img_ctx_ivd_get_exp2_fe_static_blc(uint32 context_id, ik_static_blc_level_t *p_exp2_frontend_static_blc)
{
    amba_ik_system_print_str_5("[IK] get_exp2_fe_static_blc is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id == 0U) {
        //TBD
    }
    if (p_exp2_frontend_static_blc != NULL) {
        //TBD
        int32 tmp = p_exp2_frontend_static_blc->black_b;
        p_exp2_frontend_static_blc->black_b = tmp;
    }
    return IK_ERR_0004;
}

uint32 img_ctx_get_exp2_fe_static_blc(uint32 context_id, ik_static_blc_level_t *p_exp2_frontend_static_blc)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        (void)amba_ik_system_lock_mutex(&p_ctx->organization.mutex, IK_WAIT_FOREVER);
        (void)amba_ik_system_memcpy(p_exp2_frontend_static_blc, &p_ctx->filters.input_param.exp2_frontend_static_blc, sizeof(ik_static_blc_level_t));
        (void)amba_ik_system_unlock_mutex(&p_ctx->organization.mutex);
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

uint32 img_ctx_ivd_get_exp1_fe_wb_gain(uint32 context_id, ik_frontend_wb_gain_t *p_exp1_frontend_wb_gain)
{
    amba_ik_system_print_str_5("[IK] get_exp1_fe_wb_gain is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id == 0U) {
        //TBD
    }
    if (p_exp1_frontend_wb_gain != NULL) {
        uint32 tmp = p_exp1_frontend_wb_gain->b_gain;
        p_exp1_frontend_wb_gain->b_gain = tmp;
    }
    return IK_ERR_0004;
}

uint32 img_ctx_get_exp1_fe_wb_gain(uint32 context_id, ik_frontend_wb_gain_t *p_exp1_frontend_wb_gain)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        (void)amba_ik_system_lock_mutex(&p_ctx->organization.mutex, IK_WAIT_FOREVER);
        (void)amba_ik_system_memcpy(p_exp1_frontend_wb_gain, &p_ctx->filters.input_param.exp1_frontend_wb_gain, sizeof(ik_frontend_wb_gain_t));
        (void)amba_ik_system_unlock_mutex(&p_ctx->organization.mutex);
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

uint32 img_ctx_ivd_get_exp2_fe_wb_gain(uint32 context_id, ik_frontend_wb_gain_t *p_exp2_frontend_wb_gain)
{
    amba_ik_system_print_str_5("[IK] get_exp2_fe_wb_gain is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id == 0U) {
        //TBD
    }
    if (p_exp2_frontend_wb_gain != NULL) {
        uint32 tmp = p_exp2_frontend_wb_gain->b_gain;
        p_exp2_frontend_wb_gain->b_gain = tmp;
    }
    return IK_ERR_0004;
}

uint32 img_ctx_get_exp2_fe_wb_gain(uint32 context_id, ik_frontend_wb_gain_t *p_exp2_frontend_wb_gain)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        (void)amba_ik_system_lock_mutex(&p_ctx->organization.mutex, IK_WAIT_FOREVER);
        (void)amba_ik_system_memcpy(p_exp2_frontend_wb_gain, &p_ctx->filters.input_param.exp2_frontend_wb_gain, sizeof(ik_frontend_wb_gain_t));
        (void)amba_ik_system_unlock_mutex(&p_ctx->organization.mutex);
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

uint32 img_ctx_ivd_get_hdr_raw_offset(uint32 context_id, ik_hdr_raw_info_t *p_raw_info)
{
    amba_ik_system_print_str_5("[IK] get_hdr_raw_offset is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id == 0U) {
        //TBD
    }
    if (p_raw_info != NULL) {
        uint32 tmp = p_raw_info->x_offset[0];
        p_raw_info->x_offset[0] = tmp;
    }
    return IK_ERR_0004;
}

uint32 img_ctx_get_hdr_raw_offset(uint32 context_id, ik_hdr_raw_info_t *p_raw_info)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        (void)amba_ik_system_lock_mutex(&p_ctx->organization.mutex, IK_WAIT_FOREVER);
        (void)amba_ik_system_memcpy(p_raw_info, &p_ctx->filters.input_param.hdr_raw_info, sizeof(ik_hdr_raw_info_t));
        (void)amba_ik_system_unlock_mutex(&p_ctx->organization.mutex);
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

uint32 img_ctx_ivd_get_ce(uint32 context_id, ik_ce_t *p_ce)
{
    amba_ik_system_print_str_5("[IK] get_ce is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id == 0U) {
        //TBD
    }
    if (p_ce != NULL) {
        uint32 tmp = p_ce->enable;
        p_ce->enable = tmp;
    }
    return IK_ERR_0004;
}

uint32 img_ctx_get_ce(uint32 context_id, ik_ce_t *p_ce)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        (void)amba_ik_system_lock_mutex(&p_ctx->organization.mutex, IK_WAIT_FOREVER);
        (void)amba_ik_system_memcpy(p_ce, &p_ctx->filters.input_param.ce, sizeof(ik_ce_t));
        (void)amba_ik_system_unlock_mutex(&p_ctx->organization.mutex);
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

uint32 img_ctx_ivd_get_ce_input_table(uint32 context_id, ik_ce_input_table_t *p_ce_input_table)
{
    amba_ik_system_print_str_5("[IK] get_ce_input_table is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id == 0U) {
        //TBD
    }
    if (p_ce_input_table != NULL) {
        uint32 tmp = p_ce_input_table->input_enable;
        p_ce_input_table->input_enable = tmp;
    }
    return IK_ERR_0004;
}

uint32 img_ctx_get_ce_input_table(uint32 context_id, ik_ce_input_table_t *p_ce_input_table)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        (void)amba_ik_system_lock_mutex(&p_ctx->organization.mutex, IK_WAIT_FOREVER);
        (void)amba_ik_system_memcpy(p_ce_input_table, &p_ctx->filters.input_param.ce_input_table, sizeof(ik_ce_input_table_t));
        (void)amba_ik_system_unlock_mutex(&p_ctx->organization.mutex);
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

uint32 img_ctx_ivd_get_ce_out_table(uint32 context_id, ik_ce_output_table_t *p_ce_out_table)
{
    amba_ik_system_print_str_5("[IK] get_ce_out_table is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id == 0U) {
        //TBD
    }
    if (p_ce_out_table != NULL) {
        uint32 tmp = p_ce_out_table->output_enable;
        p_ce_out_table->output_enable = tmp;
    }
    return IK_ERR_0004;
}

uint32 img_ctx_get_ce_out_table(uint32 context_id, ik_ce_output_table_t *p_ce_out_table)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        (void)amba_ik_system_lock_mutex(&p_ctx->organization.mutex, IK_WAIT_FOREVER);
        (void)amba_ik_system_memcpy(p_ce_out_table, &p_ctx->filters.input_param.ce_out_table, sizeof(ik_ce_output_table_t));
        (void)amba_ik_system_unlock_mutex(&p_ctx->organization.mutex);
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

uint32 img_ctx_ivd_get_hdr_exp_num(const uint32 context_id, uint32 *p_hdr_exp_num)
{
    amba_ik_system_print_str_5("[IK] get_hdr_exp_num() is invalid!", DC_S, DC_S, DC_S, DC_S, DC_S);
    if (context_id == 0U) {
        //TBD
    }
    if (p_hdr_exp_num != NULL) {
        //TBD
        *p_hdr_exp_num = 0U;
    }
    return IK_ERR_0004;
}

uint32 img_ctx_get_hdr_exp_num(const uint32 context_id, uint32 *p_hdr_exp_num)
{
    amba_ik_context_entity_t *p_ctx = NULL;
    uint32 rval;

    if (p_hdr_exp_num == NULL) {
        rval = IK_ERR_0005;
    } else {
        rval = img_ctx_get_context(context_id, &p_ctx);
        if (rval == IK_OK) {
            (void)amba_ik_system_lock_mutex(&p_ctx->organization.mutex, IK_WAIT_FOREVER);
            (void)amba_ik_system_memcpy(p_hdr_exp_num, &p_ctx->filters.input_param.num_of_exposures, sizeof(uint32));
            (void)amba_ik_system_unlock_mutex(&p_ctx->organization.mutex);
        }
    }
    return rval;
}

uint32 img_ctx_ivd_get_hdr_blend(uint32 context_id, ik_hdr_blend_t *p_hdr_blend)
{
    amba_ik_system_print_str_5("[IK] get_hdr_blend is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id == 0U) {
        //TBD
    }
    if (p_hdr_blend != NULL) {
        uint32 tmp = p_hdr_blend->enable;
        p_hdr_blend->enable = tmp;
    }
    return IK_ERR_0004;
}

uint32 img_ctx_get_hdr_blend(uint32 context_id, ik_hdr_blend_t *p_hdr_blend)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        (void)amba_ik_system_lock_mutex(&p_ctx->organization.mutex, IK_WAIT_FOREVER);
        (void)amba_ik_system_memcpy(p_hdr_blend, &p_ctx->filters.input_param.hdr_blend, sizeof(ik_hdr_blend_t));
        (void)amba_ik_system_unlock_mutex(&p_ctx->organization.mutex);
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

uint32 img_ctx_ivd_get_exp0_fe_static_blc(uint32 context_id, ik_static_blc_level_t *p_exp0_frontend_static_blc)
{
    amba_ik_system_print_str_5("[IK] get_exp0_fe_static_blc is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id == 0U) {
        //TBD
    }
    if (p_exp0_frontend_static_blc != NULL) {
        int32 tmp = p_exp0_frontend_static_blc->black_b;
        p_exp0_frontend_static_blc->black_b = tmp;
    }
    return IK_ERR_0004;
}

uint32 img_ctx_get_exp0_fe_static_blc(uint32 context_id, ik_static_blc_level_t *p_exp0_frontend_static_blc)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        (void)amba_ik_system_lock_mutex(&p_ctx->organization.mutex, IK_WAIT_FOREVER);
        (void)amba_ik_system_memcpy(p_exp0_frontend_static_blc, &p_ctx->filters.input_param.exp0_frontend_static_blc, sizeof(ik_static_blc_level_t));
        (void)amba_ik_system_unlock_mutex(&p_ctx->organization.mutex);
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

uint32 img_ctx_ivd_get_exp0_fe_wb_gain(uint32 context_id, ik_frontend_wb_gain_t *p_exp0_frontend_wb_gain)
{
    amba_ik_system_print_str_5("[IK] get_exp0_fe_wb_gain( is invalid!", NULL, NULL, NULL, NULL, NULL);
    if (context_id == 0U) {
        //TBD
    }
    if (p_exp0_frontend_wb_gain != NULL) {
        uint32 tmp = p_exp0_frontend_wb_gain->b_gain;
        p_exp0_frontend_wb_gain->b_gain = tmp;
    }
    return IK_ERR_0004;
}

uint32 img_ctx_get_exp0_fe_wb_gain(uint32 context_id, ik_frontend_wb_gain_t *p_exp0_frontend_wb_gain)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        (void)amba_ik_system_lock_mutex(&p_ctx->organization.mutex, IK_WAIT_FOREVER);
        (void)amba_ik_system_memcpy(p_exp0_frontend_wb_gain, &p_ctx->filters.input_param.exp0_frontend_wb_gain, sizeof(ik_frontend_wb_gain_t));
        (void)amba_ik_system_unlock_mutex(&p_ctx->organization.mutex);
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

