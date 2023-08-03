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

#include "secure_credential_provisioning_ca_type.h"

#include "secure_credential_provisioning_ca_gen_key.h"

int secure_credential_provisioning_grenrate_ecdsa_key(void)
{
    TEEC_Context context;
    TEEC_Session session;
    TEEC_Result result;
    TEEC_Operation operation;
    uint32_t origin;
    TEEC_UUID svc_id = SECURE_CREDENTIAL_PROVISIONING_UUID;

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

    operation.paramTypes = TEEC_PARAM_TYPES(TEEC_NONE,
                                            TEEC_NONE, TEEC_NONE,
                                            TEEC_NONE);

    result = TEEC_InvokeCommand(&session, CMD_GENERATE_KEY, &operation, &origin);

    if (result != TEEC_SUCCESS) {
        printf("generate_key TEEC_InvokeCommand failed, result=0x%x, origin=0x%x\n", result, origin);
        goto cleanup_2;
    }

cleanup_2:
    TEEC_CloseSession(&session);
cleanup_1:
    TEEC_FinalizeContext(&context);

    return result;

}

