/**
 *  @file AmbaB8_SerdesCalibration.c
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
 *  @details B8 driver APIs
 *
 */

#include "AmbaB8.h"
#include "AmbaB8_Calibration.h"

#include "AmbaB8_PHY.h"

#ifdef B8_DEV_VERSION
#include "AmbaB8_IoMap.h"
#include "AmbaB8CSL_VIN.h"
#include "AmbaB8CSL_PLL.h"
#include "AmbaB8_Serdes.h"
#include "AmbaB8_Packer.h"
#include "AmbaB8_Depacker.h"
#include "AmbaB8_Scratchpad.h"
#include "AmbaB8_PLL.h"
#include "AmbaB8_SPI.h"
#include "AmbaB8_I2C.h"
#include "AmbaB8_GPIO.h"
#include "AmbaB8_Communicate.h"
#include "AmbaGPIO.h"

/* Searching Type */
typedef enum SERDES_ANNEAL_TYPE_e_ {
    BOOT_POINT_SEARCH = 0,
    BEST_POINT_SEARCH
} SERDES_ANNEAL_TYPE_e;

typedef enum _SERDES_STATUS_e_ {
    SEARCH_DOWN = 0,
    SEARCH_UP,
    SEARCH_STOP

} SERDES_STATUS_e;
#endif

typedef struct {
    SERDES_REGISTER_s *pReg;
    UINT32 Value;
    UINT32 StartBit;
    UINT32 BitMask;
    UINT32 BitDepth;
    char   NameTag[5];
} SERDES_FIELD_s;

typedef struct {
    SERDES_FIELD_s Cap0;        /* CTLE 0x5268[3:0] */
    SERDES_FIELD_s Cap1;        /* CTLE 0x5268[7:4] */
    SERDES_FIELD_s Cap2;        /* CTLE 0x5268[11:8] */
    SERDES_FIELD_s Res0;        /* CTLE 0x5268[13:12] */
    SERDES_FIELD_s Res1;        /* CTLE 0x5268[15:14] */
    SERDES_FIELD_s Res2;        /* CTLE 0x5268[17:16] */
    /* msb-bit: sign bit */
    /* lsb-bit: magnitude */
    SERDES_FIELD_s Tap1;        /* DFE 0x525c[5:0] */
    SERDES_FIELD_s Tap2;        /* DFE 0x525c[12:8] */
    SERDES_FIELD_s Tap3;        /* DFE 0x525c[19:16] */
    SERDES_FIELD_s Tap4;        /* DFE 0x525c[23:20] */
} SERDES_CAL_FIELD_TAB_s;

#ifdef B8_DEV_VERSION
/* Global Search Condition */
UINT32 IterationOuter   = 400;  /* Iteration for outer loop (for tuning) */
UINT32 IterationInner   = 10;   /* Iteration for inner loop (for tuning) */
DOUBLE WeightT          = 0.6;  /* Weighting for T          (for tuning) */
DOUBLE WeightOuter      = 11;   /* Weighting for outer loop (for tuning) */
DOUBLE WeightInner      = 4;    /* Weighting for inner loop (for tuning) */
DOUBLE ExitEnergy       = 0.0;

static DOUBLE E_MinSofar = 1.0;
#endif

#ifdef BUILT_IN_SERDES
extern SERDES_REGISTER_TAB_s BuiltInParamRegTable;

#ifdef B8_DEV_VERSION
static SERDES_CAL_FIELD_TAB_s CalFieldTable = {
    .Cap0 = {
        .pReg = &ParamRegTable.ctle,
        .Value = 0xc,
        .StartBit = 0,
        .BitDepth = 4,
        .BitMask = 0xf,
        .NameTag = "Cap0",
    },
    .Cap1 = {
        .pReg = &ParamRegTable.ctle,
        .Value = 0x2,
        .StartBit = 4,
        .BitDepth = 4,
        .BitMask = 0xf,
        .NameTag = "Cap1",
    },
    .Cap2 = {
        .pReg = &ParamRegTable.ctle,
        .Value = 0x0,
        .StartBit = 8,
        .BitDepth = 4,
        .BitMask = 0xf,
        .NameTag = "Cap2",
    },
    .Res0 = {
        .pReg = &ParamRegTable.ctle,
        .Value = 0x1,
        .StartBit = 12,
        .BitDepth = 2,
        .BitMask = 0x3,
        .NameTag = "Res0",
    },
    .Res1 = {
        .pReg = &ParamRegTable.ctle,
        .Value = 0x1,
        .StartBit = 14,
        .BitDepth = 2,
        .BitMask = 0x3,
        .NameTag = "Res1",
    },
    .Res2 = {
        .pReg = &ParamRegTable.ctle,
        .Value = 0x2,
        .StartBit = 16,
        .BitDepth = 2,
        .BitMask = 0x3,
        .NameTag = "Res2",
    },
    .Tap1 = {
        .pReg = &ParamRegTable.dfe,
        .Value = 0x0,
        .StartBit = 0,
        .BitDepth = 6,
        .BitMask = 0x3f,
        .NameTag = "Tap1",
    },
    .Tap2 = {
        .pReg = &ParamRegTable.dfe,
        .Value = 0x3,
        .StartBit = 8,
        .BitDepth = 5,
        .BitMask = 0x1f,
        .NameTag = "Tap2",
    },
    .Tap3 = {
        .pReg = &ParamRegTable.dfe,
        .Value = 0x3,
        .StartBit = 16,
        .BitDepth = 4,
        .BitMask = 0xf,
        .NameTag = "Tap3",
    },
    .Tap4 = {
        .pReg = &ParamRegTable.dfe,
        .Value = 0x3,
        .StartBit = 20,
        .BitDepth = 4,
        .BitMask = 0xf,
        .NameTag = "Tap4",
    },

};
#endif
#endif

#ifdef BUILT_IN_SERDES
SERDES_CAL_FIELD_TAB_s BuiltInCalFieldTable = {
    .Cap0 = {
        .pReg = &BuiltInParamRegTable.ctle,
        .Value = 4,
        .StartBit = 0,
        .BitMask = 0xf,
    },
    .Res0 = {
        .pReg = &BuiltInParamRegTable.ctle,
        .Value = 3,
        .StartBit = 4,
        .BitMask = 0xf,
    },
    .Tap1 = {
        .pReg = &BuiltInParamRegTable.dfe,
        .Value = 1,
        .StartBit = 0,
        .BitMask = 0xf,
    },
    .Tap2 = {
        .pReg = &BuiltInParamRegTable.dfe,
        .Value = 1,
        .StartBit = 4,
        .BitMask = 0xf,
    },
};
#endif

#ifdef B8_DEV_VERSION
/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: Random_double_0_1
 *
 *  @Description:: Get a random value from 0 ~ 1
 *
 *  @Input      :: none
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *      DOUBLE : Value
\*-----------------------------------------------------------------------------------------------*/
static DOUBLE Random_double_0_1(void)
{
    UINT32 RandomVal;
    DOUBLE Value;

    AmbaB8_Wrap_rand(&RandomVal);
    Value = (DOUBLE)(RandomVal % 1000000) / 1000000;  /* Return 0 ~ 1, step is 0.000001 */

    return Value;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: Average_double
 *
 *  @Description:: Get average
 *
 *  @Input      :: none
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *      DOUBLE : Avg
\*-----------------------------------------------------------------------------------------------*/
static DOUBLE Average_double(DOUBLE *Array, int Size)
{
    int i;
    DOUBLE Avg = 0;

    for (i = 0; i < Size; i++)
        Avg += Array[i];

    Avg /= Size;

    return Avg;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: Square_of_sigma_double
 *
 *  @Description:: Get square of sigma
 *
 *  @Input      :: none
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *      DOUBLE : Square
\*-----------------------------------------------------------------------------------------------*/
static DOUBLE Square_of_sigma_double(DOUBLE *Array, DOUBLE Avg, int Size)
{
    int i;
    DOUBLE Square = 0;

    for (i = 0; i < Size; i++)
        Square += (Array[i] - Avg) * (Array[i] - Avg);

    Square /= Size;

    return Square;
}

static void Serdes_UpdateFieldRegister(SERDES_FIELD_s* pField)
{
    pField->pReg->Data &= (~(pField->BitMask << pField->StartBit));
    pField->pReg->Data |= ((pField->Value & pField->BitMask) << pField->StartBit);
}

static void Serdes_ConfigFieldRegister(UINT32 ChipID, SERDES_FIELD_s* pField)
{
    AmbaB8_RegWriteU32(ChipID, pField->pReg->Addr, 1, B8_DATA_WIDTH_32BIT, 1, &pField->pReg->Data);
}
#endif
static void Serdes_UpdatePhySetting(UINT32 ChipID, const GLOBAL_SEARCH_PARAM_s *pSerdesPhyConfig)
{
#ifndef B8_DEV_VERSION
    static SERDES_CAL_FIELD_TAB_s CalFieldTable = {
        .Cap0 = {
            .pReg = &ParamRegTable.ctle,
            .Value = 0xc,
            .StartBit = 0,
            .BitDepth = 4,
            .BitMask = 0xf,
            .NameTag = "Cap0",
        },
        .Cap1 = {
            .pReg = &ParamRegTable.ctle,
            .Value = 0x2,
            .StartBit = 4,
            .BitDepth = 4,
            .BitMask = 0xf,
            .NameTag = "Cap1",
        },
        .Cap2 = {
            .pReg = &ParamRegTable.ctle,
            .Value = 0x0,
            .StartBit = 8,
            .BitDepth = 4,
            .BitMask = 0xf,
            .NameTag = "Cap2",
        },
        .Res0 = {
            .pReg = &ParamRegTable.ctle,
            .Value = 0x1,
            .StartBit = 12,
            .BitDepth = 2,
            .BitMask = 0x3,
            .NameTag = "Res0",
        },
        .Res1 = {
            .pReg = &ParamRegTable.ctle,
            .Value = 0x1,
            .StartBit = 14,
            .BitDepth = 2,
            .BitMask = 0x3,
            .NameTag = "Res1",
        },
        .Res2 = {
            .pReg = &ParamRegTable.ctle,
            .Value = 0x2,
            .StartBit = 16,
            .BitDepth = 2,
            .BitMask = 0x3,
            .NameTag = "Res2",
        },
        .Tap1 = {
            .pReg = &ParamRegTable.dfe,
            .Value = 0x0,
            .StartBit = 0,
            .BitDepth = 6,
            .BitMask = 0x3f,
            .NameTag = "Tap1",
        },
        .Tap2 = {
            .pReg = &ParamRegTable.dfe,
            .Value = 0x3,
            .StartBit = 8,
            .BitDepth = 5,
            .BitMask = 0x1f,
            .NameTag = "Tap2",
        },
        .Tap3 = {
            .pReg = &ParamRegTable.dfe,
            .Value = 0x3,
            .StartBit = 16,
            .BitDepth = 4,
            .BitMask = 0xf,
            .NameTag = "Tap3",
        },
        .Tap4 = {
            .pReg = &ParamRegTable.dfe,
            .Value = 0x3,
            .StartBit = 20,
            .BitDepth = 4,
            .BitMask = 0xf,
            .NameTag = "Tap4",
        },

    };
#endif

    if ((ChipID & B8_MAIN_CHIP_ID_BUILT_IN_B8NF) != 0U) {
#ifdef BUILT_IN_SERDES
        /* built-in */
        BuiltInCalFieldTable.Cap0.Value = (pSerdesPhyConfig->Cap & BuiltInCalFieldTable.Cap0.BitMask);
        BuiltInCalFieldTable.Cap0.pReg->Data &= (~(BuiltInCalFieldTable.Cap0.BitMask << BuiltInCalFieldTable.Cap0.StartBit));
        BuiltInCalFieldTable.Cap0.pReg->Data |= ((BuiltInCalFieldTable.Cap0.Value & BuiltInCalFieldTable.Cap0.BitMask) << BuiltInCalFieldTable.Cap0.StartBit);

        BuiltInCalFieldTable.Res0.Value = (pSerdesPhyConfig->Res & BuiltInCalFieldTable.Res0.BitMask);
        BuiltInCalFieldTable.Res0.pReg->Data &= (~(BuiltInCalFieldTable.Res0.BitMask << BuiltInCalFieldTable.Res0.StartBit));
        BuiltInCalFieldTable.Res0.pReg->Data |= ((BuiltInCalFieldTable.Res0.Value & BuiltInCalFieldTable.Res0.BitMask) << BuiltInCalFieldTable.Res0.StartBit);

        BuiltInCalFieldTable.Tap1.Value = (pSerdesPhyConfig->Tap1 & BuiltInCalFieldTable.Tap1.BitMask);
        BuiltInCalFieldTable.Tap1.pReg->Data &= (~(BuiltInCalFieldTable.Tap1.BitMask << BuiltInCalFieldTable.Tap1.StartBit));
        BuiltInCalFieldTable.Tap1.pReg->Data |= ((BuiltInCalFieldTable.Tap1.Value & BuiltInCalFieldTable.Tap1.BitMask) << BuiltInCalFieldTable.Tap1.StartBit);

        BuiltInCalFieldTable.Tap2.Value = (pSerdesPhyConfig->Tap2 & CalFieldTable.Tap2.BitMask);
        BuiltInCalFieldTable.Tap2.pReg->Data &= (~(BuiltInCalFieldTable.Tap2.BitMask << BuiltInCalFieldTable.Tap2.StartBit));
        BuiltInCalFieldTable.Tap2.pReg->Data |= ((BuiltInCalFieldTable.Tap2.Value & BuiltInCalFieldTable.Tap2.BitMask) << BuiltInCalFieldTable.Tap2.StartBit);
#endif

    } else {
        /* b8n */
        CalFieldTable.Cap0.Value = ((pSerdesPhyConfig->Cap) & CalFieldTable.Cap0.BitMask);
        CalFieldTable.Cap0.pReg->Data &= (~(CalFieldTable.Cap0.BitMask << CalFieldTable.Cap0.StartBit));
        CalFieldTable.Cap0.pReg->Data |= ((CalFieldTable.Cap0.Value & CalFieldTable.Cap0.BitMask) << CalFieldTable.Cap0.StartBit);

        CalFieldTable.Cap1.Value = ((pSerdesPhyConfig->Cap >> (UINT32)4U) & CalFieldTable.Cap1.BitMask);
        CalFieldTable.Cap1.pReg->Data &= (~(CalFieldTable.Cap1.BitMask << CalFieldTable.Cap1.StartBit));
        CalFieldTable.Cap1.pReg->Data |= ((CalFieldTable.Cap1.Value & CalFieldTable.Cap1.BitMask) << CalFieldTable.Cap1.StartBit);

        CalFieldTable.Cap2.Value = ((pSerdesPhyConfig->Cap >> (UINT32)8U) & CalFieldTable.Cap2.BitMask);
        CalFieldTable.Cap2.pReg->Data &= (~(CalFieldTable.Cap2.BitMask << CalFieldTable.Cap2.StartBit));
        CalFieldTable.Cap2.pReg->Data |= ((CalFieldTable.Cap2.Value & CalFieldTable.Cap2.BitMask) << CalFieldTable.Cap2.StartBit);


        CalFieldTable.Res0.Value = ((pSerdesPhyConfig->Res) & CalFieldTable.Res0.BitMask);
        CalFieldTable.Res0.pReg->Data &= (~(CalFieldTable.Res0.BitMask << CalFieldTable.Res0.StartBit));
        CalFieldTable.Res0.pReg->Data |= ((CalFieldTable.Res0.Value & CalFieldTable.Res0.BitMask) << CalFieldTable.Res0.StartBit);

        CalFieldTable.Res1.Value = ((pSerdesPhyConfig->Res >> (UINT32)2U) & CalFieldTable.Res1.BitMask);
        CalFieldTable.Res1.pReg->Data &= (~(CalFieldTable.Res1.BitMask << CalFieldTable.Res1.StartBit));
        CalFieldTable.Res1.pReg->Data |= ((CalFieldTable.Res1.Value & CalFieldTable.Res1.BitMask) << CalFieldTable.Res1.StartBit);

        CalFieldTable.Res2.Value = ((pSerdesPhyConfig->Res >> (UINT32)4U) & CalFieldTable.Res2.BitMask);
        CalFieldTable.Res2.pReg->Data &= (~(CalFieldTable.Res2.BitMask << CalFieldTable.Res2.StartBit));
        CalFieldTable.Res2.pReg->Data |= ((CalFieldTable.Res2.Value & CalFieldTable.Res2.BitMask) << CalFieldTable.Res2.StartBit);

        CalFieldTable.Tap1.Value = (pSerdesPhyConfig->Tap1 & CalFieldTable.Tap1.BitMask);
        CalFieldTable.Tap1.pReg->Data &= (~(CalFieldTable.Tap1.BitMask << CalFieldTable.Tap1.StartBit));
        CalFieldTable.Tap1.pReg->Data |= ((CalFieldTable.Tap1.Value & CalFieldTable.Tap1.BitMask) << CalFieldTable.Tap1.StartBit);

        CalFieldTable.Tap2.Value = (pSerdesPhyConfig->Tap2 & CalFieldTable.Tap2.BitMask);
        CalFieldTable.Tap2.pReg->Data &= (~(CalFieldTable.Tap2.BitMask << CalFieldTable.Tap2.StartBit));
        CalFieldTable.Tap2.pReg->Data |= ((CalFieldTable.Tap2.Value & CalFieldTable.Tap2.BitMask) << CalFieldTable.Tap2.StartBit);

        CalFieldTable.Tap3.Value = (pSerdesPhyConfig->Tap3 & CalFieldTable.Tap3.BitMask);
        CalFieldTable.Tap3.pReg->Data &= (~(CalFieldTable.Tap3.BitMask << CalFieldTable.Tap3.StartBit));
        CalFieldTable.Tap3.pReg->Data |= ((CalFieldTable.Tap3.Value & CalFieldTable.Tap3.BitMask) << CalFieldTable.Tap3.StartBit);

        CalFieldTable.Tap4.Value = (pSerdesPhyConfig->Tap4 & CalFieldTable.Tap4.BitMask);
        CalFieldTable.Tap4.pReg->Data &= (~(CalFieldTable.Tap4.BitMask << CalFieldTable.Tap4.StartBit));
        CalFieldTable.Tap4.pReg->Data |= ((CalFieldTable.Tap4.Value & CalFieldTable.Tap4.BitMask) << CalFieldTable.Tap4.StartBit);

        AmbaB8_PrintUInt5("c h'%02x %02x %02x", CalFieldTable.Cap0.Value, CalFieldTable.Cap1.Value, CalFieldTable.Cap2.Value, 0U, 0U);
        AmbaB8_PrintUInt5("r h'%02x %02x %02x", CalFieldTable.Res0.Value, CalFieldTable.Res1.Value, CalFieldTable.Res2.Value, 0U, 0U);
        AmbaB8_PrintUInt5("t h'%02x %02x %02x %02x", CalFieldTable.Tap1.Value, CalFieldTable.Tap2.Value, CalFieldTable.Tap3.Value, CalFieldTable.Tap4.Value, 0U);
    }
}

#ifdef B8_DEV_VERSION
/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: Serdes_LowSpeedTest
 *
 *  @Description:: SERDES low speed test
 *
 *  @Input      ::
 *      ChipID:    B8 chip id
 *      Round:     Test round
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *      int : Pass round
\*-----------------------------------------------------------------------------------------------*/
static UINT32 Serdes_LowSpeedTest(UINT32 ChipID, UINT32 Round)
{
    UINT32 DataBuf32[4];
    UINT32 RandomValue[4];
    UINT32 i;
    UINT32 RetVal = B8_ERR_NONE;
    B8_COMM_CONFIG_s CommConfig = {0};

    for(i = 0; i < Round; i++) {
        AmbaB8_Wrap_rand(&RandomValue[0]);
        RandomValue[0] = (RandomValue[0] % 65535) + 1;
        AmbaB8_Wrap_rand(&RandomValue[1]);
        RandomValue[1] = (RandomValue[1] % 65535) + 1;
        AmbaB8_Wrap_rand(&RandomValue[2]);
        RandomValue[2] = (RandomValue[2] % 65535) + 1;
        AmbaB8_Wrap_rand(&RandomValue[3]);
        RandomValue[3] = (RandomValue[3] % 65535) + 1;

        DataBuf32[0] = RandomValue[0];
        DataBuf32[1] = RandomValue[1];
        DataBuf32[2] = RandomValue[2];
        DataBuf32[3] = RandomValue[3];

        RetVal = AmbaB8_RegWrite(ChipID, & pAmbaB8_PllReg->CorePllFraction, 1, B8_DATA_WIDTH_32BIT, 1, &DataBuf32);
        DataBuf32[0] = DataBuf32[1] = DataBuf32[2] = DataBuf32[3] = 0U;

        if (RetVal == B8_ERR_NONE) {
            RetVal = AmbaB8_RegRead(ChipID, & (pAmbaB8_PllReg->CorePllFraction), 1, B8_DATA_WIDTH_32BIT, 1, DataBuf32);
        }

        if (RetVal != B8_ERR_NONE || DataBuf32[0] != RandomValue[0]) {
            AmbaB8_PrintUInt5("Low speed test fail at round %d/%d", i, Round, 0U, 0U, 0U);
            AmbaB8_PrintUInt5("0x%08x(read) != 0x%08x(write)", DataBuf32[0], RandomValue[0], 0U, 0U, 0U);

            /* pwm reset & configure */
            AmbaB8_CommReset(ChipID);
            CommConfig.ReplayTimes = 7U;
            CommConfig.ChunkSize = 2U;
            CommConfig.CreditPwr = 4U;
            AmbaB8_CommConfig(ChipID, &CommConfig);

            return i;
        }
    }

    AmbaB8_PrintUInt5("Low speed test pass (%d)", Round, 0U, 0U, 0U, 0U);

    return Round;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: Serdes_GetLinkTestResult
 *
 *  @Description:: Get link test result
 *
 *  @Input      ::
 *      ChipID:     B8 chip id
 *      SerDesRate: SERDES data rate
 *      TestRound:  Test round
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *      double Energy
\*-----------------------------------------------------------------------------------------------*/
static double Serdes_GetLinkTestResult(UINT32 ChipID, UINT32 SerDesRate, UINT32 TestRound)
{
    double Energy = 0;
    int PassRound = 0;

    if (AmbaB8_SerdesLink(ChipID, SerDesRate) != B8_ERR_NONE) {
        return 1.0;
    } else {
        PassRound = Serdes_LowSpeedTest(ChipID, TestRound);
        Energy = ((double)TestRound - (double)PassRound) / (double)TestRound;

        return Energy;
    }
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: Serdes_GetPatternTestResult
 *
 *  @Description:: Get pattern test result
 *
 *  @Input      ::
 *      ChipID:              B8 chip id
 *      DelayTime:           Delay time in each training (ms)
 *      pMaxTotalPktNum:     Pointer to maximum total packet number
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *      DOUBLE Energy
\*-----------------------------------------------------------------------------------------------*/
static DOUBLE Serdes_GetPatternTestResult(UINT32 ChipID, UINT32 SerDesRate, UINT32 DelayTime, UINT32 *pMaxTotalPktNum)
{
    DOUBLE Energy = 0.0, SubEnergy = 0.0;
    UINT32 PassPktNum = 0;
    UINT32 ErrorPktNum = 0;
    UINT32 CorrectedPktNum = 0;
    UINT32 SubChipID;
    UINT32 i;

    if (DelayTime == 0) {
        DelayTime = 1000;  /* Default delay time is 1000(ms) */
    }
    if (DelayTime < 100) {
        DelayTime = 100;
    }

    if (AmbaB8_SerdesLink(ChipID, SerDesRate) == B8_ERR_NONE && AmbaB8_PackerTestMode(ChipID) == B8_ERR_NONE) {
        /* 100ms pre-test */
        AmbaKAL_TaskSleep(100);

        if (AmbaB8_GetSubChipCount(ChipID) <= 1U) {
            AmbaB8_DepackerGetStatPkt(ChipID, &PassPktNum, &ErrorPktNum, &CorrectedPktNum);
            if ((PassPktNum == 0) || ((PassPktNum + ErrorPktNum) <= (0.9 * (*pMaxTotalPktNum) * 100 / DelayTime))) {
                Energy = 1.0;
            } else {
                Energy = ((DOUBLE)ErrorPktNum + (DOUBLE)CorrectedPktNum) / ((DOUBLE)PassPktNum + (DOUBLE)ErrorPktNum);
            }
        } else {
            for (i = 0; i < B8_MAX_NUM_SUBCHIP; i++) {
                if ((ChipID & (0x1 << i)) != 0U) {
                    SubChipID = ((ChipID & B8_MAIN_CHIP_ID_MASK) | (1U << i));
                    AmbaB8_DepackerGetStatPkt(SubChipID, &PassPktNum, &ErrorPktNum, &CorrectedPktNum);
                    if ((PassPktNum == 0) || ((PassPktNum + ErrorPktNum) <= (0.9 * (*pMaxTotalPktNum) * 100 / DelayTime))) {
                        SubEnergy = 1.0;
                    } else {
                        SubEnergy = ((DOUBLE)ErrorPktNum + (DOUBLE)CorrectedPktNum) / ((DOUBLE)PassPktNum + (DOUBLE)ErrorPktNum);
                    }

                    if (Energy < SubEnergy) Energy = SubEnergy;
                }
            }
        }

        /* Continue testing */
        if (Energy < E_MinSofar) {
            AmbaKAL_TaskSleep(DelayTime - 100);
            AmbaB8_PrintUInt5("  Test time: %d ms", DelayTime, 0U, 0U, 0U, 0U);

            if (AmbaB8_GetSubChipCount(ChipID) <= 1U) {
                AmbaB8_DepackerGetStatPkt(ChipID, &PassPktNum, &ErrorPktNum, &CorrectedPktNum);
                AmbaB8_PrintUInt5("  PassPktNum: %d, ErrorPktNum: %d, CorrectedPktNum: %d", PassPktNum, ErrorPktNum, CorrectedPktNum, 0U, 0U);

                if ((PassPktNum == 0) || ((PassPktNum + ErrorPktNum) <= 0.9 * (*pMaxTotalPktNum))) {
                    Energy = 1;
                    AmbaB8_PrintStr5("  Test fail!", NULL, NULL, NULL, NULL, NULL);

                } else {
                    Energy = ((DOUBLE)ErrorPktNum + (DOUBLE)CorrectedPktNum) / ((DOUBLE)PassPktNum + (DOUBLE)ErrorPktNum);
                    *pMaxTotalPktNum = PassPktNum + ErrorPktNum;
                }
            } else {
                for (i = 0; i < B8_MAX_NUM_SUBCHIP; i++) {
                    if ((ChipID & (0x1 << i)) != 0U) {
                        SubChipID = ((ChipID & B8_MAIN_CHIP_ID_MASK) | (1U << i));

                        AmbaB8_DepackerGetStatPkt(SubChipID, &PassPktNum, &ErrorPktNum, &CorrectedPktNum);
                        AmbaB8_PrintUInt5("  (ChipID=0x%x) PassPktNum: %d, ErrorPktNum: %d, CorrectedPktNum: %d", SubChipID, PassPktNum, ErrorPktNum, CorrectedPktNum, 0U);
                        if ((PassPktNum == 0) || ((PassPktNum + ErrorPktNum) <= 0.9 * (*pMaxTotalPktNum))) {
                            SubEnergy = 1;
                            AmbaB8_PrintStr5("  Test fail!", NULL, NULL, NULL, NULL, NULL);

                        } else {
                            SubEnergy = ((DOUBLE)ErrorPktNum + (DOUBLE)CorrectedPktNum) / ((DOUBLE)PassPktNum + (DOUBLE)ErrorPktNum);
                            *pMaxTotalPktNum = PassPktNum + ErrorPktNum;
                        }

                        if (Energy < SubEnergy) Energy = SubEnergy;
                    }
                }
            }
        } else {
            AmbaB8_PrintUInt5("  Test time: 100 ms", 0U, 0U, 0U, 0U, 0U);
            AmbaB8_PrintUInt5("  PassPktNum: %d, ErrorPktNum: %d, CorrectedPktNum: %d", PassPktNum, ErrorPktNum, CorrectedPktNum, 0U, 0U);
            AmbaB8_PrintStr5("  Pre-Test fail!", NULL, NULL, NULL, NULL, NULL);
        }
    } else {
        Energy = 1.0;
    }

    return Energy;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: Serdes_Anneal_Tune
 *
 *  @Description:: Tuning SERDES parameters
 *
 *  @Input      ::
 *      ChipID:       B8 chip id
 *      SerDesRate:   SERDES data rate
 *      TimeInterval: Time interval for each phy configuration (ms)
 *      AnnealType:   Annealing searching type
 *      pMemBase:     Pointer to memory base for calibration
 *
 *  @Output     ::
 *      pOutputPhyConfig: Pointer to SERDES output phy configuration
 *
 *  @Return     ::
 *      int : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static int Serdes_Anneal_Tune(UINT32 ChipID, UINT32 SerDesRate, UINT32 TimeInterval,
                              SERDES_ANNEAL_TYPE_e AnnealType, GLOBAL_SEARCH_PARAM_s *pOutputPhyConfig, void *pMemBase)
{
    GLOBAL_SEARCH_PARAM_s X_MinSofar = {0};
    GLOBAL_SEARCH_PARAM_s X_Min = {0};
    GLOBAL_SEARCH_PARAM_s X_Temp = {0};
    DOUBLE E_Min = 1;
    DOUBLE T_Array[16] = {0};
    DOUBLE T_Avg = 0;
    DOUBLE T_0 = 0;
    UINT64 X_RecordIndex = 0;
    UINT64* X_RecordPtr;
    UINT32 MaxRound = 0x10000;                      /* 64*32*16*16*4096*64 = 137,438,953,472 */
    UINT32 i, j, k, l;
    UINT32 MaxTotalPktNum = 0;

    X_RecordPtr = pMemBase;

    /* Get T_0 */
    for(i = 0; i < 16; i++) {
        if ((ChipID & B8_MAIN_CHIP_ID_BUILT_IN_B8NF) != 0U) {
            /* built-in */
            X_Temp.Cap  = i;
            X_Temp.Res  = i;
            X_Temp.Tap1 = i;
            X_Temp.Tap2 = i;

        } else {
            /* b8n */
            X_Temp.Cap  = (UINT32)((0xfff * i) / 15. + 0.5);
            X_Temp.Res  = (UINT32)((0x3f  * i) / 15. + 0.5);
            X_Temp.Tap1 = (UINT32)((0x1f  * i) / 15. + 0.5);
            X_Temp.Tap2 = (UINT32)((0xf   * i) / 15. + 0.5);
            X_Temp.Tap3 = (UINT32)((0x7   * i) / 15. + 0.5);
            X_Temp.Tap4 = (UINT32)((0x7   * i) / 15. + 0.5);
        }
        Serdes_UpdatePhySetting(ChipID, &X_Temp);

        if (AnnealType == BOOT_POINT_SEARCH) {
            T_Array[i] = Serdes_GetLinkTestResult(ChipID, SerDesRate, 1000);
        } else {
            T_Array[i] = Serdes_GetPatternTestResult(ChipID, SerDesRate, TimeInterval, &MaxTotalPktNum);
        }
        AmbaB8_PrintUInt5("  T_Array[%d]: %u * 10e-6", i, T_Array[i], 0U, 0U, 0U);
    }

    T_Avg = Average_double(T_Array, 16);
    T_0 = Square_of_sigma_double(T_Array, T_Avg, 16);
    T_0 = (T_0 == 0 ? 1 : T_0);

    AmbaB8_PrintStr5("========== Annealing search ==========", NULL, NULL, NULL, NULL, NULL);
    AmbaB8_PrintUInt5("Iteration(outer): %u", IterationOuter, 0U, 0U, 0U, 0U);
    AmbaB8_PrintUInt5("Iteration(inner): %u", IterationInner, 0U, 0U, 0U, 0U);
    AmbaB8_PrintUInt5("Weighting(t): %u * 10e-2", (UINT32)(WeightT * 100), 0U, 0U, 0U, 0U);
    AmbaB8_PrintUInt5("Weighting(outer): %u * 10e-2", (UINT32)(WeightOuter * 100), 0U, 0U, 0U, 0U);
    AmbaB8_PrintUInt5("Weighting(inner): %u * 10e-2", (UINT32)(WeightInner * 100), 0U, 0U, 0U, 0U);
    AmbaB8_PrintUInt5("T_0: %u * 10e-6", (UINT32)(T_0 * 1000000), 0U, 0U, 0U, 0U);
    AmbaB8_PrintUInt5("MaxTotalPktNum: %u", MaxTotalPktNum, 0U, 0U, 0U, 0U);

    /* Annealing searching algorithm */
    for(i = 0; i < IterationOuter; i++) {
        DOUBLE Ti = T_0 / (1 + (WeightT * T_0 * i / (IterationOuter - 1)));
        GLOBAL_SEARCH_PARAM_s X_Outer;

        if ((ChipID & B8_MAIN_CHIP_ID_BUILT_IN_B8NF) != 0U) {
            X_Outer.Cap  = (X_Min.Cap   + (UINT32)(WeightOuter * Random_double_0_1())) % 16;
            X_Outer.Res  = (X_Min.Res   + (UINT32)(WeightOuter * Random_double_0_1())) % 16;
            X_Outer.Tap1 = (X_Min.Tap1  + (UINT32)(WeightOuter * Random_double_0_1())) % 16;
            X_Outer.Tap2 = (X_Min.Tap2  + (UINT32)(WeightOuter * Random_double_0_1())) % 16;

        } else {
            X_Outer.Cap  = (X_Min.Cap   + (UINT32)(WeightOuter * Random_double_0_1())) % (0xfff + 1);
            X_Outer.Res  = (X_Min.Res   + (UINT32)(WeightOuter * Random_double_0_1())) % (0x3f + 1);
            X_Outer.Tap1 = (X_Min.Tap1  + (UINT32)(WeightOuter * Random_double_0_1())) % (0x1f + 1);
            X_Outer.Tap2 = (X_Min.Tap2  + (UINT32)(WeightOuter * Random_double_0_1())) % (0xf + 1);
            X_Outer.Tap3 = (X_Min.Tap3  + (UINT32)(WeightOuter * Random_double_0_1())) % (0x7 + 1);
            X_Outer.Tap4 = (X_Min.Tap4  + (UINT32)(WeightOuter * Random_double_0_1())) % (0x7 + 1);
        }

        for(j = 0; j < IterationInner; j++) {
            UINT32 TuneDone = 0;
            DOUBLE E = 1;
            DOUBLE E_Delta = 0;
            DOUBLE P_Accept = 0;
            DOUBLE Random_0_1 = 0;
            GLOBAL_SEARCH_PARAM_s X_Inner = {0};
            UINT64 Value = 0;

            AmbaB8_PrintUInt5("\n---------- Round: [%d][%d] ----------", i, j, 0U, 0U, 0U);

            /* Exclude repeated values */
            for (k = 0; k < MaxRound; k++) {
                UINT64 DesideCount = 0;

                if ((ChipID & B8_MAIN_CHIP_ID_BUILT_IN_B8NF) != 0U) {
                    X_Inner.Cap  = (X_Outer.Cap  + (UINT32)(WeightInner * Random_double_0_1())) % 16;
                    X_Inner.Res  = (X_Outer.Res  + (UINT32)(WeightInner * Random_double_0_1())) % 16;
                    X_Inner.Tap1 = (X_Outer.Tap1 + (UINT32)(WeightInner * Random_double_0_1())) % 16;
                    X_Inner.Tap2 = (X_Outer.Tap2 + (UINT32)(WeightInner * Random_double_0_1())) % 16;

                    Value = (X_Inner.Cap) + (X_Inner.Res << 4) + (X_Inner.Tap1 << 8) + (X_Inner.Tap2 << 12);

                } else {
                    X_Inner.Cap  = (X_Outer.Cap  + (UINT32)(WeightInner * Random_double_0_1())) % (0xfff + 1);
                    X_Inner.Res  = (X_Outer.Res  + (UINT32)(WeightInner * Random_double_0_1())) % (0x3f + 1);
                    X_Inner.Tap1 = (X_Outer.Tap1 + (UINT32)(WeightInner * Random_double_0_1())) % (0x1f + 1);
                    X_Inner.Tap2 = (X_Outer.Tap2 + (UINT32)(WeightInner * Random_double_0_1())) % (0xf + 1);
                    X_Inner.Tap3 = (X_Outer.Tap3 + (UINT32)(WeightInner * Random_double_0_1())) % (0x7 + 1);
                    X_Inner.Tap4 = (X_Outer.Tap4 + (UINT32)(WeightInner * Random_double_0_1())) % (0x7 + 1);

                    Value = (((UINT64)X_Inner.Cap & 0xfff) | (((UINT64)X_Inner.Res & 0x3f) << 12) |
                             (((UINT64)X_Inner.Tap1 & 0x1f) << 32) | ((((UINT64)X_Inner.Tap2 & 0xf) << 32) << 8) |
                             ((((UINT64)X_Inner.Tap3 & 0x7) << 32) << 16) | ((((UINT64)X_Inner.Tap4 & 0x7) << 32) << 20));
                }

                for (l = 0; l < X_RecordIndex; l++) {
                    if (Value != X_RecordPtr[l]) {
                        DesideCount++;
                    }
                }

                if (DesideCount == X_RecordIndex) {
                    break;
                }
            }

            X_RecordPtr[X_RecordIndex] = Value;
            X_RecordIndex++;

            Serdes_UpdatePhySetting(ChipID, &X_Inner);

            if (AnnealType == BOOT_POINT_SEARCH) {
                E = Serdes_GetLinkTestResult(ChipID, SerDesRate, 1000);
            } else {
                E = Serdes_GetPatternTestResult(ChipID, SerDesRate, TimeInterval, &MaxTotalPktNum);
            }

            E_Delta = E - E_Min;
            P_Accept = exp(E_Delta * (-1) / Ti);
            Random_0_1 = Random_double_0_1();

            if (E_Delta < 0) {
                /* better */
                E_Min = E;
                AmbaB8_Wrap_memcpy(&X_Min, &X_Inner, sizeof(GLOBAL_SEARCH_PARAM_s));
                if (E < E_MinSofar) {
                    E_MinSofar = E;
                    AmbaB8_Wrap_memcpy(&X_MinSofar, &X_Inner, sizeof(GLOBAL_SEARCH_PARAM_s));
                }

                if ((E == 0) || ((AnnealType == BEST_POINT_SEARCH) && (E <= ExitEnergy))) {
                    TuneDone = 1;
                }
            } else if(Random_0_1 < P_Accept) {
                E_Min = E;
                AmbaB8_Wrap_memcpy(&X_Min, &X_Inner, sizeof(GLOBAL_SEARCH_PARAM_s));
            }

            AmbaB8_PrintUInt5("  Ti: %u * 10e-6", (UINT32)(Ti * 1000000), 0U, 0U, 0U, 0U);
            AmbaB8_PrintUInt5("  Random_0_1: %u * 10e-6, P_Accept: %u * 10e-6", (UINT32)(Random_0_1 * 1000000), (UINT32)(P_Accept * 1000000), 0U, 0U, 0U);
            AmbaB8_PrintUInt5("  E_Min: %u * 10e-6", (UINT32)(E_Min * 1000000), 0U, 0U, 0U, 0U);
            AmbaB8_PrintUInt5("  E_Delta: %u * 10e-6", (UINT32)(E_Delta * 1000000), 0U, 0U, 0U, 0U);
            AmbaB8_PrintUInt5("  E_Cur: %u * 10e-6", (UINT32)(E * 1000000), 0U, 0U, 0U, 0U);
            AmbaB8_PrintUInt5("  E_MinSofar: %u * 10e-6", (UINT32)(E_MinSofar * 1000000), 0U, 0U, 0U, 0U);
            AmbaB8_PrintUInt5("  X_MinSofar: Cap: 0x%x, Res: 0x%x", \
                              X_MinSofar.Cap, X_MinSofar.Res, 0U, 0U, 0U);
            AmbaB8_PrintUInt5("  X_MinSofar: Tap1: 0x%x, Tap2: 0x%x, Tap3: 0x%x, Tap4: 0x%x", \
                              X_MinSofar.Tap1, X_MinSofar.Tap2, X_MinSofar.Tap3, X_MinSofar.Tap4, 0U);

            if (TuneDone == 1) goto tune_done;
        }
    }

tune_done:

    if ((AnnealType == BOOT_POINT_SEARCH) && (E_MinSofar != 0)) {
        return B8_ERR_UNEXPECTED;

    } else {
        AmbaB8_Wrap_memcpy(pOutputPhyConfig, &X_MinSofar, sizeof(GLOBAL_SEARCH_PARAM_s));
        return OK;
    }
}
#endif

#ifdef B8_DEV_VERSION
/* Local Search Calibration */
static UINT32 Serdes_ComputeIndex(UINT32 ChipID, UINT32* pIndex, UINT32 SampleTime)
{
    UINT32 PassPktNum = 0, ErrorPktNum = 0, CorrectedPktNum = 0;
    UINT32 CurIndex;
    UINT32 RetVal;

    /* Get pkt statistics during SampleTime (ms) */
    AmbaB8_DepackerResetPktCounter(ChipID);
    AmbaKAL_TaskSleep(SampleTime);
    AmbaB8_DepackerGetStatPkt(ChipID, &PassPktNum, &ErrorPktNum, &CorrectedPktNum);

    AmbaB8_PrintUInt5("PassPktNum = %d, ErrorPktNum = %d, CorrectedPktNum = %d", PassPktNum, ErrorPktNum, CorrectedPktNum, 0U, 0U);

    if ((PassPktNum + ErrorPktNum) == 0U) {
        AmbaB8_PrintUInt5("[Serdes] ChipID: 0x%x, pkt#0", ChipID, 0U, 0U, 0U, 0U);
        RetVal = B8_ERR_SERDES_LINK;
    } else {
        /* Compute current index */
        CurIndex = (UINT32)(((UINT64)ErrorPktNum + CorrectedPktNum) * 1e8 / ((UINT64)PassPktNum + ErrorPktNum));

        AmbaB8_PrintUInt5("ChipID: 0x%x, BER in %d msec: (0x%x * 1e8)", ChipID, SampleTime, CurIndex, 0U, 0U);

        /* Better, update index */
        if ((CurIndex == 0) || (CurIndex < *pIndex)) {
            *pIndex = CurIndex;
            RetVal = B8_ERR_NONE;
            /* Worse */
        } else {
            RetVal = B8_ERR_UNEXPECTED;
        }
    }

    return RetVal;
}

static UINT32 Serdes_LinearSearch(UINT32 ChipID, SERDES_FIELD_s* pField, UINT32 SamplePeriod)
{
    SERDES_STATUS_e Status = SEARCH_DOWN;
    UINT32 Index = 0xffffffffU;  /* initialize index as max.*/

    /* Compute initial index */
    Serdes_ComputeIndex(ChipID, &Index, SamplePeriod);
    AmbaB8_PrintStr5("[Serdes] %s", pField->NameTag, NULL, NULL, NULL, NULL);
    AmbaB8_PrintUInt5("[Serdes] ChipID: 0x%x, Addr: 0x%x, StartBit: %d, BitMask: 0x%x, Value: 0x%x", \
                      ChipID, pField->pReg->Addr, pField->StartBit, pField->BitMask, pField->Value);

    while (pField->Value > 0U && Index != 0U) {
        /* Try search down */
        (pField->Value)--;
        Serdes_UpdateFieldRegister(pField);
        Serdes_ConfigFieldRegister(ChipID, pField);

        if (Serdes_ComputeIndex(ChipID, &Index, SamplePeriod) == B8_ERR_NONE) {
            /* Keep searching down and then stop */
            Status = SEARCH_STOP;
        } else {
            /* Restore */
            (pField->Value)++;
            Serdes_UpdateFieldRegister(pField);
            Serdes_ConfigFieldRegister(ChipID, pField);
            break;
        }
    }

    while (Status != SEARCH_STOP && pField->Value < pField->BitMask && Index != 0U) {
        /* Try search up */
        (pField->Value)++;
        Serdes_UpdateFieldRegister(pField);
        Serdes_ConfigFieldRegister(ChipID, pField);

        if (Serdes_ComputeIndex(ChipID, &Index, SamplePeriod) == B8_ERR_NONE) {
            /* Search up */
            continue;
        } else {
            /* Restore */
            (pField->Value)--;
            Serdes_UpdateFieldRegister(pField);
            Serdes_ConfigFieldRegister(ChipID, pField);
            break;
        }
    }

    Status = SEARCH_STOP;

    AmbaB8_PrintUInt5("[Serdes] ->>> 0x%x", pField->Value, 0U, 0U, 0U, 0U);
    return Index;
}

static UINT32 Serdes_SignLinearSearch(UINT32 ChipID, SERDES_FIELD_s* pField, UINT32 SamplePeriod)
{
    SERDES_STATUS_e Status = SEARCH_DOWN;
    UINT32 Index = 0xffffffffU;  /* initialize index as max.*/
    INT8 RefValue;
    INT8 Step;

    /* Compute initial index */
    Serdes_ComputeIndex(ChipID, &Index, SamplePeriod);
    AmbaB8_PrintStr5("[Serdes] %s", pField->NameTag, NULL, NULL, NULL, NULL);
    AmbaB8_PrintUInt5("[Serdes]ChipID: 0x%x, Addr: 0x%04x, StartBit: %d, BitDepth: 0x%x, Value: 0x%x", \
                      ChipID, pField->pReg->Addr, pField->StartBit, pField->BitDepth, pField->Value);

    RefValue = (INT8)(pField->Value << (8U - pField->BitDepth));
    Step = (INT8)(1 << (8U - pField->BitDepth));
    AmbaB8_PrintInt5("refval: %d", ((RefValue >> (8U - pField->BitDepth)) & pField->BitMask), 0, 0, 0, 0);

    while ((RefValue > (-128 + Step)) && Index != 0U) {
        /* Try search down -- */
        RefValue -= Step;
        pField->Value = (UINT32)((RefValue >> (8U - pField->BitDepth)) & pField->BitMask);
        AmbaB8_PrintInt5("--: %d", ((RefValue >> (8U - pField->BitDepth)) & pField->BitMask), 0, 0, 0, 0);

        Serdes_UpdateFieldRegister(pField);
        Serdes_ConfigFieldRegister(ChipID, pField);

        if (Serdes_ComputeIndex(ChipID, &Index, SamplePeriod) == B8_ERR_NONE) {
            /* Keep searching down and then stop */
            Status = SEARCH_STOP;
        } else {
            /* Restore */
            RefValue += Step;
            pField->Value = (UINT32)((RefValue >> (8U - pField->BitDepth)) & pField->BitMask);
            AmbaB8_PrintInt5("++: %d", ((RefValue >> (8U - pField->BitDepth)) & pField->BitMask), 0, 0, 0, 0);

            Serdes_UpdateFieldRegister(pField);
            Serdes_ConfigFieldRegister(ChipID, pField);
            break;
        }
    }

    while (Status != SEARCH_STOP && (RefValue < (128 - Step)) && Index != 0U) {
        /* Try search up  */
        RefValue += Step;
        pField->Value = (UINT32)((RefValue >> (8U - pField->BitDepth)) & pField->BitMask);
        AmbaB8_PrintInt5("++: %d", ((RefValue >> (8U - pField->BitDepth)) & pField->BitMask), 0, 0, 0, 0);

        Serdes_UpdateFieldRegister(pField);
        Serdes_ConfigFieldRegister(ChipID, pField);

        if (Serdes_ComputeIndex(ChipID, &Index, SamplePeriod) == B8_ERR_NONE) {
            /* Search up */
            continue;
        } else {
            /* Restore */
            RefValue -= Step;
            pField->Value = (UINT32)((RefValue >> (8U - pField->BitDepth)) & pField->BitMask);
            AmbaB8_PrintInt5("--: %d\n", ((RefValue >> (8U - pField->BitDepth)) & pField->BitMask), 0, 0, 0, 0);

            Serdes_UpdateFieldRegister(pField);
            Serdes_ConfigFieldRegister(ChipID, pField);
            break;
        }
    }

    Status = SEARCH_STOP;

    AmbaB8_PrintUInt5("[Serdes] ->>> 0x%x", pField->Value, 0U, 0U, 0U, 0U);
    return Index;
}

UINT32 AmbaB8_SerdesCalibration(UINT32 ChipID, UINT32 SamplePeriod, GLOBAL_SEARCH_PARAM_s *pCalibParam)
{
    UINT32 Index;

    if (!(ChipID & B8_SUB_CHIP_ID_MASK)) {
        AmbaB8_PrintStr5("No Serdes Link available", NULL, NULL, NULL, NULL, NULL);
        return B8_ERR_ARG;
    }

    if (pCalibParam == NULL) {
        AmbaB8_PrintStr5("Null calibration struct", NULL, NULL, NULL, NULL, NULL);
        return B8_ERR_ARG;
    }

    Index = Serdes_LinearSearch((ChipID & B8_MAIN_CHIP_ID_B8NF_MASK), &CalFieldTable.Res0, SamplePeriod);
    Index = Serdes_LinearSearch((ChipID & B8_MAIN_CHIP_ID_B8NF_MASK), &CalFieldTable.Res1, SamplePeriod);
    Index = Serdes_LinearSearch((ChipID & B8_MAIN_CHIP_ID_B8NF_MASK), &CalFieldTable.Res2, SamplePeriod);
    Index = Serdes_LinearSearch((ChipID & B8_MAIN_CHIP_ID_B8NF_MASK), &CalFieldTable.Cap0, SamplePeriod);
    Index = Serdes_LinearSearch((ChipID & B8_MAIN_CHIP_ID_B8NF_MASK), &CalFieldTable.Cap1, SamplePeriod);
    Index = Serdes_LinearSearch((ChipID & B8_MAIN_CHIP_ID_B8NF_MASK), &CalFieldTable.Cap2, SamplePeriod);
    Index = Serdes_SignLinearSearch((ChipID & B8_MAIN_CHIP_ID_B8NF_MASK), &CalFieldTable.Tap1, SamplePeriod);
    Index = Serdes_SignLinearSearch((ChipID & B8_MAIN_CHIP_ID_B8NF_MASK), &CalFieldTable.Tap2, SamplePeriod);
    Index = Serdes_SignLinearSearch((ChipID & B8_MAIN_CHIP_ID_B8NF_MASK), &CalFieldTable.Tap3, SamplePeriod);
    Index = Serdes_SignLinearSearch((ChipID & B8_MAIN_CHIP_ID_B8NF_MASK), &CalFieldTable.Tap4, SamplePeriod);

    pCalibParam->Cap = CalFieldTable.Cap0.Value | CalFieldTable.Cap1.Value << 4 | CalFieldTable.Cap2.Value << 8;
    pCalibParam->Res = CalFieldTable.Res0.Value | CalFieldTable.Res1.Value << 2 | CalFieldTable.Res2.Value << 4;
    pCalibParam->Tap1 = CalFieldTable.Tap1.Value;
    pCalibParam->Tap2 = CalFieldTable.Tap2.Value;
    pCalibParam->Tap3 = CalFieldTable.Tap3.Value;
    pCalibParam->Tap4 = CalFieldTable.Tap4.Value;

    AmbaB8_PrintUInt5("[Serdes] Calibration Results w/ Index: %d", Index, 0U, 0U, 0U, 0U);
    AmbaB8_PrintUInt5("Cap:  0x%x", pCalibParam->Cap, 0U, 0U, 0U, 0U);
    AmbaB8_PrintUInt5("Res:  0x%x", pCalibParam->Res, 0U, 0U, 0U, 0U);
    AmbaB8_PrintUInt5("Tap1: 0x%x", pCalibParam->Tap1, 0U, 0U, 0U, 0U);
    AmbaB8_PrintUInt5("Tap2: 0x%x", pCalibParam->Tap2, 0U, 0U, 0U, 0U);
    AmbaB8_PrintUInt5("Tap3: 0x%x", pCalibParam->Tap3, 0U, 0U, 0U, 0U);
    AmbaB8_PrintUInt5("Tap4: 0x%x", pCalibParam->Tap4, 0U, 0U, 0U, 0U);

    return B8_ERR_NONE;
}
#endif

#ifdef B8_DEV_VERSION
/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaB8_SerdesGlobalCalibration
 *
 *  @Description:: B8 SERDES calibration
 *
 *  @Input      ::
 *      ChipID:       B8 chip id
 *      SerDesRate:   SERDES data rate
 *      TimeInterval: Time interval for each phy configuration (ms)
 *      pMemBase:     Pointer to memory base for calibration
 *
 *  @Output     ::
 *      pCalibPhyConfig: Pointer to calibration phy configuration
 *
 *  @Return     ::
 *      int : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
UINT32 AmbaB8_SerdesGlobalCalibration(UINT32 ChipID, UINT32 SerDesRate, UINT32 TimeInterval, void *pMemBase)
{
    UINT32 Seed;
    GLOBAL_SEARCH_PARAM_s CalibPhyConfig = {0};
    extern UINT32 LocalSearchEnable;
#ifdef BUILT_IN_SERDES
    extern void AmbaSERDES_SetLinkStatus(UINT32 ChipID, UINT32 Status);
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

    /* Set random seed */
    AmbaB8_KAL_GetSysTickCount(&Seed);
    AmbaB8_Wrap_srand(Seed);

    /* Force DFE Adaptation Off */
    LocalSearchEnable = 0U;

    /* Initial link */
    (void) AmbaB8_SerdesLink(ChipID, SerDesRate);

    /* Search for the best point */
    AmbaB8_PrintStr5("==================================== Find the best point ====================================", \
                     NULL, NULL, NULL, NULL, NULL);
    Serdes_Anneal_Tune(ChipID, SerDesRate, TimeInterval, BEST_POINT_SEARCH, &CalibPhyConfig, pMemBase);

    AmbaB8_PrintStr5("Best point", NULL, NULL, NULL, NULL, NULL);
    AmbaB8_PrintUInt5("Cap:  0x%x", CalibPhyConfig.Cap, 0U, 0U, 0U, 0U);
    AmbaB8_PrintUInt5("Res:  0x%x", CalibPhyConfig.Res, 0U, 0U, 0U, 0U);
    AmbaB8_PrintUInt5("Tap1: 0x%x", CalibPhyConfig.Tap1, 0U, 0U, 0U, 0U);
    AmbaB8_PrintUInt5("Tap2: 0x%x", CalibPhyConfig.Tap2, 0U, 0U, 0U, 0U);
    AmbaB8_PrintUInt5("Tap3: 0x%x", CalibPhyConfig.Tap3, 0U, 0U, 0U, 0U);
    AmbaB8_PrintUInt5("Tap4: 0x%x", CalibPhyConfig.Tap4, 0U, 0U, 0U, 0U);

    return B8_ERR_NONE;
}
#endif

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaB8_SerdesSetCalibParam
 *
 *  @Description:: Set SERDES calibration parameters
 *
 *  @Input      ::
 *      ChipID:       B8 chip id
 *      pCalibParam:  Pointer to calibration data
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *      int : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
UINT32 AmbaB8_SerdesSetCalibParam(UINT32 ChipID, const GLOBAL_SEARCH_PARAM_s *pCalibParam)
{
    UINT32 RetVal = B8_ERR_NONE;

    if (pCalibParam == NULL) {
        RetVal = B8_ERR_ARG;
    } else {
        if ((ChipID & B8_SUB_CHIP_ID_MASK) != B8_SUB_CHIP_ID_B8N) {
            AmbaB8_PrintUInt5("Cap:  0x%x", pCalibParam->Cap, 0U, 0U, 0U, 0U);
            AmbaB8_PrintUInt5("Res:  0x%x", pCalibParam->Res, 0U, 0U, 0U, 0U);
            AmbaB8_PrintUInt5("Tap1: 0x%x", pCalibParam->Tap1, 0U, 0U, 0U, 0U);
            AmbaB8_PrintUInt5("Tap2: 0x%x", pCalibParam->Tap2, 0U, 0U, 0U, 0U);
            AmbaB8_PrintUInt5("Tap3: 0x%x", pCalibParam->Tap3, 0U, 0U, 0U, 0U);
            AmbaB8_PrintUInt5("Tap4: 0x%x", pCalibParam->Tap4, 0U, 0U, 0U, 0U);
            Serdes_UpdatePhySetting(ChipID, pCalibParam);
        } else {
            RetVal = B8_ERR_ARG;
        }
    }

    return RetVal;
}


