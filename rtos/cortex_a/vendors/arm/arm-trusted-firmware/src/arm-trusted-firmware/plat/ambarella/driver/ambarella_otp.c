/*
 * Copyright (c) 2017, ARM Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#include <assert.h>
#include <lib/mmio.h>
#include <lib/spinlock.h>
#include <drivers/gpio.h>
#include <drivers/delay_timer.h>
#include <plat_private.h>
#include "ambarella_otp.h"

#define D_LOCKED_RETCODE (-80)
#define D_NOT_AVAILABLE_RETCODE (-81)

/* keep consistent with invoker */
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

//#define D_LOCK_ZONE_A_EXPLICITE

/* enable it for debug purpose */
#define D_INTERNAL_CHECK

static spinlock_t otp_access_lock;

#if defined(GPIO_OTP_PWR_SW) && !IMAGE_BL2
static void ambarella_otp_write_enable(void)
{
	gpio_set_value(GPIO_OTP_PWR_SW, GPIO_LEVEL_HIGH);
	mdelay(10);
}

static void ambarella_otp_write_disable(void)
{
	gpio_set_value(GPIO_OTP_PWR_SW, GPIO_LEVEL_LOW);
	mdelay(10);
}

void ambarella_otp_init(void)
{
	gpio_set_direction(GPIO_OTP_PWR_SW, GPIO_DIR_OUT);
	gpio_set_value(GPIO_OTP_PWR_SW, GPIO_LEVEL_LOW);
}
#else
static void ambarella_otp_write_enable(void)
{
}

static void ambarella_otp_write_disable(void)
{
}

void ambarella_otp_init(void)
{
}
#endif

int ambarella_otp_read(uint32_t bit_addr, uint32_t length, uint32_t *value)
{
	uint32_t _bit_addr = round_down(bit_addr, 32);

#ifdef D_INTERNAL_CHECK
	if (_bit_addr != round_down(bit_addr + length - 1, 32) || bit_addr > OTP_BIT_SIZE) {
		ERROR("Invalid OTP read address: 0x%x - 0x%x\n",
				bit_addr, bit_addr + length);
		return -31;
	}
#endif

	spin_lock(&otp_access_lock);

	ambarella_otp_init();

	mmio_write_32(OTP_CTRL1_REG, 0);
	mmio_clrbits_32(OTP_CTRL1_REG, FSM_WRITE_MODE);		/* fsm_write_mode = 0 */
	mmio_setbits_32(OTP_CTRL1_REG, DBG_READ_MODE);
	mmio_setbits_32(OTP_CTRL1_REG, READ_FSM_ENABLE);

	while (!(mmio_read_32(OTP_OBSV_REG) & READ_OBSV_RDY));

	mmio_clrsetbits_32(OTP_CTRL1_REG, (OTP_BIT_SIZE - 1), _bit_addr);

	mmio_setbits_32(OTP_CTRL1_REG, READ_ENABLE);

	while (!(mmio_read_32(OTP_OBSV_REG) & READ_OBSV_DONE));

	*value = mmio_read_32(OTP_READ_DOUT_REG);
	*value >>= (bit_addr % 32);
	*value &= (1ULL << length) - 1;

	mmio_clrbits_32(OTP_CTRL1_REG, READ_ENABLE);

	while (!(mmio_read_32(OTP_OBSV_REG) & READ_OBSV_RDY));

	spin_unlock(&otp_access_lock);

	return 0;
}

int ambarella_otp_write(uint32_t bit_addr, uint32_t length, uint32_t value)
{
	uint32_t _bit_addr = round_down(bit_addr, 32), i, j, val, failed;
	int ret = 0;

#ifdef D_INTERNAL_CHECK
	if (_bit_addr != round_down(bit_addr + length - 1, 32) || bit_addr > OTP_BIT_SIZE) {
		ERROR("Invalid OTP write address: 0x%x - 0x%x\n",
				bit_addr, bit_addr + length);
		return -41;
	}
#endif

	if (ambarella_otp_read(bit_addr, length, &val) < 0)
		return -42;

	spin_lock(&otp_access_lock);

	ambarella_otp_init();

	ambarella_otp_write_enable();

	for (i = 0; i < length; i++) {
		/* do nothing if the OTP bit value is the same as required. */
		if (((val ^ value) & (1U << i)) == 0)
			continue;

		/* the OTP bit can only be set, but cannot be clear. */
		if (val & (1U << i)) {
			ERROR("cannot clear OTP bit at 0x%x\n", bit_addr + i);
			ret = -43;
			goto __write_disable;
		}

		for (j = 0; j < 3; j++) {
			mmio_write_32(OTP_CTRL1_REG, 0);
			mmio_setbits_32(OTP_CTRL1_REG, FSM_WRITE_MODE);
			mmio_setbits_32(OTP_CTRL1_REG, PROG_FSM_ENABLE);

			while (!(mmio_read_32(OTP_OBSV_REG) & WRITE_PROG_RDY));

			mmio_clrsetbits_32(OTP_CTRL1_REG, (OTP_BIT_SIZE - 1), bit_addr + i);

			mmio_setbits_32(OTP_CTRL1_REG, PROG_ENABLE);
			while (!(mmio_read_32(OTP_OBSV_REG) & WRITE_PROG_DONE));
			failed = mmio_read_32(OTP_OBSV_REG) & WRITE_PROG_FAIL;

			mmio_clrbits_32(OTP_CTRL1_REG, PROG_ENABLE);
			while (!(mmio_read_32(OTP_OBSV_REG) & WRITE_PROG_RDY));

			if (failed == 0)
				break;
		}

		if (j >= 3) {
			ERROR("OTP write at 0x%x failed.\n", bit_addr + i);
			ret = -44;
			goto __write_disable;
		}
	}

__write_disable:
	ambarella_otp_write_disable();
	spin_unlock(&otp_access_lock);

	return ret;
}

/* no parameter check is needed here */
static int ambarella_otp_read_field(uint32_t addr, uint32_t len,
	void *v)
{
	uint32_t i, *pu32 = v;

	for (i = 0; i < len; i += 32) {
		if (ambarella_otp_read(addr + i, 32, pu32++) < 0) {
			ERROR("OTP read 0x%x failed\n", addr + i);
			return -10;
		}
	}

	return 0;
}

/* no parameter check is needed here */
static int ambarella_otp_write_field(uint32_t addr, uint32_t len,
	void *v, uint32_t lock_index,
	uint32_t write_content, uint32_t write_lock, uint32_t simulate_write,
	const char *op_str)
{
	uint32_t i, lock_bit, value, *pu32;
	int ret;

	if (simulate_write) {
		if (write_content) {
			NOTICE("[Simulate OTP write]: %s: addr 0x%08x, length %d\n",
				op_str, addr, len);
		}
		if (write_lock) {
			NOTICE("[Simulate OTP write]: %s's lock bit: %d\n",
				op_str, lock_index);
		}
		return 0;
	}

	if (write_content) {
		for (i = 0, pu32 = v; i < len; i += 32) {
			if (ambarella_otp_write(addr + i, 32, *pu32++) < 0) {
				ERROR("OTP write 0x%x failed\n", addr + i);
				return -20;
			}
		}

		for (i = 0, pu32 = v; i < len; i += 32) {
			if (ambarella_otp_read(addr + i, 32, &value) < 0) {
				ERROR("OTP read back 0x%x failed\n", addr + i);
				return -21;
			}

			if (value != *pu32++) {
				ERROR("OTP field data dismatch[%d]: 0x%08x, 0x%08x\n",
					addr + i, value, *(pu32 - 1));
				return -22;
			}
		}
	}

	if (write_lock) {
		NOTICE("!!write lock index %d\n", lock_index);
		if (ambarella_otp_write(WRITE_LOCK_BIT_ADDR + lock_index, 1, 1) < 0) {
			ERROR("OTP write lock bit failed: %d\n", lock_index);
			return -23;
		}

		ret = ambarella_otp_read(WRITE_LOCK_BIT_ADDR + lock_index, 1, &lock_bit);
		if (ret < 0 || lock_bit == 0) {
			ERROR("OTP write lock, readback check failed: %d\n", lock_index);
			return -24;
		}
	}

	return 0;
}

static int is_locked(uint32_t lock_bit_index)
{
	uint32_t lock_bit = 0;
	int ret = ambarella_otp_read(WRITE_LOCK_BIT_ADDR + lock_bit_index, 1, &lock_bit);

	if (0 > ret) {
		ERROR("read lock bits (%d) failed\n", lock_bit_index);
		return ret;
	}

	if (lock_bit) {
		return 1;
	}

	return 0;
}

static int store_embedded_flag(unsigned char *p,
	unsigned int length, unsigned int content_length,
	unsigned int lock_index, const char *otp_str)
{
	uint32_t *p_embed_flag = NULL;
	int ret;

	if (length == content_length) {
		p_embed_flag = NULL;
	} else if (length == (content_length + 4)) {
		p_embed_flag = (uint32_t *) ((unsigned long) p + content_length);
	} else {
		ERROR("length not expected (%d) for read %s\n", length, otp_str);
		return -2;
	}

	if (p_embed_flag) {
		*p_embed_flag = 0;

		ret = is_locked(lock_index);
		if (0 < ret) {
			*p_embed_flag |= D_FLAG_LOCKED;
		} else if (0 > ret) {
			*p_embed_flag |= D_FLAG_HW_ERROR;
			return -50;
		}
	}
	return 0;
}

static int check_length_get_embedded_flag(unsigned char *p,
	unsigned int length, unsigned int content_length,
	unsigned int *write_content, unsigned int *write_lock,
	unsigned int *simulate)
{
	unsigned int embedded_flag;

	if (length == content_length) {
		*write_content = 1;
		*write_lock = 1;
		*simulate = 0;
	} else if (length == (content_length + 4)) {
		embedded_flag = *((uint32_t *) ((unsigned long) p + content_length));
		if (embedded_flag & D_IN_FLAG_WRITE_CONTENT) {
			*write_content = 1;
		} else {
			*write_content = 0;
		}
		if (embedded_flag & D_IN_FLAG_WRITE_LOCK) {
			*write_lock = 1;
			NOTICE("write lock request\n");
		} else {
			*write_lock = 0;
		}
		if (embedded_flag & D_IN_FLAG_SIMULATE_WRITE) {
			*simulate = 1;
		} else {
			*simulate = 0;
		}
	} else {
		ERROR("length not expected (%d)\n",
			length);
		return -60;
	}

	return 0;
}

int ambarella_otp_read_amba_unique_id(uint8_t *p, uint32_t length)
{
	uint32_t id_addr, id_len;

	if ((!p) || (!length)) {
		ERROR("zero buf or length.\n");
		return -1;
	}

	if (length < UNIQUE_ID_BITS / 8) {
		ERROR("not enough buffer length (%d) for read unique ID, (need %d)\n",
			length, UNIQUE_ID_BITS / 8);
		return -2;
	}

	INFO("Reading amba unique id\n");

	id_addr = UNIQUE_ID_ADDR;
	id_len = UNIQUE_ID_BITS;

#if defined(AMBARELLA_CV25)
	{
		uint32_t v;

		if (ambarella_otp_read(55, 6, &v) < 0)
			return -1;
		if (!v) {
			NOTICE("cv25 chip rev A0 or engineering sample\n");
			id_addr = UNIQUE_ID_ADDR_ORICV25;
		}
	}
#endif

	return ambarella_otp_read_field(id_addr, id_len, p);
}

int ambarella_otp_read_rot_pubkey(uint8_t *p, uint32_t length,
	uint32_t key_index)
{
	uint32_t pk_addr, pk_len;
	uint32_t lock_bit, invalid_bis = 0;
	int ret;

	if ((!p) || (!length)) {
		ERROR("zero buf or length.\n");
		return -1;
	}

#if defined (OTP_LAYOUT_V1)
	lock_bit = LOCK_BIT_ROT_BASE + key_index;
#elif defined (OTP_LAYOUT_V2)
	lock_bit = LOCK_BIT_ROT_BASE;
#else
	ERROR("not supported layout\n");
	return -3;
#endif

	if (key_index + 1 > ROT_KEY_NUM) {
		ERROR("bad pubkey index %d.\n", key_index);
		return -1;
	}

	INFO("Reading the ROT Public Key %d\n", key_index);

	ret = store_embedded_flag(p,
		length, ROT_PUBKEY_BITS / 8,
		lock_bit, "pub key");
	if (0 > ret) {
		return ret;
	}

    // store invalid/revoke information
	if (length == ((ROT_PUBKEY_BITS / 8) + 4)) {
		uint32_t *p_embed_flag =
			(uint32_t *) ((unsigned long) p + (ROT_PUBKEY_BITS / 8));

		if (NON_REVOKABLE_KEY_INDEX != key_index) {
			ret = ambarella_otp_read(DATA_INVALID_BIT_ADDR + key_index, 1, &invalid_bis);
			if (ret < 0) {
				ERROR("read invalid bits 0x%x + %d failed\n",
					DATA_INVALID_BIT_ADDR, key_index);
				return ret;
			}
			if (invalid_bis & 0x1) {
				*p_embed_flag |= D_FLAG_REVOKED;
			}
		}
	}

	/* If it's RSA, the pub key is RSA N plus RSA RN */
	pk_addr = ROT_PUBKEY_ADDR + key_index * ROT_PUBKEY_BITS;
	pk_len = ROT_PUBKEY_BITS;

	return ambarella_otp_read_field(pk_addr, pk_len, p);
}

int ambarella_otp_write_rot_pubkey(uint8_t *p, uint32_t length,
	uint32_t key_index)
{
	uint32_t pk_addr, pk_len;
	uint32_t lock_bit;
	uint32_t simulate = 0, write_content = 0, write_lock = 0;
	int ret;

	INFO("Writing the Public Key %d\n", key_index);

	if ((key_index + 1) > ROT_KEY_NUM) {
		ERROR("bad pubkey index %d.\n", key_index);
		return -1;
	}

	/* check lock status */
#if defined (OTP_LAYOUT_V1)
	lock_bit = LOCK_BIT_ROT_BASE + key_index;
#elif defined (OTP_LAYOUT_V2)
	lock_bit = LOCK_BIT_ROT_BASE;
#else
	ERROR("not supported layout\n");
	return -3;
#endif
	ret = is_locked(lock_bit);
	if (0 < ret) {
		ERROR("ROT pubkey (%d) already locked\n", key_index);
		return D_LOCKED_RETCODE;
	} else if (0 > ret) {
		ERROR("Read pubkey (%d) lock bit failed\n", key_index);
		return -4;
	}

	if (p && length) {
		ret = check_length_get_embedded_flag(p,
			length, ROT_PUBKEY_BITS / 8,
			&write_content, &write_lock, &simulate);
		if (0 > ret) {
			ERROR("length not expected (%d) for write pubkey\n",
				length);
			return -5;
		}

		pk_addr = ROT_PUBKEY_ADDR + key_index * ROT_PUBKEY_BITS;
		pk_len = ROT_PUBKEY_BITS;

		/* layout2 must use lock API */
#if defined (OTP_LAYOUT_V2)
		write_lock = 0;
#endif

		return ambarella_otp_write_field(pk_addr, pk_len,
			p, lock_bit,
			write_content, write_lock, simulate,
			"write rot pubkey");
	} else {
		ERROR("zero data or length.\n");
		return -6;
	}

	return ret;
}

int ambarella_otp_lock_rot_pubkey(uint32_t key_index, uint32_t simulate)
{
	uint32_t lock_bit_addr;

	INFO("Locking the Public Key %d\n", key_index);

#if defined (OTP_LAYOUT_V1)
	if ((key_index + 1) > ROT_KEY_NUM) {
		ERROR("bad pubkey index %d.\n", key_index);
		return -1;
	}
	lock_bit_addr = WRITE_LOCK_BIT_ADDR + LOCK_BIT_ROT_BASE + key_index;
#elif defined (OTP_LAYOUT_V2)
	if (ROT_KEY_NUM == key_index) {
		lock_bit_addr = WRITE_LOCK_BIT_ADDR + LOCK_BIT_ROT_BASE;
	} else {
		ERROR("lock all pubkeys together, should use index = %d (total num).\n",
			ROT_KEY_NUM);
		return -2;
	}
#else
	ERROR("not supported layout\n");
	return -3;
#endif

	if (simulate) {
		NOTICE("[Simulate OTP write]: rot key's lock bit: %d\n",
			lock_bit_addr - WRITE_LOCK_BIT_ADDR);
		return 0;
	}

	return ambarella_otp_write(lock_bit_addr, 1, 1);
}

int ambarella_otp_read_customer_id(uint8_t *p, uint32_t length)
{
	int ret;
	INFO("Reading the customer ID\n");

	if ((!p) || (!length)) {
		ERROR("zero buf or length.\n");
		return -1;
	}

	ret = store_embedded_flag(p,
		length, CUSTOMER_ID_BITS / 8,
		LOCK_BIT_CUSTOMER_ID, "customer id");
	if (0 > ret) {
		return ret;
	}

	return ambarella_otp_read_field(CUSTOMER_ID_ADDR, CUSTOMER_ID_BITS, p);
}

int ambarella_otp_write_customer_id(uint8_t *p, uint32_t length)
{
	uint32_t lock_bit;
	uint32_t write_content = 0, write_lock = 0, simulate = 0;
	int ret = 0;

	INFO("Writing the customer ID\n");

	/* check lock status */
	lock_bit = LOCK_BIT_CUSTOMER_ID;
	ret = is_locked(lock_bit);
	if (0 < ret) {
		ERROR("customer id (serial number) already locked\n");
		return D_LOCKED_RETCODE;
	} else if (0 > ret) {
		ERROR("read lock bit failed\n");
		return -4;
	}

	if (p && length) {
		ret = check_length_get_embedded_flag(p,
			length, CUSTOMER_ID_BITS / 8,
			&write_content, &write_lock, &simulate);
		if (0 > ret) {
			ERROR("length not expected (%d) for write customer id\n",
				length);
			return -5;
		}

		return ambarella_otp_write_field(CUSTOMER_ID_ADDR, CUSTOMER_ID_BITS,
			p, lock_bit,
			write_content, write_lock, simulate,
			"write customer id");
	} else {
		ERROR("zero data or length.\n");
		return -6;
	}

	return ret;
}

static int get_mono_cnt_addr_bits(uint32_t mono_cnt_index,
	uint32_t *mono_counter_addr, uint32_t *mono_counter_bits)
{
#if defined (OTP_LAYOUT_V1)
	if (mono_cnt_index) {
		ERROR("not available on this platform, mono index %d\n", mono_cnt_index);
		return D_NOT_AVAILABLE_RETCODE;
	}
	*mono_counter_addr = MONO_CNT_0_ADDR;
	*mono_counter_bits = MONO_CNT_0_BITS;
#elif defined (OTP_LAYOUT_V2)
	if (!mono_cnt_index) {
		*mono_counter_addr = MONO_CNT_0_ADDR;
		*mono_counter_bits = MONO_CNT_0_BITS;
	} else if (1 == mono_cnt_index) {
		*mono_counter_addr = MONO_CNT_1_ADDR;
		*mono_counter_bits = MONO_CNT_1_BITS;
	} else if (2 == mono_cnt_index) {
		*mono_counter_addr = MONO_CNT_2_ADDR;
		*mono_counter_bits = MONO_CNT_2_BITS;
	} else {
		ERROR("bad mono counter index %d.\n", mono_cnt_index);
		return -2;
	}
#else
	ERROR("not available on this platform\n");
	return D_NOT_AVAILABLE_RETCODE;
#endif

	return 0;
}

int ambarella_otp_read_mono_counter(uint32_t *p,  uint32_t length,
	uint32_t mono_cnt_index)
{
	uint32_t i, j, val, cnt;
	uint32_t mono_counter_addr, mono_counter_bits;
	int ret;

	NOTICE("Reading the mono counter\n");

	if ((!p) || (!length)) {
		ERROR("zero buf or length.\n");
		return -1;
	}

	if (length != sizeof(uint32_t)) {
		ERROR("bad data length (%d).\n", length);
		return -1;
	}

	ret = get_mono_cnt_addr_bits(mono_cnt_index,
		&mono_counter_addr, &mono_counter_bits);
	if (0 > ret) {
		return ret;
	}

	cnt = 0;
	for (i = 0; i < mono_counter_bits; i += 32) {
		ret = ambarella_otp_read(mono_counter_addr + i, 32, &val);
		if (0 > ret) {
			ERROR("read mono counter failed, addr 0x%x\n", mono_counter_addr + i);
			return ret;
		}

		for (j = 0; j < 32; j++) {
			if (val & (1U << j)) {
				cnt ++;
			}
		}
	}
	*p = cnt;

	return 0;
}

int ambarella_otp_increase_mono_counter(uint32_t mono_cnt_index,
	uint32_t simulate)
{
	uint32_t i, j, val;
	uint32_t mono_counter_addr, mono_counter_bits;
	int ret;

	NOTICE("Increasing the monotonic counter\n");

	ret = get_mono_cnt_addr_bits(mono_cnt_index,
		&mono_counter_addr, &mono_counter_bits);
	if (0 > ret) {
		return ret;
	}

	for (i = 0; i < mono_counter_bits; i += 32) {
		ret = ambarella_otp_read(mono_counter_addr + i, 32, &val);
		if (0 > ret) {
			ERROR("read mono counter failed, addr 0x%x\n", mono_counter_addr + i);
			return ret;
		}

		for (j = 0; j < 32; j++) {
			if (!(val & (1U << j))) {
				val |= 1U << j;
				break;
			}
		}

		if (j >= 32)
			continue;

		if (simulate) {
			NOTICE("[Simulate OTP write]: increase mono counter %d: bit addr 0x%08x\n",
				mono_cnt_index, mono_counter_addr + i + j);
			return 0;
		}

		ret = ambarella_otp_write(mono_counter_addr + i + j, 1, 1);
		if (0 > ret) {
			ERROR("write mono counter failed, addr 0x%x\n", mono_counter_addr + i + j);
			return ret;
		}

		break;
	}

	return 0;
}

int ambarella_otp_permanently_enable_secure_boot(uint32_t simulate)
{
	uint32_t lock_bits = 0, v = 0;
	uint32_t sysconfig_lock_bit;
	uint32_t addr;
	int ret = 0;

#ifdef SYSCNFIG_LOCK_BIT
	sysconfig_lock_bit = LOCK_BIT_SYS_CONFIG;
#else
	sysconfig_lock_bit = 0;
#endif

	if (ambarella_otp_read(WRITE_LOCK_BIT_ADDR, 32, &lock_bits) < 0) {
		ERROR("enable secure boot: read lock bits failed\n");
		return -1;
	}

	if (!(lock_bits & (1U << sysconfig_lock_bit))) {

		addr = SYS_CONFIG_BIT_ADDR + SECURE_BOOT_BIT;
		NOTICE("Permanently enable secure boot: write OTP[0x%08x]\n", addr);
		if (!simulate) {
			ret= ambarella_otp_write(addr, 1, 1);
			if (0 > ret)
				return ret;
		} else {
			NOTICE("[Simulate OTP write]: write secure boot bit 0x%08x\n", addr);
		}

		addr = SYS_CONFIG_BIT_ADDR + SECURE_BOOT_BIT + 32;
		NOTICE("Permanently enable secure boot: write OTP[0x%08x]\n", addr);
		if (!simulate) {
			ret = ambarella_otp_write(addr, 1, 1);
			if (0 > ret)
				return ret;
		} else {
			NOTICE("[Simulate OTP write]: write secure boot mask bit 0x%08x\n", addr);
		}

		/* when enable secure boot, JTAG efuse shall be set also, otherwise JTAG is always enabled */
		addr = SYS_CONFIG_BIT_ADDR + JTAG_EFUSE_BIT;
		NOTICE("Permanently enable secure boot: write JTAG efuse.\n");
		if (!simulate) {
			ret = ambarella_otp_write(addr, 1, 1);
			if (0 > ret) {
				return ret;
			}
		} else {
			NOTICE("[Simulate OTP write]: write JTAG efuse 0x%08x\n", addr);
		}

		addr = WRITE_LOCK_BIT_ADDR + sysconfig_lock_bit;
		NOTICE("Permanently enable secure boot: lock sysconfig.\n");
		if (!simulate) {
			ret = ambarella_otp_write(addr, 1, 1);
			if (0 > ret) {
				return ret;
			}
		} else {
			NOTICE("[Simulate OTP write]: lock sysconfig 0x%08x\n", addr);
		}

		NOTICE("Permanently enable secure boot: done.\n");
	} else {
	   addr = SYS_CONFIG_BIT_ADDR + SECURE_BOOT_BIT;
	   ret = ambarella_otp_read(addr, 1, &v);
	   if (0 > ret) {
		   ERROR("error: read OTP[%d] failed.\n", addr);
		   return ret;
	   }
	   if (v & 0x1) {
		   addr = SYS_CONFIG_BIT_ADDR + SECURE_BOOT_BIT + 32;
		   ret = ambarella_otp_read(addr, 1, &v);
		   if (0 > ret) {
			   ERROR("error: read OTP[%d] failed.\n", addr);
			   return ret;
		   }
		   if (v & 0x1) {
			   NOTICE("Permanently enable secure boot: already enabled.\n");
		   } else {
			   ERROR("error: sysconfig already locked, but otp[%d] is 0.\n",
				   addr);
			   return -3;
		   }
	   } else {
		   ERROR("error: sysconfig already locked, but otp[%d] is 0.\n",
			   addr);
		   return -4;
	   }
	}

	return 0;
}

int ambarella_otp_read_huk(uint8_t *p, uint32_t length)
{
	int ret;

	if ((!p) || (!length)) {
		ERROR("zero buf or length.\n");
		return -1;
	}

	ret = store_embedded_flag(p,
		length, HUK_BITS / 8,
		LOCK_BIT_HUK_NONCE, "customer id");
	if (0 > ret) {
		return ret;
	}

	return ambarella_otp_read_field(HUK_ADDR, HUK_BITS, p);
}

int ambarella_otp_write_huk_nonce(uint8_t *p, uint32_t length)
{
	uint32_t lock_bit;
	uint32_t write_content = 0, write_lock = 0, simulate = 0;
	int ret;

	if ((!p) || (!length)) {
		ERROR("zero buf or length.\n");
		return -1;
	}

	lock_bit = LOCK_BIT_HUK_NONCE;
	ret = is_locked(lock_bit);
	if (0 < ret) {
		ERROR("HUK already locked\n");
		return D_LOCKED_RETCODE;
	} else if (0 > ret) {
		ERROR("Read HUK lock bit failed\n");
		return -4;
	}

	ret = check_length_get_embedded_flag(p,
		length, (HUK_BITS + HW_NONCE_BITS) / 8,
		&write_content, &write_lock, &simulate);
	if (0 > ret) {
		ERROR("length not expected (%d) for write huk nonce\n",
			length);
		return -5;
	}

	return ambarella_otp_write_field(HUK_ADDR,
		HUK_BITS + HW_NONCE_BITS,
		p, lock_bit,
		write_content, write_lock, simulate,
		"write huk nonce");
}

int ambarella_otp_read_aes_key(uint8_t *p, uint32_t length,
	uint32_t key_index)
{
	uint32_t aes_addr;
	int ret;

	if ((!p) || (!length)) {
		ERROR("zero buf or length.\n");
		return -1;
	}

	if ((key_index + 1) > AES_KEY_NUM) {
		ERROR("bad aeskey index %d.\n", key_index);
		return -1;
	}

	ret = store_embedded_flag(p,
		length, AES_KEY_BITS / 8,
		LOCK_BIT_AES_KEY_BASE + key_index, "aes key");
	if (0 > ret) {
		return ret;
	}

	aes_addr = AES_KEY_BASE_ADDR + key_index * AES_KEY_BITS;

	return ambarella_otp_read_field(aes_addr, AES_KEY_BITS, p);
}

int ambarella_otp_write_aes_key(uint8_t *p, uint32_t length,
	uint32_t key_index)
{
	uint32_t aes_addr, lock_bit;
	uint32_t write_content = 0, write_lock = 0, simulate = 0;
	int ret = 0;

	if (key_index + 1 > AES_KEY_NUM) {
		ERROR("bad aeskey index %d.\n", key_index);
		return -1;
	}

	lock_bit = LOCK_BIT_AES_KEY_BASE + key_index;
	ret = is_locked(lock_bit);
	if (0 < ret) {
		ERROR("AES key[%d] already locked\n", key_index);
		return D_LOCKED_RETCODE;
	} else if (0 > ret) {
		ERROR("Read AES key[%d] lock bit failed\n", key_index);
		return -4;
	}

	if (p && length) {
		ret = check_length_get_embedded_flag(p,
			length, AES_KEY_BITS / 8,
			&write_content, &write_lock, &simulate);
		if (0 > ret) {
			ERROR("length not expected (%d) for write aes key\n",
				length);
			return -5;
		}

		aes_addr = AES_KEY_BASE_ADDR + key_index * AES_KEY_BITS;

		return ambarella_otp_write_field(aes_addr, AES_KEY_BITS,
			p, lock_bit,
			write_content, write_lock, simulate,
			"write aes key");
	} else {
		ERROR("zero data or length.\n");
		return -6;
	}

	return ret;
}

int ambarella_otp_read_ecc_key(uint8_t *p, uint32_t length,
	uint32_t key_index)
{
	uint32_t ecc_addr;
	int ret;

	if ((!p) || (!length)) {
		ERROR("zero buf or length.\n");
		return -1;
	}

	if ((key_index + 1) > ECC_KEY_NUM) {
		ERROR("bad ecckey index %d.\n", key_index);
		return -1;
	}

	ret = store_embedded_flag(p,
		length, ECC_KEY_BITS / 8,
		LOCK_BIT_ECC_KEY_BASE + key_index, "ecc key");
	if (0 > ret) {
		return ret;
	}

	ecc_addr = ECC_KEY_BASE_ADDR + key_index * ECC_KEY_BITS;

	return ambarella_otp_read_field(ecc_addr, ECC_KEY_BITS, p);
}

int ambarella_otp_write_ecc_key(uint8_t *p, uint32_t length,
	uint32_t key_index)
{
	uint32_t ecc_addr, lock_bit;
	uint32_t write_content = 0, write_lock = 0, simulate = 0;
	int ret = 0;

	if ((key_index + 1) > ECC_KEY_NUM) {
		ERROR("bad ecckey index %d.\n", key_index);
		return -1;
	}

	lock_bit = LOCK_BIT_ECC_KEY_BASE + key_index;
	ret = is_locked(lock_bit);
	if (0 < ret) {
		ERROR("ECC key[%d] already locked\n", key_index);
		return D_LOCKED_RETCODE;
	} else if (0 > ret) {
		ERROR("Read ECC key[%d] lock bit failed\n", key_index);
		return -4;
	}

	if (p && length) {
		ret = check_length_get_embedded_flag(p,
			length, ECC_KEY_BITS / 8,
			&write_content, &write_lock, &simulate);
		if (0 > ret) {
			ERROR("length not expected (%d) for write ecc key\n",
				length);
			return -5;
		}

		ecc_addr = ECC_KEY_BASE_ADDR + key_index * ECC_KEY_BITS;

		return ambarella_otp_write_field(ecc_addr, ECC_KEY_BITS,
			p, lock_bit,
			write_content, write_lock, simulate,
			"write ecc key");
	} else {
		ERROR("zero data or length.\n");
		return -6;
	}

	return ret;
}

int ambarella_otp_read_user_slot_g0(uint8_t *p, uint32_t length,
	uint32_t slot_index)
{
	uint32_t slot_addr;
	int ret;

	if ((!p) || (!length)) {
		ERROR("zero buf or length.\n");
		return -1;
	}

	if ((slot_index + 1) > USR_SLOT_G0_NUM) {
		ERROR("bad user slot group 0 index %d.\n", slot_index);
		return -1;
	}

	ret = store_embedded_flag(p,
		length, USR_SLOT_G0_BITS / 8,
		LOCK_BIT_USR_SLOT_G0_BASE + slot_index, "usr slot g0");
	if (0 > ret) {
		return ret;
	}

	slot_addr = USR_SLOT_G0_ADDR + slot_index * USR_SLOT_G0_BITS;

	return ambarella_otp_read_field(slot_addr, USR_SLOT_G0_BITS, p);
}

int ambarella_otp_write_user_slot_g0(uint8_t *p, uint32_t length,
	uint32_t slot_index)
{
	uint32_t slot_addr, lock_bit;
	uint32_t write_content = 0, write_lock = 0, simulate = 0;
	int ret = 0;

	if ((slot_index + 1) > USR_SLOT_G0_NUM) {
		ERROR("bad user slot group0 index %d.\n", slot_index);
		return -1;
	}

	lock_bit = LOCK_BIT_USR_SLOT_G0_BASE + slot_index;
	ret = is_locked(lock_bit);
	if (0 < ret) {
		ERROR("User slot group0 [%d] already locked\n", slot_index);
		return D_LOCKED_RETCODE;
	} else if (0 > ret) {
		ERROR("Read User slot group0 [%d] lock bit failed\n", slot_index);
		return -4;
	}

	if (p && length) {
		ret = check_length_get_embedded_flag(p,
			length, USR_SLOT_G0_BITS / 8,
			&write_content, &write_lock, &simulate);
		if (0 > ret) {
			ERROR("length not expected (%d) for write usr slot g0\n",
				length);
			return -5;
		}

		slot_addr = USR_SLOT_G0_ADDR + slot_index * USR_SLOT_G0_BITS;

		return ambarella_otp_write_field(slot_addr, USR_SLOT_G0_BITS,
			p, lock_bit,
			write_content, write_lock, simulate,
			"write usr slot g0");
	} else {
		ERROR("zero data or length.\n");
		return -6;
	}

	return ret;
}

int ambarella_otp_read_user_slot_g1(uint8_t *p, uint32_t length,
	uint32_t slot_index)
{
#if defined (OTP_LAYOUT_V2)
	uint32_t slot_addr;
	int ret;

	if ((!p) || (!length)) {
		ERROR("zero buf or length.\n");
		return -1;
	}

	if ((slot_index + 1) > USR_SLOT_G1_NUM) {
		ERROR("bad user slot group1 index %d.\n", slot_index);
		return -1;
	}

	ret = store_embedded_flag(p,
		length, USR_SLOT_G1_BITS / 8,
		LOCK_BIT_USR_SLOT_G1_BASE + slot_index, "usr slot g1");
	if (0 > ret) {
		return ret;
	}

	slot_addr = USR_SLOT_G1_ADDR + slot_index * USR_SLOT_G1_BITS;

	return ambarella_otp_read_field(slot_addr, USR_SLOT_G1_BITS, p);
#else
	ERROR("not available on this platform\n");
	return D_NOT_AVAILABLE_RETCODE;
#endif
}

int ambarella_otp_write_user_slot_g1(uint8_t *p, uint32_t length,
	uint32_t slot_index)
{
#if defined (OTP_LAYOUT_V2)
	uint32_t slot_addr, lock_bit;
	uint32_t write_content = 0, write_lock = 0, simulate = 0;
	int ret = 0;

	if (slot_index + 1 > USR_SLOT_G1_NUM) {
		ERROR("bad user slot group1 index %d.\n", slot_index);
		return -1;
	}

	lock_bit = LOCK_BIT_USR_SLOT_G1_BASE + slot_index;
	ret = is_locked(lock_bit);
	if (0 < ret) {
		ERROR("User slot group1 [%d] already locked\n", slot_index);
		return D_LOCKED_RETCODE;
	} else if (0 > ret) {
		ERROR("Read User slot group1 [%d] lock bit failed\n", slot_index);
		return -4;
	}

	if (p && length) {
		ret = check_length_get_embedded_flag(p,
			length, USR_SLOT_G1_BITS / 8,
			&write_content, &write_lock, &simulate);
		if (0 > ret) {
			ERROR("length not expected (%d) for write usr slot g1\n",
				length);
			return -5;
		}

		slot_addr = USR_SLOT_G1_ADDR + slot_index * USR_SLOT_G1_BITS;
		return ambarella_otp_write_field(slot_addr, USR_SLOT_G1_BITS,
			p, lock_bit,
			write_content, write_lock, simulate,
			"write usr slot g1");
	} else {
		ERROR("zero data or length.\n");
		return -6;
	}

	return ret;
#else
	ERROR("not available on this platform\n");
	return D_NOT_AVAILABLE_RETCODE;
#endif
}


int ambarella_otp_read_user_data_g0(uint8_t *p, uint32_t length,
	uint32_t data_index)
{
#if defined (OTP_LAYOUT_V2)
	uint32_t data_addr;

	if ((!p) || (!length)) {
		ERROR("zero buf or length.\n");
		return -1;
	}

	if ((data_index + 1) > USR_DATA_G0_NUM) {
		ERROR("bad user data group0 index %d.\n", data_index);
		return -1;
	}

	if (length < (USR_DATA_G0_BITS / 8)) {
		ERROR("buffer length (%d) is not enough, (need %d) for read data g0.\n",
			length, (USR_DATA_G0_BITS / 8));
		return -2;
	}

	data_addr = USR_DATA_G0_ADDR + data_index * USR_DATA_G0_BITS;

	return ambarella_otp_read_field(data_addr, USR_DATA_G0_BITS, p);
#else
	ERROR("not available on this platform\n");
	return D_NOT_AVAILABLE_RETCODE;
#endif
}

int ambarella_otp_write_user_data_g0(uint8_t *p, uint32_t length,
	uint32_t data_index)
{
#if defined (OTP_LAYOUT_V2)
	uint32_t data_addr;
	uint32_t write_content = 1, write_lock = 0, simulate = 0;
	int ret = 0;

	if (data_index + 1 > USR_DATA_G0_NUM) {
		ERROR("bad user data group0 index %d.\n", data_index);
		return -1;
	}

	ret = check_length_get_embedded_flag(p,
		length, USR_DATA_G0_BITS / 8,
		&write_content, &write_lock, &simulate);
	if (0 > ret) {
		ERROR("length not expected (%d) for write data g0\n",
			length);
		return -5;
	}

	data_addr = USR_DATA_G0_ADDR + data_index * USR_DATA_G0_BITS;

	return ambarella_otp_write_field(data_addr, USR_SLOT_G0_BITS,
		p, -1,
		write_content, 0, simulate,
		"write usr data g0");
#else
	ERROR("not available on this platform\n");
	return D_NOT_AVAILABLE_RETCODE;
#endif
}

int ambarella_otp_read_test_region(uint8_t *p, uint32_t length)
{
	int ret = 0;

	if ((!p) || (!length)) {
		ERROR("zero buf or length.\n");
		return -1;
	}

	ret = store_embedded_flag(p,
		length, TEST_REGION_BITS / 8,
		LOCK_BIT_TEST_REGION, "test region");
	if (0 > ret) {
		return ret;
	}
	return ambarella_otp_read_field(TEST_REGION_ADDR, TEST_REGION_BITS, p);
}

int ambarella_otp_write_test_region(uint8_t *p, uint32_t length)
{
	uint32_t write_content = 1, write_lock = 0, simulate = 0;
	int ret;

	ret = is_locked(LOCK_BIT_TEST_REGION);
	if (ret) {
		ERROR("test region locked\n");
		return D_LOCKED_RETCODE;
	}

	if (p && length) {
		ret = check_length_get_embedded_flag(p,
			length, TEST_REGION_BITS / 8,
			&write_content, &write_lock, &simulate);
		if (0 > ret) {
			ERROR("length not expected (%d) for write test region\n",
				length);
			return -5;
		}

		return ambarella_otp_write_field(TEST_REGION_ADDR, TEST_REGION_BITS,
			p, LOCK_BIT_TEST_REGION,
			write_content, write_lock, simulate,
			"write test region");
	} else {
		ERROR("zero data or length.\n");
		return -6;
	}

	return 0;
}

int ambarella_otp_revoke_key(uint32_t index, uint32_t simulate)
{
	uint32_t v;
	uint32_t invalid_bit;
	int ret;

	if (ROT_KEY_NUM <= index) {
		ERROR("error: bad revoke key index %d\n", index);
		return -1;
	}

	if (NON_REVOKABLE_KEY_INDEX == index) {
		ERROR("error: key index %d is not revokable\n", index);
		return -1;
	}

	NOTICE("Revoke key %d\n", index);

	invalid_bit = DATA_INVALID_BIT_ADDR + index;

	if (simulate) {
		NOTICE("[Simulate OTP write]: Revoke key %d, write invalid bit 0x%08x\n",
			index, invalid_bit);
		return 0;
	}

	ret = ambarella_otp_write(invalid_bit, 1, 1);
	if (ret < 0) {
		ERROR("write 0x%x failed, revoke(%d) failed\n",
			invalid_bit, index);
		return ret;
	}

	ret = ambarella_otp_read(invalid_bit, 1, &v);
	if (ret < 0) {
		ERROR("write 0x%x, read back failed, revoke(%d) failed\n",
			invalid_bit, index);
		return ret;
	}

	if (v != 1) {
		ERROR("invalid bit (0x%x) read back not expected, revoke key %d failed\n",
			invalid_bit, index);
		return -3;
	}

	return 0;
}

int ambarella_otp_query_otp_setting(uint32_t *p, uint32_t length)
{
	otp_setting_t *setting = (otp_setting_t *) p;
	unsigned int sysconfig_lock_bit = 0;
#if defined (OTP_LAYOUT_V2)
    unsigned int v;
#endif

	if (!p || length != (sizeof(otp_setting_t))) {
		ERROR("invalid params %p, %d.\n", p, length);
		return -1;
	}

#ifdef SYSCNFIG_LOCK_BIT
	sysconfig_lock_bit = LOCK_BIT_SYS_CONFIG;
#else
	sysconfig_lock_bit = 0;
#endif

#if defined (OTP_LAYOUT_V1)
	setting->otp_layout_ver = 1;
	setting->not_revokeable_key_index = 2;
#elif defined (OTP_LAYOUT_V2)
    setting->otp_layout_ver = 2;
	setting->not_revokeable_key_index = 0;
#endif

	if (ambarella_otp_read(WRITE_LOCK_BIT_ADDR, 32, &setting->lock_bits) < 0) {
		ERROR("read lock bit failed.\n");
		return -2;
	}

	if (ambarella_otp_read(DATA_INVALID_BIT_ADDR, 32, &setting->invalid_bits) < 0) {
		ERROR("read data invalid bit failed.\n");
		return -3;
	}

	if (ambarella_otp_read(SYS_CONFIG_BIT_ADDR, 32, &setting->sysconfig) < 0) {
		ERROR("read sys config failed.\n");
		return -4;
	}

	if (ambarella_otp_read(SYS_CONFIG_BIT_ADDR + 32, 32, &setting->sysconfig_mask) < 0) {
		ERROR("read sys config mask failed.\n");
		return -5;
	}

	if ((setting->sysconfig & (1U << SECURE_BOOT_BIT))
		&& (setting->sysconfig_mask & (1U << SECURE_BOOT_BIT))) {
		setting->secure_boot_permanent_en = 1;
	} else {
		setting->secure_boot_permanent_en = 0;
	}

	if (setting->sysconfig_mask & (1U << (JTAG_EFUSE_BIT - 32))) {
		setting->jtag_efuse = 1;
	} else {
		setting->jtag_efuse = 0;
	}

	if (setting->lock_bits & (1U << sysconfig_lock_bit)) {
		setting->sysconfig_locked = 1;
	} else {
		setting->sysconfig_locked = 0;
	}

	if (setting->lock_bits & (1U << LOCK_BIT_HUK_NONCE)) {
		setting->huk_locked = 1;
	} else {
		setting->huk_locked = 0;
	}

	if (setting->lock_bits & (1U << LOCK_BIT_CUSTOMER_ID)) {
		setting->customer_id_locked = 1;
	} else {
		setting->customer_id_locked = 0;
	}

	if (setting->lock_bits & (1U << LOCK_BIT_A)) {
		setting->zone_a_locked = 1;
	} else {
		setting->zone_a_locked = 0;
	}

#if defined (OTP_LAYOUT_V1)
	setting->cst_seed_cuk_locked = 0;
	setting->usr_cuk_locked = 0;
	setting->anti_rollback_en = 0;
	setting->secure_usb_boot_dis = 0;
	setting->all_rot_key_lock_together = 0;
#elif defined (OTP_LAYOUT_V2)
	if (setting->lock_bits & (1 << LOCK_BIT_CST_PLANTED_SEED_CUK)) {
		setting->cst_seed_cuk_locked = 1;
	} else {
		setting->cst_seed_cuk_locked = 0;
	}
	if (setting->lock_bits & (1 << LOCK_BIT_USR_PLANTED_CUK)) {
		setting->usr_cuk_locked = 1;
	} else {
		setting->usr_cuk_locked = 0;
	}
	if (ambarella_otp_read(EN_ANTI_ROLLBACK_BIT, 1, &v) < 0) {
		ERROR("read bst anti rollback failed.\n");
		return -6;
	}
	if (v & 0x1) {
		setting->anti_rollback_en = 1;
	} else {
		setting->anti_rollback_en = 0;
	}
	if (ambarella_otp_read(DIS_SECURE_USB_BOOT_BIT, 1, &v) < 0) {
		ERROR("read disable secure usb boot failed.\n");
		return -7;
	}
	if (v & 0x1) {
		setting->secure_usb_boot_dis = 1;
	} else {
		setting->secure_usb_boot_dis = 0;
	}
	setting->all_rot_key_lock_together = 1;
#endif

	setting->rot_pubkey_lock_base = LOCK_BIT_ROT_BASE;
	setting->aes_key_lock_base = LOCK_BIT_AES_KEY_BASE;
	setting->ecc_key_lock_base = LOCK_BIT_ECC_KEY_BASE;
	setting->usr_slot_g0_lock_base = LOCK_BIT_USR_SLOT_G0_BASE;
#if defined (OTP_LAYOUT_V1)
	setting->usr_slot_g1_lock_base = 0;
#elif defined (OTP_LAYOUT_V2)
	setting->usr_slot_g1_lock_base = LOCK_BIT_USR_SLOT_G1_BASE;
#endif

	setting->num_of_rot_pub_keys = ROT_KEY_NUM;
	setting->num_of_aes_keys = AES_KEY_NUM;
	setting->num_of_ecc_keys = ECC_KEY_NUM;
	setting->num_of_usr_slot_g0 = USR_SLOT_G0_NUM;
#if defined (OTP_LAYOUT_V1)
	setting->num_of_usr_slot_g1 = 0;
#elif defined (OTP_LAYOUT_V2)
	setting->num_of_usr_slot_g1 = USR_SLOT_G1_NUM;
#endif

	return 0;
}

int ambarella_otp_set_jtag_efuse(uint32_t simulate)
{
	uint32_t lock_bits = 0, jtag = 0;
	unsigned int sysconfig_lock_bit;
	unsigned int addr;

#ifdef SYSCNFIG_LOCK_BIT
	sysconfig_lock_bit = LOCK_BIT_SYS_CONFIG;
#else
	sysconfig_lock_bit = 0;
#endif

	if (ambarella_otp_read(WRITE_LOCK_BIT_ADDR, 1, &lock_bits) < 0)
		return -1;

	addr = SYS_CONFIG_BIT_ADDR + JTAG_EFUSE_BIT;
	if (!(lock_bits & (1U << sysconfig_lock_bit))) {
		if (ambarella_otp_read(addr, 1, &jtag) < 0) {
			ERROR("read otp[0x%08x] failed.\n", addr);
			return -1;
		}

		if (jtag & 0x1) {
			NOTICE("JTAG efuse already set.\n");
		} else {
			if (!simulate) {
				if (ambarella_otp_write(addr, 1, 1) < 0)
					return -2;

				if (ambarella_otp_read(addr, 1, &jtag) < 0)
					return -3;

				if (jtag == 0) {
					ERROR("Set JTAG efuse failed: lock bit = 0x%08x\n", lock_bits);
					return -4;
				}

				NOTICE("Set JTAG efuse done\n");
			} else {
				NOTICE("[Simulate OTP write]: set JTAG efuse, write bit 0x%08x\n",
					addr);
			}
		}
	} else {

		if (ambarella_otp_read(addr, 1, &jtag) < 0) {
			NOTICE("read otp[0x%08x] failed.\n", addr);
			return -5;
		}
		if (jtag & 0x1) {
			NOTICE("JTAG efuse is already set.\n");
		} else {
			ERROR("sysconfig already locked, JTAG efuse is not set.\n");
			return -6;
		}
	}

	return 0;
}

int ambarella_otp_lock_zone_a(uint32_t simulate)
{
#ifndef SYSCNFIG_LOCK_BIT
#ifdef D_LOCK_ZONE_A_EXPLICITE
	uint32_t v = 0;

	if (simulate) {
		NOTICE("[Simulate OTP write]: lock zone a, write bit 0x%08x\n",
				WRITE_LOCK_BIT_ADDR);
		return 0;
	}

	if (ambarella_otp_write(WRITE_LOCK_BIT_ADDR, 1, 1) < 0)
		return -1;

	if (ambarella_otp_read(WRITE_LOCK_BIT_ADDR, 1, &v) < 0)
		return -2;

	if (v != 1) {
		ERROR("Lock zone a failed.\n");
		return -3;
	}

	NOTICE("Lock zone a done!\n");
#endif
#else
	NOTICE("Chip does not need customer lock zone A\n");
#endif
	return 0;
}

int ambarella_otp_read_sysconfig(uint8_t *p, uint32_t length)
{
	int ret;
	unsigned int sysconfig_lock_bit;

	if ((!p) || (!length)) {
		ERROR("zero buf or length.\n");
		return -1;
	}

	NOTICE("Reading the sysconfig\n");

#ifdef SYSCNFIG_LOCK_BIT
	sysconfig_lock_bit = LOCK_BIT_SYS_CONFIG;
#else
	sysconfig_lock_bit = 0;
#endif

	ret = store_embedded_flag(p,
		length, SYS_CONFIG_BITS / 8,
		sysconfig_lock_bit, "sysconfig");
	if (0 > ret) {
		return ret;
	}
	return ambarella_otp_read_field(SYS_CONFIG_BIT_ADDR, SYS_CONFIG_BITS, p);
}

int ambarella_otp_write_sysconfig(uint8_t *p, uint32_t length)
{
	uint32_t sysconfig_lock_bit;
	uint32_t write_content = 0, write_lock = 0, simulate = 0;
	int ret = 0;

#ifdef SYSCNFIG_LOCK_BIT
	sysconfig_lock_bit = LOCK_BIT_SYS_CONFIG;
#else
	sysconfig_lock_bit = 0;
#endif

	NOTICE("Writing the sysconfig\n");

	/* check lock status */
	ret = is_locked(sysconfig_lock_bit);
	if (0 < ret) {
		ERROR("sysconfig and its selection already locked\n");
		return D_LOCKED_RETCODE;
	} else if (0 > ret) {
		ERROR("read lock bit failed\n");
		return -4;
	}

	if (p && length) {
		ret = check_length_get_embedded_flag(p,
			length, SYS_CONFIG_BITS / 8,
			&write_content, &write_lock, &simulate);
		if (0 > ret) {
			ERROR("length not expected (%d) for write sys config\n",
				length);
			return -5;
		}

		return ambarella_otp_write_field(SYS_CONFIG_BIT_ADDR,
			SYS_CONFIG_BITS, p, sysconfig_lock_bit,
			write_content, write_lock, simulate,
			"write sysconfig");
	} else {
		ERROR("zero data or length.\n");
		return -6;
	}

	return ret;
}

int ambarella_otp_read_cst_planted_seed(uint8_t *p, uint32_t length)
{
#if defined (OTP_LAYOUT_V2)
	int ret;

	if ((!p) || (!length)) {
		ERROR("zero buf or length.\n");
		return -1;
	}

	INFO("Reading the cst seed\n");

	ret = store_embedded_flag(p,
		length, CST_PLANTED_SEED_BITS / 8,
		LOCK_BIT_CST_PLANTED_SEED_CUK, "cst seed");
	if (0 > ret) {
		return ret;
	}
	return ambarella_otp_read_field(CST_PLANTED_SEED_ADDR, CST_PLANTED_SEED_BITS, p);
#else
	ERROR("not available on this platform\n");
	return D_NOT_AVAILABLE_RETCODE;
#endif
}

int ambarella_otp_read_cst_planted_cuk(uint8_t *p, uint32_t length)
{
#if defined (OTP_LAYOUT_V2)
	int ret;

	if ((!p) || (!length)) {
		ERROR("zero buf or length.\n");
		return -1;
	}

	INFO("Reading the cst cuk\n");

	ret = store_embedded_flag(p,
		length, CST_PLANTED_CUK_BITS / 8,
		LOCK_BIT_CST_PLANTED_SEED_CUK, "cst cuk");
	if (0 > ret) {
		return ret;
	}
	return ambarella_otp_read_field(CST_PLANTED_CUK_ADDR, CST_PLANTED_CUK_BITS, p);
#else
	ERROR("not available on this platform\n");
	return D_NOT_AVAILABLE_RETCODE;
#endif
}

int ambarella_otp_write_cst_planted_seed_and_cuk(uint8_t *p,
	uint32_t length)
{
#if defined (OTP_LAYOUT_V2)
	uint32_t write_content = 0, write_lock = 0, simulate = 0;
	int ret = 0;

	INFO("Writing customer seed and CUK\n");

	/* check lock status */
	ret = is_locked(LOCK_BIT_CST_PLANTED_SEED_CUK);
	if (0 < ret) {
		ERROR("cst seed and secret already locked\n");
		return D_LOCKED_RETCODE;
	} else if (0 > ret) {
		ERROR("read lock bit failed\n");
		return -4;
	}

	if (p && length) {

		ret = check_length_get_embedded_flag(p,
			length, (CST_PLANTED_SEED_BITS + CST_PLANTED_CUK_BITS) / 8,
			&write_content, &write_lock, &simulate);
		if (0 > ret) {
			ERROR("length not expected (%d) for write cst seed and cuk\n",
				length);
			return -5;
		}

		ret = ambarella_otp_write_field(CST_PLANTED_SEED_ADDR,
			CST_PLANTED_SEED_BITS, p, 0,
			write_content, 0, simulate, "write cst seed");
		if (0 > ret) {
			ERROR("write cst seed failed\n");
			return ret;
		}

		ret = ambarella_otp_write_field(CST_PLANTED_CUK_ADDR,
			CST_PLANTED_CUK_BITS, p + (CST_PLANTED_SEED_BITS / 8),
			LOCK_BIT_CST_PLANTED_SEED_CUK,
			write_content, write_lock, simulate,
			"write cst cuk");
		if (0 > ret) {
			ERROR("write cst cuk failed\n");
			return ret;
		}
	} else {
		ERROR("zero data or length.\n");
		return -6;
	}

	return ret;
#else
	ERROR("not available on this platform\n");
	return D_NOT_AVAILABLE_RETCODE;
#endif
}

int ambarella_otp_read_user_planted_cuk(uint8_t *p, uint32_t length)
{
#if defined (OTP_LAYOUT_V2)
	int ret;

	if ((!p) || (!length)) {
		ERROR("zero buf or length.\n");
		return -1;
	}

	INFO("Reading the user planted cuk\n");

	ret = store_embedded_flag(p,
		length, USR_PLANTED_CUK_BITS / 8,
		LOCK_BIT_USR_PLANTED_CUK, "cst cuk");
	if (0 > ret) {
		return ret;
	}
	return ambarella_otp_read_field(USR_PLANTED_CUK_ADDR, USR_PLANTED_CUK_BITS, p);
#else
	ERROR("not available on this platform\n");
	return D_NOT_AVAILABLE_RETCODE;
#endif
}


int ambarella_otp_write_user_planted_cuk(uint8_t *p, uint32_t length)
{
#if defined (OTP_LAYOUT_V2)
	uint32_t write_content = 0, write_lock = 0, simulate = 0;
	int ret = 0;

	INFO("Writing user planted cuk\n");

	/* check lock status */
	ret = is_locked(LOCK_BIT_USR_PLANTED_CUK);
	if (0 < ret) {
		ERROR("user planted cuk already locked\n");
		return D_LOCKED_RETCODE;
	} else if (0 > ret) {
		ERROR("read lock bit failed\n");
		return -4;
	}

	if (p && length) {
		ret = check_length_get_embedded_flag(p,
			length, USR_PLANTED_CUK_BITS / 8,
			&write_content, &write_lock, &simulate);
		if (0 > ret) {
			ERROR("length not expected (%d) for write usr seed\n",
				length);
			return -5;
		}

		ret = ambarella_otp_write_field(USR_PLANTED_CUK_ADDR,
			USR_PLANTED_CUK_BITS, p, LOCK_BIT_USR_PLANTED_CUK,
			write_content, write_lock, simulate,
			"write usr cuk");
		if (0 > ret) {
			ERROR("write user planted cuk failed\n");
			return ret;
		}
	} else {
		ERROR("zero data or length.\n");
		return -6;
	}

	return ret;
#else
	ERROR("not available on this platform\n");
	return D_NOT_AVAILABLE_RETCODE;
#endif
}

int ambarella_otp_read_bst_ver(uint32_t *p, uint32_t length)
{
#if defined (OTP_LAYOUT_V2)
	uint32_t i, j, val, cnt;
	int ret;

	NOTICE("Reading the bst version counter\n");

	if (length != sizeof(uint32_t)) {
		ERROR("bad data length (%d).\n", length);
		return -1;
	}

	cnt = 0;
	for (i = 0; i < BST_VER_BITS; i += 32) {
		ret = ambarella_otp_read(BST_VER_ADDR + i, 32, &val);
		if (0 > ret) {
			ERROR("read bst version counter failed, addr 0x%x\n", BST_VER_ADDR + i);
			return ret;
		}

		for (j = 0; j < 32; j++) {
			if (val & (1U << j)) {
				cnt ++;
			}
		}
	}
	*p = cnt;

	return 0;
#else
	ERROR("not available on this platform\n");
	return D_NOT_AVAILABLE_RETCODE;
#endif
}

int ambarella_otp_increase_bst_ver(uint32_t simulate)
{
#if defined (OTP_LAYOUT_V2)
	uint32_t i, j, val;
	int ret;

	NOTICE("Increasing the bst version counter\n");

	for (i = 0; i < BST_VER_BITS; i += 32) {
		ret = ambarella_otp_read(BST_VER_ADDR + i, 32, &val);
		if (0 > ret) {
			ERROR("read bst version counter failed, addr 0x%x\n", BST_VER_ADDR + i);
			return ret;
		}

		for (j = 0; j < 32; j++) {
			if (!(val & (1U << j))) {
				val |= 1U << j;
				break;
			}
		}

		if (j >= 32)
			continue;

		if (simulate) {
			NOTICE("[Simulate OTP write]: increase bst counter: bit addr 0x%08x\n",
				BST_VER_ADDR + i + j);
			return 0;
		}
		ret = ambarella_otp_write(BST_VER_ADDR + i + j, 1, 1);
		if (0> ret) {
			ERROR("write bst version counter failed, addr 0x%x\n", BST_VER_ADDR + i + j);
			return ret;
		}

		break;
	}

	return 0;
#else
	ERROR("not available on this platform\n");
	return D_NOT_AVAILABLE_RETCODE;
#endif
}

int ambarella_otp_set_bst_anti_rollback(uint32_t simulate)
{
#if defined (OTP_LAYOUT_V2)
	int ret = 0;

	NOTICE("Set BST anti-rollback\n");

	if (simulate) {
		NOTICE("[Simulate OTP write]: enable bst anti rollback: bit addr 0x%08x\n",
			EN_ANTI_ROLLBACK_BIT);
		return 0;
	}

	ret = ambarella_otp_write(EN_ANTI_ROLLBACK_BIT, 1, 1);
	if (0 > ret) {
		NOTICE("Set BST anti-rollback failed, ret %d\n", ret);
		return ret;
	}

	NOTICE("Set BST anti-rollback done\n");

	return 0;
#else
	ERROR("not available on this platform\n");
	return D_NOT_AVAILABLE_RETCODE;
#endif
}

int ambarella_otp_disable_secure_usb_boot(uint32_t simulate)
{
#if defined (OTP_LAYOUT_V2)
	int ret = 0;

	NOTICE("Disable Secure USB boot\n");

	if (simulate) {
		NOTICE("[Simulate OTP write]: disable Secure USB boot: bit addr 0x%08x\n",
			DIS_SECURE_USB_BOOT_BIT);
		return 0;
	}

	ret = ambarella_otp_write(DIS_SECURE_USB_BOOT_BIT, 1, 1);
	if (0 > ret) {
		NOTICE("Disable Secure USB boot failed, ret %d\n", ret);
		return ret;
	}

	NOTICE("Disable Secure USB boot done\n");

	return 0;
#else
	ERROR("not available on this platform\n");
	return D_NOT_AVAILABLE_RETCODE;
#endif
}

int ambarella_otp_read_misc_config(uint32_t *p)
{
	int ret = 0;

	if (!p) {
		ERROR("zero buf\n");
		return -1;
	}

	INFO("Reading misc config\n");

	ret = ambarella_otp_read(MISC_CONFIG_ADDR, MISC_CONFIG_BITS, p);
	if (0 > ret)
		return ret;

	p[1] = MISC_CONFIG_BITS;

	return 0;
}

int ambarella_otp_write_misc_config(uint32_t misc_config, uint32_t simulate)
{
	if (!simulate) {
		int ret = 0;

		ret = ambarella_otp_write(MISC_CONFIG_ADDR, MISC_CONFIG_BITS, misc_config);
		if (0 > ret)
			return ret;
	} else {
		NOTICE("[Simulate OTP write]: write misc config: bit addr 0x%08x, size %d\n",
			MISC_CONFIG_ADDR, MISC_CONFIG_BITS);
	}

	return 0;
}

extern int ambarella_otp_nw_query_otp_setting(
	unsigned long *sys_config, unsigned long *lock_bit_invalid_bit)
{
	unsigned int sys_conf = 0, sys_conf_mask = 0;
	unsigned int lock_bit = 0, invalid_bit = 0;

	if (ambarella_otp_read(SYS_CONFIG_BIT_ADDR, 32, &sys_conf) < 0) {
		ERROR("read sys config failed.\n");
		return -2;
	}

	if (ambarella_otp_read(SYS_CONFIG_BIT_ADDR + 32, 32, &sys_conf_mask) < 0) {
		ERROR("read sys config mask failed.\n");
		return -3;
	}

	if (ambarella_otp_read(WRITE_LOCK_BIT_ADDR, 32, &lock_bit) < 0) {
		ERROR("read lock bit failed.\n");
		return -4;
	}

	if (ambarella_otp_read(DATA_INVALID_BIT_ADDR, 32, &invalid_bit) < 0) {
		ERROR("read data invalid bit failed.\n");
		return -5;
	}

	*sys_config = ((unsigned long) sys_conf)
		| (((unsigned long) sys_conf_mask) << 32);
	*lock_bit_invalid_bit = ((unsigned long) lock_bit)
		| (((unsigned long) invalid_bit) << 32);

	return 0;
}

