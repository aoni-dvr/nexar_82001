/**
 *  @file AmbaSensor_IMX677.h
 *
 *  Copyright (c) 2021 Ambarella International LP
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
 *  @details Control APIs of SONY IMX677 CMOS sensor with SLVS-EC interface
 *
 */

#ifndef AMBA_SENSOR_IMX677_H
#define AMBA_SENSOR_IMX677_H

/*-----------------------------------------------------------------------------------------------*\
 * Control registers
\*-----------------------------------------------------------------------------------------------*/
#define IMX677_NUM_PLSTMG_REG          95U
#define IMX677_NUM_READOUT_MODE_REG    59U
#define IMX677_NUM_ATTR_REG            21U

#define IMX677_SDO_ACT_REG             0x0050U
#define IMX677_STB_AUTO_REG            0x0054U
#define IMX677_FREQ_REG                0x1A0BU
#define IMX677_FREQ_PLL_REG            0x07E1U
#define IMX677_WAKEUP_REG              0x0002U
#define IMX677_CLPSQRST_REG            0x1900U
#define IMX677_DLR_HSWIDTH_LSB_REG     0x1C02U
#define IMX677_DLR_HSWIDTH_MSB_REG     0x1C03U
#define IMX677_SHR_LT_LSB_REG          0x0010U
#define IMX677_SHR_LT_MSB_REG          0x0011U
#define IMX677_SVR_LSB_REG             0x0012U
#define IMX677_SVR_MSB_REG             0x0013U
#define IMX677_SPL_LSB_REG             0x0014U
#define IMX677_SPL_MSB_REG             0x0015U
#define IMX677_DGAIN_LT_REG            0x0084U
#define IMX677_APGC_LT_LSB_REG         0x0602U
#define IMX677_APGC_LT_MSB_REG         0x0603U

#define IMX677_5599_4223_30P      0U    /* Mode0 (2.304Gbps, 8lane, 12-bit A/D conversion, 12-bit length output) */
#define IMX677_5599_4223_30P_1    1U    /* Mode0 (4.608Gbps, 8lane, 12-bit A/D conversion, 12-bit length output) */
#define IMX677_5599_4223_15P      2U    /* Mode0 (2.304Gbps, 4lane, 12-bit A/D conversion, 12-bit length output) */
#define IMX677_5599_4223_30P_2    3U    /* Mode0 (4.608Gbps, 4lane, 12-bit A/D conversion, 12-bit length output) */
#define IMX677_5599_4223_30P_3    4U    /* Mode2 (4.608Gbps, 8lane, 11-bit A/D conversion, 12-bit length output) */
#define IMX677_5599_4223_30P_4    5U    /* Mode3 (4.608Gbps, 8lane, 10-bit A/D conversion, 10-bit length output) */
#define IMX677_5599_3119_30P      6U    /* Mode18(4.608Gbps, 8lane, 10-bit A/D conversion, 10-bit length output) */
#define IMX677_1865_4223_30P      7U    /* Mode22(4.608Gbps, 8lane, 11-bit A/D conversion, 12-bit length output) */
#define IMX677_NUM_MODE           8U

#define IMX677_MAX_APGC           979U  /* Max. approximately 27dB */
#define IMX677_MAX_DGAIN          3U    /* Max. 18dB */

typedef struct {
    UINT16  Addr;
    UINT8   Data;
} IMX677_REG_s;

typedef struct {
    UINT16  Addr;
    UINT8   Data[IMX677_NUM_MODE];
} IMX677_MODE_REG_s;

typedef struct {
    UINT32  InputClk;                       /* Sensor side input clock frequency */
    UINT32  NumTickPerXhs;                  /* XHS period (in input clock cycles) */
    DOUBLE  NumXhsPerH;                     /* horizontal operating period (in number of XHS pulses) */
    UINT32  NumXhsPerV;                     /* vertical operating period (in number of XHS pulses) */
    UINT32  NumXvsPerV;                     /* vertical operating period (in number of XVS pulses) */
    AMBA_VIN_FRAME_RATE_s   FrameRate;      /* framerate value of this sensor mode */
} IMX677_FRAME_TIMING_s;

typedef struct {
    UINT8                   DualLink;
    UINT8                   MinSHR;
    IMX677_FRAME_TIMING_s   FrameTiming;
    FLOAT                   RowTime;        /* Calculated in IMX677_Init */
} IMX677_MODE_INFO_s;

typedef struct {
    AMBA_SENSOR_STATUS_INFO_s   Status;
    UINT32                      CurrentAgcCtrl[2];
    UINT32                      CurrentDgcCtrl[2];
    UINT32                      CurrentShrCtrl[2];
} IMX677_CTRL_s;

/*-----------------------------------------------------------------------------------------------*\
 *  Defined in AmbaSensor_IMX677.c
\*-----------------------------------------------------------------------------------------------*/
extern const AMBA_SENSOR_DEVICE_INFO_s IMX677DeviceInfo;
extern const AMBA_SENSOR_INPUT_INFO_s IMX677InputInfo[IMX677_NUM_MODE];
extern const AMBA_SENSOR_OUTPUT_INFO_s IMX677OutputInfo[IMX677_NUM_MODE];
extern IMX677_MODE_INFO_s IMX677ModeInfoList[IMX677_NUM_MODE];
extern const AMBA_SENSOR_HDR_INFO_s IMX677HdrInfo[IMX677_NUM_MODE];

extern IMX677_REG_s IMX677PlstmgRegTable[IMX677_NUM_PLSTMG_REG];
extern IMX677_MODE_REG_s IMX677ModeRegTable[IMX677_NUM_READOUT_MODE_REG];
extern IMX677_REG_s IMX677AttrRegTable[IMX677_NUM_ATTR_REG];

extern AMBA_SENSOR_OBJ_s AmbaSensor_IMX677Obj;

#endif /* AMBA_SENSOR_IMX677_H */
