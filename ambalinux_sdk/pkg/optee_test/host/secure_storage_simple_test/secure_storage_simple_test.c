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

#include "secure_storage_simple_test.h"

TEEC_UUID svc_id = SEC_STORAGE_SIMPLE_TEST_UUID;

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

int secure_storage_simple_test_ca_create_object(const char *secure_object)
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

int secure_storage_simple_test_ca_load_object(const char *secure_object_name,
        unsigned int outbuf_size, char *output)
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

int secure_storage_simple_test_ca_load_2buffer(const char *secure_object_name,
        unsigned int outbuf_1_size, char *output_1, unsigned int outbuf_2_size, char *output_2)
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
    operation.paramTypes = TEEC_PARAM_TYPES(TEEC_MEMREF_TEMP_OUTPUT, TEEC_MEMREF_TEMP_OUTPUT,
                                            TEEC_MEMREF_TEMP_INPUT, TEEC_NONE);
    operation.params[0].tmpref.size = outbuf_1_size;
    operation.params[0].tmpref.buffer = (void *) output_1;
    operation.params[1].tmpref.size = outbuf_2_size;
    operation.params[1].tmpref.buffer = (void *) output_2;
    operation.params[2].tmpref.size = strlen(secure_object_name);
    operation.params[2].tmpref.buffer = (void *) secure_object_name;

    /**4) Send command to TA */
    ret = ca_send_command(&operation, &session, CMD_READ_TEST_OPER);
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

int secure_storage_simple_test_ca_store_buffer(const char *buffer, const char *secure_object_name)
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

int secure_storage_simple_test_ca_delete_object(const char *secure_object_name)
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
        printf("delete object %s success.\n", secure_object_name);
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

int main(int argc, char *argv[])
{
    //int ret;

    int i = 0;
    char *secure_object_name;
    char *input_string;
    char output[128] = {0x0};
    int operation;

    if (argc == 1) {
        printf("secure_storage_simple_test's options:\n");
        printf("\t'--storebuffer [%%s] [%%s]: 'store a buffer (%%s) to secure object (%%s)\n");
        printf("\t'--load2buffer [%%s]: load secure object (%%s) to buffer\n");
        printf("\t'--delete [%%s]: delete secure object (%%s)\n");
        return (-1);
    }

    for (i = 1; i < argc; i++) {
        if (!strcmp("--storebuffer", argv[i])) {
            if ((i + 2) < argc) {
                input_string = argv[i + 1];
                secure_object_name = argv[i + 2];
                i += 2;
            } else {
                printf("[input_string argument] --storebuffer: should follow string (%%s) and secure object (%%s).\n");
                return (-1);
            }
            operation = ESecurestorageOperation_StoreBuffer;
        } else if (!strcmp("--load2buffer", argv[i])) {
            if ((i + 1) < argc) {
                secure_object_name = argv[i + 1];
                i ++;
            } else {
                printf("[input_string argument] --load2buffer: should follow secure object (%%s).\n");
                return (-1);
            }
            operation = ESecurestorageOperation_LoadToBuffer;
        } else if (!strcmp("--delete", argv[i])) {
            if ((i + 1) < argc) {
                secure_object_name = argv[i + 1];
                i ++;
            } else {
                printf("[input_string argument] --delete: should follow secure object (%%s).\n");
                return (-1);
            }
            operation = ESecurestorageOperation_Delete;
        } else {
            printf("error: NOT processed option(%s).\n", argv[i]);
            printf("secure_storage_simple_test's options:\n");
            printf("\t'--storebuffer [%%s] [%%s]: 'store a buffer (%%s) to secure object (%%s)\n");
            printf("\t'--load2buffer [%%s]: load secure object (%%s) to buffer\n");
            printf("\t'--delete [%%s]: delete secure object (%%s)\n");
            return (-1);
        }
    }

    switch (operation) {
        case ESecurestorageOperation_StoreBuffer:
            secure_storage_simple_test_ca_create_object(secure_object_name);
            secure_storage_simple_test_ca_store_buffer(input_string, secure_object_name);
            break;

        case ESecurestorageOperation_LoadToBuffer:
            memset(output, 0x0, 128);
            secure_storage_simple_test_ca_load_object(secure_object_name, 128, output);
            printf("Load string object (%s): %s\n", secure_object_name, output);
            break;


        case ESecurestorageOperation_Delete:
            secure_storage_simple_test_ca_delete_object(secure_object_name);
            break;

        default:
            printf("error: bad operation %d.\n", operation);
            break;
    }

#if 0
    char test_buf_1[] = "this is a test buf for secure storage";
    char test_buf_2[] = "this buf will be divided into 2 pieces in test3";
    char secure_obj_1_name[] = "test_1_obj";
    char secure_obj_2_name[] = "test_2_obj";
    char secure_obj_3_name[] = "test_3_obj";
    char out_buf[80] = {0x0};
    char out_buf_2[80] = {0x0};

    /** test1: create a empty secure obj and load it*/
    secure_storage_simple_test_ca_create_object(secure_obj_1_name);
    secure_storage_simple_test_ca_load_object(secure_obj_1_name, 128, out_buf);
    secure_storage_simple_test_ca_delete_object(secure_obj_1_name);
    printf("output for test1: %s \n", out_buf);

    /** test2: create a empty secure obj, write test_buf_1 in it*/
    memset(out_buf, 0x0, 80);
    printf("test_buf_1: %s, length: %ld\n", test_buf_1, sizeof(test_buf_1) - 1);
    secure_storage_simple_test_ca_create_object(secure_obj_2_name);
    secure_storage_simple_test_ca_store_buffer(test_buf_1, secure_obj_2_name);

    /** load obj in test2*/
    secure_storage_simple_test_ca_load_object(secure_obj_2_name, 80, out_buf);
    secure_storage_simple_test_ca_delete_object(secure_obj_2_name);
    printf("output for test2: %s \n", out_buf);

    /** test3: create a empty secure obj, write test_buf_2 in it, read obj twice and load to 2 buffer*/
    memset(out_buf, 0x0, 80);
    printf("test_buf_2: %s, length: %ld\n", test_buf_2, sizeof(test_buf_2) - 1);
    secure_storage_simple_test_ca_create_object(secure_obj_3_name);
    secure_storage_simple_test_ca_store_buffer(test_buf_2, secure_obj_3_name);
    secure_storage_simple_test_ca_load_2buffer(secure_obj_3_name, 80, out_buf, 80, out_buf_2);
    secure_storage_simple_test_ca_delete_object(secure_obj_3_name);
    printf("output for test3:\n%s\n%s\n", out_buf, out_buf_2);
#endif

    return 0;
}
