/**
 *  @file AmbaBST_InitLPDDR5.asm
 *
 *  Copyright (c) 2021 Ambarella International LP
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
 *  @details DDR Controller initializations for LPDDR5
 *
 */

#include "AmbaBST.h"

/*
 *  DDRC Initializtions
 */
        .global __AmbaDramInit

__AmbaDramInit:
        MOV     X29, X30                                /* backup link register (X30) to X29 */

        /* X20 = Base Address of RCT registers */
        LDR     X20, =(AMBA_DBG_PORT_RCT_BASE_ADDR)
        /* X23 = Base Address of DBG_PORT_DDRH0 registers */
        LDR     X23, =(AMBA_DBG_PORT_DDRH0_BASE_ADDR)
        /* X24 = Base Address of DBG_PORT_DDRH1 registers */
        LDR     X24, =(AMBA_DBG_PORT_DDRH1_BASE_ADDR)

        /* X21 = Base Address of DDRC0 registers */
        LDR     X21, =(AMBA_CORTEX_A76_DDR_CTRL0_BASE_ADDR)
        /* X22 = Base Address of DDRC1 registers */
        LDR     X22, =(AMBA_CORTEX_A76_DDR_CTRL1_BASE_ADDR)
        /* X25 = Base Address of DRAM registers */
        LDR     X25, =(AMBA_CORTEX_A76_DRAM_CTRL_BASE_ADDR)

/*
 *  setup DDR PLL, X23 = Base Address of DBG_PORT_DDRH0
 */
        ADR     X0, PLL_DdrCtrlParam                    /* X0 = Base Address of PLL_DdrCtrlParam */
        LDP     W1, W2, [X0], #8                        /* Load 2 words from X0 to W1 and W2 */
        LDR     W3, [X0]                                /* Load 1 word from X0 to W3 */
        STR     W1, [X23, #PLL_DDR_PLL_CTRL_REG]        /* DDR PLL Control Register Value = W1 */
        STR     W2, [X23, #PLL_DDR_PLL_CTRL2_REG]       /* DDR PLL Control-2 Register Value = W2 */
        STR     W3, [X23, #PLL_DDR_PLL_CTRL3_REG]       /* DDR PLL Control-3 Register Value = W3 */
        ADD     X17, X23, #PLL_DDR_PLL_CTRL_REG         /* X17 = DDR PLL Control Register */
        BL      PllCtrlReg_Enable

/*
 *  setup DDR PLL, X24 = Base Address of DBG_PORT_DDRH1
 */
        ADR     X0, PLL_DdrCtrlParam                    /* X0 = Base Address of PLL_DdrCtrlParam */
        LDP     W1, W2, [X0], #8                        /* Load 2 words from X0 to W1 and W2 */
        LDR     W3, [X0]                                /* Load 1 word from X0 to W3 */
        STR     W1, [X24, #PLL_DDR_PLL_CTRL_REG]        /* DDR PLL Control Register Value = W1 */
        STR     W2, [X24, #PLL_DDR_PLL_CTRL2_REG]       /* DDR PLL Control-2 Register Value = W2 */
        STR     W3, [X24, #PLL_DDR_PLL_CTRL3_REG]       /* DDR PLL Control-3 Register Value = W3 */
        ADD     X17, X24, #PLL_DDR_PLL_CTRL_REG         /* X17 = DDR PLL Control Register */
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

        ADR     X0, PLL_CortexCtrlParam
        LDP     W1, W2, [X0], #8
        LDR     W3, [X0]
        UBFX    W4, W2, #8, #1
        ADD     W4, W4, #1
        UBFX    W5, W2, #9, #1
        ADD     W5, W5, #1
        UBFX    W6, W1, #24, #7
        ADD     W6, W6, #1
        MUL     W6, W6, W4
        MUL     W6, W6, W5
        ANDS    W7, W2, #0X1000
        BNE     GET_PLL_DONE
        UBFX    W7, W2, #11, #1
        ADD     W7, W7, #1
        UBFX    W8, W1, #16, #4
        ADD     W8, W8, #1
        MUL     W7, W7, W8
        MUL     W7, W7, W4
        UDIV     W6, W6, W7
GET_PLL_DONE:
        CMP     W6, #0x32
        BLT     SET_DSU
LOAD_MAX_DSU:
        LDR     W1, =0x31100000
        LDR     W2, =0x30520000
        LDR     W3, =0x000C8002
SET_DSU:
        STR     W1, [X20, #PLL_DSU_PLL_CTRL_REG]
        STR     W2, [X20, #PLL_DSU_PLL_CTRL2_REG]
        STR     W3, [X20, #PLL_DSU_PLL_CTRL3_REG]
        ADD     X17, X20, #PLL_DSU_PLL_CTRL_REG
        BL      PllCtrlReg_Enable

        ADR     X0, DRAMC_ModeParam
        LDR     W1, [X0]
        STR     W1, [X25]

/*
 *  Get DIE1 info, W7 bit 26 = DIE1 exist
 */
        ADR     X1, DRAM_Config1Param
        LDR     W7, [X1]

/*
 *  Enter DDRC setting loop
 */
        MOV     X26, #0x0                                /* X26 : current host ID */
DDRC_HOST_INIT_LOOP:
        CMP     X26, #DRAMC_HOST_MAX_NUM
        BGE     DONE
        ADD     X26, X26, #1
        LDR     W1, [X25]
        MOV     W2, #1
        LSL     X2, X2, X26
        ANDS    X1, X1, X2
        BEQ     DDRC_HOST_INIT_LOOP
        CMP     X26, #1
        Beq     1f
        B       2f

1:      MOV     X27, X21
        ADR     X0, DLL_Setting0D0Param_Host0
        B       START_INIT
2:      MOV     X27, X22
        ADR     X0, DLL_Setting0D0Param_Host1

START_INIT:
/*
 *  setup DLL Control Selection Registers, X0 = Base Address of DLL Control Select Registers
 */
        LDP     W1, W2, [X0, #DLL_SETTING0D0PARAM]
        LDP     W3, W4, [X0, #DLL_SETTING1D0PARAM]
        LDP     W5, W6, [X0, #DLL_SETTING2D0PARAM]
        STR     W1, [X27, #DDRC_DLL_CTRL_SEL0_DIE0_REG]
        STR     W3, [X27, #DDRC_DLL_CTRL_SEL1_DIE0_REG]
        STR     W5, [X27, #DDRC_DLL_CTRL_SEL2_DIE0_REG]

        TST     W7, #0x4000000
        BEQ     PROG_DIE1_DLL_END

PROG_DIE1_DLL:
        STR     W2, [X27, #DDRC_DLL_CTRL_SEL0_DIE1_REG]
        STR     W4, [X27, #DDRC_DLL_CTRL_SEL1_DIE1_REG]
        STR     W6, [X27, #DDRC_DLL_CTRL_SEL2_DIE1_REG]

PROG_DIE1_DLL_END:
        LDP     W1, W2, [X0, #DLL_SBC0PARAM]
        LDP     W3, W4, [X0, #DLL_SBC2PARAM]
        STR     W1, [X27, #DDRC_DLL_SBC_0_REG]
        STR     W2, [X27, #DDRC_DLL_SBC_1_REG]
        STR     W3, [X27, #DDRC_DLL_SBC_2_REG]
        STR     W4, [X27, #DDRC_DLL_SBC_3_REG]

        /* Die0 Delay */
        ADD     X3, X0, #DRAM_DIE0BYTE0DLY0PARAM
        ADD     X4, X27, #DDRC_BYTE0_DIE0_DLY0_REG
        BL      DdrcByteDelay_Set

        ADD     X3, X0, #DRAM_DIE0BYTE1DLY0PARAM
        ADD     X4, X27, #DDRC_BYTE1_DIE0_DLY0_REG
        BL      DdrcByteDelay_Set

        ADD     X3, X0, #DRAM_DIE0BYTE2DLY0PARAM
        ADD     X4, X27, #DDRC_BYTE2_DIE0_DLY0_REG
        BL      DdrcByteDelay_Set

        ADD     X3, X0, #DRAM_DIE0BYTE3DLY0PARAM
        ADD     X4, X27, #DDRC_BYTE3_DIE0_DLY0_REG
        BL      DdrcByteDelay_Set

        TST     W7, #0x4000000
        BEQ     PROG_DIE1_DELAY_END

        /* Die1 Delay */
PROG_DIE1_DELAY:
        ADD     X3, X0, #DRAM_DIE1BYTE0DLY0PARAM
        ADD     X4, X27, #DDRC_BYTE0_DIE1_DLY0_REG
        BL      DdrcByteDelay_Set

        ADD     X3, X0, #DRAM_DIE1BYTE1DLY0PARAM
        ADD     X4, X27, #DDRC_BYTE1_DIE1_DLY0_REG
        BL      DdrcByteDelay_Set

        ADD     X3, X0, #DRAM_DIE1BYTE2DLY0PARAM
        ADD     X4, X27, #DDRC_BYTE2_DIE1_DLY0_REG
        BL      DdrcByteDelay_Set

        ADD     X3, X0, #DRAM_DIE1BYTE3DLY0PARAM
        ADD     X4, X27, #DDRC_BYTE3_DIE1_DLY0_REG
        BL      DdrcByteDelay_Set

PROG_DIE1_DELAY_END:
        LDP     W1, W2, [X0, #DRAM_CKDLYPARAM]
        LDR     W3, [X0, #DRAM_CKEDLYPARAM]
        STR     W1, [X27, #DDRC_CK_DLY_REG]
        STR     W2, [X27, #DDRC_CA_DELAY_COARSE_REG]
        STR     W3, [X27, #DDRC_CKE_DELAY_COARSE_REG]

        LDP     W1, W2, [X0, #DRAM_VREF0PARAM]
        STR     W1, [X27, #DDRC_VREF_0_REG]
        STR     W2, [X27, #DDRC_VREF_1_REG]
        /**
         *  Wait for a while
         */
        BL      DllProgram_Wait

        /* Wait for pll lock, need at least 40 us delay */
        MOVZ    W19, #960
        BL      RctTimer_Wait

/*
 *  Delay access to DRAM controller - on cold-reset, there may be
 *  POR-induced di/dt - some delay here will avoid that state
 */
        MOVZ    W19, #(24 * 2000 + 24 * 10)
        BL      RctTimer_Wait

/*
 *   DDR Controller Initializations
 */
        ADR     X1, DRAM_CtrlParam
        LDR     w28, [X1]                                /* w28 = Ddrc Reset Value */
        STR     w28, [X27, #DDRC_CTRL_REG]

        MOVZ    W19, #4800                               /* W19 = 200us */
        BL      RctTimer_Wait

        /* write to DDRC_CONFIG1_REG */
        //ADR     X1, DRAM_Config1Param
        //LDR     W7, [X1]
        STR     W7, [X27, #DDRC_CONFIG1_REG]
        /* write to DDRC_CONFIG2_REG */
        ADR     X1, DRAM_Config2Param
        LDR     W2, [X1]
        STR     W2, [X27, #DDRC_CONFIG2_REG]
/*
 *  Write DRAM_TIMING1/DRAM_TIMING2/DRAM_TIMING3
 */
        ADR     X1, DRAM_Timing1Param                   /* X0 = Base Address of DRAM_Timing1Param */
        LDP     W2, W3, [X1], #8                        /* Load 2 words from X0 to W1 and W2 */
        LDP     W4, W5, [X1], #8                        /* Load 2 words from X0 to W3 and W4 */
        LDR     W6, [X1]
        STR     W2, [X27, #DDRC_TIMING1_REG]
        STR     W3, [X27, #DDRC_TIMING2_REG]
        STR     W4, [X27, #DDRC_TIMING3_REG]
        STR     W5, [X27, #DDRC_TIMING4_REG]
        STR     W6, [X27, #DDRC_TIMING5_REG]
        ADR     X1, DRAM_DeulDieTimingParam
        LDR     W2, [X1]
        STR     W2, [X27, #DDRC_DUAL_DIE_TIMING_REG]
        ADR     X1, DRAM_RefreshTimingParam
        LDR     W2, [X1]
        STR     W2, [X27, #DDRC_REFRESH_TIMING_REG]
        ADR     X1, DRAM_Lp5TimingParam
        LDR     W2, [X1]
        STR     W2, [X27, #DDRC_LP5_TIMING_REG]
        LDR     W1, =AMBA_RAM_DDR_RTT_CALIBRATION_OFFSET
        STR     W1, [X27, #DDRC_RSVD_SPACE_REG]          /* DRAM0 RTT Address = AMBA_RAM_DDR_RTT_CALIBRATION_OFFSET */
        /* Arrange the order of DQ byte-0/1/2/3 */
        LDR     W1, [X0, #DRAM_BYTEMAPPARAM]
        STR     W1, [X27, #DDRC_BYTE_MAP_REG]

        ADR     X1, DRAM_DtteTimingParam
        LDR     W2, [X1]
        STR     W2, [X27, #DDRC_DTTE_TIMING_REG]
        LDR     W1, [X0, #DRAM_PADTERMPARAM]
        STR     W1, [X27, #DDRC_PAD_TERM_REG]
        LDR     W1, [X0, #DRAM_DQSPUPDPARAM]
        STR     W1, [X27, #DDRC_DQS_PUPD_REG]
        LDR     W1, [X0, #DRAM_CAPADCTRLPARAM]
        STR     W1, [X27, #DDRC_CA_PAD_CTRL_REG]
        LDR     W1, [X0, #DRAM_DQPADCTRLPARAM]
        STR     W1, [X27, #DDRC_DQ_PAD_CTRL_REG]
        LDR     W1, [X0, #DRAM_IBIASPARAM]
        STR     W1, [X27, #DDRC_IBIAS_REG]
        MOVZ    W19, #24
        BL      RctTimer_Wait

        MOVZ    W1, #0
        ORR     W1, W1, #(DDRC_IO_CTRL_RESET)
        STR     W1, [X27, #DDRC_IO_CTRL_REG]
        /* wait tINIT3 = 2ms */
        MOVZ    W19, #48024
        BL      RctTimer_Wait

        /* Do DLL reset */
        MOVZ    W1, #DDRC_INIT_CTRL_DLL_RESET           /* enable DLL reset */
        STR     W1, [X27, #DDRC_INIT_CTRL_REG]
        BL      DramInitCtrlReg_Wait

        MOVZ    W19, #24000
        BL      RctTimer_Wait

        /* exit Power Down - drive CS = 1 */
        MOVZ    W1, #1
        MOVZ    W2, #0
        MOVZ    W4, #0x0303
        BL      DramUInst_Set
        /* wait tCSH = 3ns */
        MOVZ    W19, #24
        BL      RctTimer_Wait

        /* exit Power Down - release CS */
        MOVZ    W1, #2
        MOVZ    W2, #0
        MOVZ    W4, #0x0303
        BL      DramUInst_Set
        /* wait tINIT5 = 2us */
        MOVZ    W19, #72
        BL      RctTimer_Wait

        /* Program MR16 */
        LDR     W1, [X0, #LPDDR5_MODEREG16PARAMFSP0]
        BL      DramModeReg_Set

        /* Program MR1 */
        LDR     W1, [X0, #LPDDR5_MODEREG1PARAMFSP0]
        BL      DramModeReg_Set

        /* Program MR2 */
        LDR     W1, [X0, #LPDDR5_MODEREG2PARAMFSP0]
        BL      DramModeReg_Set

        /* Program MR18 */
        LDR     W1, [X0, #LPDDR5_MODEREG18PARAMFSP0]
        BL      DramModeReg_Set

        /* Program MR19 */
        LDR     W1, [X0, #LPDDR5_MODEREG19PARAMFSP0]
        BL      DramModeReg_Set

        /* Program MR30 */
        //LDR     W1, [X0, #LPDDR5_MODEREG30PARAMFSP0]
        //STR     W1, [X27, #DDRC_MODE_REG]
        //BL      DramModeReg_Wait

        /* Program MR3 */
        LDR     W1, [X0, #LPDDR5_MODEREG3PARAMFSP0]
        BL      DramModeReg_Set

        /* Program MR10 */
        LDR     W1, [X0, #LPDDR5_MODEREG10PARAMFSP0]
        BL      DramModeReg_Set

        /* Program MR13 */
        LDR     W1, [X0, #LPDDR5_MODEREG13PARAMFSP0]
        BL      DramModeReg_Set

        /* Program MR41 */
        LDR     W1, [X0, #LPDDR5_MODEREG41PARAMFSP0]
        BL      DramModeReg_Set

        /* Program MR11 */
        LDR     W1, [X0, #LPDDR5_MODEREG11PARAMFSP0]
        BL      DramModeReg_Set

        /* Program MR12 */
        ADD     X2, X0, #LPDDR5_MODEREG12C0D0PARAM
        BL      DramModeReg_SetChannelDie

        /* Program MR24 */
        LDR     W1, [X0, #LPDDR5_MODEREG24PARAMFSP0]
        BL      DramModeReg_Set

        /* Program MR14 */
        ADD     X2, X0, #LPDDR5_MODEREG14C0D0PARAM
        BL      DramModeReg_SetChannelDie

        /* Program MR15 */
        ADD     X2, X0, #LPDDR5_MODEREG15C0D0PARAM
        BL      DramModeReg_SetChannelDie

        /* Program MR17 */
        LDR     W1, [X0, #LPDDR5_MODEREG17PARAMFSP0]
        BL      DramModeReg_Set

        /* Program MR18 */
        //ADR     X0, LPDDR5_ModeReg18ParamFsp0
        //LDR     W1, [X0]
        //STR     W1, [X27, #DDRC_MODE_REG]
        //BL      DramModeReg_Wait

        /* Program MR20 */
        LDR     W1, [X0, #LPDDR5_MODEREG20PARAMFSP0]
        BL      DramModeReg_Set

        /* Program MR28 */
        LDR     W1, [X0, #LPDDR5_MODEREG28PARAMFSP0]
        BL      DramModeReg_Set

        /* Program MR22 */
        LDR     W1, [X0, #LPDDR5_MODEREG22PARAMFSP0]
        BL      DramModeReg_Set

        /* Program MR30 */
        ADD     X2, X0, #LPDDR5_MODEREG30C0D0PARAMFSP0
        BL      DramModeReg_SetChannelDie

        /* Perform ZQ calibration for die 0 */
        MOVZ    W1, #0xd
        MOVZ    W2, #0x670
        MOVZ    W4, #0x301
        BL      DramUInst_Set

        /* wait TZQLAT = 30ns */
        MOVZ    W19, #24
        BL      RctTimer_Wait

        TST     W7, #0x4000000
        BEQ     SKIP_DIE1_ZQC
        /* Perform ZQ calibration for die 1 */
        MOVZ    W1, #0xd
        MOVZ    W2, #0x670
        MOVZ    W4, #0x302
        BL      DramUInst_Set


        /* wait TZQLAT = 30ns */
        MOVZ    W19, #24
        BL      RctTimer_Wait

SKIP_DIE1_ZQC:
        /* trigger manual long PAD calibration */
        MOVZ    W1, #(DDRC_ZQ_CTRL_LONG | DDRC_ZQ_CTRL_RESET)
        STR     W1, [X27, #DDRC_ZCTRL_REG]

        /* wait tCSH = 100ns */
        MOVZ    W19, #2400
        BL      RctTimer_Wait

        MOVZ    W1, #(DDRC_ZQ_CTRL_RESET)
        STR     W1, [X27, #DDRC_ZCTRL_REG]

        /* Perform trigger cas_fs  */
        MOVZ    W1, #0x100e
        MOVZ    W2, #0x0
        MOVZ    W4, #0x303
        BL      DramUInst_Set

        /* tZQLAT: idle time (at least 30 ns) */
        MOVZ    W19, #24
        BL      RctTimer_Wait

        /* Program MR46_0 */
        LDR     W1, [X0, #LPDDR5_MODEREG46PARAMFSP_DIE0]
        ORR     W1, W1, #1
        BL      DramModeReg_Set

        /* set DQS gate to 1 */
        ADR     X2, DRAM_Config2Param
        LDR     W3, [X2]
        BIC     W1, W3, #0x00001800
        ORR     W1, W1, #0x00000800
        STR     W1, [X27, #DDRC_CONFIG2_REG]

        /* Do DLL reset */
        MOVZ    W1, #DDRC_INIT_CTRL_GET_RTT           /* enable DLL reset */
        STR     W1, [X27, #DDRC_INIT_CTRL_REG]
        BL      DramInitCtrlReg_Wait

        /* Restore DQS gate to original value */
        STR     W3, [X27, #DDRC_CONFIG2_REG]

        /* Program MR46_0 */
        LDR     W1, [X0, #LPDDR5_MODEREG46PARAMFSP_DIE0]
        BL      DramModeReg_Set

        TST     W7, #0x4000000
        BEQ     SKIP_DIE1_RTT
        /* Program MR46_1 */
        LDR     W1, [X0, #LPDDR5_MODEREG46PARAMFSP_DIE1]
        ORR     W1, W1, #1
        BL      DramModeReg_Set

        /* set DQS gate to 1 */
        BIC     W1, W3, #0x00001800
        ORR     W1, W1, #0x00000800
        STR     W1, [X27, #DDRC_CONFIG2_REG]

        /* Do DLL reset */
        MOVZ    W1, #(DDRC_INIT_CTRL_GET_RTT | DDRC_INIT_CTRL_DIE1)           /* enable DLL reset */
        STR     W1, [X27, #DDRC_INIT_CTRL_REG]
        BL      DramInitCtrlReg_Wait

        /* Restore DQS gate to original value */
        STR     W3, [X27, #DDRC_CONFIG2_REG]

        /* Program MR46_1 */
        LDR     W1, [X0, #LPDDR5_MODEREG46PARAMFSP_DIE1]
        BL      DramModeReg_Set
SKIP_DIE1_RTT:

        /* set DRAM_ENABLE bit */
        MOVZ    W1, #( DDRC_CTRL_AUTO_REF_EN | DDRC_CTRL_ENABLE )
        ORR     W1, w28, W1
        STR     W1, [X27, #DDRC_CTRL_REG]
        B       DDRC_HOST_INIT_LOOP
        /* ========== DDRC Iinit End=========== */

DONE:

/*
 *  DRAM INITIALIZATION done, Return to the Caller
 */
        RET     X29                                     /* Return to caller */

/*
 *  DDRC Byte Delay register set Subroutine
 */
/*
 *  Notice the sequence of is important to let this function succeed
 *  DLY0(0x0) -> DLY1(0x4) -> DLY2(0x8) -> DLY3(0xc) -> DLY4(0x10) -> DLY5(0x14).
 *  X3 = the address of DLY0 parameter
 *  X4 = the address of DLY0 register
 */
DdrcByteDelay_Set:
        LDP     W1, W2, [X3, #0]
        STP     W1, W2, [X4, #0]
        LDP     W1, W2, [X3, #8]
        STP     W1, W2, [X4, #8]
        LDP     W1, W2, [X3, #16]
        STP     W1, W2, [X4, #16]
        RET

/*
 *  DRAM mode register set separating channel/die Subroutine
 */
/*
 *  Notice the sequence of is important to let this function succeed
 *  C0D0(0x0) -> C0D1(0x4) -> C1D0(0x8) -> C1D1(0xc).
 *  X2 = the address of C0D0
 *  X7 = the CONFIG1 value to check dual die(rank)
 */
DramModeReg_SetChannelDie:
        MOV     X3, X30             // Backup return address
        /* Die 0 */
        LDR     W1, [X2]            // C0D0
        BL      DramModeReg_Set
        LDR     W1, [X2, #8]        // C1D0
        BL      DramModeReg_Set

        TST     W7, #0x4000000
        BEQ     DramModeReg_SetChannelDie_Done
        /* Die 1 */
        LDR     W1, [X2, #4]        // C0D1
        BL      DramModeReg_Set
        LDR     W1, [X2, #12]       // C1D1
        BL      DramModeReg_Set
DramModeReg_SetChannelDie_Done:
        MOV     X30, X3             // Restore return address
        RET

/*
 *  DRAM mode register set and waiting for idle Subroutine
 */
DramModeReg_Set:
        STR     W1, [X27, #DDRC_MODE_REG]
/*
 *  DRAM mode register waiting for idle Subroutine
 */
DramModeReg_Wait:
        LDR     W1, [X27, #DDRC_MODE_REG]
        TBNZ    W1, #31, DramModeReg_Wait               /* Loop until busy flag is clear */
        RET                                             /* Return to caller */

/*
 *  DRAM init control register waiting for idle Subroutine,
 *  W2 = DDRC_INIT_CTRL_ZQ_CLIB or
 *       DDRC_INIT_CTRL_DLL_RESET or
 *       DDRC_INIT_CTRL_PAD_CLIB or
 *       DDRC_INIT_CTRL_GET_RTT
 */
DramInitCtrlReg_Wait:
        and     w2, w1, #0xf
        LDR     W1, [X27, #DDRC_INIT_CTRL_REG]
        ANDS    W1, W1, W2
        B.NE    DramInitCtrlReg_Wait
        RET                                             /* Return to caller */

/*
 *  DRAM Micro instruction set and waiting Subroutine
 */
/*
 *  W1 = the value to be set to UINST1 register
 *  W2 = the value to be set to UINST2 register
 *  W4 = the value to be set to UINST4 register
 */
DramUInst_Set:
        STR     W1, [X27, #DDRC_UINST1_REG]
        STR     W2, [X27, #DDRC_UINST2_REG]
        STR     W4, [X27, #DDRC_UINST4_REG]
        MOVZ    W1, #0x1
        STR     W1, [X27, #DDRC_UINST5_REG]

DramUInst5Reg_Wait:
        LDR     W1, [X27, #DDRC_UINST5_REG]
        ANDS    W1, W1,#0x1
        B.NE    DramUInst5Reg_Wait
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
        MOV     W19, #4800

/*
 *  RTC Timer waiting Subroutine, default CLK_REF (Clock Frequency) = 24MHz
 */
RctTimer_Wait:
        MOVZ    W2, #1                                  /* W2 = 1 */
        STR     W2, [X20, #RCT_TIMER_CTRL_REG]          /* Reset RCT Timer */
        STR     WZR, [X20, #RCT_TIMER_CTRL_REG]          /* Enable RCT Timer */

/*
 *  Fix RCT timer metastability issue : freeze RCT timer before fetch RCT Timer Counter.
 */

RctTimer_Wait_L1:
        LDR     W18, [X20, #RCT_TIMER_COUNT_REG]        /* W18 = Current RCT Timer Counter */
        CMP     W19, W18                                /* W19 > W18 */
        B.HI    RctTimer_Wait_L1                        /* yes, loop */

        RET                                             /* Return to caller */

