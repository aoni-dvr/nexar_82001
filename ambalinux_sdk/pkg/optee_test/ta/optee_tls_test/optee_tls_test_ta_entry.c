/*******************************************************************************
 * optee_tls_test_ta_entry.c
 *
 * History:
 *  2021/01/05 - [Bo-Xi Chen] create file
 *
 * Copyright 2020 Ambarella International LP
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

#include "optee_tls_test_ta_type.h"
#include "optee_tls_test_ta_handle.h"

TEE_Result TA_CreateEntryPoint(void)
{
    DLOG("optee_tls_test: TA_CreateEntryPoint\n");
    return TEE_SUCCESS;
}

TEE_Result TA_OpenSessionEntryPoint(uint32_t paramTypes, TEE_Param __maybe_unused params[4],
    void **sessionContext)
{
    DLOG("optee_tls_test: TA_OpenSessionEntryPoint\n");

    UNUSED(paramTypes);
    UNUSED(params);

    optee_tls_client_context* ctx;
    ctx = TEE_Malloc(sizeof(optee_tls_client_context), 0);
    if (!ctx) {
        return TEE_ERROR_OUT_OF_MEMORY;
    }

    ctx->socket_handle = TEE_Malloc(sizeof(optee_tls_socket_handle), 0);
    if (!ctx->socket_handle) {
        return TEE_ERROR_OUT_OF_MEMORY;
    }

    ctx->ssl = TEE_Malloc(sizeof(mbedtls_ssl_context), 0);
    if (!ctx->ssl) {
        return TEE_ERROR_OUT_OF_MEMORY;
    }

    *sessionContext = (void *)ctx;

    return TEE_SUCCESS;
}

void TA_CloseSessionEntryPoint(void *session_context)
{
    DLOG("optee_tls_test: TA_CloseSessionEntryPoint\n");
    optee_tls_client_context *ctx = (optee_tls_client_context *)session_context;

    if (ctx->socket_handle) {
        TEE_Free(ctx->socket_handle);
    }
    if (ctx->ssl) {
        TEE_Free(ctx->ssl);
    }
    if (ctx) {
        TEE_Free(ctx);
    }
}

void TA_DestroyEntryPoint(void)
{
    DLOG("optee_tls_test: TA_DestroyEntryPoint\n");
}

TEE_Result TA_InvokeCommandEntryPoint(void *session_context,
    uint32_t cmd_id,
    uint32_t paramTypes, TEE_Param params[4])
{
    int ret = 0;

    DLOG("CMD_ID = %d\n", cmd_id);

    switch (cmd_id) {
        case CMD_OPEN:
            DLOG("open tls connection\n");
            ret = optee_tls_ta_open(session_context, paramTypes, params);
            break;
        case CMD_CLOSE:
            DLOG("close tls connection\n");
            ret = optee_tls_ta_close(session_context, paramTypes, params);
            break;
        case CMD_WRITE:
            DLOG("start write operation\n");
            ret = optee_tls_ta_write(session_context, paramTypes, params);
            break;
        case CMD_READ:
            DLOG("start read operation\n");
            ret = optee_tls_ta_read(session_context, paramTypes, params);
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

