/*
 * Copyright (C) 2018 Ambarella Inc.
 * All rights reserved.
 *
 * Author: Bo-xi Chen <bxchen@ambarella.com>
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */
#include <tee_ta_api.h>
#include "trace.h"

#include "data_at_rest_ta_type.h"
#include "data_at_rest_ta_handle.h"

TEE_Result TA_CreateEntryPoint(void)
{
    DLOG("data_at_rest: TA_CreateEntryPoint\n");
    return TEE_SUCCESS;
}

TEE_Result TA_OpenSessionEntryPoint(uint32_t paramTypes, TEE_Param __maybe_unused params[4],
    void __maybe_unused **sessionContext)
{
    TEE_Result ret=TEE_SUCCESS;
    DLOG("data_at_rest: TA_OpenSessionEntryPoint\n");

    UNUSED(paramTypes);
    UNUSED(params);
    UNUSED(sessionContext);

    return ret;
}

void TA_CloseSessionEntryPoint(void __maybe_unused *session_context)
{
    DLOG("data_at_rest: TA_CloseSessionEntryPoint\n");
    UNUSED(session_context);
}

void TA_DestroyEntryPoint(void)
{
    DLOG("data_at_rest: TA_DestroyEntryPoint\n");
}

TEE_Result TA_InvokeCommandEntryPoint(void __maybe_unused *session_context,
    uint32_t cmd_id,
    uint32_t param_types, TEE_Param params[4])
{
    int ret = 0;
    UNUSED(param_types);

    DLOG("CMD_ID = %d\n", cmd_id);

    switch (cmd_id) {
        case CMD_GEN_DEVICE_KEY:
            DLOG("Generate device key\n");
            ret = data_at_rest_ta_gen_device_key(param_types, params);
            break;
        case CMD_ENCRYPT:
            DLOG("encrypt data with aes\n");
            ret = data_at_rest_ta_aes_encrypt(param_types, params);
            break;
        case CMD_DECRYPT:
            DLOG("decrypt data with aes\n");
            ret = data_at_rest_ta_aes_decrypt(param_types, params);
            break;
        case CMD_FREE_DEVICE_KEY:
            DLOG("free device key\n");
            ret = data_at_rest_ta_free_device_key(param_types, params);
            break;
        default:
            DLOG("unexpected cmd_id %d\n", cmd_id);
            return TEE_ERROR_BAD_PARAMETERS;
    }

    if (ret) {
        DLOG("handle fail, ret %d\n", ret);
        return TEE_ERROR_BAD_STATE;
    }

    return  TEE_SUCCESS;
}

