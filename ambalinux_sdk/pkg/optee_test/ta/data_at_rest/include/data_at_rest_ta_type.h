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
#ifndef MOUDLE_DATA_AT_REST_TA_TYPE_H_
#define MOUDLE_DATA_AT_REST_TA_TYPE_H_

//93793b28-69fa-11e8-adc0-fa7ae01bbebc

#define DATA_AT_REST_UUID {0x93793b28, 0x69fa, 0x11e8, \
	{ \
		0xad, 0xc0, 0xfa, 0x7a, 0xe0, 0x1b, 0xbe, 0xbc \
	} \
}

/* Define the command ID */
#define CMD_GEN_DEVICE_KEY	1U
#define CMD_ENCRYPT			3U
#define CMD_DECRYPT			4U
#define CMD_FREE_DEVICE_KEY	5U

/* Define the debug flag */
#define DEBUG
#define DLOG    MSG_RAW
//#define DLOG    ta_debug

typedef struct
{
    char* in_buf;
    char* out_buf;
    char* iv;
    unsigned int data_len;
    unsigned int key_len;
    unsigned int iv_len;
	unsigned int algorithm_id;
} aes_operation_t;

typedef enum
{
    E_AES_MODE_INVALID = 0,
    E_AES_MODE_CBC = 1,
    E_AES_MODE_ECB = 2,
    E_AES_MODE_CTR = 3,
    E_AES_MODE_CBC_CTS = 4,
} E_AES_MODE;

typedef struct
{
    E_AES_MODE mode;
} aes_operation_info_t;


#define UNUSED(x) (void)(x)

#define OK 0
#define FAIL -1

#endif