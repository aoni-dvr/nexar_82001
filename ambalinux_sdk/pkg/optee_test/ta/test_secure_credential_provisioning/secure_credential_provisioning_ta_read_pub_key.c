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

#include "secure_credential_provisioning_ta_type.h"

#include "secure_credential_provisioning_ta_read_pub_key.h"

int secure_credential_provisioning_ta_read_pub_key_oper(char* pub_key_x, unsigned int x_len, char* pub_key_y, unsigned int y_len)
{
	TEE_Result result;
	uint32_t objectID = 1;
	uint32_t ECDSA_KEY_ID = TEE_STORAGE_PRIVATE;
	uint32_t flags = TEE_DATA_FLAG_ACCESS_READ |
			 TEE_DATA_FLAG_ACCESS_WRITE |
			 TEE_DATA_FLAG_ACCESS_WRITE_META |
			 TEE_DATA_FLAG_SHARE_READ |
			 TEE_DATA_FLAG_SHARE_WRITE |
			 TEE_DATA_FLAG_OVERWRITE;

	TEE_ObjectHandle ECDSA_key_1 = (TEE_ObjectHandle)NULL;

	result = TEE_OpenPersistentObject(ECDSA_KEY_ID,
					  &objectID, sizeof(objectID),
					  flags, &ECDSA_key_1);
	if (result != TEE_SUCCESS) {
		DLOG("Failed to open persistent key: 0x%x", result);
	}

	if (ECDSA_key_1 == TEE_HANDLE_NULL)
	{
		DLOG("ECDSA key not exist!\n");
		return FAIL;
	}

	result = TEE_GetObjectBufferAttribute(ECDSA_key_1, TEE_ATTR_ECC_PUBLIC_VALUE_X , pub_key_x, &x_len);
	if(TEE_SUCCESS != result)
	{
		DLOG("Get Public key X fail, Ret value is:0x%x\n", result);
		result = FAIL;
	}

	result = TEE_GetObjectBufferAttribute(ECDSA_key_1, TEE_ATTR_ECC_PUBLIC_VALUE_Y , pub_key_y, &y_len);
	if(TEE_SUCCESS != result)
	{
		DLOG("Get Public key Y fail, Ret value is:0x%x\n", result);
		result = FAIL;
	}

	TEE_CloseObject(ECDSA_key_1);

	return result;

}
