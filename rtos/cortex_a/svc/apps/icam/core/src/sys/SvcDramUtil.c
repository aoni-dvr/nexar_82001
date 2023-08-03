/**
*  @file SvcDramUtil.c
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
*  @details dram traffic functions
*
*/

#include "AmbaKAL.h"
#include "AmbaSYS.h"
#if defined(CONFIG_BUILD_COMSVC_DRAMSHMOO)
#include "AmbaDramAdjust.h"
#endif

#include "SvcErrCode.h"
#include "SvcLog.h"
#include "SvcWrap.h"
#include "SvcDramUtil.h"

#if defined(SVC_DRAMUTIL_CMD_USED)

#define SVC_LOG_DRAMUTIL       "DRAMUTIL"

/**
* profile dram traffic
* @param [in] pInput input parameters
* @param [out] pResult result of profile
* @return none
*/
void SvcDramUtil_Profile(SVC_DRAMUTIL_PAR_s *pInput, SVC_DRAMUTIL_RES_s *pResult)
{
    UINT32               i, BurstLen = 64U;
    UINT32               Burst[DRAM_GROUP_NUM], MBurst[DRAM_GROUP_NUM];
    AMBA_DRAMC_STATIS_s  *pDramStatic;

    static UINT32            IsMtxCreated = 0U;
    static AMBA_KAL_MUTEX_t  DramUtilMtx;
    static char              DramUtilMtxName[] = "SvcDramUtilMtx";

    AmbaMisra_TouchUnused(pInput);

    AmbaSvcWrap_MisraMemset(Burst, 0, sizeof(Burst));
    AmbaSvcWrap_MisraMemset(MBurst, 0, sizeof(MBurst));
    AmbaSvcWrap_MisraMemset(pResult, 0, sizeof(SVC_DRAMUTIL_RES_s));
    pDramStatic = &(pResult->DramStatic);

    if (IsMtxCreated == 0U) {
        if (AmbaKAL_MutexCreate(&DramUtilMtx, DramUtilMtxName) == SVC_OK) {
            IsMtxCreated = 1U;
        } else {
            SvcLog_NG(SVC_LOG_DRAMUTIL, "!!!! fail to create mutex !!!!\n", 0U, 0U);
        }
    }

    if (AmbaKAL_MutexTake(&DramUtilMtx, AMBA_KAL_WAIT_FOREVER) == SVC_OK) {
        if (AmbaDRAMC_EnableStatis() != OK) {
            SvcLog_NG(SVC_LOG_DRAMUTIL, "!!!! fail to enable dram statistic !!!!\n", 0U, 0U);
        }

        if (AmbaKAL_TaskSleep(pInput->MSec) != OK) {
            SvcLog_NG(SVC_LOG_DRAMUTIL, "!!!! fail to task sleep !!!!\n", 0U, 0U);
        }

        if (AmbaDRAMC_GetStatisInfo(pDramStatic) != OK) {
            SvcLog_NG(SVC_LOG_DRAMUTIL, "!!!! fail to get dram statistic info !!!!\n", 0U, 0U);
        }

        if (AmbaKAL_MutexGive(&DramUtilMtx) != SVC_OK) {
            SvcLog_NG(SVC_LOG_DRAMUTIL, "!!!! fail to give mutex !!!!\n", 0U, 0U);
        }
    } else {
        SvcLog_NG(SVC_LOG_DRAMUTIL, "!!!! fail to take mutex !!!!\n", 0U, 0U);
    }

    /* calc dram bandwidth */
    pResult->TotalTraffic = 0U;
    if (AmbaSYS_GetClkFreq(AMBA_SYS_CLK_DRAM, &i) == OK) {
        pResult->TotalBandWidth = i;
    } else {
        pResult->TotalBandWidth = 0U;
        SvcLog_NG(SVC_LOG_DRAMUTIL, "!!!! fail to get dram clock !!!!\n", 0U, 0U);
    }
    pResult->TotalBandWidth *= pInput->DramPiece;

    switch (pInput->DramType) {
    case SVC_DRAMUTIL_LP4_16:
        BurstLen = 32U;
        pResult->TotalBandWidth *= 4U;    /* 2 x 2bytes(16b) */
        break;
    case SVC_DRAMUTIL_DDR4_32:
        BurstLen = 32U;
        pResult->TotalBandWidth *= 8U;    /* 2 x 4bytes(32b) */
        break;
    case SVC_DRAMUTIL_LP4_32:
    case SVC_DRAMUTIL_LP5_32:
    default:
        BurstLen = 64U;
        pResult->TotalBandWidth *= 8U;    /* 2 x 4bytes(32b) */
        break;
    }
    pResult->TotalBandWidth = (pResult->TotalBandWidth * pInput->MSec) / 1000U;

    for (i = 0U; i < AMBA_NUM_DRAM_CLIENT; i++) {
        switch (i) {
        #if defined(AMBA_DRAM_CLIENT_AXI)
        case AMBA_DRAM_CLIENT_AXI:
        #endif
        #if defined(AMBA_DRAM_CLIENT_CA53)
        case AMBA_DRAM_CLIENT_CA53:
        #endif
        #if defined(AMBA_DRAM_CLIENT_CA76_0)
        case AMBA_DRAM_CLIENT_CA76_0:
        #endif
        #if defined(AMBA_DRAM_CLIENT_CA76_1)
        case AMBA_DRAM_CLIENT_CA76_1:
        #endif
            Burst[DRAM_GROUP_CPU] += pDramStatic->ClientBurstStatis[i];
            if (0U < pInput->ProfileMode) {
                if (pInput->DramType != SVC_DRAMUTIL_DDR4_32) {
                    MBurst[DRAM_GROUP_CPU] += pDramStatic->ClientMaskWriteStatis[i];
                }
            }
            break;
        #if defined(AMBA_DRAM_CLIENT_ORCME)
        case AMBA_DRAM_CLIENT_ORCME:
        #endif
        #if defined(AMBA_DRAM_CLIENT_ORCME0)
        case AMBA_DRAM_CLIENT_ORCME0:
        #endif
        #if defined(AMBA_DRAM_CLIENT_ORCME1)
        case AMBA_DRAM_CLIENT_ORCME1:
        #endif
        #if defined(AMBA_DRAM_CLIENT_ORCCODE)
        case AMBA_DRAM_CLIENT_ORCCODE:
        #endif
        #if defined(AMBA_DRAM_CLIENT_ORCCODE0)
        case AMBA_DRAM_CLIENT_ORCCODE0:
        #endif
        #if defined(AMBA_DRAM_CLIENT_ORCCODE1)
        case AMBA_DRAM_CLIENT_ORCCODE1:
        #endif
        #if defined(AMBA_DRAM_CLIENT_SMEM)
        case AMBA_DRAM_CLIENT_SMEM:
        #endif
        #if defined(AMBA_DRAM_CLIENT_SMEM_WR)
        case AMBA_DRAM_CLIENT_SMEM_WR:
        #endif
        #if defined(AMBA_DRAM_CLIENT_SMEM_RD)
        case AMBA_DRAM_CLIENT_SMEM_RD:
        #endif
            Burst[DRAM_GROUP_DSP] += pDramStatic->ClientBurstStatis[i];
            if (0U < pInput->ProfileMode) {
                if (pInput->DramType != SVC_DRAMUTIL_DDR4_32) {
                    MBurst[DRAM_GROUP_DSP] += pDramStatic->ClientMaskWriteStatis[i];
                }
            }
            break;
        #if defined(AMBA_DRAM_CLIENT_ORCVP)
        case AMBA_DRAM_CLIENT_ORCVP:
        case AMBA_DRAM_CLIENT_ORCL2:
            Burst[DRAM_GROUP_CVORC] += pDramStatic->ClientBurstStatis[i];

            if (0U < pInput->ProfileMode) {
                if (pInput->DramType != SVC_DRAMUTIL_DDR4_32) {
                    MBurst[DRAM_GROUP_CVORC] += pDramStatic->ClientMaskWriteStatis[i];
                }
            }
            break;
        #if defined(AMBA_DRAM_CLIENT_VMEM)
        case AMBA_DRAM_CLIENT_VMEM:
        #endif
        #if defined(AMBA_DRAM_CLIENT_VMEM0)
        case AMBA_DRAM_CLIENT_VMEM0:
        #endif
            Burst[DRAM_GROUP_VPU] += pDramStatic->ClientBurstStatis[i];

            if (0U < pInput->ProfileMode) {
                if (pInput->DramType != SVC_DRAMUTIL_DDR4_32) {
                    MBurst[DRAM_GROUP_VPU] += pDramStatic->ClientMaskWriteStatis[i];
                }
            }
            break;
        #if defined(AMBA_DRAM_CLIENT_FEX)
        case AMBA_DRAM_CLIENT_FEX:
        #if defined(AMBA_DRAM_CLIENT_BMEM)
        case AMBA_DRAM_CLIENT_BMEM:
        #endif
            Burst[DRAM_GROUP_SPU] += pDramStatic->ClientBurstStatis[i];

            if (0U < pInput->ProfileMode) {
                if (pInput->DramType != SVC_DRAMUTIL_DDR4_32) {
                    MBurst[DRAM_GROUP_SPU] += pDramStatic->ClientMaskWriteStatis[i];
                }
            }
            break;
        #endif
        #endif
        default:
            Burst[DRAM_GROUP_PERI] += pDramStatic->ClientBurstStatis[i];
            if (0U < pInput->ProfileMode) {
                if (pInput->DramType != SVC_DRAMUTIL_DDR4_32) {
                    MBurst[DRAM_GROUP_PERI] += pDramStatic->ClientMaskWriteStatis[i];
                }
            }
            break;
        }
    }

    for (i = 0U; i < DRAM_GROUP_NUM; i++) {
        pResult->Traffic[i] = BurstLen;
        pResult->Traffic[i] *= ((UINT64)Burst[i] + (UINT64)MBurst[i]);
        pResult->TotalTraffic += pResult->Traffic[i];
    }

    /* utilization */
    pResult->TotalUtil = 0.0;
    if (pResult->TotalBandWidth != 0U) {
        pResult->TotalUtil = (DOUBLE)pResult->TotalTraffic;
        pResult->TotalUtil *= (DOUBLE)100U;
        pResult->TotalUtil /= (DOUBLE)pResult->TotalBandWidth;
    }
}

#endif

#if defined(CONFIG_BUILD_COMSVC_DRAMSHMOO) && defined(CONFIG_DRAM_TYPE_LPDDR5)

#define SVC_LOG_DRAMADJ       "DRAMADJ"

static UINT32 g_DramAdjInit = 0U;

void SvcDramUtil_Adjust(UINT32 TaskPri, UINT32 TaskCpuBits, UINT32 AdjDuration)
{
    AMBA_DRAM_ADJUST_CONFIG_s  AdjCfg;

    if (g_DramAdjInit == 0U) {
        g_DramAdjInit = 1U;
        AmbaDramAdjust_Init();
    }

    AdjCfg.CoreMask = TaskCpuBits;
    AdjCfg.Priority = TaskPri;
    AdjCfg.Duration = AdjDuration;
    if (AmbaDramAdjust_Start(&AdjCfg) == SVC_OK) {
        SvcLog_OK(SVC_LOG_DRAMADJ, "dram adjust is enabled", 0U, 0U);
    } else {
        SvcLog_NG(SVC_LOG_DRAMADJ, "fail to enable dram adjust", 0U, 0U);
    }
}

#endif
