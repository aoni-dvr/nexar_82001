/*
 * Copyright (c) 2016, ARM Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef __PLAT_PRIVATE_H__
#define __PLAT_PRIVATE_H__

#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <lib/utils_def.h>
#include <lib/mmio.h>
#include <ambarella_def.h>
#include <platform_def.h>
#include <boot_cookie.h>
#if defined(CONFIG_ATF_AMBALINK)
#include "../driver/ambarella_dram.h"
#endif


enum {
	NOTIFY_MCU_POWEROFF = 0,
	NOTIFY_MCU_SUSPEND,
	NOTIFY_MCU_RECOVERY,
};

/*
 * Ambarella Monitor. If unsure, comment out below line to disable memory monitor
 */
/* #define AMBARELLA_ENABLE_MEM_MONITOR */

extern void ambarella_relocate_suspend(void);

struct mmap_region;
extern void ambarella_mmap_setup(const struct mmap_region *mmap);

extern void ambarella_soc_fixup(void);

extern void ambarella_gic_driver_init(void);
extern void ambarella_gic_distif_init(void);
extern void ambarella_gic_pcpu_init(void);

extern void ambarella_gpio_init(void);
extern void ambarella_gpio_notify_mcu(uint32_t notify);
extern int ambarella_gpio_security_handle(uint64_t *value, u_register_t reg, uint32_t sas, uint32_t wnr);
extern int ambarella_pinctrl_security_handle(uint64_t *value, u_register_t reg, uint32_t sas, uint32_t wrn);

extern void ambarella_gpio_security_request(uintptr_t *reg, uint32_t bank);
extern void ambarella_pinctrl_security_request(uintptr_t *reg, uint32_t *mask);

extern int32_t ambarella_fdt_init(void);

extern uint32_t get_sys_timer_parent_freq_hz(void);
extern uint32_t get_core_bus_freq_hz(void);
extern uint32_t get_ddr_freq_hz(void);

extern void bl31_plat_prepare_kernel32_entry(uint64_t x1, uint64_t x2,
		uint64_t x3, uint64_t x4);

extern int ambarella_io_setup(void);
extern int ambarella_nand_init(uintptr_t *block_dev_spec);
extern int ambarella_spinor_init(uintptr_t *block_dev_spec);
extern int ambarella_emmc_init(uintptr_t *block_dev_spec);

extern int ambarella_otp_read(uint32_t bit_addr, uint32_t length, uint32_t *value);
extern int ambarella_otp_write(uint32_t bit_addr, uint32_t length, uint32_t value);

extern int ambarella_otp_read_amba_unique_id(uint8_t *p, uint32_t length);
extern int ambarella_otp_read_rot_pubkey(uint8_t *p, uint32_t length,
	uint32_t key_index);
extern int ambarella_otp_write_rot_pubkey(uint8_t *p, uint32_t length,
	uint32_t key_index);
extern int ambarella_otp_lock_rot_pubkey(uint32_t key_index, uint32_t simulate);
extern int ambarella_otp_read_customer_id(uint8_t *p, uint32_t length);
extern int ambarella_otp_write_customer_id(uint8_t *p, uint32_t length);
extern int ambarella_otp_read_mono_counter(uint32_t *p,  uint32_t length,
	uint32_t mono_cnt_index);
extern int ambarella_otp_increase_mono_counter(uint32_t mono_cnt_index,
	uint32_t simulate);
extern int ambarella_otp_permanently_enable_secure_boot(uint32_t simulate);
extern int ambarella_otp_read_huk(uint8_t *p, uint32_t length);
extern int ambarella_otp_write_huk_nonce(uint8_t *p, uint32_t length);
extern int ambarella_otp_read_aes_key(uint8_t *p, uint32_t length,
	uint32_t key_index);
extern int ambarella_otp_write_aes_key(uint8_t *p, uint32_t length,
	uint32_t key_index);
extern int ambarella_otp_read_ecc_key(uint8_t *p, uint32_t length,
	uint32_t key_index);
extern int ambarella_otp_write_ecc_key(uint8_t *p, uint32_t length,
	uint32_t key_index);
extern int ambarella_otp_read_user_slot_g0(uint8_t *p, uint32_t length,
	uint32_t slot_index);
extern int ambarella_otp_write_user_slot_g0(uint8_t *p, uint32_t length,
	uint32_t slot_index);
extern int ambarella_otp_read_user_slot_g1(uint8_t *p, uint32_t length,
	uint32_t slot_index);
extern int ambarella_otp_write_user_slot_g1(uint8_t *p, uint32_t length,
	uint32_t slot_index);
extern int ambarella_otp_read_user_data_g0(uint8_t *p, uint32_t length,
	uint32_t data_index);
extern int ambarella_otp_write_user_data_g0(uint8_t *p, uint32_t length,
	uint32_t data_index);
extern int ambarella_otp_read_test_region(uint8_t *p, uint32_t length);
extern int ambarella_otp_write_test_region(uint8_t *p, uint32_t length);
extern int ambarella_otp_revoke_key(uint32_t index, uint32_t simulate);
//extern int ambarella_otp_get_chip_repair_info(uint32_t *p, uint32_t length);
extern int ambarella_otp_query_otp_setting(uint32_t *p, uint32_t length);
extern int ambarella_otp_set_jtag_efuse(uint32_t simulate);
extern int ambarella_otp_lock_zone_a(uint32_t simulate);
extern int ambarella_otp_read_sysconfig(uint8_t *p, uint32_t length);
extern int ambarella_otp_write_sysconfig(uint8_t *p, uint32_t length);
extern int ambarella_otp_read_cst_planted_seed(uint8_t *p, uint32_t length);
extern int ambarella_otp_read_cst_planted_cuk(uint8_t *p, uint32_t length);
extern int ambarella_otp_write_cst_planted_seed_and_cuk(uint8_t *p,
	uint32_t length);
extern int ambarella_otp_read_user_planted_cuk(uint8_t *p, uint32_t length);
extern int ambarella_otp_write_user_planted_cuk(uint8_t *p, uint32_t length);
extern int ambarella_otp_read_bst_ver(uint32_t *p, uint32_t length);
extern int ambarella_otp_increase_bst_ver(uint32_t simulate);
extern int ambarella_otp_set_bst_anti_rollback(uint32_t simulate);
extern int ambarella_otp_disable_secure_usb_boot(uint32_t simulate);
extern int ambarella_otp_read_misc_config(uint32_t *p);
extern int ambarella_otp_write_misc_config(uint32_t misc_config, uint32_t simulate);

extern int ambarella_otp_nw_query_otp_setting(
	unsigned long *sys_config, unsigned long *lock_bit_invalid_bit);

extern void ambarella_security_setup(void);
extern void ambarella_device_security_setup(uint32_t ctrl_bit);

extern void ambarella_el2_rsvd_mem(uint64_t *base, uint64_t *limit);
extern void ambarella_el2_pgtable_setup(void);
extern void ambarella_el2_runtime_setup(void);

extern uintptr_t ambarella_el2_fault_handler(uint32_t smc_fid,
		u_register_t x1, u_register_t x2, u_register_t x3,
		u_register_t x4, void *cookie, void *handle, u_register_t flags);

extern int32_t ambarella_el2_config_monitor_region(uint64_t start, uint32_t len,
		uint32_t access);
extern void ambarella_el2_enable_monitor_region(uint64_t start, uint32_t len,
		uint32_t access);
extern void ambarella_el2_disable_monitor_region(uint64_t start, uint32_t len,
		uint32_t access);

extern void rct_soft_reset_vp_cluster(void);
extern void ambarella_att_setup(void);

#ifdef AMBARELLA_ENABLE_MEM_MONITOR
static inline bool ambarella_is_secure_boot(void)
{
	return true;
}
#else
static inline bool ambarella_is_secure_boot(void)
{
#if defined(AMBARELLA_CV22) || defined(AMBARELLA_CV25) \
	|| defined(AMBARELLA_CV28) || defined(AMBARELLA_S6LM) || defined(AMBARELLA_CV5)
	return (mmio_read_32(SECSP_BOOT_STS_REG) & 1);
#else
	return !!(mmio_read_32(RCT_REG(SYS_CONFIG_OFFSET)) & SYS_CONFIG_SECURE_BOOT);
#endif
}
#endif

static inline bool ambarella_el2_is_used(void)
{
	/* EL2 is not used if no signed DTB.*/
	return boot_cookie_ptr()->dtb_ram_start != 0;
}

extern uint64_t dram_vpn_page_size(void);

extern uint64_t ambarella_node_addr_base(void *fdt, int32_t offset);

extern void ambarella_cpufreq_parse(void);

extern int ambarella_secure_cpufreq_update(unsigned long clk_idx,
		unsigned long rate, unsigned long parent_rate);

extern uint32_t ambarella_lp5_adjust_islp5(void);
extern void ambarella_lp5_adjust_init(void);
extern void ambarella_lp5_adjust_run(void);
extern void ambarella_lp5_adjust_set_pval(uint32_t pval);
extern uint32_t ambarella_lp5_adjust_get_pval(void);
extern void ambarella_lp5_adjust_set_nval(uint32_t nval);
extern uint32_t ambarella_lp5_adjust_get_nval(void);
extern void ambarella_lp5_adjust_show_switch(void);
#if defined(CONFIG_ATF_AMBALINK)
extern void ambarella_dramc_reset_statis_ctrl(void);
extern void ambarella_dramc_enable_statis_ctrl(void);
extern void ambarella_dramc_disable_statis_ctrl(void);
extern AMBA_DRAMC_STATIS_s* ambarella_dramc_get_statis_info(void);
extern void ambarella_dramc_write(uint32_t offset, uint32_t val);
extern uint32_t ambarella_dramc_read(uint32_t offset);
extern void ambarella_ddrc_write(uint32_t ddrc, uint32_t offset, uint32_t val);
extern uint32_t ambarella_ddrc_read(uint32_t ddrc, uint32_t offset);
#endif


#endif /* __PLAT_PRIVATE_H__ */
