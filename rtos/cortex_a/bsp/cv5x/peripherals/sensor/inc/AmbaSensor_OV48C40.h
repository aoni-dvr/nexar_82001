/**
 *  @file AmbaSensor_OV48C40.h
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
 *  @details Control APIs of OV 48C40 CMOS sensor with MIPI interface
 *
 */

#ifndef AMBA_SENSOR_OV48C40_H
#define AMBA_SENSOR_OV48C40_H

#define OV48C40_CPHY

#define OV48C40_I2C_SLAVE_ADDRESS    0x6CU

//#define OV48C40_NUM_READOUT_MODE_REG      (1977U)   // 627U+288U +864U +198U
#define OV48C40_NUM_READOUT_MODE_REG_TOP     (627U)
#define OV48C40_NUM_READOUT_MODE_REG_MID1    (288U)
#define OV48C40_NUM_READOUT_MODE_REG_MID2    (864U)
#define OV48C40_NUM_READOUT_MODE_REG_BOTTOM  (198U)

#define OV48C40_NUM_DISABLE_PD_REGS          (1440U)    //1440U
#define OV48C40_NUM_DCG_VS_REGS              (12U) //1990-1977-1U

#define OV48C40_NUM_POST_REGS                (40U)
#define OV48C40_NUM_POST_REG_MODES           (14U)
#define OV48C40_NUM_4032_3024_END_REGS       (4U)  //2022-1977-40-1

/*-----------------------------------------------------------------------------------------------*\
 * Control registers
\*-----------------------------------------------------------------------------------------------*/

#define    OV48C40_MODE_SEL_REG               0x100U
#define    OV48C40_FRM_LENGTH_LINES_MSB_REG   0x1111U
#define    OV48C40_EXPO_COARSE_HIGH_REG       0x3500U
#define    OV48C40_EXPO_COARSE_MED_REG        0x3501U
#define    OV48C40_EXPO_COARSE_LOW_REG        0x3502U
#define    OV48C40_ANA_GAIN_GLOBAL_MSB_REG    0x3508U
#define    OV48C40_ANA_GAIN_GLOBAL_LSB_REG    0x3509U
#define    OV48C40_DGAIN_GLOBAL_COARSE_REG    0x350AU
#define    OV48C40_DGAIN_GLOBAL_MSB_REG       0x350BU
#define    OV48C40_DGAIN_GLOBAL_LSB_REG       0x350CU
#define    OV48C40_DGAIN_GB_COARSE_REG        0x3510U
#define    OV48C40_DGAIN_GB_MSB_REG           0x3511U
#define    OV48C40_DGAIN_GB_LSB_REG           0x3512U
#define    OV48C40_DGAIN_GR_COARSE_REG        0x3513U
#define    OV48C40_DGAIN_GR_MSB_REG           0x3514U
#define    OV48C40_DGAIN_GR_LSB_REG           0x3515U
#define    OV48C40_DGAIN_R_COARSE_REG         0x3516U
#define    OV48C40_DGAIN_R_MSB_REG            0x3517U
#define    OV48C40_DGAIN_R_LSB_REG            0x3518U
#define    OV48C40_EXPO_MED_HIGH_REG          0x3540U
#define    OV48C40_EXPO_MED_MED_REG           0x3541U
#define    OV48C40_EXPO_MED_LOW_REG           0x3542U
#define    OV48C40_ANA_GAIN_MED_MSB_REG       0x3548U
#define    OV48C40_ANA_GAIN_MED_LSB_REG       0x3549U
#define    OV48C40_EXPO_SHORT_HIGH_REG        0x3580U
#define    OV48C40_EXPO_SHORT_MED_REG         0x3581U
#define    OV48C40_EXPO_SHORT_LOW_REG         0x3582U
#define    OV48C40_ANA_GAIN_SHORT_MSB_REG     0x3588U
#define    OV48C40_ANA_GAIN_SHORT_LSB_REG     0x3589U

/*-----------------------------------------------------------------------------------------------*\
 *  Application Mode Definition
\*-----------------------------------------------------------------------------------------------*/
#define    OV48C40_7680_4320_24P                   0U
#define    OV48C40_6352_3834_30P                   1U
#define    OV48C40_4032_3024_60P                   2U
#define    OV48C40_4032_3024_50P                   3U
#define    OV48C40_4032_3024_48P                   4U
#define    OV48C40_4032_3024_30P                   5U
#define    OV48C40_4032_3024_25P                   6U
#define    OV48C40_4032_3024_24P                   7U
#define    OV48C40_1920_1080_30P                   8U
#define    OV48C40_1280_720_480P                   9U
#define    OV48C40_3840_2160_120P                 10U
#define    OV48C40_3840_2160_100P                 11U
#define    OV48C40_3840_2160_30P                  12U
#define    OV48C40_2016_1512_240P                 13U
#define    OV48C40_2016_1512_200P                 14U
#define    OV48C40_2016_1512_120P                 15U
#define    OV48C40_2016_1512_100P                 16U
#define    OV48C40_2016_1512_60P                  17U
#define    OV48C40_2016_1512_50P                  18U
#define    OV48C40_2016_1512_48P                  19U
#define    OV48C40_2016_1512_30P                  20U
#define    OV48C40_2016_1512_25P                  21U
#define    OV48C40_2016_1512_24P                  22U
#define    OV48C40_8064_6048_15P                  23U
#define    OV48C40_6352_5512_15P                  24U
#define    OV48C40_3680_2756_30P                  25U
#define    OV48C40_3680_2068_30P                  26U
#define    OV48C40_4032_3024_30P_DCG_HDR          27U
#define    OV48C40_4032_3024_25P_DCG_HDR          28U
#define    OV48C40_4032_3024_24P_DCG_HDR          29U
#define    OV48C40_2016_1512_60P_DCG_HDR          30U
#define    OV48C40_2016_1512_30P_DCG_HDR          31U
#define    OV48C40_2016_1512_25P_DCG_HDR          32U
#define    OV48C40_4032_3024_27P_DCG_VS_HDR       33U
#define    OV48C40_4032_3024_25P_DCG_VS_HDR       34U
#define    OV48C40_4032_3024_24P_DCG_VS_HDR       35U
#define    OV48C40_2016_1512_30P_DCG_VS_HDR       36U
#define    OV48C40_2016_1512_25P_DCG_VS_HDR       37U
#define    OV48C40_NUM_MODE                       38U

typedef struct {
    UINT16  Addr;
    UINT8   Data;
} OV48C40_REG_s;

typedef struct {
    UINT16  Addr;
    UINT8   Data[OV48C40_NUM_MODE];
} OV48C40_MODE_REG_s;

typedef struct {
    UINT16  Addr;
    UINT8   Data[OV48C40_NUM_POST_REG_MODES];
} OV48C40_MODE_POST_REG_s;

typedef struct {
    UINT32  InputClk;                       /* Sensor side input clock frequency */
    UINT32  PixelRate;                      /* Sensor side pixel rate */
    UINT32  Linelengthpck;                  /* XHS period (in input clock cycles) */
    UINT32  FrameLengthLines;               /* horizontal operating period (in number of XHS pulses) */
    AMBA_VIN_FRAME_RATE_s   FrameRate;      /* frame rate value of this sensor mode */
} OV48C40_FRAME_TIMING_s;

typedef struct {
    OV48C40_FRAME_TIMING_s   FrameTiming;
    UINT32  HdrType;
    FLOAT   RowTime;
} OV48C40_MODE_INFO_s;

typedef struct  {
    AMBA_SENSOR_STATUS_INFO_s   Status;
    UINT32                      CurrentAgc[3];
    UINT32                      CurrentDgc[3];
    UINT32                      CurrentShrCtrl[3];
} OV48C40_CTRL_s;

/*-----------------------------------------------------------------------------------------------*\
 *  Defined in AmbaSensor_OV48C40.c
\*-----------------------------------------------------------------------------------------------*/
extern const AMBA_SENSOR_DEVICE_INFO_s OV48C40DeviceInfo;
extern const AMBA_SENSOR_INPUT_INFO_s OV48C40InputInfoNormalReadout[OV48C40_NUM_MODE];
extern const AMBA_SENSOR_OUTPUT_INFO_s OV48C40OutputInfo[OV48C40_NUM_MODE];
extern OV48C40_MODE_INFO_s OV48C40ModeInfoList[OV48C40_NUM_MODE];
extern const AMBA_SENSOR_HDR_INFO_s OV48C40HdrInfo[OV48C40_NUM_MODE];

extern OV48C40_MODE_REG_s OV48C40ModeRegTableTop[OV48C40_NUM_READOUT_MODE_REG_TOP];
extern OV48C40_MODE_REG_s OV48C40ModeRegTableMid1[OV48C40_NUM_READOUT_MODE_REG_MID1];
extern OV48C40_MODE_REG_s OV48C40ModeRegTableMid2[OV48C40_NUM_READOUT_MODE_REG_MID2];
extern OV48C40_MODE_REG_s OV48C40ModeRegTableBottom[OV48C40_NUM_READOUT_MODE_REG_BOTTOM];
extern UINT32 PostRegSensorModeMapping[OV48C40_NUM_POST_REGS];
extern OV48C40_MODE_POST_REG_s OV48C40ModePostRegTable[OV48C40_NUM_POST_REGS];

extern OV48C40_REG_s OV48C40_DISABLE_PD_REG_TABLE[OV48C40_NUM_DISABLE_PD_REGS];
extern OV48C40_REG_s OV48C40_4032_3024_END_REG_TABLE[OV48C40_NUM_4032_3024_END_REGS];
extern OV48C40_REG_s OV48C40_DCG_VS_REG_TABLE[OV48C40_NUM_DCG_VS_REGS];

extern AMBA_SENSOR_OBJ_s AmbaSensor_OV48C40Obj;

#endif /* _AMBA_SENSOR_OV48C40_H_ */
