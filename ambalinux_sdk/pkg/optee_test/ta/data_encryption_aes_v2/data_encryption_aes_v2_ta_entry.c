/*******************************************************************************
 * data_encryption_aes_v2_ta_entry.c
 *
 * History:
 *  2021/08/13 - [Bo-Xi Chen] create file
 *
 * Copyright (C) 2021 Ambarella International LP.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 ******************************************************************************/

#include "tee_internal_api.h"
#include "tee_api_defines.h"
#include "trace.h"
#include "tee_api_defines_extensions.h"

#include "data_encryption_aes_v2_ta_type.h"
#include "data_encryption_aes_v2_ta_handle.h"

TEE_Result TA_CreateEntryPoint(void)
{
    //DLOG("data_encryption_aes: TA_CreateEntryPoint\n");
    return TEE_SUCCESS;
}

TEE_Result TA_OpenSessionEntryPoint(uint32_t paramTypes, TEE_Param __maybe_unused params[4],
    void __maybe_unused **sessionContext)
{
    TEE_Result ret=TEE_SUCCESS;
    //DLOG("data_encryption_aes: TA_OpenSessionEntryPoint\n");

    UNUSED(paramTypes);
    UNUSED(params);
    UNUSED(sessionContext);

    return ret;
}

void TA_CloseSessionEntryPoint(void __maybe_unused *session_context)
{
    //DLOG("data_encryption_aes: TA_CloseSessionEntryPoint\n");
    UNUSED(session_context);
}

void TA_DestroyEntryPoint(void)
{
    //DLOG("data_encryption_aes: TA_DestroyEntryPoint\n");
}

TEE_Result TA_InvokeCommandEntryPoint(void __maybe_unused *session_context,
    uint32_t cmd_id,
    uint32_t paramTypes, TEE_Param params[4])
{
    int ret = 0;

    //DLOG("CMD_ID = %d\n", cmd_id);

    switch (cmd_id) {
        case CMD_AES_INIT:
            ret = data_encryption_aes_v2_ta_init(paramTypes, params);
            break;
        case CMD_AES_INIT_PASSWORD:
            ret = data_encryption_aes_v2_ta_init_password(paramTypes, params);
            break;
        case CMD_AES_UPDATE:
            ret = data_encryption_aes_v2_ta_update(paramTypes, params);
            break;
        case CMD_AES_FINAL:
            ret = data_encryption_aes_v2_ta_final(paramTypes, params);
            break;
        default:
            DLOG("unexpected cmd_id %d\n", cmd_id);
            return TEE_ERROR_BAD_PARAMETERS;
            break;
    }

    if (ret) {
        DLOG("handle fail, ret %d\n", ret);
        return TEE_ERROR_BAD_STATE;
    }

    return  TEE_SUCCESS;
}


