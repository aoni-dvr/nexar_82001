#define DDR_START           0x00000000
#define DDR_RSVD_SIZE       CONFIG_FWPROG_SYS_LOADADDR
#define ATT_PAGE_SIZE       0x40000

#if defined(CONFIG_ATF_BL32_SHMEM_SIZE)
#define BL32_SHMEM_SIZE     CONFIG_ATF_BL32_SHMEM_SIZE
#else
#define BL32_SHMEM_SIZE     0
#endif

#if defined(CONFIG_PIO_SIZE)
#define PIO_SIZE            CONFIG_PIO_SIZE
#else
#define PIO_SIZE            0
#endif

#if (CONFIG_FWPROG_R52SYS_LOADADDR != 0x00000000)
#define DDR_SIZE            (CONFIG_FWPROG_R52SYS_LOADADDR - BL32_SHMEM_SIZE - PIO_SIZE)
#else
#define DDR_SIZE            (CONFIG_DDR_SIZE - BL32_SHMEM_SIZE - PIO_SIZE)
#endif

#ifdef CONFIG_AMBALINK_MEM_SIZE
#define LINUX_SIZE          CONFIG_AMBALINK_MEM_SIZE
#else
#define LINUX_SIZE          0
#endif

#ifdef CONFIG_AMBALINK_SHARED_MEM_ADDR
#define SHARED_MEM_SIZE     CONFIG_AMBALINK_SHARED_MEM_ADDR
#else
#define SHARED_MEM_SIZE     0
#endif

#define ATT_PAGE_SIZE       0x40000

#ifdef CONFIG_DSP_PROT_CACHE_BUF_SIZE
#define DSP_PROT_CACHE_BUF_SIZE	CONFIG_DSP_PROT_CACHE_BUF_SIZE
#else
#if defined(CONFIG_SOC_CV2FS) || defined(CONFIG_SOC_CV22FS)
#define DSP_PROT_CACHE_BUF_SIZE	0x00450000
#else
#define DSP_PROT_CACHE_BUF_SIZE	0x00100000
#endif
#endif

#ifdef CONFIG_DSP_PROT_BUF_SIZE
#define DSP_PROT_BUF_SIZE       CONFIG_DSP_PROT_BUF_SIZE
#else
#define DSP_PROT_BUF_SIZE       0x00300000
#endif

#if defined(CONFIG_ARM64)
OUTPUT_FORMAT("elf64-littleaarch64")
OUTPUT_ARCH(aarch64)
#else
OUTPUT_FORMAT("elf32-littlearm")
OUTPUT_ARCH(arm)
#endif
ENTRY(__vector)
SECTIONS
{
	. = DDR_START;
	__ddr_start = .;
	__ddr_resv_start = .;
	. = __ddr_resv_start + DDR_RSVD_SIZE;
	__ddr_resv_end = .;
	.text : {
		__ddr_region0_ro_start = .;
		*AmbaBoot_A*.o (.text*)
		*AmbaHwStack.o (.text*)
		*AmbaBoot_A*.o (.rodata*)
		*AmbaHwStack.o (.rodata*)
#if defined(CONFIG_ARM64)
        . = ALIGN(4096);
        __system_exception_table_start = .;
        * (ExceptionTable*)
#else
        __system_exception_table_start = __ddr_region0_ro_start;
#endif
		* (.text*)
		* (.rodata*)
		* (.constdata*)
		/* For libaudio.a */
		* (i.__ARM_common*)
		* (FFTCODE*)
		__ddr_region0_ro_end = .;

		. = ALIGN(64);
		__ddr_region1_ro_start = .;
		__ddr_region1_ro_end = .;

		. = ALIGN(64);
		__ddr_region2_ro_start = .;
		__ddr_region2_ro_end = .;

		. = ALIGN(64);
		__ddr_region3_ro_start = .;
		__ddr_region3_ro_end = .;

		. = ALIGN(64);
		__ddr_region4_ro_start = .;
		__ddr_region4_ro_end = .;

		. = ALIGN(64);
		__ddr_region5_ro_start = .;
		__ddr_region5_ro_end = .;
	}

	/* The symbol is necessary for firmware programmer */
	.dsp_buf (NOLOAD) : {
		. = ALIGN(ATT_PAGE_SIZE);
		__dsp_cache_buf_start = .;
		. = __dsp_cache_buf_start + DSP_PROT_CACHE_BUF_SIZE;
		. = ALIGN(ATT_PAGE_SIZE);
		__dsp_cache_buf_end = .;
		. = ALIGN(ATT_PAGE_SIZE);
		__dsp_buf_start = .;
		. = __dsp_buf_start + DSP_PROT_BUF_SIZE;
		. = ALIGN(ATT_PAGE_SIZE);
		__dsp_buf_end = .;
	}

	.data : {
		. = ALIGN(0x1000);
		__ddr_region0_rw_start = .;
		*AmbaBoot.o (.data*)
		*AmbaHwStack.o (.data*)
		* (.data*)
		__ddr_region0_rw_end = .;

		. = ALIGN(64);
		__ddr_region1_rw_start = .;
		__ddr_region1_rw_end = .;

		. = ALIGN(64);
		__ddr_region2_rw_start = .;
		__ddr_region2_rw_end = .;

		. = ALIGN(64);
		__ddr_region3_rw_start = .;
		__ddr_region3_rw_end = .;

		. = ALIGN(64);
		__ddr_region4_rw_start = .;
		__ddr_region4_rw_end = .;

		. = ALIGN(64);
		__ddr_region5_rw_start = .;
		__ddr_region5_rw_end = .;
	}

	/* Provided malloc heap for libc. */
	. = ALIGN(0x1000);
	PROVIDE (end = .);
	__sbrk_start = .;
	. = __sbrk_start + 0x2000;
	/*
	 * Provide a end symbol for newlib malloc.
	 * We dont use malloc for newlib, so it should be ok.
	 */
	__sbrk_end = .;

    . = ALIGN(64);
    __backtrace_start = .;
    . = __backtrace_start + 0x100000;
    __backtrace_end = .;

    __varsym_start = .;
    . = __varsym_start + 0x20000;
    __varsym_end = .;

	.bss.noinit : {
		. = ALIGN(64);
		__noinit_start = .;
		* (.bss.noinit*)
		__noinit_end = .;
	}

	.bss : {
		. = ALIGN(64);
		__bss_start = .;
		* (.bss*)
		__bss_end = .;
	}

	/*********************************************/
	/* ucode orcode region                       */
    /* it is not used in io-ut, but bsp needs it */
    /* so assigned 0 for it                      */
	/*********************************************/
	. = ALIGN(0x100000);
	__ucode_start = .;
	__ucode_orcode_start = .;
	__ucode_orcode_end = .;

	. = ALIGN(64);
	__ucode_orcme_start = .;
	__ucode_orcme_end = .;

	. = ALIGN(64);
	__ucode_orcmd_start = .;
	__ucode_orcmd_end = .;

	. = ALIGN(64);
	__ucode_default_data_start = .;
	__ucode_default_data_end = .;
	__ucode_end = .;

	/**********************************/
	/*  ATT DSP data region           */
	/**********************************/
    __dsp_data_start = .;
    __dsp_data_end = .;

	/***************************/
	/*  non-cached heap region */
	/***************************/
	. = ALIGN(0x100000);
	__non_cache_heap_start = .;
	. = __non_cache_heap_start + 0x1000000;
	__non_cache_heap_end = .;

	/***************************/
	/*  cached heap region     */
	/***************************/
	. = ALIGN(0x100000);
	__cache_heap_start = .;
	. = __ddr_start + DDR_SIZE - LINUX_SIZE;
	__cache_heap_end = .;
	ASSERT(__cache_heap_start <= __cache_heap_end, "Not enough memory for linux and CV")
#if defined(CONFIG_ATF_BL33_BLD_BASE) && defined(CONFIG_ATF_SPD_OPTEE)
    /* Core running OP-TEE runs bld at EL1 */
	ASSERT(__cache_heap_end <= CONFIG_ATF_BL33_BLD_BASE, "LD Error: Overlap BLD")
#endif /* CONFIG_ATF_BL33_BLD_BASE */

#ifdef CONFIG_ENABLE_AMBALINK
	. = SHARED_MEM_SIZE;
	__linux_start = .;
	. = __linux_start + LINUX_SIZE;
	__linux_end = .;
#endif

    __cv_sys_start = .;
    . = __cv_sys_start + 0x4000;
    __cv_sys_end = .;

	/*************************/
	/* DDR end               */
	/*************************/
	. = __ddr_start + DDR_SIZE;
	__ddr_end = .;

#ifdef CONFIG_ENABLE_AMBALINK
	ASSERT((__ddr_start + DDR_SIZE) >= __linux_end, "__linux_end > __ddr_end")
#endif

#ifdef CONFIG_ATF_BL32_SHMEM_SIZE
	ASSERT(__ddr_end <= (CONFIG_DDR_SIZE - CONFIG_ATF_BL32_SHMEM_SIZE), "LD Error: Overlap ATF BL32_SHMEM")
#endif // CONFIG_ATF_BL32_SHMEM_SIZE
}

