/**
 *  @file AmbaSbrg_Max96707_9286.c
 *
 *  @copyright Copyright (c) 2018 Ambarella, Inc.
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
 *  @details MAX96707 & MAX9286 driver APIs
 *
 */

#include "AmbaKAL.h"
#include "AmbaI2C.h"
#include "AmbaPrint.h"
#include "AmbaVIN.h"

#include "AmbaSbrg_Max96707_9286.h"
#include "bsp.h"

#define IDC_ADDR_LINK0  0x2U
#define IDC_ADDR_LINK1  0x4U
#define IDC_ADDR_LINK2  0x6U
#define IDC_ADDR_LINK3  0x8U
#define IDC_ADDR_ALL    0xAU

static MAX96707_9286_SERDES_INFO_s MaxSerDesInfo[AMBA_NUM_VIN_CHANNEL];

static UINT32 MAX96707_9286I2cChannel[AMBA_NUM_VIN_CHANNEL] = {
    [AMBA_VIN_CHANNEL0] = AMBA_MAXIM_I2C_CHANNEL,
    [AMBA_VIN_CHANNEL1] = AMBA_MAXIM_I2C_CHANNEL_PIP,
    [AMBA_VIN_CHANNEL2] = AMBA_MAXIM_I2C_CHANNEL_PIP2,
    [AMBA_VIN_CHANNEL3] = AMBA_MAXIM_I2C_CHANNEL_PIP3,
    [AMBA_VIN_CHANNEL4] = AMBA_MAXIM_I2C_CHANNEL_PIP4,
    [AMBA_VIN_CHANNEL5] = AMBA_MAXIM_I2C_CHANNEL_PIP5,
};

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: Max96707_9286_RegWrite
 *
 *  @Description:: Write Max96707_9286 registers
 *
 *  @Input      ::
 *      SlaveID:   Slave id
 *      Addr:      Register address
 *      Data:      Register value
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *      int : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static UINT32 Max96707_9286_RegWrite(UINT32 VinID, UINT8 SlaveID, UINT8 Addr, UINT8 Data)
{
    UINT32 RetVal;
    AMBA_I2C_TRANSACTION_s I2cConfig = {0};

    UINT8 TxDataBuf[2];
    UINT32 TxSize;

    I2cConfig.SlaveAddr = SlaveID;
    I2cConfig.DataSize  = 2U;
    I2cConfig.pDataBuf  = TxDataBuf;

    TxDataBuf[0] = Addr;
    TxDataBuf[1] = Data;

    RetVal = AmbaI2C_MasterWrite(MAX96707_9286I2cChannel[VinID], AMBA_I2C_SPEED_FAST, &I2cConfig, &TxSize, 1000U);

    if (RetVal != I2C_ERR_NONE) {
        AmbaPrint_ModulePrintStr5(MAX96707_9286_MODULE_ID, "I2C does not work!!!!!", NULL, NULL, NULL, NULL, NULL);
    }

    AmbaPrint_ModulePrintUInt5(MAX96707_9286_MODULE_ID, "[W] ====> 0x%02x 0x%02x 0x%02x", SlaveID, Addr, Data, 0U, 0U);

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: Max96707_9286_RegRead
 *
 *  @Description:: Read Max96707_9286 registers
 *
 *  @Input      ::
 *      SlaveID:   Slave id
 *      Addr:      Register address
 *      pRxData:   Pointer to register value
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *      int : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static UINT32 Max96707_9286_RegRead(UINT32 VinID, UINT8 SlaveID, UINT8 Addr, UINT8 *pRxData)
{
    UINT32 RetVal;
    AMBA_I2C_TRANSACTION_s I2cTxConfig = {0};
    AMBA_I2C_TRANSACTION_s I2cRxConfig = {0};
    UINT8 TxDataBuf;
    UINT32 TxSize;

    I2cTxConfig.SlaveAddr = SlaveID;
    I2cTxConfig.DataSize  = 1U;
    I2cTxConfig.pDataBuf  = &TxDataBuf;

    I2cRxConfig.SlaveAddr = SlaveID + (UINT32)1U;
    I2cRxConfig.DataSize  = 1U;
    I2cRxConfig.pDataBuf  = pRxData;

    TxDataBuf = Addr;
    RetVal = AmbaI2C_MasterReadAfterWrite(MAX96707_9286I2cChannel[VinID], AMBA_I2C_SPEED_FAST, 1U,
                                          &I2cTxConfig, &I2cRxConfig, &TxSize, 1000U);

    if (RetVal != I2C_ERR_NONE) {
        AmbaPrint_ModulePrintStr5(MAX96707_9286_MODULE_ID, "I2C does not work!!!!!", NULL, NULL, NULL, NULL, NULL);
    }

    //AmbaPrint_ModulePrintUInt5(MAX96707_9286_MODULE_ID, "[R] ====> 0x%02x 0x%02x", SlaveID, Addr, 0U, 0U, 0U);

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: Max96707_9286_GetNumInputLinks
 *
 *  @Description:: Get number of input links according to SensorID
 *
 *  @Input      ::
 *      SensorID:   Sensor ID
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          int :   Number of input links
\*-----------------------------------------------------------------------------------------------*/
static UINT8 Max96707_9286_GetNumInputLinks(UINT32 SensorID)
{
    UINT8 NumInputLinks = 0U;
    UINT32 SensorIDToCheck = SensorID;

    while (SensorIDToCheck != 0U) {
        NumInputLinks += (UINT8)SensorIDToCheck & 0x1U;
        SensorIDToCheck >>= 1U;
    }

    return NumInputLinks;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: Max96707_9286_GetNumCSIOutputLanes
 *
 *  @Description:: Get number of CSI-2 output lanes according to SensorID
 *
 *  @Input      ::
 *      SensorID:   Sensor ID
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          int :   Number of CSI-2 output lanes
\*-----------------------------------------------------------------------------------------------*/
static UINT8 Max96707_9286_GetNumCSIOutputLanes(UINT32 SensorID)
{
    return Max96707_9286_GetNumInputLinks(SensorID);
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: Max96707_9286_GetLinkOutputOrder
 *
 *  @Description:: Get link output order config
 *
 *  @Input      ::
 *      SensorID:   Sensor ID
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          int :   Link output order config
\*-----------------------------------------------------------------------------------------------*/
static UINT8 Max96707_9286_GetLinkOutputOrder(UINT32 EnabledLinkID)
{
    UINT32 i = 0, j = 0, k = 3U;
    UINT8 OutputOrder = 0x0U;

    for (; i < 4U; i++) {
        if ((EnabledLinkID & ((UINT32)1U << i)) != 0U) {
            OutputOrder |= (UINT8)(j << (i << 1U));
            j++;
        } else {
            OutputOrder |= (UINT8)(k << (i << 1U));
            k--;
        }
    }

    return OutputOrder;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: Max96707_9286_FindArrayIndex
 *
 *  @Description:: Search the index of the given data
 *
 *  @Input      ::
 *      Array:   The array to be searched
 *      Data:    The given data
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *      UINT32 : The index value of the given data
\*-----------------------------------------------------------------------------------------------*/
static UINT32 Max96707_9286_FindArrayIndex(const UINT8* Array, UINT32 ArraySize, UINT8 Data)
{
    UINT32 i;
    UINT32 index = 0U;

    for (i = 0U; i < ArraySize; i++) {
        if (Array[i] == Data) {
            index = i;
            break;
        }
    }

    return index;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: Max96707_9286_GetDesSlaveID
 *
 *  @Description:: Search Deserializer Slave ID
 *
 *  @Input      ::
 *      VinID:   Vin ID
 *      RetDesSlaveID: Address to save deserializer Slave ID found
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *      UINT32 : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static UINT32 Max96707_9286_GetDesSlaveID(UINT32 VinID, UINT8* RetDesSlaveID)
{
    /* All possible Max9286 I2c address */
    static const UINT8 DesSlaveID[NUM_IDC_ADDR_MAX9286] = {
        [0] = IDC_ADDR_MAX9286_A,
        [1] = IDC_ADDR_MAX9286_B,
    };

    /* Recording used I2c address and bus to avoid conflict. 0U = available, 1U = used */
    static UINT32 UsedSlaveID[NUM_IDC_ADDR_MAX9286][AMBA_NUM_I2C_CHANNEL] = {
        {0U, 0U, 0U, 0U},  //IDC_ADDR_MAX9286_A
        {0U, 0U, 0U, 0U},  //IDC_ADDR_MAX9286_B
    };

    /* User-defined VIN<->SlaveID mapping */
    static const UINT8 DefinedSlaveID[AMBA_NUM_VIN_CHANNEL] = {
        [AMBA_VIN_CHANNEL0] = IDC_ADDR_MAX9286_VIN0,
        [AMBA_VIN_CHANNEL1] = IDC_ADDR_MAX9286_VIN1,
        [AMBA_VIN_CHANNEL2] = IDC_ADDR_MAX9286_VIN2,
        [AMBA_VIN_CHANNEL3] = IDC_ADDR_MAX9286_VIN3,
        [AMBA_VIN_CHANNEL4] = IDC_ADDR_MAX9286_VIN4,
        [AMBA_VIN_CHANNEL5] = IDC_ADDR_MAX9286_VIN5,
    };

    UINT32 RetVal = MAX96707_9286_ERR_NONE;
    UINT32 index, i;
    UINT8 RegData = 0U;
    UINT8 RetSlaveID = 0xffU;  /* Default uninitialized Max9286 I2c address */

    AmbaPrint_ModulePrintStr5(MAX96707_9286_MODULE_ID, "Max9286 I2C address scan:(Please ignore the following \"I2C does not work\" message.)", NULL, NULL, NULL, NULL, NULL);
    index = Max96707_9286_FindArrayIndex(DesSlaveID, NUM_IDC_ADDR_MAX9286, DefinedSlaveID[VinID]);  /* Scanning start from defined SlaveID first */

    for (i = index; i < (index + NUM_IDC_ADDR_MAX9286); i++) {
        /* Cicular array traverse since i may not start from 0 */
        if(UsedSlaveID[i % NUM_IDC_ADDR_MAX9286][MAX96707_9286I2cChannel[VinID]] == 1U) {
            /* Skip I2c addr that conflicts on the same I2c channel */
            AmbaPrint_ModulePrintUInt5(MAX96707_9286_MODULE_ID, "Max9286 I2C address:0x%x on i2c channel:%d has been used.", DesSlaveID[i % NUM_IDC_ADDR_MAX9286], MAX96707_9286I2cChannel[VinID], 0U, 0U, 0U);
        } else {
            AmbaPrint_ModulePrintUInt5(MAX96707_9286_MODULE_ID, "Trying addr:0x%x ...", DesSlaveID[i % NUM_IDC_ADDR_MAX9286], 0U, 0U, 0U, 0U);
            RetVal = Max96707_9286_RegRead(VinID, DesSlaveID[i % NUM_IDC_ADDR_MAX9286], 0x09U, &RegData);  /* Read R0x09 to get Max9286 SlaveID */

            if ((RetVal == MAX96707_9286_ERR_NONE) && (RegData == DesSlaveID[i % NUM_IDC_ADDR_MAX9286])) {
                RetSlaveID = DesSlaveID[i % NUM_IDC_ADDR_MAX9286];
                UsedSlaveID[i % NUM_IDC_ADDR_MAX9286][MAX96707_9286I2cChannel[VinID]] = 1U;
                AmbaPrint_ModulePrintUInt5(MAX96707_9286_MODULE_ID, "Max9286 I2C address:0x%x found on Vin%d.", RetSlaveID, VinID, 0U, 0U, 0U);
                break;
            }
        }
    }

    if (RetSlaveID == 0xffU) {
        AmbaPrint_ModulePrintUInt5(MAX96707_9286_MODULE_ID, "No Max9286 I2C address found on Vin%d, GetDesSlaveID failed!!", VinID, 0U, 0U, 0U, 0U);
        RetVal = MAX96707_9286_ERR_ARG;
    }

    if (RetSlaveID != DefinedSlaveID[VinID]) {
        AmbaPrint_ModulePrintUInt5(MAX96707_9286_MODULE_ID, "Please ensure found Max9286 I2C address:0x%x matches with the definition:0x%x in header file.", RetSlaveID, DefinedSlaveID[VinID], 0U, 0U, 0U);
    }

    *RetDesSlaveID = RetSlaveID;  /* Store the I2c address found */
    AmbaPrint_ModulePrintStr5(MAX96707_9286_MODULE_ID, "Max9286 I2C address scan done!", NULL, NULL, NULL, NULL, NULL);

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: Max96707_9286_Init
 *
 *  @Description:: Init Max96707_9286 API
 *
 *  @Input      ::
 *      DesSlaveID:   DeSerializer i2c slave addr
 *      SerSlaveID:   Serializer i2c addr
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *      int :      OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
UINT32 Max96707_9286_Init(UINT32 VinID)
{
    MAX96707_9286_SERDES_INFO_s *CurSerDesInfo = &MaxSerDesInfo[VinID];
    UINT32 RetVal = MAX96707_9286_ERR_NONE;

    if (CurSerDesInfo->InitDone == 0U) {
        /* Create a mutex */
        if (AmbaKAL_MutexCreate(&CurSerDesInfo->Mutex, NULL) != KAL_ERR_NONE) {
            AmbaPrint_ModulePrintStr5(MAX96707_9286_MODULE_ID, "[Max96707_9286] AmbaKAL_MutexCreate() failed!", NULL, NULL, NULL, NULL, NULL);
            RetVal |= MAX96707_9286_ERR_MUTEX;
        } else {
            RetVal |= Max96707_9286_GetDesSlaveID(VinID, &CurSerDesInfo->DesAddr);
            CurSerDesInfo->SerAddr0 = (UINT8)(IDC_ADDR_MAX96707 + IDC_ADDR_LINK0);
            CurSerDesInfo->SerAddr1 = (UINT8)(IDC_ADDR_MAX96707 + IDC_ADDR_LINK1);
            CurSerDesInfo->SerAddr2 = (UINT8)(IDC_ADDR_MAX96707 + IDC_ADDR_LINK2);
            CurSerDesInfo->SerAddr3 = (UINT8)(IDC_ADDR_MAX96707 + IDC_ADDR_LINK3);
            CurSerDesInfo->SerAddrBroadCast = (UINT8)(IDC_ADDR_MAX96707 + IDC_ADDR_ALL);
            CurSerDesInfo->InitDone = 1U;
        }
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: Max96707_9286_Config
 *
 *  @Description:: Configure Max96707_9286 API
 *
 *  @Input      ::
 *      pSerdesConfig:   pointer to Serdes configuation
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *      int :      OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
UINT32 Max96707_9286_Config(UINT32 VinID, const MAX96707_9286_SERDES_CONFIG_s *pSerdesConfig)
{
    UINT32 RetVal = MAX96707_9286_ERR_NONE, RetValTmp = KAL_ERR_NONE;
    MAX96707_9286_SERDES_INFO_s *CurSerDesInfo = &MaxSerDesInfo[VinID];
    UINT8 BWS, HIM, CSICfg, LinkCfg, OutOrd, Config, PolCfg;
    UINT8 RxData;
    UINT8 SensorSlaveID, NumDataBits;
    UINT32 FSyncPeriod[3];
    UINT32 EnabledLinkID;

    RetVal |= Max96707_9286_Init(VinID);

    if (RetVal == MAX96707_9286_ERR_NONE) {
        /* Take the Mutex */
        if (AmbaKAL_MutexTake(&CurSerDesInfo->Mutex, AMBA_KAL_WAIT_FOREVER) != KAL_ERR_NONE) {
            RetVal = MAX96707_9286_ERR_MUTEX;  /* should never happen */
        } else {

            if (pSerdesConfig == NULL) {
                RetVal = MAX96707_9286_ERR_ARG;
            } else {
                EnabledLinkID = pSerdesConfig->EnabledLinkID;
                SensorSlaveID = pSerdesConfig->SensorSlaveID;
                NumDataBits = pSerdesConfig->NumDataBits;

                /* Read Max9286 power-up config */
                RetVal |= Max96707_9286_RegRead(VinID, CurSerDesInfo->DesAddr, 0x1c, &RxData);
                BWS = RxData & 0x1U;
                HIM = ((RxData >> 4U) != 0U) ? 1U : 0U;
                AmbaPrint_ModulePrintUInt5(MAX96707_9286_MODULE_ID, "Init Max96707 & Max9286 (BWS=%d, HIM=%d) ...", BWS, HIM, 0U, 0U, 0U);

                /* Prepare config data for Max9286 */
                LinkCfg = 0xe0U | (UINT8)EnabledLinkID;
                if (pSerdesConfig->PixelRate < 25000000U) {
                    CSICfg  = (UINT8)((Max96707_9286_GetNumCSIOutputLanes(EnabledLinkID) - 1U) << 6U); /* CSIDBL = 0, DBL = 0 */
                } else {
                    CSICfg  = (UINT8)(0x30U | ((Max96707_9286_GetNumCSIOutputLanes(EnabledLinkID) - 1U) << 6U)); /* CSIDBL = 1, DBL = 1 */
                }
                CSICfg |= (NumDataBits >> 1U) + 1U; /* RAW8: 0x5, RAW10: 0x6, RAW12: 0x7, RAW14: 0x8 */
                OutOrd = Max96707_9286_GetLinkOutputOrder(EnabledLinkID);

                /* Prepare config data for Max96707 */
                if (pSerdesConfig->PixelRate < 25000000U) {
                    Config  = 0x04U | (UINT8)(BWS << 5U); /* DBL = 0 */
                } else {
                    Config  = 0x84U | (UINT8)(BWS << 5U); /* DBL = 1 */
                }

                /* Prepare H/Vsync polarity setting */
                RetVal |= Max96707_9286_RegRead(VinID, CurSerDesInfo->DesAddr, 0x0c, &PolCfg);
                if (pSerdesConfig->VSyncPolarity == MAX96707_9286_ACTIVE_LOW) {
                    PolCfg |= 0x08U; /* Set INVVS (bit[3]) */
                } else {  /* Default is active high */
                    PolCfg &= 0xf7U; /* Clear INVVS (bit[3]) */
                }

                if (pSerdesConfig->HSyncPolarity == MAX96707_9286_ACTIVE_LOW) {
                    PolCfg |= 0x04U; /* Set INVHS (bit[2]) */
                } else {  /* Default is active high */
                    PolCfg &= 0xfbU; /* Clear INVHS (bit[2]) */
                }

                /*-----------------------------------------------------------------------*\
                 * Reset all Max96707 POC by Max9286 GPIO0
                \*-----------------------------------------------------------------------*/
                RetVal |= Max96707_9286_RegWrite(VinID, CurSerDesInfo->DesAddr, 0x0f, 0xa);
                (void) AmbaKAL_TaskSleep(100);
                RetVal |= Max96707_9286_RegWrite(VinID, CurSerDesInfo->DesAddr, 0x0f, 0xb);
                (void) AmbaKAL_TaskSleep(150);

                /*-----------------------------------------------------------------------*\
                 * Reverse Control Channel Setup
                \*-----------------------------------------------------------------------*/
                if (HIM != 0U) {
                    RetVal |= Max96707_9286_RegWrite(VinID, IDC_ADDR_MAX96707, 0x04, 0x43);             /* Enable Configuration Link */
                    (void) AmbaKAL_TaskSleep(2);
                    RetVal |= Max96707_9286_RegWrite(VinID, IDC_ADDR_MAX96707, 0x4d, 0xc0);             /* Select coax cable mode */
                    (void) AmbaKAL_TaskSleep(2);
                    RetVal |= Max96707_9286_RegWrite(VinID, IDC_ADDR_MAX96707, 0x06, 0xa9);             /* 2.2dB preemphasis for 8m cable, 0xaa:3.3dB */
                    (void) AmbaKAL_TaskSleep(2);
                } else {
                    RetVal |= Max96707_9286_RegWrite(VinID, CurSerDesInfo->DesAddr, 0x3f, 0x4f);        /* Enable Custom Reverse Channel & First Pulse Length */
                    (void) AmbaKAL_TaskSleep(2);
                    RetVal |= Max96707_9286_RegWrite(VinID, CurSerDesInfo->DesAddr, 0x3b, 0x1e);        /* Reverse Channel Amplitude to mid level and transition time */
                    (void) AmbaKAL_TaskSleep(2);
                    RetVal |= Max96707_9286_RegWrite(VinID, IDC_ADDR_MAX96707, 0x04, 0x43);             /* Enable Configuration Link */
                    (void) AmbaKAL_TaskSleep(2);
                    RetVal |= Max96707_9286_RegWrite(VinID, IDC_ADDR_MAX96707, 0x08, 0x01);             /* Serializer reverse channel input gain and thresholds */
                    (void) AmbaKAL_TaskSleep(2);
                    RetVal |= Max96707_9286_RegWrite(VinID, IDC_ADDR_MAX96707, 0x97, 0x5f);             /* Serializer reverse channel input gain and thresholds */
                    (void) AmbaKAL_TaskSleep(2);
                    RetVal |= Max96707_9286_RegWrite(VinID, CurSerDesInfo->DesAddr, 0x3b, 0x19);        /* Reverse Channel Amplitude level */
                    (void) AmbaKAL_TaskSleep(2);
                    RetVal |= Max96707_9286_RegWrite(VinID, IDC_ADDR_MAX96707, 0x4d, 0x40);             /* Select coax cable mode */
                    (void) AmbaKAL_TaskSleep(2);
                    RetVal |= Max96707_9286_RegWrite(VinID, IDC_ADDR_MAX96707, 0x06, 0xa9);             /* 2.2dB preemphasis for 8m cable */
                    (void) AmbaKAL_TaskSleep(2);
                }

                /*-----------------------------------------------------------------------*\
                 * MAX9286 Pre MAX96707 Setup
                \*-----------------------------------------------------------------------*/
                RetVal |= Max96707_9286_RegWrite(VinID, CurSerDesInfo->DesAddr, 0x15, 0x03);            /* Disable CSI output */
                RetVal |= Max96707_9286_RegWrite(VinID, CurSerDesInfo->DesAddr, 0x12, CSICfg);          /* CSI Lanes, CSI DBL, GMSL DBL, Data Type */
                RetVal |= Max96707_9286_RegWrite(VinID, CurSerDesInfo->DesAddr, 0x0c, PolCfg);          /* H/Vsync polarity */
                RetVal |= Max96707_9286_RegWrite(VinID, CurSerDesInfo->DesAddr, 0x00, LinkCfg);         /* Enable GMSL Links */
                RetVal |= Max96707_9286_RegWrite(VinID, CurSerDesInfo->DesAddr, 0x69, 0x3f);            /* Enable Auto Comeback and Auto Masking */
                RetVal |= Max96707_9286_RegWrite(VinID, CurSerDesInfo->DesAddr, 0x0b, OutOrd);          /* Links in the CSI output order */

                if (pSerdesConfig->FrameSyncPeriod != 0U) {
                    FSyncPeriod[0] = (pSerdesConfig->FrameSyncPeriod & 0x0000ffU);                      /* FSYNC PERIOD_L */
                    FSyncPeriod[1] = (pSerdesConfig->FrameSyncPeriod & 0x00ff00U) >> 8;                 /* FSYNC PERIOD_M */
                    FSyncPeriod[2] = (pSerdesConfig->FrameSyncPeriod & 0xff0000U) >> 16;                /* FSYNC PERIOD_H */
                    RetVal |= Max96707_9286_RegWrite(VinID, CurSerDesInfo->DesAddr, 0x06, (UINT8)FSyncPeriod[0]);
                    RetVal |= Max96707_9286_RegWrite(VinID, CurSerDesInfo->DesAddr, 0x07, (UINT8)FSyncPeriod[1]);
                    RetVal |= Max96707_9286_RegWrite(VinID, CurSerDesInfo->DesAddr, 0x08, (UINT8)FSyncPeriod[2]);
                    RetVal |= Max96707_9286_RegWrite(VinID, CurSerDesInfo->DesAddr, 0x01, 0x00);        /* Internal frame Sync Manual */
                } else {
                    RetVal |= Max96707_9286_RegWrite(VinID, CurSerDesInfo->DesAddr, 0x01, 0xe0);        /* External frame Sync */
                }

                /*-----------------------------------------------------------------------*\
                 * Setup Link 0
                \*-----------------------------------------------------------------------*/
                if ((EnabledLinkID & 0x1U) != 0U) {
                    RetVal |= Max96707_9286_RegWrite(VinID, CurSerDesInfo->DesAddr, 0x0a, 0xf1);                             /* Enable Link 0 Reverse Channel */
                    RetVal |= Max96707_9286_RegWrite(VinID, IDC_ADDR_MAX96707, 0x00, CurSerDesInfo->SerAddr0);               /* Change serializer I2C Slave Address */
                    RetVal |= Max96707_9286_RegWrite(VinID, CurSerDesInfo->SerAddr0, 0x07, Config);                          /* DBL, Edge Select, HS/VS Encoding */
                    (void) AmbaKAL_TaskSleep(2);
                    RetVal |= Max96707_9286_RegWrite(VinID, CurSerDesInfo->SerAddr0, 0x09, SensorSlaveID + IDC_ADDR_LINK0);  /* 0x82 Unique Link 0 Image Sensor I2C Slave Address */
                    RetVal |= Max96707_9286_RegWrite(VinID, CurSerDesInfo->SerAddr0, 0x0a, SensorSlaveID);                   /* 0x80 Link 0 Image Sensor Address */
                    RetVal |= Max96707_9286_RegWrite(VinID, CurSerDesInfo->SerAddr0, 0x0b, CurSerDesInfo->SerAddrBroadCast); /* Serializer Broadcast Address */
                    RetVal |= Max96707_9286_RegWrite(VinID, CurSerDesInfo->SerAddr0, 0x0c, CurSerDesInfo->SerAddr0);         /* Link 0 Serializer Address */
                }
                /*-----------------------------------------------------------------------*\
                 * Setup Link 1
                \*-----------------------------------------------------------------------*/
                if ((EnabledLinkID & 0x2U) != 0U) {
                    RetVal |= Max96707_9286_RegWrite(VinID, CurSerDesInfo->DesAddr, 0x0a, 0xf2);                             /* Enable Link 1 Reverse Channel */
                    RetVal |= Max96707_9286_RegWrite(VinID, IDC_ADDR_MAX96707, 0x00, CurSerDesInfo->SerAddr1);               /* Change serializer I2C Slave Address */
                    RetVal |= Max96707_9286_RegWrite(VinID, CurSerDesInfo->SerAddr1, 0x07, Config);                          /* DBL, Edge Select, HS/VS Encoding */
                    (void) AmbaKAL_TaskSleep(2);
                    RetVal |= Max96707_9286_RegWrite(VinID, CurSerDesInfo->SerAddr1, 0x09, SensorSlaveID + IDC_ADDR_LINK1);  /* Unique Link 1 Image Sensor I2C Slave Address */
                    RetVal |= Max96707_9286_RegWrite(VinID, CurSerDesInfo->SerAddr1, 0x0a, SensorSlaveID);                   /* Link 1 Image Sensor Address */
                    RetVal |= Max96707_9286_RegWrite(VinID, CurSerDesInfo->SerAddr1, 0x0b, CurSerDesInfo->SerAddrBroadCast); /* Serializer Broadcast Address */
                    RetVal |= Max96707_9286_RegWrite(VinID, CurSerDesInfo->SerAddr1, 0x0c, CurSerDesInfo->SerAddr1);         /* Link 1 Serializer Address */
                }
                /*-----------------------------------------------------------------------*\
                 * Setup Link 2
                \*-----------------------------------------------------------------------*/
                if ((EnabledLinkID & 0x4U) != 0U) {
                    RetVal |= Max96707_9286_RegWrite(VinID, CurSerDesInfo->DesAddr, 0x0a, 0xf4);                             /* Enable Link 2 Reverse Channel */
                    RetVal |= Max96707_9286_RegWrite(VinID, IDC_ADDR_MAX96707, 0x00, CurSerDesInfo->SerAddr2);               /* Change serializer I2C Slave Address */
                    RetVal |= Max96707_9286_RegWrite(VinID, CurSerDesInfo->SerAddr2, 0x07, Config);                          /* DBL, Edge Select, HS/VS Encoding */
                    (void) AmbaKAL_TaskSleep(2);
                    RetVal |= Max96707_9286_RegWrite(VinID, CurSerDesInfo->SerAddr2, 0x09, SensorSlaveID + IDC_ADDR_LINK2);  /* Unique Link 2 Image Sensor I2C Slave Address */
                    RetVal |= Max96707_9286_RegWrite(VinID, CurSerDesInfo->SerAddr2, 0x0a, SensorSlaveID);                   /* Link 2 Image Sensor Address */
                    RetVal |= Max96707_9286_RegWrite(VinID, CurSerDesInfo->SerAddr2, 0x0b, CurSerDesInfo->SerAddrBroadCast); /* Serializer Broadcast Address */
                    RetVal |= Max96707_9286_RegWrite(VinID, CurSerDesInfo->SerAddr2, 0x0c, CurSerDesInfo->SerAddr2);         /* Link 2 Serializer Address */
                }
                /*-----------------------------------------------------------------------*\
                 * Setup Link 3
                \*-----------------------------------------------------------------------*/
                if ((EnabledLinkID & 0x8U) != 0U) {
                    RetVal |= Max96707_9286_RegWrite(VinID, CurSerDesInfo->DesAddr, 0x0a, 0xf8);                             /* Enable Link 3 Reverse Channel */
                    RetVal |= Max96707_9286_RegWrite(VinID, IDC_ADDR_MAX96707, 0x00, CurSerDesInfo->SerAddr3);               /* Change serializer I2C Slave Address */
                    RetVal |= Max96707_9286_RegWrite(VinID, CurSerDesInfo->SerAddr3, 0x07, Config);                          /* DBL, Edge Select, HS/VS Encoding */
                    (void) AmbaKAL_TaskSleep(2);
                    RetVal |= Max96707_9286_RegWrite(VinID, CurSerDesInfo->SerAddr3, 0x09, SensorSlaveID + IDC_ADDR_LINK3);  /* Unique Link 3 Image Sensor I2C Slave Address */
                    RetVal |= Max96707_9286_RegWrite(VinID, CurSerDesInfo->SerAddr3, 0x0a, SensorSlaveID);                   /* Link 3 Image Sensor Address */
                    RetVal |= Max96707_9286_RegWrite(VinID, CurSerDesInfo->SerAddr3, 0x0b, CurSerDesInfo->SerAddrBroadCast); /* Serializer Broadcast Address */
                    RetVal |= Max96707_9286_RegWrite(VinID, CurSerDesInfo->SerAddr3, 0x0c, CurSerDesInfo->SerAddr3);         /* Link 3 Serializer Address */
                }

                /*-----------------------------------------------------------------------*\
                 * MAX9286 Post MAX96707 Setup
                \*-----------------------------------------------------------------------*/
                RetVal |= Max96707_9286_RegWrite(VinID, CurSerDesInfo->DesAddr, 0x0a, 0xff);                                 /* Enable all I2C reverse and forward channels */
                RetVal |= Max96707_9286_RegWrite(VinID, CurSerDesInfo->DesAddr, 0x34, 0x36);                                 /* Disable auto ACK for camera init. -Optional- */

                AmbaPrint_ModulePrintStr5(MAX96707_9286_MODULE_ID, "Init Max96707 and Max9286 done!", NULL, NULL, NULL, NULL, NULL);
            }

            /* Release the Mutex */
            RetValTmp = AmbaKAL_MutexGive(&CurSerDesInfo->Mutex);
        }
    }

    return (RetValTmp == KAL_ERR_NONE) ? RetVal : MAX96707_9286_ERR_MUTEX;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: Max96707_9286_EnableVsRetiming
 *
 *  @Description:: Configure Vsync pulse width and timing.
 *                 HW(sensor) configuration dependent.
 *                 Need to call Max96707_9286_Config first before using this API.
 *
 *  @Input      ::
 *      pRetimingConfig:   pointer to retiming configuation
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *      int :      OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
UINT32 Max96707_9286_EnableVsRetiming(UINT32 VinID, UINT32 ChipID, const MAX96707_9286_RETIMING_s *pRetimingConfig)
{
    UINT32 RetVal = MAX96707_9286_ERR_NONE;
    const MAX96707_9286_SERDES_INFO_s *CurSerDesInfo = &MaxSerDesInfo[VinID];
    UINT8 SerAddr = CurSerDesInfo->SerAddr0;
    UINT8 SyncGenCfg;
    UINT8 VS_H[3], VS_L[3], VS_Delay[3];

    RetVal |= Max96707_9286_Init(VinID);

    if (ChipID == MAX96707_LINK0_ID) {
        SerAddr = CurSerDesInfo->SerAddr0;
    } else if (ChipID == MAX96707_LINK1_ID) {
        SerAddr = CurSerDesInfo->SerAddr1;
    } else if (ChipID == MAX96707_LINK2_ID) {
        SerAddr = CurSerDesInfo->SerAddr2;
    } else if (ChipID == MAX96707_LINK3_ID) {
        SerAddr = CurSerDesInfo->SerAddr3;
    } else if (ChipID == MAX96707_ALL_ID) {
        SerAddr = CurSerDesInfo->SerAddrBroadCast;
    } else {
        AmbaPrint_ModulePrintUInt5(MAX96707_9286_MODULE_ID, "Not supported ChipID: 0x%x!", ChipID, 0U, 0U, 0U, 0U);
        RetVal = MAX96707_9286_ERR_ARG;
    }

    if (RetVal == MAX96707_9286_ERR_NONE) {
        /* Boundary check */
        if ((pRetimingConfig->VsHighWidth < 1U) || (pRetimingConfig->VsLowWidth < 1U)
            || ((pRetimingConfig->VsHighWidth + pRetimingConfig->VsLowWidth) >= 0xffffffU)) {

            AmbaPrint_ModulePrintUInt5(MAX96707_9286_MODULE_ID, "Invalid pulse width (H: %d, L: %d)!!", pRetimingConfig->VsHighWidth, pRetimingConfig->VsLowWidth, 0U, 0U, 0U);
            RetVal = MAX96707_9286_ERR_ARG;

        } else if (pRetimingConfig->VsDelay >= (pRetimingConfig->VsHighWidth + pRetimingConfig->VsLowWidth)) {
            AmbaPrint_ModulePrintUInt5(MAX96707_9286_MODULE_ID, "Invalid Vsync delay: %d !!", pRetimingConfig->VsDelay, 0U, 0U, 0U, 0U);
            RetVal = MAX96707_9286_ERR_ARG;

        } else {
            /* Vsync Retiming config */
            VS_H[0] = (UINT8)(pRetimingConfig->VsHighWidth & 0x0000ffU);
            VS_H[1] = (UINT8)((pRetimingConfig->VsHighWidth & 0x00ff00U) >> 8U);
            VS_H[2] = (UINT8)((pRetimingConfig->VsHighWidth & 0xff0000U) >> 16U);

            VS_L[0] = (UINT8)(pRetimingConfig->VsLowWidth & 0x0000ffU);
            VS_L[1] = (UINT8)((pRetimingConfig->VsLowWidth & 0x00ff00U) >> 8U);
            VS_L[2] = (UINT8)((pRetimingConfig->VsLowWidth & 0xff0000U) >> 16U);

            VS_Delay[0] = (UINT8)(pRetimingConfig->VsDelay & 0x0000ffU);
            VS_Delay[1] = (UINT8)((pRetimingConfig->VsDelay & 0x00ff00U) >> 8U);
            VS_Delay[2] = (UINT8)((pRetimingConfig->VsDelay & 0xff0000U) >> 16U);

            RetVal |= Max96707_9286_RegRead(VinID, SerAddr, 0x43, &SyncGenCfg);
            SyncGenCfg |= 0x20U; /* Set GEN_VS (bit[5]) */

            RetVal |= Max96707_9286_RegWrite(VinID, SerAddr, 0x43, SyncGenCfg);
            RetVal |= Max96707_9286_RegWrite(VinID, SerAddr, 0x49, VS_H[0]);
            RetVal |= Max96707_9286_RegWrite(VinID, SerAddr, 0x48, VS_H[1]);
            RetVal |= Max96707_9286_RegWrite(VinID, SerAddr, 0x47, VS_H[2]);
            RetVal |= Max96707_9286_RegWrite(VinID, SerAddr, 0x4c, VS_L[0]);
            RetVal |= Max96707_9286_RegWrite(VinID, SerAddr, 0x4b, VS_L[1]);
            RetVal |= Max96707_9286_RegWrite(VinID, SerAddr, 0x4a, VS_L[2]);
            RetVal |= Max96707_9286_RegWrite(VinID, SerAddr, 0x46, VS_Delay[0]);
            RetVal |= Max96707_9286_RegWrite(VinID, SerAddr, 0x45, VS_Delay[1]);
            RetVal |= Max96707_9286_RegWrite(VinID, SerAddr, 0x44, VS_Delay[2]);
        }
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: Max96707_9286_ReverseDataPin
 *
 *  @Description:: Reverse Max96707 parallel data input pin mapping.
 *                 HW(sensor) configuration dependent.
 *                 Need to call Max96707_9286_Config first before using this API.
 *  @Input      ::
 *      NumDataBits:   bit/width of input pixel
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *      int :      OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
UINT32 Max96707_9286_ReverseDataPin(UINT32 VinID, UINT32 ChipID, UINT8 NumDataBits)
{
    UINT32 RetVal = MAX96707_9286_ERR_NONE;
    const MAX96707_9286_SERDES_INFO_s *CurSerDesInfo = &MaxSerDesInfo[VinID];
    UINT8 SerAddr = CurSerDesInfo->SerAddr0;

    RetVal |= Max96707_9286_Init(VinID);

    if (ChipID == MAX96707_LINK0_ID) {
        SerAddr = CurSerDesInfo->SerAddr0;
    } else if (ChipID == MAX96707_LINK1_ID) {
        SerAddr = CurSerDesInfo->SerAddr1;
    } else if (ChipID == MAX96707_LINK2_ID) {
        SerAddr = CurSerDesInfo->SerAddr2;
    } else if (ChipID == MAX96707_LINK3_ID) {
        SerAddr = CurSerDesInfo->SerAddr3;
    } else if (ChipID == MAX96707_ALL_ID) {
        SerAddr = CurSerDesInfo->SerAddrBroadCast;
    } else {
        AmbaPrint_ModulePrintUInt5(MAX96707_9286_MODULE_ID, "Not supported ChipID: 0x%x!", ChipID, 0U, 0U, 0U, 0U);
        RetVal = MAX96707_9286_ERR_ARG;
    }

    if (RetVal == MAX96707_9286_ERR_NONE) {
        if (NumDataBits == 8U) {
            RetVal |= Max96707_9286_RegWrite(VinID, SerAddr, 0x20, 0x07);
            RetVal |= Max96707_9286_RegWrite(VinID, SerAddr, 0x21, 0x06);
            RetVal |= Max96707_9286_RegWrite(VinID, SerAddr, 0x22, 0x05);
            RetVal |= Max96707_9286_RegWrite(VinID, SerAddr, 0x23, 0x04);
            RetVal |= Max96707_9286_RegWrite(VinID, SerAddr, 0x24, 0x03);
            RetVal |= Max96707_9286_RegWrite(VinID, SerAddr, 0x25, 0x02);
            RetVal |= Max96707_9286_RegWrite(VinID, SerAddr, 0x26, 0x01);
            RetVal |= Max96707_9286_RegWrite(VinID, SerAddr, 0x27, 0x00);
            RetVal |= Max96707_9286_RegWrite(VinID, SerAddr, 0x28, 0x40);
            RetVal |= Max96707_9286_RegWrite(VinID, SerAddr, 0x29, 0x40);
            RetVal |= Max96707_9286_RegWrite(VinID, SerAddr, 0x2a, 0x40);
            RetVal |= Max96707_9286_RegWrite(VinID, SerAddr, 0x2b, 0x40);
            RetVal |= Max96707_9286_RegWrite(VinID, SerAddr, 0x30, 0x17);
            RetVal |= Max96707_9286_RegWrite(VinID, SerAddr, 0x31, 0x16);
            RetVal |= Max96707_9286_RegWrite(VinID, SerAddr, 0x32, 0x15);
            RetVal |= Max96707_9286_RegWrite(VinID, SerAddr, 0x33, 0x14);
            RetVal |= Max96707_9286_RegWrite(VinID, SerAddr, 0x34, 0x13);
            RetVal |= Max96707_9286_RegWrite(VinID, SerAddr, 0x35, 0x12);
            RetVal |= Max96707_9286_RegWrite(VinID, SerAddr, 0x36, 0x11);
            RetVal |= Max96707_9286_RegWrite(VinID, SerAddr, 0x37, 0x10);
            RetVal |= Max96707_9286_RegWrite(VinID, SerAddr, 0x38, 0x40);
            RetVal |= Max96707_9286_RegWrite(VinID, SerAddr, 0x39, 0x40);
            RetVal |= Max96707_9286_RegWrite(VinID, SerAddr, 0x3a, 0x40);
            RetVal |= Max96707_9286_RegWrite(VinID, SerAddr, 0x3b, 0x40);
        } else if (NumDataBits == 10U) {
            RetVal |= Max96707_9286_RegWrite(VinID, SerAddr, 0x20, 0x09);
            RetVal |= Max96707_9286_RegWrite(VinID, SerAddr, 0x21, 0x08);
            RetVal |= Max96707_9286_RegWrite(VinID, SerAddr, 0x22, 0x07);
            RetVal |= Max96707_9286_RegWrite(VinID, SerAddr, 0x23, 0x06);
            RetVal |= Max96707_9286_RegWrite(VinID, SerAddr, 0x24, 0x05);
            RetVal |= Max96707_9286_RegWrite(VinID, SerAddr, 0x25, 0x04);
            RetVal |= Max96707_9286_RegWrite(VinID, SerAddr, 0x26, 0x03);
            RetVal |= Max96707_9286_RegWrite(VinID, SerAddr, 0x27, 0x02);
            RetVal |= Max96707_9286_RegWrite(VinID, SerAddr, 0x28, 0x01);
            RetVal |= Max96707_9286_RegWrite(VinID, SerAddr, 0x29, 0x00);
            RetVal |= Max96707_9286_RegWrite(VinID, SerAddr, 0x2a, 0x40);
            RetVal |= Max96707_9286_RegWrite(VinID, SerAddr, 0x2b, 0x40);
            RetVal |= Max96707_9286_RegWrite(VinID, SerAddr, 0x30, 0x19);
            RetVal |= Max96707_9286_RegWrite(VinID, SerAddr, 0x31, 0x18);
            RetVal |= Max96707_9286_RegWrite(VinID, SerAddr, 0x32, 0x17);
            RetVal |= Max96707_9286_RegWrite(VinID, SerAddr, 0x33, 0x16);
            RetVal |= Max96707_9286_RegWrite(VinID, SerAddr, 0x34, 0x15);
            RetVal |= Max96707_9286_RegWrite(VinID, SerAddr, 0x35, 0x14);
            RetVal |= Max96707_9286_RegWrite(VinID, SerAddr, 0x36, 0x13);
            RetVal |= Max96707_9286_RegWrite(VinID, SerAddr, 0x37, 0x12);
            RetVal |= Max96707_9286_RegWrite(VinID, SerAddr, 0x38, 0x11);
            RetVal |= Max96707_9286_RegWrite(VinID, SerAddr, 0x39, 0x10);
            RetVal |= Max96707_9286_RegWrite(VinID, SerAddr, 0x3a, 0x40);
            RetVal |= Max96707_9286_RegWrite(VinID, SerAddr, 0x3b, 0x40);
        } else if (NumDataBits == 12U) {
            RetVal |= Max96707_9286_RegWrite(VinID, SerAddr, 0x20, 0x0b);
            RetVal |= Max96707_9286_RegWrite(VinID, SerAddr, 0x21, 0x0a);
            RetVal |= Max96707_9286_RegWrite(VinID, SerAddr, 0x22, 0x09);
            RetVal |= Max96707_9286_RegWrite(VinID, SerAddr, 0x23, 0x08);
            RetVal |= Max96707_9286_RegWrite(VinID, SerAddr, 0x24, 0x07);
            RetVal |= Max96707_9286_RegWrite(VinID, SerAddr, 0x25, 0x06);
            RetVal |= Max96707_9286_RegWrite(VinID, SerAddr, 0x26, 0x05);
            RetVal |= Max96707_9286_RegWrite(VinID, SerAddr, 0x27, 0x04);
            RetVal |= Max96707_9286_RegWrite(VinID, SerAddr, 0x28, 0x03);
            RetVal |= Max96707_9286_RegWrite(VinID, SerAddr, 0x29, 0x02);
            RetVal |= Max96707_9286_RegWrite(VinID, SerAddr, 0x2a, 0x01);
            RetVal |= Max96707_9286_RegWrite(VinID, SerAddr, 0x2b, 0x00);
            RetVal |= Max96707_9286_RegWrite(VinID, SerAddr, 0x30, 0x1b);
            RetVal |= Max96707_9286_RegWrite(VinID, SerAddr, 0x31, 0x1a);
            RetVal |= Max96707_9286_RegWrite(VinID, SerAddr, 0x32, 0x19);
            RetVal |= Max96707_9286_RegWrite(VinID, SerAddr, 0x33, 0x18);
            RetVal |= Max96707_9286_RegWrite(VinID, SerAddr, 0x34, 0x17);
            RetVal |= Max96707_9286_RegWrite(VinID, SerAddr, 0x35, 0x16);
            RetVal |= Max96707_9286_RegWrite(VinID, SerAddr, 0x36, 0x15);
            RetVal |= Max96707_9286_RegWrite(VinID, SerAddr, 0x37, 0x14);
            RetVal |= Max96707_9286_RegWrite(VinID, SerAddr, 0x38, 0x13);
            RetVal |= Max96707_9286_RegWrite(VinID, SerAddr, 0x39, 0x12);
            RetVal |= Max96707_9286_RegWrite(VinID, SerAddr, 0x3a, 0x11);
            RetVal |= Max96707_9286_RegWrite(VinID, SerAddr, 0x3b, 0x10);
        } else {
            AmbaPrint_ModulePrintUInt5(MAX96707_9286_MODULE_ID, "Unsupported data width: %d bit!!", NumDataBits, 0U, 0U, 0U, 0U);
        }
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: Max96707_9286_RegisterWrite
 *
 *  @Description:: Write Max96707_9286 registers API
 *
 *  @Input      ::
 *      ChipID:    Chip id
 *      Addr:      Register address
 *      Data:      Register value
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *      int : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
UINT32 Max96707_9286_RegisterWrite(UINT32 VinID, UINT32 ChipID, UINT8 Addr, UINT8 Data)
{
    MAX96707_9286_SERDES_INFO_s *CurSerDesInfo = &MaxSerDesInfo[VinID];
    UINT32 RetVal = MAX96707_9286_ERR_NONE, RetValTmp = KAL_ERR_NONE;

    RetVal |= Max96707_9286_Init(VinID);
    if (RetVal == MAX96707_9286_ERR_NONE) {

        if (ChipID == MAX96707_LINK0_ID) {
            RetVal |= Max96707_9286_RegWrite(VinID, CurSerDesInfo->SerAddr0, Addr, Data);
        } else if (ChipID == MAX96707_LINK1_ID) {
            RetVal |= Max96707_9286_RegWrite(VinID, CurSerDesInfo->SerAddr1, Addr, Data);
        } else if (ChipID == MAX96707_LINK2_ID) {
            RetVal |= Max96707_9286_RegWrite(VinID, CurSerDesInfo->SerAddr2, Addr, Data);
        } else if (ChipID == MAX96707_LINK3_ID) {
            RetVal |= Max96707_9286_RegWrite(VinID, CurSerDesInfo->SerAddr3, Addr, Data);
        } else if (ChipID == MAX96707_ALL_ID) {
            /* Take the Mutex */
            if (AmbaKAL_MutexTake(&CurSerDesInfo->Mutex, AMBA_KAL_WAIT_FOREVER) != KAL_ERR_NONE) {
                RetVal = MAX96707_9286_ERR_MUTEX;  /* should never happen */
            } else {

                RetVal |= Max96707_9286_RegWrite(VinID, CurSerDesInfo->SerAddrBroadCast, Addr, Data);

                /* Release the Mutex */
                RetValTmp = AmbaKAL_MutexGive(&CurSerDesInfo->Mutex);
            }
        } else if (ChipID == MAX9286_ID) {
            RetVal |= Max96707_9286_RegWrite(VinID, CurSerDesInfo->DesAddr, Addr, Data);
        } else {
            RetVal = MAX96707_9286_ERR_UNKNOWN_CHIP_ID;
        }
    }

    return (RetValTmp == KAL_ERR_NONE) ? RetVal : MAX96707_9286_ERR_MUTEX;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: Max96707_9286_RegisterRead
 *
 *  @Description:: Read Max96707_9286 registers API
 *
 *  @Input      ::
 *      ChipID:    Chip id
 *      Addr:      Register address
 *      pData:     Pointer to register value
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *      int : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
UINT32 Max96707_9286_RegisterRead(UINT32 VinID, UINT32 ChipID, UINT8 Addr, UINT8 *pData)
{
    const MAX96707_9286_SERDES_INFO_s *CurSerDesInfo = &MaxSerDesInfo[VinID];
    UINT32 RetVal = MAX96707_9286_ERR_NONE;

    RetVal |= Max96707_9286_Init(VinID);
    if (RetVal == MAX96707_9286_ERR_NONE) {

        if (ChipID == MAX96707_LINK0_ID) {
            RetVal |= Max96707_9286_RegRead(VinID, CurSerDesInfo->SerAddr0, Addr, pData);
        } else if (ChipID == MAX96707_LINK1_ID) {
            RetVal |= Max96707_9286_RegRead(VinID, CurSerDesInfo->SerAddr1, Addr, pData);
        } else if (ChipID == MAX96707_LINK2_ID) {
            RetVal |= Max96707_9286_RegRead(VinID, CurSerDesInfo->SerAddr2, Addr, pData);
        } else if (ChipID == MAX96707_LINK3_ID) {
            RetVal |= Max96707_9286_RegRead(VinID, CurSerDesInfo->SerAddr3, Addr, pData);
        } else if (ChipID == MAX9286_ID) {
            RetVal |= Max96707_9286_RegRead(VinID, CurSerDesInfo->DesAddr, Addr, pData);
        } else {
            RetVal = MAX96707_9286_ERR_UNKNOWN_CHIP_ID;
        }
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: Max96707_9286_EnableCSIOutput
 *
 *  @Description:: Enable Max9286 CSI-2 output API
 *
 *  @Input      ::
 *      FramePeriod:  Frame period (in ms)
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *      int :      OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
UINT32 Max96707_9286_EnableCSIOutput(UINT32 VinID, UINT32 FramePeriod)
{
    MAX96707_9286_SERDES_INFO_s *CurSerDesInfo = &MaxSerDesInfo[VinID];
    UINT32 RetVal = MAX96707_9286_ERR_NONE, RetValTmp = KAL_ERR_NONE;

    RetVal |= Max96707_9286_Init(VinID);
    if (RetVal == MAX96707_9286_ERR_NONE) {

        /* Take the Mutex */
        if (AmbaKAL_MutexTake(&CurSerDesInfo->Mutex, AMBA_KAL_WAIT_FOREVER) != KAL_ERR_NONE) {
            RetVal = MAX96707_9286_ERR_MUTEX;  /* should never happen */
        } else {

            RetVal |= Max96707_9286_RegWrite(VinID, CurSerDesInfo->DesAddr, 0x34, 0xb6);             /* Re enable auto ACK -Optional- */
            RetVal |= Max96707_9286_RegWrite(VinID, CurSerDesInfo->SerAddrBroadCast, 0x04, 0x83);    /* Enable all serializer and disable configuration link */
            (void) AmbaKAL_TaskSleep(FramePeriod * 2U);                                              /* wait minimum 2 frames of delay for lines to sync */
            RetVal |= Max96707_9286_RegWrite(VinID, CurSerDesInfo->DesAddr, 0x15, 0x8b);             /* Enable CSI output (bit[7]=1: W x (4 x H)), Line-Interleave */

            /* Release the Mutex */
            RetValTmp = AmbaKAL_MutexGive(&CurSerDesInfo->Mutex);
        }
    }

    return (RetValTmp == KAL_ERR_NONE) ? RetVal : MAX96707_9286_ERR_MUTEX;
}
