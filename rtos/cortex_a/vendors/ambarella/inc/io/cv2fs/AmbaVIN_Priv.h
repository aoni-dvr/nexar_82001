/**
 *  @file AmbaVIN_Priv.h
 *
 *  Copyright (c) 2020 Ambarella International LP
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
 *  @details Definitions & Constants for Video Input Middleware APIs
 *
 */

#ifndef AMBA_VIN_PRIV_H
#define AMBA_VIN_PRIV_H

#define AMBA_VIN_CHANNEL0       0U
#define AMBA_VIN_CHANNEL1       1U
#define AMBA_VIN_CHANNEL2       2U
#define AMBA_VIN_CHANNEL3       3U
#define AMBA_VIN_CHANNEL4       4U
#define AMBA_VIN_CHANNEL8       5U
#define AMBA_NUM_VIN_CHANNEL    6U

#define AMBA_VIN_MSYNC0         0U
#define AMBA_VIN_MSYNC1         1U
#define AMBA_NUM_VIN_MSYNC      2U

#define AMBA_VIN_SENSOR_CLOCK0      0U
#define AMBA_VIN_SENSOR_CLOCK1      1U
#define AMBA_VIN_SENSOR_CLOCK2      2U
#define AMBA_NUM_VIN_SENSOR_CLOCK   3U

#define AMBA_VIN_TERMINATION_VALUE_DEFAULT  6U

#define AMBA_VIN_DVP_SYNC_PIN_SPCLK_N_0     0U
#define AMBA_VIN_DVP_SYNC_PIN_SPCLK_N_1     1U
#define AMBA_VIN_DVP_SYNC_PIN_SPCLK_P_1     2U
#define AMBA_VIN_DVP_SYNC_PIN_SD_P_8        19U
#define AMBA_VIN_DVP_SYNC_PIN_SD_N_8        20U
#define AMBA_VIN_DVP_SYNC_PIN_SD_P_9        21U
#define AMBA_VIN_DVP_SYNC_PIN_SD_N_9        22U
#define AMBA_VIN_DVP_SYNC_PIN_SD_P_10       23U
#define AMBA_VIN_DVP_SYNC_PIN_SD_N_10       24U
#define AMBA_VIN_DVP_SYNC_PIN_SD_P_11       25U
#define AMBA_VIN_DVP_SYNC_PIN_SD_N_11       26U


#define AMBA_VIN_DELAYED_VSYNC_NUM          2U          /* Number of DelayVsync output port */

#define AMBA_VIN_VSDLY_SRC_HV_MSYNC0        0U          /* idsp_vin0_iopad_master_hsync/vsync */
#define AMBA_VIN_VSDLY_SRC_HV_MSYNC1        1U          /* idsp_vin1_iopad_master_hsync/vsync */
#define AMBA_VIN_VSDLY_SRC_H_MSYNC0_V_EXT   2U          /* idsp_vin0_iopad_master_hsync + ext_vsync_src_input */
#define AMBA_VIN_VSDLY_SRC_H_MSYNC1_V_EXT   3U          /* idsp_vin1_iopad_master_hsync + ext_vsync_src_input */

#define AMBA_VIN_VSDLY_POL_ACTIVE_LOW       0U          /* Polarity: active low */
#define AMBA_VIN_VSDLY_POL_ACTIVE_HIGH      1U          /* Polarity: active high */

#define AMBA_VIN_VSDLY_WIDTH_AS_INPUT       0U          /* Output pulse width = input pulse width */
#define AMBA_VIN_VSDLY_WIDTH_MANUAL         1U          /* User defined output pulse width */

#define AMBA_VIN_VSDLY_TIME_MAX         65530U          /* Maximum value for delay period */
#define AMBA_VIN_VSDLY_WIDTH_MAX        65530U          /* Maximum value for pulse width */

#define AMBA_VIN_MSYNC_PERIOD_MIN           1U          /* Minimum value for H/V sync period */
#define AMBA_VIN_MSYNC_PERIOD_MAX  0xffffffffU          /* Maximum value for H/V sync period */

#define AMBA_VIN_MSYNC_WIDTH_MIN          0x1U          /* Minimum value for H/V sync pulse width */
#define AMBA_VIN_MSYNC_WIDTH_MAX       0xffffU          /* Maximum value for H/V sync pulse width */

#define AMBA_VIN_MSYNC_DELAY_MIN           0x0          /* Minimum value for H/V sync offset */
#define AMBA_VIN_MSYNC_DELAY_MAX       0xffffU          /* Maximum value for H/V sync offset */

#define AMBA_VIN_MIPI_DATARATE_MIN   80000000U          /* Minimum value for MIPI data rate */
#define AMBA_VIN_MIPI_DATARATE_MAX 2500000000U          /* Maximum value for MIPI data rate */


typedef struct {
    UINT32 RefClk;                                      /* Reference clock rate */
    UINT32 HsDlyClk;                                    /* Number of clock cycles for hsync to be delayed */
    UINT32 VsDlyClk[AMBA_VIN_DELAYED_VSYNC_NUM];        /* Number of clock cycles for vsync to be delayed */
    UINT32 HsPulseWidth;                                /* Hsync output pulse width (unit: clk cycle), 0 = Use source pulse width */
    UINT32 VsPulseWidth[AMBA_VIN_DELAYED_VSYNC_NUM];    /* Vsync output pulse width (unit: clk cycle), 0 = Use source pulse width */
} AMBA_VIN_DELAYED_VSYNC_FINE_ADJUST_s;

typedef struct {
    UINT32 HsPolInput;                                  /* Hsync input polarity, 0 = active low, 1 = active high, check AMBA_VIN_VSDLY_POL_xxx */
    UINT32 VsPolInput;                                  /* Vsync input polarity, 0 = active low, 1 = active high, check AMBA_VIN_VSDLY_POL_xxx */
    UINT32 HsPolOutput;                                 /* Hsync output polarity, 0 = active low, 1 = active high, check AMBA_VIN_VSDLY_POL_xxx */
    UINT32 VsPolOutput[AMBA_VIN_DELAYED_VSYNC_NUM];     /* Vsync output polarity, 0 = active low, 1 = active high, check AMBA_VIN_VSDLY_POL_xxx */
} AMBA_VIN_DELAYED_VSYNC_POLARITY_s;

typedef struct {
    UINT32                                DelayedHVsyncSource;                         /* H/Vsync source select, check AMBA_VIN_VSDLY_SRC_xxx */
    UINT32                                VsDlyPeriod[AMBA_VIN_DELAYED_VSYNC_NUM];     /* Number of hsync period for vsync to be delayed */
    AMBA_VIN_DELAYED_VSYNC_POLARITY_s     Polarity;                                    /* H/Vsync polarity configuration */
    AMBA_VIN_DELAYED_VSYNC_FINE_ADJUST_s  FineAdjust;                                  /* Delay period / pulse width fine-tune configuration. If not used, please set this struct to {0}. */
} AMBA_VIN_DELAYED_VSYNC_CONFIG_s;

typedef struct {
    UINT8   DataType;           /* Data type of MIPI embedded data packet */
    UINT8   DataTypeMask;       /* Data type mask of MIPI embedded data packet.
                                   bit[n] =1U: ignore bit[n] of Data Type when checking Data Type */
} AMBA_VIN_MIPI_EMB_DATA_CONFIG_s;

UINT32 AmbaVIN_DelayedVSyncEnable(const AMBA_VIN_DELAYED_VSYNC_CONFIG_s *pDelayedVSyncConfig);
UINT32 AmbaVIN_DelayedVSyncDisable(void);

UINT32 AmbaVIN_MipiEmbDataConfig(UINT32 VinID, const AMBA_VIN_MIPI_EMB_DATA_CONFIG_s *pMipiEmbDataConfig);

#endif /* AMBA_VIN_PRIV_H */
