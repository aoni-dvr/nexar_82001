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
#define GPIO_PIN_000        0U
#define GPIO_PIN_001        1U
#define GPIO_PIN_002        2U
#define GPIO_PIN_003        3U
#define GPIO_PIN_004        4U
#define GPIO_PIN_005        5U
#define GPIO_PIN_006        6U
#define GPIO_PIN_007        7U
#define GPIO_PIN_008        8U
#define GPIO_PIN_009        9U
#define GPIO_PIN_010        10U
#define GPIO_PIN_011        11U
#define GPIO_PIN_012        12U
#define GPIO_PIN_013        13U
#define GPIO_PIN_014        14U
#define GPIO_PIN_015        15U
#define GPIO_PIN_016        16U
#define GPIO_PIN_017        17U
#define GPIO_PIN_018        18U
#define GPIO_PIN_019        19U
#define GPIO_PIN_020        20U
#define GPIO_PIN_021        21U
#define GPIO_PIN_022        22U
#define GPIO_PIN_023        23U
#define GPIO_PIN_024        24U
#define GPIO_PIN_025        25U
#define GPIO_PIN_026        26U
#define GPIO_PIN_027        27U
#define GPIO_PIN_028        28U
#define GPIO_PIN_029        29U
#define GPIO_PIN_030        30U
#define GPIO_PIN_031        31U
/* Group 1 */
#define GPIO_PIN_032        32U
#define GPIO_PIN_033        33U
#define GPIO_PIN_034        34U
#define GPIO_PIN_035        35U
#define GPIO_PIN_036        36U
#define GPIO_PIN_037        37U
#define GPIO_PIN_038        38U
#define GPIO_PIN_039        39U
#define GPIO_PIN_040        40U
#define GPIO_PIN_041        41U
#define GPIO_PIN_042        42U
#define GPIO_PIN_043        43U
#define GPIO_PIN_044        44U
#define GPIO_PIN_045        45U
#define GPIO_PIN_046        46U
#define GPIO_PIN_047        47U
#define GPIO_PIN_048        48U
#define GPIO_PIN_049        49U
#define GPIO_PIN_050        50U
#define GPIO_PIN_051        51U
#define GPIO_PIN_052        52U
#define GPIO_PIN_053        53U
#define GPIO_PIN_054        54U
#define GPIO_PIN_055        55U
#define GPIO_PIN_056        56U
#define GPIO_PIN_057        57U
#define GPIO_PIN_058        58U
#define GPIO_PIN_059        59U
#define GPIO_PIN_060        60U
#define GPIO_PIN_061        61U
#define GPIO_PIN_062        62U
#define GPIO_PIN_063        63U
/* Group 2 */
#define GPIO_PIN_064        64U
#define GPIO_PIN_065        65U
#define GPIO_PIN_066        66U
#define GPIO_PIN_067        67U
#define GPIO_PIN_068        68U
#define GPIO_PIN_069        69U
#define GPIO_PIN_070        70U
#define GPIO_PIN_071        71U
#define GPIO_PIN_072        72U
#define GPIO_PIN_073        73U
#define GPIO_PIN_074        74U
#define GPIO_PIN_075        75U
#define GPIO_PIN_076        76U
#define GPIO_PIN_077        77U
#define GPIO_PIN_078        78U
#define GPIO_PIN_079        79U
#define GPIO_PIN_080        80U
#define GPIO_PIN_081        81U
#define GPIO_PIN_082        82U
#define GPIO_PIN_083        83U
#define GPIO_PIN_084        84U
#define GPIO_PIN_085        85U
#define GPIO_PIN_086        86U
#define GPIO_PIN_087        87U
#define GPIO_PIN_088        88U
#define GPIO_PIN_089        89U
#define GPIO_PIN_090        90U
#define GPIO_PIN_091        91U
#define GPIO_PIN_092        92U
#define GPIO_PIN_093        93U
#define GPIO_PIN_094        94U
#define GPIO_PIN_095        95U
/* Group 3 */
#define GPIO_PIN_096        96U
#define GPIO_PIN_097        97U
#define GPIO_PIN_098        98U
#define GPIO_PIN_099        99U
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

/* CAN Bus */
#define GPIO_PIN_098_CAN0_RX            (((UINT32)1U << 12U) | GPIO_PIN_098)    /* RX pin of the CAN bus controller 0 */
#define GPIO_PIN_099_CAN0_TX            (((UINT32)1U << 12U) | GPIO_PIN_099)    /* TX pin of the CAN bus controller 0 */
#define GPIO_PIN_100_CAN1_RX            (((UINT32)1U << 12U) | GPIO_PIN_100)    /* RX pin of the CAN bus controller 1 */
#define GPIO_PIN_101_CAN1_TX            (((UINT32)1U << 12U) | GPIO_PIN_101)    /* TX pin of the CAN bus controller 1 */
#define GPIO_PIN_044_CAN2_RX            (((UINT32)5U << 12U) | GPIO_PIN_044)    /* RX pin of the CAN bus controller 2 */
#define GPIO_PIN_048_CAN2_TX            (((UINT32)5U << 12U) | GPIO_PIN_048)    /* TX pin of the CAN bus controller 2 */
#define GPIO_PIN_053_CAN3_RX            (((UINT32)5U << 12U) | GPIO_PIN_053)    /* RX pin of the CAN bus controller 3 */
#define GPIO_PIN_054_CAN3_TX            (((UINT32)5U << 12U) | GPIO_PIN_054)    /* TX pin of the CAN bus controller 3 */
#define GPIO_PIN_055_CAN4_RX            (((UINT32)5U << 12U) | GPIO_PIN_055)    /* RX pin of the CAN bus controller 4 */
#define GPIO_PIN_056_CAN4_TX            (((UINT32)5U << 12U) | GPIO_PIN_056)    /* TX pin of the CAN bus controller 4 */
#define GPIO_PIN_057_CAN5_RX            (((UINT32)5U << 12U) | GPIO_PIN_057)    /* RX pin of the CAN bus controller 5 */
#define GPIO_PIN_058_CAN5_TX            (((UINT32)5U << 12U) | GPIO_PIN_058)    /* TX pin of the CAN bus controller 5 */

#define GPIO_PIN_DEBOUNCE_GPIO0         0U
#define GPIO_PIN_DEBOUNCE_GPIO1         0U

/* Ethernet */
#define GPIO_PIN_027_ENET0_TXEN         (((UINT32)1U << 12U) | GPIO_PIN_027)    /* Ethernet 0 RMII TXEN (TX data ready to transmit) */
#define GPIO_PIN_028_ENET0_TXD0         (((UINT32)1U << 12U) | GPIO_PIN_028)    /* Ethernet 0 RMII transmit data bit 0 */
#define GPIO_PIN_029_ENET0_TXD1         (((UINT32)1U << 12U) | GPIO_PIN_029)    /* Ethernet 0 RMII transmit data bit 1 */
#define GPIO_PIN_032_ENET0_RXD0         (((UINT32)1U << 12U) | GPIO_PIN_032)    /* Ethernet 0 RMII receive data bit 0 */
#define GPIO_PIN_033_ENET0_RXD1         (((UINT32)1U << 12U) | GPIO_PIN_033)    /* Ethernet 0 RMII receive data bit 1 */
#define GPIO_PIN_036_ENET0_RXDV         (((UINT32)1U << 12U) | GPIO_PIN_036)    /* Ethernet 0 RMII RXDV (RX data valid) */
#define GPIO_PIN_037_ENET0_MDC          (((UINT32)1U << 12U) | GPIO_PIN_037)    /* Ethernet 0 RMII MDC pin */
#define GPIO_PIN_038_ENET0_MDIO         (((UINT32)1U << 12U) | GPIO_PIN_038)    /* Ethernet 0 RMII MDIO pin */
#define GPIO_PIN_039_ENET0_PTP_PPS      (((UINT32)1U << 12U) | GPIO_PIN_039)    /* Ethernet 0 RMII PTP PPS */
#define GPIO_PIN_040_ENET0_REF_CLK0     (((UINT32)1U << 12U) | GPIO_PIN_040)    /* Ethernet 0 RMII 1st reference clock */
#define GPIO_PIN_041_ENET0_REF_CLK1     (((UINT32)1U << 12U) | GPIO_PIN_041)    /* Ethernet 0 RMII 2nd reference clock */
#define GPIO_PIN_042_ENET0_EXT_OSC      (((UINT32)1U << 12U) | GPIO_PIN_042)    /* Ethernet 0 RMII external clock source */
#define GPIO_PIN_043_ENET0_REF_CLK1     (((UINT32)1U << 12U) | GPIO_PIN_043)    /* Ethernet 0 RMII 2nd reference clock */

#define GPIO_PIN_027_ENET0_G_TXEN       (((UINT32)2U << 12U) | GPIO_PIN_027)    /* Ethernet 0 RGMII TXEN (TX data ready to transmit) */
#define GPIO_PIN_028_ENET0_G_TXD0       (((UINT32)2U << 12U) | GPIO_PIN_028)    /* Ethernet 0 RGMII transmit data bit 0 */
#define GPIO_PIN_029_ENET0_G_TXD1       (((UINT32)2U << 12U) | GPIO_PIN_029)    /* Ethernet 0 RGMII transmit data bit 1 */
#define GPIO_PIN_030_ENET0_G_TXD2       (((UINT32)2U << 12U) | GPIO_PIN_030)    /* Ethernet 0 RGMII transmit data bit 2 */
#define GPIO_PIN_031_ENET0_G_TXD3       (((UINT32)2U << 12U) | GPIO_PIN_031)    /* Ethernet 0 RGMII transmit data bit 3 */
#define GPIO_PIN_032_ENET0_G_RXD0       (((UINT32)2U << 12U) | GPIO_PIN_032)    /* Ethernet 0 RGMII receive data bit 0 */
#define GPIO_PIN_033_ENET0_G_RXD1       (((UINT32)2U << 12U) | GPIO_PIN_033)    /* Ethernet 0 RGMII receive data bit 1 */
#define GPIO_PIN_034_ENET0_G_RXD2       (((UINT32)2U << 12U) | GPIO_PIN_034)    /* Ethernet 0 RGMII receive data bit 2 */
#define GPIO_PIN_035_ENET0_G_RXD3       (((UINT32)2U << 12U) | GPIO_PIN_035)    /* Ethernet 0 RGMII receive data bit 3 */
#define GPIO_PIN_036_ENET0_G_RXDV       (((UINT32)2U << 12U) | GPIO_PIN_036)    /* Ethernet 0 RGMII RXDV (RX data valid) */
#define GPIO_PIN_037_ENET0_G_MDC        (((UINT32)2U << 12U) | GPIO_PIN_037)    /* Ethernet 0 RGMII MDC pin */
#define GPIO_PIN_038_ENET0_G_MDIO       (((UINT32)2U << 12U) | GPIO_PIN_038)    /* Ethernet 0 RGMII MDIO pin */
#define GPIO_PIN_039_ENET0_G_PTP_PPS    (((UINT32)2U << 12U) | GPIO_PIN_039)    /* Ethernet 0 RGMII PTP PPS */
#define GPIO_PIN_040_ENET0_G_CLK_RX     (((UINT32)2U << 12U) | GPIO_PIN_040)    /* Ethernet 0 RGMII 1st reference clock */
#define GPIO_PIN_041_ENET0_G_GTX_CLK    (((UINT32)2U << 12U) | GPIO_PIN_041)    /* Ethernet 0 RGMII clock */
#define GPIO_PIN_042_ENET0_G_EXT_OSC    (((UINT32)2U << 12U) | GPIO_PIN_042)    /* Ethernet 0 RGMII external clock source */
#define GPIO_PIN_043_ENET0_G_REF_CLK1   (((UINT32)2U << 12U) | GPIO_PIN_043)    /* Ethernet 0 RGMII 1st reference clock */

#define GPIO_PIN_044_ENET1_TXEN         (((UINT32)1U << 12U) | GPIO_PIN_044)    /* Ethernet 1 RMII TXEN (TX data ready to transmit) */
#define GPIO_PIN_045_ENET1_TXD0         (((UINT32)1U << 12U) | GPIO_PIN_045)    /* Ethernet 1 RMII transmit data bit 0 */
#define GPIO_PIN_046_ENET1_TXD1         (((UINT32)1U << 12U) | GPIO_PIN_046)    /* Ethernet 1 RMII transmit data bit 1 */
#define GPIO_PIN_049_ENET1_RXD0         (((UINT32)1U << 12U) | GPIO_PIN_049)    /* Ethernet 1 RMII receive data bit 0 */
#define GPIO_PIN_050_ENET1_RXD1         (((UINT32)1U << 12U) | GPIO_PIN_050)    /* Ethernet 1 RMII receive data bit 1 */
#define GPIO_PIN_053_ENET1_RXDV         (((UINT32)1U << 12U) | GPIO_PIN_053)    /* Ethernet 1 RMII RXDV (RX data valid) */
#define GPIO_PIN_054_ENET1_MDC          (((UINT32)1U << 12U) | GPIO_PIN_054)    /* Ethernet 1 RMII MDC pin */
#define GPIO_PIN_055_ENET1_MDIO         (((UINT32)1U << 12U) | GPIO_PIN_055)    /* Ethernet 1 RMII MDIO pin */
#define GPIO_PIN_056_ENET1_PTP_PPS      (((UINT32)1U << 12U) | GPIO_PIN_056)    /* Ethernet 1 RMII PTP PPS */
#define GPIO_PIN_057_ENET1_REF_CLK0     (((UINT32)1U << 12U) | GPIO_PIN_057)    /* Ethernet 1 RMII 1st reference clock */
#define GPIO_PIN_058_ENET1_REF_CLK1     (((UINT32)1U << 12U) | GPIO_PIN_058)    /* Ethernet 1 RMII 2nd reference clock */

#define GPIO_PIN_044_ENET1_G_TXEN       (((UINT32)2U << 12U) | GPIO_PIN_044)    /* Ethernet 1 RGMII TXEN (TX data ready to transmit) */
#define GPIO_PIN_045_ENET1_G_TXD0       (((UINT32)2U << 12U) | GPIO_PIN_045)    /* Ethernet 1 RGMII transmit data bit 0 */
#define GPIO_PIN_046_ENET1_G_TXD1       (((UINT32)2U << 12U) | GPIO_PIN_046)    /* Ethernet 1 RGMII transmit data bit 1 */
#define GPIO_PIN_047_ENET1_G_TXD2       (((UINT32)2U << 12U) | GPIO_PIN_047)    /* Ethernet 1 RGMII transmit data bit 2 */
#define GPIO_PIN_048_ENET1_G_TXD3       (((UINT32)2U << 12U) | GPIO_PIN_048)    /* Ethernet 1 RGMII transmit data bit 3 */
#define GPIO_PIN_049_ENET1_G_RXD0       (((UINT32)2U << 12U) | GPIO_PIN_049)    /* Ethernet 1 RGMII receive data bit 0 */
#define GPIO_PIN_050_ENET1_G_RXD1       (((UINT32)2U << 12U) | GPIO_PIN_050)    /* Ethernet 1 RGMII receive data bit 1 */
#define GPIO_PIN_051_ENET1_G_RXD2       (((UINT32)2U << 12U) | GPIO_PIN_051)    /* Ethernet 1 RGMII receive data bit 2 */
#define GPIO_PIN_052_ENET1_G_RXD3       (((UINT32)2U << 12U) | GPIO_PIN_052)    /* Ethernet 1 RGMII receive data bit 3 */
#define GPIO_PIN_053_ENET1_G_RXDV       (((UINT32)2U << 12U) | GPIO_PIN_053)    /* Ethernet 1 RGMII RXDV (RX data valid) */
#define GPIO_PIN_054_ENET1_G_MDC        (((UINT32)2U << 12U) | GPIO_PIN_054)    /* Ethernet 1 RGMII MDC pin */
#define GPIO_PIN_055_ENET1_G_MDIO       (((UINT32)2U << 12U) | GPIO_PIN_055)    /* Ethernet 1 RGMII MDIO pin */
#define GPIO_PIN_056_ENET1_G_PTP_PPS    (((UINT32)2U << 12U) | GPIO_PIN_056)    /* Ethernet 1 RGMII PTP PPS */
#define GPIO_PIN_057_ENET1_G_CLK_RX     (((UINT32)2U << 12U) | GPIO_PIN_057)    /* Ethernet 1 RGMII 1st reference clock */
#define GPIO_PIN_058_ENET1_G_GTX_CLK    (((UINT32)2U << 12U) | GPIO_PIN_058)    /* Ethernet 1 RGMII clock */

#define GPIO_PIN_037_ENET0_SG_MDC       (((UINT32)4U << 12U) | GPIO_PIN_037)    /* Ethernet 0 SGMII Management Data Clock */
#define GPIO_PIN_038_ENET0_SG_MDIO      (((UINT32)4U << 12U) | GPIO_PIN_038)    /* Ethernet 0 SGMII Management Data */
#define GPIO_PIN_054_ENET1_SG_MDC       (((UINT32)3U << 12U) | GPIO_PIN_054)    /* Ethernet 1 SGMII Management Data Clock */
#define GPIO_PIN_055_ENET1_SG_MDIO      (((UINT32)3U << 12U) | GPIO_PIN_055)    /* Ethernet 1 SGMII Management Data */

/* I2C */
#define GPIO_PIN_001_I2C_SLAVE_CLK      (((UINT32)1U << 12U) | GPIO_PIN_001)    /* Clock pin of the I2C slave port */
#define GPIO_PIN_002_I2C_SLAVE_DATA     (((UINT32)1U << 12U) | GPIO_PIN_002)    /* Data pin of the I2C slave port */
#define GPIO_PIN_003_I2C0_CLK           (((UINT32)2U << 12U) | GPIO_PIN_003)    /* Clock pin of the 1st I2C master port */
#define GPIO_PIN_004_I2C0_DATA          (((UINT32)2U << 12U) | GPIO_PIN_004)    /* Data pin of the 1st I2C master port */
#define GPIO_PIN_005_I2C1_CLK           (((UINT32)2U << 12U) | GPIO_PIN_005)    /* Clock pin of the 2nd I2C master port */
#define GPIO_PIN_006_I2C1_DATA          (((UINT32)2U << 12U) | GPIO_PIN_006)    /* Data pin of the 2nd I2C master port */
#define GPIO_PIN_008_I2C2_CLK           (((UINT32)2U << 12U) | GPIO_PIN_008)    /* Clock pin of the 3rd I2C master port */
#define GPIO_PIN_009_I2C2_DATA          (((UINT32)2U << 12U) | GPIO_PIN_009)    /* Data pin of the 3rd I2C master port */
#define GPIO_PIN_010_I2C3_CLK           (((UINT32)2U << 12U) | GPIO_PIN_010)    /* Clock pin of the 4th I2C master port */
#define GPIO_PIN_011_I2C3_DATA          (((UINT32)2U << 12U) | GPIO_PIN_011)    /* Data pin of the 4th I2C master port */
#define GPIO_PIN_012_I2C4_CLK           (((UINT32)2U << 12U) | GPIO_PIN_012)    /* Clock pin of the 5th I2C master port */
#define GPIO_PIN_013_I2C4_DATA          (((UINT32)2U << 12U) | GPIO_PIN_013)    /* Data pin of the 5th I2C master port */
#define GPIO_PIN_014_I2C5_CLK           (((UINT32)2U << 12U) | GPIO_PIN_014)    /* Clock pin of the 6th I2C master port */
#define GPIO_PIN_015_I2C5_DATA          (((UINT32)2U << 12U) | GPIO_PIN_015)    /* Data pin of the 6th I2C master port */
#define GPIO_PIN_051_I2C5_CLK           (((UINT32)1U << 12U) | GPIO_PIN_051)    /* Clock pin of the 6th I2C master port */
#define GPIO_PIN_052_I2C5_DATA          (((UINT32)1U << 12U) | GPIO_PIN_052)    /* Data pin of the 6th I2C master port */

/* I2S */
#define GPIO_PIN_021_I2S0_CLK           (((UINT32)1U << 12U) | GPIO_PIN_021)
#define GPIO_PIN_022_I2S0_SI_0          (((UINT32)1U << 12U) | GPIO_PIN_022)    /* Serial data input pin of the I2S controller */
#define GPIO_PIN_023_I2S0_SO_0          (((UINT32)1U << 12U) | GPIO_PIN_023)    /* Serial data output pin of the I2S controller */
#define GPIO_PIN_024_I2S0_SI_1          (((UINT32)1U << 12U) | GPIO_PIN_024)    /* Serial data input pin of the I2S controller */
#define GPIO_PIN_025_I2S0_SO_1          (((UINT32)1U << 12U) | GPIO_PIN_025)    /* Serial data output pin of the I2S controller */
#define GPIO_PIN_026_I2S0_WS            (((UINT32)1U << 12U) | GPIO_PIN_026)    /* Word select pin of the I2S controller */

/* NAND (SPI) */
#define GPIO_PIN_059_NAND_SPI_CLK       (((UINT32)2U << 12U) | GPIO_PIN_059)    /* SPI Clock for NAND flash */
#define GPIO_PIN_060_NAND_SPI_EN        (((UINT32)2U << 12U) | GPIO_PIN_060)    /* SPI Slave Select for NAND flash */
#define GPIO_PIN_065_NAND_SPI_DATA0     (((UINT32)2U << 12U) | GPIO_PIN_065)    /* Data[0] for NAND flash */
#define GPIO_PIN_066_NAND_SPI_DATA1     (((UINT32)2U << 12U) | GPIO_PIN_066)    /* Data[1] for NAND flash */
#define GPIO_PIN_067_NAND_SPI_DATA2     (((UINT32)2U << 12U) | GPIO_PIN_067)    /* Data[2] for NAND flash */
#define GPIO_PIN_068_NAND_SPI_DATA3     (((UINT32)2U << 12U) | GPIO_PIN_068)    /* Data[3] for NAND flash */

#define GPIO_PIN_077_NAND_SPI_CLK       (((UINT32)4U << 12U) | GPIO_PIN_077)    /* SPI Clock for NAND flash */
#define GPIO_PIN_078_NAND_SPI_EN        (((UINT32)4U << 12U) | GPIO_PIN_078)    /* SPI Slave Select for NAND flash */
#define GPIO_PIN_079_NAND_SPI_DATA0     (((UINT32)4U << 12U) | GPIO_PIN_079)    /* Data[0] for NAND flash */
#define GPIO_PIN_080_NAND_SPI_DATA1     (((UINT32)4U << 12U) | GPIO_PIN_080)    /* Data[1] for NAND flash */
#define GPIO_PIN_081_NAND_SPI_DATA2     (((UINT32)4U << 12U) | GPIO_PIN_081)    /* Data[2] for NAND flash */
#define GPIO_PIN_082_NAND_SPI_DATA3     (((UINT32)4U << 12U) | GPIO_PIN_082)    /* Data[3] for NAND flash */

/* NOR (SPI) */
#define GPIO_PIN_059_NOR_SPI_CLK        (((UINT32)3U << 12U) | GPIO_PIN_059)    /* SPI Clock for NOR flash */
#define GPIO_PIN_060_NOR_SPI_EN         (((UINT32)3U << 12U) | GPIO_PIN_060)    /* SPI Slave Select for NOR flash */
#define GPIO_PIN_065_NOR_SPI_DATA0      (((UINT32)3U << 12U) | GPIO_PIN_065)    /* Data[0] for NOR flash */
#define GPIO_PIN_066_NOR_SPI_DATA1      (((UINT32)3U << 12U) | GPIO_PIN_066)    /* Data[1] for NOR flash */
#define GPIO_PIN_067_NOR_SPI_DATA2      (((UINT32)3U << 12U) | GPIO_PIN_067)    /* Data[2] for NOR flash */
#define GPIO_PIN_068_NOR_SPI_DATA3      (((UINT32)3U << 12U) | GPIO_PIN_068)    /* Data[3] for NOR flash */
#define GPIO_PIN_073_NOR_SPI_DATA4      (((UINT32)3U << 12U) | GPIO_PIN_073)    /* Data[4] for NOR flash */
#define GPIO_PIN_074_NOR_SPI_DATA5      (((UINT32)3U << 12U) | GPIO_PIN_074)    /* Data[5] for NOR flash */
#define GPIO_PIN_075_NOR_SPI_DATA6      (((UINT32)3U << 12U) | GPIO_PIN_075)    /* Data[6] for NOR flash */
#define GPIO_PIN_076_NOR_SPI_DATA7      (((UINT32)3U << 12U) | GPIO_PIN_076)    /* Data[7] for NOR flash */
#define GPIO_PIN_085_NOR_SPI_DQS        (((UINT32)3U << 12U) | GPIO_PIN_085)    /* DQS input for NOR flash */

#define GPIO_PIN_077_NOR_SPI_CLK        (((UINT32)5U << 12U) | GPIO_PIN_077)    /* SPI Clock for NOR flash */
#define GPIO_PIN_078_NOR_SPI_EN         (((UINT32)5U << 12U) | GPIO_PIN_078)    /* SPI Slave Select for NOR flash */
#define GPIO_PIN_079_NOR_SPI_DATA0      (((UINT32)5U << 12U) | GPIO_PIN_079)    /* Data[0] for NOR flash */
#define GPIO_PIN_080_NOR_SPI_DATA1      (((UINT32)5U << 12U) | GPIO_PIN_080)    /* Data[1] for NOR flash */
#define GPIO_PIN_081_NOR_SPI_DATA2      (((UINT32)5U << 12U) | GPIO_PIN_081)    /* Data[2] for NOR flash */
#define GPIO_PIN_082_NOR_SPI_DATA3      (((UINT32)5U << 12U) | GPIO_PIN_082)    /* Data[3] for NOR flash */

/* PWM */
#define GPIO_PIN_000_PWM3               (((UINT32)3U << 12U) | GPIO_PIN_000)    /* Output pin of the PWM controller 3 */
#define GPIO_PIN_003_PWM4               (((UINT32)3U << 12U) | GPIO_PIN_003)    /* Output pin of the PWM controller 4 */
#define GPIO_PIN_004_PWM5               (((UINT32)3U << 12U) | GPIO_PIN_004)    /* Output pin of the PWM controller 5 */
#define GPIO_PIN_005_PWM6               (((UINT32)3U << 12U) | GPIO_PIN_005)    /* Output pin of the PWM controller 6 */
#define GPIO_PIN_006_PWM7               (((UINT32)3U << 12U) | GPIO_PIN_006)    /* Output pin of the PWM controller 7 */
#define GPIO_PIN_018_PWM0               (((UINT32)1U << 12U) | GPIO_PIN_018)    /* Output pin of the PWM controller 0 */
#define GPIO_PIN_019_PWM1               (((UINT32)1U << 12U) | GPIO_PIN_019)    /* Output pin of the PWM controller 1 */
#define GPIO_PIN_020_PWM2               (((UINT32)1U << 12U) | GPIO_PIN_020)    /* Output pin of the PWM controller 2 */
#define GPIO_PIN_027_PWM8               (((UINT32)3U << 12U) | GPIO_PIN_027)    /* Output pin of the PWM controller 8 */
#define GPIO_PIN_028_PWM9               (((UINT32)3U << 12U) | GPIO_PIN_028)    /* Output pin of the PWM controller 9 */
#define GPIO_PIN_029_PWM10              (((UINT32)3U << 12U) | GPIO_PIN_029)    /* Output pin of the PWM controller 10 */
#define GPIO_PIN_030_PWM11              (((UINT32)3U << 12U) | GPIO_PIN_030)    /* Output pin of the PWM controller 11 */
#define GPIO_PIN_035_PWM3               (((UINT32)3U << 12U) | GPIO_PIN_035)    /* Output pin of the PWM controller 3 */
#define GPIO_PIN_036_PWM4               (((UINT32)3U << 12U) | GPIO_PIN_036)    /* Output pin of the PWM controller 4 */
#define GPIO_PIN_037_PWM5               (((UINT32)3U << 12U) | GPIO_PIN_037)    /* Output pin of the PWM controller 5 */
#define GPIO_PIN_038_PWM6               (((UINT32)3U << 12U) | GPIO_PIN_038)    /* Output pin of the PWM controller 6 */
#define GPIO_PIN_039_PWM7               (((UINT32)3U << 12U) | GPIO_PIN_039)    /* Output pin of the PWM controller 7 */
#define GPIO_PIN_040_PWM8               (((UINT32)3U << 12U) | GPIO_PIN_040)    /* Output pin of the PWM controller 8 */
#define GPIO_PIN_041_PWM9               (((UINT32)3U << 12U) | GPIO_PIN_041)    /* Output pin of the PWM controller 9 */
#define GPIO_PIN_045_PWM11              (((UINT32)5U << 12U) | GPIO_PIN_045)    /* Output pin of the PWM controller 11 */
#define GPIO_PIN_090_PWM10              (((UINT32)5U << 12U) | GPIO_PIN_090)    /* Output pin of the PWM controller 10 */

/* SD0/SD */
#define GPIO_PIN_061_SD0_CLK            (((UINT32)2U << 12U) | GPIO_PIN_061)    /* Clock pin of SD port 0 */
#define GPIO_PIN_062_SD0_CMD            (((UINT32)2U << 12U) | GPIO_PIN_062)    /* Command pin of SD port 0 */
#define GPIO_PIN_063_SD0_CD             (((UINT32)2U << 12U) | GPIO_PIN_063)    /* Card detect pin of SD port 0 */
#define GPIO_PIN_064_SD0_WP             (((UINT32)2U << 12U) | GPIO_PIN_064)    /* Write protect pin of SD port 0 */
#define GPIO_PIN_069_SD0_DATA0          (((UINT32)2U << 12U) | GPIO_PIN_069)    /* Data pin 0 of SD port 0 */
#define GPIO_PIN_070_SD0_DATA1          (((UINT32)2U << 12U) | GPIO_PIN_070)    /* Data pin 1 of SD port 0 */
#define GPIO_PIN_071_SD0_DATA2          (((UINT32)2U << 12U) | GPIO_PIN_071)    /* Data pin 2 of SD port 0 */
#define GPIO_PIN_072_SD0_DATA3          (((UINT32)2U << 12U) | GPIO_PIN_072)    /* Data pin 3 of SD port 0 */
#define GPIO_PIN_073_SD0_DATA4          (((UINT32)2U << 12U) | GPIO_PIN_073)    /* Data pin 4 of SD port 0 */
#define GPIO_PIN_074_SD0_DATA5          (((UINT32)2U << 12U) | GPIO_PIN_074)    /* Data pin 5 of SD port 0 */
#define GPIO_PIN_075_SD0_DATA6          (((UINT32)2U << 12U) | GPIO_PIN_075)    /* Data pin 6 of SD port 0 */
#define GPIO_PIN_076_SD0_DATA7          (((UINT32)2U << 12U) | GPIO_PIN_076)    /* Data pin 7 of SD port 0 */
#define GPIO_PIN_086_SD0_RESET          (((UINT32)2U << 12U) | GPIO_PIN_086)    /* Reset pin of SD port 0 */
#define GPIO_PIN_102_SD0_HS_SEL         (((UINT32)1U << 12U) | GPIO_PIN_102)    /* High speed mode select of SD port 0 */

/* SD1/SDIO0 */
#define GPIO_PIN_077_SD1_CLK            (((UINT32)2U << 12U) | GPIO_PIN_077)    /* Clock pin of SD port 1 */
#define GPIO_PIN_078_SD1_CMD            (((UINT32)2U << 12U) | GPIO_PIN_078)    /* Command pin of SD port 1 */
#define GPIO_PIN_079_SD1_DATA0          (((UINT32)2U << 12U) | GPIO_PIN_079)    /* Data pin 0 of SD port 1 */
#define GPIO_PIN_080_SD1_DATA1          (((UINT32)2U << 12U) | GPIO_PIN_080)    /* Data pin 1 of SD port 1 */
#define GPIO_PIN_081_SD1_DATA2          (((UINT32)2U << 12U) | GPIO_PIN_081)    /* Data pin 2 of SD port 1 */
#define GPIO_PIN_082_SD1_DATA3          (((UINT32)2U << 12U) | GPIO_PIN_082)    /* Data pin 3 of SD port 1 */
#define GPIO_PIN_083_SD1_CD             (((UINT32)2U << 12U) | GPIO_PIN_083)    /* Card detect pin of SD port 1 */
#define GPIO_PIN_084_SD1_WP             (((UINT32)2U << 12U) | GPIO_PIN_084)    /* Write protect pin of SD port 1 */
#define GPIO_PIN_000_SD1_HS_SEL         (((UINT32)5U << 12U) | GPIO_PIN_000)    /* High speed mode select of SD port 1 */
#define GPIO_PIN_007_SD1_HS_SEL         (((UINT32)4U << 12U) | GPIO_PIN_007)    /* High speed mode select of SD port 1 */

/* Serial Peripheral Interface */
#define GPIO_PIN_003_SPI0_SCLK          (((UINT32)1U << 12U) | GPIO_PIN_003)    /* Serial Clock pin of the SPI master port 0 */
#define GPIO_PIN_004_SPI0_MOSI          (((UINT32)1U << 12U) | GPIO_PIN_004)    /* MOSI: TXD pin of the SPI master port 0 */
#define GPIO_PIN_005_SPI0_MISO          (((UINT32)1U << 12U) | GPIO_PIN_005)    /* MISO: RXD pin of the SPI master port 0 */
#define GPIO_PIN_006_SPI0_SS0           (((UINT32)1U << 12U) | GPIO_PIN_006)    /* The 1st Slave Select pin of the SPI master port 0 */
#define GPIO_PIN_007_SPI0_SS1           (((UINT32)1U << 12U) | GPIO_PIN_007)    /* The 2nd Slave Select pin of the SPI master port 0 */
#define GPIO_PIN_008_SPI0_SS2           (((UINT32)3U << 12U) | GPIO_PIN_008)    /* The 3rd Slave Select pin of the SPI master port 0 */
#define GPIO_PIN_043_SPI0_SS2           (((UINT32)4U << 12U) | GPIO_PIN_043)    /* The 3rd Slave Select pin of the SPI master port 0 */
#define GPIO_PIN_009_SPI0_SS3           (((UINT32)3U << 12U) | GPIO_PIN_009)    /* The 4th Slave Select pin of the SPI master port 0 */
#define GPIO_PIN_010_SPI0_SS4           (((UINT32)3U << 12U) | GPIO_PIN_010)    /* The 5th Slave Select pin of the SPI master port 0 */
#define GPIO_PIN_011_SPI0_SS5           (((UINT32)3U << 12U) | GPIO_PIN_011)    /* The 6th Slave Select pin of the SPI master port 0 */
#define GPIO_PIN_001_SPI0_SS6           (((UINT32)3U << 12U) | GPIO_PIN_001)    /* The 7th Slave Select pin of the SPI master port 0 */
#define GPIO_PIN_002_SPI0_SS7           (((UINT32)3U << 12U) | GPIO_PIN_002)    /* The 8th Slave Select pin of the SPI master port 0 */

#define GPIO_PIN_021_SPI1_SCLK          (((UINT32)2U << 12U) | GPIO_PIN_021)    /* Serial Clock pin of the SPI master port 1 */
#define GPIO_PIN_022_SPI1_MOSI          (((UINT32)2U << 12U) | GPIO_PIN_022)    /* MOSI: TXD pin of the SPI master port 1 */
#define GPIO_PIN_024_SPI1_MISO          (((UINT32)2U << 12U) | GPIO_PIN_024)    /* MISO: RXD pin of the SPI master port 1 */
#define GPIO_PIN_023_SPI1_SS0           (((UINT32)2U << 12U) | GPIO_PIN_023)    /* The 1st Slave Select pin of the SPI master port 1 */

#define GPIO_PIN_008_SPI2_SCLK          (((UINT32)1U << 12U) | GPIO_PIN_008)    /* Serial Clock pin of the SPI master port 2 */
#define GPIO_PIN_009_SPI2_MOSI          (((UINT32)1U << 12U) | GPIO_PIN_009)    /* MOSI: TXD pin of the SPI master port 2 */
#define GPIO_PIN_010_SPI2_MISO          (((UINT32)1U << 12U) | GPIO_PIN_010)    /* MISO: RXD pin of the SPI master port 2 */
#define GPIO_PIN_011_SPI2_SS0           (((UINT32)1U << 12U) | GPIO_PIN_011)    /* The 1st Slave Select pin of the SPI master port 2 */

#define GPIO_PIN_012_SPI3_SCLK          (((UINT32)1U << 12U) | GPIO_PIN_012)    /* Serial Clock pin of the SPI master port 3 */
#define GPIO_PIN_013_SPI3_MOSI          (((UINT32)1U << 12U) | GPIO_PIN_013)    /* MOSI: TXD pin of the SPI master port 3 */
#define GPIO_PIN_014_SPI3_MISO          (((UINT32)1U << 12U) | GPIO_PIN_014)    /* MISO: RXD pin of the SPI master port 3 */
#define GPIO_PIN_015_SPI3_SS0           (((UINT32)1U << 12U) | GPIO_PIN_015)    /* The 1st Slave Select pin of the SPI master port 3 */

#define GPIO_PIN_053_SPI4_SCLK          (((UINT32)4U << 12U) | GPIO_PIN_053)    /* Serial Clock pin of the SPI master port 4 */
#define GPIO_PIN_054_SPI4_MOSI          (((UINT32)4U << 12U) | GPIO_PIN_054)    /* MOSI: TXD pin of the SPI master port 4 */
#define GPIO_PIN_055_SPI4_MISO          (((UINT32)4U << 12U) | GPIO_PIN_055)    /* MISO: RXD pin of the SPI master port 4 */
#define GPIO_PIN_056_SPI4_SS0           (((UINT32)4U << 12U) | GPIO_PIN_056)    /* The 1st Slave Select pin of the SPI master port 4 */

#define GPIO_PIN_049_SPI5_SCLK          (((UINT32)3U << 12U) | GPIO_PIN_049)    /* Serial Clock pin of the SPI master port 5 */
#define GPIO_PIN_045_SPI5_MOSI          (((UINT32)3U << 12U) | GPIO_PIN_045)    /* MOSI: TXD pin of the SPI master port 5 */
#define GPIO_PIN_050_SPI5_MISO          (((UINT32)3U << 12U) | GPIO_PIN_050)    /* MISO: RXD pin of the SPI master port 5 */
#define GPIO_PIN_046_SPI5_SS0           (((UINT32)3U << 12U) | GPIO_PIN_046)    /* The 1st Slave Select pin of the SPI master port 5 */

#define GPIO_PIN_012_SPI_SLAVE_SCLK     (((UINT32)3U << 12U) | GPIO_PIN_012)    /* Serial Clock pin of the SPI slave port */
#define GPIO_PIN_013_SPI_SLAVE_MISO     (((UINT32)3U << 12U) | GPIO_PIN_013)    /* MISO: TXD pin of the SPI slave port */
#define GPIO_PIN_014_SPI_SLAVE_MOSI     (((UINT32)3U << 12U) | GPIO_PIN_014)    /* MOSI: RXD pin of the SPI slave port */
#define GPIO_PIN_015_SPI_SLAVE_SS       (((UINT32)3U << 12U) | GPIO_PIN_015)    /* Slave Select pin of the SPI slave port  */

#define GPIO_PIN_049_SPI_SLAVE_SCLK     (((UINT32)5U << 12U) | GPIO_PIN_049)    /* Serial Clock pin of the SPI slave port */
#define GPIO_PIN_050_SPI_SLAVE_MISO     (((UINT32)5U << 12U) | GPIO_PIN_050)    /* MISO: TXD pin of the SPI slave port */
#define GPIO_PIN_051_SPI_SLAVE_MOSI     (((UINT32)5U << 12U) | GPIO_PIN_051)    /* MOSI: RXD pin of the SPI slave port */
#define GPIO_PIN_052_SPI_SLAVE_SS       (((UINT32)5U << 12U) | GPIO_PIN_052)    /* Slave Select pin of the SPI slave port  */

/* Interval Timer */
#define GPIO_PIN_000_TMR0_EXT_CLK       (((UINT32)1U << 12U) | GPIO_PIN_000)    /* External clock source of interval timer 0 */
#define GPIO_PIN_001_TMR1_EXT_CLK       (((UINT32)2U << 12U) | GPIO_PIN_001)    /* External clock source of interval timer 1 */
#define GPIO_PIN_002_TMR2_EXT_CLK       (((UINT32)2U << 12U) | GPIO_PIN_002)    /* External clock source of interval timer 2 */
#define GPIO_PIN_090_TMR0_EXT_CLK       (((UINT32)4U << 12U) | GPIO_PIN_090)    /* External clock source of interval timer 0 */
#define GPIO_PIN_092_TMR1_EXT_CLK       (((UINT32)5U << 12U) | GPIO_PIN_092)    /* External clock source of interval timer 1 */
#define GPIO_PIN_093_TMR2_EXT_CLK       (((UINT32)5U << 12U) | GPIO_PIN_093)    /* External clock source of interval timer 2 */
#define GPIO_PIN_094_TMR1_EXT_CLK       (((UINT32)2U << 12U) | GPIO_PIN_094)    /* External clock source of interval timer 1 */

/* Universal Asynchronous Receiver Transmitter */
#define GPIO_PIN_016_UART_APB_RXD       (((UINT32)1U << 12U) | GPIO_PIN_016)    /* Receive Data pin of the UART_APB port 0 */
#define GPIO_PIN_017_UART_APB_TXD       (((UINT32)1U << 12U) | GPIO_PIN_017)    /* Transmit Data pin of the UART_APB port 0 */

#define GPIO_PIN_090_UART0_RXD          (((UINT32)1U << 12U) | GPIO_PIN_090)    /* Receive Data pin of the UART_AHB port 0 */
#define GPIO_PIN_091_UART0_TXD          (((UINT32)1U << 12U) | GPIO_PIN_091)    /* Transmit Data pin of the UART_AHB port 0 */
#define GPIO_PIN_092_UART0_CTS_N        (((UINT32)1U << 12U) | GPIO_PIN_092)    /* Clear-to-Send pin of the UART_AHB port 0 */
#define GPIO_PIN_093_UART0_RTS_N        (((UINT32)1U << 12U) | GPIO_PIN_093)    /* Request-to-Send pin of the UART_AHB port 0 */

#define GPIO_PIN_094_UART1_RXD          (((UINT32)1U << 12U) | GPIO_PIN_094)    /* Receive Data pin of the UART_AHB port 1 */
#define GPIO_PIN_095_UART1_TXD          (((UINT32)1U << 12U) | GPIO_PIN_095)    /* Transmit Data pin of the UART_AHB port 1 */
#define GPIO_PIN_096_UART1_CTS_N        (((UINT32)1U << 12U) | GPIO_PIN_096)    /* Clear-to-Send pin of the UART_AHB port 1 */
#define GPIO_PIN_097_UART1_RTS_N        (((UINT32)1U << 12U) | GPIO_PIN_097)    /* Request-to-Send pin of the UART_AHB port 1 */

#define GPIO_PIN_049_UART2_RXD          (((UINT32)4U << 12U) | GPIO_PIN_049)    /* Receive Data pin of the UART_AHB port 2 */
#define GPIO_PIN_050_UART2_TXD          (((UINT32)4U << 12U) | GPIO_PIN_050)    /* Transmit Data pin of the UART_AHB port 2 */
#define GPIO_PIN_051_UART2_CTS_N        (((UINT32)4U << 12U) | GPIO_PIN_051)    /* Clear-to-Send pin of the UART_AHB port 2 */
#define GPIO_PIN_052_UART2_RTS_N        (((UINT32)4U << 12U) | GPIO_PIN_052)    /* Request-to-Send pin of the UART_AHB port 2 */
#define GPIO_PIN_077_UART2_RXD          (((UINT32)1U << 12U) | GPIO_PIN_077)    /* Receive Data pin of the UART_AHB port 2 */
#define GPIO_PIN_078_UART2_TXD          (((UINT32)1U << 12U) | GPIO_PIN_078)    /* Transmit Data pin of the UART_AHB port 2 */
#define GPIO_PIN_079_UART2_CTS_N        (((UINT32)1U << 12U) | GPIO_PIN_079)    /* Clear-to-Send pin of the UART_AHB port 2 */
#define GPIO_PIN_080_UART2_RTS_N        (((UINT32)1U << 12U) | GPIO_PIN_080)    /* Request-to-Send pin of the UART_AHB port 2 */

#define GPIO_PIN_012_UART3_RXD          (((UINT32)4U << 12U) | GPIO_PIN_012)    /* Receive Data pin of the UART_AHB port 3 */
#define GPIO_PIN_013_UART3_TXD          (((UINT32)4U << 12U) | GPIO_PIN_013)    /* Transmit Data pin of the UART_AHB port 3 */
#define GPIO_PIN_014_UART3_CTS_N        (((UINT32)4U << 12U) | GPIO_PIN_014)    /* Clear-to-Send pin of the UART_AHB port 3 */
#define GPIO_PIN_015_UART3_RTS_N        (((UINT32)4U << 12U) | GPIO_PIN_015)    /* Request-to-Send pin of the UART_AHB port 3 */
#define GPIO_PIN_083_UART3_RXD          (((UINT32)1U << 12U) | GPIO_PIN_083)    /* Receive Data pin of the UART_AHB port 3 */
#define GPIO_PIN_084_UART3_TXD          (((UINT32)1U << 12U) | GPIO_PIN_084)    /* Transmit Data pin of the UART_AHB port 3 */
#define GPIO_PIN_081_UART3_CTS_N        (((UINT32)1U << 12U) | GPIO_PIN_081)    /* Clear-to-Send pin of the UART_AHB port 3 */
#define GPIO_PIN_082_UART3_RTS_N        (((UINT32)1U << 12U) | GPIO_PIN_082)    /* Request-to-Send pin of the UART_AHB port 3 */

/* Universal Serial Bus */
#define GPIO_PIN_001_USB0_EHCI_OC       (((UINT32)5U << 12U) | GPIO_PIN_001)    /* Over-Current pin of the USB EHCI port 0 */
#define GPIO_PIN_002_USB0_EHCI_PWR      (((UINT32)5U << 12U) | GPIO_PIN_002)    /* USB EHCI port 0 power enable */
#define GPIO_PIN_016_USB0_EHCI_OC       (((UINT32)2U << 12U) | GPIO_PIN_016)    /* Over-Current pin of the USB EHCI port 0 */
#define GPIO_PIN_017_USB0_EHCI_PWR      (((UINT32)2U << 12U) | GPIO_PIN_017)    /* USB EHCI port 0 power enable */
#define GPIO_PIN_024_USB0_EHCI_OC       (((UINT32)3U << 12U) | GPIO_PIN_024)    /* Over-Current pin of the USB EHCI port 0 */
#define GPIO_PIN_025_USB0_EHCI_PWR      (((UINT32)3U << 12U) | GPIO_PIN_025)    /* USB EHCI port 0 power enable */
#define GPIO_PIN_057_USB0_EHCI_OC       (((UINT32)4U << 12U) | GPIO_PIN_057)    /* Over-Current pin of the USB EHCI port 0 */
#define GPIO_PIN_058_USB0_EHCI_PWR      (((UINT32)4U << 12U) | GPIO_PIN_058)    /* USB EHCI port 0 power enable */
#define GPIO_PIN_090_USB0_EHCI_OC       (((UINT32)3U << 12U) | GPIO_PIN_090)    /* Over-Current pin of the USB EHCI port 0 */
#define GPIO_PIN_092_USB0_EHCI_PWR      (((UINT32)3U << 12U) | GPIO_PIN_092)    /* USB EHCI port 0 power enable */

/* Video Input */
#define GPIO_PIN_088_VIN0_MASTER_HSYNC  (((UINT32)1U << 12U) | GPIO_PIN_088)    /* VIN0 Master HSYNC */
#define GPIO_PIN_089_VIN0_MASTER_VSYNC  (((UINT32)1U << 12U) | GPIO_PIN_089)    /* VIN0 Master VSYNC */

#define GPIO_PIN_088_VIN1_MASTER_HSYNC  (((UINT32)2U << 12U) | GPIO_PIN_088)    /* VIN1/PIP Master HSYNC */
#define GPIO_PIN_089_VIN1_MASTER_VSYNC  (((UINT32)2U << 12U) | GPIO_PIN_089)    /* VIN1/PIP Master VSYNC */
#define GPIO_PIN_097_VIN1_MASTER_HSYNC  (((UINT32)2U << 12U) | GPIO_PIN_097)    /* VIN1/PIP Master HSYNC */
#define GPIO_PIN_096_VIN1_MASTER_VSYNC  (((UINT32)2U << 12U) | GPIO_PIN_096)    /* VIN1/PIP Master VSYNC */

#define GPIO_PIN_001_VIN_STRIG_0        (((UINT32)4U << 12U) | GPIO_PIN_001)    /* VIN strig 0 */
#define GPIO_PIN_002_VIN_STRIG_1        (((UINT32)4U << 12U) | GPIO_PIN_002)    /* VIN strig 1 */
#define GPIO_PIN_018_VIN_STRIG_0        (((UINT32)2U << 12U) | GPIO_PIN_018)    /* VIN strig 0 */
#define GPIO_PIN_019_VIN_STRIG_1        (((UINT32)2U << 12U) | GPIO_PIN_019)    /* VIN strig 1 */

/* One-Time Programmable */
#define GPIO_PIN_000_OTP_VPP            (((UINT32)4U << 12U) | GPIO_PIN_000)    /* VPP for OTP programming */
#define GPIO_PIN_007_OTP_VPP            (((UINT32)2U << 12U) | GPIO_PIN_007)    /* VPP for OTP programming */
#define GPIO_PIN_046_OTP_VPP            (((UINT32)5U << 12U) | GPIO_PIN_046)    /* VPP for OTP programming */

/* Delayed Vsync/Hsync Generator */
#define GPIO_PIN_008_SYNCDLY_VS_OUT0    (((UINT32)4U << 12U) | GPIO_PIN_008)    /* Delayed vsync output 0 (on-chip master sync) */
#define GPIO_PIN_009_SYNCDLY_VS_OUT1    (((UINT32)4U << 12U) | GPIO_PIN_009)    /* Delayed vsync output 1 (external slave sync) */
#define GPIO_PIN_010_SYNCDLY_HS_OUT0    (((UINT32)4U << 12U) | GPIO_PIN_010)    /* Delayed hsync output 0 */
#define GPIO_PIN_011_SYNCDLY_HS_OUT1    (((UINT32)4U << 12U) | GPIO_PIN_011)    /* Delayed hsync output 1 */
#define GPIO_PIN_047_SYNCDLY_HS_OUT0    (((UINT32)3U << 12U) | GPIO_PIN_047)    /* Delayed hsync output 0 */
#define GPIO_PIN_048_SYNCDLY_HS_OUT1    (((UINT32)3U << 12U) | GPIO_PIN_048)    /* Delayed hsync output 1 */
#define GPIO_PIN_051_SYNCDLY_VS_OUT0    (((UINT32)3U << 12U) | GPIO_PIN_051)    /* Delayed vsync output 0 (on-chip master sync) */
#define GPIO_PIN_052_SYNCDLY_VS_OUT1    (((UINT32)3U << 12U) | GPIO_PIN_052)    /* Delayed vsync output 1 (external slave sync) */
#define GPIO_PIN_053_SYNCDLY_VS_IN      (((UINT32)3U << 12U) | GPIO_PIN_053)    /* External vsync input */
#define GPIO_PIN_056_SYNCDLY_VS_IN      (((UINT32)3U << 12U) | GPIO_PIN_056)    /* External vsync input */

/* WatchDog Timer */
#define GPIO_PIN_000_WDT_RST            (((UINT32)2U << 12U) | GPIO_PIN_000)    /* WDT extend reset */
#define GPIO_PIN_007_WDT_RST            (((UINT32)3U << 12U) | GPIO_PIN_007)    /* WDT extend reset */
#define GPIO_PIN_020_WDT_RST            (((UINT32)2U << 12U) | GPIO_PIN_020)    /* WDT extend reset */
#define GPIO_PIN_043_WDT_RST            (((UINT32)3U << 12U) | GPIO_PIN_043)    /* WDT extend reset */
#define GPIO_PIN_087_WDT_RST            (((UINT32)2U << 12U) | GPIO_PIN_087)    /* WDT extend reset */
#define GPIO_PIN_091_WDT_RST            (((UINT32)5U << 12U) | GPIO_PIN_091)    /* WDT extend reset */

#define AMBA_GPIO_GROUP0                0x0U    /* GPIO pin 0~31    */
#define AMBA_GPIO_GROUP1                0x1U    /* GPIO pin 32~63   */
#define AMBA_GPIO_GROUP2                0x2U    /* GPIO pin 64~95   */
#define AMBA_GPIO_GROUP3                0x3U    /* GPIO pin 96~127  */
#define AMBA_NUM_GPIO_GROUP             0x4U

#endif /* AMBA_GPIO_PRIV_H */
