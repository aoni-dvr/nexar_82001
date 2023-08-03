/**
 * @file AmbaIPC_Profile.c
 *
 * Copyright (c) 2020 Ambarella International LP
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
 * @details RPC and RPMSG profiling related functions are implemented here.
 *
 */
#include "AmbaTypes.h"
#include "AmbaKAL.h"
#include "AmbaPrint.h"
#include "AmbaLink.h"
#include "AmbaLinkInternal.h"
#include "AmbaLinkPrivate.h"
#include "AmbaIPC_Profile.h"
#include "AmbaRTSL_PLL.h"
#include "libwrapc.h"

#ifdef RPMSG_DEBUG
extern AMBA_RPMSG_PROFILE_s *svq_profile, *rvq_profile;
extern AMBA_RPMSG_STATISTIC_s *rpmsg_stat;
#endif


unsigned int ToGetSvqBuf_Profile(void)
{
    unsigned int to_get_buffer = 0;
#ifdef RPMSG_DEBUG
    unsigned int diff;

    to_get_buffer = AmbaCSL_TmrGetCurrentVal(PROFILE_TIMER);
    /* calculate rpmsg injection rate */
    if( rpmsg_stat->TxLastInjectTime != 0 ){
        diff = calc_timer_diff(rpmsg_stat->TxLastInjectTime, to_get_buffer);
    }
    else{
        diff = 0;
    }
    rpmsg_stat->TxTotalInjectTime += diff;
    rpmsg_stat->TxLastInjectTime = to_get_buffer;
#endif

    return to_get_buffer;
}

void GetSvqBufDone_Profile(unsigned int to_get_buffer, int idx)
{
#ifdef RPMSG_DEBUG
    unsigned int get_buffer = 0;
#else
    UNUSED(to_get_buffer);
#endif

    if(idx < 0) {
        return;
    }

#ifdef RPMSG_DEBUG
    get_buffer = AmbaCSL_TmrGetCurrentVal(PROFILE_TIMER);
    svq_profile[idx].ToGetSvqBuffer = to_get_buffer;
    svq_profile[idx].GetSvqBuffer = get_buffer;
#endif

}

/* before return the used buffer, we store statistics to avoid the same buffer used again
        to overwrite the original statistics. */
void SyncProfileData(struct profile_data *data, int idx)
{
#ifdef RPMSG_DEBUG
    if(data != NULL) {
        data->ToGetSvqBuffer = rvq_profile[idx].ToGetSvqBuffer;
        data->GetSvqBuffer = rvq_profile[idx].GetSvqBuffer;
        data->SvqToSendInterrupt = rvq_profile[idx].SvqToSendInterrupt;
        data->SvqSendInterrupt = rvq_profile[idx].SvqSendInterrupt;
    }
#else
    UNUSED(data);
    UNUSED(idx);
#endif
}

void record_rpmsg_stats(int idx, struct profile_data data){
#ifdef RPMSG_DEBUG
    unsigned int diff;

    UNUSED(idx);
    diff = calc_timer_diff(data.ToGetSvqBuffer, data.SvqToSendInterrupt);
    rpmsg_stat->LxSendRpmsgTime += diff;

    diff = calc_timer_diff(data.SvqToSendInterrupt, data.ToGetRvqBuffer);
    rpmsg_stat->TxResponseTime += diff;
    if(diff > rpmsg_stat->MaxTxResponseTime){
        rpmsg_stat->MaxTxResponseTime = diff;
    }

    diff = calc_timer_diff(data.ToGetRvqBuffer, data.ToRecvData);
    rpmsg_stat->TxRecvRpmsgTime += diff;
    diff = calc_timer_diff(data.ToRecvData, data.RecvData);
    rpmsg_stat->TxRecvCallBackTime += diff;
    if(diff > rpmsg_stat->MaxTxRecvCBTime){
        rpmsg_stat->MaxTxRecvCBTime = diff;
    }
    if(diff < rpmsg_stat->MinTxRecvCBTime){
        rpmsg_stat->MinTxRecvCBTime = diff;
    }
    diff = calc_timer_diff(data.RecvData, data.RvqDone);
    rpmsg_stat->TxReleaseVqTime += diff;
    diff = calc_timer_diff(data.ToGetSvqBuffer, data.ToRecvData);
    rpmsg_stat->LxToTxRpmsgTime += diff;

    if(diff > rpmsg_stat->MaxLxToTxRpmsgTime){
        rpmsg_stat->MaxLxToTxRpmsgTime = diff;
    }
    if(diff < rpmsg_stat->MinLxToTxRpmsgTime){
        rpmsg_stat->MinLxToTxRpmsgTime = diff;
    }
    rpmsg_stat->LxToTxCount++;
#else
    UNUSED(data);
    UNUSED(idx);
#endif
}

#ifdef RPMSG_DEBUG
void InitRpmsgProfile(void){
    UINT32 profile_addr;

    rpmsg_stat = (AMBA_RPMSG_STATISTIC_s *) (RPMSG_PROFILING_ADDR );
    profile_addr =  (RPMSG_PROFILING_ADDR ) + sizeof(AMBA_RPMSG_STATISTIC_s);
    memset(rpmsg_stat, 0, sizeof(AMBA_RPMSG_STATISTIC_s));
    rvq_profile = (AMBA_RPMSG_PROFILE_s *) profile_addr;
    memset(rvq_profile, 0, sizeof(AMBA_RPMSG_PROFILE_s)*(AmbaLinkCtrl.RpmsgNumBuf/2));
    profile_addr = profile_addr + sizeof(AMBA_RPMSG_PROFILE_s)*(AmbaLinkCtrl.RpmsgNumBuf/2);
    svq_profile = (AMBA_RPMSG_PROFILE_s *) profile_addr;
    memset(svq_profile, 0, sizeof(AMBA_RPMSG_PROFILE_s)*(AmbaLinkCtrl.RpmsgNumBuf/2));

    rpmsg_stat->MinTxToLxRpmsgTime = 0xFFFFFFFF;
    rpmsg_stat->MinTxRecvCBTime = 0xFFFFFFFF;
    rpmsg_stat->MinLxToTxRpmsgTime = 0xFFFFFFFF;
    rpmsg_stat->MinLxRecvCBTime = 0xFFFFFFFF;
}

void CalcRpmsgProfile(void)
{
    double duration = 0;
    unsigned int ApbFreq = AmbaRTSL_PllGetApbClk();
    /* ms Factor. */
    double mFactor = (double) (1000 / (double) ApbFreq);

    AmbaPrint_ModulePrintStr5(AMBALINK_MODULE_ID, "------------------ Simplified RPMSG PROFILE ------------------", NULL, NULL, NULL, NULL, NULL);
    AmbaPrint_Flush();
    AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, "Total rpmsg packets (TX -> LK): %d", rpmsg_stat->TxToLxCount, 0U, 0U, 0U, 0U);
    AmbaPrint_Flush();
    duration =  ((double)rpmsg_stat->TxToLxRpmsgTime / (double)rpmsg_stat->TxToLxCount) * mFactor;
    AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, "Avg time (TX to LK)           : %6.6lf ms", duration, 0U, 0U, 0U, 0U);
    AmbaPrint_Flush();

    AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, "Total rpmsg packets (LK -> TX): %d", rpmsg_stat->LxToTxCount, 0U, 0U, 0U, 0U);
    AmbaPrint_Flush();
    duration =  ((double)rpmsg_stat->LxToTxRpmsgTime / (double)rpmsg_stat->LxToTxCount) * mFactor;
    AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, "Avg time (LK -> TX)           : %6.6lf ms\n\n", duration, 0U, 0U, 0U, 0U);
    AmbaPrint_Flush();

    AmbaPrint_ModulePrintStr5(AMBALINK_MODULE_ID, "------------------ Detailed RPMSG PROFILE ------------------", NULL, NULL, NULL, NULL, NULL);
    AmbaPrint_Flush();
    AmbaPrint_ModulePrintStr5(AMBALINK_MODULE_ID, "------------------ From ThreadX to Linux -------------------", NULL, NULL, NULL, NULL, NULL);
    AmbaPrint_Flush();
    duration = (double) rpmsg_stat->TxToLxCount / ((double) rpmsg_stat->TxTotalInjectTime * mFactor);
    AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, "Avg injection rate in ThreadX    : %lf", duration, 0U, 0U, 0U, 0U);
    AmbaPrint_Flush();
    duration =  ((double)rpmsg_stat->TxSendRpmsgTime / (double) rpmsg_stat->TxToLxCount) * mFactor;
    AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, "Avg InjectRpmsgTime              : %6.6lf ms", duration, 0U, 0U, 0U, 0U);
    AmbaPrint_Flush();
    duration =  ((double)rpmsg_stat->LxResponseTime / (double) rpmsg_stat->TxToLxWakeUpCount) * mFactor;

    AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, "Avg LinuxResponseTime            : %6.6lf ms", duration, 0U, 0U, 0U, 0U);
    AmbaPrint_Flush();
    duration =  ((double)rpmsg_stat->LxRecvRpmsgTime / (double)rpmsg_stat->TxToLxCount) * mFactor;
    AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, "Avg PickUpRpmsgTime              : %6.6lf ms", duration, 0U, 0U, 0U, 0U);
    AmbaPrint_Flush();
    duration =  ((double)rpmsg_stat->LxRecvCallBackTime / (double)rpmsg_stat->TxToLxCount) * mFactor;
    AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, "Avg RecvCallBackTime             : %6.6lf ms", duration, 0U, 0U, 0U, 0U);
    AmbaPrint_Flush();
    duration = (double) rpmsg_stat->MaxLxResponseTime * mFactor;
    AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, "Max Linux ResponseTime           : %6.6lf ms", duration, 0U, 0U, 0U, 0U);
    AmbaPrint_Flush();

    duration = (double) rpmsg_stat->MaxTxToLxRpmsgTime * mFactor;
    AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, "Max RpmsgTime                    : %6.6lf ms", duration, 0U, 0U, 0U, 0U);
    AmbaPrint_Flush();
    duration = (double) rpmsg_stat->MinTxToLxRpmsgTime * mFactor;
    AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, "Min RpmsgTime                    : %6.6lf ms", duration, 0U, 0U, 0U, 0U);
    AmbaPrint_Flush();

    duration = (double) rpmsg_stat->MaxLxRecvCBTime * mFactor;
    AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, "Max RecvCBTime                   : %6.6lf ms", duration, 0U, 0U, 0U, 0U);
    AmbaPrint_Flush();
    duration = (double) rpmsg_stat->MinLxRecvCBTime * mFactor;
    AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, "Min RecvCBTime                   : %6.6lf ms", duration, 0U, 0U, 0U, 0U);
    AmbaPrint_Flush();

    AmbaPrint_ModulePrintStr5(AMBALINK_MODULE_ID, "------------------ From Linux to ThreadX ------------------", NULL, NULL, NULL, NULL, NULL);
    AmbaPrint_Flush();
    duration = (double) rpmsg_stat->LxToTxCount / ((double) rpmsg_stat->LxTotalInjectTime * mFactor);
    AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, "Avg injection rate in Linux      : %lf", duration, 0U, 0U, 0U, 0U);
    AmbaPrint_Flush();
    duration =  ((double)rpmsg_stat->LxSendRpmsgTime / (double)rpmsg_stat->LxToTxCount) * mFactor;
    AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, "Avg InjectRpmsgTime              : %6.6lf ms", duration, 0U, 0U, 0U, 0U);
    AmbaPrint_Flush();
    duration =  ((double)rpmsg_stat->TxResponseTime / (double)rpmsg_stat->LxToTxCount) * mFactor;
    AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, "Avg ThreadXResponseTime          : %6.6lf ms", duration, 0U, 0U, 0U, 0U);
    AmbaPrint_Flush();
    duration =  ((double)rpmsg_stat->TxRecvRpmsgTime / (double)rpmsg_stat->LxToTxCount) * mFactor;
    AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, "Avg PickUpRpmsgTime              : %6.6lf ms", duration, 0U, 0U, 0U, 0U);
    AmbaPrint_Flush();
    duration =  ((double)rpmsg_stat->TxRecvCallBackTime / (double)rpmsg_stat->LxToTxCount) * mFactor;
    AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, "Avg RecvCallBackTime             : %6.6lf ms", duration, 0U, 0U, 0U, 0U);
    AmbaPrint_Flush();
    duration = (double) rpmsg_stat->MaxTxResponseTime * mFactor;
    AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, "Max ThreadX ResponseTime         : %6.6lf ms", duration, 0U, 0U, 0U, 0U);
    AmbaPrint_Flush();
    duration = (double) rpmsg_stat->MaxLxToTxRpmsgTime * mFactor;
    AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, "Max RpmsgTime                    : %6.6lf ms", duration, 0U, 0U, 0U, 0U);
    AmbaPrint_Flush();
    duration = (double) rpmsg_stat->MinLxToTxRpmsgTime * mFactor;
    AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, "Min RpmsgTime                    : %6.6lf ms", duration, 0U, 0U, 0U, 0U);
    AmbaPrint_Flush();
    duration = (double) rpmsg_stat->MaxTxRecvCBTime * mFactor;
    AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, "Max RecvCBTime                   : %6.6lf ms", duration, 0U, 0U, 0U, 0U);
    AmbaPrint_Flush();
    duration = (double) rpmsg_stat->MinTxRecvCBTime * mFactor;
    AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, "Min RecvCBTime                   : %6.6lf ms", duration, 0U, 0U, 0U, 0U);
    AmbaPrint_Flush();
    duration = (double) rpmsg_stat->LxRvqIsrCount / (double) rpmsg_stat->TxToLxCount;
    AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, "Rvq ISR is invoked %d times, the aggregation rate is %lf", rpmsg_stat->LxRvqIsrCount, duration, 0U, 0U, 0U);
    AmbaPrint_Flush();
}

#endif
