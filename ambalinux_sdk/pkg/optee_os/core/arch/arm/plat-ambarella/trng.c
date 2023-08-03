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

#include <console.h>
#include <mm/core_memprot.h>
#include <sm/optee_smc.h>
#include <crypto/crypto.h>
#include <rng_support.h>

#include <drivers/ambarella_trng.h>

unsigned char hw_get_random_byte(void)
{
	static int read_byte_index = 16, max_bytes = 16;
	static unsigned int random_data[4] = {
		0x0,
	};
	int ret = 0;
	unsigned char ret_v = 0x0;

	if (read_byte_index >= max_bytes) {
		ret = ambarella_trng_read((unsigned int *) random_data,
			sizeof(random_data));
		if (ret < 0) {
			EMSG("access TRNG fail");
			return 0x0;
		}
		read_byte_index = 0;
	}

	ret_v = ((unsigned char *) random_data)[read_byte_index];

	read_byte_index ++;

	return ret_v;
}


