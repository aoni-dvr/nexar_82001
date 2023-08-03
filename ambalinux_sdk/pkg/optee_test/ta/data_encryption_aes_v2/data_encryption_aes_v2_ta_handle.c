/*******************************************************************************
 * data_encryption_aes_v2_ta.c
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
#include "tee_api_types.h"
#include "trace.h"
#include "tee_api_defines_extensions.h"

#include "data_encryption_aes_v2_ta_type.h"
#include "data_encryption_aes_v2_ta.h"

#include "data_encryption_aes_v2_ta_handle.h"

extern const void *ta_head;
#define VAL2HANDLE(v) (void *)(uintptr_t)(v == TEE_HANDLE_NULL ? v : v + (uintptr_t)&ta_head)
#define HANDLE2VAL(h) (uint32_t)(h == TEE_HANDLE_NULL ? (uintptr_t)h : (uintptr_t)((uintptr_t)h - (uintptr_t)&ta_head))

#if 0
static void TA_Printf(char* buf, unsigned int len)
{
    unsigned int index = 0U;
    for(index = 0U; index < len; index++)
    {
        if(index < 15U)
        {
        }
        else if(0U == index%16U)
        {
            DLOG("\n");
        }
        else
        {
        }

        DLOG("0x%02x, ", (buf[index] & 0xFFU));

    }
    DLOG("\n\n");
}
#endif

static void __set_aes_iv(aes_operation_t *op)
{
    char *buf = NULL;
    buf = TEE_Malloc(16, 0x0);

    op->iv = buf;
    op->iv_len = 16;
}

static void __set_info(aes_operation_t *op, aes_operation_info_t *info)
{
    switch (info->action) {
        case E_AES_OP_ENCRYPT:
            op->mode = TEE_MODE_ENCRYPT;
            break;
        case E_AES_OP_DECRYPT:
            op->mode = TEE_MODE_DECRYPT;
            break;
        default:
            DLOG("invalid action %d\n", info->action);
            break;
    }

    switch (info->mode) {
        case E_AES_MODE_CBC:
            op->algorithm_id = TEE_ALG_AES_CBC_NOPAD;
            break;
        case E_AES_MODE_ECB:
            op->algorithm_id = TEE_ALG_AES_ECB_NOPAD;
            break;
        case E_AES_MODE_CTR:
            op->algorithm_id = TEE_ALG_AES_CTR;
            break;
        case E_AES_MODE_XTS:
            op->algorithm_id = TEE_ALG_AES_XTS;
            break;
        default:
            DLOG("invalid mode %d\n", info->mode);
            break;
    }
}


int data_encryption_aes_v2_ta_init(unsigned int paramTypes, TEE_Param params[4])
{
    aes_operation_t op;
    aes_operation_info_t info;
    unsigned int *buf;
    UNUSED(paramTypes);

    buf = params[0].memref.buffer;
    info.mode = (E_AES_MODE)buf[0];
    info.action = (E_AES_OPERATION)buf[1];
    op.key_len = buf[2];
    op.key = params[1].memref.buffer;
    op.key2 = params[2].memref.buffer;
    op.operation = TEE_HANDLE_NULL;

    __set_aes_iv(&op);
    __set_info(&op, &info);

    data_encryption_aes_v2_ta_init_oper(&op);

    params[3].value.a = HANDLE2VAL(op.operation);

    return 0;
}

int data_encryption_aes_v2_ta_init_password(unsigned int paramTypes, TEE_Param params[4])
{
    aes_operation_t op;
    aes_operation_info_t info;
    unsigned int *buf;
    UNUSED(paramTypes);

    char* password;
    unsigned int password_len;

    buf = params[0].memref.buffer;
    info.mode = (E_AES_MODE)buf[0];
    info.action = (E_AES_OPERATION)buf[1];
    op.key_len = buf[2];
    password = params[1].memref.buffer;
    password_len = params[1].memref.size;
    op.operation = TEE_HANDLE_NULL;

    __set_aes_iv(&op);
    __set_info(&op, &info);

    op.key = TEE_Malloc(64, 0);
    data_encryption_aes_ta_v2_sha512_oper(password, password_len, op.key);
    if (info.mode == E_AES_MODE_XTS) {
    op.key2 = op.key  + op.key_len / 8;   //set key2 for aes-xts
    }

    data_encryption_aes_v2_ta_init_oper(&op);

    params[2].value.a = HANDLE2VAL(op.operation);

    return 0;
}

int data_encryption_aes_v2_ta_update(unsigned int paramTypes, TEE_Param params[4])
{
    aes_operation_t op;
    UNUSED(paramTypes);

    op.in_buf = params[0].memref.buffer;
    op.data_len = params[0].memref.size;
    op.out_buf = params[1].memref.buffer;
    op.process_len = params[1].memref.size;
    op.out_len = params[2].value.a;
    op.operation = VAL2HANDLE(params[3].value.a);

    data_encryption_aes_v2_ta_update_oper(&op);

    params[2].value.a = op.process_len;

    return 0;
}

int data_encryption_aes_v2_ta_final(unsigned int paramTypes, TEE_Param params[4])
{
    aes_operation_t op;
    UNUSED(paramTypes);

    op.in_buf = params[0].memref.buffer;
    op.data_len = params[0].memref.size;
    op.operation = VAL2HANDLE(params[3].value.a);
    op.out_buf = params[1].memref.buffer;
    op.out_len = params[2].value.a;

    data_encryption_aes_v2_ta_final_oper(&op);
    params[2].value.a = op.out_len;
    TEE_FreeOperation(op.operation);

    return 0;
}

