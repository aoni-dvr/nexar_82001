#include "Ctfw_TaskUtil.h"
#include "Ctfw_FexTask.h"
#include "cvapi_flexidag_ambafex_cv2.h"
#include "cvapi_svccvalgo_ambafex.h"
#include "AmbaMisraFix.h"

typedef struct {
    SVC_CV_ALGO_HANDLE_s ScaHandle;
} FEX_TASK_WORK_BUFF_s;

typedef struct {
    flexidag_memblk_t TaskWorkBuff;
    flexidag_memblk_t ScaAlgoBuf;
} FEX_PRIVATE_s;

#define ALLOC_FEX_SCA_REQ_BUFF_SIZE (2UL<<20UL)
#define ALLOC_FEX_OUT_BUFF_SIZE     (2UL<<20UL)

#define CTFW_TASK_NAME "Fex"

static UINT32 CftwFex_QuerySize(UINT32 *pTaskStackSize, UINT32 *pAlgBuffMapSize, UINT32 *pAlgWorkSize, UINT32 *pAlgOutputSize)
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
        AlgBuffMapSize += CTFW_CV_MEM_ALIGN((UINT32)sizeof(FEX_PRIVATE_s));
    }

    // AlgWorkSize
    {
        {
            AlgWorkSize += CTFW_CV_MEM_ALIGN((UINT32)sizeof(FEX_TASK_WORK_BUFF_s));
        }
        {
            AlgWorkSize += CTFW_CV_MEM_ALIGN((UINT32)ALLOC_FEX_SCA_REQ_BUFF_SIZE);
        }
    }

    // AlgOutputSize
    {
        AlgOutputSize += CTFW_CV_MEM_ALIGN((UINT32)ALLOC_FEX_OUT_BUFF_SIZE);
    }

    *pAlgBuffMapSize = AlgBuffMapSize;
    *pAlgWorkSize = AlgWorkSize;
    *pAlgOutputSize = AlgOutputSize;

    return RetVal;
}

static UINT32 CftwFex_SetName(char TaskName[])
{
    AmbaUtility_StringAppend(TaskName, MAX_CTFW_NAME_LEN, CTFW_TASK_NAME);
    return ARM_OK;
}

static UINT32 CftwFex_Init(const void *pAlgConfig, flexidag_memblk_t FlexiBinBlk[],
                           UINT32 MemPoolID, const flexidag_memblk_t* const pAlgBuffMap, CtfwTaskConfig_t *pTaskSetting)
{
    UINT32 RetVal = ARM_OK;
    FEX_PRIVATE_s *pPrivatePtr;

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
            RetVal |= ArmMemPool_Allocate(MemPoolID, (UINT32)sizeof(FEX_TASK_WORK_BUFF_s), &pPrivatePtr->TaskWorkBuff);
        }
    }

    // Algo private init
    if ( RetVal == ARM_OK ) {
        FEX_TASK_WORK_BUFF_s *pTaskWorkPtr;
        SVC_CV_ALGO_HANDLE_s *pScaHandle;

        SVC_CV_ALGO_QUERY_CFG_s ScaQueryCfg;
        SVC_CV_ALGO_CREATE_CFG_s ScaCreatCfg;

        flexidag_memblk_t FlexiBin;

        flexidag_memblk_t *pAlgoBlk;

        AmbaMisra_TypeCast(&pTaskWorkPtr, &pPrivatePtr->TaskWorkBuff.pBuffer);
        pAlgoBlk = &pPrivatePtr->ScaAlgoBuf;

        pScaHandle = &pTaskWorkPtr->ScaHandle;

        pScaHandle->pAlgoObj = &AmbaFexAlgoObj;
        //        pHandle->ScaHandle.pAlgoObj = pScaHandle->pAlgoObj;

        //ArmLog_STR(CTFW_TASK_NAME, "## call sca.query", Rval);
        //sca_query
        ScaQueryCfg.pAlgoObj = &AmbaFexAlgoObj;
        RetVal |= AmbaFexAlgoObj.Query(pScaHandle, &ScaQueryCfg);
        ArmLog_DBG(CTFW_TASK_NAME, "## ScaQueryCfg.TotalReqBufSz=%d, Rval=%d", ScaQueryCfg.TotalReqBufSz, RetVal);
        RetVal |= ArmMemPool_Allocate(MemPoolID, ScaQueryCfg.TotalReqBufSz, pAlgoBlk);

        if (RetVal == ARM_OK) {

            //ArmLog_STR(CTFW_TASK_NAME, "## call create sca.create"); fflush(stdout);
            RetVal |= Ctfw_CheckRetVal(__func__, "AmbaWrap_memset", AmbaWrap_memset(&ScaCreatCfg, 0, sizeof(ScaCreatCfg)));
            RetVal |= Ctfw_CheckRetVal(__func__, "AmbaWrap_memset", AmbaWrap_memset(&FlexiBin, 0, sizeof(FlexiBin)));
            pScaHandle->pAlgoCtrl = NULL;
            ScaCreatCfg.pAlgoBuf = pAlgoBlk;
            ScaCreatCfg.pBin[0U] = &FlexiBin;
            ScaCreatCfg.NumFD = 1U;
            RetVal |= AmbaFexAlgoObj.Create(pScaHandle, &ScaCreatCfg);
        }
    }

    return RetVal;
}

#if 0
static void EchoFexResult(const AMBA_CV_FEX_DATA_s *pOut, const UINT32 ScaleId)
{
    ULONG ULFex, KeyPointCntAddr, KeyPointAddr;
    const UINT8 *pKeyPointCnt;
    const AMBA_CV_FEX_COORD_s *pKeyPoints;
    UINT32 i, j;
    const AMBA_CV_FEX_DATA_s *pFex;

    (void)AmbaWrap_memcpy(&pFex, &pOut, sizeof(void*));
    (void)AmbaWrap_memcpy(&ULFex, &pFex, sizeof(void*));

    if (pFex->PrimaryList[ScaleId].Enable != 0U) {
        KeyPointCntAddr = ULFex + pFex->PrimaryList[ScaleId].KeypointsCountOffset;
        KeyPointAddr = ULFex + pFex->PrimaryList[ScaleId].KeypointsOffset;
        (void)AmbaWrap_memcpy(&pKeyPointCnt, &KeyPointCntAddr, sizeof(void*));
        (void)AmbaWrap_memcpy(&pKeyPoints, &KeyPointAddr, sizeof(void*));

        //for (i = 0U; i < 64U; i++) {
        for (i = 32U; i < 33U; i++) {
            ArmLog_DBG(CTFW_TASK_NAME, "PrimaryList Bucket[%d], KeyPtNum = %d", i, pKeyPointCnt[i]);
            for (j = 0U; j < pKeyPointCnt[i]; j++) {
                ArmLog_DBG(CTFW_TASK_NAME, "KeyPoint[%d]:", j, 0U);
                ArmLog_DBG(CTFW_TASK_NAME, "%d, %d", pKeyPoints[(i * CV_FEX_MAX_KEYPOINTS) + j].X,
                           pKeyPoints[(i * CV_FEX_MAX_KEYPOINTS) + j].Y);
            }
        }
    } else {
        ArmLog_DBG(CTFW_TASK_NAME, "FEX PrimaryList[%d] is Disabled", ScaleId, 0U);
    }
    if (pFex->SecondaryList[ScaleId].Enable != 0U) {
        KeyPointCntAddr = ULFex + pFex->SecondaryList[ScaleId].KeypointsCountOffset;
        KeyPointAddr = ULFex + pFex->SecondaryList[ScaleId].KeypointsOffset;
        (void)AmbaWrap_memcpy(&pKeyPointCnt, &KeyPointCntAddr, sizeof(void*));
        (void)AmbaWrap_memcpy(&pKeyPoints, &KeyPointAddr, sizeof(void*));
        //for (i = 0U; i < 64U; i++) {
        for (i = 32U; i < 33U; i++) {
            ArmLog_DBG(CTFW_TASK_NAME, "SecondaryList Bucket[%d], KeyPtNum = %d", i, pKeyPointCnt[i]);
            for (j = 0U; j < pKeyPointCnt[i]; j++) {
                ArmLog_DBG(CTFW_TASK_NAME, "KeyPoint[%d]:", j, 0U);
                ArmLog_DBG(CTFW_TASK_NAME, "%d, %d", pKeyPoints[(i * CV_FEX_MAX_KEYPOINTS) + j].X,
                           pKeyPoints[(i * CV_FEX_MAX_KEYPOINTS) + j].Y);
            }
        }
    } else {
        ArmLog_DBG(CTFW_TASK_NAME, "FEX SecondaryList[%d] is Disabled", ScaleId, 0U);
    }
}


#endif

static UINT32 CftwFex_Process(const CtfwFrameMetadata_t *pMetadata, const flexidag_memblk_t* const pAlgInputData[],
                              const flexidag_memblk_t* const pAlgBuffMap, flexidag_memblk_t* const pAlgOutputData)
{
    UINT32 RetVal = ARM_OK;

    FEX_TASK_WORK_BUFF_s *pTaskWorkPtr;
    const FEX_PRIVATE_s *pPrivatePtr;

    SVC_CV_ALGO_HANDLE_s *pScaHandle;
    AMBA_CV_FLEXIDAG_IO_s ScaInBuf;
    AMBA_CV_FLEXIDAG_IO_s ScaOutBuf;
    SVC_CV_ALGO_FEED_CFG_s ScaFeedCfg;

    (void) pMetadata;
    AmbaMisra_TouchUnused(pAlgOutputData);

    // Orgnize input
    // payload of input buffer is in memio_source_recv_picinfo_t;
#if 0
    if (0U) {
        // Debug
        memio_source_recv_picinfo_t *pIn;
        (void) AmbaWrap_memcpy(&pIn, &pAlgInputData[0U]->pBuffer, sizeof(void*));
        ArmLog_ERR(CTFW_TASK_NAME, "pIn->pic_info.pyramid.image_width_m1 = %d", pIn->pic_info.pyramid.image_width_m1, 0U);
        (void) AmbaKAL_TaskSleep(300);
    }
#endif

    // Orgnize work buffer
    AmbaMisra_TypeCast(&pPrivatePtr, &pAlgBuffMap->pBuffer);
    AmbaMisra_TypeCast(&pTaskWorkPtr, &pPrivatePtr->TaskWorkBuff.pBuffer);

    // Orgnize output
    // payload of output buffer is in AMBA_CV_FEX_DATA_s;

    // Algo private routine
    // ####################################

    pScaHandle = &pTaskWorkPtr->ScaHandle;

    ScaFeedCfg.pIn = &ScaInBuf;
    ScaFeedCfg.pIn->num_of_buf = 1U;
    ScaFeedCfg.pIn->buf[0U] = *(pAlgInputData[0U]);

    ScaFeedCfg.pOut = &ScaOutBuf;
    ScaFeedCfg.pOut->num_of_buf = 1U;
    ScaFeedCfg.pOut->buf[0U] = *pAlgOutputData;

    RetVal |= pScaHandle->pAlgoObj->Feed(pScaHandle, &ScaFeedCfg);

#if 0
    {
        AMBA_CV_FEX_BUF_s *pFexOut;
        UINT32 ScaleID = 2U;
        (void) AmbaWrap_memcpy(&pFexOut, &pAlgOutputData, sizeof(void*));
        EchoFexResult(pFexOut->pBuffer, ScaleID);
    }
#endif

    return RetVal;
}

static UINT32 CftwFex_DeInit(UINT32 MemPoolID, const flexidag_memblk_t* const pAlgBuffMap)
{
    UINT32 RetVal = ARM_OK;
    FEX_PRIVATE_s *pPrivatePtr;
    FEX_TASK_WORK_BUFF_s *pTaskWorkPtr;

    AmbaMisra_TypeCast(&pPrivatePtr, &pAlgBuffMap->pBuffer);
    AmbaMisra_TypeCast(&pTaskWorkPtr, &pPrivatePtr->TaskWorkBuff.pBuffer);

    {
        SVC_CV_ALGO_HANDLE_s *pScaHandle;
        SVC_CV_ALGO_DELETE_CFG_s DeleteCfg;

        pScaHandle = &pTaskWorkPtr->ScaHandle;

        RetVal |= Ctfw_CheckRetVal(__func__, "AmbaWrap_memset", AmbaWrap_memset(&DeleteCfg, 0, sizeof(DeleteCfg)));
        // AlgoBufMemblk get free inside?
        RetVal |= AmbaFexAlgoObj.Delete(pScaHandle, &DeleteCfg);
    }

    RetVal |= ArmMemPool_Free(MemPoolID, &pPrivatePtr->ScaAlgoBuf);
    RetVal |= ArmMemPool_Free(MemPoolID, &pPrivatePtr->TaskWorkBuff);

    return RetVal;
}

CtfwTaskObj_t gCtfwFexTaskObj = {
    .pSetName = CftwFex_SetName,
    .pQuerySize = CftwFex_QuerySize,
    .pInit = CftwFex_Init,
    .pProcess = CftwFex_Process,
    .pDeInit = CftwFex_DeInit,
};

