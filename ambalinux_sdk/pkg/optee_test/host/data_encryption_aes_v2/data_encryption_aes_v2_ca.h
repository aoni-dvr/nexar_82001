/*******************************************************************************
 * data_encryption_aes_v2_ca.c
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

#ifndef MOUDLE_DATA_ENCRYPTION_AES_V2_CA_H_
#define MOUDLE_DATA_ENCRYPTION_AES_V2_CA_H_

#include "tee_api_types.h"

typedef struct {
    TEE_OperationHandle op_handle;
    TEEC_Context context;
    TEEC_Session session;

    char *input_file_name;
    char *output_file_name;

    const char *aes_mode;
    const char *aes_oper;
    const char *aes_len;

    E_AES_MODE e_aes_mode;
    E_AES_OPERATION e_aes_oper;
    unsigned int e_aes_len;

    unsigned int process_len;
} data_encryption_aes_v2_test_context;

extern TEEC_Result aes_init(data_encryption_aes_v2_test_context* ctx,
                   char *key, char *key2);

extern TEEC_Result aes_init_password(data_encryption_aes_v2_test_context* ctx,
                   char *password, unsigned int password_len);

extern TEEC_Result aes_update(data_encryption_aes_v2_test_context* ctx, unsigned char* input, unsigned int in_len, unsigned char* output, unsigned int* out_len);

extern TEEC_Result aes_final(data_encryption_aes_v2_test_context* ctx, unsigned char* input, unsigned int in_len, unsigned char* output, unsigned int* out_len);

extern void free_aes_context(data_encryption_aes_v2_test_context* ctx);

#endif
