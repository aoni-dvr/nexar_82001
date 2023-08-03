/*
 * Copyright (c) 2013-2019, ARM Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <assert.h>
#include <string.h>

#include <arch.h>
#include <arch_helpers.h>
#include <common/debug.h>
#include <lib/pmf/pmf.h>
#include <lib/runtime_instr.h>
#include <lib/smccc.h>
#include <plat/common/platform.h>
#include <services/arm_arch_svc.h>

#include "psci_private.h"

#if defined(CONFIG_ATF_SUSPEND_SRAM) && defined(AMBARELLA_CV5)
typedef void (*SpCode_f)(void);
typedef void (*SpWakeCode_f)(uint32_t);
#endif
/*******************************************************************************
 * PSCI frontend api for servicing SMCs. Described in the PSCI spec.
 ******************************************************************************/
int psci_cpu_on(u_register_t target_cpu,
		uintptr_t entrypoint,
		u_register_t context_id)

{
	int rc;
	entry_point_info_t ep;

	/* Determine if the cpu exists of not */
	rc = psci_validate_mpidr(target_cpu);
	if (rc != PSCI_E_SUCCESS)
		return PSCI_E_INVALID_PARAMS;

	/* Validate the entry point and get the entry_point_info */
	rc = psci_validate_entry_point(&ep, entrypoint, context_id);
	if (rc != PSCI_E_SUCCESS)
		return rc;

	/*
	 * To turn this cpu on, specify which power
	 * levels need to be turned on
	 */
	return psci_cpu_on_start(target_cpu, &ep);
}

unsigned int psci_version(void)
{
	return PSCI_MAJOR_VER | PSCI_MINOR_VER;
}

int psci_cpu_suspend(unsigned int power_state,
		     uintptr_t entrypoint,
		     u_register_t context_id)
{
	int rc;
	unsigned int target_pwrlvl, is_power_down_state;
	entry_point_info_t ep;
	psci_power_state_t state_info = { {PSCI_LOCAL_STATE_RUN} };
	plat_local_state_t cpu_pd_state;

	/* Validate the power_state parameter */
	rc = psci_validate_power_state(power_state, &state_info);
	if (rc != PSCI_E_SUCCESS) {
		assert(rc == PSCI_E_INVALID_PARAMS);
		return rc;
	}

	/*
	 * Get the value of the state type bit from the power state parameter.
	 */
	is_power_down_state = psci_get_pstate_type(power_state);

	/* Sanity check the requested suspend levels */
	assert(psci_validate_suspend_req(&state_info, is_power_down_state)
			== PSCI_E_SUCCESS);

	target_pwrlvl = psci_find_target_suspend_lvl(&state_info);
	if (target_pwrlvl == PSCI_INVALID_PWR_LVL) {
		ERROR("Invalid target power level for suspend operation\n");
		panic();
	}

	/* Fast path for CPU standby.*/
	if (is_cpu_standby_req(is_power_down_state, target_pwrlvl)) {
		if  (psci_plat_pm_ops->cpu_standby == NULL)
			return PSCI_E_INVALID_PARAMS;

		/*
		 * Set the state of the CPU power domain to the platform
		 * specific retention state and enter the standby state.
		 */
		cpu_pd_state = state_info.pwr_domain_state[PSCI_CPU_PWR_LVL];
		psci_set_cpu_local_state(cpu_pd_state);

#if ENABLE_PSCI_STAT
		plat_psci_stat_accounting_start(&state_info);
#endif

#if ENABLE_RUNTIME_INSTRUMENTATION
		PMF_CAPTURE_TIMESTAMP(rt_instr_svc,
		    RT_INSTR_ENTER_HW_LOW_PWR,
		    PMF_NO_CACHE_MAINT);
#endif

		psci_plat_pm_ops->cpu_standby(cpu_pd_state);

		/* Upon exit from standby, set the state back to RUN. */
		psci_set_cpu_local_state(PSCI_LOCAL_STATE_RUN);

#if ENABLE_RUNTIME_INSTRUMENTATION
		PMF_CAPTURE_TIMESTAMP(rt_instr_svc,
		    RT_INSTR_EXIT_HW_LOW_PWR,
		    PMF_NO_CACHE_MAINT);
#endif

#if ENABLE_PSCI_STAT
		plat_psci_stat_accounting_stop(&state_info);

		/* Update PSCI stats */
		psci_stats_update_pwr_up(PSCI_CPU_PWR_LVL, &state_info);
#endif

		return PSCI_E_SUCCESS;
	}

	/*
	 * If a power down state has been requested, we need to verify entry
	 * point and program entry information.
	 */
	if (is_power_down_state != 0U) {
		rc = psci_validate_entry_point(&ep, entrypoint, context_id);
		if (rc != PSCI_E_SUCCESS)
			return rc;
	}

	/*
	 * Do what is needed to enter the power down state. Upon success,
	 * enter the final wfi which will power down this CPU. This function
	 * might return if the power down was abandoned for any reason, e.g.
	 * arrival of an interrupt
	 */
	psci_cpu_suspend_start(&ep,
			    target_pwrlvl,
			    &state_info,
			    is_power_down_state);

	return PSCI_E_SUCCESS;
}


int psci_system_suspend(uintptr_t entrypoint, u_register_t context_id)
{
	int rc;
	psci_power_state_t state_info;
	entry_point_info_t ep;

	/* Check if the current CPU is the last ON CPU in the system */
	if (psci_is_last_on_cpu() == 0U)
		return PSCI_E_DENIED;

	/* Validate the entry point and get the entry_point_info */
	rc = psci_validate_entry_point(&ep, entrypoint, context_id);
	if (rc != PSCI_E_SUCCESS)
		return rc;

	/* Query the psci_power_state for system suspend */
	psci_query_sys_suspend_pwrstate(&state_info);

	/*
	 * Check if platform allows suspend to Highest power level
	 * (System level)
	 */
	if (psci_find_target_suspend_lvl(&state_info) < PLAT_MAX_PWR_LVL)
		return PSCI_E_DENIED;

	/* Ensure that the psci_power_state makes sense */
	assert(psci_validate_suspend_req(&state_info, PSTATE_TYPE_POWERDOWN)
						== PSCI_E_SUCCESS);
	assert(is_local_state_off(
			state_info.pwr_domain_state[PLAT_MAX_PWR_LVL]) != 0);

	/*
	 * Do what is needed to enter the system suspend state. This function
	 * might return if the power down was abandoned for any reason, e.g.
	 * arrival of an interrupt
	 */
	psci_cpu_suspend_start(&ep,
			    PLAT_MAX_PWR_LVL,
			    &state_info,
			    PSTATE_TYPE_POWERDOWN);

	return PSCI_E_SUCCESS;
}

#if defined(CONFIG_ATF_SUSPEND_SRAM) && defined(AMBARELLA_CV5)
#include <drivers/gpio.h>
#include <lib/mmio.h>
#if 0
#define GPIO_DATA_OFFSET        0x00
#define GPIO_DIR_OFFSET         0x04
#define GPIO_IS_OFFSET          0x08
#define GPIO_IBE_OFFSET         0x0c
#define GPIO_IEV_OFFSET         0x10
#define GPIO_IE_OFFSET          0x14
#define GPIO_AFSEL_OFFSET       0x18
#define GPIO_RIS_OFFSET         0x1c
#define GPIO_MIS_OFFSET         0x20
#define GPIO_IC_OFFSET          0x24
#define GPIO_MASK_OFFSET        0x28
#define GPIO_ENABLE_OFFSET      0x2c
uint32_t GpioReg[5][7] = {0};

void psci_dump_gpio(void)
{
    uintptr_t gpio_base[] = {
        GPIO0_BASE, GPIO1_BASE, GPIO2_BASE, GPIO3_BASE,
        GPIO4_BASE
    };
    uintptr_t iomux_base = IOMUX_BASE;
    uint32_t i;
//    uint32_t *pGpioData = (uint32_t *)(SCRATCHPAD_SRAM_BASE + sizeof(struct platform_fast_resume_code) + sizeof(uint64_t));

    for (i = 0; i < 5; i++) {
//        pGpioData[i] = mmio_read_32(gpio_base[i] + GPIO_DATA_OFFSET);
        GpioReg[i][0] = mmio_read_32(gpio_base[i] + GPIO_DATA_OFFSET);
        GpioReg[i][1] = mmio_read_32(gpio_base[i] + GPIO_DIR_OFFSET);
        GpioReg[i][2] = mmio_read_32(gpio_base[i] + GPIO_AFSEL_OFFSET);
        GpioReg[i][3] = mmio_read_32(gpio_base[i] + GPIO_MASK_OFFSET);

        GpioReg[i][4] = mmio_read_32(iomux_base + (i * 12));
        GpioReg[i][5] = mmio_read_32(iomux_base + (i * 12) + 4);
        GpioReg[i][6] = mmio_read_32(iomux_base + (i * 12) + 8);
    }
}
#endif

void psci_suspend_gpio(void)
{
    uint32_t gpio;

    // disable SD/SDIO0/SDIO1
    for (gpio = 79; gpio < 110; gpio++) {
//        gpio_set_direction(gpio, GPIO_DIR_OUT);
//        gpio_set_value(gpio, GPIO_LEVEL_LOW);
        gpio_set_direction(gpio, GPIO_DIR_IN);
        gpio_set_value(gpio, GPIO_LEVEL_LOW);
    }

    // reset ethernet phy
    gpio_set_direction(77, GPIO_DIR_OUT);
    gpio_set_value(77, GPIO_LEVEL_LOW);
    gpio_set_direction(78, GPIO_DIR_OUT);
    gpio_set_value(78, GPIO_LEVEL_LOW);

    // disable ENET port
    for (gpio = 19; gpio < 34; gpio++) {
        gpio_set_direction(gpio, GPIO_DIR_IN);
        gpio_set_value(gpio, GPIO_LEVEL_LOW);
    }

    // disable ext osc
    gpio_set_direction(34, GPIO_DIR_IN);
    gpio_set_value(34, GPIO_LEVEL_LOW);

    // disable 2nd ext ref clk
    gpio_set_direction(35, GPIO_DIR_IN);
    gpio_set_value(35, GPIO_LEVEL_LOW);

    // disable ENET1 port
    for (gpio = 36; gpio < 51; gpio++) {
        gpio_set_direction(gpio, GPIO_DIR_IN);
        gpio_set_value(gpio, GPIO_LEVEL_LOW);
    }

    //disable CAN
    for (gpio = 127; gpio < 131; gpio++) {
//        gpio_set_direction(gpio, GPIO_DIR_OUT);
//        gpio_set_value(gpio, GPIO_LEVEL_LOW);
        gpio_set_direction(gpio, GPIO_DIR_IN);
        gpio_set_value(gpio, GPIO_LEVEL_LOW);
    }
//====20220613
    // WDT_EXT_RST
    gpio_set_direction(0, GPIO_DIR_IN);
    gpio_set_value(0, GPIO_LEVEL_LOW);
    // disable IDC2
    gpio_set_direction(1, GPIO_DIR_IN);
    gpio_set_value(1, GPIO_LEVEL_LOW);
    gpio_set_direction(2, GPIO_DIR_IN);
    gpio_set_value(2, GPIO_LEVEL_LOW);
    // disable IDC1
    gpio_set_direction(3, GPIO_DIR_IN);
    gpio_set_value(3, GPIO_LEVEL_LOW);
    gpio_set_direction(4, GPIO_DIR_IN);
    gpio_set_value(4, GPIO_LEVEL_LOW);
    // disable IDC4
    gpio_set_direction(6, GPIO_DIR_IN);
    gpio_set_value(6, GPIO_LEVEL_LOW);
#if !defined(CONFIG_BSP_CV5DK_OPTION_A_V100)
    gpio_set_direction(7, GPIO_DIR_IN);
    gpio_set_value(7, GPIO_LEVEL_LOW);
#endif
    // disable IDC3
    gpio_set_direction(8, GPIO_DIR_IN);
    gpio_set_value(8, GPIO_LEVEL_LOW);
    gpio_set_direction(9, GPIO_DIR_IN);
    gpio_set_value(9, GPIO_LEVEL_LOW);
    // disable PWM
    gpio_set_direction(12, GPIO_DIR_IN);
    gpio_set_value(12, GPIO_LEVEL_LOW);
    gpio_set_direction(13, GPIO_DIR_IN);
    gpio_set_value(13, GPIO_LEVEL_LOW);
    // disable I2S
    for (gpio = 14; gpio < 19; gpio++) {
        gpio_set_direction(gpio, GPIO_DIR_IN);
        gpio_set_value(gpio, GPIO_LEVEL_LOW);
    }
    // disable IDC5
    gpio_set_direction(107, GPIO_DIR_IN);
    gpio_set_value(107, GPIO_LEVEL_LOW);
    gpio_set_direction(108, GPIO_DIR_IN);
    gpio_set_value(108, GPIO_LEVEL_LOW);

    // disable HDMI CEC/HPD
    gpio_set_direction(110, GPIO_DIR_IN);
    gpio_set_value(110, GPIO_LEVEL_LOW);
    gpio_set_direction(111, GPIO_DIR_IN);
    gpio_set_value(111, GPIO_LEVEL_LOW);

    // disable DDC
    gpio_set_direction(112, GPIO_DIR_IN);
    gpio_set_value(112, GPIO_LEVEL_LOW);
    gpio_set_direction(113, GPIO_DIR_IN);
    gpio_set_value(113, GPIO_LEVEL_LOW);

    // disable IDC
    gpio_set_direction(114, GPIO_DIR_IN);
    gpio_set_value(114, GPIO_LEVEL_LOW);
    gpio_set_direction(115, GPIO_DIR_IN);
    gpio_set_value(115, GPIO_LEVEL_LOW);

    // disable HSYNC/VSYNC
    gpio_set_direction(117, GPIO_DIR_IN);
    gpio_set_value(117, GPIO_LEVEL_LOW);
    gpio_set_direction(118, GPIO_DIR_IN);
    gpio_set_value(118, GPIO_LEVEL_LOW);

    // disable IDCS
    gpio_set_direction(137, GPIO_DIR_IN);
    gpio_set_value(137, GPIO_LEVEL_LOW);
    gpio_set_direction(138, GPIO_DIR_IN);
    gpio_set_value(138, GPIO_LEVEL_LOW);

}

static void idle2wake_gpio(uint32_t gpio)
{
    uint64_t GpioBaseReg = 0x20E4013000;
    volatile uint32_t *pRegAddr;
    struct platform_fast_resume_code *pScratchpad = (struct platform_fast_resume_code *) SCRATCHPAD_SRAM_BASE;
    SpCode_f EnterDdrSr = (SpCode_f) pScratchpad->ddr_sr_enter;
    SpCode_f ExitDdrSr = (SpCode_f) pScratchpad->ddr_sr_exit;
    u_register_t scr;
    uint32_t gpio_no = (gpio & 0x1F);

    pRegAddr = (uint32_t *)(GpioBaseReg + ((gpio >> 5) * 0x1000));

    EnterDdrSr();

    scr = read_scr_el3();
    /* Enable Physical IRQ and FIQ bit for NS world to wake the CPU */
    write_scr_el3(scr | SCR_IRQ_BIT | SCR_FIQ_BIT);
    isb();
    dsb();

    // Check gpio wakeup source
    while (1) {
        if ((*pRegAddr & (1 << gpio_no)) != 0) {
            break;
        }
        wfi();
    }
    write_scr_el3(scr);

    ExitDdrSr();
    __asm__ volatile("nop");
}

static void idle2wake(uint32_t gpio)
{
    struct platform_fast_resume_code *pScratchpad = (struct platform_fast_resume_code *) SCRATCHPAD_SRAM_BASE;
    SpCode_f EnterDdrSr = (SpCode_f) pScratchpad->ddr_sr_enter;
    SpCode_f ExitDdrSr = (SpCode_f) pScratchpad->ddr_sr_exit;
    u_register_t scr;

    EnterDdrSr();

    scr = read_scr_el3();
    /* Enable Physical IRQ and FIQ bit for NS world to wake the CPU */
    write_scr_el3(scr | SCR_IRQ_BIT | SCR_FIQ_BIT);
    isb();
    dsb();

    wfi();

    write_scr_el3(scr);

    ExitDdrSr();
    __asm__ volatile("nop");
}

int psci_system_wake(uintptr_t entrypoint, u_register_t context_id)
{
    struct platform_fast_resume_code *pScratchpad = (struct platform_fast_resume_code *)SCRATCHPAD_SRAM_BASE;
    SpWakeCode_f AliasRunSpCode = (SpWakeCode_f) pScratchpad->idle2wake_code;
    uint64_t StackAddr = (uint64_t) &pScratchpad->idle2wake_stack[STACK_SIZE];
    register uint64_t SP = 0;
    uint32_t WakeupGpio = pScratchpad->wakeup_gpio;
    uint32_t *pScratchpadCode = (uint32_t *) pScratchpad->idle2wake_code;
    uint32_t *pDramCode = (uint32_t *)idle2wake_gpio;
    uint32_t max_words = CODE_SIZE/sizeof(uint32_t), i;

    // If not wakeup from GPIO, use normal idle2wake code without checking gpio source.
    if (WakeupGpio == (~0)) {
        pDramCode = (uint32_t *)idle2wake;
    }
    for (i = 0; i < max_words; i++) {
        pScratchpadCode[i] = pDramCode[i];
        if (pDramCode[i] == 0xD65F03C0) { // RET code.
            break;
        }
    }

    (void) context_id;

//    psci_dump_gpio();

    psci_suspend_gpio();

	/* Flush all caches. */
	dcsw_op_all(DCCISW);

	/* disable mmu and icache */
	disable_mmu_icache_el3();

    /* save old stack */
    StackAddr = StackAddr - sizeof(uint64_t);

    __asm__ volatile (
        "mov %1, sp \n" : "=r"(SP) : "r"(SP)
    );
    ((uint64_t *)StackAddr)[0] = SP;

    /* initialize stack */
    StackAddr = StackAddr - sizeof(uint64_t);
    __asm__ volatile (
        "mov sp, %1 \n" : "=r" (StackAddr) : "r" (StackAddr)
    );
    __asm__ volatile (
        "STP %1, x30, [sp, #-16]! \n" : "=r"(SP) : "r"(SP)
    );

    AliasRunSpCode(WakeupGpio);

    /* restore stack and registers */
    __asm__ volatile (
        "LDP %0, x30, [sp], #16 \n" : "=r"(SP) : "r"(SP)
    );
    __asm__ volatile (
        "mov sp, %1 \n" : "=r"(SP) : "r"(SP)
    );

    enable_mmu_icache_el3();

	return PSCI_E_SUCCESS;
}

static void idlecore(void)
{
    volatile struct platform_fast_resume_code *pScratchpad = (struct platform_fast_resume_code *) SCRATCHPAD_SRAM_BASE;

    while (1) {
        // check "WaKe"
        if (pScratchpad->wakecore_status == 0x57614B65) {
            break;
        }
        wfi();
    }
}

int psci_cpu_off(void)
{
    struct platform_fast_resume_code *pScratchpad = (struct platform_fast_resume_code *)SCRATCHPAD_SRAM_BASE;
    SpCode_f CpuIdleCode = (SpCode_f) pScratchpad->idlecore_code;
    uint64_t StackAddr = (uint64_t) &pScratchpad->idlecore_stack[STACK_SIZE];
    register uint64_t SP = 0;
    SpCode_f pEntryPoint = NULL;
    u_register_t scr;
    uint32_t *pScratchpadCode = (uint32_t *) pScratchpad->idlecore_code;
    uint32_t *pDramCode = (uint32_t *)idlecore;
    uint32_t max_words = CODE_SIZE/sizeof(uint32_t), i;

    for (i = 0; i < max_words; i++) {
        pScratchpadCode[i] = pDramCode[i];
        if (pDramCode[i] == 0xD65F03C0) { // RET code.
            break;
        }
    }

    psci_flush_cpu_data(psci_svc_cpu_data.aff_info_state);
    psci_set_aff_info_state(AFF_STATE_OFF);
    psci_dsbish();
    psci_inv_cpu_data(psci_svc_cpu_data.aff_info_state);

	dcsw_op_all(DCCISW);
	disable_mmu_icache_el3();

    /* save old stack */
    StackAddr = StackAddr - sizeof(uint64_t);
    __asm__ volatile (
        "mov %1, sp \n" : "=r"(SP) : "r"(SP)
    );
    ((uint64_t *)StackAddr)[0] = SP;

    /* initialize stack */
    StackAddr = StackAddr - sizeof(uint64_t);
    __asm__ volatile (
        "mov sp, %1 \n" : "=r" (StackAddr) : "r" (StackAddr)
    );
    __asm__ volatile (
        "STP %1, x30, [sp, #-16]! \n" : "=r"(SP) : "r"(SP)
    );

    scr = read_scr_el3();

    /* Enable Physical IRQ and FIQ bit for NS world to wake the CPU */
    write_scr_el3(scr | SCR_IRQ_BIT | SCR_FIQ_BIT);
    isb();
    dsb();

    CpuIdleCode();
    {
        uint32_t interrupt;
        interrupt = plat_ic_acknowledge_interrupt();
        plat_ic_end_of_interrupt(interrupt);
    }
    write_scr_el3(scr);

    pEntryPoint = (SpCode_f) pScratchpad->wakecore_entry;
    pEntryPoint();

    // Should not be here
    /* restore stack and registers */
    __asm__ volatile (
        "LDP %0, x30, [sp], #16 \n" : "=r"(SP) : "r"(SP)
    );
    __asm__ volatile (
        "mov sp, %1 \n" : "=r"(SP) : "r"(SP)
    );

    enable_mmu_icache_el3();

	return PSCI_E_INVALID_PARAMS;
}
#else
int psci_cpu_off(void)
{
	int rc;
	unsigned int target_pwrlvl = PLAT_MAX_PWR_LVL;

	/*
	 * Do what is needed to power off this CPU and possible higher power
	 * levels if it able to do so. Upon success, enter the final wfi
	 * which will power down this CPU.
	 */
	rc = psci_do_cpu_off(target_pwrlvl);

	/*
	 * The only error cpu_off can return is E_DENIED. So check if that's
	 * indeed the case.
	 */
	assert(rc == PSCI_E_DENIED);

	return rc;
}
#endif

int psci_affinity_info(u_register_t target_affinity,
		       unsigned int lowest_affinity_level)
{
	int ret;
	unsigned int target_idx;

	/* We dont support level higher than PSCI_CPU_PWR_LVL */
	if (lowest_affinity_level > PSCI_CPU_PWR_LVL)
		return PSCI_E_INVALID_PARAMS;

	/* Calculate the cpu index of the target */
	ret = plat_core_pos_by_mpidr(target_affinity);
	if (ret == -1) {
		return PSCI_E_INVALID_PARAMS;
	}
	target_idx = (unsigned int)ret;

	/*
	 * Generic management:
	 * Perform cache maintanence ahead of reading the target CPU state to
	 * ensure that the data is not stale.
	 * There is a theoretical edge case where the cache may contain stale
	 * data for the target CPU data - this can occur under the following
	 * conditions:
	 * - the target CPU is in another cluster from the current
	 * - the target CPU was the last CPU to shutdown on its cluster
	 * - the cluster was removed from coherency as part of the CPU shutdown
	 *
	 * In this case the cache maintenace that was performed as part of the
	 * target CPUs shutdown was not seen by the current CPU's cluster. And
	 * so the cache may contain stale data for the target CPU.
	 */
	flush_cpu_data_by_index(target_idx,
				psci_svc_cpu_data.aff_info_state);

	return psci_get_aff_info_state_by_idx(target_idx);
}

int psci_migrate(u_register_t target_cpu)
{
	int rc;
	u_register_t resident_cpu_mpidr;

	rc = psci_spd_migrate_info(&resident_cpu_mpidr);
	if (rc != PSCI_TOS_UP_MIG_CAP)
		return (rc == PSCI_TOS_NOT_UP_MIG_CAP) ?
			  PSCI_E_DENIED : PSCI_E_NOT_SUPPORTED;

	/*
	 * Migrate should only be invoked on the CPU where
	 * the Secure OS is resident.
	 */
	if (resident_cpu_mpidr != read_mpidr_el1())
		return PSCI_E_NOT_PRESENT;

	/* Check the validity of the specified target cpu */
	rc = psci_validate_mpidr(target_cpu);
	if (rc != PSCI_E_SUCCESS)
		return PSCI_E_INVALID_PARAMS;

	assert((psci_spd_pm != NULL) && (psci_spd_pm->svc_migrate != NULL));

	rc = psci_spd_pm->svc_migrate(read_mpidr_el1(), target_cpu);
	assert((rc == PSCI_E_SUCCESS) || (rc == PSCI_E_INTERN_FAIL));

	return rc;
}

int psci_migrate_info_type(void)
{
	u_register_t resident_cpu_mpidr;

	return psci_spd_migrate_info(&resident_cpu_mpidr);
}

u_register_t psci_migrate_info_up_cpu(void)
{
	u_register_t resident_cpu_mpidr;
	int rc;

	/*
	 * Return value of this depends upon what
	 * psci_spd_migrate_info() returns.
	 */
	rc = psci_spd_migrate_info(&resident_cpu_mpidr);
	if ((rc != PSCI_TOS_NOT_UP_MIG_CAP) && (rc != PSCI_TOS_UP_MIG_CAP))
		return (u_register_t)(register_t) PSCI_E_INVALID_PARAMS;

	return resident_cpu_mpidr;
}

int psci_node_hw_state(u_register_t target_cpu,
		       unsigned int power_level)
{
	int rc;

	/* Validate target_cpu */
	rc = psci_validate_mpidr(target_cpu);
	if (rc != PSCI_E_SUCCESS)
		return PSCI_E_INVALID_PARAMS;

	/* Validate power_level against PLAT_MAX_PWR_LVL */
	if (power_level > PLAT_MAX_PWR_LVL)
		return PSCI_E_INVALID_PARAMS;

	/*
	 * Dispatch this call to platform to query power controller, and pass on
	 * to the caller what it returns
	 */
	assert(psci_plat_pm_ops->get_node_hw_state != NULL);
	rc = psci_plat_pm_ops->get_node_hw_state(target_cpu, power_level);
	assert(((rc >= HW_ON) && (rc <= HW_STANDBY))
		|| (rc == PSCI_E_NOT_SUPPORTED)
		|| (rc == PSCI_E_INVALID_PARAMS));
	return rc;
}

int psci_features(unsigned int psci_fid)
{
	unsigned int local_caps = psci_caps;

	if (psci_fid == SMCCC_VERSION)
		return PSCI_E_SUCCESS;

	/* Check if it is a 64 bit function */
	if (((psci_fid >> FUNCID_CC_SHIFT) & FUNCID_CC_MASK) == SMC_64)
		local_caps &= PSCI_CAP_64BIT_MASK;

	/* Check for invalid fid */
	if (!(is_std_svc_call(psci_fid) && is_valid_fast_smc(psci_fid)
			&& is_psci_fid(psci_fid)))
		return PSCI_E_NOT_SUPPORTED;


	/* Check if the psci fid is supported or not */
	if ((local_caps & define_psci_cap(psci_fid)) == 0U)
		return PSCI_E_NOT_SUPPORTED;

	/* Format the feature flags */
	if ((psci_fid == PSCI_CPU_SUSPEND_AARCH32) ||
	    (psci_fid == PSCI_CPU_SUSPEND_AARCH64)) {
		/*
		 * The trusted firmware does not support OS Initiated Mode.
		 */
		unsigned int ret = ((FF_PSTATE << FF_PSTATE_SHIFT) |
			(((FF_SUPPORTS_OS_INIT_MODE == 1U) ? 0U : 1U)
				<< FF_MODE_SUPPORT_SHIFT));
		return (int) ret;
	}

	/* Return 0 for all other fid's */
	return PSCI_E_SUCCESS;
}

/*******************************************************************************
 * PSCI top level handler for servicing SMCs.
 ******************************************************************************/
u_register_t psci_smc_handler(uint32_t smc_fid,
			  u_register_t x1,
			  u_register_t x2,
			  u_register_t x3,
			  u_register_t x4,
			  void *cookie,
			  void *handle,
			  u_register_t flags)
{
	u_register_t ret;

#if !defined(CONFIG_ATF_AMBALINK)  /* allow secure RTOS */
	if (is_caller_secure(flags))
		return (u_register_t)SMC_UNK;
#endif

	/* Check the fid against the capabilities */
	if ((psci_caps & define_psci_cap(smc_fid)) == 0U)
		return (u_register_t)SMC_UNK;

	if (((smc_fid >> FUNCID_CC_SHIFT) & FUNCID_CC_MASK) == SMC_32) {
		/* 32-bit PSCI function, clear top parameter bits */

		uint32_t r1 = (uint32_t)x1;
		uint32_t r2 = (uint32_t)x2;
		uint32_t r3 = (uint32_t)x3;

		switch (smc_fid) {
		case PSCI_VERSION:
			ret = (u_register_t)psci_version();
			break;

		case PSCI_CPU_OFF:
			ret = (u_register_t)psci_cpu_off();
			break;

		case PSCI_CPU_SUSPEND_AARCH32:
			ret = (u_register_t)psci_cpu_suspend(r1, r2, r3);
			break;

		case PSCI_CPU_ON_AARCH32:
			ret = (u_register_t)psci_cpu_on(r1, r2, r3);
			break;

		case PSCI_AFFINITY_INFO_AARCH32:
			ret = (u_register_t)psci_affinity_info(r1, r2);
			break;

		case PSCI_MIG_AARCH32:
			ret = (u_register_t)psci_migrate(r1);
			break;

		case PSCI_MIG_INFO_TYPE:
			ret = (u_register_t)psci_migrate_info_type();
			break;

		case PSCI_MIG_INFO_UP_CPU_AARCH32:
			ret = psci_migrate_info_up_cpu();
			break;

		case PSCI_NODE_HW_STATE_AARCH32:
			ret = (u_register_t)psci_node_hw_state(r1, r2);
			break;

		case PSCI_SYSTEM_SUSPEND_AARCH32:
			ret = (u_register_t)psci_system_suspend(r1, r2);
			break;

		case PSCI_SYSTEM_OFF:
			psci_system_off();
			/* We should never return from psci_system_off() */
			break;

		case PSCI_SYSTEM_RESET:
			psci_system_reset();
			/* We should never return from psci_system_reset() */
			break;

		case PSCI_FEATURES:
			ret = (u_register_t)psci_features(r1);
			break;

#if ENABLE_PSCI_STAT
		case PSCI_STAT_RESIDENCY_AARCH32:
			ret = psci_stat_residency(r1, r2);
			break;

		case PSCI_STAT_COUNT_AARCH32:
			ret = psci_stat_count(r1, r2);
			break;
#endif
		case PSCI_MEM_PROTECT:
			ret = psci_mem_protect(r1);
			break;

		case PSCI_MEM_CHK_RANGE_AARCH32:
			ret = psci_mem_chk_range(r1, r2);
			break;

		case PSCI_SYSTEM_RESET2_AARCH32:
			/* We should never return from psci_system_reset2() */
			ret = psci_system_reset2(r1, r2);
			break;

		default:
			WARN("Unimplemented PSCI Call: 0x%x\n", smc_fid);
			ret = (u_register_t)SMC_UNK;
			break;
		}
	} else {
		/* 64-bit PSCI function */

		switch (smc_fid) {
		case PSCI_CPU_SUSPEND_AARCH64:
			ret = (u_register_t)
				psci_cpu_suspend((unsigned int)x1, x2, x3);
			break;

		case PSCI_CPU_ON_AARCH64:
			ret = (u_register_t)psci_cpu_on(x1, x2, x3);
			break;

		case PSCI_AFFINITY_INFO_AARCH64:
			ret = (u_register_t)
				psci_affinity_info(x1, (unsigned int)x2);
			break;

		case PSCI_MIG_AARCH64:
			ret = (u_register_t)psci_migrate(x1);
			break;

		case PSCI_MIG_INFO_UP_CPU_AARCH64:
			ret = psci_migrate_info_up_cpu();
			break;

		case PSCI_NODE_HW_STATE_AARCH64:
			ret = (u_register_t)psci_node_hw_state(
					x1, (unsigned int) x2);
			break;

		case PSCI_SYSTEM_SUSPEND_AARCH64:
			ret = (u_register_t)psci_system_suspend(x1, x2);
			break;

#if defined(CONFIG_ATF_SUSPEND_SRAM) && defined(AMBARELLA_CV5)
        case PSCI_SYSTEM_WAKE_AARCH64:
            ret = (u_register_t)psci_system_wake(x1, x2);
            break;
#endif
#if ENABLE_PSCI_STAT
		case PSCI_STAT_RESIDENCY_AARCH64:
			ret = psci_stat_residency(x1, (unsigned int) x2);
			break;

		case PSCI_STAT_COUNT_AARCH64:
			ret = psci_stat_count(x1, (unsigned int) x2);
			break;
#endif

		case PSCI_MEM_CHK_RANGE_AARCH64:
			ret = psci_mem_chk_range(x1, x2);
			break;

		case PSCI_SYSTEM_RESET2_AARCH64:
			/* We should never return from psci_system_reset2() */
			ret = psci_system_reset2((uint32_t) x1, x2);
			break;

		default:
			WARN("Unimplemented PSCI Call: 0x%x\n", smc_fid);
			ret = (u_register_t)SMC_UNK;
			break;
		}
	}

	return ret;
}
