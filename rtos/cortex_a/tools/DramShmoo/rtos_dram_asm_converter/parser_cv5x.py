import bitarray
import bitarray.util

import plat

from util import Bitfield, Register

__version__ = "v1.1"

TEST_BYTE = False

DRAM_TYPE_TABLE = {0: "LPDDR4", 1: "LPDDR5"}

ASM_TEMPLATE = {
    "LPDDR4": "/lib/CV5x_LPDDR4_temp.asm",
    "LPDDR5": "/lib/CV5x_LPDDR5_temp.asm",
}

H_TO_ASM_MAP = {
    "PLL_CORTEX_CTRL_REG_VAL": ["PLL_CortexCtrlParam"],
    "PLL_CORE_CTRL_REG_VAL": ["PLL_CoreCtrlParam"],

    "DRAM_MODE_REG_VAL": ["DRAMC_ModeParam"],

    # n-to-1 mapping
    # ---------- DDRC0 ----------
    "PLL_DDR_CTRL_REG_DDRH0_VAL": ["PLL_DdrCtrlParam"],
    "PLL_DDR_CTRL2_REG_DDRH0_VAL": ["PLL_DdrCtrl2Param"],
    "PLL_DDR_CTRL3_REG_DDRH0_VAL": ["PLL_DdrCtrl3Param"],
    # ---------- DDRC1 ----------
    "PLL_DDR_CTRL_REG_DDRH1_VAL": ["PLL_DdrCtrlParam"],
    "PLL_DDR_CTRL2_REG_DDRH1_VAL": ["PLL_DdrCtrl2Param"],
    "PLL_DDR_CTRL3_REG_DDRH1_VAL": ["PLL_DdrCtrl3Param"],

    # n-to-1 mapping
    # ---------- DDRC0 ----------
    "DLL_CTRL_SEL0_DIE0_DDRC0_VAL": ["DLL_Setting0D0Param_Host0"],
    "DLL_CTRL_SEL1_DIE0_DDRC0_VAL": ["DLL_Setting1D0Param_Host0"],
    "DLL_CTRL_SEL2_DIE0_DDRC0_VAL": ["DLL_Setting2D0Param_Host0"],
    "DLL_CTRL_SEL0_DIE1_DDRC0_VAL": ["DLL_Setting0D1Param_Host0"],
    "DLL_CTRL_SEL1_DIE1_DDRC0_VAL": ["DLL_Setting1D1Param_Host0"],
    "DLL_CTRL_SEL2_DIE1_DDRC0_VAL": ["DLL_Setting2D1Param_Host0"],
    "DLL_CTRL_SBC_0_DDRC0_VAL": ["DLL_Sbc0Param_Host0"],
    "DLL_CTRL_SBC_1_DDRC0_VAL": ["DLL_Sbc1Param_Host0"],
    "DLL_CTRL_SBC_2_DDRC0_VAL": ["DLL_Sbc2Param_Host0"],
    "DLL_CTRL_SBC_3_DDRC0_VAL": ["DLL_Sbc3Param_Host0"],
    # ---------- DDRC0 ----------
    "DLL_CTRL_SEL0_DIE0_DDRC1_VAL": ["DLL_Setting0D0Param_Host1"],
    "DLL_CTRL_SEL1_DIE0_DDRC1_VAL": ["DLL_Setting1D0Param_Host1"],
    "DLL_CTRL_SEL2_DIE0_DDRC1_VAL": ["DLL_Setting2D0Param_Host1"],
    "DLL_CTRL_SEL0_DIE1_DDRC1_VAL": ["DLL_Setting0D1Param_Host1"],
    "DLL_CTRL_SEL1_DIE1_DDRC1_VAL": ["DLL_Setting1D1Param_Host1"],
    "DLL_CTRL_SEL2_DIE1_DDRC1_VAL": ["DLL_Setting2D1Param_Host1"],
    "DLL_CTRL_SBC_0_DDRC1_VAL": ["DLL_Sbc0Param_Host1"],
    "DLL_CTRL_SBC_1_DDRC1_VAL": ["DLL_Sbc1Param_Host1"],
    "DLL_CTRL_SBC_2_DDRC1_VAL": ["DLL_Sbc2Param_Host1"],
    "DLL_CTRL_SBC_3_DDRC1_VAL": ["DLL_Sbc3Param_Host1"],

    # n-to-1 mapping
    # ---------- DDRC0 ----------
    "BYTE0_DLY0_DIE0_DDRC0_VAL": ["DRAM_Die0Byte0Dly0Param_Host0"],
    "BYTE0_DLY1_DIE0_DDRC0_VAL": ["DRAM_Die0Byte0Dly1Param_Host0"],
    "BYTE0_DLY2_DIE0_DDRC0_VAL": ["DRAM_Die0Byte0Dly2Param_Host0"],
    "BYTE0_DLY3_DIE0_DDRC0_VAL": ["DRAM_Die0Byte0Dly3Param_Host0"],
    "BYTE0_DLY4_DIE0_DDRC0_VAL": ["DRAM_Die0Byte0Dly4Param_Host0"],
    "BYTE0_DLY5_DIE0_DDRC0_VAL": ["DRAM_Die0Byte0Dly5Param_Host0"],
    "BYTE0_DLY0_DIE1_DDRC0_VAL": ["DRAM_Die1Byte0Dly0Param_Host0"],
    "BYTE0_DLY1_DIE1_DDRC0_VAL": ["DRAM_Die1Byte0Dly1Param_Host0"],
    "BYTE0_DLY2_DIE1_DDRC0_VAL": ["DRAM_Die1Byte0Dly2Param_Host0"],
    "BYTE0_DLY3_DIE1_DDRC0_VAL": ["DRAM_Die1Byte0Dly3Param_Host0"],
    "BYTE0_DLY4_DIE1_DDRC0_VAL": ["DRAM_Die1Byte0Dly4Param_Host0"],
    "BYTE0_DLY5_DIE1_DDRC0_VAL": ["DRAM_Die1Byte0Dly5Param_Host0"],
    "BYTE1_DLY0_DIE0_DDRC0_VAL": ["DRAM_Die0Byte1Dly0Param_Host0"],
    "BYTE1_DLY1_DIE0_DDRC0_VAL": ["DRAM_Die0Byte1Dly1Param_Host0"],
    "BYTE1_DLY2_DIE0_DDRC0_VAL": ["DRAM_Die0Byte1Dly2Param_Host0"],
    "BYTE1_DLY3_DIE0_DDRC0_VAL": ["DRAM_Die0Byte1Dly3Param_Host0"],
    "BYTE1_DLY4_DIE0_DDRC0_VAL": ["DRAM_Die0Byte1Dly4Param_Host0"],
    "BYTE1_DLY5_DIE0_DDRC0_VAL": ["DRAM_Die0Byte1Dly5Param_Host0"],
    "BYTE1_DLY0_DIE1_DDRC0_VAL": ["DRAM_Die1Byte1Dly0Param_Host0"],
    "BYTE1_DLY1_DIE1_DDRC0_VAL": ["DRAM_Die1Byte1Dly1Param_Host0"],
    "BYTE1_DLY2_DIE1_DDRC0_VAL": ["DRAM_Die1Byte1Dly2Param_Host0"],
    "BYTE1_DLY3_DIE1_DDRC0_VAL": ["DRAM_Die1Byte1Dly3Param_Host0"],
    "BYTE1_DLY4_DIE1_DDRC0_VAL": ["DRAM_Die1Byte1Dly4Param_Host0"],
    "BYTE1_DLY5_DIE1_DDRC0_VAL": ["DRAM_Die1Byte1Dly5Param_Host0"],
    "BYTE2_DLY0_DIE0_DDRC0_VAL": ["DRAM_Die0Byte2Dly0Param_Host0"],
    "BYTE2_DLY1_DIE0_DDRC0_VAL": ["DRAM_Die0Byte2Dly1Param_Host0"],
    "BYTE2_DLY2_DIE0_DDRC0_VAL": ["DRAM_Die0Byte2Dly2Param_Host0"],
    "BYTE2_DLY3_DIE0_DDRC0_VAL": ["DRAM_Die0Byte2Dly3Param_Host0"],
    "BYTE2_DLY4_DIE0_DDRC0_VAL": ["DRAM_Die0Byte2Dly4Param_Host0"],
    "BYTE2_DLY5_DIE0_DDRC0_VAL": ["DRAM_Die0Byte2Dly5Param_Host0"],
    "BYTE2_DLY0_DIE1_DDRC0_VAL": ["DRAM_Die1Byte2Dly0Param_Host0"],
    "BYTE2_DLY1_DIE1_DDRC0_VAL": ["DRAM_Die1Byte2Dly1Param_Host0"],
    "BYTE2_DLY2_DIE1_DDRC0_VAL": ["DRAM_Die1Byte2Dly2Param_Host0"],
    "BYTE2_DLY3_DIE1_DDRC0_VAL": ["DRAM_Die1Byte2Dly3Param_Host0"],
    "BYTE2_DLY4_DIE1_DDRC0_VAL": ["DRAM_Die1Byte2Dly4Param_Host0"],
    "BYTE2_DLY5_DIE1_DDRC0_VAL": ["DRAM_Die1Byte2Dly5Param_Host0"],
    "BYTE3_DLY0_DIE0_DDRC0_VAL": ["DRAM_Die0Byte3Dly0Param_Host0"],
    "BYTE3_DLY1_DIE0_DDRC0_VAL": ["DRAM_Die0Byte3Dly1Param_Host0"],
    "BYTE3_DLY2_DIE0_DDRC0_VAL": ["DRAM_Die0Byte3Dly2Param_Host0"],
    "BYTE3_DLY3_DIE0_DDRC0_VAL": ["DRAM_Die0Byte3Dly3Param_Host0"],
    "BYTE3_DLY4_DIE0_DDRC0_VAL": ["DRAM_Die0Byte3Dly4Param_Host0"],
    "BYTE3_DLY5_DIE0_DDRC0_VAL": ["DRAM_Die0Byte3Dly5Param_Host0"],
    "BYTE3_DLY0_DIE1_DDRC0_VAL": ["DRAM_Die1Byte3Dly0Param_Host0"],
    "BYTE3_DLY1_DIE1_DDRC0_VAL": ["DRAM_Die1Byte3Dly1Param_Host0"],
    "BYTE3_DLY2_DIE1_DDRC0_VAL": ["DRAM_Die1Byte3Dly2Param_Host0"],
    "BYTE3_DLY3_DIE1_DDRC0_VAL": ["DRAM_Die1Byte3Dly3Param_Host0"],
    "BYTE3_DLY4_DIE1_DDRC0_VAL": ["DRAM_Die1Byte3Dly4Param_Host0"],
    "BYTE3_DLY5_DIE1_DDRC0_VAL": ["DRAM_Die1Byte3Dly5Param_Host0"],
    # ---------- DDRC1 ----------
    "BYTE0_DLY0_DIE0_DDRC1_VAL": ["DRAM_Die0Byte0Dly0Param_Host1"],
    "BYTE0_DLY1_DIE0_DDRC1_VAL": ["DRAM_Die0Byte0Dly1Param_Host1"],
    "BYTE0_DLY2_DIE0_DDRC1_VAL": ["DRAM_Die0Byte0Dly2Param_Host1"],
    "BYTE0_DLY3_DIE0_DDRC1_VAL": ["DRAM_Die0Byte0Dly3Param_Host1"],
    "BYTE0_DLY4_DIE0_DDRC1_VAL": ["DRAM_Die0Byte0Dly4Param_Host1"],
    "BYTE0_DLY5_DIE0_DDRC1_VAL": ["DRAM_Die0Byte0Dly5Param_Host1"],
    "BYTE0_DLY0_DIE1_DDRC1_VAL": ["DRAM_Die1Byte0Dly0Param_Host1"],
    "BYTE0_DLY1_DIE1_DDRC1_VAL": ["DRAM_Die1Byte0Dly1Param_Host1"],
    "BYTE0_DLY2_DIE1_DDRC1_VAL": ["DRAM_Die1Byte0Dly2Param_Host1"],
    "BYTE0_DLY3_DIE1_DDRC1_VAL": ["DRAM_Die1Byte0Dly3Param_Host1"],
    "BYTE0_DLY4_DIE1_DDRC1_VAL": ["DRAM_Die1Byte0Dly4Param_Host1"],
    "BYTE0_DLY5_DIE1_DDRC1_VAL": ["DRAM_Die1Byte0Dly5Param_Host1"],
    "BYTE1_DLY0_DIE0_DDRC1_VAL": ["DRAM_Die0Byte1Dly0Param_Host1"],
    "BYTE1_DLY1_DIE0_DDRC1_VAL": ["DRAM_Die0Byte1Dly1Param_Host1"],
    "BYTE1_DLY2_DIE0_DDRC1_VAL": ["DRAM_Die0Byte1Dly2Param_Host1"],
    "BYTE1_DLY3_DIE0_DDRC1_VAL": ["DRAM_Die0Byte1Dly3Param_Host1"],
    "BYTE1_DLY4_DIE0_DDRC1_VAL": ["DRAM_Die0Byte1Dly4Param_Host1"],
    "BYTE1_DLY5_DIE0_DDRC1_VAL": ["DRAM_Die0Byte1Dly5Param_Host1"],
    "BYTE1_DLY0_DIE1_DDRC1_VAL": ["DRAM_Die1Byte1Dly0Param_Host1"],
    "BYTE1_DLY1_DIE1_DDRC1_VAL": ["DRAM_Die1Byte1Dly1Param_Host1"],
    "BYTE1_DLY2_DIE1_DDRC1_VAL": ["DRAM_Die1Byte1Dly2Param_Host1"],
    "BYTE1_DLY3_DIE1_DDRC1_VAL": ["DRAM_Die1Byte1Dly3Param_Host1"],
    "BYTE1_DLY4_DIE1_DDRC1_VAL": ["DRAM_Die1Byte1Dly4Param_Host1"],
    "BYTE1_DLY5_DIE1_DDRC1_VAL": ["DRAM_Die1Byte1Dly5Param_Host1"],
    "BYTE2_DLY0_DIE0_DDRC1_VAL": ["DRAM_Die0Byte2Dly0Param_Host1"],
    "BYTE2_DLY1_DIE0_DDRC1_VAL": ["DRAM_Die0Byte2Dly1Param_Host1"],
    "BYTE2_DLY2_DIE0_DDRC1_VAL": ["DRAM_Die0Byte2Dly2Param_Host1"],
    "BYTE2_DLY3_DIE0_DDRC1_VAL": ["DRAM_Die0Byte2Dly3Param_Host1"],
    "BYTE2_DLY4_DIE0_DDRC1_VAL": ["DRAM_Die0Byte2Dly4Param_Host1"],
    "BYTE2_DLY5_DIE0_DDRC1_VAL": ["DRAM_Die0Byte2Dly5Param_Host1"],
    "BYTE2_DLY0_DIE1_DDRC1_VAL": ["DRAM_Die1Byte2Dly0Param_Host1"],
    "BYTE2_DLY1_DIE1_DDRC1_VAL": ["DRAM_Die1Byte2Dly1Param_Host1"],
    "BYTE2_DLY2_DIE1_DDRC1_VAL": ["DRAM_Die1Byte2Dly2Param_Host1"],
    "BYTE2_DLY3_DIE1_DDRC1_VAL": ["DRAM_Die1Byte2Dly3Param_Host1"],
    "BYTE2_DLY4_DIE1_DDRC1_VAL": ["DRAM_Die1Byte2Dly4Param_Host1"],
    "BYTE2_DLY5_DIE1_DDRC1_VAL": ["DRAM_Die1Byte2Dly5Param_Host1"],
    "BYTE3_DLY0_DIE0_DDRC1_VAL": ["DRAM_Die0Byte3Dly0Param_Host1"],
    "BYTE3_DLY1_DIE0_DDRC1_VAL": ["DRAM_Die0Byte3Dly1Param_Host1"],
    "BYTE3_DLY2_DIE0_DDRC1_VAL": ["DRAM_Die0Byte3Dly2Param_Host1"],
    "BYTE3_DLY3_DIE0_DDRC1_VAL": ["DRAM_Die0Byte3Dly3Param_Host1"],
    "BYTE3_DLY4_DIE0_DDRC1_VAL": ["DRAM_Die0Byte3Dly4Param_Host1"],
    "BYTE3_DLY5_DIE0_DDRC1_VAL": ["DRAM_Die0Byte3Dly5Param_Host1"],
    "BYTE3_DLY0_DIE1_DDRC1_VAL": ["DRAM_Die1Byte3Dly0Param_Host1"],
    "BYTE3_DLY1_DIE1_DDRC1_VAL": ["DRAM_Die1Byte3Dly1Param_Host1"],
    "BYTE3_DLY2_DIE1_DDRC1_VAL": ["DRAM_Die1Byte3Dly2Param_Host1"],
    "BYTE3_DLY3_DIE1_DDRC1_VAL": ["DRAM_Die1Byte3Dly3Param_Host1"],
    "BYTE3_DLY4_DIE1_DDRC1_VAL": ["DRAM_Die1Byte3Dly4Param_Host1"],
    "BYTE3_DLY5_DIE1_DDRC1_VAL": ["DRAM_Die1Byte3Dly5Param_Host1"],

    "CK_DELAY_DDRC0_VAL": ["DRAM_CkDlyParam_Host0"],
    "CK_DELAY_DDRC1_VAL": ["DRAM_CkDlyParam_Host1"],

    "CA_DELAY_COARSE_DDRC0_VAL": ["DRAM_CaDelayCoarseParam_Host0"],
    "CA_DELAY_COARSE_DDRC1_VAL": ["DRAM_CaDelayCoarseParam_Host1"],

    "CKE_DELAY_COARSE_DDRC0_VAL": ["DRAM_CkeDlyParam_Host0"],
    "CKE_DELAY_COARSE_DDRC1_VAL": ["DRAM_CkeDlyParam_Host1"],

    "DDRIO_VREF_0_DDRC0_VAL": ["DRAM_Vref0Param_Host0"],
    "DDRIO_VREF_0_DDRC1_VAL": ["DRAM_Vref0Param_Host1"],

    "DDRIO_VREF_1_DDRC0_VAL": ["DRAM_Vref1Param_Host0"],
    "DDRIO_VREF_1_DDRC1_VAL": ["DRAM_Vref1Param_Host1"],

    # n-to-1 mapping
    # ---------- DDRC0 ----------
    "DRAM_CONTROL_DDRC0_VAL": ["DRAM_CtrlParam"],
    "DRAM_CONFIG1_DDRC0_VAL": ["DRAM_Config1Param"],
    "DRAM_CONFIG2_DDRC0_VAL": ["DRAM_Config2Param"],
    "DRAM_TIMING1_DDRC0_VAL": ["DRAM_Timing1Param"],
    "DRAM_TIMING2_DDRC0_VAL": ["DRAM_Timing2Param"],
    "DRAM_TIMING3_DDRC0_VAL": ["DRAM_Timing3Param"],
    "DRAM_TIMING4_DDRC0_VAL": ["DRAM_Timing4Param"],
    "DRAM_TIMING5_DDRC0_VAL": ["DRAM_Timing5Param"],
    "DUAL_DIE_TIMING_DDRC0_VAL": ["DRAM_DeulDieTimingParam"],
    "REFRESH_TIMING_DDRC0_VAL": ["DRAM_RefreshTimingParam"],
    "LP5_TIMING_DDRC0_VAL": ["DRAM_Lp5TimingParam"],
    "DRAM_WDQS_TIMING_DDRC0_VAL": ["DRAM_WdqsTimingParam"],
    "DTTE_TIMING_DDRC0_VAL": ["DRAM_DtteTimingParam"],
    # ---------- DDRC1 ----------
    "DRAM_CONTROL_DDRC1_VAL": ["DRAM_CtrlParam"],
    "DRAM_CONFIG1_DDRC1_VAL": ["DRAM_Config1Param"],
    "DRAM_CONFIG2_DDRC1_VAL": ["DRAM_Config2Param"],
    "DRAM_TIMING1_DDRC1_VAL": ["DRAM_Timing1Param"],
    "DRAM_TIMING2_DDRC1_VAL": ["DRAM_Timing2Param"],
    "DRAM_TIMING3_DDRC1_VAL": ["DRAM_Timing3Param"],
    "DRAM_TIMING4_DDRC1_VAL": ["DRAM_Timing4Param"],
    "DRAM_TIMING5_DDRC1_VAL": ["DRAM_Timing5Param"],
    "DUAL_DIE_TIMING_DDRC1_VAL": ["DRAM_DeulDieTimingParam"],
    "REFRESH_TIMING_DDRC1_VAL": ["DRAM_RefreshTimingParam"],
    "LP5_TIMING_DDRC1_VAL": ["DRAM_Lp5TimingParam"],
    "DRAM_WDQS_TIMING_DDRC1_VAL": ["DRAM_WdqsTimingParam"],
    "DTTE_TIMING_DDRC1_VAL": ["DRAM_DtteTimingParam"],

    # n-to-1 mapping
    # ---------- DDRC0 ----------
    "DDRIO_PAD_CTRL_DDRC0_VAL": ["DRAM_PadTermParam_Host0"],
    "DDRIO_DQS_PUPD_DDRC0_VAL": ["DRAM_DqsPuPdParam_Host0"],
    "DDRIO_CA_PADCTRL_DDRC0_VAL": ["DRAM_CaPadCtrlParam_Host0"],
    "DDRIO_DQ_PADCTRL_DDRC0_VAL": ["DRAM_DqPadCtrlParam_Host0"],
    "DDRIO_IBIAS_CTRL_DDRC0_VAL": ["DRAM_IbiasParam_Host0"],
    "DRAM_BYTE_MAP_DDRC0_VAL": ["DRAM_ByteMapParam_Host0"],
    # ---------- DDRC1 ----------
    "DDRIO_PAD_CTRL_DDRC1_VAL": ["DRAM_PadTermParam_Host1"],
    "DDRIO_DQS_PUPD_DDRC1_VAL": ["DRAM_DqsPuPdParam_Host1"],
    "DDRIO_CA_PADCTRL_DDRC1_VAL": ["DRAM_CaPadCtrlParam_Host1"],
    "DDRIO_DQ_PADCTRL_DDRC1_VAL": ["DRAM_DqPadCtrlParam_Host1"],
    "DDRIO_IBIAS_CTRL_DDRC1_VAL": ["DRAM_IbiasParam_Host1"],
    "DRAM_BYTE_MAP_DDRC1_VAL": ["DRAM_ByteMapParam_Host1"],

    # 1-to-n mapping
    "LPDDR4_MR1_VAL":   ["LPDDR4_ModeReg1ParamFsp0_Host0"],
    "LPDDR4_MR2_VAL":   ["LPDDR4_ModeReg2ParamFsp0_Host0"],
    "LPDDR4_MR3_VAL":   ["LPDDR4_ModeReg3ParamFsp0_Host0"],
    "LPDDR4_MR11_VAL":  ["LPDDR4_ModeReg11ParamFsp0_Host0"],
    "LPDDR4_MR12_VAL":  ["LPDDR4_ModeReg12ParamC0D0_Host0",
                         "LPDDR4_ModeReg12ParamC0D1_Host0",
                         "LPDDR4_ModeReg12ParamC1D0_Host0",
                         "LPDDR4_ModeReg12ParamC1D1_Host0",],
    "LPDDR4_MR13_VAL":  ["LPDDR4_ModeReg13ParamFsp0_Host0"],
    "LPDDR4_MR14_VAL":  ["LPDDR4_ModeReg14ParamC0D0_Host0",
                         "LPDDR4_ModeReg14ParamC0D1_Host0",
                         "LPDDR4_ModeReg14ParamC1D0_Host0",
                         "LPDDR4_ModeReg14ParamC1D1_Host0",],
    "LPDDR4_MR22_VAL":  ["LPDDR4_ModeReg22ParamFsp0_Host0"],

    # 1-to-n mapping
    "LPDDR5_MR1_VAL":   ["LPDDR5_ModeReg1ParamFsp0_Host0"],
    "LPDDR5_MR2_VAL":   ["LPDDR5_ModeReg2ParamFsp0_Host0"],
    "LPDDR5_MR3_VAL":   ["LPDDR5_ModeReg3ParamFsp0_Host0"],
    "LPDDR5_MR10_VAL":  ["LPDDR5_ModeReg10ParamFsp0_Host0"],
    "LPDDR5_MR11_VAL":  ["LPDDR5_ModeReg11ParamFsp0_Host0"],
    "LPDDR5_MR12_VAL":  ["LPDDR5_ModeReg12ParamC0D0_Host0",
                         "LPDDR5_ModeReg12ParamC0D1_Host0",
                         "LPDDR5_ModeReg12ParamC1D0_Host0",
                         "LPDDR5_ModeReg12ParamC1D1_Host0"],
    "LPDDR5_MR13_VAL":  ["LPDDR5_ModeReg13ParamFsp0_Host0"],
    "LPDDR5_MR14_VAL":  ["LPDDR5_ModeReg14ParamC0D0_Host0",
                         "LPDDR5_ModeReg14ParamC0D1_Host0",
                         "LPDDR5_ModeReg14ParamC1D0_Host0",
                         "LPDDR5_ModeReg14ParamC1D1_Host0"],
    "LPDDR5_MR15_VAL":  ["LPDDR5_ModeReg15ParamC0D0_Host0",
                         "LPDDR5_ModeReg15ParamC0D1_Host0",
                         "LPDDR5_ModeReg15ParamC1D0_Host0",
                         "LPDDR5_ModeReg15ParamC1D1_Host0"],
    "LPDDR5_MR16_VAL":  ["LPDDR5_ModeReg16ParamFsp0_Host0"],
    "LPDDR5_MR17_VAL":  ["LPDDR5_ModeReg17ParamFsp0_Host0"],
    "LPDDR5_MR18_VAL":  ["LPDDR5_ModeReg18ParamFsp0_Host0"],
    "LPDDR5_MR19_VAL":  ["LPDDR5_ModeReg19ParamFsp0_Host0"],
    "LPDDR5_MR20_VAL":  ["LPDDR5_ModeReg20ParamFsp0_Host0"],
    "LPDDR5_MR22_VAL":  ["LPDDR5_ModeReg22ParamFsp0_Host0"],
    "LPDDR5_MR24_VAL":  ["LPDDR5_ModeReg24ParamFsp0_Host0"],
    "LPDDR5_MR28_VAL":  ["LPDDR5_ModeReg28ParamFsp0_Host0"],
    "LPDDR5_MR30_VAL":  ["LPDDR5_ModeReg30ParamC0D0Fsp0_Host0",
                         "LPDDR5_ModeReg30ParamC0D1Fsp0_Host0",
                         "LPDDR5_ModeReg30ParamC1D0Fsp0_Host0",
                         "LPDDR5_ModeReg30ParamC1D1Fsp0_Host0"],
    "LPDDR5_MR41_VAL":  ["LPDDR5_ModeReg41ParamFsp0_Host0"],
    "LPDDR5_MR46_VAL":  ["LPDDR5_ModeReg46ParamFsp_die0_Host0",
                         "LPDDR5_ModeReg46ParamFsp_die1_Host0"],
}

def version():
    return "parser_cv5x: %s" % __version__

def get_dram_type(d):
    regs = []
    k_list = ["DRAM_Config1Param", "DRAM_CONFIG1_DDRC0_VAL", "DRAM_CONFIG1_DDRC1_VAL"]
    for k, v in d.items():
        if k in k_list:
            #print("%s: 0x%08x" % (k, v[0]))
            reg = Register(k, bitarray.util.int2ba(d[k][0], 32, 'little'), None, False,
                Bitfield("DRAM_TYPE", (2, 0), lambda x: DRAM_TYPE_TABLE[x]),
            )
            regs.append(reg)

    if not plat.all_equal([r.value() for r in regs]):
        print("[Warning] Register value is not all identical!", end='')
        for r in regs:
            print(" (%s: 0x%08x)" % (r.name, r.value()), end='')
        print()

    return regs[0].bitfield["DRAM_TYPE"].mapped_value()

def parse(out, d, platform):
    # ========== Search for DRAM_BUS_WIDTH ==========
    k = "DRAMC_ModeParam"
    if k in d:
        dram_bus_width_table = {2: "64 bits"}
        reg = Register(k, bitarray.util.int2ba(d[k][0], 32, 'little'), out, True,
            Bitfield("DRAM_BUS_WIDTH", (4, 3), lambda x: dram_bus_width_table[x])
        )
        bus_width = int(reg.bitfield["DRAM_BUS_WIDTH"].mapped_value().split()[0])
        del d[k]

    # ========== Search for DRAM TYPE/SIZE/DUAL_RANK ==========
    k = "DRAM_Config1Param"
    if k in d:
        #dram_type_table = {0: "LPDDR4", 1: "LPDDR5"}
        dram_dual_rank_table = {0: "Disable", 1: "Enable"}
        dram_ch_size_table = {
            0: "2 Gbits",
            1: "3 Gbits",
            2: "4 Gbits",
            3: "6 Gbits",
            4: "8 Gbits",
            5: "12 Gbits",
            6: "16 Gbits",
            7: "24 Gbits",
            8: "32 Gbits"
        }
        dram_size_table = {k: "%d %s" % (int(v.split()[0])*4, v.split()[1])
            for k, v in dram_ch_size_table.items()}

        # dram type = reg[2:0]
        # dram dual rank = reg[26]
        # dram size (per ch/die) = reg[5:2] (only 1 die per channel)
        reg = Register(k, bitarray.util.int2ba(d[k][0], 32, 'little'), out, True,
            Bitfield("DRAM_TYPE", (1, 0), lambda x: DRAM_TYPE_TABLE[x]),
            Bitfield("DRAM_DUAL_RANK", (26, 26), lambda x: dram_dual_rank_table[x]),
            Bitfield("DRAM_CH_SIZE", (5, 2), lambda x: dram_ch_size_table[x])
        )
        dram_type = reg.bitfield["DRAM_TYPE"].mapped_value()
        dual_rank = reg.bitfield["DRAM_DUAL_RANK"].value()
        dram_size_per_ch, postfix = reg.bitfield["DRAM_CH_SIZE"].mapped_value().split()
        dram_size_per_ch = int(dram_size_per_ch)
        dram_size = dram_size_per_ch * (2 if dual_rank else 1) * (bus_width / 16)
        out.update({"DRAM_SIZE": "%d %s" % (dram_size, postfix)})
        del out["DRAM_CH_SIZE"]
        del d[k]
        print("[Info] DRAM type: %s" % dram_type)

    # ========== Find DRAM freq ==========
    k = "PLL_DdrCtrlParam"
    freq = 24
    reg = Register(k, bitarray.util.int2ba(d[k][0], 32, 'little'), None, True,
        Bitfield("Intp", (30, 24), lambda x: x+1),
        Bitfield("Sout", (19, 16), lambda x: x+1),
        Bitfield("Sdiv", (15, 12), lambda x: x+1)
    )
    intp = reg.bitfield["Intp"].mapped_value()
    sout = reg.bitfield["Sout"].mapped_value()
    sdiv = reg.bitfield["Sdiv"].mapped_value()
    k = "PLL_DdrCtrl2Param"
    reg = Register(k, bitarray.util.int2ba(d[k][0], 32, 'little'), None, True,
        Bitfield("Fsdiv", (9, 9), lambda x: x+1),
        Bitfield("Fsout", (11, 11), lambda x: x+1),
        Bitfield("ClkDiv2", (8, 8), lambda x: x+1),
        Bitfield("Reg2Bit12", (12, 12), lambda x: x+1)
    )
    fsdiv = reg.bitfield["Fsdiv"].mapped_value()
    fsout = reg.bitfield["Fsout"].mapped_value()
    clkdiv2 = reg.bitfield["ClkDiv2"].mapped_value()
    reg2bit12 = reg.bitfield["Reg2Bit12"].value()
    if reg2bit12:
        freq = freq / 2
        freq = freq * clkdiv2 * fsdiv * sdiv * intp
    else:
        freq = freq / fsout / sout / 2
        freq = freq * fsdiv * sdiv * intp

    if (dram_type == "LPDDR4"):
        freq = freq / 2 # LPDDR4 clock is divided by 2 in PLL module

    out.update({"DRAM_FREQUENCY": "%d" % freq})

    # ========== Find dq r/w dly ==========
    k = "DRAM_DieXByteXDly1Param_HostX"
    reg_name, reg_raw = plat.collect_from_all_host(k, d, platform, "byte", False if TEST_BYTE else True)
    reg = Register(reg_name, reg_raw, None, True,
        Bitfield("R_DLY_COARSE", (19, 15), None),
        Bitfield("W_DLY_COARSE", (14, 10), None)
    )
    r_dly_coarse = reg.bitfield["R_DLY_COARSE"].value()
    w_dly_coarse = reg.bitfield["W_DLY_COARSE"].value()
    """
    if TEST_BYTE:
        r_dly_coarse_item = {}
        w_dly_coarse_item = {}
        for h in range(plat.HOST_COUNT[platform]):
            for b in range(plat.BYTE_COUNT[platform]):
                reg_name, reg_raw = plat.collect_from_target(k, d, "H%dD0B%d" % (h, b), True)
                reg = Register(reg_name, reg_raw, None, True,
                    Bitfield("R_DLY_COARSE", (19, 15), None),
                    Bitfield("W_DLY_COARSE", (14, 10), None)
                )
                r_dly_coarse_item.update({"H%dB%d" % (h, b): reg.bitfield["R_DLY_COARSE"].value()})
                w_dly_coarse_item.update({"H%dB%d" % (h, b): reg.bitfield["W_DLY_COARSE"].value()})
    """

    k = "DRAM_DieXByteXDly4Param_HostX"
    reg_name, reg_raw = plat.collect_from_all_host(k, d, platform, "byte", False if TEST_BYTE else True)
    reg = Register(reg_name, reg_raw, None, True,
        Bitfield("R_DLY_FINE", (4, 0), None)    # currently all bits are the same, we use bit0
    )
    r_dly_fine = reg.bitfield["R_DLY_FINE"].value()
    """
    if TEST_BYTE:
        r_dly_fine_item = {}
        for h in range(plat.HOST_COUNT[platform]):
            for b in range(plat.BYTE_COUNT[platform]):
                reg_name, reg_raw = plat.collect_from_target(k, d, "H%dD0B%d" % (h, b), True)
                reg = Register(reg_name, reg_raw, None, True,
                    Bitfield("R_DLY_FINE", (4, 0), None)    # currently all bits are the same, we use bit0
                )
                r_dly_fine_item.update({"H%dB%d" % (h, b): reg.bitfield["R_DLY_FINE"].value()})
    """

    k = "DRAM_DieXByteXDly2Param_HostX"
    reg_name, reg_raw = plat.collect_from_all_host(k, d, platform, "byte", False if TEST_BYTE else True)
    reg = Register(reg_name, reg_raw, None, True,
        Bitfield("W_DLY_FINE", (4, 0), None)    # currently all bits are the same, we use bit0
    )
    w_dly_fine = reg.bitfield["W_DLY_FINE"].value()
    """
    if TEST_BYTE:
        w_dly_fine_item = {}
        for h in range(plat.HOST_COUNT[platform]):
            for b in range(plat.BYTE_COUNT[platform]):
                reg_name, reg_raw = plat.collect_from_target(k, d, "H%dD0B%d" % (h, b), True)
                reg = Register(reg_name, reg_raw, None, True,
                    Bitfield("W_DLY_FINE", (4, 0), None)    # currently all bits are the same, we use bit0
                )
                w_dly_fine_item.update({"H%dB%d" % (h, b): reg.bitfield["W_DLY_FINE"].value()})
    """

    out.update({"DDRC_DQ_READ_DELAY": r_dly_coarse*12 + r_dly_fine})
    out.update({"DDRC_DQ_WRITE_DELAY": w_dly_coarse*12 + w_dly_fine})
    """
    if TEST_BYTE:
        for h in range(plat.HOST_COUNT[platform]):
            for b in range(plat.BYTE_COUNT[platform]):
                out.update({"DDRC%d_BYTE%d_DQ_READ_DELAY" % (h, b): r_dly_coarse_item["H%dB%d" % (h, b)]*12 + r_dly_fine_item["H%dB%d" % (h, b)]})
                out.update({"DDRC%d_BYTE%d_DQ_WRITE_DELAY" % (h, b): r_dly_coarse_item["H%dB%d" % (h, b)]*12 + r_dly_fine_item["H%dB%d" % (h, b)]})
    """

    # ========== Find dq/dqs vref ==========
    k = "DRAM_Vref0Param_HostX"
    reg_name, reg_raw = plat.collect_from_all_host(k, d, platform, None)
    reg = Register(reg_name, reg_raw, out, True,
        Bitfield("DDRC_DQ_VREF", (5, 0), None),
        Bitfield("DDRC_DQS_VREF", (11, 6), None)
    )

    # ========== Find DLL ==========
    k = "DLL_Setting0DXParam_HostX"
    reg_name, reg_raw = plat.collect_from_all_host(k, d, platform, "die", False)
    reg = Register(reg_name, reg_raw, out, True,
        Bitfield("DDRC_READ_DLL", (5, 0), hex)
    )
    for h in range(plat.HOST_COUNT[platform]):
        reg_name, reg_raw = plat.collect_from_target(k, d, "H%dD0" % h, True)
        reg = Register(reg_name, reg_raw, out, True,
            Bitfield("DDRC%d_READ_DLL" % h, (5, 0), hex)
        )
    """
    reg_name, reg_raw = plat.collect_from_target(k, d, "H0D0", True)
    reg = Register(reg_name, reg_raw, out, True,
        Bitfield("DDRC0_BYTE0_READ_DLL", (5, 0), hex),
        Bitfield("DDRC0_BYTE1_READ_DLL", (13, 8), hex),
        Bitfield("DDRC0_BYTE2_READ_DLL", (21, 16), hex),
        Bitfield("DDRC0_BYTE3_READ_DLL", (29, 24), hex)
    )
    reg_name, reg_raw = plat.collect_from_target(k, d, "H1D0", True)
    reg = Register(reg_name, reg_raw, out, True,
        Bitfield("DDRC1_BYTE0_READ_DLL", (5, 0), hex),
        Bitfield("DDRC1_BYTE1_READ_DLL", (13, 8), hex),
        Bitfield("DDRC1_BYTE2_READ_DLL", (21, 16), hex),
        Bitfield("DDRC1_BYTE3_READ_DLL", (29, 24), hex)
    )
    """

    k = "DLL_Setting1DXParam_HostX"
    reg_name, reg_raw = plat.collect_from_all_host(k, d, platform, "die", False)
    reg = Register(reg_name, reg_raw, out, True,
        Bitfield("DDRC_SYNC_DLL", (5, 0), hex)
    )
    for h in range(plat.HOST_COUNT[platform]):
        reg_name, reg_raw = plat.collect_from_target(k, d, "H%dD0" % h, True)
        reg = Register(reg_name, reg_raw, out, True,
            Bitfield("DDRC%d_SYNC_DLL" % h, (5, 0), hex)
        )
    """
    reg_name, reg_raw = plat.collect_from_target(k, d, "H0D0", True)
    reg = Register(reg_name, reg_raw, out, True,
        Bitfield("DDRC0_BYTE0_SYNC_DLL", (5, 0), hex),
        Bitfield("DDRC0_BYTE1_SYNC_DLL", (13, 8), hex),
        Bitfield("DDRC0_BYTE2_SYNC_DLL", (21, 16), hex),
        Bitfield("DDRC0_BYTE3_SYNC_DLL", (29, 24), hex)
    )
    reg_name, reg_raw = plat.collect_from_target(k, d, "H1D0", True)
    reg = Register(reg_name, reg_raw, out, True,
        Bitfield("DDRC1_BYTE0_SYNC_DLL", (5, 0), hex),
        Bitfield("DDRC1_BYTE1_SYNC_DLL", (13, 8), hex),
        Bitfield("DDRC1_BYTE2_SYNC_DLL", (21, 16), hex),
        Bitfield("DDRC1_BYTE3_SYNC_DLL", (29, 24), hex)
    )
    """

    k = "DLL_Setting2DXParam_HostX"
    reg_name, reg_raw = plat.collect_from_all_host(k, d, platform, "die", False)
    reg = Register(reg_name, reg_raw, out, True,
        Bitfield("DDRC_WRITE_DLL", (5, 0), hex)
    )
    for h in range(plat.HOST_COUNT[platform]):
        reg_name, reg_raw = plat.collect_from_target(k, d, "H%dD0" % h, True)
        reg = Register(reg_name, reg_raw, out, True,
            Bitfield("DDRC%d_WRITE_DLL" % h, (5, 0), hex)
        )
    """
    reg_name, reg_raw = plat.collect_from_target(k, d, "H0D0", True)
    reg = Register(reg_name, reg_raw, out, True,
        Bitfield("DDRC0_BYTE0_WRITE_DLL", (5, 0), hex),
        Bitfield("DDRC0_BYTE1_WRITE_DLL", (13, 8), hex),
        Bitfield("DDRC0_BYTE2_WRITE_DLL", (21, 16), hex),
        Bitfield("DDRC0_BYTE3_WRITE_DLL", (29, 24), hex)
    )
    reg_name, reg_raw = plat.collect_from_target(k, d, "H1D0", True)
    reg = Register(reg_name, reg_raw, out, True,
        Bitfield("DDRC1_BYTE0_WRITE_DLL", (5, 0), hex),
        Bitfield("DDRC1_BYTE1_WRITE_DLL", (13, 8), hex),
        Bitfield("DDRC1_BYTE2_WRITE_DLL", (21, 16), hex),
        Bitfield("DDRC1_BYTE3_WRITE_DLL", (29, 24), hex)
    )
    """

    # ========== Find TERM/DDS/PDDS ==========
    k = "DRAM_DqPadCtrlParam_HostX"
    reg_name, reg_raw = plat.collect_from_all_host(k, d, platform, None)
    reg = Register(reg_name, reg_raw, out, True,
        Bitfield("DDRC_TERM", (5, 3), lambda x: plat.DRIVE_STRENGTH[x]),
        Bitfield("DDRC_DDS", (9, 7), lambda x: plat.DRIVE_STRENGTH[x]),
        Bitfield("DDRC_PDDS", (2, 0), lambda x: plat.DRIVE_STRENGTH[x])
    )

    # ========== Find DRAM_DS ==========
    k = "LPDDR5_ModeReg3ParamFsp0_HostX" if (dram_type == "LPDDR5") else "LPDDR4_ModeReg3ParamFsp0_HostX"
    reg_name, reg_raw = plat.collect_from_all_host(k, d, platform, None)
    reg = Register(reg_name, reg_raw, out, True,
        Bitfield("DRAM_DS", (2, 0) if (dram_type == "LPDDR5") else (5, 3), lambda x: plat.DRAM_DS[dram_type][x])
    )

    # ========== Find DRAM_ODT ==========
    k = "LPDDR5_ModeReg11ParamFsp0_HostX" if (dram_type == "LPDDR5") else "LPDDR4_ModeReg11ParamFsp0_HostX"
    reg_name, reg_raw = plat.collect_from_all_host(k, d, platform, None)
    reg = Register(reg_name, reg_raw, out, True,
        Bitfield("DRAM_DQ_ODT", (2, 0), lambda x: plat.DRAM_ODT[dram_type][x]),
        Bitfield("DRAM_CA_ODT", (6, 4), lambda x: plat.DRAM_ODT[dram_type][x])
    )

    # ========== Find DRAM_CA_VREF ==========
    k = "LPDDR5_ModeReg12ParamCXDX_HostX" if (dram_type == "LPDDR5") else "LPDDR4_ModeReg12ParamCXDX_HostX"
    reg_name, reg_raw = plat.collect_from_all_host(k, d, platform, "ch")
    reg = Register(reg_name, reg_raw, out, True,
        Bitfield("DRAM_CA_VREF", (6, 0) if (dram_type == "LPDDR5") else (5, 0), None)
    )

    # ========== Find DRAM_DQ_VREF ==========
    if (dram_type == "LPDDR5"):
        k = ["LPDDR5_ModeReg14ParamCXDX_HostX", "LPDDR5_ModeReg15ParamCXDX_HostX"]
    else:
        k = ["LPDDR4_ModeReg14ParamCXDX_HostX"]
    reg_name, reg_raw = plat.collect_multiple(k, d, platform, "ch")
    reg = Register(reg_name, reg_raw, out, True,
        Bitfield("DRAM_DQ_VREF", (6, 0) if (dram_type == "LPDDR5") else (5, 0), None)
    )

    # ========== Find DRAM_SOC_ODT ==========
    k = "LPDDR5_ModeReg17ParamFsp0_HostX" if (dram_type == "LPDDR5") else "LPDDR4_ModeReg22ParamFsp0_HostX"
    reg_name, reg_raw = plat.collect_from_all_host(k, d, platform, None)
    reg = Register(reg_name, reg_raw, out, True,
        Bitfield("DRAM_SOC_ODT", (2, 0) if (dram_type == "LPDDR5") else (2, 0), lambda x: plat.DRAM_ODT[dram_type][x])
    )

    if (dram_type == "LPDDR5"):
        # ========== Find DRAM_WCK_ODT ==========
        k = "LPDDR5_ModeReg18ParamFsp0_HostX"
        reg_name, reg_raw = plat.collect_from_all_host(k, d, platform, None)
        reg = Register(reg_name, reg_raw, out, True,
            Bitfield("DRAM_WCK_ODT", (2, 0), lambda x: plat.DRAM_ODT[dram_type][x])
        )

        # ========== Find DRAM_NT_DQ_ODT ==========
        k = "LPDDR5_ModeReg41ParamFsp0_HostX"
        reg_name, reg_raw = plat.collect_from_all_host(k, d, platform, None)
        reg = Register(reg_name, reg_raw, out, True,
            Bitfield("DRAM_NT_DQ_ODT", (7, 5), lambda x: plat.DRAM_ODT[dram_type][x])
        )

        # ========== Find DDRC_WCK_WRITE_DELAY ==========
        k = "DRAM_DieXByteXDly0Param_HostX"
        reg_name, reg_raw = plat.collect_from_all_host(k, d, platform, "byte", False)
        reg = Register(reg_name, reg_raw, out, True,
            Bitfield("DDRC_WCK_WRITE_DELAY", (9, 0), None)
        )
        for host in range(plat.HOST_COUNT[platform]):
            reg_name, reg_raw = plat.collect_from_target(k, d, "H%dD0B0" % host, True)
            reg = Register(reg_name, reg_raw, out, True,
                Bitfield("DDRC%d_WCK_WRITE_DELAY" % host, (9, 0), None)
            )

    # ========== Find DDRC_CA_DELAY ==========

    # ========== Find DDRC_BYTE_MAP ==========
    k = "DRAM_ByteMapParam_HostX"
    """
    reg_name, reg_raw = plat.collect_from_all_host(k, d, platform, None)
    reg = Register(reg_name, reg_raw, out, True,
        Bitfield("DDRC_BYTE_MAP", (7, 0), hex)
    )
    """
    for host in range(plat.HOST_COUNT[platform]):
        reg_name, reg_raw = plat.collect_from_target(k, d, "H%d" % host, True)
        reg = Register(reg_name, reg_raw, out, True,
            Bitfield("DDRC%d_BYTE_MAP" % h, (7, 0), lambda x: "B%dB%dB%dB%d" % ((x>>6)&0x3, (x>>4)&0x3, (x>>2)&0x3, x&0x3))
        )

    # ========== Extra ==========
    out.update({"IDSP_FREQUENCY": "672 MHz"})
    out.update({"VISION_FREQUENCY": "504 MHz"})
    out.update({"DRAM_TRAINING": "Disable"})
    out.update({"CHIP_NAME": platform})

    # ========== Show unused ==========
    for k in d:
        print("[Warning] %s unused! (Info could be lost in translations...)" % k)


