/**
 *  @file AmbaYuv_LT6911.c
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
 *  @details Control APIs of OmniVision LT6911 CMOS sensor with MIPI interface
 *
 */

#include "AmbaKAL.h"
#include "AmbaWrap.h"

#include "AmbaVIN.h"
#include "AmbaI2C.h"
#include "AmbaGPIO.h"
#include "AmbaYuv.h"
#include "AmbaYuv_LT6911.h"

#include "AmbaPrint.h"

#include "bsp.h"

static UINT32 LT6911I2cChannel[AMBA_NUM_VIN_CHANNEL] = {
    [AMBA_VIN_CHANNEL0]  = AMBA_YUV_I2C_CHANNEL,
    [AMBA_VIN_CHANNEL1]  = AMBA_YUV_I2C_CHANNEL_VIN1,
    [AMBA_VIN_CHANNEL2]  = AMBA_YUV_I2C_CHANNEL_VIN2,
    [AMBA_VIN_CHANNEL3]  = AMBA_YUV_I2C_CHANNEL_VIN3,
    [AMBA_VIN_CHANNEL4]  = AMBA_YUV_I2C_CHANNEL_VIN4,
    [AMBA_VIN_CHANNEL5]  = AMBA_YUV_I2C_CHANNEL_VIN5,
    [AMBA_VIN_CHANNEL6]  = AMBA_YUV_I2C_CHANNEL_VIN6,
    [AMBA_VIN_CHANNEL7]  = AMBA_YUV_I2C_CHANNEL_VIN7,
    [AMBA_VIN_CHANNEL8]  = AMBA_YUV_I2C_CHANNEL_VIN8,
    [AMBA_VIN_CHANNEL9]  = AMBA_YUV_I2C_CHANNEL_VIN9,
    [AMBA_VIN_CHANNEL10] = AMBA_YUV_I2C_CHANNEL_VIN10,
    [AMBA_VIN_CHANNEL11] = AMBA_YUV_I2C_CHANNEL_VIN11,
    [AMBA_VIN_CHANNEL12] = AMBA_YUV_I2C_CHANNEL_VIN12,
    [AMBA_VIN_CHANNEL13] = AMBA_YUV_I2C_CHANNEL_VIN13,
};

/*-----------------------------------------------------------------------------------------------*\
 * LT6911 runtime status
\*-----------------------------------------------------------------------------------------------*/
static AMBA_YUV_STATUS_INFO_s LT6911Status[AMBA_NUM_VIN_CHANNEL] = {0};

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: LT6911_ConfigVin
 *
 *  @Description:: Configure VIN to receieve output data of the new mode
 *
 *  @Input      ::
 *      pModeInfo:  Details of the specified Yuv mode
 *
 *  @Output     :: none
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
static UINT32 LT6911_ConfigVin(UINT32 VinID, UINT32 ModeID)
{
    UINT32 RetVal;
    AMBA_VIN_MIPI_CONFIG_s LT6911VinConfig = {
        .Config = {
            .FrameRate = {
                .Interlace      = 0,
                .TimeScale      = 0,
                .NumUnitsInTick = 0,
            },
            .ColorSpace     = AMBA_VIN_COLOR_SPACE_YUV,
            .YuvOrder       = AMBA_YUV_YUV_ORDER_CB_Y0_CR_Y1,
            .NumDataBits    = 8U,
            .NumSkipFrame       = 1U,
            .RxHvSyncCtrl           = {
                .NumActivePixels    = 0,
                .NumActiveLines     = 0,
                .NumTotalPixels     = 0,
                .NumTotalLines      = 0,
            },
            .SplitCtrl = {0},
            .DelayedVsync = 0,
        },
        .NumActiveLanes = 4U,
        .DataType = 0x1eU,
        .DataTypeMask = 0x3fU,
        .VirtChanHDREnable = 0U,
        .VirtChanHDRConfig = {0}
    };
    const AMBA_YUV_OUTPUT_INFO_s *pOutputInfo = &LT6911ModeInfo[ModeID].OutputInfo;

    LT6911VinConfig.Config.RxHvSyncCtrl.NumActivePixels  = pOutputInfo->OutputWidth;
    LT6911VinConfig.Config.RxHvSyncCtrl.NumActiveLines   = pOutputInfo->OutputHeight;

    LT6911VinConfig.Config.YuvOrder = pOutputInfo->YuvOrder;

    LT6911VinConfig.NumActiveLanes = LT6911ModeInfo[ModeID].NumDataLanes;

    if (AmbaWrap_memcpy(&LT6911VinConfig.Config.FrameRate, &pOutputInfo->FrameRate, sizeof(AMBA_VIN_FRAME_RATE_s)) == ERR_NONE) {
        RetVal = AmbaVIN_MipiConfig(VinID, &LT6911VinConfig);
    } else {
        RetVal = YUV_ERR_UNEXPECTED;
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: LT6911_HardwareReset
 *
 *  @Description:: Reset LT6911 Yuv Device
 *
 *  @Input      :: none
 *
 *  @Output     :: none
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
static UINT32 LT6911_HardwareReset(UINT32 VinID)
{
    UINT32 RetVal;

    RetVal = AmbaUserGPIO_YuvResetCtrl(VinID, AMBA_GPIO_LEVEL_LOW);

    if (RetVal == GPIO_ERR_NONE) {
        RetVal = AmbaUserGPIO_YuvResetCtrl(VinID, AMBA_GPIO_LEVEL_HIGH);
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: LT6911_Enable
 *
 *  @Description:: Power on Yuv Device
 *
 *  @Input      ::
 *      Chan:   Vin ID
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          int : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static UINT32 LT6911_Enable(const AMBA_YUV_CHANNEL_s *pChan)
{
    UINT32 RetVal;

    if (pChan->VinID > AMBA_NUM_VIN_CHANNEL) {
        RetVal = YUV_ERR_ARG;
    } else {
        RetVal = LT6911_HardwareReset(pChan->VinID);
        //LT6911 i2c works fine after 1s delay
#ifdef CONFIG_LT6911_ERASED_MCU_FW
        (void)AmbaKAL_TaskSleep(2);
#else
        (void)AmbaKAL_TaskSleep(1000);
#endif
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: LT6911_GetStatus
 *
 *  @Description:: Get current Yuv device status
 *
 *  @Input      ::
 *      Chan:   Vin ID
 *
 *  @Output     ::
 *      pStatus:    pointer to current status
 *
 *  @Return     ::
 *          int : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static UINT32 LT6911_GetStatus(const AMBA_YUV_CHANNEL_s *pChan, AMBA_YUV_STATUS_INFO_s *pStatus)
{
    UINT32 RetVal = YUV_ERR_NONE;
    if ((pChan->VinID > AMBA_NUM_VIN_CHANNEL) || (pStatus == NULL)) {
        RetVal = YUV_ERR_ARG;
    } else {
        if (AmbaWrap_memcpy(pStatus, &LT6911Status[pChan->VinID], sizeof(AMBA_YUV_STATUS_INFO_s)) != ERR_NONE) {
            RetVal = YUV_ERR_UNEXPECTED;
        }
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: LT6911_GetModeInfo
 *
 *  @Description:: Get current Yuv device status
 *
 *  @Input      ::
 *      Chan:   Vin ID
 *
 *  @Output     ::
 *      pStatus:    pointer to current status
 *
 *  @Return     ::
 *          int : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static UINT32 LT6911_GetModeInfo(const AMBA_YUV_CHANNEL_s *pChan, const AMBA_YUV_CONFIG_s *pMode, AMBA_YUV_MODE_INFO_s *pModeInfo)
{
    UINT32 RetVal = YUV_ERR_NONE;

    if ((pChan->VinID > AMBA_NUM_VIN_CHANNEL) || (pModeInfo == NULL)) {
        RetVal = YUV_ERR_ARG;
    } else {
        pModeInfo->Config.ModeID = pMode->ModeID;

        if (AmbaWrap_memcpy(&pModeInfo->OutputInfo, &LT6911ModeInfo[pMode->ModeID].OutputInfo, sizeof(AMBA_YUV_OUTPUT_INFO_s)) != ERR_NONE) {
            RetVal = YUV_ERR_UNEXPECTED;
        }
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: LT6911_Init
 *
 *  @Description:: Initialize Yuv device driver
 *
 *  @Input      ::
 *      Chan:   Vin ID
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          int : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static UINT32 LT6911_Init(const AMBA_YUV_CHANNEL_s *pChan)
{
    UINT32 RetVal = YUV_ERR_NONE;

    if (pChan->VinID > AMBA_NUM_VIN_CHANNEL) {
        RetVal = YUV_ERR_ARG;
    }

    return RetVal;
}

static UINT32 LT6911_RegRead(UINT32 VinID, UINT16 Addr, UINT8 *pRxData)
{
    UINT32 RetVal = YUV_ERR_NONE;
    AMBA_I2C_TRANSACTION_s I2cTxConfig = {0};
    AMBA_I2C_TRANSACTION_s I2cRxConfig = {0};
    UINT8 TxData[2];
    UINT32 TxSize;

    I2cTxConfig.SlaveAddr = LT6911_I2C_SLAVE_ADDR;
    I2cTxConfig.DataSize  = 2U;
    I2cTxConfig.pDataBuf  = TxData;

    TxData[0] = 0xffU;
    TxData[1] = (UINT8) ((Addr & 0xff00U) >> 8U);

    RetVal |= AmbaI2C_MasterWrite(LT6911I2cChannel[VinID], AMBA_I2C_SPEED_STANDARD,
                                  &I2cTxConfig, &TxSize, 1000U);


    I2cTxConfig.SlaveAddr = LT6911_I2C_SLAVE_ADDR;
    I2cTxConfig.DataSize  = 1U;
    I2cTxConfig.pDataBuf  = TxData;
    TxData[0] = (UINT8)(Addr & 0xffU);

    I2cRxConfig.SlaveAddr = LT6911_I2C_SLAVE_ADDR | 1U;
    I2cRxConfig.DataSize  = 1U;
    I2cRxConfig.pDataBuf  = pRxData;

    RetVal |= AmbaI2C_MasterReadAfterWrite(LT6911I2cChannel[VinID], AMBA_I2C_SPEED_STANDARD, 1U, &I2cTxConfig,
                                           &I2cRxConfig, &TxSize, 1000U);

    if (RetVal != I2C_ERR_NONE) {
        AmbaPrint_ModulePrintUInt5(YUV_MODULE_ID, "[LT6911] I2C does not work!!!!!", 0U, 0U, 0U, 0U, 0U);
    }

//    AmbaPrint_ModulePrintUInt5(YUV_MODULE_ID, "read 0x%x", Addr, 0U, 0U, 0U, 0U);

    return RetVal;
}

static UINT32 LT6911_RegWrite(UINT32 VinID, UINT16 Addr, UINT8 Data)
{
    UINT32 RetVal;
    AMBA_I2C_TRANSACTION_s I2cConfig = {0};
    UINT8 TxDataBuf[2];
    UINT32 TxSize;

    I2cConfig.SlaveAddr = LT6911_I2C_SLAVE_ADDR;
    I2cConfig.DataSize  = 2U;
    I2cConfig.pDataBuf  = TxDataBuf;

    TxDataBuf[0] = 0xffU;
    TxDataBuf[1] = (UINT8) ((Addr & 0xff00U) >> 8U);

    RetVal = AmbaI2C_MasterWrite(LT6911I2cChannel[VinID], AMBA_I2C_SPEED_STANDARD,
                                 &I2cConfig, &TxSize, 1000U);

    if (RetVal != I2C_ERR_NONE) {
        AmbaPrint_ModulePrintUInt5(YUV_MODULE_ID, "[LT6911] I2C does not work!!!!!", 0U, 0U, 0U, 0U, 0U);
    }

    TxDataBuf[0] = (UINT8) (Addr & 0x00ffU);
    TxDataBuf[1] = Data;

    RetVal = AmbaI2C_MasterWrite(LT6911I2cChannel[VinID], AMBA_I2C_SPEED_STANDARD,
                                 &I2cConfig, &TxSize, 1000U);

//    AmbaPrint_ModulePrintUInt5(YUV_MODULE_ID, "write 0x%x: 0x%x", Addr, Data, 0U, 0U, 0U);
    if (RetVal != I2C_ERR_NONE) {
        AmbaPrint_ModulePrintUInt5(YUV_MODULE_ID, "[LT6911] I2C does not work!!!!!", 0U, 0U, 0U, 0U, 0U);
    }

    return RetVal;
}

static UINT32 Lt6911_IdCheck(const AMBA_YUV_CHANNEL_s *pChan)
{
    UINT32 RetVal = YUV_ERR_NONE;
    UINT8 Val_a000 = 0U, Val_a001 = 0U;

    RetVal |= LT6911_RegRead(pChan->VinID, 0xa000, &Val_a000);
    RetVal |= LT6911_RegRead(pChan->VinID, 0xa001, &Val_a001);

    if (RetVal == YUV_ERR_NONE) {
        if ((Val_a000 == 0x16U) && (Val_a001 == 0x05U)) {
            RetVal = YUV_ERR_NONE;
        } else {
            AmbaPrint_ModulePrintUInt5(YUV_MODULE_ID, "ID check 0xa000=0x%x, 0xa001=0x%x", Val_a000, Val_a001, 0U, 0U, 0U);
            RetVal = YUV_ERR_COMMUNICATE;
        }
    } else {
        RetVal = YUV_ERR_COMMUNICATE;
    }

    return RetVal;
}

static void Lt6911_AudioI2sSet(const AMBA_YUV_CHANNEL_s *pChan)
{
    UINT32 i;

    for (i = 0U; i < LT6911_NUM_AUDIO_I2S_REG; i ++) {
        (void) LT6911_RegWrite(pChan->VinID, AudioI2sRegTable[i].Addr, AudioI2sRegTable[i].Data);
    }
}

static void Lt6911_HdmiEdidSet(const AMBA_YUV_CHANNEL_s *pChan, UINT32 ModeID)
{
    UINT32 i;
    UINT8 CrcSum = 0U;

    for (i = 0U; i < 127U; i ++) {
        CrcSum += EdidRegTable[ModeID][i];
    }
    EdidRegTable[ModeID][0x7f] = (0xffU - CrcSum) + 1U;

    CrcSum = 0U;
    for (i = 128U; i < 255U; i ++) {
        CrcSum += EdidRegTable[ModeID][i];
    }
    EdidRegTable[ModeID][0xff] = (0xffU - CrcSum) + 1U;

    (void) LT6911_RegWrite(pChan->VinID, 0xc08e, 0x03);
    for (i = 0U; i < LT6911_NUM_EDID_REG; i ++) {
        (void) LT6911_RegWrite(pChan->VinID, 0xc090, EdidRegTable[ModeID][i]);
    }
    (void) LT6911_RegWrite(pChan->VinID, 0xc08e, 0x02);
}

static void Lt6911_RxPhySet(const AMBA_YUV_CHANNEL_s *pChan)
{
    UINT32 i;

    for (i = 0U; i < LT6911_NUM_RX_PHY_REG; i ++) {
        (void) LT6911_RegWrite(pChan->VinID, RxPhyRegTable[i].Addr, RxPhyRegTable[i].Data);
    }
}

static void Lt6911_ClockStableCheck(const AMBA_YUV_CHANNEL_s *pChan)
{
    UINT8 RData = 0U;

    (void) LT6911_RegWrite(pChan->VinID, 0xa034U, 0x1U);

    while ((RData & 0x1U) == 0U) {
        (void) AmbaKAL_TaskSleep(10);
        (void) LT6911_RegRead(pChan->VinID, 0xb8b0, &RData);
//        AmbaPrint_ModulePrintUInt5(YUV_MODULE_ID, "Clock stable Reg: 0x%x", RData, 0U, 0U, 0U, 0U);
    }
}

static void Lt6911_RxPllLockIndicate(const AMBA_YUV_CHANNEL_s *pChan)
{
    UINT8 RData = 0x40U;

    (void) LT6911_RegWrite(pChan->VinID, 0x9003, 0xFE);
    (void) AmbaKAL_TaskSleep(10);
    (void) LT6911_RegWrite(pChan->VinID, 0x9003, 0xFF);
    (void) AmbaKAL_TaskSleep(100);

    while ((RData & 0x40U) != 0U) {
        (void) AmbaKAL_TaskSleep(10);
        (void) LT6911_RegRead(pChan->VinID, 0xb8b0, &RData);
//        AmbaPrint_ModulePrintUInt5(YUV_MODULE_ID, "RxPllLock Reg: 0x%x", RData, 0U, 0U, 0U, 0U);
    }
}

static void Lt6911_RxPllEnable(const AMBA_YUV_CHANNEL_s *pChan)
{
    UINT32 i;

    for (i = 0U; i < LT6911_NUM_RX_PLL_ENABLE_REG; i ++) {
        (void) LT6911_RegWrite(pChan->VinID, RxPllEnableRegTable[i].Addr, RxPllEnableRegTable[i].Data);
    }
}

static UINT32 LT6911_GetDetClk(const AMBA_YUV_CHANNEL_s *pChan)
{
    UINT8 Val_0xB8B1, Val_0xB8B2, Val_0xB8B3;

    (void) LT6911_RegWrite(pChan->VinID, 0xA034, 0x01);
    (void) AmbaKAL_TaskSleep(10);

    (void) LT6911_RegRead(pChan->VinID, 0xB8B3, &Val_0xB8B3);
    (void) LT6911_RegRead(pChan->VinID, 0xB8B2, &Val_0xB8B2);
    (void) LT6911_RegRead(pChan->VinID, 0xB8B1, &Val_0xB8B1);

    return (((UINT32)Val_0xB8B1 << 16U) + ((UINT32)Val_0xB8B2 << 8U) + (UINT32)Val_0xB8B3);
}

static void LT6911_RxPllFreqSet(const AMBA_YUV_CHANNEL_s *pChan)
{
    UINT32 DetClk;

    DetClk = LT6911_GetDetClk(pChan);

    AmbaPrint_ModulePrintUInt5(YUV_MODULE_ID, "[LT6911_RxPllFreqSet] DetClk: %d", DetClk, 0U, 0U, 0U, 0U);

    /* DetClk means PixelClock */
#if 0
    if (DetClk >= 197000U) {
        (void) LT6911_RegWrite(pChan->VinID, 0xB02A, 0x47);
    } else if (DetClk < 96000U) {
        (void) LT6911_RegWrite(pChan->VinID, 0xB02A, 0x77);
    } else {
        (void) LT6911_RegWrite(pChan->VinID, 0xB02A, 0x67);
    }
#else
    if (LT6911Status[pChan->VinID].ModeInfo.Config.ModeID == LT6911_3840_2160_30P) {
        (void) LT6911_RegWrite(pChan->VinID, 0xB02A, 0x47);
    } else if (LT6911Status[pChan->VinID].ModeInfo.Config.ModeID == LT6911_1280_720_60P) {
        (void) LT6911_RegWrite(pChan->VinID, 0xB02A, 0x77);
    } else if (LT6911Status[pChan->VinID].ModeInfo.Config.ModeID == LT6911_1280_720_30P) {
        (void) LT6911_RegWrite(pChan->VinID, 0xB02A, 0x77);
    } else if (LT6911Status[pChan->VinID].ModeInfo.Config.ModeID == LT6911_1920_1080_60P) {
        (void) LT6911_RegWrite(pChan->VinID, 0xB02A, 0x67);
    } else if (LT6911Status[pChan->VinID].ModeInfo.Config.ModeID == LT6911_1920_1080_30P) {
        (void) LT6911_RegWrite(pChan->VinID, 0xB02A, 0x77);
    } else {
        (void) LT6911_RegWrite(pChan->VinID, 0xB02A, 0x47);
    }
#endif

    (void) LT6911_RegWrite(pChan->VinID, 0xB02B, 0x42);
}

static void LT6911_RxPllCalibration(const AMBA_YUV_CHANNEL_s *pChan)
{
    UINT8 RData = 0U;

    while ((RData & 0x40U) == 0U) {
        (void) LT6911_RegWrite(pChan->VinID, 0x900F, 0x7F);
        (void) AmbaKAL_TaskSleep(10);
        (void) LT6911_RegWrite(pChan->VinID, 0x900F, 0xFF);

        (void) LT6911_RegWrite(pChan->VinID, 0xB814, 0x14);
        (void) AmbaKAL_TaskSleep(10);
        (void) LT6911_RegWrite(pChan->VinID, 0xB814, 0x94);
        (void) AmbaKAL_TaskSleep(10);
        (void) LT6911_RegWrite(pChan->VinID, 0xB814, 0xB4);

        /* get rx pll lock status */
        (void) LT6911_RegWrite(pChan->VinID, 0x9003, 0xFE);
        (void) AmbaKAL_TaskSleep(10);
        (void) LT6911_RegWrite(pChan->VinID, 0x9003, 0xFF);
        (void) AmbaKAL_TaskSleep(100);

        (void) LT6911_RegRead(pChan->VinID, 0xb8b0, &RData);
//        AmbaPrint_ModulePrintUInt5(YUV_MODULE_ID, "[Calib]RxPllLock Reg: 0x%x", RData, 0U, 0U, 0U, 0U);

    }
}

static void Lt6911_RxCdrEnable(const AMBA_YUV_CHANNEL_s *pChan)
{
    (void) LT6911_RegWrite(pChan->VinID, 0x9003,0xEF);
    (void) AmbaKAL_TaskSleep(10);
    (void) LT6911_RegWrite(pChan->VinID, 0x9003,0xFF);
}

static void Lt6911_RxPiEnable(const AMBA_YUV_CHANNEL_s *pChan)
{
    (void) LT6911_RegWrite(pChan->VinID, 0xB01F, 0x00);
    (void) LT6911_RegWrite(pChan->VinID, 0xB020, 0x00);
    (void) LT6911_RegWrite(pChan->VinID, 0x900D, 0xFE);
    (void) AmbaKAL_TaskSleep(10);
    (void) LT6911_RegWrite(pChan->VinID, 0x900D, 0xFF);
}

static void Lt6911_PixelPllEnable(const AMBA_YUV_CHANNEL_s *pChan)
{
    (void) LT6911_RegWrite(pChan->VinID, 0xB014, 0x38);
    (void) LT6911_RegWrite(pChan->VinID, 0x900D, 0xDF);
    (void) AmbaKAL_TaskSleep(10);
    (void) LT6911_RegWrite(pChan->VinID, 0x900D, 0xFF);
}

static void Lt6911_HdmiModuleReset(const AMBA_YUV_CHANNEL_s *pChan)
{
    (void) LT6911_RegWrite(pChan->VinID, 0x9008, 0xDF);
    (void) AmbaKAL_TaskSleep(10);
    (void) LT6911_RegWrite(pChan->VinID, 0x9008, 0xFF);
}

static void Lt6911_EqSet(const AMBA_YUV_CHANNEL_s *pChan)
{
    (void) LT6911_RegWrite(pChan->VinID, 0x900B, 0xFE);
    (void) LT6911_RegWrite(pChan->VinID, 0x900A, 0x3F);
    (void) AmbaKAL_TaskSleep(5);
    (void) LT6911_RegWrite(pChan->VinID, 0x900A, 0xBF);
    (void) LT6911_RegWrite(pChan->VinID, 0xD800, 0x11);
}

static void Lt6911_RxPiPhaseSet(const AMBA_YUV_CHANNEL_s *pChan)
{
    UINT32 DetClk = LT6911_GetDetClk(pChan);
    UINT16 MinErr = 0xffffU;
    UINT8 BestPhase = 0U, i;
    UINT8 ErrCntH, ErrCntL;

    AmbaPrint_ModulePrintUInt5(YUV_MODULE_ID, "[Lt6911_RxPiPhaseSet] DetClk: %d", DetClk, 0U, 0U, 0U, 0U);
    if (DetClk < 100000U) {
        (void) LT6911_RegWrite(pChan->VinID, 0xB827, 0x0F); /* PI Softmode Enable */
        for (i = 0U; i < 4U; i ++) {
            (void) LT6911_RegWrite(pChan->VinID, 0xB829, (UINT8)(i * 0x10U));
            (void) LT6911_RegWrite(pChan->VinID, 0xC020, 0x07);
            (void) LT6911_RegWrite(pChan->VinID, 0xC020, 0x01);
            (void) AmbaKAL_TaskSleep(100);

            (void) LT6911_RegRead(pChan->VinID, 0xC025, &ErrCntL);
            (void) LT6911_RegRead(pChan->VinID, 0xC024, &ErrCntH);

            if (((((UINT16)ErrCntH & 0x7fU) << 8U) + (UINT16)ErrCntL) <= MinErr) {
                MinErr = ((((UINT16)ErrCntH & 0x7fU) << 8U) + (UINT16)ErrCntL);
                BestPhase = i;
            }
        }
        (void) LT6911_RegWrite(pChan->VinID, 0xB829, (UINT8)(BestPhase*0x10U));
        (void) LT6911_RegWrite(pChan->VinID, 0xB82A, (UINT8)(BestPhase*0x10U));
        (void) LT6911_RegWrite(pChan->VinID, 0xB82B, (UINT8)(BestPhase*0x10U));
    } else {
        (void) LT6911_RegWrite(pChan->VinID, 0xB827, 0x00U);
    }
}

static void Lt6911_ColorSpaceConversion(const AMBA_YUV_CHANNEL_s *pChan)
{
    UINT8 RData;

    (void) LT6911_RegRead(pChan->VinID, 0xC071, &RData);
    RData = RData & 0x60U;

    if (RData == 0x00U) { /* RGB to YUV422 */
        (void) LT6911_RegWrite(pChan->VinID, 0xA050, 0x00);
        (void) LT6911_RegWrite(pChan->VinID, 0xA055, 0x88);

    } else if (RData == 0x20U) { /* YUV 422 to YUV422 */
        (void) LT6911_RegWrite(pChan->VinID, 0xA050, 0x04);
        (void) LT6911_RegWrite(pChan->VinID, 0xA055, 0x00);

    } else if (RData == 0x40U) { /* YUV444 to YUV422 */
        (void) LT6911_RegWrite(pChan->VinID, 0xA050, 0x00);
        (void) LT6911_RegWrite(pChan->VinID, 0xA055, 0x08);

    } else {
        (void) LT6911_RegWrite(pChan->VinID, 0xA050, 0x00);
        (void) LT6911_RegWrite(pChan->VinID, 0xA055, 0x88);

    }

}

static void Lt6911_SyncPolaritySet(const AMBA_YUV_CHANNEL_s *pChan)
{
    UINT8 RData;
    UINT8 HSyncPol;
    UINT8 VSyncPol;
    UINT8 HSyncReverse = 0U;
    UINT8 VSyncReverse = 0U;

    (void) LT6911_RegRead(pChan->VinID, 0xD2A0, &RData);
    HSyncPol = RData & 0x2U;
    VSyncPol = RData & 0x1U;

    if (HSyncPol == 0U) {
        (void) LT6911_RegRead(pChan->VinID, 0xC005, &RData);
        if ((RData & (UINT8)0x20) != 0U) {
            HSyncReverse = (RData & 0xdfU);
        } else {
            HSyncReverse = (RData | 0x20U);
        }

        (void) LT6911_RegWrite(pChan->VinID, 0xC005, HSyncReverse);
    }

    if (VSyncPol == 0U) {
        (void) LT6911_RegRead(pChan->VinID, 0xC005, &RData);
        if ((RData & 0x10U) != 0U) {
            VSyncReverse = (RData & 0xefU);
        } else {
            VSyncReverse = (RData | 0x10U);
        }

        (void) LT6911_RegWrite(pChan->VinID, 0xC005, VSyncReverse);
    }
}

static void Lt6911_TxPhySet(const AMBA_YUV_CHANNEL_s *pChan)
{
    UINT32 i;

    for (i = 0U; i < LT6911_NUM_TX_PHY_REG; i ++) {
        (void) LT6911_RegWrite(pChan->VinID, TxPhyRegTable[i].Addr, TxPhyRegTable[i].Data);
    }
}

static void Lt6911_TxPortSet(const AMBA_YUV_CHANNEL_s *pChan)
{
    UINT32 i;

    for (i = 0U; i < LT6911_NUM_TX_PORT_REG; i ++) {
        (void) LT6911_RegWrite(pChan->VinID, TxPortRegTable[i].Addr, TxPortRegTable[i].Data);
    }
}

static UINT32 Lt6911_TimingCheck(const AMBA_YUV_CHANNEL_s *pChan, UINT32 ModeID)
{
    UINT32 RetVal = YUV_ERR_NONE;
    const LT6911_MODE_TIMING_s *pModeTiming = &LT6911ModeTiming[ModeID];
    LT6911_MODE_TIMING_s ModeTiming;
    UINT32 PixelClock;
    UINT8 RDataHsb = 0U;
    UINT8 RDataMsb = 0U;
    UINT8 RDataLsb = 0U;
    UINT8 RData = 0U;

    /* Pixel clock */
    (void) LT6911_RegWrite(pChan->VinID, 0xA034, 0x0B);

    (void) LT6911_RegRead(pChan->VinID, 0xB8B1, &RDataHsb);
    (void) LT6911_RegRead(pChan->VinID, 0xB8B2, &RDataMsb);
    (void) LT6911_RegRead(pChan->VinID, 0xB8B3, &RDataLsb);

    PixelClock = (((UINT32)RDataHsb << 16U) | ((UINT32)RDataMsb << 8U) | (UINT32)RDataLsb) * 1000U;

    /* H timing */
    (void) LT6911_RegRead(pChan->VinID, 0xD289, &RDataMsb);
    (void) LT6911_RegRead(pChan->VinID, 0xD28A, &RDataLsb);
    ModeTiming.Htotal = (UINT16)((((UINT32)RDataMsb << 8U) | (UINT32)RDataLsb) << 1U);

    (void) LT6911_RegRead(pChan->VinID, 0xD28B, &RDataMsb);
    (void) LT6911_RegRead(pChan->VinID, 0xD28C, &RDataLsb);
    ModeTiming.Hact = (UINT16)((((UINT32)RDataMsb << 8U) | (UINT32)RDataLsb) << 1U);

    (void) LT6911_RegRead(pChan->VinID, 0xD294, &RDataMsb);
    (void) LT6911_RegRead(pChan->VinID, 0xD295, &RDataLsb);
    ModeTiming.Hs = (UINT16)((((UINT32)RDataMsb << 8U) | (UINT32)RDataLsb) << 1U);

    (void) LT6911_RegRead(pChan->VinID, 0xD298, &RDataMsb);
    (void) LT6911_RegRead(pChan->VinID, 0xD299, &RDataLsb);
    ModeTiming.Hbp = (UINT16)((((UINT32)RDataMsb << 8U) | (UINT32)RDataLsb) << 1U);

    (void) LT6911_RegRead(pChan->VinID, 0xD29C, &RDataMsb);
    (void) LT6911_RegRead(pChan->VinID, 0xD29D, &RDataLsb);
    ModeTiming.Hfp = (UINT16)((((UINT32)RDataMsb << 8U) | (UINT32)RDataLsb) << 1U);

    /* V timing */
    (void) LT6911_RegRead(pChan->VinID, 0xD29E, &RDataMsb);
    (void) LT6911_RegRead(pChan->VinID, 0xD29F, &RDataLsb);
    ModeTiming.Vtotal = ((UINT16)RDataMsb << 8U) | (UINT16)RDataLsb;

    (void) LT6911_RegRead(pChan->VinID, 0xD296, &RDataMsb);
    (void) LT6911_RegRead(pChan->VinID, 0xD297, &RDataLsb);
    ModeTiming.Vact = ((UINT16)RDataMsb << 8U) | (UINT16)RDataLsb;

    (void) LT6911_RegRead(pChan->VinID, 0xD286, &RData);
    ModeTiming.Vs = (UINT16)RData;

    (void) LT6911_RegRead(pChan->VinID, 0xD287, &RData);
    ModeTiming.Vbp = (UINT16)RData;

    (void) LT6911_RegRead(pChan->VinID, 0xD288, &RData);
    ModeTiming.Vfp = (UINT16)RData;

    if ((PixelClock == 0U) || (ModeTiming.Htotal != pModeTiming->Htotal) || (ModeTiming.Vtotal != pModeTiming->Vtotal)) {
        RetVal = YUV_ERR_COMMUNICATE;
    }

    AmbaPrint_ModulePrintUInt5(YUV_MODULE_ID, "[Lt6911_TimingCheck] HDMI timing check:", 0U, 0U, 0U, 0U, 0U);
    AmbaPrint_ModulePrintUInt5(YUV_MODULE_ID, "[Lt6911_TimingCheck] PixelClock:%d:", PixelClock, 0U, 0U, 0U, 0U);
    AmbaPrint_ModulePrintUInt5(YUV_MODULE_ID, "[Lt6911_TimingCheck] H:%d(active %d, sync:%d, fp:%d, bp:%d)",
                               ModeTiming.Htotal, ModeTiming.Hact, ModeTiming.Hs, ModeTiming.Hfp, ModeTiming.Hbp);
    AmbaPrint_ModulePrintUInt5(YUV_MODULE_ID, "[Lt6911_TimingCheck] V:%d(active %d, sync:%d, fp:%d, bp:%d)",
                               ModeTiming.Vtotal, ModeTiming.Vact, ModeTiming.Vs, ModeTiming.Vfp, ModeTiming.Vbp);

    return RetVal;
}

static void Lt6911_HdmiToMipiEnable(const AMBA_YUV_CHANNEL_s *pChan, UINT32 NumMipiLane)
{
    (void) LT6911_RegWrite(pChan->VinID, 0xB044, 0xC0);
    (void) LT6911_RegWrite(pChan->VinID, 0xA044, 0x80);
    (void) LT6911_RegWrite(pChan->VinID, 0xA049, 0x80);

    if (NumMipiLane == 4U) {
        (void) LT6911_RegWrite(pChan->VinID, 0xC201, 0x4F);
        (void) LT6911_RegWrite(pChan->VinID, 0xC256, 0x4F);
        (void) LT6911_RegWrite(pChan->VinID, 0xC202, 0x01);
        (void) LT6911_RegWrite(pChan->VinID, 0xC203, 0x30);
    } else if (NumMipiLane == 3U) {
        (void) LT6911_RegWrite(pChan->VinID, 0xC201, 0x7F);
        (void) LT6911_RegWrite(pChan->VinID, 0xC256, 0x7F);
        (void) LT6911_RegWrite(pChan->VinID, 0xC202, 0x01);
        (void) LT6911_RegWrite(pChan->VinID, 0xC203, 0x90);
    } else if (NumMipiLane == 2U) {
        (void) LT6911_RegWrite(pChan->VinID, 0xC201, 0x6F);
        (void) LT6911_RegWrite(pChan->VinID, 0xC256, 0x6F);
        (void) LT6911_RegWrite(pChan->VinID, 0xC202, 0x02);
        (void) LT6911_RegWrite(pChan->VinID, 0xC203, 0x58);
    } else if (NumMipiLane == 1U) {
        (void) LT6911_RegWrite(pChan->VinID, 0xC201, 0x5F);
        (void) LT6911_RegWrite(pChan->VinID, 0xC256, 0x5F);
        (void) LT6911_RegWrite(pChan->VinID, 0xC202, 0x03);
        (void) LT6911_RegWrite(pChan->VinID, 0xC203, 0x00);
    } else {
        (void) LT6911_RegWrite(pChan->VinID, 0xC201, 0x4F);
        (void) LT6911_RegWrite(pChan->VinID, 0xC256, 0x4F);
        (void) LT6911_RegWrite(pChan->VinID, 0xC202, 0x01);
        (void) LT6911_RegWrite(pChan->VinID, 0xC203, 0x30);
    }
}

static void Lt6911_MipiClockSet(const AMBA_YUV_CHANNEL_s *pChan, UINT32 ModeID)
{
    UINT8 ByteClk = (UINT8)(LT6911ModeInfo[ModeID].MipiClock / 4U);
    UINT8 FreqDiv = 0U;

    (void) LT6911_RegWrite(pChan->VinID, 0xB03A, 0x38);
    (void) LT6911_RegWrite(pChan->VinID, 0xB03B, 0x01);

    if ((ByteClk < 187U) && (ByteClk >= 80U)) {
        (void) LT6911_RegWrite(pChan->VinID, 0xB03E, 0x18);
        FreqDiv = 1U;
    } else if ((ByteClk < 80U) && (ByteClk >= 40U)) {
        (void) LT6911_RegWrite(pChan->VinID, 0xB03E, 0x08);
        FreqDiv = 2U;
    } else if ((ByteClk < 40U) && (ByteClk >= 20U)) {
        (void) LT6911_RegWrite(pChan->VinID, 0xB03E, 0x28);
        FreqDiv = 4U;
    } else if ((ByteClk < 20U) && (ByteClk >= 10U)) {
        (void) LT6911_RegWrite(pChan->VinID, 0xB03E, 0x38);
        FreqDiv = 8U;
    } else {
        AmbaPrint_ModulePrintUInt5(YUV_MODULE_ID, "Mipi Clock is invalid", 0U, 0U, 0U, 0U, 0U);
    }

    (void) LT6911_RegWrite(pChan->VinID, 0xB03F, 0x10);
    (void) LT6911_RegWrite(pChan->VinID, 0xB040, 0x10);

    (void) LT6911_RegWrite(pChan->VinID, 0xB041, (UINT8)((ByteClk * FreqDiv * 8U) / 27U));

    (void) LT6911_RegWrite(pChan->VinID, 0x900F, 0xDF);
    (void) LT6911_RegWrite(pChan->VinID, 0xB044, 0x90);
    (void) AmbaKAL_TaskSleep(10U);
    (void) LT6911_RegWrite(pChan->VinID, 0x900F, 0xFF);
    (void) LT6911_RegWrite(pChan->VinID, 0xB044, 0xC0);

}

static void Lt6911_MipiDphySet(const AMBA_YUV_CHANNEL_s *pChan, UINT32 ModeID)
{
    UINT8 Lpx = (UINT8)((LT6911ModeInfo[ModeID].MipiClock / 80U) + 4U);
    UINT8 Hsprep = (UINT8)((LT6911ModeInfo[ModeID].MipiClock / 100U) + 4U);

    (void) LT6911_RegWrite(pChan->VinID, 0xC22F, Hsprep);
    (void) LT6911_RegWrite(pChan->VinID, 0xC230, Lpx);
    (void) LT6911_RegWrite(pChan->VinID, 0xC231, 0xAA);

    (void) LT6911_RegWrite(pChan->VinID, 0xC252, Hsprep);
    (void) LT6911_RegWrite(pChan->VinID, 0xC253, Lpx);
    (void) LT6911_RegWrite(pChan->VinID, 0xC254, 0xAA);

    (void) LT6911_RegWrite(pChan->VinID, 0xC221, 0x05);
    (void) LT6911_RegWrite(pChan->VinID, 0xC222, 0x05);
    (void) LT6911_RegWrite(pChan->VinID, 0xC223, 0x07);
    (void) LT6911_RegWrite(pChan->VinID, 0xC224, 0x1f);

    (void) LT6911_RegWrite(pChan->VinID, 0xC228, 0x0A);
    (void) LT6911_RegWrite(pChan->VinID, 0xC22A, 0x20);

    (void) LT6911_RegWrite(pChan->VinID, 0xC21B, 0x10);
    (void) LT6911_RegWrite(pChan->VinID, 0xC21D, 0x50);

}

static void Lt6911_MipiProtocalSet(const AMBA_YUV_CHANNEL_s *pChan, UINT32 ModeID)
{
    const LT6911_MODE_TIMING_s *pModeTiming = &LT6911ModeTiming[ModeID];
    (void) LT6911_RegWrite(pChan->VinID, 0xC20C, (UINT8)(pModeTiming->Hact / 256U));
    (void) LT6911_RegWrite(pChan->VinID, 0xC20D, (UINT8)(pModeTiming->Hact % 256U));
    (void) LT6911_RegWrite(pChan->VinID, 0xC235, (UINT8)(pModeTiming->Vs + pModeTiming->Vbp));

    (void) LT6911_RegWrite(pChan->VinID, 0xC206, (UINT8)(pModeTiming->Vact / 256U));
    (void) LT6911_RegWrite(pChan->VinID, 0xC207, (UINT8)(pModeTiming->Vact % 256U));

    (void) LT6911_RegWrite(pChan->VinID, 0xC20F, 0x55U);
    (void) LT6911_RegWrite(pChan->VinID, 0xC210, (UINT8)((pModeTiming->Hact / 4U) / 256U));
    (void) LT6911_RegWrite(pChan->VinID, 0xC211, (UINT8)((pModeTiming->Hact / 4U) % 256U));

    /* LRMODE = 0*/
    (void) LT6911_RegWrite(pChan->VinID, 0xC217, 0x24);
    (void) LT6911_RegWrite(pChan->VinID, 0xC238, (UINT8)(pModeTiming->Hact / 256U));
    (void) LT6911_RegWrite(pChan->VinID, 0xC239, (UINT8)(pModeTiming->Hact % 256U));
    (void) LT6911_RegWrite(pChan->VinID, 0xC204, (UINT8)(pModeTiming->Vs + pModeTiming->Vbp - 1U));
    (void) LT6911_RegWrite(pChan->VinID, 0xC205, 0x01);
    (void) LT6911_RegWrite(pChan->VinID, 0xC208, 0x00);
    (void) LT6911_RegWrite(pChan->VinID, 0xC209, 0x01);
    (void) LT6911_RegWrite(pChan->VinID, 0xC20A, 0x01);
    (void) LT6911_RegWrite(pChan->VinID, 0xC20B, 0x01);
    (void) LT6911_RegWrite(pChan->VinID, 0xC212, 0x1E);
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: LT6911_Config
 *
 *  @Description:: Set Yuv device to indicated mode
 *
 *  @Input      ::
 *      Chan:   Vin ID
 *      Mode:   Yuv Mode
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          int : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static UINT32 LT6911_Config(const AMBA_YUV_CHANNEL_s *pChan, const AMBA_YUV_CONFIG_s *pMode)
{
    UINT32 RetVal = YUV_ERR_NONE;
    UINT32 VinID = pChan->VinID;
    UINT32 ModeID = pMode->ModeID; //(UINT32)LT6911_3840_2160_30P;
    static UINT8 Initialized[AMBA_NUM_VIN_CHANNEL] = {0};

    AMBA_VIN_MIPI_PAD_CONFIG_s LT6911PadConfig = {
        .ClkMode    = AMBA_VIN_MIPI_CLK_MODE_CONTINUOUS,
        .DateRate   = 0U,
        .EnabledPin = 0xfU
    };
    const UINT8 LaneShift[AMBA_NUM_VIN_CHANNEL] = {
        [AMBA_VIN_CHANNEL0]  = 0U,
        [AMBA_VIN_CHANNEL1]  = 0U,
        [AMBA_VIN_CHANNEL2]  = 0U,
        [AMBA_VIN_CHANNEL3]  = 0U,
        [AMBA_VIN_CHANNEL4]  = 4U,
        [AMBA_VIN_CHANNEL5]  = 4U,
        [AMBA_VIN_CHANNEL6]  = 4U,
        [AMBA_VIN_CHANNEL7]  = 4U,
        [AMBA_VIN_CHANNEL8]  = 0U,
        [AMBA_VIN_CHANNEL9]  = 0U,
        [AMBA_VIN_CHANNEL10] = 0U,
        [AMBA_VIN_CHANNEL11] = 4U,
        [AMBA_VIN_CHANNEL12] = 4U,
        [AMBA_VIN_CHANNEL13] = 4U,
    };

    if (Initialized[VinID] == 0U) {
        Initialized[VinID] = 1U;
        AmbaPrint_ModulePrintUInt5(YUV_MODULE_ID, "============  [ LT6911 ] Mode: %d ============", ModeID, 0U, 0U, 0U, 0U);

        /* Update status */
        RetVal = AmbaWrap_memcpy(&LT6911Status[pChan->VinID].ModeInfo.Config, pMode, sizeof(AMBA_YUV_CONFIG_s));
        RetVal |= AmbaWrap_memcpy(&LT6911Status[pChan->VinID].ModeInfo.OutputInfo, &LT6911ModeInfo[ModeID].OutputInfo, sizeof(AMBA_YUV_OUTPUT_INFO_s));

        if (RetVal == ERR_NONE) {
            /* Adjust mipi-phy parameters */
            LT6911PadConfig.DateRate = LT6911ModeInfo[ModeID].OutputInfo.DataRate;
            LT6911PadConfig.EnabledPin = (((UINT32)1U << LT6911ModeInfo[ModeID].NumDataLanes) - 1U) << LaneShift[pChan->VinID];
            RetVal = AmbaVIN_MipiReset(pChan->VinID, &LT6911PadConfig);

            /* turn on i2c control mode */
            (void) LT6911_RegWrite(pChan->VinID, 0x80ee, 0x1);

            /* SET_HPD_LOW */
            (void) LT6911_RegWrite(pChan->VinID, 0xC006, 0x00);

            if (Lt6911_IdCheck(pChan) != YUV_ERR_NONE) {
                AmbaPrint_ModulePrintUInt5(YUV_MODULE_ID, "LT6911 ID check fail!", 0U, 0U, 0U, 0U, 0U);
                RetVal = YUV_ERR_COMMUNICATE;

            } else {

                Lt6911_AudioI2sSet(pChan);
                Lt6911_HdmiEdidSet(pChan, ModeID);

                Lt6911_RxPhySet(pChan);
                (void) AmbaKAL_TaskSleep(300);

                /* SET_HPD_HIGH */
                (void) LT6911_RegWrite(pChan->VinID, 0xC006, 0x08);
                (void) AmbaKAL_TaskSleep(750);

                Lt6911_ClockStableCheck(pChan);

                Lt6911_RxPllLockIndicate(pChan);

                Lt6911_RxPllEnable(pChan);

                (void) AmbaKAL_TaskSleep(30);

                LT6911_RxPllFreqSet(pChan);

                LT6911_RxPllCalibration(pChan);

                Lt6911_RxCdrEnable(pChan);
                Lt6911_RxPiEnable(pChan);
                Lt6911_PixelPllEnable(pChan);
                Lt6911_HdmiModuleReset(pChan);
                Lt6911_EqSet(pChan);
                Lt6911_RxPiPhaseSet(pChan);

                (void) AmbaKAL_TaskSleep(500);

                (void) Lt6911_TimingCheck(pChan, ModeID);

                Lt6911_ColorSpaceConversion(pChan);

                Lt6911_SyncPolaritySet(pChan);

                Lt6911_TxPhySet(pChan);
                Lt6911_TxPortSet(pChan);

                Lt6911_HdmiToMipiEnable(pChan, LT6911ModeInfo[ModeID].NumDataLanes);

                Lt6911_MipiClockSet(pChan, ModeID);
                Lt6911_MipiDphySet(pChan, ModeID);
                Lt6911_MipiProtocalSet(pChan, ModeID);

                /* config vin */
                RetVal |= LT6911_ConfigVin(VinID, ModeID);
            }
        } else {
            RetVal = YUV_ERR_UNEXPECTED;
        }
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*/
AMBA_YUV_OBJ_s AmbaYuv_LT6911Obj = {
    .Init           = LT6911_Init,
    .Enable         = LT6911_Enable,
    .Disable        = NULL,          /* LT6911 does not support SW standby mode. */
    .Config         = LT6911_Config,
    .GetStatus      = LT6911_GetStatus,
    .GetModeInfo    = LT6911_GetModeInfo
};
