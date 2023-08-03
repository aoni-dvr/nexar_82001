/**
 *  @file AmbaBST.asm
 *
 *  Copyright (c) 2020 Ambarella International LP
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
 *  @details Bootstrap Code (4KB code space)
 *
 */

#include "AmbaBST.h"

        .global __vector
        .global __AmbaProgramStart
        .global __AmbaClockInit
        .global __AmbaRct2Init

#ifdef CONFIG_CHECK_TARGET_ALL
#define CONFIG_CHECK_TARGET_BST
#endif

__vector:                                           /* Make this a DATA label, so that stack usage analysis */
                                                    /* doesn't consider it an uncalled fun */

/*
 *   After a reset execution starts here, the mode is AArch64, EL3, with interrupts disabled.
 */
__AmbaProgramStart:
/*
 *  Only the CRC check is pass, then CPU jump to the first instruction of BST.
 */
#ifdef CONFIG_CHECK_TARGET_BST
        // reserved data. 4 bytes branch instruction and 28 bytes NOP.
        B      __bst_check
        NOP    // BST crc32. Will be replaced by FW programmer
        NOP
        NOP
        NOP
        NOP
        NOP
        NOP
__bst_check:
        LDR     x28, =AMBA_CORTEX_A53_FLASH_IO_BASE_ADDR // FIO start address
        LDR     w15, =0xFE0                              // size of BST (4K-32)
        LDR     w16,[X28, #4]                            // crc32 value for comparison.
        LDR     X1, =__bst_stage1_fio_end
        AND     X1, X1, #0xFFF                           //bst stage-1 size
        LDR     X0, =verify_crc32
        LDR     X2, =AMBOOT_BST_RAM_START
        SUB     X0, X0, X2                               // offset on stage 2
        ADD     X0, X0, X1                               // offset on FIO
        ORR     X0, X0, X28                              // crc32 code address on FIO
        ADD     X28, X28, #32                            // offset of 32 bytes reserved data
        BLR     X0
#endif
        /* Turn on instrucion cache and disable MMU */
        IC      IALLU                       /* I+BTB cache invalidate */
        isb
        mrs     x0, sctlr_el3
        orr     x0, x0, #0x1000
        msr     sctlr_el3, x0

        // check and save secure boot status
        BL      __AmbaSecureBootCheck

/*
 *  Initialize Boot Device
 */
        BL      __AmbaDeviceInit                    /* call __AmbaDeviceInit() */

/*
 *  Initialize DRAM Controller
 */
        BL      __AmbaDramInit                      /* call __AmbaDramInit() */

#if defined(CONFIG_BST_DRAM_TRAINING)
/*
 *  Train DRAM Controller
 */
        mov     w23, w20                           /* W20 contains RCT base after __AmbaDramInit() and we copy to W23 for RCT delay in dram_training() */
        mov     w20, w21                           /* W21 contains DDRC base after __AmbaDramInit() and we copy to W21 for DDRC access in dram_training() */
        BL      dram_training                      /* call dram_training() */
#endif

/*
 *  Initialize Chip Clock Setting
 */
        BL      __AmbaClockInit                     /* call __AmbaClockInit() */
        BL      __AmbaRct2Init

/*
 *  Copy Bootstrap Stage-2 Codes from FIO FIFO to DRAM
 */
CopyMagicCode:
        ADR     X0, __bst_stage2_load_start         /* X0 = Load Address in the FIFO */
        LDR     X1, =__bst_stage2_ram_start         /* X1 = Start Run-Address in DRAM */
        LDR     X2, =__bst_stage2_ram_end           /* X2 = End Run-Address in DRAM */

MemMove_Code:
        LDP     X3, X4, [X0], #16                   /* Load 8*2 bytes from [X0] to [X3-X4] */
        STP     X3, X4, [X1], #16                   /* Store 8*2 bytes from [X3-X4] to [X1] */
        CMP     X1, X2                              /* X1 <= X2 ? */
        B.LE    MemMove_Code                        /* if not yet finished, continue */

/*
 *  Copy Bootstrap Parameters (Magic Code, NAND and DRAM Parameters) from FIO FIFO to DRAM
 */
        /* X0 = Start Address of Parameters in FIFO */
        /* X1 = Start Address of Parameters in RAM */
        ADR     X0, AmbaWarmBootMagicCode
        LDR     X1, =AMBA_ROM_MAGIC_CODE_PHYS_ADDR
        ADR     X2, __bst_param_end                 /* X2 = End Address of Parameters in FIFO */

MemMove_Data:
        LDP     X3, X4, [X0], #16                   /* Load 8*2 bytes from [X0] to [X3-X4] */
        STP     X3, X4, [X1], #16                   /* Store 8*2 bytes from [X3-X4] to [X1] */
        CMP     X0, X2                              /* X0 <= X2 */
        B.LE    MemMove_Data                        /* if not yet finished, continue */

/*
 *  Jump to DRAM (init NAND@ Load Bootloader from NAND into DRAM@ jump to the Bootloader: _c_int00)
 */
        LDR     X0, =__AmbaBootStrapStage2
        BR      X0                                  /* Jump to Bootstrap Stage-2 start address */

__AmbaClockInit:
        LDR     X0, =(AMBA_DBG_PORT_RCT_BASE_ADDR - AMBA_CORTEX_A53_SECURE_APB_VIRT_BASE_ADDR + AMBA_CORTEX_A53_SECURE_APB_PHYS_BASE_ADDR)
        LDR     X1, =(SYS_CLOCK_CTRL_INIT_VALUE)
        STR     W1, [X0, #RCT_CLOCK_CTRL_REG]
        RET

__AmbaRct2Init:
        LDR     X0, =(AMBA_DBG_PORT_RCT_BASE_ADDR - AMBA_CORTEX_A53_SECURE_APB_VIRT_BASE_ADDR + AMBA_CORTEX_A53_SECURE_APB_PHYS_BASE_ADDR)
        MOV     W1, #1
        STR     W1, [X0, #RCT_TIMER_2_CTRL_REG]           /* Reset RCT Timer */
        MOV     W1, #0
        STR     W1, [X0, #RCT_TIMER_2_CTRL_REG]           /* Enable RCT Timer */
        RET
