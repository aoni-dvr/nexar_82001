/**
 *  @file AmbaIMU_MPU6500.c
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
 *
 *  THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 *  WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL
 *  AMBARELLA, INC. OR ITS AFFILIATES BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 *  DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 *  COMPUTER FAILURE OR MALFUNCTION; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 *  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 *  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *  @details Control APIs of INVENSENSE MPU6500 6-axis Motion sensor with SPI interface
 *
 */

// #include <stdio.h>
// #include <stdlib.h>
// #include <string.h>
// #include <math.h>

#include "AmbaTypes.h"
#include "AmbaWrap.h"
#include "AmbaKAL.h"

#include "AmbaSPI.h"
#include "AmbaGPIO.h"
#include "AmbaPWM_Def.h"
#include "AmbaPrint.h"
#include "AmbaIMU.h"
#include "AmbaIMU_MPU6500.h"

//#include "AmbaRTSL_GPIO.h"
#include "AmbaGPIO_Def.h"
//#include "AmbaUserSysCtrl.h"


// //#define MPU6500_DEBUG
// #ifdef MPU6500_DEBUG
// #define MPU6500Print AmbaPrint
// #else
// #define MPU6500Print(...)
// #endif


//#define TEST_PWM_FSYNC
#ifdef TEST_PWM_FSYNC
static UINT8 syncNum[256];
static INT32 Sof_Flag = 0;
#endif

static UINT32 MPU6500_GetStatus(AMBA_IMU_STATUS_s *pStatus);

//---- HW settings for CV25 ----
#define    PWM_SYNC_GPIO         GPIO_PIN_0_PWM7
#define    PWM_SYNC_CHAN         AMBA_PWM_CHANNEL7

//#define     GYRO_PWR_GPIO
//#define     GYRO_INT_GPIO

// #if defined CONFIG_BSP_H22DK_OPTION_A
// #define    DEF_SPI_MASTER        AMBA_SPI_MASTER2
// #else
// #define    DEF_SPI_MASTER        AMBA_SPI_MASTER1
// #endif

//#define    DEF_SPI_EN             0
//----------------------------------------

#define    GYRO_XOUT_POLARITY     1
#define    GYRO_YOUT_POLARITY    -1
#define    GYRO_ZOUT_POLARITY    -1
#define    GYRO_XOUT_CHANNEL      1
#define    GYRO_YOUT_CHANNEL      0

#define    GYRO_FULL_SCALE_500

#define    SPI_CTRL_TimeOut  1000U

static AMBA_SPI_CONFIG_s Mpu6500_Spi_Ctrl = {
    .BaudRate       = 1000000U,                                 /* Transfer BaudRate in Hz  1000000*/
    .CsPolarity     = AMBA_SPI_CHIP_SELECT_ACTIVE_LOW,          /* Slave select polarity */
    .ClkMode        = AMBA_SPI_CPOL_HIGH_CPHA_HIGH,             /* SPI Protocol mode */
    .DataFrameSize  = 8U,                                       /* Data Frame Size in Bit */
    .FrameBitOrder  = AMBA_SPI_TRANSFER_MSB_FIRST,
};


static AMBA_SPI_CONFIG_s Mpu6500_Spi_Ctrl_Read = {
    .BaudRate         = 7000000U,                               /* Transfer BaudRate in Hz 7000000*/
    .CsPolarity       = AMBA_SPI_CHIP_SELECT_ACTIVE_LOW,        /* Slave select polarity */
    .ClkMode          = AMBA_SPI_CPOL_HIGH_CPHA_HIGH,           /* SPI Protocol mode */
    .DataFrameSize    = 8U,                                     /* Data Frame Size in Bit */
    .FrameBitOrder    = AMBA_SPI_TRANSFER_MSB_FIRST,
};

typedef struct {
    UINT8 SpiChannel;
    UINT8 SpiSlaveID;
} IMU_SPI_CTRL_SELECTs;

static IMU_SPI_CTRL_SELECTs SpiCtrlSelect = {
    .SpiChannel = AMBA_IMU_SPI_CHANNEL,
    .SpiSlaveID = AMBA_IMU_SPI_SLAVE_ID,
};




static UINT8 FlagImuInit = 0U;

static UINT8 RegTxBuf[32];      // reg_read Tx buffer
static UINT8 RegRxBuf[32];      // reg_read Rx buffer
static UINT8 FiFoTxBuf[4096];   // fifo_read Tx buffer
static UINT8 FiFoRxBuf[4096];   // fifo_read Rx buffer

static UINT16 SwFrameSync = 0U;
static INT8 DLpfDlyTable[8] = {1, 3, 4, 6, 10, 18, 33, 0};

/**** user configs ****/
static UINT8  UserDLpfCfg = 0U;
static UINT16 UserGyroSampleRate = 0U;
static AMBA_IMU_INFO_s MPU6500DeviceInfo = {0};


static inline UINT16 TransletData(const UINT8 *pData)
{
    UINT16 TempH = 0x0000;
    UINT16 TempL = 0x0000;
    TempH = TempH | pData[0];
    TempL = TempL | pData[1];
    return ((TempH << 8U) | (TempL));
}



static UINT32 MPU6500_SetFuncAlt(void)
{

    (void)AmbaGPIO_SetFuncAlt(GPIO_PIN_13_SPI3_SCLK);
    (void)AmbaGPIO_SetFuncAlt(GPIO_PIN_14_SPI3_MOSI);
    (void)AmbaGPIO_SetFuncAlt(GPIO_PIN_15_SPI3_MISO);
    (void)AmbaGPIO_SetFuncAlt(GPIO_PIN_16_SPI3_SS0);
    return IMU_ERR_NONE;
}



/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MPU6500_GetInfo
 *
 *  @Description:: Get Motion Sensor information
 *
 *  @Input      :: none
 *
 *  @Output     ::
 *      pImuInfo: pointer to Device Info
 *
 *  @Return     ::
 *          int : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/


static UINT32 MPU6500_GetInfo(AMBA_IMU_INFO_s *pImuInfo)
{
    static UINT8 flg_once = 0U;
    UINT8 misra_u8;
    UINT16 misra_u16;
    if (flg_once == 0U) {
        //flg_once = 1;
        if(AmbaWrap_memset(&MPU6500DeviceInfo, 0, sizeof(AMBA_IMU_INFO_s)) != 0U) {
            AmbaPrint_PrintUInt5("ERROR: Cannot set MPU6500DeviceInfo as 0 !!!", 0U, 0U, 0U, 0U, 0U);
        }
        else {
            MPU6500DeviceInfo.GyroID = WHO_AM_I_ID;
            MPU6500DeviceInfo.Capability = ((1U << IMU_READ_BY_FIFO) | (1U << IMU_FRAME_SYNC));
        }

#ifdef PWM_SYNC_GPIO
        MPU6500DeviceInfo.PwmSyncGpio = (UINT16)PWM_SYNC_GPIO;
#else
        MPU6500DeviceInfo.PwmSyncGpio = 0U;
#endif

#ifdef PWM_SYNC_CHAN
        MPU6500DeviceInfo.PwmSyncChan = (UINT16)PWM_SYNC_CHAN;
#else
        MPU6500DeviceInfo.PwmSyncChan = 0U;
#endif

#ifdef GYRO_PWR_GPIO
        MPU6500DeviceInfo.GyroPwrGpio = GYRO_PWR_GPIO;
#endif

#ifdef GYRO_INT_GPIO
        MPU6500DeviceInfo.GyroIntGpio = GYRO_INT_GPIO;
#endif

#ifndef GYRO_XOUT_POLARITY
        MPU6500DeviceInfo.GyroXPolar = 1;
#else
        MPU6500DeviceInfo.GyroXPolar = GYRO_XOUT_POLARITY;
        if (MPU6500DeviceInfo.GyroXPolar == 0) {
            MPU6500DeviceInfo.GyroXPolar = -1;
        }

#endif

#ifndef GYRO_YOUT_POLARITY
        MPU6500DeviceInfo.GyroYPolar = 1;
#else
        MPU6500DeviceInfo.GyroYPolar = GYRO_YOUT_POLARITY;
        if (MPU6500DeviceInfo.GyroYPolar == 0) {
            MPU6500DeviceInfo.GyroYPolar = -1;
        }

#endif

#ifndef GYRO_ZOUT_POLARITY
        MPU6500DeviceInfo.GyroZPolar = -1;
#else
        MPU6500DeviceInfo.GyroZPolar = GYRO_ZOUT_POLARITY;
        if (MPU6500DeviceInfo.GyroZPolar == 0) {
            MPU6500DeviceInfo.GyroZPolar = -1;
        }

#endif

        if (MPU6500DeviceInfo.GyroXPolar == MPU6500DeviceInfo.GyroYPolar) {
            if (MPU6500DeviceInfo.GyroXPolar > 0) {
                MPU6500DeviceInfo.AccXPolar = (INT8)AMBA_IMU_NEGATIVE;
                MPU6500DeviceInfo.AccYPolar = (INT8)AMBA_IMU_POSITIVE;
            } else {
                MPU6500DeviceInfo.AccXPolar = (INT8)AMBA_IMU_POSITIVE;
                MPU6500DeviceInfo.AccYPolar = (INT8)AMBA_IMU_NEGATIVE;
            }
        } else {
            if (MPU6500DeviceInfo.GyroXPolar > 0) {
                MPU6500DeviceInfo.AccXPolar = (INT8)AMBA_IMU_POSITIVE;
                MPU6500DeviceInfo.AccYPolar = (INT8)AMBA_IMU_POSITIVE;
            } else {
                MPU6500DeviceInfo.AccXPolar = (INT8)AMBA_IMU_NEGATIVE;
                MPU6500DeviceInfo.AccYPolar = (INT8)AMBA_IMU_NEGATIVE;
            }

        }
        if (MPU6500DeviceInfo.AccXPolar == 0) {
            MPU6500DeviceInfo.AccXPolar = -1;
        }

        if (MPU6500DeviceInfo.AccYPolar == 0) {
            MPU6500DeviceInfo.AccYPolar = -1;
        }


#ifndef GYRO_XOUT_CHANNEL
        MPU6500DeviceInfo.GyroXChan = 0U;
#else
        MPU6500DeviceInfo.GyroXChan = GYRO_XOUT_CHANNEL;
#endif

#ifndef GYRO_YOUT_CHANNEL
        MPU6500DeviceInfo.GyroYChan = 1U;
#else
        MPU6500DeviceInfo.GyroYChan = GYRO_YOUT_CHANNEL;
#endif

        if ((MPU6500DeviceInfo.GyroXChan == (UINT32)AMBA_IMU_X) && (MPU6500DeviceInfo.GyroYChan == (UINT32)AMBA_IMU_Y)) {
            MPU6500DeviceInfo.AccXChan = (INT16)AMBA_IMU_Y;
            MPU6500DeviceInfo.AccYChan = (INT16)AMBA_IMU_X;
        } else {
            MPU6500DeviceInfo.AccXChan = (INT16)AMBA_IMU_X;
            MPU6500DeviceInfo.AccYChan = (INT16)AMBA_IMU_Y;
        }

#ifndef GYRO_VOL_DIVIDER_NUM
        MPU6500DeviceInfo.VolDivNum = 1U;
#else
        MPU6500DeviceInfo.VolDivNum = GYRO_VOL_DIVIDER_NUM;
#endif

#ifndef GYRO_VOL_DIVIDER_DEN
        MPU6500DeviceInfo.VolDivDen = 1U;
#else
        MPU6500DeviceInfo.VolDivDen = GYRO_VOL_DIVIDER_DEN;
#endif

        MPU6500DeviceInfo.SensorInterface   = (UINT8)AMBA_IMU_SPI;
        MPU6500DeviceInfo.SensorAxis        = (UINT8)AMBA_IMU_6AXIS;
#ifndef GYRO_FULL_SCALE_500
        MPU6500DeviceInfo.MaxSense          = 135U;                             // LSB/(deg/sec)
        MPU6500DeviceInfo.MinSense          = 127U;                             // LSB/(deg/sec)
#else
        MPU6500DeviceInfo.MaxSense          = 66U;                              // LSB/(deg/sec)
        MPU6500DeviceInfo.MinSense          = 66U;                              // LSB/(deg/sec)
#endif
        MPU6500DeviceInfo.MaxBias           = 35388U;                           // LSB     //2620;
        MPU6500DeviceInfo.MinBias           = 30148U;                           // LSB     //-2620;
        MPU6500DeviceInfo.MaxRmsNoise       = 30U;                              // LSB
        MPU6500DeviceInfo.StartUpTime       = 50U;                              // ms (35ms in spec)
#ifndef GYRO_FULL_SCALE_500
        MPU6500DeviceInfo.FullScaleRange    = 250U;                             // deg/sec
#else
        MPU6500DeviceInfo.FullScaleRange    = 500U;                             // deg/sec
#endif
        MPU6500DeviceInfo.MaxSamplingRate   = 8000U;                            // sample/sec
        MPU6500DeviceInfo.AdcResolution     = 16U;                              // bits
        MPU6500DeviceInfo.PhsDly            = DLpfDlyTable[UserDLpfCfg] + 2;    // ms
        MPU6500DeviceInfo.AccMaxSense       = 16876U;                           // LSB/g
        MPU6500DeviceInfo.AccMinSense       = 15892U;                           // LSB/g
        MPU6500DeviceInfo.AccMaxBias        = 34734U;                           // LSB  //1966;
        MPU6500DeviceInfo.AccMinBias        = 30802U;                           // LSB  //-1966;

        if(UserGyroSampleRate != 0U) {
            MPU6500DeviceInfo.SamplingRate  = UserGyroSampleRate;
        } else {
            MPU6500DeviceInfo.SamplingRate  = GYRO_SAMPLE_RATE;
        }
        //MPU6500DeviceInfo.SamplingRate    = (UserGyroSampleRate? UserGyroSampleRate : GYRO_SAMPLE_RATE);//MPU6500DeviceInfo.MaxSamplingRate / (SMPLRT_DIV + 1);
        misra_u8 = MPU6500DeviceInfo.AdcResolution - 1U;
        misra_u16 = ((UINT16)1U << misra_u8);
        MPU6500DeviceInfo.LevelShift                = misra_u16;
        MPU6500DeviceInfo.FiFoDepth                 = 4096U;            /* bytes */
        MPU6500DeviceInfo.FiFoEnable.Channels       = FIFO_EN_CHANNELS;
        MPU6500DeviceInfo.FiFoEnable.Temperature    = TEMP_FIFO_EN >> 7U;
        MPU6500DeviceInfo.FiFoEnable.GyroX          = XG_FIFO_EN >> 6U;
        MPU6500DeviceInfo.FiFoEnable.GyroY          = YG_FIFO_EN >> 5U;
        MPU6500DeviceInfo.FiFoEnable.GyroZ          = ZG_FIFO_EN >> 4U;
        MPU6500DeviceInfo.FiFoEnable.AccX           = ACCEL_FIFO_EN >> 3U;
        MPU6500DeviceInfo.FiFoEnable.AccY           = ACCEL_FIFO_EN >> 3U;
        MPU6500DeviceInfo.FiFoEnable.AccZ           = ACCEL_FIFO_EN >> 3U;
    }
    if(AmbaWrap_memcpy(pImuInfo, &MPU6500DeviceInfo, sizeof(AMBA_IMU_INFO_s)) != 0U) {
        AmbaPrint_PrintUInt5("ERROR: Cannot copy MPU6500DeviceInfo to pImuInfo!!!", 0U, 0U, 0U, 0U, 0U);
    }


    return IMU_ERR_NONE;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MPU6500_Open
 *
 *  @Description:: Open Motion Sensor
 *
 *  @Input      :: none
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          int : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static UINT32 MPU6500_Open(void)
{
    static INT16 OpenSuccess = 0;
    UINT8 reg[2];
    UINT8 ID[2] = {0};
    UINT32 Ret = IMU_ERR_NONE;
    UINT32 ActualSize = 0U;
    UINT32 NumBytes = 2U;



    (void)MPU6500_SetFuncAlt();

    if (OpenSuccess == -1) {
        AmbaPrint_PrintUInt5("ERROR: Gyro open fail!!!", 0U, 0U, 0U, 0U, 0U);
        Ret = IMU_ERR_OPNE_FAIL;
    } else {
        reg[0] = (UINT8)(REG_ID | OP_READ);

        if(0U != AmbaSPI_MasterTransferD8((UINT32)SpiCtrlSelect.SpiChannel, (UINT32)SpiCtrlSelect.SpiSlaveID, &Mpu6500_Spi_Ctrl, (UINT32)NumBytes, reg, ID, &ActualSize, SPI_CTRL_TimeOut)) {
            AmbaPrint_PrintUInt5("AmbaSPI_MasterTransferD8 NG!! reg[0]:%x reg[1]:%x", reg[0], reg[1], 0U, 0U, 0U);
        }

        if (ID[1] != WHO_AM_I_ID) {
            AmbaPrint_PrintUInt5("ERROR: Invensense gyro ID [0x%x], read gyro ID [0x%x]", WHO_AM_I_ID, ID[1], 0U, 0U, 0U);
            AmbaPrint_PrintUInt5("ERROR: Cannot open gyro with SPI!!!", 0U, 0U,0U, 0U, 0U);
            OpenSuccess = -1;
            Ret = IMU_ERR_OPNE_FAIL;
        } else {

            //---------- power set------------
            reg[0] = REG_POW_MGM1;
            reg[1] = (UINT8)(TEMP_DIS | CLK_SEL);

            if(0U != AmbaSPI_MasterTransferD8((UINT32)SpiCtrlSelect.SpiChannel, (UINT32)SpiCtrlSelect.SpiSlaveID, &Mpu6500_Spi_Ctrl, (UINT32)NumBytes, reg, NULL, &ActualSize, SPI_CTRL_TimeOut)) {
                AmbaPrint_PrintUInt5("AmbaSPI_MasterTransferD8 NG!! reg[0]:%x reg[1]:%x", reg[0], reg[1], 0U, 0U, 0U);
            }

            //--------------------------------

            reg[0] = REG_DIV;
            reg[1] = SMPLRT_DIV;

            if(0U != AmbaSPI_MasterTransferD8((UINT32)SpiCtrlSelect.SpiChannel, (UINT32)SpiCtrlSelect.SpiSlaveID, &Mpu6500_Spi_Ctrl, (UINT32)NumBytes, reg, NULL, &ActualSize, SPI_CTRL_TimeOut)) {
                AmbaPrint_PrintUInt5("AmbaSPI_MasterTransferD8 NG!! reg[0]:%x reg[1]:%x", reg[0], reg[1], 0U, 0U, 0U);
            }
            reg[0] = REG_CFG;

            if(UserDLpfCfg == 0U) {
                reg[1] = (UINT8)(EXT_SYNC_SET | DLPF_CFG | FIFO_MODE);
            } else {
                reg[1] = (UINT8)(EXT_SYNC_SET | UserDLpfCfg | FIFO_MODE);
            }



            if(0U != AmbaSPI_MasterTransferD8((UINT32)SpiCtrlSelect.SpiChannel, (UINT32)SpiCtrlSelect.SpiSlaveID, &Mpu6500_Spi_Ctrl, (UINT32)NumBytes, reg, NULL, &ActualSize, SPI_CTRL_TimeOut)) {
                AmbaPrint_PrintUInt5("AmbaSPI_MasterTransferD8 NG!! reg[0]:%x reg[1]:%x", reg[0], reg[1], 0U, 0U, 0U);
            }

            reg[0] = REG_GYRO_CFG;
#ifndef GYRO_FULL_SCALE_500
            reg[1] = FS_SEL_250;
#else
            reg[1] = FS_SEL_500;
#endif

            if(0U != AmbaSPI_MasterTransferD8((UINT32)SpiCtrlSelect.SpiChannel, (UINT32)SpiCtrlSelect.SpiSlaveID, &Mpu6500_Spi_Ctrl, (UINT32)NumBytes, reg, NULL, &ActualSize, SPI_CTRL_TimeOut)) {
                AmbaPrint_PrintUInt5("AmbaSPI_MasterTransferD8 NG!! reg[0]:%x reg[1]:%x", reg[0], reg[1], 0U, 0U, 0U);
            }

            reg[0] = REG_ACC_CFG;
            reg[1] = AFS_SEL;

            if(0U != AmbaSPI_MasterTransferD8((UINT32)SpiCtrlSelect.SpiChannel, (UINT32)SpiCtrlSelect.SpiSlaveID, &Mpu6500_Spi_Ctrl, (UINT32)NumBytes, reg, NULL, &ActualSize, SPI_CTRL_TimeOut)) {
                AmbaPrint_PrintUInt5("AmbaSPI_MasterTransferD8 NG!! reg[0]:%x reg[1]:%x", reg[0], reg[1], 0U, 0U, 0U);
            }

            reg[0] = REG_USER_CTRL;
            reg[1] = (UINT8)(FIFO_EN | I2C_IF_DIS | FIFO_RST);
            if(0U != AmbaSPI_MasterTransferD8((UINT32)SpiCtrlSelect.SpiChannel, (UINT32)SpiCtrlSelect.SpiSlaveID, &Mpu6500_Spi_Ctrl, (UINT32)NumBytes, reg, NULL, &ActualSize, SPI_CTRL_TimeOut)) {
                AmbaPrint_PrintUInt5("AmbaSPI_MasterTransferD8 NG!! reg[0]:%x reg[1]:%x", reg[0], reg[1], 0U, 0U, 0U);
            }

            reg[0] = REG_FIFO_EN;
            reg[1] = (UINT8)(TEMP_FIFO_EN | XG_FIFO_EN | YG_FIFO_EN | ZG_FIFO_EN | ACCEL_FIFO_EN);
            if(0U != AmbaSPI_MasterTransferD8((UINT32)SpiCtrlSelect.SpiChannel, (UINT32)SpiCtrlSelect.SpiSlaveID, &Mpu6500_Spi_Ctrl, (UINT32)NumBytes, reg, NULL, &ActualSize, SPI_CTRL_TimeOut)) {
                AmbaPrint_PrintUInt5("AmbaSPI_MasterTransferD8 NG!! reg[0]:%x reg[1]:%x", reg[0], reg[1], 0U, 0U, 0U);
            }

            reg[0] = REG_POW_MGM2;
            reg[1] = 0x00U;  // enable all 6 axes output
            if(0U != AmbaSPI_MasterTransferD8((UINT32)SpiCtrlSelect.SpiChannel, (UINT32)SpiCtrlSelect.SpiSlaveID, &Mpu6500_Spi_Ctrl, (UINT32)NumBytes, reg, NULL, &ActualSize, SPI_CTRL_TimeOut)) {
                AmbaPrint_PrintUInt5("AmbaSPI_MasterTransferD8 NG!! reg[0]:%x reg[1]:%x", reg[0], reg[1], 0U, 0U, 0U);
            }

            reg[0] = REG_INT_CFG;
            if (MPU6500DeviceInfo.GyroIntGpio != 0U) {
                reg[1] = (UINT8)(INT_LVL | INT_OPEN | INT_ANYRD_2CLEAR | LATCH_INT_EN | FSYNC_INT_LEVEL | FSYNC_INT_MODE_EN);
            } else {
                reg[1] = LATCH_INT_EN;
            }
            if(0U != AmbaSPI_MasterTransferD8((UINT32)SpiCtrlSelect.SpiChannel, (UINT32)SpiCtrlSelect.SpiSlaveID, &Mpu6500_Spi_Ctrl, (UINT32)NumBytes, reg, NULL, &ActualSize, SPI_CTRL_TimeOut)) {
                AmbaPrint_PrintUInt5("AmbaSPI_MasterTransferD8 NG!! reg[0]:%x reg[1]:%x", reg[0], reg[1], 0U, 0U, 0U);
            }

            reg[0] = REG_INT_EN;
            if (MPU6500DeviceInfo.GyroIntGpio != 0U) {
                reg[1] = DATA_RDY_EN;
                if(0U != AmbaSPI_MasterTransferD8((UINT32)SpiCtrlSelect.SpiChannel, (UINT32)SpiCtrlSelect.SpiSlaveID, &Mpu6500_Spi_Ctrl, (UINT32)NumBytes, reg, NULL, &ActualSize, SPI_CTRL_TimeOut)) {
                    AmbaPrint_PrintUInt5("AmbaSPI_MasterTransferD8 NG!! reg[0]:%x reg[1]:%x", reg[0], reg[1], 0U, 0U, 0U);
                }
            }

            /* reset tx/rx buffers */
            if(AmbaWrap_memset(&RegTxBuf, 0, sizeof(RegTxBuf)) != 0U) {
                AmbaPrint_PrintUInt5("ERROR: Cannot set RegTxBuf as 0", 0U, 0U, 0U, 0U, 0U);
            }
            if(AmbaWrap_memset(&RegRxBuf, 0, sizeof(RegRxBuf)) != 0U) {
                AmbaPrint_PrintUInt5("ERROR: Cannot set RegRxBuf as 0", 0U, 0U, 0U, 0U, 0U);
            }
            if(AmbaWrap_memset(&FiFoTxBuf, 0, sizeof(FiFoTxBuf)) != 0U) {
                AmbaPrint_PrintUInt5("ERROR: Cannot set FiFoTxBuf as 0", 0U, 0U, 0U, 0U, 0U);
            }
            if(AmbaWrap_memset(&FiFoRxBuf, 0, sizeof(FiFoRxBuf)) != 0U) {
                AmbaPrint_PrintUInt5("ERROR: Cannot set FiFoRxBuf as 0", 0U, 0U, 0U, 0U, 0U);
            }

            FlagImuInit = 0;

        }

    }

    return Ret;
}


/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MPU6500_Read
 *
 *  @Description:: Read Motion Sensor Data
 *
 *  @Input      ::
 *      Type:   Decide Gyro or Acc data to read
 *
 *  @Output     ::
 *      pData:  Motion Sensor Data Structure
 *
 *  @Return     ::
 *          int : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static UINT32 MPU6500_Read(UINT8 Type, AMBA_IMU_DATA_s* pData)
{
    UINT32 FrameByte;

    UINT32 Ret = IMU_ERR_NONE;
    UINT32 ActualSize = 0;
    UINT32 NumBytes = 0;
#ifdef MPU6500_DEBUG
    static int Disp_count = 0;
#endif

#ifdef TEST_PWM_FSYNC
    static int syncCount = 0;
    static int bitCount = 0;
    static int start_count = 0;
#endif


    FrameByte = (UINT32)MPU6500DeviceInfo.FiFoEnable.Channels * 2U;


    (void)MPU6500_SetFuncAlt();


    if (Type == (UINT8)AMBA_IMU_GYRO) {


        /*User get Gyro data*/
        RegTxBuf[0] = (UINT8)(REG_GYRO_XOUT_H | OP_READ);
        NumBytes = 7;
        if(0U != AmbaSPI_MasterTransferD8((UINT32)SpiCtrlSelect.SpiChannel, (UINT32)SpiCtrlSelect.SpiSlaveID, &Mpu6500_Spi_Ctrl_Read, (UINT32)NumBytes, RegTxBuf, RegRxBuf, &ActualSize, SPI_CTRL_TimeOut)) {
            AmbaPrint_PrintUInt5("AmbaSPI_MasterTransferD8 NG!! reg[0]:%x reg[1]:%x", RegTxBuf[0], RegTxBuf[1], 0U, 0U, 0U);
        }

        if ((MPU6500DeviceInfo.GyroXChan == (UINT8)AMBA_IMU_X) && (MPU6500DeviceInfo.GyroYChan == (UINT8)AMBA_IMU_Y)) {
            pData->GyroXData = TransletData(&RegRxBuf[1]);
            pData->GyroYData = TransletData(&RegRxBuf[3]);
        } else {
            pData->GyroYData = TransletData(&RegRxBuf[1]);
            pData->GyroXData = TransletData(&RegRxBuf[3]);
        }

        pData->GyroZData = TransletData(&RegRxBuf[5]);
        pData->GyroXData += MPU6500DeviceInfo.LevelShift;
        pData->GyroYData += MPU6500DeviceInfo.LevelShift;
        pData->GyroZData += MPU6500DeviceInfo.LevelShift;

    } else if (Type == (UINT8)AMBA_IMU_ACCMETER) {

        /*User get G-Sensor data*/
        RegTxBuf[0] = (UINT8)(REG_ACCEL_XOUT_H | OP_READ);
        NumBytes = 7U;
        if(0U != AmbaSPI_MasterTransferD8((UINT32)SpiCtrlSelect.SpiChannel, (UINT32)SpiCtrlSelect.SpiSlaveID, &Mpu6500_Spi_Ctrl_Read, (UINT32)NumBytes, RegTxBuf, RegRxBuf, &ActualSize, SPI_CTRL_TimeOut)) {
            AmbaPrint_PrintUInt5("AmbaSPI_MasterTransferD8 NG!! reg[0]:%x reg[1]:%x", RegTxBuf[0], RegTxBuf[1], 0U, 0U, 0U);
        }

        if ((MPU6500DeviceInfo.AccXChan == (UINT8)AMBA_IMU_Y) && (MPU6500DeviceInfo.AccYChan == (UINT8)AMBA_IMU_X)) {
            pData->AccXData = TransletData(&RegRxBuf[3]);
            pData->AccYData = TransletData(&RegRxBuf[1]);
        } else {
            pData->AccXData = TransletData(&RegRxBuf[1]);
            pData->AccYData = TransletData(&RegRxBuf[3]);
        }
        pData->AccZData = TransletData(&RegRxBuf[5]);

        pData->AccXData += MPU6500DeviceInfo.LevelShift;
        pData->AccYData += MPU6500DeviceInfo.LevelShift;
        pData->AccZData += MPU6500DeviceInfo.LevelShift;
    } else if (Type == (UINT8)AMBA_IMU_COMBO) {
        AmbaPrint_PrintUInt5("AMBA_IMU_COMBO Type = %d", Type, 0U, 0U, 0U, 0U);
        /*User get ALL data*/
        RegTxBuf[0] = (UINT8)(REG_ACCEL_XOUT_H | OP_READ);
        NumBytes = 15;
        if(0U != AmbaSPI_MasterTransferD8((UINT32)SpiCtrlSelect.SpiChannel, (UINT32)SpiCtrlSelect.SpiSlaveID, &Mpu6500_Spi_Ctrl_Read, (UINT32)NumBytes, RegTxBuf, RegRxBuf, &ActualSize, SPI_CTRL_TimeOut)) {
            AmbaPrint_PrintUInt5("AmbaSPI_MasterTransferD8 NG!! reg[0]:%x reg[1]:%x", RegTxBuf[0], RegTxBuf[1], 0U, 0U, 0U);
        }

        if ((MPU6500DeviceInfo.AccXChan == (UINT16)AMBA_IMU_Y) && (MPU6500DeviceInfo.AccYChan == (UINT16)AMBA_IMU_X)) {
            pData->AccXData = TransletData(&RegRxBuf[3]);
            pData->AccYData = TransletData(&RegRxBuf[1]);
        } else {
            pData->AccXData = TransletData(&RegRxBuf[1]);
            pData->AccYData = TransletData(&RegRxBuf[3]);
        }
        pData->AccZData = TransletData(&RegRxBuf[5]);

        pData->AccXData += MPU6500DeviceInfo.LevelShift;
        pData->AccYData += MPU6500DeviceInfo.LevelShift;
        pData->AccZData += MPU6500DeviceInfo.LevelShift;

        /* Output Temperature data */
        pData->TemperatureData = TransletData(&RegRxBuf[7]);
        pData->TemperatureData += MPU6500DeviceInfo.LevelShift;

        if ((MPU6500DeviceInfo.GyroXChan == (UINT8)AMBA_IMU_X) && (MPU6500DeviceInfo.GyroYChan == (UINT8)AMBA_IMU_Y)) {
            pData->GyroXData = TransletData(&RegRxBuf[9]);
            pData->GyroYData = TransletData(&RegRxBuf[11]);
        } else {
            pData->GyroYData = TransletData(&RegRxBuf[9]);
            pData->GyroXData = TransletData(&RegRxBuf[11]);
        }
        pData->GyroZData = TransletData(&RegRxBuf[13]);

        pData->GyroXData += MPU6500DeviceInfo.LevelShift;
        pData->GyroYData += MPU6500DeviceInfo.LevelShift;
        pData->GyroZData += MPU6500DeviceInfo.LevelShift;

        /* get frame sync in fifo */
        pData->FrameSync = SwFrameSync;

    } else if (Type == (UINT8)AMBA_IMU_FIFO_COMBO) {

        static UINT32 FifoByteCnt = 0U;
        static UINT32 ReadIndex = 0U;
        UINT16 misra_u16;
        UINT8 misra_u8;
        INT8 AccXEntry;
        INT8 AccYEntry;
        INT8 AccZEntry;
        INT8 TempEntry;
        INT8 GyroXEntry;
        INT8 GyroYEntry;
        INT8 GyroZEntry;

        /*User get ALL data*/
        if(((FifoByteCnt == 0U) && (ReadIndex == 0U))) {
            FiFoTxBuf[0] = (UINT8)(REG_FIFO_CNT_H | OP_READ);
            FiFoRxBuf[0]=0U;
            FiFoRxBuf[1]=0U;
            FiFoRxBuf[2]=0U;

            NumBytes = 3U;

            if(0U != AmbaSPI_MasterTransferD8((UINT32)SpiCtrlSelect.SpiChannel, (UINT32)SpiCtrlSelect.SpiSlaveID, &Mpu6500_Spi_Ctrl_Read, (UINT32)NumBytes, FiFoTxBuf, FiFoRxBuf, &ActualSize, SPI_CTRL_TimeOut)) {
                AmbaPrint_PrintUInt5("AmbaSPI_MasterTransferD8 NG!! reg[0]:%x reg[1]:%x", (UINT32)FiFoTxBuf[0], (UINT32)FiFoTxBuf[1], 0U, 0U, 0U);
            }

            misra_u16 = TransletData(&FiFoRxBuf[1]);
            FifoByteCnt = (0x1FFFUL) & misra_u16;
            FifoByteCnt = FifoByteCnt - (FifoByteCnt%FrameByte);


            if (FifoByteCnt != 0U) {
                FiFoTxBuf[0] = (UINT8)(REG_FIFO_RW | OP_READ);
                NumBytes = FifoByteCnt + 1U;
                if(0U != AmbaSPI_MasterTransferD8((UINT32)SpiCtrlSelect.SpiChannel, (UINT32)SpiCtrlSelect.SpiSlaveID, &Mpu6500_Spi_Ctrl_Read, (UINT32)NumBytes, FiFoTxBuf, FiFoRxBuf, &ActualSize, SPI_CTRL_TimeOut)) {
                    AmbaPrint_PrintUInt5("AmbaSPI_MasterTransferD8 NG!! reg[0]:%x reg[1]:%x", RegTxBuf[0], RegTxBuf[1], 0U, 0U, 0U);
                }
            }
#if 0
            if (0) {
                UINT8 data_temp[3] = {0U, 0U, 0U};
                AMBA_IMU_STATUS_s sts = {0U};

                FiFoTxBuf[0] = (UINT8)(REG_FIFO_CNT_H | OP_READ);
                NumBytes = 3U;
                if(0U != AmbaSPI_MasterTransferD8((UINT32)SpiCtrlSelect.SpiChannel, (UINT32)SpiCtrlSelect.SpiSlaveID, &Mpu6500_Spi_Ctrl_Read, (UINT32)NumBytes, FiFoTxBuf, data_temp, &ActualSize, SPI_CTRL_TimeOut)) {
                    AmbaPrint_PrintUInt5("AmbaSPI_MasterTransferD8 NG!! reg[0]:%x reg[1]:%x", RegTxBuf[0], RegTxBuf[1], 0U, 0U, 0U);
                }

                (void)MPU6500_GetStatus(&sts);

                if (sts.FiFoOverFlow != 0U) {
                    UINT8 reg[2];
                    reg[0] = REG_USER_CTRL;
                    reg[1] = (UINT8)(FIFO_EN | I2C_IF_DIS | FIFO_RST);
                    NumBytes = 2U;

                    if(0U != AmbaSPI_MasterTransferD8((UINT32)SpiCtrlSelect.SpiChannel, (UINT32)SpiCtrlSelect.SpiSlaveID, &Mpu6500_Spi_Ctrl, (UINT32)NumBytes, reg, NULL, &ActualSize, SPI_CTRL_TimeOut)) {
                        AmbaPrint_PrintUInt5("AmbaSPI_MasterTransferD8 NG!! reg[0]:%x reg[1]:%x", RegTxBuf[0], RegTxBuf[1], 0U, 0U, 0U);
                    }
                }
                AmbaPrint_PrintUInt5("REG_FIFO_CNT (%d %d) sts: %d", data_temp[1], data_temp[2], sts.FiFoOverFlow, 0U, 0U);

            }
#endif

#ifdef TEST_PWM_FSYNC
            /* reset Fsync counter "syncCount" &  Fsync array "syncNum" */
            syncCount = 0;
            memset(syncNum,0,sizeof(syncNum));
#endif
        }





        /*User get ALL data*/

        if (FlagImuInit == 0U) {
            /* reset fifo at the 1st time read */
            UINT8 reg[2];
            reg[0] = REG_USER_CTRL;
            reg[1] = (UINT8)(FIFO_EN | I2C_IF_DIS | FIFO_RST);
            NumBytes = 2;
            if(0U != AmbaSPI_MasterTransferD8((UINT32)SpiCtrlSelect.SpiChannel, (UINT32)SpiCtrlSelect.SpiSlaveID, &Mpu6500_Spi_Ctrl, (UINT32)NumBytes, reg, NULL, &ActualSize, SPI_CTRL_TimeOut)) {
                AmbaPrint_PrintUInt5("AmbaSPI_MasterTransferD8 NG!! reg[0]:%x reg[1]:%x", RegTxBuf[0], RegTxBuf[1], 0U, 0U, 0U);
            }


            FlagImuInit = 1U;
            FifoByteCnt = 0U;
            Ret = IMU_READ_FIFO_INIT;
        } else {
            // AmbaPrint_PrintUInt5("FifoByteCnt = %d", FifoByteCnt, 0U, 0U, 0U, 0U);
            if (FifoByteCnt < FrameByte) { //Data empty
                ReadIndex = 0U;
                FifoByteCnt = 0U;
#ifdef TEST_PWM_FSYNC
                /* reset Fsync counter "syncCount" &  Fsync array "syncNum" */
                syncNum[syncCount] = '\0';
                AmbaPrint("%s", syncNum);  // show FSYNC bits
                syncCount = 0;
                AmbaWrap_memset(syncNum,0,sizeof(syncNum));
#endif
                if(AmbaWrap_memset(FiFoRxBuf,0,sizeof(FiFoRxBuf)) != 0U) {
                    AmbaPrint_PrintUInt5("Error: Cannot set FiFoRxBuf as 0!!!", 0U, 0U, 0U, 0U, 0U);
                }
                Ret = IMU_READ_FIFO_END;
            } else {

                INT32 misra_i32;
                /* find fifo data entry */
                if (MPU6500DeviceInfo.FiFoEnable.AccX != 0U) {
                    AccXEntry = 1;
                    AccYEntry = 3;
                    AccZEntry = 5;
                    TempEntry = 7;
                    GyroXEntry = 9;
                    GyroYEntry = 11;
                    GyroZEntry = 13;
                } else {
                    AccXEntry = -1;
                    AccYEntry = -1;
                    AccZEntry = -1;
                    TempEntry = 1;
                    GyroXEntry = 3;
                    GyroYEntry = 5;
                    GyroZEntry = 7;
                }

                /* get gyro data in fifo */
                if (((MPU6500DeviceInfo.GyroXChan == (UINT8)AMBA_IMU_X) && (MPU6500DeviceInfo.GyroYChan == (UINT8)AMBA_IMU_Y))) {
                    misra_i32 = (INT32)((INT32)ReadIndex * (INT32)FrameByte) + (INT32)GyroXEntry;
                    pData->GyroXData = TransletData(&FiFoRxBuf[misra_i32]);
                    misra_i32 = (INT32)((INT32)ReadIndex * (INT32)FrameByte) + (INT32)GyroYEntry;
                    pData->GyroYData = TransletData(&FiFoRxBuf[misra_i32]);
                } else {

                    misra_i32 = (INT32)((INT32)ReadIndex * (INT32)FrameByte) + (INT32)GyroYEntry;
                    pData->GyroXData = TransletData(&FiFoRxBuf[misra_i32]);
                    misra_i32 = (INT32)((INT32)ReadIndex * (INT32)FrameByte) + (INT32)GyroXEntry;
                    pData->GyroYData = TransletData(&FiFoRxBuf[misra_i32]);
                }

                misra_i32 = (INT32)((INT32)ReadIndex * (INT32)FrameByte) + (INT32)GyroZEntry;
                pData->GyroZData = TransletData(&FiFoRxBuf[misra_i32]);

#ifdef MPU6500_DEBUG
                if((Disp_count % 400 == 0)) {
                    ("FifoByteCnt=%5d  X=%10.3f Y=%10.3f Z=%10.3f",FifoByteCnt,(FLOAT)((FLOAT)(SHORT)pData->GyroXData*7.6/1000),(FLOAT)((FLOAT)(SHORT)pData->GyroYData*7.6/1000),(FLOAT)((FLOAT)(SHORT)pData->GyroZData*7.6/1000));
                }
#endif
                pData->GyroXData += MPU6500DeviceInfo.LevelShift;
                pData->GyroYData += MPU6500DeviceInfo.LevelShift;
                pData->GyroZData += MPU6500DeviceInfo.LevelShift;


                /* get temperature in fifo */

                misra_i32 = (INT32)((INT32)ReadIndex * (INT32)FrameByte) + (INT32)TempEntry;
                pData->TemperatureData = TransletData(&FiFoRxBuf[misra_i32]);
                pData->TemperatureData += MPU6500DeviceInfo.LevelShift;


                /* get acc data in fifo, if existing */
                if (MPU6500DeviceInfo.FiFoEnable.AccX != 0U) {

                    if ((MPU6500DeviceInfo.AccXChan == (UINT16)AMBA_IMU_X) && (MPU6500DeviceInfo.AccYChan == (UINT16)AMBA_IMU_Y)) {
                        misra_i32 = (INT32)((INT32)ReadIndex * (INT32)FrameByte) + (INT32)AccXEntry;
                        pData->AccXData = TransletData(&FiFoRxBuf[misra_i32]);
                        misra_i32 = (INT32)((INT32)ReadIndex * (INT32)FrameByte) + (INT32)AccYEntry;
                        pData->AccYData = TransletData(&FiFoRxBuf[misra_i32]);
                    } else {
                        misra_i32 = (INT32)((INT32)ReadIndex * (INT32)FrameByte) + (INT32)AccYEntry;
                        pData->AccXData = TransletData(&FiFoRxBuf[misra_i32]);
                        misra_i32 = (INT32)((INT32)ReadIndex * (INT32)FrameByte) + (INT32)AccXEntry;
                        pData->AccYData = TransletData(&FiFoRxBuf[misra_i32]);
                    }
                    misra_i32 = (INT32)((INT32)ReadIndex * (INT32)FrameByte) + (INT32)AccZEntry;
                    pData->AccZData = TransletData(&FiFoRxBuf[misra_i32]);
#ifdef MPU6500_DEBUG
                    if((Disp_count % 400 == 0)) {
                        ("FifoByteCnt=%5d  AX=%10.3f AY=%10.3f AZ=%10.3f",FifoByteCnt,(float)((float)(SHORT)pData->AccXData/16384),(float)((float)(SHORT)pData->AccYData/16384),(float)((float)(SHORT)pData->AccZData/16384));
                        Disp_count = 0;
                    }
                    Disp_count++;
#endif
                    pData->AccXData += MPU6500DeviceInfo.LevelShift;
                    pData->AccYData += MPU6500DeviceInfo.LevelShift;
                    pData->AccZData += MPU6500DeviceInfo.LevelShift;
                }


                /* get frame sync in fifo */
                misra_i32 = (INT32)(((INT32)ReadIndex * (INT32)FrameByte) + (INT32)GyroXEntry) + 1;
                misra_u8 = (FiFoRxBuf[misra_i32] % 2U);
                pData->FrameSync = (UINT16)misra_u8;

#ifdef TEST_PWM_FSYNC
                syncNum[syncCount] = pData->FrameSync + '0';  // the bit "GX_L[0]" is 1 or 0

                if((Sof_Flag == 1) && (syncNum[syncCount] == '1')) {
                    start_count = 1;
                    Sof_Flag = 0;
                    /* Below variable "bitCount":
                        show how many samples during the period from
                        previous SOF (the 1st "1" bit) through current SOF
                        (count the last "0" bit from previous SOF before the current SOF 1st "1" bit) */
                    AmbaPrint("bit Num = %d", bitCount);
                    bitCount = 0;
                }
                if(start_count == 1) {
                    bitCount++;
                }

                syncCount++;
#endif


                FifoByteCnt -= FrameByte;
                ReadIndex++;
            }
        }
    } else {
        AmbaPrint_PrintUInt5("MPU6500 read", 0U, 0U, 0U, 0U, 0U);
    }

    return Ret;
}


/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MPU6500_RegRead
 *
 *  @Description:: MPU6500 Register Read Function
 *
 *  @Input      ::
 *      Addr:       Register Address
 *      SubAddr:    NULL
 *      NumBytes:   The number of data read
 *
 *  @Output     ::
 *      pInbuf :    pointer to user buffer
 *
 *  @Return     ::
 *          int : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static UINT32 MPU6500_RegRead(UINT16 Addr, UINT16 SubAddr, UINT16 *pInbuf, UINT16 NumBytes)
{
    UINT8 reg[3];
    UINT8 data_temp[3] = {0};
    UINT32 Ret;
    UINT32 ActualSize = 0U;
    (void)SubAddr;

    (void)AmbaGPIO_SetFuncAlt(GPIO_PIN_13_SPI3_SCLK);
    (void)AmbaGPIO_SetFuncAlt(GPIO_PIN_14_SPI3_MOSI);
    (void)AmbaGPIO_SetFuncAlt(GPIO_PIN_15_SPI3_MISO);
    (void)AmbaGPIO_SetFuncAlt(GPIO_PIN_16_SPI3_SS0);

    if (NumBytes > 2U) {
        Ret = IMU_ERR_REGREAD_FAIL;
    } else {
        UINT16 misra_u16;
        misra_u16 = (Addr | OP_READ);
        reg[0] = (UINT8)misra_u16;
        if(0U != AmbaSPI_MasterTransferD8((UINT32)SpiCtrlSelect.SpiChannel, (UINT32)SpiCtrlSelect.SpiSlaveID, &Mpu6500_Spi_Ctrl, ((UINT32)NumBytes + 1U), &reg[0], &data_temp[0], &ActualSize, SPI_CTRL_TimeOut)) {
            AmbaPrint_PrintUInt5("AmbaSPI_MasterTransferD8 NG!!  reg[0]:%x reg[1]:%x", reg[0], reg[1], 0U, 0U, 0U);
        }

        pInbuf[0] = data_temp[1];
        if (NumBytes == 2U) {
            pInbuf[1] = data_temp[2];
        }
        Ret = IMU_ERR_NONE;
    }
    return Ret;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MPU6500_RegWrite
 *
 *  @Description:: MPU6500 Register Write Function
 *
 *  @Input      ::
 *      Addr:       Register Address
 *      SubAddr:    NULL
 *      InData:     Data user want to write
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          int : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static UINT32 MPU6500_RegWrite(UINT16 Addr, UINT16 SubAddr, UINT16 InData)
{
    UINT8 reg[2];
    UINT32 ActualSize = 0U;
    UINT32 NumBytes = 2U;

    reg[0] = (UINT8)Addr;
    reg[1] = (UINT8)InData;

    (void)MPU6500_SetFuncAlt();
    (void)SubAddr;
    // AmbaSPI_Transfer(DEF_SPI_MASTER, &Mpu6500_Spi_Ctrl, 2, reg, NULL, 1000); //CV22 Not support
    if(0U != AmbaSPI_MasterTransferD8((UINT32)SpiCtrlSelect.SpiChannel, (UINT32)SpiCtrlSelect.SpiSlaveID, &Mpu6500_Spi_Ctrl, (UINT32)NumBytes, reg, NULL, &ActualSize, SPI_CTRL_TimeOut)) {
        AmbaPrint_PrintUInt5("AmbaSPI_MasterTransferD8 NG!!  reg[0]:%x reg[1]:%x", reg[0], reg[1], 0U, 0U, 0U);
    }
    return IMU_ERR_NONE;
}



/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MPU6500_SetSampleRate
 *
 *  @Description:: Set Motion Sensor Sampling Rate
 *
 *  @Input      ::
 *      SampleRate: Desired sampling rate, Hz
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          int : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static UINT32 MPU6500_SetSampleRate(UINT16 SampleRate)
{
    UINT32 Ret = IMU_ERR_NONE;
    if (SampleRate == 1000U) {
        UserDLpfCfg = 1U;
        UserGyroSampleRate = 1000U;
    } else if (SampleRate == 8000U) {
        UserDLpfCfg = 0U;
        UserGyroSampleRate = 8000U;
    } else {
        AmbaPrint_PrintUInt5("[MPU6500] gyro sampling rate not supported!", 0U, 0U, 0U, 0U, 0U);
        Ret = IMU_ERR_SETSAMPLERATE_FAIL;
    }
    if(Ret != IMU_ERR_SETSAMPLERATE_FAIL) {
        AmbaPrint_PrintUInt5("[MPU6500] MPU6500_SetSampleRate %d %d",  UserDLpfCfg, UserGyroSampleRate, 0U, 0U, 0U);
    }


    return Ret;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MPU6500_Close
 *
 *  @Description::
 *
 *  @Input      :: none
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          int : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static UINT32 MPU6500_Close(void)
{
    FlagImuInit = 0U;
    return IMU_ERR_NONE;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MPU6500_IoCtrl
 *
 *  @Description::
 *
 *  @Input      :: Test command id and parameters (param1, param2)
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          int : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static UINT32 MPU6500_IoCtrl(UINT16 Id, UINT32 param1, UINT32 param2)
{
    UINT32 ActualSize = 0U;

    (void)param2;
    (void)MPU6500_SetFuncAlt();


    switch (Id) {

    case 3: {
        UINT8 reg[2];
        UINT32 NumBytes = 2U;
        UINT32 misra_u32;
        reg[0] = REG_GYRO_CFG;
        reg[1] = (UINT8)(param1 << 3UL);
        if(0U != AmbaSPI_MasterTransferD8((UINT32)SpiCtrlSelect.SpiChannel, (UINT32)SpiCtrlSelect.SpiSlaveID, &Mpu6500_Spi_Ctrl, (UINT32)NumBytes, &reg[0], NULL, &ActualSize, SPI_CTRL_TimeOut)) {
            AmbaPrint_PrintUInt5("AmbaSPI_MasterTransferD8 NG!!  reg[0]:%x reg[1]:%x", reg[0], reg[1], 0U, 0U, 0U);
        }
        misra_u32 = 32768U / (UINT32)(250UL << param1);
        MPU6500DeviceInfo.MaxSense          = (UINT16)misra_u32;            /* LSB/(deg/sec)  */
        misra_u32 = 32768U / (UINT32)(250UL << param1);
        MPU6500DeviceInfo.MinSense          = (UINT16)misra_u32;            /* LSB/(deg/sec)  */
        misra_u32 = (250UL << param1);
        MPU6500DeviceInfo.FullScaleRange    = (UINT16)misra_u32;    /* deg/sec        */
    }
    break;

    case 5:
        // for code compatible
        break;

    case 8: {
        UINT8 reg[2];
        UINT32 NumBytes = 2U;
        UserDLpfCfg = (UINT8)param1;
        reg[0] = REG_CFG;
        reg[1] = (UINT8)(EXT_SYNC_SET | UserDLpfCfg | FIFO_MODE);

        if(0U != AmbaSPI_MasterTransferD8((UINT32)SpiCtrlSelect.SpiChannel, (UINT32)SpiCtrlSelect.SpiSlaveID, &Mpu6500_Spi_Ctrl, (UINT32)NumBytes, reg, NULL, &ActualSize, SPI_CTRL_TimeOut)) {
            AmbaPrint_PrintUInt5("AmbaSPI_MasterTransferD8 NG!!  reg[0]:%x reg[1]:%x", reg[0], reg[1], 0U, 0U, 0U);
        }
    }
    break;

    case 9: {

        UINT32 NumBytes = 3U;

        FiFoTxBuf[0] = (UINT8)(REG_FIFO_CNT_H | OP_READ);
        FiFoRxBuf[0]=0U;
        FiFoRxBuf[1]=0U;
        FiFoRxBuf[2]=0U;


        if(0U != AmbaSPI_MasterTransferD8((UINT32)SpiCtrlSelect.SpiChannel, (UINT32)SpiCtrlSelect.SpiSlaveID, &Mpu6500_Spi_Ctrl_Read, NumBytes, FiFoTxBuf, FiFoRxBuf, &ActualSize, SPI_CTRL_TimeOut)) {
            AmbaPrint_PrintUInt5("AmbaSPI_MasterTransferD8 NG!! reg[0]:%x reg[1]:%x", FiFoTxBuf[0], FiFoTxBuf[1], 0U, 0U, 0U);
        }
    }


    break;
    case 10:
        SwFrameSync = 1U;
        break;
    case 11:
        SwFrameSync = 0U;
        break;


#ifdef TEST_PWM_FSYNC
    case 50:
        Sof_Flag = param1;
        break;
#endif


    default:
        AmbaPrint_PrintUInt5("invalid t cmd id...",  0U, 0U, 0U, 0U, 0U);

        break;
    }

    return IMU_ERR_NONE;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MPU6500_GetStatus
 *
 *  @Description:: Get IMU device status
 *
 *  @Input      :: none
 *
 *  @Output     ::
 *      pImuInfo: pointer to device status
 *
 *  @Return     ::
 *          int : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
static UINT32 MPU6500_GetStatus(AMBA_IMU_STATUS_s *pStatus)
{
    UINT8 reg[2];
    UINT8 Status[2] = {0U};
    UINT32 ActualSize = 0U;
    UINT32 NumBytes = 2U;

    (void)MPU6500_SetFuncAlt();

    reg[0] = (UINT8)(REG_INT_STATUS | OP_READ);
    if(0U != AmbaSPI_MasterTransferD8((UINT32)SpiCtrlSelect.SpiChannel, (UINT32)SpiCtrlSelect.SpiSlaveID, &Mpu6500_Spi_Ctrl, (UINT32)NumBytes, reg, Status, &ActualSize, SPI_CTRL_TimeOut)) {
        AmbaPrint_PrintUInt5("AmbaSPI_MasterTransferD8 NG!!  reg[0]:%x reg[1]:%x", reg[0], reg[1], 0U, 0U, 0U);
    }
    pStatus->FiFoOverFlow = (Status[1] & 0x10U) >> 4U;

    return IMU_ERR_NONE;
}

/*-----------------------------------------------------------------------------------------------*/
AMBA_IMU_OBJ_s AmbaIMU_MPU6500Obj = {
    .GetDeviceInfo            = MPU6500_GetInfo,
    .Open                     = MPU6500_Open,
    .Read                     = MPU6500_Read,
    .RegRead                  = MPU6500_RegRead,
    .RegWrite                 = MPU6500_RegWrite,
    .SetSampleRate            = MPU6500_SetSampleRate,
    .Close                    = MPU6500_Close,
    .IoCtrl                   = MPU6500_IoCtrl,
    .GetStatus                = MPU6500_GetStatus,
};
