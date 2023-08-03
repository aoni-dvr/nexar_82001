/**
 *  @file AmbaDiag_CmdDDR.c
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

#include "AmbaRTSL_PLL.h"
#include "AmbaRTSL_DRAMC.h"
#include "AmbaRTSL_DDRC.h"
#include "AmbaCSL_DDRC.h"
#include "AmbaDRAMC.h"
#include "AmbaDDRC.h"
#include "AmbaWDT.h"

#include "AmbaDramAdjust.h"
#include "AmbaDramCommon.h"

#define DEFAULT_TIMER_INTERVAL      20000UL // 20 seconds
#define MAX_WDT_FEED_INTERVAL       5000UL  // We will set WDT timeout to (2 * FEED_INTERVAL) + 2, larger interval may cause timeout overflow.
#define DEFAULT_ACCESS_MASK         0xffffffffUL    // All bits

/* Definitions of DDR controller */
#define AMBA_DDRC0                  0U      /* DDRC0 */
#define AMBA_DDRC1                  1U      /* DDRC1 */
#define AMBA_NUM_DDRC               2U      /* Number of DDR Controller */
static const char* DDRC_TEXT[AMBA_NUM_DDRC] = {
    "DDRC-0",
    "DDRC-1",
};

static const UINT8 DiagDdrPhaseSetting[64] = {  /* for DLL write/sync/read phase shift  */
    0x3F, 0x3E, 0x3D, 0x3C, 0x3B, 0x3A, 0x39, 0x38, 0x37, 0x36, 0x35, 0x34, 0x33, 0x32, 0x31, 0x30,
    0x2F, 0x2E, 0x2D, 0x2C, 0x2B, 0x2A, 0x29, 0x28, 0x27, 0x26, 0x25, 0x24, 0x23, 0x22, 0x21, 0x20,
    0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
    0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F
};

typedef struct _MRDump {
    const char* name;
    UINT32 access_val;
    UINT32 die;
} MRDump;

typedef struct _RegDump {
    const char* name;
    size_t offset;
    UINT32 die;
} RegDump;

static RegDump dumplist[] = {
    {"DRAM_Ctrl",           DDRC_OFST_CTRL               , 0},
    {"DRAM_Config0",        DDRC_OFST_CONFIG0            , 0},
    {"DRAM_Config1",        DDRC_OFST_CONFIG1            , 0},
    {"DRAM_Timing1",        DDRC_OFST_TIMING0            , 0},
    {"DRAM_Timing2",        DDRC_OFST_TIMING1            , 0},
    {"DRAM_Timing3",        DDRC_OFST_TIMING2            , 0},
    {"DRAM_Timing4",        DDRC_OFST_TIMING3            , 0},
    {"DRAM_Timing5",        DDRC_OFST_TIMING4            , 0},
    {"DuelDieTiming",       DDRC_OFST_DUELDIETIMING      , 0},
    {"RefreshTiming",       DDRC_OFST_REFRESHTIMING      , 0},
    {"Lp5Timing",           DDRC_OFST_LP5TIMING          , 0},
    {"ByteMap",             DDRC_OFST_BYTEMAP            , 0},
    {"WdqsTiming",          DDRC_OFST_WDQSTIMING         , 0},
    {"DtteTiming",          DDRC_OFST_DTTETIMING         , 0},
    {"DllCtrlSel0D0",       DDRC_OFST_DLLCTRLSEL0D0      , 0},
    {"DllCtrlSel1D0",       DDRC_OFST_DLLCTRLSEL1D0      , 0},
    {"DllCtrlSel2D0",       DDRC_OFST_DLLCTRLSEL2D0      , 0},
    {"DllCtrlSel0D1",       DDRC_OFST_DLLCTRLSEL0D1      , 1},
    {"DllCtrlSel1D1",       DDRC_OFST_DLLCTRLSEL1D1      , 1},
    {"DllCtrlSel2D1",       DDRC_OFST_DLLCTRLSEL2D1      , 1},
    {"Byte0Die0Dly[0]",     DDRC_OFST_BYTE0DIE0DLY(0)    , 0},
    {"Byte0Die0Dly[1]",     DDRC_OFST_BYTE0DIE0DLY(1)    , 0},
    {"Byte0Die0Dly[2]",     DDRC_OFST_BYTE0DIE0DLY(2)    , 0},
    {"Byte0Die0Dly[3]",     DDRC_OFST_BYTE0DIE0DLY(3)    , 0},
    {"Byte0Die0Dly[4]",     DDRC_OFST_BYTE0DIE0DLY(4)    , 0},
    {"Byte0Die0Dly[5]",     DDRC_OFST_BYTE0DIE0DLY(5)    , 0},
    {"Byte1Die0Dly[0]",     DDRC_OFST_BYTE1DIE0DLY(0)    , 0},
    {"Byte1Die0Dly[1]",     DDRC_OFST_BYTE1DIE0DLY(1)    , 0},
    {"Byte1Die0Dly[2]",     DDRC_OFST_BYTE1DIE0DLY(2)    , 0},
    {"Byte1Die0Dly[3]",     DDRC_OFST_BYTE1DIE0DLY(3)    , 0},
    {"Byte1Die0Dly[4]",     DDRC_OFST_BYTE1DIE0DLY(4)    , 0},
    {"Byte1Die0Dly[5]",     DDRC_OFST_BYTE1DIE0DLY(5)    , 0},
    {"Byte2Die0Dly[0]",     DDRC_OFST_BYTE2DIE0DLY(0)    , 0},
    {"Byte2Die0Dly[1]",     DDRC_OFST_BYTE2DIE0DLY(1)    , 0},
    {"Byte2Die0Dly[2]",     DDRC_OFST_BYTE2DIE0DLY(2)    , 0},
    {"Byte2Die0Dly[3]",     DDRC_OFST_BYTE2DIE0DLY(3)    , 0},
    {"Byte2Die0Dly[4]",     DDRC_OFST_BYTE2DIE0DLY(4)    , 0},
    {"Byte2Die0Dly[5]",     DDRC_OFST_BYTE2DIE0DLY(5)    , 0},
    {"Byte3Die0Dly[0]",     DDRC_OFST_BYTE3DIE0DLY(0)    , 0},
    {"Byte3Die0Dly[1]",     DDRC_OFST_BYTE3DIE0DLY(1)    , 0},
    {"Byte3Die0Dly[2]",     DDRC_OFST_BYTE3DIE0DLY(2)    , 0},
    {"Byte3Die0Dly[3]",     DDRC_OFST_BYTE3DIE0DLY(3)    , 0},
    {"Byte3Die0Dly[4]",     DDRC_OFST_BYTE3DIE0DLY(4)    , 0},
    {"Byte3Die0Dly[5]",     DDRC_OFST_BYTE3DIE0DLY(5)    , 0},
    {"Byte0Die1Dly[0]",     DDRC_OFST_BYTE0DIE1DLY(0)    , 1},
    {"Byte0Die1Dly[1]",     DDRC_OFST_BYTE0DIE1DLY(1)    , 1},
    {"Byte0Die1Dly[2]",     DDRC_OFST_BYTE0DIE1DLY(2)    , 1},
    {"Byte0Die1Dly[3]",     DDRC_OFST_BYTE0DIE1DLY(3)    , 1},
    {"Byte0Die1Dly[4]",     DDRC_OFST_BYTE0DIE1DLY(4)    , 1},
    {"Byte0Die1Dly[5]",     DDRC_OFST_BYTE0DIE1DLY(5)    , 1},
    {"Byte1Die1Dly[0]",     DDRC_OFST_BYTE1DIE1DLY(0)    , 1},
    {"Byte1Die1Dly[1]",     DDRC_OFST_BYTE1DIE1DLY(1)    , 1},
    {"Byte1Die1Dly[2]",     DDRC_OFST_BYTE1DIE1DLY(2)    , 1},
    {"Byte1Die1Dly[3]",     DDRC_OFST_BYTE1DIE1DLY(3)    , 1},
    {"Byte1Die1Dly[4]",     DDRC_OFST_BYTE1DIE1DLY(4)    , 1},
    {"Byte1Die1Dly[5]",     DDRC_OFST_BYTE1DIE1DLY(5)    , 1},
    {"Byte2Die1Dly[0]",     DDRC_OFST_BYTE2DIE1DLY(0)    , 1},
    {"Byte2Die1Dly[1]",     DDRC_OFST_BYTE2DIE1DLY(1)    , 1},
    {"Byte2Die1Dly[2]",     DDRC_OFST_BYTE2DIE1DLY(2)    , 1},
    {"Byte2Die1Dly[3]",     DDRC_OFST_BYTE2DIE1DLY(3)    , 1},
    {"Byte2Die1Dly[4]",     DDRC_OFST_BYTE2DIE1DLY(4)    , 1},
    {"Byte2Die1Dly[5]",     DDRC_OFST_BYTE2DIE1DLY(5)    , 1},
    {"Byte3Die1Dly[0]",     DDRC_OFST_BYTE3DIE1DLY(0)    , 1},
    {"Byte3Die1Dly[1]",     DDRC_OFST_BYTE3DIE1DLY(1)    , 1},
    {"Byte3Die1Dly[2]",     DDRC_OFST_BYTE3DIE1DLY(2)    , 1},
    {"Byte3Die1Dly[3]",     DDRC_OFST_BYTE3DIE1DLY(3)    , 1},
    {"Byte3Die1Dly[4]",     DDRC_OFST_BYTE3DIE1DLY(4)    , 1},
    {"Byte3Die1Dly[5]",     DDRC_OFST_BYTE3DIE1DLY(5)    , 1},
    {"CkDly",               DDRC_OFST_CKDLY              , 0},
    {"CaDlyCoarse",         DDRC_OFST_CADLYCOARSE        , 0},
    {"CaDlyFineChADie0[0]", DDRC_OFST_CADLYFINECHADIE0(0), 0},
    {"CaDlyFineChADie0[1]", DDRC_OFST_CADLYFINECHADIE0(1), 0},
    {"CaDlyFineChADie1[0]", DDRC_OFST_CADLYFINECHADIE1(0), 1},
    {"CaDlyFineChADie1[1]", DDRC_OFST_CADLYFINECHADIE1(1), 1},
    {"CaDlyFineChBDie0[0]", DDRC_OFST_CADLYFINECHBDIE0(0), 0},
    {"CaDlyFineChBDie0[1]", DDRC_OFST_CADLYFINECHBDIE0(1), 0},
    {"CaDlyFineChBDie1[0]", DDRC_OFST_CADLYFINECHBDIE1(0), 1},
    {"CaDlyFineChBDie1[1]", DDRC_OFST_CADLYFINECHBDIE1(1), 1},
    {"CkeDlyCoarse",        DDRC_OFST_CKEDLYCOARSE       , 0},
    {"CkeDlyFine",          DDRC_OFST_CKEDLYFINE         , 0},
    {"CaPadCtrl",           DDRC_OFST_CAPADCTRL          , 0},
    {"DqPadCtrl",           DDRC_OFST_DQPADCTRL          , 0},
    {"RdVref0[0]",          DDRC_OFST_RDVREF0            , 0},
    {"RdVref0[1]",          DDRC_OFST_RDVREF1            , 0},
    {"DllStatus[0]",        DDRC_OFST_DLLSTATUS0         , 0},
    {"DllStatus[1]",        DDRC_OFST_DLLSTATUS1         , 0},
    {"ReadLinkEcc[0]",      DDRC_OFST_READLINKECC(0)     , 0},
    {"ReadLinkEcc[1]",      DDRC_OFST_READLINKECC(1)     , 0},
    {"ReadLinkEcc[2]",      DDRC_OFST_READLINKECC(2)     , 0},
    {"ReadLinkEcc[3]",      DDRC_OFST_READLINKECC(3)     , 0},
};

static MRDump mr_dumplist[] = {
    {"DRAM_MR1_D0",     0x9a010000, 0},
    {"DRAM_MR11_D0",    0x9a0b0000, 0},
    {"DRAM_MR12_D0",    0x9a0c0000, 0},
    {"DRAM_MR14_D0",    0x9a0e0000, 0},
    {"DRAM_MR15_D0",    0x9a0f0000, 0},
    {"DRAM_MR16_D0",    0x9a100000, 0},
    {"DRAM_MR19_D0",    0x9a130000, 0},
    {"DRAM_MR24_D0",    0x9a180000, 0},
    {"DRAM_MR43_D0",    0x9a2b0000, 0},
    {"DRAM_MR1_D1",     0x9c010000, 1},
    {"DRAM_MR11_D1",    0x9c0b0000, 1},
    {"DRAM_MR12_D1",    0x9c0c0000, 1},
    {"DRAM_MR14_D1",    0x9c0e0000, 1},
    {"DRAM_MR15_D1",    0x9c0f0000, 1},
    {"DRAM_MR16_D1",    0x9c100000, 1},
    {"DRAM_MR19_D1",    0x9c130000, 1},
    {"DRAM_MR24_D1",    0x9c180000, 1},
    {"DRAM_MR43_D1",    0x9c2b0000, 1},
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

extern UINT32* const pAmbaDDRC_APB_Reg[2U];

static AMBA_SHELL_PRINT_f DiagDdrPrintFunc = NULL;
static AMBA_DIAG_DDR_TASK_s DiagDdrTask GNU_ALIGNED_CACHESAFE = {0};
static INT8 DiagDdrEnableWdt = 0;

static void Diag_PrintDummy(const char *str, UINT32 len, AMBA_SHELL_PRINT_f PrintFunc)
{
    UINT32 i;
    for(i = 0; i < len; i++)
        PrintFunc(str);
}

static void Diag_PrintDdrcModeReg(UINT32 ddrc, MRDump mr, UINT32 dummy_len, AMBA_SHELL_PRINT_f PrintFunc)
{
    UINT64 args[3];

    Diag_DdrcSetModeRegWait(ddrc, 0xffffffff, mr.access_val);
    PrintFunc(mr.name);
    Diag_PrintDummy(" ", dummy_len, PrintFunc);
#if defined(CONFIG_FWPROG_ATF_ENABLE) && defined(CONFIG_LINUX)
    args[0] = 0x0;
#else
    args[0] = (UINT64)(&pAmbaDDRC_Reg[ddrc]->ModeReg);
#endif
    args[1] = HOST0_DDRC_REG_BASE_ADDR + ddrc*HOST_OFFSET + (UINT64)offsetof(AMBA_DDRC_REG_s, ModeReg);
#if defined(CONFIG_FWPROG_ATF_ENABLE) && defined(CONFIG_LINUX)
    args[2] = (UINT64)AmbaDDRC_Get(ddrc, offsetof(AMBA_DDRC_REG_s, ModeReg));
#else
    args[2] = (UINT64)pAmbaDDRC_Reg[ddrc]->ModeReg;
#endif
    Diag_PrintU64Str(" (0x%x)->[0x%x] = 0x%08x\n", 3, args, PrintFunc);
}

static void Diag_PrintDdrcReg(UINT32 ddrc, RegDump r, UINT32 dummy_len, AMBA_SHELL_PRINT_f PrintFunc)
{
#if defined(CONFIG_FWPROG_ATF_ENABLE) && defined(CONFIG_LINUX)
#else
    void* ptr;
#endif
    UINT64 args[3];

    PrintFunc(r.name);
    Diag_PrintDummy(" ", dummy_len, PrintFunc);

#if defined(CONFIG_FWPROG_ATF_ENABLE) && defined(CONFIG_LINUX)
    args[0] = 0x0;
#else
    ptr = (char*)pAmbaDDRC_Reg[ddrc] + r.offset;
    args[0] = (UINT64)ptr;
#endif
    args[1] = HOST0_DDRC_REG_BASE_ADDR + ddrc*HOST_OFFSET + (UINT64)r.offset;
#if defined(CONFIG_FWPROG_ATF_ENABLE) && defined(CONFIG_LINUX)
    args[2] = (UINT64)AmbaDDRC_Get(ddrc, r.offset);
#else
    args[2] = (UINT64)(*(UINT32*)ptr);
#endif
    Diag_PrintU64Str(" (0x%x)->[0x%x] = 0x%08x\n", 3, args, PrintFunc);
}

static INT32 Diag_DdrCheckRemainParameter(UINT32 ArgCount, char * const *pArgVector, UINT32 StartIndex, INT32 *pStep, UINT32 *pTimeInterval, UINT32 *pMask)
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

static void Diag_DdrShmooItemStart(UINT32 TimeInterval)
{
    UINT32 WdtFeedTime = (TimeInterval < MAX_WDT_FEED_INTERVAL) ? TimeInterval : MAX_WDT_FEED_INTERVAL;

    if (DiagDdrEnableWdt == 1) {
        (void)AmbaWDT_Start((WdtFeedTime << 1) + 2000U, 10U);  // (Feed interval * 2) + 2s
    }
}

static void Diag_DdrShmooItemWait(UINT32 TimeInterval)
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

static void Diag_DdrShmooItemStop(AMBA_SHELL_PRINT_f PrintFunc)
{
    if (DiagDdrEnableWdt == 1) {
        (void)AmbaWDT_Stop();
    }
    PrintFunc("Done\n");
}

static INT32 Diag_DdrGetDllValIndex(UINT32 Param)
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

#if defined(CONFIG_FWPROG_ATF_ENABLE) && defined(CONFIG_LINUX)
/**
 *  Copy from AmbaRTSL_PllGetDramClk - Get Dram clock frequency
 *  @return Dram clock frequency
 */
static UINT32 Diag_AmbaRTSL_PllGetDramClk(void)
{
    UINT32 Frequency;
    volatile UINT32 *pPllCtrlReg;
    volatile UINT32 *pPllCtrl2Reg;
    ULONG Addr;
    UINT32 Sdiv, Sout, IntProg, Fsdiv, Fsout;
    UINT32 ClkDiv2 = 0;
    UINT32 Reg2Bit12 = 0;

    Addr = (ULONG)pAmbaDDRC_APB_Reg[0];         //Addr = 0x20ed180000UL;
    AmbaMisra_TypeCast(&pPllCtrlReg, &Addr);
    Addr = (ULONG)pAmbaDDRC_APB_Reg[0] + 8UL;   //Addr = 0x20ed180000UL + 8UL;
    AmbaMisra_TypeCast(&pPllCtrl2Reg, &Addr);

    Frequency = AmbaRTSL_PllGetClkRefFreq();
    Sdiv = (pPllCtrlReg[0U] >> 12U) & 0xfU;
    Sout = (pPllCtrlReg[0U] >> 16U) & 0xfU;
    IntProg = (pPllCtrlReg[0U] >> 24U) & 0x7fU;
    Fsdiv = (pPllCtrl2Reg[0] >> 9U) & 0x1U;
    Fsout= (pPllCtrl2Reg[0] >> 11U) & 0x1U;
    ClkDiv2 = (pPllCtrl2Reg[0] >> 8U) & 0x1U;
    Reg2Bit12 = (pPllCtrl2Reg[0] >> 12U) & 0x1U;

    if ( 0x0U != Reg2Bit12 ) {
        Frequency = Frequency >> 1;
        Frequency = Frequency * (ClkDiv2 + 1U) * (Fsdiv + 1U) * (Sdiv + 1U) * (IntProg + 1U);
    } else {
        Frequency = Frequency / (Fsout + 1U) / (Sout + 1U) / 2U;
        Frequency = Frequency * (Fsdiv + 1U) * (Sdiv + 1U) * (IntProg + 1U);
    }

    if (AmbaDDRC_GetDramType(0U) != 1U) {
        if (AmbaDDRC_GetDramType(1U) != 1U) {
            Frequency /= 2U; // LPDDR4 clock is divided by 2 in PLL module
        }
    }
    return Frequency;
}
#endif

static void Diag_DdrCmdUsage(char * const *pArgVector, AMBA_SHELL_PRINT_f PrintFunc)
{
    PrintFunc("Usage: ");
    PrintFunc(pArgVector[0]);
    PrintFunc(" options ...\n");

    PrintFunc("  options:\n");
    PrintFunc("    dump        -  Dump important registers\n");
    PrintFunc("    dump [DDRC] -  Dump important registers\n");
    PrintFunc("      - [DDRC]: 0~1 for dumping DDRC host registers.\n");
    PrintFunc("    shmoo [DDRC] [sync|read|write] [+|-] [time(ms), default=20000] [step, default=1] [bitmask, default=0xffffffff]\n");
    PrintFunc("      How to run DDR quick shmoo :\n");
    PrintFunc("        - 1. Type \"sync + 1000\" :\n");
    PrintFunc("             Increase sync value and wait 1000ms, shmoo the sync byte one by one until system hang, \n");
    PrintFunc("             ex: 0x20->0x00->0x01->0x02\n");
    PrintFunc("        - 2. Type \"sync - 1000\" : \n");
    PrintFunc("             Decrease sync value and wait 1000ms, shmoo the sync byte one by one until system hang, \n");
    PrintFunc("             ex: 0x20->0x21->0x22->0x23\n");
    PrintFunc("        - 3. Repeat the above procedures on Read and Write phase.\n");
    PrintFunc("        - 4. Collect the workable ranges of sync, read and write byte.\n");
    PrintFunc("    set [DDRC] [sync|read|write|lp4rdly|lp4vref|lp4wdly|lp4mr14|lp4mr12] <value> [bitmask] \n");
    PrintFunc("      ex: set 0 lp4mr14 0x010E0010 0xffffffff\n");
    PrintFunc("      ex: set 0 lp4mr12 0x010C0008 0xffffffff\n");
    PrintFunc("\n");
    PrintFunc("    The following commands are for LPDDR4/LPDDR5 only!\n");
    PrintFunc("    [lp4rdly|lp4vref|lp4wdly|lp4mr14|lp4mr12] [DDRC] [+|-] [time(ms)] [step] [bitmask]\n");
    PrintFunc("    [dqsvref|wckdly|dqswdly|dqsgdly|ckdly|ckedly|cadly|lp5mr14|lp5mr15|lp5mr12] [DDRC] [+|-] [time(ms)] [step] [bitmask]\n");
    PrintFunc("      ex: diag ddr lp4rdly 0 + 30000 1 0xffffffff\n");
    PrintFunc("      (Read):  lp4rdly and lp4vref\n");
    PrintFunc("      (Write): lp4wdly and lp4mr14\n");
}

static void Diag_DdrDump(UINT32 ddrc, AMBA_SHELL_PRINT_f PrintFunc)
{
    UINT32 Value = 0;
    UINT32 DdrcId;
    UINT32 DdrcIdStart;
    UINT32 DdrcIdEnd;
    UINT32 isSingleDie = 0;
    UINT32 name_max = 0;
    UINT32 i;

    for (i = 0; i < sizeof(dumplist)/sizeof(RegDump); i++)
        name_max = (AmbaUtility_StringLength(dumplist[i].name) > name_max) ? AmbaUtility_StringLength(dumplist[i].name) : name_max;

    PrintFunc("--------------------------------------------------------------------------------\n");

    /* dram freq */
#if defined(CONFIG_FWPROG_ATF_ENABLE) && defined(CONFIG_LINUX)
    Diag_PrintSingleU32Str("DRAM Frequency      = %u\n", Diag_AmbaRTSL_PllGetDramClk(), PrintFunc);
#else
    Diag_PrintSingleU32Str("DRAM Frequency      = %u\n", AmbaRTSL_PllGetDramClk(), PrintFunc);
#endif
    /* Host number */
#if defined(CONFIG_FWPROG_ATF_ENABLE) && defined(CONFIG_LINUX)
    AmbaDRAMC_GetHostNum(&Value);
#else
    (void)AmbaRTSL_DramcGetHostNum(&Value);
#endif
    Diag_PrintSingleU32Str("Host number         = %u\n", Value, PrintFunc);

    if (ddrc == 0xFFFFFFFF) {
        DdrcIdStart = AMBA_DDRC0;
        DdrcIdEnd = AMBA_NUM_DDRC;
    } else {
        DdrcIdStart = ddrc;
        DdrcIdEnd = ddrc + 1;
    }

    for (DdrcId = DdrcIdStart; DdrcId < DdrcIdEnd; DdrcId ++) {
        PrintFunc("--------------------------------------------------------------------------------\n");

        /* ddrc id */
        PrintFunc(DDRC_TEXT[DdrcId]);
        PrintFunc("\n");

        /* dram type */
        PrintFunc("DRAM Type = ");
#if defined(CONFIG_FWPROG_ATF_ENABLE) && defined(CONFIG_LINUX)
        PrintFunc(AmbaDDRC_GetDramTypeStr(DdrcId));
#else
        PrintFunc(AmbaRTSL_DdrcGetDramTypeStr(DdrcId));
#endif
        PrintFunc("\n");
        /* dram size */
        PrintFunc("DRAM Size = ");
#if defined(CONFIG_FWPROG_ATF_ENABLE) && defined(CONFIG_LINUX)
        PrintFunc(AmbaDDRC_GetDramSizeStr(DdrcId));
#else
        PrintFunc(AmbaRTSL_DdrcGetDramSizeStr(DdrcId));
#endif
        PrintFunc(" (per Channel)\n");
        PrintFunc("--------------------------------------------------------------------------------\n");

        isSingleDie = Diag_DdrcIsSingleDie(DdrcId, PrintFunc);
        for (i = 0; i < sizeof(dumplist)/sizeof(RegDump); i++)
        {
            if(isSingleDie && (dumplist[i].die == 1U))
                continue;
            Diag_PrintDdrcReg(DdrcId, dumplist[i], name_max - AmbaUtility_StringLength(dumplist[i].name), PrintFunc);
        }

        for (i = 0; i < sizeof(mr_dumplist)/sizeof(MRDump); i++)
        {
            if(isSingleDie && (mr_dumplist[i].die == 1U))
                continue;
            Diag_PrintDdrcModeReg(DdrcId, mr_dumplist[i], name_max - AmbaUtility_StringLength(mr_dumplist[i].name), PrintFunc);
        }
    }
}

static void Diag_DdrSetValue(UINT32 ArgCount, char * const *pArgVector, AMBA_SHELL_PRINT_f PrintFunc)
{
    UINT32 DllByte = 0xffffffffUL;
    UINT32 DdrcId = AMBA_DDRC0, BitMask = DEFAULT_ACCESS_MASK, NewValue = 0;

    if (ArgCount < 5U) {
        Diag_DdrCmdUsage(pArgVector, PrintFunc);
        return;
    }

    (void)AmbaUtility_StringToUInt32(pArgVector[2], &DdrcId);
    (void)AmbaUtility_StringToUInt32(pArgVector[4], &NewValue);
    if (ArgCount > 5U) {
        (void)AmbaUtility_StringToUInt32(pArgVector[5], &BitMask);
    }

    if (AmbaUtility_StringCompare(pArgVector[3], "lp4rdly", 7U) == 0) {
        Diag_DdrSetLp4ReadDly(DdrcId, BitMask, NewValue, PrintFunc);
    } else if (AmbaUtility_StringCompare(pArgVector[3], "lp4vref", 7U) == 0) {
        Diag_DdrSetLp4Vref(DdrcId, BitMask, NewValue, PrintFunc);
    } else if (AmbaUtility_StringCompare(pArgVector[3], "lp4wdly", 7U) == 0) {
        Diag_DdrSetLp4WriteDly(DdrcId, BitMask, NewValue, PrintFunc);
    } else if (AmbaUtility_StringCompare(pArgVector[3], "lp4mr14", 7U) == 0) {
        Diag_DdrSetLp4MR14(DdrcId, BitMask, NewValue, PrintFunc);
    } else if (AmbaUtility_StringCompare(pArgVector[3], "lp4mr12", 7U) == 0) {
        Diag_DdrSetLp4MR12(DdrcId, BitMask, NewValue, PrintFunc);
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

        if (Diag_DdrGetDllValIndex(NewValue) >= 0) {
            Diag_DdrSetDLL(DdrcId, BitMask, DllByte, NewValue, PrintFunc);
        }
    }
}

static void Diag_DdrQuickShmoo(UINT32 ArgCount, char * const *pArgVector, AMBA_SHELL_PRINT_f PrintFunc)
{
    UINT32 DllByte = 0xffffffffUL, DllValue = 0xffffffffUL;
    UINT32 DdrcId = 0U, TimeInterval = 0U, BitMask = 0U;
    INT32 i, Index, Step = 0;
    UINT32 DllVfine[4] = {0};

    if (ArgCount < 5U) {
        Diag_DdrCmdUsage(pArgVector, PrintFunc);
    } else {
        (void)AmbaUtility_StringToUInt32(pArgVector[2], &DdrcId);
        if (Diag_DdrCheckRemainParameter(ArgCount, pArgVector, 4U, &Step, &TimeInterval, &BitMask) == 0) {
            return; // Invalid parameters
        }

#if defined(CONFIG_FWPROG_ATF_ENABLE) && defined(CONFIG_LINUX)
        DllVfine[0] = (AmbaDDRC_Get(DdrcId, offsetof(AMBA_DDRC_REG_s, DllStatus0)) >> 1U) & 0x1fU;
        DllVfine[1] = (AmbaDDRC_Get(DdrcId, offsetof(AMBA_DDRC_REG_s, DllStatus0)) >> 17U) & 0x1fU;
        DllVfine[2] = (AmbaDDRC_Get(DdrcId, offsetof(AMBA_DDRC_REG_s, DllStatus1)) >> 1U) & 0x1fU;
        DllVfine[3] = (AmbaDDRC_Get(DdrcId, offsetof(AMBA_DDRC_REG_s, DllStatus1)) >> 17U) & 0x1fU;
#else
        DllVfine[0] = (pAmbaDDRC_Reg[DdrcId]->DllStatus0 >> 1U) & 0x1fU;
        DllVfine[1] = (pAmbaDDRC_Reg[DdrcId]->DllStatus0 >> 17U) & 0x1fU;
        DllVfine[2] = (pAmbaDDRC_Reg[DdrcId]->DllStatus1 >> 1U) & 0x1fU;
        DllVfine[3] = (pAmbaDDRC_Reg[DdrcId]->DllStatus1 >> 17U) & 0x1fU;
#endif
        Diag_PrintFormattedRegValPerChan("[DDRC%d]Vfine: 0x%x,0x%x,", DdrcId, DllVfine[0], DllVfine[1], PrintFunc);
        Diag_PrintFormattedRegValPerChan("0x%x,0x%x\n", DllVfine[2], DllVfine[3], 0, PrintFunc);
        Diag_PrintFormattedRegValPerChan("[DDRC%d]DLL Lock Searching Range: 0x%x - 0x%x\n", DdrcId, (DllVfine[0] | 0x20U), (31U - DllVfine[0]), PrintFunc);

        if (AmbaUtility_StringCompare(pArgVector[3], "sync", 4U) == 0) {
            DllByte = AMBA_DDRC_DLL_BYTE_1;
#if defined(CONFIG_FWPROG_ATF_ENABLE) && defined(CONFIG_LINUX)
            DllValue = AmbaDDRC_Get(DdrcId, offsetof(AMBA_DDRC_REG_s, DllCtrlSel1D0)) & 0xFFU;
#else
            DllValue = AmbaCSL_DdrcGetD0Dll1(DdrcId) & 0xFFU;
#endif
        }
        if (AmbaUtility_StringCompare(pArgVector[3], "read", 4U) == 0) {
            DllByte = AMBA_DDRC_DLL_BYTE_0;
#if defined(CONFIG_FWPROG_ATF_ENABLE) && defined(CONFIG_LINUX)
            DllValue = AmbaDDRC_Get(DdrcId, offsetof(AMBA_DDRC_REG_s, DllCtrlSel0D0)) & 0xFFU;
#else
            DllValue = AmbaCSL_DdrcGetD0Dll0(DdrcId) & 0xFFU;
#endif
        }
        if (AmbaUtility_StringCompare(pArgVector[3], "write", 5U) == 0) {
            DllByte = AMBA_DDRC_DLL_BYTE_2;
#if defined(CONFIG_FWPROG_ATF_ENABLE) && defined(CONFIG_LINUX)
            DllValue = AmbaDDRC_Get(DdrcId, offsetof(AMBA_DDRC_REG_s, DllCtrlSel2D0)) & 0xFFU;
#else
            DllValue = AmbaCSL_DdrcGetD0Dll2(DdrcId) & 0xFFU;
#endif
        }

        Index = Diag_DdrGetDllValIndex(DllValue);
        if (Index < 0) {
            Index = 0;  // Set to reasonable range if the return DLL is out of our scan range
        }

        Diag_DdrShmooItemStart(TimeInterval);
        for (i = Index; (i >= 0) && (i < (INT32)sizeof(DiagDdrPhaseSetting)); i += Step) {
            Diag_DdrSetDLL(DdrcId, BitMask, DllByte, DiagDdrPhaseSetting[i], PrintFunc);
            Diag_DdrShmooItemWait(TimeInterval);
        }
        Diag_DdrShmooItemStop(PrintFunc);
    }
}

static void Diag_DdrLp4ReadDlyShmoo(UINT32 ArgCount, char * const *pArgVector, AMBA_SHELL_PRINT_f PrintFunc)
{
#define READ_DELAY_MAX         384
#define READ_DELAY_MIN         0
    UINT32 DdrcId = 0U, TimeInterval = 0U, BitMask = 0U, CurValue = 0U;
    INT32 i = 0, Step = 0;

    (void)AmbaUtility_StringToUInt32(pArgVector[2], &DdrcId);
    if (Diag_DdrCheckRemainParameter(ArgCount, pArgVector, 3U, &Step, &TimeInterval, &BitMask) == 0) {
        return; // Invalid parameters
    }

    CurValue = Diag_DdrGetLp4ReadDly(DdrcId, BitMask);  // DQ Read coarse, DM Read fine

    Diag_DdrShmooItemStart(TimeInterval);
    for (i = CurValue; (i >= READ_DELAY_MIN) && (i < READ_DELAY_MAX); i += Step) {
        Diag_DdrSetLp4ReadDly(DdrcId, BitMask, i, PrintFunc);
        Diag_DdrShmooItemWait(TimeInterval);
    }
    Diag_DdrShmooItemStop(PrintFunc);
}

static void Diag_DdrLp4VrefShmoo(UINT32 ArgCount, char * const *pArgVector, AMBA_SHELL_PRINT_f PrintFunc)
{
#define DQ_VREF_MAX    64
#define DQ_VREF_MIN    0
    UINT32 DdrcId = 0U, TimeInterval = 0U, BitMask = 0U, CurValue = 0U;
    INT32 i = 0, Step = 0;

    (void)AmbaUtility_StringToUInt32(pArgVector[2], &DdrcId);
    if (Diag_DdrCheckRemainParameter(ArgCount, pArgVector, 3U, &Step, &TimeInterval, &BitMask) == 0) {
        return; // Invalid parameters
    }

    CurValue = Diag_DdrGetLp4Vref(DdrcId, BitMask);

    Diag_DdrShmooItemStart(TimeInterval);
    for (i = CurValue; (i >= DQ_VREF_MIN) && (i < DQ_VREF_MAX); i += Step) {
        Diag_DdrSetLp4Vref(DdrcId, BitMask, i, PrintFunc);
        Diag_DdrShmooItemWait(TimeInterval);
    }
    Diag_DdrShmooItemStop(PrintFunc);
}

static void Diag_DdrLp4Mr14Shmoo(UINT32 ArgCount, char * const *pArgVector, AMBA_SHELL_PRINT_f PrintFunc)
{
#define LP4_MR14_MAX                    81
#define LP4_MR14_MIN                    0
    UINT32 DdrcId = 0U, TimeInterval = 0U, BitMask = 0U, CurValue = 0U;
    INT32 i = 0, Step = 0;

    (void)AmbaUtility_StringToUInt32(pArgVector[2], &DdrcId);
    if (Diag_DdrCheckRemainParameter(ArgCount, pArgVector, 3U, &Step, &TimeInterval, &BitMask) == 0) {
        return; // Invalid parameters
    }

    CurValue = Diag_DdrGetLp4MR14(DdrcId, BitMask);

    Diag_DdrShmooItemStart(TimeInterval);
    for (i = CurValue; (i >= LP4_MR14_MIN) && (i < LP4_MR14_MAX); i += Step) {
        Diag_DdrSetLp4MR14(DdrcId, BitMask, i, PrintFunc);
        Diag_DdrShmooItemWait(TimeInterval);
    }
    Diag_DdrShmooItemStop(PrintFunc);
}

static void Diag_DdrLp4Mr12Shmoo(UINT32 ArgCount, char * const *pArgVector, AMBA_SHELL_PRINT_f PrintFunc)
{
#define LP4_MR12_MAX                    81
#define LP4_MR12_MIN                    0
    UINT32 DdrcId = 0U, TimeInterval = 0U, BitMask = 0U, CurValue = 0U;
    INT32 i = 0, Step = 0;

    (void)AmbaUtility_StringToUInt32(pArgVector[2], &DdrcId);
    if (Diag_DdrCheckRemainParameter(ArgCount, pArgVector, 3U, &Step, &TimeInterval, &BitMask) == 0) {
        return; // Invalid parameters
    }

    CurValue = Diag_DdrGetLp4MR12(DdrcId, BitMask);

    Diag_DdrShmooItemStart(TimeInterval);
    for (i = CurValue; (i >= LP4_MR12_MIN) && (i < LP4_MR12_MAX); i += Step) {
        Diag_DdrSetLp4MR12(DdrcId, BitMask, i, PrintFunc);
        Diag_DdrShmooItemWait(TimeInterval);
    }
    Diag_DdrShmooItemStop(PrintFunc);
}

static void Diag_DdrLp4WrtieDlyShmoo(UINT32 ArgCount, char * const *pArgVector, AMBA_SHELL_PRINT_f PrintFunc)
{
#define WRITE_DELAY_MAX         384
#define WRITE_DELAY_MIN         0
    UINT32 DdrcId = 0U, TimeInterval = 0U, BitMask = 0U, CurValue = 0U;
    INT32 i = 0, Step = 0;

    (void)AmbaUtility_StringToUInt32(pArgVector[2], &DdrcId);
    if (Diag_DdrCheckRemainParameter(ArgCount, pArgVector, 3U, &Step, &TimeInterval, &BitMask) == 0) {
        return; // Invalid parameters
    }

    CurValue = Diag_DdrGetLp4WriteDly(DdrcId, BitMask);  // DQ Write coarse, DM Write fine

    Diag_DdrShmooItemStart(TimeInterval);
    for (i = CurValue; (i >= WRITE_DELAY_MIN) && (i < WRITE_DELAY_MAX); i += Step) {
        Diag_DdrSetLp4WriteDly(DdrcId, BitMask, i, PrintFunc);
        Diag_DdrShmooItemWait(TimeInterval);
    }
    Diag_DdrShmooItemStop(PrintFunc);
}

static void Diag_DdrDqsVrefShmoo(UINT32 ArgCount, char * const *pArgVector, AMBA_SHELL_PRINT_f PrintFunc)
{
#define DQS_VREF_MAX    64
#define DQS_VREF_MIN    0
    UINT32 DdrcId = 0U, TimeInterval = 0U, BitMask = 0U, CurValue = 0U;
    INT32 i = 0, Step = 0;

    (void)AmbaUtility_StringToUInt32(pArgVector[2], &DdrcId);
    if (Diag_DdrCheckRemainParameter(ArgCount, pArgVector, 3U, &Step, &TimeInterval, &BitMask) == 0) {
        return; // Invalid parameters
    }

    CurValue = Diag_DdrGetDqsVref(DdrcId, BitMask);

    Diag_DdrShmooItemStart(TimeInterval);
    for (i = CurValue; (i >= DQS_VREF_MIN) && (i < DQS_VREF_MAX); i += Step) {
        Diag_DdrSetDqsVref(DdrcId, BitMask, i, PrintFunc);
        Diag_DdrShmooItemWait(TimeInterval);
    }
    Diag_DdrShmooItemStop(PrintFunc);
}

static void Diag_DdrWckDlyShmoo(UINT32 ArgCount, char * const *pArgVector, AMBA_SHELL_PRINT_f PrintFunc)
{
#define WCK_DLY_MAX                 384
#define WCK_DLY_MIN                 0
    UINT32 DdrcId = 0U, TimeInterval = 0U, BitMask = 0U, CurValue = 0U;
    INT32 i = 0, Step = 0;

    (void)AmbaUtility_StringToUInt32(pArgVector[2], &DdrcId);
    if (Diag_DdrCheckRemainParameter(ArgCount, pArgVector, 3U, &Step, &TimeInterval, &BitMask) == 0) {
        return; // Invalid parameters
    }

    CurValue = Diag_DdrGetWckDly(DdrcId, BitMask);  // WCK coarse, fine

    Diag_DdrShmooItemStart(TimeInterval);
    for (i = CurValue; (i >= WCK_DLY_MIN) && (i < WCK_DLY_MAX); i += Step) {
        Diag_DdrSetWckDly(DdrcId, BitMask, i, PrintFunc);
        Diag_DdrShmooItemWait(TimeInterval);
    }
    Diag_DdrShmooItemStop(PrintFunc);
}

static void Diag_DdrDqsWriteDlyShmoo(UINT32 ArgCount, char * const *pArgVector, AMBA_SHELL_PRINT_f PrintFunc)
{
#define DQS_WRITE_DLY_MAX           384
#define DQS_WRITE_DLY_MIN           0
    UINT32 DdrcId = 0U, TimeInterval = 0U, BitMask = 0U, CurValue = 0U;
    INT32 i = 0, Step = 0;

    (void)AmbaUtility_StringToUInt32(pArgVector[2], &DdrcId);
    if (Diag_DdrCheckRemainParameter(ArgCount, pArgVector, 3U, &Step, &TimeInterval, &BitMask) == 0) {
        return; // Invalid parameters
    }

    CurValue = Diag_DdrGetDqsWriteDly(DdrcId, BitMask);  // DQS Write coarse, fine

    Diag_DdrShmooItemStart(TimeInterval);
    for (i = CurValue; (i >= DQS_WRITE_DLY_MIN) && (i < DQS_WRITE_DLY_MAX); i += Step) {
        Diag_DdrSetDqsWriteDly(DdrcId, BitMask, i, PrintFunc);
        Diag_DdrShmooItemWait(TimeInterval);
    }
    Diag_DdrShmooItemStop(PrintFunc);
}

static void Diag_DdrDqsGateDlyShmoo(UINT32 ArgCount, char * const *pArgVector, AMBA_SHELL_PRINT_f PrintFunc)
{
#define DQS_GATE_DLY_MAX            384
#define DQS_GATE_DLY_MIN            0
    UINT32 DdrcId = 0U, TimeInterval = 0U, BitMask = 0U, CurValue = 0U;
    INT32 i = 0, Step = 0;

    (void)AmbaUtility_StringToUInt32(pArgVector[2], &DdrcId);
    if (Diag_DdrCheckRemainParameter(ArgCount, pArgVector, 3U, &Step, &TimeInterval, &BitMask) == 0) {
        return; // Invalid parameters
    }

    CurValue = Diag_DdrGetDqsGateDly(DdrcId, BitMask);  // DQS Gate coarse, fine

    Diag_DdrShmooItemStart(TimeInterval);
    for (i = CurValue; (i >= DQS_GATE_DLY_MIN) && (i < DQS_GATE_DLY_MAX); i += Step) {
        Diag_DdrSetDqsGateDly(DdrcId, BitMask, i, PrintFunc);
        Diag_DdrShmooItemWait(TimeInterval);
    }
    Diag_DdrShmooItemStop(PrintFunc);
}

static void Diag_DdrCkDlyShmoo(UINT32 ArgCount, char * const *pArgVector, AMBA_SHELL_PRINT_f PrintFunc)
{
#define CK_DLY_MAX                  384
#define CK_DLY_MIN                  0
    UINT32 DdrcId = 0U, TimeInterval = 0U, BitMask = 0U, CurValue = 0U;
    INT32 i = 0, Step = 0;

    (void)AmbaUtility_StringToUInt32(pArgVector[2], &DdrcId);
    if (Diag_DdrCheckRemainParameter(ArgCount, pArgVector, 3U, &Step, &TimeInterval, &BitMask) == 0) {
        return; // Invalid parameters
    }

    CurValue = Diag_DdrGetCkDly(DdrcId, BitMask);  // CK coarse, fine

    Diag_DdrShmooItemStart(TimeInterval);
    for (i = CurValue; (i >= CK_DLY_MIN) && (i < CK_DLY_MAX); i += Step) {
        Diag_DdrSetCkDly(DdrcId, BitMask, i, PrintFunc);
        Diag_DdrShmooItemWait(TimeInterval);
    }
    Diag_DdrShmooItemStop(PrintFunc);
}

static void Diag_DdrCkeDlyShmoo(UINT32 ArgCount, char * const *pArgVector, AMBA_SHELL_PRINT_f PrintFunc)
{
#define CKE_DLY_MAX                 384
#define CKE_DLY_MIN                 0
    UINT32 DdrcId = 0U, TimeInterval = 0U, BitMask = 0U, CurValue = 0U;
    INT32 i = 0, Step = 0;

    (void)AmbaUtility_StringToUInt32(pArgVector[2], &DdrcId);
    if (Diag_DdrCheckRemainParameter(ArgCount, pArgVector, 3U, &Step, &TimeInterval, &BitMask) == 0) {
        return; // Invalid parameters
    }

    CurValue = Diag_DdrGetCkeDly(DdrcId, BitMask);  // CKE coarse, fine

    Diag_DdrShmooItemStart(TimeInterval);
    for (i = CurValue; (i >= CKE_DLY_MIN) && (i < CKE_DLY_MAX); i += Step) {
        Diag_DdrSetCkeDly(DdrcId, BitMask, i, PrintFunc);
        Diag_DdrShmooItemWait(TimeInterval);
    }
    Diag_DdrShmooItemStop(PrintFunc);
}

static void Diag_DdrCaDlyShmoo(UINT32 ArgCount, char * const *pArgVector, AMBA_SHELL_PRINT_f PrintFunc)
{
#define CA_DLY_MAX                  384
#define CA_DLY_MIN                  0
    UINT32 DdrcId = 0U, TimeInterval = 0U, BitMask = 0U, CurValue = 0U;
    INT32 i = 0, Step = 0;

    (void)AmbaUtility_StringToUInt32(pArgVector[2], &DdrcId);
    if (Diag_DdrCheckRemainParameter(ArgCount, pArgVector, 3U, &Step, &TimeInterval, &BitMask) == 0) {
        return; // Invalid parameters
    }

    CurValue = Diag_DdrGetCaDly(DdrcId, BitMask);  // CA coarse, fine

    Diag_DdrShmooItemStart(TimeInterval);
    for (i = CurValue; (i >= CA_DLY_MIN) && (i < CA_DLY_MAX); i += Step) {
        Diag_DdrSetCaDly(DdrcId, BitMask, i, PrintFunc);
        Diag_DdrShmooItemWait(TimeInterval);
    }
    Diag_DdrShmooItemStop(PrintFunc);
}

static void Diag_DdrLp5Mr14Shmoo(UINT32 ArgCount, char * const *pArgVector, AMBA_SHELL_PRINT_f PrintFunc)
{
#define LP5_MR14_MAX                    128
#define LP5_MR14_MIN                    0
    UINT32 DdrcId = 0U, TimeInterval = 0U, BitMask = 0U, CurValue = 0U;
    INT32 i = 0, Step = 0;

    (void)AmbaUtility_StringToUInt32(pArgVector[2], &DdrcId);
    if (Diag_DdrCheckRemainParameter(ArgCount, pArgVector, 3U, &Step, &TimeInterval, &BitMask) == 0) {
        return; // Invalid parameters
    }

    CurValue = Diag_DdrGetLp5MR14(DdrcId, BitMask);

    Diag_DdrShmooItemStart(TimeInterval);
    for (i = CurValue; (i >= LP5_MR14_MIN) && (i < LP5_MR14_MAX); i += Step) {
        Diag_DdrSetLp5MR14(DdrcId, BitMask, i, PrintFunc);
        Diag_DdrShmooItemWait(TimeInterval);
    }
    Diag_DdrShmooItemStop(PrintFunc);
}

static void Diag_DdrLp5Mr15Shmoo(UINT32 ArgCount, char * const *pArgVector, AMBA_SHELL_PRINT_f PrintFunc)
{
#define LP5_MR15_MAX                    128
#define LP5_MR15_MIN                    0
    UINT32 DdrcId = 0U, TimeInterval = 0U, BitMask = 0U, CurValue = 0U;
    INT32 i = 0, Step = 0;

    (void)AmbaUtility_StringToUInt32(pArgVector[2], &DdrcId);
    if (Diag_DdrCheckRemainParameter(ArgCount, pArgVector, 3U, &Step, &TimeInterval, &BitMask) == 0) {
        return; // Invalid parameters
    }

    CurValue = Diag_DdrGetLp5MR15(DdrcId, BitMask);

    Diag_DdrShmooItemStart(TimeInterval);
    for (i = CurValue; (i >= LP5_MR15_MIN) && (i < LP5_MR15_MAX); i += Step) {
        Diag_DdrSetLp5MR15(DdrcId, BitMask, i, PrintFunc);
        Diag_DdrShmooItemWait(TimeInterval);
    }
    Diag_DdrShmooItemStop(PrintFunc);
}

static void Diag_DdrLp5Mr12Shmoo(UINT32 ArgCount, char * const *pArgVector, AMBA_SHELL_PRINT_f PrintFunc)
{
#define LP5_MR12_MAX                    128
#define LP5_MR12_MIN                    0
    UINT32 DdrcId = AMBA_DDRC0, TimeInterval = DEFAULT_TIMER_INTERVAL, BitMask = 0U, CurValue = 0;
    INT32 i = 0, Step = 0;

    (void)AmbaUtility_StringToUInt32(pArgVector[2], &DdrcId);
    if (Diag_DdrCheckRemainParameter(ArgCount, pArgVector, 3U, &Step, &TimeInterval, &BitMask) == 0) {
        return; // Invalid parameters
    }

    CurValue = Diag_DdrGetLp5MR12(DdrcId, BitMask);

    Diag_DdrShmooItemStart(TimeInterval);
    for (i = CurValue; (i >= LP5_MR12_MIN) && (i < LP5_MR12_MAX); i += Step) {
        Diag_DdrSetLp5MR12(DdrcId, BitMask, i, PrintFunc);
        Diag_DdrShmooItemWait(TimeInterval);
    }
    Diag_DdrShmooItemStop(PrintFunc);
}

void Diag_DdrDoShmoo(UINT32 ArgCount, char * const *pArgVector, AMBA_SHELL_PRINT_f PrintFunc)
{
    if (AmbaUtility_StringCompare(pArgVector[1], "shmoo", 5U) == 0) {
        Diag_DdrQuickShmoo(ArgCount, pArgVector, PrintFunc);
    } else if (AmbaUtility_StringCompare(pArgVector[1], "set", 3U) == 0) {
        Diag_DdrSetValue(ArgCount, pArgVector, PrintFunc);
    } else if (AmbaUtility_StringCompare(pArgVector[1], "lp4rdly", 7U) == 0) {
        Diag_DdrLp4ReadDlyShmoo(ArgCount, pArgVector, PrintFunc);
    } else if (AmbaUtility_StringCompare(pArgVector[1], "lp4vref", 7U) == 0) {
        Diag_DdrLp4VrefShmoo(ArgCount, pArgVector, PrintFunc);
    } else if (AmbaUtility_StringCompare(pArgVector[1], "lp4wdly", 7U) == 0) {
        Diag_DdrLp4WrtieDlyShmoo(ArgCount, pArgVector, PrintFunc);
    } else if (AmbaUtility_StringCompare(pArgVector[1], "lp4mr14", 7U) == 0) {
        Diag_DdrLp4Mr14Shmoo(ArgCount, pArgVector, PrintFunc);
    } else if (AmbaUtility_StringCompare(pArgVector[1], "lp4mr12", 7U) == 0) {
        Diag_DdrLp4Mr12Shmoo(ArgCount, pArgVector, PrintFunc);
    } else if (AmbaUtility_StringCompare(pArgVector[1], "dqsvref", 7U) == 0) {
        Diag_DdrDqsVrefShmoo(ArgCount, pArgVector, PrintFunc);
    } else if (AmbaUtility_StringCompare(pArgVector[1], "wckdly", 6U) == 0) {
        Diag_DdrWckDlyShmoo(ArgCount, pArgVector, PrintFunc);
    } else if (AmbaUtility_StringCompare(pArgVector[1], "dqswdly", 7U) == 0) {
        Diag_DdrDqsWriteDlyShmoo(ArgCount, pArgVector, PrintFunc);
    } else if (AmbaUtility_StringCompare(pArgVector[1], "dqsgdly", 7U) == 0) {
        Diag_DdrDqsGateDlyShmoo(ArgCount, pArgVector, PrintFunc);
    } else if (AmbaUtility_StringCompare(pArgVector[1], "ckdly", 5U) == 0) {
        Diag_DdrCkDlyShmoo(ArgCount, pArgVector, PrintFunc);
    } else if (AmbaUtility_StringCompare(pArgVector[1], "ckedly", 6U) == 0) {
        Diag_DdrCkeDlyShmoo(ArgCount, pArgVector, PrintFunc);
    } else if (AmbaUtility_StringCompare(pArgVector[1], "cadly", 5U) == 0) {
        Diag_DdrCaDlyShmoo(ArgCount, pArgVector, PrintFunc);
    } else if (AmbaUtility_StringCompare(pArgVector[1], "lp5mr14", 7U) == 0) {
        Diag_DdrLp5Mr14Shmoo(ArgCount, pArgVector, PrintFunc);
    } else if (AmbaUtility_StringCompare(pArgVector[1], "lp5mr15", 7U) == 0) {
        Diag_DdrLp5Mr15Shmoo(ArgCount, pArgVector, PrintFunc);
    } else if (AmbaUtility_StringCompare(pArgVector[1], "lp5mr12", 7U) == 0) {
        Diag_DdrLp5Mr12Shmoo(ArgCount, pArgVector, PrintFunc);
    } else {
        Diag_DdrCmdUsage(pArgVector, PrintFunc);
    }
}

static void * Diag_DdrTaskEntry(void * EntryArg)
{
    UINT32 ArgCount = 0U, i;
    char * ArgVector[MAX_ARGV_NUM];

    AmbaMisra_TypeCast32(&ArgCount, &EntryArg);

    if (ArgCount == DiagDdrTask.ArgCount) {
        for (i = 0; i < ArgCount; i++) {
            ArgVector[i] = DiagDdrTask.ArgVector[i];
        }
        Diag_DdrDoShmoo(ArgCount, ArgVector, DiagDdrPrintFunc);
    }
    // Mark task end
    DiagDdrTask.TaskRunning = 0;

    return NULL;
}

void Diag_DdrEccStatus(AMBA_SHELL_PRINT_f PrintFunc)
{
    UINT32 Value = 0;
    UINT32 DdrcId;
    UINT32 DdrcIdStart;
    UINT32 DdrcIdEnd;
    UINT32 isSingleDie = 0;
    UINT32 name_max = 0;
    UINT32 i;

    for (i = 0; i < sizeof(dumplist)/sizeof(RegDump); i++)
        name_max = (AmbaUtility_StringLength(dumplist[i].name) > name_max) ? AmbaUtility_StringLength(dumplist[i].name) : name_max;

#if defined(CONFIG_FWPROG_ATF_ENABLE) && defined(CONFIG_LINUX)
    AmbaDRAMC_GetHostNum(&Value);
#else
    (void)AmbaRTSL_DramcGetHostNum(&Value);
#endif
    if (Value == 0x1) {
        DdrcIdStart = AMBA_DDRC0;
        DdrcIdEnd = AMBA_DDRC0 + 1;
    } else {
        DdrcIdStart = AMBA_DDRC0;
        DdrcIdEnd = AMBA_NUM_DDRC;
    }

    for (DdrcId = DdrcIdStart; DdrcId < DdrcIdEnd; DdrcId ++) {
        /* ddrc id */
        PrintFunc(DDRC_TEXT[DdrcId]);
        PrintFunc("\n");

        /* Read current statistic */
        isSingleDie = Diag_DdrcIsSingleDie(DdrcId, PrintFunc);
        for (i = 0; i < sizeof(dumplist)/sizeof(RegDump); i++)
        {
            if(isSingleDie && (dumplist[i].die == 1U))
                continue;
            if (AmbaUtility_StringCompare(dumplist[i].name, "ReadLinkEcc", 11U) == 0)
                Diag_PrintDdrcReg(DdrcId, dumplist[i], name_max - AmbaUtility_StringLength(dumplist[i].name), PrintFunc);
        }

        for (i = 0; i < sizeof(mr_dumplist)/sizeof(MRDump); i++)
        {
            if(isSingleDie && (mr_dumplist[i].die == 1U))
                continue;
            if (AmbaUtility_StringCompare(mr_dumplist[i].name, "DRAM_MR43", 9U) == 0)
                Diag_PrintDdrcModeReg(DdrcId, mr_dumplist[i], name_max - AmbaUtility_StringLength(mr_dumplist[i].name), PrintFunc);
        }

        /* Clear and enable statistic for next read */
        Value = Diag_DdrcGetMisc2(DdrcId);
        Diag_DdrcSetMisc2(DdrcId, Value & (~0x2U));
        Diag_DdrcSetMisc2(DdrcId, Value | (0x2U));
    }
}

static UINT32 Diag_DdrTaskStart(UINT32 ArgCount, char * const *pArgVector, AMBA_SHELL_PRINT_f PrintFunc)
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
                                    Diag_DdrTaskEntry,
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
 *  @RoutineName:: AmbaDiag_CmdDDR
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
void AmbaDiag_CmdDDR(UINT32 ArgCount, char * const *pArgVector, AMBA_SHELL_PRINT_f PrintFunc)
{
    if (ArgCount < 2U) {
        Diag_DdrCmdUsage(pArgVector, PrintFunc);
    } else {
        if (AmbaUtility_StringCompare(pArgVector[1], "dump", 4U) == 0) {
            UINT32 ddrc = 0xFFFFFFFF;
            if (ArgCount == 2U) {
                Diag_DdrDump(ddrc, PrintFunc);
            } else if (ArgCount == 3U) {
                (void)AmbaUtility_StringToUInt32(pArgVector[2], &ddrc);
                Diag_DdrDump(ddrc, PrintFunc);
            }
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
        } else if (AmbaUtility_StringCompare(pArgVector[1], "ecc_stat", 8U) == 0) {
            Diag_DdrEccStatus(PrintFunc);
        } else if (AmbaUtility_StringCompare(pArgVector[1], "wdt", 3U) == 0) {
            if ((ArgCount >= 3) && AmbaUtility_StringCompare(pArgVector[2], "1", 1U) == 0) {
                DiagDdrEnableWdt = 1;
            } else {
                DiagDdrEnableWdt = 0;
            }
        } else if (AmbaUtility_StringCompare(pArgVector[1], "task", 4U) == 0) {
            ArgCount -= 1U;
            Diag_DdrTaskStart(ArgCount, &pArgVector[1], PrintFunc);
        } else {
            Diag_DdrDoShmoo(ArgCount, pArgVector, PrintFunc);
        }
    }
}
