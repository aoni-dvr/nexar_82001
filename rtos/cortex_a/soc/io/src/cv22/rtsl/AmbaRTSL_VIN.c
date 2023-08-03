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

#include "AmbaRTSL_VIN.h"
#include "AmbaCSL_DebugPort.h"
#include "AmbaCSL_VIN.h"


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
    AmbaCSL_VinSetLvCmosMode(0xffU);
    AmbaCSL_VinSetLvCmosModeSpclk(0x3U);

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

    if (VinID >= AMBA_NUM_VIN_CHANNEL) {
        RetVal = VIN_ERR_ARG;
    } else {
        if (ResetFlag == 0U) {
            AmbaCSL_IdspReleaseResetAll();
        } else if (ResetFlag == 1U) {
            if (VinID == AMBA_VIN_CHANNEL0) {
                AmbaCSL_IdspResetVinPipelineSection();
            } else {    /* (VinID == AMBA_VIN_CHANNEL1) */
                AmbaCSL_IdspResetPipPipelineSection();
            }
        } else {
            RetVal = VIN_ERR_ARG;
        }
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

    if (VinID >= AMBA_NUM_VIN_CHANNEL) {
        RetVal = VIN_ERR_ARG;
    } else {
        if (VinID == AMBA_VIN_CHANNEL0) {
            RetVal = AmbaCSL_VinSetVinLvdsPadMode(PadMode, EnabledPin);
        } else { /* (VinID == AMBA_VIN_CHANNEL1) */
            RetVal = AmbaCSL_VinSetPipLvdsPadMode(PadMode, EnabledPin);
        }
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

    if (VinID >= AMBA_NUM_VIN_CHANNEL) {
        RetVal = VIN_ERR_ARG;
    } else {
        if (VinID == AMBA_VIN_CHANNEL0) {
            if (ClkMode == AMBA_VIN_MIPI_CLK_MODE_CONTINUOUS) {
                if ((EnabledPin & 0x0fU) != 0U) {
                    AmbaCSL_VinSetVinMipiForceClkHs(1);
                }
                if ((EnabledPin & 0xf0U) != 0U) {
                    AmbaCSL_VinSetPipMipiForceClkHs(1);
                }
            } else if (ClkMode == AMBA_VIN_MIPI_CLK_MODE_NON_CONTINUOUS) {
                if ((EnabledPin & 0x0fU) != 0U) {
                    AmbaCSL_VinSetVinMipiForceClkHs(0);
                }
                if ((EnabledPin & 0xf0U) != 0U) {
                    AmbaCSL_VinSetPipMipiForceClkHs(0);
                }
            } else {
                RetVal = VIN_ERR_ARG;
            }

            if ((EnabledPin & 0x0fU) != 0U) {
                AmbaCSL_VinSetVinMipiHsSettleTime(pVinMipiTiming->HsSettleTime);
                AmbaCSL_VinSetVinMipiHsTermTime(pVinMipiTiming->HsTermTime);
                AmbaCSL_VinSetVinMipiClkSettleTime(pVinMipiTiming->ClkSettleTime);
                AmbaCSL_VinSetVinMipiClkTermTime(pVinMipiTiming->ClkTermTime);
                AmbaCSL_VinSetVinMipiClkMissTime(pVinMipiTiming->ClkMissTime);
                AmbaCSL_VinSetVinMipiRxInitTime(pVinMipiTiming->RxInitTime);
            }
            if ((EnabledPin & 0xf0U) != 0U) {
                AmbaCSL_VinSetPipMipiHsSettleTime(pVinMipiTiming->HsSettleTime);
                AmbaCSL_VinSetPipMipiHsTermTime(pVinMipiTiming->HsTermTime);
                AmbaCSL_VinSetPipMipiClkSettleTime(pVinMipiTiming->ClkSettleTime);
                AmbaCSL_VinSetPipMipiClkTermTime(pVinMipiTiming->ClkTermTime);
                AmbaCSL_VinSetPipMipiClkMissTime(pVinMipiTiming->ClkMissTime);
                AmbaCSL_VinSetPipMipiRxInitTime(pVinMipiTiming->RxInitTime);
            }
        } else {    /* (VinID == AMBA_VIN_CHANNEL1) */
            if (ClkMode == AMBA_VIN_MIPI_CLK_MODE_CONTINUOUS) {
                AmbaCSL_VinSetPipMipiForceClkHs(1);
            } else if (ClkMode == AMBA_VIN_MIPI_CLK_MODE_NON_CONTINUOUS) {
                AmbaCSL_VinSetPipMipiForceClkHs(0);
            } else {
                RetVal = VIN_ERR_ARG;
            }

            AmbaCSL_VinSetPipMipiHsSettleTime(pVinMipiTiming->HsSettleTime);
            AmbaCSL_VinSetPipMipiHsTermTime(pVinMipiTiming->HsTermTime);
            AmbaCSL_VinSetPipMipiClkSettleTime(pVinMipiTiming->ClkSettleTime);
            AmbaCSL_VinSetPipMipiClkTermTime(pVinMipiTiming->ClkTermTime);
            AmbaCSL_VinSetPipMipiClkMissTime(pVinMipiTiming->ClkMissTime);
            AmbaCSL_VinSetPipMipiRxInitTime(pVinMipiTiming->RxInitTime);
        }
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
        AmbaCSL_IdspResetVinMasterSyncGeneration(1U);
    } else if (MSyncID == AMBA_VIN_MSYNC1) {
        AmbaCSL_IdspResetPipMasterSyncGeneration(1U);
    } else {
        RetVal = VIN_ERR_ARG;
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

    (void)AmbaCSL_VinMipiConfigGlobal(VinID, pVinMipiConfig->VirtChanHDREnable);
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

    RetVal = AmbaCSL_VinMasterSyncConfig(MSyncID, pMSyncConfig);

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

    if (VinID >= AMBA_NUM_VIN_CHANNEL) {
        RetVal = VIN_ERR_ARG;
    } else {
        /* dphy_rst_afe reset all clock divider in the hardened afe block.
         * When clock disappear and back, there's chance that these divider
         * are not working fine. Theoretically all these divider need to be
         * reset after clock is back. It affect MIPI D-PHY, LVDS, and SLVS mode.
         *
         * dphy_rst only reset the digital logic and only affect MIPI D-PHY mode. */
        if (VinID == AMBA_VIN_CHANNEL0) {
            if (AmbaCSL_VinGetEightLaneMipiMode() != 0x0U) {
                TmpUINT32 |= 0x330U;
            } else {
                TmpUINT32 |= 0x110U;
            }
        } else {    /* (VinID == AMBA_VIN_CHANNEL1) */
            if ((AmbaCSL_VinGetMipiMode() & 0x2U) == 0x2U) {
                TmpUINT32 |= 0x220U;
            }
        }

        /* toggle dphy_rst_afe and dphy_rst together */
        AmbaCSL_VinSetMipiCtrl0(AmbaCSL_VinGetMipiCtrl0() | TmpUINT32);
        AmbaDelayCycles(0xfffU);
        AmbaCSL_VinSetMipiCtrl0(AmbaCSL_VinGetMipiCtrl0() & ~TmpUINT32);

        /* toggle dphy_rst only */
        TmpUINT32 &= 0x030U;
        AmbaCSL_VinSetMipiCtrl0(AmbaCSL_VinGetMipiCtrl0() | TmpUINT32);
        AmbaDelayCycles(0xfffU);
        AmbaCSL_VinSetMipiCtrl0(AmbaCSL_VinGetMipiCtrl0() & ~TmpUINT32);
    }

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

    if (VinID >= AMBA_NUM_VIN_CHANNEL) {
        RetVal = VIN_ERR_ARG;
    } else {
        if (VinID == AMBA_VIN_CHANNEL0) {
            if ((AmbaCSL_VinGetMipiMode() & 0x1U) == 0x0U) {
                if ((AmbaCSL_VinGetLvdsBitMode() & 0x1U) == 0x0U) {
                    TmpUINT32 |= 0x100U;
                }
            }
            if ((AmbaCSL_VinGetMipiMode() & 0x2U) == 0x0U) {
                if ((AmbaCSL_VinGetLvdsBitMode() & 0x2U) == 0x2U) {
                    TmpUINT32 |= 0x200U;
                }
            }
        } else {    /* (VinID == AMBA_VIN_CHANNEL1) */
            if ((AmbaCSL_VinGetMipiMode() & 0x2U) == 0x0U) {
                TmpUINT32 |= 0x200U;
            }
        }

        AmbaCSL_VinSetMipiCtrl0(AmbaCSL_VinGetMipiCtrl0() | TmpUINT32);
        AmbaDelayCycles(0xfffU);
        AmbaCSL_VinSetMipiCtrl0(AmbaCSL_VinGetMipiCtrl0() & ~TmpUINT32);
    }

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

    if ((VinID == AMBA_VIN_CHANNEL0) || (VinID == AMBA_VIN_CHANNEL1)) {
        AmbaCSL_VinSetLvdsRsel((UINT8)Value);
    } else {
        RetVal = VIN_ERR_ARG;
    }

    return RetVal;
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

    RetVal = AmbaCSL_VinDataLaneRemap(VinID, pLaneRemapConfig);

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

    RetVal = AmbaCSL_VinMipiVirtChanConfig(VinID, pMipiVirtChanConfig);

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

    if (Value > 3U) {
        RetVal = VIN_ERR_ARG;
    } else {
        AmbaCSL_VinSetSensorClkDriveStr(Value);
    }

    return RetVal;
}
