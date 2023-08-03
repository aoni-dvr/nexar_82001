/**
 *  @file AmbaSensor_MAX9295_9296_IMX424_RCCB.h
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
 *  @details Control APIs of MAXIM SerDes plus SONY IMX424 CMOS sensor with MIPI interface
 *
 */

#ifndef AMBA_SENSOR_MAX9295_9296_IMX424_H
#define AMBA_SENSOR_MAX9295_9296_IMX424_H

#define MX00_IMX424_I2C_WR_BUF_SIZE           64U

#define MX00_IMX424_NUM_MAX_SENSOR_COUNT      2U

#define MX00_IMX424_SENSOR_I2C_SLAVE_ADDR     0x3A

#define MX00_IMX424_EXP_RATE_L_M              16
#define MX00_IMX424_EXP_RATE_M_S              16

/* SensorID definition */
#define MX00_IMX424_SENSOR_ID_CHAN_A          (0x00000010U)    /* sensor on MAX9295 connecting with Channel-A of MAX9296 */
#define MX00_IMX424_SENSOR_ID_CHAN_B          (0x00000100U)    /* sensor on MAX9295 connecting with Channel-B of MAX9296 */

/*-----------------------------------------------------------------------------------------------*\
 * Control registers
\*-----------------------------------------------------------------------------------------------*/
#define MX00_IMX424_NUM_REGTABLE_REG          599U

#define MX00_IMX424_STANDBY                   0x0000U

#define MX00_IMX424_INTGL_COARSE              0x0014U
#define MX00_IMX424_INTGM_COARSE              0x0018U
#define MX00_IMX424_INTGS_COARSE              0x001CU

#define MX00_IMX424_INTGL_FINE                0x0020U
#define MX00_IMX424_INTGM_FINE                0x0022U
#define MX00_IMX424_INTGS_FINE                0x0024U

#define MX00_IMX424_AGAIN                     0x0026U

#define MX00_IMX424_WBGAIN_CF0                0x002CU
#define MX00_IMX424_WBGAIN_CF1                0x002EU  /* B */
#define MX00_IMX424_WBGAIN_CF2                0x0030U  /* R */
#define MX00_IMX424_WBGAIN_CF3                0x0032U

/*-----------------------------------------------------------------------------------------------*\
 *  Application Mode Definition
\*-----------------------------------------------------------------------------------------------*/
#define MX00_IMX424_3840_1920_30P_HDR    0U  /* PWL12 3-frame 30fps */
#define MX00_IMX424_3840_1920_A30P_HDR   0U  /* PWL12 3-frame 30fps */
#define MX00_IMX424_NUM_MODE             1U

typedef struct {
    UINT16 Addr;
    UINT8  Data[MX00_IMX424_NUM_MODE];
} MX00_IMX424_MODE_REG_s;

typedef struct {
    UINT32                       InputClk;           /* Sensor side input clock frequency */
    UINT32                       DataRate;           /* Output bit/pixel clock frequency from image sensor */
    UINT8                        NumDataLanes;       /* Active data channels */
    UINT8                        NumDataBits;        /* Pixel data bit depth */
    UINT32                       LineLengthPck;      /* Active pixels + H-blanking */
    UINT32                       FrameLengthLines;   /* Number of output lines incluidng blanking */
    UINT32                       VMAX;               /* Vertical period (in unts of Coarse Shutter) */
    UINT32                       OutputWidth;        /* Valid pixels per line */
    UINT32                       OutputHeight;       /* Valid lines per frame */
    AMBA_SENSOR_AREA_INFO_s      RecordingPixels;    /* Maximum recording frame size */
    AMBA_SENSOR_AREA_INFO_s      OpticalBlackPixels; /* User clamp area */
} MX00_IMX424_SENSOR_INFO_s;

typedef struct {
    AMBA_VIN_FRAME_RATE_s        FrameRate;          /* Framerate value of this sensor mode */
    FLOAT                        RowTime;            /* Calculated in MX00_IMX424_Init */
    FLOAT                        PixelRate;          /* Calculated in MX00_IMX424_Init */
} MX00_IMX424_MODE_INFO_s;

typedef struct {
    UINT32                       AgcCtrl;
    AMBA_SENSOR_WB_CTRL_s        WbCtrl;
    UINT32                       ShutterCtrl[3];     /* [0]: Long exposure, [1]: Medium exposure, [2]: Short Exposure */
} MX00_IMX424_AE_INFO_s;

typedef struct {
    AMBA_SENSOR_STATUS_INFO_s    Status;
    MX00_IMX424_AE_INFO_s       CurrentAEInfo[MX00_IMX424_NUM_MAX_SENSOR_COUNT];
} MX00_IMX424_CTRL_s;

typedef struct {
    UINT32  Coarse:       17;
    UINT32  Reserved:     3;
    UINT32  Fine:         12;
} MX00_IMX424_INTEG_REG_s;

/*-----------------------------------------------------------------------------------------------*\
 *  Defined in AmbaSensor_MAX9295_9296_IMX424.c
\*-----------------------------------------------------------------------------------------------*/
extern const AMBA_SENSOR_DEVICE_INFO_s MX00_IMX424DeviceInfo;
extern const AMBA_SENSOR_INPUT_INFO_s MX00_IMX424InputInfo[MX00_IMX424_NUM_MODE];
extern const AMBA_SENSOR_OUTPUT_INFO_s MX00_IMX424OutputInfo[MX00_IMX424_NUM_MODE];
extern const AMBA_SENSOR_HDR_INFO_s MX00_IMX424HdrInfo[MX00_IMX424_NUM_MODE];
extern const MX00_IMX424_SENSOR_INFO_s MX00_IMX424SensorInfo[MX00_IMX424_NUM_MODE];
extern const MX00_IMX424_MODE_REG_s MX00_IMX424RegTable[MX00_IMX424_NUM_REGTABLE_REG];
extern MX00_IMX424_MODE_INFO_s MX00_IMX424ModeInfoList[MX00_IMX424_NUM_MODE];

extern AMBA_SENSOR_OBJ_s AmbaSensor_MX00_IMX424Obj;

#endif /* AMBA_SENSOR_MAX9295_9296_IMX424_H */
