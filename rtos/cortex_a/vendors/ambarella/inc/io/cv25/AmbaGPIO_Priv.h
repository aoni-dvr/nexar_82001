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
#define GPIO_PIN_87         87U
#define GPIO_PIN_88         88U
#define GPIO_PIN_89         89U
#define GPIO_PIN_90         90U
#define GPIO_PIN_91         91U
#define GPIO_PIN_92         92U
#define GPIO_PIN_93         93U
#define GPIO_PIN_94         94U
#define GPIO_PIN_95         95U
/* Group 3 */
#define GPIO_PIN_96         96U
#define GPIO_PIN_97         97U
#define GPIO_PIN_98         98U
#define GPIO_PIN_99         99U
#define GPIO_PIN_100        100U
#define GPIO_PIN_101        101U
#define GPIO_PIN_102        102U
#define GPIO_PIN_103        103U
#define GPIO_PIN_104        104U
#define GPIO_PIN_105        105U
#define GPIO_PIN_106        106U
#define GPIO_PIN_107        107U
#define GPIO_PIN_108        108U
#define GPIO_PIN_109        109U
#define GPIO_PIN_110        110U
#define GPIO_PIN_111        111U
#define GPIO_PIN_112        112U
#define GPIO_PIN_113        113U
#define GPIO_PIN_114        114U
#define GPIO_PIN_115        115U
#define GPIO_PIN_116        116U
#define GPIO_PIN_117        117U
#define GPIO_PIN_118        118U
#define GPIO_PIN_119        119U
#define GPIO_PIN_120        120U
#define GPIO_PIN_121        121U
#define GPIO_PIN_122        122U
#define GPIO_PIN_123        123U
#define GPIO_PIN_124        124U
#define GPIO_PIN_125        125U
#define GPIO_PIN_126        126U
#define GPIO_PIN_127        127U

#define AMBA_NUM_GPIO_PIN   128U  /* Total Number of GPIO logical pins */

/* Audio */
#define GPIO_PIN_108_CLK_AU             (((UINT32)1U << 12U) | GPIO_PIN_108) /* CLK_AU */
#define GPIO_PIN_108_CLK_AU3            (((UINT32)2U << 12U) | GPIO_PIN_108) /* CLK_AU3 */

/* CAN Bus */
#define GPIO_PIN_95_CAN0_RX             (((UINT32)1U << 12U) | GPIO_PIN_95)  /* RX pin of the CAN bus controller-0 */
#define GPIO_PIN_96_CAN0_TX             (((UINT32)1U << 12U) | GPIO_PIN_96)  /* TX pin of the CAN bus controller-0 */

/* Debounced GPIO */
#define GPIO_PIN_DEBOUNCE_GPIO0         GPIO_PIN_106_DEBOUNCE_GPIO0
#define GPIO_PIN_DEBOUNCE_GPIO1         GPIO_PIN_107_DEBOUNCE_GPIO1
#define GPIO_PIN_106_DEBOUNCE_GPIO0     (((UINT32)9U << 12U) | GPIO_PIN_106) /* Debounce GPIO 0 */
#define GPIO_PIN_107_DEBOUNCE_GPIO1     (((UINT32)9U << 12U) | GPIO_PIN_107) /* Debounce GPIO 1 */

/* Digital Microphone */
#define GPIO_PIN_20_DMIC_CLK            (((UINT32)3U << 12U) | GPIO_PIN_20)  /* Clock pin of the Digital Microphone */
#define GPIO_PIN_21_DMIC_DATA_IN        (((UINT32)3U << 12U) | GPIO_PIN_21)  /* Data pin of the Digital Microphone */
#define GPIO_PIN_22_DMIC_CLK            (((UINT32)3U << 12U) | GPIO_PIN_22)  /* Clock pin of the Digital Microphone */
#define GPIO_PIN_23_DMIC_DATA_IN        (((UINT32)3U << 12U) | GPIO_PIN_23)  /* Data pin of the Digital Microphone */
#define GPIO_PIN_26_DMIC_CLK            (((UINT32)4U << 12U) | GPIO_PIN_26)  /* Clock pin of the Digital Microphone */
#define GPIO_PIN_25_DMIC_DATA_IN        (((UINT32)4U << 12U) | GPIO_PIN_25)  /* Data pin of the Digital Microphone */

#define GPIO_PIN_26_DMIC_DATA_OUT       (((UINT32)2U << 12U) | GPIO_PIN_26)  /* Data pin of the Digital Microphone */
#define GPIO_PIN_31_DMIC_DATA_OUT       (((UINT32)3U << 12U) | GPIO_PIN_31)  /* Data pin of the Digital Microphone */

/* Ethernet */
#define GPIO_PIN_28_ENET_ALT_TXEN       (((UINT32)3U << 12U) | GPIO_PIN_28)  /* Ethernet RMII Transmit Enable */
#define GPIO_PIN_29_ENET_ALT_TXD0       (((UINT32)3U << 12U) | GPIO_PIN_29)  /* Ethernet RMII Transmit Data Bit 0 */
#define GPIO_PIN_30_ENET_ALT_TXD1       (((UINT32)3U << 12U) | GPIO_PIN_30)  /* Ethernet RMII Transmit Data Bit 1 */
#define GPIO_PIN_33_ENET_ALT_RXD0       (((UINT32)3U << 12U) | GPIO_PIN_33)  /* Ethernet RMII Receive Data Bit 0 */
#define GPIO_PIN_34_ENET_ALT_RXD1       (((UINT32)3U << 12U) | GPIO_PIN_34)  /* Ethernet RMII Receive Data Bit 1 */
#define GPIO_PIN_37_ENET_ALT_RXDV       (((UINT32)3U << 12U) | GPIO_PIN_37)  /* Ethernet RMII Receive Data Valid */
#define GPIO_PIN_38_ENET_ALT_MDC        (((UINT32)3U << 12U) | GPIO_PIN_38)  /* Ethernet RMII Management Data Clock */
#define GPIO_PIN_39_ENET_ALT_MDIO       (((UINT32)3U << 12U) | GPIO_PIN_39)  /* Ethernet RMII Management Data */
#define GPIO_PIN_40_ENET_ALT_PTP_PPS    (((UINT32)3U << 12U) | GPIO_PIN_40)  /* Ethernet RMII PTP PPS */
#define GPIO_PIN_41_ENET_ALT_REF_CLK0   (((UINT32)3U << 12U) | GPIO_PIN_41)  /* Ethernet RMII 1st reference clock (50 MHz) */
#define GPIO_PIN_42_ENET_ALT_REF_CLK1   (((UINT32)3U << 12U) | GPIO_PIN_42)  /* Ethernet RMII 2nd reference clock (50 MHz) */
#define GPIO_PIN_43_ENET_ALT_EXT_OSC    (((UINT32)3U << 12U) | GPIO_PIN_43)  /* Ethernet RMII external clock source */
#define GPIO_PIN_44_ENET_ALT_REF_CLK1   (((UINT32)3U << 12U) | GPIO_PIN_44)  /* Ethernet RMII 2nd reference clock (50 MHz) */
#define GPIO_PIN_68_ENET_ALT_GTX_CLK    (((UINT32)3U << 12U) | GPIO_PIN_68)  /* Ethernet RMII Clock signal for gigabit TX signals (125 MHz) */
#define GPIO_PIN_69_ENET_ALT_EXT_OSC    (((UINT32)3U << 12U) | GPIO_PIN_69)  /* Ethernet RMII external clock source */

#define GPIO_PIN_28_ENET_TXEN           (((UINT32)4U << 12U) | GPIO_PIN_28)  /* Ethernet RGMII Transmit Enable */
#define GPIO_PIN_29_ENET_TXD0           (((UINT32)4U << 12U) | GPIO_PIN_29)  /* Ethernet RGMII Transmit Data Bit 0 */
#define GPIO_PIN_30_ENET_TXD1           (((UINT32)4U << 12U) | GPIO_PIN_30)  /* Ethernet RGMII Transmit Data Bit 1 */
#define GPIO_PIN_31_ENET_TXD2           (((UINT32)4U << 12U) | GPIO_PIN_31)  /* Ethernet RGMII Transmit Data Bit 2 */
#define GPIO_PIN_32_ENET_TXD3           (((UINT32)4U << 12U) | GPIO_PIN_32)  /* Ethernet RGMII Transmit Data Bit 3 */
#define GPIO_PIN_33_ENET_RXD0           (((UINT32)4U << 12U) | GPIO_PIN_33)  /* Ethernet RGMII Receive Data Bit 0 */
#define GPIO_PIN_34_ENET_RXD1           (((UINT32)4U << 12U) | GPIO_PIN_34)  /* Ethernet RGMII Receive Data Bit 1 */
#define GPIO_PIN_35_ENET_RXD2           (((UINT32)4U << 12U) | GPIO_PIN_35)  /* Ethernet RGMII Receive Data Bit 2 */
#define GPIO_PIN_36_ENET_RXD3           (((UINT32)4U << 12U) | GPIO_PIN_36)  /* Ethernet RGMII Receive Data Bit 3 */
#define GPIO_PIN_37_ENET_RXDV           (((UINT32)4U << 12U) | GPIO_PIN_37)  /* Ethernet RGMII Receive Data Valid */
#define GPIO_PIN_38_ENET_MDC            (((UINT32)4U << 12U) | GPIO_PIN_38)  /* Ethernet RGMII Management Data Clock */
#define GPIO_PIN_39_ENET_MDIO           (((UINT32)4U << 12U) | GPIO_PIN_39)  /* Ethernet RGMII Management Data */
#define GPIO_PIN_40_ENET_PTP_PPS        (((UINT32)4U << 12U) | GPIO_PIN_40)  /* Ethernet RGMII PTP PPS */
#define GPIO_PIN_41_ENET_CLK_RX         (((UINT32)4U << 12U) | GPIO_PIN_41)  /* Ethernet RGMII 1st reference clock (50 MHz) */
#define GPIO_PIN_42_ENET_GTX_CLK        (((UINT32)4U << 12U) | GPIO_PIN_42)  /* Ethernet RGMII Clock signal for gigabit TX signals (125 MHz) */
#define GPIO_PIN_43_ENET_EXT_OSC        (((UINT32)4U << 12U) | GPIO_PIN_43)  /* Ethernet RGMII external clock source */
#define GPIO_PIN_44_ENET_REF_CLK1       (((UINT32)4U << 12U) | GPIO_PIN_44)  /* Ethernet RGMII 2nd reference clock (50 MHz) */

#define GPIO_PIN_38_ENET_AHB_MDC        (((UINT32)5U << 12U) | GPIO_PIN_38)  /* Ethernet SGMII Management Data Clock */
#define GPIO_PIN_39_ENET_AHB_MDIO       (((UINT32)5U << 12U) | GPIO_PIN_39)  /* Ethernet SGMII Management Data */

/* HDMI */
#define GPIO_PIN_82_HDMI_HPD            (((UINT32)1U << 12U) | GPIO_PIN_82)  /* HPD pin of the HDMI source side */
#define GPIO_PIN_83_HDMI_CEC            (((UINT32)1U << 12U) | GPIO_PIN_83)  /* CEC pin of the HDMI source side */

/* I2C */
#define GPIO_PIN_1_I2C0_CLK             (((UINT32)3U << 12U) | GPIO_PIN_1)   /* Clock pin of the 1st I2C master port */
#define GPIO_PIN_2_I2C0_DATA            (((UINT32)3U << 12U) | GPIO_PIN_2)   /* Data pin of the 1st I2C master port */
#define GPIO_PIN_15_I2C0_CLK            (((UINT32)3U << 12U) | GPIO_PIN_15)  /* Clock pin of the 1st I2C master port */
#define GPIO_PIN_16_I2C0_DATA           (((UINT32)3U << 12U) | GPIO_PIN_16)  /* Data pin of the 1st I2C master port */
#define GPIO_PIN_79_I2C0_CLK            (((UINT32)3U << 12U) | GPIO_PIN_79)  /* Clock pin of the 1st I2C master port */
#define GPIO_PIN_80_I2C0_DATA           (((UINT32)3U << 12U) | GPIO_PIN_80)  /* Data pin of the 1st I2C master port */
#define GPIO_PIN_95_I2C0_CLK            (((UINT32)2U << 12U) | GPIO_PIN_95)  /* Clock pin of the 1st I2C master port */
#define GPIO_PIN_96_I2C0_DATA           (((UINT32)2U << 12U) | GPIO_PIN_96)  /* Data pin of the 1st I2C master port */

#define GPIO_PIN_6_I2C1_CLK             (((UINT32)2U << 12U) | GPIO_PIN_6)   /* Clock pin of the 2nd I2C master port */
#define GPIO_PIN_7_I2C1_DATA            (((UINT32)2U << 12U) | GPIO_PIN_7)   /* Data pin of the 2nd I2C master port */
#define GPIO_PIN_101_I2C1_CLK           (((UINT32)3U << 12U) | GPIO_PIN_101) /* Clock pin of the 2nd I2C master port */
#define GPIO_PIN_102_I2C1_DATA          (((UINT32)3U << 12U) | GPIO_PIN_102) /* Data pin of the 2nd I2C master port */

#define GPIO_PIN_4_I2C2_CLK             (((UINT32)2U << 12U) | GPIO_PIN_4)   /* Clock pin of the 3rd I2C master port */
#define GPIO_PIN_8_I2C2_DATA            (((UINT32)2U << 12U) | GPIO_PIN_8)   /* Data pin of the 3rd I2C master port */
#define GPIO_PIN_103_I2C2_CLK           (((UINT32)3U << 12U) | GPIO_PIN_103) /* Clock pin of the 3rd I2C master port */
#define GPIO_PIN_104_I2C2_DATA          (((UINT32)3U << 12U) | GPIO_PIN_104) /* Data pin of the 3rd I2C master port */

#define GPIO_PIN_11_I2C3_CLK            (((UINT32)2U << 12U) | GPIO_PIN_11)  /* Clock pin of the 4th I2C master port */
#define GPIO_PIN_12_I2C3_DATA           (((UINT32)2U << 12U) | GPIO_PIN_12)  /* Data pin of the 4th I2C master port */

#define GPIO_PIN_1_I2C_SLAVE_CLK        (((UINT32)1U << 12U) | GPIO_PIN_1)   /* Clock pin of the I2C slave port */
#define GPIO_PIN_2_I2C_SLAVE_DATA       (((UINT32)1U << 12U) | GPIO_PIN_2)   /* Data pin of the I2C slave port */

/* I2S */
#define GPIO_PIN_22_I2S0_CLK            (((UINT32)1U << 12U) | GPIO_PIN_22)
#define GPIO_PIN_23_I2S0_SI_0           (((UINT32)1U << 12U) | GPIO_PIN_23)  /* Serial data input pin of the I2S controller */
#define GPIO_PIN_24_I2S0_SO_0           (((UINT32)1U << 12U) | GPIO_PIN_24)  /* Serial data output pin of the I2S controller */
#define GPIO_PIN_25_I2S0_SI_1           (((UINT32)1U << 12U) | GPIO_PIN_25)  /* Serial data input pin of the I2S controller */
#define GPIO_PIN_26_I2S0_SO_1           (((UINT32)1U << 12U) | GPIO_PIN_26)  /* Serial data output pin of the I2S controller */
#define GPIO_PIN_27_I2S0_WS             (((UINT32)1U << 12U) | GPIO_PIN_27)  /* Word select pin of the I2S controller */

#define GPIO_PIN_17_I2S0_SI_0           (((UINT32)2U << 12U) | GPIO_PIN_17)  /* Serial data input pin of the I2S controller */
#define GPIO_PIN_18_I2S0_SI_1           (((UINT32)2U << 12U) | GPIO_PIN_18)  /* Serial data input pin of the I2S controller */

/* IR */
#define GPIO_PIN_3_IR_RX                (((UINT32)1U << 12U) | GPIO_PIN_3)   /* Infrared interface input pin */

/* NAND */
#define GPIO_PIN_45_NAND_WP             (((UINT32)2U << 12U) | GPIO_PIN_45)  /* Write Protection for NAND flash */
#define GPIO_PIN_46_NAND_CE             (((UINT32)2U << 12U) | GPIO_PIN_46)  /* Chip Enable for NAND flash */
#define GPIO_PIN_47_NAND_RB             (((UINT32)2U << 12U) | GPIO_PIN_47)  /* Ready/Busy for NAND flash */
#define GPIO_PIN_52_NAND_RE             (((UINT32)2U << 12U) | GPIO_PIN_52)  /* Read Enable for NAND flash */
#define GPIO_PIN_53_NAND_WE             (((UINT32)2U << 12U) | GPIO_PIN_53)  /* Write Enable for NAND flash */
#define GPIO_PIN_54_NAND_ALE            (((UINT32)2U << 12U) | GPIO_PIN_54)  /* Address Latch Enable for NAND flash */
#define GPIO_PIN_55_NAND_DATA0          (((UINT32)2U << 12U) | GPIO_PIN_55)  /* Data[0] for NAND flash */
#define GPIO_PIN_56_NAND_DATA1          (((UINT32)2U << 12U) | GPIO_PIN_56)  /* Data[1] for NAND flash */
#define GPIO_PIN_57_NAND_DATA2          (((UINT32)2U << 12U) | GPIO_PIN_57)  /* Data[2] for NAND flash */
#define GPIO_PIN_58_NAND_DATA3          (((UINT32)2U << 12U) | GPIO_PIN_58)  /* Data[3] for NAND flash */
#define GPIO_PIN_59_NAND_DATA4          (((UINT32)2U << 12U) | GPIO_PIN_59)  /* Data[4] for NAND flash */
#define GPIO_PIN_60_NAND_DATA5          (((UINT32)2U << 12U) | GPIO_PIN_60)  /* Data[5] for NAND flash */
#define GPIO_PIN_61_NAND_DATA6          (((UINT32)2U << 12U) | GPIO_PIN_61)  /* Data[6] for NAND flash */
#define GPIO_PIN_62_NAND_DATA7          (((UINT32)2U << 12U) | GPIO_PIN_62)  /* Data[7] for NAND flash */
#define GPIO_PIN_63_NAND_CLE            (((UINT32)2U << 12U) | GPIO_PIN_63)  /* Command Latch Enable for NAND flash */

/* NAND (SPI) */
#define GPIO_PIN_45_NAND_SPI_CLK        (((UINT32)4U << 12U) | GPIO_PIN_45)  /* SPI Clock for NAND flash */
#define GPIO_PIN_59_NAND_SPI_DATA0      (((UINT32)4U << 12U) | GPIO_PIN_59)  /* Data[0] for NAND flash */
#define GPIO_PIN_60_NAND_SPI_DATA1      (((UINT32)4U << 12U) | GPIO_PIN_60)  /* Data[1] for NAND flash */
#define GPIO_PIN_61_NAND_SPI_DATA2      (((UINT32)4U << 12U) | GPIO_PIN_61)  /* Data[2] for NAND flash */
#define GPIO_PIN_62_NAND_SPI_DATA3      (((UINT32)4U << 12U) | GPIO_PIN_62)  /* Data[3] for NAND flash */
#define GPIO_PIN_55_NAND_SPI_EN0        (((UINT32)4U << 12U) | GPIO_PIN_55)  /* SPI Slave Select 0 for NAND flash */

/* NOR (SPI) */
#define GPIO_PIN_45_NOR_SPI_CLK         (((UINT32)3U << 12U) | GPIO_PIN_45)  /* SPI Clock for NOR flash */
#define GPIO_PIN_59_NOR_SPI_DATA0       (((UINT32)3U << 12U) | GPIO_PIN_59)  /* Data[0] for NOR flash */
#define GPIO_PIN_60_NOR_SPI_DATA1       (((UINT32)3U << 12U) | GPIO_PIN_60)  /* Data[1] for NOR flash */
#define GPIO_PIN_61_NOR_SPI_DATA2       (((UINT32)3U << 12U) | GPIO_PIN_61)  /* Data[2] for NOR flash */
#define GPIO_PIN_62_NOR_SPI_DATA3       (((UINT32)3U << 12U) | GPIO_PIN_62)  /* Data[3] for NOR flash */
#define GPIO_PIN_47_NOR_SPI_DATA4       (((UINT32)3U << 12U) | GPIO_PIN_47)  /* Data[4] for NOR flash */
#define GPIO_PIN_52_NOR_SPI_DATA5       (((UINT32)3U << 12U) | GPIO_PIN_52)  /* Data[5] for NOR flash */
#define GPIO_PIN_53_NOR_SPI_DATA6       (((UINT32)3U << 12U) | GPIO_PIN_53)  /* Data[6] for NOR flash */
#define GPIO_PIN_54_NOR_SPI_DATA7       (((UINT32)3U << 12U) | GPIO_PIN_54)  /* Data[7] for NOR flash */
#define GPIO_PIN_55_NOR_SPI_EN0         (((UINT32)3U << 12U) | GPIO_PIN_55)  /* SPI Slave Select 0 for NOR flash */
#define GPIO_PIN_56_NOR_SPI_EN1         (((UINT32)3U << 12U) | GPIO_PIN_56)  /* SPI Slave Select 1 for NOR flash */
#define GPIO_PIN_57_NOR_SPI_EN2         (((UINT32)3U << 12U) | GPIO_PIN_57)  /* SPI Slave Select 2 for NOR flash */
#define GPIO_PIN_58_NOR_SPI_EN3         (((UINT32)3U << 12U) | GPIO_PIN_58)  /* SPI Slave Select 3 for NOR flash */
#define GPIO_PIN_68_NOR_SPI_EN4         (((UINT32)3U << 12U) | GPIO_PIN_68)  /* SPI Slave Select 4 for NOR flash */
#define GPIO_PIN_69_NOR_SPI_EN5         (((UINT32)3U << 12U) | GPIO_PIN_69)  /* SPI Slave Select 5 for NOR flash */
#define GPIO_PIN_70_NOR_SPI_EN6         (((UINT32)3U << 12U) | GPIO_PIN_70)  /* SPI Slave Select 6 for NOR flash */
#define GPIO_PIN_71_NOR_SPI_EN7         (((UINT32)3U << 12U) | GPIO_PIN_71)  /* SPI Slave Select 7 for NOR flash */

/* PWM */
#define GPIO_PIN_44_PWM0                (((UINT32)1U << 12U) | GPIO_PIN_44)  /* Output pin of the PWM controller 0 (VD_PWM) */

#define GPIO_PIN_4_PWM3                 (((UINT32)3U << 12U) | GPIO_PIN_4)   /* Output pin of the PWM controller 3 */
#define GPIO_PIN_5_PWM4                 (((UINT32)3U << 12U) | GPIO_PIN_5)   /* Output pin of the PWM controller 4 */
#define GPIO_PIN_6_PWM5                 (((UINT32)3U << 12U) | GPIO_PIN_6)   /* Output pin of the PWM controller 5 */
#define GPIO_PIN_7_PWM6                 (((UINT32)3U << 12U) | GPIO_PIN_7)   /* Output pin of the PWM controller 6 */
#define GPIO_PIN_0_PWM7                 (((UINT32)3U << 12U) | GPIO_PIN_0)   /* Output pin of the PWM controller 7 */

#define GPIO_PIN_19_PWM0                (((UINT32)1U << 12U) | GPIO_PIN_19)  /* Output pin of the PWM controller 0 */
#define GPIO_PIN_20_PWM1                (((UINT32)1U << 12U) | GPIO_PIN_20)  /* Output pin of the PWM controller 1 */
#define GPIO_PIN_21_PWM2                (((UINT32)1U << 12U) | GPIO_PIN_21)  /* Output pin of the PWM controller 2 */

#define GPIO_PIN_32_PWM0                (((UINT32)2U << 12U) | GPIO_PIN_32)  /* Output pin of the PWM controller 0 */

#define GPIO_PIN_36_PWM3                (((UINT32)2U << 12U) | GPIO_PIN_36)  /* Output pin of the PWM controller 3 */
#define GPIO_PIN_37_PWM4                (((UINT32)2U << 12U) | GPIO_PIN_37)  /* Output pin of the PWM controller 4 */
#define GPIO_PIN_38_PWM5                (((UINT32)2U << 12U) | GPIO_PIN_38)  /* Output pin of the PWM controller 5 */
#define GPIO_PIN_39_PWM6                (((UINT32)2U << 12U) | GPIO_PIN_39)  /* Output pin of the PWM controller 6 */
#define GPIO_PIN_40_PWM7                (((UINT32)2U << 12U) | GPIO_PIN_40)  /* Output pin of the PWM controller 7 */
#define GPIO_PIN_41_PWM8                (((UINT32)2U << 12U) | GPIO_PIN_41)  /* Output pin of the PWM controller 8 */
#define GPIO_PIN_42_PWM9                (((UINT32)2U << 12U) | GPIO_PIN_42)  /* Output pin of the PWM controller 9 */

#define GPIO_PIN_28_PWM8                (((UINT32)2U << 12U) | GPIO_PIN_28)  /* Output pin of the PWM controller 8 */
#define GPIO_PIN_29_PWM9                (((UINT32)2U << 12U) | GPIO_PIN_29)  /* Output pin of the PWM controller 9 */
#define GPIO_PIN_30_PWM10               (((UINT32)2U << 12U) | GPIO_PIN_30)  /* Output pin of the PWM controller 10 */
#define GPIO_PIN_31_PWM11               (((UINT32)2U << 12U) | GPIO_PIN_31)  /* Output pin of the PWM controller 11 */

#define GPIO_PIN_99_PWM7                (((UINT32)5U << 12U) | GPIO_PIN_99)  /* Output pin of the PWM controller 7 */
#define GPIO_PIN_100_PWM8               (((UINT32)5U << 12U) | GPIO_PIN_100) /* Output pin of the PWM controller 8 */
#define GPIO_PIN_101_PWM9               (((UINT32)5U << 12U) | GPIO_PIN_101) /* Output pin of the PWM controller 9 */

/* SD0/SD */
#define GPIO_PIN_48_SD0_CLK             (((UINT32)2U << 12U) | GPIO_PIN_48)  /* Clock pin of SD port 0 */
#define GPIO_PIN_49_SD0_CMD             (((UINT32)2U << 12U) | GPIO_PIN_49)  /* Command pin of SD port 0 */
#define GPIO_PIN_50_SD0_CD              (((UINT32)2U << 12U) | GPIO_PIN_50)  /* Card detect pin of SD port 0 */
#define GPIO_PIN_51_SD0_WP              (((UINT32)2U << 12U) | GPIO_PIN_51)  /* Write protect pin of SD port 0 */
#define GPIO_PIN_64_SD0_DATA0           (((UINT32)2U << 12U) | GPIO_PIN_64)  /* Data pin 0 of SD port 0 */
#define GPIO_PIN_65_SD0_DATA1           (((UINT32)2U << 12U) | GPIO_PIN_65)  /* Data pin 1 of SD port 0 */
#define GPIO_PIN_66_SD0_DATA2           (((UINT32)2U << 12U) | GPIO_PIN_66)  /* Data pin 2 of SD port 0 */
#define GPIO_PIN_67_SD0_DATA3           (((UINT32)2U << 12U) | GPIO_PIN_67)  /* Data pin 3 of SD port 0 */
#define GPIO_PIN_68_SD0_DATA4           (((UINT32)2U << 12U) | GPIO_PIN_68)  /* Data pin 4 of SD port 0 */
#define GPIO_PIN_69_SD0_DATA5           (((UINT32)2U << 12U) | GPIO_PIN_69)  /* Data pin 5 of SD port 0 */
#define GPIO_PIN_70_SD0_DATA6           (((UINT32)2U << 12U) | GPIO_PIN_70)  /* Data pin 6 of SD port 0 */
#define GPIO_PIN_71_SD0_DATA7           (((UINT32)2U << 12U) | GPIO_PIN_71)  /* Data pin 7 of SD port 0 */
#define GPIO_PIN_81_SD0_RESET           (((UINT32)2U << 12U) | GPIO_PIN_81)  /* Reset pin of SD port 0 */
#define GPIO_PIN_105_SD0_HS_SEL         (((UINT32)1U << 12U) | GPIO_PIN_105) /* High speed mode select of SD port 0 */

/* SD1/SDIO0 */
#define GPIO_PIN_72_SD1_CLK             (((UINT32)2U << 12U) | GPIO_PIN_72)  /* Clock pin of SD port 1 (SDIO0) */
#define GPIO_PIN_73_SD1_CMD             (((UINT32)2U << 12U) | GPIO_PIN_73)  /* Command pin of SD port 1 (SDIO0) */
#define GPIO_PIN_74_SD1_DATA0           (((UINT32)2U << 12U) | GPIO_PIN_74)  /* Data pin 0 of SD port 1 (SDIO0) */
#define GPIO_PIN_75_SD1_DATA1           (((UINT32)2U << 12U) | GPIO_PIN_75)  /* Data pin 1 of SD port 1 (SDIO0) */
#define GPIO_PIN_76_SD1_DATA2           (((UINT32)2U << 12U) | GPIO_PIN_76)  /* Data pin 2 of SD port 1 (SDIO0) */
#define GPIO_PIN_77_SD1_DATA3           (((UINT32)2U << 12U) | GPIO_PIN_77)  /* Data pin 3 of SD port 1 (SDIO0) */
#define GPIO_PIN_78_SD1_CD              (((UINT32)2U << 12U) | GPIO_PIN_78)  /* Card detect pin of SD port 1 (SDIO0) */
#define GPIO_PIN_79_SD1_WP              (((UINT32)2U << 12U) | GPIO_PIN_79)  /* Write protect pin of SD port 1 (SDIO0) */
#define GPIO_PIN_80_SD1_RESET           (((UINT32)2U << 12U) | GPIO_PIN_80)  /* Reset pin of SD port 1 (SDIO0) */
#define GPIO_PIN_18_SD1_HS_SEL          (((UINT32)3U << 12U) | GPIO_PIN_18)  /* High speed mode select of SD port 1 (SDIO0) */

/* SD2/SDIO1 */
#define GPIO_PIN_97_SD2_DATA0           (((UINT32)1U << 12U) | GPIO_PIN_97)  /* Data pin 0 of SD port 2 (SDIO1) */
#define GPIO_PIN_98_SD2_DATA1           (((UINT32)1U << 12U) | GPIO_PIN_98)  /* Data pin 1 of SD port 2 (SDIO1) */
#define GPIO_PIN_99_SD2_DATA2           (((UINT32)1U << 12U) | GPIO_PIN_99)  /* Data pin 2 of SD port 2 (SDIO1) */
#define GPIO_PIN_100_SD2_DATA3          (((UINT32)1U << 12U) | GPIO_PIN_100) /* Data pin 3 of SD port 2 (SDIO1) */
#define GPIO_PIN_101_SD2_CLK            (((UINT32)1U << 12U) | GPIO_PIN_101) /* Clock pin of SD port 2 (SDIO1) */
#define GPIO_PIN_102_SD2_CMD            (((UINT32)1U << 12U) | GPIO_PIN_102) /* Command pin of SD port 2 (SDIO1) */
#define GPIO_PIN_103_SD2_CD             (((UINT32)1U << 12U) | GPIO_PIN_103) /* Card detect pin of SD port 2 (SDIO1) */
#define GPIO_PIN_104_SD2_WP             (((UINT32)1U << 12U) | GPIO_PIN_104) /* Write protect pin of SD port 2 (SDIO1) */
#define GPIO_PIN_17_SD2_HS_SEL          (((UINT32)3U << 12U) | GPIO_PIN_17)  /* High speed mode select of SD port 2 (SDIO1) */

/* SPI */
#define GPIO_PIN_4_SPI0_SCLK            (((UINT32)1U << 12U) | GPIO_PIN_4)   /* Serial Clock pin of the SPI master port 0 */
#define GPIO_PIN_5_SPI0_MOSI            (((UINT32)1U << 12U) | GPIO_PIN_5)   /* MOSI: TXD pin of the SPI master port 0 */
#define GPIO_PIN_6_SPI0_MISO            (((UINT32)1U << 12U) | GPIO_PIN_6)   /* MISO: RXD pin of the SPI master port 0 */
#define GPIO_PIN_7_SPI0_SS0             (((UINT32)1U << 12U) | GPIO_PIN_7)   /* The 1st Slave Select pin of the SPI master port 0 */
#define GPIO_PIN_8_SPI0_SS1             (((UINT32)1U << 12U) | GPIO_PIN_8)   /* The 2nd Slave Select pin of the SPI master port 0 */
#define GPIO_PIN_9_SPI0_SS2             (((UINT32)3U << 12U) | GPIO_PIN_9)   /* The 3rd Slave Select pin of the SPI master port 0 */
#define GPIO_PIN_10_SPI0_SS3            (((UINT32)3U << 12U) | GPIO_PIN_10)  /* The 4th Slave Select pin of the SPI master port 0 */
#define GPIO_PIN_11_SPI0_SS4            (((UINT32)3U << 12U) | GPIO_PIN_11)  /* The 5th Slave Select pin of the SPI master port 0 */
#define GPIO_PIN_12_SPI0_SS5            (((UINT32)3U << 12U) | GPIO_PIN_12)  /* The 6th Slave Select pin of the SPI master port 0 */
#define GPIO_PIN_13_SPI0_SS6            (((UINT32)3U << 12U) | GPIO_PIN_13)  /* The 7th Slave Select pin of the SPI master port 0 */
#define GPIO_PIN_14_SPI0_SS7            (((UINT32)3U << 12U) | GPIO_PIN_14)  /* The 8th Slave Select pin of the SPI master port 0 */
#define GPIO_PIN_97_SPI0_SCLK           (((UINT32)2U << 12U) | GPIO_PIN_97)  /* Serial Clock pin of the SPI master port 0 */
#define GPIO_PIN_98_SPI0_MOSI           (((UINT32)2U << 12U) | GPIO_PIN_98)  /* MOSI: TXD pin of the SPI master port 0 */
#define GPIO_PIN_99_SPI0_MISO           (((UINT32)2U << 12U) | GPIO_PIN_99)  /* MISO: RXD pin of the SPI master port 0 */
#define GPIO_PIN_100_SPI0_SS0           (((UINT32)2U << 12U) | GPIO_PIN_100) /* The 1st Slave Select pin of the SPI master port 0 */

#define GPIO_PIN_22_SPI1_SCLK           (((UINT32)2U << 12U) | GPIO_PIN_22)  /* Serial Clock pin of the SPI master port 1 */
#define GPIO_PIN_23_SPI1_MOSI           (((UINT32)2U << 12U) | GPIO_PIN_23)  /* MOSI: TXD pin of the SPI master port 1 */
#define GPIO_PIN_24_SPI1_MISO           (((UINT32)2U << 12U) | GPIO_PIN_24)  /* MISO: RXD pin of the SPI master port 1 */
#define GPIO_PIN_25_SPI1_SS0            (((UINT32)2U << 12U) | GPIO_PIN_25)  /* The 1st Slave Select pin of the SPI master port 1 */
#define GPIO_PIN_72_SPI1_SCLK           (((UINT32)3U << 12U) | GPIO_PIN_72)  /* Serial Clock pin of the SPI master port 1 */
#define GPIO_PIN_73_SPI1_MOSI           (((UINT32)3U << 12U) | GPIO_PIN_73)  /* MOSI: TXD pin of the SPI master port 1 */
#define GPIO_PIN_74_SPI1_MISO           (((UINT32)3U << 12U) | GPIO_PIN_74)  /* MISO: RXD pin of the SPI master port 1 */
#define GPIO_PIN_75_SPI1_SS0            (((UINT32)3U << 12U) | GPIO_PIN_75)  /* The 1st Slave Select pin of the SPI master port 1 */
#define GPIO_PIN_76_SPI1_SS1            (((UINT32)3U << 12U) | GPIO_PIN_76)  /* The 2nd Slave Select pin of the SPI master port 1 */
#define GPIO_PIN_77_SPI1_SS2            (((UINT32)3U << 12U) | GPIO_PIN_77)  /* The 3rd Slave Select pin of the SPI master port 1 */
#define GPIO_PIN_78_SPI1_SS3            (((UINT32)3U << 12U) | GPIO_PIN_78)  /* The 4th Slave Select pin of the SPI master port 1 */
#define GPIO_PIN_87_SPI1_SCLK           (((UINT32)2U << 12U) | GPIO_PIN_87)  /* Serial Clock pin of the SPI master port 1 */
#define GPIO_PIN_88_SPI1_MOSI           (((UINT32)2U << 12U) | GPIO_PIN_88)  /* MOSI: TXD pin of the SPI master port 1 */
#define GPIO_PIN_89_SPI1_MISO           (((UINT32)2U << 12U) | GPIO_PIN_89)  /* MISO: RXD pin of the SPI master port 1 */
#define GPIO_PIN_90_SPI1_SS0            (((UINT32)2U << 12U) | GPIO_PIN_90)  /* The 1st Slave Select pin of the SPI master port 1 */
#define GPIO_PIN_101_SPI1_SCLK          (((UINT32)2U << 12U) | GPIO_PIN_101) /* Serial Clock pin of the SPI master port 1 */
#define GPIO_PIN_102_SPI1_MOSI          (((UINT32)2U << 12U) | GPIO_PIN_102) /* MOSI: TXD pin of the SPI master port 1 */
#define GPIO_PIN_103_SPI1_MISO          (((UINT32)2U << 12U) | GPIO_PIN_103) /* MISO: RXD pin of the SPI master port 1 */
#define GPIO_PIN_104_SPI1_SS0           (((UINT32)2U << 12U) | GPIO_PIN_104) /* The 1st Slave Select pin of the SPI master port 1 */

#define GPIO_PIN_9_SPI2_SCLK            (((UINT32)1U << 12U) | GPIO_PIN_9)   /* Serial Clock pin of the SPI master port 2 */
#define GPIO_PIN_10_SPI2_MOSI           (((UINT32)1U << 12U) | GPIO_PIN_10)  /* MOSI: TXD pin of the SPI master port 2 */
#define GPIO_PIN_11_SPI2_MISO           (((UINT32)1U << 12U) | GPIO_PIN_11)  /* MISO: RXD pin of the SPI master port 2 */
#define GPIO_PIN_12_SPI2_SS0            (((UINT32)1U << 12U) | GPIO_PIN_12)  /* The 1st Slave Select pin of the SPI master port 2 */

#define GPIO_PIN_13_SPI3_SCLK           (((UINT32)1U << 12U) | GPIO_PIN_13)  /* Serial Clock pin of the SPI master port 3 */
#define GPIO_PIN_14_SPI3_MOSI           (((UINT32)1U << 12U) | GPIO_PIN_14)  /* MOSI: TXD pin of the SPI master port 3 */
#define GPIO_PIN_15_SPI3_MISO           (((UINT32)1U << 12U) | GPIO_PIN_15)  /* MISO: RXD pin of the SPI master port 3 */
#define GPIO_PIN_16_SPI3_SS0            (((UINT32)1U << 12U) | GPIO_PIN_16)  /* The 1st Slave Select pin of the SPI master port 3 */

#define GPIO_PIN_13_SPI_SLAVE_SCLK      (((UINT32)2U << 12U) | GPIO_PIN_13)  /* Serial Clock pin of the SPI slave port */
#define GPIO_PIN_14_SPI_SLAVE_MISO      (((UINT32)2U << 12U) | GPIO_PIN_14)  /* MISO: TXD pin of the SPI slave port */
#define GPIO_PIN_15_SPI_SLAVE_MOSI      (((UINT32)2U << 12U) | GPIO_PIN_15)  /* MOSI: RXD pin of the SPI slave port */
#define GPIO_PIN_16_SPI_SLAVE_SS        (((UINT32)2U << 12U) | GPIO_PIN_16)  /* Slave Select pin of the SPI slave port  */
#define GPIO_PIN_97_SPI_SLAVE_SCLK      (((UINT32)3U << 12U) | GPIO_PIN_97)  /* Serial Clock pin of the SPI slave port */
#define GPIO_PIN_98_SPI_SLAVE_MISO      (((UINT32)3U << 12U) | GPIO_PIN_98)  /* MISO: TXD pin of the SPI slave port */
#define GPIO_PIN_99_SPI_SLAVE_MOSI      (((UINT32)3U << 12U) | GPIO_PIN_99)  /* MOSI: RXD pin of the SPI slave port */
#define GPIO_PIN_100_SPI_SLAVE_SS       (((UINT32)3U << 12U) | GPIO_PIN_100) /* Slave Select pin of the SPI slave port  */

/* Timer */
#define GPIO_PIN_0_TIMER0               (((UINT32)1U << 12U) | GPIO_PIN_0)   /* External clock source of interval timer 0 */
#define GPIO_PIN_1_TIMER1               (((UINT32)2U << 12U) | GPIO_PIN_1)   /* External clock source of interval timer 1 */
#define GPIO_PIN_2_TIMER2               (((UINT32)2U << 12U) | GPIO_PIN_2)   /* External clock source of interval timer 2 */
#define GPIO_PIN_93_TIMER1              (((UINT32)2U << 12U) | GPIO_PIN_93)  /* External clock source of interval timer 1 */
#define GPIO_PIN_94_TIMER2              (((UINT32)2U << 12U) | GPIO_PIN_94)  /* External clock source of interval timer 2 */
#define GPIO_PIN_95_TIMER1              (((UINT32)3U << 12U) | GPIO_PIN_95)  /* External clock source of interval timer 1 */
#define GPIO_PIN_96_TIMER2              (((UINT32)3U << 12U) | GPIO_PIN_96)  /* External clock source of interval timer 2 */
#define GPIO_PIN_103_TIMER1             (((UINT32)5U << 12U) | GPIO_PIN_103) /* External clock source of interval timer 1 */
#define GPIO_PIN_104_TIMER2             (((UINT32)5U << 12U) | GPIO_PIN_104) /* External clock source of interval timer 2 */

/* UART */
#define GPIO_PIN_17_UART_APB_RXD        (((UINT32)1U << 12U) | GPIO_PIN_17)  /* Receive Data pin of the UART_APB port 0 */
#define GPIO_PIN_18_UART_APB_TXD        (((UINT32)1U << 12U) | GPIO_PIN_18)  /* Transmit Data pin of the UART_APB port 0 */

#define GPIO_PIN_87_UART0_RXD           (((UINT32)1U << 12U) | GPIO_PIN_87)  /* Receive Data pin of the UART_AHB port 0 */
#define GPIO_PIN_88_UART0_TXD           (((UINT32)1U << 12U) | GPIO_PIN_88)  /* Transmit Data pin of the UART_AHB port 0 */
#define GPIO_PIN_89_UART0_CTS_N         (((UINT32)1U << 12U) | GPIO_PIN_89)  /* Clear-to-Send pin of the UART_AHB port 0 */
#define GPIO_PIN_90_UART0_RTS_N         (((UINT32)1U << 12U) | GPIO_PIN_90)  /* Request-to-Send pin of the UART_AHB port 0 */

#define GPIO_PIN_91_UART1_RXD           (((UINT32)1U << 12U) | GPIO_PIN_91)  /* Receive Data pin of the UART_AHB port 1 */
#define GPIO_PIN_92_UART1_TXD           (((UINT32)1U << 12U) | GPIO_PIN_92)  /* Transmit Data pin of the UART_AHB port 1 */
#define GPIO_PIN_93_UART1_CTS_N         (((UINT32)1U << 12U) | GPIO_PIN_93)  /* Clear-to-Send pin of the UART_AHB port 1 */
#define GPIO_PIN_94_UART1_RTS_N         (((UINT32)1U << 12U) | GPIO_PIN_94)  /* Request-to-Send pin of the UART_AHB port 1 */

#define GPIO_PIN_72_UART2_RXD           (((UINT32)1U << 12U) | GPIO_PIN_72)  /* Receive Data pin of the UART_AHB port 2 */
#define GPIO_PIN_73_UART2_TXD           (((UINT32)1U << 12U) | GPIO_PIN_73)  /* Transmit Data pin of the UART_AHB port 2 */
#define GPIO_PIN_74_UART2_CTS_N         (((UINT32)1U << 12U) | GPIO_PIN_74)  /* Clear-to-Send pin of the UART_AHB port 2 */
#define GPIO_PIN_75_UART2_RTS_N         (((UINT32)1U << 12U) | GPIO_PIN_75)  /* Request-to-Send pin of the UART_AHB port 2 */
#define GPIO_PIN_97_UART2_RXD           (((UINT32)4U << 12U) | GPIO_PIN_97)  /* Receive Data pin of the UART_AHB port 2 */
#define GPIO_PIN_98_UART2_TXD           (((UINT32)4U << 12U) | GPIO_PIN_98)  /* Transmit Data pin of the UART_AHB port 2 */
#define GPIO_PIN_99_UART2_CTS_N         (((UINT32)4U << 12U) | GPIO_PIN_99)  /* Clear-to-Send pin of the UART_AHB port 2 */
#define GPIO_PIN_100_UART2_RTS_N        (((UINT32)4U << 12U) | GPIO_PIN_100) /* Request-to-Send pin of the UART_AHB port 2 */

#define GPIO_PIN_76_UART3_RXD           (((UINT32)1U << 12U) | GPIO_PIN_76)  /* Receive Data pin of the UART_AHB port 3 */
#define GPIO_PIN_77_UART3_TXD           (((UINT32)1U << 12U) | GPIO_PIN_77)  /* Transmit Data pin of the UART_AHB port 3 */
#define GPIO_PIN_78_UART3_CTS_N         (((UINT32)1U << 12U) | GPIO_PIN_78)  /* Clear-to-Send pin of the UART_AHB port 3 */
#define GPIO_PIN_79_UART3_RTS_N         (((UINT32)1U << 12U) | GPIO_PIN_79)  /* Request-to-Send pin of the UART_AHB port 3 */
#define GPIO_PIN_101_UART3_RXD          (((UINT32)4U << 12U) | GPIO_PIN_101) /* Receive Data pin of the UART_AHB port 3 */
#define GPIO_PIN_102_UART3_TXD          (((UINT32)4U << 12U) | GPIO_PIN_102) /* Transmit Data pin of the UART_AHB port 3 */
#define GPIO_PIN_103_UART3_CTS_N        (((UINT32)4U << 12U) | GPIO_PIN_103) /* Clear-to-Send pin of the UART_AHB port 3 */
#define GPIO_PIN_104_UART3_RTS_N        (((UINT32)4U << 12U) | GPIO_PIN_104) /* Request-to-Send pin of the UART_AHB port 3 */

/* USB */
#define GPIO_PIN_17_USB0_EHCI_OC        (((UINT32)4U << 12U) | GPIO_PIN_17)  /* Over-Current pin of the USB EHCI port 0 */
#define GPIO_PIN_18_USB0_EHCI_PWR       (((UINT32)4U << 12U) | GPIO_PIN_18)  /* USB EHCI port 0 power enable */
#define GPIO_PIN_25_USB0_EHCI_OC        (((UINT32)3U << 12U) | GPIO_PIN_25)  /* Over-Current pin of the USB EHCI port 0 */
#define GPIO_PIN_26_USB0_EHCI_PWR       (((UINT32)3U << 12U) | GPIO_PIN_26)  /* USB EHCI port 0 power enable */
#define GPIO_PIN_87_USB0_EHCI_OC        (((UINT32)3U << 12U) | GPIO_PIN_87)  /* Over-Current pin of the USB EHCI port 0 */
#define GPIO_PIN_89_USB0_EHCI_PWR       (((UINT32)3U << 12U) | GPIO_PIN_89)  /* USB EHCI port 0 power enable */
#define GPIO_PIN_97_USB0_EHCI_OC        (((UINT32)5U << 12U) | GPIO_PIN_97)  /* Over-Current pin of the USB EHCI port 0 */
#define GPIO_PIN_98_USB0_EHCI_PWR       (((UINT32)5U << 12U) | GPIO_PIN_98)  /* USB EHCI port 0 power enable */

/* Video Input */
#define GPIO_PIN_85_VIN0_MASTER_HSYNC   (((UINT32)1U << 12U) | GPIO_PIN_85)  /* VIN0/VIN Master HSYNC */
#define GPIO_PIN_86_VIN0_MASTER_VSYNC   (((UINT32)1U << 12U) | GPIO_PIN_86)  /* VIN0/VIN Master VSYNC */

#define GPIO_PIN_85_VIN1_MASTER_HSYNC   (((UINT32)2U << 12U) | GPIO_PIN_85)  /* VIN1/PIP Master HSYNC */
#define GPIO_PIN_86_VIN1_MASTER_VSYNC   (((UINT32)2U << 12U) | GPIO_PIN_86)  /* VIN1/PIP Master VSYNC */
#define GPIO_PIN_91_VIN1_MASTER_HSYNC   (((UINT32)2U << 12U) | GPIO_PIN_91)  /* VIN1/PIP Master HSYNC */
#define GPIO_PIN_92_VIN1_MASTER_VSYNC   (((UINT32)2U << 12U) | GPIO_PIN_92)  /* VIN1/PIP Master VSYNC */

#define GPIO_PIN_1_VIN_STRIG_0          (((UINT32)4U << 12U) | GPIO_PIN_1)   /* VIN strig 0 */
#define GPIO_PIN_2_VIN_STRIG_1          (((UINT32)4U << 12U) | GPIO_PIN_2)   /* VIN strig 1 */
#define GPIO_PIN_19_VIN_STRIG_0         (((UINT32)2U << 12U) | GPIO_PIN_19)  /* VIN strig 0 */
#define GPIO_PIN_20_VIN_STRIG_1         (((UINT32)2U << 12U) | GPIO_PIN_20)  /* VIN strig 1 */
#define GPIO_PIN_45_VIN_STRIG_0         (((UINT32)1U << 12U) | GPIO_PIN_45)  /* VIN strig 0 */
#define GPIO_PIN_46_VIN_STRIG_1         (((UINT32)1U << 12U) | GPIO_PIN_46)  /* VIN strig 1 */

/* Video Output Display */
#define GPIO_PIN_28_VD0_OUT_0           (((UINT32)1U << 12U) | GPIO_PIN_28)  /* Digital video output data pin 0 */
#define GPIO_PIN_29_VD0_OUT_1           (((UINT32)1U << 12U) | GPIO_PIN_29)  /* Digital video output data pin 1 */
#define GPIO_PIN_30_VD0_OUT_2           (((UINT32)1U << 12U) | GPIO_PIN_30)  /* Digital video output data pin 2 */
#define GPIO_PIN_31_VD0_OUT_3           (((UINT32)1U << 12U) | GPIO_PIN_31)  /* Digital video output data pin 3 */
#define GPIO_PIN_32_VD0_OUT_4           (((UINT32)1U << 12U) | GPIO_PIN_32)  /* Digital video output data pin 4 */
#define GPIO_PIN_33_VD0_OUT_5           (((UINT32)1U << 12U) | GPIO_PIN_33)  /* Digital video output data pin 5 */
#define GPIO_PIN_34_VD0_OUT_6           (((UINT32)1U << 12U) | GPIO_PIN_34)  /* Digital video output data pin 6 */
#define GPIO_PIN_35_VD0_OUT_7           (((UINT32)1U << 12U) | GPIO_PIN_35)  /* Digital video output data pin 7 */
#define GPIO_PIN_36_VD0_OUT_8           (((UINT32)1U << 12U) | GPIO_PIN_36)  /* Digital video output data pin 8 */
#define GPIO_PIN_37_VD0_OUT_9           (((UINT32)1U << 12U) | GPIO_PIN_37)  /* Digital video output data pin 9 */
#define GPIO_PIN_38_VD0_OUT_10          (((UINT32)1U << 12U) | GPIO_PIN_38)  /* Digital video output data pin 10 */
#define GPIO_PIN_39_VD0_OUT_11          (((UINT32)1U << 12U) | GPIO_PIN_39)  /* Digital video output data pin 11 */
#define GPIO_PIN_40_VD0_OUT_12          (((UINT32)1U << 12U) | GPIO_PIN_40)  /* Digital video output data pin 12 */
#define GPIO_PIN_41_VD0_OUT_13          (((UINT32)1U << 12U) | GPIO_PIN_41)  /* Digital video output data pin 13 */
#define GPIO_PIN_42_VD0_OUT_14          (((UINT32)1U << 12U) | GPIO_PIN_42)  /* Digital video output data pin 14 */
#define GPIO_PIN_43_VD0_OUT_15          (((UINT32)1U << 12U) | GPIO_PIN_43)  /* Digital video output data pin 15 */
#define GPIO_PIN_91_VD0_CLK             (((UINT32)3U << 12U) | GPIO_PIN_91)  /* Digital video output clock pin */
#define GPIO_PIN_92_VD0_VSYNC           (((UINT32)3U << 12U) | GPIO_PIN_92)  /* Digital video output vsync pin */
#define GPIO_PIN_93_VD0_HSYNC           (((UINT32)3U << 12U) | GPIO_PIN_93)  /* Digital video output hsync pin */
#define GPIO_PIN_94_VD0_HVLD            (((UINT32)3U << 12U) | GPIO_PIN_94)  /* Digital video output hvld pin */

/* WDT */
#define GPIO_PIN_3_WDT_RST              (((UINT32)2U << 12U) | GPIO_PIN_3)   /* WDT extend reset */
#define GPIO_PIN_8_WDT_RST              (((UINT32)3U << 12U) | GPIO_PIN_8)   /* WDT extend reset */
#define GPIO_PIN_21_WDT_RST             (((UINT32)2U << 12U) | GPIO_PIN_21)  /* WDT extend reset */
#define GPIO_PIN_44_WDT_RST             (((UINT32)2U << 12U) | GPIO_PIN_44)  /* WDT extend reset */
#define GPIO_PIN_84_WDT_RST             (((UINT32)2U << 12U) | GPIO_PIN_84)  /* WDT extend reset */
#define GPIO_PIN_102_WDT_RST            (((UINT32)5U << 12U) | GPIO_PIN_102) /* WDT extend reset */

#define AMBA_GPIO_GROUP0                0x0U    /* GPIO pin 0~31    */
#define AMBA_GPIO_GROUP1                0x1U    /* GPIO pin 32~63   */
#define AMBA_GPIO_GROUP2                0x2U    /* GPIO pin 64~95   */
#define AMBA_GPIO_GROUP3                0x3U    /* GPIO pin 96~127  */
#define AMBA_NUM_GPIO_GROUP             0x4U

#endif /* AMBA_GPIO_PRIV_H */
