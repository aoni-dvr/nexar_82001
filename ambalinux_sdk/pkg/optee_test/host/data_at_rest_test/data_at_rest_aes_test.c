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

#include "stdlib.h"
#include "stdio.h"

#include "tee_client_api.h"

#include "data_at_rest_ta_type.h"

#include "data_at_rest_ca_aes_encrypt.h"
#include "data_at_rest_ca_aes_decrypt.h"

#include "data_at_rest_aes_test.h"

static void print_memory(char *p, int size)
{
    while (size > 7) {
        printf("%02x %02x %02x %02x %02x %02x %02x %02x\n",
               p[0], p[1], p[2], p[3],
               p[4], p[5], p[6], p[7]);
        p += 8;
        size -= 8;
    }

    while (size) {
        printf("%02x ", p[0]);
        p ++;
        size --;
    }

    printf("\n");
}


static void aes_encrypt_test(E_AES_MODE aes_mode, char *in_buf, char *out_buf, unsigned int len)
{
    TEEC_Result result;

    printf("input:\n");
    print_memory(in_buf, len);
    result = aes_encrypt(in_buf, len, aes_mode,
                         out_buf);

    printf("result 0x%0x, output:\n", result);
    print_memory(out_buf, len);
}

static void aes_decrypt_test(E_AES_MODE aes_mode, char *in_buf, char *out_buf, unsigned int len)
{
    TEEC_Result result;

    printf("input:\n");
    print_memory(in_buf, len);
    result = aes_decrypt(in_buf, len, aes_mode,
                         out_buf);

    printf("result 0x%0x, output:\n", result);
    print_memory(out_buf, len);
}


int data_at_rest_aes_test(void)
{
    int len = 0;
    char *p_in = NULL;

    char cbc_in[] = {
        0xc9, 0x68, 0x46, 0x5f, 0xc2, 0xdb, 0x78, 0x62, 0xf1, 0xe0, 0x50, 0xc1, 0x75, 0xfe, 0x9f, 0x24,
        0x28, 0xf2, 0x1a, 0xc3, 0xe4, 0x77, 0xa5, 0x7f, 0xcb, 0xe0, 0x48, 0xfe, 0xb3, 0x4e, 0xba, 0x51,
    };
    char ecb_in[] = {
        0x3c, 0x48, 0xeb, 0x65, 0x3e, 0xaf, 0xaf, 0x56, 0x57, 0xfa, 0xbb, 0x6f, 0x0b, 0x0b, 0x65, 0x3c,
        0x11, 0xf3, 0x5a, 0xe3, 0x58, 0xc9, 0xcc, 0xe1, 0xad, 0x0e, 0x16, 0x0e, 0x84, 0x07, 0xf9, 0x2b,
        0x04, 0x57, 0x18, 0xa3, 0xf1, 0x4f, 0x41, 0xf8, 0xf2, 0xe6, 0xfa, 0x89, 0x7c, 0x94, 0x73, 0x5f,
        0xbb, 0x48, 0x4b, 0xba, 0xd5, 0x39, 0x6e, 0xc2, 0x5c, 0xd1, 0x29, 0xf8, 0x42, 0x10, 0xb0, 0x6f,
    };
    char ctr_in[] = {
        0x6c, 0xbc, 0xbe, 0xde, 0x1b, 0xc1, 0xb9, 0x2e, 0x23, 0x00, 0x5a, 0x8d, 0x2f, 0x8b, 0x7b, 0x83,
        0x80, 0x57, 0x97, 0x88, 0xaf, 0x8c, 0x60, 0x16, 0x37, 0xf5, 0xa0, 0x3a, 0xaf, 0x1c, 0x29, 0x6a,
        0x0c, 0xbb, 0x93, 0x15, 0x45, 0x51, 0xb5, 0x8e, 0x27, 0x86, 0x18, 0x7f, 0x4a, 0x9c, 0x0b, 0x21,
        0xe3, 0x37, 0xff, 0x08, 0x7b, 0xf1, 0x8d, 0xc5, 0xd5, 0x0d, 0x8c, 0xe6, 0xfe, 0x61, 0xfb, 0xc1,
        0xb4, 0x64, 0x89, 0xb1, 0xfd, 0x7c, 0x0c,
    };
    char cbc_cts_in[] = {
        0xc9, 0x68, 0x46, 0x5f, 0xc2, 0xdb, 0x78, 0x62, 0xf1, 0xe0, 0x50, 0xc1, 0x75, 0xfe, 0x9f, 0x24,
        0xac, 0x52, 0x9a, 0x99, 0xb2, 0x1d, 0x6b, 0x9c, 0x03, 0x23, 0xcc, 0xe6, 0x8e, 0x56, 0x79, 0x4f,
        0x99, 0xce, 0xf6, 0x8b, 0x5e, 0xf6, 0x45, 0x15, 0x55, 0x7b, 0x4d, 0x5e, 0x2e, 0xe9, 0x9d, 0xbf,
        0xb0, 0xcb, 0x10, 0x7c, 0xc6, 0x22, 0x6a, 0x78, 0x84, 0x9f, 0xf3, 0xf4, 0xaa, 0x3a, 0x6b, 0x45,
        0x22, 0xe1, 0x36, 0x1c, 0xf3, 0xe5, 0x6f,
    };

    char enc_out[256] = {0};
    char enc_dec_out[256] = {0};

    printf("AES ECB TEST\n");
    p_in = ecb_in;
    len = sizeof(ecb_in);
    aes_encrypt_test(E_AES_MODE_ECB, p_in, enc_out, len);
    aes_decrypt_test(E_AES_MODE_ECB, enc_out, enc_dec_out, len);

    printf("AES CTR TEST\n");
    p_in = ctr_in;
    len = sizeof(ctr_in);
    aes_encrypt_test(E_AES_MODE_CTR, p_in, enc_out, len);
    aes_decrypt_test(E_AES_MODE_CTR, enc_out, enc_dec_out, len);

    printf("AES CBC TEST\n");
    p_in = cbc_in;
    len = sizeof(cbc_in);
    aes_encrypt_test(E_AES_MODE_CBC, p_in, enc_out, len);
    aes_decrypt_test(E_AES_MODE_CBC, enc_out, enc_dec_out, len);

    printf("AES CBC-CTS TEST\n");
    p_in = cbc_cts_in;
    len = sizeof(cbc_cts_in);
    aes_encrypt_test(E_AES_MODE_CBC_CTS, p_in, enc_out, len);
    aes_decrypt_test(E_AES_MODE_CBC_CTS, enc_out, enc_dec_out, len);

    return 0;
}


