/**
 *  @file AmbaSensor_IMX490.h
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
 *  @details Control APIs of SONY IMX490 CMOS sensor with LVDS interface
 *
 */

#ifndef AMBA_SENSOR_IMX490_H
#define AMBA_SENSOR_IMX490_H

#define IMX490_SENSOR_I2C_SLAVE_ADDR       0x3AU

/*-----------------------------------------------------------------------------------------------*\
 * Control registers
\*-----------------------------------------------------------------------------------------------*/
#define IMX490_NUM_MODE_NML_REG           923U
#define IMX490_NUM_MODE_REG_HDR30P       1932U
#define IMX490_NUM_MODE_REG_HDR60P       1330U
#define IMX490_NUM_REG                   1534U
#define IMX490_NUM_CALIB                 1561U


#define IMX490_STANDBY         0x3C00U

#define IMX490_AGAIN_SP1H_LSB  0x36C8U
#define IMX490_AGAIN_SP1H_HSB  0x36C9U

#define IMX490_AGAIN_SP1L_LSB  0x36CAU
#define IMX490_AGAIN_SP1L_HSB  0x36CBU

#define IMX490_AGAIN_SP2H_LSB  0x36CCU
#define IMX490_AGAIN_SP2H_HSB  0x36CDU

#define IMX490_AGAIN_SP2L_LSB  0x36CEU
#define IMX490_AGAIN_SP2L_HSB  0x36CFU

#define IMX490_DGAIN_SP1H_LSB  0x36D0U
#define IMX490_DGAIN_SP1H_HSB  0x36D1U
#define IMX490_DGAIN_SP1L_LSB  0x36D2U
#define IMX490_DGAIN_SP1L_HSB  0x36D3U

#define IMX490_WBGAIN_R_LSB    0x3730U
#define IMX490_WBGAIN_R_HSB    0x3731U
#define IMX490_WBGAIN_GR_LSB   0x3732U
#define IMX490_WBGAIN_GR_HSB   0x3733U
#define IMX490_WBGAIN_GB_LSB   0x3734U
#define IMX490_WBGAIN_GB_HSB   0x3735U
#define IMX490_WBGAIN_B_LSB    0x3736U
#define IMX490_WBGAIN_B_HSB    0x3737U

#define IMX490_VMAX_LSB        0x0000U
#define IMX490_VMAX_MSB        0x0001U
#define IMX490_VMAX_HSB        0x0002U

#define IMX490_SHS1_LSB        0x36C0U
#define IMX490_SHS1_MSB        0x36C1U
#define IMX490_SHS1_HSB        0x36C2U
#define IMX490_SHS2_LSB        0x36C4U
#define IMX490_SHS2_MSB        0x36C5U
#define IMX490_SHS2_HSB        0x36C6U

#define IMX490_MAX_AGAIN                 28.5f /* in dB (TBD) */
#define IMX490_MAX_DGAIN                 42.0f
#define IMX490_MAX_TOTAL_GAIN           (IMX490_MAX_AGAIN + IMX490_MAX_DGAIN)



#define IMX490_2880_1860_A30P               0U
#define IMX490_2880_1860_A30P_HDR           1U
#define IMX490_2880_1192_A60P_HDR           2U

#define IMX490_NUM_MODE                     3U


typedef struct {
    UINT16  Addr;
    UINT8  Data;
} IMX490_SEQ_REG_s;

typedef struct {
    UINT16  Addr;
    UINT8  Data[IMX490_NUM_MODE];
} IMX490_REG_s;

typedef struct {
    UINT32  InputClk;                       /* Sensor side input clock frequency */
    UINT32  LineLengthPck;                  /* XHS period (in input clock cycles) */
    UINT32  FrameLengthLines;               /* horizontal operating period (in number of XHS pulses) */
    AMBA_VIN_FRAME_RATE_s   FrameRate;      /* framerate value of this sensor mode */
} IMX490_FRAME_TIMING_s;

typedef struct {
    IMX490_FRAME_TIMING_s   FrameTiming;
    FLOAT                   RowTime;        /* Calculated in IMX490_Init */
} IMX490_MODE_INFO_s;

typedef struct {
    AMBA_SENSOR_STATUS_INFO_s   Status;
    UINT32                      CurrentGainCtrl[4];
    UINT32                      DgcCtrl;
    UINT32                      CurrentShutterCtrl[4];
    AMBA_SENSOR_WB_CTRL_s       WbCtrl[4];
} IMX490_CTRL_s;

/*-----------------------------------------------------------------------------------------------*\
 *  Defined in AmbaSensor_IMX490.c
\*-----------------------------------------------------------------------------------------------*/
extern IMX490_SEQ_REG_s IMX490_RegTable_NML[IMX490_NUM_MODE_NML_REG];
extern IMX490_SEQ_REG_s IMX490_RegTable_HDR30P[IMX490_NUM_MODE_REG_HDR30P];
extern IMX490_SEQ_REG_s IMX490_RegTable_HDR60P[IMX490_NUM_MODE_REG_HDR60P];

extern IMX490_REG_s IMX490_RegTable[IMX490_NUM_REG];
extern IMX490_REG_s IMX490_CalibTable[IMX490_NUM_CALIB];

//----------------------------------------------------------------
extern const AMBA_SENSOR_DEVICE_INFO_s IMX490_DeviceInfo;
extern const AMBA_SENSOR_INPUT_INFO_s IMX490_InputInfo[IMX490_NUM_MODE];
extern const AMBA_SENSOR_OUTPUT_INFO_s IMX490_OutputInfo[IMX490_NUM_MODE];
extern IMX490_MODE_INFO_s IMX490_ModeInfoList[IMX490_NUM_MODE];
extern const AMBA_SENSOR_HDR_INFO_s IMX490_HdrInfo[IMX490_NUM_MODE];

extern AMBA_SENSOR_OBJ_s AmbaSensor_IMX490Obj;

#endif /* AMBA_SENSOR_IMX490_H */
