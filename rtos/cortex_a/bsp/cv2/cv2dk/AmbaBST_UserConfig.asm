/**
 *  @file AmbaBST_UserConfig.asm
 *
 *  @copyright Copyright (c) 2020 Ambarella, Inc.
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
 *  @details Reserved for user-customized configuration
 *
 */

#include "AmbaCortexA53.h"

/*  These defines should be the same as defines in AmbaBLD.h */
/*  Reload Value = (TIMER_COUNT_VALUE << TIMER_COUNT_SHIFT ) - 1, Reload Value is 32 bit value */
/*  Status Value = (TIMER_COUNT_VALUE << TIMER_COUNT_SHIFT ),     Status Value is 32 bit value */

#define TIMER_STATUS_REG                         0x00
#define TIMER_RELOAD_REG                         0x04
#define TIMER_FIRSTMATCH_REG                     0x08
#define TIMER_SECONDMATCH_REG                    0x0c
#define TIMER_CTRL_REG                           0x30
#define TIMER_COUNT_VALUE                        0xFFF     /* 16 bit value */
#define TIMER_COUNT_SHIFT                        0x14      /* Value 0 ~ 31 */

#define AMBA_DRAM_PARAMETER_OFFSET_PLL          (0x0<<2)
#define AMBA_DRAM_PARAMETER_OFFSET_TIMEING1     (0x1<<2)
#define AMBA_DRAM_PARAMETER_OFFSET_TIMEING2     (0x2<<2)
#define AMBA_DRAM_PARAMETER_OFFSET_TIMEING3     (0x3<<2)
#define AMBA_DRAM_PARAMETER_OFFSET_DQWRITEDLY   (0x4<<2)
#define AMBA_DRAM_PARAMETER_OFFSET_DQCA         (0x5<<2)
#define AMBA_DRAM_PARAMETER_OFFSET_DLL0         (0x6<<2)
#define AMBA_DRAM_PARAMETER_OFFSET_DLL1         (0x7<<2)
#define AMBA_DRAM_PARAMETER_OFFSET_DLL2         (0x8<<2)
#define AMBA_DRAM_PARAMETER_OFFSET_DLL3         (0x9<<2)
#define AMBA_DRAM_PARAMETER_OFFSET_MR14         (0xA<<2)

        .globl __AmbaBstUserConfig
        .globl __AmbaTimerInit
        .globl __AmbaTimerGet

__AmbaBstUserConfig:
        /* DO NOT USE 'BL' INSTRUCTION BECAUSE THERE IS NO STACK BEFORE DRAM INIT!! */

        /* Following is muiltiple dram/nand parameter feature example code             */
        /* System will determin whether to do replacement or not at first              */
        /* In this example, the checking method is checking GPIO0 pin0                 */
        /* If pin0 == 0, do parameter repacement                                       */
        /* else, jump to DONE and return                                               */

#ifdef SUPPORT_MULTI_PARAMETER
        /* Checking GPIO0 bit0 to detemine if use new parameters                        */
        /* Config IO mux                                                                */
        MOVZ    W4, #0x0
        MOVZ    W5, #0x1
        LDR     X3, =0xe8016000                     /* IO MUX physical address          */
        STR     W4, [X3, #0x8]

        STR     W5, [X3, #0xf0]
        
        STR     W4, [X3, #0xf0]

        /* GPIO config                                                                  */
        LDR     X3, =0xe8009000                     /* GPIO0 physical address           */
        STR     W5, [X3, #0x2C]                     /* Enable = 0x1                     */
        STR     W4, [X3, #0x04]                     /* PinDirection , Input = 0x0       */
        LDR     X0, =0x00000001
        STR     W0, [X3, #0x28]                     /* PinMask = 1, set target to pin0  */
        LDR     W0, [X3]
        TST     W0, #1
        BEQ     DONE                                /* Pin0 == 0, skip                  */

        /* DRAM parameter replace                                                       */
        ADR     X0, New_PLL_DdrCtrlParam
        LDR     X1, =PLL_DdrCtrlParam
        LDP     W3, W4, [X0], #8
        STP     W3, W4, [X1], #8
        LDR     W3, [X0]
        STR     W3, [X1]

        ADR     X0, New_DLL_Setting0Param
        LDR     X1, =DLL_Setting0Param
        LDP     W3, W4, [X0], #8
        STP     W3, W4, [X1], #8
        LDP     W3, W4, [X0], #8
        STP     W3, W4, [X1], #8

        LDR     W0, New_DLL_CtrlSelMiscParam
        LDR     X1, =DLL_CtrlSelMiscParam
        STR     W0, [X0]

        ADR     X0, New_DRAM_CtrlParam
        LDR     X1, =DRAM_CtrlParam
        LDP     W3, W4, [X0], #8
        STP     W3, W4, [X1], #8
        LDP     W3, W4, [X0], #8
        STP     W3, W4, [X1], #8
        LDR     W3, [X0]
        STR     W3, [X1]

        LDR     W0, New_DRAM_PadTermParam
        LDR     X1, =DRAM_PadTermParam
        STR     W0, [X0]

        LDR     W0, New_DRAM_PadTermParam_1
        LDR     X1, =DRAM_PadTermParam_1
        STR     W0, [X0]

#if defined(CONFIG_DRAM_LPDDR3)
        ADR     X0, New_LPDDR3_ModeReg1Param
        LDR     X1, =LPDDR3_ModeReg1Param
        LDP     W3, W4, [X0], #8
        STP     W3, W4, [X1], #8
        LDP     W3, W4, [X0], #8
        STP     W3, W4, [X1], #8

        LDR     W0, New_DRAM_PadTermParam_1
        LDR     X1, =DRAM_PadTermParam_1
        STR     W0, [X0]

        LDR     W0, New_LPDDR3_ModeReg3Param_1
        LDR     X1, =LPDDR3_ModeReg3Param_1
        STR     W0, [X0]
#elif defined(CONFIG_DRAM_LPDDR4)


#elif defined(CONFIG_DRAM_DDR3)
        ADR     X0, New_LPDDR3_ModeReg1Param
        LDR     X1, =LPDDR3_ModeReg1Param
        LDP     W3, W4, [X0], #8
        STP     W3, W4, [X1], #8
        LDR     W3, [X0]
        STR     W3, [X1]

        LDR     W0, New_DDR3_ModeReg1Param_1
        LDR     X1, =DDR3_ModeReg1Param_1
        STR     W0, [X0]
#endif
        
        /* NAND parameter replacement */
#if defined(CONFIG_ENABLE_NAND_BOOT)
        ADR     X0, New_AmbaNandTiming
        LDR     X1, =AmbaNandTiming
        LDP     W3, W4, [X0], #8
        STP     W3, W4, [X1], #8
        LDP     W3, W4, [X0], #8
        STP     W3, W4, [X1], #8
        LDP     W3, W4, [X0], #8
        STP     W3, W4, [X1], #8
#endif /* #if defined(CONFIG_ENABLE_NAND_BOOT) */

#endif /* SUPPORT_MULTI_PARAMETER */

#if defined(CONFIG_DRAM_TRAINING_SUPPORT)
        LDR     X0, =0xFFFF0E00
        LDR     W1, [X0, #AMBA_DRAM_PARAMETER_OFFSET_PLL]
        CMP     W1, #0
        BEQ     DONE

        LDR     X2, =0xdffe08b8
        LDR     X3, =0x416D6261
        STR     W3, [X2]
        LDR     W2, =0x80000000
        ANDS    W3, W1, W2
        BNE     DONE
        ADR     X2, PLL_DdrCtrlParam
        STR     W1, [X2]

        LDR     W1, [X0, #AMBA_DRAM_PARAMETER_OFFSET_TIMEING1]
        ADR     X2, DRAM_Timing1Param
        STR     W1, [X2]
        LDR     W1, [X0, #AMBA_DRAM_PARAMETER_OFFSET_TIMEING2]
        ADR     X2, DRAM_Timing2Param
        STR     W1, [X2]
        LDR     W1, [X0, #AMBA_DRAM_PARAMETER_OFFSET_TIMEING3]
        ADR     X2, DRAM_Timing3Param
        STR     W1, [X2]

        LDR     W1, [X0, #AMBA_DRAM_PARAMETER_OFFSET_DQWRITEDLY]
        ADR     X2, DRAM_lpddr4DqWriteDlyParam
        STR     W1, [X2]

        LDR     W1, [X0, #AMBA_DRAM_PARAMETER_OFFSET_DQCA]
        ADR     X2, DRAM_lpddr4DqCaVrefParam
        STR     W1, [X2]

        ADR     X2, DLL_Setting0Param
        LDP     W3, W4, [X0, #AMBA_DRAM_PARAMETER_OFFSET_DLL0]
        STP     W3, W4, [X2], #8
        LDP     W3, W4, [X0, #AMBA_DRAM_PARAMETER_OFFSET_DLL2]
        STP     W3, W4, [X2], #8

        ADR     X2, LPDDR4_ModeReg14ParamFsp0
        LDR     W3, [X0, #AMBA_DRAM_PARAMETER_OFFSET_MR14]
        STR     W3, [X2]

        LDR     X0, =0xdffe08b8
        LDR     X1, =0x61626D41
        STR     W1, [X0]
#endif

        /* The functions for the timer 0 init and get */
__AmbaTimerInit:
        LDR     W0, =(AMBA_CORTEX_A53_TIMER0_BASE_ADDR - AMBA_CORTEX_A53_NONSECURE_APB_VIRT_BASE_ADDR + AMBA_CORTEX_A53_NONSECURE_APB_PHYS_BASE_ADDR)   /* W0 = AMBA_CORTEX_A53_TIMER0_BASE_ADDR  */
        MOVZ    W1, #0
        STR     W1, [X0, #TIMER_CTRL_REG]           /* Inital the timer control */
        MOVN    W1, #0x0
        STR     W1, [X0, #TIMER_STATUS_REG]         /* Inital 1st Counter Registers : status register */
        MOVZ    W1, #0
        STR     W1, [X0, #TIMER_RELOAD_REG]         /* Inital 1st Counter Registers : reload value register */
        STR     W1, [X0, #TIMER_FIRSTMATCH_REG]     /* Inital 1st Counter Registers : first match register */
        STR     W1, [X0, #TIMER_SECONDMATCH_REG]    /* Inital 1st Counter Registers : second match register */
        MOVZ    W1, #TIMER_COUNT_VALUE
        LSL     W1, W1, #TIMER_COUNT_SHIFT
        SUB     W1, W1, #1
        STR     W1, [X0, #TIMER_RELOAD_REG]         /* Set reload value register */
        MOVZ    W1, #TIMER_COUNT_VALUE
        LSL     W1, W1, #TIMER_COUNT_SHIFT
        STR     W1, [X0]                            /* Set status value register */
        LDR     W1, [X0, #TIMER_CTRL_REG]
        ORR     W1, W1, #0x1
        STR     W1, [X0, #TIMER_CTRL_REG]           /* Enable the timer */
DONE:
        RET                                         /* Return to caller */

__AmbaTimerGet:
        LDR     W0, =(AMBA_CORTEX_A53_TIMER0_BASE_ADDR - AMBA_CORTEX_A53_NONSECURE_APB_VIRT_BASE_ADDR + AMBA_CORTEX_A53_NONSECURE_APB_PHYS_BASE_ADDR)   /* W0 = AMBA_CORTEX_A53_TIMER0_BASE_ADDR  */
        LDR     W1, [X0, #TIMER_STATUS_REG]         /* W1 = The status value for timer0  */
        LDR     W2, [X0, #TIMER_RELOAD_REG]         /* W2 = The reload value for timer0 */
        RET

#ifdef SUPPORT_MULTI_PARAMETER

New_PLL_DdrCtrlParam:           .word   0x25100000
New_PLL_DdrCtrl2Param:          .word   0x3f770000
New_PLL_DdrCtrl3Param:          .word   0x00068302

New_DRAM_CtrlParam:             .word   0x00000000
New_DRAM_ConfigParam:           .word   0x202620b8
New_DRAM_Timing1Param:          .word   0x524d9099
New_DRAM_Timing2Param:          .word   0x518cbcaa
New_DRAM_Timing3Param:          .word   0x4000006f

New_DRAM_PadTermParam:          .word   0x20034
New_DRAM_PadTermParam_1:        .word   0x20034

#if defined(CONFIG_DRAM_LPDDR3)
New_LPDDR3_ModeReg1Param:       .word   0x01010003
New_LPDDR3_ModeReg2Param:       .word   0x01020006
New_LPDDR3_ModeReg3Param:       .word   0x01030000
New_LPDDR3_ModeReg11Param:      .word   0x010b0000

New_LPDDR3_ModeReg3Param_1:     .word   0x01030000
#elif defined(CONFIG_DRAM_DDR3)
New_DDR3_ModeReg1Param:         .word   0x01010003
New_DDR3_ModeReg2Param:         .word   0x01020019
New_DDR3_ModeReg3Param:         .word   0x01030001

New_DDR3_ModeReg1Param_1:       .word   0x01010000
#endif

New_DLL_Setting0Param:          .word   0x00222222
New_DLL_Setting1Param:          .word   0x00222222
New_DLL_Setting2Param:          .word   0x00222222
New_DLL_Setting3Param:          .word   0x00222222

New_DLL_CtrlSelMiscParam:       .word   0x12e50000

#if defined(CONFIG_ENABLE_NAND_BOOT)
New_AmbaNandTiming:
        .word   0x06060A06
        .word   0x03030303
        .word   0x0605320A
        .word   0x06053223
        .word   0x0A051E01
        .word   0x00323205
#endif

#endif /* SUPPORT_MULTI_PARAMETER */
