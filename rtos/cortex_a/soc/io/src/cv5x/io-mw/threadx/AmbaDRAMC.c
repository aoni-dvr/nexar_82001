/**
 * @file AmbaDRAMC.c
 *
 *  Copyright (c) 2020 Ambarella International LP
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
 * @details DRAM controler driver
 */

#include "AmbaTypes.h"
#include "AmbaRTSL_DRAMC.h"
#include "AmbaCSL_DRAMC.h"
#include "AmbaDRAMC.h"
#include "AmbaWrap.h"
#include "AmbaMisraFix.h"

/**
 * dram statistic safety check function
 *
 * @param [out] ModuleID Set ModuleID to CEHU API
 * @param [out] InstanceID Set InstanceID to CEHU API
 * @param [out] ApiID Set ApiID to CEHU API
 * @param [out] ErrorID Set ErrorID to CEHU API
 * @return zero: caller should not trigger CEHU error, non-zero: caller should trigger CEHU error
 */
UINT32 AmbaDRAMC_StatisChecker(UINT32 *ModuleID, UINT32 *InstanceID, UINT32 *ApiID, UINT32 *ErrorID)
{
    static AMBA_DRAMC_STATIS_s LastDramStat;
    static UINT32 inited = 0U;
    AMBA_DRAMC_STATIS_s StatisTmp = {0U};
    UINT32 Ret = 0U, i;

    /* arg check */
    if ((ModuleID ==NULL) || (InstanceID ==NULL) || (ApiID ==NULL) || (ErrorID ==NULL)) {
        Ret = DRAMC_ERR_ARG;
    } else {
        /* init statis before start monitoring */
        if (inited != 1U) {
            AmbaRTSL_DramcEnableStatisCtrl();
            inited = 1U;
        }

        *ModuleID = DRAM_ERR_BASE;
        *InstanceID = 0U;
        *ErrorID = DRAMC_ERR_STATIS;

        AmbaRTSL_DramcGetStatisInfo(&StatisTmp);
        for (i = 0U; i < 32U; i++) {
            const UINT32 ReqThld = DramStatThreshold.ClientRequestStatis[i];
            const UINT32 BurstThld = DramStatThreshold.ClientBurstStatis[i];
            const UINT32 MaskWriteTld = DramStatThreshold.ClientMaskWriteStatis[i];
            //extern void AmbaPrint_PrintUInt5(const char *pFmt, UINT32 Arg1, UINT32 Arg2, UINT32 Arg3, UINT32 Arg4, UINT32 Arg5);

            if (ReqThld != 0U) {
                const UINT32 Req = StatisTmp.ClientRequestStatis[i];
                const UINT32 LastReq = LastDramStat.ClientRequestStatis[i];
                *ApiID = i;
                //AmbaPrint_PrintUInt5("Req[%d] %d -> %d thld %d", i, LastReq, Req, ReqThld, 0U);
                if ((LastReq > 0U) && (Req > LastReq) && (Req > (LastReq + ReqThld))) {
                    Ret = 1U;
                }
            }
            if (BurstThld != 0U) {
                const UINT32 Burst = StatisTmp.ClientBurstStatis[i];
                const UINT32 LastBurst = LastDramStat.ClientBurstStatis[i];
                *ApiID = i;
                //AmbaPrint_PrintUInt5("Burst[%d] %d -> %d thld %d", i, LastBurst, Burst, BurstThld, 0U);
                if ((LastBurst > 0U) && (Burst > LastBurst) && (Burst > (LastBurst + BurstThld))) {
                    Ret = 1U;
                }
            }
            if (MaskWriteTld != 0U) {
                const UINT32 MaskWrite = StatisTmp.ClientMaskWriteStatis[i];
                const UINT32 LastMaskWrite = LastDramStat.ClientMaskWriteStatis[i];
                *ApiID = i;
                //AmbaPrint_PrintUInt5("MaskWrite[%d] %d -> %d thld %d", i, LastMaskWrite, MaskWrite, MaskWriteTld, 0U);
                if ((LastMaskWrite > 0U) && (MaskWrite > LastMaskWrite) && (MaskWrite > (LastMaskWrite + MaskWriteTld))) {
                    Ret = 1U;
                }
            }
        }
        (void)AmbaWrap_memcpy(&LastDramStat, &StatisTmp, sizeof(AMBA_DRAMC_STATIS_s));
    }
    return Ret;
}

/**
 * Get dram host number
 *
 * @param [out] pHostNum
 * @return error code
 */
UINT32 AmbaDRAMC_GetHostNum(UINT32 *pHostNum)
{
    UINT32 RetVal = DRAMC_ERR_NONE;

    if (pHostNum == NULL) {
        RetVal = DRAMC_ERR_ARG;
    } else {
        RetVal = AmbaRTSL_DramcGetHostNum(pHostNum);
    }

    return RetVal;
}
