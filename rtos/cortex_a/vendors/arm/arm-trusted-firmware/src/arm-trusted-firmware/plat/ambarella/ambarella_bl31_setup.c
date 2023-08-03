/*
 * Copyright (c) 2013-2016, ARM Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <assert.h>
#include <lib/mmio.h>
#include <lib/el3_runtime/context_mgmt.h>
#include <bl31/bl31.h>
#include <lib/xlat_tables/xlat_tables_v2.h>
#include <drivers/generic_delay_timer.h>
#include <plat/common/platform.h>
#include <uart_ambarella.h>
#include <plat_private.h>
#include <boot_cookie.h>
#if defined(CONFIG_ATF_FUSA)
#include <fusa_ca53.h>
#endif

static console_t bl31_console;

static entry_point_info_t bl33_image_ep_info;
static entry_point_info_t bl32_image_ep_info;

#ifdef CONFIG_ATF_AMBALINK
/* rtos secure context for:
 *   primary core when optee disabled
 *   all secondary cores */
static cpu_context_t amba_context[4];
#endif
/*
 * Return a pointer to the 'entry_point_info' structure of the next image for
 * the security state specified. BL33 corresponds to the non-secure image type
 * while BL32 corresponds to the secure image type. A NULL pointer is returned
 * if the image does not exist.
 */
entry_point_info_t *bl31_plat_get_next_image_ep_info(uint32_t type)
{
	entry_point_info_t *next_image_info;

	next_image_info = (type == NON_SECURE) ? &bl33_image_ep_info : &bl32_image_ep_info;

	/* None of the images on this platform can have 0x0 as the entrypoint */
	if (next_image_info->pc)
		return next_image_info;
	else
		return NULL;
}

/*******************************************************************************
 * Perform any BL3-1 early platform setup, such as console init and deciding on
 * memory layout.
 ******************************************************************************/
void bl31_early_platform_setup2(u_register_t arg0, u_register_t arg1,
		u_register_t arg2, u_register_t arg3)
{
	boot_cookie_t *cookie;
	console_ambarella_register(UART0_BASE, UART_CLOCK, UART_BAUDRATE, &bl31_console);
	printf("\x1b[4l\r\n");	/* Set terminal to replacement mode */

	cookie = boot_cookie_init();
	assert(cookie);

#ifndef SPD_none
	SET_PARAM_HEAD(&bl32_image_ep_info, PARAM_EP, VERSION_1, 0);
	SET_SECURITY_STATE(bl32_image_ep_info.h.attr, SECURE);
	bl32_image_ep_info.pc = BL32_BASE;
	bl32_image_ep_info.spsr = SPSR_64(MODE_EL1, MODE_SP_ELX, DISABLE_ALL_EXCEPTIONS);
	NOTICE("BL31: Secure code at 0x%08lx \n", bl32_image_ep_info.pc);
#endif

	SET_PARAM_HEAD(&bl33_image_ep_info, PARAM_EP, VERSION_1, 0);
	SET_SECURITY_STATE(bl33_image_ep_info.h.attr, NON_SECURE);
	bl33_image_ep_info.spsr = SPSR_64(MODE_EL1, MODE_SP_ELX, DISABLE_ALL_EXCEPTIONS);
	bl33_image_ep_info.pc = cookie->bld_ram_start;
#if !defined(CONFIG_ATF_AMBALINK)
	NOTICE("BL31: Non Secure code at 0x%08lx \n", bl33_image_ep_info.pc);
#else
	/*
	 * Do not format
	 */
	(void)amba_context;
	#if defined(CONFIG_ATF_BL33_BLD_BASE) // Case: have BL2 and FIP
		/* BL1 BST -> BL2 -> BL3 FIP */
		#if !defined(CONFIG_ATF_SPD_OPTEE)
			#if defined(CONFIG_ATF_FIP_RTOS) || (defined(CONFIG_BUILD_QNX_IPL) && defined(CONFIG_ATF_HAVE_BL2))
				/* Go to S-RTOS or S-BLD */
				/* optee is disabled, so we need to set secure rtos context here */
				cm_set_context_by_index(CONFIG_BOOT_CORE_SRTOS, &amba_context[CONFIG_BOOT_CORE_SRTOS], SECURE);
				#if (defined(CONFIG_BUILD_QNX_IPL) && defined(CONFIG_ATF_HAVE_BL2))
					/* TODO: if have OP-TEE.. */
					SET_PARAM_HEAD(&bl32_image_ep_info, PARAM_EP, VERSION_1, 0);
					SET_SECURITY_STATE(bl32_image_ep_info.h.attr, SECURE);
					bl32_image_ep_info.pc = CONFIG_ATF_BL33_BLD_BASE;
					bl32_image_ep_info.spsr = SPSR_64(MODE_EL1, MODE_SP_ELX, DISABLE_ALL_EXCEPTIONS);
					bl32_image_ep_info.args.arg0 = 0;
					bl32_image_ep_info.args.arg1 = 0;
					bl32_image_ep_info.args.arg2 = 0;

					bl31_set_next_image_type(SECURE);
				#endif // (defined(CONFIG_BUILD_QNX_IPL) && defined(CONFIG_ATF_HAVE_BL2))
			#else // Go to NS boot-loader, SMC call for S-RTOS
				bl33_image_ep_info.args.arg0 = 0;
				bl33_image_ep_info.args.arg1 = 0;
				bl33_image_ep_info.args.arg2 = 0;
				bl33_image_ep_info.args.arg3 = 0;
			#endif // CONFIG_ATF_FIP_RTOS
		#endif  //#if !defined(CONFIG_ATF_SPD_OPTEE)
	#elif defined(CONFIG_ATF_BL33_BASE) // Case: ATF -> BL33
		/* BST -> amboot -> ATF -> BL33 */
		#if defined(CONFIG_BLD_SEQ_ATF_LINUX)
			/* Legacy mode: BST -> amboot -> ATF -> NS LNX */
#if defined(CONFIG_FWPROG_XTB_LOADADDR) && (CONFIG_FWPROG_XTB_LOADADDR > 0)
			bl33_image_ep_info.args.arg0 = CONFIG_FWPROG_XTB_LOADADDR & ~0xFFFFF; /* DTB must be MB aligned */
#else
			bl33_image_ep_info.args.arg0 = CONFIG_ATF_BL33_BASE & ~0xFFFFF; /* DTB must be MB aligned */
#endif // DTB address
			bl33_image_ep_info.args.arg1 = 0;
			bl33_image_ep_info.args.arg2 = 0;
			bl33_image_ep_info.args.arg3 = 0;
			#if defined(CONFIG_BOOT_CORE_SRTOS) // prepare context to run SRTOS from Linux,
				cm_set_context_by_index(CONFIG_BOOT_CORE_SRTOS, &amba_context[CONFIG_BOOT_CORE_SRTOS], SECURE);
			#endif // defined(CONFIG_BOOT_CORE_SRTOS)
		#elif defined(CONFIG_BLD_SEQ_ATF_SRTOS)
			/* Legacy mode: BST -> amboot -> ATF -> S RTOS */
			SET_PARAM_HEAD(&bl32_image_ep_info, PARAM_EP, VERSION_1, 0);
			SET_SECURITY_STATE(bl32_image_ep_info.h.attr, SECURE);
			bl32_image_ep_info.pc = CONFIG_ATF_BL33_BASE;
			#if defined(CONFIG_BUILD_QNX_SYS_IMAGE) || defined(CONFIG_THREADX64)
				bl32_image_ep_info.spsr = SPSR_64(MODE_EL1, MODE_SP_ELX, DISABLE_ALL_EXCEPTIONS);
			#else
				bl32_image_ep_info.spsr = SPSR_MODE32(MODE32_svc, SPSR_T_ARM, SPSR_E_LITTLE, DISABLE_ALL_EXCEPTIONS);
			#endif //#if defined(CONFIG_BUILD_QNX_SYS_IMAGE)
			bl32_image_ep_info.args.arg0 = 0;
			bl32_image_ep_info.args.arg1 = 0;
			bl32_image_ep_info.args.arg2 = 0;

			/* PSCI only set NS context, so we need to set secure rtos context here */
			cm_set_context(&amba_context[CONFIG_BOOT_CORE_SRTOS], SECURE);

			bl31_set_next_image_type(SECURE);
		#elif defined(CONFIG_BLD_SEQ_ATF_XEN)
			/* BST -> amboot -> ATF -> NS Xen */
			bl33_image_ep_info.pc = CONFIG_FWPROG_XEN_LOADADDR;
			bl33_image_ep_info.spsr = SPSR_64(MODE_EL2, MODE_SP_ELX,
					DISABLE_ALL_EXCEPTIONS);
			bl33_image_ep_info.args.arg0 = CONFIG_FWPROG_XTB_LOADADDR;
			bl33_image_ep_info.args.arg1 = 0;
			bl33_image_ep_info.args.arg2 = 0;
			bl33_image_ep_info.args.arg3 = 0;
			bl31_set_next_image_type(NON_SECURE);
		#else
		#error Must set CONFIG_BLD_SEQ_ATF_LINUX or CONFIG_BLD_SEQ_ATF_SRTOS
		#endif  //#if defined(CONFIG_BLD_SEQ_ATF_LINUX)

	#else
	#error Must set CONFIG_ATF_BL33_BLD_BASE or CONFIG_ATF_BL33_BASE
	#endif  //#if defined(CONFIG_ATF_BL33_BLD_BASE)
#endif  //#if defined(CONFIG_ATF_AMBALINK)
}

#if defined(CONFIG_ATF_AMBALINK)
/* x1: kernel entry, x2: DTB address, x3: xS_SWITCH_xxx, x4: reserved. */
void bl31_plat_prepare_linux_entry(uint64_t x1, uint64_t x2,
		uint64_t x3, uint64_t x4)
{
	bl33_image_ep_info.pc = x1;
	if (x3 != NS_SWITCH_AARCH64) {
		/* Aarch32 kernel */
		bl33_image_ep_info.spsr = SPSR_MODE32(
				MODE32_svc,
				SPSR_T_ARM,
				SPSR_E_LITTLE,
				DISABLE_ALL_EXCEPTIONS);
		/* Documentation/arm/Booting */
		bl33_image_ep_info.args.arg0 = 0; /* r0: 0 */
		bl33_image_ep_info.args.arg1 = 0; /* r1: machine type number discovered */
		bl33_image_ep_info.args.arg2 = 0; /* r2: physical address of tagged list in system RAM, or
						         physical address of device tree block (dtb) in system RAM */
		bl33_image_ep_info.args.arg3 = 0;
	} else {
		/* Aaarch64 kernel */
		bl33_image_ep_info.spsr = SPSR_64(
				MODE_EL1,
				MODE_SP_ELX,
				DISABLE_ALL_EXCEPTIONS);
		/* Documentation/arm64/booting.txt */
		bl33_image_ep_info.args.arg0 = x2; /* x0 = physical address of device tree blob (dtb) in system RAM. */
		bl33_image_ep_info.args.arg1 = 0;  /* 0 (reserved for future use) */
		bl33_image_ep_info.args.arg2 = 0;  /* 0 (reserved for future use) */
		bl33_image_ep_info.args.arg3 = 0;  /* 0 (reserved for future use) */
	}
	bl33_image_ep_info.args.arg4 = 0;

	/* None of the images on this platform can have 0x0 as the entrypoint */
	assert(bl33_image_ep_info.pc || bl33_image_ep_info.pc & 0x3);

	cm_init_my_context(&bl33_image_ep_info);
	cm_prepare_el3_exit(NON_SECURE);
}

#if defined(CONFIG_THREADX)
/* x1: kernel entry, x2: arg, x3: S_SWITCH_AARCH32, x4: reserved. */
void bl31_plat_prepare_rtos_entry(uint64_t x1, uint64_t x2, uint64_t x3, uint64_t x4)
{
	int cpu_idx = (int) plat_my_core_pos();

#if (CONFIG_BOOT_CORE_SRTOS == 0U)
	/* threadx at core0,1,2,3 */
	if (cpu_idx >= CONFIG_KAL_THREADX_SMP_NUM_CORES) {
		return;
	}
#elif (CONFIG_BOOT_CORE_SRTOS == 1U)
	/* op-tee at core0, threadx at core1,2,3 */
	if (cpu_idx > CONFIG_KAL_THREADX_SMP_NUM_CORES) {
		return;
	}
#else
#error CONFIG_BOOT_CORE_SRTOS
#endif
	if ((x3 == S_SWITCH_AARCH32) || (x3 == S_SWITCH_AARCH64)) {
		if (cm_get_context_by_index(cpu_idx, SECURE) == NULL) {
			cm_set_context_by_index(cpu_idx, &amba_context[cpu_idx], SECURE);
		}

		bl32_image_ep_info.pc = x1;


		if (x3 == S_SWITCH_AARCH64) {
		    bl32_image_ep_info.spsr = SPSR_64(MODE_EL1, MODE_SP_ELX, DISABLE_ALL_EXCEPTIONS);
		} else {
		    bl32_image_ep_info.spsr = SPSR_MODE32(MODE32_svc, SPSR_T_ARM, SPSR_E_LITTLE, DISABLE_ALL_EXCEPTIONS);
		}

		bl32_image_ep_info.args.arg0 = x2;
		bl32_image_ep_info.args.arg1 = 0;
		bl32_image_ep_info.args.arg2 = 0;
		bl32_image_ep_info.args.arg3 = 0;
		bl32_image_ep_info.args.arg4 = 0;
		/* None of the images on this platform can have 0x0 as the entrypoint */
		assert(bl32_image_ep_info.pc || bl32_image_ep_info.pc & 0x3);

		cm_init_my_context(&bl32_image_ep_info);
		cm_prepare_el3_exit(SECURE);

		/*
		 * Because RTOS do not set code-segment as secure.
		 * Need to disable SCR_EL3.SIF, Secure instruction fetch
		 */
		{
			cpu_context_t *ctx = cm_get_context(SECURE);
			uint32_t scr_el3;
			el3_state_t *state;

			state = get_el3state_ctx(ctx);
			scr_el3 = read_ctx_reg(state, CTX_SCR_EL3);
			scr_el3 &= ~SCR_SIF_BIT;
			write_ctx_reg(state, CTX_SCR_EL3, scr_el3);
		}
	}
}
#endif
#endif /* CONFIG_ATF_AMBALINK */
/*******************************************************************************
 * Perform the very early platform specific architectural setup here. At the
 * moment this is only intializes the mmu in a quick and dirty way.
 ******************************************************************************/
void bl31_plat_arch_setup(void)
{
#if defined(CONFIG_ATF_AMBALINK) && (defined(AMBARELLA_CV2FS) || defined(AMBARELLA_CV22FS))
	/* NIC400 setting for Secure/Non-Secure access */
	mmio_write_32(0xf000000c, NON_SECURE); /* Slave NIC400 */
#endif
	ambarella_security_setup();
	ambarella_soc_fixup();

	ambarella_mmap_setup(NULL);
}

/*******************************************************************************
 * Perform any BL3-1 platform setup code
 ******************************************************************************/
void bl31_platform_setup(void)
{
#if !defined(CONFIG_ATF_AMBALINK)
	ambarella_fdt_init();
#endif

	generic_delay_timer_init();

	/* Initialize the GIC driver, cpu and distributor interfaces */
	ambarella_gic_driver_init();
	ambarella_gic_distif_init();
	ambarella_gic_pcpu_init();

	ambarella_gpio_init();
}

unsigned int plat_get_syscnt_freq2(void)
{

	unsigned int syscnt, divisor;

	syscnt = get_sys_timer_parent_freq_hz() / AXI_SYS_TIMER_DIVISOR;
#if defined(CONFIG_SOC_CV2FS) || defined(CONFIG_SOC_CV22FS) || defined(AMBARELLA_CV5)  /* TW Ambalink: added cv2xfs support */
	divisor = 0;
#else
	divisor = (mmio_read_32(AXI_BASE + AXI_SYS_TIMER_OFFSET) >> 1) & 0xf;
#endif

	return syscnt >> divisor;
}

void bl31_plat_prepare_kernel32_entry(uint64_t x1, uint64_t x2,
		uint64_t x3, uint64_t x4)
{
	SET_SECURITY_STATE(bl33_image_ep_info.h.attr, NON_SECURE);
	bl33_image_ep_info.spsr = SPSR_MODE32(MODE32_hyp, SPSR_T_ARM,
			SPSR_E_LITTLE, DAIF_FIQ_BIT | DAIF_IRQ_BIT | DAIF_ABT_BIT);
	bl33_image_ep_info.pc = x1;
	bl33_image_ep_info.args.arg0 = 0;	/* r0 */
	bl33_image_ep_info.args.arg1 = 0;	/* r1 */
	bl33_image_ep_info.args.arg2 = x2;	/* r2, i.e., dtb */

	/* None of the images on this platform can have 0x0 as the entrypoint */
	assert(bl33_image_ep_info.pc || bl33_image_ep_info.pc & 0x3);

	cm_init_my_context(&bl33_image_ep_info);
	cm_prepare_el3_exit(NON_SECURE);
}

void bl31_plat_runtime_setup(void)
{
#if !defined(CONFIG_ATF_AMBALINK)
	ambarella_cpufreq_parse();
	ambarella_el2_pgtable_setup();

#else // CONFIG_ATF_AMBALINK
#if defined(CONFIG_BLD_SEQ_ATF_SRTOS) || defined(CONFIG_ATF_FIP_RTOS) || (defined(CONFIG_BUILD_QNX_IPL) && defined(CONFIG_ATF_HAVE_BL2))
	int cpu_idx = (int) plat_my_core_pos();

	/*
	 * Because RTOS do not set code-segment as secure.
	 * Need to disable SCR_EL3.SIF, Secure instruction fetch
	 */
	if (cpu_idx == 0) {
#if defined(CONFIG_ATF_FIP_RTOS)
		/* Boot-loader will check and switch to AARCH32 */
		psci_cpu_on(CONFIG_BOOT_CORE_SRTOS, (uintptr_t)CONFIG_ATF_BL33_BLD_BASE, 0);
	} else if (cpu_idx == CONFIG_BOOT_CORE_SRTOS) {
#endif
		cpu_context_t *ctx = cm_get_context(SECURE);
		uint32_t scr_el3;
		el3_state_t *state;

		state = get_el3state_ctx(ctx);
		scr_el3 = read_ctx_reg(state, CTX_SCR_EL3);
		scr_el3 &= ~SCR_SIF_BIT;
		write_ctx_reg(state, CTX_SCR_EL3, scr_el3);
	}
#endif
#endif /* CONFIG_ATF_AMBALINK */
#if defined(CONFIG_ATF_FUSA)
	int r;
	void *addr;
	size_t size;

	/* Allocate inter-cores shared memory used by FuSa library */
	addr = (void *) CONFIG_SHADOW_SHARED_MEM_ADDR + 0x5000;
	r = mmap_add_dynamic_region((unsigned long long) addr,  /* PA */
				    (uintptr_t) addr,           /* VA */
				    0x1000,
				    MT_NON_CACHEABLE | MT_RW);
	if (r != 0) {
		printf("FuSa: mmap_add_dynamic_region %p failed with %d!\n",
		       addr, r);
		goto fusa_done;
	}

	/* Initialize libfusa_ca53.a */
	init_fusa_ca53(addr);

	/*
	 * Part 1
	 */

	/* Steal RTOS reserved memory for buffer used by diagnostic */
	addr = (void *) 0x30000000;
	size = 0x00200000ULL;  // 2MB
	r = mmap_add_dynamic_region((unsigned long long) addr,  /* PA */
				    (uintptr_t) addr,           /* VA */
				    size,
				    MT_MEMORY | MT_RW);
	if (r != 0) {
		printf("FuSa: mmap_add_dynamic_region %p failed with %d!\n",
		       addr, r);
		goto fusa_done;
	}

	/* Execute the dcache/tlb part of the SRLF self diagnostic */
	srlf_self_diag(addr, size, SRLF_SELF_DIAG_DCACHE | SRLF_SELF_DIAG_TLB);

	/* Release the stolen RTOS reserved memory */
	mmap_remove_dynamic_region((uintptr_t) addr, size);

	/*
	 * Part 2
	 */

	/* Map again again as executable region */
	addr = (void *) 0x30000000;
	size = 0x00100000ULL;  // 1MB
	r = mmap_add_dynamic_region((unsigned long long) addr,  /* PA */
				    (uintptr_t) addr,           /* VA */
				    size,
				    MT_CODE);
	if (r != 0) {
		printf("FuSa: mmap_add_dynamic_region %p failed with %d!\n",
		       addr, r);
		goto fusa_done;
	}

	/* Execute the icache part of the SRLF self diagnostic */
	srlf_self_diag(addr, size, SRLF_SELF_DIAG_ICACHE);

	/* Release the stolen RTOS reserved memory */
	mmap_remove_dynamic_region((uintptr_t) addr, size);

fusa_done:

	do {
	} while (0);

#endif
#if 0
#if (DEBUG == 0)
	console_switch_state(CONSOLE_FLAG_RUNTIME);
#endif
#endif
}

