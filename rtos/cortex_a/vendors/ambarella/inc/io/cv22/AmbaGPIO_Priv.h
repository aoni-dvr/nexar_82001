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
/* Group 4 */
#define GPIO_PIN_128        128U
#define GPIO_PIN_129        129U
#define GPIO_PIN_130        130U
#define GPIO_PIN_131        131U
#define GPIO_PIN_132        132U
#define GPIO_PIN_133        133U
#define GPIO_PIN_134        134U
#define GPIO_PIN_135        135U
#define GPIO_PIN_136        136U
#define GPIO_PIN_137        137U
#define GPIO_PIN_138        138U
#define GPIO_PIN_139        139U
#define GPIO_PIN_140        140U
#define GPIO_PIN_141        141U
#define GPIO_PIN_142        142U
#define GPIO_PIN_143        143U
#define GPIO_PIN_144        144U
#define GPIO_PIN_145        145U
#define GPIO_PIN_146        146U
#define GPIO_PIN_147        147U
#define GPIO_PIN_148        148U
#define GPIO_PIN_149        149U
#define GPIO_PIN_150        150U
#define GPIO_PIN_151        151U
#define GPIO_PIN_152        152U
#define GPIO_PIN_153        153U
#define GPIO_PIN_154        154U
#define GPIO_PIN_155        155U
#define GPIO_PIN_156        156U
#define GPIO_PIN_157        157U
#define GPIO_PIN_158        158U
#define GPIO_PIN_159        159U

#define AMBA_NUM_GPIO_PIN   160U  /* Total Number of GPIO logical pins */

/* CAN Bus */
#define GPIO_PIN_154_CAN0_RX            (((UINT32)1U << 12U) | GPIO_PIN_154) /* RX pin of the CAN bus controller-0 */
#define GPIO_PIN_155_CAN0_TX            (((UINT32)1U << 12U) | GPIO_PIN_155) /* TX pin of the CAN bus controller-0 */

/* Debounced GPIO */
#define GPIO_PIN_DEBOUNCE_GPIO0         GPIO_PIN_158_DEBOUNCE_GPIO0
#define GPIO_PIN_DEBOUNCE_GPIO1         GPIO_PIN_159_DEBOUNCE_GPIO1
#define GPIO_PIN_158_DEBOUNCE_GPIO0     (((UINT32)9U << 12U) | GPIO_PIN_158) /* Debounce GPIO 0 */
#define GPIO_PIN_159_DEBOUNCE_GPIO1     (((UINT32)9U << 12U) | GPIO_PIN_159) /* Debounce GPIO 1 */

/* Ethernet */
#define GPIO_PIN_7_ENET_ALT_PTP_PPS     (((UINT32)3U << 12U) | GPIO_PIN_7)   /* Ethernet PTP PPS */
#define GPIO_PIN_54_ENET_ALT_TXEN       (((UINT32)3U << 12U) | GPIO_PIN_54)  /* Ethernet RMII TXEN (TX data ready to transmit) */
#define GPIO_PIN_55_ENET_ALT_TXD0       (((UINT32)3U << 12U) | GPIO_PIN_55)  /* Ethernet RMII transmit data bit 0 */
#define GPIO_PIN_56_ENET_ALT_TXD1       (((UINT32)3U << 12U) | GPIO_PIN_56)  /* Ethernet RMII transmit data bit 1 */
#define GPIO_PIN_59_ENET_ALT_RXD0       (((UINT32)3U << 12U) | GPIO_PIN_59)  /* Ethernet RMII receive data bit 0 */
#define GPIO_PIN_60_ENET_ALT_RXD1       (((UINT32)3U << 12U) | GPIO_PIN_60)  /* Ethernet RMII receive data bit 1 */
#define GPIO_PIN_63_ENET_ALT_RXDV       (((UINT32)3U << 12U) | GPIO_PIN_63)  /* Ethernet RMII RXDV (RX data valid) */
#define GPIO_PIN_64_ENET_ALT_MDC        (((UINT32)3U << 12U) | GPIO_PIN_64)  /* Ethernet RMII MDC pin */
#define GPIO_PIN_65_ENET_ALT_MDIO       (((UINT32)3U << 12U) | GPIO_PIN_65)  /* Ethernet RMII MDIO pin */
#define GPIO_PIN_66_RMII_REF_CLK1       (((UINT32)3U << 12U) | GPIO_PIN_66)  /* Ethernet RMII 2nd reference clock */
#define GPIO_PIN_67_RMII_REF_CLK0       (((UINT32)3U << 12U) | GPIO_PIN_67)  /* Ethernet RMII 1st reference clock */
#define GPIO_PIN_68_ENET_ALT_GTX_CLK    (((UINT32)3U << 12U) | GPIO_PIN_68)  /* Ethernet RMII clock */
#define GPIO_PIN_69_ENET_ALT_EXT_OSC    (((UINT32)3U << 12U) | GPIO_PIN_69)  /* Ethernet RMII external clock source */

#define GPIO_PIN_7_ENET_PTP_PPS         (((UINT32)4U << 12U) | GPIO_PIN_7)   /* Ethernet PTP PPS */
#define GPIO_PIN_54_ENET_TXEN           (((UINT32)4U << 12U) | GPIO_PIN_54)  /* Ethernet RMII TXEN (TX data ready to transmit) */
#define GPIO_PIN_55_ENET_TXD0           (((UINT32)4U << 12U) | GPIO_PIN_55)  /* Ethernet RMII transmit data bit 0 */
#define GPIO_PIN_56_ENET_TXD1           (((UINT32)4U << 12U) | GPIO_PIN_56)  /* Ethernet RMII transmit data bit 1 */
#define GPIO_PIN_57_ENET_TXD2           (((UINT32)4U << 12U) | GPIO_PIN_57)  /* Ethernet RMII transmit data bit 2 */
#define GPIO_PIN_58_ENET_TXD3           (((UINT32)4U << 12U) | GPIO_PIN_58)  /* Ethernet RMII transmit data bit 3 */
#define GPIO_PIN_59_ENET_RXD0           (((UINT32)4U << 12U) | GPIO_PIN_59)  /* Ethernet RMII receive data bit 0 */
#define GPIO_PIN_60_ENET_RXD1           (((UINT32)4U << 12U) | GPIO_PIN_60)  /* Ethernet RMII receive data bit 1 */
#define GPIO_PIN_61_ENET_RXD2           (((UINT32)4U << 12U) | GPIO_PIN_61)  /* Ethernet RMII receive data bit 2 */
#define GPIO_PIN_62_ENET_RXD3           (((UINT32)4U << 12U) | GPIO_PIN_62)  /* Ethernet RMII receive data bit 3 */
#define GPIO_PIN_63_ENET_RXDV           (((UINT32)4U << 12U) | GPIO_PIN_63)  /* Ethernet RMII RXDV (RX data valid) */
#define GPIO_PIN_64_ENET_MDC            (((UINT32)4U << 12U) | GPIO_PIN_64)  /* Ethernet RMII MDC pin */
#define GPIO_PIN_65_ENET_MDIO           (((UINT32)4U << 12U) | GPIO_PIN_65)  /* Ethernet RMII MDIO pin */
#define GPIO_PIN_67_ENET_CLK_RX         (((UINT32)4U << 12U) | GPIO_PIN_67)  /* Ethernet RMII 1st reference clock */
#define GPIO_PIN_68_ENET_GTX_CLK        (((UINT32)4U << 12U) | GPIO_PIN_68)  /* Ethernet RMII clock */
#define GPIO_PIN_69_ENET_EXT_OSC        (((UINT32)4U << 12U) | GPIO_PIN_69)  /* Ethernet RMII external clock source */

/* HDMI */
#define GPIO_PIN_111_HDMI_HPD           (((UINT32)1U << 12U) | GPIO_PIN_111) /* HPD pin of the HDMI source side */
#define GPIO_PIN_112_HDMI_CEC           (((UINT32)1U << 12U) | GPIO_PIN_112) /* CEC pin of the HDMI source side */

/* I2C */
#define GPIO_PIN_11_I2C0_CLK            (((UINT32)1U << 12U) | GPIO_PIN_11)  /* Clock pin of the 1st I2C master port */
#define GPIO_PIN_12_I2C0_DATA           (((UINT32)1U << 12U) | GPIO_PIN_12)  /* Data pin of the 1st I2C master port */
#define GPIO_PIN_13_I2C1_CLK            (((UINT32)1U << 12U) | GPIO_PIN_13)  /* Clock pin of the 2nd I2C master port */
#define GPIO_PIN_14_I2C1_DATA           (((UINT32)1U << 12U) | GPIO_PIN_14)  /* Data pin of the 2nd I2C master port */
#define GPIO_PIN_15_I2C2_CLK            (((UINT32)1U << 12U) | GPIO_PIN_15)  /* Clock pin of the 3rd I2C master port */
#define GPIO_PIN_16_I2C2_DATA           (((UINT32)1U << 12U) | GPIO_PIN_16)  /* Data pin of the 3rd I2C master port */
#define GPIO_PIN_17_I2C3_CLK            (((UINT32)1U << 12U) | GPIO_PIN_17)  /* Clock pin of the 4th I2C master port */
#define GPIO_PIN_18_I2C3_DATA           (((UINT32)1U << 12U) | GPIO_PIN_18)  /* Data pin of the 4th I2C master port */
#define GPIO_PIN_19_I2C_SLAVE_CLK       (((UINT32)1U << 12U) | GPIO_PIN_19)  /* Clock pin of the I2C slave port */
#define GPIO_PIN_20_I2C_SLAVE_DATA      (((UINT32)1U << 12U) | GPIO_PIN_20)  /* Data pin of the I2C slave port */

#define GPIO_PIN_33_I2C0_CLK            (((UINT32)2U << 12U) | GPIO_PIN_33)  /* Clock pin of the 1st I2C master port */
#define GPIO_PIN_34_I2C0_DATA           (((UINT32)2U << 12U) | GPIO_PIN_34)  /* Data pin of the 1st I2C master port */

/* I2S */
#define GPIO_PIN_44_I2S1_CLK            (((UINT32)1U << 12U) | GPIO_PIN_44)
#define GPIO_PIN_45_I2S1_SI             (((UINT32)1U << 12U) | GPIO_PIN_45)  /* Serial data input pin of the I2S controller */
#define GPIO_PIN_46_I2S1_SO             (((UINT32)1U << 12U) | GPIO_PIN_46)  /* Serial data output pin of the I2S controller */
#define GPIO_PIN_47_I2S1_WS             (((UINT32)1U << 12U) | GPIO_PIN_47)  /* Word select pin of the I2S controller */

#define GPIO_PIN_48_I2S0_CLK            (((UINT32)1U << 12U) | GPIO_PIN_48)
#define GPIO_PIN_49_I2S0_SI_0           (((UINT32)1U << 12U) | GPIO_PIN_49)  /* Serial data input pin of the I2S controller */
#define GPIO_PIN_50_I2S0_SO_0           (((UINT32)1U << 12U) | GPIO_PIN_50)  /* Serial data output pin of the I2S controller */
#define GPIO_PIN_51_I2S0_SI_1           (((UINT32)1U << 12U) | GPIO_PIN_51)  /* Serial data input pin of the I2S controller */
#define GPIO_PIN_52_I2S0_SO_1           (((UINT32)1U << 12U) | GPIO_PIN_52)  /* Serial data output pin of the I2S controller */
#define GPIO_PIN_53_I2S0_WS             (((UINT32)1U << 12U) | GPIO_PIN_53)  /* Word select pin of the I2S controller */

/* IR */
#define GPIO_PIN_21_IR_RX               (((UINT32)1U << 12U) | GPIO_PIN_21)  /* Infrared interface input pin */

/* NAND */
#define GPIO_PIN_70_NAND_WP             (((UINT32)2U << 12U) | GPIO_PIN_70)  /* Write Protection for NAND flash */
#define GPIO_PIN_71_NAND_CE             (((UINT32)2U << 12U) | GPIO_PIN_71)  /* Chip Enable for NAND flash */
#define GPIO_PIN_72_NAND_RB             (((UINT32)2U << 12U) | GPIO_PIN_72)  /* Ready/Busy for NAND flash */
#define GPIO_PIN_77_NAND_RE             (((UINT32)2U << 12U) | GPIO_PIN_77)  /* Read Enable for NAND flash */
#define GPIO_PIN_78_NAND_WE             (((UINT32)2U << 12U) | GPIO_PIN_78)  /* Write Enable for NAND flash */
#define GPIO_PIN_79_NAND_ALE            (((UINT32)2U << 12U) | GPIO_PIN_79)  /* Address Latch Enable for NAND flash */
#define GPIO_PIN_80_NAND_DATA0          (((UINT32)2U << 12U) | GPIO_PIN_80)  /* Data[0] for NAND flash */
#define GPIO_PIN_81_NAND_DATA1          (((UINT32)2U << 12U) | GPIO_PIN_81)  /* Data[1] for NAND flash */
#define GPIO_PIN_82_NAND_DATA2          (((UINT32)2U << 12U) | GPIO_PIN_82)  /* Data[2] for NAND flash */
#define GPIO_PIN_83_NAND_DATA3          (((UINT32)2U << 12U) | GPIO_PIN_83)  /* Data[3] for NAND flash */
#define GPIO_PIN_84_NAND_DATA4          (((UINT32)2U << 12U) | GPIO_PIN_84)  /* Data[4] for NAND flash */
#define GPIO_PIN_85_NAND_DATA5          (((UINT32)2U << 12U) | GPIO_PIN_85)  /* Data[5] for NAND flash */
#define GPIO_PIN_86_NAND_DATA6          (((UINT32)2U << 12U) | GPIO_PIN_86)  /* Data[6] for NAND flash */
#define GPIO_PIN_87_NAND_DATA7          (((UINT32)2U << 12U) | GPIO_PIN_87)  /* Data[7] for NAND flash */
#define GPIO_PIN_88_NAND_CLE            (((UINT32)2U << 12U) | GPIO_PIN_88)  /* Command Latch Enable for NAND flash */

/* NAND (SPI) */
#define GPIO_PIN_71_NAND_SPI_CLK        (((UINT32)4U << 12U) | GPIO_PIN_71)  /* SPI Clock for NAND flash */
#define GPIO_PIN_84_NAND_SPI_DATA0      (((UINT32)4U << 12U) | GPIO_PIN_84)  /* Data[0] for NAND flash */
#define GPIO_PIN_85_NAND_SPI_DATA1      (((UINT32)4U << 12U) | GPIO_PIN_85)  /* Data[1] for NAND flash */
#define GPIO_PIN_86_NAND_SPI_DATA2      (((UINT32)4U << 12U) | GPIO_PIN_86)  /* Data[2] for NAND flash */
#define GPIO_PIN_87_NAND_SPI_DATA3      (((UINT32)4U << 12U) | GPIO_PIN_87)  /* Data[3] for NAND flash */
#define GPIO_PIN_93_NAND_SPI_EN0        (((UINT32)4U << 12U) | GPIO_PIN_80)  /* SPI Slave Select 0 for NAND flash */

/* NOR (SPI) */
#define GPIO_PIN_71_NOR_SPI_CLK         (((UINT32)3U << 12U) | GPIO_PIN_71)  /* SPI Clock for NOR flash */
#define GPIO_PIN_84_NOR_SPI_DATA0       (((UINT32)3U << 12U) | GPIO_PIN_84)  /* Data[0] for NOR flash */
#define GPIO_PIN_85_NOR_SPI_DATA1       (((UINT32)3U << 12U) | GPIO_PIN_85)  /* Data[1] for NOR flash */
#define GPIO_PIN_86_NOR_SPI_DATA2       (((UINT32)3U << 12U) | GPIO_PIN_86)  /* Data[2] for NOR flash */
#define GPIO_PIN_87_NOR_SPI_DATA3       (((UINT32)3U << 12U) | GPIO_PIN_87)  /* Data[3] for NOR flash */
#define GPIO_PIN_72_NOR_SPI_DATA4       (((UINT32)3U << 12U) | GPIO_PIN_72)  /* Data[4] for NOR flash */
#define GPIO_PIN_77_NOR_SPI_DATA5       (((UINT32)3U << 12U) | GPIO_PIN_77)  /* Data[5] for NOR flash */
#define GPIO_PIN_78_NOR_SPI_DATA6       (((UINT32)3U << 12U) | GPIO_PIN_78)  /* Data[6] for NOR flash */
#define GPIO_PIN_79_NOR_SPI_DATA7       (((UINT32)3U << 12U) | GPIO_PIN_79)  /* Data[7] for NOR flash */
#define GPIO_PIN_80_NOR_SPI_EN0         (((UINT32)3U << 12U) | GPIO_PIN_80)  /* SPI Slave Select 0 for NOR flash */
#define GPIO_PIN_81_NOR_SPI_EN1         (((UINT32)3U << 12U) | GPIO_PIN_81)  /* SPI Slave Select 1 for NOR flash */
#define GPIO_PIN_82_NOR_SPI_EN2         (((UINT32)3U << 12U) | GPIO_PIN_82)  /* SPI Slave Select 2 for NOR flash */
#define GPIO_PIN_83_NOR_SPI_EN3         (((UINT32)3U << 12U) | GPIO_PIN_83)  /* SPI Slave Select 3 for NOR flash */
#define GPIO_PIN_93_NOR_SPI_EN4         (((UINT32)3U << 12U) | GPIO_PIN_93)  /* SPI Slave Select 4 for NOR flash */
#define GPIO_PIN_94_NOR_SPI_EN5         (((UINT32)3U << 12U) | GPIO_PIN_94)  /* SPI Slave Select 5 for NOR flash */
#define GPIO_PIN_95_NOR_SPI_EN6         (((UINT32)3U << 12U) | GPIO_PIN_95)  /* SPI Slave Select 6 for NOR flash */
#define GPIO_PIN_96_NOR_SPI_EN7         (((UINT32)3U << 12U) | GPIO_PIN_96)  /* SPI Slave Select 7 for NOR flash */

/* PWM */
#define GPIO_PIN_137_PWM0               (((UINT32)1U << 12U) | GPIO_PIN_137) /* Output pin of the PWM controller 0 (VD_PWM) */

#define GPIO_PIN_41_PWM0                (((UINT32)1U << 12U) | GPIO_PIN_41)  /* Output pin of the PWM controller 0 */
#define GPIO_PIN_42_PWM1                (((UINT32)1U << 12U) | GPIO_PIN_42)  /* Output pin of the PWM controller 1 */
#define GPIO_PIN_43_PWM2                (((UINT32)1U << 12U) | GPIO_PIN_43)  /* Output pin of the PWM controller 2 */

#define GPIO_PIN_15_PWM3                (((UINT32)2U << 12U) | GPIO_PIN_15)  /* Output pin of the PWM controller 3 */
#define GPIO_PIN_16_PWM4                (((UINT32)2U << 12U) | GPIO_PIN_16)  /* Output pin of the PWM controller 4 */
#define GPIO_PIN_29_PWM5                (((UINT32)2U << 12U) | GPIO_PIN_29)  /* Output pin of the PWM controller 5 */
#define GPIO_PIN_30_PWM6                (((UINT32)2U << 12U) | GPIO_PIN_30)  /* Output pin of the PWM controller 6 */
#define GPIO_PIN_31_PWM7                (((UINT32)2U << 12U) | GPIO_PIN_31)  /* Output pin of the PWM controller 7 */
#define GPIO_PIN_32_PWM8                (((UINT32)2U << 12U) | GPIO_PIN_32)  /* Output pin of the PWM controller 8 */
#define GPIO_PIN_153_PWM9               (((UINT32)2U << 12U) | GPIO_PIN_153) /* Output pin of the PWM controller 9 */
#define GPIO_PIN_147_PWM10              (((UINT32)2U << 12U) | GPIO_PIN_147) /* Output pin of the PWM controller 10 */
#define GPIO_PIN_149_PWM11              (((UINT32)2U << 12U) | GPIO_PIN_149) /* Output pin of the PWM controller 11 */

#define GPIO_PIN_0_PWM3                 (((UINT32)3U << 12U) | GPIO_PIN_0)   /* Output pin of the PWM controller 3 */
#define GPIO_PIN_1_PWM4                 (((UINT32)3U << 12U) | GPIO_PIN_1)   /* Output pin of the PWM controller 4 */
#define GPIO_PIN_2_PWM5                 (((UINT32)3U << 12U) | GPIO_PIN_2)   /* Output pin of the PWM controller 5 */
#define GPIO_PIN_3_PWM6                 (((UINT32)3U << 12U) | GPIO_PIN_3)   /* Output pin of the PWM controller 6 */
#define GPIO_PIN_8_PWM7                 (((UINT32)3U << 12U) | GPIO_PIN_8)   /* Output pin of the PWM controller 7 */
#define GPIO_PIN_17_PWM8                (((UINT32)3U << 12U) | GPIO_PIN_17)  /* Output pin of the PWM controller 8 */
#define GPIO_PIN_18_PWM9                (((UINT32)3U << 12U) | GPIO_PIN_18)  /* Output pin of the PWM controller 9 */

/* PWM for Stepper Motor */
#define GPIO_PIN_0_STEPPER_A0           (((UINT32)1U << 12U) | GPIO_PIN_0)   /* Pin 0 of stepper controller A */
#define GPIO_PIN_1_STEPPER_A1           (((UINT32)1U << 12U) | GPIO_PIN_1)   /* Pin 1 of stepper controller A */
#define GPIO_PIN_2_STEPPER_A2           (((UINT32)1U << 12U) | GPIO_PIN_2)   /* Pin 2 of stepper controller A */
#define GPIO_PIN_3_STEPPER_A3           (((UINT32)1U << 12U) | GPIO_PIN_3)   /* Pin 3 of stepper controller A */
#define GPIO_PIN_4_STEPPER_B0           (((UINT32)1U << 12U) | GPIO_PIN_4)   /* Pin 0 of stepper controller B */
#define GPIO_PIN_5_STEPPER_B1           (((UINT32)1U << 12U) | GPIO_PIN_5)   /* Pin 1 of stepper controller B */
#define GPIO_PIN_6_STEPPER_B2           (((UINT32)1U << 12U) | GPIO_PIN_6)   /* Pin 2 of stepper controller B */
#define GPIO_PIN_7_STEPPER_B3           (((UINT32)1U << 12U) | GPIO_PIN_7)   /* Pin 3 of stepper controller B */
#define GPIO_PIN_31_STEPPER_C0          (((UINT32)4U << 12U) | GPIO_PIN_31)  /* Pin 0 of stepper controller C */
#define GPIO_PIN_32_STEPPER_C1          (((UINT32)4U << 12U) | GPIO_PIN_32)  /* Pin 1 of stepper controller C */
#define GPIO_PIN_33_STEPPER_C2          (((UINT32)4U << 12U) | GPIO_PIN_33)  /* Pin 2 of stepper controller C */
#define GPIO_PIN_34_STEPPER_C3          (((UINT32)4U << 12U) | GPIO_PIN_34)  /* Pin 3 of stepper controller C */

/* SD0/SD */
#define GPIO_PIN_73_SD0_CLK             (((UINT32)2U << 12U) | GPIO_PIN_73)  /* Clock pin of SD port 0 */
#define GPIO_PIN_74_SD0_CMD             (((UINT32)2U << 12U) | GPIO_PIN_74)  /* Command pin of SD port 0 */
#define GPIO_PIN_75_SD0_CD              (((UINT32)2U << 12U) | GPIO_PIN_75)  /* Card detect pin of SD port 0 */
#define GPIO_PIN_76_SD0_WP              (((UINT32)2U << 12U) | GPIO_PIN_76)  /* Write protect pin of SD port 0 */
#define GPIO_PIN_89_SD0_DATA0           (((UINT32)2U << 12U) | GPIO_PIN_89)  /* Data pin 0 of SD port 0 */
#define GPIO_PIN_90_SD0_DATA1           (((UINT32)2U << 12U) | GPIO_PIN_90)  /* Data pin 1 of SD port 0 */
#define GPIO_PIN_91_SD0_DATA2           (((UINT32)2U << 12U) | GPIO_PIN_91)  /* Data pin 2 of SD port 0 */
#define GPIO_PIN_92_SD0_DATA3           (((UINT32)2U << 12U) | GPIO_PIN_92)  /* Data pin 3 of SD port 0 */
#define GPIO_PIN_93_SD0_DATA4           (((UINT32)2U << 12U) | GPIO_PIN_93)  /* Data pin 4 of SD port 0 */
#define GPIO_PIN_94_SD0_DATA5           (((UINT32)2U << 12U) | GPIO_PIN_94)  /* Data pin 5 of SD port 0 */
#define GPIO_PIN_95_SD0_DATA6           (((UINT32)2U << 12U) | GPIO_PIN_95)  /* Data pin 6 of SD port 0 */
#define GPIO_PIN_96_SD0_DATA7           (((UINT32)2U << 12U) | GPIO_PIN_96)  /* Data pin 7 of SD port 0 */
#define GPIO_PIN_109_SD0_RESET          (((UINT32)2U << 12U) | GPIO_PIN_109) /* Reset pin of SD port 0 */
#define GPIO_PIN_156_SD0_HS_SEL         (((UINT32)1U << 12U) | GPIO_PIN_156) /* High speed mode select of SD port 0 */

/* SD1/SDXC */
#define GPIO_PIN_97_SD1_CLK             (((UINT32)2U << 12U) | GPIO_PIN_97)  /* Clock pin of SD port 1 (SDXC) */
#define GPIO_PIN_98_SD1_CMD             (((UINT32)2U << 12U) | GPIO_PIN_98)  /* Command pin of SD port 1 (SDXC) */
#define GPIO_PIN_99_SD1_DATA0           (((UINT32)2U << 12U) | GPIO_PIN_99)  /* Data pin 0 of SD port 1 (SDXC) */
#define GPIO_PIN_100_SD1_DATA1          (((UINT32)2U << 12U) | GPIO_PIN_100) /* Data pin 1 of SD port 1 (SDXC) */
#define GPIO_PIN_101_SD1_DATA2          (((UINT32)2U << 12U) | GPIO_PIN_101) /* Data pin 2 of SD port 1 (SDXC) */
#define GPIO_PIN_102_SD1_DATA3          (((UINT32)2U << 12U) | GPIO_PIN_102) /* Data pin 3 of SD port 1 (SDXC) */
#define GPIO_PIN_103_SD1_CD             (((UINT32)2U << 12U) | GPIO_PIN_103) /* Card detect pin of SD port 1 (SDXC) */
#define GPIO_PIN_104_SD1_WP             (((UINT32)2U << 12U) | GPIO_PIN_104) /* Write protect pin of SD port 1 (SDXC) */
#define GPIO_PIN_105_SD1_DATA4          (((UINT32)2U << 12U) | GPIO_PIN_105) /* Data pin 4 of SD port 1 (SDXC) */
#define GPIO_PIN_106_SD1_DATA5          (((UINT32)2U << 12U) | GPIO_PIN_106) /* Data pin 5 of SD port 1 (SDXC) */
#define GPIO_PIN_107_SD1_DATA6          (((UINT32)2U << 12U) | GPIO_PIN_107) /* Data pin 6 of SD port 1 (SDXC) */
#define GPIO_PIN_108_SD1_DATA7          (((UINT32)2U << 12U) | GPIO_PIN_108) /* Data pin 7 of SD port 1 (SDXC) */
#define GPIO_PIN_110_SD1_RESET          (((UINT32)2U << 12U) | GPIO_PIN_110) /* Reset pin of SD port 1 (SDXC) */
#define GPIO_PIN_157_SD1_HS_SEL         (((UINT32)1U << 12U) | GPIO_PIN_157) /* High speed mode select of SD port 1 (SDXC) */

/* Sensor Clock */
#define GPIO_PIN_114_SENSOR_CLK1        (((UINT32)1U << 12U) | GPIO_PIN_114) /* The 2nd sensor input clock (clk_si_1) */

/* SPI */
#define GPIO_PIN_22_SPI0_SCLK           (((UINT32)1U << 12U) | GPIO_PIN_22)  /* Serial Clock pin of the SPI master port 0 */
#define GPIO_PIN_23_SPI0_MOSI           (((UINT32)1U << 12U) | GPIO_PIN_23)  /* MOSI: TXD pin of the SPI master port 0 */
#define GPIO_PIN_24_SPI0_MISO           (((UINT32)1U << 12U) | GPIO_PIN_24)  /* MISO: RXD pin of the SPI master port 0 */
#define GPIO_PIN_25_SPI0_SS0            (((UINT32)1U << 12U) | GPIO_PIN_25)  /* The 1st Slave Select pin of the SPI master port 0 */
#define GPIO_PIN_26_SPI0_SS1            (((UINT32)1U << 12U) | GPIO_PIN_26)  /* The 2nd Slave Select pin of the SPI master port 0 */
#define GPIO_PIN_27_SPI0_SS2            (((UINT32)3U << 12U) | GPIO_PIN_27)  /* The 3rd Slave Select pin of the SPI master port 0 */
#define GPIO_PIN_32_SPI0_SS3            (((UINT32)3U << 12U) | GPIO_PIN_28)  /* The 4th Slave Select pin of the SPI master port 0 */
#define GPIO_PIN_29_SPI0_SS4            (((UINT32)3U << 12U) | GPIO_PIN_29)  /* The 5th Slave Select pin of the SPI master port 0 */
#define GPIO_PIN_30_SPI0_SS5            (((UINT32)3U << 12U) | GPIO_PIN_30)  /* The 6th Slave Select pin of the SPI master port 0 */
#define GPIO_PIN_31_SPI0_SS6            (((UINT32)3U << 12U) | GPIO_PIN_31)  /* The 7th Slave Select pin of the SPI master port 0 */
#define GPIO_PIN_36_SPI0_SS7            (((UINT32)3U << 12U) | GPIO_PIN_32)  /* The 8th Slave Select pin of the SPI master port 0 */

#define GPIO_PIN_0_SPI1_SCLK            (((UINT32)2U << 12U) | GPIO_PIN_0)   /* Serial Clock pin of the SPI master port 1 */
#define GPIO_PIN_1_SPI1_MOSI            (((UINT32)2U << 12U) | GPIO_PIN_1)   /* MOSI: TXD pin of the SPI master port 1 */
#define GPIO_PIN_2_SPI1_MISO            (((UINT32)2U << 12U) | GPIO_PIN_2)   /* MISO: RXD pin of the SPI master port 1 */
#define GPIO_PIN_3_SPI1_SS0             (((UINT32)2U << 12U) | GPIO_PIN_3)   /* The 1st Slave Select pin of the SPI master port 1 */
#define GPIO_PIN_4_SPI1_SS1             (((UINT32)2U << 12U) | GPIO_PIN_4)   /* The 2nd Slave Select pin of the SPI master port 1 */
#define GPIO_PIN_5_SPI1_SS2             (((UINT32)2U << 12U) | GPIO_PIN_5)   /* The 3rd Slave Select pin of the SPI master port 1 */
#define GPIO_PIN_6_SPI1_SS3             (((UINT32)2U << 12U) | GPIO_PIN_6)   /* The 4th Slave Select pin of the SPI master port 1 */
#define GPIO_PIN_35_SPI1_SCLK           (((UINT32)2U << 12U) | GPIO_PIN_35)  /* Serial Clock pin of the SPI master port 1 */
#define GPIO_PIN_36_SPI1_MOSI           (((UINT32)2U << 12U) | GPIO_PIN_36)  /* MOSI: TXD pin of the SPI master port 1 */
#define GPIO_PIN_37_SPI1_MISO           (((UINT32)2U << 12U) | GPIO_PIN_37)  /* MISO: RXD pin of the SPI master port 1 */
#define GPIO_PIN_38_SPI1_SS0            (((UINT32)2U << 12U) | GPIO_PIN_38)  /* The 1st Slave Select pin of the SPI master port 1 */
#define GPIO_PIN_138_SPI1_SCLK          (((UINT32)2U << 12U) | GPIO_PIN_138) /* Serial Clock pin of the SPI master port 1 */
#define GPIO_PIN_139_SPI1_MOSI          (((UINT32)2U << 12U) | GPIO_PIN_139) /* MOSI: TXD pin of the SPI master port 1 */
#define GPIO_PIN_140_SPI1_MISO          (((UINT32)2U << 12U) | GPIO_PIN_140) /* MISO: RXD pin of the SPI master port 1 */
#define GPIO_PIN_141_SPI1_SS0           (((UINT32)2U << 12U) | GPIO_PIN_141) /* The 1st Slave Select pin of the SPI master port 1 */

#define GPIO_PIN_27_SPI2_SCLK           (((UINT32)1U << 12U) | GPIO_PIN_27)  /* Serial Clock pin of the SPI master port 2 */
#define GPIO_PIN_28_SPI2_MOSI           (((UINT32)1U << 12U) | GPIO_PIN_28)  /* MOSI: TXD pin of the SPI master port 2 */
#define GPIO_PIN_29_SPI2_MISO           (((UINT32)1U << 12U) | GPIO_PIN_29)  /* MISO: RXD pin of the SPI master port 2 */
#define GPIO_PIN_30_SPI2_SS0            (((UINT32)1U << 12U) | GPIO_PIN_30)  /* The 1st Slave Select pin of the SPI master port 2 */

#define GPIO_PIN_31_SPI3_SCLK           (((UINT32)1U << 12U) | GPIO_PIN_31)  /* Serial Clock pin of the SPI master port 3 */
#define GPIO_PIN_32_SPI3_MOSI           (((UINT32)1U << 12U) | GPIO_PIN_32)  /* MOSI: TXD pin of the SPI master port 3 */
#define GPIO_PIN_33_SPI3_MISO           (((UINT32)1U << 12U) | GPIO_PIN_33)  /* MISO: RXD pin of the SPI master port 3 */
#define GPIO_PIN_34_SPI3_SS0            (((UINT32)1U << 12U) | GPIO_PIN_34)  /* The 1st Slave Select pin of the SPI master port 3 */

#define GPIO_PIN_35_SPI_SLAVE_SCLK      (((UINT32)1U << 12U) | GPIO_PIN_35)  /* Serial Clock pin of the SPI slave port */
#define GPIO_PIN_36_SPI_SLAVE_MOSI      (((UINT32)1U << 12U) | GPIO_PIN_36)  /* MOSI: RXD pin of the SPI slave port */
#define GPIO_PIN_37_SPI_SLAVE_MISO      (((UINT32)1U << 12U) | GPIO_PIN_37)  /* MISO: TXD pin of the SPI slave port */
#define GPIO_PIN_38_SPI_SLAVE_SS        (((UINT32)1U << 12U) | GPIO_PIN_38)  /* Slave Select pin of the SPI slave port  */

/* Timer */
#define GPIO_PIN_8_TIMER0               (((UINT32)1U << 12U) | GPIO_PIN_8)   /* External clock source of interval timer 0 */
#define GPIO_PIN_9_TIMER1               (((UINT32)1U << 12U) | GPIO_PIN_9)   /* External clock source of interval timer 1 */
#define GPIO_PIN_10_TIMER2              (((UINT32)1U << 12U) | GPIO_PIN_10)  /* External clock source of interval timer 2 */

/* UART */
#define GPIO_PIN_39_UART_APB_RXD        (((UINT32)1U << 12U) | GPIO_PIN_39)  /* Receive Data pin of the UART_APB port 0 */
#define GPIO_PIN_40_UART_APB_TXD        (((UINT32)1U << 12U) | GPIO_PIN_40)  /* Transmit Data pin of the UART_APB port 0 */

#define GPIO_PIN_138_UART0_RXD          (((UINT32)1U << 12U) | GPIO_PIN_138) /* Receive Data pin of the UART_AHB port 0 */
#define GPIO_PIN_139_UART0_TXD          (((UINT32)1U << 12U) | GPIO_PIN_139) /* Transmit Data pin of the UART_AHB port 0 */
#define GPIO_PIN_140_UART0_CTS_N        (((UINT32)1U << 12U) | GPIO_PIN_140) /* Clear-to-Send pin of the UART_AHB port 0 */
#define GPIO_PIN_141_UART0_RTS_N        (((UINT32)1U << 12U) | GPIO_PIN_141) /* Request-to-Send pin of the UART_AHB port 0 */

#define GPIO_PIN_142_UART1_RXD          (((UINT32)1U << 12U) | GPIO_PIN_142) /* Receive Data pin of the UART_AHB port 1 */
#define GPIO_PIN_143_UART1_TXD          (((UINT32)1U << 12U) | GPIO_PIN_143) /* Transmit Data pin of the UART_AHB port 1 */
#define GPIO_PIN_144_UART1_CTS_N        (((UINT32)1U << 12U) | GPIO_PIN_144) /* Clear-to-Send pin of the UART_AHB port 1 */
#define GPIO_PIN_145_UART1_RTS_N        (((UINT32)1U << 12U) | GPIO_PIN_145) /* Request-to-Send pin of the UART_AHB port 1 */

#define GPIO_PIN_146_UART2_RXD          (((UINT32)1U << 12U) | GPIO_PIN_146) /* Receive Data pin of the UART_AHB port 2 */
#define GPIO_PIN_147_UART2_TXD          (((UINT32)1U << 12U) | GPIO_PIN_147) /* Transmit Data pin of the UART_AHB port 2 */
#define GPIO_PIN_148_UART2_CTS_N        (((UINT32)1U << 12U) | GPIO_PIN_148) /* Clear-to-Send pin of the UART_AHB port 2 */
#define GPIO_PIN_149_UART2_RTS_N        (((UINT32)1U << 12U) | GPIO_PIN_149) /* Request-to-Send pin of the UART_AHB port 2 */

#define GPIO_PIN_150_UART3_RXD          (((UINT32)1U << 12U) | GPIO_PIN_150) /* Receive Data pin of the UART_AHB port 3 */
#define GPIO_PIN_151_UART3_TXD          (((UINT32)1U << 12U) | GPIO_PIN_151) /* Transmit Data pin of the UART_AHB port 3 */
#define GPIO_PIN_152_UART3_CTS_N        (((UINT32)1U << 12U) | GPIO_PIN_152) /* Clear-to-Send pin of the UART_AHB port 3 */
#define GPIO_PIN_153_UART3_RTS_N        (((UINT32)1U << 12U) | GPIO_PIN_153) /* Request-to-Send pin of the UART_AHB port 3 */

/* USB */
#define GPIO_PIN_35_USB0_EHCI_OC        (((UINT32)3U << 12U) | GPIO_PIN_35)  /* Over-Current pin of the USB EHCI port 0 */
#define GPIO_PIN_105_USB1_EHCI_OC       (((UINT32)3U << 12U) | GPIO_PIN_105) /* Over-Current pin of the USB EHCI port 1 */
#define GPIO_PIN_37_USB0_EHCI_PWR       (((UINT32)3U << 12U) | GPIO_PIN_37)  /* USB EHCI port 0 power enable */

#define GPIO_PIN_33_USB0_EHCI_OC        (((UINT32)5U << 12U) | GPIO_PIN_33)  /* Over-Current pin of the USB EHCI port 0 */
#define GPIO_PIN_32_USB0_EHCI_PWR       (((UINT32)5U << 12U) | GPIO_PIN_32)  /* USB EHCI port 0 power enable */
#define GPIO_PIN_105_USB0_EHCI_OC       (((UINT32)5U << 12U) | GPIO_PIN_105) /* Over-Current pin of the USB EHCI port 0 */
#define GPIO_PIN_106_USB1_EHCI_OC       (((UINT32)5U << 12U) | GPIO_PIN_106) /* Over-Current pin of the USB EHCI port 1 */
#define GPIO_PIN_107_USB0_EHCI_PWR      (((UINT32)5U << 12U) | GPIO_PIN_107) /* USB EHCI port 0 power enable */
#define GPIO_PIN_108_USB1_EHCI_PWR      (((UINT32)5U << 12U) | GPIO_PIN_108) /* USB EHCI port 1 power enable */

/* Video Input */
#define GPIO_PIN_115_VIN0_MASTER_HSYNC  (((UINT32)1U << 12U) | GPIO_PIN_115) /* VIN0 Master HSYNC */
#define GPIO_PIN_116_VIN0_MASTER_VSYNC  (((UINT32)1U << 12U) | GPIO_PIN_116) /* VIN0 Master VSYNC */

#define GPIO_PIN_9_VIN1_MASTER_VSYNC    (((UINT32)3U << 12U) | GPIO_PIN_9)   /* VIN1/PIP Master VSYNC */
#define GPIO_PIN_10_VIN1_MASTER_HSYNC   (((UINT32)3U << 12U) | GPIO_PIN_10)  /* VIN1/PIP Master HSYNC */
#define GPIO_PIN_115_VIN1_MASTER_HSYNC  (((UINT32)2U << 12U) | GPIO_PIN_115) /* VIN1/PIP Master HSYNC */
#define GPIO_PIN_116_VIN1_MASTER_VSYNC  (((UINT32)2U << 12U) | GPIO_PIN_116) /* VIN1/PIP Master VSYNC */

#define GPIO_PIN_5_VIN_STRIG_0          (((UINT32)3U << 12U) | GPIO_PIN_5)   /* VIN strig 0 */
#define GPIO_PIN_6_VIN_STRIG_1          (((UINT32)3U << 12U) | GPIO_PIN_6)   /* VIN strig 1 */

/* Video Output Display */
#define GPIO_PIN_117_VD0_OUT_0          (((UINT32)1U << 12U) | GPIO_PIN_117) /* Digital video output data pin 0 */
#define GPIO_PIN_118_VD0_OUT_1          (((UINT32)1U << 12U) | GPIO_PIN_118) /* Digital video output data pin 1 */
#define GPIO_PIN_119_VD0_OUT_2          (((UINT32)1U << 12U) | GPIO_PIN_119) /* Digital video output data pin 2 */
#define GPIO_PIN_120_VD0_OUT_3          (((UINT32)1U << 12U) | GPIO_PIN_120) /* Digital video output data pin 3 */
#define GPIO_PIN_121_VD0_OUT_4          (((UINT32)1U << 12U) | GPIO_PIN_121) /* Digital video output data pin 4 */
#define GPIO_PIN_122_VD0_OUT_5          (((UINT32)1U << 12U) | GPIO_PIN_122) /* Digital video output data pin 5 */
#define GPIO_PIN_123_VD0_OUT_6          (((UINT32)1U << 12U) | GPIO_PIN_123) /* Digital video output data pin 6 */
#define GPIO_PIN_124_VD0_OUT_7          (((UINT32)1U << 12U) | GPIO_PIN_124) /* Digital video output data pin 7 */
#define GPIO_PIN_125_VD0_OUT_8          (((UINT32)1U << 12U) | GPIO_PIN_125) /* Digital video output data pin 8 */
#define GPIO_PIN_126_VD0_OUT_9          (((UINT32)1U << 12U) | GPIO_PIN_126) /* Digital video output data pin 9 */
#define GPIO_PIN_127_VD0_OUT_10         (((UINT32)1U << 12U) | GPIO_PIN_127) /* Digital video output data pin 10 */
#define GPIO_PIN_128_VD0_OUT_11         (((UINT32)1U << 12U) | GPIO_PIN_128) /* Digital video output data pin 11 */
#define GPIO_PIN_129_VD0_OUT_12         (((UINT32)1U << 12U) | GPIO_PIN_129) /* Digital video output data pin 12U */
#define GPIO_PIN_130_VD0_OUT_13         (((UINT32)1U << 12U) | GPIO_PIN_130) /* Digital video output data pin 13 */
#define GPIO_PIN_131_VD0_OUT_14         (((UINT32)1U << 12U) | GPIO_PIN_131) /* Digital video output data pin 14 */
#define GPIO_PIN_132_VD0_OUT_15         (((UINT32)1U << 12U) | GPIO_PIN_132) /* Digital video output data pin 15 */
#define GPIO_PIN_133_VD0_CLK            (((UINT32)1U << 12U) | GPIO_PIN_133) /* Digital video output clock pin */
#define GPIO_PIN_134_VD0_VSYNC          (((UINT32)1U << 12U) | GPIO_PIN_134) /* Digital video output vsync pin */
#define GPIO_PIN_135_VD0_HSYNC          (((UINT32)1U << 12U) | GPIO_PIN_135) /* Digital video output hsync pin */
#define GPIO_PIN_136_VD0_HVLD           (((UINT32)1U << 12U) | GPIO_PIN_136) /* Digital video output hvld pin */

/* WDT */
#define GPIO_PIN_4_WDT_RST              (((UINT32)3U << 12U) | GPIO_PIN_4)   /* WDT extend reset */
#define GPIO_PIN_21_WDT_RST             (((UINT32)2U << 12U) | GPIO_PIN_21)  /* WDT extend reset */
#define GPIO_PIN_43_WDT_RST             (((UINT32)2U << 12U) | GPIO_PIN_43)  /* WDT extend reset */
#define GPIO_PIN_113_WDT_RST            (((UINT32)2U << 12U) | GPIO_PIN_113) /* WDT extend reset */

#define AMBA_GPIO_GROUP0                0x0U    /* GPIO pin 0~31    */
#define AMBA_GPIO_GROUP1                0x1U    /* GPIO pin 32~63   */
#define AMBA_GPIO_GROUP2                0x2U    /* GPIO pin 64~95   */
#define AMBA_GPIO_GROUP3                0x3U    /* GPIO pin 96~127  */
#define AMBA_GPIO_GROUP4                0x4U    /* GPIO pin 128~159 */
#define AMBA_NUM_GPIO_GROUP             0x5U

#endif /* AMBA_GPIO_PRIV_H */
