/**
 * @file Amba_AdjTableSelectSample.c
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
 *
 */


// #include ".svc_autogen"

#include "AmbaTypes.h"
#include "AmbaPrint.h"
#include "AmbaImg_Proc.h"
#ifdef SVC_AMBAIMG_ADJUSTMENT
#include SVC_AMBAIMG_ADJUSTMENT
#endif
#include "AmbaImg_AaaDef.h"
#include "AmbaIQParamHandlerSample.h"
#include "Amba_AdjTableSelectSample.h"
#include "AmbaImg_External_CtrlFunc.h"
// #include "AmbaImg_CtrlFunc_Def.h"

static UINT8     TableNo[MAX_VIEW_NUM] = {0U};
static UINT32    PreviousDefine[MAX_VIEW_NUM] = {0U};
//Complexity
static UINT32 Get_Sensor_Res_Sub111(const LIVEVIEW_INFO_s *pLiveviewInfo)
{
    UINT32 Rval = 0U;
    if ((pLiveviewInfo->MainW == 352U) && (pLiveviewInfo->MainH == 240U)){
        Rval = 116U;
    }else if ((pLiveviewInfo->MainW == 320U) && (pLiveviewInfo->MainH == 240U)){
        Rval = 117U;
    }else if ((pLiveviewInfo->MainW == 3840U) && (pLiveviewInfo->MainH == 2160U)){
        Rval = 118U;
    }else{
        Rval = 100U;
    }
    return Rval;
}

static UINT32 Get_Sensor_Res_Sub11(const LIVEVIEW_INFO_s *pLiveviewInfo)
{
    UINT32 Rval = 0U;
    if ((pLiveviewInfo->MainW == 848U) && (pLiveviewInfo->MainH == 480U)){
        Rval = 111U;
    }else if ((pLiveviewInfo->MainW == 720U) && (pLiveviewInfo->MainH == 480U)){
        Rval = 112U;
    }else if ((pLiveviewInfo->MainW == 640U) && (pLiveviewInfo->MainH == 480U)){
        Rval = 113U;
    }else if ((pLiveviewInfo->MainW == 640U) && (pLiveviewInfo->MainH == 360U)){
        Rval = 114U;
    }else if ((pLiveviewInfo->MainW == 432U) && (pLiveviewInfo->MainH == 240U)){
        Rval = 115U;
    }else {
        Rval = Get_Sensor_Res_Sub111(pLiveviewInfo);
    }
    return Rval;
}

static UINT32 Get_Sensor_Res_Sub1(const LIVEVIEW_INFO_s *pLiveviewInfo)
{
    UINT32 Rval = 0U;
    if ((pLiveviewInfo->MainW == 1600U) && (pLiveviewInfo->MainH == 900U)){
        Rval = 106U;
    }else if ((pLiveviewInfo->MainW == 1440U) && (pLiveviewInfo->MainH == 1080U)){
        Rval = 107U;
    }else if ((pLiveviewInfo->MainW == 1280U) && (pLiveviewInfo->MainH == 720U)){
        Rval = 108U;
    }else if ((pLiveviewInfo->MainW == 1280U) && (pLiveviewInfo->MainH == 960U)){
        Rval = 109U;
    }else if ((pLiveviewInfo->MainW == 960U) && (pLiveviewInfo->MainH == 540U)){
        Rval = 110U;
    }else{
        Rval = Get_Sensor_Res_Sub11(pLiveviewInfo);
    }
    return Rval;
}
//Complexity
static UINT32 Get_Sensor_Resolition(const LIVEVIEW_INFO_s *pLiveviewInfo)
{
    UINT32 Rval = 0U;

    if ((pLiveviewInfo->MainW == 2560U) && (pLiveviewInfo->MainH == 1440U)){
        Rval = 101U;
    }else if ((pLiveviewInfo->MainW == 2560U) && (pLiveviewInfo->MainH == 1080U)){
        Rval = 102U;
    }else if ((pLiveviewInfo->MainW == 2304U) && (pLiveviewInfo->MainH == 1296U)){
        Rval = 103U;
    }else if ((pLiveviewInfo->MainW == 1920U) && (pLiveviewInfo->MainH == 1080U)){
        Rval = 104U;
    }else if ((pLiveviewInfo->MainW == 1600U) && (pLiveviewInfo->MainH == 1200U)){
        Rval = 105U;
    }else{
        Rval = Get_Sensor_Res_Sub1(pLiveviewInfo);
    }

    return Rval;
}

static UINT32 Get_Frame_Rate(const LIVEVIEW_INFO_s *pLiveviewInfo)
{
    UINT32 Rval;

    switch (pLiveviewInfo->FrameRateInt){

        case 15U:
            Rval = 1U;
            break;
        case 24U:
            Rval = 2U;
            break;
        case 25U:
            Rval = 3U;
            break;
        case 30U:
            Rval = 4U;
            break;
        case 40U:
            Rval = 5U;
            break;
        case 45U:
            Rval = 6U;
            break;
        case 48U:
            Rval = 7U;
            break;
        case 50U:
            Rval = 8U;
            break;
        case 60U:
            Rval = 9U;
            break;
        case 100U:
            Rval = 10U;
            break;
        case 120U:
            Rval = 11U;
            break;
        case 200U:
            Rval = 12U;
            break;
        case 240U:
            Rval = 13U;
            break;
        default:
            Rval = 0U;
            break;
    }
    return Rval;
}

static UINT32 Get_Sensor_Binning(const LIVEVIEW_INFO_s *pLiveviewInfo)
{
    UINT16            CurrBinning;
    UINT32            Rval = 0;

    //CurrBinning = 0;
    //if ((pLiveviewInfo->BinningVNum != 0U) && (pLiveviewInfo->BinningHNum != 0U)){
    //    CurrBinning = ((UINT16)pLiveviewInfo->BinningHDen * (UINT16)pLiveviewInfo->BinningVDen) /
    //                        ((UINT16)pLiveviewInfo->BinningHNum * (UINT16)pLiveviewInfo->BinningVNum);
    //}
    CurrBinning = pLiveviewInfo->BinningVDen;

    switch (CurrBinning){
        case 1:
            Rval = 1;
            break;
        case 2:
            Rval = 2;
            break;
        case 3:
            Rval = 3;
            break;
        case 4:
            Rval = 4;
            break;
        case 8:
            Rval = 5;
            break;
        default:
            Rval = 0;
            break;
    }
    return Rval;
}



static void Get_VideoMode(const LIVEVIEW_INFO_s *pLiveviewInfo, ADJ_VIDEO_MODE_s *pVideoMode)
{
    UINT32          Define = 0;
    static UINT8    VideoColorStyle = 0;

    pVideoMode->ColorStyle = VideoColorStyle;

    pVideoMode->Resolution = Get_Sensor_Resolition(pLiveviewInfo);
    pVideoMode->FrameRate  = Get_Frame_Rate(pLiveviewInfo);
    pVideoMode->OverSampling = pLiveviewInfo->OverSamplingEnable + 1U;
    pVideoMode->ColorStyle += 1U;
    pVideoMode->Binning = Get_Sensor_Binning(pLiveviewInfo);

    Define = pVideoMode->Resolution;

    Define *= 100U;
    Define += pVideoMode->FrameRate;

    Define *= 10U;
    Define += pVideoMode->OverSampling;

    Define *= 10U;
    Define += pVideoMode->ColorStyle;

    Define *= 100U;
    Define += pVideoMode->Binning;

    pVideoMode->Define = Define;
}

static void Separate_Video_Mode(ADJ_VIDEO_MODE_s *VideoMode)
{
    UINT32                    Define;

    Define = VideoMode->Define;
    VideoMode->Binning = Define % 100U;
    Define /= 100U;
    VideoMode->ColorStyle = (UINT8)(Define % 10U);
    Define /= 10U;
    VideoMode->OverSampling = (UINT8)(Define % 10U);
    Define /= 10U;
    VideoMode->FrameRate = Define % 100U;
    Define /= 100U;
    VideoMode->Resolution = Define;
}

static UINT8 Compare_Video_Mode(const ADJ_VIDEO_MODE_s *pDeviceVideoMode, UINT32 videoModeDefine)
{
    ADJ_VIDEO_MODE_s        VideoMode;
    UINT8                   rVal;

    if (videoModeDefine == 0U){
        rVal = 0;
    }
    else {
        VideoMode.Define = videoModeDefine;

        Separate_Video_Mode(&VideoMode);

        if ((VideoMode.Resolution != 100U) && (VideoMode.Resolution != pDeviceVideoMode->Resolution)){
            rVal = 0U;
        }
        else if ((VideoMode.FrameRate != 0U) && (VideoMode.FrameRate != pDeviceVideoMode->FrameRate)){
            rVal = 0U;
        }
        else if ((VideoMode.OverSampling != 0U) && (VideoMode.OverSampling != pDeviceVideoMode->OverSampling)){
            rVal = 0U;
        }
        else if ((VideoMode.ColorStyle != 0U) && (VideoMode.ColorStyle != pDeviceVideoMode->ColorStyle)){
            rVal = 0U;
        }
        else if ((VideoMode.Binning != 0U) && (VideoMode.Binning != pDeviceVideoMode->Binning)){
            rVal = 0U;
        }else {
            rVal = 1U;
        }
    }
    return (rVal);

}


static UINT8 Search_AdjTable_Idx(UINT32 viewID, const ADJ_VIDEO_MODE_s *pDeviceVideoMode, const ADJ_TABLE_IDX_s *pAdjTableIdx)
{

    UINT32              DefineTmp = 0;
    UINT32              i = 0;
    UINT32              j = 0;
    UINT8               TableHasDecided = 0;

    if (PreviousDefine[viewID] != pDeviceVideoMode->Define) {

        PreviousDefine[viewID] = pDeviceVideoMode->Define;

        for(i=0U; i<pAdjTableIdx->MaxTableCount; i++) {
            for(j=0U; j<pAdjTableIdx->MaxTableSame; j++) {
                if((Compare_Video_Mode(pDeviceVideoMode, pAdjTableIdx->Index[i][j]) != 0U) && (TableHasDecided == 0U)) {
                    TableNo[viewID] = (UINT8)i;
                    TableHasDecided = 1;
                }
            }
        }

        DefineTmp = pDeviceVideoMode->Define % 1000U;
        for(i=0U; i<pAdjTableIdx->MaxTableCount; i++) {
            for(j=0U; j<pAdjTableIdx->MaxTableSame; j++) {
                if(((DefineTmp == pAdjTableIdx->Index[i][j]) && (TableHasDecided == 0U))) {
                    TableNo[viewID] = (UINT8)i;
                    TableHasDecided = 1;
                }
            }
        }

        if(TableHasDecided == 0U) {
            AmbaPrint_ModulePrintStr5(IMGPROC_PRINT_MODULE_ID, "----No this adj table mode ", NULL, NULL, NULL, NULL, NULL);
        }
    }
    return TableNo[viewID];
}



void Amba_ADJTableSelect_Decide_ADJTable(UINT32 viewID, UINT32 photoPreview, const LIVEVIEW_INFO_s *pLiveviewInfo, UINT8 *pTableNo)
{
    ADJ_VIDEO_MODE_s            DeviceVideoMode;
    IQ_PARAM_s                  *pIQParams = NULL;

    Get_VideoMode(pLiveviewInfo, &DeviceVideoMode);

    (void)Amba_IQParam_Get_IqParams(viewID, &pIQParams);

    if (photoPreview == 1U){
        *pTableNo = Search_AdjTable_Idx(viewID, &DeviceVideoMode, &pIQParams->ADJTableParam.PhotoPreview);
    }else{
        *pTableNo = Search_AdjTable_Idx(viewID, &DeviceVideoMode, &pIQParams->ADJTableParam.Video);
    }

}


void Amba_ADJGetVideoTable(UINT8 ViewID, UINT8 *pAdjTable,UINT32 *pModeDef)
{
    *pAdjTable = TableNo[ViewID];
    *pModeDef  = PreviousDefine[ViewID];
}