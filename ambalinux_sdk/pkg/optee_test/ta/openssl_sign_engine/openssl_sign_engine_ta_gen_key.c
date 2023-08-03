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

#include "openssl_sign_engine_ta_gen_key.h"
#include "openssl_sign_engine_ta_type.h"

#if 0
static void TA_Printf(char* buf, unsigned int len)
{
    unsigned int index = 0U;
    for(index = 0U; index < len; index++) {
        if(index < 15U) {
        }
        else if(0U == index%16U) {
            DLOG("\n");
        }
        else {
        }

        DLOG("0x%02x, ", (buf[index] & 0xFFU));

    }
    DLOG("\n\n");
}
#endif

int openssl_sign_engine_ta_generate_key_oper(int key_size, int exp, int exp_length, uint8_t* modulus, int mod_length)
{
    TEE_Result result;
    //uint32_t *output;
    //uint32_t outlen;
    char* exp_buf = NULL;
    TEE_ObjectHandle transient_key = NULL;
    TEE_ObjectHandle openssl_sign_engine_key = (TEE_ObjectHandle)NULL;
    TEE_Attribute rsa_attrs[1];
    uint32_t objectID = 1;
    uint32_t KEY_ID = TEE_STORAGE_PRIVATE;
    uint32_t flags = TEE_DATA_FLAG_ACCESS_READ |
             TEE_DATA_FLAG_ACCESS_WRITE |
             TEE_DATA_FLAG_ACCESS_WRITE_META |
             TEE_DATA_FLAG_SHARE_READ |
             TEE_DATA_FLAG_SHARE_WRITE |
             TEE_DATA_FLAG_OVERWRITE;


    result = TEE_AllocateTransientObject(TEE_TYPE_RSA_KEYPAIR, key_size, &transient_key);
    if(TEE_SUCCESS != result) {
        DLOG("Do allocate obj handle fail, Ret value is:0x%x\n", result);
        result = FAIL;
        goto cleanup_1;
    }

    exp_buf = (char *)&exp;

    TEE_InitRefAttribute(&rsa_attrs[0], TEE_ATTR_RSA_PUBLIC_EXPONENT, (void*)exp_buf, (exp_length + 1) / 2);

    result = TEE_GenerateKey(transient_key, key_size, rsa_attrs, 1);
    if(TEE_SUCCESS != result) {
        DLOG("Generate key fail, Ret value is:0x%x\n", result);
        result = FAIL;
        goto cleanup_1;
    }

#if 0
    uint8_t exp1[5];
    uint32_t len = 5;

    TEE_GetObjectBufferAttribute(transient_key, TEE_ATTR_RSA_PUBLIC_EXPONENT,
                               exp1,
                               &len);
    TA_Printf(exp1, len);
#endif

    result = TEE_CreatePersistentObject(KEY_ID,
                        &objectID, sizeof(objectID),
                        flags, transient_key, NULL, 0,
                        &openssl_sign_engine_key);
    if (TEE_SUCCESS != result) {
        DLOG("Failed to create a persistent key: 0x%x", result);
        goto cleanup_2;
    }

    result = TEE_GetObjectBufferAttribute(transient_key, TEE_ATTR_RSA_MODULUS, modulus, (uint32_t*)&mod_length);
    if(TEE_SUCCESS != result) {
        DLOG("get RSA public exponent fail, Ret value is:0x%x\n", result);
        result = FAIL;
        goto cleanup_2;
    }

#if 0
    DLOG("RSA public modulus is:\n");
    TA_Printf(output, outlen
);

    TEE_CloseObject(openssl_sign_engine_key);
#endif

#if 0
    result = TEE_OpenPersistentObject(KEY_ID,
                      &objectID, sizeof(objectID),
                      flags, &openssl_sign_engine_key);
    if (result != TEE_SUCCESS) {
        DLOG("Failed to open persistent key: 0x%x", result);
        goto cleanup_2;
    }

    output = TEE_Malloc(key_size, 0)
    outlen = sizeof(output);

    result = TEE_GetObjectBufferAttribute(openssl_sign_engine_key, TEE_ATTR_RSA_MODULUS, output, &outlen);
    if(TEE_SUCCESS != result) {
        DLOG("get RSA public exponent fail, Ret value is:0x%x\n", result);
        result = FAIL;
        goto cleanup_2;
    }

    DLOG("RSA public exponent is:\n");
    TA_Printf(output, outlen
);
#endif

    return OK;
cleanup_1:
    return FAIL;
cleanup_2:
    TEE_FreeTransientObject(transient_key);
    transient_key = TEE_HANDLE_NULL;
    return FAIL;
}

