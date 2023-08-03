/**
*  @file Amba_AwbCalib.c
*
*  Copyright (c) 2020 Ambarella International LP
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
*  This file includes sample code and is only for internal testing and evaluation.  If you
*  distribute this sample code (whether in source, object, or binary code form), it will be
*  without any warranty or indemnity protection from Ambarella International LP or its affiliates.
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
*  @Amba WB calibration
*
*/
#include "AmbaTypes.h"
#include "AmbaPrint.h"
#if defined(CONFIG_QNX) || defined(CONFIG_LINUX)
#include "AmbaWrap.h"
#endif
#include "AmbaImg_External_CtrlFunc.h"
#include "Amba_AwbCalib.h"
#include "Amba_AwbCalibTskSample.h"

extern void awb_get_cal_wb_rgb(AMBA_IK_WB_GAIN_s *RGB_value);

static UINT8    AwbCalibTskInitFlag = 1U;
static WB_CALIB_INFO_s WbCalibInfo[MAX_VIEW_NUM] __attribute__((section(".bss.noinit")));
static WB_COMP_INFO_s WbCompInfo[MAX_VIEW_NUM] = {0};

static void Interpolation1D(UINT32 x, const UINT32 *pInput, const UINT32 *pValue, UINT32 *y)
{
    UINT32 Input[2], Value[2];
    // Sort
    if (pInput[0] <= pInput[1]) {
        Input[0] = pInput[0];
        Input[1] = pInput[1];
        Value[0] = pValue[0];
        Value[1] = pValue[1];
    } else {
        Input[0] = pInput[1];
        Input[1] = pInput[0];
        Value[0] = pValue[1];
        Value[1] = pValue[0];
    }

    if (x <= Input[0]) {
        *y = Value[0];
    } else if (x >= Input[1]) {
        *y = Value[1];
    } else {
        *y = ((Value[0] * (Input[1] - x)) + (Value[1] * (x - Input[0]))) / (Input[1] - Input[0]);
    }
}

UINT32 ImgProc_WbCalibCalculateWbCompInfo(UINT32 ViewID)
{

    // Get WB compensation information
    (void)AmbaImgProc_AWBGetWBCompSetInfo(ViewID, &WbCompInfo[ViewID].set_gain[0], &WbCompInfo[ViewID].set_gain[1]);
    (void)AmbaImgProc_AWBGetWBCompTargetInfo(ViewID, &WbCompInfo[ViewID].target_gain[0], &WbCompInfo[ViewID].target_gain[1]);

    // R, G, B Average to R, G, B Gain
    WbCompInfo[ViewID].set_gain[0].GainR = (WbCompInfo[ViewID].set_gain[0].GainG * WB_UNIT_GAIN) / WbCompInfo[ViewID].set_gain[0].GainR;
    WbCompInfo[ViewID].set_gain[0].GainB = (WbCompInfo[ViewID].set_gain[0].GainG * WB_UNIT_GAIN) / WbCompInfo[ViewID].set_gain[0].GainB;
    WbCompInfo[ViewID].set_gain[0].GainG = WB_UNIT_GAIN;

    WbCompInfo[ViewID].set_gain[1].GainR = (WbCompInfo[ViewID].set_gain[1].GainG * WB_UNIT_GAIN) / WbCompInfo[ViewID].set_gain[1].GainR;
    WbCompInfo[ViewID].set_gain[1].GainB = (WbCompInfo[ViewID].set_gain[1].GainG * WB_UNIT_GAIN) / WbCompInfo[ViewID].set_gain[1].GainB;
    WbCompInfo[ViewID].set_gain[1].GainG = WB_UNIT_GAIN;

    WbCompInfo[ViewID].target_gain[0].GainR = (WbCompInfo[ViewID].target_gain[0].GainG * WB_UNIT_GAIN) / WbCompInfo[ViewID].target_gain[0].GainR;
    WbCompInfo[ViewID].target_gain[0].GainB = (WbCompInfo[ViewID].target_gain[0].GainG * WB_UNIT_GAIN) / WbCompInfo[ViewID].target_gain[0].GainB;
    WbCompInfo[ViewID].target_gain[0].GainG = WB_UNIT_GAIN;

    WbCompInfo[ViewID].target_gain[1].GainR = (WbCompInfo[ViewID].target_gain[1].GainG * WB_UNIT_GAIN) / WbCompInfo[ViewID].target_gain[1].GainR;
    WbCompInfo[ViewID].target_gain[1].GainB = (WbCompInfo[ViewID].target_gain[1].GainG * WB_UNIT_GAIN) / WbCompInfo[ViewID].target_gain[1].GainB;
    WbCompInfo[ViewID].target_gain[1].GainG = WB_UNIT_GAIN;

    // Calculate delta
    WbCompInfo[ViewID].delta[0].GainR = WbCompInfo[ViewID].target_gain[0].GainR - WbCompInfo[ViewID].set_gain[0].GainR;
    WbCompInfo[ViewID].delta[0].GainG = WbCompInfo[ViewID].target_gain[0].GainG - WbCompInfo[ViewID].set_gain[0].GainG;
    WbCompInfo[ViewID].delta[0].GainB = WbCompInfo[ViewID].target_gain[0].GainB - WbCompInfo[ViewID].set_gain[0].GainB;

    WbCompInfo[ViewID].delta[1].GainR = WbCompInfo[ViewID].target_gain[1].GainR - WbCompInfo[ViewID].set_gain[1].GainR;
    WbCompInfo[ViewID].delta[1].GainG = WbCompInfo[ViewID].target_gain[1].GainG - WbCompInfo[ViewID].set_gain[1].GainG;
    WbCompInfo[ViewID].delta[1].GainB = WbCompInfo[ViewID].target_gain[1].GainB - WbCompInfo[ViewID].set_gain[1].GainB;

    return 0U;
}

UINT32 ImgProc_WbCalibTargetToSet(UINT32 ViewID,  AMBA_IK_WB_GAIN_s *pWbGain)
{
    UINT32 gain[2], delta[2], d;
    INT32  tmp;

    // For R
    gain[0] = WbCompInfo[ViewID].target_gain[0].GainR;
    gain[1] = WbCompInfo[ViewID].target_gain[1].GainR;
    delta[0] = WbCompInfo[ViewID].delta[0].GainR;
    delta[1] = WbCompInfo[ViewID].delta[1].GainR;

    (void)Interpolation1D(pWbGain->GainR, gain, delta, &d);
    tmp = (INT32)pWbGain->GainR - (INT32)d;
    if (tmp < 0) {
        pWbGain->GainR = 0;
    } else {
        pWbGain->GainR = (UINT32)tmp;
    }

    // For B
    gain[0] = WbCompInfo[ViewID].target_gain[0].GainB;
    gain[1] = WbCompInfo[ViewID].target_gain[1].GainB;
    delta[0] = WbCompInfo[ViewID].delta[0].GainB;
    delta[1] = WbCompInfo[ViewID].delta[1].GainB;

    (void)Interpolation1D(pWbGain->GainB, gain, delta, &d);
    tmp = (INT32)pWbGain->GainB - (INT32)d;
    if (tmp < 0) {
        pWbGain->GainB = 0;
    } else {
        pWbGain->GainB = (UINT32)tmp;
    }

    return 0U;
}

UINT32 ImgProc_WBCalibSetToTarget(UINT32 ViewID, AMBA_IK_WB_GAIN_s *pWbGain)
{
    UINT32 gain[2], delta[2], d;
    INT32 tmp;

    //For R
    gain[0] = WbCompInfo[ViewID].set_gain[0].GainR;
    gain[1] = WbCompInfo[ViewID].set_gain[1].GainR;
    delta[0] = WbCompInfo[ViewID].delta[0].GainR;
    delta[1] = WbCompInfo[ViewID].delta[1].GainR;

    (void)Interpolation1D(pWbGain->GainR, gain, delta, &d);
    tmp = (INT32)pWbGain->GainR + (INT32)d;
    if (tmp < 0) {
        pWbGain->GainR = 0;
    } else {
        pWbGain->GainR = (UINT32)tmp;
    }

    // For B
    gain[0] = WbCompInfo[ViewID].set_gain[0].GainB;
    gain[1] = WbCompInfo[ViewID].set_gain[1].GainB;
    delta[0] = WbCompInfo[ViewID].delta[0].GainB;
    delta[1] = WbCompInfo[ViewID].delta[1].GainB;

    (void)Interpolation1D(pWbGain->GainB, gain, delta, &d);
    tmp = (INT32)pWbGain->GainB + (INT32)d;
    if (tmp < 0) {
        pWbGain->GainB = 0;
    } else {
        pWbGain->GainB = (UINT32)tmp;
    }

    return 0U;
}


UINT32 ImgProc_WbCalibSetEnable(UINT32 ViewID, UINT32 Enable)
{
    UINT32          ViewCount = 0U;

    if (AwbCalibTskInitFlag == 1U) {
        (void)AmbaImgProc_GetTotalViewCount(&ViewCount);
        AmbaImg_AwbCalibTask_Init(ViewCount);
        AwbCalibTskInitFlag = 0U;
    }

    WbCalibInfo[ViewID].WbCalibEnable = Enable;

    return 0U;
}

UINT32 ImgProc_WbCalibGetEnable(UINT32 ViewID, UINT32 *pEnable)
{
    if(AwbCalibTskInitFlag == 0U){
        *pEnable = WbCalibInfo[ViewID].WbCalibEnable;
    }else{
        *pEnable = 0U;
    }
    return 0U;
}

UINT32 ImgProc_WbCalibCtrl(UINT32 ViewID)
{
    static  UINT32                  i = 0U;
    static  AMBA_IK_CFA_3A_DATA_s   AAAStaticTmp  __attribute__((section(".bss.noinit")));
    AMBA_IK_WB_GAIN_s   wbgain;
    UINT32  U32RVal = 0U;

    U32RVal |= AmbaImgProc_AWBGetWBGain(ViewID, 0U, IP_MODE_VIDEO, &wbgain);
    AmbaPrint_ModulePrintUInt5(IMGPROC_PRINT_MODULE_ID, "wbgain: %d %d %d", wbgain.GainR, wbgain.GainG, wbgain.GainB, 0U, 0U);

    U32RVal |= AmbaImgProc_GetCFAAAAStat(ViewID, &AAAStaticTmp);

    U32RVal |= AmbaWrap_memcpy(&WbCalibInfo[ViewID].WbCalibData[i].AWBStatistic[0], &AAAStaticTmp.Awb[0], sizeof(AMBA_IK_CFA_AWB_s) * AMBA_IK_3A_AWB_TILE_ROW_COUNT * AMBA_IK_3A_AWB_TILE_COL_COUNT);
    U32RVal |= AmbaWrap_memcpy(&WbCalibInfo[ViewID].WbCalibData[i].Header, &AAAStaticTmp.Header, sizeof(AMBA_IK_3A_HEADER_s));
    if(U32RVal != 0U){
        /* MisraC */
    }

    i++;

    i = i % WB_CAL_FRAMES;

    return 0U;
}

static UINT32 ImgProc_WbCalibGetFrameAvgRGB(UINT32 ViewID, UINT32 FrameIdx, UINT32 *pAvgR, UINT32 *pAvgG, UINT32 *pAvgB)
{
    UINT32      row = 0U, col = 0U, currTile;
    UINT32      validRowStart, validRowEnd, validColStart, validColEnd;
    UINT16      shift;
    UINT32      rowCnt, colCnt;
    UINT32      pixelInTile;
    UINT64      RTmp = 0U, GTmp = 0U, BTmp = 0U, tilesCnt= 0U;


#if !defined(CONFIG_SOC_CV2FS) && !defined(CONFIG_SOC_CV22FS)&& !defined(CONFIG_SOC_CV5) && !defined(CONFIG_SOC_CV52)
    /*Calculate pixelInTile*/
    pixelInTile = (UINT32)WbCalibInfo[ViewID].WbCalibData[FrameIdx].Header.AwbTileWidth * (UINT32)WbCalibInfo[ViewID].WbCalibData[FrameIdx].Header.AwbTileHeight / 4U; /*If user care about performance, replace to shift*/

    /*Define valid wb tile  area*/
    rowCnt = (UINT32)(WbCalibInfo[ViewID].WbCalibData[FrameIdx].Header.AwbTileNumCol);
    colCnt = (UINT32)(WbCalibInfo[ViewID].WbCalibData[FrameIdx].Header.AwbTileNumRow);
#else
    /*Calculate pixelInTile*/
    pixelInTile = (UINT32)WbCalibInfo[ViewID].WbCalibData[FrameIdx].Header.Awb.AwbTileWidth * (UINT32)WbCalibInfo[ViewID].WbCalibData[FrameIdx].Header.Awb.AwbTileHeight / 4U; /*If user care about performance, replace to shift*/

    /*Define valid wb tile  area*/
    rowCnt = (UINT32)(WbCalibInfo[ViewID].WbCalibData[FrameIdx].Header.Awb.AwbTileNumCol);
    colCnt = (UINT32)(WbCalibInfo[ViewID].WbCalibData[FrameIdx].Header.Awb.AwbTileNumRow);
#endif
    validRowStart = ((rowCnt * 1U) / 4U); /*If user care about performance, replace to shift*/
    validRowEnd = ((rowCnt * 3U) / 4U);

    validColStart = ((colCnt * 1U) / 4U);
    validColEnd = ((colCnt * 3U) / 4U);

    /*Get Average*/
    for (row = validRowStart; row < validRowEnd; row++) {
        for (col = validColStart; col < validColEnd; col++) {
            currTile = (row*colCnt) + col;
            pixelInTile -= ((UINT32)((WbCalibInfo[ViewID].WbCalibData[FrameIdx].AWBStatistic[currTile].CountMin) + (UINT32)(WbCalibInfo[ViewID].WbCalibData[FrameIdx].AWBStatistic[currTile].CountMax)) / 4U);

            shift = WbCalibInfo[ViewID].WbCalibData[FrameIdx].Header.AwbRgbShift;
            RTmp += ((UINT64)(WbCalibInfo[ViewID].WbCalibData[FrameIdx].AWBStatistic[currTile].SumR) << (UINT64)shift) / (UINT64)(pixelInTile);
            GTmp += ((UINT64)(WbCalibInfo[ViewID].WbCalibData[FrameIdx].AWBStatistic[currTile].SumG) << (UINT64)shift) / (UINT64)(pixelInTile);
            BTmp += ((UINT64)(WbCalibInfo[ViewID].WbCalibData[FrameIdx].AWBStatistic[currTile].SumB) << (UINT64)shift) / (UINT64)(pixelInTile);
            tilesCnt ++;
        }
    }

    if (tilesCnt != 0U) {
        RTmp = RTmp / tilesCnt;
        GTmp = GTmp / tilesCnt;
        BTmp = BTmp / tilesCnt;
    }

    *pAvgR = (UINT32)RTmp;
    *pAvgG = (UINT32)GTmp;
    *pAvgB = (UINT32)BTmp;

    return 0U;
}

static UINT32 ImgProc_WbCalibGetAvgRGB(UINT32 ViewID, UINT32 *pAvgR, UINT32 *pAvgG, UINT32 *pAvgB)
{
    UINT32              frame = 0U;
    UINT32              RTmp = 0U, GTmp = 0U, BTmp = 0U;
    UINT64              RSum = 0U, GSum = 0U, BSum = 0U;



    /*Calculate sum for r, g, b*/
    for (frame = 0U; frame < WB_CAL_FRAMES; frame++) {
        (void)ImgProc_WbCalibGetFrameAvgRGB(ViewID, frame, &RTmp, &GTmp, &BTmp);
        RSum += RTmp;
        GSum += GTmp;
        BSum += BTmp;
    }

    /*Compute Average of r, g, b*/
    RSum = RSum / WB_CAL_FRAMES;
    GSum = GSum / WB_CAL_FRAMES;
    BSum = BSum / WB_CAL_FRAMES;

    /*Normalize so that AvgG = 4096*/
    RSum = RSum * 4096U / GSum;
    BSum = BSum * 4096U / GSum;
    GSum = 4096U;

    *pAvgR = (UINT32)RSum;
    *pAvgG = (UINT32)GSum;
    *pAvgB = (UINT32)BSum;

    AmbaPrint_ModulePrintUInt5(IMGPROC_PRINT_MODULE_ID, "RBG Average: %d %d %d",*pAvgR, *pAvgG, *pAvgB, 0U, 0U);

    return 0U;
}

UINT32 ImgProc_WBCal(UINT32 ViewID, UINT32 Index, UINT32 FlickerMode, WB_Detect_Info_s *WbDetectInfo)
{

    AE_EV_LUT_s         evLut1, evLut2;
    AMBA_IK_WB_GAIN_s   AwbGain[2];
    AMBA_IK_WB_GAIN_s   UnitWBGain = {WB_UNIT_GAIN, WB_UNIT_GAIN, WB_UNIT_GAIN};
    AMBA_AAA_OP_INFO_s  AaaOpInfo1 = {0};
    AMBA_AAA_OP_INFO_s  AaaOpInfo2 = {0};
    UINT32              Rval = 0U;

    if ( (Index == WB_CAL_TEST) || (Index == WB_CAL_LCT) || (Index == WB_CAL_HCT)) {
        /*AaaOpInfo*/
        Rval |= AmbaImgProc_GetAAAOPInfo(ViewID, &AaaOpInfo1);
        AaaOpInfo2 = AaaOpInfo1;
        AaaOpInfo2.AeOp = 1U;
        AaaOpInfo2.AwbOp = 0U;
        Rval |= AmbaImgProc_SetAAAOPInfo(ViewID, &AaaOpInfo2);

        /*AE_EV_LUT*/
        Rval |= AmbaImgProc_AEGetAEEvLut(ViewID, &evLut1);
        evLut2 = evLut1;
        evLut2.FlickerMode = (UINT8)FlickerMode;
        Rval |= AmbaImgProc_AESetAEEvLut(ViewID, &evLut2);

        /*AMBA_AEAWBADJ_INIT*/
        Rval |= AmbaImgProc_AEInit(ViewID, 0U);
        // Rval |= AmbaImgProc_AWBInit(ViewID, 0U);

        /*Set pipe wb gain to WB_UNIT_GAIN for statistics collection*/
        (void)AmbaImgProc_AWBSetWBGain(ViewID, 0U, IP_MODE_VIDEO, &UnitWBGain);
        (void)AmbaKAL_TaskSleep(1000);
        /*Wait for wb calib data ready*/
        (void)ImgProc_WbCalibSetEnable(ViewID, 1U);
        (void)AmbaKAL_TaskSleep(2000);
        (void)ImgProc_WbCalibSetEnable(ViewID, 0U);
    }

    switch (Index) {

        case WB_CAL_TEST:   // Show the current RGB Avg Value
            (void)ImgProc_WbCalibGetAvgRGB(ViewID, &WbDetectInfo->CurR, &WbDetectInfo->CurG, &WbDetectInfo->CurB);

            break;

        case WB_CAL_LCT:    // LCT WB-Cal
            (void)ImgProc_WbCalibGetAvgRGB(ViewID, &WbDetectInfo->CurR, &WbDetectInfo->CurG, &WbDetectInfo->CurB);
            break;

        case WB_CAL_HCT:    // HCT WB-Cal
            (void)ImgProc_WbCalibGetAvgRGB(ViewID, &WbDetectInfo->CurR, &WbDetectInfo->CurG, &WbDetectInfo->CurB);
            break;

        case WB_CAL_STORE_LCT:  // Store LCT WB-Cal

            Rval |= AmbaImgProc_AWBGetWBCompSetInfo(ViewID, &AwbGain[0], &AwbGain[1]);
            AwbGain[0].GainR = WbDetectInfo->CurR;
            AwbGain[0].GainG = WbDetectInfo->CurG;
            AwbGain[0].GainB = WbDetectInfo->CurB;
            Rval |= AmbaImgProc_AWBSetWBCompSetInfo(ViewID, &AwbGain[0], &AwbGain[1]);

            Rval |= AmbaImgProc_AWBGetWBCompTargetInfo(ViewID, &AwbGain[0], &AwbGain[1]);
            AwbGain[0].GainR = WbDetectInfo->RefR;
            AwbGain[0].GainG = WbDetectInfo->RefG;
            AwbGain[0].GainB = WbDetectInfo->RefB;
            Rval |= AmbaImgProc_AWBSetWBCompTargetInfo(ViewID, &AwbGain[0], &AwbGain[1]);
            break;

        case WB_CAL_STORE_HCT:  // Store HCT WB-Cal

            Rval |= AmbaImgProc_AWBGetWBCompSetInfo(ViewID, &AwbGain[0], &AwbGain[1]);
            AwbGain[1].GainR = WbDetectInfo->CurR;
            AwbGain[1].GainG = WbDetectInfo->CurG;
            AwbGain[1].GainB = WbDetectInfo->CurB;
            Rval |= AmbaImgProc_AWBSetWBCompSetInfo(ViewID, &AwbGain[0], &AwbGain[1]);

            Rval |= AmbaImgProc_AWBGetWBCompTargetInfo(ViewID, &AwbGain[0], &AwbGain[1]);
            AwbGain[1].GainR = WbDetectInfo->RefR;
            AwbGain[1].GainG = WbDetectInfo->RefG;
            AwbGain[1].GainB = WbDetectInfo->RefB;
            Rval |= AmbaImgProc_AWBSetWBCompTargetInfo(ViewID, &AwbGain[0], &AwbGain[1]);
            break;

        case WB_CAL_RESET:  // Reset WB-Cal Value

            Rval |= AmbaImgProc_AWBGetWBCompSetInfo(ViewID, &AwbGain[0], &AwbGain[1]);
            AwbGain[0].GainR = WB_UNIT_GAIN;
            AwbGain[0].GainG = WB_UNIT_GAIN;
            AwbGain[0].GainB = WB_UNIT_GAIN;
            AwbGain[1].GainR = WB_UNIT_GAIN;
            AwbGain[1].GainG = WB_UNIT_GAIN;
            AwbGain[1].GainB = WB_UNIT_GAIN;
            Rval |= AmbaImgProc_AWBSetWBCompSetInfo(ViewID, &AwbGain[0], &AwbGain[1]);

            Rval |= AmbaImgProc_AWBGetWBCompTargetInfo(ViewID, &AwbGain[0], &AwbGain[1]);
            AwbGain[0].GainR = WB_UNIT_GAIN;
            AwbGain[0].GainG = WB_UNIT_GAIN;
            AwbGain[0].GainB = WB_UNIT_GAIN;
            AwbGain[1].GainR = WB_UNIT_GAIN;
            AwbGain[1].GainG = WB_UNIT_GAIN;
            AwbGain[1].GainB = WB_UNIT_GAIN;
            Rval |= AmbaImgProc_AWBSetWBCompTargetInfo(ViewID, &AwbGain[0], &AwbGain[1]);
            break;
        case WB_CAL_GET_LCT:    // Get LCT WB-Cal Value
            Rval |= AmbaImgProc_AWBGetWBCompSetInfo(ViewID, &AwbGain[0], &AwbGain[1]);
            WbDetectInfo->CurR = AwbGain[0].GainR;
            WbDetectInfo->CurG = AwbGain[0].GainG;
            WbDetectInfo->CurB = AwbGain[0].GainB;
            Rval |= AmbaImgProc_AWBGetWBCompTargetInfo(ViewID, &AwbGain[0], &AwbGain[1]);
            WbDetectInfo->RefR = AwbGain[0].GainR;
            WbDetectInfo->RefG = AwbGain[0].GainG;
            WbDetectInfo->RefB = AwbGain[0].GainB;
            break;
        case WB_CAL_GET_HCT:    // Get HCT WB-Cal Value
            Rval |= AmbaImgProc_AWBGetWBCompSetInfo(ViewID, &AwbGain[0], &AwbGain[1]);
            WbDetectInfo->CurR = AwbGain[1].GainR;
            WbDetectInfo->CurG = AwbGain[1].GainG;
            WbDetectInfo->CurB = AwbGain[1].GainB;
            Rval |= AmbaImgProc_AWBGetWBCompTargetInfo(ViewID, &AwbGain[0], &AwbGain[1]);
            WbDetectInfo->RefR = AwbGain[1].GainR;
            WbDetectInfo->RefG = AwbGain[1].GainG;
            WbDetectInfo->RefB = AwbGain[1].GainB;
            break;
        default:
            Rval |= IMGPROC_ERR_0003;
            break;
    }

    if ((Index == WB_CAL_TEST) || (Index == WB_CAL_LCT) || (Index == WB_CAL_HCT)) {
        Rval |= AmbaImgProc_SetAAAOPInfo(ViewID, &AaaOpInfo1);
        Rval |= AmbaImgProc_AESetAEEvLut(ViewID, &evLut1);
        // Rval |= AmbaImgProc_AWBSetAWBCtrlCap(ViewID, &awbControlMode1);

        /*AMBA_AEAWBADJ_INIT*/
        Rval |= AmbaImgProc_AEInit(ViewID, 0U);
        // Rval |= AmbaImgProc_AWBInit(ViewID, 0U);
    }

    return Rval;
}







