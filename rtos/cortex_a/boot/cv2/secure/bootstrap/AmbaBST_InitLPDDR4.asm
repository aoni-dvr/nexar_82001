/**
 *  @file AmbaBST_InitLPDDR4.asm
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
 *  @details DDR Controller initializations for LPDDR4
 *
 */

#include "AmbaBST.h"
#include <bst_regs.h>

//#define ENABLE_DRAM_SCRAMBLING_FUNC

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
        /* X22 = Base Address of DDRC registers */
        LDR     W22, =(AMBA_CORTEX_A53_DDR_CTRL1_BASE_ADDR - AMBA_CORTEX_A53_DRAM_CONFIG_VIRT_BASE_ADDR + AMBA_CORTEX_A53_DRAM_CONFIG_PHYS_BASE_ADDR)
        /* X23 = Base Address of DRAM registers */
        LDR     W23, =(AMBA_CORTEX_A53_DRAM_CTRL_BASE_ADDR - AMBA_CORTEX_A53_DRAM_CONFIG_VIRT_BASE_ADDR + AMBA_CORTEX_A53_DRAM_CONFIG_PHYS_BASE_ADDR)
        /* X25 = Base Address of both DDRC0&1 registers */
        LDR     W25, =(AMBA_CORTEX_A53_DDR_CTRL_ALL_BASE_ADDR - AMBA_CORTEX_A53_DRAM_CONFIG_VIRT_BASE_ADDR + AMBA_CORTEX_A53_DRAM_CONFIG_PHYS_BASE_ADDR)

        LDR     W1, [X20, #PLL_SYS_CONFIG_REG]
        BIC     W1, W1, #PLL_SYS_CONFIG_POC30_DISABLE_MASK    /* set poc[30] = 0 */
        STR     W1, [X20, #PLL_SYS_CONFIG_REG]

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
#if 0
        ADR     X0, PLL_CoreCtrlParam
        LDP     W1, W2, [X0], #8                        /* Load 2 words from X0 to W1 and W2 */
        LDR     W3, [X0]                                /* Load 1 word from X0 to W3 */
        STR     W1, [X20, #PLL_CORE_PLL_CTRL_REG]       /* Core PLL Control Register Value = W1 */
        STR     W2, [X20, #PLL_CORE_PLL_CTRL2_REG]      /* Core PLL Control-2 Register Value = W2 */
        STR     W3, [X20, #PLL_CORE_PLL_CTRL3_REG]      /* Core PLL Control-3 Register Value = W3 */
        ADD     X17, X20, #PLL_CORE_PLL_CTRL_REG        /* X17 = Core PLL Control Register */
        BL      PllCtrlReg_Enable
#endif

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

        MOV     X24, X21                                /* Round 1 for DDRC-0, Round 2 for DDRC-1 */
        MOV     W7, #0                                  /* Parameter offset to load */
DdrcConfigLoop:

/*
 *  setup DLL Master and Slave setting adjustments
 */
        ADR     X0, Ddrc0_DLL_Setting0Param             /* X0 = Base Address of DLL Setting Registers */
        ADD     X0, X0, X7, LSL #2
        LDP     W1, W2, [X0], #8                        /* Load 2 words from X0 to W1 and W2 */
        LDP     W3, W4, [X0], #8                        /* Load 2 words from X0 to W3 and W4 */

        ADD     X5, X24, #DDRC_DLL_SETTING_0_REG        /* DDRC DLL Setting-0~3 Register = W1~W4 */
        BL      DRAMSeqReg_Set

/*
 *  setup DLL Control Selection Registers, X0 = Base Address of DLL Control Select Registers
 */
        ADR     X0, DLL_CtrlSel0Param                    /* X0 = Base Address of DLL Sync Ctrl Registers */
        LDP     W1, W2, [X0], #8                        /* Load 2 words from X0 to W1 and W2 */
        LDP     W3, W4, [X0], #8                        /* Load 2 words from X0 to W3 and W4 */
        LDR     W5, [X0]

        STR     W5, [X24, #DDRC_DLL_CTRL_SEL_MISC_REG]  /* DDRC DLL Control Select-Misc Register = W5 */

        ADD     X5, X24, #DDRC_DLL_CTRL_SEL_0_REG       /* DDRC DLL Control Select-0~3 Register = W1 */
        BL      DRAMSeqReg_Set

        ADR     X0, DLL_CtrlSyncTrackParam              /* X0 = Base Address of DLL Sync Ctrl Sel Registers */
        LDR     W1, [X0]

        ADD     X5, X24, #DDRC_DLL_SYNC_CTRL_SEL_0_REG       /* DDRC DLL Sync Control Select-0~3 Register = W1 */
        BL      DRAMSeqReg_SetSame

        /*-----------------------------------------*\
         *  Wait for a while
        \*-----------------------------------------*/
        BL      DllProgram_Wait

        ADR     X0, DRAMC_ModeParam
        LDR     W1, [X0]
        ORR     W1, W1, #1                /* Set ddrc host number as 2 */
        STR     W1, [X23]

        /* Wait for pll lock, need at least 40 us delay */
        MOVZ    W19, #960                               /* W19 = 40 us */
        BL      RctTimer_Wait

#if 0
/*
 *  Delay access to DRAM controller - on cold-reset, there may be
 *  POR-induced di/dt - some delay here will avoid that state
 */
        ADR     X0, DRAM_PorDelay                       /* DRAM_PorDelay:                  .word   0x2710 */
        LDR     W19, [X0]                               /* W19 = Power On Reset Delay for DDRC */
        BL      RctTimer_Wait
#endif

/*
 *   DDR Controller Initializations
 */
        LDR     W1, =AMBA_RAM_DDR_RTT_CALIBRATION_OFFSET
        STR     W1, [X24, #DDRC_RSVD_SPACE_REG]         /* DRAM0 RTT Address = AMBA_RAM_DDR_RTT_CALIBRATION_OFFSET */

        /* set DDRC_LPDDR4_DQ_CA_VREF */
        ADR     X0, Ddrc0_DRAM_lpddr4DqCaVrefParam
        LDR     W1, [X0, X7]
        STR     W1, [X24, #DDRC_LPDDR4_DQ_CA_VREF_REG]

        ADR     X0, DRAM_CtrlParam
        LDR     W6, [X0]                                /* W6 = Ddrc Reset Value */
        BIC     W6, W6, #DDRC_CTRL_MASK                 /* Clear DRAM_CtrlParam bit[6:0] */
        STR     W6, [X24, #DDRC_CTRL_REG]

        /* Do DLL reset */
        MOVZ    W2, #DDRC_INIT_CTRL_DLL_RESET           /* enable DLL reset */
        BL      DramInitCtrlReg_Set

        /* disable DQS Sync initialize */
        ADR     X0, DRAM_DqsSyncPreRttParam
        LDR     W1, [X0]
        STR     W1, [X24, #DDRC_DQS_SYNC_REG]

        /* write to DDRC_CONFIG_REG (disable sync and zq_clbr) */
        ADR     X0, DRAM_ConfigParam
        LDR     W1, [X0]
        STR     W1, [X24, #DDRC_CONFIG_REG]

/*
 *  Write DRAM_TIMING1/DRAM_TIMING2/DRAM_TIMING3
 */
        ADR     X0, DRAM_Timing1Param                   /* X0 = Base Address of DRAM_Timing1Param */
        LDP     W1, W2, [X0], #8
        LDP     W3, W4, [X0], #8
        ADD     X5, X24, #DDRC_TIMING1_REG              /* DDRC DRAM_Timing-1~4 Register = W1~W4 */
        BL      DRAMSeqReg_Set

        /* write to DDRC TIMING5 */
        ADR     X0, DRAM_Timing5Lp4TrainingParam
        LDR     W1, [X0]
        STR     W1, [X24, #DDRC_TIMING5_LP4TRAIN_REG]

        /* set Ddrc0 PAD termination Values */
        ADR     X0, Ddrc0_DRAM_PadTermParam             /* Pad Termination parameter for DDRC */
        ADD     X0, X0, X7, LSL #1
        ADD     X0, X0, X7
        LDP     W1, W2, [X0], #8
        LDR     W3, [X0]
        STR     W1, [X24, #DDRC_PAD_TERM_REG]
        STR     W2, [X24, #DDRC_PAD_TERM2_REG]
        STR     W3, [X24, #DDRC_PAD_TERM3_REG]

        /* Arrange the order of DQ byte-0/1/2/3 */
        ADR     X0, DRAM_ByteMapParam                   /* Byte Map parameter for DDRC-0 */
        LDR     W1, [X0]
        STR     W1, [X24, #DDRC_BYTE_MAP_REG]

        /* set LPDDR4_DQ_READ_DLY */
        ADR     X0, Ddrc0_DRAM_lpddr4DqReadDlyParam
        LDR     W1, [X0, X7]
        STR     W1, [X24, #DDRC_LPDDR4_DQ_READ_DLY_REG]

        /* set LPDDR4_DQ_WRITE_DLY */
        ADR     X0, Ddrc0_DRAM_lpddr4DqWriteDlyParam
        LDR     W1, [X0, X7]
        STR     W1, [X24, #DDRC_LPDDR4_DQ_WRITE_DLY_REG]

#if 0
        /* set LPDDR4_DQS_WRITE_DLY */
        ADR     X0, Ddrc0_DRAM_lpddr4DqsWriteDlyParam   /* Ddrc0_DRAM_lpddr4DqsWriteDlyParam:    .word   0x00000000 */
        LDR     W1, [X0, X7]
        STR     W1, [X24, #DDRC_LPDDR4_DQS_WRITE_DLY_REG]
#endif

        /* write DRAM_ZQ_CALIB config to set T_ZQC_SEL_POR_MDDR4 for DDR ZQ Calibration */
        ADR     X0, DRAM_ZqCalibParam
        LDR     W1, [X0]
        STR     W1, [X24, #DDRC_ZQ_CALIB_REG]

        /* write to LPDDR4_TRAIN_MPC_RDLY */
        ADR     X0, Ddrc0_DRAM_lpddr4TrainMpcRdlyParam
        LDR     W1, [X0, X7]
        STR     W1, [X24, #DDRC_LPDDR4_TRAIN_MPC_RDLY_REG]

        /* wait 10 ns to allow RESET/CKE PAD to start driving after DDS is set to non-0 value */
        MOVZ    W19, #24                             	/* W19 = (1) us */
        BL      RctTimer_Wait

        ORR     W1, W6, #DDRC_CTRL_RESET
        STR     W1, [X24, #DDRC_CTRL_REG]

        /* tINIT3: idle time after before activating CKE  (at least 2000 us) */
        MOVZ    W19, #48024                             /* W19 = (2000+ 1) us */
        BL      RctTimer_Wait

        /* Assert CKE */
        ORR     W6, W6, #(DDRC_CTRL_CKE | DDRC_CTRL_RESET)
        STR     W6, [X24, #DDRC_CTRL_REG]

        /* tINIT5: idle time after first CKE assertion (at least 2 us) */
        MOVZ    W19, #72                                /* W19 = (2 + 1) us */
        BL      RctTimer_Wait


/*
 *  JEDEC: RESET Command & Initialization Sequence After RESET
 */
DRAM_MR_SETUP:
        /* Program MR13 -- set FS_WR to 0 (Frequency Set Point Write Enable) */
        ADR     X0, LPDDR4_ModeReg13ParamFsp0
        LDR     W1, [X0]
        BL      DramModeReg_Set

        MOV     W3, #DDRC_MODE_REG13_FS_WR_EN
DRAM_MR_LOOP:
        /* Issue MR3 command */
        ADR     X0, Ddrc0_LPDDR4_ModeReg3ParamFsp0
        LDR     W1, [X0, X7]
        BL      DramModeReg_Set

        /* Issue MR2 command */
        ADR     X0, LPDDR4_ModeReg2ParamFsp0
        LDR     W1, [X0]
        BL      DramModeReg_Set

        /* Issue MR1 command */
        ADR     X0, LPDDR4_ModeReg1ParamFsp0
        LDR     W1, [X0]
        BL      DramModeReg_Set

        /* Issue MR11 command (config ODT) */
        ADR     X0, Ddrc0_LPDDR4_ModeReg11ParamFsp0
        LDR     W1, [X0, X7]
        BL      DramModeReg_Set

        /* Issue MR14  */
        ADR     X0, Ddrc0_LPDDR4_ModeReg14ParamFsp0
        LDR     W1, [X0, X7]
        BL      DramModeReg_Set

        /* Issue MR12 command*/
        ADR     X0, Ddrc0_LPDDR4_ModeReg12ParamFsp0
        LDR     W1, [X0, X7]
        BL      DramModeReg_Set

        /* Issue MR22 command*/
        ADR     X0, Ddrc0_LPDDR4_ModeReg22ParamFsp0
        LDR     W1, [X0, X7]
        BL      DramModeReg_Set

        /* Program MR13 -- set FS_WR for next round or back to 0 */
        ADR     X0, LPDDR4_ModeReg13ParamFsp0
        LDR     W1, [X0]
        ORR     W1, W1, W3
        BL      DramModeReg_Set

        CBZ     W3, ZQ_CALIB_START
        MOV     W3, #0
        B       DRAM_MR_LOOP


ZQ_CALIB_START:
/*
 *  JEDEC: ZQ Calibration for each die
 */
        /* Set CS_CNTRL to Select die 0 only */
        BIC     W1, W6, #DDRC_CTRL_DIE1_DISABLE_MASK    /* disable die1 */

ZQ_CALIB_LOOP:
        STR     W1, [X24, #DDRC_CTRL_REG]

        /* issue Manual DDR ZQ CALIB command */
        MOV     W2, #DDRC_INIT_CTRL_ZQ_CLIB             /* enable DRAM ZQ calibration */
        BL      DramInitCtrlReg_Set

        /* tZQLAT: idle time (at least 30 ns) */
        MOVZ    W19, #24                                /* W19 = 1 us */
        BL      RctTimer_Wait

        ADD     W3, W3, #1                              /* W3 is 0 by end of DRAM_MR_LOOP */
        /* Set CS_CNTRL to Select die 1 only */
        BIC     W1, W6, #DDRC_CTRL_DIE0_DISABLE_MASK    /* disable die0 */
        TBZ     W3, #1, ZQ_CALIB_LOOP                   /* W3 is 1(bit[1]=0) in 1st loop and 2(bit[1]=1) in the 2nd loop */

ZQ_CALIB_DONE:
        /* Set CS_MODE back to default */
        STR     W6, [X24, #DDRC_CTRL_REG]

/*
 *  JEDEC: Initialization sequence is complete, the device is ready for any valid command.
 */
        /* set PAD_CLB_EN bit */
        MOVZ    W2, #DDRC_INIT_CTRL_PAD_CLIB            /* enable pad ZQ calibration */
        BL      DramInitCtrlReg_Set

        /* set GET_RTT_EN bit */
        MOVZ    W2, #DDRC_INIT_CTRL_GET_RTT             /* enable get round-trip-time */
        BL      DramInitCtrlReg_Set

        /* write DRAM_DQS_SYNC config */
        ADR     X0, DRAM_DqsSyncParam
        LDR     W1, [X0]
        STR     W1, [X24, #DDRC_DQS_SYNC_REG]

        /* write  DRAM_ZQ_CALIB config */
        ADR     X0, DRAM_ZqCalibParam
        LDR     W1, [X0]
        STR     W1, [X24, #DDRC_ZQ_CALIB_REG]

        /* disable DLL calibration */
        ADR     X0, DRAM_DllCalibParam
        LDR     W1, [X0]
        STR     W1, [X24, #DDRC_DLL_CALIB_REG]

        /* Enable Auto Refresh */
        ORR     W1, W6, #(DDRC_CTRL_AUTO_REF_EN)
        STR     W1, [X24, #DDRC_CTRL_REG]

        /* set SR reg (auto active delay) */
        MOV     W1, #0x1
        STR     W1, [X24, #DDRC_SELF_REFRESH_REG]

#if 0   /* we never use power down mode */
        /* set power down mode */
        ADR     X0, DRAM_PowerDownParam                 /* DRAM_PowerDownParam:            .word   0x00000030 */
        LDR     W1, [X0]
        STR     W1, [X24, #DDRC_POWER_DOWN_CTRL_REG]
#endif

        /* configure another DDRC */
        CMP     X22, X24
        CSEL    X24, X24, X22, EQ
        ADD     W7, W7, #4
        B.NE    DdrcConfigLoop

        /* set DRAM_ENABLE bit */
        ORR     W1, W6, #(DDRC_CTRL_AUTO_REF_EN | DDRC_CTRL_ENABLE)
        STR     W1, [X25, #DDRC_CTRL_REG]

/*
 *  DRAM INITIALIZATION done, Return to the Caller
 */
        LDR     X0, =SECURE_BOOT_COPY_ADDR
        LDR     W0, [X0]
        ANDS    W0, W0, #0x1
        BEQ     Done
        LDR     X24, =(AMBA_CORTEX_A53_SCRATCHPAD_S_BASE_ADDR - AMBA_CORTEX_A53_SECURE_AHB_VIRT_BASE_ADDR + AMBA_CORTEX_A53_SECURE_AHB_PHYS_BASE_ADDR)
        STR     WZR,[X20, #RCT_RNG_CTRL_REG]
        MOVZ    W0, #0x2
        STR     W0, [X24, #SCP_RNG_CTRL_REG]
Rng_Wait:
        LDR     W0, [X24, #SCP_RNG_CTRL_REG]
        TBZ     W0, #0, Rng_Wait

        LDR     W0, [X24, #SCP_RNG_DATA0]
        LDR     W1, [X24, #SCP_RNG_DATA1]
        LDR     W2, [X24, #SCP_RNG_DATA2]
        LDR     W3, [X24, #SCP_RNG_DATA3]
        LDR     W4, [X24, #SCP_RNG_DATA0]

/*
 *  Setup RNG key and enable dram scrambler feature
 */
        STR     W0, [X23, #DRAMC_SEC_KEY0]
        ADD     X5, X23, #DRAMC_SEC_KEY1
        BL      DRAMSeqReg_Set
        MOVZ    W1, #0x2
        STR     W1, [x23, #DRAMC_SEC_CTRL]
Done:
        RET     X29                                     /* Return to caller */



/*
 *  DRAM sequential register set Subroutine
 */
DRAMSeqReg_SetSame:
        MOV     W2, W1              /* Set W2 to the same value of W1 */
        MOV     W3, W1              /* Set W3 to the same value of W1 */
        MOV     W4, W1              /* Set W4 to the same value of W1 */
DRAMSeqReg_Set:
        STR     W1, [X5]            /* sequential Word-0 Register = W1 */
        STR     W2, [X5, #4]        /* sequential Word-1 Register = W2 */
        STR     W3, [X5, #8]        /* sequential Word-2 Register = W3 */
        STR     W4, [X5, #12]       /* sequential Word-3 Register = W4 */
        RET

/*
 *  DRAM mode register set and waiting for idle Subroutine
 */
DramModeReg_Set:
        STR     W1, [X24, #DDRC_MODE_REG]
/*
 *  DRAM mode register waiting for idle Subroutine
 */
DramModeReg_Wait:
        LDR     W1, [X24, #DDRC_MODE_REG]
        TBNZ    W1, #31, DramModeReg_Wait               /* Loop until busy flag is clear */
        RET                                             /* Return to caller */

/*
 *  DRAM innt control register set and waiting for idle Subroutine
 */
DramInitCtrlReg_Set:
        STR     W2, [X24, #DDRC_INIT_CTRL_REG]
/*
 *  DRAM init control register waiting for idle Subroutine,
 *  W2 = DDRC_INIT_CTRL_ZQ_CLIB or
 *       DDRC_INIT_CTRL_DLL_RESET or
 *       DDRC_INIT_CTRL_PAD_CLIB or
 *       DDRC_INIT_CTRL_GET_RTT
 */
DramInitCtrlReg_Wait:
        LDR     W1, [X24, #DDRC_INIT_CTRL_REG]
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
        LDR     W18, [X20, #RCT_TIMER_COUNT_REG]        /* W18 = Current RCT Timer Counter */
        CMP     W19, W18                                /* W19 > W18 */
        B.HI    RctTimer_Wait_L1                        /* yes, loop */

        RET                                             /* Return to caller */

