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

#include "stdio.h"
#include "stdlib.h"
#include "string.h"

#include "secure_storage_ca_handle.h"

enum {
    ESecurestorageOperation_Invalid = 0x00,
    ESecurestorageOperation_StoreFile = 0x01,
    ESecurestorageOperation_StoreBuffer = 0x02,
    ESecurestorageOperation_LoadToBuffer = 0x03,
    ESecurestorageOperation_LoadToFile = 0x04,
    ESecurestorageOperation_Rename = 0x05,
    ESecurestorageOperation_Delete = 0x06,
};

typedef struct {
    unsigned int operation;

    const char *file_name;
    const char *secure_object_name;
    const char *new_secure_object_name;
    const char *input_string;
} test_secure_storage_t;

static void __print_test_secure_storage_helps(void)
{
    printf("test_secure_storage's options\n");
    printf("\t'--storefile [%%s] [%%s]: 'store a file (%%s) to secure object (%%s)\n");
    printf("\t'--storebuffer [%%s] [%%s]: 'store a buffer (%%s) to secure object (%%s)\n");
    printf("\t'--load2buffer [%%s]: load secure object (%%s) to buffer\n");
    printf("\t'--load2file [%%s] [%%s]: load secure object (%%s) to file (%%s)\n");
    printf("\t'--rename [%%s] [%%s]: 'rename secure object (%%s) to (%%s)\n");
    printf("\t'--delete [%%s]: delete secure object (%%s)\n");
    printf("\t'--help': print help\n\n");

}

static int __init_test_secure_storage_params(int argc, char **argv, test_secure_storage_t *context)
{
    int i = 0;

    for (i = 1; i < argc; i++) {
        if (!strcmp("--storefile", argv[i])) {
            if ((i + 2) < argc) {
                context->file_name = argv[i + 1];
                context->secure_object_name = argv[i + 2];
                i += 2;
            } else {
                printf("[input_string argument] --storefile: should follow file name (%%s) and secure object (%%s).\n");
                return (-1);
            }
            context->operation = ESecurestorageOperation_StoreFile;
        } else if (!strcmp("--storebuffer", argv[i])) {
            if ((i + 2) < argc) {
                context->input_string = argv[i + 1];
                context->secure_object_name = argv[i + 2];
                i += 2;
            } else {
                printf("[input_string argument] --storebuffer: should follow string (%%s) and secure object (%%s).\n");
                return (-1);
            }
            context->operation = ESecurestorageOperation_StoreBuffer;
        } else if (!strcmp("--load2buffer", argv[i])) {
            if ((i + 1) < argc) {
                context->secure_object_name = argv[i + 1];
                i ++;
            } else {
                printf("[input_string argument] --load2buffer: should follow secure object (%%s).\n");
                return (-1);
            }
            context->operation = ESecurestorageOperation_LoadToBuffer;
        } else if (!strcmp("--load2file", argv[i])) {
            if ((i + 2) < argc) {
                context->secure_object_name = argv[i + 1];
                context->file_name = argv[i + 2];
                i += 2;
            } else {
                printf("[input_string argument] --load2file: should follow secure object (%%s) and file name (%%s).\n");
                return (-1);
            }
            context->operation = ESecurestorageOperation_LoadToFile;
        } else if (!strcmp("--rename", argv[i])) {
            if ((i + 2) < argc) {
                context->secure_object_name = argv[i + 1];
                context->new_secure_object_name = argv[i + 2];
                i += 2;
            } else {
                printf("[input_string argument] --rename: should follow old secure object (%%s) and new name (%%s).\n");
                return (-1);
            }
            context->operation = ESecurestorageOperation_Rename;
        } else if (!strcmp("--delete", argv[i])) {
            if ((i + 1) < argc) {
                context->secure_object_name = argv[i + 1];
                i ++;
            } else {
                printf("[input_string argument] --delete: should follow secure object (%%s).\n");
                return (-1);
            }
            context->operation = ESecurestorageOperation_Delete;
        } else if (!strcmp("--help", argv[i])) {
            __print_test_secure_storage_helps();
            return 1;
        } else {
            printf("error: NOT processed option(%s).\n", argv[i]);
            __print_test_secure_storage_helps();
            return (-1);
        }
    }

    return 0;
}

int main(int argc, char *argv[])
{
    int ret = 0;
    test_secure_storage_t context;
    FILE *fp;
    unsigned char *output;
    unsigned int outputlen = 0;

    memset(&context, 0x0, sizeof(context));

    if (argc > 1) {
        ret = __init_test_secure_storage_params(argc, argv, &context);
        if (ret) {
            return (-2);
        }
    } else {
        __print_test_secure_storage_helps();
        return (1);
    }

    switch (context.operation) {

        case ESecurestorageOperation_StoreFile:
            secure_store_ca_create_object(context.secure_object_name);
            secure_store_ca_store_file(context.file_name, context.secure_object_name);
            break;

        case ESecurestorageOperation_StoreBuffer:
            secure_store_ca_create_object(context.secure_object_name);
            secure_store_ca_store_buffer(context.input_string, context.secure_object_name);
            break;

        case ESecurestorageOperation_LoadToBuffer:
            outputlen = secure_store_ca_get_object_len(context.secure_object_name);
            output = (unsigned char *) malloc(outputlen + 1);
            if (output) {
                secure_store_ca_load_object(context.secure_object_name, outputlen, output);
                output[outputlen] = 0x0;
                printf("Load string object (%s): %s\n", context.secure_object_name, output);
                free(output);
            } else {
                printf("error: no memory.\n");
            }
            break;

        case ESecurestorageOperation_LoadToFile:
            outputlen = secure_store_ca_get_object_len(context.secure_object_name);
            output = (unsigned char *) malloc(outputlen);
            if (output) {
                secure_store_ca_load_object(context.secure_object_name, outputlen, output);
                fp = fopen(context.file_name, "wb");
                if (fp) {
                    fwrite(output, 1, outputlen, fp);
                    fclose(fp);
                } else {
                    printf("error: open (%s) fail.\n", context.file_name);
                }
                free(output);
            } else {
                printf("error: no memory.\n");
            }
            break;

        case ESecurestorageOperation_Rename:
            secure_store_ca_rename_object(context.secure_object_name, context.new_secure_object_name);
            break;

        case ESecurestorageOperation_Delete:
            secure_store_ca_delete_object(context.secure_object_name);
            break;

        default:
            printf("error: bad context.operation %d.\n", context.operation);
            break;
    }

    return 0;
}

