/**
 * @file Amba_Image.c
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
 *  @Description    :: app init parameters function
 *
 *
 */

#include "AmbaTypes.h"
#include "AmbaPrint.h"
#if defined(CONFIG_QNX) || defined(CONFIG_LINUX)
#include "AmbaWrap.h"
#endif
#include "AmbaDSP_ImageFilter.h"
#include "AmbaImg_Proc.h"
#include "AmbaImg_AeAwb.h"
#include "Amba_AeAwbAdj_Control.h"

#include "AmbaIQParamHandlerSample.h"
#include "Amba_Image.h"
#include "AmbaSensor.h"
#include "AmbaImg_External_CtrlFunc.h"

//#define HDR_CMP_DCMP_SAMPLE

#if !defined(CONFIG_SOC_CV2FS) && !defined(CONFIG_SOC_CV22FS)&& !defined(CONFIG_SOC_CV5)&& !defined(CONFIG_SOC_CV52)
/*
static UINT32 PointerToUINT32(const void * ptr){
    UINT32 addr = 0;
    (void)AmbaWrap_memcpy(&addr, &ptr, sizeof(UINT32));
    return addr;
}
*/
static ULONG SPointerToULONG(const void * ptr){
    ULONG addr = 0;
    UINT32 rVal = 0;

    rVal |=  AmbaWrap_memcpy(&addr, &ptr, sizeof(ULONG));
    if(rVal != 0U){
        /* MisraC */
    }
    return addr;
}

#endif

static UINT32 tableMap[MAX_VIEW_NUM][IMGPROC_PARTIAL_LOAD_SECTION_NUM] = {{0U, 0U, 0U}, {0U, 0U, 0U}, {0U, 0U, 0U}, {0U, 0U, 0U},
                                                                          {0U, 0U, 0U}, {0U, 0U, 0U}, {0U, 0U, 0U}, {0U, 0U, 0U},
                                                                          {0U, 0U, 0U}, {0U, 0U, 0U}, {0U, 0U, 0U}, {0U, 0U, 0U},
                                                                          {0U, 0U, 0U}, {0U, 0U, 0U}, {0U, 0U, 0U}, {0U, 0U, 0U}};

static UINT8 AAAParamsLoadFlg[MAX_VIEW_NUM][AAA_TABLE_MAX_NO] = {{0U, 0U}, {0U, 0U}, {0U, 0U}, {0U, 0U},
                                                                 {0U, 0U}, {0U, 0U}, {0U, 0U}, {0U, 0U},
                                                                 {0U, 0U}, {0U, 0U}, {0U, 0U}, {0U, 0U},
                                                                 {0U, 0U}, {0U, 0U}, {0U, 0U}, {0U, 0U}};

static UINT32 GetLoadForbidFlag(UINT8 section, IQ_PARAM_LOAD_FORBID_FLAG *pLoadForbidFlag)
{
    IQ_PARAM_LOAD_FORBID_FLAG       loadForbidFlagAll = {
                                                     .ImgParam =            0,
                                                     .AAAParams =           {0, 0},
                                                     .videoParams =         {0, 0, 0},
                                                     .videoMSM =            {0},
                                                     .videoMSH =            {0},
                                                     .photoPreviewParams =  {0, 0},
                                                     .stillLISOParams =     {0, 0},
                                                     .stillHISOParams =     {0},
                                                     .stillIdxInfoParams =  0,
                                                     .sceneParams =         {0, 0, 0, 0, 0},
                                                     .DEVideoParams =       {0},
                                                     .DEStillParams =       {0},
                                                    };
    IQ_PARAM_LOAD_FORBID_FLAG       loadForbidFlagVideo = {
                                                     .ImgParam =            0,
                                                     .AAAParams =           {0, 1},
                                                     .videoParams =         {0, 1, 1},
                                                     .videoMSM =            {0},
                                                     .videoMSH =            {0},
                                                     .photoPreviewParams =  {1, 1},
                                                     .stillLISOParams =     {1, 1},
                                                     .stillHISOParams =     {1},
                                                     .stillIdxInfoParams =  0,
                                                     .sceneParams =         {0, 1, 1, 1, 1},
                                                     .DEVideoParams =       {0},
                                                     .DEStillParams =       {1},
                                                    };
    IQ_PARAM_LOAD_FORBID_FLAG       loadForbidFlagStill = {
                                                     .ImgParam =            1,
                                                     .AAAParams =           {1, 0},
                                                     .videoParams =         {1, 0, 0},
                                                     .videoMSM =            {1},
                                                     .videoMSH =            {1},
                                                     .photoPreviewParams =  {0, 0},
                                                     .stillLISOParams =     {0, 0},
                                                     .stillHISOParams =     {0},
                                                     .stillIdxInfoParams =  0,
                                                     .sceneParams =         {1, 0, 0, 0, 0},
                                                     .DEVideoParams =       {1},
                                                     .DEStillParams =       {0},
                                                    };
    UINT32 rVal = 0U;


    if (section == IMGPROC_PARTIAL_LOAD_VIDEO_PART) {
        rVal |= AmbaWrap_memcpy(pLoadForbidFlag, &loadForbidFlagVideo, sizeof(IQ_PARAM_LOAD_FORBID_FLAG));
    } else if (section == IMGPROC_PARTIAL_LOAD_STILL_PART) {
        rVal |= AmbaWrap_memcpy(pLoadForbidFlag, &loadForbidFlagStill, sizeof(IQ_PARAM_LOAD_FORBID_FLAG));
    } else {
        rVal |= AmbaWrap_memcpy(pLoadForbidFlag, &loadForbidFlagAll, sizeof(IQ_PARAM_LOAD_FORBID_FLAG));
    }
    if(rVal != 0U){
        /* MisraC */
    }
    return rVal;
}

static UINT32 GetCCLoadForbidFlag(UINT32 viewID, UINT8 section, IQ_PARAM_CC_LOAD_FORBID_FLAG *pCCLoadForbidFlag)
{
    UINT32                             rVal = 0U;
    IQ_PARAM_s                         *pIQParams = NULL;


    rVal |= AmbaWrap_memset(pCCLoadForbidFlag, 0, sizeof(IQ_PARAM_CC_LOAD_FORBID_FLAG));

    if (section == IMGPROC_PARTIAL_LOAD_All) {
        //Pass
    } else {
        rVal |= Amba_IQParam_Get_IqParams(viewID, &pIQParams);
        if (section == IMGPROC_PARTIAL_LOAD_VIDEO_PART) {
            // Use videoParams[0] because this is the default table for section == IMGPROC_PARTIAL_LOAD_VIDEO_PART
            if (pIQParams->videoParams[0].FilterParam.Def.Color.Type == IMG_MODE_TV) {
                //Load VideoCC
                pCCLoadForbidFlag->stillCC[0] = 1U;
                pCCLoadForbidFlag->stillCC[1] = 1U;
                pCCLoadForbidFlag->stillCC[2] = 1U;
                pCCLoadForbidFlag->stillCC[3] = 1U;
                pCCLoadForbidFlag->stillCC[4] = 1U;
            } else if (pIQParams->videoParams[0].FilterParam.Def.Color.Type == IMG_MODE_PC) {
                //Load StillCC
                pCCLoadForbidFlag->videoCC[0] = 1U;
                pCCLoadForbidFlag->videoCC[1] = 1U;
                pCCLoadForbidFlag->videoCC[2] = 1U;
                pCCLoadForbidFlag->videoCC[3] = 1U;
                pCCLoadForbidFlag->videoCC[4] = 1U;
            } else {
                //PASS
            }

        } else if (section == IMGPROC_PARTIAL_LOAD_STILL_PART) {
            //When section == IMGPROC_PARTIAL_LOAD_STILL, the rest of the CC files should be loaded.
            if (pIQParams->videoParams[0].FilterParam.Def.Color.Type == IMG_MODE_TV) {
                //Load StillCC
                pCCLoadForbidFlag->videoCC[0] = 1U;
                pCCLoadForbidFlag->videoCC[1] = 1U;
                pCCLoadForbidFlag->videoCC[2] = 1U;
                pCCLoadForbidFlag->videoCC[3] = 1U;
                pCCLoadForbidFlag->videoCC[4] = 1U;
            } else if (pIQParams->videoParams[0].FilterParam.Def.Color.Type == IMG_MODE_PC) {
                //Load VideoCC
                pCCLoadForbidFlag->stillCC[0] = 1U;
                pCCLoadForbidFlag->stillCC[1] = 1U;
                pCCLoadForbidFlag->stillCC[2] = 1U;
                pCCLoadForbidFlag->stillCC[3] = 1U;
                pCCLoadForbidFlag->stillCC[4] = 1U;
            } else {
                //PASS
            }
        } else {
            //PASS
        }
    }
    if(rVal != 0U){
        /* MisraC */
    }
    return rVal;
}

static UINT32 GetParameterNum(INT32 sensorID, UINT32 HdrEnable)
{
    UINT32 parameterNum = 0;

    if (HdrEnable == IMG_NORMAL_MODE) {
        parameterNum = (UINT32)sensorID + 1000U;
    } else if (HdrEnable == IMG_DSP_HDR_MODE_0) {
        parameterNum = (UINT32)sensorID + 2000U;
    } else if (HdrEnable == IMG_DSP_HDR_MODE_1) {
        parameterNum = (UINT32)sensorID + 3000U;
    } else if (HdrEnable == IMG_SENSOR_HDR_MODE_0) {
        parameterNum = (UINT32)sensorID + 4000U;
    } else {
        parameterNum = (UINT32)sensorID + 5000U;
    }

    return parameterNum;
}


/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: App_Image_Init_Image_Params
 *
 *  @Description:: Entry point of inital IQ parameters
 *
 *  @Input      ::
 *    const char *name : inital module string
 *    UINT32 type      : IMG_PARAM (0), AAA_PARAM  (1), ADJ_PARAM_VIDEO (2), ADJ_PARAM_PHOTO (3), ADJ_PARAM_LISO_STILL (4)
 *  @Output     ::
 *    None
 *
 *
 *  @Return     ::
 *    UINT32 rVal      : retrun 0 means command done
\*-----------------------------------------------------------------------------------------------*/
static UINT32 App_Image_Init_Image_Params(UINT32 viewID)
{

    UINT32                  rVal = 0;
    IQ_PARAM_s              *pIQParams = NULL;

    rVal |= Amba_IQParam_Get_IqParams(viewID, &pIQParams);

    rVal |= AmbaImgProc_ChkIqParamVerNum(IQ_PARAMS_IMG_DEF, pIQParams->ImgParam.VersionNum, 0U);
    rVal |= AmbaImgProc_SetImgParam(viewID, &pIQParams->ImgParam);
    if(rVal != 0U){
        /* MisraC */
    }
    return rVal;
}
#if !defined(CONFIG_SOC_CV2FS) && !defined(CONFIG_SOC_CV22FS)
static UINT32 App_Image_Init_StillIdxInfo_Params(UINT32 viewID)
{

    UINT32                  rVal = 0;
    IQ_PARAM_s              *pIQParams = NULL;

    rVal |= Amba_IQParam_Get_IqParams(viewID, &pIQParams);

    rVal |= AmbaImgProc_ChkIqParamVerNum(IQ_PARAMS_STILL_IDX_INFO_ADJ, pIQParams->stillIdxInfoParams.VersionNumber, pIQParams->stillIdxInfoParams.ParamVersionNum);
    rVal |= AmbaImgProc_SetStillIdx(viewID, &pIQParams->stillIdxInfoParams);

    if(rVal != 0U){
        /* MisraC */
    }
    return rVal;
}
#endif
/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: App_Image_Init_AAA_Params
 *
 *  @Description:: Entry point of inital AAA parameters
 *
 *  @Input      ::
 *    const char *name : inital module string
 *    UINT32 type      : IMG_PARAM (0), AAA_PARAM  (1), ADJ_PARAM_VIDEO (2), ADJ_PARAM_PHOTO (3), ADJ_PARAM_LISO_STILL (4)
 *  @Output     ::
 *    None
 *
 *
 *  @Return     ::
 *    UINT32 rVal      : retrun 0 means command done
\*-----------------------------------------------------------------------------------------------*/
static UINT32 App_Image_Init_AAA_Params(UINT32 viewID)
{
    UINT32                  rVal = 0;
    IQ_PARAM_s              *pIQParams = NULL;
    if(AAAParamsLoadFlg[viewID][0] == 1U){
        rVal |= Amba_IQParam_Get_IqParams(viewID, &pIQParams);

        rVal |= AmbaImgProc_ChkIqParamVerNum(IQ_PARAMS_AAA, pIQParams->AAAParams[0].StructVersionNum, pIQParams->AAAParams[0].ParamVersionNum);
        rVal |= AmbaImgProc_SetAAAParam(viewID, &pIQParams->AAAParams[0]);
    }
    if(rVal != 0U){
        /* MisraC */
    }
    return rVal;
}



/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: App_Image_Init_Scene_Params
 *
 *  @Description:: Entry point of inital Scene parameters
 *
 *  @Input      ::
 *    const char *name : inital module string
 *    UINT32 type      : SCENE_DATA_S01, SCENE_DATA_S02, SCENE_DATA_S03, SCENE_DATA_S04, SCENE_DATA_S05
 *  @Output     ::
 *    None
 *
 *
 *  @Return     ::
 *    UINT32 rVal      : retrun 0 means command done
\*-----------------------------------------------------------------------------------------------*/
static UINT32 App_Image_Init_Scene_Params(UINT32 viewID, UINT8 sceneParamIdx)
{
    UINT32                  rVal = 0;
    IQ_PARAM_s              *pIQParams = NULL;
    UINT8                   i = 0;

    rVal |= Amba_IQParam_Get_IqParams(viewID, &pIQParams);

    //Load 8 scene table for each scene parameter
    for(i=(sceneParamIdx*8U); i<((sceneParamIdx*8U)+8U); i++) {
        rVal |= AmbaImgProc_ChkIqParamVerNum(IQ_PARAM_SCENE, pIQParams->sceneParams[i].VersionNumber, pIQParams->sceneParams[i].ParamVersionNum);
        rVal |= AmbaImgProc_SCSetSceneModeInfo(viewID, (INT32)i, &pIQParams->sceneParams[i]);

    }
    if(rVal != 0U){
        /* MisraC */
    }
    return rVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: App_Image_Init_De_Params
 *
 *  @Description:: Entry point of inital DE parameters
 *
 *  @Input      ::
 *    const char *name : inital module string
 *    UINT32 type      :
 *  @Output     ::
 *    None
 *
 *
 *  @Return     ::
 *    UINT32 rVal      : retrun 0 means command done
\*-----------------------------------------------------------------------------------------------*/
static UINT32 App_Image_Init_DeVideo_Params(UINT32 viewID)
{
    UINT32          rVal = 0;
    IQ_PARAM_s      *pIQParams = NULL;

    rVal |= Amba_IQParam_Get_IqParams(viewID, &pIQParams);

    //DEVideo
    rVal |= AmbaImgProc_SetDeParam(viewID, IP_MODE_VIDEO, &pIQParams->DEVideoParams[0]);
    if(rVal != 0U){
        /* MisraC */
    }
    return rVal;
}

static UINT32 App_Image_Init_DeStill_Params(UINT32 viewID)
{
    UINT32          rVal = 0;
    IQ_PARAM_s      *pIQParams = NULL;

    rVal |= Amba_IQParam_Get_IqParams(viewID, &pIQParams);

    //DEStill
    rVal |= AmbaImgProc_SetDeParam(viewID, IP_MODE_STILL, &pIQParams->DEStillParams[0]);
    if(rVal != 0U){
        /* MisraC */
    }
    return rVal;
}

#if !defined(CONFIG_SOC_CV2FS) && !defined(CONFIG_SOC_CV22FS)&& !defined(CONFIG_SOC_CV5)&& !defined(CONFIG_SOC_CV52)
static UINT32 App_Image_Init_CC_REG_Params(UINT32 viewID)
{
    UINT32              rVal = 0;
    IQ_PARAM_s          *pIQParams;
    ULONG               CCRegAddr;
    const UINT8         *pCCReg;

    rVal |= Amba_IQParam_Get_IqParams(viewID, &pIQParams);
    pCCReg = &pIQParams->CCRegParam[0];
    //CCRegAddr = PointerToUINT32(pCCReg);
    CCRegAddr = SPointerToULONG(pCCReg);
    rVal |= AmbaImgProc_SetCcRegAddr(viewID, CCRegAddr);
    if(rVal != 0U){
        /* MisraC */
    }
    return rVal;
}
#endif

#ifdef HDR_CMP_DCMP_SAMPLE
static UINT32 App_Image_HdrCmpd2330_20bit(UINT32 idx)
{
    FLOAT  x1  = 16384.0f,    y1  = 16384.0f;
    FLOAT  x2  = 32768.0f,    y2  = 24576.0f;
    FLOAT  x3  = 65536.0f,    y3  = 32768.0f;
    FLOAT  x4  = 131072.0f,   y4  = 40960.0f;
    FLOAT  x5  = 262144.0f,   y5  = 49152.0f;
    FLOAT  x6  = 524288.0f,   y6  = 57344.0f;
    FLOAT  x7  = 1048576.0f,  y7  = 65535.0f;
    FLOAT  slope0, slope1, slope2, slope3, slope4, slope5, slope6;
    FLOAT  value;
    UINT32 valueU32;
    UINT32 idxTmp;
    UINT32 bits_count = 12U;
    //static UINT32 counter = 0;

    idxTmp = idx;
    slope0 =  y1/x1;
    slope1 =  (y2  - y1)  / (x2  - x1);
    slope2 =  (y3  - y2)  / (x3  - x2);
    slope3 =  (y4  - y3)  / (x4  - x3);
    slope4 =  (y5  - y4)  / (x5  - x4);
    slope5 =  (y6  - y5)  / (x6  - x5);
    slope6 =  (y7  - y6)  / (x7  - x6);

    /* Perform pedestial here*/
    idxTmp += 168U;

    if (idxTmp <= (UINT32)x1) {
        value = ((FLOAT)idxTmp * slope0);
    } else if (idxTmp <= (UINT32)x2) {
        value = (((FLOAT)idxTmp - x1) * slope1) + y1;
    } else if (idxTmp <= (UINT32)x3) {
        value = (((FLOAT)idxTmp - x2) * slope2) + y2;
    } else if (idxTmp <= (UINT32)x4) {
        value = (((FLOAT)idxTmp - x3) * slope3) + y3;
    } else if (idxTmp <= (UINT32)x5) {
        value = (((FLOAT)idxTmp - x4) * slope4) + y4;
    } else if (idxTmp <= (UINT32)x6) {
        value = (((FLOAT)idxTmp - x5) * slope5) + y5;
    } else if (idxTmp <= (UINT32)x7) {
        value = (((FLOAT)idxTmp - x6) * slope6) + y6;
    } else {
        value = y7;
    }


    valueU32 = (UINT32)value;

    if( valueU32 >= ((UINT32)1<<bits_count)){
        valueU32 = (((UINT32)1<< bits_count) -  (UINT32)1) ;
    }
    //if(counter % 1000 == 0){
    //    AmbaPrint_PrintUInt5("App_Image_HdrCmpd2330_20bit", 0, 0, 0, 0, 0);
    //}
    //counter++;
    return valueU32;
}


static UINT32 App_Image_HdrDeCmpd2330_20bit(UINT32 idx)
{
    FLOAT  x1  = 16384.0f,    y1  = 16384.0f;
    FLOAT  x2  = 24576.0f,    y2  = 32768.0f;
    FLOAT  x3  = 32768.0f,    y3  = 65536.0f;
    FLOAT  x4  = 40960.0f,    y4  = 131072.0f;
    FLOAT  x5  = 49152.0f,    y5  = 262144.0f;
    FLOAT  x6  = 57344.0f,    y6  = 524288.0f;
    FLOAT  x7  = 65535.0f,    y7  = 1048576.0f;
    FLOAT  slope0, slope1, slope2, slope3, slope4, slope5, slope6;
    FLOAT  value;
    UINT32 valueU32;
    FLOAT  blc = 168.0f;
    UINT32 bits_count = 20U;
    //static UINT32 counter = 0;

    slope0 =  y1/x1;
    slope1 =  (y2  - y1)  / (x2  - x1);
    slope2 =  (y3  - y2)  / (x3  - x2);
    slope3 =  (y4  - y3)  / (x4  - x3);
    slope4 =  (y5  - y4)  / (x5  - x4);
    slope5 =  (y6  - y5)  / (x6  - x5);
    slope6 =  (y7  - y6)  / (x7  - x6);

    if (idx <= (UINT32)x1) {
        value = ((FLOAT)idx * slope0);
    } else if (idx <= (UINT32)x2) {
        value = (((FLOAT)idx - x1) * slope1) + y1;
    } else if (idx <= (UINT32)x3) {
        value = (((FLOAT)idx - x2) * slope2) + y2;
    } else if (idx <= (UINT32)x4) {
        value = (((FLOAT)idx - x3) * slope3) + y3;
    } else if (idx <= (UINT32)x5) {
        value = (((FLOAT)idx - x4) * slope4) + y4;
    } else if (idx <= (UINT32)x6) {
        value = (((FLOAT)idx - x5) * slope5) + y5;
    } else if (idx <= (UINT32)x7) {
        value = (((FLOAT)idx - x6) * slope6) + y6;
    } else {
        value = y7;
    }

    /* Perform BLC here, something like  */
    if (value <= blc) {
        value = 0.0f;
    } else {
        value -= blc;
    }

    valueU32 = (UINT32)value;

    if( valueU32 >= ((UINT32)1<<bits_count)){
        valueU32 = (((UINT32)1<< bits_count) -  (UINT32)1) ;
    }
    //if(counter % 1000 == 0){
    //    AmbaPrint_PrintUInt5("App_Image_HdrDeCmpd2330_20bit", 0, 0, 0, 0, 0);
    //}
    //counter++;
    return valueU32;
}
#endif

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: App_Image_Init
 *
 *  @Description:: Entry point of inital of IQ parameters
 *
 *  @Input      ::  None
 *
 *  @Output     ::  None
 *
 *  @Return     ::
 *    UINT32 rVal      : retrun 0 means command done
\*-----------------------------------------------------------------------------------------------*/
UINT32 App_Image_Init(UINT32 viewIDCount)
{
    UINT32              rVal = 0;
    UINT32              i;
    IMG_PROC_FUNC_s     IpFuncTmp = {NULL, NULL, NULL, NULL, NULL, NULL};
    void                *pAAAMem = NULL;
    SIZE_t              AAAMemSize = 0U, QueryAAAMemSize = 0U;

    rVal |= AmbaPrint_ModuleSetAllowList(IMGPROC_PRINT_MODULE_ID, 1U/*0U*/);

    rVal |= AmbaImgProc_GetMemInfo(&pAAAMem, &AAAMemSize);
    if (pAAAMem != NULL) {
        rVal |= AmbaWrap_memset(pAAAMem, 0, AAAMemSize);
    }
    rVal |= AmbaImgProc_QueryMemorySize(viewIDCount, &QueryAAAMemSize);

    if(AAAMemSize < QueryAAAMemSize) {
        AmbaPrint_ModulePrintUInt5(IMGPROC_PRINT_MODULE_ID, " [ERROR] AAAMemSize: %d   NOT ENOUGH, %d bytes at least", (UINT32)AAAMemSize, (UINT32) QueryAAAMemSize, 0U, 0U, 0U);
        AmbaPrint_Flush();
    } else {
        AmbaPrint_ModulePrintUInt5(IMGPROC_PRINT_MODULE_ID, " <App_Image_Init>, (viewIDCount = %d, AAAMemSize = %d)", viewIDCount, (UINT32)AAAMemSize, 0U, 0U, 0U);
        rVal |= AmbaImgProc_SetTotalViewCount(viewIDCount);
        rVal |= AmbaImgProc_ParamInit(viewIDCount);

        /*Hook/Reset IP functinos*/
        IpFuncTmp.Adj_Init = Amba_AdjInit;
#ifdef HDR_CMP_DCMP_SAMPLE
        IpFuncTmp.HdrDeCmpdFunc = App_Image_HdrDeCmpd2330_20bit;
        IpFuncTmp.HdrCmpdFunc =App_Image_HdrCmpd2330_20bit;
#endif
        for(i = 0U; i < viewIDCount; i++) {
            rVal |= AmbaImgProc_SetRegFunc(i, &IpFuncTmp);
        }
    }
    if(rVal != 0U){
        /* MisraC */
    }
    return rVal;
}


UINT32 App_Image_Init_Iq_Params(UINT32 viewID, INT32 sensorID)
{
    UINT32                          rVal = 0;

    rVal |=  App_Image_Init_Iq_Params_Section(viewID, sensorID, IMGPROC_PARTIAL_LOAD_All);
    return rVal;
}

UINT32 App_Image_Iq_Params_Preload(UINT32 viewID, INT32 sensorID)
{
    UINT32 rVal = 0;
    rVal |=  App_Image_Iq_Params_load_Section(viewID, sensorID, IMGPROC_PARTIAL_LOAD_All);
    if(rVal != 0U){
        /* MisraC */
    }
    return rVal;
}

UINT32 App_Image_Init_Iq_Params_Section(UINT32 viewID, INT32 sensorID, UINT8 section)
{
    UINT32                          rVal = 0;
    UINT32                          i = 0;
    AMBA_AAA_OP_INFO_s              AaaOpInfo = {ENABLE, ENABLE, ENABLE, ENABLE, 0U, 0U, 0U, 0U};
    UINT32                          HdrEnable = 0;
    UINT32                          parameterNum = 0, handlerId = 0, loadFromRom = ENABLE;
    IQ_PARAM_LOAD_FORBID_FLAG       loadForbidFlag;
    IQ_PARAM_CC_LOAD_FORBID_FLAG    CCloadForbidFlag;

    AmbaPrint_ModulePrintUInt5(IMGPROC_PRINT_MODULE_ID, "App_Image_Init_Iq_Params_Section (viewID : %d, sensorID : %d, section: %d)", viewID, (UINT32)sensorID, section, 0U, 0U);


    rVal |= AmbaImgProc_HDRGetVideoMode(viewID, &HdrEnable);
    parameterNum = GetParameterNum(sensorID, HdrEnable);
    for (i=0; i<MAX_VIEW_NUM; i++) {
        if (tableMap[i][section] == parameterNum) {
            handlerId = i;
            loadFromRom = DISABLE;
        }
    }

    if (loadFromRom == ENABLE) {
        AmbaPrint_ModulePrintStr5(IMGPROC_PRINT_MODULE_ID, "Load from ROM...", NULL, NULL, NULL, NULL, NULL);
        rVal |= Amba_IQParam_Config_IQ_Table_Path((UINT32)sensorID, HdrEnable);
        tableMap[viewID][section] = parameterNum;
    } else {
        AmbaPrint_ModulePrintUInt5(IMGPROC_PRINT_MODULE_ID, "Copy from IQParam[%d]...", handlerId, 0U, 0U, 0U, 0U);
        tableMap[viewID][section] = tableMap[handlerId][section];
    }


    rVal |= GetLoadForbidFlag(section, &loadForbidFlag);
    rVal |= Amba_IQParam_LoadIqParams(viewID, handlerId, &loadForbidFlag, loadFromRom);
    rVal |= GetCCLoadForbidFlag(viewID, section, &CCloadForbidFlag);
    rVal |= Amba_IQParam_LoadCCParams(viewID, handlerId, &loadForbidFlag, &CCloadForbidFlag, loadFromRom);

    for( i = 0U;i < AAA_TABLE_MAX_NO; i++){
        if((loadForbidFlag.AAAParams[i] == 0U) && (AAAParamsLoadFlg[viewID][i] == 0U)){
            AAAParamsLoadFlg[viewID][i] = 1U;
        }
    }

    rVal |= App_Image_Init_AAA_Params(viewID);

    if (loadForbidFlag.ImgParam == 0U) {
        rVal |= App_Image_Init_Image_Params(viewID);
    }

    for (i=0U; i<SCENE_TABLE_MAX_NO; i++) {
        if (loadForbidFlag.sceneParams[i] == 0U) {
            rVal |= App_Image_Init_Scene_Params(viewID, (UINT8)i);
        }
    }

    if (loadForbidFlag.DEVideoParams[0] == 0U) {
        rVal |= App_Image_Init_DeVideo_Params(viewID);
    }

    if (loadForbidFlag.DEStillParams[0] == 0U) {
        rVal |= App_Image_Init_DeStill_Params(viewID);
    }

#if !defined(CONFIG_SOC_CV2FS) && !defined(CONFIG_SOC_CV22FS)
    if (loadForbidFlag.stillIdxInfoParams == 0U) {
        rVal |= App_Image_Init_StillIdxInfo_Params(viewID);
    }
#if !defined(CONFIG_SOC_CV5)&& !defined(CONFIG_SOC_CV52)
    rVal |= App_Image_Init_CC_REG_Params(viewID);
#endif
#endif

    if (rVal == 0U) {
        rVal |= AmbaImgProc_SetAAAOPInfo(viewID, &AaaOpInfo);
    } else {
        AmbaPrint_ModulePrintUInt5(IMGPROC_PRINT_MODULE_ID, "Check  <App_Image_Init_Iq_Params>", 0U, 0U, 0U, 0U, 0U);
    }

    return rVal;
}

UINT32 App_Image_Iq_Params_load_Section(UINT32 viewID, INT32 sensorID, UINT8 section)
{
    UINT32 rVal = 0;
    UINT32 i = 0;
    UINT32 parameterNum = 0;
    UINT32 handlerId = 0;
    UINT32 HdrEnable = 0;
    UINT32 loadFromRom = ENABLE;
    IQ_PARAM_LOAD_FORBID_FLAG loadForbidFlag;
    IQ_PARAM_CC_LOAD_FORBID_FLAG CCloadForbidFlag;

    AmbaPrint_ModulePrintUInt5(IMGPROC_PRINT_MODULE_ID, "App_Image_Iq_Params_load_Section (viewID : %d, sensorID : %d, section: %d)", viewID, (UINT32)sensorID, section, 0U, 0U);

    rVal |= GetLoadForbidFlag(section, &loadForbidFlag);
    rVal |= AmbaImgProc_HDRGetVideoMode(viewID, &HdrEnable);
    parameterNum = GetParameterNum(sensorID, HdrEnable);
    for (i=0; i<MAX_VIEW_NUM; i++) {
        if (tableMap[i][section] == parameterNum) {
            handlerId = i;
            loadFromRom = DISABLE;
        }
    }
    for( i = 0U;i < AAA_TABLE_MAX_NO; i++){
        if((loadForbidFlag.AAAParams[i] == 0U) && (AAAParamsLoadFlg[viewID][i] == 0U)){
            AAAParamsLoadFlg[viewID][i] = 1U;
        }
    }

    if (loadFromRom == ENABLE) {
        AmbaPrint_ModulePrintStr5(IMGPROC_PRINT_MODULE_ID, "Preload: Load from ROM...", NULL, NULL, NULL, NULL, NULL);
        rVal |= Amba_IQParam_Config_IQ_Table_Path((UINT32)sensorID, HdrEnable);
        tableMap[viewID][section] = parameterNum;
        rVal |= Amba_IQParam_LoadIqParams(viewID, handlerId, &loadForbidFlag, loadFromRom);
        rVal |= GetCCLoadForbidFlag(viewID, section, &CCloadForbidFlag);
        rVal |= Amba_IQParam_LoadCCParams(viewID, handlerId, &loadForbidFlag, &CCloadForbidFlag, loadFromRom);

    } else {
        AmbaPrint_ModulePrintUInt5(IMGPROC_PRINT_MODULE_ID, "Preload: Copy from IQParam[%d]...", handlerId, 0U, 0U, 0U, 0U);
    }

    if (rVal != 0U) {
        AmbaPrint_ModulePrintUInt5(IMGPROC_PRINT_MODULE_ID, "Check  <App_Image_Iq_Params_Preload>", 0U, 0U, 0U, 0U, 0U);
    }
    return rVal;
}
