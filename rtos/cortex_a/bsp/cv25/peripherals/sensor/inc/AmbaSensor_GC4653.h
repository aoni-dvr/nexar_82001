/**
 *  @file AmbaSensor_GC4653.h
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
 *  @details Control APIs of OmniVision GC4653 CMOS sensor with MIPI interface
 *
 */

#ifndef _AMBA_SENSOR_GC4653_H_
#define _AMBA_SENSOR_GC4653_H_

#define NA 0xffff

/*-----------------------------------------------------------------------------------------------*\
 * Control registers
\*-----------------------------------------------------------------------------------------------*/
#define GC4653_NUM_READOUT_MODE_REG        143
#define GC4653_NUM_AGC_STEP                26

typedef enum _GC4653_READOUT_MODE_e_ {
    GC4653_2560_1440_30P = 0,

    GC4653_NUM_READOUT_MODE,
} GC4653_READOUT_MODE_e;

typedef enum _AMBA_SENSOR_GC4653_MODE_ID_e_ {
    AMBA_SENSOR_GC4653_2560_1440_30P = 0,


    AMBA_SENSOR_GC4653_NUM_MODE,
} AMBA_SENSOR_GC4653_MODE_ID_e;

typedef struct _GC4653_REG_s_ {
    UINT16  Addr;
    UINT8  Data[GC4653_NUM_READOUT_MODE];
} GC4653_REG_s;

typedef struct _GC4653_FRAME_TIMING_s_ {
    UINT32  InputClk;                       /* Sensor side input clock frequency */
    UINT32  Linelengthpck;                  /* Number of pixel clock cycles per line of frame */
    UINT32  FrameLengthLines;               /* Number of lines per frame */
    AMBA_VIN_FRAME_RATE_s   FrameRate;      /* framerate value of this sensor mode */
} GC4653_FRAME_TIMING_s;

typedef struct _GC4653_MODE_INFO_s_ {
    GC4653_READOUT_MODE_e      ReadoutMode;
    GC4653_FRAME_TIMING_s      FrameTiming;
} GC4653_MODE_INFO_s;

typedef struct _GC4653_CTRL_s_ {
    AMBA_SENSOR_STATUS_INFO_s   Status;
    AMBA_SENSOR_MODE_INFO_s     ModeInfo;
    UINT32                      CurrentAgcCtrl;
    UINT32                      CurrentDgcCtrl;
    UINT32                      CurrentShutterCtrl;
} GC4653_CTRL_s;

/*-----------------------------------------------------------------------------------------------*\
 *  Defined in AmbaSensor_GC4653.c
\*-----------------------------------------------------------------------------------------------*/
extern const AMBA_SENSOR_DEVICE_INFO_s GC4653DeviceInfo;
extern const AMBA_SENSOR_INPUT_INFO_s GC4653InputInfo[GC4653_NUM_READOUT_MODE_REG];
extern const AMBA_SENSOR_OUTPUT_INFO_s GC4653OutputInfo[GC4653_NUM_READOUT_MODE_REG];
extern GC4653_REG_s GC4653RegTable[GC4653_NUM_READOUT_MODE_REG];
extern GC4653_MODE_INFO_s GC4653ModeInfoList[AMBA_SENSOR_GC4653_NUM_MODE];
extern UINT8 Gc4653RegValTable[GC4653_NUM_AGC_STEP][7];
extern UINT32 Gc4653GainLevelTable[GC4653_NUM_AGC_STEP+1];

extern AMBA_SENSOR_OBJ_s AmbaSensor_GC4653Obj;

#endif /* _AMBA_SENSOR_GC4653_H_ */
