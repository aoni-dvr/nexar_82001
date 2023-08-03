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
#include "data_at_rest_ca_get_otp_data.h"

#define PTA_AMBA_OTP_UUID \
    { 0xabcdef12, 0x7e33, 0x4ad2, \
        { 0x98, 0x02, 0xe6, 0x4f, 0x2a, 0x7c, 0xc2, 0x30 } }

#define AMBA_OTP_CMD_READ_AMBA_UNIQUE_ID   2

TEEC_Result data_at_rest_read_otp_amba_unique_id(unsigned char *p_id, unsigned int len)
{
    TEEC_Context context;
    TEEC_Session session;
    TEEC_Operation operation;
    TEEC_Result result;
    TEEC_UUID svc_id = PTA_AMBA_OTP_UUID;
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
        goto tag_exit_data_at_rest_read_otp_amba_unique_id_1;
    }

    memset(&operation, 0x0, sizeof(operation));
    operation.started = 1;
    operation.paramTypes = TEEC_PARAM_TYPES(TEEC_MEMREF_TEMP_OUTPUT, TEEC_NONE,
                                            TEEC_NONE, TEEC_NONE);
    operation.params[0].tmpref.size = len;
    operation.params[0].tmpref.buffer = p_id;

    result = TEEC_InvokeCommand(&session, AMBA_OTP_CMD_READ_AMBA_UNIQUE_ID, &operation, &origin);
    if (result != TEEC_SUCCESS) {
        printf("TEEC_InvokeCommand fail, result=0x%x, origin=0x%x\n", result, origin);
        goto tag_exit_data_at_rest_read_otp_amba_unique_id_2;
    }

tag_exit_data_at_rest_read_otp_amba_unique_id_2:
    TEEC_CloseSession(&session);
tag_exit_data_at_rest_read_otp_amba_unique_id_1:
    TEEC_FinalizeContext(&context);

    return result;
}
