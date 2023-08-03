/**
 *  @file AmbaSbrg_Max9295_96712.h
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
 *  @details Definitions & Constants for MAX9295 & MAX96712 APIs
 *
 */

#ifndef AMBA_SBRG_MAX9295_96712_H
#define AMBA_SBRG_MAX9295_96712_H

#ifndef AMBA_ERROR_CODE_H
#include "AmbaErrorCode.h"
#endif

/* ERROR CODE */
#define MAX9295_96712_ERR_NONE                 (0U)
#define MAX9295_96712_ERR_INVALID_API          (BRIDGE_ERR_BASE + 1U)
#define MAX9295_96712_ERR_ARG                  (BRIDGE_ERR_BASE + 2U)
#define MAX9295_96712_ERR_SERDES_LINK          (BRIDGE_ERR_BASE + 3U)
#define MAX9295_96712_ERR_COMMUNICATE          (BRIDGE_ERR_BASE + 4U)   /* I2C to communicate with MAX9295_96712 fail*/
#define MAX9295_96712_ERR_MUTEX                (BRIDGE_ERR_BASE + 5U)
#define MAX9295_96712_ERR_UNKNOWN_CHIP_VERSION (BRIDGE_ERR_BASE + 6U)   /* unknown MAX9295_96712 chip version */
#define MAX9295_96712_ERR_UNEXPECTED           (BRIDGE_ERR_BASE + 7U)

/* print module */
#define MAX9295_96712_MODULE_ID                ((UINT16)(BRIDGE_ERR_BASE >> 16U))

/* I2C slave Addr */
#define MX01_IDC_ADDR_MAX96712           0x52U
#define MX01_IDC_ADDR_MAX9295_DEFAULT    0x80U
#define MX01_IDC_ADDR_MAX9295_A          0x82U
#define MX01_IDC_ADDR_MAX9295_B          0x84U
#define MX01_IDC_ADDR_MAX9295_C          0x86U
#define MX01_IDC_ADDR_MAX9295_D          0x88U
#define MX01_IDC_ADDR_MAX9295_BC         0x92U  //avoid conlict to MAX9296 0x90
/* Port number */
#define MAX96712_NUM_RX_PORT              4U     /* 0/1/2/3: GMSL2 Link A/B/C/D  */
#define MAX96712_NUM_TX_PORT              4U     /* 0/1/2/3: CSI Tx port A/B/C/D */

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
} MAX9295_96712_GPIO_A_REG_s;

typedef struct {
    UINT32  GpioTxId:               5;     /* [4:0] GPIO ID for pin while transmiting */
    UINT32  OutType:                1;     /* [5] Driver type selection. 0 - Open-drain, 1 - Push-pull */
    UINT32  PullUpDnSel:            2;     /* [7:6] Buffer pull up/down configuration. 0 - None, 1 - Pull-up, 2 - Pull-down, 3 - Reserved */
    UINT32  Reserved:               24;    /* for misrac compliant */
} MAX9295_96712_GPIO_B_REG_s;

#if 0
typedef enum _MAX9295_96712_RX_PORT_TO_CSI_TX_PORT_MAPPING_e_ {
    MAX9295_96712_RX_PORT_TO_CSI_TX_PORT_0,
    MAX9295_96712_RX_PORT_TO_CSI_TX_PORT_1,
} AMBA_SBRG_MAX9295_96712_RX_PORT_TO_CSI_TX_PORT_MAPPING_e;
#endif

typedef struct {
    UINT16                                EnabledLinkID;                           /* Enabled Link ID */
    UINT8                                 SensorSlaveID;                           /* Sensor slave ID */
    UINT8                                 SensorBCID;                              /* Sensor broadcast ID */
    UINT8                                 SensorAliasID[MAX96712_NUM_RX_PORT];     /* Sensor alias ID */
    UINT8                                 DataType;                                /* Image data type */
    UINT8                                 DataType2;                               /* User-defined secondary data type to be received. 0xff = not used */
    UINT8                                 NumDataBits;                             /* Pixel data bit depth */
    UINT8                                 CSIRxLaneNum;
    UINT8                                 CSITxLaneNum[MAX96712_NUM_TX_PORT];
    UINT8                                 CSITxSpeed[MAX96712_NUM_TX_PORT];        /* in multiple of 100MHz, up to 25 */
} MAX9295_96712_SERDES_CONFIG_s;

typedef struct {
    UINT32 LinkLock;    /* 0: not support to check, 1: Far-end chip access OK, 2: Far-end chip access NG */
    UINT32 VideoLock;   /* 0: not support to check, 1: Near-end chip receives video data OK, 2: receive video data NG */
    UINT32 Reserved[2];
} MAX9295_96712_SERDES_STATUS_s;

/* definition for RClkOut of Max9295_96712_SetSensorClk */
#define MAX9295_96712_RCLK_OUT_FREQ_27M       0U
#define MAX9295_96712_RCLK_OUT_FREQ_37P125M   1U
#define MAX9295_96712_RCLK_OUT_FREQ_26973027  2U  /* 27/1.001 MHz */
#define MAX9295_96712_RCLK_OUT_FREQ_37087912  3U  /* 37.125/1.001 MHz */
#define MAX9295_96712_RCLK_OUT_FREQ_24M       4U
#define MAX9295_96712_RCLK_OUT_FREQ_23976024  5U  /* 24/1.001 MHz */

/* definition for PinID of AmbaSbrg_Max9295_96712_SetGpioOutput */
#define MAX9295_96712_GPIO_PIN_0   0U
#define MAX9295_96712_GPIO_PIN_1   1U
#define MAX9295_96712_GPIO_PIN_2   2U
#define MAX9295_96712_GPIO_PIN_3   3U
#define MAX9295_96712_GPIO_PIN_4   4U
#define MAX9295_96712_GPIO_PIN_5   5U
#define MAX9295_96712_GPIO_PIN_6   6U
#define MAX9295_96712_GPIO_PIN_7   7U
#define MAX9295_96712_GPIO_PIN_8   8U
#define MAX9295_96712_GPIO_PIN_9   9U
#define MAX9295_96712_GPIO_PIN_10  10U
#define AMBA_SBRG_NUM_MAX9295_96712_GPIO_PIN 11U

/* ChipID */
#define MX01_MAX9295_A_ID    0U  /* Serializer A */
#define MX01_MAX9295_B_ID    1U  /* Serializer B */
#define MX01_MAX9295_C_ID    2U  /* Serializer C */
#define MX01_MAX9295_D_ID    3U  /* Serializer D */
#define MX01_MAX96712_ID     4U  /* Deserializer */

/* Serdes Status */
#define MX01_SERDES_STATUS_NOT_SUPPORTED 0 /* read i2c NG */
#define MX01_SERDES_STATUS_OK 1 /* Locked */
#define MX01_SERDES_STATUS_NG 2 /* Not Locked */
UINT32 Max9295_96712_Init(UINT32 VinID, UINT16 EnabledLinkID);
UINT32 Max9295_96712_GetSerdesStatus(UINT32 VinID, UINT16 EnabledLinkID, MAX9295_96712_SERDES_STATUS_s *pSerdesStatus);
UINT32 Max9295_96712_Config(UINT32 VinID, const MAX9295_96712_SERDES_CONFIG_s *pSerdesConfig);
UINT32 Max9295_96712_SetSensorClk(UINT32 VinID, UINT32 ChipID, UINT32 RClkOut);
UINT32 Max9295_96712_SetGpioOutput(UINT32 VinID, UINT32 ChipID, UINT32 PinID, UINT8 Level);
UINT32 Max9295_96712_RegisterWrite(UINT32 VinID, UINT32 ChipID, UINT16 Addr, UINT8 Data);
UINT32 Max9295_96712_RegisterRead(UINT32 VinID, UINT32 ChipID, UINT16 Addr, UINT8* pData);

#endif /* _AMBA_SBRG_MAX9295_96712_H_ */
