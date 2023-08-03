#define DDR_START           0x00000000
#define DDR_RSVD_SIZE       CONFIG_FWPROG_SYS_LOADADDR

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

#ifdef CONFIG_CV_MEM_SIZE
#define CV_SIZE         CONFIG_CV_MEM_SIZE
#else
#define CV_SIZE         0
#endif
#ifdef CONFIG_APP_MEM_SIZE
#define APP_SIZE        CONFIG_APP_MEM_SIZE
#else
#define APP_SIZE        0
#endif
#ifdef CONFIG_AMBALINK_SHARED_MEM_ADDR
#define LINUX_START     CONFIG_AMBALINK_SHARED_MEM_ADDR
#else
#define LINUX_START      0
#endif
#ifdef CONFIG_AMBALINK_MEM_SIZE
#define LINUX_SIZE      CONFIG_AMBALINK_MEM_SIZE
#else
#define LINUX_SIZE      0
#endif
#ifdef CONFIG_AMBALINK_SHARED_MEM_SIZE
#define A53_SHMEM_SIZE  CONFIG_AMBALINK_SHARED_MEM_SIZE
#else
#define A53_SHMEM_SIZE  0
#endif
#ifdef CONFIG_CV_MEM_RTOS_SIZE
#define CV_RTOS_USER_SIZE        CONFIG_CV_MEM_RTOS_SIZE
#else
#define CV_RTOS_USER_SIZE        0
#endif
#ifdef CONFIG_CV_MEM_SCHDR_SIZE
#define CV_SCHDR_SIZE			CONFIG_CV_MEM_SCHDR_SIZE
#else
#define CV_SCHDR_SIZE			(CV_SIZE - CV_RTOS_USER_SIZE)
#endif

#define ATT_PAGE_SIZE       0x40000

#if defined(CONFIG_DEVICE_TREE_SUPPORT)
#define DSP_SIZE	(CONFIG_DSP_PROT_BUF_SIZE + CONFIG_DSP_PROT_CACHE_BUF_SIZE + CONFIG_DSP_BIN_BUF_SIZE + CONFIG_DSP_DATA_BUF_SIZE)
#else
#define DSP_SIZE	0
#endif

#if defined(CONFIG_THREADX64)
#define MMU_MB_ALIGN    0x200000
#else
#define MMU_MB_ALIGN    0x100000
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
		*AmbaSysStack.o (.text*)
		*AmbaBoot_A*.o (.rodata*)
		*AmbaSysStack.o (.rodata*)
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
#if defined(CONFIG_BUILD_COMSVC_DRAMSHMOO) && !defined(CONFIG_SOC_CV2FS) && !defined(CONFIG_SOC_CV22FS)
		*(BstDramParam)
#endif

#ifdef CONFIG_BUILD_CV
		/* For ambacv init table */
		. = ALIGN(64);
		__cvtask_init_start = .;
		KEEP(* (.cvtask_create_fp*))
		__cvtask_init_end = .;

		. = ALIGN(0x80);
		__cvtask_format_start = .;
		KEEP (* (.cvtask_format*))
		__cvtask_format_end = .;
#endif

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

	.data : {
		. = ALIGN(0x1000);
		__ddr_region0_rw_start = .;
		*AmbaBoot.o (.data*)
		*AmbaSysStack.o (.data*)
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

#if defined(CONFIG_DEVICE_TREE_SUPPORT)
	.dsp_buf (NOLOAD) : {
		. = ALIGN(MMU_MB_ALIGN);
		__dsp_cache_buf_start = .;
		. = ALIGN(MMU_MB_ALIGN);
		__dsp_cache_buf_end = .;
		. = ALIGN(MMU_MB_ALIGN);
		__dsp_buf_start = .;
		. = ALIGN(MMU_MB_ALIGN);
		__dsp_buf_end = .;
	}
#else
	.dsp_buf (NOLOAD) : {
		. = ALIGN(MMU_MB_ALIGN);
		__dsp_cache_buf_start = .;
		. = __dsp_cache_buf_start + CONFIG_DSP_PROT_CACHE_BUF_SIZE;
		. = ALIGN(MMU_MB_ALIGN);
		__dsp_cache_buf_end = .;
		. = ALIGN(MMU_MB_ALIGN);
		__dsp_buf_start = .;
		. = __dsp_buf_start + CONFIG_DSP_PROT_BUF_SIZE;
		. = ALIGN(MMU_MB_ALIGN);
		__dsp_buf_end = .;
	}
#endif

	/* Provided malloc heap for libc. */
	. = ALIGN(0x1000);
	PROVIDE (end = .);
	__sbrk_start = .;
	. = __sbrk_start + 0x2000;
	__sbrk_end = .;

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

#if !defined(CONFIG_DEVICE_TREE_SUPPORT)
	/* FIXME: There are some unused memory here ! */

	/***********************/
	/* ucode orcode region */
	/***********************/
	. = ALIGN(MMU_MB_ALIGN);
	__ucode_start = .;
	__ucode_orcode_start = .;
#if defined(CONFIG_SOC_CV5) || defined (CONFIG_SOC_CV52)
	. = __ucode_orcode_start + 0x780000;
#else
	. = __ucode_orcode_start + 0x380000;
#endif
	__ucode_orcode_end = .;

	. = ALIGN(64);
	__ucode_orcme_start = .;
	. = __ucode_orcme_start + 0x80000;
	__ucode_orcme_end = .;

	. = ALIGN(64);
	__ucode_orcmd_start = .;
	. = __ucode_orcmd_start + 0x80000;
	__ucode_orcmd_end = .;

	. = ALIGN(64);
	__ucode_default_data_start = .;
	. = __ucode_default_data_start + 0x80000;
	__ucode_default_data_end = .;
	__ucode_end = .;
#endif

	/***************************/
	/*  cached heap region     */
	/***************************/
	. = ALIGN(MMU_MB_ALIGN);
	__cache_heap_start = .;
	cache_heap_start = .;       /* CERT DCL37-C */
	. = __cache_heap_start + CONFIG_SVC_CACHE_HEAP_SIZE;
	cache_heap_end = .;         /* CERT DCL37-C */
	__cache_heap_end = .;

	/***************************/
	/*  non-cached heap region */
	/***************************/
	. = ALIGN(MMU_MB_ALIGN);
	__non_cache_heap_start = .;
	non_cache_heap_start = .;       /* CERT DCL37-C */
#if (CV_SIZE < LINUX_SIZE)
	. = __ddr_start + DDR_SIZE - LINUX_SIZE - APP_SIZE - DSP_SIZE;
#else
	. = __ddr_start + DDR_SIZE - LINUX_SIZE - APP_SIZE - CV_SIZE - DSP_SIZE;
#endif
	non_cache_heap_end = .;         /* CERT DCL37-C */
	__non_cache_heap_end = .;
	ASSERT(__non_cache_heap_start <= __non_cache_heap_end, "Not enough memory for linux and CV")
#ifdef CONFIG_ATF_BL33_BLD_BASE
	ASSERT(__non_cache_heap_end <= CONFIG_ATF_BL33_BLD_BASE, "LD Error: Overlap BLD")
#endif /* CONFIG_ATF_BL33_BLD_BASE */

#if !defined(CONFIG_ATT_8GB)
	. = __non_cache_heap_end + DSP_SIZE;
#endif

    /***************************/
    /*  App region             */
    /***************************/
    . = ALIGN(MMU_MB_ALIGN);
    __app_start = .;
    . = __app_start + APP_SIZE;
    __app_end = .;

    /***************************/
    /*  Linux region           */
    /***************************/
    . = ALIGN(MMU_MB_ALIGN);
    __linux_start = .;
    __a53_shm_start = __linux_start;
    __a53_shm_end = __linux_start + A53_SHMEM_SIZE;
    . = __linux_start + LINUX_SIZE;
    __linux_end = .;
    ASSERT((__ddr_start + DDR_SIZE) >= __linux_end, "__linux_end > __ddr_end")

#if !defined(CONFIG_DEVICE_TREE_SUPPORT)
#if (CV_SIZE < LINUX_SIZE)
    /***************************/
    /*	CV region in Linux     */
    /***************************/
    __cv_start = __linux_end - CV_SIZE;
    . = __cv_start;
    ASSERT(__cv_start >= (__linux_start + 0x4000000), "Not enough memory of linux, reserved 64MB for linux")
#else
    /***************************/
    /*  CV region in RTOS only */
    /***************************/
    . = ALIGN(MMU_MB_ALIGN);
    __cv_start = .;
#endif
    ASSERT((CV_SCHDR_SIZE + CV_RTOS_USER_SIZE) <= CV_SIZE, "CV SCHDR size and USER size exceeds CV size")

    __cv_sys_start = .;
#if (CV_SIZE < LINUX_SIZE)
	. = ALIGN(0x400000);
	ASSERT(. == __cv_sys_start, "__cv_sys_start not 4MB align")
#endif
    __cv_schdr_start = .;
    . = __cv_schdr_start + CV_SCHDR_SIZE;
    __cv_schdr_end = .;
    . = __cv_schdr_end + CV_SIZE - CV_SCHDR_SIZE - CV_RTOS_USER_SIZE;
    __cv_sys_end = .;
#if (CV_SIZE < LINUX_SIZE)
	. = ALIGN(0x400000);
	ASSERT(. == __cv_sys_end, "__cv_sys_end not 4MB align")
#endif
    __cv_rtos_user_start = .;
    . = __cv_rtos_user_start + CV_RTOS_USER_SIZE;
    __cv_rtos_user_end = .;
    __cv_end = .;
    ASSERT((__ddr_start + DDR_SIZE) >= __cv_end, "__cv_end > __ddr_end")
#endif

	/*************************/
	/* DDR end               */
	/*************************/

    /*
    * Provide a end symbol for newlib malloc.
    * We don't use malloc for newlib, so it should be ok.
    */

    . = __ddr_start + DDR_SIZE;
    __ddr_end = .;

#ifdef CONFIG_ATF_BL32_SHMEM_SIZE
    ASSERT(__ddr_end <= (CONFIG_DDR_SIZE - CONFIG_ATF_BL32_SHMEM_SIZE), "LD Error: Overlap ATF BL32_SHMEM")
#endif
}
