/**
 *  @file AmbaRTSL_VIN_S.c
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
 *  @details Video Input RTSL Device Driver (shadow version)
 *
 */

#include "AmbaWrap.h"

#include "AmbaRTSL_VIN.h"
#include "AmbaCSL_VIN.h"
#include "AmbaCSL_PLL.h"
#include "AmbaCSL_Scratchpad.h"
#include "AmbaSafety_VIN.h"

#pragma GCC push_options
#pragma GCC optimize ("O0")

#define VIN_MIPI_VC_REG_MAX_VALUE     0x3U
#define VIN_MIPI_DT_REG_MAX_VALUE     0x3fU
#define VIN_NUM_ACT_LANE_MIN_VALUE    1U
#define VIN_NUM_ACT_LANE_MAX_VALUE    12U

/**
 *  AmbaRTSL_VinInit_ - VIN initializations
 */
void AmbaRTSL_VinInit_(void)
{
    const AMBA_MIPI_REG_s *pAddr = &AmbaMIPI_Mem;

    /* do initial clone for MIPI PHY registers */
    if (AmbaWrap_memcpy(&AmbaMIPI_Mem, pAmbaMIPI_Reg, sizeof(AMBA_MIPI_REG_s)) == ERR_NONE) {
        /* select VIN clk, 1=ref_clk */
        // AmbaCSL_VinSetRefClkSel_(0);

        /* configure default ib_ctrl value */
        AmbaCSL_VinSetLvdsIbCtrl_(2);

        /* configure default LVDS termination */
        AmbaCSL_VinSetLvdsRsel_(AMBA_VIN_TERMINATION_VALUE_DEFAULT);

        /* configure clock and data lanes as LVCMOS mode by default */
        AmbaCSL_VinSetLvCmosMode_(0xfffU);
        AmbaCSL_VinSetLvCmosModeSpclk_(0x7U);

        AmbaSafety_VinShadowLock();
        AmbaSafety_VinShadowPhyCfg(0U, pAddr);
        AmbaSafety_VinShadowUnLock();
    }

    return;
}

/**
 *  AmbaRTSL_VinReset_ - Reset VIN controller
 *  @param[in] VinID Indicate VIN channel
 *  @param[in] ResetFlag Reset flag (0 or 1)
 *  @return error code
 */
UINT32 AmbaRTSL_VinReset_(UINT32 VinID, UINT8 ResetFlag)
{
    UINT32 RetVal = VIN_ERR_NONE;

    if (VinID >= AMBA_NUM_VIN_CHANNEL) {
        RetVal = VIN_ERR_ARG;
    } else {
        if (ResetFlag == 0U) {
            // do nothing
        } else if (ResetFlag == 1U) {
            AmbaSafety_VinShadowLock();
            AmbaSafety_VinShadowMainRst(VinID);
            AmbaSafety_VinShadowUnLock();
        } else {
            RetVal = VIN_ERR_ARG;
        }
    }

    return RetVal;
}

/**
 *  AmbaRTSL_VinSetLvdsPadMode_ - Set Lvds Pad mode
 *  @param[in] VinID Indicate VIN channel
 *  @param[in] PadMode Indicate pad mode
 *  @param[in] EnabledPin Enabled data pins defined in bit-wise
 *  @return error code
 */
UINT32 AmbaRTSL_VinSetLvdsPadMode_(UINT32 VinID, UINT32 PadMode, UINT32 EnabledPin)
{
    UINT32 RetVal = VIN_ERR_NONE;
    const AMBA_MIPI_REG_s *pAddr = &AmbaMIPI_Mem;

    if (VinID >= AMBA_NUM_VIN_CHANNEL) {
        RetVal = VIN_ERR_ARG;
    } else {
        if ((VinID == AMBA_VIN_CHANNEL0) ||
            (VinID == AMBA_VIN_CHANNEL2) ||
            (VinID == AMBA_VIN_CHANNEL3) ||
            (VinID == AMBA_VIN_CHANNEL4)) {
            RetVal = AmbaCSL_VinSetVin0LvdsPadMode_(PadMode, EnabledPin);
        } else if (VinID == AMBA_VIN_CHANNEL1) {
            RetVal = AmbaCSL_VinSetVin1LvdsPadMode_(PadMode, EnabledPin);
        } else {    /* (VinID == AMBA_VIN_CHANNEL8) */
            RetVal = AmbaCSL_VinSetVin8LvdsPadMode_(PadMode, EnabledPin);
        }
        AmbaSafety_VinShadowLock();
        AmbaSafety_VinShadowPhyCfg(VinID, pAddr);
        AmbaSafety_VinShadowUnLock();
    }

    return RetVal;
}

/**
 *  AmbaRTSL_VinConfigMipiPhy_ - Configure MIPI PHY
 *  @param[in] VinID Indicate VIN channel
 *  @param[in] pVinMipiTiming Pointer to MIPI timing configuration
 *  @param[in] ClkMode Indicate MIPI clock behavior
 *  @param[in] EnabledPin Enabled data pins defined in bit-wise
 *  @return error code
 */
UINT32 AmbaRTSL_VinConfigMipiPhy_(UINT32 VinID, const AMBA_VIN_MIPI_TIMING_PARAM_s* pVinMipiTiming, UINT32 ClkMode, UINT32 EnabledPin)
{
    UINT32 RetVal = VIN_ERR_NONE;
    const AMBA_MIPI_REG_s *pAddr = &AmbaMIPI_Mem;

    if ((VinID >= AMBA_NUM_VIN_CHANNEL) ||
        (pVinMipiTiming == NULL) ||
        ((ClkMode != AMBA_VIN_MIPI_CLK_MODE_CONTINUOUS) &&
         (ClkMode != AMBA_VIN_MIPI_CLK_MODE_NON_CONTINUOUS)) ||
        (EnabledPin == 0U)) {
        RetVal = VIN_ERR_ARG;
    } else {
        UINT8 Value = (ClkMode == AMBA_VIN_MIPI_CLK_MODE_CONTINUOUS) ? 1U : 0U;

        if ((VinID == AMBA_VIN_CHANNEL0) ||
            (VinID == AMBA_VIN_CHANNEL2) ||
            (VinID == AMBA_VIN_CHANNEL3) ||
            (VinID == AMBA_VIN_CHANNEL4)) {

            if ((EnabledPin & 0x0fU) != 0U) {
                AmbaCSL_VinSetMipiForceClkHs0_(Value);

                AmbaCSL_VinSetMipiHsSettle0_(pVinMipiTiming->HsSettleTime);
                AmbaCSL_VinSetMipiHsTerm0_(pVinMipiTiming->HsTermTime);
                AmbaCSL_VinSetMipiClkSettle0_(pVinMipiTiming->ClkSettleTime);
                AmbaCSL_VinSetMipiClkTerm0_(pVinMipiTiming->ClkTermTime);
                AmbaCSL_VinSetMipiClkMiss0_(pVinMipiTiming->ClkMissTime);
                AmbaCSL_VinSetMipiRxInit0_(pVinMipiTiming->RxInitTime);
            }
            if ((EnabledPin & 0xf0U) != 0U) {
                AmbaCSL_VinSetMipiForceClkHs1_(Value);

                AmbaCSL_VinSetMipiHsSettle1_(pVinMipiTiming->HsSettleTime);
                AmbaCSL_VinSetMipiHsTerm1_(pVinMipiTiming->HsTermTime);
                AmbaCSL_VinSetMipiClkSettle1_(pVinMipiTiming->ClkSettleTime);
                AmbaCSL_VinSetMipiClkTerm1_(pVinMipiTiming->ClkTermTime);
                AmbaCSL_VinSetMipiClkMiss1_(pVinMipiTiming->ClkMissTime);
                AmbaCSL_VinSetMipiRxInit1_(pVinMipiTiming->RxInitTime);
            }
        } else if (VinID == AMBA_VIN_CHANNEL8) {
            AmbaCSL_VinSetMipiForceClkHs1_(Value);

            AmbaCSL_VinSetMipiHsSettle1_(pVinMipiTiming->HsSettleTime);
            AmbaCSL_VinSetMipiHsTerm1_(pVinMipiTiming->HsTermTime);
            AmbaCSL_VinSetMipiClkSettle1_(pVinMipiTiming->ClkSettleTime);
            AmbaCSL_VinSetMipiClkTerm1_(pVinMipiTiming->ClkTermTime);
            AmbaCSL_VinSetMipiClkMiss1_(pVinMipiTiming->ClkMissTime);
            AmbaCSL_VinSetMipiRxInit1_(pVinMipiTiming->RxInitTime);
        } else {    /* (VinID == AMBA_VIN_CHANNEL1) */
            AmbaCSL_VinSetMipiForceClkHs2_(Value);

            AmbaCSL_VinSetMipiHsSettle2_(pVinMipiTiming->HsSettleTime);
            AmbaCSL_VinSetMipiHsTerm2_(pVinMipiTiming->HsTermTime);
            AmbaCSL_VinSetMipiClkSettle2_(pVinMipiTiming->ClkSettleTime);
            AmbaCSL_VinSetMipiClkTerm2_(pVinMipiTiming->ClkTermTime);
            AmbaCSL_VinSetMipiClkMiss2_(pVinMipiTiming->ClkMissTime);
            AmbaCSL_VinSetMipiRxInit2_(pVinMipiTiming->RxInitTime);
        }
        AmbaSafety_VinShadowLock();
        AmbaSafety_VinShadowPhyCfg(VinID, pAddr);
        AmbaSafety_VinShadowUnLock();
    }

    return RetVal;
}

/**
 *  AmbaRTSL_VinSlvsConfig_ - Configure VIN to receive data via SLVS or Sub-LVDS interface
 *  @param[in] VinID Indicate VIN channel
 *  @param[in] pVinSlvsConfig Pointer to SLVS configuration
 *  @return error code
 */
UINT32 AmbaRTSL_VinSlvsConfig_(UINT32 VinID, const AMBA_VIN_SLVS_CONFIG_s *pVinSlvsConfig)
{
    UINT32 RetVal;

    (void)AmbaCSL_VinSlvsConfigGlobal_(VinID);
    RetVal = AmbaCSL_VinSlvsConfigMain_(VinID, pVinSlvsConfig);

    return RetVal;
}

/**
 *  AmbaRTSL_VinMipiConfig_ - Configure VIN to receive data via MIPI interface
 *  @param[in] VinID Indicate VIN channel
 *  @param[in] pVinMipiConfig Pointer to MIPI configuration
 *  @return error code
 */
UINT32 AmbaRTSL_VinMipiConfig_(UINT32 VinID, const AMBA_VIN_MIPI_CONFIG_s *pVinMipiConfig)
{
    UINT32 RetVal;

    if ((VinID == AMBA_VIN_CHANNEL0) ||
        (VinID == AMBA_VIN_CHANNEL2) ||
        (VinID == AMBA_VIN_CHANNEL3) ||
        (VinID == AMBA_VIN_CHANNEL4)) {
        if (pVinMipiConfig->NumActiveLanes <= 4U) {
            (void)AmbaCSL_VinMipiConfigGlobal_(VinID, 1U);
        } else {
            (void)AmbaCSL_VinMipiConfigGlobal_(VinID, 0U);
        }
    } else {
        (void)AmbaCSL_VinMipiConfigGlobal_(VinID, 0U);
    }

    RetVal = AmbaCSL_VinMipiConfigMain_(VinID, pVinMipiConfig);

    return RetVal;
}

/**
 *  AmbaRTSL_VinDvpConfig_ - Configure VIN to receive data via digital video port (DVP) parallel interface
 *  @param[in] VinID Indicate VIN channel
 *  @param[in] pVinDvpConfig Pointer to DVP configuration
 *  @return error code
 */
UINT32 AmbaRTSL_VinDvpConfig_(UINT32 VinID, const AMBA_VIN_DVP_CONFIG_s *pVinDvpConfig)
{
    UINT32 RetVal;

    (void)AmbaCSL_VinDvpConfigGlobal_(VinID);
    RetVal = AmbaCSL_VinDvpConfigMain_(VinID, pVinDvpConfig);

    return RetVal;
}

/**
 *  AmbaRTSL_VinResetMipiLogic_ - Reset MIPI logic
 *  @param[in] VinID Indicate VIN channel
 *  @return error code
 */
UINT32 AmbaRTSL_VinResetMipiLogic_(UINT32 VinID)
{
    UINT32 RetVal = VIN_ERR_NONE;
    UINT32 TmpUINT32 = 0U;
    const AMBA_MIPI_REG_s *pAddr = &AmbaMIPI_Mem;

    if (VinID >= AMBA_NUM_VIN_CHANNEL) {
        RetVal = VIN_ERR_ARG;
    } else {
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
            if ((AmbaCSL_VinGetMipiMode_() & 0x1U) == 0x1U) {
                if (AmbaCSL_VinGetLvdsBitMode0_() == 0x0U) {
                    TmpUINT32 |= 0x240U;
                }
            }
            if ((AmbaCSL_VinGetMipiMode_() & 0x2U) == 0x2U) {
                if (AmbaCSL_VinGetLvdsBitMode1_() == 0x0U) {
                    TmpUINT32 |= 0x480U;
                }
            }
        } else if (VinID == AMBA_VIN_CHANNEL1) {
            if ((AmbaCSL_VinGetMipiMode_() & 0x4U) == 0x4U) {
                if (AmbaCSL_VinGetLvdsBitMode2_() == 0x1U) {
                    TmpUINT32 |= 0x900U;
                }
            }
        } else {    /* (VinID == AMBA_VIN_CHANNEL8) */
            if ((AmbaCSL_VinGetMipiMode_() & 0x2U) == 0x2U) {
                if (AmbaCSL_VinGetLvdsBitMode1_() == 0x1U) {
                    TmpUINT32 |= 0x480U;
                }
            }
        }

        /* toggle dphy_rst_afe and dphy_rst together */
        AmbaCSL_VinSetMipiCtrl0_(AmbaCSL_VinGetMipiCtrl0_() | TmpUINT32);
        // AmbaDelayCycles(0xfffU);
        AmbaCSL_VinSetMipiCtrl0_(AmbaCSL_VinGetMipiCtrl0_() & ~TmpUINT32);

        /* toggle dphy_rst only */
        TmpUINT32 &= 0x1c0U;
        AmbaCSL_VinSetMipiCtrl0_(AmbaCSL_VinGetMipiCtrl0_() | TmpUINT32);
        // AmbaDelayCycles(0xfffU);
        AmbaCSL_VinSetMipiCtrl0_(AmbaCSL_VinGetMipiCtrl0_() & ~TmpUINT32);
        AmbaSafety_VinShadowLock();
        AmbaSafety_VinShadowPhyCfg(VinID, pAddr);
        AmbaSafety_VinShadowUnLock();
    }

    return RetVal;
}

/**
 *  AmbaRTSL_VinResetSlvsPhy_ - Reset Slvs PHY
 *  @param[in] VinID Indicate VIN channel
 *  @return error code
 */
UINT32 AmbaRTSL_VinResetSlvsPhy_(UINT32 VinID)
{
    UINT32 RetVal = VIN_ERR_NONE;
    UINT32 TmpUINT32 = 0U;
    const AMBA_MIPI_REG_s *pAddr = &AmbaMIPI_Mem;

    if (VinID >= AMBA_NUM_VIN_CHANNEL) {
        RetVal = VIN_ERR_ARG;
    } else {
        if (VinID == AMBA_VIN_CHANNEL0) {
            if ((AmbaCSL_VinGetMipiMode_() & 0x1U) == 0x0U) {
                if (AmbaCSL_VinGetLvdsBitMode0_() == 0x0U) {
                    TmpUINT32 |= 0x200U;
                }
            }
            if ((AmbaCSL_VinGetMipiMode_() & 0x2U) == 0x0U) {
                if (AmbaCSL_VinGetLvdsBitMode1_() == 0x0U) {
                    TmpUINT32 |= 0x400U;
                }
            }
            if ((AmbaCSL_VinGetMipiMode_() & 0x4U) == 0x0U) {
                if (AmbaCSL_VinGetLvdsBitMode2_() == 0x0U) {
                    TmpUINT32 |= 0x800U;
                }
            }
        } else if (VinID == AMBA_VIN_CHANNEL1) {
            if ((AmbaCSL_VinGetMipiMode_() & 0x4U) == 0x0U) {
                if (AmbaCSL_VinGetLvdsBitMode2_() == 0x1U) {
                    TmpUINT32 |= 0x800U;
                }
            }
        } else {    /* (VinID == AMBA_VIN_CHANNEL8) */
            if ((AmbaCSL_VinGetMipiMode_() & 0x2U) == 0x0U) {
                if (AmbaCSL_VinGetLvdsBitMode1_() == 0x1U) {
                    TmpUINT32 |= 0x400U;
                }
            }
        }

        AmbaCSL_VinSetMipiCtrl0_(AmbaCSL_VinGetMipiCtrl0_() | TmpUINT32);
        // AmbaDelayCycles(0xfffU);
        AmbaCSL_VinSetMipiCtrl0_(AmbaCSL_VinGetMipiCtrl0_() & ~TmpUINT32);
        AmbaSafety_VinShadowLock();
        AmbaSafety_VinShadowPhyCfg(VinID, pAddr);
        AmbaSafety_VinShadowUnLock();
    }

    return RetVal;
}

/**
 *  AmbaRTSL_VinSetLvdsTermination_ - Set MIPI/SLVS/LVDS termination
 *  @param[in] VinID Indicate VIN channel
 *  @param[in] Value Indicate the impendance
 *  @return error code
 */
UINT32 AmbaRTSL_VinSetLvdsTermination_(UINT32 VinID, UINT32 Value)
{
    UINT32 RetVal = VIN_ERR_NONE;
    const AMBA_MIPI_REG_s *pAddr = &AmbaMIPI_Mem;

    if ((VinID == AMBA_VIN_CHANNEL0) || (VinID == AMBA_VIN_CHANNEL1) || (VinID == AMBA_VIN_CHANNEL8)) {
        AmbaCSL_VinSetLvdsRsel_((UINT8)Value);
        AmbaSafety_VinShadowLock();
        AmbaSafety_VinShadowPhyCfg(VinID, pAddr);
        AmbaSafety_VinShadowUnLock();
    } else {
        RetVal = VIN_ERR_ARG;
    }

    return RetVal;
}

/**
 *  AmbaRTSL_VinDataLaneRemap_ - Remap data lane
 *  @param[in] VinID Indicate VIN channel
 *  @param[in] pLaneRemapConfig Pointer to lane remapping configuration
 *  @return error code
 */
UINT32 AmbaRTSL_VinDataLaneRemap_(UINT32 VinID, const AMBA_VIN_LANE_REMAP_CONFIG_s *pLaneRemapConfig)
{
    UINT32 RetVal;

    if (pLaneRemapConfig == NULL) {
        RetVal = VIN_ERR_ARG;
    } else {
        if ((pLaneRemapConfig->NumActiveLanes >= VIN_NUM_ACT_LANE_MIN_VALUE) &&
            (pLaneRemapConfig->NumActiveLanes <= VIN_NUM_ACT_LANE_MAX_VALUE)) {
            RetVal = AmbaCSL_VinDataLaneRemap_(VinID, pLaneRemapConfig);
        } else {
            RetVal = VIN_ERR_ARG;
        }
    }

    return RetVal;
}

/**
 *  AmbaRTSL_VinMipiVirtChanConfig_ - Configure MIPI virtual channel receiving
 *  @param[in] VinID Indicate VIN channel
 *  @param[in] pMipiVirtChanConfig Pointer to MIPI virtual channel receiving configuration
 *  @return error code
 */
UINT32 AmbaRTSL_VinMipiVirtChanConfig_(UINT32 VinID, const AMBA_VIN_MIPI_VC_CONFIG_s *pMipiVirtChanConfig)
{
    UINT32 RetVal;

    if (pMipiVirtChanConfig == NULL) {
        RetVal = VIN_ERR_ARG;
    } else {
        if ((pMipiVirtChanConfig->VirtChan <= VIN_MIPI_VC_REG_MAX_VALUE) &&
            (pMipiVirtChanConfig->VirtChanMask <= VIN_MIPI_VC_REG_MAX_VALUE)) {
            RetVal = AmbaCSL_VinMipiVirtChanConfig_(VinID, pMipiVirtChanConfig);
        } else {
            RetVal = VIN_ERR_ARG;
        }
    }

    return RetVal;
}

/**
 *  AmbaRTSL_VinMipiEmbDataConfig_ - Configure MIPI embedded data capturing
 *  @param[in] VinID Indicate VIN channel
 *  @param[in] pMipiEmbDataConfig Pointer to MIPI embedded data configuration
 *  @return error code
 */
UINT32 AmbaRTSL_VinMipiEmbDataConfig_(UINT32 VinID, const AMBA_VIN_MIPI_EMB_DATA_CONFIG_s *pMipiEmbDataConfig)
{
    UINT32 RetVal;

    if (pMipiEmbDataConfig == NULL) {
        RetVal = VIN_ERR_NONE;
    } else {
        if ((pMipiEmbDataConfig->DataType <= VIN_MIPI_DT_REG_MAX_VALUE) &&
            (pMipiEmbDataConfig->DataTypeMask <= VIN_MIPI_DT_REG_MAX_VALUE)) {
            RetVal = AmbaCSL_VinMipiEmbDataConfig_(VinID, pMipiEmbDataConfig);
        } else {
            RetVal = VIN_ERR_NONE;
        }
    }

    return RetVal;
}

#pragma GCC pop_options
