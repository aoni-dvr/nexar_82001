/**
 *  @file amba_bld.lds.S
 *
 *  @copyright Copyright (c) 2016 Ambarella, Inc.
 *
 *  This file and its contents ("Software") are protected by intellectual property rights including, without limitation,
 *  U.S. and/or foreign copyrights.  This Software is also the confidential and proprietary information of Ambarella, Inc.
 *  and its licensors.  You may not use, reproduce, disclose, distribute, modify, or otherwise prepare derivative works
 *  of this Software or any portion thereof except pursuant to a signed license agreement or nondisclosure agreement with
 *  Ambarella, Inc. or its authorized affiliates.  In the absence of such an agreement, you agree to promptly notify and
 *  return this Software to Ambarella, Inc.
 *
 *  THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 *  WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL
 *  AMBARELLA, INC. OR ITS AFFILIATES BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 *  DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 *  COMPUTER FAILURE OR MALFUNCTION; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 *  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 *  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *  @details Linker script for bootloader
 *
 */

#if defined(CONFIG_ARM64)
OUTPUT_FORMAT("elf64-littleaarch64")
OUTPUT_ARCH(aarch64)
#else
OUTPUT_FORMAT("elf32-littlearm")
OUTPUT_ARCH(arm)
#endif

BLD_ALIGN_SIZE  = 0x100;

ENTRY(__vector)

#if defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)
MEMORY
  {
#ifdef CONFIG_ATF_HAVE_BL2
    NORMAL (rwx): ORIGIN = CONFIG_ATF_BL33_BLD_BASE, LENGTH = 0x20000000
#else
    NORMAL (rwx): ORIGIN = CONFIG_FWPROG_BLD_LOADADDR, LENGTH = 0x20000000
#endif
    SRAM (rwx): ORIGIN = 0x20ee000000, LENGTH = 0x8000
  }

REGION_ALIAS("REGION_DRAM_TRAIN", SRAM);
#endif

SECTIONS
{
#ifdef CONFIG_ATF_HAVE_BL2
	. = CONFIG_ATF_BL33_BLD_BASE;
#elif defined(CONFIG_FWPROG_BLD_LOADADDR) && !defined(CONFIG_SOC_CV2FS)
	. = CONFIG_FWPROG_BLD_LOADADDR;
#elif defined(CONFIG_SOC_CV2FS)
	. = CONFIG_FWPROG_BLD2_LOADADDR;
#else
	. = 0x10000000;
#endif /* CONFIG_ATF_HAVE_BL2 */
	.text : {
		*AmbaBLD_Boot*.o (.text)

#if defined(CONFIG_ENABLE_SPINOR_BOOT)
		*AmbaBLD_SpiNOR*.o (.text)
#elif defined(CONFIG_ENABLE_EMMC_BOOT)
		*AmbaBLD_eMMC*.o (.text)
#elif defined(CONFIG_ENABLE_SPINAND_BOOT)
		*AmbaBLD_SPINAND*.o (.text)
#else
		*AmbaBLD_NAND*.o (.text)
#endif
		*AmbaBLD_MMU.o (.text)
		*AmbaBLD_Shell.o (.text)
		*(BstDramParam*)
		* (EXCLUDE_FILE(*AmbaDramTraining*.o) .text*)
#ifdef CONFIG_MMU
		*(.fixup)
#endif
	}

#if defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)
    dram_training_ld_addr = .;
	.dram_training_lp5 : {
        . = ALIGN(16);
        __ddrc_training_lp5_start = .;
        __ddrc_training_lp5_ro_start = .;
        . = ALIGN(16);
        *AmbaDramTraining.o (.text*)
        . = ALIGN(16);
        *AmbaDramTraining.o (training_text)
        . = ALIGN(16);
        *AmbaDramTrainingFunc.o (.text*)
        . = ALIGN(16);
        *AmbaDramTrainingFunc.o (training_text)
        . = ALIGN(16);
        *(.lp5_traning_assembly)
        . = ALIGN(16);
        ddrc_training_seq_lp5_end = .;
        . = ALIGN(16);
        *AmbaDramTrainingLog.o (.text*)
        . = ALIGN(16);
        *AmbaDramTrainingLog.o (training_text)
        . = ALIGN(16);
        *AmbaDramTraining.o (.rodata*)
        . = ALIGN(16);
        *AmbaDramTrainingFunc.o (.rodata*)
        . = ALIGN(16);
        *AmbaDramTrainingLog.o (.rodata*)
        . = ALIGN(16);
        __ddrc_training_lp5_ro_end = .;
        . = ALIGN(0x3000);
        __ddrc_training_lp5_rw_start = .;
        *AmbaDramTraining.o (.data*)
        . = ALIGN(16);
        *AmbaDramTrainingFunc.o (.data*)
        . = ALIGN(16);
        *AmbaDramTrainingLog.o (.data*)
        . = ALIGN(16);
        *AmbaDramTraining.o (.bss*)
        . = ALIGN(16);
        *AmbaDramTrainingFunc.o (.bss*)
        . = ALIGN(16);
        *AmbaDramTrainingLog.o (.bss*)
        . = ALIGN(16);
        __ddrc_training_lp5_rw_end = .;
        __ddrc_training_lp5_end = .;
	}  >REGION_DRAM_TRAIN AT>NORMAL

    __ddrc_training_lp5_ro_size = __ddrc_training_lp5_ro_end - __ddrc_training_lp5_ro_start;
    __ddrc_training_lp5_rw_size = __ddrc_training_lp5_rw_end - __ddrc_training_lp5_rw_start;
    __ddrc_training_lp5_size = __ddrc_training_lp5_end - __ddrc_training_lp5_start;
#endif

    .dram_training : {
        __ddrc_traning_seq_start = .;
        * (.lpddr4_traning_seq_stage_ca)
        . = ALIGN(16);
        ddrc_training_seq_ca_end = .;
        * (.lpddr4_traning_seq_stage_read)
        . = ALIGN(16);
        ddrc_training_seq_read_end = .;
        * (.lpddr4_traning_seq_stage_write)
        . = ALIGN(16);
        ddrc_training_seq_write_end = .;
        * (.lpddr4_traning_seq_stage_command)
        . = ALIGN(16);
        ddrc_training_seq_command_end = .;
        ddrc_training_seq_stage2_start = .;
        * (.lpddr4_traning_seq_stage2)
        . = ALIGN(16);
#if defined(CONFIG_ENABLE_SPINOR_BOOT)
        *(BstNorParam)
#elif defined(CONFIG_ENABLE_EMMC_BOOT)
        *(BstEmmcParam)
#elif defined(CONFIG_ENABLE_SPINAND_BOOT)
        *(BstSpiNandParam)
#else
        *(BstNandParam)
#endif
        . = ALIGN(16);
        ddrc_training_seq_stage2_end = .;
        __ddrc_traning_seq_end = .;
        . += (BLD_ALIGN_SIZE - (__ddrc_traning_seq_end % BLD_ALIGN_SIZE));
	}

	. = ALIGN(4);
	.data : {
		* (.data*)
		* (.rodata*)
	}

#if defined(CONFIG_ATF_HAVE_BL2) && defined(CONFIG_BUILD_QNX_IPL)
	. = ALIGN(8);
	. += 0x1000;
	__stack_ptr = .;
#endif

	. = ALIGN(4);
	.bss.noinit : {
		__noinit_start = .;
		* (.bss.noinit*)
		__noinit_end = .;
	}

	. = ALIGN(4);
	.bss : {
		. = ALIGN(4);
		__bss_start = .;
		* (.bss*)
		* (COMMON)      /* global variables will be placed in this section if compile without -fno-common */
		__bss_end = .;
	}

#ifdef CONFIG_ARM_UNWIND
	/*
	 * Stack unwinding tables
	 */
	. = ALIGN(8);
	.ARM.unwind_idx : {
		__start_unwind_idx = .;
		*(.ARM.exidx*)
		__stop_unwind_idx = .;
	}
	.ARM.unwind_tab : {
		__start_unwind_tab = .;
		*(.ARM.extab*)
		__stop_unwind_tab = .;
	}
#endif

	PROVIDE (end = .);

	.memfwprog . (NOLOAD) : {
		. = ALIGN(0x10000);
		__memfwprog_result = .;
		. = __memfwprog_result  + (0x10000);
		. = ALIGN(0x10000);
		__memfwprog_command = .;
		. = __memfwprog_command + (0x10000);
	}

	/DISCARD/ : {
		*(.ARM.exidx.exit.text)
		*(.ARM.extab.exit.text)
		*(.ARM.exidx.cpuexit.text)
		*(.ARM.extab.cpuexit.text)
		*(.ARM.exidx.devexit.text)
		*(.ARM.extab.devexit.text)
		*(.discard*)
	}

	__ddr_verify_start = .;

#ifdef CONFIG_ATF_BL32_SHMEM_SIZE
	ASSERT(__ddr_verify_start < (CONFIG_DDR_SIZE - CONFIG_ATF_BL32_SHMEM_SIZE), "LD Error: Overlap ATF BL32_SHMEM")
#endif // CONFIG_ATF_BL32_SHMEM_SIZE
}

