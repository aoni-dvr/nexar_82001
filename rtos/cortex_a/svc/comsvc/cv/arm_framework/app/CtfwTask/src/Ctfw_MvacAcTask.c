#include "Ctfw_TaskUtil.h"
#include "Ctfw_MvacAcTask.h"
#include "Ctfw_SpuFexTask.h"
#include "AmbaMisraFix.h"

typedef struct {
    AMBA_CV_MVAC_CFG_s MvacConfig;
    AMBA_MVAC_FD_HANDLE_s MvacFdHandler;
    flexidag_memblk_t InMetaRight;
    flexidag_memblk_t InMetaLeft;
    UINT32 MvacAlgoState;
    flexidag_memblk_t StateBuf;
    UINT32 FexSrcTaskType;
} MVAC_AC_TASK_WORK_BUFF_s;

typedef struct {
    flexidag_memblk_t TaskWorkBuff;
} MVAC_AC_PRIVATE_s;

#define ALLOC_MVAC_AC_STATE_BUFF_SIZE (16UL<<20UL)

#define CTFW_TASK_NAME "MvacAc"

static UINT32 CftwMvacAc_QuerySize(UINT32 *pTaskStackSize, UINT32 *pAlgBuffMapSize, UINT32 *pAlgWorkSize, UINT32 *pAlgOutputSize)
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
        AlgBuffMapSize += CTFW_CV_MEM_ALIGN((UINT32)sizeof(MVAC_AC_PRIVATE_s));
    }

    // AlgWorkSize
    {
        {
            AlgWorkSize += CTFW_CV_MEM_ALIGN((UINT32)sizeof(MVAC_AC_TASK_WORK_BUFF_s));
        }
        {
            AlgWorkSize += CTFW_CV_MEM_ALIGN((UINT32)ALLOC_MVAC_AC_STATE_BUFF_SIZE);
        }
    }

    // AlgOutputSize
    {
        AlgOutputSize += CTFW_CV_MEM_ALIGN((UINT32)sizeof(AMBA_CV_MVAC_DATA_s));
    }

    *pAlgBuffMapSize = AlgBuffMapSize;
    *pAlgWorkSize = AlgWorkSize;
    *pAlgOutputSize = AlgOutputSize;

    return RetVal;
}

static UINT32 CftwMvacAc_SetName(char TaskName[])
{
    AmbaUtility_StringAppend(TaskName, MAX_CTFW_NAME_LEN, CTFW_TASK_NAME);
    return ARM_OK;
}

static UINT32 CftwMvacAc_Init(const void *pAlgConfig, flexidag_memblk_t FlexiBinBlk[],
                              UINT32 MemPoolID, const flexidag_memblk_t* const pAlgBuffMap, CtfwTaskConfig_t *pTaskSetting)
{
    UINT32 RetVal = ARM_OK;
    MVAC_AC_PRIVATE_s *pPrivatePtr;

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
            RetVal |= ArmMemPool_Allocate(MemPoolID, (UINT32)sizeof(MVAC_AC_TASK_WORK_BUFF_s), &pPrivatePtr->TaskWorkBuff);
        }
    }

    // Algo private init
    {
        MVAC_AC_TASK_WORK_BUFF_s *pTaskWorkPtr;
        AMBA_CV_MVAC_CFG_s *pMvacConfig;
        AMBA_MVAC_FD_HANDLE_s *pMvacHandler;
        CTFW_MVAC_AC_TASK_CONFIG_t *pInConfig;

        flexidag_memblk_t FlexiDagBinBuf;
        flexidag_memblk_t TempBuf;
        UINT32 FlexiDagBinSz = 0U;
        UINT32 StateBufSz;
        UINT32 TempBufSz;
        UINT32 OutputBufNum;
        UINT32 OutputBufSz;

        AmbaMisra_TypeCast(&pTaskWorkPtr, &pPrivatePtr->TaskWorkBuff.pBuffer);
        AmbaMisra_TypeCast(&pInConfig, &pAlgConfig);

        pTaskWorkPtr->FexSrcTaskType = pInConfig->FexSrcTaskType;
        pMvacConfig = &pTaskWorkPtr->MvacConfig;
        pMvacHandler = &pTaskWorkPtr->MvacFdHandler;

        if (RetVal == ARM_OK) {
            RetVal = AmbaMVAC_Open(&FlexiDagBinBuf, FlexiDagBinSz, &StateBufSz, &TempBufSz, &OutputBufNum, &OutputBufSz, pMvacHandler);
        }

        if (RetVal == ARM_OK) {
            ArmLog_DBG(CTFW_TASK_NAME, "## AmbaMVAC_Open StateBufSz:%d TempBufSz:%d\n", StateBufSz, TempBufSz);
            ArmLog_DBG(CTFW_TASK_NAME, "## AmbaMVAC_Open OutputBufNum:%d OutputBufSz:%d\n", OutputBufNum, OutputBufSz);
            RetVal |= ArmMemPool_Allocate(MemPoolID, StateBufSz, &pTaskWorkPtr->StateBuf);
            //Rval |= ArmMemPool_Allocate(pHandle->MemPoolId, sizeof(AMBA_CV_VO_DATA_s), &OutBuf);
            RetVal |= AmbaMVAC_Init(pMvacHandler, &pTaskWorkPtr->StateBuf, &TempBuf);
        }

        if (RetVal == ARM_OK) {

            //sc5
            //        pMvacConfig->View.WarpTableInfo.HorizontalGridNumber = 128;
            //        pMvacConfig->View.WarpTableInfo.VerticalGridNumber = 96;
            //        pMvacConfig->View.WarpTableInfo.TileWidthQ16 = 2097152;
            //        pMvacConfig->View.WarpTableInfo.TileHeightQ16 = 2097152;
            //        pMvacConfig->View.ScaleId = 0;
            RetVal = AmbaWrap_memcpy(&(pMvacConfig->View), &pInConfig->MvacViewInfo, sizeof(AMBA_CV_MVAC_VIEW_INFO_s));
            if ( RetVal != ARM_OK ) {
                ArmLog_ERR(CTFW_TASK_NAME, "(%d) memcpy fail", __LINE__, 0U);
            }
            //pMvacConfig->pInMetaLeft = pInitMetaLeftBuf;
            AmbaMisra_TypeCast(&(pMvacConfig->pInMetaLeft), &pInConfig->pInitMetaLeftBuf);
            pMvacConfig->MaxIterations = 100U;
            pMvacConfig->MinEpipolarScoreThreshold = 0.0f;
            pMvacConfig->MaxFrameSpeedThreshold = 0.5f;
            pMvacConfig->MinNumDepthThreshold = 200U;
            pMvacConfig->InhibitedFrameSlotSize = 300U;
            pMvacConfig->Mode = AMBA_CV_MVAC_MODE_CONFIG;
            ArmLog_DBG(CTFW_TASK_NAME, "## ScaleId = %d", pMvacConfig->View.ScaleId, 0U);
            ArmLog_DBG(CTFW_TASK_NAME, "## WarpTableInfo.HorizontalGridNumber = %d", pMvacConfig->View.WarpTableInfo.HorizontalGridNumber, 0U);
            ArmLog_DBG(CTFW_TASK_NAME, "## WarpTableInfo.VerticalGridNumber   = %d", pMvacConfig->View.WarpTableInfo.VerticalGridNumber, 0U);
            ArmLog_DBG(CTFW_TASK_NAME, "## WarpTableInfo.TileWidthQ16         = %d", pMvacConfig->View.WarpTableInfo.TileWidthQ16, 0U);
            ArmLog_DBG(CTFW_TASK_NAME, "## WarpTableInfo.TileHeightQ16        = %d", pMvacConfig->View.WarpTableInfo.TileHeightQ16, 0U);

            RetVal |= AmbaMVAC_Cfg(pMvacHandler, pMvacConfig);

            //        pMvacConfig->Mode = AMBA_CV_MVAC_MODE_CONTROL;
            //        pMvacConfig->ControlState = AMBA_CV_MVAC_CONTROL_START;
            //        AmbaMVAC_Cfg(pMvacHandler, pMvacConfig);
        }
    }

    return RetVal;
}

static UINT32 CftwMvacAc_Process(const CtfwFrameMetadata_t *pMetadata, const flexidag_memblk_t* const pAlgInputData[],
                                 const flexidag_memblk_t* const pAlgBuffMap, flexidag_memblk_t* const pAlgOutputData)
{
    UINT32 RetVal = ARM_OK;

    MVAC_AC_TASK_WORK_BUFF_s *pTaskWorkPtr;
    MVAC_AC_PRIVATE_s *pPrivatePtr;

    AMBA_MVAC_FD_HANDLE_s *pMvacFdHandler;

    AMBA_CV_FEX_BUF_s *pIn0_FexBuf;
    AMBA_CV_FMA_BUF_s *pIn1_CurFmaBuf;
    AMBA_CV_VO_BUF_s *pIn2_VOBuf;
    AMBA_CV_MVAC_BUF_s *pOut;

    AMBA_CV_META_BUF_s *pInMetaLeft;
    AMBA_CV_META_BUF_s *pInMetaRight;
    const void *pVoid;

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
    AmbaMisra_TypeCast(&pIn2_VOBuf, &pAlgInputData[2U]);

    // Orgnize output
    // Note that output is already in format of flexidag_memblk_t, so not to copy pBuffer
    AmbaMisra_TypeCast(&pOut, &pAlgOutputData);

    // Algo private routine
    // ####################################

    pMvacFdHandler = &(pTaskWorkPtr->MvacFdHandler);

    //read metadata
    //... fread the file we need to workaround ...
    pVoid = &(pMetadata->CalibInfo.Left);
    AmbaMisra_TypeCast(&pInMetaLeft, &pVoid);
    pVoid = &(pMetadata->CalibInfo.Right);
    AmbaMisra_TypeCast(&pInMetaRight, &pVoid);
    //    ArmLog_STR(TASKNAME, "## Process, pInMetaLeft->pBuffer=%x, size=%d",
    //            pInMetaLeft->pBuffer, pInMetaLeft->buffer_size); //fflush(stdout);
    //    ArmLog_STR(TASKNAME, "## Process, pInMetaRight->pBuffer=%x, size=%d",
    //            pInMetaRight->pBuffer, pInMetaRight->buffer_size); //fflush(stdout);

    RetVal = AmbaMVAC_GetResult(pMvacFdHandler, pIn2_VOBuf, pIn1_CurFmaBuf,
                                pIn0_FexBuf, pInMetaLeft, pInMetaRight, pOut);

    {
        const AMBA_CV_MVAC_DATA_s *pOutData = pOut->pBuffer;
//
//        switch (pOutData->State) {
//            case MVAC_STATE_RESET:
//                //printf("[ut]state: RESET\n");
//                break;
//            case MVAC_STATE_WAIT:
//                //printf("[ut]state: WAIT\n");
//                break;
//            case MVAC_STATE_ACCUMULATION:
//                printf("[ut]state: ACCUMULATION\n");
//                if(pOutData->WarpInfoValid == 1){
//                    printf("[ut] generate warp table at MVAC_STATE_ACCUMULATION\n");
//                }
//                break;
//            case MVAC_STATE_SOLVE:
//                printf("[ut]state: SOLVE\n");
//                if(pOutData->WarpInfoValid == 1){
//                    UINT32 WriteSize;
//
//                    printf("[ut] generate warp table at MVAC_STATE_SOLVE\n");
//                    Rval = ArmFIO_Save(&(pOutData->pWarpInfo->CalibrationUpdates[0].WarpTable[0]),
//                            sizeof(pOutData->pWarpInfo->CalibrationUpdates[0].WarpTable[0]),
//                            "/tmp/nfs/ac/out/warp_left.bin",
//                            &WriteSize);
//                }
//                break;
//            case MVAC_STATE_END:
//                printf("[ut]state: END\n");
//                if(pOutData->WarpInfoValid == 1){
//                    UINT32 WriteSize;
//
//                    printf("[ut] generate warp table at MVAC_STATE_END\n");
//                    Rval = ArmFIO_Save(&(pOutData->pWarpInfo->CalibrationUpdates[0].WarpTable[0]),
//                            sizeof(pOutData->pWarpInfo->CalibrationUpdates[0].WarpTable[0]),
//                            "/tmp/nfs/ac/out/warp_left.bin",
//                            &WriteSize);
//
//                }
//
//                gAllFrameProcDone = 1U;
//                break;
//            case MVAC_STATE_SKIP:
//                //printf("[ut]state: SKIP\n");
//                if(pOutData->WarpInfoValid == 1){
//                    printf("[ut] generate warp table at MVAC_STATE_SKIP\n");
//                }
//                break;
//            default:
//                printf("[ut]state: ERROR\n");
//        }
//
//

        pTaskWorkPtr->MvacAlgoState = pOutData->State;
    }

    return RetVal;
}

static UINT32 CftwMvacAc_DeInit(UINT32 MemPoolID, const flexidag_memblk_t* const pAlgBuffMap)
{
    UINT32 RetVal = ARM_OK;
    MVAC_AC_TASK_WORK_BUFF_s *pTaskWorkPtr;
    MVAC_AC_PRIVATE_s *pPrivatePtr;

    AmbaMisra_TypeCast(&pPrivatePtr, &pAlgBuffMap->pBuffer);
    AmbaMisra_TypeCast(&pTaskWorkPtr, &pPrivatePtr->TaskWorkBuff.pBuffer);

    RetVal |= ArmMemPool_Free(MemPoolID, &pTaskWorkPtr->StateBuf);

    if (RetVal == ARM_OK) {
        RetVal |= ArmMemPool_Free(MemPoolID, &pPrivatePtr->TaskWorkBuff);
    }

    return RetVal;
}

CtfwTaskObj_t gCtfwMvacAcTaskObj = {
    .pSetName = CftwMvacAc_SetName,
    .pQuerySize = CftwMvacAc_QuerySize,
    .pInit = CftwMvacAc_Init,
    .pProcess = CftwMvacAc_Process,
    .pDeInit = CftwMvacAc_DeInit,
};

