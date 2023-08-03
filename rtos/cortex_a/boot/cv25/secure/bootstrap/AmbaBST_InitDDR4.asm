/**
 *  @file AmbaBST_InitDDR4.asm
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
 *  @details DDR Controller initializations for DDR4
 *
 */

#include "AmbaBST.h"

//#define ENABLE_DRAM_SCRAMBLING_FUNC 1

/*
 *  DDRC Initializtions
 */
        .global __AmbaDramInit

__AmbaDramInit:
        MOV     X29, X30                                /* backup link register (X30) to X29 */

        /* X20 = Base Address of RCT registers */
        LDR     W20, =(AMBA_DBG_PORT_RCT_BASE_ADDR - AMBA_CORTEX_A53_SECURE_APB_VIRT_BASE_ADDR + AMBA_CORTEX_A53_SECURE_APB_PHYS_BASE_ADDR)
        /* X21 = Base Address of DDRC registers */
        LDR     W21, =(AMBA_CORTEX_A53_DDR_CTRL0_BASE_ADDR - AMBA_CORTEX_A53_DRAM_CONFIG_VIRT_BASE_ADDR + AMBA_CORTEX_A53_DRAM_CONFIG_PHYS_BASE_ADDR)
        /* X21 = Base Address of DDRC registers */
        LDR     W22, =(AMBA_CORTEX_A53_DRAM_CTRL_BASE_ADDR - AMBA_CORTEX_A53_DRAM_CONFIG_VIRT_BASE_ADDR + AMBA_CORTEX_A53_DRAM_CONFIG_PHYS_BASE_ADDR)

/*
 *  setup DDR PLL, X20 = Base Address of RCT
 */
        ADR     X0, PLL_DdrCtrlParam                    /* X0 = Base Address of PLL_DdrCtrlParam */
        LDP     W1, W2, [X0], #8                        /* Load 2 words from X0 to W1 and W2 */
        LDR     W3, [X0]                                /* Load 1 word from X0 to W3 */
        STR     W1, [X20, #PLL_DDR_PLL_CTRL_REG]        /* DDR PLL Control Register Value = W1 */
        STR     W2, [X20, #PLL_DDR_PLL_CTRL2_REG]       /* DDR PLL Control-2 Register Value = W2 */
        STR     W3, [X20, #PLL_DDR_PLL_CTRL3_REG]       /* DDR PLL Control-3 Register Value = W3 */
        ADD     X17, X20, #PLL_DDR_PLL_CTRL_REG         /* X17 = DDR PLL Control Register */
        BL      PllCtrlReg_Enable

/*
 *  setup Core PLL, X20 = Base Address of RCT
 */
        ADR     X0, PLL_CoreCtrlParam
        LDP     W1, W2, [X0], #8                        /* Load 2 words from X0 to W1 and W2 */
        LDR     W3, [X0]                                /* Load 1 word from X0 to W3 */
        STR     W1, [X20, #PLL_CORE_PLL_CTRL_REG]       /* Core PLL Control Register Value = W1 */
        STR     W2, [X20, #PLL_CORE_PLL_CTRL2_REG]      /* Core PLL Control-2 Register Value = W2 */
        STR     W3, [X20, #PLL_CORE_PLL_CTRL3_REG]      /* Core PLL Control-3 Register Value = W3 */
        ADD     X17, X20, #PLL_CORE_PLL_CTRL_REG        /* X17 = Core PLL Control Register */
        BL      PllCtrlReg_Enable

/*
 *  setup Cortex PLL, X20 = Base Address of RCT
 */
        ADR     X0, PLL_CortexCtrlParam                 /* X0 = Base Address of PLL_CortexCtrlParam */
        LDP     W1, W2, [X0], #8                        /* Load 2 words from X0 to W1 and W2 */
        LDR     W3, [X0]                                /* Load 1 word from X0 to W3 */
        STR     W1, [X20, #PLL_CORTEX_PLL_CTRL_REG]     /* Cortex PLL Control Register Value = W1 */
        STR     W2, [X20, #PLL_CORTEX_PLL_CTRL2_REG]    /* Cortex PLL Control-2 Register Value = W2 */
        STR     W3, [X20, #PLL_CORTEX_PLL_CTRL3_REG]    /* Cortex PLL Control-3 Register Value = W3 */
        ADD     X17, X20, #PLL_CORTEX_PLL_CTRL_REG      /* X17 = Cortex PLL Control Register */
        BL      PllCtrlReg_Enable

/*
 *  setup DLL Master and Slave setting adjustments
 */
        ADR     X0, DLL_Setting0Param                   /* X0 = Base Address of DLL Setting Registers */
        LDP     W1, W2, [X0], #8                        /* Load 2 words from X0 to W1 and W2 */
        LDP     W3, W4, [X0], #8                        /* Load 2 words from X0 to W3 and W4 */

        STR     W1, [X21, #DDRC_DLL_SETTING_0_REG]      /* DDRC DLL Setting-0 Register = W1 */
        STR     W2, [X21, #DDRC_DLL_SETTING_1_REG]      /* DDRC DLL Setting-1 Register = W2 */
        STR     W3, [X21, #DDRC_DLL_SETTING_2_REG]      /* DDRC DLL Setting-2 Register = W3 */
        STR     W4, [X21, #DDRC_DLL_SETTING_3_REG]      /* DDRC DLL Setting-3 Register = W4 */
/*
 *  setup DLL Control Selection Registers, X0 = Base Address of DLL Control Select Registers
 */
        LDP     W1, W2, [X0], #8                        /* Load 2 words from X0 to W1 and W2 */
        LDP     W3, W4, [X0], #8                        /* Load 2 words from X0 to W3 and W4 */
        LDR     W5, [X0]                                /* Load 1 word from X0 to W5 */

        STR     W1, [X21, #DDRC_DLL_CTRL_SEL_0_REG]     /* DDRC DLL Control Select-0 Register = W1 */
        STR     W2, [X21, #DDRC_DLL_CTRL_SEL_1_REG]     /* DDRC DLL Control Select-1 Register = W2 */
        STR     W3, [X21, #DDRC_DLL_CTRL_SEL_2_REG]     /* DDRC DLL Control Select-2 Register = W3 */
        STR     W4, [X21, #DDRC_DLL_CTRL_SEL_3_REG]     /* DDRC DLL Control Select-3 Register = W4 */

        STR     W5, [X21, #DDRC_DLL_CTRL_SEL_MISC_REG]  /* DDRC DLL Control Select-Misc Register = W5 */

        /**
         *  Wait for a while
         */
        BL      DllProgram_Wait

        ADR     X0, DRAMC_ModeParam
        LDR     W1, [X0]
        STR     W1, [X22]

        /* Wait for dll lock, need at least 300000 ps delay */
        MOVZ    W19, #24                                 /* W19 = 1 us */
        BL      RctTimer_Wait

/*
 *  Delay access to DRAM controller - on cold-reset, there may be
 *  POR-induced di/dt - some delay here will avoid that state
 */
        ADR     X0, DRAM_PorDelay
        LDR     W19, [X0]                               /* W19 = Power On Reset Delay for DDRC */
        BL      RctTimer_Wait

/*
 *   DDR Controller Initializations
 */
        LDR     W1, =AMBA_RAM_DDR_RTT_CALIBRATION_OFFSET
        STR     W1, [X21, #DDRC_RSVD_SPACE_REG]         /* DRAM0 RTT Address = AMBA_RAM_DDR_RTT_CALIBRATION_OFFSET */

        ADR     X0, DRAM_CtrlParam
        LDR     W6, [X0]                                /* W6 = Ddrc Reset Value */
        BIC     W6, W6, #DDRC_CTRL_MASK                 /* Clear DRAM_CtrlParam bit[6:0] */
        STR     W6, [X21, #DDRC_CTRL_REG]

        /* Wait for clean clock 200us */
        MOVZ    W19, #4824                              /* W19 = 201 us */
        BL      RctTimer_Wait

        /* disable DQS Sync initialize */
        ADR     X0, DRAM_DqsSyncPreRttParam
        LDR     W1, [X0]
        STR     W1, [X21, #DDRC_DQS_SYNC_REG]

        /* write to DDRC_CONFIG_REG */
        ADR     X0, DRAM_ConfigParam
        LDR     W1, [X0]
        STR     W1, [X21, #DDRC_CONFIG_REG]

/*
 *  Write DRAM_TIMING1/DRAM_TIMING2/DRAM_TIMING3
 */
        ADR     X0, DRAM_Timing1Param                   /* X0 = Base Address of DRAM_Timing1Param */
        LDP     W1, W2, [X0], #8                        /* Load 2 words from X0 to W1 and W2 */
        LDP     W3, W4, [X0], #8                        /* Load 2 words from X0 to W3 and W4 */
        STR     W1, [X21, #DDRC_TIMING1_REG]            /* W1 = the value of DRAM_Timing1Param */
        STR     W2, [X21, #DDRC_TIMING2_REG]            /* W2 = the value of DRAM_Timing2Param */
        STR     W3, [X21, #DDRC_TIMING3_REG]            /* W3 = the value of DRAM_Timing3Param */
        STR     W4, [X21, #DDRC_TIMING4_REG]            /* W4 = the value of DRAM_Timing4Param */

        /* write to DDRC TIMING5 */
        ADR     X0, DRAM_Timing5Lp4TrainingParam
        LDR     W1, [X0]
        STR     W1, [X21, #DDRC_TIMING5_LP4TRAIN_REG]

        /* set PAD termination impedance and driving strength */
        ADR     X0, DRAM_PadTermParam                   /* Pad Termination parameter for DDRC */
        LDR     W1, [X0]
        STR     W1, [X21, #DDRC_PAD_TERM_REG]

        /* set PAD termination2 impedance and driving strength */
        ADR     X0, DRAM_PadTerm2Param
        LDR     W1, [X0]
        STR     W1, [X21, #DDRC_PAD_TERM2_REG]

        /* set DRAM_VREF (PAD term3)*/
        ADR     X0, DRAM_PadTerm3Param
        LDR     W1, [X0]
        STR     W1, [X21, #DDRC_PAD_TERM3_REG]

        /* Arrange the order of DQ byte-0/1/2/3 */
        ADR     X2, DRAM_ByteMapParam                   /* Byte Map parameter for DDRC-0 */
        LDR     W1, [X2]
        STR     W1, [X21, #DDRC_BYTE_MAP_REG]

        /* wait 10 ns to allow RESET/CKE PAD to start driving after DDS is set to non-0 value */
        MOVZ    W19, #24                                /* W19 = 1 us */
        BL      RctTimer_Wait

        ORR     W1, W6, #DDRC_CTRL_RESET
        STR     W1, [X21, #DDRC_CTRL_REG]

        /* After RESET_n is de-asserted, wait for another 500us until CKE becomes active */
        MOVZ    W19, #12024                             /* W19 = (500+ 1) us */
        BL      RctTimer_Wait

        /* Assert CKE */
        ORR     W1, W6, #(DDRC_CTRL_CKE | DDRC_CTRL_RESET)
        STR     W1, [X21, #DDRC_CTRL_REG]

        /* wait 400 ns */
        MOVZ    W19, #24                                /* W19 = (1) us */
        BL      RctTimer_Wait

/*
 *  JEDEC: RESET Command & Initialization Sequence After RESET
 */
        /* Issue MR3 command */
        ADR     X0, DDR4_ModeReg3Param
        LDR     W1, [X0]
        STR     W1, [X21, #DDRC_MODE_REG]
        BL      DramModeReg_Wait

        /* Issue MR6 command */
        ADR     X0, DDR4_ModeReg6Param
        LDR     W1, [X0]
        STR     W1, [X21, #DDRC_MODE_REG]
        BL      DramModeReg_Wait

        /* Issue MR5 command */
        ADR     X0, DDR4_ModeReg5Param
        LDR     W1, [X0]
        STR     W1, [X21, #DDRC_MODE_REG]
        BL      DramModeReg_Wait

        /* Issue MR4 command */
        ADR     X0, DDR4_ModeReg4Param
        LDR     W1, [X0]
        STR     W1, [X21, #DDRC_MODE_REG]
        BL      DramModeReg_Wait

        /* Issue MR2 command */
        ADR     X0, DDR4_ModeReg2Param
        LDR     W1, [X0]
        STR     W1, [X21, #DDRC_MODE_REG]
        BL      DramModeReg_Wait

        /* Issue MR1 command */
        ADR     X0, DDR4_ModeReg1Param
        LDR     W1, [X0]
        STR     W1, [X21, #DDRC_MODE_REG]
        BL      DramModeReg_Wait

        /* Issue MR0 command */
        ADR     X0, DDR4_ModeReg0Param
        LDR     W1, [X0]
        STR     W1, [X21, #DDRC_MODE_REG]
        BL      DramModeReg_Wait

        /* Issue DLL reset via MR0 command */
        ADR     X0, DDR4_ModeReg0Param_DLL_Rst
        LDR     W1, [X0]
        STR     W1, [X21, #DDRC_MODE_REG]
        BL      DramModeReg_Wait

        /* issue Manual DDR ZQ CALIB command */
        MOV     W2, #DDRC_INIT_CTRL_ZQ_CLIB             /* enable DRAM ZQ calibration */
        STR     W2, [X21, #DDRC_INIT_CTRL_REG]
        BL      DramInitCtrlReg_Wait

        /* issue DLL Rst command */
        MOV     W2, #DDRC_INIT_CTRL_DLL_RESET
        STR     W2, [X21, #DDRC_INIT_CTRL_REG]
        BL      DramInitCtrlReg_Wait

        /* set PAD_CLB_EN bit */
        MOVZ    W2, #DDRC_INIT_CTRL_SHORT_PAD_CLIB
        STR     W2, [X21, #DDRC_INIT_CTRL_REG]
        BL      DramInitCtrlReg_Wait

        /* set GET_RTT_EN bit */
        MOVZ    W2, #DDRC_INIT_CTRL_GET_RTT             /* enable get round-trip-time */
        STR     W2, [X21, #DDRC_INIT_CTRL_REG]
        BL      DramInitCtrlReg_Wait

        /* write DRAM_DQS_SYNC config */
        ADR     X0, DRAM_DqsSyncParam
        LDR     W1, [X0]
        STR     W1, [X21, #DDRC_DQS_SYNC_REG]

        /* write  DRAM_ZQ_CALIB config */
        ADR     X0, DRAM_ZqCalibParam
        LDR     W1, [X0]
        STR     W1, [X21, #DDRC_ZQ_CALIB_REG]

        /* disable DLL calibration */
        ADR     X0, DRAM_DllCalibParam
        LDR     W1, [X0]
        STR     W1, [X21, #DDRC_DLL_CALIB_REG]

        /* Enable Auto Refresh */
        MOVZ    W1, #( DDRC_CTRL_AUTO_REF_EN | DDRC_CTRL_CKE | DDRC_CTRL_RESET )
        ORR     W1, W6, W1
        STR     W1, [X21, #DDRC_CTRL_REG]

        /* set SR reg (auto active delay) */
        MOV     W1, #0x1
        STR     W1, [X21, #DDRC_SELF_REFRESH_REG]

        /* set power down mode */
        ADR     X0, DRAM_PowerDownParam
        LDR     W1, [X0]
        STR     W1, [X21, #DDRC_POWER_DOWN_CTRL_REG]

        /* set DRAM_ENABLE bit */
        MOVZ    W1, #( DDRC_CTRL_AUTO_REF_EN | DDRC_CTRL_CKE | DDRC_CTRL_RESET | DDRC_CTRL_ENABLE )
        ORR     W1, W6, W1
        STR     W1, [X21, #DDRC_CTRL_REG]

/*
 *  DRAM INITIALIZATION done, Return to the Caller
 */
#ifdef ENABLE_DRAM_SCRAMBLING_FUNC
        /* Enable dram scrambler feature */
        MOVZ    W1, #0x2
        STR     W1, [x22, #0x218]
#endif
        RET     X29                                     /* Return to caller */

/*
 *  DRAM mode register waiting for idle Subroutine
 */
DramModeReg_Wait:
        LDR     W1, [X21, #DDRC_MODE_REG]
        TBNZ    W1, #31, DramModeReg_Wait               /* Loop until busy flag is clear */
        RET                                             /* Return to caller */

/*
 *  DRAM lpddr4 ctrl register waiting for training mode ready (check DDRC_TIMING5_LP4TRAIN_REG[25])
 */
Lpddr4CtrlReg_Wait:
        LDR     W1, [X21, #DDRC_TIMING5_LP4TRAIN_REG]
        TBZ     W1, #25, Lpddr4CtrlReg_Wait             /* Loop until busy flag is clear */
        RET                                             /* Return to caller */

UinstBusyCheck:
        LDR     W1, [X21, #DDRC_UINST_BUSY_REG]
        ANDS    W1, W1,#0x1
        BNE     UinstBusyCheck
        RET

/*
 *  DRAM init control register waiting for idle Subroutine,
 *  W2 = DDRC_INIT_CTRL_ZQ_CLIB or
 *       DDRC_INIT_CTRL_DLL_RESET or
 *       DDRC_INIT_CTRL_PAD_CLIB or
 *       DDRC_INIT_CTRL_GET_RTT
 */
DramInitCtrlReg_Wait:
        LDR     W1, [X21, #DDRC_INIT_CTRL_REG]
        ANDS    W1, W1, W2
        B.NE    DramInitCtrlReg_Wait
        RET                                             /* Return to caller */

/*
 *  Toggle PLL control register bit-0 (Write Enable) Subroutine
 */
PllCtrlReg_Enable:
        ORR     W1, W1, #1                              /* set bit-0: Write Enable = 1 */
        STR     W1, [X17]                               /* PLL Control Register Value = W1 */
        EOR     W1, W1, #1                              /* clear bit-0: Write Enable = 0 */
        STR     W1, [X17]                               /* PLL Control Register Value = W1 */

DllProgram_Wait:
        MOV     W19, #120                               /* W19 = 5us */

/*
 *  RTC Timer waiting Subroutine, default CLK_REF (Clock Frequency) = 24MHz
 */
RctTimer_Wait:
        MOVZ    W0, #1                                  /* W0 = 1 */
        STR     W0, [X20, #RCT_TIMER_CTRL_REG]          /* Reset RCT Timer */
        MOVZ    W0, #0                                  /* W0 = 0 */
        STR     W0, [X20, #RCT_TIMER_CTRL_REG]          /* Enable RCT Timer */

/*
 *  Fix RCT timer metastability issue : freeze RCT timer before fetch RCT Timer Counter.
 */

RctTimer_Wait_L1:
        MOVZ    W0, #2                                  /* W0 = 2 */
        STR     W0, [X20, #RCT_TIMER_CTRL_REG]          /* Freeze RCT Timer */
        LDR     W18, [X20, #RCT_TIMER_COUNT_REG]        /* W18 = Current RCT Timer Counter */
        MOVZ    W0, #0                                  /* W0 = 0 */
        STR     W0, [X20, #RCT_TIMER_CTRL_REG]          /* Enable RCT Timer */
        CMP     W19, W18                                /* W19 > W18 */
        B.HI    RctTimer_Wait_L1                        /* yes, loop */

        RET                                             /* Return to caller */

