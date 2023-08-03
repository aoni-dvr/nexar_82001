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
#include "AmbaRTSL_DRAMC.h"
#include "AmbaDRAMC.h"
#include "AmbaWDT.h"

#include "AmbaDramAdjust.h"

#define DEFAULT_TIMER_INTERVAL      20000UL // 20 seconds
#define MAX_WDT_FEED_INTERVAL       5000UL  // We will set WDT timeout to (2 * FEED_INTERVAL) + 2, larger interval may cause timeout overflow.
#define DEFAULT_ACCESS_MASK         0xffffffffUL    // All bits

#define READ_DELAY_BITS             5U      // 5 bits
#define READ_DELAY_POS_COARSE       15U     // [19:15]
#define READ_DELAY_POS_FINE         25U     // [29:25]
#define READ_DELAY_MASK             ((1U << READ_DELAY_BITS) - 1U)
#define DQ_VREF_BITS                6U      // 6 bits
#define DQ_VREF_POS1                0U      // [5:0]
#define DQ_VREF_POS2                16U     // [21:16]
#define DQ_VREF_MASK                ((1U << DQ_VREF_BITS) - 1U)
#define LP4_MR14_BITS               7U
#define LP4_MR14_MASK               ((1U << LP4_MR14_BITS) - 1U)
#define LP4_MR12_BITS               7U
#define LP4_MR12_MASK               ((1U << LP4_MR12_BITS) - 1U)
#define WRITE_DELAY_BITS            5U      // 5 bits
#define WRITE_DELAY_POS_COARSE      10U     // [14:10]
#define WRITE_DELAY_POS_FINE        20U     // [24:20]
#define WRITE_DELAY_MASK            ((1U << WRITE_DELAY_BITS) - 1U)
#define DQS_VREF_BITS               6U      // 6 bits
#define DQS_VREF_POS1               6U      // [11:6]
#define DQS_VREF_POS2               22U     // [27:22]
#define DQS_VREF_MASK               ((1U << DQS_VREF_BITS) - 1U)
#define WCK_DLY_BITS                5U      // 5 bits
#define WCK_DLY_POS_COARSE          5U      // Delay0[9:5]
#define WCK_DLY_POS_FINE            0U      // Delay0[4:0]
#define WCK_DLY_MASK                ((1U << WCK_DLY_BITS) - 1U)
#define DQS_WRITE_DLY_BITS          5U      // 5 bits
#define DQS_WRITE_DLY_POS_COARSE    15U     // Delay0[19:15]
#define DQS_WRITE_DLY_POS_FINE      10U     // Delay0[14:10]
#define DQS_WRITE_DLY_MASK          ((1U << DQS_WRITE_DLY_BITS) - 1U)
#define DQS_GATE_DLY_BITS           5U      // 5 bits
#define DQS_GATE_DLY_POS_COARSE     5U      // Delay1[9:5]
#define DQS_GATE_DLY_POS_FINE       0U      // Delay1[4:0]
#define DQS_GATE_DLY_MASK           ((1U << DQS_GATE_DLY_BITS) - 1U)
#define CK_DLY_BITS                 5U      // 5 bits
#define CK_DLY_POSA_COARSE          5U      // CK_Delay[9:5]
#define CK_DLY_POSA_FINE            0U      // CK_Delay[4:0]
#define CK_DLY_POSB_COARSE          21U     // CK_Delay[25:21]
#define CK_DLY_POSB_FINE            16U     // CK_Delay[20:16]
#define CK_DLY_MASK                 ((1U << CK_DLY_BITS) - 1U)
#define CKE_DLY_BITS                5U      // 5 bits
#define CKE_DLY_POSA_COARSE         0U      // CKE_Coarse[4:0]
#define CKE_DLY_POSB_COARSE         8U      // CKE_Coarse[12:8]
#define CKE_DLY_POS0A_FINE          0U      // CKE_Fine[4:0]
#define CKE_DLY_POS0B_FINE          8U      // CKE_Fine[12:8]
#define CKE_DLY_POS1A_FINE          16U     // CKE_Fine[20:16]
#define CKE_DLY_POS1B_FINE          24U     // CKE_Fine[28:24]
#define CKE_DLY_MASK                ((1U << CKE_DLY_BITS) - 1U)
#define CA_DLY_BITS                 5U      // 5 bits
#define CA_DLY_POSA_COARSE          0U      // CA_Coarse[4:0]
#define CA_DLY_POSB_COARSE          8U      // CA_Coarse[12:8]
#define CA_DLY_POS0_ADDR0_FINE      0U      // CA_Fine[0][4:0]
#define CA_DLY_POS0_ADDR1_FINE      8U      // CA_Fine[0][12:8]
#define CA_DLY_POS0_ADDR2_FINE      16U     // CA_Fine[0][20:16]
#define CA_DLY_POS0_ADDR3_FINE      24U     // CA_Fine[0][28:24]
#define CA_DLY_POS0_ADDR4_FINE      0U      // CA_Fine[1][4:0]
#define CA_DLY_POS0_ADDR5_FINE      8U      // CA_Fine[1][12:8]
#define CA_DLY_POS0_ADDR6_FINE      16U     // CA_Fine[1][20:16]
#define CA_DLY_MASK                 ((1U << CA_DLY_BITS) - 1U)
#define LP5_MR14_BITS               7U
#define LP5_MR14_MASK               ((1U << LP5_MR14_BITS) - 1U)
#define LP5_MR15_BITS               7U
#define LP5_MR15_MASK               ((1U << LP5_MR15_BITS) - 1U)
#define LP5_MR12_BITS               7U
#define LP5_MR12_MASK               ((1U << LP5_MR12_BITS) - 1U)

/* Definitions of DDR controller */
#define AMBA_DDRC0                  0U      /* DDRC0 */
#if defined(CONFIG_SOC_CV5)
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
    UINT64 ArgUINT64[3];

    AmbaMisra_TypeCast64(&pUINT32, &pRegAddr);
    AmbaMisra_TypeCast64(&ArgUINT64[0], &pUINT32);
    ArgUINT64[1] = (UINT64) *pUINT32;
    (void)AmbaUtility_StringPrintUInt64(StrBuf, sizeof(StrBuf), pFmtString, 2U, ArgUINT64);
    PrintFunc(StrBuf);
}

static void IoDiag_PrintFormattedRegValPerChan(const char *pFmtString, UINT32 DdrcId, UINT32 RegValue, UINT32 ChanValue, AMBA_SHELL_PRINT_f PrintFunc)
{
    char StrBuf[128];
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
    UINT32 RetVal = 0U;

    /* Host number */
    RetVal = AmbaDRAMC_GetHostNum(&HostNumber);
    if (RetVal != DRAMC_ERR_NONE) {
        return RetVal;
    }

    /* Host Start id */
    RetVal = AmbaRTSL_DramcGetHostStartId(&HostStart);

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

static UINT32 IoDiag_DdrcGetModeReg(UINT32 DdrcId, UINT32 BitMask)
{
    if (BitMask & 0xffffU) {
        return AmbaCSL_DdrcGetModeReg(DdrcId) & 0xffU;
    } else if (BitMask & 0xffff0000U) {
        return (AmbaCSL_DdrcGetModeReg(DdrcId) >> 8) & 0xffU;
    }

    return 0;
}

static void IoDiag_DdrcSetModeRegWait(UINT32 DdrcId, UINT32 BitMask, UINT32 Val)
{
    UINT32  Value = 0;

    Val |= 0x18000000U; // Set bit [27~28] to be default both channel
    if (BitMask & 0xffffU) {
        if ((BitMask & 0xffff0000U) == 0) {
            Val &= ~(0x2U << 27U);  // Single channel 0
        }
    } else if (BitMask & 0xffff0000U) {
        Val &= ~(0x1U << 27U);      // Single channel 1
    }

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
    PrintFunc("\n The following commands are for LPDDR4/LPDDR5 only!\n");
    PrintFunc(" [lp4rdly|lp4vref|lp4wdly|lp4mr14|lp4mr12] [0:DDRC0|1:DDRC1] [+|-] [time(ms)] [step] [bitmask]\n");
    PrintFunc(" [dqsvref|wckdly|dqswdly|dqsgdly|ckdly|ckedly|cadly|lp5mr14|lp5mr15|lp5mr12] [0:DDRC0|1:DDRC1] [+|-] [time(ms)] [step] [bitmask]\n");
    PrintFunc("          diag ddr lp4rdly 0 + 30000 1 0xffffffff\n");
    PrintFunc("          Read : lp4rdly and lp4vref\n");
    PrintFunc("          Write: lp4wdly and lp4mr14\n");
}

static void IoDiag_DdrDump(AMBA_SHELL_PRINT_f PrintFunc)
{
    const char DiagDdrDramType[4][8] = {
        [0] = "LPDDR4",
        [1] = "LPDDR5",
        [2] = "N/A",
        [3] = "N/A",
    };
    const char DiagDdrDramSize[10][8] = {
        [0] = "2Gb",
        [1] = "3Gb",
        [2] = "4Gb",
        [3] = "6Gb",
        [4] = "8Gb",
        [5] = "12Gb",
        [6] = "16Gb",
        [7] = "24Gb",
        [8] = "32Gb",
        [9] = "N/A",
    };
#if 0
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
#endif
    UINT32 Value = 0;
    UINT32 DdrcId, DdrcIdStart = 0U, DdrcIdEnd = 0U, DdrcNumber = 0U;

    IoDiag_DdrGetDdrcEnableId(&DdrcNumber, &DdrcIdStart, &DdrcIdEnd);

    PrintFunc("--------------------------------------------------------------------------------\n");

    /* dram freq */
    IoDiag_PrintFormattedInt("DRAM Frequency      = %u\n", AmbaRTSL_PllGetDramClk(), PrintFunc);
    /* Host number */
    IoDiag_PrintFormattedInt("Host number         = %u\n", DdrcNumber, PrintFunc);

    for (DdrcId = DdrcIdStart; DdrcId <= DdrcIdEnd; DdrcId ++) {
        PrintFunc("--------------------------------------------------------------------------------\n");

        /* ddrc id */
        IoDiag_PrintFormattedInt("DDRC-%d:\n", DdrcId, PrintFunc);

        /* dram type */
        Value = AmbaCSL_DdrcGetDramType(DdrcId);
        PrintFunc("DRAM Type = ");
        PrintFunc(DiagDdrDramType[Value]);
        PrintFunc("\n");
        /* dram size */
        Value = AmbaCSL_DdrcGetDramSize(DdrcId);
        PrintFunc("DRAM Size = ");
        PrintFunc(DiagDdrDramSize[Value]);
        PrintFunc(" (per Channel)\n");
        PrintFunc("--------------------------------------------------------------------------------\n");

        IoDiag_PrintFormattedRegVal("DRAM_Ctrl                    (0x%x)  = 0x%.8x\n", &pAmbaDDRC_Reg[DdrcId]->Ctrl,                 PrintFunc);
        IoDiag_PrintFormattedRegVal("DRAM_Config0                 (0x%x)  = 0x%.8x\n", &pAmbaDDRC_Reg[DdrcId]->Config0,              PrintFunc);
        IoDiag_PrintFormattedRegVal("DRAM_Config1                 (0x%x)  = 0x%.8x\n", &pAmbaDDRC_Reg[DdrcId]->Config1,              PrintFunc);
        IoDiag_PrintFormattedRegVal("DRAM_Timing1                 (0x%x)  = 0x%.8x\n", &pAmbaDDRC_Reg[DdrcId]->Timing0,              PrintFunc);
        IoDiag_PrintFormattedRegVal("DRAM_Timing2                 (0x%x)  = 0x%.8x\n", &pAmbaDDRC_Reg[DdrcId]->Timing1,              PrintFunc);
        IoDiag_PrintFormattedRegVal("DRAM_Timing3                 (0x%x)  = 0x%.8x\n", &pAmbaDDRC_Reg[DdrcId]->Timing2,              PrintFunc);
        IoDiag_PrintFormattedRegVal("DRAM_Timing4                 (0x%x)  = 0x%.8x\n", &pAmbaDDRC_Reg[DdrcId]->Timing3,              PrintFunc);
        IoDiag_PrintFormattedRegVal("DRAM_Timing5                 (0x%x)  = 0x%.8x\n", &pAmbaDDRC_Reg[DdrcId]->Timing4,              PrintFunc);
        IoDiag_PrintFormattedRegVal("DuelDieTiming                (0x%x)  = 0x%.8x\n", &pAmbaDDRC_Reg[DdrcId]->DuelDieTiming,        PrintFunc);
        IoDiag_PrintFormattedRegVal("RefreshTiming                (0x%x)  = 0x%.8x\n", &pAmbaDDRC_Reg[DdrcId]->RefreshTiming,        PrintFunc);
        IoDiag_PrintFormattedRegVal("Lp5Timing                    (0x%x)  = 0x%.8x\n", &pAmbaDDRC_Reg[DdrcId]->Lp5Timing,            PrintFunc);
        IoDiag_PrintFormattedRegVal("ByteMap                      (0x%x)  = 0x%.8x\n", &pAmbaDDRC_Reg[DdrcId]->ByteMap,              PrintFunc);
        IoDiag_PrintFormattedRegVal("WdqsTiming                   (0x%x)  = 0x%.8x\n", &pAmbaDDRC_Reg[DdrcId]->WdqsTiming,           PrintFunc);
        IoDiag_PrintFormattedRegVal("DtteTiming                   (0x%x)  = 0x%.8x\n", &pAmbaDDRC_Reg[DdrcId]->DtteTiming,           PrintFunc);
        IoDiag_PrintFormattedRegVal("DllCtrlSel0D0                (0x%x)  = 0x%.8x\n", &pAmbaDDRC_Reg[DdrcId]->DllCtrlSel0D0,        PrintFunc);
        IoDiag_PrintFormattedRegVal("DllCtrlSel0D1                (0x%x)  = 0x%.8x\n", &pAmbaDDRC_Reg[DdrcId]->DllCtrlSel0D1,        PrintFunc);
        IoDiag_PrintFormattedRegVal("DllCtrlSel1D0                (0x%x)  = 0x%.8x\n", &pAmbaDDRC_Reg[DdrcId]->DllCtrlSel1D0,        PrintFunc);
        IoDiag_PrintFormattedRegVal("DllCtrlSel1D1                (0x%x)  = 0x%.8x\n", &pAmbaDDRC_Reg[DdrcId]->DllCtrlSel1D1,        PrintFunc);
        IoDiag_PrintFormattedRegVal("DllCtrlSel2D0                (0x%x)  = 0x%.8x\n", &pAmbaDDRC_Reg[DdrcId]->DllCtrlSel2D0,        PrintFunc);
        IoDiag_PrintFormattedRegVal("DllCtrlSel2D1                (0x%x)  = 0x%.8x\n", &pAmbaDDRC_Reg[DdrcId]->DllCtrlSel2D1,        PrintFunc);
        IoDiag_PrintFormattedRegVal("Byte0Die0Dly[0]              (0x%x)  = 0x%.8x\n", &pAmbaDDRC_Reg[DdrcId]->Byte0Die0Dly[0],      PrintFunc);
        IoDiag_PrintFormattedRegVal("Byte0Die0Dly[1]              (0x%x)  = 0x%.8x\n", &pAmbaDDRC_Reg[DdrcId]->Byte0Die0Dly[1],      PrintFunc);
        IoDiag_PrintFormattedRegVal("Byte0Die0Dly[2]              (0x%x)  = 0x%.8x\n", &pAmbaDDRC_Reg[DdrcId]->Byte0Die0Dly[2],      PrintFunc);
        IoDiag_PrintFormattedRegVal("Byte0Die0Dly[3]              (0x%x)  = 0x%.8x\n", &pAmbaDDRC_Reg[DdrcId]->Byte0Die0Dly[3],      PrintFunc);
        IoDiag_PrintFormattedRegVal("Byte0Die0Dly[4]              (0x%x)  = 0x%.8x\n", &pAmbaDDRC_Reg[DdrcId]->Byte0Die0Dly[4],      PrintFunc);
        IoDiag_PrintFormattedRegVal("Byte0Die0Dly[5]              (0x%x)  = 0x%.8x\n", &pAmbaDDRC_Reg[DdrcId]->Byte0Die0Dly[5],      PrintFunc);
        IoDiag_PrintFormattedRegVal("Byte1Die0Dly[0]              (0x%x)  = 0x%.8x\n", &pAmbaDDRC_Reg[DdrcId]->Byte1Die0Dly[0],      PrintFunc);
        IoDiag_PrintFormattedRegVal("Byte1Die0Dly[1]              (0x%x)  = 0x%.8x\n", &pAmbaDDRC_Reg[DdrcId]->Byte1Die0Dly[1],      PrintFunc);
        IoDiag_PrintFormattedRegVal("Byte1Die0Dly[2]              (0x%x)  = 0x%.8x\n", &pAmbaDDRC_Reg[DdrcId]->Byte1Die0Dly[2],      PrintFunc);
        IoDiag_PrintFormattedRegVal("Byte1Die0Dly[3]              (0x%x)  = 0x%.8x\n", &pAmbaDDRC_Reg[DdrcId]->Byte1Die0Dly[3],      PrintFunc);
        IoDiag_PrintFormattedRegVal("Byte1Die0Dly[4]              (0x%x)  = 0x%.8x\n", &pAmbaDDRC_Reg[DdrcId]->Byte1Die0Dly[4],      PrintFunc);
        IoDiag_PrintFormattedRegVal("Byte1Die0Dly[5]              (0x%x)  = 0x%.8x\n", &pAmbaDDRC_Reg[DdrcId]->Byte1Die0Dly[5],      PrintFunc);
        IoDiag_PrintFormattedRegVal("Byte2Die0Dly[0]              (0x%x)  = 0x%.8x\n", &pAmbaDDRC_Reg[DdrcId]->Byte2Die0Dly[0],      PrintFunc);
        IoDiag_PrintFormattedRegVal("Byte2Die0Dly[1]              (0x%x)  = 0x%.8x\n", &pAmbaDDRC_Reg[DdrcId]->Byte2Die0Dly[1],      PrintFunc);
        IoDiag_PrintFormattedRegVal("Byte2Die0Dly[2]              (0x%x)  = 0x%.8x\n", &pAmbaDDRC_Reg[DdrcId]->Byte2Die0Dly[2],      PrintFunc);
        IoDiag_PrintFormattedRegVal("Byte2Die0Dly[3]              (0x%x)  = 0x%.8x\n", &pAmbaDDRC_Reg[DdrcId]->Byte2Die0Dly[3],      PrintFunc);
        IoDiag_PrintFormattedRegVal("Byte2Die0Dly[4]              (0x%x)  = 0x%.8x\n", &pAmbaDDRC_Reg[DdrcId]->Byte2Die0Dly[4],      PrintFunc);
        IoDiag_PrintFormattedRegVal("Byte2Die0Dly[5]              (0x%x)  = 0x%.8x\n", &pAmbaDDRC_Reg[DdrcId]->Byte2Die0Dly[5],      PrintFunc);
        IoDiag_PrintFormattedRegVal("Byte3Die0Dly[0]              (0x%x)  = 0x%.8x\n", &pAmbaDDRC_Reg[DdrcId]->Byte3Die0Dly[0],      PrintFunc);
        IoDiag_PrintFormattedRegVal("Byte3Die0Dly[1]              (0x%x)  = 0x%.8x\n", &pAmbaDDRC_Reg[DdrcId]->Byte3Die0Dly[1],      PrintFunc);
        IoDiag_PrintFormattedRegVal("Byte3Die0Dly[2]              (0x%x)  = 0x%.8x\n", &pAmbaDDRC_Reg[DdrcId]->Byte3Die0Dly[2],      PrintFunc);
        IoDiag_PrintFormattedRegVal("Byte3Die0Dly[3]              (0x%x)  = 0x%.8x\n", &pAmbaDDRC_Reg[DdrcId]->Byte3Die0Dly[3],      PrintFunc);
        IoDiag_PrintFormattedRegVal("Byte3Die0Dly[4]              (0x%x)  = 0x%.8x\n", &pAmbaDDRC_Reg[DdrcId]->Byte3Die0Dly[4],      PrintFunc);
        IoDiag_PrintFormattedRegVal("Byte3Die0Dly[5]              (0x%x)  = 0x%.8x\n", &pAmbaDDRC_Reg[DdrcId]->Byte3Die0Dly[5],      PrintFunc);
        if (pAmbaDDRC_Reg[DdrcId]->Config0.DuelDieEn == 1U) {
            IoDiag_PrintFormattedRegVal("Byte0Die1Dly[0]              (0x%x)  = 0x%.8x\n", &pAmbaDDRC_Reg[DdrcId]->Byte0Die1Dly[0],      PrintFunc);
            IoDiag_PrintFormattedRegVal("Byte0Die1Dly[1]              (0x%x)  = 0x%.8x\n", &pAmbaDDRC_Reg[DdrcId]->Byte0Die1Dly[1],      PrintFunc);
            IoDiag_PrintFormattedRegVal("Byte0Die1Dly[2]              (0x%x)  = 0x%.8x\n", &pAmbaDDRC_Reg[DdrcId]->Byte0Die1Dly[2],      PrintFunc);
            IoDiag_PrintFormattedRegVal("Byte0Die1Dly[3]              (0x%x)  = 0x%.8x\n", &pAmbaDDRC_Reg[DdrcId]->Byte0Die1Dly[3],      PrintFunc);
            IoDiag_PrintFormattedRegVal("Byte0Die1Dly[4]              (0x%x)  = 0x%.8x\n", &pAmbaDDRC_Reg[DdrcId]->Byte0Die1Dly[4],      PrintFunc);
            IoDiag_PrintFormattedRegVal("Byte0Die1Dly[5]              (0x%x)  = 0x%.8x\n", &pAmbaDDRC_Reg[DdrcId]->Byte0Die1Dly[5],      PrintFunc);
            IoDiag_PrintFormattedRegVal("Byte1Die1Dly[0]              (0x%x)  = 0x%.8x\n", &pAmbaDDRC_Reg[DdrcId]->Byte1Die1Dly[0],      PrintFunc);
            IoDiag_PrintFormattedRegVal("Byte1Die1Dly[1]              (0x%x)  = 0x%.8x\n", &pAmbaDDRC_Reg[DdrcId]->Byte1Die1Dly[1],      PrintFunc);
            IoDiag_PrintFormattedRegVal("Byte1Die1Dly[2]              (0x%x)  = 0x%.8x\n", &pAmbaDDRC_Reg[DdrcId]->Byte1Die1Dly[2],      PrintFunc);
            IoDiag_PrintFormattedRegVal("Byte1Die1Dly[3]              (0x%x)  = 0x%.8x\n", &pAmbaDDRC_Reg[DdrcId]->Byte1Die1Dly[3],      PrintFunc);
            IoDiag_PrintFormattedRegVal("Byte1Die1Dly[4]              (0x%x)  = 0x%.8x\n", &pAmbaDDRC_Reg[DdrcId]->Byte1Die1Dly[4],      PrintFunc);
            IoDiag_PrintFormattedRegVal("Byte1Die1Dly[5]              (0x%x)  = 0x%.8x\n", &pAmbaDDRC_Reg[DdrcId]->Byte1Die1Dly[5],      PrintFunc);
            IoDiag_PrintFormattedRegVal("Byte2Die1Dly[0]              (0x%x)  = 0x%.8x\n", &pAmbaDDRC_Reg[DdrcId]->Byte2Die1Dly[0],      PrintFunc);
            IoDiag_PrintFormattedRegVal("Byte2Die1Dly[1]              (0x%x)  = 0x%.8x\n", &pAmbaDDRC_Reg[DdrcId]->Byte2Die1Dly[1],      PrintFunc);
            IoDiag_PrintFormattedRegVal("Byte2Die1Dly[2]              (0x%x)  = 0x%.8x\n", &pAmbaDDRC_Reg[DdrcId]->Byte2Die1Dly[2],      PrintFunc);
            IoDiag_PrintFormattedRegVal("Byte2Die1Dly[3]              (0x%x)  = 0x%.8x\n", &pAmbaDDRC_Reg[DdrcId]->Byte2Die1Dly[3],      PrintFunc);
            IoDiag_PrintFormattedRegVal("Byte2Die1Dly[4]              (0x%x)  = 0x%.8x\n", &pAmbaDDRC_Reg[DdrcId]->Byte2Die1Dly[4],      PrintFunc);
            IoDiag_PrintFormattedRegVal("Byte2Die1Dly[5]              (0x%x)  = 0x%.8x\n", &pAmbaDDRC_Reg[DdrcId]->Byte2Die1Dly[5],      PrintFunc);
            IoDiag_PrintFormattedRegVal("Byte3Die1Dly[0]              (0x%x)  = 0x%.8x\n", &pAmbaDDRC_Reg[DdrcId]->Byte3Die1Dly[0],      PrintFunc);
            IoDiag_PrintFormattedRegVal("Byte3Die1Dly[1]              (0x%x)  = 0x%.8x\n", &pAmbaDDRC_Reg[DdrcId]->Byte3Die1Dly[1],      PrintFunc);
            IoDiag_PrintFormattedRegVal("Byte3Die1Dly[2]              (0x%x)  = 0x%.8x\n", &pAmbaDDRC_Reg[DdrcId]->Byte3Die1Dly[2],      PrintFunc);
            IoDiag_PrintFormattedRegVal("Byte3Die1Dly[3]              (0x%x)  = 0x%.8x\n", &pAmbaDDRC_Reg[DdrcId]->Byte3Die1Dly[3],      PrintFunc);
            IoDiag_PrintFormattedRegVal("Byte3Die1Dly[4]              (0x%x)  = 0x%.8x\n", &pAmbaDDRC_Reg[DdrcId]->Byte3Die1Dly[4],      PrintFunc);
            IoDiag_PrintFormattedRegVal("Byte3Die1Dly[5]              (0x%x)  = 0x%.8x\n", &pAmbaDDRC_Reg[DdrcId]->Byte3Die1Dly[5],      PrintFunc);
        }
        IoDiag_PrintFormattedRegVal("CkDly                        (0x%x)  = 0x%.8x\n", &pAmbaDDRC_Reg[DdrcId]->CkDly,                PrintFunc);
        IoDiag_PrintFormattedRegVal("CaDlyCoarse                  (0x%x)  = 0x%.8x\n", &pAmbaDDRC_Reg[DdrcId]->CaDlyCoarse,          PrintFunc);
        IoDiag_PrintFormattedRegVal("CaDlyFineChADie0[0]          (0x%x)  = 0x%.8x\n", &pAmbaDDRC_Reg[DdrcId]->CaDlyFineChADie0[0],  PrintFunc);
        IoDiag_PrintFormattedRegVal("CaDlyFineChADie0[1]          (0x%x)  = 0x%.8x\n", &pAmbaDDRC_Reg[DdrcId]->CaDlyFineChADie0[1],  PrintFunc);
        if (pAmbaDDRC_Reg[DdrcId]->Config0.DuelDieEn == 1U) {
            IoDiag_PrintFormattedRegVal("CaDlyFineChADie1[0]          (0x%x)  = 0x%.8x\n", &pAmbaDDRC_Reg[DdrcId]->CaDlyFineChADie1[0],  PrintFunc);
            IoDiag_PrintFormattedRegVal("CaDlyFineChADie1[1]          (0x%x)  = 0x%.8x\n", &pAmbaDDRC_Reg[DdrcId]->CaDlyFineChADie1[1],  PrintFunc);
        }
        IoDiag_PrintFormattedRegVal("CaDlyFineChBDie0[0]          (0x%x)  = 0x%.8x\n", &pAmbaDDRC_Reg[DdrcId]->CaDlyFineChBDie0[0],  PrintFunc);
        IoDiag_PrintFormattedRegVal("CaDlyFineChBDie0[1]          (0x%x)  = 0x%.8x\n", &pAmbaDDRC_Reg[DdrcId]->CaDlyFineChBDie0[1],  PrintFunc);
        if (pAmbaDDRC_Reg[DdrcId]->Config0.DuelDieEn == 1U) {
            IoDiag_PrintFormattedRegVal("CaDlyFineChBDie1[0]          (0x%x)  = 0x%.8x\n", &pAmbaDDRC_Reg[DdrcId]->CaDlyFineChBDie1[0],  PrintFunc);
            IoDiag_PrintFormattedRegVal("CaDlyFineChBDie1[1]          (0x%x)  = 0x%.8x\n", &pAmbaDDRC_Reg[DdrcId]->CaDlyFineChBDie1[1],  PrintFunc);
        }
        IoDiag_PrintFormattedRegVal("CkeDlyCoarse                 (0x%x)  = 0x%.8x\n", &pAmbaDDRC_Reg[DdrcId]->CkeDlyCoarse,         PrintFunc);
        IoDiag_PrintFormattedRegVal("CkeDlyFine                   (0x%x)  = 0x%.8x\n", &pAmbaDDRC_Reg[DdrcId]->CkeDlyFine,           PrintFunc);
        IoDiag_PrintFormattedRegVal("CaPadCtrl                    (0x%x)  = 0x%.8x\n", &pAmbaDDRC_Reg[DdrcId]->CaPadCtrl,            PrintFunc);
        IoDiag_PrintFormattedRegVal("DqPadCtrl                    (0x%x)  = 0x%.8x\n", &pAmbaDDRC_Reg[DdrcId]->DqPadCtrl,            PrintFunc);
        IoDiag_PrintFormattedRegVal("RdVref0[0]                   (0x%x)  = 0x%.8x\n", &pAmbaDDRC_Reg[DdrcId]->RdVref0,              PrintFunc);
        IoDiag_PrintFormattedRegVal("RdVref0[1]                   (0x%x)  = 0x%.8x\n", &pAmbaDDRC_Reg[DdrcId]->RdVref1,              PrintFunc);
        IoDiag_PrintFormattedRegVal("DllStatus[0]                 (0x%x)  = 0x%.8x\n", &pAmbaDDRC_Reg[DdrcId]->DllStatus0,           PrintFunc);
        IoDiag_PrintFormattedRegVal("DllStatus[1]                 (0x%x)  = 0x%.8x\n", &pAmbaDDRC_Reg[DdrcId]->DllStatus1,           PrintFunc);

        IoDiag_DdrcSetModeRegWait(DdrcId, 0xffffffff, 0x9a010000);
        IoDiag_PrintFormattedRegVal("DRAM_Mode1RegD0              (0x%x)  = 0x%.8x\n", &pAmbaDDRC_Reg[DdrcId]->ModeReg,              PrintFunc);
        IoDiag_DdrcSetModeRegWait(DdrcId, 0xffffffff, 0x9a0b0000);
        IoDiag_PrintFormattedRegVal("DRAM_Mode11RegD0             (0x%x)  = 0x%.8x\n", &pAmbaDDRC_Reg[DdrcId]->ModeReg,              PrintFunc);
        IoDiag_DdrcSetModeRegWait(DdrcId, 0xffffffff, 0x9a0c0000);
        IoDiag_PrintFormattedRegVal("DRAM_Mode12RegD0             (0x%x)  = 0x%.8x\n", &pAmbaDDRC_Reg[DdrcId]->ModeReg,              PrintFunc);
        IoDiag_DdrcSetModeRegWait(DdrcId, 0xffffffff, 0x9a0e0000);
        IoDiag_PrintFormattedRegVal("DRAM_Mode14RegD0             (0x%x)  = 0x%.8x\n", &pAmbaDDRC_Reg[DdrcId]->ModeReg,              PrintFunc);
        IoDiag_DdrcSetModeRegWait(DdrcId, 0xffffffff, 0x9a0f0000);
        IoDiag_PrintFormattedRegVal("DRAM_Mode15RegD0             (0x%x)  = 0x%.8x\n", &pAmbaDDRC_Reg[DdrcId]->ModeReg,              PrintFunc);
        IoDiag_DdrcSetModeRegWait(DdrcId, 0xffffffff, 0x9a100000);
        IoDiag_PrintFormattedRegVal("DRAM_Mode16RegD0             (0x%x)  = 0x%.8x\n", &pAmbaDDRC_Reg[DdrcId]->ModeReg,              PrintFunc);
        IoDiag_DdrcSetModeRegWait(DdrcId, 0xffffffff, 0x9a130000);
        IoDiag_PrintFormattedRegVal("DRAM_Mode19RegD0             (0x%x)  = 0x%.8x\n", &pAmbaDDRC_Reg[DdrcId]->ModeReg,              PrintFunc);
        IoDiag_DdrcSetModeRegWait(DdrcId, 0xffffffff, 0x9a180000);
        IoDiag_PrintFormattedRegVal("DRAM_Mode24RegD0             (0x%x)  = 0x%.8x\n", &pAmbaDDRC_Reg[DdrcId]->ModeReg,              PrintFunc);
        if (pAmbaDDRC_Reg[DdrcId]->Config0.DuelDieEn == 1U) {
            IoDiag_DdrcSetModeRegWait(DdrcId, 0xffffffff, 0x9c010000);
            IoDiag_PrintFormattedRegVal("DRAM_Mode1RegD1              (0x%x)  = 0x%.8x\n", &pAmbaDDRC_Reg[DdrcId]->ModeReg,              PrintFunc);
            IoDiag_DdrcSetModeRegWait(DdrcId, 0xffffffff, 0x9c0b0000);
            IoDiag_PrintFormattedRegVal("DRAM_Mode11RegD1             (0x%x)  = 0x%.8x\n", &pAmbaDDRC_Reg[DdrcId]->ModeReg,              PrintFunc);
            IoDiag_DdrcSetModeRegWait(DdrcId, 0xffffffff, 0x9c0c0000);
            IoDiag_PrintFormattedRegVal("DRAM_Mode12RegD1             (0x%x)  = 0x%.8x\n", &pAmbaDDRC_Reg[DdrcId]->ModeReg,              PrintFunc);
            IoDiag_DdrcSetModeRegWait(DdrcId, 0xffffffff, 0x9c0e0000);
            IoDiag_PrintFormattedRegVal("DRAM_Mode14RegD1             (0x%x)  = 0x%.8x\n", &pAmbaDDRC_Reg[DdrcId]->ModeReg,              PrintFunc);
            IoDiag_DdrcSetModeRegWait(DdrcId, 0xffffffff, 0x9c0f0000);
            IoDiag_PrintFormattedRegVal("DRAM_Mode15RegD1             (0x%x)  = 0x%.8x\n", &pAmbaDDRC_Reg[DdrcId]->ModeReg,              PrintFunc);
            IoDiag_DdrcSetModeRegWait(DdrcId, 0xffffffff, 0x9c100000);
            IoDiag_PrintFormattedRegVal("DRAM_Mode16RegD1             (0x%x)  = 0x%.8x\n", &pAmbaDDRC_Reg[DdrcId]->ModeReg,              PrintFunc);
            IoDiag_DdrcSetModeRegWait(DdrcId, 0xffffffff, 0x9c130000);
            IoDiag_PrintFormattedRegVal("DRAM_Mode19RegD1             (0x%x)  = 0x%.8x\n", &pAmbaDDRC_Reg[DdrcId]->ModeReg,              PrintFunc);
            IoDiag_DdrcSetModeRegWait(DdrcId, 0xffffffff, 0x9c180000);
            IoDiag_PrintFormattedRegVal("DRAM_Mode24RegD1             (0x%x)  = 0x%.8x\n", &pAmbaDDRC_Reg[DdrcId]->ModeReg,              PrintFunc);
        }
    }
}


static void IoDiag_DdrSetDLL(UINT32 DdrcId, UINT32 BitMask, UINT32 DllByte, UINT32 DllValue, AMBA_SHELL_PRINT_f PrintFunc)
{
    UINT32 DllRegVal = 0, Mask, i;

    /* Get original value */
    if (DllByte == AMBA_DDRC_DLL_BYTE_0) {
        DllRegVal = AmbaCSL_DdrcGetD0Dll0(DdrcId);
    } else if (DllByte == AMBA_DDRC_DLL_BYTE_1) {
        DllRegVal = AmbaCSL_DdrcGetD0Dll1(DdrcId);
    } else if (DllByte == AMBA_DDRC_DLL_BYTE_2) {
        DllRegVal = AmbaCSL_DdrcGetD0Dll2(DdrcId);
    }

    /* Update field accroding to BitMask */
    DllValue = DllValue & 0xffU;
    for (i = 0; i < 4; i++) {
        Mask = 0xffU << (i * 8);
        if (BitMask & Mask) {
            DllRegVal &= (~Mask);
            DllRegVal |= (DllValue << (i * 8));
        }
    }

    /* Set new value */
    if (DllByte == AMBA_DDRC_DLL_BYTE_0) {
        AmbaCSL_DdrcSetD0Dll0(DdrcId, DllRegVal);
        AmbaCSL_DdrcSetD1Dll0(DdrcId, DllRegVal);
        DllRegVal = AmbaCSL_DdrcGetD0Dll0(DdrcId);
    } else if (DllByte == AMBA_DDRC_DLL_BYTE_1) {
        AmbaCSL_DdrcSetD0Dll1(DdrcId, DllRegVal);
        AmbaCSL_DdrcSetD1Dll1(DdrcId, DllRegVal);
        DllRegVal = AmbaCSL_DdrcGetD0Dll1(DdrcId);
    } else if (DllByte == AMBA_DDRC_DLL_BYTE_2) {
        AmbaCSL_DdrcSetD0Dll2(DdrcId, DllRegVal);
        AmbaCSL_DdrcSetD1Dll2(DdrcId, DllRegVal);
        DllRegVal = AmbaCSL_DdrcGetD0Dll2(DdrcId);
    }

    IoDiag_PrintFormattedRegValPerChan("[DDRC%d][DLL%d]Reg = 0x%08x\n", DdrcId, DllByte, DllRegVal, PrintFunc);
}

static UINT32 IoDiag_DdrcGetByteDly(UINT32 DdrcId, UINT32 ByteNum, UINT32 Index)
{
    if (ByteNum == 0U) {
        return AmbaCSL_DdrcGetByte0D0Dly(DdrcId, Index);
    } else if (ByteNum == 1U) {
        return AmbaCSL_DdrcGetByte1D0Dly(DdrcId, Index);
    } else if (ByteNum == 2U) {
        return AmbaCSL_DdrcGetByte2D0Dly(DdrcId, Index);
    } else if (ByteNum == 3U) {
        return AmbaCSL_DdrcGetByte3D0Dly(DdrcId, Index);
    }
    return 0;
}

static UINT32 IoDiag_DdrcGetDqVref(UINT32 DdrcId, UINT32 ByteNum)
{
    if (ByteNum == 0U) {
        return (AmbaCSL_DdrcGetRdVref0(DdrcId) >> DQ_VREF_POS1) & DQ_VREF_MASK;
    } else if (ByteNum == 1U) {
        return (AmbaCSL_DdrcGetRdVref0(DdrcId) >> DQ_VREF_POS2) & DQ_VREF_MASK;
    } else if (ByteNum == 2U) {
        return (AmbaCSL_DdrcGetRdVref1(DdrcId) >> DQ_VREF_POS1) & DQ_VREF_MASK;
    } else if (ByteNum == 3U) {
        return (AmbaCSL_DdrcGetRdVref1(DdrcId) >> DQ_VREF_POS2) & DQ_VREF_MASK;
    }
    return 0;
}

static UINT32 IoDiag_DdrcGetDqsVrefPerByte(UINT32 DdrcId, UINT32 ByteNum)
{
    if (ByteNum == 0U) {
        return (AmbaCSL_DdrcGetRdVref0(DdrcId) >> DQS_VREF_POS1) & DQS_VREF_MASK;
    } else if (ByteNum == 1U) {
        return (AmbaCSL_DdrcGetRdVref0(DdrcId) >> DQS_VREF_POS2) & DQS_VREF_MASK;
    } else if (ByteNum == 2U) {
        return (AmbaCSL_DdrcGetRdVref1(DdrcId) >> DQS_VREF_POS1) & DQS_VREF_MASK;
    } else if (ByteNum == 3U) {
        return (AmbaCSL_DdrcGetRdVref1(DdrcId) >> DQS_VREF_POS2) & DQS_VREF_MASK;
    }
    return 0;
}

static void IoDiag_DdrcSetByteDly(UINT32 DdrcId, UINT32 ByteNum, UINT32 Index, UINT32 Val)
{
    if (ByteNum == 0U) {
        AmbaCSL_DdrcSetByte0D0Dly(DdrcId, Index, Val);
        AmbaCSL_DdrcSetByte0D1Dly(DdrcId, Index, Val);
    } else if (ByteNum == 1U) {
        AmbaCSL_DdrcSetByte1D0Dly(DdrcId, Index, Val);
        AmbaCSL_DdrcSetByte1D1Dly(DdrcId, Index, Val);
    } else if (ByteNum == 2U) {
        AmbaCSL_DdrcSetByte2D0Dly(DdrcId, Index, Val);
        AmbaCSL_DdrcSetByte2D1Dly(DdrcId, Index, Val);
    } else if (ByteNum == 3U) {
        AmbaCSL_DdrcSetByte3D0Dly(DdrcId, Index, Val);
        AmbaCSL_DdrcSetByte3D1Dly(DdrcId, Index, Val);
    }
}

static void IoDiag_DdrcSetDqVref(UINT32 DdrcId, UINT32 ByteNum, UINT32 Val)
{
    UINT32 CurRegValue = 0U, NewRegValue = 0U;

    if (ByteNum == 0U) {
        CurRegValue = AmbaCSL_DdrcGetRdVref0(DdrcId);
        NewRegValue = CurRegValue & ~(DQ_VREF_MASK << DQ_VREF_POS1);
        NewRegValue = NewRegValue | (Val << DQ_VREF_POS1);
        AmbaCSL_DdrcSetRdVref0(DdrcId, NewRegValue);
    } else if (ByteNum == 1U) {
        CurRegValue = AmbaCSL_DdrcGetRdVref0(DdrcId);
        NewRegValue = CurRegValue & ~(DQ_VREF_MASK << DQ_VREF_POS2);
        NewRegValue = NewRegValue | (Val << DQ_VREF_POS2);
        AmbaCSL_DdrcSetRdVref0(DdrcId, NewRegValue);
    } else if (ByteNum == 2U) {
        CurRegValue = AmbaCSL_DdrcGetRdVref1(DdrcId);
        NewRegValue = CurRegValue & ~(DQ_VREF_MASK << DQ_VREF_POS1);
        NewRegValue = NewRegValue | (Val << DQ_VREF_POS1);
        AmbaCSL_DdrcSetRdVref1(DdrcId, NewRegValue);
    } else if (ByteNum == 3U) {
        CurRegValue = AmbaCSL_DdrcGetRdVref1(DdrcId);
        NewRegValue = CurRegValue & ~(DQ_VREF_MASK << DQ_VREF_POS2);
        NewRegValue = NewRegValue | (Val << DQ_VREF_POS2);
        AmbaCSL_DdrcSetRdVref1(DdrcId, NewRegValue);
    }
}

static void IoDiag_DdrcSetDqsVrefPerByte(UINT32 DdrcId, UINT32 ByteNum, UINT32 Val)
{
    UINT32 CurRegValue = 0U, NewRegValue = 0U;

    if (ByteNum == 0U) {
        CurRegValue = AmbaCSL_DdrcGetRdVref0(DdrcId);
        NewRegValue = CurRegValue & ~(DQS_VREF_MASK << DQS_VREF_POS1);
        NewRegValue = NewRegValue | (Val << DQS_VREF_POS1);
        AmbaCSL_DdrcSetRdVref0(DdrcId, NewRegValue);
    } else if (ByteNum == 1U) {
        CurRegValue = AmbaCSL_DdrcGetRdVref0(DdrcId);
        NewRegValue = CurRegValue & ~(DQS_VREF_MASK << DQS_VREF_POS2);
        NewRegValue = NewRegValue | (Val << DQS_VREF_POS2);
        AmbaCSL_DdrcSetRdVref0(DdrcId, NewRegValue);
    } else if (ByteNum == 2U) {
        CurRegValue = AmbaCSL_DdrcGetRdVref1(DdrcId);
        NewRegValue = CurRegValue & ~(DQS_VREF_MASK << DQS_VREF_POS1);
        NewRegValue = NewRegValue | (Val << DQS_VREF_POS1);
        AmbaCSL_DdrcSetRdVref1(DdrcId, NewRegValue);
    } else if (ByteNum == 3U) {
        CurRegValue = AmbaCSL_DdrcGetRdVref1(DdrcId);
        NewRegValue = CurRegValue & ~(DQS_VREF_MASK << DQS_VREF_POS2);
        NewRegValue = NewRegValue | (Val << DQS_VREF_POS2);
        AmbaCSL_DdrcSetRdVref1(DdrcId, NewRegValue);
    }
}

static UINT32 IoDiag_DdrGetLp4ReadDly(UINT32 DdrcId, UINT32 BitMask)
{
    UINT32 CurRegValue = 0, Coarse = 0, Fine = 0, CurDelay = 0, i, j;

    for (i = 0U; i < 4U; i++) {
        if ((0xffU << (i*8)) & BitMask) {
            CurRegValue = IoDiag_DdrcGetByteDly(DdrcId, i, 1U);
            Coarse = (CurRegValue >> READ_DELAY_POS_COARSE) & READ_DELAY_MASK;
            Fine = (CurRegValue >> READ_DELAY_POS_FINE) & READ_DELAY_MASK;
            /* Add fine of DQ[0~3] */
            CurRegValue = IoDiag_DdrcGetByteDly(DdrcId, i, 4U);
            for (j = 0U; j < 4; j++) {
                Fine += (CurRegValue >> (j*8)) & WRITE_DELAY_MASK;
            }
            /* Add fine of DQ[4~7] */
            CurRegValue = IoDiag_DdrcGetByteDly(DdrcId, i, 5U);
            for (j = 0U; j < 4; j++) {
                Fine += (CurRegValue >> (j*8)) & WRITE_DELAY_MASK;
            }
            /* Average fine of DM and DQ[0~7] */
            Fine /= 9;
            CurDelay = (Coarse * 12U) + Fine;
            return CurDelay;
        }
    }

    return 0;
}

static UINT32 IoDiag_DdrGetLp4Vref(UINT32 DdrcId, UINT32 BitMask)
{
    UINT32 i;

    for (i = 0U; i < 4U; i++) {
        if ((0xffU << (i*8)) & BitMask) {
            return IoDiag_DdrcGetDqVref(DdrcId, i);
        }
    }

    return 0;
}

static UINT32 IoDiag_DdrGetLp4MR14(UINT32 DdrcId, UINT32 BitMask)
{
    UINT32 CurRegValue = 0;

    IoDiag_DdrcSetModeRegWait(DdrcId, /*BitMask*/0xffffffff, 0x9a0e0000); // Since the controller supports both channel data in a read, we can use full mask
    CurRegValue = IoDiag_DdrcGetModeReg(DdrcId, BitMask) & LP4_MR14_MASK; /* bit[6:0] */
    if (CurRegValue > 64U) {
        CurRegValue = CurRegValue - 64U + 30U;  /* bit[6] is set, we should remove bit[6] and remap bit[5:0] to 31~80 */
    }

    return CurRegValue;
}

static UINT32 IoDiag_DdrGetLp4MR12(UINT32 DdrcId, UINT32 BitMask)
{
    UINT32 CurRegValue = 0;

    IoDiag_DdrcSetModeRegWait(DdrcId, /*BitMask*/0xffffffff, 0x9a0c0000); // Since the controller supports both channel data in a read, we can use full mask
    CurRegValue = IoDiag_DdrcGetModeReg(DdrcId, BitMask) & LP4_MR12_MASK; /* bit[6:0] */
    if (CurRegValue > 64U) {
        CurRegValue = CurRegValue - 64U + 30U;  /* bit[6] is set, we should remove bit[6] and remap bit[5:0] to 31~80 */
    }

    return CurRegValue;
}

static UINT32 IoDiag_DdrGetLp4WriteDly(UINT32 DdrcId, UINT32 BitMask)
{
    UINT32 CurRegValue = 0, Coarse = 0, Fine = 0, CurDelay = 0, i, j;

    for (i = 0U; i < 4U; i++) {
        if ((0xffU << (i*8)) & BitMask) {
            CurRegValue = IoDiag_DdrcGetByteDly(DdrcId, i, 1U);
            Coarse = (CurRegValue >> WRITE_DELAY_POS_COARSE) & WRITE_DELAY_MASK;
            Fine = (CurRegValue >> WRITE_DELAY_POS_FINE) & WRITE_DELAY_MASK;
            /* Add fine of DQ[0~3] */
            CurRegValue = IoDiag_DdrcGetByteDly(DdrcId, i, 2U);
            for (j = 0U; j < 4; j++) {
                Fine += (CurRegValue >> (j*8)) & WRITE_DELAY_MASK;
            }
            /* Add fine of DQ[4~7] */
            CurRegValue = IoDiag_DdrcGetByteDly(DdrcId, i, 3U);
            for (j = 0U; j < 4; j++) {
                Fine += (CurRegValue >> (j*8)) & WRITE_DELAY_MASK;
            }
            /* Average fine of DM and DQ[0~7] */
            Fine /= 9;
            CurDelay = (Coarse * 12U) + Fine;
            return CurDelay;
        }
    }

    return 0;
}

static UINT32 IoDiag_DdrGetDqsVref(UINT32 DdrcId, UINT32 BitMask)
{
    UINT32 i;

    for (i = 0U; i < 4U; i++) {
        if ((0xffU << (i*8)) & BitMask) {
            return IoDiag_DdrcGetDqsVrefPerByte(DdrcId, i);
        }
    }

    return 0;
}

static UINT32 IoDiag_DdrGetWckDly(UINT32 DdrcId, UINT32 BitMask)
{
    UINT32 CurRegValue = 0, Coarse = 0, Fine = 0, CurDelay = 0, i;

    for (i = 0U; i < 4U; i++) {
        if ((0xffU << (i*8)) & BitMask) {
            CurRegValue = IoDiag_DdrcGetByteDly(DdrcId, i, 0U);
            Coarse = (CurRegValue >> WCK_DLY_POS_COARSE) & WCK_DLY_MASK;
            Fine = (CurRegValue >> WCK_DLY_POS_FINE) & WCK_DLY_MASK;
            CurDelay = (Coarse * 12U) + Fine;
            return CurDelay;
        }
    }

    return 0;
}

static UINT32 IoDiag_DdrGetDqsWriteDly(UINT32 DdrcId, UINT32 BitMask)
{
    UINT32 CurRegValue = 0, Coarse = 0, Fine = 0, CurDelay = 0, i;

    for (i = 0U; i < 4U; i++) {
        if ((0xffU << (i*8)) & BitMask) {
            CurRegValue = IoDiag_DdrcGetByteDly(DdrcId, i, 0U);
            Coarse = (CurRegValue >> DQS_WRITE_DLY_POS_COARSE) & DQS_WRITE_DLY_MASK;
            Fine = (CurRegValue >> DQS_WRITE_DLY_POS_FINE) & DQS_WRITE_DLY_MASK;
            CurDelay = (Coarse * 12U) + Fine;
            return CurDelay;
        }
    }

    return 0;
}

static UINT32 IoDiag_DdrGetDqsGateDly(UINT32 DdrcId, UINT32 BitMask)
{
    UINT32 CurRegValue = 0, Coarse = 0, Fine = 0, CurDelay = 0, i;

    for (i = 0U; i < 4U; i++) {
        if ((0xffU << (i*8)) & BitMask) {
            CurRegValue = IoDiag_DdrcGetByteDly(DdrcId, i, 1U);
            Coarse = (CurRegValue >> DQS_GATE_DLY_POS_COARSE) & DQS_GATE_DLY_MASK;
            Fine = (CurRegValue >> DQS_GATE_DLY_POS_FINE) & DQS_GATE_DLY_MASK;
            CurDelay = (Coarse * 12U) + Fine;
            return CurDelay;
        }
    }

    return 0;
}

static UINT32 IoDiag_DdrGetCkDly(UINT32 DdrcId, UINT32 BitMask)
{
    UINT32 CurRegValue = 0, Coarse = 0, Fine = 0, CurDelay = 0;

    CurRegValue = AmbaCSL_DdrcGetCkDly(DdrcId);
    if (BitMask & 0xffffU) {
        Coarse = (CurRegValue >> CK_DLY_POSA_COARSE) & CK_DLY_MASK;
        Fine = (CurRegValue >> CK_DLY_POSA_FINE) & CK_DLY_MASK;
    } else if (BitMask & 0xffff0000U) {
        Coarse = (CurRegValue >> CK_DLY_POSB_COARSE) & CK_DLY_MASK;
        Fine = (CurRegValue >> CK_DLY_POSB_FINE) & CK_DLY_MASK;
    }
    CurDelay = (Coarse * 12U) + Fine;

    return CurDelay;
}

static UINT32 IoDiag_DdrGetCkeDly(UINT32 DdrcId, UINT32 BitMask)
{
    UINT32 CurRegValue = 0, CurRegValueFine = 0, Coarse = 0, Fine = 0, CurDelay = 0;

    CurRegValue = AmbaCSL_DdrcGetCkeDlyCoars(DdrcId);  // CKE coarse
    CurRegValueFine = AmbaCSL_DdrcGetCkeDlyFine(DdrcId);   // CKE fine
    if (BitMask & 0xffffU) {
        Coarse = (CurRegValue >> CKE_DLY_POSA_COARSE) & CKE_DLY_MASK;
        Fine = (CurRegValueFine >> CKE_DLY_POS0A_FINE) & CKE_DLY_MASK;
    } else if (BitMask & 0xffff0000U) {
        Coarse = (CurRegValue >> CKE_DLY_POSB_COARSE) & CKE_DLY_MASK;
        Fine = (CurRegValueFine >> CKE_DLY_POS0B_FINE) & CKE_DLY_MASK;
    }
    CurDelay = (Coarse * 12U) + Fine;

    return CurDelay;
}

static UINT32 IoDiag_DdrGetCaDly(UINT32 DdrcId, UINT32 BitMask)
{
    UINT32 CurRegValue = 0, CurRegValueFine = 0, Coarse = 0, Fine = 0, CurDelay = 0;

    CurRegValue = AmbaCSL_DdrcGetCaDlyCoars(DdrcId);                // CA coarse
    if (BitMask & 0xffffU) {
        CurRegValueFine = AmbaCSL_DdrcGetCaDlyFineChAD0(DdrcId, 0); // CA fine
        Coarse = (CurRegValue >> CA_DLY_POSA_COARSE) & CA_DLY_MASK;
        Fine = (CurRegValueFine >> CA_DLY_POS0_ADDR0_FINE) & CA_DLY_MASK;
    } else if (BitMask & 0xffff0000U) {
        CurRegValueFine = AmbaCSL_DdrcGetCaDlyFineChBD0(DdrcId, 0); // CA fine
        Coarse = (CurRegValue >> CA_DLY_POSB_COARSE) & CA_DLY_MASK;
        Fine = (CurRegValueFine >> CA_DLY_POS0_ADDR0_FINE) & CA_DLY_MASK;
    }
    CurDelay = (Coarse * 12U) + Fine;

    return CurDelay;
}

static UINT32 IoDiag_DdrGetLp5MR14(UINT32 DdrcId, UINT32 BitMask)
{
    UINT32 CurRegValue = 0;

    IoDiag_DdrcSetModeRegWait(DdrcId, /*BitMask*/0xffffffff, 0x9a0e0000); // Since the controller supports both channel data in a read, we can use full mask
    CurRegValue = IoDiag_DdrcGetModeReg(DdrcId, BitMask) & LP5_MR14_MASK; /* bit[6:0] */

    return CurRegValue;
}

static UINT32 IoDiag_DdrGetLp5MR15(UINT32 DdrcId, UINT32 BitMask)
{
    UINT32 CurRegValue = 0;

    IoDiag_DdrcSetModeRegWait(DdrcId, /*BitMask*/0xffffffff, 0x9a0f0000); // Since the controller supports both channel data in a read, we can use full mask
    CurRegValue = IoDiag_DdrcGetModeReg(DdrcId, BitMask) & LP5_MR15_MASK; /* bit[6:0] */

    return CurRegValue;
}

static UINT32 IoDiag_DdrGetLp5MR12(UINT32 DdrcId, UINT32 BitMask)
{
    UINT32 CurRegValue = 0;

    IoDiag_DdrcSetModeRegWait(DdrcId, /*BitMask*/0xffffffff, 0x9a0c0000); // Since the controller supports both channel data in a read, we can use full mask
    CurRegValue = IoDiag_DdrcGetModeReg(DdrcId, BitMask) & LP5_MR12_MASK; /* bit[6:0] */

    return CurRegValue;
}

static void IoDiag_DdrSetLp4ReadDly(UINT32 DdrcId, UINT32 BitMask, UINT32 DelayValue, AMBA_SHELL_PRINT_f PrintFunc)
{
    UINT32 NewRegValue = 0, NewRegValueFine = 0, Coarse = 0, Fine = 0, CurDelay = 0, i, Mask;

    Coarse = DelayValue / 12U;
    Fine = DelayValue % 12U;

    for (i = 0U; i < 4U; i++) {
        Mask = 0xffU << (i * 8);
        if (Mask & BitMask) {
            NewRegValue = IoDiag_DdrcGetByteDly(DdrcId, i, 1U);
            NewRegValue &= ~((READ_DELAY_MASK << READ_DELAY_POS_FINE) | (READ_DELAY_MASK << READ_DELAY_POS_COARSE));    // Clear [29:25], [19:15]
            NewRegValue |= (Fine << READ_DELAY_POS_FINE) | (Coarse << READ_DELAY_POS_COARSE);
            NewRegValueFine = (Fine << 24U) | (Fine << 16U) | (Fine << 8U) | (Fine << 0U);
            IoDiag_DdrcSetByteDly(DdrcId, i, 1U, NewRegValue);          // DQ Read coarse, DM Read fine
            IoDiag_DdrcSetByteDly(DdrcId, i, 4U, NewRegValueFine);      // DQ Read fine
            IoDiag_DdrcSetByteDly(DdrcId, i, 5U, NewRegValueFine);      // DQ Read fine

            CurDelay = IoDiag_DdrGetLp4ReadDly(DdrcId, Mask);
            IoDiag_PrintFormattedRegValPerChan("[DDRC%d][DqRead Delay%d]Delay1,4,5 = 0x%08x,", DdrcId, i, NewRegValue, PrintFunc);
            IoDiag_PrintFormattedRegValPerChan(" 0x%08x, 0x%08x (value = 0x%x)\n", NewRegValueFine, NewRegValueFine, CurDelay, PrintFunc);
        }
    }
}

static void IoDiag_DdrSetLp4Vref(UINT32 DdrcId, UINT32 BitMask, UINT32 Val, AMBA_SHELL_PRINT_f PrintFunc)
{
    UINT32 NewRegValue = 0, RegIndex = 0, i;

    for (i = 0U; i < 4U; i++) {
        if ((0xffU << (i*8)) & BitMask) {
            IoDiag_DdrcSetDqVref(DdrcId, i, Val);

            if (i < 2) {
                NewRegValue = AmbaCSL_DdrcGetRdVref0(DdrcId);
                RegIndex = 0;
            } else {
                NewRegValue = AmbaCSL_DdrcGetRdVref1(DdrcId);
                RegIndex = 1;
            }
            IoDiag_PrintFormattedRegValPerChan("[DDRC%d][DQ Vref%d]TrainDqVref%d =", DdrcId, i, RegIndex, PrintFunc);
            IoDiag_PrintFormattedRegValPerChan(" 0x%08x (value = 0x%x)\n", NewRegValue, IoDiag_DdrcGetDqVref(DdrcId, i), 0, PrintFunc);
        }
    }
}

static void IoDiag_DdrSetLp4MR14(UINT32 DdrcId, UINT32 BitMask, UINT32 Val, AMBA_SHELL_PRINT_f PrintFunc)
{
    UINT32 NewRegValue = 0;

    Val &= LP4_MR14_MASK;
    if (Val <= 50) {
        NewRegValue = 0x9f0e0000UL | Val;
    } else {
        NewRegValue = 0x9f0e0040UL | (Val - 30); /* Remap 51~80 to bit[5:0] and set bit[6] */
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
        NewRegValue = 0x9f0c0000UL | Val;
    } else {
        NewRegValue = 0x9f0c0040UL | (Val - 30); /* Remap 51~80 to bit[5:0] and set bit[6] */
    }
    IoDiag_DdrcSetModeRegWait(DdrcId, BitMask, NewRegValue);

    NewRegValue = IoDiag_DdrGetLp4MR12(DdrcId, BitMask);
    IoDiag_PrintFormattedRegValPerChan("[LPDDR4_%d][MR12 Vref]Lpddr4CaVref = 0x%08x (value = 0x%x)\n", DdrcId, AmbaCSL_DdrcGetModeReg(DdrcId), NewRegValue, PrintFunc);
}

static void IoDiag_DdrSetLp4WriteDly(UINT32 DdrcId, UINT32 BitMask, UINT32 DelayValue, AMBA_SHELL_PRINT_f PrintFunc)
{
    UINT32 NewRegValue = 0, NewRegValueFine = 0, Coarse = 0, Fine = 0, CurDelay = 0, i, Mask;

    Coarse = DelayValue / 12U;
    Fine = DelayValue % 12U;

    for (i = 0U; i < 4U; i++) {
        Mask = 0xffU << (i * 8);
        if (Mask & BitMask) {
            NewRegValue = IoDiag_DdrcGetByteDly(DdrcId, i, 1U);
            NewRegValue &= ~((WRITE_DELAY_MASK << WRITE_DELAY_POS_FINE) | (WRITE_DELAY_MASK << WRITE_DELAY_POS_COARSE));    // Clear [24:20], [14:10]
            NewRegValue |= (Fine << WRITE_DELAY_POS_FINE) | (Coarse << WRITE_DELAY_POS_COARSE);
            NewRegValueFine = (Fine << 24U) | (Fine << 16U) | (Fine << 8U) | (Fine << 0U);
            IoDiag_DdrcSetByteDly(DdrcId, i, 1U, NewRegValue);          // DQ Write coarse, DM Write fine
            IoDiag_DdrcSetByteDly(DdrcId, i, 2U, NewRegValueFine);      // DQ Write fine
            IoDiag_DdrcSetByteDly(DdrcId, i, 3U, NewRegValueFine);      // DQ Write fine

            CurDelay = IoDiag_DdrGetLp4WriteDly(DdrcId, Mask);
            IoDiag_PrintFormattedRegValPerChan("[DDRC%d][DqWrite Delay%d]Delay1,2,3 = 0x%08x,", DdrcId, i, NewRegValue, PrintFunc);
            IoDiag_PrintFormattedRegValPerChan(" 0x%08x, 0x%08x (value = 0x%x)\n", NewRegValueFine, NewRegValueFine, CurDelay, PrintFunc);
        }
    }
}

static void IoDiag_DdrSetDqsVref(UINT32 DdrcId, UINT32 BitMask, UINT32 Val, AMBA_SHELL_PRINT_f PrintFunc)
{
    UINT32 NewRegValue = 0, RegIndex = 0, i;

    for (i = 0U; i < 4U; i++) {
        if ((0xffU << (i*8)) & BitMask) {
            IoDiag_DdrcSetDqsVrefPerByte(DdrcId, i, Val);

            if (i < 2) {
                NewRegValue = AmbaCSL_DdrcGetRdVref0(DdrcId);
                RegIndex = 0;
            } else {
                NewRegValue = AmbaCSL_DdrcGetRdVref1(DdrcId);
                RegIndex = 1;
            }
            IoDiag_PrintFormattedRegValPerChan("[DDRC%d][DQS Vref%d]TrainDqVref%d =", DdrcId, i, RegIndex, PrintFunc);
            IoDiag_PrintFormattedRegValPerChan(" 0x%08x (value = 0x%x)\n", NewRegValue, IoDiag_DdrcGetDqsVrefPerByte(DdrcId, i), 0, PrintFunc);
        }
    }
}

static void IoDiag_DdrSetWckDly(UINT32 DdrcId, UINT32 BitMask, UINT32 DelayValue, AMBA_SHELL_PRINT_f PrintFunc)
{
    UINT32 NewRegValue = 0, Coarse = 0, Fine = 0, CurDelay = 0, i, Mask;

    Coarse = DelayValue / 12U;
    Fine = DelayValue % 12U;

    for (i = 0U; i < 4U; i++) {
        Mask = 0xffU << (i * 8);
        if (Mask & BitMask) {
            NewRegValue = IoDiag_DdrcGetByteDly(DdrcId, i, 0U);
            NewRegValue &= ~((WCK_DLY_MASK << WCK_DLY_POS_COARSE) | (WCK_DLY_MASK << WCK_DLY_POS_FINE));
            NewRegValue |= (Coarse << WCK_DLY_POS_COARSE) | (Fine << WCK_DLY_POS_FINE);
            IoDiag_DdrcSetByteDly(DdrcId, i, 0U, NewRegValue);      // WCK coarse, WCK fine

            CurDelay = IoDiag_DdrGetWckDly(DdrcId, Mask);
            IoDiag_PrintFormattedRegValPerChan("[DDRC%d][Wck Delay%d]Delay0 = 0x%08x,", DdrcId, i, NewRegValue, PrintFunc);
            IoDiag_PrintFormattedRegValPerChan(" (value = 0x%x)\n", CurDelay, 0, 0, PrintFunc);
        }
    }
}

static void IoDiag_DdrSetDqsWriteDly(UINT32 DdrcId, UINT32 BitMask, UINT32 DelayValue, AMBA_SHELL_PRINT_f PrintFunc)
{
    UINT32 NewRegValue = 0, Coarse = 0, Fine = 0, CurDelay = 0, i, Mask;

    Coarse = DelayValue / 12U;
    Fine = DelayValue % 12U;

    for (i = 0U; i < 4U; i++) {
        Mask = 0xffU << (i * 8);
        if (Mask & BitMask) {
            NewRegValue = IoDiag_DdrcGetByteDly(DdrcId, i, 0U);
            NewRegValue &= ~((DQS_WRITE_DLY_MASK << DQS_WRITE_DLY_POS_COARSE) | (DQS_WRITE_DLY_MASK << DQS_WRITE_DLY_POS_FINE));
            NewRegValue |= (Coarse << DQS_WRITE_DLY_POS_COARSE) | (Fine << DQS_WRITE_DLY_POS_FINE);
            IoDiag_DdrcSetByteDly(DdrcId, i, 0U, NewRegValue);      // DQS Write coarse, DQS Write fine

            CurDelay = IoDiag_DdrGetDqsWriteDly(DdrcId, Mask);
            IoDiag_PrintFormattedRegValPerChan("[DDRC%d][DqsWrite Delay%d]Delay0 = 0x%08x,", DdrcId, i, NewRegValue, PrintFunc);
            IoDiag_PrintFormattedRegValPerChan(" (value = 0x%x)\n", CurDelay, 0, 0, PrintFunc);
        }
    }
}

static void IoDiag_DdrSetDqsGateDly(UINT32 DdrcId, UINT32 BitMask, UINT32 DelayValue, AMBA_SHELL_PRINT_f PrintFunc)
{
    UINT32 NewRegValue = 0, Coarse = 0, Fine = 0, CurDelay = 0, i, Mask;

    Coarse = DelayValue / 12U;
    Fine = DelayValue % 12U;

    for (i = 0U; i < 4U; i++) {
        Mask = 0xffU << (i * 8);
        if (Mask & BitMask) {
            NewRegValue = IoDiag_DdrcGetByteDly(DdrcId, i, 1U);
            NewRegValue &= ~((DQS_GATE_DLY_MASK << DQS_GATE_DLY_POS_COARSE) | (DQS_GATE_DLY_MASK << DQS_GATE_DLY_POS_FINE));
            NewRegValue |= (Coarse << DQS_GATE_DLY_POS_COARSE) | (Fine << DQS_GATE_DLY_POS_FINE);
            IoDiag_DdrcSetByteDly(DdrcId, i, 1U, NewRegValue);      // DQS Gate coarse, DQS Gate fine

            CurDelay = IoDiag_DdrGetDqsGateDly(DdrcId, Mask);
            IoDiag_PrintFormattedRegValPerChan("[DDRC%d][DqsGate Delay%d]Delay1 = 0x%08x,", DdrcId, i, NewRegValue, PrintFunc);
            IoDiag_PrintFormattedRegValPerChan(" (value = 0x%x)\n", CurDelay, 0, 0, PrintFunc);
        }
    }
}

static void IoDiag_DdrSetCkDly(UINT32 DdrcId, UINT32 BitMask, UINT32 DelayValue, AMBA_SHELL_PRINT_f PrintFunc)
{
    UINT32 NewRegValue = 0, Coarse = 0, Fine = 0, CurDelay = 0;

    Coarse = DelayValue / 12U;
    Fine = DelayValue % 12U;

    NewRegValue = AmbaCSL_DdrcGetCkDly(DdrcId);
    if (BitMask & 0xffffU) {
        NewRegValue &= ~0xffffU;
        NewRegValue |= (Coarse << CK_DLY_POSA_COARSE) | (Fine << CK_DLY_POSA_FINE);
    }
    if (BitMask & 0xffff0000U) {
        NewRegValue &= ~0xffff0000U;
        NewRegValue |= (Coarse << CK_DLY_POSB_COARSE) | (Fine << CK_DLY_POSB_FINE);
    }
    AmbaCSL_DdrcSetCkDly(DdrcId, NewRegValue);

    CurDelay = IoDiag_DdrGetCkDly(DdrcId, BitMask);
    IoDiag_PrintFormattedRegValPerChan("[DDRC%d][Ck Dly]Reg = 0x%08x (value = 0x%x)\n", DdrcId, NewRegValue, CurDelay, PrintFunc);
}

static void IoDiag_DdrSetCkeDly(UINT32 DdrcId, UINT32 BitMask, UINT32 DelayValue, AMBA_SHELL_PRINT_f PrintFunc)
{
    UINT32 NewRegValue = 0, NewRegValueFine = 0, Coarse = 0, Fine = 0, CurDelay = 0;

    Coarse = DelayValue / 12U;
    Fine = DelayValue % 12U;

    NewRegValue = AmbaCSL_DdrcGetCkeDlyCoars(DdrcId);
    NewRegValueFine = AmbaCSL_DdrcGetCkeDlyFine(DdrcId);
    if (BitMask & 0xffffU) {
        NewRegValue &= ~(CKE_DLY_MASK << CKE_DLY_POSA_COARSE);
        NewRegValue |= (Coarse << CKE_DLY_POSA_COARSE);
        NewRegValueFine &= ~((CKE_DLY_MASK << CKE_DLY_POS0A_FINE) | (CKE_DLY_MASK << CKE_DLY_POS1A_FINE));
        NewRegValueFine |= (Fine << CKE_DLY_POS0A_FINE) | (Fine << CKE_DLY_POS1A_FINE);
    }
    if (BitMask & 0xffff0000U) {
        NewRegValue &= ~(CKE_DLY_MASK << CKE_DLY_POSB_COARSE);
        NewRegValue |= (Coarse << CKE_DLY_POSB_COARSE);
        NewRegValueFine &= ~((CKE_DLY_MASK << CKE_DLY_POS0B_FINE) | (CKE_DLY_MASK << CKE_DLY_POS1B_FINE));
        NewRegValueFine |= (Fine << CKE_DLY_POS0B_FINE) | (Fine << CKE_DLY_POS1B_FINE);
    }
    AmbaCSL_DdrcSetCkeDlyCoars(DdrcId, NewRegValue);
    AmbaCSL_DdrcSetCkeDlyFine(DdrcId, NewRegValueFine);

    CurDelay = IoDiag_DdrGetCkeDly(DdrcId, BitMask);
    IoDiag_PrintFormattedRegValPerChan("[DDRC%d][Cke Dly]Coarse = 0x%08x, Fine = 0x%08x", DdrcId, NewRegValue, NewRegValueFine, PrintFunc);
    IoDiag_PrintFormattedRegValPerChan(" (value = 0x%x)\n", CurDelay, 0, 0, PrintFunc);
}

static void IoDiag_DdrSetCaDly(UINT32 DdrcId, UINT32 BitMask, UINT32 DelayValue, AMBA_SHELL_PRINT_f PrintFunc)
{
    UINT32 NewRegValue = 0, NewRegValueFine = 0, NewRegValueFine2 = 0, Coarse = 0, Fine = 0, CurDelay = 0;

    Coarse = DelayValue / 12U;
    Fine = DelayValue % 12U;

    NewRegValue = AmbaCSL_DdrcGetCaDlyCoars(DdrcId);
    NewRegValueFine = (Fine << CA_DLY_POS0_ADDR0_FINE) | (Fine << CA_DLY_POS0_ADDR1_FINE) |
                      (Fine << CA_DLY_POS0_ADDR2_FINE) | (Fine << CA_DLY_POS0_ADDR3_FINE);
    NewRegValueFine2 = (Fine << CA_DLY_POS0_ADDR4_FINE) | (Fine << CA_DLY_POS0_ADDR5_FINE) |
                       (Fine << CA_DLY_POS0_ADDR6_FINE);
    if (BitMask & 0xffffU) {
        NewRegValue &= ~(CA_DLY_MASK << CA_DLY_POSA_COARSE);
        NewRegValue |= (Coarse << CA_DLY_POSA_COARSE);
        AmbaCSL_DdrcSetCaDlyFineChAD0(DdrcId, 0, NewRegValueFine);
        AmbaCSL_DdrcSetCaDlyFineChAD1(DdrcId, 0, NewRegValueFine);
        AmbaCSL_DdrcSetCaDlyFineChAD0(DdrcId, 1, NewRegValueFine2);
        AmbaCSL_DdrcSetCaDlyFineChAD1(DdrcId, 1, NewRegValueFine2);
    }
    if (BitMask & 0xffff0000U) {
        NewRegValue &= ~(CA_DLY_MASK << CA_DLY_POSB_COARSE);
        NewRegValue |= (Coarse << CA_DLY_POSB_COARSE);
        AmbaCSL_DdrcSetCaDlyFineChBD0(DdrcId, 0, NewRegValueFine);
        AmbaCSL_DdrcSetCaDlyFineChBD1(DdrcId, 0, NewRegValueFine);
        AmbaCSL_DdrcSetCaDlyFineChBD0(DdrcId, 1, NewRegValueFine2);
        AmbaCSL_DdrcSetCaDlyFineChBD1(DdrcId, 1, NewRegValueFine2);
    }
    AmbaCSL_DdrcSetCaDlyCoars(DdrcId, NewRegValue);

    CurDelay = IoDiag_DdrGetCaDly(DdrcId, BitMask);
    IoDiag_PrintFormattedRegValPerChan("[DDRC%d][Ca Dly]Coarse = 0x%08x, Fine0,1 = 0x%08x", DdrcId, NewRegValue, NewRegValueFine, PrintFunc);
    IoDiag_PrintFormattedRegValPerChan(",0x%08x (value = 0x%x)\n", NewRegValueFine2, CurDelay, 0, PrintFunc);
}

static void IoDiag_DdrSetLp5MR14(UINT32 DdrcId, UINT32 BitMask, UINT32 Val, AMBA_SHELL_PRINT_f PrintFunc)
{
    UINT32 NewRegValue = 0;

    Val &= LP5_MR14_MASK;
    NewRegValue = 0x9f0e0000UL | Val;
    IoDiag_DdrcSetModeRegWait(DdrcId, BitMask, NewRegValue);

    NewRegValue = IoDiag_DdrGetLp5MR14(DdrcId, BitMask);
    IoDiag_PrintFormattedRegValPerChan("[LPDDR5_%d][MR14 Vref]Dq[7:0] = 0x%08x (value = 0x%x)\n", DdrcId, AmbaCSL_DdrcGetModeReg(DdrcId), NewRegValue, PrintFunc);
}

static void IoDiag_DdrSetLp5MR15(UINT32 DdrcId, UINT32 BitMask, UINT32 Val, AMBA_SHELL_PRINT_f PrintFunc)
{
    UINT32 NewRegValue = 0;

    Val &= LP5_MR15_MASK;
    NewRegValue = 0x9f0f0000UL | Val;
    IoDiag_DdrcSetModeRegWait(DdrcId, BitMask, NewRegValue);

    NewRegValue = IoDiag_DdrGetLp5MR15(DdrcId, BitMask);
    IoDiag_PrintFormattedRegValPerChan("[LPDDR5_%d][MR15 Vref]Dq[15:8] = 0x%08x (value = 0x%x)\n", DdrcId, AmbaCSL_DdrcGetModeReg(DdrcId), NewRegValue, PrintFunc);
}

static void IoDiag_DdrSetLp5MR12(UINT32 DdrcId, UINT32 BitMask, UINT32 Val, AMBA_SHELL_PRINT_f PrintFunc)
{
    UINT32 NewRegValue = 0;

    Val &= LP5_MR12_MASK;
    NewRegValue = 0x9f0c0000UL | Val;
    IoDiag_DdrcSetModeRegWait(DdrcId, BitMask, NewRegValue);

    NewRegValue = IoDiag_DdrGetLp5MR12(DdrcId, BitMask);
    IoDiag_PrintFormattedRegValPerChan("[LPDDR5_%d][MR12 Vref]Ca = 0x%08x (value = 0x%x)\n", DdrcId, AmbaCSL_DdrcGetModeReg(DdrcId), NewRegValue, PrintFunc);
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
    UINT32 DllByte = 0xffffffffUL, DllValue = 0xffffffffUL;
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

        DllVfine[0] = (pAmbaDDRC_Reg[DdrcId]->DllStatus0 >> 1U) & 0x1fU;
        DllVfine[1] = (pAmbaDDRC_Reg[DdrcId]->DllStatus0 >> 17U) & 0x1fU;
        DllVfine[2] = (pAmbaDDRC_Reg[DdrcId]->DllStatus1 >> 1U) & 0x1fU;
        DllVfine[3] = (pAmbaDDRC_Reg[DdrcId]->DllStatus1 >> 17U) & 0x1fU;
        IoDiag_PrintFormattedRegValPerChan("[DDRC%d]Vfine: 0x%x,0x%x,", DdrcId, DllVfine[0], DllVfine[1], PrintFunc);
        IoDiag_PrintFormattedRegValPerChan("0x%x,0x%x\n", DllVfine[2], DllVfine[3], 0, PrintFunc);
        IoDiag_PrintFormattedRegValPerChan("[DDRC%d]DLL Lock Searching Range: 0x%x - 0x%x\n", DdrcId, (DllVfine[0] | 0x20U), (31U - DllVfine[0]), PrintFunc);

        if (AmbaUtility_StringCompare(pArgVector[3], "sync", 4U) == 0) {
            DllByte = AMBA_DDRC_DLL_BYTE_1;
            DllValue = AmbaCSL_DdrcGetD0Dll1(DdrcId) & 0xFFU;
        }
        if (AmbaUtility_StringCompare(pArgVector[3], "read", 4U) == 0) {
            DllByte = AMBA_DDRC_DLL_BYTE_0;
            DllValue = AmbaCSL_DdrcGetD0Dll0(DdrcId) & 0xFFU;
        }
        if (AmbaUtility_StringCompare(pArgVector[3], "write", 5U) == 0) {
            DllByte = AMBA_DDRC_DLL_BYTE_2;
            DllValue = AmbaCSL_DdrcGetD0Dll2(DdrcId) & 0xFFU;
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
#define READ_DELAY_MAX         384
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
#define WRITE_DELAY_MAX         384
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

static void IoDiag_DdrDqsVrefShmoo(UINT32 ArgCount, char * const *pArgVector, AMBA_SHELL_PRINT_f PrintFunc)
{
#define DQS_VREF_MAX    64
#define DQS_VREF_MIN    0
    UINT32 DdrcId = 0U, TimeInterval = 0U, BitMask = 0U, CurValue = 0U;
    INT32 i = 0, Step = 0;

    if (IoDiag_DdrCheckDdrcParameter(pArgVector[2], &DdrcId, PrintFunc) == 0) {
        return; // Invalid parameter
    }
    if (IoDiag_DdrCheckRemainParameter(ArgCount, pArgVector, 3U, &Step, &TimeInterval, &BitMask) == 0) {
        return; // Invalid parameters
    }

    CurValue = IoDiag_DdrGetDqsVref(DdrcId, BitMask);

    IoDiag_DdrShmooItemStart(TimeInterval);
    for (i = CurValue; (i >= DQS_VREF_MIN) && (i < DQS_VREF_MAX); i += Step) {
        IoDiag_DdrSetDqsVref(DdrcId, BitMask, i, PrintFunc);
        IoDiag_DdrShmooItemWait(TimeInterval);
    }
    IoDiag_DdrShmooItemStop(PrintFunc);
}

static void IoDiag_DdrWckDlyShmoo(UINT32 ArgCount, char * const *pArgVector, AMBA_SHELL_PRINT_f PrintFunc)
{
#define WCK_DLY_MAX                 384
#define WCK_DLY_MIN                 0
    UINT32 DdrcId = 0U, TimeInterval = 0U, BitMask = 0U, CurValue = 0U;
    INT32 i = 0, Step = 0;

    if (IoDiag_DdrCheckDdrcParameter(pArgVector[2], &DdrcId, PrintFunc) == 0) {
        return; // Invalid parameter
    }
    if (IoDiag_DdrCheckRemainParameter(ArgCount, pArgVector, 3U, &Step, &TimeInterval, &BitMask) == 0) {
        return; // Invalid parameters
    }

    CurValue = IoDiag_DdrGetWckDly(DdrcId, BitMask);  // WCK coarse, fine

    IoDiag_DdrShmooItemStart(TimeInterval);
    for (i = CurValue; (i >= WCK_DLY_MIN) && (i < WCK_DLY_MAX); i += Step) {
        IoDiag_DdrSetWckDly(DdrcId, BitMask, i, PrintFunc);
        IoDiag_DdrShmooItemWait(TimeInterval);
    }
    IoDiag_DdrShmooItemStop(PrintFunc);
}

static void IoDiag_DdrDqsWriteDlyShmoo(UINT32 ArgCount, char * const *pArgVector, AMBA_SHELL_PRINT_f PrintFunc)
{
#define DQS_WRITE_DLY_MAX           384
#define DQS_WRITE_DLY_MIN           0
    UINT32 DdrcId = 0U, TimeInterval = 0U, BitMask = 0U, CurValue = 0U;
    INT32 i = 0, Step = 0;

    if (IoDiag_DdrCheckDdrcParameter(pArgVector[2], &DdrcId, PrintFunc) == 0) {
        return; // Invalid parameter
    }
    if (IoDiag_DdrCheckRemainParameter(ArgCount, pArgVector, 3U, &Step, &TimeInterval, &BitMask) == 0) {
        return; // Invalid parameters
    }

    CurValue = IoDiag_DdrGetDqsWriteDly(DdrcId, BitMask);  // DQS Write coarse, fine

    IoDiag_DdrShmooItemStart(TimeInterval);
    for (i = CurValue; (i >= DQS_WRITE_DLY_MIN) && (i < DQS_WRITE_DLY_MAX); i += Step) {
        IoDiag_DdrSetDqsWriteDly(DdrcId, BitMask, i, PrintFunc);
        IoDiag_DdrShmooItemWait(TimeInterval);
    }
    IoDiag_DdrShmooItemStop(PrintFunc);
}

static void IoDiag_DdrDqsGateDlyShmoo(UINT32 ArgCount, char * const *pArgVector, AMBA_SHELL_PRINT_f PrintFunc)
{
#define DQS_GATE_DLY_MAX            384
#define DQS_GATE_DLY_MIN            0
    UINT32 DdrcId = 0U, TimeInterval = 0U, BitMask = 0U, CurValue = 0U;
    INT32 i = 0, Step = 0;

    if (IoDiag_DdrCheckDdrcParameter(pArgVector[2], &DdrcId, PrintFunc) == 0) {
        return; // Invalid parameter
    }
    if (IoDiag_DdrCheckRemainParameter(ArgCount, pArgVector, 3U, &Step, &TimeInterval, &BitMask) == 0) {
        return; // Invalid parameters
    }

    CurValue = IoDiag_DdrGetDqsGateDly(DdrcId, BitMask);  // DQS Gate coarse, fine

    IoDiag_DdrShmooItemStart(TimeInterval);
    for (i = CurValue; (i >= DQS_GATE_DLY_MIN) && (i < DQS_GATE_DLY_MAX); i += Step) {
        IoDiag_DdrSetDqsGateDly(DdrcId, BitMask, i, PrintFunc);
        IoDiag_DdrShmooItemWait(TimeInterval);
    }
    IoDiag_DdrShmooItemStop(PrintFunc);
}

static void IoDiag_DdrCkDlyShmoo(UINT32 ArgCount, char * const *pArgVector, AMBA_SHELL_PRINT_f PrintFunc)
{
#define CK_DLY_MAX                  384
#define CK_DLY_MIN                  0
    UINT32 DdrcId = 0U, TimeInterval = 0U, BitMask = 0U, CurValue = 0U;
    INT32 i = 0, Step = 0;

    if (IoDiag_DdrCheckDdrcParameter(pArgVector[2], &DdrcId, PrintFunc) == 0) {
        return; // Invalid parameter
    }
    if (IoDiag_DdrCheckRemainParameter(ArgCount, pArgVector, 3U, &Step, &TimeInterval, &BitMask) == 0) {
        return; // Invalid parameters
    }

    CurValue = IoDiag_DdrGetCkDly(DdrcId, BitMask);  // CK coarse, fine

    IoDiag_DdrShmooItemStart(TimeInterval);
    for (i = CurValue; (i >= CK_DLY_MIN) && (i < CK_DLY_MAX); i += Step) {
        IoDiag_DdrSetCkDly(DdrcId, BitMask, i, PrintFunc);
        IoDiag_DdrShmooItemWait(TimeInterval);
    }
    IoDiag_DdrShmooItemStop(PrintFunc);
}

static void IoDiag_DdrCkeDlyShmoo(UINT32 ArgCount, char * const *pArgVector, AMBA_SHELL_PRINT_f PrintFunc)
{
#define CKE_DLY_MAX                 384
#define CKE_DLY_MIN                 0
    UINT32 DdrcId = 0U, TimeInterval = 0U, BitMask = 0U, CurValue = 0U;
    INT32 i = 0, Step = 0;

    if (IoDiag_DdrCheckDdrcParameter(pArgVector[2], &DdrcId, PrintFunc) == 0) {
        return; // Invalid parameter
    }
    if (IoDiag_DdrCheckRemainParameter(ArgCount, pArgVector, 3U, &Step, &TimeInterval, &BitMask) == 0) {
        return; // Invalid parameters
    }

    CurValue = IoDiag_DdrGetCkeDly(DdrcId, BitMask);  // CKE coarse, fine

    IoDiag_DdrShmooItemStart(TimeInterval);
    for (i = CurValue; (i >= CKE_DLY_MIN) && (i < CKE_DLY_MAX); i += Step) {
        IoDiag_DdrSetCkeDly(DdrcId, BitMask, i, PrintFunc);
        IoDiag_DdrShmooItemWait(TimeInterval);
    }
    IoDiag_DdrShmooItemStop(PrintFunc);
}

static void IoDiag_DdrCaDlyShmoo(UINT32 ArgCount, char * const *pArgVector, AMBA_SHELL_PRINT_f PrintFunc)
{
#define CA_DLY_MAX                  384
#define CA_DLY_MIN                  0
    UINT32 DdrcId = 0U, TimeInterval = 0U, BitMask = 0U, CurValue = 0U;
    INT32 i = 0, Step = 0;

    if (IoDiag_DdrCheckDdrcParameter(pArgVector[2], &DdrcId, PrintFunc) == 0) {
        return; // Invalid parameter
    }
    if (IoDiag_DdrCheckRemainParameter(ArgCount, pArgVector, 3U, &Step, &TimeInterval, &BitMask) == 0) {
        return; // Invalid parameters
    }

    CurValue = IoDiag_DdrGetCaDly(DdrcId, BitMask);  // CA coarse, fine

    IoDiag_DdrShmooItemStart(TimeInterval);
    for (i = CurValue; (i >= CA_DLY_MIN) && (i < CA_DLY_MAX); i += Step) {
        IoDiag_DdrSetCaDly(DdrcId, BitMask, i, PrintFunc);
        IoDiag_DdrShmooItemWait(TimeInterval);
    }
    IoDiag_DdrShmooItemStop(PrintFunc);
}

static void IoDiag_DdrLp5Mr14Shmoo(UINT32 ArgCount, char * const *pArgVector, AMBA_SHELL_PRINT_f PrintFunc)
{
#define LP5_MR14_MAX                    128
#define LP5_MR14_MIN                    0
    UINT32 DdrcId = 0U, TimeInterval = 0U, BitMask = 0U, CurValue = 0U;
    INT32 i = 0, Step = 0;

    if (IoDiag_DdrCheckDdrcParameter(pArgVector[2], &DdrcId, PrintFunc) == 0) {
        return; // Invalid parameter
    }
    if (IoDiag_DdrCheckRemainParameter(ArgCount, pArgVector, 3U, &Step, &TimeInterval, &BitMask) == 0) {
        return; // Invalid parameters
    }

    CurValue = IoDiag_DdrGetLp5MR14(DdrcId, BitMask);

    IoDiag_DdrShmooItemStart(TimeInterval);
    for (i = CurValue; (i >= LP5_MR14_MIN) && (i < LP5_MR14_MAX); i += Step) {
        IoDiag_DdrSetLp5MR14(DdrcId, BitMask, i, PrintFunc);
        IoDiag_DdrShmooItemWait(TimeInterval);
    }
    IoDiag_DdrShmooItemStop(PrintFunc);
}

static void IoDiag_DdrLp5Mr15Shmoo(UINT32 ArgCount, char * const *pArgVector, AMBA_SHELL_PRINT_f PrintFunc)
{
#define LP5_MR15_MAX                    128
#define LP5_MR15_MIN                    0
    UINT32 DdrcId = 0U, TimeInterval = 0U, BitMask = 0U, CurValue = 0U;
    INT32 i = 0, Step = 0;

    if (IoDiag_DdrCheckDdrcParameter(pArgVector[2], &DdrcId, PrintFunc) == 0) {
        return; // Invalid parameter
    }
    if (IoDiag_DdrCheckRemainParameter(ArgCount, pArgVector, 3U, &Step, &TimeInterval, &BitMask) == 0) {
        return; // Invalid parameters
    }

    CurValue = IoDiag_DdrGetLp5MR15(DdrcId, BitMask);

    IoDiag_DdrShmooItemStart(TimeInterval);
    for (i = CurValue; (i >= LP5_MR15_MIN) && (i < LP5_MR15_MAX); i += Step) {
        IoDiag_DdrSetLp5MR15(DdrcId, BitMask, i, PrintFunc);
        IoDiag_DdrShmooItemWait(TimeInterval);
    }
    IoDiag_DdrShmooItemStop(PrintFunc);
}

static void IoDiag_DdrLp5Mr12Shmoo(UINT32 ArgCount, char * const *pArgVector, AMBA_SHELL_PRINT_f PrintFunc)
{
#define LP5_MR12_MAX                    128
#define LP5_MR12_MIN                    0
    UINT32 DdrcId = AMBA_DDRC0, TimeInterval = DEFAULT_TIMER_INTERVAL, BitMask = 0U, CurValue = 0;
    INT32 i = 0, Step = 0;

    if (IoDiag_DdrCheckDdrcParameter(pArgVector[2], &DdrcId, PrintFunc) == 0) {
        return; // Invalid parameter
    }
    if (IoDiag_DdrCheckRemainParameter(ArgCount, pArgVector, 3U, &Step, &TimeInterval, &BitMask) == 0) {
        return; // Invalid parameters
    }

    CurValue = IoDiag_DdrGetLp5MR12(DdrcId, BitMask);

    IoDiag_DdrShmooItemStart(TimeInterval);
    for (i = CurValue; (i >= LP5_MR12_MIN) && (i < LP5_MR12_MAX); i += Step) {
        IoDiag_DdrSetLp5MR12(DdrcId, BitMask, i, PrintFunc);
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
    } else if (AmbaUtility_StringCompare(pArgVector[1], "dqsvref", 7U) == 0) {
        IoDiag_DdrDqsVrefShmoo(ArgCount, pArgVector, PrintFunc);
    } else if (AmbaUtility_StringCompare(pArgVector[1], "wckdly", 6U) == 0) {
        IoDiag_DdrWckDlyShmoo(ArgCount, pArgVector, PrintFunc);
    } else if (AmbaUtility_StringCompare(pArgVector[1], "dqswdly", 7U) == 0) {
        IoDiag_DdrDqsWriteDlyShmoo(ArgCount, pArgVector, PrintFunc);
    } else if (AmbaUtility_StringCompare(pArgVector[1], "dqsgdly", 7U) == 0) {
        IoDiag_DdrDqsGateDlyShmoo(ArgCount, pArgVector, PrintFunc);
    } else if (AmbaUtility_StringCompare(pArgVector[1], "ckdly", 5U) == 0) {
        IoDiag_DdrCkDlyShmoo(ArgCount, pArgVector, PrintFunc);
    } else if (AmbaUtility_StringCompare(pArgVector[1], "ckedly", 6U) == 0) {
        IoDiag_DdrCkeDlyShmoo(ArgCount, pArgVector, PrintFunc);
    } else if (AmbaUtility_StringCompare(pArgVector[1], "cadly", 5U) == 0) {
        IoDiag_DdrCaDlyShmoo(ArgCount, pArgVector, PrintFunc);
    } else if (AmbaUtility_StringCompare(pArgVector[1], "lp5mr14", 7U) == 0) {
        IoDiag_DdrLp5Mr14Shmoo(ArgCount, pArgVector, PrintFunc);
    } else if (AmbaUtility_StringCompare(pArgVector[1], "lp5mr15", 7U) == 0) {
        IoDiag_DdrLp5Mr15Shmoo(ArgCount, pArgVector, PrintFunc);
    } else if (AmbaUtility_StringCompare(pArgVector[1], "lp5mr12", 7U) == 0) {
        IoDiag_DdrLp5Mr12Shmoo(ArgCount, pArgVector, PrintFunc);
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
#if defined(CONFIG_DRAM_TYPE_LPDDR5) && defined(CONFIG_BUILD_COMSVC_DRAMSHMOO)
        } else if (AmbaUtility_StringCompare(pArgVector[1], "adjust", 6U) == 0) {
            if (AmbaUtility_StringCompare(pArgVector[2], "start", 5U) == 0) {
                AMBA_DRAM_ADJUST_CONFIG_s Config;
                UINT32 Duration = 10000U;

                if (ArgCount > 3) {
                    (void)AmbaUtility_StringToUInt32(pArgVector[3], &Duration);
                }
                AmbaDramAdjust_Init();
                Config.Duration = Duration;
                Config.CoreMask = 1;
                Config.Priority = AMBA_KAL_TASK_LOWEST_PRIORITY - 10;
                (void)AmbaDramAdjust_Start(&Config);
            } else if (AmbaUtility_StringCompare(pArgVector[2], "stop", 4U) == 0) {
                AmbaDramAdjust_Stop();
            } else if (AmbaUtility_StringCompare(pArgVector[2], "show", 4U) == 0) {
                AmbaDramAdjust_Show();
            }
#endif
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
