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
#include "tee_internal_api.h"

#include "data_at_rest_ta_aes_decrypt.h"
#include "data_at_rest_ta_type.h"

int data_at_rest_ta_aes_decrypt_oper(aes_operation_t *op)
{
    TEE_OperationHandle operation = NULL;
    TEE_ObjectHandle key_obj;
    char* in_buf = op->in_buf;
    char* out_buf = op->out_buf;
    unsigned int data_len = op->data_len;
    TEE_Result result = 0;
    const char* objectID = "device_key";
    uint32_t flags = TEE_DATA_FLAG_ACCESS_READ |
             TEE_DATA_FLAG_ACCESS_WRITE |
             TEE_DATA_FLAG_ACCESS_WRITE_META |
             TEE_DATA_FLAG_SHARE_READ |
             TEE_DATA_FLAG_SHARE_WRITE |
             TEE_DATA_FLAG_OVERWRITE;

    /* Allocate the operation handle */
    result = TEE_AllocateOperation(&operation, op->algorithm_id, TEE_MODE_DECRYPT, op->key_len);
    if (TEE_SUCCESS != result) {
        DLOG("TEE_AllocateOperation fail, result 0x%x\n", result);
        goto cleanup_1;
    }

    result = TEE_OpenPersistentObject(TEE_STORAGE_PRIVATE, objectID, sizeof(objectID), flags, &key_obj);
        if (result != TEE_SUCCESS) {
            DLOG("Failed to open persistent key: 0x%x", result);
        }

    /* Assemble aes operation handle */
    result = TEE_SetOperationKey(operation, key_obj);
    if (TEE_SUCCESS != result) {
        DLOG("TEE_SetOperationKey fail, result 0x%x\n", result);
        goto cleanup_2;
    }

    /* Initialze cipher operation */
    TEE_CipherInit(operation, op->iv, op->iv_len);

    /* Do the final AES operation */
    result = TEE_CipherDoFinal(operation, in_buf, data_len, out_buf, &data_len);
    if (TEE_SUCCESS != result) {
        DLOG("TEE_SetOperationKey fail, result 0x%x\n", result);
        goto cleanup_2;
    }

cleanup_2:
    TEE_FreeOperation(operation);
    TEE_CloseObject(key_obj);
cleanup_1:
    return result;
}

