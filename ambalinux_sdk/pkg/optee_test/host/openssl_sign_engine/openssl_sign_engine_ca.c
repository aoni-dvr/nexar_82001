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

#include <openssl/engine.h>
#include <openssl/err.h>
#include <openssl/evp.h>
#include <openssl/rsa.h>

#include "tee_client_api.h"
#include "tee_api_defines.h"
#include "openssl_sign_engine_ca.h"
#include "openssl_sign_engine_ca_type.h"

#ifndef __unused
#define __unused    __attribute__((unused))
#endif

static const char *engine_tee_sign_id = "tee_sign";
static const char *engine_tee_sign_name = "tee_sign engine support";

#if 0
static void Print_Buffer(uint8_t* buf, uint32_t len)
{
    uint32_t index = 0U;
    for(index = 0U; index < len; index++) {
        if(index < 15U) {
        }
        else if(0U == index%16U) {
            printf("\n");
        }
        else {
        }

        printf("0x%02x, ", (buf[index] & 0xFFU));

    }
    printf("\n\n");
}
#endif

static int tee_rsa_sign(int type, const uint8_t *m, uint32_t m_length,
                     uint8_t *sigret, uint32_t *siglen, const RSA *rsa);

static int tee_rsa_verify(int dtype, const uint8_t *m,
                       uint32_t m_length, const uint8_t *sigbuf,
                       uint32_t siglen, const RSA *rsa);

static int tee_rsa_gen_key(RSA *rsa, int bits, BIGNUM *e, BN_GENCB *cb);

static RSA_METHOD tee_rsa = {
    "TEE_RSA",
    0,                          /* pub_enc */
    0,                          /* pub_dec */
    0,                          /* priv_enc */
    0,                          /* priv_dec */
    0,                          /* rsa_mod_exp */
    0,                          /* bn_mod_exp */
    0,                          /* init */
    0,                          /* finish */
    RSA_FLAG_SIGN_VER,          /* flags */
    NULL,                       /* app_data */
    tee_rsa_sign,               /* rsa_sign */
    tee_rsa_verify,             /* rsa_verify */
    tee_rsa_gen_key             /*rsa ley_gen */
};

static int tee_data_to_buffer(uint8_t* tee_data, int data_len, uint8_t* out)
{
    const char Hex[] = "0123456789ABCDEF";

    int i = 0;
    for(i = 0; i < data_len; i++) {
        *(out++) = Hex[*tee_data >> 4];
        *(out++) = Hex[*tee_data & 0x0f];
        tee_data++;
    }

    return 0;
}

static int BN_to_tee_data(BIGNUM *bn, char* out)
{
    const char Hex[] = "0123456789ABCDEF";

    int i = 0, j = 0;
    int bytes;
    char* buf = NULL;
    char* temp = NULL;

    bytes = BN_num_bytes(bn);
    buf = BN_bn2hex(bn);
    //printf("%s\n", buf);
    temp = buf;
    for(i = 0; i < bytes * 2; i++) {
        for(j = 0; j < 16; j++) {
            if(Hex[j] == *temp) {
                *(temp++) = j;
                break;
            }
        }
    }

    for(i = 0; i < bytes; i++) {
        *(out) = ((*buf++) << 4);
        *(out++) += *(buf);
        buf++;
    }

    return 0;
}

static uint32_t set_hash_type(int openssl_type)
{
    uint32_t tee_type;

    switch (openssl_type) {
        case NID_md5:
            tee_type = TEE_ALG_MD5;
            break;
        case NID_sha224:
            tee_type = TEE_ALG_SHA224;
            break;
        case NID_sha256:
            tee_type = TEE_ALG_SHA256;
            break;
        case NID_sha384:
            tee_type = TEE_ALG_SHA384;
            break;
        case NID_sha512:
            tee_type = TEE_ALG_SHA512;
            break;
        default:
            printf("invalid hash type!\n");
            return TEE_ERROR_NOT_SUPPORTED;
            break;
    }

    return tee_type;
}

static void ERR_load_tee_sign_strings(void)
{
    return;
}

static int tee_sign_init(__unused ENGINE* e)
{
    printf("tee_sign engine init done.\n");
    return 1;
}

static int tee_sign_finish(__unused ENGINE* e)
{
    return 1;
}

static int tee_sign_destroy(__unused ENGINE* e)
{
    return 1;
}

int openssl_sign_engine_ca_generate_key(int key_size, int exp, int exp_length, uint8_t *modulus, int mod_length)
{
    TEEC_Context context;
    TEEC_Session session;
    TEEC_Result result;
    TEEC_Operation operation;
    uint32_t origin;
    TEEC_UUID svc_id = OPENSSL_SIGN_ENGINE_UUID;

    result = TEEC_InitializeContext(NULL, &context);
    if (result != TEEC_SUCCESS) {
        printf("TEEC_InitializeContext fail, result=0x%x\n", result);
        return result;
    }

    result = TEEC_OpenSession(&context, &session, &svc_id,
        TEEC_LOGIN_PUBLIC, NULL, NULL, &origin);
    if (result != TEEC_SUCCESS) {
        printf("TEEC_OpenSession failed, result=0x%x, origin=0x%x\n", result, origin);
        goto cleanup_1;
    }

    memset(&operation, 0x0, sizeof(operation));

    operation.started = 1;

    operation.paramTypes = TEEC_PARAM_TYPES(TEEC_VALUE_INPUT,
                     TEEC_VALUE_INPUT, TEEC_MEMREF_TEMP_OUTPUT, TEEC_NONE);
    operation.params[0].value.a = key_size;
    operation.params[1].value.a = exp;
    operation.params[1].value.b = exp_length;
    operation.params[2].tmpref.buffer = modulus;
    operation.params[2].tmpref.size = mod_length;

    result = TEEC_InvokeCommand(&session, CMD_GENERATE_KEY, &operation, &origin);

    if (result != TEEC_SUCCESS) {
        printf("generate_key TEEC_InvokeCommand failed, result=0x%x, origin=0x%x\n", result, origin);
        goto cleanup_2;
    }

cleanup_2:
    TEEC_CloseSession(&session);
cleanup_1:
    TEEC_FinalizeContext(&context);

    return result;

}

static int openssl_sign_engine_ca_digi_sign(int type, uint8_t *m, uint32_t m_length,
                     uint8_t *sigret, uint32_t siglen)
{
    TEEC_Context context;
    TEEC_Session session;
    TEEC_Operation operation;
    TEEC_Result result;
    TEEC_UUID svc_id = OPENSSL_SIGN_ENGINE_UUID;
    uint32_t origin;

    uint32_t tee_hash_type;

    tee_hash_type = set_hash_type(type);
    if(tee_hash_type == TEE_ERROR_NOT_SUPPORTED) {
        return tee_hash_type;
    }

    result = TEEC_InitializeContext(NULL, &context);
    if (result != TEEC_SUCCESS) {
        printf("TEEC_InitializeContext fail, result=0x%x\n", result);
        return result;
    }

    result = TEEC_OpenSession(&context, &session, &svc_id,
        TEEC_LOGIN_PUBLIC, NULL, NULL, &origin);
    if (result != TEEC_SUCCESS) {
        printf("TEEC_OpenSession failed, result=0x%x, origin=0x%x\n", result, origin);
        goto cleanup_1;
    }

    memset(&operation, 0x0, sizeof(operation));
    operation.started = 1;
    operation.paramTypes = TEEC_PARAM_TYPES(TEEC_VALUE_INPUT, TEEC_MEMREF_TEMP_INPUT,
        TEEC_MEMREF_TEMP_OUTPUT, TEEC_NONE);
    operation.params[0].value.a = tee_hash_type;
    operation.params[1].tmpref.buffer = m;
    operation.params[1].tmpref.size = m_length;

    operation.params[2].tmpref.buffer = sigret;
    operation.params[2].tmpref.size = siglen;


    result = TEEC_InvokeCommand(&session, CMD_DIGI_SIGN, &operation, &origin);
    if (result != TEEC_SUCCESS) {
        printf("TEEC_InvokeCommand fail, result=0x%x, origin=0x%x\n", result, origin);
        goto cleanup_2;
    }

cleanup_2:
    TEEC_CloseSession(&session);
cleanup_1:
    TEEC_FinalizeContext(&context);

    return result;
}

static int openssl_sign_engine_ca_verify_sign(int type, uint8_t *m, uint32_t m_length,
                     uint8_t *sigret, uint32_t siglen, uint32_t exp, uint8_t *modulus, uint32_t mod_length)
{
    TEEC_Context context;
    TEEC_Session session;
    TEEC_Operation operation;
    TEEC_Result result;
    TEEC_UUID svc_id = OPENSSL_SIGN_ENGINE_UUID;
    uint32_t origin;

    uint32_t tee_hash_type;

    tee_hash_type = set_hash_type(type);
    if(tee_hash_type == TEE_ERROR_NOT_SUPPORTED) {
        return tee_hash_type;
    }

    result = TEEC_InitializeContext(NULL, &context);
    if (result != TEEC_SUCCESS) {
        printf("TEEC_InitializeContext fail, result=0x%x\n", result);
        return result;
    }

    result = TEEC_OpenSession(&context, &session, &svc_id,
        TEEC_LOGIN_PUBLIC, NULL, NULL, &origin);
    if (result != TEEC_SUCCESS) {
        printf("TEEC_OpenSession failed, result=0x%x, origin=0x%x\n", result, origin);
        goto cleanup_1;
    }

    memset(&operation, 0x0, sizeof(operation));
    operation.started = 1;
    operation.paramTypes = TEEC_PARAM_TYPES(TEEC_VALUE_INPUT, TEEC_MEMREF_TEMP_INPUT,
        TEEC_MEMREF_TEMP_INPUT, TEEC_MEMREF_TEMP_INPUT);
    operation.params[0].value.a = tee_hash_type;
    operation.params[0].value.b = exp;
    operation.params[1].tmpref.buffer = m;
    operation.params[1].tmpref.size = m_length;

    operation.params[2].tmpref.buffer = sigret;
    operation.params[2].tmpref.size = siglen;
    operation.params[3].tmpref.buffer = modulus;
    operation.params[3].tmpref.size = mod_length;

    result = TEEC_InvokeCommand(&session, CMD_VERIFY_SIGN, &operation, &origin);
    if (result != TEEC_SUCCESS) {
        printf("TEEC_InvokeCommand fail, result=0x%x, origin=0x%x\n", result, origin);
        goto cleanup_2;
    }

cleanup_2:
    TEEC_CloseSession(&session);
cleanup_1:
    TEEC_FinalizeContext(&context);

    return result;
}

static int tee_rsa_sign(int type, const uint8_t *m, uint32_t m_length,
                     uint8_t *sigret, uint32_t *siglen, __unused const RSA* rsa)
{
    printf("tee_rsa_sign start.\n");
    return openssl_sign_engine_ca_digi_sign(type, (uint8_t *)m, m_length, sigret, *siglen);
}

static int tee_rsa_verify(int dtype, const uint8_t *m,
                       uint32_t m_length, const uint8_t *sigbuf,
                       uint32_t siglen, const RSA *rsa)
{
    int exp = 0;
    char* modulus = NULL;
    int mod_length = 0;

    printf("tee_rsa_verify start.\n");

    mod_length = BN_num_bytes(rsa->n);
    modulus = (char*)malloc(sizeof(char*) * mod_length);
    BN_to_tee_data(rsa->n, modulus);
    //Print_Buffer(modulus, mod_length);

    exp = atoi(BN_bn2dec(rsa->e));
    //printf("exp:%d\n", exp);

    return openssl_sign_engine_ca_verify_sign(dtype, (uint8_t *)m, m_length, (uint8_t *)sigbuf, siglen, (uint32_t)exp, (uint8_t *)modulus, mod_length);
}

static int tee_rsa_gen_key(RSA* rsa, int bits, BIGNUM* e, __unused BN_GENCB* cb)
{
    char* exp_buf;
    int exp = 0;
    int e_length = 0;
    uint8_t* modulus;
    uint8_t* mod_buf;
    int mod_length = 0;
    BIGNUM* n;
    int ret;

    modulus = (uint8_t*)malloc(sizeof(uint8_t*) * bits);
    mod_length = bits;
    exp_buf = BN_bn2dec(e);
    e_length  = strlen(exp_buf);
    exp = atoi(exp_buf);
    //printf("exp:%d\n", exp);

    ret = openssl_sign_engine_ca_generate_key(bits, exp, e_length, modulus, mod_length);

    if(ret == TEE_SUCCESS) {
        n = BN_new();
        mod_buf = (uint8_t*)malloc(sizeof(uint8_t*) * mod_length / 4);
        tee_data_to_buffer(modulus, mod_length / 8, mod_buf);
        BN_hex2bn(&n, (char *)mod_buf);
        rsa->e = e;
        rsa->n = n;
    }

    return !ret;
}

static int bind_engine(ENGINE *e)
{

    if(!ENGINE_set_id(e, engine_tee_sign_id)
        || !ENGINE_set_name(e, engine_tee_sign_name)
        || !ENGINE_set_RSA(e, &tee_rsa)
        //|| !ENGINE_set_ciphers(e, tee_sign_ciphers)
        //|| !ENGINE_set_digests(e, tee_sign_digests)
        //|| !ENGINE_set_pkey_meths(e, test_pkey_meths)
        || !ENGINE_set_destroy_function(e, tee_sign_destroy)
        || !ENGINE_set_init_function(e, tee_sign_init)
        || !ENGINE_set_finish_function(e, tee_sign_finish)
        /* || !ENGINE_set_ctrl_function(e, tee_sign_ctrl) */
        /* || !ENGINE_set_cmd_defns(e, tee_sign_cmd_defns) */)
    return 0;

    /* Ensure the tee_sign error handling is set up */
    ERR_load_tee_sign_strings();
    return 1;

}

#ifdef ENGINE_DYNAMIC_SUPPORT
static int bind_helper(ENGINE *e, const char *id) {
    if(id && (strcmp(id, engine_tee_sign_id) != 0)) {
        return 0;
    }
    if(!bind_engine(e)) {
        return 0;
    }
    return 1;
}

IMPLEMENT_DYNAMIC_CHECK_FN()
IMPLEMENT_DYNAMIC_BIND_FN(bind_helper)
#else
static ENGINE *engine_tee_sign(void)
{
    ENGINE *ret = ENGINE_new();
    if(!ret) {
        return NULL;
    }
    if(!bind_engine(ret)) {
        ENGINE_free(ret);
        return NULL;
    }

    return ret;
}

void ENGINE_load_tee_sign(void)
{
    ENGINE *engine = engine_tee_sign();
    if(!engine) {
        return;
    }
    ENGINE_add(engine);
    ENGINE_free(engine);
    ERR_clear_error();
}
#endif
