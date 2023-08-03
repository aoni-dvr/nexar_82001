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
#include <stdlib.h>
#include "tee_internal_api.h"

#include "openssl_sign_engine_ta_hash.h"
#include "openssl_sign_engine_ta_type.h"
#include "openssl_sign_engine_ta_digi_sign.h"
#include "openssl_sign_engine_ta_gen_key.h"

#include "openssl_sign_engine_ta_handle.h"

int openssl_sign_engine_ta_generate_key(uint32_t paramTypes, TEE_Param params[4])
{
    int key_size;
    uint8_t* modulus;
    int exp;
    int exp_length, mod_length;
    int ret;

    UNUSED(paramTypes);

    key_size = params[0].value.a;
    exp = params[1].value.a;
    exp_length = params[1].value.b;
    modulus = params[2].memref.buffer;
    mod_length = params[2].memref.size;

    //DLOG("key size: %d\n", key_size);
    ret = openssl_sign_engine_ta_generate_key_oper(key_size, exp, exp_length, modulus, mod_length);
    if (ret == OK) {
        DLOG("generate key done!\n");
    }

    return ret;

}

int openssl_sign_engine_ta_digi_sign(uint32_t paramTypes, TEE_Param params[4])
{
    char* input_data = NULL;
    char* signed_data = NULL;
    uint32_t input_len = 0U;
    uint32_t hash_type;

    UNUSED(paramTypes);

    hash_type = params[0].value.a;
    input_data = params[1].memref.buffer;
    input_len = params[1].memref.size;
    signed_data = params[2].memref.buffer;


    openssl_sign_engine_ta_digi_sign_oper(hash_type, input_len, input_data, signed_data);

    return OK;
}

int openssl_sign_engine_ta_verify_sign(uint32_t paramTypes, TEE_Param params[4])
{
    char* input_data = NULL;
    char* signed_data = NULL;
    uint8_t* modulus = NULL;
    uint32_t input_len = 0U;
    uint32_t siglen = 0U;
    uint32_t hash_type;
    uint32_t exp = 0U;
    uint32_t mod_length = 0U;

    UNUSED(paramTypes);

    hash_type = params[0].value.a;
    exp = params[0].value.b;
    input_data = params[1].memref.buffer;
    input_len = params[1].memref.size;
    signed_data = params[2].memref.buffer;
    siglen = params[2].memref.size;
    modulus = params[3].memref.buffer;
    mod_length = params[3].memref.size;

    openssl_sign_engine_ta_verify_sign_oper(hash_type, input_len, input_data, siglen, signed_data, exp, modulus, mod_length);

    return OK;
}

