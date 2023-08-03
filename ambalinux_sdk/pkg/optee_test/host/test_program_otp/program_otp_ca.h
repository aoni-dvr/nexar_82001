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

#ifndef MOUDLE_PROGRAM_OTP_CA_H_
#define MOUDLE_PROGRAM_OTP_CA_H_

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
 *  OTP efuse bit and write lock bit
 */

#define SECURE_BOOT_BIT					6
#define JTAG_EFUSE_BIT					63

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
 *  OTP Monotonic counter, OTP_LAYOUT_V2 have three momo counts
 */
#define MONO_CNT_0_BITS		256
#if defined (OTP_LAYOUT_V1)
#define NUM_OF_MONO_CNT		1
#elif defined (OTP_LAYOUT_V2)
#define NUM_OF_MONO_CNT		3
#define MONO_CNT_1_BITS		256
#define MONO_CNT_2_BITS		512
#endif

/*
 *  OTP User Slot Group 0 (lockable)
 */
#define USR_SLOT_G0_NUM		6
#define USR_SLOT_G0_BITS		256

/*
 *  OTP User Slot Group 1 (lockable), OTP_LAYOUT_V2 only
 */
#define USR_SLOT_G1_NUM		4
#define USR_SLOT_G1_BITS		1024

/*
 *  OTP User Data Group 0 (not lockable), OTP_LAYOUT_V2 only
 */
#define USR_DATA_G0_NUM		3
#define USR_DATA_G0_BITS		1024

/*
 *  OTP Test Region
 */
#define TEST_REGION_BITS		128

/*
 *  other OTP fields, OTP_LAYOUT_V2 only
 */
#define CST_PLANTED_SEED_BITS		32
#define CST_PLANTED_CUK_BITS			128
#define MISC_CONFIG_BITS				14
#define BST_VER_BITS					96
#define USR_PLANTED_CUK_BITS		256


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

typedef struct {
    unsigned int api_ver_major : 16;
    unsigned int api_ver_minor : 8;
    unsigned int api_ver_patch : 8;

    unsigned int driver_ver_major : 16;
    unsigned int driver_ver_minor : 8;
    unsigned int driver_ver_patch : 8;

    unsigned int date_year : 16;
    unsigned int date_month : 8;
    unsigned int date_day : 8;
} otp_version_t;

extern void print_memory_u16_be(unsigned char *p, unsigned int size);
extern void print_memory_u8(unsigned char *p, unsigned int size);

extern TEEC_Result do_write_otp_pubkey(unsigned char *p_key, unsigned int len,
    unsigned int key_index, unsigned int lock, unsigned int simulate);
extern TEEC_Result do_read_otp_pubkey(unsigned char *p_key, unsigned int len,
    unsigned int key_index, unsigned int *locked, unsigned int *revoked);
extern TEEC_Result do_read_otp_pubkey_digest(unsigned char *p_key, unsigned int len,
    unsigned int key_index, unsigned int *locked, unsigned int *revoked);
extern TEEC_Result do_read_otp_amba_unique_id(unsigned char *p_id, unsigned int len);
extern TEEC_Result do_write_otp_customer_id(unsigned char *p_id, unsigned int len,
    unsigned int lock, unsigned int simulate);
extern TEEC_Result do_read_otp_customer_id(unsigned char *p_id, unsigned int len,
    unsigned int *locked);
extern TEEC_Result do_read_otp_mono_counter(unsigned int *counter,
    unsigned int mono_index);
extern TEEC_Result do_increase_otp_mono_counter(
    unsigned int mono_index, unsigned int simulate);
extern TEEC_Result do_permanently_enable_secure_boot(unsigned int simulate);
extern TEEC_Result do_generate_hw_unique_encryption_key(unsigned int simulate);
extern TEEC_Result do_revoke_key(unsigned int key_index, unsigned int simulate);

/* this is optional */
extern TEEC_Result do_write_aes_key(unsigned char *key, unsigned int length,
    unsigned int key_index, unsigned int lock, unsigned int simulate);

/* this is optional */
extern TEEC_Result do_write_ecc_key(unsigned char *key, unsigned int length,
    unsigned int key_index, unsigned int lock, unsigned int simulate);

extern TEEC_Result do_write_usr_slot_g0(unsigned char *content, unsigned int length,
    unsigned int index, unsigned int lock, unsigned int simulate);

/* this is optional */
extern TEEC_Result do_read_usr_slot_g0(unsigned char *content, unsigned int length,
    unsigned int index, unsigned int *locked);

extern TEEC_Result do_write_usr_slot_g1(unsigned char *content, unsigned int length,
    unsigned int index, unsigned int lock, unsigned int simulate);

/* this is optional */
extern TEEC_Result do_read_usr_slot_g1(unsigned char *content, unsigned int length,
    unsigned int index, unsigned int *locked);

extern TEEC_Result do_write_test_region(unsigned char *content, unsigned int length,
    unsigned int lock, unsigned int simulate);
extern TEEC_Result do_read_test_region(unsigned char *content, unsigned int length,
    unsigned int *locked);

extern TEEC_Result do_query_otp_setting(otp_setting_t *p_setting);

/* this is optional */
extern TEEC_Result do_set_jtag_efuse(unsigned int simulate);
extern TEEC_Result do_lock_zone_a(unsigned int simulate);

extern TEEC_Result do_enable_bst_anti_rollback(unsigned int simulate);
extern TEEC_Result do_lock_rot_pubkey(unsigned int index, unsigned int simulate);
extern TEEC_Result do_write_usr_data_g0(unsigned char *content, unsigned int length,
    unsigned int index, unsigned int simulate);
extern TEEC_Result do_read_usr_data_g0(unsigned char *content, unsigned int length,
    unsigned int index);
extern TEEC_Result do_write_sysconfig(unsigned char *content, unsigned int length,
    unsigned int lock, unsigned int simulate);
extern TEEC_Result do_read_sysconfig(unsigned char *content, unsigned int length,
    unsigned int *locked);
extern TEEC_Result do_write_cst_seed_cuk(unsigned char *content, unsigned int length,
    unsigned int lock, unsigned int simulate);
extern TEEC_Result do_read_cst_seed(unsigned char *content, unsigned int length,
    unsigned int *locked);
extern TEEC_Result do_read_cst_cuk(unsigned char *content, unsigned int length,
    unsigned int *locked);
extern TEEC_Result do_write_usr_cuk(unsigned char *content, unsigned int length,
    unsigned int lock, unsigned int simulate);
extern TEEC_Result do_read_usr_cuk(unsigned char *content, unsigned int length,
    unsigned int *locked);
extern TEEC_Result do_disable_secure_usb_boot(unsigned int simulate);
extern TEEC_Result do_increase_bst_version(unsigned int simulate);
extern TEEC_Result do_read_bst_version(unsigned int *version);
extern TEEC_Result do_read_misc_config(unsigned int *config, unsigned int *total_bits);
extern TEEC_Result do_write_misc_config(unsigned int config, unsigned int simulate);

extern int do_query_version(otp_version_t *version);

#endif

