/**
 *  @file SvcVinBufMonitorTask.c
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
 *  @details svc vin buffer monitor task
 *
 */

#include "AmbaTypes.h"
#include "AmbaDef.h"
#include "AmbaShell.h"
#include "AmbaKAL.h"
#include "AmbaSYS.h"
#include "AmbaFS.h"

#include "AmbaDSP.h"
#include "AmbaDSP_ImageFilter.h"
#include "AmbaDSP_ImageUtility.h"
#include "AmbaDSP_Capability.h"
#include "AmbaDSP_Liveview.h"
#include "AmbaDSP_VOUT.h"
#include "AmbaDSP_VideoEnc.h"
#include "AmbaDSP_Event.h"
#include "AmbaHDMI_Def.h"

#include "AmbaSensor.h"
#include "AmbaFPD.h"
#include "AmbaSvcWrap.h"
#include "inc/SvcVinBufMonitorTask.h"

#include "SvcErrCode.h"
#include "SvcLog.h"
#include "SvcMem.h"
#include "SvcBuffer.h"
#include "SvcIK.h"
#include "SvcLiveview.h"
#include "SvcDisplay.h"
#include "SvcEnc.h"
#include "SvcVinSrc.h"
#include "SvcVoutSrc.h"
#include "SvcInfoPack.h"
#include "SvcTaskList.h"
#include "SvcSysStat.h"
#include "SvcAppStat.h"
#include "SvcResCfg.h"
#include "SvcWrap.h"

#include "SvcBufMap.h"

#define SVC_VIN_BUF_MON_INIT       (0x00000001U)
#define SVC_VIN_BUF_MON_DBG_LOG    (0x00000004U)
#define SVC_VIN_BUF_MON_MEM_LOG    (0x00000008U)

typedef struct {
    UINT32 VinID;
    UINT32 FreeNum;
    UINT32 TotalNum;
    UINT64 CapSeqCnt;
    UINT64 CapPts;
} SVC_VIN_BUF_MON_MSG_s;

typedef struct {
#define SVC_VIN_BUF_MON_STATE_ACTIVE   (0x1U)
    UINT32                      State[AMBA_DSP_MAX_VIN_NUM];
    UINT32                      VinSelectBits;
    UINT32                      EffectVinNum[AMBA_DSP_MAX_VIN_NUM];
    UINT32                      EffectVinBits[AMBA_DSP_MAX_VIN_NUM];    // Dependency vin bits if effect on
    UINT32                      EffectMaster[AMBA_DSP_MAX_VIN_NUM];
    UINT16                      ViewZoneNum[AMBA_DSP_MAX_VIN_NUM];
    UINT16                      ViewZoneIDs[AMBA_DSP_MAX_VIN_NUM][AMBA_DSP_MAX_VIEWZONE_NUM];
    UINT32                      ViewZoneVinID[AMBA_DSP_MAX_VIEWZONE_NUM];
    char                        Name[32];
#define SVC_VIN_BUF_MON_FLAG_DATA_RDY  (0x1U)
    AMBA_KAL_EVENT_FLAG_t       Flag;
    SVC_TASK_CTRL_s             Ctrl;
    UINT8                       Stack[0xF000U];
    AMBA_KAL_MSG_QUEUE_t        Que;
    SVC_VIN_BUF_MON_MSG_s          QueBuf[AMBA_DSP_MAX_VIN_NUM * 3U];
} SVC_VIN_BUF_MON_CTRL_s;

typedef struct {
#define SVC_VIN_BUF_MON_MEM_LOG_CFG     (0x1U)
#define SVC_VIN_BUF_MON_MEM_FLG_ON      (0x2U)
#define SVC_VIN_BUF_MON_MEM_FLG_WRITE   (0x10U)
#define SVC_VIN_BUF_MON_MEM_FLG_FILE    (0x20U)
#define SVC_VIN_BUF_MON_MEM_FLG_SHOW    (0x40U)
    UINT32 State;

    UINT8 *pMemBase;
    UINT8 *pMemLimit;
    UINT8 *pMemWrite;
    UINT32 MemSize;
    UINT32 RcvSize;
    UINT32 WriteProcCnt;

    AMBA_FS_FILE *pFilePoint;
    AMBA_SHELL_PRINT_f PrintFunc;
} SVC_VIN_BUF_MON_MEM_LOG_CTRL_s;

static UINT32 SvcVinBufMonTask_WaitWriteDone(INT32 TimeOut);
static UINT32 SvcVinBufMonTask_WaitStateDone(INT32 TimeOut, UINT32 ChkState);
static UINT32 SvcVinBufMonTask_GetDigitalNum(UINT32 Val);
static UINT32 SvcVinBufMonTask_MemDiff(const void *pPoint1, const void *pPoint2);
static void   SvcVinBufMonTask_MemLogProc(UINT32 LastNLine);

#define SVC_VIN_BUF_MON_MEM_LOG_SYS_TIME_ALIGN      (10U)
static UINT32 SvcVinBufMonTask_MemLogSysTime(char *pBuf, UINT32 BufSize);
#define SVC_VIN_BUF_MON_MEM_LOG_ORC_TIME_ALIGN      (10U)
static UINT32 SvcVinBufMonTask_MemLogOrcTime(char *pBuf, UINT32 BufSize);

static void   SvcVinBufMonTask_MemLogCfg(UINT8 *pBuf, UINT32 BufSize);
static void   SvcVinBufMonTask_MemLogRst(void);
static void   SvcVinBufMonTask_MemLogEna(UINT32 Enable);
static void   SvcVinBufMonTask_MemLogInfo(void);
       void   SvcVinBufMonTask_MemLogWrite(const char *pStrBuf, UINT32 StrLen);
static void   SvcVinBufMonTask_MemLogDump(const char *pFileName);
static void   SvcVinBufMonTask_MemLogShow(UINT32 LastNLine);
static void   SvcVinBufMonTask_MemLog(UINT32 PrefixOn, const char *pStrBuf, UINT32 StrLen);
       void   SvcVinBufMonTask_ShellEntry(UINT32 ArgCount, char * const *pArgVector, AMBA_SHELL_PRINT_f PrintFunc);

static SVC_VIN_BUF_MON_MEM_LOG_CTRL_s SvcVinBufMomMemLogCtrl GNU_SECTION_NOZEROINIT;

static void   SvcVinBufMonTask_Info(void);
static void*  SvcVinBufMonTask_TaskEntry(void* EntryArg);
static UINT32 SvcVinBufMonTask_RawRdyHdlr(const void *pEventData);
static UINT32 SvcVinBufMonTask_YuvRdyHdlr(const void *pEventData);
static void   SvcVinBufMonTask_VinBufProc(UINT32 VinID);
static void   SvcVinBufMonTask_ErrHdlr(const char *pCaller, UINT32 CodeLine, UINT32 ErrCode);

#define SVC_LOG_VIN_BUF_MON "VBMON"
#define SVC_VIN_BUF_MON_ERR_HDLR SvcVinBufMonTask_ErrHdlr(__func__, __LINE__, ErrCode);

static SVC_VIN_BUF_MON_CTRL_s SvcVinBufMonCtrl GNU_SECTION_NOZEROINIT;
static UINT32              SvcVinBufMonFlag = 0U; //SVC_VIN_BUF_MON_DBG_LOG;

static UINT32 SvcVinBufMonTask_WaitWriteDone(INT32 TimeOut)
{
    UINT32 RetVal = SVC_NG, ErrCode;
    INT32 CurTimeOut = TimeOut;
    const UINT8 *pPreWp = SvcVinBufMomMemLogCtrl.pMemWrite;

    do {
        ErrCode = AmbaKAL_TaskSleep(10U); SVC_VIN_BUF_MON_ERR_HDLR
        if ((pPreWp == SvcVinBufMomMemLogCtrl.pMemWrite)
          &&((SvcVinBufMomMemLogCtrl.State & SVC_VIN_BUF_MON_MEM_FLG_WRITE) == 0U)) {
            RetVal = SVC_OK;
            break;
        }
        CurTimeOut -= 10;
    } while (CurTimeOut > 0);

    return RetVal;
}

static UINT32 SvcVinBufMonTask_WaitStateDone(INT32 TimeOut, UINT32 ChkState)
{
    UINT32 RetVal = SVC_NG, ErrCode;
    INT32 CurTimeOut = TimeOut;

    if ((SvcVinBufMomMemLogCtrl.State & ChkState) > 0U) {
        do {
            ErrCode = AmbaKAL_TaskSleep(100U); SVC_VIN_BUF_MON_ERR_HDLR
            if ((SvcVinBufMomMemLogCtrl.State & ChkState) == 0U) {
                RetVal = SVC_OK;
                break;
            }
            CurTimeOut -= 100;
        } while (CurTimeOut > 0);
    }

    return RetVal;
}

static UINT32 SvcVinBufMonTask_GetDigitalNum(UINT32 Val)
{
    UINT32 RetVal = 0U;

    // compare 4 times

    if (Val > 99999U) {                 // 6, 7, 8, 9, 10
        if (Val > 99999999U) {          // 9, 10
            if (Val > 999999999U) {     // 10
                RetVal = 10U;
            } else {                    // 9
                RetVal = 9U;
            }
        } else {                        // 6, 7, 8
            if (Val > 9999999U) {
                RetVal = 8U;
            } else if (Val > 999999U) { // 7
                RetVal = 7U;
            } else {                    // 6
                RetVal = 6U;
            }
        }
    } else {                            // 1, 2, 3, 4, 5
        if (Val > 999U) {               // 4, 5
            if (Val > 9999U) {
                RetVal = 5U;
            } else {
                RetVal = 4U;
            }
        } else {                        // 1, 2, 3
            if (Val > 99U) {
                RetVal = 3U;
            } else if (Val > 9U) {
                RetVal = 2U;
            } else {
                RetVal = 1U;
            }
        }
    }

    return RetVal;
}

static UINT32 SvcVinBufMonTask_MemDiff(const void *pPoint1, const void *pPoint2)
{
    UINT32 CurDiff = 0U;
    ULONG Point1Addr = 0U, Point2Addr = 0U;

    AmbaMisra_TypeCast(&(Point1Addr), &(pPoint1));
    AmbaMisra_TypeCast(&(Point2Addr), &(pPoint2));

    if (Point1Addr >= Point2Addr) {
        CurDiff = (UINT32)(Point1Addr - Point2Addr);
    } else {
        CurDiff = (UINT32)(Point2Addr - Point1Addr);
    }

    return CurDiff;
}

static void SvcVinBufMonTask_MemLogCfg(UINT8 *pBuf, UINT32 BufSize)
{
    UINT32 ErrCode;
    SVC_VIN_BUF_MON_MEM_LOG_CTRL_s *pLogCtrl = &SvcVinBufMomMemLogCtrl;

    // SvcVinBufMomMemLogCtrl memory does not initial.
    if ((SvcVinBufMonFlag & SVC_VIN_BUF_MON_MEM_LOG) == 0U) {
        AmbaSvcWrap_MisraMemset(&SvcVinBufMomMemLogCtrl, 0, sizeof(SvcVinBufMomMemLogCtrl));
        SvcVinBufMonFlag |= SVC_VIN_BUF_MON_MEM_LOG;
    }

    // Vin buffer monitor memory log has been configured. Re-configure it.
    if ((pLogCtrl->State & SVC_VIN_BUF_MON_MEM_LOG_CFG) > 0U) {
        UINT32 ChkState = SVC_VIN_BUF_MON_MEM_FLG_FILE | SVC_VIN_BUF_MON_MEM_FLG_SHOW;

        if ((pLogCtrl->State & ChkState) > 0U) {
            // Wait some time to make sure state idle.
            ErrCode = SvcVinBufMonTask_WaitStateDone(30000, ChkState); SVC_VIN_BUF_MON_ERR_HDLR
        }

        // Disable memory log
        pLogCtrl->State &= ~SVC_VIN_BUF_MON_MEM_FLG_ON;

        // Wait some time to make sure buffer idle
        ErrCode = SvcVinBufMonTask_WaitWriteDone(1000); SVC_VIN_BUF_MON_ERR_HDLR
    }

    ErrCode = SVC_OK;

    // configure by current insert buffer
    if (pBuf != NULL) {
        pLogCtrl->pMemBase  = pBuf;
        pLogCtrl->pMemWrite = pLogCtrl->pMemBase;
        pLogCtrl->pMemLimit = &(pLogCtrl->pMemBase[BufSize]);
        pLogCtrl->MemSize   = BufSize;
        pLogCtrl->RcvSize   = 0U;
    } else {
        // For debug, using ituner buffer to configure memory log buffer.
#ifdef FMEM_ID_ITUNER
        ULONG CurBufBase = 0U;
        UINT32 CurBufSize = 0U;
        if (0U != SvcBuffer_Request(SVC_BUFFER_FIXED, FMEM_ID_ITUNER, &CurBufBase, &CurBufSize)) {
            CurBufBase = 0U;
            CurBufSize = 0U;
        }

        if ((CurBufBase == 0U) || (CurBufSize == 0U)) {
            ErrCode = SVC_NG;
            SvcLog_NG(SVC_LOG_VIN_BUF_MON, "Fail to configure memory base - configure buffer should not null", 0U, 0U);
        } else {
            AmbaMisra_TypeCast(&(pLogCtrl->pMemBase), &(CurBufBase));
            if (pLogCtrl->pMemBase == NULL) {
                ErrCode = SVC_NG;
                SvcLog_NG(SVC_LOG_VIN_BUF_MON, "Fail to configure memory base - get invalid buffer address", 0U, 0U);
            } else {
                pLogCtrl->pMemWrite = pLogCtrl->pMemBase;
                pLogCtrl->pMemLimit = &(pLogCtrl->pMemBase[CurBufSize]);
                pLogCtrl->MemSize   = CurBufSize;
                pLogCtrl->RcvSize   = 0U;
            }
        }
#else
        pLogCtrl->pMemBase  = NULL;
        pLogCtrl->pMemWrite = NULL;
        pLogCtrl->pMemLimit = NULL;
        pLogCtrl->MemSize   = 0U;
        pLogCtrl->RcvSize   = 0U;
        ErrCode = SVC_NG;
#endif
    }

    if (ErrCode == SVC_OK) {
        SVC_WRAP_PRINT "Config mem log done. Base(%p), Limit(%p), Write(%p)"
            SVC_PRN_ARG_S SVC_LOG_VIN_BUF_MON
            SVC_PRN_ARG_CPOINT pLogCtrl->pMemBase  SVC_PRN_ARG_POST
            SVC_PRN_ARG_CPOINT pLogCtrl->pMemLimit SVC_PRN_ARG_POST
            SVC_PRN_ARG_CPOINT pLogCtrl->pMemWrite SVC_PRN_ARG_POST
            SVC_PRN_ARG_E

        pLogCtrl->State |= SVC_VIN_BUF_MON_MEM_LOG_CFG;
    } else {
        pLogCtrl->State &= ~SVC_VIN_BUF_MON_MEM_LOG_CFG;
    }
}

static void SvcVinBufMonTask_MemLogRst(void)
{
    UINT32 ErrCode;
    SVC_VIN_BUF_MON_MEM_LOG_CTRL_s *pLogCtrl = &SvcVinBufMomMemLogCtrl;

    if ((SvcVinBufMonFlag & SVC_VIN_BUF_MON_MEM_LOG) == 0U) {
        SvcLog_NG(SVC_LOG_VIN_BUF_MON, "Fail to reset mem log - configure module first", 0U, 0U);
    } else if ((pLogCtrl->State & SVC_VIN_BUF_MON_MEM_LOG_CFG) == 0U) {
        SvcLog_NG(SVC_LOG_VIN_BUF_MON, "Fail to reset mem log - configure module first.", 0U, 0U);
    } else if ((pLogCtrl->State & (SVC_VIN_BUF_MON_MEM_FLG_FILE | SVC_VIN_BUF_MON_MEM_FLG_SHOW)) > 0U) {
        SVC_WRAP_PRINT "Fail to reset mem log - invalid state(0x%X)"
            SVC_PRN_ARG_S SVC_LOG_VIN_BUF_MON
            SVC_PRN_ARG_UINT32 pLogCtrl->State SVC_PRN_ARG_POST
            SVC_PRN_ARG_E
    } else {
        UINT32 OriState = pLogCtrl->State & SVC_VIN_BUF_MON_MEM_FLG_ON;

        // Disable memory log
        pLogCtrl->State &= ~SVC_VIN_BUF_MON_MEM_FLG_ON;

        // Wait some time to make sure buffer idle
        ErrCode = SvcVinBufMonTask_WaitWriteDone(1000); SVC_VIN_BUF_MON_ERR_HDLR

        // Reset buffer setting
        pLogCtrl->pMemWrite = pLogCtrl->pMemBase;
        pLogCtrl->RcvSize   = 0U;

        // Resume memory log state
        pLogCtrl->State |= OriState;

        SVC_WRAP_PRINT "Reset mem log. Base(%p), Limit(%p), Write(%p)"
            SVC_PRN_ARG_S SVC_LOG_VIN_BUF_MON
            SVC_PRN_ARG_CPOINT pLogCtrl->pMemBase  SVC_PRN_ARG_POST
            SVC_PRN_ARG_CPOINT pLogCtrl->pMemLimit SVC_PRN_ARG_POST
            SVC_PRN_ARG_CPOINT pLogCtrl->pMemWrite SVC_PRN_ARG_POST
            SVC_PRN_ARG_E
    }
}

static void SvcVinBufMonTask_MemLogEna(UINT32 Enable)
{
    SVC_VIN_BUF_MON_MEM_LOG_CTRL_s *pLogCtrl = &SvcVinBufMomMemLogCtrl;

    if ((SvcVinBufMonFlag & SVC_VIN_BUF_MON_MEM_LOG) == 0U) {
        SvcLog_NG(SVC_LOG_VIN_BUF_MON, "Fail to reset mem log - configure module first", 0U, 0U);
    } else if ((pLogCtrl->State & SVC_VIN_BUF_MON_MEM_LOG_CFG) == 0U) {
        SvcLog_NG(SVC_LOG_VIN_BUF_MON, "Fail to reset mem log - configure module first.", 0U, 0U);
    } else {
        if (Enable > 0U) {
            pLogCtrl->State |= SVC_VIN_BUF_MON_MEM_FLG_ON;
        } else {
            pLogCtrl->State &= ~SVC_VIN_BUF_MON_MEM_FLG_ON;
        }

        SVC_WRAP_PRINT "%s mem log"
            SVC_PRN_ARG_S SVC_LOG_VIN_BUF_MON
            SVC_PRN_ARG_CSTR (((pLogCtrl->State & SVC_VIN_BUF_MON_MEM_FLG_ON) > 0U)?"Enable":"Disable") SVC_PRN_ARG_POST
            SVC_PRN_ARG_E
    }
}

static void SvcVinBufMonTask_MemLogInfo(void)
{
    const SVC_VIN_BUF_MON_MEM_LOG_CTRL_s *pLogCtrl = &SvcVinBufMomMemLogCtrl;

    if ((SvcVinBufMonFlag & SVC_VIN_BUF_MON_MEM_LOG) == 0U) {
        SvcLog_NG(SVC_LOG_VIN_BUF_MON, "Fail to reset mem log - configure module first", 0U, 0U);
    } else {
        SVC_WRAP_PRINT "%p / %p / %p, Size(%d), Rcv(%u), State(0x%X)"
            SVC_PRN_ARG_S SVC_LOG_VIN_BUF_MON
            SVC_PRN_ARG_CPOINT pLogCtrl->pMemBase  SVC_PRN_ARG_POST
            SVC_PRN_ARG_CPOINT pLogCtrl->pMemLimit SVC_PRN_ARG_POST
            SVC_PRN_ARG_CPOINT pLogCtrl->pMemWrite SVC_PRN_ARG_POST
            SVC_PRN_ARG_UINT32 pLogCtrl->MemSize   SVC_PRN_ARG_POST
            SVC_PRN_ARG_UINT32 pLogCtrl->RcvSize   SVC_PRN_ARG_POST
            SVC_PRN_ARG_UINT32 pLogCtrl->State     SVC_PRN_ARG_POST
            SVC_PRN_ARG_E
    }
}

static void SvcVinBufMonTask_MemLogProc(UINT32 LastNLine)
{
    UINT32 ErrCode;
    const SVC_VIN_BUF_MON_MEM_LOG_CTRL_s *pLogCtrl = &SvcVinBufMomMemLogCtrl;

    UINT32 CmpAddr1 = 0U, CmpAddr2 = 0U;
    AmbaMisra_TypeCast(&(CmpAddr1), &(pLogCtrl->pMemWrite));
    AmbaMisra_TypeCast(&(CmpAddr2), &(pLogCtrl->pMemLimit));

    if ((pLogCtrl->pMemBase == NULL)
      ||(pLogCtrl->pMemLimit == NULL)
      ||(pLogCtrl->pMemWrite == NULL)) {
        SvcLog_NG(SVC_LOG_VIN_BUF_MON, "Fail to proc mem log - invalid buffer", 0U, 0U);
    } else if (CmpAddr1 > CmpAddr2) {
        SvcLog_NG(SVC_LOG_VIN_BUF_MON, "Fail to proc mem log - invalid write pointer", 0U, 0U);
    } else if (pLogCtrl->RcvSize == 0U) {
        SvcLog_DBG(SVC_LOG_VIN_BUF_MON, "Buffer is empty", 0U, 0U);
    } else if (((pLogCtrl->State & SVC_VIN_BUF_MON_MEM_FLG_FILE) > 0U)
              &&((pLogCtrl->State & SVC_VIN_BUF_MON_MEM_FLG_SHOW) == 0U)
              &&(pLogCtrl->pFilePoint == NULL)) {
        SvcLog_DBG(SVC_LOG_VIN_BUF_MON, "Fail to proc mem log - invalid file pointer", 0U, 0U);
    } else {
        char *pStart = NULL, *pEnd = NULL, *pLimit = NULL, *pCharLF;

        // Configure start/end pointer
        AmbaMisra_TypeCast(&(pLimit), &(pLogCtrl->pMemLimit));

        if (pLogCtrl->RcvSize <= pLogCtrl->MemSize) {
            AmbaMisra_TypeCast(&(pStart), &(pLogCtrl->pMemBase));
            if (pStart != NULL) {
                pEnd = &(pStart[pLogCtrl->RcvSize]);
            }
        } else {
            AmbaMisra_TypeCast(&(pStart), &(pLogCtrl->pMemWrite));

            if (pLogCtrl->pMemWrite == pLogCtrl->pMemBase) {
                pEnd = pLimit;
            } else {
                pEnd = pStart;
                pEnd --;
            }
        }

        if ((pStart == NULL) || (pEnd == NULL)) {
            SvcLog_NG(SVC_LOG_VIN_BUF_MON, "Fail to proc mem log - start/end buffer", 0U, 0U);
        } else {
            char StrBuf[512U];
            UINT32 StrBufSize = (UINT32)sizeof(StrBuf), StrLen = 0U;
            UINT32 NumOfSuccess = 0U;

            if (LastNLine > 0U) {
                UINT32 CurLineNum = 0U;
                pCharLF = pEnd;
                while ((pCharLF != pStart) && (CurLineNum <= LastNLine)) {
                    if (*pCharLF == '\n') {
                        CurLineNum ++;
                    }
                    pCharLF--;
                }

                pCharLF ++;

                pStart = pCharLF;
            }

            if (pLogCtrl->PrintFunc != NULL) {
                pLogCtrl->PrintFunc("---- Process memory log buffer start ----\n");

                AmbaSvcWrap_MisraMemset(StrBuf, 0, sizeof(StrBufSize)); StrLen = 0U;
                SVC_WRAP_SNPRINT "  Base(%p), Limit(%p), WritePoint(%p), BufSize(%d), RcvSize(%u)\n"
                    SVC_SNPRN_ARG_S     StrBuf
                    SVC_SNPRN_ARG_BSIZE StrBufSize
                    SVC_SNPRN_ARG_RLEN  &StrLen
                    SVC_SNPRN_ARG_CPOINT pLogCtrl->pMemBase  SVC_PRN_ARG_POST
                    SVC_SNPRN_ARG_CPOINT pLogCtrl->pMemLimit SVC_PRN_ARG_POST
                    SVC_SNPRN_ARG_CPOINT pLogCtrl->pMemWrite SVC_PRN_ARG_POST
                    SVC_SNPRN_ARG_UINT32 pLogCtrl->MemSize   SVC_PRN_ARG_POST
                    SVC_SNPRN_ARG_UINT32 pLogCtrl->RcvSize   SVC_PRN_ARG_POST
                    SVC_SNPRN_ARG_E
                (pLogCtrl->PrintFunc)(StrBuf);

                AmbaSvcWrap_MisraMemset(StrBuf, 0, sizeof(StrBufSize)); StrLen = 0U;
                SVC_WRAP_SNPRINT "  pStart(%p), pEnd(%p), pLimit(%p)\n"
                    SVC_SNPRN_ARG_S     StrBuf
                    SVC_SNPRN_ARG_BSIZE StrBufSize
                    SVC_SNPRN_ARG_RLEN  &StrLen
                    SVC_SNPRN_ARG_CPOINT pStart SVC_PRN_ARG_POST
                    SVC_SNPRN_ARG_CPOINT pEnd   SVC_PRN_ARG_POST
                    SVC_SNPRN_ARG_CPOINT pLimit SVC_PRN_ARG_POST
                    SVC_SNPRN_ARG_E
                (pLogCtrl->PrintFunc)(StrBuf);

            }

            AmbaSvcWrap_MisraMemset(StrBuf, 0, sizeof(StrBufSize)); StrLen = 0U;

            pCharLF = pStart;
            while (pCharLF != pEnd) {

                // Copy string by line
                while ((StrLen + 1U) < StrBufSize) {    // the local string buffer is not full
                    StrBuf[StrLen] = *pCharLF;          // copy char to local buffer
                    StrLen ++; pCharLF ++;

                    if ((StrBuf[StrLen - 1U] == '\n')   // hit line char
                      ||(pCharLF == pLimit)             // hit the scan buffer limit
                      ||(pCharLF == pEnd)) {
                        break;
                    }
                }
                if (StrLen > 0U) {
                    StrBuf[StrLen] = '\0';
                    if ((StrBuf[StrLen - 1U] == '\n') || ((StrLen + 1U) == StrBufSize)) {

                        if ((pLogCtrl->State & SVC_VIN_BUF_MON_MEM_FLG_SHOW) > 0U) {
                            // Print to console
                            if (pLogCtrl->PrintFunc != NULL) {
                                (pLogCtrl->PrintFunc)(StrBuf);
                            }
                        }

                        if ((pLogCtrl->State & SVC_VIN_BUF_MON_MEM_FLG_FILE) > 0U) {
                            ErrCode = AmbaFS_FileWrite(StrBuf, 1, StrLen, pLogCtrl->pFilePoint, &NumOfSuccess);
                            AmbaMisra_TouchUnused(&ErrCode);
                            AmbaMisra_TouchUnused(&NumOfSuccess);
                        }

                        // Reset local string buffer for next turn
                        AmbaSvcWrap_MisraMemset(StrBuf, 0, StrBufSize); StrLen = 0U;
                    }
                }
                if (pCharLF == pLimit) {
                    AmbaMisra_TypeCast(&(pCharLF), &(pLogCtrl->pMemBase));
                }
            }

            if (pLogCtrl->PrintFunc != NULL) {
                pLogCtrl->PrintFunc("---- Process memory log buffer done ----\n");
            }
        }
    }
}

static UINT32 SvcVinBufMonTask_MemLogSysTime(char *pBuf, UINT32 BufSize)
{
    UINT32 RetSize = 0U;
    const UINT32 StrBufSize = SVC_VIN_BUF_MON_MEM_LOG_SYS_TIME_ALIGN + 5U;

    if ((pBuf != NULL) && (BufSize >= StrBufSize)) {
        char pStrBuf[StrBufSize];
        UINT32 DigitalLen, PaddingCnt = 0U;
        UINT32 TimeTicks = 0U;
        const INT32 ASCII_0 = 48; // '0'

        if (0U != AmbaKAL_GetSysTickCount(&TimeTicks)) {
            TimeTicks = 0U;
        }

        DigitalLen = SvcVinBufMonTask_GetDigitalNum(TimeTicks);

        pStrBuf[0] = '[';
        pStrBuf[1] = 'S';
        pStrBuf[2] = ':';
        if (DigitalLen >= SVC_VIN_BUF_MON_MEM_LOG_SYS_TIME_ALIGN) {
            RetSize = AmbaUtility_UInt32ToStr(&(pStrBuf[3]), StrBufSize - 3U, TimeTicks, 10U);
            if (RetSize == 0U) {
                // misra-c. no apply time tick string
            }
        } else {
            PaddingCnt = SVC_VIN_BUF_MON_MEM_LOG_SYS_TIME_ALIGN - DigitalLen;
            AmbaSvcWrap_MisraMemset(&(pStrBuf[3]), ASCII_0, PaddingCnt);
            RetSize = AmbaUtility_UInt32ToStr(&(pStrBuf[3U + PaddingCnt]), (StrBufSize - 3U) - PaddingCnt, TimeTicks, 10U);
            if (RetSize == 0U) {
                // misra-c. no apply time tick string
            }
        }
        pStrBuf[StrBufSize - 2U] = ']';
        pStrBuf[StrBufSize - 1U] = '\0';

        SvcWrap_strcpy(pBuf, BufSize, pStrBuf);

        AmbaMisra_TouchUnused(&RetSize);

        RetSize = StrBufSize - 1U;
    }

    return RetSize;

}

static UINT32 SvcVinBufMonTask_MemLogOrcTime(char *pBuf, UINT32 BufSize)
{
    UINT32 RetSize = 0U;
    const UINT32 StrBufSize = SVC_VIN_BUF_MON_MEM_LOG_ORC_TIME_ALIGN + 5U;

    if ((pBuf != NULL) && (BufSize >= StrBufSize)) {
        char pStrBuf[StrBufSize];
        UINT32 DigitalLen, PaddingCnt = 0U;
        UINT32 TimeTicks = 0U;
        const INT32 ASCII_0 = 48; // '0'

        if (0U != AmbaSYS_GetOrcTimer(&TimeTicks)) {
            TimeTicks = 0U;
        }

        DigitalLen = SvcVinBufMonTask_GetDigitalNum(TimeTicks);

        pStrBuf[0] = '[';
        pStrBuf[1] = 'O';
        pStrBuf[2] = ':';
        if (DigitalLen >= SVC_VIN_BUF_MON_MEM_LOG_ORC_TIME_ALIGN) {
            RetSize = AmbaUtility_UInt32ToStr(&(pStrBuf[3]), StrBufSize - 3U, TimeTicks, 10U);
            if (RetSize == 0U) {
                // misra-c. no apply time tick string
            }
        } else {
            PaddingCnt = SVC_VIN_BUF_MON_MEM_LOG_ORC_TIME_ALIGN - DigitalLen;
            AmbaSvcWrap_MisraMemset(&(pStrBuf[3]), ASCII_0, PaddingCnt);
            RetSize = AmbaUtility_UInt32ToStr(&(pStrBuf[3U + PaddingCnt]), (StrBufSize - 3U) - PaddingCnt, TimeTicks, 10U);
            if (RetSize == 0U) {
                // misra-c. no apply time tick string
            }
        }
        pStrBuf[StrBufSize - 2U] = ']';
        pStrBuf[StrBufSize - 1U] = '\0';

        SvcWrap_strcpy(pBuf, BufSize, pStrBuf);

        AmbaMisra_TouchUnused(&RetSize);

        RetSize = StrBufSize - 1U;
    }

    return RetSize;
}

/**
 * Write debug msg to memory log
 *
 * @param [in] pStrBuf debug msg string buffer
 * @param [in] StrLen debug msg string length
 */
void SvcVinBufMonTask_MemLogWrite(const char *pStrBuf, UINT32 StrLen)
{
    SVC_VIN_BUF_MON_MEM_LOG_CTRL_s *pLogCtrl = &SvcVinBufMomMemLogCtrl;
    UINT32 CmpAddr1 = 0U, CmpAddr2 = 0U;

    AmbaMisra_TypeCast(&(CmpAddr1), &(pLogCtrl->pMemWrite));
    AmbaMisra_TypeCast(&(CmpAddr2), &(pLogCtrl->pMemLimit));

    if ((SvcVinBufMonFlag & SVC_VIN_BUF_MON_MEM_LOG) == 0U) {
        // SvcLog_NG(SVC_LOG_VIN_BUF_MON, "Fail to write mem log - configure module first", 0U, 0U);
    } else if ((pLogCtrl->State & SVC_VIN_BUF_MON_MEM_LOG_CFG) == 0U) {
        // SvcLog_NG(SVC_LOG_VIN_BUF_MON, "Fail to write mem log - configure module first.", 0U, 0U);
    } else if ((pLogCtrl->State & SVC_VIN_BUF_MON_MEM_FLG_ON) == 0U) {
        // module has been disable
    } else if ((pLogCtrl->State & SVC_VIN_BUF_MON_MEM_FLG_WRITE) > 0U) {
        // system is busy to handle other write process. Ignore this one.
    } else if (CmpAddr1 >= CmpAddr2) {
        // ignore process. something wrong
    } else if (StrLen == 0U) {
        // ignore size zero
    } else if (pStrBuf == NULL) {
        // ignore null string buffer
    } else {
        UINT8 *pCurWrite = pLogCtrl->pMemWrite;
        UINT32 CopySize = StrLen;
        const UINT8 *pCmpPoint = NULL;

        pLogCtrl->State |= SVC_VIN_BUF_MON_MEM_FLG_WRITE;
        pLogCtrl->WriteProcCnt += 1U;

        // Update write pointer before copy data.
        pCmpPoint = &(pCurWrite[StrLen]);
        AmbaMisra_TypeCast(&(CmpAddr1), &(pCmpPoint));
        AmbaMisra_TypeCast(&(CmpAddr2), &(pLogCtrl->pMemLimit));
        if (CmpAddr1 >= CmpAddr2) {
            pLogCtrl->pMemWrite = &(pLogCtrl->pMemBase[StrLen - SvcVinBufMonTask_MemDiff(pLogCtrl->pMemLimit, pCurWrite)]);
        } else {
            pLogCtrl->pMemWrite = &(pCurWrite[StrLen]);
        }

        // Copy data to local buffer pointer
        if (StrLen > SvcVinBufMonTask_MemDiff(pLogCtrl->pMemLimit, pCurWrite)) {
            UINT32 TmpSize;
            CopySize = SvcVinBufMonTask_MemDiff(pLogCtrl->pMemLimit, pCurWrite);
            AmbaSvcWrap_MisraMemcpy(pCurWrite, pStrBuf, CopySize);

            TmpSize = StrLen - CopySize;
            AmbaSvcWrap_MisraMemcpy(pLogCtrl->pMemBase
                                 , &pStrBuf[CopySize], (SIZE_t)(TmpSize));
        } else {
            AmbaSvcWrap_MisraMemcpy(pCurWrite, pStrBuf, StrLen);
        }

        if (pLogCtrl->RcvSize < 0xFFFFFFFFU) {
            if ((0xFFFFFFFFU - pLogCtrl->RcvSize) <= StrLen) {
                pLogCtrl->RcvSize = 0xFFFFFFFFU;
            } else {
                pLogCtrl->RcvSize += StrLen;
            }
        }

        if (pLogCtrl->WriteProcCnt >= 1U) {
            pLogCtrl->WriteProcCnt -= 1U;
        }
        if (pLogCtrl->WriteProcCnt == 0U) {
            pLogCtrl->State &= ~SVC_VIN_BUF_MON_MEM_FLG_WRITE;
        }
    }

}

static void SvcVinBufMonTask_MemLogDump(const char *pFileName)
{
    UINT32 ErrCode;
    SVC_VIN_BUF_MON_MEM_LOG_CTRL_s *pLogCtrl = &SvcVinBufMomMemLogCtrl;

    if ((SvcVinBufMonFlag & SVC_VIN_BUF_MON_MEM_LOG) == 0U) {
        SvcLog_NG(SVC_LOG_VIN_BUF_MON, "Fail to dump mem log - configure module first", 0U, 0U);
    } else if ((pLogCtrl->State & SVC_VIN_BUF_MON_MEM_LOG_CFG) == 0U) {
        SvcLog_NG(SVC_LOG_VIN_BUF_MON, "Fail to dump mem log - configure module first.", 0U, 0U);
    } else if ((pLogCtrl->State & SVC_VIN_BUF_MON_MEM_FLG_SHOW) > 0U) {
        SVC_WRAP_PRINT "Fail to dump mem log - invalid state(0x%X)"
            SVC_PRN_ARG_S SVC_LOG_VIN_BUF_MON
            SVC_PRN_ARG_UINT32 pLogCtrl->State SVC_PRN_ARG_POST
            SVC_PRN_ARG_E
    } else if (pFileName == NULL) {
        SvcLog_NG(SVC_LOG_VIN_BUF_MON, "Fail to dump mem log - filename should not null!", 0U, 0U);
    } else if (0U != AmbaFS_FileOpen(pFileName, "wb", &(pLogCtrl->pFilePoint))) {
        SVC_WRAP_PRINT "Fail to dump mem log - create file. %s"
            SVC_PRN_ARG_S SVC_LOG_VIN_BUF_MON
            SVC_PRN_ARG_CSTR pFileName SVC_PRN_ARG_POST
            SVC_PRN_ARG_E
    } else {
        UINT32 OriState = pLogCtrl->State & SVC_VIN_BUF_MON_MEM_FLG_ON;

        // Disable memory log
        pLogCtrl->State &= ~SVC_VIN_BUF_MON_MEM_FLG_ON;

        // Change "SHOW" state
        pLogCtrl->State |= SVC_VIN_BUF_MON_MEM_FLG_FILE;

        // Wait some time to make sure buffer idle
        ErrCode = SvcVinBufMonTask_WaitWriteDone(1000); SVC_VIN_BUF_MON_ERR_HDLR

        // Process memory
        SvcVinBufMonTask_MemLogProc(0U);

        ErrCode = AmbaFS_FileClose(pLogCtrl->pFilePoint); SVC_VIN_BUF_MON_ERR_HDLR
        SVC_WRAP_PRINT "Success to dump memory log to file - %s"
            SVC_PRN_ARG_S SVC_LOG_VIN_BUF_MON
            SVC_PRN_ARG_CSTR pFileName SVC_PRN_ARG_POST
            SVC_PRN_ARG_E
        pLogCtrl->pFilePoint = NULL;

        // Change "SHOW" state
        pLogCtrl->State &= ~SVC_VIN_BUF_MON_MEM_FLG_FILE;

        // Resume memory log state
        pLogCtrl->State |= OriState;
    }
}

static void SvcVinBufMonTask_MemLogShow(UINT32 LastNLine)
{
    UINT32 ErrCode;
    SVC_VIN_BUF_MON_MEM_LOG_CTRL_s *pLogCtrl = &SvcVinBufMomMemLogCtrl;

    if ((SvcVinBufMonFlag & SVC_VIN_BUF_MON_MEM_LOG) == 0U) {
        SvcLog_NG(SVC_LOG_VIN_BUF_MON, "Fail to show mem log - configure module first", 0U, 0U);
    } else if ((pLogCtrl->State & SVC_VIN_BUF_MON_MEM_LOG_CFG) == 0U) {
        SvcLog_NG(SVC_LOG_VIN_BUF_MON, "Fail to show mem log - configure module first.", 0U, 0U);
    } else if ((pLogCtrl->State & SVC_VIN_BUF_MON_MEM_FLG_FILE) > 0U) {
        SVC_WRAP_PRINT "Fail to show mem log - invalid state(0x%X)"
            SVC_PRN_ARG_S SVC_LOG_VIN_BUF_MON
            SVC_PRN_ARG_UINT32 pLogCtrl->State SVC_PRN_ARG_POST
            SVC_PRN_ARG_E
    } else {
        UINT32 OriState = pLogCtrl->State & SVC_VIN_BUF_MON_MEM_FLG_ON;

        // Disable memory log
        pLogCtrl->State &= ~SVC_VIN_BUF_MON_MEM_FLG_ON;

        // Change "SHOW" state
        pLogCtrl->State |= SVC_VIN_BUF_MON_MEM_FLG_SHOW;

        // Wait some time to make sure buffer idle
        ErrCode = SvcVinBufMonTask_WaitWriteDone(1000); SVC_VIN_BUF_MON_ERR_HDLR

        // Process memory
        SvcVinBufMonTask_MemLogProc(LastNLine);

        // Change "SHOW" state
        pLogCtrl->State &= ~SVC_VIN_BUF_MON_MEM_FLG_SHOW;

        // Resume memory log state
        pLogCtrl->State |= OriState;
    }
}

static void SvcVinBufMonTask_MemLog(UINT32 PrefixOn, const char *pStrBuf, UINT32 StrLen)
{
    if ((pStrBuf != NULL) && (StrLen > 0U)) {
        if (PrefixOn > 0U) {
            char PrefixBuf[64U];
            UINT32 PrefixBufNum = 64U;
            UINT32 CurStrLen;

            AmbaSvcWrap_MisraMemset(PrefixBuf, 0, sizeof(PrefixBuf));
            CurStrLen = SvcVinBufMonTask_MemLogSysTime(&(PrefixBuf[0]), PrefixBufNum);
            if (PrefixBufNum > CurStrLen) {
                CurStrLen += SvcVinBufMonTask_MemLogOrcTime(&(PrefixBuf[CurStrLen]), PrefixBufNum - CurStrLen);
            }
            SvcVinBufMonTask_MemLogWrite(PrefixBuf, CurStrLen);
            SvcVinBufMonTask_MemLogWrite(" ", 1U);
        }

        SvcVinBufMonTask_MemLogWrite(pStrBuf, StrLen);
    }
}

void SvcVinBufMonTask_ShellEntry(UINT32 ArgCount, char * const *pArgVector, AMBA_SHELL_PRINT_f PrintFunc)
{
    UINT32 ErrCode;
    UINT32 CurArgCount = ArgCount;

    if ((CurArgCount > 0U) && (pArgVector != NULL)) {
        if (0 == SvcWrap_strcmp("arg_dbg", pArgVector[CurArgCount - 1U])) {
            UINT32 ArgIdx;

            CurArgCount --;

            SVC_WRAP_PRINT "" SVC_PRN_ARG_S SVC_LOG_VIN_BUF_MON SVC_PRN_ARG_E
            SVC_WRAP_PRINT "==== %sShell Arguments Setting%s (%s%d%s) ===="
                SVC_PRN_ARG_S SVC_LOG_VIN_BUF_MON
                SVC_PRN_ARG_CSTR   "\033""[38;2;255;255;128m""\033""[48;2;0;0;0m" SVC_PRN_ARG_POST
                SVC_PRN_ARG_CSTR   "\033""[0m"                                    SVC_PRN_ARG_POST
                SVC_PRN_ARG_CSTR   "\033""[38;2;255;128;128m""\033""[48;2;0;0;0m" SVC_PRN_ARG_POST
                SVC_PRN_ARG_UINT32 CurArgCount                                    SVC_PRN_ARG_POST
                SVC_PRN_ARG_CSTR   "\033""[0m"                                    SVC_PRN_ARG_POST
                SVC_PRN_ARG_E
            for (ArgIdx = 0U; ArgIdx < CurArgCount; ArgIdx ++) {
                SVC_WRAP_PRINT "  pArgVector[%s%d%s] : %s%s%s"
                    SVC_PRN_ARG_S SVC_LOG_VIN_BUF_MON
                    SVC_PRN_ARG_CSTR   "\033""[38;2;255;128;128m""\033""[48;2;0;0;0m" SVC_PRN_ARG_POST
                    SVC_PRN_ARG_UINT32 ArgIdx                                         SVC_PRN_ARG_POST
                    SVC_PRN_ARG_CSTR   "\033""[0m"                                    SVC_PRN_ARG_POST
                    SVC_PRN_ARG_CSTR   "\033""[38;2;84;204;239m""\033""[48;2;0;0;0m"  SVC_PRN_ARG_POST
                    SVC_PRN_ARG_CSTR   pArgVector[ArgIdx]                             SVC_PRN_ARG_POST
                    SVC_PRN_ARG_CSTR   "\033""[0m"                                    SVC_PRN_ARG_POST
                    SVC_PRN_ARG_E
            }
        }
    }

    if ((CurArgCount > 2U) && (pArgVector != NULL)) {

        SvcVinBufMomMemLogCtrl.PrintFunc = PrintFunc;

        if (0 == SvcWrap_strcmp("cfg", pArgVector[2U])) {
            SvcVinBufMonTask_MemLogCfg(NULL, 0U);
        } else if (0 == SvcWrap_strcmp("on", pArgVector[2U])) {
            SvcVinBufMonTask_MemLogEna(1U);
        } else if (0 == SvcWrap_strcmp("off", pArgVector[2U])) {
            SvcVinBufMonTask_MemLogEna(0U);
        } else if (0 == SvcWrap_strcmp("reset", pArgVector[2U])) {
            SvcVinBufMonTask_MemLogRst();
        } else if (0 == SvcWrap_strcmp("info", pArgVector[2U])) {
            SvcVinBufMonTask_MemLogInfo();
        } else if (0 == SvcWrap_strcmp("dump", pArgVector[2U])) {
            if (CurArgCount > 3U) {
                SvcVinBufMonTask_MemLogDump(pArgVector[3U]);
            }
        } else if (0 == SvcWrap_strcmp("show", pArgVector[2U])) {
            UINT32 LastNLine = 0U;

            if (ArgCount > 3U) {
                ErrCode = SvcWrap_strtoul(pArgVector[3U], &LastNLine); SVC_VIN_BUF_MON_ERR_HDLR
            }

            SvcVinBufMonTask_MemLogShow(LastNLine);
        } else if (0 == SvcWrap_strcmp("dbg", pArgVector[2U])) {
            if (ArgCount > 3U) {
                if ((pArgVector[3][0] == '0') && (pArgVector[3][1] == '\0')) {
                    SvcVinBufMonFlag &= ~SVC_VIN_BUF_MON_DBG_LOG;
                } else {
                    SvcVinBufMonFlag |= SVC_VIN_BUF_MON_DBG_LOG;
                }

                SVC_WRAP_PRINT "%s vin buf monitor debug msg."
                    SVC_PRN_ARG_S SVC_LOG_VIN_BUF_MON
                    SVC_PRN_ARG_CSTR (((SvcVinBufMonFlag & SVC_VIN_BUF_MON_DBG_LOG) > 0U)?"Enable":"Disable") SVC_PRN_ARG_POST
                    SVC_PRN_ARG_E
            }
        } else {
            // misra-c
        }
    }
}


static void SvcVinBufMonTask_ErrHdlr(const char *pCaller, UINT32 CodeLine, UINT32 ErrCode)
{
    if (pCaller != NULL) {
        if (ErrCode != 0U) {
            SVC_WRAP_PRINT "Catch ErrCode(0x%08x) @ %s, %d"
            SVC_PRN_ARG_S SVC_LOG_VIN_BUF_MON
            SVC_PRN_ARG_UINT32 ErrCode  SVC_PRN_ARG_POST
            SVC_PRN_ARG_CSTR   pCaller  SVC_PRN_ARG_POST
            SVC_PRN_ARG_UINT32 CodeLine SVC_PRN_ARG_POST
            SVC_PRN_ARG_E
        }
    }
}

static UINT32 SvcVinBufMonTask_RawRdyHdlr(const void *pEventData)
{
    UINT32 ErrCode;
    const AMBA_DSP_RAW_DATA_RDY_EXT3_s *pData;

    AmbaMisra_TypeCast(&(pData), &(pEventData));

    if (pData != NULL) {
        if (pData->VinId < AMBA_DSP_MAX_VIN_NUM) {
            SVC_VIN_BUF_MON_MSG_s BufMonMsg;
            UINT32 CurMaxRawDepth;

#ifdef CONFIG_ICAM_DSP_VIN_RAW_BUF_NUM
            CurMaxRawDepth = CONFIG_ICAM_DSP_VIN_RAW_BUF_NUM;
#else
            CurMaxRawDepth = 0U;
#endif

            AmbaSvcWrap_MisraMemset(&BufMonMsg, 0, sizeof(BufMonMsg));
            BufMonMsg.VinID     = (UINT32)pData->VinId;             BufMonMsg.VinID    &= 0xFFU;
            BufMonMsg.FreeNum   = (UINT32)pData->RawBufferNumFree;  BufMonMsg.FreeNum  &= 0xFFU;
            BufMonMsg.TotalNum  = CurMaxRawDepth;                   BufMonMsg.TotalNum &= 0xFFU;
            BufMonMsg.CapPts    = pData->CapPts;
            BufMonMsg.CapSeqCnt = pData->CapSequence;

            if ((SvcVinBufMonFlag & SVC_VIN_BUF_MON_DBG_LOG) > 0U) {
                SVC_WRAP_SNPRINT_s SnPrnCtrl;
                char StrBuf[128];
                UINT32 StrBufLen;

                AmbaSvcWrap_MisraMemset(StrBuf, 0, sizeof(StrBuf)); StrBufLen = 0U;
                AmbaSvcWrap_MisraMemset(&SnPrnCtrl, 0, sizeof(SnPrnCtrl));
                SnPrnCtrl.pStrFmt = "VinID(%2d)      data rdy, CapSeqCnt(0x%016llX), Free(%2d) / Total(%2d)";
                SnPrnCtrl.Argv[SnPrnCtrl.Argc].Uint64 = (UINT64)(BufMonMsg.VinID); SnPrnCtrl.Argv[SnPrnCtrl.Argc].Uint64 &= 0xFFFFFFFFU;    SnPrnCtrl.Argc++;
                SnPrnCtrl.Argv[SnPrnCtrl.Argc].Uint64 = BufMonMsg.CapSeqCnt;                                                                SnPrnCtrl.Argc++;
                SnPrnCtrl.Argv[SnPrnCtrl.Argc].Uint64 = (UINT64)(BufMonMsg.FreeNum); SnPrnCtrl.Argv[SnPrnCtrl.Argc].Uint64 &= 0xFFFFFFFFU;  SnPrnCtrl.Argc++;
                SnPrnCtrl.Argv[SnPrnCtrl.Argc].Uint64 = (UINT64)(BufMonMsg.TotalNum); SnPrnCtrl.Argv[SnPrnCtrl.Argc].Uint64 &= 0xFFFFFFFFU; SnPrnCtrl.Argc++;
                StrBufLen = SvcWrap_snprintf(StrBuf, (UINT32)sizeof(StrBuf), &SnPrnCtrl);
                if (StrBufLen > 0U) {
                    SvcVinBufMonTask_MemLog(1U, StrBuf, StrBufLen);
                    if (BufMonMsg.VinID < AMBA_DSP_MAX_VIN_NUM) {
                        if (BufMonMsg.VinID == SvcVinBufMonCtrl.EffectMaster[BufMonMsg.VinID]) {
                            SvcVinBufMonTask_MemLog(0U, " ***", 4U);
                        }
                    }
                    SvcVinBufMonTask_MemLog(0U, "\n", 1U);
                }
                AmbaMisra_TouchUnused(&(BufMonMsg.CapPts));
            }

#if defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)
            if (SvcVinBufMonCtrl.EffectVinNum[BufMonMsg.VinID] > 1U) {  // effect on
                // If specify vin buffer proc cmd is not triggered, ...
                if ((SvcVinBufMonCtrl.State[BufMonMsg.VinID] & SVC_VIN_BUF_MON_STATE_ACTIVE) == 0U) {
                    // If specify vin buffer free space <= 1, request vin buffer proc
                    if (BufMonMsg.FreeNum <= 1U) {
                        ErrCode = AmbaKAL_MsgQueueSend(&(SvcVinBufMonCtrl.Que), &BufMonMsg, AMBA_KAL_NO_WAIT); SVC_VIN_BUF_MON_ERR_HDLR
                    }
                } else { // If specify vin buffer proc cmd has been processed, ignore current sync and resume state
                    SvcVinBufMonCtrl.State[BufMonMsg.VinID] &= ~SVC_VIN_BUF_MON_STATE_ACTIVE;
                }
            }
#endif
        }

        ErrCode = AmbaKAL_EventFlagSet(&(SvcVinBufMonCtrl.Flag), SVC_VIN_BUF_MON_FLAG_DATA_RDY); SVC_VIN_BUF_MON_ERR_HDLR
    }

    return 0U;
}

static UINT32 SvcVinBufMonTask_YuvRdyHdlr(const void *pEventData)
{
    const AMBA_DSP_YUV_DATA_RDY_s *pData;
    SVC_WRAP_SNPRINT_s SnPrnCtrl;

    AmbaMisra_TypeCast(&(pData), &(pEventData));

    if (pData != NULL) {

        if ((pData->ViewZoneId & 0x80000000U) == 0U) {
            if ((SvcVinBufMonFlag & SVC_VIN_BUF_MON_DBG_LOG) > 0U) {
                char StrBuf[128];
                UINT32 StrBufLen;

                AmbaSvcWrap_MisraMemset(StrBuf, 0, sizeof(StrBuf)); StrBufLen = 0U;
                AmbaSvcWrap_MisraMemset(&SnPrnCtrl, 0, sizeof(SnPrnCtrl));
                SnPrnCtrl.pStrFmt = "ViewZoneID(%2d) data rdy, CapSeqCnt(0x%016llX), From VinID(%2d)\n";
                SnPrnCtrl.Argv[SnPrnCtrl.Argc].Uint64 = (UINT64)(pData->ViewZoneId); SnPrnCtrl.Argv[SnPrnCtrl.Argc].Uint64 &= 0xFFFFFFFFU; SnPrnCtrl.Argc++;
                SnPrnCtrl.Argv[SnPrnCtrl.Argc].Uint64 = pData->CapSequence; SnPrnCtrl.Argc++;
                SnPrnCtrl.Argv[SnPrnCtrl.Argc].Uint64 = (UINT64)(SvcVinBufMonCtrl.ViewZoneVinID[pData->ViewZoneId]);
                SnPrnCtrl.Argv[SnPrnCtrl.Argc].Uint64&= 0xFFU; SnPrnCtrl.Argc++;
                StrBufLen = SvcWrap_snprintf(StrBuf, (UINT32)sizeof(StrBuf), &SnPrnCtrl);
                if (StrBufLen > 0U) {
                    SvcVinBufMonTask_MemLog(1U, StrBuf, StrBufLen);
                }
            }
        }
    }

    return 0U;
}

static void SvcVinBufMonTask_VinBufProc(UINT32 VinID)
{
    if (VinID < AMBA_DSP_MAX_VIN_NUM) {
#if defined(CONFIG_SOC_CV5)|| defined(CONFIG_SOC_CV52)
        UINT32 Idx, ErrCode;
        AMBA_DSP_LIVEVIEW_DROP_REPEAT_s DropCfg[AMBA_DSP_MAX_VIEWZONE_NUM];

        AmbaSvcWrap_MisraMemset(&DropCfg, 0, sizeof(DropCfg));
        for (Idx = 0U; Idx < SvcVinBufMonCtrl.ViewZoneNum[VinID]; Idx ++) {
            DropCfg[Idx].Method = AMBA_DSP_LV_DROP_RPT_METHOD_ON_DEMAND;
            DropCfg[Idx].Option = AMBA_DSP_LV_DROP_RPT_OPTION_DROP;
        }

        ErrCode = AmbaDSP_LiveviewDropRepeatCtrl( SvcVinBufMonCtrl.ViewZoneNum[VinID]
                                                , SvcVinBufMonCtrl.ViewZoneIDs[VinID]
                                                , DropCfg);
        if (ErrCode != 0U) {
            SvcLog_NG(SVC_LOG_VIN_BUF_MON, "Fail to proc VinID(%d) raw buf proc cmd! ErrCode(0x%08X)", VinID, ErrCode);
        }
#endif
        AmbaMisra_TouchUnused(&VinID);
    }
}

static void* SvcVinBufMonTask_TaskEntry(void* EntryArg)
{
    UINT32 ErrCode;
    ULONG ArgVal = 0U;
    SVC_VIN_BUF_MON_MSG_s BufMonMsg;
    SVC_WRAP_SNPRINT_s SnPrnCtrl;
    char StrBuf[64];
    UINT32 StrBufLen;
    UINT32 ActualFlag = 0U, MaxWaitTime = 10000U;

    if (SvcVinBufMonCtrl.VinSelectBits > 0U) {
        ErrCode = AmbaDSP_EventHandlerRegister(AMBA_DSP_EVENT_LV_RAW_RDY, SvcVinBufMonTask_RawRdyHdlr); SVC_VIN_BUF_MON_ERR_HDLR
        ErrCode = AmbaDSP_EventHandlerRegister(AMBA_DSP_EVENT_LV_YUV_DATA_RDY, SvcVinBufMonTask_YuvRdyHdlr); SVC_VIN_BUF_MON_ERR_HDLR

        ErrCode = AmbaKAL_EventFlagGet( &(SvcVinBufMonCtrl.Flag)
                                      , SVC_VIN_BUF_MON_FLAG_DATA_RDY
                                      , AMBA_KAL_FLAGS_ALL
                                      , AMBA_KAL_FLAGS_CLEAR_NONE
                                      , &ActualFlag
                                      , MaxWaitTime);
        if (ErrCode != 0U) {
            SvcLog_NG(SVC_LOG_VIN_BUF_MON, "Wait raw data rdy fail. Timeout(%d), ErrCode(0x%08X)", MaxWaitTime, ErrCode);
        }

        AmbaMisra_TouchUnused(EntryArg);
        AmbaSvcWrap_MisraMemcpy(&ArgVal, EntryArg, sizeof(ULONG));

        while ( ArgVal != 0xCafeU ) {
            AmbaSvcWrap_MisraMemset(&BufMonMsg, 0, sizeof(BufMonMsg));
            ErrCode = AmbaKAL_MsgQueueReceive(&(SvcVinBufMonCtrl.Que), &BufMonMsg, AMBA_KAL_WAIT_FOREVER);
            if (ErrCode == 0U) {

                if (BufMonMsg.VinID < AMBA_DSP_MAX_VIN_NUM) {
                    if ((SvcVinBufMonCtrl.VinSelectBits & (0x1UL << BufMonMsg.VinID)) > 0U) {
                        if ((SvcVinBufMonCtrl.State[BufMonMsg.VinID] & SVC_VIN_BUF_MON_STATE_ACTIVE) > 0U) {

                            if ((SvcVinBufMonFlag & SVC_VIN_BUF_MON_DBG_LOG) > 0U) {
                                AmbaSvcWrap_MisraMemset(StrBuf, 0, sizeof(StrBuf)); StrBufLen = 0U;
                                AmbaSvcWrap_MisraMemset(&SnPrnCtrl, 0, sizeof(SnPrnCtrl));
                                SnPrnCtrl.pStrFmt = "VinID(%2d) buf cmd is processing. Ignore the new buf proc req.\n";
                                SnPrnCtrl.Argv[SnPrnCtrl.Argc].Uint64 = (UINT64)(BufMonMsg.VinID); SnPrnCtrl.Argv[SnPrnCtrl.Argc].Uint64 &= 0xFFFFFFFFU;    SnPrnCtrl.Argc++;
                                StrBufLen = SvcWrap_snprintf(StrBuf, (UINT32)sizeof(StrBuf), &SnPrnCtrl);
                                if (StrBufLen > 0U) {
                                    SvcVinBufMonTask_MemLog(1U, StrBuf, StrBufLen);
                                }
                            }

                        } else { //if ((SvcVinBufMonCtrl.State[BufMonMsg.VinID] & SVC_VIN_BUF_MON_STATE_ACTIVE) == 0U)
                            SvcVinBufMonCtrl.State[BufMonMsg.VinID] |= SVC_VIN_BUF_MON_STATE_ACTIVE;

                            if ((SvcVinBufMonFlag & SVC_VIN_BUF_MON_DBG_LOG) > 0U) {
                                AmbaSvcWrap_MisraMemset(StrBuf, 0, sizeof(StrBuf)); StrBufLen = 0U;
                                AmbaSvcWrap_MisraMemset(&SnPrnCtrl, 0, sizeof(SnPrnCtrl));
                                SnPrnCtrl.pStrFmt = "VinID(%2d) buf cmd - begin\n";
                                SnPrnCtrl.Argv[SnPrnCtrl.Argc].Uint64 = (UINT64)(BufMonMsg.VinID); SnPrnCtrl.Argv[SnPrnCtrl.Argc].Uint64 &= 0xFFFFFFFFU;    SnPrnCtrl.Argc++;
                                StrBufLen = SvcWrap_snprintf(StrBuf, (UINT32)sizeof(StrBuf), &SnPrnCtrl);
                                if (StrBufLen > 0U) {
                                    SvcVinBufMonTask_MemLog(1U, StrBuf, StrBufLen);
                                }
                            }

                            SvcVinBufMonTask_VinBufProc(BufMonMsg.VinID);

                            if ((SvcVinBufMonFlag & SVC_VIN_BUF_MON_DBG_LOG) > 0U) {
                                AmbaSvcWrap_MisraMemset(StrBuf, 0, sizeof(StrBuf)); StrBufLen = 0U;
                                AmbaSvcWrap_MisraMemset(&SnPrnCtrl, 0, sizeof(SnPrnCtrl));
                                SnPrnCtrl.pStrFmt = "VinID(%2d) buf cmd - end\n";
                                SnPrnCtrl.Argv[SnPrnCtrl.Argc].Uint64 = (UINT64)(BufMonMsg.VinID); SnPrnCtrl.Argv[SnPrnCtrl.Argc].Uint64 &= 0xFFFFFFFFU;    SnPrnCtrl.Argc++;
                                StrBufLen = SvcWrap_snprintf(StrBuf, (UINT32)sizeof(StrBuf), &SnPrnCtrl);
                                if (StrBufLen > 0U) {
                                    SvcVinBufMonTask_MemLog(1U, StrBuf, StrBufLen);
                                }
                            }

                        }
                    }
                }
            }

            AmbaMisra_TouchUnused(&ArgVal);
        }
    }

    return NULL;
}

static void SvcVinBufMonTask_Info(void)
{
    UINT32 VinID;
    UINT16 Idx;

    SvcLog_DBG(SVC_LOG_VIN_BUF_MON, "", 0U, 0U);
    SVC_WRAP_PRINT "====== Vin Buffer Monitor Task ======" SVC_PRN_ARG_S SVC_LOG_VIN_BUF_MON SVC_PRN_ARG_E
    SVC_WRAP_PRINT "  VinSelectBits : 0x%08X" SVC_PRN_ARG_S SVC_LOG_VIN_BUF_MON
        SVC_PRN_ARG_UINT32 SvcVinBufMonCtrl.VinSelectBits SVC_PRN_ARG_POST
        SVC_PRN_ARG_E
    SVC_WRAP_PRINT "  VinID | EftVinNum | EftVinBits | Master | ViewZoneNum | ViewZoneID" SVC_PRN_ARG_S SVC_LOG_VIN_BUF_MON SVC_PRN_ARG_E
    for (VinID = 0U; VinID < AMBA_DSP_MAX_VIN_NUM; VinID ++) {
        if ((SvcVinBufMonCtrl.VinSelectBits & (0x1UL << VinID)) > 0U) {
            SVC_WRAP_PRINT "     %2d |     %2d    | 0x%08X |   %2d   |          %2d | %2d" SVC_PRN_ARG_S SVC_LOG_VIN_BUF_MON
                SVC_PRN_ARG_UINT32 VinID                                SVC_PRN_ARG_POST
                SVC_PRN_ARG_UINT32 SvcVinBufMonCtrl.EffectVinNum[VinID]   SVC_PRN_ARG_POST
                SVC_PRN_ARG_UINT32 SvcVinBufMonCtrl.EffectVinBits[VinID]  SVC_PRN_ARG_POST
                SVC_PRN_ARG_UINT32 SvcVinBufMonCtrl.EffectMaster[VinID]   SVC_PRN_ARG_POST
                SVC_PRN_ARG_UINT16 SvcVinBufMonCtrl.ViewZoneNum[VinID]    SVC_PRN_ARG_POST
                SVC_PRN_ARG_UINT16 SvcVinBufMonCtrl.ViewZoneIDs[VinID][0] SVC_PRN_ARG_POST
                SVC_PRN_ARG_E
            for (Idx = 1U; Idx < SvcVinBufMonCtrl.ViewZoneNum[VinID]; Idx ++) {
                SVC_WRAP_PRINT "                                                          %2d" SVC_PRN_ARG_S SVC_LOG_VIN_BUF_MON
                    SVC_PRN_ARG_UINT16 SvcVinBufMonCtrl.ViewZoneIDs[VinID][Idx] SVC_PRN_ARG_POST
                    SVC_PRN_ARG_E
            }
        }
    }
}

/**
 * Create vin buffer monitor task
 * return 0-OK, 1-NG
 */
UINT32 SvcVinBufMonTask_Create(void)
{
    UINT32 RetVal = SVC_OK, ErrCode;

    if ((SvcVinBufMonFlag & SVC_VIN_BUF_MON_INIT) > 0U) {
        SvcLog_DBG(SVC_LOG_VIN_BUF_MON, "Vin buffer monitor task has been created", 0U, 0U);
    } else {
        SVC_LIV_INFO_s CurLivInfo;
        UINT32 Idx, VinID, VinSrc;
        UINT32 VinIDs[AMBA_DSP_MAX_VIN_NUM], VinNum = 0U;
        UINT32 ViewZoneNum, ViewZoneIdx;
        UINT32 ViewZoneIDs[AMBA_DSP_MAX_VIEWZONE_NUM];

        // Initial setting
        AmbaSvcWrap_MisraMemset(&SvcVinBufMonCtrl, 0, sizeof(SvcVinBufMonCtrl));
        AmbaUtility_StringAppend(SvcVinBufMonCtrl.Name, (UINT32)sizeof(SvcVinBufMonCtrl.Name), "SvcVinBufMonTask");
        AmbaSvcWrap_MisraMemset(SvcVinBufMonCtrl.ViewZoneVinID, 255, sizeof(SvcVinBufMonCtrl.ViewZoneVinID));

        AmbaSvcWrap_MisraMemset(VinIDs, 0, sizeof(VinIDs));
        if (0U == SvcResCfg_GetVinIDs(VinIDs, &VinNum)) {
            for (Idx = 0U; Idx < VinNum; Idx ++) {
                if (VinIDs[Idx] < AMBA_DSP_MAX_VIN_NUM) {
                    VinID = VinIDs[Idx];
                    VinSrc = 255U;

                    if (0U == SvcResCfg_GetVinSrc(VinID, &VinSrc)) {
                        if ((VinSrc == SVC_VIN_SRC_SENSOR) || (VinSrc == SVC_VIN_SRC_YUV)) {
                            SvcVinBufMonCtrl.VinSelectBits |= (UINT32)(0x1UL << VinID);
                            SvcVinBufMonCtrl.EffectMaster[VinID] = VinID;
                            SvcVinBufMonCtrl.EffectVinNum[VinID] = 1U;
                            SvcVinBufMonCtrl.EffectVinBits[VinID] = (UINT32)(0x1UL << VinID);

                            ViewZoneNum = 0U;
                            AmbaSvcWrap_MisraMemset(ViewZoneIDs, 0, sizeof(ViewZoneIDs));
                            if (0U == SvcResCfg_GetFovIdxsInVinID(VinID, ViewZoneIDs, &ViewZoneNum)) {
                                SvcVinBufMonCtrl.ViewZoneNum[VinID] = (UINT16)ViewZoneNum;
                                for (ViewZoneIdx = 0U; ViewZoneIdx < ViewZoneNum; ViewZoneIdx ++) {
                                    SvcVinBufMonCtrl.ViewZoneIDs[VinID][ViewZoneIdx] = (UINT16)ViewZoneIDs[ViewZoneIdx];
                                    SvcVinBufMonCtrl.ViewZoneVinID[ViewZoneIDs[ViewZoneIdx]] = VinID;
                                }
                            }
                        }
                    }
                }
            }
        }

        // Exist valid vin for buffer monitor.
        if (SvcVinBufMonCtrl.VinSelectBits > 0U) {
            SvcLiveview_InfoGet(&CurLivInfo);
            if ((CurLivInfo.pNumStrm != NULL) &&
                (CurLivInfo.pStrmCfg != NULL) &&
                (CurLivInfo.pStrmCfg->pChanCfg != NULL)) {
                UINT32 StrmIdx, ChanIdx;
                UINT32 DepVinBits;
                UINT32 CurViewZoneID, CurViewZoneSrc, CurMaster, BitIdx;
                UINT32 NumOfStrm = *(CurLivInfo.pNumStrm);

                // Check ResCfg about effect
                for (StrmIdx = 0U; StrmIdx < NumOfStrm; StrmIdx ++) {

                    DepVinBits = 0U;
                    CurMaster  = 255U;

                    // Scan streaming channel setting to get dependency vin bits and stream bits
                    for (ChanIdx = 0U; ChanIdx < CurLivInfo.pStrmCfg[StrmIdx].NumChan; ChanIdx ++) {

                        CurViewZoneID = (UINT32)(CurLivInfo.pStrmCfg[StrmIdx].pChanCfg[ChanIdx].ViewZoneId);
                        CurViewZoneID &= 0xFFFFU;

                        if (CurViewZoneID < AMBA_DSP_MAX_VIEWZONE_NUM) {
                            CurViewZoneSrc = 255U;
                            if (0U == SvcResCfg_GetFovSrc(CurViewZoneID, &CurViewZoneSrc)) {
                                if ((CurViewZoneSrc == SVC_VIN_SRC_SENSOR) || (CurViewZoneSrc == SVC_VIN_SRC_YUV)) {
                                    VinID = 255U;
                                    if (0U != SvcResCfg_GetVinIDOfFovIdx(CurViewZoneID, &VinID)) {
                                        SvcLog_NG(SVC_LOG_VIN_BUF_MON, "Fail to create vin buf monitor task - get ViewZone(%d)'s VinID", CurViewZoneID, 0U);
                                    } else {
                                        if (VinID < AMBA_DSP_MAX_VIN_NUM) {
                                            DepVinBits |= (UINT32)(0x1UL << VinID);

                                            if (ChanIdx == 0U) {
                                                CurMaster = VinID;
                                            }
                                        }
                                    }
                                }
                            }

                        }
                    }

                    // Configure the each vin dependency bits
                    for (VinID = 0U; VinID < AMBA_DSP_MAX_VIN_NUM; VinID ++) {
                        if ((DepVinBits & (0x1UL << VinID)) > 0U) {
                            SvcVinBufMonCtrl.EffectVinBits[VinID] |= DepVinBits;
                            SvcVinBufMonCtrl.EffectMaster[VinID] = CurMaster;
                        }
                    }
                }

                // Configure the each vin dependency number
                for (VinID = 0U; VinID < AMBA_DSP_MAX_VIN_NUM; VinID ++) {
                    if (SvcVinBufMonCtrl.EffectVinBits[VinID] > 0U) {
                        SvcVinBufMonCtrl.EffectVinNum[VinID] = 0U;
                        for (BitIdx = 0U; BitIdx < 32U; BitIdx ++) {
                            if ((SvcVinBufMonCtrl.EffectVinBits[VinID] & (0x1UL << BitIdx)) > 0U) {
                                SvcVinBufMonCtrl.EffectVinNum[VinID] ++;
                            }
                        }
                    }
                }
            }

            // Create event flag
            ErrCode = AmbaKAL_EventFlagCreate( &(SvcVinBufMonCtrl.Flag), SvcVinBufMonCtrl.Name );
            if (ErrCode != 0U) {
                RetVal = SVC_NG;
                SvcLog_NG(SVC_LOG_VIN_BUF_MON, "Fail to create vin buf monitor task - create flag fail. ErrCode(0x%08X)", ErrCode, 0U);
            } else {
                ErrCode = AmbaKAL_EventFlagClear(&(SvcVinBufMonCtrl.Flag), 0xFFFFFFFFU); SVC_VIN_BUF_MON_ERR_HDLR
            }

            // Create queue
            if (RetVal == 0U) {
                ErrCode = AmbaKAL_MsgQueueCreate( &(SvcVinBufMonCtrl.Que)
                                                , SvcVinBufMonCtrl.Name
                                                , (UINT32)sizeof(SVC_VIN_BUF_MON_MSG_s)
                                                , SvcVinBufMonCtrl.QueBuf
                                                , (UINT32)sizeof(SvcVinBufMonCtrl.QueBuf) );
                if (ErrCode != 0U) {
                    RetVal = SVC_NG;
                    SvcLog_NG(SVC_LOG_VIN_BUF_MON, "Fail to create vin buf monitor task - create queue fail. ErrCode(0x%08X)", ErrCode, 0U);
                }
            }

            // Create task
            if (RetVal == 0U) {
                SvcVinBufMonCtrl.Ctrl.CpuBits    = SVC_VIN_BUF_MON_TASK_CPU_BITS;
                SvcVinBufMonCtrl.Ctrl.Priority   = SVC_VIN_BUF_MON_TASK_PRI;
                SvcVinBufMonCtrl.Ctrl.EntryFunc  = SvcVinBufMonTask_TaskEntry;
                SvcVinBufMonCtrl.Ctrl.StackSize  = (UINT32)sizeof(SvcVinBufMonCtrl.Stack);
                SvcVinBufMonCtrl.Ctrl.pStackBase = &(SvcVinBufMonCtrl.Stack[0]);

                ErrCode = SvcTask_Create(SvcVinBufMonCtrl.Name, &(SvcVinBufMonCtrl.Ctrl));
                if (ErrCode != 0U) {
                    RetVal = SVC_NG;
                    SvcLog_NG(SVC_LOG_VIN_BUF_MON, "Fail to create vin buf monitor task - create task fail. ErrCode(0x%08X)", ErrCode, 0U);
                } else {
                    SvcLog_OK(SVC_LOG_VIN_BUF_MON, "create vin buf monitor task done", 0U, 0U);

                    SvcVinBufMonFlag |= SVC_VIN_BUF_MON_INIT;
                }
            }
        }

        if ((SvcVinBufMonFlag & SVC_VIN_BUF_MON_DBG_LOG) > 0U) {
            SvcVinBufMonTask_Info();
        }
    }

    return RetVal;
}

/**
 * Delete vin buffer monitor task
 * return 0-OK, 1-NG
 */
UINT32 SvcVinBufMonTask_Delete(void)
{
    UINT32 RetVal = SVC_OK, ErrCode;

    if ((SvcVinBufMonFlag & SVC_VIN_BUF_MON_INIT) > 0U) {
        // Unregister dsp event
        ErrCode = AmbaDSP_EventHandlerUnRegister(AMBA_DSP_EVENT_LV_RAW_RDY, SvcVinBufMonTask_RawRdyHdlr); SVC_VIN_BUF_MON_ERR_HDLR
        ErrCode = AmbaDSP_EventHandlerUnRegister(AMBA_DSP_EVENT_LV_YUV_DATA_RDY, SvcVinBufMonTask_YuvRdyHdlr); SVC_VIN_BUF_MON_ERR_HDLR

        // Destroy task
        ErrCode = SvcTask_Destroy(&(SvcVinBufMonCtrl.Ctrl)); SVC_VIN_BUF_MON_ERR_HDLR

        // Delete queue
        ErrCode = AmbaKAL_MsgQueueFlush(&(SvcVinBufMonCtrl.Que)); SVC_VIN_BUF_MON_ERR_HDLR
        if (ErrCode == 0U) {
            ErrCode = AmbaKAL_MsgQueueDelete(&(SvcVinBufMonCtrl.Que)); SVC_VIN_BUF_MON_ERR_HDLR
        }

        // Delete Flag
        ErrCode = AmbaKAL_EventFlagDelete(&(SvcVinBufMonCtrl.Flag)); SVC_VIN_BUF_MON_ERR_HDLR

        // Reset all setting
        AmbaSvcWrap_MisraMemset(&SvcVinBufMonCtrl, 0, sizeof(SvcVinBufMonCtrl));
        SvcVinBufMonFlag &= ~SVC_VIN_BUF_MON_INIT;

        SvcLog_OK(SVC_LOG_VIN_BUF_MON, "delete vin buf monitor task done", 0U, 0U);
    }

    return RetVal;
}




