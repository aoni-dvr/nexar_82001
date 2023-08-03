/*
 * Copyright (c) 2013-2016, ARM Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#include <assert.h>
#include <lib/utils_def.h>
#include <lib/mmio.h>
#include <lib/psci/psci.h>
#include <drivers/arm/gicv2.h>
#include <drivers/delay_timer.h>
#include <plat/common/platform.h>
#include <plat_private.h>

#define DEAD(...)	do {wfi(); ERROR(__VA_ARGS__); panic();} while(0)

#define CORE_PWR_STATE(state) \
		((state)->pwr_domain_state[MPIDR_AFFLVL0])
#define SYSTEM_PWR_STATE(state) \
		((state)->pwr_domain_state[PLAT_MAX_PWR_LVL])

extern uint64_t ambarella_sec_entrypoint;

static uint32_t key_saved_once = 0;

static void ambarella_cpu_standby(plat_local_state_t cpu_state)
{
	unsigned int scr = read_scr_el3();

	assert(cpu_state == PLAT_MAX_RET_STATE);

	/* Enable Physical IRQ and FIQ bit for NS world to wake the CPU */
	write_scr_el3(scr | SCR_IRQ_BIT | SCR_FIQ_BIT);
	isb();
	dsb();
	wfi();

	/*
	 * Restore SCR to the original value, synchronisation of scr_el3 is
	 * done by eret while el3_exit to save some execution cycles.
	 */
	write_scr_el3(scr);
}

static int ambarella_pwr_domain_on(u_register_t mpidr)
{
#if defined(CONFIG_ATF_AMBALINK) && (defined(AMBARELLA_CV2FS) || defined(AMBARELLA_CV22FS))
	unsigned long v = 0;
	unsigned int cpu = plat_core_pos_by_mpidr(mpidr);
	unsigned long ptn = 0xDEADBEE0 | (cpu & 0x0F);

	/*core0 write 0xdeadbeef*/
	mmio_write_32(SCRATCHPAD_BASE + AHBSP_DATA3_OFFSET, ptn);

	ptn = 0xCAFEFAC0 | (cpu & 0x0F);
	/*core3 write 0xcafeface*/
	do {
		__asm__ volatile ("wfe");
		v =  mmio_read_32(SCRATCHPAD_BASE + AHBSP_DATA3_OFFSET);
	} while (v != ptn);

	/*core0 write ambarella_sec_entrypoint, core3 jump to ambarella_sec_entrypoint*/
	mmio_write_32(SCRATCHPAD_BASE + AHBSP_DATA3_OFFSET, ambarella_sec_entrypoint);
#else
	unsigned int bit, cpu = plat_core_pos_by_mpidr(mpidr);
#if defined(CONFIG_ATF_SUSPEND_SRAM) && defined(AMBARELLA_CV5)
    struct platform_fast_resume_code *pScratchpad = (struct platform_fast_resume_code *)SCRATCHPAD_SRAM_BASE;
#endif

	if (cpu == -1)
		return PSCI_E_INTERN_FAIL;

	switch (cpu) {
	case 1:
		bit = CORTEX_CORE1_RESET_MASK;
		break;
	case 2:
		bit = CORTEX_CORE2_RESET_MASK;
		break;
	case 3:
		bit = CORTEX_CORE3_RESET_MASK;
		break;
	default:
		ERROR("%s: not allowed to power on cpu%d.\n", __func__, cpu);
		panic();
	}

#if defined(CONFIG_ATF_SUSPEND_SRAM) && defined(AMBARELLA_CV5)
    if (pScratchpad->wakecore_status == 0xcafecafe) {
        pScratchpad->wakecore_entry = ambarella_sec_entrypoint;
        pScratchpad->wakecore_status = 0x57614B65; // set to "WaKe"
        gicv2_raise_sgi(ARM_IRQ_SEC_SGI_0, 1);
        return PSCI_E_SUCCESS;
    }
#endif
	/* put the secondary cpu into reset state */
	mmio_setbits_32(AXI_BASE + CORTEX_RESET_OFFSET, bit);

	mmio_write_32(AXI_BASE + CORTEX_RVBARADDR0_OFFSET + cpu * 4, CORTEX_RVBAR_ADDR(BL31_BASE));
	isb();
	dsb();
	mmio_clrbits_32(AXI_BASE + CORTEX_RESET_OFFSET, bit);
#endif

	return PSCI_E_SUCCESS;
}

static void ambarella_pwr_domain_off(const psci_power_state_t *target_state)
{
	/* Prevent interrupts from spuriously waking up this cpu */
	gicv2_cpuif_disable();
}

static void ambarella_pwr_domain_suspend(const psci_power_state_t *target_state)
{
	if (CORE_PWR_STATE(target_state) != PLAT_MAX_OFF_STATE)
		return;

	/* Prevent interrupts from spuriously waking up this cpu */
	gicv2_cpuif_disable();

	if (SYSTEM_PWR_STATE(target_state) == PLAT_MAX_OFF_STATE) {
		/* SYSTEM_SUSPEND only on CPU0 */
		assert(plat_my_core_pos() == 0);

#if 0
		/* Turn off all of the secondary cpus */
		mmio_setbits_32(AXI_BASE + CORTEX_RESET_OFFSET,
					CORTEX_CORE1_RESET_MASK |
					CORTEX_CORE2_RESET_MASK |
					CORTEX_CORE3_RESET_MASK);
#endif
	}
}

static void ambarella_pwr_domain_on_finish(const psci_power_state_t *target_state)
{
	/* Initialize the GIC per-cpu and distributor interfaces */
	ambarella_gic_pcpu_init();
}

static void ambarella_pwr_domain_suspend_finish(const psci_power_state_t *target_state)
{
	if (SYSTEM_PWR_STATE(target_state) != PLAT_MAX_OFF_STATE)
		goto finish;

	ambarella_security_setup();
	ambarella_soc_fixup();

	/* Initialize the GIC distributor interfaces */
	ambarella_gic_distif_init();
finish:
	/* Initialize the GIC per-cpu and distributor interfaces */
	ambarella_gic_pcpu_init();
}

static void __dead2 ambarella_system_off(void)
{
	ambarella_gpio_notify_mcu(NOTIFY_MCU_POWEROFF);

	DEAD("Ambarella System Off: operation not handled.\n");
}

static void __dead2 ambarella_system_reset(void)
{
	mmio_clrbits_32(RCT_BASE + SOFT_OR_DLL_RESET_OFFSET, 0x1);
	mmio_setbits_32(RCT_BASE + SOFT_OR_DLL_RESET_OFFSET, 0x1);
	isb();
	dsb();

	DEAD("Ambarella System Reset: operation not handled.\n");
}

static int ambarella_validate_power_state(unsigned int power_state,
				psci_power_state_t *req_state)
{
	int pstate = psci_get_pstate_type(power_state);

	assert(req_state);

	/* Sanity check the requested state */
	if (pstate == PSTATE_TYPE_STANDBY)
		req_state->pwr_domain_state[MPIDR_AFFLVL0] = PLAT_MAX_RET_STATE;
	else
		req_state->pwr_domain_state[MPIDR_AFFLVL0] = PLAT_MAX_OFF_STATE;

	/* We expect the 'state id' to be zero */
	if (psci_get_pstate_id(power_state))
		return PSCI_E_INVALID_PARAMS;

	return PSCI_E_SUCCESS;
}

static int ambarella_validate_ns_entrypoint(unsigned long ns_entrypoint)
{
	return PSCI_E_SUCCESS;
}

static void ambarella_get_sys_suspend_power_state(psci_power_state_t *req_state)
{
	for (int i = MPIDR_AFFLVL0; i <= PLAT_MAX_PWR_LVL; i++)
		req_state->pwr_domain_state[i] = PLAT_MAX_OFF_STATE;
}

static void __dead2 ambarella_pwr_domain_pwr_down_wfi(const psci_power_state_t *target_state)
{
	uint32_t key_in, key_out;

	if (SYSTEM_PWR_STATE(target_state) != PLAT_MAX_OFF_STATE) {
		/* Flush all caches. */
		dcsw_op_all(DCCISW);
		psci_power_down_wfi();
		DEAD("CPU%d is not power down\n", plat_my_core_pos());
	}

	/* always set bit2 of pwc_status when suspend */
	mmio_setbits_32(PWC_REG(PWC_SET_STA_OFFSET), 0x1 << 2);
	mmio_write_32(PWC_REG(PWC_SET_RTC_OFFSET), mmio_read_32(PWC_REG(PWC_CUR_RTC_OFFSET)));
	mmio_write_32(PWC_REG(PWC_RESET_OFFSET), 0x1);
	mdelay(10);
	mmio_write_32(PWC_REG(PWC_RESET_OFFSET), 0x0);

	/* save key for dram scrambling if secure boot is enabled */
	if (ambarella_is_secure_boot() && (!key_saved_once)) {
		mmio_write_32(SECURE_SCRATCHPAD_REG(AHBSP_PWC_STROBE_OFFSET), 1);
		mdelay(10); /* at least 3ms, but we use 10ms for ensurance */
		mmio_write_32(SECURE_SCRATCHPAD_REG(AHBSP_PWC_STROBE_OFFSET), 0);

		key_in = mmio_read_32(PWC_DBG_REG(PWC_KEY_IN0_REG_OFFSET));
		key_out = mmio_read_32(PWC_DBG_REG(PWC_KEY_OUT0_REG_OFFSET));
		assert(key_in == key_out);
		key_saved_once = 1;
	}

	ambarella_gpio_notify_mcu(NOTIFY_MCU_SUSPEND);

	/* Flush all caches. */
	dcsw_op_all(DCCISW);
	/* disable mmu and icache */
	disable_mmu_icache_el3();
	tlbialle3();
	dsb();
	isb();

	ambarella_relocate_suspend();

	/* Should never reach here */
	DEAD("Ambarella System Power Down: operation not handled.\n");
}

/*******************************************************************************
 * Export the platform handlers to enable psci to invoke them
 ******************************************************************************/
static const struct plat_psci_ops ambarella_psci_ops = {
	.cpu_standby			= ambarella_cpu_standby,
	.pwr_domain_on			= ambarella_pwr_domain_on,
	.pwr_domain_on_finish		= ambarella_pwr_domain_on_finish,
	.pwr_domain_off			= ambarella_pwr_domain_off,
	.pwr_domain_suspend		= ambarella_pwr_domain_suspend,
	.pwr_domain_suspend_finish	= ambarella_pwr_domain_suspend_finish,
	.pwr_domain_pwr_down_wfi	= ambarella_pwr_domain_pwr_down_wfi,
	.system_off			= ambarella_system_off,
	.system_reset			= ambarella_system_reset,
	.validate_power_state		= ambarella_validate_power_state,
	.validate_ns_entrypoint		= ambarella_validate_ns_entrypoint,
	.get_sys_suspend_power_state	= ambarella_get_sys_suspend_power_state,
};


/*******************************************************************************
 * Export the platform specific power ops.
 ******************************************************************************/
int plat_setup_psci_ops(uintptr_t sec_entrypoint,
			const struct plat_psci_ops **psci_ops)
{
	ambarella_sec_entrypoint = sec_entrypoint; /* i.e., bl31_warm_entrypoint */
	*psci_ops = &ambarella_psci_ops;

	return 0;
}
