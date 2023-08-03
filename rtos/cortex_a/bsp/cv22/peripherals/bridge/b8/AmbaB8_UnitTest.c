/**
 *  @file AmbaB8_Test.c
 *
 *  @copyright Copyright (c) 2015 Ambarella, Inc.
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
 *  @details B8 Test APIs
 *
 */

#include "AmbaB8.h"
#include "AmbaB8_UnitTest.h"
#include "AmbaB8_IoMap.h"
#include "AmbaB8_GPIO.h"
#include "AmbaB8_I2C.h"
#include "AmbaB8_SPI.h"
#include "AmbaB8_PLL.h"
#include "AmbaB8_Packer.h"
#include "AmbaB8_Depacker.h"
#include "AmbaB8_Communicate.h"
#include "AmbaB8_Calibration.h"
#include "AmbaB8_Serdes.h"
#include "AmbaB8D_Calibration.h"
#include "AmbaB8D_Serdes.h"

#include "AmbaB8_PHY.h"

#ifdef B8_DEV_VERSION
#define REQUIRED_CAL_MEMORY_SIZE    (8*65536)
#define UTIL_MAX_HEX_STR_LEN        (11)
#endif
#define B8_NUM_MODULES              (24)

typedef struct {
    UINT16 Offset;
    UINT16 Count;
} B8_REG_RANGE_s;

typedef struct {
    const char *Name;
    UINT32 StartAddr;
    UINT16 NumOfRegs;
    UINT16 NumOfRegRangesToSkip;
    const B8_REG_RANGE_s *pRegRangesToSkip; /* skip registers that shouldn't be dumped, such as rx fifo */
} B8_REG_DUMP_LIST_s;

typedef struct {
    UINT32 ChipID;
    UINT8 AddrInc;
    UINT32 RegAddr;
} B8_ARGUMENT_s;


#if (B8_DBG_TEMP)
UINT32 gB8DbgShowWCmd       = 0U;

/* config related */
UINT32 gB8DbgConfig         = 1U;
UINT32 gB8DbgSensorConfig   = 1U;
UINT32 gB8DbgVinConfig      = 1U;

/* serdes related */
UINT32 gB8DbgSerDesRate     = 0xffU;

/* sensor related */
UINT32 gB8DbgVinAltID          = 1U;
UINT32 gB8DbgSensorID       = 0xffU;
#endif

#ifdef B8_DEV_VERSION
static AMBA_KAL_BYTE_POOL_t *pCalibMemoryPool = NULL;
static void* pCalibMemoryAddress = NULL;
static UINT8 CalPool[REQUIRED_CAL_MEMORY_SIZE];
#endif
static B8_REG_RANGE_s B8_PwmEncRegToSkip[1] = {
    [0] = { .Offset = 0x0018, .Count = 13 },  /* Offset: 0x0018 - 0x0048 */
};

static B8_REG_RANGE_s B8_SpiRegToSkip[1] = {
    [0] = { .Offset = 0x0060, .Count = 128 }, /* Offset: 0x0060 - 0x025c */
};

static B8_REG_RANGE_s B8_CodecRegToSkip[9] = {
    [0] = { .Offset = 0x0028, .Count = 2   }, /* Offset: 0x0028 - 0x002c */
    [1] = { .Offset = 0x0034, .Count = 3   }, /* Offset: 0x0034 - 0x003c */
    [2] = { .Offset = 0x0050, .Count = 124 }, /* Offset: 0x0050 - 0x023c */
    [3] = { .Offset = 0x02d0, .Count = 76  }, /* Offset: 0x02d0 - 0x03fc */
    [4] = { .Offset = 0x0a0c, .Count = 125 }, /* Offset: 0x0a0c - 0x0bfc */
    [5] = { .Offset = 0x1000, .Count = 64  }, /* Offset: 0x1000 - 0x10fc */
    [6] = { .Offset = 0x1200, .Count = 16  }, /* Offset: 0x1200 - 0x123c */
    [7] = { .Offset = 0x12d0, .Count = 76  }, /* Offset: 0x12d0 - 0x13fc */
    [8] = { .Offset = 0x1a0c, .Count = 125 }, /* Offset: 0x1a0c - 0x1bfc */
};

static B8_REG_RANGE_s B8_I2cRegToSkip[2] = {
    [0] = { .Offset = 0x0008, .Count = 1, },  /* Offset: 0x0008 */
    [1] = { .Offset = 0x002c, .Count = 1, },  /* Offset: 0x002c */
};

static B8_REG_DUMP_LIST_s B8_RegDumpList[B8_NUM_MODULES] = {
    {"RCT",            B8_AHB_RCT_BASE_ADDR,            495,  0, NULL              },
    {"Packer 0",       B8_AHB_PACKER0_BASE_ADDR,        29,   0, NULL              },
    {"Depacker 0",     B8_AHB_DEPACKER_0_BASE_ADDR,     29,   0, NULL              },
    {"Frame-Sync",     B8_AHB_FSYNC_BASE_ADDR,          48,   0, NULL              },
    {"MIPI Debug",     B8_AHB_MIPI_DBG_BASE_ADDR,       276,  0, NULL              },
    {"CFA Codec",      B8_AHB_CFG_CODEC_BASE_ADDR,      2048, 9, B8_CodecRegToSkip },
    {"Merger",         B8_AHB_MERGER_BASE_ADDR,         59,   0, NULL              },
    {"PWM Encoder",    B8_AHB_PWM_ENCODER_BASE_ADDR,    115,  1, B8_PwmEncRegToSkip},
    {"PWM Decoder",    B8_AHB_PWM_DECODER_BASE_ADDR,    8,    0, NULL              },
    {"VIC 0",          B8_AHB_VIC0_BASE_ADDR,           27,   0, NULL              },
    {"SSI_AHB_2SP",    B8_AHB_SSI_BASE_ADDR,            164,  1, B8_SpiRegToSkip   },
    {"Cfg_VIN 0",      B8_AHB_VIN0_BASE_ADDR,           64,   0, NULL              },
    {"Cfg_VIN 1",      B8_AHB_VIN1_BASE_ADDR,           64,   0, NULL              },
    {"Cfg_Prescale 0", B8_AHB_PRESCALE0_BASE_ADDR,      64,   0, NULL              },
    {"Cfg_Prescale 1", B8_AHB_PRESCALE1_BASE_ADDR,      64,   0, NULL              },
    {"Cfg_Comp 0",     B8_AHB_COMPRESSOR0_BASE_ADDR,    9,    0, NULL              },
    {"Cfg_Decomp 0",   B8_AHB_DECOMPRESSOR0_BASE_ADDR,  10,   0, NULL              },
    {"Scratchpad",     B8_AHB_SCRATCHPAD_BASE_ADDR,     50,   0, NULL              },
    {"VOUT0",          B8_AHB_VOUT0_BASE_ADDR,          38,   0, NULL              },
    {"VOUT1",          B8_AHB_VOUT1_BASE_ADDR,          38,   0, NULL              },
    {"IOMUX",          B8_APB_IOMUX_BASE_ADDR,          61,   0, NULL              },
    {"GPIO",           B8_APB_GPIO_BASE_ADDR,           12,   0, NULL              },
    {"IDC_0",          B8_APB_I2C0_BASE_ADDR,           16,   2, B8_I2cRegToSkip   },
    {"I2C_IOCTRL",     B8_APB_I2C_IOCTRL_BASE_ADDR,     1,    0, NULL              },
};


static void TestB8_PrintFormattedInt(const char *pFmtString, UINT32 Value, AMBA_SHELL_PRINT_f PrintFunc)
{
    char StrBuf[64];
    UINT32 ArgUINT32[2];

    ArgUINT32[0] = Value;
    (void)AmbaB8_Utility_StrPrintUInt32(StrBuf, sizeof(StrBuf), pFmtString, 1U, ArgUINT32);
    PrintFunc(StrBuf);
}

static void B8_Usage(AMBA_SHELL_PRINT_f PrintFunc)
{
    PrintFunc("Usage: b8 writew <chip_id> [ahb|apb] <addr>(+) data0 data1 ..\n");
    PrintFunc("Usage: b8 writeh <chip_id> [ahb|apb] <addr>(+) data0 data1 ..\n");
    PrintFunc("Usage: b8 writeb <chip_id> [ahb|apb] <addr>(+) data0 data1 ..\n");
    PrintFunc("Usage: b8 readw <chip_id> [ahb|apb] <addr>(+) <count>\n");
    PrintFunc("Usage: b8 readh <chip_id> [ahb|apb] <addr>(+) <count>\n");
    PrintFunc("Usage: b8 readb <chip_id> [ahb|apb] <addr>(+) <count>\n");
    PrintFunc("Usage: b8 dumpreg <chip_id> <flags>\n");
    PrintFunc("Usage: b8 vin <chip_id> <vin_id>\n");
    PrintFunc("Usage: b8 phy <chip_id> <cap> <res> <tap1> <tap2> <tap3> <tap4>\n");
    PrintFunc("Usage: b8 scan <chip_id> <serdes_rate>\n");
    PrintFunc("Usage: b8 init <chip_id> <serdes_rate> (<mipi_rate>)\n");
    PrintFunc("Usage: b8 config sensor (0:disable | 1:enable)\n");
    PrintFunc("Usage: b8 config serdes (0:disable | 1:enable)\n");

}

#ifdef DFE_SEARCH
static AMBA_KAL_TASK_t B8UT_Task = {0};
static UINT8           B8UT_Stack[4096] = {0};
static void B8UT_TestTask(UINT32 EntryArg)
{
    extern UINT32 AmbaB8_SerdesLocalAdaptHandler(UINT32 Timeout);
    extern UINT32 AmbaB8_SerdesMonitorHandler(UINT32 Timeout);
    AmbaB8_Misra_TouchUnused(&EntryArg);

    while(1) {
        AmbaB8_SerdesLocalAdaptHandler(AMBA_KAL_WAIT_FOREVER);
        AmbaKAL_TaskSleep(1);
    }
}
#endif

#ifdef B8_DEV_VERSION
/* debug purpose !!!TBD remove after */
static void B8_SerdesGlobalSearch(INT32 ArgCount, char * const * pArgVector, AMBA_SHELL_PRINT_f PrintFunc)
{
    extern UINT32 IterationOuter; /* = 400; */                /* Iteration for outer loop (for tuning) */
    extern UINT32 IterationInner; /* = 10;  */                /* Iteration for inner loop (for tuning) */
    extern DOUBLE WeightOuter;    /* = 11;  */                /* Weighting for outer loop (for tuning) */
    extern DOUBLE WeightInner;    /* = 4;   */                /* Weighting for inner loop (for tuning) */
    extern DOUBLE WeightT;        /* = 0.6; */                /* Weighting for T          (for tuning) */
    extern DOUBLE ExitEnergy;     /* = 0.0; */

    UINT32 ChipID;
    UINT32 SerDesRate;
    UINT32 TimeInterval = 500;
    UINT32 _WeightInner;
    UINT32 _WeightOuter;
    UINT32 _WeightT;    /* *100  = 0.6 */
    UINT32 _ExitEnergy; /* *1000000 = 0.001 */

    (void) PrintFunc;
    pCalibMemoryAddress = CalPool;
    if (ArgCount >= 4) {
        (void) AmbaB8_Utility_StringToUInt32(pArgVector[2], &ChipID);
        (void) AmbaB8_Utility_StringToUInt32(pArgVector[3], &SerDesRate);
#if (B8_DBG_TEMP)
        gB8DbgSerDesRate = SerDesRate;
#endif
        // testing
        if (ArgCount >= 5) (void) AmbaB8_Utility_StringToUInt32(pArgVector[4], &TimeInterval);
        if (ArgCount >= 6) (void) AmbaB8_Utility_StringToUInt32(pArgVector[5], &IterationOuter);
        if (ArgCount >= 7) (void) AmbaB8_Utility_StringToUInt32(pArgVector[6], &IterationInner);
        if (ArgCount >= 8) {
            (void)AmbaB8_Utility_StringToUInt32(pArgVector[7], &_WeightOuter);
            WeightOuter = _WeightOuter;
        }
        if (ArgCount >= 9) {
            (void)AmbaB8_Utility_StringToUInt32(pArgVector[8], &_WeightInner);
            WeightInner = _WeightInner;
        }
        if (ArgCount >= 10) {
            (void) AmbaB8_Utility_StringToUInt32(pArgVector[9], &_WeightT);
            WeightT = (DOUBLE)_WeightT / 100.;
        }
        if (ArgCount >= 11) {
            (void) AmbaB8_Utility_StringToUInt32(pArgVector[10], &_ExitEnergy);
            ExitEnergy = (DOUBLE)_ExitEnergy / 1000000.;
        }

#if 0
        if (pCalibMemoryAddress == NULL) {
            UINT32 FreeSpace = 0;

            AmbaKAL_BytePoolInfoGet(pCalibMemoryPool, &FreeSpace);
            if (FreeSpace > REQUIRED_CAL_MEMORY_SIZE) {
                (void) AmbaKAL_BytePoolAllocate(pCalibMemoryPool, &pCalibMemoryAddress, REQUIRED_CAL_MEMORY_SIZE, 100);
                (void) AmbaB8_SerdesGlobalCalibration(ChipID, SerDesRate, TimeInterval, pCalibMemoryAddress);
            } else {
                AmbaB8_PrintUInt5("Not enough Calib memory, need %d, left:%d", REQUIRED_CAL_MEMORY_SIZE, FreeSpace, 0U, 0U, 0U);
            }
        } else {
            (void) AmbaB8_SerdesGlobalCalibration(ChipID, SerDesRate, TimeInterval, pCalibMemoryAddress);
        }
#else
    (void) AmbaB8_SerdesGlobalCalibration(ChipID, SerDesRate, TimeInterval, pCalibMemoryAddress);
#endif
    }

}
#endif
static void B8_ParsingArgument(char * const * pArgVector, B8_ARGUMENT_s *pB8Arg)
{
    UINT32 ChipID;
    UINT32 Addr;
    UINT32 i = 0U;
    char Plus[8] = {'\0'};
    char AddrString[16] = {'\0'};

    /* Chip ID */
    (void) AmbaB8_Utility_StringToUInt32(pArgVector[2], &ChipID);
    pB8Arg->ChipID = ChipID;

    /* Address */
    while ((pArgVector[4][i] != '\0') && (pArgVector[4][i] != '+')) {
        AddrString[i] = pArgVector[4][i];
        i ++;
    }
    Plus[0] = pArgVector[4][i];

    (void) AmbaB8_Utility_StringToUInt32(AddrString, &Addr);

    if (Plus[0] == '\0') {
        pB8Arg->AddrInc = 0;
    } else if (AmbaB8_Utility_StringCompare(Plus, "+", 1U) == 0) {
        pB8Arg->AddrInc = 1;
    } else {
        AmbaB8_PrintStr5("Wrong Argument: \'%s\'", Plus, NULL, NULL, NULL, NULL);
    }
    /* Bus */
    if (AmbaB8_Utility_StringCompare(pArgVector[3], "ahb", 3U) == 0) {
        pB8Arg->RegAddr = B8_AHB_BASE_ADDR + Addr;
    } else if (AmbaB8_Utility_StringCompare(pArgVector[3], "apb", 3U) == 0) {
        pB8Arg->RegAddr = B8_APB_BASE_ADDR + Addr;
    } else {
        AmbaB8_PrintStr5("Wrong Argument: \'%s\'", pArgVector[3], NULL, NULL, NULL, NULL);
    }


    //AmbaB8_PrintUInt5("AddrInc: %d, ChipID: %d, Addr: 0x%x", pB8Arg->AddrInc, pB8Arg->ChipID, pB8Arg->RegAddr, 0U, 0U);

}

static void B8_WriteW(UINT32 ArgCount, char * const * pArgVector)
{
    B8_ARGUMENT_s pB8Arg;
    UINT32 i;
    UINT32 DataBuf[128U] = {0};
    UINT32 DataSize;

    B8_ParsingArgument(pArgVector, &pB8Arg);

    /* Data*/
    DataSize = ArgCount - 5U;
    for (i = 0U; i < DataSize; i++) {
        (void) AmbaB8_Utility_StringToUInt32(pArgVector[i + 5U], &DataBuf[i]);
    }
    /* Call Write function */
    (void) AmbaB8_RegWriteU32(pB8Arg.ChipID, pB8Arg.RegAddr, pB8Arg.AddrInc, B8_DATA_WIDTH_32BIT, DataSize, DataBuf);
}

static void B8_WriteH(UINT32 ArgCount, char * const * pArgVector)
{
    B8_ARGUMENT_s pB8Arg;
    UINT32 i;
    UINT16 DataBuf[128U] = {0};
    UINT32 DataSize;
    UINT32 Data;

    B8_ParsingArgument(pArgVector, &pB8Arg);
    /* Data*/
    DataSize = ArgCount - 5U;
    for (i = 0U; i < DataSize; i++) {
        (void) AmbaB8_Utility_StringToUInt32(pArgVector[i + 5U], &Data);
        DataBuf[i] = (UINT16)(Data & 0xffffU);
    }
    /* Call Write function */
    (void) AmbaB8_RegWriteU32(pB8Arg.ChipID, pB8Arg.RegAddr, pB8Arg.AddrInc, B8_DATA_WIDTH_16BIT, DataSize, DataBuf);
}

static void B8_WriteB(UINT32 ArgCount, char * const * pArgVector)
{
    B8_ARGUMENT_s pB8Arg;
    UINT32 i;
    UINT8 DataBuf[128U] = {0};
    UINT32 DataSize;
    UINT32 Data;

    B8_ParsingArgument(pArgVector, &pB8Arg);
    /* Data*/
    DataSize = ArgCount - 5U;
    for (i = 0U; i < DataSize; i++) {
        (void) AmbaB8_Utility_StringToUInt32(pArgVector[i + 5U], &Data);
        DataBuf[i] = (UINT8)(Data & 0xffU);
    }
    /* Call Write function */
    (void) AmbaB8_RegWriteU32(pB8Arg.ChipID, pB8Arg.RegAddr, pB8Arg.AddrInc, B8_DATA_WIDTH_8BIT, DataSize, DataBuf);
}

static void B8_ReadW(char * const * pArgVector)
{
    B8_ARGUMENT_s B8Arg;
    UINT32 i, DataSize;
    UINT32 AddrOffset = 0;
    UINT32 TestB8_DataBuf[4U][128U] = {0};
    UINT32 *DataBuf[4U] = {TestB8_DataBuf[0], TestB8_DataBuf[1], TestB8_DataBuf[2], TestB8_DataBuf[3]};
    void* pRxDataBuf[4] = {NULL};

    (void) AmbaB8_Wrap_memcpy(&pRxDataBuf[0], &DataBuf[0], sizeof(void*));
    (void) AmbaB8_Wrap_memcpy(&pRxDataBuf[1], &DataBuf[1], sizeof(void*));
    (void) AmbaB8_Wrap_memcpy(&pRxDataBuf[2], &DataBuf[2], sizeof(void*));
    (void) AmbaB8_Wrap_memcpy(&pRxDataBuf[3], &DataBuf[3], sizeof(void*));

    B8_ParsingArgument(pArgVector, &B8Arg);
    /* DataSize*/
    (void) AmbaB8_Utility_StringToUInt32(pArgVector[5], &DataSize);

    /* Call read function */
    (void) AmbaB8_RegReadU32(B8Arg.ChipID, B8Arg.RegAddr, B8Arg.AddrInc, B8_DATA_WIDTH_32BIT, DataSize, DataBuf[0]);
    for (i = 0U; i < DataSize; i ++) {
        AmbaB8_PrintUInt5("B8[0x%x] %04Xh: %08X", B8Arg.ChipID, B8Arg.RegAddr + AddrOffset, DataBuf[0][i], 0U, 0U);
        AddrOffset += ((UINT32)B8Arg.AddrInc << 2U);
    }
}

static void B8_ReadH(char * const * pArgVector)
{
    B8_ARGUMENT_s B8Arg;
    UINT32 i, DataSize;
    UINT32 AddrOffset = 0;
    UINT16 TestB8_DataBuf[4U][128U] = {0};
    UINT16 *DataBuf[4U] = {TestB8_DataBuf[0], TestB8_DataBuf[1], TestB8_DataBuf[2], TestB8_DataBuf[3]};
    void* pRxDataBuf[4] = {NULL};

    (void) AmbaB8_Wrap_memcpy(&pRxDataBuf[0], &DataBuf[0], sizeof(void*));
    (void) AmbaB8_Wrap_memcpy(&pRxDataBuf[1], &DataBuf[1], sizeof(void*));
    (void) AmbaB8_Wrap_memcpy(&pRxDataBuf[2], &DataBuf[2], sizeof(void*));
    (void) AmbaB8_Wrap_memcpy(&pRxDataBuf[3], &DataBuf[3], sizeof(void*));

    B8_ParsingArgument(pArgVector, &B8Arg);
    /* DataSize*/
    (void) AmbaB8_Utility_StringToUInt32(pArgVector[5], &DataSize);

    /* Call read function */
    (void) AmbaB8_RegReadU32(B8Arg.ChipID, B8Arg.RegAddr, B8Arg.AddrInc, B8_DATA_WIDTH_16BIT, DataSize, DataBuf[0]);
    for (i = 0U; i < DataSize; i ++) {
        AmbaB8_PrintUInt5("B8[0x%2X] %04Xh: %04X", B8Arg.ChipID, B8Arg.RegAddr + AddrOffset, DataBuf[0][i], 0U, 0U);
        AddrOffset += ((UINT32)B8Arg.AddrInc << 1U);
    }
}

static void B8_ReadB(char * const * pArgVector)
{
    B8_ARGUMENT_s B8Arg;
    UINT32 i, DataSize;
    UINT32 AddrOffset = 0;
    UINT8 TestB8_DataBuf[4U][128U] = {0};
    UINT8 *DataBuf[4U] = {TestB8_DataBuf[0], TestB8_DataBuf[1], TestB8_DataBuf[2], TestB8_DataBuf[3]};
    void* pRxDataBuf[4] = {NULL};

    (void) AmbaB8_Wrap_memcpy(&pRxDataBuf[0], &DataBuf[0], sizeof(void*));
    (void) AmbaB8_Wrap_memcpy(&pRxDataBuf[1], &DataBuf[1], sizeof(void*));
    (void) AmbaB8_Wrap_memcpy(&pRxDataBuf[2], &DataBuf[2], sizeof(void*));
    (void) AmbaB8_Wrap_memcpy(&pRxDataBuf[3], &DataBuf[3], sizeof(void*));

    B8_ParsingArgument(pArgVector, &B8Arg);
    /* DataSize*/
    (void) AmbaB8_Utility_StringToUInt32(pArgVector[5], &DataSize);

    /* Call read function */
    (void) AmbaB8_RegReadU32(B8Arg.ChipID, B8Arg.RegAddr, B8Arg.AddrInc, B8_DATA_WIDTH_8BIT, DataSize, DataBuf[0]);
    for (i = 0U; i < DataSize; i ++) {
        AmbaB8_PrintUInt5("B8[0x%2X] %04Xh: %02X", B8Arg.ChipID, B8Arg.RegAddr + AddrOffset, DataBuf[0][i], 0U, 0U);
        AddrOffset += (UINT32)B8Arg.AddrInc;
    }
}

static void B8_VinInfoParse(UINT32 ChipID, UINT32 VinID)
{
    UINT32 WData, RData[4] = {0}, i;
    UINT32 MaxNumCheck = 100;
    UINT8 RxValidHS = 0, RxActiveHS = 0, ClkActiveHs = 0;
    UINT32 VinBaseAddr = 0;
    /* VIN status */
    if (VinID == 0U) {
        VinBaseAddr = 0xe000c000U;
    } else {
        VinBaseAddr = 0xe000c400U;
    }

    WData = 0xffffffffU;
    (void) AmbaB8_RegWriteU32(ChipID, (VinBaseAddr + 0x9cU), 0, B8_DATA_WIDTH_32BIT, 1, &WData);
    (void) AmbaKAL_TaskSleep(300);
    (void) AmbaB8_RegReadU32(ChipID, (VinBaseAddr + 0x9cU), 0, B8_DATA_WIDTH_32BIT, 1, RData);
    AmbaB8_PrintUInt5("VIN[%d] status: 0x%08x\n", VinID, RData[0], 0U, 0U, 0U);

    AmbaB8_PrintUInt5(" GOT_ACT_SOF:\t\t\t%d\n", (RData[0] & 0x1U), 0U, 0U, 0U, 0U);
    AmbaB8_PrintUInt5(" STRIG0_STATUS:\t\t\t%d\n", (RData[0] >> 1U) & 0x1U, 0U, 0U, 0U, 0U);
    AmbaB8_PrintUInt5(" STRIG1_STATUS:\t\t\t%d\n", (RData[0] >> 2U) & 0x1U, 0U, 0U, 0U, 0U);
    AmbaB8_PrintUInt5(" SFIFO_OVERFLOW:\t\t%d\n", (RData[0] >> 3U) & 0x1U, 0U, 0U, 0U, 0U);
    AmbaB8_PrintUInt5(" SHORT_LINE:\t\t\t%d\n", (RData[0] >> 4U) & 0x1U, 0U, 0U, 0U, 0U);
    AmbaB8_PrintUInt5(" SHORT_FRAME:\t\t\t%d\n", (RData[0] >> 5U) & 0x1U, 0U, 0U, 0U, 0U);
    AmbaB8_PrintUInt5(" FIELD:\t\t\t\t%d\n", (RData[0] >> 6U) & 0x7U, 0U, 0U, 0U, 0U);
    AmbaB8_PrintUInt5(" SYNC_TIMEOUT:\t\t\t%d\n", (RData[0] >> 9U) & 0x1U, 0U, 0U, 0U, 0U);
    AmbaB8_PrintUInt5(" DBG_RESERVED0:\t\t\t%d\n", (RData[0] >> 10U) & 0x1U, 0U, 0U, 0U, 0U);
    AmbaB8_PrintUInt5(" GOT_VSYNC:\t\t\t%d\n", (RData[0] >> 11U) & 0x1U, 0U, 0U, 0U, 0U);
    AmbaB8_PrintUInt5(" SENT_MASTER_VSYNC:\t\t%d\n", (RData[0] >> 12U) & 0x1U, 0U, 0U, 0U, 0U);
    AmbaB8_PrintUInt5(" GOT_WIN_EOF:\t\t\t%d\n", (RData[0] >> 13U) & 0x1U, 0U, 0U, 0U, 0U);
    AmbaB8_PrintUInt5(" UNCORRECTABLE_656_ERROR:\t%d\n", (RData[0] >> 14U) & 0x1U, 0U, 0U, 0U, 0U);
    AmbaB8_PrintUInt5("=================================\n", 0U, 0U, 0U, 0U, 0U);

    (void) AmbaB8_RegReadU32(ChipID, VinBaseAddr, 0, B8_DATA_WIDTH_32BIT, 1, RData);
    AmbaB8_PrintUInt5("VIN[%d] Config: \n", VinID, 0U, 0U, 0U, 0U);
    AmbaB8_PrintUInt5(" WORD_SIZE:\t\t%d-bit\n", 8U + (2U * ((RData[0] >> 3U) & 0x7U)), 0U, 0U, 0U, 0U);
    (void) AmbaB8_RegReadU32(ChipID, VinBaseAddr + 0x24U, 1, B8_DATA_WIDTH_32BIT, 4, RData);
    AmbaB8_PrintUInt5(" CROP_REGION:\t\t{%d, %d, %d, %d}\n", RData[0], RData[1], RData[2], RData[3], 0U);

    /* MIPI PHY status */
    for (i = 0; i < MaxNumCheck; i ++) {
        (void) AmbaB8_RegReadU32(ChipID, 0xe0005430U, 0, B8_DATA_WIDTH_32BIT, 1, RData);
        ClkActiveHs |= (UINT8)((RData[0] >> 4U) & 0x3U);

        (void) AmbaB8_RegReadU32(ChipID, 0xe000543cU, 0, B8_DATA_WIDTH_32BIT, 1, RData);
        RxActiveHS |= (UINT8)((RData[0] >> 8U) & 0xffU);

        (void) AmbaB8_RegReadU32(ChipID, 0xe0005438U, 0, B8_DATA_WIDTH_32BIT, 1, RData);
        RxValidHS |= (UINT8)(RData[0] & 0xffU);

        /* wait for section selection */;
        (void) AmbaKAL_TaskSleep(2U);
    }

    AmbaB8_PrintUInt5("MIPI PHY status:\n", 0U, 0U, 0U, 0U, 0U);

    if ((ClkActiveHs & 1U) != 0U) {
        AmbaB8_PrintUInt5(" ClkActiveHs[0] check:\tOK\n", 0U, 0U, 0U, 0U, 0U);
    } else {
        AmbaB8_PrintUInt5(" ClkActiveHs[0] check:\tNG\n", 0U, 0U, 0U, 0U, 0U);
    }
    if ((ClkActiveHs & 2U) != 0U) {
        AmbaB8_PrintUInt5(" ClkActiveHs[1] check:\tOK\n", 0U, 0U, 0U, 0U, 0U);
    } else {
        AmbaB8_PrintUInt5(" ClkActiveHs[1] check:\tNG\n", 0U, 0U, 0U, 0U, 0U);
    }

    AmbaB8_PrintUInt5(" RxActiveHS check:\n", 0U, 0U, 0U, 0U, 0U);
    for (i = 0U; i < 8U; i ++) {
        if (((RxActiveHS >> i) & 0x1U) != 0U) {
            AmbaB8_PrintUInt5("     [%d]:\t\tOK\n", i, 0U, 0U, 0U, 0U);
        } else {
            AmbaB8_PrintUInt5("     [%d]:\t\tNG\n", i, 0U, 0U, 0U, 0U);
        }
    }

    AmbaB8_PrintUInt5(" RxValidHS check:\n", 0U, 0U, 0U, 0U, 0U);
    for (i = 0U; i < 8U; i ++) {
        if (((RxValidHS >> i) & 0x1U) != 0U) {
            AmbaB8_PrintUInt5("     [%d]:\t\tOK\n", i, 0U, 0U, 0U, 0U);
        } else {
            AmbaB8_PrintUInt5("     [%d]:\t\tNG\n", i, 0U, 0U, 0U, 0U);
        }
    }
}

static void B8_DumpReg(UINT32 ChipID, UINT32 RegAddr, UINT16 DataSize, UINT16 NumOfRegRangesToSkip, const B8_REG_RANGE_s *pRegRangesToSkip, AMBA_SHELL_PRINT_f PrintFunc)
{
    UINT16 i, k;
    UINT32 RData = 0U;

    for (i = 0U; i < DataSize; i++) {
        if (pRegRangesToSkip != NULL) {
            for (k = 0U; k < NumOfRegRangesToSkip; k++) {
                if ((i >= (pRegRangesToSkip[k].Offset >> 2U)) &&
                    (i < (UINT16)(((UINT32)pRegRangesToSkip[k].Offset + ((UINT32)pRegRangesToSkip[k].Count << 2U)) >> 2U))) {
                    if ((i & 0x3U) == 0x0U) {
                        TestB8_PrintFormattedInt("0x%x", RegAddr, PrintFunc);
                        PrintFunc(": <..skip..> ");
                    } else {
                        PrintFunc("<..skip..> ");
                    }
                    break;
                }
            }
            if (k == NumOfRegRangesToSkip) {
                if ((i & 0x3U) == 0x0U) {
                    (void) AmbaB8_RegReadU32(ChipID, RegAddr, 0, B8_DATA_WIDTH_32BIT, 1, &RData);
                    TestB8_PrintFormattedInt("0x%x", RegAddr, PrintFunc);
                    PrintFunc(": ");
                    TestB8_PrintFormattedInt("0x%x", RData, PrintFunc);
                    PrintFunc(" ");
                } else {
                    (void) AmbaB8_RegReadU32(ChipID, RegAddr, 0, B8_DATA_WIDTH_32BIT, 1, &RData);
                    TestB8_PrintFormattedInt("0x%x", RData, PrintFunc);
                    PrintFunc(" ");
                }
            }
        } else {
            if ((i & 0x3U) == 0x0U) {
                (void) AmbaB8_RegReadU32(ChipID, RegAddr, 0, B8_DATA_WIDTH_32BIT, 1, &RData);
                TestB8_PrintFormattedInt("0x%x", RegAddr, PrintFunc);
                PrintFunc(": ");
                TestB8_PrintFormattedInt("0x%x", RData, PrintFunc);
                PrintFunc(" ");
            } else {
                (void) AmbaB8_RegReadU32(ChipID, RegAddr, 0, B8_DATA_WIDTH_32BIT, 1, &RData);
                TestB8_PrintFormattedInt("0x%x", RData, PrintFunc);
                PrintFunc(" ");
            }
        }

        if ((i & 0x3U) == 0x3U) {
            PrintFunc("\n");
        }
        RegAddr += 4U;
    }

    if ((i & 0x3U) != 0x0U) {
        PrintFunc("\n");
    }
}


static void B8_DumpRegList(char * const * pArgVector, AMBA_SHELL_PRINT_f PrintFunc)
{
    UINT32 i;
    UINT32 ChipID;
    UINT32 Mask;

    (void) AmbaB8_Utility_StringToUInt32(pArgVector[2], &ChipID);
    (void) AmbaB8_Utility_StringToUInt32(pArgVector[3], &Mask);

    for (i = 0U; i < (sizeof(B8_RegDumpList) / sizeof((B8_RegDumpList)[0])); i++) {
        if ((Mask & ((UINT32)1U << i)) != 0U) {
            UINT32 RegAddr = B8_RegDumpList[i].StartAddr;
            UINT16 DataSize = B8_RegDumpList[i].NumOfRegs;
            UINT16 NumOfRegRangesToSkip = B8_RegDumpList[i].NumOfRegRangesToSkip;
            const B8_REG_RANGE_s *pRegRangesToSkip = B8_RegDumpList[i].pRegRangesToSkip;

            PrintFunc("ChipID: ");
            TestB8_PrintFormattedInt("0x%x", ChipID, PrintFunc);
            PrintFunc(", ");
            PrintFunc("Module name: [");
            PrintFunc(B8_RegDumpList[i].Name);
            PrintFunc("] \n");
            PrintFunc("-------------------------------------------------------\n");

            B8_DumpReg(ChipID, RegAddr, DataSize, NumOfRegRangesToSkip, pRegRangesToSkip, PrintFunc);

            PrintFunc("-------------------------------------------------------\n");
        }
    }
}
static void B8_GpioShowInfo(UINT32 GpioID, const B8_GPIO_PIN_INFO_s *pPinInfo, AMBA_SHELL_PRINT_f PrintFunc)
{
    TestB8_PrintFormattedInt(" [%d]:\t", GpioID, PrintFunc);
    if (pPinInfo->Config == B8_GPIO_CONFIG_ALTERNATE) {
        PrintFunc("ALT   ");
    } else if (pPinInfo->Config == B8_GPIO_CONFIG_OUTPUT) {
        PrintFunc("OUT   ");
    } else {
        PrintFunc("IN    ");
    }

    if (pPinInfo->Level == B8_GPIO_LEVEL_HIGH) {
        PrintFunc("HIGH\n");
    } else {
        PrintFunc("LOW\n");
    }
}

static void B8_GpioDump(UINT32 ChipID, UINT32 GpioID, AMBA_SHELL_PRINT_f PrintFunc)
{
    B8_GPIO_PIN_INFO_s PinInfo;
    UINT32 i;

    PrintFunc(" GPIO\tFUNC  LEVEL\n"
              "-------------------------\n");

    if (GpioID >= B8_NUM_GPIO_PIN) {
        for (i = 0U; i < B8_NUM_GPIO_PIN; i++) {
            if (AmbaB8_GpioGetPinInfo(ChipID, i, &PinInfo) == 0U) {
                B8_GpioShowInfo(i, &PinInfo, PrintFunc);
            }
        }
    } else {
        if (AmbaB8_GpioGetPinInfo(ChipID, GpioID, &PinInfo) == 0U) {
            B8_GpioShowInfo(GpioID, &PinInfo, PrintFunc);
        }
    }

    PrintFunc("--------------------------\n");
}

static void B8_GpioConfig(UINT32 ArgCount, char * const * pArgVector, AMBA_SHELL_PRINT_f PrintFunc)
{
    UINT32 ChipID, GpioID, PinLevel;

    if (ArgCount >= 5U) {
        (void) AmbaB8_Utility_StringToUInt32(pArgVector[2], &ChipID);

        if (AmbaB8_Utility_StringCompare(pArgVector[3], "show", 4U) == 0) {

            if (AmbaB8_Utility_StringCompare(pArgVector[4], "all", 3U) == 0) {
                B8_GpioDump(ChipID, B8_NUM_GPIO_PIN, PrintFunc);   /* Dump all pin info */
            } else {
                (void) AmbaB8_Utility_StringToUInt32(pArgVector[4], &GpioID);
                B8_GpioDump(ChipID, GpioID, PrintFunc);                 /* Dump single pin info */
            }
        } else {
            (void) AmbaB8_Utility_StringToUInt32(pArgVector[3], &GpioID);

            if (AmbaB8_Utility_StringCompare(pArgVector[4], "in", 2) == 0) {
                (void) AmbaB8_GpioSetInput(ChipID, GpioID);

            } else if ((AmbaB8_Utility_StringCompare(pArgVector[4], "out", 3) == 0) && (ArgCount >= 6U)) {
                (void) AmbaB8_Utility_StringToUInt32(pArgVector[5], &PinLevel);
                (void) AmbaB8_GpioSetOutput(ChipID, GpioID, PinLevel);

            } else {
                PrintFunc("Usage: b8 gpio <chip_id> [show|<pin_id>] [option]\n");
                PrintFunc("       b8 gpio <chip_id> show [all|<pin>] - show the GPIO pin status\n");
                PrintFunc("       b8 gpio <chip_id> <pin> in - set input mode\n");
                PrintFunc("       b8 gpio <chip_id> <pin> out [0|1] - output mode with logic low/high\n");
            }
        }
    } else {
        PrintFunc("Usage: b8 gpio <chip_id> [show|<pin_id>] [option]\n");
        PrintFunc("       b8 gpio <chip_id> show [all|<pin>] - show the GPIO pin status\n");
        PrintFunc("       b8 gpio <chip_id> <pin> in - set input mode\n");
        PrintFunc("       b8 gpio <chip_id> <pin> out [0|1] - output mode with logic low/high\n");
    }
}

#ifdef B8_DEV_VERSION
void AmbaB8_UTSetMemoryPool(AMBA_KAL_BYTE_POOL_t *pPool)
{
    pCalibMemoryPool = pPool;
}
#endif

void AmbaB8_UTCmd(UINT32 ArgCount, char * const * pArgVector, AMBA_SHELL_PRINT_f PrintFunc)
{
    if (ArgCount >= 1U) {
        if (AmbaB8_Utility_StringCompare(pArgVector[1], "writew", 6U) == 0) {
            /* Write Word */
            if (ArgCount >= 5U) {
                B8_WriteW(ArgCount, pArgVector);
            } else {
                PrintFunc("Usage: b8 writew <chip_id> [ahb|apb] <addr>(+) data0 data1 ..\n");
            }
        } else if (AmbaB8_Utility_StringCompare(pArgVector[1], "writeh", 6U) == 0) {
            /* Write Half-Word */
            if (ArgCount >= 5U) {
                B8_WriteH(ArgCount, pArgVector);
            } else {
                PrintFunc("Usage: b8 writeh <chip_id> [ahb|apb] <addr>(+) data0 data1 ..\n");
            }
        } else if (AmbaB8_Utility_StringCompare(pArgVector[1], "writeb", 6U) == 0) {
            /* Write Byte */
            if (ArgCount >= 5U) {
                B8_WriteB(ArgCount, pArgVector);
            } else {
                PrintFunc("Usage: b8 writeb <chip_id> [ahb|apb] <addr>(+) data0 data1 ..\n");
            }
        } else if (AmbaB8_Utility_StringCompare(pArgVector[1], "readw", 5U) == 0) {
            /* Read Word */
            if (ArgCount == 6U) {
                B8_ReadW(pArgVector);
            } else {
                PrintFunc("Usage: b8 readw <chip_id> [ahb|apb] <addr>(+) <count>\n");
            }
        } else if (AmbaB8_Utility_StringCompare(pArgVector[1], "readh", 5U) == 0) {
            /* Read Half-Word */
            if (ArgCount == 6U) {
                B8_ReadH(pArgVector);
            } else {
                PrintFunc("Usage: b8 readh <chip_id> [ahb|apb] <addr>(+) <count>\n");
            }
        } else if (AmbaB8_Utility_StringCompare(pArgVector[1], "readb", 5U) == 0) {
            /* Read Byte */
            if (ArgCount == 6U) {
                B8_ReadB(pArgVector);
            } else {
                PrintFunc("Usage: b8 readb <chip_id> [ahb|apb] <addr>(+) <count>\n");
            }
        } else if (AmbaB8_Utility_StringCompare(pArgVector[1], "vin", 3U) == 0) {
            if (ArgCount >= 4U) {
                UINT32 ChipID, VinID;

                (void) AmbaB8_Utility_StringToUInt32(pArgVector[2], &ChipID);
                (void) AmbaB8_Utility_StringToUInt32(pArgVector[3], &VinID);
                B8_VinInfoParse(ChipID, VinID);
            } else {
                PrintFunc("Usage: b8 vin <chip_id> <vin_id>\n");
            }

            /* register dump */
        } else if (AmbaB8_Utility_StringCompare(pArgVector[1], "dumpreg", 7U) == 0) {
            UINT32 i;

            if (ArgCount >= 4U) {
                B8_DumpRegList(pArgVector, PrintFunc);
            } else {
                PrintFunc("Usage: b8 dumpreg <chip_id> <flags>\n");
                PrintFunc("    flags: \n");
                for (i = 0U; i < (sizeof(B8_RegDumpList) / sizeof((B8_RegDumpList)[0])); i++) {
                    PrintFunc("        ");
                    TestB8_PrintFormattedInt("0x%x", (UINT32)1U << i, PrintFunc);

                    PrintFunc(": ");
                    PrintFunc(B8_RegDumpList[i].Name);
                    PrintFunc("\n");
                }
            }

            /* pll info */
        } else if (AmbaB8_Utility_StringCompare(pArgVector[1], "pll", 3U) == 0) {
            UINT32 ChipID;

            if (ArgCount >= 3U) {
                (void) AmbaB8_Utility_StringToUInt32(pArgVector[2], &ChipID);
                AmbaB8_PrintUInt5("CoreClk: \t%d", AmbaB8_PllGetCoreClk(ChipID), 0U, 0U, 0U, 0U);
                AmbaB8_PrintUInt5("IdspClk: \t%d", AmbaB8_PllGetIdspClk(ChipID), 0U, 0U, 0U, 0U);
                AmbaB8_PrintUInt5("SwphyClk: \t%d", AmbaB8_PllGetSwphyClk(ChipID), 0U, 0U, 0U, 0U);
                AmbaB8_PrintUInt5("SpiClk: \t%d", AmbaB8_PllGetSpiClk(ChipID), 0U, 0U, 0U, 0U);
                AmbaB8_PrintUInt5("SensorClk: \t%d", AmbaB8_PllGetSensorClk(ChipID), 0U, 0U, 0U, 0U);
                AmbaB8_PrintUInt5("Vout0Clk: \t%d", AmbaB8_PllGetVout0Clk(ChipID), 0U, 0U, 0U, 0U);
                AmbaB8_PrintUInt5("Vout1Clk: \t%d", AmbaB8_PllGetVout1Clk(ChipID), 0U, 0U, 0U, 0U);
            }

            /* gpio info */
        } else if (AmbaB8_Utility_StringCompare(pArgVector[1], "gpio", 4U) == 0) {
            B8_GpioConfig(ArgCount, pArgVector, PrintFunc);

            /* HSRX equalizer setup */
        } else if (AmbaB8_Utility_StringCompare(pArgVector[1], "phy", 3U) == 0) {
            GLOBAL_SEARCH_PARAM_s CalibParam = {0};
            UINT32 ChipID;

            if (ArgCount >= 9U) {
                (void) AmbaB8_Utility_StringToUInt32(pArgVector[2], &ChipID);
                (void) AmbaB8_Utility_StringToUInt32(pArgVector[3], &CalibParam.Cap);
                (void) AmbaB8_Utility_StringToUInt32(pArgVector[4], &CalibParam.Res);
                (void) AmbaB8_Utility_StringToUInt32(pArgVector[5], &CalibParam.Tap1);
                (void) AmbaB8_Utility_StringToUInt32(pArgVector[6], &CalibParam.Tap2);
                (void) AmbaB8_Utility_StringToUInt32(pArgVector[7], &CalibParam.Tap3);
                (void) AmbaB8_Utility_StringToUInt32(pArgVector[8], &CalibParam.Tap4);

                if ((ChipID & B8_MAIN_CHIP_ID_B8ND_MASK) != 0U) {
                    (void) AmbaB8D_SerdesSetGlobalCalParam(ChipID, &CalibParam);
                } else {
                    (void) AmbaB8_SerdesSetCalibParam(ChipID, &CalibParam);
                }
            } else {
                PrintFunc("Usage: t b8 phy <chip_id> <cap> <res> <tap1> <tap2> <tap3> <tap4>\n");
            }
#ifdef B8_DEV_VERSION
        } else if (AmbaB8_Utility_StringCompare(pArgVector[1], "sweep", 5U) == 0) {

            UINT32 ChipID, SerDesRate, TimeInterval;

            if (ArgCount >= 5U) {

                (void) AmbaB8_Utility_StringToUInt32(pArgVector[2], &ChipID);
                (void) AmbaB8_Utility_StringToUInt32(pArgVector[3], &SerDesRate);
                (void) AmbaB8_Utility_StringToUInt32(pArgVector[4], &TimeInterval);
#if (B8_DBG_TEMP)
                gB8DbgSerDesRate = SerDesRate;
#endif
                AmbaB8_SerdesSweepCalibration(ChipID, SerDesRate, TimeInterval);
            } else {
                PrintFunc("Usage: b8 sweep <chip_id> <serdes_rate> <TimeInterval> \n");
                PrintFunc("e.g. b8 sweep 0x11 0 500\n");
            }
            /* global search for HSRX equalizer */
        } else if (AmbaB8_Utility_StringCompare(pArgVector[1], "scan", 4U) == 0) {
            if (ArgCount >= 4) {
                B8_SerdesGlobalSearch(ArgCount, pArgVector, PrintFunc);
            } else {
                PrintFunc("Usage: t b8 scan <chip_id> <serdes_rate>\n");
                PrintFunc("       -serdes_rate:\n");
                PrintFunc("         0: 2P30G\n");
                PrintFunc("         1: 2P59G\n");
                PrintFunc("         2: 3P02G\n");
                PrintFunc("         3: 3P45G\n");
                PrintFunc("         4: 3P60G\n");
                PrintFunc("         5: 3P74G\n");
                PrintFunc("         6: 3P88G\n");
                PrintFunc("         7: 4P03G\n");
                PrintFunc("         8: 4P17G\n");
                PrintFunc("         9: 4P32G\n");
                PrintFunc("        10: 4P46G\n");
                PrintFunc("        11: 5P04G\n");
                PrintFunc("        12: 5P47G\n");
                PrintFunc("        13: 5P76G\n");
                PrintFunc("        14: 6P04G\n");
                PrintFunc("Usage: b8 scan <chip_id> <serdes_rate> (<TimeInterval> <IterationOuter> <IterationInner> <WeightOuter> <WeightInner> <WeightT*100> <ExitEnergy*10^6>)\n");
                PrintFunc("e.g. t b8 scan 0x11 0 500 400 10 11 4 60 10\n");
            }
#endif
            /* linear search for HSRX equalizer */
#ifdef B8_DEV_VERSION
        } else if (AmbaB8_Utility_StringCompare(pArgVector[1], "qscan", 5U) == 0) {
            if (ArgCount >= 4) {
                UINT32 ChipID, SamplePeriod;
                GLOBAL_SEARCH_PARAM_s CalibParam;
                (void) AmbaB8_Utility_StringToUInt32(pArgVector[2], &ChipID);
                (void) AmbaB8_Utility_StringToUInt32(pArgVector[3], &SamplePeriod);
                AmbaB8_SerdesCalibration(ChipID, SamplePeriod, &CalibParam);
            } else {
                PrintFunc("Usage: t b8 qscan <chip_id> <sample_time>");
            }

        } else if (AmbaB8_Utility_StringCompare(pArgVector[1], "tap", 3U) == 0) {
            if (ArgCount >= 3) {
                UINT32 ChipID;

                (void) AmbaB8_Utility_StringToUInt32(pArgVector[2], &ChipID);

                AmbaB8_SerdesHsRxDfeStatus(ChipID);
            } else {
                PrintFunc("Usage: t b8 tap <chip_id> ");
            }
#endif
            /* B8 initialization */
        } else if (AmbaB8_Utility_StringCompare(pArgVector[1], "init", 4U) == 0) {
            if (ArgCount >= 3U) {
                UINT32 ChipID;
#ifdef BUILT_IN_SERDES
                extern void AmbaSERDES_SetLinkStatus(UINT32 ChipID, UINT32 Status);
#endif

                (void) AmbaB8_Utility_StringToUInt32(pArgVector[2], &ChipID);
#if (B8_DBG_TEMP)
                if (ArgCount >= 4U) {
                    (void) AmbaB8_Utility_StringToUInt32(pArgVector[3], &gB8DbgSerDesRate);
                }
#endif
                /* Semaphore and Mutex initialization */
                (void) AmbaB8_CommInit();
                (void) AmbaB8_I2cInit();
                (void) AmbaB8_SpiInit();

                /* PLL initialization */
                if ((ChipID & B8_SUB_CHIP_ID_MASK) != 0U) {
                    (void) AmbaB8_PllInit(ChipID, 72000000U);
                }
                if ((ChipID & B8_MAIN_CHIP_ID_B8N_MASK) != 0U) {
                    (void) AmbaB8_PllInit((ChipID & B8_MAIN_CHIP_ID_B8N_MASK), 72000000U);
                }
#ifdef BUILT_IN_SERDES
                if ((ChipID & B8_MAIN_CHIP_ID_BUILT_IN_B8NF) != 0U) {
                    AmbaSERDES_SetLinkStatus(0xffU, 1U);
                }
#endif

            } else {
                PrintFunc("Usage: b8 init <chip_id> (<serdes_rate>)\n");
                PrintFunc("       -serdes_rate:\n");
                PrintFunc("         0: 2P30G\n");
                PrintFunc("         1: 2P59G\n");
                PrintFunc("         2: 3P02G\n");
                PrintFunc("         3: 3P45G\n");
                PrintFunc("         4: 3P60G\n");
                PrintFunc("         5: 3P74G\n");
                PrintFunc("         6: 3P88G\n");
                PrintFunc("         7: 4P03G\n");
                PrintFunc("         8: 4P17G\n");
                PrintFunc("         9: 4P32G\n");
                PrintFunc("        10: 4P46G\n");
                PrintFunc("        11: 5P04G\n");
                PrintFunc("        12: 5P47G\n");
                PrintFunc("        13: 5P76G\n");
                PrintFunc("        14: 6P04G\n");
            }

            /* B8 serdes link */
        } else if (AmbaB8_Utility_StringCompare(pArgVector[1], "link", 4U) == 0) {
            UINT32 ChipID = 0x11;
            UINT32 SerDesRate = 0U;
#ifdef BUILT_IN_SERDES
            extern void AmbaSERDES_SetLinkStatus(UINT32 ChipID, UINT32 Status);
#endif
            if (ArgCount >= 3U) {
                (void) AmbaB8_Utility_StringToUInt32(pArgVector[2], &ChipID);
#if (B8_DBG_TEMP)
                if (ArgCount >= 4U) {
                    (void) AmbaB8_Utility_StringToUInt32(pArgVector[3], &SerDesRate);
                    gB8DbgSerDesRate = SerDesRate;
                }
#endif

                /* Semaphore and Mutex init */
                (void) AmbaB8_CommInit();
                (void) AmbaB8_I2cInit();
                (void) AmbaB8_SpiInit();

                /* PLL initialization */
                (void) AmbaB8_PllInit(ChipID, 72000000U); /* B8F PLL */
                if ((ChipID & B8_MAIN_CHIP_ID_B8NF_MASK) != 0U) {
                    (void) AmbaB8_PllInit((ChipID & B8_MAIN_CHIP_ID_B8NF_MASK), 72000000U); /* B8N PLL */
                }

                (void) AmbaB8_SerdesLink(ChipID, SerDesRate);
            } else {
                PrintFunc("Usage: b8 link <chip_id> (<serdes_rate>)\n");
                PrintFunc("       -serdes_rate:\n");
                PrintFunc("         0: 2P30G\n");
                PrintFunc("         1: 2P59G\n");
                PrintFunc("         2: 3P02G\n");
                PrintFunc("         3: 3P45G\n");
                PrintFunc("         4: 3P60G\n");
                PrintFunc("         5: 3P74G\n");
                PrintFunc("         6: 3P88G\n");
                PrintFunc("         7: 4P03G\n");
                PrintFunc("         8: 4P17G\n");
                PrintFunc("         9: 4P32G\n");
                PrintFunc("        10: 4P46G\n");
                PrintFunc("        11: 5P04G\n");
                PrintFunc("        12: 5P47G\n");
                PrintFunc("        13: 5P76G\n");
                PrintFunc("        14: 6P04G\n");
            }
            /* B8 packer pattern generation mode */
        } else if (AmbaB8_Utility_StringCompare(pArgVector[1], "testmode", 8U) == 0) {
            if (ArgCount >= 3U) {
                UINT32 ChipID;
                (void) AmbaB8_Utility_StringToUInt32(pArgVector[2], &ChipID);
                (void) AmbaB8_PackerTestMode(ChipID);

            } else {
                PrintFunc("Usage: b8 testmode <chip_id> \n");
            }

            /* B8 depacker statistics data */
        } else if (AmbaB8_Utility_StringCompare(pArgVector[1U], "stat", 4U) == 0) {
            UINT32 ChipID;
            UINT32 RData[3] = {0};

            if (ArgCount >= 3U) {
                (void) AmbaB8_Utility_StringToUInt32(pArgVector[2U], &ChipID);

                if ((ChipID & B8_MAIN_CHIP_ID_B8NF_MASK) != 0U) {
                    ChipID = ChipID & B8_MAIN_CHIP_ID_B8NF_MASK;
                }

                if (ArgCount == 3U) {
                    RData[0] = AmbaB8_DepackerGetPassPkt(ChipID);
                    RData[1] = AmbaB8_DepackerGetErrorPkt(ChipID);
                    RData[2] = AmbaB8_DepackerGetCorrectedPkt(ChipID);

                    AmbaB8_PrintUInt5("Pass Pkt: \t\t%u\n", RData[0], 0U, 0U, 0U, 0U);
                    AmbaB8_PrintUInt5("Err Pkt: \t%u\n", RData[1], 0U, 0U, 0U, 0U);
                    AmbaB8_PrintUInt5("Corrected Pkt: \t%u\n", RData[2], 0U, 0U, 0U, 0U);

                } else { /*if (ArgCount >= 4U) { */
                    if (AmbaB8_Utility_StringCompare(pArgVector[3U], "reset", 4U) == 0) {
                        AmbaB8_DepackerResetPktCounter(ChipID);
                    } else {
                        UINT32 Interval = 0U;

                        (void) AmbaB8_Utility_StringToUInt32(pArgVector[3], &Interval);

                        AmbaB8_DepackerResetPktCounter(ChipID);

                        RData[0] = AmbaB8_DepackerGetPassPkt(ChipID);
                        RData[1] = AmbaB8_DepackerGetErrorPkt(ChipID);
                        RData[2] = AmbaB8_DepackerGetCorrectedPkt(ChipID);

                        (void) AmbaKAL_TaskSleep(Interval);

                        RData[0] = AmbaB8_DepackerGetPassPkt(ChipID) - RData[0];
                        RData[1] = AmbaB8_DepackerGetErrorPkt(ChipID) - RData[1];
                        RData[2] = AmbaB8_DepackerGetCorrectedPkt(ChipID) - RData[2];

                        AmbaB8_PrintUInt5("Accumulated packet statistics in %d msec\n", Interval, 0U, 0U, 0U, 0U);
                        AmbaB8_PrintUInt5("Pass Pkt: \t%u\n", RData[0], 0U, 0U, 0U, 0U);
                        AmbaB8_PrintUInt5("Err Pkt: \t%u\n", RData[1], 0U, 0U, 0U, 0U);
                        AmbaB8_PrintUInt5("Corrected Pkt: \t%u\n", RData[2], 0U, 0U, 0U, 0U);
                    }
                }
            } else {
                PrintFunc("Usage: t b8 stat <chip_id>\t\t\t\t- show current packet statistics counter\n");
                PrintFunc("Usage: t b8 stat <chip_id> reset\t\t\t- reset packet statistics counter\n");
                PrintFunc("Usage: t b8 stat <chip_id> <time_interval>\t\t- monitor packet statistics during certain time interval\n");
            }

#ifdef PHY_SHMOO
        } else if (AmbaB8_Utility_StringCompare(pArgVector[1U], "shmoo", 5U) == 0) {
            extern void AmbaB8_SerdesLsShmoo(UINT32 ChipID, UINT32 Mode, const char *pFileName);
            UINT32 ChipID;
            UINT32 Mode;
            CHAR FileName[64];

            if (ArgCount >= 5) {
                (void) AmbaB8_Utility_StringToUInt32(pArgVector[2U], &ChipID);
                (void) AmbaB8_Utility_StringToUInt32(pArgVector[3U], &Mode);
                strcpy(&FileName[0], "c:\\");
                strcat(&FileName[0], pArgVector[4U]);
                strcat(&FileName[0], ".txt");

                AmbaB8_SerdesLsShmoo(ChipID, Mode, &FileName[0]);

            } else {
                PrintFunc("Usage: t b8 shmoo <chip_id> <mode> <filename>\n");
                PrintFunc("         - mode: \n");
                PrintFunc("              0: PRE-LSTX shmoo(LS_LINK)\n");
                PrintFunc("              1: POS-LSTX shmoo(LS_LINK)\n");
                PrintFunc("              2: POS-LSTX shmoo(LS_LINK/HS_LINK)\n");
                PrintFunc("              3: LSRX shmoo(LS_LINK)\n");
            }
#endif
        } else if (AmbaB8_Utility_StringCompare(pArgVector[1U], "commrst", 7U) == 0) {
            UINT32 ChipID;

            if (ArgCount >= 3U) {
                (void) AmbaB8_Utility_StringToUInt32(pArgVector[2U], &ChipID);
                (void) AmbaB8_CommReset(ChipID);
            } else {
                PrintFunc("Usage: t b8 commrst <chip_id> \n");
            }

        } else if (AmbaB8_Utility_StringCompare(pArgVector[1U], "swphyclk", 8U) == 0) {
            extern UINT32 SwphyClk;
            UINT32 ChipID, DivVal;

            if (ArgCount >= 4U) {
                (void) AmbaB8_Utility_StringToUInt32(pArgVector[2U], &ChipID);
                (void) AmbaB8_Utility_StringToUInt32(pArgVector[3U], &SwphyClk);

                if ((ChipID & B8_MAIN_CHIP_ID_BUILT_IN_MASK) != 0U) {
                    if (312000000U >= SwphyClk) {
                        DivVal = (312000000U / SwphyClk) & 0x3ffU;
                    } else {
                        DivVal = 1U;
                    }
                } else {
                    if (312000000U >= SwphyClk) {
                        DivVal = (312000000U / SwphyClk) & 0x3ffU;
                    } else {
                        DivVal = 1U;
                    }
                }
                AmbaB8_PrintUInt5("set gclk_swphy = %d (Hz), actual frequency = %d (Hz)\n", SwphyClk, (312000000U / DivVal), 0U, 0U, 0U);
            } else {
                PrintFunc("USAGE: t b8 swphyclk <chip_id> <frequency>\n");
                PrintFunc("       - lstx clk setup\n\n");
            }

        }  else if (AmbaB8_Utility_StringCompare(pArgVector[1], "calib", 5U) == 0) {
            UINT32 SearchIndex = 0U;

            if ( (AmbaB8_Utility_StringCompare(pArgVector[2U], "hstx", 4U) == 0) && (ArgCount >= 5U)) {
                extern UINT32 HsTxBias[2U];
                if (AmbaB8_Utility_StringCompare(pArgVector[3U], "gs", 2U) == 0) {
                    SearchIndex = 0U;
                } else { /* if (AmbaB8_Utility_StringCompare(pArgVector[3U], "ls", 2U) == 0) { */
                    SearchIndex = 1U;
                }
                (void) AmbaB8_Utility_StringToUInt32(pArgVector[4U], &HsTxBias[SearchIndex]);
                HsTxBias[SearchIndex] &= 0xffU;

            } else if ((AmbaB8_Utility_StringCompare(pArgVector[2U], "sel", 3U) == 0) && (ArgCount >= 8U)) {
                extern SERDES_AUTO_CAL_SEL_s AutoCalSelect[2U];
                UINT32 BitVal;

                if (AmbaB8_Utility_StringCompare(pArgVector[3U], "gs", 2U) == 0) {
                    SearchIndex = 0U;
                } else { /* if (AmbaB8_Utility_StringCompare(pArgVector[3U], "ls", 2U) == 0) { */
                    SearchIndex = 1U;
                }
                (void) AmbaB8_Utility_StringToUInt32(pArgVector[4U], &BitVal);
                AutoCalSelect[SearchIndex].EqVcmCal = (UINT8)(BitVal & (UINT32)0x1);
                (void) AmbaB8_Utility_StringToUInt32(pArgVector[5U], &BitVal);
                AutoCalSelect[SearchIndex].CtleVcmCal = (UINT8)(BitVal & (UINT32)0x1);
                (void) AmbaB8_Utility_StringToUInt32(pArgVector[6U], &BitVal);
                AutoCalSelect[SearchIndex].SlicerOffsetCal = (UINT8)(BitVal & (UINT32)0x1);
                (void) AmbaB8_Utility_StringToUInt32(pArgVector[7U], &BitVal);
                AutoCalSelect[SearchIndex].CtleOffsetCal = (UINT8)(BitVal & (UINT32)0x1);

            } else if ((AmbaB8_Utility_StringCompare(pArgVector[2U], "time", 4U) == 0)  && (ArgCount >= 4U)) {
                extern UINT32 AutoCalTimeInMs[4U];
                (void) AmbaB8_Utility_StringToUInt32(pArgVector[3U], &AutoCalTimeInMs[0]);
                (void) AmbaB8_Utility_StringToUInt32(pArgVector[4U], &AutoCalTimeInMs[1]);
                (void) AmbaB8_Utility_StringToUInt32(pArgVector[5U], &AutoCalTimeInMs[2]);
                (void) AmbaB8_Utility_StringToUInt32(pArgVector[6U], &AutoCalTimeInMs[3]);

            } else if ((AmbaB8_Utility_StringCompare(pArgVector[2U], "prelstx", 7U) == 0) && (ArgCount >= 4U)) {
                extern UINT32 prelstx;
                (void) AmbaB8_Utility_StringToUInt32(pArgVector[3U], &prelstx);

            } else if ((AmbaB8_Utility_StringCompare(pArgVector[2U], "poslstx", 7U) == 0) && (ArgCount >= 4U)) {
                extern UINT32 poslstx;
                (void) AmbaB8_Utility_StringToUInt32(pArgVector[3U], &poslstx);

            } else if ((AmbaB8_Utility_StringCompare(pArgVector[2U], "lsrx", 4U) == 0) && (ArgCount >= 4U)) {
                extern UINT32 Reg5288LsRx;
                (void) AmbaB8_Utility_StringToUInt32(pArgVector[3U], &Reg5288LsRx);

            } else if ((AmbaB8_Utility_StringCompare(pArgVector[2U], "lsrxc", 5U) == 0)  && (ArgCount >= 4U)) {
                extern UINT32 Reg5288LsRx;
                UINT32 lsrxc; //reg5288[23:20]: ls_rx_c
                (void) AmbaB8_Utility_StringToUInt32(pArgVector[3U], &lsrxc);
                Reg5288LsRx = (Reg5288LsRx & (UINT32)0xFF0FFFFFU) | ((lsrxc & (UINT32)0xfU) << 20U);

            } else if ((AmbaB8_Utility_StringCompare(pArgVector[2U], "lsrxctr", 7U) == 0) && (ArgCount >= 4U)) {
                extern UINT32 Reg5288LsRx;
                UINT32 lsrxctr;//reg5288[19:12]: ls_rx_ctr
                (void) AmbaB8_Utility_StringToUInt32(pArgVector[3U], &lsrxctr);
                Reg5288LsRx = (Reg5288LsRx & (UINT32)0xFFF00FFFU) | ((lsrxctr & (UINT32)0xffU) << 12U);

            } else if ((AmbaB8_Utility_StringCompare(pArgVector[2U], "lsrxr", 5U) == 0) && (ArgCount >= 4U)) {
                extern UINT32 Reg5288LsRx;
                UINT32 lsrxr;//reg5288[11:8]: ls_rx_r
                (void) AmbaB8_Utility_StringToUInt32(pArgVector[3U], &lsrxr);
                Reg5288LsRx = (Reg5288LsRx & (UINT32)0xFFFFF0FFU) | ((lsrxr & (UINT32)0xfU) << 8U);
            } else if ((AmbaB8_Utility_StringCompare(pArgVector[2U], "term", 4U) == 0) && (ArgCount >= 4U)) {
                extern UINT32 TxTermAutoEnable;
                extern UINT32 RxTermAutoEnable;
                if (ArgCount >= 4U) {
                    (void) AmbaB8_Utility_StringToUInt32(pArgVector[3U], &TxTermAutoEnable);
                }
                if (ArgCount >= 5U) {
                    (void) AmbaB8_Utility_StringToUInt32(pArgVector[4U], &RxTermAutoEnable);
                }

            } else if ((AmbaB8_Utility_StringCompare(pArgVector[2U], "man", 3U) == 0) && (ArgCount >= 10U)) {

                /* Device auto calibration parameters */
                extern UINT32 VcmEqBufferCtrl;
                extern UINT32 VcmCtleCtrl;
                extern UINT32 VosCtleCtrl;
                extern UINT32 VosSlicerDat1Ctrl;
                extern UINT32 VosSlicerDat0Ctrl;
                extern UINT32 VosSlicerEdg1Ctrl;
                extern UINT32 VosSlicerEdg0Ctrl;

                if (ArgCount >= 4U) {
                    (void) AmbaB8_Utility_StringToUInt32(pArgVector[3U], &VcmEqBufferCtrl);
                }
                if (ArgCount >= 5U) {
                    (void) AmbaB8_Utility_StringToUInt32(pArgVector[4U], &VcmCtleCtrl);
                }
                if (ArgCount >= 6U) {
                    (void) AmbaB8_Utility_StringToUInt32(pArgVector[5U], &VosCtleCtrl);
                }
                if (ArgCount >= 7U) {
                    (void) AmbaB8_Utility_StringToUInt32(pArgVector[6U], &VosSlicerDat1Ctrl);
                }
                if (ArgCount >= 8U) {
                    (void) AmbaB8_Utility_StringToUInt32(pArgVector[7U], &VosSlicerDat0Ctrl);
                }
                if (ArgCount >= 9U) {
                    (void) AmbaB8_Utility_StringToUInt32(pArgVector[8U], &VosSlicerEdg1Ctrl);
                }
                if (ArgCount >= 10U) {
                    (void) AmbaB8_Utility_StringToUInt32(pArgVector[9U], &VosSlicerEdg0Ctrl);
                }

            } else if ((AmbaB8_Utility_StringCompare(pArgVector[2U], "lpgain", 6U) == 0) && (ArgCount >= 4U)) {
                extern UINT32 LpgainVcm;
                (void) AmbaB8_Utility_StringToUInt32(pArgVector[3U], &LpgainVcm);

            } else if ((AmbaB8_Utility_StringCompare(pArgVector[2U], "autoloop", 8U) == 0) && (ArgCount >= 4U)) {
                extern UINT32 AutoCalibNumbers;
                (void) AmbaB8_Utility_StringToUInt32(pArgVector[3U], &AutoCalibNumbers);
            } else if ((AmbaB8_Utility_StringCompare(pArgVector[2U], "autoths", 7U) == 0) && (ArgCount >= 4U)) {
                extern UINT32 AutoCalibThreshold;
                (void) AmbaB8_Utility_StringToUInt32(pArgVector[3U], &AutoCalibThreshold);

            } else {
                PrintFunc("USAGE: t b8 calib hstx <gs/ls> <hstx_bias>\n");
                PrintFunc("       - gs/ls: gs: global search, ls: loacl search\n");
                PrintFunc("       - hstx_bias: assign hstx[7:0] to the following link\n\n");

                PrintFunc("       t b8 calib poslstx <ls_amp>\n");
                PrintFunc("       - low speed amplitude after hs-link establishment\n\n");

                PrintFunc("       t b8 calib prelstx <ls_amp>\n");
                PrintFunc("       - low speed amplitude before hs-link establishment\n\n");

                PrintFunc("       t b8 calib lsrx <val_reg_5288>\n");
                PrintFunc("       - low speed rx setting\n\n");

                PrintFunc("       t b8 calib sel <gs/ls> <eq_vcm> < ctle_vcm> <slicer_offset> <ctle_offset>\n");
                PrintFunc("       - gs/ls: gs: global search, ls: loacl search\n");
                PrintFunc("       - others: 0: auto mode    , 1: manual mode\n\n");

                PrintFunc("       t b8 calib time <eq_vcm_time> < ctle_vcm_time> <slicer_offset_time> <ctle_offset_time>\n");
                PrintFunc("       - *_*_time: amount of auto calibration time in millisecond\n\n");

                PrintFunc("       t b8 calib term <tx_auto_term> <rx_auto_term>\n");
                PrintFunc("       - 1: enable; 0: disable auto temination calibration\n\n");

                PrintFunc("       t b8 calib term man <vcm_databuf_ctrl> <vcm_ctle_ctrl> <offset_ctle_code> <code_in_data1> <code_in_data0> <code_in_edge1> <code_in_edge0>\n\n");

            }
#ifdef VLSI_DEBUG
        } else if (AmbaB8_Utility_StringCompare(pArgVector[1], "upload", 6U) == 0) {
            extern void B8_UploadCommand(int Argc, char * const * Argv);
            extern UINT32 SerdesTestDebug;
            extern UINT32 MemLoadTraining;

            if (ArgCount >= 3 && (AmbaB8_Utility_StringCompare(pArgVector[2], "enable", 6U) == 0)) {
                if (ArgCount >= 4) {
                    (void) AmbaB8_Utility_StringToUInt32(pArgVector[3], &MemLoadTraining);

                } else {
                    PrintFunc("Usage: b8 upload enable [1|0]\n");
                }
            } else if (ArgCount >= 3 && (AmbaB8_Utility_StringCompare(pArgVector[2], "dbg", 3U) == 0)) {
                if (ArgCount >= 4) {
                    (void) AmbaB8_Utility_StringToUInt32(pArgVector[3], &SerdesTestDebug);
                } else {
                    PrintFunc("Usage: b8 upload dbg [1|0]\n");
                }
            } else {
                B8_UploadCommand(ArgCount, pArgVector);
            }
#endif


#if (B8_DBG_TEMP)
            /* hook sensor/serdes link config */
        } else if (AmbaB8_Utility_StringCompare(pArgVector[1], "config", 6U) == 0) {
            if (ArgCount >= 4U) {
                if (AmbaB8_Utility_StringCompare(pArgVector[2], "serdes", 6U) == 0) {
                    (void) AmbaB8_Utility_StringToUInt32(pArgVector[3], &gB8DbgConfig);
                    if (!gB8DbgConfig) PrintFunc("Disable serdes Config\n");

                } else if (AmbaB8_Utility_StringCompare(pArgVector[2], "sensorid", 8U) == 0) {
                    (void) AmbaB8_Utility_StringToUInt32(pArgVector[3], &gB8DbgSensorID);

                } else if (AmbaB8_Utility_StringCompare(pArgVector[2], "sensor", 6U) == 0) {
                    (void) AmbaB8_Utility_StringToUInt32(pArgVector[3], &gB8DbgSensorConfig);
                    if (!gB8DbgSensorConfig) PrintFunc("Disable sensor Config\n");

                } else if (AmbaB8_Utility_StringCompare(pArgVector[2], "vin", 3U) == 0) {
                    (void) AmbaB8_Utility_StringToUInt32(pArgVector[3], &gB8DbgVinConfig);
                    if (!gB8DbgVinConfig) PrintFunc("Disable vin Config\n");
                }
            } else {
                PrintFunc("Usage: b8 config serdes (0:disable | 1:enable)\n");
                PrintFunc("Usage: b8 config sensor (0:disable | 1:enable)\n");
                PrintFunc("Usage: b8 config vin    (0:disable | 1:enable)\n");
                PrintFunc("Usage: b8 config sensorid <sensor_id>\n");
            }

            /* register write command debug */
        } else if (AmbaB8_Utility_StringCompare(pArgVector[1U], "dbg", 3U) == 0) {
            if (ArgCount >= 3) {
                (void) AmbaB8_Utility_StringToUInt32(pArgVector[2U], &gB8DbgShowWCmd);
            } else {
                PrintFunc("USAGE: t b8 dbg <display_write_cmd>\n");
                PrintFunc("       - display_write_cmd: 0:no debug msg, 1:show debug msg\n");
            }

        } else if (AmbaB8_Utility_StringCompare(pArgVector[1U], "altvin", 6U) == 0) {
            if (ArgCount >= 3) {
                (void) AmbaB8_Utility_StringToUInt32(pArgVector[2U], &gB8DbgVinAltID);
            } else {
                PrintFunc("Usage: t b8 vinalt <vin_alt_id>\n");
            }
#endif

            /* B8D config */
        } else if (AmbaB8_Utility_StringCompare(pArgVector[1U], "b8dinit", 7U) == 0) {
            (void) AmbaB8D_SerdesLink(0x100d, B8_SERDES_RATE_2P3G);
#ifdef CONFIG_LCD_B8D_Y85006
        } else if (AmbaB8_Utility_StringCompare(pArgVector[1U], "b8dprog", 7U) == 0) {
            extern int LCD_B8D_Y85006Config(UINT32 Mode);
            UINT32 DataBuf[2];
            UINT32 SerdesRate = 0xff;
            UINT32 TestMode = 0; /* 0: test pattern, 1: liveview */

            (void) AmbaB8_Utility_StringToUInt32(pArgVector[2U], &SerdesRate);

            if (SerdesRate == 2) {
                AmbaB8_ShellPrintf("Choose B8_SERDES_RATE_2P3G\n");
            } else {
                AmbaB8_ShellPrintf("Unsupported Serdes rate\n");
            }

            LCD_B8D_Y85006Config(SerdesRate);

            (void) AmbaB8_Utility_StringToUInt32(pArgVector[3U], &TestMode);
            if (TestMode == 0) {
                DataBuf[0] = 0x11U;
                AmbaB8_RegWriteU32(0x1000U, 0xe0000840U, 0, B8_DATA_WIDTH_32BIT, 1, DataBuf);
            }
#endif
#ifdef CONFIG_LCD_B8D_Y85183
        } else if (AmbaB8_Utility_StringCompare(pArgVector[1U], "b8nprog", 7U) == 0) {
            extern int LCD_B8N_Y85183Config(UINT32 Mode);
            UINT32 DataBuf[2];
            UINT32 TestMode = 0; /* 0: test pattern, 1: liveview */

            AmbaB8_PrintUInt5("B8n vout prog\n", 0U, 0U, 0U, 0U, 0U);
            LCD_B8N_Y85183Config(0);

            (void) AmbaB8_Utility_StringToUInt32(pArgVector[2U], &TestMode);
            if (TestMode == 0) {
                DataBuf[0] = 0x11U;
                AmbaB8_RegWriteU32(0x10U, 0xe0000840U, 0, B8_DATA_WIDTH_32BIT, 1, DataBuf);
            }
#endif
        } else if (AmbaB8_Utility_StringCompare(pArgVector[1U], "lsrxbias", 8U) == 0) {
            extern UINT32 LsRxBias;
            UINT32 TimeOut;
            (void) AmbaB8_Utility_StringToUInt32(pArgVector[2U], &TimeOut);
            LsRxBias = TimeOut;
            AmbaB8_PrintUInt5("Set LsRxBias to %d\n", LsRxBias, 0U, 0U, 0U, 0U);

        } else if (AmbaB8_Utility_StringCompare(pArgVector[1U], "lsrxpost", 8U) == 0) {
            extern UINT32 PostLsRxBias;
            UINT32 TimeOut;
            (void) AmbaB8_Utility_StringToUInt32(pArgVector[2U], &TimeOut);
            PostLsRxBias = TimeOut;
            AmbaB8_PrintUInt5("Set PostLsRxBias to %d\n", PostLsRxBias, 0U, 0U, 0U, 0U);

        } else if (AmbaB8_Utility_StringCompare(pArgVector[1U], "lsspeed", 7U) == 0) {
            extern UINT32 LsSpeed;
            UINT32 Speed;
            (void) AmbaB8_Utility_StringToUInt32(pArgVector[2U], &Speed);
            LsSpeed = Speed;
            AmbaB8_PrintUInt5("Set ls speed to %d MHz\n", LsSpeed, 0U, 0U, 0U, 0U);
        } else {
            B8_Usage(PrintFunc);
        }

    } else {
        B8_Usage(PrintFunc);
    }
}

