/*
 * Copyright (C) 2018 Ambarella Inc.
 * All rights reserved.
 *
 * Author: Zhi He <zhe@ambarella.com>
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
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include "tee_client_api.h"
#include <unistd.h>

#include "program_otp_ca.h"

#define PTA_AMBA_OTP_UUID \
		{ 0xabcdef12, 0x7e33, 0x4ad2, \
			{ 0x98, 0x02, 0xe6, 0x4f, 0x2a, 0x7c, 0xc2, 0x30 } }

#define AMBA_OTP_CMD_WRITE_PUBKEY   0
#define AMBA_OTP_CMD_READ_PUBKEY_DIGEST   1
#define AMBA_OTP_CMD_READ_AMBA_UNIQUE_ID   2
#define AMBA_OTP_CMD_WRITE_CUSTOMER_ID   4
#define AMBA_OTP_CMD_READ_CUSTOMER_ID   5
#define AMBA_OTP_CMD_READ_MONOTINIC_COUNTER   6
#define AMBA_OTP_CMD_INCREASE_MONOTINIC_COUNTER   7
#define AMBA_OTP_CMD_PERMANENTLY_ENABLE_SECURE_BOOT   8
#define AMBA_OTP_CMD_GEN_HW_UNIQUE_ENCRYPTION_KEY   9

#define AMBA_OTP_CMD_GEN_AES_KEY   10
#define AMBA_OTP_CMD_WRITE_AES_KEY   11

#define AMBA_OTP_CMD_GEN_ECC_KEY   12
#define AMBA_OTP_CMD_WRITE_ECC_KEY   13

#define AMBA_OTP_CMD_SET_USR_SLOT_G0   14
#define AMBA_OTP_CMD_GET_USR_SLOT_G0   15

#define AMBA_OTP_CMD_SET_USR_SLOT_G1   16
#define AMBA_OTP_CMD_GET_USR_SLOT_G1   17

#define AMBA_OTP_CMD_REVOKE_KEY   18
#define AMBA_OTP_CMD_QUERY_OTP_SETTING   20

/* optional */
#define AMBA_OTP_CMD_SET_JTAG_EFUSE   21
#define AMBA_OTP_CMD_LOCK_ZONA_A   22

/* optional */
#define AMBA_OTP_CMD_GET_TEST_REGION  23
#define AMBA_OTP_CMD_SET_TEST_REGION  24

#define AMBA_OTP_CMD_EN_ANTI_ROLLBACK  25

#define AMBA_OTP_CMD_LOCK_PUKEY  26

#define AMBA_OTP_CMD_GET_USR_DATA_G0  27
#define AMBA_OTP_CMD_SET_USR_DATA_G0  28

/* optional */
#define AMBA_OTP_CMD_READ_PUBKEY		31

/* debug only */
#define AMBA_OTP_CMD_DEBUG_SHOW_CONTENT	32

#define AMBA_OTP_CMD_GET_SYSCONFIG  33
#define AMBA_OTP_CMD_SET_SYSCONFIG  34
#define AMBA_OTP_CMD_GET_CST_SEED  35
#define AMBA_OTP_CMD_GET_CST_CUK  36
#define AMBA_OTP_CMD_SET_CST_SEED_CUK  37
#define AMBA_OTP_CMD_GET_USR_CUK  38
#define AMBA_OTP_CMD_SET_USR_CUK  39
#define AMBA_OTP_CMD_DIS_SECURE_USB_BOOT  40

#define AMBA_OTP_CMD_GET_BST_VER  41
#define AMBA_OTP_CMD_INCREASE_BST_VER  42

#define AMBA_OTP_CMD_GET_MISC_CINFIG  43
#define AMBA_OTP_CMD_SET_MISC_CINFIG  44

void print_memory_u16_be(unsigned char *p, unsigned int size)
{
    if (size & 0x7) {
        printf("size (%d) should be multiply of 8\n", size);
    }

    while (size > 7) {
        printf("%02x%02x %02x%02x %02x%02x %02x%02x\n",
            p[1], p[0], p[3], p[2],
            p[5], p[4], p[7], p[6]);
        p += 8;
        size -= 8;
    }

    if (size) {
        printf("tail by bytes\n");
        while (size) {
            printf("%02x ", p[0]);
            p ++;
            size --;
        }
    }

    printf("\n");
}

void print_memory_u8(unsigned char *p, unsigned int size)
{
    while (size > 7) {
        printf("%02x%02x%02x%02x%02x%02x%02x%02x\n",
            p[0], p[1], p[2], p[3],
            p[4], p[5], p[6], p[7]);
        p += 8;
        size -= 8;
    }

    while (size) {
        printf("%02x", p[0]);
        p ++;
        size --;
    }

    printf("\n");
}

TEEC_Result do_write_otp_pubkey(unsigned char *p_key, unsigned int len,
    unsigned int key_index, unsigned int lock, unsigned int simulate)
{
    TEEC_Context context;
    TEEC_Session session;
    TEEC_Operation operation;
    TEEC_Result result;
    TEEC_UUID svc_id = PTA_AMBA_OTP_UUID;
    uint32_t origin;

    result = TEEC_InitializeContext(NULL, &context);
    if (result != TEEC_SUCCESS) {
        printf("TEEC_InitializeContext failed, result=0x%x\n", result);
        return result;
    }

    result = TEEC_OpenSession(&context, &session, &svc_id,
        TEEC_LOGIN_PUBLIC, NULL, NULL, &origin);
    if (result != TEEC_SUCCESS) {
        printf("TEEC_OpenSession failed, result=0x%x, origin=0x%x\n", result, origin);
        goto tag_exit_do_write_otp_pubkey_1;
    }

    memset(&operation, 0x0, sizeof(operation));
    operation.started = 1;
    operation.paramTypes = TEEC_PARAM_TYPES(TEEC_MEMREF_TEMP_INPUT, TEEC_VALUE_INPUT,
        TEEC_VALUE_INPUT, TEEC_NONE);
    operation.params[0].tmpref.size = len;
    operation.params[0].tmpref.buffer = p_key;
    operation.params[1].value.a = key_index;
    operation.params[1].value.b = lock;
    operation.params[2].value.a = simulate;

    result = TEEC_InvokeCommand(&session,
        AMBA_OTP_CMD_WRITE_PUBKEY, &operation, &origin);
    if (result != TEEC_SUCCESS) {
        printf("TEEC_InvokeCommand failed, result=0x%x, origin=0x%x\n", result, origin);
        goto tag_exit_do_write_otp_pubkey_2;
    }

tag_exit_do_write_otp_pubkey_2:
    TEEC_CloseSession(&session);
tag_exit_do_write_otp_pubkey_1:
    TEEC_FinalizeContext(&context);

    return result;
}

TEEC_Result do_read_otp_pubkey(unsigned char *p_key, unsigned int len,
    unsigned int key_index, unsigned int *locked, unsigned int *revoked)
{
    TEEC_Context context;
    TEEC_Session session;
    TEEC_Operation operation;
    TEEC_Result result;
    TEEC_UUID svc_id = PTA_AMBA_OTP_UUID;
    uint32_t origin;

    result = TEEC_InitializeContext(NULL, &context);
    if (result != TEEC_SUCCESS) {
        printf("TEEC_InitializeContext failed, result=0x%x\n", result);
        return result;
    }

    result = TEEC_OpenSession(&context, &session, &svc_id,
        TEEC_LOGIN_PUBLIC, NULL, NULL, &origin);
    if (result != TEEC_SUCCESS) {
        printf("TEEC_OpenSession failed, result=0x%x, origin=0x%x\n", result, origin);
        goto tag_exit_do_read_otp_pubkey_1;
    }

    memset(&operation, 0x0, sizeof(operation));
    operation.started = 1;
    operation.paramTypes = TEEC_PARAM_TYPES(TEEC_MEMREF_TEMP_OUTPUT, TEEC_VALUE_INOUT,
        TEEC_VALUE_OUTPUT, TEEC_NONE);
    operation.params[0].tmpref.size = len;
    operation.params[0].tmpref.buffer = p_key;
    operation.params[1].value.a = key_index;

    result = TEEC_InvokeCommand(&session,
        AMBA_OTP_CMD_READ_PUBKEY, &operation, &origin);
    if (result != TEEC_SUCCESS) {
        printf("TEEC_InvokeCommand failed, result=0x%x, origin=0x%x\n", result, origin);
        goto tag_exit_do_read_otp_pubkey_2;
    }
    *locked = operation.params[1].value.b;
    *revoked = operation.params[2].value.a;

tag_exit_do_read_otp_pubkey_2:
    TEEC_CloseSession(&session);
tag_exit_do_read_otp_pubkey_1:
    TEEC_FinalizeContext(&context);

    return result;
}

TEEC_Result do_read_otp_pubkey_digest(unsigned char *p_key, unsigned int len,
    unsigned int key_index, unsigned int *locked, unsigned int *revoked)
{
    TEEC_Context context;
    TEEC_Session session;
    TEEC_Operation operation;
    TEEC_Result result;
    TEEC_UUID svc_id = PTA_AMBA_OTP_UUID;
    uint32_t origin;

    result = TEEC_InitializeContext(NULL, &context);
    if (result != TEEC_SUCCESS) {
        printf("TEEC_InitializeContext failed, result=0x%x\n", result);
        return result;
    }

    result = TEEC_OpenSession(&context, &session, &svc_id,
        TEEC_LOGIN_PUBLIC, NULL, NULL, &origin);
    if (result != TEEC_SUCCESS) {
        printf("TEEC_OpenSession failed, result=0x%x, origin=0x%x\n", result, origin);
        goto tag_exit_do_read_otp_pubkey_digest_1;
    }

    memset(&operation, 0x0, sizeof(operation));
    operation.started = 1;
    operation.paramTypes = TEEC_PARAM_TYPES(TEEC_MEMREF_TEMP_OUTPUT, TEEC_VALUE_INOUT,
        TEEC_VALUE_OUTPUT, TEEC_NONE);
    operation.params[0].tmpref.size = len;
    operation.params[0].tmpref.buffer = p_key;
    operation.params[1].value.a = key_index;

    result = TEEC_InvokeCommand(&session,
        AMBA_OTP_CMD_READ_PUBKEY_DIGEST, &operation, &origin);
    if (result != TEEC_SUCCESS) {
        printf("TEEC_InvokeCommand failed, result=0x%x, origin=0x%x\n", result, origin);
        goto tag_exit_do_read_otp_pubkey_digest_2;
    }
    *locked = operation.params[1].value.b;
    *revoked = operation.params[2].value.a;

tag_exit_do_read_otp_pubkey_digest_2:
    TEEC_CloseSession(&session);
tag_exit_do_read_otp_pubkey_digest_1:
    TEEC_FinalizeContext(&context);

    return result;
}


TEEC_Result do_read_otp_amba_unique_id(unsigned char *p_id, unsigned int len)
{
    TEEC_Context context;
    TEEC_Session session;
    TEEC_Operation operation;
    TEEC_Result result;
    TEEC_UUID svc_id = PTA_AMBA_OTP_UUID;
    uint32_t origin;

    result = TEEC_InitializeContext(NULL, &context);
    if (result != TEEC_SUCCESS) {
        printf("TEEC_InitializeContext fail, result=0x%x\n", result);
        return result;
    }

    result = TEEC_OpenSession(&context, &session, &svc_id,
        TEEC_LOGIN_PUBLIC, NULL, NULL, &origin);
    if (result != TEEC_SUCCESS) {
        printf("TEEC_OpenSession failed, result=0x%x, origin=0x%x\n", result, origin);
        goto tag_exit_do_read_otp_amba_unique_id_1;
    }

    memset(&operation, 0x0, sizeof(operation));
    operation.started = 1;
    operation.paramTypes = TEEC_PARAM_TYPES(TEEC_MEMREF_TEMP_OUTPUT, TEEC_NONE,
        TEEC_NONE, TEEC_NONE);
    operation.params[0].tmpref.size = len;
    operation.params[0].tmpref.buffer = p_id;

    result = TEEC_InvokeCommand(&session,
        AMBA_OTP_CMD_READ_AMBA_UNIQUE_ID, &operation, &origin);
    if (result != TEEC_SUCCESS) {
        printf("TEEC_InvokeCommand fail, result=0x%x, origin=0x%x\n", result, origin);
        goto tag_exit_do_read_otp_amba_unique_id_2;
    }

tag_exit_do_read_otp_amba_unique_id_2:
    TEEC_CloseSession(&session);
tag_exit_do_read_otp_amba_unique_id_1:
    TEEC_FinalizeContext(&context);

    return result;
}


TEEC_Result do_write_otp_customer_id(unsigned char *p_id, unsigned int len,
    unsigned int lock, unsigned int simulate)
{
    TEEC_Context context;
    TEEC_Session session;
    TEEC_Operation operation;
    TEEC_Result result;
    TEEC_UUID svc_id = PTA_AMBA_OTP_UUID;
    uint32_t origin;

    result = TEEC_InitializeContext(NULL, &context);
    if (result != TEEC_SUCCESS) {
        printf("TEEC_InitializeContext failed, result=0x%x\n", result);
        return result;
    }

    result = TEEC_OpenSession(&context, &session, &svc_id,
        TEEC_LOGIN_PUBLIC, NULL, NULL, &origin);
    if (result != TEEC_SUCCESS) {
        printf("TEEC_OpenSession failed, result=0x%x, origin=0x%x\n", result, origin);
        goto tag_exit_do_write_otp_customer_id_1;
    }

    memset(&operation, 0x0, sizeof(operation));
    operation.started = 1;
    operation.paramTypes = TEEC_PARAM_TYPES(TEEC_MEMREF_TEMP_INPUT, TEEC_VALUE_INPUT,
        TEEC_NONE, TEEC_NONE);
    operation.params[0].tmpref.size = len;
    operation.params[0].tmpref.buffer = p_id;
    operation.params[1].value.a = lock;
    operation.params[1].value.b = simulate;

    result = TEEC_InvokeCommand(&session,
        AMBA_OTP_CMD_WRITE_CUSTOMER_ID, &operation, &origin);
    if (result != TEEC_SUCCESS) {
        printf("TEEC_InvokeCommand failed, result=0x%x, origin=0x%x\n", result, origin);
        goto tag_exit_do_write_otp_customer_id_2;
    }

tag_exit_do_write_otp_customer_id_2:
    TEEC_CloseSession(&session);
tag_exit_do_write_otp_customer_id_1:
    TEEC_FinalizeContext(&context);

    return result;
}


TEEC_Result do_read_otp_customer_id(unsigned char *p_id, unsigned int len,
    unsigned int *locked)
{
    TEEC_Context context;
    TEEC_Session session;
    TEEC_Operation operation;
    TEEC_Result result;
    TEEC_UUID svc_id = PTA_AMBA_OTP_UUID;
    uint32_t origin;

    result = TEEC_InitializeContext(NULL, &context);
    if (result != TEEC_SUCCESS) {
        printf("TEEC_InitializeContext failed, result=0x%x\n", result);
        return result;
    }

    result = TEEC_OpenSession(&context, &session, &svc_id,
        TEEC_LOGIN_PUBLIC, NULL, NULL, &origin);
    if (result != TEEC_SUCCESS) {
        printf("TEEC_OpenSession failed, result=0x%x, origin=0x%x\n", result, origin);
        goto tag_exit_do_read_otp_customer_id_1;
    }

    memset(&operation, 0x0, sizeof(operation));
    operation.started = 1;
    operation.paramTypes = TEEC_PARAM_TYPES(TEEC_MEMREF_TEMP_OUTPUT, TEEC_VALUE_OUTPUT,
        TEEC_NONE, TEEC_NONE);
    operation.params[0].tmpref.size = len;
    operation.params[0].tmpref.buffer = p_id;

    result = TEEC_InvokeCommand(&session,
        AMBA_OTP_CMD_READ_CUSTOMER_ID, &operation, &origin);
    if (result != TEEC_SUCCESS) {
        printf("TEEC_InvokeCommand failed, result=0x%x, origin=0x%x\n", result, origin);
        goto tag_exit_do_read_otp_customer_id_2;
    }
    *locked = operation.params[1].value.a;

tag_exit_do_read_otp_customer_id_2:
    TEEC_CloseSession(&session);
tag_exit_do_read_otp_customer_id_1:
    TEEC_FinalizeContext(&context);

    return result;
}


TEEC_Result do_read_otp_mono_counter(unsigned int *counter,
    unsigned int mono_index)
{
    TEEC_Context context;
    TEEC_Session session;
    TEEC_Operation operation;
    TEEC_Result result;
    TEEC_UUID svc_id = PTA_AMBA_OTP_UUID;
    uint32_t origin;

    result = TEEC_InitializeContext(NULL, &context);
    if (result != TEEC_SUCCESS) {
        printf("TEEC_InitializeContext failed, result=0x%x\n", result);
        return result;
    }

    result = TEEC_OpenSession(&context, &session, &svc_id,
        TEEC_LOGIN_PUBLIC, NULL, NULL, &origin);
    if (result != TEEC_SUCCESS) {
        printf("TEEC_OpenSession failed, result=0x%x, origin=0x%x\n", result, origin);
        goto tag_exit_do_read_otp_mono_counter_1;
    }

    memset(&operation, 0x0, sizeof(operation));
    operation.started = 1;
    operation.paramTypes = TEEC_PARAM_TYPES(TEEC_VALUE_INOUT, TEEC_NONE,
        TEEC_NONE, TEEC_NONE);
    operation.params[0].value.b = mono_index;

    result = TEEC_InvokeCommand(&session,
        AMBA_OTP_CMD_READ_MONOTINIC_COUNTER, &operation, &origin);
    if (result != TEEC_SUCCESS) {
        printf("TEEC_InvokeCommand failed, result=0x%x, origin=0x%x\n", result, origin);
        goto tag_exit_do_read_otp_mono_counter_2;
    }
    *counter = operation.params[0].value.a;

tag_exit_do_read_otp_mono_counter_2:
    TEEC_CloseSession(&session);
tag_exit_do_read_otp_mono_counter_1:
    TEEC_FinalizeContext(&context);

    return result;
}


TEEC_Result do_increase_otp_mono_counter(
    unsigned int mono_index, unsigned int simulate)
{
    TEEC_Context context;
    TEEC_Session session;
    TEEC_Operation operation;
    TEEC_Result result;
    TEEC_UUID svc_id = PTA_AMBA_OTP_UUID;
    uint32_t origin;

    result = TEEC_InitializeContext(NULL, &context);
    if (result != TEEC_SUCCESS) {
        printf("TEEC_InitializeContext failed, result=0x%x\n", result);
        return result;
    }

    result = TEEC_OpenSession(&context, &session, &svc_id,
        TEEC_LOGIN_PUBLIC, NULL, NULL, &origin);
    if (result != TEEC_SUCCESS) {
        printf("TEEC_OpenSession failed, result=0x%x, origin=0x%x\n", result, origin);
        goto tag_exit_do_increase_otp_mono_counter_1;
    }

    memset(&operation, 0x0, sizeof(operation));
    operation.started = 1;
    operation.paramTypes = TEEC_PARAM_TYPES(TEEC_VALUE_INPUT, TEEC_NONE,
        TEEC_NONE, TEEC_NONE);
    operation.params[0].value.a = mono_index;
    operation.params[0].value.b = simulate;

    result = TEEC_InvokeCommand(&session,
        AMBA_OTP_CMD_INCREASE_MONOTINIC_COUNTER, &operation, &origin);
    if (result != TEEC_SUCCESS) {
        printf("TEEC_InvokeCommand failed, result=0x%x, origin=0x%x\n", result, origin);
        goto tag_exit_do_increase_otp_mono_counter_2;
    }

tag_exit_do_increase_otp_mono_counter_2:
    TEEC_CloseSession(&session);
tag_exit_do_increase_otp_mono_counter_1:
    TEEC_FinalizeContext(&context);

    return result;
}


TEEC_Result do_permanently_enable_secure_boot(unsigned int simulate)
{
    TEEC_Context context;
    TEEC_Session session;
    TEEC_Operation operation;
    TEEC_Result result;
    TEEC_UUID svc_id = PTA_AMBA_OTP_UUID;
    uint32_t origin;

    result = TEEC_InitializeContext(NULL, &context);
    if (result != TEEC_SUCCESS) {
        printf("TEEC_InitializeContext failed, result=0x%x\n", result);
        return result;
    }

    result = TEEC_OpenSession(&context, &session, &svc_id,
        TEEC_LOGIN_PUBLIC, NULL, NULL, &origin);
    if (result != TEEC_SUCCESS) {
        printf("TEEC_OpenSession failed, result=0x%x, origin=0x%x\n", result, origin);
        goto tag_exit_do_permanently_enable_secure_boot_1;
    }

    memset(&operation, 0x0, sizeof(operation));
    operation.started = 1;
    operation.paramTypes = TEEC_PARAM_TYPES(TEEC_VALUE_INPUT, TEEC_NONE,
        TEEC_NONE, TEEC_NONE);
    operation.params[0].value.a = simulate;

    result = TEEC_InvokeCommand(&session,
        AMBA_OTP_CMD_PERMANENTLY_ENABLE_SECURE_BOOT, &operation, &origin);
    if (result != TEEC_SUCCESS) {
        printf("TEEC_InvokeCommand failed, result=0x%x, origin=0x%x\n", result, origin);
        goto tag_exit_do_permanently_enable_secure_boot_2;
    }

tag_exit_do_permanently_enable_secure_boot_2:
    TEEC_CloseSession(&session);
tag_exit_do_permanently_enable_secure_boot_1:
    TEEC_FinalizeContext(&context);

    return result;
}


TEEC_Result do_generate_hw_unique_encryption_key(unsigned int simulate)
{
    TEEC_Context context;
    TEEC_Session session;
    TEEC_Operation operation;
    TEEC_Result result;
    TEEC_UUID svc_id = PTA_AMBA_OTP_UUID;
    uint32_t origin;

    result = TEEC_InitializeContext(NULL, &context);
    if (result != TEEC_SUCCESS) {
        printf("TEEC_InitializeContext failed, result=0x%x\n", result);
        return result;
    }

    result = TEEC_OpenSession(&context, &session, &svc_id,
        TEEC_LOGIN_PUBLIC, NULL, NULL, &origin);
    if (result != TEEC_SUCCESS) {
        printf("TEEC_OpenSession failed, result=0x%x, origin=0x%x\n", result, origin);
        goto tag_exit_do_generate_hw_unique_encryption_key_1;
    }

    memset(&operation, 0x0, sizeof(operation));
    operation.started = 1;
    operation.paramTypes = TEEC_PARAM_TYPES(TEEC_VALUE_INPUT, TEEC_NONE,
        TEEC_NONE, TEEC_NONE);
    operation.params[0].value.a = simulate;

    result = TEEC_InvokeCommand(&session,
        AMBA_OTP_CMD_GEN_HW_UNIQUE_ENCRYPTION_KEY, &operation, &origin);
    if (result != TEEC_SUCCESS) {
        printf("TEEC_InvokeCommand failed, result=0x%x, origin=0x%x\n", result, origin);
        goto tag_exit_do_generate_hw_unique_encryption_key_2;
    }

tag_exit_do_generate_hw_unique_encryption_key_2:
    TEEC_CloseSession(&session);
tag_exit_do_generate_hw_unique_encryption_key_1:
    TEEC_FinalizeContext(&context);

    return result;
}


TEEC_Result do_revoke_key(unsigned int key_index, unsigned int simulate)
{
    TEEC_Context context;
    TEEC_Session session;
    TEEC_Operation operation;
    TEEC_Result result;
    TEEC_UUID svc_id = PTA_AMBA_OTP_UUID;
    uint32_t origin;

    result = TEEC_InitializeContext(NULL, &context);
    if (result != TEEC_SUCCESS) {
        printf("TEEC_InitializeContext failed, result=0x%x\n", result);
        return result;
    }

    result = TEEC_OpenSession(&context, &session, &svc_id,
        TEEC_LOGIN_PUBLIC, NULL, NULL, &origin);
    if (result != TEEC_SUCCESS) {
        printf("TEEC_OpenSession failed, result=0x%x, origin=0x%x\n", result, origin);
        goto tag_exit_do_revoke_key_1;
    }

    memset(&operation, 0x0, sizeof(operation));
    operation.started = 1;
    operation.paramTypes = TEEC_PARAM_TYPES(TEEC_VALUE_INPUT, TEEC_NONE,
        TEEC_NONE, TEEC_NONE);
    operation.params[0].value.a = key_index;
    operation.params[0].value.b = simulate;

    result = TEEC_InvokeCommand(&session,
        AMBA_OTP_CMD_REVOKE_KEY, &operation, &origin);
    if (result != TEEC_SUCCESS) {
        printf("TEEC_InvokeCommand failed, result=0x%x, origin=0x%x\n", result, origin);
        goto tag_exit_do_revoke_key_2;
    }

tag_exit_do_revoke_key_2:
    TEEC_CloseSession(&session);
tag_exit_do_revoke_key_1:
    TEEC_FinalizeContext(&context);

    return result;
}


TEEC_Result do_write_aes_key(unsigned char *key, unsigned int length,
    unsigned int key_index, unsigned int lock, unsigned int simulate)
{
    TEEC_Context context;
    TEEC_Session session;
    TEEC_Operation operation;
    TEEC_Result result;
    TEEC_UUID svc_id = PTA_AMBA_OTP_UUID;
    uint32_t origin;

    result = TEEC_InitializeContext(NULL, &context);
    if (result != TEEC_SUCCESS) {
        printf("TEEC_InitializeContext failed, result=0x%x\n", result);
        return result;
    }

    result = TEEC_OpenSession(&context, &session, &svc_id,
        TEEC_LOGIN_PUBLIC, NULL, NULL, &origin);
    if (result != TEEC_SUCCESS) {
        printf("TEEC_OpenSession failed, result=0x%x, origin=0x%x\n", result, origin);
        goto tag_exit_do_write_aes_key_1;
    }

    memset(&operation, 0x0, sizeof(operation));
    operation.started = 1;
    operation.paramTypes = TEEC_PARAM_TYPES(TEEC_MEMREF_TEMP_INPUT, TEEC_VALUE_INPUT,
        TEEC_VALUE_INPUT, TEEC_NONE);
    operation.params[0].tmpref.size = length;
    operation.params[0].tmpref.buffer = key;
    operation.params[1].value.a = key_index;
    operation.params[1].value.b = lock;
    operation.params[2].value.a = simulate;

    result = TEEC_InvokeCommand(&session,
        AMBA_OTP_CMD_WRITE_AES_KEY, &operation, &origin);
    if (result != TEEC_SUCCESS) {
        printf("TEEC_InvokeCommand failed, result=0x%x, origin=0x%x\n", result, origin);
        goto tag_exit_do_write_aes_key_2;
    }

tag_exit_do_write_aes_key_2:
    TEEC_CloseSession(&session);
tag_exit_do_write_aes_key_1:
    TEEC_FinalizeContext(&context);

    return result;
}


TEEC_Result do_write_ecc_key(unsigned char *key, unsigned int length,
    unsigned int key_index, unsigned int lock, unsigned int simulate)
{
    TEEC_Context context;
    TEEC_Session session;
    TEEC_Operation operation;
    TEEC_Result result;
    TEEC_UUID svc_id = PTA_AMBA_OTP_UUID;
    uint32_t origin;

    result = TEEC_InitializeContext(NULL, &context);
    if (result != TEEC_SUCCESS) {
        printf("TEEC_InitializeContext failed, result=0x%x\n", result);
        return result;
    }

    result = TEEC_OpenSession(&context, &session, &svc_id,
        TEEC_LOGIN_PUBLIC, NULL, NULL, &origin);
    if (result != TEEC_SUCCESS) {
        printf("TEEC_OpenSession failed, result=0x%x, origin=0x%x\n", result, origin);
        goto tag_exit_do_write_ecc_key_1;
    }

    memset(&operation, 0x0, sizeof(operation));
    operation.started = 1;
    operation.paramTypes = TEEC_PARAM_TYPES(TEEC_MEMREF_TEMP_INPUT, TEEC_VALUE_INPUT,
        TEEC_VALUE_INPUT, TEEC_NONE);
    operation.params[0].tmpref.size = length;
    operation.params[0].tmpref.buffer = key;
    operation.params[1].value.a = key_index;
    operation.params[1].value.b = lock;
    operation.params[2].value.a = simulate;

    result = TEEC_InvokeCommand(&session,
        AMBA_OTP_CMD_WRITE_ECC_KEY, &operation, &origin);
    if (result != TEEC_SUCCESS) {
        printf("TEEC_InvokeCommand failed, result=0x%x, origin=0x%x\n", result, origin);
        goto tag_exit_do_write_ecc_key_2;
    }

tag_exit_do_write_ecc_key_2:
    TEEC_CloseSession(&session);
tag_exit_do_write_ecc_key_1:
    TEEC_FinalizeContext(&context);

    return result;
}


TEEC_Result do_write_usr_slot_g0(unsigned char *content, unsigned int length,
    unsigned int index, unsigned int lock, unsigned int simulate)
{
    TEEC_Context context;
    TEEC_Session session;
    TEEC_Operation operation;
    TEEC_Result result;
    TEEC_UUID svc_id = PTA_AMBA_OTP_UUID;
    uint32_t origin;

    result = TEEC_InitializeContext(NULL, &context);
    if (result != TEEC_SUCCESS) {
        printf("TEEC_InitializeContext failed, result=0x%x\n", result);
        return result;
    }

    result = TEEC_OpenSession(&context, &session, &svc_id,
        TEEC_LOGIN_PUBLIC, NULL, NULL, &origin);
    if (result != TEEC_SUCCESS) {
        printf("TEEC_OpenSession failed, result=0x%x, origin=0x%x\n", result, origin);
        goto tag_exit_do_write_usr_slot_g0_1;
    }

    memset(&operation, 0x0, sizeof(operation));
    operation.started = 1;
    operation.paramTypes = TEEC_PARAM_TYPES(TEEC_MEMREF_TEMP_INPUT, TEEC_VALUE_INPUT,
        TEEC_VALUE_INPUT, TEEC_NONE);
    operation.params[0].tmpref.size = length;
    operation.params[0].tmpref.buffer = content;
    operation.params[1].value.a = index;
    operation.params[1].value.b = lock;
    operation.params[2].value.a = simulate;

    result = TEEC_InvokeCommand(&session,
        AMBA_OTP_CMD_SET_USR_SLOT_G0, &operation, &origin);
    if (result != TEEC_SUCCESS) {
        printf("TEEC_InvokeCommand failed, result=0x%x, origin=0x%x\n", result, origin);
        goto tag_exit_do_write_usr_slot_g0_2;
    }

tag_exit_do_write_usr_slot_g0_2:
    TEEC_CloseSession(&session);
tag_exit_do_write_usr_slot_g0_1:
    TEEC_FinalizeContext(&context);

    return result;
}


TEEC_Result do_read_usr_slot_g0(unsigned char *content, unsigned int length,
    unsigned int index, unsigned int *locked)
{
    TEEC_Context context;
    TEEC_Session session;
    TEEC_Operation operation;
    TEEC_Result result;
    TEEC_UUID svc_id = PTA_AMBA_OTP_UUID;
    uint32_t origin;

    result = TEEC_InitializeContext(NULL, &context);
    if (result != TEEC_SUCCESS) {
        printf("TEEC_InitializeContext failed, result=0x%x\n", result);
        return result;
    }

    result = TEEC_OpenSession(&context, &session, &svc_id,
        TEEC_LOGIN_PUBLIC, NULL, NULL, &origin);
    if (result != TEEC_SUCCESS) {
        printf("TEEC_OpenSession failed, result=0x%x, origin=0x%x\n", result, origin);
        goto tag_exit_do_read_usr_slot_g0_1;
    }

    memset(&operation, 0x0, sizeof(operation));
    operation.started = 1;
    operation.paramTypes = TEEC_PARAM_TYPES(TEEC_MEMREF_TEMP_OUTPUT, TEEC_VALUE_INOUT,
        TEEC_NONE, TEEC_NONE);
    operation.params[0].tmpref.size = length;
    operation.params[0].tmpref.buffer = content;
    operation.params[1].value.a = index;

    result = TEEC_InvokeCommand(&session,
        AMBA_OTP_CMD_GET_USR_SLOT_G0, &operation, &origin);
    if (result != TEEC_SUCCESS) {
        printf("TEEC_InvokeCommand failed, result=0x%x, origin=0x%x\n", result, origin);
        goto tag_exit_do_read_usr_slot_g0_2;
    }
    *locked = operation.params[1].value.b;

tag_exit_do_read_usr_slot_g0_2:
    TEEC_CloseSession(&session);
tag_exit_do_read_usr_slot_g0_1:
    TEEC_FinalizeContext(&context);

    return result;
}


TEEC_Result do_write_usr_slot_g1(unsigned char *content, unsigned int length,
    unsigned int index, unsigned int lock, unsigned int simulate)
{
    TEEC_Context context;
    TEEC_Session session;
    TEEC_Operation operation;
    TEEC_Result result;
    TEEC_UUID svc_id = PTA_AMBA_OTP_UUID;
    uint32_t origin;

    result = TEEC_InitializeContext(NULL, &context);
    if (result != TEEC_SUCCESS) {
        printf("TEEC_InitializeContext failed, result=0x%x\n", result);
        return result;
    }

    result = TEEC_OpenSession(&context, &session, &svc_id,
        TEEC_LOGIN_PUBLIC, NULL, NULL, &origin);
    if (result != TEEC_SUCCESS) {
        printf("TEEC_OpenSession failed, result=0x%x, origin=0x%x\n", result, origin);
        goto tag_exit_do_write_usr_slot_g1_1;
    }

    memset(&operation, 0x0, sizeof(operation));
    operation.started = 1;
    operation.paramTypes = TEEC_PARAM_TYPES(TEEC_MEMREF_TEMP_INPUT, TEEC_VALUE_INPUT,
        TEEC_VALUE_INPUT, TEEC_NONE);
    operation.params[0].tmpref.size = length;
    operation.params[0].tmpref.buffer = content;
    operation.params[1].value.a = index;
    operation.params[1].value.b = lock;
    operation.params[2].value.a = simulate;

    result = TEEC_InvokeCommand(&session,
        AMBA_OTP_CMD_SET_USR_SLOT_G1, &operation, &origin);
    if (result != TEEC_SUCCESS) {
        printf("TEEC_InvokeCommand failed, result=0x%x, origin=0x%x\n", result, origin);
        goto tag_exit_do_write_usr_slot_g1_2;
    }

tag_exit_do_write_usr_slot_g1_2:
    TEEC_CloseSession(&session);
tag_exit_do_write_usr_slot_g1_1:
    TEEC_FinalizeContext(&context);

    return result;
}


TEEC_Result do_read_usr_slot_g1(unsigned char *content, unsigned int length,
    unsigned int index, unsigned int *locked)
{
    TEEC_Context context;
    TEEC_Session session;
    TEEC_Operation operation;
    TEEC_Result result;
    TEEC_UUID svc_id = PTA_AMBA_OTP_UUID;
    uint32_t origin;

    result = TEEC_InitializeContext(NULL, &context);
    if (result != TEEC_SUCCESS) {
        printf("TEEC_InitializeContext fail, result=0x%x\n", result);
        return result;
    }

    result = TEEC_OpenSession(&context, &session, &svc_id,
        TEEC_LOGIN_PUBLIC, NULL, NULL, &origin);
    if (result != TEEC_SUCCESS) {
        printf("TEEC_OpenSession failed, result=0x%x, origin=0x%x\n", result, origin);
        goto tag_exit_do_read_usr_slot_g1_1;
    }

    memset(&operation, 0x0, sizeof(operation));
    operation.started = 1;
    operation.paramTypes = TEEC_PARAM_TYPES(TEEC_MEMREF_TEMP_OUTPUT, TEEC_VALUE_INOUT,
        TEEC_NONE, TEEC_NONE);
    operation.params[0].tmpref.size = length;
    operation.params[0].tmpref.buffer = content;
    operation.params[1].value.a = index;

    result = TEEC_InvokeCommand(&session,
        AMBA_OTP_CMD_GET_USR_SLOT_G1, &operation, &origin);
    if (result != TEEC_SUCCESS) {
        printf("TEEC_InvokeCommand fail, result=0x%x, origin=0x%x\n", result, origin);
        goto tag_exit_do_read_usr_slot_g1_2;
    }
    *locked = operation.params[1].value.b;

tag_exit_do_read_usr_slot_g1_2:
    TEEC_CloseSession(&session);
tag_exit_do_read_usr_slot_g1_1:
    TEEC_FinalizeContext(&context);

    return result;
}

TEEC_Result do_write_test_region(unsigned char *content, unsigned int length,
    unsigned int lock, unsigned int simulate)
{
    TEEC_Context context;
    TEEC_Session session;
    TEEC_Operation operation;
    TEEC_Result result;
    TEEC_UUID svc_id = PTA_AMBA_OTP_UUID;
    uint32_t origin;

    result = TEEC_InitializeContext(NULL, &context);
    if (result != TEEC_SUCCESS) {
        printf("TEEC_InitializeContext failed, result=0x%x\n", result);
        return result;
    }

    result = TEEC_OpenSession(&context, &session, &svc_id,
        TEEC_LOGIN_PUBLIC, NULL, NULL, &origin);
    if (result != TEEC_SUCCESS) {
        printf("TEEC_OpenSession failed, result=0x%x, origin=0x%x\n", result, origin);
        goto tag_exit_do_write_test_region_1;
    }

    memset(&operation, 0x0, sizeof(operation));
    operation.started = 1;
    operation.paramTypes = TEEC_PARAM_TYPES(TEEC_MEMREF_TEMP_INPUT, TEEC_VALUE_INPUT,
        TEEC_NONE, TEEC_NONE);
    operation.params[0].tmpref.size = length;
    operation.params[0].tmpref.buffer = content;
    operation.params[1].value.a = lock;
    operation.params[1].value.b = simulate;

    result = TEEC_InvokeCommand(&session,
        AMBA_OTP_CMD_SET_TEST_REGION, &operation, &origin);
    if (result != TEEC_SUCCESS) {
        printf("TEEC_InvokeCommand failed, result=0x%x, origin=0x%x\n", result, origin);
        goto tag_exit_do_write_test_region_2;
    }

tag_exit_do_write_test_region_2:
    TEEC_CloseSession(&session);
tag_exit_do_write_test_region_1:
    TEEC_FinalizeContext(&context);

    return result;
}


TEEC_Result do_read_test_region(unsigned char *content, unsigned int length,
    unsigned int *locked)
{
    TEEC_Context context;
    TEEC_Session session;
    TEEC_Operation operation;
    TEEC_Result result;
    TEEC_UUID svc_id = PTA_AMBA_OTP_UUID;
    uint32_t origin;

    result = TEEC_InitializeContext(NULL, &context);
    if (result != TEEC_SUCCESS) {
        printf("TEEC_InitializeContext failed, result=0x%x\n", result);
        return result;
    }

    result = TEEC_OpenSession(&context, &session, &svc_id,
        TEEC_LOGIN_PUBLIC, NULL, NULL, &origin);
    if (result != TEEC_SUCCESS) {
        printf("TEEC_OpenSession failed, result=0x%x, origin=0x%x\n", result, origin);
        goto tag_exit_do_read_test_region_1;
    }

    memset(&operation, 0x0, sizeof(operation));
    operation.started = 1;
    operation.paramTypes = TEEC_PARAM_TYPES(TEEC_MEMREF_TEMP_OUTPUT, TEEC_VALUE_INOUT,
        TEEC_NONE, TEEC_NONE);
    operation.params[0].tmpref.size = length;
    operation.params[0].tmpref.buffer = content;

    result = TEEC_InvokeCommand(&session,
        AMBA_OTP_CMD_GET_TEST_REGION, &operation, &origin);
    if (result != TEEC_SUCCESS) {
        printf("TEEC_InvokeCommand failed, result=0x%x, origin=0x%x\n", result, origin);
        goto tag_exit_do_read_test_region_2;
    }
    *locked = operation.params[1].value.a;

tag_exit_do_read_test_region_2:
    TEEC_CloseSession(&session);
tag_exit_do_read_test_region_1:
    TEEC_FinalizeContext(&context);

    return result;
}


TEEC_Result do_query_otp_setting(otp_setting_t *p_setting)
{
    TEEC_Context context;
    TEEC_Session session;
    TEEC_Operation operation;
    TEEC_Result result;
    TEEC_UUID svc_id = PTA_AMBA_OTP_UUID;
    uint32_t origin;

    result = TEEC_InitializeContext(NULL, &context);
    if (result != TEEC_SUCCESS) {
        printf("TEEC_InitializeContext failed, result=0x%x\n", result);
        return result;
    }

    result = TEEC_OpenSession(&context, &session, &svc_id,
        TEEC_LOGIN_PUBLIC, NULL, NULL, &origin);
    if (result != TEEC_SUCCESS) {
        printf("TEEC_OpenSession failed, result=0x%x, origin=0x%x\n", result, origin);
        goto tag_exit_do_query_otp_setting_1;
    }

    memset(&operation, 0x0, sizeof(operation));
    operation.started = 1;
    operation.paramTypes = TEEC_PARAM_TYPES(TEEC_MEMREF_TEMP_OUTPUT, TEEC_NONE,
        TEEC_NONE, TEEC_NONE);
    operation.params[0].tmpref.size = sizeof(otp_setting_t);
    operation.params[0].tmpref.buffer = p_setting;

    result = TEEC_InvokeCommand(&session,
        AMBA_OTP_CMD_QUERY_OTP_SETTING, &operation, &origin);
    if (result != TEEC_SUCCESS) {
        printf("TEEC_InvokeCommand failed, result=0x%x, origin=0x%x\n", result, origin);
        goto tag_exit_do_query_otp_setting_2;
    }

tag_exit_do_query_otp_setting_2:
    TEEC_CloseSession(&session);
tag_exit_do_query_otp_setting_1:
    TEEC_FinalizeContext(&context);

    return result;
}


TEEC_Result do_set_jtag_efuse(unsigned int simulate)
{
    TEEC_Context context;
    TEEC_Session session;
    TEEC_Operation operation;
    TEEC_Result result;
    TEEC_UUID svc_id = PTA_AMBA_OTP_UUID;
    uint32_t origin;

    result = TEEC_InitializeContext(NULL, &context);
    if (result != TEEC_SUCCESS) {
        printf("TEEC_InitializeContext failed, result=0x%x\n", result);
        return result;
    }

    result = TEEC_OpenSession(&context, &session, &svc_id,
        TEEC_LOGIN_PUBLIC, NULL, NULL, &origin);
    if (result != TEEC_SUCCESS) {
        printf("TEEC_OpenSession failed, result=0x%x, origin=0x%x\n", result, origin);
        goto tag_exit_do_set_jtag_efuse_1;
    }

    memset(&operation, 0x0, sizeof(operation));
    operation.started = 1;
    operation.paramTypes = TEEC_PARAM_TYPES(TEEC_VALUE_INPUT, TEEC_NONE,
        TEEC_NONE, TEEC_NONE);
    operation.params[0].value.a = simulate;

    result = TEEC_InvokeCommand(&session,
        AMBA_OTP_CMD_SET_JTAG_EFUSE, &operation, &origin);
    if (result != TEEC_SUCCESS) {
        printf("TEEC_InvokeCommand failed, result=0x%x, origin=0x%x\n", result, origin);
        goto tag_exit_do_set_jtag_efuse_2;
    }

tag_exit_do_set_jtag_efuse_2:
    TEEC_CloseSession(&session);
tag_exit_do_set_jtag_efuse_1:
    TEEC_FinalizeContext(&context);

    return result;
}


TEEC_Result do_lock_zone_a(unsigned int simulate)
{
    TEEC_Context context;
    TEEC_Session session;
    TEEC_Operation operation;
    TEEC_Result result;
    TEEC_UUID svc_id = PTA_AMBA_OTP_UUID;
    uint32_t origin;

    result = TEEC_InitializeContext(NULL, &context);
    if (result != TEEC_SUCCESS) {
        printf("TEEC_InitializeContext failed, result=0x%x\n", result);
        return result;
    }

    result = TEEC_OpenSession(&context, &session, &svc_id,
        TEEC_LOGIN_PUBLIC, NULL, NULL, &origin);
    if (result != TEEC_SUCCESS) {
        printf("TEEC_OpenSession failed, result=0x%x, origin=0x%x\n", result, origin);
        goto tag_exit_do_lock_zone_a_1;
    }

    memset(&operation, 0x0, sizeof(operation));
    operation.started = 1;
    operation.paramTypes = TEEC_PARAM_TYPES(TEEC_VALUE_INPUT, TEEC_NONE,
        TEEC_NONE, TEEC_NONE);
    operation.params[0].value.a = simulate;

    result = TEEC_InvokeCommand(&session,
        AMBA_OTP_CMD_LOCK_ZONA_A, &operation, &origin);
    if (result != TEEC_SUCCESS) {
        printf("TEEC_InvokeCommand failed, result=0x%x, origin=0x%x\n", result, origin);
        goto tag_exit_do_lock_zone_a_2;
    }

tag_exit_do_lock_zone_a_2:
    TEEC_CloseSession(&session);
tag_exit_do_lock_zone_a_1:
    TEEC_FinalizeContext(&context);

    return result;
}


TEEC_Result do_enable_bst_anti_rollback(unsigned int simulate)
{
    TEEC_Context context;
    TEEC_Session session;
    TEEC_Operation operation;
    TEEC_Result result;
    TEEC_UUID svc_id = PTA_AMBA_OTP_UUID;
    uint32_t origin;

    result = TEEC_InitializeContext(NULL, &context);
    if (result != TEEC_SUCCESS) {
        printf("TEEC_InitializeContext failed, result=0x%x\n", result);
        return result;
    }

    result = TEEC_OpenSession(&context, &session, &svc_id,
        TEEC_LOGIN_PUBLIC, NULL, NULL, &origin);
    if (result != TEEC_SUCCESS) {
        printf("TEEC_OpenSession failed, result=0x%x, origin=0x%x\n", result, origin);
        goto tag_exit_do_enable_bst_anti_rollback_1;
    }

    memset(&operation, 0x0, sizeof(operation));
    operation.started = 1;
    operation.paramTypes = TEEC_PARAM_TYPES(TEEC_VALUE_INPUT, TEEC_NONE,
        TEEC_NONE, TEEC_NONE);
    operation.params[0].value.a = simulate;

    result = TEEC_InvokeCommand(&session,
        AMBA_OTP_CMD_EN_ANTI_ROLLBACK, &operation, &origin);
    if (result != TEEC_SUCCESS) {
        printf("TEEC_InvokeCommand failed, result=0x%x, origin=0x%x\n", result, origin);
        goto tag_exit_do_enable_bst_anti_rollback_2;
    }

tag_exit_do_enable_bst_anti_rollback_2:
    TEEC_CloseSession(&session);
tag_exit_do_enable_bst_anti_rollback_1:
    TEEC_FinalizeContext(&context);

    return result;
}


TEEC_Result do_lock_rot_pubkey(unsigned int index, unsigned int simulate)
{
    TEEC_Context context;
    TEEC_Session session;
    TEEC_Operation operation;
    TEEC_Result result;
    TEEC_UUID svc_id = PTA_AMBA_OTP_UUID;
    uint32_t origin;

    result = TEEC_InitializeContext(NULL, &context);
    if (result != TEEC_SUCCESS) {
        printf("TEEC_InitializeContext failed, result=0x%x\n", result);
        return result;
    }

    result = TEEC_OpenSession(&context, &session, &svc_id,
        TEEC_LOGIN_PUBLIC, NULL, NULL, &origin);
    if (result != TEEC_SUCCESS) {
        printf("TEEC_OpenSession failed, result=0x%x, origin=0x%x\n", result, origin);
        goto tag_exit_do_lock_rot_pubkey_1;
    }

    memset(&operation, 0x0, sizeof(operation));
    operation.started = 1;
    operation.paramTypes = TEEC_PARAM_TYPES(TEEC_VALUE_INPUT, TEEC_NONE,
        TEEC_NONE, TEEC_NONE);
    operation.params[0].value.a = index;
    operation.params[0].value.b = simulate;

    result = TEEC_InvokeCommand(&session,
        AMBA_OTP_CMD_LOCK_PUKEY, &operation, &origin);
    if (result != TEEC_SUCCESS) {
        printf("TEEC_InvokeCommand failed, result=0x%x, origin=0x%x\n", result, origin);
        goto tag_exit_do_lock_rot_pubkey_2;
    }

tag_exit_do_lock_rot_pubkey_2:
    TEEC_CloseSession(&session);
tag_exit_do_lock_rot_pubkey_1:
    TEEC_FinalizeContext(&context);

    return result;
}


TEEC_Result do_write_usr_data_g0(unsigned char *content, unsigned int length,
    unsigned int index, unsigned int simulate)
{
    TEEC_Context context;
    TEEC_Session session;
    TEEC_Operation operation;
    TEEC_Result result;
    TEEC_UUID svc_id = PTA_AMBA_OTP_UUID;
    uint32_t origin;

    result = TEEC_InitializeContext(NULL, &context);
    if (result != TEEC_SUCCESS) {
        printf("TEEC_InitializeContext failed, result=0x%x\n", result);
        return result;
    }

    result = TEEC_OpenSession(&context, &session, &svc_id,
        TEEC_LOGIN_PUBLIC, NULL, NULL, &origin);
    if (result != TEEC_SUCCESS) {
        printf("TEEC_OpenSession failed, result=0x%x, origin=0x%x\n", result, origin);
        goto tag_exit_do_write_usr_data_g0_1;
    }

    memset(&operation, 0x0, sizeof(operation));
    operation.started = 1;
    operation.paramTypes = TEEC_PARAM_TYPES(TEEC_MEMREF_TEMP_INPUT, TEEC_VALUE_INPUT,
        TEEC_NONE, TEEC_NONE);
    operation.params[0].tmpref.size = length;
    operation.params[0].tmpref.buffer = content;
    operation.params[1].value.a = index;
    operation.params[1].value.b = simulate;

    result = TEEC_InvokeCommand(&session,
        AMBA_OTP_CMD_SET_USR_DATA_G0, &operation, &origin);
    if (result != TEEC_SUCCESS) {
        printf("TEEC_InvokeCommand failed, result=0x%x, origin=0x%x\n", result, origin);
        goto tag_exit_do_write_usr_data_g0_2;
    }

tag_exit_do_write_usr_data_g0_2:
    TEEC_CloseSession(&session);
tag_exit_do_write_usr_data_g0_1:
    TEEC_FinalizeContext(&context);

    return result;
}


TEEC_Result do_read_usr_data_g0(unsigned char *content, unsigned int length,
    unsigned int index)
{
    TEEC_Context context;
    TEEC_Session session;
    TEEC_Operation operation;
    TEEC_Result result;
    TEEC_UUID svc_id = PTA_AMBA_OTP_UUID;
    uint32_t origin;

    result = TEEC_InitializeContext(NULL, &context);
    if (result != TEEC_SUCCESS) {
        printf("TEEC_InitializeContext failed, result=0x%x\n", result);
        return result;
    }

    result = TEEC_OpenSession(&context, &session, &svc_id,
        TEEC_LOGIN_PUBLIC, NULL, NULL, &origin);
    if (result != TEEC_SUCCESS) {
        printf("TEEC_OpenSession failed, result=0x%x, origin=0x%x\n", result, origin);
        goto tag_exit_do_read_usr_data_g0_1;
    }

    memset(&operation, 0x0, sizeof(operation));
    operation.started = 1;
    operation.paramTypes = TEEC_PARAM_TYPES(TEEC_MEMREF_TEMP_OUTPUT, TEEC_VALUE_INOUT,
        TEEC_NONE, TEEC_NONE);
    operation.params[0].tmpref.size = length;
    operation.params[0].tmpref.buffer = content;
    operation.params[1].value.a = index;

    result = TEEC_InvokeCommand(&session,
        AMBA_OTP_CMD_GET_USR_DATA_G0, &operation, &origin);
    if (result != TEEC_SUCCESS) {
        printf("TEEC_InvokeCommand failed, result=0x%x, origin=0x%x\n", result, origin);
        goto tag_exit_do_read_usr_data_g0_2;
    }

tag_exit_do_read_usr_data_g0_2:
    TEEC_CloseSession(&session);
tag_exit_do_read_usr_data_g0_1:
    TEEC_FinalizeContext(&context);

    return result;
}


TEEC_Result do_write_sysconfig(unsigned char *content, unsigned int length,
    unsigned int lock, unsigned int simulate)
{
    TEEC_Context context;
    TEEC_Session session;
    TEEC_Operation operation;
    TEEC_Result result;
    TEEC_UUID svc_id = PTA_AMBA_OTP_UUID;
    uint32_t origin;

    result = TEEC_InitializeContext(NULL, &context);
    if (result != TEEC_SUCCESS) {
        printf("TEEC_InitializeContext failed, result=0x%x\n", result);
        return result;
    }

    result = TEEC_OpenSession(&context, &session, &svc_id,
        TEEC_LOGIN_PUBLIC, NULL, NULL, &origin);
    if (result != TEEC_SUCCESS) {
        printf("TEEC_OpenSession failed, result=0x%x, origin=0x%x\n", result, origin);
        goto tag_exit_do_write_sysconfig_1;
    }

    memset(&operation, 0x0, sizeof(operation));
    operation.started = 1;
    operation.paramTypes = TEEC_PARAM_TYPES(TEEC_MEMREF_TEMP_INPUT, TEEC_VALUE_INPUT,
        TEEC_NONE, TEEC_NONE);
    operation.params[0].tmpref.size = length;
    operation.params[0].tmpref.buffer = content;
    operation.params[1].value.a = lock;
    operation.params[1].value.b = simulate;

    result = TEEC_InvokeCommand(&session,
        AMBA_OTP_CMD_SET_SYSCONFIG, &operation, &origin);
    if (result != TEEC_SUCCESS) {
        printf("TEEC_InvokeCommand failed, result=0x%x, origin=0x%x\n", result, origin);
        goto tag_exit_do_write_sysconfig_2;
    }

tag_exit_do_write_sysconfig_2:
    TEEC_CloseSession(&session);
tag_exit_do_write_sysconfig_1:
    TEEC_FinalizeContext(&context);

    return result;
}


TEEC_Result do_read_sysconfig(unsigned char *content, unsigned int length,
    unsigned int *locked)
{
    TEEC_Context context;
    TEEC_Session session;
    TEEC_Operation operation;
    TEEC_Result result;
    TEEC_UUID svc_id = PTA_AMBA_OTP_UUID;
    uint32_t origin;

    result = TEEC_InitializeContext(NULL, &context);
    if (result != TEEC_SUCCESS) {
        printf("TEEC_InitializeContext failed, result=0x%x\n", result);
        return result;
    }

    result = TEEC_OpenSession(&context, &session, &svc_id,
        TEEC_LOGIN_PUBLIC, NULL, NULL, &origin);
    if (result != TEEC_SUCCESS) {
        printf("TEEC_OpenSession failed, result=0x%x, origin=0x%x\n", result, origin);
        goto tag_exit_do_read_sysconfig_1;
    }

    memset(&operation, 0x0, sizeof(operation));
    operation.started = 1;
    operation.paramTypes = TEEC_PARAM_TYPES(TEEC_MEMREF_TEMP_OUTPUT, TEEC_VALUE_OUTPUT,
        TEEC_NONE, TEEC_NONE);
    operation.params[0].tmpref.size = length;
    operation.params[0].tmpref.buffer = content;

    result = TEEC_InvokeCommand(&session,
        AMBA_OTP_CMD_GET_SYSCONFIG, &operation, &origin);
    if (result != TEEC_SUCCESS) {
        printf("TEEC_InvokeCommand failed, result=0x%x, origin=0x%x\n", result, origin);
        goto tag_exit_do_read_sysconfig_2;
    }
    *locked = operation.params[1].value.a;

tag_exit_do_read_sysconfig_2:
    TEEC_CloseSession(&session);
tag_exit_do_read_sysconfig_1:
    TEEC_FinalizeContext(&context);

    return result;
}


TEEC_Result do_write_cst_seed_cuk(unsigned char *content, unsigned int length,
    unsigned int lock, unsigned int simulate)
{
    TEEC_Context context;
    TEEC_Session session;
    TEEC_Operation operation;
    TEEC_Result result;
    TEEC_UUID svc_id = PTA_AMBA_OTP_UUID;
    uint32_t origin;

    result = TEEC_InitializeContext(NULL, &context);
    if (result != TEEC_SUCCESS) {
        printf("TEEC_InitializeContext failed, result=0x%x\n", result);
        return result;
    }

    result = TEEC_OpenSession(&context, &session, &svc_id,
        TEEC_LOGIN_PUBLIC, NULL, NULL, &origin);
    if (result != TEEC_SUCCESS) {
        printf("TEEC_OpenSession failed, result=0x%x, origin=0x%x\n", result, origin);
        goto tag_exit_do_write_cst_seed_cuk_1;
    }

    memset(&operation, 0x0, sizeof(operation));
    operation.started = 1;
    operation.paramTypes = TEEC_PARAM_TYPES(TEEC_MEMREF_TEMP_INPUT, TEEC_VALUE_INPUT,
        TEEC_NONE, TEEC_NONE);
    operation.params[0].tmpref.size = length;
    operation.params[0].tmpref.buffer = content;
    operation.params[1].value.a = lock;
    operation.params[1].value.b = simulate;

    result = TEEC_InvokeCommand(&session,
        AMBA_OTP_CMD_SET_CST_SEED_CUK, &operation, &origin);
    if (result != TEEC_SUCCESS) {
        printf("TEEC_InvokeCommand failed, result=0x%x, origin=0x%x\n", result, origin);
        goto tag_exit_do_write_cst_seed_cuk_2;
    }

tag_exit_do_write_cst_seed_cuk_2:
    TEEC_CloseSession(&session);
tag_exit_do_write_cst_seed_cuk_1:
    TEEC_FinalizeContext(&context);

    return result;
}


TEEC_Result do_read_cst_seed(unsigned char *content, unsigned int length,
    unsigned int *locked)
{
    TEEC_Context context;
    TEEC_Session session;
    TEEC_Operation operation;
    TEEC_Result result;
    TEEC_UUID svc_id = PTA_AMBA_OTP_UUID;
    uint32_t origin;

    result = TEEC_InitializeContext(NULL, &context);
    if (result != TEEC_SUCCESS) {
        printf("TEEC_InitializeContext failed, result=0x%x\n", result);
        return result;
    }

    result = TEEC_OpenSession(&context, &session, &svc_id,
        TEEC_LOGIN_PUBLIC, NULL, NULL, &origin);
    if (result != TEEC_SUCCESS) {
        printf("TEEC_OpenSession failed, result=0x%x, origin=0x%x\n", result, origin);
        goto tag_exit_do_read_cst_seed_1;
    }

    memset(&operation, 0x0, sizeof(operation));
    operation.started = 1;
    operation.paramTypes = TEEC_PARAM_TYPES(TEEC_MEMREF_TEMP_OUTPUT, TEEC_VALUE_OUTPUT,
        TEEC_NONE, TEEC_NONE);
    operation.params[0].tmpref.size = length;
    operation.params[0].tmpref.buffer = content;

    result = TEEC_InvokeCommand(&session,
        AMBA_OTP_CMD_GET_CST_SEED, &operation, &origin);
    if (result != TEEC_SUCCESS) {
        printf("TEEC_InvokeCommand failed, result=0x%x, origin=0x%x\n", result, origin);
        goto tag_exit_do_read_cst_seed_2;
    }
    *locked = operation.params[1].value.a;

tag_exit_do_read_cst_seed_2:
    TEEC_CloseSession(&session);
tag_exit_do_read_cst_seed_1:
    TEEC_FinalizeContext(&context);

    return result;
}

TEEC_Result do_read_cst_cuk(unsigned char *content, unsigned int length,
    unsigned int *locked)
{
    TEEC_Context context;
    TEEC_Session session;
    TEEC_Operation operation;
    TEEC_Result result;
    TEEC_UUID svc_id = PTA_AMBA_OTP_UUID;
    uint32_t origin;

    result = TEEC_InitializeContext(NULL, &context);
    if (result != TEEC_SUCCESS) {
        printf("TEEC_InitializeContext failed, result=0x%x\n", result);
        return result;
    }

    result = TEEC_OpenSession(&context, &session, &svc_id,
        TEEC_LOGIN_PUBLIC, NULL, NULL, &origin);
    if (result != TEEC_SUCCESS) {
        printf("TEEC_OpenSession failed, result=0x%x, origin=0x%x\n", result, origin);
        goto tag_exit_do_read_cst_cuk_1;
    }

    memset(&operation, 0x0, sizeof(operation));
    operation.started = 1;
    operation.paramTypes = TEEC_PARAM_TYPES(TEEC_MEMREF_TEMP_OUTPUT, TEEC_VALUE_OUTPUT,
        TEEC_NONE, TEEC_NONE);
    operation.params[0].tmpref.size = length;
    operation.params[0].tmpref.buffer = content;

    result = TEEC_InvokeCommand(&session,
        AMBA_OTP_CMD_GET_CST_CUK, &operation, &origin);
    if (result != TEEC_SUCCESS) {
        printf("TEEC_InvokeCommand failed, result=0x%x, origin=0x%x\n", result, origin);
        goto tag_exit_do_read_cst_cuk_2;
    }
    *locked = operation.params[1].value.a;

tag_exit_do_read_cst_cuk_2:
    TEEC_CloseSession(&session);
tag_exit_do_read_cst_cuk_1:
    TEEC_FinalizeContext(&context);

    return result;
}


TEEC_Result do_write_usr_cuk(unsigned char *content, unsigned int length,
    unsigned int lock, unsigned int simulate)
{
    TEEC_Context context;
    TEEC_Session session;
    TEEC_Operation operation;
    TEEC_Result result;
    TEEC_UUID svc_id = PTA_AMBA_OTP_UUID;
    uint32_t origin;

    result = TEEC_InitializeContext(NULL, &context);
    if (result != TEEC_SUCCESS) {
        printf("TEEC_InitializeContext failed, result=0x%x\n", result);
        return result;
    }

    result = TEEC_OpenSession(&context, &session, &svc_id,
        TEEC_LOGIN_PUBLIC, NULL, NULL, &origin);
    if (result != TEEC_SUCCESS) {
        printf("TEEC_OpenSession failed, result=0x%x, origin=0x%x\n", result, origin);
        goto tag_exit_do_write_usr_cuk_1;
    }

    memset(&operation, 0x0, sizeof(operation));
    operation.started = 1;
    operation.paramTypes = TEEC_PARAM_TYPES(TEEC_MEMREF_TEMP_INPUT, TEEC_VALUE_INPUT,
        TEEC_NONE, TEEC_NONE);
    operation.params[0].tmpref.size = length;
    operation.params[0].tmpref.buffer = content;
    operation.params[1].value.a = lock;
    operation.params[1].value.a = simulate;

    result = TEEC_InvokeCommand(&session,
        AMBA_OTP_CMD_SET_USR_CUK, &operation, &origin);
    if (result != TEEC_SUCCESS) {
        printf("TEEC_InvokeCommand failed, result=0x%x, origin=0x%x\n", result, origin);
        goto tag_exit_do_write_usr_cuk_2;
    }

tag_exit_do_write_usr_cuk_2:
    TEEC_CloseSession(&session);
tag_exit_do_write_usr_cuk_1:
    TEEC_FinalizeContext(&context);

    return result;
}


TEEC_Result do_read_usr_cuk(unsigned char *content, unsigned int length,
    unsigned int *locked)
{
    TEEC_Context context;
    TEEC_Session session;
    TEEC_Operation operation;
    TEEC_Result result;
    TEEC_UUID svc_id = PTA_AMBA_OTP_UUID;
    uint32_t origin;

    result = TEEC_InitializeContext(NULL, &context);
    if (result != TEEC_SUCCESS) {
        printf("TEEC_InitializeContext failed, result=0x%x\n", result);
        return result;
    }

    result = TEEC_OpenSession(&context, &session, &svc_id,
        TEEC_LOGIN_PUBLIC, NULL, NULL, &origin);
    if (result != TEEC_SUCCESS) {
        printf("TEEC_OpenSession failed, result=0x%x, origin=0x%x\n", result, origin);
        goto tag_exit_do_read_usr_cuk_1;
    }

    memset(&operation, 0x0, sizeof(operation));
    operation.started = 1;
    operation.paramTypes = TEEC_PARAM_TYPES(TEEC_MEMREF_TEMP_OUTPUT, TEEC_VALUE_OUTPUT,
        TEEC_NONE, TEEC_NONE);
    operation.params[0].tmpref.size = length;
    operation.params[0].tmpref.buffer = content;

    result = TEEC_InvokeCommand(&session,
        AMBA_OTP_CMD_GET_USR_CUK, &operation, &origin);
    if (result != TEEC_SUCCESS) {
        printf("TEEC_InvokeCommand failed, result=0x%x, origin=0x%x\n", result, origin);
        goto tag_exit_do_read_usr_cuk_2;
    }
    *locked = operation.params[1].value.a;

tag_exit_do_read_usr_cuk_2:
    TEEC_CloseSession(&session);
tag_exit_do_read_usr_cuk_1:
    TEEC_FinalizeContext(&context);

    return result;
}


TEEC_Result do_disable_secure_usb_boot(unsigned int simulate)
{
    TEEC_Context context;
    TEEC_Session session;
    TEEC_Operation operation;
    TEEC_Result result;
    TEEC_UUID svc_id = PTA_AMBA_OTP_UUID;
    uint32_t origin;

    result = TEEC_InitializeContext(NULL, &context);
    if (result != TEEC_SUCCESS) {
        printf("TEEC_InitializeContext failed, result=0x%x\n", result);
        return result;
    }

    result = TEEC_OpenSession(&context, &session, &svc_id,
        TEEC_LOGIN_PUBLIC, NULL, NULL, &origin);
    if (result != TEEC_SUCCESS) {
        printf("TEEC_OpenSession failed, result=0x%x, origin=0x%x\n", result, origin);
        goto tag_exit_do_disable_secure_usb_boot_1;
    }

    memset(&operation, 0x0, sizeof(operation));
    operation.started = 1;
    operation.paramTypes = TEEC_PARAM_TYPES(TEEC_VALUE_INPUT, TEEC_NONE,
        TEEC_NONE, TEEC_NONE);
    operation.params[0].value.a = simulate;

    result = TEEC_InvokeCommand(&session,
        AMBA_OTP_CMD_DIS_SECURE_USB_BOOT, &operation, &origin);
    if (result != TEEC_SUCCESS) {
        printf("TEEC_InvokeCommand failed, result=0x%x, origin=0x%x\n", result, origin);
        goto tag_exit_do_disable_secure_usb_boot_2;
    }

tag_exit_do_disable_secure_usb_boot_2:
    TEEC_CloseSession(&session);
tag_exit_do_disable_secure_usb_boot_1:
    TEEC_FinalizeContext(&context);

    return result;
}


TEEC_Result do_increase_bst_version(unsigned int simulate)
{
    TEEC_Context context;
    TEEC_Session session;
    TEEC_Operation operation;
    TEEC_Result result;
    TEEC_UUID svc_id = PTA_AMBA_OTP_UUID;
    uint32_t origin;

    result = TEEC_InitializeContext(NULL, &context);
    if (result != TEEC_SUCCESS) {
        printf("TEEC_InitializeContext failed, result=0x%x\n", result);
        return result;
    }

    result = TEEC_OpenSession(&context, &session, &svc_id,
        TEEC_LOGIN_PUBLIC, NULL, NULL, &origin);
    if (result != TEEC_SUCCESS) {
        printf("TEEC_OpenSession failed, result=0x%x, origin=0x%x\n", result, origin);
        goto tag_exit_do_increase_bst_version_1;
    }

    memset(&operation, 0x0, sizeof(operation));
    operation.started = 1;
    operation.paramTypes = TEEC_PARAM_TYPES(TEEC_VALUE_INPUT, TEEC_NONE,
        TEEC_NONE, TEEC_NONE);
    operation.params[0].value.a = simulate;

    result = TEEC_InvokeCommand(&session,
        AMBA_OTP_CMD_INCREASE_BST_VER, &operation, &origin);
    if (result != TEEC_SUCCESS) {
        printf("TEEC_InvokeCommand failed, result=0x%x, origin=0x%x\n", result, origin);
        goto tag_exit_do_increase_bst_version_2;
    }

tag_exit_do_increase_bst_version_2:
    TEEC_CloseSession(&session);
tag_exit_do_increase_bst_version_1:
    TEEC_FinalizeContext(&context);

    return result;
}


TEEC_Result do_read_bst_version(unsigned int *version)
{
    TEEC_Context context;
    TEEC_Session session;
    TEEC_Operation operation;
    TEEC_Result result;
    TEEC_UUID svc_id = PTA_AMBA_OTP_UUID;
    uint32_t origin;

    result = TEEC_InitializeContext(NULL, &context);
    if (result != TEEC_SUCCESS) {
        printf("TEEC_InitializeContext failed, result=0x%x\n", result);
        return result;
    }

    result = TEEC_OpenSession(&context, &session, &svc_id,
        TEEC_LOGIN_PUBLIC, NULL, NULL, &origin);
    if (result != TEEC_SUCCESS) {
        printf("TEEC_OpenSession failed, result=0x%x, origin=0x%x\n", result, origin);
        goto tag_exit_do_read_bst_version_1;
    }

    memset(&operation, 0x0, sizeof(operation));
    operation.started = 1;
    operation.paramTypes = TEEC_PARAM_TYPES(TEEC_VALUE_OUTPUT, TEEC_NONE,
        TEEC_NONE, TEEC_NONE);

    result = TEEC_InvokeCommand(&session,
        AMBA_OTP_CMD_GET_BST_VER, &operation, &origin);
    if (result != TEEC_SUCCESS) {
        printf("TEEC_InvokeCommand failed, result=0x%x, origin=0x%x\n", result, origin);
        goto tag_exit_do_read_bst_version_2;
    }
    *version = operation.params[0].value.a;

tag_exit_do_read_bst_version_2:
    TEEC_CloseSession(&session);
tag_exit_do_read_bst_version_1:
    TEEC_FinalizeContext(&context);

    return result;
}

TEEC_Result do_read_misc_config(unsigned int *config, unsigned int *total_bits)
{
    TEEC_Context context;
    TEEC_Session session;
    TEEC_Operation operation;
    TEEC_Result result;
    TEEC_UUID svc_id = PTA_AMBA_OTP_UUID;
    uint32_t origin;

    result = TEEC_InitializeContext(NULL, &context);
    if (result != TEEC_SUCCESS) {
        printf("TEEC_InitializeContext failed, result=0x%x\n", result);
        return result;
    }

    result = TEEC_OpenSession(&context, &session, &svc_id,
        TEEC_LOGIN_PUBLIC, NULL, NULL, &origin);
    if (result != TEEC_SUCCESS) {
        printf("TEEC_OpenSession failed, result=0x%x, origin=0x%x\n", result, origin);
        goto tag_exit_do_read_misc_config_1;
    }

    memset(&operation, 0x0, sizeof(operation));
    operation.started = 1;
    operation.paramTypes = TEEC_PARAM_TYPES(TEEC_VALUE_OUTPUT, TEEC_NONE,
        TEEC_NONE, TEEC_NONE);

    result = TEEC_InvokeCommand(&session,
        AMBA_OTP_CMD_GET_MISC_CINFIG, &operation, &origin);
    if (result != TEEC_SUCCESS) {
        printf("TEEC_InvokeCommand failed, result=0x%x, origin=0x%x\n", result, origin);
        goto tag_exit_do_read_misc_config_2;
    }
    *config = operation.params[0].value.a;
    *total_bits = operation.params[0].value.b;

tag_exit_do_read_misc_config_2:
    TEEC_CloseSession(&session);
tag_exit_do_read_misc_config_1:
    TEEC_FinalizeContext(&context);

    return result;
}


TEEC_Result do_write_misc_config(unsigned int config, unsigned int simulate)
{
    TEEC_Context context;
    TEEC_Session session;
    TEEC_Operation operation;
    TEEC_Result result;
    TEEC_UUID svc_id = PTA_AMBA_OTP_UUID;
    uint32_t origin;

    result = TEEC_InitializeContext(NULL, &context);
    if (result != TEEC_SUCCESS) {
        printf("TEEC_InitializeContext failed, result=0x%x\n", result);
        return result;
    }

    result = TEEC_OpenSession(&context, &session, &svc_id,
        TEEC_LOGIN_PUBLIC, NULL, NULL, &origin);
    if (result != TEEC_SUCCESS) {
        printf("TEEC_OpenSession failed, result=0x%x, origin=0x%x\n", result, origin);
        goto tag_exit_do_write_misc_config_1;
    }

    memset(&operation, 0x0, sizeof(operation));
    operation.started = 1;
    operation.paramTypes = TEEC_PARAM_TYPES(TEEC_VALUE_INPUT, TEEC_NONE,
        TEEC_NONE, TEEC_NONE);
    operation.params[0].value.a = config;
    operation.params[0].value.b = simulate;

    result = TEEC_InvokeCommand(&session,
        AMBA_OTP_CMD_SET_MISC_CINFIG, &operation, &origin);
    if (result != TEEC_SUCCESS) {
        printf("TEEC_InvokeCommand failed, result=0x%x, origin=0x%x\n", result, origin);
        goto tag_exit_do_write_misc_config_2;
    }

tag_exit_do_write_misc_config_2:
    TEEC_CloseSession(&session);
tag_exit_do_write_misc_config_1:
    TEEC_FinalizeContext(&context);

    return result;
}

#define OTP_API_VER_MAJOR 1
#define OTP_API_VER_MINOR 0
#define OTP_API_VER_PATCH 0

#define OTP_DRV_VER_MAJOR 1
#define OTP_DRV_VER_MINOR 0
#define OTP_DRV_VER_PATCH 0

#define OTP_MODI_DATE_YEAR 2022
#define OTP_MODI_DATE_MONTH 3
#define OTP_MODI_DATE_DAY 15

int do_query_version(otp_version_t *version)
{
    if (!version) {
        printf("null version\n");
        return (-1);
    }

	version->api_ver_major = OTP_API_VER_MAJOR;
	version->api_ver_minor = OTP_API_VER_MINOR;
	version->api_ver_patch = OTP_API_VER_PATCH;

	version->driver_ver_major = OTP_DRV_VER_MAJOR;
	version->driver_ver_minor = OTP_DRV_VER_MINOR;
	version->driver_ver_patch = OTP_DRV_VER_PATCH;

	version->date_year = OTP_MODI_DATE_YEAR;
	version->date_month = OTP_MODI_DATE_MONTH;
	version->date_day = OTP_MODI_DATE_DAY;

    return 0;
}

