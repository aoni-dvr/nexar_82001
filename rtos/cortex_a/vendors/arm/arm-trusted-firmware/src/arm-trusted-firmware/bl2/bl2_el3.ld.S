/*
 * Copyright (c) 2017-2020, ARM Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <common/bl_common.ld.h>
#include <lib/xlat_tables/xlat_tables_defs.h>

OUTPUT_FORMAT(PLATFORM_LINKER_FORMAT)
OUTPUT_ARCH(PLATFORM_LINKER_ARCH)
ENTRY(bl2_entrypoint)

MEMORY {
#if BL2_IN_XIP_MEM
    ROM (rx): ORIGIN = BL2_RO_BASE, LENGTH = BL2_RO_LIMIT - BL2_RO_BASE
    RAM (rwx): ORIGIN = BL2_RW_BASE, LENGTH = BL2_RW_LIMIT - BL2_RW_BASE
#else
    RAM (rwx): ORIGIN = BL2_BASE, LENGTH = BL2_LIMIT - BL2_BASE
#endif
#if defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)
    SRAM (rwx): ORIGIN = 0x20ee000000, LENGTH = 0x8000
#endif
}

#if defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)
REGION_ALIAS("REGION_DRAM_TRAIN", SRAM);
#endif

#if !BL2_IN_XIP_MEM
#define ROM RAM
#endif

SECTIONS
{
#if BL2_IN_XIP_MEM
    . = BL2_RO_BASE;
    ASSERT(. == ALIGN(PAGE_SIZE),
           "BL2_RO_BASE address is not aligned on a page boundary.")
#else
    . = BL2_BASE;
    ASSERT(. == ALIGN(PAGE_SIZE),
           "BL2_BASE address is not aligned on a page boundary.")
#endif

#if SEPARATE_CODE_AND_RODATA
    .text . : {
        __TEXT_START__ = .;
	__TEXT_RESIDENT_START__ = .;
	*bl2_el3_entrypoint.o(.text*)
	*(.text.asm.*)
	__TEXT_RESIDENT_END__ = .;
#if defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)
        *(SORT_BY_ALIGNMENT(EXCLUDE_FILE(*AmbaDramTraining*.o) .text*))
#else
        *(SORT_BY_ALIGNMENT(.text*))
#endif
        *(.vectors)
        . = ALIGN(PAGE_SIZE);
        __TEXT_END__ = .;
     } >ROM

    .rodata . : {
        __RODATA_START__ = .;
#if defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)
	    *(SORT_BY_ALIGNMENT(EXCLUDE_FILE(*AmbaDramTraining*.o) .rodata*))
#else
        *(SORT_BY_ALIGNMENT(.rodata*))
#endif

	RODATA_COMMON

        . = ALIGN(PAGE_SIZE);
        __RODATA_END__ = .;
    } >ROM

    ASSERT(__TEXT_RESIDENT_END__ - __TEXT_RESIDENT_START__ <= PAGE_SIZE,
          "Resident part of BL2 has exceeded its limit.")
#else
    ro . : {
        __RO_START__ = .;
	__TEXT_RESIDENT_START__ = .;
	*bl2_el3_entrypoint.o(.text*)
	*(.text.asm.*)
	__TEXT_RESIDENT_END__ = .;
#if defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)
        *(SORT_BY_ALIGNMENT(EXCLUDE_FILE(*AmbaDramTraining*.o) .text*))
        *(SORT_BY_ALIGNMENT(EXCLUDE_FILE(*AmbaDramTraining*.o) .rodata*))
#else
        *(SORT_BY_ALIGNMENT(.text*))
        *(SORT_BY_ALIGNMENT(.rodata*))
#endif

	RODATA_COMMON

        *(.vectors)
        __RO_END_UNALIGNED__ = .;
        /*
         * Memory page(s) mapped to this section will be marked as
         * read-only, executable.  No RW data from the next section must
         * creep in.  Ensure the rest of the current memory page is unused.
         */
        . = ALIGN(PAGE_SIZE);

        __RO_END__ = .;
    } >ROM
#endif

    ASSERT(__CPU_OPS_END__ > __CPU_OPS_START__,
          "cpu_ops not defined for this platform.")

#if defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)
    dram_training_ld_addr = .;
	.dram_training_lp5 : {
        dram_training_ex_addr = .;
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
	}  >REGION_DRAM_TRAIN AT>RAM

    __ddrc_training_lp5_ro_size = __ddrc_training_lp5_ro_end - __ddrc_training_lp5_ro_start;
    __ddrc_training_lp5_rw_size = __ddrc_training_lp5_rw_end - __ddrc_training_lp5_rw_start;
    __ddrc_training_lp5_size = __ddrc_training_lp5_end - __ddrc_training_lp5_start;
    . = dram_training_ld_addr + SIZEOF(.dram_training_lp5);
#endif

#if BL2_IN_XIP_MEM
    . = BL2_RW_BASE;
    ASSERT(BL2_RW_BASE == ALIGN(PAGE_SIZE),
           "BL2_RW_BASE address is not aligned on a page boundary.")
#endif

    /*
     * Define a linker symbol to mark start of the RW memory area for this
     * image.
     */
    __RW_START__ = . ;

    /*
     * .data must be placed at a lower address than the stacks if the stack
     * protector is enabled. Alternatively, the .data.stack_protector_canary
     * section can be placed independently of the main .data section.
     */
    .data . : {
        __DATA_RAM_START__ = .;
#if defined(CONFIG_AUTO_DRAM_TRAINING) && !defined(CONFIG_BST_DRAM_TRAINING)
        . = ALIGN(8);
        __ddrc_traning_seq_start = .;
        * (.lpddr4_traning_seq_stage_read)
        . = ALIGN(8);
        ddrc_training_seq_read_end = .;
        * (.lpddr4_traning_seq_stage_write)
        . = ALIGN(8);
        ddrc_training_seq_write_end = .;
        * (.lpddr4_traning_seq_stage_command)
        . = ALIGN(8);
        ddrc_training_seq_command_end = .;
        ddrc_training_seq_stage2_start = .;
        * (.lpddr4_traning_seq_stage2)
        . = ALIGN(8);
        *(BstDramParam)
        . = ALIGN(8);
#if defined(CONFIG_ENABLE_SPINOR_BOOT)
        *(BstNorParam)
#elif defined(CONFIG_ENABLE_EMMC_BOOT)
        *(BstEmmcParam)
#elif defined(CONFIG_ENABLE_SPINAND_BOOT)
        *(BstSpiNandParam)
#else
        *(BstNandParam)
#endif
        . = ALIGN(8);
        ddrc_training_seq_stage2_end = .;
        __ddrc_traning_seq_end = .;
/*
        . += (0x100 - (__ddrc_traning_seq_end % 0x100));
*/
        . = ALIGN(4);
#endif
        *(SORT_BY_ALIGNMENT(.data*))
        __DATA_RAM_END__ = .;
    } >RAM AT>ROM

    /*
     * .rela.dyn needs to come after .data for the read-elf utility to parse
     * this section correctly. Ensure 8-byte alignment so that the fields of
     * RELA data structure are aligned.
     */
    . = ALIGN(8);
    __RELA_START__ = .;
    .rela.dyn . : {
    } >RAM
    __RELA_END__ = .;

    stacks (NOLOAD) : {
        __STACKS_START__ = .;
        *(tzfw_normal_stacks)
        __STACKS_END__ = .;
    } >RAM AT>RAM

    BSS_SECTION >RAM AT>RAM
    XLAT_TABLE_SECTION >RAM AT>RAM

#if USE_COHERENT_MEM
    /*
     * The base address of the coherent memory section must be page-aligned (4K)
     * to guarantee that the coherent data are stored on their own pages and
     * are not mixed with normal data.  This is required to set up the correct
     * memory attributes for the coherent data page tables.
     */
    coherent_ram (NOLOAD) : ALIGN(PAGE_SIZE) {
        __COHERENT_RAM_START__ = .;
        *(tzfw_coherent_mem)
        __COHERENT_RAM_END_UNALIGNED__ = .;
        /*
         * Memory page(s) mapped to this section will be marked
         * as device memory.  No other unexpected data must creep in.
         * Ensure the rest of the current memory page is unused.
         */
        . = ALIGN(PAGE_SIZE);
        __COHERENT_RAM_END__ = .;
    } >RAM
#endif

    /*
     * Define a linker symbol to mark end of the RW memory area for this
     * image.
     */
    __RW_END__ = .;
    __BL2_END__ = .;

    /DISCARD/ : {
        *(.dynsym .dynstr .hash .gnu.hash)
    }

#if BL2_IN_XIP_MEM
    __BL2_RAM_START__ = ADDR(.data);
    __BL2_RAM_END__ = .;

    __DATA_ROM_START__ = LOADADDR(.data);
    __DATA_SIZE__ = SIZEOF(.data);

    /*
     * The .data section is the last PROGBITS section so its end marks the end
     * of BL2's RO content in XIP memory..
     */
    __BL2_ROM_END__ =  __DATA_ROM_START__ + __DATA_SIZE__;
    ASSERT(__BL2_ROM_END__ <= BL2_RO_LIMIT,
           "BL2's RO content has exceeded its limit.")
#endif
    __BSS_SIZE__ = SIZEOF(.bss);


#if USE_COHERENT_MEM
    __COHERENT_RAM_UNALIGNED_SIZE__ =
        __COHERENT_RAM_END_UNALIGNED__ - __COHERENT_RAM_START__;
#endif

#if BL2_IN_XIP_MEM
    ASSERT(. <= BL2_RW_LIMIT, "BL2's RW content has exceeded its limit.")
#else
    ASSERT(. <= BL2_LIMIT, "BL2 image has exceeded its limit.")
#endif
}
