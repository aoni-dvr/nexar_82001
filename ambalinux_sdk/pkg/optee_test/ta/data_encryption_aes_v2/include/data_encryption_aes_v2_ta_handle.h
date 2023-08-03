/*******************************************************************************
 * data_encryption_aes_v2_ta_handle.h
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

#ifndef MOUDLE_ENCRYPTION_AES_V2_TA_HANDLE_H_
#define MOUDLE_ENCRYPTION_AES_V2_TA_HANDLE_H_

extern int data_encryption_aes_v2_ta_init(unsigned int paramTypes, TEE_Param params[4]);

extern int data_encryption_aes_v2_ta_init_password(unsigned int paramTypes, TEE_Param params[4]);

extern int data_encryption_aes_v2_ta_update(unsigned int paramTypes, TEE_Param params[4]);

extern int data_encryption_aes_v2_ta_final(unsigned int paramTypes, TEE_Param params[4]);

#endif

