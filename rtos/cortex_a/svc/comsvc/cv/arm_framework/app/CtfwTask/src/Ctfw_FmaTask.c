#include "Ctfw_TaskUtil.h"
#include "Ctfw_FmaTask.h"
#include "Ctfw_SpuFexTask.h"

#include "cvapi_svccvalgo_ambafma.h"
#include "cvapi_flexidag_ambafma_cv2.h"
#include "AmbaMisraFix.h"

typedef struct {
    SVC_CV_ALGO_HANDLE_s ScaHandle;
    flexidag_memblk_t FmaOut;
    UINT32 FexSrcTaskType;
} FMA_TASK_WORK_BUFF_s;

typedef struct {
    flexidag_memblk_t TaskWorkBuff;
    flexidag_memblk_t ScaAlgoBuf;
} FMA_PRIVATE_s;

#define ALLOC_FMA_SCA_REQ_BUFF_SIZE (5UL<<20UL)
//AmbaCV_FlexidagOpen : output_size = 0x1115bc 0x0 0x0 0x0 0x0
#define ALLOC_FMA_OUT_BUFF_SIZE     (3UL<<20UL)

#define CTFW_TASK_NAME "Fma"

static UINT32 CftwFma_QuerySize(UINT32 *pTaskStackSize, UINT32 *pAlgBuffMapSize, UINT32 *pAlgWorkSize, UINT32 *pAlgOutputSize)
{
    UINT32 RetVal = ARM_OK;
    UINT32 AlgBuffMapSize = 0U;
    UINT32 AlgWorkSize = 0U;
    UINT32 AlgOutputSize = 0U;

    *pTaskStackSize = 16384U;
    *pAlgBuffMapSize = 0U;
    *pAlgWorkSize = 0U;
    *pAlgOutputSize = 0U;

    // AlgBuffMapSize
    {
        AlgBuffMapSize += CTFW_CV_MEM_ALIGN((UINT32)sizeof(FMA_PRIVATE_s));
    }

    // AlgWorkSize
    {
        {
            AlgWorkSize += CTFW_CV_MEM_ALIGN((UINT32)sizeof(FMA_TASK_WORK_BUFF_s));
        }
        {
            AlgWorkSize += CTFW_CV_MEM_ALIGN((UINT32)ALLOC_FMA_SCA_REQ_BUFF_SIZE);
        }
    }

    // AlgOutputSize
    {
        AlgOutputSize += CTFW_CV_MEM_ALIGN((UINT32)ALLOC_FMA_OUT_BUFF_SIZE);
    }

    *pAlgBuffMapSize = AlgBuffMapSize;
    *pAlgWorkSize = AlgWorkSize;
    *pAlgOutputSize = AlgOutputSize;

    return RetVal;
}

static UINT32 CftwFma_SetName(char TaskName[])
{
    AmbaUtility_StringAppend(TaskName, MAX_CTFW_NAME_LEN, CTFW_TASK_NAME);
    return ARM_OK;
}

static void Config_AmbaFmaMvac(AMBA_CV_FMA_MVAC_CFG_s *pCfg, const UINT32 ScaleId)
{
    UINT32 Idx;

    //set fma default config
    for(Idx = 0U; Idx < MAX_HALF_OCTAVES; ++Idx) {
        pCfg->ScaleCfg[Idx].Mode = 0U;
    }
    pCfg->ScaleCfg[ScaleId].Mode = (UINT8)CV_FMA_ENABLE_ALL;
    pCfg->ScaleCfg[ScaleId].StereoMatch.SearchRadiusX = 1U;
    pCfg->ScaleCfg[ScaleId].StereoMatch.SearchRadiusY = 1U;
    pCfg->ScaleCfg[ScaleId].StereoMatch.DistanceThUp = 45.7f;
    pCfg->ScaleCfg[ScaleId].StereoMatch.DistanceThDown = 45.7f;
    pCfg->ScaleCfg[ScaleId].StereoMatch.MatchScoreTh = 50U;

    pCfg->ScaleCfg[ScaleId].TemporalMatch.SearchRadiusX = 1U;
    pCfg->ScaleCfg[ScaleId].TemporalMatch.SearchRadiusY = 2U;
    pCfg->ScaleCfg[ScaleId].TemporalMatch.MatchScoreTh = 50U;
}

static UINT32 CftwFma_Init(const void *pAlgConfig, flexidag_memblk_t FlexiBinBlk[],
                           UINT32 MemPoolID, const flexidag_memblk_t* const pAlgBuffMap, CtfwTaskConfig_t *pTaskSetting)
{
    UINT32 RetVal = ARM_OK;
    FMA_PRIVATE_s *pPrivatePtr;
    const CTFW_FMA_TASK_CONFIG_t *pConfig;

    AmbaMisra_TouchUnused(FlexiBinBlk);

    AmbaMisra_TypeCast(&pPrivatePtr, &pAlgBuffMap->pBuffer);
    AmbaMisra_TypeCast(&pConfig, &pAlgConfig);

    // Task env setting
    {
        pTaskSetting->ProcInterval = 1U;
        pTaskSetting->TaskPriority = 50U;
        pTaskSetting->TaskCoreSel = 0U;
    }

    // Partitioning memory pool
    {
        {
            RetVal |= ArmMemPool_Allocate(MemPoolID, (UINT32)sizeof(FMA_TASK_WORK_BUFF_s), &pPrivatePtr->TaskWorkBuff);
        }
    }

    // Algo private init
    {
        FMA_TASK_WORK_BUFF_s *pTaskWorkPtr;
        SVC_CV_ALGO_HANDLE_s *pScaHandle;

        SVC_CV_ALGO_QUERY_CFG_s ScaQueryCfg;
        SVC_CV_ALGO_CREATE_CFG_s ScaCreatCfg;

        flexidag_memblk_t FlexiBin;

        flexidag_memblk_t *pAlgoBlk;

        RetVal |= AmbaWrap_memset(&ScaCreatCfg, 0, sizeof(ScaCreatCfg));
        RetVal |= AmbaWrap_memset(&FlexiBin, 0, sizeof(FlexiBin));

        AmbaMisra_TypeCast(&pTaskWorkPtr, &pPrivatePtr->TaskWorkBuff.pBuffer);
        pAlgoBlk = &pPrivatePtr->ScaAlgoBuf;

        pScaHandle = &pTaskWorkPtr->ScaHandle;

        pScaHandle->pAlgoObj = &AmbaFmaAlgoObj;
        //        pHandle->ScaHandle.pAlgoObj = pScaHandle->pAlgoObj;

        //ArmLog_STR(TASKNAME, "## call sca.query", Rval);
        //sca_query
        ScaQueryCfg.pAlgoObj = &AmbaFmaAlgoObj;
        RetVal |= AmbaFmaAlgoObj.Query(pScaHandle, &ScaQueryCfg);
        ArmLog_DBG(CTFW_TASK_NAME, "## ScaQueryCfg.TotalReqBufSz=%d, Rval=%d", ScaQueryCfg.TotalReqBufSz, RetVal);
        RetVal |= ArmMemPool_Allocate(MemPoolID, ScaQueryCfg.TotalReqBufSz, pAlgoBlk);

        if (RetVal == ARM_OK) {

            //ArmLog_STR(TASKNAME, "## call create sca.create"); fflush(stdout);
            pScaHandle->pAlgoCtrl = NULL;
            ScaCreatCfg.pAlgoBuf = pAlgoBlk;
            ScaCreatCfg.pBin[0U] = &FlexiBin;
            ScaCreatCfg.NumFD = 1U;
            RetVal = AmbaFmaAlgoObj.Create(pScaHandle, &ScaCreatCfg);
        }
        if (RetVal == ARM_OK) {
            SVC_CV_ALGO_CTRL_CFG_s ScaCtrlCfg;
            AMBA_CV_FMA_MVAC_CFG_s FmaCfg;

            Config_AmbaFmaMvac(&FmaCfg, pConfig->ScaleID);
            ScaCtrlCfg.CtrlType = AMBA_FMA_CONTROL_TYPE_MVAC;
            ScaCtrlCfg.pCtrlParam = &FmaCfg;
            RetVal |= AmbaFmaAlgoObj.Control(pScaHandle, &ScaCtrlCfg);
        }
        if (RetVal == ARM_OK) {
            UINT32 OutBufSize = ScaCreatCfg.OutputSz[0U];

            // Fma keep buffer addr internally, so allocate a buffer addtionally
            RetVal |= ArmMemPool_Allocate(MemPoolID, OutBufSize, &pTaskWorkPtr->FmaOut);
        }
        {
            pTaskWorkPtr->FexSrcTaskType = pConfig->FexSrcTaskType;
        }
    }

    return RetVal;
}
#if 0
static void EchoFmaResult(const AMBA_CV_FMA_DATA_s *out,  UINT32 ScaleId)
{
    const AMBA_CV_FMA_DATA_s *pFma;
    ULONG ULFma, IndexAddr;
    const UINT16 *pIndex;
    UINT32 i;
    AmbaMisra_TypeCast(&pFma, &out);
    AmbaMisra_TypeCast(&ULFma, &pFma);
    if (pFma->StereoMatch[ScaleId].Valid != 0U) {
        IndexAddr = ULFma + pFma->StereoMatch[ScaleId].MpIndex;
        AmbaMisra_TypeCast(&pIndex, &IndexAddr);
        for (i = 0U; i < 50U; i++) {
            AmbaPrint_PrintUInt5("StereoMatch[%d] = %d", i, pIndex[i], 0U, 0U, 0U);
        }
    } else {
        AmbaPrint_PrintUInt5("StereoMatch[%d] is invalid", ScaleId, 0U, 0U, 0U, 0U);
    }

    if (pFma->TemporalMatch[ScaleId].Valid != 0U) {
        IndexAddr = ULFma + pFma->TemporalMatch[ScaleId].MpIndex;
        AmbaMisra_TypeCast(&pIndex, &IndexAddr);
        for (i = 0U; i < 50U; i++) {
            AmbaPrint_PrintUInt5("TemporalMatch[%d] = %d", i, pIndex[i], 0U, 0U, 0U);
        }
    } else {
        AmbaPrint_PrintUInt5("TemporalMatch[%d] is invalid", ScaleId, 0U, 0U, 0U, 0U);
    }
}

static void EchoFexResult(const AMBA_CV_FEX_DATA_s *pOut, const UINT32 ScaleId)
{
    ULONG ULFex, KeyPointCntAddr, KeyPointAddr;
    const UINT8 *pKeyPointCnt;
    const AMBA_CV_FEX_COORD_s *pKeyPoints;
    UINT32 i, j;
    const AMBA_CV_FEX_DATA_s *pFex;
    AmbaMisra_TypeCast(&pFex, &pOut);
    AmbaMisra_TypeCast(&ULFex, &pFex);
    if (pFex->PrimaryList[ScaleId].Enable != 0U) {
        KeyPointCntAddr = ULFex + pFex->PrimaryList[ScaleId].KeypointsCountOffset;
        KeyPointAddr = ULFex + pFex->PrimaryList[ScaleId].KeypointsOffset;
        AmbaMisra_TypeCast(&pKeyPointCnt, &KeyPointCntAddr);
        AmbaMisra_TypeCast(&pKeyPoints, &KeyPointAddr);
        for (i = 0U; i < 1U; i++) {
            AmbaPrint_PrintUInt5("PrimaryList Bucket[%d]:", i, 0U, 0U, 0U, 0U);
            for (j = 0U; j < pKeyPointCnt[i]; j++) {
                AmbaPrint_PrintUInt5("KeyPoint.2[%d]: %d, %d", j,
                                     pKeyPoints[(i * CV_FEX_MAX_KEYPOINTS) + j].X,
                                     pKeyPoints[(i * CV_FEX_MAX_KEYPOINTS) + j].Y, 0U, 0U);
            }
        }
    } else {
        AmbaPrint_PrintUInt5("FEX PrimaryList[%d] is Disabled", ScaleId, 0U, 0U, 0U, 0U);
    }
    if (pFex->SecondaryList[ScaleId].Enable != 0U) {
        KeyPointCntAddr = ULFex + pFex->SecondaryList[ScaleId].KeypointsCountOffset;
        KeyPointAddr = ULFex + pFex->SecondaryList[ScaleId].KeypointsOffset;
        AmbaMisra_TypeCast(&pKeyPointCnt, &KeyPointCntAddr);
        AmbaMisra_TypeCast(&pKeyPoints, &KeyPointAddr);
        for (i = 0U; i < 1U; i++) {
            AmbaPrint_PrintUInt5("SecondaryList Bucket[%d]:", i, 0U, 0U, 0U, 0U);
            for (j = 0U; j < pKeyPointCnt[i]; j++) {
                AmbaPrint_PrintUInt5("KeyPoint.2[%d]: %d, %d", j,
                                     pKeyPoints[(i * CV_FEX_MAX_KEYPOINTS) + j].X,
                                     pKeyPoints[(i * CV_FEX_MAX_KEYPOINTS) + j].Y, 0U, 0U);
            }
        }
    } else {
        AmbaPrint_PrintUInt5("FEX SecondaryList[%d] is Disabled", ScaleId, 0U, 0U, 0U, 0U);
    }
}
#endif
static UINT32 CftwFma_Process(const CtfwFrameMetadata_t *pMetadata, const flexidag_memblk_t* const pAlgInputData[],
                              const flexidag_memblk_t* const pAlgBuffMap, flexidag_memblk_t* const pAlgOutputData)
{
    UINT32 RetVal = ARM_OK;

    FMA_TASK_WORK_BUFF_s *pTaskWorkPtr;
    const FMA_PRIVATE_s *pPrivatePtr;

    SVC_CV_ALGO_HANDLE_s *pScaHandle;
    AMBA_CV_FLEXIDAG_IO_s ScaInBuf;
    AMBA_CV_FLEXIDAG_IO_s ScaOutBuf;
    SVC_CV_ALGO_FEED_CFG_s ScaFeedCfg;

    const flexidag_memblk_t *pFexInBlk;

    (void) pMetadata;
    AmbaMisra_TouchUnused(pAlgOutputData);

    // Orgnize work buffer
    AmbaMisra_TypeCast(&pPrivatePtr, &pAlgBuffMap->pBuffer);
    AmbaMisra_TypeCast(&pTaskWorkPtr, &pPrivatePtr->TaskWorkBuff.pBuffer);

    // Orgnize input
    {
        if ( pTaskWorkPtr->FexSrcTaskType == CTFW_TASK_SPU_FEX ) {
            const CTFW_SPU_FEX_TASK_OUT_t *pSpuFexOut;
            AmbaMisra_TypeCast(&pSpuFexOut, &pAlgInputData[0U]->pBuffer);
            pFexInBlk = &pSpuFexOut->FexOut;
        } else {
            // By default treat the others as CTFW_TASK_FEX
            pFexInBlk = pAlgInputData[0U];
        }
    }

    // Orgnize output
    // payload of output buffer is in AMBA_CV_FMA_DATA_s;

    // Algo private routine
    // ####################################

    pScaHandle = &pTaskWorkPtr->ScaHandle;

    ScaFeedCfg.pIn = &ScaInBuf;
    ScaFeedCfg.pIn->num_of_buf = 1U;
    RetVal |= AmbaWrap_memcpy(
                  &(ScaFeedCfg.pIn->buf[0U]),
                  pFexInBlk,
                  sizeof(flexidag_memblk_t));

    ScaFeedCfg.pOut = &ScaOutBuf;
    ScaFeedCfg.pOut->num_of_buf = 1U;
    ScaFeedCfg.pOut->buf[0U] = pTaskWorkPtr->FmaOut;

    RetVal |= pScaHandle->pAlgoObj->Feed(pScaHandle, &ScaFeedCfg);

    if ( RetVal == ARM_OK ) {
        // Collect output from the one FmaOut
        RetVal |= AmbaCV_UtilityCmaMemInvalid(&(ScaFeedCfg.pOut->buf[0U]));
        RetVal |= AmbaWrap_memcpy(pAlgOutputData->pBuffer, ScaFeedCfg.pOut->buf[0U].pBuffer,
                                  ScaFeedCfg.pOut->buf[0U].buffer_size);
        if ( RetVal != ARM_OK ) {
            ArmLog_ERR(CTFW_TASK_NAME, "(%d) memcpy fail", __LINE__, 0U);
        }
    }

    return RetVal;
}

static UINT32 CftwFma_DeInit(UINT32 MemPoolID, const flexidag_memblk_t* const pAlgBuffMap)
{
    UINT32 RetVal = ARM_OK;
    FMA_PRIVATE_s *pPrivatePtr;
    FMA_TASK_WORK_BUFF_s *pTaskWorkPtr;

    AmbaMisra_TypeCast(&pPrivatePtr, &pAlgBuffMap->pBuffer);
    AmbaMisra_TypeCast(&pTaskWorkPtr, &pPrivatePtr->TaskWorkBuff.pBuffer);

    RetVal |= ArmMemPool_Free(MemPoolID, &pTaskWorkPtr->FmaOut);

    {
        SVC_CV_ALGO_HANDLE_s *pScaHandle;
        SVC_CV_ALGO_DELETE_CFG_s DeleteCfg;

        pScaHandle = &pTaskWorkPtr->ScaHandle;

        RetVal |= Ctfw_CheckRetVal(__func__, "AmbaWrap_memset", AmbaWrap_memset(&DeleteCfg, 0, sizeof(DeleteCfg)));
        // AlgoBufMemblk get free inside?
        RetVal |= AmbaFmaAlgoObj.Delete(pScaHandle, &DeleteCfg);
    }

    RetVal |= ArmMemPool_Free(MemPoolID, &pPrivatePtr->ScaAlgoBuf);
    RetVal |= ArmMemPool_Free(MemPoolID, &pPrivatePtr->TaskWorkBuff);

    return RetVal;
}

CtfwTaskObj_t gCtfwFmaTaskObj = {
    .pSetName = CftwFma_SetName,
    .pQuerySize = CftwFma_QuerySize,
    .pInit = CftwFma_Init,
    .pProcess = CftwFma_Process,
    .pDeInit = CftwFma_DeInit,
};

