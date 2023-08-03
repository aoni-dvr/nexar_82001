/**
 *  @file AmbaSbrg_Max9295_9296.h
 *
 *  @copyright Copyright (c) 2018 Ambarella, Inc.
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
 *  @details Definitions & Constants for MAX9295 & MAX9296 APIs
 *
 */

#ifndef AMBA_SBRG_MAX9295_9296_H
#define AMBA_SBRG_MAX9295_9296_H

#ifndef AMBA_ERROR_CODE_H
#include "AmbaErrorCode.h"
#endif

/* ERROR CODE */
#define MAX9295_9296_ERR_NONE                 (0U)
#define MAX9295_9296_ERR_INVALID_API          (BRIDGE_ERR_BASE + 1U)
#define MAX9295_9296_ERR_ARG                  (BRIDGE_ERR_BASE + 2U)
#define MAX9295_9296_ERR_SERDES_LINK          (BRIDGE_ERR_BASE + 3U)
#define MAX9295_9296_ERR_COMMUNICATE          (BRIDGE_ERR_BASE + 4U)    /* I2C to communicate with MAX9295_9296 fail*/
#define MAX9295_9296_ERR_UNKNOWN_CHIP_VERSION (BRIDGE_ERR_BASE + 5U)    /* unknown MAX9295_9296 chip version */
#define MAX9295_9296_ERR_UNEXPECTED           (BRIDGE_ERR_BASE + 6U)
#define MAX9295_9296_ERR_MUTEX                (BRIDGE_ERR_BASE + 7U)

/* print module */
#define MAX9295_9296_MODULE_ID                ((UINT16)(BRIDGE_ERR_BASE >> 16U))

/* I2C slave Addr */
#define MX00_IDC_ADDR_MAX9296_0         0x90U
#if defined(CONFIG_BSP_CV22DK)
#define MX00_IDC_ADDR_MAX9296_1         0x90U
#else
#define MX00_IDC_ADDR_MAX9296_1         0x94U
#endif

#define MX00_IDC_ADDR_MAX9295_DEFAULT   0x80U
#define MX00_IDC_ADDR_MAX9295_0_A       0x7EU
#define MX00_IDC_ADDR_MAX9295_0_B       0x82U

#define MX00_IDC_ADDR_MAX9295_1_A       0x8CU
#define MX00_IDC_ADDR_MAX9295_1_B       0x8EU

/* Port number */
#define MAX9296_NUM_RX_PORT              2U     /* 0: GMSL2 Link A,  1: GMSL2 Link B  */
#define MAX9296_NUM_TX_PORT              2U     /* 0: CSI Tx port A, 1: CSI Tx port B */

/* GPIO register definition */
typedef struct {
    UINT32  GpioOutDis:             1;     /* [0] 1 = disable GPIO output driver */
    UINT32  GpioTxEn:               1;     /* [1] 1 = enable GPIO TX source for GMSL2 transmission */
    UINT32  GpioRxEn:               1;     /* [2] 1 = enable GPIO Out source for GMSL2 reception */
    UINT32  GpioIn:                 1;     /* [3] GPIO pin input value */
    UINT32  GpioOut:                1;     /* [4] GPIO pin output drive value when GpioRxEn=0 */
    UINT32  TxCompEn:               1;     /* [5] Jitter minimization compesation enable */
    UINT32  TxPrio:                 1;     /* [6] Priority for GPIO scheduling. 0 - Low priority, 1 - High priority */
    UINT32  ResCfg:                 1;     /* [7] Resistor pull-up/pull-down strength. 0 - 40kohm, 1 - 1Mohm  */
    UINT32  Reserved:               24;    /* for misrac compliant */
} MAX9295_9296_GPIO_A_REG_s;

typedef struct {
    UINT32  GpioTxId:               5;     /* [4:0] GPIO ID for pin while transmiting */
    UINT32  OutType:                1;     /* [5] Driver type selection. 0 - Open-drain, 1 - Push-pull */
    UINT32  PullUpDnSel:            2;     /* [7:6] Buffer pull up/down configuration. 0 - None, 1 - Pull-up, 2 - Pull-down, 3 - Reserved */
    UINT32  Reserved:               24;    /* for misrac compliant */
} MAX9295_9296_GPIO_B_REG_s;

#if 0
typedef struct {
    UINT32  GpioRxId:               5;     /* [4:0] GPIO ID for pin while receiving */
    UINT32  Reserved0:              2;     /* [6:5] */
    UINT32  OvrResCfg:              1;     /* [7] Override non-GPIO port function IO setting.
                                                 0 - Non-GPIO function determines IO type when alternative function is selected
                                                 1 - ResCfg and PullUpDnSel determine IO type for non-GPIO configuration*/
    UINT32  Reserved:               24;    /* for misrac compliant */
} MAX9295_9296_GPIO_C_REG_s;
#endif


#if 0
typedef enum _MAX9295_9296_RX_PORT_TO_CSI_TX_PORT_MAPPING_e_ {
    MAX9295_9296_RX_PORT_TO_CSI_TX_PORT_0,
    MAX9295_9296_RX_PORT_TO_CSI_TX_PORT_1,
} MAX9295_9296_RX_PORT_TO_CSI_TX_PORT_MAPPING_e;
#endif

typedef struct {
    UINT16                                                  EnabledLinkID;                           /* Enabled Link ID */
    UINT8                                                   SensorSlaveID;                           /* Sensor slave ID */
    UINT8                                                   SensorAliasID[MAX9296_NUM_RX_PORT];      /* Sensor alias ID */
    UINT8                                                   NumDataBits;                             /* Pixel data bit depth */
    UINT8                                                   CSIRxLaneNum;
    UINT8                                                   CSITxLaneNum[MAX9296_NUM_TX_PORT];
    UINT8                                                   CSITxSpeed[MAX9296_NUM_TX_PORT];         /* in multiple of 100MHz, up to 25 */
    UINT8                                                   HighBandwidthModeEn;       /* 0: GMSL2 mode2([TBD]MAX9296 datarate < 950Mbps/lane, wait for FAE's reply)
                                                                                          1: GMSL2 mode1([TBD]MAX9296 datarate >= 950Mbps/lane, wait for FAE's reply)
                                                                                          Only verify 1ch/2ch 10bit case (OX05b1S) for mode1 */

} MAX9295_9296_SERDES_CONFIG_s;

typedef struct {
    UINT32 LinkLock;    /* 0: not support to check, 1: Far-end chip access OK, 2: Far-end chip access NG */
    UINT32 VideoLock;   /* 0: not support to check, 1: Near-end chip receives video data OK, 2: receive video data NG */
    UINT32 Reserved[2];
} MAX9295_9296_SERDES_STATUS_s;

typedef struct {
    UINT8  Reg0xE;
    float Ver;
} MAX9295_9296_CHIP_VERSION_s;

/* definition for RClkOut of Max9295_9296_SetSensorClk */
#define MAX9295_9296_RCLK_OUT_FREQ_37P125M    0U
#define MAX9295_9296_RCLK_OUT_FREQ_24M        1U
#define MAX9295_9296_RCLK_OUT_FREQ_27M        2U
#define MAX9295_9296_RCLK_OUT_FREQ_26973027   3U  /* 27/1.001 MHz */

/* definition for PinID of AmbaSbrg_Max9295_9296_SetGpioOutput */
#define MAX9295_9296_GPIO_PIN_0   0U
#define MAX9295_9296_GPIO_PIN_1   1U
#define MAX9295_9296_GPIO_PIN_2   2U
#define MAX9295_9296_GPIO_PIN_3   3U
#define MAX9295_9296_GPIO_PIN_4   4U
#define MAX9295_9296_GPIO_PIN_5   5U
#define MAX9295_9296_GPIO_PIN_6   6U
#define MAX9295_9296_GPIO_PIN_7   7U
#define MAX9295_9296_GPIO_PIN_8   8U
#define MAX9295_9296_GPIO_PIN_9   9U
#define MAX9295_9296_GPIO_PIN_10  10U
#define AMBA_SBRG_NUM_MAX9295_9296_GPIO_PIN 11U

/* ChipID */
#define MX00_MAX9295_A_ID     0U  /* Serializer A */
#define MX00_MAX9295_B_ID     1U  /* Serializer B */
#define MX00_MAX9296_ID       2U  /* Deserializer */

/* Serdes Status */
#define MX00_SERDES_STATUS_NOT_SUPPORTED 0 /* read i2c NG */
#define MX00_SERDES_STATUS_OK 1 /* Locked */
#define MX00_SERDES_STATUS_NG 2 /* Not Locked */

UINT32 Max9295_9296_Init(UINT32 VinID, UINT16 EnabledLinkID);
UINT32 Max9295_9296_DeInit(UINT32 VinID);
UINT32 Max9295_9296_Config(UINT32 VinID, const MAX9295_9296_SERDES_CONFIG_s *pSerdesConfig);
UINT32 Max9295_9296_SetSensorClk(UINT32 VinID, UINT32 ChipID, UINT32 RClkOut);
UINT32 Max9295_9296_SetGpioOutput(UINT32 VinID, UINT32 ChipID, UINT32 PinID, UINT8 Level);
UINT32 Max9295_9296_RegisterWrite(UINT32 VinID, UINT32 ChipID, UINT16 Addr, UINT8 Data);
UINT32 Max9295_9296_RegisterRead(UINT32 VinID, UINT32 ChipID, UINT16 Addr, UINT8* pData);
UINT32 Max9295_9296_GetSerdesStatus(UINT32 VinID, UINT16 EnabledLinkID, MAX9295_9296_SERDES_STATUS_s *pSerdesStatus);
#endif /* AMBA_SBRG_MAX9295_9296_H */
