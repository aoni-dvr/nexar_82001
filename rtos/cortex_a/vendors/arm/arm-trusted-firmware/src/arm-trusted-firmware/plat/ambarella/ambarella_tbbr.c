/*
 * Copyright (c) 2017, ARM Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#include <plat/common/platform.h>
#include <common/debug.h>
#include <lib/mmio.h>
#include <plat_private.h>
#include <ambarella_def.h>
#include <platform_def.h>

extern char ambarella_rotpk[], ambarella_rotpk_raw[], ambarella_rotpk_end[];
extern char ambarella_embeded_rotpk_der[], ambarella_embeded_rotpk_der_end[];

#if defined(PLAT_CFG_ATF_EMBED_PUB_COT_ROOT)
static int plat_get_embedded_rotpk_info(
			void **key_ptr, unsigned int *key_len,
			unsigned int *flags)
{
	*key_ptr = ambarella_embeded_rotpk_der;
	*key_len = ambarella_embeded_rotpk_der_end - ambarella_embeded_rotpk_der;
	*flags = 0;
	return 0;
}
#else
/* Only apply to RSA key */
static int plat_get_otp_rotpk_info(
			void **key_ptr, unsigned int *key_len,
			unsigned int *flags)
{
	uint32_t i, lock_bit, invalid_bit, pk_n_addr, pk_n;

	if (ambarella_otp_read(WRITE_LOCK_BIT_ADDR, 32, &lock_bit) < 0)
		return -1;

	if (ambarella_otp_read(DATA_INVALID_BIT_ADDR, 32, &invalid_bit) < 0)
		return -1;

	/* The 3rd Public Key cannot be invalid. */
	invalid_bit &= ~(1 << (ROT_KEY_NUM - 1));

	for (i = 0; i < ROT_KEY_NUM; i++) {
		if (!(lock_bit & (1 << (i + LOCK_BIT_ROT_BASE)))) {
			*key_len = 0;
			*flags = ROTPK_NOT_DEPLOYED;
			WARN("The Public Key %d is NOT Locked!\n", i);
			return 0;
		}

		if (!(invalid_bit & (1 << i)))
			break;
	}

	INFO("Using the Public Key %d\n", i);

	/* RSA N plus RSA RN */
	pk_n_addr = ROT_PUBKEY_ADDR + i * ROT_PUBKEY_BITS;

#if defined(CONFIG_ATF_AMBALINK) && (defined(CONFIG_SOC_CV2FS) || defined(CONFIG_SOC_CV22FS))
	for (i = 0; i < ROT_PUBKEY_BITS; i += 64) {
		if (ambarella_otp_read(pk_n_addr + ROT_PUBKEY_BITS - 64 - i, 32, &pk_n) < 0)
			return -1;
		ambarella_rotpk_raw[i / 16 + 0] = (pk_n >> 24) & 0xff;
		ambarella_rotpk_raw[i / 16 + 1] = (pk_n >> 16) & 0xff;
		ambarella_rotpk_raw[i / 16 + 2] = (pk_n >> 8) & 0xff;
		ambarella_rotpk_raw[i / 16 + 3] = (pk_n >> 0) & 0xff;
	}
#else
	for (i = 0; i < (ROT_PUBKEY_BITS / 2); i += 32) {
		if (ambarella_otp_read(pk_n_addr + (ROT_PUBKEY_BITS / 2) - 32 - i, 32, &pk_n) < 0)
			return -1;
		ambarella_rotpk_raw[i / 8 + 0] = (pk_n >> 24) & 0xff;
		ambarella_rotpk_raw[i / 8 + 1] = (pk_n >> 16) & 0xff;
		ambarella_rotpk_raw[i / 8 + 2] = (pk_n >> 8) & 0xff;
		ambarella_rotpk_raw[i / 8 + 3] = (pk_n >> 0) & 0xff;
	}
#endif

	*key_ptr = ambarella_rotpk;
	*key_len = ambarella_rotpk_end - ambarella_rotpk;
	*flags = 0;

	return 0;
}
#endif

int plat_get_rotpk_info(void *cookie, void **key_ptr, unsigned int *key_len,
			unsigned int *flags)
{
	if (!ambarella_is_secure_boot()) {
		*key_len = 0;
		*flags = ROTPK_NOT_DEPLOYED;
		WARN("It's not Secure Boot in POC setting!\n");
		return 0;
	}

#if !defined(PLAT_CFG_ATF_EMBED_PUB_COT_ROOT)
	return plat_get_otp_rotpk_info(
			key_ptr, key_len,
			flags);
#else
	return plat_get_embedded_rotpk_info(
			key_ptr, key_len,
			flags);
#endif
}

int plat_get_nv_ctr(void *cookie, unsigned int *nv_ctr)
{
	/*
	 * No support for non-volatile counter.  Update the ROT key to protect
	 * the system against rollback.
	 */
	*nv_ctr = 0;

	return 0;
}

int plat_set_nv_ctr(void *cookie, unsigned int nv_ctr)
{
	return 0;
}

int plat_get_mbedtls_heap(void **heap_addr, size_t *heap_size)
{
	return get_mbedtls_heap_helper(heap_addr, heap_size);
}

