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
#define CMD_TRUNCATE_OPER          4U     /**< Command ID of truncate operation in secure storage */
#define CMD_RENAME_OPER            5U     /**< Command ID of rename operation in secure storage */
#define CMD_DELETE_OPER               6U     /**< Command ID of close and delete operation in secure storage */
#define CMD_GET_LEN_OPER           7U     /**< Command ID of get secure file length operation in secure storage */

/* Define the UUID of this TA */
#define TA_SEC_STORAGE_TEST_UUID \
    { 0x59e4d3d3, 0x0199, 0x4f74, \
        { \
            0xb9, 0x4d, 0x53, 0xd3, 0xda, 0xa5, 0x7d, 0x73 \
        } \
    }

