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
#include <stdio.h>

#include "tee_client_api.h"

#include "secure_credential_provisioning_ca_gen_key.h"
#include "secure_credential_provisioning_ca_read_pub_key.h"
#include "secure_credential_provisioning_ca_digi_sign.h"

int main(int argc, char *argv[])
{

    unsigned int x_len = 32;
    unsigned int y_len = 32;
    char x_output[32];
    char y_output[32];
    printf("%d, %p\n", argc, argv);

    char data[] = {
        0x6d, 0xa6, 0xa7, 0x9b, 0x4f, 0xe6, 0xb2, 0x2f, 0x77, 0x2b, 0x40, 0x9d, 0x7d, 0xdf, 0x02, 0x9a,
        0x9c, 0x15, 0xc3, 0x84, 0xb5, 0x90, 0x2f, 0x5d, 0x7a, 0xe2, 0xef, 0x3e, 0xec, 0x18, 0x60, 0x26,
    };
    unsigned int data_len = sizeof(data);

    secure_credential_provisioning_grenrate_ecdsa_key();
    read_pub_key(x_len, x_output, y_len, y_output);
    digi_sign(data_len, data);
    return 0;
}

