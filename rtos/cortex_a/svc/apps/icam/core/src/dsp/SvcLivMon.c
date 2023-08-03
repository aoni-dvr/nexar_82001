/**
 *  @file SvcLivMon.c
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
 *  @details svc liveview monitor
 *
 */

#include "AmbaDef.h"
#include "AmbaDSP.h"
#include "AmbaDSP_Event.h"
#include "AmbaShell.h"
#include "AmbaUtility.h"
#include "AmbaSYS.h"

#include "SvcErrCode.h"
#include "SvcLog.h"
#include "SvcWrap.h"
#include "SvcLivMon.h"

#define SVC_LOG_LIVMON      "LIVMON"

typedef struct {
    UINT64  SeqNo;

    UINT64  CurTick;
    UINT64  RealTick;
    UINT64  ExpTick;
    UINT64  MaxDiff;
    UINT64  MinDiff;

    UINT64  ExpFrameTick;
} SVC_LIVMON_RAW_s;

typedef struct {
    UINT64  SeqNo;
    UINT64  DropCount;
} SVC_LIVMON_YUV_s;

static SVC_LIVMON_RAW_s  g_LivMonRaw[AMBA_DSP_MAX_VIN_NUM] GNU_SECTION_NOZEROINIT;
static SVC_LIVMON_YUV_s  g_LivMonYuv[AMBA_DSP_MAX_VIEWZONE_NUM] GNU_SECTION_NOZEROINIT;

static UINT64 DiffTime(const SVC_LIVMON_RAW_s *pLivMon, UINT64 CurrTime)
{
    UINT64  Diff = 0U;

    if (0U < pLivMon->CurTick) {
        if (pLivMon->CurTick <= CurrTime) {
            Diff = CurrTime - pLivMon->CurTick;
        } else {
            /* hw_time is 32bit only */
            Diff = 0xFFFFFFFFU - pLivMon->CurTick;
            Diff += CurrTime;
        }
    }

    return Diff;
}

static UINT32 LivRawDatatRdy(const void *pEventData)
{
    UINT16                         VinID;
    UINT64                         Diff;
    SVC_LIVMON_RAW_s               LivMonRaw;
    const AMBA_DSP_RAW_DATA_RDY_s  *pRawRdy = NULL;

    AmbaMisra_TypeCast(&(pRawRdy), &(pEventData));

    if (pRawRdy != NULL) {
        VinID = pRawRdy->VinId;

        LivMonRaw = g_LivMonRaw[VinID];

        if (0U < LivMonRaw.CurTick) {
            LivMonRaw.ExpTick += LivMonRaw.ExpFrameTick;
        }
        Diff = DiffTime(&LivMonRaw, pRawRdy->CapPts);
        LivMonRaw.CurTick = pRawRdy->CapPts;
        LivMonRaw.RealTick += Diff;

        if (0U < LivMonRaw.MaxDiff) {
            LivMonRaw.MaxDiff = GetMaxValU64(LivMonRaw.MaxDiff, Diff);
        } else {
            LivMonRaw.MaxDiff = Diff;
        }

        if (0U < LivMonRaw.MinDiff) {
            LivMonRaw.MinDiff = GetMinValU64(LivMonRaw.MinDiff, Diff);
        } else {
            LivMonRaw.MinDiff = Diff;
        }

        if (0U < LivMonRaw.SeqNo) {
            if (pRawRdy->CapSequence != (LivMonRaw.SeqNo + 1U)) {
                SvcLog_NG(SVC_LOG_LIVMON, "SeqNo is wrong, %u/%u", (UINT32)LivMonRaw.SeqNo,
                                                                   (UINT32)pRawRdy->CapSequence);
            }
        }
        LivMonRaw.SeqNo = pRawRdy->CapSequence;

        g_LivMonRaw[VinID] = LivMonRaw;
    }

    return SVC_OK;
}

static UINT32 LivYuvDatatRdy(const void *pEventData)
{
    SVC_LIVMON_YUV_s               LivMonYuv;
    const AMBA_DSP_YUV_DATA_RDY_s  *pYuvRdy = NULL;

    AmbaMisra_TypeCast(&(pYuvRdy), &(pEventData));

    if (pYuvRdy != NULL) {
        if ((pYuvRdy->ViewZoneId & 0x80000000U) == 0U) {
            if (pYuvRdy->ViewZoneId < AMBA_DSP_MAX_VIEWZONE_NUM) {
                LivMonYuv = g_LivMonYuv[pYuvRdy->ViewZoneId];

                if (0U < LivMonYuv.SeqNo) {
                    if (pYuvRdy->CapSequence != (LivMonYuv.SeqNo + 1U)) {
                        LivMonYuv.DropCount++;

                        SvcLog_NG(SVC_LOG_LIVMON, "R2Y drop(%u/%u)", (UINT32)LivMonYuv.SeqNo
                                                                , (UINT32)pYuvRdy->CapSequence);
                    }
                }
                LivMonYuv.SeqNo = pYuvRdy->CapSequence;

                g_LivMonYuv[pYuvRdy->ViewZoneId] = LivMonYuv;
            }
        }
    }

    return SVC_OK;
}


/**
* enable liveview monitor
* @param [in] ExpTimeScale expected time scale of frame rate
* @param [in] ExpNumTick expected number tick of frame rate
* @return none
*/
void SvcLivMon_Enable(UINT32 ExpTimeScale, UINT32 ExpNumTick)
{
    UINT32  i = 0U;
    UINT64  ExpFrameTick;

    AmbaSvcWrap_MisraMemset(g_LivMonRaw, 0, sizeof(g_LivMonRaw));
    AmbaSvcWrap_MisraMemset(g_LivMonYuv, 0, sizeof(g_LivMonYuv));

    (void)AmbaSYS_GetClkFreq(AMBA_SYS_CLK_AUD_0, &i);
    ExpFrameTick = i;
    ExpFrameTick *= ExpNumTick;
    ExpFrameTick /= ExpTimeScale;
    for (i = 0U; i < AMBA_DSP_MAX_VIN_NUM; i++) {
        g_LivMonRaw[i].ExpFrameTick = ExpFrameTick;
    }

    if (AmbaDSP_EventHandlerRegister(AMBA_DSP_EVENT_LV_RAW_RDY, LivRawDatatRdy) != SVC_OK) {
        SvcLog_NG(SVC_LOG_LIVMON, "fail to register LV_RAW_RDY event", 0U, 0U);
    }

    if (AmbaDSP_EventHandlerRegister(AMBA_DSP_EVENT_LV_YUV_DATA_RDY, LivYuvDatatRdy) != SVC_OK) {
        SvcLog_NG(SVC_LOG_LIVMON, "fail to register LV_YUV_RDY event", 0U, 0U);
    }
}

/**
* disable liveview monitor
* @return none
*/
void SvcLivMon_Disable(void)
{
    if (AmbaDSP_EventHandlerUnRegister(AMBA_DSP_EVENT_LV_RAW_RDY, LivRawDatatRdy) != SVC_OK) {
        SvcLog_NG(SVC_LOG_LIVMON, "fail to un-register LV_RAW_RDY event", 0U, 0U);
    }

    if (AmbaDSP_EventHandlerUnRegister(AMBA_DSP_EVENT_LV_YUV_DATA_RDY, LivYuvDatatRdy) != SVC_OK) {
        SvcLog_NG(SVC_LOG_LIVMON, "fail to un-register LV_YUV_RDY event", 0U, 0U);
    }
}

/**
* status dump of liveview monitor
* @param [in] PrintFunc print function of shell
* @return none
*/
void SvcLivMon_Dump(AMBA_SHELL_PRINT_f PrintFunc)
{
    char              StrBuf[256];
    UINT32            i = 0U, Rval, BufLen = 256U, IsSlow;
    UINT64            Diff, TimeMs, HwTick;
    SVC_LIVMON_RAW_s  LivMonRaw;
    SVC_LIVMON_YUV_s  LivMonYuv;

    (void)AmbaSYS_GetClkFreq(AMBA_SYS_CLK_AUD_0, &i);
    HwTick = i;

    PrintFunc("[raw]\n");
    for (i = 0U; i < AMBA_DSP_MAX_VIN_NUM; i++) {
        LivMonRaw = g_LivMonRaw[i];
        if (LivMonRaw.RealTick == 0U) {
            continue;
        }

        SVC_WRAP_SNPRINT            " [vin%u][seqno_%llu]\n"
            SVC_SNPRN_ARG_S         StrBuf
            SVC_SNPRN_ARG_UINT32    i               SVC_SNPRN_ARG_POST
            SVC_SNPRN_ARG_UINT64    LivMonRaw.SeqNo
            SVC_SNPRN_ARG_BSIZE     BufLen
            SVC_SNPRN_ARG_RLEN      &Rval
            SVC_SNPRN_ARG_E
        PrintFunc(StrBuf);

        SVC_WRAP_SNPRINT            "  max/min/exp diff(%llu/%llu/%llu)\n"
            SVC_SNPRN_ARG_S         StrBuf
            SVC_SNPRN_ARG_UINT64    LivMonRaw.MaxDiff     
            SVC_SNPRN_ARG_UINT64    LivMonRaw.MinDiff     
            SVC_SNPRN_ARG_UINT64    LivMonRaw.ExpFrameTick
            SVC_SNPRN_ARG_BSIZE     BufLen
            SVC_SNPRN_ARG_RLEN      &Rval
            SVC_SNPRN_ARG_E
        PrintFunc(StrBuf);

        if (LivMonRaw.RealTick <= LivMonRaw.ExpTick) {
            Diff = LivMonRaw.ExpTick - LivMonRaw.RealTick;
            IsSlow = 0U;
        } else {
            Diff = LivMonRaw.RealTick - LivMonRaw.ExpTick;
            IsSlow = 1U;
        }

        TimeMs = GetRoundUpValU64((Diff * 1000U), HwTick);
        SVC_WRAP_SNPRINT        "  %s_%llums, real/exp/diff tick(%llu/%llu/%llu)\n"
            SVC_SNPRN_ARG_S           StrBuf
            SVC_SNPRN_ARG_CSTR        ((0U < IsSlow) ? "slow" : "fast") SVC_SNPRN_ARG_POST
            SVC_SNPRN_ARG_UINT64      TimeMs                           
            SVC_SNPRN_ARG_UINT64    LivMonRaw.RealTick                 
            SVC_SNPRN_ARG_UINT64    LivMonRaw.ExpTick                  
            SVC_SNPRN_ARG_UINT64    Diff                               
            SVC_SNPRN_ARG_BSIZE     BufLen
            SVC_SNPRN_ARG_RLEN      &Rval
            SVC_SNPRN_ARG_E
        PrintFunc(StrBuf);
    }

    PrintFunc("[yuv]\n");
    for (i = 0U; i < AMBA_DSP_MAX_VIEWZONE_NUM; i++) {
        LivMonYuv = g_LivMonYuv[i];
        if (LivMonYuv.SeqNo == 0U) {
            continue;
        }

        SVC_WRAP_SNPRINT            " [vzone%u][seqno_%llu]\n  drop_count(%llu)\n"
            SVC_SNPRN_ARG_S         StrBuf
            SVC_SNPRN_ARG_UINT32    i                   SVC_SNPRN_ARG_POST
            SVC_SNPRN_ARG_UINT64    LivMonYuv.SeqNo    
            SVC_SNPRN_ARG_UINT64    LivMonYuv.DropCount
            SVC_SNPRN_ARG_BSIZE     BufLen
            SVC_SNPRN_ARG_RLEN      &Rval
            SVC_SNPRN_ARG_E
        PrintFunc(StrBuf);
    }
}
