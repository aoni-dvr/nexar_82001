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
#include <stdio.h>
#include <string.h>

#include "tee_client_api.h"

#include "data_at_rest_ca_type.h"

#include "data_at_rest_ca_aes_encrypt.h"

TEEC_Result aes_encrypt(char *p_data, unsigned int len, E_AES_MODE mode,
                        char *output)
{
    TEEC_Context context;
    TEEC_Session session;
    TEEC_Operation operation;
    TEEC_Result result;
    TEEC_UUID svc_id = DATA_AT_REST_UUID;
    uint32_t origin;

    result = TEEC_InitializeContext(NULL, &context);
    if (result != TEEC_SUCCESS) {
        printf("TEEC_InitializeContext fail, result=0x%x\n", result);
        return result;
    }

    result = TEEC_OpenSession(&context, &session, &svc_id,
                              TEEC_LOGIN_PUBLIC, NULL, NULL, &origin);
    if (result != TEEC_SUCCESS) {
        printf("TEEC_OpenSession failed, result=0x%x, origin=0x%x\n", result, origin);
        goto cleanup_1;
    }

    memset(&operation, 0x0, sizeof(operation));
    operation.started = 1;
    operation.paramTypes = TEEC_PARAM_TYPES(TEEC_VALUE_INPUT, TEEC_MEMREF_TEMP_INPUT,
                                            TEEC_MEMREF_TEMP_OUTPUT, TEEC_VALUE_INPUT);
    operation.params[0].value.a = mode;
    operation.params[1].tmpref.size = len;
    operation.params[1].tmpref.buffer = p_data;
    operation.params[2].tmpref.size = len;
    operation.params[2].tmpref.buffer = output;
    operation.params[3].value.a = len;

    result = TEEC_InvokeCommand(&session, CMD_ENCRYPT, &operation, &origin);
    if (result != TEEC_SUCCESS) {
        printf("TEEC_InvokeCommand fail, result=0x%x, origin=0x%x\n", result, origin);
        goto cleanup_2;
    }

cleanup_2:
    TEEC_CloseSession(&session);
cleanup_1:
    TEEC_FinalizeContext(&context);

    return result;
}
