/*******************************************************************************
 * data_encryption_aes_v2_ta_type.h
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


#ifndef MOUDLE_DATA_ENCRYPTION_AES_V2_TA_TYPE_H_
#define MOUDLE_DATA_ENCRYPTION_AES_V2_TA_TYPE_H_

#include "tee_api_types.h"

//fd1a7e05-50f7-475b-850d-472cd1a474b0

#define DATA_ENCRYPTION_AES_V2_UUID_ID {0xfd1a7e05, 0x50f7, 0x475b, \
    { \
        0x85, 0x0d, 0x47, 0x2c, 0xd1, 0xa4, 0x74, 0xb0 \
    } \
}

/* Define the comman ID */
#define CMD_AES_INIT            1U
#define CMD_AES_INIT_PASSWORD   2U
#define CMD_AES_UPDATE          3U
#define CMD_AES_FINAL           4U

/* Define the debug flag */
#define DEBUG
#define DLOG    MSG_RAW
//#define DLOG    ta_debug

#define UNUSED(x) (void)(x)

/* AES operation type */
typedef enum
{
    E_AES_OP_INVALID = 0,
    E_AES_OP_ENCRYPT = 1,
    E_AES_OP_DECRYPT = 2,
} E_AES_OPERATION;

/* AES mode type */
typedef enum
{
    E_AES_MODE_INVALID = 0,
    E_AES_MODE_CBC = 1,
    E_AES_MODE_ECB = 2,
    E_AES_MODE_CTR = 3,
    E_AES_MODE_XTS = 4,
} E_AES_MODE;

typedef struct
{
    E_AES_OPERATION action;
    E_AES_MODE mode;
} aes_operation_info_t;

#endif


