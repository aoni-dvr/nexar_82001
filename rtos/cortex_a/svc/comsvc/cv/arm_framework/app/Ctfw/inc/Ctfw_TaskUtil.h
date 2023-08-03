#ifndef CTFW_TASK_UTIL_H
#define CTFW_TASK_UTIL_H

#include "SvcCvAlgo.h"
#include "AmbaUtility.h"
#include "AmbaKAL.h"
//#include "AmbaPrint.h"
#include "AmbaWrap.h"
//#include<sys/time.h>

static inline UINT32 CTFW_ALIGN128(UINT32 x)
{
    return (((x) + 127U) & 0xFFFFFF80U);
}

static inline UINT32 CTFW_ALIGN4K(UINT32 x)
{
    return (((x) + 4095U) & 0xFFFFF000U);
}

#if defined(CONFIG_QNX)
#define CTFW_CV_MEM_ALIGN CTFW_ALIGN4K
#elif defined(CONFIG_THREADX)
#define CTFW_CV_MEM_ALIGN CTFW_ALIGN128
#elif defined(CONFIG_LINUX)
// Should be 128? But align with 4K in ambalink
#define CTFW_CV_MEM_ALIGN CTFW_ALIGN4K
#else
#error "Unknown OS. Add definition first: CONFIG_${OS_NAME}"
#endif

#define MAX_PROFILER_NUM (8U)
#define CTFW_MAX_BUFFER_DEPTH (4U)
#define MAX_MSG_BUFFER_DEPTH (CTFW_MAX_BUFFER_DEPTH*4U)

#define MAX_CTFW_TASK_NUM (32U)
#define MAX_CTFW_TASK_INSTANCE_NUM (4U)

#define MAX_CTFW_NAME_LEN (64U)

#define TO_UP_STREAM (0U)
#define TO_DOWN_STREAM (1U)
#define FROM_UP_STREAM (0U)
#define FROM_DOWN_STREAM (1U)

#define MAX_UP_STREAM_NODES     (6U)
#define MAX_DOWN_STREAM_NODES   (6U)

#define CTFW_TASK_STATE_NA    (0U)
#define CTFW_TASK_STATE_IDLE  (1U)
#define CTFW_TASK_STATE_READY (2U)
#define CTFW_TASK_STATE_PROC  (3U)
#define CTFW_TASK_STATE_NUM   (4U)

#define CTFW_TASK_CMD_INIT    (1U)
#define CTFW_TASK_CMD_START   (2U)
#define CTFW_TASK_CMD_STOP    (3U)
#define CTFW_TASK_CMD_DEINIT  (4U)
#define CTFW_TASK_CMD_UPCB    (5U)
#define CTFW_TASK_CMD_DNCB    (6U)
#define CTFW_TASK_CMD_FLEXI_DONE_CB       (7U)
#define CTFW_TASK_CMD_CONNECT_UP_STREAM   (8U)
#define CTFW_TASK_CMD_CONNECT_DOWN_STREAM (9U)
#define CTFW_TASK_CMD_DISCONNECT_UP_STREAM   (10U)
#define CTFW_TASK_CMD_DISCONNECT_DOWN_STREAM (11U)
#define CTFW_TASK_CMD_NUM     (12U)

// Ctfw task type
#define CTFW_TASK_FEX           (0U)
#define CTFW_TASK_SPU_FEX       (1U)

typedef struct {
    UINT32 ProcInterval;
    UINT32 TaskPriority;
    UINT32 TaskCoreSel;
} CtfwTaskConfig_t;

typedef struct {
    flexidag_memblk_t Left;
    flexidag_memblk_t Right;
} CtfwCalibInfo_t;

typedef struct {
    UINT64 StartTimeUs; // Keep start time
    UINT64 AccuTimeUs;  // Accumulate Diff from current time to StartTime when Ctfw_ProfilerTimerStop is called
    UINT32 AccuCount;   // How many times the Ctfw_ProfilerTimerStop is called
    UINT32 MaxTimeDiff;
    UINT32 MinTimeDiff;
    UINT32 ErrorCount;  // How many times the task get error from its processor
} CtfwProfiler_t;

struct CtfwTaskHandle_s;

typedef struct {
    char Task[MAX_CTFW_NAME_LEN];
    char MsgQ[MAX_CTFW_NAME_LEN];
    char InBufMtx[MAX_CTFW_NAME_LEN];
    char OutBufMtx[MAX_CTFW_NAME_LEN];
    char TaskMtx[MAX_CTFW_NAME_LEN];
} CtfwResourceName_t;

typedef struct {
    UINT32 FrameIndex;
    UINT32 TimeStamp;
    UINT32 IsLastFrame;
    UINT32 ErrorState;
    CtfwCalibInfo_t CalibInfo;
} CtfwFrameMetadata_t;

typedef struct {
    flexidag_memblk_t CtfwData;
    flexidag_memblk_t MetaData;
} CtfwFrame_t;

typedef struct {
    struct CtfwTaskHandle_s *pFromHandle;
    struct CtfwTaskHandle_s *pToHandle;
    CtfwFrame_t Frame;
} CtfwDesc_t;

typedef struct {
    UINT32 Depth;
    CtfwDesc_t       Desc[CTFW_MAX_BUFFER_DEPTH];
    UINT32 ReadIdx;
    UINT32 WriteIdx;
} CtfwBuffer_t;

//Note: pointers assigned into CtfwCbMsg_t cannot be volatile.
typedef struct {
    CtfwDesc_t Desc;
    void *pPrivate;
} CtfwCbMsg_t;

typedef struct {
    UINT32 Cmd;
    CtfwDesc_t Desc;
    void *pPrivate;
} CtfwTaskMsg_t;

typedef void (*CtfwGetProcResult_t)(const CtfwFrameMetadata_t *pMetadata, const void *pOutData);

typedef struct {
    UINT32 (*pSetName)(char TaskName[]);
    UINT32 (*pQuerySize)(UINT32 *pTaskStackSize, UINT32 *pAlgBuffMapSize, UINT32 *pAlgWorkSize, UINT32 *pAlgOutputSize);
    UINT32 (*pInit)(const void *pAlgConfig, flexidag_memblk_t FlexiBinBlk[],
                    UINT32 MemPoolID, const flexidag_memblk_t* const pAlgBuffMap, CtfwTaskConfig_t *pTaskSetting);
    UINT32 (*pProcess)(const CtfwFrameMetadata_t *pMetadata, const flexidag_memblk_t* const pAlgInputData[],
                       const flexidag_memblk_t* const pAlgBuffMap, flexidag_memblk_t* const pAlgOutputData);
    UINT32 (*pDeInit)(UINT32 MemPoolID, const flexidag_memblk_t* const pAlgBuffMap);
} CtfwTaskObj_t;

typedef struct CtfwTaskHandle_s {
    UINT32 MemPoolId;
    char RegTaskName[MAX_CTFW_NAME_LEN];
    UINT32 InstId;
    UINT32 CurrState;
    UINT32 TaskRun;
    CCF_TSK_s Task;
    ArmMsgQueue_t TaskMsgQ;
    CtfwTaskMsg_t TaskMsgQBuffer[MAX_MSG_BUFFER_DEPTH];
    ArmMutex_t TskMutex;
    ArmMutex_t InBufMtx;
    ArmMutex_t OutBufMtx;
    CtfwBuffer_t InputBuffer[MAX_UP_STREAM_NODES];
    CtfwBuffer_t OutputBuffer[MAX_DOWN_STREAM_NODES];
    flexidag_memblk_t OutBuf[CTFW_MAX_BUFFER_DEPTH];
    flexidag_memblk_t OutMetadataBuf[CTFW_MAX_BUFFER_DEPTH];
    UINT32 OutBuffDepth;
    flexidag_memblk_t AlgoTaskTotalBuf;
    CtfwResourceName_t CtfwResourceName;
    void (*UpStreamCallBack)(const CtfwCbMsg_t *pMsg);
    void (*DownStreamCallBack)(const CtfwCbMsg_t *pMsg);
    UINT32 UpStreamTaskNum;
    struct CtfwTaskHandle_s *pUpStreamList[MAX_UP_STREAM_NODES];
    UINT32 DownStreamTaskNum;
    struct CtfwTaskHandle_s *pDownStreamList[MAX_DOWN_STREAM_NODES];
    CtfwProfiler_t Profiler[MAX_PROFILER_NUM];
    CtfwGetProcResult_t GetProcCbFxn;

    CtfwTaskConfig_t TaskSetting;
    CtfwTaskObj_t AlgTaskObj;
    flexidag_memblk_t AlgBuffMap;
} CtfwTaskHandle_t;

const char* Ctfw_GetTaskName(const CtfwTaskHandle_t *pHandle);
UINT32 Ctfw_CheckRetVal(const char *pCallerName, const char *pCalleeName, UINT32 RetVal);

UINT32 Ctfw_ProcUpStreamCb(CtfwTaskHandle_t *pHandle, const CtfwTaskMsg_t *pMsg);
UINT32 Ctfw_CallbackUpStream(CtfwTaskHandle_t *pHandle);
UINT32 Ctfw_CallbackDownStream(CtfwTaskHandle_t *pHandle);

UINT32 Ctfw_WaitState(CtfwTaskHandle_t *pHandle, UINT32 State);
UINT32 Ctfw_MsgQueueCreate(CtfwTaskHandle_t *pHandle);
UINT32 Ctfw_MsgQueueDelete(CtfwTaskHandle_t *pHandle);
UINT32 Ctfw_MsgQueueSend(CtfwTaskHandle_t *pHandle, UINT32 Cmd, const CtfwCbMsg_t *pCbMsg);

UINT32 Ctfw_InBufferInit(CtfwTaskHandle_t *pHandle, UINT32 Depth);
UINT32 Ctfw_InBufferDeinit(CtfwTaskHandle_t *pHandle, UINT32 Depth);
UINT32 Ctfw_OutBufferInit(CtfwTaskHandle_t *pHandle, UINT32 Depth);
UINT32 Ctfw_OutBufferDeinit(CtfwTaskHandle_t *pHandle, UINT32 Depth);

UINT32 Ctfw_IsBufferEmpty(CtfwTaskHandle_t *pHandle, const CtfwBuffer_t *pBuf, UINT32 *pIsEmpty);
UINT32 Ctfw_IsBufferFull(CtfwTaskHandle_t *pHandle, const CtfwBuffer_t *pBuf, UINT32 *pIsFull);
UINT32 Ctfw_BufferUprpToAll(CtfwTaskHandle_t *pHandle, UINT32 ToUpDown);
UINT32 Ctfw_BufferUprp(CtfwTaskHandle_t *pHandle, CtfwBuffer_t *pBuf);
UINT32 Ctfw_BufferUpwp(CtfwTaskHandle_t *pHandle, CtfwBuffer_t *pBuf);
UINT32 Ctfw_BufferDescPush(CtfwTaskHandle_t *pHandle, CtfwBuffer_t *pBuf, const CtfwDesc_t *pDesc);
UINT32 Ctfw_BufferDescPushToAllDownStream(CtfwTaskHandle_t *pHandle, const CtfwDesc_t *pCtfwDesc);
UINT32 Ctfw_CheckInOutReady(CtfwTaskHandle_t *pHandle);

UINT32 Ctfw_WhosCall(const CtfwTaskHandle_t *pHandle, const CtfwTaskHandle_t *pFromHandle, UINT32 *pIndex, UINT32 FromUpDown);

UINT32 Ctfw_ProfilerGetTime(UINT64 *pTimeUs);
UINT32 Ctfw_ProfilerTimerReset(CtfwTaskHandle_t *pHandle, UINT32 ProfileId);
UINT32 Ctfw_ProfilerStart(CtfwTaskHandle_t *pHandle, UINT32 ProfileId);
UINT32 Ctfw_ProfilerStop(CtfwTaskHandle_t *pHandle, UINT32 ProcStat, UINT32 ProfileId);
UINT32 Ctfw_ProfilerTimerReport(const CtfwTaskHandle_t *pHandle, UINT32 ProfileId);

UINT32 Ctfw_TaskConnect(CtfwTaskHandle_t *pUpStreamNode, CtfwTaskHandle_t *pDownStreamNode);
UINT32 Ctfw_CmdConnectDownStream(CtfwTaskHandle_t *pHandle, CtfwTaskHandle_t *pDownStreamNode);
UINT32 Ctfw_CmdConnectUpStream(CtfwTaskHandle_t *pHandle, CtfwTaskHandle_t *pUpStreamNode);
UINT32 Ctfw_CmdDisconnectStream(CtfwTaskHandle_t *pHandle, CtfwTaskHandle_t *pNode, UINT32 UpDown);
UINT32 Ctfw_InsertUpStreamList(CtfwTaskHandle_t *pHandle, CtfwTaskHandle_t *pUpStreamNode);
UINT32 Ctfw_InsertDownStreamList(CtfwTaskHandle_t *pHandle, CtfwTaskHandle_t *pDownStreamNode);
UINT32 Ctfw_DeleteStreamList(CtfwTaskHandle_t *pHandle, const CtfwTaskHandle_t *pNode, UINT32 UpDown);
UINT32 Ctfw_ConnectUpCheck(const CtfwTaskHandle_t *pHandle, const CtfwTaskHandle_t *pUpStreamNode);
UINT32 Ctfw_ConnectDownCheck(const CtfwTaskHandle_t *pHandle, const CtfwTaskHandle_t *pDownStreamNode);
UINT32 Ctfw_DisconnectCheck(const CtfwTaskHandle_t *pHandle, const CtfwTaskHandle_t *pNode, UINT32 UpDown);
UINT32 Ctfw_TaskConnect_TF(CtfwTaskHandle_t *pUpStreamNode, CtfwTaskHandle_t *pDownStreamNode);
UINT32 Ctfw_TaskDisconnect_TF(CtfwTaskHandle_t *pUpStreamNode, CtfwTaskHandle_t *pDownStreamNode);
UINT32 Ctfw_TaskCheckConnection(const CtfwTaskHandle_t *pHandle);
UINT32 Ctfw_ShiftFlexiMemblkBuff(const flexidag_memblk_t *pIn, ULONG ShiftSize, flexidag_memblk_t *pOut);

UINT32 Ctfw_GetSourceFramePoolSize(UINT32 SrcDataSize, UINT32 PoolFrameNum, UINT32 *pFramePoolSize);
UINT32 Cftw_MemPool_Allocate(UINT32 PoolId, UINT32 ReqSize, flexidag_memblk_t *pOutBuf);
UINT32 Ctfw_AllocSourceFramePool(UINT32 SrcDataSize, UINT32 MemPoolID, CtfwFrame_t FramePool[], UINT32 PoolFrameNum);

#endif
