/**
*  @file SvcSwPll.c
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
*  @details svc sw pll
*
*/

#include "AmbaKAL.h"
#include "AmbaWrap.h"
#include "AmbaPrint.h" 

#include "AmbaTMR.h"
#include "AmbaDSP.h"
#include "AmbaDSP_Liveview.h"
#include "AmbaDSP_Event.h"

#include "AmbaFTCM.h"
#include "AmbaUtility.h"
#include "AmbaSYS.h"

#include "SvcLog.h"
#include "SvcErrCode.h"
#include "SvcWrap.h"
#include "SvcSwPll.h"
#include "SvcTask.h"
#include "SvcResCfg.h"
#include "SvcIPC.h"
#include "AmbaDspInt.h"

#define ADJUST_AUDIO       (0)

#define SWPLL_LOG_FRM_NUM (5U)

#define SWPLL_VIN_TASK_PRIORITY       (3U)
#define SWPLL_VOUT_TASK_PRIORITY      (15U)
#if (ADJUST_AUDIO == 1)
#define SWPLL_AUDIO_TASK_PRIORITY     (19U)
#endif

#define SWPLL_TIMEOUT (10000U)
#define SYSTIME_TIMESCALE (1000000) /* in microsecond resolution */

#define NUM_LOG_INT (30U)
#define PRINT_DIFF_STOP (100U)

typedef struct {
    UINT64 Cnt;         /**< The index of frame */
    UINT64 U64CurrTime; /**< UINT64 system time */
    INT32  Diff;        /**< Diff returned by AmbaFTCM_Check */
    UINT32 Event;       /**< EventMask returned by AmbaFTCM_Check */
    UINT32 Clk;         /**< Target ClkFreq returned by AmbaFTCM_Check */
} SWPLL_LOG_FRM_s;

typedef struct {
    UINT64 Cnt; /**< The number of AmbaFTCM_Check called*/
    UINT32 LastNFrmsIdx;    /**< Wp of LastNFrms */
    SWPLL_LOG_FRM_s LastNFrms[SWPLL_LOG_FRM_NUM]; /**< Ring buffer for the latest n frames */
} SWPLL_LOG_DEV_s;

typedef struct {
    UINT64 MinSlopeCnt; /**< Counter that MinSlope is set */
    UINT64 MaxSlopeCnt; /**< Counter that MaxSlope is set */
    UINT64 IdealSlopeCnt; /**< Counter that IdealSlope is set */
    INT64 DevDiff;  /**< Difference between the current device time(by slope) and the ideal device time(by interrupt)*/
    INT64 SysDiff;  /**< Difference between the current device time(by slope) and the system time(by AmbaTMR)*/
} SWPLL_LOG_DEV_CLK_s;

typedef struct {
    INT64 SysOffset;   /**< SysTime offset of Slope */
    INT64 DevOffset;   /**< DevTime offset of Slope */
} SWPLL_SLOPE_s;

typedef struct {
    INT64 ClkFreq;          /**< Clock frequency */
    INT64 Idx;              /**< (For SyncINT) Index = CheckCnt - ShortFrameCnt + LongFrameCnt */
    INT64 FreqInTick;       /**< (For SyncINT) Frequency in tick. (UnitsInTick * SysemTimeScale) */
    INT64 TimeScale;        /**< (For SyncINT) Timescale of frame rate. */
    INT64 FrameTime;        /**< (For SyncINT) FrameTime. */
    INT64 InitDevTick;      /**< (For SyncTick) Initial device tick. */
    INT64 InitDevTime;      /**< Initial device time. */
    INT64 LastDevTime;      /**< Last device time. */
    INT64 SyncPtSysTime;    /**< System time for last sync point. */
    INT64 SyncPtDevTime;    /**< Device time for last sync point. */
    INT64 LastUpdateSysTime;/**< System time for last update of IdealSlope. */
    INT64 LastUpdateDevTime;/**< Device time for last update of IdealSlope. */
    SWPLL_SLOPE_s IdealSlope; /**< Ideal Slope */
    SWPLL_SLOPE_s MinSlope;   /**< Maximum slope */
    SWPLL_SLOPE_s MaxSlope;   /**< Minimum slope */
    SWPLL_SLOPE_s *pCurrSlope; /**< Current slope. Pointer to IdealSlope, MinSlope, or MaxSlope*/
} SWPLL_DEV_CLK_s;

typedef struct {
    UINT8 DevId;                /**< Device Id*/
    UINT64 Cnt;                 /**< The number of AmbaFTCM_Check called*/
    UINT64 U64SysTime;          /**< UINT64 system time */
    UINT32 EventMask;           /**< Event mask */
    AMBA_FTCM_CHECK_INFO_s Info;/* Event Info*/
} SWPLL_MSG_s;

#if (ADJUST_AUDIO == 1)
typedef struct{
    UINT64 AudioTick;       /* AudioTick from RawDataReady. */
    UINT32 Lsb20SysTime;    /* LSB20 of system time. */
} AUDIO_MSG_s;
#endif

typedef struct {
    UINT8  DevId;                /**< Device Id*/
    UINT64 Time;                 /**< The number of AmbaFTCM_Check called*/
    INT32  Diff;
} SWPLL_INT_LOG_s;

typedef UINT32 (*SWPLL_ADJUST_f)(UINT32 TargetClk);

#define INVALID_DEV_ID (0xFFU)

typedef struct {
    AMBA_FTCM_CFG_s        FtcmCfg;
    UINT32                 RefClkTmrId;
    SWPLL_MSG_s            MsgQueBuf[64];
#if (ADJUST_AUDIO == 1)
    AUDIO_MSG_s            AudioMsgQueBuf[64];
    AMBA_KAL_MSG_QUEUE_t   AudioMsgQue;
    SVC_TASK_CTRL_s        AudioTask;
#endif
    AMBA_KAL_MSG_QUEUE_t   MsgQue;
    SVC_TASK_CTRL_s        VinTask;
    SVC_TASK_CTRL_s        VoutTask;
    SVC_TASK_CTRL_s        AdjustTask;
    AMBA_KAL_MUTEX_t       DevClkMutex;     /**< For protecting DevClk. */
    AMBA_KAL_MUTEX_t       TmrMutex;  /**< For protecting U64 system time. (GetSystemTimeU64Us). */
    SWPLL_LOG_DEV_s        DevLog[SVC_SWPLL_MAX_NUM_DEV];
    SWPLL_LOG_DEV_CLK_s    DevClkLog[SVC_SWPLL_MAX_NUM_DEV];
    UINT8                  DevId[SVC_SWPLL_MAX_NUM_DEV];
    SWPLL_DEV_CLK_s        DevClk[SVC_SWPLL_MAX_NUM_DEV];
    UINT16                 PrintDiff[SVC_SWPLL_MAX_NUM_DEV];
    SWPLL_ADJUST_f         AdjClk[SVC_SWPLL_MAX_NUM_DEV];
    UINT8                  ClkSrc[SVC_SWPLL_MAX_NUM_VIN];
    SWPLL_INT_LOG_s        LastInt[NUM_LOG_INT];
    UINT8                  LastIntWp;
    UINT8                  FixedFrameRate[SVC_SWPLL_MAX_NUM_DEV];
    UINT8                  Valid[SVC_SWPLL_MAX_NUM_DEV];
    UINT8                  RefDevID;
    UINT32                 EnableDevMask;
} SVC_SW_PLL_CTRL_s;

static UINT8 SwPll_DebugOn = 0;
static UINT8 SwPll_Created = 0;
static SVC_DSP_INT_PORT_s g_VinIntPort GNU_SECTION_NOZEROINIT;
static SVC_DSP_INT_PORT_s g_VoutIntPort GNU_SECTION_NOZEROINIT;
static SVC_SW_PLL_CTRL_s SwPll GNU_SECTION_NOZEROINIT;

static UINT32 SwPllGetSystemTimeLsb20Us(UINT32 *pLsb20SysTime)
{
    UINT32 TimeLeft;
    UINT32 Rval = AmbaTMR_ShowTickCount(SwPll.RefClkTmrId, &TimeLeft);
    if (Rval == OK) {
        *pLsb20SysTime = 0xFFFFFU - (TimeLeft & 0xFFFFFU);
    } else {
        Rval = SVC_NG;
        SvcLog_NG(SVC_LOG_SWPLL, "AmbaTMR_ShowTickCount failed! return %u", Rval, 0);
    }
    return Rval;
}

static UINT32 SwPllGetSystemTimeU64Us(UINT64 *pU64SysTime)
{
    UINT32 Rval = AmbaKAL_MutexTake(&SwPll.TmrMutex, SWPLL_TIMEOUT);
    if (Rval == OK) {
        static UINT64 U64LastTime = 0U;
        UINT64 U64Msb44 = U64LastTime & (0xFFFFFFFFFFF00000ULL);
        UINT32 U32Lsb20;
        Rval = SwPllGetSystemTimeLsb20Us(&U32Lsb20);
        if (Rval == OK) {
            if ((U64LastTime & 0xFFFFFULL) > (UINT64)U32Lsb20) {
                U64Msb44 += 0x100000ULL;
            }
            U64LastTime = U64Msb44 | (UINT64)U32Lsb20;
            *pU64SysTime = U64LastTime;
        }
        if (Rval == OK) {
            if (AmbaKAL_MutexGive(&SwPll.TmrMutex) != OK) {
                SvcLog_NG(SVC_LOG_SWPLL, "AmbaKAL_MutexGive TmrMutex failed! return %u", Rval, 0);
            }
        }
    } else {
        SvcLog_NG(SVC_LOG_SWPLL, "AmbaKAL_MutexTake TmrMutex failed! return %u", Rval, 0);
    }
    return Rval;
}

static UINT32 SwPllGetSystemTimeU32Us(UINT32 *pU32SysTime)
{
    UINT64 U64Tick = 0U;
    UINT32 Rval = SwPllGetSystemTimeU64Us(&U64Tick);
    *pU32SysTime = (UINT32) U64Tick;
    return Rval;
}

static inline INT64 CONVERT_TIME(INT64 TimeStamp, INT64 OriTimeScale, INT64 NewTimeScale)
{
    // TimeStamp * NewTimeScale / OriTimeScale
    return ((TimeStamp / OriTimeScale) * NewTimeScale) + (((TimeStamp % OriTimeScale) * NewTimeScale) / OriTimeScale);
}

static UINT32 SwPllGetPseudoTick(UINT8 DevId, UINT32 *pPseudoTick)
{
    UINT32 Rval = AmbaKAL_MutexTake(&SwPll.DevClkMutex, SWPLL_TIMEOUT);
    if (Rval == OK) {
        if (SwPll.DevClk[DevId].pCurrSlope != NULL) { /* Check if the DevClk was used. */
            UINT64 U64CurrSysTime;
            Rval = SwPllGetSystemTimeU64Us(&U64CurrSysTime);
            if (Rval == OK) {
                const SWPLL_DEV_CLK_s *DevClk = &SwPll.DevClk[DevId];
                INT64 CurrDevTime = DevClk->SyncPtDevTime + CONVERT_TIME((INT64)U64CurrSysTime - DevClk->SyncPtSysTime, DevClk->pCurrSlope->SysOffset, DevClk->pCurrSlope->DevOffset);
                *pPseudoTick = (UINT32)CONVERT_TIME(CurrDevTime, SYSTIME_TIMESCALE, (INT64)DevClk->ClkFreq);
            }
        }
        if (AmbaKAL_MutexGive(&SwPll.DevClkMutex) != OK) {
            Rval = SVC_NG;
            SvcLog_NG(SVC_LOG_SWPLL, "AmbaKAL_MutexGive fail, line %u return %u", __LINE__, Rval);
        }
    } else {
        Rval = SVC_NG;
        SvcLog_NG(SVC_LOG_SWPLL, "AmbaKAL_MutexTake fail, line %u return %u", __LINE__, Rval);
    }
    return Rval;
}

static UINT32 GetTickVin0(UINT32 *pVin0Tick)
{
    return SwPllGetPseudoTick(SwPll.DevId[SVC_SWPLL_DEV_VIN0], pVin0Tick);
}

static UINT32 GetTickVin1(UINT32 *pVin1Tick)
{
    return SwPllGetPseudoTick(SwPll.DevId[SVC_SWPLL_DEV_VIN1], pVin1Tick);
}

static inline void PrintInt64(const char *pFmt, const INT64 *Args, UINT32 NumArg)
{
    char Buf[5][100U];
    char *pBuf[5] = {NULL};
    UINT32 I;
    if (NumArg > 5U) {
        SvcLog_NG(SVC_LOG_SWPLL, "PrintInt64 too much argument!", 0, 0);
    } else {
        for (I = 0U; I < NumArg; I++) {
            (void)AmbaUtility_Int64ToStr(&Buf[I][0], 100U, Args[I], 10U);
            pBuf[I] = &Buf[I][0];
        }
        AmbaPrint_PrintStr5(pFmt, pBuf[0], pBuf[1], pBuf[2], pBuf[3], pBuf[4]);
    }
}

void SvcSwPll_PrintINT(void)
{
    UINT8 i, j;
    INT64 I64Val[3];
    for (j = 0; j < NUM_LOG_INT; j ++) {
        if ((SwPll.LastIntWp + j) < NUM_LOG_INT) {
            i = SwPll.LastIntWp + j;
        } else {
            i = SwPll.LastIntWp + j - NUM_LOG_INT;
        }
        I64Val[0] = (INT64)SwPll.LastInt[i].DevId;
        I64Val[1] = (INT64)SwPll.LastInt[i].Time;
        I64Val[2] = (INT64)SwPll.LastInt[i].Diff;
        PrintInt64("Dev %s Time %s Diff %s", I64Val, 3);
    }
}

static void SwPllSyncDevSlope(UINT8 DevId, INT64 IdealDevTime, INT64 CurrDevTime, UINT64 U64SysTime)
{
#define SYNCPT_TOLERANCE (200LL) /* us */
#define SYNCPT_DURATION  (60 * SYSTIME_TIMESCALE) /* update slope every 1 min */
    SWPLL_DEV_CLK_s *DevClk = &SwPll.DevClk[DevId];
    SWPLL_LOG_DEV_CLK_s *Log = &SwPll.DevClkLog[DevId];
    INT64 Diff = CurrDevTime - IdealDevTime;
    Log->DevDiff = Diff;
    Log->SysDiff = CurrDevTime - (INT64)U64SysTime;
    /* Check if the difference between IdealDevTime and CurrDevTime exceed threshold */
    if (Diff > SYNCPT_TOLERANCE) {
        if (DevClk->pCurrSlope != &DevClk->MinSlope) {
            DevClk->pCurrSlope = &DevClk->MinSlope;
            DevClk->SyncPtDevTime = CurrDevTime;
            DevClk->SyncPtSysTime = (INT64)U64SysTime;
            Log->MinSlopeCnt++;
        }
    } else if (Diff < -SYNCPT_TOLERANCE){
        if (DevClk->pCurrSlope != &DevClk->MaxSlope) {
            DevClk->pCurrSlope = &DevClk->MaxSlope;
            DevClk->SyncPtDevTime = CurrDevTime;
            DevClk->SyncPtSysTime = (INT64)U64SysTime;
            Log->MaxSlopeCnt++;
        }
    } else {
        if (((DevClk->pCurrSlope == &DevClk->MinSlope) && (Diff <= 0)) || ((DevClk->pCurrSlope == &DevClk->MaxSlope) && (Diff >= 0))) {
            DevClk->pCurrSlope = &DevClk->IdealSlope;
            DevClk->SyncPtDevTime = CurrDevTime;
            DevClk->SyncPtSysTime = (INT64)U64SysTime;
            Log->IdealSlopeCnt++;
        }
    }
    /* Update SyncSlope for every SYNCPT_DURATION */
    if (((INT64)U64SysTime - DevClk->LastUpdateSysTime) > SYNCPT_DURATION) {
        DevClk->IdealSlope.DevOffset = IdealDevTime - DevClk->LastUpdateDevTime;
        DevClk->IdealSlope.SysOffset = (INT64)U64SysTime - DevClk->LastUpdateSysTime;
        DevClk->LastUpdateDevTime = IdealDevTime;
        DevClk->LastUpdateSysTime = (INT64)U64SysTime;
    }
}

static UINT32 SwPllSyncDevINT(UINT8 DevId, UINT64 U64SysTime, UINT32 Event, UINT32 LongFrameCnt)
{
/* NOTE: This is only a simple example code for generating a pseudo device tick. */
/*       APP could impletement the pseudo tick with their own method. */
    SWPLL_DEV_CLK_s *DevClk = &SwPll.DevClk[DevId];
    UINT32 Rval = AmbaKAL_MutexTake(&SwPll.DevClkMutex, SWPLL_TIMEOUT);
    if (Rval == OK) {
        if (DevClk->pCurrSlope != NULL) { /* Check if the DevClk was used. */
            if (DevClk->Idx == -1) { /* first time*/
                DevClk->InitDevTime = (INT64)U64SysTime;
                DevClk->SyncPtDevTime = DevClk->InitDevTime;
                DevClk->SyncPtSysTime = DevClk->InitDevTime;
                DevClk->LastUpdateSysTime = DevClk->InitDevTime;
                DevClk->LastUpdateDevTime = DevClk->InitDevTime;
                DevClk->Idx = 0;
            } else {
                INT64 IdealDevTime, CurrDevTime;
                /* Update index */
                if ((Event & AMBA_FTCM_EVENT_LONG_FRAME) != 0U) {
                    DevClk->Idx += (1 + (INT64)LongFrameCnt);
                } else if ((Event & AMBA_FTCM_EVENT_SHORT_FRAME) != 0U) {
                    //DevClk->Idx = DevClk->Idx;
                } else {
                    DevClk->Idx++;
                }
                /* Calculate ideal device time (by interrupt) / current device time (by slope) */
                if (((INT64)U64SysTime - DevClk->LastDevTime) > (DevClk->FrameTime * 9 / 10)) {
                    IdealDevTime = DevClk->InitDevTime + CONVERT_TIME(DevClk->Idx, DevClk->TimeScale, DevClk->FreqInTick);
                    CurrDevTime = DevClk->SyncPtDevTime + CONVERT_TIME((INT64)U64SysTime - DevClk->SyncPtSysTime, DevClk->pCurrSlope->SysOffset, DevClk->pCurrSlope->DevOffset);
                    SwPllSyncDevSlope(DevId, IdealDevTime, CurrDevTime, U64SysTime);
                }
            }
            DevClk->LastDevTime = (INT64)U64SysTime;
        }
        if (AmbaKAL_MutexGive(&SwPll.DevClkMutex) != OK) {
            Rval = SVC_NG;
            SvcLog_NG(SVC_LOG_SWPLL, "AmbaKAL_MutexGive fail, line %u return %u", __LINE__, Rval);
        }
    } else {
        Rval = SVC_NG;
        SvcLog_NG(SVC_LOG_SWPLL, "AmbaKAL_MutexTake fail, line %u return %u", __LINE__, Rval);
    }
    return Rval;
}

static UINT32 SvcSwPll_CheckINT(UINT8 DevId)
{
    UINT64 U64SysTime = 0U;
    UINT32 Event;
    UINT32 Rval = SwPllGetSystemTimeU64Us(&U64SysTime); /* dbg log */
    AMBA_FTCM_CHECK_INFO_s Info;
    if (Rval != OK) {
        SvcLog_NG(SVC_LOG_SWPLL, "SwPllGetSystemTimeU64Us fail, line %u", __LINE__, 0);
    } else {
        Rval = AmbaFTCM_Check(DevId, 0, &Event, &Info);
        if (Rval != OK) {
            SvcLog_NG(SVC_LOG_SWPLL, "AmbaFTCM_Check DevId %u fail, return %u", DevId, Rval);
        }
        if (Rval == OK) {
            Rval = SwPllSyncDevINT(DevId, U64SysTime, Event, Info.LongFrameCnt);
        }
        if (Rval == OK) {
            SWPLL_LOG_DEV_s *Log = &SwPll.DevLog[DevId];
            SWPLL_LOG_FRM_s *Frm = &Log->LastNFrms[Log->LastNFrmsIdx];
            SwPll.LastInt[SwPll.LastIntWp].DevId = DevId;
            SwPll.LastInt[SwPll.LastIntWp].Time = U64SysTime;
            SwPll.LastInt[SwPll.LastIntWp].Diff = Info.Diff;
            SwPll.LastIntWp ++;
            if (SwPll.LastIntWp == NUM_LOG_INT) {
                SwPll.LastIntWp = 0;
            }
            /* dbg log */
            Log->Cnt++;
            Frm->U64CurrTime = U64SysTime;
            Frm->Diff = Info.Diff;
            Frm->Event = Event;
            Frm->Clk = Info.ClkFreq;
            Frm->Cnt = Log->Cnt;
            Log->LastNFrmsIdx++;
            if (Log->LastNFrmsIdx == SWPLL_LOG_FRM_NUM) {
                Log->LastNFrmsIdx = 0U;
            }
            if (Event != AMBA_FTCM_EVENT_NONE) {
                SWPLL_MSG_s Msg;
                Msg.DevId = DevId;
                Msg.Cnt = SwPll.DevLog[DevId].Cnt;
                Msg.U64SysTime = U64SysTime;
                Msg.EventMask = Event;
                Msg.Info = Info;
                if (AmbaKAL_MsgQueueSend(&SwPll.MsgQue, &Msg, AMBA_KAL_NO_WAIT) != OK) {
                    SvcLog_NG(SVC_LOG_SWPLL, "AmbaKAL_MsgQueueSend fail, return %u", Rval, 0);
                }
            }
        }
        if (Rval == OK) {
            if (SwPll.PrintDiff[DevId] != 0U) {
                INT64 I64Val[2];
                I64Val[0] = (INT64)DevId;
                I64Val[1] = (INT64)Info.Diff;
                PrintInt64("Dev %s Diff %s", I64Val, 2);
                SwPll.PrintDiff[DevId] ++;
                if (SwPll.PrintDiff[DevId] == PRINT_DIFF_STOP) {
                    SwPll.PrintDiff[DevId] = 0;
                }
            }
        }
    }
    return Rval;
}

static void* SvcSwPll_VinTaskEntry(void* EntryArg)
{
    UINT32  Rval, WaitFlag, ActualFlags = 0U;
    const ULONG   *pArg;

    AmbaMisra_TouchUnused(EntryArg);
    AmbaMisra_TypeCast(&pArg, &EntryArg);

    SvcLog_OK(SVC_LOG_SWPLL, "SvcSwPll_VinTaskEntry Created", 0, 0);

    WaitFlag = (SVC_DSP_VIN0_SOF | SVC_DSP_VIN1_SOF);
    while ((*pArg) == 0U) {
        Rval = AmbaDspInt_Take(&g_VinIntPort, WaitFlag, &ActualFlags, SWPLL_TIMEOUT);
        if (Rval != OK) {
            if (SwPll_DebugOn == 1U) { 
                SvcLog_NG(SVC_LOG_SWPLL, "WaitVinInterrupt fail, return %u", Rval, 0);
            }
            continue;
        }
        if ((ActualFlags & SVC_DSP_VIN0_SOF) != 0U) {
            //SvcLog_OK(SVC_LOG_SWPLL, "vin0 dev %u", SwPll.DevId[SVC_SWPLL_DEV_VIN0], 0);
            if (INVALID_DEV_ID != SwPll.DevId[SVC_SWPLL_DEV_VIN0]) {
                Rval = SvcSwPll_CheckINT(SwPll.DevId[SVC_SWPLL_DEV_VIN0]);
                if (Rval != OK) {
                    SvcLog_NG(SVC_LOG_SWPLL, "check Vin0SOF failed!", Rval, 0);
                }
            }
        }
        if ((ActualFlags & SVC_DSP_VIN1_SOF) != 0U) {
            //SvcLog_OK(SVC_LOG_SWPLL, "vin1 dev %u", SwPll.DevId[SVC_SWPLL_DEV_VIN1], 0);
            if (INVALID_DEV_ID != SwPll.DevId[SVC_SWPLL_DEV_VIN1]) {
                Rval = SvcSwPll_CheckINT(SwPll.DevId[SVC_SWPLL_DEV_VIN1]);
                if (Rval != OK) {
                    SvcLog_NG(SVC_LOG_SWPLL, "check Vin1SOF failed!", Rval, 0);
                }
            }
        }
    }

    return NULL;
}

static void* SvcSwPll_VoutTaskEntry(void* EntryArg)
{
    UINT32  Rval, WaitFlag, ActualFlags = 0U;
    const ULONG   *pArg;

    AmbaMisra_TouchUnused(EntryArg);
    AmbaMisra_TypeCast(&pArg, &EntryArg);

    SvcLog_OK(SVC_LOG_SWPLL, "SvcSwPll_VoutTaskEntry Created", 0, 0);

    WaitFlag = (SVC_DSP_VOUT0_INT | SVC_DSP_VOUT1_INT);
    while ((*pArg) == 0U) {
        Rval = AmbaDspInt_Take(&g_VoutIntPort, WaitFlag, &ActualFlags, SWPLL_TIMEOUT);
        if (Rval != OK) {
            if (SwPll_DebugOn == 1U) { 
                SvcLog_NG(SVC_LOG_SWPLL, "WaitVoutInterrupt fail, return %u", Rval, 0);
            }
            continue;
        }
        if ((ActualFlags & SVC_DSP_VOUT0_INT) != 0U) {
            //SvcLog_OK(SVC_LOG_SWPLL, "vout0 dev %u", SwPll.DevId[SVC_SWPLL_DEV_VOUT0], 0);
            if (INVALID_DEV_ID != SwPll.DevId[SVC_SWPLL_DEV_VOUT0]) {
                Rval = SvcSwPll_CheckINT(SwPll.DevId[SVC_SWPLL_DEV_VOUT0]);
                if (Rval != OK) {
                    SvcLog_NG(SVC_LOG_SWPLL, "check Vout0SOF failed!", Rval, 0);
                }
            }
        }
        if ((ActualFlags & SVC_DSP_VOUT1_INT) != 0U) {
            //SvcLog_OK(SVC_LOG_SWPLL, "vout1 dev %u", SwPll.DevId[SVC_SWPLL_DEV_VOUT1], 0);
            if (INVALID_DEV_ID != SwPll.DevId[SVC_SWPLL_DEV_VOUT1]) {
                Rval = SvcSwPll_CheckINT(SwPll.DevId[SVC_SWPLL_DEV_VOUT1]);
                if (Rval != OK) {
                    SvcLog_NG(SVC_LOG_SWPLL, "check Vout1SOF failed!", Rval, 0);
                }
            }
        }
    }

    return NULL;
}

#if (ADJUST_AUDIO == 1)
static UINT32 SyncDevTick(UINT8 DevId, UINT64 U64SysTime, UINT64 Tick)
{
/* NOTE: This is only a simple example code for genrating a pseudo device tick. */
/*       APP could impletement the pseudo tick with their own method. */
    SWPLL_DEV_CLK_s *DevClk = &SwPll.DevClk[DevId];
    UINT32 Rval = AmbaKAL_MutexTake(&SwPll.DevClkMutex, SWPLL_TIMEOUT);
    if (Rval == OK) {
        if (DevClk->pCurrSlope != NULL) { /* Check if the DevClk was used. */
            if (DevClk->InitDevTick == -1) {    /* FirstTime */
                DevClk->InitDevTick = (INT64)Tick;
                DevClk->InitDevTime = (INT64)U64SysTime;
                DevClk->SyncPtDevTime = DevClk->InitDevTime;
                DevClk->SyncPtSysTime = DevClk->InitDevTime;
                DevClk->LastUpdateSysTime = DevClk->InitDevTime;
                DevClk->LastUpdateDevTime = DevClk->InitDevTime;
            } else {
                INT64 IdealDevTime, CurrDevTime;
                /* Calculate ideal device time (by tick) / current device time (by slope) */
                IdealDevTime = DevClk->InitDevTime + CONVERT_TIME((INT64)Tick - DevClk->InitDevTick, (INT64)DevClk->ClkFreq, SYSTIME_TIMESCALE);
                CurrDevTime = DevClk->SyncPtDevTime + CONVERT_TIME((INT64)U64SysTime - DevClk->SyncPtSysTime, DevClk->pCurrSlope->SysOffset, DevClk->pCurrSlope->DevOffset);
                SwPllSyncDevSlope(DevId, IdealDevTime, CurrDevTime, U64SysTime);
            }
        }
        if (AmbaKAL_MutexGive(&SwPll.DevClkMutex) != OK) {
            Rval = SVC_NG;
            SvcLog_NG(SVC_LOG_SWPLL, "AmbaKAL_MutexGive fail, line %u return %u", __LINE__, Rval);
        }
    } else {
        Rval = SVC_NG;
        SvcLog_NG(SVC_LOG_SWPLL, "AmbaKAL_MutexTake fail, line %u return %u", __LINE__, Rval);
    }
    return Rval;
}

static UINT32 CheckAudio(const AUDIO_MSG_s *pAudioMsg)
{
    UINT32 Rval = OK;
    if (SwPll.Valid[SwPll.DevId[SVC_SWPLL_DEV_AUDIO]] == 1U) {
        AMBA_FTCM_CHECK_INFO_s Info = {0};
        UINT64 U64SysTime = 0U;
        UINT32 Event, AudioTick;
        Rval = SwPllGetPseudoTick(SwPll.DevId[SVC_SWPLL_DEV_AUDIO], &AudioTick);
        if (Rval == OK) {
            Rval = AmbaFTCM_Check(SwPll.DevId[SVC_SWPLL_DEV_AUDIO], AudioTick, &Event, &Info);
            if (Rval == OK) {
                /* Convert U32SysTime to UINT64 */
                Rval = SwPllGetSystemTimeU64Us(&U64SysTime);
                if (Rval == OK) {
                    UINT64 U64Msb44 = U64SysTime & (0xFFFFFFFFFFF00000ULL);
                    UINT64 U64Lsb20 = (UINT64)pAudioMsg->Lsb20SysTime;
                    if ((U64SysTime & 0xFFFFFULL) < U64Lsb20) {
                        U64Msb44 -= 0x100000ULL;
                    }
                    U64SysTime = U64Msb44 | U64Lsb20;
                    Rval = SyncDevTick(SwPll.DevId[SVC_SWPLL_DEV_AUDIO], U64SysTime, pAudioMsg->AudioTick);
                } else {
                    SvcLog_NG(SVC_LOG_SWPLL, "SwPllGetSystemTimeU64Us fail, line %u return %u", __LINE__, Rval);
                }
            } else {
                SvcLog_NG(SVC_LOG_SWPLL, "AmbaFTCM_Check fail, line %u return %u", __LINE__, Rval);
            }
            /* dbg log */
            if (Rval == OK) {
                SWPLL_LOG_DEV_s *Log = &SwPll.DevLog[SwPll.DevId[SVC_SWPLL_DEV_AUDIO]];
                SWPLL_LOG_FRM_s *Frm = &Log->LastNFrms[Log->LastNFrmsIdx];
                SwPll.LastInt[SwPll.LastIntWp].DevId = SwPll.DevId[SVC_SWPLL_DEV_AUDIO];
                SwPll.LastInt[SwPll.LastIntWp].Time = U64SysTime;
                SwPll.LastInt[SwPll.LastIntWp].Diff = Info.Diff;
                SwPll.LastIntWp ++;
                if (SwPll.LastIntWp == NUM_LOG_INT) {
                    SwPll.LastIntWp = 0;
                }
                Log->Cnt++;
                Frm->U64CurrTime = U64SysTime;
                Frm->Diff = Info.Diff;
                Frm->Event = Event;
                Frm->Clk = Info.ClkFreq;
                Frm->Cnt = pAudioMsg->AudioTick;
                Log->LastNFrmsIdx++;
                if (Log->LastNFrmsIdx == SWPLL_LOG_FRM_NUM) {
                    Log->LastNFrmsIdx = 0U;
                }
            }
            if (Rval == OK) {
                if (Event != AMBA_FTCM_EVENT_NONE) {
                    SWPLL_MSG_s Msg = {0};
                    Msg.DevId = SwPll.DevId[SVC_SWPLL_DEV_AUDIO];
                    Msg.Cnt = SwPll.DevLog[SwPll.DevId[SVC_SWPLL_DEV_AUDIO]].Cnt;
                    //Msg.U64SysTime = U64SysTime;
                    Msg.EventMask = Event;
                    Msg.Info = Info;
                    if (AmbaKAL_MsgQueueSend(&SwPll.MsgQue, &Msg, AMBA_KAL_NO_WAIT) != OK) {
                        SvcLog_NG(SVC_LOG_SWPLL, "AmbaKAL_MsgQueueSend fail, line %u return %u", __LINE__, Rval);
                    }
                }
            }
        } else {
            SvcLog_NG(SVC_LOG_SWPLL, "SwPllGetPseudoTick fail, line %u return %u", __LINE__, Rval);
        }
    }
    return Rval;
}

static void* SvcSwPll_AudioTaskEntry(void* EntryArg)
{
    UINT32 Rval;
    AUDIO_MSG_s Msg;

    AmbaMisra_TouchUnused(EntryArg);
    SvcLog_OK(SVC_LOG_SWPLL, "SvcSwPll_AudioTaskEntry Created", 0, 0);

    for (;;) {
        Rval = AmbaKAL_MsgQueueReceive(&SwPll.AudioMsgQue, &Msg, AMBA_KAL_WAIT_FOREVER);
        if (Rval == OK) {
            Rval = CheckAudio(&Msg);
        } else {
            SvcLog_NG(SVC_LOG_SWPLL, "AmbaKAL_MsgQueueReceive for audio failed, return %u", Rval, 0);
            Rval = SVC_NG;
        }
    }

    return NULL;
}

static UINT32 LvRawDataRdyHandler(const void *EventInfo)
{
    UINT32 Rval, Lsb20SysTime;
    AUDIO_MSG_s Msg;
    const AMBA_DSP_RAW_DATA_RDY_s *RawData = NULL;
    AmbaMisra_TypeCast(&RawData, &EventInfo);
    Rval = SwPllGetSystemTimeLsb20Us(&Lsb20SysTime);
    if (Rval == OK) {
        Msg.AudioTick = RawData->CapPts;
        Msg.Lsb20SysTime = Lsb20SysTime;
        Rval = AmbaKAL_MsgQueueSend(&SwPll.AudioMsgQue, &Msg, AMBA_KAL_NO_WAIT);
        if (Rval != OK) {
            Rval = SVC_NG;
            SvcLog_NG(SVC_LOG_SWPLL, "AmbaKAL_MsgQueueSend line %u failed, return %u", __LINE__, Rval);
        }
    } else {
        Rval = SVC_NG;
        SvcLog_NG(SVC_LOG_SWPLL, "SwPllGetSystemTimeLsb20Us line %u failed, return %u", __LINE__, Rval);
    }
    return Rval;
}
#endif

static inline void PrintFrm(UINT8 DevId, const SWPLL_LOG_FRM_s *LogFrm)
{
    INT64 I64Val[5] = {0};
    I64Val[0] = (INT64)DevId;
    I64Val[1] = (INT64)LogFrm->Cnt;
    I64Val[2] = (INT64)LogFrm->U64CurrTime;
    I64Val[3] = (INT64)LogFrm->Event;
    I64Val[4] = (INT64)LogFrm->Clk;
    PrintInt64("[LastNFrms] Dev %s Cnt %s Time %s Event %s Clk %s", I64Val, 5);
}

static void DumpLastNFrms(UINT8 DevId)
{
    UINT32 I, Idx = SwPll.DevLog[DevId].LastNFrmsIdx;
    for (I = 0U; I < SWPLL_LOG_FRM_NUM; I++) {
        PrintFrm(DevId, &SwPll.DevLog[DevId].LastNFrms[Idx]);
        Idx++;
        if (Idx == SWPLL_LOG_FRM_NUM) {
            Idx = 0U;
        }
    }
}

static UINT32 Vin0_Adjust (UINT32 TargetClk)
{
    UINT32 ActualClk, OrgClk;
    UINT32 Rval = AmbaSYS_GetClkFreq(SwPll.ClkSrc[0], &OrgClk);
    if (Rval != OK) {
        SvcLog_NG(SVC_LOG_SWPLL, "AmbaSYS_GetClkFreq %u for vin0 fail, return %u", TargetClk, Rval);
    } else {
        if (SwPll_DebugOn == 1U) {        
            SvcLog_OK(SVC_LOG_SWPLL, "Vin0 OrgClk %u", OrgClk, 0);
        }
    }
    if (Rval == OK) {
        Rval = SvcIPC_SetClkFreq(SwPll.ClkSrc[0], TargetClk, &ActualClk);
        if (Rval != OK) {
            SvcLog_NG(SVC_LOG_SWPLL, "SvcIPC_SetClkFreq %u for vin0 fail, return %u", TargetClk, Rval);
        } else {
            if (SwPll_DebugOn == 1U) {        
                SvcLog_OK(SVC_LOG_SWPLL, "Vin0_Adjust TargetClk %u ActualClk %u", TargetClk, ActualClk);
            }
        }
    }
    return Rval;
}

static UINT32 Vin1_Adjust (UINT32 TargetClk)
{
    UINT32 ActualClk, OrgClk;
    UINT32 Rval = AmbaSYS_GetClkFreq(SwPll.ClkSrc[1], &OrgClk);
    if (Rval != OK) {
        SvcLog_NG(SVC_LOG_SWPLL, "AmbaSYS_GetClkFreq %u for vin1 fail, return %u", TargetClk, Rval);
    } else {
        if (SwPll_DebugOn == 1U) {        
            SvcLog_OK(SVC_LOG_SWPLL, "Vin1 OrgClk %u", OrgClk, 0);
        }
    }
    if (Rval == OK) {
        Rval = SvcIPC_SetClkFreq(SwPll.ClkSrc[1], TargetClk, &ActualClk);
        if (Rval != OK) {
            SvcLog_NG(SVC_LOG_SWPLL, "SvcIPC_SetClkFreq %u for vin1 fail, return %u", TargetClk, Rval);
        } else {
            if (SwPll_DebugOn == 1U) {        
                SvcLog_OK(SVC_LOG_SWPLL, "Vin1_Adjust TargetClk %u ActualClk %u", TargetClk, ActualClk);
            }
        }
    }
    return Rval;
}

static UINT32 Vout0_Adjust (UINT32 TargetClk)
{
    UINT32 ActualClk, OrgClk;
    UINT32 Rval = AmbaSYS_GetClkFreq(AMBA_SYS_CLK_VID_OUT0, &OrgClk);
    if (Rval != OK) {
        SvcLog_NG(SVC_LOG_SWPLL, "AmbaSYS_GetClkFreq %u for vout0 fail, return %u", TargetClk, Rval);
    } else {
        if (SwPll_DebugOn == 1U) {        
            SvcLog_OK(SVC_LOG_SWPLL, "Vout0 OrgClk %u", OrgClk, 0);
        }
    }
    if (Rval == OK) {
        Rval = SvcIPC_SetClkFreq(AMBA_SYS_CLK_VID_OUT0, TargetClk, &ActualClk);
        if (Rval != OK) {
            SvcLog_NG(SVC_LOG_SWPLL, "SvcIPC_SetClkFreq %u for vout0 fail, return %u", TargetClk, Rval);
        } else {
            if (SwPll_DebugOn == 1U) {        
                SvcLog_OK(SVC_LOG_SWPLL, "Vout0_Adjust TargetClk %u ActualClk %u", TargetClk, ActualClk);
            }
        }
    }
    return Rval;
}

static UINT32 Vout1_Adjust (UINT32 TargetClk)
{
    UINT32 ActualClk, OrgClk;
    UINT32 Rval = AmbaSYS_GetClkFreq(AMBA_SYS_CLK_VID_OUT1, &OrgClk);
    if (Rval != OK) {
        SvcLog_NG(SVC_LOG_SWPLL, "AmbaSYS_GetClkFreq %u for vout1 fail, return %u", TargetClk, Rval);
    } else {
        if (SwPll_DebugOn == 1U) {
            SvcLog_OK(SVC_LOG_SWPLL, "Vout1 OrgClk %u", OrgClk, 0);
        }
    }
    if (Rval == OK) {
        Rval = SvcIPC_SetClkFreq(AMBA_SYS_CLK_VID_OUT1, TargetClk, &ActualClk);
        if (Rval != OK) {
            SvcLog_NG(SVC_LOG_SWPLL, "SvcIPC_SetClkFreq %u for vout1 fail, return %u", TargetClk, Rval);
        } else {
            if (SwPll_DebugOn == 1U) {
                SvcLog_OK(SVC_LOG_SWPLL, "Vout1_Adjust TargetClk %u ActualClk %u", TargetClk, ActualClk);
            }
        }
    }
    return Rval;
}

#if (ADJUST_AUDIO == 1)
static UINT32 Audio_Adjust (UINT32 TargetClk)
{
    UINT32 ActualClk, OrgClk;
    UINT32 Rval = AmbaSYS_GetClkFreq(AMBA_SYS_CLK_AUD_0, &OrgClk);
    if (Rval != OK) {
        SvcLog_NG(SVC_LOG_SWPLL, "AmbaSYS_GetClkFreq %u for audio fail, return %u", Rval, 0);
    } else {
        if (SwPll_DebugOn == 1U) {
            SvcLog_OK(SVC_LOG_SWPLL, "Audio OrgClk %u", OrgClk, 0);
        }
    }
    if (Rval == OK) {
        Rval = SvcIPC_SetClkFreq(AMBA_SYS_CLK_AUD_0, TargetClk, &ActualClk);
        if (Rval != OK) {
            SvcLog_NG(SVC_LOG_SWPLL, "SvcIPC_SetClkFreq %u for audio fail, return %u", TargetClk, Rval);
        } else {
            if (SwPll_DebugOn == 1U) {
                SvcLog_OK(SVC_LOG_SWPLL, "Audio_Adjust TargetClk %u ActualClk %u", TargetClk, ActualClk);
            }
        }
    }
    return Rval;
}
#endif

static UINT32 AdjustClk(UINT8 DevId, UINT32 ClkFreq)
{
    UINT32 Rval = SVC_OK;
    if (SwPll.AdjClk[DevId] != NULL) {
        Rval = SwPll.AdjClk[DevId](ClkFreq);
    } else {
        SvcLog_NG(SVC_LOG_SWPLL, "AdjustClk for Dev %u is NULL", DevId, 0);
        Rval = SVC_NG;
    }
    return Rval;
}

static void* SvcSwPll_AdjTaskEntry(void* EntryArg)
{
    UINT32 Rval = OK;
    SWPLL_MSG_s Msg = {0};
    INT64 I64Val[5] = {0};
    const ULONG  *pArg;

    AmbaMisra_TouchUnused(EntryArg);
    AmbaMisra_TypeCast(&pArg, &EntryArg);

    while ((*pArg) == 0U) {
        Rval = AmbaKAL_MsgQueueReceive(&SwPll.MsgQue, &Msg, AMBA_KAL_WAIT_FOREVER);
        if (Rval != OK) {
            SvcLog_NG(SVC_LOG_SWPLL, "AmbaKAL_MsgQueueReceive fail, return %u", Rval, 0);
        } else {
            if ((Msg.EventMask & AMBA_FTCM_EVENT_ADJUST_CLK) != 0U) {
                I64Val[0] = (INT64)Msg.DevId;
                I64Val[1] = (INT64)Msg.Info.ClkFreq;
                I64Val[2] = (INT64)Msg.Cnt;
                I64Val[3] = (INT64)Msg.U64SysTime;
                I64Val[4] = (INT64)Msg.Info.Diff;
                if (SwPll_DebugOn == 1U) {
                    PrintInt64("Dev %s AMBA_FTCM_EVENT_ADJUST_CLK! ClkFreq %s Cnt %s Time %s Diff %s", I64Val, 5);
                }
                //SvcLog_OK(SVC_LOG_SWPLL, "AMBA_FTCM_EVENT_ADJUST_CLK DevId %u Cnt %u", Msg.DevId, Msg.Cnt);
                //SvcLog_OK(SVC_LOG_SWPLL, "                           ClkFreq %u Diff %u", Msg.Info.ClkFreq, Msg.Info.Diff);
                Rval = AdjustClk(Msg.DevId, Msg.Info.ClkFreq);
                if (Rval != OK) {
                    AmbaPrint_PrintUInt5("[Error] FtcmUT(%u) FTCMAdjustClk failed!", __LINE__, 0U, 0U, 0U, 0U);
                } else {
                    if (SwPll_DebugOn == 1U) {
                        SwPll.PrintDiff[Msg.DevId] = 1;
                    }
                }
            }
            if ((Msg.EventMask & AMBA_FTCM_EVENT_LONG_FRAME) != 0U) {
                I64Val[0] = (INT64)Msg.DevId;
                I64Val[1] = (INT64)Msg.Info.LongFrameCnt;
                I64Val[2] = (INT64)Msg.Cnt;
                I64Val[3] = (INT64)Msg.U64SysTime;
                I64Val[4] = (INT64)Msg.Info.Diff;
                PrintInt64("Dev %s AMBA_FTCM_EVENT_LONG_FRAME! LongFrmCnt %s Cnt %s Time %s Diff %s", I64Val, 5);
                //SvcLog_OK(SVC_LOG_SWPLL, "AMBA_FTCM_EVENT_LONG_FRAME DevId %u LongFrameCnt %u", Msg.DevId, Msg.Info.LongFrameCnt);
                DumpLastNFrms(Msg.DevId);/* dbg log */
            }
            if ((Msg.EventMask & AMBA_FTCM_EVENT_SHORT_FRAME) != 0U) {
                I64Val[0] = (INT64)Msg.DevId;
                I64Val[1] = (INT64)Msg.Cnt;
                I64Val[2] = (INT64)Msg.U64SysTime;
                I64Val[3] = (INT64)Msg.Info.Diff;
                PrintInt64("Dev %s AMBA_FTCM_EVENT_SHORT_FRAME! Cnt %s Time %s Diff %s", I64Val, 4);
                //SvcLog_OK(SVC_LOG_SWPLL, "AMBA_FTCM_EVENT_SHORT_FRAME DevId %u Cnt %u", Msg.DevId, Msg.Cnt);
                DumpLastNFrms(Msg.DevId);/* dbg log */
            }
            if ((Msg.EventMask & AMBA_FTCM_EVENT_INVALID_CONFIG) != 0U) {
                I64Val[0] = (INT64)Msg.DevId;
                I64Val[1] = (INT64)Msg.Cnt;
                I64Val[2] = (INT64)Msg.U64SysTime;
                I64Val[3] = (INT64)Msg.Info.Diff;
                I64Val[4] = (INT64)Msg.Info.Info;
                PrintInt64("Dev %s AMBA_FTCM_EVENT_INVALID_CONFIG! Cnt %s Time %s Diff %s InvalidClkFreq %s", I64Val, 5);
                //SvcLog_OK(SVC_LOG_SWPLL, "AMBA_FTCM_EVENT_INVALID_CONFIG DevId %u Cnt %u", Msg.DevId, Msg.Cnt);
                //DumpLastNFrms(Msg.DevId);
            }
        }
    }

    return NULL;
}

static UINT32 ResetDevClk(UINT8 DevId, UINT32 ClkFreq, const AMBA_FTCM_FRAME_RATE_s *pFrameRate)
{
#define SYNCPT_ADJVAL    (1000) /* us, slope = 1 +- (1000/1000000) */

    UINT32 Rval;
    /* Note: The min/max slope should be able to catch up Max/MinClk when adjusting.*/
    /*       It's not a common case, since only RefClk need a device tick (and which should not be adjusted). */
    Rval = AmbaKAL_MutexTake(&SwPll.DevClkMutex, SWPLL_TIMEOUT);
    if (Rval == OK) {
        SWPLL_DEV_CLK_s *DevClk = &SwPll.DevClk[DevId];
        AmbaSvcWrap_MisraMemset(&SwPll.DevClkLog[DevId], 0, sizeof(SWPLL_LOG_DEV_CLK_s));
        DevClk->ClkFreq = (INT64)ClkFreq;
        if (pFrameRate != NULL) {
            DevClk->FreqInTick = (INT64)pFrameRate->UnitsInTick * SYSTIME_TIMESCALE;
            DevClk->TimeScale = (INT64)pFrameRate->TimeScale;
            DevClk->FrameTime = DevClk->FreqInTick / DevClk->TimeScale;
        }
        DevClk->Idx = -1;
        DevClk->InitDevTick = -1;
        DevClk->LastDevTime = 0;
        DevClk->SyncPtDevTime = 0;
        DevClk->SyncPtSysTime = 0;
        DevClk->IdealSlope.DevOffset = 1;
        DevClk->IdealSlope.SysOffset = 1;
        DevClk->MinSlope.DevOffset = SYSTIME_TIMESCALE - SYNCPT_ADJVAL;
        DevClk->MinSlope.SysOffset = SYSTIME_TIMESCALE;
        DevClk->MaxSlope.DevOffset = SYSTIME_TIMESCALE + SYNCPT_ADJVAL;
        DevClk->MaxSlope.SysOffset = SYSTIME_TIMESCALE;
        DevClk->pCurrSlope = &DevClk->IdealSlope;
        if (AmbaKAL_MutexGive(&SwPll.DevClkMutex) != OK) {
            Rval = SVC_NG;
            SvcLog_NG(SVC_LOG_SWPLL, "AmbaKAL_MutexGive fail, line %u return %u", __LINE__, Rval);
        }
    } else {
        Rval = SVC_NG;
        SvcLog_NG(SVC_LOG_SWPLL, "AmbaKAL_MutexTake fail, line %u return %u", __LINE__, Rval);
    }
    return Rval;
}

static UINT32 ResetDev(UINT8 DevId)
{
    UINT32 Rval;
    AmbaSvcWrap_MisraMemset(&SwPll.DevLog[DevId], 0, sizeof(SWPLL_LOG_DEV_s));
    SwPll.DevLog[DevId].Cnt = 0xFFFFFFFFFFFFFFFFU; //-1
    Rval = ResetDevClk(DevId, SwPll.FtcmCfg.DevCfg[DevId].InitClkFreq, &SwPll.FtcmCfg.DevCfg[DevId].FrameRate);
    return Rval;
}

UINT32 SvcSwPll_Enable(UINT32 DevMask, UINT8 Enable)
{
    UINT32 Rval = 0U;
    UINT8 I;
    if (Enable != 0U) {
        for (I = 0U; I < SwPll.FtcmCfg.NumDev; I++) {
            if ((DevMask & ((UINT32)1U << I)) != 0U) {
                Rval = ResetDev(I);
                if (Rval != OK) {
                    break;
                }
            }
        }
        if (Rval == OK) {
            Rval = AmbaFTCM_Enable(DevMask);
            if (Rval != OK) {
                SvcLog_NG(SVC_LOG_SWPLL, "AmbaFTCM_Enable DevMask 0x%x, return 0x%x", DevMask, Rval);
            } else {
                SwPll.EnableDevMask |= DevMask;
                SvcLog_OK(SVC_LOG_SWPLL, "EnableDevMask 0x%x", SwPll.EnableDevMask, 0);
            }
        }
    } else {
        Rval = AmbaFTCM_Disable(DevMask);
        if (Rval != OK) {
            SvcLog_NG(SVC_LOG_SWPLL, "AmbaFTCM_Disable DevMask 0x%x, return 0x%x", DevMask, Rval);
        } else {
            SwPll.EnableDevMask &= ~DevMask;
            SvcLog_OK(SVC_LOG_SWPLL, "EnableDevMask 0x%x", SwPll.EnableDevMask, 0);
        }
    }
    return Rval;
}

/**
* debug control of swpll module
* @param [in] On debug control, 0-off, 1-on
* @return none
*/
void SvcSwPll_Debug(UINT8 On)
{
    SwPll_DebugOn = On;
    SvcLog_OK(SVC_LOG_SWPLL, "SvcSwPll_Debug %u", On, 0);
}

/**
* create of swpll module
* @param [in] pCfg configuration of swpll
* @return 0-OK, 1-NG
*/
UINT32 SvcSwPll_Create(const SVC_SW_PLL_CFG_s *pCfg)
{
    UINT32          Rval;
    static UINT8    SvcSwPllAdj[SVC_SW_PLL_STACK_SIZE] GNU_SECTION_NOZEROINIT;
    static UINT8    SvcSwPllVin[SVC_SW_PLL_STACK_SIZE] GNU_SECTION_NOZEROINIT;
    static UINT8    SvcSwPllVout[SVC_SW_PLL_STACK_SIZE] GNU_SECTION_NOZEROINIT;
    static char     MsgQueName[] = "SwPll_MsgQue";
    static char     MutexName[] = "SwPll_DevClkMutex";
    static char     TmrMutexName[] = "SwPll_TmrMutex";
#if (ADJUST_AUDIO == 1)
    static UINT8    SvcSwPllAudio[SVC_SW_PLL_STACK_SIZE] GNU_SECTION_NOZEROINIT;
    static char     AudioMqueueName[] = "SwPll_AudioMsgQue";
#endif

    if (SwPll_Created == 0U) {
        AmbaSvcWrap_MisraMemset(&SwPll, 0x0, sizeof(SwPll));

        Rval = AmbaKAL_MsgQueueCreate(&SwPll.MsgQue, MsgQueName, sizeof(SWPLL_MSG_s), SwPll.MsgQueBuf, sizeof(SwPll.MsgQueBuf));
        if (OK != Rval) {
            SvcLog_NG(SVC_LOG_SWPLL, "MsgQue created failed! return %u", Rval, 0U);
        }
        if (Rval == OK) {
            /* get timer for FTCM */
            Rval = AmbaTMR_FindAvail(&SwPll.FtcmCfg.TimerId, 5000U);
            if (Rval != OK) {
                SvcLog_NG(SVC_LOG_SWPLL, "AmbaTMR_FindAvail for FTCM failed! return %u", Rval, 0U);
            } else {
                SvcLog_OK(SVC_LOG_SWPLL, "FtcmCfg.TimerId %u", SwPll.FtcmCfg.TimerId, 0U);
                Rval = AmbaTMR_Acquire(SwPll.FtcmCfg.TimerId, 5000U);
                if (Rval != OK) {
                    SvcLog_NG(SVC_LOG_SWPLL, "AmbaTMR_Acquire TimerId %u for FTCM failed! return %u", SwPll.FtcmCfg.TimerId, Rval);
                }
            }
            /* get timer as reference clock */
            Rval = AmbaTMR_FindAvail(&SwPll.RefClkTmrId, 5000);
            if (Rval != OK) {
                SvcLog_NG(SVC_LOG_SWPLL, "AmbaTMR_FindAvail as reference failed! return %u", Rval, 0U);
            } else {
                SvcLog_OK(SVC_LOG_SWPLL, "RefClkTmrId %u", SwPll.RefClkTmrId, 0U);
                Rval = AmbaTMR_Acquire(SwPll.RefClkTmrId, 5000);
                if (Rval != OK) {
                    SvcLog_NG(SVC_LOG_SWPLL, "AmbaTMR_Acquire TimerId %u as reference failed! return %u", SwPll.RefClkTmrId, Rval);
                }
                if (Rval == OK) {
                    Rval = AmbaTMR_Config(SwPll.RefClkTmrId, 1000000, 0x100000);
                    if (Rval != OK) {
                        SvcLog_NG(SVC_LOG_SWPLL, "AmbaTMR_Config TimerId %u as reference failed! return %u", SwPll.RefClkTmrId, Rval);
                    }
                }
                if (Rval == OK) {
                    Rval = AmbaTMR_Start(SwPll.RefClkTmrId, 0x100000);
                    if (Rval != OK) {
                        SvcLog_NG(SVC_LOG_SWPLL, "AmbaTMR_Start TimerId %u as reference failed! return %u", SwPll.RefClkTmrId, Rval);
                    }
                }
            }
            if (Rval == OK) {
                Rval = AmbaKAL_MutexCreate(&SwPll.DevClkMutex, MutexName);
                if (Rval != OK) {
                    SvcLog_NG(SVC_LOG_SWPLL, "AmbaKAL_MutexCreate SwPll.DevClkMutex failed! return %u", Rval, 0);
                } else {
                    Rval = AmbaKAL_MutexCreate(&SwPll.TmrMutex, TmrMutexName);
                    if (Rval != OK) {
                        SvcLog_NG(SVC_LOG_SWPLL, "AmbaKAL_MutexCreate SwPll.TmrMutex failed! return %u", Rval, 0);
                    }
                }
            }
            if (Rval == OK) {
                const SVC_RES_CFG_s  *pResCfg = SvcResCfg_Get();
                UINT32 VinID[SVC_SWPLL_MAX_NUM_VIN], VinNum, i, DevMsk = 0, SensorIdx;
                Rval = SvcResCfg_GetVinIDs(VinID, &VinNum);
                if (Rval != OK) {
                    SvcLog_NG(SVC_LOG_SWPLL, "SvcResCfg_GetVinIDs failed! return %u", Rval, 0);
                } else {
                    UINT8 NumVinSkipped = 0, DevId = 0;
                    for (i = 0; i < SVC_SWPLL_MAX_NUM_DEV; i ++) {
                        SwPll.DevId[i] = INVALID_DEV_ID;
                    }
                    SwPll.RefDevID = SVC_SWPLL_MAX_NUM_DEV; // mean no reference device, set to hardware timer.
                    for (i = 0; i < VinNum; i ++) {
                        if (pResCfg->VinCfg[VinID[i]].SkipSwpll == 1U) {
                            NumVinSkipped ++;
                        } else {
                            SwPll.DevId[VinID[i]] = (UINT8)DevId;
                            Rval = SvcResCfg_GetSerdesIdxOfSensorIdx(VinID[i], 0, &SensorIdx);
                            if (Rval == OK) {
                                if (SensorIdx != 0xDeadbeafU) {
                                    Rval = AmbaSYS_GetClkFreq(AMBA_SYS_CLK_VID_IN0 + VinID[i], &SwPll.FtcmCfg.DevCfg[DevId].InitClkFreq);
                                    if (Rval != OK) {
                                        SvcLog_NG(SVC_LOG_SWPLL, "AmbaSYS_GetClkFreq %u failed! return %u", AMBA_SYS_CLK_VID_IN0 + VinID[i], Rval);
                                    }
                                } else {
                                    Rval = AmbaSYS_GetClkFreq(AMBA_SYS_CLK_REF_OUT0 + VinID[i], &SwPll.FtcmCfg.DevCfg[DevId].InitClkFreq);
                                    if (Rval != OK) {
                                        SvcLog_NG(SVC_LOG_SWPLL, "AmbaSYS_GetClkFreq %u failed! return %u", AMBA_SYS_CLK_REF_OUT0 + VinID[i], Rval);
                                    }
                                }
                                if (Rval == OK) {
                                    SwPll.FtcmCfg.DevCfg[DevId].MaxClkFreq = SwPll.FtcmCfg.DevCfg[DevId].InitClkFreq + (SwPll.FtcmCfg.DevCfg[DevId].InitClkFreq / 10000U);
                                    SwPll.FtcmCfg.DevCfg[DevId].MinClkFreq = SwPll.FtcmCfg.DevCfg[DevId].InitClkFreq - (SwPll.FtcmCfg.DevCfg[DevId].InitClkFreq / 10000U);
                                    SwPll.FtcmCfg.DevCfg[DevId].FrameRate.TimeScale = pResCfg->VinCfg[VinID[i]].FrameRate.TimeScale;
                                    SwPll.FtcmCfg.DevCfg[DevId].FrameRate.UnitsInTick = pResCfg->VinCfg[VinID[i]].FrameRate.NumUnitsInTick;
                                    SwPll.FtcmCfg.DevCfg[DevId].Delay = (INT32)DevId * 4000;
                                    SwPll.FtcmCfg.DevCfg[DevId].Tolerance = 500;
                                    if (VinID[i] == 0U) {
                                        SwPll.AdjClk[DevId] = Vin0_Adjust;
                                        if (SensorIdx != 0xDeadbeafU) {
                                            SwPll.ClkSrc[0] = AMBA_SYS_CLK_VID_IN0; // master sync 0
                                        } else {
                                            SwPll.ClkSrc[0] = AMBA_SYS_CLK_REF_OUT0; // clk_si
                                        }
                                        //SwPll.FtcmCfg.DevCfg[DevId].Delay = 2000; // test
                                    } else {
                                        SwPll.AdjClk[DevId] = Vin1_Adjust;
                                        if (SensorIdx != 0xDeadbeafU) {
                                            SwPll.ClkSrc[1] = AMBA_SYS_CLK_VID_IN1; // master sync 1
                                        } else {
                                            SwPll.ClkSrc[1] = AMBA_SYS_CLK_REF_OUT1; // clk_si2
                                        }
                                        //SwPll.FtcmCfg.DevCfg[DevId].Delay = 0; // test
                                    }
                                    DevMsk |= (UINT32)1 << (DevId);
                                    SwPll.Valid[DevId] = 1;
                                    if (pResCfg->VinCfg[VinID[i]].FixedFrameRate == 1U) { // for non-adjusted device
                                        if (DevId == 0U) {
                                            SwPll.FtcmCfg.DevCfg[DevId].Delay = 0;
                                        } else {
                                            SwPll.FtcmCfg.DevCfg[DevId].Delay = 0x7FFFFFFF;
                                        }
                                        SwPll.FixedFrameRate[DevId] = 1;
                                        if (SwPll.RefDevID == SVC_SWPLL_MAX_NUM_DEV) {
                                            SwPll.FtcmCfg.RefClk.ClkFreq = SwPll.FtcmCfg.DevCfg[DevId].InitClkFreq;
                                            SwPll.RefDevID = (UINT8)DevId;
                                            if (VinID[i] == 0U) {
                                                SwPll.FtcmCfg.RefClk.GetTick = GetTickVin0;
                                            } else {
                                                SwPll.FtcmCfg.RefClk.GetTick = GetTickVin1;
                                            }
                                            if (SwPll_DebugOn == 1U) {
                                                SvcLog_OK(SVC_LOG_SWPLL, "RefClk %u Dev %u", SwPll.FtcmCfg.RefClk.ClkFreq, DevId);
                                            }
                                        } else {
                                            SwPll.Valid[DevId] = 0;
                                            SvcLog_OK(SVC_LOG_SWPLL, "Warning: multiple devices non-adjustable, Dev %u", DevId, 0);
                                        }
                                    }
                                    if (SwPll_DebugOn == 1U) {
                                        SvcLog_OK(SVC_LOG_SWPLL, "Dev %u VinID %u", DevId, VinID[i]);
                                        SvcLog_OK(SVC_LOG_SWPLL, "  clk %u", SwPll.FtcmCfg.DevCfg[DevId].InitClkFreq, 0);
                                        SvcLog_OK(SVC_LOG_SWPLL, "  MaxClkFreq %u MinClkFreq %u", SwPll.FtcmCfg.DevCfg[DevId].MaxClkFreq, SwPll.FtcmCfg.DevCfg[DevId].MinClkFreq);
                                        SvcLog_OK(SVC_LOG_SWPLL, "  TimeScale %u UnitsInTick %u", SwPll.FtcmCfg.DevCfg[DevId].FrameRate.TimeScale, SwPll.FtcmCfg.DevCfg[DevId].FrameRate.UnitsInTick);
                                        SvcLog_OK(SVC_LOG_SWPLL, "  Delay %u Tolerance %u", (UINT32)SwPll.FtcmCfg.DevCfg[DevId].Delay, SwPll.FtcmCfg.DevCfg[DevId].Tolerance);
                                        SvcLog_OK(SVC_LOG_SWPLL, "  clk src id 0x%x", (UINT32)SwPll.ClkSrc[VinID[i]], 0);
                                    }
                                    DevId ++;
                                }
                            }
                        }
                    }
                    VinNum -= NumVinSkipped;
                    SwPll.FtcmCfg.NumDev = (UINT8)VinNum + (UINT8)pResCfg->DispNum;
                    if (Rval == OK) {
                        for (i = 0; i < pResCfg->DispNum; i ++) {
                            SwPll.DevId[SVC_SWPLL_MAX_NUM_VIN + pResCfg->DispStrm[i].VoutID] = (UINT8)(VinNum + i);
                            Rval = AmbaSYS_GetClkFreq(AMBA_SYS_CLK_VID_OUT0 + pResCfg->DispStrm[i].VoutID, &SwPll.FtcmCfg.DevCfg[VinNum + i].InitClkFreq);
                            if (Rval != OK) {
                                SvcLog_NG(SVC_LOG_SWPLL, "AmbaSYS_GetClkFreq %u failed! return %u", AMBA_SYS_CLK_VID_OUT0 + pResCfg->DispStrm[i].VoutID, Rval);
                                break;
                            }
                            SwPll.FtcmCfg.DevCfg[VinNum + i].MaxClkFreq = SwPll.FtcmCfg.DevCfg[VinNum + i].InitClkFreq + (SwPll.FtcmCfg.DevCfg[VinNum + i].InitClkFreq / 10000U);
                            SwPll.FtcmCfg.DevCfg[VinNum + i].MinClkFreq = SwPll.FtcmCfg.DevCfg[VinNum + i].InitClkFreq - (SwPll.FtcmCfg.DevCfg[VinNum + i].InitClkFreq / 10000U);
                            SwPll.FtcmCfg.DevCfg[VinNum + i].FrameRate.TimeScale = pResCfg->DispStrm[i].FrameRate.TimeScale;
                            SwPll.FtcmCfg.DevCfg[VinNum + i].FrameRate.UnitsInTick = pResCfg->DispStrm[i].FrameRate.NumUnitsInTick;
                            SwPll.FtcmCfg.DevCfg[VinNum + i].Delay = 0x7FFFFFFF; // keep the latency at boot
                            SwPll.FtcmCfg.DevCfg[VinNum + i].Tolerance = 500;
                            DevMsk |= (UINT32)1 << (VinNum + i);
                            SwPll.Valid[VinNum + i] = 1;
                            if (pResCfg->DispStrm[i].VoutID == 0U) {
                                SwPll.AdjClk[VinNum + i] = Vout0_Adjust;
                            } else {
                                SwPll.AdjClk[VinNum + i] = Vout1_Adjust;
                            }
                            if (SwPll_DebugOn == 1U) {
                                SvcLog_OK(SVC_LOG_SWPLL, "Dev %u VoutID %u", VinNum + i, pResCfg->DispStrm[i].VoutID);
                                SvcLog_OK(SVC_LOG_SWPLL, "  clk %u", SwPll.FtcmCfg.DevCfg[VinNum + i].InitClkFreq, 0);
                                SvcLog_OK(SVC_LOG_SWPLL, "  MaxClkFreq %u MinClkFreq %u", SwPll.FtcmCfg.DevCfg[VinNum + i].MaxClkFreq, SwPll.FtcmCfg.DevCfg[VinNum + i].MinClkFreq);
                                SvcLog_OK(SVC_LOG_SWPLL, "  TimeScale %u UnitsInTick %u", SwPll.FtcmCfg.DevCfg[VinNum + i].FrameRate.TimeScale, SwPll.FtcmCfg.DevCfg[VinNum + i].FrameRate.UnitsInTick);
                                SvcLog_OK(SVC_LOG_SWPLL, "  Delay %u Tolerance %u", (UINT32)SwPll.FtcmCfg.DevCfg[VinNum + i].Delay, SwPll.FtcmCfg.DevCfg[VinNum + i].Tolerance);
                            }
                        }
                    }
#if (ADJUST_AUDIO == 1)
                    if (Rval == OK) {
                        if ((i + VinNum) < SVC_SWPLL_MAX_NUM_DEV) {
                            SwPll.DevId[SVC_SWPLL_DEV_AUDIO] = (UINT8)i + (UINT8)VinNum;
                            SwPll.Valid[i + VinNum] = 1;
                            SwPll.AdjClk[VinNum + i] = Audio_Adjust;
                            DevMsk |= (UINT32)1 << (VinNum + i);
                            SwPll.FtcmCfg.NumDev ++;
                            Rval = AmbaSYS_GetClkFreq(AMBA_SYS_CLK_AUD_0, &SwPll.FtcmCfg.DevCfg[i + VinNum].InitClkFreq);
                            if (Rval != OK) {
                                SvcLog_NG(SVC_LOG_SWPLL, "AmbaSYS_GetClkFreq %u failed! return %u", AMBA_SYS_CLK_VID_IN0 + i, Rval);
                            }
                            SwPll.FtcmCfg.DevCfg[i + VinNum].MaxClkFreq = SwPll.FtcmCfg.DevCfg[i + VinNum].InitClkFreq + (SwPll.FtcmCfg.DevCfg[i + VinNum].InitClkFreq / 10000U);
                            SwPll.FtcmCfg.DevCfg[i + VinNum].MinClkFreq = SwPll.FtcmCfg.DevCfg[i + VinNum].InitClkFreq - (SwPll.FtcmCfg.DevCfg[i + VinNum].InitClkFreq / 10000U);
                            SwPll.FtcmCfg.DevCfg[i + VinNum].FrameRate.TimeScale = 0;
                            SwPll.FtcmCfg.DevCfg[i + VinNum].FrameRate.UnitsInTick = 0;
                            SwPll.FtcmCfg.DevCfg[i + VinNum].Delay = 0x7FFFFFFF;
                            SwPll.FtcmCfg.DevCfg[i + VinNum].Tolerance = 0;
                            if (SwPll_DebugOn == 1U) {
                                SvcLog_OK(SVC_LOG_SWPLL, "Dev %u Audio", i + VinNum, 0);
                                SvcLog_OK(SVC_LOG_SWPLL, "  clk %u", SwPll.FtcmCfg.DevCfg[VinNum + i].InitClkFreq, 0);
                                SvcLog_OK(SVC_LOG_SWPLL, "  MaxClkFreq %u MinClkFreq %u", SwPll.FtcmCfg.DevCfg[VinNum + i].MaxClkFreq, SwPll.FtcmCfg.DevCfg[VinNum + i].MinClkFreq);
                                SvcLog_OK(SVC_LOG_SWPLL, "  TimeScale %u UnitsInTick %u", SwPll.FtcmCfg.DevCfg[VinNum + i].FrameRate.TimeScale, SwPll.FtcmCfg.DevCfg[VinNum + i].FrameRate.UnitsInTick);
                                SvcLog_OK(SVC_LOG_SWPLL, "  Delay %u Tolerance %u", (UINT32)SwPll.FtcmCfg.DevCfg[VinNum + i].Delay, SwPll.FtcmCfg.DevCfg[VinNum + i].Tolerance);
                            }
                        } else {
                            Rval = SVC_NG;
                            SvcLog_NG(SVC_LOG_SWPLL, "No device id for audio!", 0, 0);
                        }
                    }
#endif
                    if ((Rval == OK) && (SwPll.FtcmCfg.NumDev > 0U)) {
                        if (SwPll.RefDevID == SVC_SWPLL_MAX_NUM_DEV) {
                            SwPll.FtcmCfg.RefClk.ClkFreq = 1000000;
                            SwPll.FtcmCfg.RefClk.GetTick = SwPllGetSystemTimeU32Us;
                            if (SwPll_DebugOn == 1U) {
                                SvcLog_OK(SVC_LOG_SWPLL, "RefClk %u", SwPll.FtcmCfg.RefClk.ClkFreq, 0);
                            }
                        }
                        Rval = AmbaFTCM_Start(&SwPll.FtcmCfg);
                    }
                }
                if (Rval != OK) {
                    SvcLog_NG(SVC_LOG_SWPLL, "AmbaFTCM_Start failed! return 0x%x", Rval, 0);
                } else {
                    if (SwPll_DebugOn == 1U) {
                        SvcLog_OK(SVC_LOG_SWPLL, "NumDev %u DevMsk 0x%x", SwPll.FtcmCfg.NumDev, DevMsk);
                    }
                    Rval = SvcSwPll_Enable(DevMsk, 1);
                }
            }
        }
        if (Rval == OK) {
            SwPll.AdjustTask.Priority    = pCfg->Priority;
            SwPll.AdjustTask.EntryFunc   = SvcSwPll_AdjTaskEntry;
            SwPll.AdjustTask.EntryArg    = 0U;
            SwPll.AdjustTask.pStackBase  = SvcSwPllAdj;
            SwPll.AdjustTask.StackSize   = SVC_SW_PLL_STACK_SIZE;
            SwPll.AdjustTask.CpuBits     = pCfg->CpuBits;
            Rval = SvcTask_Create("SvcSwPllAdj", &SwPll.AdjustTask);
            if (SVC_OK != Rval) {
                SvcLog_NG(SVC_LOG_SWPLL, "SvcSwPll_AdjTaskEntry created failed! return %u", Rval, 0U);
            }
        }
        if (Rval == OK) {
            /* open vin interrupt port */
            if (AmbaDspInt_Open(&g_VinIntPort) != SVC_OK) {
                SvcLog_NG(SVC_LOG_SWPLL, "fail to open vin int port", 0U, 0U);
            }

            SwPll.VinTask.Priority    = SWPLL_VIN_TASK_PRIORITY;
            SwPll.VinTask.EntryFunc   = SvcSwPll_VinTaskEntry;
            SwPll.VinTask.EntryArg    = 0U;
            SwPll.VinTask.pStackBase  = SvcSwPllVin;
            SwPll.VinTask.StackSize   = SVC_SW_PLL_STACK_SIZE;
            SwPll.VinTask.CpuBits     = pCfg->CpuBits;
            Rval = SvcTask_Create("SvcSwPllVin", &SwPll.VinTask);
            if (SVC_OK != Rval) {
                SvcLog_NG(SVC_LOG_SWPLL, "SvcSwPll_VinTaskEntry created failed! return %u", Rval, 0U);
            }
        }
        if (Rval == OK) {
            /* open vout interrupt port */
            if (AmbaDspInt_Open(&g_VoutIntPort) != SVC_OK) {
                SvcLog_NG(SVC_LOG_SWPLL, "fail to open vout int port", 0U, 0U);
            }

            SwPll.VoutTask.Priority    = SWPLL_VOUT_TASK_PRIORITY;
            SwPll.VoutTask.EntryFunc   = SvcSwPll_VoutTaskEntry;
            SwPll.VoutTask.EntryArg    = 0U;
            SwPll.VoutTask.pStackBase  = SvcSwPllVout;
            SwPll.VoutTask.StackSize   = SVC_SW_PLL_STACK_SIZE;
            SwPll.VoutTask.CpuBits     = pCfg->CpuBits;
            Rval = SvcTask_Create("SvcSwPllVout", &SwPll.VoutTask);
            if (SVC_OK != Rval) {
                SvcLog_NG(SVC_LOG_SWPLL, "SvcSwPll_VoutTaskEntry created failed! return %u", Rval, 0U);
            }
        }
#if (ADJUST_AUDIO == 1)
        if (Rval == OK) {
            Rval = AmbaKAL_MsgQueueCreate(&SwPll.AudioMsgQue, AudioMqueueName, sizeof(AUDIO_MSG_s), SwPll.AudioMsgQueBuf, sizeof(SwPll.AudioMsgQueBuf));
            if (SVC_OK != Rval) {
                SvcLog_NG(SVC_LOG_SWPLL, "AmbaKAL_MsgQueueCreate for audio created failed! return %u", Rval, 0U);
            }
        }
        if (Rval == OK) {
            SwPll.AudioTask.Priority    = SWPLL_AUDIO_TASK_PRIORITY;
            SwPll.AudioTask.EntryFunc   = SvcSwPll_AudioTaskEntry;
            SwPll.AudioTask.EntryArg    = 0U;
            SwPll.AudioTask.pStackBase  = SvcSwPllAudio;
            SwPll.AudioTask.StackSize   = SVC_SW_PLL_STACK_SIZE;
            SwPll.AudioTask.CpuBits     = pCfg->CpuBits;
            Rval = SvcTask_Create("SvcSwPllAudio", &SwPll.AudioTask);
            if (SVC_OK != Rval) {
                SvcLog_NG(SVC_LOG_SWPLL, "SvcSwPll_AudioTaskEntry created failed! return %u", Rval, 0U);
            }
        }
        if (Rval == OK) {
            Rval = AmbaDSP_EventHandlerRegister(AMBA_DSP_EVENT_LV_RAW_RDY, LvRawDataRdyHandler);
        }
#endif
        if (Rval == OK) {
            SwPll_Created = 1;
        }
    } else {
        Rval = SVC_OK;
    }
    return Rval;
}

/**
* delete of swpll module
* @return 0-OK, 1-NG
*/
UINT32 SvcSwPll_Delete(void)
{
    UINT32 Rval;

    if (SwPll_Created == 1U) {
        Rval = SvcTask_Destroy(&SwPll.AdjustTask);
        if (Rval != SVC_OK) {
            SvcLog_NG(SVC_LOG_SWPLL, "SvcSwPll_AdjTaskEntry delete failed! return %u", Rval, 0U);
        }
        if (Rval == SVC_OK) {
            Rval = SvcTask_Destroy(&SwPll.VinTask);
            if (Rval != SVC_OK) {
                SvcLog_NG(SVC_LOG_SWPLL, "SvcSwPll_VinTaskEntry delete failed! return %u", Rval, 0U);
            }

            /* close vin interrupt port */
            if (AmbaDspInt_Close(&g_VinIntPort) != SVC_OK) {
                SvcLog_NG(SVC_LOG_SWPLL, "fail to close vin int port", 0U, 0U);
            }
        }
        if (Rval == SVC_OK) {
            Rval = SvcTask_Destroy(&SwPll.VoutTask);
            if (Rval != SVC_OK) {
                SvcLog_NG(SVC_LOG_SWPLL, "SvcSwPll_VoutTaskEntry delete failed! return %u", Rval, 0U);
            }

            /* close vout interrupt port */
            if (AmbaDspInt_Close(&g_VoutIntPort) != SVC_OK) {
                SvcLog_NG(SVC_LOG_SWPLL, "fail to close vout int port", 0U, 0U);
            }
        }
#if (ADJUST_AUDIO == 1)
        if (Rval == SVC_OK) {
            Rval = SvcTask_Destroy(&SwPll.AudioTask);
            if (Rval != SVC_OK) {
                SvcLog_NG(SVC_LOG_SWPLL, "SvcSwPll_AudioTaskEntry delete failed! return %u", Rval, 0U);
            }
        }
        if (Rval == OK) {
            Rval = AmbaDSP_EventHandlerUnRegister(AMBA_DSP_EVENT_LV_RAW_RDY, LvRawDataRdyHandler);
        }
        if (Rval == SVC_OK) {
            Rval = AmbaKAL_MsgQueueDelete(&SwPll.AudioMsgQue);
            if (Rval != OK) {
                SvcLog_NG(SVC_LOG_SWPLL, "AudioMsgQue delete failed! return %u", Rval, 0U);
            }
        }
#endif
        if (Rval == SVC_OK) {
            Rval = AmbaKAL_MsgQueueDelete(&SwPll.MsgQue);
            if (Rval != OK) {
                SvcLog_NG(SVC_LOG_SWPLL, "MsgQue delete failed! return %u", Rval, 0U);
            }
        }
        if (Rval == OK) {
            Rval = SvcSwPll_Enable(SwPll.EnableDevMask, 0);
        }
        if (Rval == OK) {
            Rval = AmbaFTCM_Stop();
            if (Rval != OK) {
                SvcLog_NG(SVC_LOG_SWPLL, "AmbaFTCM_Stop failed! return %u", Rval, 0U);
            }
        }
        if (Rval == OK) {
            Rval = AmbaTMR_Stop(SwPll.RefClkTmrId);
            if (Rval != OK) {
                SvcLog_NG(SVC_LOG_SWPLL, "AmbaTMR_Stop RefClkTmrId %u failed! return %u", SwPll.RefClkTmrId, Rval);
            }
        }
        if (Rval == OK) {
            Rval = AmbaTMR_Release(SwPll.RefClkTmrId);
            if (Rval != OK) {
                SvcLog_NG(SVC_LOG_SWPLL, "AmbaTMR_Release RefClkTmrId %u failed! return %u", SwPll.RefClkTmrId, Rval);
            }
        }
        if (Rval == OK) {
            Rval = AmbaTMR_Stop(SwPll.FtcmCfg.TimerId);
            if (Rval != OK) {
                SvcLog_NG(SVC_LOG_SWPLL, "AmbaTMR_Stop FtcmCfg.TimerId %u failed! return %u", SwPll.FtcmCfg.TimerId, Rval);
            }
        }
        if (Rval == OK) {
            Rval = AmbaTMR_Release(SwPll.FtcmCfg.TimerId);
            if (Rval != OK) {
                SvcLog_NG(SVC_LOG_SWPLL, "AmbaTMR_Release FtcmCfg.TimerId %u failed! return %u", SwPll.FtcmCfg.TimerId, Rval);
            }
        }
        if (Rval == OK) {
            Rval = AmbaKAL_MutexDelete(&SwPll.DevClkMutex);
            if (Rval != OK) {
                SvcLog_NG(SVC_LOG_SWPLL, "AmbaKAL_MutexDelete SwPll.DevClkMutex failed! return %u", Rval, 0);
            } else {
                Rval = AmbaKAL_MutexDelete(&SwPll.TmrMutex);
                if (Rval != OK) {
                    SvcLog_NG(SVC_LOG_SWPLL, "AmbaKAL_MutexDelete SwPll.TmrMutex failed! return %u", Rval, 0);
                }
            }
        }
        if (Rval == OK) {
            SwPll_Created = 0;
        }
    } else {
        Rval = SVC_OK;
    }
    return Rval;
}
