/**
 *  @file AmbaSensor_IMX290_MIPI.h
 *
 *  Copyright (c) 2020 Ambarella International LP
 *
 *  This file and its contents ("Software") are protected by intellectual
 *  property rights including, without limitation, U.S. and/or foreign
 *  copyrights. This Software is also the confidential and proprietary
 *  information of Ambarella International LP and its licensors. You may not use, reproduce,
 *  disclose, distribute, modify, or otherwise prepare derivative works of this
 *  Software or any portion thereof except pursuant to a signed license agreement
 *  or nondisclosure agreement with Ambarella International LP or its authorized affiliates.
 *  In the absence of such an agreement, you agree to promptly notify and return
 *  this Software to Ambarella International LP.
 *
 *  This file includes sample code and is only for internal testing and evaluation.  If you
 *  distribute this sample code (whether in source, object, or binary code form), it will be
 *  without any warranty or indemnity protection from Ambarella International LP or its affiliates.
 *
 *  THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
 *  INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF NON-INFRINGEMENT,
 *  MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 *  IN NO EVENT SHALL AMBARELLA INTERNATIONAL LP OR ITS AFFILIATES BE LIABLE FOR ANY DIRECT,
 *  INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 *  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 *  LOSS OF USE, DATA, OR PROFITS; COMPUTER FAILURE OR MALFUNCTION; OR BUSINESS
 *  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 *  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 *  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *  POSSIBILITY OF SUCH DAMAGE.
 *
 *  @details Control APIs of SONY IMX290_MIPI CMOS sensor with LVDS interface
 *
 */

#ifndef AMBA_SENSOR_IMX290_MIPI_H
#define AMBA_SENSOR_IMX290_MIPI_H

#define IMX290_MIPI_SENSOR_I2C_SLAVE_ADDR       0x34U

/*-----------------------------------------------------------------------------------------------*\
 * Control registers
\*-----------------------------------------------------------------------------------------------*/
#define IMX290_MIPI_NUM_INIT_REG                45U
#define IMX290_MIPI_NUM_MODE_REG                69U

#define IMX290_MIPI_STANDBY         0x3000U
#define IMX290_MIPI_MASTERSTOP      0x3002U
#define IMX290_MIPI_RESET           0x3003U
#define IMX290_MIPI_AGAIN           0x3014U
#define IMX290_MIPI_VMAX_LSB        0x3018U
#define IMX290_MIPI_VMAX_MSB        0x3019U
#define IMX290_MIPI_VMAX_HSB        0x301AU
#define IMX290_MIPI_SHS1_LSB        0x3020U
#define IMX290_MIPI_SHS1_MSB        0x3021U
#define IMX290_MIPI_SHS1_HSB        0x3022U
#define IMX290_MIPI_SHS2_LSB        0x3024U
#define IMX290_MIPI_SHS2_MSB        0x3025U
#define IMX290_MIPI_SHS2_HSB        0x3026U
#define IMX290_MIPI_SHS3_LSB        0x3028U
#define IMX290_MIPI_SHS3_MSB        0x3029U
#define IMX290_MIPI_SHS3_HSB        0x302AU
#define IMX290_MIPI_RHS1_LSB        0x3030U
#define IMX290_MIPI_RHS1_MSB        0x3031U
#define IMX290_MIPI_RHS1_HSB        0x3032U
#define IMX290_MIPI_RHS2_LSB        0x3034U
#define IMX290_MIPI_RHS2_MSB        0x3035U
#define IMX290_MIPI_RHS2_HSB        0x3036U
#define IMX290_MIPI_DOL_FORMAT      0x3045U
#define IMX290_MIPI_DOL_SYNCSIGNAL  0x3106U
#define IMX290_MIPI_DOL_HBFIXEN     0x3107U
#define IMX290_MIPI_NULL0_SIZEV     0x3415U



#define IMX290_MIPI_1920_1080_60P           0U
#define IMX290_MIPI_1920_1080_30P           1U
#define IMX290_MIPI_1920_1080_60P_HDR       2U /* DOL 2-frame 60fps */
#define IMX290_MIPI_1920_1080_30P_HDR_0     3U /* DOL 2-frame 30fps */
#define IMX290_MIPI_1920_1080_30P_HDR_1     4U /* DOL 3-frame 30fps */
#define IMX290_MIPI_1820_400_60P_HDR        5U /* DOL 2-frame 60fps */
#define IMX290_MIPI_1920_500_60P_HDR        6U /* DOL 3-frame 60fps */
#define IMX290_MIPI_1096_736_60P_HDR        7U /* DOL 2-frame 60fps */
#define IMX290_MIPI_1920_1080_50P           8U
#define IMX290_MIPI_1920_1080_25P           9U
#define IMX290_MIPI_1920_1080_50P_HDR      10U /* DOL 2-frame 50fps */
#define IMX290_MIPI_1920_1080_25P_HDR_0    11U /* DOL 2-frame 25fps */
#define IMX290_MIPI_1920_1080_25P_HDR_1    12U /* DOL 3-frame 25fps */
#define IMX290_MIPI_1820_400_50P_HDR       13U /* DOL 2-frame 50fps */
#define IMX290_MIPI_1920_1080_30P1         14U /* 2-lane 30 fps */
#define IMX290_MIPI_1920_1080_60P1         15U /* 2-lane 60 fps */
#define IMX290_MIPI_1920_1080_A30P         16U /* Accurate 30 fps */
#define IMX290_MIPI_1920_1080_A60P         17U /* Accurate 60 fps */
#define IMX290_MIPI_1920_1080_A30P_HDR_0   18U /* DOL 2-frame Accurate 30fps */
#define IMX290_MIPI_1920_1080_A60P_HDR     19U /* DOL 2-frame Accurate 60fps */
#define IMX290_MIPI_NUM_MODE               20U

typedef struct {
    UINT16  Addr;
    UINT8  Data;
} IMX290_MIPI_SEQ_REG_s;

typedef struct {
    UINT16  Addr;
    UINT8  Data[IMX290_MIPI_NUM_MODE];
} IMX290_MIPI_REG_s;

typedef struct {
    UINT32  InputClk;                       /* Sensor side input clock frequency */
    UINT32  LineLengthPck;                  /* XHS period (in input clock cycles) */
    UINT32  FrameLengthLines;               /* horizontal operating period (in number of XHS pulses) */
    AMBA_VIN_FRAME_RATE_s   FrameRate;      /* framerate value of this sensor mode */
} IMX290_MIPI_FRAME_TIMING_s;

typedef struct {
    IMX290_MIPI_FRAME_TIMING_s   FrameTiming;
    FLOAT                   RowTime;        /* Calculated in IMX290_MIPI_Init */
} IMX290_MIPI_MODE_INFO_s;

typedef struct {
    AMBA_SENSOR_STATUS_INFO_s   Status;
    UINT32                      CurrentGainCtrl;
    UINT32                      CurrentShutterCtrl[3];
} IMX290_MIPI_CTRL_s;

/*-----------------------------------------------------------------------------------------------*\
 *  Defined in AmbaSensor_IMX290_MIPI.c
\*-----------------------------------------------------------------------------------------------*/
extern const AMBA_SENSOR_DEVICE_INFO_s IMX290_MIPIDeviceInfo;
extern const AMBA_SENSOR_INPUT_INFO_s IMX290_MIPIInputInfo[IMX290_MIPI_NUM_MODE];
extern const AMBA_SENSOR_OUTPUT_INFO_s IMX290_MIPIOutputInfo[IMX290_MIPI_NUM_MODE];
extern IMX290_MIPI_MODE_INFO_s IMX290_MIPIModeInfoList[IMX290_MIPI_NUM_MODE];
extern const AMBA_SENSOR_HDR_INFO_s IMX290_MIPIHdrInfo[IMX290_MIPI_NUM_MODE];

extern IMX290_MIPI_SEQ_REG_s IMX290_MIPIInitRegTable[IMX290_MIPI_NUM_INIT_REG];
extern IMX290_MIPI_REG_s IMX290_MIPIRegTable[IMX290_MIPI_NUM_MODE_REG];

extern AMBA_SENSOR_OBJ_s AmbaSensor_IMX290_MIPIObj;

#endif /* AMBA_SENSOR_IMX290_MIPI_H */
