/**
 *  @file AmbaRTSL_VIN.c
 *
 *  @copyright Copyright (c) 2020 Ambarella International LP
 *
 *  This file and its contents ("Software") are protected by intellectual
 *  property rights including, without limitation, U.S. and/or foreign
 *  copyrights. This Software is also the confidential and proprietary
 *  information of Ambarella International LP and its licensors. You may not use, reproduce,
 *  disclose, distribute, modify, or otherwise prepare derivative works of this
 *  Software or any portion thereof except pursuant to a signed license agreement
 *  or nondisclosure agreement with Ambarella International LP or its authorized affiliates.
 *  In the absence of such an agreement, you agree to promptly notify and return
 *  this Software to Ambarella International LP.
 *
 *  THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
 *  INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF NON-INFRINGEMENT,
 *  MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 *  IN NO EVENT SHALL AMBARELLA INTERNATIONAL LP OR ITS AFFILIATES BE LIABLE FOR ANY DIRECT,
 *  INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 *  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 *  LOSS OF USE, DATA, OR PROFITS; COMPUTER FAILURE OR MALFUNCTION; OR BUSINESS
 *  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 *  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 *  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *  POSSIBILITY OF SUCH DAMAGE.
 *
 *  @details Video Input RTSL Device Driver
 *
 */

#include "AmbaTypes.h"
#include "AmbaDef.h"
#include "AmbaMisraFix.h"

#include "AmbaRTSL_VIN.h"
#include "AmbaCSL_DebugPort.h"
#include "AmbaCSL_VIN.h"
#include "AmbaCSL_PLL.h"
#include "AmbaCSL_Scratchpad.h"

#define VIN_MIPI_VC_REG_MAX_VALUE     0x3U
#define VIN_MIPI_DT_REG_MAX_VALUE     0x3fU
#define VIN_NUM_ACT_LANE_MIN_VALUE    1U
#define VIN_NUM_ACT_LANE_MAX_VALUE    12U

/**
 *  AmbaRTSL_VinInit - VIN initializations
 */
void AmbaRTSL_VinInit(void)
{
    /* select VIN clk, 1=ref_clk */
    AmbaCSL_VinSetRefClkSel(0);

    /* configure default ib_ctrl value */
    AmbaCSL_VinSetLvdsIbCtrl(2);

    /* configure default LVDS termination */
    AmbaCSL_VinSetLvdsRsel(AMBA_VIN_TERMINATION_VALUE_DEFAULT);

    /* configure clock and data lanes as LVCMOS mode by default */
    AmbaCSL_VinSetLvCmosMode(0xfffU);
    AmbaCSL_VinSetLvCmosModeSpclk(0x7U);

    return;
}

/**
 *  AmbaRTSL_VinReset - Reset VIN controller
 *  @param[in] VinID Indicate VIN channel
 *  @param[in] ResetFlag Reset flag (0 or 1)
 *  @return error code
 */
UINT32 AmbaRTSL_VinReset(UINT32 VinID, UINT8 ResetFlag)
{
    UINT32 RetVal = VIN_ERR_NONE;

    /* Jenn-Yuan said VLSI team confirms that each VIN reset on CV2FS can be issued at the same time
     * or sequentially, but not overlapped (which could be very rare timing). And if we need to reset
     * multiple VIN back to back, we need to put a small delay (< 1ms) between section resets, or reset
     * them at the same time with a single write to pAmbaIDSP_CtrlReg->SectionReset. */
    if (ResetFlag == 0U) {
        AmbaCSL_IdspReleaseResetAll();
    } else if (ResetFlag == 1U) {
        if (VinID == AMBA_VIN_CHANNEL0) {
            AmbaCSL_IdspResetVin0PipelineSection();
        } else if (VinID == AMBA_VIN_CHANNEL1) {
            AmbaCSL_IdspResetVin1PipelineSection();
        } else if (VinID == AMBA_VIN_CHANNEL2) {
            AmbaCSL_IdspResetVin2PipelineSection();
        } else if (VinID == AMBA_VIN_CHANNEL3) {
            AmbaCSL_IdspResetVin3PipelineSection();
        } else if (VinID == AMBA_VIN_CHANNEL4) {
            AmbaCSL_IdspResetVin4PipelineSection();
        } else {    /* (VinID == AMBA_VIN_CHANNEL8) */
            AmbaCSL_IdspResetVin8PipelineSection();
        }
        AmbaDelayCycles(0x3fffU);
    } else {
        RetVal = VIN_ERR_ARG;
    }

    return RetVal;
}

/**
 *  AmbaRTSL_VinSetLvdsPadMode - Set Lvds Pad mode
 *  @param[in] VinID Indicate VIN channel
 *  @param[in] PadMode Indicate pad mode
 *  @param[in] EnabledPin Enabled data pins defined in bit-wise
 *  @return error code
 */
UINT32 AmbaRTSL_VinSetLvdsPadMode(UINT32 VinID, UINT32 PadMode, UINT32 EnabledPin)
{
    UINT32 RetVal = VIN_ERR_NONE;

    if ((VinID == AMBA_VIN_CHANNEL0) ||
        (VinID == AMBA_VIN_CHANNEL2) ||
        (VinID == AMBA_VIN_CHANNEL3) ||
        (VinID == AMBA_VIN_CHANNEL4)) {
        RetVal = AmbaCSL_VinSetVin0LvdsPadMode(PadMode, EnabledPin);
    } else if (VinID == AMBA_VIN_CHANNEL1) {
        RetVal = AmbaCSL_VinSetVin1LvdsPadMode(PadMode, EnabledPin);
    } else {    /* (VinID == AMBA_VIN_CHANNEL8) */
        RetVal = AmbaCSL_VinSetVin8LvdsPadMode(PadMode, EnabledPin);
    }

    return RetVal;
}

/**
 *  AmbaRTSL_VinConfigMipiPhy - Configure MIPI PHY
 *  @param[in] VinID Indicate VIN channel
 *  @param[in] pVinMipiTiming Pointer to MIPI timing configuration
 *  @param[in] ClkMode Indicate MIPI clock behavior
 *  @param[in] EnabledPin Enabled data pins defined in bit-wise
 *  @return error code
 */
UINT32 AmbaRTSL_VinConfigMipiPhy(UINT32 VinID, const AMBA_VIN_MIPI_TIMING_PARAM_s* pVinMipiTiming, UINT32 ClkMode, UINT32 EnabledPin)
{
    UINT32 RetVal = VIN_ERR_NONE;
    UINT8  Value  = (ClkMode == AMBA_VIN_MIPI_CLK_MODE_CONTINUOUS) ? 1U : 0U;

    if ((VinID == AMBA_VIN_CHANNEL0) ||
        (VinID == AMBA_VIN_CHANNEL2) ||
        (VinID == AMBA_VIN_CHANNEL3) ||
        (VinID == AMBA_VIN_CHANNEL4)) {

        AmbaCSL_VinSetMipiForceClkHs0(Value);

        AmbaCSL_VinSetMipiHsSettle0(pVinMipiTiming->HsSettleTime);
        AmbaCSL_VinSetMipiHsTerm0(pVinMipiTiming->HsTermTime);
        AmbaCSL_VinSetMipiClkSettle0(pVinMipiTiming->ClkSettleTime);
        AmbaCSL_VinSetMipiClkTerm0(pVinMipiTiming->ClkTermTime);
        AmbaCSL_VinSetMipiClkMiss0(pVinMipiTiming->ClkMissTime);
        AmbaCSL_VinSetMipiRxInit0(pVinMipiTiming->RxInitTime);

        if ((EnabledPin & 0xf0U) != 0U) {
            AmbaCSL_VinSetMipiForceClkHs1(Value);

            AmbaCSL_VinSetMipiHsSettle1(pVinMipiTiming->HsSettleTime);
            AmbaCSL_VinSetMipiHsTerm1(pVinMipiTiming->HsTermTime);
            AmbaCSL_VinSetMipiClkSettle1(pVinMipiTiming->ClkSettleTime);
            AmbaCSL_VinSetMipiClkTerm1(pVinMipiTiming->ClkTermTime);
            AmbaCSL_VinSetMipiClkMiss1(pVinMipiTiming->ClkMissTime);
            AmbaCSL_VinSetMipiRxInit1(pVinMipiTiming->RxInitTime);
        }
    } else if (VinID == AMBA_VIN_CHANNEL8) {
        AmbaCSL_VinSetMipiForceClkHs1(Value);

        AmbaCSL_VinSetMipiHsSettle1(pVinMipiTiming->HsSettleTime);
        AmbaCSL_VinSetMipiHsTerm1(pVinMipiTiming->HsTermTime);
        AmbaCSL_VinSetMipiClkSettle1(pVinMipiTiming->ClkSettleTime);
        AmbaCSL_VinSetMipiClkTerm1(pVinMipiTiming->ClkTermTime);
        AmbaCSL_VinSetMipiClkMiss1(pVinMipiTiming->ClkMissTime);
        AmbaCSL_VinSetMipiRxInit1(pVinMipiTiming->RxInitTime);
    } else {    /* (VinID == AMBA_VIN_CHANNEL1) */
        AmbaCSL_VinSetMipiForceClkHs2(Value);

        AmbaCSL_VinSetMipiHsSettle2(pVinMipiTiming->HsSettleTime);
        AmbaCSL_VinSetMipiHsTerm2(pVinMipiTiming->HsTermTime);
        AmbaCSL_VinSetMipiClkSettle2(pVinMipiTiming->ClkSettleTime);
        AmbaCSL_VinSetMipiClkTerm2(pVinMipiTiming->ClkTermTime);
        AmbaCSL_VinSetMipiClkMiss2(pVinMipiTiming->ClkMissTime);
        AmbaCSL_VinSetMipiRxInit2(pVinMipiTiming->RxInitTime);
    }

    return RetVal;
}

/**
 *  AmbaRTSL_VinResetHvSyncOutput - Reset master H/V Sync generator
 *  @param[in] MSyncID Indicate Master sync pin ID
 *  @return error code
 */
UINT32 AmbaRTSL_VinResetHvSyncOutput(UINT32 MSyncID)
{
    UINT32 RetVal = VIN_ERR_NONE;

    if (MSyncID == AMBA_VIN_MSYNC0) {
        AmbaCSL_IdspResetVin0MasterSyncGeneration(1U);
    } else { /* AMBA_VIN_MSYNC1 */
        AmbaCSL_IdspResetVin1MasterSyncGeneration(1U);
    }

    return RetVal;
}

/**
 *  AmbaRTSL_VinSlvsConfig - Configure VIN to receive data via SLVS or Sub-LVDS interface
 *  @param[in] VinID Indicate VIN channel
 *  @param[in] pVinSlvsConfig Pointer to SLVS configuration
 *  @return error code
 */
UINT32 AmbaRTSL_VinSlvsConfig(UINT32 VinID, const AMBA_VIN_SLVS_CONFIG_s *pVinSlvsConfig)
{
    UINT32 RetVal;

    (void)AmbaCSL_VinSlvsConfigGlobal(VinID);
    RetVal = AmbaCSL_VinSlvsConfigMain(VinID, pVinSlvsConfig);

    return RetVal;
}

/**
 *  AmbaRTSL_VinMipiConfig - Configure VIN to receive data via MIPI interface
 *  @param[in] VinID Indicate VIN channel
 *  @param[in] pVinMipiConfig Pointer to MIPI configuration
 *  @return error code
 */
UINT32 AmbaRTSL_VinMipiConfig(UINT32 VinID, const AMBA_VIN_MIPI_CONFIG_s *pVinMipiConfig)
{
    UINT32 RetVal;

    if ((VinID == AMBA_VIN_CHANNEL0) ||
        (VinID == AMBA_VIN_CHANNEL2) ||
        (VinID == AMBA_VIN_CHANNEL3) ||
        (VinID == AMBA_VIN_CHANNEL4)) {
        if (pVinMipiConfig->NumActiveLanes <= 4U) {
            (void)AmbaCSL_VinMipiConfigGlobal(VinID, 1U);
        } else {
            (void)AmbaCSL_VinMipiConfigGlobal(VinID, 0U);
        }
    } else {
        (void)AmbaCSL_VinMipiConfigGlobal(VinID, 0U);
    }

    RetVal = AmbaCSL_VinMipiConfigMain(VinID, pVinMipiConfig);

    return RetVal;
}

/**
 *  AmbaRTSL_VinDvpConfig - Configure VIN to receive data via digital video port (DVP) parallel interface
 *  @param[in] VinID Indicate VIN channel
 *  @param[in] pVinDvpConfig Pointer to DVP configuration
 *  @return error code
 */
UINT32 AmbaRTSL_VinDvpConfig(UINT32 VinID, const AMBA_VIN_DVP_CONFIG_s *pVinDvpConfig)
{
    UINT32 RetVal;

    (void)AmbaCSL_VinDvpConfigGlobal(VinID);
    RetVal = AmbaCSL_VinDvpConfigMain(VinID, pVinDvpConfig);

    return RetVal;
}

/**
 *  AmbaRTSL_VinMasterSyncConfig - Configure master H/V Sync generator
 *  @param[in] MSyncID Indicate Master sync pin ID
 *  @param[in] pMSyncConfig Pointer to H/V Sync configuration
 *  @return error code
 */
UINT32 AmbaRTSL_VinMasterSyncConfig(UINT32 MSyncID, const AMBA_VIN_MASTER_SYNC_CONFIG_s *pMSyncConfig)
{
    UINT32 RetVal;

    if (pMSyncConfig == NULL) {
        RetVal = VIN_ERR_ARG;
    } else {
        if ((pMSyncConfig->HSync.Period >= AMBA_VIN_MSYNC_PERIOD_MIN) &&
            (pMSyncConfig->VSync.Period >= AMBA_VIN_MSYNC_PERIOD_MIN) &&
            (pMSyncConfig->HSync.PulseWidth >= AMBA_VIN_MSYNC_WIDTH_MIN) &&
            (pMSyncConfig->HSync.PulseWidth <= AMBA_VIN_MSYNC_WIDTH_MAX) &&
            (pMSyncConfig->VSync.PulseWidth >= AMBA_VIN_MSYNC_WIDTH_MIN) &&
            (pMSyncConfig->VSync.PulseWidth <= AMBA_VIN_MSYNC_WIDTH_MAX)) {
            RetVal = AmbaCSL_VinMasterSyncConfig(MSyncID, pMSyncConfig);
        } else {
            RetVal = VIN_ERR_ARG;
        }
    }

    return RetVal;
}

/**
 *  AmbaRTSL_VinResetMipiLogic - Reset MIPI logic
 *  @param[in] VinID Indicate VIN channel
 *  @return error code
 */
UINT32 AmbaRTSL_VinResetMipiLogic(UINT32 VinID)
{
    UINT32 RetVal = VIN_ERR_NONE;
    UINT32 TmpUINT32 = 0U;

    /* dphy_rst_afe reset all clock divider in the hardened afe block.
     * When clock disappear and back, there's chance that these divider
     * are not working fine. Theoretically all these divider need to be
     * reset after clock is back. It affect MIPI D-PHY, LVDS, and SLVS mode.
     *
     * dphy_rst only reset the digital logic and only affect MIPI D-PHY mode. */
    if ((VinID == AMBA_VIN_CHANNEL0) ||
        (VinID == AMBA_VIN_CHANNEL2) ||
        (VinID == AMBA_VIN_CHANNEL3) ||
        (VinID == AMBA_VIN_CHANNEL4)) {
        if ((AmbaCSL_VinGetMipiMode() & 0x1U) == 0x1U) {
            if (AmbaCSL_VinGetLvdsBitMode0() == 0x0U) {
                TmpUINT32 |= 0x240U;
            }
        }
        if ((AmbaCSL_VinGetMipiMode() & 0x2U) == 0x2U) {
            if (AmbaCSL_VinGetLvdsBitMode1() == 0x0U) {
                TmpUINT32 |= 0x480U;
            }
        }
    } else if (VinID == AMBA_VIN_CHANNEL1) {
        if ((AmbaCSL_VinGetMipiMode() & 0x4U) == 0x4U) {
            if (AmbaCSL_VinGetLvdsBitMode2() == 0x1U) {
                TmpUINT32 |= 0x900U;
            }
        }
    } else {    /* (VinID == AMBA_VIN_CHANNEL8) */
        if ((AmbaCSL_VinGetMipiMode() & 0x2U) == 0x2U) {
            if (AmbaCSL_VinGetLvdsBitMode1() == 0x1U) {
                TmpUINT32 |= 0x480U;
            }
        }
    }

    /* toggle dphy_rst_afe and dphy_rst together */
    AmbaCSL_VinSetMipiCtrl0(AmbaCSL_VinGetMipiCtrl0() | TmpUINT32);
    AmbaDelayCycles(0xfffU);
    AmbaCSL_VinSetMipiCtrl0(AmbaCSL_VinGetMipiCtrl0() & ~TmpUINT32);

    /* toggle dphy_rst only */
    TmpUINT32 &= 0x1c0U;
    AmbaCSL_VinSetMipiCtrl0(AmbaCSL_VinGetMipiCtrl0() | TmpUINT32);
    AmbaDelayCycles(0xfffU);
    AmbaCSL_VinSetMipiCtrl0(AmbaCSL_VinGetMipiCtrl0() & ~TmpUINT32);

    return RetVal;
}

/**
 *  AmbaRTSL_VinResetSlvsPhy - Reset Slvs PHY
 *  @param[in] VinID Indicate VIN channel
 *  @return error code
 */
UINT32 AmbaRTSL_VinResetSlvsPhy(UINT32 VinID)
{
    UINT32 RetVal = VIN_ERR_NONE;
    UINT32 TmpUINT32 = 0U;

    if (VinID == AMBA_VIN_CHANNEL0) {
        if ((AmbaCSL_VinGetMipiMode() & 0x1U) == 0x0U) {
            if (AmbaCSL_VinGetLvdsBitMode0() == 0x0U) {
                TmpUINT32 |= 0x200U;
            }
        }
        if ((AmbaCSL_VinGetMipiMode() & 0x2U) == 0x0U) {
            if (AmbaCSL_VinGetLvdsBitMode1() == 0x0U) {
                TmpUINT32 |= 0x400U;
            }
        }
        if ((AmbaCSL_VinGetMipiMode() & 0x4U) == 0x0U) {
            if (AmbaCSL_VinGetLvdsBitMode2() == 0x0U) {
                TmpUINT32 |= 0x800U;
            }
        }
    } else if (VinID == AMBA_VIN_CHANNEL1) {
        if ((AmbaCSL_VinGetMipiMode() & 0x4U) == 0x0U) {
            if (AmbaCSL_VinGetLvdsBitMode2() == 0x1U) {
                TmpUINT32 |= 0x800U;
            }
        }
    } else {    /* (VinID == AMBA_VIN_CHANNEL8) */
        if ((AmbaCSL_VinGetMipiMode() & 0x2U) == 0x0U) {
            if (AmbaCSL_VinGetLvdsBitMode1() == 0x1U) {
                TmpUINT32 |= 0x400U;
            }
        }
    }

    AmbaCSL_VinSetMipiCtrl0(AmbaCSL_VinGetMipiCtrl0() | TmpUINT32);
    AmbaDelayCycles(0xfffU);
    AmbaCSL_VinSetMipiCtrl0(AmbaCSL_VinGetMipiCtrl0() & ~TmpUINT32);

    return RetVal;
}

/**
 *  AmbaRTSL_VinSetLvdsTermination - Set MIPI/SLVS/LVDS termination
 *  @param[in] VinID Indicate VIN channel
 *  @param[in] Value Indicate the impendance
 *  @return error code
 */
UINT32 AmbaRTSL_VinSetLvdsTermination(UINT32 VinID, UINT32 Value)
{
    UINT32 RetVal = VIN_ERR_NONE;

    AmbaMisra_TouchUnused(&VinID);
    AmbaCSL_VinSetLvdsRsel((UINT8)Value);

    return RetVal;
}

/*
 *  @RoutineName:: AmbaRTSL_VinDelayedVSyncEnable
 *
 *  @Description:: Config Delayed HSYNC/VSYNC
 *
 *  @Input      ::
 *      pDelayedVSyncConfig:  Configuration of the DelayedVSync
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *       UINT32 :  VIN_ERR_NONE(0) / NG
 */
UINT32 AmbaRTSL_VinDelayedVSyncEnable(const AMBA_VIN_DELAYED_VSYNC_CONFIG_s *pDelayedVSyncConfig)
{
    UINT32 RetVal = VIN_ERR_NONE;
    static UINT8 InitDone = 0U;
    UINT32 RefClk;
    UINT32 VsdelaySrc;

    if (pDelayedVSyncConfig == NULL) {
        RetVal = VIN_ERR_ARG;
    } else {
        RefClk = pDelayedVSyncConfig->FineAdjust.RefClk;
        VsdelaySrc = pDelayedVSyncConfig->DelayedHVsyncSource;

        /* Boundary Check for H/Vsync Delay Period and Pulse Width */
        if ((pDelayedVSyncConfig->VsDlyPeriod[0] > AMBA_VIN_VSDLY_TIME_MAX) ||
            (pDelayedVSyncConfig->VsDlyPeriod[1] > AMBA_VIN_VSDLY_TIME_MAX)) {
            RetVal = VIN_ERR_ARG;
        } else {
            if (RefClk != 0U) {
                if ((pDelayedVSyncConfig->FineAdjust.HsDlyClk > AMBA_VIN_VSDLY_TIME_MAX) ||
                    (pDelayedVSyncConfig->FineAdjust.VsDlyClk[0] > AMBA_VIN_VSDLY_TIME_MAX) ||
                    (pDelayedVSyncConfig->FineAdjust.VsDlyClk[1] > AMBA_VIN_VSDLY_TIME_MAX)) {
                    RetVal = VIN_ERR_ARG;
                } else if ((pDelayedVSyncConfig->FineAdjust.HsPulseWidth > AMBA_VIN_VSDLY_WIDTH_MAX) ||
                           (pDelayedVSyncConfig->FineAdjust.VsPulseWidth[0] > AMBA_VIN_VSDLY_WIDTH_MAX) ||
                           (pDelayedVSyncConfig->FineAdjust.VsPulseWidth[1] > AMBA_VIN_VSDLY_WIDTH_MAX)) {
                    RetVal = VIN_ERR_ARG;
                } else {
                    /* Boundary check passed */
                }
            }
        }

        /* Vsdelay Config */
        if (RetVal == VIN_ERR_NONE) {
            AmbaCSL_SetVsdelayUpdateConfig(1);
            AmbaCSL_SetVsdelayEnable();

            if(InitDone == 0U) {
                InitDone = 1U;

                /* DelayVsync Source Configuration */
                RetVal |= AmbaCSL_SetVsdelaySrc(VsdelaySrc);

                /* Delay period Configuration */
                AmbaCSL_SetVsdelayVsync0Dly(pDelayedVSyncConfig->VsDlyPeriod[0]);
                AmbaCSL_SetVsdelayVsync1Dly(pDelayedVSyncConfig->VsDlyPeriod[1]);

                /* Polarity Configuration */
                AmbaCSL_SetVsdelayHsyncPolInput(pDelayedVSyncConfig->Polarity.HsPolInput);
                AmbaCSL_SetVsdelayVsyncPolInput(pDelayedVSyncConfig->Polarity.VsPolInput);
                AmbaCSL_SetVsdelayHsyncPolOutput(pDelayedVSyncConfig->Polarity.HsPolOutput);
                AmbaCSL_SetVsdelayVsync0PolOutput(pDelayedVSyncConfig->Polarity.VsPolOutput[0]);
                AmbaCSL_SetVsdelayVsync1PolOutput(pDelayedVSyncConfig->Polarity.VsPolOutput[1]);

                /* Fine-adjust delay period / pulse width configuration */
                if (RefClk != 0U) {
                    AmbaCSL_SetVsdelayHsyncDlyClk(pDelayedVSyncConfig->FineAdjust.HsDlyClk);
                    AmbaCSL_SetVsdelayVsync0DlyClk(pDelayedVSyncConfig->FineAdjust.VsDlyClk[0]);
                    AmbaCSL_SetVsdelayVsync1DlyClk(pDelayedVSyncConfig->FineAdjust.VsDlyClk[1]);

                    AmbaCSL_SetVsdelayWidth(pDelayedVSyncConfig->FineAdjust.HsPulseWidth, pDelayedVSyncConfig->FineAdjust.VsPulseWidth);
                }
            } else {
                /* Already initialized, only dly_v_pulse is allowed to be changed. */
                AmbaCSL_SetVsdelayVsync0Dly(pDelayedVSyncConfig->VsDlyPeriod[0]);
                AmbaCSL_SetVsdelayVsync1Dly(pDelayedVSyncConfig->VsDlyPeriod[1]);
            }

            AmbaCSL_SetVsdelayUpdateConfig(0);
        }
    }

    return RetVal;
}

/*
 *  @RoutineName:: AmbaRTSL_VinDelayedVSyncDisable
 *
 *  @Description:: Disable Delayed HSYNC/VSYNC
 *
 *  @Input      :: none
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *       UINT32 :  VIN_ERR_NONE(0)
 */
UINT32 AmbaRTSL_VinDelayedVSyncDisable(void)
{
    AmbaCSL_SetVsdelayUpdateConfig(1);
    AmbaCSL_SetVsdelayDisable();
    AmbaCSL_SetVsdelayUpdateConfig(0);

    return VIN_ERR_NONE;
}

/**
 *  AmbaRTSL_VinDataLaneRemap - Remap data lane
 *  @param[in] VinID Indicate VIN channel
 *  @param[in] pLaneRemapConfig Pointer to lane remapping configuration
 *  @return error code
 */
UINT32 AmbaRTSL_VinDataLaneRemap(UINT32 VinID, const AMBA_VIN_LANE_REMAP_CONFIG_s *pLaneRemapConfig)
{
    UINT32 RetVal;

    if (pLaneRemapConfig == NULL) {
        RetVal = VIN_ERR_ARG;
    } else {
        RetVal = AmbaCSL_VinDataLaneRemap(VinID, pLaneRemapConfig);
    }

    return RetVal;
}

/**
 *  AmbaRTSL_VinMipiVirtChanConfig - Configure MIPI virtual channel receiving
 *  @param[in] VinID Indicate VIN channel
 *  @param[in] pMipiVirtChanConfig Pointer to MIPI virtual channel receiving configuration
 *  @return error code
 */
UINT32 AmbaRTSL_VinMipiVirtChanConfig(UINT32 VinID, const AMBA_VIN_MIPI_VC_CONFIG_s *pMipiVirtChanConfig)
{
    UINT32 RetVal;

    if (pMipiVirtChanConfig == NULL) {
        RetVal = VIN_ERR_ARG;
    } else {
        RetVal = AmbaCSL_VinMipiVirtChanConfig(VinID, pMipiVirtChanConfig);
    }

    return RetVal;
}

/**
 *  AmbaRTSL_VinMipiEmbDataConfig - Configure MIPI embedded data capturing
 *  @param[in] VinID Indicate VIN channel
 *  @param[in] pMipiEmbDataConfig Pointer to MIPI embedded data configuration
 *  @return error code
 */
UINT32 AmbaRTSL_VinMipiEmbDataConfig(UINT32 VinID, const AMBA_VIN_MIPI_EMB_DATA_CONFIG_s *pMipiEmbDataConfig)
{
    UINT32 RetVal;

    if (pMipiEmbDataConfig == NULL) {
        RetVal = VIN_ERR_NONE;
    } else {
        if ((pMipiEmbDataConfig->DataType <= VIN_MIPI_DT_REG_MAX_VALUE) &&
            (pMipiEmbDataConfig->DataTypeMask <= VIN_MIPI_DT_REG_MAX_VALUE)) {
            RetVal = AmbaCSL_VinMipiEmbDataConfig(VinID, pMipiEmbDataConfig);
        } else {
            RetVal = VIN_ERR_NONE;
        }
    }

    return RetVal;
}

/**
 *  AmbaRTSL_VinSetSensorClkDrvStr - Set the driving strength of sensor clock
 *  @param[in] Value Indicate the strength
 *  @return error code
 */
UINT32 AmbaRTSL_VinSetSensorClkDrvStr(UINT32 Value)
{
    UINT32 RetVal = VIN_ERR_NONE;

    AmbaCSL_VinSetSensorClkDriveStr(Value);

    return RetVal;
}
