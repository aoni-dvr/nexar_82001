#include "Ctfw_TaskUtil.h"
#include "Ctfw_MvacVoTask.h"
#include "Ctfw_SpuFexTask.h"
#include "AmbaMisraFix.h"

typedef struct {
    AMBA_CV_VO_CFG_s VoConfig;
    AMBA_VO_FD_HANDLE_s VoFdHandler;
    AMBA_CV_FMA_BUF_s PreFmaBuf;
    flexidag_memblk_t StateBuf;
    flexidag_memblk_t TempBuf;
    UINT32 FexSrcTaskType;
} MVAC_VO_TASK_WORK_BUFF_s;

typedef struct {
    flexidag_memblk_t TaskWorkBuff;
} MVAC_VO_PRIVATE_s;

#define ALLOC_MVAC_VO_STATE_BUFF_SIZE (4UL<<20UL)

#define CTFW_TASK_NAME "MvacVo"

static UINT32 CftwMvacVo_QuerySize(UINT32 *pTaskStackSize, UINT32 *pAlgBuffMapSize, UINT32 *pAlgWorkSize, UINT32 *pAlgOutputSize)
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
        AlgBuffMapSize += CTFW_CV_MEM_ALIGN((UINT32)sizeof(MVAC_VO_PRIVATE_s));
    }

    // AlgWorkSize
    {
        {
            AlgWorkSize += CTFW_CV_MEM_ALIGN((UINT32)sizeof(MVAC_VO_TASK_WORK_BUFF_s));
        }
        {
            AlgWorkSize += CTFW_CV_MEM_ALIGN((UINT32)ALLOC_MVAC_VO_STATE_BUFF_SIZE);
        }
    }

    // AlgOutputSize
    {
        AlgOutputSize += CTFW_CV_MEM_ALIGN((UINT32)sizeof(AMBA_CV_VO_DATA_s));
    }

    *pAlgBuffMapSize = AlgBuffMapSize;
    *pAlgWorkSize = AlgWorkSize;
    *pAlgOutputSize = AlgOutputSize;

    return RetVal;
}

static UINT32 CftwMvacVo_SetName(char TaskName[])
{
    AmbaUtility_StringAppend(TaskName, MAX_CTFW_NAME_LEN, CTFW_TASK_NAME);
    return ARM_OK;
}

static UINT32 CftwMvacVo_Init(const void *pAlgConfig, flexidag_memblk_t FlexiBinBlk[],
                              UINT32 MemPoolID, const flexidag_memblk_t* const pAlgBuffMap, CtfwTaskConfig_t *pTaskSetting)
{
    UINT32 RetVal = ARM_OK;
    MVAC_VO_PRIVATE_s *pPrivatePtr;

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
            RetVal |= ArmMemPool_Allocate(MemPoolID, (UINT32)sizeof(MVAC_VO_TASK_WORK_BUFF_s), &pPrivatePtr->TaskWorkBuff);
        }
    }

    // Algo private init
    {
        MVAC_VO_TASK_WORK_BUFF_s *pTaskWorkPtr;
        AMBA_CV_VO_CFG_s *pVoConfig;
        AMBA_VO_FD_HANDLE_s *pVoFdHandler;
        CTFW_MVAC_VO_TASK_CONFIG_t *pInConfig;
        flexidag_memblk_t FlexiDagBinBuf;
        flexidag_memblk_t StateBuf;
        flexidag_memblk_t TempBuf;
        UINT32 FlexiDagBinSz = 0;
        UINT32 StateBufSz;
        UINT32 TempBufSz;
        UINT32 OutputBufNum;
        UINT32 OutputBufSz;

        AmbaMisra_TypeCast(&pTaskWorkPtr, &pPrivatePtr->TaskWorkBuff.pBuffer);
        AmbaMisra_TypeCast(&pInConfig, &pAlgConfig);

        pTaskWorkPtr->FexSrcTaskType = pInConfig->FexSrcTaskType;
        pVoConfig = &pTaskWorkPtr->VoConfig;
        pVoFdHandler = &pTaskWorkPtr->VoFdHandler;
        RetVal |= AmbaWrap_memset(&pTaskWorkPtr->PreFmaBuf, 0, sizeof(AMBA_CV_FMA_BUF_s));

        RetVal |= AmbaVO_Open(&FlexiDagBinBuf, FlexiDagBinSz, &StateBufSz, &TempBufSz, &OutputBufNum, &OutputBufSz, pVoFdHandler);

        if (RetVal == ARM_OK) {
            ArmLog_DBG(CTFW_TASK_NAME, "## AmbaVO_Open StateBufSz:%d TempBufSz:%d\n", StateBufSz, TempBufSz);
            ArmLog_DBG(CTFW_TASK_NAME, "## AmbaVO_Open OutputBufNum:%d OutputBufSz:%d\n", OutputBufNum, OutputBufSz);
            RetVal |= ArmMemPool_Allocate(MemPoolID, StateBufSz, &StateBuf);
            //Rval |= ArmMemPool_Allocate(pHandle->MemPoolId, sizeof(AMBA_CV_VO_DATA_s), &OutBuf);
            RetVal |= AmbaVO_Init(pVoFdHandler, &StateBuf, &TempBuf);
        }

        if (RetVal == ARM_OK) {
            RetVal |= Ctfw_CheckRetVal(__func__, "AmbaWrap_memcpy", AmbaWrap_memcpy(pVoConfig, &pInConfig->VoCfg, sizeof(AMBA_CV_VO_CFG_s)));
            //pVoConfig->pInitMetaRightBuf = pInitMetaRightBuf;
            AmbaMisra_TypeCast(&(pVoConfig->pInMetaRight), &(pInConfig->pInitMetaRightBuf));
            RetVal |= AmbaVO_Cfg(pVoFdHandler, pVoConfig);
        }
    }

    return RetVal;
}

static UINT32 CftwMvacVo_Process(const CtfwFrameMetadata_t *pMetadata, const flexidag_memblk_t* const pAlgInputData[],
                                 const flexidag_memblk_t* const pAlgBuffMap, flexidag_memblk_t* const pAlgOutputData)
{
    UINT32 RetVal = ARM_OK;

    MVAC_VO_TASK_WORK_BUFF_s *pTaskWorkPtr;
    MVAC_VO_PRIVATE_s *pPrivatePtr;

    AMBA_VO_FD_HANDLE_s *pVoFdHandler;

    AMBA_CV_FMA_BUF_s *pPreFmaBuf;
    AMBA_CV_FEX_BUF_s *pIn0_FexBuf;
    AMBA_CV_FMA_BUF_s *pIn1_CurFmaBuf;
    AMBA_CV_VO_BUF_s *pOut;

    (void) pMetadata;
    //AmbaMisra_TouchUnused(pAlgOutputData);

    // Orgnize work buffer
    AmbaMisra_TypeCast(&pPrivatePtr, &pAlgBuffMap->pBuffer);
    AmbaMisra_TypeCast(&pTaskWorkPtr, &pPrivatePtr->TaskWorkBuff.pBuffer);

    // Orgnize input
    {
        if ( pTaskWorkPtr->FexSrcTaskType == CTFW_TASK_SPU_FEX ) {
            CTFW_SPU_FEX_TASK_OUT_t *pSpuFexOut;
            flexidag_memblk_t *pFexOut;
            AmbaMisra_TypeCast(&pSpuFexOut, &pAlgInputData[0U]->pBuffer);
            pFexOut = &pSpuFexOut->FexOut;
            AmbaMisra_TypeCast(&pIn0_FexBuf, &pFexOut);
        } else {
            // By default treat the others as CTFW_TASK_FEX
            AmbaMisra_TypeCast(&pIn0_FexBuf, &pAlgInputData[0U]);
        }
    }
    AmbaMisra_TypeCast(&pIn1_CurFmaBuf, &pAlgInputData[1U]);

    // Orgnize output
    // Note that output is already in format of flexidag_memblk_t, so not to copy pBuffer
    AmbaMisra_TypeCast(&pOut, &pAlgOutputData);

    // Algo private routine
    // ####################################
    pVoFdHandler = &(pTaskWorkPtr->VoFdHandler);

    pPreFmaBuf = &(pTaskWorkPtr->PreFmaBuf);

    RetVal = AmbaVO_GetResult(pVoFdHandler, pPreFmaBuf, pIn1_CurFmaBuf, pIn0_FexBuf, pOut);

    // Debug
#if 0
    if(0U) {
        char DBStr0[50U];
        char DBStr1[50U];

        (void)AmbaUtility_DoubleToStr(&DBStr0[0U],
                                      sizeof(DBStr0), (DOUBLE)pOut->pBuffer->Rotation[0U], 5U);
        (void)AmbaUtility_DoubleToStr(&DBStr1[0U],
                                      sizeof(DBStr1), (DOUBLE)pOut->pBuffer->Rotation[1U], 5U);
        ArmLog_STR(CTFW_TASK_NAME, "############ Rot[0] = %s, Rot[1] = %s", DBStr0, DBStr1);
    }
#endif

    //backup PrevFma
    pTaskWorkPtr->PreFmaBuf = *pIn1_CurFmaBuf;

    return RetVal;
}

static UINT32 CftwMvacVo_DeInit(UINT32 MemPoolID, const flexidag_memblk_t* const pAlgBuffMap)
{
    UINT32 RetVal = ARM_OK;
    MVAC_VO_TASK_WORK_BUFF_s *pTaskWorkPtr;
    MVAC_VO_PRIVATE_s *pPrivatePtr;

    AmbaMisra_TypeCast(&pPrivatePtr, &pAlgBuffMap->pBuffer);
    AmbaMisra_TypeCast(&pTaskWorkPtr, &pPrivatePtr->TaskWorkBuff.pBuffer);

    RetVal |= ArmMemPool_Free(MemPoolID, &pTaskWorkPtr->StateBuf);

    if (RetVal == ARM_OK) {
        RetVal |= ArmMemPool_Free(MemPoolID, &pPrivatePtr->TaskWorkBuff);
    }

    return RetVal;
}

CtfwTaskObj_t gCtfwMvacVoTaskObj = {
    .pSetName = CftwMvacVo_SetName,
    .pQuerySize = CftwMvacVo_QuerySize,
    .pInit = CftwMvacVo_Init,
    .pProcess = CftwMvacVo_Process,
    .pDeInit = CftwMvacVo_DeInit,
};

