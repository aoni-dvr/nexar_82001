/**
 *  @file AmbaSbrg_TI953_954.h
 *
 *  @copyright Copyright (c) 2019 Ambarella, Inc.
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
 *  @details Definitions & Constants for TI953 & TI954 APIs
 *
 */

#ifndef TI953_954_H
#define TI953_954_H

#ifndef AMBA_ERROR_CODE_H
#include "AmbaErrorCode.h"
#endif

#define TI953_954_ERR_NONE                  (0U)
#define TI953_954_ERR_ARG                   (BRIDGE_ERR_BASE + 1U)
#define TI953_954_ERR_MUTEX                 (BRIDGE_ERR_BASE + 2U)
#define TI953_954_ERR_UNKNOWN_CHIP_ID       (BRIDGE_ERR_BASE + 3U)
#define TI953_954_ERR_SERDES_LINK           (BRIDGE_ERR_BASE + 4U)

#define TI953_954_MODULE_ID                 ((UINT16)(BRIDGE_ERR_BASE >> 16U))


/*-----------------------------------------------------------------------------------------------*\
 * Device Slave ID Definition
\*-----------------------------------------------------------------------------------------------*/
#define IDC_ADDR_TI953_A_LINK0              0x82U
#define IDC_ADDR_TI953_A_LINK1              0x84U
#define IDC_ADDR_TI953_B_LINK0              0x92U
#define IDC_ADDR_TI953_B_LINK1              0x94U

/* Hardware Configuration Settings */
#define IDC_ADDR_TI954_A                    0x60U
#define IDC_ADDR_TI954_B                    0x60U
/* ------------------------------- */

/*-----------------------------------------------------------------------------------------------*\
 * TI953/954 Port ID Definition
\*-----------------------------------------------------------------------------------------------*/
#define TI953_954_RX_PORT_0                  0x1U
#define TI953_954_RX_PORT_1                  0x2U

#define TI954_NUM_RX_PORT                      2U

/*-----------------------------------------------------------------------------------------------*\
 * Sensor Clock Mode
\*-----------------------------------------------------------------------------------------------*/
#define TI953_954_RCLK_OUT_FREQ_25M            0U
#define TI953_954_RCLK_OUT_FREQ_27M            1U

/*-----------------------------------------------------------------------------------------------*\
 * Serdes Output Definition
\*-----------------------------------------------------------------------------------------------*/
#define TI953_954_CSI_LANE_NUM_1               1U
#define TI953_954_CSI_LANE_NUM_2               2U
#define TI953_954_CSI_LANE_NUM_3               3U
#define TI953_954_CSI_LANE_NUM_4               4U

#define TI953_954_NUM_CSI_LANE                 4U


#define TI953_954_CSI_TX_SPEED_1P6G            0U
#define TI953_954_CSI_TX_SPEED_800M            1U
#define TI953_954_CSI_TX_SPEED_400M            2U

#define TI953_954_LINE_INTERLEAVE_FWD          0U
#define TI953_954_LINE_CONCATENATED_FWD        1U


typedef struct {
    UINT16 EnabledRxPortID;                  /* Enabled Rx Port, please check TI953_954_RX_PORT_x */
    UINT8  SensorSlaveID;                    /* Original sensor slave address */
    UINT8  SensorAliasID[TI954_NUM_RX_PORT]; /* aliased sensor slave address */
} TI953_954_SERDES_CONFIG_s;

typedef struct {
    UINT32   CSILaneNum;                     /* Output data lane number, only support 1, 2, 3, 4-lane */
    UINT32   CSITxSpeed;                     /* TI954 CSI output data rate, please check TI953_954_CSI_TX_SPEED_xxxx */
    UINT32   ForwardingMode;                 /* 0U: Line-Interleaved , 1U: Line-Concatenated, please check TI953_954_LINE_xxx_FWD */
} TI953_954_SERDES_OUTPUT_CONFIG_s;


UINT32 TI953_954_Init(UINT32 VinID, UINT16 RxPortID);
UINT32 TI953_954_Config(UINT32 VinID, const TI953_954_SERDES_CONFIG_s *pSerdesConfig);
UINT32 TI953_954_SetSensorClk(UINT32 VinID, UINT16 RxPortID, UINT32 RClkOut);
UINT32 TI953_954_EnableCSIOutput(UINT32 VinID, const TI953_954_SERDES_OUTPUT_CONFIG_s *pSerdesOutputConfig);

#endif /* TI953_954_H */
