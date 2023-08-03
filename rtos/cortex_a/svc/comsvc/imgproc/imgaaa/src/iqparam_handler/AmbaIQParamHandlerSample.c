/**
 * @file AmbaIQParamHandlerSample.c
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
 *  @Description    :: Implementation of IQ param handler
 *
 */

// #include ".svc_autogen"

#include "AmbaTypes.h"
#include "AmbaPrint.h"
#if defined(CONFIG_QNX) || defined(CONFIG_LINUX)
#include "AmbaWrap.h"
#endif
#include "AmbaUtility.h"
#include "AmbaNVM_Partition.h"
#include "AmbaNAND.h"
#include "AmbaSYS.h"
#include "AmbaSvcWrap.h"
#ifdef SVC_AMBAIMG_ADJUSTMENT
#include SVC_AMBAIMG_ADJUSTMENT
#endif
#include "AmbaIQParamHandlerSample.h"
#include "AmbaIQParamPath.h"
#include "AmbaImg_External_CtrlFunc.h"
#include "AmbaFS.h"
#ifdef CONFIG_QNX
#define AmbaMisra_TouchUnused(x)
#define AmbaMisra_TypeCast(a, b) memcpy((void *)a, (void *)b, sizeof(void *))
#endif
#ifdef CONFIG_LINUX
#include "AmbaMisraFix.h"
#endif
static IQ_PARAM_s      *pIQParamBuf[MAX_VIEW_NUM] = {NULL, NULL, NULL, NULL,
                                                     NULL, NULL, NULL, NULL,
                                                     NULL, NULL, NULL, NULL,
                                                     NULL, NULL, NULL, NULL};
static IQ_TABLE_PATH_s IQTablePaths GNU_SECTION_NOZEROINIT;

static COLOR_TABLE_MAP ColorTableMap[MAX_VIEW_NUM] GNU_SECTION_NOZEROINIT;
static UINT16          SceneColorTable[MAX_VIEW_NUM] GNU_SECTION_NOZEROINIT;
static UINT16          DEColorTable[MAX_VIEW_NUM] GNU_SECTION_NOZEROINIT;

static AMBA_IQ_PARAM_TABLE_PATH_f IQTablePathExtraProc = NULL;
static READ_ROM_FILE              ReadRomFileWrap = NULL;

#if !defined(CONFIG_SOC_CV2FS) && !defined(CONFIG_SOC_CV22FS)&& !defined(CONFIG_SOC_CV5)&& !defined(CONFIG_SOC_CV52)
static AMBA_IQ_PARAM_TABLE_DSP_PATH_f IQTableDspPathExtraProc = NULL;
static IQ_TABLE_DSP_PATH_s IQTableDspPaths = {
    .CcReg = {"capture1_CC_Reg.bin"},
};
#endif
//CertC
static IQ_CC_Table IQCcTableTmp[MAX_VIEW_NUM] GNU_SECTION_NOZEROINIT;

static UINT32 LoadFileData(const void *pDestBuff, const char *pPath);


static ULONG IQPointerToAddr(const void * ptr){
    ULONG addr = 0;
    UINT32 U32RVal = 0U;
    U32RVal |= AmbaWrap_memcpy(&addr, &ptr, sizeof(ULONG));
    if(U32RVal != 0U){
        /* MisraC */
    }
    return addr;
}



static UINT32 LoadRomData(const void *pDestBuff, const char *pPath, UINT8 forbidFlag)
{
    UINT32 rVal = 0;
    UINT32 fileSize = 0;
    UINT32 NvmID;
    UINT8  *pDataBuf = NULL;
    UINT32 U32RVal = 0U;

    /* Load file if forbidFlag = 0, otherwise don't load */
    if (forbidFlag == 0U){
        if ((pPath != NULL) && (pPath[1] == ':') && (pPath[2] == '\\')) {
            rVal = LoadFileData(pDestBuff, pPath);
        } else {
            //Config NVMId
            U32RVal = AmbaSvcWrap_GetNVMID(&NvmID, 1U, AMBA_NVM_ROM_REGION_SYS_DATA);
            if (AMBA_NUM_NVM <= NvmID) {
                AmbaPrint_ModulePrintUInt5(IMGPROC_PRINT_MODULE_ID, "check LoadRomData in AmbaIQParamHandlerSample.c", 0U, 0U, 0U, 0U, 0U);
            } else {
                U32RVal |= AmbaNVM_GetRomFileSize(NvmID, AMBA_NVM_ROM_REGION_SYS_DATA, pPath, &fileSize); //FIXME, fileSize should be a parameter

                if (fileSize != 0U){
                    AmbaMisra_TypeCast(&pDataBuf, &pDestBuff);
                    if (ReadRomFileWrap == NULL) {
                        rVal = AmbaNVM_ReadRomFile(NvmID, AMBA_NVM_ROM_REGION_SYS_DATA, pPath, 0U, fileSize, pDataBuf, 5000);
                    } else {
                        rVal = ReadRomFileWrap(NvmID, AMBA_NVM_ROM_REGION_SYS_DATA, pPath, 0U, fileSize, pDataBuf, 5000);
                    }
                } else {
                    rVal = IMGPROC_ERR_0007;
                }

                if (rVal != 0U) {
                    AmbaPrint_ModulePrintStr5(IMGPROC_PRINT_MODULE_ID, "load IQ Params from ROMFS error, %s", pPath, NULL, NULL, NULL, NULL);
                } else {
                    // AmbaPrint_ModulePrintStr5(IMGPROC_PRINT_MODULE_ID, "load IQ Params from ROMFS success, %s", pPath,  NULL, NULL, NULL, NULL);
                }
            }
        }
    } else {
        rVal = 0;
    }
    if(U32RVal != 0U){
        /* MisraC */
    }
    return rVal;
}

/*CC*/
static UINT32 Load_CC_Params(UINT32 viewID, UINT32 ccSetIdx, const IQ_PARAM_CC_LOAD_FORBID_FLAG* pCCLoadForbidFlag)
{
    UINT32  rVal = 0;
    UINT32   i;

    for(i=0U; i<DEF_CC_TABLE_NO; i++) {
        if (pIQParamBuf[viewID] != NULL) {
            rVal |= LoadRomData(&pIQParamBuf[viewID]->videoCC[i].buf[0], IQTablePaths.CCSetPaths[ccSetIdx].videoCC[i].path, pCCLoadForbidFlag->videoCC[i]);
        }
    }

    for(i=0U; i<DEF_CC_TABLE_NO; i++) {
        if (pIQParamBuf[viewID] != NULL) {
            rVal |= LoadRomData(&pIQParamBuf[viewID]->stillCC[i].buf[0], IQTablePaths.CCSetPaths[ccSetIdx].stillCC[i].path, pCCLoadForbidFlag->stillCC[i]);
        }
    }

    return rVal;

}

static IQ_CC_Table* Get_CC_Params(UINT32 viewID, UINT8 CCType)
{
    IQ_CC_Table *pCCParams = NULL;
    UINT32 RVal =0U;
    RVal = AmbaWrap_memset(&IQCcTableTmp[viewID], 0, sizeof(IQ_CC_Table));
    if(RVal != 0U){
        /* MisraC */
    }
    pCCParams = &IQCcTableTmp[viewID]; //CertC

    if(CCType == IMG_MODE_TV) {
        if (pIQParamBuf[viewID] != NULL) {
            pCCParams = &pIQParamBuf[viewID]->videoCC[0];
        }
    }
    else if(CCType == IMG_MODE_PC) {
        if (pIQParamBuf[viewID] != NULL) {
            pCCParams = &pIQParamBuf[viewID]->stillCC[0];
        }
    }
    else {
        AmbaPrint_ModulePrintStr5(IMGPROC_PRINT_MODULE_ID, "Unknow CCType (check <%s>)", __func__, NULL, NULL, NULL, NULL);
    }

    return pCCParams;
}

static void Attach_CC_Table_Addr(UINT32 viewID, const IQ_PARAM_LOAD_FORBID_FLAG* pLoadForbidFlag)
{
    UINT32               i, CCIdx;
    const IQ_CC_Table   *pCCParams = NULL;
    IQ_PARAM_s *pIqParam = NULL;
    UINT32 RVal =0U;
    RVal = AmbaWrap_memset(&IQCcTableTmp[viewID], 0, sizeof(IQ_CC_Table));
    pCCParams = &IQCcTableTmp[viewID]; //CertC
    if(RVal != 0U){
        /* MisraC */
    }
    if (pIQParamBuf[viewID] != NULL) {
        pIqParam = pIQParamBuf[viewID];

        //Attach CC param to ADJ Param

        // ImgParam
        CCIdx = 1; //Use idx as 1 as default

        pCCParams = Get_CC_Params(viewID, IMG_MODE_TV);
        pIqParam->ImgParam.ColorCorrVideoAddr = (UINT32)IQPointerToAddr(&pCCParams[CCIdx].buf[0]);//check

        pCCParams = Get_CC_Params(viewID, IMG_MODE_PC);
        pIqParam->ImgParam.ColorCorrStillAddr = (UINT32)IQPointerToAddr(&pCCParams[CCIdx].buf[0]);


        // video
        for (i=0U; i<ADJ_VIDEO_TABLE_MAX_NO; i++) {
            if (pLoadForbidFlag->videoParams[i] == 0U) {
                pCCParams = Get_CC_Params(viewID, pIqParam->videoParams[i].FilterParam.Def.Color.Type);
                for (CCIdx=0U; CCIdx<DEF_CC_TABLE_NO; CCIdx++) {
                    pIqParam->videoParams[i].FilterParam.Def.Color.Table[CCIdx].MatrixThreeDTableAddr = IQPointerToAddr(&pCCParams[CCIdx].buf[0]);
                }
            }
        }

        //photo
        for (i=0U; i<ADJ_PHOTO_TABLE_MAX_NO; i++) {
            if (pLoadForbidFlag->photoPreviewParams[i] == 0U) {
                pCCParams = Get_CC_Params(viewID, pIqParam->photoPreviewParams[i].FilterParam.Def.Color.Type);
                for (CCIdx=0U; CCIdx<DEF_CC_TABLE_NO; CCIdx++) {
                    pIqParam->photoPreviewParams[i].FilterParam.Def.Color.Table[CCIdx].MatrixThreeDTableAddr = IQPointerToAddr(&pCCParams[CCIdx].buf[0]);
                }
            }
        }
#if !defined(CONFIG_SOC_CV2FS) && !defined(CONFIG_SOC_CV22FS)

        //still LISO
        for (i=0U; i<ADJ_STILL_LISO_TABLE_MAX_NO; i++) {
            if (pLoadForbidFlag->stillLISOParams[i] == 0U) {
                pCCParams = Get_CC_Params(viewID, pIqParam->stillLISOParams[i].Def.Color.Type);
                for (CCIdx = 0U; CCIdx<DEF_CC_TABLE_NO; CCIdx++) {
                    pIqParam->stillLISOParams[i].Def.Color.Table[CCIdx].MatrixThreeDTableAddr = IQPointerToAddr(&pCCParams[CCIdx].buf[0]);
                }
            }
        }

        //still HISO
        for (i=0U; i<ADJ_STILL_HISO_TABLE_MAX_NO; i++) {
            if (pLoadForbidFlag->stillHISOParams[i] == 0U) {
                pCCParams = Get_CC_Params(viewID, pIqParam->stillHISOParams[i].Def.Color.Type);
                for(CCIdx = 0U; CCIdx<DEF_CC_TABLE_NO; CCIdx++) {
                    pIqParam->stillHISOParams[i].Def.Color.Table[CCIdx].MatrixThreeDTableAddr = IQPointerToAddr(&pCCParams[CCIdx].buf[0]);
                }
            }
        }

#endif
    }
}

#if !defined(CONFIG_SOC_CV2FS) && !defined(CONFIG_SOC_CV22FS)&& !defined(CONFIG_SOC_CV5)&& !defined(CONFIG_SOC_CV52)
// CC Reg
static UINT32 Load_CC_REG(UINT32 viewID)
{
    UINT32      rVal = 0;
    const IQ_PARAM_s *pIqParam = NULL;

    if (pIQParamBuf[viewID] != NULL) {
        pIqParam = pIQParamBuf[viewID];
        rVal |= LoadRomData(&pIqParam->CCRegParam[0], &IQTableDspPaths.CcReg.path[0], 0U);
    }

    return rVal;
}
#endif

/*Digital Effect*/
static UINT32 Load_DE_Params(UINT32 viewID, const IQ_PARAM_LOAD_FORBID_FLAG* pLoadForbidFlag)
{
    UINT32  i = 0;
    UINT32  rVal = 0;
    const IQ_PARAM_s *pIqParam = NULL;

    if (pIQParamBuf[viewID] != NULL) {
        pIqParam = pIQParamBuf[viewID];

        rVal |= LoadRomData(&pIqParam->DEVideoParams[0], &IQTablePaths.DEVideo[0].path[0], pLoadForbidFlag->DEVideoParams[0]);
        rVal |= LoadRomData(&pIqParam->DEStillParams[0], &IQTablePaths.DEStill[0].path[0], pLoadForbidFlag->DEStillParams[0]);

        // Init DEColorTableMap
        for (i = 0U; i<(UINT32)DIGITAL_LAST; i++) {
            ColorTableMap[viewID].DEColorTableMap[i] = pIqParam->DEVideoParams[0].DeInfo[i].Cc3dNo;
        }

        //Init DEColorTable
        DEColorTable[viewID] = SYSTEM_DEFAULT;
    } else {
        AmbaPrint_ModulePrintUInt5(IMGPROC_PRINT_MODULE_ID, "[%d] Null IQParamBuf[%d]", __LINE__, viewID, 0U, 0U, 0U);
    }

    return rVal;
}

static UINT32 Get_CC_Set_Index_By_DE(UINT32 viewID, INT32 deMode)
{
    UINT32 colorSetIdx = 0;

    DEColorTable[viewID] = ColorTableMap[viewID].DEColorTableMap[deMode];

    if((DEColorTable[viewID] == SYSTEM_DEFAULT) && (SceneColorTable[viewID] == SYSTEM_DEFAULT)) {
        colorSetIdx = 0U;
    }
    else if((DEColorTable[viewID] == SYSTEM_DEFAULT) && (SceneColorTable[viewID] != SYSTEM_DEFAULT)) {
        colorSetIdx = SceneColorTable[viewID];
    }
    else {
        colorSetIdx = DEColorTable[viewID];
    }

    AmbaPrint_ModulePrintUInt5(IMGPROC_PRINT_MODULE_ID, "DE Mode CC set index: %d", colorSetIdx, 0U, 0U, 0U, 0U);


    return colorSetIdx;
}

/*Scene*/
static UINT32 Load_Scene_Params(UINT32 viewID, const IQ_PARAM_LOAD_FORBID_FLAG* pLoadForbidFlag)
{
    UINT32  i = 0;
    UINT32  rVal = 0;
    const IQ_PARAM_s *pIqParam = NULL;

    if (pIQParamBuf[viewID] != NULL) {
        pIqParam = pIQParamBuf[viewID];

        for (i=0U; i<(UINT32)SCENE_TABLE_MAX_NO; i++) {
            rVal |= LoadRomData(&pIqParam->sceneParams[i*(UINT32)SCENE_TABLE_CONTAIN_SETS], &IQTablePaths.scene[i].path[0], pLoadForbidFlag->sceneParams[i]);
        }

        //Init SceneColorTableMap
        for (i=0U; i<(SCENE_TABLE_MAX_NO * SCENE_TABLE_CONTAIN_SETS); i++) {
            ColorTableMap[viewID].SceneColorTableMap[i] = pIqParam->sceneParams[i].Def.ColorTable;
        }
    } else {
        AmbaPrint_ModulePrintUInt5(IMGPROC_PRINT_MODULE_ID, "[%d] Null IQParamBuf[%d]", __LINE__, viewID, 0U, 0U, 0U);
    }

    //Init SceneColorTable
    SceneColorTable[viewID] = SYSTEM_DEFAULT;

    return rVal;
}

static UINT32 Get_CC_Set_Index_By_Scene(UINT32 viewID, INT32 sceneMode)
{
    UINT32  colorSetIdx;
    INT32   sceneModeTmp;

    if(sceneMode == SCENE_AUTO) {
        sceneModeTmp = SCENE_OFF;
    } else {
        sceneModeTmp = sceneMode;
    }

    SceneColorTable[viewID] = ColorTableMap[viewID].SceneColorTableMap[sceneModeTmp];

    if(DEColorTable[viewID] != SYSTEM_DEFAULT) {
        colorSetIdx = DEColorTable[viewID];
    }
    else if(SceneColorTable[viewID] == SYSTEM_DEFAULT) {
        colorSetIdx = 0;
    }
    else{
        colorSetIdx = SceneColorTable[viewID];
    }

    AmbaPrint_ModulePrintUInt5(IMGPROC_PRINT_MODULE_ID, "Scene Mode CC set index: %d", colorSetIdx, 0U, 0U, 0U, 0U);

    return colorSetIdx;
}

//ADJ param
static UINT32 Load_Adj_Params(UINT32 viewID, const IQ_PARAM_LOAD_FORBID_FLAG* pLoadForbidFlag)
{
    UINT32  rVal = 0;
    UINT32  i = 0;
    const IQ_PARAM_s *pIqParam = NULL;

    if (pIQParamBuf[viewID] != NULL) {
        pIqParam = pIQParamBuf[viewID];

        //ImgParam
        rVal |= LoadRomData(&pIqParam->ImgParam, IQTablePaths.ImgParam.path, pLoadForbidFlag->ImgParam);

        //aaa default

        for (i=0U; i<AAA_TABLE_MAX_NO; i++) {
            rVal |= LoadRomData(&pIqParam->AAAParams[i], IQTablePaths.aaaDefault[i].path, pLoadForbidFlag->AAAParams[i]);
        }

        //video table and photo preview table
        for (i=0U; i<ADJ_VIDEO_TABLE_MAX_NO; i++) {
            rVal |= LoadRomData(&pIqParam->videoParams[i], IQTablePaths.video[i].path, pLoadForbidFlag->videoParams[i]);
        }
        for (i=0U; i<ADJ_PHOTO_TABLE_MAX_NO; i++) {
            rVal |= LoadRomData(&pIqParam->photoPreviewParams[i], IQTablePaths.photo[i].path, pLoadForbidFlag->photoPreviewParams[i]);
        }

#if !defined(CONFIG_SOC_CV2FS) && !defined(CONFIG_SOC_CV22FS)
        //still liso and still high iso
        for (i=0U; i<ADJ_STILL_LISO_TABLE_MAX_NO; i++) {
            rVal |= LoadRomData(&pIqParam->stillLISOParams[i], IQTablePaths.stillLISO[i].path, pLoadForbidFlag->stillLISOParams[i]);
        }

        for (i=0U; i<ADJ_STILL_HISO_TABLE_MAX_NO; i++) {
            rVal |= LoadRomData(&pIqParam->stillHISOParams[i], IQTablePaths.stillHISO[i].path, pLoadForbidFlag->stillHISOParams[i]);
        }

        //still idx info params
        rVal |= LoadRomData(&pIqParam->stillIdxInfoParams, IQTablePaths.stillIdxInfo.path, pLoadForbidFlag->stillIdxInfoParams);
#endif
    } else {
        AmbaPrint_ModulePrintUInt5(IMGPROC_PRINT_MODULE_ID, "[%d] Null IQParamBuf[%d]", __LINE__, viewID, 0U, 0U, 0U);
    }

    return rVal;
}

UINT32 Amba_IQParam_Load_CC_By_Scene(UINT32 viewID, INT32 sceneMode)
{
    UINT32                          rVal = 0;
    UINT32                          colorSetIdx = 0;
    IQ_PARAM_CC_LOAD_FORBID_FLAG    CCLoadForbidFlag;


    colorSetIdx = Get_CC_Set_Index_By_Scene(viewID, sceneMode);

    rVal |= AmbaWrap_memset(&CCLoadForbidFlag, 0, sizeof(IQ_PARAM_CC_LOAD_FORBID_FLAG));
    rVal |= Load_CC_Params(viewID, colorSetIdx, &CCLoadForbidFlag);
    if(rVal != 0U){
        /* MisraC */
    }
    return rVal;
}

UINT32 Amba_IQParam_Load_CC_By_DE(UINT32 viewID, INT32 deMode)
{
    UINT32                          rVal = 0;
    UINT32                          colorSetIdx = 0;
    IQ_PARAM_CC_LOAD_FORBID_FLAG    CCLoadForbidFlag;

    colorSetIdx = Get_CC_Set_Index_By_DE(viewID, deMode);

    rVal |= AmbaWrap_memset(&CCLoadForbidFlag, 0, sizeof(IQ_PARAM_CC_LOAD_FORBID_FLAG));

    rVal |= Load_CC_Params(viewID, colorSetIdx, &CCLoadForbidFlag);
    if(rVal != 0U){
        /* MisraC */
    }
    return rVal;
}

UINT32 Amba_IQParam_Get_IqParams(UINT32 viewID, IQ_PARAM_s **pIQParams)
{
    UINT32 rVal = 0;

    if ((viewID < MAX_VIEW_NUM) && (pIQParamBuf[viewID] != NULL)) {
        *pIQParams = pIQParamBuf[viewID];
    }

    return rVal;
}

static UINT32 LoadADJTableParam(UINT32 viewID)
{
    UINT32 rVal = 0;
    const IQ_PARAM_s *pIqParam = NULL;

    if ((viewID < MAX_VIEW_NUM) && (pIQParamBuf[viewID] != NULL)) {
        pIqParam = pIQParamBuf[viewID];

        rVal |= LoadRomData(&pIqParam->ADJTableParam, IQTablePaths.ADJTable.path, 0U); //(Must load)
    }

    return rVal;
}

UINT32 Amba_IQParam_LoadIqParams(UINT32 viewID, UINT32 handlerID, const IQ_PARAM_LOAD_FORBID_FLAG* pLoadForbidFlag, UINT32 loadFromRom)
{
    UINT32   rVal = 0;
    IQ_PARAM_s *pIqParam = NULL;
    const IQ_PARAM_s *pIqParamHdlr = NULL;

    if (loadFromRom == ENABLE) {
        rVal |= LoadADJTableParam(viewID);
        rVal |= Load_Adj_Params(viewID, pLoadForbidFlag);
        rVal |= Load_Scene_Params(viewID, pLoadForbidFlag);
        rVal |= Load_DE_Params(viewID, pLoadForbidFlag);
    } else {
        if ((pIQParamBuf[viewID] != NULL) &&
            (pIQParamBuf[handlerID] != NULL)) {

            pIqParam = pIQParamBuf[viewID];
            pIqParamHdlr = pIQParamBuf[handlerID];
            rVal |= AmbaWrap_memcpy(pIqParam, pIqParamHdlr, sizeof(IQ_PARAM_s));
        }
    }
    if(rVal != 0U){
        /* MisraC */
    }
    return rVal;
}

UINT32 Amba_IQParam_LoadCCParams(UINT32 viewID, UINT32 handlerID, const IQ_PARAM_LOAD_FORBID_FLAG* pLoadForbidFlag, const IQ_PARAM_CC_LOAD_FORBID_FLAG* pCCLoadForbidFlag, UINT32 loadFromRom)
{
    UINT32   rVal = 0;
    IQ_PARAM_s *pIqParam = NULL;
    const IQ_PARAM_s *pIqParamHdlr = NULL;

    if (loadFromRom == ENABLE) {
        rVal |= Load_CC_Params(viewID, 0U, pCCLoadForbidFlag);  //Use CC set index = 0 as default
#if !defined(CONFIG_SOC_CV2FS) && !defined(CONFIG_SOC_CV22FS)&& !defined(CONFIG_SOC_CV5)&& !defined(CONFIG_SOC_CV52)
        rVal |= Load_CC_REG(viewID);
#endif
    } else {
        if ((pIQParamBuf[viewID] != NULL) &&
            (pIQParamBuf[handlerID] != NULL)) {

            pIqParam = pIQParamBuf[viewID];
            pIqParamHdlr = pIQParamBuf[handlerID];
            rVal |= AmbaWrap_memcpy(pIqParam, pIqParamHdlr, sizeof(IQ_PARAM_s));
        }
    }
    Attach_CC_Table_Addr(viewID, pLoadForbidFlag);
    if(rVal != 0U){
        /* MisraC */
    }
    return rVal;
}
//Complexity
#if defined(IMG_SENSOR_HDR_MODE_2)
static UINT32 IQParam_IQ_Table_Path_SHDR2(UINT32 sensorID)
{
    UINT32  rVal = 0U;
    UINT32  U32RVal = 0U;
    switch(sensorID) {
        case IQ_SSR_IMX490:
            AmbaPrint_ModulePrintStr5(IMGPROC_PRINT_MODULE_ID, "Load IMX490HDR4 IQ table", NULL, NULL, NULL, NULL, NULL);
            U32RVal |= AmbaWrap_memcpy(&IQTablePaths, &IMX490SHDR4Path, sizeof(IQ_TABLE_PATH_s));
            break;
        case IQ_SSR_IMX728:
            AmbaPrint_ModulePrintStr5(IMGPROC_PRINT_MODULE_ID, "Load IMX728SHDR4 IQ table", NULL, NULL, NULL, NULL, NULL);
            U32RVal |= AmbaWrap_memcpy(&IQTablePaths, &IMX728SHDR4Path, sizeof(IQ_TABLE_PATH_s));
            break;
        case IQ_SSR_TI953_954_AR0220_RCCB:
            AmbaPrint_ModulePrintStr5(IMGPROC_PRINT_MODULE_ID, "Load AR0220SHDR4RCCB IQ table", NULL, NULL, NULL, NULL, NULL);
            U32RVal |= AmbaWrap_memcpy(&IQTablePaths, &AR0220SHDR4RCCBPath, sizeof(IQ_TABLE_PATH_s));
            break;
        case IQ_SSR_MAX_9295_9296_RCCB_AR0220:
            AmbaPrint_ModulePrintStr5(IMGPROC_PRINT_MODULE_ID, "Load AR0220SHDR4RCCB IQ table", NULL, NULL, NULL, NULL, NULL);
            U32RVal |= AmbaWrap_memcpy(&IQTablePaths, &AR0220SHDR4RCCBPath, sizeof(IQ_TABLE_PATH_s));
            break;
        case IQ_SSR_MAX9295_9296_OX03C:
            AmbaPrint_ModulePrintStr5(IMGPROC_PRINT_MODULE_ID, "Load OX03CSHDR4 IQ table", NULL, NULL, NULL, NULL, NULL);
            U32RVal |= AmbaWrap_memcpy(&IQTablePaths, &OX03CSHDR4Path, sizeof(IQ_TABLE_PATH_s));
            break;
        case IQ_SSR_MAX9295_9296_OX03F:
            AmbaPrint_ModulePrintStr5(IMGPROC_PRINT_MODULE_ID, "Load OX03FSHDR4 IQ table", NULL, NULL, NULL, NULL, NULL);
            U32RVal |= AmbaWrap_memcpy(&IQTablePaths, &OX03FSHDR4Path, sizeof(IQ_TABLE_PATH_s));
            break;
        case IQ_SSR_MAX9295_9296_OX03C_ADAS:
            AmbaPrint_ModulePrintStr5(IMGPROC_PRINT_MODULE_ID, "Load OX03CSHDR4_ADAS IQ table", NULL, NULL, NULL, NULL, NULL);
            U32RVal |= AmbaWrap_memcpy(&IQTablePaths, &OX03CSHDR4_ADASPath, sizeof(IQ_TABLE_PATH_s));
            break;
        case IQ_SSR_MAX9295_9296_OX03C_EMR:
            AmbaPrint_ModulePrintStr5(IMGPROC_PRINT_MODULE_ID, "Load OX03CSHDR4_EMR IQ table", NULL, NULL, NULL, NULL, NULL);
            U32RVal |= AmbaWrap_memcpy(&IQTablePaths, &OX03CSHDR4_EMRPath, sizeof(IQ_TABLE_PATH_s));
            break;
        case IQ_SSR_MAX9295_9296_OX03F_ADAS:
            AmbaPrint_ModulePrintStr5(IMGPROC_PRINT_MODULE_ID, "Load OX03FSHDR4_ADAS IQ table", NULL, NULL, NULL, NULL, NULL);
            U32RVal |= AmbaWrap_memcpy(&IQTablePaths, &OX03FSHDR4_ADASPath, sizeof(IQ_TABLE_PATH_s));
            break;
        case IQ_SSR_MAX9295_9296_OX03F_EMR:
            AmbaPrint_ModulePrintStr5(IMGPROC_PRINT_MODULE_ID, "Load OX03FSHDR4_EMR IQ table", NULL, NULL, NULL, NULL, NULL);
            U32RVal |= AmbaWrap_memcpy(&IQTablePaths, &OX03FSHDR4_EMRPath, sizeof(IQ_TABLE_PATH_s));
            break;
        default:
            AmbaPrint_PrintStr5("<%s> Invalid sensorID", __func__, NULL, NULL, NULL, NULL);
            AmbaPrint_Flush();
            rVal = IMGPROC_ERR_0007;
            break;
    }
    if(U32RVal != 0U){
        /* MisraC */
    }
    return rVal;
}
#endif

static UINT32 IQParam_IQ_Table_Path_SHDR1(UINT32 sensorID)
{
    UINT32  rVal = 0U;
    UINT32  U32RVal = 0U;
    switch(sensorID) {
        case IQ_SSR_B6N_IMX390:
        case IQ_SSR_MAX9295_9296_IMX390:
        case IQ_SSR_MAX9295_96712_IMX390:
            AmbaPrint_ModulePrintStr5(IMGPROC_PRINT_MODULE_ID, "Load IMX390HDR3 IQ table", NULL, NULL, NULL, NULL, NULL);
            U32RVal |= AmbaWrap_memcpy(&IQTablePaths, &IMX390SHDR3Path, sizeof(IQ_TABLE_PATH_s));
            break;
        case IQ_SSR_MAX9295_9296_IMX390RCCB:
            AmbaPrint_ModulePrintStr5(IMGPROC_PRINT_MODULE_ID, "Load IMX390HDR3RCCB IQ table", NULL, NULL, NULL, NULL, NULL);
            U32RVal |= AmbaWrap_memcpy(&IQTablePaths, &IMX390SHDR3RCCBPath, sizeof(IQ_TABLE_PATH_s));
            break;
        // case IQ_SSR_B6N_IMX390_24:
        case IQ_SSR_IMX390_24:
        case IQ_SSR_MAX9295_9296_IMX390_24:
        case IQ_SSR_MAX9295_96712_IMX390_24:
        // case IQ_SSR_MAX9295_9288_IMX390_24:
            AmbaPrint_ModulePrintStr5(IMGPROC_PRINT_MODULE_ID, "Load IMX390HDR3_24 IQ table", NULL, NULL, NULL, NULL, NULL);
            U32RVal |= AmbaWrap_memcpy(&IQTablePaths, &IMX390SHDR3_24_Path, sizeof(IQ_TABLE_PATH_s));
            break;

        case IQ_SSR_B6N_IMX424:
        case IQ_SSR_MAX9295_9296_IMX424:
             AmbaPrint_ModulePrintStr5(IMGPROC_PRINT_MODULE_ID, "Load IMX424HDR3 IQ table", NULL, NULL, NULL, NULL, NULL);
             U32RVal |= AmbaWrap_memcpy(&IQTablePaths, &IMX424SHDR3Path, sizeof(IQ_TABLE_PATH_s));
             break;
        case IQ_SSR_IMX490:
            AmbaPrint_ModulePrintStr5(IMGPROC_PRINT_MODULE_ID, "Load IMX490HDR4 IQ table", NULL, NULL, NULL, NULL, NULL);
            U32RVal |= AmbaWrap_memcpy(&IQTablePaths, &IMX490SHDR4Path, sizeof(IQ_TABLE_PATH_s));
            break;
        // case IQ_SSR_TI953_954_AR0220:
        //     AmbaPrint_ModulePrintStr5(IMGPROC_PRINT_MODULE_ID, "Load AR0220HDR4 IQ table", NULL, NULL, NULL, NULL, NULL);
        //     U32RVal |= AmbaWrap_memcpy(&IQTablePaths, &AR0220SHDR4Path, sizeof(IQ_TABLE_PATH_s));
        //     break;
        // case IQ_SSR_TI953_960_AR0231:
        //     AmbaPrint_ModulePrintStr5(IMGPROC_PRINT_MODULE_ID, "Load AR0231 IQ table", NULL, NULL, NULL, NULL, NULL);
        //     U32RVal |= AmbaWrap_memcpy(&IQTablePaths, &AR0231Path, sizeof(IQ_TABLE_PATH_s));
        //     break;
        // case IQ_SSR_AR0147_SHDR3:
        //     AmbaPrint_ModulePrintStr5(IMGPROC_PRINT_MODULE_ID, "Load AR0147SHDR3 IQ table", NULL, NULL, NULL, NULL, NULL);
        //     U32RVal |= AmbaWrap_memcpy(&IQTablePaths, &AR0147SHDR3Path, sizeof(IQ_TABLE_PATH_s));
        //     break;

        case IQ_SSR_MAX_9295_9296_RCCB_AR0220:
            AmbaPrint_ModulePrintStr5(IMGPROC_PRINT_MODULE_ID, "Load AR0220SHDR3RCCB IQ table", NULL, NULL, NULL, NULL, NULL);
            U32RVal |= AmbaWrap_memcpy(&IQTablePaths, &AR0220SHDR3RCCBPath, sizeof(IQ_TABLE_PATH_s));
            break;
        case IQ_SSR_MAX_9295_9296_AR0820_RCCB:
            AmbaPrint_ModulePrintStr5(IMGPROC_PRINT_MODULE_ID, "Load AR0820SHDR3 RCCB IQ table", NULL, NULL, NULL, NULL, NULL);
            U32RVal |= AmbaWrap_memcpy(&IQTablePaths, &AR0820SHDR3RCCBPath, sizeof(IQ_TABLE_PATH_s));
            break;
        case IQ_SSR_IMX686:
            AmbaPrint_ModulePrintStr5(IMGPROC_PRINT_MODULE_ID, "Load IMX686SHDR3 IQ table", NULL, NULL, NULL, NULL, NULL);
            U32RVal |= AmbaWrap_memcpy(&IQTablePaths, &IMX686SHDR3Path, sizeof(IQ_TABLE_PATH_s));
            break;
        default:
            AmbaPrint_PrintStr5("<%s> Invalid sensorID", __func__, NULL, NULL, NULL, NULL);
            AmbaPrint_Flush();
            rVal = IMGPROC_ERR_0007;
            break;
    }
    if(U32RVal != 0U){
        /* MisraC */
    }
    return rVal;
}

static UINT32 IQParam_IQ_Table_Path_SHDR0(UINT32 sensorID)
{
    UINT32  rVal = 0U;
    UINT32  U32RVal = 0U;
    switch(sensorID) {
        case IQ_SSR_MAX9295_9296_AR0233:
        case IQ_SSR_TI953_954_AR0233:
        case IQ_SSR_MAX9295_96712_AR0233:
        case IQ_SSR_MAX96717_96716_AR0233:
            AmbaPrint_ModulePrintStr5(IMGPROC_PRINT_MODULE_ID, "Load AR0233SHDR2 IQ table", NULL, NULL, NULL, NULL, NULL);
            U32RVal |= AmbaWrap_memcpy(&IQTablePaths, &AR0233SHDR2Path, sizeof(IQ_TABLE_PATH_s));
            break;
        case IQ_SSR_TI913_954_OV10635:
            AmbaPrint_ModulePrintStr5(IMGPROC_PRINT_MODULE_ID, "Load OV10635SHDR2 IQ table", NULL, NULL, NULL, NULL, NULL);
            U32RVal |= AmbaWrap_memcpy(&IQTablePaths, &OV10635SHDR2Path, sizeof(IQ_TABLE_PATH_s));
            break;
        case IQ_SSR_MAX9295_9296_OV2778:
            AmbaPrint_ModulePrintStr5(IMGPROC_PRINT_MODULE_ID, "Load OV2778SHDR2 IQ table", NULL, NULL, NULL, NULL, NULL);
            U32RVal |= AmbaWrap_memcpy(&IQTablePaths, &OV2778SHDR2Path, sizeof(IQ_TABLE_PATH_s));
            break;
        case IQ_SENSOR_MAX9295_9296_VG1762:
            AmbaPrint_PrintStr5("Load VG1762SDHR2 IQ table", NULL, NULL, NULL, NULL, NULL);
            U32RVal |= AmbaWrap_memcpy(&IQTablePaths, &VG1762SHDR2Path, sizeof(IQ_TABLE_PATH_s));
            break;
        case IQ_SENSOR_MAX9295_9296_VG1762D:
            AmbaPrint_PrintStr5("Load VG1762DOUTPUT_SDHR2 IQ table", NULL, NULL, NULL, NULL, NULL);
            U32RVal |= AmbaWrap_memcpy(&IQTablePaths, &VG1762DOUTPUTSHDR2Path, sizeof(IQ_TABLE_PATH_s));
            break;
        case IQ_SENSOR_MAX9295_9296_VG1762DIR:
            AmbaPrint_PrintStr5("Load VG1762DOUTPUT_IR IQ table", NULL, NULL, NULL, NULL, NULL);
            U32RVal |= AmbaWrap_memcpy(&IQTablePaths, &VG1762DOUTPUTIRPath, sizeof(IQ_TABLE_PATH_s));
            break;
        case IQ_SSR_OV48C40:
            AmbaPrint_ModulePrintStr5(IMGPROC_PRINT_MODULE_ID, "Load OV48C40SHDR2 IQ table", NULL, NULL, NULL, NULL, NULL);
            U32RVal |= AmbaWrap_memcpy(&IQTablePaths, &OV48C40SHDR2Path, sizeof(IQ_TABLE_PATH_s));
            break;
        default:
            AmbaPrint_PrintStr5("<%s> Invalid sensorID", __func__, NULL, NULL, NULL, NULL);
            AmbaPrint_Flush();
            rVal = IMGPROC_ERR_0007;
            break;
    }
    /*Sensor HDR 2 exposures*/
    // if (sensorID == IQ_SSR_AR0147_SHDR2) {
    //     AmbaPrint_ModulePrintStr5(IMGPROC_PRINT_MODULE_ID, "Load AR0147SHDR2 IQ table", NULL, NULL, NULL, NULL, NULL);
    //     U32RVal |= AmbaWrap_memcpy(&IQTablePaths, &AR0147SHDR2Path, sizeof(IQ_TABLE_PATH_s));
    // }
    if(U32RVal != 0U){
        /* MisraC */
    }
    return rVal;
}

static UINT32 IQParam_IQ_Table_Path_DHDR0(UINT32 sensorID)
{
    UINT32  rVal = 0U;
    UINT32  U32RVal = 0U;
    switch(sensorID) {
        case IQ_SSR_IMX290:
            AmbaPrint_ModulePrintStr5(IMGPROC_PRINT_MODULE_ID, "Load IMX290HDR2 IQ table", NULL, NULL, NULL, NULL, NULL);
            U32RVal |= AmbaWrap_memcpy(&IQTablePaths, &IMX290HDR2Path, sizeof(IQ_TABLE_PATH_s));
            break;
        case IQ_SSR_IMX577:
        case IQ_SSR_IMX586:
        case IQ_SSR_IMX334:
            AmbaPrint_ModulePrintStr5(IMGPROC_PRINT_MODULE_ID, "Load IMX577HDR2 IQ table", NULL, NULL, NULL, NULL, NULL);
            U32RVal |= AmbaWrap_memcpy(&IQTablePaths, &IMX577HDR2Path, sizeof(IQ_TABLE_PATH_s));
            break;
        case IQ_SSR_AR0239:
        case IQ_SSR_MAX9295_9296_AR0239:
            AmbaPrint_ModulePrintStr5(IMGPROC_PRINT_MODULE_ID, "Load AR0239HDR2 IQ table", NULL, NULL, NULL, NULL, NULL);
            U32RVal |= AmbaWrap_memcpy(&IQTablePaths, &AR0239HDR2Path, sizeof(IQ_TABLE_PATH_s));
            break;
        case IQ_SSR_OV48C40:
            AmbaPrint_ModulePrintStr5(IMGPROC_PRINT_MODULE_ID, "Load OV48C40HDR2 IQ table", NULL, NULL, NULL, NULL, NULL);
            U32RVal |= AmbaWrap_memcpy(&IQTablePaths, &OV48C40HDR2Path, sizeof(IQ_TABLE_PATH_s));
            break;
        default:
            AmbaPrint_PrintStr5("<%s> Invalid sensorID", __func__, NULL, NULL, NULL, NULL);
            AmbaPrint_Flush();
            rVal = IMGPROC_ERR_0007;
            break;
    }
    if(U32RVal != 0U){
        /* MisraC */
    }
    return rVal;
}
static UINT32 IQParam_IQ_Table_Path_Normal1(UINT32 sensorID)
{
    UINT32  rVal = 0U;
    UINT32  U32RVal = 0U;
    switch(sensorID) {
        case IQ_SSR_AR0239:
        case IQ_SSR_MAX9295_9296_AR0239:
            AmbaPrint_ModulePrintStr5(IMGPROC_PRINT_MODULE_ID, "Load AR0239 IQ table", NULL, NULL, NULL, NULL, NULL);
            U32RVal |= AmbaWrap_memcpy(&IQTablePaths, &AR0239Path, sizeof(IQ_TABLE_PATH_s));
            break;
        case IQ_SSR_MAX9295_9296_AR0233:
            AmbaPrint_ModulePrintStr5(IMGPROC_PRINT_MODULE_ID, "Load AR0233 IQ table", NULL, NULL, NULL, NULL, NULL);
            U32RVal |= AmbaWrap_memcpy(&IQTablePaths, &AR0233Path, sizeof(IQ_TABLE_PATH_s));
            break;
        case IQ_SSR_MAX_9295_9296_AR0820_RCCB:
            AmbaPrint_ModulePrintStr5(IMGPROC_PRINT_MODULE_ID, "Load AR0820 RCCB IQ table", NULL, NULL, NULL, NULL, NULL);
            U32RVal |= AmbaWrap_memcpy(&IQTablePaths, &AR0820RCCBPath, sizeof(IQ_TABLE_PATH_s));
            break;
        case IQ_SSR_OV2312_SIMUL_RGBIR:
            AmbaPrint_ModulePrintStr5(IMGPROC_PRINT_MODULE_ID, "Load OV2312 RGBIR IQ table", NULL, NULL, NULL, NULL, NULL);
            U32RVal |= AmbaWrap_memcpy(&IQTablePaths, &OV2312SIMULRGBIRPath, sizeof(IQ_TABLE_PATH_s));
            break;
        case IQ_SSR_OV2312_SIMUL_IR:
            AmbaPrint_ModulePrintStr5(IMGPROC_PRINT_MODULE_ID, "Load OV2312 IR IQ table", NULL, NULL, NULL, NULL, NULL);
            U32RVal |= AmbaWrap_memcpy(&IQTablePaths, &OV2312SIMULIRPath, sizeof(IQ_TABLE_PATH_s));
            break;
        case IQ_SSR_OV2312_TD_RGBIR:
            AmbaPrint_ModulePrintStr5(IMGPROC_PRINT_MODULE_ID, "Load OV2312 RGBIR IQ table", NULL, NULL, NULL, NULL, NULL);
            U32RVal |= AmbaWrap_memcpy(&IQTablePaths, &OV2312TDRGBIRPath, sizeof(IQ_TABLE_PATH_s));
            break;
        case IQ_SSR_OV2312_TD_IR:
            AmbaPrint_ModulePrintStr5(IMGPROC_PRINT_MODULE_ID, "Load OV2312 IR IQ table", NULL, NULL, NULL, NULL, NULL);
            U32RVal |= AmbaWrap_memcpy(&IQTablePaths, &OV2312TDIRPath, sizeof(IQ_TABLE_PATH_s));
            break;
        case IQ_SSR_MAX9295_96712_IMX224:
        case IQ_SSR_MAX_9295_9296_IMX224:
            AmbaPrint_ModulePrintStr5(IMGPROC_PRINT_MODULE_ID, "Load IMX224 IQ table", NULL, NULL, NULL, NULL, NULL);
            U32RVal |= AmbaWrap_memcpy(&IQTablePaths, &IMX224Path, sizeof(IQ_TABLE_PATH_s));
            break;
        case IQ_SSR_IMX490:
            AmbaPrint_ModulePrintStr5(IMGPROC_PRINT_MODULE_ID, "Load IMX490 IQ table", NULL, NULL, NULL, NULL, NULL);
            U32RVal |= AmbaWrap_memcpy(&IQTablePaths, &IMX490Path, sizeof(IQ_TABLE_PATH_s));
            break;
        case IQ_SSR_MAX9295_9296_OV2778:
            AmbaPrint_ModulePrintStr5(IMGPROC_PRINT_MODULE_ID, "Load OV2778 IQ table", NULL, NULL, NULL, NULL, NULL);
            U32RVal |= AmbaWrap_memcpy(&IQTablePaths, &OV2778Path, sizeof(IQ_TABLE_PATH_s));
            break;
        case IQ_SSR_OV48C40:
            AmbaPrint_ModulePrintStr5(IMGPROC_PRINT_MODULE_ID, "Load OV48C40 IQ table", NULL, NULL, NULL, NULL, NULL);
            U32RVal |= AmbaWrap_memcpy(&IQTablePaths, &OV48C40Path, sizeof(IQ_TABLE_PATH_s));
            break;
        case IQ_SSR_IMX455:
            AmbaPrint_ModulePrintStr5(IMGPROC_PRINT_MODULE_ID, "Load IMX455 IQ table", NULL, NULL, NULL, NULL, NULL);
            U32RVal |= AmbaWrap_memcpy(&IQTablePaths, &IMX455Path, sizeof(IQ_TABLE_PATH_s));
            break;
        case IQ_SSR_OX05B1S:
            AmbaPrint_ModulePrintStr5(IMGPROC_PRINT_MODULE_ID, "Load OX05B1S IQ table", NULL, NULL, NULL, NULL, NULL);
            U32RVal |= AmbaWrap_memcpy(&IQTablePaths, &OX05B1SPath, sizeof(IQ_TABLE_PATH_s));
            break;
        // case IQ_SSR_TI953_954_AR0220:
        //     AmbaPrint_ModulePrintStr5(IMGPROC_PRINT_MODULE_ID, "Load AR0220 IQ table", NULL, NULL, NULL, NULL, NULL);
        //     U32RVal |= AmbaWrap_memcpy(&IQTablePaths, &AR0220Path, sizeof(IQ_TABLE_PATH_s));
        //     break;
        // case IQ_SSR_TI953_960_AR0231:
        //     AmbaPrint_ModulePrintStr5(IMGPROC_PRINT_MODULE_ID, "Load AR0231 IQ table", NULL, NULL, NULL, NULL, NULL);
        //     U32RVal |= AmbaWrap_memcpy(&IQTablePaths, &AR0231Path, sizeof(IQ_TABLE_PATH_s));
        //     break;
        default:
            AmbaPrint_PrintStr5("<%s> Invalid sensorID", __func__, NULL, NULL, NULL, NULL);
            AmbaPrint_Flush();
            rVal = IMGPROC_ERR_0007;
            break;
    }
    if(U32RVal != 0U){
        /* MisraC */
    }
    return rVal;
}
static UINT32 IQParam_IQ_Table_Path_Normal(UINT32 sensorID)
{
    UINT32  rVal = 0U;
    UINT32  U32RVal = 0U;
    switch(sensorID) {
        // case IQ_SSR_MAX9295_9296_AR0144:
        //     AmbaPrint_ModulePrintStr5(IMGPROC_PRINT_MODULE_ID, "Load AR0144 IQ table", NULL, NULL, NULL, NULL, NULL);
        //     U32RVal |= AmbaWrap_memcpy(&IQTablePaths, &AR0144Path, sizeof(IQ_TABLE_PATH_s));
        //     break;
        //case IQ_SSR_MAX96705_9286_AR0144:
        case IQ_SSR_MAX96707_9286_AR0144:
            AmbaPrint_ModulePrintStr5(IMGPROC_PRINT_MODULE_ID, "Load AR0144 IQ table", NULL, NULL, NULL, NULL, NULL);
            U32RVal |= AmbaWrap_memcpy(&IQTablePaths, &AR0144Path, sizeof(IQ_TABLE_PATH_s));
            break;
        // case IQ_SSR_MAX96705_9286_OV9716:
        //     AmbaPrint_ModulePrintStr5(IMGPROC_PRINT_MODULE_ID, "Load OV9716 IQ table", NULL, NULL, NULL, NULL, NULL);
        //     U32RVal |= AmbaWrap_memcpy(&IQTablePaths, &OV9716Path, sizeof(IQ_TABLE_PATH_s));
        //     break;
        case IQ_SSR_IMX183:
            AmbaPrint_ModulePrintStr5(IMGPROC_PRINT_MODULE_ID, "Load IMX183 IQ table", NULL, NULL, NULL, NULL, NULL);
            U32RVal |= AmbaWrap_memcpy(&IQTablePaths, &IMX183Path, sizeof(IQ_TABLE_PATH_s));
            break;
        // case IQ_SSR_IMX334:
        //     AmbaPrint_ModulePrintStr5(IMGPROC_PRINT_MODULE_ID, "Load IMX334 IQ table", NULL, NULL, NULL, NULL, NULL);
        //     U32RVal |= AmbaWrap_memcpy(&IQTablePaths, &IMX334Path, sizeof(IQ_TABLE_PATH_s));
        //     break;
        case IQ_SSR_IMX377:
            AmbaPrint_ModulePrintStr5(IMGPROC_PRINT_MODULE_ID, "Load IMX377 IQ table", NULL, NULL, NULL, NULL, NULL);
            U32RVal |= AmbaWrap_memcpy(&IQTablePaths, &IMX377Path, sizeof(IQ_TABLE_PATH_s));
            break;
        case IQ_SSR_IMX377Q:
            AmbaPrint_ModulePrintStr5(IMGPROC_PRINT_MODULE_ID, "Load IMX377Q IQ table", NULL, NULL, NULL, NULL, NULL);
            U32RVal |= AmbaWrap_memcpy(&IQTablePaths, &IMX377Path, sizeof(IQ_TABLE_PATH_s));
            break;
        case IQ_SSR_IMX290:
            AmbaPrint_ModulePrintStr5(IMGPROC_PRINT_MODULE_ID, "Load IMX290 IQ table", NULL, NULL, NULL, NULL, NULL);
            U32RVal |= AmbaWrap_memcpy(&IQTablePaths, &IMX290Path, sizeof(IQ_TABLE_PATH_s));
            break;
        case IQ_SSR_B6N_IMX390:
            AmbaPrint_ModulePrintStr5(IMGPROC_PRINT_MODULE_ID, "Load IMX390 IQ table", NULL, NULL, NULL, NULL, NULL);
            U32RVal |= AmbaWrap_memcpy(&IQTablePaths, &IMX390Path, sizeof(IQ_TABLE_PATH_s));
            break;
        case IQ_SSR_IMX577:
        case IQ_SSR_IMX334:
            AmbaPrint_ModulePrintStr5(IMGPROC_PRINT_MODULE_ID, "Load IMX577 IQ table", NULL, NULL, NULL, NULL, NULL);
            U32RVal |= AmbaWrap_memcpy(&IQTablePaths, &IMX577Path, sizeof(IQ_TABLE_PATH_s));
            break;
        case IQ_SSR_IMX586:
            AmbaPrint_ModulePrintStr5(IMGPROC_PRINT_MODULE_ID, "Load IMX586 IQ table", NULL, NULL, NULL, NULL, NULL);
            U32RVal |= AmbaWrap_memcpy(&IQTablePaths, &IMX586Path, sizeof(IQ_TABLE_PATH_s));
            break;
        case IQ_SSR_IMX686:
            AmbaPrint_ModulePrintStr5(IMGPROC_PRINT_MODULE_ID, "Load IMX686 IQ table", NULL, NULL, NULL, NULL, NULL);
            U32RVal |= AmbaWrap_memcpy(&IQTablePaths, &IMX686Path, sizeof(IQ_TABLE_PATH_s));
            break;
        case IQ_SSR_K351P:
            AmbaPrint_ModulePrintStr5(IMGPROC_PRINT_MODULE_ID, "Load K351P IQ table", NULL, NULL, NULL, NULL, NULL);
            U32RVal |= AmbaWrap_memcpy(&IQTablePaths, &K351PPath, sizeof(IQ_TABLE_PATH_s));
            break;
        case IQ_SSR_IMX415:
            AmbaPrint_ModulePrintStr5(IMGPROC_PRINT_MODULE_ID, "Load IMX415 IQ table", NULL, NULL, NULL, NULL, NULL);
            U32RVal |= AmbaWrap_memcpy(&IQTablePaths, &IMX415Path, sizeof(IQ_TABLE_PATH_s));
            break;
        case IQ_SSR_GC2053:
            AmbaPrint_ModulePrintStr5(IMGPROC_PRINT_MODULE_ID, "Load GC2053 IQ table", NULL, NULL, NULL, NULL, NULL);
            U32RVal |= AmbaWrap_memcpy(&IQTablePaths, &GC2053Path, sizeof(IQ_TABLE_PATH_s));
            break;
        case IQ_SSR_GC4653:
            AmbaPrint_ModulePrintStr5(IMGPROC_PRINT_MODULE_ID, "Load GC4653 IQ table", NULL, NULL, NULL, NULL, NULL);
            U32RVal |= AmbaWrap_memcpy(&IQTablePaths, &GC4653Path, sizeof(IQ_TABLE_PATH_s));
            break;
        default:
            rVal = IQParam_IQ_Table_Path_Normal1(sensorID);
            break;
    }
    if(U32RVal != 0U){
        /* MisraC */
    }
    return rVal;
}

UINT32 Amba_IQParam_Config_IQ_Table_Path(UINT32 sensorID, UINT32 HdrEnable)
{
    UINT32  rVal = 0U;
    UINT32  U32RVal = 0U;

    AmbaPrint_ModulePrintUInt5(IMGPROC_PRINT_MODULE_ID, "Amba_IQParam_Config_IQ_Table_Path (HdrEnable: %d)", HdrEnable, 0U, 0U, 0U, 0U);

    if (HdrEnable == IMG_NORMAL_MODE) {
        /*Without HDR*/
        rVal = IQParam_IQ_Table_Path_Normal(sensorID);
    } else if(HdrEnable == IMG_DSP_HDR_MODE_0) {
        /*DSP HDR 2 exposures*/
        rVal = IQParam_IQ_Table_Path_DHDR0(sensorID);
    } else if(HdrEnable == IMG_DSP_HDR_MODE_1) {
        /*DSP HDR 3 exposures*/

    } else if(HdrEnable == IMG_SENSOR_HDR_MODE_0) {
        rVal = IQParam_IQ_Table_Path_SHDR0(sensorID);
    } else if(HdrEnable == IMG_SENSOR_HDR_MODE_1) {
        /*Sensor HDR 3 exposures*/
        rVal = IQParam_IQ_Table_Path_SHDR1(sensorID);
#if defined(IMG_SENSOR_HDR_MODE_2)
    } else if (HdrEnable == IMG_SENSOR_HDR_MODE_2) {
        /*Sensor HDR 4 exposures*/
        rVal = IQParam_IQ_Table_Path_SHDR2(sensorID);
#endif
    } else {
        /*Invalid HDR mode*/
        AmbaPrint_ModulePrintStr5(IMGPROC_PRINT_MODULE_ID, "<%s>  Invalid HDR mode", __func__, NULL, NULL, NULL, NULL);
        AmbaPrint_ModulePrintStr5(IMGPROC_PRINT_MODULE_ID, "Load IMX377 IQ table", NULL, NULL, NULL, NULL, NULL);
        U32RVal |= AmbaWrap_memcpy(&IQTablePaths, &IMX377Path, sizeof(IQ_TABLE_PATH_s));
    }

    if (IQTablePathExtraProc != NULL) {
        AmbaPrint_ModulePrintStr5(IMGPROC_PRINT_MODULE_ID, "Load Extra IQ table", NULL, NULL, NULL, NULL, NULL);
        IQTablePathExtraProc(&IQTablePaths);
    }

#if !defined(CONFIG_SOC_CV2FS) && !defined(CONFIG_SOC_CV22FS)&& !defined(CONFIG_SOC_CV5)&& !defined(CONFIG_SOC_CV52)
    if (IQTableDspPathExtraProc != NULL) {
        AmbaPrint_ModulePrintStr5(IMGPROC_PRINT_MODULE_ID, "Load Extra IQ DSP table", NULL, NULL, NULL, NULL, NULL);
        IQTableDspPathExtraProc(&IQTableDspPaths);
    }
#endif
    if(U32RVal != 0U){
        /* MisraC */
    }
    rVal |= U32RVal;
    if(rVal != 0U){
        /* MisraC */
    }
    return rVal;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////

static UINT32 LoadFileData(const void *pDestBuff, const char *pPath)
{
    UINT32 RetVal = 0U, PRetVal = 0U;

    if (pDestBuff == NULL) {
        RetVal = IMGPROC_ERR_0007;
    }

    if (pPath == NULL) {
        RetVal = IMGPROC_ERR_0007;
    }

    if (RetVal == 0U) {
        AMBA_FS_FILE *pFile = NULL;
        AMBA_FS_FILE_INFO_s FileInfo;

        PRetVal |= AmbaWrap_memset(&FileInfo, 0, sizeof(FileInfo)); AmbaMisra_TouchUnused(&PRetVal);
        PRetVal |= AmbaFS_GetFileInfo(pPath, &FileInfo);
        if (PRetVal != 0U) {
            RetVal = IMGPROC_ERR_0007;
            AmbaPrint_ModulePrintStr5(IMGPROC_PRINT_MODULE_ID, "Get IQ table file size fail - %s", pPath, NULL, NULL, NULL, NULL);
        } else {
            UINT32 FileSize = (UINT32)(FileInfo.Size);
            UINT32 RetSize;

            if (FileSize == 0U) {
                RetVal = IMGPROC_ERR_0007;
                AmbaPrint_ModulePrintStr5(IMGPROC_PRINT_MODULE_ID, "Get IQ table file size should not zero - %s", pPath, NULL, NULL, NULL, NULL);
            } else {
                PRetVal = AmbaFS_FileOpen(pPath, "rb", &pFile);
                if (PRetVal != 0U) {
                    RetVal = IMGPROC_ERR_0007;
                    AmbaPrint_ModulePrintStr5(IMGPROC_PRINT_MODULE_ID, "Open IQ table fail - %s", pPath, NULL, NULL, NULL, NULL);
                } else {
                    void *pBuf;
                    PRetVal = AmbaWrap_memcpy(&(pBuf), &(pDestBuff), sizeof(void *));
                    PRetVal = AmbaWrap_memset(pBuf, 0, FileSize); AmbaMisra_TouchUnused(&PRetVal);
                    PRetVal = AmbaFS_FileRead(pBuf, FileSize, 1, pFile, &RetSize); AmbaMisra_TouchUnused(&RetSize); AmbaMisra_TouchUnused(&PRetVal);
                    PRetVal = AmbaFS_FileClose(pFile); AmbaMisra_TouchUnused(&PRetVal);
                }
            }
        }
    }
    if(PRetVal==0U){
        //misraC
    }

    return RetVal;
}

/*Callbacks for app layer*/
UINT32 Amba_IQParam_RegisterIQPathProc(AMBA_IQ_PARAM_TABLE_PATH_f IqTblPathProc)
{
    IQTablePathExtraProc = IqTblPathProc;

    return 0U;
}

UINT32 Amba_IQParam_RegisterReadRomFile(READ_ROM_FILE pFunc)
{
    ReadRomFileWrap = pFunc;

    return 0U;
}

#if !defined(CONFIG_SOC_CV2FS) && !defined(CONFIG_SOC_CV22FS)&& !defined(CONFIG_SOC_CV5)&& !defined(CONFIG_SOC_CV52)
UINT32 Amba_IQParam_RegisterIQDspPathProc(AMBA_IQ_PARAM_TABLE_DSP_PATH_f IqTblPathProc)
{
    IQTableDspPathExtraProc = IqTblPathProc;

    return 0U;
}
#endif


UINT32 Amba_IQParam_QueryIqBufSize(UINT32 *pSize)
{
    if (pSize != NULL) {
        *pSize = (UINT32)sizeof(IQ_PARAM_s);
    }

    return 0U;
}

UINT32 Amba_IQParam_SetIqBuf(UINT32 viewID,const void *pBuf)
{
    IQ_PARAM_s *pIq = NULL;

    if (viewID >= MAX_VIEW_NUM) {
        AmbaPrint_ModulePrintUInt5(IMGPROC_PRINT_MODULE_ID, "Amba_IQParam_SetIqBuf : Invalid viewID[%d]", viewID, 0U, 0U, 0U, 0U);
    } else if (pBuf == NULL) {
        AmbaPrint_ModulePrintUInt5(IMGPROC_PRINT_MODULE_ID, "Amba_IQParam_SetIqBuf : Null Buf", 0U, 0U, 0U, 0U, 0U);
    } else {
        AmbaMisra_TypeCast(&pIq, &pBuf);
        pIQParamBuf[viewID] = pIq;
    }

    return 0U;
}
//Complexity
static UINT32 IQParam_Path_Prt_Sub1111(UINT32 sensorID)
{
    UINT32 RVal = 0U;
    switch(sensorID) {
        case IQ_SSR_IMX455:
            AmbaPrint_PrintStr5("<%s>", IMX455Path.ImgParam.path , NULL, NULL, NULL, NULL);
            break;
        case IQ_SSR_IMX586:
            AmbaPrint_PrintStr5("<%s>", IMX586Path.ImgParam.path , NULL, NULL, NULL, NULL);
            AmbaPrint_PrintStr5("<%s>", IMX586Path.ImgParam.path , NULL, NULL, NULL, NULL);
            AmbaPrint_PrintStr5("<%s>", IMX586Path.ImgParam.path , NULL, NULL, NULL, NULL);
            break;
        case IQ_SSR_IMX686:
            AmbaPrint_PrintStr5("<%s>", IMX686Path.ImgParam.path , NULL, NULL, NULL, NULL);
            AmbaPrint_PrintStr5("<%s>", IMX686SHDR3Path.ImgParam.path , NULL, NULL, NULL, NULL);
            break;
        case IQ_SSR_OV48C40:
            AmbaPrint_PrintStr5("<%s>", OV48C40Path.ImgParam.path , NULL, NULL, NULL, NULL);
            AmbaPrint_PrintStr5("<%s>", OV48C40SHDR2Path.ImgParam.path , NULL, NULL, NULL, NULL);
            AmbaPrint_PrintStr5("<%s>", OV48C40HDR2Path.ImgParam.path , NULL, NULL, NULL, NULL);

            break;
        case IQ_SSR_MAX9295_9296_OX03C:
            AmbaPrint_PrintStr5("<%s>", OX03CSHDR4Path.ImgParam.path , NULL, NULL, NULL, NULL);
            AmbaPrint_PrintStr5("<%s>", OX03CSHDR4Path.ImgParam.path , NULL, NULL, NULL, NULL);
            AmbaPrint_PrintStr5("<%s>", OX03CSHDR4Path.ImgParam.path , NULL, NULL, NULL, NULL);
            break;
        case IQ_SSR_MAX9295_9296_OX03F:
            AmbaPrint_PrintStr5("<%s>", OX03FSHDR4Path.ImgParam.path , NULL, NULL, NULL, NULL);
            AmbaPrint_PrintStr5("<%s>", OX03FSHDR4Path.ImgParam.path , NULL, NULL, NULL, NULL);
            AmbaPrint_PrintStr5("<%s>", OX03FSHDR4Path.ImgParam.path , NULL, NULL, NULL, NULL);
            break;
        case IQ_SSR_MAX9295_9296_OX03C_ADAS:
            AmbaPrint_PrintStr5("<%s>", OX03CSHDR4_ADASPath.ImgParam.path , NULL, NULL, NULL, NULL);
            AmbaPrint_PrintStr5("<%s>", OX03CSHDR4_ADASPath.ImgParam.path , NULL, NULL, NULL, NULL);
            AmbaPrint_PrintStr5("<%s>", OX03CSHDR4_ADASPath.ImgParam.path , NULL, NULL, NULL, NULL);
            break;
        case IQ_SSR_MAX9295_9296_OX03C_EMR:
            AmbaPrint_PrintStr5("<%s>", OX03CSHDR4_EMRPath.ImgParam.path , NULL, NULL, NULL, NULL);
            AmbaPrint_PrintStr5("<%s>", OX03CSHDR4_EMRPath.ImgParam.path , NULL, NULL, NULL, NULL);
            AmbaPrint_PrintStr5("<%s>", OX03CSHDR4_EMRPath.ImgParam.path , NULL, NULL, NULL, NULL);
            break;
        case IQ_SSR_MAX9295_9296_OX03F_ADAS:
            AmbaPrint_PrintStr5("<%s>", OX03FSHDR4_ADASPath.ImgParam.path , NULL, NULL, NULL, NULL);
            AmbaPrint_PrintStr5("<%s>", OX03FSHDR4_ADASPath.ImgParam.path , NULL, NULL, NULL, NULL);
            AmbaPrint_PrintStr5("<%s>", OX03FSHDR4_ADASPath.ImgParam.path , NULL, NULL, NULL, NULL);
            break;
        case IQ_SSR_MAX9295_9296_OX03F_EMR:
            AmbaPrint_PrintStr5("<%s>", OX03FSHDR4_EMRPath.ImgParam.path , NULL, NULL, NULL, NULL);
            AmbaPrint_PrintStr5("<%s>", OX03FSHDR4_EMRPath.ImgParam.path , NULL, NULL, NULL, NULL);
            AmbaPrint_PrintStr5("<%s>", OX03FSHDR4_EMRPath.ImgParam.path , NULL, NULL, NULL, NULL);
            break;
        case IQ_SSR_IMX728:
            AmbaPrint_PrintStr5("<%s>", IMX728SHDR4Path.ImgParam.path , NULL, NULL, NULL, NULL);
            AmbaPrint_PrintStr5("<%s>", IMX728SHDR4Path.ImgParam.path , NULL, NULL, NULL, NULL);
            AmbaPrint_PrintStr5("<%s>", IMX728SHDR4Path.ImgParam.path , NULL, NULL, NULL, NULL);
            break;
        case IQ_SSR_OX05B1S:
            AmbaPrint_PrintStr5("<%s>", OX05B1SPath.ImgParam.path , NULL, NULL, NULL, NULL);
            AmbaPrint_PrintStr5("<%s>", OX05B1SPath.ImgParam.path , NULL, NULL, NULL, NULL);
            AmbaPrint_PrintStr5("<%s>", OX05B1SPath.ImgParam.path , NULL, NULL, NULL, NULL);
            break;
        default:
            AmbaPrint_PrintStr5("<%s> Invalid sensorID", __func__, NULL, NULL, NULL, NULL);
            AmbaPrint_Flush();
            RVal = IMGPROC_ERR_0007;
            break;
    }
    return RVal;
}
static UINT32 IQParam_Path_Prt_Sub111(UINT32 sensorID)
{
    UINT32 RVal = 0U;
    switch(sensorID) {
        case IQ_SSR_MAX_9295_9296_AR0820:
        case IQ_SSR_MAX_9295_9296_AR0820_RCCB:
            AmbaPrint_PrintStr5("<%s>", AR0820RCCBPath.ImgParam.path , NULL, NULL, NULL, NULL);
            AmbaPrint_PrintStr5("<%s>", AR0820SHDR3RCCBPath.ImgParam.path , NULL, NULL, NULL, NULL);
            break;
        case IQ_SSR_OV2312_TD_RGBIR:
        case IQ_SSR_OV2312_TD_IR:
        case IQ_SSR_OV2312_SIMUL_RGBIR:
        case IQ_SSR_OV2312_SIMUL_IR:
            AmbaPrint_PrintStr5("<%s>", OV2312SIMULRGBIRPath.ImgParam.path , NULL, NULL, NULL, NULL);
            AmbaPrint_PrintStr5("<%s>", OV2312SIMULIRPath.ImgParam.path , NULL, NULL, NULL, NULL);
            AmbaPrint_PrintStr5("<%s>", OV2312TDRGBIRPath.ImgParam.path , NULL, NULL, NULL, NULL);
            AmbaPrint_PrintStr5("<%s>", OV2312TDIRPath.ImgParam.path , NULL, NULL, NULL, NULL);
            break;
        case IQ_SSR_TI913_954_OV10635:
            AmbaPrint_PrintStr5("<%s>", OV10635SHDR2Path.ImgParam.path , NULL, NULL, NULL, NULL);
            break;
        case IQ_SSR_MAX9295_9296_OV2778:
            AmbaPrint_PrintStr5("<%s>", OV2778Path.ImgParam.path , NULL, NULL, NULL, NULL);
            AmbaPrint_PrintStr5("<%s>", OV2778SHDR2Path.ImgParam.path , NULL, NULL, NULL, NULL);
            break;
        case IQ_SENSOR_MAX9295_9296_VG1762:
        case IQ_SENSOR_MAX9295_9296_VG1762D:
        case IQ_SENSOR_MAX9295_9296_VG1762DIR:
            AmbaPrint_PrintStr5("<%s>", VG1762SHDR2Path.ImgParam.path , NULL, NULL, NULL, NULL);
            AmbaPrint_PrintStr5("<%s>", VG1762DOUTPUTSHDR2Path.ImgParam.path , NULL, NULL, NULL, NULL);
            AmbaPrint_PrintStr5("<%s>", VG1762DOUTPUTIRPath.ImgParam.path , NULL, NULL, NULL, NULL);
            break;
        default:
            RVal = IQParam_Path_Prt_Sub1111(sensorID);
            break;
    }
    return RVal;
}
static UINT32 IQParam_Path_Prt_Sub11(UINT32 sensorID)
{
    UINT32 RVal = 0U;
    switch(sensorID) {
        case IQ_SSR_MAX9295_96712_IMX224:
        case IQ_SSR_MAX_9295_9296_IMX224:
            AmbaPrint_PrintStr5("<%s>", IMX224Path.ImgParam.path , NULL, NULL, NULL, NULL);
            break;
        case IQ_SSR_IMX490:
        case IQ_SSR_MAX9295_9296_IMX490:
            AmbaPrint_PrintStr5("<%s>", IMX490Path.ImgParam.path , NULL, NULL, NULL, NULL);
            AmbaPrint_PrintStr5("<%s>", IMX490SHDR4Path.ImgParam.path , NULL, NULL, NULL, NULL);
            break;
        case IQ_SSR_MAX9295_9296_AR0233:
        case IQ_SSR_TI953_954_AR0233:
        case IQ_SSR_MAX9295_96712_AR0233:
        case IQ_SSR_MAX96717_96716_AR0233:
            AmbaPrint_PrintStr5("<%s>", AR0233Path.ImgParam.path , NULL, NULL, NULL, NULL);
            AmbaPrint_PrintStr5("<%s>", AR0233SHDR2Path.ImgParam.path , NULL, NULL, NULL, NULL);
            break;
        case IQ_SSR_MAX_9295_9296_RCCB_AR0220:
        case IQ_SSR_MAX_9295_9296_RCCC_AR0220:
        case IQ_SSR_TI953_954_AR0220_RCCB:
            AmbaPrint_PrintStr5("<%s>", AR0220SHDR3RCCBPath.ImgParam.path , NULL, NULL, NULL, NULL);
            AmbaPrint_PrintStr5("<%s>", AR0220SHDR4RCCBPath.ImgParam.path , NULL, NULL, NULL, NULL);
            break;
        default:
            RVal = IQParam_Path_Prt_Sub111(sensorID);
            break;
    }
    return RVal;
}
static UINT32 IQParam_Path_Prt_Sub1(UINT32 sensorID)
{
    UINT32 RVal = 0U;
    switch(sensorID) {
        case IQ_SSR_IMX390_24:
        case IQ_SSR_B6N_IMX390:
        case IQ_SSR_MAX9295_9296_IMX390:
        case IQ_SSR_MAX9295_96712_IMX390:
        case IQ_SSR_B6N_IMX390_RCCB:
        case IQ_SSR_MAX9295_9296_IMX390_24:
        case IQ_SSR_MAX9295_96712_IMX390_24:
        case IQ_SSR_MAX9295_9296_IMX390RCCB:
            AmbaPrint_PrintStr5("<%s>", IMX390Path.ImgParam.path , NULL, NULL, NULL, NULL);
            AmbaPrint_PrintStr5("<%s>", IMX390SHDR3Path.ImgParam.path , NULL, NULL, NULL, NULL);
            AmbaPrint_PrintStr5("<%s>", IMX390SHDR3RCCBPath.ImgParam.path , NULL, NULL, NULL, NULL);
            AmbaPrint_PrintStr5("<%s>", IMX390SHDR3_24_Path.ImgParam.path , NULL, NULL, NULL, NULL);
            break;
        case IQ_SSR_B6N_IMX424:
        case IQ_SSR_MAX9295_9296_IMX424:
            AmbaPrint_PrintStr5("<%s>", IMX424SHDR3Path.ImgParam.path , NULL, NULL, NULL, NULL);
            break;
        case IQ_SSR_AR0239:
        case IQ_SSR_MAX9295_9296_AR0239:
            AmbaPrint_PrintStr5("<%s>", AR0239Path.ImgParam.path , NULL, NULL, NULL, NULL);
            AmbaPrint_PrintStr5("<%s>", AR0239HDR2Path.ImgParam.path , NULL, NULL, NULL, NULL);
            break;
        default:
            RVal = IQParam_Path_Prt_Sub11(sensorID);
            break;
    }
    return RVal;
}
//
UINT32 Amba_IQParam_IQ_Table_Path_Prt(UINT32 sensorID)
{
    UINT32 RVal = 0U;
    switch(sensorID) {
#if 0
        case IQ_SSR_IMX377:
        case IQ_SSR_IMX377Q:
        case IQ_SSR_IMX377D:
            AmbaPrint_PrintStr5("<%s>", IMX377Path.ImgParam.path , NULL, NULL, NULL, NULL);
            break;
        case IQ_SSR_IMX183:
            AmbaPrint_PrintStr5("<%s>", IMX183Path.ImgParam.path , NULL, NULL, NULL, NULL);
            break;
        case IQ_SSR_MAX96707_9286_AR0144:
            AmbaPrint_PrintStr5("<%s>", AR0144Path.ImgParam.path , NULL, NULL, NULL, NULL);
            break;
        case IQ_SSR_IMX290:
        case IQ_SSR_B6N_IMX290:
            AmbaPrint_PrintStr5("<%s>", IMX290Path.ImgParam.path , NULL, NULL, NULL, NULL);
            AmbaPrint_PrintStr5("<%s>", IMX290HDR2Path.ImgParam.path , NULL, NULL, NULL, NULL);
            break;
        case IQ_SSR_IMX577:
            AmbaPrint_PrintStr5("<%s>", IMX577Path.ImgParam.path , NULL, NULL, NULL, NULL);
            AmbaPrint_PrintStr5("<%s>", IMX577HDR2Path.ImgParam.path , NULL, NULL, NULL, NULL);
            break;
        case IQ_SSR_B6N_IMX390:
        case IQ_SSR_MAX9295_9296_IMX390:
        case IQ_SSR_MAX9295_96712_IMX390:
        case IQ_SSR_B6N_IMX390_RCCB:
        case IQ_SSR_MAX9295_9296_IMX390_24:
        case IQ_SSR_MAX9295_96712_IMX390_24:
        case IQ_SSR_MAX9295_9296_IMX390RCCB:
            AmbaPrint_PrintStr5("<%s>", IMX390Path.ImgParam.path , NULL, NULL, NULL, NULL);
            AmbaPrint_PrintStr5("<%s>", IMX390SHDR3Path.ImgParam.path , NULL, NULL, NULL, NULL);
            AmbaPrint_PrintStr5("<%s>", IMX390SHDR3RCCBPath.ImgParam.path , NULL, NULL, NULL, NULL);
            AmbaPrint_PrintStr5("<%s>", IMX390SHDR3_24_Path.ImgParam.path , NULL, NULL, NULL, NULL);
            break;
        case IQ_SSR_B6N_IMX424:
        case IQ_SSR_MAX9295_9296_IMX424:
            AmbaPrint_PrintStr5("<%s>", IMX424SHDR3Path.ImgParam.path , NULL, NULL, NULL, NULL);
            break;
        case IQ_SSR_AR0239:
        case IQ_SSR_MAX9295_9296_AR0239:
            AmbaPrint_PrintStr5("<%s>", AR0239Path.ImgParam.path , NULL, NULL, NULL, NULL);
            AmbaPrint_PrintStr5("<%s>", AR0239HDR2Path.ImgParam.path , NULL, NULL, NULL, NULL);
            break;
        case IQ_SSR_MAX9295_96712_IMX224:
        case IQ_SSR_MAX_9295_9296_IMX224:
            AmbaPrint_PrintStr5("<%s>", IMX224Path.ImgParam.path , NULL, NULL, NULL, NULL);
            break;
        case IQ_SSR_IMX490:
        case IQ_SSR_MAX9295_9296_IMX490:
            AmbaPrint_PrintStr5("<%s>", IMX490Path.ImgParam.path , NULL, NULL, NULL, NULL);
            AmbaPrint_PrintStr5("<%s>", IMX490SHDR4Path.ImgParam.path , NULL, NULL, NULL, NULL);
            break;
        case IQ_SSR_MAX9295_9296_AR0233:
        case IQ_SSR_TI953_954_AR0233:
        case IQ_SSR_MAX9295_96712_AR0233:
        case IQ_SSR_MAX96717_96716_AR0233:
            AmbaPrint_PrintStr5("<%s>", AR0233Path.ImgParam.path , NULL, NULL, NULL, NULL);
            AmbaPrint_PrintStr5("<%s>", AR0233SHDR2Path.ImgParam.path , NULL, NULL, NULL, NULL);
            break;
        case IQ_SSR_MAX_9295_9296_RCCB_AR0220:
        case IQ_SSR_MAX_9295_9296_RCCC_AR0220:
        case IQ_SSR_TI953_954_AR0220_RCCB:
            AmbaPrint_PrintStr5("<%s>", AR0220SHDR3RCCBPath.ImgParam.path , NULL, NULL, NULL, NULL);
            AmbaPrint_PrintStr5("<%s>", AR0220SHDR4RCCBPath.ImgParam.path , NULL, NULL, NULL, NULL);
            break;
        case IQ_SSR_MAX_9295_9296_AR0820:
        case IQ_SSR_MAX_9295_9296_AR0820_RCCB:
            AmbaPrint_PrintStr5("<%s>", AR0820RCCBPath.ImgParam.path , NULL, NULL, NULL, NULL);
            AmbaPrint_PrintStr5("<%s>", AR0820SHDR3RCCBPath.ImgParam.path , NULL, NULL, NULL, NULL);
            break;
        case IQ_SSR_OV2312_TD_RGBIR:
        case IQ_SSR_OV2312_TD_IR:
        case IQ_SSR_OV2312_SIMUL_RGBIR:
        case IQ_SSR_OV2312_SIMUL_IR:
            AmbaPrint_PrintStr5("<%s>", OV2312SIMULRGBIRPath.ImgParam.path , NULL, NULL, NULL, NULL);
            AmbaPrint_PrintStr5("<%s>", OV2312SIMULIRPath.ImgParam.path , NULL, NULL, NULL, NULL);
            AmbaPrint_PrintStr5("<%s>", OV2312TDRGBIRPath.ImgParam.path , NULL, NULL, NULL, NULL);
            AmbaPrint_PrintStr5("<%s>", OV2312TDIRPath.ImgParam.path , NULL, NULL, NULL, NULL);
            break;
        case IQ_SSR_TI913_954_OV10635:
            AmbaPrint_PrintStr5("<%s>", OV10635SHDR2Path.ImgParam.path , NULL, NULL, NULL, NULL);
            break;
        case IQ_SSR_MAX9295_9296_OV2778:
            AmbaPrint_PrintStr5("<%s>", OV2778Path.ImgParam.path , NULL, NULL, NULL, NULL);
            AmbaPrint_PrintStr5("<%s>", OV2778SHDR2Path.ImgParam.path , NULL, NULL, NULL, NULL);
            break;
        case IQ_SENSOR_MAX9295_9296_VG1762:
        case IQ_SENSOR_MAX9295_9296_VG1762D:
        case IQ_SENSOR_MAX9295_9296_VG1762DIR:
            AmbaPrint_PrintStr5("<%s>", VG1762SHDR2Path.ImgParam.path , NULL, NULL, NULL, NULL);
            AmbaPrint_PrintStr5("<%s>", VG1762DOUTPUTSHDR2Path.ImgParam.path , NULL, NULL, NULL, NULL);
            AmbaPrint_PrintStr5("<%s>", VG1762DOUTPUTIRPath.ImgParam.path , NULL, NULL, NULL, NULL);
            break;
        case IQ_SSR_IMX586:
            AmbaPrint_PrintStr5("<%s>", IMX586Path.ImgParam.path , NULL, NULL, NULL, NULL);
            AmbaPrint_PrintStr5("<%s>", IMX586Path.ImgParam.path , NULL, NULL, NULL, NULL);
            AmbaPrint_PrintStr5("<%s>", IMX586Path.ImgParam.path , NULL, NULL, NULL, NULL);
            break;
        case IQ_SSR_IMX686:
            AmbaPrint_PrintStr5("<%s>", IMX686Path.ImgParam.path , NULL, NULL, NULL, NULL);
            AmbaPrint_PrintStr5("<%s>", IMX686SHDR3Path.ImgParam.path , NULL, NULL, NULL, NULL);
            break;
        case IQ_SSR_OV48C40:
            AmbaPrint_PrintStr5("<%s>", OV48C40Path.ImgParam.path , NULL, NULL, NULL, NULL);
            AmbaPrint_PrintStr5("<%s>", OV48C40SHDR2Path.ImgParam.path , NULL, NULL, NULL, NULL);
            break;
        case IQ_SSR_MAX9295_9296_OX03C:
            AmbaPrint_PrintStr5("<%s>", OX03CSHDR4Path.ImgParam.path , NULL, NULL, NULL, NULL);
            AmbaPrint_PrintStr5("<%s>", OX03CSHDR4Path.ImgParam.path , NULL, NULL, NULL, NULL);
            AmbaPrint_PrintStr5("<%s>", OX03CSHDR4Path.ImgParam.path , NULL, NULL, NULL, NULL);
            break;
        case IQ_SSR_MAX9295_9296_OX03F:
            AmbaPrint_PrintStr5("<%s>", OX03FSHDR4Path.ImgParam.path , NULL, NULL, NULL, NULL);
            AmbaPrint_PrintStr5("<%s>", OX03FSHDR4Path.ImgParam.path , NULL, NULL, NULL, NULL);
            AmbaPrint_PrintStr5("<%s>", OX03FSHDR4Path.ImgParam.path , NULL, NULL, NULL, NULL);
            break;
        case IQ_SSR_MAX9295_9296_OX03C_ADAS:
            AmbaPrint_PrintStr5("<%s>", OX03CSHDR4_ADASPath.ImgParam.path , NULL, NULL, NULL, NULL);
            AmbaPrint_PrintStr5("<%s>", OX03CSHDR4_ADASPath.ImgParam.path , NULL, NULL, NULL, NULL);
            AmbaPrint_PrintStr5("<%s>", OX03CSHDR4_ADASPath.ImgParam.path , NULL, NULL, NULL, NULL);
            break;
        case IQ_SSR_MAX9295_9296_OX03C_EMR:
            AmbaPrint_PrintStr5("<%s>", OX03CSHDR4_EMRPath.ImgParam.path , NULL, NULL, NULL, NULL);
            AmbaPrint_PrintStr5("<%s>", OX03CSHDR4_EMRPath.ImgParam.path , NULL, NULL, NULL, NULL);
            AmbaPrint_PrintStr5("<%s>", OX03CSHDR4_EMRPath.ImgParam.path , NULL, NULL, NULL, NULL);
            break;
        case IQ_SSR_MAX9295_9296_OX03F_ADAS:
            AmbaPrint_PrintStr5("<%s>", OX03FSHDR4_ADASPath.ImgParam.path , NULL, NULL, NULL, NULL);
            AmbaPrint_PrintStr5("<%s>", OX03FSHDR4_ADASPath.ImgParam.path , NULL, NULL, NULL, NULL);
            AmbaPrint_PrintStr5("<%s>", OX03FSHDR4_ADASPath.ImgParam.path , NULL, NULL, NULL, NULL);
            break;
        case IQ_SSR_MAX9295_9296_OX03F_EMR:
            AmbaPrint_PrintStr5("<%s>", OX03FSHDR4_EMRPath.ImgParam.path , NULL, NULL, NULL, NULL);
            AmbaPrint_PrintStr5("<%s>", OX03FSHDR4_EMRPath.ImgParam.path , NULL, NULL, NULL, NULL);
            AmbaPrint_PrintStr5("<%s>", OX03FSHDR4_EMRPath.ImgParam.path , NULL, NULL, NULL, NULL);
            break;
        default:
            AmbaPrint_PrintStr5("<%s> Invalid sensorID", __func__, NULL, NULL, NULL, NULL);
            AmbaPrint_Flush();
            RVal = IMGPROC_ERR_0007;
            break;
#else
        case IQ_SSR_IMX377:
        case IQ_SSR_IMX377Q:
        case IQ_SSR_IMX377D:
            AmbaPrint_PrintStr5("<%s>", IMX377Path.ImgParam.path , NULL, NULL, NULL, NULL);
            break;
        case IQ_SSR_IMX183:
            AmbaPrint_PrintStr5("<%s>", IMX183Path.ImgParam.path , NULL, NULL, NULL, NULL);
            break;
        case IQ_SSR_MAX96707_9286_AR0144:
            AmbaPrint_PrintStr5("<%s>", AR0144Path.ImgParam.path , NULL, NULL, NULL, NULL);
            break;
        case IQ_SSR_IMX290:
        case IQ_SSR_B6N_IMX290:
            AmbaPrint_PrintStr5("<%s>", IMX290Path.ImgParam.path , NULL, NULL, NULL, NULL);
            AmbaPrint_PrintStr5("<%s>", IMX290HDR2Path.ImgParam.path , NULL, NULL, NULL, NULL);
            break;
        case IQ_SSR_IMX577:
            AmbaPrint_PrintStr5("<%s>", IMX577Path.ImgParam.path , NULL, NULL, NULL, NULL);
            AmbaPrint_PrintStr5("<%s>", IMX577HDR2Path.ImgParam.path , NULL, NULL, NULL, NULL);
            break;
        default:
            RVal = IQParam_Path_Prt_Sub1(sensorID);
            break;
#endif
    }
    return RVal;
}

