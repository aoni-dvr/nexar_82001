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

#if CONFIG_BST_LARGE_SIZE
#if defined(CONFIG_ENABLE_SECURITY) && (defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52))
        .global verify_2nd_done
#endif
#define FIFO_8K_ADDR_START             0x20e000c000
#define FIFO_8K_ADDR_END               0x20e000e000
#define COPY_SRAM_ADDR_START           0x20e0033c00

.text
.section .text.first_stage
#else
.text
#endif

__vector:                                           /* Make this a DATA label, so that stack usage analysis */
                                                    /* doesn't consider it an uncalled fun */

/*
 *   After a reset execution starts here, the mode is AArch64, EL3, with interrupts disabled.
 */
__AmbaProgramStart:
        NOP    // BST crc32. Will be replaced by FW programmer
        NOP
        NOP
        NOP
        NOP
        NOP
        NOP
        NOP    // BST crc32. Will be replaced by FW programmer
        NOP
        NOP
        NOP
        NOP
        NOP
        NOP

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
        LDR     x28, =AMBA_CORTEX_A76_FLASH_IO_BASE_ADDR // FIO start address
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

#if CONFIG_BST_LARGE_SIZE
        /* clear_bss */
        ldr     x0, =__bss_start
        ldr     x1, =__bss_end
1:      str     xzr, [x0], #8
        cmp     x0, x1
        b.lo    1b

        /* Relocate Stage-2 */
        ADR     X0, __bst_stage2_load_start         /* X0 = Load Address in the FIFO */
        LDR     X1, =__bst_stage2_ram_start         /* X1 = Start Run-Address in DRAM */
        LDR     X2, =__bst_stage2_ram_end           /* X2 = End Run-Address in DRAM */
2:
        LDP     X3, X4, [X0], #16                   /* Load 8*2 bytes from [X0] to [X3-X4] */
        STP     X3, X4, [X1], #16                   /* Store 8*2 bytes from [X3-X4] to [X1] */
        CMP     X1, X2                              /* X1 <= X2 ? */
        B.LE    2b                                  /* if not yet finished, continue */
#endif

/*
 *  Load user preferences
 */
        BL      __AmbaBstLoadPref                   /* call __AmbaBstLoadPref(), W9 keeps return status, 1:okay, 0:invalid */

#if !defined(CONFIG_WDT_PIN_NON)
        ldr    X0, =AMBA_CORTEX_A76_IO_MUX_BASE_ADDR
        mov    w1, (IOMUX_FUNC_ID & 0x1)
        lsl    w1, w1, #PIN_OFFSET
        ldr    w2, [x0, #IOMUX_REG_0]
        bic    w2, w2, w1
        orr    w1, w2, w1
        str    w1, [x0, #IOMUX_REG_0]
        mov    w1, ((IOMUX_FUNC_ID >> 1U) & 0x1)
        lsl    w1, w1, #PIN_OFFSET
        ldr    w2, [x0, #IOMUX_REG_1]
        bic    w2, w2, w1
        orr    w1, w2, w1
        str    w1, [x0, #IOMUX_REG_1]
        mov    w1, ((IOMUX_FUNC_ID >> 2U) & 0x1)
        lsl    w1, w1, #PIN_OFFSET
        ldr    w2, [x0, #IOMUX_REG_2]
        bic    w2, w2, w1
        orr    w1, w2, w1
        str    w1, [x0, #IOMUX_REG_2]
        mov    w1, 0x1
        str    w1, [x0, #0xf0]
        mov    w1, 0x0
        str    w1, [x0, #0xf0]

        ldr    X0, =GPIO_BASE_ADDR
        ldr    x1, =0x1
        str    w1, [x0, #0x2c]
        ldr    x1, =0x0
        str    w1, [x0, #0x28]

        //WDT 15ms
        ldr    X0, =0x20ed080000
        ldr    w1, [x0, #0x68]
        bic    w1, w1, #1
        str    w1, [x0, #0x68]
        LDR    X0, =AMBA_CORTEX_A76_WDT_BASE_ADDR
        ldr    x1, =0x175890    /* 15ms under 102MHz APB (1/4 Core clock)*/
        str    w1, [x0, #0x4]
        mov    w1, #0x4755
        str    w1, [x0, #0x8]
        ldr    w1, =0xfffffff
        str    w1, [x0, #0x18]
        mov    w1, #0x9
        str    w1, [x0, #0xc]
#endif

        // check and save secure boot status
        BL      __AmbaSecureBootCheck

/*
 *  Initialize Boot Device
 */
        BL      __AmbaDeviceInit                    /* call __AmbaDeviceInit() */

/*
 *  Initialize DRAM Controller
 */
        BL      __AmbaDramInit                      /* call _AmbaDramInit() */

#if !defined(CONFIG_WDT_PIN_NON)
        /* AXI clock checking code */
        mov     x19, x25                            /* x25 holds the DRAM address in __AmbaDramInit() */
        bl      axi_clk_phase_test
        /* Restore dram axi0/axi1 credit. Priority and throttle will be set in BLD. */
        mov     w1, #0x7
        str     w1, [x19, #0x10]
        str     w1, [x19, #0x14]
#endif

#if CONFIG_BST_LARGE_SIZE
        ldr     x23, =RCT_BASE
        ldr     w24, [x23, #SYS_CONFIG_OFFSET]      /* Save POC value */
        bl      spad_result_backup
        bl      copy_2nd_bst

.section .text.second_stage
start_2nd_stage:
#if defined(CONFIG_ENABLE_SECURITY) && (defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52))
        ldr     x22, =(AHB_SP0_RAM_BASE + 0x2000)
        mov     w14, #0x2000        /* It's the second 8k size of bootstrap. */
        BL      verify_signature    /* Verify the second 8k size of bootstrap by rom code and jump to the start of the second 8k size(back_2nd_stage.S). */
        B       back_2nd            /* Add this to create symbol back_2nd in map file, but it will not jump to back_2nd. */
verify_2nd_done:
#endif
        bl      spad_result_restore
        /* Use FIO FIFO as Stack */
        ldr     x0, =__stack_end
        mov     sp, x0
/*
 * Init UART for debug purpose
 */
        MOV     W19, W9                             /* Save W9 in W19 */
        BL      bst_uart_init                       /* call bst_uart_init() */

        /* Skip training if training result is available */
        MOV     W9, W19                             /* Restore W9 from W19 */
        CMP     W9, #1                              /* W9 will be set in __AmbaBstLoadPref() */
        BNE     DO_TRAINING
    #if defined(CONFIG_BST_WDELAY_UPDATE_DRIFT)
        BL      AmbaDramAdjust
        B       SKIP_TRAINING
    #elif defined(CONFIG_BST_WDELAY_UPDATE_RETRAIN)
        /* Update training_param to be train write delay only (bit[1:0] are set) */
        ADR     X1, training_param
        LDR     W2, [X1]
        BIC     W2, W2, #0x1c                       /* Clear bit[4:2] */
        ORR     W2, W2, #0x03                       /* Set bit[1:0] */
        STR     W2, [x1]
    #else
        B       SKIP_TRAINING
    #endif

DO_TRAINING:
/*
 *  Run DRAM Training
 */
    #ifdef CONFIG_BST_DRAM_TRAINING
        BL      training_time_start
        BL      dram_training                       /* call dram_training() */
        BL      training_time_show
    #endif
SKIP_TRAINING:
#endif  /* CONFIG_BST_LARGE_SIZE */

/*
 *  Initialize Chip Clock Setting
 */
        BL      __AmbaClockInit                     /* call __AmbaClockInit() */
        BL      __AmbaRct2Init

#if !CONFIG_BST_LARGE_SIZE
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
#endif
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
        LDR     X0, =(AMBA_DBG_PORT_RCT_BASE_ADDR - AMBA_CORTEX_A76_APB_VIRT_BASE_ADDR + AMBA_CORTEX_A76_APB_PHYS_BASE_ADDR)
        LDR     X1, =(SYS_CLOCK_CTRL_INIT_VALUE)
        STR     W1, [X0, #RCT_CLOCK_CTRL_REG]
        RET

__AmbaRct2Init:
        LDR     X0, =(AMBA_DBG_PORT_RCT_BASE_ADDR - AMBA_CORTEX_A76_APB_VIRT_BASE_ADDR + AMBA_CORTEX_A76_APB_PHYS_BASE_ADDR)
        MOV     W1, #1
        STR     W1, [X0, #RCT_TIMER_2_CTRL_REG]           /* Reset RCT Timer */
        MOV     W1, #0
        STR     W1, [X0, #RCT_TIMER_2_CTRL_REG]           /* Enable RCT Timer */
        RET
