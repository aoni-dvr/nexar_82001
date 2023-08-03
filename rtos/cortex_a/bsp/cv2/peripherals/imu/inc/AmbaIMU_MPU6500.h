/**
 *  @file AmbaIMU_MPU6500.h
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
 *  @details Control APIs of INVENSENSE MPU6500 6-axis Motion sensor with SPI interface
 *
 */

#ifndef AMBA_IMU_MPU6500_H
#define AMBA_IMU_MPU6500_H



/*-----------------------------------------------------------------------------------------------*\
 * Control registers
\*-----------------------------------------------------------------------------------------------*/


//Digital Gyro Register Define
#define REG_DIV             0x19U
#define REG_CFG             0x1AU
#define REG_GYRO_CFG        0x1BU
#define REG_ACC_CFG         0x1CU
#define REG_ACC_CFG_2       0x1DU
#define REG_FIFO_EN         0x23U
#define REG_INT_CFG         0x37U
#define REG_INT_EN          0x38U
#define REG_INT_STATUS      0x3AU
#define REG_ACCEL_XOUT_H    0x3BU
#define REG_ACCEL_XOUT_L    0x3CU
#define REG_ACCEL_YOUT_H    0x3DU
#define REG_ACCEL_YOUT_L    0x3EU
#define REG_ACCEL_ZOUT_H    0x3FU
#define REG_ACCEL_ZOUT_L    0x40U
#define REG_TEMP_OUT_H      0x41U
#define REG_TEMP_OUT_L      0x42U
#define REG_GYRO_XOUT_H     0x43U
#define REG_GYRO_XOUT_L     0x44U
#define REG_GYRO_YOUT_H     0x45U
#define REG_GYRO_YOUT_L     0x46U
#define REG_GYRO_ZOUT_H     0x47U
#define REG_GYRO_ZOUT_L     0x48U
#define REG_USER_CTRL       0x6AU
#define REG_POW_MGM1        0x6BU
#define REG_POW_MGM2        0x6CU
#define REG_FIFO_CNT_H      0x72U
#define REG_FIFO_CNT_L      0x73U

#define REG_FIFO_RW         0x74U
#define REG_ID              0x75U
//Digital Gyro Register Define End

//Data Define
#define OP_READ             (1U << 7U)

/*------------------ Register 0x19 ------------------*/
#ifdef GYRO_INT_GPIO
#define SMPLRT_DIV          0//3                   U       //This value can be modfied if needed
#else
#define SMPLRT_DIV          0U
#endif
/*------------------ Register 0x1A ------------------*/
#define FIFO_MODE           (0U << 6U)
#define EXT_SYNC_SET        (2U << 3U)  // FSYNC bit at GX_L[0]

/*
    DLPF_CFG         Gyro Scope
                    (BW,   Delay, Fs)
    0 :         ( 250hz,  0.97ms, 8khz)
    1 :         ( 184hz,  2.9 ms, 1khz)
    2 :         (  92hz,  3.9 ms, 1khz)
    3 :         (  41hz,  5.9 ms, 1khz)
    4 :         (  20hz,  9.9 ms, 1khz)
    5 :         (  10hz, 17.85ms, 1khz)
    6 :         (   5hz, 33.48ms, 1khz)
    7 :         (3600hz,  0.17ms  8khz)
*/
#define DLPF_CFG            0U
#define GYRO_SAMPLE_RATE    8000U


/*------------------ Register 0x1B ------------------*/
/*
    FS_SEL      Full Scale Range    LSB Sensitivity(LSB/dps)
    0 :         +/-  250            131.0
    1 :         +/-  500            65.5
    2 :         +/- 1000            32.8
    3 :         +/- 2000            16.4
*/
#define FS_SEL_250          (0U << 3U)
#define FS_SEL_500          (1U << 3U)
/*------------------ Register 0x1C ------------------*/
/*
    AFS_SEL     Full Scale Range    LSB Sensitivity(LSB/g)
    0 :         +/-  2g             16384
    1 :         +/-  4g             8192
    2 :         +/-  8g             4096
    3 :         +/- 16g             2048
*/
#define AFS_SEL             (0U << 3U)
/*------------------ Register 0x1D ------------------*/
/*
A_DLPF_CFG      Accelerometer
                (BW, Delay, Fs)
    0 :         ( 460hz,  1.94ms, 1khz)
    1 :         ( 184hz,  5.8 ms, 1khz)
    2 :         (  92hz,  7.8 ms, 1khz)
    3 :         (  41hz, 11.8 ms, 1khz)
    4 :         (  20hz, 19.8 ms, 1khz)
    5 :         (  10hz, 35.7 ms, 1khz)
    6 :         (   5hz, 66.96ms, 1khz)
    7 :         ( 460hz,  1.94ms, 1khz)
*/
#define A_DLPF_CFG          2U
/*------------------ Register 0x23 ------------------*/
#define TEMP_FIFO_EN        (1U << 7U)
#define XG_FIFO_EN          (1U << 6U)
#define YG_FIFO_EN          (1U << 5U)
#define ZG_FIFO_EN          (1U << 4U)
#define ACCEL_FIFO_EN       (1U << 3U)
#define FIFO_EN_CHANNELS    7U//7
/*------------------ Register 0x37 ------------------*/
#define INT_LVL             (1U << 7U)  // 0: active high, 1: active low
#define INT_OPEN            (0U << 6U)  // 0: push-pull, 1: open drain
#define LATCH_INT_EN        (1U << 5U)
#define INT_ANYRD_2CLEAR    (1U << 4U)
#define FSYNC_INT_LEVEL     (0U << 3U)
#define FSYNC_INT_MODE_EN   (1U << 2U)
/*------------------ Register 0x38 ------------------*/
#define FIFO_OVERFLOW_EN    (0U << 4U)
#define FSYNC_INT_EN        (0U << 3U)
#define DATA_RDY_EN         (0U << 0U)
/*------------------ Register 0x3A ------------------*/
#define DATA_RDY_INT_MASK   0x1U
/*------------------ Register 0x6A ------------------*/
#define FIFO_EN             (1U << 6U)
#define I2C_IF_DIS          (1U << 4U)
#define FIFO_RST            (1U << 2U)

/*------------------ Register 0x6B ------------------*/
#define TEMP_DIS            (1U << 3U)
#define CLK_SEL             (3U << 0U)
/*------------------ Register 0x75 ------------------*/
#define WHO_AM_I_ID         (0x70U)

//Data Define End

/* Spi Chan and Slave id for imu control */
#define AMBA_IMU_SPI_CHANNEL     AMBA_SPI_MASTER3
#define AMBA_IMU_SPI_SLAVE_ID    0x1U

extern AMBA_IMU_OBJ_s AmbaIMU_MPU6500Obj;

#endif /* _AMBA_IMU_MPU6500_H_ */
