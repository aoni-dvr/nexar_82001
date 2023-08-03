/**
 *  @file AmbaRscData.c
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
 *  @details svc record source - data
 *
 */

#include "AmbaTypes.h"
#include "AmbaKAL.h"
#include "AmbaMisraFix.h"
#include "AmbaWrap.h"
#include "AmbaDef.h"
#include "AmbaDSP.h"
#include "AmbaPrint.h"
#include "AmbaAudio_AENC.h"
#include "AmbaSYS.h"
#include "AmbaUtility.h"
#include "AmbaRecFrwk.h"
#include "AmbaRscInf.h"
#include "AmbaSvcWrap.h"
#include "AmbaRscData.h"
#include "AmbaCodecCom.h"

#define RDA_DBG_ADV     0
#define RDA_DBG_PRN     1
static inline void RDA_DBG(const char *pFormat, UINT32 Arg1, UINT32 Arg2)
{
#if RDA_DBG_PRN
    AmbaPrint_ModulePrintUInt5(AMBA_RECODER_PRINT_MODULE_ID, pFormat, Arg1, Arg2, 0U, 0U, 0U);
#else
    AmbaMisra_TouchUnused(&pFormat);
    AmbaMisra_TouchUnused(&Arg1);
    AmbaMisra_TouchUnused(&Arg2);
#endif
}

static inline void RDA_NG(const char *pFormat, UINT32 Arg1, UINT32 Arg2)
{
    AmbaPrint_ModulePrintUInt5(AMBA_RECODER_PRINT_MODULE_ID, pFormat, Arg1, Arg2, 0U, 0U, 0U);
}

#define SVC_RDA_STATE_INVALID       (0U)
#define SVC_RDA_STATE_IDLE          (1U)
#define SVC_RDA_STATE_RUN           (2U)
#define SVC_RDA_STATE_HALT          (3U)

#define AMBA_RSC_WARN_MUL            (5U)    /* 5 frames */
#define AMBA_RSC_WARN_PRN            (5U)    /* 5 seconds */

typedef struct {
    AMBA_KAL_MUTEX_t        Mutex;
    UINT32                  WrIdx;
    UINT32                  RdIdx;
    UINT32                  MaxDataNum;
    AMBA_DATG_s             *pDataBuf;

    UINT32                  Queue[CONFIG_AMBA_REC_MAX_DRSC_QUELEN];
    AMBA_KAL_MSG_QUEUE_t    QueID;

    AMBA_RSC_STATIS_s       Statis;
    UINT32                  State;

    UINT64                  RawDuration;
    UINT64                  HwTimeScale;

    UINT64                  WarningTh;
    UINT32                  WarnPrnTh;
    UINT32                  WarnPrnCount;
} SVC_RDA_PRIV_s;

static UINT64 DiffFrameTime(const AMBA_RSC_STATIS_s *pStatis, UINT64 CurrTime)
{
    UINT64  Diff = 0U;

    if (0U < pStatis->RxHwTime) {
        if (pStatis->RxHwTime <= CurrTime) {
            Diff = CurrTime - pStatis->RxHwTime;
        } else {
            /* hw_time is 32bit only */
            Diff = 0xFFFFFFFFU - pStatis->RxHwTime;
            Diff += CurrTime;
        }
    }

    return Diff;
}

static void CmpFrameTime(const SVC_RDA_PRIV_s *pPriv, UINT64 Diff)
{
#if RDA_DBG_ADV
    UINT32  IsSlow, Err;
    UINT64  TimeMs, DiffVal;
    char AdvStrBuf[64];
    UINT32 CurStrLen;

    if (Diff <= pPriv->RawDuration) {
        DiffVal = pPriv->RawDuration - Diff;
        IsSlow = 0U;
    } else {
        DiffVal = Diff - pPriv->RawDuration;
        IsSlow = 1U;
    }

    if (pPriv->WarningTh < DiffVal) {
        TimeMs = GetRoundUpValU64((DiffVal * 1000U), pPriv->HwTimeScale);

        // "[d] %s %llums"
        Err = AmbaWrap_memset(AdvStrBuf, 0, sizeof(AdvStrBuf));
        if (Err != RECODER_OK) {
            RDA_NG( "[d] AmbaWrap_memset failed %u", Err, 0U);
        }
        AmbaUtility_StringAppend(AdvStrBuf, sizeof(AdvStrBuf), "[d] ");
        AmbaUtility_StringAppend(AdvStrBuf, sizeof(AdvStrBuf), (((0U < IsSlow) ? "slow " : "fast "));
        CurStrLen = AmbaUtility_StringLength(AdvStrBuf);
        if (CurStrLen < sizeof(AdvStrBuf)) {
            Err = AmbaUtility_UInt64ToStr(&(AdvStrBuf[CurStrLen]), sizeof(AdvStrBuf) - CurStrLen,
                                    TimeMs, 10U);
            if (Err == 0U) {
                RDA_NG( "[d] AmbaUtility_UInt64ToStr failed %u", Err, 0U);
            }
        }
        AmbaUtility_StringAppend(AdvStrBuf, sizeof(AdvStrBuf), "ms");

        AmbaPrint_PrintStr5(AdvStrBuf, NULL, NULL, NULL, NULL, NULL);
    }

#else
    /* do nothing */
    AmbaMisra_TouchUnused(&pPriv);
    AmbaMisra_TouchUnused(&Diff);
#endif
}

static UINT64 CalcLtTime(UINT64 BaseTime, UINT64 IncTime, UINT32 *pIsCarry)
{
    UINT64  CalcTime;
    char AdvStrBuf[64];
    UINT32 CurStrLen, Err;

    CalcTime = BaseTime + IncTime;
    *pIsCarry = 0U;

    if (CalcTime < BaseTime) {
        *pIsCarry = 1U;

        // "Carry, PreTime/CurTime/(%llu/%llu)"
        Err = AmbaWrap_memset(AdvStrBuf, 0, sizeof(AdvStrBuf));
        if (Err != RECODER_OK) {
            RDA_NG( "[d] AmbaWrap_memset failed %u", Err, 0U);
        }
        AmbaUtility_StringAppend(AdvStrBuf, (UINT32)sizeof(AdvStrBuf), "Carry, PreTime/CurTime/(");
        CurStrLen = (UINT32)AmbaUtility_StringLength(AdvStrBuf);
        if (CurStrLen < (UINT32)sizeof(AdvStrBuf)) {
            Err = AmbaUtility_UInt64ToStr(&(AdvStrBuf[CurStrLen]), (UINT32)sizeof(AdvStrBuf) - CurStrLen,
                                    BaseTime, 10U);
            if (Err == 0U) {
                RDA_NG( "[d] AmbaUtility_UInt64ToStr failed %u", Err, 0U);
            }
        }
        AmbaUtility_StringAppend(AdvStrBuf, (UINT32)sizeof(AdvStrBuf), "/");
        CurStrLen = (UINT32)AmbaUtility_StringLength(AdvStrBuf);
        if (CurStrLen < (UINT32)sizeof(AdvStrBuf)) {
            Err = AmbaUtility_UInt64ToStr(&(AdvStrBuf[CurStrLen]), (UINT32)sizeof(AdvStrBuf) - CurStrLen,
                                    CalcTime, 10U);
            if (Err == 0U) {
                RDA_NG( "[d] AmbaUtility_UInt64ToStr failed %u", Err, 0U);
            }
        }
        AmbaUtility_StringAppend(AdvStrBuf, (UINT32)sizeof(AdvStrBuf), ")");

        AmbaPrint_PrintStr5(AdvStrBuf, NULL, NULL, NULL, NULL, NULL);
    }

    return CalcTime;
}

static void CmpLtTime(SVC_RDA_PRIV_s *pPriv, const AMBA_RSC_STATIS_s *pStatis)
{
#if RDA_DBG_ADV
    UINT32  IsSlow, Err;
    UINT64  Diff, TimeMs;
    char AdvStrBuf[150];
    UINT32 CurStrLen;

    if (pStatis->RxLtExpTimeCarry == pStatis->RxLtHwTimeCarry) {
        if (pStatis->RxLtHwTime <= pStatis->RxLtExpTime) {
            Diff = pStatis->RxLtExpTime - pStatis->RxLtHwTime;
            IsSlow = 0U;
        } else {
            Diff = pStatis->RxLtHwTime - pStatis->RxLtExpTime;
            IsSlow = 1U;
        }
    } else {
        if (pStatis->RxLtHwTimeCarry < pStatis->RxLtExpTimeCarry) {
            Diff = 0xFFFFFFFFFFFFFFFFU - pStatis->RxLtHwTime;
            Diff += pStatis->RxLtExpTime;
            IsSlow = 0U;
        } else {
            Diff = 0xFFFFFFFFFFFFFFFFU - pStatis->RxLtExpTime;
            Diff += pStatis->RxLtHwTime;
            IsSlow = 1U;
        }
    }

#if 0
    {
        TimeMs = GetRoundUpValU64((Diff * 1000U), pPriv->HwTimeScale);

        // "[d_lt]%s_%llums, ExpCarry/ExpTime/HwCarry/HwTime(%llu/%llu/%llu/%llu)"
        Err = AmbaWrap_memset(AdvStrBuf, 0, sizeof(AdvStrBuf));
        if (Err != RECODER_OK) {
            RDA_NG( "[d] AmbaWrap_memset failed %u", Err, 0U);
        }
        AmbaUtility_StringAppend(AdvStrBuf, sizeof(AdvStrBuf), "[d_lt]");
        AmbaUtility_StringAppend(AdvStrBuf, sizeof(AdvStrBuf), (((0U < IsSlow) ? "slow_" : "fast_"));
        CurStrLen = AmbaUtility_StringLength(AdvStrBuf);
        if (CurStrLen < sizeof(AdvStrBuf)) {
            Err = AmbaUtility_UInt64ToStr(&(AdvStrBuf[CurStrLen]), sizeof(AdvStrBuf) - CurStrLen,
                                    TimeMs, 10U);
            if (Err == 0U) {
                RDA_NG( "[d] AmbaUtility_UInt64ToStr failed %u", Err, 0U);
            }
        }
        AmbaUtility_StringAppend(AdvStrBuf, sizeof(AdvStrBuf), "ms, ExpCarry/ExpTime/HwCarry/HwTime(");
        CurStrLen = AmbaUtility_StringLength(AdvStrBuf);
        if (CurStrLen < sizeof(AdvStrBuf)) {
            Err = AmbaUtility_UInt64ToStr(&(AdvStrBuf[CurStrLen]), sizeof(AdvStrBuf) - CurStrLen,
                    pStatis->RxLtExpTimeCarry, 10U);
            if (Err == 0U) {
                RDA_NG( "[d] AmbaUtility_UInt64ToStr failed %u", Err, 0U);
            }
        }
        AmbaUtility_StringAppend(AdvStrBuf, sizeof(AdvStrBuf), "/");
        CurStrLen = AmbaUtility_StringLength(AdvStrBuf);
        if (CurStrLen < sizeof(AdvStrBuf)) {
            Err = AmbaUtility_UInt64ToStr(&(AdvStrBuf[CurStrLen]), sizeof(AdvStrBuf) - CurStrLen,
                    pStatis->RxLtExpTime, 10U);
            if (Err == 0U) {
                RDA_NG( "[d] AmbaUtility_UInt64ToStr failed %u", Err, 0U);
            }
        }
        AmbaUtility_StringAppend(AdvStrBuf, sizeof(AdvStrBuf), "/");
        CurStrLen = AmbaUtility_StringLength(AdvStrBuf);
        if (CurStrLen < sizeof(AdvStrBuf)) {
            Err = AmbaUtility_UInt64ToStr(&(AdvStrBuf[CurStrLen]), sizeof(AdvStrBuf) - CurStrLen,
                    pStatis->RxLtHwTimeCarry, 10U);
            if (Err == 0U) {
                RDA_NG( "[d] AmbaUtility_UInt64ToStr failed %u", Err, 0U);
            }
        }
        AmbaUtility_StringAppend(AdvStrBuf, sizeof(AdvStrBuf), "/");
        CurStrLen = AmbaUtility_StringLength(AdvStrBuf);
        if (CurStrLen < sizeof(AdvStrBuf)) {
            Err = AmbaUtility_UInt64ToStr(&(AdvStrBuf[CurStrLen]), sizeof(AdvStrBuf) - CurStrLen,
                    pStatis->RxLtHwTime, 10U);
            if (Err == 0U) {
                RDA_NG( "[d] AmbaUtility_UInt64ToStr failed %u", Err, 0U);
            }
        }
        AmbaUtility_StringAppend(AdvStrBuf, sizeof(AdvStrBuf), ")");

        AmbaPrint_PrintStr5(AdvStrBuf, NULL, NULL, NULL, NULL, NULL);
    }
#endif

    pPriv->WarnPrnCount++;
    if (pPriv->WarnPrnTh <= pPriv->WarnPrnCount) {
        pPriv->WarnPrnCount = 0U;

        if (pPriv->WarningTh < Diff) {
            TimeMs = GetRoundUpValU64((Diff * 1000U), pPriv->HwTimeScale);

            // "[d] %s %llums, ExpTime/HwTime(%llu/%llu)"
            Err = AmbaWrap_memset(AdvStrBuf, 0, sizeof(AdvStrBuf));
            if (Err != RECODER_OK) {
                RDA_NG( "[d] AmbaWrap_memset failed %u", Err, 0U);
            }
            AmbaUtility_StringAppend(AdvStrBuf, (UINT32)sizeof(AdvStrBuf), "[d]");
            AmbaUtility_StringAppend(AdvStrBuf, (UINT32)sizeof(AdvStrBuf), (((0U < IsSlow) ? "slow " : "fast "));
            CurStrLen = (UINT32)AmbaUtility_StringLength(AdvStrBuf);
            if (CurStrLen < (UINT32)sizeof(AdvStrBuf)) {
                Err = AmbaUtility_UInt64ToStr(&(AdvStrBuf[CurStrLen]), (UINT32)sizeof(AdvStrBuf) - CurStrLen,
                                        TimeMs, 10U);
                if (Err == 0U) {
                    RDA_NG( "[d] AmbaUtility_UInt64ToStr failed %u", Err, 0U);
                }
            }
            AmbaUtility_StringAppend(AdvStrBuf, (UINT32)sizeof(AdvStrBuf), "ms, ExpTime/HwTime(");
            CurStrLen = (UINT32)AmbaUtility_StringLength(AdvStrBuf);
            if (CurStrLen < (UINT32)sizeof(AdvStrBuf)) {
                Err = AmbaUtility_UInt64ToStr(&(AdvStrBuf[CurStrLen]), (UINT32)sizeof(AdvStrBuf) - CurStrLen,
                        pStatis->RxLtExpTime, 10U);
                if (Err == 0U) {
                    RDA_NG( "[d] AmbaUtility_UInt64ToStr failed %u", Err, 0U);
                }
            }
            AmbaUtility_StringAppend(AdvStrBuf, (UINT32)sizeof(AdvStrBuf), "/");
            CurStrLen = (UINT32)AmbaUtility_StringLength(AdvStrBuf);
            if (CurStrLen < (UINT32)sizeof(AdvStrBuf)) {
                Err = AmbaUtility_UInt64ToStr(&(AdvStrBuf[CurStrLen]), (UINT32)sizeof(AdvStrBuf) - CurStrLen,
                        pStatis->RxLtHwTime, 10U);
                if (Err == 0U) {
                    RDA_NG( "[d] AmbaUtility_UInt64ToStr failed %u", Err, 0U);
                }
            }
            AmbaUtility_StringAppend(AdvStrBuf, (UINT32)sizeof(AdvStrBuf), ")");

            AmbaPrint_PrintStr5(AdvStrBuf, NULL, NULL, NULL, NULL, NULL);
        }
    }
#else
    AmbaMisra_TouchUnused(pPriv);
    (void)pStatis;
#endif
}

static void EnData(SVC_RDA_PRIV_s *pPriv, void *pData)
{
    UINT32  WrIdx, IsOverFlow = 0U, Err;

    if (AmbaKAL_MutexTake(&(pPriv->Mutex), AMBA_KAL_WAIT_FOREVER) == RECODER_OK) {
        WrIdx = pPriv->WrIdx;
        Err = AmbaWrap_memcpy(&(pPriv->pDataBuf[WrIdx]), pData, sizeof(AMBA_DATG_s));
        if (Err != RECODER_OK) {
            RDA_NG( "[d] AmbaWrap_memcpy failed %u", Err, 0U);
        }

        pPriv->WrIdx++;
        if (pPriv->WrIdx == pPriv->MaxDataNum) {
            pPriv->WrIdx = 0U;
        }

        if (pPriv->WrIdx == pPriv->RdIdx) {
            IsOverFlow = 1U;
        }

        Err = AmbaKAL_MutexGive(&(pPriv->Mutex));
        if (Err != RECODER_OK) {
            RDA_NG( "[d] AmbaKAL_MutexGive failed %u", Err, 0U);
        }

        if (AmbaKAL_MsgQueueSend(&(pPriv->QueID), &WrIdx, AMBA_KAL_NO_WAIT) != RECODER_OK) {
            RDA_NG( "[d] fail to enqueue", 0U, 0U);
        }

        if (0U < IsOverFlow) {
            RDA_NG( "[d] overflow", 0U, 0U);
        }
    }

    AmbaMisra_TouchUnused(pData);
}

static UINT32 EnQueue(const AMBA_REC_SRC_s *pRda, void *pDataInfo)
{
    const UINT8       *pPrivData = pRda->PrivData;
    UINT32            Rval = RECODER_OK, DataSize = 0U, IsEos = 0U, IsCarry, Err;
    UINT64            HwTime = 0U, Diff;
    AMBA_RSC_STATIS_s  Statis;
    SVC_RDA_PRIV_s    *pPriv;
    const AMBA_DATG_s  *pDataG = NULL;
    char AdvStrBuf[256];
    UINT32 CurStrLen;

    AmbaMisra_TypeCast(&pPriv, &(pPrivData));
    Statis = pPriv->Statis;

    if (pPriv->State == SVC_RDA_STATE_IDLE) {
        /* [idle] -> [run] */
        pPriv->State = SVC_RDA_STATE_RUN;
    }

    if (pPriv->State == SVC_RDA_STATE_RUN) {
        AmbaMisra_TypeCast(&pDataG, &pDataInfo);

        if (pDataG->Eos == 0U) {
            DataSize = (UINT32)sizeof(AMBA_DATG_s);
            HwTime   = pDataG->CapTime[TICK_TYPE_AUDIO];

            Statis.RxCount++;
        } else {
            IsEos = 1U;
        }

        if (0U < DataSize) {
            Statis.RxSize += DataSize;
        }

        /* monitor incoming hw time */
        if (0U < HwTime) {
            Diff = DiffFrameTime(&Statis, HwTime);
#if RDA_DBG_ADV
            {
                UINT64  TimeMs = GetRoundUpValU64((Diff * 1000U), pPriv->HwTimeScale);

                // "[d]%llums, PrevHwTime/CurrHwTime(%llu/%llu)"
                Err = AmbaWrap_memset(AdvStrBuf, 0, sizeof(AdvStrBuf));
                if (Err != RECODER_OK) {
                    RDA_NG( "[d] AmbaWrap_memset failed %u", Err, 0U);
                }
                AmbaUtility_StringAppend(AdvStrBuf, (UINT32)sizeof(AdvStrBuf), "[d]");
                CurStrLen = (UINT32)AmbaUtility_StringLength(AdvStrBuf);
                if (CurStrLen < (UINT32)sizeof(AdvStrBuf)) {
                    Err = AmbaUtility_UInt64ToStr(&(AdvStrBuf[CurStrLen]), (UINT32)sizeof(AdvStrBuf) - CurStrLen,
                                            TimeMs, 10U);
                    if (Err == 0U) {
                        RDA_NG( "[d] AmbaUtility_UInt64ToStr failed %u", Err, 0U);
                    }
                }
                AmbaUtility_StringAppend(AdvStrBuf, (UINT32)sizeof(AdvStrBuf), "ms, PrevHwTime/CurrHwTime(");
                CurStrLen = (UINT32)AmbaUtility_StringLength(AdvStrBuf);
                if (CurStrLen < (UINT32)sizeof(AdvStrBuf)) {
                    Err = AmbaUtility_UInt64ToStr(&(AdvStrBuf[CurStrLen]), (UINT32)sizeof(AdvStrBuf) - CurStrLen,
                            Statis.RxHwTime, 10U);
                    if (Err == 0U) {
                        RDA_NG( "[d] AmbaUtility_UInt64ToStr failed %u", Err, 0U);
                    }
                }
                AmbaUtility_StringAppend(AdvStrBuf, sizeof(AdvStrBuf), "/");
                CurStrLen = (UINT32)AmbaUtility_StringLength(AdvStrBuf);
                if (CurStrLen < (UINT32)sizeof(AdvStrBuf)) {
                    Err = AmbaUtility_UInt64ToStr(&(AdvStrBuf[CurStrLen]), (UINT32)sizeof(AdvStrBuf) - CurStrLen,
                            HwTime, 10U);
                    if (Err == 0U) {
                        RDA_NG( "[d] AmbaUtility_UInt64ToStr failed %u", Err, 0U);
                    }
                }
                AmbaUtility_StringAppend(AdvStrBuf, (UINT32)sizeof(AdvStrBuf), ")");

                AmbaPrint_PrintStr5(AdvStrBuf, NULL, NULL, NULL, NULL, NULL);
            }
#endif
            Statis.RxHwTime = HwTime;

            if (0U < Diff) {
                /* calc. long-term hw_time */
                Statis.RxLtHwTime = CalcLtTime(Statis.RxLtHwTime, Diff, &IsCarry);
                if (0U < IsCarry) {
                    Statis.RxLtHwTimeCarry++;
                }

                /* check frame to frame diff */
                CmpFrameTime(pPriv, Diff);
            }

            /* check long-term diff */
            if (0U < Statis.RxLtHwTime) {
                /* calc. long-term expect_time */
                Statis.RxLtExpTime = CalcLtTime(Statis.RxLtExpTime, pPriv->RawDuration, &IsCarry);
                if (0U < IsCarry) {
                    Statis.RxLtExpTimeCarry++;
                }
                CmpLtTime(pPriv, &Statis);

            }
        }

        /* copy data to buffer */
        EnData(pPriv, pDataInfo);

        if (0U < IsEos) {
            /* [run] -> [halt] */
            pPriv->State = SVC_RDA_STATE_HALT;
            RDA_DBG("rsc got eos", 0U, 0U);
        }

        if (pPriv->State == SVC_RDA_STATE_HALT) {
            UINT64  BRate;
            DOUBLE  Frate;

            /* calc. total frame-rate/bitrate */
            Frate = ((DOUBLE)Statis.RxCount - 1.0);
            Frate *= (DOUBLE)pPriv->HwTimeScale;
            Frate /= (DOUBLE)Statis.RxLtHwTime;
            BRate = (Statis.RxSize * 8U);
            BRate = (BRate * pPriv->HwTimeScale) / Statis.RxLtHwTime;

            // "##[d] frate/brate(%.3ffps/%llubps), frames/size(%llu/%llubytes)"
            Err = AmbaWrap_memset(AdvStrBuf, 0, sizeof(AdvStrBuf));
            if (Err != RECODER_OK) {
                RDA_NG( "[d] AmbaWrap_memset failed %u", Err, 0U);
            }
            AmbaUtility_StringAppend(AdvStrBuf, (UINT32)sizeof(AdvStrBuf), "##[d] frate/brate(");
            AmbaUtility_StringAppendDouble(AdvStrBuf, (UINT32)sizeof(AdvStrBuf), Frate, 3U);
            AmbaUtility_StringAppend(AdvStrBuf, (UINT32)sizeof(AdvStrBuf), "fps/");
            CurStrLen = (UINT32)AmbaUtility_StringLength(AdvStrBuf);
            if (CurStrLen < (UINT32)sizeof(AdvStrBuf)) {
                Err = AmbaUtility_UInt64ToStr(&(AdvStrBuf[CurStrLen]), (UINT32)sizeof(AdvStrBuf) - CurStrLen,
                        BRate, 10U);
                if (Err == 0U) {
                    RDA_NG( "[d] AmbaUtility_UInt64ToStr failed %u", Err, 0U);
                }
            }
            AmbaUtility_StringAppend(AdvStrBuf, (UINT32)sizeof(AdvStrBuf), "bps), frames/size(");
            CurStrLen = (UINT32)AmbaUtility_StringLength(AdvStrBuf);
            if (CurStrLen < (UINT32)sizeof(AdvStrBuf)) {
                Err = AmbaUtility_UInt64ToStr(&(AdvStrBuf[CurStrLen]), (UINT32)sizeof(AdvStrBuf) - CurStrLen,
                        Statis.RxCount, 10U);
                if (Err == 0U) {
                    RDA_NG( "[d] AmbaUtility_UInt64ToStr failed %u", Err, 0U);
                }
            }
            AmbaUtility_StringAppend(AdvStrBuf, (UINT32)sizeof(AdvStrBuf), "/");
            CurStrLen = (UINT32)AmbaUtility_StringLength(AdvStrBuf);
            if (CurStrLen < (UINT32)sizeof(AdvStrBuf)) {
                Err = AmbaUtility_UInt64ToStr(&(AdvStrBuf[CurStrLen]), (UINT32)sizeof(AdvStrBuf) - CurStrLen,
                        Statis.RxSize, 10U);
                if (Err == 0U) {
                    RDA_NG( "[d] AmbaUtility_UInt64ToStr failed %u", Err, 0U);
                }
            }
            AmbaUtility_StringAppend(AdvStrBuf, (UINT32)sizeof(AdvStrBuf), "bytes)");

            AmbaPrint_PrintStr5(AdvStrBuf, NULL, NULL, NULL, NULL, NULL);
        }
    }

    pPriv->Statis = Statis;
    return Rval;
}

static UINT32 DeQueue(const AMBA_REC_SRC_s *pRda, AMBA_REC_FRWK_DESC_s *pDesc)
{
    const UINT8     *pPrivData = pRda->PrivData;
    UINT32          Rval, RdIdx, Err;
    SVC_RDA_PRIV_s  *pPriv;

    AmbaMisra_TypeCast(&pPriv, &(pPrivData));

    Rval = AmbaKAL_MsgQueueReceive(&(pPriv->QueID), &RdIdx, AMBA_KAL_NO_WAIT);
    if (Rval == RECODER_OK) {
        const AMBA_RSC_TEXT_DESC_s  *pTDesc;
        AMBA_DATG_s                 *pDataG;

        AmbaMisra_TypeCast(&pTDesc, &(pDesc->pSrcData));
        AmbaMisra_TypeCast(&pDataG, &(pTDesc->pRawData));

        Err = AmbaWrap_memcpy(pDataG, &(pPriv->pDataBuf[RdIdx]), sizeof(AMBA_DATG_s));
        if (Err != RECODER_OK) {
            RDA_NG( "[d] AmbaWrap_memcpy failed %u", Err, 0U);
        }
        pDesc->SrcBit     = pRda->SrcBit;
        pDesc->SrcType    = pRda->UsrCfg.SrcType;
        pDesc->SrcSubType = pRda->UsrCfg.SubType;

        if (AmbaKAL_MutexTake(&(pPriv->Mutex), AMBA_KAL_WAIT_FOREVER) == RECODER_OK) {
            pPriv->RdIdx = RdIdx;
            Err = AmbaKAL_MutexGive(&(pPriv->Mutex));
            if (Err != RECODER_OK) {
                RDA_NG( "[d] AmbaKAL_MutexGive failed %u", Err, 0U);
            }
        }
    }

    return Rval;
}

static UINT32 QueueSize(const AMBA_REC_SRC_s *pRda)
{
    const UINT8                *pPrivData = pRda->PrivData;
    UINT32                     Rval, QSize = 0U;
    SVC_RDA_PRIV_s             *pPriv;
    AMBA_KAL_MSG_QUEUE_INFO_s  QInfo;

    AmbaMisra_TypeCast(&pPriv, &(pPrivData));
    Rval = AmbaKAL_MsgQueueQuery(&(pPriv->QueID), &QInfo);
    if (Rval == RECODER_OK) {
        QSize = QInfo.NumEnqueued;
    }

    return QSize;
}

/**
* record source creation
* @param [in]  pRda source info
* @return ErrorCode
*/
UINT32 AmbaRscData_Create(AMBA_REC_SRC_s *pRda)
{
    static char    RdaQue[32];
    static char    RdaMux[] = "RscDataMux";
    static UINT32  RdaPriv = (UINT32)sizeof(SVC_RDA_PRIV_s);

    const UINT8               *pPrivData = pRda->PrivData;
    UINT32                    Rval = RECODER_ERROR_GENERAL_ERROR, Reserved = CONFIG_AMBA_REC_RSC_PRIV_SIZE;
    UINT32                    AudioClk, DataNum, Err;
    ULONG                     Num;
    SVC_RDA_PRIV_s            *pPriv;
    const AMBA_RSC_USR_CFG_s  *pUsrCfg;

    AmbaMisra_TypeCast(&pPriv, &(pPrivData));
    if (RdaPriv <= Reserved) {
        pRda->pfnEnque   = EnQueue;
        pRda->pfnDeque   = DeQueue;
        pRda->pfnQueSize = QueueSize;

        /* reset parameters */
        Err = AmbaWrap_memset(pPriv, 0, sizeof(SVC_RDA_PRIV_s));
        if (Err != RECODER_OK) {
            RDA_NG( "[d] AmbaWrap_memset failed %u", Err, 0U);
        }

        pUsrCfg = &(pRda->UsrCfg);
        DataNum = pUsrCfg->MemSize / (UINT32)sizeof(AMBA_DATG_s);
        if (0U < DataNum) {
            /* calc. warning threshold */
            AudioClk = 12288000U;   /* 12.288 MHz */
            if (AmbaSYS_GetClkFreq(AMBA_SYS_CLK_AUD_0, &AudioClk) != SYS_ERR_NONE) {
                RDA_NG( "fail to get audio clock", 0U, 0U);
            }

            pPriv->HwTimeScale = AudioClk;
            pPriv->RawDuration = AudioClk;
            pPriv->RawDuration *= pUsrCfg->NumUnitsInTick;
            pPriv->RawDuration /= pUsrCfg->TimeScale;

            pPriv->WarningTh = pPriv->RawDuration * AMBA_RSC_WARN_MUL;
            RDA_DBG( "RawDur/WarnTh(%u/%u)", (UINT32)pPriv->RawDuration
                                                    , (UINT32)pPriv->WarningTh);
            pPriv->WarnPrnTh = GetRoundUpValU32(pUsrCfg->TimeScale, pUsrCfg->NumUnitsInTick);
            pPriv->WarnPrnTh *= AMBA_RSC_WARN_PRN;

            /* create queue */
            AmbaMisra_TypeCast(&(pPriv->pDataBuf), &(pUsrCfg->MemBase));
            pPriv->MaxDataNum = DataNum;

            Num = pRda->UsrCfg.StreamID;
            if (0U < AmbaUtility_UInt32ToStr(RdaQue, 32U, (UINT32)Num, 16U)) {
                AmbaUtility_StringAppend(RdaQue, 32, "RscDataQue");
            }

            Rval = AmbaKAL_MsgQueueCreate(&(pPriv->QueID),
                                          RdaQue,
                                          (UINT32)sizeof(UINT32),
                                          &(pPriv->Queue),
                                          (UINT32)sizeof(pPriv->Queue));
            if (Rval == RECODER_OK) {
                Rval = AmbaKAL_MutexCreate(&(pPriv->Mutex), RdaMux);
                if (Rval == RECODER_OK) {
                    /* [invalid] -> [idle] */
                    pPriv->State = SVC_RDA_STATE_IDLE;
                }
            }
        }
    }

    if (Rval != RECODER_OK) {
        RDA_NG( "fail to create data record source, (%u/%u)", RdaPriv, Reserved);
    }

    RDA_DBG("rsc is created!", 0U, 0U);
    return Rval;
}

/**
* record source destroying
* @param [in]  pRda source info
* @return ErrorCode
*/
UINT32 AmbaRscData_Destroy(const AMBA_REC_SRC_s *pRda)
{
    const UINT8     *pPrivData = pRda->PrivData;
    UINT32          Rval = RECODER_ERROR_GENERAL_ERROR;
    SVC_RDA_PRIV_s  *pPriv;

    AmbaMisra_TypeCast(&pPriv, &(pPrivData));
    if ((pPriv->State == SVC_RDA_STATE_IDLE) || (pPriv->State == SVC_RDA_STATE_HALT)) {
        Rval = AmbaKAL_MsgQueueDelete(&(pPriv->QueID));
        if (Rval != RECODER_OK) {
            RDA_NG( "fail to destroy queue of data record source", 0U, 0U);
        }

        Rval = AmbaKAL_MutexDelete(&(pPriv->Mutex));
        if (Rval != RECODER_OK) {
            RDA_NG( "fail to destroy mutex of data record source", 0U, 0U);
        }

        /* [idle] -> [invalid] */
        pPriv->State = SVC_RDA_STATE_INVALID;

        RDA_DBG("rsc is destroyed, RxCount/RxSize(%X/%X)", (UINT32)pPriv->Statis.RxCount,
                                                           (UINT32)pPriv->Statis.RxSize);
    } else {
        RDA_NG( "rsc isn't in idle state(%u)", pPriv->State, 0U);
    }

    return Rval;
}

/**
* record master control
* @param [in]  pRda source info
* @param [in]  CtrlType control type
* @param [in]  pParam pointer of control parameter
*/
void AmbaRscData_Control(const AMBA_REC_SRC_s *pRda, UINT32 CtrlType, void *pParam)
{
    const UINT8                *pPrivData = pRda->PrivData;
    SVC_RDA_PRIV_s             *pPriv;
    AMBA_KAL_MSG_QUEUE_INFO_s  QueInfo = {0};
    UINT32                     Err;

    AmbaMisra_TypeCast(&pPriv, &(pPrivData));

    switch (CtrlType) {
    case AMBA_RSC_DATA_STATIS_GET:
        Err = AmbaKAL_MsgQueueQuery(&(pPriv->QueID), &QueInfo);
        if (Err != RECODER_OK) {
            RDA_NG( "[d] AmbaKAL_MsgQueueQuery failed %u", Err, 0U);
        }
        pPriv->Statis.QueLen = QueInfo.NumEnqueued;

        Err = AmbaWrap_memcpy(pParam, &(pPriv->Statis), sizeof(AMBA_RSC_STATIS_s));
        if (Err != RECODER_OK) {
            RDA_NG( "[d] AmbaWrap_memcpy failed %u", Err, 0U);
        }
        break;
    case AMBA_RSC_DATA_RESET:
        if ((pPriv->State == SVC_RDA_STATE_IDLE) || (pPriv->State == SVC_RDA_STATE_HALT)) {
            /* reset statis */
            Err = AmbaWrap_memset(&(pPriv->Statis), 0, sizeof(AMBA_RSC_STATIS_s));
            if (Err != RECODER_OK) {
                RDA_NG( "[d] AmbaWrap_memset failed %u", Err, 0U);
            }

            /* [halt] -> [idle] */
            pPriv->State = SVC_RDA_STATE_IDLE;
        } else {
            RDA_NG( "rsc isn't in idle/halt state(%u)", pPriv->State, 0U);
        }
        break;
    case AMBA_RSC_DATA_DUMMY_EOS:
        if (pPriv->State == SVC_RDA_STATE_RUN) {
            AMBA_DATG_s  DataG;

            Err = AmbaWrap_memset(&DataG, 0, sizeof(AMBA_DATG_s));
            if (Err != RECODER_OK) {
                RDA_NG( "[d] AmbaWrap_memset failed %u", Err, 0U);
            }
            /* insert dummy eos if needed */
            DataG.Eos = 1U;
            Err = EnQueue(pRda, &DataG);
            if (Err != RECODER_OK) {
                RDA_NG( "[d] EnQueue failed %u", Err, 0U);
            }
        }
        break;
    default:
        RDA_NG( "unknown control type", 0U, 0U);
        break;
    }
}

/**
* query function of data record source
* @param [in]  SrcType source type
* @param [in]  MaxQueLen max msg queue number
* @param [out]  pSize memory size
*/
void AmbaRscData_EvalMemSize(UINT32 SrcType, UINT32 MaxQueLen, UINT32 *pSize)
{
    UINT32  Val = 0U, MsgSize = 0U;

    if (SrcType == AMBA_RSC_TYPE_DATA) {
        MsgSize = (UINT32)sizeof(AMBA_DATG_s);
    } else {
        /* do nothing */
    }

    if (0U < MsgSize) {
        Val = (MsgSize * MaxQueLen);
        Val = GetRoundUpValU32(Val, (UINT32)AMBA_CACHE_LINE_SIZE);
    }

    (*pSize) = (Val * (UINT32)AMBA_CACHE_LINE_SIZE);
}

/**
* format canbus data to text string
* @param [in]  pText buffer of text string
* @param [in]  pDataG data structure
* @return length of text string
*/
UINT32 AmbaRscData_CanToText(char *pText, const AMBA_DATG_s *pDataG)
{
    UINT32  Rval, Err;

    // T:%u L:%u G:%u XS:%.4f WD:%u XWA:%.4f XWAS:%.4f
    Err = AmbaWrap_memset(pText, 0, CONFIG_AMBA_REC_DRSC_TEXTLEN);
    if (Err != RECODER_OK) {
        RDA_NG( "[d] AmbaWrap_memset failed %u", Err, 0U);
    }
    AmbaUtility_StringAppend(pText, CONFIG_AMBA_REC_DRSC_TEXTLEN, "T:");
    AmbaUtility_StringAppendUInt32(pText, CONFIG_AMBA_REC_DRSC_TEXTLEN, (UINT32)pDataG->CapTime[TICK_TYPE_AUDIO], 10U);
    AmbaUtility_StringAppend(pText, CONFIG_AMBA_REC_DRSC_TEXTLEN, " L:");
    AmbaUtility_StringAppendUInt32(pText, CONFIG_AMBA_REC_DRSC_TEXTLEN, pDataG->CanBus.TurnLightStatus, 10U);
    AmbaUtility_StringAppend(pText, CONFIG_AMBA_REC_DRSC_TEXTLEN, " G:");
    AmbaUtility_StringAppendUInt32(pText, CONFIG_AMBA_REC_DRSC_TEXTLEN, pDataG->CanBus.GearStatus, 10U);
    AmbaUtility_StringAppend(pText, CONFIG_AMBA_REC_DRSC_TEXTLEN, " XS:");
    AmbaUtility_StringAppendDouble(pText, CONFIG_AMBA_REC_DRSC_TEXTLEN, pDataG->CanBus.XferSpeed, 4U);
    AmbaUtility_StringAppend(pText, CONFIG_AMBA_REC_DRSC_TEXTLEN, " WD:");
    AmbaUtility_StringAppendUInt32(pText, CONFIG_AMBA_REC_DRSC_TEXTLEN, pDataG->CanBus.WheelDir, 10U);
    AmbaUtility_StringAppend(pText, CONFIG_AMBA_REC_DRSC_TEXTLEN, " XWA:");
    AmbaUtility_StringAppendDouble(pText, CONFIG_AMBA_REC_DRSC_TEXTLEN, pDataG->CanBus.XferWheelAngle, 4U);
    AmbaUtility_StringAppend(pText, CONFIG_AMBA_REC_DRSC_TEXTLEN, " XWAS:");
    AmbaUtility_StringAppendDouble(pText, CONFIG_AMBA_REC_DRSC_TEXTLEN, pDataG->CanBus.XferWheelAngleSpeed, 4U);
    Rval = (UINT32)AmbaUtility_StringLength(pText);

    return Rval;
}

/**
* format ptp data to text string
* @param [in]  pText buffer of text string
* @param [in]  pDataG data structure
* @return length of text string
*/
UINT32 AmbaRscData_PtpToText(char *pText, const AMBA_DATG_s *pDataG)
{
    UINT32  Rval, Err;

    // S:%u NS:%u
    Err = AmbaWrap_memset(pText, 0, CONFIG_AMBA_REC_DRSC_TEXTLEN);
    if (Err != RECODER_OK) {
        RDA_NG( "[d] AmbaWrap_memset failed %u", Err, 0U);
    }
    AmbaUtility_StringAppend(pText, CONFIG_AMBA_REC_DRSC_TEXTLEN, "S:");
    AmbaUtility_StringAppendUInt32(pText, CONFIG_AMBA_REC_DRSC_TEXTLEN, pDataG->EthPTP.Sec, 10U);
    AmbaUtility_StringAppend(pText, CONFIG_AMBA_REC_DRSC_TEXTLEN, " NS:");
    AmbaUtility_StringAppendUInt32(pText, CONFIG_AMBA_REC_DRSC_TEXTLEN, pDataG->EthPTP.NanoSec, 10U);
    Rval = (UINT32)AmbaUtility_StringLength(pText);

    return Rval;
}

static UINT32 Str2Double(const char *pStr, DOUBLE *pVal)
{
    char    TokenBuf[UTIL_MAX_STR_TOKEN_NUMBER][UTIL_MAX_STR_TOKEN_LENGTH];
    DOUBLE  DoubleVal;
    UINT32  U32Val = 0U, TokCount, Err, Rval = RECODER_OK, Divisor = 1U, i, Afterpoint;

    Err = AmbaUtility_StringToken(pStr, '.', TokenBuf, &TokCount);
    if (Err != 0UL) {
        RDA_NG( "AmbaUtility_StringToken failed %u", Err, 0U);
        Rval = RECODER_ERROR_GENERAL_ERROR;
    } else {
        if (TokCount == 2U) {
            Err = AmbaUtility_StringToUInt32(&(TokenBuf[1][0]), &U32Val);
            if (Err != 0UL) {
                RDA_NG( "AmbaUtility_StringToUInt32 failed %u", Err, 0U);
                AmbaPrint_PrintStr5("%s", &(TokenBuf[1][0]), NULL, NULL, NULL, NULL);
                Rval = RECODER_ERROR_GENERAL_ERROR;
            }

            Afterpoint = (UINT32)AmbaUtility_StringLength(&(TokenBuf[1][0]));

            for (i = 0U; i < Afterpoint; i++) {
                Divisor *= 10U;
            }

            DoubleVal = (DOUBLE)U32Val;
            DoubleVal = DoubleVal / ((DOUBLE)Divisor);

            if (TokenBuf[0][0] != '-') {
                Err = AmbaUtility_StringToUInt32(&(TokenBuf[0][0]), &U32Val);
                if (Err != 0UL) {
                    RDA_NG( "AmbaUtility_StringToUInt32 failed %u", Err, 0U);
                    AmbaPrint_PrintStr5("%s", &(TokenBuf[0][0]), NULL, NULL, NULL, NULL);
                    Rval = RECODER_ERROR_GENERAL_ERROR;
                }

                DoubleVal = DoubleVal + ((DOUBLE)U32Val);
                *pVal = DoubleVal;
            } else {
                Err = AmbaUtility_StringToUInt32(&(TokenBuf[0][1]), &U32Val);
                if (Err != 0UL) {
                    RDA_NG( "AmbaUtility_StringToUInt32 failed %u", Err, 0U);
                    AmbaPrint_PrintStr5("%s", &(TokenBuf[0][1]), NULL, NULL, NULL, NULL);
                    Rval = RECODER_ERROR_GENERAL_ERROR;
                }

                DoubleVal = DoubleVal + ((DOUBLE)U32Val);
                DoubleVal = ((DOUBLE)0.0000) - DoubleVal;
                *pVal = DoubleVal;
            }
        } else {
            RDA_NG( "unknown double format", Err, 0U);
            Rval = RECODER_ERROR_GENERAL_ERROR;
        }
    }

    return Rval;
}

/**
* format text string to canbus data
* @param [in]  pText buffer of text string
* @param [in]  pDataG data structure
*/
void AmbaRscData_TextToCan(const char *pText, AMBA_DATG_s *pDataG)
{
    UINT32              Err, Rval = RECODER_OK, TokCount, Len;
    char                TokenBuf[UTIL_MAX_STR_TOKEN_NUMBER][UTIL_MAX_STR_TOKEN_LENGTH];
    AMBA_DATG_CANBUS_s   *pData = &(pDataG->CanBus);

    /* "T:%u L:%u G:%u XS:%.4f WD:%u XWA:%.4f XWAS:%.4f" */

    /* the first 2 bytes is string length */
    Err = AmbaUtility_StringToken(&(pText[2]), ':', TokenBuf, &TokCount);
    if (Err != 0UL) {
        RDA_NG( "AmbaUtility_StringToken failed %u", Err, 0U);
        Rval = RECODER_ERROR_GENERAL_ERROR;
    } else {
        if (TokCount != 8U) {
            RDA_NG( "unknown canbus data format, TokCount = %u", TokCount, 0U);
            AmbaPrint_PrintStr5("%s", &(pText[2]), NULL, NULL, NULL, NULL);
            Rval = RECODER_ERROR_GENERAL_ERROR;
        }
    }

    /* CapTime */
    if (Rval == RECODER_OK) {
        UINT32 CapTime;

        Len = (UINT32)AmbaUtility_StringLength(&(TokenBuf[1][0]));
        if ((Len >= 2UL) && (Len <= (UINT32)UTIL_MAX_STR_TOKEN_LENGTH)) {
            TokenBuf[1][Len - 2U] = '\0';
            Err = AmbaUtility_StringToUInt32(&(TokenBuf[1][0]), &CapTime);
            if (Err != 0UL) {
                RDA_NG( "AmbaUtility_StringToUInt32 failed %u", Err, 0U);
                AmbaPrint_PrintStr5("%s", &(TokenBuf[1][0]), NULL, NULL, NULL, NULL);
                Rval = RECODER_ERROR_GENERAL_ERROR;
            } else {
                pDataG->CapTime[TICK_TYPE_AUDIO] = CapTime;
            }
        } else {
            RDA_NG( "CapTime Len %u error", Len, 0U);
            Rval = RECODER_ERROR_GENERAL_ERROR;
        }
    }
    /* TurnLightStatus */
    if (Rval == RECODER_OK) {
        UINT32 TurnLightStatus;

        Len = (UINT32)AmbaUtility_StringLength(&(TokenBuf[2][0]));
        if ((Len >= 2UL) && (Len <= (UINT32)UTIL_MAX_STR_TOKEN_LENGTH)) {
            TokenBuf[2][Len - 2U] = '\0';
            Err = AmbaUtility_StringToUInt32(&(TokenBuf[2][0]), &TurnLightStatus);
            if (Err != 0UL) {
                RDA_NG( "AmbaUtility_StringToUInt32 failed %u", Err, 0U);
                AmbaPrint_PrintStr5("%s", &(TokenBuf[2][0]), NULL, NULL, NULL, NULL);
                Rval = RECODER_ERROR_GENERAL_ERROR;
            } else {
                pData->TurnLightStatus = (UINT8)TurnLightStatus;
            }
        } else {
            RDA_NG( "TurnLightStatus Len %u error", Len, 0U);
            Rval = RECODER_ERROR_GENERAL_ERROR;
        }
    }
    /* GearStatus */
    if (Rval == RECODER_OK) {
        UINT32 GearStatus;

        Len = (UINT32)AmbaUtility_StringLength(&(TokenBuf[3][0]));
        if ((Len >= 3UL) && (Len <= (UINT32)UTIL_MAX_STR_TOKEN_LENGTH)) {
            TokenBuf[3][Len - 3U] = '\0';
            Err = AmbaUtility_StringToUInt32(&(TokenBuf[3][0]), &GearStatus);
            if (Err != 0UL) {
                RDA_NG( "AmbaUtility_StringToUInt32 failed %u", Err, 0U);
                AmbaPrint_PrintStr5("%s", &(TokenBuf[3][0]), NULL, NULL, NULL, NULL);
                Rval = RECODER_ERROR_GENERAL_ERROR;
            } else {
                pData->GearStatus = (UINT8)GearStatus;
            }
        } else {
            RDA_NG( "GearStatus Len %u error", Len, 0U);
            Rval = RECODER_ERROR_GENERAL_ERROR;
        }
    }
    /* XferSpeed */
    if (Rval == RECODER_OK) {
        DOUBLE  XferSpeed;

        Len = (UINT32)AmbaUtility_StringLength(&(TokenBuf[4][0]));
        if ((Len >= 3UL) && (Len <= (UINT32)UTIL_MAX_STR_TOKEN_LENGTH)) {
            TokenBuf[4][Len - 3U] = '\0';
            Err = Str2Double(&(TokenBuf[4][0]), &XferSpeed);
            if (RECODER_OK == Err) {
                pData->XferSpeed = XferSpeed;
            } else {
                Rval = RECODER_ERROR_GENERAL_ERROR;
            }
        } else {
            RDA_NG( "XferSpeed Len %u error", Len, 0U);
            Rval = RECODER_ERROR_GENERAL_ERROR;
        }
    }
    /* WheelDir */
    if (Rval == RECODER_OK) {
        UINT32 WheelDir;

        Len = (UINT32)AmbaUtility_StringLength(&(TokenBuf[5][0]));
        if ((Len >= 4UL) && (Len <= (UINT32)UTIL_MAX_STR_TOKEN_LENGTH)) {
            TokenBuf[5][Len - 4U] = '\0';
            Err = AmbaUtility_StringToUInt32(&(TokenBuf[5][0]), &WheelDir);
            if (Err != 0UL) {
                RDA_NG( "AmbaUtility_StringToUInt32 failed %u", Err, 0U);
                AmbaPrint_PrintStr5("%s", &(TokenBuf[5][0]), NULL, NULL, NULL, NULL);
                Rval = RECODER_ERROR_GENERAL_ERROR;
            } else {
                pData->WheelDir = WheelDir;
            }
        } else {
            RDA_NG( "WheelDir Len %u error", Len, 0U);
            Rval = RECODER_ERROR_GENERAL_ERROR;
        }
    }
    /* XferWheelAngle */
    if (Rval == RECODER_OK) {
        DOUBLE  XferWheelAngle;

        Len = (UINT32)AmbaUtility_StringLength(&(TokenBuf[6][0]));
        if ((Len >= 5UL) && (Len <= (UINT32)UTIL_MAX_STR_TOKEN_LENGTH)) {
            TokenBuf[6][Len - 5U] = '\0';
            Err = Str2Double(&(TokenBuf[6][0]), &XferWheelAngle);
            if (RECODER_OK == Err) {
                pData->XferWheelAngle = XferWheelAngle;
            } else {
                Rval = RECODER_ERROR_GENERAL_ERROR;
            }
        } else {
            RDA_NG( "XferWheelAngle Len %u error", Len, 0U);
            Rval = RECODER_ERROR_GENERAL_ERROR;
        }
    }
    /* XferWheelAngleSpeed */
    if (Rval == RECODER_OK) {
        DOUBLE  XferWheelAngleSpeed;

        Err = Str2Double(&(TokenBuf[7][0]), &XferWheelAngleSpeed);
        if (RECODER_OK == Err) {
            pData->XferWheelAngleSpeed = XferWheelAngleSpeed;
        } else {
            Rval = RECODER_ERROR_GENERAL_ERROR;
        }
    }

    AmbaMisra_TouchUnused(&Rval);
}


