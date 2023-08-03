// SPDX-License-Identifier: BSD-2-Clause
/*
 * Copyright (c) 2016, Ambarella Inc. All rights reserved.
 * All rights reserved.
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
#include <tee/uuid.h>
#include <console.h>
#include <mm/core_memprot.h>
#include <sm/optee_smc.h>
#include <kernel/tee_common_otp.h>
#include <crypto/crypto.h>

#include <smc_define.h>

TEE_Result tee_otp_get_hw_unique_key(struct tee_hw_unique_key *hwkey)
{
	static int already_get_hw_unique_encryption_key = 0;
	static unsigned char hw_unique_encryption_key[HW_UNIQUE_KEY_LENGTH] __aligned(64) = {
		0x12, 0xfd, 0x43, 0x83, 0xde, 0x2a, 0xc4, 0xf5,
		0xe6, 0xc7, 0x81, 0xfe, 0xa5, 0x5d, 0xb6, 0x3c,
	};
	int ret = 0;
	int already_programed = 1;

	/* use cached huk to speed up, access otp is slow, we should prevent frequently access otp */
	if (already_get_hw_unique_encryption_key) {
		memcpy(&hwkey->data[0], hw_unique_encryption_key, sizeof(hwkey->data));
		return TEE_SUCCESS;
	}

	IMSG("first time access HUK");

	ret = thread_smc(OPTEE_AMBA_SIP_OTP_GET_HUK,
			virt_to_phys(hw_unique_encryption_key), HW_UNIQUE_KEY_LENGTH, 0);
	if (ret < 0) {
		IMSG("access HUK in OTP failed, generate HUK.");
		already_programed = 0;
	}

	if (!already_programed) {
		IMSG("auto program HUK at first time");

		/* automatically generate & program HUK if it's not programed yet */
		/* to remove pontential risk of non-consistent of HUK */
		static unsigned char hw_huk_nonce[32] __aligned(64) = {
			0x12, 0xfd, 0x43, 0x83, 0xde, 0x2a, 0xc4, 0xf5,
			0xe6, 0xc7, 0x81, 0xfe, 0xa5, 0x5d, 0xb6, 0x3c,
			0x12, 0xfd, 0x43, 0x83, 0xde, 0x2a, 0xc4, 0xf5,
			0xe6, 0xc7, 0x81, 0xfe, 0xa5, 0x5d, 0xb6, 0x3c,
		};
		if (TEE_SUCCESS == crypto_rng_read(hw_huk_nonce, sizeof(hw_huk_nonce))) {
			ret = thread_smc(OPTEE_AMBA_SIP_OTP_SET_HUK_NONCE,
				virt_to_phys(hw_huk_nonce), sizeof(hw_huk_nonce), 0);
			if (ret < 0) {
				EMSG("write HUK in OTP failed.\n");
				return TEE_ERROR_GENERIC;
			}
		} else {
			EMSG("access TRNG failed.");
            return TEE_ERROR_GENERIC;
		}
		memcpy(&hwkey->data[0], hw_huk_nonce, HW_UNIQUE_KEY_LENGTH);
	} else {
		memcpy(&hwkey->data[0], hw_unique_encryption_key, sizeof(hw_unique_encryption_key));
	}

	already_get_hw_unique_encryption_key = 1;

	return TEE_SUCCESS;
}

int tee_otp_get_die_id(uint8_t *buffer, size_t len)
{
	static const char pattern[4] = { 'b', 'e', 'e', 'f' };
	size_t i;

	for (i = 0; i < len; i++)
		buffer[i] = pattern[i % 4];

	return 0;
}

