/*
* Copyright (c) 2020 Ambarella International LP
*
* This file and its contents ("Software") are protected by intellectual
* property rights including, without limitation, U.S. and/or foreign
* copyrights. This Software is also the confidential and proprietary
* information of Ambarella International LP and its licensors. You may not use, reproduce,
* disclose, distribute, modify, or otherwise prepare derivative works of this
* Software or any portion thereof except pursuant to a signed license agreement
* or nondisclosure agreement with Ambarella International LP or its authorized affiliates.
* In the absence of such an agreement, you agree to promptly notify and return
* this Software to Ambarella International LP.
*
* This file includes sample code and is only for internal testing and evaluation.  If you
* distribute this sample code (whether in source, object, or binary code form), it will be
* without any warranty or indemnity protection from Ambarella International LP or its affiliates.
*
* THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
* INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF NON-INFRINGEMENT,
* MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
* IN NO EVENT SHALL AMBARELLA INTERNATIONAL LP OR ITS AFFILIATES BE LIABLE FOR ANY DIRECT,
* INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
* (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
* LOSS OF USE, DATA, OR PROFITS; COMPUTER FAILURE OR MALFUNCTION; OR BUSINESS
* INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
* CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
* ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
* POSSIBILITY OF SUCH DAMAGE.
*/

#include "ArmErrCode.h"
#include "AmbaTypes.h"
#include "SvcCvAlgo.h"
#include "cvapi_flexidag.h"
#include "AmbaCtfw_MvacFlow.h"
#include "AmbaMisraFix.h"

#if defined(CONFIG_QNX)
#include "cvapi_flexidag_unittest.h"
#define SD_ROOT_PATH "/sd0"
#elif defined(CONFIG_LINUX)
#define SD_ROOT_PATH "/tmp/SD0"
#else // threadX and the others
#include "cvapi_flexidag_unittest.h"
#define SD_ROOT_PATH "c:"
#endif


#define UT_NAME "AmbaAc_UT"

static const char *gInputPath;

#define UT_CFG_MVAC_PROC_SCALE_ID          (2U)
#define UT_CFG_MVAC_VO_PROC_WIDTH          (1920U)
#define UT_CFG_MVAC_VO_PROC_HEIGHT         (960U)

#define SCALE_0_IMG_WIDTH           (3840U)
#define SCALE_0_IMG_HEIGHT          (1920U)

#define IN_SCALE_2_IMG_ROI_WIDTH    (1920U)
#define IN_SCALE_2_IMG_ROI_HEIGHT   (960U)

#define UT_AC_OUT_PATH SD_ROOT_PATH"/CTFW_UT/out/ac"

#define MaxFileNameLen (128U)
#define SCALE_ID (2U)


static void RefCV_GetMvacResult(const CtfwFrameMetadata_t *pMetadata, const void *pResult)
{
    UINT32 Rval = ARM_OK;
    AMBA_CV_MVAC_DATA_s *pAcResult;
    char Filename[MaxFileNameLen];

    if ( pMetadata->ErrorState != 0U ) {
        ArmLog_STR(UT_NAME, "## %s, get error", __func__, 0U);
    } else {

        AmbaMisra_TypeCast(&pAcResult, &(pResult));

        switch (pAcResult->State) {
        case AMBA_CV_MVAC_STATE_RESET:
            ArmLog_DBG(UT_NAME, "## state: RESET", 0U, 0U);
            break;
        case AMBA_CV_MVAC_STATE_WAIT:
            ArmLog_DBG(UT_NAME, "## state: WAIT", 0U, 0U);
            break;
        case AMBA_CV_MVAC_STATE_ACCUMULATION:
            ArmLog_DBG(UT_NAME, "## state: ACCUMULATION", 0U, 0U);
            break;
        case AMBA_CV_MVAC_STATE_SOLVE:
            ArmLog_DBG(UT_NAME, "## state: SOLVE", 0U, 0U);
            if (pAcResult->pWarpInfo != NULL) {
                UINT32 WriteSize;
                if (pAcResult->pWarpInfo->CalibrationUpdates[0].WarpTableValid == 1U) {
                    Rval |= AmbaWrap_memset(Filename, 0, sizeof(Filename));
                    AmbaUtility_StringAppend(Filename, (UINT32)sizeof(Filename), UT_AC_OUT_PATH"/warp_left.bin");
                    ArmLog_DBG(UT_NAME, "[ut] got warp table", 0U, 0U);
                    Rval |= ArmFIO_Save(&(pAcResult->pWarpInfo->CalibrationUpdates[0].WarpTable),
                                        (UINT32)sizeof(pAcResult->pWarpInfo->CalibrationUpdates[0].WarpTable),
                                        Filename, &WriteSize);
                    //AmbaCtfw_MvacProfilerReport();
                }
                if (pAcResult->pWarpInfo->CalibrationUpdates[0].MetaCalibValid == 1U) {
                    Rval |= AmbaWrap_memset(Filename, 0, sizeof(Filename));
                    AmbaUtility_StringAppend(Filename, (UINT32)sizeof(Filename), UT_AC_OUT_PATH"/calib_left.bin");
                    ArmLog_DBG(UT_NAME, "[ut] got calib info", 0U, 0U);
                    Rval |= ArmFIO_Save(&(pAcResult->pWarpInfo->CalibrationUpdates[0].MetaCalib),
                                        (UINT32)sizeof(pAcResult->pWarpInfo->CalibrationUpdates[0].MetaCalib),
                                        Filename, &WriteSize);
                }


            }
            break;
        case AMBA_CV_MVAC_STATE_END:
            ArmLog_DBG(UT_NAME, "## state: END", 0U, 0U);
            if (pAcResult->pWarpInfo != NULL) {
                UINT32 WriteSize;
                if (pAcResult->pWarpInfo->CalibrationUpdates[0].WarpTableValid == 1U) {
                    Rval |= AmbaWrap_memset(Filename, 0, sizeof(Filename));
                    AmbaUtility_StringAppend(Filename, (UINT32)sizeof(Filename), UT_AC_OUT_PATH"/warp_left.bin");
                    ArmLog_DBG(UT_NAME, "[ut] got warp table", 0U, 0U);
                    Rval |= ArmFIO_Save(&(pAcResult->pWarpInfo->CalibrationUpdates[0].WarpTable),
                                        (UINT32)sizeof(pAcResult->pWarpInfo->CalibrationUpdates[0].WarpTable),
                                        Filename, &WriteSize);
                    //AmbaCtfw_MvacProfilerReport();
                }
                if (pAcResult->pWarpInfo->CalibrationUpdates[0].MetaCalibValid == 1U) {
                    Rval |= AmbaWrap_memset(Filename, 0, sizeof(Filename));
                    AmbaUtility_StringAppend(Filename, (UINT32)sizeof(Filename), UT_AC_OUT_PATH"/calib_left.bin");
                    ArmLog_DBG(UT_NAME, "[ut] got calib info", 0U, 0U);
                    Rval |= ArmFIO_Save(&(pAcResult->pWarpInfo->CalibrationUpdates[0].MetaCalib),
                                        (UINT32)sizeof(pAcResult->pWarpInfo->CalibrationUpdates[0].MetaCalib),
                                        Filename, &WriteSize);
                }


            }
            break;
        case AMBA_CV_MVAC_STATE_SKIP:
            ArmLog_DBG(UT_NAME, "## state: SKIP", 0U, 0U);

            if (pAcResult->pWarpInfo != NULL) {
                UINT32 WriteSize;
                if (pAcResult->pWarpInfo->CalibrationUpdates[0].WarpTableValid == 1U) {
                    Rval |= AmbaWrap_memset(Filename, 0, sizeof(Filename));
                    AmbaUtility_StringAppend(Filename, (UINT32)sizeof(Filename), UT_AC_OUT_PATH"/warp_left.bin");
                    ArmLog_DBG(UT_NAME, "[ut] got warp table", 0U, 0U);
                    Rval |= ArmFIO_Save(&(pAcResult->pWarpInfo->CalibrationUpdates[0].WarpTable),
                                        (UINT32)sizeof(pAcResult->pWarpInfo->CalibrationUpdates[0].WarpTable),
                                        Filename, &WriteSize);
                    //AmbaCtfw_MvacProfilerReport();
                }
                if (pAcResult->pWarpInfo->CalibrationUpdates[0].MetaCalibValid == 1U) {
                    Rval |= AmbaWrap_memset(Filename, 0, sizeof(Filename));
                    AmbaUtility_StringAppend(Filename, (UINT32)sizeof(Filename), UT_AC_OUT_PATH"/calib_left.bin");
                    ArmLog_DBG(UT_NAME, "[ut] got calib info", 0U, 0U);
                    Rval |= ArmFIO_Save(&(pAcResult->pWarpInfo->CalibrationUpdates[0].MetaCalib),
                                        (UINT32)sizeof(pAcResult->pWarpInfo->CalibrationUpdates[0].MetaCalib),
                                        Filename, &WriteSize);
                }


            }
            break;
        default:
            ArmLog_ERR(UT_NAME, "## callback at state: ERROR", 0U, 0U);
            break;
        }

        AmbaMisra_TouchUnused(&Rval);
    }

    return;
}

static void ConfigPicInfoHeader(memio_source_recv_picinfo_t *pPicInfo, UINT32 Scale0Width, UINT32 Scale0Pitch, UINT32 Scale0Height)
{
    UINT32 Idx;
    UINT32 Rval = ARM_OK;

    Rval |= AmbaWrap_memset(pPicInfo, 0, sizeof(memio_source_recv_picinfo_t));

    pPicInfo->pic_info.pyramid.image_width_m1 = (UINT16) (Scale0Width - 1U);
    pPicInfo->pic_info.pyramid.image_height_m1 = (UINT16) (Scale0Height - 1U);
    pPicInfo->pic_info.pyramid.image_pitch_m1 = (Scale0Pitch - 1U);

    for (Idx = 0U; Idx < MAX_HALF_OCTAVES; Idx++) {
        pPicInfo->pic_info.pyramid.half_octave[Idx].ctrl.disable = 1U;
    }

    (void) Rval;

    return;
}

static void ConfigPicInfoScale(memio_source_recv_picinfo_t *pPicInfo, UINT32 ScaleId, const roi_t *pRoi,
                               UINT32 RoiPitch, const char *pLumaLeftAddr, const char *pLumaRightAddr)
{
    ULONG PicInfoAddr;
    ULONG LumaAddr;
    UINT32 LumaLeftOffset;
    UINT32 LumaRightOffset;
    ULONG DiffAddr;

    AmbaMisra_TypeCast(&PicInfoAddr, &pPicInfo);

    AmbaMisra_TypeCast(&LumaAddr, &pLumaLeftAddr);
    DiffAddr = LumaAddr - PicInfoAddr;
    LumaLeftOffset =  (UINT32)DiffAddr;

    AmbaMisra_TypeCast(&LumaAddr, &pLumaRightAddr);
    DiffAddr = LumaAddr - PicInfoAddr;
    LumaRightOffset =  (UINT32)DiffAddr;

    pPicInfo->pic_info.pyramid.half_octave[ScaleId].ctrl.mode = 1U;
    pPicInfo->pic_info.pyramid.half_octave[ScaleId].ctrl.octave_mode = 0U;
    pPicInfo->pic_info.pyramid.half_octave[ScaleId].ctrl.disable = 0U;
    pPicInfo->pic_info.pyramid.half_octave[ScaleId].ctrl.roi_pitch = (UINT16) RoiPitch;
    pPicInfo->pic_info.pyramid.half_octave[ScaleId].roi_start_row = (INT16) pRoi->m_start_col;
    pPicInfo->pic_info.pyramid.half_octave[ScaleId].roi_start_col = (INT16) pRoi->m_start_row;
    pPicInfo->pic_info.pyramid.half_octave[ScaleId].roi_height_m1 = pRoi->m_height_m1;
    pPicInfo->pic_info.pyramid.half_octave[ScaleId].roi_width_m1 = pRoi->m_width_m1;

    pPicInfo->pic_info.rpLumaLeft[ScaleId] = LumaLeftOffset;
    pPicInfo->pic_info.rpLumaRight[ScaleId] = LumaRightOffset;

    return;
}

static UINT32 RefCV_LoadCalibData(const CtfwCalibInfo_t *pCalibInfo)
{
    UINT32 Rval = ARM_OK;
    char Filename[MaxFileNameLen];
    UINT32 Length, ActualLoadSize;
    const char *pStrRet;

    // read metadata
    {
        pStrRet = ArmStdC_strcpy(Filename, MaxFileNameLen, gInputPath);
        ArmStdC_strcat(Filename, MaxFileNameLen, "/metadata/meta_fov0.bin");
        ArmLog_STR(UT_NAME, "## filename: %s", Filename, NULL);
        Rval |= ArmFIO_GetSize(Filename, &Length);
    }
    if (Rval == ARM_OK) {
        Rval = ArmFIO_Load(pCalibInfo->Left.pBuffer, Length, Filename, &ActualLoadSize);
        //ArmLog_U64(UT_NAME, "## metadata left, pBuffer=%llx", (UINT64)pCalibInfo->Left.pBuffer, (UINT64)0U);
    }

    if (Rval == ARM_OK) {
        pStrRet = ArmStdC_strcpy(Filename, MaxFileNameLen, gInputPath);
        ArmStdC_strcat(Filename, MaxFileNameLen, "/metadata/meta_fov1.bin");
        ArmLog_STR(UT_NAME, "## filename: %s", Filename, NULL);
        Rval |= ArmFIO_GetSize(Filename, &Length);
    }
    if (Rval == ARM_OK) {
        Rval = ArmFIO_Load(pCalibInfo->Right.pBuffer, Length, Filename, &ActualLoadSize);
        //ArmLog_U64(UT_NAME, "## metadata right, pBuffer=%llx", (UINT64)pCalibInfo->Right.pBuffer, (UINT64)0U);
    }

    (void) pStrRet;

    return Rval;
}

static UINT32 RefCV_LoadWarpTable(AMBA_CV_MVAC_WARP_TABLE_INFO_s *pWarpTableInfo)
{
    UINT32 Rval = ARM_OK;
    char Filename[MaxFileNameLen];
    UINT32 Length, ActualLoadSize;
    const char *pStrRet;

    {
        pStrRet = ArmStdC_strcpy(Filename, MaxFileNameLen, gInputPath);
        ArmStdC_strcat(Filename, MaxFileNameLen, "/metadata/warp_info.bin");
        ArmLog_STR(UT_NAME, "## filename: %s", Filename, NULL);
        Rval |= ArmFIO_GetSize(Filename, &Length);
    }

    if (Rval == ARM_OK) {
        Rval = ArmFIO_Load(pWarpTableInfo, Length, Filename, &ActualLoadSize);
    }

    (void) pStrRet;

    return Rval;
}

static UINT32 RefCV_SetupFmaTaskConfig(CTFW_FMA_TASK_CONFIG_t *pFmaTaskConfig)
{
    UINT32 Rval = ARM_OK;

    pFmaTaskConfig->ScaleID = UT_CFG_MVAC_PROC_SCALE_ID;
    pFmaTaskConfig->FexSrcTaskType = CTFW_TASK_FEX;

    return Rval;
}

static UINT32 RefCV_SetupVoTaskConfig(const CtfwCalibInfo_t *pCalibInfo,
                                      CTFW_MVAC_VO_TASK_CONFIG_t *pVoTaskConfig)
{
    UINT32 Rval = ARM_OK;
    AMBA_CV_VO_CFG_s *pVoConfig = &pVoTaskConfig->VoCfg;
    const flexidag_memblk_t *pMetaRightBuf = &pCalibInfo->Right;

    AmbaMisra_TypeCast(&pVoTaskConfig->pInitMetaRightBuf,
                       &pMetaRightBuf);
    pVoTaskConfig->FexSrcTaskType = CTFW_TASK_FEX;

    pVoConfig->View.RoiX = 0U;
    pVoConfig->View.RoiY = 0U;
    pVoConfig->View.RoiWidth = UT_CFG_MVAC_VO_PROC_WIDTH;
    pVoConfig->View.RoiHeight = UT_CFG_MVAC_VO_PROC_HEIGHT;
    pVoConfig->View.Scale = UT_CFG_MVAC_PROC_SCALE_ID;
    pVoConfig->MaxMinimizationIters = 8U;
    pVoConfig->ViewMaxMinimizationFeaturesStereo = 400U;

    return Rval;
}

static UINT32 RefCV_SetupMvacTaskConfig(const CtfwCalibInfo_t *pCalibInfo,
                                        const AMBA_CV_MVAC_WARP_TABLE_INFO_s *pWarpTableInfo, CTFW_MVAC_AC_TASK_CONFIG_t *pAcTaskConfig)
{
    UINT32 Rval = ARM_OK;
    const flexidag_memblk_t *pMetaLeftBuf = &pCalibInfo->Left;

    AmbaMisra_TypeCast(&pAcTaskConfig->pInitMetaLeftBuf,
                       &pMetaLeftBuf);
    pAcTaskConfig->FexSrcTaskType = CTFW_TASK_FEX;

    pAcTaskConfig->MvacViewInfo.WarpTableInfo = *pWarpTableInfo;
    pAcTaskConfig->MvacViewInfo.ScaleId = UT_CFG_MVAC_PROC_SCALE_ID;

    return Rval;
}

static UINT32 RefCV_LoadMvacInputLuma(UINT32 FrameCnt, flexidag_memblk_t *pLumaLeft,
                                      flexidag_memblk_t *pLumaRight)
{
    UINT32 Rval = ARM_OK;
    char Filename[MaxFileNameLen];
    char FileIndex[MaxFileNameLen] = {0};
    UINT32 Length = 0U;
    UINT32 ActualLoadSize;
    flexidag_memblk_t *pOutBufDesc;
    const char *pStrRet;

    // read input image
    //load left image
    //generate filename
    {
        UINT32 RetStrLen;
        pStrRet = ArmStdC_strcpy(Filename, MaxFileNameLen, gInputPath);
        ArmStdC_strcat(Filename, MaxFileNameLen, "/left_2_y/");
        RetStrLen = AmbaUtility_StringPrintUInt32(FileIndex, (UINT32)sizeof(FileIndex), "%010d", 1U, &FrameCnt);
        ArmStdC_strcat(Filename, MaxFileNameLen, FileIndex);
        ArmStdC_strcat(Filename, MaxFileNameLen, ".y");
        ArmLog_STR(UT_NAME, "## filename: %s", Filename, NULL);

        //load to outbuf
        Rval |= ArmFIO_GetSize(Filename, &Length);
        (void) RetStrLen;
    }

    pOutBufDesc = pLumaLeft;
    if ((Rval == ARM_OK) && (Length <= pOutBufDesc->buffer_size)) {
        Rval |= ArmFIO_Load(pOutBufDesc->pBuffer, Length, Filename, &ActualLoadSize);
        Rval |= AmbaCV_UtilityCmaMemClean(pOutBufDesc);
    } else {
        ArmLog_ERR(UT_NAME, "## Proc2Uprp, ArmFIO_Load failed, Rval=%d, Length=%d", Rval, Length);
    }

    if(Rval != ARM_OK) {
        ArmLog_ERR(UT_NAME, "## Proc2Uprp, ArmFIO_Load failed", 0U, 0U);
    }

    //load right image
    //generate filename
    if (Rval == ARM_OK) {
        UINT32 RetStrLen;
        pStrRet = ArmStdC_strcpy(Filename, MaxFileNameLen, gInputPath);
        ArmStdC_strcat(Filename, MaxFileNameLen, "/right_2_y/");
        RetStrLen = AmbaUtility_StringPrintUInt32(FileIndex, (UINT32)sizeof(FileIndex), "%010d", 1U, &FrameCnt);
        ArmStdC_strcat(Filename, MaxFileNameLen, FileIndex);
        ArmStdC_strcat(Filename, MaxFileNameLen, ".y");
        ArmLog_STR(UT_NAME, "## filename: %s", Filename, NULL);
        //load to outbuf
        Rval |= ArmFIO_GetSize(Filename, &Length);
        (void) RetStrLen;
    }

    pOutBufDesc = pLumaRight;
    if ((Rval == ARM_OK) && (Length <= pOutBufDesc->buffer_size)) {
        Rval = ArmFIO_Load(pOutBufDesc->pBuffer, Length, Filename, &ActualLoadSize);
        Rval |= AmbaCV_UtilityCmaMemClean(pOutBufDesc);
    } else {
        ArmLog_ERR(UT_NAME, "## Proc2Uprp, ArmFIO_Load failed, Rval=%d, Length=%d", Rval, Length);
    }
    if(Rval != ARM_OK) {
        ArmLog_ERR(UT_NAME, "## Proc2Uprp, ArmFIO_Load failed", 0U, 0U);
    }

    (void) pStrRet;

    return Rval;
}

void RefCV_CtfwAc_UT(const char *pInputPath, UINT32 StartFrameIdx,
                     UINT32 EndFrameIdx, UINT32 FrameIdxStep)
{
    UINT32 Rval = ARM_OK;

    static flexidag_memblk_t TotalBuf = {0};
    static UINT32 MemPoolId;

    UINT32 BuffIdx;

    static CtfwCalibInfo_t CalibInfo;
    static AMBA_CV_MVAC_WARP_TABLE_INFO_s WarpTableInfo;

    flexidag_memblk_t PicInfoBuff[CTFW_MAX_BUFFER_DEPTH];
    flexidag_memblk_t LumaLeft[CTFW_MAX_BUFFER_DEPTH];
    flexidag_memblk_t LumaRight[CTFW_MAX_BUFFER_DEPTH];
    static CtfwFrame_t InFramePool[CTFW_MAX_BUFFER_DEPTH];

    UINT32 ImageSize = 1844620U;//((1920U*960U) + 1420U);
    UINT32 MetaSize = 3000U;
    UINT32 PicInfoSize = (UINT32)sizeof(memio_source_recv_picinfo_t);
    UINT32 AllocSingleChSize = (CTFW_MAX_BUFFER_DEPTH*(CTFW_CV_MEM_ALIGN(ImageSize)+CTFW_CV_MEM_ALIGN(PicInfoSize)))+CTFW_CV_MEM_ALIGN(MetaSize);
    UINT32 AllocTotalSize = AllocSingleChSize*2U; // left and right

    static CTFW_FMA_TASK_CONFIG_t FmaTaskConfig;
    static CTFW_MVAC_VO_TASK_CONFIG_t VoTaskConfig;
    static CTFW_MVAC_AC_TASK_CONFIG_t AcTaskConfig;

    UINT32 FrameCnt = 0U;

    gInputPath = pInputPath;

    // alloc memory
    {
        Rval = AmbaCV_UtilityCmaMemAlloc(AllocTotalSize, 1U, &TotalBuf);
        if (Rval != 0U) {
            ArmLog_ERR(UT_NAME, "## Can't allocate %u from CmaMem", AllocTotalSize, 0U);
        } else {
            Rval = ArmMemPool_Create(&TotalBuf, &MemPoolId);
        }
    }

    Rval |= Cftw_MemPool_Allocate(MemPoolId, MetaSize, &CalibInfo.Left);
    Rval |= Cftw_MemPool_Allocate(MemPoolId, MetaSize, &CalibInfo.Right);
    for (BuffIdx = 0U; BuffIdx < CTFW_MAX_BUFFER_DEPTH; BuffIdx++) {
        memio_source_recv_picinfo_t *pPicinfo;
        UINT32 ScaleIdx;
        roi_t ScaleRoi;
        UINT32 ScaleRoiPitch;

        Rval |= Cftw_MemPool_Allocate(MemPoolId, (UINT32)sizeof(memio_source_recv_picinfo_t), &(PicInfoBuff[BuffIdx]));
        AmbaMisra_TypeCast(&pPicinfo, &(PicInfoBuff[BuffIdx].pBuffer));
        ConfigPicInfoHeader(pPicinfo, SCALE_0_IMG_WIDTH, SCALE_0_IMG_WIDTH, SCALE_0_IMG_HEIGHT);

        Rval |= Cftw_MemPool_Allocate(MemPoolId, ImageSize, &(LumaLeft[BuffIdx]));
        Rval |= Cftw_MemPool_Allocate(MemPoolId, ImageSize, &(LumaRight[BuffIdx]));

        ScaleIdx = 2U;
        ScaleRoi.m_start_col = 0U;
        ScaleRoi.m_start_row = 0U;
        ScaleRoi.m_width_m1 = (UINT16) IN_SCALE_2_IMG_ROI_WIDTH - 1U;
        ScaleRoi.m_height_m1 = (UINT16) IN_SCALE_2_IMG_ROI_HEIGHT- 1U;
        ScaleRoiPitch = IN_SCALE_2_IMG_ROI_WIDTH;
        ConfigPicInfoScale(pPicinfo, ScaleIdx, &ScaleRoi, ScaleRoiPitch,
                           LumaLeft[BuffIdx].pBuffer, LumaRight[BuffIdx].pBuffer);
    }

    Rval |= RefCV_LoadCalibData(&CalibInfo);
    Rval |= RefCV_LoadWarpTable(&WarpTableInfo);

    Rval |= RefCV_SetupVoTaskConfig(&CalibInfo, &VoTaskConfig);
    Rval |= RefCV_SetupMvacTaskConfig(&CalibInfo, &WarpTableInfo, &AcTaskConfig);
    Rval |= RefCV_SetupFmaTaskConfig(&FmaTaskConfig);

    {
        CtfwGetProcResult_t GetMvacResultCbFxn = RefCV_GetMvacResult;
        Rval |= AmbaCtfw_MvacInit(&FmaTaskConfig,
                                  &VoTaskConfig, &AcTaskConfig, GetMvacResultCbFxn, CTFW_MAX_BUFFER_DEPTH, InFramePool);
    }

    for (FrameCnt = StartFrameIdx; FrameCnt <= EndFrameIdx; FrameCnt+=FrameIdxStep) {
        CtfwFrame_t *pFrame;
        UINT32 FrameBuffIdx = FrameCnt % CTFW_MAX_BUFFER_DEPTH;
        flexidag_memblk_t *pLumaLeft;
        flexidag_memblk_t *pLumaRight;
        const flexidag_memblk_t *pPicInfoBuff;

        pLumaLeft = &LumaLeft[FrameBuffIdx];
        pLumaRight = &LumaRight[FrameBuffIdx];
        pPicInfoBuff = &PicInfoBuff[FrameBuffIdx];
        pFrame = &InFramePool[FrameBuffIdx];

        Rval |= RefCV_LoadMvacInputLuma(FrameCnt, pLumaLeft, pLumaRight);

        Rval |= AmbaCtfw_MvacProduceFrame(pPicInfoBuff, &CalibInfo, FrameCnt, pFrame);
        if (Rval == ARM_OK) {
            Rval |= AmbaCtfw_MvacFeedOneFrame(pFrame);
        }

        Rval |= AmbaKAL_TaskSleep(100U);
    }

    AmbaCtfw_MvacProfilerReport();

    AmbaMisra_TouchUnused(&Rval);
}

#if defined(CONFIG_LINUX)
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <stdint.h>
#include <assert.h>
#include <signal.h>
#include <fcntl.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include "schdr_api.h"
#include "cvtask_api.h"
#include "ambint.h"
#include "cvapi_ambacv_flexidag.h"
//#include "rtos/AmbaRTOSWrapper.h"

static void RefCV_SignalHandlerShutdown(int sig)
{
    printf("%s: Got signal %d, program exits!\n",__FILE__,sig);
    exit(0);
}

int main(int argc, char **argv)
{
    static UINT32 init = 0;
    UINT32 ret = 0U;
    AMBA_CV_FLEXIDAG_SCHDR_CFG_s cfg;

    if ( argc < 5 ) {
        printf("Usage: %s [Input Folder] [StartFrame Index] [EndFrame Index] [Frame Index Step]\n", argv[0U]);
        printf("Ex: %s \n", argv[0U]);
        exit(-1);
    }

    /* Allow ourselves to be shut down gracefully by a signal */
    signal(SIGTERM, RefCV_SignalHandlerShutdown);
    signal(SIGHUP, RefCV_SignalHandlerShutdown);
    signal(SIGUSR1, RefCV_SignalHandlerShutdown);
    signal(SIGQUIT, RefCV_SignalHandlerShutdown);
    signal(SIGINT, RefCV_SignalHandlerShutdown);
    signal(SIGKILL, RefCV_SignalHandlerShutdown);

    /* init scheduler */
    cfg.cpu_map = 0xD;
    cfg.log_level = LVL_DEBUG;
    AmbaCV_FlexidagSchdrStart(&cfg);

    {
        const char* pInputPath = argv[1U];
        UINT32 StartFrameIdx = atoi(argv[2U]);
        UINT32 EndFrameIdx = atoi(argv[3U]);
        UINT32 FrameIdxStep = atoi(argv[4U]);

        RefCV_CtfwAc_UT(pInputPath, StartFrameIdx, EndFrameIdx, FrameIdxStep);
    }

    return 0;
}
#endif

