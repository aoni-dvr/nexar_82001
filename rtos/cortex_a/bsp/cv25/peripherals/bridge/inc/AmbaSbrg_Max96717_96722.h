/**
 *  @file AmbaSbrg_Max96717_96722.h
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
 *  @details Definitions & Constants for MAX96717 & MAX96722 APIs
 *
 */

#ifndef MAX96717_96722_H
#define MAX96717_96722_H

#ifndef AMBA_ERROR_CODE_H
#include "AmbaErrorCode.h"
#endif

/* ERROR CODE */
#define MAX96717_96722_ERR_NONE                 (0U)
#define MAX96717_96722_ERR_INVALID_API          (BRIDGE_ERR_BASE + 1U)
#define MAX96717_96722_ERR_ARG                  (BRIDGE_ERR_BASE + 2U)
#define MAX96717_96722_ERR_SERDES_LINK          (BRIDGE_ERR_BASE + 3U)
#define MAX96717_96722_ERR_COMMUNICATE          (BRIDGE_ERR_BASE + 4U)    /* I2C to communicate with MAX96717_96722 fail*/
#define MAX96717_96722_ERR_UNKNOWN_CHIP_VERSION (BRIDGE_ERR_BASE + 5U)    /* unknown MAX96717_96722 chip version */

/* Print module */
#define MAX96717_96722_MODULE_ID                 ((UINT16)(BRIDGE_ERR_BASE >> 16U))

/* I2C slave Addr */
#define IDC_ADDR_MAX96722_0                    0x52U
#define IDC_ADDR_MAX96722_1                    0x52U

#define IDC_ADDR_MAX96717_DEFAULT              0x80U
#define IDC_ADDR_MAX96717_DEFAULT2             0x84U

#define IDC_ADDR_MAX96717_0_A                  0x7EU
#define IDC_ADDR_MAX96717_0_B                  0x82U
#define IDC_ADDR_MAX96717_1_A                  0x8CU
#define IDC_ADDR_MAX96717_1_B                  0x8EU

/* Port number */
#define MAX96722_NUM_RX_PORT                   2U     /* 0: GMSL2 Link A,  1: GMSL2 Link B  */
#define MAX96722_NUM_TX_PORT                   2U     /* 0: CSI Tx port A, 1: CSI Tx port B */

/* definition for RClkOut of Max96717_96722_SetSensorClk */
#define MAX96717_96722_CLKFREQ_25M_MFP2        0U
#define MAX96717_96722_CLKFREQ_24M_MFP2        1U
#define MAX96717_96722_CLKFREQ_27M_MFP2        2U
#define MAX96717_96722_CLKFREQ_25M_MFP4        3U
#define MAX96717_96722_CLKFREQ_24M_MFP4        4U

/* definition for PinID of AmbaSbrg_Max96717_96722_SetGpioOutput */
#define MAX96717_96722_GPIO_PIN_0              0U
#define MAX96717_96722_GPIO_PIN_1              1U
#define MAX96717_96722_GPIO_PIN_2              2U
#define MAX96717_96722_GPIO_PIN_3              3U
#define MAX96717_96722_GPIO_PIN_4              4U
#define MAX96717_96722_GPIO_PIN_5              5U
#define MAX96717_96722_GPIO_PIN_6              6U
#define MAX96717_96722_GPIO_PIN_7              7U
#define MAX96717_96722_GPIO_PIN_8              8U
#define MAX96717_96722_GPIO_PIN_9              9U
#define MAX96717_96722_GPIO_PIN_10             10U
#define MAX96717_96722_NUM_GPIO_PIN            11U

/* ChipID */
#define MAX96717_A_ID                          0U  /* Serializer A */
#define MAX96717_B_ID                          1U  /* Serializek B */
#define MAX96722_ID                            2U  /* Deserializer */

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
} MAX96717_96722_GPIO_A_REG_s;

typedef struct {
    UINT32  GpioTxId:               5;     /* [4:0] GPIO ID for pin while transmiting */
    UINT32  OutType:                1;     /* [5] Driver type selection. 0 - Open-drain, 1 - Push-pull */
    UINT32  PullUpDnSel:            2;     /* [7:6] Buffer pull up/down configuration. 0 - None, 1 - Pull-up, 2 - Pull-down, 3 - Reserved */
    UINT32  Reserved:               24;    /* for misrac compliant */
} MAX96717_96722_GPIO_B_REG_s;

typedef struct {
    UINT8 SerGpio;
    UINT8 DesGpio;
} MAX96717_96722_FRAMESYNC_SEL_s;

typedef struct {
    UINT16  EnabledLinkID;                           /* Enabled Link ID */
    UINT8   SensorSlaveID;                           /* Sensor slave ID */
    UINT8   SensorAliasID[MAX96722_NUM_RX_PORT];     /* Sensor alias ID */
    UINT8   NumDataBits;                             /* Pixel data bit depth */
    UINT8   DataType;
    UINT8   CSIRxLaneNum;
    UINT8   CSITxLaneNum[MAX96722_NUM_TX_PORT];
    UINT8   CSITxSpeed[MAX96722_NUM_TX_PORT];         /* in multiple of 100MHz, up to 25 */
    UINT8   EnableExtFrameSync;
    MAX96717_96722_FRAMESYNC_SEL_s FrameSyncSel;
} MAX96717_96722_SERDES_CONFIG_s;

UINT32 Max96717_96722_Init(UINT32 VinID, UINT16 EnabledLinkID);
UINT32 Max96717_96722_DeInit(UINT32 VinID);
UINT32 Max96717_96722_Config(UINT32 VinID, const MAX96717_96722_SERDES_CONFIG_s *pSerdesConfig);
UINT32 Max96717_96722_SetSensorClk(UINT32 VinID, UINT32 ChipID, UINT32 RClkOut);
UINT32 Max96717_96722_SetGpioOutput(UINT32 VinID, UINT32 ChipID, UINT32 PinID, UINT8 Level);
UINT32 Max96717_96722_RegisterWrite(UINT32 VinID, UINT32 ChipID, UINT16 Addr, UINT8 Data);
UINT32 Max96717_96722_RegisterRead(UINT32 VinID, UINT32 ChipID, UINT8 Addr, UINT8* pData);

#endif /* MAX96717_96722_H */
