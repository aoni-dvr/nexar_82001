/**
 *  @file AmbaReg_RCT.h
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
 *  @details Definitions & Constants for RCT (Reset, Clock and Test) registers
 *
 */

#ifndef AMBA_REG_RCT_H
#define AMBA_REG_RCT_H

#ifndef AMBA_TYPES_H
#include "AmbaTypes.h"
#endif

/*
 * RCT: System Configuration Register
 */
typedef struct {
    UINT32  EthFuncEnable:              1;      /* [0] Ethernet functions: 0 = disable, 1 = enable */
    UINT32  ClkConfig:                  3;      /* [3:1] Power-On Reset IDSP/Core/DDR clock configuration */
    UINT32  BootMode:                   2;      /* [5:4] Boot mode select [1:0] */
    UINT32  SecureBoot:                 1;      /* [6] 1 = Secure boot */
    UINT32  Reserved0:                  1;      /* [7] */
    UINT32  BootBypass:                 1;      /* [8] 1 = Bypass fetching boot code from external device */
    UINT32  PeripheralClkMode:          1;      /* [9] Peripherals Clock Mode, 0 = gclk_core/2, 1 = gclk_core */
    UINT32  UsbBoot:                    1;      /* [10] 1 = Force USB boot */
    UINT32  ClkSource:                  2;      /* [12:11] Clock source for core & ddr clock: 0 = Normal, 1 = Ref clock, 2 = Reserved, 3 = Reserved */
    UINT32  RefClk72mSelect:            1;      /* [13] 0 = reference clock is 24MHz, 1 = reference clock is 72MHz */
    UINT32  BootOption0:                6;      /* [19:14] Boot mode option [5:0] */
    UINT32  EthPhyTypeSelect:           1;      /* [20] 0 = RGMII mode, 1 = RMII mode */
    UINT32  CortexBootMode:             1;      /* [21] Cortex boot mode, 0 = 64-bit mode, 1 = 32-bit mode */
    UINT32  BootOption1:                1;      /* [22] Boot mode option [6] */
    UINT32  Reserved1:                  7;      /* [29:23] Set to zero always (No Pins) */
    UINT32  RefClk48mSelect:            1;      /* [30] 0 = reference clock is 24MHz, 1 = reference clock is 48MHz */
    UINT32  SysConfigSource:            1;      /* [31] 0 = config data is set by pins, 1 = config data is read from OTP rom (except bit[10,31])) */
} AMBA_SYS_CONFIG_REG_s;

/*
 * RCT: Power Control Register
 */
typedef struct {
    UINT32  Reserved0:                  1;      /* [0] Reserved */
    UINT32  UsbPhySuspendDevice:        1;      /* [1] 0 = suspend USB PHY, 1 = suspend controlled by USB device controller */
    UINT32  UsbPhyNeverSuspend:         1;      /* [2] 1 = Never suspend USB PHY, 0 = Allow suspend USB PHY */
    UINT32  Reserved1:                  2;      /* [4:3] Reserved */
    UINT32  PwcPowerDown:               1;      /* [5] PWC power down sequence starts by the transition from 0 to 1  */
    UINT32  Reserved2:                  6;      /* [11:6] Reserved */
    UINT32  UsbPhySuspendHost:          1;      /* [12] 0 = suspend USB PHY, 1 = suspend controlled by USB host controller */
    UINT32  Reserved3:                  19;     /* [31:13] Reserved */
} AMBA_POWER_CTRL_REG_s;

/*
 * RCT: ADC Power Control Register
 */
typedef struct {
    UINT32  Reserved0:                  1;      /* [0] Reserved */
    UINT32  PowerDown:                  1;      /* [1] 1 = ADC power down */
    UINT32  Reserved1:                  6;      /* [7:2] Reserved */
    UINT32  PowerDownScaler:            4;      /* [11:8] 1 = ADC scaler power down */
    UINT32  Reserved2:                  12;     /* [23:12] Reserved */
    UINT32  Ictrl:                      3;      /* [26:24] ictrl */
    UINT32  Cctrl:                      3;      /* [29:27] cctrl */
    UINT32  Reserved3:                  2;      /* [31:30] Reserved */
} AMBA_ADC_POWER_CTRL_REG_s;

/*
 * RCT: PLL Control Register for PLL Integer configurations
 */
typedef struct {
    UINT32  WriteEnable:                1;      /* [0] Write Enable: 1 = enable */
    UINT32  Reserved0:                  1;      /* [1] Reserved */
    UINT32  Bypass:                     1;      /* [2] Bypass: 0 = disable, 1 = enable */
    UINT32  FractionEnable:             1;      /* [3] Enable fraction register: 0 = disable(integer only), 1 = enable(interger+fraction) */
    UINT32  ForcePllReset:              1;      /* [4] Force PLL reset: 0 = disable, 1 = enable (except for core & arm pll) */
    UINT32  PowerDown:                  1;      /* [5] Power down the PLL: 0 = disable, 1 = enable */
    UINT32  HaltVCO:                    1;      /* [6] Halt VCO oscillation */
    UINT32  TristateLoopFilter:         1;      /* [7] Tristate loop filter voltage: 0 = disable, 1 = enable */
    UINT32  PllToutAsync:               4;      /* [11:8] Select for pll_tout_async */
    UINT32  Sdiv:                       4;      /* [15:12] SDIV = Slow Divider */
    UINT32  Sout:                       4;      /* [19:16] SOUT = Scaler for PLL Output */
    UINT32  Reserved1:                  2;      /* [21:20] Reserved */
    UINT32  DsmReset:                   1;      /* [22] DSM Reset. (Only for video and cortex pll) */
    UINT32  Reserved2:                  1;      /* [23] Reserved */
    UINT32  IntProg:                    7;      /* [30:24] INTPROG[6:0] */
    UINT32  Reserved3:                  1;      /* [31] Reserved */
} AMBA_PLL_CTRL_REG_s;

/*
 * RCT: PLL Control2 Register
 */
typedef struct {
    UINT32  HsDivider:                  12;     /* [11:0] High speed divider control */
    UINT32  BypassHsDivider:            1;      /* [12] Bypass high speed divider */
    UINT32  BypassMDIV:                 1;      /* [13] Bypass MDIV */
    UINT32  DiffVcoEnable:              1;      /* [14] Differential VCO enable */
    UINT32  DutyCycleTune:              1;      /* [15] Duty cycle tune */
    UINT32  ChargePumpCurrent:          8;      /* [23:16] Charge pump current */
    UINT32  Reserved:                   4;      /* [27:24] Reserved */
    UINT32  ChargePumpDcBias:           2;      /* [29:28] Charge pump DC bias point */
    UINT32  DsmModeCtrl:                2;      /* [31:30] DSM mode control */
} AMBA_PLL_CTRL2_REG_s;

/*
 * RCT: PLL Control3 Register
 */
typedef struct {
    UINT32  Reserved0:                  1;      /* [0] */
    UINT32  VcoRange:                   2;      /* [2:1] VCO range */
    UINT32  VcoClamp:                   2;      /* [4:3] VCO clamp */
    UINT32  Reserved1:                  2;      /* [6:5] */
    UINT32  DsmDitherGain:              2;      /* [8:7] DSM dither gain */
    UINT32  Reserved2:                  4;      /* [12:9] */
    UINT32  LpfRz:                      4;      /* [16:13] Feed forward zero resistor select */
    UINT32  IrefCtrl:                   3;      /* [19:17] Bias current control */
    UINT32  BypassJdiv:                 1;      /* [20] Bypass JDIV */
    UINT32  BypassJdout:                1;      /* [21] Bypass JDOUT */
    UINT32  Reserved3:                  10;     /* [31:22] */
} AMBA_PLL_CTRL3_REG_s;

/*
 * RCT: PLL 24-bit Divider with Soft Reset Register
 */
typedef struct {
    UINT32  Divider:                    24;     /* [23:0] Divider */
    UINT32  SoftReset:                  1;      /* [24] 1 - Reset, 0 - deassert (Only for SPI, PWM) */
    UINT32  Reserved:                   7;      /* [31:25] Reserved */
} AMBA_PLL_DIVIDER_24BIT_REG_s;

/*
 * RCT: PLL 16-bit Divider with Soft Reset Register
 */
typedef struct {
    UINT32  Divider:                    16;     /* [15:0] Integer Div Value */
    UINT32  SoftReset:                  1;      /* [16] 1 - Reset, 0 - deassert */
    UINT32  Reserved:                   15;     /* [31:17] Reserved */
} AMBA_PLL_DIVIDER_16BIT_REG_s;

/*
 * RCT: PLL 4-bit Divider with Write Enable Register
 */
typedef struct {
    UINT32  WriteEnable:                1;      /* [0] Write enable */
    UINT32  Reserved0:                  3;      /* [3:1] Reserved */
    UINT32  Divider:                    4;      /* [7:4] Integer Div Value */
    UINT32  Reserved1:                  24;     /* [31:8] Reserved */
} AMBA_PLL_DIVIDER_4BIT_REG_s;

/*
 * RCT: PLL Divider for DDR Initialization Register
 */
typedef struct {
    UINT32  Divider:                    8;      /* [7:0] Integer Div Value */
    UINT32  SoftReset:                  1;      /* [8] 1 = Reset divider */
    UINT32  lpddr2InitEn:               1;      /* [9] 1 = gclk_ddrio is drrived from clk_ddr_init */
    UINT32  Reserved:                   22;     /* [31:10] Reserved */
} AMBA_PLL_DIVIDER_DDR_INIT_REG_s;

/*
 * RCT: PLL Clock Divider Control Register
 */
typedef struct {
    UINT32  Audio0PostScaler:           1;      /* [0] Audio0 post scaler */
    UINT32  SensorPostScaler:           1;      /* [1] Sensor post scaler */
    UINT32  Reserved0:                  4;      /* [5:2] Reserved */
    UINT32  Vin0Scaler:                 1;      /* [6] VIN0 scaler */
    UINT32  Reserved1:                  1;      /* [7] Reserved */
    UINT32  IdspPostScaler:             1;      /* [8] IDSP post scaler */
    UINT32  Reserved2:                  4;      /* [12:9] Reserved */
    UINT32  AudioPreScaler:             1;      /* [13] Audio pre scaler */
    UINT32  SensorPreScaler:            1;      /* [14] Sensor pre scaler */
    UINT32  VoutLcdPreScaler:           1;      /* [15] Video 2 (Vout LCD) pre scaler */
    UINT32  Reserved3:                  1;      /* [16] Reserved */
    UINT32  HdmiPreScaler:              1;      /* [17] HDMI pre scaler */
    UINT32  DdrAltScaler:               1;      /* [18] DDR Alternate Source scaler */
    UINT32  Reserved4:                  4;      /* [22:19] Reserved */
    UINT32  Vin1Scaler:                 1;      /* [23] VIN1 scaler */
    UINT32  EnetPostScaler:             1;      /* [24] Ethernet post scaler */
    UINT32  Reserved5:                  1;      /* [25] Reserved */
    UINT32  Audio3PostScaler:           1;      /* [26] Audio3 post scaler */
    UINT32  Reserved6:                  1;      /* [27] Reserved */
    UINT32  VoutTvPostScaler:           1;      /* [28] VOUT TV post scaler */
    UINT32  Reserved7:                  3;      /* [31:29] Reserved */
} AMBA_PLL_SCALER_CTRL_REG_s;

/*
 * RCT: PLL Clock Disable Control Register
 */
typedef struct {
    UINT32  Audio0PostScaler:           1;      /* [0] 1 = shutoff clock */
    UINT32  SensorPostScalar:           1;      /* [1] 1 = shutoff clock */
    UINT32  Reserved0:                  4;      /* [5:2] Reserved */
    UINT32  Vin0PostScalar:             1;      /* [6] 1 = shutoff clock */
    UINT32  Reserved1:                  1;      /* [7] Reserved */
    UINT32  IdspPostScaler:             1;      /* [8] 1 = shutoff clock */
    UINT32  CortexPostScaler:           1;      /* [9] 1 = shutoff clock */
    UINT32  Reserved2:                  13;     /* [22:10] Reserved */
    UINT32  Vin1PostScalar:             1;      /* [23] 1 = shutoff clock */
    UINT32  EnetPostScaler:             1;      /* [24] 1 = shutoff clock */
    UINT32  Reserved3:                  1;      /* [25] Reserved */
    UINT32  Audio3PostScaler:           1;      /* [26] 1 = shutoff clock */
    UINT32  Reserved4:                  1;      /* [27] Reserved */
    UINT32  VoutTvPostScaler:           1;      /* [28] 1 = shutoff clock */
    UINT32  Reserved5:                  3;      /* [31:29] Reserved */
} AMBA_CLK_DISABLE_REG_s;

/*
 * RCT: DDR Clock DividersReset Register
 */
typedef struct {
    UINT32  DddrioClkDivReset:          1;      /* [0] Soft reset for DDRIO clock dividers - Edge Triggered, 0 to 1 change causes the ddr clock dividers and DDRC to be reset */
    UINT32  Reserved:                   31;     /* [31:1] Reserved */
} AMBA_DDR_CLK_DIV_RESET_REG_s;

/*
 * RCT: PLL Lock Status Register
 */
typedef struct {
    UINT32  VoutLcdLock:                1;      /* [0] PLL video2 (LCD) lock: 0 - unlock, 1 - lock */
    UINT32  Reserved0:                  1;      /* [1] Reserved */
    UINT32  VisionLock:                 1;      /* [2] PLL vision lock: 0 - unlock, 1 - lock */
    UINT32  CortexLock:                 1;      /* [3] PLL Cortex lock: 0 - unlock, 1 - lock */
    UINT32  SensorLock:                 1;      /* [4] PLL sensor lock: 0 - unlock, 1 - lock */
    UINT32  IdspLock:                   1;      /* [5] PLL IDSP lock: 0 - unlock, 1 - lock */
    UINT32  DdrLock:                    1;      /* [6] PLL DDR lock: 0 - unlock, 1 - lock */
    UINT32  CoreLock:                   1;      /* [7] PLL core lock: 0 - unlock, 1 - lock */
    UINT32  AudioLock:                  1;      /* [8] PLL audio lock: 0 - unlock, 1 - lock */
    UINT32  HdmiLock:                   1;      /* [9] PLL HDMI lock: 0 - unlock, 1 - lock */
    UINT32  Reserved1:                  3;      /* [12:10] Reserved */
    UINT32  SdLock:                     1;      /* [13] PLL SD lock: 0 - unlock, 1 - lock */
    UINT32  EnetLock:                   1;      /* [14] PLL ENET lock: 0 - unlock, 1 - lock */
    UINT32  Reserved2:                  1;      /* [15] Reserved */
    UINT32  NandLock:                   1;      /* [16] PLL NAND lock: 0 - unlock, 1 - lock */
    UINT32  Reserved3:                  15;     /* [31:17] Reserved */
} AMBA_PLL_LOCK_STATUS_REG_s;

/*
 * RCT: Clock Source Selection Register, software shouldn't touch
 */
typedef struct {
    UINT32  ClkSrc:                     2;      /* [1:0] 0 = Default, 1 = Use ref_clk, 2/3 = Reserved */
    UINT32  Reserved:                   30;     /* [31:2] reserved */
} AMBA_PLL_CLK_SRC_SELECT_REG_s;

/*
 * RCT: PLL Reference Clock Selection Register
 */
typedef struct {
    UINT32  ClkRefSel:                  1;      /* [0] PLL reference selection: 0 = use 24 MHz crystal, 1 = use external source */
    UINT32  Reserved:                   31;     /* [31:1] Reserved */
} AMBA_PLL_REF_CLK_SELECT_REG_s;

/*
 * RCT: External Clock Source Selection Register
 */
typedef struct {
    UINT32  ExtClkSrc:                  2;      /* [1:0] 2'b01 = xx_clk_si, 2'b10 = muxed_lvds_spclk / 4, others = no clock */
    UINT32  Reserved:                   30;     /* [31:2] Reserved */
} AMBA_PLL_EXTERNAL_CLK_SRC_REG_s;

/*
 * RCT: Audio External Clock Source Control Register
 */
typedef struct {
    UINT32  AudioClkSrc:                1;      /* [0] gclk_au is: 0 = use on-chip PLL generated clock, 1 = use external clock source iopad_apb_clk_au_c */
    UINT32  Reserved:                   31;     /* [31:1] Reserved */
} AMBA_AUDIO_EXT_CLK_CTRL_REG_s;

/*
 * RCT: SD Clock Source Selection Register
 */
typedef struct {
    UINT32  Sd0ClkSrc:                  1;      /* [0] gclk_sd48 clock divider source, 0 = pll_out_sd, 1 = pll_out_core */
    UINT32  Reserved0:                  1;      /* [1] reserved */
    UINT32  Sdio0ClkSrc:                1;      /* [2] gclk_sdio0 clock divider source, 0 = pll_out_sd, 1 = pll_out_core */
    UINT32  Sdio1ClkSrc:                1;      /* [3] gclk_sdio1 clock divider source, 0 = pll_out_sd, 1 = pll_out_core */
    UINT32  Reserved1:                  28;     /* [31:4] reserved */
} AMBA_SD_CLK_SRC_SELECT_REG_s;

/*
 * RCT: HDMI Clock Select Register
 */
typedef struct {
    UINT32  RefClkSel:                  1;      /* [0] Select ref clock for HDMI: 0 = xx_xin, 1 = output of hdmi-prescaler */
    UINT32  PhyTestModeEnable:          1;      /* [1] HDMI PHY test mode: 0 = disable, 1 = enable */
    UINT32  UsePhyClkVo:                1;      /* [2] 0 = Use gclk_vo, 1 = Use phy_clk_vo for HDMI digital logic and VOUT */
    UINT32  Reserved0:                  1;      /* [3] Reserved */
    UINT32  PowerDown:                  1;      /* [4] 0 = Power down HDMI PHY (active low) */
    UINT32  Reserved1:                  3;      /* [7:5] Reserved */
    UINT32  ClkRatioSelect:             2;      /* [9:8] 2'bx0 = 1/10 clock rate, 2'b01 = 1/40 clock rate, 2'b11 = bypass pll clock */
    UINT32  Reserved2:                  22;     /* [31:10] Reserved */
} AMBA_HDMI_CLK_SELECT_REG_s;

/*
 * RCT: SSI/SPI Reference Clock Selection Register
 */
typedef struct {
    UINT32  ClkSrc:                     2;      /* [1:0] SPI clock source: 2'b01 = gclk_core, others = no clock */
    UINT32  Reserved:                   30;     /* [31:2] Reserved */
} AMBA_SPI_CLK_REF_SELECT_REG_s;

/*
 * RCT: SPI-NOR Reference Clock Selection Register
 */
typedef struct {
    UINT32  ClkSrc:                     2;      /* [1:0] SPI clock source: 2'b00 = pll_out_enet, 2'b01 = gclk_core, 2'b10 = gclk_ssi,2' b11 = clk_ref */
    UINT32  Reserved:                   30;     /* [31:2] Reserved */
} AMBA_SPI_NOR_CLK_REF_SELECT_REG_s;

/*
 * RCT: PWM Clock Source Selection Register
 */
typedef struct {
    UINT32  ClkSrc:                     2;      /* [1:0] PWM clock source: 0 = gclk_apb, 1 = gclk_cortex, 2 = pll_out_enet, 3 = clk_ref */
    UINT32  Reserved:                   30;     /* [31:2] Reserved */
} AMBA_PWM_CLK_SRC_SELECT_REG_s;

/*
 * RCT: UART Clock Source Selection Register
 */
typedef struct {
    UINT32  ClkSrc:                     2;      /* [1:0] UART clock source: 0 = clk_ref, 1 = gclk_core, 2 = pll_out_enet, 3 = pll_out_sd */
    UINT32  Reserved:                   30;     /* [31:2] Reserved */
} AMBA_UART_CLK_SRC_SELECT_REG_s;

/*
 * RCT: ADC Clock Source Selection Register
 */
typedef struct {
    UINT32  ClkSrc:                     2;      /* [1:0] ADC clock source: 0 = clk_ref, 1 = gclk_vision, 2 = gclk_nand, 3 = no clock */
    UINT32  Reserved:                   30;     /* [31:2] Reserved */
} AMBA_ADC_CLK_SRC_SELECT_REG_s;

/*
 * RCT: Video2(LCD) Clock Source Selection Register
 */
typedef struct {
    UINT32  ClkSrc:                     1;      /* [0] 0 = gclk_vo2, 1 = gclk_vo */
    UINT32  Reserved:                   31;     /* [31:1] Reserved */
} AMBA_VOUT_LCD_CLK_SELECT_REG_s;

/*
 * RCT: CAN Bus Clock Source Selection Register
 */
typedef struct {
    UINT32  ClkSrc:                     2;      /* [1:0] CAN clock source: 0 = clk_ref, 1 = pll_out_enet, 2 = gclk_ahb, 3 = pll_out_sd */
    UINT32  Reserved:                   30;     /* [31:2] Reserved */
} AMBA_CAN_CLK_SRC_SELECT_REG_s;

/*
 * RCT: Muxed LVDS Clock Selection Register
 */
typedef struct {
    UINT32  ClkSrc:                     3;      /* [2:0] 0x1 = spclk_c[0], 0x2 = spclk_c[1], 0x4 = spclk_c[2] */
    UINT32  Reserved:                   29;     /* [31:3] Reserved */
} AMBA_MUX_LVDS_SPCLK_SELECT_REG_s;

/*
 * RCT: Muxed MIPI RX Byte Clock HS Selection Register
 */
typedef struct {
    UINT32  ClkSrc:                     3;      /* [2:0] 0x1 = mipi_rxbyteclkhs[0], 0x2 = mipi_rxbyteclkhs[1], 0x4 = mipi_rxbyteclkhs[2] */
    UINT32  Reserved:                   29;     /* [31:3] Reserved */
} AMBA_MUX_MIPI_RX_BYTE_CLK_SELECT_REG_s;

/*
 * RCT: Ethernet Clock Divider Register
 */
typedef struct {
    UINT32  Divider:                    8;      /* [7:0] Integer Div Value */
    UINT32  SoftReset:                  1;      /* [8] 1 = Reset divider */
    UINT32  Reserved:                   23;     /* [31:9] Reserved */
} AMBA_GTX_CLK_DIVIDER_REG_s;

/*
 * RCT: Ethernet Clock Divider Source Selection Register
 */
typedef struct {
    UINT32  ClkSrc:                     2;      /* [1:0] 0 = from gclk_enet_clkrmii_from_pll, 1 = from pll_out_cortext, 2 = pll_out_sd, 3 = clk_ref */
    UINT32  Reserved:                   30;     /* [31:2] Reserved */
} AMBA_GTX_CLK_DIVIDER_SRC_REG_s;

/*
 * RCT: Ethernet Clock Source Select Register
 */
typedef struct {
    UINT32  UseInternalClk:             1;      /* [0] 0 = External source (xx_enent_ext_osc_clk), 1 = internal source gclk_gtx_int */
    UINT32  Reserved:                   31;     /* [31:1] Reserved */
} AMBA_GTX_CLK_SRC_REG_s;

/*
 * RCT: Clock Enable Control Register
 */
typedef struct {
    UINT32  Reserved0:                  2;      /* [1:0] */
    UINT32  EnableMEMD:                 1;      /* [2] 0 = disable, 1 = enable */
    UINT32  Reserved1:                  1;      /* [3] Reserved */
    UINT32  EnableTSFM:                 1;      /* [4] 0 = disable, 1 = enable */
    UINT32  Reserved2:                  1;      /* [5] Reserved */
    UINT32  EnableCODE:                 1;      /* [6] 0 = disable, 1 = enable */
    UINT32  Reserved3:                  1;      /* [7] Reserved */
    UINT32  EnableSMEM:                 1;      /* [8] 0 = disable, 1 = enable */
    UINT32  Reserved4:                  1;      /* [9] Reserved */
    UINT32  EnableAXI:                  1;      /* [10] 0 = disable, 1 = enable */
    UINT32  Reserved5:                  1;      /* [11] Reserved */
    UINT32  EnableIDSP:                 1;      /* [12] 0 = disable, 1 = enable */
    UINT32  EnableCORE:                 1;      /* [13] 0 = disable, 1 = enable */
    UINT32  EnableLF:                   1;      /* [14] 0 = disable, 1 = enable */
    UINT32  EnableMDXF:                 1;      /* [15] 0 = disable, 1 = enable */
    UINT32  EnableMCTF:                 1;      /* [16] 0 = disable, 1 = enable */
    UINT32  Reserved6:                  1;      /* [17] Reserved */
    UINT32  EnableOL2C:                 1;      /* [18] 0 = disable, 1 = enable */
    UINT32  Reserved7:                  1;      /* [19] Reserved */
    UINT32  EnableVORC:                 1;      /* [20] 0 = disable, 1 = enable */
    UINT32  Reserved8:                  3;      /* [23:21] Reserved */
    UINT32  EnableVMEM0:                1;      /* [24] 0 = disable, 1 = enable */
    UINT32  Reserved9:                  1;      /* [25] Reserved */
    UINT32  EnableVP0:                  1;      /* [26] 0 = disable, 1 = enable */
    UINT32  Reserved10:                 5;      /* [31:27] Reserved */
} AMBA_CLK_ENABLE_CTRL_REG_s;

/*
 * RCT: PLL Clock Observation Control Register
 */
typedef struct {
    UINT32  PllOutSel:                  5;      /* [4:0] PLL output selection */
    UINT32  EnableObsv:                 1;      /* [5] 1 = Enable observation 1/16 of pll clock on xx_clk_si pin */
    UINT32  Reserved0:                  2;      /* [7:6] Reserved */
    UINT32  MipiLvdsObsv:               2;      /* [9:8] MIPI/LVDS clock observed on xx_clk_au. 2'bx0 = fn_clock, 2'b01 = muxed spclk_c, 2'b11 = muxed RxByteClkHS */
    UINT32  Reserved1:                  22;     /* [31:10] Reserved */
} AMBA_PLL_CLK_OBSV_REG_s;

/*
 * RCT: Software Reset 0 Register (Chip Software Reset and DLL Reset)
 */
typedef struct {
    UINT32  ChipSoftReset:              1;      /* [0] Chip soft reset control, active high */
    UINT32  Dll0Reset:                  1;      /* [1] DDR_IO 0 reset control, active low */
    UINT32  Reserved0:                  1;      /* [2] Reserved */
    UINT32  VoutReset:                  1;      /* [3] VOUT software reset control, active low */
    UINT32  Reserved1:                  28;     /* [31:4] Reserved */
} AMBA_SOFT_RESET0_REG_s;

/*
 * RCT: Software Reset 1 Register
 */
typedef struct {
    UINT32  DdrcReset:                  1;      /* [0] 1 = reset level signal for ddrc */
    UINT32  AxiReset:                   1;      /* [1] 1 = generate a reset pulse for axi */
    UINT32  IdspReset:                  1;      /* [2] 1 = reset level signal for idsp */
    UINT32  Reserved0:                  1;      /* [3] Reserved */
    UINT32  Vdsp1Reset:                 1;      /* [4] 1 = reset level signal for vdsp1 */
    UINT32  Vdsp2Reset:                 1;      /* [5] 1 = reset level signal for fma_top */
    UINT32  Reserved1:                  2;      /* [7:6] Reserved */
    UINT32  StorcReset:                 1;      /* [8] 1 = reset level signal for storc */
    UINT32  Vmem0Reset:                 1;      /* [9] 1 = reset level signal for vmem0 */
    UINT32  Vp0Reset:                   1;      /* [10] 1 = reset level signal for vp0 */
    UINT32  Reserved2:                  21;     /* [31:11] Reserved */
} AMBA_SOFT_RESET1_REG_s;

/*
 * RCT: Flash Controller Warm Reset Register
 */
typedef struct {
    UINT32  FlashReset:                 1;      /* [0] Flash reset, active high */
    UINT32  Reserved0:                  2;      /* [2:1] Reserved, active high */
    UINT32  FioReset:                   1;      /* [3] FIO Reset, active high */
    UINT32  Reserved1:                  28;     /* [31:4] Reserved, active high */
} AMBA_FIO_RESET_REG_s;

/*
 * RCT: Watchdog Timer Reset Register
 */
typedef struct {
    UINT32  WdtResetIndicator:          1;      /* [0] 0 = Not WDT reset, 1 = WDT reset. (Write 1 to clear) */
    UINT32  Reserved:                   31;     /* [31:1] Reserved */
} AMBA_WDT_RESET_STATE_REG_s;

/*
 * RCT: USB Reset Control Register
 */

#define USB_RSTCTRL_REG_HOST_RST_SHIFT    (0UL)
#define USB_RSTCTRL_REG_DEVICE_RST_SHIFT  (1UL)

#define USB_RSTCTRL_REG_HOST_RST_MASK     (1UL /*<< USB_RSTCTRL_REG_HOST_RST_SHIFT*/)
#define USB_RSTCTRL_REG_DEVICE_RST_MASK   (1UL << USB_RSTCTRL_REG_DEVICE_RST_SHIFT)

/*
 * RCT: IO PAD GPIO Control Register
 */
typedef struct {
    UINT32  Reserved0:                  3;      /* [2:0] Reserved */
    UINT32  InputType:                  1;      /* [3] 0 = CMOS, 1 = Schmitt input */
    UINT32  Reserved1:                  28;     /* [31:4] Reserved */
} AMBA_IO_CTRL_REG_s;

/*
 * RCT: HDMI Physical Control Register
 */
typedef struct {
    UINT32  Reset:                      1;      /* [0] 0 = Reset HDMI PHY, 1 = Release HDMI PHY reset */
    UINT32  Reserved0:                  3;      /* [3:1] Reserved */
    UINT32  SinkCurrent:                4;      /* [7:4] Sink current increment for output driver */
    UINT32  BiasCurrent:                4;      /* [11:8] Bias current adjusting */
    UINT32  Reserved1:                  4;      /* [15:12] Reserved */
    UINT32  FirstTapPreEmphasis:        4;      /* [19:16] 1st-tap Pre-Emphasis Current */
    UINT32  SecondTapPreEmphasis:       3;      /* [22:20] 2nd-tap Pre-Emphasis Current */
    UINT32  SecondTapDisable:           1;      /* [23] To disable 2nd-tap pre-emphasis */
    UINT32  ThirdTapPreEmphasis:        3;      /* [26:24] 3rd-tap Pre-Emphasis Current */
    UINT32  ThirdTapDisable:            1;      /* [27] To disable 3rd-tap pre-emphasis */
    UINT32  TerminationResistance:      2;      /* [29:28] Termination resistance */
    UINT32  Deskew:                     1;      /* [30] 1: Deskew between data and clock channel */
    UINT32  DoubleOutputCurrent:        1;      /* [31] Double the output current when termination is on */
} AMBA_HDMI_PHY_CTRL_REG_s;

/*
 * RCT: Random Number Generator Control Register
 */
typedef struct {
    UINT32  PowerDown:                  1;      /* [0] Rng_pd */
    UINT32  VcoStop:                    1;      /* [1] Rng_vcostop */
    UINT32  Select:                     1;      /* [2] Rng_sel */
    UINT32  Reserved0:                  5;      /* [7:3] Reserved */
    UINT32  Range:                      2;      /* [9:8] Rng_range */
    UINT32  Trim1:                      2;      /* [11:10] Rng_trim1 */
    UINT32  Trim2:                      2;      /* [13:12] Rng_trim2 */
    UINT32  Trim3:                      2;      /* [15:14] Rng_trim3 */
    UINT32  Reserved1:                  16;     /* [31:16] Reserved */
} AMBA_RAND_NUM_GEN_CTRL_REG_s;

/*
 * RCT: RCT Timer Control Register
 */
typedef struct {
    UINT32  Ctrl:                       2;      /* [1:0] 2'b00 = Enable timer, 2'b01 = Reset timer, 2'b1x = freeze timer */
    UINT32  Reserved:                   30;     /* [31:2] Reserved */
} AMBA_RCT_TIMER_CTRL_REG_s;

/*
 * RCT: AHB Misc Enable Control Register
 */
typedef struct {
    UINT32  Reserved0:                  2;      /* [1:0] Reserved */
    UINT32  SdHsPol:                    1;      /* [2] SD HS polarity */
    UINT32  Sdio0HsPol:                 1;      /* [3] SDIO0 HS polarity */
    UINT32  Sdio1HsPol:                 1;      /* [4] SDIO1 HS polarity */
    UINT32  EnetRefClkDir:              1;      /* [5] Ethernet reference clock (xx_enet_clk_rx) direction */
    UINT32  Reserved1:                  26;     /* [31:6] Reserved */
} AMBA_AHB_MISC_EN_REG_s;

/*
 * RCT: GPIO Debounce Enable Control Register
 */
typedef struct {
    UINT32  EnableGpio0Debounce:        1;      /* [0] 1 = Enable debounce logic for xx_debounce_gpio_0 */
    UINT32  EnableGpio1Debounce:        1;      /* [1] 1 = Enable debounce logic for xx_debounce_gpio_1 */
    UINT32  Reserved:                   30;     /* [31:2] Reserved */
} AMBA_GPIO_DEBOUNCE_CTRL_REG_s;

/*
 * RCT: Sensor Clock Pad Control Register
 */
typedef struct {
    UINT32  PadMode:                    1;      /* [0] clk_si pad is: 0 = output mode, 1 = input mode */
    UINT32  Reserved:                   31;     /* [31:1] Reserved */
} AMBA_SENSOR_CLK_PAD_CTRL_REG_s;

/*
 * RCT: SD PHY Control Registers
 */
typedef struct {
    UINT32  Reserved0:                  17;     /* [16:0] Reserved */
    UINT32  SdDllBypass:                1;      /* [17] sd_dll_bypass */
    UINT32  SdDataCmdBypass:            1;      /* [18] sd_data_cmd_bypass */
    UINT32  RxClkPolarity:              1;      /* [19] rx_clk_pol */
    UINT32  SdDutySelect:               2;      /* [21:20] sd_duty_sel[1:0] */
    UINT32  DelayChainSelect:           2;      /* [23:22] delay_chain_sel[1:0] */
    UINT32  Clk270Alone:                1;      /* [24] clk270_alone */
    UINT32  Reset:                      1;      /* [25] rst */
    UINT32  SdClkOutBypass:             1;      /* [26] sd_clkout_bypass */
    UINT32  SdDinClkPolarity:           1;      /* [27] change clock polarity  */
    UINT32  Reserved1:                  4;      /* [31:28] Reserved */
} AMBA_SD_PHY_CTRL0_REG_s;

typedef struct {
    UINT32  SdSelect0:                  8;      /* [7:0] rct_sd_sel0[7:0] */
    UINT32  SdSelect1:                  8;      /* [15:8] rct_sd_sel1[7:0] */
    UINT32  SdSelect2:                  8;      /* [23:16] rct_sd_sel2[7:0] */
    UINT32  SdSelect3:                  8;      /* [31:24] rct_sd_sel3[7:0] */
} AMBA_SD_PHY_CTRL1_REG_s;

typedef struct {
    UINT32  EnableDll:                  1;      /* [0] enable DLL*/
    UINT32  CoarseDelayStep:            3;      /* [3:1] adjust coarse delay with 1ns step */
    UINT32  LockRange:                  2;      /* [5:4] adjust lock range */
    UINT32  InternalClk:                1;      /* [6] select 1/16 internal clock */
    UINT32  PDbb:                       1;      /* [7] select PD-bb */
    UINT32  Reserved0:                  3;      /* [10:8] Reserved */
    UINT32  BypassFilter:               1;      /* [11] bypass filter when asserted high */
    UINT32  SelectFSM:                  1;      /* [12] Select FSM */
    UINT32  ForceLockVshift:            1;      /* [13] force lock when vshift=1F */
    UINT32  ForceLockCycle:             1;      /* [14] force lock after 32 internal cycles */
    UINT32  PowerDownShift:             1;      /* [15] power down shift register when locked */
    UINT32  EnableAutoCoarse:           1;      /* [16] Enable automatic coarse adjustment */
    UINT32  Reserved1:                  15;     /* [31:17] Reserved */
} AMBA_SD_PHY_CTRL2_REG_s;

/*
 * RCT: SD PHY Lock Oberservation0 Register
 */
typedef struct {
    UINT32  Sd0PhyStatus:               16;     /* [15:0] sd_rct_phy_sts[15:0] */
    UINT32  Sd1PhyStatus:               16;     /* [31:16] sdio_rct_phy_sts[15:0] */
} AMBA_SD_PHY_OBSERV0_REG_s;

/*
 * RCT: USB PHY Select Register
 */

#define USBPHY_SEL_REG_USB0SEL_SHIFT         (0UL)
#define USBPHY_SEL_REG_USB0SEL_MASK          (0x01UL /*<< USBPHY_SEL_REG_USB0SEL_SHIFT*/)

/*
 * RCT: USB PHY Control Registers
 */

#define USBPHY0_CTRL_REG_SOFT_RESET_SHIFT         (2UL)
#define USBPHY0_CTRL_REG_REF_CLK_SETL_SHIFT       (3UL)
#define USBPHY0_CTRL_REG_COMNON_RST_SHIFT         (6UL)
#define USBPHY0_CTRL_REG_COMPDIS_TUNE_SHIFT       (7UL)
#define USBPHY0_CTRL_REG_OTG_TUNE_SHIFT           (10UL)
#define USBPHY0_CTRL_REG_SQRX_TUNE_SHIFT          (13UL)
#define USBPHY0_CTRL_REG_TX_FSLS_TUNE_SHIFT       (16UL)
#define USBPHY0_CTRL_REG_TX_RISE_TUNE_SHIFT       (20UL)
#define USBPHY0_CTRL_REG_TX_VREF_TUNE_SHIFT       (22UL)
#define USBPHY0_CTRL_REG_TX_HSXV_TUNE_SHIFT       (26UL)
#define USBPHY0_CTRL_REG_ATE_RESET_SHIFT          (28UL)
#define USBPHY0_CTRL_REG_SLEEPM_TUNE_SHIFT        (30UL)

#define USBPHY0_CTRL_REG_SOFT_RESET_MASK          (0x01UL << USBPHY0_CTRL_REG_SOFT_RESET_SHIFT)
#define USBPHY0_CTRL_REG_REF_CLK_SETL_MASK        (0x03UL << USBPHY0_CTRL_REG_REF_CLK_SETL_SHIFT)
#define USBPHY0_CTRL_REG_COMNON_RST_MASK          (0x01UL << USBPHY0_CTRL_REG_COMNON_RST_SHIFT)
#define USBPHY0_CTRL_REG_COMPDIS_TUNE_MASK        (0x07UL << USBPHY0_CTRL_REG_COMPDIS_TUNE_SHIFT)
#define USBPHY0_CTRL_REG_OTG_TUNE_MASK            (0x07UL << USBPHY0_CTRL_REG_OTG_TUNE_SHIFT)
#define USBPHY0_CTRL_REG_SQRX_TUNE_MASK           (0x07UL << USBPHY0_CTRL_REG_SQRX_TUNE_SHIFT)
#define USBPHY0_CTRL_REG_TX_FSLS_TUNE_MASK        (0x0FUL << USBPHY0_CTRL_REG_TX_FSLS_TUNE_SHIFT)
#define USBPHY0_CTRL_REG_TX_RISE_TUNE_MASK        (0x03UL << USBPHY0_CTRL_REG_TX_RISE_TUNE_SHIFT)
#define USBPHY0_CTRL_REG_TX_VREF_TUNE_MASK        (0x0FUL << USBPHY0_CTRL_REG_TX_VREF_TUNE_SHIFT)
#define USBPHY0_CTRL_REG_TX_HSXV_TUNE_MASK        (0x03UL << USBPHY0_CTRL_REG_TX_HSXV_TUNE_SHIFT)
#define USBPHY0_CTRL_REG_ATE_RESET_MASK           (0x01UL << USBPHY0_CTRL_REG_ATE_RESET_SHIFT)
#define USBPHY0_CTRL_REG_SLEEPM_TUNE_MASK         (0x01UL << USBPHY0_CTRL_REG_SLEEPM_TUNE_SHIFT)

#define USBPHY1_CTRL_REG_DCD_ENABLE_SHIFT      (1UL)
#define USBPHY1_CTRL_REG_SRC_VOL_EN_SHIFT      (2UL)
#define USBPHY1_CTRL_REG_DET_VOL_EN_SHIFT      (3UL)
#define USBPHY1_CTRL_REG_CHG_SRC_SEL_SHIFT     (4UL)
#define USBPHY1_CTRL_REG_TX_AMP_TUNE_SHIFT     (12UL)

#define USBPHY1_CTRL_REG_DCD_ENABLE_MASK       (0x01UL << USBPHY1_CTRL_REG_DCD_ENABLE_SHIFT)
#define USBPHY1_CTRL_REG_SRC_VOL_EN_MASK       (0x01UL << USBPHY1_CTRL_REG_SRC_VOL_EN_SHIFT)
#define USBPHY1_CTRL_REG_DET_VOL_EN_MASK       (0x01UL << USBPHY1_CTRL_REG_DET_VOL_EN_SHIFT)
#define USBPHY1_CTRL_REG_CHG_SRC_SEL_MASK      (0x01UL << USBPHY1_CTRL_REG_CHG_SRC_SEL_SHIFT)
#define USBPHY1_CTRL_REG_TX_AMP_TUNE_MASK      (0x03UL << USBPHY1_CTRL_REG_TX_AMP_TUNE_SHIFT)

/*
 * RCT: USB Battery Charge Control Register
 */
typedef struct {
    UINT32  OpMode:                     2;      /* [1:0] Op Mode */
    UINT32  DmPullDown:                 1;      /* [2] D- Pull down */
    UINT32  DpPullDown:                 1;      /* [3] D+ Pull down */
    UINT32  BatteryChargeModeEnable:    1;      /* [4] Battery Charge Mode Enable */
    UINT32  Reserved:                   27;     /* [31:5] Reserved */
} AMBA_USB_CHARGE_CTRL_REG_s;

/*
 * RCT: MIPI DSI Control Registers
 */
typedef struct {
    UINT32  ClkGenerationEnable:        1;      /* [0] clkgen_en */
    UINT32  TxClkEscapeEnable:          1;      /* [1] txclkesc_en */
    UINT32  TxClkOutPol:                1;      /* [2] txclkout_pol */
    UINT32  Reserved0:                  1;      /* [3] Reserved */
    UINT32  TxClkEscapeDivider:         2;      /* [5:4] txclkesc_div[1:0] */
    UINT32  Reserved1:                  2;      /* [7:6] Reserved */
    UINT32  TxBiasEnable:               1;      /* [8] tx_bias_en */
    UINT32  TxIbCtrl:                   3;      /* [11:9] tx_ib_ctrl[2:0] */
    UINT32  Reserved2:                  4;      /* [15:12] Reserved */
    UINT32  TxVmdrv:                    1;      /* [16] tx_vmdrv */
    UINT32  TxVcmset:                   3;      /* [19:17] tx_vcmset[2:0] */
    UINT32  TxPib:                      4;      /* [23:20] tx_pib[3:0] */
    UINT32  LvdsResEnable:              1;      /* [24] lvds_res_en */
    UINT32  Reserved3:                  3;      /* [27:25] Reserved */
    UINT32  LvdsRsel:                   4;      /* [31:28] lvds_rsel[3:0] */
} AMBA_MIPI_DSI_CTRL0_REG_s;

typedef struct {
    UINT32  TxSlewCtrl:                 3;      /* [2:0] tx_slew_ctrl[2:0] */
    UINT32  Reserved0:                  1;      /* [3] Reserved */
    UINT32  TxLowpowerDelay:            2;      /* [5:4] tx_lp_dly[1:0] */
    UINT32  Reserved1:                  26;     /* [31:6] Reserved */
} AMBA_MIPI_DSI_CTRL1_REG_s;

typedef struct {
    UINT32  EnableClk:                  1;      /* [0] enable_clk */
    UINT32  Reserved0:                  3;      /* [3:1] Reserved */
    UINT32  TxEnable:                   4;      /* [7:4] tx_enable, tx_enable is soft reset for mipi_dsi  */
    UINT32  ForceInitClk:               1;      /* [8] force_init_clk */
    UINT32  Reserved1:                  3;      /* [11:9] Reserved */
    UINT32  ForceInit:                  4;      /* [15:12] force_init[3:0] */
    UINT32  ForceStopClk:               1;      /* [16] force_stop_clk */
    UINT32  Reserved2:                  3;      /* [19:17] Reserved */
    UINT32  ForceStop:                  4;      /* [23:20] force_stop[3:0] */
    UINT32  LpdtTimeoutCtrl:            5;      /* [28:24] lpdt_timeout_ctrl[4:0] */
    UINT32  Reserved3:                  3;      /* [31:29] Reserved */
} AMBA_MIPI_DSI_CTRL2_REG_s;

typedef struct {
    UINT32  RctInitTxCtrl:              6;      /* [5:0] rct_init_tx_ctrl[5:0] */
    UINT32  Reserved0:                  2;      /* [7:6] Reserved */
    UINT32  RctClkTrailCtrl:            5;      /* [12:8] rct_clk_trail_ctrl[4:0] */
    UINT32  Reserved1:                  3;      /* [15:13] Reserved */
    UINT32  RctClkZeroCtrl:             6;      /* [21:16] clk_zero_ctrl[5:0] */
    UINT32  Reserved2:                  2;      /* [23:22] Reserved */
    UINT32  RctClkPrepareCtrl:          6;      /* [29:24] clk_prepare_ctrl[5:0] */
    UINT32  RctClkContModeCtrl:         1;      /* [30] clk_cont_clk_mode */
    UINT32  Reserved3:                  1;      /* [31] Reserved */
} AMBA_MIPI_DSI_CTRL3_REG_s;

typedef struct {
    UINT32  ZeroCtrl:                   6;      /* [5:0] zero_ctrl[5:0] */
    UINT32  Reserved0:                  2;      /* [7:6] Reserved */
    UINT32  PrepareCtrl:                6;      /* [13:8] prepare_ctrl[5:0] */
    UINT32  Reserved1:                  2;      /* [15:14] Reserved */
    UINT32  TrailCtrl:                  5;      /* [20:16] trail_ctrl[4:0] */
    UINT32  Reserved2:                  3;      /* [23:21] Reserved */
    UINT32  LpxCtrl:                    8;      /* [31:24] lpx_ctrl[7:0] */
} AMBA_MIPI_DSI_CTRL4_REG_s;

typedef struct {
    UINT32  RctTxPrbsEnable:            1;      /* [0] rct_tx_prbs_lpbk_en */
    UINT32  Reserved0:                  7;      /* [7:1] Reserved */
    UINT32  RctTxClkMonitorEnable:      1;      /* [8] rct_tx_clk_monitor_en */
    UINT32  Reserved1:                  23;     /* [31:9] Reserved */
} AMBA_MIPI_DSI_CTRL5_REG_s;

/*
 * RCT: DAC Control Registers
 */
typedef struct {
    UINT32  DdfsFrequency:              24;     /* [23:0] ddfs_freq[23:0] */
    UINT32  DemEnable:                  1;      /* [24] dem_en */
    UINT32  BistEnable:                 1;      /* [25] bist_en */
    UINT32  Reserved0:                  2;      /* [27:26] Reserved */
    UINT32  Reset:                      1;      /* [28] reset (1 to assert, 0 to deassert) */
    UINT32  SelVrefin:                  1;      /* [29] sel_vrefin */
    UINT32  PowerDown:                  1;      /* [30] pd */
    UINT32  Reserved1:                  1;      /* [31] Reserved */
} AMBA_DAC_CTRL_REG_s;

/*
 * RCT: Ring Oscillator Ctrl Registers
 */
typedef struct {
    UINT32  ClkRefDiv:                  4;      /* [3:0] clk_ref_divisor */
    UINT32  ClkRoscDiv:                 2;      /* [5:4] clk_rosc_divisor */
    UINT32  ClkRefDisRst1:              1;      /* [6] clk_ref_divisor_rst_l */
    UINT32  ClkRoscDivRst1:             1;      /* [7] clk_rosc_divider_rst_l */
    UINT32  BinInfOfDll7bCh:            7;      /* [14:8] bin_in of dll_7b_ch */
    UINT32  Reserved0:                  1;      /* [15] Reserved */
    UINT32  RoscEnable:                 1;      /* [16] rosc_en */
    UINT32  Reserved1:                  15;     /* [31:17] Reserved */
} AMBA_RING_OSC_CTRL_REG_s;

/*
 * RCT: Ring Oscillator Observation Registers
 */
typedef struct {
    UINT32  RoscSampleCount:           24;      /* [23:0] rosc_sample_count */
    UINT32  Reserved:                  7;       /* [30:24] reserved */
    UINT32  ClkRefDivdRst:             1;       /* [31] clk_ref_divisor_rst_l */
} AMBA_RING_OSC_OBSV_REG_s;

#define AMBA_RCT_TIMER_ENABLE    0U                 /* Enable RCT timer */
#define AMBA_RCT_TIMER_RESET     1U                 /* Reset RCT timer */
#define AMBA_RCT_TIMER_FREEZE    2U                 /* Freeze RTC Timer */

#define AMB_VIN_PAD_MODE_SLVS   0x0U
#define AMB_VIN_PAD_MODE_LVCMOS 0x1U
#define AMB_VIN_PAD_MODE_MIPI   0x2U

/*
 * RCT: VOUT Clock Selection Register
 */
typedef struct {
    UINT32  VoutASelect:                1;      /* [0] rct_ahb_vout_clk_voa_sel */
    UINT32  VoutBSelect:                2;      /* [2:1] rct_ahb_vout_clk_vob_sel */
    UINT32  ComboPhySelect:             1;      /* [3] rct_combo_phy_clk_in_sel */
    UINT32  VoutBypassSelect:           1;      /* [4] rct_ahb_vout_bypass_clk_sel */
    UINT32  VinRefSelect:               1;      /* [5] rct_vin_ref_clk_sel */
    UINT32  Reserved:                   26;     /* [31:6] Reserved */
} AMBA_VOUT_CLK_SELECT_REG_s;

/*
 * RCT: All Registers
 */
typedef struct {
    volatile AMBA_PLL_CTRL_REG_s                    CorePllCtrl;            /* 0x000(RW): Core PLL integer configurations */
    volatile UINT32                                 CorePllFraction;        /* 0x004(RW): Core PLL fractional configurations */
    volatile AMBA_HDMI_CLK_SELECT_REG_s             HdmiClkSelect;          /* 0x008(RW): HDMI clock select */
    volatile AMBA_PLL_DIVIDER_16BIT_REG_s           Sd0ClkDivider;          /* 0x00C(RW): SD0/SD48 Clock Divider (LSB 17-bit) */
    volatile AMBA_HDMI_PHY_CTRL_REG_s               HdmiPhyCtrl;            /* 0x010(RW): HDMI PHY control register */
    volatile UINT32                                 Reserved0[4];           /* 0x014-0x020: Reserved */
    volatile AMBA_PLL_CTRL_REG_s                    SensorPllCtrl;          /* 0x024(RW): Sensor PLL integer configurations */
    volatile UINT32                                 SensorPllFraction;      /* 0x028(RW): Sensor PLL fractional configurations */
    volatile AMBA_PLL_LOCK_STATUS_REG_s             PllLockStatus;          /* 0x02C(RO): Hold the PLL lock status */
    volatile AMBA_PLL_DIVIDER_4BIT_REG_s            SensorPllPostscaler;    /* 0x030(RW): Sensor PLL post-scaler (LSB 8-bit) */
    volatile AMBA_SYS_CONFIG_REG_s                  SysConfig;              /* 0x034(RO): System configuration */
    volatile AMBA_PLL_DIVIDER_24BIT_REG_s           UartApbClkDivider;      /* 0x038(RW): Control register for UART_APB divider (LSB 24-bit) */
    volatile AMBA_PLL_DIVIDER_24BIT_REG_s           SpiMasterClkDivider;    /* 0x03C(RW): Control register for SPI master divider (LSB 24-bit) */
    volatile UINT32                                 Reserved1;              /* 0x040: Reserved */
    volatile AMBA_PLL_DIVIDER_24BIT_REG_s           IrClkDivider;           /* 0x044(RW): Control register for IR divider (LSB 24-bit) */
    volatile UINT32                                 Reserved2;              /* 0x048: Reserved */
    volatile AMBA_PLL_DIVIDER_4BIT_REG_s            SensorPllPrescaler;     /* 0x04C(RW): Sensor PLL pre-scaler (LSB 8-bit) */
    volatile AMBA_POWER_CTRL_REG_s                  PowerCtrl;              /* 0x050(RW): Analog power control */
    volatile AMBA_PLL_CTRL_REG_s                    AudioPllCtrl;           /* 0x054(RW): Audio PLL integer configurations */
    volatile UINT32                                 AudioPllFraction;       /* 0x058(RW): Audio PLL fractional configurations */
    volatile AMBA_PLL_DIVIDER_4BIT_REG_s            AudioPllPostscaler;     /* 0x05C(RW): Audio PLL post-scaler (LSB 8-bit) */
    volatile AMBA_PLL_DIVIDER_4BIT_REG_s            AudioPllPrescaler;      /* 0x060(RW): Audio PLL pre-scaler (LSB 8-bit) */
    volatile UINT32                                 Reserved3;              /* 0x064: Reserved */
    volatile UINT32                                 SoftReset0;             /* 0x068(RW): Software/DLL reset */
    volatile UINT32                                 Reserved4[2];           /* 0x06C-0x070: Reserved */
    volatile UINT32                                 FioReset;               /* 0x074(WO): Flash controller warm resets Active High */
    volatile UINT32                                 WdtResetState;          /* 0x078(RW): Watchdog Timer Reset State (Write Protected by 0x260) */
    volatile UINT32                                 Reserved5;              /* 0x07C: Reserved */
    volatile AMBA_PLL_DIVIDER_24BIT_REG_s           DebounceClkDivider;     /* 0x080(RW): SD card/USB VBUS detect debounce timing parameter */
    volatile AMBA_PLL_DIVIDER_24BIT_REG_s           PwmClkDivider;          /* 0x084(RW): Control register for PWM divider */
    volatile AMBA_PLL_DIVIDER_24BIT_REG_s           GpioDebounceClkDivider; /* 0x088(RW): Debounce clock for GPIO pins */
    volatile UINT32                                 ClkEnableCtrl;          /* 0x08C(RW): Clock enable control for all clusters */
    volatile UINT32                                 Reserved6[2];           /* 0x090-0x094: Reserved */
    volatile AMBA_GPIO_DEBOUNCE_CTRL_REG_s          GpioDebounceSelect;     /* 0x098(RW): Select whether a particular GPIO is debounced */
    volatile AMBA_PLL_DIVIDER_16BIT_REG_s           AdcClkDivider;          /* 0x09C(RW): ADC clock divider (LSB 17 bits) */
    volatile UINT32                                 Reserved7;              /* 0x0A0: Reserved */
    volatile AMBA_PLL_REF_CLK_SELECT_REG_s          AudioRefClkCtrl0;       /* 0x0A4(RW): Control Audio PLL reference clock */
    volatile AMBA_AUDIO_EXT_CLK_CTRL_REG_s          AudioExtClkCtrl;        /* 0x0A8(RW): Control if using external clock for audio */
    volatile AMBA_PLL_REF_CLK_SELECT_REG_s          VoutTvRefClkCtrl0;      /* 0x0AC(RW): Control Video (HDMI) PLL reference clock */
    volatile UINT32                                 Reserved8;              /* 0x0B0: Reserved */
    volatile AMBA_PLL_EXTERNAL_CLK_SRC_REG_s        AudioRefClkCtrl1;       /* 0x0B4(RW): Control which external clock used for audio reference clock */
    volatile AMBA_PLL_EXTERNAL_CLK_SRC_REG_s        VoutTvRefClkCtrl1;      /* 0x0B8(RW): Control which external clock used for video (TV) reference clock */
    volatile AMBA_SENSOR_CLK_PAD_CTRL_REG_s         SensorClk0PadCtrl;      /* 0x0BC(RW): Sensor Clock Pad is Input or Output */
    volatile AMBA_PLL_CTRL_REG_s                    VoutLcdPllCtrl;         /* 0x0C0(RW): Video2(LCD) PLL integer configurations */
    volatile UINT32                                 VoutLcdPllFraction;     /* 0x0C4(RW): Video2(LCD) PLL fractional configurations */
    volatile AMBA_PLL_DIVIDER_4BIT_REG_s            VoutLcdPllPrescaler;    /* 0x0C8(RW): Video2(LCD) PLL pre-scaler (LSB 8-bit) */
    volatile AMBA_PLL_DIVIDER_16BIT_REG_s           VoutLcdPllPostscaler;   /* 0x0CC(RW): Video2(LCD) PLL post-scaler (LSB 17 bits) */
    volatile AMBA_PLL_EXTERNAL_CLK_SRC_REG_s        VoutLcdRefClkCtrl1;     /* 0x0D0(RW): Control which external clock used for video2 reference clock */
    volatile UINT32                                 Reserved9;              /* 0x0D4: Reserved */
    volatile AMBA_PLL_REF_CLK_SELECT_REG_s          VoutLcdRefClkCtrl0;     /* 0x0D8(RW): Control if using external clock for video2 */
    volatile AMBA_PLL_CTRL_REG_s                    DdrPllCtrl;             /* 0x0DC(RW): DDR PLL integer configurations */
    volatile UINT32                                 DdrPllFraction;         /* 0x0E0(RW): DDR PLL fractional configurations */
    volatile AMBA_PLL_CTRL_REG_s                    IdspPllCtrl;            /* 0x0E4(RW): IDSP PLL integer configurations */
    volatile UINT32                                 IdspPllFraction;        /* 0x0E8(RW): IDSP PLL fractional configurations */
    volatile AMBA_PLL_DIVIDER_24BIT_REG_s           SpiSlaveClkDivider;     /* 0x0EC(RW): Control register for SPI slave divider (LSB 25 bits) */
    volatile UINT32                                 Reserved10[4];          /* 0x0F0-0x0FC: Reserved */

    volatile AMBA_PLL_CTRL2_REG_s                   CorePllCtrl2;           /* 0x100(RW): Core PLL contorl2 register */
    volatile AMBA_PLL_CTRL3_REG_s                   CorePllCtrl3;           /* 0x104(RW): Core PLL contorl3 register */
    volatile AMBA_PLL_CTRL2_REG_s                   IdspPllCtrl2;           /* 0x108(RW): IDSP PLL contorl2 register */
    volatile AMBA_PLL_CTRL3_REG_s                   IdspPllCtrl3;           /* 0x10C(RW): IDSP PLL contorl3 register */
    volatile AMBA_PLL_CTRL2_REG_s                   DdrPllCtrl2;            /* 0x110(RW): DDR PLL contorl2 register */
    volatile AMBA_PLL_CTRL3_REG_s                   DdrPllCtrl3;            /* 0x114(RW): DDR PLL contorl3 register */
    volatile UINT32                                 Reserved11;             /* 0x118: Reserved */
    volatile AMBA_PLL_CTRL2_REG_s                   SensorPllCtrl2;         /* 0x11C(RW): Sensor PLL contorl2 register */
    volatile AMBA_PLL_CTRL3_REG_s                   SensorPllCtrl3;         /* 0x120(RW): Sensor PLL contorl3 register */
    volatile AMBA_PLL_CTRL2_REG_s                   AudioPllCtrl2;          /* 0x124(RW): Audio PLL contorl2 register */
    volatile UINT32                                 Reserved12;             /* 0x128: Reserved */
    volatile AMBA_PLL_CTRL3_REG_s                   AudioPllCtrl3;          /* 0x12C(RW): Audio PLL contorl3 register */
    volatile UINT32                                 Reserved13[3];          /* 0x130-0x138: Reserved */
    volatile AMBA_PLL_CTRL2_REG_s                   VoutLcdPllCtrl2;        /* 0x13C(RW): Video2(LCD) PLL contorl2 register */
    volatile AMBA_PLL_CTRL3_REG_s                   VoutLcdPllCtrl3;        /* 0x140(RW): Video2(LCD) PLL contorl3 register */
    volatile UINT32                                 Reserved14;             /* 0x144: Reserved */
    volatile UINT32                                 DdrCalibClkDivider;     /* 0x148(RW): Clock divider to generate ddr calibration clock (LSB 4-bit) */
    volatile UINT32                                 Reserved15;             /* 0x14C: Reserved */
    volatile AMBA_PLL_CTRL2_REG_s                   HdmiPllCtrl2;           /* 0x150(RW): HDMI PLL contorl2 register */
    volatile AMBA_PLL_CTRL3_REG_s                   HdmiPllCtrl3;           /* 0x154(RW): HDMI PLL contorl3 register */
    volatile UINT32                                 Reserved16[3];          /* 0x158-0x160: Reserved */
    volatile AMBA_PLL_CTRL_REG_s                    HdmiPllCtrl;            /* 0x164(RW): HDMI PLL integer configurations */
    volatile UINT32                                 HdmiPllFraction;        /* 0x168(RW): HDMI PLL fractional configurations */
    volatile UINT32                                 Reserved17;             /* 0x16C: Reserved */
    volatile AMBA_PLL_DIVIDER_4BIT_REG_s            HdmiPhyPrescaler;       /* 0x170(RW): HDMI PLL pre-scaler (LSB 16-bit) */
    volatile UINT32                                 CorePllOutObsv;         /* 0x174(RO): Register to capture PLL output signals (for test/debug) */
    volatile UINT32                                 IdspPllOutObsv;         /* 0x178(RO): Register to capture PLL output signals (for test/debug) */
    volatile UINT32                                 DdrPllOutObsv;          /* 0x17C(RO): Register to capture PLL output signals (for test/debug) */
    volatile UINT32                                 SensorPllOutObsv;       /* 0x180(RO): Register to capture PLL output signals (for test/debug) */
    volatile UINT32                                 AudioPllOutObsv;        /* 0x184(RO): Register to capture PLL output signals (for test/debug) */
    volatile UINT32                                 Reserved18;             /* 0x188: Reserved */
    volatile UINT32                                 VoutLcdPllOutObsv;      /* 0x18C(RO): Register to capture PLL output signals (for test/debug) */
    volatile UINT32                                 Reserved19;             /* 0x190: Reserved */
    volatile UINT32                                 HdmiPllOutObsv;         /* 0x194(RO): Register to capture PLL output signals (for test/debug) */
    volatile AMBA_ADC_POWER_CTRL_REG_s              AdcPowerCtrl;           /* 0x198(RW): ADC Power Control register */
    volatile AMBA_SPI_CLK_REF_SELECT_REG_s          SpiMasterRefClkSelect;  /* 0x19C(RW): SSI master clock reference selection (LSB 2-bit) */
    volatile UINT32                                 Reserved20;             /* 0x1A0: Reserved */
    volatile AMBA_RAND_NUM_GEN_CTRL_REG_s           RngCtrl;                /* 0x1A4(RW): Random number generator */
    volatile UINT32                                 Reserved21[7];          /* 0x1A8-0x1C0: Reserved */
    volatile AMBA_PWM_CLK_SRC_SELECT_REG_s          PwmClkSrcSelect;        /* 0x1C4(RW): PWM Clock Source Selection */
    volatile AMBA_UART_CLK_SRC_SELECT_REG_s         UartApbClkSrcSelect;    /* 0x1C8(RW): UART APB Clock Source Selection */
    volatile UINT32                                 Reserved22;             /* 0x1CC: Reserved */
    volatile AMBA_ADC_CLK_SRC_SELECT_REG_s          AdcClkSelect;           /* 0x1D0(RW): ADC Clock Source Selection (LSB 2-bit) */
    volatile AMBA_VOUT_LCD_CLK_SELECT_REG_s         VoutLcdClkSelect;       /* 0x1D4(RW): Vout LCD Clock Source Selection (LSB 1-bit) */
    volatile UINT32                                 Reserved23[2];          /* 0x1D8-0x1DC: Reserved */
    volatile AMBA_PLL_CLK_OBSV_REG_s                ClkObsvCtrl;            /* 0x1E0(RW): Enables observation of PLL outputs on xx_clk_si pin (LSB 5-bit) */
    volatile UINT32                                 Reserved24[4];          /* 0x1E4-0x1F0: Reserved */
    volatile AMBA_PLL_DIVIDER_4BIT_REG_s            IdspPllPostscaler;      /* 0x1F4(RW): IDSP PLL post-scaler */
    volatile AMBA_IO_CTRL_REG_s                     IoCtrlGpio;             /* 0x1F8(RW): IO Control for GPIO (without drive strength & pull control) */
    volatile AMBA_IO_CTRL_REG_s                     IoCtrlMisc;             /* 0x1FC(RW): IO Control for all the pads other than 0x200-0x218 */

    volatile AMBA_IO_CTRL_REG_s                     IoCtrlWp;               /* 0x200(RW): IO Control for xx_wp pad */
    volatile AMBA_IO_CTRL_REG_s                     IoCtrlSmio[4];          /* 0x204-0x210(RW): IO Control for SMIO (without drive strength & pull control) */
    volatile UINT32                                 Reserved25;             /* 0x214: Reserved */
    volatile AMBA_IO_CTRL_REG_s                     IoCtrlSensorInput;      /* 0x218(RW): IO Control for VIN output pins (without drive strength & pull control) */
    volatile AMBA_AHB_MISC_EN_REG_s                 AhbMiscEnable;          /* 0x21C(RW): AHB Misc Enable Control */
    volatile AMBA_PLL_DIVIDER_DDR_INIT_REG_s        DdrInitClkDivider;      /* 0x220(RW): Clock contorl for DDR init */
    volatile AMBA_DDR_CLK_DIV_RESET_REG_s           DdrDividerReset;        /* 0x224(RW): Soft reset for DDRIO clock dividers (LSB 1-bit) */
    volatile AMBA_SOFT_RESET1_REG_s                 SoftReset1;             /* 0x228(RW): Soft reset for DDRC/iDSP/VDSP/SMEM (LSB 7-bit) */
    volatile UINT32                                 Reserved26;             /* 0x22C: Reserved */
    volatile AMBA_PLL_DIVIDER_4BIT_REG_s            Vin0ClkDivider;         /* 0x230(RW): VIN0 clock divider */
    volatile UINT32                                 Reserved27[8];          /* 0x234-0x250: Reserved */
    volatile UINT32                                 RctTimer0Counter;       /* 0x254(RO): Free running timer/counter on reference clock */
    volatile UINT32                                 RctTimer0Ctrl;          /* 0x258(RW): RCT timer control (LSB 2-bit) */
    volatile AMBA_PLL_LOCK_STATUS_REG_s             RawLockStatus;          /* 0x25C(RO): Hold the raw PLL lock status */
    volatile UINT32                                 UnlockWdtClear;         /* 0x260(RW): Make the write to WdtResetState work */
    volatile AMBA_PLL_CTRL_REG_s                    CortexPllCtrl;          /* 0x264(RW): Cortex PLL integer configurations */
    volatile UINT32                                 CortexPllFraction;      /* 0x268(RW): Cortex PLL fractional configurations */
    volatile AMBA_PLL_CTRL2_REG_s                   CortexPllCtrl2;         /* 0x26C(RW): Cortex PLL contorl2 register */
    volatile AMBA_PLL_CTRL3_REG_s                   CortexPllCtrl3;         /* 0x270(RW): Cortex PLL contorl3 register */
    volatile UINT32                                 CortexPllOutObsv;       /* 0x274(RO): Cortex PLL observation register to capture PLL output signals (for test/debug) */
    volatile UINT32                                 Reserved28[12];         /* 0x278-0x2A4: Reserved */
    volatile AMBA_GTX_CLK_DIVIDER_REG_s             GtxClkDivider;          /* 0x2A8(RW): Clock divider of gclk_gtx */
    volatile AMBA_GTX_CLK_DIVIDER_SRC_REG_s         GtxClkDividerSrc;       /* 0x2AC(RW): Clock divider source of gclk_gtx */
    volatile AMBA_GTX_CLK_SRC_REG_s                 GtxClkSelect;           /* 0x2B0(RW): Clock source select of gclk_gtx */
    volatile UINT32                                 Reserved29[3];          /* 0x2B4-0x2BC: Reserved */
    volatile UINT32                                 UsbPhySelect;           /* 0x2C0(RW): USB PHY Select register */
    volatile UINT32                                 Usb0PhyCtrl0;           /* 0x2C4(RW): USB0 (Device) PHY Control 0 register */
    volatile UINT32                                 Usb0PhyCtrl1;           /* 0x2C8(RW): USB0 (Device) PHY Control 1 register */
    volatile UINT32                                 UsbReset;               /* 0x2CC(RW): Soft Reset for Host/Device controller */
    volatile UINT32                                 Reserved30;             /* 0x2D0: Reserved */
    volatile AMBA_USB_CHARGE_CTRL_REG_s             UsbChargeCtrl;          /* 0x2D4(RW): USB battery charge control (LSB 5-bit) */
    volatile AMBA_CLK_DISABLE_REG_s                 ClkDisableCtrl;         /* 0x2D8(RW): Clocks disable control */
    volatile AMBA_PLL_CLK_SRC_SELECT_REG_s          CoreClkSrcSelect;       /* 0x2DC(RW): Clock source (LSB 2-bit), software shouldn't touch  */
    volatile UINT32                                 Reserved31;             /* 0x2E0: Reserved */
    volatile AMBA_PLL_CLK_SRC_SELECT_REG_s          DdrClkSrcSelect;        /* 0x2E4(RW): Clock source (LSB 2-bit), software shouldn't touch  */
    volatile AMBA_PLL_DIVIDER_4BIT_REG_s            DdrAltClkDivider;       /* 0x2E8(RW): DDR alternate clock source (divided from pll_out_core) */
    volatile AMBA_PLL_DIVIDER_4BIT_REG_s            VoutTvClkDivider;       /* 0x2EC(RW): VOUT TV clock source (divided from pll_out_hdmi) */
    volatile UINT32                                 Reserved32[4];          /* 0x2F0-0x2FC: Reserved */

    volatile UINT32                                 OtpConfigValue[2];      /* 0x300-0x304(RO): OTP data for configuration */
    volatile UINT32                                 Reserved33[3];          /* 0x308-0x310: Reserved */
    volatile UINT32                                 Gpio0DriveStrength[2];  /* 0x314-0x318(RW): GPIO Pin 0-31 Driving Strength */
    volatile UINT32                                 Gpio1DriveStrength[2];  /* 0x31C-0x320(RW): GPIO Pin 32-63 Driving Strength */
    volatile UINT32                                 Gpio2DriveStrength[2];  /* 0x324-0x328(RW): GPIO Pin 64-95 Driving Strength */
    volatile UINT32                                 Gpio3DriveStrength[2];  /* 0x32C-0x330(RW): GPIO Pin 96-127 Driving Strength */
    volatile UINT32                                 Reserved34[49];         /* 0x334-0x3F4: Reserved */
    volatile AMBA_PLL_SCALER_CTRL_REG_s             ScalerBypass;           /* 0x3F8(RW): Bypass Clock Dividers */
    volatile AMBA_PLL_SCALER_CTRL_REG_s             ScalerReset;            /* 0x3FC(RW): Reset Clock Dividers */

    volatile UINT32                                 Reserved35[12];         /* 0x400-0x42C: Reserved */
    volatile AMBA_PLL_DIVIDER_16BIT_REG_s           Sdio0ClkDivider;        /* 0x430(RW): SDIO0 Clock Divider (LSB 17-bit) */
    volatile AMBA_PLL_DIVIDER_16BIT_REG_s           Sdio1ClkDivider;        /* 0x434(RW): SDIO1 Clock Divider (LSB 17-bit) */
    volatile UINT32                                 Gpio4DriveStrength[2];  /* 0x438-0x43C(RW): GPIO Pin 128-159 Driving Strength */
    volatile UINT32                                 Reserved36[4];          /* 0x440-0x44C: Reserved */
    volatile UINT32                                 MiscDriveStrength[2];   /* 0x450-0x454(RW): Misc Pin (clk_au[1], detect_vbus[2], clk_si[3]) Driving Strength */
    volatile UINT32                                 Reserved37[11];         /* 0x458-0x480: Reserved */
    volatile AMBA_MUX_LVDS_SPCLK_SELECT_REG_s       MuxedLvdsSpclk;         /* 0x484(RW): External Reference Clock Selection for muxed_lvds_spclk */
    volatile AMBA_MUX_MIPI_RX_BYTE_CLK_SELECT_REG_s MuxedMipiRxByteClkHS;   /* 0x488(RW): External Reference Clock Selection for muxed_RxByteClkHS */
    volatile UINT32                                 Reserved38[2];          /* 0x48C-0x490: Reserved */
    volatile UINT32                                 RctTimer1Counter;       /* 0x494(RO): Free running timer/counter on reference clock */
    volatile UINT32                                 RctTimer1Ctrl;          /* 0x498(RW): RCT timer control (LSB 2-bit) */
    volatile UINT32                                 Reserved39[4];          /* 0x49C-0x4A8: Reserved */
    volatile AMBA_PLL_CTRL_REG_s                    SdPllCtrl;              /* 0x4AC(RW): SD PLL integer configurations */
    volatile UINT32                                 SdPllFraction;          /* 0x4B0(RW): SD PLL fractional configurations */
    volatile AMBA_PLL_CTRL2_REG_s                   SdPllCtrl2;             /* 0x4B4(RW): SD PLL contorl2 register */
    volatile AMBA_PLL_CTRL3_REG_s                   SdPllCtrl3;             /* 0x4B8(RW): SD PLL contorl3 register */
    volatile UINT32                                 SdPllOutObsv;           /* 0x4BC(RO): Register to capture PLL output signals (for test/debug) */
    volatile AMBA_SD_PHY_CTRL0_REG_s                Sd0PhyCtrl0;            /* 0x4C0(RW): SD0/SD PHY Control 0 */
    volatile AMBA_SD_PHY_CTRL1_REG_s                Sd0PhyCtrl1;            /* 0x4C4(RW): SD0/SD PHY Control 1 */
    volatile AMBA_SD_PHY_CTRL0_REG_s                Sd1PhyCtrl0;            /* 0x4C8(RW): SD1/SDIO0 PHY Control 0 */
    volatile AMBA_SD_PHY_CTRL1_REG_s                Sd1PhyCtrl1;            /* 0x4CC(RW): SD1/SDIO0 PHY Control 1 */
    volatile AMBA_SD_CLK_SRC_SELECT_REG_s           SdClkSrcSelect;         /* 0x4D0(RW): SD Clock Source Selection */
    volatile UINT32                                 Reserved40[6];          /* 0x4D4-0x4E8: Reserved */
    volatile AMBA_SPI_CLK_REF_SELECT_REG_s          SpiSlaveRefClkSelect;   /* 0x4EC(RW): SPI slave clock reference selection (LSB 1-bit) */
    volatile AMBA_SD_PHY_OBSERV0_REG_s              SdPhyObsv0;             /* 0x4F0(RO): SD PHY Observation */
    volatile UINT32                                 Reserved41;             /* 0x4F4: Reserved */
    volatile AMBA_PLL_CLK_SRC_SELECT_REG_s          CortexClkSrcSelect;     /* 0x4F8(RW): Clock source for cortex (LSB 2-bit), software shouldn't touch  */
    volatile UINT32                                 Reserved42;             /* 0x4FC: Reserved */

    volatile UINT32                                 Reserved43[6];          /* 0x500-0x514: Reserved */
    volatile AMBA_PLL_DIVIDER_24BIT_REG_s           SpiNorClkDivider;       /* 0x518(RW): Control register for SPI-NOR divider (LSB 25 bits) */
    volatile AMBA_SPI_NOR_CLK_REF_SELECT_REG_s      SpiNorRefClkSelect;     /* 0x51C(RW): SPI-NOR clock reference selection (LSB 2-bit) */
    volatile AMBA_PLL_CTRL_REG_s                    EnetPllCtrl;            /* 0x520(RW): Ethernet PLL integer configurations */
    volatile UINT32                                 EnetPllFraction;        /* 0x524(RW): Ethernet PLL fractional configurations */
    volatile AMBA_PLL_CTRL2_REG_s                   EnetPllCtrl2;           /* 0x528(RW): Ethernet PLL contorl2 register */
    volatile AMBA_PLL_CTRL3_REG_s                   EnetPllCtrl3;           /* 0x52C(RW): Ethernet PLL contorl3 register */
    volatile UINT32                                 EnetPllOutObsv;         /* 0x530(RO): Register to capture PLL output signals (for test/debug) */
    volatile AMBA_PLL_DIVIDER_4BIT_REG_s            EnetPllPostscaler;      /* 0x534(RW): Ethernet PLL post-scaler (LSB 8-bit) */
    volatile UINT32                                 OtpChipIdValue[2];      /* 0x538-0x53C(RO): OTP data for foundry to track the wafer information */
    volatile UINT32                                 Reserved44[11];         /* 0x540-0x568: Reserved */
    volatile AMBA_CAN_CLK_SRC_SELECT_REG_s          CanClkSrcSelect;        /* 0x56C(RW): CAN Clock Source Selection (LSB 2-bit) */
    volatile UINT32                                 CanClkDivider;          /* 0x570(RW): CAN Clock Divider (LSB 8-bit) */
    volatile AMBA_PLL_DIVIDER_4BIT_REG_s            Vin1ClkDivider;         /* 0x574(RW): Sensor VIN1 scaler */
    volatile UINT32                                 Reserved45[34];         /* 0x578-0X5FC: Reserved */

    volatile UINT32                                 Reserved46;             /* 0x600: Reserved */
    volatile AMBA_PLL_DIVIDER_4BIT_REG_s            Audio2Postscaler;       /* 0x604(RW): Audio2 post-scaler (LSB 8-bit) */
    volatile UINT32                                 Reserved47[11];         /* 0x608-0x630: Reserved */
    volatile UINT32                                 Usb0PhyCtrl2;           /* 0x634(RW): USB0 (Device) PHY Control 2 register */
    volatile UINT32                                 Reserved48;             /* 0x638: Reserved */
    volatile UINT32                                 Usb0PhyVdatRefTune;     /* 0x63C(RW): USB0 (Device) PHY VdatRefTune register */
    volatile UINT32                                 Reserved49[9];          /* 0x640-0x660: Reserved */
    volatile AMBA_SD_PHY_CTRL2_REG_s                Sd0PhyCtrl2;            /* 0x664(RW): SD0/SD PHY Control 2 */
    volatile AMBA_SD_PHY_CTRL2_REG_s                Sd1PhyCtrl2;            /* 0x668(RW): SD1/SDIO0 PHY Control 2 */
    volatile UINT32                                 OtpAmbaIdValue;         /* 0x66C(RO): OTP data for chip ID */
    volatile UINT32                                 MipiDsiCtrl0;           /* 0x670(RW): MIPI DSI Control 0 */
    volatile UINT32                                 MipiDsiCtrl1;           /* 0x674(RW): MIPI DSI Control 1 */
    volatile UINT32                                 MipiDsiCtrl2;           /* 0x678(RW): MIPI DSI Control 2 */
    volatile AMBA_MIPI_DSI_CTRL3_REG_s              MipiDsiCtrl3;           /* 0x67C(RW): MIPI DSI Control 3 */
    volatile AMBA_MIPI_DSI_CTRL4_REG_s              MipiDsiCtrl4;           /* 0x680(RW): MIPI DSI Control 4 */
    volatile UINT32                                 MipiDsiCtrl5;           /* 0x684(RW): MIPI DSI Control 5 */
    volatile AMBA_DAC_CTRL_REG_s                    DacCtrl;                /* 0x688(RW): DAC control register */
    volatile UINT32                                 MipiDsiAuxCtrl;         /* 0x68C(RW): MIPI DSI auxiliary control register */
    volatile UINT32                                 Reserved50[4];          /* 0x690-0x69C: Reserved */
    volatile UINT32                                 MipiDsiObsv;            /* 0x6A0(RO): MIPI DSI observation register */
    volatile UINT32                                 Reserved51[5];          /* 0x6A4-0x6B4: Reserved */
    volatile UINT32                                 EnetClkSrcSelect;       /* 0x6B8(RW): Ethernet Clock Source Selection */
    volatile UINT32                                 Reserved52[10];         /* 0x6BC-0x6E0: Reserved */
    volatile AMBA_PLL_CTRL_REG_s                    VisionPllCtrl;          /* 0x6E4(RW): Vision PLL integer configurations */
    volatile UINT32                                 VisionPllFraction;      /* 0x6E8(RW): Vision PLL fractional configurations */
    volatile AMBA_PLL_CTRL2_REG_s                   VisionPllCtrl2;         /* 0x6EC(RW): Vision PLL contorl2 register */
    volatile AMBA_PLL_CTRL3_REG_s                   VisionPllCtrl3;         /* 0x6F0(RW): Vision PLL contorl3 register */
    volatile UINT32                                 VisionPllObsv;          /* 0x6F4(RO): Vision PLL output observation */
    volatile AMBA_PLL_CTRL_REG_s                    NandPllCtrl;            /* 0x6F8(RW): Stereo PLL integer configurations */
    volatile UINT32                                 NandPllFraction;        /* 0x6FC(RW): Stereo PLL fractional configurations */

    volatile AMBA_PLL_CTRL2_REG_s                   NandPllCtrl2;           /* 0x700(RW): Stereo PLL contorl2 register */
    volatile AMBA_PLL_CTRL3_REG_s                   NandPllCtrl3;           /* 0x704(RW): Stereo PLL contorl3 register */
    volatile UINT32                                 NandPllObsv;            /* 0x708(RO): Stereo PLL output observation */
    volatile AMBA_RING_OSC_CTRL_REG_s               RingOscCtrl;            /* 0x70C(RW): Ring oscillator ctrl */
    volatile AMBA_RING_OSC_OBSV_REG_s               RingOscObsv;            /* 0x710(RO): Ring oscillator observation */
    volatile AMBA_PLL_DIVIDER_24BIT_REG_s           Uart0ClkDivider;        /* 0x714(RW): Control register for UART_0 divider (LSB 24-bit) */
    volatile AMBA_PLL_DIVIDER_24BIT_REG_s           Uart1ClkDivider;        /* 0x718(RW): Control register for UART_1 divider (LSB 24-bit) */
    volatile AMBA_PLL_DIVIDER_24BIT_REG_s           Uart2ClkDivider;        /* 0x71C(RW): Control register for UART_2 divider (LSB 24-bit) */
    volatile AMBA_PLL_DIVIDER_24BIT_REG_s           Uart3ClkDivider;        /* 0x720(RW): Control register for UART_3 divider (LSB 24-bit) */
    volatile UINT32                                 Reserved53[2];          /* 0x724-0x728: Reserved */
    volatile AMBA_UART_CLK_SRC_SELECT_REG_s         Uart0ClkSrcSelect;      /* 0x72C(RW): UART_0 Clock Source Selection */
    volatile AMBA_UART_CLK_SRC_SELECT_REG_s         Uart1ClkSrcSelect;      /* 0x730(RW): UART_1 Clock Source Selection */
    volatile AMBA_UART_CLK_SRC_SELECT_REG_s         Uart2ClkSrcSelect;      /* 0x734(RW): UART_2 Clock Source Selection */
    volatile AMBA_UART_CLK_SRC_SELECT_REG_s         Uart3ClkSrcSelect;      /* 0x738(RW): UART_3 Clock Source Selection */
    volatile UINT32                                 Reserved54[2];          /* 0x73C-0x740: Reserved */
    volatile AMBA_VOUT_CLK_SELECT_REG_s             VoutClkSelect;          /* 0x744(RW): VOUT clock selection register */
    volatile UINT32                                 Reserved55[11];         /* 0x748-0x770: Reserved */
    volatile UINT32                                 OtpSpareDataOut;        /* 0x774(RO): OTP Spare Data Out observation */
    volatile UINT32                                 Reserved56[2];          /* 0x778-0x77C: Reserved */
    volatile UINT32                                 Scratchpad[5];          /* 0x780-0x790(RW): General registers to retain data through soft-reset */
} AMBA_RCT_REG_s;

/*
 * MIPI/VIN PHY Registers
 */

typedef union {
    UINT32  Data;

    struct {
        UINT32  RxUlpsActiveNotClk0:        1;      /* [0] */
        UINT32  RxUlpsActiveNotClk1:        1;      /* [1] */
        UINT32  RxUlpsActiveNotClk2:        1;      /* [2] */
        UINT32  Reserved0:                  1;      /* [3] Reserved */
        UINT32  RxClkActiveHS0:             1;      /* [4] */
        UINT32  RxClkActiveHS1:             1;      /* [5] */
        UINT32  RxClkActiveHS2:             1;      /* [6] */
        UINT32  Reserved1:                  1;      /* [7] Reserved */
        UINT32  RxClkStopstate0:            1;      /* [8] */
        UINT32  RxClkStopstate1:            1;      /* [9] */
        UINT32  RxClkStopstate2:            1;      /* [10] */
        UINT32  Reserved2:                  21;     /* [31:11] Reserved */
    } Bits;
} AMBA_DPHY_OBSERVE0_REG_u;

typedef union {
    UINT32  Data;

    struct {
        UINT32  ErrControl:                 8;      /* [7:0] ErrControl[7:0] */
        UINT32  RxValidHS:                  8;      /* [15:8] RxValidHS[7:0] */
        UINT32  ErrSotHS:                   8;      /* [23:16] ErrSotHS[7:0] */
        UINT32  ErrSotSyncHS:               8;      /* [31:24] ErrSotSyncHS[7:0]*/
    } Bits;
} AMBA_DPHY_OBSERVE1_REG_u;

typedef union {
    UINT32  Data;

    struct {
        UINT32  RxActiveHS:                 8;      /* [7:0] RxActiveHS[7:0] */
        UINT32  RxStopState:                8;      /* [15:8] RxStopState[7:0] */
        UINT32  RxSyncHS:                   8;      /* [23:16] RxSyncHS[7:0] */
        UINT32  RxUlpsActiveNot:            8;      /* [31:24] RxUlpsActiveNot[7:0] */
    } Bits;
} AMBA_DPHY_OBSERVE2_REG_u;

typedef union {
    UINT32  Data;

    struct {
        UINT32  RxStateDataOut:             3;      /* [2:0] Rx_state_data_o, selected data lane's rx_state */
        UINT32  Reserved0:                  1;      /* [3] Reserved */
        UINT32  RxStateClkOut:              4;      /* [7:4] Rx_state_clk_o, selected clock lane's rx_state */
        UINT32  Reserved1:                  24;     /* [31:8] Reserved */
    } Bits;
} AMBA_DPHY_OBSERVE4_REG_u;

typedef union {
    UINT32  Data;

    struct {
        UINT32  DlyMatchMode:               2;      /* [1:0] dly_match_mode */
        UINT32  Reserved0:                  2;      /* [3:2] Reserved */
        UINT32  DphyRst0:                   1;      /* [4] reset mipi dphy digital part */
        UINT32  DphyRst1:                   1;      /* [5] reset mipi dphy digital part */
        UINT32  DphyRst2:                   1;      /* [6] reset mipi dphy digital part */
        UINT32  Reserved1:                  1;      /* [7] Reserved */
        UINT32  RstDphyAfe:                 3;      /* [10:8] reset mipi dphy analog front end */
        UINT32  Reserved2:                  1;      /* [11] Reserved */
        UINT32  NoVbn:                      1;      /* [12] no_vbn */
        UINT32  Reserved3:                  3;      /* [15:13] Reserved */
        UINT32  BitMode0:                   2;      /* [17:16] bit_mode0[1:0] */
        UINT32  BitMode1:                   2;      /* [19:18] bit_mode1[1:0] */
        UINT32  BitMode2:                   2;      /* [21:20] bit_mode2[1:0] */
        UINT32  Reserved4:                  2;      /* [23:22] Reserved */
        UINT32  ForceR100Off:               3;      /* [26:24] force_r100_off[1:0] (for clock lanes) */
        UINT32  Reserved5:                  1;      /* [27] Reserved */
        UINT32  LvdsResistanceSelect:       4;      /* [31:28] lvds_rsel */
    } Bits;
} AMBA_DPHY_CTRL0_REG_u;

typedef union {
    UINT32  Data;

    struct {
        UINT32  LvdsIbCtrl:                 2;      /* [1:0] lvds_ib_ctrl[1:0] (rx frontend current control) */
        UINT32  Reserved0:                  2;      /* [3:2] Reserved */
        UINT32  PowerDownFunc:              2;      /* [5:4] pd_func (functional power down) */
        UINT32  Reserved1:                  2;      /* [7:6] Reserved */
        UINT32  PowerDownLvds:              1;      /* [8] lvds_pd (pd rx frontend 'mipi_lvds_comp') */
        UINT32  Reserved2:                  7;      /* [15:9] Reserved */
        UINT32  DataR100Off:                8;      /* [23:16] data_r100_off[7:0] */
        UINT32  Reserved3:                  8;      /* [31:24] Reserved */
    } Bits;
} AMBA_DPHY_CTRL1_REG_u;

typedef union {
    UINT32  Data;

    struct {
        UINT32  ForceRxMode0:               1;      /* [0] ForceRxMode0 */
        UINT32  ForceRxMode1:               1;      /* [1] ForceRxMode1 */
        UINT32  ForceRxMode2:               1;      /* [2] ForceRxMode2 */
        UINT32  ForceRxMode3:               1;      /* [3] ForceRxMode3 */
        UINT32  ForceRxMode4:               1;      /* [4] ForceRxMode4 */
        UINT32  ForceRxMode5:               1;      /* [5] ForceRxMode5 */
        UINT32  ForceRxMode6:               1;      /* [6] ForceRxMode6 */
        UINT32  ForceRxMode7:               1;      /* [7] ForceRxMode7 */
        UINT32  Reserved0:                  8;      /* [15:8] Reserved */
        UINT32  ForceClkHs0:                1;      /* [16] ForceClkHs0, force MIPI Rx clock channel to go into HS state */
        UINT32  ForceClkHs1:                1;      /* [17] ForceClkHs1, force MIPI Rx clock channel to go into HS state */
        UINT32  ForceClkHs2:                1;      /* [18] ForceClkHs2, force MIPI Rx clock channel to go into HS state */
        UINT32  Reserved1:                  1;      /* [19] Reserved */
        UINT32  ForceMipiHsGp0:             1;      /* [20] force_mipi_hs0, force MIPI Rx data channel to go into HS state */
        UINT32  ForceMipiHsGp1:             1;      /* [21] force_mipi_hs1, force MIPI Rx data channel to go into HS state */
        UINT32  ForceMipiHsGp2:             1;      /* [22] force_mipi_hs2, force MIPI Rx data channel to go into HS state */
        UINT32  Reserved2:                  9;      /* [31:23] Reserved */
    } Bits;
} AMBA_DPHY_CTRL2_REG_u;

typedef union {
    UINT32  Data;

    struct {
        UINT32  LvCmosMode:                 8;      /* [7:0] LvCmos Mode */
        UINT32  Reserved0:                  8;      /* [15:8] Reserved */
        UINT32  LvCmosSpClk:                3;      /* [18:16] LvCmos_mode_spclk */
        UINT32  Reserved1:                  1;      /* [19] Reserved */
        UINT32  MipiMode:                   3;      /* [22:20] mipi_mode */
        UINT32  Reserved2:                  9;      /* [31:23] Reserved */
    } Bits;
} AMBA_DPHY_CTRL3_REG_u;

typedef union {
    UINT32  Data;

    struct {
        UINT32  HsTermCtrl0:                5;      /* [4:0] hs_term_ctrl */
        UINT32  Reserved0:                  3;      /* [7:5] Reserved */
        UINT32  HsSettleCtrl0:              6;      /* [13:8] hs_settle_ctrl */
        UINT32  Reserved1:                  2;      /* [15:14] Reserved */
        UINT32  InitRxCtrl0:                7;      /* [22:16] init_rx_ctrl */
        UINT32  Reserved2:                  1;      /* [23] Reserved */
        UINT32  ClkMissCtrl0:               5;      /* [28:24] clk_miss_ctrl */
        UINT32  Reserved3:                  3;      /* [31:29] Reserved */
    } Bits;
} AMBA_DPHY_CTRL4_REG_u;

typedef union {
    UINT32  Data;

    struct {
        UINT32  ClkSettleCtrl0:             6;      /* [5:0] hs_settle_ctrl */
        UINT32  Reserved0:                  2;      /* [7:6] Reserved */
        UINT32  ClkTermCtrl0:               5;      /* [12:8] clk_term_ctrl */
        UINT32  Reserved1:                  19;     /* [31:13] Reserved */
    } Bits;
} AMBA_DPHY_CTRL5_REG_u;

typedef union {
    UINT32  Data;

    struct {
        UINT32  RxStateDataSelect:          4;      /* [3:0] rx_state_data_sel */
        UINT32  RxStateClkSelect:           2;      /* [5:4] rx_state_clk_sel */
        UINT32  Reserved0:                  2;      /* [7:6] Reserved */
        UINT32  DataRctDebSelect:           4;      /* [11:8] data_rct_deb_sel */
        UINT32  RstnRctDeb:                 1;      /* [12] rst_n_rct_deb */
        UINT32  Reserved1:                  19;     /* [31:13] Reserved */
    } Bits;
} AMBA_DPHY_CTRL6_REG_u;

typedef union {
    UINT32  Data;

    struct {
        UINT32  HsTermCtrl1:                5;      /* [4:0] hs_term_ctrl */
        UINT32  Reserved0:                  3;      /* [7:5] Reserved */
        UINT32  HsSettleCtrl1:              6;      /* [13:8] hs_settle_ctrl */
        UINT32  Reserved1:                  2;      /* [15:14] Reserved */
        UINT32  InitRxCtrl1:                7;      /* [22:16] init_rx_ctrl */
        UINT32  Reserved2:                  1;      /* [23] Reserved */
        UINT32  ClkMissCtrl1:               5;      /* [28:24] clk_miss_ctrl */
        UINT32  Reserved3:                  3;      /* [31:29] Reserved */
    } Bits;
} AMBA_DPHY_CTRL7_REG_u;

typedef union {
    UINT32  Data;

    struct {
        UINT32  ClkSettleCtrl1:             6;      /* [5:0] hs_settle_ctrl */
        UINT32  Reserved0:                  2;      /* [7:6] Reserved */
        UINT32  ClkTermCtrl1:               5;      /* [12:8] clk_term_ctrl */
        UINT32  Reserved1:                  19;     /* [31:13] Reserved */
    } Bits;
} AMBA_DPHY_CTRL8_REG_u;

typedef union {
    UINT32  Data;

    struct {
        UINT32  HsTermCtrl2:                5;      /* [4:0] hs_term_ctrl */
        UINT32  Reserved0:                  3;      /* [7:5] Reserved */
        UINT32  HsSettleCtrl2:              6;      /* [13:8] hs_settle_ctrl */
        UINT32  Reserved1:                  2;      /* [15:14] Reserved */
        UINT32  InitRxCtrl2:                7;      /* [22:16] init_rx_ctrl */
        UINT32  Reserved2:                  1;      /* [23] Reserved */
        UINT32  ClkMissCtrl2:               5;      /* [28:24] clk_miss_ctrl */
        UINT32  Reserved3:                  3;      /* [31:29] Reserved */
    } Bits;
} AMBA_DPHY_CTRL9_REG_u;

typedef union {
    UINT32  Data;

    struct {
        UINT32  ClkSettleCtrl2:             6;      /* [5:0] hs_settle_ctrl */
        UINT32  Reserved0:                  2;      /* [7:6] Reserved */
        UINT32  ClkTermCtrl2:               5;      /* [12:8] clk_term_ctrl */
        UINT32  Reserved1:                  19;     /* [31:13] Reserved */
    } Bits;
} AMBA_DPHY_CTRL10_REG_u;

typedef union {
    UINT32  Data;

    struct {
        UINT32  CfgLaneEnable:              24;     /* [23:0] cfg_lane_enable */
        UINT32  CfgRefClkSelect:            1;      /* [24] cfg_ref_clk_sel */
        UINT32  CfgMipiMode:                1;      /* [25] cfg_mipi_mode */
        UINT32  CfgTestPattern:             2;      /* [27:26] cfg_test_pattern */
        UINT32  Reserved0:                  3;      /* [30:28] Reserved */
        UINT32  Rst_l:                      1;      /* [31] rst_l */
    } Bits;
} AMBA_PHY_BIST_CTRL11_REG_u;

typedef union {
    UINT32  Data;

    struct {
        UINT32  DbgRegEotFail:              8;      /* [7:0] dbg_reg_eot_fail */
        UINT32  Reserved0:                  24;     /* [31:8] Reserved */
    } Bits;
} AMBA_PHY_BIST_OBSV11_REG_u;

typedef union {
    UINT32  Data;

    struct {
        UINT32  DbgRegEotPass:              8;      /* [7:0] dbg_reg_eot_pass */
        UINT32  Reserved0:                  24;     /* [31:8] Reserved */
    } Bits;
} AMBA_PHY_BIST_OBSV12_REG_u;

typedef union {
    UINT32  Data;

    struct {
        UINT32  DbgRegSotFail:              8;      /* [7:0] dbg_reg_sot_fail */
        UINT32  Reserved0:                  24;     /* [31:8] Reserved */
    } Bits;
} AMBA_PHY_BIST_OBSV13_REG_u;

typedef union {
    UINT32  Data;

    struct {
        UINT32  DbgRegSotPass:              8;      /* [7:0] dbg_reg_sot_pass */
        UINT32  Reserved0:                  24;     /* [31:8] Reserved */
    } Bits;
} AMBA_PHY_BIST_OBSV14_REG_u;

typedef union {
    UINT32  Data;

    struct {
        UINT32  DbgRegTestPatternFail:      8;      /* [7:0] dbg_reg_test_pattern_fail */
        UINT32  Reserved0:                  24;     /* [31:8] Reserved */
    } Bits;
} AMBA_PHY_BIST_OBSV15_REG_u;

typedef union {
    UINT32  Data;

    struct {
        UINT32  DbgRegTestPatternPass:      8;      /* [7:0] dbg_reg_test_patttern_pass */
        UINT32  Reserved0:                  24;     /* [31:8] Reserved */
    } Bits;
} AMBA_PHY_BIST_OBSV16_REG_u;

typedef union {
    UINT32  Data;

    struct {
        UINT32  PhyBistFail:                8;      /* [7:0] phy_bist_fail */
        UINT32  Reserved0:                  24;     /* [31:8] Reserved */
    } Bits;
} AMBA_PHY_BIST_OBSV17_REG_u;

typedef union {
    UINT32  Data;

    struct {
        UINT32  PhyBistDone:                8;      /* [7:0] phy_bist_done */
        UINT32  Reserved0:                  24;     /* [31:8] Reserved */
    } Bits;
} AMBA_PHY_BIST_OBSV18_REG_u;

/*
 * MIPI/VIN PHY All Registers
 */
typedef struct {
    volatile AMBA_DPHY_OBSERVE0_REG_u               Dphy0Obsv0;             /* 0x000(RO): MIPI DPHY0 Observation (32-bit) */
    volatile AMBA_DPHY_OBSERVE1_REG_u               Dphy0Obsv1;             /* 0x004(RO): MIPI DPHY0 Observation (32-bit) */
    volatile AMBA_DPHY_OBSERVE2_REG_u               Dphy0Obsv2;             /* 0x008(RO): MIPI DPHY0 Observation Rx related (32-bit) */
    volatile UINT32                                 Dphy0Obsv3;             /* 0x00C(RO): MIPI DPHY0 Observation (32-bit) */
    volatile AMBA_DPHY_OBSERVE4_REG_u               Dphy0Obsv4;             /* 0x010(RO): MIPI DPHY0 Observation (32-bit) */
    volatile AMBA_DPHY_CTRL0_REG_u                  Dphy0Ctrl0;             /* 0x014(RW): MIPI DPHY0 Control 0 */
    volatile AMBA_DPHY_CTRL1_REG_u                  Dphy0Ctrl1;             /* 0x018(RW): MIPI DPHY0 Control 1 */
    volatile AMBA_DPHY_CTRL2_REG_u                  Dphy0Ctrl2;             /* 0x01C(RW): MIPI DPHY0 Control 2 */
    volatile AMBA_DPHY_CTRL3_REG_u                  Dphy0Ctrl3;             /* 0x020(RW): MIPI DPHY0 Control 3 */
    volatile AMBA_DPHY_CTRL4_REG_u                  Dphy0Ctrl4;             /* 0x024(RW): MIPI DPHY0 Control 4 */
    volatile AMBA_DPHY_CTRL5_REG_u                  Dphy0Ctrl5;             /* 0x028(RW): MIPI DPHY0 Control 5 */
    volatile AMBA_DPHY_CTRL6_REG_u                  Dphy0Ctrl6;             /* 0x02C(RW): MIPI DPHY0 Control 6 */
    volatile AMBA_DPHY_CTRL7_REG_u                  Dphy0Ctrl7;             /* 0x030(RW): MIPI DPHY0 Control 7 */
    volatile AMBA_DPHY_CTRL8_REG_u                  Dphy0Ctrl8;             /* 0x034(RW): MIPI DPHY0 Control 8 */
    volatile AMBA_DPHY_CTRL9_REG_u                  Dphy0Ctrl9;             /* 0x038(RW): MIPI DPHY0 Control 9 */
    volatile AMBA_DPHY_CTRL10_REG_u                 Dphy0Ctrl10;            /* 0x03C(RW): MIPI DPHY0 Control 10 */
    volatile AMBA_PHY_BIST_CTRL11_REG_u             MipiPhyBistCtrl11;      /* 0x040(RW): Control register for mipi_phy_bist */
    volatile AMBA_PHY_BIST_OBSV11_REG_u             MipiPhyBistObsv11;      /* 0x044(RO): Observe register for mipi_phy_bist */
    volatile AMBA_PHY_BIST_OBSV12_REG_u             MipiPhyBistObsv12;      /* 0x048(RO): Observe register for mipi_phy_bist */
    volatile AMBA_PHY_BIST_OBSV13_REG_u             MipiPhyBistObsv13;      /* 0x04C(RO): Observe register for mipi_phy_bist */
    volatile AMBA_PHY_BIST_OBSV14_REG_u             MipiPhyBistObsv14;      /* 0x050(RO): Observe register for mipi_phy_bist */
    volatile AMBA_PHY_BIST_OBSV15_REG_u             MipiPhyBistObsv15;      /* 0x054(RO): Observe register for mipi_phy_bist */
    volatile AMBA_PHY_BIST_OBSV16_REG_u             MipiPhyBistObsv16;      /* 0x058(RO): Observe register for mipi_phy_bist */
    volatile AMBA_PHY_BIST_OBSV17_REG_u             MipiPhyBistObsv17;      /* 0x05C(RO): Observe register for mipi_phy_bist */
    volatile AMBA_PHY_BIST_OBSV18_REG_u             MipiPhyBistObsv18;      /* 0x060(RO): Observe register for mipi_phy_bist */
} AMBA_MIPI_REG_s;

/*
 * Defined in AmbaMmioBase.asm
 */
#ifdef CONFIG_LINUX
extern AMBA_RCT_REG_s *pAmbaRCT_Reg;
#else
extern AMBA_RCT_REG_s *const pAmbaRCT_Reg;
#endif
extern AMBA_MIPI_REG_s *pAmbaMIPI_Reg;

#endif /* AMBA_REG_RCT_H */
