/**
 *  @file AmbaSensor_IMX383.h
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
 *  @details Control APIs of SONY IMX383 CMOS sensor with SLVS-EC interface
 *
 */

#ifndef AMBA_SENSOR_IMX383_H
#define AMBA_SENSOR_IMX383_H

/*-----------------------------------------------------------------------------------------------*\
 * Control registers
\*-----------------------------------------------------------------------------------------------*/
#define IMX383_NUM_PLSTMG_REG          203U
#define IMX383_NUM_READOUT_MODE_REG    252U
#define IMX383_NUM_ATTR_REG            21U

#define IMX383_OP_REG                  0x0000U
#define IMX383_STBPL_REG               0x04EBU

#define IMX383_5472_3648_30P      0U    /* Type 1 Approx. 20.48 M pixels (3:2), Mode 1 */
#define IMX383_5472_3648_60P      1U    /* Type 1 Approx. 20.48 M pixels (3:2), Mode 1N */
#define IMX383_4096_2160_60P      2U    /* Type 1/1.4 Approx. 9.03 M pixels (17:9), Mode 1 */
#define IMX383_1880_934_30P       3U    /* Type 1 Approx. 20.48 M pixels (3:2), Mode 7 */
#define IMX383_2736_1538_60P      4U    /* Type 1/1.06 Approx. 17.25 M pixels (16:9), Mode 9 */
#define IMX383_2736_1538_120P     5U    /* Type 1/1.06 Approx. 17.25 M pixels (16:9), Mode 2 */
#define IMX383_4096_2160_30P      6U    /* Type 1/1.4 Approx. 9.03 M pixels (17:9), Mode 1 */
#define IMX383_NUM_MODE           7U

typedef struct {
    UINT16  Addr;
    UINT8   Data;
} IMX383_REG_s;

typedef struct {
    UINT16  Addr;
    UINT8   Data[IMX383_NUM_MODE];
} IMX383_MODE_REG_s;

typedef struct {
    UINT32  InputClk;                       /* Sensor side input clock frequency */
    UINT32  NumTickPerXhs;                  /* XHS period (in input clock cycles) */
    DOUBLE  NumXhsPerH;                     /* horizontal operating period (in number of XHS pulses) */
    UINT32  NumXhsPerV;                     /* vertical operating period (in number of XHS pulses) */
    UINT32  NumXvsPerV;                     /* vertical operating period (in number of XVS pulses) */
    AMBA_VIN_FRAME_RATE_s   FrameRate;      /* framerate value of this sensor mode */
} IMX383_FRAME_TIMING_s;

typedef struct {
    UINT8                   DualLink;
    IMX383_FRAME_TIMING_s   FrameTiming;
    FLOAT                   RowTime;        /* Calculated in IMX383_Init */
} IMX383_MODE_INFO_s;

typedef struct {
    AMBA_SENSOR_STATUS_INFO_s   Status;
    UINT32                      CurrentAgcCtrl[2];
    UINT32                      CurrentDgcCtrl[2];
    UINT32                      CurrentShrCtrl[2];
} IMX383_CTRL_s;

/*-----------------------------------------------------------------------------------------------*\
 *  Defined in AmbaSensor_IMX383.c
\*-----------------------------------------------------------------------------------------------*/
extern const AMBA_SENSOR_DEVICE_INFO_s IMX383DeviceInfo;
extern const AMBA_SENSOR_INPUT_INFO_s IMX383InputInfo[IMX383_NUM_MODE];
extern const AMBA_SENSOR_OUTPUT_INFO_s IMX383OutputInfo[IMX383_NUM_MODE];
extern IMX383_MODE_INFO_s IMX383ModeInfoList[IMX383_NUM_MODE];
extern const AMBA_SENSOR_HDR_INFO_s IMX383HdrInfo[IMX383_NUM_MODE];

extern IMX383_REG_s IMX383PlstmgRegTable[IMX383_NUM_PLSTMG_REG];
extern IMX383_MODE_REG_s IMX383ModeRegTable[IMX383_NUM_READOUT_MODE_REG];
extern IMX383_REG_s IMX383AttrRegTable[IMX383_NUM_ATTR_REG];

extern AMBA_SENSOR_OBJ_s AmbaSensor_IMX383Obj;

#endif /* AMBA_SENSOR_IMX383_H */
