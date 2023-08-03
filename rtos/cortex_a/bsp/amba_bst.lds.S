/**
 *  @file amba_bst.lds.S
 *
 *  @copyright Copyright (c) 2019 Ambarella, Inc.
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
 *  @details Linker script for bootstrap
 *
 */

DRAM_START_ADDR = 0x00000000;
MIN_DRAM_SIZE   = 0x01000000;

#if defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)
NAND_BST_START = 0x20e000c000;
#elif defined(CONFIG_ENABLE_SPINOR_BOOT) || defined(CONFIG_SOC_CV22)
NAND_BST_START = 0xe0003000;
#else
NAND_BST_START = 0xFFFF0000;
#endif

#if defined(CONFIG_BST_LARGE_SIZE)
    #if defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)
    MEMORY {
        BST_1_SRAM (rwx): ORIGIN = 0x20e000c000, LENGTH = 0x2000
        BST_2_SRAM (rwx): ORIGIN = 0x20e0031000, LENGTH = 0x1000
        BST_3_SRAM (rwx): ORIGIN = 0x20e0032000, LENGTH = 0x2000
        BST_D_SRAM (rwx): ORIGIN = 0x20e0030000, LENGTH = 0x1000
        BST_S_SRAM (rwx): ORIGIN = 0x20e000c000, LENGTH = 0x2000
    }
    #endif
#endif

#if defined(CONFIG_ENABLE_SECURITY)
    #if defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)
    #if defined(CONFIG_BST_LARGE_SIZE)
    BST_PAD_SIZE    = 0x2000; /* 8K bytes padding to create 16k bootstrap. */
    #else
    BST_PAD_SIZE    = 0xFC0; /* 4K - 64.  64 bytes are for CV5 ECC signature. */
    #endif
    #else
    BST_PAD_SIZE    = 0xF00; /* 4K - 256. 256 bytes are for CV2X/CV2XFS RSA signature. */
    #endif
#else
    #if defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)
    BST_PAD_SIZE    = 0x2000; /* 8K bytes for BST. */
    #else
    BST_PAD_SIZE    = 0x1000; /* 4K bytes for BST. */
    #endif
#endif
BST_ALIGN_SIZE  = 0x8;

AMBOOT_BST_RAM_START = DRAM_START_ADDR + 0x200;


#if defined(CONFIG_ARM64)
OUTPUT_FORMAT("elf64-littleaarch64", "elf64-littleaarch64", "elf64-littleaarch64")
OUTPUT_ARCH(aarch64)
#else
OUTPUT_FORMAT("elf32-littlearm", "elf32-littlearm", "elf32-littlearm")
OUTPUT_ARCH(arm)
#endif

ENTRY(__vector)

#if defined(CONFIG_BST_LARGE_SIZE)
/* CV5x+LP5 and later chips */
SECTIONS
{
	.bst_stage1 : {
#if defined(CONFIG_ENABLE_SECURITY)
		. += 16;                                /* BST header size */
#endif
        __bst_stage1_ram_start = .;
		*AmbaBST.o (.text.first_stage)
		*AmbaBST_SecureBoot.o  (.text)
		*AmbaBST_Init*.o (.text)
		*AmbaBST_LoadPref* (.text)
		*AmbaBST_UserConfig* (.text)
		*AmbaBST_Memdiag* (.text)
		*AmbaBST.o (.rodata)
		*AmbaBST_SecureBoot.o  (.rodata)
		*AmbaBST_Init*.o (.rodata)
		*utils.o (.text.first_stage .rodata)
		*AmbaBST_LoadPref* (.rodata)
		*AmbaBST_UserConfig* (.rodata)
		*AmbaBST_Memdiag* (.rodata)
		*(AmbaDeviceInit)
		. = ALIGN(16);
		__bst_stage1_ram_end = .;
	} > BST_1_SRAM

	.bst_stage2 : AT (ADDR(.bst_stage1) + SIZEOF(.bst_stage1)) {
		__bst_stage2_ram_start = .;
		*AmbaBST.o (.text.second_stage)
		*sram_fifo*.o (.text.first_stage)
		*(BstDramParam)
#if defined(CONFIG_ENABLE_SPINOR_BOOT)
		*(BstNorParam)
#elif defined(CONFIG_ENABLE_EMMC_BOOT)
		*(BstEmmcParam)
#elif defined(CONFIG_ENABLE_SPINAND_BOOT)
		*(BstSpiNandParam)
#else
		*(BstNandParam)
#endif
		__bst_param_end = .;
		*uartp.o (.text* .rodata* .data*)       /* For CV5 BST training */
		*utils.o (.text.second_stage)
		*rsa*.o (.text)
		*auth*.o (.text)
#if defined(CONFIG_BST_WDELAY_UPDATE_DRIFT)
		*dram_adjust_lpddr5.o (.text* .rodata*)
		. = ALIGN(4);
#endif
		. = ALIGN(16);
		__bst_stage2_ram_end = .;
	} > BST_2_SRAM

	__bst_stage2_load_start = LOADADDR(.bst_stage2);
	__bst_stage2_load_end = __bst_stage2_load_start + __bst_stage2_ram_end - __bst_stage2_ram_start;

#if defined(CONFIG_ENABLE_SECURITY)
	ASSERT(SIZEOF(.bst_stage1) + SIZEOF(.bst_stage2) <= 7168, "Out of size 7k for dram training result")
#endif
	.bst_training : AT (ADDR(.bst_stage1) + BST_PAD_SIZE) {
#if defined(CONFIG_ENABLE_SECURITY)
		. += 16;                                /* BST header size */
#if defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)
		*back_2nd_stage*.o (.text)              /* It must at the beginning of the second 8k of bootstrap. */
#endif
#endif
#if defined(CONFIG_BST_DRAM_TRAINING)
		*dram_training*.o (.text* .rodata* .data*)
#endif
#if defined(CONFIG_ENABLE_SPINOR_BOOT)
		*AmbaBST_SpiNOR*.o (.text)
#elif defined(CONFIG_ENABLE_EMMC_BOOT)
		*AmbaBST_eMMC*.o (.text)
#elif defined(CONFIG_ENABLE_SPINAND_BOOT)
		*AmbaBST_SpiNAND*.o (.text)
#else
		*AmbaBST_NAND*.o (.text)
#endif
#if defined(CONFIG_ENABLE_SECURITY)
		. += 64;                                /* Security signature size */
#endif
		. += BST_PAD_SIZE - (SIZEOF(.bst_training)) ;
	} > BST_3_SRAM

	.bss (NOLOAD) : {
		. = ALIGN(8);
		__bss_start = .;
		* (.bss)
		. = ALIGN(8);
		__bss_end = .;
    } > BST_D_SRAM

    .stack (NOLOAD) : {
		. = ALIGN(32);
		__stack_start = .;
		. = __stack_start + 0x2000;
		. = ALIGN(32);		/* make sure sp is 32B align */
		__stack_end = .;
	} > BST_S_SRAM

	/DISCARD/ : {
		*(.ARM.exidx.exit.text)
		*(.ARM.extab.exit.text)
		*(.ARM.exidx.cpuexit.text)
		*(.ARM.extab.cpuexit.text)
		*(.ARM.exidx.devexit.text)
		*(.ARM.extab.devexit.text)
		*(.ARM*)
		*(.debug*)
		*(.discard*)
        *(.eh_frame*)
	}
}

#else   /* defined(CONFIG_BST_LARGE_SIZE) */

/* CV2x/CV2xFS or CV5x+LP4 */
SECTIONS
{
	. = NAND_BST_START;

	.bst_stage1 . : {
		*AmbaBST.o (.text)
        *AmbaBST_SecureBoot.o  (.text)
		*AmbaBST_Init*.o (.text)
        *utils.o (.text*)
#if defined(CONFIG_BST_DRAM_TRAINING)
		*dram_training_lpddr4.o (.text* .rodata*)
#endif
		*AmbaBST_LoadPref* (.text)
		*AmbaBST_UserConfig* (.text)
		*AmbaBST_Memdiag* (.text)
		*AmbaBST.o (.rodata)
        *AmbaBST_SecureBoot.o  (.rodata)
		*AmbaBST_Init*.o (.rodata)
        *utils.o (.rodata)
		*AmbaBST_LoadPref* (.rodata)
		*AmbaBST_UserConfig* (.rodata)
		*AmbaBST_Memdiag* (.rodata)
		*(BstDramParam)
		*(AmbaDeviceInit)
        . += BST_ALIGN_SIZE - (. % BST_ALIGN_SIZE);
	}

	.bst_stage2 AMBOOT_BST_RAM_START : AT (ADDR(.bst_stage1) + SIZEOF(.bst_stage1)) {
		__bst_stage2_ram_start = .;
#if defined(CONFIG_ENABLE_SPINOR_BOOT)
		*AmbaBST_SpiNOR*.o (.text)
#elif defined(CONFIG_ENABLE_EMMC_BOOT)
		*AmbaBST_eMMC*.o (.text)
#elif defined(CONFIG_ENABLE_SPINAND_BOOT)
		*AmbaBST_SpiNAND*.o (.text)
#else
		*AmbaBST_NAND*.o (.text)
#endif
		*rsa*.o (.text)
		*auth*.o (.text)
		__bst_stage2_ram_end = .;
        . += BST_ALIGN_SIZE - (__bst_stage2_ram_end % BST_ALIGN_SIZE);
	}

	.data (NAND_BST_START + SIZEOF(.bst_stage1) + SIZEOF(.bst_stage2)) : AT (ADDR(.bst_stage1) + SIZEOF(.bst_stage1) + SIZEOF(.bst_stage2)) {
		__bst_param_start = .;
#if defined(CONFIG_ENABLE_SPINOR_BOOT)
		*(BstNorParam)
#elif defined(CONFIG_ENABLE_EMMC_BOOT)
		*(BstEmmcParam)
#elif defined(CONFIG_ENABLE_SPINAND_BOOT)
		*(BstSpiNandParam)
#else
		*(BstNandParam)
#endif
		__bst_param_end = .;
		. += BST_PAD_SIZE - (SIZEOF(.bst_stage1) + SIZEOF(.bst_stage2) + __bst_param_end - __bst_param_start) ;
	}

	__bst_stage2_load_start = LOADADDR(.bst_stage2);
	__bst_stage2_load_end = __bst_stage2_load_start + __bst_stage2_ram_end - __bst_stage2_ram_start;

	/DISCARD/ : {
		*(.ARM.exidx.exit.text)
		*(.ARM.extab.exit.text)
		*(.ARM.exidx.cpuexit.text)
		*(.ARM.extab.cpuexit.text)
		*(.ARM.exidx.devexit.text)
		*(.ARM.extab.devexit.text)
		*(.ARM*)
		*(.debug*)
		*(.discard*)
	}
}

#endif  /* defined(CONFIG_BST_LARGE_SIZE) */
