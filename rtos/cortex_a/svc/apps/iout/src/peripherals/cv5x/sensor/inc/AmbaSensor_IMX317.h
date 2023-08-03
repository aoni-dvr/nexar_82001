/**
 *  @file AmbaSensor_IMX317.h
 *
 *  @copyright Copyright (c) 2020 Ambarella, Inc.
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
 *  @details Control APIs of SONY IMX317 CMOS sensor with LVDS interface
 *
 */

#ifndef AMBA_SENSOR_IMX317_H
#define AMBA_SENSOR_IMX317_H

/*-----------------------------------------------------------------------------------------------*\
 * Control registers
\*-----------------------------------------------------------------------------------------------*/
#define IMX317_NUM_READOUT_PSTMG_REG   25U
#define IMX317_NUM_READOUT_MODE_REG    29U

#define IMX317_OP_REG                  0x0000U
#define IMX317_CLPSQRST_REG            0x0001U
#define IMX317_DCKRST_REG              0x0009U
#define IMX317_PGC_LSB_REG             0x000AU
#define IMX317_PGC_MSB_REG             0x000BU
#define IMX317_SHR_LSB_REG             0x000CU
#define IMX317_SHR_MSB_REG             0x000DU
#define IMX317_SVR_LSB_REG             0x000EU
#define IMX317_SVR_MSB_REG             0x000FU
#define IMX317_DGAIN_REG               0x0012U
#define IMX317_MDVREV_REG              0x001AU
#define IMX317_SYS_MODE_REG            0x003EU
#define IMX317_PSMOVEN_REG             0x00EEU
#define IMX317_PLL_CKEN_REG            0x00E6U
#define IMX317_PACKEN_REG              0x00E8U
#define IMX317_STBPL_IF_AD_REG         0x010BU
#define IMX317_PLRD1_LSB_REG           0x0120U
#define IMX317_PLRD1_MSB_REG           0x0121U
#define IMX317_PLRD2_REG               0x0122U
#define IMX317_PLRD3_REG               0x0129U
#define IMX317_PLRD4_REG               0x012AU
#define IMX317_PLRD5_REG               0x012DU
#define IMX317_PSLVDS1_LSB_REG         0x032CU
#define IMX317_PSLVDS1_MSB_REG         0x032DU
#define IMX317_PSLVDS2_LSB_REG         0x034AU
#define IMX317_PSLVDS2_MSB_REG         0x034BU
#define IMX317_PSLVDS3_LSB_REG         0x05B6U
#define IMX317_PSLVDS3_MSB_REG         0x05B7U
#define IMX317_PSLVDS4_LSB_REG         0x05B8U
#define IMX317_PSLVDS4_MSB_REG         0x05B9U

#define IMX317_LVDS_8CH                0U
#define IMX317_LVDS_6CH                1U
#define IMX317_LVDS_4CH                2U
#define IMX317_LVDS_2CH                3U
#define IMX317_LVDS_1CH                4U

#define AMBA_SENSOR_IMX317_3840_2162_30P      0U    /* (customized mode for calibration) */
#define AMBA_SENSOR_IMX317_3840_2160_30P      1U    /* TYPE_2_5_MODE_0 */
#define AMBA_SENSOR_IMX317_3840_2160_60P      2U    /* TYPE_2_5_MODE_1 */
#define AMBA_SENSOR_IMX317_1920_1080_60P      3U    /* TYPE_2_5_MODE_2 */
#define AMBA_SENSOR_IMX317_1920_1080_120P     4U    /* TYPE_2_5_MODE_3 */
#define AMBA_SENSOR_IMX317_1920_1080_30P      5U    /* TYPE_2_5_MODE_4 */
#define AMBA_SENSOR_IMX317_1280_540_240P      6U    /* TYPE_2_5_MODE_6 */
#define AMBA_SENSOR_IMX317_NUM_MODE           7U

typedef struct {
    UINT16  Addr;
    UINT8  Data;
} IMX317_REG_s;

typedef struct {
    UINT16  Addr;
    UINT8   Data[AMBA_SENSOR_IMX317_NUM_MODE];
} IMX317_MODE_REG_s;

typedef struct {
    UINT32  InputClk;                       /* Sensor side input clock frequency */
    UINT32  NumTickPerXhs;                  /* XHS period (in input clock cycles) */
    UINT32  NumXhsPerH;                     /* horizontal operating period (in number of XHS pulses) */
    UINT32  NumXhsPerV;                     /* vertical operating period (in number of XHS pulses) */
    UINT32  NumXvsPerV;                     /* vertical operating period (in number of XVS pulses) */
    AMBA_VIN_FRAME_RATE_s   FrameRate;      /* framerate value of this sensor mode */
} IMX317_FRAME_TIMING_s;

typedef struct {
    IMX317_FRAME_TIMING_s   FrameTiming;
    FLOAT                   RowTime;        /* Calculated in IMX317_Init */
} IMX317_MODE_INFO_s;

typedef struct {
    AMBA_SENSOR_STATUS_INFO_s   Status;
    UINT32                      CurrentAgcCtrl[2];
    UINT32                      CurrentDgcCtrl[2];
    UINT32                      CurrentShrCtrl[2];
} IMX317_CTRL_s;

/*-----------------------------------------------------------------------------------------------*\
 *  Defined in AmbaSensor_IMX317.c
\*-----------------------------------------------------------------------------------------------*/
extern const AMBA_SENSOR_DEVICE_INFO_s IMX317DeviceInfo;
extern const AMBA_SENSOR_INPUT_INFO_s IMX317InputInfo[AMBA_SENSOR_IMX317_NUM_MODE];
extern const AMBA_SENSOR_OUTPUT_INFO_s IMX317OutputInfo[AMBA_SENSOR_IMX317_NUM_MODE];
extern IMX317_MODE_INFO_s IMX317ModeInfoList[AMBA_SENSOR_IMX317_NUM_MODE];
extern const AMBA_SENSOR_HDR_INFO_s IMX317HdrInfo[AMBA_SENSOR_IMX317_NUM_MODE];

extern IMX317_REG_s IMX317PlstmgRegTable[IMX317_NUM_READOUT_PSTMG_REG];
extern IMX317_MODE_REG_s IMX317ModeRegTable[IMX317_NUM_READOUT_MODE_REG];

extern AMBA_SENSOR_OBJ_s AmbaSensor_IMX317Obj;

#endif /* AMBA_SENSOR_IMX317_H */
