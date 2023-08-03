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

#include "data_at_rest_ta_generate_key.h"
#include "data_at_rest_ta_type.h"

int data_at_rest_ta_set_aes_key(char* key)
{
    TEE_Result result;
    size_t aes_key_size = 256;
    TEE_Attribute attr;
    TEE_ObjectHandle transient_key = NULL;
    TEE_ObjectHandle AES_key = (TEE_ObjectHandle)NULL;
    const char* objectID = "device_key";
    uint32_t flags = TEE_DATA_FLAG_ACCESS_READ |
             TEE_DATA_FLAG_ACCESS_WRITE |
             TEE_DATA_FLAG_ACCESS_WRITE_META |
             TEE_DATA_FLAG_SHARE_READ |
             TEE_DATA_FLAG_SHARE_WRITE |
             TEE_DATA_FLAG_OVERWRITE;

    result = TEE_AllocateTransientObject(TEE_TYPE_AES, aes_key_size, &transient_key);
    if (TEE_SUCCESS != result) {
       DLOG("TEE_AllocateTransientObject fail, result 0x%x\n", result);
       goto cleanup_2;
    }

    /* Set the key object parameter */
    TEE_InitRefAttribute(&attr, TEE_ATTR_SECRET_VALUE, key, 16);
    result = TEE_PopulateTransientObject(transient_key, &attr, 1);
    if (TEE_SUCCESS != result) {
       DLOG("TEE_PopulateTransientObject fail, result 0x%x\n", result);
       goto cleanup_1;
    }

    result = TEE_CreatePersistentObject(TEE_STORAGE_PRIVATE,
                        objectID, sizeof(objectID),
                        flags, transient_key, NULL, 0,
                        &AES_key);
    if (TEE_SUCCESS != result) {
        DLOG("Failed to create a persistent key: 0x%x", result);
        goto cleanup_2;
    }
    DLOG("create persistent key success\n");
    TEE_CloseObject(AES_key);

cleanup_2:
    TEE_FreeTransientObject(transient_key);
cleanup_1:
    return result;

}

int data_at_rest_ta_free_peristent_obj(char* objectID)
{
    TEE_Result result;
    TEE_ObjectHandle per_obj = (TEE_ObjectHandle)NULL;
    uint32_t flags = TEE_DATA_FLAG_ACCESS_READ |
             TEE_DATA_FLAG_ACCESS_WRITE |
             TEE_DATA_FLAG_ACCESS_WRITE_META |
             TEE_DATA_FLAG_SHARE_READ |
             TEE_DATA_FLAG_SHARE_WRITE |
             TEE_DATA_FLAG_OVERWRITE;

    result = TEE_OpenPersistentObject(TEE_STORAGE_PRIVATE, objectID, sizeof(objectID), flags, &per_obj);
    if (result != TEE_SUCCESS) {
        DLOG("Failed to open persistent key: 0x%x", result);
        goto cleanup_1;
    }

    TEE_CloseAndDeletePersistentObject(per_obj);


 cleanup_1:
    return result;
}
