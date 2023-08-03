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

#define TEE_EXEC_FAIL   0x0000FFFF

#define DLOG    MSG_RAW

#define UNUSED(x) (void)(x)

/* Define the command index in this TA(Get Aes boot key) */
#define CMD_CREATE_OPER            1U     /**< Command ID of read operation in secure storage */
#define CMD_READ_OPER              2U     /**< Command ID of read operation in secure storage */
#define CMD_WRITE_OPER             3U     /**< Command ID of write operation in secure storage */
#define CMD_RENAME_OPER            4U     /**< Command ID of rename operation in secure storage */
#define CMD_DELETE_OPER            5U     /**< Command ID of close and delete operation in secure storage */
#define CMD_READ_TEST_OPER         6U

enum {
    ESecurestorageOperation_Invalid = 0x00,
    ESecurestorageOperation_StoreBuffer = 0x01,
    ESecurestorageOperation_LoadToBuffer = 0x02,
    ESecurestorageOperation_Delete = 0x03,
};

/* Define the UUID of this TA */
#define SEC_STORAGE_SIMPLE_TEST_UUID \
    { 0xecb2fab9, 0x4806, 0x4126, \
        { \
          0x9c, 0x00, 0x20, 0x54, 0xc5, 0x93, 0x4f, 0x01 \
        } \
    }

extern int secure_storage_simple_test_ca_create_object(const char *secure_object);

extern int secure_storage_simple_test_ca_load_object(const char *secure_object_name,
        unsigned int outbuf_size, char *output);

extern int secure_storage_simple_test_ca_load_2buffer(const char *secure_object_name,
        unsigned int outbuf_1_size, char *output_1, unsigned int outbuf_2_size, char *output_2);

extern int secure_storage_simple_test_ca_store_buffer(const char *buffer, const char *secure_object_name);

extern int secure_storage_simple_test_ca_delete_object(const char *secure_object_name);
