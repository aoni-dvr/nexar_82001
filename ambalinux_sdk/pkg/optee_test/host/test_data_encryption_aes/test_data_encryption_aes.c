
#include "stdlib.h"
#include "stdio.h"
#include "string.h"

#include "tee_client_api.h"

#include "data_encryption_aes_ta_type.h"

#include "data_encryption_aes_ca.h"

typedef struct {
    char *input_file_name;
    char *output_file_name;

    const char *aes_mode;
    const char *aes_oper;
    const char *aes_len;

    E_AES_MODE e_aes_mode;
    E_AES_OPERATION e_aes_oper;
    unsigned int e_aes_len;
} test_data_encryption_aes_context;

static void __print_test_data_encryption_aes_helps(void)
{
    printf("test_data_encryption_aes's options\n");
    printf("\t'--inputfile [%%s]': input file for encryption/decryption\n");
    printf("\t'--outputfile [%%s]': output encrypted/decrypted file\n");
    printf("\t'--aesmode: supported aes mode :  CBC, ECB, CTR or CTS\n");
    printf("\t'--aesoper': enc or dec\n");
    printf("\t'--len': 128, 192 or 256\n");
}

static E_AES_OPERATION check_aes_oper(test_data_encryption_aes_context *ctx)
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

static E_AES_MODE check_aes_mode(test_data_encryption_aes_context *ctx)
{

    if (!strcmp(ctx->aes_mode, "CBC")) {
        return E_AES_MODE_CBC;
    } else if (!strcmp(ctx->aes_mode, "ECB")) {
        return E_AES_MODE_ECB;
    } else if (!strcmp(ctx->aes_mode, "CTR")) {
        return E_AES_MODE_CTR;
    } else if (!strcmp(ctx->aes_mode, "CTS")) {
        return E_AES_MODE_CTS;
    } else {
        printf("error : invalid aes mode.\n");
        return E_AES_MODE_INVALID;
    }

    return 0;

}

static unsigned int check_aes_len(test_data_encryption_aes_context *ctx)
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

static int __init_test_data_encryption_aes_params(
    test_data_encryption_aes_context *ctx,
    int argc, char *argv[])
{
    int i = 0;

    for (i = 1; i < argc; i++) {
        if (!strcmp("--inputfile", argv[i])) {
            if ((i + 1) < argc) {
                printf("[input argument] --inputfile, %s.\n", argv[i + 1]);
                ctx->input_file_name = argv[i + 1];
                i ++;
            } else {
                printf("error: [input argument] --inputfile should follow input file name\n");
                return (-1);
            }
        } else if (!strcmp("--outputfile", argv[i])) {
            if ((i + 1) < argc) {
                printf("[input argument] --outputfile, %s.\n", argv[i + 1]);
                ctx->output_file_name = argv[i + 1];
                i ++;
            } else {
                printf("error: [input argument] --outputfile should follow output file name\n");
                return (-1);
            }
        } else if (!strcmp("--aesmode", argv[i])) {
            if ((i + 1) < argc) {
                printf("[input argument] --aesmode, %s.\n", argv[i + 1]);
                ctx->aes_mode = argv[i + 1];
                ctx->e_aes_mode = check_aes_mode(ctx);
                if (ctx->e_aes_mode == E_AES_MODE_INVALID) {
                    return (-1);
                }
                i ++;
            } else {
                printf("error: [input argument] --aesmode should follow aes mode: CBC, ECB, CTR or CBC_CTS\n");
                return (-1);
            }
        } else if (!strcmp("--aesoper", argv[i])) {
            if ((i + 1) < argc) {
                printf("[input argument] --aesoper, %s.\n", argv[i + 1]);
                ctx->aes_oper = argv[i + 1];
                ctx->e_aes_oper = check_aes_oper(ctx);
                if (ctx->e_aes_oper == E_AES_OP_INVALID) {
                    return (-1);
                }
                i ++;
            } else {
                printf("error: [input argument] --aesoper should follow aes oper: enc or dec\n");
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

int main(int argc, char *argv[])
{
    unsigned int len = 0;
    int ret = 0;
    char *p_in = NULL;

    FILE *input_file;
    FILE *output_file;

    test_data_encryption_aes_context ctx;

    char *out_buf;
    char password[256] = {0};

    char *p;
    int i, q;

    if (argc > 1) {
        ret = __init_test_data_encryption_aes_params(&ctx, argc, argv);
        if (0 > ret) {
            printf("parameters check fail, ret %d\n", ret);
            __print_test_data_encryption_aes_helps();
            return (-1);
        }
    } else {
        __print_test_data_encryption_aes_helps();
        return (1);
    }

    if ((!ctx.aes_len) || (!ctx.aes_mode) || (!ctx.aes_oper) || (!ctx.input_file_name) || (!ctx.output_file_name)) {
        printf("bad parameters.\n");
        return(-1);
    }

    if ((input_file = fopen(ctx.input_file_name, "rb")) == NULL) {
        printf("open file %s failed.\n", ctx.input_file_name);
        return -1;
    }

    fseek(input_file, 0L, SEEK_END);
    len = ftell(input_file);
    fseek(input_file, 0L, SEEK_SET);
    p_in = (char *)malloc(sizeof(char) * (len + 16));
    fread(p_in, len, 1, input_file);
    fclose(input_file);
    out_buf = (char *)malloc(sizeof(char) * ((len > ctx.e_aes_len) ? len : ctx.e_aes_len) + 16);

    printf("input password for aes: \n");
    do {
        fgets(password, sizeof(password), stdin);
    } while ((password[0] > 127) || (password[0] < 33));

    if (ctx.e_aes_oper == E_AES_OP_ENCRYPT) {
        if ((ctx.e_aes_mode == E_AES_MODE_CBC) || (ctx.e_aes_mode == E_AES_MODE_ECB)) {   //padding data
            p = p_in + len;
            q = len % 16;
            if (q == 0) {
                for (i = 0; i < 16; i++) {
                    *p = 16;
                    p++;
                }
                *p = '\0';
                len = len + 16;
            } else {
                for (i = 0; i < (16 - q) ; i++) {
                    *p =  16 - q;
                    p++;
                }
                *p = '\0';
                len = len + 16 - q;
            }
        } else if (ctx.e_aes_mode == E_AES_MODE_CTS) {
            p = p_in + len;
            if (len >= 16) {
                *p = 16;
                p++;
                *p = '\0';
                len++;
            } else {
                for (i = 0; i <= (int)(16 - len) ; i++) {
                    *p =  16 - len;
                    p++;
                }
                *p = '\0';
                len = 16 + 1;
            }

        }
    }

    ret = do_aes(p_in, len, ctx.e_aes_mode, ctx.e_aes_len, password, strlen(password), ctx.e_aes_oper, out_buf);
    if (ret != TEEC_SUCCESS) {
        printf("aes operation fail, ret 0x%x.\n", ret);
        return -1;
    }

    if (ctx.e_aes_oper == E_AES_OP_DECRYPT) {
        if ((ctx.e_aes_mode == E_AES_MODE_CBC) || (ctx.e_aes_mode == E_AES_MODE_ECB)) {
            p = out_buf + len - 1;
            q = *p;
            if (q == 16) {
                len = len - 16;
            } else {
                len = len - q;
            }

            p = out_buf + len;
            *p  = '\0';

        } else if (ctx.e_aes_mode == E_AES_MODE_CTS) {
            p = out_buf + len - 1;
            q = *p;
            if (q == 16) {
                len--;
            } else {
                len = len - q - 1;
            }

            p = out_buf + len;
            *p  = '\0';
        }
    }
    if ((output_file = fopen(ctx.output_file_name, "wb")) == NULL) {
        printf("open file %s failed.\n", ctx.output_file_name);
        return -1;
    }

    fwrite(out_buf, len, 1, output_file);
    fclose(output_file);
    return 0;
}


