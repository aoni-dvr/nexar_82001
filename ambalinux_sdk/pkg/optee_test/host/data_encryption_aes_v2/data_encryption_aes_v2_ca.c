/*******************************************************************************
 * data_encryption_aes_v2_ca.c
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

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include "tee_client_api.h"
#include "tee_api_types.h"
#include <unistd.h>

#include <assert.h>

#include "data_encryption_aes_v2_ta_type.h"

#include "data_encryption_aes_v2_ca.h"

#define MAX_UPDATE_SIZE 20 * 1024

#if 0
static void print_memory(unsigned char *p, int size)
{
    while (size > 7) {
        printf("%02x %02x %02x %02x %02x %02x %02x %02x\n",
               p[0], p[1], p[2], p[3],
               p[4], p[5], p[6], p[7]);
        p += 8;
        size -= 8;
    }

    while (size) {
        printf("%02x ", p[0]);
        p ++;
        size --;
    }

    printf("\n");
}
#endif

void free_aes_context(data_encryption_aes_v2_test_context* ctx)
{
    TEEC_CloseSession(&(ctx->session));
    TEEC_FinalizeContext(&(ctx->context));
    ctx->op_handle = TEE_HANDLE_NULL;

    free(ctx);
}

TEEC_Result aes_init_password(data_encryption_aes_v2_test_context* ctx,
                   char *password, unsigned int password_len)
{
    TEEC_Result result;
    TEEC_UUID svc_id = DATA_ENCRYPTION_AES_V2_UUID_ID;
    uint32_t origin;
    unsigned int input_info[3];
    TEEC_Operation operation;

    input_info[0] = ctx->e_aes_mode;
    input_info[1] = ctx->e_aes_oper;
    input_info[2] = ctx->e_aes_len;

    result = TEEC_InitializeContext(NULL, &(ctx->context));
    if (result != TEEC_SUCCESS) {
        printf("TEEC_InitializeContext fail, result=0x%x\n", result);
        return result;
    }

    result = TEEC_OpenSession(&(ctx->context), &(ctx->session), &svc_id,
                              TEEC_LOGIN_PUBLIC, NULL, NULL, &origin);
    if (result != TEEC_SUCCESS) {
        printf("TEEC_OpenSession failed, result=0x%x, origin=0x%x\n", result, origin);
        goto tag_exit_aes_init_1;
    }

    memset(&operation, 0x0, sizeof(operation));
    operation.started = 1;
    operation.paramTypes = TEEC_PARAM_TYPES(TEEC_MEMREF_TEMP_INPUT, TEEC_MEMREF_TEMP_INPUT,
                                            TEEC_VALUE_INOUT, TEEC_NONE);
    operation.params[0].tmpref.size = sizeof(input_info);
    operation.params[0].tmpref.buffer = input_info;
    operation.params[1].tmpref.size = password_len;
    operation.params[1].tmpref.buffer = password;
    operation.params[2].value.a = TEE_HANDLE_NULL;

    result = TEEC_InvokeCommand(&(ctx->session), CMD_AES_INIT_PASSWORD, &operation, &origin);
    if (result != TEEC_SUCCESS) {
        printf("TEEC_InvokeCommand fail, result=0x%x, origin=0x%x\n", result, origin);
        goto tag_exit_aes_init_2;
    }

    ctx->op_handle = (TEE_OperationHandle)(uintptr_t)operation.params[2].value.a;

    return result;

tag_exit_aes_init_2:
    TEEC_CloseSession(&(ctx->session));
tag_exit_aes_init_1:
    TEEC_FinalizeContext(&(ctx->context));

    return result;
}
TEEC_Result aes_init(data_encryption_aes_v2_test_context* ctx,
                   char *key, char *key2)
{
    TEEC_Result result;
    TEEC_UUID svc_id = DATA_ENCRYPTION_AES_V2_UUID_ID;
    uint32_t origin;
    unsigned int input_info[3];
    TEEC_Operation operation;

    input_info[0] = ctx->e_aes_mode;
    input_info[1] = ctx->e_aes_oper;
    input_info[2] = ctx->e_aes_len;

    result = TEEC_InitializeContext(NULL, &(ctx->context));
    if (result != TEEC_SUCCESS) {
        printf("TEEC_InitializeContext fail, result=0x%x\n", result);
        return result;
    }

    result = TEEC_OpenSession(&(ctx->context), &(ctx->session), &svc_id,
                              TEEC_LOGIN_PUBLIC, NULL, NULL, &origin);
    if (result != TEEC_SUCCESS) {
        printf("TEEC_OpenSession failed, result=0x%x, origin=0x%x\n", result, origin);
        goto tag_exit_aes_init_1;
    }

    memset(&operation, 0x0, sizeof(operation));
    operation.started = 1;
    operation.paramTypes = TEEC_PARAM_TYPES(TEEC_MEMREF_TEMP_INPUT, TEEC_MEMREF_TEMP_INPUT,
                                            TEEC_MEMREF_TEMP_INPUT, TEEC_VALUE_INOUT);
    operation.params[0].tmpref.size = sizeof(input_info);
    operation.params[0].tmpref.buffer = input_info;
    operation.params[1].tmpref.size = strlen(key);
    operation.params[1].tmpref.buffer = key;
    if (ctx->e_aes_mode== E_AES_MODE_XTS) {
        operation.params[2].tmpref.size = strlen(key2);
        operation.params[2].tmpref.buffer = key2;
    }
    operation.params[3].value.a = TEE_HANDLE_NULL;

    result = TEEC_InvokeCommand(&(ctx->session), CMD_AES_INIT, &operation, &origin);
    if (result != TEEC_SUCCESS) {
        printf("TEEC_InvokeCommand fail, result=0x%x, origin=0x%x\n", result, origin);
        goto tag_exit_aes_init_2;
    }

    ctx->op_handle = (TEE_OperationHandle)(uintptr_t)operation.params[3].value.a;

    return result;

tag_exit_aes_init_2:
    TEEC_CloseSession(&(ctx->session));
tag_exit_aes_init_1:
    TEEC_FinalizeContext(&(ctx->context));

    return result;
}


TEEC_Result aes_update(data_encryption_aes_v2_test_context* ctx, unsigned char* input, unsigned int in_len, unsigned char* output, unsigned int* out_len)
{
    TEEC_Result result;
    uint32_t origin;
    TEEC_Operation operation;
    unsigned int update_len;
    unsigned int outlen;    //processed len per block
    unsigned int block_num;
    unsigned int block_size = MAX_UPDATE_SIZE;
    unsigned char *p_in = input;
    unsigned char *p_out = output;
    unsigned int remain_len = *out_len;
    unsigned int out_len_block;

    if ((ctx->e_aes_mode == E_AES_MODE_XTS)) {
        if (in_len < 16) {
            printf("error: aes_xts need at least 16 bytes data.\n");
            return TEEC_ERROR_BAD_PARAMETERS;
        }
        update_len = in_len;
    } else {
        update_len = (in_len % 16 == 0)? (in_len - 16) : (in_len - in_len % 16);  //leave last block for final
    }

    block_num = (update_len % block_size == 0) ? (update_len / block_size) : (update_len / block_size + 1);
    for (unsigned int i = 0; i < block_num; i++) {
        out_len_block = (update_len < (unsigned int) MAX_UPDATE_SIZE) ? (update_len) : ((unsigned int) MAX_UPDATE_SIZE);
        memset(&operation, 0x0, sizeof(operation));
        operation.started = 1;
        operation.paramTypes = TEEC_PARAM_TYPES(TEEC_MEMREF_TEMP_INPUT, TEEC_MEMREF_TEMP_OUTPUT,
                                                TEEC_VALUE_INOUT, TEEC_VALUE_INOUT);
        operation.params[0].tmpref.size = (update_len < (unsigned int) MAX_UPDATE_SIZE) ? (update_len) : ((unsigned int) MAX_UPDATE_SIZE);
        operation.params[0].tmpref.buffer = p_in;
        operation.params[1].tmpref.size = block_size;
        operation.params[1].tmpref.buffer = p_out;
        operation.params[2].value.a = out_len_block;
        assert((uintptr_t)(ctx->op_handle) <= UINT32_MAX);
        operation.params[3].value.a = (uint32_t)(uintptr_t)(ctx->op_handle);

        result = TEEC_InvokeCommand(&(ctx->session), CMD_AES_UPDATE, &operation, &origin);
        if (result != TEEC_SUCCESS) {
            printf("TEEC_InvokeCommand fail, result=0x%x, origin=0x%x\n", result, origin);
            TEEC_CloseSession(&(ctx->session));
            return result;
        }

        outlen = operation.params[2].value.a;

        if ((ctx->e_aes_mode == E_AES_MODE_XTS) && (i == 0)) {  //aes-xts will store 16 bytes of date in operationhandle.
            ctx->process_len += outlen;
            p_in = p_in + outlen + 16;
            update_len =update_len - outlen - 16;
        } else {
            ctx->process_len += outlen;
            p_in += outlen;
            update_len -= outlen;
        }
        p_out += outlen;
        remain_len -= outlen;   //remain buf_len

        ctx->op_handle = (TEE_OperationHandle)(uintptr_t)operation.params[3].value.a;
    }

    *out_len = remain_len;
    return result;

}

TEEC_Result aes_final(data_encryption_aes_v2_test_context* ctx, unsigned char* input, unsigned int in_len, unsigned char* output, unsigned int* out_len)
{
    TEEC_Operation operation;
    TEEC_Result result;
    uint32_t origin;

    unsigned char *p = NULL, *p_out = NULL;
    int i = 0, q = 0;

    input += ctx->process_len;
    p_out = output;
    p_out += ctx->process_len;

    if (ctx->e_aes_mode == E_AES_MODE_XTS) {
        in_len = 0;
    } else {
        in_len -= ctx->process_len;
    }

    if (ctx->e_aes_oper == E_AES_OP_ENCRYPT) {  //padding data for encryption
        if ((ctx->e_aes_mode == E_AES_MODE_CBC) || (ctx->e_aes_mode == E_AES_MODE_ECB)) {   //padding data
            p = input + in_len;
            q = in_len % 16;
            if (q == 0) {
                for (i = 0; i < 16; i++) {
                    *p = 16;
                    p++;
                }
                *p = '\0';
                in_len += 16;
            } else {
                for (i = 0; i < (16 - q) ; i++) {
                    *p =  16 - q;
                    p++;
                }
                *p = '\0';
                in_len = in_len + 16 - q;
            }
        }
    }

    if ((in_len > 0) || (ctx->e_aes_mode == E_AES_MODE_XTS)) {  //aes-xts will always do aes_final
        memset(&operation, 0x0, sizeof(operation));
        operation.started = 1;
        operation.paramTypes = TEEC_PARAM_TYPES(TEEC_MEMREF_TEMP_INPUT, TEEC_MEMREF_TEMP_OUTPUT,
                                                TEEC_VALUE_INOUT, TEEC_VALUE_INPUT);
        operation.params[0].tmpref.size = in_len;
        operation.params[0].tmpref.buffer = input;
        operation.params[1].tmpref.size = 32;
        operation.params[1].tmpref.buffer = p_out;
        operation.params[2].value.a = *out_len;
        assert((uintptr_t)(ctx->op_handle) <= UINT32_MAX);
        operation.params[3].value.a = (uint32_t)(uintptr_t)(ctx->op_handle);


        result = TEEC_InvokeCommand(&(ctx->session), CMD_AES_FINAL, &operation, &origin);
        if (result != TEEC_SUCCESS) {
            printf("TEEC_InvokeCommand fail, result=0x%x, origin=0x%x\n", result, origin);
            goto tag_exit_aes_final;
        }

        *out_len = operation.params[2].value.a;
    }

    *out_len += ctx->process_len;

    if (ctx->e_aes_oper == E_AES_OP_DECRYPT) {
        if ((ctx->e_aes_mode == E_AES_MODE_CBC) || (ctx->e_aes_mode == E_AES_MODE_ECB)) {
            p = output + *out_len - 1;
            q = *p;
            if (q > 16) {
                printf("padding error: wrong password?\n");
            } else {
                *out_len = *out_len - q;
            }

            p = output + *out_len;
            *p  = '\0';

        }
    }
tag_exit_aes_final:
    TEEC_CloseSession(&(ctx->session));

    return result;
}


