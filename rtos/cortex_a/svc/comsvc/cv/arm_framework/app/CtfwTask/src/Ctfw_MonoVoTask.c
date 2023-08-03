#include "Ctfw_MonoVoTask.h"
#include "Ctfw_OffexTask_CV2A.h"

#define CTFW_TASK_NAME "findPose"

typedef struct {
    AMBA_MONOVO_CFG_s monoVoCfg;
} MONOVO_TASK_WORK_BUFF_s;

typedef struct {
    flexidag_memblk_t tskHdlrBuf;
    flexidag_memblk_t tskWorkBuf;
} MONOVO_PRIVATE_s;

static UINT32 CtfwMonoVo_SetName(char TaskName[])
{
    (void) AmbaUtility_StringAppend(TaskName, MAX_CTFW_NAME_LEN, CTFW_TASK_NAME);
    return ARM_OK;
}

static UINT32 CtfwMonoVo_QuerySize(UINT32 *pTaskStackSize, UINT32 *pAlgBuffMapSize, UINT32 *pAlgWorkSize, UINT32 *pAlgOutputSize)
{
    UINT32 RetVal = ARM_OK;
    UINT32 taskHdlrSz = 0U;
    UINT32 AlgBuffMapSize = 0U;
    UINT32 AlgWorkSize = 0U;
    UINT32 AlgOutputSize = 0U;
    UINT32 tmpSzUI32 = 0U;
    UINT64 tmpSzUI64 = 0UL;

    *pAlgBuffMapSize = 0U;
    *pAlgWorkSize = 0U;
    *pAlgOutputSize = 0U;

    /* AlgBuffMapSize */
    tmpSzUI64 = sizeof(MONOVO_PRIVATE_s);
    AmbaMisra_TypeCast32((void *)&tmpSzUI32, (void *)&tmpSzUI64);
    AlgBuffMapSize += CTFW_CV_MEM_ALIGN(tmpSzUI32);
    tmpSzUI32 = 0U;

    /* AlgWorkSize */
    (void) AmbaMonoVo_GetHdlrSize(&taskHdlrSz);
    AlgWorkSize += CTFW_CV_MEM_ALIGN(taskHdlrSz);
    tmpSzUI64 = sizeof(MONOVO_TASK_WORK_BUFF_s);
    AmbaMisra_TypeCast32((void *)&tmpSzUI32, (void *)&tmpSzUI64);
    AlgWorkSize += CTFW_CV_MEM_ALIGN(tmpSzUI32);
    tmpSzUI32 = 0U;

    /* AlgOutputSize */
    tmpSzUI64 = sizeof(AMBA_MONOVO_RESULT_s);
    AmbaMisra_TypeCast32((void *)&tmpSzUI32, (void *)&tmpSzUI64);
    AlgOutputSize += CTFW_CV_MEM_ALIGN(tmpSzUI32);
    tmpSzUI32 = 0U;

    *pTaskStackSize = 16384U;
    *pAlgBuffMapSize = AlgBuffMapSize;
    *pAlgWorkSize = AlgWorkSize;
    *pAlgOutputSize = AlgOutputSize;

    return RetVal;
}

static UINT32 CtfwMonoVo_Init(const void *pAlgConfig, flexidag_memblk_t FlexiBinBlk[],
                              UINT32 MemPoolID, const flexidag_memblk_t* const pAlgBuffMap, CtfwTaskConfig_t *pTaskSetting)
{
    UINT32 Rval = ARM_OK;
    MONOVO_PRIVATE_s *pPrivatePtr = NULL;
    void *pMonoVoHdlr = NULL;

    AmbaMisra_TouchUnused(FlexiBinBlk);

    /* Task env setting */
    pTaskSetting->ProcInterval = 1U;
    pTaskSetting->TaskPriority = 50U;
    pTaskSetting->TaskCoreSel = 0U;

    /* allocate buffer from memory pool */
    Rval |= AmbaWrap_memcpy((void *)&pPrivatePtr, (const void *)&pAlgBuffMap->pBuffer, sizeof(void *));

    /* algo handler init */
    if (Rval == ARM_OK) {
        UINT32 hdlrSz = 0U;

        (void) AmbaMonoVo_GetHdlrSize(&hdlrSz);
        Rval |= ArmMemPool_Allocate(MemPoolID, hdlrSz, &pPrivatePtr->tskHdlrBuf);
        // ArmLog_DBG(CTFW_TASK_NAME, "## alloc handler buffer %u", Rval, 0U);
    }
    if (Rval == ARM_OK) {
        AmbaMisra_TypeCast(&pMonoVoHdlr, (void *)&pPrivatePtr->tskHdlrBuf.pBuffer);
        Rval |= AmbaMonoVo_Init(pMonoVoHdlr);
        // ArmLog_DBG(CTFW_TASK_NAME, "## init handler rval: %u, pMonoVoHdlrL 0x%p", Rval, pMonoVoHdlr);
    }

    /* algo config */
    if (Rval == ARM_OK) {
        UINT64 tmpSzUI64 = sizeof(MONOVO_TASK_WORK_BUFF_s);
        UINT32 tmpSzUI32 = 0U;

        AmbaMisra_TypeCast32((void *)&tmpSzUI32, (void *)&tmpSzUI64);
        Rval |= ArmMemPool_Allocate(MemPoolID, tmpSzUI32, &pPrivatePtr->tskWorkBuf);
        // ArmLog_DBG(CTFW_TASK_NAME, "## alloc work buffer rval: %u, tskWorkBuf: 0x%p", Rval, pPrivatePtr->tskWorkBuf.pBuffer);
    }
    if (Rval == ARM_OK) {
        MONOVO_TASK_WORK_BUFF_s *pTskWorkPtr = NULL;

        AmbaMisra_TypeCast((void *)&pTskWorkPtr, (const void *)(&pPrivatePtr->tskWorkBuf.pBuffer));
        (void) AmbaWrap_memcpy((void *)&pTskWorkPtr->monoVoCfg, pAlgConfig, sizeof(AMBA_MONOVO_CFG_s));
        Rval |= AmbaMonoVo_Cfg(pMonoVoHdlr, &pTskWorkPtr->monoVoCfg);
        // ArmLog_DBG(CTFW_TASK_NAME, "## configure %u", Rval, 0U);
    }

    return Rval;
}

static UINT32 CtfwMonoVo_Process(const CtfwFrameMetadata_t *pMetadata, const flexidag_memblk_t* const pAlgInputData[],
                                 const flexidag_memblk_t* const pAlgBuffMap, flexidag_memblk_t *pAlgOutputData)
{
    UINT32 Rval = ARM_OK;
    MONOVO_PRIVATE_s *pPrivatePtr = NULL;
    void *pMonoVoHdlr = NULL;
    AMBA_CV_SPU_BUF_s pIn;
    FLOAT pMatchPt1[AMBA_MONOVO_SAMPLE_MAX], pMatchPt2[AMBA_MONOVO_SAMPLE_MAX];
    AMBA_MONOVO_BUF_s pOut;

    (void) pMetadata;
    AmbaMisra_TouchUnused(pAlgOutputData);

    // ArmLog_DBG(CTFW_TASK_NAME, "## process start", 0U, 0U);

    /* get handler buffer */
    Rval = AmbaWrap_memcpy((void *)&pPrivatePtr, (const void *)&pAlgBuffMap->pBuffer, sizeof(void *));
    AmbaMisra_TypeCast((void *)&pMonoVoHdlr, (void *)&pPrivatePtr->tskHdlrBuf.pBuffer);

    /* arrange input by copy value from spu dof buffer */
    if (Rval == ARM_OK) {
        Rval = AmbaWrap_memcpy((void *)&pIn, (const void *)pAlgInputData[0U], sizeof(flexidag_memblk_t));
        // ArmLog_DBG(CTFW_TASK_NAME, " ## input ready: 0x%p", pIn.pBuffer, 0U);
    }

    /* Sample spufex matching points here */
    if (Rval == ARM_OK) {
        Rval = AmbaMonoVo_SampleDof(pMonoVoHdlr, &pIn, pMatchPt1, pMatchPt2);
        // ArmLog_DBG(CTFW_TASK_NAME, "AmbaMonoVo_SampleDof() rval: %d", Rval, 0U);
    }

    if (Rval == ARM_OK) {
        Rval = AmbaMonoVo_Process(pMonoVoHdlr, pMatchPt1, pMatchPt2);
        // ArmLog_DBG(CTFW_TASK_NAME, "AmbaMonoVo_Process() rval: %d", Rval, 0U);
    }

    /* arrange output by copy address*/
    if (Rval == ARM_OK) {
        (void) AmbaWrap_memcpy((void *)&pOut, (void *)pAlgOutputData, sizeof(flexidag_memblk_t));
        Rval = AmbaMonoVo_GetResult(pMonoVoHdlr, &pOut);
        // ArmLog_DBG(CTFW_TASK_NAME, "AmbaMonoVo_GetResult(), rval: %d", Rval, 0U);
    }

    return Rval;
}

static UINT32 CtfwMonoVo_DeInit(UINT32 MemPoolID, const flexidag_memblk_t* const pAlgBuffMap)
{
    UINT32 RetVal = ARM_OK;
    (void) MemPoolID;
    (void) pAlgBuffMap;
    return RetVal;
}

CtfwTaskObj_t gCtfwMonoVoTaskObj = {
    .pSetName = CtfwMonoVo_SetName,
    .pQuerySize = CtfwMonoVo_QuerySize,
    .pInit = CtfwMonoVo_Init,
    .pProcess = CtfwMonoVo_Process,
    .pDeInit = CtfwMonoVo_DeInit,
};