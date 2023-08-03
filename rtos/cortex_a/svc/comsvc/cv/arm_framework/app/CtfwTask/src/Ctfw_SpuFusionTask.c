#include "Ctfw_TaskUtil.h"
#include "Ctfw_SpuFusionTask.h"
#include "Ctfw_SpuFexTask.h"
#include "cvapi_flexidag_ambaspufusion_cv2.h"
#include "cvapi_svccvalgo_ambaspufusion.h"
#include "AmbaMisraFix.h"

typedef struct {
    SVC_CV_ALGO_HANDLE_s ScaHandle02;
    AMBA_STEREO_FD_HANDLE_s StereoHandle02;

    SVC_CV_ALGO_HANDLE_s ScaHandle024;
    AMBA_STEREO_FD_HANDLE_s StereoHandle024;

    SVC_CV_ALGO_HANDLE_s *pCurScaHandle;
    AMBA_STEREO_FD_HANDLE_s *pCurStereoHandle;

    CTFW_SPU_FUSION_TASK_CONFIG_t StoredSpuFusionCfg;

} SPU_FUSION_TASK_WORK_BUFF_s;

typedef struct {
    flexidag_memblk_t TaskWorkBuff;
    flexidag_memblk_t ScaleHackImgBuf;
    flexidag_memblk_t ScaAlgoBuf;
} SPU_FUSION_PRIVATE_s;

#define ALLOC_SPU_FUSION_SCA_REQ_BUFF_SIZE  (2UL<<20UL)
#define ALLOC_SPU_FUSION_OUT_BUFF_SIZE      (16UL<<20UL)

#define CTFW_TASK_NAME "SpuFusion"

static UINT32 CftwSpuFusion_QuerySize(UINT32 *pTaskStackSize, UINT32 *pAlgBuffMapSize, UINT32 *pAlgWorkSize, UINT32 *pAlgOutputSize)
{
    UINT32 RetVal = ARM_OK;
    UINT32 AlgBuffMapSize = 0U;
    UINT32 AlgWorkSize = 0U;
    UINT32 AlgOutputSize = 0U;

    *pTaskStackSize = 16384;
    *pAlgBuffMapSize = 0U;
    *pAlgWorkSize = 0U;
    *pAlgOutputSize = 0U;

    // AlgBuffMapSize
    {
        AlgBuffMapSize += CTFW_CV_MEM_ALIGN((UINT32)sizeof(SPU_FUSION_PRIVATE_s));
    }

    // AlgWorkSize
    {
        {
            AlgWorkSize += CTFW_CV_MEM_ALIGN((UINT32)sizeof(SPU_FUSION_TASK_WORK_BUFF_s));
        }
        {
            AlgWorkSize += CTFW_CV_MEM_ALIGN((UINT32)sizeof(memio_source_recv_picinfo_t));
        }
        {
            AlgWorkSize += CTFW_CV_MEM_ALIGN((UINT32)ALLOC_SPU_FUSION_SCA_REQ_BUFF_SIZE);
        }
    }

    // AlgOutputSize
    {
        AlgOutputSize += CTFW_CV_MEM_ALIGN((UINT32)ALLOC_SPU_FUSION_OUT_BUFF_SIZE);
    }

    *pAlgBuffMapSize = AlgBuffMapSize;
    *pAlgWorkSize = AlgWorkSize;
    *pAlgOutputSize = AlgOutputSize;

    return RetVal;
}

static UINT32 CftwSpuFusion_SetName(char TaskName[])
{
    AmbaUtility_StringAppend(TaskName, MAX_CTFW_NAME_LEN, CTFW_TASK_NAME);
    return ARM_OK;
}

static UINT32 CftwSpuFusion_Init(const void *pAlgConfig, flexidag_memblk_t FlexiBinBlk[],
                                 UINT32 MemPoolID, const flexidag_memblk_t* const pAlgBuffMap, CtfwTaskConfig_t *pTaskSetting)
{
    UINT32 RetVal = ARM_OK;
    SPU_FUSION_PRIVATE_s *pPrivatePtr;

    (void) pAlgConfig;
    AmbaMisra_TouchUnused(FlexiBinBlk);

    AmbaMisra_TypeCast(&pPrivatePtr, &pAlgBuffMap->pBuffer);

    // Task env setting
    {
        pTaskSetting->ProcInterval = 1U;
        pTaskSetting->TaskPriority = 50U;
        pTaskSetting->TaskCoreSel = 0U;
    }

    // Partitioning memory pool
    {
        {
            RetVal |= Cftw_MemPool_Allocate(MemPoolID, (UINT32)sizeof(SPU_FUSION_TASK_WORK_BUFF_s), &pPrivatePtr->TaskWorkBuff);
        }
        {
            RetVal |= Cftw_MemPool_Allocate(MemPoolID, (UINT32)sizeof(memio_source_recv_picinfo_t), &pPrivatePtr->ScaleHackImgBuf);
        }
    }

    // Algo private init
    if ( RetVal == ARM_OK ) {
        SPU_FUSION_TASK_WORK_BUFF_s *pTaskWorkPtr;
        SVC_CV_ALGO_QUERY_CFG_s ScaQueryCfg = {NULL, 0, NULL};
        SVC_CV_ALGO_CREATE_CFG_s ScaCreateCfg = {
            .NumFD = 0U,
            .pBin = {NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL},
            .OutputNum = 0,
            .OutputSz = {0, 0, 0, 0, 0, 0, 0, 0},
            .pExtCreateCfg = NULL
        };

        flexidag_memblk_t FlexiBin;
        flexidag_memblk_t *pAlgoBlk;

        const CTFW_SPU_FUSION_TASK_CONFIG_t *pInSpuFusionCfg;

        AmbaMisra_TypeCast(&pTaskWorkPtr, &pPrivatePtr->TaskWorkBuff.pBuffer);
        RetVal |= AmbaWrap_memset(pTaskWorkPtr, 0, sizeof(SPU_FUSION_TASK_WORK_BUFF_s));

        AmbaMisra_TypeCast(&pInSpuFusionCfg, &pAlgConfig);
        RetVal |= AmbaWrap_memcpy(&pTaskWorkPtr->StoredSpuFusionCfg, pInSpuFusionCfg,
                                  sizeof(CTFW_SPU_FUSION_TASK_CONFIG_t));

        pAlgoBlk = &pPrivatePtr->ScaAlgoBuf;

        if ( pTaskWorkPtr->StoredSpuFusionCfg.LayerNum == 3U ) {
            pTaskWorkPtr->pCurScaHandle = &pTaskWorkPtr->ScaHandle024;
            pTaskWorkPtr->pCurStereoHandle = &pTaskWorkPtr->StereoHandle024;
        } else {
            pTaskWorkPtr->pCurScaHandle = &pTaskWorkPtr->ScaHandle02;
            pTaskWorkPtr->pCurStereoHandle = &pTaskWorkPtr->StereoHandle02;
        }

        //pScaHandle->pAlgoObj = &AmbaSpuFusionAlgoObj;
        //        pHandle->ScaHandle.pAlgoObj = pScaHandle->pAlgoObj;

        //ArmLog_STR(CTFW_TASK_NAME, "## call sca.query", Rval);
        //sca_query
        ScaQueryCfg.pAlgoObj = &AmbaSpuFusionAlgoObj;

        RetVal |= AmbaSpuFusionAlgoObj.Query(pTaskWorkPtr->pCurScaHandle, &ScaQueryCfg);
        AmbaMisra_TypeCast(&pTaskWorkPtr->pCurScaHandle->pAlgoCtrl, &pTaskWorkPtr->pCurStereoHandle);
        ArmLog_DBG(CTFW_TASK_NAME, "## ScaQueryCfg.TotalReqBufSz=%d, Rval=%d", ScaQueryCfg.TotalReqBufSz, RetVal);
        RetVal |= Cftw_MemPool_Allocate(MemPoolID, ScaQueryCfg.TotalReqBufSz, pAlgoBlk);

        if (RetVal == ARM_OK) {
            const UINT32 *pLayerNum = &pTaskWorkPtr->StoredSpuFusionCfg.LayerNum;

            //ArmLog_STR(CTFW_TASK_NAME, "## call create sca.create"); fflush(stdout);
            RetVal |= Ctfw_CheckRetVal(__func__, "AmbaWrap_memset", AmbaWrap_memset(&FlexiBin, 0, sizeof(FlexiBin)));
            AmbaMisra_TypeCast(&ScaCreateCfg.pExtCreateCfg, &pLayerNum);

            // TBD: requiredå?
            if ( pTaskWorkPtr->StoredSpuFusionCfg.LayerNum == 3U ) {
                ScaCreateCfg.NumFD = 3U;
            }

            ScaCreateCfg.pAlgoBuf = pAlgoBlk;
            ScaCreateCfg.pBin[0U] = &FlexiBin;
            RetVal |= AmbaSpuFusionAlgoObj.Create(pTaskWorkPtr->pCurScaHandle, &ScaCreateCfg);
        }
    }

    return RetVal;
}

// For CV2 exclusively
// set scale: Shift upward all the scales
// pSpu -> hacked in place, because it includes relative address of SPU
// pImg -> copy to another, because image address shouldn't be used in fusion
static void ScaleUpPicInfo(UINT32 ReferenceScaleId, const AMBA_CV_SPU_BUF_s *pSpu,
                           const AMBA_CV_IMG_BUF_s *pInImg, flexidag_memblk_t *pOutImg)
{
    UINT32 ScaleId;
    UINT32 RetVal = ARM_OK;

    if ( ReferenceScaleId == 0U ) {
        // Nothing to do
        RetVal = AmbaWrap_memcpy(pOutImg, pInImg, sizeof(AMBA_CV_IMG_BUF_s));
        if ( RetVal != ARM_OK ) {
            ArmLog_ERR(CTFW_TASK_NAME, "(%d) memcpy fail", __LINE__, 0U);
        }
    } else {
        // Shift up spu
        {
            AMBA_CV_SPU_DATA_s  *pSpuData;
            UINT32 HackScale0Width;
            UINT32 HackScale0Height;

            AmbaMisra_TypeCast(&pSpuData, &pSpu->pBuffer);

            HackScale0Width = pSpuData->Scales[ReferenceScaleId].DisparityWidth;
            HackScale0Height = pSpuData->Scales[ReferenceScaleId].DisparityHeight;

            pSpuData->Reserved_0[1U] = HackScale0Height;
            pSpuData->Reserved_0[2U] = HackScale0Width;

            for (ScaleId = 0U; ScaleId < MAX_HALF_OCTAVES; ScaleId++) {
                UINT32 TargetScaleId = ScaleId + ReferenceScaleId;
                if ( TargetScaleId < MAX_HALF_OCTAVES ) {
                    pSpuData->Scales[ScaleId] = pSpuData->Scales[TargetScaleId];
                } else {
                    // zero out the remain scales
                    RetVal |= Ctfw_CheckRetVal(__func__, "AmbaWrap_memset", AmbaWrap_memset(&pSpuData->Scales[ScaleId], 0, sizeof(AMBA_CV_SPU_SCALE_OUT_s)));
                    pSpuData->Scales[ScaleId].Status = (INT32)0xFFFFFFFEUL; // Not valid
                }
            }
        }
        // Shift up pic info
        {
            const memio_source_recv_picinfo_t *pInPicInfo;
            memio_source_recv_picinfo_t *pOutPicInfo;
            UINT16 HackScale0Width;
            UINT32 HackScale0Pitch;
            UINT16 HackScale0Height;

            AmbaMisra_TypeCast(&pInPicInfo, &pInImg->pBuffer);
            AmbaMisra_TypeCast(&pOutPicInfo, &pOutImg->pBuffer);

            HackScale0Width = (pInPicInfo->pic_info.pyramid.half_octave[ReferenceScaleId].roi_width_m1 + 1U);
            HackScale0Pitch = pInPicInfo->pic_info.pyramid.half_octave[ReferenceScaleId].ctrl.roi_pitch;
            HackScale0Height = (pInPicInfo->pic_info.pyramid.half_octave[ReferenceScaleId].roi_height_m1 + 1U);

            pOutPicInfo->pic_info.pyramid.image_width_m1 = (HackScale0Width - 1U);
            pOutPicInfo->pic_info.pyramid.image_height_m1 = (HackScale0Height - 1U);
            pOutPicInfo->pic_info.pyramid.image_pitch_m1 = (HackScale0Pitch - 1U);

            for (ScaleId = 0U; ScaleId < MAX_HALF_OCTAVES; ScaleId++) {
                UINT32 TargetScaleId = ScaleId + ReferenceScaleId;
                if ( TargetScaleId < MAX_HALF_OCTAVES ) {
                    pOutPicInfo->pic_info.pyramid.half_octave[ScaleId] = pInPicInfo->pic_info.pyramid.half_octave[TargetScaleId];
                    pOutPicInfo->pic_info.rpLumaLeft[ScaleId] = pInPicInfo->pic_info.rpLumaLeft[TargetScaleId];
                    pOutPicInfo->pic_info.rpLumaRight[ScaleId] = pInPicInfo->pic_info.rpLumaRight[TargetScaleId];
                } else {
                    // disable the remain scales
                    pOutPicInfo->pic_info.pyramid.half_octave[ScaleId].ctrl.disable = 1U;
                }
            }
        }
    }

    (void) RetVal;

    return;
}

static UINT32 CftwSpuFusion_Process(const CtfwFrameMetadata_t *pMetadata, const flexidag_memblk_t* const pAlgInputData[],
                                    const flexidag_memblk_t* const pAlgBuffMap, flexidag_memblk_t* const pAlgOutputData)
{
    UINT32 RetVal = ARM_OK;

    const SPU_FUSION_TASK_WORK_BUFF_s *pTaskWorkPtr;
    SPU_FUSION_PRIVATE_s *pPrivatePtr;

    SVC_CV_ALGO_HANDLE_s *pScaHandle;
    AMBA_CV_FLEXIDAG_IO_s ScaInBuf;
    AMBA_CV_FLEXIDAG_IO_s ScaOutBuf;
    SVC_CV_ALGO_FEED_CFG_s ScaFeedCfg;

    const CTFW_SPU_FEX_TASK_OUT_t *pIn0_SpuFexOut;
    const AMBA_CV_SPU_BUF_s *pIn0_SpuBuf;
    const AMBA_CV_IMG_BUF_s *pIn1_ImgBuf;
    flexidag_memblk_t *pIn1_HackImgBuf;

    (void) pMetadata;
    AmbaMisra_TouchUnused(pAlgOutputData);

    // Orgnize work buffer
    AmbaMisra_TypeCast(&pPrivatePtr, &pAlgBuffMap->pBuffer);
    AmbaMisra_TypeCast(&pTaskWorkPtr, &pPrivatePtr->TaskWorkBuff.pBuffer);

    // Orgnize input
    AmbaMisra_TypeCast(&pIn0_SpuFexOut, &pAlgInputData[0U]->pBuffer);
    AmbaMisra_TypeCast(&pIn1_ImgBuf, &pAlgInputData[1U]);

    // Get Spu from SpuFex
    {
        const flexidag_memblk_t *pSpuBlk;
        pSpuBlk = &pIn0_SpuFexOut->SpuOut;
        AmbaMisra_TypeCast(&pIn0_SpuBuf, &pSpuBlk);
    }

    // Hack to lift up scale
    {
        flexidag_memblk_t *pSpuBlk;

        pIn1_HackImgBuf = &pPrivatePtr->ScaleHackImgBuf;
        ScaleUpPicInfo(pTaskWorkPtr->StoredSpuFusionCfg.ReferenceScaleId,
                       pIn0_SpuBuf, pIn1_ImgBuf, pIn1_HackImgBuf);

        AmbaMisra_TypeCast(&pSpuBlk, &pIn0_SpuBuf);
        RetVal |= AmbaCV_UtilityCmaMemClean(pSpuBlk);

        RetVal |= AmbaCV_UtilityCmaMemClean(pIn1_HackImgBuf);
    }

    // Orgnize output

    // Algo private routine
    // ####################################

    pScaHandle = pTaskWorkPtr->pCurScaHandle;

    ScaFeedCfg.pIn = &ScaInBuf;
    ScaFeedCfg.pIn->num_of_buf = 2U;
    RetVal |= AmbaWrap_memcpy(
                  &(ScaFeedCfg.pIn->buf[0U]),
                  pIn0_SpuBuf, sizeof(flexidag_memblk_t));
    if ( RetVal != ARM_OK ) {
        ArmLog_ERR(CTFW_TASK_NAME, "(%d) memcpy fail", __LINE__, 0U);
    }

    RetVal |= AmbaWrap_memcpy(
                  &(ScaFeedCfg.pIn->buf[1U]),
                  pIn1_HackImgBuf, sizeof(flexidag_memblk_t));
    if ( RetVal != ARM_OK ) {
        ArmLog_ERR(CTFW_TASK_NAME, "(%d) memcpy fail", __LINE__, 0U);
    }

    ScaFeedCfg.pOut = &ScaOutBuf;
    ScaFeedCfg.pOut->num_of_buf = 1U;
    ScaFeedCfg.pOut->buf[0U] = *pAlgOutputData;
    RetVal |= pScaHandle->pAlgoObj->Feed(pScaHandle, &ScaFeedCfg);

    return RetVal;
}

static UINT32 CftwSpuFusion_DeInit(UINT32 MemPoolID, const flexidag_memblk_t* const pAlgBuffMap)
{
    UINT32 RetVal = ARM_OK;
    SPU_FUSION_PRIVATE_s *pPrivatePtr;
    SPU_FUSION_TASK_WORK_BUFF_s *pTaskWorkPtr;

    AmbaMisra_TypeCast(&pPrivatePtr, &pAlgBuffMap->pBuffer);
    AmbaMisra_TypeCast(&pTaskWorkPtr, &pPrivatePtr->TaskWorkBuff.pBuffer);

    {
        SVC_CV_ALGO_DELETE_CFG_s DeleteCfg;

        RetVal |= Ctfw_CheckRetVal(__func__, "AmbaWrap_memset", AmbaWrap_memset(&DeleteCfg, 0, sizeof(DeleteCfg)));
        // AlgoBufMemblk get free inside?
        RetVal |= AmbaSpuFusionAlgoObj.Delete(pTaskWorkPtr->pCurScaHandle, &DeleteCfg);
    }

    RetVal |= ArmMemPool_Free(MemPoolID, &pPrivatePtr->ScaAlgoBuf);
    RetVal |= ArmMemPool_Free(MemPoolID, &pPrivatePtr->ScaleHackImgBuf);
    RetVal |= ArmMemPool_Free(MemPoolID, &pPrivatePtr->TaskWorkBuff);

    return RetVal;
}

CtfwTaskObj_t gCtfwSpuFusionTaskObj = {
    .pSetName = CftwSpuFusion_SetName,
    .pQuerySize = CftwSpuFusion_QuerySize,
    .pInit = CftwSpuFusion_Init,
    .pProcess = CftwSpuFusion_Process,
    .pDeInit = CftwSpuFusion_DeInit,
};

