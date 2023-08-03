/**
 *  @file AmbaB8_VOUT.c
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
 *  @details Ambarella B8 Vout APIs
 *
 */
#include "AmbaB8.h"
#include "AmbaB8_Communicate.h"
#include "AmbaB8_PLL.h"
#include "AmbaB8_Packer.h"
#include "AmbaB8_Scratchpad.h"

#include "AmbaB8CSL_PLL.h"
#include "AmbaB8CSL_PHY.h"
#include "AmbaB8CSL_VOUT.h"
#include "AmbaB8CSL_Scratchpad.h"

#if 0
static AMBA_B8_VOUT_CSC_MATRIX_s DefaultCsc = {
    /* Limited to Full */
    .Coef = {
        [0] = {  1.16438f, -0.39200f, -0.81300f },
        [1] = {  1.16438f,  2.01700f,  0.00000f },
        [2] = {  1.16438f,  0.00000f,  1.59600f },
    },
    .Offset = { [0] =  135.61f, [1] = -276.81f, [2] = -222.92f },
    .MinVal = { [0] = 0U,       [1] = 0U,       [2] = 0U       },
    .MaxVal = { [0] = 255U,     [1] = 255U,     [2] = 255U     },
};
#else
static AMBA_B8_VOUT_CSC_MATRIX_s DefaultCsc = {
    .Coef = {
        [0] = {  1.00000f,  0.00000f,  0.00000f },
        [1] = {  0.00000f,  1.00000f,  0.00000f },
        [2] = {  0.00000f,  0.00000f,  1.00000f },
    },
    .Offset = { [0] = 0.00000f, [1] = 0.00000f, [2] = 0.00000f },
    .MinVal = { [0] = 0U,       [1] = 0U,       [2] = 0U       },
    .MaxVal = { [0] = 255U,     [1] = 255U,     [2] = 255U     },
};
#endif

/* Data Identifier (ID) (Virtual channel ID + Packet data type) */
#define MIPI_DSI_PKT_DCS_SHORTWRITE0        0x05U   /* Short write. 0 parameter */
#define MIPI_DSI_PKT_DCS_SHORTWRITE1        0x15U   /* Short write. 1 parameter */
/* #define MIPI_DSI_PKT_DCS_READ               0x06U */   /* Read */
#define MIPI_DSI_PKT_GENERIC_NONIMANGE_DATA 0x29U   /* Generic non-image data (long) */
#define MIPI_DSI_PKT_DCS_LONGWRITE          0x39U   /* Long write */

typedef struct {
    UINT32  Parameter:          8;      /* [7:0] Parameter */
    UINT32  CommandType:        8;      /* [15:8] Command type */
    UINT32  DataType:           6;      /* [21:16] The data type (ID) of the sending command */
    UINT32  Reserved:           10;     /* [31:22] Reserved */
} AMBA_MIPI_DCS_SHORT_COMMAND_HEADER_s;

typedef struct {
    UINT32  WordCount1:         8;      /* [7:0] Word count 1 (wc_1) */
    UINT32  WordCount0:         8;      /* [15:8] Word count 0 (wc_0), pending wc-1 bytes */
    UINT32  DataType:           6;      /* [21:16] The data type (ID) of the sending command */
    UINT32  Reserved:           2;      /* [23:22] Reserved */
    UINT32  CommandType:        8;      /* [31:24] Command type */
} AMBA_MIPI_DCS_LONG_COMMAND_HEADER_s;

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
    DOUBLE MaxValue;

    (void)AmbaB8_Wrap_pow(2.0, (DOUBLE) Width, &MaxValue);
    if (Count > (MaxValue - 1.0)) {
        CalCount = (UINT32)MaxValue - 1U;
    } else if (Count == 0.0) {
        CalCount = 0U;
    } else {
        CalCount = (UINT32)Count - 1U;
    }

    return (UINT8)CalCount;
}

static inline INT32 Float2Int32(FLOAT WorkFLOAT)
{
    INT32 WorkINT;

    if (WorkFLOAT >= 0.0F) {
        WorkINT = (INT32)(WorkFLOAT);
    } else {
        WorkINT = 0 - (INT32)(-WorkFLOAT);
    }
    return WorkINT;
}

static void B8_DsiReset(UINT32 ChipID, UINT32 Channel)
{
    B8_SYSTEM_RESET_REG_s SysReset = {0U};

    if (Channel == 0U) {
        SysReset.Dsi0 = 1U;
    } else { /* if (Channel == 1U) { */
        SysReset.Dsi1 = 1U;
    }
    (void) AmbaB8_RegWrite(ChipID, & (pAmbaB8_PllReg->SysReset), 0U, B8_DATA_WIDTH_32BIT, 1U, &SysReset);
    if (Channel == 0U) {
        SysReset.Dsi0 = 0U;
    } else { /* if (Channel == 1U) { */
        SysReset.Dsi1 = 0U;
    }
    (void) AmbaB8_RegWrite(ChipID, & (pAmbaB8_PllReg->SysReset), 0U, B8_DATA_WIDTH_32BIT, 1U, &SysReset);
}

static void B8_VoutSetDisplayTiming(UINT32 ChipID, UINT32 Channel, const AMBA_B8_VIDEO_TIMING_s *pVideoTiming)
{
    UINT32 RegData[8];
    B8_VOUT_FRAME_SIZE_REG_s FrameSize = {0U};
    B8_VOUT_ACTIVE_START_REG_s ActiveRegionStart = {0U};
    B8_VOUT_ACTIVE_END_REG_s ActiveRegionEnd = {0U};
    B8_VOUT_HSYNC_CTRL_REG_s HsyncCtrl = {0U};
    B8_VOUT_VSYNC_CTRL_REG_s VsyncStart = {0U};
    B8_VOUT_VSYNC_CTRL_REG_s VsyncEnd = {0U};
    B8_VOUT_ADJUSTED_START_REG_s AdjustedStart = {0U};

    FrameSize.FrameWidth = pVideoTiming->Htotal - 1U;
    FrameSize.FrameHeight = pVideoTiming->Vtotal - 1U;

    ActiveRegionStart.StartColumn = pVideoTiming->ActiveColStart;  /* Cannot be 0 */
    ActiveRegionStart.StartRow = pVideoTiming->ActiveRowStart;     /* Cannot be 0 */
    ActiveRegionEnd.EndColumn = pVideoTiming->ActiveColStart + pVideoTiming->ActiveColWidth - 1U;
    ActiveRegionEnd.EndRow = pVideoTiming->ActiveRowHeight + pVideoTiming->ActiveRowStart - 1U;

    HsyncCtrl.EndColumn = pVideoTiming->HsyncColEnd;
    HsyncCtrl.StartColumn = pVideoTiming->HsyncColStart;

    VsyncStart.Column = pVideoTiming->VsyncColStart;
    VsyncStart.Row = pVideoTiming->VsyncRowStart;
    VsyncEnd.Column = pVideoTiming->VsyncColEnd;
    VsyncEnd.Row = pVideoTiming->VsyncRowEnd;

    /* VOUT */
    AmbaB8_Misra_TypeCast32(&RegData[0], &FrameSize);
    AmbaB8_Misra_TypeCast32(&RegData[1], &ActiveRegionStart);
    AmbaB8_Misra_TypeCast32(&RegData[2], &ActiveRegionEnd);
    RegData[3] = 0x375893U;
    (void) AmbaB8_RegWrite(ChipID, & (pAmbaB8_VoutReg[Channel]->FrameSize), 1, B8_DATA_WIDTH_32BIT, 4, &RegData[0]);

    AmbaB8_Misra_TypeCast32(&RegData[0], &HsyncCtrl);
    AmbaB8_Misra_TypeCast32(&RegData[1], &VsyncStart);
    AmbaB8_Misra_TypeCast32(&RegData[2], &VsyncEnd);
    (void) AmbaB8_RegWrite(ChipID, & (pAmbaB8_VoutReg[Channel]->HsyncCtrl), 1, B8_DATA_WIDTH_32BIT, 3, &RegData[0]);

    /* Adjust start. By setting adjusted_start_v equals to active_region_start_v , and adjusted_start_h < active_region_start_h , VOUT will drop the first
    frame in DSI/FPD, and get in sync with input frame timing. */
    AdjustedStart.AdjStartV = pVideoTiming->ActiveRowStart;
    AdjustedStart.AdjStartH = pVideoTiming->ActiveColStart - 1U;
    (void) AmbaB8_RegWrite(ChipID, & (pAmbaB8_VoutReg[Channel]->AdjustedStart), 0, B8_DATA_WIDTH_32BIT, 1, &AdjustedStart);
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: B8_VoutSetMipiTxParam
 *
 *  @Description:: Set mipi tx parameters
 *
 *  @Input      ::
 *      ChipID:        B8 chip id
 *      pMipiTxConfig: Pointer to mipi tx configure
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          int : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static UINT32 B8_VoutSetMipiTxParam(UINT32 ChipID, UINT32 Channel, const B8_VOUT_MIPI_TX_CONFIG_s *pMipiTxConfig)
{
    UINT32 RetVal, Data;
    B8_DSI0_CTRL_REG3_REG_s Dsi0CtrlReg3Reg = {0U};
    B8_DSI0_CTRL_REG4_REG_s Dsi0CtrlReg4Reg = {0U};

    if (Channel > 1U) {
        RetVal = B8_ERR_ARG;
    } else {
        if (Channel == 0U) {
            /* output adjustment */
            Data = 0x61400313U;
            (void) AmbaB8_RegWrite(ChipID, & (pAmbaB8_PhyReg->Dsi0CtrlReg0Reg), 0, B8_DATA_WIDTH_32BIT, 1, &Data);

            /* timing protocol */
            Dsi0CtrlReg3Reg.RctDsi0InitTxCtrl = pMipiTxConfig->TxInitCtrl;
            Dsi0CtrlReg3Reg.RctDsi0ClkTrailCtrl = pMipiTxConfig->ClkTrailCtrl;
            Dsi0CtrlReg3Reg.RctDsi0ClkZeroCtrl = pMipiTxConfig->ClkZeroCtrl;
            Dsi0CtrlReg3Reg.RctDsi0ClkPrepareCtrl = pMipiTxConfig->ClkPrepareCtrl;

            Dsi0CtrlReg4Reg.RctDsi0HsZeroCtrl = pMipiTxConfig->HsZeroCtrl;
            Dsi0CtrlReg4Reg.RctDsi0HsPrepareCtrl = pMipiTxConfig->HsPrepareCtrl;
            Dsi0CtrlReg4Reg.RctDsi0HsTrailCtrl = pMipiTxConfig->HsTrailCtrl;
            Dsi0CtrlReg4Reg.RctDsi0HsLpxCtrl = pMipiTxConfig->HsLpxCtrl;

            /* mipi continuous mode */
            Dsi0CtrlReg3Reg.RctDsi0ClkContClkMode = 1U;

            (void) AmbaB8_RegWrite(ChipID, & (pAmbaB8_PhyReg->Dsi0CtrlReg3Reg), 0, B8_DATA_WIDTH_32BIT, 1, &Dsi0CtrlReg3Reg);
            (void) AmbaB8_RegWrite(ChipID, & (pAmbaB8_PhyReg->Dsi0CtrlReg4Reg), 0, B8_DATA_WIDTH_32BIT, 1, &Dsi0CtrlReg4Reg);

            /* enable aux  */
            Data = 0x00000001;
            (void) AmbaB8_RegWrite(ChipID, & (pAmbaB8_PhyReg->Dsi0CtrlAux0Reg), 0, B8_DATA_WIDTH_32BIT, 1, &Data);
            /* enable digital DSI PHY */
            Data = 0x1000001f;
            (void) AmbaB8_RegWrite(ChipID, & (pAmbaB8_PhyReg->Dsi0CtrlReg2Reg), 0, B8_DATA_WIDTH_32BIT, 1, &Data);
        } else {
            /* output adjustment */
            Data = 0x61400313U;
            (void) AmbaB8_RegWrite(ChipID, & (pAmbaB8_PhyReg->Dsi1CtrlReg0Reg), 0, B8_DATA_WIDTH_32BIT, 1, &Data);

            /* timing protocol */
            Dsi0CtrlReg3Reg.RctDsi0InitTxCtrl = pMipiTxConfig->TxInitCtrl;
            Dsi0CtrlReg3Reg.RctDsi0ClkTrailCtrl = pMipiTxConfig->ClkTrailCtrl;
            Dsi0CtrlReg3Reg.RctDsi0ClkZeroCtrl = pMipiTxConfig->ClkZeroCtrl;
            Dsi0CtrlReg3Reg.RctDsi0ClkPrepareCtrl = pMipiTxConfig->ClkPrepareCtrl;

            Dsi0CtrlReg4Reg.RctDsi0HsZeroCtrl = pMipiTxConfig->HsZeroCtrl;
            Dsi0CtrlReg4Reg.RctDsi0HsPrepareCtrl = pMipiTxConfig->HsPrepareCtrl;
            Dsi0CtrlReg4Reg.RctDsi0HsTrailCtrl = pMipiTxConfig->HsTrailCtrl;
            Dsi0CtrlReg4Reg.RctDsi0HsLpxCtrl = pMipiTxConfig->HsLpxCtrl;

            /* mipi continuous mode */
            Dsi0CtrlReg3Reg.RctDsi0ClkContClkMode = 1U;

            (void) AmbaB8_RegWrite(ChipID, & (pAmbaB8_PhyReg->Dsi1CtrlReg3Reg), 0, B8_DATA_WIDTH_32BIT, 1, &Dsi0CtrlReg3Reg);
            (void) AmbaB8_RegWrite(ChipID, & (pAmbaB8_PhyReg->Dsi1CtrlReg4Reg), 0, B8_DATA_WIDTH_32BIT, 1, &Dsi0CtrlReg4Reg);

            /* enable aux  */
            Data = 0x00000001;
            (void) AmbaB8_RegWrite(ChipID, & (pAmbaB8_PhyReg->Dsi1CtrlAux0Reg), 0, B8_DATA_WIDTH_32BIT, 1, &Data);
            /* enable digital DSI PHY */
            Data = 0x1000001f;
            (void) AmbaB8_RegWrite(ChipID, & (pAmbaB8_PhyReg->Dsi1CtrlReg2Reg), 0, B8_DATA_WIDTH_32BIT, 1, &Data);
        }

        /* reset DSI */
        B8_DsiReset(ChipID, Channel);
        RetVal = B8_ERR_NONE;
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: B8_VoutCalculateMipiTxParam
 *
 *  @Description:: Calculate mipi tx parameters
 *
 *  @Input      ::
 *      ChipID:        B8 chip id
 *
 *  @Output     ::
 *      pMipiTxConfig: Pointer to mipi tx configure
 *
 *  @Return     ::
 *          int : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static UINT32 B8_VoutCalculateMipiTxParam(UINT32 Frequency, B8_VOUT_MIPI_TX_CONFIG_s *pMipiTxConfig)
{
    UINT32 BitRateInMhz;
    DOUBLE TxByteClkHS, UI;
    DOUBLE ClkPrepare, ClkPrepareMin, ClkPrepareMax, ClkPrepareSec;
    DOUBLE HsLpxMin;
    DOUBLE HsTrailMin;
    DOUBLE HsPrepare, HsPrepareMin, HsPrepareMax, HsPrepareSec;
    DOUBLE HsZeroMin, HsZeroMinSec;
    DOUBLE ClkZeroMinSec, ClkZeroMin;
    DOUBLE ClkTrailMin;
    DOUBLE InitTxMin;

    UINT8 ClkPrepareCtrl;
    UINT8 ClkZeroCtrl;
    UINT8 ClkTrailCtrl;
    UINT8 HsPrepareCtrl;
    UINT8 HsZeroCtrl;
    UINT8 HsTrailCtrl;
    UINT8 HsLpxCtrl;
    UINT8 TxInitCtrl;

    AmbaB8_PrintUInt5("-- VoutClk: %d", Frequency, 0U, 0U, 0U, 0U);
    BitRateInMhz = (UINT32)(Frequency / (UINT32)1e6);           /* in MHz */
    /********************************************************
    * TX (TxByteClkHS)
    ********************************************************/
    TxByteClkHS = ((DOUBLE)BitRateInMhz * (DOUBLE)1e6) / 8.0;   /* in Hz */
    UI = 1.0 / ((DOUBLE)BitRateInMhz * (DOUBLE)1e6);            /* in second */

    /* ClkPrepare (HsPrepare/ClkPrepare are shared) */
    ClkPrepareMin = 38e-9 * TxByteClkHS ;
    (void) AmbaB8_Wrap_ceil(ClkPrepareMin, &ClkPrepareMin);

    ClkPrepareMax = 95e-9 * TxByteClkHS;
    (void) AmbaB8_Wrap_floor(ClkPrepareMax, &ClkPrepareMax);

    ClkPrepareSec = (38e-9 + 95e-9) / 2.0;
    ClkPrepare = ClkPrepareSec * TxByteClkHS;
    (void) AmbaB8_Wrap_floor((ClkPrepare + 0.5), &ClkPrepare);
    ClkPrepareCtrl = BoundaryCheck(6, ClkPrepare);

    /* ClkZero (HsZero/ClkZero are shared) */
    ClkZeroMinSec = 300e-9 - ClkPrepareSec;
    ClkZeroMin = ClkZeroMinSec * TxByteClkHS;
    (void) AmbaB8_Wrap_ceil(ClkZeroMin, &ClkZeroMin);
    ClkZeroCtrl = BoundaryCheck(6, ClkZeroMin);

    /* ClkTrail */
    ClkTrailMin = 60e-9 * TxByteClkHS;
    (void) AmbaB8_Wrap_ceil(ClkTrailMin, &ClkTrailMin);
    ClkTrailCtrl = BoundaryCheck(5, ClkTrailMin);

    /* HsLpx */
    HsLpxMin = 50e-9 * TxByteClkHS;
    (void) AmbaB8_Wrap_ceil(HsLpxMin, &HsLpxMin);
    HsLpxCtrl = BoundaryCheck(8, HsLpxMin);

    /* HsTrail */
    if ((8.0 * UI) >= (60e-9 + (4.0 * UI))) {
        HsTrailMin = 8.0 * UI;
    } else {
        HsTrailMin = 60e-9 + (4.0 * UI);
    }
    HsTrailMin = HsTrailMin * TxByteClkHS;
    (void) AmbaB8_Wrap_ceil(HsTrailMin, &HsTrailMin);
    HsTrailCtrl = BoundaryCheck(5, HsTrailMin);

    /* HsPrepare */
    HsPrepareMin = (40e-9 + (4.0 * UI)) * TxByteClkHS;
    (void) AmbaB8_Wrap_ceil(HsPrepareMin, &HsPrepareMin);

    HsPrepareMax = (85e-9 + (6.0 * UI)) * TxByteClkHS;
    (void) AmbaB8_Wrap_floor(HsPrepareMax, &HsPrepareMax);

    HsPrepareSec = ((40e-9 + (4.0 * UI)) + (85e-9 + (6.0 * UI))) / 2.0;
    HsPrepare = HsPrepareSec * TxByteClkHS;
    (void) AmbaB8_Wrap_floor((HsPrepare + 0.5), &HsPrepare);
    HsPrepareCtrl = BoundaryCheck(6, HsPrepare);

    /* HsZero */
    HsZeroMinSec = (145e-9 + (10.0 * UI)) - HsPrepareSec;
    HsZeroMin = HsZeroMinSec * TxByteClkHS;
    (void) AmbaB8_Wrap_ceil(HsZeroMin, &HsZeroMin);
    HsZeroCtrl = BoundaryCheck(6, HsZeroMin);

    /* InitTx */
    InitTxMin = (100e-6 * TxByteClkHS) / 1024.0;
    (void) AmbaB8_Wrap_ceil(InitTxMin, &InitTxMin);
    TxInitCtrl = BoundaryCheck(6, InitTxMin);

    /* Fill MIPI-Tx Parameters */
    pMipiTxConfig->ClkPrepareCtrl = ClkPrepareCtrl;
    pMipiTxConfig->ClkZeroCtrl   = ClkZeroCtrl;
    pMipiTxConfig->ClkTrailCtrl  = ClkTrailCtrl;
    pMipiTxConfig->HsPrepareCtrl = HsPrepareCtrl;
    pMipiTxConfig->HsZeroCtrl    = HsZeroCtrl;
    pMipiTxConfig->HsTrailCtrl   = HsTrailCtrl;
    pMipiTxConfig->HsLpxCtrl     = HsLpxCtrl;
    pMipiTxConfig->TxInitCtrl    = TxInitCtrl;

    return B8_ERR_NONE;
}

static void B8_VoutMipiDsiCommandSetup(UINT32 ChipID, UINT32 Channel, UINT32 DataType, UINT32 NumParam, const UINT8 *pParam)
{
    UINT32 CtrlRegVal = 0x0U;
    UINT32 RegVal, i;
    UINT8 WorkByte[16];

    if (pParam != NULL) {
        /* Up to 16 payload bytes */
        for (i = 0U; i < 16U; i++) {
            if (i < NumParam) {
                WorkByte[i] = pParam[i];
            } else {
                WorkByte[i] = 0x00U;
            }
        }

        /* Write back payload bytes */
        RegVal = WorkByte[3];
        RegVal = (RegVal << 8U) | WorkByte[2];
        RegVal = (RegVal << 8U) | WorkByte[1];
        RegVal = (RegVal << 8U) | WorkByte[0];
        if (Channel < AMBA_NUM_B8_VOUT_CHANNEL) {
            (void) AmbaB8_RegWrite(ChipID, & (pAmbaB8_VoutReg[Channel]->MipiCmdLongWord0), 0, B8_DATA_WIDTH_32BIT, 1, &RegVal);
        } else {
            /* For Misra-C */
        }

        RegVal = WorkByte[7];
        RegVal = (RegVal << 8U) | WorkByte[6];
        RegVal = (RegVal << 8U) | WorkByte[5];
        RegVal = (RegVal << 8U) | WorkByte[4];
        if (Channel < AMBA_NUM_B8_VOUT_CHANNEL) {
            (void) AmbaB8_RegWrite(ChipID, & (pAmbaB8_VoutReg[Channel]->MipiCmdLongWord1), 0, B8_DATA_WIDTH_32BIT, 1, &RegVal);
        } else {
            /* For Misra-C */
        }

        RegVal = WorkByte[11];
        RegVal = (RegVal << 8U) | WorkByte[10];
        RegVal = (RegVal << 8U) | WorkByte[9];
        RegVal = (RegVal << 8U) | WorkByte[8];
        if (Channel < AMBA_NUM_B8_VOUT_CHANNEL) {
            (void) AmbaB8_RegWrite(ChipID, & (pAmbaB8_VoutReg[Channel]->MipiCmdLongWord2), 0, B8_DATA_WIDTH_32BIT, 1, &RegVal);
        } else {
            /* For Misra-C */
        }

        RegVal = WorkByte[15];
        RegVal = (RegVal << 8U) | WorkByte[14];
        RegVal = (RegVal << 8U) | WorkByte[13];
        RegVal = (RegVal << 8U) | WorkByte[12];
        if (Channel < AMBA_NUM_B8_VOUT_CHANNEL) {
            (void) AmbaB8_RegWrite(ChipID, & (pAmbaB8_VoutReg[Channel]->MipiCmdLongWord3), 0, B8_DATA_WIDTH_32BIT, 1, &RegVal);
        } else {
            /* For Misra-C */
        }
    }

    /* Set command control */
    if (DataType == MIPI_DSI_PKT_GENERIC_NONIMANGE_DATA) {
        CtrlRegVal = 0x00040000U;
    } else if (DataType == MIPI_DSI_PKT_DCS_LONGWRITE) {
        CtrlRegVal = 0x00080000U;
    } else {
        CtrlRegVal = 0x00000000U;
    }
    if (Channel < AMBA_NUM_B8_VOUT_CHANNEL) {
        (void) AmbaB8_RegWrite(ChipID, & (pAmbaB8_VoutReg[Channel]->MipiCmdCtrl), 0, B8_DATA_WIDTH_32BIT, 1, &CtrlRegVal);
    } else {
        /* For Misra-C */
    }
}

static void B8_VoutSetCsc(UINT32 ChipID, UINT32 Channel, const AMBA_B8_VOUT_CSC_MATRIX_s *pCscMatrix)
{
    B8_VOUT_CSC_PARAM_0_REG_s   CscParam0 = {0U};
    B8_VOUT_CSC_PARAM_1_REG_s   CscParam1 = {0U};
    B8_VOUT_CSC_PARAM_2_REG_s   CscParam2 = {0U};
    B8_VOUT_CSC_PARAM_3_REG_s   CscParam3 = {0U};
    B8_VOUT_CSC_PARAM_4_REG_s   CscParam4 = {0U};
    B8_VOUT_CSC_PARAM_5_REG_s   CscParam5 = {0U};
    B8_VOUT_CSC_PARAM_6_REG_s   CscParam6 = {0U};
    B8_VOUT_CSC_PARAM_7_REG_s   CscParam7 = {0U};
    B8_VOUT_CSC_PARAM_8_REG_s   CscParam8 = {0U};

    CscParam0.CoefA0 = (UINT16)Float2Int32(pCscMatrix->Coef[0][0] * 1024.0F);
    CscParam0.CoefA1 = (UINT16)Float2Int32(pCscMatrix->Coef[0][1] * 1024.0F);
    CscParam1.CoefA2 = (UINT16)Float2Int32(pCscMatrix->Coef[0][2] * 1024.0F);
    CscParam1.CoefA3 = (UINT16)Float2Int32(pCscMatrix->Coef[1][0] * 1024.0F);
    CscParam2.CoefA4 = (UINT16)Float2Int32(pCscMatrix->Coef[1][1] * 1024.0F);
    CscParam2.CoefA5 = (UINT16)Float2Int32(pCscMatrix->Coef[1][2] * 1024.0F);
    CscParam3.CoefA6 = (UINT16)Float2Int32(pCscMatrix->Coef[2][0] * 1024.0F);
    CscParam3.CoefA7 = (UINT16)Float2Int32(pCscMatrix->Coef[2][1] * 1024.0F);
    CscParam4.CoefA8 = (UINT16)Float2Int32(pCscMatrix->Coef[2][2] * 1024.0F);
    CscParam4.ConstB0 = (UINT16)Float2Int32(pCscMatrix->Offset[0]);
    CscParam5.ConstB1 = (UINT16)Float2Int32(pCscMatrix->Offset[1]);
    CscParam5.ConstB2 = (UINT16)Float2Int32(pCscMatrix->Offset[2]);
    CscParam6.Output0ClampLow  = pCscMatrix->MinVal[0];
    CscParam6.Output0ClampHigh = pCscMatrix->MaxVal[0];
    CscParam7.Output1ClampLow  = pCscMatrix->MinVal[1];
    CscParam7.Output1ClampHigh = pCscMatrix->MaxVal[1];
    CscParam8.Output2ClampLow  = pCscMatrix->MinVal[2];
    CscParam8.Output2ClampHigh = pCscMatrix->MaxVal[2];

    (void)AmbaB8_RegWrite(ChipID, & (pAmbaB8_VoutReg[Channel]->CscParam0), 0, B8_DATA_WIDTH_32BIT, 1, &CscParam0);
    (void)AmbaB8_RegWrite(ChipID, & (pAmbaB8_VoutReg[Channel]->CscParam1), 0, B8_DATA_WIDTH_32BIT, 1, &CscParam1);
    (void)AmbaB8_RegWrite(ChipID, & (pAmbaB8_VoutReg[Channel]->CscParam2), 0, B8_DATA_WIDTH_32BIT, 1, &CscParam2);
    (void)AmbaB8_RegWrite(ChipID, & (pAmbaB8_VoutReg[Channel]->CscParam3), 0, B8_DATA_WIDTH_32BIT, 1, &CscParam3);
    (void)AmbaB8_RegWrite(ChipID, & (pAmbaB8_VoutReg[Channel]->CscParam4), 0, B8_DATA_WIDTH_32BIT, 1, &CscParam4);
    (void)AmbaB8_RegWrite(ChipID, & (pAmbaB8_VoutReg[Channel]->CscParam5), 0, B8_DATA_WIDTH_32BIT, 1, &CscParam5);
    (void)AmbaB8_RegWrite(ChipID, & (pAmbaB8_VoutReg[Channel]->CscParam6), 0, B8_DATA_WIDTH_32BIT, 1, &CscParam6);
    (void)AmbaB8_RegWrite(ChipID, & (pAmbaB8_VoutReg[Channel]->CscParam7), 0, B8_DATA_WIDTH_32BIT, 1, &CscParam7);
    (void)AmbaB8_RegWrite(ChipID, & (pAmbaB8_VoutReg[Channel]->CscParam8), 0, B8_DATA_WIDTH_32BIT, 1, &CscParam8);
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaB8_VoutEnable
 *
 *  @Description:: Enable B8 VOUT
 *
 *  @Input      ::
 *     ChipID:      B8 chip id
 *     Channel:     Vout Channel
 *
 *  @Output     :: none
 *
 *  @Return     ::
\*-----------------------------------------------------------------------------------------------*/
UINT32 AmbaB8_VoutEnable(UINT32 ChipID, UINT32 Channel)
{
    UINT32 RegData[1];
    UINT32 RetVal;

    RegData[0] = 0x1U;
    RetVal = AmbaB8_RegWrite(ChipID, & (pAmbaB8_VoutReg[Channel]->FrameEnable), 0, B8_DATA_WIDTH_32BIT, 1, &RegData[0]);

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaB8_VoutMipiCsiConfig
 *
 *  @Description:: Configure B8 VOUT
 *
 *  @Input      ::
 *     ChipID:       B8 chip id
 *      pVoutConfig: Pointer to B8 VOUT configure
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *      int : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
UINT32 AmbaB8_VoutMipiCsiConfig(UINT32 ChipID, UINT32 Channel, const B8_VOUT_CONFIG_s *pVoutConfig)
{
    B8_VOUT_MIPI_TX_CONFIG_s MipiTxConfig = {0U};
    B8_VOUT_CTRL_REG_s Ctrl = {0U};
    B8_VOUT_FRAME_SIZE_REG_s FrameSize = {0U};
    B8_VOUT_ACTIVE_START_REG_s ActiveRegionStart = {0U};
    B8_VOUT_ACTIVE_END_REG_s ActiveRegionEnd = {0U};
    B8_VOUT_VSYNC_CTRL_REG_s VsyncStart = {0U};
    B8_VOUT_VSYNC_CTRL_REG_s VsyncEnd = {0U};
    B8_VOUT_ADJUSTED_START_REG_s AdjustedStart = {0U};
    B8_VOUT_SFIFO_CTRL_REG_s InputSyncFifoCtrl = {0U};
    B8_VOUT_MODE_REG_s OutputMode = {0U};
    B8_SYSTEM_RESET_REG_s SysReset = {0};

    UINT8 VoRatio;
    UINT32 Tturn; /* ByteClkCycle; */
    UINT32 RetVal = B8_ERR_NONE;
    UINT32 SensorPixelRate;
    UINT32 DataBuf32[8];
    DOUBLE FrameWidth;
    DOUBLE FloorVal;

    /* for B8n VOUT only */
    if ((ChipID & B8_MAIN_CHIP_ID_B8N_MASK) == 0U) {
        RetVal = B8_ERR_INVALID_API;
    } else {
        /* pll_vo / vout_div = clk_vo, vout_div = #BitDepth / 2 * 8 / #Lanes */
        VoRatio = (UINT8)(((pVoutConfig->Output.NumDataBits / 2U) * 8U) / pVoutConfig->Output.NumDataLanes);

        /* set VOUT clock (must before CalculateMipiTxParam) */
        if (Channel == B8_VOUT_CHANNEL1) {
            (void) AmbaB8_PllSetVout1Clk((ChipID & B8_MAIN_CHIP_ID_B8N_MASK), pVoutConfig->Output.DataRate);
            (void) AmbaB8_PllSetVoutClkHint((ChipID & B8_MAIN_CHIP_ID_B8N_MASK), 1U, VoRatio);
        } else {
            (void) AmbaB8_PllSetVout0Clk((ChipID & B8_MAIN_CHIP_ID_B8N_MASK), pVoutConfig->Output.DataRate);
            (void) AmbaB8_PllSetVoutClkHint((ChipID & B8_MAIN_CHIP_ID_B8N_MASK), 0U, VoRatio);
        }

        /* configure MIPI Tx Parameters */
        (void) B8_VoutCalculateMipiTxParam(pVoutConfig->Output.DataRate, &MipiTxConfig);
        (void) B8_VoutSetMipiTxParam((ChipID & B8_MAIN_CHIP_ID_B8N_MASK), Channel, &MipiTxConfig);

        if (Channel == B8_VOUT_CHANNEL1) {
            /* Reset DSI */
            SysReset.Dsi1 = 0x1;
            (void) AmbaB8_Wrap_memcpy(&DataBuf32[0], &SysReset, sizeof(SysReset));
            SysReset.Dsi1 = 0x0;
            (void) AmbaB8_Wrap_memcpy(&DataBuf32[1], &SysReset, sizeof(SysReset));
            (void) AmbaB8_RegWrite(ChipID, &(pAmbaB8_PllReg->SysReset), 0, B8_DATA_WIDTH_32BIT, 2, DataBuf32);
        } else {
            /* Reset DSI */
            SysReset.Dsi0 = 0x1;
            (void) AmbaB8_Wrap_memcpy(&DataBuf32[0], &SysReset, sizeof(SysReset));
            SysReset.Dsi0 = 0x0;
            (void) AmbaB8_Wrap_memcpy(&DataBuf32[1], &SysReset, sizeof(SysReset));
            (void) AmbaB8_RegWrite(ChipID, &(pAmbaB8_PllReg->SysReset), 0, B8_DATA_WIDTH_32BIT, 2, DataBuf32);
        }

        /* compute Tturn */
        /* ByteClkCycle = 1000000000U / (pVoutConfig->Output.DataRate / 8U); */
        Tturn = (((UINT32)MipiTxConfig.HsTrailCtrl + 1U) +
                 (((UINT32)MipiTxConfig.ClkTrailCtrl + 1U) * 3U) +
                 (((UINT32)MipiTxConfig.HsLpxCtrl + 1U) * 2U) +
                 (((UINT32)MipiTxConfig.HsPrepareCtrl + 1U) * 2U) +
                 (((UINT32)MipiTxConfig.HsZeroCtrl + 1U) * 3U )) + 10U;

        /* compute FrameWidth based on consistent throughput */
        (void) AmbaB8_Wrap_floor(((DOUBLE)pVoutConfig->Input.DataRate * (DOUBLE)pVoutConfig->Input.NumDataLanes) / (DOUBLE)pVoutConfig->Input.NumDataBits, &FloorVal);
        SensorPixelRate = (UINT32) FloorVal;
        FrameWidth = ((((DOUBLE) pVoutConfig->Output.DataRate / (((DOUBLE)pVoutConfig->Output.NumDataBits * (DOUBLE)2.0) / (DOUBLE)pVoutConfig->Output.NumDataLanes)) / (DOUBLE)SensorPixelRate) * (DOUBLE)pVoutConfig->LineLengthPck);
        (void) AmbaB8_Wrap_floor((FrameWidth + 0.5), &FrameWidth);

        /* configure */
        Ctrl.DigitalOutputEnable = 1U;
        Ctrl.MipiCsiEnable = 1U;
        if (pVoutConfig->Output.NumDataLanes == 8U) {
            Ctrl.MipiLaneNum = 3U;
        } else if (pVoutConfig->Output.NumDataLanes == 4U) {
            Ctrl.MipiLaneNum = 2U;
        } else if (pVoutConfig->Output.NumDataLanes == 2U) {
            Ctrl.MipiLaneNum = 1U;
        } else {
            Ctrl.MipiLaneNum = 0U;
        }

        Ctrl.VariableStartPointEn = 1U;             /* tuning AdjStart */

        if (Ctrl.FixedLineTiming == 1U)  {
            /* same thoughput as sensor px rate */
            FrameSize.FrameWidth        = (UINT16)FrameWidth - 1U;
        } else {
            /* variable horizontal blanking time */
            /* note: 1.5 for 50% buffer */
            /* +2 means: 2 clk_vo cycles for packet header & footer */
            /* clk_vo : symbol_clk = 8 : VoRatio */
            (void) AmbaB8_Wrap_floor((((DOUBLE)Tturn * 1.5) * 8.0) / (DOUBLE)VoRatio, &FloorVal);
            FrameSize.FrameWidth        = (UINT16)((((pVoutConfig->OutputWidth / 4U) + 2U) + (UINT16)FloorVal) * 2U);
        }
        FrameSize.FrameHeight           = pVoutConfig->FrameLengthLines - 1U;

        VsyncStart.Column               = 206U;
        VsyncStart.Row                  = 4U;

        ActiveRegionStart.StartColumn   = 16U;
        ActiveRegionStart.StartRow      = VsyncStart.Row + 1U;
        ActiveRegionEnd.EndColumn       = ((pVoutConfig->OutputWidth / 2U) - 1U) + ActiveRegionStart.StartColumn;
        ActiveRegionEnd.EndRow          = (pVoutConfig->OutputHeight - 1U) + ActiveRegionStart.StartRow;

        VsyncEnd.Column                 = 303U;
        VsyncEnd.Row                    = ActiveRegionEnd.EndRow + 1U;

        AdjustedStart.AdjStartH         = ActiveRegionStart.StartColumn;
        AdjustedStart.AdjStartV         = ActiveRegionStart.StartRow - 2U;

        InputSyncFifoCtrl.InSfifoRefresh = 1U;
        if ((pVoutConfig->Source == B8_VOUT_SOURCE_BYPASS_VIN0) || (pVoutConfig->Source == B8_VOUT_SOURCE_BYPASS_VIN1)) {
            InputSyncFifoCtrl.InSfifoHighWater = (pVoutConfig->OutputWidth / 2U) - 16U;

        } else if (pVoutConfig->Source == B8_VOUT_SOURCE_MERGER_CODEC) {
            InputSyncFifoCtrl.InSfifoHighWater = 640U;

        } else { /* B8_VOUT_SOURCE_MERGER */
            InputSyncFifoCtrl.InSfifoHighWater = 128U;
        }

        OutputMode.HsyncPoLarity = 1U;
        OutputMode.VsyncPoLarity = 1U;
        OutputMode.ClockOutputDivider = 1U;
        OutputMode.ClockDividerEnable = 0U;
        OutputMode.MipiFrameCount = 1U;
        OutputMode.MipiSendBlankLines = 0U;
        OutputMode.MipiLineTimingAll = 1U;
        OutputMode.MipiEccOrderReserved1 = 0U;
        OutputMode.HvldPoLarity = 1U;
        OutputMode.ColorSeqEvenLines = 1U;
        OutputMode.ColorSeqOddLines = 0U;
        if (pVoutConfig->Output.NumDataBits == 8U) {
            OutputMode.OutputMode = B8_VOUT_MIPI_RAW_8BIT;
        } else if (pVoutConfig->Output.NumDataBits == 10U) {
            OutputMode.OutputMode = B8_VOUT_MIPI_RAW_10BIT;
        } else {
            OutputMode.OutputMode = B8_VOUT_MIPI_RAW_12BIT;
        }

        (void) AmbaB8_Wrap_memcpy(&DataBuf32[0], &Ctrl, sizeof(B8_VOUT_CTRL_REG_s));
        DataBuf32[1] = 0U;                          /* status */
        (void) AmbaB8_Wrap_memcpy(&DataBuf32[2], &FrameSize, sizeof(B8_VOUT_FRAME_SIZE_REG_s));
        (void) AmbaB8_Wrap_memcpy(&DataBuf32[3], &ActiveRegionStart, sizeof(B8_VOUT_ACTIVE_START_REG_s));
        (void) AmbaB8_Wrap_memcpy(&DataBuf32[4], &ActiveRegionEnd, sizeof(B8_VOUT_ACTIVE_END_REG_s));
        DataBuf32[5] = 0x00975893U;                 /* color to display if enable deadline missed */
        (void) AmbaB8_Wrap_memcpy(&DataBuf32[6], &OutputMode, sizeof(B8_VOUT_MODE_REG_s));
        DataBuf32[7] = 0x00040005U;                 /* hsync control */
        (void) AmbaB8_RegWrite((ChipID & B8_MAIN_CHIP_ID_B8N_MASK), & (pAmbaB8_VoutReg[Channel]->Ctrl), 1U, B8_DATA_WIDTH_32BIT, 8U, DataBuf32);

        (void) AmbaB8_Wrap_memcpy(&DataBuf32[0], &VsyncStart, sizeof(B8_VOUT_VSYNC_CTRL_REG_s));
        (void) AmbaB8_Wrap_memcpy(&DataBuf32[1], &VsyncEnd, sizeof(B8_VOUT_VSYNC_CTRL_REG_s));
        DataBuf32[2] = 0x0U;
        DataBuf32[3] = 0x00000804U;
        DataBuf32[4] = 0x00e4e400U;
        DataBuf32[5] = 0x0U;
        DataBuf32[6] = 0x0U;
        DataBuf32[7] = 0x0U;
        (void) AmbaB8_RegWrite((ChipID & B8_MAIN_CHIP_ID_B8N_MASK), & (pAmbaB8_VoutReg[Channel]->VsyncStart), 1U, B8_DATA_WIDTH_32BIT, 8U, DataBuf32);

        DataBuf32[0] = 0x0U;
        (void) AmbaB8_Wrap_memcpy(&DataBuf32[1], &AdjustedStart, sizeof(B8_VOUT_ADJUSTED_START_REG_s));
        (void) AmbaB8_Wrap_memcpy(&DataBuf32[2], &InputSyncFifoCtrl, sizeof(B8_VOUT_SFIFO_CTRL_REG_s));
        DataBuf32[3] = 0x0U;                        /* disable background generation for mipi csi mode */
        (void) AmbaB8_RegWrite((ChipID & B8_MAIN_CHIP_ID_B8N_MASK), & (pAmbaB8_VoutReg[Channel]->MipiPhyCtlStatus1), 1U, B8_DATA_WIDTH_32BIT, 4U, DataBuf32);

        DataBuf32[0] = 0x1U;                        /* frame enable */
        (void) AmbaB8_RegWrite((ChipID & B8_MAIN_CHIP_ID_B8N_MASK), & (pAmbaB8_VoutReg[Channel]->FrameEnable), 1U, B8_DATA_WIDTH_32BIT, 1U, DataBuf32);

        (void) AmbaB8_VoutInputSrcEnable((ChipID & B8_MAIN_CHIP_ID_B8N_MASK), Channel);

    }
    return RetVal;
}

UINT32 AmbaB8_VoutFpdLinkConfig(UINT32 ChipID, const AMBA_B8_VOUT_DISPLAY_CONFIG_s *pDisplayConfig)
{
    UINT32 RetVal = B8_ERR_NONE;
    UINT32 RegData[3];
    B8_VOUT_CTRL_REG_s B8DisplayCtrl = {0U};
    B8_VOUT_MODE_REG_s B8OutputModel = {0U};
    UINT32 Channel = pDisplayConfig->VoutChannel;

    if (Channel == B8_VOUT_CHANNEL0) {
        (void) AmbaB8_PllSetVoutClkHint(ChipID, Channel, 7U);
        RetVal = AmbaB8_PllSetVout0Clk(ChipID, pDisplayConfig->VideoTiming.PixelClock * 7U);
    } else {
        (void) AmbaB8_PllSetVoutClkHint(ChipID, Channel, 7U);
        RetVal = AmbaB8_PllSetVout1Clk(ChipID, pDisplayConfig->VideoTiming.PixelClock * 7U);
    }

    /* Fill up the display control */
    B8DisplayCtrl.DigitalOutputEnable = 1U;
    B8DisplayCtrl.FixedFormatSelect = 0U;
    /* New in B8 vout */
    B8DisplayCtrl.FixedLineTiming = 1U;
    B8DisplayCtrl.VariableStartPointEn = 0U;
    B8DisplayCtrl.Yuv422Input = 1U;

    B8DisplayCtrl.FpdFourthLaneEn = pDisplayConfig->FpdLinkCtrl.FourthLaneEnable;
    B8DisplayCtrl.FpdMsbSelectRW = pDisplayConfig->FpdLinkCtrl.FpdLinkMode;
    if ((pDisplayConfig->FpdLinkCtrl.FpdDualPortEn) != 0U) {
        B8DisplayCtrl.FpdDualPorts = 1U;
        RegData[0] = 0x1U;
        (void) AmbaB8_RegWrite(ChipID, & (pAmbaB8_PhyReg->Dsi8LaneModeReg), 0, B8_DATA_WIDTH_32BIT, 1, &RegData[0]);
    }
    B8DisplayCtrl.FpdCtlOnSecondPort = pDisplayConfig->FpdLinkCtrl.FpdDualPortCtrl;

    /* Fill up the output mode */
    B8OutputModel.OutputMode = 15U;
    B8OutputModel.HvldPoLarity = pDisplayConfig->PolarityCtrl.HvldPolarity;

    /* DSI Control */
    RegData[0] = 0x51ac0b23U;    /* Ctrl 0 */
    RegData[1] = 0x0U;           /* Ctrl 1 */
    RegData[2] = 0x1000001fU;    /* Ctrl 2 */
    if (Channel == B8_VOUT_CHANNEL0) {
        (void) AmbaB8_RegWrite(ChipID, & (pAmbaB8_PhyReg->Dsi0CtrlReg0Reg), 1, B8_DATA_WIDTH_32BIT, 3, &RegData[0]);
    } else {
        (void) AmbaB8_RegWrite(ChipID, & (pAmbaB8_PhyReg->Dsi1CtrlReg0Reg), 1, B8_DATA_WIDTH_32BIT, 3, &RegData[0]);
    }
    RegData[0] = 0x00000100U;    /* AUX CTRL: Enable FPD mode */
    if (Channel == B8_VOUT_CHANNEL0) {
        (void) AmbaB8_RegWrite(ChipID, & (pAmbaB8_PhyReg->Dsi0CtrlAux0Reg), 0, B8_DATA_WIDTH_32BIT, 1, &RegData[0]);
    } else {
        (void) AmbaB8_RegWrite(ChipID, & (pAmbaB8_PhyReg->Dsi1CtrlAux0Reg), 0, B8_DATA_WIDTH_32BIT, 1, &RegData[0]);
    }

    /* Reset DSI PHY */
    if (Channel == B8_VOUT_CHANNEL0) {
        RegData[0] = 0x00000008U;
        (void) AmbaB8_RegWrite(ChipID, & (pAmbaB8_PllReg->SysReset), 0, B8_DATA_WIDTH_32BIT, 1, &RegData[0]);
        RegData[0] = 0x00000000U;
        (void) AmbaB8_RegWrite(ChipID, & (pAmbaB8_PllReg->SysReset), 0, B8_DATA_WIDTH_32BIT, 1, &RegData[0]);
    } else {
        RegData[0] = 0x00040000U;
        (void) AmbaB8_RegWrite(ChipID, & (pAmbaB8_PllReg->SysReset), 0, B8_DATA_WIDTH_32BIT, 1, &RegData[0]);
        RegData[0] = 0x00000000U;
        (void) AmbaB8_RegWrite(ChipID, & (pAmbaB8_PllReg->SysReset), 0, B8_DATA_WIDTH_32BIT, 1, &RegData[0]);
    }

    /* Write Ctrl register */
    (void) AmbaB8_RegWrite((ChipID), & (pAmbaB8_VoutReg[Channel]->Ctrl), 0, B8_DATA_WIDTH_32BIT, 1, &B8DisplayCtrl);

    /* Write Outputmode register */
    (void) AmbaB8_RegWrite((ChipID), & (pAmbaB8_VoutReg[Channel]->OutputMode), 0, B8_DATA_WIDTH_32BIT, 1, &B8OutputModel);

    /* Write CSC table */
    if (pDisplayConfig->pCscMatrix == NULL) {
        /* Default csc */
        B8_VoutSetCsc(ChipID, Channel, &DefaultCsc);
    } else {
        B8_VoutSetCsc(ChipID, Channel, pDisplayConfig->pCscMatrix);
    }

    /* Write video timing */
    B8_VoutSetDisplayTiming(ChipID, Channel, &pDisplayConfig->VideoTiming);

    return RetVal;
}

UINT32 AmbaB8_VoutMipiDsiConfig(UINT32 ChipID, const AMBA_B8_VOUT_DISPLAY_CONFIG_s *pDisplayConfig)
{
    UINT32 RetVal = B8_ERR_NONE;
    UINT32 RegData[7];
    UINT8 Ratio = 24U;
    B8_VOUT_CTRL_REG_s B8DisplayCtrl = {0U};
    B8_VOUT_MODE_REG_s B8OutputMode = {0U};
    B8_VOUT_MIPI_TX_CONFIG_s MipiTxConfig;
    B8_DSI0_CTRL_REG3_REG_s DsiCtrl3 = {0U};
    B8_DSI0_CTRL_REG4_REG_s DsiCtrl4 = {0U};
    B8_VOUT_HBP_SYNC_REG_s HbpSync = {0U};
    B8_VOUT_HFP_VB_REG_s MipiHfpVb = {0U};
    UINT32 Channel = pDisplayConfig->VoutChannel;
    UINT8 MipiDsiMode = pDisplayConfig->OutputMode;
    UINT8 MipiLane = 1;

    if (pDisplayConfig->MipiCtrl.MipiLaneNum == B8_VOUT_MIPI_8_LANE) {
        MipiLane = 8;
    } else if (pDisplayConfig->MipiCtrl.MipiLaneNum == B8_VOUT_MIPI_4_LANE) {
        MipiLane = 4;
    } else if (pDisplayConfig->MipiCtrl.MipiLaneNum == B8_VOUT_MIPI_2_LANE) {
        MipiLane = 2;
    } else {
        RetVal = B8_ERR_ARG;
    }

    if ((MipiDsiMode == B8_VOUT_MIPI_RGB_24BIT) || (MipiDsiMode == B8_VOUT_MIPI_RGB_18BIT)) {
        Ratio = 24U / MipiLane;
    } else if ((MipiDsiMode == B8_VOUT_MIPI_RGB_16BIT) || (MipiDsiMode == B8_VOUT_MIPI_YUV_16BIT)) {
        Ratio = 16U / MipiLane;
    } else if (MipiDsiMode == B8_VOUT_MIPI_RAW_8BIT) {
        Ratio = 8U / MipiLane;
    } else {
        RetVal = B8_ERR_ARG;
    }

    if (Channel == B8_VOUT_CHANNEL0) {
        (void) AmbaB8_PllSetVoutClkHint(ChipID, Channel, Ratio);
        RetVal = AmbaB8_PllSetVout0Clk(ChipID, pDisplayConfig->VideoTiming.PixelClock * Ratio);
    } else {
        (void) AmbaB8_PllSetVoutClkHint(ChipID, Channel, Ratio);
        RetVal = AmbaB8_PllSetVout1Clk(ChipID, pDisplayConfig->VideoTiming.PixelClock * Ratio);
    }

    /* Fill up the display control */
    B8DisplayCtrl.DigitalOutputEnable = 1U;
    B8DisplayCtrl.FixedFormatSelect = 0U;
    B8DisplayCtrl.MipiSyncEndEn = pDisplayConfig->MipiCtrl.MipiSyncEndEnable;
    B8DisplayCtrl.MipiEotpEn = pDisplayConfig->MipiCtrl.MipiEotpEnable;
    B8DisplayCtrl.MipiLaneNum = pDisplayConfig->MipiCtrl.MipiLaneNum;
    B8DisplayCtrl.MipiCsiEnable = 0U;
    /* New in B8A vout */
    B8DisplayCtrl.FixedLineTiming = 1U;
    B8DisplayCtrl.VariableStartPointEn = 1U;
    B8DisplayCtrl.Yuv422Input = 1U;

    /* Fill up the output mode */
    B8OutputMode.OutputMode = MipiDsiMode;
    B8OutputMode.HvldPoLarity = pDisplayConfig->PolarityCtrl.HvldPolarity;
    B8OutputMode.HsyncPoLarity = pDisplayConfig->PolarityCtrl.HsyncPolarity;
    B8OutputMode.VsyncPoLarity = pDisplayConfig->PolarityCtrl.VsyncPolarity;

    B8OutputMode.MipiLineTiming = 1U;
    B8OutputMode.MipiLineCount = 0U;
    B8OutputMode.MipiFrameCount = 0U;
    B8OutputMode.MipiSendBlankLines = 0U;
    B8OutputMode.MipiLineTimingAll = 1U;
    B8OutputMode.MipiEccOrderReserved1 = 0U;

    /* MIPI DSI Control */
    RegData[0] = 0x61400313U;    /* Ctrl 0 */
    RegData[1] = 0x0U;           /* Ctrl 1 */
    RegData[2] = 0x1000001fU;    /* Ctrl 2 */
    (void) B8_VoutCalculateMipiTxParam(pDisplayConfig->VideoTiming.PixelClock * Ratio, &MipiTxConfig);
    /* Ctrl 3 */
    DsiCtrl3.RctDsi0ClkPrepareCtrl = MipiTxConfig.ClkPrepareCtrl;
    DsiCtrl3.RctDsi0ClkTrailCtrl = MipiTxConfig.ClkTrailCtrl;
    DsiCtrl3.RctDsi0ClkZeroCtrl = MipiTxConfig.ClkZeroCtrl;
    DsiCtrl3.RctDsi0InitTxCtrl = MipiTxConfig.TxInitCtrl;
    AmbaB8_Misra_TypeCast32(&RegData[3], &DsiCtrl3);
    /* Ctrl 4 */
    DsiCtrl4.RctDsi0HsLpxCtrl = MipiTxConfig.HsLpxCtrl;
    DsiCtrl4.RctDsi0HsPrepareCtrl = MipiTxConfig.HsPrepareCtrl;
    DsiCtrl4.RctDsi0HsTrailCtrl = MipiTxConfig.HsTrailCtrl;
    DsiCtrl4.RctDsi0HsZeroCtrl =  MipiTxConfig.HsZeroCtrl;
    AmbaB8_Misra_TypeCast32(&RegData[4], &DsiCtrl4);
    RegData[5] = 0U;             /* Ctrl 5 */
    RegData[6] = 0x00000001U;    /* AUX control */
    if (Channel == B8_VOUT_CHANNEL0) {
        (void) AmbaB8_RegWrite(ChipID, & (pAmbaB8_PhyReg->Dsi0CtrlReg0Reg), 1, B8_DATA_WIDTH_32BIT, 7, &RegData[0]);
    } else {
        (void) AmbaB8_RegWrite(ChipID, & (pAmbaB8_PhyReg->Dsi1CtrlReg0Reg), 1, B8_DATA_WIDTH_32BIT, 7, &RegData[0]);
    }

    /* Reset DSI PHY */
    if (Channel == B8_VOUT_CHANNEL0) {
        RegData[0] = 0x00000008U;
        (void) AmbaB8_RegWrite(ChipID, & (pAmbaB8_PllReg->SysReset), 0, B8_DATA_WIDTH_32BIT, 1, &RegData[0]);
        RegData[0] = 0x00000000U;
        (void) AmbaB8_RegWrite(ChipID, & (pAmbaB8_PllReg->SysReset), 0, B8_DATA_WIDTH_32BIT, 1, &RegData[0]);
    } else {
        RegData[0] = 0x00040000U;
        (void) AmbaB8_RegWrite(ChipID, & (pAmbaB8_PllReg->SysReset), 0, B8_DATA_WIDTH_32BIT, 1, &RegData[0]);
        RegData[0] = 0x00000000U;
        (void) AmbaB8_RegWrite(ChipID, & (pAmbaB8_PllReg->SysReset), 0, B8_DATA_WIDTH_32BIT, 1, &RegData[0]);
    }

    /* Write Ctrl register */
    (void) AmbaB8_RegWrite(ChipID, & (pAmbaB8_VoutReg[Channel]->Ctrl), 0, B8_DATA_WIDTH_32BIT, 1, &B8DisplayCtrl);

    /* Write Outputmode register */
    (void) AmbaB8_RegWrite(ChipID, & (pAmbaB8_VoutReg[Channel]->OutputMode), 0, B8_DATA_WIDTH_32BIT, 1, &B8OutputMode);

    /* Mipi blanking control */
    if (pDisplayConfig->MipiCtrl.PayloadSizeAtVblankLines != 0U) {
        MipiHfpVb.DsiVblankWc = pDisplayConfig->MipiCtrl.PayloadSizeAtVblankLines;
        MipiHfpVb.DsiVblankUseBlank = 1U;
    }
    if (pDisplayConfig->MipiCtrl.PayloadSizeAtHsyncFrontPorch !=0U) {
        MipiHfpVb.DsiHfpWc = pDisplayConfig->MipiCtrl.PayloadSizeAtHsyncFrontPorch;
        MipiHfpVb.DsiHfpUseBlank = 1U;
    }
    if (pDisplayConfig->MipiCtrl.PayloadSizeAtHsyncPulse != 0U) {
        HbpSync.DsiSyncWc = pDisplayConfig->MipiCtrl.PayloadSizeAtHsyncPulse;
        HbpSync.DsiSyncUseBlank = 1U;
    }
    if (pDisplayConfig->MipiCtrl.PayloadSizeAtHsyncBackPorch != 0U) {
        HbpSync.DsiHbpWc = pDisplayConfig->MipiCtrl.PayloadSizeAtHsyncBackPorch;
        HbpSync.DsiHbpUseBlank = 1U;
    }
    (void) AmbaB8_RegWrite(ChipID, & (pAmbaB8_VoutReg[Channel]->MipiHfpVb), 0, B8_DATA_WIDTH_32BIT, 1, &MipiHfpVb);
    (void) AmbaB8_RegWrite(ChipID, & (pAmbaB8_VoutReg[Channel]->HbpSync), 0, B8_DATA_WIDTH_32BIT, 1, &HbpSync);

    /* Write CSC table */
    if ( pDisplayConfig->pCscMatrix == NULL) {
        /* Default csc */
        B8_VoutSetCsc(ChipID, Channel, &DefaultCsc);
    } else {
        B8_VoutSetCsc(ChipID, Channel, pDisplayConfig->pCscMatrix);
    }

    /* Write video timing */
    B8_VoutSetDisplayTiming(ChipID, Channel, &pDisplayConfig->VideoTiming);

    return RetVal;
}

UINT32 AmbaB8_VoutMipiDsiDcsWrite(UINT32 ChipID, UINT32 Channel, UINT32 DcsCmd, UINT32 NumParam, const UINT8 *pParam)
{
    AMBA_MIPI_DCS_SHORT_COMMAND_HEADER_s ShortCmdHeader;
    AMBA_MIPI_DCS_LONG_COMMAND_HEADER_s LongCmdHeader;
    UINT32 RegVal = 0U, RetVal = B8_ERR_NONE;

    if (NumParam < 2U) {
        /* Fill up header */
        AmbaB8_Misra_TypeCast32(&ShortCmdHeader, &RegVal);
        ShortCmdHeader.CommandType = (UINT8)DcsCmd;
        if ((NumParam != 0U) && (pParam != NULL)) {
            ShortCmdHeader.DataType = MIPI_DSI_PKT_DCS_SHORTWRITE1;
            ShortCmdHeader.Parameter = *pParam;
        } else {
            ShortCmdHeader.DataType = MIPI_DSI_PKT_DCS_SHORTWRITE0;
        }
        AmbaB8_Misra_TypeCast32(&RegVal, &ShortCmdHeader);
        if (Channel < AMBA_NUM_B8_VOUT_CHANNEL) {
            (void) AmbaB8_RegWrite(ChipID, & (pAmbaB8_VoutReg[Channel]->MipiCmdParam), 0, B8_DATA_WIDTH_32BIT, 1, &RegVal);
        } else {
            RetVal = B8_ERR_UNEXPECTED;
        }

        /* Setup Mipi command */
        B8_VoutMipiDsiCommandSetup(ChipID, Channel, ShortCmdHeader.DataType, NumParam, NULL);
    } else {
        /* Fill up header */
        AmbaB8_Misra_TypeCast32(&LongCmdHeader, &RegVal);
        LongCmdHeader.DataType = MIPI_DSI_PKT_DCS_LONGWRITE;
        LongCmdHeader.CommandType = (UINT8)DcsCmd;
        LongCmdHeader.WordCount0 = (UINT16)NumParam + 1U;    /* WordCount0 = send byte + 1 */
        AmbaB8_Misra_TypeCast32(&RegVal, &LongCmdHeader);
        if (Channel < AMBA_NUM_B8_VOUT_CHANNEL) {
            (void) AmbaB8_RegWrite(ChipID, & (pAmbaB8_VoutReg[Channel]->MipiCmdParam), 0, B8_DATA_WIDTH_32BIT, 1, &RegVal);
        } else {
            RetVal = B8_ERR_UNEXPECTED;
        }

        /* Setup Mipi command */
        B8_VoutMipiDsiCommandSetup(ChipID, Channel, MIPI_DSI_PKT_DCS_LONGWRITE, NumParam, pParam);
    }

    return RetVal;
}

UINT32 AmbaB8_VoutSourceConfig(UINT32 ChipID, UINT32 VoutChannel, UINT32 VoutSource, const AMBA_B8_VOUT_DISPLAY_CONFIG_s *pDisplayConfig)
{
    UINT32 RetVal = B8_ERR_NONE;
    UINT32 DataBuf[2];
    UINT16 DataBuf16[10];
    UINT16 ActiveWidth = pDisplayConfig->VideoTiming.ActiveColWidth;
    UINT16 ActiveHeight = pDisplayConfig->VideoTiming.ActiveRowHeight;
    B8_SCRATCHPAD_VOUT_SRC_REG_s VoutSrcReg;
    B8_PACKER_CONFIG_s PackerCfg;

    if ((VoutChannel >= AMBA_NUM_B8_VOUT_CHANNEL) || (VoutSource >= AMBA_NUM_B8_VOUT_SOURCE)) {
        RetVal = B8_ERR_ARG;
    } else if ((VoutSource == B8_VOUT_SOURCE_BYPASS_VIN0) || (VoutSource == B8_VOUT_SOURCE_BYPASS_VIN1)) {
        /* Change clk_source to spclk0 to avoid clock domain issue */
        (void)AmbaB8_PllSetVoutClkSrc(ChipID, VoutChannel, B8_PLL_VO_SENSOR_SRC_SPCLK0);

        /* Change data path*/
        (void)AmbaB8_RegRead(ChipID, & (pAmbaB8_ScratchpadReg->VoutSrc), 0, B8_DATA_WIDTH_32BIT, 1, &VoutSrcReg);

        if (VoutSource == B8_VOUT_SOURCE_BYPASS_VIN0 ) {
            VoutSrcReg.Vin0BypassEn = 1;
        } else {
            VoutSrcReg.Vin1BypassEn = 1;
        }
        if (VoutChannel == B8_VOUT_CHANNEL0) {
            (void)AmbaB8_PllSetVout0RefFreq(ChipID, pDisplayConfig->VideoTiming.PixelClock);
            VoutSrcReg.Vout0SrcEn = 1;
            VoutSrcReg.Vout0SrcSel = 1;
        } else {
            (void)AmbaB8_PllSetVout1RefFreq(ChipID, pDisplayConfig->VideoTiming.PixelClock);
            VoutSrcReg.Vout1SrcEn = 1;
            VoutSrcReg.Vout1SrcSel = 1;
        }
        RetVal = AmbaB8_RegWrite(ChipID, & (pAmbaB8_ScratchpadReg->VoutSrc), 0, B8_DATA_WIDTH_32BIT, 1, &VoutSrcReg);

    } else {
        /* B8N packer */
        PackerCfg.PelDepth = B8_PACKER_12PIXEL_DEPTH;
        PackerCfg.PktByteNumMin = 64;
        PackerCfg.PktByteNumMax = 239;
        AmbaB8_PackerConfig(ChipID & B8_MAIN_CHIP_ID_B8N_MASK, &PackerCfg);

        /* B8N scratchpad */
        DataBuf[0] = 0xc0cf00c0U;
        (void) AmbaB8_RegWriteU32(ChipID & B8_MAIN_CHIP_ID_B8N_MASK, 0xe000e800U, 0, B8_DATA_WIDTH_32BIT, 1, DataBuf);
        DataBuf[0] = 0x15U;
        (void) AmbaB8_RegWriteU32(ChipID & B8_MAIN_CHIP_ID_B8N_MASK, 0xe000e8c4U, 0, B8_DATA_WIDTH_32BIT, 1, DataBuf);

        /* Update vout source */
        (void) AmbaB8_RegRead(ChipID, & (pAmbaB8_ScratchpadReg->VoutSrc), 0, B8_DATA_WIDTH_32BIT, 1, &VoutSrcReg);

        if (VoutChannel == B8_VOUT_CHANNEL0) {
            VoutSrcReg.Vout0SrcSel = (UINT8)VoutSource;
            VoutSrcReg.Vout0SrcEn = 1U;
            VoutSrcReg.Vout1SrcEn = 0U;
        } else { /*if (VoutChannel == B8_VOUT_CHANNEL1) { */
            VoutSrcReg.Vout1SrcSel = (UINT8)VoutSource;
            VoutSrcReg.Vout1SrcEn = 1U;
            VoutSrcReg.Vout0SrcEn = 0U;
        }

        RetVal = AmbaB8_RegWrite(ChipID, & (pAmbaB8_ScratchpadReg->VoutSrc), 0, B8_DATA_WIDTH_32BIT, 1, &VoutSrcReg);

        if (VoutSource == B8_VOUT_SOURCE_MERGER) {
            /* Program data path: Vinf -> Decomp -> Codec -> Merger */
            /* Vinf*/
            DataBuf[0] = ((((UINT32)ActiveWidth * 2U) - 1U) << 16U) | ((UINT32)ActiveHeight - 1U);
            DataBuf[1] = 0xffffU;
            (void) AmbaB8_RegWriteU32(ChipID, 0xe000e808U, 1, B8_DATA_WIDTH_32BIT, 2, DataBuf);

            /* Decompressor */
            DataBuf16[0] = 0x34U;
            DataBuf16[1] = ((UINT16)(ActiveWidth * 2U) - 1U);
            DataBuf16[2] = ActiveHeight - 1U;
            DataBuf16[3] = 0x0U;
            DataBuf16[4] = 0xd500U;
            DataBuf16[5] = 0x2b97U;
            DataBuf16[6] = 0x7d01U;
            DataBuf16[7] = 0xbc2U;
            DataBuf16[8] = 0x0U;
            DataBuf16[9] = 0x1U;
            (void) AmbaB8_RegWriteU32(ChipID, 0xe000e000U, 1, B8_DATA_WIDTH_16BIT, 10, DataBuf16);

            /* Merger */
            DataBuf[0] = 0xE4U;
            (void) AmbaB8_RegWriteU32(ChipID, 0xe0009008U, 0, B8_DATA_WIDTH_32BIT, 1, DataBuf);
            DataBuf[0] = 0x8000U;
            (void) AmbaB8_RegWriteU32(ChipID, 0xe000900CU, 0, B8_DATA_WIDTH_32BIT, 1, DataBuf);
            DataBuf[0] = (((UINT32)ActiveWidth * 2U) << 16U) | ((UINT32)ActiveWidth * 2U);
            (void) AmbaB8_RegWriteU32(ChipID, 0xe0009010U, 0, B8_DATA_WIDTH_32BIT, 1, DataBuf);
            DataBuf[0] = (((UINT32)ActiveWidth * 2U) << 16U) | ((UINT32)ActiveWidth * 2U);
            (void) AmbaB8_RegWriteU32(ChipID, 0xe0009014U, 0, B8_DATA_WIDTH_32BIT, 1, DataBuf);
            DataBuf[0] = ((UINT32)0x300cU << 12U) | ((UINT32)ActiveHeight - 1U);
            (void) AmbaB8_RegWriteU32(ChipID, 0xe000902CU, 0, B8_DATA_WIDTH_32BIT, 1, DataBuf);
            DataBuf[0] = 0x2FFFU;
            (void) AmbaB8_RegWriteU32(ChipID, 0xe0009030U, 0, B8_DATA_WIDTH_32BIT, 1, DataBuf);
            DataBuf[0] = ((UINT32)0xB40U << 16U) | ((UINT32)ActiveWidth * 2U);
            (void) AmbaB8_RegWriteU32(ChipID, 0xe0009034U, 0, B8_DATA_WIDTH_32BIT, 1, DataBuf);
            DataBuf[0] = ((UINT32)0xB40U << 16U) | ((UINT32)ActiveWidth * 2U);
            (void) AmbaB8_RegWriteU32(ChipID, 0xe0009038U, 0, B8_DATA_WIDTH_32BIT, 1, DataBuf);
            DataBuf[0] = ((UINT32)0xB40U << 16U) | ((UINT32)ActiveWidth * 2U);
            (void) AmbaB8_RegWriteU32(ChipID, 0xe000903CU, 0, B8_DATA_WIDTH_32BIT, 1, DataBuf);
            DataBuf[0] = ((UINT32)0xB40U << 16U) | ((UINT32)ActiveWidth * 2U);
            (void) AmbaB8_RegWriteU32(ChipID, 0xe0009040U, 0, B8_DATA_WIDTH_32BIT, 1, DataBuf);
            DataBuf[0] = 0x1U;
            (void) AmbaB8_RegWriteU32(ChipID, 0xe0009064U, 0, B8_DATA_WIDTH_32BIT, 1, DataBuf);
            DataBuf[0] = 0x1U;
            (void) AmbaB8_RegWriteU32(ChipID, 0xe0009070U, 0, B8_DATA_WIDTH_32BIT, 1, DataBuf);
            DataBuf[0] = 0x511U;
            (void) AmbaB8_RegWriteU32(ChipID, 0xe0009000U, 0, B8_DATA_WIDTH_32BIT, 1, DataBuf);
        }
    }

    return RetVal;
}

