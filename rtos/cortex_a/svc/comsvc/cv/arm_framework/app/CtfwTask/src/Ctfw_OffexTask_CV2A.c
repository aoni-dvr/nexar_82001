#include "Ctfw_OffexTask_CV2A.h"
#include "cvapi_svccvalgo_ambaoffex.h"

#define CTFW_TASK_NAME "OffexCv2a"

extern SVC_CV_ALGO_OBJ_s AmbaOfFexAlgoObj;

typedef struct {
    SVC_CV_ALGO_HANDLE_s ScaHandle;
    UINT32 spuOutSz;
    UINT32 fexOutSz;
} OFFEXCV2A_TASK_WORK_BUFF_s;

typedef struct {
    flexidag_memblk_t taskWorkBuf;
} OFFEXCV2A_PRIVATE_s;

static UINT32 CtfwOffexCv2a_SetName(char TaskName[])
{
    (void) AmbaUtility_StringAppend(TaskName, MAX_CTFW_NAME_LEN, CTFW_TASK_NAME);
    return ARM_OK;
}

static UINT32 CtfwOffexCv2a_QuerySize(UINT32 *pTaskStackSize, UINT32 *pAlgBuffMapSize, UINT32 *pAlgWorkSize, UINT32 *pAlgOutputSize)
{
    UINT32 RetVal = ARM_OK;
    UINT32 AlgBuffMapSize = 0U;
    UINT32 AlgWorkSize = 0U;
    UINT32 AlgOutputSize = 0U;
    UINT32 tmpSzUI32 = 0U;
    UINT64 tmpSzUI64 = 0UL;

    /* AlgBuffMapSize */
    tmpSzUI64 = sizeof(OFFEXCV2A_PRIVATE_s);
    AmbaMisra_TypeCast32((void *)&tmpSzUI32, (void *)&tmpSzUI64);
    AlgBuffMapSize += CTFW_CV_MEM_ALIGN(tmpSzUI32);
    tmpSzUI32 = 0U;

    /* AlgWorkSize */
    tmpSzUI64 = sizeof(OFFEXCV2A_TASK_WORK_BUFF_s);
    AmbaMisra_TypeCast32((void *)&tmpSzUI32, (void *)&tmpSzUI64);
    AlgWorkSize += CTFW_CV_MEM_ALIGN(tmpSzUI32);
    tmpSzUI32 = 0U;
    tmpSzUI64 = SCA_REQ_BUF_SZ;
    AmbaMisra_TypeCast32((void *)&tmpSzUI32, (void *)&tmpSzUI64);
    AlgWorkSize += CTFW_CV_MEM_ALIGN(tmpSzUI32);
    tmpSzUI32 = 0U;

    /* AlgOutputSize */
    tmpSzUI64 = SCA_OUTPUT_BUF_SZ;
    AmbaMisra_TypeCast32((void *)&tmpSzUI32, (void *)&tmpSzUI64);
    AlgOutputSize += CTFW_CV_MEM_ALIGN(tmpSzUI32);
    tmpSzUI32 = 0U;

    *pTaskStackSize = 16384U;
    *pAlgBuffMapSize = AlgBuffMapSize;
    *pAlgWorkSize = AlgWorkSize;
    *pAlgOutputSize = AlgOutputSize;

    return RetVal;
}
#if 0
static UINT32 OffexCv2a_AlgoOutCallbck(UINT32 Event, const SVC_CV_ALGO_OUTPUT_s *pEventData)
{
    UINT32 Rval = ARM_OK;

    (void) Event;
    (void) pEventData;

    return Rval;
}
#endif
static void config_ambaOffex_(const AMBA_CV_SPUFEX_CFG_s* pSpuFexCfg, const CTFW_OFFEXCV2_TASK_CONFIG_t* pTaskCfg)
{
    // FIXME:

    (void ) pSpuFexCfg;
    (void ) pTaskCfg;
}

static UINT32 CtfwOffexCv2a_Init(const void* pAlgConfig, flexidag_memblk_t FlexiBinBlk[],
                                 UINT32 MemPoolID, const flexidag_memblk_t* const pAlgBuffMap, CtfwTaskConfig_t* pTaskSetting)
{
    UINT32 Rval = ARM_OK;
    OFFEXCV2A_PRIVATE_s *pPrivatePtr = NULL;
    OFFEXCV2A_TASK_WORK_BUFF_s *pTaskWorkPtr = NULL;
    SVC_CV_ALGO_HANDLE_s *pScaHandle = NULL;
    SVC_CV_ALGO_QUERY_CFG_s ScaQueryCfg;
    flexidag_memblk_t algBlkBuf;
    UINT32 tmpSzUI32 = 0U;
    UINT64 tmpSzUI64 = 0UL;

    /* Task env setting */
    pTaskSetting->ProcInterval = 1U;
    pTaskSetting->TaskPriority = 50U;
    pTaskSetting->TaskCoreSel = 0U;

    /* allocate task work buffer from memory pool */
    Rval = AmbaWrap_memcpy((void *)&pPrivatePtr, (const void *)&pAlgBuffMap->pBuffer, sizeof(void *));
    if(Rval != ARM_OK) {
        ArmLog_ERR(CTFW_TASK_NAME, "## CtfwOffexCv2a_Init(), pAlgBuffMap memcpy failed: %u", Rval, 0U);
    }
    tmpSzUI64 = sizeof(OFFEXCV2A_TASK_WORK_BUFF_s);
    AmbaMisra_TypeCast32((void *)&tmpSzUI32, (void *)&tmpSzUI64);
    Rval |= ArmMemPool_Allocate(MemPoolID, tmpSzUI32, &pPrivatePtr->taskWorkBuf);
    // ArmLog_DBG(CTFW_TASK_NAME, "## alloc work buffer pPrivatePtr: 0x%p, Rval = %u", pPrivatePtr, Rval);

    /* query from sca object */
    if (Rval == ARM_OK) {
        (void) AmbaWrap_memcpy((void *)&pTaskWorkPtr, (void *)&pPrivatePtr->taskWorkBuf.pBuffer, sizeof(void* ));
        pScaHandle = &pTaskWorkPtr->ScaHandle;
        ScaQueryCfg.pAlgoObj = &AmbaOfFexAlgoObj;
        Rval |= AmbaOfFexAlgoObj.Query(pScaHandle, &ScaQueryCfg);
        // ArmLog_DBG(CTFW_TASK_NAME, "## ScaQueryCfg.TotalReqBufSz=%d, Rval=%d", ScaQueryCfg.TotalReqBufSz, Rval);
    }

    /* allocate algo blk buffer from memory pool */
    if (Rval == ARM_OK) {
        Rval |= ArmMemPool_Allocate(MemPoolID, ScaQueryCfg.TotalReqBufSz, &algBlkBuf);
        // ArmLog_DBG(CTFW_TASK_NAME, "## alloc algo memblk size = %u, Rval = %u", algBlkBuf.buffer_size, Rval);
    }
    if (Rval == ARM_OK) {
        SVC_CV_ALGO_CREATE_CFG_s ScaCreatCfg;

        (void) AmbaWrap_memset(&ScaCreatCfg, 0, sizeof(SVC_CV_ALGO_CREATE_CFG_s));
        ScaCreatCfg.NumFD = 1U;
        ScaCreatCfg.pAlgoBuf = &algBlkBuf;
        ScaCreatCfg.pBin[0] = &FlexiBinBlk[0];
        pScaHandle->pAlgoCtrl = NULL;
        Rval |= AmbaOfFexAlgoObj.Create(pScaHandle, &ScaCreatCfg);
        // ArmLog_DBG(CTFW_TASK_NAME, "## sca create, Rval = %u", Rval, 0U);

        /* allocate output buf and check size is large enough */
        if(Rval == ARM_OK) {
            UINT32 Idx = 0;
            UINT32 totalSz = 0;
            UINT32 OutputSz[8] = {0};

            for (Idx = 0U; Idx < ScaCreatCfg.OutputNum; Idx ++) {
                OutputSz[Idx] = CTFW_CV_MEM_ALIGN(ScaCreatCfg.OutputSz[Idx]);
                totalSz += OutputSz[Idx];
            }

            /* output buffer size */
            pTaskWorkPtr->spuOutSz = OutputSz[0];
            pTaskWorkPtr->fexOutSz = OutputSz[1];

            /* check size */
            tmpSzUI32 = 0U;
            tmpSzUI64 = SCA_OUTPUT_BUF_SZ;
            AmbaMisra_TypeCast32((void *)&tmpSzUI32, (void *)&tmpSzUI64);
            if(totalSz > tmpSzUI32) {
                ArmLog_ERR(CTFW_TASK_NAME, "## flexidag return output sz is larger than allocated, totalSz: %u, allocateSz: %llu",
                           totalSz, tmpSzUI32);
                Rval = ARM_NG;
            }
        }
    }

    /* sca configure */
    if (Rval == ARM_OK) {
        SVC_CV_ALGO_CTRL_CFG_s ScaCtrlCfg;
        CTFW_OFFEXCV2_TASK_CONFIG_t *pTaskCfg;
        AMBA_CV_SPUFEX_CFG_s spuFexCfg;

        (void) AmbaWrap_memcpy((void *)&pTaskCfg, (void *)&pAlgConfig, sizeof(void *));
        config_ambaOffex_(&spuFexCfg, pTaskCfg);
        // FIXME: can using other configurations
        // ScaCtrlCfg.CtrlType = AMBA_OFFEX_CONTROL_TYPE_CTFW;
        // ScaCtrlCfg.pCtrlParam = &spuFexCfg;
        // Rval |= AmbaOfFexAlgoObj.Control(pScaHandle, &ScaCtrlCfg);
        (void) &ScaCtrlCfg;
        // ArmLog_DBG(CTFW_TASK_NAME, "## sca control is hacked, Rval = %u", Rval, 0U);
    }

    /* for debug */
#if 0
    if (Rval == 0U) {
        SVC_CV_ALGO_REGCB_CFG_s ScaCbCfg;
        ScaCbCfg.Mode = 0U;
        ScaCbCfg.Callback = OffexCv2a_AlgoOutCallbck;
        Rval = SvcCvAlgo_RegCallback(pScaHandle, &ScaCbCfg);
        ArmLog_DBG(CTFW_TASK_NAME, "## sca register callback %u", Rval, 0U);
    }
#endif

    return Rval;
}

static UINT32 CtfwOffexCv2a_Process(const CtfwFrameMetadata_t* pMetadata, const flexidag_memblk_t* const pAlgInputData[],
                                    const flexidag_memblk_t* const pAlgBuffMap, flexidag_memblk_t *pAlgOutputData)
{
    UINT32 RetVal = ARM_OK;
    OFFEXCV2A_PRIVATE_s *pPrivatePtr = NULL;
    OFFEXCV2A_TASK_WORK_BUFF_s *pTaskWorkPtr = NULL;
    SVC_CV_ALGO_HANDLE_s *pScaHandle = NULL;
    SVC_CV_ALGO_FEED_CFG_s ScaFeedCfg;
    AMBA_CV_FLEXIDAG_IO_s InBuf;
    AMBA_CV_FLEXIDAG_IO_s OutBuf;
    flexidag_memblk_t algOut[2];
    ULONG paddru64 = 0UL;

    (void) pMetadata;
    AmbaMisra_TouchUnused(pAlgOutputData);

    // ArmLog_DBG(CTFW_TASK_NAME, "## process start", 0U, 0U);

    /* arrange handle */
    RetVal |= AmbaWrap_memcpy((void *)&pPrivatePtr, (const void *)&pAlgBuffMap->pBuffer, sizeof(void*));
    if(RetVal != ARM_OK) {
        ArmLog_ERR(CTFW_TASK_NAME, "## fCtfwOffexCv2a_Process(), pAlgBuffMap memcpy failed: %u", RetVal, 0U);
    }
    AmbaMisra_TypeCast((void *)&pTaskWorkPtr, (void *)&pPrivatePtr->taskWorkBuf.pBuffer);
    pScaHandle = &pTaskWorkPtr->ScaHandle;

    /* arrange input and output bufs */
    ScaFeedCfg.pIn = &InBuf;
    ScaFeedCfg.pIn->num_of_buf = 1U;
    RetVal = AmbaWrap_memcpy((void *)&ScaFeedCfg.pIn->buf[0U], (const void *)pAlgInputData[0U], sizeof(flexidag_memblk_t));
    if(RetVal != ARM_OK) {
        ArmLog_ERR(CTFW_TASK_NAME, "## fCtfwOffexCv2a_Process(), pAlgInputData memcpy failed: %u", RetVal, 0U);
    }

    ScaFeedCfg.pOut = &OutBuf;
    ScaFeedCfg.pOut->num_of_buf = 2U;
    /* prepare the first output buffer */
    (void) AmbaWrap_memcpy((void *)&algOut[0], (void *)pAlgOutputData, sizeof(flexidag_memblk_t));
    algOut[0].buffer_size = pTaskWorkPtr->spuOutSz;
    (void) AmbaWrap_memcpy((void *)&ScaFeedCfg.pOut->buf[0U], (void *)&algOut[0], sizeof(flexidag_memblk_t));
    /* prepare the second output buffer */
    (void) AmbaWrap_memcpy((void *)&paddru64, (void *)&algOut[0].pBuffer, sizeof(void *));
    paddru64 += (ULONG)pTaskWorkPtr->spuOutSz;
    (void) AmbaWrap_memcpy((void *)&algOut[1].pBuffer, (void *)&paddru64, sizeof(void *));
    // algOut[1].pBuffer = (char *)((ULONG)algOut[0].pBuffer + (ULONG)pTaskWorkPtr->spuOutSz);
    algOut[1].buffer_daddr = algOut[0].buffer_daddr + (ULONG)pTaskWorkPtr->spuOutSz;
    algOut[1].buffer_size = pTaskWorkPtr->fexOutSz;
    algOut[1].buffer_cacheable = algOut[0].buffer_cacheable;
    (void) AmbaWrap_memcpy((void *)&ScaFeedCfg.pOut->buf[1U], (void *)&algOut[1], sizeof(flexidag_memblk_t));

    /* Run */
    RetVal |= pScaHandle->pAlgoObj->Feed(pScaHandle, &ScaFeedCfg);

    return RetVal;
}

static UINT32 CtfwOffexCv2a_DeInit(UINT32 MemPoolID, const flexidag_memblk_t* const pAlgBuffMap)
{
    UINT32 RetVal = ARM_OK;
    OFFEXCV2A_PRIVATE_s *pPrivatePtr = NULL;
    OFFEXCV2A_TASK_WORK_BUFF_s *pTaskWorkPtr = NULL;
    SVC_CV_ALGO_HANDLE_s *pScaHandle = NULL;

    (void) MemPoolID;
    RetVal = AmbaWrap_memcpy((void *)&pPrivatePtr, (const void *)&pAlgBuffMap->pBufferm, sizeof(void*));
    if(RetVal != ARM_OK) {
        ArmLog_ERR(CTFW_TASK_NAME, "## CtfwOffexCv2a_DeInit(), pAlgBuffMap memcpy failed: %u", RetVal, 0U);
    }
    AmbaMisra_TypeCast((void *)&pTaskWorkPtr, (void *)&pPrivatePtr->taskWorkBuf.pBuffer);
    pScaHandle = &pTaskWorkPtr->ScaHandle;

    RetVal |= pScaHandle->pAlgoObj->Delete(pScaHandle, NULL);

    return RetVal;
}

CtfwTaskObj_t gCtfwOffexCv2akObj = {
    .pSetName = CtfwOffexCv2a_SetName,
    .pQuerySize = CtfwOffexCv2a_QuerySize,
    .pInit = CtfwOffexCv2a_Init,
    .pProcess = CtfwOffexCv2a_Process,
    .pDeInit = CtfwOffexCv2a_DeInit,
};
