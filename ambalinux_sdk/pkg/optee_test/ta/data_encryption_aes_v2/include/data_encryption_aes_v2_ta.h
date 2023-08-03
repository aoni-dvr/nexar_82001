/*******************************************************************************
 * data_encryption_aes_ta_v2.h
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

#ifndef MOUDLE_DATA_ENCRYPTION_AES_V2_TA_H_
#define MOUDLE_DATA_ENCRYPTION_AES_V2_TA_H_

typedef struct
{
     char* in_buf;
     char* out_buf;
     char* key;
     char* key2; //key for AES-XTS
     char* iv;
     unsigned int data_len;
     unsigned int key_len;
     unsigned int iv_len;
     unsigned int algorithm_id;
     unsigned int out_len;
     unsigned int process_len;

     TEE_OperationHandle operation;
     TEE_OperationMode mode;
} aes_operation_t;

extern int data_encryption_aes_ta_v2_sha512_oper(char* input, unsigned int inlen, char* output);

extern int data_encryption_aes_v2_ta_init_oper(aes_operation_t *op);

extern int data_encryption_aes_v2_ta_update_oper(aes_operation_t *op);

extern int data_encryption_aes_v2_ta_final_oper(aes_operation_t *op);
#endif
