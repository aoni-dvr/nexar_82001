/**
 *  @file AmbaRTSL_SD_Ctrl.c
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
 *  @details SD Delay Control RTSL APIs
 *
 */

#include "AmbaTypes.h"

#include "AmbaCSL_SD.h"
#include "AmbaRTSL_SD.h"
#include "AmbaMisraFix.h"

static void AmbaRTSL_SDPhySetBypass(UINT32 SdChanNo, UINT32 Type, UINT8 Arg)
{
    switch (Type) {
    case AMBA_SD_PHY_CLK_OUT_BYPASS:
        if (SdChanNo == AMBA_SD_CHANNEL0) {
            AmbaCSL_RctSetSd0CtrlClkOutBypass(Arg);
        } else {
            AmbaCSL_RctSetSd2CtrlClkOutBypass(Arg);
        }
        break;
    case AMBA_SD_PHY_DATA_CMD_BYPASS:
        if (SdChanNo == AMBA_SD_CHANNEL0) {
            AmbaCSL_RctSetSd0CtrlDataCmdBypass(Arg);
        } else {
            AmbaCSL_RctSetSd2CtrlDataCmdBypass(Arg);
        }
        break;
    case AMBA_SD_PHY_DLL_BYPASS:
        if (SdChanNo == AMBA_SD_CHANNEL0) {
            AmbaCSL_RctSetSd0CtrlDllBypass(Arg);
        } else {
            AmbaCSL_RctSetSd2CtrlDllBypass(Arg);
        }
        break;
    default:
        /* For Misrac check */
        break;
    }
}

/**
 *  AmbaRTSL_SDPhySetConfig -
 *  @param[in] SdChanNo SD Channel Number
 *  @param[in] Type
 *  @param[in] Value
 */
void AmbaRTSL_SDPhySetConfig(UINT32 SdChanNo, UINT32 Type, UINT32 Value)
{
    UINT8 Arg = 1U;

    if (Value == 0U) {
        Arg = 0U;
    }

    switch (Type) {
    case AMBA_SD_PHY_DIN_CLK_POL:
        if (SdChanNo == AMBA_SD_CHANNEL0) {
            AmbaCSL_RctSetSd0CtrlDinClkPolarity(Arg);
        } else {
            AmbaCSL_RctSetSd2CtrlDinClkPolarity(Arg);
        }
        break;
    case AMBA_SD_PHY_RESET:
        if (SdChanNo == AMBA_SD_CHANNEL0) {
            AmbaCSL_RctSetSd0CtrlReset(Arg);
        } else {
            AmbaCSL_RctSetSd2CtrlReset(Arg);
        }
        break;
    case AMBA_SD_PHY_CLK270_ALONE:
        if (SdChanNo == AMBA_SD_CHANNEL0) {
            AmbaCSL_RctSetSd0CtrlClk270Alone(Arg);
        } else {
            AmbaCSL_RctSetSd2CtrlClk270Alone(Arg);
        }
        break;
    case AMBA_SD_PHY_RX_CLK_POL:
        if (SdChanNo == AMBA_SD_CHANNEL0) {
            AmbaCSL_RctSetSd0CtrlRxClkPolarity(Arg);
        } else {
            AmbaCSL_RctSetSd2CtrlRxClkPolarity(Arg);
        }
        break;
    default:
        // For Misrac check
        AmbaRTSL_SDPhySetBypass(SdChanNo, Type, Arg);
        break;
    }
}

/**
 *  AmbaRTSL_SDPhyGetConfig -
 *  @param[in] SdChanNo SD Channel Number
 *  @param[in] Type
 *  @return
 */
UINT8 AmbaRTSL_SDPhyGetConfig(UINT32 SdChanNo, UINT32 Type)
{
    UINT8 Rval = 0x0U;

    switch(Type) {
    case AMBA_SD_PHY_DIN_CLK_POL:
        if(SdChanNo == AMBA_SD_CHANNEL0) {
            Rval = AmbaCSL_RctGetSd0CtrlDinClkPolarity();
        } else {
            Rval = AmbaCSL_RctGetSd2CtrlDinClkPolarity();
        }
        break;
    case AMBA_SD_PHY_CLK_OUT_BYPASS:
        if(SdChanNo == AMBA_SD_CHANNEL0) {
            Rval = AmbaCSL_RctGetSd0CtrlClkoutBypass();
        } else {
            Rval = AmbaCSL_RctGetSd2CtrlClkoutBypass();
        }
        break;
    case AMBA_SD_PHY_RESET:
        if(SdChanNo == AMBA_SD_CHANNEL0) {
            Rval = AmbaCSL_RctGetSd0CtrlReset();
        } else {
            Rval = AmbaCSL_RctGetSd2CtrlReset();
        }
        break;
    case AMBA_SD_PHY_CLK270_ALONE:
        if(SdChanNo == AMBA_SD_CHANNEL0) {
            Rval = AmbaCSL_RctGetSd0CtrlClk270Alone();
        } else {
            Rval = AmbaCSL_RctGetSd2CtrlClk270Alone();
        }
        break;
    case AMBA_SD_PHY_RX_CLK_POL:
        if(SdChanNo == AMBA_SD_CHANNEL0) {
            Rval = AmbaCSL_RctGetSd0CtrlRxClkPolarity();
        } else {
            Rval = AmbaCSL_RctGetSd2CtrlRxClkPolarity();
        }
        break;
    case AMBA_SD_PHY_DATA_CMD_BYPASS:
        if(SdChanNo == AMBA_SD_CHANNEL0) {
            Rval = AmbaCSL_RctGetSd0CtrlDataCmdBypass();
        } else {
            Rval = AmbaCSL_RctGetSd2CtrlDataCmdBypass();
        }
        break;
    case AMBA_SD_PHY_DLL_BYPASS:
        if(SdChanNo == AMBA_SD_CHANNEL0) {
            Rval = AmbaCSL_RctGetSd0CtrlDllBypass();
        } else {
            Rval = AmbaCSL_RctGetSd2CtrlDllBypass();
        }
        break;
    default:
        // For Misrac check
        AmbaMisra_TouchUnused(&Type);
        break;
    }

    return (Rval & 0x7U);
}

/**
 *  AmbaRTSL_SDPhySetDelayChainSel -
 *  @param[in] SdChanNo SD Channel Number
 *  @param[in] Type
 */
void AmbaRTSL_SDPhySetDelayChainSel(UINT32 SdChanNo, UINT8 Value)
{
    UINT8 Arg = Value;

    if (Value > 0x3U) {
        Arg = 0x3U;
    }

    if (SdChanNo == AMBA_SD_CHANNEL0) {
        AmbaCSL_RctSetSd0CtrlDelayChainSelect(Arg);
    }
}

/**
 *  AmbaRTSL_SDPhyGetDelayChainSel -
 *  @param[in] SdChanNo SD Channel Number
 *  @return
 */
UINT8 AmbaRTSL_SDPhyGetDelayChainSel(UINT32 SdChanNo)
{
    UINT8 Rval = 0x0;

    if(SdChanNo == AMBA_SD_CHANNEL0) {
        Rval =  AmbaCSL_RctGetSd0CtrlDelayChainSelect();
    } else {
        Rval =  AmbaCSL_RctGetSd2CtrlDelayChainSelect();
    }

    return (Rval & 0x7U);
}

/**
 *  AmbaRTSL_SDPhySetDutySelect -
 *  @param[in] SdChanNo SD Channel Number
 *  @param[in] Value
 */
void AmbaRTSL_SDPhySetDutySelect(UINT32 SdChanNo, UINT8 Value)
{
    UINT8 Arg = Value;

    if (Value > 0x2U) {
        Arg = 0x2U;
    }

    if (SdChanNo == AMBA_SD_CHANNEL0) {
        AmbaCSL_RctSetSd0CtrlSdDutySelect(Arg);
    }
}

/**
 *  AmbaRTSL_SDPhyGetDutySelect -
 *  @param[in] SdChanNo SD Channel Number
 *  @return
 */
UINT8 AmbaRTSL_SDPhyGetDutySelect(UINT32 SdChanNo)
{
    UINT8 Rval;

    AmbaMisra_TouchUnused(&SdChanNo);
    Rval = AmbaCSL_RctGetSd0CtrlSdDutySelect();
    return (Rval & 0x7U);
}

/**
 *  AmbaRTSL_SD0PhyGetSharedBusCtrl -
 *  @param[in] Type
 *  @return
 */
static UINT8 AmbaRTSL_SD0PhyGetSharedBusCtrl(AMBA_SD_PHY_SBC_e Type)
{
    UINT8 Rval = 0x0;

    switch(Type) {
    case AMBA_SD_PHY_SBC_AUTO_COARSE_ADJ:
        Rval = AmbaCSL_RctGetSd0CtrlEnableAutoCoarse();
        break;
    case AMBA_SD_PHY_SBC_POWER_DOWN_SHIFT:
        Rval = AmbaCSL_RctGetSd0CtrlPowerDownShift();
        break;
    case AMBA_SD_PHY_SBC_FORCE_LOCK_INTERCYC:
        Rval = AmbaCSL_RctGetSd0CtrlForceLockCycle();
        break;
    case AMBA_SD_PHY_SBC_FORCE_LOCK_VSHIFT:
        Rval = AmbaCSL_RctGetSd0CtrlForceLockVshift();
        break;
    case AMBA_SD_PHY_SBC_SEL_FSM:
        Rval = AmbaCSL_RctGetSd0CtrlSelectFSM();
        break;
    case AMBA_SD_PHY_SBC_BYPASS_HIGH:
        Rval = AmbaCSL_RctGetSd0CtrlBypassFilter();
        break;
    case AMBA_SD_PHY_SBC_SEL_PDBB:
        Rval = AmbaCSL_RctGetSd0CtrlPDbb();
        break;
    case AMBA_SD_PHY_SBC_SEL_INTERCLOCK:
        Rval = AmbaCSL_RctGetSd0CtrlSelectInterClock();
        break;
    case AMBA_SD_PHY_SBC_LOCK_RANGE:
        Rval = AmbaCSL_RctGetSd0CtrlLockRange();
        break;
    case AMBA_SD_PHY_SBC_COARSE_DELAY:
        Rval = AmbaCSL_RctGetSd0CtrlCoarseDelayStep();
        break;
    case AMBA_SD_PHY_SBC_ENABLEDLL:
        Rval = AmbaCSL_RctGetSd0CtrlEnableDll();
        break;
    default:
        // For Misrac check
        AmbaMisra_TouchUnused(&Type);
        break;
    }

    return Rval;
}

/**
 *  AmbaRTSL_SD2PhyGetSharedBusCtrl -
 *  @param[in] Type
 *  @return
 */
static UINT8 AmbaRTSL_SD2PhyGetSharedBusCtrl(AMBA_SD_PHY_SBC_e Type)
{
    UINT8 Rval = 0x0;

    switch(Type) {
    case AMBA_SD_PHY_SBC_AUTO_COARSE_ADJ:
        Rval = AmbaCSL_RctGetSd2CtrlEnableAutoCoarse();
        break;
    case AMBA_SD_PHY_SBC_POWER_DOWN_SHIFT:
        Rval = AmbaCSL_RctGetSd2CtrlPowerDownShift();
        break;
    case AMBA_SD_PHY_SBC_FORCE_LOCK_INTERCYC:
        Rval = AmbaCSL_RctGetSd2CtrlForceLockCycle();
        break;
    case AMBA_SD_PHY_SBC_FORCE_LOCK_VSHIFT:
        Rval = AmbaCSL_RctGetSd2CtrlForceLockVshift();
        break;
    case AMBA_SD_PHY_SBC_SEL_FSM:
        Rval = AmbaCSL_RctGetSd2CtrlSelectFSM();
        break;
    case AMBA_SD_PHY_SBC_BYPASS_HIGH:
        Rval = AmbaCSL_RctGetSd2CtrlBypassFilter();
        break;
    case AMBA_SD_PHY_SBC_SEL_PDBB:
        Rval = AmbaCSL_RctGetSd2CtrlPDbb();
        break;
    case AMBA_SD_PHY_SBC_SEL_INTERCLOCK:
        Rval = AmbaCSL_RctGetSd2CtrlSelectInterClock();
        break;
    case AMBA_SD_PHY_SBC_LOCK_RANGE:
        Rval = AmbaCSL_RctGetSd2CtrlLockRange();
        break;
    case AMBA_SD_PHY_SBC_COARSE_DELAY:
        Rval = AmbaCSL_RctGetSd2CtrlCoarseDelayStep();
        break;
    case AMBA_SD_PHY_SBC_ENABLEDLL:
        Rval = AmbaCSL_RctGetSd2CtrlEnableDll();
        break;
    default:
        // For Misrac check
        AmbaMisra_TouchUnused(&Type);
        break;
    }

    return Rval;
}

/**
 *  AmbaRTSL_SDPhyGetSharedBusCtrl -
 *  @param[in] SdChanNo SD Channel Number
 *  @param[in] Type
 *  @return
 */
UINT32 AmbaRTSL_SDPhyGetSharedBusCtrl(UINT32 SdChanNo, AMBA_SD_PHY_SBC_e Type)
{
    UINT8 Rval;

    if(SdChanNo == AMBA_SD_CHANNEL0) {
        Rval = AmbaRTSL_SD0PhyGetSharedBusCtrl(Type);
    } else {
        Rval = AmbaRTSL_SD2PhyGetSharedBusCtrl(Type);
    }

    return (UINT32) Rval;
}

/**
 *  AmbaRTSL_SD0PhySetSharedBusCtrl -
 *  @param[in] Type
 *  @param[in] Value
 *  @return
 */
static void AmbaRTSL_SD0PhySetSharedBusCtrl(AMBA_SD_PHY_SBC_e Type, UINT8 Value)
{
    switch(Type) {
    case AMBA_SD_PHY_SBC_AUTO_COARSE_ADJ:
        AmbaCSL_RctSetSd0CtrlEnableAutoCoarse(Value);
        break;
    case AMBA_SD_PHY_SBC_POWER_DOWN_SHIFT:
        AmbaCSL_RctSetSd0CtrlPowerDownShift(Value);
        break;
    case AMBA_SD_PHY_SBC_FORCE_LOCK_INTERCYC:
        AmbaCSL_RctSetSd0CtrlForceLockCycle(Value);
        break;
    case AMBA_SD_PHY_SBC_FORCE_LOCK_VSHIFT:
        AmbaCSL_RctSetSd0CtrlForceLockVshift(Value);
        break;
    case AMBA_SD_PHY_SBC_SEL_FSM:
        AmbaCSL_RctSetSd0CtrlSelectFSM(Value);
        break;
    case AMBA_SD_PHY_SBC_BYPASS_HIGH:
        AmbaCSL_RctSetSd0CtrlBypassFilter(Value);
        break;
    case AMBA_SD_PHY_SBC_SEL_PDBB:
        AmbaCSL_RctSetSd0CtrlPDbb(Value);
        break;
    case AMBA_SD_PHY_SBC_SEL_INTERCLOCK:
        AmbaCSL_RctSetSd0CtrlSelectInterClock(Value);
        break;
    case AMBA_SD_PHY_SBC_LOCK_RANGE:
        AmbaCSL_RctSetSd0CtrlLockRange(Value);
        break;
    case AMBA_SD_PHY_SBC_COARSE_DELAY:
        AmbaCSL_RctSetSd0CtrlCoarseDelayStep(Value);
        break;
    case AMBA_SD_PHY_SBC_ENABLEDLL:
        AmbaCSL_RctSetSd0CtrlEnableDll(Value);
        break;
    default:
        // For Misrac check
        AmbaMisra_TouchUnused(&Type);
        break;
    }
}

/**
 *  AmbaRTSL_SD2PhySetSharedBusCtrl -
 *  @param[in] Type
 *  @param[in] Value
 */
static void AmbaRTSL_SD2PhySetSharedBusCtrl(AMBA_SD_PHY_SBC_e Type, UINT8 Value)
{
    switch(Type) {
    case AMBA_SD_PHY_SBC_AUTO_COARSE_ADJ:
        AmbaCSL_RctSetSd2CtrlEnableAutoCoarse(Value);
        break;
    case AMBA_SD_PHY_SBC_POWER_DOWN_SHIFT:
        AmbaCSL_RctSetSd2CtrlPowerDownShift(Value);
        break;
    case AMBA_SD_PHY_SBC_FORCE_LOCK_INTERCYC:
        AmbaCSL_RctSetSd2CtrlForceLockCycle(Value);
        break;
    case AMBA_SD_PHY_SBC_FORCE_LOCK_VSHIFT:
        AmbaCSL_RctSetSd2CtrlForceLockVshift(Value);
        break;
    case AMBA_SD_PHY_SBC_SEL_FSM:
        AmbaCSL_RctSetSd2CtrlSelectFSM(Value);
        break;
    case AMBA_SD_PHY_SBC_BYPASS_HIGH:
        AmbaCSL_RctSetSd2CtrlBypassFilter(Value);
        break;
    case AMBA_SD_PHY_SBC_SEL_PDBB:
        AmbaCSL_RctSetSd2CtrlPDbb(Value);
        break;
    case AMBA_SD_PHY_SBC_SEL_INTERCLOCK:
        AmbaCSL_RctSetSd2CtrlSelectInterClock(Value);
        break;
    case AMBA_SD_PHY_SBC_LOCK_RANGE:
        AmbaCSL_RctSetSd2CtrlLockRange(Value);
        break;
    case AMBA_SD_PHY_SBC_COARSE_DELAY:
        AmbaCSL_RctSetSd2CtrlCoarseDelayStep(Value);
        break;
    case AMBA_SD_PHY_SBC_ENABLEDLL:
        AmbaCSL_RctSetSd2CtrlEnableDll(Value);
        break;
    default:
        // For Misrac check
        AmbaMisra_TouchUnused(&Type);
        break;
    }
}

/**
 *  AmbaRTSL_SDPhySetSharedBusCtrl -
 *  @param[in] SdChanNo SD Channel Number
 *  @param[in] Type
 *  @param[in] Value
 */
void AmbaRTSL_SDPhySetSharedBusCtrl(UINT32 SdChanNo, AMBA_SD_PHY_SBC_e Type, UINT8 Value)
{
    if(SdChanNo == AMBA_SD_CHANNEL0) {
        AmbaRTSL_SD0PhySetSharedBusCtrl( Type, Value);
    } else {
        AmbaRTSL_SD2PhySetSharedBusCtrl( Type, Value);
    }
}

/**
 *  AmbaRTSL_SDPhySetSelect -
 *  @param[in] SdChanNo SD Channel Number
 *  @param[in] Type
 *  @param[in] Value
 */
void AmbaRTSL_SDPhySetSelect(UINT32 SdChanNo, UINT32 Type, UINT8 Value)
{
    switch(Type) {
    case AMBA_SD_PHY_SEL0:
        if (SdChanNo == AMBA_SD_CHANNEL0) {
            AmbaCSL_RctSetSd0CtrlSelect0(Value);
        } else {
            AmbaCSL_RctSetSd2CtrlSelect0(Value);
        }
        break;
#if !defined(CONFIG_SOC_CV28) && !defined (CONFIG_SOC_CV5)&& !defined (CONFIG_SOC_CV52)
    case AMBA_SD_PHY_SEL1:
        if (SdChanNo == AMBA_SD_CHANNEL0) {
            AmbaCSL_RctSetSd0CtrlSelect1(Value);
        } else {
            AmbaCSL_RctSetSd2CtrlSelect1(Value);
        }
        break;
#endif
    case AMBA_SD_PHY_SEL2:
        if (SdChanNo == AMBA_SD_CHANNEL0) {
            AmbaCSL_RctSetSd0CtrlSelect2(Value);
        } else {
            AmbaCSL_RctSetSd2CtrlSelect2(Value);
        }
        break;
    case AMBA_SD_PHY_SEL3:
        if (SdChanNo == AMBA_SD_CHANNEL0) {
            AmbaCSL_RctSetSd0CtrlSelect3(Value);
        } else {
            AmbaCSL_RctSetSd2CtrlSelect3(Value);
        }
        break;
    default:
        // For Misrac check
        AmbaMisra_TouchUnused(&Type);
        break;
    }
}

/**
 *  AmbaRTSL_SDPhyGetSelect -
 *  @param[in] SdChanNo SD Channel Number
 *  @param[in] Type
 *  @return
 */
UINT8 AmbaRTSL_SDPhyGetSelect(UINT32 SdChanNo, UINT32 Type)
{
    UINT8 Rval = 0x0;

    switch(Type) {
    case AMBA_SD_PHY_SEL0:
        if (SdChanNo == AMBA_SD_CHANNEL0) {
            Rval = AmbaCSL_RctGetSd0CtrlSelect0();
        } else {
            Rval = AmbaCSL_RctGetSd2CtrlSelect0();
        }
        break;
#if !defined(CONFIG_SOC_CV28) && !defined (CONFIG_SOC_CV5)&& !defined (CONFIG_SOC_CV52)
    case AMBA_SD_PHY_SEL1:
        if (SdChanNo == AMBA_SD_CHANNEL0) {
            Rval = AmbaCSL_RctGetSd0CtrlSelect1();
        } else {
            Rval = AmbaCSL_RctGetSd2CtrlSelect1();
        }
        break;
#endif
    case AMBA_SD_PHY_SEL2:
        if (SdChanNo == AMBA_SD_CHANNEL0) {
            Rval = AmbaCSL_RctGetSd0CtrlSelect2();
        } else {
            Rval = AmbaCSL_RctGetSd2CtrlSelect2();
        }
        break;
    case AMBA_SD_PHY_SEL3:
        if (SdChanNo == AMBA_SD_CHANNEL0) {
            Rval = AmbaCSL_RctGetSd0CtrlSelect3();
        } else {
            Rval = AmbaCSL_RctGetSd2CtrlSelect3();
        }
        break;
    default:
        // For Misrac check
        AmbaMisra_TouchUnused(&Type);
        break;
    }

    return Rval;
}

/**
 *  AmbaRTSL_SDPhyReset -
 *  @param[in] SdChanNo SD Channel Number
 */
void AmbaRTSL_SDPhyReset(UINT32 SdChanNo)
{
    if (SdChanNo == AMBA_SD_CHANNEL0) {
        AmbaCSL_RctSetSd0CtrlPhy0(0x0);
        AmbaCSL_RctSetSd0CtrlClkOutBypass(0x1);
        AmbaCSL_RctSetSd0CtrlDataCmdBypass(0x1);
        AmbaCSL_RctSetSd0CtrlDllBypass(0x1);
    } else {
        AmbaCSL_RctSetSd2CtrlPhy0(0x0);
        AmbaCSL_RctSetSd2CtrlClkOutBypass(0x1);
        AmbaCSL_RctSetSd2CtrlDataCmdBypass(0x1);
        AmbaCSL_RctSetSd2CtrlDllBypass(0x1);
    }
}

/**
 *  AmbaRTSL_SDPhyDisable -
 *  @param[in] SdChanNo SD Channel Number
 */
void AmbaRTSL_SDPhyDisable(UINT32 SdChanNo)
{
    if (SdChanNo == AMBA_SD_CHANNEL0) {
        AmbaRTSL_SDPhyReset(AMBA_SD_CHANNEL0);
        AmbaCSL_SDSetRdLatencyCtrl(AMBA_SD_CHANNEL0, 0);
    } else {
        AmbaRTSL_SDPhyReset(AMBA_SD_CHANNEL1);
        AmbaCSL_SDSetRdLatencyCtrl(AMBA_SD_CHANNEL1, 0);
    }
}

/**
 *  AmbaRTSL_SDPhySelReset -
 *  @param[in] SdChanNo SD Channel Number
 */
void AmbaRTSL_SDPhySelReset(UINT32 SdChanNo)
{
    if (SdChanNo == AMBA_SD_CHANNEL0) {
        AmbaCSL_RctSetSd0CtrlSelect(0x0);
    } else {
        AmbaCSL_RctSetSd2CtrlSelect(0x0);
    }
}

/**
 *  AmbaRTSL_SDSetRdLatencyCtrl -
 *  @param[in] SdChanNo SD Channel Number
 *  @param[in] DelayCycle
 */
void AmbaRTSL_SDSetRdLatencyCtrl(UINT32 SdChanNo, UINT8 DelayCycle)
{
    AmbaCSL_SDSetRdLatencyCtrl(SdChanNo, DelayCycle);
}

/**
 *  AmbaRTSL_SDGetRdLatencyCtrl -
 *  @param[in] SdChanNo SD Channel Number
 *  @return
 */
UINT32 AmbaRTSL_SDGetRdLatencyCtrl(UINT32 SdChanNo)
{
    return AmbaCSL_SDGetRdLatencyCtrl(SdChanNo);
}
