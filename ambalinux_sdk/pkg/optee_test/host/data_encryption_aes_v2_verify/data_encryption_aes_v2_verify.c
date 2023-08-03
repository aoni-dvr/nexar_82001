/*******************************************************************************
 * data_encryption_aes_v2_verify.c
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

#include "stdlib.h"
#include "stdio.h"
#include "string.h"
#include <unistd.h>

#include "tee_client_api.h"

#include "data_encryption_aes_v2_ta_type.h"

#include "data_encryption_aes_v2_ca.h"

typedef struct {
    const char *vector_name;
    int vector_len;
    int aes_len;
    E_AES_MODE e_aes_mode;
} test_vector_context;

#define MAX_VECTOR_SIZE 300001

#if 0
static void print_memory(const unsigned char *p, int size)
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
#endif

#define VECTOR_NUM 53

static const char* get_aes_mode_string(E_AES_MODE e_aes_mode) {
    const char* p[6] = {"cbc", "ecb", "ctr", "cbc", "cts", "xts"};

    if (e_aes_mode == E_AES_MODE_CBC) {
        return p[0];
    } else if (e_aes_mode == E_AES_MODE_ECB) {
        return p[1];
    } else if (e_aes_mode == E_AES_MODE_CTR) {
        return p[2];
    } else if (e_aes_mode == E_AES_MODE_CBC) {
        return p[3];
    } else if (e_aes_mode == E_AES_MODE_XTS) {
        return p[5];
    } else {
        printf("invalid aes mode.\n");
        return NULL;
    }
}

int main (int argc, char *argv[])
{

    test_vector_context test_vector[] = {
        {"aes_128_ctr_9", 9, 128, E_AES_MODE_CTR}, {"aes_128_ctr_16", 16, 128, E_AES_MODE_CTR}, {"aes_128_ctr_1001", 1001, 128, E_AES_MODE_CTR}, {"aes_128_ctr_30000", 30000, 128, E_AES_MODE_CTR}, {"aes_128_ctr_300001", 300001, 128, E_AES_MODE_CTR},
        {"aes_192_ctr_9", 9, 192, E_AES_MODE_CTR}, {"aes_192_ctr_16", 16, 192, E_AES_MODE_CTR}, {"aes_192_ctr_1001", 1001, 192, E_AES_MODE_CTR}, {"aes_192_ctr_30000", 30000, 192, E_AES_MODE_CTR}, {"aes_192_ctr_300001", 300001, 192, E_AES_MODE_CTR},
        {"aes_256_ctr_9", 9, 256, E_AES_MODE_CTR}, {"aes_256_ctr_16", 16, 256, E_AES_MODE_CTR}, {"aes_256_ctr_1001", 1001, 256, E_AES_MODE_CTR}, {"aes_256_ctr_30000", 30000, 256, E_AES_MODE_CTR}, {"aes_256_ctr_300001", 300001, 256, E_AES_MODE_CTR},
        {"aes_128_cbc_9", 9, 128, E_AES_MODE_CBC}, {"aes_128_cbc_16", 16, 128, E_AES_MODE_CBC}, {"aes_128_cbc_1001", 1001, 128, E_AES_MODE_CBC}, {"aes_128_cbc_30000", 30000, 128, E_AES_MODE_CBC}, {"aes_128_cbc_300001", 300001, 128, E_AES_MODE_CBC},
        {"aes_192_cbc_9", 9, 192, E_AES_MODE_CBC}, {"aes_192_cbc_16", 16, 192, E_AES_MODE_CBC}, {"aes_192_cbc_1001", 1001, 192, E_AES_MODE_CBC}, {"aes_192_cbc_30000", 30000, 192, E_AES_MODE_CBC}, {"aes_192_cbc_300001", 300001, 192, E_AES_MODE_CBC},
        {"aes_256_cbc_9", 9, 256, E_AES_MODE_CBC}, {"aes_256_cbc_16", 16, 256, E_AES_MODE_CBC}, {"aes_256_cbc_1001", 1001, 256, E_AES_MODE_CBC}, {"aes_256_cbc_30000", 30000, 256, E_AES_MODE_CBC}, {"aes_256_cbc_300001", 300001, 256, E_AES_MODE_CBC},
        {"aes_128_ecb_9", 9, 128, E_AES_MODE_ECB}, {"aes_128_ecb_16", 16, 128, E_AES_MODE_ECB}, {"aes_128_ecb_1001", 1001, 128, E_AES_MODE_ECB}, {"aes_128_ecb_30000", 30000, 128, E_AES_MODE_ECB}, {"aes_128_ecb_300001", 300001, 128, E_AES_MODE_ECB},
        {"aes_192_ecb_9", 9, 192, E_AES_MODE_ECB}, {"aes_192_ecb_16", 16, 192, E_AES_MODE_ECB}, {"aes_192_ecb_1001", 1001, 192, E_AES_MODE_ECB}, {"aes_192_ecb_30000", 30000, 192, E_AES_MODE_ECB}, {"aes_192_ecb_300001", 300001, 192, E_AES_MODE_ECB},
        {"aes_256_ecb_9", 9, 256, E_AES_MODE_ECB}, {"aes_256_ecb_16", 16, 256, E_AES_MODE_ECB}, {"aes_256_ecb_1001", 1001, 256, E_AES_MODE_ECB}, {"aes_256_ecb_30000", 30000, 256, E_AES_MODE_ECB}, {"aes_256_ecb_300001", 300001, 256, E_AES_MODE_ECB},
        {"aes_128_xts_16", 16, 128, E_AES_MODE_XTS}, {"aes_128_xts_1001", 1001, 128, E_AES_MODE_XTS}, {"aes_128_xts_30000", 30000, 128, E_AES_MODE_XTS}, {"aes_128_xts_300001", 300001, 128, E_AES_MODE_XTS},
        {"aes_256_xts_16", 16, 256, E_AES_MODE_XTS}, {"aes_256_xts_1001", 1001, 256, E_AES_MODE_XTS}, {"aes_256_xts_30000", 30000, 256, E_AES_MODE_XTS}, {"aes_256_xts_300001", 300001, 256, E_AES_MODE_XTS},
    };

    unsigned int len = 0, out_len = 0, enc_buf_len = 0;
    int ret = 0, i = 0, success_count = 0, fail_count = 0;
    unsigned char *p_in, *p_out;
    unsigned char *verify_enc_buf, *verify_source_buf;
    char *password;
    const char *aes_mode = NULL;
    FILE *fp_source, *fp_enc;
    char source_file_name[256];
    char enc_file_name[256];

    data_encryption_aes_v2_test_context ctx;

    if (argc != 2) {
        printf("usage: data_encryption_aes_v2_verify <password>\n");
        return (-1);
    }

    password = argv[1];
    printf("password for verify: %s\n", password);

    verify_source_buf = (unsigned char* )malloc(MAX_VECTOR_SIZE + 16);
    verify_enc_buf = (unsigned char *)malloc(MAX_VECTOR_SIZE + 16);
    p_out = (unsigned char *)malloc(MAX_VECTOR_SIZE + 16);

    for (i = 0; i < VECTOR_NUM; i++) {
        memset(p_out, 0x0, (MAX_VECTOR_SIZE + 16));

        //get source file and enc file
        sprintf(source_file_name, "%s%s%s", test_vector[i].vector_name, "_", "source");
        sprintf(enc_file_name, "%s%s%s", test_vector[i].vector_name, "_", "enc");
        fp_source = fopen(source_file_name, "r");
        fp_enc = fopen(enc_file_name, "r");
        if (!fp_enc) {
            if (fp_source) {
                printf("cannot find file %s", enc_file_name);
            }
            continue;
        }

        memset(&ctx, 0x0, sizeof(data_encryption_aes_v2_test_context));
        len = test_vector[i].vector_len;
        ctx.e_aes_len = test_vector[i].aes_len;


        //set enc params from vector

        fread(verify_source_buf, 1, len, fp_source);

        fseek(fp_enc, 0L, SEEK_END);
        enc_buf_len = ftell(fp_enc);
        fseek(fp_enc, 0L, SEEK_SET);

        out_len = ((len > (ctx.e_aes_len / 8)) ? len : (ctx.e_aes_len / 8)) + 16;

        fread(verify_enc_buf, 1, enc_buf_len, fp_enc);

        fclose(fp_source);
        fclose(fp_enc);

        p_in = verify_source_buf;

        ctx.e_aes_mode = test_vector[i].e_aes_mode;
        ctx.e_aes_oper = E_AES_OP_ENCRYPT;

        aes_mode = get_aes_mode_string(ctx.e_aes_mode);

        ret = aes_init_password(&ctx, password, strlen(password));
        if (ret != TEEC_SUCCESS) {
            printf("aes init fail, ret 0x%x.\n", ret);
            return (-3);
        }
        ret = aes_update(&ctx, (unsigned char *)p_in, len, (unsigned char *)p_out, &out_len);
        if (ret != TEEC_SUCCESS) {
            printf("aes update fail, ret 0x%x.\n", ret);
            return (-4);
        }
        ret = aes_final(&ctx, (unsigned char *)p_in, len, (unsigned char *)p_out, &out_len);
        if (ret != TEEC_SUCCESS) {
            printf("aes final fail, ret 0x%x.\n", ret);
            return (-5);
        }

        //print_memory((const unsigned char *)p_out, out_len);
        //print_memory(verify_enc_buf, strlen((const char *)verify_enc_buf));

        if (strncmp((char *)verify_enc_buf, (char *)p_out, out_len)) {
            printf("verfiy aes for aes_%s_%d_%d_enc failed\n", aes_mode, ctx.e_aes_len, len);
            fail_count++;
        } else {
            success_count ++;
        }

        memset(&ctx, 0x0, sizeof(data_encryption_aes_v2_test_context));
        memset(p_out, 0x0, out_len);

        //set dec params from vector
        p_in = verify_enc_buf;

        ctx.e_aes_len = test_vector[i].aes_len;
        ctx.e_aes_mode = test_vector[i].e_aes_mode;
        ctx.e_aes_oper = E_AES_OP_DECRYPT;

        aes_mode = get_aes_mode_string(ctx.e_aes_mode);
        if (ret) {
            return (-2);
        }

        ret = aes_init_password(&ctx, password, strlen(password));
        if (ret != TEEC_SUCCESS) {
            printf("aes init fail, ret 0x%x.\n", ret);
            return (-3);
        }

        ret = aes_update(&ctx, (unsigned char *)p_in, enc_buf_len, (unsigned char *)p_out, &out_len);
        if (ret != TEEC_SUCCESS) {
            printf("aes update fail, ret 0x%x.\n", ret);
            return (-4);
        }

        ret = aes_final(&ctx, (unsigned char *)p_in, enc_buf_len, (unsigned char *)p_out, &out_len);
        if (ret != TEEC_SUCCESS) {
            printf("aes final fail, ret 0x%x.\n", ret);
            return (-5);
        }

        if (strncmp((char *)verify_source_buf, (char *)p_out, out_len)) {
            printf("verfiy aes for aes_%s_%d_%d_dec failed\n", aes_mode, ctx.e_aes_len, len);
            fail_count++;
        } else {
            success_count++;
        }

        memset(p_out, 0x0, out_len);
        memset(verify_source_buf, 0x0, (MAX_VECTOR_SIZE + 16));
        memset(verify_enc_buf, 0x0, (MAX_VECTOR_SIZE + 16));

        p_in = NULL;

        printf("test vector aes_%s_%d_%d done.\n", aes_mode, ctx.e_aes_len, len);

    }

    printf("%d vectors have been tested with %d success and %d fail.\n", VECTOR_NUM * 2, success_count, fail_count);

    if(verify_source_buf) {
        verify_source_buf = NULL;
    }

    if(verify_enc_buf) {
        verify_enc_buf = NULL;
    }

    if(p_out) {
        p_out = NULL;
    }

}

