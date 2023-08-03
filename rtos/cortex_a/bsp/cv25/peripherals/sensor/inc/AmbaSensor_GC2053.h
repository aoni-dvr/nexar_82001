/**
 *  @file AmbaSensor_GC2053.h
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
 *  @details Control APIs of OmniVision GC2053 CMOS sensor with MIPI interface
 *
 */

#ifndef _AMBA_SENSOR_GC2053_H_
#define _AMBA_SENSOR_GC2053_H_

#define NA 0xffff

/*-----------------------------------------------------------------------------------------------*\
 * Control registers
\*-----------------------------------------------------------------------------------------------*/
#define GC2053_NUM_READOUT_MODE_REG        135
#define GC2053_NUM_AGC_STEP                29

typedef enum _GC2053_READOUT_MODE_e_ {
    GC2053_1920_1080_30P = 0,
	GC2053_1920_1080_25P,

    GC2053_NUM_READOUT_MODE,
} GC2053_READOUT_MODE_e;

typedef enum _AMBA_SENSOR_GC2053_MODE_ID_e_ {
    AMBA_SENSOR_GC2053_1920_1080_30P = 0,
	AMBA_SENSOR_GC2053_1920_1080_25P,

    AMBA_SENSOR_GC2053_NUM_MODE,
} AMBA_SENSOR_GC2053_MODE_ID_e;

typedef struct _GC2053_REG_s_ {
    UINT8  Addr;
    UINT8  Data[GC2053_NUM_READOUT_MODE];
} GC2053_REG_s;

typedef struct _GC2053_FRAME_TIMING_s_ {
    UINT32  InputClk;                       /* Sensor side input clock frequency */
    UINT32  Linelengthpck;                  /* Number of pixel clock cycles per line of frame */
    UINT32  FrameLengthLines;               /* Number of lines per frame */
    AMBA_VIN_FRAME_RATE_s   FrameRate;      /* framerate value of this sensor mode */
} GC2053_FRAME_TIMING_s;

typedef struct _GC2053_MODE_INFO_s_ {
    GC2053_READOUT_MODE_e      ReadoutMode;
    GC2053_FRAME_TIMING_s      FrameTiming;
} GC2053_MODE_INFO_s;

typedef struct _GC2053_CTRL_s_ {
    AMBA_SENSOR_STATUS_INFO_s   Status;
    AMBA_SENSOR_MODE_INFO_s     ModeInfo;
    UINT32                      CurrentAgcCtrl;
    UINT32                      CurrentDgcCtrl;
    UINT32                      CurrentShutterCtrl;
} GC2053_CTRL_s;

/*-----------------------------------------------------------------------------------------------*\
 *  Defined in AmbaSensor_GC2053.c
\*-----------------------------------------------------------------------------------------------*/
extern const AMBA_SENSOR_DEVICE_INFO_s GC2053DeviceInfo;
extern const AMBA_SENSOR_INPUT_INFO_s GC2053InputInfo[GC2053_NUM_READOUT_MODE_REG];
extern const AMBA_SENSOR_OUTPUT_INFO_s GC2053OutputInfo[GC2053_NUM_READOUT_MODE_REG];
extern GC2053_REG_s GC2053RegTable[GC2053_NUM_READOUT_MODE_REG];
extern GC2053_MODE_INFO_s GC2053ModeInfoList[AMBA_SENSOR_GC2053_NUM_MODE];
extern UINT8 Gc2053RegValTable[GC2053_NUM_AGC_STEP][4];
extern UINT32 Gc2053GainLevelTable[GC2053_NUM_AGC_STEP+1];

extern AMBA_SENSOR_OBJ_s AmbaSensor_GC2053Obj;

#endif /* _AMBA_SENSOR_GC2053_H_ */
