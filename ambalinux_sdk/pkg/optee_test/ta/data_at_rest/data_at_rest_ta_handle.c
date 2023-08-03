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

#include "data_at_rest_ta_sha256.h"
#include "data_at_rest_ta_aes_encrypt.h"
#include "data_at_rest_ta_aes_decrypt.h"
#include "data_at_rest_ta_type.h"
#include "data_at_rest_ta_generate_key.h"

#include "data_at_rest_ta_handle.h"


char g_Aes256Iv[] = {0x1DU, 0x44U, 0x05U, 0x2BU, 0x61U, 0x49U, 0x17U, 0xF8U,
    0x58U, 0xE0U, 0x90U, 0x43U, 0x84U, 0xA1U, 0xC1U, 0x75U
};

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

static unsigned char* data_at_rest_ta_strcat(unsigned char* dst, unsigned int dstlength, const unsigned char* src, unsigned srclength)
{
    unsigned

    char* cp = dst;
    unsigned int count = 0;
    while(count != dstlength)
{
    cp++;
    count++;
}
    count = 0;
    while(count != srclength + 1)
{
    *cp++ = *src++;
    count++;
}
    return(dst);
}

static void __set_info(aes_operation_t *op, aes_operation_info_t *info)
{
    op->key_len = 256;
    op->iv = (char *) g_Aes256Iv;
    op->iv_len = 16;

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
        case E_AES_MODE_CBC_CTS:
            op->algorithm_id = TEE_ALG_AES_CTS;
            break;
        default:
            DLOG("wrong mode %d\n", info->mode);
            break;
    }
}

int data_at_rest_ta_gen_device_key(unsigned int paramTypes, TEE_Param params[4])
{
    char* device_key = NULL;
    char* input_data;
    char output[256+1];
    int result;
    unsigned int input_len = 256+16;
    unsigned int output_len = 256;
    unsigned char* uniqueid;
    unsigned int uniqueidlen;
    unsigned char nonce[16];
    unsigned int noncelen = 16;

    UNUSED(paramTypes);

    uniqueid = params[0].memref.buffer;
    uniqueidlen = params[0].memref.size;

    TEE_GenerateRandom(nonce, noncelen);

    input_data = (char *) data_at_rest_ta_strcat(nonce, noncelen, (const unsigned char *) uniqueid, uniqueidlen);

    result = data_at_rest_ta_sha256_oper(input_data, input_len, output, &output_len);
    if(result != OK) {
        DLOG("hash fail, result = %d", result);
        return FAIL;
        }

    device_key = output;

    //TA_Printf(device_key, output_len);

    DLOG("hash done!\n");

    result = data_at_rest_ta_set_aes_key(device_key);
    if(result != OK) {
        DLOG("set aes key fail, result = %d", result);
        return FAIL;
        }

    return OK;
}


int data_at_rest_ta_aes_encrypt(unsigned int paramTypes, TEE_Param params[4])
{
    aes_operation_t op;
    aes_operation_info_t info;
    UNUSED(paramTypes);

    //DLOG("Start to do AES operation\n");

    info.mode = params[0].value.a;
    op.in_buf = params[1].memref.buffer;
    op.data_len = params[3].value.a;
    op.out_buf = params[2].memref.buffer;

     __set_info(&op, &info);

    DLOG("ID: 0x%x\n", op.algorithm_id);

    data_at_rest_ta_aes_encrypt_oper(&op);

    return OK;
}

int data_at_rest_ta_aes_decrypt(unsigned int paramTypes, TEE_Param params[4])
{
    aes_operation_t op;
    aes_operation_info_t info;
    //char test[] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06};
    UNUSED(paramTypes);

    //DLOG("Start to do AES operation\n");

    info.mode = params[0].value.a;
    op.in_buf = params[1].memref.buffer;
    op.data_len = params[3].value.a;
    op.out_buf = params[2].memref.buffer;
    //TEE_MemMove(op.out_buf, test, sizeof(test));

    __set_info(&op, &info);

    DLOG("ID: 0x%x\n", op.algorithm_id);

    data_at_rest_ta_aes_decrypt_oper(&op);

    return OK;
}

int data_at_rest_ta_free_device_key(unsigned int paramTypes, TEE_Param params[4])
{
    UNUSED(paramTypes);
    UNUSED(params);

    char* device_key_id = (char *)"device_key";
    data_at_rest_ta_free_peristent_obj(device_key_id);

    DLOG("free device key success.\n");

    return OK;
}
