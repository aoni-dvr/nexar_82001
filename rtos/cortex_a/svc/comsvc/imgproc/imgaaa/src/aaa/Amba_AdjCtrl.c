/**
 * @file Amba_AdjCtrl.c
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
 *  @Description    :: Sample Auto Adjust algorithm
 *
 */

// #include ".svc_autogen"

#include "AmbaTypes.h"
#include "AmbaPrint.h"
#include "AmbaWrap.h"

#include "AmbaDSP_ImageFilter.h"

#include "AmbaDSP_Image3aStatistics.h"
#include "AmbaImg_Proc.h"
#include "AmbaImg_AeAwb.h"
#include "Amba_AeAwbAdj_Control.h"
#ifdef SVC_AMBAIMG_ADJUSTMENT
#include SVC_AMBAIMG_ADJUSTMENT
#endif
#include "AmbaImg_AaaDef.h"

#include "AmbaImg_External_CtrlFunc.h"
#include "AmbaIQParamHandlerSample.h"
#include "Amba_AdjTableSelectSample.h"

#define UPDATE_FLAG_ON (UINT8)1

static UINT8 AmbaAdjMaunalEnable[MAX_VIEW_NUM] = {0};
static UINT8 AmbaAdjMaunalTableNo[MAX_VIEW_NUM] = {0};

static INT32 AmbaAdjTableNo[MAX_VIEW_NUM] = {-1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1};

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: SDecide_AdjTableNo
 *
 *  @Description:: decide ADJ table number
 *
 *  @Input      ::
 *    none
 *
 *  @Output     ::
 *   UINT32 *pAdjTableNo : pointer to ADJ table number
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
static void SDecide_AdjTableNo(UINT32 ViewId, UINT8 *pAdjTableNo)
{
    /*
    *  Setup conditions to choose ADJ table number
    */
    if(AmbaAdjTableNo[ViewId] > 0){
        *pAdjTableNo = (UINT8)AmbaAdjTableNo[ViewId];
    }
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: Amba_Set_AdjTableNo
 *
 *  @Description:: Set ADJ table number
 *
 *  @Input      ::
 *   UINT32 AdjTableNo : the ADJ table number
 *
 *  @Output     ::
 *   none
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
void Amba_Set_AdjTableNo(UINT32 ViewId, INT32 AdjTableNo)
{
    //
    //  Setup conditions to choose ADJ table number
    //
    AmbaAdjTableNo[ViewId] = AdjTableNo;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: _Decide_StillAdjTableNo
 *
 *  @Description:: decide Still ADJ table number
 *
 *  @Input      ::
 *    none
 *
 *  @Output     ::
 *   UINT32 *pAdjTableNo : pointer to ADJ table number
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
// static void _Decide_StillAdjTableNo(const UINT32 *pAdjTableNo)
// {
//     //
//     //  Setup conditions to choose ADJ table number
//     //
//     //  *pAdjTableNo = 0;
//

// }


INT32 App_Image_Init_Ae_Tile_Config(UINT32 ViewId, const AMBA_IK_MODE_CFG_s *mode)
{
    INT32                       RVal = 0;
    UINT32                      U32RVal = 0U;//20210809
    IMG_PARAM_s                 ImgParam;
#if !defined(CONFIG_SOC_CV2FS) && !defined(CONFIG_SOC_CV22FS)&& !defined(CONFIG_SOC_CV5)&& !defined(CONFIG_SOC_CV52)
    INT32                       MemcmpRet;
    AMBA_IK_AAA_STAT_INFO_s     AaaStat, AaaStatTmp;
#else
    AMBA_IK_AE_STAT_INFO_s      AeStat;
#endif
    RVal = (INT32)AmbaImgProc_GetImgParam(ViewId, &ImgParam);

    if (RVal != 0) {
        AmbaPrint_ModulePrintStr5(IMGPROC_PRINT_MODULE_ID, "---- %s, Get ImgParam Error!! ----",__func__, NULL, NULL, NULL, NULL);
        RVal = -1;
    } else {
#if !defined(CONFIG_SOC_CV2FS) && !defined(CONFIG_SOC_CV22FS)&& !defined(CONFIG_SOC_CV5)&& !defined(CONFIG_SOC_CV52)
        U32RVal |= AmbaIK_GetAaaStatInfo(mode, &AaaStatTmp);
        if(U32RVal != 0U){
            /* MisaC */
        }
        AaaStat = AaaStatTmp;

        AaaStat.AeTileNumCol   = ImgParam.AaaStatisticsInfo.AeTileNumCol;
        AaaStat.AeTileNumRow   = ImgParam.AaaStatisticsInfo.AeTileNumRow;
        AaaStat.AeTileColStart = ImgParam.AaaStatisticsInfo.AeTileColStart;
        AaaStat.AeTileRowStart = ImgParam.AaaStatisticsInfo.AeTileRowStart;
        AaaStat.AeTileWidth    = ImgParam.AaaStatisticsInfo.AeTileWidth;
        AaaStat.AeTileHeight   = ImgParam.AaaStatisticsInfo.AeTileHeight;
        AaaStat.AePixMinValue  = ImgParam.AaaStatisticsInfo.AePixMinValue;
        AaaStat.AePixMaxValue  = ImgParam.AaaStatisticsInfo.AePixMaxValue;
        U32RVal |= AmbaWrap_memcmp(&AaaStat, &AaaStatTmp, sizeof(AaaStat), &MemcmpRet);
        if(U32RVal != 0U){
            /* MisaC */
        }
        if (MemcmpRet !=0) {
            U32RVal |= AmbaIK_SetAaaStatInfo(mode, &AaaStat);
            if(U32RVal != 0U){
                /* MisaC */
            }
            AmbaPrint_ModulePrintStr5(IMGPROC_PRINT_MODULE_ID, "%s, update Ae tile settings", __func__, NULL, NULL, NULL, NULL);
            AmbaPrint_ModulePrintUInt5(IMGPROC_PRINT_MODULE_ID, "row = %d, col = %d", AaaStat.AeTileNumRow, AaaStat.AeTileNumCol, 0U, 0U, 0U);
        }
#else
        U32RVal |= AmbaIK_GetAeStatInfo(mode, &AeStat);
        if(U32RVal != 0U){
            /* MisaC */
        }
        AeStat.AeTileNumCol   = ImgParam.AaaStatisticsInfo.AeTileNumCol;
        AeStat.AeTileNumRow   = ImgParam.AaaStatisticsInfo.AeTileNumRow;
        AeStat.AeTileColStart = ImgParam.AaaStatisticsInfo.AeTileColStart;
        AeStat.AeTileRowStart = ImgParam.AaaStatisticsInfo.AeTileRowStart;
        AeStat.AeTileWidth    = ImgParam.AaaStatisticsInfo.AeTileWidth;
        AeStat.AeTileHeight   = ImgParam.AaaStatisticsInfo.AeTileHeight;
        AeStat.AePixMinValue  = ImgParam.AaaStatisticsInfo.AePixMinValue;
        AeStat.AePixMaxValue  = ImgParam.AaaStatisticsInfo.AePixMaxValue;

        U32RVal |= AmbaIK_SetAeStatInfo(mode, &AeStat);
        if(U32RVal != 0U){
            /* MisaC */
        }
        AmbaPrint_ModulePrintStr5(IMGPROC_PRINT_MODULE_ID, "%s, update Ae tile settings", __func__, NULL, NULL, NULL, NULL);
        AmbaPrint_ModulePrintUInt5(IMGPROC_PRINT_MODULE_ID, "row = %d, col = %d", AeStat.AeTileNumRow, AeStat.AeTileNumCol, 0U, 0U, 0U);

#endif
    }
    return RVal;
}

INT32 App_Image_Init_Awb_Tile_Config(UINT32 ViewId, const AMBA_IK_MODE_CFG_s *mode)
{
    INT32              RVal = 0;
    UINT32             U32RVal = 0U;//20210809
    IMG_PARAM_s        ImgParam;
#if !defined(CONFIG_SOC_CV2FS) && !defined(CONFIG_SOC_CV22FS)&& !defined(CONFIG_SOC_CV5)&& !defined(CONFIG_SOC_CV52)
    INT32              MemcmpRet;
    AMBA_IK_AAA_STAT_INFO_s     AaaStat, AaaStatTmp;
#else
    AMBA_IK_AWB_STAT_INFO_s      AwbStat;
#endif
    RVal = (INT32)AmbaImgProc_GetImgParam(ViewId, &ImgParam);

    if (RVal != 0) {
        AmbaPrint_ModulePrintStr5(IMGPROC_PRINT_MODULE_ID, "---- %s, Get ImgParam Error!! ----",__func__, NULL, NULL, NULL, NULL);
        RVal = -1;
    } else {
#if !defined(CONFIG_SOC_CV2FS) && !defined(CONFIG_SOC_CV22FS)&& !defined(CONFIG_SOC_CV5)&& !defined(CONFIG_SOC_CV52)
        U32RVal |= AmbaIK_GetAaaStatInfo(mode, &AaaStatTmp);
        AaaStat = AaaStatTmp;

        AaaStat.AwbTileNumCol   = ImgParam.AaaStatisticsInfo.AwbTileNumCol;
        AaaStat.AwbTileNumRow   = ImgParam.AaaStatisticsInfo.AwbTileNumRow;
        AaaStat.AwbTileColStart = ImgParam.AaaStatisticsInfo.AwbTileColStart;
        AaaStat.AwbTileRowStart = ImgParam.AaaStatisticsInfo.AwbTileRowStart;
        AaaStat.AwbTileWidth    = ImgParam.AaaStatisticsInfo.AwbTileWidth;
        AaaStat.AwbTileHeight   = ImgParam.AaaStatisticsInfo.AwbTileHeight;
        AaaStat.AwbTileActiveWidth    = ImgParam.AaaStatisticsInfo.AwbTileActiveWidth;
        AaaStat.AwbTileActiveHeight   = ImgParam.AaaStatisticsInfo.AwbTileActiveHeight;
        AaaStat.AwbPixMinValue  = ImgParam.AaaStatisticsInfo.AwbPixMinValue;
        AaaStat.AwbPixMaxValue  = ImgParam.AaaStatisticsInfo.AwbPixMaxValue;
        U32RVal |= AmbaWrap_memcmp(&AaaStat, &AaaStatTmp, sizeof(AaaStat), &MemcmpRet);
        U32RVal |= AmbaIK_SetAaaStatInfo(mode, &AaaStat);
        if(U32RVal != 0U){
            /* MisaC */
        }
        AmbaPrint_ModulePrintStr5(IMGPROC_PRINT_MODULE_ID, "%s, update Awb tile settings", __func__, NULL, NULL, NULL, NULL);
        AmbaPrint_ModulePrintUInt5(IMGPROC_PRINT_MODULE_ID, "row = %d, col = %d", AaaStat.AwbTileNumRow, AaaStat.AwbTileNumCol, 0U, 0U, 0U);

#else
        U32RVal |= AmbaIK_GetAwbStatInfo(mode, &AwbStat);
        AwbStat.AwbTileNumCol   = ImgParam.AaaStatisticsInfo.AwbTileNumCol;
        AwbStat.AwbTileNumRow   = ImgParam.AaaStatisticsInfo.AwbTileNumRow;
        AwbStat.AwbTileColStart = ImgParam.AaaStatisticsInfo.AwbTileColStart;
        AwbStat.AwbTileRowStart = ImgParam.AaaStatisticsInfo.AwbTileRowStart;
        AwbStat.AwbTileWidth    = ImgParam.AaaStatisticsInfo.AwbTileWidth;
        AwbStat.AwbTileHeight   = ImgParam.AaaStatisticsInfo.AwbTileHeight;
        AwbStat.AwbTileActiveWidth    = ImgParam.AaaStatisticsInfo.AwbTileActiveWidth;
        AwbStat.AwbTileActiveHeight   = ImgParam.AaaStatisticsInfo.AwbTileActiveHeight;
        AwbStat.AwbPixMinValue  = ImgParam.AaaStatisticsInfo.AwbPixMinValue;
        AwbStat.AwbPixMaxValue  = ImgParam.AaaStatisticsInfo.AwbPixMaxValue;

        U32RVal |= AmbaIK_SetAwbStatInfo(mode, &AwbStat);
        if(U32RVal != 0U){
            /* MisaC */
        }
        AmbaPrint_ModulePrintStr5(IMGPROC_PRINT_MODULE_ID, "%s, update Awb tile settings", __func__, NULL, NULL, NULL, NULL);
        AmbaPrint_ModulePrintUInt5(IMGPROC_PRINT_MODULE_ID, "row = %d, col = %d", AwbStat.AwbTileNumRow, AwbStat.AwbTileNumCol, 0U, 0U, 0U);
#endif



    }
     return RVal;
}

INT32 App_Image_Init_Af_Tile_Config(UINT32 ViewId, const AMBA_IK_MODE_CFG_s *mode)
{
    INT32              RVal = 0;
    UINT32             U32RVal = 0U;//20210809
    IMG_PARAM_s        ImgParam;
#if !defined(CONFIG_SOC_CV2FS) && !defined(CONFIG_SOC_CV22FS)&& !defined(CONFIG_SOC_CV5)&& !defined(CONFIG_SOC_CV52)
    INT32              MemcmpRet;
    AMBA_IK_AAA_STAT_INFO_s AaaStat, AaaStatTmp;
#else
    AMBA_IK_AF_STAT_INFO_s  AfStat;
#endif

    RVal = (INT32)AmbaImgProc_GetImgParam(ViewId, &ImgParam);

    if (RVal != 0) {
        AmbaPrint_ModulePrintStr5(IMGPROC_PRINT_MODULE_ID, "---- %s, Get ImgParam Error!! ----",__func__, NULL, NULL, NULL, NULL);
    RVal = -1;
    } else {
#if !defined(CONFIG_SOC_CV2FS) && !defined(CONFIG_SOC_CV22FS)&& !defined(CONFIG_SOC_CV5)&& !defined(CONFIG_SOC_CV52)
        U32RVal |= AmbaIK_GetAaaStatInfo(mode, &AaaStatTmp);
        AaaStat = AaaStatTmp;

        AaaStat.AfTileNumCol       = ImgParam.AaaStatisticsInfo.AfTileNumCol;
        AaaStat.AfTileNumRow       = ImgParam.AaaStatisticsInfo.AfTileNumRow;
        AaaStat.AfTileColStart     = ImgParam.AaaStatisticsInfo.AfTileColStart;
        AaaStat.AfTileRowStart     = ImgParam.AaaStatisticsInfo.AfTileRowStart;
        AaaStat.AfTileWidth        = ImgParam.AaaStatisticsInfo.AfTileWidth;
        AaaStat.AfTileHeight       = ImgParam.AaaStatisticsInfo.AfTileHeight;
        AaaStat.AfTileActiveWidth  = ImgParam.AaaStatisticsInfo.AfTileActiveWidth;
        AaaStat.AfTileActiveHeight = ImgParam.AaaStatisticsInfo.AfTileActiveHeight;
        U32RVal |= AmbaWrap_memcmp(&AaaStat, &AaaStatTmp, sizeof(AaaStat), &MemcmpRet);
        if(U32RVal != 0U){
            /* MisaC */
        }
        if (MemcmpRet !=0) {
            U32RVal |= AmbaIK_SetAaaStatInfo(mode, &AaaStat);
            if(U32RVal != 0U){
                /* MisaC */
            }
            AmbaPrint_ModulePrintStr5(IMGPROC_PRINT_MODULE_ID, "%s, update Af tile settings", __func__, NULL, NULL, NULL, NULL);
            AmbaPrint_ModulePrintUInt5(IMGPROC_PRINT_MODULE_ID, "row = %d, col = %d", AaaStat.AfTileNumRow, AaaStat.AfTileNumCol, 0U, 0U, 0U);
        }
#else
        U32RVal |= AmbaIK_GetAfStatInfo(mode, &AfStat);
        AfStat.AfTileNumCol       = ImgParam.AaaStatisticsInfo.AfTileNumCol;
        AfStat.AfTileNumRow       = ImgParam.AaaStatisticsInfo.AfTileNumRow;
        AfStat.AfTileColStart     = ImgParam.AaaStatisticsInfo.AfTileColStart;
        AfStat.AfTileRowStart     = ImgParam.AaaStatisticsInfo.AfTileRowStart;
        AfStat.AfTileWidth        = ImgParam.AaaStatisticsInfo.AfTileWidth;
        AfStat.AfTileHeight       = ImgParam.AaaStatisticsInfo.AfTileHeight;
        AfStat.AfTileActiveWidth  = ImgParam.AaaStatisticsInfo.AfTileActiveWidth;
        AfStat.AfTileActiveHeight = ImgParam.AaaStatisticsInfo.AfTileActiveHeight;

        U32RVal |= AmbaIK_SetAfStatInfo(mode, &AfStat);    //cfa
        U32RVal |= AmbaIK_SetPgAfStatInfo(mode, &AfStat);    //pg
        if(U32RVal != 0U){
            /* MisaC */
        }
        AmbaPrint_ModulePrintStr5(IMGPROC_PRINT_MODULE_ID, "%s, update Af tile settings", __func__, NULL, NULL, NULL, NULL);
        AmbaPrint_ModulePrintUInt5(IMGPROC_PRINT_MODULE_ID, "row = %d, col = %d", AfStat.AfTileNumRow, AfStat.AfTileNumCol, 0U, 0U, 0U);
#endif
    }
     return RVal;
}


static UINT32 Amba_Adj_VideoHdr_Init(UINT32 ViewId)
{
    UINT32          rVal = 0;
    HDR_INFO_s      HdrInfo;

    AmbaPrint_ModulePrintUInt5(IMGPROC_PRINT_MODULE_ID, "<Amba_Adj_VideoHdr_Init>    ViewId: %d", ViewId, 0U, 0U, 0U, 0U);

    rVal |= AmbaWrap_memset(&HdrInfo, 0, sizeof(HdrInfo));

    //Get AE info
    rVal |= AmbaImgProc_AEGetExpInfo(ViewId, 0, IP_MODE_VIDEO, &HdrInfo.AeInfo[0]);

    //Get AWB info
    rVal |= AmbaImgProc_AWBGetWBGain(ViewId, 0, IP_MODE_VIDEO, &HdrInfo.WbGain[0]);

    //Init HDR
    rVal |= AmbaImgProc_HDRVideoInit(ViewId, &HdrInfo);
    if(rVal != 0U){
        /* MisraC */
    }
    return rVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: Amba_AdjInit
 *
 *  @Description:: To initialize the ADJ algorithm
 *
 *  @Input      ::
 *    UINT32 chNo : channel number
 *
 *  @Output     :: none
 *
 *  @Return     :: none
 *
\*-----------------------------------------------------------------------------------------------*/
UINT32 Amba_AdjInit(UINT32 ViewId)
{
    UINT32                    rVal = 0;
    AMBA_AE_INFO_s            AeInfotmp ={0};
    AMBA_IK_WB_GAIN_s         WbGain ={0};
    ADJ_VIDEO_IQ_INFO_s       AdjVideoIqInfo;
    IQ_PARAM_s                *pIQParams = NULL;
    ADJ_VIDEO_PARAM_s         *pAdjVideoParam;
    UINT8                     TableNo = 0;
    UINT32                    ChkPhotoPreview = 0;
    APP_INFO_FUNC_s           AppInfoFunc = {NULL, NULL};
    LIVEVIEW_INFO_s           LiveviewInfo;
    //AMBA_IK_MODE_CFG_s        Mode;
    rVal |= AmbaWrap_memset(&AdjVideoIqInfo,0,sizeof(AdjVideoIqInfo));

    rVal |= AmbaImgProc_ADJVideoInit(ViewId);
    /*Set AE, AWB info*/
    rVal |= AmbaImgProc_AEGetExpInfo(ViewId, 0U, IP_MODE_VIDEO, &AeInfotmp);
    rVal |= AmbaImgProc_AWBGetWBGain(ViewId, 0U, IP_MODE_VIDEO, &WbGain);
    AdjVideoIqInfo.AE.ShutterIndex = AeInfotmp.ShutterIndex;
    AdjVideoIqInfo.AE.EvIndex = AeInfotmp.EvIndex;
    AdjVideoIqInfo.AE.NfIndex = AeInfotmp.NfIndex;
    AdjVideoIqInfo.AE.Dgain = AeInfotmp.Dgain;
    AdjVideoIqInfo.AE.IsoValue = AeInfotmp.IsoValue;
    AdjVideoIqInfo.AE.Flash = 0;
    AdjVideoIqInfo.WB = WbGain;

    /*Get App Info callback*/
    rVal |= AmbaImgProc_GetAppInfoFunc(ViewId, &AppInfoFunc);

    /*Get LiveviewInfo and decide adj table number*/
    AppInfoFunc.GetLiveViewInfo((UINT16)ViewId, &LiveviewInfo);
    Amba_ADJTableSelect_Decide_ADJTable(ViewId, ChkPhotoPreview, &LiveviewInfo, &TableNo);

    /*Get IQ params*/
    rVal |= Amba_IQParam_Get_IqParams(ViewId, &pIQParams);


    rVal |= AmbaImgProc_GetPhotoPreview(ViewId, &ChkPhotoPreview);
    if (ChkPhotoPreview == 0U) {
        if(AmbaAdjMaunalEnable[ViewId]==0U) {
            SDecide_AdjTableNo(ViewId, &TableNo);
        }
        else {
            TableNo = AmbaAdjMaunalTableNo[ViewId];
        }

        pAdjVideoParam = pIQParams->videoParams;

        AdjVideoIqInfo.Mode = IP_MODE_LISO_VIDEO;
    }
    else {
        SDecide_AdjTableNo(ViewId, &TableNo);
        pAdjVideoParam = pIQParams->photoPreviewParams;
        AdjVideoIqInfo.Mode = IP_CAPTURE_MODE;
    }

    /*Set AdjVideoIqInfo*/
    AdjVideoIqInfo.pAwbAe = &pAdjVideoParam[TableNo].AwbAe;
    AdjVideoIqInfo.pFilterParam = &pAdjVideoParam[TableNo].FilterParam;
    AdjVideoIqInfo.pHdrShortParam = &pAdjVideoParam[TableNo].HdrShortParam;

    rVal |= AmbaImgProc_ADJVideoCtrl(ViewId, &AdjVideoIqInfo);

    /*HDR Init*/
    rVal |= Amba_Adj_VideoHdr_Init(ViewId);
    if(rVal != 0U){
        /* MisraC */
    }
    return rVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: Amba_AdjControl
 *
 *  @Description:: ADJ algorithm control API
 *
 *  @Input      ::
 *    UINT32 chNo : channel number
 *
 *  @Output     :: none
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
void Amba_AdjControl(UINT32 ViewId)
{
    //UINT16                    EvIndex, NfIndex;
    AMBA_AE_INFO_s            AeInfotmp = {0};
    AMBA_IK_WB_GAIN_s         WbGain = {0};
    UINT32                    ChkPhotoPreview = 0;
    ADJ_VIDEO_IQ_INFO_s       AdjVideoIqInfo;
    IQ_PARAM_s                *pIQParams = NULL;
    ADJ_VIDEO_PARAM_s         *pAdjVideoParam;
    UINT8                     TableNo = 0;
    APP_INFO_FUNC_s           AppInfoFunc = {NULL, NULL};
    LIVEVIEW_INFO_s           LiveviewInfo;
    //DOUBLE              DoubleTmp;
    UINT32                    U32RVal = 0U;


    U32RVal |= AmbaWrap_memset(&AdjVideoIqInfo,0,sizeof(AdjVideoIqInfo));
    /*Set AE AWB Info*/
    U32RVal |= AmbaImgProc_AEGetExpInfo(ViewId, 0U, IP_MODE_VIDEO, &AeInfotmp);
    //(void)AmbaImgProc_AEGetExpToEvIndex(ViewId, IP_MODE_VIDEO, &AeInfotmp, &EvIndex);
    //(void)AmbaImgProc_AEGetExpToNfIndex(ViewId, IP_MODE_VIDEO, &AeInfotmp, &NfIndex);
    U32RVal |= AmbaImgProc_AWBGetWBGain(ViewId, 0U, IP_MODE_VIDEO, &WbGain);
    AdjVideoIqInfo.AE.ShutterIndex = AeInfotmp.ShutterIndex;
    AdjVideoIqInfo.AE.EvIndex = AeInfotmp.EvIndex;
    AdjVideoIqInfo.AE.NfIndex = AeInfotmp.NfIndex;
    AdjVideoIqInfo.AE.Dgain = AeInfotmp.Dgain;
    AdjVideoIqInfo.AE.IsoValue = AeInfotmp.IsoValue;
    AdjVideoIqInfo.AE.FogStrength = 0;
    AdjVideoIqInfo.AE.Flash = 0;
    AdjVideoIqInfo.WB = WbGain;

    /*Get App Info callback*/
    U32RVal |= AmbaImgProc_GetAppInfoFunc(ViewId, &AppInfoFunc);

    /*Get LiveviewInfo and decide adj table number*/
    AppInfoFunc.GetLiveViewInfo((UINT16)ViewId, &LiveviewInfo);
    Amba_ADJTableSelect_Decide_ADJTable(ViewId, ChkPhotoPreview, &LiveviewInfo, &TableNo);

    /*Get IQ params*/
    U32RVal |= Amba_IQParam_Get_IqParams(ViewId, &pIQParams);
    U32RVal |= AmbaImgProc_GetPhotoPreview(ViewId, &ChkPhotoPreview);

    if (ChkPhotoPreview == 0U) {
        if(AmbaAdjMaunalEnable[ViewId]==0U) {
            SDecide_AdjTableNo(ViewId, &TableNo); //Get Adj Table No.
        }
        else {
            TableNo = AmbaAdjMaunalTableNo[ViewId];
        }

        pAdjVideoParam = pIQParams->videoParams;

        AdjVideoIqInfo.Mode = IP_PREVIEW_MODE;

    } else {
        SDecide_AdjTableNo(ViewId, &TableNo);

        pAdjVideoParam = pIQParams->photoPreviewParams;

        AdjVideoIqInfo.Mode = IP_CAPTURE_MODE;

    }

    /*Set AdjVideoIqInfo*/
    AdjVideoIqInfo.pAwbAe = &pAdjVideoParam[TableNo].AwbAe;
    AdjVideoIqInfo.pFilterParam = &pAdjVideoParam[TableNo].FilterParam;
    AdjVideoIqInfo.pHdrShortParam = &pAdjVideoParam[TableNo].HdrShortParam;

    U32RVal |= AmbaImgProc_ADJAEAWBCtrl(ViewId, &AdjVideoIqInfo);

    AdjVideoIqInfo.Mode = IP_MODE_LISO_VIDEO;

    //DoubleTmp = (DOUBLE)AeInfotmp.HdrRatio * (DOUBLE)100.0;
    // AmbaPrint_PrintUInt5("---- EvIndex : %d, NfIndex : %d, FogStrength : %d", EvIndex, NfIndex, AeInfotmp.FogStrength, 0, 0);
    //(void)AmbaImgProc_ADJCeCtrl(ViewId, AeInfotmp.HdrRatio, AeInfotmp.FogStrength);
   U32RVal |= AmbaImgProc_ADJVideoCtrl(ViewId, &AdjVideoIqInfo);
   U32RVal |= AmbaImgProc_AEGetExpInfo(ViewId, 0U, IP_MODE_VIDEO, &AeInfotmp);
   U32RVal |= AmbaImgProc_ADJCeCtrl(ViewId, AeInfotmp.HdrRatio, AeInfotmp.FogStrength);
    if(U32RVal != 0U){
        /* MisraC */
    }
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: Amba_AdjStillControl
 *
 *  @Description:: STILL ADJ algorithm control API
 *
 *  @Input      ::
 *    UINT32 chNo : channel number
 *    UINT32 aeIndx : Index to certain still AE information
 *    AMBA_IK_MODE_CFG_s *mode : pointer to DSP control mode
 *
 *  @Output     :: none
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
void Amba_AdjStillControl(UINT32 ViewId, UINT32 aeIndx, /*const AMBA_IK_MODE_CFG_s *mode,*/ UINT16 algoMode)
{

    static FLOAT                    BaseStillBlcShtTime = 60.0f;
    UINT32                          HdrEnable = 0;
    UINT32                          StillAdjTableNo = 0;
    UINT16                          ShutterIndex = 0;
    DOUBLE                          ShutterIndexTmp = 0.0f;
    AMBA_AAA_OP_INFO_s              AaaOpInfo ={0};
    AMBA_AE_INFO_s                  StillAeInfo[MAX_AEB_NUM] = {0};
    AMBA_IK_WB_GAIN_s               StillWbGain[MAX_AEB_NUM];
    ADJ_STILL_IQ_INFO_s             AdjStillIqInfo;
    IQ_PARAM_s                      *pIQParams = NULL;
    UINT32                          U32RVal = 0U;

    UINT32 LinearCeEnable = 0;
    HDR_INFO_s    HdrInfo[MAX_AEB_NUM] = {0};
    UINT32        i = 0U, j = 0U;


    for (i = 0U; i < MAX_AEB_NUM; i++) {
        StillWbGain[i].GainR = (UINT32)4096;
        StillWbGain[i].GainG = (UINT32)4096;
        StillWbGain[i].GainB = (UINT32)4096;
    }

    U32RVal |= AmbaWrap_memset(&AdjStillIqInfo,0,sizeof(AdjStillIqInfo));
    U32RVal |= AmbaImgProc_GetAAAOPInfo(ViewId, &AaaOpInfo);

    AdjStillIqInfo.Mode            = (UINT8)algoMode; //IP_MODE_LISO_STILL/IP_MODE_HISO_STILL

    if (AaaOpInfo.AdjOp == 1U) {
        U32RVal |= AmbaImgProc_AEGetExpInfo(ViewId, 0U, IP_MODE_STILL, StillAeInfo);
        U32RVal |= AmbaImgProc_AWBGetWBGain(ViewId, 0U, IP_MODE_STILL, StillWbGain);
        U32RVal |= AmbaWrap_log2(((DOUBLE)BaseStillBlcShtTime/(DOUBLE)StillAeInfo[aeIndx].ShutterTime), &ShutterIndexTmp);
        ShutterIndexTmp *= 128.0;
        ShutterIndex = (UINT16)ShutterIndexTmp;
        if(AaaOpInfo.AeOp == 0U){
            U32RVal |= AmbaImgProc_AEGetExpToEvIndex(ViewId, IP_MODE_STILL, &StillAeInfo[aeIndx], &StillAeInfo[aeIndx].EvIndex);
            U32RVal |= AmbaImgProc_AEGetExpToNfIndex(ViewId, IP_MODE_STILL, &StillAeInfo[aeIndx], &StillAeInfo[aeIndx].NfIndex);
        }


        // AdjStillIqInfo.Mode            = (UINT8)algoMode; //IP_MODE_LISO_STILL/IP_MODE_HISO_STILL
        AdjStillIqInfo.AE.ShutterIndex = (INT16)ShutterIndex;
        AdjStillIqInfo.AE.EvIndex      = StillAeInfo[aeIndx].EvIndex;
        AdjStillIqInfo.AE.NfIndex      = StillAeInfo[aeIndx].NfIndex;
        AdjStillIqInfo.AE.Dgain        = StillAeInfo[aeIndx].Dgain;
        AdjStillIqInfo.AE.IsoValue     = StillAeInfo[aeIndx].IsoValue;

        AdjStillIqInfo.WB              = StillWbGain[aeIndx];
        AdjStillIqInfo.AEBIndex        = aeIndx;

        // _Decide_StillAdjTableNo(&StillAdjTableNo);//Get Adj Table No.

        AmbaPrint_ModulePrintUInt5(IMGPROC_PRINT_MODULE_ID, " StillMode : %d, ShIndex : %5d, EvIndex : %5d, NfIndex : %5d ",
            (UINT32)AdjStillIqInfo.Mode, (UINT32)AdjStillIqInfo.AE.ShutterIndex, (UINT32)AdjStillIqInfo.AE.EvIndex, (UINT32)AdjStillIqInfo.AE.NfIndex, 0U);


        U32RVal |= Amba_IQParam_Get_IqParams(ViewId, &pIQParams);
        AdjStillIqInfo.pStillLISOParam = &pIQParams->stillLISOParams[StillAdjTableNo];
    }

    if ((AaaOpInfo.AdjOp == 0U) && (AdjStillIqInfo.AE.ShutterIndex==0)) {
        // AdjStillIqInfo.Mode            = (UINT8)algoMode; //IP_MODE_LISO_STILL/IP_MODE_HISO_STILL
        AdjStillIqInfo.AE.ShutterIndex = 1012;
        AdjStillIqInfo.AE.EvIndex      = 0;
        AdjStillIqInfo.AE.NfIndex      = 0;

        AdjStillIqInfo.WB              = StillWbGain[aeIndx];
        U32RVal |= Amba_IQParam_Get_IqParams(ViewId, &pIQParams);
        AdjStillIqInfo.pStillLISOParam = &pIQParams->stillLISOParams[StillAdjTableNo];

    }


    if(algoMode==(UINT16)IP_MODE_HISO_STILL){
        AdjStillIqInfo.pStillHISOParam = &pIQParams->stillHISOParams[StillAdjTableNo];
    }else{
        AdjStillIqInfo.pStillLISOParam = &pIQParams->stillLISOParams[StillAdjTableNo];
    }

    U32RVal |= AmbaImgProc_HDRGetVideoMode(ViewId, &HdrEnable);
    U32RVal |= AmbaImgProc_ADJStillCtrl(ViewId, &AdjStillIqInfo);
    /* move to outside for external still adj */
    //U32RVal |= AmbaImgProc_ADJSetStillParam(ViewId, aeIndx, mode);

    U32RVal |= AmbaImgProc_HDRGetLinearCEEnable(ViewId, &LinearCeEnable);
    if(LinearCeEnable == 1U){
        U32RVal |= AmbaWrap_memset(HdrInfo, 0, sizeof(HDR_INFO_s) * MAX_AEB_NUM);
        for (i=0; i<MAX_AEB_NUM; i++) {
            for (j=0U; j<(UINT32)MAX_EXP_GROUP_NUM; j++) {
                HdrInfo[i].AeInfo[j] = StillAeInfo[i];
                HdrInfo[i].WbGain[j] = StillWbGain[i];
            }
        }
        U32RVal |= AmbaImgProc_HDRStillInit(ViewId, HdrInfo);
    }

    if(U32RVal != 0U){
        /* MisraC */
    }
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: Amba_AdjPivControl
 *
 *  @Description:: PIV ADJ algorithm control API
 *
 *  @Input      ::
 *    UINT32 chNo : channel number
 *    AMBA_IK_MODE_CFG_s *mode : pointer to DSP control mode
 *
 *  @Output     :: none
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
void Amba_AdjPivControl(UINT32 ViewId, /*const AMBA_IK_MODE_CFG_s *mode,*/ UINT16 algoMode)
{

    static FLOAT                    BaseStillBlcShtTime = 60.0f;
    UINT32                          HdrEnable = 0;
    UINT32                          StillAdjTableNo = 0;
    UINT16                          ShutterIndex = 0;
    DOUBLE                          ShutterIndexTmp = 0.0f;
    AMBA_AAA_OP_INFO_s              AaaOpInfo = {0};
    AMBA_AE_INFO_s                  VideoAeInfo = {0};
    AMBA_IK_WB_GAIN_s               VideoWbGain;
    ADJ_STILL_IQ_INFO_s             AdjPivIqInfo;
    IQ_PARAM_s                      *pIQParams = NULL;
    UINT32                          U32RVal = 0U;//20210809

    // Get_ViewID(ViewId, &ViewId);
    VideoWbGain.GainR = (UINT32)4096;
    VideoWbGain.GainG = (UINT32)4096;
    VideoWbGain.GainB = (UINT32)4096;

    U32RVal |= AmbaWrap_memset(&AdjPivIqInfo,0,sizeof(AdjPivIqInfo));
    U32RVal |= AmbaImgProc_GetAAAOPInfo(ViewId, &AaaOpInfo);

    AdjPivIqInfo.Mode            = (UINT8)algoMode; //IP_MODE_LISO_STILL/IP_MODE_HISO_STILL

    if (AaaOpInfo.AdjOp == 1U) {
        U32RVal |= AmbaImgProc_AEGetExpInfo(ViewId, 0U, IP_MODE_VIDEO, &VideoAeInfo);
        U32RVal |= AmbaImgProc_AWBGetWBGain(ViewId, 0U, IP_MODE_VIDEO, &VideoWbGain);

        U32RVal |= AmbaWrap_log2(((DOUBLE)BaseStillBlcShtTime/(DOUBLE)VideoAeInfo.ShutterTime), &ShutterIndexTmp);
        ShutterIndexTmp *= 128.0;
        ShutterIndex = (UINT16)ShutterIndexTmp;
        if(AaaOpInfo.AeOp == 0U){
            U32RVal |= AmbaImgProc_AEGetExpToEvIndex(ViewId, IP_MODE_VIDEO, &VideoAeInfo, &VideoAeInfo.EvIndex);
            U32RVal |= AmbaImgProc_AEGetExpToNfIndex(ViewId, IP_MODE_VIDEO, &VideoAeInfo, &VideoAeInfo.NfIndex);
        }


        // AdjPivIqInfo.Mode            = (UINT8)algoMode; //IP_MODE_LISO_STILL/IP_MODE_HISO_STILL
        AdjPivIqInfo.AE.ShutterIndex = (INT16)ShutterIndex;
        AdjPivIqInfo.AE.EvIndex      = VideoAeInfo.EvIndex;
        AdjPivIqInfo.AE.NfIndex      = VideoAeInfo.NfIndex;
        AdjPivIqInfo.AE.Dgain        = VideoAeInfo.Dgain;
        AdjPivIqInfo.AE.IsoValue     = VideoAeInfo.IsoValue;

        AdjPivIqInfo.WB              = VideoWbGain;
        AdjPivIqInfo.AEBIndex        = 0U;

        AmbaPrint_ModulePrintUInt5(IMGPROC_PRINT_MODULE_ID, " StillMode : %d, ShIndex : %5d, EvIndex : %5d, NfIndex : %5d ",
            (UINT32)AdjPivIqInfo.Mode, (UINT32)AdjPivIqInfo.AE.ShutterIndex, (UINT32)AdjPivIqInfo.AE.EvIndex, (UINT32)AdjPivIqInfo.AE.NfIndex, 0U);


        U32RVal |= Amba_IQParam_Get_IqParams(ViewId, &pIQParams);
        AdjPivIqInfo.pStillLISOParam = &pIQParams->stillLISOParams[StillAdjTableNo];
    }

    if ((AaaOpInfo.AdjOp == 0U) && (AdjPivIqInfo.AE.ShutterIndex==0)) {
        // AdjPivIqInfo.Mode            = (UINT8)algoMode; //IP_MODE_LISO_STILL/IP_MODE_HISO_STILL
        AdjPivIqInfo.AE.ShutterIndex = 1012;
        AdjPivIqInfo.AE.EvIndex      = 0;
        AdjPivIqInfo.AE.NfIndex      = 0;

        AdjPivIqInfo.WB              = VideoWbGain;
        U32RVal |= Amba_IQParam_Get_IqParams(ViewId, &pIQParams);
        AdjPivIqInfo.pStillLISOParam = &pIQParams->stillLISOParams[StillAdjTableNo];

    }

    if(algoMode==(UINT16)IP_MODE_HISO_STILL){
        AdjPivIqInfo.pStillHISOParam = &pIQParams->stillHISOParams[StillAdjTableNo];
    }else{
        AdjPivIqInfo.pStillLISOParam = &pIQParams->stillLISOParams[StillAdjTableNo];
    }

    U32RVal |= AmbaImgProc_HDRGetVideoMode(ViewId, &HdrEnable);
    U32RVal |= AmbaImgProc_ADJStillCtrl(ViewId, &AdjPivIqInfo);
    /* move to outside for external still adj */
    //U32RVal |= AmbaImgProc_ADJSetStillParam(ViewId, 0U, mode);
    if(U32RVal != 0U){
        /* MisraC */
    }
}

void Amba_Set_MADJ(UINT32 ViewId, UINT8 enable, UINT32 madj_no)
{
    AmbaAdjMaunalEnable[ViewId] = enable;
    AmbaAdjMaunalTableNo[ViewId] = (UINT8)madj_no;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: Amba_AdjSetDefog
 *
 *  @Description:: STILL ADJ algorithm Defog API
 *
 *  @Input      ::
 *    UINT32 chNo : channel number
 *    UINT32 enable : defog enable/disable
 *    INT32 sceneMode: scene mode
 *
 *  @Output     :: none
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
void Amba_AdjSetDefog(UINT32 ViewId, UINT32 enable, INT32 sceneMode)
{
    UINT32 U32RVal = 0U;
    if (enable > 0UL) {
        Amba_Set_MADJ(ViewId, 1U, 2U);
        U32RVal |= AmbaImgProc_SCSetSceneMode(ViewId, IP_MODE_VIDEO, &sceneMode);
        U32RVal |= AmbaImgProc_SCSetSceneMode(ViewId, IP_MODE_STILL, &sceneMode);
    } else {
        Amba_Set_MADJ(ViewId, 0U, 0U);
        U32RVal |= AmbaImgProc_SCSetSceneMode(ViewId, IP_MODE_VIDEO, &sceneMode);
        U32RVal |= AmbaImgProc_SCSetSceneMode(ViewId, IP_MODE_STILL, &sceneMode);
    }
    if(U32RVal != 0U){
        /* MisraC */
    }
}


/*-----------------------------------------------------------------------------------------------*\
\*-----------------------------------------------------------------------------------------------*/
//Start of Complexity
static UINT32 AdjSetVPipeCParams_Sub4(const PIPELINE_CONTROL_s *VideoPipeCtrl, const AMBA_IK_MODE_CFG_s *mode)
{
    UINT32                 RVal = 0U;
#if defined(CONFIG_SOC_CV2FS) || defined(CONFIG_SOC_CV22FS)  || defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)
        if (VideoPipeCtrl->ChromaFilterUpdate == UPDATE_FLAG_ON){
            RVal |= AmbaIK_SetChromaFilter(mode, &VideoPipeCtrl->ChromaFilter);
        }

        if (VideoPipeCtrl->GbGrMismatchUpdate == UPDATE_FLAG_ON){
            RVal |= AmbaIK_SetGrgbMismatch(mode, &VideoPipeCtrl->GbGrMismatch);
        }

        if (VideoPipeCtrl->ResamplerStrUpdate == UPDATE_FLAG_ON){
            RVal |= AmbaIK_SetResampStrength(mode, &VideoPipeCtrl->ResamplerStr);
        }

        if (VideoPipeCtrl->MctfInfoUpdate == UPDATE_FLAG_ON){
            RVal |= AmbaIK_SetVideoMctf(mode, &VideoPipeCtrl->MctfInfo);
            RVal |= AmbaIK_SetVideoMctfTa(mode, &VideoPipeCtrl->MctfTemporalAdjust);
        }

        if (VideoPipeCtrl->CEInfoUpdate == 1U) {
            RVal |= AmbaIK_SetCe(mode, &VideoPipeCtrl->CEInfo);
        }
        if (VideoPipeCtrl->ColorDeptNRUpdate == UPDATE_FLAG_ON){
            RVal |= AmbaIK_SetLumaNoiseReduction(mode, &VideoPipeCtrl->ColorDeptNR);
        }

        if (VideoPipeCtrl->WideChromaFilterUpdate == UPDATE_FLAG_ON){
            RVal |= AmbaIK_SetWideChromaFilter(mode, &VideoPipeCtrl->WideChromaFilter);
        }

        if (VideoPipeCtrl->WideChromaFilterCMBUpdate == UPDATE_FLAG_ON){
            RVal |= AmbaIK_SetWideChromaFilterCombine(mode, &VideoPipeCtrl->WideChromaFilterCMB);
        }
#else
        if (VideoPipeCtrl->ChromaFilterUpdate == UPDATE_FLAG_ON){
            RVal |= AmbaIK_SetChromaFilter(mode, &VideoPipeCtrl->ChromaFilter);
        }

        if (VideoPipeCtrl->GbGrMismatchUpdate == UPDATE_FLAG_ON){
            RVal |= AmbaIK_SetGrgbMismatch(mode, &VideoPipeCtrl->GbGrMismatch);
        }

        if (VideoPipeCtrl->ResamplerStrUpdate == UPDATE_FLAG_ON){
            RVal |= AmbaIK_SetResampStrength(mode, &VideoPipeCtrl->ResamplerStr);
        }

        if (VideoPipeCtrl->MctfInfoUpdate == UPDATE_FLAG_ON){
            RVal |= AmbaIK_SetVideoMctf(mode, &VideoPipeCtrl->MctfInfo);
            RVal |= AmbaIK_SetVideoMctfTa(mode, &VideoPipeCtrl->MctfTemporalAdjust);
        }

        if (VideoPipeCtrl->CEInfoUpdate == 1U) {
            RVal |= AmbaIK_SetCe(mode, &VideoPipeCtrl->CEInfo);
        }
#endif
    return RVal;
}

static UINT32 AdjSetVPipeCParams_Sub3(const PIPELINE_CONTROL_s *VideoPipeCtrl, const AMBA_IK_MODE_CFG_s *mode)
{
    UINT32                 RVal = 0U;
#if defined(CONFIG_SOC_CV2FS) || defined(CONFIG_SOC_CV22FS)  || defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)
        if (VideoPipeCtrl->SharpenBBothUpdate == UPDATE_FLAG_ON){
            RVal |= AmbaIK_SetFnlShpNsBoth(mode, &VideoPipeCtrl->SharpenBBoth);
        }

        if (VideoPipeCtrl->SharpenBNoiseUpdate == UPDATE_FLAG_ON){
            RVal |= AmbaIK_SetFnlShpNsNoise(mode, &VideoPipeCtrl->SharpenBNoise);
        }

        if (VideoPipeCtrl->SharpenBFirUpdate == UPDATE_FLAG_ON){
            RVal |= AmbaIK_SetFnlShpNsFir(mode, &VideoPipeCtrl->SharpenBFir);
        }

        if (VideoPipeCtrl->SharpenBCoringUpdate == UPDATE_FLAG_ON){
            RVal |= AmbaIK_SetFnlShpNsCoring(mode, &VideoPipeCtrl->SharpenBCoring);
        }

        if (VideoPipeCtrl->SharpenBCoringIndexScaleUpdate == UPDATE_FLAG_ON){
            RVal |= AmbaIK_SetFnlShpNsCorIdxScl(mode, &VideoPipeCtrl->SharpenBCoringIndexScale);
        }

        if (VideoPipeCtrl->SharpenBMinCoringResultUpdate == UPDATE_FLAG_ON){
            RVal |= AmbaIK_SetFnlShpNsMinCorRst(mode, &VideoPipeCtrl->SharpenBMinCoringResult);
        }

        if (VideoPipeCtrl->SharpenBMaxCoringResultUpdate == UPDATE_FLAG_ON){
            RVal |= AmbaIK_SetFnlShpNsMaxCorRst(mode, &VideoPipeCtrl->SharpenBMaxCoringResult);
        }

        if (VideoPipeCtrl->SharpenBScaleCoringUpdate == UPDATE_FLAG_ON){
            RVal |= AmbaIK_SetFnlShpNsSclCor(mode, &VideoPipeCtrl->SharpenBScaleCoring);
        }
#else
        if (VideoPipeCtrl->SharpenBBothUpdate == UPDATE_FLAG_ON){
            RVal |= AmbaIK_SetFnlShpNsBoth(mode, &VideoPipeCtrl->SharpenBBoth);
        }

        if (VideoPipeCtrl->SharpenBNoiseUpdate == UPDATE_FLAG_ON){
            RVal |= AmbaIK_SetFnlShpNsNoise(mode, &VideoPipeCtrl->SharpenBNoise);
        }

        if (VideoPipeCtrl->SharpenBFirUpdate == UPDATE_FLAG_ON){
            RVal |= AmbaIK_SetFnlShpNsFir(mode, &VideoPipeCtrl->SharpenBFir);
        }

        if (VideoPipeCtrl->SharpenBCoringUpdate == UPDATE_FLAG_ON){
            RVal |= AmbaIK_SetFnlShpNsCoring(mode, &VideoPipeCtrl->SharpenBCoring);
        }

        if (VideoPipeCtrl->SharpenBCoringIndexScaleUpdate == UPDATE_FLAG_ON){
            RVal |= AmbaIK_SetFnlShpNsCorIdxScl(mode, &VideoPipeCtrl->SharpenBCoringIndexScale);
        }

        if (VideoPipeCtrl->SharpenBMinCoringResultUpdate == UPDATE_FLAG_ON){
            RVal |= AmbaIK_SetFnlShpNsMinCorRst(mode, &VideoPipeCtrl->SharpenBMinCoringResult);
        }

        if (VideoPipeCtrl->SharpenBMaxCoringResultUpdate == UPDATE_FLAG_ON){
            RVal |= AmbaIK_SetFnlShpNsMaxCorRst(mode, &VideoPipeCtrl->SharpenBMaxCoringResult);
        }

        if (VideoPipeCtrl->SharpenBScaleCoringUpdate == UPDATE_FLAG_ON){
            RVal |= AmbaIK_SetFnlShpNsSclCor(mode, &VideoPipeCtrl->SharpenBScaleCoring);
        }
#endif
    return RVal;
}

static UINT32 AdjSetVPipeCParams_Sub2(const PIPELINE_CONTROL_s *VideoPipeCtrl, const AMBA_IK_MODE_CFG_s *mode)
{
    UINT32                 RVal = 0U;
#if defined(CONFIG_SOC_CV2FS) || defined(CONFIG_SOC_CV22FS)  || defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)
        if (VideoPipeCtrl->LisoProcessSelectUpdate == UPDATE_FLAG_ON){
            RVal |= AmbaIK_SetFirstLumaProcMode(mode, &VideoPipeCtrl->LisoProcessSelect);
        }

        if (VideoPipeCtrl->AsfUpdate == UPDATE_FLAG_ON){
            RVal |= AmbaIK_SetAdvSpatFltr(mode, &VideoPipeCtrl->Asf);
        }

        if (VideoPipeCtrl->SharpenBothUpdate == UPDATE_FLAG_ON){
            RVal |= AmbaIK_SetFstShpNsBoth(mode, &VideoPipeCtrl->SharpenBoth);
        }

        if (VideoPipeCtrl->SharpenNoiseUpdate == UPDATE_FLAG_ON){
            RVal |= AmbaIK_SetFstShpNsNoise(mode, &VideoPipeCtrl->SharpenNoise);
        }

        if (VideoPipeCtrl->SharpenFirUpdate == UPDATE_FLAG_ON){
            RVal |= AmbaIK_SetFstShpNsFir(mode, &VideoPipeCtrl->SharpenFir);
        }

        if (VideoPipeCtrl->SharpenCoringUpdate == UPDATE_FLAG_ON){
            RVal |= AmbaIK_SetFstShpNsCoring(mode, &VideoPipeCtrl->SharpenCoring);
        }

        if (VideoPipeCtrl->SharpenCoringIndexScaleUpdate == UPDATE_FLAG_ON){
            RVal |= AmbaIK_SetFstShpNsCorIdxScl(mode, &VideoPipeCtrl->SharpenCoringIndexScale);
        }

        if (VideoPipeCtrl->SharpenMinCoringResultUpdate == UPDATE_FLAG_ON){
            RVal |= AmbaIK_SetFstShpNsMinCorRst(mode, &VideoPipeCtrl->SharpenMinCoringResult);
        }

        if (VideoPipeCtrl->SharpenMaxCoringResultUpdate == UPDATE_FLAG_ON){
            RVal |= AmbaIK_SetFstShpNsMaxCorRst(mode, &VideoPipeCtrl->SharpenMaxCoringResult);
        }

        if (VideoPipeCtrl->SharpenScaleCoringUpdate == UPDATE_FLAG_ON){
            RVal |= AmbaIK_SetFstShpNsSclCor(mode, &VideoPipeCtrl->SharpenScaleCoring);
        }
#else
        if (VideoPipeCtrl->LisoProcessSelectUpdate == UPDATE_FLAG_ON){
            RVal |= AmbaIK_SetFirstLumaProcMode(mode, &VideoPipeCtrl->LisoProcessSelect);
        }

        if (VideoPipeCtrl->AsfUpdate == UPDATE_FLAG_ON){
            RVal |= AmbaIK_SetAdvSpatFltr(mode, &VideoPipeCtrl->Asf);
        }

        if (VideoPipeCtrl->SharpenBothUpdate == UPDATE_FLAG_ON){
            RVal |= AmbaIK_SetFstShpNsBoth(mode, &VideoPipeCtrl->SharpenBoth);
        }

        if (VideoPipeCtrl->SharpenNoiseUpdate == UPDATE_FLAG_ON){
            RVal |= AmbaIK_SetFstShpNsNoise(mode, &VideoPipeCtrl->SharpenNoise);
        }

        if (VideoPipeCtrl->SharpenFirUpdate == UPDATE_FLAG_ON){
            RVal |= AmbaIK_SetFstShpNsFir(mode, &VideoPipeCtrl->SharpenFir);
        }

        if (VideoPipeCtrl->SharpenCoringUpdate == UPDATE_FLAG_ON){
            RVal |= AmbaIK_SetFstShpNsCoring(mode, &VideoPipeCtrl->SharpenCoring);
        }

        if (VideoPipeCtrl->SharpenCoringIndexScaleUpdate == UPDATE_FLAG_ON){
            RVal |= AmbaIK_SetFstShpNsCorIdxScl(mode, &VideoPipeCtrl->SharpenCoringIndexScale);
        }

        if (VideoPipeCtrl->SharpenMinCoringResultUpdate == UPDATE_FLAG_ON){
            RVal |= AmbaIK_SetFstShpNsMinCorRst(mode, &VideoPipeCtrl->SharpenMinCoringResult);
        }

        if (VideoPipeCtrl->SharpenMaxCoringResultUpdate == UPDATE_FLAG_ON){
            RVal |= AmbaIK_SetFstShpNsMaxCorRst(mode, &VideoPipeCtrl->SharpenMaxCoringResult);
        }

        if (VideoPipeCtrl->SharpenScaleCoringUpdate == UPDATE_FLAG_ON){
            RVal |= AmbaIK_SetFstShpNsSclCor(mode, &VideoPipeCtrl->SharpenScaleCoring);
        }
#endif
    return RVal;
}

static UINT32 AdjSetVPipeCParams_Sub1(UINT32 ViewID, const PIPELINE_CONTROL_s *VideoPipeCtrl, UINT32 HdrEnable, const AMBA_IK_MODE_CFG_s *mode)
{
    UINT32                 RVal = 0U;

#if defined(CONFIG_SOC_CV2FS) || defined(CONFIG_SOC_CV22FS)  || defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)
        if (VideoPipeCtrl->RgbYuvMatrixUpdate == UPDATE_FLAG_ON){
                RVal |= AmbaIK_SetRgbToYuvMatrix(mode, &VideoPipeCtrl->RgbYuvMatrix);
        }

        if (VideoPipeCtrl->BlackCorrUpdate == UPDATE_FLAG_ON){
            if ((HdrEnable != (UINT8)IMG_DSP_HDR_MODE_0) && (HdrEnable != (UINT8)IMG_DSP_HDR_MODE_1)) {
                RVal |= AmbaIK_SetFeStaticBlc(mode, &VideoPipeCtrl->BlackCorr, 0U/*ExpIdx*/);
            }
        }

        if (VideoPipeCtrl->AntiAliasingUpdate == UPDATE_FLAG_ON){
            RVal |= AmbaIK_SetAntiAliasing(mode, &VideoPipeCtrl->AntiAliasing);
        }

        if (VideoPipeCtrl->BadpixCorrUpdate == UPDATE_FLAG_ON){
            RVal |= AmbaIK_SetDynamicBadPxlCor(mode, &VideoPipeCtrl->BadpixCorr);
        }

        if (VideoPipeCtrl->CfaFilterUpdate == UPDATE_FLAG_ON){
            RVal |= AmbaIK_SetCfaNoiseFilter(mode, &VideoPipeCtrl->CfaFilter);
        }

        if (VideoPipeCtrl->RgbIrUpdate == UPDATE_FLAG_ON){
            RVal |= AmbaIK_SetRgbIr(mode, &VideoPipeCtrl->RgbIr);
        }

#if defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)
        if (VideoPipeCtrl->StoredIRUpdate == UPDATE_FLAG_ON){
            RVal |= AmbaIK_SetStoreIr(mode, &VideoPipeCtrl->StoredIR);
        }
#endif

        if (VideoPipeCtrl->DemosaicUpdate == UPDATE_FLAG_ON){
            RVal |= AmbaIK_SetDemosaic(mode, &VideoPipeCtrl->Demosaic);
        }

        if (VideoPipeCtrl->ColorCorrUpdate == UPDATE_FLAG_ON){
            static IMG_PARAM_s VImgParam[MAX_VIEW_NUM] GNU_SECTION_NOZEROINIT;
            RVal |= AmbaImgProc_GetImgParam(ViewID, &VImgParam[ViewID]);
            if(RVal == 0U){
                RVal |= AmbaIK_SetCfaLeakageFilter(mode, &VImgParam[ViewID].CfaLeakageFilterVideo);
            }
            RVal |= AmbaIK_SetColorCorrection(mode, &VideoPipeCtrl->ColorCorr);
        }

        if (VideoPipeCtrl->GammaUpdate == UPDATE_FLAG_ON){
            RVal |= AmbaIK_SetToneCurve(mode, &VideoPipeCtrl->GammaTable);
        }

        if (VideoPipeCtrl->ChromaScaleUpdate == UPDATE_FLAG_ON){
            RVal |= AmbaIK_SetChromaScale(mode, &VideoPipeCtrl->ChromaScale);
        }

        if (VideoPipeCtrl->ChromaMedianUpdate == UPDATE_FLAG_ON){
            RVal |= AmbaIK_SetChromaMedianFilter(mode, &VideoPipeCtrl->ChromaMedian);
        }
#else
        if (VideoPipeCtrl->RgbYuvMatrixUpdate == UPDATE_FLAG_ON){
            RVal |= AmbaIK_SetRgbToYuvMatrix(mode, &VideoPipeCtrl->RgbYuvMatrix);
        }

        if (VideoPipeCtrl->BlackCorrUpdate == UPDATE_FLAG_ON){
            if ((HdrEnable != (UINT8)IMG_DSP_HDR_MODE_0) && (HdrEnable != (UINT8)IMG_DSP_HDR_MODE_1)) {
                RVal |= AmbaIK_SetFeStaticBlc(mode, &VideoPipeCtrl->BlackCorr, 0U/*ExpIdx*/);
            }
        }

        if (VideoPipeCtrl->AntiAliasingUpdate == UPDATE_FLAG_ON){
            RVal |= AmbaIK_SetAntiAliasing(mode, &VideoPipeCtrl->AntiAliasing);
        }

        if (VideoPipeCtrl->BadpixCorrUpdate == UPDATE_FLAG_ON){
            RVal |= AmbaIK_SetDynamicBadPxlCor(mode, &VideoPipeCtrl->BadpixCorr);
        }

        if (VideoPipeCtrl->CfaFilterUpdate == UPDATE_FLAG_ON){
            RVal |= AmbaIK_SetCfaNoiseFilter(mode, &VideoPipeCtrl->CfaFilter);
        }

        if (VideoPipeCtrl->RgbIrUpdate == UPDATE_FLAG_ON){
            RVal |= AmbaIK_SetRgbIr(mode, &VideoPipeCtrl->RgbIr);
        }

        if (VideoPipeCtrl->DemosaicUpdate == UPDATE_FLAG_ON){
            RVal |= AmbaIK_SetDemosaic(mode, &VideoPipeCtrl->Demosaic);
        }

        if (VideoPipeCtrl->ColorCorrUpdate == UPDATE_FLAG_ON){
            static AMBA_IK_COLOR_CORRECTION_REG_s ColorCorrRegCur[MAX_VIEW_NUM] GNU_SECTION_NOZEROINIT;
            ULONG                          CcRegAdd;
            const ULONG                    *pCcReg = NULL;
            static IMG_PARAM_s VImgParam[MAX_VIEW_NUM] GNU_SECTION_NOZEROINIT;
            RVal |= AmbaImgProc_GetImgParam(ViewID, &VImgParam[ViewID]);
            if(RVal == 0U){
                RVal |= AmbaIK_SetCfaLeakageFilter(mode, &VImgParam[ViewID].CfaLeakageFilterVideo);
            }
            RVal |= AmbaImgProc_GetCcRegAddr(ViewID, &CcRegAdd);
            RVal |= AmbaWrap_memcpy(&pCcReg, &CcRegAdd, sizeof(ULONG));

            RVal |= AmbaWrap_memcpy(ColorCorrRegCur[ViewID].RegSettingTable,pCcReg,sizeof(AMBA_IK_COLOR_CORRECTION_REG_s));
            RVal |= AmbaIK_SetColorCorrectionReg(mode, &ColorCorrRegCur[ViewID]);
            RVal |= AmbaIK_SetColorCorrection(mode, &VideoPipeCtrl->ColorCorr);
            if(RVal != 0U){
                /* MisraC */
            }
        }

        if (VideoPipeCtrl->GammaUpdate == UPDATE_FLAG_ON){
            RVal |= AmbaIK_SetToneCurve(mode, &VideoPipeCtrl->GammaTable);
        }

        if (VideoPipeCtrl->ChromaScaleUpdate == UPDATE_FLAG_ON){
            RVal |= AmbaIK_SetChromaScale(mode, &VideoPipeCtrl->ChromaScale);
        }

        if (VideoPipeCtrl->ChromaMedianUpdate == UPDATE_FLAG_ON){
            RVal |= AmbaIK_SetChromaMedianFilter(mode, &VideoPipeCtrl->ChromaMedian);
        }
#endif
    return RVal;
}
//End of Complexity
UINT32 Amba_Adj_SetVideoPipeCtrlParams(UINT32 ViewID, const AMBA_IK_MODE_CFG_s *mode)
{
    AMBA_IK_MODE_CFG_s     IKMode;
    UINT32                 HdrEnable = 0;
    UINT32                 RVal = 0U;
    ULONG                  Addr;
    //const ULONG            *pAddr = NULL;
    const PIPELINE_CONTROL_s     *VideoPipeCtrl;

    if (mode == NULL){
        RVal |= AmbaWrap_memset(&IKMode, 0, sizeof(IKMode));
    }else{
        RVal |= AmbaWrap_memcpy(&IKMode, mode, sizeof(IKMode));
    }
    RVal |= AmbaImgProc_ADJGetVideoAddr(ViewID, &Addr);
    RVal |= AmbaImgProc_HDRGetVideoMode(ViewID, &HdrEnable);
    RVal |= AmbaWrap_memcpy(&VideoPipeCtrl,&Addr,sizeof(ULONG));

    if(RVal == 0U){
        RVal |= AdjSetVPipeCParams_Sub1(ViewID, VideoPipeCtrl, HdrEnable, &IKMode);
        RVal |= AdjSetVPipeCParams_Sub2(VideoPipeCtrl, &IKMode);
        RVal |= AdjSetVPipeCParams_Sub3(VideoPipeCtrl, &IKMode);
        RVal |= AdjSetVPipeCParams_Sub4(VideoPipeCtrl, &IKMode);
    }else{
        /* MisraC */
    }
    if(RVal != 0U){
        /* MisraC */
    }
//AmbaPrint_PrintUInt5("---- Amba_Adj_SetVideoPipeCtrlParams : %d", ViewID, 0, 0, 0, 0);
    return RVal;
}


static UINT32 ResetVPipeCtrlFlags(UINT32 ViewID)
{
    ULONG                  Addr;
    PIPELINE_CONTROL_s     *VideoPipeCtrl;
    UINT32 RVal = 0U;

    RVal |= AmbaImgProc_ADJGetVideoAddr(ViewID, &Addr);
    RVal |= AmbaWrap_memcpy(&VideoPipeCtrl,&Addr,sizeof(ULONG));
    if(RVal != 0U){
#if defined(CONFIG_SOC_CV2FS) || defined(CONFIG_SOC_CV22FS)  || defined(CONFIG_SOC_CV5)  || defined(CONFIG_SOC_CV52)
        VideoPipeCtrl->RgbYuvMatrixUpdate= 0U;
        VideoPipeCtrl->BlackCorrUpdate= 0U;
        VideoPipeCtrl->AntiAliasingUpdate= 0U;
        VideoPipeCtrl->BadpixCorrUpdate= 0U;
        VideoPipeCtrl->CfaFilterUpdate= 0U;
        VideoPipeCtrl->RgbIrUpdate = 0U;
#if defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)
        VideoPipeCtrl->StoredIRUpdate = 0U;
#endif
        VideoPipeCtrl->DemosaicUpdate= 0U;
        VideoPipeCtrl->ColorCorrUpdate= 0U;
        VideoPipeCtrl->GammaUpdate= 0U;
        VideoPipeCtrl->ChromaScaleUpdate= 0U;
        VideoPipeCtrl->ChromaMedianUpdate= 0U;
        VideoPipeCtrl->AsfUpdate= 0U;
        VideoPipeCtrl->SharpenBothUpdate= 0U;
        VideoPipeCtrl->SharpenNoiseUpdate= 0U;
        VideoPipeCtrl->SharpenFirUpdate= 0U;
        VideoPipeCtrl->SharpenCoringUpdate= 0U;
        VideoPipeCtrl->SharpenCoringIndexScaleUpdate= 0U;
        VideoPipeCtrl->SharpenMinCoringResultUpdate= 0U;
        VideoPipeCtrl->SharpenMaxCoringResultUpdate= 0U;
        VideoPipeCtrl->SharpenScaleCoringUpdate= 0U;
        VideoPipeCtrl->SharpenBBothUpdate= 0U;
        VideoPipeCtrl->SharpenBNoiseUpdate= 0U;
        VideoPipeCtrl->SharpenBFirUpdate= 0U;
        VideoPipeCtrl->SharpenBCoringUpdate= 0U;
        VideoPipeCtrl->SharpenBCoringIndexScaleUpdate= 0U;
        VideoPipeCtrl->SharpenBMinCoringResultUpdate= 0U;
        VideoPipeCtrl->SharpenBMaxCoringResultUpdate= 0U;
        VideoPipeCtrl->SharpenBScaleCoringUpdate= 0U;
        VideoPipeCtrl->ChromaFilterUpdate= 0U;
        VideoPipeCtrl->GbGrMismatchUpdate= 0U;
        VideoPipeCtrl->ResamplerStrUpdate= 0U;
        VideoPipeCtrl->MctfInfoUpdate= 0U;
        VideoPipeCtrl->LisoProcessSelectUpdate= 0U;
        VideoPipeCtrl->ColorDeptNRUpdate= 0U;
        VideoPipeCtrl->WideChromaFilterUpdate= 0U;
        VideoPipeCtrl->WideChromaFilterCMBUpdate= 0U;
        VideoPipeCtrl->CEInfoUpdate = 0U;
#else
        VideoPipeCtrl->RgbYuvMatrixUpdate= 0U;
        VideoPipeCtrl->BlackCorrUpdate= 0U;
        VideoPipeCtrl->AntiAliasingUpdate= 0U;
        VideoPipeCtrl->BadpixCorrUpdate= 0U;
        VideoPipeCtrl->CfaFilterUpdate= 0U;
        VideoPipeCtrl->DemosaicUpdate= 0U;
        VideoPipeCtrl->ColorCorrUpdate= 0U;
        VideoPipeCtrl->GammaUpdate= 0U;
        VideoPipeCtrl->ChromaScaleUpdate= 0U;
        VideoPipeCtrl->ChromaMedianUpdate= 0U;
        VideoPipeCtrl->AsfUpdate= 0U;
        VideoPipeCtrl->SharpenBothUpdate= 0U;
        VideoPipeCtrl->SharpenNoiseUpdate= 0U;
        VideoPipeCtrl->SharpenFirUpdate= 0U;
        VideoPipeCtrl->SharpenCoringUpdate= 0U;
        VideoPipeCtrl->SharpenCoringIndexScaleUpdate= 0U;
        VideoPipeCtrl->SharpenMinCoringResultUpdate= 0U;
        VideoPipeCtrl->SharpenMaxCoringResultUpdate= 0U;
        VideoPipeCtrl->SharpenScaleCoringUpdate= 0U;
        VideoPipeCtrl->SharpenBBothUpdate= 0U;
        VideoPipeCtrl->SharpenBNoiseUpdate= 0U;
        VideoPipeCtrl->SharpenBFirUpdate= 0U;
        VideoPipeCtrl->SharpenBCoringUpdate= 0U;
        VideoPipeCtrl->SharpenBCoringIndexScaleUpdate= 0U;
        VideoPipeCtrl->SharpenBMinCoringResultUpdate= 0U;
        VideoPipeCtrl->SharpenBMaxCoringResultUpdate= 0U;
        VideoPipeCtrl->SharpenBScaleCoringUpdate= 0U;
        VideoPipeCtrl->ChromaFilterUpdate= 0U;
        VideoPipeCtrl->GbGrMismatchUpdate= 0U;
        VideoPipeCtrl->ResamplerStrUpdate= 0U;
        VideoPipeCtrl->MctfInfoUpdate= 0U;
        VideoPipeCtrl->LisoProcessSelectUpdate= 0U;
        VideoPipeCtrl->CEInfoUpdate = 0U;
#endif
    }
    return RVal;
}

static UINT32 ResetVHdrPipeCtrlFlags(UINT32 ViewID)
{
    ULONG                  Addr;
    PIPELINE_CONTROL_s     *VideoPipeCtrl;
    UINT32 RVal = 0U;

    RVal |= AmbaImgProc_ADJGetVideoAddr(ViewID, &Addr);
    RVal |= AmbaWrap_memcpy(&VideoPipeCtrl,&Addr,sizeof(ULONG));

    if(RVal != 0U){
#if defined(CONFIG_SOC_CV2FS) || defined(CONFIG_SOC_CV22FS)  || defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)
        VideoPipeCtrl->HdrBlendUpdate = 0U;
        VideoPipeCtrl->HdrBlackCorr0Update = 0U;
        VideoPipeCtrl->HdrBlackCorr1Update = 0U;
        VideoPipeCtrl->HdrBlackCorr2Update = 0U;
        /*Compand & Decompand*/
        VideoPipeCtrl->FEToneCurveUpdate = 0U;
        VideoPipeCtrl->HDSToneCurveUpdate = 0U;
#else
        VideoPipeCtrl->HdrBlendUpdate = 0U;
        VideoPipeCtrl->HdrBlackCorr0Update = 0U;
        VideoPipeCtrl->HdrBlackCorr1Update = 0U;
        VideoPipeCtrl->HdrBlackCorr2Update = 0U;
        /*Compand & Decompand*/
        VideoPipeCtrl->FEToneCurveUpdate = 0U;
        VideoPipeCtrl->HDSToneCurveUpdate = 0U;
#endif
    }
    return RVal;
}

static UINT32 ResetVCePipeCtrlFlags(UINT32 ViewID)
{
    ULONG                  Addr;
    PIPELINE_CONTROL_s     *VideoPipeCtrl;
    UINT32 RVal = 0U;

    RVal |= AmbaImgProc_ADJGetVideoAddr(ViewID, &Addr);
    RVal |= AmbaWrap_memcpy(&VideoPipeCtrl,&Addr,sizeof(ULONG));
    if(RVal != 0U){
#if defined(CONFIG_SOC_CV2FS) || defined(CONFIG_SOC_CV22FS)  || defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)
        VideoPipeCtrl->CEInputTableUpdate = 0U;
        VideoPipeCtrl->CEOutputTableUpdate = 0U;
#else
        VideoPipeCtrl->CEInputTableUpdate = 0U;
        VideoPipeCtrl->CEOutputTableUpdate = 0U;
#endif
    }
    return RVal;
}

UINT32 Amba_Adj_GetVideoUpdFlag(UINT32 ViewID, UINT16 *pUpdateFlag)
{
    UINT16 rval16 = 0;
    ULONG                  Addr;
    const PIPELINE_CONTROL_s     *VideoPipeCtrl;
    UINT32 RVal = 0U;

    RVal |= AmbaImgProc_ADJGetVideoAddr(ViewID, &Addr);
    RVal |= AmbaWrap_memcpy(&VideoPipeCtrl,&Addr,sizeof(ULONG));
    if(RVal == 0U){
#if defined(CONFIG_SOC_CV2FS) || defined(CONFIG_SOC_CV22FS)  || defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)
        RVal += (UINT32)VideoPipeCtrl->RgbYuvMatrixUpdate +
                (UINT32)VideoPipeCtrl->BlackCorrUpdate +
                (UINT32)VideoPipeCtrl->AntiAliasingUpdate +
                (UINT32)VideoPipeCtrl->BadpixCorrUpdate +
                (UINT32)VideoPipeCtrl->CfaFilterUpdate +
                (UINT32)VideoPipeCtrl->DemosaicUpdate +
                (UINT32)VideoPipeCtrl->ColorCorrUpdate +
                (UINT32)VideoPipeCtrl->GammaUpdate +
                (UINT32)VideoPipeCtrl->ChromaScaleUpdate +
                (UINT32)VideoPipeCtrl->ChromaMedianUpdate +
                (UINT32)VideoPipeCtrl->AsfUpdate +
                (UINT32)VideoPipeCtrl->SharpenBothUpdate +
                (UINT32)VideoPipeCtrl->SharpenNoiseUpdate +
                (UINT32)VideoPipeCtrl->SharpenFirUpdate +
                (UINT32)VideoPipeCtrl->SharpenCoringUpdate +
                (UINT32)VideoPipeCtrl->SharpenCoringIndexScaleUpdate +
                (UINT32)VideoPipeCtrl->SharpenMinCoringResultUpdate +
                (UINT32)VideoPipeCtrl->SharpenMaxCoringResultUpdate +
                (UINT32)VideoPipeCtrl->SharpenScaleCoringUpdate +
                (UINT32)VideoPipeCtrl->SharpenBBothUpdate +
                (UINT32)VideoPipeCtrl->SharpenBNoiseUpdate +
                (UINT32)VideoPipeCtrl->SharpenBFirUpdate +
                (UINT32)VideoPipeCtrl->SharpenBCoringUpdate +
                (UINT32)VideoPipeCtrl->SharpenBCoringIndexScaleUpdate +
                (UINT32)VideoPipeCtrl->SharpenBMinCoringResultUpdate +
                (UINT32)VideoPipeCtrl->SharpenBMaxCoringResultUpdate +
                (UINT32)VideoPipeCtrl->SharpenBScaleCoringUpdate +
                (UINT32)VideoPipeCtrl->ChromaFilterUpdate +
                (UINT32)VideoPipeCtrl->GbGrMismatchUpdate +
                (UINT32)VideoPipeCtrl->ResamplerStrUpdate +
                (UINT32)VideoPipeCtrl->MctfInfoUpdate +
                (UINT32)VideoPipeCtrl->CEInfoUpdate +
                (UINT32)VideoPipeCtrl->LisoProcessSelectUpdate +
                (UINT32)VideoPipeCtrl->WideChromaFilterUpdate +
                (UINT32)VideoPipeCtrl->WideChromaFilterCMBUpdate +
                (UINT32)VideoPipeCtrl->ColorDeptNRUpdate;

        if(RVal > 0U){
            rval16 = 1U;
        }
        *pUpdateFlag = rval16;
#else
        RVal += (UINT32)VideoPipeCtrl->RgbYuvMatrixUpdate +
                (UINT32)VideoPipeCtrl->BlackCorrUpdate +
                (UINT32)VideoPipeCtrl->AntiAliasingUpdate +
                (UINT32)VideoPipeCtrl->BadpixCorrUpdate +
                (UINT32)VideoPipeCtrl->CfaFilterUpdate +
                (UINT32)VideoPipeCtrl->DemosaicUpdate +
                (UINT32)VideoPipeCtrl->ColorCorrUpdate +
                (UINT32)VideoPipeCtrl->GammaUpdate +
                (UINT32)VideoPipeCtrl->ChromaScaleUpdate +
                (UINT32)VideoPipeCtrl->ChromaMedianUpdate +
                (UINT32)VideoPipeCtrl->AsfUpdate +
                (UINT32)VideoPipeCtrl->SharpenBothUpdate +
                (UINT32)VideoPipeCtrl->SharpenNoiseUpdate +
                (UINT32)VideoPipeCtrl->SharpenFirUpdate +
                (UINT32)VideoPipeCtrl->SharpenCoringUpdate +
                (UINT32)VideoPipeCtrl->SharpenCoringIndexScaleUpdate +
                (UINT32)VideoPipeCtrl->SharpenMinCoringResultUpdate +
                (UINT32)VideoPipeCtrl->SharpenMaxCoringResultUpdate +
                (UINT32)VideoPipeCtrl->SharpenScaleCoringUpdate +
                (UINT32)VideoPipeCtrl->SharpenBBothUpdate +
                (UINT32)VideoPipeCtrl->SharpenBNoiseUpdate +
                (UINT32)VideoPipeCtrl->SharpenBFirUpdate +
                (UINT32)VideoPipeCtrl->SharpenBCoringUpdate +
                (UINT32)VideoPipeCtrl->SharpenBCoringIndexScaleUpdate +
                (UINT32)VideoPipeCtrl->SharpenBMinCoringResultUpdate +
                (UINT32)VideoPipeCtrl->SharpenBMaxCoringResultUpdate +
                (UINT32)VideoPipeCtrl->SharpenBScaleCoringUpdate +
                (UINT32)VideoPipeCtrl->ChromaFilterUpdate +
                (UINT32)VideoPipeCtrl->GbGrMismatchUpdate +
                (UINT32)VideoPipeCtrl->ResamplerStrUpdate +
                (UINT32)VideoPipeCtrl->MctfInfoUpdate +
                (UINT32)VideoPipeCtrl->CEInfoUpdate +
                (UINT32)VideoPipeCtrl->LisoProcessSelectUpdate;
        if(RVal > 0U){
            rval16 = 1U;
        }
        *pUpdateFlag = rval16;
#endif
    }
    return RVal;
}

UINT32 Amba_Adj_ResetVideoFlags(UINT32 ViewID, UINT8 Mode)
{
    UINT32 RVal = 0;
    if(Mode == 0U){ //Normal
        RVal |= ResetVPipeCtrlFlags(ViewID);
    }else if (Mode == 1U){ // HDR
        RVal |= ResetVHdrPipeCtrlFlags(ViewID);
    }else if (Mode == 2U){ //CE
        RVal |= ResetVCePipeCtrlFlags(ViewID);
    }else{
        /* MisraC */
    }
    return RVal;
}
//Still
//Start of Complexity
#if defined(CONFIG_SOC_CV2FS) || defined(CONFIG_SOC_CV22FS)

// #elif defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)

#else

static UINT32 AdjSetSPipeCParams_HISO8(const PIPELINE_STILL_CONTROL_s *StillPipeCtrl, const AMBA_IK_MODE_CFG_s *mode)
{
    UINT32                 RVal = 0U;
        //HISO HiLi Combine
        if (StillPipeCtrl->HIsoHiLiCombineUpdate == UPDATE_FLAG_ON){
            RVal |= AmbaIK_SetHiliCombine(mode, &StillPipeCtrl->HIsoHiLiCombine);
        }

        //HISO Luma Combine
        if (StillPipeCtrl->HIsoLumaFilterCombineUpdate == UPDATE_FLAG_ON){
            RVal |= AmbaIK_SetHiLumaCombine(mode, &StillPipeCtrl->HIsoLumaFilterCombine);
        }
        if (StillPipeCtrl->HIsoLowAsfCombineUpdate == UPDATE_FLAG_ON){
            RVal |= AmbaIK_SetHiLowASFCombine(mode, &StillPipeCtrl->HIsoLowAsfCombine);
        }

        //HISO FreqRecover
        if (StillPipeCtrl->HIsoLiLumaMidHightFreqRcvrUpdate == UPDATE_FLAG_ON){
            RVal |= AmbaIK_SetHiMidHighFreqRecover(mode, &StillPipeCtrl->HIsoLiLumaMidHightFreqRcvr);
        }

        if (StillPipeCtrl->HIsoLumaBlendUpdate == UPDATE_FLAG_ON){
            RVal |= AmbaIK_SetHiLumaBlend(mode, &StillPipeCtrl->HIsoLumaBlend);
        }

        if (StillPipeCtrl->HIsoNonSmoothUpdate == UPDATE_FLAG_ON){
            RVal |= AmbaIK_SetHiNonSmoothDetect(mode, &StillPipeCtrl->HIsoNonSmooth);
        }
    return RVal;
}

static UINT32 AdjSetSPipeCParams_HISO7(const PIPELINE_STILL_CONTROL_s *StillPipeCtrl, const AMBA_IK_MODE_CFG_s *mode)
{
    UINT32                 RVal = 0U;
        //HISO Li Chroma ASF
        if (StillPipeCtrl->HIsoLiChromaAsfUpdate == UPDATE_FLAG_ON){
            RVal |= AmbaIK_SetHiLowChromaAsf(mode, &StillPipeCtrl->HIsoLiChromaAsf);
        }
        //HISO Chroma ASF
        if (StillPipeCtrl->HIsoChromaAsfUpdate == UPDATE_FLAG_ON){
            RVal |= AmbaIK_SetHiChromaAsf(mode, &StillPipeCtrl->HIsoChromaAsf);
        }

        //HISO Chroma filter
        if (StillPipeCtrl->HIsoChromaFilterPreUpdate == UPDATE_FLAG_ON){
            RVal |= AmbaIK_SetHiChromaFilterPre(mode, &StillPipeCtrl->HIsoChromaFilterPre);
        }
        if (StillPipeCtrl->HIsoChromaFilterHighUpdate == UPDATE_FLAG_ON){
            RVal |= AmbaIK_SetHiChromaFilterHigh(mode, &StillPipeCtrl->HIsoChromaFilterHigh);
        }
        if (StillPipeCtrl->HIsoChromaFilterMedUpdate == UPDATE_FLAG_ON){
            RVal |= AmbaIK_SetHiChromaFilterMed(mode, &StillPipeCtrl->HIsoChromaFilterMed);
        }
        if (StillPipeCtrl->HIsoChromaFilterLowUpdate == UPDATE_FLAG_ON){
            RVal |= AmbaIK_SetHiChromaFilterLow(mode, &StillPipeCtrl->HIsoChromaFilterLow);
        }
        if (StillPipeCtrl->HIsoChromaFilterVLowUpdate == UPDATE_FLAG_ON){
            RVal |= AmbaIK_SetHiChromaFilterVeryLow(mode, &StillPipeCtrl->HIsoChromaFilterVLow);
        }


        //HISO Chroma Combine
        if (StillPipeCtrl->HIsoChromaFilterMedCombineUpdate == UPDATE_FLAG_ON){
            RVal |= AmbaIK_SetHiChromaFltrMedCom(mode, &StillPipeCtrl->HIsoChromaFilterMedCombine);
        }
        if (StillPipeCtrl->HIsoChromaFilterLowCombineUpdate == UPDATE_FLAG_ON){
            RVal |= AmbaIK_SetHiChromaFltrLowCom(mode, &StillPipeCtrl->HIsoChromaFilterLowCombine);
        }
        if (StillPipeCtrl->HIsoChromaFilterVLowCombineUpdate == UPDATE_FLAG_ON){
            RVal |= AmbaIK_SetHiChromaFltrVeryLowCom(mode, &StillPipeCtrl->HIsoChromaFilterVLowCombine);
        }
    return RVal;
}

static UINT32 AdjSetSPipeCParams_HISO6(const PIPELINE_STILL_CONTROL_s *StillPipeCtrl, const AMBA_IK_MODE_CFG_s *mode)
{
    UINT32                 RVal = 0U;
        //HIso Li2 Sharpen
        if (StillPipeCtrl->HIsoLi2SharpenBBothUpdate == UPDATE_FLAG_ON){
            RVal |= AmbaIK_SetLi2ShpNsBoth(mode, &StillPipeCtrl->HIsoLi2SharpenBBoth);
        }

        if (StillPipeCtrl->HIsoLi2SharpenNoiseUpdate == UPDATE_FLAG_ON){
            RVal |= AmbaIK_SetLi2ShpNsNoise(mode, &StillPipeCtrl->HIsoLi2SharpenNoise);
        }

        if (StillPipeCtrl->HIsoLi2SharpenFirUpdate == UPDATE_FLAG_ON){
            RVal |= AmbaIK_SetLi2ShpNsFir(mode, &StillPipeCtrl->HIsoLi2SharpenFir);
        }

        if (StillPipeCtrl->HIsoLi2SharpenBCoringUpdate == UPDATE_FLAG_ON){
            RVal |= AmbaIK_SetLi2ShpNsCoring(mode, &StillPipeCtrl->HIsoLi2SharpenBCoring);
        }

        if (StillPipeCtrl->HIsoLi2SharpenCoringIndexScaleUpdate == UPDATE_FLAG_ON){
            RVal |= AmbaIK_SetLi2ShpNsCorIdxScl(mode, &StillPipeCtrl->HIsoLi2SharpenCoringIndexScale);
        }

        if (StillPipeCtrl->HIsoLi2SharpenMinCoringResultUpdate == UPDATE_FLAG_ON){
            RVal |= AmbaIK_SetLi2ShpNsMinCorRst(mode, &StillPipeCtrl->HIsoLi2SharpenMinCoringResult);
        }

        if (StillPipeCtrl->HIsoLi2SharpenScaleCoringUpdate == UPDATE_FLAG_ON){
            RVal |= AmbaIK_SetLi2ShpNsSclCor(mode, &StillPipeCtrl->HIsoLi2SharpenScaleCoring);
        }

        if (StillPipeCtrl->HIsoLi2SharpenMaxCoringResultUpdate == UPDATE_FLAG_ON){
            RVal |= AmbaIK_SetLi2ShpNsMaxCorRst(mode, &StillPipeCtrl->HIsoLi2SharpenMaxCoringResult);
        }
    return RVal;
}

static UINT32 AdjSetSPipeCParams_HISO5(const PIPELINE_STILL_CONTROL_s *StillPipeCtrl, const AMBA_IK_MODE_CFG_s *mode)
{
    UINT32                 RVal = 0U;
        //Hili Sharpen
        if (StillPipeCtrl->HIsoLiSharpenBBothUpdate == UPDATE_FLAG_ON){
            RVal |= AmbaIK_SetHiliShpNsBoth(mode, &StillPipeCtrl->HIsoLiSharpenBBoth);
        }

        if (StillPipeCtrl->HIsoLiSharpenNoiseUpdate == UPDATE_FLAG_ON){
            RVal |= AmbaIK_SetHiliShpNsNoise(mode, &StillPipeCtrl->HIsoLiSharpenNoise);
        }

        if (StillPipeCtrl->HIsoLiSharpenFirUpdate == UPDATE_FLAG_ON){
            RVal |= AmbaIK_SetHiliShpNsFir(mode, &StillPipeCtrl->HIsoLiSharpenFir);
        }

        if (StillPipeCtrl->HIsoLiSharpenBCoringUpdate == UPDATE_FLAG_ON){
            RVal |= AmbaIK_SetHiliShpNsCoring(mode, &StillPipeCtrl->HIsoLiSharpenBCoring);
        }

        if (StillPipeCtrl->HIsoLiSharpenCoringIndexScaleUpdate == UPDATE_FLAG_ON){
            RVal |= AmbaIK_SetHiliShpNsCorIdxScl(mode, &StillPipeCtrl->HIsoLiSharpenCoringIndexScale);
        }

        if (StillPipeCtrl->HIsoLiSharpenMinCoringResultUpdate == UPDATE_FLAG_ON){
            RVal |= AmbaIK_SetHiliShpNsMinCorRst(mode, &StillPipeCtrl->HIsoLiSharpenMinCoringResult);
        }

        if (StillPipeCtrl->HIsoLiSharpenScaleCoringUpdate == UPDATE_FLAG_ON){
            RVal |= AmbaIK_SetHiliShpNsSclCor(mode, &StillPipeCtrl->HIsoLiSharpenScaleCoring);
        }

        if (StillPipeCtrl->HIsoLiSharpenMaxCoringResultUpdate == UPDATE_FLAG_ON){
            RVal |= AmbaIK_SetHiliShpNsMaxCorRst(mode, &StillPipeCtrl->HIsoLiSharpenMaxCoringResult);
        }
    return RVal;
}

static UINT32 AdjSetSPipeCParams_HISO4(const PIPELINE_STILL_CONTROL_s *StillPipeCtrl, const AMBA_IK_MODE_CFG_s *mode)
{
    UINT32                 RVal = 0U;
        //Med Sharpen
        if (StillPipeCtrl->HIsoMedSharpenBothUpdate == UPDATE_FLAG_ON){
            RVal |= AmbaIK_SetHiMedShpNsBoth(mode, &StillPipeCtrl->HIsoMedSharpenBoth);
        }

        if (StillPipeCtrl->HIsoMedSharpenNoiseUpdate == UPDATE_FLAG_ON){
            RVal |= AmbaIK_SetHiMedShpNsNoise(mode, &StillPipeCtrl->HIsoMedSharpenNoise);
        }

        if (StillPipeCtrl->HIsoMedSharpenFirUpdate == UPDATE_FLAG_ON){
            RVal |= AmbaIK_SetHiMedShpNsFir(mode, &StillPipeCtrl->HIsoMedSharpenFir);
        }

        if (StillPipeCtrl->HIsoMedSharpenCoringUpdate == UPDATE_FLAG_ON){
            RVal |= AmbaIK_SetHiMedShpNsCoring(mode, &StillPipeCtrl->HIsoMedSharpenCoring);
        }

        if (StillPipeCtrl->HIsoMedSharpenCoringIndexScaleUpdate == UPDATE_FLAG_ON){
            RVal |= AmbaIK_SetHiMedShpNsCorIdxScl(mode, &StillPipeCtrl->HIsoMedSharpenCoringIndexScale);
        }

        if (StillPipeCtrl->HIsoMedSharpenMinCoringResultUpdate == UPDATE_FLAG_ON){
            RVal |= AmbaIK_SetHiMedShpNsMinCorRst(mode, &StillPipeCtrl->HIsoMedSharpenMinCoringResult);
        }

        if (StillPipeCtrl->HIsoMedSharpenScaleCoringUpdate == UPDATE_FLAG_ON){
            RVal |= AmbaIK_SetHiMedShpNsSclCor(mode, &StillPipeCtrl->HIsoMedSharpenScaleCoring);
        }

        if (StillPipeCtrl->HIsoMedSharpenMaxCoringResultUpdate == UPDATE_FLAG_ON){
            RVal |= AmbaIK_SetHiMedShpNsMaxCorRst(mode, &StillPipeCtrl->HIsoMedSharpenMaxCoringResult);
        }

    return RVal;
}

static UINT32 AdjSetSPipeCParams_HISO3(const PIPELINE_STILL_CONTROL_s *StillPipeCtrl, const AMBA_IK_MODE_CFG_s *mode)
{
    UINT32                 RVal = 0U;
        //High Sharpen
        if (StillPipeCtrl->HIsoHighSharpenBothUpdate == UPDATE_FLAG_ON){
            RVal |= AmbaIK_SetHiHighShpNsBoth(mode, &StillPipeCtrl->HIsoHighSharpenBoth);
        }

        if (StillPipeCtrl->HIsoHighSharpenNoiseUpdate == UPDATE_FLAG_ON){
            RVal |= AmbaIK_SetHiHighShpNsNoise(mode, &StillPipeCtrl->HIsoHighSharpenNoise);
        }

        if (StillPipeCtrl->HIsoHighSharpenFirUpdate == UPDATE_FLAG_ON){
            RVal |= AmbaIK_SetHiHighShpNsFir(mode, &StillPipeCtrl->HIsoHighSharpenFir);
        }

        if (StillPipeCtrl->HIsoHighSharpenCoringUpdate == UPDATE_FLAG_ON){
            RVal |= AmbaIK_SetHiHighShpNsCoring(mode, &StillPipeCtrl->HIsoHighSharpenCoring);
        }

        if (StillPipeCtrl->HIsoHighSharpenCoringIndxScaleUpdate == UPDATE_FLAG_ON){
            RVal |= AmbaIK_SetHiHighShpNsCorIdxScl(mode, &StillPipeCtrl->HIsoHighSharpenCoringIndxScale);
        }

        if (StillPipeCtrl->HIsoHighSharpenMinCoringResultUpdate == UPDATE_FLAG_ON){
            RVal |= AmbaIK_SetHiHighShpNsMinCorRst(mode, &StillPipeCtrl->HIsoHighSharpenMinCoringResult);
        }

        if (StillPipeCtrl->HIsoHighSharpenScaleCoringUpdate == UPDATE_FLAG_ON){
            RVal |= AmbaIK_SetHiHighShpNsSclCor(mode, &StillPipeCtrl->HIsoHighSharpenScaleCoring);
        }

        if (StillPipeCtrl->HIsoHighSharpenMaxCoringResultUpdate == UPDATE_FLAG_ON){
            RVal |= AmbaIK_SetHiHighShpNsMaxCorRst(mode, &StillPipeCtrl->HIsoHighSharpenMaxCoringResult);
        }
    return RVal;
}

static UINT32 AdjSetSPipeCParams_HISO2(const PIPELINE_STILL_CONTROL_s *StillPipeCtrl, const AMBA_IK_MODE_CFG_s *mode)
{
    UINT32                 RVal = 0U;
        //HIsoSelect
        if (StillPipeCtrl->HIsoSelectUpdate == UPDATE_FLAG_ON){
            RVal |= AmbaIK_SetHiSelect(mode, &StillPipeCtrl->HIsoSelect);
        }

        //HISO Luma ASF
        if (StillPipeCtrl->HIsoAsfUpdate == UPDATE_FLAG_ON){
            RVal |= AmbaIK_SetHiAsf(mode, &StillPipeCtrl->HIsoAsf);
        }

        if (StillPipeCtrl->HIsoHighAsfUpdate == UPDATE_FLAG_ON){
            RVal |= AmbaIK_SetHiHighAsf(mode, &StillPipeCtrl->HIsoHighAsf);
        }

        if (StillPipeCtrl->HIsoHigh2AsfUpdate == UPDATE_FLAG_ON){
            RVal |= AmbaIK_SetHiHigh2Asf(mode, &StillPipeCtrl->HIsoHigh2Asf);
        }

        if (StillPipeCtrl->HIsoMedAsfUpdate == UPDATE_FLAG_ON){
            RVal |= AmbaIK_SetHiMed1Asf(mode, &StillPipeCtrl->HIsoMedAsf);
        }

        if (StillPipeCtrl->HIsoMed2AsfUpdate == UPDATE_FLAG_ON){
            RVal |= AmbaIK_SetHiMed2Asf(mode, &StillPipeCtrl->HIsoMed2Asf);
        }

        if (StillPipeCtrl->HIsoLowAsfUpdate == UPDATE_FLAG_ON){
            RVal |= AmbaIK_SetHiLowAsf(mode, &StillPipeCtrl->HIsoLowAsf);
        }

        if (StillPipeCtrl->HIsoLi2AsfUpdate == UPDATE_FLAG_ON){
            RVal |= AmbaIK_SetLi2Asf(mode, &StillPipeCtrl->HIsoLi2Asf);
        }
    return RVal;
}

static UINT32 AdjSetSPipeCParams_HISO1(const PIPELINE_STILL_CONTROL_s *StillPipeCtrl, const AMBA_IK_MODE_CFG_s *mode)
{
    UINT32                 RVal = 0U;
        if (StillPipeCtrl->HIsoCfaLeakageFilterUpdate == UPDATE_FLAG_ON){
            RVal |= AmbaIK_SetHiCfaLeakageFilter(mode, &StillPipeCtrl->HIsoCfaLeakageFilter);
        }

        if (StillPipeCtrl->HIsoAntiAliasingUpdate == UPDATE_FLAG_ON){
            RVal |= AmbaIK_SetHiAntiAliasing(mode, &StillPipeCtrl->HIsoAntiAliasing);
        }

        if (StillPipeCtrl->HIsoBadpixCorrUpdate == UPDATE_FLAG_ON){
            RVal |= AmbaIK_SetHiDynamicBPC(mode, &StillPipeCtrl->HIsoBadpixCorr);
        }

        if (StillPipeCtrl->HIsoCfaFilterUpdate == UPDATE_FLAG_ON){
            RVal |= AmbaIK_SetHiCfaNoiseFilter(mode, &StillPipeCtrl->HIsoCfaFilter);
        }

        if (StillPipeCtrl->HIsoGbGrMismatchUpdate == UPDATE_FLAG_ON){
            RVal |= AmbaIK_SetHiGrGbMismatch(mode, &StillPipeCtrl->HIsoGbGrMismatch);
        }

        if (StillPipeCtrl->HIsoDemosaicUpdate == UPDATE_FLAG_ON){
            RVal |= AmbaIK_SetHiDemosaic(mode, &StillPipeCtrl->HIsoDemosaic);
        }

        if (StillPipeCtrl->HIsoChromaMedianUpdate == UPDATE_FLAG_ON){
            RVal |= AmbaIK_SetHiChromaMedianFilter(mode, &StillPipeCtrl->HIsoChromaMedian);
        }

        if (StillPipeCtrl->HIsoLi2CfaLeakageFilterUpdate == UPDATE_FLAG_ON){
            RVal |= AmbaIK_SetLi2CfaLeakageFilter(mode, &StillPipeCtrl->HIsoLi2CfaLeakageFilter);
        }

        if (StillPipeCtrl->HIsoLi2AntiAliasingUpdate == UPDATE_FLAG_ON){
            RVal |= AmbaIK_SetLi2AntiAliasing(mode, &StillPipeCtrl->HIsoLi2AntiAliasing);
        }

        if (StillPipeCtrl->HIsoLi2BadpixCorrUpdate == UPDATE_FLAG_ON){
            RVal |= AmbaIK_SetLi2DynamicBPC(mode, &StillPipeCtrl->HIsoLi2BadpixCorr);
        }

        if (StillPipeCtrl->HIsoLi2CfaFilterUpdate == UPDATE_FLAG_ON){
            RVal |= AmbaIK_SetLi2CfaNoiseFilter(mode, &StillPipeCtrl->HIsoLi2CfaFilter);
        }

        if (StillPipeCtrl->HIsoLi2GbGrMismatchUpdate == UPDATE_FLAG_ON){
            RVal |= AmbaIK_SetLi2GrGbMismatch(mode, &StillPipeCtrl->HIsoLi2GbGrMismatch);
        }

        if (StillPipeCtrl->HIsoLi2DemosaicUpdate == UPDATE_FLAG_ON){
            RVal |= AmbaIK_SetLi2Demosaic(mode, &StillPipeCtrl->HIsoLi2Demosaic);
        }
    return RVal;
}

static UINT32 AdjSetSPipeCParams_Sub4(const PIPELINE_STILL_CONTROL_s *StillPipeCtrl, const AMBA_IK_MODE_CFG_s *mode)
{
    UINT32                 RVal = 0U;
        if (StillPipeCtrl->ChromaFilterUpdate == UPDATE_FLAG_ON){
            RVal |= AmbaIK_SetChromaFilter(mode, &StillPipeCtrl->ChromaFilter);
        }

        if (StillPipeCtrl->GbGrMismatchUpdate == UPDATE_FLAG_ON){
            RVal |= AmbaIK_SetGrgbMismatch(mode, &StillPipeCtrl->GbGrMismatch);
        }

        if (StillPipeCtrl->ResamplerStrUpdate == UPDATE_FLAG_ON){
            RVal |= AmbaIK_SetResampStrength(mode, &StillPipeCtrl->ResamplerStr);
        }

        if(StillPipeCtrl->CEInfoUpdate == UPDATE_FLAG_ON){
            RVal |= AmbaIK_SetCe(mode, &StillPipeCtrl->CEInfo);
        }

        //still ik pipe is the same as video; ik team suggestes setting all zero mctf
        RVal |= AmbaIK_SetVideoMctf(mode, &StillPipeCtrl->MctfInfo);
        RVal |= AmbaIK_SetVideoMctfTa(mode, &StillPipeCtrl->MctfTemporalAdjust);
#if defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)
        if (StillPipeCtrl->ColorDeptNRUpdate == UPDATE_FLAG_ON){
            RVal |= AmbaIK_SetLumaNoiseReduction(mode, &StillPipeCtrl->ColorDeptNR);
        }

        if (StillPipeCtrl->WideChromaFilterUpdate == UPDATE_FLAG_ON){
            RVal |= AmbaIK_SetWideChromaFilter(mode, &StillPipeCtrl->WideChromaFilter);
        }

        if (StillPipeCtrl->WideChromaFilterCMBUpdate == UPDATE_FLAG_ON){
            RVal |= AmbaIK_SetWideChromaFilterCombine(mode, &StillPipeCtrl->WideChromaFilterCMB);
        }
#endif
    return RVal;
}

static UINT32 AdjSetSPipeCParams_Sub3(const PIPELINE_STILL_CONTROL_s *StillPipeCtrl, const AMBA_IK_MODE_CFG_s *mode)
{
    UINT32                 RVal = 0U;
        if (StillPipeCtrl->SharpenBBothUpdate == UPDATE_FLAG_ON){
            RVal |= AmbaIK_SetFnlShpNsBoth(mode, &StillPipeCtrl->SharpenBBoth);
        }

        if (StillPipeCtrl->SharpenBNoiseUpdate == UPDATE_FLAG_ON){
            RVal |= AmbaIK_SetFnlShpNsNoise(mode, &StillPipeCtrl->SharpenBNoise);
        }

        if (StillPipeCtrl->SharpenBFirUpdate == UPDATE_FLAG_ON){
            RVal |= AmbaIK_SetFnlShpNsFir(mode, &StillPipeCtrl->SharpenBFir);
        }

        if (StillPipeCtrl->SharpenBCoringUpdate == UPDATE_FLAG_ON){
            RVal |= AmbaIK_SetFnlShpNsCoring(mode, &StillPipeCtrl->SharpenBCoring);
        }

        if (StillPipeCtrl->SharpenBCoringIndexScaleUpdate == UPDATE_FLAG_ON){
            RVal |= AmbaIK_SetFnlShpNsCorIdxScl(mode, &StillPipeCtrl->SharpenBCoringIndexScale);
        }

        if (StillPipeCtrl->SharpenBMinCoringResultUpdate == UPDATE_FLAG_ON){
            RVal |= AmbaIK_SetFnlShpNsMinCorRst(mode, &StillPipeCtrl->SharpenBMinCoringResult);
        }

        if (StillPipeCtrl->SharpenBMaxCoringResultUpdate == UPDATE_FLAG_ON){
            RVal |= AmbaIK_SetFnlShpNsMaxCorRst(mode, &StillPipeCtrl->SharpenBMaxCoringResult);
        }

        if (StillPipeCtrl->SharpenBScaleCoringUpdate == UPDATE_FLAG_ON){
            RVal |= AmbaIK_SetFnlShpNsSclCor(mode, &StillPipeCtrl->SharpenBScaleCoring);
        }
    return RVal;
}

static UINT32 AdjSetSPipeCParams_Sub2(const PIPELINE_STILL_CONTROL_s *StillPipeCtrl, const AMBA_IK_MODE_CFG_s *mode)
{
    UINT32                 RVal = 0U;
        if (StillPipeCtrl->AsfUpdate == UPDATE_FLAG_ON){
            RVal |= AmbaIK_SetAdvSpatFltr(mode, &StillPipeCtrl->Asf);
        }

        if(StillPipeCtrl->SharpenBothUpdate == UPDATE_FLAG_ON){
            RVal |= AmbaIK_SetFstShpNsBoth(mode, &StillPipeCtrl->SharpenBoth);
        }

        if(StillPipeCtrl->SharpenNoiseUpdate == UPDATE_FLAG_ON){
            RVal |= AmbaIK_SetFstShpNsNoise(mode, &StillPipeCtrl->SharpenNoise);
        }

        if(StillPipeCtrl->SharpenFirUpdate == UPDATE_FLAG_ON){ // 20210604
            RVal |= AmbaIK_SetFstShpNsFir(mode, &StillPipeCtrl->SharpenFir);
        }

        if(StillPipeCtrl->SharpenCoringUpdate == UPDATE_FLAG_ON){
            RVal |= AmbaIK_SetFstShpNsCoring(mode, &StillPipeCtrl->SharpenCoring);
        }

        if(StillPipeCtrl->SharpenCoringIndexScaleUpdate == UPDATE_FLAG_ON){
            RVal |= AmbaIK_SetFstShpNsCorIdxScl(mode, &StillPipeCtrl->SharpenCoringIndexScale);
        }

        if(StillPipeCtrl->SharpenMinCoringResultUpdate == UPDATE_FLAG_ON){
            RVal |= AmbaIK_SetFstShpNsMinCorRst(mode, &StillPipeCtrl->SharpenMinCoringResult);
        }

        if (StillPipeCtrl->SharpenMaxCoringResultUpdate == UPDATE_FLAG_ON){
            RVal |= AmbaIK_SetFstShpNsMaxCorRst(mode, &StillPipeCtrl->SharpenMaxCoringResult);
        }

        if(StillPipeCtrl->SharpenScaleCoringUpdate == UPDATE_FLAG_ON){
            RVal |= AmbaIK_SetFstShpNsSclCor(mode, &StillPipeCtrl->SharpenScaleCoring);
        }
    return RVal;
}

static UINT32 AdjSetSPipeCParams_Sub1(UINT32 ViewID, const PIPELINE_STILL_CONTROL_s *StillPipeCtrl, const AMBA_IK_MODE_CFG_s *mode)
{
    UINT32                 RVal = 0U;
        if(StillPipeCtrl->LisoProcessSelectUpdate == UPDATE_FLAG_ON){
            RVal |= AmbaIK_SetFirstLumaProcMode(mode, &StillPipeCtrl->LisoProcessSelect);
        }

        if (StillPipeCtrl->RgbYuvMatrixUpdate == UPDATE_FLAG_ON){ // 20210604
            RVal |= AmbaIK_SetRgbToYuvMatrix(mode, &StillPipeCtrl->RgbYuvMatrix);
        }

        if (StillPipeCtrl->BlackCorrUpdate == UPDATE_FLAG_ON){
            RVal |= AmbaIK_SetFeStaticBlc(mode, &StillPipeCtrl->BlackCorr, 0);
        }

        if (StillPipeCtrl->AntiAliasingUpdate == UPDATE_FLAG_ON){
            RVal |= AmbaIK_SetAntiAliasing(mode, &StillPipeCtrl->AntiAliasing);
        }

        if (StillPipeCtrl->BadpixCorrUpdate == UPDATE_FLAG_ON){
            RVal |= AmbaIK_SetDynamicBadPxlCor(mode, &StillPipeCtrl->BadpixCorr);
        }

        if (StillPipeCtrl->CfaFilterUpdate == UPDATE_FLAG_ON){ // 20210604
            RVal |= AmbaIK_SetCfaNoiseFilter(mode, &StillPipeCtrl->CfaFilter);
        }

        if (StillPipeCtrl->DemosaicUpdate == UPDATE_FLAG_ON){
            RVal |= AmbaIK_SetDemosaic(mode, &StillPipeCtrl->Demosaic);
        }

#if defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)
        if (StillPipeCtrl->ColorCorrUpdate == UPDATE_FLAG_ON) {
            static IMG_PARAM_s VImgParam[MAX_VIEW_NUM] GNU_SECTION_NOZEROINIT;
            RVal |= AmbaImgProc_GetImgParam(ViewID, &VImgParam[ViewID]);
            if(RVal == 0U){
                RVal |= AmbaIK_SetCfaLeakageFilter(mode, &VImgParam[ViewID].CfaLeakageFilterStill);
            }
            RVal |= AmbaIK_SetColorCorrection(mode, &StillPipeCtrl->ColorCorr);
            if(RVal != 0U){
                /* MisraC */
            }
        }
#else
        if (StillPipeCtrl->ColorCorrUpdate == UPDATE_FLAG_ON) {

            static AMBA_IK_COLOR_CORRECTION_REG_s ColorCorrRegCur[MAX_VIEW_NUM] GNU_SECTION_NOZEROINIT;
            ULONG                         CcRegAdd;
            const ULONG                   *pCcReg = NULL;
            static IMG_PARAM_s VImgParam[MAX_VIEW_NUM] GNU_SECTION_NOZEROINIT;
            RVal |= AmbaImgProc_GetImgParam(ViewID, &VImgParam[ViewID]);
            if(RVal == 0U){
                RVal |= AmbaIK_SetCfaLeakageFilter(mode, &VImgParam[ViewID].CfaLeakageFilterStill);
            }
            RVal |= AmbaImgProc_GetCcRegAddr(ViewID, &CcRegAdd);
            RVal |= AmbaWrap_memcpy(&pCcReg, &CcRegAdd, sizeof(ULONG));
            RVal |= AmbaWrap_memcpy(ColorCorrRegCur[ViewID].RegSettingTable,pCcReg,sizeof(AMBA_IK_COLOR_CORRECTION_REG_s));
            RVal |= AmbaIK_SetColorCorrectionReg(mode, &ColorCorrRegCur[ViewID]);
            RVal |= AmbaIK_SetColorCorrection(mode, &StillPipeCtrl->ColorCorr);
            if(RVal != 0U){
                /* MisraC */
            }
        }
#endif
        if (StillPipeCtrl->GammaUpdate == UPDATE_FLAG_ON){
            RVal |= AmbaIK_SetToneCurve(mode, &StillPipeCtrl->GammaTable);
        }

        if (StillPipeCtrl->ChromaScaleUpdate == UPDATE_FLAG_ON){
            RVal |= AmbaIK_SetChromaScale(mode, &StillPipeCtrl->ChromaScale);
        }

        if (StillPipeCtrl->ChromaMedianUpdate == UPDATE_FLAG_ON){
            RVal |= AmbaIK_SetChromaMedianFilter(mode, &StillPipeCtrl->ChromaMedian);
        }
    return RVal;
}

#endif

//End of Complexity
UINT32 Amba_Adj_SetStillPipeCtrlParams(UINT32 ViewID, UINT32 AEBIndex, const AMBA_IK_MODE_CFG_s *mode)
{
    UINT32                 RVal = 0U;
#if defined(CONFIG_SOC_CV2FS) || defined(CONFIG_SOC_CV22FS)//  || defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)
    if(ViewID == 255U){
        /* MisraC */
    }
    if(AEBIndex == 255U){
        /* MisraC */
    }
    if(mode == NULL){
        /* MisraC */
    }
#else
    AMBA_IK_MODE_CFG_s     IKMode;
    UINT32                 HdrEnable = 0;
    ULONG                  Addr;
    const PIPELINE_STILL_CONTROL_s     *StillPipeCtrl;

    if(mode == NULL){
        RVal |= AmbaWrap_memset(&IKMode, 0, sizeof(IKMode));
    }else{
        RVal |= AmbaWrap_memcpy(&IKMode, mode, sizeof(IKMode));
    }

    RVal |= AmbaImgProc_ADJGetStillAddr(ViewID, &Addr);
    RVal |= AmbaImgProc_HDRGetVideoMode(ViewID, &HdrEnable);
    RVal |= AmbaWrap_memcpy(&StillPipeCtrl,&Addr,sizeof(ULONG));

    if(RVal == 0U){
    /* Still HDR filter */
    /*Set VinToneCurve and FrontEndToneCurve*/

        if (StillPipeCtrl->HDSToneCurveUpdate == 1U) {
            // This is in VIN Block, so should be controled by imageFramewrok or scheduler
        }
        if (StillPipeCtrl->FEToneCurveUpdate == 1U) {
            RVal |= AmbaIK_SetFeToneCurve(&IKMode, &StillPipeCtrl->FEToneCurve);
        }

        if (StillPipeCtrl->HdrBlendUpdate[AEBIndex] == 1U) {
            RVal |= AmbaIK_SetHdrBlend(&IKMode, &StillPipeCtrl->HdrBlend[AEBIndex]);
        }

        if (StillPipeCtrl->HdrBlackCorr0Update == 1U) {
            RVal |= AmbaIK_SetFeStaticBlc(&IKMode, &StillPipeCtrl->HdrBlackCorr0, 0);
        }
        if (StillPipeCtrl->HdrBlackCorr1Update == 1U) {
            RVal |= AmbaIK_SetFeStaticBlc(&IKMode, &StillPipeCtrl->HdrBlackCorr1, 1);
        }
        if (StillPipeCtrl->HdrBlackCorr2Update == 1U) {
            RVal |= AmbaIK_SetFeStaticBlc(&IKMode, &StillPipeCtrl->HdrBlackCorr2, 2);
        }

        if (StillPipeCtrl->CEInfoUpdate == 1U) {
            RVal |= AmbaIK_SetCe(&IKMode, &StillPipeCtrl->CEInfo);
        }

        if (StillPipeCtrl->CEInputTableUpdate == 1U) {
            RVal |= AmbaIK_SetCeInputTable(&IKMode, &StillPipeCtrl->CEInputTable);
        }

        if (StillPipeCtrl->CEOutputTableUpdate == 1U) {
            RVal |= AmbaIK_SetCeOutputTable(&IKMode, &StillPipeCtrl->CEOutputTable);
        }
        /*End of Still HDR filter */
        RVal |= AdjSetSPipeCParams_Sub1(ViewID, StillPipeCtrl, &IKMode); //1
        RVal |= AdjSetSPipeCParams_Sub2(StillPipeCtrl, &IKMode); //2
        RVal |= AdjSetSPipeCParams_Sub3(StillPipeCtrl, &IKMode); //3
        RVal |= AdjSetSPipeCParams_Sub4(StillPipeCtrl, &IKMode); //4

        /* Start of HISO filter */
        RVal |= AdjSetSPipeCParams_HISO1(StillPipeCtrl, &IKMode);
        RVal |= AdjSetSPipeCParams_HISO2(StillPipeCtrl, &IKMode);
        RVal |= AdjSetSPipeCParams_HISO3(StillPipeCtrl, &IKMode);
        RVal |= AdjSetSPipeCParams_HISO4(StillPipeCtrl, &IKMode);
        RVal |= AdjSetSPipeCParams_HISO5(StillPipeCtrl, &IKMode);
        RVal |= AdjSetSPipeCParams_HISO6(StillPipeCtrl, &IKMode);
        RVal |= AdjSetSPipeCParams_HISO7(StillPipeCtrl, &IKMode);
        RVal |= AdjSetSPipeCParams_HISO8(StillPipeCtrl, &IKMode);
    }
#endif

    return RVal;
}


static UINT32 ResetSPipeCtrlFlags(UINT32 ViewID)
{
    UINT32                 RVal = 0U;
#if defined(CONFIG_SOC_CV2FS) || defined(CONFIG_SOC_CV22FS)
    if(ViewID == 255U){
        /* MisraC */
    }
#else
    ULONG                  Addr;
    PIPELINE_STILL_CONTROL_s     *StillPipeCtrl;

    RVal |= AmbaImgProc_ADJGetStillAddr(ViewID, &Addr);
    RVal |= AmbaWrap_memcpy(&StillPipeCtrl,&Addr,sizeof(ULONG));
    if(RVal == 0U){
        StillPipeCtrl->RgbYuvMatrixUpdate= 0;
        StillPipeCtrl->BlackCorrUpdate= 0;
        StillPipeCtrl->AntiAliasingUpdate= 0;
        StillPipeCtrl->BadpixCorrUpdate= 0;
        StillPipeCtrl->CfaFilterUpdate= 0;
        StillPipeCtrl->DemosaicUpdate= 0;
        StillPipeCtrl->ColorCorrUpdate= 0;
        StillPipeCtrl->GammaUpdate= 0;
        StillPipeCtrl->ChromaScaleUpdate= 0;
        StillPipeCtrl->ChromaMedianUpdate= 0;
        StillPipeCtrl->AsfUpdate= 0;
        StillPipeCtrl->SharpenBothUpdate= 0;
        StillPipeCtrl->SharpenNoiseUpdate= 0;
        StillPipeCtrl->SharpenFirUpdate= 0;
        StillPipeCtrl->SharpenCoringUpdate= 0;
        StillPipeCtrl->SharpenCoringIndexScaleUpdate= 0;
        StillPipeCtrl->SharpenMinCoringResultUpdate= 0;
        StillPipeCtrl->SharpenMaxCoringResultUpdate= 0;
        StillPipeCtrl->SharpenScaleCoringUpdate= 0;
        StillPipeCtrl->SharpenBBothUpdate= 0;
        StillPipeCtrl->SharpenBNoiseUpdate= 0;
        StillPipeCtrl->SharpenBFirUpdate= 0;
        StillPipeCtrl->SharpenBCoringUpdate= 0;
        StillPipeCtrl->SharpenBCoringIndexScaleUpdate= 0;
        StillPipeCtrl->SharpenBMinCoringResultUpdate= 0;
        StillPipeCtrl->SharpenBMaxCoringResultUpdate= 0;
        StillPipeCtrl->SharpenBScaleCoringUpdate= 0;
        StillPipeCtrl->ChromaFilterUpdate= 0;
        StillPipeCtrl->GbGrMismatchUpdate= 0;
        StillPipeCtrl->ResamplerStrUpdate= 0;
        StillPipeCtrl->MctfInfoUpdate= 0;
        StillPipeCtrl->LisoProcessSelectUpdate= 0;
#if defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)
        StillPipeCtrl->ColorDeptNRUpdate= 0;
        StillPipeCtrl->WideChromaFilterUpdate= 0;
        StillPipeCtrl->WideChromaFilterCMBUpdate= 0;
#endif
        //Still Hiso
        StillPipeCtrl->HIsoCfaLeakageFilterUpdate= 0;
        StillPipeCtrl->HIsoAntiAliasingUpdate= 0;
        StillPipeCtrl->HIsoBadpixCorrUpdate= 0;
        StillPipeCtrl->HIsoCfaFilterUpdate= 0;
        StillPipeCtrl->HIsoGbGrMismatchUpdate= 0;
        StillPipeCtrl->HIsoDemosaicUpdate= 0;
        StillPipeCtrl->HIsoChromaMedianUpdate= 0;
        StillPipeCtrl->HIsoLi2CfaLeakageFilterUpdate= 0;
        StillPipeCtrl->HIsoLi2AntiAliasingUpdate= 0;
        StillPipeCtrl->HIsoLi2BadpixCorrUpdate= 0;
        StillPipeCtrl->HIsoLi2CfaFilterUpdate= 0;
        StillPipeCtrl->HIsoLi2GbGrMismatchUpdate= 0;
        StillPipeCtrl->HIsoLi2DemosaicUpdate= 0;
        StillPipeCtrl->HIsoSelectUpdate= 0;
        StillPipeCtrl->HIsoAsfUpdate= 0;
        StillPipeCtrl->HIsoHighAsfUpdate= 0;
        StillPipeCtrl->HIsoMedAsfUpdate= 0;
        StillPipeCtrl->HIsoLowAsfUpdate= 0;
        StillPipeCtrl->HIsoMed2AsfUpdate= 0;
        StillPipeCtrl->HIsoLi2AsfUpdate= 0;
        StillPipeCtrl->HIsoHigh2AsfUpdate= 0;
        StillPipeCtrl->HIsoHighSharpenBothUpdate= 0;
        StillPipeCtrl->HIsoHighSharpenNoiseUpdate= 0;
        StillPipeCtrl->HIsoHighSharpenFirUpdate= 0;
        StillPipeCtrl->HIsoHighSharpenCoringUpdate= 0;
        StillPipeCtrl->HIsoHighSharpenCoringIndxScaleUpdate= 0;
        StillPipeCtrl->HIsoHighSharpenMinCoringResultUpdate= 0;
        StillPipeCtrl->HIsoHighSharpenScaleCoringUpdate= 0;
        StillPipeCtrl->HIsoHighSharpenMaxCoringResultUpdate= 0;
        StillPipeCtrl->HIsoMedSharpenBothUpdate= 0;
        StillPipeCtrl->HIsoMedSharpenNoiseUpdate= 0;
        StillPipeCtrl->HIsoMedSharpenFirUpdate= 0;
        StillPipeCtrl->HIsoMedSharpenCoringUpdate= 0;
        StillPipeCtrl->HIsoMedSharpenCoringIndexScaleUpdate= 0;
        StillPipeCtrl->HIsoMedSharpenMinCoringResultUpdate= 0;
        StillPipeCtrl->HIsoMedSharpenScaleCoringUpdate= 0;
        StillPipeCtrl->HIsoMedSharpenMaxCoringResultUpdate= 0;
        StillPipeCtrl->HIsoLiSharpenBBothUpdate= 0;
        StillPipeCtrl->HIsoLiSharpenNoiseUpdate= 0;
        StillPipeCtrl->HIsoLiSharpenFirUpdate= 0;
        StillPipeCtrl->HIsoLiSharpenBCoringUpdate= 0;
        StillPipeCtrl->HIsoLiSharpenCoringIndexScaleUpdate= 0;
        StillPipeCtrl->HIsoLiSharpenMinCoringResultUpdate= 0;
        StillPipeCtrl->HIsoLiSharpenScaleCoringUpdate= 0;
        StillPipeCtrl->HIsoLiSharpenMaxCoringResultUpdate= 0;
        StillPipeCtrl->HIsoLi2SharpenBBothUpdate= 0;
        StillPipeCtrl->HIsoLi2SharpenNoiseUpdate= 0;
        StillPipeCtrl->HIsoLi2SharpenFirUpdate= 0;
        StillPipeCtrl->HIsoLi2SharpenBCoringUpdate= 0;
        StillPipeCtrl->HIsoLi2SharpenCoringIndexScaleUpdate= 0;
        StillPipeCtrl->HIsoLi2SharpenMinCoringResultUpdate= 0;
        StillPipeCtrl->HIsoLi2SharpenScaleCoringUpdate= 0;
        StillPipeCtrl->HIsoLi2SharpenMaxCoringResultUpdate= 0;
        StillPipeCtrl->HIsoLiChromaAsfUpdate= 0;
        StillPipeCtrl->HIsoChromaAsfUpdate= 0;
        StillPipeCtrl->HIsoChromaFilterPreUpdate= 0;
        StillPipeCtrl->HIsoChromaFilterHighUpdate= 0;
        StillPipeCtrl->HIsoChromaFilterMedUpdate= 0;
        StillPipeCtrl->HIsoChromaFilterLowUpdate= 0;
        StillPipeCtrl->HIsoChromaFilterVLowUpdate= 0;
        StillPipeCtrl->HIsoChromaFilterMedCombineUpdate= 0;
        StillPipeCtrl->HIsoChromaFilterLowCombineUpdate= 0;
        StillPipeCtrl->HIsoChromaFilterVLowCombineUpdate= 0;
        StillPipeCtrl->HIsoHiLiCombineUpdate= 0;
        StillPipeCtrl->HIsoLumaFilterCombineUpdate= 0;
        StillPipeCtrl->HIsoLowAsfCombineUpdate= 0;
        StillPipeCtrl->HIsoLiLumaMidHightFreqRcvrUpdate= 0;
        StillPipeCtrl->HIsoLumaBlendUpdate= 0;
        StillPipeCtrl->HIsoNonSmoothUpdate= 0;

        // CEInfo is calculated in ADJ
        StillPipeCtrl->CEInfoUpdate = 0U;
    }
#endif
    return RVal;
}

static UINT32 ResetSHdrPipeCtrlFlags(UINT32 ViewID)
{
    UINT32 RVal = 0U;
#if defined(CONFIG_SOC_CV2FS) || defined(CONFIG_SOC_CV22FS)
    if(ViewID == 255U){
        /* MisraC */
    }
#else
    ULONG  Addr;
    UINT32 i = 0;
    PIPELINE_STILL_CONTROL_s     *StillPipeCtrl;

    RVal |= AmbaImgProc_ADJGetStillAddr(ViewID, &Addr);
    RVal |= AmbaWrap_memcpy(&StillPipeCtrl,&Addr,sizeof(ULONG));
    if(RVal == 0U){
        for (i=0; i<MAX_AEB_NUM; i++) {
            StillPipeCtrl->HdrBlendUpdate[i] = 0U;
        }
        StillPipeCtrl->HdrBlackCorr0Update = 0U;
        StillPipeCtrl->HdrBlackCorr1Update = 0U;
        StillPipeCtrl->HdrBlackCorr2Update = 0U;

        /*Compand & Decompand*/
        StillPipeCtrl->FEToneCurveUpdate = 0U;
        StillPipeCtrl->HDSToneCurveUpdate = 0U;
    }
#endif
    return RVal;
}

static UINT32 ResetSCePipeCtrlFlags(UINT32 ViewID)
{
    UINT32 RVal = 0U;
#if defined(CONFIG_SOC_CV2FS) || defined(CONFIG_SOC_CV22FS)
    if(ViewID == 255U){
        /* MisraC */
    }
#else
    ULONG  Addr;
    PIPELINE_STILL_CONTROL_s     *StillPipeCtrl;

    RVal |= AmbaImgProc_ADJGetStillAddr(ViewID, &Addr);
    RVal |= AmbaWrap_memcpy(&StillPipeCtrl,&Addr,sizeof(ULONG));
    if(RVal == 0U){
        /* Contrast enhancement */
        StillPipeCtrl->CEInputTableUpdate = 0U;
        StillPipeCtrl->CEOutputTableUpdate = 0U;
    }
#endif
    return RVal;
}

UINT32 Amba_Adj_ResetStillFlags(UINT32 ViewID, UINT8 Mode)
{
    UINT32 RVal = 0;

    if(Mode == 0U){ //Normal
        RVal |= ResetSPipeCtrlFlags(ViewID);
    }else if (Mode == 1U){ // HDR
        RVal |= ResetSHdrPipeCtrlFlags(ViewID);
    }else if (Mode == 2U){ //CE
        RVal |= ResetSCePipeCtrlFlags(ViewID);
    }else{
        /* MisraC */
    }

    return RVal;
}

static UINT32 PrtSPipeCtrlFlags(UINT32 ViewID)
{
    UINT32                 RVal = 0U;
#if defined(CONFIG_SOC_CV2FS) || defined(CONFIG_SOC_CV22FS)
    if(ViewID == 255U){
        /* MisraC */
    }
#else
    ULONG                  Addr;
    const PIPELINE_STILL_CONTROL_s     *StillPipeCtrl;

    RVal |= AmbaImgProc_ADJGetStillAddr(ViewID, &Addr);
    RVal |= AmbaWrap_memcpy(&StillPipeCtrl,&Addr,sizeof(ULONG));
    if(RVal == 0U){
        AmbaPrint_PrintUInt5("RgbYuvMatrixUpdate : %d",                      StillPipeCtrl->RgbYuvMatrixUpdate, 0U, 0U, 0U, 0U);
        AmbaPrint_PrintUInt5("BlackCorrUpdate : %d",                         StillPipeCtrl->BlackCorrUpdate, 0U, 0U, 0U, 0U);
        AmbaPrint_PrintUInt5("AntiAliasingUpdate : %d",                      StillPipeCtrl->AntiAliasingUpdate, 0U, 0U, 0U, 0U);
        AmbaPrint_PrintUInt5("BadpixCorrUpdate : %d",                        StillPipeCtrl->BadpixCorrUpdate, 0U, 0U, 0U, 0U);
        AmbaPrint_PrintUInt5("CfaFilterUpdate : %d",                         StillPipeCtrl->CfaFilterUpdate, 0U, 0U, 0U, 0U);
        AmbaPrint_PrintUInt5("DemosaicUpdate : %d",                          StillPipeCtrl->DemosaicUpdate, 0U, 0U, 0U, 0U);
        AmbaPrint_PrintUInt5("ColorCorrUpdate : %d",                         StillPipeCtrl->ColorCorrUpdate, 0U, 0U, 0U, 0U);
        AmbaPrint_PrintUInt5("GammaUpdate : %d",                             StillPipeCtrl->GammaUpdate, 0U, 0U, 0U, 0U);
        AmbaPrint_PrintUInt5("ChromaScaleUpdate : %d",                       StillPipeCtrl->ChromaScaleUpdate, 0U, 0U, 0U, 0U);
        AmbaPrint_PrintUInt5("ChromaMedianUpdate : %d",                      StillPipeCtrl->ChromaMedianUpdate, 0U, 0U, 0U, 0U);
        AmbaPrint_PrintUInt5("AsfUpdate : %d",                               StillPipeCtrl->AsfUpdate, 0U, 0U, 0U, 0U);
        AmbaPrint_PrintUInt5("SharpenBothUpdate : %d",                       StillPipeCtrl->SharpenBothUpdate, 0U, 0U, 0U, 0U);
        AmbaPrint_PrintUInt5("SharpenNoiseUpdate : %d",                      StillPipeCtrl->SharpenNoiseUpdate, 0U, 0U, 0U, 0U);
        AmbaPrint_PrintUInt5("SharpenFirUpdate : %d",                        StillPipeCtrl->SharpenFirUpdate, 0U, 0U, 0U, 0U);
        AmbaPrint_PrintUInt5("SharpenCoringUpdate : %d",                     StillPipeCtrl->SharpenCoringUpdate, 0U, 0U, 0U, 0U);
        AmbaPrint_PrintUInt5("SharpenCoringIndexScaleUpdate : %d",           StillPipeCtrl->SharpenCoringIndexScaleUpdate, 0U, 0U, 0U, 0U);
        AmbaPrint_PrintUInt5("SharpenMinCoringResultUpdate : %d",            StillPipeCtrl->SharpenMinCoringResultUpdate, 0U, 0U, 0U, 0U);
        AmbaPrint_PrintUInt5("SharpenMaxCoringResultUpdate : %d",            StillPipeCtrl->SharpenMaxCoringResultUpdate, 0U, 0U, 0U, 0U);
        AmbaPrint_PrintUInt5("SharpenScaleCoringUpdate : %d",                StillPipeCtrl->SharpenScaleCoringUpdate, 0U, 0U, 0U, 0U);
        AmbaPrint_PrintUInt5("SharpenBBothUpdate : %d",                      StillPipeCtrl->SharpenBBothUpdate, 0U, 0U, 0U, 0U);
        AmbaPrint_PrintUInt5("SharpenBNoiseUpdate : %d",                     StillPipeCtrl->SharpenBNoiseUpdate, 0U, 0U, 0U, 0U);
        AmbaPrint_PrintUInt5("SharpenBFirUpdate : %d",                       StillPipeCtrl->SharpenBFirUpdate, 0U, 0U, 0U, 0U);
        AmbaPrint_PrintUInt5("SharpenBCoringUpdate : %d",                    StillPipeCtrl->SharpenBCoringUpdate, 0U, 0U, 0U, 0U);
        AmbaPrint_PrintUInt5("SharpenBCoringIndexScaleUpdate : %d",          StillPipeCtrl->SharpenBCoringIndexScaleUpdate, 0U, 0U, 0U, 0U);
        AmbaPrint_PrintUInt5("SharpenBMinCoringResultUpdate : %d",           StillPipeCtrl->SharpenBMinCoringResultUpdate, 0U, 0U, 0U, 0U);
        AmbaPrint_PrintUInt5("SharpenBMaxCoringResultUpdate : %d",           StillPipeCtrl->SharpenBMaxCoringResultUpdate, 0U, 0U, 0U, 0U);
        AmbaPrint_PrintUInt5("SharpenBScaleCoringUpdate : %d",               StillPipeCtrl->SharpenBScaleCoringUpdate, 0U, 0U, 0U, 0U);
        AmbaPrint_PrintUInt5("ChromaFilterUpdate : %d",                      StillPipeCtrl->ChromaFilterUpdate, 0U, 0U, 0U, 0U);
        AmbaPrint_PrintUInt5("GbGrMismatchUpdate : %d",                      StillPipeCtrl->GbGrMismatchUpdate, 0U, 0U, 0U, 0U);
        AmbaPrint_PrintUInt5("ResamplerStrUpdate : %d",                      StillPipeCtrl->ResamplerStrUpdate, 0U, 0U, 0U, 0U);
        AmbaPrint_PrintUInt5("MctfInfoUpdate : %d",                          StillPipeCtrl->MctfInfoUpdate, 0U, 0U, 0U, 0U);
        AmbaPrint_PrintUInt5("LisoProcessSelectUpdate : %d",                 StillPipeCtrl->LisoProcessSelectUpdate, 0U, 0U, 0U, 0U);
#if defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)
        AmbaPrint_PrintUInt5("ColorDeptNRUpdate : %d",                          StillPipeCtrl->ColorDeptNRUpdate, 0U, 0U, 0U, 0U);
        AmbaPrint_PrintUInt5("WideChromaFilterUpdate : %d",                  StillPipeCtrl->WideChromaFilterUpdate, 0U, 0U, 0U, 0U);
        AmbaPrint_PrintUInt5("WideChromaFilterCMBUpdate : %d",                  StillPipeCtrl->WideChromaFilterCMBUpdate, 0U, 0U, 0U, 0U);
#endif
        //Still Hiso
        AmbaPrint_PrintUInt5("HIsoCfaLeakageFilterUpdate : %d",                      StillPipeCtrl->HIsoCfaLeakageFilterUpdate, 0U, 0U, 0U, 0U);
        AmbaPrint_PrintUInt5("HIsoAntiAliasingUpdate : %d",                          StillPipeCtrl->HIsoAntiAliasingUpdate, 0U, 0U, 0U, 0U);
        AmbaPrint_PrintUInt5("HIsoBadpixCorrUpdate : %d",                            StillPipeCtrl->HIsoBadpixCorrUpdate, 0U, 0U, 0U, 0U);
        AmbaPrint_PrintUInt5("HIsoCfaFilterUpdate : %d",                             StillPipeCtrl->HIsoCfaFilterUpdate, 0U, 0U, 0U, 0U);
        AmbaPrint_PrintUInt5("HIsoGbGrMismatchUpdate : %d",                          StillPipeCtrl->HIsoGbGrMismatchUpdate, 0U, 0U, 0U, 0U);
        AmbaPrint_PrintUInt5("HIsoDemosaicUpdate : %d",                              StillPipeCtrl->HIsoDemosaicUpdate, 0U, 0U, 0U, 0U);
        AmbaPrint_PrintUInt5("HIsoChromaMedianUpdate : %d",                          StillPipeCtrl->HIsoChromaMedianUpdate, 0U, 0U, 0U, 0U);
        AmbaPrint_PrintUInt5("HIsoLi2CfaLeakageFilterUpdate : %d",                   StillPipeCtrl->HIsoLi2CfaLeakageFilterUpdate, 0U, 0U, 0U, 0U);
        AmbaPrint_PrintUInt5("HIsoLi2AntiAliasingUpdate : %d",                       StillPipeCtrl->HIsoLi2AntiAliasingUpdate, 0U, 0U, 0U, 0U);
        AmbaPrint_PrintUInt5("HIsoLi2BadpixCorrUpdate : %d",                         StillPipeCtrl->HIsoLi2BadpixCorrUpdate, 0U, 0U, 0U, 0U);
        AmbaPrint_PrintUInt5("HIsoLi2CfaFilterUpdate : %d",                          StillPipeCtrl->HIsoLi2CfaFilterUpdate, 0U, 0U, 0U, 0U);
        AmbaPrint_PrintUInt5("HIsoLi2GbGrMismatchUpdate : %d",                       StillPipeCtrl->HIsoLi2GbGrMismatchUpdate, 0U, 0U, 0U, 0U);
        AmbaPrint_PrintUInt5("HIsoLi2DemosaicUpdate : %d",                           StillPipeCtrl->HIsoLi2DemosaicUpdate, 0U, 0U, 0U, 0U);
        AmbaPrint_PrintUInt5("HIsoSelectUpdate : %d",                                StillPipeCtrl->HIsoSelectUpdate, 0U, 0U, 0U, 0U);
        AmbaPrint_PrintUInt5("HIsoAsfUpdate : %d",                                   StillPipeCtrl->HIsoAsfUpdate, 0U, 0U, 0U, 0U);
        AmbaPrint_PrintUInt5("HIsoHighAsfUpdate : %d",                               StillPipeCtrl->HIsoHighAsfUpdate, 0U, 0U, 0U, 0U);
        AmbaPrint_PrintUInt5("HIsoMedAsfUpdate : %d",                                StillPipeCtrl->HIsoMedAsfUpdate, 0U, 0U, 0U, 0U);
        AmbaPrint_PrintUInt5("HIsoLowAsfUpdate : %d",                                StillPipeCtrl->HIsoLowAsfUpdate, 0U, 0U, 0U, 0U);
        AmbaPrint_PrintUInt5("HIsoMed2AsfUpdate : %d",                               StillPipeCtrl->HIsoMed2AsfUpdate, 0U, 0U, 0U, 0U);
        AmbaPrint_PrintUInt5("HIsoLi2AsfUpdate : %d",                                StillPipeCtrl->HIsoLi2AsfUpdate, 0U, 0U, 0U, 0U);
        AmbaPrint_PrintUInt5("HIsoHigh2AsfUpdate : %d",                              StillPipeCtrl->HIsoHigh2AsfUpdate, 0U, 0U, 0U, 0U);
        AmbaPrint_PrintUInt5("HIsoHighSharpenBothUpdate : %d",                       StillPipeCtrl->HIsoHighSharpenBothUpdate, 0U, 0U, 0U, 0U);
        AmbaPrint_PrintUInt5("HIsoHighSharpenNoiseUpdate : %d",                      StillPipeCtrl->HIsoHighSharpenNoiseUpdate, 0U, 0U, 0U, 0U);
        AmbaPrint_PrintUInt5("HIsoHighSharpenFirUpdate : %d",                        StillPipeCtrl->HIsoHighSharpenFirUpdate, 0U, 0U, 0U, 0U);
        AmbaPrint_PrintUInt5("HIsoHighSharpenCoringUpdate : %d",                     StillPipeCtrl->HIsoHighSharpenCoringUpdate, 0U, 0U, 0U, 0U);
        AmbaPrint_PrintUInt5("HIsoHighSharpenCoringIndxScaleUpdate : %d",            StillPipeCtrl->HIsoHighSharpenCoringIndxScaleUpdate, 0U, 0U, 0U, 0U);
        AmbaPrint_PrintUInt5("HIsoHighSharpenMinCoringResultUpdate : %d",            StillPipeCtrl->HIsoHighSharpenMinCoringResultUpdate, 0U, 0U, 0U, 0U);
        AmbaPrint_PrintUInt5("HIsoHighSharpenScaleCoringUpdate : %d",                StillPipeCtrl->HIsoHighSharpenScaleCoringUpdate, 0U, 0U, 0U, 0U);
        AmbaPrint_PrintUInt5("HIsoHighSharpenMaxCoringResultUpdate : %d",            StillPipeCtrl->HIsoHighSharpenMaxCoringResultUpdate, 0U, 0U, 0U, 0U);
        AmbaPrint_PrintUInt5("HIsoMedSharpenBothUpdate : %d",                        StillPipeCtrl->HIsoMedSharpenBothUpdate, 0U, 0U, 0U, 0U);
        AmbaPrint_PrintUInt5("HIsoMedSharpenNoiseUpdate : %d",                       StillPipeCtrl->HIsoMedSharpenNoiseUpdate, 0U, 0U, 0U, 0U);
        AmbaPrint_PrintUInt5("HIsoMedSharpenFirUpdate : %d",                         StillPipeCtrl->HIsoMedSharpenFirUpdate, 0U, 0U, 0U, 0U);
        AmbaPrint_PrintUInt5("HIsoMedSharpenCoringUpdate : %d",                      StillPipeCtrl->HIsoMedSharpenCoringUpdate, 0U, 0U, 0U, 0U);
        AmbaPrint_PrintUInt5("HIsoMedSharpenCoringIndexScaleUpdate : %d",            StillPipeCtrl->HIsoMedSharpenCoringIndexScaleUpdate, 0U, 0U, 0U, 0U);
        AmbaPrint_PrintUInt5("HIsoMedSharpenMinCoringResultUpdate : %d",             StillPipeCtrl->HIsoMedSharpenMinCoringResultUpdate, 0U, 0U, 0U, 0U);
        AmbaPrint_PrintUInt5("HIsoMedSharpenScaleCoringUpdate : %d",                 StillPipeCtrl->HIsoMedSharpenScaleCoringUpdate, 0U, 0U, 0U, 0U);
        AmbaPrint_PrintUInt5("HIsoMedSharpenMaxCoringResultUpdate : %d",             StillPipeCtrl->HIsoMedSharpenMaxCoringResultUpdate, 0U, 0U, 0U, 0U);
        AmbaPrint_PrintUInt5("HIsoLiSharpenBBothUpdate : %d",                        StillPipeCtrl->HIsoLiSharpenBBothUpdate, 0U, 0U, 0U, 0U);
        AmbaPrint_PrintUInt5("HIsoLiSharpenNoiseUpdate : %d",                        StillPipeCtrl->HIsoLiSharpenNoiseUpdate, 0U, 0U, 0U, 0U);
        AmbaPrint_PrintUInt5("HIsoLiSharpenFirUpdate : %d",                          StillPipeCtrl->HIsoLiSharpenFirUpdate, 0U, 0U, 0U, 0U);
        AmbaPrint_PrintUInt5("HIsoLiSharpenBCoringUpdate : %d",                      StillPipeCtrl->HIsoLiSharpenBCoringUpdate, 0U, 0U, 0U, 0U);
        AmbaPrint_PrintUInt5("HIsoLiSharpenCoringIndexScaleUpdate : %d",             StillPipeCtrl->HIsoLiSharpenCoringIndexScaleUpdate, 0U, 0U, 0U, 0U);
        AmbaPrint_PrintUInt5("HIsoLiSharpenMinCoringResultUpdate : %d",              StillPipeCtrl->HIsoLiSharpenMinCoringResultUpdate, 0U, 0U, 0U, 0U);
        AmbaPrint_PrintUInt5("HIsoLiSharpenScaleCoringUpdate : %d",                  StillPipeCtrl->HIsoLiSharpenScaleCoringUpdate, 0U, 0U, 0U, 0U);
        AmbaPrint_PrintUInt5("HIsoLiSharpenMaxCoringResultUpdate : %d",              StillPipeCtrl->HIsoLiSharpenMaxCoringResultUpdate, 0U, 0U, 0U, 0U);
        AmbaPrint_PrintUInt5("HIsoLi2SharpenBBothUpdate : %d",                       StillPipeCtrl->HIsoLi2SharpenBBothUpdate, 0U, 0U, 0U, 0U);
        AmbaPrint_PrintUInt5("HIsoLi2SharpenNoiseUpdate : %d",                       StillPipeCtrl->HIsoLi2SharpenNoiseUpdate, 0U, 0U, 0U, 0U);
        AmbaPrint_PrintUInt5("HIsoLi2SharpenFirUpdate : %d",                         StillPipeCtrl->HIsoLi2SharpenFirUpdate, 0U, 0U, 0U, 0U);
        AmbaPrint_PrintUInt5("HIsoLi2SharpenBCoringUpdate : %d",                     StillPipeCtrl->HIsoLi2SharpenBCoringUpdate, 0U, 0U, 0U, 0U);
        AmbaPrint_PrintUInt5("HIsoLi2SharpenCoringIndexScaleUpdate : %d",            StillPipeCtrl->HIsoLi2SharpenCoringIndexScaleUpdate, 0U, 0U, 0U, 0U);
        AmbaPrint_PrintUInt5("HIsoLi2SharpenMinCoringResultUpdate : %d",             StillPipeCtrl->HIsoLi2SharpenMinCoringResultUpdate, 0U, 0U, 0U, 0U);
        AmbaPrint_PrintUInt5("HIsoLi2SharpenScaleCoringUpdate : %d",                 StillPipeCtrl->HIsoLi2SharpenScaleCoringUpdate, 0U, 0U, 0U, 0U);
        AmbaPrint_PrintUInt5("HIsoLi2SharpenMaxCoringResultUpdate : %d",             StillPipeCtrl->HIsoLi2SharpenMaxCoringResultUpdate, 0U, 0U, 0U, 0U);
        AmbaPrint_PrintUInt5("HIsoLiChromaAsfUpdate : %d",                           StillPipeCtrl->HIsoLiChromaAsfUpdate, 0U, 0U, 0U, 0U);
        AmbaPrint_PrintUInt5("HIsoChromaAsfUpdate : %d",                             StillPipeCtrl->HIsoChromaAsfUpdate, 0U, 0U, 0U, 0U);
        AmbaPrint_PrintUInt5("HIsoChromaFilterPreUpdate : %d",                       StillPipeCtrl->HIsoChromaFilterPreUpdate, 0U, 0U, 0U, 0U);
        AmbaPrint_PrintUInt5("HIsoChromaFilterHighUpdate : %d",                      StillPipeCtrl->HIsoChromaFilterHighUpdate, 0U, 0U, 0U, 0U);
        AmbaPrint_PrintUInt5("HIsoChromaFilterMedUpdate : %d",                       StillPipeCtrl->HIsoChromaFilterMedUpdate, 0U, 0U, 0U, 0U);
        AmbaPrint_PrintUInt5("HIsoChromaFilterLowUpdate : %d",                       StillPipeCtrl->HIsoChromaFilterLowUpdate, 0U, 0U, 0U, 0U);
        AmbaPrint_PrintUInt5("HIsoChromaFilterVLowUpdate : %d",                      StillPipeCtrl->HIsoChromaFilterVLowUpdate, 0U, 0U, 0U, 0U);
        AmbaPrint_PrintUInt5("HIsoChromaFilterMedCombineUpdate : %d",                StillPipeCtrl->HIsoChromaFilterMedCombineUpdate, 0U, 0U, 0U, 0U);
        AmbaPrint_PrintUInt5("HIsoChromaFilterLowCombineUpdate : %d",                StillPipeCtrl->HIsoChromaFilterLowCombineUpdate, 0U, 0U, 0U, 0U);
        AmbaPrint_PrintUInt5("HIsoChromaFilterVLowCombineUpdate : %d",               StillPipeCtrl->HIsoChromaFilterVLowCombineUpdate, 0U, 0U, 0U, 0U);
        AmbaPrint_PrintUInt5("HIsoHiLiCombineUpdate : %d",                           StillPipeCtrl->HIsoHiLiCombineUpdate, 0U, 0U, 0U, 0U);
        AmbaPrint_PrintUInt5("HIsoLumaFilterCombineUpdate : %d",                     StillPipeCtrl->HIsoLumaFilterCombineUpdate, 0U, 0U, 0U, 0U);
        AmbaPrint_PrintUInt5("HIsoLowAsfCombineUpdate : %d",                         StillPipeCtrl->HIsoLowAsfCombineUpdate, 0U, 0U, 0U, 0U);
        AmbaPrint_PrintUInt5("HIsoLiLumaMidHightFreqRcvrUpdate : %d",                StillPipeCtrl->HIsoLiLumaMidHightFreqRcvrUpdate, 0U, 0U, 0U, 0U);
        AmbaPrint_PrintUInt5("HIsoLumaBlendUpdate : %d",                             StillPipeCtrl->HIsoLumaBlendUpdate, 0U, 0U, 0U, 0U);
        AmbaPrint_PrintUInt5("HIsoNonSmoothUpdate : %d",                              StillPipeCtrl->HIsoNonSmoothUpdate, 0U, 0U, 0U, 0U);

        // CEInfo is calculated in ADJ
        AmbaPrint_PrintUInt5("CEInfoUpdate : %d",                                    StillPipeCtrl->CEInfoUpdate, 0U, 0U, 0U, 0U);
    }
#endif
    return RVal;
}

static UINT32 PrtSHdrPipeCtrlFlags(UINT32 ViewID)
{
    UINT32 RVal = 0U;
#if defined(CONFIG_SOC_CV2FS) || defined(CONFIG_SOC_CV22FS)
    if(ViewID == 255U){
        /* MisraC */
    }
#else
    ULONG  Addr;
    UINT32 i = 0;
    const PIPELINE_STILL_CONTROL_s     *StillPipeCtrl;

    RVal |= AmbaImgProc_ADJGetStillAddr(ViewID, &Addr);
    RVal |= AmbaWrap_memcpy(&StillPipeCtrl,&Addr,sizeof(ULONG));
    if(RVal == 0U){
        for (i=0; i<MAX_AEB_NUM; i++) {
            AmbaPrint_PrintUInt5("HdrBlendUpdate : %d, i %d", StillPipeCtrl->HdrBlendUpdate[i], i, 0U, 0U, 0U);
        }
        AmbaPrint_PrintUInt5("HdrBlackCorr0Update : %d", StillPipeCtrl->HdrBlackCorr0Update, 0U, 0U, 0U, 0U);
        AmbaPrint_PrintUInt5("HdrBlackCorr1Update : %d", StillPipeCtrl->HdrBlackCorr1Update, 0U, 0U, 0U, 0U);
        AmbaPrint_PrintUInt5("HdrBlackCorr2Update : %d", StillPipeCtrl->HdrBlackCorr2Update, 0U, 0U, 0U, 0U);

        /*Compand & Decompand*/
        AmbaPrint_PrintUInt5("FEToneCurveUpdate : %d", StillPipeCtrl->FEToneCurveUpdate, 0U, 0U, 0U, 0U);
        AmbaPrint_PrintUInt5("HDSToneCurveUpdate : %d", StillPipeCtrl->HDSToneCurveUpdate, 0U, 0U, 0U, 0U);
    }
#endif
    return RVal;
}

static UINT32 PrtSCePipeCtrlFlags(UINT32 ViewID)
{
    UINT32 RVal = 0U;
#if defined(CONFIG_SOC_CV2FS) || defined(CONFIG_SOC_CV22FS)
    if(ViewID == 255U){
        /* MisraC */
    }
#else
    ULONG  Addr;
    const PIPELINE_STILL_CONTROL_s     *StillPipeCtrl;

    RVal |= AmbaImgProc_ADJGetStillAddr(ViewID, &Addr);
    RVal |= AmbaWrap_memcpy(&StillPipeCtrl,&Addr,sizeof(ULONG));
    AmbaPrint_PrintUInt5("Start PrtSCePipeCtrlFlags : %d, RVal : %d", ViewID, RVal, 0U, 0U, 0U);
    if(RVal == 0U){
        /* Contrast enhancement */
        AmbaPrint_PrintUInt5("CEInputTableUpdate : %d", StillPipeCtrl->CEInputTableUpdate, 0U, 0U, 0U, 0U);
        AmbaPrint_PrintUInt5("CEOutputTableUpdate : %d", StillPipeCtrl->CEOutputTableUpdate, 0U, 0U, 0U, 0U);
    }
#endif
    return RVal;
}

UINT32 Amba_Adj_PrtStillFlags(UINT32 ViewID)
{
    UINT32 RVal = 0;

    RVal |= PrtSPipeCtrlFlags(ViewID);
    RVal |= PrtSHdrPipeCtrlFlags(ViewID);
    RVal |= PrtSCePipeCtrlFlags(ViewID);
    return RVal;
}
