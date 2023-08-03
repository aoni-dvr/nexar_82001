/*******************************************************************************
 * data_encryption_aes_v2_test.c
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

#include "tee_client_api.h"

#include "data_encryption_aes_v2_ta_type.h"

#include "data_encryption_aes_v2_ca.h"

#if 0
static void print_memory(unsigned char *p, int size)
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

static void __print_data_encryption_aes_v2_test_helps(void)
{
    printf("data_enctyption_aes_v2_test's options\n");
    printf("\t'--in [%%s]': input file for encryption/decryption\n");
    printf("\t'--out [%%s]': output encrypted/decrypted file\n");
    printf("\t'--mode: supported aes mode :  CBC, ECB, CTR or XTS\n");
    printf("\t'--op': enc or dec\n");
    printf("\t'--len': 128, 192 or 256\n");
}

static E_AES_OPERATION check_aes_oper(data_encryption_aes_v2_test_context *ctx)
{
    if (!strcmp(ctx->aes_oper, "enc")) {
        return E_AES_OP_ENCRYPT;
    } else if (!strcmp(ctx->aes_oper, "dec")) {
        return E_AES_OP_DECRYPT;
    } else {
        printf("error : invalid aes oper.\n");
        return E_AES_OP_INVALID;
    }

}

static E_AES_MODE check_aes_mode(data_encryption_aes_v2_test_context *ctx)
{

    if (!strcmp(ctx->aes_mode, "CBC")) {
        return E_AES_MODE_CBC;
    } else if (!strcmp(ctx->aes_mode, "ECB")) {
        return E_AES_MODE_ECB;
    } else if (!strcmp(ctx->aes_mode, "CTR")) {
        return E_AES_MODE_CTR;
    } else if (!strcmp(ctx->aes_mode, "XTS")) {
        return E_AES_MODE_XTS;
    } else {
        printf("error : invalid aes mode.\n");
        return E_AES_MODE_INVALID;
    }

    return 0;

}

static unsigned int check_aes_len(data_encryption_aes_v2_test_context *ctx)
{
    if (!strcmp(ctx->aes_len, "128")) {
        return 128;
    } else if (!strcmp(ctx->aes_len, "192")) {
        return 192;
    } else if (!strcmp(ctx->aes_len, "256")) {
        return 256;
    } else {
        printf("error : invalid aes length.\n");
        return (-1);
    }

}

static int __init_data_encryption_aes_v2_test_params(
    data_encryption_aes_v2_test_context *ctx,
    int argc, char *argv[])
{
    int i = 0;

    for (i = 1; i < argc; i++) {
        if (!strcmp("--in", argv[i])) {
            if ((i + 1) < argc) {
                printf("[input argument] --in, %s.\n", argv[i + 1]);
                ctx->input_file_name = argv[i + 1];
                i ++;
            } else {
                printf("error: [input argument] --in should follow input file name\n");
                return (-1);
            }
        } else if (!strcmp("--out", argv[i])) {
            if ((i + 1) < argc) {
                printf("[input argument] --out, %s.\n", argv[i + 1]);
                ctx->output_file_name = argv[i + 1];
                i ++;
            } else {
                printf("error: [input argument] --out should follow output file name\n");
                return (-1);
            }
        } else if (!strcmp("--mode", argv[i])) {
            if ((i + 1) < argc) {
                printf("[input argument] --mode, %s.\n", argv[i + 1]);
                ctx->aes_mode = argv[i + 1];
                ctx->e_aes_mode = check_aes_mode(ctx);
                if (ctx->e_aes_mode == E_AES_MODE_INVALID) {
                    return (-1);
                }
                i ++;
            } else {
                printf("error: [input argument] --mode should follow aes mode: CBC, ECB, CTR or CBC_CTS\n");
                return (-1);
            }
        } else if (!strcmp("--op", argv[i])) {
            if ((i + 1) < argc) {
                printf("[input argument] --op, %s.\n", argv[i + 1]);
                ctx->aes_oper = argv[i + 1];
                ctx->e_aes_oper = check_aes_oper(ctx);
                if (ctx->e_aes_oper == E_AES_OP_INVALID) {
                    return (-1);
                }
                i ++;
            } else {
                printf("error: [input argument] --op should follow aes oper: enc or dec\n");
                return (-1);
            }
        } else if (!strcmp("--len", argv[i])) {
            if ((i + 1) < argc) {
                printf("[input argument] --len, %s.\n", argv[i + 1]);
                ctx->aes_len = argv[i + 1];
                ctx->e_aes_len = check_aes_len(ctx);
                if (ctx->e_aes_len == (unsigned int)(-1)) {
                    return (-1);
                }
                i ++;
            } else {
                printf("error: [input argument] --len should follow aes length: 128, 192 or 256\n");
                return (-1);
            }
        } else {
            printf("error: NOT processed option(%s).\n", argv[i]);
            return (-1);
        }
    }

    return 0;
}

int main (int argc, char *argv[])
{
    unsigned int len = 0, out_len = 0;
    int ret = 0;
    char *p_in = NULL;

    FILE *input_file;
    FILE *output_file;

    data_encryption_aes_v2_test_context ctx;

    memset(&ctx, 0x0, sizeof(data_encryption_aes_v2_test_context));

    char *out_buf;
    char password[256] = {0};


    if (argc > 1) {
        ret = __init_data_encryption_aes_v2_test_params(&ctx, argc, argv);
        if (0 > ret) {
            printf("parameters check fail, ret %d\n", ret);
            __print_data_encryption_aes_v2_test_helps();
            return (-1);
        }
    } else {
        __print_data_encryption_aes_v2_test_helps();
        return (1);
    }

    if ((!ctx.aes_len) || (!ctx.aes_mode) || (!ctx.aes_oper) || (!ctx.input_file_name) || (!ctx.output_file_name)) {
        printf("bad parameters.\n");
        __print_data_encryption_aes_v2_test_helps();
        return(-2);
    }

    if ((input_file = fopen(ctx.input_file_name, "rb")) == NULL) {
        printf("open file %s failed.\n", ctx.input_file_name);
        return (-3);
    }

    fseek(input_file, 0L, SEEK_END);
    len = ftell(input_file);
    fseek(input_file, 0L, SEEK_SET);
    p_in = (char *)malloc(sizeof(char) * (len + 16));
    fread(p_in, len, 1, input_file);
    fclose(input_file);
    out_len = ((len > (ctx.e_aes_len / 8)) ? len : (ctx.e_aes_len / 8)) + 16;
    out_buf = (char *)malloc(sizeof(char) * out_len);

    printf("input password for aes: \n");
    do {
        fgets(password, sizeof(password), stdin);
    } while ((password[0] > 127) || (password[0] < 33));

    ret = aes_init_password(&ctx, password, strlen(password) - 1);
    if (ret != TEEC_SUCCESS) {
        printf("aes init fail, ret 0x%x.\n", ret);
        return -1;
    }
    ret = aes_update(&ctx, (unsigned char *)p_in, len, (unsigned char *)out_buf, &out_len);
    if (ret != TEEC_SUCCESS) {
        printf("aes update fail, ret 0x%x.\n", ret);
        return (-4);
    }

    ret = aes_final(&ctx, (unsigned char *)p_in, len, (unsigned char *)out_buf, &out_len);
    if (ret != TEEC_SUCCESS) {
        printf("aes final fail, ret 0x%x.\n", ret);
        return (-5);
    }

    if ((output_file = fopen(ctx.output_file_name, "wb")) == NULL) {
        printf("open file %s failed.\n", ctx.output_file_name);
        return (-3);
    }

    ret = fwrite(out_buf, 1, out_len, output_file);
    if ((unsigned int)ret != out_len) {
        printf("write out file %s failed.\n", ctx.output_file_name);
        fclose(output_file);
        return (-6);
     }

    printf("aes operation done.\n");

    fclose(output_file);
    return 0;
}
