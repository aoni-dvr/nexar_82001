/*******************************************************************************
 * optee_tls_test_ca.c
 *
 * History:
 *  2021/01/05 - [Bo-Xi Chen] create file
 *
 * Copyright 2020 Ambarella International LP
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

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include "tee_client_api.h"

#include <mbedtls/ssl.h>

#include "optee_tls_test_ca.h"
#include "optee_tls_test_type.h"

char* optee_tls_read_cert(const char *path, int *size)
{
    FILE *cert_file;
    int len = 0;
    char *buf = NULL;

    cert_file = fopen(path, "r");
    if (cert_file == NULL) {
        printf("open cert_file %s fail.\n", path);
        return buf;
    }

    fseek(cert_file, 0, SEEK_END);
    if ((len = ftell(cert_file)) == -1 ) {
        fclose(cert_file);
        return buf;
    }
    fseek(cert_file, 0, SEEK_SET);

    buf = (char *)malloc(sizeof(char) * len);
    *size = len;

    fread(buf, 1, len, cert_file);

    return buf;
}

optee_tls_context *optee_tls_initialize(int debug_level)
{
    optee_tls_context *ctx;
    TEEC_Context *tee_ctx;
    TEEC_Session *tee_sess;

    ctx = (optee_tls_context*)malloc(sizeof(optee_tls_context));
    tee_ctx = (TEEC_Context*)malloc(sizeof(TEEC_Context));
    tee_sess = (TEEC_Session*)malloc(sizeof(TEEC_Session));

    ctx->ctx = tee_ctx;
    ctx->sess = tee_sess;
    ctx->debug_level = debug_level;

    return ctx;
}

TEEC_Result optee_tls_open(optee_tls_context* optee_tls_ctx, char *server_ip, int server_port, char *cert_file, int cert_len)
{
    TEEC_Operation operation;
    TEEC_Result result;
    TEEC_UUID svc_id = OPTEE_TLS_TEST_UUID;
    uint32_t origin;

    result = TEEC_InitializeContext(NULL, optee_tls_ctx->ctx);
    if (result != TEEC_SUCCESS) {
        printf("TEEC_InitializeContext fail, result=0x%x\n", result);
        return result;
    }

    result = TEEC_OpenSession(optee_tls_ctx->ctx, optee_tls_ctx->sess, &svc_id,
        TEEC_LOGIN_PUBLIC, NULL, NULL, &origin);
    if (result != TEEC_SUCCESS) {
        printf("TEEC_OpenSession failed, result=0x%x, origin=0x%x\n", result, origin);
        goto tag_exit_optee_tls_1;
    }

    memset(&operation, 0x0, sizeof(operation));
    operation.started = 1;
    operation.paramTypes = TEEC_PARAM_TYPES(TEEC_MEMREF_TEMP_INPUT, TEEC_VALUE_INPUT,
        TEEC_MEMREF_TEMP_INPUT, TEEC_VALUE_INPUT);
    operation.params[0].tmpref.size = strlen(server_ip) + 1;
    operation.params[0].tmpref.buffer = server_ip;
    operation.params[1].value.a = server_port;
    operation.params[1].value.b = optee_tls_ctx->debug_level;
    operation.params[2].tmpref.size = cert_len + 1;
    operation.params[2].tmpref.buffer = cert_file;
    operation.params[3].value.a = optee_tls_ctx->debug_level;

    result = TEEC_InvokeCommand(optee_tls_ctx->sess, CMD_OPEN, &operation, &origin);
    if (result != TEEC_SUCCESS) {
        printf("TEEC_InvokeCommand fail, result=0x%x, origin=0x%x\n", result, origin);
        goto tag_exit_optee_tls_2;
    }

    return result;

tag_exit_optee_tls_2:
    TEEC_CloseSession(optee_tls_ctx->sess);
tag_exit_optee_tls_1:
    TEEC_FinalizeContext(optee_tls_ctx->ctx);

    return result;

}

TEEC_Result optee_tls_close(optee_tls_context *optee_tls_ctx)
{
    TEEC_Operation operation;
    TEEC_Result result;
    uint32_t origin;

    memset(&operation, 0x0, sizeof(operation));
    operation.started = 1;
    operation.paramTypes = TEEC_PARAM_TYPES(TEEC_NONE, TEEC_NONE,
        TEEC_NONE, TEEC_NONE);

    result = TEEC_InvokeCommand(optee_tls_ctx->sess, CMD_CLOSE, &operation, &origin);
    if (result != TEEC_SUCCESS) {
        printf("TEEC_InvokeCommand fail, result=0x%x, origin=0x%x\n", result, origin);
        return result;
    }

    TEEC_CloseSession(optee_tls_ctx->sess);
    TEEC_FinalizeContext(optee_tls_ctx->ctx);

    if (optee_tls_ctx->ctx) {
        free(optee_tls_ctx->ctx);
        optee_tls_ctx->ctx = NULL;
    }

    if (optee_tls_ctx->sess) {
        free(optee_tls_ctx->sess);
        optee_tls_ctx->sess = NULL;
    }

    if (optee_tls_ctx) {
        free(optee_tls_ctx);
        optee_tls_ctx = NULL;
    }

    return result;
}

int optee_tls_write(optee_tls_context *optee_tls_ctx, char *buf, int num)
{
    TEEC_Operation operation;
    TEEC_Result result;
    uint32_t origin;
    int write_len = 0;

    if (num > MAX_WRITE_LENGTH) {
        printf("write num %d is too large, max num: %d\n", num, MAX_WRITE_LENGTH);
        return (-1);
    }

    memset(&operation, 0x0, sizeof(operation));
    operation.started = 1;
    operation.paramTypes = TEEC_PARAM_TYPES(TEEC_MEMREF_TEMP_INPUT, TEEC_VALUE_OUTPUT,
        TEEC_NONE, TEEC_NONE);
    operation.params[0].tmpref.size = num;
    operation.params[0].tmpref.buffer = buf;


    result = TEEC_InvokeCommand(optee_tls_ctx->sess, CMD_WRITE, &operation, &origin);
    if (result != TEEC_SUCCESS) {
        printf("TEEC_InvokeCommand fail, result=0x%x, origin=0x%x\n", result, origin);
        return (-2);
    }

    write_len = operation.params[1].value.a;

    return write_len;
}

int optee_tls_read(optee_tls_context *optee_tls_ctx, char *out_buf, int buf_size, int num)
{
    TEEC_Operation operation;
    TEEC_Result result;
    uint32_t origin;
    int read_len = 0;

    if (num > buf_size) {
        printf("buf size too small\n");
        return (-1);
    }

    memset(&operation, 0x0, sizeof(operation));
    operation.started = 1;
    operation.paramTypes = TEEC_PARAM_TYPES(TEEC_MEMREF_TEMP_OUTPUT, TEEC_VALUE_INPUT,
        TEEC_VALUE_OUTPUT, TEEC_NONE);
    operation.params[0].tmpref.size = buf_size;
    operation.params[0].tmpref.buffer = out_buf;
    operation.params[1].value.a = num;

    result = TEEC_InvokeCommand(optee_tls_ctx->sess, CMD_READ, &operation, &origin);
    if (result != TEEC_SUCCESS) {
        printf("TEEC_InvokeCommand fail, result=0x%x, origin=0x%x\n", result, origin);
            return (-2);
    }

    read_len = operation.params[2].value.a;

    return read_len;
}

