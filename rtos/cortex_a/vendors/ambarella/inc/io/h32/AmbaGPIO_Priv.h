/**
 *  @file AmbaGPIO_Priv.h
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
 *  @details Common Definitions & Constants for GPIO APIs
 *
 */

#ifndef AMBA_GPIO_PRIV_H
#define AMBA_GPIO_PRIV_H

/* Group 0 */
#define GPIO_PIN_0          0U
#define GPIO_PIN_1          1U
#define GPIO_PIN_2          2U
#define GPIO_PIN_3          3U
#define GPIO_PIN_4          4U
#define GPIO_PIN_5          5U
#define GPIO_PIN_6          6U
#define GPIO_PIN_7          7U
#define GPIO_PIN_8          8U
#define GPIO_PIN_9          9U
#define GPIO_PIN_10         10U
#define GPIO_PIN_11         11U
#define GPIO_PIN_12         12U
#define GPIO_PIN_13         13U
#define GPIO_PIN_14         14U
#define GPIO_PIN_15         15U
#define GPIO_PIN_16         16U
#define GPIO_PIN_17         17U
#define GPIO_PIN_18         18U
#define GPIO_PIN_19         19U
#define GPIO_PIN_20         20U
#define GPIO_PIN_21         21U
#define GPIO_PIN_22         22U
#define GPIO_PIN_23         23U
#define GPIO_PIN_24         24U
#define GPIO_PIN_25         25U
#define GPIO_PIN_26         26U
#define GPIO_PIN_27         27U
#define GPIO_PIN_28         28U
#define GPIO_PIN_29         29U
#define GPIO_PIN_30         30U
#define GPIO_PIN_31         31U
/* Group 1 */
#define GPIO_PIN_32         32U
#define GPIO_PIN_33         33U
#define GPIO_PIN_34         34U
#define GPIO_PIN_35         35U
#define GPIO_PIN_36         36U
#define GPIO_PIN_37         37U
#define GPIO_PIN_38         38U
#define GPIO_PIN_39         39U
#define GPIO_PIN_40         40U
#define GPIO_PIN_41         41U
#define GPIO_PIN_42         42U
#define GPIO_PIN_43         43U
#define GPIO_PIN_44         44U
#define GPIO_PIN_45         45U
#define GPIO_PIN_46         46U
#define GPIO_PIN_47         47U
#define GPIO_PIN_48         48U
#define GPIO_PIN_49         49U
#define GPIO_PIN_50         50U
#define GPIO_PIN_51         51U
#define GPIO_PIN_52         52U
#define GPIO_PIN_53         53U
#define GPIO_PIN_54         54U
#define GPIO_PIN_55         55U
#define GPIO_PIN_56         56U
#define GPIO_PIN_57         57U
#define GPIO_PIN_58         58U
#define GPIO_PIN_59         59U
#define GPIO_PIN_60         60U
#define GPIO_PIN_61         61U
#define GPIO_PIN_62         62U
#define GPIO_PIN_63         63U
/* Group 2 */
#define GPIO_PIN_64         64U
#define GPIO_PIN_65         65U
#define GPIO_PIN_66         66U
#define GPIO_PIN_67         67U
#define GPIO_PIN_68         68U
#define GPIO_PIN_69         69U
#define GPIO_PIN_70         70U
#define GPIO_PIN_71         71U
#define GPIO_PIN_72         72U
#define GPIO_PIN_73         73U
#define GPIO_PIN_74         74U
#define GPIO_PIN_75         75U
#define GPIO_PIN_76         76U
#define GPIO_PIN_77         77U
#define GPIO_PIN_78         78U
#define GPIO_PIN_79         79U
#define GPIO_PIN_80         80U
#define GPIO_PIN_81         81U
#define GPIO_PIN_82         82U
#define GPIO_PIN_83         83U
#define GPIO_PIN_84         84U
#define GPIO_PIN_85         85U
#define GPIO_PIN_86         86U
#define AMBA_NUM_GPIO_PIN   87U  /* Total Number of GPIO logical pins */

/* Audio */
#define GPIO_PIN_CLK_AU                 (((UINT32)1U << 12U) | GPIO_PIN_86)  /* CLK_AU */

/* Digital Microphone */
#define GPIO_PIN_3_DMIC_CLK             (((UINT32)4U << 12U) | GPIO_PIN_3)   /* Clock pin of the Digital Microphone */
#define GPIO_PIN_4_DMIC_DATA_IN         (((UINT32)4U << 12U) | GPIO_PIN_4)   /* Data input pin of the Digital Microphone */
#define GPIO_PIN_8_DMIC_CLK             (((UINT32)5U << 12U) | GPIO_PIN_8)   /* Clock pin of the Digital Microphone */
#define GPIO_PIN_9_DMIC_DATA_IN         (((UINT32)5U << 12U) | GPIO_PIN_9)   /* Data input pin of the Digital Microphone */

#define GPIO_PIN_15_DMIC_CLK            (((UINT32)3U << 12U) | GPIO_PIN_15)  /* Clock pin of the Digital Microphone */
#define GPIO_PIN_16_DMIC_DATA_IN        (((UINT32)3U << 12U) | GPIO_PIN_16)  /* Data input pin of the Digital Microphone */
#define GPIO_PIN_17_DMIC_DATA_OUT       (((UINT32)3U << 12U) | GPIO_PIN_17)  /* Data output pin of the Digital Microphone */
#define GPIO_PIN_22_DMIC_DATA_OUT       (((UINT32)3U << 12U) | GPIO_PIN_22)  /* Data output pin of the Digital Microphone */

#define GPIO_PIN_DEBOUNCE_GPIO0         0U
#define GPIO_PIN_DEBOUNCE_GPIO1         0U

/* Ethernet */
#define GPIO_PIN_19_ENET_ALT_TXEN       (((UINT32)3U << 12U) | GPIO_PIN_19)  /* Ethernet RMII Transmit Enable */
#define GPIO_PIN_20_ENET_ALT_TXD0       (((UINT32)3U << 12U) | GPIO_PIN_20)  /* Ethernet RMII Transmit Data Bit 0 */
#define GPIO_PIN_21_ENET_ALT_TXD1       (((UINT32)3U << 12U) | GPIO_PIN_21)  /* Ethernet RMII Transmit Data Bit 1 */
#define GPIO_PIN_24_ENET_ALT_RXD0       (((UINT32)3U << 12U) | GPIO_PIN_24)  /* Ethernet RMII Receive Data Bit 0 */
#define GPIO_PIN_25_ENET_ALT_RXD1       (((UINT32)3U << 12U) | GPIO_PIN_25)  /* Ethernet RMII Receive Data Bit 1 */
#define GPIO_PIN_28_ENET_ALT_RXDV       (((UINT32)3U << 12U) | GPIO_PIN_28)  /* Ethernet RMII Receive Data Valid */
#define GPIO_PIN_29_ENET_ALT_MDC        (((UINT32)3U << 12U) | GPIO_PIN_29)  /* Ethernet RMII Management Data Clock */
#define GPIO_PIN_30_ENET_ALT_MDIO       (((UINT32)3U << 12U) | GPIO_PIN_30)  /* Ethernet RMII Management Data */
#define GPIO_PIN_31_ENET_ALT_PTP_PPS    (((UINT32)3U << 12U) | GPIO_PIN_31)  /* Ethernet RMII PTP PPS */
#define GPIO_PIN_32_ENET_ALT_REF_CLK0   (((UINT32)3U << 12U) | GPIO_PIN_32)  /* Ethernet RMII 1st reference clock (50 MHz) */
#define GPIO_PIN_33_ENET_ALT_REF_CLK1   (((UINT32)3U << 12U) | GPIO_PIN_33)  /* Ethernet RMII 2nd reference clock (50 MHz) */
#define GPIO_PIN_34_ENET_ALT_EXT_OSC    (((UINT32)3U << 12U) | GPIO_PIN_34)  /* Ethernet RMII external clock source */
#define GPIO_PIN_35_ENET_ALT_REF_CLK1   (((UINT32)3U << 12U) | GPIO_PIN_35)  /* Ethernet RMII 2nd reference clock (50 MHz) */

#define GPIO_PIN_19_ENET_TXEN           (((UINT32)4U << 12U) | GPIO_PIN_19)  /* Ethernet RGMII Transmit Enable */
#define GPIO_PIN_20_ENET_TXD0           (((UINT32)4U << 12U) | GPIO_PIN_20)  /* Ethernet RGMII Transmit Data Bit 0 */
#define GPIO_PIN_21_ENET_TXD1           (((UINT32)4U << 12U) | GPIO_PIN_21)  /* Ethernet RGMII Transmit Data Bit 1 */
#define GPIO_PIN_22_ENET_TXD2           (((UINT32)4U << 12U) | GPIO_PIN_22)  /* Ethernet RGMII Transmit Data Bit 2 */
#define GPIO_PIN_23_ENET_TXD3           (((UINT32)4U << 12U) | GPIO_PIN_23)  /* Ethernet RGMII Transmit Data Bit 3 */
#define GPIO_PIN_24_ENET_RXD0           (((UINT32)4U << 12U) | GPIO_PIN_24)  /* Ethernet RGMII Receive Data Bit 0 */
#define GPIO_PIN_25_ENET_RXD1           (((UINT32)4U << 12U) | GPIO_PIN_25)  /* Ethernet RGMII Receive Data Bit 1 */
#define GPIO_PIN_26_ENET_RXD2           (((UINT32)4U << 12U) | GPIO_PIN_26)  /* Ethernet RGMII Receive Data Bit 2 */
#define GPIO_PIN_27_ENET_RXD3           (((UINT32)4U << 12U) | GPIO_PIN_27)  /* Ethernet RGMII Receive Data Bit 3 */
#define GPIO_PIN_28_ENET_RXDV           (((UINT32)4U << 12U) | GPIO_PIN_28)  /* Ethernet RGMII Receive Data Valid */
#define GPIO_PIN_29_ENET_MDC            (((UINT32)4U << 12U) | GPIO_PIN_29)  /* Ethernet RGMII Management Data Clock */
#define GPIO_PIN_30_ENET_MDIO           (((UINT32)4U << 12U) | GPIO_PIN_30)  /* Ethernet RGMII Management Data */
#define GPIO_PIN_31_ENET_PTP_PPS        (((UINT32)4U << 12U) | GPIO_PIN_31)  /* Ethernet RGMII PTP PPS */
#define GPIO_PIN_32_ENET_CLK_RX         (((UINT32)4U << 12U) | GPIO_PIN_32)  /* Ethernet RGMII 1st reference clock (50 MHz) */
#define GPIO_PIN_33_ENET_GTX_CLK        (((UINT32)4U << 12U) | GPIO_PIN_33)  /* Ethernet RGMII Clock signal for gigabit TX signals (125 MHz) */
#define GPIO_PIN_34_ENET_EXT_OSC        (((UINT32)4U << 12U) | GPIO_PIN_34)  /* Ethernet RGMII external clock source */
#define GPIO_PIN_35_ENET_REF_CLK1       (((UINT32)4U << 12U) | GPIO_PIN_35)  /* Ethernet RGMII 2nd reference clock (50 MHz) */

#define GPIO_PIN_29_ENET_AHB_MDC        (((UINT32)5U << 12U) | GPIO_PIN_29)  /* Ethernet SGMII Management Data Clock */
#define GPIO_PIN_30_ENET_AHB_MDIO       (((UINT32)5U << 12U) | GPIO_PIN_30)  /* Ethernet SGMII Management Data */
#define GPIO_PIN_31_ENET_ALT_REF_CLK1   (((UINT32)5U << 12U) | GPIO_PIN_31)  /* Ethernet SGMII 2nd reference clock (50 MHz) */

/* HDMI */
#define GPIO_PIN_64_HDMI_HPD            (((UINT32)1U << 12U) | GPIO_PIN_64)  /* HPD pin of the HDMI source side */
#define GPIO_PIN_65_HDMI_CEC            (((UINT32)1U << 12U) | GPIO_PIN_65)  /* CEC pin of the HDMI source side */

/* I2C */
#define GPIO_PIN_15_I2C0_CLK            (((UINT32)4U << 12U) | GPIO_PIN_15)  /* Clock pin of the 1st I2C master port */
#define GPIO_PIN_14_I2C0_DATA           (((UINT32)4U << 12U) | GPIO_PIN_14)  /* Data pin of the 1st I2C master port */
#define GPIO_PIN_24_I2C0_CLK            (((UINT32)2U << 12U) | GPIO_PIN_24)  /* Clock pin of the 1st I2C master port */
#define GPIO_PIN_25_I2C0_DATA           (((UINT32)2U << 12U) | GPIO_PIN_25)  /* Data pin of the 1st I2C master port */
#define GPIO_PIN_61_I2C0_CLK            (((UINT32)3U << 12U) | GPIO_PIN_61)  /* Clock pin of the 1st I2C master port */
#define GPIO_PIN_62_I2C0_DATA           (((UINT32)3U << 12U) | GPIO_PIN_62)  /* Data pin of the 1st I2C master port */

#define GPIO_PIN_3_I2C1_CLK             (((UINT32)2U << 12U) | GPIO_PIN_3)   /* Clock pin of the 2nd I2C master port */
#define GPIO_PIN_4_I2C1_DATA            (((UINT32)2U << 12U) | GPIO_PIN_4)   /* Data pin of the 2nd I2C master port */
#define GPIO_PIN_79_I2C1_CLK            (((UINT32)3U << 12U) | GPIO_PIN_79)  /* Clock pin of the 2nd I2C master port */
#define GPIO_PIN_80_I2C1_DATA           (((UINT32)3U << 12U) | GPIO_PIN_80)  /* Data pin of the 2nd I2C master port */

#define GPIO_PIN_1_I2C2_CLK             (((UINT32)2U << 12U) | GPIO_PIN_1)   /* Clock pin of the 3rd I2C master port */
#define GPIO_PIN_5_I2C2_DATA            (((UINT32)2U << 12U) | GPIO_PIN_5)   /* Data pin of the 3rd I2C master port */
#define GPIO_PIN_81_I2C2_CLK            (((UINT32)3U << 12U) | GPIO_PIN_81)  /* Clock pin of the 3rd I2C master port */
#define GPIO_PIN_82_I2C2_DATA           (((UINT32)3U << 12U) | GPIO_PIN_82)  /* Data pin of the 3rd I2C master port */
#define GPIO_PIN_83_I2C2_CLK            (((UINT32)1U << 12U) | GPIO_PIN_83)  /* Clock pin of the 3rd I2C master port */
#define GPIO_PIN_84_I2C2_DATA           (((UINT32)1U << 12U) | GPIO_PIN_84)  /* Data pin of the 3rd I2C master port */

#define GPIO_PIN_8_I2C3_CLK             (((UINT32)2U << 12U) | GPIO_PIN_8)   /* Clock pin of the 4th I2C master port */
#define GPIO_PIN_9_I2C3_DATA            (((UINT32)2U << 12U) | GPIO_PIN_9)   /* Data pin of the 4th I2C master port */

#define GPIO_PIN_8_I2C_SLAVE_CLK        (((UINT32)3U << 12U) | GPIO_PIN_8)   /* Clock pin of the I2C slave port */
#define GPIO_PIN_9_I2C_SLAVE_DATA       (((UINT32)3U << 12U) | GPIO_PIN_9)   /* Data pin of the I2C slave port */
#define GPIO_PIN_61_I2C_SLAVE_CLK       (((UINT32)4U << 12U) | GPIO_PIN_61)  /* Clock pin of the I2C slave port */
#define GPIO_PIN_62_I2C_SLAVE_DATA      (((UINT32)4U << 12U) | GPIO_PIN_62)  /* Data pin of the I2C slave port */
#define GPIO_PIN_81_I2C_SLAVE_CLK       (((UINT32)5U << 12U) | GPIO_PIN_81)  /* Clock pin of the I2C slave port */
#define GPIO_PIN_82_I2C_SLAVE_DATA      (((UINT32)5U << 12U) | GPIO_PIN_82)  /* Data pin of the I2C slave port */

/* I2S */
#define GPIO_PIN_15_I2S0_CLK            (((UINT32)1U << 12U) | GPIO_PIN_15)
#define GPIO_PIN_16_I2S0_SI_0           (((UINT32)1U << 12U) | GPIO_PIN_16)  /* Serial data input pin of the I2S controller */
#define GPIO_PIN_17_I2S0_SO_0           (((UINT32)1U << 12U) | GPIO_PIN_17)  /* Serial data output pin of the I2S controller */
#define GPIO_PIN_18_I2S0_WS             (((UINT32)1U << 12U) | GPIO_PIN_18)  /* Word select pin of the I2S controller */

#define GPIO_PIN_10_I2S0_SI_0           (((UINT32)2U << 12U) | GPIO_PIN_10)  /* Serial data input pin of the I2S controller */

/* IR */
#define GPIO_PIN_0_IR_RX                (((UINT32)1U << 12U) | GPIO_PIN_0)   /* Infrared interface input pin */
#define GPIO_PIN_62_IR_RX               (((UINT32)5U << 12U) | GPIO_PIN_62)  /* Infrared interface input pin */

/* NAND (SPI) */
#define GPIO_PIN_36_NAND_SPI_CLK        (((UINT32)2U << 12U) | GPIO_PIN_36)  /* SPI Clock for NAND flash */
#define GPIO_PIN_37_NAND_SPI_CS         (((UINT32)2U << 12U) | GPIO_PIN_37)  /* SPI Slave Select for NAND flash */
#define GPIO_PIN_42_NAND_SPI_DATA0      (((UINT32)2U << 12U) | GPIO_PIN_42)  /* Data[0] for NAND flash */
#define GPIO_PIN_43_NAND_SPI_DATA1      (((UINT32)2U << 12U) | GPIO_PIN_43)  /* Data[1] for NAND flash */
#define GPIO_PIN_44_NAND_SPI_DATA2      (((UINT32)2U << 12U) | GPIO_PIN_44)  /* Data[2] for NAND flash */
#define GPIO_PIN_45_NAND_SPI_DATA3      (((UINT32)2U << 12U) | GPIO_PIN_45)  /* Data[3] for NAND flash */

#define GPIO_PIN_54_NAND_SPI_CLK        (((UINT32)4U << 12U) | GPIO_PIN_54)  /* SPI Clock for NAND flash */
#define GPIO_PIN_55_NAND_SPI_CS         (((UINT32)4U << 12U) | GPIO_PIN_55)  /* SPI Slave Select for NAND flash */
#define GPIO_PIN_56_NAND_SPI_DATA0      (((UINT32)4U << 12U) | GPIO_PIN_56)  /* Data[0] for NAND flash */
#define GPIO_PIN_57_NAND_SPI_DATA1      (((UINT32)4U << 12U) | GPIO_PIN_57)  /* Data[1] for NAND flash */
#define GPIO_PIN_58_NAND_SPI_DATA2      (((UINT32)4U << 12U) | GPIO_PIN_58)  /* Data[2] for NAND flash */
#define GPIO_PIN_59_NAND_SPI_DATA3      (((UINT32)4U << 12U) | GPIO_PIN_59)  /* Data[3] for NAND flash */

/* NOR (SPI) */
#define GPIO_PIN_36_NOR_SPI_CLK         (((UINT32)3U << 12U) | GPIO_PIN_36)  /* SPI Clock for NOR flash */
#define GPIO_PIN_42_NOR_SPI_DATA0       (((UINT32)3U << 12U) | GPIO_PIN_42)  /* Data[0] for NOR flash */
#define GPIO_PIN_43_NOR_SPI_DATA1       (((UINT32)3U << 12U) | GPIO_PIN_43)  /* Data[1] for NOR flash */
#define GPIO_PIN_44_NOR_SPI_DATA2       (((UINT32)3U << 12U) | GPIO_PIN_44)  /* Data[2] for NOR flash */
#define GPIO_PIN_45_NOR_SPI_DATA3       (((UINT32)3U << 12U) | GPIO_PIN_45)  /* Data[3] for NOR flash */
#define GPIO_PIN_37_NOR_SPI_EN0         (((UINT32)3U << 12U) | GPIO_PIN_37)  /* SPI Slave Select 0 for NOR flash */
#define GPIO_PIN_47_NOR_SPI_EN1         (((UINT32)3U << 12U) | GPIO_PIN_47)  /* SPI Slave Select 1 for NOR flash */
#define GPIO_PIN_48_NOR_SPI_EN2         (((UINT32)3U << 12U) | GPIO_PIN_48)  /* SPI Slave Select 2 for NOR flash */
#define GPIO_PIN_49_NOR_SPI_EN3         (((UINT32)3U << 12U) | GPIO_PIN_49)  /* SPI Slave Select 3 for NOR flash */
#define GPIO_PIN_50_NOR_SPI_EN4         (((UINT32)3U << 12U) | GPIO_PIN_50)  /* SPI Slave Select 4 for NOR flash */
#define GPIO_PIN_51_NOR_SPI_EN5         (((UINT32)3U << 12U) | GPIO_PIN_51)  /* SPI Slave Select 5 for NOR flash */
#define GPIO_PIN_52_NOR_SPI_EN6         (((UINT32)3U << 12U) | GPIO_PIN_52)  /* SPI Slave Select 6 for NOR flash */
#define GPIO_PIN_53_NOR_SPI_EN7         (((UINT32)3U << 12U) | GPIO_PIN_53)  /* SPI Slave Select 7 for NOR flash */

#define GPIO_PIN_54_NOR_SPI_CLK         (((UINT32)5U << 12U) | GPIO_PIN_54)  /* SPI Clock for NOR flash */
#define GPIO_PIN_55_NOR_SPI_EN0         (((UINT32)5U << 12U) | GPIO_PIN_55)  /* SPI Slave Select 0 for NOR flash */
#define GPIO_PIN_56_NOR_SPI_DATA0       (((UINT32)5U << 12U) | GPIO_PIN_56)  /* Data[0] for NOR flash */
#define GPIO_PIN_57_NOR_SPI_DATA1       (((UINT32)5U << 12U) | GPIO_PIN_57)  /* Data[1] for NOR flash */
#define GPIO_PIN_58_NOR_SPI_DATA2       (((UINT32)5U << 12U) | GPIO_PIN_58)  /* Data[2] for NOR flash */
#define GPIO_PIN_59_NOR_SPI_DATA3       (((UINT32)5U << 12U) | GPIO_PIN_59)  /* Data[3] for NOR flash */

/* OTP */
#define GPIO_PIN_35_VPP                 (((UINT32)1U << 12U) | GPIO_PIN_35)  /* Enable pin for OTP VPP  */

/* PWM */
#define GPIO_PIN_12_PWM0                (((UINT32)1U << 12U) | GPIO_PIN_12)  /* Output pin of the PWM controller 0 */
#define GPIO_PIN_13_PWM1                (((UINT32)1U << 12U) | GPIO_PIN_13)  /* Output pin of the PWM controller 1 */
#define GPIO_PIN_14_PWM2                (((UINT32)1U << 12U) | GPIO_PIN_14)  /* Output pin of the PWM controller 2 */
#define GPIO_PIN_0_PWM3                 (((UINT32)3U << 12U) | GPIO_PIN_0)   /* Output pin of the PWM controller 3 */
#define GPIO_PIN_1_PWM4                 (((UINT32)3U << 12U) | GPIO_PIN_1)   /* Output pin of the PWM controller 4 */
#define GPIO_PIN_2_PWM5                 (((UINT32)3U << 12U) | GPIO_PIN_2)   /* Output pin of the PWM controller 5 */
#define GPIO_PIN_3_PWM6                 (((UINT32)3U << 12U) | GPIO_PIN_3)   /* Output pin of the PWM controller 6 */
#define GPIO_PIN_4_PWM7                 (((UINT32)3U << 12U) | GPIO_PIN_4)   /* Output pin of the PWM controller 7 */
#define GPIO_PIN_19_PWM8                (((UINT32)2U << 12U) | GPIO_PIN_19)  /* Output pin of the PWM controller 8 */
#define GPIO_PIN_20_PWM9                (((UINT32)2U << 12U) | GPIO_PIN_20)  /* Output pin of the PWM controller 9 */
#define GPIO_PIN_21_PWM10               (((UINT32)2U << 12U) | GPIO_PIN_21)  /* Output pin of the PWM controller 10 */
#define GPIO_PIN_22_PWM11               (((UINT32)2U << 12U) | GPIO_PIN_22)  /* Output pin of the PWM controller 11 */

#define GPIO_PIN_5_PWM0                 (((UINT32)5U << 12U) | GPIO_PIN_5)   /* Output pin of the PWM controller 0 */
#define GPIO_PIN_23_PWM0                (((UINT32)2U << 12U) | GPIO_PIN_23)  /* Output pin of the PWM controller 0 */

#define GPIO_PIN_27_PWM3                (((UINT32)2U << 12U) | GPIO_PIN_27)  /* Output pin of the PWM controller 3 */
#define GPIO_PIN_28_PWM4                (((UINT32)2U << 12U) | GPIO_PIN_28)  /* Output pin of the PWM controller 4 */
#define GPIO_PIN_29_PWM5                (((UINT32)2U << 12U) | GPIO_PIN_29)  /* Output pin of the PWM controller 5 */
#define GPIO_PIN_30_PWM6                (((UINT32)2U << 12U) | GPIO_PIN_30)  /* Output pin of the PWM controller 6 */
#define GPIO_PIN_31_PWM7                (((UINT32)2U << 12U) | GPIO_PIN_31)  /* Output pin of the PWM controller 7 */
#define GPIO_PIN_32_PWM8                (((UINT32)2U << 12U) | GPIO_PIN_32)  /* Output pin of the PWM controller 8 */
#define GPIO_PIN_33_PWM9                (((UINT32)2U << 12U) | GPIO_PIN_33)  /* Output pin of the PWM controller 9 */

#define GPIO_PIN_77_PWM7                (((UINT32)5U << 12U) | GPIO_PIN_77)  /* Output pin of the PWM controller 7 */
#define GPIO_PIN_78_PWM8                (((UINT32)5U << 12U) | GPIO_PIN_78)  /* Output pin of the PWM controller 8 */
#define GPIO_PIN_79_PWM9                (((UINT32)5U << 12U) | GPIO_PIN_79)  /* Output pin of the PWM controller 9 */

/* SD0/SD */
#define GPIO_PIN_38_SD0_CLK             (((UINT32)2U << 12U) | GPIO_PIN_38)  /* Clock pin of SD port 0 */
#define GPIO_PIN_39_SD0_CMD             (((UINT32)2U << 12U) | GPIO_PIN_39)  /* Command pin of SD port 0 */
#define GPIO_PIN_40_SD0_CD              (((UINT32)2U << 12U) | GPIO_PIN_40)  /* Card detect pin of SD port 0 */
#define GPIO_PIN_41_SD0_WP              (((UINT32)2U << 12U) | GPIO_PIN_41)  /* Write protect pin of SD port 0 */
#define GPIO_PIN_46_SD0_DATA0           (((UINT32)2U << 12U) | GPIO_PIN_46)  /* Data pin 0 of SD port 0 */
#define GPIO_PIN_47_SD0_DATA1           (((UINT32)2U << 12U) | GPIO_PIN_47)  /* Data pin 1 of SD port 0 */
#define GPIO_PIN_48_SD0_DATA2           (((UINT32)2U << 12U) | GPIO_PIN_48)  /* Data pin 2 of SD port 0 */
#define GPIO_PIN_49_SD0_DATA3           (((UINT32)2U << 12U) | GPIO_PIN_49)  /* Data pin 3 of SD port 0 */
#define GPIO_PIN_50_SD0_DATA4           (((UINT32)2U << 12U) | GPIO_PIN_50)  /* Data pin 4 of SD port 0 */
#define GPIO_PIN_51_SD0_DATA5           (((UINT32)2U << 12U) | GPIO_PIN_51)  /* Data pin 5 of SD port 0 */
#define GPIO_PIN_52_SD0_DATA6           (((UINT32)2U << 12U) | GPIO_PIN_52)  /* Data pin 6 of SD port 0 */
#define GPIO_PIN_53_SD0_DATA7           (((UINT32)2U << 12U) | GPIO_PIN_53)  /* Data pin 7 of SD port 0 */
#define GPIO_PIN_63_SD0_RESET           (((UINT32)2U << 12U) | GPIO_PIN_63)  /* Reset pin of SD port 0 */
#define GPIO_PIN_85_SD0_HS_SEL          (((UINT32)1U << 12U) | GPIO_PIN_85)  /* High speed mode select of SD port 0 */

/* SD1/SDIO0 */
#define GPIO_PIN_54_SD1_CLK             (((UINT32)2U << 12U) | GPIO_PIN_54)  /* Clock pin of SD port 1 (SDIO0) */
#define GPIO_PIN_55_SD1_CMD             (((UINT32)2U << 12U) | GPIO_PIN_55)  /* Command pin of SD port 1 (SDIO0) */
#define GPIO_PIN_56_SD1_DATA0           (((UINT32)2U << 12U) | GPIO_PIN_56)  /* Data pin 0 of SD port 1 (SDIO0) */
#define GPIO_PIN_57_SD1_DATA1           (((UINT32)2U << 12U) | GPIO_PIN_57)  /* Data pin 1 of SD port 1 (SDIO0) */
#define GPIO_PIN_58_SD1_DATA2           (((UINT32)2U << 12U) | GPIO_PIN_58)  /* Data pin 2 of SD port 1 (SDIO0) */
#define GPIO_PIN_59_SD1_DATA3           (((UINT32)2U << 12U) | GPIO_PIN_59)  /* Data pin 3 of SD port 1 (SDIO0) */
#define GPIO_PIN_60_SD1_CD              (((UINT32)2U << 12U) | GPIO_PIN_60)  /* Card detect pin of SD port 1 (SDIO0) */
#define GPIO_PIN_61_SD1_WP              (((UINT32)2U << 12U) | GPIO_PIN_61)  /* Write protect pin of SD port 1 (SDIO0) */
#define GPIO_PIN_62_SD1_RESET           (((UINT32)2U << 12U) | GPIO_PIN_62)  /* Reset pin of SD port 1 (SDIO0) */
#define GPIO_PIN_11_SD1_HS_SEL          (((UINT32)3U << 12U) | GPIO_PIN_11)  /* High speed mode select of SD port 1 (SDIO0) */

/* SD2/SDIO1 */
#define GPIO_PIN_75_SD2_DATA0           (((UINT32)1U << 12U) | GPIO_PIN_75)  /* Data pin 0 of SD port 2 (SDIO1) */
#define GPIO_PIN_76_SD2_DATA1           (((UINT32)1U << 12U) | GPIO_PIN_76)  /* Data pin 1 of SD port 2 (SDIO1) */
#define GPIO_PIN_77_SD2_DATA2           (((UINT32)1U << 12U) | GPIO_PIN_77)  /* Data pin 2 of SD port 2 (SDIO1) */
#define GPIO_PIN_78_SD2_DATA3           (((UINT32)1U << 12U) | GPIO_PIN_78)  /* Data pin 3 of SD port 2 (SDIO1) */
#define GPIO_PIN_79_SD2_CLK             (((UINT32)1U << 12U) | GPIO_PIN_79)  /* Clock pin of SD port 2 (SDIO1) */
#define GPIO_PIN_80_SD2_CMD             (((UINT32)1U << 12U) | GPIO_PIN_80)  /* Command pin of SD port 2 (SDIO1) */
#define GPIO_PIN_81_SD2_CD              (((UINT32)1U << 12U) | GPIO_PIN_81)  /* Card detect pin of SD port 2 (SDIO1) */
#define GPIO_PIN_82_SD2_WP              (((UINT32)1U << 12U) | GPIO_PIN_82)  /* Write protect pin of SD port 2 (SDIO1) */
#define GPIO_PIN_10_SD2_HS_SEL          (((UINT32)3U << 12U) | GPIO_PIN_10)  /* High speed mode select of SD port 2 (SDIO1) */

/* SPI */
#define GPIO_PIN_1_SPI0_SCLK            (((UINT32)1U << 12U) | GPIO_PIN_1)   /* Serial Clock pin of the SPI master port 0 */
#define GPIO_PIN_2_SPI0_MOSI            (((UINT32)1U << 12U) | GPIO_PIN_2)   /* MOSI: TXD pin of the SPI master port 0 */
#define GPIO_PIN_3_SPI0_MISO            (((UINT32)1U << 12U) | GPIO_PIN_3)   /* MISO: RXD pin of the SPI master port 0 */
#define GPIO_PIN_4_SPI0_SS0             (((UINT32)1U << 12U) | GPIO_PIN_4)   /* The 1st Slave Select pin of the SPI master port 0 */
#define GPIO_PIN_5_SPI0_SS1             (((UINT32)1U << 12U) | GPIO_PIN_5)   /* The 2nd Slave Select pin of the SPI master port 0 */
#define GPIO_PIN_6_SPI0_SS2             (((UINT32)3U << 12U) | GPIO_PIN_6)   /* The 3rd Slave Select pin of the SPI master port 0 */
#define GPIO_PIN_7_SPI0_SS3             (((UINT32)3U << 12U) | GPIO_PIN_7)   /* The 4th Slave Select pin of the SPI master port 0 */
#define GPIO_PIN_12_SPI0_SS4            (((UINT32)4U << 12U) | GPIO_PIN_12)  /* The 5th Slave Select pin of the SPI master port 0 */
#define GPIO_PIN_13_SPI0_SS5            (((UINT32)4U << 12U) | GPIO_PIN_13)  /* The 6th Slave Select pin of the SPI master port 0 */
#define GPIO_PIN_75_SPI0_SCLK           (((UINT32)2U << 12U) | GPIO_PIN_75)  /* Serial Clock pin of the SPI master port 0 */
#define GPIO_PIN_76_SPI0_MOSI           (((UINT32)2U << 12U) | GPIO_PIN_76)  /* MOSI: TXD pin of the SPI master port 0 */
#define GPIO_PIN_77_SPI0_MISO           (((UINT32)2U << 12U) | GPIO_PIN_77)  /* MISO: RXD pin of the SPI master port 0 */
#define GPIO_PIN_78_SPI0_SS0            (((UINT32)2U << 12U) | GPIO_PIN_78)  /* The 1st Slave Select pin of the SPI master port 0 */
#define GPIO_PIN_35_SPI0_SS2            (((UINT32)5U << 12U) | GPIO_PIN_35)  /* The 3rd Slave Select pin of the SPI master port 0 */

#define GPIO_PIN_15_SPI1_SCLK           (((UINT32)2U << 12U) | GPIO_PIN_15)  /* Serial Clock pin of the SPI master port 1 */
#define GPIO_PIN_17_SPI1_MOSI           (((UINT32)2U << 12U) | GPIO_PIN_17)  /* MOSI: TXD pin of the SPI master port 1 */
#define GPIO_PIN_16_SPI1_MISO           (((UINT32)2U << 12U) | GPIO_PIN_16)  /* MISO: RXD pin of the SPI master port 1 */
#define GPIO_PIN_69_SPI1_SCLK           (((UINT32)2U << 12U) | GPIO_PIN_69)  /* Serial Clock pin of the SPI master port 1 */
#define GPIO_PIN_70_SPI1_MOSI           (((UINT32)2U << 12U) | GPIO_PIN_70)  /* MOSI: TXD pin of the SPI master port 1 */
#define GPIO_PIN_71_SPI1_MISO           (((UINT32)2U << 12U) | GPIO_PIN_71)  /* MISO: RXD pin of the SPI master port 1 */
#define GPIO_PIN_72_SPI1_SS0            (((UINT32)2U << 12U) | GPIO_PIN_72)  /* The 1st Slave Select pin of the SPI master port 1 */
#define GPIO_PIN_54_SPI1_SCLK           (((UINT32)3U << 12U) | GPIO_PIN_54)  /* Serial Clock pin of the SPI master port 1 */
#define GPIO_PIN_55_SPI1_MOSI           (((UINT32)3U << 12U) | GPIO_PIN_55)  /* MOSI: TXD pin of the SPI master port 1 */
#define GPIO_PIN_56_SPI1_MISO           (((UINT32)3U << 12U) | GPIO_PIN_56)  /* MISO: RXD pin of the SPI master port 1 */
#define GPIO_PIN_57_SPI1_SS0            (((UINT32)3U << 12U) | GPIO_PIN_57)  /* The 1st Slave Select pin of the SPI master port 1 */
#define GPIO_PIN_58_SPI1_SS1            (((UINT32)3U << 12U) | GPIO_PIN_58)  /* The 2nd Slave Select pin of the SPI master port 1 */
#define GPIO_PIN_59_SPI1_SS2            (((UINT32)3U << 12U) | GPIO_PIN_59)  /* The 3rd Slave Select pin of the SPI master port 1 */
#define GPIO_PIN_60_SPI1_SS3            (((UINT32)3U << 12U) | GPIO_PIN_60)  /* The 4th Slave Select pin of the SPI master port 1 */
#define GPIO_PIN_79_SPI1_SCLK           (((UINT32)2U << 12U) | GPIO_PIN_79)  /* Serial Clock pin of the SPI master port 1 */
#define GPIO_PIN_80_SPI1_MOSI           (((UINT32)2U << 12U) | GPIO_PIN_80)  /* MOSI: TXD pin of the SPI master port 1 */
#define GPIO_PIN_81_SPI1_MISO           (((UINT32)2U << 12U) | GPIO_PIN_81)  /* MISO: RXD pin of the SPI master port 1 */
#define GPIO_PIN_82_SPI1_SS0            (((UINT32)2U << 12U) | GPIO_PIN_82)  /* The 1st Slave Select pin of the SPI master port 1 */

#define GPIO_PIN_6_SPI2_SCLK            (((UINT32)1U << 12U) | GPIO_PIN_6)   /* Serial Clock pin of the SPI master port 2 */
#define GPIO_PIN_7_SPI2_MOSI            (((UINT32)1U << 12U) | GPIO_PIN_7)   /* MOSI: TXD pin of the SPI master port 2 */
#define GPIO_PIN_8_SPI2_MISO            (((UINT32)1U << 12U) | GPIO_PIN_8)   /* MISO: RXD pin of the SPI master port 2 */
#define GPIO_PIN_9_SPI2_SS0             (((UINT32)1U << 12U) | GPIO_PIN_9)   /* The 1st Slave Select pin of the SPI master port 2 */

#define GPIO_PIN_12_SPI3_SCLK           (((UINT32)5U << 12U) | GPIO_PIN_12)  /* Serial Clock pin of the SPI master port 3 */
#define GPIO_PIN_11_SPI3_MOSI           (((UINT32)5U << 12U) | GPIO_PIN_11)  /* MOSI: TXD pin of the SPI master port 3 */
#define GPIO_PIN_10_SPI3_MISO           (((UINT32)5U << 12U) | GPIO_PIN_10)  /* MISO: RXD pin of the SPI master port 3 */
#define GPIO_PIN_13_SPI3_SS0            (((UINT32)5U << 12U) | GPIO_PIN_13)  /* The 1st Slave Select pin of the SPI master port 3 */

#define GPIO_PIN_6_SPI_SLAVE_SCLK       (((UINT32)4U << 12U) | GPIO_PIN_6)   /* Serial Clock pin of the SPI slave port */
#define GPIO_PIN_7_SPI_SLAVE_MISO       (((UINT32)4U << 12U) | GPIO_PIN_7)   /* MISO: TXD pin of the SPI slave port */
#define GPIO_PIN_8_SPI_SLAVE_MOSI       (((UINT32)4U << 12U) | GPIO_PIN_8)   /* MOSI: RXD pin of the SPI slave port */
#define GPIO_PIN_9_SPI_SLAVE_SS         (((UINT32)4U << 12U) | GPIO_PIN_9)   /* Slave Select pin of the SPI slave port  */
#define GPIO_PIN_75_SPI_SLAVE_SCLK      (((UINT32)3U << 12U) | GPIO_PIN_75)  /* Serial Clock pin of the SPI slave port */
#define GPIO_PIN_76_SPI_SLAVE_MISO      (((UINT32)3U << 12U) | GPIO_PIN_76)  /* MISO: TXD pin of the SPI slave port */
#define GPIO_PIN_77_SPI_SLAVE_MOSI      (((UINT32)3U << 12U) | GPIO_PIN_77)  /* MOSI: RXD pin of the SPI slave port */
#define GPIO_PIN_78_SPI_SLAVE_SS        (((UINT32)3U << 12U) | GPIO_PIN_78)  /* Slave Select pin of the SPI slave port  */

/* Timer */
#define GPIO_PIN_0_TIMER0               (((UINT32)4U << 12U) | GPIO_PIN_0)   /* External clock source of interval timer 0 */
#define GPIO_PIN_73_TIMER0              (((UINT32)4U << 12U) | GPIO_PIN_73)  /* External clock source of interval timer 0 */

/* UART */
#define GPIO_PIN_10_UART_APB_RXD        (((UINT32)1U << 12U) | GPIO_PIN_10)  /* Receive Data pin of the UART_APB port 0 */
#define GPIO_PIN_11_UART_APB_TXD        (((UINT32)1U << 12U) | GPIO_PIN_11)  /* Transmit Data pin of the UART_APB port 0 */

#define GPIO_PIN_69_UART0_RXD           (((UINT32)1U << 12U) | GPIO_PIN_69)  /* Receive Data pin of the UART_AHB port 0 */
#define GPIO_PIN_70_UART0_TXD           (((UINT32)1U << 12U) | GPIO_PIN_70)  /* Transmit Data pin of the UART_AHB port 0 */
#define GPIO_PIN_71_UART0_CTS_N         (((UINT32)1U << 12U) | GPIO_PIN_71)  /* Clear-to-Send pin of the UART_AHB port 0 */
#define GPIO_PIN_72_UART0_RTS_N         (((UINT32)1U << 12U) | GPIO_PIN_72)  /* Request-to-Send pin of the UART_AHB port 0 */

#define GPIO_PIN_73_UART1_RXD           (((UINT32)1U << 12U) | GPIO_PIN_73)  /* Receive Data pin of the UART_AHB port 1 */
#define GPIO_PIN_74_UART1_TXD           (((UINT32)1U << 12U) | GPIO_PIN_74)  /* Transmit Data pin of the UART_AHB port 1 */
#define GPIO_PIN_62_UART1_CTS_N         (((UINT32)1U << 12U) | GPIO_PIN_62)  /* Clear-to-Send pin of the UART_AHB port 1 */
#define GPIO_PIN_63_UART1_RTS_N         (((UINT32)1U << 12U) | GPIO_PIN_63)  /* Request-to-Send pin of the UART_AHB port 1 */

#define GPIO_PIN_54_UART2_RXD           (((UINT32)1U << 12U) | GPIO_PIN_54)  /* Receive Data pin of the UART_AHB port 2 */
#define GPIO_PIN_55_UART2_TXD           (((UINT32)1U << 12U) | GPIO_PIN_55)  /* Transmit Data pin of the UART_AHB port 2 */
#define GPIO_PIN_56_UART2_CTS_N         (((UINT32)1U << 12U) | GPIO_PIN_56)  /* Clear-to-Send pin of the UART_AHB port 2 */
#define GPIO_PIN_57_UART2_RTS_N         (((UINT32)1U << 12U) | GPIO_PIN_57)  /* Request-to-Send pin of the UART_AHB port 2 */
#define GPIO_PIN_75_UART2_RXD           (((UINT32)4U << 12U) | GPIO_PIN_75)  /* Receive Data pin of the UART_AHB port 2 */
#define GPIO_PIN_76_UART2_TXD           (((UINT32)4U << 12U) | GPIO_PIN_76)  /* Transmit Data pin of the UART_AHB port 2 */
#define GPIO_PIN_77_UART2_CTS_N         (((UINT32)4U << 12U) | GPIO_PIN_77)  /* Clear-to-Send pin of the UART_AHB port 2 */
#define GPIO_PIN_78_UART2_RTS_N         (((UINT32)4U << 12U) | GPIO_PIN_78)  /* Request-to-Send pin of the UART_AHB port 2 */

#define GPIO_PIN_58_UART3_RXD           (((UINT32)1U << 12U) | GPIO_PIN_58)  /* Receive Data pin of the UART_AHB port 3 */
#define GPIO_PIN_59_UART3_TXD           (((UINT32)1U << 12U) | GPIO_PIN_59)  /* Transmit Data pin of the UART_AHB port 3 */
#define GPIO_PIN_60_UART3_CTS_N         (((UINT32)1U << 12U) | GPIO_PIN_60)  /* Clear-to-Send pin of the UART_AHB port 3 */
#define GPIO_PIN_61_UART3_RTS_N         (((UINT32)1U << 12U) | GPIO_PIN_61)  /* Request-to-Send pin of the UART_AHB port 3 */
#define GPIO_PIN_79_UART3_RXD           (((UINT32)4U << 12U) | GPIO_PIN_79)  /* Receive Data pin of the UART_AHB port 3 */
#define GPIO_PIN_80_UART3_TXD           (((UINT32)4U << 12U) | GPIO_PIN_80)  /* Transmit Data pin of the UART_AHB port 3 */
#define GPIO_PIN_81_UART3_CTS_N         (((UINT32)4U << 12U) | GPIO_PIN_81)  /* Clear-to-Send pin of the UART_AHB port 3 */
#define GPIO_PIN_82_UART3_RTS_N         (((UINT32)4U << 12U) | GPIO_PIN_82)  /* Request-to-Send pin of the UART_AHB port 3 */

/* USB */
#define GPIO_PIN_10_USB0_EHCI_OC        (((UINT32)4U << 12U) | GPIO_PIN_10)  /* Over-Current pin of the USB EHCI port 0 */
#define GPIO_PIN_11_USB0_EHCI_PWR       (((UINT32)4U << 12U) | GPIO_PIN_11)  /* USB EHCI port 0 power enable */
#define GPIO_PIN_69_USB0_EHCI_OC        (((UINT32)3U << 12U) | GPIO_PIN_69)  /* Over-Current pin of the USB EHCI port 0 */
#define GPIO_PIN_71_USB0_EHCI_PWR       (((UINT32)3U << 12U) | GPIO_PIN_71)  /* USB EHCI port 0 power enable */
#define GPIO_PIN_75_USB0_EHCI_OC        (((UINT32)5U << 12U) | GPIO_PIN_75)  /* Over-Current pin of the USB EHCI port 0 */
#define GPIO_PIN_76_USB0_EHCI_PWR       (((UINT32)5U << 12U) | GPIO_PIN_76)  /* USB EHCI port 0 power enable */

/* Video Input */
#define GPIO_PIN_4_VIN0_MASTER_HSYNC    (((UINT32)5U << 12U) | GPIO_PIN_4)   /* VIN0/VIN Master HSYNC */
#define GPIO_PIN_67_VIN0_MASTER_HSYNC   (((UINT32)1U << 12U) | GPIO_PIN_67)  /* VIN0/VIN Master HSYNC */
#define GPIO_PIN_68_VIN0_MASTER_VSYNC   (((UINT32)1U << 12U) | GPIO_PIN_68)  /* VIN0/VIN Master VSYNC */

#define GPIO_PIN_67_VIN1_MASTER_HSYNC   (((UINT32)2U << 12U) | GPIO_PIN_67)  /* VIN1/PIP Master HSYNC */
#define GPIO_PIN_68_VIN1_MASTER_VSYNC   (((UINT32)2U << 12U) | GPIO_PIN_68)  /* VIN1/PIP Master VSYNC */
#define GPIO_PIN_74_VIN1_MASTER_HSYNC   (((UINT32)2U << 12U) | GPIO_PIN_74)  /* VIN1/PIP Master HSYNC */
#define GPIO_PIN_73_VIN1_MASTER_VSYNC   (((UINT32)2U << 12U) | GPIO_PIN_73)  /* VIN1/PIP Master VSYNC */

#define GPIO_PIN_12_VIN_STRIG_0         (((UINT32)2U << 12U) | GPIO_PIN_12)  /* VIN strig 0 */
#define GPIO_PIN_13_VIN_STRIG_1         (((UINT32)2U << 12U) | GPIO_PIN_13)  /* VIN strig 1 */
#define GPIO_PIN_20_VIN_STRIG_0         (((UINT32)5U << 12U) | GPIO_PIN_20)  /* VIN strig 0 */
#define GPIO_PIN_21_VIN_STRIG_1         (((UINT32)5U << 12U) | GPIO_PIN_21)  /* VIN strig 1 */
#define GPIO_PIN_36_VIN_STRIG_0         (((UINT32)1U << 12U) | GPIO_PIN_36)  /* VIN strig 0 */
#define GPIO_PIN_37_VIN_STRIG_1         (((UINT32)1U << 12U) | GPIO_PIN_37)  /* VIN strig 1 */

/* Video Output Display */
#define GPIO_PIN_19_VD0_OUT_0           (((UINT32)1U << 12U) | GPIO_PIN_19)  /* Digital video output data pin 0 */
#define GPIO_PIN_20_VD0_OUT_1           (((UINT32)1U << 12U) | GPIO_PIN_20)  /* Digital video output data pin 1 */
#define GPIO_PIN_21_VD0_OUT_2           (((UINT32)1U << 12U) | GPIO_PIN_21)  /* Digital video output data pin 2 */
#define GPIO_PIN_22_VD0_OUT_3           (((UINT32)1U << 12U) | GPIO_PIN_22)  /* Digital video output data pin 3 */
#define GPIO_PIN_23_VD0_OUT_4           (((UINT32)1U << 12U) | GPIO_PIN_23)  /* Digital video output data pin 4 */
#define GPIO_PIN_24_VD0_OUT_5           (((UINT32)1U << 12U) | GPIO_PIN_24)  /* Digital video output data pin 5 */
#define GPIO_PIN_25_VD0_OUT_6           (((UINT32)1U << 12U) | GPIO_PIN_25)  /* Digital video output data pin 6 */
#define GPIO_PIN_26_VD0_OUT_7           (((UINT32)1U << 12U) | GPIO_PIN_26)  /* Digital video output data pin 7 */
#define GPIO_PIN_27_VD0_OUT_8           (((UINT32)1U << 12U) | GPIO_PIN_27)  /* Digital video output data pin 8 */
#define GPIO_PIN_28_VD0_OUT_9           (((UINT32)1U << 12U) | GPIO_PIN_28)  /* Digital video output data pin 9 */
#define GPIO_PIN_29_VD0_OUT_10          (((UINT32)1U << 12U) | GPIO_PIN_29)  /* Digital video output data pin 10 */
#define GPIO_PIN_30_VD0_OUT_11          (((UINT32)1U << 12U) | GPIO_PIN_30)  /* Digital video output data pin 11 */
#define GPIO_PIN_31_VD0_OUT_12          (((UINT32)1U << 12U) | GPIO_PIN_31)  /* Digital video output data pin 12 */
#define GPIO_PIN_32_VD0_OUT_13          (((UINT32)1U << 12U) | GPIO_PIN_32)  /* Digital video output data pin 13 */
#define GPIO_PIN_33_VD0_OUT_14          (((UINT32)1U << 12U) | GPIO_PIN_33)  /* Digital video output data pin 14 */
#define GPIO_PIN_34_VD0_OUT_15          (((UINT32)1U << 12U) | GPIO_PIN_34)  /* Digital video output data pin 15 */
#define GPIO_PIN_73_VD0_CLK             (((UINT32)3U << 12U) | GPIO_PIN_73)  /* Digital video output clock pin */
#define GPIO_PIN_74_VD0_VSYNC           (((UINT32)3U << 12U) | GPIO_PIN_74)  /* Digital video output vsync pin */
#define GPIO_PIN_70_VD0_HSYNC           (((UINT32)3U << 12U) | GPIO_PIN_70)  /* Digital video output hsync pin */
#define GPIO_PIN_72_VD0_HVLD            (((UINT32)3U << 12U) | GPIO_PIN_72)  /* Digital video output hvld pin */

/* WDT */
#define GPIO_PIN_0_WDT_RST              (((UINT32)2U << 12U) | GPIO_PIN_0)   /* WDT extend reset */
#define GPIO_PIN_5_WDT_RST              (((UINT32)3U << 12U) | GPIO_PIN_5)   /* WDT extend reset */
#define GPIO_PIN_7_WDT_RST              (((UINT32)2U << 12U) | GPIO_PIN_7)   /* WDT extend reset */
#define GPIO_PIN_35_WDT_RST             (((UINT32)2U << 12U) | GPIO_PIN_35)  /* WDT extend reset */
#define GPIO_PIN_66_WDT_RST             (((UINT32)2U << 12U) | GPIO_PIN_66)  /* WDT extend reset */
#define GPIO_PIN_80_WDT_RST             (((UINT32)5U << 12U) | GPIO_PIN_80)  /* WDT extend reset */

#define AMBA_GPIO_GROUP0                0x0U    /* GPIO pin 0~31    */
#define AMBA_GPIO_GROUP1                0x1U    /* GPIO pin 32~63   */
#define AMBA_GPIO_GROUP2                0x2U    /* GPIO pin 64~95   */
#define AMBA_NUM_GPIO_GROUP             0x3U

#endif /* AMBA_GPIO_PRIV_H */
