#include "Ctfw_TaskUtil.h"
#include "Ctfw_SourceTask.h"
#include "AmbaMisraFix.h"

#define CTFW_TASK_NAME "Source"

static UINT32 CftwSource_QuerySize(UINT32 *pTaskStackSize, UINT32 *pAlgBuffMapSize, UINT32 *pAlgWorkSize, UINT32 *pAlgOutputSize)
{
    *pTaskStackSize = 16384U;
    *pAlgBuffMapSize = 0U;
    *pAlgWorkSize = 0U;
    *pAlgOutputSize = 0U;
    return ARM_OK;
}

static UINT32 CftwSource_SetName(char TaskName[])
{
    AmbaUtility_StringAppend(TaskName, MAX_CTFW_NAME_LEN, CTFW_TASK_NAME);
    return ARM_OK;
}

static UINT32 CftwSource_Init(const void *pAlgConfig, flexidag_memblk_t FlexiBinBlk[],
                              UINT32 MemPoolID, const flexidag_memblk_t* const pAlgBuffMap, CtfwTaskConfig_t *pTaskSetting)
{
    (void) pAlgConfig;
    AmbaMisra_TouchUnused(FlexiBinBlk);
    (void) MemPoolID;
    (void) pAlgBuffMap;

    // Task env setting
    {
        pTaskSetting->ProcInterval = 1U;
        pTaskSetting->TaskPriority = 50U;
        pTaskSetting->TaskCoreSel = 0U;
    }

    return ARM_OK;
}

static UINT32 CftwSource_Process(const CtfwFrameMetadata_t *pMetadata, const flexidag_memblk_t* const pAlgInputData[],
                                 const flexidag_memblk_t* const pAlgBuffMap, flexidag_memblk_t* const pAlgOutputData)
{
    const flexidag_memblk_t *pFirstInput;

    (void) pMetadata;
    (void) pAlgBuffMap;

    // simply copy input address to output
    pFirstInput = pAlgInputData[0U];
    *pAlgOutputData = *pFirstInput;

    return ARM_OK;
}

static UINT32 CftwSource_DeInit(UINT32 MemPoolID, const flexidag_memblk_t* const pAlgBuffMap)
{
    (void) MemPoolID;
    (void) pAlgBuffMap;
    return ARM_OK;
}

CtfwTaskObj_t gCtfwSourceTaskObj = {
    .pSetName = CftwSource_SetName,
    .pQuerySize = CftwSource_QuerySize,
    .pInit = CftwSource_Init,
    .pProcess = CftwSource_Process,
    .pDeInit = CftwSource_DeInit,
};

