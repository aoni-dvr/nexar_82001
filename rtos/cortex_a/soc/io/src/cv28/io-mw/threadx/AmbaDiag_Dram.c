/**
 *  @file AmbaDiag_Dram.c
 *
 *  @copyright Copyright (c) 2019 Ambarella, Inc.
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
 *  @details Diagnostic functions for Dram Controller.
 *
 */

#include "AmbaTypes.h"

#include "AmbaKAL.h"
#include "AmbaIOUtility.h"

#include "AmbaSYS.h"
#include "AmbaSYS_Ctrl.h"
#include "AmbaRTSL_DRAMC.h"

static UINT32 DramElapTimeStart = 0U;

static void Diag_PrintFormattedInt(AMBA_SYS_LOG_f LogFunc, const char *pFmtString, UINT32 Value, UINT32 Digits)
{
    char StrBuf[64];
    UINT32 ArgsUInt32[2];
    UINT32 StringLength, i;

    ArgsUInt32[0] = Value;

    StringLength = IO_UtilityStringPrintUInt32(StrBuf, sizeof(StrBuf), pFmtString, 1U, ArgsUInt32);
    if (StringLength < Digits) {
        for (i = 0; i < (Digits - StringLength); i++) {
            LogFunc(" ");
        }
    }
    LogFunc(StrBuf);
}

/**
 *  AmbaDiag_DramResetStatis - Reset statistics
 */
void AmbaDiag_DramResetStatis(void)
{
    UINT32 CpuId = 0U;
    (void)AmbaKAL_GetSmpCpuID(&CpuId);
    DramElapTimeStart = AmbaSysProfile_GetTimeStamp(CpuId);
    AmbaRTSL_DramcEnableStatisCtrl();
}

/**
 *  AmbaDiag_DramShowStatisInfo - Show dram load info
 *  @return error code
 */
UINT32 AmbaDiag_DramShowStatisInfo(AMBA_SYS_LOG_f LogFunc)
{
    AMBA_DRAMC_STATIS_s DramStatic = {0};
    static const char * const DramClientName[NUM_DRAM_CLIENT] = {
        [AMBA_DRAM_CLIENT_CA53]          = "AXI_CA53",
        [AMBA_DRAM_CLIENT_DMA0]          = "DMA0    ",
        [AMBA_DRAM_CLIENT_DMA1]          = "DMA1    ",
        [AMBA_DRAM_CLIENT_ENET]          = "ENET    ",
        [AMBA_DRAM_CLIENT_FDMA]          = "FDMA    ",
        [AMBA_DRAM_CLIENT_CANC]          = "CANC    ",
        [AMBA_DRAM_CLIENT_GDMA]          = "GDMA    ",
        [AMBA_DRAM_CLIENT_SD0]           = "SD0     ",
        [AMBA_DRAM_CLIENT_SD1]           = "SD1     ",
        [AMBA_DRAM_CLIENT_SD2]           = "SD2     ",
        [AMBA_DRAM_CLIENT_USB_DEVICE]    = "USB_DEV ",
        [AMBA_DRAM_CLIENT_USB_HOST]      = "USE_HOST",
        [AMBA_DRAM_CLIENT_ORCME]         = "ORCME   ",
        [AMBA_DRAM_CLIENT_ORCCODE]       = "ORCCODE ",
        [AMBA_DRAM_CLIENT_ORCVP]         = "ORCVP   ",
        [AMBA_DRAM_CLIENT_ORCL2]         = "ORCL2   ",
        [AMBA_DRAM_CLIENT_SMEM]          = "SMEM    ",
        [AMBA_DRAM_CLIENT_VMEM0]         = "VMEM0   "
    };
    UINT32 ElapsedTime, CpuId = 0U;
    UINT32 i, RetVal = DRAMC_ERR_NONE;

    AmbaRTSL_DramcGetStatisInfo(&DramStatic);
    (void)AmbaKAL_GetSmpCpuID(&CpuId);
    ElapsedTime = AmbaSysProfile_GetTimeStamp(CpuId);
    ElapsedTime -= DramElapTimeStart;
    ElapsedTime ++;

    LogFunc("----------------------------------------------------------\n");
    LogFunc(" Elapsed time: ");
    Diag_PrintFormattedInt(LogFunc, "%u", ElapsedTime, 10U);
    LogFunc("  microseconds\n");
    LogFunc("----------------------------------------------------------\n");
    LogFunc(" ID  CLIENT_NAME REQUEST         BURST       MASK_BURST\n");
    LogFunc("----------------------------------------------------------\n");

    for(i = 0U; i < NUM_DRAM_CLIENT; i++) {
        Diag_PrintFormattedInt(LogFunc, "%d", i, 3U);
        LogFunc("  ");
        LogFunc(DramClientName[i]);
        Diag_PrintFormattedInt(LogFunc, "%u", DramStatic.ClientRequestStatis[i], 11U);
        Diag_PrintFormattedInt(LogFunc, "%u", DramStatic.ClientBurstStatis[i], 11U);
        Diag_PrintFormattedInt(LogFunc, "%u", DramStatic.ClientMaskWriteStatis[i], 11U);
        LogFunc("\n");

        LogFunc("----------------------------------------------------------\n");
    }

    return RetVal;
}
