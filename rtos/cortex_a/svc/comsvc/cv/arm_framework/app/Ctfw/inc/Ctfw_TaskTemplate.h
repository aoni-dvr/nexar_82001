#ifndef CTFW_TASK_TEMPLATE_H
#define CTFW_TASK_TEMPLATE_H

#include "Ctfw_TaskUtil.h"

typedef struct {
    UINT32 RegSlot[MAX_CTFW_TASK_INSTANCE_NUM];
    char TaskName[MAX_CTFW_NAME_LEN];
} CtfwRegTask_t;

typedef struct {
    UINT32 RegTaskNum;
    CtfwRegTask_t RegTask[MAX_CTFW_TASK_NUM];
} CtfwRegDesk_t;

UINT32 Ctfw_Init(void);
UINT32 Ctfw_TaskQuerySize(const CtfwTaskObj_t *pAlgTaskObj, UINT32 OutBuffDepth, UINT32 *pTotalSize);
UINT32 Ctfw_TaskInit(const CtfwTaskObj_t *pAlgTaskObj, CtfwGetProcResult_t pGetProcCbFxn,
                     const void *pAlgoConfig, flexidag_memblk_t FlexiBinBlk[], const char* FlowName,
                     UINT32 MemPoolId, UINT32 OutBuffDepth, CtfwTaskHandle_t *pHandle);
UINT32 Ctfw_TaskStart(CtfwTaskHandle_t *pHandle);
UINT32 Ctfw_TaskStop(CtfwTaskHandle_t *pHandle);
UINT32 Ctfw_TaskDeinit(UINT32 MemPoolId, CtfwTaskHandle_t *pHandle);

#endif
