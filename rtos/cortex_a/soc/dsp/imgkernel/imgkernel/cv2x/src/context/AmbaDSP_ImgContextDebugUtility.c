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

#include "ik_data_type.h"
#include "AmbaDSP_ImgContextDebugUtility.h"
#include "AmbaDSP_ImgArchSpec.h"
#include "AmbaDSP_ImgArchSystemAPI.h"
#include "AmbaDSP_ImgContextComponentIF.h"


void img_ctx_set_diag_case_id(uint32 context_id, uint8 diag_case_id)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval |= img_ctx_get_context(context_id, &p_ctx);
    if ((rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        p_ctx->filters.input_param.diag_case_id = diag_case_id;
    } else {
        if(p_ctx->organization.initial_flag == 0U) {
            amba_ik_system_print_uint32_5("[IK] context_id = %d is not initial", context_id, DC_U, DC_U, DC_U, DC_U);
        } else {
            amba_ik_system_print_str_5("[IK] can't get context", NULL, NULL, NULL, NULL, NULL);
        }
    }
}
#if 0
uint8 img_ctx_get_diag_case_id(uint32 context_id)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;
    uint8 diag_case_id = 0;

    rval |= img_ctx_get_context(context_id, &p_ctx);
    if ((rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        diag_case_id = (context_id < (uint32)MAX_CONTEXT_NUM) ? p_ctx->filters.input_param.diag_case_id : 0U;
    } else {
        if(p_ctx->organization.initial_flag == 0U) {
            amba_ik_system_print_uint32_5("[IK] context_id = %d is not initial", context_id, DC_U, DC_U, DC_U, DC_U);
        } else {
            amba_ik_system_print_str_5("[IK] can't get context", NULL, NULL, NULL, NULL, NULL);
        }
    }

    return diag_case_id;
}
#endif
