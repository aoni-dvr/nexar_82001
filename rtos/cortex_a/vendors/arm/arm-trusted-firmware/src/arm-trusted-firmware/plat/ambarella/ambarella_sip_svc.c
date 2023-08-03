/*
 * Copyright (c) 2013-2016, ARM Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#include <assert.h>
#include <common/runtime_svc.h>
#include <lib/pmf/pmf.h>
#include <lib/mmio.h>
#include <lib/xlat_tables/xlat_tables_compat.h>
#include <plat_private.h>
#include <ambarella_smc.h>
#if defined(CONFIG_ATF_AMBALINK)
#include <lib/el3_runtime/cpu_data.h>
#include <lib/el3_runtime/context_mgmt.h>
#if defined(CONFIG_ATF_FUSA)
#include <fusa_ca53.h>
#endif
#include "../driver/ambarella_dram.h"
#endif /* CONFIG_ATF_AMBALINK */

static uint64_t ambarella_freq_setup(uint32_t smc_fid,
		u_register_t x1, u_register_t x2, u_register_t x3, u_register_t x4,
		void *cookie, void *handle, u_register_t flags)
{
	uint32_t fn = FNID_OF_SMC(smc_fid);

	switch (fn) {
	case AMBA_SCM_CNTFRQ_SETUP_CMD:
		write_cntfrq_el0(plat_get_syscnt_freq2());
		SMC_RET1(handle, SMC_OK);

	default:
		SMC_RET1(handle, SMC_UNK);
	}
}

static uint64_t ambarella_switch_to_aarch32(uint32_t smc_fid,
		u_register_t x1, u_register_t x2, u_register_t x3, u_register_t x4,
		void *cookie, void *handle, u_register_t flags)
{
	uint32_t fn = FNID_OF_SMC(smc_fid);

	switch (fn) {
	case AMBA_SIP_AARCH32_KERNEL:
		if (psci_secondaries_brought_up())
			SMC_RET1(handle, -2);

		bl31_plat_prepare_kernel32_entry(x1, x2, x3, x4);
		SMC_RET0(handle);

	default:
		SMC_RET1(handle, SMC_UNK);
	}
}

static uint64_t ambarella_lp5_adjust(uint32_t smc_fid,
		u_register_t x1, u_register_t x2, u_register_t x3, u_register_t x4,
		void *cookie, void *handle, u_register_t flags)
{
	uint32_t fn = FNID_OF_SMC(smc_fid);
	uint32_t rval;

	switch (fn) {
	case AMBA_SIP_LP5_ADJUST_ISLP5:
		rval = ambarella_lp5_adjust_islp5();
		SMC_RET1(handle, rval);

	case AMBA_SIP_LP5_ADJUST_INIT:
		ambarella_lp5_adjust_init();
		SMC_RET1(handle, SMC_OK);

	case AMBA_SIP_LP5_ADJUST_RUN:
		ambarella_lp5_adjust_run();
		SMC_RET1(handle, SMC_OK);

	case AMBA_SIP_LP5_ADJUST_SET_PVAL:
		ambarella_lp5_adjust_set_pval(x1);
		SMC_RET1(handle, SMC_OK);

	case AMBA_SIP_LP5_ADJUST_GET_PVAL:
		rval = ambarella_lp5_adjust_get_pval();
		SMC_RET1(handle, rval);

	case AMBA_SIP_LP5_ADJUST_SET_NVAL:
		ambarella_lp5_adjust_set_nval(x1);
		SMC_RET1(handle, SMC_OK);

	case AMBA_SIP_LP5_ADJUST_GET_NVAL:
		rval = ambarella_lp5_adjust_get_nval();
		SMC_RET1(handle, rval);

	case AMBA_SIP_LP5_ADJUST_SHOW_SWITCH:
		ambarella_lp5_adjust_show_switch();
		SMC_RET1(handle, SMC_OK);

	default:
		SMC_RET1(handle, SMC_UNK);
	}
}

#if defined(CONFIG_ATF_AMBALINK)
static uint64_t ambarella_dramc_stats(uint32_t smc_fid,
		u_register_t x1, u_register_t x2, u_register_t x3, u_register_t x4,
		void *cookie, void *handle, u_register_t flags)
{
	uint32_t fn = FNID_OF_SMC(smc_fid);
    AMBA_DRAMC_STATIS_s* dram_stats = NULL;

	switch (fn) {
	case AMBA_SIP_DRAMC_STATS_ENABLE:
		ambarella_dramc_enable_statis_ctrl();
		SMC_RET1(handle, SMC_OK);

	case AMBA_SIP_DRAMC_STATS_DISABLE:
		ambarella_dramc_disable_statis_ctrl();
		SMC_RET1(handle, SMC_OK);

	case AMBA_SIP_DRAMC_STATS_GET_INFO:
		dram_stats = ambarella_dramc_get_statis_info();
		SMC_RET1(handle, dram_stats);

	default:
		SMC_RET1(handle, SMC_UNK);
	}
}

static uint64_t ambarella_dramc_ops(uint32_t smc_fid,
        u_register_t x1, u_register_t x2, u_register_t x3, u_register_t x4,
        void *cookie, void *handle, u_register_t flags)
{
    uint32_t fn = FNID_OF_SMC(smc_fid);
    uint32_t rval;

    switch (fn) {
    case AMBA_SIP_DRAMC_OPS_READ:
        rval = ambarella_dramc_read(x1);
        SMC_RET1(handle, rval);

    case AMBA_SIP_DRAMC_OPS_WRITE:
        ambarella_dramc_write(x1, x2);
        SMC_RET1(handle, SMC_OK);

    default:
        SMC_RET1(handle, SMC_UNK);
    }
}

static uint64_t ambarella_ddrc_ops(uint32_t smc_fid,
        u_register_t x1, u_register_t x2, u_register_t x3, u_register_t x4,
        void *cookie, void *handle, u_register_t flags)
{
    uint32_t fn = FNID_OF_SMC(smc_fid);
    uint32_t rval;

    switch (fn) {
    case AMBA_SIP_DDRC_OPS_READ:
        rval = ambarella_ddrc_read(x1, x2);
        SMC_RET1(handle, rval);

    case AMBA_SIP_DDRC_OPS_WRITE:
        ambarella_ddrc_write(x1, x2, x3);
        SMC_RET1(handle, SMC_OK);

    default:
        SMC_RET1(handle, SMC_UNK);
    }
}

#endif

#ifdef AMBARELLA_ENABLE_MEM_MONITOR
static uint64_t ambarella_memory_monitor(uint32_t smc_fid,
		u_register_t x1, u_register_t x2, u_register_t x3, u_register_t x4,
		void *cookie, void *handle, u_register_t flags)
{
	uint32_t fn = FNID_OF_SMC(smc_fid);
	uint64_t rval = SMC_OK;

	switch (fn) {
	case AMBA_SIP_MONITOR_CONFIG:
		if (ambarella_el2_config_monitor_region(x1, (uint32_t)x2, (uint32_t)x3))
			rval = SMC_UNK;
		break;
	case AMBA_SIP_MONITOR_ENABLE:
		ambarella_el2_enable_monitor_region(x1, (uint32_t)x2, (uint32_t)x3);
		break;
	case AMBA_SIP_MONITOR_DISABLE:
		ambarella_el2_disable_monitor_region(x1, (uint32_t)x2, (uint32_t)x3);
		break;
	default:
		rval = SMC_UNK;
	}

	SMC_RET1(handle, rval);
}
#endif

static uint64_t ambarella_vp_config(uint32_t smc_fid,
		u_register_t x1, u_register_t x2, u_register_t x3, u_register_t x4,
		void *cookie, void *handle, u_register_t flags)
{
	uint32_t fn = FNID_OF_SMC(smc_fid);
	uint64_t rval = SMC_UNK;

	switch (fn) {
	case AMBA_SIP_VP_CONFIG_RESET:
		rct_soft_reset_vp_cluster();
		rval = SMC_OK;
		break;
	default:
		break;
	}

	SMC_RET1(handle, rval);
}

static uint64_t ambarella_get_otp_huk(uint32_t smc_fid,
		u_register_t x1, u_register_t x2, u_register_t x3, u_register_t x4,
		void *cookie, void *handle, u_register_t flags)
{
	uint32_t fn = FNID_OF_SMC(smc_fid), base = 0, size = 0;
	int ret;

	if (x1 && x2) {
		base = round_down(x1, PAGE_SIZE);
		size = round_up(x2, PAGE_SIZE);

		ret = mmap_add_dynamic_region(base, base, size, MT_SECURE | MT_RW_DATA);
		if (ret) {
			ERROR("failed to map memory for UUID/CHIP_ID: %d\n", ret);
			SMC_RET1(handle, SMC_UNK);
		}

		inv_dcache_range(x1, x2);
	}

	switch (fn) {
	case AMBA_SIP_GET_AMBA_UNIQUE_ID:
		ret = ambarella_otp_read_amba_unique_id((uint8_t *) x1, (uint32_t) x2);
		break;

	case AMBA_SIP_GET_PUKEY:
		ret = ambarella_otp_read_rot_pubkey((uint8_t *) x1,
			(uint32_t) x2, (uint32_t) x3);
		break;

	case AMBA_SIP_SET_PUKEY:
		ret = ambarella_otp_write_rot_pubkey((uint8_t *) x1,
			(uint32_t) x2, (uint32_t) x3);
		break;

	case AMBA_SIP_LOCK_PUKEY:
		ret = ambarella_otp_lock_rot_pubkey((uint32_t) x1, (uint32_t) x2);
		break;

	case AMBA_SIP_GET_CUSTOMER_ID:
		ret = ambarella_otp_read_customer_id((uint8_t *) x1, x2);
		break;

	case AMBA_SIP_SET_CUSTOMER_ID:
		ret = ambarella_otp_write_customer_id((uint8_t *) x1, (uint32_t) x2);
		break;

	case AMBA_SIP_GET_COUNTER:
		ret = ambarella_otp_read_mono_counter((uint32_t *) x1,
			(uint32_t) x2, (uint32_t) x3);
		break;

	case AMBA_SIP_ADD_COUNTER:
		ret = ambarella_otp_increase_mono_counter((uint32_t) x1, (uint32_t) x2);
		break;

	case AMBA_SIP_PERMANENTLY_ENABLE_SECURE_BOOT:
		ret = ambarella_otp_permanently_enable_secure_boot((uint32_t) x1);
		break;

	case AMBA_SIP_GET_HUK:
		ret = ambarella_otp_read_huk((uint8_t *) x1, x2);
		break;

	case AMBA_SIP_SET_HUK_NONCE:
		ret = ambarella_otp_write_huk_nonce((uint8_t *) x1, x2);
		break;

	case AMBA_SIP_GET_AES_KEY:
		ret = ambarella_otp_read_aes_key((uint8_t *) x1, x2, x3);
		break;

	case AMBA_SIP_SET_AES_KEY:
		ret = ambarella_otp_write_aes_key((uint8_t *) x1, x2, x3);
		break;

	case AMBA_SIP_GET_ECC_KEY:
		ret = ambarella_otp_read_ecc_key((uint8_t *) x1, x2, x3);
		break;

	case AMBA_SIP_SET_ECC_KEY:
		ret = ambarella_otp_write_ecc_key((uint8_t *) x1, x2, x3);
		break;

	case AMBA_SIP_GET_USR_SLOT_G0:
		ret = ambarella_otp_read_user_slot_g0((uint8_t *) x1, x2, x3);
		break;

	case AMBA_SIP_SET_USR_SLOT_G0:
		ret = ambarella_otp_write_user_slot_g0((uint8_t *) x1, x2, x3);
		break;

	case AMBA_SIP_GET_USR_SLOT_G1:
		ret = ambarella_otp_read_user_slot_g1((uint8_t *) x1, x2, x3);
		break;

	case AMBA_SIP_SET_USR_SLOT_G1:
		ret = ambarella_otp_write_user_slot_g1((uint8_t *) x1, x2, x3);
		break;

	case AMBA_SIP_GET_USR_DATA_G0:
		ret = ambarella_otp_read_user_data_g0((uint8_t *) x1, x2, x3);
		break;

	case AMBA_SIP_SET_USR_DATA_G0:
		ret = ambarella_otp_write_user_data_g0((uint8_t *) x1, x2, x3);
		break;

	case AMBA_SIP_GET_TEST_REGION:
		ret = ambarella_otp_read_test_region((uint8_t *) x1, x2);
		break;

	case AMBA_SIP_SET_TEST_REGION:
		ret = ambarella_otp_write_test_region((uint8_t *) x1, x2);
		break;

	case AMBA_SIP_REVOKE_KEY:
		ret = ambarella_otp_revoke_key((uint32_t) x1, (uint32_t) x2);
		break;

	case AMBA_SIP_QUERY_OTP_SETTING:
		ret = ambarella_otp_query_otp_setting((uint32_t *) x1, x2);
		break;

	case AMBA_SIP_SET_JTAG_EFUSE:
		ret = ambarella_otp_set_jtag_efuse((uint32_t) x1);
		break;

	case AMBA_SIP_LOCK_ZONA_A:
		ret = ambarella_otp_lock_zone_a((uint32_t) x1);
		break;

	case AMBA_SIP_GET_SYSCONFIG:
		ret = ambarella_otp_read_sysconfig((uint8_t *) x1, x2);
		break;

	case AMBA_SIP_SET_SYSCONFIG:
		ret = ambarella_otp_write_sysconfig((uint8_t *) x1, x2);
		break;

	case AMBA_SIP_GET_CST_SEED:
		ret = ambarella_otp_read_cst_planted_seed((uint8_t *) x1, x2);
		break;

	case AMBA_SIP_GET_CST_CUK:
		ret = ambarella_otp_read_cst_planted_cuk((uint8_t *) x1, x2);
		break;

	case AMBA_SIP_SET_CST_SEED_CUK:
		ret = ambarella_otp_write_cst_planted_seed_and_cuk((uint8_t *) x1, x2);
		break;

	case AMBA_SIP_GET_USR_CUK:
		ret = ambarella_otp_read_user_planted_cuk((uint8_t *) x1, x2);
		break;

	case AMBA_SIP_SET_USR_CUK:
		ret = ambarella_otp_write_user_planted_cuk((uint8_t *) x1, x2);
		break;

	case AMBA_SIP_GET_BST_VER:
		ret = ambarella_otp_read_bst_ver((uint32_t *) x1, x2);
		break;

	case AMBA_SIP_INCREASE_BST_VER:
		ret = ambarella_otp_increase_bst_ver((uint32_t) x1);
		break;

	case AMBA_SIP_EN_ANTI_ROLLBACK:
		ret = ambarella_otp_set_bst_anti_rollback((uint32_t) x1);
		break;

	case AMBA_SIP_DIS_SECURE_USB_BOOT:
		ret = ambarella_otp_disable_secure_usb_boot((uint32_t) x1);
		break;

	case AMBA_SIP_GET_MISC_CONFIG:
		ret = ambarella_otp_read_misc_config((uint32_t *) x1);
		break;

	case AMBA_SIP_SET_MISC_CONFIG:
		ret = ambarella_otp_write_misc_config((uint32_t) x1, (uint32_t) x2);
		break;

	default:
		ERROR("not expected fn 0x%08x\n", fn);
		ret = -1;
		break;
	}

	if (x1 && x2) {
		flush_dcache_range(x1, x2);
		mmap_remove_dynamic_region(base, size);
	}

	if (ret < 0) {
		ERROR("fn (%d) failed, ret %d\n", fn, ret);
	}

	SMC_RET1(handle, (ret < 0) ? SMC_UNK : SMC_OK);

}

/*
 * This function handles ARM defined SiP Calls
 */
static uintptr_t ambarella_sip_handler(uint32_t smc_fid,
		u_register_t x1, u_register_t x2, u_register_t x3, u_register_t x4,
		void *cookie, void *handle, u_register_t flags)
{
	uint32_t svc = SVC_OF_SMC(smc_fid);
	uint32_t fn = FNID_OF_SMC(smc_fid);
	int ret = 0;

#if defined(CONFIG_ATF_FUSA)
	/* Handle FuSa SMC cases. These are valid services for both secure */
	/* and non-secure worlds */
	switch (smc_fid) {
		break;
	case FUSA_SIP_EX_SELF_DIAG:
		ex_self_diag();
		SMC_RET0(handle);
		break;
	case FUSA_SIP_ISA_SELF_DIAG:
		isa_self_diag();
		SMC_RET0(handle);
		break;
	case FUSA_SIP_CREG_SELF_DIAG:
		creg_self_diag();
		SMC_RET0(handle);
		break;
	case FUSA_SIP_BP_SELF_DIAG:
		bp_self_diag();
		SMC_RET0(handle);
		break;
	case FUSA_SIP_L2_SELF_DIAG:
		l2_self_diag();
		SMC_RET0(handle);
		break;
	case FUSA_SIP_ECCFI_SELF_DIAG:
		eccfi_self_diag();
		SMC_RET0(handle);
		break;
	case FUSA_SIP_SRLF_SELF_DIAG:
		srlf_self_diag((void *) x1, (size_t) x2, (UINT32) x3);
		SMC_RET0(handle);
		break;
	case FUSA_SIP_FUSA_FAULT_INJECT:
		libfusaca53_fault_inject((uint32_t) x1);
		SMC_RET0(handle);
		break;
	default:
		break;
	}
#endif
	/* Determine which security state this SMC originated from */
	if (is_caller_secure(flags)) {
		switch (svc) {
		case AMBA_SIP_ACCESS_OTP:
			return ambarella_get_otp_huk(smc_fid, x1, x2, x3, x4, cookie, handle, flags);

		default:
			SMC_RET1(handle, SMC_UNK);
		}
	} else {
		uint32_t uuid[4] = {0,};
		uint32_t customid[4] = {0,};
		u_register_t *p = NULL;

#if defined(CONFIG_ATF_AMBALINK) && !defined(CONFIG_ATF_SPD_OPTEE)
		/*allow REE access when no TEE*/
		if (svc == AMBA_SIP_ACCESS_OTP) {
			return ambarella_get_otp_huk(smc_fid, x1, x2, x3, x4, cookie, handle, flags);
		}
#endif
		if (svc == AMBA_SIP_ACCESS_OTP && fn == AMBA_SIP_GET_AMBA_UNIQUE_ID) {
			ret = ambarella_otp_read_amba_unique_id((uint8_t *)uuid, sizeof(uuid));
			if (ret < 0) {
				SMC_RET1(handle, SMC_UNK);
			}
			p = (u_register_t *)uuid;
			SMC_RET3(handle, 0, p[0], p[1]);
		}

		if (svc == AMBA_SIP_ACCESS_OTP && fn == AMBA_SIP_GET_CUSTOMER_ID) {
			ret = ambarella_otp_read_customer_id((uint8_t *)customid,
				sizeof(customid));
			if (ret < 0) {
				SMC_RET1(handle, SMC_UNK);
			}
			p = (u_register_t *)customid;
			SMC_RET3(handle, 0, p[0], p[1]);
		}

		if (svc == AMBA_SIP_ACCESS_OTP && fn == AMBA_SIP_NW_QUERY_OTP_SETTING) {
			unsigned long sys_config = 0;
			unsigned long lock_bit_invalid_bit = 0;
			ret = ambarella_otp_nw_query_otp_setting(
				&sys_config, &lock_bit_invalid_bit);
			if (ret < 0) {
				SMC_RET1(handle, SMC_UNK);
			}
			p = (u_register_t *)customid;
			SMC_RET3(handle, 0, sys_config, lock_bit_invalid_bit);
		}
	}
#if defined(CONFIG_ATF_AMBALINK)
#define AMBA_SIP_LINUX_SUSPEND_DONE         (0x8200FF02)
#define AMBA_SIP_BOOT_LINUX                 (0x8200FF03)
#define AMBA_SIP_BOOT_RTOS                  (0x8200FF04)

	switch (smc_fid) {
	case AMBA_SIP_BOOT_LINUX:
		{
			extern void bl31_plat_prepare_linux_entry(uint64_t x1, uint64_t x2, uint64_t x3, uint64_t x4);
			if (x3 > NS_SWITCH_AARCH64) {
				ERROR("%s: invalid parameters\n", __func__);
				SMC_RET1(handle, SMC_UNK);
			}

			bl31_plat_prepare_linux_entry(x1, x2, x3, x4);

			SMC_RET0(handle);
		}

	case AMBA_SIP_LINUX_SUSPEND_DONE:
		{
			plat_local_state_t local_state;

			local_state = get_cpu_data_by_index(x1, psci_svc_cpu_data.local_state);

			SMC_RET1(handle, is_local_state_off(local_state));
		}

#if defined(CONFIG_THREADX)
	case AMBA_SIP_BOOT_RTOS:
		{
			extern void bl31_plat_prepare_rtos_entry(uint64_t x1, uint64_t x2, uint64_t x3, uint64_t x4);
			if (!is_caller_secure(flags)) {
				bl31_plat_prepare_rtos_entry(x1, x2, x3, x4);
				SMC_RET0(handle);
			} else {
				SMC_RET1(handle, SMC_UNK);
			}
		}
#endif
	}
#endif /* CONFIG_ATF_AMBALINK */
	switch (svc) {
	case AMBA_SCM_SVC_QUERY:
		SMC_RET1(handle, SMC_64);

	case AMBA_SCM_SVC_FREQ:
		return ambarella_freq_setup(smc_fid, x1, x2, x3, x4, cookie, handle, flags);

	case AMBA_SIP_SWITCH_TO_AARCH32:
		return ambarella_switch_to_aarch32(smc_fid, x1, x2, x3, x4, cookie, handle, flags);

#if !defined(CONFIG_ATF_AMBALINK)
	case AMBA_SIP_SVC_EL2_FAULT:
		return ambarella_el2_fault_handler(smc_fid, x1, x2, x3, x4, cookie, handle, flags);

#endif
#ifdef AMBARELLA_ENABLE_MEM_MONITOR
	case AMBA_SIP_MEMORY_MONITOR:
		return ambarella_memory_monitor(smc_fid, x1, x2, x3, x4, cookie, handle, flags);
#endif

	case AMBA_SIP_VP_CONFIG:
		return ambarella_vp_config(smc_fid, x1, x2, x3, x4, cookie, handle, flags);

	case AMBA_SIP_SECURITY_CPUFREQ:
		ret = ambarella_secure_cpufreq_update(x1, x2, x3);
		if (ret < 0) {
			SMC_RET1(handle, SMC_UNK);
		} else {
			SMC_RET1(handle, SMC_OK);
		}

	case AMBA_SIP_LP5_ADJUST:
		return ambarella_lp5_adjust(smc_fid, x1, x2, x3, x4, cookie, handle, flags);

#if defined(CONFIG_ATF_AMBALINK)
	case AMBA_SIP_DRAMC_STATS:
		return ambarella_dramc_stats(smc_fid, x1, x2, x3, x4, cookie, handle, flags);

    case AMBA_SIP_DRAMC_OPS:
        return ambarella_dramc_ops(smc_fid, x1, x2, x3, x4, cookie, handle, flags);

    case AMBA_SIP_DDRC_OPS:
        return ambarella_ddrc_ops(smc_fid, x1, x2, x3, x4, cookie, handle, flags);
#endif

	default:
		NOTICE("Unimplemented ARM SiP Service Call: 0x%x \n", smc_fid);
		SMC_RET1(handle, SMC_UNK);
	}
}


/* Define a runtime service descriptor for fast SMC calls */
DECLARE_RT_SVC(
	ambarella_sip_svc,
	OEN_SIP_START,
	OEN_SIP_END,
	SMC_TYPE_FAST,
	NULL,
	ambarella_sip_handler
);

