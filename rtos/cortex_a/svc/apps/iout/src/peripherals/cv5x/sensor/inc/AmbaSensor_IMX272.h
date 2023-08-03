/**
 *  @file AmbaSensor_IMX272.h
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
 *  @details Control APIs of SONY IMX272 CMOS sensor with SLVS-EC interface
 *
 */

#ifndef AMBA_SENSOR_IMX272_H
#define AMBA_SENSOR_IMX272_H

/*-----------------------------------------------------------------------------------------------*\
 * Control registers
\*-----------------------------------------------------------------------------------------------*/
#define IMX272_NUM_READOUT_MODE_REG    116U
#define IMX272_NUM_ATTR_REG            21U

#define IMX272_OP_REG                  0x0200U
#define IMX272_STBCP_REG               0x0270U
#define IMX272_STBPL_REG               0x026DU
#define IMX272_STNBY_GL_X_REG          0x026FU

#define IMX272_5280_3956_60P      0U    /* Mode No.0: All-pixel (12 bits) */
#define IMX272_5280_3956_30P      1U    /* Mode No.1: All-pixel low noise (12 bits) */
#define IMX272_5280_3044_60P      2U    /* Mode No.4: All-pixel (12 bits, 16:9) */
#define IMX272_5280_3044_30P      3U    /* Mode No.5: All-pixel low noise (12 bits, wide) */
#define IMX272_4128_2264_60P      4U    /* Mode No.9: 4 K movie 4128 x 2264 cropping */
#define IMX272_2640_1522_60P      5U    /* Mode No.10: HD 1080: Vertical/horizontal 2/2 binning mode */
#define IMX272_2640_1522_120P     6U    /* Mode No.11: HD 1080:: Vertical 1/2 subsampling horizontal 2/2 binning mode */
#define IMX272_1760_1018_60P      7U    /* Mode No.12: HD 720 60p: Vertical 2/3 subsampling binning horizontal 3/3 binning mode */
#define IMX272_NUM_MODE           8U

typedef struct {
    UINT16  Addr;
    UINT8   Data;
} IMX272_REG_s;

typedef struct {
    UINT16  Addr;                           /* high byte: Chip ID, low byte: Start address */
    UINT8   Data[IMX272_NUM_MODE];
} IMX272_MODE_REG_s;

typedef struct {
    UINT32  InputClk;                       /* Sensor side input clock frequency */
    UINT32  NumTickPerXhs;                  /* XHS period (in input clock cycles) */
    DOUBLE  NumXhsPerH;                     /* horizontal operating period (in number of XHS pulses) */
    UINT32  NumXhsPerV;                     /* vertical operating period (in number of XHS pulses) */
    UINT32  NumXvsPerV;                     /* vertical operating period (in number of XVS pulses) */
    AMBA_VIN_FRAME_RATE_s   FrameRate;      /* framerate value of this sensor mode */
} IMX272_FRAME_TIMING_s;

typedef struct {
    UINT8                   DualLink;
    IMX272_FRAME_TIMING_s   FrameTiming;
    FLOAT                   RowTime;        /* Calculated in IMX272_Init */
} IMX272_MODE_INFO_s;

typedef struct {
    AMBA_SENSOR_STATUS_INFO_s   Status;
    UINT32                      CurrentAgcCtrl[2];
    UINT32                      CurrentDgcCtrl[2];
    UINT32                      CurrentShrCtrl[2];
} IMX272_CTRL_s;

/*-----------------------------------------------------------------------------------------------*\
 *  Defined in AmbaSensor_IMX272.c
\*-----------------------------------------------------------------------------------------------*/
extern const AMBA_SENSOR_DEVICE_INFO_s IMX272DeviceInfo;
extern const AMBA_SENSOR_INPUT_INFO_s IMX272InputInfo[IMX272_NUM_MODE];
extern const AMBA_SENSOR_OUTPUT_INFO_s IMX272OutputInfo[IMX272_NUM_MODE];
extern IMX272_MODE_INFO_s IMX272ModeInfoList[IMX272_NUM_MODE];
extern const AMBA_SENSOR_HDR_INFO_s IMX272HdrInfo[IMX272_NUM_MODE];

extern IMX272_MODE_REG_s IMX272ModeRegTable[IMX272_NUM_READOUT_MODE_REG];
extern IMX272_REG_s IMX272AttrRegTable[IMX272_NUM_ATTR_REG];

extern AMBA_SENSOR_OBJ_s AmbaSensor_IMX272Obj;

#endif /* AMBA_SENSOR_IMX272_H */
