/*******************************************************************************
 * data_encryption_aes_ta_v2.c
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
#include "data_encryption_aes_v2_ta.h"

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

int data_encryption_aes_ta_v2_sha512_oper(char* input, unsigned int inlen, char* output)
{
    TEE_Result result = 0;
    TEE_OperationHandle operationhandle;
    int Retval = 0;
    unsigned int outlen = 64;

    result = TEE_AllocateOperation(&operationhandle, TEE_ALG_SHA512, TEE_MODE_DIGEST, 0);
    if(result != TEE_SUCCESS)
    {
        DLOG("Allocate SHA operation handle fail\n");
        Retval = -1;
        goto cleanup_1;
    }

    TEE_DigestUpdate(operationhandle, input, inlen);

    result = TEE_DigestDoFinal(operationhandle, NULL, 0, output, &outlen);
    if(result != TEE_SUCCESS)
    {
        DLOG("Do the final sha operation fail\n");
        Retval = -1;
        goto cleanup_2;
    }

    cleanup_2:
        TEE_FreeOperation(operationhandle);
    cleanup_1:
        return Retval;
}

int data_encryption_aes_v2_ta_init_oper(aes_operation_t *op)
{
    unsigned int op_keysize = op->key_len;
    TEE_Attribute attr;
    TEE_Result result = 0;
    TEE_ObjectHandle key_obj = NULL;
    TEE_ObjectHandle key2_obj = NULL;   //key2 for AES-XTS

    if (op->algorithm_id == TEE_ALG_AES_XTS) {
        op_keysize *= 2;
    }

    result = TEE_AllocateOperation(&(op->operation), op->algorithm_id, op->mode, op_keysize);
    if (TEE_SUCCESS != result) {
        DLOG("TEE_AllocateOperation fail, result 0x%x\n", result);
        goto tag_exit_aes_init;
    }

    result = TEE_AllocateTransientObject(TEE_TYPE_AES, op->key_len, &key_obj);
    if (TEE_SUCCESS != result) {
        DLOG("TEE_AllocateTransientObject fail, result 0x%x\n", result);
        goto tag_exit_aes_init;
    }

    TEE_InitRefAttribute(&attr, TEE_ATTR_SECRET_VALUE, op->key, op->key_len /8);
    result = TEE_PopulateTransientObject(key_obj, &attr, 1);
    if (TEE_SUCCESS != result) {
        DLOG("TEE_PopulateTransientObject fail, result 0x%x\n", result);
        goto tag_exit_aes_init;
    }

    if (op->algorithm_id == TEE_ALG_AES_XTS) {  //set key2 for aes-xts
        result = TEE_AllocateTransientObject(TEE_TYPE_AES, op->key_len, &key2_obj);
        if (TEE_SUCCESS != result) {
        DLOG("TEE_AllocateTransientObject key2 fail, result 0x%x\n", result);
        goto tag_exit_aes_init;
        }

        TEE_InitRefAttribute(&attr, TEE_ATTR_SECRET_VALUE, op->key2, op->key_len / 8);
        result = TEE_PopulateTransientObject(key2_obj, &attr, 1);
        if (TEE_SUCCESS != result) {
            DLOG("TEE_PopulateTransientObject fail, result 0x%x\n", result);
            goto tag_exit_aes_init;
        }

        result = TEE_SetOperationKey2(op->operation, key_obj, key2_obj);
        if (TEE_SUCCESS != result) {
            DLOG("TEE_SetOperationKey fail, result 0x%x\n", result);
            goto tag_exit_aes_init;
        }

        TEE_FreeTransientObject(key2_obj);
    } else {
        result = TEE_SetOperationKey(op->operation, key_obj);
        if (TEE_SUCCESS != result) {
            DLOG("TEE_SetOperationKey fail, result 0x%x\n", result);
            goto tag_exit_aes_init;
        }
    }

    TEE_CipherInit(op->operation, op->iv, op->iv_len);

    tag_exit_aes_init:
        return result;
}

int data_encryption_aes_v2_ta_update_oper(aes_operation_t *op) {
    char *in_buf = op->in_buf;
    char *out_buf = op->out_buf;
    unsigned int result;
    op->process_len = op->out_len;
    result = TEE_CipherUpdate(op->operation, in_buf, op->data_len, out_buf, &(op->process_len));
    if (TEE_SUCCESS != result) {
        DLOG("TEE_CipherUpdate fail, result 0x%x\n", result);
        return result;
    }

    return result;

}

int data_encryption_aes_v2_ta_final_oper(aes_operation_t *op) {
    char *in_buf = op->in_buf;
    char *out_buf = op->out_buf;
    unsigned int out_len = op->out_len;
    unsigned int result;

    result = TEE_CipherDoFinal(op->operation, in_buf, op->data_len, out_buf, &out_len);
    if (TEE_SUCCESS != result) {
        DLOG("TEE_CipherDoFinal fail, result 0x%x\n", result);
        return result;
    }
    op->out_len = out_len;

    return result;
}
