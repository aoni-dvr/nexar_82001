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
#define AMBA_VIN_CHANNEL5       5U
#define AMBA_NUM_VIN_CHANNEL    6U

#define AMBA_VIN_MSYNC0         0U
#define AMBA_VIN_MSYNC1         1U
#define AMBA_NUM_VIN_MSYNC      2U

#define AMBA_VIN_SENSOR_CLOCK0      0U
#define AMBA_VIN_SENSOR_CLOCK1      1U
#define AMBA_NUM_VIN_SENSOR_CLOCK   2U

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

#define AMBA_VIN_DELAYED_VSYNC_NUM          5U          /* VSYNC0, VSYNC1, VSYNC2, VSYNC3, VSYNC4 */

#define AMBA_VIN_VSDLY_SRC_MSYNC0_VSYNC     0U          /* idsp_vin_iopad_master_vsync*/
#define AMBA_VIN_VSDLY_SRC_MSYNC1_VSYNC     1U          /* idsp_pip_iopad_master_vsync */

#define AMBA_VIN_VSDLY_POL_ACTIVE_LOW       0U          /* polarity of Vsync: low active */
#define AMBA_VIN_VSDLY_POL_ACTIVE_HIGH      1U          /* polarity of Vsync: high active */

#define AMBA_VIN_VSDLY_UNIT_MSYNC0_HSYNC    0U          /* idsp_vin_iopad_master_hsync*/
#define AMBA_VIN_VSDLY_UNIT_MSYNC1_HSYNC    1U          /* idsp_pip_iopad_master_hsync */

typedef struct {
    UINT32 DelayedVsyncSource;                          /* Vsync source select, check AMBA_VIN_VSDLY_SRC_xxx */
    UINT32 Polarity;                                    /* 0U: Active Low, 1U: Active High, check AMBA_VIN_VSDLY_POL_xxx */
    UINT32 DelayedUnit;                                 /* Unit of Delay, check AMBA_VIN_VSDLY_UNIT_xxx */

    /* Vsync delay(unit: DelayedUnit) = DelayCounter * DelayMultiplier */
    UINT32 DelayCounter[AMBA_VIN_DELAYED_VSYNC_NUM];      /* Max. value: 1023 */
    UINT32 DelayMultiplier[AMBA_VIN_DELAYED_VSYNC_NUM];   /* Max. value: 7 */

} AMBA_VIN_DELAYED_VSYNC_CONFIG_s;

UINT32 AmbaVIN_DelayedVSyncConfig(const AMBA_VIN_DELAYED_VSYNC_CONFIG_s *pDelayedVSyncConfig);

#endif /* AMBA_VIN_PRIV_H */
