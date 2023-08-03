/**
 *  @file AmbaSbrg_TI913_954.h
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
 *  @details Definitions & Constants for TI913 & TI954 APIs
 *
 */

#ifndef TI913_954_H
#define TI913_954_H

#ifndef AMBA_ERROR_CODE_H
#include "AmbaErrorCode.h"
#endif

#define TI913_954_ERR_NONE                  (0U)
#define TI913_954_ERR_ARG                   (BRIDGE_ERR_BASE + 1U)
#define TI913_954_ERR_MUTEX                 (BRIDGE_ERR_BASE + 2U)
#define TI913_954_ERR_UNKNOWN_CHIP_ID       (BRIDGE_ERR_BASE + 3U)

#define TI913_954_MODULE_ID                 ((UINT16)(BRIDGE_ERR_BASE >> 16U))


/*-----------------------------------------------------------------------------------------------*\
 * TI913/954 Default I2C Address
\*-----------------------------------------------------------------------------------------------*/
#define TI03_IDC_ADDR_TI913_A_LINK0         0x82U
#define TI03_IDC_ADDR_TI913_A_LINK1         0x84U
#define TI03_IDC_ADDR_TI913_B_LINK0         0x86U
#define TI03_IDC_ADDR_TI913_B_LINK1         0x88U
#define TI03_IDC_ADDR_TI913_C_LINK0         0x92U
#define TI03_IDC_ADDR_TI913_C_LINK1         0x94U

#define TI03_IDC_ADDR_TI954_A               0x60U
#define TI03_IDC_ADDR_TI954_B               0x74U
#define TI03_IDC_ADDR_TI954_C               0x7AU

/*-----------------------------------------------------------------------------------------------*\
 * TI913/954 Chip ID Definition
\*-----------------------------------------------------------------------------------------------*/
#define TI913_A_CHIP_ID                     1U  /* Serializer on rx port 0 of Deserializer */
#define TI913_B_CHIP_ID                     2U  /* Serializer on rx port 1 of Deserializer */
#define TI954_CHIP_ID                       4U  /* Deserializer */

/*-----------------------------------------------------------------------------------------------*\
 * TI913/954 Port ID Definition
\*-----------------------------------------------------------------------------------------------*/
#define TI913_954_RX_PORT_0                 1U
#define TI913_954_RX_PORT_1                 2U
#define TI954_NUM_RX_PORT                   2U

/*-----------------------------------------------------------------------------------------------*\
 * TI913/954 SerDes FPD3 Mode
\*-----------------------------------------------------------------------------------------------*/
#define TI913_954_RAW12_50MHz               1U
#define TI913_954_RAW12_75MHz               2U
#define TI913_954_RAW10_100MHz              3U

/*-----------------------------------------------------------------------------------------------*\
 * TI913/954 GPIO PIN Definition
\*-----------------------------------------------------------------------------------------------*/
#define TI954_GPIO_PIN_0   0U
#define TI954_GPIO_PIN_1   1U
#define TI954_GPIO_PIN_2   2U
#define TI954_GPIO_PIN_3   3U
#define TI954_GPIO_PIN_4   4U
#define TI954_GPIO_PIN_5   5U
#define TI954_GPIO_PIN_6   6U
#define TI954_GPIO_NUM     7U

#define TI913_GPIO_PIN_0   0U
#define TI913_GPIO_PIN_1   1U
#define TI913_GPIO_PIN_2   2U
#define TI913_GPIO_PIN_3   3U
#define TI913_GPIO_NUM     4U

/*-----------------------------------------------------------------------------------------------*\
 * Sensor Clock Mode
\*-----------------------------------------------------------------------------------------------*/
#define TI913_954_RCLK_OUT_FREQ_25M         0U
#define TI913_954_RCLK_OUT_FREQ_27M         1U

/*-----------------------------------------------------------------------------------------------*\
 * Serdes CSI Output Definition
\*-----------------------------------------------------------------------------------------------*/
#define TI913_954_CSI_LANE_NUM_1            1U
#define TI913_954_CSI_LANE_NUM_2            2U
#define TI913_954_CSI_LANE_NUM_3            3U
#define TI913_954_CSI_LANE_NUM_4            4U
#define TI913_954_NUM_CSI_LANE              4U

#define TI913_954_CSI_TX_SPEED_1P6G         0U
#define TI913_954_CSI_TX_SPEED_800M         1U
#define TI913_954_CSI_TX_SPEED_400M         2U

#define TI913_954_LINE_INTERLEAVE_FWD       0U
#define TI913_954_LINE_CONCATENATED_FWD     1U

/*-----------------------------------------------------------------------------------------------*\
 * TI913/954 FrameSync Mode
\*-----------------------------------------------------------------------------------------------*/
#define TI913_954_NO_FSYNC                  0U
#define TI913_954_EXTERNAL_FSYNC            1U
#define TI913_954_INTERNAL_FSYNC            2U

typedef struct {
    UINT8  EnabledRxPortID;
    UINT8  SensorSlaveID;
    UINT8  SensorBroadcastAliasID;
    UINT8  SensorAliasID[TI954_NUM_RX_PORT];
    UINT8  TI913Alias[TI954_NUM_RX_PORT];
    UINT8  TI954Alias;
    UINT8  Fpd3Mode;
    UINT8  FvPolarity;  /* 0: active high, 1: active low */
    UINT8  LvPolarity;  /* 0: active high, 1: active low */
    struct {
        UINT8 NumDataLanes;       /* active data channels */
        UINT8 DataRate;
        UINT8 ForwardingMode;
    } CSIOutputInfo;
    struct {
        UINT8 FrameSyncEnable;
        UINT8 TI913Gpio;
        UINT8 TI954Gpio;
        UINT32  TimeScale;      /* time scale */
        UINT32  NumUnitsInTick; /* Frames per Second = TimeScale / NumUnitsInTick */
    } FrameSyncSel;
} TI913_954_SERDES_CONFIG_s;

UINT32 TI913_954_Init(UINT32 VinID, const TI913_954_SERDES_CONFIG_s *pSerdesConfig);
UINT32 TI913_954_Config(UINT32 VinID);
UINT32 TI913_954_SetSensorClk(UINT32 VinID, UINT32 ChipID, UINT32 RClkOut);
UINT32 TI913_954_SetGpioOutput(UINT32 VinID, UINT32 ChipID, UINT32 PinID, UINT32 PinLevel);
UINT32 TI913_954_EnableCSIOutput(UINT32 VinID);
UINT32 TI913_954_Patgen(UINT32 VinID);

#endif /* TI913_954_H */
