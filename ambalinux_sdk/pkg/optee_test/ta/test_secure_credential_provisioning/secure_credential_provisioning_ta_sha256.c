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
#include "tee_internal_api.h"
#include "tee_api_defines_extensions.h"

#include "secure_credential_provisioning_ta_sha256.h"
#include "secure_credential_provisioning_ta_type.h"

static void TA_Printf(char* buf, unsigned int len)
{
    unsigned int index = 0U;
    for(index = 0U; index < len; index++)
    {
        if(index < 15U)
        {
        }
        else if(0U == index%16U)
        {
            DLOG("\n");
        }
        else
        {
        }

        DLOG("0x%02x, ", (buf[index] & 0xFFU));

    }
    DLOG("\n\n");
}


int secure_credential_provisioning_ta_sha256_oper(char* input, unsigned int inlen, char* output, unsigned int* outlen)
{
	TEE_Result result = 0;
	TEE_OperationHandle operationhandle;
	int Retval = OK;

	result = TEE_AllocateOperation(&operationhandle, TEE_ALG_SHA256, TEE_MODE_DIGEST, 0);
	if(result != TEE_SUCCESS)
	{
		DLOG("Allocate SHA operation handle fail\n");
		Retval = FAIL;
		goto cleanup_1;
	}

	TEE_DigestUpdate(operationhandle, input, inlen);

	result = TEE_DigestDoFinal(operationhandle, NULL, 0, output, outlen);
    DLOG("The output length is :%d\n", *outlen);
    DLOG("The return value is :0x%x\n", result);
	if(result != TEE_SUCCESS)
	{
		DLOG("Do the final sha operation fail\n");
		Retval = FAIL;
		goto cleanup_2;
	}

	DLOG("Hash value just like follow:\n");
	TA_Printf(output, *outlen);

	cleanup_2:
		TEE_FreeOperation(operationhandle);
	cleanup_1:
		return Retval;
}
