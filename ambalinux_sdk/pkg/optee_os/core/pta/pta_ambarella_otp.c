/*
 * Copyright (C) 2018 Ambarella Inc.
 * All rights reserved.
 *
 * Author: Zhi He <zhe@ambarella.com>
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

#include <compiler.h>
#include <stdio.h>
#include <trace.h>
#include <kernel/pseudo_ta.h>
#include <mm/tee_pager.h>
#include <mm/tee_mm.h>
#include <mm/core_memprot.h>
#include <string.h>
#include <string_ext.h>
#include <sm/optee_smc.h>
#include <crypto/crypto.h>
#include <malloc.h>

#include <drivers/ambarella_trng.h>

#include <smc_define.h>

/* this is for debug OTP only, it must be disabled in real product to prevent sensitive information on OTP expose to non-secure world */
//#define DEBUG_OTP

/* this is an option to expose public ROT key in OTP to non-secure world */
#define EXPOSE_OTP_PUB_ROT_KEY

/* this is an option to expose user slot group0 of OTP to non-secure world */
#define EXPOSE_OTP_USER_SLOT_GRP0

/* this is an option to expose user slot group1 of OTP to non-secure world */
#define EXPOSE_OTP_USER_SLOT_GRP1

/* this is an option to let external application program AES key */
#define WRITE_OTP_AES_KEY_FROM_EXTERNAL

/* this is an option to let external application program ECC key */
#define WRITE_OTP_ECC_KEY_FROM_EXTERNAL

#define TA_NAME		"ambarella_otp.ta"

#define PTA_AMBA_OTP_UUID \
		{ 0xabcdef12, 0x7e33, 0x4ad2, \
			{ 0x98, 0x02, 0xe6, 0x4f, 0x2a, 0x7c, 0xc2, 0x30 } }

#define AMBA_OTP_CMD_WRITE_PUBKEY   0
#define AMBA_OTP_CMD_READ_PUBKEY_DIGEST   1
#define AMBA_OTP_CMD_READ_AMBA_UNIQUE_ID   2
#define AMBA_OTP_CMD_WRITE_CUSTOMER_ID   4
#define AMBA_OTP_CMD_READ_CUSTOMER_ID   5
#define AMBA_OTP_CMD_READ_MONOTINIC_COUNTER   6
#define AMBA_OTP_CMD_INCREASE_MONOTINIC_COUNTER   7
#define AMBA_OTP_CMD_PERMANENTLY_ENABLE_SECURE_BOOT   8
#define AMBA_OTP_CMD_GEN_HW_UNIQUE_ENCRYPTION_KEY   9

#define AMBA_OTP_CMD_GEN_AES_KEY   10
#define AMBA_OTP_CMD_WRITE_AES_KEY   11

#define AMBA_OTP_CMD_GEN_ECC_KEY   12
#define AMBA_OTP_CMD_WRITE_ECC_KEY   13

#define AMBA_OTP_CMD_SET_USR_SLOT_G0   14
#define AMBA_OTP_CMD_GET_USR_SLOT_G0   15

#define AMBA_OTP_CMD_SET_USR_SLOT_G1   16
#define AMBA_OTP_CMD_GET_USR_SLOT_G1   17

#define AMBA_OTP_CMD_REVOKE_KEY   18
#define AMBA_OTP_CMD_QUERY_OTP_SETTING   20

/* optional */
#define AMBA_OTP_CMD_SET_JTAG_EFUSE   21
#define AMBA_OTP_CMD_LOCK_ZONA_A   22

/* optional */
#define AMBA_OTP_CMD_GET_TEST_REGION  23
#define AMBA_OTP_CMD_SET_TEST_REGION  24

#define AMBA_OTP_CMD_EN_ANTI_ROLLBACK  25

#define AMBA_OTP_CMD_LOCK_PUKEY  26

#define AMBA_OTP_CMD_GET_USR_DATA_G0  27
#define AMBA_OTP_CMD_SET_USR_DATA_G0  28

/* optional */
#define AMBA_OTP_CMD_READ_PUBKEY		31

/* debug only */
#define AMBA_OTP_CMD_DEBUG_SHOW_CONTENT	32

#define AMBA_OTP_CMD_GET_SYSCONFIG  33
#define AMBA_OTP_CMD_SET_SYSCONFIG  34
#define AMBA_OTP_CMD_GET_CST_SEED  35
#define AMBA_OTP_CMD_GET_CST_CUK  36
#define AMBA_OTP_CMD_SET_CST_SEED_CUK  37
#define AMBA_OTP_CMD_GET_USR_CUK  38
#define AMBA_OTP_CMD_SET_USR_CUK  39
#define AMBA_OTP_CMD_DIS_SECURE_USB_BOOT  40

#define AMBA_OTP_CMD_GET_BST_VER  41
#define AMBA_OTP_CMD_INCREASE_BST_VER  42

#define AMBA_OTP_CMD_GET_MISC_CINFIG  43
#define AMBA_OTP_CMD_SET_MISC_CINFIG  44


//from ambarella_def.h

/*
 *  OTP Zone A Lock check
 */
#if defined(ambarella_cv2) || defined(ambarella_cv22) || defined(ambarella_cv25) || defined(ambarella_cv28) || defined(ambarella_s6lm)
#define OTP_LAYOUT_V1
#elif defined (ambarella_cv5) || defined(ambarella_cv52)
#define OTP_LAYOUT_V2
#elif defined (ambarella_cv2fs) || defined(ambarella_cv22fs)
#define OTP_LAYOUT_V1
#else
#error "missing PLATFORM_ARCH=ambarella_cvxx in BR2_TARGET_OPTEE_OS_ADDITIONAL_VARIABLES"
#endif

/*
 * OTP Layout
 */

#define CHIP_REPAIR_INFO_ADDR		(0x0)		/* bit addr */
#define CHIP_REPAIR_INFO_BITS		16000

/*
 *  OTP sysconfig
 */
#define SYS_CONFIG_BITS			64

/*
 *  OTP ROT public key
 */
#if defined (OTP_LAYOUT_V1)
#define ROT_KEY_NUM			3
#define ROT_PUBKEY_BITS		4096
#elif defined (OTP_LAYOUT_V2)
#define ROT_KEY_NUM			16
#define ROT_PUBKEY_BITS		256
#endif

/*
 *  OTP HUK
 */
#define HW_UNIQUE_ENCRYPTION_KEY_BITS	128
#define HW_NONCE_BITS			128

/*
 *  OTP customer defined AES and ECC keys
 */
#define AES_KEY_NUM			4
#define ECC_KEY_NUM			4
#define AES_KEY_BITS			256
#define ECC_KEY_BITS			256

/*
 *  OTP Unique ID
 */
#define UNIQUE_ID_BITS			128

/*
 *  OTP Customer ID (Customer serial number)
 */
#define CUSTOMER_ID_BITS		128

/*
 *  OTP User Slot Group 0 (lockable)
 */
#define USR_SLOT_G0_NUM		6
#define USR_SLOT_G0_BITS		256

/*
 *  OTP User Slot Group 1 (lockable)
 */
#if defined (OTP_LAYOUT_V2)
#define USR_SLOT_G1_NUM		4
#define USR_SLOT_G1_BITS		1024
#endif

/*
 *  OTP User Data Group 0 (not lockable)
 */
#if defined (OTP_LAYOUT_V2)
#define USR_DATA_G0_NUM		3
#define USR_DATA_G0_BITS		1024
#endif

/*
 *  OTP Test Region
 */
#define TEST_REGION_BITS		128


/*
 *  other OTP fields
 */
#if defined (OTP_LAYOUT_V2)

#define CST_PLANTED_SEED_BITS		32
#define CST_PLANTED_CUK_BITS			128
#define MISC_CONFIG_BITS				14
#define BST_VER_BITS					96
#define USR_PLANTED_CUK_BITS		256
#endif


#define HASH_ALGO		TEE_ALG_SHA256
#define HASH_SIZE		32

// output flag
#define D_FLAG_LOCKED 0x01
#define D_FLAG_PARAM_ERROR 0x02
#define D_FLAG_HW_ERROR 0x08
#define D_FLAG_DATA_MISMATCH 0x04
#define D_FLAG_REVOKED 0x10

// input flag
#define D_IN_FLAG_WRITE_CONTENT 0x01
#define D_IN_FLAG_WRITE_LOCK 0x02

#define D_IN_FLAG_SIMULATE_WRITE 0x08

static TEE_Result amba_otp_write_rot_pubkey(unsigned int *p, unsigned int length,
	unsigned int key_index, unsigned int lock, unsigned int simulate)
{
	uint8_t pukey[(ROT_PUBKEY_BITS / 8) + 4] __aligned(4096);
	uint32_t *p_embed_flag = (uint32_t *) ((unsigned long) pukey + (ROT_PUBKEY_BITS / 8));
	int ret = 0;

	if (key_index >= ROT_KEY_NUM) {
		EMSG("bad key_index %d", key_index);
		return TEE_ERROR_BAD_PARAMETERS;
	}

	if (length != (ROT_PUBKEY_BITS / 8)) {
		EMSG("length %d not expected %d", length, (ROT_PUBKEY_BITS / 8));
		return TEE_ERROR_BAD_PARAMETERS;
	}

	memcpy(pukey, p, length);
	*p_embed_flag = D_IN_FLAG_WRITE_CONTENT;
	if (lock) {
		*p_embed_flag |= D_IN_FLAG_WRITE_LOCK;
	}

	if (simulate) {
		*p_embed_flag |= D_IN_FLAG_SIMULATE_WRITE;
	}

	ret = thread_smc(OPTEE_AMBA_SIP_OTP_SET_PUKEY,
			virt_to_phys(pukey), sizeof(pukey), key_index);
	if (ret < 0) {
		EMSG("write pubkey(%d) in otp failed", key_index);
		return TEE_ERROR_ACCESS_DENIED;
	}

	return TEE_SUCCESS;
}

#ifdef EXPOSE_OTP_PUB_ROT_KEY
static TEE_Result amba_otp_read_rot_pubkey(unsigned int *p, unsigned int length,
	unsigned int key_index, unsigned int *locked, unsigned int *revoked)
{
	uint8_t pukey[(ROT_PUBKEY_BITS / 8) + 4] __aligned(4096);
	uint32_t *p_embed_flag = (uint32_t *) ((unsigned long) pukey + (ROT_PUBKEY_BITS / 8));
	int ret;

	if (key_index >= ROT_KEY_NUM) {
		EMSG("bad key_index %d", key_index);
		return TEE_ERROR_BAD_PARAMETERS;
	}

	if (length != (ROT_PUBKEY_BITS / 8)) {
		EMSG("length %d not expected %d", length, (ROT_PUBKEY_BITS / 8));
		return TEE_ERROR_BAD_PARAMETERS;
	}

	ret = thread_smc(OPTEE_AMBA_SIP_OTP_GET_PUKEY,
			virt_to_phys(pukey), sizeof(pukey), key_index);
	if (ret < 0) {
		EMSG("PuKey(%d) is faild to fetched from the platform.", key_index);
		return TEE_ERROR_ACCESS_DENIED;
	}
	if ((*p_embed_flag) & D_FLAG_LOCKED) {
		*locked = 1;
	} else {
		*locked = 0;
	}
	if ((*p_embed_flag) & D_FLAG_REVOKED) {
		*revoked = 1;
	} else {
		*revoked = 0;
	}
	memcpy(p, pukey, sizeof(pukey));

	return TEE_SUCCESS;
}
#endif

static TEE_Result amba_otp_read_rot_pubkey_digest(unsigned int *p, unsigned int length,
	unsigned int key_index, unsigned int *locked, unsigned int *revoked)
{
	uint8_t pukey[(ROT_PUBKEY_BITS / 8) + 4] __aligned(4096);
	uint8_t digest[32] __aligned(4096);
	uint32_t *p_embed_flag = (uint32_t *) ((unsigned long) pukey + (ROT_PUBKEY_BITS / 8));
	int ret;
	void *hash_ctx;

	if (key_index >= ROT_KEY_NUM) {
		EMSG("bad key_index %d", key_index);
		return TEE_ERROR_BAD_PARAMETERS;
	}

	if (length != (HASH_SIZE)) {
		EMSG("length %d not expected %d", length, (HASH_SIZE));
		return TEE_ERROR_BAD_PARAMETERS;
	}

	ret = thread_smc(OPTEE_AMBA_SIP_OTP_GET_PUKEY,
			virt_to_phys(pukey), sizeof(pukey), key_index);
	if (ret < 0) {
		EMSG("PuKey(%d) is faild to fetched from the platform.", key_index);
		return TEE_ERROR_ACCESS_DENIED;
	}
	if ((*p_embed_flag) & D_FLAG_LOCKED) {
		*locked = 1;
	} else {
		*locked = 0;
	}
	if ((*p_embed_flag) & D_FLAG_REVOKED) {
		*revoked = 1;
	} else {
		*revoked = 0;
	}
	ret = crypto_hash_alloc_ctx(&hash_ctx, HASH_ALGO);
	if (ret) {
		EMSG("crypto_hash_alloc_ctx fail, ret %d\n.", ret);
		return ret;
	}
	ret = crypto_hash_init(hash_ctx);
	if (ret) {
		EMSG("crypto_hash_init fail, ret %d\n.", ret);
		return ret;
	}
	ret = crypto_hash_update(hash_ctx, (const uint8_t *)pukey, sizeof(pukey));
	if (ret) {
		EMSG("crypto_hash_update fail, ret %d\n.", ret);
		return ret;
	}
	ret = crypto_hash_final(hash_ctx, digest, HASH_SIZE);
	if (ret) {
		EMSG("crypto_hash_final fail, ret %d\n.", ret);
		return ret;
	}

	memcpy(p, digest, sizeof(digest));

	return TEE_SUCCESS;
}

static TEE_Result amba_otp_read_amba_unique_id(unsigned int *p, unsigned int length)
{
	uint8_t amba_id[UNIQUE_ID_BITS / 8] __aligned(4096);
	int ret;

	if (length != (UNIQUE_ID_BITS / 8)) {
		EMSG("length %d not expected %d", length, (UNIQUE_ID_BITS / 8));
		return TEE_ERROR_BAD_PARAMETERS;
	}

	ret = thread_smc(OPTEE_AMBA_SIP_OTP_GET_UNIQUE_ID,
			virt_to_phys(amba_id), sizeof(amba_id), 0);
	if (ret < 0) {
		EMSG("Unique ID is faild to fetched from the platform.");
		return TEE_ERROR_ACCESS_DENIED;
	}

	memcpy(p, amba_id, sizeof(amba_id));

	return TEE_SUCCESS;
}

static void __get_random_bytes(unsigned char *p_buf, unsigned int size)
{
	static int started = 0;
	uint8_t amba_id[UNIQUE_ID_BITS / 8] __aligned(4096);
	static uint8_t input[HASH_SIZE] = {0x0};
	uint8_t digest[HASH_SIZE];
	void *hash_ctx;
	static unsigned int index = 0;

	if (!started) {
		thread_smc(OPTEE_AMBA_SIP_OTP_GET_UNIQUE_ID,
			virt_to_phys(amba_id), sizeof(amba_id), 0);
		memcpy(input, amba_id, sizeof(amba_id));
		crypto_hash_alloc_ctx(&hash_ctx, HASH_ALGO);
		crypto_hash_init(hash_ctx);
		crypto_hash_update(hash_ctx, (const uint8_t *) input, sizeof(input));
		crypto_hash_final(hash_ctx, digest, HASH_SIZE);
		memcpy(input, digest, sizeof(input));
		started = 1;
	}

	while (size) {
		index = input[index & 0x1f] & 0x1f;
		input[index] = input[index] ^ 0x5c;
		crypto_hash_alloc_ctx(&hash_ctx, HASH_ALGO);
		crypto_hash_init(hash_ctx);
		crypto_hash_update(hash_ctx, (const uint8_t *) input, sizeof(input));
		crypto_hash_final(hash_ctx, digest, HASH_SIZE);
		memcpy(input, digest, sizeof(input));

		if (size >= HASH_SIZE) {
			memcpy(p_buf, digest, HASH_SIZE);
			p_buf += HASH_SIZE;
			size -= HASH_SIZE;
		} else {
			memcpy(p_buf, digest, size);
			break;
		}
	}

	return;
}

static TEE_Result amba_otp_write_customer_id(unsigned int *p, unsigned int length,
	unsigned int lock, unsigned int simulate)
{
	uint8_t customer_id[(CUSTOMER_ID_BITS / 8) + 4] __aligned(4096);
	uint32_t *p_embed_flag = (uint32_t *) ((unsigned long) customer_id + (CUSTOMER_ID_BITS / 8));
	int ret;

	if (length != (CUSTOMER_ID_BITS / 8)) {
		EMSG("length %d not expected %d", length, (CUSTOMER_ID_BITS / 8));
		return TEE_ERROR_BAD_PARAMETERS;
	}

	memcpy(customer_id, p, length);
	*p_embed_flag = D_IN_FLAG_WRITE_CONTENT;
	if (lock) {
		*p_embed_flag |= D_IN_FLAG_WRITE_LOCK;
	}

	if (simulate) {
		*p_embed_flag |= D_IN_FLAG_SIMULATE_WRITE;
	}

	ret = thread_smc(OPTEE_AMBA_SIP_OTP_SET_CUSTOMER_ID,
			virt_to_phys(customer_id), sizeof(customer_id), 0);
	if (ret < 0) {
		EMSG("Customer ID is faild to write to the platform.");
		return TEE_ERROR_ACCESS_DENIED;
	}

	return TEE_SUCCESS;
}

static TEE_Result amba_otp_read_customer_id(unsigned int *p, unsigned int length,
	unsigned int *locked)
{
	uint8_t customer_id[(CUSTOMER_ID_BITS / 8) + 4] __aligned(4096);
	uint32_t *p_embed_flag = (uint32_t *) ((unsigned long) customer_id + (CUSTOMER_ID_BITS / 8));
	int ret;

	if (length != (CUSTOMER_ID_BITS / 8)) {
		EMSG("length %d not expected %d", length, (CUSTOMER_ID_BITS / 8));
		return TEE_ERROR_BAD_PARAMETERS;
	}

	ret = thread_smc(OPTEE_AMBA_SIP_OTP_GET_CUSTOMER_ID,
			virt_to_phys(customer_id), sizeof(customer_id), 0);
	if (ret < 0) {
		EMSG("Customer ID is faild to fetched from the platform.");
		return TEE_ERROR_ACCESS_DENIED;
	}
	if ((*p_embed_flag) & D_FLAG_LOCKED) {
		*locked = 1;
	} else {
		*locked = 0;
	}

	memcpy(p, customer_id, sizeof(customer_id));

	return TEE_SUCCESS;
}

static TEE_Result amba_otp_read_monotonic_counter(unsigned int *mono_counter,
	unsigned int mono_index)
{
	uint32_t counter __aligned(4096);
	int ret;

	ret = thread_smc(OPTEE_AMBA_SIP_OTP_GET_COUNTER,
		virt_to_phys(&counter), sizeof(counter), mono_index);
	if (ret < 0) {
		EMSG("monotonic_counter is faild to fetched from the platform.");
		return TEE_ERROR_ACCESS_DENIED;
	}

	*mono_counter = counter;

	return TEE_SUCCESS;
}

static TEE_Result amba_otp_increase_monotonic_counter(
	unsigned int mono_index, unsigned int simulate)
{
	int ret;

	ret = thread_smc(OPTEE_AMBA_SIP_OTP_ADD_COUNTER, mono_index, simulate, 0);
	if (ret < 0) {
		EMSG("monotonic_counter is faild to increase.");
		return TEE_ERROR_ACCESS_DENIED;
	}

	return TEE_SUCCESS;
}

static TEE_Result amba_otp_permanently_enable_secure_boot(unsigned int simulate)
{
	int ret;

	ret = thread_smc(OPTEE_AMBA_SIP_OTP_PERMANENTLY_ENABLE_SECURE_BOOT, simulate, 0, 0);
	if (ret < 0) {
		EMSG("permanently enable secure boot failed.");
		return TEE_ERROR_ACCESS_DENIED;
	}

	return TEE_SUCCESS;
}

static TEE_Result amba_otp_generate_hw_unique_encryption_key(unsigned int simulate)
{
	int ret, i;
	int random_data_not_zero = 0;
	uint32_t random_data[8 + 1] __aligned(4096);
	uint32_t *p_embed_flag = (uint32_t *) &random_data[8];

#if 0
	ret = ambarella_trng_read(random_data,
		sizeof(random_data) - 4);
#else
	__get_random_bytes(random_data, sizeof(random_data) - 4);
#endif

	for (i = 0; i < 8; i++) {
		if (random_data[i] != 0) {
			random_data_not_zero = 1;
			break;
		}
	}

	if (!random_data_not_zero) {
		/* in theory, all zero is possible but very rare (valid data from rng), we add a print here */
		/* if several chips have this print, the trng part need be checked */
		EMSG("generate random data 0.");
	}

	if (simulate) {
		*p_embed_flag = D_IN_FLAG_SIMULATE_WRITE | D_IN_FLAG_WRITE_CONTENT | D_IN_FLAG_WRITE_LOCK;
	} else {
		*p_embed_flag = D_IN_FLAG_WRITE_CONTENT | D_IN_FLAG_WRITE_LOCK;
	}

	ret = thread_smc(OPTEE_AMBA_SIP_OTP_SET_HUK_NONCE,
		virt_to_phys(random_data), sizeof(random_data), 1);
	if (ret < 0) {
		EMSG("write huk_nonce failed.");
		return TEE_ERROR_ACCESS_DENIED;
	}

	return TEE_SUCCESS;
}

static TEE_Result amba_otp_generate_aes_key(unsigned int key_index,
	unsigned int simulate)
{
	int ret;
	uint32_t random_data[(AES_KEY_BITS / 32) + 1] __aligned(4096);
	uint32_t *p_embed_flag = (uint32_t *) &random_data[AES_KEY_BITS / 32];

	if (key_index >= AES_KEY_NUM) {
		EMSG("bad key_index %d", key_index);
		return TEE_ERROR_BAD_PARAMETERS;
	}

#if 0
	ret = ambarella_trng_read(random_data,
		sizeof(random_data) - 4);
#else
	__get_random_bytes(random_data, sizeof(random_data) - 4);
#endif

	if (simulate) {
		*p_embed_flag = D_IN_FLAG_SIMULATE_WRITE | D_IN_FLAG_WRITE_CONTENT | D_IN_FLAG_WRITE_LOCK;
	} else {
		*p_embed_flag = D_IN_FLAG_WRITE_CONTENT | D_IN_FLAG_WRITE_LOCK;
	}

	ret = thread_smc(OPTEE_AMBA_SIP_OTP_SET_AES_KEY,
		virt_to_phys(random_data), sizeof(random_data), key_index);
	if (ret < 0) {
		EMSG("write AES key failed.");
		return TEE_ERROR_ACCESS_DENIED;
	}

	return TEE_SUCCESS;
}

#ifdef WRITE_OTP_AES_KEY_FROM_EXTERNAL
static TEE_Result amba_otp_write_aes_key(unsigned int *p,
	unsigned int length, unsigned int key_index,
	unsigned int lock, unsigned int simulate)
{
	uint8_t aes_key[(AES_KEY_BITS / 8) + 4] __aligned(4096);
	uint32_t *p_embed_flag = (uint32_t *) ((unsigned long) aes_key + (AES_KEY_BITS / 8));
	int ret;

	if (key_index >= AES_KEY_NUM) {
		EMSG("bad key_index %d", key_index);
		return TEE_ERROR_BAD_PARAMETERS;
	}

	if (length != (AES_KEY_BITS / 8)) {
		EMSG("length %d not expected %d", length, (AES_KEY_BITS / 8));
		return TEE_ERROR_BAD_PARAMETERS;
	}

	memcpy(aes_key, p, length);
	*p_embed_flag = D_IN_FLAG_WRITE_CONTENT;
	if (lock) {
		*p_embed_flag |= D_IN_FLAG_WRITE_LOCK;
	}

	if (simulate) {
		*p_embed_flag |= D_IN_FLAG_SIMULATE_WRITE;
	}

	ret = thread_smc(OPTEE_AMBA_SIP_OTP_SET_AES_KEY,
			virt_to_phys(aes_key), sizeof(aes_key), key_index);
	if (ret < 0) {
		EMSG("AES key is faild to write to the platform.");
		return TEE_ERROR_ACCESS_DENIED;
	}

	return TEE_SUCCESS;
}
#endif

static TEE_Result amba_otp_generate_ecc_key(unsigned int key_index)
{
	EMSG("not implemeted yet, %d", key_index);
	return TEE_ERROR_NOT_IMPLEMENTED;
}

#ifdef WRITE_OTP_ECC_KEY_FROM_EXTERNAL
static TEE_Result amba_otp_write_ecc_key(unsigned int *p,
	unsigned int length, unsigned int key_index,
	unsigned int lock, unsigned int simulate)
{
	uint8_t ecc_key[(ECC_KEY_BITS / 8) + 4] __aligned(4096);
	uint32_t *p_embed_flag = (uint32_t *) ((unsigned long) ecc_key + (ECC_KEY_BITS / 8));
	int ret;

	if (key_index >= ECC_KEY_NUM) {
		EMSG("bad key_index %d", key_index);
		return TEE_ERROR_BAD_PARAMETERS;
	}

	if (length != (ECC_KEY_BITS / 8)) {
		EMSG("length %d not expected %d", length, (ECC_KEY_BITS / 8));
		return TEE_ERROR_BAD_PARAMETERS;
	}

	memcpy(ecc_key, p, length);
	*p_embed_flag = D_IN_FLAG_WRITE_CONTENT;
	if (lock) {
		*p_embed_flag |= D_IN_FLAG_WRITE_LOCK;
	}

	if (simulate) {
		*p_embed_flag |= D_IN_FLAG_SIMULATE_WRITE;
	}

	ret = thread_smc(OPTEE_AMBA_SIP_OTP_SET_ECC_KEY,
			virt_to_phys(ecc_key), sizeof(ecc_key), key_index);
	if (ret < 0) {
		EMSG("ECC key is faild to write to the platform.");
		return TEE_ERROR_ACCESS_DENIED;
	}

	return TEE_SUCCESS;
}
#endif

static TEE_Result amba_otp_write_user_slot_g0(unsigned int *p,
	unsigned int length, unsigned int slot_index,
	unsigned int lock, unsigned int simulate)
{
	uint8_t content[(USR_SLOT_G0_BITS / 8) + 4] __aligned(4096);
	uint32_t *p_embed_flag = (uint32_t *) ((unsigned long) content + (USR_SLOT_G0_BITS / 8));
	int ret;

	if (slot_index >= USR_SLOT_G0_NUM) {
		EMSG("bad user slot g0 index %d", slot_index);
		return TEE_ERROR_BAD_PARAMETERS;
	}

	if (length != (USR_SLOT_G0_BITS / 8)) {
		EMSG("length %d not expected %d", length, (USR_SLOT_G0_BITS / 8));
		return TEE_ERROR_BAD_PARAMETERS;
	}

	memcpy(content, p, length);
	*p_embed_flag = D_IN_FLAG_WRITE_CONTENT;
	if (lock) {
		*p_embed_flag |= D_IN_FLAG_WRITE_LOCK;
	}

	if (simulate) {
		*p_embed_flag |= D_IN_FLAG_SIMULATE_WRITE;
	}

	ret = thread_smc(OPTEE_AMBA_SIP_OTP_SET_USR_SLOT_G0,
			virt_to_phys(content), sizeof(content), slot_index);
	if (ret < 0) {
		EMSG("user slot g0 (%d) is faild to write to the platform.", slot_index);
		return TEE_ERROR_ACCESS_DENIED;
	}

	return TEE_SUCCESS;
}

#ifdef EXPOSE_OTP_USER_SLOT_GRP0
static TEE_Result amba_otp_read_user_slot_g0(unsigned int *p,
	unsigned int length, unsigned int slot_index, unsigned int *locked)
{
	uint8_t content[(USR_SLOT_G0_BITS / 8) + 4] __aligned(4096);
	uint32_t *p_embed_flag = (uint32_t *) ((unsigned long) content + (USR_SLOT_G0_BITS / 8));
	int ret;

	if (slot_index >= USR_SLOT_G0_NUM) {
		EMSG("bad reserved_dx_index %d", slot_index);
		return TEE_ERROR_BAD_PARAMETERS;
	}

	if (length != (USR_SLOT_G0_BITS / 8)) {
		EMSG("length %d not expected %d", length, (USR_SLOT_G0_BITS / 8));
		return TEE_ERROR_BAD_PARAMETERS;
	}

	ret = thread_smc(OPTEE_AMBA_SIP_OTP_GET_USR_SLOT_G0,
			virt_to_phys(content), sizeof(content), slot_index);
	if (ret < 0) {
		EMSG("reserved dx (%d) is faild to fetched from the platform.", slot_index);
		return TEE_ERROR_ACCESS_DENIED;
	}
	if ((*p_embed_flag) & D_FLAG_LOCKED) {
		*locked = 1;
	} else {
		*locked = 0;
	}

	memcpy(p, content, sizeof(content));

	return TEE_SUCCESS;
}
#endif

#ifdef OTP_LAYOUT_V2

static TEE_Result amba_otp_write_user_slot_g1(unsigned int *p, unsigned int length,
	unsigned int slot_index, unsigned int lock, unsigned int simulate)
{
	uint8_t content[(USR_SLOT_G1_BITS / 8) + 4] __aligned(4096);
	uint32_t *p_embed_flag = (uint32_t *) ((unsigned long) content + (USR_SLOT_G1_BITS / 8));
	int ret;

	if (length != (USR_SLOT_G1_BITS / 8)) {
		EMSG("length %d not expected %d", length, (USR_SLOT_G1_BITS / 8));
		return TEE_ERROR_BAD_PARAMETERS;
	}

	memcpy(content, p, length);
	*p_embed_flag = D_IN_FLAG_WRITE_CONTENT;
	if (lock) {
		*p_embed_flag |= D_IN_FLAG_WRITE_LOCK;
	}

	if (simulate) {
		*p_embed_flag |= D_IN_FLAG_SIMULATE_WRITE;
	}

	ret = thread_smc(OPTEE_AMBA_SIP_OTP_SET_USR_SLOT_G1,
			virt_to_phys(content), sizeof(content), slot_index);
	if (ret < 0) {
		EMSG("user slot g1 is faild to write to the platform.");
		return TEE_ERROR_ACCESS_DENIED;
	}

	return TEE_SUCCESS;
}

#ifdef EXPOSE_OTP_USER_SLOT_GRP1
static TEE_Result amba_otp_read_user_slot_g1(unsigned int *p, unsigned int length,
	unsigned int slot_index, unsigned int *locked)
{
	uint8_t content[(USR_SLOT_G1_BITS / 8) + 4] __aligned(4096);
	uint32_t *p_embed_flag = (uint32_t *) ((unsigned long) content + (USR_SLOT_G1_BITS / 8));
	int ret;

	if (length != (USR_SLOT_G1_BITS / 8)) {
		EMSG("length %d not expected %d", length, (USR_SLOT_G1_BITS / 8));
		return TEE_ERROR_BAD_PARAMETERS;
	}

	ret = thread_smc(OPTEE_AMBA_SIP_OTP_GET_USR_SLOT_G1,
			virt_to_phys(content), sizeof(content), slot_index);
	if (ret < 0) {
		EMSG("user slot g1 is faild to fetched from the platform.");
		return TEE_ERROR_ACCESS_DENIED;
	}
	if ((*p_embed_flag) & D_FLAG_LOCKED) {
		*locked = 1;
	} else {
		*locked = 0;
	}

	memcpy(p, content, sizeof(content));

	return TEE_SUCCESS;
}
#endif

#endif

static TEE_Result amba_otp_revoke_key(unsigned int key_index,
	unsigned int simulate)
{
	int ret;

	ret = thread_smc(OPTEE_AMBA_SIP_OTP_REVOKE_KEY, key_index, simulate, 0);
	if (ret < 0) {
		EMSG("revoke key (%d) failed.\n", key_index);
		return TEE_ERROR_ACCESS_DENIED;
	}

	return TEE_SUCCESS;
}

static TEE_Result amba_otp_write_test_region(unsigned char *p, unsigned int length,
	unsigned int lock, unsigned int simulate)
{
	uint8_t test_content[(TEST_REGION_BITS / 8) + 4] __aligned(4096);
	uint32_t *p_embed_flag = (uint32_t *) ((unsigned long) test_content + (TEST_REGION_BITS / 8));
	int ret;

	if (length != (TEST_REGION_BITS / 8)) {
		EMSG("length %d not expected for test region",
			length);
		return TEE_ERROR_BAD_PARAMETERS;
	}

	memcpy(test_content, p, length);
	*p_embed_flag = D_IN_FLAG_WRITE_CONTENT;
	if (lock) {
		*p_embed_flag |= D_IN_FLAG_WRITE_LOCK;
	}

	if (simulate) {
		*p_embed_flag |= D_IN_FLAG_SIMULATE_WRITE;
	}

	ret = thread_smc(OPTEE_AMBA_SIP_SET_TEST_REGION,
			virt_to_phys(test_content), sizeof(test_content), 0);
	if (ret < 0) {
		EMSG("Test region is faild to write to the platform.");
		return TEE_ERROR_ACCESS_DENIED;
	}

	return TEE_SUCCESS;
}

static TEE_Result amba_otp_read_test_region(unsigned int *p, unsigned int length,
	unsigned int *locked)
{
	uint8_t test_content[(TEST_REGION_BITS / 8) + 4] __aligned(4096);
	uint32_t *p_embed_flag = (uint32_t *) ((unsigned long) test_content + (TEST_REGION_BITS / 8));
	int ret;

	if (length != (TEST_REGION_BITS / 8)) {
		EMSG("length %d not expected %d", length, (TEST_REGION_BITS / 8));
		return TEE_ERROR_BAD_PARAMETERS;
	}

	ret = thread_smc(OPTEE_AMBA_SIP_GET_TEST_REGION,
			virt_to_phys(test_content), sizeof(test_content), 0);
	if (ret < 0) {
		EMSG("Customer ID is faild to fetched from the platform.");
		return TEE_ERROR_ACCESS_DENIED;
	}
	if ((*p_embed_flag) & D_FLAG_LOCKED) {
		*locked = 1;
	} else {
		*locked = 0;
	}

	memcpy(p, test_content, sizeof(test_content));

	return TEE_SUCCESS;
}

#ifdef OTP_LAYOUT_V2
static TEE_Result amba_otp_enable_bst_anti_rollback(unsigned simulate)
{
	int ret;

	ret = thread_smc(OPTEE_AMBA_SIP_EN_ANTI_ROLLBACK, simulate, 0, 0);
	if (ret < 0) {
		EMSG("enable BST anti rollback failed.");
		return TEE_ERROR_ACCESS_DENIED;
	}

	return TEE_SUCCESS;
}
#endif

//keep sync with otp driver in atf
typedef struct {
	unsigned int lock_bits;
	unsigned int invalid_bits;
	unsigned int sysconfig;
	unsigned int sysconfig_mask;

	unsigned int secure_boot_permanent_en : 1;
	unsigned int jtag_efuse : 1;
	unsigned int sysconfig_locked : 1;
	unsigned int huk_locked : 1;
	unsigned int customer_id_locked : 1;
	unsigned int zone_a_locked : 1;
	unsigned int cst_seed_cuk_locked : 1;
	unsigned int usr_cuk_locked : 1;
	unsigned int anti_rollback_en : 1;
	unsigned int secure_usb_boot_dis : 1;
	unsigned int all_rot_key_lock_together : 1;

	unsigned int otp_layout_ver : 8; // cv2x&s6lm ver = 1, cv5 ver = 2
	unsigned int not_revokeable_key_index : 8;
	unsigned int reserved : 5;

	unsigned int rot_pubkey_lock_base;
	unsigned int aes_key_lock_base;
	unsigned int ecc_key_lock_base;
	unsigned int usr_slot_g0_lock_base;
	unsigned int usr_slot_g1_lock_base;

	unsigned int num_of_rot_pub_keys;
	unsigned int num_of_aes_keys;
	unsigned int num_of_ecc_keys;
	unsigned int num_of_usr_slot_g0;
	unsigned int num_of_usr_slot_g1;
} otp_setting_t;

static TEE_Result amba_otp_query_setting(unsigned int *p, unsigned int length)
{
	otp_setting_t setting;
	int ret;

	if (length != sizeof(setting)) {
		EMSG("otp_setting size mismatch %d, %d.", (unsigned int) sizeof(setting), length);
		return TEE_ERROR_ACCESS_DENIED;
	}

	ret = thread_smc(OPTEE_AMBA_SIP_OTP_QUERY_OTP_SETTING,
			virt_to_phys(&setting), sizeof(setting), 0);
	if (ret < 0) {
		EMSG("chip setting is faild to fetched from the platform.\n");
		return TEE_ERROR_ACCESS_DENIED;
	}

	memcpy(p, &setting, sizeof(setting));

	return TEE_SUCCESS;
}

static TEE_Result amba_otp_set_jtag_efuse(unsigned simulate)
{
	int ret;

	ret = thread_smc(OPTEE_AMBA_SIP_OTP_SET_JTAG_EFUSE, simulate, 0, 0);
	if (ret < 0) {
		EMSG("set JTAG efuse fail.");
		return TEE_ERROR_ACCESS_DENIED;
	}

	return TEE_SUCCESS;
}

static TEE_Result amba_otp_lock_zone_a(unsigned simulate)
{
	int ret;

	ret = thread_smc(OPTEE_AMBA_SIP_OTP_LOCK_ZONA_A, simulate, 0, 0);
	if (ret < 0) {
		EMSG("lock Zone A failed.");
		return TEE_ERROR_ACCESS_DENIED;
	}

	return TEE_SUCCESS;
}

static TEE_Result amba_otp_lock_rot_pubkey(unsigned int key_index,
	unsigned simulate)
{
	int ret;

	ret = thread_smc(OPTEE_AMBA_SIP_OTP_LOCK_PUKEY, key_index, simulate, 0);
	if (ret < 0) {
		EMSG("lock rot pubkey (%d) failed.", key_index);
		return TEE_ERROR_ACCESS_DENIED;
	}

	return TEE_SUCCESS;
}

static TEE_Result amba_otp_write_sysconfig(unsigned int *p, unsigned int length,
	unsigned int lock, unsigned int simulate)
{
	uint8_t sysconfig[(SYS_CONFIG_BITS / 8) + 4] __aligned(4096);
	uint32_t *p_embed_flag = (uint32_t *) ((unsigned long) sysconfig + (SYS_CONFIG_BITS / 8));
	int ret;

	if (length != (SYS_CONFIG_BITS / 8)) {
		EMSG("length %d not expected %d", length, (SYS_CONFIG_BITS / 8));
		return TEE_ERROR_BAD_PARAMETERS;
	}

	memcpy(sysconfig, p, length);
	*p_embed_flag = D_IN_FLAG_WRITE_CONTENT;
	if (lock) {
		*p_embed_flag |= D_IN_FLAG_WRITE_LOCK;
	}

	if (simulate) {
		*p_embed_flag |= D_IN_FLAG_SIMULATE_WRITE;
	}

	ret = thread_smc(OPTEE_AMBA_SIP_OTP_SET_SYSCONFIG,
			virt_to_phys(sysconfig), sizeof(sysconfig), 0);
	if (ret < 0) {
		EMSG("sysconfig is faild to write to the platform.");
		return TEE_ERROR_ACCESS_DENIED;
	}

	return TEE_SUCCESS;
}

static TEE_Result amba_otp_read_sysconfig(unsigned int *p, unsigned int length,
	unsigned int *locked)
{
	uint8_t sysconfig[(SYS_CONFIG_BITS / 8) + 4] __aligned(4096);
	uint32_t *p_embed_flag = (uint32_t *) ((unsigned long) sysconfig + (SYS_CONFIG_BITS / 8));
	int ret;

	if (length != (SYS_CONFIG_BITS / 8)) {
		EMSG("length %d not expected %d", length, (SYS_CONFIG_BITS / 8));
		return TEE_ERROR_BAD_PARAMETERS;
	}

	ret = thread_smc(OPTEE_AMBA_SIP_OTP_GET_SYSCONFIG,
			virt_to_phys(sysconfig), sizeof(sysconfig), 0);
	if (ret < 0) {
		EMSG("sysconfig is faild to fetched from the platform.");
		return TEE_ERROR_ACCESS_DENIED;
	}
	if ((*p_embed_flag) & D_FLAG_LOCKED) {
		*locked = 1;
	} else {
		*locked = 0;
	}

	memcpy(p, sysconfig, sizeof(sysconfig));

	return TEE_SUCCESS;
}

#ifdef OTP_LAYOUT_V2
static TEE_Result amba_otp_write_user_data_g0(unsigned int *p, unsigned int length,
	unsigned int data_index, unsigned int simulate)
{
	uint8_t data[(USR_DATA_G0_BITS / 8) + 4] __aligned(4096);
	uint32_t *p_embed_flag = (uint32_t *) ((unsigned long) data + (USR_DATA_G0_BITS / 8));
	int ret;

	if (length != (USR_DATA_G0_BITS / 8)) {
		EMSG("length %d not expected %d", length, (USR_DATA_G0_BITS / 8));
		return TEE_ERROR_BAD_PARAMETERS;
	}

	memcpy(data, p, length);
	*p_embed_flag = D_IN_FLAG_WRITE_CONTENT;
	if (simulate) {
		*p_embed_flag |= D_IN_FLAG_SIMULATE_WRITE;
	}

	ret = thread_smc(OPTEE_AMBA_SIP_OTP_SET_USR_DATA_G0,
			virt_to_phys(data), sizeof(data), data_index);
	if (ret < 0) {
		EMSG("User Data g0 (%d) is faild to write to the platform.", data_index);
		return TEE_ERROR_ACCESS_DENIED;
	}

	return TEE_SUCCESS;
}

static TEE_Result amba_otp_read_user_data_g0(unsigned int *p, unsigned int length,
	unsigned int data_index)
{
	uint8_t data[USR_DATA_G0_BITS / 8] __aligned(4096);
	int ret;

	if (length != (USR_DATA_G0_BITS / 8)) {
		EMSG("length %d not expected %d", length, (USR_DATA_G0_BITS / 8));
		return TEE_ERROR_BAD_PARAMETERS;
	}

	ret = thread_smc(OPTEE_AMBA_SIP_OTP_GET_USR_DATA_G0,
			virt_to_phys(data), sizeof(data), data_index);
	if (ret < 0) {
		EMSG("User Data g0 (%d) is faild to fetched from the platform.", data_index);
		return TEE_ERROR_ACCESS_DENIED;
	}

	memcpy(p, data, sizeof(data));

	return TEE_SUCCESS;
}

static TEE_Result amba_otp_read_cst_seed(unsigned int *p, unsigned int length,
	unsigned int *locked)
{
	uint8_t seed[(CST_PLANTED_SEED_BITS / 8) + 4] __aligned(4096);
	uint32_t *p_embed_flag = (uint32_t *) ((unsigned long) seed + (CST_PLANTED_SEED_BITS / 8));
	int ret;

	if (length != (CST_PLANTED_SEED_BITS / 8)) {
		EMSG("length %d not expected %d", length, (CST_PLANTED_SEED_BITS / 8));
		return TEE_ERROR_BAD_PARAMETERS;
	}

	ret = thread_smc(OPTEE_AMBA_SIP_OTP_GET_CST_SEED,
			virt_to_phys(seed), sizeof(seed), 0);
	if (ret < 0) {
		EMSG("cst seed is faild to fetched from the platform.");
		return TEE_ERROR_ACCESS_DENIED;
	}
	if ((*p_embed_flag) & D_FLAG_LOCKED) {
		*locked = 1;
	} else {
		*locked = 0;
	}

	memcpy(p, seed, sizeof(seed));

	return TEE_SUCCESS;
}

static TEE_Result amba_otp_read_cst_cuk(unsigned int *p, unsigned int length,
	unsigned int *locked)
{
	uint8_t cuk[(CST_PLANTED_CUK_BITS / 8) + 4] __aligned(4096);
	uint32_t *p_embed_flag = (uint32_t *) ((unsigned long) cuk + (CST_PLANTED_SEED_BITS / 8));
	int ret;

	if (length != (CST_PLANTED_CUK_BITS / 8)) {
		EMSG("length %d not expected %d", length, (CST_PLANTED_CUK_BITS / 8));
		return TEE_ERROR_BAD_PARAMETERS;
	}

	ret = thread_smc(OPTEE_AMBA_SIP_OTP_GET_CST_CUK,
			virt_to_phys(cuk), sizeof(cuk), 0);
	if (ret < 0) {
		EMSG("cst cuk is faild to fetched from the platform.");
		return TEE_ERROR_ACCESS_DENIED;
	}
	if ((*p_embed_flag) & D_FLAG_LOCKED) {
		*locked = 1;
	} else {
		*locked = 0;
	}

	memcpy(p, cuk, sizeof(cuk));

	return TEE_SUCCESS;
}

static TEE_Result amba_otp_write_cst_seed_cuk(unsigned int *p, unsigned int length,
	unsigned int lock, unsigned int simulate)
{
	uint8_t seed_cuk[((CST_PLANTED_SEED_BITS + CST_PLANTED_CUK_BITS) / 8) + 4] __aligned(4096);
	uint32_t *p_embed_flag = (uint32_t *) ((unsigned long) seed_cuk + ((CST_PLANTED_SEED_BITS + CST_PLANTED_CUK_BITS) / 8));
	int ret;

	if (length != ((CST_PLANTED_SEED_BITS + CST_PLANTED_CUK_BITS) / 8)) {
		EMSG("length %d not expected %d", length,
			((CST_PLANTED_SEED_BITS + CST_PLANTED_CUK_BITS) / 8));
		return TEE_ERROR_BAD_PARAMETERS;
	}

	memcpy(seed_cuk, p, length);
	*p_embed_flag = D_IN_FLAG_WRITE_CONTENT;
	if (lock) {
		*p_embed_flag |= D_IN_FLAG_WRITE_LOCK;
	}

	if (simulate) {
		*p_embed_flag |= D_IN_FLAG_SIMULATE_WRITE;
	}

	ret = thread_smc(OPTEE_AMBA_SIP_OTP_SET_CST_SEED_CUK,
			virt_to_phys(seed_cuk), sizeof(seed_cuk), 0);
	if (ret < 0) {
		EMSG("cst seed and cuk is faild to write to the platform.");
		return TEE_ERROR_ACCESS_DENIED;
	}

	return TEE_SUCCESS;
}

static TEE_Result amba_otp_read_usr_cuk(unsigned int *p, unsigned int length,
	unsigned int *locked)
{
	uint8_t cuk[(USR_PLANTED_CUK_BITS / 8) + 4] __aligned(4096);
	uint32_t *p_embed_flag = (uint32_t *) ((unsigned long) cuk + (USR_PLANTED_CUK_BITS / 8));
	int ret;

	if (length != (USR_PLANTED_CUK_BITS / 8)) {
		EMSG("length %d not expected %d", length, (USR_PLANTED_CUK_BITS / 8));
		return TEE_ERROR_BAD_PARAMETERS;
	}

	ret = thread_smc(OPTEE_AMBA_SIP_OTP_GET_USR_CUK,
			virt_to_phys(cuk), sizeof(cuk), 0);
	if (ret < 0) {
		EMSG("user cuk is faild to fetched from the platform.");
		return TEE_ERROR_ACCESS_DENIED;
	}
	if ((*p_embed_flag) & D_FLAG_LOCKED) {
		*locked = 1;
	} else {
		*locked = 0;
	}

	memcpy(p, cuk, sizeof(cuk));

	return TEE_SUCCESS;
}

static TEE_Result amba_otp_write_usr_cuk(unsigned int *p, unsigned int length,
	unsigned int lock, unsigned int simulate)
{
	uint8_t cuk[(USR_PLANTED_CUK_BITS / 8) + 4] __aligned(4096);
	uint32_t *p_embed_flag = (uint32_t *) ((unsigned long) cuk + (USR_PLANTED_CUK_BITS / 8));
	int ret;

	if (length != (USR_PLANTED_CUK_BITS / 8)) {
		EMSG("length %d not expected %d", length,
			(USR_PLANTED_CUK_BITS / 8));
		return TEE_ERROR_BAD_PARAMETERS;
	}

	memcpy(cuk, p, length);
	*p_embed_flag = D_IN_FLAG_WRITE_CONTENT;
	if (lock) {
		*p_embed_flag |= D_IN_FLAG_WRITE_LOCK;
	}

	if (simulate) {
		*p_embed_flag |= D_IN_FLAG_SIMULATE_WRITE;
	}

	ret = thread_smc(OPTEE_AMBA_SIP_OTP_SET_USR_CUK,
			virt_to_phys(cuk), sizeof(cuk), 0);
	if (ret < 0) {
		EMSG("user cuk is faild to write to the platform.");
		return TEE_ERROR_ACCESS_DENIED;
	}

	return TEE_SUCCESS;
}

static TEE_Result amba_otp_disable_secure_usb_boot(unsigned int simulate)
{
	int ret;

	ret = thread_smc(OPTEE_AMBA_SIP_OTP_DIS_SECURE_USB_BOOT, simulate, 0, 0);
	if (ret < 0) {
		EMSG("permanently disable secure usb boot failed.");
		return TEE_ERROR_ACCESS_DENIED;
	}

	return TEE_SUCCESS;
}

static TEE_Result amba_otp_read_bst_version(unsigned int *bst_version)
{
	uint32_t counter __aligned(4096);
	int ret;

	ret = thread_smc(OPTEE_AMBA_SIP_OTP_GET_BST_VER,
		virt_to_phys(&counter), sizeof(counter), 0);
	if (ret < 0) {
		EMSG("bst version is faild to fetched from the platform.");
		return TEE_ERROR_ACCESS_DENIED;
	}

	*bst_version = counter;

	return TEE_SUCCESS;
}

static TEE_Result amba_otp_increase_bst_version(unsigned int simulate)
{
	int ret;

	ret = thread_smc(OPTEE_AMBA_SIP_OTP_INCREASE_BST_VER,
		simulate, 0, 0);
	if (ret < 0) {
		EMSG("bst version is faild to increase.");
		return TEE_ERROR_ACCESS_DENIED;
	}

	return TEE_SUCCESS;
}
#endif

static TEE_Result amba_otp_write_misc_config(unsigned int config)
{
	uint32_t v = config;
	int ret;

	ret = thread_smc(OPTEE_AMBA_SIP_OTP_SET_MISC_CONFIG,
			virt_to_phys(&v), 4, 0);
	if (ret < 0) {
		EMSG("misc config is faild to write to the platform.");
		return TEE_ERROR_ACCESS_DENIED;
	}

	return TEE_SUCCESS;
}

static TEE_Result amba_otp_read_misc_config(unsigned int *config,
	unsigned int *total_bits)
{
	uint32_t v[2] __aligned(4096);
	int ret;

	ret = thread_smc(OPTEE_AMBA_SIP_OTP_GET_MISC_CONFIG,
			virt_to_phys(&v[0]), 8, 0);
	if (ret < 0) {
		EMSG("misc config is faild to fetched from the platform.");
		return TEE_ERROR_ACCESS_DENIED;
	}

	config[0] = v[0];
	total_bits[1] = v[1];

	return TEE_SUCCESS;
}

/*
 * Trusted Application Entry Points
 */

static TEE_Result invoke_command(void *psess __unused,
				uint32_t cmd, uint32_t ptypes __unused,
				TEE_Param params[TEE_NUM_PARAMS])
{
	switch (cmd) {
	case AMBA_OTP_CMD_WRITE_PUBKEY:
		return amba_otp_write_rot_pubkey(params[0].memref.buffer,
			params[0].memref.size, params[1].value.a, params[1].value.b,
			params[2].value.a);
#ifdef EXPOSE_OTP_PUB_ROT_KEY
	case AMBA_OTP_CMD_READ_PUBKEY:
		return amba_otp_read_rot_pubkey(params[0].memref.buffer,
			params[0].memref.size, params[1].value.a, &params[1].value.b,
			&params[2].value.a);
#endif
	case AMBA_OTP_CMD_READ_PUBKEY_DIGEST:
		return amba_otp_read_rot_pubkey_digest(params[0].memref.buffer,
			params[0].memref.size, params[1].value.a, &params[1].value.b,
			&params[2].value.a);
	case AMBA_OTP_CMD_READ_AMBA_UNIQUE_ID:
		return amba_otp_read_amba_unique_id(params[0].memref.buffer,
			params[0].memref.size);
	case AMBA_OTP_CMD_WRITE_CUSTOMER_ID:
		return amba_otp_write_customer_id(params[0].memref.buffer,
			params[0].memref.size, params[1].value.a, params[1].value.b);
	case AMBA_OTP_CMD_READ_CUSTOMER_ID:
		return amba_otp_read_customer_id(params[0].memref.buffer,
			params[0].memref.size, &params[1].value.a);
	case AMBA_OTP_CMD_READ_MONOTINIC_COUNTER:
		return amba_otp_read_monotonic_counter(&params[0].value.a,
			params[0].value.b);
	case AMBA_OTP_CMD_INCREASE_MONOTINIC_COUNTER:
		return amba_otp_increase_monotonic_counter(
			params[0].value.a, params[0].value.b);
	case AMBA_OTP_CMD_PERMANENTLY_ENABLE_SECURE_BOOT:
		return amba_otp_permanently_enable_secure_boot(params[0].value.a);
	case AMBA_OTP_CMD_GEN_HW_UNIQUE_ENCRYPTION_KEY:
		return amba_otp_generate_hw_unique_encryption_key(params[0].value.a);
	case AMBA_OTP_CMD_GEN_AES_KEY:
		return amba_otp_generate_aes_key(params[0].value.a, params[0].value.b);
#ifdef WRITE_OTP_AES_KEY_FROM_EXTERNAL
	case AMBA_OTP_CMD_WRITE_AES_KEY:
		return amba_otp_write_aes_key(params[0].memref.buffer,
			params[0].memref.size, params[1].value.a, params[1].value.b,
			params[2].value.a);
#endif
	case AMBA_OTP_CMD_GEN_ECC_KEY:
		return amba_otp_generate_ecc_key(params[0].value.a);
#ifdef WRITE_OTP_ECC_KEY_FROM_EXTERNAL
	case AMBA_OTP_CMD_WRITE_ECC_KEY:
		return amba_otp_write_ecc_key(params[0].memref.buffer,
			params[0].memref.size, params[1].value.a, params[1].value.b,
			params[2].value.a);
#endif
	case AMBA_OTP_CMD_SET_USR_SLOT_G0:
		return amba_otp_write_user_slot_g0(params[0].memref.buffer,
			params[0].memref.size, params[1].value.a, params[1].value.b,
			params[2].value.a);
#ifdef EXPOSE_OTP_USER_SLOT_GRP0
	case AMBA_OTP_CMD_GET_USR_SLOT_G0:
		return amba_otp_read_user_slot_g0(params[0].memref.buffer,
			params[0].memref.size, params[1].value.a, &params[1].value.b);
#endif

#ifdef OTP_LAYOUT_V2
	case AMBA_OTP_CMD_SET_USR_SLOT_G1:
		return amba_otp_write_user_slot_g1(params[0].memref.buffer,
			params[0].memref.size, params[1].value.a, params[1].value.b,
			params[2].value.a);
#ifdef EXPOSE_OTP_USER_SLOT_GRP0
	case AMBA_OTP_CMD_GET_USR_SLOT_G1:
		return amba_otp_read_user_slot_g1(params[0].memref.buffer,
			params[0].memref.size, params[1].value.a, &params[1].value.b);
#endif
#endif
	case AMBA_OTP_CMD_REVOKE_KEY:
		return amba_otp_revoke_key(params[0].value.a, params[0].value.b);
		break;
	case AMBA_OTP_CMD_QUERY_OTP_SETTING:
		return amba_otp_query_setting(params[0].memref.buffer,
			params[0].memref.size);
		break;
	case AMBA_OTP_CMD_SET_JTAG_EFUSE:
		return amba_otp_set_jtag_efuse(params[0].value.a);
		break;
	case AMBA_OTP_CMD_LOCK_ZONA_A:
		return amba_otp_lock_zone_a(params[0].value.a);
		break;
	case AMBA_OTP_CMD_GET_TEST_REGION:
		return amba_otp_read_test_region(params[0].memref.buffer,
			params[0].memref.size, &params[1].value.a);
		break;
	case AMBA_OTP_CMD_SET_TEST_REGION:
		return amba_otp_write_test_region(params[0].memref.buffer,
			params[0].memref.size, params[1].value.a, params[1].value.b);
		break;
	case AMBA_OTP_CMD_LOCK_PUKEY:
		return amba_otp_lock_rot_pubkey(params[0].value.a, params[0].value.b);
		break;
	case AMBA_OTP_CMD_GET_MISC_CINFIG:
		return amba_otp_read_misc_config(&params[0].value.a, &params[0].value.b);
		break;
	case AMBA_OTP_CMD_SET_MISC_CINFIG:
		return amba_otp_write_misc_config(params[0].value.a);
		break;
	case AMBA_OTP_CMD_GET_SYSCONFIG:
		return amba_otp_read_sysconfig(params[0].memref.buffer,
			params[0].memref.size, &params[1].value.a);
		break;
	case AMBA_OTP_CMD_SET_SYSCONFIG:
		return amba_otp_write_sysconfig(params[0].memref.buffer,
			params[0].memref.size, params[1].value.a, params[1].value.b);
		break;

#ifdef OTP_LAYOUT_V2
	case AMBA_OTP_CMD_EN_ANTI_ROLLBACK:
		return amba_otp_enable_bst_anti_rollback(params[0].value.a);
		break;
	case AMBA_OTP_CMD_GET_CST_SEED:
		return amba_otp_read_cst_seed(params[0].memref.buffer,
			params[0].memref.size, &params[1].value.a);
		break;
	case AMBA_OTP_CMD_GET_CST_CUK:
		return amba_otp_read_cst_cuk(params[0].memref.buffer,
			params[0].memref.size, &params[1].value.a);
		break;
	case AMBA_OTP_CMD_SET_CST_SEED_CUK:
		return amba_otp_write_cst_seed_cuk(params[0].memref.buffer,
			params[0].memref.size, params[1].value.a, params[1].value.b);
		break;
	case AMBA_OTP_CMD_GET_USR_CUK:
		return amba_otp_read_usr_cuk(params[0].memref.buffer,
			params[0].memref.size, &params[1].value.a);
		break;
	case AMBA_OTP_CMD_SET_USR_CUK:
		return amba_otp_write_usr_cuk(params[0].memref.buffer,
			params[0].memref.size, params[1].value.a, params[1].value.b);
		break;
	case AMBA_OTP_CMD_DIS_SECURE_USB_BOOT:
		return amba_otp_disable_secure_usb_boot(params[0].value.a);
		break;
	case AMBA_OTP_CMD_GET_BST_VER:
		return amba_otp_read_bst_version(&params[0].value.a);
		break;
	case AMBA_OTP_CMD_INCREASE_BST_VER:
		return amba_otp_increase_bst_version(params[0].value.a);
		break;
#endif
	default:
		EMSG("not supported cmd %d", cmd);
		break;
	}
	return TEE_ERROR_NOT_SUPPORTED;
}

pseudo_ta_register(.uuid = PTA_AMBA_OTP_UUID, .name = TA_NAME,
		   .flags = PTA_DEFAULT_FLAGS,
		   .invoke_command_entry_point = invoke_command);

