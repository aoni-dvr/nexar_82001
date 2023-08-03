/**
 *  @file AmbaIMU.h
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
 *  @details Definitions for Ambarella IMU (Accelerometer and Gyroscope Devices) driver APIs
 *
 */

#ifndef AMBA_IMU_H
#define AMBA_IMU_H

#include "AmbaTypes.h"
#include "AmbaErrorCode.h"


#define IMU_READ_BY_FIFO   0U
#define IMU_FRAME_SYNC     1U
#define IMU_SENSOR_TIME    2U
#define IMU_TEMPERATURE    3U


/* ERROR CODE */

#define IMU_ERR_NONE          (0U)
#define IMU_ERR_INVALID_API   (IMU_ERR_BASE + 1U)
#define IMU_ERR_OPNE_FAIL     (IMU_ERR_BASE + 2U)
#define IMU_ERR_READ_FAIL     (IMU_ERR_BASE + 3U)
#define IMU_ERR_REGREAD_FAIL  (IMU_ERR_BASE + 4U)
#define IMU_ERR_REGWRITE_FAIL (IMU_ERR_BASE + 5U)
#define IMU_ERR_SETSAMPLERATE_FAIL (IMU_ERR_BASE + 6U)

#define IMU_READ_FIFO_END (IMU_ERR_BASE + 99U)
#define IMU_READ_FIFO_INIT (IMU_ERR_BASE + 98U)


typedef enum {
    AMBA_IMU_SENSOR0,
    AMBA_IMU_SENSOR1,

    AMBA_NUM_IMU_SENSOR
} AMBA_IMU_NUM_e;

typedef enum {
    AMBA_IMU_NULL,
    AMBA_IMU_GYRO,      /* 3-axis gyroscope */
    AMBA_IMU_ACCMETER,  /* 3-axis accelerometer */
    AMBA_IMU_COMBO,     /* 6-axis data */
    AMBA_IMU_FIFO_GYRO,      /* 3-axis gyro data from fifo */
    AMBA_IMU_FIFO_ACCMETER,  /* 3-axis acc data from fifo */
    AMBA_IMU_FIFO_COMBO,     /* 6-axis data from fifo */

} AMBA_IMU_TYPE_e;

typedef enum {
    AMBA_IMU_SPI,
    AMBA_IMU_I2C,

} AMBA_IMU_INTERFACE_e_;

typedef enum {
    AMBA_IMU_2AXIS,
    AMBA_IMU_3AXIS,
    AMBA_IMU_6AXIS,

} AMBA_IMU_AXIS_e_;

typedef enum {
    AMBA_IMU_X,
    AMBA_IMU_Y,
    AMBA_IMU_Z,

} AMBA_IMU_DIR_e_;

typedef enum {
    AMBA_IMU_NEGATIVE,
    AMBA_IMU_POSITIVE,

} AMBA_IMU_POLARITY_e_;

typedef struct {
    UINT16 GyroXData;
    UINT16 GyroYData;
    UINT16 GyroZData;
    UINT16 TemperatureData;
    UINT16 AccXData;
    UINT16 AccYData;
    UINT16 AccZData;
    UINT16 FrameSync;
} AMBA_IMU_DATA_s;

typedef struct {
    UINT32  GyroX:          1;
    UINT32  GyroY:          1;
    UINT32  GyroZ:          1;
    UINT32  AccX:           1;
    UINT32  AccY:           1;
    UINT32  AccZ:           1;
    UINT32  Reserved:       4;
    UINT32  Temperature:    1;
    UINT32  Channels:       5;
} AMBA_IMU_FIFO_CHANNEL_s;

typedef struct {
    UINT8   GyroID;             /* gyro sensor id*/
    UINT32  Capability;         /* imu sensor capability*/
    UINT16  PwmSyncGpio;       /* GPIO number outputs pwm serving as fifo fsync signal */
    UINT16  PwmSyncChan;        /* PWM channel number outputs pwm serving as fifo fsync signal */
    UINT8   GyroPwrGpio;        /* GPIO number controls gyro sensor power*/
    UINT8   GyroHpsGpio;        /* GPIO number controls gyro sensor hps*/
    UINT8   GyroIntGpio;        /* GPIO number connect to gyro sensor interrupt pin*/
    UINT8   GyroXChan;          /* gyro sensor x axis Channel*/
    UINT8   GyroYChan;          /* gyro sensor y axis Channel*/
    UINT8   GyroZChan;          /* gyro sensor z axis Channel*/
    UINT8   GyroTChan;          /* gyro sensor t axis Channel*/
    UINT8   GyroXReg;           /* gyro sensor x axis reg*/
    UINT8   GyroYReg;           /* gyro sensor y axis reg*/
    UINT8   GyroZReg;           /* gyro sensor z axis reg*/
    INT8    GyroXPolar;         /* gyro sensor x polarity*/
    INT8    GyroYPolar;         /* gyro sensor y polarity*/
    INT8    GyroZPolar;         /* gyro sensor z polarity*/
    UINT8   VolDivNum;          /* numerator of voltage divider*/
    UINT8   VolDivDen;          /* denominator of voltage divider*/

    UINT8   SensorInterface;    /* gyro sensor interface*/
    UINT8   SensorAxis;         /* gyro sensor axis*/
    UINT8   MaxRmsNoise;        /* gyro sensor rms noise level*/
    UINT8   AdcResolution;      /* gyro adc resolution, unit in bit(s)*/
    INT8    PhsDly;             /* gyro sensor phase delay, unit in ms*/
    UINT16  SamplingRate;       /* gyro sampling rate, unit in samples / sec*/
    UINT16  MaxSamplingRate;    /* max gyro sampling rate, unit in samples / sec*/
    UINT16  MaxBias;            /* max gyro sensor bias*/
    UINT16  MinBias;            /* min gyro sensor bias*/
    UINT16  MaxSense;           /* max gyro sensor sensitivity*/
    UINT16  MinSense;           /* min gyro sensor sensitivity*/
    UINT16  StartUpTime;        /* gyro sensor start-up time*/
    UINT16  FullScaleRange;     /* gyro full scale range*/
    UINT16  MaxSclk;            /* max serial clock for interface, unit in 100khz*/

    UINT16  FiFoDepth;          /* imu device fifo depth, in bytes */
    AMBA_IMU_FIFO_CHANNEL_s FiFoEnable; /* imu fifo channel enables */

    UINT16  AccXChan;           /* acc sensor x axis Channel*/
    UINT16  AccYChan;           /* acc sensor y axis Channel*/
    UINT16  AccZChan;           /* acc sensor z axis Channel*/
    INT8    AccXPolar;          /* acc sensor x polarity*/
    INT8    AccYPolar;          /* acc sensor y polarity*/
    INT8    AccZPolar;          /* acc sensor z polarity*/
    UINT16  AccSamplingRate;    /* acc sampling rate, unit in samples / sec*/
    UINT16  AccMaxSamplingRate; /* max acc sampling rate, unit in samples / sec*/
    UINT16  AccMaxBias;
    UINT16  AccMinBias;
    UINT16  AccMaxSense;        /* max acc sensor sensitivity*/
    UINT16  AccMinSense;        /* min acc sensor sensitivity*/
    UINT8   AccMaxRmsNoise;     /* acc sensor rms noise level*/

    UINT16  LevelShift;
} AMBA_IMU_INFO_s;

typedef struct {
    UINT8   FiFoOverFlow;
    UINT8   Reserved[7];
} AMBA_IMU_STATUS_s;

typedef struct {
    UINT32 VinID;
    UINT32 Reserved;
} AMBA_IMU_CHANNEL_s;

typedef struct {
    UINT32 (*GetDeviceInfo)(AMBA_IMU_INFO_s *pImuInfo);
    UINT32 (*Open)(void);
    UINT32 (*Read)(UINT8 Type, AMBA_IMU_DATA_s *pData);
    UINT32 (*RegRead)(UINT16 Addr, UINT16 SubAddr, UINT16 *pInbuf, UINT16 NumBytes);
    UINT32 (*RegWrite)(UINT16 Addr, UINT16 SubAddr, UINT16 InData);
    UINT32 (*SetSampleRate)(UINT16 TimeInUs);
    UINT32 (*Close)(void);
    UINT32 (*IoCtrl)(UINT16 Id, UINT32 param1, UINT32 param2);
    UINT32 (*GetStatus)(AMBA_IMU_STATUS_s *pStatus);
} AMBA_IMU_OBJ_s;


/*-----------------------------------------------------------------------------------------------*\
 * Defined in AmbaIMU.c
\*-----------------------------------------------------------------------------------------------*/
extern AMBA_IMU_OBJ_s *pAmbaImuObj[AMBA_NUM_IMU_SENSOR];
extern void AmbaIMU_Hook(UINT8 Chan, AMBA_IMU_OBJ_s *pImuObj);

static inline UINT32 AmbaIMU_GetDeviceInfo(UINT8 Chan, AMBA_IMU_INFO_s *pDevInfo)
{
    return ((pAmbaImuObj[Chan]->GetDeviceInfo == NULL) ? IMU_ERR_INVALID_API : pAmbaImuObj[Chan]->GetDeviceInfo(pDevInfo));
}
static inline UINT32 AmbaIMU_Open(UINT8 Chan)
{
    return ((pAmbaImuObj[Chan]->Open == NULL) ? IMU_ERR_INVALID_API : pAmbaImuObj[Chan]->Open());
}
static inline UINT32 AmbaIMU_Read(UINT8 Chan,UINT8 Type, AMBA_IMU_DATA_s *pData)
{
    return ((pAmbaImuObj[Chan]->Read == NULL) ? IMU_ERR_INVALID_API : pAmbaImuObj[Chan]->Read(Type, pData));
}
static inline UINT32 AmbaIMU_RegRead(UINT8 Chan, UINT16 Addr, UINT16 SubAddr, UINT16 *pInbuf, UINT16 NumBytes)
{
    return ((pAmbaImuObj[Chan]->RegRead == NULL) ? IMU_ERR_INVALID_API : pAmbaImuObj[Chan]->RegRead(Addr, SubAddr, pInbuf, NumBytes));
}
static inline UINT32 AmbaIMU_RegWrite(UINT8 Chan, UINT16 Addr, UINT16 SubAddr, UINT16 InData)
{
    return ((pAmbaImuObj[Chan]->RegWrite == NULL) ? IMU_ERR_INVALID_API : pAmbaImuObj[Chan]->RegWrite(Addr, SubAddr, InData));
}
static inline UINT32 AmbaIMU_SetSampleRate(UINT8 Chan, UINT16 TimeInUs)
{
    return ((pAmbaImuObj[Chan]->SetSampleRate == NULL) ? IMU_ERR_INVALID_API : pAmbaImuObj[Chan]->SetSampleRate(TimeInUs));
}
static inline UINT32 AmbaIMU_Close(UINT8 Chan)
{
    return ((pAmbaImuObj[Chan]->Close == NULL) ? IMU_ERR_INVALID_API : pAmbaImuObj[Chan]->Close());
}
static inline UINT32 AmbaIMU_IoCtrl(UINT8 Chan, UINT16 Id, UINT32 param1, UINT32 param2)
{
    return ((pAmbaImuObj[Chan]->IoCtrl == NULL) ? IMU_ERR_INVALID_API : pAmbaImuObj[Chan]->IoCtrl(Id,param1,param2));
}
static inline UINT32 AmbaIMU_GetStatus(UINT8 Chan,AMBA_IMU_STATUS_s *pStatus)
{
    return ((pAmbaImuObj[Chan]->GetStatus == NULL) ? IMU_ERR_INVALID_API : pAmbaImuObj[Chan]->GetStatus(pStatus));
}

#endif /* _AMBA_IMU_H_ */

