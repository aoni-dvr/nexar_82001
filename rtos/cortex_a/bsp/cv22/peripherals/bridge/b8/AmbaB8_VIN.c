/**
 *  @file AmbaB8_VIN.c
 *
 *  @copyright Copyright (c) 2015 Ambarella, Inc.
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
 *  @details Ambarella B8 Vin APIs
 *
 */

#include "AmbaB8.h"
#include "AmbaB8_Communicate.h"
#include "AmbaB8_PLL.h"

#include "AmbaB8CSL_VIN.h"
#include "AmbaB8CSL_PHY.h"
#include "AmbaB8CSL_Scratchpad.h"

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: BoundaryCheck
 *
 *  @Description:: Check the boundary for mipi-phy parameters
 *
 *  @Input      ::
 *      Width:    Bus Width
 *      Count:    Count Number
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *                 CalCount: Calculated count number
\*-----------------------------------------------------------------------------------------------*/
static UINT8 BoundaryCheck(UINT32 Width, DOUBLE Count)
{
    UINT32 CalCount;
    DOUBLE MaxVal;

    (void) AmbaB8_Wrap_pow(2.0, (DOUBLE)Width, &MaxVal);
    if (Count > (MaxVal - 1.0)) {
        CalCount = (UINT32)MaxVal - 1U;
    } else if (Count == 0.0) {
        CalCount = 0U;
    } else {
        CalCount = (UINT32)Count - 1U;
    }

    return (UINT8)CalCount;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: B8_CalculateMipiRxParam
 *
 *  @Description:: Calculate mipi rx parameters
 *
 *  @Input      ::
 *      ChipID:        B8 chip id
 *      DataRate:      Input MIPI data rate
 *
 *  @Output     ::
 *      pMipiRxConfig: Pointer to mipi rx configure
 *
 *  @Return     ::
 *          int : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static UINT32 B8_CalculateMipiRxParam(UINT32 ChipID, UINT32 DataRate, B8_VIN_MIPI_RX_CONFIG_s *pMipiRxConfig)
{
    DOUBLE BitRateInMhz, RxRefClkInMhz;
    DOUBLE TxByteClkHS, UI;
    DOUBLE ClkPrepare, ClkPrepareMin, ClkPrepareMax, ClkPrepareSec;
    DOUBLE HsLpxMin;
//    DOUBLE HsTrailMin;
    DOUBLE HsSettle, HsSettleMin, HsSettleMax;
    DOUBLE HsPrepare, HsPrepareMin, HsPrepareMax, HsPrepareSec;
    DOUBLE HsZeroMin, HsZeroMinSec;
    DOUBLE HsTermMax, HsTermSecMax;
    DOUBLE ClkZeroMinSec, ClkZeroMin;
    /* DOUBLE ClkMissMax; */
    DOUBLE ClkSettle, ClkSettleMin;
    DOUBLE ClkTrailMin;
    DOUBLE ClkTermMax;
    DOUBLE RxRefClk, RxDDRClkHS;
    DOUBLE InitTxMin;
    DOUBLE InitRxMin;

    UINT8 HsSettleTime;
    UINT8 HsTermTime;
    UINT8 ClkSettleTime;
    UINT8 ClkTermTime;
    /* UINT8 ClkMissTime; */
    UINT8 RxInitTime;


    (void)AmbaB8_Wrap_ceil(((DOUBLE)DataRate / 1000000.0), &BitRateInMhz);
    (void)AmbaB8_Wrap_ceil(((DOUBLE)AmbaB8_PllGetIdspClk(ChipID) / 1000000.0), &RxRefClkInMhz);

    AmbaB8_PrintUInt5("IdspClk: %d", AmbaB8_PllGetIdspClk(ChipID), 0U, 0U, 0U, 0U);
    AmbaB8_PrintUInt5("MIPI DataRate: %d", DataRate, 0U, 0U, 0U, 0U);

    /********************************************************
    * TX (TxByteClkHS)
    ********************************************************/
    TxByteClkHS = (DOUBLE)BitRateInMhz * (DOUBLE)1e6 / 8.0; // in Hz
    UI = 1.0 / ((DOUBLE)BitRateInMhz * (DOUBLE)1e6);        // in second

    /* ClkPrepare */
    ClkPrepareMin = 38e-9 * TxByteClkHS ;
    (void)AmbaB8_Wrap_floor((ClkPrepareMin + 0.5), &ClkPrepareMin);

    ClkPrepareMax = 95e-9 * TxByteClkHS;
    (void)AmbaB8_Wrap_floor((ClkPrepareMax + 0.5), &ClkPrepareMax);

    ClkPrepareSec = (38e-9 + 95e-9) / 2.0;
    ClkPrepare = ClkPrepareSec * TxByteClkHS;
    (void)AmbaB8_Wrap_floor((ClkPrepare + 0.5), &ClkPrepare);

    /* ClkZero */
    ClkZeroMinSec = (DOUBLE)300e-9 - (ClkPrepare / TxByteClkHS);
    ClkZeroMin = ClkZeroMinSec * TxByteClkHS;
    (void)AmbaB8_Wrap_floor((ClkZeroMin + 0.5), &ClkZeroMin);

    /* ClkTrail */
    ClkTrailMin = 60e-9 * TxByteClkHS;
    (void)AmbaB8_Wrap_floor((ClkTrailMin + 0.5), &ClkTrailMin);

    /* HsLpx */
    HsLpxMin = 50e-9 * TxByteClkHS;
    (void)AmbaB8_Wrap_floor((HsLpxMin + 0.5), &HsLpxMin);

    /* HsTrail */
#if 0
    HsTrailMin = max(8 * UI, 60e-9 + 4 * UI) * TxByteClkHS;
    (void)AmbaB8_Wrap_floor((HsTrailMin + 0.5), &HsTrailMin);
#endif
    /* HsPrepare */
    HsPrepareMin = ((DOUBLE)40e-9 + (4.0 * UI)) * TxByteClkHS;
    (void)AmbaB8_Wrap_floor((HsPrepareMin + 0.5), &HsPrepareMin);

    HsPrepareMax = ((DOUBLE)85e-9 + (6.0 * UI)) * TxByteClkHS;
    (void)AmbaB8_Wrap_floor((HsPrepareMax + 0.5), &HsPrepareMax);

    HsPrepareSec = (((DOUBLE)40e-9 + (4.0 * UI)) + (85e-9 + (6.0 * UI))) / 2.0;
    HsPrepare = HsPrepareSec * TxByteClkHS;
    (void)AmbaB8_Wrap_floor((HsPrepare + 0.5), &HsPrepare);

    /* HsZero */
    HsZeroMinSec = (DOUBLE)145e-9 + (10.0 * UI) - HsPrepareSec;
    HsZeroMin = HsZeroMinSec * TxByteClkHS;
    (void)AmbaB8_Wrap_ceil(HsZeroMin, &HsZeroMin);

    /* InitTx */
    InitTxMin = (DOUBLE)100e-6 * (TxByteClkHS / 1024.0);
    (void)AmbaB8_Wrap_floor((InitTxMin + 0.5), &InitTxMin);

    /********************************************************
    * RX
    * (RxRefClk for clock lane)
    * (RxDDRClkHS for data lane)
    *******************************************************/
    RxRefClk = (RxRefClkInMhz * (DOUBLE)1e6) / 2.0;
    RxDDRClkHS = (BitRateInMhz * (DOUBLE)1e6) / 4.0;

    /* InitRx */
    InitRxMin = ((DOUBLE)100e-6 * RxRefClk) / 1024.0;
    (void)AmbaB8_Wrap_floor((InitRxMin + 0.5), &InitRxMin);
    RxInitTime = BoundaryCheck(7, InitRxMin);

    /* ClkMiss */
    /*
        ClkMissMax = 60e-9 * RxRefClk;
        (void)AmbaB8_Wrap_floor((ClkMissMax + 0.5), &ClkMissMax);
        ClkMissTime = BoundaryCheck(5, ClkMissMax);
    */
    /* ClkTerm */
    ClkTermMax = 38e-9 * RxRefClk;
    (void)AmbaB8_Wrap_floor((ClkTermMax + 0.5), &ClkTermMax);
    ClkTermTime = BoundaryCheck(5, ClkTermMax);

    /* ClkSettle */
    ClkSettleMin = ClkPrepareSec * RxRefClk;
    (void)AmbaB8_Wrap_floor((ClkSettleMin + 0.5), &ClkSettleMin);

    ClkSettle = (ClkPrepareSec + ClkZeroMinSec) * RxRefClk;
    ClkSettleTime = BoundaryCheck(6, ClkSettle);

    /* HsTerm */
    HsTermSecMax = 35e-9 + (4.0 * UI);
    HsTermMax = HsTermSecMax * RxDDRClkHS;
    (void)AmbaB8_Wrap_floor(HsTermMax, &HsTermMax);
    HsTermTime = BoundaryCheck(5, HsTermMax);

    /* HsSettle */
    HsSettleMin = HsPrepareSec * RxDDRClkHS;
    (void)AmbaB8_Wrap_floor((HsSettleMin + 0.5), &HsSettleMin);
    HsSettleMax = (((DOUBLE)145e-9 + (10.0 * UI) - 35e-9) - (4.0 * UI)) * RxDDRClkHS;
    (void)AmbaB8_Wrap_floor(HsSettleMax, &HsSettleMax);

    HsSettle = ((HsPrepareSec - HsTermSecMax) + HsZeroMinSec) * RxDDRClkHS;
    (void)AmbaB8_Wrap_floor(HsSettle, &HsSettle);
    if (HsSettle > HsSettleMax) {
        HsSettle = HsSettleMax;
    }
    HsSettleTime = BoundaryCheck(6, HsSettle);

    pMipiRxConfig->HsSettleTime  = HsSettleTime;
    pMipiRxConfig->HsTermTime    = HsTermTime;
    pMipiRxConfig->ClkSettleTime = ClkSettleTime;
    pMipiRxConfig->ClkTermTime   = ClkTermTime;

    /* There's a ratio requirement: RxDDRClkHS > 2.5 RxRefClk
     * 2.5 is a number with margin. When this ratio not meet, it's quit easy
     * for mipi_dphy to think sensor clock is missing (not toggling).
     * Per VLSI's information on 2019/9/12, we can increase the miss_ctrl as
     * large as we can to allow faster IDSP_clk. It just increased the time for PHY
     * to be aware of real clock disappeared, which is no effect currently. */
    pMipiRxConfig->ClkMissTime   = 0x1f;
    pMipiRxConfig->RxInitTime    = RxInitTime;

    return B8_ERR_NONE;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: B8_VinSetMipiRxParam
 *
 *  @Description:: set mipi rx timing parameters
 *
 *  @Input      ::
 *      ChipID:        B8 chip id
 *      DataRate:      MIPI data rate per lane
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          int : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static UINT32 B8_VinSetMipiRxParam(UINT32 ChipID, UINT32 Channel, UINT32 DataRate)
{
    UINT32 RetVal;
    UINT32 DataBuf32[2U];
    B8_VIN_MIPI_RX_CONFIG_s MipiRxConfig = {0};
    B8_RCT_DPHY_CTRL4_REG_s RctDphyCtrl4Reg = {0};
    B8_RCT_DPHY_CTRL5_REG_s RctDphyCtrl5Reg = {0};

    RetVal = B8_CalculateMipiRxParam(ChipID, DataRate, &MipiRxConfig);
    if (RetVal == B8_ERR_NONE) {
        RctDphyCtrl4Reg.HsTermCtrl0    = MipiRxConfig.HsTermTime;
        RctDphyCtrl4Reg.HsSettleCtrl0  = MipiRxConfig.HsSettleTime;
        RctDphyCtrl4Reg.InitRxCtrl0    = MipiRxConfig.RxInitTime;
        RctDphyCtrl4Reg.ClkMissCtrl0   = MipiRxConfig.ClkMissTime;

        RctDphyCtrl5Reg.ClkSettleCtrl0 = MipiRxConfig.ClkSettleTime;
        RctDphyCtrl5Reg.ClkTermCtrl0   = MipiRxConfig.ClkTermTime;

        (void) AmbaB8_Wrap_memcpy(&DataBuf32[0], &RctDphyCtrl4Reg, sizeof(UINT32));
        (void) AmbaB8_Wrap_memcpy(&DataBuf32[1], &RctDphyCtrl5Reg, sizeof(UINT32));

        if (Channel == B8_VIN_CHANNEL0) {
            (void) AmbaB8_RegWrite(ChipID, & (pAmbaB8_PhyReg->RctDphyCtrl4Reg), 1U, B8_DATA_WIDTH_32BIT, 2U, DataBuf32);
        } else {
            (void) AmbaB8_RegWrite(ChipID, & (pAmbaB8_PhyReg->RctDphyCtrl6Reg), 1U, B8_DATA_WIDTH_32BIT, 2U, DataBuf32);
        }
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: B8_VinCaptureConfig
 *
 *  @Description:: Vin capture configuration
 *
 *  @Input      ::
 *      ChipID:         B8 chip id
 *      pCaptureWindow: Pointer to B8 Capture Window
 *      Channel:        B8 Vin channel
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *      int : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
UINT32 AmbaB8_VinCaptureConfig(UINT32 ChipID, UINT32 Channel, const B8_VIN_WINDOW_s *pCaptureWindow)
{
    UINT32 DataBuf32[4];

    /* Set Crop Region */
    DataBuf32[0] = pCaptureWindow->StartX;
    DataBuf32[1] = pCaptureWindow->StartY;
    DataBuf32[2] = pCaptureWindow->EndX;
    DataBuf32[3] = pCaptureWindow->EndY;
    (void) AmbaB8_RegWrite(ChipID, & (pAmbaB8_VinReg[Channel]->CropStartCol), 1U, B8_DATA_WIDTH_32BIT, 4U, DataBuf32);

    /* Config Done (no matter what value) */
    DataBuf32[0] = 0x0001;
    (void) AmbaB8_RegWrite(ChipID, & (pAmbaB8_VinReg[Channel]->ConfigDone), 0U, B8_DATA_WIDTH_32BIT, 1U, DataBuf32);

    return OK;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaB8_VinReset
 *
 *  @Description:: Reset B8 VIN
 *
 *  @Input      ::
 *      ChipID:          B8 chip id
 *      Channel:         B8 Vin channel
 *      VinInterface:    B8 Vin interface
 *      pVinMipiDphyCtrl Pointer to B8 MIPI DPHY configuration
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *      int : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
UINT32 AmbaB8_VinReset(UINT32 ChipID, UINT32 Channel, UINT32 VinInterface, const B8_VIN_MIPI_DPHY_CTRL_s *pVinMipiDphyCtrl)
{
    UINT32 RetVal;
    UINT32 DataBuf32[4U] = {0};
    B8_RCT_DPHY_CTRL0_REG_s pDphyCtrl0;
    B8_RCT_DPHY_CTRL1_REG_s pDphyCtrl1;
    B8_RCT_DPHY_CTRL2_REG_s pDphyCtrl2;
    B8_RCT_DPHY_CTRL3_REG_s pDphyCtrl3;

    RetVal = AmbaB8_RegRead(ChipID, & (pAmbaB8_PhyReg->RctDphyCtrl0Reg), 1U, B8_DATA_WIDTH_32BIT, 4U, DataBuf32);

    if (RetVal == B8_ERR_NONE) {
        AmbaB8_Misra_TypeCast32(&pDphyCtrl0, &DataBuf32[0]);
        AmbaB8_Misra_TypeCast32(&pDphyCtrl1, &DataBuf32[1]);
        AmbaB8_Misra_TypeCast32(&pDphyCtrl2, &DataBuf32[2]);
        AmbaB8_Misra_TypeCast32(&pDphyCtrl3, &DataBuf32[3]);

        /* mipi protocal timing */
        if (pVinMipiDphyCtrl != NULL) {
            (void) B8_VinSetMipiRxParam(ChipID, Channel, pVinMipiDphyCtrl->DataRate);
            /* alex: No force clock needed(but should be fine) */
            if (pVinMipiDphyCtrl->ClockMode == B8_VIN_MIPI_CONTINUOUS_CLOCK_MODE) {
                if (Channel == B8_VIN_CHANNEL0) {
                    pDphyCtrl2.ForceClkHs0 = 0x1U;
                } else {
                    pDphyCtrl2.ForceClkHs1 = 0x1U;
                }
                (void) AmbaB8_RegWrite(ChipID, & (pAmbaB8_PhyReg->RctDphyCtrl2Reg), 0U, B8_DATA_WIDTH_32BIT, 1U, &pDphyCtrl2);
            }
        }

        if (VinInterface == B8_VIN_MIPI) {
            /* rx termination resistance = 6 */
            pDphyCtrl0.LvdsRsel = 0x6U;
            /* bit mode = 0 */
            pDphyCtrl0.BitMode = 0x0U;
            (void) AmbaB8_RegWrite(ChipID, & (pAmbaB8_PhyReg->RctDphyCtrl0Reg), 0U, B8_DATA_WIDTH_32BIT, 1U, &pDphyCtrl0);

            /* ibias = 2 */
            pDphyCtrl1.LvdsIbCtrl = 0x2U;
            /* termination on */
            pDphyCtrl1.DataR100Off = 0x0U;
            pDphyCtrl1.ClkR100Off = 0x0U;
            (void) AmbaB8_RegWrite(ChipID, & (pAmbaB8_PhyReg->RctDphyCtrl1Reg), 0U, B8_DATA_WIDTH_32BIT, 1U, &pDphyCtrl1);

            /* change to mipi mode */
            if (Channel == B8_VIN_CHANNEL0) {
                pDphyCtrl3.MipiMode |= 0x1U;
            } else {
                pDphyCtrl3.MipiMode |= 0x2U;
            }
            pDphyCtrl3.LvcmosMode = 0x0U;
            pDphyCtrl3.LvcmosModeSpclk = 0x0U;
            (void) AmbaB8_RegWrite(ChipID, & (pAmbaB8_PhyReg->RctDphyCtrl3Reg), 0U, B8_DATA_WIDTH_32BIT, 1U, &pDphyCtrl3);

            /* assert mipi dphy afe reset */
            pDphyCtrl0.DphyRstAfe = 0x1U;
            (void) AmbaB8_RegWrite(ChipID, & (pAmbaB8_PhyReg->RctDphyCtrl0Reg), 0U, B8_DATA_WIDTH_32BIT, 1U, &pDphyCtrl0);
            /* assert mipi dphy reset */
            pDphyCtrl1.DphyRst = 0x3U;
            (void) AmbaB8_RegWrite(ChipID, & (pAmbaB8_PhyReg->RctDphyCtrl1Reg), 0U, B8_DATA_WIDTH_32BIT, 1U, &pDphyCtrl1);

            (void) AmbaB8_KAL_TaskSleep(1);

            /* de-assert dphy reset */
            pDphyCtrl1.DphyRst = 0x0U;
            (void) AmbaB8_RegWrite(ChipID, & (pAmbaB8_PhyReg->RctDphyCtrl1Reg), 0, B8_DATA_WIDTH_32BIT, 1, &pDphyCtrl1);
            /* de-assert mipi dphy afe reset */
            pDphyCtrl0.DphyRstAfe = 0x0U;
            (void) AmbaB8_RegWrite(ChipID, & (pAmbaB8_PhyReg->RctDphyCtrl0Reg), 0U, B8_DATA_WIDTH_32BIT, 1U, &pDphyCtrl0);

        } else if (VinInterface == B8_VIN_LVDS) {
            /* change to slvs mode */
            if (Channel == B8_VIN_CHANNEL0) {
                pDphyCtrl3.MipiMode &= ~0x1U;
            } else {
                pDphyCtrl3.MipiMode &= ~0x2U;
            }
            pDphyCtrl3.LvcmosMode = 0x0U;
            pDphyCtrl3.LvcmosModeSpclk = 0x0U;

            /* rx termination resistance = 6 */
            pDphyCtrl0.LvdsRsel = 0x6U;
            /* bit mode = 3 */
            pDphyCtrl0.BitMode = 0x3U;
            (void) AmbaB8_RegWrite(ChipID, & (pAmbaB8_PhyReg->RctDphyCtrl0Reg), 0U, B8_DATA_WIDTH_32BIT, 1U, &pDphyCtrl0);

            /* ibias = 2 */
            pDphyCtrl1.LvdsIbCtrl = 0x2U;
            /* termination on */
            pDphyCtrl1.DataR100Off = 0x0U;
            pDphyCtrl1.ClkR100Off = 0x0U;
            (void) AmbaB8_RegWrite(ChipID, & (pAmbaB8_PhyReg->RctDphyCtrl1Reg), 0U, B8_DATA_WIDTH_32BIT, 1U, &pDphyCtrl1);

            (void) AmbaB8_RegWrite(ChipID, & (pAmbaB8_PhyReg->RctDphyCtrl3Reg), 0U, B8_DATA_WIDTH_32BIT, 1U, &pDphyCtrl3);
        } else if (VinInterface == B8_VIN_LVCMOS) {
            pDphyCtrl0.Async = 0xffU;
            (void) AmbaB8_RegWrite(ChipID, & (pAmbaB8_PhyReg->RctDphyCtrl0Reg), 0U, B8_DATA_WIDTH_32BIT, 1U, &pDphyCtrl0);

            pDphyCtrl3.LvcmosMode = 0xff;
            pDphyCtrl3.LvcmosModeSpclk = 0x3U;
            pDphyCtrl3.MipiMode = 0x0U;
            (void) AmbaB8_RegWrite(ChipID, & (pAmbaB8_PhyReg->RctDphyCtrl3Reg), 0U, B8_DATA_WIDTH_32BIT, 1U, &pDphyCtrl3);
        } else {
            RetVal = B8_ERR_ARG;
        }
    }

    return RetVal;
}
/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaB8_VinConfigSLVS
 *
 *  @Description:: Transfer user Vin config to B8 Vin register
 *
 *  @Input      ::
 *      ChipID:         B8 chip id
 *      Channel:        B8 Vin channel
 *      pVinSlvsConfig: Pointer to B8 SLVS Configure
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *      int : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
UINT32 AmbaB8_VinConfigSLVS(UINT32 ChipID, UINT32 Channel, const B8_VIN_SLVS_CONFIG_s *pVinSlvsConfig)
{
    UINT32 RetVal = B8_ERR_NONE;
    B8_VIN_GLOBAL_REG_s GlobalConfig = {0U};
    B8_VIN_CTRL_REG_s VinCtrl = {0U};
    UINT32 DataBuf32[16];
    UINT32 VinCtrlVal = 0U;
    B8_VIN_DATA_LANE_PIN_SELECT0_REG_s DataLanePinSelect0 = {0U};
    B8_VIN_DATA_LANE_PIN_SELECT1_REG_s DataLanePinSelect1 = {0U};

    if (pVinSlvsConfig == NULL) {
        RetVal = B8_ERR_ARG;
    } else {
        /* SLVS pin mapping */
        if (pVinSlvsConfig->NumDataLane >= 8U) {
            DataLanePinSelect1.DataLane7PinSelect = pVinSlvsConfig->DataLaneSelect[7];
            VinCtrlVal |= (UINT32)1U << (pVinSlvsConfig->DataLaneSelect[7] + 6U);
        }
        if (pVinSlvsConfig->NumDataLane >= 7U) {
            DataLanePinSelect1.DataLane6PinSelect = pVinSlvsConfig->DataLaneSelect[6];
            VinCtrlVal |= (UINT32)1U << (pVinSlvsConfig->DataLaneSelect[6] + 6U);
        }
        if (pVinSlvsConfig->NumDataLane >= 6U) {
            DataLanePinSelect1.DataLane5PinSelect = pVinSlvsConfig->DataLaneSelect[5];
            VinCtrlVal |= (UINT32)1U << (pVinSlvsConfig->DataLaneSelect[5] + 6U);
        }
        if (pVinSlvsConfig->NumDataLane >= 5U) {
            DataLanePinSelect1.DataLane4PinSelect = pVinSlvsConfig->DataLaneSelect[4];
            VinCtrlVal |= (UINT32)1U << (pVinSlvsConfig->DataLaneSelect[4] + 6U);
        }
        if (pVinSlvsConfig->NumDataLane >= 4U) {
            DataLanePinSelect0.DataLane3PinSelect = pVinSlvsConfig->DataLaneSelect[3];
            VinCtrlVal |= (UINT32)1U << (pVinSlvsConfig->DataLaneSelect[3] + 6U);
        }
        if (pVinSlvsConfig->NumDataLane >= 3U) {
            DataLanePinSelect0.DataLane2PinSelect = pVinSlvsConfig->DataLaneSelect[2];
            VinCtrlVal |= (UINT32)1U << (pVinSlvsConfig->DataLaneSelect[2] + 6U);
        }
        if (pVinSlvsConfig->NumDataLane >= 2U) {
            DataLanePinSelect0.DataLane1PinSelect = pVinSlvsConfig->DataLaneSelect[1];
            VinCtrlVal |= (UINT32)1U << (pVinSlvsConfig->DataLaneSelect[1] + 6U);
        }
        if (pVinSlvsConfig->NumDataLane >= 1U) {
            DataLanePinSelect0.DataLane0PinSelect = pVinSlvsConfig->DataLaneSelect[0];
            VinCtrlVal |= (UINT32)1U << (pVinSlvsConfig->DataLaneSelect[0] + 6U);
        }

        /* Global Config region to select sensor type */
        GlobalConfig.SensorType = 0;
        (void) AmbaB8_RegWrite(ChipID, & (pAmbaB8_VinReg[Channel]->GlobalConfig), 0, B8_DATA_WIDTH_32BIT, 1, &GlobalConfig);

        /* Pin mapping */
        (void) AmbaB8_Wrap_memcpy(&DataBuf32[0], &DataLanePinSelect0, sizeof(UINT32));
        (void) AmbaB8_Wrap_memcpy(&DataBuf32[1], &DataLanePinSelect1, sizeof(UINT32));
        (void) AmbaB8_RegWrite(ChipID, & (pAmbaB8_VinReg[Channel]->DataLanePinSelect0), 1, B8_DATA_WIDTH_32BIT, 2, DataBuf32);

        /* Sync timeout */
        DataBuf32[0] = 1;
        DataBuf32[1] = 0;
        (void) AmbaB8_RegWrite(ChipID, & (pAmbaB8_VinReg[Channel]->SyncDelayL), 1, B8_DATA_WIDTH_32BIT, 2, DataBuf32);

        DataBuf32[0] = 1;
        DataBuf32[1] = 0;
        (void) AmbaB8_RegWrite(ChipID, & (pAmbaB8_VinReg[Channel]->IntrDelayL), 1, B8_DATA_WIDTH_32BIT, 2, DataBuf32);

        /* Sync timeout */
        DataBuf32[0] = 0xFFFF;
        DataBuf32[1] = 0xFFFF;
        (void) AmbaB8_RegWrite(ChipID, & (pAmbaB8_VinReg[Channel]->SyncTimeOut), 1, B8_DATA_WIDTH_32BIT, 2, DataBuf32);

        /* Sync detect */
        DataBuf32[0] = 0x8000;
        DataBuf32[1] = 0x8000;
        (void) AmbaB8_RegWrite(ChipID, & (pAmbaB8_VinReg[Channel]->SlvsSyncDetectMask), 1, B8_DATA_WIDTH_32BIT, 2, DataBuf32);

        /* Sync compare */
        DataBuf32[0] = pVinSlvsConfig->SyncDetectCtrl.CustomSyncCode.SyncCodeMask;
        (void) AmbaB8_RegWrite(ChipID, & (pAmbaB8_VinReg[Channel]->SlvsSyncCompareMask), 0, B8_DATA_WIDTH_32BIT, 1, DataBuf32);

        DataBuf32[0] = pVinSlvsConfig->SyncDetectCtrl.CustomSyncCode.PatternSol;
        DataBuf32[1] = pVinSlvsConfig->SyncDetectCtrl.CustomSyncCode.PatternEol;
        (void) AmbaB8_RegWrite(ChipID, & (pAmbaB8_VinReg[Channel]->SlvsSolPattern), 1, B8_DATA_WIDTH_32BIT, 2, DataBuf32);

        DataBuf32[0] = pVinSlvsConfig->SyncDetectCtrl.CustomSyncCode.PatternSof;
        DataBuf32[1] = pVinSlvsConfig->SyncDetectCtrl.CustomSyncCode.PatternEof;
        (void) AmbaB8_RegWrite(ChipID, & (pAmbaB8_VinReg[Channel]->SlvsSofPattern), 1, B8_DATA_WIDTH_32BIT, 2, DataBuf32);

        DataBuf32[0] = pVinSlvsConfig->SyncDetectCtrl.CustomSyncCode.PatternSov;
        DataBuf32[1] = pVinSlvsConfig->SyncDetectCtrl.CustomSyncCode.PatternEov;
        (void) AmbaB8_RegWrite(ChipID, & (pAmbaB8_VinReg[Channel]->SlvsSovPattern), 1, B8_DATA_WIDTH_32BIT, 2, DataBuf32);

        /* SLVS control */
        DataBuf32[0] = ((UINT32)0x8U | pVinSlvsConfig->SyncDetectCtrl.SyncInterleaving);
        DataBuf32[1] = 0x1DCU;
        (void) AmbaB8_RegWrite(ChipID, & (pAmbaB8_VinReg[Channel]->SlvsSyncCodeLocation), 1, B8_DATA_WIDTH_32BIT, 2, DataBuf32);

        DataBuf32[0] = ((UINT32)pVinSlvsConfig->SyncDetectCtrl.CustomSyncCode.DetectEnable.Eov << 5U) |
                       ((UINT32)pVinSlvsConfig->SyncDetectCtrl.CustomSyncCode.DetectEnable.Sov << 4U) |
                       ((UINT32)pVinSlvsConfig->SyncDetectCtrl.CustomSyncCode.DetectEnable.Eof << 3U) |
                       ((UINT32)pVinSlvsConfig->SyncDetectCtrl.CustomSyncCode.DetectEnable.Sof << 2U) |
                       ((UINT32)pVinSlvsConfig->SyncDetectCtrl.CustomSyncCode.DetectEnable.Eol << 1U) |
                       ((UINT32)pVinSlvsConfig->SyncDetectCtrl.CustomSyncCode.DetectEnable.Sol);
        (void) AmbaB8_RegWrite(ChipID, & (pAmbaB8_VinReg[Channel]->SlvsSyncCodeDetect), 0, B8_DATA_WIDTH_32BIT, 1, DataBuf32);

        /* Active Region Size */
        DataBuf32[0] = pVinSlvsConfig->RxHvSyncCtrl.NumActivePixels - 1U;
        DataBuf32[1] = pVinSlvsConfig->RxHvSyncCtrl.NumActiveLines - 1U;
        (void) AmbaB8_RegWrite(ChipID, & (pAmbaB8_VinReg[Channel]->ActiveRegionWidth), 1, B8_DATA_WIDTH_32BIT, 2, DataBuf32);

        /* Split Width */
        DataBuf32[0] = pVinSlvsConfig->SplitCtrl.SplitWidth;
        (void) AmbaB8_RegWrite(ChipID, & (pAmbaB8_VinReg[Channel]->LineSplitWidth), 0, B8_DATA_WIDTH_32BIT, 1, DataBuf32);

        /* VIN Control register */
        (void) AmbaB8_Wrap_memcpy(&VinCtrl, &VinCtrlVal, sizeof(UINT32));
        VinCtrl.SwReset = 1U;
        VinCtrl.Enable = 1U;
        VinCtrl.OutputEnable = 1U;
        VinCtrl.BitsPerPixel = (pVinSlvsConfig->NumDataBits - 8U) >> 1U;

        (void) AmbaB8_Wrap_memcpy(&DataBuf32[0], &VinCtrl, sizeof(UINT32));
        (void) AmbaB8_RegWrite(ChipID, & (pAmbaB8_VinReg[Channel]->Ctrl), 0, B8_DATA_WIDTH_32BIT, 1, DataBuf32);
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaB8_VinConfigMIPI
 *
 *  @Description:: Transfer user Vin config to B8 Vin register
 *
 *  @Input      ::
 *      ChipID:         B8 chip id
 *      Channel:        B8 Vin channel
 *      pVinMipiConfig: Pointer to B8 MIPI Configure
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *      int : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
UINT32 AmbaB8_VinConfigMIPI(UINT32 ChipID, UINT32 Channel, const B8_VIN_MIPI_CONFIG_s *pVinMipiConfig)
{
    UINT32 RetVal = B8_ERR_NONE;
    B8_VIN_GLOBAL_REG_s GlobalConfig = {0U};
    B8_VIN_MIPI_DATA_FILTER_REG_s MipiDataFilter = {0U};
    B8_VIN_MIPI_CTRL_REG_s MipiCtrl = {0U};
    B8_VIN_CTRL_REG_s VinCtrl = {0U};
    UINT32 DataLaneEnable;
    UINT32 DataBuf32[4];

    if (pVinMipiConfig == NULL) {
        RetVal = B8_ERR_ARG;
    } else {
        /* Global Config region to select sensor type */
        GlobalConfig.SensorType = 0x2U;
        (void) AmbaB8_RegWrite(ChipID, & (pAmbaB8_VinReg[Channel]->GlobalConfig), 0, B8_DATA_WIDTH_32BIT, 1, &GlobalConfig);

        /* MIPI Control */
        MipiDataFilter.VirtChanMask    = 0x3U;
        MipiDataFilter.VirtChanPattern = 0x0U;
        MipiDataFilter.DataTypePattern = pVinMipiConfig->DataType;
        MipiDataFilter.DataTypeMask    = pVinMipiConfig->DataTypeMask;

        MipiCtrl.DeCompressionEnable = 0U;
        MipiCtrl.DeCompressionMode = 0U;
        MipiCtrl.ByteSwapEnable = 0U;
        MipiCtrl.EccEnable = 1U;
        MipiCtrl.FwdErrPkt = 1U;

        (void) AmbaB8_Wrap_memcpy(&DataBuf32[0], &MipiDataFilter, sizeof(B8_VIN_MIPI_DATA_FILTER_REG_s));
        (void) AmbaB8_Wrap_memcpy(&DataBuf32[1], &MipiCtrl, sizeof(B8_VIN_MIPI_CTRL_REG_s));
        (void) AmbaB8_RegWrite(ChipID, & (pAmbaB8_VinReg[Channel]->MipiDataFilter), 1, B8_DATA_WIDTH_32BIT, 2, DataBuf32);

        /* Active Region Size */
        DataBuf32[0] = pVinMipiConfig->RxHvSyncCtrl.NumActivePixels - 1U;
        DataBuf32[1] = pVinMipiConfig->RxHvSyncCtrl.NumActiveLines - 1U;
        (void) AmbaB8_RegWrite(ChipID, & (pAmbaB8_VinReg[Channel]->ActiveRegionWidth), 1, B8_DATA_WIDTH_32BIT, 2, DataBuf32);

        /* VIN Control register */
        DataLaneEnable = ((UINT32)1U << pVinMipiConfig->NumActiveLanes) - 1U;
        VinCtrl.SwReset = 1U;
        VinCtrl.Enable = 1U;
        VinCtrl.OutputEnable = 1U;
        VinCtrl.BitsPerPixel = (pVinMipiConfig->NumDataBits - 8U) >> 1U;
        VinCtrl.DataLane0Enable = (UINT8)(DataLaneEnable & 0x1U);
        VinCtrl.DataLane1Enable = (UINT8)((DataLaneEnable >> 1U) & 0x1U);
        VinCtrl.DataLane2Enable = (UINT8)((DataLaneEnable >> 2U) & 0x1U);
        VinCtrl.DataLane3Enable = (UINT8)((DataLaneEnable >> 3U) & 0x1U);

        (void) AmbaB8_Wrap_memcpy(&DataBuf32[0], &VinCtrl, sizeof(B8_VIN_CTRL_REG_s));
        (void) AmbaB8_RegWrite(ChipID, & (pAmbaB8_VinReg[Channel]->Ctrl), 0, B8_DATA_WIDTH_32BIT, 1, DataBuf32);
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaB8_VinConfigDVP
 *
 *  @Description:: Transfer user Vin config to B8 Vin register
 *
 *  @Input      ::
 *      ChipID:         B8 chip id
 *      Channel:        B8 Vin channel
 *      pVinDvpConfig:  Pointer to B8 DVP Configure
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *      UINT32 : B8_ERR_NONE(0)
\*-----------------------------------------------------------------------------------------------*/
UINT32 AmbaB8_VinConfigDVP(UINT32 ChipID, UINT32 Channel, const B8_VIN_DVP_CONFIG_s *pVinDvpConfig)
{
    UINT32 RetVal = B8_ERR_NONE;
    B8_VIN_GLOBAL_REG_s GlobalConfig = {0U};
    B8_VIN_CTRL_REG_s VinCtrl = {0U};
    B8_VIN_DVP_CTRL_REG_s DvpCtrl = {0U};
    B8_VIN_SYNC_PIN_SELECT_REG_s SyncPinSelect = {0U};
    UINT32 DataBuf32[4];

    if (pVinDvpConfig == NULL) {
        RetVal = B8_ERR_ARG;
    } else {
        /* Global Config region to select sensor type */
        GlobalConfig.SensorType = 0x1U;
        (void) AmbaB8_RegWrite(ChipID, & (pAmbaB8_VinReg[Channel]->GlobalConfig), 0U, B8_DATA_WIDTH_32BIT, 1U, &GlobalConfig);

        /* VIN Control register */
        VinCtrl.SwReset = 1U;
        VinCtrl.Enable = 1U;
        VinCtrl.OutputEnable = 1U;

        /* Parallel LVCMOS */
        DvpCtrl.PadType = 0U;

        if (pVinDvpConfig->DvpWide == B8_VIN_DVP_WIDE_1_PIXEL) {
            DvpCtrl.ParallelWidth = 0U;
        } else if (pVinDvpConfig->DvpWide == B8_VIN_DVP_WIDE_2_PIXELS) {
            DvpCtrl.ParallelWidth = 1U;
        } else {
            RetVal = B8_ERR_ARG;
        }

        /* Data latch edge */
        if (pVinDvpConfig->DataLatchEdge == B8_VIN_DVP_LATCH_EDGE_RISING) {
            DvpCtrl.ParallelDataRate = 0U;  /* SDR */
            DvpCtrl.ParallelDataEdge = 0U;
        } else if (pVinDvpConfig->DataLatchEdge == B8_VIN_DVP_LATCH_EDGE_FALLING) {
            DvpCtrl.ParallelDataRate = 0U;  /* SDR */
            DvpCtrl.ParallelDataEdge = 1U;
        } else if (pVinDvpConfig->DataLatchEdge == B8_VIN_DVP_LATCH_EDGE_BOTH) {
            DvpCtrl.ParallelDataRate = 1U;  /* DDR */
        } else {
            RetVal = B8_ERR_ARG;
        }

        /* YUV */
        if (pVinDvpConfig->ColorSpace == B8_VIN_COLOR_SPACE_YUV) {
            DvpCtrl.YuvEnable = 1U;
            DvpCtrl.YuvOrder = pVinDvpConfig->YuvOrder;
        } else {
            DvpCtrl.YuvEnable = 0U;
        }

        DvpCtrl.FieldMode = 0U;

        /* BT.601 Sync */
        DvpCtrl.SyncType = 1U;
        DvpCtrl.HsyncPolarity = pVinDvpConfig->SyncDetectCtrl.SyncPinConfig.HsyncPolarity;
        DvpCtrl.VsyncPolarity = pVinDvpConfig->SyncDetectCtrl.SyncPinConfig.VsyncPolarity;
        DvpCtrl.FieldPolarity = pVinDvpConfig->SyncDetectCtrl.SyncPinConfig.FieldPolarity;
        SyncPinSelect.FieldPinSelect = 2U;
        SyncPinSelect.VsyncPinSelect = 1U;
        SyncPinSelect.HsyncPinSelect = 0U;

        (void) AmbaB8_Wrap_memcpy(&DataBuf32[0], &VinCtrl, sizeof(B8_VIN_CTRL_REG_s));
        (void) AmbaB8_Wrap_memcpy(&DataBuf32[1], &DvpCtrl, sizeof(B8_VIN_DVP_CTRL_REG_s));
        (void) AmbaB8_Wrap_memcpy(&DataBuf32[2], &SyncPinSelect, sizeof(B8_VIN_SYNC_PIN_SELECT_REG_s));
        (void) AmbaB8_RegWrite(ChipID, & (pAmbaB8_VinReg[Channel]->Ctrl), 1U, B8_DATA_WIDTH_32BIT, 3U, DataBuf32);

        /* Active Region Size */
        DataBuf32[0] = pVinDvpConfig->RxHvSyncCtrl.NumActivePixels - 1U;
        DataBuf32[1] = pVinDvpConfig->RxHvSyncCtrl.NumActiveLines - 1U;
        (void) AmbaB8_RegWrite(ChipID, & (pAmbaB8_VinReg[Channel]->ActiveRegionWidth), 1U, B8_DATA_WIDTH_32BIT, 2U, DataBuf32);
        /*(TODO) Crop region */
        (void) AmbaB8_RegWrite(ChipID, & (pAmbaB8_VinReg[Channel]->CropEndCol), 1U, B8_DATA_WIDTH_32BIT, 2U, DataBuf32);

        /* Should be larger than 1, otherwise VIN will be dead */
        DataBuf32[0] = 1U;
        DataBuf32[1] = 0U;
        DataBuf32[2] = 1U;
        DataBuf32[3] = 0U;
        (void) AmbaB8_RegWrite(ChipID, & (pAmbaB8_VinReg[Channel]->SyncDelayL), 1U, B8_DATA_WIDTH_32BIT, 4U, DataBuf32);
    }

    return RetVal;
}


/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaB8_VinConfigMasterSync
 *
 *  @Description:: Vin master sync configuration
 *
 *  @Input      ::
 *      ChipID:            B8 chip id
 *      pMasterSyncConfig: Pointer to master sync configuration
 *      Channel:           B8 Vin channel
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *      int : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
UINT32 AmbaB8_VinConfigMasterSync(UINT32 ChipID, UINT32 MSyncChan, const B8_VIN_MASTER_SYNC_CONFIG_s *pMasterSyncConfig)
{
    B8_VIN_MASTER_SYNC_CTRL_REG_s MasterSyncCtrl;
    UINT32 Data32Buf[8];

    MasterSyncCtrl.HsyncPolarity  = pMasterSyncConfig->HSync.Polarity;
    MasterSyncCtrl.VsyncPolarity  = pMasterSyncConfig->VSync.Polarity;
    MasterSyncCtrl.NoVblankHsync  = ~pMasterSyncConfig->ToggleHsyncInVblank;
    MasterSyncCtrl.InterruptMode  = 0U;
    MasterSyncCtrl.VSyncWidthUnit = 0U;
    MasterSyncCtrl.NumVsyncs      = 1U;
    MasterSyncCtrl.Continuous     = 1U;
    MasterSyncCtrl.Preempt        = 1U;

    Data32Buf[0] = pMasterSyncConfig->HSync.Period & 0xffffU;
    Data32Buf[1] = (pMasterSyncConfig->HSync.Period >> 16U) & 0xffffU;
    Data32Buf[2] = pMasterSyncConfig->HSync.PulseWidth;
    Data32Buf[3] = pMasterSyncConfig->HSyncDelayCycles;
    Data32Buf[4] = pMasterSyncConfig->VSync.Period;
    Data32Buf[5] = pMasterSyncConfig->VSync.PulseWidth;
    Data32Buf[6] = pMasterSyncConfig->VSyncDelayCycles;
    (void) AmbaB8_Wrap_memcpy(&Data32Buf[7], &MasterSyncCtrl, sizeof(B8_VIN_MASTER_SYNC_CTRL_REG_s));

    /* Enable Master Sync */
    (void) AmbaB8_RegWrite(ChipID, & (pAmbaB8_VinReg[MSyncChan]->HsyncPeriodL), 1, B8_DATA_WIDTH_32BIT, 8, Data32Buf);

    return B8_ERR_NONE;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaB8_VinConfigDelayedMasterSync
 *
 *  @Description:: Delayed Master Sync configuration
 *
 *  @Input      ::
 *                Chan:   delayed master sync channel
 *      pVinSyncConfig:   pointer to delayed Master sync config
 *
 *  @Output     :: none
 *
 *  @Return     :: OK/NG
\*-----------------------------------------------------------------------------------------------*/
UINT32 AmbaB8_VinConfigDelayedMSync(UINT32 ChipID, UINT32 DelayedMSyncChan, const B8_VIN_DELAYED_MASTER_SYNC_CONFIG_s *pSyncConfig)
{
    UINT32 RetVal = B8_ERR_NONE;
    B8_SCRATCHPAD_SYNC_CFG_REG_s SyncConfig;
    B8_SCRATCHPAD_SYNC_DELAY_REG_s SyncDelay;

    (void) AmbaB8_RegRead(ChipID, & (pAmbaB8_ScratchpadReg->SyncConfig), 0, B8_DATA_WIDTH_32BIT, 1, &SyncConfig);
    (void) AmbaB8_RegRead(ChipID, & (pAmbaB8_ScratchpadReg->SyncDelay), 0, B8_DATA_WIDTH_32BIT, 1, &SyncDelay);

    /* select Vsync source */
    if (pSyncConfig->VsyncSource == B8_DELAYED_MASTER_SYNC_SOURCE_VIN) {
        SyncConfig.VsyncExtSel = 0U;
        SyncConfig.VsyncMasterVin1Sel = 0U;
    } else if (pSyncConfig->VsyncSource == B8_DELAYED_MASTER_SYNC_SOURCE_PIP) {
        SyncConfig.VsyncExtSel = 0U;
        SyncConfig.VsyncMasterVin1Sel = 1U;
    } else if (pSyncConfig->VsyncSource == B8_DELAYED_MASTER_SYNC_SOURCE_EXTERNAL) {
        SyncConfig.VsyncExtSel = 1U;
    } else {
        RetVal = B8_ERR_ARG;
    }

    /* select Hsync source */
    if (pSyncConfig->HsyncSource == B8_DELAYED_MASTER_SYNC_SOURCE_VIN) {
        SyncConfig.HsyncExtSel = 0U;
        SyncConfig.HsyncMasterVin1Sel = 0U;
    } else if (pSyncConfig->HsyncSource == B8_DELAYED_MASTER_SYNC_SOURCE_PIP) {
        SyncConfig.HsyncExtSel = 0U;
        SyncConfig.HsyncMasterVin1Sel = 1U;
    } else if (pSyncConfig->HsyncSource == B8_DELAYED_MASTER_SYNC_SOURCE_EXTERNAL) {
        SyncConfig.HsyncExtSel = 1U;
    } else {
        RetVal = B8_ERR_ARG;
    }

    if (DelayedMSyncChan == B8_DELAYED_MASTER_SYNC_CHANNEL0) {
        /* delay control */
        SyncDelay.Dly0 = pSyncConfig->DelayTimeInHsync;
        if (pSyncConfig->DelayTimeInHsync > 0U) {
            SyncDelay.DlyMult0 = pSyncConfig->DelayTimeMultiplier;
        }
        /* force V/Hsync_delay0 to output to V/HSYNC0 */
        SyncConfig.Vsync0Sel = 2U;
        if ((pSyncConfig->HsyncSource == B8_DELAYED_MASTER_SYNC_SOURCE_VIN) ||
            (pSyncConfig->HsyncSource == B8_DELAYED_MASTER_SYNC_SOURCE_EXTERNAL)) {
            SyncConfig.Hsync0Sel = 0U;
        } else {
            SyncConfig.Hsync0Sel = 1U;
        }
    } else if (DelayedMSyncChan == B8_DELAYED_MASTER_SYNC_CHANNEL1) {
        /* delay control */
        SyncDelay.Dly1 = pSyncConfig->DelayTimeInHsync;
        if (pSyncConfig->DelayTimeInHsync > 0U) {
            SyncDelay.DlyMult1 = pSyncConfig->DelayTimeMultiplier;
        }
        /* force V/Hsync_delay1 to output to V/HSYNC1 */
        SyncConfig.Vsync1Sel = 3U;
        if ((pSyncConfig->HsyncSource == B8_DELAYED_MASTER_SYNC_SOURCE_PIP) ||
            (pSyncConfig->HsyncSource == B8_DELAYED_MASTER_SYNC_SOURCE_EXTERNAL)) {
            SyncConfig.Hsync1Sel = 1U;
        } else {
            SyncConfig.Hsync1Sel = 0U;
        }
    } else {
        RetVal = B8_ERR_ARG;
    }

    if (RetVal == B8_ERR_NONE) {
        (void) AmbaB8_RegWrite(ChipID, & (pAmbaB8_ScratchpadReg->SyncConfig), 0, B8_DATA_WIDTH_32BIT, 1, &SyncConfig);
        (void) AmbaB8_RegWrite(ChipID, & (pAmbaB8_ScratchpadReg->SyncDelay), 0, B8_DATA_WIDTH_32BIT, 1, &SyncDelay);
    }

    return RetVal;
}

void AmbaB8_VinDPhyReset(UINT32 ChipID, UINT8 Reset)
{
    B8_RCT_DPHY_CTRL0_REG_s RctDphyCtrl0Reg;
    B8_RCT_DPHY_CTRL1_REG_s RctDphyCtrl1Reg;

    (void) AmbaB8_RegRead(ChipID, & (pAmbaB8_PhyReg->RctDphyCtrl0Reg), 0, B8_DATA_WIDTH_32BIT, 1, &RctDphyCtrl0Reg);
    (void) AmbaB8_RegRead(ChipID, & (pAmbaB8_PhyReg->RctDphyCtrl1Reg), 0, B8_DATA_WIDTH_32BIT, 1, &RctDphyCtrl1Reg);

    if (Reset == 0U) {
        /* de-assert DPHY reset */
        RctDphyCtrl1Reg.DphyRst = 0x0U;
        (void) AmbaB8_RegWrite(ChipID, & (pAmbaB8_PhyReg->RctDphyCtrl1Reg), 0, B8_DATA_WIDTH_32BIT, 1, &RctDphyCtrl1Reg);

        AmbaB8_PrintUInt5("de-assert B8 Vin DPHY @ ChipID=0x%x", ChipID, 0U, 0U, 0U, 0U);
    } else {
        /* assert dphy reset */
        RctDphyCtrl1Reg.DphyRst = 0x3U;
        (void) AmbaB8_RegWrite(ChipID, & (pAmbaB8_PhyReg->RctDphyCtrl1Reg), 0, B8_DATA_WIDTH_32BIT, 1, &RctDphyCtrl1Reg);

        AmbaB8_PrintUInt5("assert B8 Vin DPHY @ ChipID=0x%x", ChipID, 0U, 0U, 0U, 0U);
    }

}

UINT32 AmbaB8_VinWaitMipiSignalLock(UINT32 ChipID, UINT32 Channel, UINT32 TimeOut)
{
    B8_DPHY_RCT_OBSV0_REG_s DphyRctObsv0Reg = {0};
    UINT32 RetVal = B8_ERR_NONE;
    UINT32 MipiRxClkActiveHs = 0U;

    /* Wait for D-PHY lock signal */
    while ((MipiRxClkActiveHs == 0U) && (TimeOut > 0U)) {
        (void) AmbaB8_RegRead(ChipID, & pAmbaB8_PhyReg->DphyRctObsv0Reg, 0, B8_DATA_WIDTH_32BIT, 1, &DphyRctObsv0Reg);

        if (Channel == B8_VIN_CHANNEL0) {
            MipiRxClkActiveHs = DphyRctObsv0Reg.Rxclkactivehs0;
        } else { /* B8_VIN_CHANNEL1 */
            MipiRxClkActiveHs = DphyRctObsv0Reg.Rxclkactivehs1;
        }

        (void) AmbaKAL_TaskSleep(1U);
        TimeOut--;
    }
    if (TimeOut == 0U) {
        RetVal = B8_ERR_TIMEOUT;
    }

    return RetVal;
}

