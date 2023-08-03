/**
 *  @file SvcCvLogger.c
 *
 * Copyright (c) [2020] Ambarella International LP
 *
 * This file and its contents ("Software") are protected by intellectual
 * property rights including, without limitation, U.S. and/or foreign
 * copyrights. This Software is also the confidential and proprietary
 * information of Ambarella International LP and its licensors. You may not use, reproduce,
 * disclose, distribute, modify, or otherwise prepare derivative works of this
 * Software or any portion thereof except pursuant to a signed license agreement
 * or nondisclosure agreement with Ambarella International LP or its authorized affiliates.
 * In the absence of such an agreement, you agree to promptly notify and return
 * this Software to Ambarella International LP.
 *
 * This file includes sample code and is only for internal testing and evaluation.  If you
 * distribute this sample code (whether in source, object, or binary code form), it will be
 * without any warranty or indemnity protection from Ambarella International LP or its affiliates.
 *
 * THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF NON-INFRINGEMENT,
 * MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL AMBARELLA INTERNATIONAL LP OR ITS AFFILIATES BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; COMPUTER FAILURE OR MALFUNCTION; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 *  @details Implementation of Svc CV logger utility
 *
 */

#include "AmbaTypes.h"

#include "AmbaDSP.h"
#include "AmbaDSP_Liveview.h"
#include "AmbaDSP_Event_Def.h"
#include "AmbaDSP_Event.h"
#include "AmbaNAND.h"
#include "AmbaUtility.h"
#include "AmbaWrap.h"
#include "AmbaCache.h"

#include "SvcErrCode.h"
#include "SvcLog.h"
#include "SvcWrap.h"
#include "SvcTask.h"

#include "AmbaVfs.h"
#include "AmbaCodecCom.h"
#include "RefFlow_Common.h"
#include "AmbaSvcWrap.h"
#include "SvcCvLogger.h"

#define SVC_LOG_CV_LOGGER         "CV_LGR"

#define LOGGER_FLAG_FILE_CLOSED                 (1U)

#define LOGGER_FLUSH_TARGET_INDEX               (0U)
#define LOGGER_FLUSH_TARGET_TRUNK               (1U)

typedef struct {
    ULONG   BufAddr[SVC_CV_LOGGER_MAX_HDLR];
    UINT32  BufSize[SVC_CV_LOGGER_MAX_HDLR];
    SVC_CV_LOGGER_HDLR_s *pHdlr[SVC_CV_LOGGER_MAX_HDLR];
} SVC_CV_LOGGER_CTRL_s;

typedef struct {
    UINT8   Valid;
    UINT32  PTS;
    ULONG  TrunkPtr;
    UINT32  TrunkSize;
} SVC_CV_LOGGER_INDEX_s;

#define LOGGER_EVENT_FLUSH_BUFFER_TO_FILE       (0U)
#define LOGGER_EVENT_FLUSH_AND_CLOSE            (1U)    /* Flush all data to file and close the file */
#define LOGGER_EVENT_FLUSH_PTS_AND_OPEN         (2U)    /* Flush data to file by PTS and open new file */

typedef struct {
    UINT32  Event;
    SVC_CV_LOGGER_HDLR_s *pHdlr;
    UINT32  Param[1];
} SVC_CV_LOGGER_EVENT_s;

static UINT8                MaxLogger;
static SVC_CV_LOGGER_CTRL_s LoggerCtrl;
static AMBA_KAL_MSG_QUEUE_t LoggeQuetId;
static UINT8                SvcCvLoggerDbg = 0U;

static void   CvLogger_MutexTake(AMBA_KAL_MUTEX_t *pMutex);
static void   CvLogger_MutexGive(AMBA_KAL_MUTEX_t *pMutex);
static void*  CvLogger_TaskEntry(void* EntryArg);
static UINT32 CvLogger_BufferGet(SVC_CV_LOGGER_BUFFER_s *pTrunkBuf, UINT32 ReqSize, SVC_CV_LOGGER_WRITE_BUFFER_s *pWrBuf);
static UINT32 CvLogger_BufferUpdate(SVC_CV_LOGGER_BUFFER_s *pIndexBuf,
                                    SVC_CV_LOGGER_BUFFER_s *pTrunkBuf,
                                    SVC_CV_LOGGER_WRITE_BUFFER_s *pWrBuf,
                                    UINT32 WroteSize);

static UINT32 CvLogger_BufferFlushBySize(SVC_CV_LOGGER_BUFFER_s *pIndexBuf,
                                         SVC_CV_LOGGER_BUFFER_s *pTrunkBuf,
                                         const SVC_CV_LOGGER_FILE_DESC_s *pFileDesc,
                                         UINT32 TargetBuf, UINT32 FlushSize);
static UINT32 CvLogger_BufferFlushByPTS(SVC_CV_LOGGER_BUFFER_s *pIndexBuf,
                                        SVC_CV_LOGGER_BUFFER_s *pTrunkBuf,
                                        const SVC_CV_LOGGER_FILE_DESC_s *pFileDesc,
                                        UINT32 TargetPTS);

static UINT32 CvLogger_BufferSanityCheck(const SVC_CV_LOGGER_BUFFER_s *pBuf);
static void   CvLogger_BufferSpaceGet(ULONG BufBase, ULONG BufEnd, ULONG Wp, ULONG Rp, ULONG *pSpace);
//static void   CvLogger_BufferDataSizeGet(ULONG BufBase, ULONG BufEnd, ULONG Wp, ULONG Rp, UINT32 *pDataSize);
static UINT32 CvLogger_FileWrite(ULONG StartAddr, ULONG Size, ULONG BufBase, ULONG BufEnd, const SVC_CV_LOGGER_FILE_DESC_s *pFileDesc, UINT32 *pWrSize);
static UINT32 CvLogger_PTSCompare(UINT32 CurPTS, UINT32 TargetPTS);

static UINT32 FsWrap_FileOpen(const char *pFileName, const char *pMode, UINT32 EnableAsync, SVC_CV_LOGGER_FILE_DESC_s *pFileDesc);
static UINT32 FsWrap_FileClose(const SVC_CV_LOGGER_FILE_DESC_s *pFileDesc);
static UINT32 FsWrap_FileWrite(void *pBuf, UINT32 Size, UINT32 Count, const SVC_CV_LOGGER_FILE_DESC_s *pFileDesc, UINT32 *pNumSuccess);

void SvcCvLogger_DebugEnable(UINT32 DebugEnable)
{
    SvcCvLoggerDbg = (UINT8)DebugEnable;
}

static void CvLogger_UlongDbg(const char *pFormat, ULONG Arg1, ULONG Arg2, ULONG Arg3, ULONG Arg4, ULONG Arg5)
{
    if (SvcCvLoggerDbg > 0U) {
        SvcWrap_PrintUL(pFormat, Arg1, Arg2, Arg3, Arg4, Arg5);
    }
}

static void CvLogger_Dbg(const char *pFormat, UINT32 Arg1, UINT32 Arg2, UINT32 Arg3, UINT32 Arg4, UINT32 Arg5)
{
    if (SvcCvLoggerDbg > 0U) {
        AmbaPrint_PrintUInt5(pFormat, Arg1, Arg2, Arg3, Arg4, Arg5);
    }
}

static void CvLogger_MutexTake(AMBA_KAL_MUTEX_t *pMutex)
{
    if (KAL_ERR_NONE != AmbaKAL_MutexTake(pMutex, 5000)) {
        SvcLog_NG(SVC_LOG_CV_LOGGER, "MutexTake: timeout", 0U, 0U);
    }
}

static void CvLogger_MutexGive(AMBA_KAL_MUTEX_t *pMutex)
{
    if (KAL_ERR_NONE != AmbaKAL_MutexGive(pMutex)) {
        SvcLog_NG(SVC_LOG_CV_LOGGER, "MutexGive: error", 0U, 0U);
    }
}

/**
* Initialize CV logger utility
* @param [in] pCfg initialization configuration of cv logger
* @return 0-OK, 1-NG
*/
UINT32 SvcCvLogger_Init(SVC_CV_LOGGER_INIT_CFG_s *pCfg)
{
    #define LOGGER_QUEUE_SIZE          (32U)
    #define LOGGER_STACK_SIZE          (0x2000U)
    UINT32 RetVal;
    UINT32 i, PartSize;
    ULONG  Base;
    static char MsgQueName[] = "LOGMQ";
    static SVC_CV_LOGGER_EVENT_s LoggerQueue[LOGGER_QUEUE_SIZE];
    static SVC_TASK_CTRL_s LoggerTask;
    static UINT8 TaskStack[LOGGER_STACK_SIZE] GNU_SECTION_NOZEROINIT;

    if ((pCfg != NULL) && (pCfg->MaxLogger < SVC_CV_LOGGER_MAX_HDLR)) {
        RetVal = AmbaWrap_memset(&LoggerCtrl, 0, sizeof(LoggerCtrl));
        if (SVC_OK != RetVal) {
            SvcLog_NG(SVC_LOG_CV_LOGGER, "memset LoggerCtrl failed", 0U, 0U);
        }
        MaxLogger = pCfg->MaxLogger;

        RetVal = AmbaKAL_MsgQueueCreate(&LoggeQuetId,
                                        MsgQueName,
                                        (UINT32)sizeof(SVC_CV_LOGGER_EVENT_s),
                                        LoggerQueue,
                                        (UINT32)LOGGER_QUEUE_SIZE * (UINT32)sizeof(SVC_CV_LOGGER_EVENT_s));
        if (RetVal != SVC_OK ) {
            SvcLog_NG(SVC_LOG_CV_LOGGER, "MsgQueueCreate error(0x%x)", RetVal, 0U);
        }

        /* Allocate memory */
        Base = pCfg->MemBase;
        PartSize = pCfg->MemSize / MaxLogger;
        for (i = 0; i < MaxLogger; i++) {
            LoggerCtrl.BufAddr[i] = Base;
            LoggerCtrl.BufSize[i] = PartSize;
            Base += PartSize;
        }

        LoggerTask.Priority   = 80;
        LoggerTask.EntryFunc  = CvLogger_TaskEntry;
        LoggerTask.EntryArg   = 0U;
        LoggerTask.pStackBase = TaskStack;
        LoggerTask.StackSize  = LOGGER_STACK_SIZE;
        LoggerTask.CpuBits    = 0x01U;
        RetVal = SvcTask_Create("CvLoggerTask", &LoggerTask);
        if (RetVal != SVC_OK ) {
            SvcLog_NG(SVC_LOG_CV_LOGGER, "TaskCreate error(0x%x)", RetVal, 0U);
        }
    } else {
        SvcLog_NG(SVC_LOG_CV_LOGGER, "Init: invalid info", 0U, 0U);
        RetVal = SVC_NG;
    }

    AmbaMisra_TouchUnused(pCfg);

    return RetVal;
}

/**
* Create CV logger Handler
* @param [in] pCfg configuration of CV logger
* @param [out] pHdlr CV logger handler
* @return 0-OK, 1-NG
*/
UINT32 SvcCvLogger_Create(SVC_CV_LOGGER_CFG_s *pCfg, SVC_CV_LOGGER_HDLR_s *pHdlr)
{
    #define DEFAULT_FHEAD_TRUNK_BUF_SIZE         (1024U)
    #define DEFAULT_FHEAD_INDEX_BUF_SIZE         (sizeof(SVC_CV_LOGGER_INDEX_s) * 10U)
    #define DEFAULT_LOG_TRUNK_BUF_SIZE           (4096U * 1024U)
    #define DEFAULT_LOG_INDEX_BUF_SIZE           (sizeof(SVC_CV_LOGGER_INDEX_s) * 1000U)
    #define DEFAULT_BUF_SIZE                     (DEFAULT_FHEAD_INDEX_BUF_SIZE + \
                                                  DEFAULT_LOG_INDEX_BUF_SIZE + \
                                                  DEFAULT_FHEAD_TRUNK_BUF_SIZE + \
                                                  DEFAULT_LOG_TRUNK_BUF_SIZE)
    UINT32 RetVal = SVC_NG;
    static char MutexName[] = "CvLogAPIMutex";
    static char EvtFlagName[] = "CvLogEvtFlag";
    UINT32 Idx, i;
    ULONG  BufBase;
    UINT32 RemainSpace;
    SVC_CV_LOGGER_BUFFER_s *pBuf;
    SVC_CV_LOGGER_INDEX_s *pIndexArr;

    if ((pCfg != NULL) && (pHdlr != NULL)) {
        RetVal = AmbaWrap_memset(pHdlr, 0, sizeof(SVC_CV_LOGGER_HDLR_s));
        if (SVC_OK != RetVal) {
            SvcLog_NG(SVC_LOG_CV_LOGGER, "memset pHdlr failed", 0U, 0U);
        }

        for (Idx = 0; Idx < MaxLogger; Idx++) {
            if (LoggerCtrl.pHdlr[Idx] == NULL) {
                LoggerCtrl.pHdlr[Idx] = pHdlr;
                pHdlr->ID = (UINT8)Idx;
                break;
            }
        }

        if (Idx < MaxLogger) {
            /* Create mutex */
            if (KAL_ERR_NONE != AmbaKAL_MutexCreate(&pHdlr->ApiMutex, MutexName)) {
                SvcLog_NG(SVC_LOG_CV_LOGGER, "MutexCreate failed", 0U, 0U);
                RetVal = SVC_NG;
            }

            if (KAL_ERR_NONE != AmbaKAL_EventFlagCreate(&pHdlr->EventFlag, EvtFlagName)) {
                SvcLog_NG(SVC_LOG_CV_LOGGER, "EventFlagCreate failed", 0U, 0U);
                RetVal = SVC_NG;
            }

            /* Allocate buffer */
            BufBase = LoggerCtrl.BufAddr[Idx];
            RemainSpace = LoggerCtrl.BufSize[Idx];

            if ((BufBase != 0U) && (RemainSpace >= DEFAULT_BUF_SIZE)) {
                /* File head index buffer */
                pBuf = &pHdlr->Buf[SVC_CV_LOGGER_FHEAD_INDEX_BUFFER];
                pBuf->Base   = BufBase;
                pBuf->Size   = (UINT32)DEFAULT_FHEAD_INDEX_BUF_SIZE;
                pBuf->End    = pBuf->Base + pBuf->Size;
                pBuf->Wp     = BufBase;
                pBuf->Rp     = BufBase;
                AmbaMisra_TypeCast(&pIndexArr, &pBuf->Base);
                for (i = 0; i < (DEFAULT_FHEAD_INDEX_BUF_SIZE / sizeof(SVC_CV_LOGGER_INDEX_s)); i++) {
                    pIndexArr[i].Valid = 0U;
                }
                CvLogger_UlongDbg("[CV_LGR] Alloc buf: Base 0x%x, Size %d, End 0x%x, Wp 0x%x, Rp 0x%x",
                                       pBuf->Base, (ULONG)pBuf->Size, pBuf->End, pBuf->Wp, pBuf->Rp);
                BufBase += DEFAULT_FHEAD_INDEX_BUF_SIZE;

                /* Log index buffer */
                pBuf = &pHdlr->Buf[SVC_CV_LOGGER_LOG_INDEX_BUFFER];
                pBuf->Base   = BufBase;
                pBuf->Size   = (UINT32)DEFAULT_LOG_INDEX_BUF_SIZE;
                pBuf->End    = pBuf->Base + pBuf->Size;
                pBuf->Wp     = BufBase;  /* Index 0 */
                pBuf->Rp     = BufBase;
                AmbaMisra_TypeCast(&pIndexArr, &pBuf->Base);
                for (i = 0; i < (DEFAULT_LOG_INDEX_BUF_SIZE / sizeof(SVC_CV_LOGGER_INDEX_s)); i++) {
                    pIndexArr[i].Valid = 0U;
                }
                CvLogger_UlongDbg("[CV_LGR] Alloc buf: Base 0x%x, Size %d, End 0x%x, Wp 0x%x, Rp 0x%x",
                                       pBuf->Base, (ULONG)pBuf->Size, pBuf->End, pBuf->Wp, pBuf->Rp);
                BufBase += DEFAULT_LOG_INDEX_BUF_SIZE;

                /* File head trunk buffer */
                pBuf = &pHdlr->Buf[SVC_CV_LOGGER_FHEAD_TRUNK_BUFFER];
                pBuf->Base   = BufBase;
                pBuf->Size   = DEFAULT_FHEAD_TRUNK_BUF_SIZE;
                pBuf->End    = pBuf->Base + pBuf->Size;
                pBuf->Wp     = BufBase;
                pBuf->Rp     = BufBase;
                CvLogger_UlongDbg("[CV_LGR] Alloc buf: Base 0x%x, Size %d, End 0x%x, Wp 0x%x, Rp 0x%x",
                                       pBuf->Base, (ULONG)pBuf->Size, pBuf->End, pBuf->Wp, pBuf->Rp);
                BufBase += pBuf->Size;

                /* Log trunk buffer */
                pBuf = &pHdlr->Buf[SVC_CV_LOGGER_LOG_TRUNK_BUFFER];
                pBuf->Base   = BufBase;
                pBuf->Size   = (UINT32)((LoggerCtrl.BufAddr[Idx] + (ULONG)(LoggerCtrl.BufSize[Idx])) - BufBase); /* Remain */
                pBuf->End    = pBuf->Base + pBuf->Size;
                pBuf->Wp     = BufBase;
                pBuf->Rp     = BufBase;
                pHdlr->LogTrunkBufTh = ((DEFAULT_LOG_TRUNK_BUF_SIZE * 3U) / 5U);
                CvLogger_UlongDbg("[CV_LGR] Alloc buf: Base 0x%x, Size %d, End 0x%x, Wp 0x%x, Rp 0x%x",
                                       pBuf->Base, (ULONG)pBuf->Size, pBuf->End, pBuf->Wp, pBuf->Rp);
            } else {
                SvcLog_NG(SVC_LOG_CV_LOGGER, "Alloc mem failed. Remain(%u), Req(%u)", RemainSpace, (UINT32)DEFAULT_BUF_SIZE);
                RetVal = SVC_NG;
            }

            if (RetVal == SVC_OK) {
                /* Open Log file */
                AmbaPrint_PrintStr5("[CV_LGR] Create %s", pCfg->pFileName, NULL, NULL, NULL, NULL);
                RetVal = FsWrap_FileOpen(pCfg->pFileName, "w", 1U, &pHdlr->FileDesc);
                if (RetVal == SVC_OK) {
                    SvcWrap_strcpy(pHdlr->FileName, sizeof(pHdlr->FileName), pCfg->pFileName);
                    pHdlr->Started = 1U;
                } else {
                    SvcLog_NG(SVC_LOG_CV_LOGGER, "FileOpen error(0x%x)", RetVal, 0U);
                }
            }
        } else {
            SvcLog_NG(SVC_LOG_CV_LOGGER, "Exceed max(%u) hdlr", MaxLogger, 0U);
            RetVal = SVC_NG;
        }
    } else {
        SvcLog_NG(SVC_LOG_CV_LOGGER, "Create: invalid info", 0U, 0U);
        RetVal = SVC_NG;
    }

    AmbaMisra_TouchUnused(pCfg);

    return RetVal;
}

/**
* Delete CV logger Handler
* @param [in] pHdlr CV logger handler
* @return 0-OK, 1-NG
*/
UINT32 SvcCvLogger_Delete(SVC_CV_LOGGER_HDLR_s *pHdlr)
{
    UINT32 RetVal;
    UINT32 ActualFlag = 0;
    SVC_CV_LOGGER_EVENT_s EventInfo;

    if ((pHdlr != NULL) && (pHdlr->Started == 1U)) {
        CvLogger_MutexTake(&pHdlr->ApiMutex);

        LoggerCtrl.pHdlr[pHdlr->ID] = NULL;
        pHdlr->Started = 0U;

        RetVal = AmbaKAL_EventFlagClear(&pHdlr->EventFlag, LOGGER_FLAG_FILE_CLOSED);
        if (RetVal != SVC_OK) {
            SvcLog_NG(SVC_LOG_CV_LOGGER, "EventFlagGet error(0x%x)", RetVal, 0U);
        }

        EventInfo.Event = LOGGER_EVENT_FLUSH_AND_CLOSE;
        EventInfo.pHdlr = pHdlr;
        RetVal = AmbaKAL_MsgQueueSend(&LoggeQuetId, &EventInfo, AMBA_KAL_WAIT_FOREVER);
        if (RetVal != SVC_OK) {
            SvcLog_NG(SVC_LOG_CV_LOGGER, "Send FLUSH_REQ error(0x%x)", RetVal, 0U);
        }

        RetVal = AmbaKAL_EventFlagGet(&pHdlr->EventFlag,
                                      LOGGER_FLAG_FILE_CLOSED,
                                      AMBA_KAL_FLAGS_ANY,
                                      AMBA_KAL_FLAGS_CLEAR_AUTO,
                                      &ActualFlag,
                                      AMBA_KAL_WAIT_FOREVER);
        if (RetVal != SVC_OK) {
            SvcLog_NG(SVC_LOG_CV_LOGGER, "EventFlagGet error(0x%x)", RetVal, 0U);
        }

        CvLogger_MutexGive(&pHdlr->ApiMutex);

        RetVal = AmbaKAL_EventFlagDelete(&pHdlr->EventFlag);
        RetVal |= AmbaKAL_MutexDelete(&pHdlr->ApiMutex);
        if (RetVal != KAL_ERR_NONE) {
            SvcLog_NG(SVC_LOG_CV_LOGGER, "Delete Flag/Mutex error(0x%X)", RetVal, 0U);
            RetVal = SVC_NG;
        }
    } else {
        SvcLog_NG(SVC_LOG_CV_LOGGER, "Create: invalid info", 0U, 0U);
        RetVal = SVC_NG;
    }

    return RetVal;
}

/**
* Get a buffer space to write a trunk
* @param [in] pHdlr CV logger handler
* @param [in] ReqSize Required buffer size
* @param [in] Flag Flag to indicate the attribute of the trunk
* @param [out] pWrBuf pointer to write buffer
* @return 0-OK, 1-NG
*/
UINT32 SvcCvLogger_TrunkBufferGet(SVC_CV_LOGGER_HDLR_s *pHdlr, UINT32 ReqSize, UINT32 Flag, SVC_CV_LOGGER_WRITE_BUFFER_s *pWrBuf)
{
    UINT32 RetVal = SVC_OK;
    UINT32 BufType;

    if ((pHdlr != NULL) &&
        (pHdlr->Started == 1U) &&
        (pWrBuf != NULL)) {
        CvLogger_MutexTake(&pHdlr->ApiMutex);

        if (0U < (Flag & SVC_CV_LOGGER_FLAG_FILE_HEAD)) {
            BufType = SVC_CV_LOGGER_FHEAD_TRUNK_BUFFER;
        } else {
            BufType = SVC_CV_LOGGER_LOG_TRUNK_BUFFER;
        }

        pWrBuf->Flag = Flag;
        RetVal = CvLogger_BufferGet(&pHdlr->Buf[BufType], ReqSize, pWrBuf);
        if (RetVal != SVC_OK) {
            SvcLog_NG(SVC_LOG_CV_LOGGER, "WriteBufferGet: BufferGet failed", 0U, 0U);
        }

        //CvLogger_Dbg("[CV_LGR%u] BufferGet: Addr 0x%x, %d, W/R 0x%x 0x%x", pHdlr->ID, pWrBuf->Addr, pWrBuf->Size, pHdlr->Buf[BufType].Wp, pHdlr->Buf[BufType].Rp);

        CvLogger_MutexGive(&pHdlr->ApiMutex);
    } else {
        SvcLog_NG(SVC_LOG_CV_LOGGER, "WriteBufferGet: invalid info", 0U, 0U);
        RetVal = SVC_NG;
    }

    return RetVal;
}

/**
* Update the written trunk data
* @param [in] pHdlr CV logger handler
* @param [in] pWrBuf pointer to write buffer
* @param [out] WroteSize used size
* @return 0-OK, 1-NG
*/
UINT32 SvcCvLogger_TrunkBufferUpdate(SVC_CV_LOGGER_HDLR_s *pHdlr, SVC_CV_LOGGER_WRITE_BUFFER_s *pWrBuf, UINT32 WroteSize)
{
    UINT32 RetVal = SVC_OK;
    UINT32 BufType, IdxBufType;
    UINT32 FlushTh0, FlushTh1;
    ULONG BufSpace, IdxSpace;
    SVC_CV_LOGGER_BUFFER_s *pTrunkBuf, *pIndexBuf;
    SVC_CV_LOGGER_EVENT_s EventInfo;

    if ((pHdlr != NULL) &&
        (pHdlr->Started == 1U) &&
        (pWrBuf != NULL)) {
        CvLogger_MutexTake(&pHdlr->ApiMutex);

        //CvLogger_Dbg("[CV_LGR%u] BufferUp: Addr 0x%x, %d, Flag 0x%x", pHdlr->ID, pWrBuf->Addr, pWrBuf->Size, pWrBuf->Flag, 0U);

        if (0U < (pWrBuf->Flag & SVC_CV_LOGGER_FLAG_FILE_HEAD)) {
            BufType = SVC_CV_LOGGER_FHEAD_TRUNK_BUFFER;
            IdxBufType = SVC_CV_LOGGER_FHEAD_INDEX_BUFFER;
        } else {
            BufType = SVC_CV_LOGGER_LOG_TRUNK_BUFFER;
            IdxBufType = SVC_CV_LOGGER_LOG_INDEX_BUFFER;
        }

        pTrunkBuf = &pHdlr->Buf[BufType];
        pIndexBuf = &pHdlr->Buf[IdxBufType];

        RetVal = CvLogger_BufferUpdate(pIndexBuf,
                                       pTrunkBuf,
                                       pWrBuf,
                                       WroteSize);
        if (RetVal == SVC_OK) {
            if (SVC_OK != CvLogger_BufferSanityCheck(pTrunkBuf)) {
                SvcLog_NG(SVC_LOG_CV_LOGGER, "WriteBufferUpdate: buf[%u] disorder", BufType, 0U);
                RetVal |= SVC_NG;
            }
            if (SVC_OK != CvLogger_BufferSanityCheck(pIndexBuf)) {
                SvcLog_NG(SVC_LOG_CV_LOGGER, "WriteBufferUpdate: buf[%u] disorder", BufType, 0U);
                RetVal |= SVC_NG;
            }
        }

        if (RetVal == SVC_OK) {
            /* Check if buffers are closing to full */
            if (BufType == SVC_CV_LOGGER_LOG_TRUNK_BUFFER) {
                /* Flush if buffer space is under threshold */
                CvLogger_BufferSpaceGet(pTrunkBuf->Base, pTrunkBuf->End, pTrunkBuf->Wp, pTrunkBuf->Rp, &BufSpace);
                FlushTh0 = pHdlr->LogTrunkBufTh;
                CvLogger_BufferSpaceGet(pIndexBuf->Base, pIndexBuf->End, pIndexBuf->Wp, pIndexBuf->Rp, &IdxSpace);
                FlushTh1 = (pIndexBuf->Size >> 1U);
                //CvLogger_Dbg("[CV_LGR%u] Check: BufSp(%d %d) IndexSp(%d %d)", pHdlr->ID, BufSpace, FlushTh0, IdxSpace, FlushTh1);

                EventInfo.pHdlr = pHdlr;
                if ((BufSpace < FlushTh0) || (IdxSpace < FlushTh1)) {
                    EventInfo.Event = LOGGER_EVENT_FLUSH_BUFFER_TO_FILE;
                } else {
                    EventInfo.Event = 0xFFFFFFFFU;
                }

                if ((EventInfo.Event != 0xFFFFFFFFU) && (pHdlr->FlushReq < 3U)) {
                    RetVal = AmbaKAL_MsgQueueSend(&LoggeQuetId, &EventInfo, AMBA_KAL_NO_WAIT);
                    if (RetVal == SVC_OK) {
                        pHdlr->FlushReq++;
                        CvLogger_Dbg("[CV_LGR%u] send FLUSH_REQ", pHdlr->ID, 0U, 0U, 0U, 0U);
                    } else {
                        SvcLog_NG(SVC_LOG_CV_LOGGER, "TrunkBufferUpdate sends FLUSH_REQ error(0x%x)", RetVal, 0U);
                    }
                }
            } else {
                /* Do nothing */
            }
        } else {
            SvcLog_NG(SVC_LOG_CV_LOGGER, "WriteBufferUpdate: BufferUpdate failed", 0U, 0U);
        }

        CvLogger_MutexGive(&pHdlr->ApiMutex);
    } else {
        SvcLog_NG(SVC_LOG_CV_LOGGER, "WriteBufferUpdate: invalid info", 0U, 0U);
        RetVal = SVC_NG;
    }

    return RetVal;
}

/**
* Control interface of CV logger
* @param [in] pHdlr CV logger handler
* @param [in] CtrlType control type
* @param [pInfo] information of control
* @return 0-OK, 1-NG
*/
UINT32 SvcCvLogger_Ctrl(SVC_CV_LOGGER_HDLR_s *pHdlr, UINT32 CtrlType, void *pInfo)
{
    UINT32 RetVal = SVC_OK;
    SVC_CV_LOGGER_EVENT_s EventInfo;
    const SVC_CV_LOGGER_VIDEO_STATUS_s *pVidInfo;

    if ((pHdlr != NULL) &&
        (pHdlr->Started == 1U)) {
        CvLogger_MutexTake(&pHdlr->ApiMutex);

        SvcLog_DBG(SVC_LOG_CV_LOGGER, "SvcCvLogger_Ctrl: 0x%x", CtrlType, 0U);

        switch (CtrlType) {
        case SVC_CV_LOGGER_CTRL_VIDEO_START:
            AmbaMisra_TypeCast(&pVidInfo, &pInfo);
            EventInfo.Event    = LOGGER_EVENT_FLUSH_PTS_AND_OPEN;
            EventInfo.pHdlr    = pHdlr;
            EventInfo.Param[0] = (UINT32)((UINT64)pVidInfo->VidCapPts - (UINT64)1U); /* search under the origianl VidCapPts */
            SvcWrap_strcpy(pHdlr->FileName, sizeof(pHdlr->FileName), pVidInfo->pFileName);
            RetVal = AmbaKAL_MsgQueueSend(&LoggeQuetId, &EventInfo, AMBA_KAL_WAIT_FOREVER);
            if (RetVal != SVC_OK) {
                SvcLog_NG(SVC_LOG_CV_LOGGER, "Ctrl sends FLUSH_PTS_AND_OPEN error(0x%x)", RetVal, 0U);
            }
            break;
        case SVC_CV_LOGGER_CTRL_VIDEO_CLOSE:
            AmbaMisra_TypeCast(&pVidInfo, &pInfo);
            if (pVidInfo->IsLastFile == 1U) {
                EventInfo.Event    = LOGGER_EVENT_FLUSH_PTS_AND_OPEN;
                EventInfo.pHdlr    = pHdlr;
                EventInfo.Param[0] = (UINT32)pVidInfo->VidCapPts; /* search under or equalvalent to the VidCapPts */
                SvcWrap_strcpy(pHdlr->FileName, sizeof(pHdlr->FileName), pVidInfo->pFileName);
                RetVal = AmbaKAL_MsgQueueSend(&LoggeQuetId, &EventInfo, AMBA_KAL_WAIT_FOREVER);
                if (RetVal != SVC_OK) {
                    SvcLog_NG(SVC_LOG_CV_LOGGER, "Ctrl sends FLUSH_PTS_AND_CLOSE error(0x%x)", RetVal, 0U);
                }
            }
            break;
        default:
            SvcLog_NG(SVC_LOG_CV_LOGGER, "SvcCvLogger_Ctrl: invalid CtrlType", 0U, 0U);
            RetVal = SVC_NG;
            break;
        }

        CvLogger_MutexGive(&pHdlr->ApiMutex);
    } else {
        SvcLog_NG(SVC_LOG_CV_LOGGER, "SvcCvLogger_Ctrl: invalid info", 0U, 0U);
        RetVal = SVC_NG;
    }

    AmbaMisra_TouchUnused(pInfo);

    return RetVal;
}

static void* CvLogger_TaskEntry(void* EntryArg)
{
    UINT32 RetVal = SVC_OK;
    SVC_CV_LOGGER_EVENT_s EventInfo;
    SVC_CV_LOGGER_HDLR_s *pHdlr;
    UINT32 BufType, IdxBufType;
    UINT8 IssueFlush = 0U, IssueFileClose = 0U, IssueFileOpen = 0U;
    UINT32 TargetBuf = 0U, FlushSize = 0U, TargetPTS = 0U;
    const SVC_CV_LOGGER_BUFFER_s *pTrunkBuf, *pIndexBuf;
    UINT32 FlushTh0, FlushTh1;
    ULONG BufSpace, IdxSpace;

    AmbaMisra_TouchUnused(EntryArg);

    SvcLog_DBG(SVC_LOG_CV_LOGGER, "CvLogger_TaskEntry[%d] start", 0U, 0U);

    while (RetVal == SVC_OK) {
        RetVal = AmbaKAL_MsgQueueReceive(&LoggeQuetId, &EventInfo, AMBA_KAL_WAIT_FOREVER);

        if (RetVal == SVC_OK) {
            pHdlr = EventInfo.pHdlr;
            CvLogger_Dbg("[CV_LGR%u] Recv event(0x%x)", pHdlr->ID, EventInfo.Event, 0U, 0U, 0U);

            BufType    = SVC_CV_LOGGER_LOG_TRUNK_BUFFER;
            IdxBufType = SVC_CV_LOGGER_LOG_INDEX_BUFFER;
            switch (EventInfo.Event) {
            case LOGGER_EVENT_FLUSH_BUFFER_TO_FILE:
                /* Flush partial Log trunks */
                pTrunkBuf = &pHdlr->Buf[SVC_CV_LOGGER_LOG_TRUNK_BUFFER];
                pIndexBuf = &pHdlr->Buf[SVC_CV_LOGGER_LOG_INDEX_BUFFER];
                CvLogger_BufferSpaceGet(pTrunkBuf->Base, pTrunkBuf->End, pTrunkBuf->Wp, pTrunkBuf->Rp, &BufSpace);
                FlushTh0 = pHdlr->LogTrunkBufTh;
                CvLogger_BufferSpaceGet(pIndexBuf->Base, pIndexBuf->End, pIndexBuf->Wp, pIndexBuf->Rp, &IdxSpace);
                FlushTh1 = (pIndexBuf->Size >> 1U);

                IssueFlush = 1U;
                if (BufSpace < FlushTh0) {
                    TargetBuf  = LOGGER_FLUSH_TARGET_TRUNK;
                    FlushSize  = (pHdlr->Buf[SVC_CV_LOGGER_LOG_TRUNK_BUFFER].Size >> 2U);
                } else if (IdxSpace < FlushTh1) {
                    TargetBuf  = LOGGER_FLUSH_TARGET_INDEX;
                    FlushSize  = (pHdlr->Buf[SVC_CV_LOGGER_LOG_INDEX_BUFFER].Size >> 2U);
                } else {
                    IssueFlush = 0U;
                }
                break;
            case LOGGER_EVENT_FLUSH_AND_CLOSE:
                /* Flush all Log trunks */
                TargetBuf      = LOGGER_FLUSH_TARGET_INDEX;
                FlushSize      = 0xFFFFFFFFU;
                IssueFlush     = 1U;
                IssueFileClose = 1U;
                break;
            case LOGGER_EVENT_FLUSH_PTS_AND_OPEN:
                /* Flush Log trunks by PTS */
                TargetBuf      = LOGGER_FLUSH_TARGET_INDEX;
                IssueFlush     = 2U;
                TargetPTS      = EventInfo.Param[0];
                IssueFileClose = 1U;
                IssueFileOpen  = 1U;
                break;
            default:
                IssueFlush     = 0U;
                IssueFileClose = 0U;
                IssueFileOpen  = 0U;
                SvcLog_NG(SVC_LOG_CV_LOGGER, "CvLogger_TaskEntry: Event(0x%x)", EventInfo.Event, 0U);
                break;
            }

            /* Flush file head trunks to beginning of file */
            if (pHdlr->HeadFlushed == 0U) {
                pHdlr->HeadFlushed = 1U;
                RetVal = CvLogger_BufferFlushBySize(&pHdlr->Buf[SVC_CV_LOGGER_FHEAD_INDEX_BUFFER],
                                                    &pHdlr->Buf[SVC_CV_LOGGER_FHEAD_TRUNK_BUFFER],
                                                    &pHdlr->FileDesc,
                                                    LOGGER_FLUSH_TARGET_INDEX,
                                                    0xFFFFFFFFU);
                if (RetVal != SVC_OK) {
                    SvcLog_NG(SVC_LOG_CV_LOGGER, "Flush file head trunk failed", 0U, 0U);
                }
            }

            /* Flush data to file */
            if (IssueFlush > 0U) {
                if (IssueFlush == 1U) {
                    RetVal = CvLogger_BufferFlushBySize(&pHdlr->Buf[IdxBufType],
                                                        &pHdlr->Buf[BufType],
                                                        &pHdlr->FileDesc,
                                                        TargetBuf, FlushSize);
                    if (RetVal != SVC_OK) {
                        SvcLog_NG(SVC_LOG_CV_LOGGER, "Flush Buf[%d, %d] failed", IdxBufType, BufType);
                    }
                } else if (IssueFlush == 2U) {
                    RetVal = CvLogger_BufferFlushByPTS(&pHdlr->Buf[IdxBufType],
                                                       &pHdlr->Buf[BufType],
                                                       &pHdlr->FileDesc,
                                                       TargetPTS);
                    if (RetVal != SVC_OK) {
                        SvcLog_NG(SVC_LOG_CV_LOGGER, "Flush Buf[%d, %d] failed", IdxBufType, BufType);
                    }
                } else {
                    /* Do nothing */
                }
                IssueFlush = 0U;
                pHdlr->FlushReq = 0U;
            }

            /* Close file */
            if (IssueFileClose == 1U) {
                RetVal = FsWrap_FileClose(&pHdlr->FileDesc);
                if (RetVal != SVC_OK) {
                    SvcLog_NG(SVC_LOG_CV_LOGGER, "FileClose error(0x%x)", RetVal, 0U);
                }

                RetVal = AmbaKAL_EventFlagSet(&pHdlr->EventFlag, LOGGER_FLAG_FILE_CLOSED);
                if (RetVal != SVC_OK) {
                    SvcLog_NG(SVC_LOG_CV_LOGGER, "Set FILE_CLOSED error(0x%x)", RetVal, 0U);
                }
                IssueFileClose = 0U;
            }

            /* Open new file */
            if (IssueFileOpen == 1U) {
                RetVal = FsWrap_FileOpen(pHdlr->FileName, "w", 1U, &pHdlr->FileDesc);
                AmbaPrint_PrintStr5("[CV_LGR] Create %s", pHdlr->FileName, NULL, NULL, NULL, NULL);
                if (RetVal == SVC_OK) {
                    pHdlr->HeadFlushed = 0U;
                } else {
                    SvcLog_NG(SVC_LOG_CV_LOGGER, "FileOpen error(0x%x)", RetVal, 0U);
                }
                IssueFileOpen = 0U;
            }

            CvLogger_Dbg("[CV_LGR%u] Event(0x%x) done", pHdlr->ID, EventInfo.Event, 0U, 0U, 0U);
        } else {
            SvcLog_NG(SVC_LOG_CV_LOGGER, "MsgQueueReceive error(0x%x)", RetVal, 0U);
        }
    }

    SvcLog_DBG(SVC_LOG_CV_LOGGER, "CvLogger_TaskEntry[%u]: exit", 0U, 0U);

    return NULL;
}

static UINT32 CvLogger_BufferGet(SVC_CV_LOGGER_BUFFER_s *pTrunkBuf, UINT32 ReqSize, SVC_CV_LOGGER_WRITE_BUFFER_s *pWrBuf)
{
    UINT32 RetVal = SVC_OK;
    ULONG  Wp, Rp;
    ULONG  BufBase, BufEnd;

    /* Get buffer status */
    Wp = pTrunkBuf->Wp;
    Rp = pTrunkBuf->Rp;
    BufBase = pTrunkBuf->Base;
    BufEnd  = pTrunkBuf->End;

    /* Get a buffer region (ReqSize + 1) without being truncated */
    if (Wp >= Rp) {
        if ((BufEnd - Wp) > ReqSize) {
            pWrBuf->Addr = Wp;
            pWrBuf->Size = ReqSize;
            Wp = Wp + ReqSize;
        } else if ((Rp - BufBase) > ReqSize) {
            pWrBuf->Addr = BufBase;
            pWrBuf->Size = ReqSize;
            Wp = BufBase + ReqSize;
        } else {
            RetVal = SVC_NG;
        }
    } else { /* Wp < Rp */
        if ((Rp - Wp) > ReqSize) {
            pWrBuf->Addr = Wp;
            pWrBuf->Size = ReqSize;
            Wp = Wp + ReqSize;
        } else {
            RetVal = SVC_NG;
        }
    }

    if (RetVal == SVC_OK) {
        /* Update write point */
        pTrunkBuf->Wp = Wp;
    }

    AmbaMisra_TouchUnused(pTrunkBuf);

    return RetVal;
}

static UINT32 CvLogger_BufferUpdate(SVC_CV_LOGGER_BUFFER_s *pIndexBuf, SVC_CV_LOGGER_BUFFER_s *pTrunkBuf, SVC_CV_LOGGER_WRITE_BUFFER_s *pWrBuf, UINT32 WroteSize)
{
    UINT32 RetVal = SVC_OK;
    ULONG  IndexWp;
    ULONG  BufBase, BufEnd;
    RF_LOG_HEADER_V0_s *pTrunkHdr;
    SVC_CV_LOGGER_INDEX_s *pIndex;

    AmbaMisra_TouchUnused(pTrunkBuf);

    /* Get buffer status */
    IndexWp = pIndexBuf->Wp;
    BufBase = pIndexBuf->Base;
    BufEnd  = pIndexBuf->End;

    if (WroteSize <=  pWrBuf->Size) {
        AmbaMisra_TypeCast(&pTrunkHdr, &pWrBuf->Addr);
        AmbaMisra_TypeCast(&pIndex, &IndexWp);

        pTrunkHdr->Magic = RF_DATA_FMT_HDR_MGC;
        if (pIndex->Valid == 0U) {
            pIndex->Valid     = 1U;
            pIndex->PTS       = pTrunkHdr->CapPTS;
            pIndex->TrunkPtr  = pWrBuf->Addr;
            pIndex->TrunkSize = WroteSize;

            IndexWp += sizeof(SVC_CV_LOGGER_INDEX_s);
            if (IndexWp >= BufEnd) {
                IndexWp = BufBase;
            }

            /* Update write point */
            if (IndexWp != pIndexBuf->Rp) {
                pIndexBuf->Wp = IndexWp;
                //CvLogger_Dbg("[CV_LGR] Index Wp 0x%x Rp 0x%x", pIndexBuf->Wp, pIndexBuf->Rp, 0U, 0U, 0U);
            } else {
                SvcLog_NG(SVC_LOG_CV_LOGGER, "BufferUpdate: index buffer full(1)", 0U, 0U);
                RetVal = SVC_NG;
            }
        } else {
            SvcLog_NG(SVC_LOG_CV_LOGGER, "BufferUpdate: index buffer full(0)", 0U, 0U);
            RetVal = SVC_NG;
        }
    } else {
        SvcLog_NG(SVC_LOG_CV_LOGGER, "BufferUpdate: invalid wrote size(%u)", WroteSize, 0U);
        RetVal = SVC_NG;
    }

    AmbaMisra_TouchUnused(pWrBuf);

    return RetVal;
}

static UINT32 CvLogger_BufferFlushBySize(SVC_CV_LOGGER_BUFFER_s *pIndexBuf, SVC_CV_LOGGER_BUFFER_s *pTrunkBuf, const SVC_CV_LOGGER_FILE_DESC_s *pFileDesc, UINT32 TargetBuf, UINT32 FlushSize)
{
    UINT32 RetVal = SVC_OK;
    UINT32 NumSuccess = 0U, TrunkSize, TotalFlush = 0U;
    ULONG  IndexWp, IndexRp, TrunkRp;
    ULONG  BufBase, BufEnd;
    SVC_CV_LOGGER_INDEX_s *pIndex;

    /* Get buffer status */
    IndexWp = pIndexBuf->Wp;
    IndexRp = pIndexBuf->Rp;
    BufBase = pIndexBuf->Base;
    BufEnd  = pIndexBuf->End;

    CvLogger_UlongDbg("[CV_LGR] FlushBySize index Wp 0x%x Rp 0x%x", IndexWp, IndexRp, 0U, 0U, 0U);

    if (IndexWp != IndexRp) {
        while (TotalFlush < FlushSize) {
            //CvLogger_Dbg("[CV_LGR] Flush Rp 0x%x Target %d Total %d", IndexRp, FlushSize, TotalFlush, 0U, 0U);

            AmbaMisra_TypeCast(&pIndex, &IndexRp);
            if (pIndex->Valid == 1U) {
                TrunkSize = pIndex->TrunkSize;
                if (SVC_OK != CvLogger_FileWrite(pIndex->TrunkPtr,
                                                 TrunkSize,
                                                 pTrunkBuf->Base,
                                                 pTrunkBuf->End,
                                                 pFileDesc,
                                                 &NumSuccess)) {
                    SvcLog_NG(SVC_LOG_CV_LOGGER, "File write failed", 0U, 0U);
                }
                if (TargetBuf == LOGGER_FLUSH_TARGET_TRUNK) {
                    TotalFlush += TrunkSize;
                } else {
                    TotalFlush += (UINT32)sizeof(SVC_CV_LOGGER_INDEX_s);
                }

                /* Release the index */
                pIndex->Valid = 0U;

                /* Update trunk Rp */
                TrunkRp = pIndex->TrunkPtr + (ULONG)TrunkSize;
                if (TrunkRp >= pTrunkBuf->End) {
                    SvcLog_NG(SVC_LOG_CV_LOGGER, "Rp over buf end", 0U, 0U);
                }
                pTrunkBuf->Rp = TrunkRp;
            } else {
                CvLogger_UlongDbg("[CV_LGR] Skip index(0x%x)", IndexRp, 0U, 0U, 0U, 0U);
            }

            /* Update index Rp */
            IndexRp += sizeof(SVC_CV_LOGGER_INDEX_s);
            if (IndexRp >= BufEnd) {
                IndexRp = BufBase;
            }
            pIndexBuf->Rp = IndexRp;

            /* End of data */
            if (IndexRp == IndexWp) {
                break;
            }
        }
    }

    return RetVal;
}

static UINT32 CvLogger_BufferFlushByPTS(SVC_CV_LOGGER_BUFFER_s *pIndexBuf, SVC_CV_LOGGER_BUFFER_s *pTrunkBuf, const SVC_CV_LOGGER_FILE_DESC_s *pFileDesc, UINT32 TargetPTS)
{
    UINT32 RetVal = SVC_OK;
    UINT32 NumSuccess = 0U, TrunkSize;
    ULONG  IndexWp, IndexRp, TrunkRp;
    ULONG  BufBase, BufEnd, Run;
    SVC_CV_LOGGER_INDEX_s *pIndex;
    UINT32 LastPTS = 0U;

    /* Get buffer status */
    IndexWp = pIndexBuf->Wp;
    IndexRp = pIndexBuf->Rp;
    BufBase = pIndexBuf->Base;
    BufEnd  = pIndexBuf->End;
    AmbaMisra_TypeCast(&pIndex, &IndexRp);

    CvLogger_UlongDbg("[CV_LGR] FlushByPTS index Wp 0x%x Rp 0x%x TargetPTS %d", IndexWp, IndexRp, (ULONG)TargetPTS, 0U, 0U);

    if (IndexWp != IndexRp) {
        /* Flush if trunks PTS are under or equal to TargetPTS */
        Run = 1U;
        while (Run == 1U) {
            AmbaMisra_TypeCast(&pIndex, &IndexRp);
            if (pIndex->Valid == 1U) {
                if (0U == CvLogger_PTSCompare(pIndex->PTS, TargetPTS)) {
                    TrunkSize = pIndex->TrunkSize;
                    if (SVC_OK != CvLogger_FileWrite(pIndex->TrunkPtr,
                                                     TrunkSize,
                                                     pTrunkBuf->Base,
                                                     pTrunkBuf->End,
                                                     pFileDesc,
                                                     &NumSuccess)) {
                        SvcLog_NG(SVC_LOG_CV_LOGGER, "File write failed", 0U, 0U);
                    }

                    /* Release the index */
                    pIndex->Valid = 0U;

                    /* Update trunk Rp */
                    TrunkRp = pIndex->TrunkPtr + (ULONG)TrunkSize;
                    if (TrunkRp >= pTrunkBuf->End) {
                        SvcLog_NG(SVC_LOG_CV_LOGGER, "Rp over buf end", 0U, 0U);
                    }
                    pTrunkBuf->Rp = TrunkRp;
                    LastPTS = pIndex->PTS;
                } else {
                    Run = 0U;
                    break;
                }
            } else {
                CvLogger_UlongDbg("[CV_LGR] Skip index(0x%x)", IndexRp, 0U, 0U, 0U, 0U);
            }

            /* Update index Rp */
            IndexRp += sizeof(SVC_CV_LOGGER_INDEX_s);
            if (IndexRp >= BufEnd) {
                IndexRp = BufBase;
            }
            pIndexBuf->Rp = IndexRp;

            /* End of data */
            if (IndexRp == IndexWp) {
                Run = 0U;
            }
        }
    }

    if (IndexWp != IndexRp) {
        /* Expolre if any later trunks of older PTS. */
        AmbaMisra_TypeCast(&pIndex, &IndexRp);
        Run = 10U;
        while (Run > 0U) {
            Run--;

            AmbaMisra_TypeCast(&pIndex, &IndexRp);
            if (pIndex->Valid == 1U) {
                if (0U == CvLogger_PTSCompare(pIndex->PTS, TargetPTS)) {
                    TrunkSize = pIndex->TrunkSize;
                    if (SVC_OK != CvLogger_FileWrite(pIndex->TrunkPtr,
                                                     TrunkSize,
                                                     pTrunkBuf->Base,
                                                     pTrunkBuf->End,
                                                     pFileDesc,
                                                     &NumSuccess)) {
                        SvcLog_NG(SVC_LOG_CV_LOGGER, "File write failed", 0U, 0U);
                    }
                    LastPTS = pIndex->PTS;
                }
            } else {
                CvLogger_UlongDbg("[CV_LGR] Skip index(0x%x)", IndexRp, 0U, 0U, 0U, 0U);
            }

            /* Update index Rp */
            IndexRp += sizeof(SVC_CV_LOGGER_INDEX_s);
            if (IndexRp >= BufEnd) {
                IndexRp = BufBase;
            }

            /* End of data */
            if (IndexRp == IndexWp) {
                Run = 0U;
            }
        }
    }

    CvLogger_Dbg("[CV_LGR] FlushByPTS done. Last PTS = %d", LastPTS, 0U, 0U, 0U, 0U);

    return RetVal;
}

static UINT32 CvLogger_BufferSanityCheck(const SVC_CV_LOGGER_BUFFER_s *pBuf)
{
    UINT32 RetVal = SVC_NG;

    if ((pBuf->Wp >= pBuf->Base) && (pBuf->Wp < pBuf->End) &&
        (pBuf->Rp >= pBuf->Base) && (pBuf->Rp < pBuf->End)) {
        RetVal = SVC_OK;
    }

    return RetVal;
}

static void CvLogger_BufferSpaceGet(ULONG BufBase, ULONG BufEnd, ULONG Wp, ULONG Rp, ULONG *pSpace)
{
    if (Wp >= Rp) {
        *pSpace = (BufEnd - Wp) + (Rp - BufBase);
    } else { /* Wp < Rp */
        *pSpace = Rp - Wp;
    }
}

#if 0
static void CvLogger_BufferDataSizeGet(ULONG BufBase, ULONG BufEnd, ULONG Wp, ULONG Rp, UINT32 *pDataSize)
{
    if (Wp >= Rp) {
        *pDataSize = Wp - Rp;
    } else { /* Wp < Rp */
        *pDataSize = (Wp - BufBase) + (BufEnd - Rp);
    }
}
#endif

static UINT32 CvLogger_FileWrite(ULONG StartAddr, ULONG Size, ULONG BufBase, ULONG BufEnd, const SVC_CV_LOGGER_FILE_DESC_s *pFileDesc, UINT32 *pWrSize)
{
    UINT32 RetVal = SVC_OK;
    UINT32 NumSuccess0 = 0U, NumSuccess1 = 0U;
    ULONG WrSize;
    void   *pStartAddr;

    if ((StartAddr + Size) < BufEnd) {
        AmbaMisra_TypeCast(&pStartAddr, &StartAddr);
        RetVal = FsWrap_FileWrite(pStartAddr, 1U, (UINT32)Size, pFileDesc, &NumSuccess0);
    } else {
        AmbaMisra_TypeCast(&pStartAddr, &StartAddr);
        WrSize = BufEnd - StartAddr;
        RetVal = FsWrap_FileWrite(pStartAddr, 1U, (UINT32)WrSize, pFileDesc, &NumSuccess0);

        AmbaMisra_TypeCast(&pStartAddr, &BufBase);
        WrSize = Size - WrSize;
        RetVal |= FsWrap_FileWrite(pStartAddr, 1U, (UINT32)WrSize, pFileDesc, &NumSuccess1);
    }

    if (RetVal == SVC_OK) {
        *pWrSize = NumSuccess0 + NumSuccess1;
    }

    return RetVal;
}

/* Compare CurPTS with TargetPTS. */
/* 0 - CurPTS is under or equals to TargetPTS, 1 - CurPTS is over TargetPTS */
static UINT32 CvLogger_PTSCompare(UINT32 CurPTS, UINT32 TargetPTS)
{
    #define HALF_OF_U32     (0x80000000U)
    UINT32 RetVal = 0U;
    UINT32 Diff;

    if (TargetPTS > CurPTS) {
        Diff = TargetPTS - CurPTS;
        if (Diff > HALF_OF_U32) {
            /* CurPTS wraps around */
            RetVal = 1U;
        } else {
            RetVal = 0U;
        }
    } else if (TargetPTS < CurPTS) {
        Diff = CurPTS - TargetPTS;
        if (Diff > HALF_OF_U32) {
            /* TargetPTS wraps around */
            RetVal = 0U;
        } else {
            RetVal = 1U;
        }
    } else {
        RetVal = 0U;
    }

    return RetVal;
}

static UINT32 FsWrap_FileOpen(const char *pFileName, const char *pMode, UINT32 EnableAsync, SVC_CV_LOGGER_FILE_DESC_s *pFileDesc)
{
    UINT32 RetVal;

    RetVal = AmbaVFS_Open(pFileName, pMode, EnableAsync, &pFileDesc->VfsFile);

    return RetVal;
}

static UINT32 FsWrap_FileClose(const SVC_CV_LOGGER_FILE_DESC_s *pFileDesc)
{
    UINT32 RetVal;

    RetVal = AmbaVFS_Sync(&pFileDesc->VfsFile);
    if (RetVal == SVC_OK) {
        RetVal = AmbaVFS_Close(&pFileDesc->VfsFile);
    } else {
        SvcLog_NG(SVC_LOG_CV_LOGGER, "AmbaVFS_Sync fail RetVal %d", RetVal, 0U);
    }
    return RetVal;
}

static UINT32 FsWrap_FileWrite(void *pBuf, UINT32 Size, UINT32 Count, const SVC_CV_LOGGER_FILE_DESC_s *pFileDesc, UINT32 *pNumSuccess)
{
    UINT32 RetVal;

    RetVal = AmbaVFS_Write(pBuf, Size, Count, &pFileDesc->VfsFile, pNumSuccess);

    return RetVal;
}
/**
* Initialize CV logger utility
* @param [in] pBufIn input buffer
* @param [in] Width input width
* @param [in] Height input height
* @param [in] Pitch input pitch
* @param [in] pBufOut output buffer
* @param [in] BufOutSize outbuffer size
* @param [in] UsedLength encode length
* @return 0 : encode ok 1 : encode fail
*/
UINT32 SvcCvLogger_RunLengthEncode(const UINT8* pBufIn, UINT32 Width, UINT32 Height, UINT32 Pitch, UINT8* pBufOut, UINT32 BufOutSize, UINT32* UsedLength) {
    UINT32 Rval = 0U;
    UINT32 size;

    Rval = AmbaWrap_memset(pBufOut, 0, BufOutSize);
    if (Rval != SVC_OK) {
        SvcLog_NG(SVC_LOG_CV_LOGGER, "AmbaWrap_memset fail %d", Rval, 0U);
    }

    size = 0U;
    for (UINT32 j = 0U; j < Height; j++) {
        UINT32 idx = j*Pitch;
        for (UINT32 i = 0U; i < Width; i++) {
            if (size == 0U) {
                size = 2U;
                pBufOut[size - 2U] = pBufIn[idx];
                pBufOut[size - 1U] = 1U;
            } else if (size >= BufOutSize) {
                AmbaPrint_PrintUInt5("Warning! RunLengEncoded size is larger than buffer size (%d)\nForce stop encode now!", BufOutSize, 0U, 0U, 0U, 0U);
                Rval = 1U;
                break;
            } else {
                if (pBufOut[size - 2U] == pBufIn[idx]) {
                    if (pBufOut[size - 1U] < 0xFFU) {
                        pBufOut[size - 1U] = pBufOut[size - 1U] + 1U;
                    } else {
                        size += 2U;
                        pBufOut[size - 2U] = pBufIn[idx];
                        pBufOut[size - 1U] = 1U;
                    }
                } else {
                    size += 2U;
                    pBufOut[size - 2U] = pBufIn[idx];
                    pBufOut[size - 1U] = 1U;
                }
            }

            idx++;
        }

        if (Rval == 1U) {
            break;
        }
    }

    *UsedLength = size;

    return Rval;
}

