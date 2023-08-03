/**
 *  @file AmbaSensor_MAX9295_9296_VG1762.h
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
 *  @details Control APIs of MAX9295_9296 plus ST VG1762 CMOS sensor with MIPI interface
 *
 */

#ifndef MAX9295_9296_VG1762_H
#define MAX9295_9296_VG1762_H

#define MX00_VG1762_NUM_MAX_SENSOR_COUNT 2U


#define MX00_VG1762_I2C_SLAVE_ADDRESS    0x20U
#define MX00_VG1762_SENSOR_I2C_MAX_SIZE  64U

#define MX00_VG1762_MAX_AGAIN                 4.0f /* gain 2X (TBD) */
#define MX00_VG1762_MAX_DGAIN                 1.0f
#define MX00_VG1762_MAX_TOTAL_GAIN           (MX00_VG1762_MAX_AGAIN * MX00_VG1762_MAX_DGAIN)
#define MX00_VG1762_MAX_CONTX_NUM             4U
/* SensorID definition */
#define MX00_VG1762_SENSOR_ID_CHAN_A   (0x00000010U) /* sensor on MAX9295 connecting with Channel-A of MAX9296 */
#define MX00_VG1762_SENSOR_ID_CHAN_B   (0x00000100U) /* sensor on MAX9295 connecting with Channel-B of MAX9296 */

/*-----------------------------------------------------------------------------------------------*\
 * Control registers
\*-----------------------------------------------------------------------------------------------*/

#define MX00_VG1762_STANDBY                     0x0202U
///////
#define MX00_VG1762_AGAIN                       0x0508U
#define MX00_VG1762_DGAIN_LONG                  0x050AU
#define MX00_VG1762_DGAIN_SHORT                 0x0512U
#define MX00_VG1762_COARSE_EXPOSURE_LONG        0x0500U
#define MX00_VG1762_COARSE_EXPOSURE_SHORT       0x0504U
/////CONTX BASE
#define MX00_VG1762_CONTX0_BASE                 0x0500U
#define MX00_VG1762_CONTX1_BASE                 0x0550U
#define MX00_VG1762_CONTX2_BASE                 0x05A0U
#define MX00_VG1762_CONTX3_BASE                 0x05F0U
/////Reg OFFSET
#define MX00_VG1762_READOUT_CTRL_OFFSET         0x0030U
#define MX00_VG1762_Y_START_OFFSET              0x001cU
#define MX00_VG1762_Y_START_OFFSET1             0x001dU
#define MX00_VG1762_Y_END_OFFSET                0x001eU
#define MX00_VG1762_Y_END_OFFSET1               0x001fU
#define MX00_VG1762_HDR_CTRL_OFFSET             0x0032U
#define MX00_VG1762_FL_OFFSET                   0x001aU
#define MX00_VG1762_FL_OFFSET1                  0x001bU
#define MX00_VG1762_COARSE_EXPL_OFFSET          0x0000U
#define MX00_VG1762_COARSE_EXPL_OFFSET1         0x0001U
#define MX00_VG1762_COARSE_EXPS_OFFSET          0x0004U
#define MX00_VG1762_COARSE_EXPS_OFFSET1         0x0005U
#define MX00_VG1762_MERGE_CTRL_OFFSET           0x0034U

#define MX00_VG1762_AGAIN_OFFSET                0x0008U
#define MX00_VG1762_DGAIN_LONG_OFFSET           0x000AU
#define MX00_VG1762_DGAIN_SHORT_OFFSET          0x0012U

#define MX00_VG1762_CONTX_RPTCNT_OFFSET         0x004AU
#define MX00_VG1762_NEXT_CONTX_OFFSET           0x004CU

#define MX00_VG1762_SIGNALS_CTRL_OFFSET         0x0022U



/*-----------------------------------------------------------------------------------------------*\
 *  Application Mode Definition
\*-----------------------------------------------------------------------------------------------*/
#define MX00_VG1762_1920_1080_SHDR_30P          0U
#define MX00_VG1762_1920_1080_SHDR_60P          1U
#define MX00_VG1762_1920_1080_30P               2U
#define MX00_VG1762_1920_1080_SHDR_30P_1        3U
#define MX00_VG1762_NUM_MODE                    4U

typedef struct {
    UINT32  InputClk;                                    /* Sensor side input clock frequency */
    UINT32  DataRate;                                    /* output bit/pixel clock frequency from image sensor */
    UINT8   NumDataLanes;                                /* active data channels */
    UINT8   NumDataBits;                                 /* pixel data bit depth */
    UINT32  LineLengthPck;                               /* Active pixels + H-blanking */
    UINT32  FrameLengthLines;                            /* Active lines + V-blanking */
    UINT32  OutputWidth;                                 /* valid pixels per line */
    UINT32  OutputHeight;                                /* valid lines per frame */
    AMBA_SENSOR_AREA_INFO_s RecordingPixels;             /* maximum recording frame size */
    AMBA_SENSOR_AREA_INFO_s OpticalBlackPixels;          /* user clamp area */
} MX00_VG1762_SENSOR_INFO_s;


typedef struct {
    UINT32  InputClk;                                  /* Sensor side input clock frequency */
    UINT32  HMAX;                                      /* HMAX */
    UINT32  VMAX;                                      /* VMAX */
    AMBA_VIN_FRAME_RATE_s  FrameRate;                  /* Frame rate value of this sensor mode */
} MX00_VG1762_FRAME_TIMING_s;

typedef struct {
    MX00_VG1762_FRAME_TIMING_s       FrameTime;
    FLOAT                             RowTime;    /* Calculated in MX00_VG1762_Init */
    FLOAT                             PixelRate;  /* Calculated in MX00_VG1762_Init */
} MX00_VG1762_MODE_INFO_s;

typedef struct {
    AMBA_SENSOR_STATUS_INFO_s  Status;
    UINT32                     CurrentAgcCtrl[MX00_VG1762_NUM_MAX_SENSOR_COUNT][3];      /* [0]: LONG, [1]: MEDIUM, [2]: SHORT */
    UINT32                     CurrentDgcCtrl[MX00_VG1762_NUM_MAX_SENSOR_COUNT][3];      /* [0]: LONG, [1]: MEDIUM, [2]: SHORT */
    AMBA_SENSOR_WB_CTRL_s      CurrentWbCtrl[MX00_VG1762_NUM_MAX_SENSOR_COUNT][3];       /* [0]: LONG, [1]: MEDIUM, [2]: SHORT */
    UINT32                     CurrentShutterCtrl[MX00_VG1762_NUM_MAX_SENSOR_COUNT][3];  /* [0]: LONG, [1]: MEDIUM, [2]: SHORT */
} MX00_VG1762_CTRL_s;

typedef struct  {
    UINT16  Addr;
    UINT8   Data[16];
    UINT32  DataSize;
} MX00_VG1762_SEQ_REG_s;

typedef struct  {
    UINT16  Addr;
    UINT8   Data[MX00_VG1762_NUM_MODE];
} MX00_VG1762_REG_s;

/*-----------------------------------------------------------------------------------------------*\
 *  Defined in AmbaSensor_MX00_VG1762.c
\*-----------------------------------------------------------------------------------------------*/
#define MX00_VG1762_NUM_MODE_REG             51U
#define MX00_VG1762_NUM_LUT16T12             16U
#define MX00_VG1762_NUM_CONTX_REG            17U//13U

extern MX00_VG1762_SEQ_REG_s MX00_VG1762_RegTable[MX00_VG1762_NUM_MODE_REG];
extern MX00_VG1762_SEQ_REG_s MX00_VG1762Lut16to12Table[MX00_VG1762_NUM_LUT16T12];
extern MX00_VG1762_REG_s MX00_VG1762ContxTable[MX00_VG1762_NUM_CONTX_REG];

extern const AMBA_SENSOR_DEVICE_INFO_s MX00_VG1762_DeviceInfo;
extern const MX00_VG1762_SENSOR_INFO_s MX00_VG1762_SensorInfo[MX00_VG1762_NUM_MODE];
extern const AMBA_SENSOR_INPUT_INFO_s MX00_VG1762_InputInfo[MX00_VG1762_NUM_MODE];
extern const AMBA_SENSOR_OUTPUT_INFO_s MX00_VG1762_OutputInfo[MX00_VG1762_NUM_MODE];
extern const AMBA_SENSOR_HDR_INFO_s MX00_VG1762_HdrInfo[MX00_VG1762_NUM_MODE];
extern MX00_VG1762_MODE_INFO_s MX00_VG1762ModeInfoList[MX00_VG1762_NUM_MODE];
extern AMBA_SENSOR_OBJ_s AmbaSensor_MX00_VG1762Obj;

#endif /* MAX9295_9296_VG1762_H */
