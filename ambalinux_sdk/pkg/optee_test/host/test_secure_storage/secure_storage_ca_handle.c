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

#include "tee_client_api.h"

#include "secure_storage_ta_type.h"

#include "secure_storage_ca_handle.h"

TEEC_UUID svc_id = TA_SEC_STORAGE_TEST_UUID;

static unsigned char *__load_file_to_mem(const char *file_name, int *data_size)
{
    unsigned char *p_buf = NULL;

    FILE *pf = fopen((const char *) file_name, "rb");
    if (!pf) {
        printf("error: open file (%s) fail\n", file_name);
        return NULL;
    }

    fseek(pf, 0, SEEK_END);
    data_size[0] = ftell(pf);
    fseek(pf, 0, SEEK_SET);

    if (!data_size[0]) {
        fclose(pf);
        printf("error: empty file (%s) fail\n", file_name);
        return NULL;
    }

    p_buf = (unsigned char *) malloc(data_size[0]);
    if (p_buf) {
        fread(p_buf, 1, data_size[0], pf);
    } else {
        printf("error: no memory, request size %d\n", data_size[0]);
        return NULL;
    }

    fclose(pf);
    return p_buf;
}

static int ca_send_command(TEEC_Operation *operation, TEEC_Session *session, unsigned int commandID)
{
    TEEC_Result result;
    int ret = 0;
    unsigned int origin;

    result = TEEC_InvokeCommand(session, commandID, operation, &origin);
    if (result != TEEC_SUCCESS) {
        printf("InvokeCommand failed, ReturnCode=0x%x, ReturnOrigin=0x%x\n", result, origin);
        ret = (-1);
    } else {
        //printf("InvokeCommand success\n");
        ret = 0;
    }

    return ret;
}

int secure_store_ca_create_object(const char *secure_object)
{
    TEEC_Session   session;    /* Define the session of TA&CA */
    TEEC_Operation operation;  /* Define the operation for communicating between TA&CA */
    TEEC_Context task_context;
    TEEC_Result ret = 0;       /* Define the return value of function */
    unsigned int origin;

    ret = TEEC_InitializeContext(NULL, &task_context);
    if (ret != TEEC_SUCCESS) {
        printf("secure store create file TEEC_InitializeContext fail, result=0x%x\n", ret);
        goto cleanup_1;
    }

    ret = TEEC_OpenSession(&task_context, &session, &svc_id,
                           TEEC_LOGIN_PUBLIC, NULL, NULL, &origin);
    if (ret != TEEC_SUCCESS) {
        printf("secure store create file TEEC_OpenSession failed, result=0x%x, origin=0x%x\n", ret, origin);
        goto cleanup_2;
    }

    /**3) Set the communication context between CA&TA */
    memset(&operation, 0x0, sizeof(TEEC_Operation));
    operation.started = 1;
    operation.paramTypes = TEEC_PARAM_TYPES(TEEC_MEMREF_TEMP_INPUT, TEEC_NONE,
                                            TEEC_NONE, TEEC_NONE);
    operation.params[0].tmpref.size = strlen(secure_object);
    operation.params[0].tmpref.buffer = (void *) secure_object;

    /**4) Send command to TA */
    ret = ca_send_command(&operation, &session, CMD_CREATE_OPER);
    if (ret) {
        goto cleanup_3;
    } else {
        goto cleanup_1;
    }

    /**5) The clean up operation */
cleanup_3:
    TEEC_CloseSession(&session);
cleanup_2:
    TEEC_FinalizeContext(&task_context);
cleanup_1:
    return ret;
}

int secure_store_ca_get_object_len(const char *secure_object_name)
{
    TEEC_Session   session;    /* Define the session of TA&CA */
    TEEC_Operation operation;  /* Define the operation for communicating between TA&CA */
    TEEC_Context task_context;
    TEEC_Result ret = 0;       /* Define the return value of function */
    unsigned int origin;
    unsigned int obj_len;

    ret = TEEC_InitializeContext(NULL, &task_context);
    if (ret != TEEC_SUCCESS) {
        printf("secure store create file TEEC_InitializeContext fail, result=0x%x\n", ret);
        goto cleanup_1;
    }

    ret = TEEC_OpenSession(&task_context, &session, &svc_id,
                           TEEC_LOGIN_PUBLIC, NULL, NULL, &origin);
    if (ret != TEEC_SUCCESS) {
        printf("secure store create file TEEC_OpenSession failed, result=0x%x, origin=0x%x\n", ret, origin);
        goto cleanup_2;
    }

    /**3) Set the communication context between CA&TA */
    memset(&operation, 0x0, sizeof(TEEC_Operation));
    operation.started = 1;
    operation.paramTypes = TEEC_PARAM_TYPES(TEEC_MEMREF_TEMP_INPUT, TEEC_VALUE_OUTPUT,
                                            TEEC_NONE, TEEC_NONE);
    operation.params[0].tmpref.size = strlen(secure_object_name);
    operation.params[0].tmpref.buffer = (void *) secure_object_name;

    /**4) Send command to TA */
    ret = ca_send_command(&operation, &session, CMD_GET_LEN_OPER);
    if (ret) {
        goto cleanup_3;
    }

    obj_len = operation.params[1].value.a;
    ret = obj_len;
    goto cleanup_1;

    /**5) The clean up operation */
cleanup_3:
    TEEC_CloseSession(&session);
cleanup_2:
    TEEC_FinalizeContext(&task_context);
cleanup_1:
    return ret;
}

int secure_store_ca_load_object(const char *secure_object_name,
                                unsigned int outbuf_size, unsigned char *output)
{
    TEEC_Session   session;    /* Define the session of TA&CA */
    TEEC_Operation operation;  /* Define the operation for communicating between TA&CA */
    TEEC_Context task_context;
    TEEC_Result ret = 0;       /* Define the return value of function */
    unsigned int origin;


    ret = TEEC_InitializeContext(NULL, &task_context);
    if (ret != TEEC_SUCCESS) {
        printf("secure store read file TEEC_InitializeContext fail, result=0x%x\n", ret);
        goto cleanup_1;
    }

    ret = TEEC_OpenSession(&task_context, &session, &svc_id,
                           TEEC_LOGIN_PUBLIC, NULL, NULL, &origin);
    if (ret != TEEC_SUCCESS) {
        printf("secure store read file TEEC_OpenSession failed, result=0x%x, origin=0x%x\n", ret, origin);
        goto cleanup_2;
    }

    /**3) Set the communication context between CA&TA */
    memset(&operation, 0x0, sizeof(TEEC_Operation));
    operation.started = 1;
    operation.paramTypes = TEEC_PARAM_TYPES(TEEC_MEMREF_TEMP_OUTPUT, TEEC_MEMREF_TEMP_INPUT,
                                            TEEC_NONE, TEEC_NONE);
    operation.params[0].tmpref.size = outbuf_size;
    operation.params[0].tmpref.buffer = (void *) output;
    operation.params[1].tmpref.size = strlen(secure_object_name);
    operation.params[1].tmpref.buffer = (void *) secure_object_name;

    /**4) Send command to TA */
    ret = ca_send_command(&operation, &session, CMD_READ_OPER);
    if (ret) {
        goto cleanup_3;
    } else {
        goto cleanup_1;
    }

    /**5) The clean up operation */
cleanup_3:
    TEEC_CloseSession(&session);
cleanup_2:
    TEEC_FinalizeContext(&task_context);
cleanup_1:
    return ret;
}

int secure_store_ca_store_file(const char *file_name, const char *secure_object_name)
{
    TEEC_Session   session;    /* Define the session of TA&CA */
    TEEC_Operation operation;  /* Define the operation for communicating between TA&CA */
    TEEC_Context task_context;
    TEEC_Result ret = 0;       /* Define the return value of function */
    unsigned int origin;
    unsigned char *p_input_buf = NULL;
    int buf_size = 0;

    p_input_buf = __load_file_to_mem(file_name, &buf_size);
    if (!p_input_buf) {
        printf("error: __load_file_to_mem fail\n");
        goto cleanup_1;
    }

    ret = TEEC_InitializeContext(NULL, &task_context);
    if (ret != TEEC_SUCCESS) {
        printf("secure store write file TEEC_InitializeContext fail, result=0x%x\n", ret);
        goto cleanup_1;
    }

    ret = TEEC_OpenSession(&task_context, &session, &svc_id,
                           TEEC_LOGIN_PUBLIC, NULL, NULL, &origin);
    if (ret != TEEC_SUCCESS) {
        printf("secure store write file TEEC_OpenSession failed, result=0x%x, origin=0x%x\n", ret, origin);
        goto cleanup_2;
    }

    /**3) Set the communication context between CA&TA */
    memset(&operation, 0x0, sizeof(TEEC_Operation));
    operation.started = 1;
    operation.paramTypes = TEEC_PARAM_TYPES(TEEC_MEMREF_TEMP_INPUT, TEEC_MEMREF_TEMP_INPUT,
                                            TEEC_NONE, TEEC_NONE);
    operation.params[0].tmpref.size = buf_size;
    operation.params[0].tmpref.buffer = (void *) p_input_buf;
    operation.params[1].tmpref.size = strlen(secure_object_name);
    operation.params[1].tmpref.buffer = (void *) secure_object_name;

    /**4) Send command to TA */
    ret = ca_send_command(&operation, &session, CMD_WRITE_OPER);
    if (ret) {
        goto cleanup_3;
    } else {
        goto cleanup_1;
    }

    /**5) The clean up operation */
cleanup_3:
    TEEC_CloseSession(&session);
cleanup_2:
    TEEC_FinalizeContext(&task_context);
cleanup_1:

    if (p_input_buf) {
        free(p_input_buf);
    }

    return ret;
}

int secure_store_ca_store_buffer(const char *buffer, const char *secure_object_name)
{
    TEEC_Session   session;    /* Define the session of TA&CA */
    TEEC_Operation operation;  /* Define the operation for communicating between TA&CA */
    TEEC_Context task_context;
    TEEC_Result ret = 0;       /* Define the return value of function */
    unsigned int origin;


    ret = TEEC_InitializeContext(NULL, &task_context);
    if (ret != TEEC_SUCCESS) {
        printf("secure store write file TEEC_InitializeContext fail, result=0x%x\n", ret);
        goto cleanup_1;
    }

    ret = TEEC_OpenSession(&task_context, &session, &svc_id,
                           TEEC_LOGIN_PUBLIC, NULL, NULL, &origin);
    if (ret != TEEC_SUCCESS) {
        printf("secure store write file TEEC_OpenSession failed, result=0x%x, origin=0x%x\n", ret, origin);
        goto cleanup_2;
    }

    /**3) Set the communication context between CA&TA */
    memset(&operation, 0x0, sizeof(TEEC_Operation));
    operation.started = 1;
    operation.paramTypes = TEEC_PARAM_TYPES(TEEC_MEMREF_TEMP_INPUT, TEEC_MEMREF_TEMP_INPUT,
                                            TEEC_NONE, TEEC_NONE);
    operation.params[0].tmpref.size = strlen(buffer);
    operation.params[0].tmpref.buffer = (void *) buffer;
    operation.params[1].tmpref.size = strlen(secure_object_name);
    operation.params[1].tmpref.buffer = (void *) secure_object_name;

    /**4) Send command to TA */
    ret = ca_send_command(&operation, &session, CMD_WRITE_OPER);
    if (ret) {
        goto cleanup_3;
    } else {
        goto cleanup_1;
    }

    /**5) The clean up operation */
cleanup_3:
    TEEC_CloseSession(&session);
cleanup_2:
    TEEC_FinalizeContext(&task_context);
cleanup_1:
    return ret;
}

int secure_store_ca_truncate_object(const char *secure_object_name, unsigned int size)
{
    TEEC_Session   session;    /* Define the session of TA&CA */
    TEEC_Operation operation;  /* Define the operation for communicating between TA&CA */
    TEEC_Context task_context;
    TEEC_Result ret = 0;       /* Define the return value of function */
    unsigned int origin;


    ret = TEEC_InitializeContext(NULL, &task_context);
    if (ret != TEEC_SUCCESS) {
        printf("secure store truncate file TEEC_InitializeContext fail, result=0x%x\n", ret);
        goto cleanup_1;
    }

    ret = TEEC_OpenSession(&task_context, &session, &svc_id,
                           TEEC_LOGIN_PUBLIC, NULL, NULL, &origin);
    if (ret != TEEC_SUCCESS) {
        printf("secure store truncate file TEEC_OpenSession failed, result=0x%x, origin=0x%x\n", ret, origin);
        goto cleanup_2;
    }

    /**3) Set the communication context between CA&TA */
    memset(&operation, 0x0, sizeof(TEEC_Operation));
    operation.started = 1;
    operation.paramTypes = TEEC_PARAM_TYPES(TEEC_VALUE_INPUT, TEEC_MEMREF_TEMP_INPUT,
                                            TEEC_NONE, TEEC_NONE);
    operation.params[0].value.a = size;
    operation.params[1].tmpref.buffer = (void *) secure_object_name;
    operation.params[1].tmpref.size = strlen(secure_object_name);

    /**4) Send command to TA */
    ret = ca_send_command(&operation, &session, CMD_TRUNCATE_OPER);
    if (ret) {
        goto cleanup_3;
    } else {
        goto cleanup_1;
    }

    /**5) The clean up operation */
cleanup_3:
    TEEC_CloseSession(&session);
cleanup_2:
    TEEC_FinalizeContext(&task_context);
cleanup_1:
    return ret;
}

int secure_store_ca_rename_object(const char *old_name, const char *new_name)
{
    TEEC_Session   session;    /* Define the session of TA&CA */
    TEEC_Operation operation;  /* Define the operation for communicating between TA&CA */
    TEEC_Context task_context;
    TEEC_Result ret = 0;       /* Define the return value of function */
    unsigned int origin;


    ret = TEEC_InitializeContext(NULL, &task_context);
    if (ret != TEEC_SUCCESS) {
        printf("secure store rename file TEEC_InitializeContext fail, result=0x%x\n", ret);
        goto cleanup_1;
    }

    ret = TEEC_OpenSession(&task_context, &session, &svc_id,
                           TEEC_LOGIN_PUBLIC, NULL, NULL, &origin);
    if (ret != TEEC_SUCCESS) {
        printf("secure store rename file TEEC_OpenSession failed, result=0x%x, origin=0x%x\n", ret, origin);
        goto cleanup_2;
    }

    /**3) Set the communication context between CA&TA */
    memset(&operation, 0x0, sizeof(TEEC_Operation));
    operation.started = 1;
    operation.paramTypes = TEEC_PARAM_TYPES(TEEC_MEMREF_TEMP_INPUT, TEEC_MEMREF_TEMP_INPUT,
                                            TEEC_NONE, TEEC_NONE);
    operation.params[0].tmpref.size = strlen(old_name);
    operation.params[0].tmpref.buffer = (void *) old_name;
    operation.params[1].tmpref.size = strlen(new_name);
    operation.params[1].tmpref.buffer = (void *) new_name;

    /**4) Send command to TA */
    ret = ca_send_command(&operation, &session, CMD_RENAME_OPER);
    if (ret) {
        goto cleanup_3;
    } else {
        goto cleanup_1;
    }

    /**5) The clean up operation */
cleanup_3:
    TEEC_CloseSession(&session);
cleanup_2:
    TEEC_FinalizeContext(&task_context);
cleanup_1:
    return ret;
}

int secure_store_ca_delete_object(const char *secure_object_name)
{
    TEEC_Session   session;    /* Define the session of TA&CA */
    TEEC_Operation operation;  /* Define the operation for communicating between TA&CA */
    TEEC_Context task_context;
    TEEC_Result ret = 0;       /* Define the return value of function */
    unsigned int origin;


    ret = TEEC_InitializeContext(NULL, &task_context);
    if (ret != TEEC_SUCCESS) {
        printf("secure store delete file TEEC_InitializeContext fail, result=0x%x\n", ret);
        goto cleanup_1;
    }

    ret = TEEC_OpenSession(&task_context, &session, &svc_id,
                           TEEC_LOGIN_PUBLIC, NULL, NULL, &origin);
    if (ret != TEEC_SUCCESS) {
        printf("secure store delete file TEEC_OpenSession failed, result=0x%x, origin=0x%x\n", ret, origin);
        goto cleanup_2;
    }

    /**3) Set the communication context between CA&TA */
    memset(&operation, 0x0, sizeof(TEEC_Operation));
    operation.started = 1;
    operation.paramTypes = TEEC_PARAM_TYPES(TEEC_MEMREF_TEMP_INPUT, TEEC_NONE,
                                            TEEC_NONE, TEEC_NONE);
    operation.params[0].tmpref.size = strlen(secure_object_name);
    operation.params[0].tmpref.buffer = (void *) secure_object_name;

    /**4) Send command to TA */
    ret = ca_send_command(&operation, &session, CMD_DELETE_OPER);
    if (ret) {
        goto cleanup_3;
    } else {
        goto cleanup_1;
    }

    /**5) The clean up operation */
cleanup_3:
    TEEC_CloseSession(&session);
cleanup_2:
    TEEC_FinalizeContext(&task_context);
cleanup_1:
    return ret;
}

