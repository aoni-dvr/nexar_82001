/**
 *  @file AmbaIOUTDiag_CmdDDR.c
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
 *  @details DDR diagnostic command.
 *
 */

#include "AmbaTypes.h"
#include "AmbaMisraFix.h"
#include "AmbaDef.h"

#include "AmbaKAL.h"
#include "AmbaShell.h"
#include "AmbaUtility.h"
#include "AmbaWrap.h"

#include "AmbaIOUTDiag.h"

#include "AmbaRTSL_PLL.h"
#include "AmbaCSL_DDRC.h"
#include "AmbaWDT.h"

#define DEFAULT_TIMER_INTERVAL      20000UL // 20 seconds
#define MAX_WDT_FEED_INTERVAL       5000UL  // We will set WDT timeout to (2 * FEED_INTERVAL) + 2, larger interval may cause timeout overflow.
#define DEFAULT_ACCESS_MASK         0xffffffffUL    // All bits

#define READ_DELAY_BITS             4U      // 4 bits
#define READ_DELAY_POS1             1U      // [0] is enable
#define READ_DELAY_POS2             5U      // [4:1] for each byte
#define READ_DELAY_MASK             ((1U << READ_DELAY_BITS) - 1U)
#define DQ_VREF_BITS                7U      // 7 bits
#define DQ_VREF_POS1                7U      // [6:0] for CA each channel
#define DQ_VREF_POS2                14U     // [13:7] for DQ each channel
#define DQ_VREF_MASK                ((1U << DQ_VREF_BITS) - 1U)
#define LP4_MR14_BITS               7U
#define LP4_MR14_MASK               ((1U << LP4_MR14_BITS) - 1U)
#define LP4_MR12_BITS               7U
#define LP4_MR12_MASK               ((1U << LP4_MR12_BITS) - 1U)
#define WRITE_DELAY_BITS            7U      // 7 bits
#define WRITE_DELAY_POS             7U      // [7:0]
#define WRITE_DELAY_MASK            ((1U << WRITE_DELAY_BITS) - 1U)

/* Definitions of DDR controller */
#define AMBA_DDRC0                  0U      /* DDRC0 */
#ifdef CONFIG_SOC_CV2
#define AMBA_DDRC1                  1U      /* DDRC1 */
#define AMBA_NUM_DDRC               2U      /* Number of DDR Controller */
#else
#define AMBA_NUM_DDRC               1U      /* Number of DDR Controller */
#endif

/* Definitions of DDRC DLL byte */
#define AMBA_DDRC_DLL_BYTE_0        0U      /* DLL_Read */
#define AMBA_DDRC_DLL_BYTE_1        1U      /* DLL_Sync */
#define AMBA_DDRC_DLL_BYTE_2        2U      /* DLL_Write */

static const UINT8 DiagDdrPhaseSetting[64] = {  /* for DLL write/sync/read phase shift  */
    0x3F, 0x3E, 0x3D, 0x3C, 0x3B, 0x3A, 0x39, 0x38, 0x37, 0x36, 0x35, 0x34, 0x33, 0x32, 0x31, 0x30,
    0x2F, 0x2E, 0x2D, 0x2C, 0x2B, 0x2A, 0x29, 0x28, 0x27, 0x26, 0x25, 0x24, 0x23, 0x22, 0x21, 0x20,
    0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
    0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F
};

#define TASK_NAME_LEN   16U
#define MAX_ARGV_NUM    16U
#define MAX_ARGV_LEN    16U

typedef struct {
    UINT8               Stack[4096];
    AMBA_KAL_TASK_t     Task;
    char                TaskName[TASK_NAME_LEN];
    char                ArgVector[MAX_ARGV_NUM][MAX_ARGV_LEN];
    UINT32              ArgCount;
    UINT32              TaskCreated;
    UINT32              TaskRunning;
} AMBA_DIAG_DDR_TASK_s;

static AMBA_SHELL_PRINT_f DiagDdrPrintFunc = NULL;
static AMBA_DIAG_DDR_TASK_s DiagDdrTask GNU_ALIGNED_CACHESAFE = {0};
static INT8 DiagDdrEnableWdt = 0;

static void IoDiag_PrintFormattedRegVal(const char *pFmtString, volatile const void *pRegAddr, AMBA_SHELL_PRINT_f PrintFunc)
{
    char StrBuf[64];
    const UINT32 *pUINT32;
    UINT32 ArgUINT32[3];

    AmbaMisra_TypeCast32(&pUINT32, &pRegAddr);
    AmbaMisra_TypeCast32(&ArgUINT32[0], &pUINT32);
    ArgUINT32[1] = *pUINT32;
    (void)AmbaUtility_StringPrintUInt32(StrBuf, sizeof(StrBuf), pFmtString, 2U, ArgUINT32);
    PrintFunc(StrBuf);
}

static void IoDiag_PrintFormattedRegValPerChan(const char *pFmtString, UINT32 DdrcId, UINT32 RegValue, UINT32 ChanValue, AMBA_SHELL_PRINT_f PrintFunc)
{
    char StrBuf[64];
    UINT32 ArgUINT32[5];

    ArgUINT32[0] = DdrcId;
    ArgUINT32[1] = RegValue;
    ArgUINT32[2] = ChanValue;

    (void)AmbaUtility_StringPrintUInt32(StrBuf, sizeof(StrBuf), pFmtString, 3U, ArgUINT32);
    PrintFunc(StrBuf);
}

static void IoDiag_PrintFormattedInt(const char *pFmtString, UINT32 Value, AMBA_SHELL_PRINT_f PrintFunc)
{
    char StrBuf[64];
    UINT32 ArgUINT32[3];

    ArgUINT32[0] = Value;
    (void)AmbaUtility_StringPrintUInt32(StrBuf, sizeof(StrBuf), pFmtString, 1U, ArgUINT32);
    PrintFunc(StrBuf);
}

static UINT32 IoDiag_DdrGetDdrcEnableId(UINT32 *Number, UINT32 *Start, UINT32 *End)
{
    UINT32 HostNumber = 1U, HostStart = 0U;
    UINT32 RetVal = 0;

#ifdef CONFIG_SOC_CV2
    HostNumber = 2;
#endif

    if (Number != NULL) {
        *Number = HostNumber;
    }
    if (Start != NULL) {
        *Start = HostStart;
    }
    if (End != NULL) {
        *End = HostStart + (HostNumber - 1);
    }

    return RetVal;
}

static INT32 IoDiag_DdrCheckDdrcParameter(char const *pArgDdrc, UINT32 *pDdrcId, AMBA_SHELL_PRINT_f PrintFunc)
{
    UINT32 DdrcIdStart = 0U, DdrcIdEnd = 0U;

    if ((pArgDdrc == NULL) || (pDdrcId == NULL)) {
        return 0;
    }

    (void)AmbaUtility_StringToUInt32(pArgDdrc, pDdrcId);

    IoDiag_DdrGetDdrcEnableId(NULL, &DdrcIdStart, &DdrcIdEnd);
    if ((*pDdrcId < DdrcIdStart) || (*pDdrcId > DdrcIdEnd)) {
        PrintFunc("Invalid DdrcId.\n");
        return 0;
    }

    return 1;
}

static INT32 IoDiag_DdrCheckRemainParameter(UINT32 ArgCount, char * const *pArgVector, UINT32 StartIndex, INT32 *pStep, UINT32 *pTimeInterval, UINT32 *pMask)
{
    UINT32 Multiple;

    if (AmbaUtility_StringCompare(pArgVector[StartIndex], "+", 1U) == 0) {
        *pStep = 1;
    } else if (AmbaUtility_StringCompare(pArgVector[StartIndex], "-", 1U) == 0) {
        *pStep = -1;
    } else {
        return 0;
    }

    if (ArgCount > (StartIndex + 1U)) {
        (void)AmbaUtility_StringToUInt32(pArgVector[StartIndex + 1U], pTimeInterval);
    } else {
        *pTimeInterval = DEFAULT_TIMER_INTERVAL;
    }

    if (ArgCount > (StartIndex + 2U)) {
        (void)AmbaUtility_StringToUInt32(pArgVector[StartIndex + 2U], &Multiple);
        *pStep *= Multiple;
    }

    if (ArgCount > (StartIndex + 3U)) {
        (void)AmbaUtility_StringToUInt32(pArgVector[StartIndex + 3U], pMask);
    } else {
        *pMask = DEFAULT_ACCESS_MASK;
    }

    return 1;
}

static void IoDiag_DdrShmooItemStart(UINT32 TimeInterval)
{
    UINT32 WdtFeedTime = (TimeInterval < MAX_WDT_FEED_INTERVAL) ? TimeInterval : MAX_WDT_FEED_INTERVAL;

    if (DiagDdrEnableWdt == 1) {
        (void)AmbaWDT_Start((WdtFeedTime << 1) + 2000U, 10U);  // (Feed interval * 2) + 2s
    }
}

static void IoDiag_DdrShmooItemWait(UINT32 TimeInterval)
{
    UINT32 WdtFeedTime = (TimeInterval < MAX_WDT_FEED_INTERVAL) ? TimeInterval : MAX_WDT_FEED_INTERVAL;
    UINT32 RemainTime = TimeInterval;

    if (DiagDdrEnableWdt == 1) {
        while (RemainTime > WdtFeedTime) {
            (void)AmbaKAL_TaskSleep(WdtFeedTime);
            (void)AmbaWDT_Feed();
            RemainTime -= WdtFeedTime;
        }
        (void)AmbaKAL_TaskSleep(RemainTime);
        (void)AmbaWDT_Feed();
    } else {
        (void)AmbaKAL_TaskSleep(TimeInterval);
    }
}

static void IoDiag_DdrShmooItemStop(AMBA_SHELL_PRINT_f PrintFunc)
{
    if (DiagDdrEnableWdt == 1) {
        (void)AmbaWDT_Stop();
    }
    PrintFunc("Done\n");
}

static INT32 IoDiag_DdrGetDllValIndex(UINT32 Param)
{
    INT32 i, RetVal = -1;

    for (i = sizeof(DiagDdrPhaseSetting); i > 0; i--) {
        if (Param == (UINT32)DiagDdrPhaseSetting[i - 1]) {
            RetVal = (i - 1);
            break;
        }
    }

    return RetVal;
}

static UINT32 IoDiag_DdrcGetDllSetting(UINT32 DdrcId, UINT32 BitMask)
{
    if (BitMask & 0xffU) {
        return AmbaCSL_DdrcGetDll0Setting(DdrcId);
    } else if (BitMask & 0xff00U) {
        return AmbaCSL_DdrcGetDll1Setting(DdrcId);
    } else if (BitMask & 0xff0000U) {
        return AmbaCSL_DdrcGetDll2Setting(DdrcId);
    } else if (BitMask & 0xff000000U) {
        return AmbaCSL_DdrcGetDll3Setting(DdrcId);
    }
    return 0;
}

static void IoDiag_DdrcSetDllSetting(UINT32 DdrcId, UINT32 BitMask, UINT32 Val, AMBA_SHELL_PRINT_f PrintFunc)
{
    if (BitMask & 0xffU) {
        AmbaCSL_DdrcSetDll0Setting(DdrcId, Val);
        IoDiag_PrintFormattedRegValPerChan("[DDRC%d][DLL%d]Reg = 0x%06x\n", DdrcId, 0, AmbaCSL_DdrcGetDll0Setting(DdrcId), PrintFunc);
    }
    if (BitMask & 0xff00U) {
        AmbaCSL_DdrcSetDll1Setting(DdrcId, Val);
        IoDiag_PrintFormattedRegValPerChan("[DDRC%d][DLL%d]Reg = 0x%06x\n", DdrcId, 1, AmbaCSL_DdrcGetDll1Setting(DdrcId), PrintFunc);
    }
    if (BitMask & 0xff0000U) {
        AmbaCSL_DdrcSetDll2Setting(DdrcId, Val);
        IoDiag_PrintFormattedRegValPerChan("[DDRC%d][DLL%d]Reg = 0x%06x\n", DdrcId, 2, AmbaCSL_DdrcGetDll2Setting(DdrcId), PrintFunc);
    }
    if (BitMask & 0xff000000U) {
        AmbaCSL_DdrcSetDll3Setting(DdrcId, Val);
        IoDiag_PrintFormattedRegValPerChan("[DDRC%d][DLL%d]Reg = 0x%06x\n", DdrcId, 3, AmbaCSL_DdrcGetDll3Setting(DdrcId), PrintFunc);
    }
}

static void IoDiag_DdrcSetModeRegWait(UINT32 DdrcId, UINT32 BitMask, UINT32 Val)
{
    UINT32  Value = 0;

#if 0   // In CV2x, we are unable to write MR register for different channel (it's write to both channel always) except during training.
    Val &= 0xf3ffffffU; // Clear bit [26~27] to be default both channel
    if (BitMask & 0xffffU) {
        if ((BitMask & 0xffff0000U) == 0) {
            Val |= 0x2U << 26U;     // Single channel 0
        }
    } else if (BitMask & 0xffff0000U) {
        Val |= 0x3U << 26U;         // Single channel 1
    }
#else
    AmbaMisra_TouchUnused(&BitMask);
#endif

    AmbaCSL_DdrcSetModeReg(DdrcId, Val);
    /* Wait busy bit gone */
    while ((0x0U != (AmbaCSL_DdrcGetModeReg(DdrcId) & 0x80000000U)) && (Value++ < 100)) {
        (void)AmbaKAL_TaskSleep(10);
    }
}

static void IoDiag_DdrCmdUsage(char * const *pArgVector, AMBA_SHELL_PRINT_f PrintFunc)
{
    PrintFunc("Usage: ");
    PrintFunc(pArgVector[0]);
    PrintFunc(" [options] ...\n");
    PrintFunc(" dump\n");
    PrintFunc(" shmoo [0:DDRC0|1:DDRC1] [sync|read|write] [+|-] [time(ms), default=20000] [step, default=1] [bitmask, default=0xffffffff]\n");
    PrintFunc("          How to run DDR quick shmoo :\n");
    PrintFunc("           - 1. Type \"sync + 1000\" : Increase sync value and wait 1000ms, shmoo the sync byte one by one until system hang, ex: 0x20->0x00->0x01->0x02\n");
    PrintFunc("           - 2. Type \"sync - 1000\" : Decrease sync value and wait 1000ms, shmoo the sync byte one by one until system hang, ex: 0x20->0x21->0x22->0x23\n");
    PrintFunc("           - 3. Repeat the above procedures on Read and Write phase.\n");
    PrintFunc("           - 4. Collect the workable ranges of sync, read and write byte.\n");
    PrintFunc(" set [0:DDRC0|1:DDRC1] [sync|read|write|lp4rdly|lp4vref|lp4wdly|lp4mr14|lp4mr12] <value> [bitmask] \n");
    PrintFunc("          set 0 lp4mr14 0x010E0010 0xffffffff\n");
    PrintFunc("          set 0 lp4mr12 0x010C0008 0xffffffff\n");
    PrintFunc("\n The following commands are for LPDDR4 only!\n");
    PrintFunc(" [lp4rdly|lp4vref|lp4wdly|lp4mr14|lp4mr12] [0:DDRC0|1:DDRC1] [+|-] [time(ms)] [step] [bitmask]\n");
    PrintFunc("          diag ddr lp4rdly 0 + 30000 1 0xffffffff\n");
    PrintFunc("          Read : lp4rdly and lp4vref\n");
    PrintFunc("          Write: lp4wdly and lp4mr14\n");
}

static void IoDiag_DdrDump(AMBA_SHELL_PRINT_f PrintFunc)
{
    const char DiagDdrDramType[8][8] = {
        [0] = "DDR2",
        [1] = "DDR3",
        [2] = "LPDDR",
        [3] = "LPDDR2",
        [4] = "LPDDR3",
        [5] = "DDR4",
        [6] = "LPDDR4",
        [7] = "N/A"
    };
    const char DiagDdrDramSize[8][8] = {
        [0] = "256Mb",
        [1] = "512Mb",
        [2] = "1Gb",
        [3] = "2Gb",
        [4] = "4Gb",
        [5] = "8Gb",
        [6] = "16Gb",
        [7] = "N/A"
    };
    const char DiagDdrPageSize[4][8] = {
        [0] = "1KB",
        [1] = "2KB",
        [2] = "4KB",
        [3] = "N/A"
    };
    const char DiagDdrDqBusWidth[2][8] = {
        [0] = "32-bit",
        [1] = "16-bit"
    };

    UINT32 Value;
    UINT32 DdrcId;

    PrintFunc("--------------------------------------------------------------------------------\n");

    /* dram freq */
    IoDiag_PrintFormattedInt("DRAM Frequency      = %u\n", AmbaRTSL_PllGetDramClk(), PrintFunc);

    for (DdrcId = AMBA_DDRC0; DdrcId < AMBA_NUM_DDRC; DdrcId ++) {
        PrintFunc("--------------------------------------------------------------------------------\n");

        /* ddrc id */
        if (DdrcId == AMBA_DDRC0) {
            PrintFunc("DDRC-0:\n");
        } else {
            PrintFunc("DDRC-1:\n");
        }

        /* dram type */
        Value = AmbaCSL_DdrcGetDramType(DdrcId);
        PrintFunc("DRAM Type = ");
        PrintFunc(DiagDdrDramType[Value]);
        PrintFunc("\n");
        /* dram size */
        Value = AmbaCSL_DdrcGetDramSize(DdrcId);
        PrintFunc("DRAM Size = ");
        PrintFunc(DiagDdrDramSize[Value]);
        PrintFunc("\n");
        /* DQ bus width */
        Value = AmbaCSL_DdrcGetDqBusWidth(DdrcId);
        PrintFunc("DQ width = ");
        PrintFunc(DiagDdrDqBusWidth[Value]);
        PrintFunc("\n");
        /* page size */
        Value = AmbaCSL_DdrcGetPageSize(DdrcId);
        PrintFunc("Page size = ");
        PrintFunc(DiagDdrPageSize[Value]);
        PrintFunc("\n");

        PrintFunc("--------------------------------------------------------------------------------\n");

        IoDiag_PrintFormattedRegVal("DRAM_Ctrl                    (0x%x)  = 0x%.8x\n", &pAmbaDDRC_Reg[DdrcId]->Ctrl,                 PrintFunc);
        IoDiag_PrintFormattedRegVal("DRAM_Config                  (0x%x)  = 0x%.8x\n", &pAmbaDDRC_Reg[DdrcId]->Config,               PrintFunc);
        IoDiag_PrintFormattedRegVal("DRAM_Timing1                 (0x%x)  = 0x%.8x\n", &pAmbaDDRC_Reg[DdrcId]->Timing1,              PrintFunc);
        IoDiag_PrintFormattedRegVal("DRAM_Timing2                 (0x%x)  = 0x%.8x\n", &pAmbaDDRC_Reg[DdrcId]->Timing2,              PrintFunc);
        IoDiag_PrintFormattedRegVal("DRAM_Timing3                 (0x%x)  = 0x%.8x\n", &pAmbaDDRC_Reg[DdrcId]->Timing3,              PrintFunc);
        IoDiag_PrintFormattedRegVal("DRAM_Timing4                 (0x%x)  = 0x%.8x\n", &pAmbaDDRC_Reg[DdrcId]->Timing4,              PrintFunc);
        IoDiag_PrintFormattedRegVal("DRAM_InitCtrl                (0x%x)  = 0x%.8x\n", &pAmbaDDRC_Reg[DdrcId]->InitCtrl,             PrintFunc);
#if !defined(CONFIG_DRAM_TYPE_DDR4)
        IoDiag_PrintFormattedRegVal("DRAM_Mode14Reg               (0x%x)  = 0x%.8x\n", &pAmbaDDRC_Reg[DdrcId]->ModeReg,              PrintFunc);
        IoDiag_DdrcSetModeRegWait(DdrcId, 0xffffffffU, 0x000c0000);
        IoDiag_PrintFormattedRegVal("DRAM_Mode12Reg               (0x%x)  = 0x%.8x\n", &pAmbaDDRC_Reg[DdrcId]->ModeReg,              PrintFunc);
        IoDiag_DdrcSetModeRegWait(DdrcId, 0xffffffffU, 0x000e0000);
        IoDiag_PrintFormattedRegVal("DRAM_Mode14Reg               (0x%x)  = 0x%.8x\n", &pAmbaDDRC_Reg[DdrcId]->ModeReg,              PrintFunc);
#endif
        IoDiag_PrintFormattedRegVal("DRAM_DqsSync                 (0x%x)  = 0x%.8x\n", &pAmbaDDRC_Reg[DdrcId]->DqsSync,              PrintFunc);
        IoDiag_PrintFormattedRegVal("DRAM_PadTerm                 (0x%x)  = 0x%.8x\n", &pAmbaDDRC_Reg[DdrcId]->PadTerm,              PrintFunc);
        IoDiag_PrintFormattedRegVal("DRAM_PadTerm2                (0x%x)  = 0x%.8x\n", &pAmbaDDRC_Reg[DdrcId]->PadTerm2,             PrintFunc);
        IoDiag_PrintFormattedRegVal("DRAM_PadTerm3                (0x%x)  = 0x%.8x\n", &pAmbaDDRC_Reg[DdrcId]->PadTerm3,             PrintFunc);
        IoDiag_PrintFormattedRegVal("DRAM_ZqCalib                 (0x%x)  = 0x%.8x\n", &pAmbaDDRC_Reg[DdrcId]->ZqCalib,              PrintFunc);
        IoDiag_PrintFormattedRegVal("DRAM_RsvdSpace               (0x%x)  = 0x%.8x\n", &pAmbaDDRC_Reg[DdrcId]->RsvdSpace,            PrintFunc);
        IoDiag_PrintFormattedRegVal("DRAM_ByteMap                 (0x%x)  = 0x%.8x\n", &pAmbaDDRC_Reg[DdrcId]->ByteMap,              PrintFunc);
#if defined(CONFIG_SOC_CV25) || defined(CONFIG_SOC_CV28) || defined(CONFIG_SOC_H32)
        IoDiag_PrintFormattedRegVal("DRAM_lpddr4DqWriteDlyParam   (0x%x)  = 0x%.8x\n", &pAmbaDDRC_Reg[DdrcId]->TrnCs0DqWriteDly,      PrintFunc);
        IoDiag_PrintFormattedRegVal("DRAM_lpddr4DqReadDlyParam    (0x%x)  = 0x%.8x\n", &pAmbaDDRC_Reg[DdrcId]->TrnCs0DqReadDly, PrintFunc);
        IoDiag_PrintFormattedRegVal("DRAM_lpddr4DqsWriteDlyParam  (0x%x)  = 0x%.8x\n", &pAmbaDDRC_Reg[DdrcId]->TrnCs0DqsWriteDly,     PrintFunc);
        IoDiag_PrintFormattedRegVal("DRAM_lpddr4CkCaWriteDlyParam (0x%x)  = 0x%.8x\n", &pAmbaDDRC_Reg[DdrcId]->TrnCkCaWriteDly, PrintFunc);
#else
        IoDiag_PrintFormattedRegVal("DRAM_lpddr4DqWriteDlyParam   (0x%x)  = 0x%.8x\n", &pAmbaDDRC_Reg[DdrcId]->TrainDqWriteDly,      PrintFunc);
        IoDiag_PrintFormattedRegVal("DRAM_lpddr4DqReadDlyParam    (0x%x)  = 0x%.8x\n", &pAmbaDDRC_Reg[DdrcId]->TrainDqReadDly, PrintFunc);
        IoDiag_PrintFormattedRegVal("DRAM_lpddr4DqsWriteDlyParam  (0x%x)  = 0x%.8x\n", &pAmbaDDRC_Reg[DdrcId]->TrainDqsWriteDly,     PrintFunc);
        IoDiag_PrintFormattedRegVal("DRAM_lpddr4CkCaWriteDlyParam (0x%x)  = 0x%.8x\n", &pAmbaDDRC_Reg[DdrcId]->TrainCkCaWriteDly,    PrintFunc);
#endif
#if defined(CONFIG_SOC_CV25) || defined(CONFIG_SOC_CV28) || defined(CONFIG_SOC_H32)
        IoDiag_PrintFormattedRegVal("DRAM_lpddr4DqCaVrefParam     (0x%x)  = 0x%.8x\n", &pAmbaDDRC_Reg[DdrcId]->TrnDqCaVref, PrintFunc);
#else
        IoDiag_PrintFormattedRegVal("DRAM_lpddr4DqCaVrefParam     (0x%x)  = 0x%.8x\n", &pAmbaDDRC_Reg[DdrcId]->TrainDqCaVref, PrintFunc);
#endif
        IoDiag_PrintFormattedRegVal("DLL_Setting0                 (0x%x)  = 0x%.8x\n", &pAmbaDDRC_Reg[DdrcId]->DdrioDll0,            PrintFunc);
        IoDiag_PrintFormattedRegVal("DLL_Setting1                 (0x%x)  = 0x%.8x\n", &pAmbaDDRC_Reg[DdrcId]->DdrioDll1,            PrintFunc);
        IoDiag_PrintFormattedRegVal("DLL_Setting2                 (0x%x)  = 0x%.8x\n", &pAmbaDDRC_Reg[DdrcId]->DdrioDll2,            PrintFunc);
        IoDiag_PrintFormattedRegVal("DLL_Setting3                 (0x%x)  = 0x%.8x\n", &pAmbaDDRC_Reg[DdrcId]->DdrioDll3,            PrintFunc);
        IoDiag_PrintFormattedRegVal("DLL_CtrlSel0                 (0x%x)  = 0x%.8x\n", &pAmbaDDRC_Reg[DdrcId]->DdrioDllCtrlSel0,     PrintFunc);
        IoDiag_PrintFormattedRegVal("DLL_CtrlSel1                 (0x%x)  = 0x%.8x\n", &pAmbaDDRC_Reg[DdrcId]->DdrioDllCtrlSel1,     PrintFunc);
        IoDiag_PrintFormattedRegVal("DLL_CtrlSel2                 (0x%x)  = 0x%.8x\n", &pAmbaDDRC_Reg[DdrcId]->DdrioDllCtrlSel2,     PrintFunc);
        IoDiag_PrintFormattedRegVal("DLL_CtrlSel3                 (0x%x)  = 0x%.8x\n", &pAmbaDDRC_Reg[DdrcId]->DdrioDllCtrlSel3,     PrintFunc);
        IoDiag_PrintFormattedRegVal("DLL_CtrlSelMisc              (0x%x)  = 0x%.8x\n", &pAmbaDDRC_Reg[DdrcId]->DdrioDllCtrlMisc,     PrintFunc);
#if defined(CONFIG_SOC_CV25) || defined(CONFIG_SOC_CV28) || defined(CONFIG_SOC_H32)
        IoDiag_PrintFormattedRegVal("DLL_SyncCtrlSel0             (0x%x)  = 0x%.8x\n", &pAmbaDDRC_Reg[DdrcId]->DdrioCs0DllSyncCtlSel0, PrintFunc);
        IoDiag_PrintFormattedRegVal("DLL_SyncCtrlSel1             (0x%x)  = 0x%.8x\n", &pAmbaDDRC_Reg[DdrcId]->DdrioCs0DllSyncCtlSel1, PrintFunc);
        IoDiag_PrintFormattedRegVal("DLL_SyncCtrlSel2             (0x%x)  = 0x%.8x\n", &pAmbaDDRC_Reg[DdrcId]->DdrioCs0DllSyncCtlSel2, PrintFunc);
        IoDiag_PrintFormattedRegVal("DLL_SyncCtrlSel3             (0x%x)  = 0x%.8x\n", &pAmbaDDRC_Reg[DdrcId]->DdrioCs0DllSyncCtlSel3, PrintFunc);
#else
        IoDiag_PrintFormattedRegVal("DLL_SyncCtrlSel0             (0x%x)  = 0x%.8x\n", &pAmbaDDRC_Reg[DdrcId]->DdrioDllSyncCtrlSel0, PrintFunc);
        IoDiag_PrintFormattedRegVal("DLL_SyncCtrlSel1             (0x%x)  = 0x%.8x\n", &pAmbaDDRC_Reg[DdrcId]->DdrioDllSyncCtrlSel1, PrintFunc);
        IoDiag_PrintFormattedRegVal("DLL_SyncCtrlSel2             (0x%x)  = 0x%.8x\n", &pAmbaDDRC_Reg[DdrcId]->DdrioDllSyncCtrlSel2, PrintFunc);
        IoDiag_PrintFormattedRegVal("DLL_SyncCtrlSel3             (0x%x)  = 0x%.8x\n", &pAmbaDDRC_Reg[DdrcId]->DdrioDllSyncCtrlSel3, PrintFunc);
#endif
    }
}

static void IoDiag_DdrSetDLL(UINT32 DdrcId, UINT32 BitMask, UINT32 DllByte, UINT32 DllValue, AMBA_SHELL_PRINT_f PrintFunc)
{
    UINT32 DllRegVal = IoDiag_DdrcGetDllSetting(DdrcId, BitMask);

    switch (DllByte) {
    case AMBA_DDRC_DLL_BYTE_0:
        DllRegVal &= 0xffff00UL;
        DllRegVal |= (DllValue & 0xffUL);
        break;

    case AMBA_DDRC_DLL_BYTE_1:
        DllRegVal &= 0xff00ffUL;
        DllRegVal |= (DllValue & 0xffUL) << 8UL;
        break;

    case AMBA_DDRC_DLL_BYTE_2:
        DllRegVal &= 0x00ffffUL;
        DllRegVal |= (DllValue & 0xffUL) << 16UL;
        break;

    default:
        DllRegVal &= 0xffffffUL;
        break;
    }

    IoDiag_DdrcSetDllSetting(DdrcId, BitMask, DllRegVal, PrintFunc);
}

static UINT32 IoDiag_DdrGetLp4ReadDly(UINT32 DdrcId, UINT32 BitMask)
{
    UINT32 CurRegValue = 0, CurDelay = 0, i;

    for (i = 0U; i < 4U; i++) {
        if ((0xffU << (i*8)) & BitMask) {
            CurRegValue = AmbaCSL_DdrcGetDqReadDly(DdrcId);
            CurDelay = (CurRegValue >> ((READ_DELAY_POS2 * i) + READ_DELAY_POS1)) & READ_DELAY_MASK;
            return CurDelay;
        }
    }

    return 0;
}

static UINT32 IoDiag_DdrGetLp4Vref(UINT32 DdrcId, UINT32 BitMask)
{
    UINT32 CurRegValue = 0, CurValue = 0, i;

    for (i = 0U; i < 2U; i++) {
        if ((0xffffU << (i*16)) & BitMask) {
            CurRegValue = AmbaCSL_DdrcGetDqVref(DdrcId);
            CurValue = (CurRegValue >> ((DQ_VREF_POS2 * i) + DQ_VREF_POS1)) & DQ_VREF_MASK;
            return CurValue;
        }
    }

    return 0;
}

static UINT32 IoDiag_DdrGetLp4MR14(UINT32 DdrcId, UINT32 BitMask)
{
    UINT32 CurRegValue = 0;

    IoDiag_DdrcSetModeRegWait(DdrcId, BitMask, 0x000e0000);
    CurRegValue = AmbaCSL_DdrcGetModeReg(DdrcId) & LP4_MR14_MASK; /* bit[6:0] */
    if (CurRegValue > 64U) {
        CurRegValue = CurRegValue - 64U + 30U;  /* bit[6] is set, we should remove bit[6] and remap bit[5:0] to 31~80 */
    }

    return CurRegValue;
}

static UINT32 IoDiag_DdrGetLp4MR12(UINT32 DdrcId, UINT32 BitMask)
{
    UINT32 CurRegValue = 0;

    IoDiag_DdrcSetModeRegWait(DdrcId, BitMask, 0x000c0000);
    CurRegValue = AmbaCSL_DdrcGetModeReg(DdrcId) & LP4_MR12_MASK; /* bit[6:0] */
    if (CurRegValue > 64U) {
        CurRegValue = CurRegValue - 64U + 30U;  /* bit[6] is set, we should remove bit[6] and remap bit[5:0] to 31~80 */
    }

    return CurRegValue;
}

static UINT32 IoDiag_DdrGetLp4WriteDly(UINT32 DdrcId, UINT32 BitMask)
{
    UINT32 CurRegValue = 0, CurDelay = 0, i;

    for (i = 0U; i < 4U; i++) {
        if ((0xffU << (i*8)) & BitMask) {
            CurRegValue = AmbaCSL_DdrcGetDqWriteDly(DdrcId);
            CurDelay = (CurRegValue >> (WRITE_DELAY_POS * i)) & WRITE_DELAY_MASK;
            return CurDelay;
        }
    }

    return 0;
}

static void IoDiag_DdrSetLp4ReadDly(UINT32 DdrcId, UINT32 BitMask, UINT32 DelayValue, AMBA_SHELL_PRINT_f PrintFunc)
{
    UINT32 NewRegValue = 0, CurDelay = 0, i, Mask;

    for (i = 0U; i < 4U; i++) {
        Mask = 0xffU << (i * 8);
        if (Mask & BitMask) {
            NewRegValue = AmbaCSL_DdrcGetDqReadDly(DdrcId);
            NewRegValue &= ~(READ_DELAY_MASK << ((READ_DELAY_POS2 * i) + READ_DELAY_POS1));
            NewRegValue |= ((DelayValue & READ_DELAY_MASK) << ((READ_DELAY_POS2 * i) + READ_DELAY_POS1));
            AmbaCSL_DdrcSetDqReadDly(DdrcId, NewRegValue);

            CurDelay = IoDiag_DdrGetLp4ReadDly(DdrcId, Mask);
            IoDiag_PrintFormattedRegValPerChan("[DDRC%d][Read Delay%d]TrainDqReadDly = 0x%08x", DdrcId, i, NewRegValue, PrintFunc);
            IoDiag_PrintFormattedRegValPerChan(" (value = 0x%x)\n", CurDelay, 0, 0, PrintFunc);
        }
    }
}

static void IoDiag_DdrSetLp4Vref(UINT32 DdrcId, UINT32 BitMask, UINT32 Val, AMBA_SHELL_PRINT_f PrintFunc)
{
    UINT32 NewRegValue = 0, CurValue = 0, i;

    for (i = 0U; i < 2U; i++) {
        if ((0xffffU << (i*16)) & BitMask) {
            NewRegValue = AmbaCSL_DdrcGetDqVref(DdrcId);
            NewRegValue &= ~(DQ_VREF_MASK << ((DQ_VREF_POS2 * i) + DQ_VREF_POS1));        // DQ
            NewRegValue |= ((Val & DQ_VREF_MASK) << ((DQ_VREF_POS2 * i) + DQ_VREF_POS1));
            NewRegValue &= ~(DQ_VREF_MASK << ((DQ_VREF_POS2 * i) ));                         // CA
            NewRegValue |= ((Val & DQ_VREF_MASK) << ((DQ_VREF_POS2 * i) ));
            AmbaCSL_DdrcSetDqVref(DdrcId, NewRegValue);

            CurValue = IoDiag_DdrGetLp4Vref(DdrcId, BitMask);
            IoDiag_PrintFormattedRegValPerChan("[DDRC%d][DQ Vref%d]TrainDqVref = 0x%08x", DdrcId, i, NewRegValue, PrintFunc);
            IoDiag_PrintFormattedRegValPerChan(" (value = 0x%x)\n", CurValue, 0, 0, PrintFunc);
        }
    }
}

static void IoDiag_DdrSetLp4MR14(UINT32 DdrcId, UINT32 BitMask, UINT32 Val, AMBA_SHELL_PRINT_f PrintFunc)
{
    UINT32 NewRegValue = 0;

    Val &= LP4_MR14_MASK;
    if (Val <= 50) {
        NewRegValue = 0x010e0000UL | Val;
    } else {
        NewRegValue = 0x010e0040UL | (Val - 30); /* Remap 51~80 to bit[5:0] and set bit[6] */
    }
    IoDiag_DdrcSetModeRegWait(DdrcId, BitMask, NewRegValue);

    NewRegValue = IoDiag_DdrGetLp4MR14(DdrcId, BitMask);
    IoDiag_PrintFormattedRegValPerChan("[LPDDR4_%d][MR14 Vref]Lpddr4DqVref = 0x%08x (value = 0x%x)\n", DdrcId, AmbaCSL_DdrcGetModeReg(DdrcId), NewRegValue, PrintFunc);
}

static void IoDiag_DdrSetLp4MR12(UINT32 DdrcId, UINT32 BitMask, UINT32 Val, AMBA_SHELL_PRINT_f PrintFunc)
{
    UINT32 NewRegValue = 0;

    Val &= LP4_MR12_MASK;
    if (Val <= 50) {
        NewRegValue = 0x010c0000UL | Val;
    } else {
        NewRegValue = 0x010c0040UL | (Val - 30); /* Remap 51~80 to bit[5:0] and set bit[6] */
    }
    IoDiag_DdrcSetModeRegWait(DdrcId, BitMask, NewRegValue);

    NewRegValue = IoDiag_DdrGetLp4MR12(DdrcId, BitMask);
    IoDiag_PrintFormattedRegValPerChan("[LPDDR4_%d][MR12 Vref]Lpddr4CaVref = 0x%08x (value = 0x%x)\n", DdrcId, AmbaCSL_DdrcGetModeReg(DdrcId), NewRegValue, PrintFunc);
}

static void IoDiag_DdrSetLp4WriteDly(UINT32 DdrcId, UINT32 BitMask, UINT32 DelayValue, AMBA_SHELL_PRINT_f PrintFunc)
{
    UINT32 NewRegValue = 0, CurDelay = 0, i, Mask;

    for (i = 0U; i < 4U; i++) {
        Mask = 0xffU << (i * 8);
        if (Mask & BitMask) {
            NewRegValue = AmbaCSL_DdrcGetDqWriteDly(DdrcId);
            NewRegValue &= ~(WRITE_DELAY_MASK << (WRITE_DELAY_POS * i));
            NewRegValue |= ((DelayValue & WRITE_DELAY_MASK) << (WRITE_DELAY_POS * i));
            NewRegValue |= (0xDU << 28U);
            AmbaCSL_DdrcSetDqWriteDly(DdrcId, NewRegValue);
            NewRegValue &= ~(0x1U << 28U);
            AmbaCSL_DdrcSetDqWriteDly(DdrcId, NewRegValue);

            CurDelay = IoDiag_DdrGetLp4WriteDly(DdrcId, Mask);
            IoDiag_PrintFormattedRegValPerChan("[DDRC%d][Write Delay%d]TrainWriteDly = 0x%08x", DdrcId, i, NewRegValue, PrintFunc);
            IoDiag_PrintFormattedRegValPerChan(" (value = 0x%x)\n", CurDelay, 0, 0, PrintFunc);
        }
    }
}

static void IoDiag_DdrSetValue(UINT32 ArgCount, char * const *pArgVector, AMBA_SHELL_PRINT_f PrintFunc)
{
    UINT32 DllByte = 0xffffffffUL;
    UINT32 DdrcId = AMBA_DDRC0, BitMask = DEFAULT_ACCESS_MASK, NewValue = 0;

    if (ArgCount < 5U) {
        IoDiag_DdrCmdUsage(pArgVector, PrintFunc);
        return;
    }

    if (IoDiag_DdrCheckDdrcParameter(pArgVector[2], &DdrcId, PrintFunc) == 0) {
        return; // Invalid parameter
    }
    (void)AmbaUtility_StringToUInt32(pArgVector[4], &NewValue);
    if (ArgCount > 5U) {
        (void)AmbaUtility_StringToUInt32(pArgVector[5], &BitMask);
    }

    if (AmbaUtility_StringCompare(pArgVector[3], "lp4rdly", 7U) == 0) {
        IoDiag_DdrSetLp4ReadDly(DdrcId, BitMask, NewValue, PrintFunc);
    } else if (AmbaUtility_StringCompare(pArgVector[3], "lp4vref", 7U) == 0) {
        IoDiag_DdrSetLp4Vref(DdrcId, BitMask, NewValue, PrintFunc);
    } else if (AmbaUtility_StringCompare(pArgVector[3], "lp4wdly", 7U) == 0) {
        IoDiag_DdrSetLp4WriteDly(DdrcId, BitMask, NewValue, PrintFunc);
    } else if (AmbaUtility_StringCompare(pArgVector[3], "lp4mr14", 7U) == 0) {
        IoDiag_DdrSetLp4MR14(DdrcId, BitMask, NewValue, PrintFunc);
    } else if (AmbaUtility_StringCompare(pArgVector[3], "lp4mr12", 7U) == 0) {
        IoDiag_DdrSetLp4MR12(DdrcId, BitMask, NewValue, PrintFunc);
    } else {
        if (AmbaUtility_StringCompare(pArgVector[3], "sync", 4U) == 0) {
            DllByte = AMBA_DDRC_DLL_BYTE_1;
        }
        if (AmbaUtility_StringCompare(pArgVector[3], "read", 4U) == 0) {
            DllByte = AMBA_DDRC_DLL_BYTE_0;
        }
        if (AmbaUtility_StringCompare(pArgVector[3], "write", 5U) == 0) {
            DllByte = AMBA_DDRC_DLL_BYTE_2;
        }

        if (IoDiag_DdrGetDllValIndex(NewValue) >= 0) {
            IoDiag_DdrSetDLL(DdrcId, BitMask, DllByte, NewValue, PrintFunc);
        }
    }
}


static void IoDiag_DdrQuickShmoo(UINT32 ArgCount, char * const *pArgVector, AMBA_SHELL_PRINT_f PrintFunc)
{
    UINT32 DllByte = 0xffffffffUL, DllValue = 0xffffffffUL, DllRegVal;
    UINT32 DdrcId = 0U, TimeInterval = 0U, BitMask = 0U;
    INT32 i, Index, Step = 0;
    UINT32 DllVfine[4] = {0};

    if (ArgCount < 5U) {
        IoDiag_DdrCmdUsage(pArgVector, PrintFunc);
    } else {
        if (IoDiag_DdrCheckDdrcParameter(pArgVector[2], &DdrcId, PrintFunc) == 0) {
            return; // Invalid parameter
        }
        if (IoDiag_DdrCheckRemainParameter(ArgCount, pArgVector, 4U, &Step, &TimeInterval, &BitMask) == 0) {
            return; // Invalid parameters
        }

        DllVfine[0] = (pAmbaDDRC_Reg[DdrcId]->DdrioDllStatusSel0 >> 16U) & 0x1fU;
        DllVfine[1] = (pAmbaDDRC_Reg[DdrcId]->DdrioDllStatusSel1 >> 16U) & 0x1fU;
        DllVfine[2] = (pAmbaDDRC_Reg[DdrcId]->DdrioDllStatusSel2 >> 16U) & 0x1fU;
        DllVfine[3] = (pAmbaDDRC_Reg[DdrcId]->DdrioDllStatusSel3 >> 16U) & 0x1fU;
        IoDiag_PrintFormattedRegValPerChan("[DDRC%d]Vfine: 0x%x,0x%x,", DdrcId, DllVfine[0], DllVfine[1], PrintFunc);
        IoDiag_PrintFormattedRegValPerChan("0x%x,0x%x\n", DllVfine[2], DllVfine[3], 0, PrintFunc);
        IoDiag_PrintFormattedRegValPerChan("[DDRC%d]DLL Lock Searching Range: 0x%x - 0x%x\n", DdrcId, (DllVfine[0] | 0x20U), (31U - DllVfine[0]), PrintFunc);

        DllRegVal = IoDiag_DdrcGetDllSetting(DdrcId, BitMask);
        if (AmbaUtility_StringCompare(pArgVector[3], "sync", 4U) == 0) {
            DllByte = AMBA_DDRC_DLL_BYTE_1;
            DllValue = (DllRegVal & 0x00ff00UL) >> 8UL;
        }
        if (AmbaUtility_StringCompare(pArgVector[3], "read", 4U) == 0) {
            DllByte = AMBA_DDRC_DLL_BYTE_0;
            DllValue = (DllRegVal & 0x0000ffUL);
        }
        if (AmbaUtility_StringCompare(pArgVector[3], "write", 5U) == 0) {
            DllByte = AMBA_DDRC_DLL_BYTE_2;
            DllValue = (DllRegVal & 0xff0000UL) >> 16UL;
        }

        Index = IoDiag_DdrGetDllValIndex(DllValue);
        if (Index < 0) {
            Index = 0;  // Set to reasonable range if the return DLL is out of our scan range
        }

        IoDiag_DdrShmooItemStart(TimeInterval);
        for (i = Index; (i >= 0) && (i < (INT32)sizeof(DiagDdrPhaseSetting)); i += Step) {
            IoDiag_DdrSetDLL(DdrcId, BitMask, DllByte, DiagDdrPhaseSetting[i], PrintFunc);
            IoDiag_DdrShmooItemWait(TimeInterval);
        }
        IoDiag_DdrShmooItemStop(PrintFunc);
    }
}

static void IoDiag_DdrLp4ReadDlyShmoo(UINT32 ArgCount, char * const *pArgVector, AMBA_SHELL_PRINT_f PrintFunc)
{
#define READ_DELAY_MAX         16
#define READ_DELAY_MIN         0
    UINT32 DdrcId = 0U, TimeInterval = 0U, BitMask = 0U, CurValue = 0U;
    INT32 i = 0, Step = 0;

    if (IoDiag_DdrCheckDdrcParameter(pArgVector[2], &DdrcId, PrintFunc) == 0) {
        return; // Invalid parameter
    }
    if (IoDiag_DdrCheckRemainParameter(ArgCount, pArgVector, 3U, &Step, &TimeInterval, &BitMask) == 0) {
        return; // Invalid parameters
    }

    CurValue = IoDiag_DdrGetLp4ReadDly(DdrcId, BitMask);  // DQ Read coarse, DM Read fine

    IoDiag_DdrShmooItemStart(TimeInterval);
    for (i = CurValue; (i >= READ_DELAY_MIN) && (i < READ_DELAY_MAX); i += Step) {
        IoDiag_DdrSetLp4ReadDly(DdrcId, BitMask, i, PrintFunc);
        IoDiag_DdrShmooItemWait(TimeInterval);
    }
    IoDiag_DdrShmooItemStop(PrintFunc);
}

static void IoDiag_DdrLp4VrefShmoo(UINT32 ArgCount, char * const *pArgVector, AMBA_SHELL_PRINT_f PrintFunc)
{
#define DQ_VREF_MAX    64
#define DQ_VREF_MIN    0
    UINT32 DdrcId = 0U, TimeInterval = 0U, BitMask = 0U, CurValue = 0U;
    INT32 i = 0, Step = 0;

    if (IoDiag_DdrCheckDdrcParameter(pArgVector[2], &DdrcId, PrintFunc) == 0) {
        return; // Invalid parameter
    }
    if (IoDiag_DdrCheckRemainParameter(ArgCount, pArgVector, 3U, &Step, &TimeInterval, &BitMask) == 0) {
        return; // Invalid parameters
    }

    CurValue = IoDiag_DdrGetLp4Vref(DdrcId, BitMask);

    IoDiag_DdrShmooItemStart(TimeInterval);
    for (i = CurValue; (i >= DQ_VREF_MIN) && (i < DQ_VREF_MAX); i += Step) {
        IoDiag_DdrSetLp4Vref(DdrcId, BitMask, i, PrintFunc);
        IoDiag_DdrShmooItemWait(TimeInterval);
    }
    IoDiag_DdrShmooItemStop(PrintFunc);
}

static void IoDiag_DdrLp4Mr14Shmoo(UINT32 ArgCount, char * const *pArgVector, AMBA_SHELL_PRINT_f PrintFunc)
{
#define LP4_MR14_MAX                    81
#define LP4_MR14_MIN                    0
    UINT32 DdrcId = 0U, TimeInterval = 0U, BitMask = 0U, CurValue = 0U;
    INT32 i = 0, Step = 0;

    if (IoDiag_DdrCheckDdrcParameter(pArgVector[2], &DdrcId, PrintFunc) == 0) {
        return; // Invalid parameter
    }
    if (IoDiag_DdrCheckRemainParameter(ArgCount, pArgVector, 3U, &Step, &TimeInterval, &BitMask) == 0) {
        return; // Invalid parameters
    }

    CurValue = IoDiag_DdrGetLp4MR14(DdrcId, BitMask);

    IoDiag_DdrShmooItemStart(TimeInterval);
    for (i = CurValue; (i >= LP4_MR14_MIN) && (i < LP4_MR14_MAX); i += Step) {
        IoDiag_DdrSetLp4MR14(DdrcId, BitMask, i, PrintFunc);
        IoDiag_DdrShmooItemWait(TimeInterval);
    }
    IoDiag_DdrShmooItemStop(PrintFunc);
}

static void IoDiag_DdrLp4Mr12Shmoo(UINT32 ArgCount, char * const *pArgVector, AMBA_SHELL_PRINT_f PrintFunc)
{
#define LP4_MR12_MAX                    81
#define LP4_MR12_MIN                    0
    UINT32 DdrcId = 0U, TimeInterval = 0U, BitMask = 0U, CurValue = 0U;
    INT32 i = 0, Step = 0;

    if (IoDiag_DdrCheckDdrcParameter(pArgVector[2], &DdrcId, PrintFunc) == 0) {
        return; // Invalid parameter
    }
    if (IoDiag_DdrCheckRemainParameter(ArgCount, pArgVector, 3U, &Step, &TimeInterval, &BitMask) == 0) {
        return; // Invalid parameters
    }

    CurValue = IoDiag_DdrGetLp4MR12(DdrcId, BitMask);

    IoDiag_DdrShmooItemStart(TimeInterval);
    for (i = CurValue; (i >= LP4_MR12_MIN) && (i < LP4_MR12_MAX); i += Step) {
        IoDiag_DdrSetLp4MR12(DdrcId, BitMask, i, PrintFunc);
        IoDiag_DdrShmooItemWait(TimeInterval);
    }
    IoDiag_DdrShmooItemStop(PrintFunc);
}

static void IoDiag_DdrLp4WrtieDlyShmoo(UINT32 ArgCount, char * const *pArgVector, AMBA_SHELL_PRINT_f PrintFunc)
{
#define WRITE_DELAY_MAX         128
#define WRITE_DELAY_MIN         0
    UINT32 DdrcId = 0U, TimeInterval = 0U, BitMask = 0U, CurValue = 0U;
    INT32 i = 0, Step = 0;

    if (IoDiag_DdrCheckDdrcParameter(pArgVector[2], &DdrcId, PrintFunc) == 0) {
        return; // Invalid parameter
    }
    if (IoDiag_DdrCheckRemainParameter(ArgCount, pArgVector, 3U, &Step, &TimeInterval, &BitMask) == 0) {
        return; // Invalid parameters
    }

    CurValue = IoDiag_DdrGetLp4WriteDly(DdrcId, BitMask);  // DQ Write coarse, DM Write fine

    IoDiag_DdrShmooItemStart(TimeInterval);
    for (i = CurValue; (i >= WRITE_DELAY_MIN) && (i < WRITE_DELAY_MAX); i += Step) {
        IoDiag_DdrSetLp4WriteDly(DdrcId, BitMask, i, PrintFunc);
        IoDiag_DdrShmooItemWait(TimeInterval);
    }
    IoDiag_DdrShmooItemStop(PrintFunc);
}

void IoDiag_DdrDoShmoo(UINT32 ArgCount, char * const *pArgVector, AMBA_SHELL_PRINT_f PrintFunc)
{
    if (AmbaUtility_StringCompare(pArgVector[1], "shmoo", 5U) == 0) {
        IoDiag_DdrQuickShmoo(ArgCount, pArgVector, PrintFunc);
    } else if (AmbaUtility_StringCompare(pArgVector[1], "set", 3U) == 0) {
        IoDiag_DdrSetValue(ArgCount, pArgVector, PrintFunc);
    } else if (AmbaUtility_StringCompare(pArgVector[1], "lp4rdly", 7U) == 0) {
        IoDiag_DdrLp4ReadDlyShmoo(ArgCount, pArgVector, PrintFunc);
    } else if (AmbaUtility_StringCompare(pArgVector[1], "lp4vref", 7U) == 0) {
        IoDiag_DdrLp4VrefShmoo(ArgCount, pArgVector, PrintFunc);
    } else if (AmbaUtility_StringCompare(pArgVector[1], "lp4wdly", 7U) == 0) {
        IoDiag_DdrLp4WrtieDlyShmoo(ArgCount, pArgVector, PrintFunc);
    } else if (AmbaUtility_StringCompare(pArgVector[1], "lp4mr14", 7U) == 0) {
        IoDiag_DdrLp4Mr14Shmoo(ArgCount, pArgVector, PrintFunc);
    } else if (AmbaUtility_StringCompare(pArgVector[1], "lp4mr12", 7U) == 0) {
        IoDiag_DdrLp4Mr12Shmoo(ArgCount, pArgVector, PrintFunc);
    } else {
        IoDiag_DdrCmdUsage(pArgVector, PrintFunc);
    }
}

static void * IoDiag_DdrTaskEntry(void * EntryArg)
{
    UINT32 ArgCount = 0U, i;
    char * ArgVector[MAX_ARGV_NUM];

    AmbaMisra_TypeCast32(&ArgCount, &EntryArg);

    if (ArgCount == DiagDdrTask.ArgCount) {
        for (i = 0; i < ArgCount; i++) {
            ArgVector[i] = DiagDdrTask.ArgVector[i];
        }
        IoDiag_DdrDoShmoo(ArgCount, ArgVector, DiagDdrPrintFunc);
    }
    // Mark task end
    DiagDdrTask.TaskRunning = 0;

    return NULL;
}

static UINT32 IoDiag_DdrTaskStart(UINT32 ArgCount, char * const *pArgVector, AMBA_SHELL_PRINT_f PrintFunc)
{
    UINT32 RetVal = ~OK;
    void * EntryArg;
    UINT32 ArgUINT32[1], i;

    if (DiagDdrTask.TaskRunning == 0U) {
        // Found idle task to run
        if (DiagDdrTask.TaskCreated == 1U) {
            // Task has been used, delete it before re-create
            (void)AmbaKAL_TaskTerminate(&DiagDdrTask.Task);
            (void)AmbaKAL_TaskDelete(&DiagDdrTask.Task);
        }
        // Setup parameter for new task
        if (DiagDdrPrintFunc == NULL) {
            DiagDdrPrintFunc = PrintFunc;
        }
        if (ArgCount > MAX_ARGV_NUM) {
            ArgCount = MAX_ARGV_NUM;
        }
        DiagDdrTask.ArgCount = ArgCount;
        for (i = 0; i < ArgCount; i++) {
            AmbaUtility_StringCopy(DiagDdrTask.ArgVector[i], MAX_ARGV_LEN, pArgVector[i]);
        }
        DiagDdrTask.TaskCreated = 1;
        DiagDdrTask.TaskRunning = 1;
        ArgUINT32[0] = i;
        (void)AmbaUtility_StringPrintUInt32(DiagDdrTask.TaskName, TASK_NAME_LEN, "DiagDdr%d", 1U, ArgUINT32);
        AmbaMisra_TypeCast32(&EntryArg, &i);
        AmbaMisra_TouchUnused(EntryArg);
        // Create new task
        RetVal = AmbaKAL_TaskCreate(&DiagDdrTask.Task,
                                    DiagDdrTask.TaskName,
                                    AMBA_KAL_TASK_LOWEST_PRIORITY - 8U,
                                    IoDiag_DdrTaskEntry,
                                    EntryArg,
                                    DiagDdrTask.Stack,
                                    sizeof(DiagDdrTask.Stack),
                                    AMBA_KAL_AUTO_START);
        if (RetVal == OK) {
            PrintFunc("[Diag][DDR] ");
            PrintFunc(DiagDdrTask.TaskName);
            PrintFunc(" starts.\n");
        }
    } else {
        PrintFunc("[Diag][DDR] Tasks is running\n");
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaDiag_DDR
 *
 *  @Description:: DDR diagnostic command
 *
 *  @Input      ::
 *      ArgCount:   Argument count
 *      pArgVector: Argument vector
 *      PrintFunc:  function to print messages on shell task
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          int : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
void AmbaIOUTDiag_CmdDDR(UINT32 ArgCount, char * const *pArgVector, AMBA_SHELL_PRINT_f PrintFunc)
{
    if (ArgCount < 2U) {
        IoDiag_DdrCmdUsage(pArgVector, PrintFunc);
    } else {
        if (AmbaUtility_StringCompare(pArgVector[1], "dump", 4U) == 0) {
            IoDiag_DdrDump(PrintFunc);
        } else if (AmbaUtility_StringCompare(pArgVector[1], "wdt", 3U) == 0) {
            if ((ArgCount >= 3) && AmbaUtility_StringCompare(pArgVector[2], "1", 1U) == 0) {
                DiagDdrEnableWdt = 1;
            } else {
                DiagDdrEnableWdt = 0;
            }
        } else if (AmbaUtility_StringCompare(pArgVector[1], "task", 4U) == 0) {
            ArgCount -= 1U;
            IoDiag_DdrTaskStart(ArgCount, &pArgVector[1], PrintFunc);
        } else {
            IoDiag_DdrDoShmoo(ArgCount, pArgVector, PrintFunc);
        }
    }
}
