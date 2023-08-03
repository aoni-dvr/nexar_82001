#include "Ctfw_TaskUtil.h"
#include "Ctfw_StixelTask.h"
#include "AmbaMisraFix.h"

#if defined(CONFIG_SOC_CV2FS)
#include "cvapi_flexidag_ambaspufex_def_cv2fs.h"
#else
#include "cvapi_flexidag_ambaspufex_def_cv2.h"
#endif

#define MAX_STL_FX_IN_BUFFER_NUM        (8U)
#define MAX_STL_FX_OUT_BUFFER_NUM       (8U)

#define STIXEL_FLEXIDAG_SYNC_MSG_BUFF_DEPTH (4U)

#define STIXEL_INSTANCE_IDX_0    (0U)
#define STIXEL_INSTANCE_IDX_1    (1U)
#define STIXEL_INSTANCE_NUM      (2U)

#define APP_STIXEL_TSK_CV_OUT_BUFF_SIZE (1UL<<20UL)

typedef struct {
    UINT32 AlgoIndex;
} STIXEL_SYNC_VP_MSG_s;

typedef struct {
    UINT32 InstId;

    SVC_CV_ALGO_HANDLE_s ScaHandles[STIXEL_FX_IDX_NUM];
    CTFW_STIXEL_TASK_CONFIG_t TaskConfig;
    CAM_PITCH_DET_MANUAL_HDLR_s ManualDetHdlr;

    CAM_PITCH_DET_MANUAL_PRIOR_ESTI_s PriorEstimatedGround;

    UINT32 VPOutputNum[STIXEL_FX_IDX_NUM];
    UINT32 VPOutputNonAlignedSz[STIXEL_FX_IDX_NUM][MAX_STL_FX_OUT_BUFFER_NUM];

    STIXEL_SYNC_VP_MSG_s VPRunUserData[STIXEL_FX_IDX_NUM];

    // For synchronizing each flexiDag
    ArmMsgQueue_t FlexiSyncMsgQ;
    char FlexiSyncMsgQName[MAX_CTFW_NAME_LEN];
    STIXEL_SYNC_VP_MSG_s FlexiSyncMsgQBuff[STIXEL_FLEXIDAG_SYNC_MSG_BUFF_DEPTH];
} STIXEL_TASK_WORK_BUFF_s;

typedef struct {
    flexidag_memblk_t TaskWorkBuff;
    // VP input struct of flexiDag
    flexidag_memblk_t MemIORawInfo[MAX_STL_FX_IN_BUFFER_NUM];
    flexidag_memblk_t VPOutputBuf[STIXEL_FX_IDX_NUM][MAX_STL_FX_OUT_BUFFER_NUM];

    flexidag_memblk_t AutoDetWorkBuff;

    // Work buffer for VP
    flexidag_memblk_t ScaAlgoBuf[STIXEL_FX_IDX_NUM];
    flexidag_memblk_t VPStixelEstiRoadCfg;
    flexidag_memblk_t VPStixelHeightSegCfg;

} STIXEL_PRIVATE_s;

// For multiple stixel instance
static STIXEL_TASK_WORK_BUFF_s *gTaskHandle[STIXEL_INSTANCE_NUM];

#define DBG_STIXEL_DUMP_FLEXIDAG_OUTPUT (0U)

#define CTFW_TASK_NAME "Stixel"

#if defined(CONFIG_QNX)
#define SD_ROOT_PATH "/sd0"
#elif defined(CONFIG_LINUX)
#define SD_ROOT_PATH "/tmp/SD0"
#else
#define SD_ROOT_PATH "c:"
#endif

static UINT32 CftwStixel_QuerySize(UINT32 *pTaskStackSize, UINT32 *pAlgBuffMapSize, UINT32 *pAlgWorkSize, UINT32 *pAlgOutputSize)
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
        AlgBuffMapSize += CTFW_CV_MEM_ALIGN((UINT32)sizeof(STIXEL_PRIVATE_s));
    }

    // AlgWorkSize
    {
        {
            AlgWorkSize += CTFW_CV_MEM_ALIGN((UINT32)sizeof(STIXEL_TASK_WORK_BUFF_s));
        }
        {
            // Temp buffer + State buffer
            AlgWorkSize += CTFW_CV_MEM_ALIGN((UINT32)STIXEL_REQUIRED_CV_SIZE_VDISP);
            AlgWorkSize += CTFW_CV_MEM_ALIGN((UINT32)STIXEL_REQUIRED_CV_SIZE_GEN_ESTI_ROAD);
            AlgWorkSize += CTFW_CV_MEM_ALIGN((UINT32)STIXEL_REQUIRED_CV_SIZE_PREPROCESS);
            AlgWorkSize += CTFW_CV_MEM_ALIGN((UINT32)STIXEL_REQUIRED_CV_SIZE_FREE_SPACE);
            AlgWorkSize += CTFW_CV_MEM_ALIGN((UINT32)STIXEL_REQUIRED_CV_SIZE_HEIGHT_SEG);
        }
        {
            // MemIO strcut
            UINT32 Idx;
            for (Idx = 0U; Idx < MAX_STL_FX_IN_BUFFER_NUM; Idx++) {
                AlgWorkSize += CTFW_CV_MEM_ALIGN((UINT32)sizeof(memio_source_recv_raw_t));
            }
        }
        {
            // Input configuration
            AlgWorkSize += CTFW_CV_MEM_ALIGN((UINT32)sizeof(AMBA_CV_STIXEL_GEN_ESTI_ROAD_CFG_s));
            AlgWorkSize += CTFW_CV_MEM_ALIGN((UINT32)sizeof(AMBA_CV_STIXEL_HEIGHT_SEG_CFG_s));
        }
        {
            // CV Output buffer
            AlgWorkSize += CTFW_CV_MEM_ALIGN((UINT32)APP_STIXEL_TSK_CV_OUT_BUFF_SIZE);
        }
        {
            UINT32 CamPhAutoDetWorkSize = 0U;
            RetVal |= CamPhDet_GetAutoDetWorkSize(&CamPhAutoDetWorkSize);
            AlgWorkSize += CTFW_CV_MEM_ALIGN(CamPhAutoDetWorkSize);
        }
    }

    // AlgOutputSize
    {
        AlgOutputSize += CTFW_CV_MEM_ALIGN((UINT32)sizeof(CTFW_STIXEL_TASK_OUT_t));
    }

    *pAlgBuffMapSize = AlgBuffMapSize;
    *pAlgWorkSize = AlgWorkSize;
    *pAlgOutputSize = AlgOutputSize;

    return RetVal;
}

static UINT32 CftwStixel_SetName(char TaskName[])
{
    AmbaUtility_StringAppend(TaskName, MAX_CTFW_NAME_LEN, CTFW_TASK_NAME);
    return ARM_OK;
}

static UINT32 Stixel_AlgoOutCallback(UINT32 Inst, UINT32 Event, const SVC_CV_ALGO_OUTPUT_s *pEventData)
{
    UINT32 RetVal = ARM_OK;
    STIXEL_TASK_WORK_BUFF_s *pTaskWorkBuffPtr = gTaskHandle[Inst];
    const STIXEL_SYNC_VP_MSG_s *pUserDataPtr;
    STIXEL_SYNC_VP_MSG_s Msg;

    if (Event == CALLBACK_EVENT_OUTPUT) {
        if (NULL != pEventData->pUserData) {
            AmbaMisra_TypeCast(&pUserDataPtr, &pEventData->pUserData);

            if (pUserDataPtr->AlgoIndex < STIXEL_FX_IDX_NUM) {
                RetVal |= AmbaWrap_memcpy(&Msg, pEventData->pUserData, sizeof(Msg));
                RetVal |= ArmMsgQueue_Send(&pTaskWorkBuffPtr->FlexiSyncMsgQ, &Msg);
                if (RetVal != ARM_OK) {
                    ArmLog_ERR(CTFW_TASK_NAME, "## Inst(%d) ArmMsgQueue_Send fail", Inst, 0U);
                }
            } else {
                ArmLog_ERR(CTFW_TASK_NAME, "## Inst(%d) AlgoOutCallback: Invalid algo index(%d)", Inst, pUserDataPtr->AlgoIndex);
            }
        } else {
            ArmLog_ERR(CTFW_TASK_NAME, "## Inst(%d) AlgoOutCallback: Invalid user data", Inst, 0U);
        }
    }

    return RetVal;
}

static UINT32 Stixel_AlgoOutCallback0(UINT32 Event, const SVC_CV_ALGO_OUTPUT_s *pEventData)
{
    return Stixel_AlgoOutCallback(STIXEL_INSTANCE_IDX_0, Event, pEventData);
}

static UINT32 Stixel_AlgoOutCallback1(UINT32 Event, const SVC_CV_ALGO_OUTPUT_s *pEventData)
{
    return Stixel_AlgoOutCallback(STIXEL_INSTANCE_IDX_1, Event, pEventData);
}

static UINT32 Stixel_DeInitFlexiDagBin(UINT32 MemPoolId, UINT32 ScaHandleIdx, STIXEL_PRIVATE_s *pTaskPrivate)
{
    STIXEL_TASK_WORK_BUFF_s *pTaskWorkBuff;
    SVC_CV_ALGO_HANDLE_s *pScaHandle;
    SVC_CV_ALGO_DELETE_CFG_s DeleteCfg;
    UINT32 OutBuffIdx;
    const flexidag_memblk_t *pAlgoBlk;
    UINT32 RetVal = ARM_OK;

    AmbaMisra_TypeCast(&pTaskWorkBuff, &pTaskPrivate->TaskWorkBuff.pBuffer);
    pScaHandle = &pTaskWorkBuff->ScaHandles[ScaHandleIdx];

    // unuse config
    RetVal |= AmbaWrap_memset(&DeleteCfg, 0, sizeof(DeleteCfg));
    RetVal |= SvcCvAlgo_Delete(pScaHandle, &DeleteCfg);

    AmbaMisra_TypeCast(&pTaskWorkBuff, &pTaskPrivate->TaskWorkBuff.pBuffer);
    pAlgoBlk = &pTaskPrivate->ScaAlgoBuf[ScaHandleIdx];

    RetVal |= ArmMemPool_Free(MemPoolId, pAlgoBlk);
    for (OutBuffIdx = 0U ; OutBuffIdx < pTaskWorkBuff->VPOutputNum[ScaHandleIdx]; OutBuffIdx++) {
        RetVal |= ArmMemPool_Free(MemPoolId, &pTaskPrivate->VPOutputBuf[ScaHandleIdx][OutBuffIdx]);
    }

    return RetVal;
}

static UINT32 Stixel_InitFlexiDagBin(SVC_CV_ALGO_OBJ_s *pAlgoObj, UINT32 MemPoolId,
                                     flexidag_memblk_t *pFlexiBinBlk,
                                     const SVC_CV_ALGO_CALLBACK_f pCBFxn, UINT32 ScaHandleIdx, STIXEL_PRIVATE_s *pTaskPrivate)
{
    SVC_CV_ALGO_QUERY_CFG_s QCfg;
    SVC_CV_ALGO_CREATE_CFG_s CCfg;
    UINT32 OutBuffIdx;
    STIXEL_TASK_WORK_BUFF_s *pTaskWorkBuff;
    flexidag_memblk_t *pAlgoBlk;
    SVC_CV_ALGO_HANDLE_s *pScaHandle;
    UINT32 RetVal = ARM_OK;

    AmbaMisra_TypeCast(&pTaskWorkBuff, &pTaskPrivate->TaskWorkBuff.pBuffer);
    pAlgoBlk = &pTaskPrivate->ScaAlgoBuf[ScaHandleIdx];
    pScaHandle = &pTaskWorkBuff->ScaHandles[ScaHandleIdx];

    RetVal |= AmbaWrap_memset(pScaHandle, 0, sizeof(SVC_CV_ALGO_HANDLE_s));
    QCfg.pAlgoObj = pAlgoObj;
    RetVal |= SvcCvAlgo_Query(pScaHandle, &QCfg);

    if (RetVal == 0U) {
        ArmLog_DBG(CTFW_TASK_NAME, "## ScaQueryCfg.TotalReqBufSz=%d, RetVal=%d", QCfg.TotalReqBufSz, RetVal);
        ArmLog_STR(CTFW_TASK_NAME, "## call create sca total buf", NULL, NULL);
        RetVal |= Cftw_MemPool_Allocate(MemPoolId, QCfg.TotalReqBufSz, pAlgoBlk);
    }

    if (RetVal == 0U) {
        CCfg.NumFD = 1;
        CCfg.pAlgoBuf = pAlgoBlk;
        CCfg.pBin[0] = pFlexiBinBlk;
        RetVal = SvcCvAlgo_Create(pScaHandle, &CCfg);
        ArmLog_STR(CTFW_TASK_NAME, "## SvcCvAlgo_Create: name: ", pScaHandle->FDs[0U].FDName, NULL);
        if (RetVal == ARM_OK) {
            pTaskWorkBuff->VPOutputNum[ScaHandleIdx] = CCfg.OutputNum;
            for (OutBuffIdx = 0U ; OutBuffIdx < CCfg.OutputNum; OutBuffIdx++) {
                pTaskWorkBuff->VPOutputNonAlignedSz[ScaHandleIdx][OutBuffIdx] = CCfg.OutputSz[OutBuffIdx];
                RetVal |= Cftw_MemPool_Allocate(MemPoolId, CCfg.OutputSz[OutBuffIdx],
                                                &pTaskPrivate->VPOutputBuf[ScaHandleIdx][OutBuffIdx]);
            }
        } else {
            ArmLog_ERR(CTFW_TASK_NAME, "## SvcCvAlgo_Create Fail, RetVal=%d", RetVal, 0);
        }
    }

    if (RetVal == 0U) {
        SVC_CV_ALGO_REGCB_CFG_s CBCfg;
        CBCfg.Mode = 0U;
        CBCfg.Callback = pCBFxn;
        RetVal = SvcCvAlgo_RegCallback(pScaHandle, &CBCfg);
        if (RetVal == ARM_OK) {
            //
        } else {
            ArmLog_ERR(CTFW_TASK_NAME, "## SvcCvAlgo_RegCallback Fail, RetVal=%d", RetVal, 0);
        }
    }

    return RetVal;
}

static UINT32 CftwStixel_Init(const void *pAlgConfig, flexidag_memblk_t FlexiBinBlk[],
                              UINT32 MemPoolID, const flexidag_memblk_t* const pAlgBuffMap, CtfwTaskConfig_t *pTaskSetting)
{
    UINT32 RetVal = ARM_OK;
    STIXEL_PRIVATE_s *pPrivatePtr;
    STIXEL_TASK_WORK_BUFF_s *pTaskWorkBuffPtr = NULL;
    //Always Use 1st instance temporarily
    UINT32 InstIdx = 0U;
    static const SVC_CV_ALGO_CALLBACK_f StixelAlgoOutCbList[STIXEL_INSTANCE_NUM] = {
        Stixel_AlgoOutCallback0,
        Stixel_AlgoOutCallback1,
    };

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
            RetVal |= Cftw_MemPool_Allocate(MemPoolID, (UINT32)sizeof(STIXEL_TASK_WORK_BUFF_s), &pPrivatePtr->TaskWorkBuff);

            if (RetVal == ARM_OK) {
                AmbaMisra_TypeCast(&pTaskWorkBuffPtr, &pPrivatePtr->TaskWorkBuff.pBuffer);
                RetVal |= AmbaWrap_memset(pTaskWorkBuffPtr, 0, pPrivatePtr->TaskWorkBuff.buffer_size);
            }
        }

        if (RetVal == ARM_OK) {
            UINT32 ScaHandleIdx;

            ScaHandleIdx = STIXEL_FX_IDX_VDISP;
            RetVal |= Stixel_InitFlexiDagBin(&StixelVDispAlgoObj, MemPoolID,
                                             &FlexiBinBlk[ScaHandleIdx],
                                             StixelAlgoOutCbList[InstIdx], ScaHandleIdx, pPrivatePtr);

            ScaHandleIdx = STIXEL_FX_IDX_GEN_ESTI_ROAD;
            RetVal |= Stixel_InitFlexiDagBin(&StixelGenEstiRoadAlgoObj, MemPoolID,
                                             &FlexiBinBlk[ScaHandleIdx],
                                             StixelAlgoOutCbList[InstIdx], ScaHandleIdx, pPrivatePtr);

            ScaHandleIdx = STIXEL_FX_IDX_PREPROCESS;
            RetVal |= Stixel_InitFlexiDagBin(&StixelPreprocessAlgoObj, MemPoolID,
                                             &FlexiBinBlk[ScaHandleIdx],
                                             StixelAlgoOutCbList[InstIdx], ScaHandleIdx, pPrivatePtr);

            ScaHandleIdx = STIXEL_FX_IDX_FREE_SPACE;
            RetVal |= Stixel_InitFlexiDagBin(&StixelFreeSpaceAlgoObj, MemPoolID,
                                             &FlexiBinBlk[ScaHandleIdx],
                                             StixelAlgoOutCbList[InstIdx], ScaHandleIdx, pPrivatePtr);

            ScaHandleIdx = STIXEL_FX_IDX_HEIGHT_SEG;
            RetVal |= Stixel_InitFlexiDagBin(&StixelHeightSegAlgoObj, MemPoolID,
                                             &FlexiBinBlk[ScaHandleIdx],
                                             StixelAlgoOutCbList[InstIdx], ScaHandleIdx, pPrivatePtr);
        }

        if (RetVal == ARM_OK) {
            UINT32 CamPhAutoDetWorkSize = 0U;
            RetVal |= CamPhDet_GetAutoDetWorkSize(&CamPhAutoDetWorkSize);
            RetVal |= ArmMemPool_Allocate(MemPoolID, CamPhAutoDetWorkSize, &pPrivatePtr->AutoDetWorkBuff);
        }

        if (RetVal == ARM_OK) {
            RetVal |= Cftw_MemPool_Allocate(MemPoolID, (UINT32)sizeof(AMBA_CV_STIXEL_GEN_ESTI_ROAD_CFG_s), &pPrivatePtr->VPStixelEstiRoadCfg);
            RetVal |= Cftw_MemPool_Allocate(MemPoolID, (UINT32)sizeof(AMBA_CV_STIXEL_HEIGHT_SEG_CFG_s), &pPrivatePtr->VPStixelHeightSegCfg);
        }

        if (RetVal == ARM_OK) {
            UINT32 BuffIdx;
            UINT32 BuffNum;
            BuffNum = (UINT32)sizeof(pPrivatePtr->MemIORawInfo);
            BuffNum /= (UINT32)sizeof(pPrivatePtr->MemIORawInfo[0U]);
            for (BuffIdx = 0U; BuffIdx < BuffNum; BuffIdx++) {
                RetVal |= Cftw_MemPool_Allocate(MemPoolID, (UINT32)sizeof(memio_source_recv_raw_t), &pPrivatePtr->MemIORawInfo[BuffIdx]);
            }
        }
    }

    // Algo private init
    if (RetVal == ARM_OK) {
        {
            pTaskWorkBuffPtr->InstId = InstIdx;
            gTaskHandle[InstIdx] = pTaskWorkBuffPtr;
        }


        {
            CAM_PITCH_DET_MANUAL_HDLR_s *pManualDetHdlr = NULL;
            const CTFW_STIXEL_TASK_CONFIG_t *pTaskConfig = NULL;
            const AMBA_CV_STIXEL_CFG_V2_s *pStoreStixelConfig = NULL;
            const AMBA_CV_STIXEL_ROI_s *pRoiCfg;

            pManualDetHdlr = &pTaskWorkBuffPtr->ManualDetHdlr;
            AmbaMisra_TypeCast(&pTaskConfig, &pAlgConfig);
            RetVal |= Ctfw_CheckRetVal(__func__, "AmbaWrap_memcpy",
                                       AmbaWrap_memcpy(&pTaskWorkBuffPtr->TaskConfig, pTaskConfig, sizeof(CTFW_STIXEL_TASK_CONFIG_t)));

            pRoiCfg = &pTaskWorkBuffPtr->TaskConfig.StixelCfg.RoiCfg;
            if (pRoiCfg->Width != STIXEL_IN_DISPARITY_WIDTH) {
                ArmLog_ERR(CTFW_TASK_NAME, "## Configured ROI width(%d) != %d", pRoiCfg->Width, STIXEL_IN_DISPARITY_WIDTH);
                RetVal |= ARM_NG;
            }
            if (pRoiCfg->Height != STIXEL_IN_DISPARITY_HEIGHT) {
                ArmLog_ERR(CTFW_TASK_NAME, "## Configured ROI Height(%d) != %d", pRoiCfg->Height, STIXEL_IN_DISPARITY_HEIGHT);
                RetVal |= ARM_NG;
            }

            pStoreStixelConfig = &pTaskWorkBuffPtr->TaskConfig.StixelCfg;

            RetVal |= CamPhDet_InitManualDetection(pStoreStixelConfig->AlgCfg.ManualDetCfg.ExtParam.Height,
                                                   pStoreStixelConfig->AlgCfg.ManualDetCfg.ExtParam.Pitch,
                                                   pStoreStixelConfig->AlgCfg.IntParam.V0, pStoreStixelConfig->AlgCfg.IntParam.Baseline, pStoreStixelConfig->AlgCfg.IntParam.Fu,
                                                   (DOUBLE)STIXEL_OUT_VDISPARITY_PRECISION,
                                                   pStoreStixelConfig->AlgCfg.ManualDetCfg.PitchStride,
                                                   pStoreStixelConfig->AlgCfg.ManualDetCfg.HeightStride,
                                                   pStoreStixelConfig->AlgCfg.ManualDetCfg.PitchSetNum,
                                                   pStoreStixelConfig->AlgCfg.ManualDetCfg.HeightSetNum,
                                                   &(pTaskWorkBuffPtr->PriorEstimatedGround),
                                                   pManualDetHdlr);
        }

        // Setup msg queue for synchronizing each flexiDag
        {
            AmbaUtility_StringAppend(pTaskWorkBuffPtr->FlexiSyncMsgQName, MAX_CTFW_NAME_LEN, "StixelTaskFlexiSyncQ");
            RetVal |= ArmMsgQueue_Create(&pTaskWorkBuffPtr->FlexiSyncMsgQ, &(pTaskWorkBuffPtr->FlexiSyncMsgQName[0U]),
                                         (UINT32)sizeof(pTaskWorkBuffPtr->FlexiSyncMsgQBuff[0U]), STIXEL_FLEXIDAG_SYNC_MSG_BUFF_DEPTH,
                                         &(pTaskWorkBuffPtr->FlexiSyncMsgQBuff[0U]));
        }
    }

    return RetVal;
}

static UINT32 Stixel_AlgoWaitCallback(UINT32 Inst, UINT32 WaitAlgoIndex)
{
    UINT32 Rval = ARM_OK;
    STIXEL_SYNC_VP_MSG_s Msg;
    STIXEL_TASK_WORK_BUFF_s *pTaskWorkBuffPtr = gTaskHandle[Inst];

    Rval = ArmMsgQueue_Recv(&pTaskWorkBuffPtr->FlexiSyncMsgQ, &Msg);

    if (ARM_OK == Rval) {
        if (Msg.AlgoIndex != WaitAlgoIndex) {
            ArmLog_ERR(CTFW_TASK_NAME, "## Wait AlgoIndex(0x%x) msg failed. Recv AlgoIndex(%u)", Msg.AlgoIndex, WaitAlgoIndex);
            Rval = ARM_NG;
        }
    } else {
        ArmLog_ERR(CTFW_TASK_NAME, "## Wait AlgoIndex(0x%x) msg failed)", WaitAlgoIndex, 0U);
        Rval = ARM_NG;
    }

    return Rval;
}

static UINT32 Stixel_RunFlexiDagBlockWait(const STIXEL_PRIVATE_s *pPrivatePtr, UINT32 ScaAlgoIndex,
        UINT32 InBuffNum, flexidag_memblk_t InBuff[], const UINT32 InBuffSize[],
        const UINT32 InBuffPitch[])
{
    UINT32 RetVal = ARM_OK;
    SVC_CV_ALGO_FEED_CFG_s FeedCfg;
    UINT32 InputIdx;
    UINT32 OutputIdx;
    AMBA_CV_FLEXIDAG_IO_s InFxBuf;
    AMBA_CV_FLEXIDAG_IO_s OutFxBuf;

    SVC_CV_ALGO_HANDLE_s *pScaHandle;
    UINT32 OutBuffNum;
    const flexidag_memblk_t *pOutBuffBlk;

    STIXEL_TASK_WORK_BUFF_s *pTaskWorkBuffPtr;

    AmbaMisra_TypeCast(&pTaskWorkBuffPtr, &pPrivatePtr->TaskWorkBuff.pBuffer);

    pScaHandle = &pTaskWorkBuffPtr->ScaHandles[ScaAlgoIndex];
    OutBuffNum = pTaskWorkBuffPtr->VPOutputNum[ScaAlgoIndex];
    pOutBuffBlk = pPrivatePtr->VPOutputBuf[ScaAlgoIndex];

    InFxBuf.num_of_buf = InBuffNum;
    for (InputIdx = 0U; InputIdx < InBuffNum; InputIdx++) {
        memio_source_recv_raw_t *pMemIORawInfo;

        AmbaMisra_TypeCast(&pMemIORawInfo, &pPrivatePtr->MemIORawInfo[InputIdx].pBuffer);

        pMemIORawInfo->magic = 0U;
        pMemIORawInfo->addr = (UINT32)InBuff[InputIdx].buffer_caddr;
        pMemIORawInfo->size = InBuffSize[InputIdx];
        pMemIORawInfo->pitch = InBuffPitch[InputIdx];
        pMemIORawInfo->batch_cnt = 0U;
        pMemIORawInfo->rsv = 0U;

        InFxBuf.buf[InputIdx] = pPrivatePtr->MemIORawInfo[InputIdx];
        RetVal |= AmbaCV_UtilityCmaMemClean(&InFxBuf.buf[InputIdx]);
        RetVal |= AmbaCV_UtilityCmaMemClean(&InBuff[InputIdx]);
    }

    {
        // fill AMBA_CV_FLEXIDAG_IO_s: out structure
        OutFxBuf.num_of_buf = OutBuffNum;
        for (OutputIdx = 0U; OutputIdx < OutBuffNum; OutputIdx++) {
            OutFxBuf.buf[OutputIdx] = pOutBuffBlk[OutputIdx];
            RetVal |= AmbaCV_UtilityCmaMemInvalid(&OutFxBuf.buf[OutputIdx]);
        }
    }

    {
        STIXEL_SYNC_VP_MSG_s *pUserDataBuf = &pTaskWorkBuffPtr->VPRunUserData[ScaAlgoIndex];
        pUserDataBuf->AlgoIndex = ScaAlgoIndex;
        FeedCfg.pIn = &InFxBuf;
        FeedCfg.pOut = &OutFxBuf;
        AmbaMisra_TypeCast(&FeedCfg.pUserData, &pUserDataBuf);
        RetVal |= SvcCvAlgo_Feed(pScaHandle, &FeedCfg);
        if ( RetVal != ARM_OK ) {
            ArmLog_ERR(CTFW_TASK_NAME, "## (%d) SvcCvAlgo_Feed fail", __LINE__, 0U);
        }
    }

    // Note, FeedCfg can't be free before waiting for message
    if ( RetVal == ARM_OK ) {
        RetVal |= Stixel_AlgoWaitCallback(pTaskWorkBuffPtr->InstId, ScaAlgoIndex);
        if ( RetVal != ARM_OK ) {
            ArmLog_ERR(CTFW_TASK_NAME, "## (%d) Stixel_AlgoWaitCallback fail", __LINE__, 0U);
        }
    }

    return RetVal;
}

static UINT32 Stixel_ProcAndWaitFlexiDag(const STIXEL_PRIVATE_s *pPrivatePtr, UINT32 ScaAlgoIndex,
        UINT32 InBuffNum, flexidag_memblk_t InBuff[], const UINT32 InBuffSize[], const UINT32 InBuffPitch[],
        UINT32 SaveFile, const char* pSaveFilePrefix)
{
    UINT32 RetVal = 0U;
    UINT32 SaveIdx;
    const STIXEL_TASK_WORK_BUFF_s *pTaskWorkBuffPtr;
    UINT32 OutBuffNum;
    const UINT32 *pOutputSz;
    const flexidag_memblk_t *pOutBuffBlk;

    AmbaMisra_TypeCast(&pTaskWorkBuffPtr, &pPrivatePtr->TaskWorkBuff.pBuffer);

    OutBuffNum = pTaskWorkBuffPtr->VPOutputNum[ScaAlgoIndex];
    pOutputSz = pTaskWorkBuffPtr->VPOutputNonAlignedSz[ScaAlgoIndex];
    pOutBuffBlk = pPrivatePtr->VPOutputBuf[ScaAlgoIndex];

    RetVal |= Stixel_RunFlexiDagBlockWait(pPrivatePtr, ScaAlgoIndex, InBuffNum,
                                          InBuff, InBuffSize, InBuffPitch);

    if ( RetVal == 0U ) {
        if ( SaveFile != 0U ) {
            char AppendStr[64U];
            char SaveStr[128U];
            UINT8 *pU8Ptr;
            UINT32 WriteSize = 0U;
            UINT32 RetStrLen;

            RetVal |= Ctfw_CheckRetVal(__func__, "AmbaWrap_memset", AmbaWrap_memset(AppendStr, 0, sizeof(AppendStr)));
            RetVal |= Ctfw_CheckRetVal(__func__, "AmbaWrap_memset", AmbaWrap_memset(SaveStr, 0, sizeof(SaveStr)));

            for (SaveIdx = 0U; SaveIdx < InBuffNum; SaveIdx++) {

                // save in file, name  = pSaveFilePrefix + file index + "_in.raw"
                RetStrLen = AmbaUtility_UInt32ToStr(AppendStr, (UINT32)sizeof(AppendStr), SaveIdx, 10U);
                AmbaUtility_StringAppend(AppendStr, (UINT32)sizeof(AppendStr), "_in.raw");

                RetVal |= AmbaWrap_memset(SaveStr, 0, sizeof(SaveStr));
                AmbaUtility_StringAppend(SaveStr, (UINT32)sizeof(SaveStr), pSaveFilePrefix);
                AmbaUtility_StringAppend(SaveStr, (UINT32)sizeof(SaveStr), AppendStr);

                AmbaMisra_TypeCast(&pU8Ptr, &InBuff[SaveIdx].pBuffer);
                RetVal |= ArmFIO_Save(pU8Ptr, InBuffSize[SaveIdx], SaveStr, &WriteSize);
            }
            for (SaveIdx = 0U; SaveIdx < OutBuffNum; SaveIdx++) {

                // save out file, name  = pSaveFilePrefix + file index + "_out.raw"
                RetStrLen = AmbaUtility_UInt32ToStr(AppendStr, (UINT32)sizeof(AppendStr), SaveIdx, 10U);
                AmbaUtility_StringAppend(AppendStr, (UINT32)sizeof(AppendStr), "_out.raw");

                RetVal |= AmbaWrap_memset(SaveStr, 0, sizeof(SaveStr));
                AmbaUtility_StringAppend(SaveStr, (UINT32)sizeof(SaveStr), pSaveFilePrefix);
                AmbaUtility_StringAppend(SaveStr, (UINT32)sizeof(SaveStr), AppendStr);

                AmbaMisra_TypeCast(&pU8Ptr, &pOutBuffBlk[SaveIdx].pBuffer);
                RetVal |= ArmFIO_Save(pU8Ptr, pOutputSz[SaveIdx], SaveStr, &WriteSize);
            }

            (void) RetStrLen;
        }
    } else {
        ArmLog_ERR(CTFW_TASK_NAME, "Stixel_ProcAndWaitFlexiDag failed", __LINE__, 0U);
    }

    return RetVal;
}

static UINT32 Stixel_SpuOutToStixelDisparity(UINT32 InSpuScaleId,
        const AMBA_CV_SPU_BUF_s *pSpuOut,
        const AMBA_CV_STIXEL_CFG_V2_s *pInStixelCfg,
        AMBA_CV_STIXEL_DISPARITY_s *pOutDisparity,
        AMBA_CV_STIXEL_CFG_V2_s *pOutStixelCfg)
{
    UINT32 RetVal = ARM_OK;
    UINT32 ShiftSize;
    UINT32 SizeOfUInt16 = (UINT32)sizeof(UINT16);
    const AMBA_CV_SPU_DATA_s *pSpuData;
    char *pSpuScaleData;
    const AMBA_CV_STIXEL_ROI_s *pRoi;
    const AMBA_CV_SPU_SCALE_OUT_s *pScaleDisp;

    flexidag_memblk_t ScaleDispBlk;

    AmbaMisra_TypeCast(&pSpuData, &pSpuOut->pBuffer);

    pScaleDisp = &pSpuData->Scales[InSpuScaleId];

    // Extract disparity
    {
        AmbaMisra_TypeCast(&pSpuScaleData, &pSpuOut->pBuffer);
        pSpuScaleData = &pSpuScaleData[pScaleDisp->DisparityMapOffset];

        ScaleDispBlk.pBuffer = pSpuScaleData;
        ScaleDispBlk.buffer_daddr = pSpuOut->BufferDaddr + pScaleDisp->DisparityMapOffset;
        ScaleDispBlk.buffer_cacheable = pSpuOut->BufferCacheable;
        ScaleDispBlk.buffer_size = pScaleDisp->BufSize;
        ScaleDispBlk.buffer_caddr = pSpuOut->BufferCaddr + pScaleDisp->DisparityMapOffset;
        ScaleDispBlk.reserved_expansion = pSpuOut->Reserved;
#if 0
        {
            UINT32 WriteSize;
            static UINT32 FileIdx = 0U;
            char SaveName[128U];
            sprintf(SaveName, "/tmp/SD0/%010d.raw", FileIdx);
            RetVal |= ArmFIO_Save(ScaleDispBlk.pBuffer, ScaleDispBlk.buffer_size,
                                  SaveName, &WriteSize);
            FileIdx++;
        }
#endif
    }
    pRoi = &pInStixelCfg->RoiCfg;

    pOutDisparity->Width = pRoi->Width;
    pOutDisparity->Height = pRoi->Height;
    pOutDisparity->Pitch = pScaleDisp->DisparityPitch - (SizeOfUInt16*pRoi->StartX);

    ShiftSize = pRoi->StartY*pScaleDisp->DisparityPitch;
    ShiftSize += (pRoi->StartX*SizeOfUInt16);

    RetVal |= Ctfw_ShiftFlexiMemblkBuff(&ScaleDispBlk, ShiftSize, &pOutDisparity->Buff);
    if ( RetVal != ARM_OK ) {
        ArmLog_ERR(CTFW_TASK_NAME, "(%d) Error, Ctfw_ShiftFlexiMemblkBuff Failed.", __LINE__, 0U);
    }

    *pOutStixelCfg = *pInStixelCfg;
    pOutStixelCfg->AlgCfg.IntParam.U0 = pInStixelCfg->AlgCfg.IntParam.U0 - (DOUBLE)pRoi->StartX;
    pOutStixelCfg->AlgCfg.IntParam.V0 = pInStixelCfg->AlgCfg.IntParam.V0 - (DOUBLE)pRoi->StartY;

    return RetVal;
}

static UINT32 CftwStixel_EstimateRoad(UINT32 RoadEstiMode,
                                      const CAM_PITCH_DET_V_DISP_s *pVdisp,
                                      const void *pAutoDetWorkBuff,
                                      const CAM_PITCH_DET_AUTO_PARAM_s *pAutoDetCfg,
                                      const CAM_PITCH_DET_MANUAL_HDLR_s *pManualDetHdlr,
                                      const CAM_PITCH_DET_MANUAL_THR_s *pThr,
                                      CAM_PITCH_DET_MANUAL_PRIOR_ESTI_s *pPriorEsti,
                                      CAM_PITCH_DET_LINE_s *pEstiGCLine,
                                      DOUBLE *pEstiCamHeight,
                                      DOUBLE *pEstiCamPitch)
{
    UINT32 RetVal = 0U;

    if ( RoadEstiMode == STIXEL_ROAD_ESTI_MODE_AUTO ) {
        const UINT8 *pU8;
        AmbaMisra_TypeCast(&pU8, &pAutoDetWorkBuff);
        RetVal |= CamPhDet_AutoDetection(pU8,
                                         pVdisp,
                                         pAutoDetCfg,
                                         pEstiCamHeight, pEstiCamPitch, pEstiGCLine);
    } else {
        UINT32 QualifiedRoadEsti;
        RetVal |= CamPhDet_ManualDetection(pVdisp,
                                           pManualDetHdlr, pThr, pPriorEsti,
                                           pEstiCamHeight, pEstiCamPitch, pEstiGCLine, &QualifiedRoadEsti);
    }

    return RetVal;
}

static UINT32 CftwStixel_Process(const CtfwFrameMetadata_t *pMetadata, const flexidag_memblk_t* const pAlgInputData[],
                                 const flexidag_memblk_t* const pAlgBuffMap, flexidag_memblk_t* const pAlgOutputData)
{
    UINT32 RetVal = ARM_OK;

    UINT32 FxInBuffNum = 0U;
    flexidag_memblk_t FxInBuff[MAX_STL_FX_IN_BUFFER_NUM];
    UINT32 FxInBuffSize[MAX_STL_FX_IN_BUFFER_NUM] = {0U};
    UINT32 FxInBuffPitch[MAX_STL_FX_IN_BUFFER_NUM] = {0U};
    AMBA_CV_STIXEL_GEN_ESTI_ROAD_CFG_s *pVPStixelEstiRoadCfg;
    AMBA_CV_STIXEL_HEIGHT_SEG_CFG_s *pVPStixelHeightSegCfg;

    AMBA_CV_STIXEL_CFG_V2_s StixelCfg;
    AMBA_CV_STIXEL_DISPARITY_s Disparity;
    const char *pSaveFilePrefix;

    const AMBA_CV_STIXEL_CFG_V2_s *pRawStixelConfig;

    UINT32 VDispPrecision = STIXEL_OUT_VDISPARITY_PRECISION;
    UINT32 VDispStartY = STIXEL_IN_VDISPARITY_START_Y;
    UINT32 VDispHeight = STIXEL_IN_VDISPARITY_HEIGHT;
    UINT32 VDispOutWidth = STIXEL_OUT_VDISPARITY_WIDTH;

    UINT32 TransDispWidth;
    UINT32 TransDispHeight;

    const AMBA_CV_SPU_BUF_s *pIn;
    CTFW_STIXEL_TASK_OUT_t *pOut;

    const STIXEL_PRIVATE_s *pPrivatePtr;
    STIXEL_TASK_WORK_BUFF_s *pTaskWorkBuffPtr;

    CAM_PITCH_DET_CAM_INT_s IntrinsticParam;
    CAM_PITCH_DET_LINE_s EstiGCLine;
    DOUBLE EstiCamHeight = 0.0;
    DOUBLE EstiCamPitch = 0.0;

    UINT32 SizeOfUInt16 = (UINT32)sizeof(UINT16);

    (void) pMetadata;
    AmbaMisra_TouchUnused(pAlgOutputData);

    RetVal |= AmbaWrap_memset(FxInBuff, 0, sizeof(FxInBuff));
    RetVal |= AmbaWrap_memset(&IntrinsticParam, 0, sizeof(IntrinsticParam));
    RetVal |= AmbaWrap_memset(&EstiGCLine, 0, sizeof(EstiGCLine));

    // Orgnize input
    AmbaMisra_TypeCast(&pIn, &pAlgInputData[0U]);

    // Orgnize work buffer
    AmbaMisra_TypeCast(&pPrivatePtr, &pAlgBuffMap->pBuffer);
    AmbaMisra_TypeCast(&pTaskWorkBuffPtr, &pPrivatePtr->TaskWorkBuff.pBuffer);

    // Orgnize output
    AmbaMisra_TypeCast(&pOut, &pAlgOutputData->pBuffer);

    // Algo private routine
    // ####################################
    pRawStixelConfig = &pTaskWorkBuffPtr->TaskConfig.StixelCfg;
    RetVal |= Stixel_SpuOutToStixelDisparity(pTaskWorkBuffPtr->TaskConfig.InDispScaleId,
              pIn, pRawStixelConfig, &Disparity, &StixelCfg);

    TransDispWidth = Disparity.Height;
    TransDispHeight = Disparity.Width/STIXEL_COLUMN_WIDTH;

    AmbaMisra_TypeCast(&pVPStixelEstiRoadCfg, &pPrivatePtr->VPStixelEstiRoadCfg.pBuffer);
    AmbaMisra_TypeCast(&pVPStixelHeightSegCfg, &pPrivatePtr->VPStixelHeightSegCfg.pBuffer);

    IntrinsticParam.BaseLineMeter = StixelCfg.AlgCfg.IntParam.Baseline;
    IntrinsticParam.FocalLengthPixel = StixelCfg.AlgCfg.IntParam.Fu;
    IntrinsticParam.V0 = StixelCfg.AlgCfg.IntParam.V0;
    AmbaMisra_TouchUnused(&IntrinsticParam);

    // V-disparity
    // ----------------------------------------------------------
    {
        flexidag_memblk_t VerShiftDisparityBuff;
        UINT32 ShiftSize;

        RetVal |= AmbaWrap_memset(&VerShiftDisparityBuff, 0, sizeof(VerShiftDisparityBuff));

        // Vertical shift to the center of disparity
        ShiftSize = (Disparity.Width*VDispStartY)*SizeOfUInt16;
        RetVal |= Ctfw_ShiftFlexiMemblkBuff(&Disparity.Buff, ShiftSize, &VerShiftDisparityBuff);
        if ( RetVal != ARM_OK ) {
            ArmLog_ERR(CTFW_TASK_NAME, "(%d) Error, Ctfw_ShiftFlexiMemblkBuff Failed.", __LINE__, 0U);
        }

        // Flexidag In
        FxInBuffNum = 1U;
        FxInBuff[0U] = VerShiftDisparityBuff;
        FxInBuffSize[0U] = Disparity.Pitch*VDispHeight;
        FxInBuffPitch[0U] = Disparity.Pitch;

        pSaveFilePrefix = SD_ROOT_PATH "/STIXEL_UT_VDisparity";

        RetVal |= Stixel_ProcAndWaitFlexiDag(pPrivatePtr, STIXEL_FX_IDX_VDISP,
                                             FxInBuffNum, FxInBuff, FxInBuffSize, FxInBuffPitch,
                                             DBG_STIXEL_DUMP_FLEXIDAG_OUTPUT, pSaveFilePrefix);
    }

    // Calculate camera pitch angle
    // ----------------------------------------------------------
    if (RetVal == ARM_OK) {
        CAM_PITCH_DET_V_DISP_s Vdisp;
        const void *pAutoDetWorkPtr;
        const CAM_PITCH_DET_MANUAL_HDLR_s *pManualHdlrPtr;
        CAM_PITCH_DET_AUTO_PARAM_s AutoDetCfg;
        CAM_PITCH_DET_MANUAL_THR_s VdispThr;
        DOUBLE XPixelCountThrDB;

        Vdisp.Precision = VDispPrecision;
        Vdisp.StartY = VDispStartY;
        Vdisp.Width = VDispOutWidth;
        Vdisp.Height = VDispHeight;

        AmbaMisra_TypeCast(&Vdisp.pBuff, &pPrivatePtr->VPOutputBuf[STIXEL_FX_IDX_VDISP][0U].pBuffer);

        AmbaMisra_TypeCast(&pAutoDetWorkPtr, &pPrivatePtr->AutoDetWorkBuff.pBuffer);
        pManualHdlrPtr = &pTaskWorkBuffPtr->ManualDetHdlr;

        AutoDetCfg.IntCam = IntrinsticParam;

        {
            XPixelCountThrDB = (DOUBLE)Vdisp.Width;
            XPixelCountThrDB *= StixelCfg.AlgCfg.ManualDetCfg.VDispParam.XRatioThr;
        }
        VdispThr.XPixelCountThr = (UINT32)XPixelCountThrDB;
        VdispThr.YRatioThr = StixelCfg.AlgCfg.ManualDetCfg.VDispParam.YRatioThr;

        RetVal |= CftwStixel_EstimateRoad(StixelCfg.AlgCfg.RoadEstiMode,
                                          &Vdisp,
                                          pAutoDetWorkPtr,
                                          &AutoDetCfg,
                                          pManualHdlrPtr,
                                          &VdispThr,
                                          &pTaskWorkBuffPtr->PriorEstimatedGround,
                                          &EstiGCLine,
                                          &EstiCamHeight,
                                          &EstiCamPitch);
#if 0
        {
            char EstiCamHeightStrBuff[50U];
            char EstiCamPitchStrBuff[50U];
            (void) AmbaUtility_DoubleToStr(&EstiCamHeightStrBuff[0U],
                                           (UINT32)sizeof(EstiCamHeightStrBuff), EstiCamHeight, 5U);
            (void) AmbaUtility_DoubleToStr(&EstiCamPitchStrBuff[0U],
                                           (UINT32)sizeof(EstiCamPitchStrBuff), EstiCamPitch, 5U);

            ArmLog_STR(CTFW_TASK_NAME, "EstiCamHeight = %s, EstiCamPitch = %s\n",
                       EstiCamHeightStrBuff, EstiCamPitchStrBuff);
        }
#endif
    }

    // GenEstiRoad
    // ----------------------------------------------------------
    if (RetVal == ARM_OK) {
        DOUBLE EstiSinCamPitch = 0.0;
        DOUBLE EstiCosCamPitch = 0.0;
        UINT32 RoadSectionIdx;

        RetVal |= AmbaWrap_sin(EstiCamPitch, &EstiSinCamPitch);
        RetVal |= AmbaWrap_cos(EstiCamPitch, &EstiCosCamPitch);

        pVPStixelEstiRoadCfg->ImageV0 = (FLOAT)StixelCfg.AlgCfg.IntParam.V0;
        pVPStixelEstiRoadCfg->BaseLine = (FLOAT)StixelCfg.AlgCfg.IntParam.Baseline;
        pVPStixelEstiRoadCfg->FocalLength = (FLOAT)StixelCfg.AlgCfg.IntParam.Fu;
        pVPStixelEstiRoadCfg->DetObjectHeight = (FLOAT)StixelCfg.AlgCfg.DetObjectHeight;
        for (RoadSectionIdx = 0U; RoadSectionIdx < STIXEL_ROAD_SECTION_NUM; RoadSectionIdx++) {
            pVPStixelEstiRoadCfg->RoadSection[RoadSectionIdx].Bottom = (FLOAT)Disparity.Height - 1.f;
            pVPStixelEstiRoadCfg->RoadSection[RoadSectionIdx].M = (FLOAT)EstiGCLine.m;
            pVPStixelEstiRoadCfg->RoadSection[RoadSectionIdx].C = (FLOAT)EstiGCLine.c;
            pVPStixelEstiRoadCfg->RoadSection[RoadSectionIdx].SinCamPitch = (FLOAT)EstiSinCamPitch;
            pVPStixelEstiRoadCfg->RoadSection[RoadSectionIdx].CosCamPitch = (FLOAT)EstiCosCamPitch;
        }

        // Flexidag In
        FxInBuffNum = 1U;
        FxInBuff[0U] = pPrivatePtr->VPStixelEstiRoadCfg;
        FxInBuffSize[0U] = pPrivatePtr->VPStixelEstiRoadCfg.buffer_size;
        FxInBuffPitch[0U] = pPrivatePtr->VPStixelEstiRoadCfg.buffer_size;

        pSaveFilePrefix = SD_ROOT_PATH "/STIXEL_UT_GenEstiRoad";

        RetVal |= Stixel_ProcAndWaitFlexiDag(pPrivatePtr, STIXEL_FX_IDX_GEN_ESTI_ROAD,
                                             FxInBuffNum, FxInBuff, FxInBuffSize, FxInBuffPitch,
                                             DBG_STIXEL_DUMP_FLEXIDAG_OUTPUT, pSaveFilePrefix);
    }

    // Preprocess
    // ----------------------------------------------------------
    if (RetVal == ARM_OK) {
        // Flexidag In
        FxInBuffNum = 1U;
        FxInBuff[0U] = Disparity.Buff;
        FxInBuffSize[0U] = Disparity.Pitch*Disparity.Height;
        FxInBuffPitch[0U] = Disparity.Pitch;

        pSaveFilePrefix = SD_ROOT_PATH "/STIXEL_UT_Preprocess";

        RetVal |= Stixel_ProcAndWaitFlexiDag(pPrivatePtr, STIXEL_FX_IDX_PREPROCESS,
                                             FxInBuffNum, FxInBuff, FxInBuffSize, FxInBuffPitch,
                                             DBG_STIXEL_DUMP_FLEXIDAG_OUTPUT, pSaveFilePrefix);
    }

    // Free space
    // ----------------------------------------------------------
    if (RetVal == ARM_OK) {
        // Flexidag In
        FxInBuffNum = 4U;
        FxInBuff[0U] = pPrivatePtr->VPOutputBuf[STIXEL_FX_IDX_PREPROCESS][0U];
        FxInBuffSize[0U] = pPrivatePtr->VPOutputBuf[STIXEL_FX_IDX_PREPROCESS][0U].buffer_size;
        FxInBuffPitch[0U] = TransDispWidth*SizeOfUInt16;

        FxInBuff[1U] = pPrivatePtr->VPOutputBuf[STIXEL_FX_IDX_GEN_ESTI_ROAD][0U];
        FxInBuffSize[1U] = pPrivatePtr->VPOutputBuf[STIXEL_FX_IDX_GEN_ESTI_ROAD][0U].buffer_size;
        FxInBuffPitch[1U] = TransDispWidth*SizeOfUInt16;

        FxInBuff[2U] = pPrivatePtr->VPOutputBuf[STIXEL_FX_IDX_GEN_ESTI_ROAD][1U];
        FxInBuffSize[2U] = pPrivatePtr->VPOutputBuf[STIXEL_FX_IDX_GEN_ESTI_ROAD][1U].buffer_size;
        FxInBuffPitch[2U] = TransDispWidth*SizeOfUInt16;

        FxInBuff[3U] = pPrivatePtr->VPOutputBuf[STIXEL_FX_IDX_GEN_ESTI_ROAD][2U];
        FxInBuffSize[3U] = pPrivatePtr->VPOutputBuf[STIXEL_FX_IDX_GEN_ESTI_ROAD][2U].buffer_size;
        FxInBuffPitch[3U] = TransDispWidth*SizeOfUInt16;

        pSaveFilePrefix = SD_ROOT_PATH "/STIXEL_UT_Free_Space";

        RetVal |= Stixel_ProcAndWaitFlexiDag(pPrivatePtr, STIXEL_FX_IDX_FREE_SPACE,
                                             FxInBuffNum, FxInBuff, FxInBuffSize, FxInBuffPitch,
                                             DBG_STIXEL_DUMP_FLEXIDAG_OUTPUT, pSaveFilePrefix);
    }

    // Height segmentation
    // ----------------------------------------------------------
    if (RetVal == ARM_OK) {
        pVPStixelHeightSegCfg->MinDisparity = StixelCfg.AlgCfg.MinDisparity;

        // Flexidag In
        FxInBuffNum = 4U;
        FxInBuff[0U] = pPrivatePtr->VPOutputBuf[STIXEL_FX_IDX_PREPROCESS][0U];
        FxInBuffSize[0U] = pPrivatePtr->VPOutputBuf[STIXEL_FX_IDX_PREPROCESS][0U].buffer_size;
        FxInBuffPitch[0U] = TransDispWidth*SizeOfUInt16;

        FxInBuff[1U] = pPrivatePtr->VPOutputBuf[STIXEL_FX_IDX_FREE_SPACE][0U];
        FxInBuffSize[1U] = pPrivatePtr->VPOutputBuf[STIXEL_FX_IDX_FREE_SPACE][0U].buffer_size;
        FxInBuffPitch[1U] = TransDispHeight*SizeOfUInt16;

        FxInBuff[2U] = pPrivatePtr->VPOutputBuf[STIXEL_FX_IDX_GEN_ESTI_ROAD][3U];
        FxInBuffSize[2U] = pPrivatePtr->VPOutputBuf[STIXEL_FX_IDX_GEN_ESTI_ROAD][3U].buffer_size;
        FxInBuffPitch[2U] = STIXEL_IN_MAX_DISPARITY*SizeOfUInt16;

        FxInBuff[3U] = pPrivatePtr->VPStixelHeightSegCfg;
        FxInBuffSize[3U] = pPrivatePtr->VPStixelHeightSegCfg.buffer_size;
        FxInBuffPitch[3U] = pPrivatePtr->VPStixelHeightSegCfg.buffer_size;

        pSaveFilePrefix = SD_ROOT_PATH "/STIXEL_UT_Height_Seg";

        RetVal |= Stixel_ProcAndWaitFlexiDag(pPrivatePtr, STIXEL_FX_IDX_HEIGHT_SEG,
                                             FxInBuffNum, FxInBuff, FxInBuffSize, FxInBuffPitch,
                                             DBG_STIXEL_DUMP_FLEXIDAG_OUTPUT, pSaveFilePrefix);
    }

    // To stixel
    // --------------------------------------------------
    if (RetVal == ARM_OK) {
        const UINT16 *pStixelBottomPos;
        const UINT16 *pStixelConfidence;
        const UINT16 *pStixelTopPos;
        const UINT16 *pStixelAvgDisparity;
        UINT32 StixelIdx;

        AmbaMisra_TypeCast(&pStixelBottomPos, &pPrivatePtr->VPOutputBuf[STIXEL_FX_IDX_FREE_SPACE][0U].pBuffer);
        AmbaMisra_TypeCast(&pStixelConfidence, &pPrivatePtr->VPOutputBuf[STIXEL_FX_IDX_FREE_SPACE][1U].pBuffer);
        AmbaMisra_TypeCast(&pStixelTopPos, &pPrivatePtr->VPOutputBuf[STIXEL_FX_IDX_HEIGHT_SEG][0U].pBuffer);
        AmbaMisra_TypeCast(&pStixelAvgDisparity, &pPrivatePtr->VPOutputBuf[STIXEL_FX_IDX_HEIGHT_SEG][1U].pBuffer);

        pOut->StixelInfo.StixelNum = STIXEL_OUT_STIXEL_NUM;
        pOut->StixelInfo.Width = STIXEL_COLUMN_WIDTH;
        for (StixelIdx = 0U; StixelIdx < pOut->StixelInfo.StixelNum; StixelIdx++) {
            AMBA_CV_STIXEL_s *pStixel = &pOut->StixelInfo.StixelList[StixelIdx];
            pStixel->Top = pStixelTopPos[StixelIdx];
            pStixel->Bottom = pStixelBottomPos[StixelIdx];
            pStixel->AvgDisparity = pStixelAvgDisparity[StixelIdx];
            pStixel->Confidence = pStixelConfidence[StixelIdx];
        }
    }

    return RetVal;
}

static UINT32 CftwStixel_DeInit(UINT32 MemPoolID, const flexidag_memblk_t* const pAlgBuffMap)
{
    UINT32 RetVal = ARM_OK;
    STIXEL_PRIVATE_s *pPrivatePtr;
    STIXEL_TASK_WORK_BUFF_s *pTaskWorkBuffPtr = NULL;

    AmbaMisra_TypeCast(&pPrivatePtr, &pAlgBuffMap->pBuffer);
    AmbaMisra_TypeCast(&pTaskWorkBuffPtr, &pPrivatePtr->TaskWorkBuff.pBuffer);

    RetVal |= ArmMsgQueue_Delete(&pTaskWorkBuffPtr->FlexiSyncMsgQ, &(pTaskWorkBuffPtr->FlexiSyncMsgQName[0U]));

    {
        UINT32 BuffIdx;
        UINT32 BuffNum;
        BuffNum = (UINT32)sizeof(pPrivatePtr->MemIORawInfo);
        BuffNum /= (UINT32)sizeof(pPrivatePtr->MemIORawInfo[0U]);
        for (BuffIdx = 0U; BuffIdx < BuffNum; BuffIdx++) {
            RetVal |= ArmMemPool_Free(MemPoolID, &pPrivatePtr->MemIORawInfo[BuffIdx]);
        }
    }

    RetVal |= ArmMemPool_Free(MemPoolID, &pPrivatePtr->VPStixelEstiRoadCfg);
    RetVal |= ArmMemPool_Free(MemPoolID, &pPrivatePtr->VPStixelHeightSegCfg);

    RetVal |= ArmMemPool_Free(MemPoolID, &pPrivatePtr->AutoDetWorkBuff);

    {
        UINT32 ScaHandleIdx;

        ScaHandleIdx = STIXEL_FX_IDX_VDISP;
        RetVal |= Stixel_DeInitFlexiDagBin(MemPoolID, ScaHandleIdx, pPrivatePtr);

        ScaHandleIdx = STIXEL_FX_IDX_GEN_ESTI_ROAD;
        RetVal |= Stixel_DeInitFlexiDagBin(MemPoolID, ScaHandleIdx, pPrivatePtr);

        ScaHandleIdx = STIXEL_FX_IDX_PREPROCESS;
        RetVal |= Stixel_DeInitFlexiDagBin(MemPoolID, ScaHandleIdx, pPrivatePtr);

        ScaHandleIdx = STIXEL_FX_IDX_FREE_SPACE;
        RetVal |= Stixel_DeInitFlexiDagBin(MemPoolID, ScaHandleIdx, pPrivatePtr);

        ScaHandleIdx = STIXEL_FX_IDX_HEIGHT_SEG;
        RetVal |= Stixel_DeInitFlexiDagBin(MemPoolID, ScaHandleIdx, pPrivatePtr);
    }

    RetVal |= ArmMemPool_Free(MemPoolID, &pPrivatePtr->TaskWorkBuff);

    return RetVal;
}

CtfwTaskObj_t gCtfwStixelTaskObj = {
    .pSetName = CftwStixel_SetName,
    .pQuerySize = CftwStixel_QuerySize,
    .pInit = CftwStixel_Init,
    .pProcess = CftwStixel_Process,
    .pDeInit = CftwStixel_DeInit,
};

