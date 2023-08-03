/*******************************************************************************
 * optee_tls_ta_type.h
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

#include <tee_internal_api.h>
#include <tee_internal_api_extensions.h>

#include <pta_socket.h>
#include <tee_isocket.h>
#include <tee_tcpsocket.h>

#include "mbedtls/ssl.h"

//889d1c5e-c0b9-4407-badc-727b6ce65c45

#define OPTEE_TLS_TEST_UUID {0x889d1c5e, 0xc0b9, 0x4407, \
    { \
        0xba, 0xdc, 0x72, 0x7b, 0x6c, 0xe6, 0x5c, 0x45 \
    } \
}

/* Define the comman ID */
#define CMD_OPEN                        1U
#define CMD_CLOSE                       2U
#define CMD_WRITE                       3U
#define CMD_READ                        4U

/* Define the debug flag */
#define DEBUG
#define DLOG    MSG_RAW
//#define DLOG    ta_debug

#define UNUSED(x) (void)(x)

typedef struct
{
    int socket_handle;
    TEE_TASessionHandle session;
} optee_tls_socket_handle;

typedef struct
{
    optee_tls_socket_handle *socket_handle;

    mbedtls_ssl_context *ssl;
} optee_tls_client_context;

