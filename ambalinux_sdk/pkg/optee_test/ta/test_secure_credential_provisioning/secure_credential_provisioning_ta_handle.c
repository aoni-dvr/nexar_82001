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
#include <stdlib.h>
#include "tee_internal_api.h"

#include "secure_credential_provisioning_ta_sha256.h"
#include "secure_credential_provisioning_ta_type.h"
#include "secure_credential_provisioning_ta_digi_sign.h"
#include "secure_credential_provisioning_ta_read_pub_key.h"
#include "secure_credential_provisioning_ta_generate_key.h"

#include "secure_credential_provisioning_ta_handle.h"

int secure_credential_provisioning_ta_generate_ecdsa_key(void)
{
    secure_credential_provisioning_ta_generate_ecdsa_key_oper( );

    DLOG("generate ECDSA key done!\n");
    return OK;

}

int secure_credential_provisioning_ta_read_pub_key(unsigned int paramTypes, TEE_Param params[4])
{
    char* ECDSA_public_key_X_buf;
    char* ECDSA_public_key_Y_buf;
    unsigned int X_len = 0U;
    unsigned int Y_len = 0U;

    UNUSED(paramTypes);

    DLOG("start read pubkey!\n");
    ECDSA_public_key_X_buf = params[0].memref.buffer;
    ECDSA_public_key_Y_buf = params[1].memref.buffer;
    X_len = params[0].memref.size;
    Y_len = params[1].memref.size;

    secure_credential_provisioning_ta_read_pub_key_oper(ECDSA_public_key_X_buf, X_len, ECDSA_public_key_Y_buf, Y_len);

    return OK;
}

int secure_credential_provisioning_ta_digi_sign(unsigned int paramTypes, TEE_Param params[4])
{
    char* input_data = NULL;
    char* signed_data = NULL;
    unsigned int input_len = 0U;

    UNUSED(paramTypes);

    input_data = params[0].memref.buffer;
    input_len = params[0].memref.size;

    //printf("%u , %u\n" , input_len , output_len);
    signed_data  = (char*)malloc(sizeof(int) * 512);

    secure_credential_provisioning_ta_digi_sign_oper(input_len, input_data, signed_data);

    return OK;
}
