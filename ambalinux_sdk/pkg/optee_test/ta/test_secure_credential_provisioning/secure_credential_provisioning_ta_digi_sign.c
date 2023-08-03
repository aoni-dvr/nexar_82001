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
#include <tee_api.h>
#include <tee_api_defines.h>

#include "secure_credential_provisioning_ta_digi_sign.h"
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

int secure_credential_provisioning_ta_digi_sign_oper(unsigned int len, char* input, char* output)
{
	TEE_ObjectHandle key;
	TEE_Result result = FAIL;
	TEE_OperationHandle ECDSA_operation_handle = NULL;
    char hash[32];
	unsigned int outlen = 32U;

	//char ECDSA_public_key_X_buf[32];
	//unsigned int X_len = 32U;

	uint32_t objectID = 1;
	uint32_t ECDSA_KEY_ID = TEE_STORAGE_PRIVATE;
	uint32_t flags = TEE_DATA_FLAG_ACCESS_READ |
			 TEE_DATA_FLAG_ACCESS_WRITE |
			 TEE_DATA_FLAG_ACCESS_WRITE_META |
			 TEE_DATA_FLAG_SHARE_READ |
			 TEE_DATA_FLAG_SHARE_WRITE |
			 TEE_DATA_FLAG_OVERWRITE;

	//TA_Printf(input, len);
	DLOG("%p \n", output );
	DLOG("%p \n", input );

	secure_credential_provisioning_ta_sha256_oper(input, len, hash, &outlen);
    //DLOG("The output hash value is\n");
    //TA_Printf(hash, outlen);
    outlen = 64U;

	result = TEE_OpenPersistentObject(ECDSA_KEY_ID, &objectID, sizeof(objectID), flags, &key);
	if (result != TEE_SUCCESS) {
		DLOG("Failed to open persistent key: 0x%x", result);
        goto cleanup_1;
	}

#if 0
	result = TEE_GetObjectBufferAttribute(key, TEE_ATTR_ECC_PUBLIC_VALUE_X , ECDSA_public_key_X_buf, &X_len);
	if(TEE_SUCCESS != result)
	{
		DLOG("Get Public key X fail, Ret value is:0x%x\n", result);
		result = FAIL;
	}
	DLOG("ECDSA public key X value:\n");
    TA_Printf(ECDSA_public_key_X_buf, X_len);

	result = TEE_GetObjectBufferAttribute(key, TEE_ATTR_ECC_PUBLIC_VALUE_Y , ECDSA_public_key_X_buf, &X_len);
	if(TEE_SUCCESS != result)
	{
		DLOG("Get Public key Y fail, Ret value is:0x%x\n", result);
		result = FAIL;
	}
	DLOG("ECDSA public key Y value:\n");
    TA_Printf(ECDSA_public_key_X_buf, X_len);

	result = TEE_GetObjectBufferAttribute(key, TEE_ATTR_ECC_PRIVATE_VALUE , ECDSA_public_key_X_buf, &X_len);
	if(TEE_SUCCESS != result)
	{
		DLOG("Get private key fail, Ret value is:0x%x\n", result);
		result = FAIL;
	}
	DLOG("ECDSA private key value:\n");
    TA_Printf(ECDSA_public_key_X_buf, X_len);
#endif


	if(key == TEE_HANDLE_NULL)
	{
		DLOG("ECDSA key not exist!\n");
		result = FAIL;
	}

    result = TEE_AllocateOperation(&ECDSA_operation_handle, TEE_ALG_ECDSA_P256, TEE_MODE_SIGN, 256);
    if(TEE_SUCCESS != result)
    {
        DLOG("The allocate operate handle fail, the return value is: 0x%x\n", result);
        result = FAIL;
        goto cleanup_2;
    }

    result = TEE_SetOperationKey(ECDSA_operation_handle, key);
    if(TEE_SUCCESS != result)
    {
        DLOG("Set operation key fail, return value is:0x%x\n", result);
        result = FAIL;
        goto cleanup_1;
    }

    result = TEE_AsymmetricSignDigest(ECDSA_operation_handle, NULL, 0, hash, 32, output, &outlen);
    if(result != TEE_SUCCESS)
    {
    	DLOG("Sign digest fail, the return value is: 0x%x\n", result);
        return result;
    }
    DLOG("The Sign output len:%d\n", outlen);
    TA_Printf(output, outlen);

cleanup_1:
    TEE_FreeOperation(ECDSA_operation_handle);
    ECDSA_operation_handle = TEE_HANDLE_NULL;
    TEE_CloseObject(key);
cleanup_2:
    return result;

}

