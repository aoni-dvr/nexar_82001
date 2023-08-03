/**
 *  @file AmbaB8_PLL.h
 *
 *  @copyright Copyright (c) 2015 Ambarella, Inc.
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
 *  @details Definitions & Constants for B6 PLL APIs
 *
 */

#ifndef AMBA_B8_PLL_H
#define AMBA_B8_PLL_H

#define B8_PLL_CORE         0U
#define B8_PLL_MPHY_TX0     1U
#define B8_PLL_MPHY_TX1     2U
#define B8_PLL_MPHY_RX      3U
#define B8_PLL_SENSOR       4U
#define B8_PLL_VIDEO        5U
#define AMBA_NUM_B8_PLL     6U                  /* Total number of PLLs */

#define B8_PLL_VO_SENSOR_SRC_REF_CLK           0U
#define B8_PLL_VO_SENSOR_SRC_RXHS_SYMBOL_CLK   1U
#define B8_PLL_VO_SENSOR_SRC_SPCLK0            3U
#define B8_PLL_VO_SENSOR_SRC_RXBYTE_CLK        7U

/* Clock frequency generated from pll_core */
typedef struct {
    UINT32 CoreFreq;                            /* gclk_core = gclk_apb */
    UINT32 IdspFreq;                            /* gclk_idsp */
    UINT32 SwphyFreq;                           /* gclk_swphy */
    UINT32 SpiFreq;                             /* gclk_ssi */

} B8_CORE_PLL_CLK_CTRL_s;

/* Clock frequency generated from pll_vo */
typedef struct {
    UINT32 Video0Freq;                          /* gclk_vo_dsi0 */

} B8_VIDEO_PLL_CLK_CTRL_s;

/* Clock frequency generated from pll_si */
typedef struct {
    UINT32 SensorFreq;                          /* gclk_so_int */
    UINT32 Video1Freq;                          /* gclk_vo_dsi1 */

} B8_SENSOR_PLL_CLK_CTRL_s;

typedef struct {
    UINT32 ClkRefFreq;                          /* Clock Reference Frequency(Hz) */
    UINT32 PllRefFreq[AMBA_NUM_B8_PLL];
    UINT32 PllOutClkFreq[AMBA_NUM_B8_PLL];      /* Frequency(Hz) */
    FLOAT  fPllOutClkFreq[AMBA_NUM_B8_PLL];     /* Frequency(Hz) */
    B8_CORE_PLL_CLK_CTRL_s   CorePllClkCtrl;    /* Clocks derived by Core PLL */
    B8_VIDEO_PLL_CLK_CTRL_s  VideoPllClkCtrl;   /* Clocks derived by Video PLL */
    B8_SENSOR_PLL_CLK_CTRL_s SensorPllClkCtrl;  /* Clocks derived by Sensor PLL */

} B8_CLK_DOMAIN_CTRL_s;

/*-----------------------------------------------------------------------------------------------*\
 * Defined in AmbaB8_PLL.c
\*-----------------------------------------------------------------------------------------------*/
UINT32 AmbaB8_PllInit(UINT32 ChipID, UINT32 ClkRefFreq);
UINT32 AmbaB8_PllSetCoreClk(UINT32 ChipID, UINT32 Frequency);
UINT32 AmbaB8_PllSetSwphyClk(UINT32 ChipID, UINT32 Frequency);
UINT32 AmbaB8_PllSetMphyTx0Clk(UINT32 ChipID, UINT32 SerDesRate);
UINT32 AmbaB8_PllSetMphyRxClk(UINT32 ChipID, UINT32 SerDesRate);
UINT32 AmbaB8_PllSetSensorClk(UINT32 ChipID, UINT32 Frequency);
UINT32 AmbaB8_PllSetVout0Clk(UINT32 ChipID, UINT32 Frequency);
UINT32 AmbaB8_PllSetVout1Clk(UINT32 ChipID, UINT32 Frequency);
UINT32 AmbaB8_PllRescaleSensorClk(UINT32 ChipID, INT32 FrequencyDiff);

UINT32 AmbaB8_PllGetCoreClk(UINT32 ChipID);
UINT32 AmbaB8_PllGetIdspClk(UINT32 ChipID);
UINT32 AmbaB8_PllGetSwphyClk(UINT32 ChipID);
UINT32 AmbaB8_PllGetSpiClk(UINT32 ChipID);
UINT32 AmbaB8_PllGetSensorClk(UINT32 ChipID);
UINT32 AmbaB8_PllGetVout0Clk(UINT32 ChipID);
UINT32 AmbaB8_PllGetVout1Clk(UINT32 ChipID);
UINT32 AmbaB8_PllGetPwmClkRef(UINT32 ChipID);

UINT32 AmbaB8_PllSwPllInit(void);
UINT32 AmbaB8_PllSwPllConfig(UINT32 ChipID, UINT32 DiffThresholdInUs);
UINT32 AmbaB8_PllSwPllEnable(void);
UINT32 AmbaB8_PllSwPllHandler(UINT32 Timeout);

void AmbaB8_PllSwPllVin0CallBackFunc(void);
void AmbaB8_PllSwPllVin1CallBackFunc(void);
void AmbaB8_PllSwPllVin2CallBackFunc(void);
void AmbaB8_PllSwPllVin3CallBackFunc(void);
void AmbaB8_PllSwPllVin4CallBackFunc(void);
void AmbaB8_PllSwPllVin5CallBackFunc(void);

UINT32 AmbaB8_PllSetVoutClkHint(UINT32 ChipID, UINT32 Channel, UINT8 Ratio);
UINT32 AmbaB8_PllSetVoutClkSrc(UINT32 ChipID, UINT32 Channel, UINT8 ClkSrc);
UINT32 AmbaB8_PllSetVout0RefFreq(UINT32 ChipID, UINT32 ClkRefFreq);
UINT32 AmbaB8_PllSetVout1RefFreq(UINT32 ChipID, UINT32 ClkRefFreq);

#endif /* AMBA_B8_PLL_H */
