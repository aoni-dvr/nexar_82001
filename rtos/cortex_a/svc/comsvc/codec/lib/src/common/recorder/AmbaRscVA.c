/**
 *  @file AmbaRscVA.c
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
 *  @details svc record source - video/audio
 *
 */

#include "AmbaTypes.h"
#include "AmbaKAL.h"
#include "AmbaMisraFix.h"
#include "AmbaWrap.h"
#include "AmbaDef.h"
#include "AmbaDSP.h"
#include "AmbaAudio_AENC.h"
#include "AmbaSYS.h"
#include "AmbaUtility.h"
#include "AmbaPrint.h"
#include "AmbaRecFrwk.h"
#include "AmbaRscInf.h"
#include "AmbaSvcWrap.h"
#include "AmbaRscVA.h"
#include "AmbaCodecCom.h"


#define RVA_DBG_ADV     0

#define RVA_DBG_PRN     1
static inline void RSV_DBG(const char *pFormat, UINT32 Arg1, UINT32 Arg2)
{
#if RVA_DBG_PRN
    AmbaPrint_ModulePrintUInt5(AMBA_RECODER_PRINT_MODULE_ID, pFormat, Arg1, Arg2, 0U, 0U, 0U);
#else
    AmbaMisra_TouchUnused(&pFormat);
    AmbaMisra_TouchUnused(&Arg1);
    AmbaMisra_TouchUnused(&Arg2);
#endif
}

static inline void RSV_NG(const char *pFormat, UINT32 Arg1, UINT32 Arg2)
{
    AmbaPrint_ModulePrintUInt5(AMBA_RECODER_PRINT_MODULE_ID, pFormat, Arg1, Arg2, 0U, 0U, 0U);
}

#define AMBA_RSC_VA_STATE_INVALID    (0U)
#define AMBA_RSC_VA_STATE_IDLE       (1U)
#define AMBA_RSC_VA_STATE_RUN        (2U)
#define AMBA_RSC_VA_STATE_HALT       (3U)

#define AMBA_RSC_WARN_DIV            (2U)    /* half frame */
#define AMBA_RSC_WARN_PRN            (5U)    /* 5 seconds */

#define AMBA_RSC_VID_QUE             AMBA_DSP_ENC_PIC_RDY_s
#define AMBA_RSC_AUD_QUE             AMBA_AENC_AUDIO_DESC_s

typedef struct {
    UINT32                  SrcType;

    void                    *pQueue;
    UINT32                  QueueLen;
    UINT64                  TxNum;
    UINT64                  RxNum;
    AMBA_KAL_MUTEX_t        Mutex;

    AMBA_RSC_STATIS_s       Statis;
    UINT32                  State;
    UINT32                  MakeDummyEos;

    UINT64                  RawDuration;
    UINT64                  HwTimeScale;

    UINT64                  WarningTh;
    UINT32                  WarnPrnTh;
    UINT32                  WarnPrnCount;
} AMBA_RSC_VA_PRIV_s;

static UINT32 MsgQueueCreate(AMBA_RSC_VA_PRIV_s *pPriv, const AMBA_RSC_USR_CFG_s *pUsrCfg)
{
    UINT32       Rval = RECODER_OK, Err, MsgSize = 0U;
    static char  MutexName[] = "RscVAMutex";

    Err = AmbaKAL_MutexCreate(&(pPriv->Mutex), MutexName);
    if (Err != KAL_ERR_NONE) {
        RSV_NG( "AmbaKAL_MutexCreate failed %u", Err, 0U);
        Rval = RECODER_ERROR_GENERAL_ERROR;
    }

    if (pPriv->SrcType == AMBA_RSC_TYPE_VIDEO) {
        MsgSize = (UINT32)sizeof(AMBA_RSC_VID_QUE);
    } else if (pPriv->SrcType == AMBA_RSC_TYPE_AUDIO) {
        MsgSize = (UINT32)sizeof(AMBA_RSC_AUD_QUE);
    } else {
        RSV_NG( "invalid source type %u", pPriv->SrcType, 0U);
        Rval = RECODER_ERROR_GENERAL_ERROR;
    }

    if (Rval == RECODER_OK) {
        pPriv->TxNum = 0U;
        pPriv->RxNum = 0U;
        AmbaMisra_TypeCast(&(pPriv->pQueue), &(pUsrCfg->MemBase));
        pPriv->QueueLen = GetRoundDownValU32(pUsrCfg->MemSize, MsgSize);
    }

    return Rval;
}

static UINT32 MsgQueueDelete(AMBA_RSC_VA_PRIV_s *pPriv)
{
    UINT32       Rval = RECODER_OK, Err;

    Err = AmbaKAL_MutexDelete(&(pPriv->Mutex));
    if (Err != KAL_ERR_NONE) {
        RSV_NG( "AmbaKAL_MutexCreate failed %u", Err, 0U);
        Rval = RECODER_ERROR_GENERAL_ERROR;
    }

    return Rval;
}

static UINT32 MsgQueueTx(AMBA_RSC_VA_PRIV_s *pPriv, const void *pMsg)
{
    UINT32             Rval = RECODER_OK, Err, MtxTake = 0U;
    UINT64             Idx;
    AMBA_RSC_VID_QUE   *pVQue = NULL;
    AMBA_RSC_AUD_QUE   *pAQue = NULL;

    Err = AmbaKAL_MutexTake(&(pPriv->Mutex), 1500U);
    if (Err != RECODER_OK) {
        RSV_NG( "AmbaKAL_MutexTake failed %u", Err, 0U);
        Rval = RECODER_ERROR_GENERAL_ERROR;
    } else {
        MtxTake = 1U;
    }

    if (Rval == RECODER_OK) {
        if (pPriv->SrcType == AMBA_RSC_TYPE_VIDEO) {
            AmbaMisra_TypeCast(&pVQue, &(pPriv->pQueue));
        } else if (pPriv->SrcType == AMBA_RSC_TYPE_AUDIO) {
            AmbaMisra_TypeCast(&pAQue, &(pPriv->pQueue));
        } else {
            RSV_NG( "invalid source type %u", pPriv->SrcType, 0U);
            Rval = RECODER_ERROR_GENERAL_ERROR;
        }
    }

    if (Rval == RECODER_OK) {
        if ((pPriv->TxNum - pPriv->RxNum) >= (UINT64)pPriv->QueueLen) {
            RSV_NG( "Queue full %u/%u", (UINT32)pPriv->TxNum, (UINT32)pPriv->RxNum);
            Rval = RECODER_ERROR_GENERAL_ERROR;
        }
    }

    if (Rval == RECODER_OK) {
        Idx = pPriv->TxNum % (UINT64)pPriv->QueueLen;

        if (pVQue != NULL) {
            const AMBA_RSC_VID_QUE *pVMsg = NULL;

            AmbaMisra_TypeCast(&pVMsg, &pMsg);
            if (pVMsg != NULL) {
                Err = AmbaWrap_memcpy(&(pVQue[Idx]), pVMsg, sizeof(AMBA_RSC_VID_QUE));
                if (Err != RECODER_OK) {
                    RSV_NG( "AmbaWrap_memcpy failed %u", Err, 0U);
                }
            }
        } else if (pAQue != NULL) {
            const AMBA_RSC_AUD_QUE *pAMsg = NULL;

            AmbaMisra_TypeCast(&pAMsg, &pMsg);
            if (pAMsg != NULL) {
                Err = AmbaWrap_memcpy(&(pAQue[Idx]), pAMsg, sizeof(AMBA_RSC_AUD_QUE));
                if (Err != RECODER_OK) {
                    RSV_NG( "AmbaWrap_memcpy failed %u", Err, 0U);
                }
            }
        } else {
            /* should not get here */
        }

        (pPriv->TxNum)++;
    }

    if (MtxTake == 1U) {
        Err = AmbaKAL_MutexGive(&(pPriv->Mutex));
        if (Err != RECODER_OK) {
            RSV_NG( "AmbaKAL_MutexGive failed %u", Err, 0U);
            Rval = RECODER_ERROR_GENERAL_ERROR;
        }
    }

    return Rval;
}

static UINT32 MsgQueueRx(AMBA_RSC_VA_PRIV_s *pPriv, void *pMsg)
{
    UINT32                   Rval = RECODER_OK, Err, MtxTake = 0U;
    UINT64                   Idx;
    const AMBA_RSC_VID_QUE   *pVQue = NULL;
    const AMBA_RSC_AUD_QUE   *pAQue = NULL;

    Err = AmbaKAL_MutexTake(&(pPriv->Mutex), AMBA_KAL_NO_WAIT);
    if (Err != RECODER_OK) {
        // RSV_NG( "AmbaKAL_MutexTake failed %u", Err, 0U);
        Rval = RECODER_ERROR_GENERAL_ERROR;
    } else {
        MtxTake = 1U;
    }

    if (Rval == RECODER_OK) {
        if (pPriv->SrcType == AMBA_RSC_TYPE_VIDEO) {
            AmbaMisra_TypeCast(&pVQue, &(pPriv->pQueue));
        } else if (pPriv->SrcType == AMBA_RSC_TYPE_AUDIO) {
            AmbaMisra_TypeCast(&pAQue, &(pPriv->pQueue));
        } else {
            RSV_NG( "invalid source type %u", pPriv->SrcType, 0U);
            Rval = RECODER_ERROR_GENERAL_ERROR;
        }
    }

    if (Rval == RECODER_OK) {
        if (pPriv->RxNum >= pPriv->TxNum) {
            /* queue empty */
            Rval = RECODER_ERROR_GENERAL_ERROR;
        }
    }

    if (Rval == RECODER_OK) {
        Idx = pPriv->RxNum % (UINT64)pPriv->QueueLen;

        if (pVQue != NULL) {
            Err = AmbaWrap_memcpy(pMsg, &(pVQue[Idx]), sizeof(AMBA_RSC_VID_QUE));
            if (Err != RECODER_OK) {
                RSV_NG( "AmbaWrap_memcpy failed %u", Err, 0U);
            }
        } else if (pAQue != NULL){
            Err = AmbaWrap_memcpy(pMsg, &(pAQue[Idx]), sizeof(AMBA_RSC_AUD_QUE));
            if (Err != RECODER_OK) {
                RSV_NG( "AmbaWrap_memcpy failed %u", Err, 0U);
            }
        } else {
            /* should not get here */
        }

        (pPriv->RxNum)++;
    }

    if (MtxTake == 1U) {
        Err = AmbaKAL_MutexGive(&(pPriv->Mutex));
        if (Err != RECODER_OK) {
            RSV_NG( "AmbaKAL_MutexGive failed %u", Err, 0U);
            Rval = RECODER_ERROR_GENERAL_ERROR;
        }
    }

    return Rval;
}

static UINT32 MsgQueueQuery(const AMBA_REC_SRC_s *pVARsc)
{
    const UINT8                *pPrivData = pVARsc->PrivData;
    UINT32                     Rval = RECODER_OK, QSize = 0U, Err;
    AMBA_RSC_VA_PRIV_s         *pPriv;

    AmbaMisra_TypeCast(&pPriv, &(pPrivData));

    Err = AmbaKAL_MutexTake(&(pPriv->Mutex), 1500U);
    if (Err != RECODER_OK) {
        RSV_NG( "AmbaKAL_MutexTake failed %u", Err, 0U);
        Rval = RECODER_ERROR_GENERAL_ERROR;
    }

    if (Rval == RECODER_OK) {
        QSize = (UINT32)(pPriv->TxNum - pPriv->RxNum);

        Err = AmbaKAL_MutexGive(&(pPriv->Mutex));
        if (Err != RECODER_OK) {
            RSV_NG( "AmbaKAL_MutexGive failed %u", Err, 0U);
            // Rval = RECODER_ERROR_GENERAL_ERROR;
        }
    }

    return QSize;
}

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

static void CmpFrameTime(const AMBA_RSC_VA_PRIV_s *pPriv, UINT64 Diff, UINT32 SrcType, UINT32 Id)
{
#if RVA_DBG_ADV
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

        if (SrcType == AMBA_RSC_TYPE_VIDEO) {

            // [v%d] %s %llums
            Err = AmbaWrap_memset(AdvStrBuf, 0, (UINT32)sizeof(AdvStrBuf));
            if (Err != RECODER_OK) {
                RSV_NG( "AmbaWrap_memset failed %u", Err, 0U);
            }
            AmbaUtility_StringAppend(AdvStrBuf, (UINT32)sizeof(AdvStrBuf), "[v");
            AmbaUtility_StringAppendUInt32(AdvStrBuf, (UINT32)sizeof(AdvStrBuf), Id, 10U);
            AmbaUtility_StringAppend(AdvStrBuf, (UINT32)sizeof(AdvStrBuf), "] ");
            AmbaUtility_StringAppend(AdvStrBuf, (UINT32)sizeof(AdvStrBuf), ((0U < IsSlow) ? "slow" : "fast"));
            AmbaUtility_StringAppend(AdvStrBuf, (UINT32)sizeof(AdvStrBuf), " ");
            CurStrLen = (UINT32)AmbaUtility_StringLength(AdvStrBuf);
            if (CurStrLen < (UINT32)sizeof(AdvStrBuf)) {
                Err = AmbaUtility_UInt64ToStr(&(AdvStrBuf[CurStrLen]), (UINT32)sizeof(AdvStrBuf) - CurStrLen,
                        TimeMs, 10U);
                if (Err == 0U) {
                    RSV_NG( "AmbaUtility_UInt64ToStr failed %u", Err, 0U);
                }
            }
            AmbaUtility_StringAppend(AdvStrBuf, (UINT32)sizeof(AdvStrBuf), "ms");

            AmbaPrint_PrintStr5(AdvStrBuf, NULL, NULL, NULL, NULL, NULL);
        } else if (SrcType == AMBA_RSC_TYPE_AUDIO) {

            // [a] %s %llums
            Err = AmbaWrap_memset(AdvStrBuf, 0, (UINT32)sizeof(AdvStrBuf));
            if (Err == 0U) {
                RSV_NG( "AmbaUtility_UInt64ToStr failed %u", Err, 0U);
            }
            AmbaUtility_StringAppend(AdvStrBuf, (UINT32)sizeof(AdvStrBuf), "[a] ");
            AmbaUtility_StringAppend(AdvStrBuf, (UINT32)sizeof(AdvStrBuf), ((0U < IsSlow) ? "slow" : "fast"));
            AmbaUtility_StringAppend(AdvStrBuf, (UINT32)sizeof(AdvStrBuf), " ");
            CurStrLen = (UINT32)AmbaUtility_StringLength(AdvStrBuf);
            if (CurStrLen < (UINT32)sizeof(AdvStrBuf)) {
                Err = AmbaUtility_UInt64ToStr(&(AdvStrBuf[CurStrLen]), (UINT32)sizeof(AdvStrBuf) - CurStrLen,
                        TimeMs, 10U);
                if (Err == 0U) {
                    RSV_NG( "AmbaUtility_UInt64ToStr failed %u", Err, 0U);
                }
            }
            AmbaUtility_StringAppend(AdvStrBuf, (UINT32)sizeof(AdvStrBuf), "ms");

            AmbaPrint_PrintStr5(AdvStrBuf, NULL, NULL, NULL, NULL, NULL);
        } else {
            /* do nothing */
        }
    }
#else
    /* do nothing */
    AmbaMisra_TouchUnused(&pPriv);
    AmbaMisra_TouchUnused(&Diff);
    AmbaMisra_TouchUnused(&SrcType);
    AmbaMisra_TouchUnused(&Id);
#endif
}

static UINT64 CalcLtTime(UINT64 BaseTime, UINT64 IncTime, UINT32 *pIsCarry)
{
    UINT64  CalcTime;
    char AdvStrBuf[70];
    UINT32 CurStrLen, Err;

    CalcTime = BaseTime + IncTime;
    *pIsCarry = 0U;

    if (CalcTime < BaseTime) {
        *pIsCarry = 1U;

        // Carry, PreTime/CurTime/(%llu/%llu)
        Err = AmbaWrap_memset(AdvStrBuf, 0, (UINT32)sizeof(AdvStrBuf));
        if (Err != RECODER_OK) {
            RSV_NG( "AmbaWrap_memset failed %u", Err, 0U);
        }
        AmbaUtility_StringAppend(AdvStrBuf, (UINT32)sizeof(AdvStrBuf), "Carry, PreTime/CurTime/(");
        CurStrLen = (UINT32)AmbaUtility_StringLength(AdvStrBuf);
        if (CurStrLen < (UINT32)sizeof(AdvStrBuf)) {
            Err = AmbaUtility_UInt64ToStr(&(AdvStrBuf[CurStrLen]), (UINT32)sizeof(AdvStrBuf) - CurStrLen,
                    BaseTime, 10U);
            if (Err == 0U) {
                RSV_NG( "AmbaUtility_UInt64ToStr failed %u", Err, 0U);
            }
        }
        AmbaUtility_StringAppend(AdvStrBuf, (UINT32)sizeof(AdvStrBuf), "/");
        CurStrLen = (UINT32)AmbaUtility_StringLength(AdvStrBuf);
        if (CurStrLen < (UINT32)sizeof(AdvStrBuf)) {
            Err = AmbaUtility_UInt64ToStr(&(AdvStrBuf[CurStrLen]), (UINT32)sizeof(AdvStrBuf) - CurStrLen,
                    CalcTime, 10U);
            if (Err == 0U) {
                RSV_NG( "AmbaUtility_UInt64ToStr failed %u", Err, 0U);
            }
        }
        AmbaUtility_StringAppend(AdvStrBuf, (UINT32)sizeof(AdvStrBuf), ")");

        AmbaPrint_PrintStr5(AdvStrBuf, NULL, NULL, NULL, NULL, NULL);
    }

    return CalcTime;
}

static void CmpLtTime(AMBA_RSC_VA_PRIV_s *pPriv,
                      const AMBA_RSC_STATIS_s *pStatis,
                      UINT32 SrcType,
                      UINT32 Id)
{
#if RVA_DBG_ADV
    UINT32  IsSlow, Err;
    UINT64  Diff, TimeMs;
    char AdvStrBuf[200];
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

        AmbaAdvPrint("[%s_lt]%s_%llums, ExpCarry/ExpTime/HwCarry/HwTime(%llu/%llu/%llu/%llu)", ((SrcType == AMBA_RSC_TYPE_VIDEO) ? "v" : "a"),
         ((0U < IsSlow) ? "slow" : "fast"), TimeMs, pStatis->RxLtExpTimeCarry, pStatis->RxLtExpTime, pStatis->RxLtHwTimeCarry, pStatis->RxLtHwTime);
    }
#endif

    pPriv->WarnPrnCount++;
    if (pPriv->WarnPrnTh <= pPriv->WarnPrnCount) {
        pPriv->WarnPrnCount = 0U;

        if (pPriv->WarningTh < Diff) {
            TimeMs = GetRoundUpValU64((Diff * 1000U), pPriv->HwTimeScale);
            if (SrcType == AMBA_RSC_TYPE_VIDEO) {

                // [v%d_lt] %s %llums, ExpTime/HwTime(%llu/%llu)
                Err = AmbaWrap_memset(AdvStrBuf, 0, sizeof(AdvStrBuf));
                if (Err != RECODER_OK) {
                    RSV_NG( "AmbaWrap_memset failed %u", Err, 0U);
                }
                AmbaUtility_StringAppend(AdvStrBuf, (UINT32)sizeof(AdvStrBuf), "[v");
                AmbaUtility_StringAppendUInt32(AdvStrBuf, (UINT32)sizeof(AdvStrBuf), Id, 10U);
                AmbaUtility_StringAppend(AdvStrBuf, (UINT32)sizeof(AdvStrBuf), "_lt] ");
                AmbaUtility_StringAppend(AdvStrBuf, (UINT32)sizeof(AdvStrBuf),  ((0U < IsSlow) ? "slow " : "fast "));
                CurStrLen = (UINT32)AmbaUtility_StringLength(AdvStrBuf);
                if (CurStrLen < (UINT32)sizeof(AdvStrBuf)) {
                    Err = AmbaUtility_UInt64ToStr(&(AdvStrBuf[CurStrLen]), (UINT32)sizeof(AdvStrBuf) - CurStrLen,
                            TimeMs, 10U);
                    if (Err == 0U) {
                        RSV_NG( "AmbaUtility_UInt64ToStr failed %u", Err, 0U);
                    }
                }
                AmbaUtility_StringAppend(AdvStrBuf, (UINT32)sizeof(AdvStrBuf), "ms, ExpTime/HwTime(");
                CurStrLen = (UINT32)AmbaUtility_StringLength(AdvStrBuf);
                if (CurStrLen < (UINT32)sizeof(AdvStrBuf)) {
                    Err = AmbaUtility_UInt64ToStr(&(AdvStrBuf[CurStrLen]), (UINT32)sizeof(AdvStrBuf) - CurStrLen,
                            pStatis->RxLtExpTime, 10U);
                    if (Err == 0U) {
                        RSV_NG( "AmbaUtility_UInt64ToStr failed %u", Err, 0U);
                    }
                }
                AmbaUtility_StringAppend(AdvStrBuf, (UINT32)sizeof(AdvStrBuf), "/");
                CurStrLen = (UINT32)AmbaUtility_StringLength(AdvStrBuf);
                if (CurStrLen < (UINT32)sizeof(AdvStrBuf)) {
                    Err = AmbaUtility_UInt64ToStr(&(AdvStrBuf[CurStrLen]), (UINT32)sizeof(AdvStrBuf) - CurStrLen,
                            pStatis->RxLtHwTime, 10U);
                    if (Err == 0U) {
                        RSV_NG( "AmbaUtility_UInt64ToStr failed %u", Err, 0U);
                    }
                }
                AmbaUtility_StringAppend(AdvStrBuf, (UINT32)sizeof(AdvStrBuf), ")");

                AmbaPrint_PrintStr5(AdvStrBuf, NULL, NULL, NULL, NULL, NULL);
            } else if (SrcType == AMBA_RSC_TYPE_AUDIO) {
                // [a_lt] %s %llums, ExpTime/HwTime(%llu/%llu)
                Err = AmbaWrap_memset(AdvStrBuf, 0, sizeof(AdvStrBuf));
                if (Err != RECODER_OK) {
                    RSV_NG( "AmbaWrap_memset failed %u", Err, 0U);
                }
                AmbaUtility_StringAppend(AdvStrBuf, (UINT32)sizeof(AdvStrBuf), "[a_lt] ");
                AmbaUtility_StringAppend(AdvStrBuf, (UINT32)sizeof(AdvStrBuf),  ((0U < IsSlow) ? "slow " : "fast "));
                CurStrLen = (UINT32)AmbaUtility_StringLength(AdvStrBuf);
                if (CurStrLen < (UINT32)sizeof(AdvStrBuf)) {
                    Err = AmbaUtility_UInt64ToStr(&(AdvStrBuf[CurStrLen]), (UINT32)sizeof(AdvStrBuf) - CurStrLen,
                            TimeMs, 10U);
                    if (Err == 0U) {
                        RSV_NG( "AmbaUtility_UInt64ToStr failed %u", Err, 0U);
                    }
                }
                AmbaUtility_StringAppend(AdvStrBuf, (UINT32)sizeof(AdvStrBuf), "ms, ExpTime/HwTime(");
                CurStrLen = (UINT32)AmbaUtility_StringLength(AdvStrBuf);
                if (CurStrLen < (UINT32)sizeof(AdvStrBuf)) {
                    Err = AmbaUtility_UInt64ToStr(&(AdvStrBuf[CurStrLen]), (UINT32)sizeof(AdvStrBuf) - CurStrLen,
                            pStatis->RxLtExpTime, 10U);
                    if (Err == 0U) {
                        RSV_NG( "AmbaUtility_UInt64ToStr failed %u", Err, 0U);
                    }
                }
                AmbaUtility_StringAppend(AdvStrBuf, (UINT32)sizeof(AdvStrBuf), "/");
                CurStrLen = (UINT32)AmbaUtility_StringLength(AdvStrBuf);
                if (CurStrLen < (UINT32)sizeof(AdvStrBuf)) {
                    Err = AmbaUtility_UInt64ToStr(&(AdvStrBuf[CurStrLen]), (UINT32)sizeof(AdvStrBuf) - CurStrLen,
                            pStatis->RxLtHwTime, 10U);
                    if (Err == 0U) {
                        RSV_NG( "AmbaUtility_UInt64ToStr failed %u", Err, 0U);
                    }
                }
                AmbaUtility_StringAppend(AdvStrBuf, (UINT32)sizeof(AdvStrBuf), ")");

                AmbaPrint_PrintStr5(AdvStrBuf, NULL, NULL, NULL, NULL, NULL);
            } else {
                /* do nothing */
            }
        }
    }
#else
    AmbaMisra_TouchUnused(pPriv);
    (void)pStatis;
    (void)SrcType;
    (void)Id;
#endif
}

static UINT32 EnQueue(const AMBA_REC_SRC_s *pVARsc, void *pDataInfo)
{
    const UINT8                   *pPrivData = pVARsc->PrivData;
    UINT32                        Rval = RECODER_OK, DataSize = 0U, IsEos = 0U, IsCarry, SrcType, Err;
    UINT64                        HwTime = 0U, Diff;
    AMBA_RSC_STATIS_s             Statis;
    AMBA_RSC_VA_PRIV_s            *pPriv;
    const AMBA_RSC_VID_QUE        *pVDesc = NULL;
    const AMBA_RSC_AUD_QUE        *pADesc = NULL;
    char AdvStrBuf[150];
    UINT32 CurStrLen;

    AmbaMisra_TouchUnused(pDataInfo);

    AmbaMisra_TypeCast(&pPriv, &(pPrivData));
    SrcType = pVARsc->UsrCfg.SrcType;
    Statis = pPriv->Statis;

    if (pPriv->State == AMBA_RSC_VA_STATE_IDLE) {
        /* [idle] -> [run] */
        pPriv->State = AMBA_RSC_VA_STATE_RUN;
    }

    if (pPriv->State == AMBA_RSC_VA_STATE_RUN) {
        if (SrcType == AMBA_RSC_TYPE_VIDEO) {
            AmbaMisra_TypeCast(&pVDesc, &pDataInfo);

            if (pVDesc->PicSize != AMBA_DSP_ENC_END_MARK) {
                DataSize = pVDesc->PicSize;
                if ((pVDesc->TileIdx == 0U) && (pVDesc->SliceIdx == 0U)) {
                    if (pVDesc->FrameType != PIC_FRAME_B) {
                        HwTime = pVDesc->CaptureTimeStamp;
                    }
                    Statis.RxCount++;
                }
            } else {
                IsEos = 1U;
            }
        } else if (SrcType == AMBA_RSC_TYPE_AUDIO) {
            AmbaMisra_TypeCast(&pADesc, &pDataInfo);

            if (pADesc->Eos == 0U) {
                DataSize = pADesc->DataSize;
                HwTime = pADesc->AudioTicks;

                Statis.RxCount++;
            } else {
                IsEos = 1U;
            }
        } else {
            /* do nothing */
            Rval = RECODER_ERROR_GENERAL_ERROR;
        }

        if (0U < DataSize) {
            Statis.RxSize += DataSize;
        }

        /* monitor incoming hw time */
        if (0U < HwTime) {
            Diff = DiffFrameTime(&Statis, HwTime);
#if RVA_DBG_ADV
            {
                UINT64  TimeMs = GetRoundUpValU64((Diff * 1000U), pPriv->HwTimeScale);
                char AdvStrBuf[128];
                UINT32 CurStrLen;

                // [%s]%llums, PrevHwTime/CurrHwTime(%llu/%llu)
                Err = AmbaWrap_memset(AdvStrBuf, 0, (UINT32)sizeof(AdvStrBuf));
                if (Err != RECODER_OK) {
                    RSV_NG( "AmbaWrap_memset failed %u", Err, 0U);
                }
                AmbaUtility_StringAppend(AdvStrBuf, (UINT32)sizeof(AdvStrBuf), ((SrcType == AMBA_RSC_TYPE_VIDEO) ? "[v]" : "[a]"));
                CurStrLen = (UINT32)AmbaUtility_StringLength(AdvStrBuf);
                if (CurStrLen < (UINT32)sizeof(AdvStrBuf)) {
                    Err = AmbaUtility_UInt64ToStr(&(AdvStrBuf[CurStrLen]), (UINT32)sizeof(AdvStrBuf) - CurStrLen,
                            TimeMs, 10U);
                    if (Err == 0U) {
                        RSV_NG( "AmbaUtility_UInt64ToStr failed %u", Err, 0U);
                    }
                }
                AmbaUtility_StringAppend(AdvStrBuf, (UINT32)sizeof(AdvStrBuf), "ms, PrevHwTime/CurrHwTime(");
                CurStrLen = (UINT32)AmbaUtility_StringLength(AdvStrBuf);
                if (CurStrLen < (UINT32)sizeof(AdvStrBuf)) {
                    Err = AmbaUtility_UInt64ToStr(&(AdvStrBuf[CurStrLen]), (UINT32)sizeof(AdvStrBuf) - CurStrLen,
                            Statis.RxHwTime, 10U);
                    if (Err == 0U) {
                        RSV_NG( "AmbaUtility_UInt64ToStr failed %u", Err, 0U);
                    }
                }
                AmbaUtility_StringAppend(AdvStrBuf, (UINT32)sizeof(AdvStrBuf), "/");
                CurStrLen = (UINT32)AmbaUtility_StringLength(AdvStrBuf);
                if (CurStrLen < (UINT32)sizeof(AdvStrBuf)) {
                    Err = AmbaUtility_UInt64ToStr(&(AdvStrBuf[CurStrLen]), (UINT32)sizeof(AdvStrBuf) - CurStrLen,
                            HwTime, 10U);
                    if (Err == 0U) {
                        RSV_NG( "AmbaUtility_UInt64ToStr failed %u", Err, 0U);
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
                if (SrcType == AMBA_RSC_TYPE_VIDEO) {
                    CmpFrameTime(pPriv, Diff, SrcType, pVDesc->StreamId);
                } else if (SrcType == AMBA_RSC_TYPE_AUDIO) {
                    CmpFrameTime(pPriv, Diff, SrcType, 0U);
                } else {
                    /* do nothing */
                }
            }

            /* check long-term diff */
            if (0U < Statis.RxLtHwTime) {
                /* calc. long-term expect_time */
                Statis.RxLtExpTime = CalcLtTime(Statis.RxLtExpTime, pPriv->RawDuration, &IsCarry);
                if (0U < IsCarry) {
                    Statis.RxLtExpTimeCarry++;
                }

                if (SrcType == AMBA_RSC_TYPE_VIDEO) {
                    CmpLtTime(pPriv, &Statis, SrcType, pVDesc->StreamId);
                } else if (SrcType == AMBA_RSC_TYPE_AUDIO) {
                    CmpLtTime(pPriv, &Statis, SrcType, 0U);
                } else {
                    /* do nothing */
                }
            }
        }

        if (Rval == RECODER_OK) {
            Rval = MsgQueueTx(pPriv, pDataInfo);
            if (Rval != RECODER_OK) {
                if (SrcType == AMBA_RSC_TYPE_VIDEO) {
                    RSV_NG( "[video%d] fail to enqueue", pVDesc->StreamId, 0U);
                } else if (SrcType == AMBA_RSC_TYPE_AUDIO) {
                    RSV_NG( "[audio] fail to enqueue", 0U, 0U);
                } else {
                    /* do nothing */
                }
            }
        }

        if (0U < IsEos) {
            /* [run] -> [halt] */
            pPriv->State = AMBA_RSC_VA_STATE_HALT;
            RSV_DBG("rsc got eos", 0U, 0U);
        } else {
            if (0U < pPriv->MakeDummyEos) {
                if (SrcType == AMBA_RSC_TYPE_AUDIO) {
                    AMBA_RSC_AUD_QUE         ADesc;

                    Rval = AmbaWrap_memset(&ADesc, 0, sizeof(AMBA_RSC_AUD_QUE));
                    if (Rval != RECODER_OK) {
                        RSV_NG( "AmbaWrap_memset failed %u", Rval, 0U);
                    }

                    /* insert dummy eos if needed */
                    ADesc.Eos = 1U;
                    Rval = MsgQueueTx(pPriv, &ADesc);
                    if (Rval == RECODER_OK) {
                        RSV_DBG("dummy eos is inserted", 0U, 0U);
                    } else {
                        RSV_NG( "fail to enqueue", 0U, 0U);
                    }

                    /* [run] -> [halt] */
                    pPriv->State = AMBA_RSC_VA_STATE_HALT;
                }
                pPriv->MakeDummyEos = 0U;
            }
        }

        if (pPriv->State == AMBA_RSC_VA_STATE_HALT) {
            UINT64  BRate;
            DOUBLE  Frate;

            /* calc. total frame-rate/bitrate */
            Frate = ((DOUBLE)Statis.RxCount - 1.0);
            Frate *= (DOUBLE)pPriv->HwTimeScale;
            Frate /= (DOUBLE)Statis.RxLtHwTime;
            BRate = (Statis.RxSize * 8U);
            BRate = (BRate * pPriv->HwTimeScale) / Statis.RxLtHwTime;

            if (SrcType == AMBA_RSC_TYPE_VIDEO) {
                UINT32 Temp = 0U;

                AmbaMisra_TypeCast(&(Temp), &(pVDesc->StreamId)); Temp &= 0xFFU;

                // ##[v%d] frate/brate(%.3ffps/%llubps), frames/size(%llu/%llubytes)
                Err = AmbaWrap_memset(AdvStrBuf, 0, sizeof(AdvStrBuf));
                if (Err != RECODER_OK) {
                    RSV_NG( "AmbaWrap_memset failed %u", Err, 0U);
                }
                AmbaUtility_StringAppend(AdvStrBuf, (UINT32)sizeof(AdvStrBuf), "##[v");
                AmbaUtility_StringAppendUInt32(AdvStrBuf, (UINT32)sizeof(AdvStrBuf), Temp, 10U);
                AmbaUtility_StringAppend(AdvStrBuf, (UINT32)sizeof(AdvStrBuf), "] frate/brate(");
                AmbaUtility_StringAppendDouble(AdvStrBuf, (UINT32)sizeof(AdvStrBuf), Frate, 3U);
                AmbaUtility_StringAppend(AdvStrBuf, (UINT32)sizeof(AdvStrBuf), "fps/");
                CurStrLen = (UINT32)AmbaUtility_StringLength(AdvStrBuf);
                if (CurStrLen < (UINT32)sizeof(AdvStrBuf)) {
                    Err = AmbaUtility_UInt64ToStr(&(AdvStrBuf[CurStrLen]), (UINT32)sizeof(AdvStrBuf) - CurStrLen,
                            BRate, 10U);
                    if (Err == 0U) {
                        RSV_NG( "AmbaUtility_UInt64ToStr failed %u", Err, 0U);
                    }
                }
                AmbaUtility_StringAppend(AdvStrBuf, (UINT32)sizeof(AdvStrBuf), "bps), frames/size(");
                CurStrLen = (UINT32)AmbaUtility_StringLength(AdvStrBuf);
                if (CurStrLen < (UINT32)sizeof(AdvStrBuf)) {
                    Err = AmbaUtility_UInt64ToStr(&(AdvStrBuf[CurStrLen]), (UINT32)sizeof(AdvStrBuf) - CurStrLen,
                            Statis.RxCount, 10U);
                    if (Err == 0U) {
                        RSV_NG( "AmbaUtility_UInt64ToStr failed %u", Err, 0U);
                    }
                }
                AmbaUtility_StringAppend(AdvStrBuf, (UINT32)sizeof(AdvStrBuf), "/");
                CurStrLen = (UINT32)AmbaUtility_StringLength(AdvStrBuf);
                if (CurStrLen < (UINT32)sizeof(AdvStrBuf)) {
                    Err = AmbaUtility_UInt64ToStr(&(AdvStrBuf[CurStrLen]), (UINT32)sizeof(AdvStrBuf) - CurStrLen,
                            Statis.RxSize, 10U);
                    if (Err == 0U) {
                        RSV_NG( "AmbaUtility_UInt64ToStr failed %u", Err, 0U);
                    }
                }
                AmbaUtility_StringAppend(AdvStrBuf, (UINT32)sizeof(AdvStrBuf), "bytes)");

                AmbaPrint_PrintStr5(AdvStrBuf, NULL, NULL, NULL, NULL, NULL);
            } else if (SrcType == AMBA_RSC_TYPE_AUDIO) {

                // ##[a] frate/brate(%.3ffps/%llubps), frames/size(%llu/%llubytes)
                Err = AmbaWrap_memset(AdvStrBuf, 0, (UINT32)sizeof(AdvStrBuf));
                if (Err != RECODER_OK) {
                    RSV_NG( "AmbaWrap_memset failed %u", Err, 0U);
                }
                AmbaUtility_StringAppend(AdvStrBuf, (UINT32)sizeof(AdvStrBuf), "##[a] frate/brate(");
                AmbaUtility_StringAppendDouble(AdvStrBuf, (UINT32)sizeof(AdvStrBuf), Frate, 3U);
                AmbaUtility_StringAppend(AdvStrBuf, (UINT32)sizeof(AdvStrBuf), "fps/");
                CurStrLen = (UINT32)AmbaUtility_StringLength(AdvStrBuf);
                if (CurStrLen < (UINT32)sizeof(AdvStrBuf)) {
                    Err = AmbaUtility_UInt64ToStr(&(AdvStrBuf[CurStrLen]), (UINT32)sizeof(AdvStrBuf) - CurStrLen,
                            BRate, 10U);
                    if (Err == 0U) {
                        RSV_NG( "AmbaUtility_UInt64ToStr failed %u", Err, 0U);
                    }
                }
                AmbaUtility_StringAppend(AdvStrBuf, (UINT32)sizeof(AdvStrBuf), "bps), frames/size(");
                CurStrLen = (UINT32)AmbaUtility_StringLength(AdvStrBuf);
                if (CurStrLen < (UINT32)sizeof(AdvStrBuf)) {
                    Err = AmbaUtility_UInt64ToStr(&(AdvStrBuf[CurStrLen]), (UINT32)sizeof(AdvStrBuf) - CurStrLen,
                            Statis.RxCount, 10U);
                    if (Err == 0U) {
                        RSV_NG( "AmbaUtility_UInt64ToStr failed %u", Err, 0U);
                    }
                }
                AmbaUtility_StringAppend(AdvStrBuf, (UINT32)sizeof(AdvStrBuf), "/");
                CurStrLen = (UINT32)AmbaUtility_StringLength(AdvStrBuf);
                if (CurStrLen < (UINT32)sizeof(AdvStrBuf)) {
                    Err = AmbaUtility_UInt64ToStr(&(AdvStrBuf[CurStrLen]), (UINT32)sizeof(AdvStrBuf) - CurStrLen,
                            Statis.RxSize, 10U);
                    if (Err == 0U) {
                        RSV_NG( "AmbaUtility_UInt64ToStr failed %u", Err, 0U);
                    }
                }
                AmbaUtility_StringAppend(AdvStrBuf, (UINT32)sizeof(AdvStrBuf), "bytes)");

                AmbaPrint_PrintStr5(AdvStrBuf, NULL, NULL, NULL, NULL, NULL);
            } else {
                /* do nothing */
            }
        }
    }

    pPriv->Statis = Statis;
    return Rval;
}

static UINT32 DeQueue(const AMBA_REC_SRC_s *pVARsc, AMBA_REC_FRWK_DESC_s *pDesc)
{
    const UINT8        *pPrivData = pVARsc->PrivData;
    UINT32             Rval;
    AMBA_RSC_VA_PRIV_s  *pPriv;

    AmbaMisra_TypeCast(&pPriv, &(pPrivData));

    Rval = MsgQueueRx(pPriv, pDesc->pSrcData);
    if (Rval == RECODER_OK) {
        pDesc->SrcBit     = pVARsc->SrcBit;
        pDesc->SrcType    = pVARsc->UsrCfg.SrcType;
        pDesc->SrcSubType = pVARsc->UsrCfg.SubType;
        pDesc->SrcBufBase = pVARsc->UsrCfg.BsBufBase;
        pDesc->SrcBufSize = pVARsc->UsrCfg.BsBufSize;
    }

    return Rval;
}

/**
* record source creation
* @param [in]  pVARsc source info
* @return ErrorCode
*/
UINT32 AmbaRscVA_Create(AMBA_REC_SRC_s *pVARsc)
{
    static UINT32             RscVAPriv = (UINT32)sizeof(AMBA_RSC_VA_PRIV_s);
    const UINT8               *pPrivData = pVARsc->PrivData;
    UINT32                    Rval = RECODER_ERROR_GENERAL_ERROR, Reserved = CONFIG_AMBA_REC_RSC_PRIV_SIZE;
    UINT32                    MsgSize = 0U, AudioClk;
    AMBA_RSC_VA_PRIV_s        *pPriv;
    const AMBA_RSC_USR_CFG_s  *pUsrCfg;

    AmbaMisra_TypeCast(&pPriv, &(pPrivData));
    if (RscVAPriv <= Reserved) {
        pVARsc->pfnEnque   = EnQueue;
        pVARsc->pfnDeque   = DeQueue;
        pVARsc->pfnQueSize = MsgQueueQuery;

        /* reset parameters */
        Rval = AmbaWrap_memset(pPriv, 0, sizeof(AMBA_RSC_VA_PRIV_s));
        if (Rval != RECODER_OK) {
            RSV_NG( "AmbaWrap_memset failed %u", Rval, 0U);
        }

        pUsrCfg = &(pVARsc->UsrCfg);
        if (0U < pUsrCfg->MemSize) {
            if (pUsrCfg->SrcType == AMBA_RSC_TYPE_VIDEO) {
                MsgSize = (UINT32)sizeof(AMBA_RSC_VID_QUE);
                pPriv->SrcType = AMBA_RSC_TYPE_VIDEO;
            } else if (pUsrCfg->SrcType == AMBA_RSC_TYPE_AUDIO) {
                MsgSize = (UINT32)sizeof(AMBA_RSC_AUD_QUE);
                pPriv->SrcType = AMBA_RSC_TYPE_AUDIO;
            } else {
                /* do nothing */
            }

            if (0U < MsgSize) {
                /* calc. warning threshold */
                AudioClk = 12288000U;   /* 12.288 MHz */
                if (AmbaSYS_GetClkFreq(AMBA_SYS_CLK_AUD_0, &AudioClk) != SYS_ERR_NONE) {
                    RSV_NG( "fail to get audio clock", 0U, 0U);
                }

                pPriv->HwTimeScale = AudioClk;
                pPriv->RawDuration = AudioClk;
                pPriv->RawDuration *= pUsrCfg->NumUnitsInTick;
                pPriv->RawDuration /= pUsrCfg->TimeScale;

                pPriv->WarningTh = pPriv->RawDuration / AMBA_RSC_WARN_DIV;
                RSV_DBG("RawDur/WarnTh(%u/%u)", (UINT32)pPriv->RawDuration
                                                          , (UINT32)pPriv->WarningTh);
                pPriv->WarnPrnTh = GetRoundUpValU32(pUsrCfg->TimeScale, pUsrCfg->NumUnitsInTick);
                pPriv->WarnPrnTh *= AMBA_RSC_WARN_PRN;

                /* create queue */
                Rval = MsgQueueCreate(pPriv, pUsrCfg);
                if (Rval == RECODER_OK) {
                    /* [invalid] -> [idle] */
                    pPriv->State = AMBA_RSC_VA_STATE_IDLE;
                }
            }
        } else {
            RSV_NG( "no memory to create queue", 0U, 0U);
        }
    }

    RSV_DBG( "MsgSize(%u)", MsgSize, 0U);
    if (Rval != RECODER_OK) {
        RSV_NG( "fail to create video source, (%u/%u)", RscVAPriv, Reserved);
    }

    RSV_DBG("rsc is created!", 0U, 0U);
    return Rval;
}

/**
* record source destroying
* @param [in]  pVARsc source info
* @return ErrorCode
*/
UINT32 AmbaRscVA_Destroy(const AMBA_REC_SRC_s *pVARsc)
{
    const UINT8        *pPrivData = pVARsc->PrivData;
    UINT32             Rval = RECODER_ERROR_GENERAL_ERROR;
    AMBA_RSC_VA_PRIV_s  *pPriv;

    AmbaMisra_TypeCast(&pPriv, &(pPrivData));
    if ((pPriv->State == AMBA_RSC_VA_STATE_IDLE) || (pPriv->State == AMBA_RSC_VA_STATE_HALT)) {
        Rval = MsgQueueDelete(pPriv);
        if (Rval != RECODER_OK) {
            RSV_NG( "fail to destroy record source video", 0U, 0U);
        }

        /* [idle] -> [invalid] */
        pPriv->State = AMBA_RSC_VA_STATE_INVALID;

        RSV_DBG("rsc is destroyed, RxCount/RxSize(%X/%X)", (UINT32)pPriv->Statis.RxCount,
                                                        (UINT32)pPriv->Statis.RxSize);
    } else {
        RSV_NG( "rsc isn't in idle state(%u)", pPriv->State, 0U);
    }

    return Rval;
}

/**
* record master control
* @param [in]  pVARsc source info
* @param [in]  CtrlType control type
* @param [in]  pParam pointer of control parameter
*/
void AmbaRscVA_Control(const AMBA_REC_SRC_s *pVARsc, UINT32 CtrlType, void *pParam)
{
    const UINT8         *pPrivData = pVARsc->PrivData;
    UINT32              Rval;
    AMBA_RSC_VA_PRIV_s  *pPriv;

    AmbaMisra_TypeCast(&pPriv, &(pPrivData));

    switch (CtrlType) {
    case AMBA_RSC_VA_STATIS_GET:
        pPriv->Statis.QueLen = MsgQueueQuery(pVARsc);

        Rval = AmbaWrap_memcpy(pParam, &(pPriv->Statis), sizeof(AMBA_RSC_STATIS_s));
        if (Rval != RECODER_OK) {
            RSV_NG( "AmbaWrap_memcpy failed %u", Rval, 0U);
        }
        break;
    case AMBA_RSC_VA_RESET:
        if ((pPriv->State == AMBA_RSC_VA_STATE_IDLE) || (pPriv->State == AMBA_RSC_VA_STATE_HALT)) {
            /* reset statis */
            Rval = AmbaWrap_memset(&(pPriv->Statis), 0, sizeof(AMBA_RSC_STATIS_s));
            if (Rval != RECODER_OK) {
                RSV_NG( "AmbaWrap_memset failed %u", Rval, 0U);
            }

            /* [halt] -> [idle] */
            pPriv->State = AMBA_RSC_VA_STATE_IDLE;
        } else {
            RSV_NG( "rsc isn't in idle/halt state(%u)", pPriv->State, 0U);
        }
        break;
    case AMBA_RSC_VA_DUMMY_EOS:
        if (pPriv->State == AMBA_RSC_VA_STATE_RUN) {
            pPriv->MakeDummyEos = 1U;
        }
        break;
    default:
        RSV_NG( "unknown control type", 0U, 0U);
        break;
    }
}

/**
* query function of video record source
* @param [in]  SrcType source type
* @param [in]  MaxQueLen max msg queue number
* @param [out]  pSize memory size
*/
void AmbaRscVA_EvalMemSize(UINT32 SrcType, UINT32 MaxQueLen, UINT32 *pSize)
{
    UINT32  Val = 0U, MsgSize = 0U;

    if (SrcType == AMBA_RSC_TYPE_VIDEO) {
        MsgSize = (UINT32)sizeof(AMBA_RSC_VID_QUE);
    } else if (SrcType == AMBA_RSC_TYPE_AUDIO) {
        MsgSize = (UINT32)sizeof(AMBA_RSC_AUD_QUE);
    } else {
        /* do nothing */
    }

    if (0U < MsgSize) {
        Val = (MsgSize * MaxQueLen);
        Val = GetRoundUpValU32(Val, (UINT32)AMBA_CACHE_LINE_SIZE);
    }

    (*pSize) = (Val * (UINT32)AMBA_CACHE_LINE_SIZE);
}
