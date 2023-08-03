/**
 *  @file AmbaSbrg_MAX96717_96712.h
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
 *  @details Definitions & Constants for MAX96717 & MAX96712 APIs
 *
 */

#ifndef MAX96717_96712_H
#define MAX96717_96712_H

#ifndef AMBA_ERROR_CODE_H
#include "AmbaErrorCode.h"
#endif

/* ERROR CODE */
#define MAX96717_96712_ERR_NONE                 (0U)
#define MAX96717_96712_ERR_INVALID_API          (BRIDGE_ERR_BASE + 1U)
#define MAX96717_96712_ERR_ARG                  (BRIDGE_ERR_BASE + 2U)
#define MAX96717_96712_ERR_SERDES_LINK          (BRIDGE_ERR_BASE + 3U)
#define MAX96717_96712_ERR_COMMUNICATE          (BRIDGE_ERR_BASE + 4U)    /* I2C to communicate with MAX96717_96712 fail*/
#define MAX96717_96712_ERR_UNKNOWN_CHIP_VERSION (BRIDGE_ERR_BASE + 5U)    /* unknown MAX96717_96712 chip version */

/* Print module */
#define MAX96717_96712_MODULE_ID                 ((UINT16)(BRIDGE_ERR_BASE >> 16U))

/* I2C slave Addr */
/* Need to modify per Hardware configuration */
#define MX07_IDC_ADDR_MAX96712_0        0x52U   /* device address of MAX96712 (on VIN0) */
#define MX07_IDC_ADDR_MAX96712_4        0x52U   /* device address of MAX96712 (on VIN4) */
#define MX07_IDC_ADDR_MAX96712_8        0x52U   /* device address of MAX96712 (on VIN8) */
#define MX07_IDC_ADDR_MAX96712_11       0x52U   /* device address of MAX96712 (on VIN11) */
#define MX07_IDC_ADDR_MAX96717_DEFAULT  0x80U   /* default device address of MAX96717 set by CFG0 pin */
#define MX07_IDC_ADDR_MAX96717_DEFAULT2 0x84U   /* default device address 2 of MAX96717 set by CFG0 pin */

/* Software configurable */
/* Need to modify if the defined address conflicts with the other device on the same i2c bus */
#define MX07_IDC_ADDR_MAX96717_0_A      0x7EU   /* device address of MAX96717 on PortA of MAX96712 (VIN0) */
#define MX07_IDC_ADDR_MAX96717_0_B      0x82U   /* device address of MAX96717 on PortB of MAX96712 (VIN0) */
#define MX07_IDC_ADDR_MAX96717_0_C      0x86U   /* device address of MAX96717 on PortC of MAX96712 (VIN0) */
#define MX07_IDC_ADDR_MAX96717_0_D      0x88U   /* device address of MAX96717 on PortD of MAX96712 (VIN0) */
#define MX07_IDC_ADDR_MAX96717_4_A      0x7EU   /* device address of MAX96717 on PortA of MAX96712 (VIN4) */
#define MX07_IDC_ADDR_MAX96717_4_B      0x82U   /* device address of MAX96717 on PortB of MAX96712 (VIN4) */
#define MX07_IDC_ADDR_MAX96717_4_C      0x86U   /* device address of MAX96717 on PortC of MAX96712 (VIN4) */
#define MX07_IDC_ADDR_MAX96717_4_D      0x88U   /* device address of MAX96717 on PortD of MAX96712 (VIN4) */
#define MX07_IDC_ADDR_MAX96717_8_A      0x7EU   /* device address of MAX96717 on PortA of MAX96712 (VIN8) */
#define MX07_IDC_ADDR_MAX96717_8_B      0x82U   /* device address of MAX96717 on PortB of MAX96712 (VIN8) */
#define MX07_IDC_ADDR_MAX96717_8_C      0x86U   /* device address of MAX96717 on PortC of MAX96712 (VIN8) */
#define MX07_IDC_ADDR_MAX96717_8_D      0x88U   /* device address of MAX96717 on PortD of MAX96712 (VIN8) */
#define MX07_IDC_ADDR_MAX96717_11_A     0x7EU   /* device address of MAX96717 on PortA of MAX96712 (VIN11) */
#define MX07_IDC_ADDR_MAX96717_11_B     0x82U   /* device address of MAX96717 on PortB of MAX96712 (VIN11) */
#define MX07_IDC_ADDR_MAX96717_11_C     0x86U   /* device address of MAX96717 on PortC of MAX96712 (VIN11) */
#define MX07_IDC_ADDR_MAX96717_11_D     0x88U   /* device address of MAX96717 on PortD of MAX96712 (VIN11) */
#define MX07_IDC_ADDR_MAX96717_BCST     0x90U

/* GPIO Base Addr */
#define MAX96717_GPIO_BASE_ADDR     0x2BEU
#define MAX96712_GPIO_BASE_ADDR     0x300U
#define MAX96712_M_GPIO_BASE_ADDR   0x337U //receive external FSYNC signal for each link

/* Port number */
#define MAX96712_NUM_RX_PORT                   4U     /* 0: GMSL2 Link A,  1: GMSL2 Link B,  2: GMSL2 Link C,  3: GMSL2 Link D  */
#define MAX96712_NUM_TX_PORT                   4U     /* 0: CSI Tx port A, 1: CSI Tx port B, 2: CSI Tx port C, 3: CSI Tx port D */

/* definition for RCLK_OUT_FREQ of MAX96717_96712_SetSensorClk */
#define MAX96717_96712_RCLK_OUT_FREQ_24M    0U
#define MAX96717_96712_RCLK_OUT_FREQ_25M    1U
#define MAX96717_96712_RCLK_OUT_FREQ_27M    2U

/* definition for MAX96717 PinID of AmbaSbrg_MAX96717_96712_SetGpioOutput */
#define MAX96717_GPIO_PIN_0     0U
#define MAX96717_GPIO_PIN_1     1U
#define MAX96717_GPIO_PIN_2     2U
#define MAX96717_GPIO_PIN_3     3U
#define MAX96717_GPIO_PIN_4     4U
#define MAX96717_GPIO_PIN_5     5U
#define MAX96717_GPIO_PIN_6     6U
#define MAX96717_GPIO_PIN_7     7U
#define MAX96717_GPIO_PIN_8     8U
#define MAX96717_GPIO_PIN_9     9U
#define MAX96717_GPIO_PIN_10    10U
#define MAX96717_NUM_GPIO_PIN   11U

/* definition for MAX96712 PinID of AmbaSbrg_MAX96717_96712_SetGpioOutput */
#define MAX96712_GPIO_PIN_0     0U
#define MAX96712_GPIO_PIN_1     1U
#define MAX96712_GPIO_PIN_2     2U
#define MAX96712_GPIO_PIN_3     3U
#define MAX96712_GPIO_PIN_4     4U
#define MAX96712_GPIO_PIN_5     5U
#define MAX96712_GPIO_PIN_6     6U
#define MAX96712_GPIO_PIN_7     7U
#define MAX96712_GPIO_PIN_8     8U
#define MAX96712_GPIO_PIN_9     9U
#define MAX96712_GPIO_PIN_10    10U
#define MAX96712_GPIO_PIN_11    11U
#define MAX96712_GPIO_PIN_12    12U
#define MAX96712_GPIO_PIN_13    13U
#define MAX96712_GPIO_PIN_14    14U
#define MAX96712_GPIO_PIN_15    15U
#define MAX96712_GPIO_PIN_16    16U
#define MAX96712_NUM_GPIO_PIN   17U

/* ChipID */
#define MAX96717_A_ID       0U  /* Serializer A */
#define MAX96717_B_ID       1U  /* Serializer B */
#define MAX96717_C_ID       2U  /* Serializer C */
#define MAX96717_D_ID       3U  /* Serializer D */
#define MAX96712_ID         4U  /* Deserializer */

/* Deserializer Link ID */
#define MAX96712_LINK_A     1U
#define MAX96712_LINK_B     2U
#define MAX96712_LINK_C     4U
#define MAX96712_LINK_D     8U

/* GMSL2 GPIO Forwarding Configs */
#define MAX96717_96712_GPIO_DIR_SER2DES     0U
#define MAX96717_96712_GPIO_DIR_DES2SER     1U
#define MAX96717_96712_GPIO_LOW_PRIO        0U
#define MAX96717_96712_GPIO_HIGH_PRIO       1U
#define MAX96717_96712_GPIO_RESISTOR_40K    0U
#define MAX96717_96712_GPIO_RESISTOR_1M     1U
#define MAX96717_96712_GPIO_OPEN_DRAIN      0U
#define MAX96717_96712_GPIO_PUSH_PULL       1U
#define MAX96717_96712_GPIO_PULL_UP         1U
#define MAX96717_96712_GPIO_PULL_DN         2U

/* GMSL2 Forward Link Bit Rate */
#define MAX96717_96712_FORWARD_LINK_3GBPS   0x1U    /* 3Gbps + Twisted-Pair */
#define MAX96717_96712_FORWARD_LINK_6GBPS   0x2U    /* 6Gbps + COAX */

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
} MAX96717_96712_GPIO_A_REG_s;

typedef struct {
    UINT32  GpioTxId:               5;     /* [4:0] GPIO ID for pin while transmiting */
    UINT32  OutType:                1;     /* [5] Driver type selection. 0 - Open-drain, 1 - Push-pull */
    UINT32  PullUpDnSel:            2;     /* [7:6] Buffer pull up/down configuration. 0 - None, 1 - Pull-up, 2 - Pull-down, 3 - Reserved */
    UINT32  Reserved:               24;    /* for misrac compliant */
} MAX96717_96712_GPIO_B_REG_s;

typedef struct {
    UINT32  GpioRxId:               5;     /* [4:0] GPIO ID for pin while receiving */
    UINT32  Rsvd:                   2;     /* [6:5] Reserved */
    UINT32  OvrResCfg:              1;     /* [7] Override non-GPIO port function IO setting */
    UINT32  Reserved:               24;    /* for misrac compliant */
} MAX96717_96712_GPIO_C_REG_s;

typedef struct {
    UINT32  GpioTxId:               5;     /* [4:0] GPIO ID for pin while receiving */
    UINT32  GpioTxEn:               1;     /* [5] 1 = enable GPIO TX source for GMSL2 transmission */
    UINT32  TxCompEn:               1;     /* [6] Jitter minimization compesation enable */
    UINT32  TxPrio:                 1;     /* [7] Priority for GPIO scheduling. 0 - Low priority, 1 - High priority */
    UINT32  Reserved:               24;    /* for misrac compliant */
} MAX96717_96712_GPIO_MULTI_REG_s;

typedef struct {
    UINT8 SerGpio;      /* GPIO selection for serializer MFP */
    UINT8 DesGpio;      /* GPIO selection for deserializer MFP */
    UINT8 Direction;    /* Communication direction, 0 - SER-to-DES, 1 - DES-to-SER */
    UINT8 Priority;     /* Priority for GPIO scheduling, 0 - Low priority, 1 - High priority */
    UINT8 ResistorCfg;  /* Resistor pull-up/pull-down strength, 0 - 40kohm, 1 - 1Mohm  */
    UINT8 OutType;      /* Output driver type selection, 0 - Open-drain, 1 - Push-pull */
    UINT8 PullUpDnSel;  /* Input buffer pull up/down resistor configuration, 0 - None, 1 - Pull-up, 2 - Pull-down */
} MAX96717_96712_GPIO_FORWARDING_CONFIG_s;

typedef struct {
    UINT8 SerGpio;
    UINT8 DesGpio;
} MAX96717_96712_FRAMESYNC_SEL_s;

typedef struct {
    UINT16  EnabledLinkID;                           /* Enabled Link ID */
    UINT8   SensorSlaveID;                           /* Sensor slave ID */
    UINT8   SensorBCID;                              /* Sensor broadcast ID */
    UINT8   SensorAliasID[MAX96712_NUM_RX_PORT];     /* Sensor alias ID */
    UINT8   NumDataBits;                             /* Pixel data bit depth */
    UINT8   DataType;
    UINT8   CSIRxLaneNum;
    UINT8   CSIRxLaneMapping[4];                    /* Map Sensor Lane X/X/X/X to Serializer Lane 0/1/2/3 */
    UINT8   CSIRxLanePolarity[4];                   /* Serializer Lane 0/1/2/3 Polarity Setting. 0: Normal, 1: Inverse */
    UINT8   CSITxLaneNum[MAX96712_NUM_TX_PORT];
    UINT8   CSITxSpeed[MAX96712_NUM_TX_PORT];         /* in multiple of 100MHz, up to 25 */
    UINT8   EnableExtFrameSync;
    MAX96717_96712_FRAMESYNC_SEL_s FrameSyncSel;
} MAX96717_96712_SERDES_CONFIG_s;

UINT32 Max96717_96712_Init(UINT32 VinID, UINT16 EnabledLinkID, UINT8 ForwardLinkRate);
UINT32 Max96717_96712_DeInit(UINT32 VinID);
UINT32 Max96717_96712_Config(UINT32 VinID, const MAX96717_96712_SERDES_CONFIG_s *pSerdesConfig);
UINT32 Max96717_96712_SetSensorClk(UINT32 VinID, UINT32 ChipID, UINT32 RClkGpio, UINT32 RClkOutFreq);
UINT32 Max96717_96712_SetGpioOutput(UINT32 VinID, UINT32 ChipID, UINT32 PinID, UINT8 Level);
UINT32 Max96717_96712_GPIO_Forwarding(UINT32 VinID, UINT16 EnabledLinkID, const MAX96717_96712_GPIO_FORWARDING_CONFIG_s *pGpioFwding);
UINT32 Max96717_96712_RegisterWrite(UINT32 VinID, UINT32 ChipID, UINT16 Addr, UINT8 Data);
UINT32 Max96717_96712_RegisterRead(UINT32 VinID, UINT32 ChipID, UINT16 Addr, UINT8* pData);

#endif /* MAX96717_96712_H */
