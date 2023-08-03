/**
 *  @file AmbaSensor_IMX455.h
 *
 *  @copyright Copyright (c) 2021 Ambarella, Inc.
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
 *  @details Control APIs of SONY IMX455 CMOS sensor with SLVS-EC interface
 *
 */

#ifndef AMBA_SENSOR_IMX455_H
#define AMBA_SENSOR_IMX455_H

/*-----------------------------------------------------------------------------------------------*\
 * Control registers
\*-----------------------------------------------------------------------------------------------*/
#define IMX455_NUM_INIT_REG         171U
#define IMX455_NUM_ATTR_REG         21U

#define IMX455_STANDBY_WAKEUP_REG   0x0000U /* STANDBY and WAKEUP */
#define IMX455_MODE_REG             0x0001U
#define IMX455_PWR_CTRL_REG         0x019EU /* PWR_AUTO and SLP_CTRL */
#define IMX455_PLL_AD_SETTING_REG   0x0366U

#define IMX455_SHR_L                0x0016U
#define IMX455_SHR_H                0x0017U

/* Analog gain setting */
#define IMX455_APGC_N_L             0x002EU
#define IMX455_APGC_N_H             0x002FU
#define IMX455_APGC_S_L             0x0030U
#define IMX455_APGC_S_H             0x0031U
#define IMX455_APGC_ADD             0x002DU
#define IMX455_CLPAPGC              0x004DU

/* Digital gain setting */
#define IMX455_DGAIN                0x003EU

#define IMX455_7696_4320_19P    0U    /* Mode 0A-b: All-pixel readout / 8K 16:9 cropping / HS-AD 14-bit / Max. 19.70fps */
#define IMX455_7696_4320_30P    1U    /* Mode 0A-c: All-pixel readout / 8K 16:9 cropping / HS-AD 12-bit / Max. 30.99fps */
#define IMX455_NUM_MODE         2U

#define IMX455_MAX_APGC         0xEFCU  /* Max. 23.979 dB */
#define IMX455_MAX_DGAIN        6U      /* Max. 36 dB */

typedef struct {
    UINT16  Addr;
    UINT8   Data;
} IMX455_REG_s;

typedef struct {
    UINT16  Addr;
    UINT8   Data[IMX455_NUM_MODE];
} IMX455_INIT_REG_s;

typedef struct {
    UINT32  InputClk;                               /* Sensor side input clock frequency */
    UINT32  DataRate;                               /* output bit/pixel clock frequency from image sensor */
    UINT8   NumDataLanes;                           /* active data channels */
    UINT8   NumDataBits;                            /* pixel data bit depth */
    UINT32  LineLengthPck;                          /* Active pixels + H-blanking */
    UINT32  FrameLengthLines;                       /* Active lines + V-blanking */
    UINT32  OutputWidth;                            /* valid pixels per line */
    UINT32  OutputHeight;                           /* valid lines per frame */
    AMBA_SENSOR_AREA_INFO_s RecordingPixels;        /* maximum recording frame size */
    AMBA_SENSOR_AREA_INFO_s OpticalBlackPixels;     /* user clamp area */
} IMX455_SENSOR_INFO_s;

typedef struct {
    UINT32  InputClk;                       /* Sensor side input clock frequency */
    UINT32  NumTickPerXhs;                  /* XHS period (in input clock cycles) */
    DOUBLE  NumXhsPerH;                     /* horizontal operating period (in number of XHS pulses) */
    UINT32  NumXhsPerV;                     /* vertical operating period (in number of XHS pulses) */
    UINT32  NumXvsPerV;                     /* vertical operating period (in number of XVS pulses) */
    AMBA_VIN_FRAME_RATE_s   FrameRate;      /* framerate value of this sensor mode */
} IMX455_FRAME_TIMING_s;

typedef struct {
    UINT8                   DualLink;
    IMX455_FRAME_TIMING_s   FrameTiming;
    FLOAT                   RowTime;        /* Calculated in IMX455_Init */
} IMX455_MODE_INFO_s;

typedef struct {
    AMBA_SENSOR_STATUS_INFO_s   Status;
    UINT32                      CurrentAgcCtrl[2];
    UINT32                      CurrentDgcCtrl[2];
    UINT32                      CurrentShrCtrl[2];
} IMX455_CTRL_s;

/*-----------------------------------------------------------------------------------------------*\
 *  Defined in AmbaSensor_IMX455.c
\*-----------------------------------------------------------------------------------------------*/
extern const AMBA_SENSOR_DEVICE_INFO_s IMX455DeviceInfo;
extern const AMBA_SENSOR_INPUT_INFO_s IMX455InputInfo[IMX455_NUM_MODE];
extern const AMBA_SENSOR_OUTPUT_INFO_s IMX455OutputInfo[IMX455_NUM_MODE];
extern IMX455_MODE_INFO_s IMX455ModeInfoList[IMX455_NUM_MODE];
extern const AMBA_SENSOR_HDR_INFO_s IMX455HdrInfo[IMX455_NUM_MODE];

extern IMX455_INIT_REG_s IMX455InitRegTable[IMX455_NUM_INIT_REG];
extern IMX455_REG_s IMX455AttrRegTable[IMX455_NUM_ATTR_REG];

extern AMBA_SENSOR_OBJ_s AmbaSensor_IMX455Obj;

#endif /* AMBA_SENSOR_IMX455_H */
