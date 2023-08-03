/*
 * Copyright (c) 2017, ARM Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stdint.h>
#include <assert.h>
#include <lib/spinlock.h>
#include <plat_private.h>
#include "AmbaRTSL_OTP.h"

//#define D_LOCK_ZONE_A_EXPLICITE
#define D_NOT_AVAILABLE_RETCODE (-81)

int ambarella_otp_lock_rot_pubkey(uint32_t key_index, uint32_t simulate)
{
    INFO("Not implemented. Locking the Public Key %d\n", key_index);
    return D_NOT_AVAILABLE_RETCODE;
}

int ambarella_otp_read_user_data_g0(uint8_t *p, uint32_t length, uint32_t data_index)
{
    INFO("%s: Not implemented\n", __func__);
    return D_NOT_AVAILABLE_RETCODE;
}

int ambarella_otp_write_user_data_g0(uint8_t *p, uint32_t length, uint32_t data_index)
{
    INFO("%s: Not implemented\n", __func__);
    return D_NOT_AVAILABLE_RETCODE;
}

int ambarella_otp_read_user_data_g1(uint8_t *p, uint32_t length, uint32_t data_index)
{
    INFO("%s: Not implemented\n", __func__);
    return D_NOT_AVAILABLE_RETCODE;
}

int ambarella_otp_write_user_data_g1(uint8_t *p, uint32_t length, uint32_t data_index)
{
    INFO("%s: Not implemented\n", __func__);
    return D_NOT_AVAILABLE_RETCODE;
}

int ambarella_otp_read_test_region(uint8_t *p, uint32_t length)
{
    INFO("%s: Not implemented\n", __func__);
    return D_NOT_AVAILABLE_RETCODE;
}

int ambarella_otp_write_test_region(uint8_t *p, uint32_t length)
{
    INFO("%s: Not implemented\n", __func__);
    return D_NOT_AVAILABLE_RETCODE;
}

int ambarella_otp_read_sysconfig(uint8_t *p, uint32_t length)
{
    INFO("%s: Not implemented\n", __func__);
    return D_NOT_AVAILABLE_RETCODE;
}

int ambarella_otp_write_sysconfig(uint8_t *p, uint32_t length)
{
    INFO("%s: Not implemented\n", __func__);
    return D_NOT_AVAILABLE_RETCODE;
}

int ambarella_otp_read_cst_planted_seed(uint8_t *p, uint32_t length)
{
    INFO("%s: Not implemented\n", __func__);
    return D_NOT_AVAILABLE_RETCODE;
}

int ambarella_otp_read_cst_planted_cuk(uint8_t *p, uint32_t length)
{
    INFO("%s: Not implemented\n", __func__);
    return D_NOT_AVAILABLE_RETCODE;
}

int ambarella_otp_write_cst_planted_seed_and_cuk(uint8_t *p, uint32_t length)
{
    INFO("%s: Not implemented\n", __func__);
    return D_NOT_AVAILABLE_RETCODE;
}

int ambarella_otp_read_user_planted_cuk(uint8_t *p, uint32_t length)
{
    INFO("%s: Not implemented\n", __func__);
    return D_NOT_AVAILABLE_RETCODE;
}

int ambarella_otp_write_user_planted_cuk(uint8_t *p, uint32_t length)
{
    INFO("%s: Not implemented\n", __func__);
    return D_NOT_AVAILABLE_RETCODE;
}

int ambarella_otp_read_bst_ver(uint32_t *p,  uint32_t length)
{
    INFO("%s: Not implemented\n", __func__);
    return D_NOT_AVAILABLE_RETCODE;
}

int ambarella_otp_increase_bst_ver(uint32_t simulate)
{
    INFO("%s: Not implemented\n", __func__);
    return D_NOT_AVAILABLE_RETCODE;
}

int ambarella_otp_set_bst_anti_rollback(uint32_t simulate)
{
    INFO("%s: Not implemented\n", __func__);
    return D_NOT_AVAILABLE_RETCODE;
}

int ambarella_otp_disable_secure_usb_boot(uint32_t simulate)
{
    INFO("%s: Not implemented\n", __func__);
    return D_NOT_AVAILABLE_RETCODE;
}

int ambarella_otp_write_misc_config(uint32_t misc_config, uint32_t simulate)
{
    INFO("%s: Not implemented\n", __func__);
    return D_NOT_AVAILABLE_RETCODE;
}

int ambarella_otp_read_misc_config(uint32_t *p)
{
    INFO("%s: Not implemented\n", __func__);
    return D_NOT_AVAILABLE_RETCODE;
}

int ambarella_otp_nw_query_otp_setting(unsigned long *sys_config, unsigned long *lock_bit_invalid_bit)
{
    INFO("%s: Not implemented\n", __func__);
    return D_NOT_AVAILABLE_RETCODE;
}

int ambarella_otp_read_amba_unique_id(uint8_t *p, uint32_t length)
{
    uint32_t uerr = 1;

    uerr = AmbaRTSL_OtpAmbaUniqueIDRead(p, length);
    if (uerr != 0u) {
        return -1;
    }

    return 0;
}

static int ambarella_otp_read_pukey(uint8_t *p, uint32_t length, uint32_t key_index)
{
    uint32_t uerr = 1;
    uint32_t status = 0;

    uerr = AmbaRTSL_OtpPublicKeyRead(p, length, key_index, &status);
    // TODO: check status
    if (uerr != 0u) {
        return -1;
    }

    return 0;
}

int ambarella_otp_read_rot_pubkey(uint8_t *p, uint32_t length, uint32_t key_index)
{
    return ambarella_otp_read_pukey(p, length, key_index);
}

static int ambarella_otp_write_pukey(uint8_t *p, uint32_t length, uint32_t key_index)
{
    uint32_t uerr = 1;

    uerr = AmbaRTSL_OtpPublicKeyWrite(p, length, key_index);
    if (uerr != 0u) {
        return -1;
    }

    return 0;
}

int ambarella_otp_write_rot_pubkey(uint8_t *p, uint32_t length, uint32_t key_index)
{
    return ambarella_otp_write_pukey(p, length, key_index);
}

int ambarella_otp_read_customer_id(uint8_t *p, uint32_t length)
{
    uint32_t uerr = 1;

    uerr = AmbaRTSL_OtpCustomerUniIDRead(p, length);
    if (uerr != 0u) {
        return -1;
    }

    return 0;
}

int ambarella_otp_write_customer_id(uint8_t *p, uint32_t length)
{
    uint32_t uerr = 1;

    uerr = AmbaRTSL_OtpCustomerUniIDWrite(p, length);
    if (uerr != 0u) {
        return -1;
    }

    return 0;
}

static int ambarella_otp_read_counter(uint32_t *p,  uint32_t length)
{
    uint32_t uerr = 1;

    if (length != sizeof(uint32_t)) {
        ERROR("data length (%d) is not enough (need %d) for read counter.\n", length, (unsigned int) sizeof(uint32_t));
        return -2;
    }
    uerr = AmbaRTSL_OtpMonoCounterRead(p);
    if (uerr != 0u) {
        return -1;
    }

    return 0;
}

int ambarella_otp_read_mono_counter(uint32_t *p,  uint32_t length, uint32_t mono_cnt_index)
{
    /* always read counter 0 */
    return ambarella_otp_read_counter(p, length);
}

static int ambarella_otp_add_counter(void)
{
    uint32_t uerr = 1;

    uerr = AmbaRTSL_OtpMonoCounterIncrease();
    if (uerr != 0u) {
        return -1;
    }

    return 0;
}

int ambarella_otp_increase_mono_counter(uint32_t mono_cnt_index, uint32_t simulate)
{
    /* always increase counter 0 */
    (void)mono_cnt_index;

    return ambarella_otp_add_counter();
}

int ambarella_otp_permanently_enable_secure_boot(uint32_t simulate)
{
    uint32_t uerr = 1;

    uerr = AmbaRTSL_OtpSecureBootEnable();
    if (uerr != 0u) {
        return -1;
    }

    return 0;
}

int ambarella_otp_read_huk(uint8_t *p, uint32_t length)
{
    uint32_t i, val;
    unsigned int *pu32 = (unsigned int *) p;

    if (length < (HUK_BITS / 8)) {
        ERROR("buffer length (%d) is not enough (need %d) for read huk.\n",
               length, (HUK_BITS / 8));
        return -2;
    }

    for (i = 0; i < HUK_BITS; i += 32) {
        if (ambarella_otp_read(HUK_ADDR + i, 32, &val) < 0) {
            return -1;
        }
        *pu32++ = val;
    }

    return 0;
}

int ambarella_otp_write_huk_nonce(uint8_t *p, uint32_t length)
{
    uint32_t i, lock_bit;
    unsigned int *pu32 = (unsigned int *) p;

    if (ambarella_otp_read(WRITE_LOCK_BIT_ADDR, 32, &lock_bit) < 0)
        return -1;

    if (lock_bit & (1 << LOCK_BIT_HUK_NONCE)) {
        ERROR("OTP 'huk_nonce' already writen, lock_bit 0x%08x.\n", lock_bit);
        return -2;
    }

    if (length != ((HUK_BITS + HW_NONCE_BITS) / 8)) {
        ERROR("data length (%d) is not expected (shall be %d) for write huk and nonce.\n",
            length, ((HUK_BITS + HW_NONCE_BITS) / 8));
        return -2;
    }

    for (i = 0; i < (HUK_BITS + HW_NONCE_BITS); i += 32) {
        if (ambarella_otp_write(HUK_ADDR + i, 32, *pu32++) < 0)
            return -1;
    }

    if (ambarella_otp_write(WRITE_LOCK_BIT_ADDR + LOCK_BIT_HUK_NONCE, 1, 1) < 0)
        return -1;

    return 0;
}

int ambarella_otp_read_aes_key(uint8_t *p, uint32_t length, uint32_t key_index)
{
    uint32_t i, val, addr;
    unsigned int *pu32 = (unsigned int *) p;

    if ((key_index + 1) > AES_KEY_NUM) {
        ERROR("bad aeskey index %d.\n", key_index);
        return -1;
    }

    if (length < (AES_KEY_BITS / 8)) {
        ERROR("buffer length (%d) is not enough (need %d) for read aes key.\n",
            length, (AES_KEY_BITS / 8));
        return -2;
    }

    addr = AES_KEY_BASE_ADDR + (key_index * AES_KEY_BITS);

    for (i = 0; i < AES_KEY_BITS; i += 32) {
         if (ambarella_otp_read(addr + i, 32, &val) < 0)
             return -1;
         *pu32++ = val;
    }

    return 0;
}

int ambarella_otp_write_aes_key(uint8_t *p, uint32_t length, uint32_t key_index)
{
    uint32_t uerr = 1;

    uerr = AmbaRTSL_OtpAesKeyWrite(p, length, key_index);
    if (uerr != 0u) {
        return -1;
    }

    return 0;
}

int ambarella_otp_read_ecc_key(uint8_t *p, uint32_t length, uint32_t key_index)
{
    uint32_t i, val, addr;
    unsigned int *pu32 = (unsigned int *) p;

    if ((key_index + 1) > ECC_KEY_NUM) {
        ERROR("bad ecckey index %d.\n", key_index);
        return -1;
    }

    if (length < (ECC_KEY_BITS / 8)) {
        ERROR("buffer length (%d) is not enough (need %d) for read ecc key.\n",
            length, (ECC_KEY_BITS / 8));
        return -2;
    }

    addr = ECC_KEY_BASE_ADDR + (key_index * ECC_KEY_BITS);

    for (i = 0; i < ECC_KEY_BITS; i += 32) {
        if (ambarella_otp_read(addr + i, 32, &val) < 0)
            return -1;
        *pu32++ = val;
    }

    return 0;
}

int ambarella_otp_write_ecc_key(uint8_t *p, uint32_t length, uint32_t key_index)
{
    uint32_t i, lock_bit, addr;
    unsigned int *pu32 = (unsigned int *) p;

    if ((key_index + 1) > ECC_KEY_NUM) {
        ERROR("bad ecckey index %d.\n", key_index);
        return -1;
    }

    if (length < (ECC_KEY_BITS / 8)) {
        ERROR("buffer length (%d) is not enough (shall be %d) for write ecc key.\n",
            length, (ECC_KEY_BITS / 8));
        return -2;
    }

    if (ambarella_otp_read(WRITE_LOCK_BIT_ADDR, 32, &lock_bit) < 0)
        return -1;

    if (lock_bit & (1 << (LOCK_BIT_ECC_KEY_BASE + key_index))) {
        ERROR("OTP 'ecc key %d' already writen, lock_bit 0x%08x.\n", key_index, lock_bit);
        return -2;
    }

    addr = ECC_KEY_BASE_ADDR + (key_index * ECC_KEY_BITS);

    for (i = 0; i < ECC_KEY_BITS; i += 32) {
        if (ambarella_otp_write(addr + i, 32, *pu32++) < 0)
            return -1;
    }

    if (ambarella_otp_write(WRITE_LOCK_BIT_ADDR + LOCK_BIT_ECC_KEY_BASE + key_index, 1, 1) < 0)
        return -1;

    return 0;
}

static int ambarella_otp_read_reserved_dx(uint8_t *p, uint32_t length, uint32_t dx_index)
{
    uint32_t uerr = 1;

    uerr = AmbaRTSL_OtpReservedDXRead(p, length, dx_index);
    if (uerr != 0u) {
        return -1;
    }

    return 0;
}

int ambarella_otp_read_user_slot_g0(uint8_t *p, uint32_t length, uint32_t slot_index)
{
    return ambarella_otp_read_reserved_dx(p, length, slot_index);
}

static int ambarella_otp_write_reserved_dx(uint8_t *p, uint32_t length, uint32_t dx_index)
{
    uint32_t uerr = 1;

    uerr = AmbaRTSL_OtpReservedDXWrite(p, length, dx_index);
    if (uerr != 0u) {
        return -1;
    }

    return 0;
}

int ambarella_otp_write_user_slot_g0(uint8_t *p, uint32_t length, uint32_t slot_index)
{
    return ambarella_otp_write_reserved_dx(p, length, slot_index);
}

#if 0
static int ambarella_otp_read_reserved_b2(uint8_t *p, uint32_t length)
{
    uint32_t uerr = 1;

    uerr = AmbaRTSL_OtpReservedB2Read(p, length);
    if (uerr != 0u) {
        return -1;
    }

    return 0;
}

static int ambarella_otp_write_reserved_b2(uint8_t *p, uint32_t length)
{
    uint32_t uerr = 1;

    uerr = AmbaRTSL_OtpReservedB2Write(p, length);
    if (uerr != 0u) {
        return -1;
    }

    return 0;
}
#endif

int ambarella_otp_read_user_slot_g1(uint8_t *p, uint32_t length, uint32_t slot_index)
{
#if 0
    /* do not save lock status to cookie */
    (void)cookie;

    return ambarella_otp_read_reserved_b2(p, length)
#else
    INFO("%s: Not implemented\n", __func__);
    return D_NOT_AVAILABLE_RETCODE;
#endif
}

int ambarella_otp_write_user_slot_g1(uint8_t *p, uint32_t length, uint32_t slot_index)
{
#if 0
    return ambarella_otp_write_reserved_b2(p, length);
#else
    INFO("%s: Not implemented\n", __func__);
    return D_NOT_AVAILABLE_RETCODE;
#endif
}

int ambarella_otp_revoke_key(uint32_t index, uint32_t simulate)
{
    uint32_t uerr = 1;

    uerr = AmbaRTSL_OtpPublicKeyRevoke(index);
    if (uerr != 0u) {
        return -1;
    }

    return 0;
}

#if 0
int ambarella_otp_get_chip_repair_info(uint32_t *p, uint32_t length)
{
	unsigned int i = 0;

	if ((!p) || (length != (500 * 4))) {
		ERROR("invalid params %p, %d.\n", p, length);
		return -1;
	}

	NOTICE("read chip repair info\n");

	for (i = 0; i < (CHIP_REPAIR_INFO_ADDR + CHIP_REPAIR_INFO_BITS); i += 32) {
		if (ambarella_otp_read(i, 32, p) < 0)
			return -1;
		p++;
	}

	return 0;
}
#endif

int ambarella_otp_query_otp_setting(uint32_t *p, uint32_t length)
{
	if ((!p) || (length != (4 * 4))) {
		ERROR("invalid params %p, %d.\n", p, length);
		return -1;
	}

	if (ambarella_otp_read(WRITE_LOCK_BIT_ADDR, 32, p) < 0) {
		ERROR("read lock bit fail.\n");
		return -1;
	}

	if (ambarella_otp_read(DATA_INVALID_BIT_ADDR, 32, p + 1) < 0) {
		ERROR("read data invalid bit fail.\n");
		return -1;
	}

	if (ambarella_otp_read(0, 32, p + 2) < 0) {
		ERROR("read sys config 1 fail.\n");
		return -1;
	}

	if (ambarella_otp_read(32, 32, p + 3) < 0) {
		ERROR("read sys config 2 fail.\n");
		return -1;
	}

	return 0;
}

int ambarella_otp_set_jtag_efuse(uint32_t simulate)
{
	unsigned int v = 0;

	NOTICE("Set JTAG efuse...\n");

	if (ambarella_otp_read(WRITE_LOCK_BIT_ADDR, 1, &v) < 0) {
		ERROR("read lockbit fail.\n");
		return -1;
	}

	if (!(v & 0x1)) {
		if (ambarella_otp_read(SYS_CONFIG_BIT_ADDR + JTAG_EFUSE_BIT, 1, &v) < 0) {
			ERROR("read otp63 fail.\n");
			return -1;
		}

		if (v & 0x1) {
			NOTICE("JTAG efuse already set.\n");
		} else {
			if (ambarella_otp_write(SYS_CONFIG_BIT_ADDR + JTAG_EFUSE_BIT, 1, 1) < 0) {
				return -1;
			}
			NOTICE("Set JTAG efuse: done.\n");
		}
	} else {
		if (ambarella_otp_read(SYS_CONFIG_BIT_ADDR + JTAG_EFUSE_BIT, 1, &v) < 0) {
			ERROR("read otp63 fail.\n");
			return -1;
		}
		if (v & 0x1) {
			NOTICE("JTAG efuse is already set.\n");
		} else {
			ERROR("sysconfig already locked, JTAG efuse is not set.\n");
			return (-3);
		}
	}

	return 0;
}

int ambarella_otp_lock_zone_a(uint32_t simulate)
{
#ifdef D_LOCK_ZONE_A_EXPLICITE
	unsigned int v = 0;

	NOTICE("Lock zone a...\n");

	if (ambarella_otp_read(WRITE_LOCK_BIT_ADDR, 1, &v) < 0) {
		ERROR("read lockbit fail.\n");
		return -1;
	}

	if (!(v & 0x1)) {
		if (ambarella_otp_write(WRITE_LOCK_BIT_ADDR, 1, 1) < 0) {
			return -1;
		}
		NOTICE("Lock zone a: done.\n");
	} else {
		NOTICE("Lock zone a is already locked.\n");
	}
#endif
	return 0;
}

