/**
 *  @file AmbaStu_AlgoTaskUtil.h
 *
 *  @copyright Copyright (c) 2018 Ambarella, Inc.
 *
 *  This file and its contents ("Software") are protected by intellectual property rights including, without limitation,
 *  U.S. and/or foreign copyrights.  This Software is also the confidential and proprietary information of Ambarella, Inc.
 *  and its licensors.  You may not use, reproduce, disclose, distribute, modify, or otherwise prepare derivative works
 *  of this Software or any portion thereof except pursuant to a signed license agreement or nondisclosure agreement with
 *  Ambarella, Inc. or its authorized affiliates.  In the absence of such an agreement, you agree to promptly notify and
 *  return this Software to Ambarella, Inc.
 *
 *  THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 *  WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL
 *  AMBARELLA, INC. OR ITS AFFILIATES BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 *  DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 *  COMPUTER FAILURE OR MALFUNCTION; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 *  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 *  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *  @details Provide Stereo algo task utilities
 *
 */

#ifndef __APPAC_ALGO_TASK_HANDLE_H__
#define __APPAC_ALGO_TASK_HANDLE_H__

#include "SvcCvAlgo.h"
#ifdef CONFIG_QNX
#include <sys/time.h>
#endif
#define MAX_PROFILER_NUM 8U
#define MAX_BUFFER_DEPTH 4U

#define MAX_APPAC_NAME_LEN 32U

#define TO_UP_STREAM 0U
#define TO_DOWN_STREAM 1U
#define FROM_UP_STREAM 0U
#define FROM_DOWN_STREAM 1U

#define MAX_UP_STREAM_NODES 4U
#define MAX_DOWN_STREAM_NODES 4U

#define APPAC_TASK_STATE_NA    0U
#define APPAC_TASK_STATE_IDLE  1U
#define APPAC_TASK_STATE_READY 2U
#define APPAC_TASK_STATE_PROC  3U
#define APPAC_TASK_STATE_NUM   4U

#define APPAC_TASK_CMD_INIT    1U
#define APPAC_TASK_CMD_START   2U
#define APPAC_TASK_CMD_STOP    3U
#define APPAC_TASK_CMD_DEINIT  4U
#define APPAC_TASK_CMD_UPCB    5U
#define APPAC_TASK_CMD_DNCB    6U
#define APPAC_TASK_CMD_FLEXI_DONE_CB       7U
#define APPAC_TASK_CMD_CONNECT_UP_STREAM   8U
#define APPAC_TASK_CMD_CONNECT_DOWN_STREAM 9U
#define APPAC_TASK_CMD_DISCONNECT_UP_STREAM   10U
#define APPAC_TASK_CMD_DISCONNECT_DOWN_STREAM 11U
#define APPAC_TASK_CMD_NUM     12U

typedef void (*AcGetResultCb_t)(void *pAcResult);


typedef struct CalibInfo_s {
    flexidag_memblk_t Left;
    flexidag_memblk_t Right;
} AppAcCalibInfo_t;

typedef struct AppAcMetadata_s{
    UINT32 FrameNum;
    AppAcCalibInfo_t CalibInfo;
} AppAcMetadata_t;

typedef struct AppAcPicInfo_s {
    flexidag_memblk_t PicInfo;
} AppAcPicInfo_t;

typedef struct AppAcProfiler_s {
    UINT64 StartTimeUs; // Keep start time
    UINT64 AccuTimeUs;  // Accumulate Diff from current time to StartTime when AppAc_ProfilerTimerStop is called
    UINT32 AccuCount;   // How many times the AppAc_ProfilerTimerStop is called
    UINT32 MaxTimeDiff;
    UINT32 MinTimeDiff;
} AppAcProfiler_t;

typedef struct AppAcCbMsg_s AppAcCbMsg_t;
typedef struct AppAcTaskHandle_s AppAcTaskHandle_t;

typedef struct AppAcResourceName_s {
    char Task[MAX_APPAC_NAME_LEN];
    char MsgQ[MAX_APPAC_NAME_LEN];
    char InBufMtx[MAX_APPAC_NAME_LEN];
    char OutBufMtx[MAX_APPAC_NAME_LEN];
    char TaskMtx[MAX_APPAC_NAME_LEN];
} AppAcResourceName_t;

typedef struct AppAcDesc_s {
    UINT32 FrameNum;
    AppAcTaskHandle_t *pFromHandle;
    AppAcTaskHandle_t *pToHandle;
    UINT32 IsLastFrame;
    flexidag_memblk_t Info;
    AppAcMetadata_t Metadata;
} AppAcDesc_t;

typedef struct AppAcBuffer_s {
    UINT32 Depth;
    AppAcDesc_t       Desc[MAX_BUFFER_DEPTH];

    UINT32 ReadIdx;
    UINT32 WriteIdx;
} AppAcBuffer_t;

typedef struct AppAcTaskHandle_s {
    UINT32 MemPoolId;
    UINT32 CurrState;
    UINT32 ProcInterval;
    CCF_TSK_s Task;
    ArmMsgQueue_t TaskMsgQ;
    ArmMutex_t Mutex;
    ArmMutex_t InBufMtx;
    ArmMutex_t OutBufMtx;
    AppAcBuffer_t InputBuffer[MAX_UP_STREAM_NODES];
    AppAcBuffer_t OutputBuffer[MAX_DOWN_STREAM_NODES];
    flexidag_memblk_t OutBuf[MAX_BUFFER_DEPTH];
    flexidag_memblk_t AlgoTaskTotalBuf;
    flexidag_memblk_t StateBuf;
    flexidag_memblk_t AppAcPrivateMemblk;
    AppAcResourceName_t AppAcResourceName;
    //SVC_CV_ALGO_HANDLE_s ScaHandle;
    void (*UpStreamCallBack)(AppAcCbMsg_t *Msg);
    void (*DownStreamCallBack)(AppAcCbMsg_t *Msg);
    struct AppAcTaskHandle_s *pUpStreamList[MAX_UP_STREAM_NODES];
    struct AppAcTaskHandle_s *pDownStreamList[MAX_DOWN_STREAM_NODES];
    AppAcProfiler_t Profiler[MAX_PROFILER_NUM];
    void *private;
} AppAcTaskHandle_t;

//Note: pointers assigned into AppAcCbMsg_t cannot be volatile.
typedef struct AppAcCbMsg_s {
    AppAcDesc_t Desc;
    void *pPrivate;
} AppAcCbMsg_t;

typedef struct AppAcTaskMsg_s{
    UINT32 Cmd;
    AppAcDesc_t Desc;
    void *pPrivate;
} AppAcTaskMsg_t;

UINT32 AppAc_ProcUpStreamCb(AppAcTaskHandle_t *pHandle, AppAcTaskMsg_t *pMsg);
UINT32 AppAc_CallbackUpStream(AppAcTaskHandle_t *pHandle);
UINT32 AppAc_CallbackDownStream(AppAcTaskHandle_t *pHandle);

UINT32 AppAc_WaitState(AppAcTaskHandle_t *pHandle, UINT32 State);
UINT32 AppAc_MsgQueueCreate(AppAcTaskHandle_t *pHandle);
UINT32 AppAc_MsgQueueDelete(AppAcTaskHandle_t *pHandle);
UINT32 AppAc_MsgQueueSend(AppAcTaskHandle_t *pHandle, UINT32 Cmd, AppAcCbMsg_t *pCbMsg);

UINT32 AppAc_BufferInit(AppAcTaskHandle_t *pHandle, UINT32 Depth, UINT32 OutBufSize);
UINT32 AppAc_BufferDeinit(AppAcTaskHandle_t *pHandle, UINT32 Depth);
UINT32 AppAc_IsBufferEmpty(AppAcTaskHandle_t *pHandle, AppAcBuffer_t *pBuf, UINT32 *pIsEmpty);
UINT32 AppAc_IsBufferFull(AppAcTaskHandle_t *pHandle, AppAcBuffer_t *pBuf, UINT32 *pIsFull);
UINT32 AppAc_BufferUprpToAll(AppAcTaskHandle_t *pHandle, UINT32 ToUpDown);
UINT32 AppAc_BufferUprp(AppAcTaskHandle_t *pHandle, AppAcBuffer_t *pBuf);
UINT32 AppAc_BufferUpwp(AppAcTaskHandle_t *pHandle, AppAcBuffer_t *pBuf);
UINT32 AppAc_BufferDescPush(AppAcTaskHandle_t *pHandle, AppAcBuffer_t *pBuf, AppAcDesc_t *pDesc);
UINT32 AppAc_BufferDescPushToAllDownStream(AppAcTaskHandle_t *pHandle, AppAcDesc_t *pAppAcDesc);
UINT32 AppAc_CheckInOutReady(AppAcTaskHandle_t *pHandle);

UINT32 AppAc_WhosCall(AppAcTaskHandle_t *pHandle, AppAcTaskHandle_t *pFromHandle, UINT32 *pIndex, UINT32 FromUpDown);

UINT32 AppAc_ProfilerGetTime(UINT64 *pTimeUs);
UINT32 AppAc_ProfilerTimerReset(AppAcTaskHandle_t *pHandle, UINT32 ProfileId);
UINT32 AppAc_ProfilerTimerStart(AppAcTaskHandle_t *pHandle, UINT32 ProfileId);
UINT32 AppAc_ProfilerTimerStop(AppAcTaskHandle_t *pHandle, UINT32 ProfileId);
UINT32 AppAc_ProfilerTimerReport(AppAcTaskHandle_t *pHandle, UINT32 ProfileId);

UINT32 AppAc_TaskConnect(AppAcTaskHandle_t *pUpStreamNode, AppAcTaskHandle_t *pDownStreamNode);
UINT32 AppAc_CmdConnectDownStream(AppAcTaskHandle_t *pHandle, AppAcTaskHandle_t *pDownStreamNode);
UINT32 AppAc_CmdConnectUpStream(AppAcTaskHandle_t *pHandle, AppAcTaskHandle_t *pUpStreamNode);
UINT32 AppAc_CmdDisconnectStream(AppAcTaskHandle_t *pHandle, AppAcTaskHandle_t *pNode, UINT32 UpDown);
UINT32 AppAc_InsertUpStreamList(AppAcTaskHandle_t *pHandle, AppAcTaskHandle_t *pUpStreamNode);
UINT32 AppAc_InsertDownStreamList(AppAcTaskHandle_t *pHandle, AppAcTaskHandle_t *pDownStreamNode);
UINT32 AppAc_DeleteStreamList(AppAcTaskHandle_t *pHandle, AppAcTaskHandle_t *pNode, UINT32 UpDown);
UINT32 AppAc_ConnectUpCheck(AppAcTaskHandle_t *pHandle, AppAcTaskHandle_t *pUpStreamNode);
UINT32 AppAc_ConnectDownCheck(AppAcTaskHandle_t *pHandle, AppAcTaskHandle_t *pDownStreamNode);
UINT32 AppAc_DisconnectCheck(AppAcTaskHandle_t *pHandle, AppAcTaskHandle_t *pNode, UINT32 UpDown);
UINT32 AppAc_TaskConnect_TF(AppAcTaskHandle_t *pUpStreamNode, AppAcTaskHandle_t *pDownStreamNode);
UINT32 AppAc_TaskDisconnect_TF(AppAcTaskHandle_t *pUpStreamNode, AppAcTaskHandle_t *pDownStreamNode);

UINT32 AppAc_GetFrameNum(AppAcPicInfo_t *pPicInfo, UINT32 *pFrameNum);

#endif
