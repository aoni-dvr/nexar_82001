/**
 *  @file AmbaRTSL_PLL_Def.h
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
 *  @details Common Definitions & Constants for PLL APIs
 *
 */

#ifndef AMBA_RTSL_PLL_DEF_H
#define AMBA_RTSL_PLL_DEF_H

#ifndef AMBA_TYPES_H
#include "AmbaTypes.h"
#endif
#ifndef AMBA_ERROR_CODE_H
#include "AmbaErrorCode.h"
#endif

#define PLL_ERR_0000        (PLL_ERR_BASE)              /* Invalid argument */
#define PLL_ERR_0001        (PLL_ERR_BASE + 0X1U)       /* Reserved for development stage */
#define PLL_ERR_0002        (PLL_ERR_BASE + 0X2U)       /* Not Yet Implemented */

#define PLL_ERR_NONE        OK
#define PLL_ERR_ARG         PLL_ERR_0000
#define PLL_ERR_DEVEL       PLL_ERR_0001
#define PLL_ERR_IMPL        PLL_ERR_0002

#define AMBA_PLL_AUDIO                          0U
#define AMBA_PLL_CORE                           1U
#define AMBA_PLL_CORTEX                         2U
#define AMBA_PLL_DDR                            3U
#define AMBA_PLL_ENET                           4U
#define AMBA_PLL_IDSP                           5U
#define AMBA_PLL_SD                             6U
#define AMBA_PLL_SENSOR0                        7U
#define AMBA_PLL_VIDEO2                         8U      /* for VOUT_LCD */
#define AMBA_PLL_VISION                         9U
#define AMBA_NUM_PLL                            10U     /* Total number of PLLs */

#define AMBA_PLL_VOUT_LCD_MIPI_DSI              0U      /* MIPI DSI */
#define AMBA_PLL_VOUT_LCD_FPD_LINK              1U      /* FPD-Link */
#define AMBA_PLL_VOUT_LCD_CVBS                  2U      /* CVBS */
#define AMBA_PLL_VOUT_LCD_MIPI_CSI              3U      /* MIPI CSI */
#define AMBA_NUM_PLL_VOUT_LCD_TYPE              4U

#define AMBA_PLL_ADC_CLK_REF                    0U
#define AMBA_PLL_ADC_CLK_IDSP                   1U
#define AMBA_PLL_ADC_CLK_NAND                   2U
#define AMBA_NUM_PLL_ADC_CLK_CONFIG             3U

#define AMBA_PLL_CAN_CLK_REF                    0U
#define AMBA_PLL_CAN_CLK_ETHERNET               1U
#define AMBA_PLL_CAN_CLK_AHB                    2U
#define AMBA_NUM_PLL_CAN_CLK_CONFIG             3U      /* Total Number of CAN Reference Clock Selections */

#define AMBA_PLL_ETHERNET_REF_CLK_OUTPUT        0U
#define AMBA_PLL_ETHERNET_REF_CLK_INPUT         1U

#define AMBA_PLL_VIN1_CLK_EXT                   0U
#define AMBA_PLL_VIN1_CLK_REF                   1U
#define AMBA_NUM_PLL_VIN1_CLK_CONFIG            2U

#define AMBA_PLL_ETHERNET_CLK_EXT               0U
#define AMBA_PLL_ETHERNET_CLK_REF               1U
#define AMBA_NUM_PLL_ETHERNET_CLK_CONFIG        2U

#define AMBA_PLL_SD_CLK_SD                      0U
#define AMBA_PLL_SD_CLK_CORE                    1U
#define AMBA_NUM_PLL_SD_CLK_CONFIG              2U

#define AMBA_PLL_SPI_CLK_ETHERNET               0U
#define AMBA_PLL_SPI_CLK_CORE                   1U
#define AMBA_PLL_SPI_CLK_SPI                    2U
#define AMBA_PLL_SPI_CLK_REF                    3U
#define AMBA_NUM_PLL_SPI_CLK_CONFIG             4U

#define AMBA_PLL_MIPI_DSI_CLK_VIDEO2            0U
#define AMBA_PLL_MIPI_DSI_CLK_HDMI              1U
#define AMBA_NUM_PLL_MIPI_DSI_CLK_CONFIG        2U

#define AMBA_PLL_PWM_CLK_APB                    0U
#define AMBA_PLL_PWM_CLK_CORTEX                 1U
#define AMBA_PLL_PWM_CLK_ENET                   2U
#define AMBA_PLL_PWM_CLK_REF                    3U
#define AMBA_NUM_PLL_PWM_CLK_CONFIG             4U

#define AMBA_PLL_UART_CLK_REF                   0U      /* clk_ref */
#define AMBA_PLL_UART_CLK_CORE                  1U      /* gclk_core */
#define AMBA_PLL_UART_CLK_ENET                  2U      /* pll_out_enet */
#define AMBA_PLL_UART_CLK_SD                    3U      /* pll_out_sd */
#define AMBA_NUM_PLL_UART_CLK_CONFIG            4U

#define AMBA_PLL_MUX_SPCLK_NA                   0x0U
#define AMBA_PLL_MUX_SPCLK_C0                   0x1U
#define AMBA_PLL_MUX_SPCLK_C1                   0x2U
#define AMBA_PLL_MUX_SPCLK_C2                   0x3U
#define AMBA_PLL_NUM_MUX_SPCLK                  0x4U    /* Total number of spclk_c */

#define AMBA_PLL_AUDIO_CLK_EXT                  0x0U    /* External pin: iopad_apb_clk_au_c */
#define AMBA_PLL_AUDIO_CLK_REF                  0x1U    /* clk_ref */
#define AMBA_PLL_AUDIO_CLK_SENSOR               0x2U    /* xx_clk_si */
#define AMBA_PLL_AUDIO_CLK_MUXED_LVDS_SPCLK     0x3U    /* MUXED_LVDS_SPCLK */
#define AMBA_NUM_PLL_AUDIO_CLK_CONFIG           0x4U    /* Total Number of Audio Reference Clock Selections */

#define AMBA_PLL_GTX_CLK_EXT                    0x0U
#define AMBA_PLL_GTX_CLK_INT                    0x1U
#define AMBA_PLL_NUM_GTX_CLK                    0x2U

#define AMBA_PLL_SENSOR_REF_CLK_OUTPUT          0x0U
#define AMBA_PLL_SENSOR_REF_CLK_INPUT           0x1U

#define AMBA_PLL_VOUT_LCD_CLK_REF               0x0U    /* clk_ref */
#define AMBA_PLL_VOUT_LCD_CLK_SENSOR            0x1U    /* xx_clk_si */
#define AMBA_PLL_VOUT_LCD_CLK_MUXED_LVDS_SPCLK  0x2U    /* MUXED_LVDS_SPCLK */
#define AMBA_NUM_PLL_VOUT_LCD_CLK_CONFIG        0x3U

#define AMBA_PLL_SENSOR_CLK_0                   0x0U
#define AMBA_PLL_NUM_SENSOR_CLK                 0x1U

#define AMBA_PLL_VOUT_CLK_LCD                   0x0U
#define AMBA_PLL_NUM_VOUT_CLK                   0x1U

typedef struct {
    UINT32  ExtAudioClkFreq;                    /* iopad_apb_clk_au_c */
    UINT32  ExtEthernetClkFreq;                 /* xx_enet_clk_rx */
    UINT32  ExtSensorInputClkFreq;              /* xx_clk_si */
    UINT32  ExtMuxLvdsSpclkClkFreq;             /* muxed_lvds_spclk */

    UINT32  SpclkMuxSelect;
} AMBA_PLL_EXT_CLK_CONFIG_s;

#endif /* AMBA_RTSL_PLL_DEF_H */
