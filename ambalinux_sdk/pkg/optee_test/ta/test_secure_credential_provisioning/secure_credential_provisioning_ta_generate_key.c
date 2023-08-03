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

#include "secure_credential_provisioning_ta_generate_key.h"
#include "secure_credential_provisioning_ta_type.h"

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

int secure_credential_provisioning_ta_generate_ecdsa_key_oper(void)
{
    TEE_Result result;
    TEE_Attribute ecdsa_attrs[1];
    char output[256];
    unsigned int outlen = 256;
    size_t ecdsa_key_size = 256;
    TEE_ObjectHandle transient_key = NULL;
    TEE_ObjectHandle ECDSA_key = (TEE_ObjectHandle)NULL;
    uint32_t objectID = 1;
    uint32_t ECDSA_KEY_ID = TEE_STORAGE_PRIVATE;
    uint32_t flags = TEE_DATA_FLAG_ACCESS_READ |
             TEE_DATA_FLAG_ACCESS_WRITE |
             TEE_DATA_FLAG_ACCESS_WRITE_META |
             TEE_DATA_FLAG_SHARE_READ |
             TEE_DATA_FLAG_SHARE_WRITE |
             TEE_DATA_FLAG_OVERWRITE;

    result = TEE_AllocateTransientObject(TEE_TYPE_ECDSA_KEYPAIR, ecdsa_key_size, &transient_key);
    if(TEE_SUCCESS != result)
    {
        DLOG("Do allocate obj handle fail, Ret value is:0x%x\n", result);
        result = FAIL;
        goto cleanup_1;
    }

    ecdsa_attrs[0].attributeID = TEE_ATTR_ECC_CURVE;
    ecdsa_attrs[0].content.value.a = TEE_ECC_CURVE_NIST_P256;
    ecdsa_attrs[0].content.value.b = 0;

    result = TEE_GenerateKey(transient_key, ecdsa_key_size, ecdsa_attrs, 1);
    if(TEE_SUCCESS != result)
    {
        DLOG("Generate key fail, Ret value is:0x%x\n", result);
        result = FAIL;
        goto cleanup_1;
    }

    result = TEE_CreatePersistentObject(ECDSA_KEY_ID,
                        &objectID, sizeof(objectID),
                        flags, transient_key, NULL, 0,
                        &ECDSA_key);
    if (result != TEE_SUCCESS) {
        DLOG("Failed to create a persistent key: 0x%x", result);
        goto cleanup_2;
    }

    TEE_CloseObject(ECDSA_key);

    result = TEE_OpenPersistentObject(ECDSA_KEY_ID,
                      &objectID, sizeof(objectID),
                      flags, &ECDSA_key);
    if (result != TEE_SUCCESS) {
        DLOG("Failed to open persistent key: 0x%x", result);
        goto cleanup_2;
    }

    result = TEE_GetObjectBufferAttribute(ECDSA_key, TEE_ATTR_ECC_PRIVATE_VALUE, output, &outlen);
    if(TEE_SUCCESS != result)
    {
        DLOG("Do populate obj handle fail, Ret value is:0x%x\n", result);
        result = FAIL;
        goto cleanup_2;
    }

    DLOG("ECDSA private key is:\n");
    TA_Printf(output, outlen);

#if 0
    result = TEE_GetObjectBufferAttribute(ECDSA_key, TEE_ATTR_ECC_PUBLIC_VALUE_X, output, &outlen);
    if(TEE_SUCCESS != result)
    {
        DLOG("Do populate obj handle fail, Ret value is:0x%x\n", result);
        result = FAIL;
        goto cleanup_2;
    }

    DLOG("ECDSA pub A key is:\n");
    TA_Printf(output, outlen);
#endif


    return OK;
cleanup_1:
    return FAIL;
cleanup_2:
    TEE_FreeTransientObject(transient_key);
    transient_key = TEE_HANDLE_NULL;
    return FAIL;
}

