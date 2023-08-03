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
#include <tee_api.h>
#include <tee_api_defines.h>

#include "openssl_sign_engine_ta_digi_sign.h"
#include "openssl_sign_engine_ta_hash.h"
#include "openssl_sign_engine_ta_type.h"

#if 0
static void TA_Printf(uint8_t* buf, uint32_t len)
{
    uint32_t index = 0U;
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

static uint32_t set_tee_algorithm(int hash_type)
{
    uint32_t tee_algorithm;

    switch (hash_type) {
        case TEE_ALG_MD5:
            tee_algorithm = TEE_ALG_RSASSA_PKCS1_V1_5_MD5;
            break;
        case TEE_ALG_SHA224:
            tee_algorithm = TEE_ALG_RSASSA_PKCS1_V1_5_SHA224;
            break;
        case TEE_ALG_SHA256:
            tee_algorithm = TEE_ALG_RSASSA_PKCS1_V1_5_SHA256;
            break;
        case TEE_ALG_SHA384:
            tee_algorithm = TEE_ALG_RSASSA_PKCS1_V1_5_SHA384;
            break;
        case TEE_ALG_SHA512:
            tee_algorithm = TEE_ALG_RSASSA_PKCS1_V1_5_SHA512;
            break;
        default:
            return TEE_ERROR_NOT_SUPPORTED;
            break;
    }
    return tee_algorithm;
}

int openssl_sign_engine_ta_digi_sign_oper(uint32_t hash_type, uint32_t len, char* input, char* output)
{
    TEE_ObjectHandle key = NULL;
    TEE_Result result = FAIL;
    TEE_OperationHandle openssl_sign_handle = NULL;
    char hash[32];
    //uint8_t* modulus;
    uint32_t key_size = 0;
    uint32_t outlen = 32U;
    uint32_t alg = 0;
    uint32_t objectID = 1;
    TEE_ObjectInfo key_info;
    uint32_t RSA_KEY_ID = TEE_STORAGE_PRIVATE;
    uint32_t flags = TEE_DATA_FLAG_ACCESS_READ |
             TEE_DATA_FLAG_ACCESS_WRITE |
             TEE_DATA_FLAG_ACCESS_WRITE_META |
             TEE_DATA_FLAG_SHARE_READ |
             TEE_DATA_FLAG_SHARE_WRITE |
             TEE_DATA_FLAG_OVERWRITE;

    openssl_sign_engine_ta_hash_oper(hash_type, input, len, hash, &outlen);
    //DLOG("The output hash value is\n");
    //TA_Printf(hash, outlen);

    result = TEE_OpenPersistentObject(RSA_KEY_ID, &objectID, sizeof(objectID), flags, &key);
    if (result != TEE_SUCCESS) {
        DLOG("Failed to open persistent key: 0x%x", result);
        goto cleanup_2;
    }

    result = TEE_GetObjectInfo1(key, &key_info);
    if(TEE_SUCCESS != result)
    {
        DLOG("Get object info fail, Ret value is:0x%x\n", result);
        result = FAIL;
        goto cleanup_2;
    }
    key_size = key_info.keySize;

#if 0
    modulus = TEE_Malloc(key_size, 0);

    result = TEE_GetObjectBufferAttribute(key, TEE_ATTR_RSA_MODULUS, modulus, &key_size);
    if(TEE_SUCCESS != result) {
        DLOG("get object buffer fail, Ret value is:0x%x\n", result);
        result = FAIL;
        goto cleanup_2;
    }

    DLOG("RSA modulus is:\n");
    TA_Printf(modulus, key_size
);
#endif

    if(key == TEE_HANDLE_NULL) {
        DLOG("RSA key not exist!\n");
        result = FAIL;
    }

    alg = set_tee_algorithm(hash_type);
    if(hash_type == TEE_ERROR_NOT_SUPPORTED) {
        DLOG("invalid hash type!\n");
        result = FAIL;
        goto cleanup_2;
    }

    result = TEE_AllocateOperation(&openssl_sign_handle, alg, TEE_MODE_SIGN, 2048);
    if(TEE_SUCCESS != result) {
        DLOG("The allocate operate handle fail, the return value is: 0x%x\n", result);
        result = FAIL;
        goto cleanup_2;
    }

    result = TEE_SetOperationKey(openssl_sign_handle, key);
    if(TEE_SUCCESS != result) {
        DLOG("Set operation key fail, return value is:0x%x\n", result);
        result = FAIL;
        goto cleanup_1;
    }

    outlen = key_size / 8;
    result = TEE_AsymmetricSignDigest(openssl_sign_handle, NULL, 0, hash, 32, output, &outlen);
    if(result != TEE_SUCCESS) {
        DLOG("Sign digest fail, the return value is: 0x%x\n", result);
        return result;
    }

    DLOG("digital signature done.\n");

cleanup_1:
    TEE_FreeOperation(openssl_sign_handle);
    openssl_sign_handle = TEE_HANDLE_NULL;
    TEE_CloseObject(key);
cleanup_2:
    return result;

}

int openssl_sign_engine_ta_verify_sign_oper(uint32_t hash_type, uint32_t len, char* input, uint32_t siglen, char* sig,
                                                                            uint32_t exp, uint8_t* modulus, uint32_t mod_length)
{
    TEE_ObjectHandle key = NULL;
    TEE_Result result = FAIL;
    TEE_OperationHandle openssl_sign_handle = NULL;
    char hash[32];
    uint8_t* exp_buf;
    uint32_t exp_length = 0;
    uint32_t hash_len = 32U;
    uint32_t exp_1 = exp;
    //uint32_t key_size = mod_length;
    uint32_t alg = 0;
    TEE_Attribute rsa_attrs[2];

    openssl_sign_engine_ta_hash_oper(hash_type, input, len, hash, &hash_len);
    //DLOG("The output hash value is\n");
    //TA_Printf(hash, hash_len);

    //DLOG("exp: %d\n", exp_1);
    while(exp_1 != 0) {
        exp_1 /= 16;
        exp_length++;
    }
    //DLOG("explen: %d\n", exp_length);

    exp_length = (exp_length + 1) / 2;

    exp_buf = (uint8_t *)&exp;

    //TA_Printf(exp_buf, exp_length);;

    TEE_InitRefAttribute(&rsa_attrs[0], TEE_ATTR_RSA_PUBLIC_EXPONENT, (void*)exp_buf, exp_length);
    TEE_InitRefAttribute(&rsa_attrs[1], TEE_ATTR_RSA_MODULUS, (void*)modulus, mod_length);

    alg = set_tee_algorithm(hash_type);
    if(hash_type == TEE_ERROR_NOT_SUPPORTED) {
        DLOG("invalid hash type!\n");
        result = FAIL;
        goto cleanup_2;
    }

    result = TEE_AllocateTransientObject(TEE_TYPE_RSA_PUBLIC_KEY, 2048, &key);
    if(TEE_SUCCESS != result) {
        DLOG("The allocate transient object fail, the return value is: 0x%x\n", result);
        result = FAIL;
        goto cleanup_1;
    }

    result = TEE_PopulateTransientObject(key, rsa_attrs, 2);
    if(TEE_SUCCESS != result) {
        DLOG("populate transient object fail, the return value is: 0x%x\n", result);
        result = FAIL;
        goto cleanup_1;
    }

    result = TEE_AllocateOperation(&openssl_sign_handle, alg, TEE_MODE_VERIFY, 2048);
    if(TEE_SUCCESS != result) {
        DLOG("The allocate operate handle fail, the return value is: 0x%x\n", result);
        result = FAIL;
        goto cleanup_1;
    }

    result = TEE_SetOperationKey(openssl_sign_handle, key);
    if(TEE_SUCCESS != result) {
        DLOG("set operation key fail, the return value is: 0x%x\n", result);
        result = FAIL;
        goto cleanup_1;
    }
    result = TEE_AsymmetricVerifyDigest(openssl_sign_handle, NULL, 0, hash, hash_len, sig, siglen);
    if(TEE_SUCCESS != result) {
        DLOG("verify signature fail, the return value is: 0x%x\n", result);
        return result;
    }

    DLOG("verify signature success.\n");

cleanup_1:
    TEE_FreeOperation(openssl_sign_handle);
    openssl_sign_handle = TEE_HANDLE_NULL;
    TEE_CloseObject(key);
cleanup_2:
    return result;

}

