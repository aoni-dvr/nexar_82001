/**
 *  @file AmbaReg_RCT.h
 *
 *  @copyright Copyright (c) 2021 Ambarella, Inc.
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
    UINT32  Reserved0:                  1;      /* [0] */
    UINT32  ClkConfig:                  3;      /* [3:1] Power-On Reset IDSP/Core/DDR clock configuration */
    UINT32  BootMode:                   2;      /* [5:4] Boot mode select [1:0] */
    UINT32  SecureBoot:                 1;      /* [6] 1 = Secure boot */
    UINT32  Reserved1:                  1;      /* [7] */
    UINT32  BootBypass:                 1;      /* [8] 1 = Bypass fetching boot code from external device */
    UINT32  PeripheralClkMode:          1;      /* [9] Peripherals Clock Mode, 0 = gclk_core/2, 1 = gclk_core */
    UINT32  UsbBoot:                    1;      /* [10] 1 = Force USB boot */
    UINT32  ClkSource:                  2;      /* [12:11] Clock source for core & ddr clock: 0 = Normal, 1 = Ref clock, 2 = Reserved, 3 = Reserved */
    UINT32  BootOption0:                9;      /* [21:13] Boot mode option [8:0] */
    UINT32  Reserved2:                  9;      /* [30:22] Set to zero always (No Pins) */
    UINT32  SysConfigSource:            1;      /* [31] 0 = config data is set by pins, 1 = config data is read from OTP rom (except bit[10,31])) */
} AMBA_SYS_CONFIG_REG_s;

/*
 * RCT: Power Control Register
 */
typedef struct {
    UINT32  Reserved0:                  1;      /* [0] Reserved */
    UINT32  UsbPhySuspendDevice:        1;      /* [1] 0 = suspend USB PHY, 1 = suspend controlled by USB device controller */
    UINT32  UsbPhyNeverSuspend:         1;      /* [2] 1 = Never suspend USB PHY0, 0 = Allow suspend USB PHY */
    UINT32  Reserved1:                  2;      /* [4:3] Reserved */
    UINT32  PwcPowerDown:               1;      /* [5] PWC power down sequence starts by the transition from 0 to 1  */
    UINT32  Reserved2:                  6;      /* [11:6] Reserved */
    UINT32  UsbPhySuspendHost:          1;      /* [12] 0 = suspend USB PHY, 1 = suspend controlled by USB host controller */
    UINT32  UsbPhy1NeverSuspend:        1;      /* [13] 1 = Never suspend USB PHY1, 0 = Allow suspend USB PHY */
    UINT32  Reserved3:                  18;     /* [31:14] Reserved */
} AMBA_POWER_CTRL_REG_s;

/*
 * RCT: ADC Power Control Register
 */
typedef struct {
    UINT32  ClkSelect:                  1;      /* [0] Clock ADC select */
    UINT32  PowerDown:                  1;      /* [1] 1 = ADC power down */
    UINT32  Reserved1:                  6;      /* [7:2] Reserved */
    UINT32  PowerDownScaler:            16;     /* [23:8] 1 = ADC scaler power down */
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
    UINT32  Vin0Scaler:                 1;      /* [6] VIN0 post scaler */
    UINT32  Reserved1:                  1;      /* [7] Reserved */
    UINT32  IdspPostScaler:             1;      /* [8] IDSP post scaler */
    UINT32  Reserved2:                  4;      /* [12:9] Reserved */
    UINT32  AudioPreScaler:             1;      /* [13] Audio pre scaler */
    UINT32  SensorPreScaler:            1;      /* [14] Sensor pre scaler */
    UINT32  VideoAPreScaler:            1;      /* [15] Video A pre scaler */
    UINT32  Reserved3:                  1;      /* [16] Reserved */
    UINT32  HdmiPreScaler:              1;      /* [17] HDMI pre scaler */
    UINT32  Reserved4:                  4;      /* [21:18] Reserved */
    UINT32  SysCntScaler:               1;      /* [22] System counter post scaler */
    UINT32  Vin1Scaler:                 1;      /* [23] VIN1 post scaler */
    UINT32  EnetPostScaler:             1;      /* [24] Ethernet post scaler */
    UINT32  Audio2PostScaler:           1;      /* [25] Audio2 post scaler */
    UINT32  Audio3PostScaler:           1;      /* [26] Audio3 post scaler */
    UINT32  Reserved7:                  5;      /* [31:27] Reserved */
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
    UINT32  Reserved2:                  13;     /* [21:9] Reserved */
    UINT32  SysCntPostScalar:           1;      /* [22] 1 = shutoff clock */
    UINT32  Vin1PostScalar:             1;      /* [23] 1 = shutoff clock */
    UINT32  EnetPostScaler:             1;      /* [24] 1 = shutoff clock */
    UINT32  Enet1PostScaler:            1;      /* [25] 1 = shutoff clock */
    UINT32  Audio3PostScaler:           1;      /* [26] 1 = shutoff clock */
    UINT32  Audio2PostScaler:           1;      /* [27] 1 = shutoff clock */
    UINT32  VoutTvPostScaler:           1;      /* [28] 1 = shutoff clock */
    UINT32  Reserved3:                  3;      /* [31:29] Reserved */
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
    UINT32  VideoALock:                 1;      /* [0] PLL video A lock: 0 - unlock, 1 - lock */
    UINT32  Reserved0:                  1;      /* [1] Reserved */
    UINT32  VisionLock:                 1;      /* [2] PLL vision lock: 0 - unlock, 1 - lock */
    UINT32  CortexLock:                 1;      /* [3] PLL cortex lock: 0 - unlock, 1 - lock */
    UINT32  SensorLock:                 1;      /* [4] PLL sensor lock: 0 - unlock, 1 - lock */
    UINT32  IdspLock:                   1;      /* [5] PLL IDSP lock: 0 - unlock, 1 - lock */
    UINT32  Reserved1:                  1;      /* [6] Reserved */
    UINT32  CoreLock:                   1;      /* [7] PLL core lock: 0 - unlock, 1 - lock */
    UINT32  AudioLock:                  1;      /* [8] PLL audio lock: 0 - unlock, 1 - lock */
    UINT32  DdrHost0Lock:               1;      /* [9] PLL DDR host0 lock: 0 - unlock, 1 - lock */
    UINT32  DdrHost1Lock:               1;      /* [10] PLL DDR host1 lock: 0 - unlock, 1 - lock */
    UINT32  HdmiLock:                   1;      /* [11] PLL HDMI lock: 0 - unlock, 1 - lock */
    UINT32  HdmiLcLock:                 1;      /* [12] PLL HDMILC lock: 0 - unlock, 1 - lock */
    UINT32  SdLock:                     1;      /* [13] PLL SD lock: 0 - unlock, 1 - lock */
    UINT32  EnetLock:                   1;      /* [14] PLL ENET lock: 0 - unlock, 1 - lock */
    UINT32  IdspvLock:                  1;      /* [15] PLL IDSPV lock: 0 - unlock, 1 - lock */
    UINT32  NandLock:                   1;      /* [16] PLL NAND lock: 0 - unlock, 1 - lock */
    UINT32  SlveLock:                   1;      /* [17] PLL SLVS lock: 0 - unlock, 1 - lock */
    UINT32  DsuLock:                    1;      /* [18] PLL DSU lock: 0 - unlock, 1 - lock */
    UINT32  Reserved2:                  13;     /* [31:19] Reserved */
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
    UINT32  AudioClkSrc:                1;      /* [0] gclk_au is: 0 = xx_clk_au, 1 = unused */
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
typedef union {
    UINT32  Data;

    struct {
        UINT32  RefClkSel:                  1;      /* [0] Select ref clock for HDMI: 0 = xx_hdmi_xo, 1 = xx_xin */
        UINT32  Reserved0:                  1;      /* [1] Reserved */
        UINT32  UsePhyClkVo:                1;      /* [2] 0 = Use gclk_vo, 1 = Use phy_clk_vo for HDMI digital logic and VOUT */
        UINT32  SelRing:                    1;      /* [3] 0 = pll1, 1 = pll1/pll cascased output */
        UINT32  PowerDown:                  1;      /* [4] 0 = Power down HDMI PHY (active low) */
        UINT32  Reserved1:                  3;      /* [7:5] Reserved */
        UINT32  ClkRatioSelect:             2;      /* [9:8] 2'bx0 = 1/10 clock rate, 2'b01 = 1/40 clock rate, 2'b11 = bypass pll clock */
        UINT32  Reserved2:                  5;      /* [14:10] Reserved */
        UINT32  PllCascade:                 1;      /* [15] cascade lcpll before ringpll  */
        UINT32  Reserved3:                  16;     /* [31:16] Reserved */
    } Bits;
} AMBA_HDMI_CLK_SELECT_REG_u;
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
    UINT32  ClkSrc:                     2;      /* [1:0] ADC clock source: 0 = clk_ref, 1 = gclk_idsp, 2 = gclk_nand, 3 = clk_ref */
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
    UINT32  ClkSrc:                     2;      /* [1:0] 0 = from gclk_enet_clkrmii_from_pll, 1 = from pll_out_cortex, 2 = pll_out_sd, 3 = clk_ref */
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
 * RCT: Ethernet Control Register
 */
typedef struct {
    UINT32  EnetSelect:                 1;      /* [0] 0 = Non-Ethernet mode, 1 = Ethernet mode */
    UINT32  EnetPhyIntfSelect:          1;      /* [1] 0 = RGMII mode, 1 = RMII mode */
    UINT32  Reserved:                   30;     /* [31:2] Reserved */
} AMBA_ENET_CTRL_REG_s;

/*
 * RCT: CAN Reference Clock Selection Register
 */
typedef struct {
    UINT32  ClkSrc:                     2;      /* [1:0] SPI clock source: 2'b00 = pll_out_ref, 2'b01 = pll_out_ent, 2'b10 = pll_out_sd,2' b11 = glck_ahb */
    UINT32  Reserved:                   30;     /* [31:2] Reserved */
} AMBA_CAN_CLK_REF_SELECT_REG_s;

/*
 * RCT: CAN Clock Divider Register
 */
typedef struct {
    UINT32  Divider:                    8;      /* [7:0] Integer Div Value */
    UINT32  SoftReset:                  1;      /* [8] 1 = Reset divider */
    UINT32  Reserved:                   23;     /* [31:9] Reserved */
} AMBA_CAN_CLK_DIVIDER_REG_s;
/*
 * RCT: Clock Enable Control Register
 */
typedef struct {
    UINT32  EnableORC0:                 1;    /* [0] */
    UINT32  EnableME0:                  1;    /* [1] */
    UINT32  EnableMDENC0:               1;    /* [2] */
    UINT32  EnableTSFM0:                1;    /* [3] */
    UINT32  EnableMDDEC0:               1;    /* [4] */
    UINT32  EnablePJPEG0:               1;    /* [5] */
    UINT32  EnableLF0:                  1;    /* [6] */
    UINT32  EnableHMX0:                 1;    /* [7] */
    UINT32  EnableORC1:                 1;    /* [8] */
    UINT32  EnableME1:                  1;    /* [9] */
    UINT32  EnableMDENC1:               1;    /* [10]*/
    UINT32  EnableTSFM01:               1;    /* [11]*/
    UINT32  EnableMDDEC1:               1;    /* [12]*/
    UINT32  EnablePJPEG1:               1;    /* [13]*/
    UINT32  EnableLF1:                  1;    /* [14]*/
    UINT32  EnableHMX1:                 1;    /* [15]*/
    UINT32  EnableSMEM:                 1;    /* [16]*/
    UINT32  EnableCODE:                 1;    /* [17]*/
    UINT32  EnableAXI0:                 1;    /* [18]*/
    UINT32  Reserved0:                  1;    /* [19]*/
    UINT32  EnableIDSP0:                1;    /* [20]*/
    UINT32  EnableIDSP1:                1;    /* [21]*/
    UINT32  EnableOL2C:                 1;    /* [22]*/
    UINT32  EnableVORC:                 1;    /* [23]*/
    UINT32  EnableVMEM:                 1;    /* [24]*/
    UINT32  EnableVP:                   1;    /* [25]*/
    UINT32  EnableIDSPV:                1;    /* [26]*/
    UINT32  Reserved1:                  1;    /* [27]*/
    UINT32  EnableDBSE:                 1;    /* [28]*/
    UINT32  Reserved2:                  3;    /* [29:31]*/
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
    UINT32  Vdsp2Reset:                 1;      /* [5] 1 = reset level signal for vdsp2 */
    UINT32  Reserved1:                  2;      /* [7:6] Reserved */
    UINT32  StorcReset:                 1;      /* [8] 1 = reset level signal for storc */
    UINT32  VmemReset:                  1;      /* [9] 1 = reset level signal for vmem0 */
    UINT32  VpReset:                    1;      /* [10] 1 = reset level signal for vp0 */
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
typedef union {
    UINT32  Data;

    struct {
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
    } Bits;
} AMBA_HDMI_PHY_CTRL_REG_u;

/*
 * RCT: T2V Control Register
 */
typedef struct {
    UINT32  PowerDown:                  1;      /* [0] Power control */
    UINT32  SelLowpT2V:                 1;      /* [1] sel_lowp_t2v */
    UINT32  Reserved0:                  6;      /* [7:2] Reserved */
    UINT32  CtrlT2V:                    8;      /* [15:8] ctrl_t2v */
    UINT32  Reserved1:                  16;     /* [31:16] Reserved */
} AMBA_T2V_CTRL_REG_s;

/*
 * RCT: Random Number Generator Control Register
 */
typedef struct {
    UINT32  PowerDown:                  1;      /* [0] Rng_pd */
    UINT32  Reset:                      1;      /* [1] Rng_rst */
    UINT32  VcoStop:                    1;      /* [2] Rng_vcostop */
    UINT32  Select:                     1;      /* [3] Rng_sel */
    UINT32  TrimEn:                     1;      /* [4] Rng_trim_en */
    UINT32  ErrCheck:                   1;      /* [5] Rng_err_check */
    UINT32  Reserved0:                  2;      /* [7:6] Reserved */
    UINT32  Range:                      2;      /* [9:8] Rng_range */
    UINT32  Trim1:                      3;      /* [12:10] Rng_trim1 */
    UINT32  Trim2:                      3;      /* [15:13] Rng_trim2 */
    UINT32  Reserved1:                  16;     /* [31:16] Reserved */
} AMBA_RAND_NUM_GEN_CTRL_REG_s;

/*
 * RCT: RCT Timer Control Register
 */
#define AMBA_RCT_TIMER_ENABLE    0U             /* Enable RCT timer */
#define AMBA_RCT_TIMER_RESET     1U             /* Reset RCT timer */
#define AMBA_RCT_TIMER_FREEZE    2U             /* Freeze RTC Timer */

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
    UINT32  RxClkPolarity:              1;      /* [19] sd_dll_clk_pol */
    UINT32  SdDutySelect:               2;      /* [21:20] sd_duty_sel[1:0] */
    UINT32  DelayChainSelect:           2;      /* [23:22] delay_chain_sel[1:0] */
    UINT32  Clk270Alone:                1;      /* [24] clk270_alone */
    UINT32  Reset:                      1;      /* [25] rst */
    UINT32  SdClkOutBypass:             1;      /* [26] sd_clkout_bypass */
    UINT32  SdDinClkPolarity:           1;      /* [27] sd_din_clk_pol  */
    UINT32  Reserved1:                  4;      /* [31:28] Reserved */
} AMBA_SD_PHY_CTRL0_REG_s;

typedef struct {
    UINT32  SdSelect0:                  6;      /* [5:0] rct_sd_sel0[5:0] */
    UINT32  Reserved0:                 10;      /* [15:6] Reserved */
    UINT32  SdSelect2:                  6;      /* [21:16] rct_sd_sel2[7:0] */
    UINT32  Reserved1:                  2;      /* [23:22] Reserved */
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

typedef struct {
    UINT32  PllPTune:                   4;      /* [3:0] */
    UINT32  PllITune:                   2;      /* [5:4] */
    UINT32  PllBTune:                   1;      /* [6] */
    UINT32  Reserved:                   25;     /* [31:7] Reserved */
} AMBA_USB_PHY_CTRL2_REG_s;

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

/*
 * RCT: VOUT Clock Selection Register
 */
typedef struct {
    UINT32  VoutARefClkSelect:          1;      /* [0] vout_a_clk_ref_sel  */
    UINT32  DsiAClkInSelect:            1;      /* [1] mipi_dsi_a_clk_in_sel */
    UINT32  VoutASelect:                1;      /* [2] rct_ahb_vout_clk_vo_a_sel */
    UINT32  Reserved0:                  1;      /* [3] Reserved */
    UINT32  VoutBypassSelect:           1;      /* [4] phy_clk_vo_b_bypass_clk_sel */
    UINT32  Reserved1:                  3;      /* [7:5] Reserved */
    UINT32  VoutBRefClkSelect:          1;      /* [8] vout_b_clk_ref_sel  */
    UINT32  DsiBClkInSelect:            1;      /* [9] mipi_dsi_b_clk_in_sel */
    UINT32  VoutBSelect:                1;      /* [10] rct_ahb_vout_clk_vo_b_sel */
    UINT32  Reserved2:                  1;      /* [11] Reserved */
    UINT32  VoutCSelect:                2;      /* [13:12] rct_ahb_vout_clk_vo_b_sel */
    UINT32  Reserved3:                  18;     /* [31:14] Reserved */
} AMBA_VOUT_CLK_SELECT_REG_s;

typedef struct {
    UINT32  VinRefClkSelect:            5;      /* [4:0] vin_ref_clk_sel */
    UINT32  Reserved:                   27;     /* [31:5] Reserved */
} AMBA_VIN_CLK_SELECT_REG_s;

/*
 * RCT: VIN1/PIP Clock Divider Source Selection Register
 */
typedef struct {
    UINT32  ClkSrc:                     1;      /* [0] 0 = from pll_out_enet, 1 = from pll_out_sensor */
    UINT32  Reserved:                   31;     /* [31:1] Reserved */
} AMBA_VIN1_CLK_DIVIDER_SRC_REG_s;

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
    UINT32  TxAuxEn:                    1;      /* [0] 1: Enable auxiliary driver for TX. 0: disable */
    UINT32  TxPreEn:                    1;      /* [1] 1: With tx_aux_en=1, set this bit to 1 to use auxiliary driver as pre-emphasis driver. 0: no pre-emphasis */
    UINT32  Reserved0:                  2;      /* [3:2] Reserved */
    UINT32  TxPre:                      3;      /* [6:4] Pre-emphasis current */
    UINT32  Reserved1:                  1;      /* [7] Reserved */
    UINT32  TxMode:                     2;      /* [9:8] 0: MIPI DSI mode. 1: FPD mode. 2: serial SLVS mode */
    UINT32  Reserved2:                  2;      /* [11:10] Reserved */
    UINT32  TxUlvCm:                    1;      /* [12] 1: Enable ultra low vcm mode */
    UINT32  BtaDisable:                 1;      /* [13] 0: Enable TX BTA and allow peripherla to drive lines. 1: Disable LP BTA */
    UINT32  BtaRqst:                    1;      /* [14] 1: TX sends out BTA upon rising edge of bta_rqst */
    UINT32  TestPatternEn:              1;      /* [15] 1: Enable driver to output a given 8bit test pattern */
    UINT32  TestPattern:                8;      /* [23:16] 8bit test pattern when bit15 is set */
    UINT32  ObsvSel:                    3;      /* [26:24] 0: clk ch. 1-4: data ch0-3, 5-6: lp receive data */
    UINT32  Reserved3:                  1;      /* [27] Reserved */
    UINT32  ErrClr:                     1;      /* [28] 1: Clear error status from lp receiver */
    UINT32  LpRecvClr:                  1;      /* [29] 1: Manual clear BTA received bytes */
    UINT32  Reserved4:                  1;      /* [30] Reserved */
    UINT32  CsiBypass:                  1;      /* [31] 1: Enable VIN to VOUT bypass mode */
} AMBA_MIPI_DSI_AUX_CTRL_REG_s;

/*
 * RCT: All Registers
 */
typedef struct {
    volatile AMBA_PLL_CTRL_REG_s                    CorePllCtrl;            /* 0x000(RW): Core PLL integer configurations */
    volatile UINT32                                 CorePllFraction;        /* 0x004(RW): Core PLL fractional configurations */
    volatile AMBA_HDMI_CLK_SELECT_REG_u             HdmiClkSelect;          /* 0x008(RW): HDMI clock select */
    volatile AMBA_PLL_DIVIDER_16BIT_REG_s           Sd0ClkDivider;          /* 0x00C(RW): SD0/SD48 Clock Divider (LSB 17-bit) */
    volatile UINT32                                 Reserved0[5];           /* 0x010-0x020: Reserved */
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
    volatile UINT32                                 Reserved4[3];           /* 0x06C-0x074: Reserved */
    volatile UINT32                                 WdtResetState;          /* 0x078(RW): Watchdog Timer Reset State (Write Protected by 0x260) */
    volatile UINT32                                 Reserved5;              /* 0x07C: Reserved */
    volatile AMBA_PLL_DIVIDER_24BIT_REG_s           DebounceClkDivider;     /* 0x080(RW): SD card/USB VBUS detect debounce timing parameter */
    volatile AMBA_PLL_DIVIDER_24BIT_REG_s           PwmClkDivider;          /* 0x084(RW): Control register for PWM divider */
    volatile UINT32                                 Reserved6;              /* 0x088: Reserved */
    volatile UINT32                                 ClkEnableCtrl;          /* 0x08C(RW): Clock enable control for all clusters */
    volatile UINT32                                 Reserved7[3];           /* 0x090-0x098: Reserved */
    volatile AMBA_PLL_DIVIDER_16BIT_REG_s           AdcClkDivider;          /* 0x09C(RW): ADC clock divider (LSB 17 bits) */
    volatile UINT32                                 Reserved8;              /* 0x0A0: Reserved */
    volatile AMBA_PLL_REF_CLK_SELECT_REG_s          AudioRefClkCtrl0;       /* 0x0A4(RW): Control Audio PLL reference clock */
    volatile AMBA_AUDIO_EXT_CLK_CTRL_REG_s          AudioExtClkCtrl;        /* 0x0A8(RW): Control if using external clock for audio */
    volatile UINT32                                 Reserved9;              /* 0x0AC: Reserved */
    volatile UINT32                                 Reserved10;             /* 0x0B0: Reserved */
    volatile AMBA_PLL_EXTERNAL_CLK_SRC_REG_s        AudioRefClkCtrl1;       /* 0x0B4(RW): Control which external clock used for audio reference clock */
    volatile UINT32                                 Reserved0B8;            /* 0x0B8: Reserved */
    volatile AMBA_SENSOR_CLK_PAD_CTRL_REG_s         SensorClk0PadCtrl;      /* 0x0BC(RW): Sensor Clock Pad is Input or Output */
    volatile AMBA_PLL_CTRL_REG_s                    VideoAPllCtrl;          /* 0x0C0(RW): VideoA PLL integer configurations */
    volatile UINT32                                 VideoAPllFraction;      /* 0x0C4(RW): VideoA PLL fractional configurations */
    volatile AMBA_PLL_DIVIDER_4BIT_REG_s            VideoAPllPrescaler;     /* 0x0C8(RW): VideoA PLL pre-scaler (LSB 8-bit) */
    volatile AMBA_PLL_DIVIDER_16BIT_REG_s           VideoAPllPostscaler;    /* 0x0CC(RW): VideoA PLL post-scaler (LSB 17 bits) */
    volatile AMBA_PLL_EXTERNAL_CLK_SRC_REG_s        VideoARefClkCtrl1;      /* 0x0D0(RW): Control which external clock used for video A reference clock */
    volatile UINT32                                 Reserved11;             /* 0x0D4: Reserved */
    volatile AMBA_PLL_REF_CLK_SELECT_REG_s          VideoARefClkCtrl0;      /* 0x0D8(RW): Control if using external clock for video A */
    volatile UINT32                                 Reserved12[2];          /* 0x0DC-0xE0: Reserved */
    volatile AMBA_PLL_CTRL_REG_s                    IdspPllCtrl;            /* 0x0E4(RW): IDSP PLL integer configurations */
    volatile UINT32                                 IdspPllFraction;        /* 0x0E8(RW): IDSP PLL fractional configurations */
    volatile AMBA_PLL_DIVIDER_24BIT_REG_s           SpiSlaveClkDivider;     /* 0x0EC(RW): Control register for SPI slave divider (LSB 25 bits) */
    volatile UINT32                                 Reserved13[4];          /* 0x0F0-0x0FC: Reserved */

    volatile AMBA_PLL_CTRL2_REG_s                   CorePllCtrl2;           /* 0x100(RW): Core PLL contorl2 register */
    volatile AMBA_PLL_CTRL3_REG_s                   CorePllCtrl3;           /* 0x104(RW): Core PLL contorl3 register */
    volatile AMBA_PLL_CTRL2_REG_s                   IdspPllCtrl2;           /* 0x108(RW): IDSP PLL contorl2 register */
    volatile AMBA_PLL_CTRL3_REG_s                   IdspPllCtrl3;           /* 0x10C(RW): IDSP PLL contorl3 register */
    volatile UINT32                                 Reserved14[3];          /* 0x110-0x118: Reserved */
    volatile AMBA_PLL_CTRL2_REG_s                   SensorPllCtrl2;         /* 0x11C(RW): Sensor PLL contorl2 register */
    volatile AMBA_PLL_CTRL3_REG_s                   SensorPllCtrl3;         /* 0x120(RW): Sensor PLL contorl3 register */
    volatile AMBA_PLL_CTRL2_REG_s                   AudioPllCtrl2;          /* 0x124(RW): Audio PLL contorl2 register */
    volatile UINT32                                 Reserved15;             /* 0x128: Reserved */
    volatile AMBA_PLL_CTRL3_REG_s                   AudioPllCtrl3;          /* 0x12C(RW): Audio PLL contorl3 register */
    volatile UINT32                                 Reserved16[3];          /* 0x130-0x138: Reserved */
    volatile AMBA_PLL_CTRL2_REG_s                   VideoAPllCtrl2;         /* 0x13C(RW): VideoA PLL contorl2 register */
    volatile AMBA_PLL_CTRL3_REG_s                   VideoAPllCtrl3;         /* 0x140(RW): VideoA PLL contorl3 register */
    volatile UINT32                                 Reserved17[12];         /* 0x144-0x170: Reserved */
    volatile UINT32                                 CorePllOutObsv;         /* 0x174(RO): Register to capture PLL output signals (for test/debug) */
    volatile UINT32                                 IdspPllOutObsv;         /* 0x178(RO): Register to capture PLL output signals (for test/debug) */
    volatile UINT32                                 Reserved18;             /* 0x17C: Reserved */
    volatile UINT32                                 SensorPllOutObsv;       /* 0x180(RO): Register to capture PLL output signals (for test/debug) */
    volatile UINT32                                 AudioPllOutObsv;        /* 0x184(RO): Register to capture PLL output signals (for test/debug) */
    volatile UINT32                                 Reserved19;             /* 0x188: Reserved */
    volatile UINT32                                 VideoAPllOutObsv;       /* 0x18C(RO): Register to capture PLL output signals (for test/debug) */
    volatile UINT32                                 Reserved20;             /* 0x190: Reserved */
    volatile UINT32                                 Reserved21;             /* 0x194: Reserved */
    volatile AMBA_ADC_POWER_CTRL_REG_s              AdcPowerCtrl;           /* 0x198(RW): ADC Power Control register */
    volatile AMBA_SPI_CLK_REF_SELECT_REG_s          SpiMasterRefClkSelect;  /* 0x19C(RW): SSI master clock reference selection (LSB 2-bit) */
    volatile AMBA_T2V_CTRL_REG_s                    T2vCtrl;                /* 0x1A0(RW): Control register for T2V */
    volatile AMBA_RAND_NUM_GEN_CTRL_REG_s           RngCtrl;                /* 0x1A4(RW): Random number generator */
    volatile AMBA_PLL_CTRL_REG_s                    HdmiPllCtrl;            /* 0x1A8(RW): HDMI PLL integer configurations */
    volatile UINT32                                 HdmiPllFraction;        /* 0x1AC(RW): HDMI PLL fractional configurations */
    volatile AMBA_PLL_CTRL2_REG_s                   HdmiPllCtrl2;           /* 0x1B0(RW): HDMI PLL contorl2 register */
    volatile AMBA_PLL_CTRL3_REG_s                   HdmiPllCtrl3;           /* 0x1B4(RW): HDMI PLL contorl3 register */
    volatile UINT32                                 HdmiPllOutObsv;         /* 0x1B8(RO): Register to capture PLL output signals (for test/debug) */
    volatile UINT32                                 Reserved22[2];          /* 0x1BC-0x1C0: Reserved */
    volatile AMBA_PWM_CLK_SRC_SELECT_REG_s          PwmClkSrcSelect;        /* 0x1C4(RW): PWM Clock Source Selection */
    volatile AMBA_UART_CLK_SRC_SELECT_REG_s         UartApbClkSrcSelect;    /* 0x1C8(RW): UART APB Clock Source Selection */
    volatile UINT32                                 Reserved23;             /* 0x1CC: Reserved */
    volatile AMBA_ADC_CLK_SRC_SELECT_REG_s          AdcClkSelect;           /* 0x1D0(RW): ADC Clock Source Selection (LSB 2-bit) */
    volatile UINT32                                 Reserved24[3];          /* 0x1D4-0x1DC: Reserved */
    volatile AMBA_PLL_CLK_OBSV_REG_s                ClkObsvCtrl;            /* 0x1E0(RW): Enables observation of PLL outputs on xx_clk_si pin (LSB 5-bit) */
    volatile UINT32                                 Reserved25[5];          /* 0x1E4-0x1F4: Reserved */
    volatile UINT32                                 Reserved26;             /* 0x1F8: Reserved */
    volatile AMBA_IO_CTRL_REG_s                     IoCtrlMisc;             /* 0x1FC(RW): IO Control for all the pads other than 0x200-0x218 */

    volatile UINT32                                 Reserved27;             /* 0x200: Reserved */
    volatile AMBA_IO_CTRL_REG_s                     IoCtrlSmio[4];          /* 0x204-0x210(RW): IO Control for SMIO (without drive strength & pull control) */
    volatile UINT32                                 Reserved28;             /* 0x214: Reserved */
    volatile AMBA_IO_CTRL_REG_s                     IoCtrlSensorInput;      /* 0x218(RW): IO Control for VIN output pins (without drive strength & pull control) */
    volatile AMBA_AHB_MISC_EN_REG_s                 AhbMiscEnable;          /* 0x21C(RW): AHB Misc Enable Control */
    volatile UINT32                                 Reserved29;             /* 0x220: Reserved */
    volatile AMBA_DDR_CLK_DIV_RESET_REG_s           DdrDividerReset;        /* 0x224(RW): Soft reset for DDRIO clock dividers (LSB 1-bit) */
    volatile AMBA_SOFT_RESET1_REG_s                 SoftReset1;             /* 0x228(RW): Soft reset for DDRC/iDSP/VDSP/SMEM (LSB 7-bit) */
    volatile UINT32                                 Reserved30;             /* 0x22C: Reserved */
    volatile AMBA_PLL_DIVIDER_4BIT_REG_s            Vin4ClkDivider;         /* 0x230(RW): VIN4 clock divider */
    volatile UINT32                                 Reserved31[8];          /* 0x234-0x250: Reserved */
    volatile UINT32                                 RctTimer0Counter;       /* 0x254(RO): Free running timer/counter on reference clock */
    volatile UINT32                                 RctTimer0Ctrl;          /* 0x258(RW): RCT timer control (LSB 2-bit) */
    volatile AMBA_PLL_LOCK_STATUS_REG_s             RawLockStatus;          /* 0x25C(RO): Hold the raw PLL lock status */
    volatile UINT32                                 UnlockWdtClear;         /* 0x260(RW): Make the write to WdtResetState work */
    volatile AMBA_PLL_CTRL_REG_s                    CortexPllCtrl;          /* 0x264(RW): Cortex PLL integer configurations */
    volatile UINT32                                 CortexPllFraction;      /* 0x268(RW): Cortex PLL fractional configurations */
    volatile AMBA_PLL_CTRL2_REG_s                   CortexPllCtrl2;         /* 0x26C(RW): Cortex PLL contorl2 register */
    volatile AMBA_PLL_CTRL3_REG_s                   CortexPllCtrl3;         /* 0x270(RW): Cortex PLL contorl3 register */
    volatile UINT32                                 CortexPllOutObsv;       /* 0x274(RO): Cortex PLL observation register to capture PLL output signals (for test/debug) */
    volatile UINT32                                 Reserved32[12];         /* 0x278-0x2A4: Reserved */
    volatile AMBA_GTX_CLK_DIVIDER_REG_s             GtxClkDivider;          /* 0x2A8(RW): Clock divider of gclk_gtx */
    volatile AMBA_GTX_CLK_DIVIDER_SRC_REG_s         GtxClkDividerSrc;       /* 0x2AC(RW): Clock divider source of gclk_gtx */
    volatile AMBA_GTX_CLK_SRC_REG_s                 GtxClkSelect;           /* 0x2B0(RW): Clock source select of gclk_gtx */
    volatile UINT32                                 Reserved33[9];          /* 0x2B4-0x2D4: Reserved */
    volatile AMBA_CLK_DISABLE_REG_s                 ClkDisableCtrl;         /* 0x2D8(RW): Clocks disable control */
    volatile AMBA_PLL_CLK_SRC_SELECT_REG_s          CoreClkSrcSelect;       /* 0x2DC(RW): Clock source (LSB 2-bit), software shouldn't touch  */
    volatile UINT32                                 Reserved34;             /* 0x2E0: Reserved */
    volatile AMBA_PLL_CLK_SRC_SELECT_REG_s          DdrClkSrcSelect;        /* 0x2E4(RW): Clock source (LSB 2-bit), software shouldn't touch  */
    volatile UINT32                                 Reserved35;             /* 0x2E8: Reserved */
    volatile AMBA_PLL_DIVIDER_4BIT_REG_s            VoutBClkDivider;        /* 0x2EC(RW): VoutB clock divider */
    volatile UINT32                                 Reserved36[4];          /* 0x2F0-0x2FC: Reserved */

    volatile UINT32                                 OtpConfigValue[2];      /* 0x300-0x304(RO): OTP data for configuration */
    volatile UINT32                                 ClkDivReset;            /* 0x308(RW): Soft reset for synchronous dividers */
    volatile UINT32                                 Reserved37[2];          /* 0x30C-0x310: Reserved */
    volatile UINT32                                 Gpio0DriveStrength[2];  /* 0x314-0x318(RW): GPIO Pin 0-31 Driving Strength */
    volatile UINT32                                 Gpio1DriveStrength[2];  /* 0x31C-0x320(RW): GPIO Pin 32-63 Driving Strength */
    volatile UINT32                                 Gpio2DriveStrength[2];  /* 0x324-0x328(RW): GPIO Pin 64-95 Driving Strength */
    volatile UINT32                                 Gpio3DriveStrength[2];  /* 0x32C-0x330(RW): GPIO Pin 96-127 Driving Strength */
    volatile UINT32                                 Reserved38[49];         /* 0x334-0x3F4: Reserved */
    volatile AMBA_PLL_SCALER_CTRL_REG_s             ScalerBypass;           /* 0x3F8(RW): Bypass Clock Dividers */
    volatile AMBA_PLL_SCALER_CTRL_REG_s             ScalerReset;            /* 0x3FC(RW): Reset Clock Dividers */

    volatile UINT32                                 Reserved39;             /* 0x400: Reserved */
    volatile AMBA_PLL_CTRL_REG_s                    HdmiLcPllCtrl;          /* 0x404(RW): HDMI LC PLL integer configurations */
    volatile UINT32                                 HdmiLcPllFraction;      /* 0x408(RW): HDMI LC PLL fractional configurations */
    volatile AMBA_PLL_CTRL2_REG_s                   HdmiLcPllCtrl2;         /* 0x40C(RW): HDMI LC PLL contorl2 register */
    volatile AMBA_PLL_CTRL3_REG_s                   HdmiLcPllCtrl3;         /* 0x410(RW): HDMI LC PLL contorl3 register */
    volatile UINT32                                 HdmiLcPllOutObsv;       /* 0x414(RO): Register to capture PLL output signals */
    volatile UINT32                                 HdmiLcPllCtrl4;         /* 0x418(RW): HDMI LC PLL contorl4 register */
    volatile UINT32                                 HdmiLcPllCregObsv;      /* 0x41C(RW): HDMI LC PLL creg obsv register */
    volatile UINT32                                 Reserved40[4];          /* 0x420-0x42C: Reserved */
    volatile AMBA_PLL_DIVIDER_16BIT_REG_s           Sd1ClkDivider;          /* 0x430(RW): SD1/SDIO0 Clock Divider (LSB 17-bit) */
    volatile AMBA_PLL_DIVIDER_16BIT_REG_s           Sd2ClkDivider;          /* 0x434(RW): SD2/SDIO1 Clock Divider (LSB 17-bit) */
    volatile UINT32                                 Gpio4DriveStrength[2];  /* 0x438-0x43C(RW): GPIO Pin 128-159 Driving Strength */
    volatile UINT32                                 Reserved41[4];          /* 0x440-0x44C: Reserved */
    volatile UINT32                                 MiscDriveStrength[2];   /* 0x450-0x454(RW): Misc Pin (clk_au[0], usb0_detect_vbus[1], usb1_detect_vbus[2], clk_si[3], clk_si1[4], pcie_perst_n[5], pcie_clkreq_n[6]) Driving Strength */
    volatile UINT32                                 Reserved42[11];         /* 0x458-0x480: Reserved */
    volatile AMBA_MUX_LVDS_SPCLK_SELECT_REG_s       MuxedLvdsSpclk;         /* 0x484(RW): External Reference Clock Selection for muxed_lvds_spclk */
    volatile AMBA_MUX_MIPI_RX_BYTE_CLK_SELECT_REG_s MuxedMipiRxByteClkHS;   /* 0x488(RW): External Reference Clock Selection for muxed_RxByteClkHS */
    volatile UINT32                                 SlvsEcCkRecoverSel;     /* 0x48C(RW): Select ck_recovered from slvsec_phy for observation */
    volatile UINT32                                 Reserved43;             /* 0x490: Reserved */
    volatile UINT32                                 RctTimer1Counter;       /* 0x494(RO): Free running timer/counter on reference clock */
    volatile UINT32                                 RctTimer1Ctrl;          /* 0x498(RW): RCT timer control (LSB 2-bit) */
    volatile UINT32                                 Reserved44[4];          /* 0x49C-0x4A8: Reserved */
    volatile AMBA_PLL_CTRL_REG_s                    SdPllCtrl;              /* 0x4AC(RW): SD PLL integer configurations */
    volatile UINT32                                 SdPllFraction;          /* 0x4B0(RW): SD PLL fractional configurations */
    volatile AMBA_PLL_CTRL2_REG_s                   SdPllCtrl2;             /* 0x4B4(RW): SD PLL contorl2 register */
    volatile AMBA_PLL_CTRL3_REG_s                   SdPllCtrl3;             /* 0x4B8(RW): SD PLL contorl3 register */
    volatile UINT32                                 SdPllOutObsv;           /* 0x4BC(RO): Register to capture PLL output signals (for test/debug) */
    volatile UINT32                                 Reserved45[4];          /* 0x4C0-0xC44: Reserved */
    volatile AMBA_SD_CLK_SRC_SELECT_REG_s           SdClkSrcSelect;         /* 0x4D0(RW): SD Clock Source Selection */
    volatile UINT32                                 Reserved46[6];          /* 0x4D4-0x4E8: Reserved */
    volatile AMBA_SPI_CLK_REF_SELECT_REG_s          SpiSlaveRefClkSelect;   /* 0x4EC(RW): SPI slave clock reference selection (LSB 1-bit) */
    volatile UINT32                                 Reserved47[5];          /* 0x4F0-0x500: Reserved */

    volatile AMBA_PLL_CTRL_REG_s                    SlvsEcPllCtrl;          /* 0x504(RW): SLVSEC PLL integer configurations */
    volatile UINT32                                 SlvsEcPllFraction;      /* 0x508(RW): SLVSEC PLL fractional configurations */
    volatile AMBA_PLL_CTRL2_REG_s                   SlvsEcPllCtrl2;         /* 0x50C(RW): SLVSEC PLL contorl2 register */
    volatile AMBA_PLL_CTRL3_REG_s                   SlvsEcPllCtrl3;         /* 0x510(RW): SLVSEC PLL contorl3 register */
    volatile UINT32                                 SlvsEcPllOutObsv;       /* 0x514(RO): Register to capture PLL output signals */
    volatile AMBA_PLL_DIVIDER_24BIT_REG_s           SpiNorClkDivider;       /* 0x518(RW): Control register for SPI-NOR divider (LSB 25 bits) */
    volatile AMBA_SPI_NOR_CLK_REF_SELECT_REG_s      SpiNorRefClkSelect;     /* 0x51C(RW): SPI-NOR clock reference selection (LSB 2-bit) */
    volatile AMBA_PLL_CTRL_REG_s                    EnetPllCtrl;            /* 0x520(RW): Ethernet PLL integer configurations */
    volatile UINT32                                 EnetPllFraction;        /* 0x524(RW): Ethernet PLL fractional configurations */
    volatile AMBA_PLL_CTRL2_REG_s                   EnetPllCtrl2;           /* 0x528(RW): Ethernet PLL contorl2 register */
    volatile AMBA_PLL_CTRL3_REG_s                   EnetPllCtrl3;           /* 0x52C(RW): Ethernet PLL contorl3 register */
    volatile UINT32                                 EnetPllOutObsv;         /* 0x530(RO): Register to capture PLL output signals (for test/debug) */
    volatile AMBA_PLL_DIVIDER_4BIT_REG_s            EnetPllPostscaler;      /* 0x534(RW): Ethernet PLL post-scaler (LSB 8-bit) */
    volatile UINT32                                 OtpChipIdValue[2];      /* 0x538-0x53C(RO): OTP data for foundry to track the wafer information */
    volatile UINT32                                 Reserved48[11];         /* 0x540-0x568: Reserved */
    volatile AMBA_CAN_CLK_REF_SELECT_REG_s          CanClkSel;              /* 0x56c: Souce clock select for gclk_can */
    volatile AMBA_CAN_CLK_DIVIDER_REG_s             CanClkDiv;              /* 0x570: Souce clock select for gclk_can */
    volatile AMBA_PLL_DIVIDER_4BIT_REG_s            Vin0ClkDivider;         /* 0x574(RW): VIN0 scaler */
    volatile AMBA_PLL_DIVIDER_4BIT_REG_s            Sensor2Postscaler;      /* 0x578(RW):  Control register for SENSOR2 post-scaler   */
    volatile UINT32                                 Reserved49[2];          /* 0x57C-0x580: Reserved */
    volatile UINT32                                 MipiDsi0Ctrl0;          /* 0x584(RW): Vout0/Video_A mipi dsi interface control */
    volatile UINT32                                 MipiDsi0Ctrl1;          /* 0x588(RW): Vout0/Video_A mipi dsi interface control */
    volatile UINT32                                 MipiDsi0Ctrl2;          /* 0x58C(RW): Vout0/Video_A mipi dsi interface control */
    volatile AMBA_MIPI_DSI_CTRL3_REG_s              MipiDsi0Ctrl3;          /* 0x590(RW): Vout0/Video_A mipi dsi interface control */
    volatile AMBA_MIPI_DSI_CTRL4_REG_s              MipiDsi0Ctrl4;          /* 0x594(RW): Vout0/Video_A mipi dsi interface control */
    volatile UINT32                                 MipiDsi0Ctrl5;          /* 0x598(RW): Vout0/Video_A mipi dsi interface control */
    volatile AMBA_MIPI_DSI_AUX_CTRL_REG_s           MipiDsi0AuxCtrl;        /* 0x59C(RW): Vout0/Video_A mipi dsi interface aux control */
    volatile UINT32                                 MipiDsi0Obsv[3];        /* 0x5A0-0x5A8(RW): Vout0/Video_A mipi dsi interface observation */
    volatile UINT32                                 Reserved50[2];          /* 0x5AC-0x5B0: Reserved */
    volatile UINT32                                 MipiDsi1Ctrl0;          /* 0x5B4(RW): Vout1/Video_B mipi dsi interface control */
    volatile UINT32                                 MipiDsi1Ctrl1;          /* 0x5B8(RW): Vout1/Video_B mipi dsi interface control */
    volatile UINT32                                 MipiDsi1Ctrl2;          /* 0x5BC(RW): Vout1/Video_B mipi dsi interface control */
    volatile AMBA_MIPI_DSI_CTRL3_REG_s              MipiDsi1Ctrl3;          /* 0x5C0(RW): Vout1/Video_B mipi dsi interface control */
    volatile AMBA_MIPI_DSI_CTRL4_REG_s              MipiDsi1Ctrl4;          /* 0x5C4(RW): Vout1/Video_B mipi dsi interface control */
    volatile UINT32                                 MipiDsi1Ctrl5;          /* 0x5C8(RW): Vout1/Video_B mipi dsi interface control */
    volatile AMBA_MIPI_DSI_AUX_CTRL_REG_s           MipiDsi1AuxCtrl;        /* 0x5CC(RW): Vout1/Video_B mipi dsi interface aux control */
    volatile UINT32                                 MipiDsi1Obsv[3];        /* 0x5D0-0x5D8(RW): Vout1/Video_B mipi dsi interface observation */
    volatile UINT32                                 Reserved51[9];          /* 0x5DC-0x5FC: Reserved */

    volatile AMBA_PLL_DIVIDER_4BIT_REG_s            Audio2Postscaler;       /* 0x600(RW): Audio2 post-scaler (LSB 8-bit) */
    volatile AMBA_PLL_DIVIDER_4BIT_REG_s            Audio3Postscaler;       /* 0x604(RW): Audio3 post-scaler (LSB 8-bit) */
    volatile UINT32                                 Audio2UseExtClk;        /* 0x608(RW): Control if using external clock for audio */
    volatile UINT32                                 Reserved52[24];         /* 0x60C-0x668: Reserved */
    volatile UINT32                                 OtpAmbaIdValue;         /* 0x66C(RO): OTP data for chip ID */
    volatile UINT32                                 Reserved53[6];          /* 0x670-0x684: Reserved */
    volatile AMBA_DAC_CTRL_REG_s                    DacCtrl;                /* 0x688(RW): DAC control register */
    volatile UINT32                                 Reserved54;             /* 0x68C: Reserved */
    volatile UINT32                                 DftObsv;                /* 0x690(RO): Observation bus for dft signal */
    volatile UINT32                                 Reserved55[6];          /* 0x694-0x6A8: Reserved */
    volatile UINT32                                 EmaSramsOnepCtrl;       /* 0x6AC(RW): Control register for EMA settings of rf1 & ra1 srams  */
    volatile UINT32                                 EmaSramsTwopCtrl;       /* 0x6B0(RW): Control register for EMA settings of rf2 & rd2 srams  */
    volatile UINT32                                 Reserved56[12];         /* 0x6B4-0x6E0: Reserved */
    volatile AMBA_PLL_CTRL_REG_s                    VisionPllCtrl;          /* 0x6E4(RW): Vision PLL integer configurations */
    volatile UINT32                                 VisionPllFraction;      /* 0x6E8(RW): Vision PLL fractional configurations */
    volatile AMBA_PLL_CTRL2_REG_s                   VisionPllCtrl2;         /* 0x6EC(RW): Vision PLL contorl2 register */
    volatile AMBA_PLL_CTRL3_REG_s                   VisionPllCtrl3;         /* 0x6F0(RW): Vision PLL contorl3 register */
    volatile UINT32                                 VisionPllOutObsv;       /* 0x6F4(RO): Register to capture PLL output signals (for test/debug) */
    volatile AMBA_PLL_CTRL_REG_s                    NandPllCtrl;            /* 0x6F8(RW): Nand PLL integer configurations */
    volatile UINT32                                 NandPllFraction;        /* 0x6FC(RW): Nand PLL fractional configurations */

    volatile AMBA_PLL_CTRL2_REG_s                   NandPllCtrl2;           /* 0x700(RW): Nand PLL contorl2 register */
    volatile AMBA_PLL_CTRL3_REG_s                   NandPllCtrl3;           /* 0x704(RW): Nand PLL contorl3 register */
    volatile UINT32                                 NandPllOutObsv;         /* 0x708(RO): Register to capture PLL output signals (for test/debug) */
    volatile AMBA_RING_OSC_CTRL_REG_s               RingOscCtrl;            /* 0x70C(RW): Ring oscillator ctrl */
    volatile AMBA_RING_OSC_OBSV_REG_s               RingOscObsv;            /* 0x710(RO): Ring oscillator observation */
    volatile AMBA_PLL_DIVIDER_24BIT_REG_s           Uart0ClkDivider;        /* 0x714(RW): Control register for UART_0 divider (LSB 24-bit) */
    volatile AMBA_PLL_DIVIDER_24BIT_REG_s           Uart1ClkDivider;        /* 0x718(RW): Control register for UART_1 divider (LSB 24-bit) */
    volatile AMBA_PLL_DIVIDER_24BIT_REG_s           Uart2ClkDivider;        /* 0x71C(RW): Control register for UART_2 divider (LSB 24-bit) */
    volatile AMBA_PLL_DIVIDER_24BIT_REG_s           Uart3ClkDivider;        /* 0x720(RW): Control register for UART_3 divider (LSB 24-bit) */
    volatile UINT32                                 Reserved57[2];          /* 0x724-0x728: Reserved */
    volatile AMBA_UART_CLK_SRC_SELECT_REG_s         Uart0ClkSrcSelect;      /* 0x72C(RW): UART_0 Clock Source Selection */
    volatile AMBA_UART_CLK_SRC_SELECT_REG_s         Uart1ClkSrcSelect;      /* 0x730(RW): UART_1 Clock Source Selection */
    volatile AMBA_UART_CLK_SRC_SELECT_REG_s         Uart2ClkSrcSelect;      /* 0x734(RW): UART_2 Clock Source Selection */
    volatile AMBA_UART_CLK_SRC_SELECT_REG_s         Uart3ClkSrcSelect;      /* 0x738(RW): UART_3 Clock Source Selection */
    volatile UINT32                                 Reserved58[2];          /* 0x73C-0x740: Reserved */
    volatile AMBA_VOUT_CLK_SELECT_REG_s             VoutClkSelect;          /* 0x744(RW): VOUT clock selection register */
    volatile AMBA_VIN_CLK_SELECT_REG_s              VinClkSelect;           /* 0x748(RW): VIN clock selection register */

    volatile AMBA_PLL_CTRL_REG_s                    Sensor2PllCtrl;         /* 0x74C(RW): Sensor2 PLL integer configurations */
    volatile AMBA_PLL_CTRL2_REG_s                   Sensor2PllCtrl2;        /* 0x750(RW): Sensor2 PLL contorl2 register */
    volatile AMBA_PLL_CTRL3_REG_s                   Sensor2PllCtrl3;        /* 0x754(RW): Sensor2 PLL contorl3 register */
    volatile UINT32                                 Sensor2PllFraction;     /* 0x758(RW): Sensor2 PLL fractional configurations */
    volatile UINT32                                 Sensor2PllOutObsv;      /* 0x75C(RO): Register to capture PLL output signals (for test/debug) */
    volatile UINT32                                 Reserved59[4];          /* 0x760-0x76c: Reserved */
    volatile UINT32                                 OtpT2vCalibDataOut;     /* 0x770(RO): Obersvation register for OTP T2V Data out  */
    volatile UINT32                                 OtpWLockDataOut;        /* 0x774(RO): OTP Write Lock Data Out */
    volatile UINT32                                 Reserved60[2];          /* 0x778-0x77C: Reserved */
    volatile UINT32                                 Scratchpad[5];          /* 0x780-0x790(RW): General registers to retain data through soft-reset */
    volatile AMBA_PLL_DIVIDER_4BIT_REG_s            SysCntClkDivider;       /* 0x794(RW): System counter scaler */
    volatile AMBA_VIN1_CLK_DIVIDER_SRC_REG_s        Vin1ClkDividerSrc;      /* 0x798(RW): Clock divider source of gclk_so_pip */
    volatile UINT32                                 Reserved61[9];          /* 0x79C-0x7BC: Reserved */
    volatile UINT32                                 TcCtrl;                 /* 0x7C0(RW): Control register for "tc" IP which provides calibration codes to hdmi_phy & serdes */
    volatile UINT32                                 TcObsv;                 /* 0x7C4(RW): Observe register for "tc" IP */
    volatile UINT32                                 Reserved62[4];          /* 0x7C8-0x7D4: Reserved */
    volatile AMBA_PLL_CTRL_REG_s                    IdspvPllCtrl;           /* 0x7D8(RW): IDSPV PLL integer configurations */
    volatile UINT32                                 IdspvPllFraction;       /* 0x7DC(RW): IDSPV PLL fractional configurations */
    volatile AMBA_PLL_CTRL2_REG_s                   IdspvPllCtrl2;          /* 0x7E0(RW): IDSPV PLL contorl2 register */
    volatile AMBA_PLL_CTRL3_REG_s                   IdspvPllCtrl3;          /* 0x7E4(RW): IDSPV PLL contorl3 register */
    volatile UINT32                                 IdspvPllOutObsv;        /* 0x7E8(RO): Register to capture PLL output signals (for test/debug) */
    volatile UINT32                                 RngObsv;                /* 0x7EC(RO): Observe register for RNG */
    volatile AMBA_HDMI_PHY_CTRL_REG_u               HdmiPhyCtrl;            /* 0x7F0(RW): HDMI PHY control register */
    volatile UINT32                                 HdmiPhyCtrl2;           /* 0x7F4(RW): HDMI PHY control register 2 */
    volatile UINT32                                 Reserved63;             /* 0x7F8: Reserved */
    volatile UINT32                                 DdrHostCtrl;            /* 0x7FC(RW): Soft resets for ddrhost debug port */

    volatile UINT32                                 Reserved64[5];          /* 0x800-0x810: Reserved */
    volatile AMBA_PLL_CTRL_REG_s                    DsuPllCtrl;             /* 0x814(RW): DSU PLL integer configurations */
    volatile UINT32                                 DsuPllFraction;         /* 0x818(RW): DSU PLL fractional configurations */
    volatile AMBA_PLL_CTRL2_REG_s                   DsuPllCtrl2;            /* 0x81C(RW): DSU PLL contorl2 register */
    volatile AMBA_PLL_CTRL3_REG_s                   DsuPllCtrl3;            /* 0x820(RW): DSU PLL contorl3 register */
    volatile UINT32                                 DsuPllOutObsv;          /* 0x824(RO): Register to capture PLL output signals (for test/debug) */
} AMBA_RCT_REG_s;

/*
 * MIPI/VIN PHY Registers
 */

typedef struct {
    UINT32  RxUlpsActiveNotClk0:        1;      /* [0] */
    UINT32  RxUlpsActiveNotClk1:        1;      /* [1] */
    UINT32  Reserved0:                  2;      /* [3:2] Reserved */
    UINT32  RxClkActiveHS0:             1;      /* [4] */
    UINT32  RxClkActiveHS1:             1;      /* [5] */
    UINT32  Reserved1:                  2;      /* [7:6] Reserved */
    UINT32  RxClkStopstate0:            1;      /* [8] */
    UINT32  RxClkStopstate1:            1;      /* [9] */
    UINT32  Reserved2:                  1;      /* [10] Reserved */
    UINT32  FineTuneObs:                5;      /* [15:11] */
    UINT32  CoarseTuneObs:              5;      /* [20:16] */
    UINT32  MinusDlyFlag:               1;      /* [21] */
    UINT32  AddDlyFlag:                 1;      /* [22] */
    UINT32  CoarseFinished:             1;      /* [23] */
    UINT32  SkewState:                  3;      /* [26:24] */
    UINT32  Reserved3:                  5;      /* [31:27] Reserved */
} AMBA_DPHY_OBSERVE0_REG_s;

typedef struct {
    UINT32  ErrControl:                 8;      /* [7:0] ErrControl[7:0] */
    UINT32  RxValidHS:                  8;      /* [15:8] RxValidHS[7:0] */
    UINT32  ErrSotHS:                   8;      /* [23:16] ErrSotHS[7:0] */
    UINT32  ErrSotSyncHS:               8;      /* [31:24] ErrSotSyncHS[7:0]*/
} AMBA_DPHY_OBSERVE1_REG_s;

typedef struct {
    UINT32  RxActiveHS:                 8;      /* [7:0] RxActiveHS[7:0] */
    UINT32  RxStopState:                8;      /* [15:8] RxStopState[7:0] */
    UINT32  RxSyncHS:                   8;      /* [23:16] RxSyncHS[7:0] */
    UINT32  RxUlpsActiveNot:            8;      /* [31:24] RxUlpsActiveNot[7:0] */
} AMBA_DPHY_OBSERVE2_REG_s;

typedef struct {
    UINT32  RxStateData:                3;      /* [2:0] Rx_state_data_o, selected data lane's rx_state */
    UINT32  Reserved0:                  1;      /* [3] Reserved */
    UINT32  RxStateClk:                 4;      /* [7:4] Rx_state_clk_o, selected clock lane's rx_state */
    UINT32  Reserved1:                  24;     /* [31:8] Reserved */
} AMBA_DPHY_OBSERVE4_REG_s;

typedef struct {
    UINT32  DlyMatchMode:               2;      /* [1:0] dly_match_mode */
    UINT32  HystCompSw:                 1;      /* [2] */
    UINT32  Reserved0:                  1;      /* [3] Reserved */
    UINT32  DphyRst0:                   1;      /* [4] reset mipi dphy digital part */
    UINT32  DphyRst1:                   1;      /* [5] reset mipi dphy digital part */
    UINT32  Reserved1:                  2;      /* [7:6] Reserved */
    UINT32  RstDphyAfe:                 2;      /* [9:8] reset mipi dphy analog front end */
    UINT32  Reserved2:                  2;      /* [11:10] Reserved */
    UINT32  NoVbn:                      1;      /* [12] disable NMOS bias voltage */
    UINT32  Reserved3:                  3;      /* [15:13] Reserved */
    UINT32  BitMode0:                   2;      /* [17:16] bit_mode0[1:0] */
    UINT32  BitMode1:                   2;      /* [19:18] bit_mode1[1:0] */
    UINT32  Reserved4:                  4;      /* [23:20] Reserved */
    UINT32  ForceR100Off:               2;      /* [25:24] force_r100_off (for clock lanes) */
    UINT32  Reserved5:                  2;      /* [27:26] Reserved */
    UINT32  LvdsResistanceSelect:       4;      /* [31:28] lvds_rsel */
} AMBA_DPHY_CTRL0_REG_s;

typedef struct {
    UINT32  LvdsIbCtrl:                 2;      /* [1:0] lvds_ib_ctrl (rx frontend current control) */
    UINT32  PowerDownLvds:              1;      /* [2] lvds_pd (pd rx frontend 'mipi_lvds_comp') */
    UINT32  Reserved0:                  21;     /* [23:3] Reserved */
    UINT32  DataR100Off:                8;      /* [31:24] data_r100_off[7:0] */
} AMBA_DPHY_CTRL1_REG_s;

typedef struct {
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
    UINT32  Reserved1:                  2;      /* [19:18] Reserved */
    UINT32  ForceMipiHsGp0:             1;      /* [20] force_mipi_hs0, force MIPI Rx data channel to go into HS state */
    UINT32  ForceMipiHsGp1:             1;      /* [21] force_mipi_hs1, force MIPI Rx data channel to go into HS state */
    UINT32  Reserved2:                  10;     /* [31:22] Reserved */
} AMBA_DPHY_CTRL2_REG_s;

typedef struct {
    UINT32  LvCmosMode:                 8;      /* [7:0] LvCmos Mode */
    UINT32  Reserved0:                  8;      /* [15:8] Reserved */
    UINT32  LvCmosSpClk:                2;      /* [17:16] LvCmos_mode_spclk */
    UINT32  Reserved1:                  2;      /* [19:18] Reserved */
    UINT32  MipiMode:                   2;      /* [21:20] mipi_mode */
    UINT32  Reserved2:                  10;     /* [31:22] Reserved */
} AMBA_DPHY_CTRL3_REG_s;

typedef struct {
    UINT32  HsTermCtrl0:                5;      /* [4:0] hs_term_ctrl */
    UINT32  Reserved0:                  3;      /* [7:5] Reserved */
    UINT32  HsSettleCtrl0:              6;      /* [13:8] hs_settle_ctrl */
    UINT32  Reserved1:                  2;      /* [15:14] Reserved */
    UINT32  InitRxCtrl0:                7;      /* [22:16] init_rx_ctrl */
    UINT32  Reserved2:                  1;      /* [23] Reserved */
    UINT32  ClkMissCtrl0:               5;      /* [28:24] clk_miss_ctrl */
    UINT32  Reserved3:                  3;      /* [31:29] Reserved */
} AMBA_DPHY_CTRL4_REG_s;

typedef struct {
    UINT32  ClkSettleCtrl0:             6;      /* [5:0] hs_settle_ctrl */
    UINT32  Reserved0:                  2;      /* [7:6] Reserved */
    UINT32  ClkTermCtrl0:               5;      /* [12:8] clk_term_ctrl */
    UINT32  Reserved1:                  19;     /* [31:13] Reserved */
} AMBA_DPHY_CTRL5_REG_s;

typedef struct {
    UINT32  RxStateDataSelect:          4;      /* [3:0] rx_state_data_sel */
    UINT32  RxStateClkSelect:           2;      /* [5:4] rx_state_clk_sel */
    UINT32  Reserved0:                  2;      /* [7:6] Reserved */
    UINT32  DataRctDebSelect:           4;      /* [11:8] data_rct_deb_sel */
    UINT32  RstnRctDeb:                 1;      /* [12] rst_n_rct_deb */
    UINT32  Reserved1:                  19;     /* [31:13] Reserved */
} AMBA_DPHY_CTRL6_REG_s;

typedef struct {
    UINT32  HsTermCtrl1:                5;      /* [4:0] hs_term_ctrl */
    UINT32  Reserved0:                  3;      /* [7:5] Reserved */
    UINT32  HsSettleCtrl1:              6;      /* [13:8] hs_settle_ctrl */
    UINT32  Reserved1:                  2;      /* [15:14] Reserved */
    UINT32  InitRxCtrl1:                7;      /* [22:16] init_rx_ctrl */
    UINT32  Reserved2:                  1;      /* [23] Reserved */
    UINT32  ClkMissCtrl1:               5;      /* [28:24] clk_miss_ctrl */
    UINT32  Reserved3:                  3;      /* [31:29] Reserved */
} AMBA_DPHY_CTRL7_REG_s;

typedef struct {
    UINT32  ClkSettleCtrl1:             6;      /* [5:0] hs_settle_ctrl */
    UINT32  Reserved0:                  2;      /* [7:6] Reserved */
    UINT32  ClkTermCtrl1:               5;      /* [12:8] clk_term_ctrl */
    UINT32  Reserved1:                  19;     /* [31:13] Reserved */
} AMBA_DPHY_CTRL8_REG_s;

typedef struct {
    UINT32  ForceSkewCal:               8;      /* [7:0] force_skew_cal, force skew calibration state machine to start ignoring syncode */
    UINT32  Reserved0:                  4;      /* [11:8] Reserved */
    UINT32  SkewObsSel:                 4;      /* [15:12] */
    UINT32  SkewCalPause:               1;      /* [16] Stop the update of delayline coefficients */
    UINT32  EnDlyManual:                1;      /* [17] EnDlyManual, Enable manually control of delay line coefficients */
    UINT32  Reserved1:                  1;      /* [18] Reserved */
    UINT32  SkewCalTuneSel:             3;      /* [21:19] skew_cal_tune_sel, channel selection of data lane's skew calibration coefficient */
    UINT32  SkewCalExpire:              7;      /* [28:22] skew_cal_expire, Expire time for skew cal sync state */
    UINT32  TimeWindow:                 2;      /* [30:29] time_window, set the timing window of skew calibration */
    UINT32  EnCal:                      1;      /* [31] EnCal, enable skew calibration */
} AMBA_DPHY_CTRL9_REG_s;

typedef struct {
    UINT32  FineTuneInCh1:              5;      /* [4:0] Set fine_tune_ch1 in manual mode */
    UINT32  Reserved0:                  3;      /* [7:5] Reserved */
    UINT32  CoarseTuneInCh1:            5;      /* [12:8] Set fine_tune_ch1 in manual mode */
    UINT32  Reserved1:                  3;      /* [15:13] Reserved */
    UINT32  FineTuneInCh0:              5;      /* [20:16] Set fine_tune_ch0 in manual mode */
    UINT32  Reserved2:                  3;      /* [23:21] Reserved */
    UINT32  CoarseTuneInCh0:            5;      /* [28:24] Set fine_tune_ch0 in manual mode */
    UINT32  Reserved3:                  3;      /* [31:29] Reserved */
} AMBA_DPHY_CTRL10_REG_s;

typedef struct {
    UINT32  FineTuneInCh3:              5;      /* [4:0] Set fine_tune_ch3 in manual mode */
    UINT32  Reserved0:                  3;      /* [7:5] Reserved */
    UINT32  CoarseTuneInCh3:            5;      /* [12:8] Set fine_tune_ch3 in manual mode */
    UINT32  Reserved1:                  3;      /* [15:13] Reserved */
    UINT32  FineTuneInCh2:              5;      /* [20:16] Set fine_tune_ch2 in manual mode */
    UINT32  Reserved2:                  3;      /* [23:21] Reserved */
    UINT32  CoarseTuneInCh2:            5;      /* [28:24] Set fine_tune_ch2 in manual mode */
    UINT32  Reserved3:                  3;      /* [31:29] Reserved */
} AMBA_DPHY_CTRL11_REG_s;

typedef struct {
    UINT32  FineTuneInCh5:              5;      /* [4:0] Set fine_tune_ch5 in manual mode */
    UINT32  Reserved0:                  3;      /* [7:5] Reserved */
    UINT32  CoarseTuneInCh5:            5;      /* [12:8] Set fine_tune_ch5 in manual mode */
    UINT32  Reserved1:                  3;      /* [15:13] Reserved */
    UINT32  FineTuneInCh4:              5;      /* [20:16] Set fine_tune_ch4 in manual mode */
    UINT32  Reserved2:                  3;      /* [23:21] Reserved */
    UINT32  CoarseTuneInCh4:            5;      /* [28:24] Set fine_tune_ch4 in manual mode */
    UINT32  Reserved3:                  3;      /* [31:29] Reserved */
} AMBA_DPHY_CTRL12_REG_s;

typedef struct {
    UINT32  FineTuneInCh7:              5;      /* [4:0] Set fine_tune_ch7 in manual mode */
    UINT32  Reserved0:                  3;      /* [7:5] Reserved */
    UINT32  CoarseTuneInCh7:            5;      /* [12:8] Set fine_tune_ch7 in manual mode */
    UINT32  Reserved1:                  3;      /* [15:13] Reserved */
    UINT32  FineTuneInCh6:              5;      /* [20:16] Set fine_tune_ch6 in manual mode */
    UINT32  Reserved2:                  3;      /* [23:21] Reserved */
    UINT32  CoarseTuneInCh6:            5;      /* [28:24] Set fine_tune_ch6 in manual mode */
    UINT32  Reserved3:                  3;      /* [31:29] Reserved */
} AMBA_DPHY_CTRL13_REG_s;

typedef struct {
    UINT32  Reserved0:                  27;     /* [26:0] Reserved */
    UINT32  LoopbackOutSel:             3;      /* [29:27] dphy_lb_outsel */
    UINT32  LoopbackEn:                 1;      /* [30] dphy_lb_en */
    UINT32  LoopbackClr:                1;      /* [31] dphy_lb_clr  */
} AMBA_DPHY_CTRL15_REG_s;

typedef struct {
    UINT32  Reserved0:                  24;     /* [23:0] Reserved */
    UINT32  CfgRefClkSelect:            1;      /* [24] cfg_ref_clk_sel */
    UINT32  CfgMipiMode:                1;      /* [25] cfg_mipi_mode */
    UINT32  CfgTestPattern:             2;      /* [27:26] cfg_test_pattern */
    UINT32  Reserved1:                  3;      /* [30:28] Reserved */
    UINT32  Rst_l:                      1;      /* [31] rst_l */
} AMBA_PHY_BIST_CTRL11_REG_s;

typedef struct {
    UINT32  CtleOutComp:                1;     /* [0] */
    UINT32  VcmComp:                    1;     /* [1] */
    UINT32  LsPoverIn:                  1;     /* [2] */
    UINT32  Fdcc:                       1;     /* [3] */
    UINT32  Fdccsmpl:                   1;     /* [4] */
    UINT32  DctrlPhaselock:             1;     /* [5] when phase lock detector enabled: =1 phase locked; =0 not locked */
    UINT32  Reserved0:                  26;    /* [31:6] Reserved */
} AMBA_SERDES_RX_OBSV0_REG_s;

typedef struct {
    UINT32  SgnvosIn:                   1;     /* [0] slicer offset calib manual: slicer offset sign control; default=0 */
    UINT32  SwapOffsetCalib:            1;     /* [1] Calibration of ctle/vga offset */
    UINT32  ObsvCalib:                  1;     /* [2] `=0 mipi_cdr_out is DCO related blocks; =1 mipi_cdr_out is Calibration related blocks; default=0 */
    UINT32  Saving:                     1;     /* [3] save dco dctrl to reg array */
    UINT32  ObErrSymbol:                1;     /* [4] 0:default */
    UINT32  EnDsm:                      1;     /* [5] CDR DCO mode only */
    UINT32  ManualDfeVcm:               1;     /* [6] dfe vcm calibration manual mode */
    UINT32  ManualCtleVcm:              1;     /* [7] ctle vcm calibration manual mode */
    UINT32  ManualEqVcm:                1;     /* [8] eq data buffer vcm calibration manual mode */
    UINT32  ManualSlicerBufferVcm:      1;     /* [9] slicer buffer vcm calibration manual mode */
    UINT32  RctSerdesTermResEn:         1;     /* [10] `=1 */
    UINT32  SlvsMode:                   1;     /* [11] `=1 */
    UINT32  RstnRct:                    1;     /* [12] manual reset the serdes; low active; default = 0 */
    UINT32  SwapScan0:                  1;     /* [13] slicer offset calib scan0 swap p/n; default=0 */
    UINT32  SwapScan1:                  1;     /* [14] slicer offset calib scan1 swap p/n; default=0 */
    UINT32  SlicerSelIQ:                1;     /* [15] set which clock method to use. =1 */
    UINT32  SettleWin:                  8;     /* [23:16] set calibration corrector settle wait time to wait analog circuit settle before looking at the results. Time = settle_win * 4 * 41.6ns (24MHz case)  */
    UINT32  BinthmShiftIn:              3;     /* [26:24] RCT manually control binthm_shift; the larger the more current */
    UINT32  FilterMode:                 1;     /* [27] PI_MODE; 1:DCO_MODE;  */
    UINT32  DcoGain:                    3;     /* [30:28] set dco gain */
    UINT32  Bypasspi:                   1;     /* [31] slicer control1 bypass cmos pi for i and q pathes */
} AMBA_SERDES_RX_CTRL0_REG_s;

typedef struct {
    UINT32  AutoBand:                   1;     /* [0] enable DCO auto band shift; default = 1 */
    UINT32  DcoPd:                      1;     /* [1] ; default=1  */
    UINT32  EnDither:                   1;     /* [2] enable dither for DCO dsm; default = 0 */
    UINT32  ClearOf:                    1;     /* [3] clear adder/dsm overflow flag; default = 0 */
    UINT32  SwapData0:                  1;     /* [4] slicer offset calib data0 swap p/n; default=0 */
    UINT32  SwapData1:                  1;     /* [5] slicer offset calib data1 swap p/n; default=0 */
    UINT32  SwapEdge0:                  1;     /* [6] slicer offset calib edge0 swap p/n; default=0 */
    UINT32  SwapEdge1:                  1;     /* [7] slicer offset calib edge1 swap p/n; default=0 */
    UINT32  Seldiv4:                    1;     /* [8] slicer clock gen dcc detector output frequency selction control bit;0 ckdiv2; 1 ckdiv4; default=0 */
    UINT32  BypassDcc:                  1;     /* [9] bypass dutycycle correction block before slicer use */
    UINT32  Pderr:                      1;     /* [10] power down error-slicer control; default=0 */
    UINT32  Pdeye:                      1;     /* [11] power down eye-slicer control; default=0 */
    UINT32  VoscalibEnIn:               1;     /* [12] slicer offset calibration enable; short buffer P/N output; default=0 */
    UINT32  DccStart:                   1;     /* [13] dcc_start; default=0 */
    UINT32  DccManual:                  1;     /* [14] dcc manual mode; default=1 */
    UINT32  DccManualLoad:              1;     /* [15] dcc load manual value for auto calib; default=0 */
    UINT32  DccSwap:                    1;     /* [16] reverse comparasion result; default=0 */
    UINT32  PdDes:                      1;     /* [17] power down serdes */
    UINT32  Sgnvref:                    1;     /* [18] Error slicer Vref sign control; default=1 */
    UINT32  Bypassctle:                 1;     /* [19] dfe data input control to bypass ctle; low active. =0 bypass CTLE stage. =1 */
    UINT32  ManualVgaCtleOffset:        1;     /* [20] ctle offset calibration manual mode */
    UINT32  Pddata:                     1;     /* [21] power down data/edge path summber */
    UINT32  DccPrbsEn:                  1;     /* [22] enable prbs_dither for dcc; default=0 */
    UINT32  FlipDco:                    1;     /* [23] Invert the UPDN results from phase detector */
    UINT32  PauseLfDco:                 1;     /* [24] Pause DCO loop filter..<1 */
    UINT32  FreqLockMode:               1;     /* [25] =0 Auto freq. lock =1 Manual mode.<0 */
    UINT32  RefClkSelDco:               1;     /* [26] Refclk select for DCO-CDR in FA =0 24MHz;=1 72MHz; default = 1 */
    UINT32  PdSlicer:                   1;     /* [27] power down slicer */
    UINT32  FreqReacquire:              1;     /* [28] Re-acquire frequency lock. */
    UINT32  StartCalib:                 1;     /* [29] Calibration start ctrl; multiple calibration's starts are selected by sel_calib */
    UINT32  SlicerOffsetManual:         1;     /* [30] slicer offset calibration manual mode; bypass accumulator code output; default=1 */
    UINT32  CdrEyemonEn:                1;     /* [31] CDR Monitor mode only. CDR eye monitor enable. 0 */
} AMBA_SERDES_RX_CTRL1_REG_s;

typedef struct {
    UINT32  ForceFreqLock:              1;     /* [0] Force frequency lock during auto FA mode; and using dctrl just before force_locked.; default = 0 */
    UINT32  CalibLoad:                  1;     /* [1] calibration circuit initial(manual) value load; default = 0 */
    UINT32  SslmsDfeManual:             1;     /* [2] Manual control of sslms dfe adaptation.; default = 0 */
    UINT32  SslmsDfePause:              1;     /* [3] Pause sslms dfe adaptation; default = 1 */
    UINT32  SslmsVrefManual:            1;     /* [4] Set 1 to manual control slice vref.; default = 0 */
    UINT32  SslmsVrefPause:             1;     /* [5] Pause sslms vref control adaptation.; default = 1 */
    UINT32  SslmsCtleManual:            1;     /* [6] set 1 to enable manual tuning of ctle res and cap; default = 0 */
    UINT32  SslmsCtleRpause:            1;     /* [7] pause ctle sslms res adaptation.; default = 1 */
    UINT32  SslmsCtleCpause:            1;     /* [8] pause ctle sslms cap adaptation.; default = 1 */
    UINT32  DccManualInv:               1;     /* [9] manual ctrl dcc object clk inverse; default=0; auto */
    UINT32  HaltDco:                    1;     /* [10] default=0 */
    UINT32  PdCtle:                     1;     /* [11] power down ctle stage; default = 0 */
    UINT32  PdCalib:                    1;     /* [12] pd_calibration; default = 1 */
    UINT32  Calibdone:                  1;     /* [13] slicer clock gen dc detector calibration done indicator signal;default=1 */
    UINT32  PdVga:                      1;     /* [14] Vga power down control; default=0 */
    UINT32  ManualRst:                  1;     /* [15] =0; slvsec_phy can reset the serdes; =1; rct reset the serdes; default = 0 */
    UINT32  LckdetThrsh:                4;     /* [19:16] threashhold setting for phase lock detection; 0-15 */
    UINT32  RctManualLckdet:            1;     /* [20] `=1:use rct to decide whether enable phase lock detection; =0 */
    UINT32  RctEnLckdet:                1;     /* [21] when manual_lckdet =1 */
    UINT32  PdSf:                       1;     /* [22] power down source follower stage; default = 0 */
    UINT32  SfIctrl:                    1;     /* [23] Increase source follower bias current; default = 0 */
    UINT32  SslmsVgaManual:             1;     /* [24] set 1 to enable manual tuning of vga res; default = 0 */
    UINT32  SslmsVgaRpause:             1;     /* [25] Pause sslms vga adaptation; default = 1 */
    UINT32  ForcePhaseLock:             1;     /* [26] When =1; force phase lock if phase lock detection is not working */
    UINT32  Reserved0:                  1;     /* [27] Reserved */
    UINT32  GainSelUnitSummerIn:        1;     /* [28] set unit summer DC gain in manual mode: = 1 0.5x Gain; = 0 1xGain */
    UINT32  GainSelSlicerBufIn:         1;     /* [29] set slicerBuf DC gain in manual mode: = 1 0.5x Gain; = 0 1xGain */
    UINT32  SlicerBW:                   2;     /* [31:30] Slicer Bandwidth control. =11 is fastest; =00 is slowest */
} AMBA_SERDES_RX_CTRL2_REG_s;

typedef struct {
    UINT32  Vrefgear:                   2;     /* [1:0] Error slicer Vref gear control; 00 avdd_min; 01 avd_nom; 11 avdd_max; default=1  */
    UINT32  DccWinMsb:                  2;     /* [3:2] dcc window counter msb; default=1; window cnt upper limit is {dcc_win_msb[1:0] */
    UINT32  Vosgear:                    2;     /* [5:4] slicer offset gear mode control 00 avdd_min; 01 avd_nom; 11 avdd_max; default=1  */
    UINT32  FreqDivSel:                 2;     /* [7:6] Select the pre_freq divider dividing ratio. This affect the FA locking resolution. default = 1; start_cnt = ref_clk/2^11; frequency counter will count cycles when start_cnt =1. then counter should be around 1024 at lock; the frequency error will be around */
    UINT32  DatarateSel:                2;     /* [9:8] Select the coarse target datarate in which DCO freq acquisition will be locking at. 0 1.152Gbps. 1 2.304Gbps. 2 4.608Gpbs. 3 6.912 Gbps.; default=2'h1  */
    UINT32  VcmFbSelin:                 2;     /* [11:10] determine which cm is selected */
    UINT32  SslmsEqMode:                3;     /* [14:12] sslms adaptation mode select. 0 stop all adaptation. 1. vga adaptation 2. vref adapat. 3. ctle adapt. 4.dfe adapt */
    UINT32  Reserved0:                  1;     /* [15] Reserved */
    UINT32  CtleFuncEnIn:               2;     /* [17:16] enable ctle degen cap&res */
    UINT32  VgaFuncEnIn:                1;     /* [18] enable vga res degen */
    UINT32  Reserved1:                  1;     /* [19] Reserved */
    UINT32  CalibLckDetThrsh:           4;     /* [23:20] Calibration lock detector threshold setting */
    UINT32  SlicerBufBW:                2;     /* [25:24] slicerBuf bandwidth select */
    UINT32  Reserved2:                  6;     /* [31:26] Reserved */
} AMBA_SERDES_RX_CTRL3_REG_s;

typedef struct {
    UINT32  CalibLckDetWin:             8;     /* [7:0] Set calibration lock detection sample duration window. T = (calibLckDetWin+1)*16*13n (72MHz case). T = (calibLckDetWin+1)*16*41n(24MHz case) */
    UINT32  BypassSummerVcmCalib:       1;     /* [8] = 1: bypass UnitSummer VCM Calibration in calibration controller */
    UINT32  BypassSlicerBufVcmCalib:    1;     /* [9] = 1: bypass Slicer Buffer VCM Calibration in calibration controller */
    UINT32  BypassCtleVcmCalib:         1;     /* [10] = 1: bypass CTLE VCM Calibration in calibration controller */
    UINT32  BypassVgaVcmCalib:          1;     /* [11] = 1: bypass VGA VCM Calibration in calibration controller */
    UINT32  IctrlCdr:                   3;     /* [14:12] slicer cdr ibias control ibias_cdr={2+Dec{ictrl[2:0]}}*ip15u/5 uA; default=3 */
    UINT32  BypassCtle2OffsetCalib:     1;     /* [15] = 1: bypass CTLE2 Offset Calibration in calibration controller */
    UINT32  IctrlDfe:                   3;     /* [18:16] slicer dfe ibias control ibias_dfe={2+Dec{ictrl[2:0]}}*ibias_cdr/5 uA; default=3  */
    UINT32  BypassSlicerOffsetCalib:    1;     /* [19] = 1: bypass slicer offset Calibration in calibration controller */
    UINT32  Pibw:                       4;     /* [23:20] slicer clock gen CMOS PI bandwidth control bits; default=8 */
    UINT32  Kdcc:                       3;     /* [26:24] slicer clock gen dcc dectector gain control signal; default=4 */
    UINT32  BypassCtle1OffsetCalib:     1;     /* [27] = 1: bypass CTLE1 Offset Calibration in calibration controller */
    UINT32  Dccbias:                    3;     /* [30:28] slicer dcc detector frequency bias control; default=4 */
    UINT32  IbiasMode:                  1;     /* [31] reserved1 */
} AMBA_SERDES_RX_CTRL4_REG_s;

typedef struct {
    UINT32  VocmdfeIn:                  5;     /* [4:0] slicer dfe summer output common mode level control bits; default=h13 */
    UINT32  Reserved0:                  3;     /* [7:5] Reserved */
    UINT32  CdrOutSelect:               6;     /* [13:8] CDR status groups selection */
    UINT32  Reserved1:                  2;     /* [15:14] Reserved */
    UINT32  IctrlCkgen:                 3;     /* [18:16] default=3 */
    UINT32  BypassVgaOffsetCalib:       1;     /* [19] = 1: bypass VGA Offset Calibration in calibration controller */
    UINT32  PhasedFrGain:               4;     /* [23:20] full rate phase detector gain.<7 */
    UINT32  SelCalib:                   3;     /* [26:24] Calibration enable selection 0 eq data buf vcm 1 ctle vcm 2 dfe vcm 3 slicer buf vcm 4 ctle offset 5 NA */
    UINT32  VocmdfebufIn:               5;     /* [31:27] slicer data buffer output common mode level control; default=10011 */
} AMBA_SERDES_RX_CTRL5_REG_s;

typedef struct {
    UINT32  AccumFModeDco:              12;    /* [11:0] Clamp for integer path of DCO mode filter. */
    UINT32  DctrlIn:                    14;    /* [25:12] Manually set up dctrl band[25 22] = dctl [21 12] */
    UINT32  Reserved0:                  2;     /* [27:26] Reserved */
    UINT32  IctrlEq:                    3;     /* [30:28] ctle ibias control ibias_ctle={2+Dec{ictrl[2:0]}}*ibias_cdr/5 uA; default=3  */
    UINT32  Reserved1:                  1;     /* [31] Reserved */
} AMBA_SERDES_RX_CTRL6_REG_s;

typedef struct {
    UINT32  DivIntDco:                  4;     /* [3:0] Integration path gain; default=4'h6 */
    UINT32  DivPDco:                    4;     /* [7:4] Propagational path gain; default=4'hf */
    UINT32  FreqoffsetThrsh:            4;     /* [11:8] Frequency offset tolerance threshold value.; default=4'h3 */
    UINT32  FreqAdjust:                 16;    /* [27:12] Adjust locking frequency. freq_in_lock = datarate_sel*[cnt/(cnt+freq_adjust)] in both 24MHz/72MHz reference clk case. Cnt=2048 in default. freq_adjust = [(datarate_sel/freq_in_lock) - 1]*cnt */
    UINT32  Reserved0:                  4;     /* [31:28] Reserved */
} AMBA_SERDES_RX_CTRL7_REG_s;

typedef struct {
    UINT32  Tap1_in:                    6;     /* [5:0] DFE 1st tap weight value control in manual mode. */
    UINT32  Reserved0:                  2;     /* [7:6] Reserved */
    UINT32  Tap2_in:                    5;     /* [12:8] DFE 2nd tap weight value control in manual mode. */
    UINT32  Reserved1:                  11;    /* [23:13] Reserved */
    UINT32  SslmsDfeGain:               3;     /* [26:24] sslms dfe adaptatation gain control. */
    UINT32  BandShiftIn:                5;     /* [31:27] manually control DCO band_shift; the larger the more current. */
} AMBA_SERDES_RX_CTRL8_REG_s;

typedef struct {
    UINT32  VrefIn:                     7;     /* [6:0] Slicer Vref value control in manual mode. */
    UINT32  Reserved0:                  1;     /* [7] Reserved */
    UINT32  RctrlIn:                    5;     /* [12:8] rx terminal resistor thermo control code manual mode; default=5'b10000 */
    UINT32  ManualTermEn:               1;     /* [13] enable manual set for termination resistor; default 0 */
    UINT32  Reserved1:                  10;    /* [23:14] Reserved */
    UINT32  PiEye:                      8;     /* [31:24] slicer clock gen CMOS PI eye phase PI code; default=32 */
} AMBA_SERDES_RX_CTRL9_REG_s;

typedef struct {
    UINT32  VcmSetIn:                   8;     /* [7:0] vcm reference selection for auto calibration; thm code; min 8'hff=0.3V; max 8'h01=0.7V */
    UINT32  OffsetCodeIn:               6;     /* [13:8] Vga/ctle offset calib manual input; in 2s; encode inside module to obtain offset_ctle_ctrlp(/n)in_3(/2/1) */
    UINT32  Reserved0:                  2;     /* [15:14] Reserved */
    UINT32  DctrlClamp:                 8;     /* [23:16] Clamp dctrl; range +/- dctrl_clamp*2 */
    UINT32  SslmsVrefGain:              3;     /* [26:24] sslms vref adaptatation gain control. */
    UINT32  Reserved1:                  1;     /* [27] Reserved */
    UINT32  SslmsCtleCgain:             3;     /* [30:28] set ctle sslms cap gain. */
    UINT32  Reserved2:                  1;     /* [31] Reserved */
} AMBA_SERDES_RX_CTRL10_REG_s;

typedef struct {
    UINT32  CtleCctrl0_in:              4;     /* [3:0] ctle stage 0 cap tune in manual mode. */
    UINT32  CtleCctrl1_in:              4;     /* [7:4] ctle stage 1 cap tune in manual mode. */
    UINT32  CtleRctrl0_in:              4;     /* [11:8] ctle stage 0 res tune in manual mode. */
    UINT32  CtleRctrl1_in:              4;     /* [15:12] ctle stage 1 res tune in manual mode. */
    UINT32  VgaRctrlIn:                 4;     /* [19:16] vga res tune in manual mode. */
    UINT32  SslmsCtleRgain:             3;     /* [22:20] set ctle sslms res gain. */
    UINT32  Reserved0:                  1;     /* [23] Reserved */
    UINT32  VcmVgaCtrlIn:               4;     /* [27:24] vcm databuf setting; for manual mode. */
    UINT32  VcmCtleCtrlpinIn:           4;     /* [31:28] vcm ctle setting; for manual mode. */
} AMBA_SERDES_RX_CTRL11_REG_s;

typedef struct {
    UINT32  LpgainVcm:                  4;     /* [3:0] lpgain adjustment for vcm calibration */
    UINT32  Reserved0:                  24;    /* [27:4] Reserved */
    UINT32  SslmsVgaRgain:              3;     /* [30:28] set ctle sslms res gain. */
    UINT32  Reserved1:                  1;     /* [31] Reserved */
} AMBA_SERDES_RX_CTRL12_REG_s;

typedef struct {
    UINT32  DccselIn:                   4;     /* [3:0] dfe_tap1_ctrl[3 0]; 0~15; tap1 de_amphasize factor; default 0  */
    UINT32  Reserved0:                  28;    /* [31:4] Reserved */
} AMBA_SERDES_RX_CTRL13_REG_s;

typedef struct {
    UINT32  CodeInData0:                6;     /* [5:0] slicer offset calib data0 in 2s; default0 */
    UINT32  Reserved0:                  1;     /* [6] Reserved */
    UINT32  CodeInEdge0:                6;     /* [12:7] slicer offset calib edge0 in 2s; default0 */
    UINT32  Reserved1:                  3;     /* [15:13] Reserved */
    UINT32  CodeInData1:                6;     /* [21:16] slicer offset calib data1 in 2s; default0 */
    UINT32  Reserved2:                  2;     /* [23:22] Reserved */
    UINT32  CodeInEdge1:                6;     /* [29:24] slicer offset calib edge1 in 2s; defualt0 */
    UINT32  Reserved3:                  2;     /* [31:30] Reserved */
} AMBA_SERDES_RX_CTRL14_REG_s;

typedef struct {
    UINT32  PhyVinDataNCtrl:            1;     /* [0] OBSV PPI Signal, =2'h1, 10bit mode, =2'h3, 20bit mode. OtherValue not valide */
    UINT32  PhyVinPhyDoRdy:             1;     /* [1] OBSV PPI Signal, =2'h1, 10bit ready, =2'h3, 20bit ready. */
    UINT32  RXSymbolErr:                1;     /* [2] OBSV PPI signal  */
    UINT32  Reserved0:                  1;     /* [3] Reserved */
    UINT32  PhyVinSlvsecEnd:            1;     /* [4] OBSV PPI signal */
    UINT32  PhyVinSlvsecStart:          1;     /* [5] OBSV PPI signal */
    UINT32  Reserved1:                  6;     /* [11:6] Reserved */
    UINT32  PhyVinSlvsecReady:          1;     /* [12] OBSV PPI signal, = 1 SLVSECPHY is ready */
    UINT32  PhyVinStandbyDet:           1;     /* [13] OBSV PPI signal, =1 SLVSECPHY StandBy request is detected */
    UINT32  ResError:                   1;     /* [14] OBSV PPI signal,  */
    UINT32  Reserved2:                  17;    /* [31:15] Reserved */
} AMBA_SLVSEC_PHY_OBSV0_REG_s;

typedef struct {
    UINT32  PrbsDone:                   1;     /* [0] */
    UINT32  PrbsPass:                   1;     /* [1] */
    UINT32  Reserved0:                  30;    /* [31:2] Reserved */
} AMBA_SLVSEC_PHY_OBSV1_REG_s;

typedef struct {
    UINT32  PrbsErr:                    32;    /* [31:0] prbs0 test error count results */
} AMBA_SLVSEC_PHY_OBSV2_REG_s;

typedef struct {
    UINT32  SlvsecLpbkPass:             1;     /* [0] slvsec_loopbk_pass */
    UINT32  SlvsecLpbkReady:            1;     /* [1] slvsec_loopbk_ready */
    UINT32  Reserved0:                  2;     /* [3:2] Reserved */
    UINT32  MphyRxState:                4;     /* [7:4] mphy_rx_state */
    UINT32  Reserved1:                  24;    /* [31:8] Reserved */
} AMBA_SLVSEC_PHY_OBSV3_REG_s;

typedef struct {
    UINT32  RctPhySetup:                1;     /* [0] manual PPI signal */
    UINT32  RctPhyConfig:               1;     /* [1] */
    UINT32  RctPhyStandby:              1;     /* [2] */
    UINT32  MphyForceRxReady:           1;     /* [3] 1 to force SLVSEC RX to enter READY state. */
    UINT32  RefclkFreq:                 1;     /* [4] 0:24MHz, 1:72MHz for 100u counter inside RX */
    UINT32  NoRdError:                  1;     /* [5] */
    UINT32  MphySymRealign:             2;     /* [7:6] 0:No re-alignment, 1,2,3:Number of consecutive 8b10b RD error to re-align */
    UINT32  ManualPhyPpi:               1;     /* [8] =1, enable manual contrl phy PPI signals by rct, bypass Link layer; defatul= 0  */
    UINT32  MphyFastSim:                1;     /* [9] (Simulation Only) for fast sim time */
    UINT32  MphySymAlignNolock:         1;     /* [10] 0: Symbol alignment position locks after 3 markers found.  1: Symbol alignment position free running. */
    UINT32  RxStandbyDetEn:             1;     /* [11] =1, enable sdtandby detection; default = 1 */
    UINT32  RctRstnPhy:                 1;     /* [12] manual reset phy from rct */
    UINT32  SlvsecLpbkEn:               1;     /* [13] */
    UINT32  SlvsecLpbkStart:            1;     /* [14] */
    UINT32  Reserved0:                  1;     /* [15] Reserved */
    UINT32  ForceSymSync:               4;     /* [19:16] SLVSEC RX Only.  Non-zero values force SLVSEC RX to lock onto the programmed bit position during bit synchronization. */
    UINT32  PrbsLpbkEn:                 1;     /* [20] prbs loopbk enable>.<0 */
    UINT32  CntBasedLckDetect:          1;     /* [21] `=0 PHY will wait CDR phase lock signal under training state, and start check MARKER byte after CDR indicate lock; =1 PHY will use timer instead */
    UINT32  SqMode:                     1;     /* [22] `=1 PHY will enable sq detector function */
    UINT32  Reserved1:                  9;     /* [31:23] Reserved */
} AMBA_SLVSEC_PHY_CTRL0_REG_s;

typedef struct {
    UINT32  RxLineresetCtrl:            5;     /* [4:0] MPHY mode only. */
    UINT32  IsExactDeskew:              1;     /* [5] SLVSEC RX Only.  Set to 1 to match received Deskew Code with programmed code with no error. */
    UINT32  IsExactEnd:                 1;     /* [6] SLVSEC RX Only.  Set to 1 to match received End Code with programmed code with no error. */
    UINT32  IsExactPad:                 1;     /* [7] SLVSEC RX Only.  Set to 1 to match received Pad Code with programmed code with no error. */
    UINT32  IsExactStandby:             1;     /* [8] SLVSEC RX Only.  Set to 1 to match received Standby Code with programmed code with no error. */
    UINT32  IsExactStart:               1;     /* [9] SLVSEC RX Only.  Set to 1 to match received Start Code with programmed code with no error. */
    UINT32  Bypass_8b10b:               1;     /* [10] SLVSEC RX Only.  0: Enable 8b10b decoder. 1: Bypass.  Bypass decoder logic will disable deskew logic. */
    UINT32  PrbsInTest:                 1;     /* [11] */
    UINT32  CdrCntCtrl:                 17;    /* [28:12] Wait cdr_cnt time to wait cdr lock after phy rx_state enters 9 before start check marker. This one may need modification */
    UINT32  Reserved0:                  3;     /* [31:29] Reserved */
} AMBA_SLVSEC_PHY_CTRL1_REG_s;

typedef struct {
    UINT32  DeskewSymbol:               8;     /* [7:0] set deskew symbol, default = 8'h60 */
    UINT32  StandbySymbol:              8;     /* [15:8] set standby symbol, defatul = 8'h03 */
    UINT32  IdleCode:                   8;     /* [23:16] set idle_code, defatul = 8'h000 */
    UINT32  Reserved0:                  8;     /* [31:24] Reserved */
} AMBA_SLVSEC_PHY_CTRL2_REG_s;

typedef struct {
    UINT32  SRxUlpsClkNot:              1;     /* [0] */
    UINT32  SRxClkActiveHsClk:          1;     /* [1] */
    UINT32  SUlpsActiveNotClk:          1;     /* [2] */
    UINT32  SStopStateClk:              1;     /* [3] */
    UINT32  Reserved0:                  4;     /* [7:4] Reserved */
    UINT32  PBitClkHsDiv8:              1;     /* [8] */
    UINT32  PBitClkHsDiv4:              1;     /* [9] */
    UINT32  PBitClkHsDiv2:              1;     /* [10] */
    UINT32  Reserved1:                  5;     /* [15:11] Reserved */
    UINT32  SLaneEnableC:               1;     /* [16] Enable of slave clock lane  */
    UINT32  SLaneEnable0:               1;     /* [17] Enable of slave data0 lane */
    UINT32  SLaneEnable1:               1;     /* [18] Enable of slave data1 lane */
    UINT32  SLaneEnable2:               1;     /* [19] Enable of slave data2 lane */
    UINT32  SLaneEnable3:               1;     /* [20] Enable of slave data3 lane */
    UINT32  Reserved2:                  3;     /* [23:21] Reserved */
    UINT32  SLaneBiasEnC:               1;     /* [24] Bias enable of slave clock lane  */
    UINT32  SLaneBiasEn0:               1;     /* [25] Bias enable of slave data0 lane */
    UINT32  SLaneBiasEn1:               1;     /* [26] Bias enable of slave data1 lane */
    UINT32  SLaneBiasEn2:               1;     /* [27] Bias enable of slave data2 lane */
    UINT32  SLaneBiasEn3:               1;     /* [28] Bias enable of slave data3 lane */
    UINT32  Reserved3:                  3;     /* [31:29] Reserved */
} AMBA_DCPHY_RCT_OBSV0_REG_s;

typedef struct {
    UINT32  Reserved0:                  4;     /* [3:0] Reserved */
    UINT32  SStopState0:                1;     /* [4] */
    UINT32  SStopState1:                1;     /* [5] */
    UINT32  SStopState2:                1;     /* [6] */
    UINT32  SStopState3:                1;     /* [7] */
    UINT32  SUlpsActiveNot0:            1;     /* [8] */
    UINT32  SUlpsActiveNot1:            1;     /* [9] */
    UINT32  SUlpsActiveNot2:            1;     /* [10] */
    UINT32  SUlpsActiveNot3:            1;     /* [11] */
    UINT32  SErr0SotHs:                 1;     /* [12] */
    UINT32  SErr1SotHs:                 1;     /* [13] */
    UINT32  SErr2SotHs:                 1;     /* [14] */
    UINT32  SErr3SotHs:                 1;     /* [15] */
    UINT32  SErr0Esc:                   1;     /* [16] */
    UINT32  SErr1Esc:                   1;     /* [17] */
    UINT32  SErr2Esc:                   1;     /* [18] */
    UINT32  SErr3Esc:                   1;     /* [19] */
    UINT32  SErr0Control:               1;     /* [20] */
    UINT32  SErr1Control:               1;     /* [21] */
    UINT32  SErr2Control:               1;     /* [22] */
    UINT32  SErr3Control:               1;     /* [23] */
    UINT32  SErr0SotSyncHs:             1;     /* [24] */
    UINT32  SErr1SotSyncHs:             1;     /* [25] */
    UINT32  SErr2SotSyncHs:             1;     /* [26] */
    UINT32  SErr3SotSyncHs:             1;     /* [27] */
    UINT32  SRxInvalidCodeHs0:          1;     /* [28] */
    UINT32  SRxInvalidCodeHs1:          1;     /* [29] */
    UINT32  SRxInvalidCodeHs2:          1;     /* [30] */
    UINT32  Reserved1:                  1;     /* [31] Reserved */
} AMBA_DCPHY_RCT_OBSV1_REG_s;

typedef struct {
    UINT32  Reserved0:                  24;    /* [23:0] Reserved */
    UINT32  SRxUlpsEsc0:                1;     /* [24] */
    UINT32  SRxUlpsEsc1:                1;     /* [25] */
    UINT32  SRxUlpsEsc2:                1;     /* [26] */
    UINT32  SRxUlpsEsc3:                1;     /* [27] */
    UINT32  SRxClkEsc0:                 1;     /* [28] */
    UINT32  SRxClkEsc1:                 1;     /* [29] */
    UINT32  SRxClkEsc2:                 1;     /* [30] */
    UINT32  SRxClkEsc3:                 1;     /* [31] */
} AMBA_DCPHY_RCT_OBSV2_REG_s;

typedef struct {
    UINT32  SForceRxmode0:              1;     /* [0] Force lane0 into receive mode/wait for stop state */
    UINT32  SForceRxmode1:              1;     /* [1] Force lane1 into receive mode/wait for stop state */
    UINT32  SForceRxmode2:              1;     /* [2] Force lane2 into receive mode/wait for stop state */
    UINT32  SForceRxmode3:              1;     /* [3] Force lane3 into receive mode/wait for stop state */
    UINT32  MResetn0:                   1;     /* [4] Reset signal of master data0 lane (actvie low) */
    UINT32  MResetn1:                   1;     /* [5] Reset signal of master data1 lane (actvie low) */
    UINT32  MResetn2:                   1;     /* [6] Reset signal of master data2 lane (actvie low) */
    UINT32  MResetn3:                   1;     /* [7] Reset signal of master data3 lane (actvie low) */
    UINT32  MResetnC:                   1;     /* [8] Reset signal of master clock lane (actvie low) */
    UINT32  Reserved0:                  4;     /* [12:9] Reserved */
    UINT32  MCphyMode0:                 1;     /* [13] C-PHY mode select siganl of master data0 lane */
    UINT32  MCphyMode1:                 1;     /* [14] C-PHY mode select siganl of master data1 lane */
    UINT32  MCphyMode2:                 1;     /* [15] C-PHY mode select siganl of master data2 lane */
    UINT32  SCphyMode0:                 1;     /* [16] C-PHY mode select siganl of slave data0 lane */
    UINT32  SCphyMode1:                 1;     /* [17] C-PHY mode select siganl of slave data1 lane */
    UINT32  SCphyMode2:                 1;     /* [18] C-PHY mode select siganl of slave data2 lane */
    UINT32  BCphyMode:                  1;     /* [19] C-PHY mode select siganl of BIAS & DTB block */
    UINT32  PCphyMode:                  1;     /* [20] C-PHY mode select siganl of PLL block */
    UINT32  Reserved1:                  3;     /* [23:21] Reserved */
    UINT32  SResetn0:                   1;     /* [24] Reset signal of slave data0 lane (actvie low) */
    UINT32  SResetn1:                   1;     /* [25] Reset signal of slave data1 lane (actvie low) */
    UINT32  SResetn2:                   1;     /* [26] Reset signal of slave data2 lane (actvie low) */
    UINT32  SResetn3:                   1;     /* [27] Reset signal of slave data3 lane (actvie low) */
    UINT32  PMresetn:                   1;     /* [28] Master block reset to PLL */
    UINT32  SResetnC:                   1;     /* [29] Reset signal of slave clock lane (actvie low) */
    UINT32  Reserved2:                  2;     /* [31:30] Reserved */
} AMBA_DCPHY_RCT_CTRL0_REG_s;

typedef struct {
    UINT32  SLaneApbAddr0:              6;     /* [5:0] Offset APB address of slave data0 lane */
    UINT32  Reserved0:                  2;     /* [7:6] Reserved */
    UINT32  SLaneApbAddr1:              6;     /* [13:8] Offset APB address of slave data1 lane */
    UINT32  Reserved1:                  2;     /* [15:14] Reserved */
    UINT32  SLaneApbAddr2:              6;     /* [21:16] Offset APB address of slave data2 lane */
    UINT32  Reserved2:                  2;     /* [23:22] Reserved */
    UINT32  SLaneApbAddr3:              6;     /* [29:24] Offset APB address of slave data3 lane */
    UINT32  Reserved3:                  2;     /* [31:30] Reserved */
} AMBA_DCPHY_RCT_CTRL1_REG_s;

typedef struct {
    UINT32  Reserved0:                  8;     /* [7:0] Reserved */
    UINT32  BLaneApbAddr:               6;     /* [13:8] Offset APB address of BIAS & DTB blcok */
    UINT32  Reserved1:                  2;     /* [15:14] Reserved */
    UINT32  PLaneApbAddr:               6;     /* [21:16] Offset APB address of PLL block */
    UINT32  Reserved2:                  2;     /* [23:22] Reserved */
    UINT32  SLaneApbAddrC:              6;     /* [29:24] Offset APB address of slave clock lane */
    UINT32  Reserved3:                  2;     /* [31:30] Reserved */
} AMBA_DCPHY_RCT_CTRL2_REG_s;

/*
 * MIPI/VIN PHY All Registers
 */
typedef struct {
    volatile AMBA_DPHY_OBSERVE0_REG_s               Dphy0Obsv0;             /* 0x000(RO): MIPI DPHY0 Observation (32-bit) */
    volatile AMBA_DPHY_OBSERVE1_REG_s               Dphy0Obsv1;             /* 0x004(RO): MIPI DPHY0 Observation (32-bit) */
    volatile AMBA_DPHY_OBSERVE2_REG_s               Dphy0Obsv2;             /* 0x008(RO): MIPI DPHY0 Observation Rx related (32-bit) */
    volatile UINT32                                 Dphy0Obsv3;             /* 0x00C(RO): MIPI DPHY0 Observation (32-bit) */
    volatile AMBA_DPHY_OBSERVE4_REG_s               Dphy0Obsv4;             /* 0x010(RO): MIPI DPHY0 Observation (32-bit) */
    volatile UINT32                                 Dphy0Obsv5;             /* 0x014(RO): MIPI DPHY0 Observation (32-bit) */
    volatile UINT32                                 Dphy0Obsv6;             /* 0x018(RO): MIPI DPHY0 Observation (32-bit) */
    volatile AMBA_DPHY_CTRL0_REG_s                  Dphy0Ctrl0;             /* 0x01C(RW): MIPI DPHY0 Control 0 */
    volatile AMBA_DPHY_CTRL1_REG_s                  Dphy0Ctrl1;             /* 0x020(RW): MIPI DPHY0 Control 1 */
    volatile AMBA_DPHY_CTRL2_REG_s                  Dphy0Ctrl2;             /* 0x024(RW): MIPI DPHY0 Control 2 */
    volatile AMBA_DPHY_CTRL3_REG_s                  Dphy0Ctrl3;             /* 0x028(RW): MIPI DPHY0 Control 3 */
    volatile AMBA_DPHY_CTRL4_REG_s                  Dphy0Ctrl4;             /* 0x02C(RW): MIPI DPHY0 Control 4 */
    volatile AMBA_DPHY_CTRL5_REG_s                  Dphy0Ctrl5;             /* 0x030(RW): MIPI DPHY0 Control 5 */
    volatile AMBA_DPHY_CTRL6_REG_s                  Dphy0Ctrl6;             /* 0x034(RW): MIPI DPHY0 Control 6 */
    volatile AMBA_DPHY_CTRL7_REG_s                  Dphy0Ctrl7;             /* 0x038(RW): MIPI DPHY0 Control 7 */
    volatile AMBA_DPHY_CTRL8_REG_s                  Dphy0Ctrl8;             /* 0x03C(RW): MIPI DPHY0 Control 8 */
    volatile AMBA_DPHY_CTRL9_REG_s                  Dphy0Ctrl9;             /* 0x040(RW): MIPI DPHY0 Control 9 */
    volatile AMBA_DPHY_CTRL10_REG_s                 Dphy0Ctrl10;            /* 0x044(RW): MIPI DPHY0 Control 10 */
    volatile AMBA_DPHY_CTRL11_REG_s                 Dphy0Ctrl11;            /* 0x048(RW): MIPI DPHY0 Control 11 */
    volatile AMBA_DPHY_CTRL12_REG_s                 Dphy0Ctrl12;            /* 0x04C(RW): MIPI DPHY0 Control 12 */
    volatile AMBA_DPHY_CTRL13_REG_s                 Dphy0Ctrl13;            /* 0x050(RW): MIPI DPHY0 Control 13 */
    volatile UINT32                                 Dphy0Ctrl14;            /* 0x054(RW): MIPI DPHY0 Control 14 */
    volatile AMBA_DPHY_CTRL15_REG_s                 Dphy0Ctrl15;            /* 0x058(RW): MIPI DPHY0 Control 15 */
    volatile AMBA_PHY_BIST_CTRL11_REG_s             MipiPhyBistCtrl11;      /* 0x05C(RW): Control register for mipi_phy_bist */
    volatile UINT32                                 MipiPhyBistCtrl12;      /* 0x060(RW): Control register for mipi_phy_bist (CfgLaneEnable, 32-bit) */
    volatile UINT32                                 MipiPhyBistObsv11;      /* 0x064(RO): Observe register (phy_bist_dbg_reg_eot_fail, 32-bit) */
    volatile UINT32                                 MipiPhyBistObsv12;      /* 0x068(RO): Observe register (phy_bist_dbg_reg_eot_pass, 32-bit) */
    volatile UINT32                                 MipiPhyBistObsv13;      /* 0x06C(RO): Observe register (phy_bist_dbg_reg_sot_fail, 32-bit) */
    volatile UINT32                                 MipiPhyBistObsv14;      /* 0x070(RO): Observe register (phy_bist_dbg_reg_sot_pass, 32-bit) */
    volatile UINT32                                 MipiPhyBistObsv15;      /* 0x074(RO): Observe register (phy_bist_dbg_reg_test_pattern_fail, 32-bit) */
    volatile UINT32                                 MipiPhyBistObsv16;      /* 0x078(RO): Observe register (phy_bist_dbg_reg_test_pattern_pass, 32-bit) */
    volatile UINT32                                 MipiPhyBistObsv17;      /* 0x07C(RO): Observe register (phy_bist_fail, 32-bit) */
    volatile UINT32                                 MipiPhyBistObsv18;      /* 0x080(RO): Observe register (phy_bist_done, 32-bit) */
    volatile AMBA_SERDES_RX_OBSV0_REG_s             SerdesRx0Obsv0;         /* 0x084(RO): */
    volatile UINT32                                 SerdesRx0Obsv1;         /* 0x088(RO): Muxed output of serdes_rx_dig status */
    volatile AMBA_SERDES_RX_OBSV0_REG_s             SerdesRx1Obsv0;         /* 0x08C(RO): */
    volatile UINT32                                 SerdesRx1Obsv1;         /* 0x090(RO): Muxed output of serdes_rx_dig status */
    volatile AMBA_SERDES_RX_OBSV0_REG_s             SerdesRx2Obsv0;         /* 0x094(RO): */
    volatile UINT32                                 SerdesRx2Obsv1;         /* 0x098(RO): Muxed output of serdes_rx_dig status */
    volatile AMBA_SERDES_RX_OBSV0_REG_s             SerdesRx3Obsv0;         /* 0x09C(RO): */
    volatile UINT32                                 SerdesRx3Obsv1;         /* 0x0A0(RO): Muxed output of serdes_rx_dig status */
    volatile AMBA_SERDES_RX_OBSV0_REG_s             SerdesRx4Obsv0;         /* 0x0A4(RO): */
    volatile UINT32                                 SerdesRx4Obsv1;         /* 0x0A8(RO): Muxed output of serdes_rx_dig status */
    volatile AMBA_SERDES_RX_OBSV0_REG_s             SerdesRx5Obsv0;         /* 0x0AC(RO): */
    volatile UINT32                                 SerdesRx5Obsv1;         /* 0x0B0(RO): Muxed output of serdes_rx_dig status */
    volatile AMBA_SERDES_RX_OBSV0_REG_s             SerdesRx6Obsv0;         /* 0x0B4(RO): */
    volatile UINT32                                 SerdesRx6Obsv1;         /* 0x0B8(RO): Muxed output of serdes_rx_dig status */
    volatile AMBA_SERDES_RX_OBSV0_REG_s             SerdesRx7Obsv0;         /* 0x0BC(RO): */
    volatile UINT32                                 SerdesRx7Obsv1;         /* 0x0C0(RO): Muxed output of serdes_rx_dig status */
    volatile AMBA_SERDES_RX_OBSV0_REG_s             SerdesRx8Obsv0;         /* 0x0C4(RO): */
    volatile UINT32                                 SerdesRx8Obsv1;         /* 0x0C8(RO): Muxed output of serdes_rx_dig status */
    volatile AMBA_SERDES_RX_OBSV0_REG_s             SerdesRx9Obsv0;         /* 0x0CC(RO): */
    volatile UINT32                                 SerdesRx9Obsv1;         /* 0x0D0(RO): Muxed output of serdes_rx_dig status */
    volatile AMBA_SERDES_RX_OBSV0_REG_s             SerdesRx10Obsv0;        /* 0x0D4(RO): */
    volatile UINT32                                 SerdesRx10Obsv1;        /* 0x0D8(RO): Muxed output of serdes_rx_dig status */
    volatile AMBA_SERDES_RX_OBSV0_REG_s             SerdesRx11Obsv0;        /* 0x0DC(RO): */
    volatile UINT32                                 SerdesRx11Obsv1;        /* 0x0E0(RO): Muxed output of serdes_rx_dig status */
    volatile AMBA_SERDES_RX_CTRL0_REG_s             SerdesRx0Ctrl0;         /* 0x0E4(RW): */
    volatile AMBA_SERDES_RX_CTRL1_REG_s             SerdesRx0Ctrl1;         /* 0x0E8(RW): */
    volatile AMBA_SERDES_RX_CTRL2_REG_s             SerdesRx0Ctrl2;         /* 0x0EC(RW): */
    volatile AMBA_SERDES_RX_CTRL3_REG_s             SerdesRx0Ctrl3;         /* 0x0F0(RW): */
    volatile AMBA_SERDES_RX_CTRL4_REG_s             SerdesRx0Ctrl4;         /* 0x0F4(RW): */
    volatile AMBA_SERDES_RX_CTRL5_REG_s             SerdesRx0Ctrl5;         /* 0x0F8(RW): */
    volatile AMBA_SERDES_RX_CTRL6_REG_s             SerdesRx0Ctrl6;         /* 0x0FC(RW): */
    volatile AMBA_SERDES_RX_CTRL7_REG_s             SerdesRx0Ctrl7;         /* 0x100(RW): */
    volatile AMBA_SERDES_RX_CTRL8_REG_s             SerdesRx0Ctrl8;         /* 0x104(RW): */
    volatile AMBA_SERDES_RX_CTRL9_REG_s             SerdesRx0Ctrl9;         /* 0x108(RW): */
    volatile AMBA_SERDES_RX_CTRL10_REG_s            SerdesRx0Ctrl10;        /* 0x10C(RW): */
    volatile AMBA_SERDES_RX_CTRL11_REG_s            SerdesRx0Ctrl11;        /* 0x110(RW): */
    volatile AMBA_SERDES_RX_CTRL12_REG_s            SerdesRx0Ctrl12;        /* 0x114(RW): */
    volatile AMBA_SERDES_RX_CTRL13_REG_s            SerdesRx0Ctrl13;        /* 0x118(RW): */
    volatile AMBA_SERDES_RX_CTRL14_REG_s            SerdesRx0Ctrl14;        /* 0x11C(RW): */
    volatile AMBA_SERDES_RX_CTRL0_REG_s             SerdesRx1Ctrl0;         /* 0x120(RW): */
    volatile AMBA_SERDES_RX_CTRL1_REG_s             SerdesRx1Ctrl1;         /* 0x124(RW): */
    volatile AMBA_SERDES_RX_CTRL2_REG_s             SerdesRx1Ctrl2;         /* 0x128(RW): */
    volatile AMBA_SERDES_RX_CTRL3_REG_s             SerdesRx1Ctrl3;         /* 0x12C(RW): */
    volatile AMBA_SERDES_RX_CTRL4_REG_s             SerdesRx1Ctrl4;         /* 0x130(RW): */
    volatile AMBA_SERDES_RX_CTRL5_REG_s             SerdesRx1Ctrl5;         /* 0x134(RW): */
    volatile AMBA_SERDES_RX_CTRL6_REG_s             SerdesRx1Ctrl6;         /* 0x138(RW): */
    volatile AMBA_SERDES_RX_CTRL7_REG_s             SerdesRx1Ctrl7;         /* 0x13C(RW): */
    volatile AMBA_SERDES_RX_CTRL8_REG_s             SerdesRx1Ctrl8;         /* 0x140(RW): */
    volatile AMBA_SERDES_RX_CTRL9_REG_s             SerdesRx1Ctrl9;         /* 0x144(RW): */
    volatile AMBA_SERDES_RX_CTRL10_REG_s            SerdesRx1Ctrl10;        /* 0x148(RW): */
    volatile AMBA_SERDES_RX_CTRL11_REG_s            SerdesRx1Ctrl11;        /* 0x14C(RW): */
    volatile AMBA_SERDES_RX_CTRL12_REG_s            SerdesRx1Ctrl12;        /* 0x150(RW): */
    volatile AMBA_SERDES_RX_CTRL13_REG_s            SerdesRx1Ctrl13;        /* 0x154(RW): */
    volatile AMBA_SERDES_RX_CTRL14_REG_s            SerdesRx1Ctrl14;        /* 0x158(RW): */
    volatile AMBA_SERDES_RX_CTRL0_REG_s             SerdesRx2Ctrl0;         /* 0x15C(RW): */
    volatile AMBA_SERDES_RX_CTRL1_REG_s             SerdesRx2Ctrl1;         /* 0x160(RW): */
    volatile AMBA_SERDES_RX_CTRL2_REG_s             SerdesRx2Ctrl2;         /* 0x164(RW): */
    volatile AMBA_SERDES_RX_CTRL3_REG_s             SerdesRx2Ctrl3;         /* 0x168(RW): */
    volatile AMBA_SERDES_RX_CTRL4_REG_s             SerdesRx2Ctrl4;         /* 0x16C(RW): */
    volatile AMBA_SERDES_RX_CTRL5_REG_s             SerdesRx2Ctrl5;         /* 0x170(RW): */
    volatile AMBA_SERDES_RX_CTRL6_REG_s             SerdesRx2Ctrl6;         /* 0x174(RW): */
    volatile AMBA_SERDES_RX_CTRL7_REG_s             SerdesRx2Ctrl7;         /* 0x178(RW): */
    volatile AMBA_SERDES_RX_CTRL8_REG_s             SerdesRx2Ctrl8;         /* 0x17C(RW): */
    volatile AMBA_SERDES_RX_CTRL9_REG_s             SerdesRx2Ctrl9;         /* 0x180(RW): */
    volatile AMBA_SERDES_RX_CTRL10_REG_s            SerdesRx2Ctrl10;        /* 0x184(RW): */
    volatile AMBA_SERDES_RX_CTRL11_REG_s            SerdesRx2Ctrl11;        /* 0x188(RW): */
    volatile AMBA_SERDES_RX_CTRL12_REG_s            SerdesRx2Ctrl12;        /* 0x18C(RW): */
    volatile AMBA_SERDES_RX_CTRL13_REG_s            SerdesRx2Ctrl13;        /* 0x190(RW): */
    volatile AMBA_SERDES_RX_CTRL14_REG_s            SerdesRx2Ctrl14;        /* 0x194(RW): */
    volatile AMBA_SERDES_RX_CTRL0_REG_s             SerdesRx3Ctrl0;         /* 0x198(RW): */
    volatile AMBA_SERDES_RX_CTRL1_REG_s             SerdesRx3Ctrl1;         /* 0x19C(RW): */
    volatile AMBA_SERDES_RX_CTRL2_REG_s             SerdesRx3Ctrl2;         /* 0x1A0(RW): */
    volatile AMBA_SERDES_RX_CTRL3_REG_s             SerdesRx3Ctrl3;         /* 0x1A4(RW): */
    volatile AMBA_SERDES_RX_CTRL4_REG_s             SerdesRx3Ctrl4;         /* 0x1A8(RW): */
    volatile AMBA_SERDES_RX_CTRL5_REG_s             SerdesRx3Ctrl5;         /* 0x1AC(RW): */
    volatile AMBA_SERDES_RX_CTRL6_REG_s             SerdesRx3Ctrl6;         /* 0x1B0(RW): */
    volatile AMBA_SERDES_RX_CTRL7_REG_s             SerdesRx3Ctrl7;         /* 0x1B4(RW): */
    volatile AMBA_SERDES_RX_CTRL8_REG_s             SerdesRx3Ctrl8;         /* 0x1B8(RW): */
    volatile AMBA_SERDES_RX_CTRL9_REG_s             SerdesRx3Ctrl9;         /* 0x1BC(RW): */
    volatile AMBA_SERDES_RX_CTRL10_REG_s            SerdesRx3Ctrl10;        /* 0x1C0(RW): */
    volatile AMBA_SERDES_RX_CTRL11_REG_s            SerdesRx3Ctrl11;        /* 0x1C4(RW): */
    volatile AMBA_SERDES_RX_CTRL12_REG_s            SerdesRx3Ctrl12;        /* 0x1C8(RW): */
    volatile AMBA_SERDES_RX_CTRL13_REG_s            SerdesRx3Ctrl13;        /* 0x1CC(RW): */
    volatile AMBA_SERDES_RX_CTRL14_REG_s            SerdesRx3Ctrl14;        /* 0x1D0(RW): */
    volatile AMBA_SERDES_RX_CTRL0_REG_s             SerdesRx4Ctrl0;         /* 0x1D4(RW): */
    volatile AMBA_SERDES_RX_CTRL1_REG_s             SerdesRx4Ctrl1;         /* 0x1D8(RW): */
    volatile AMBA_SERDES_RX_CTRL2_REG_s             SerdesRx4Ctrl2;         /* 0x1DC(RW): */
    volatile AMBA_SERDES_RX_CTRL3_REG_s             SerdesRx4Ctrl3;         /* 0x1E0(RW): */
    volatile AMBA_SERDES_RX_CTRL4_REG_s             SerdesRx4Ctrl4;         /* 0x1E4(RW): */
    volatile AMBA_SERDES_RX_CTRL5_REG_s             SerdesRx4Ctrl5;         /* 0x1E8(RW): */
    volatile AMBA_SERDES_RX_CTRL6_REG_s             SerdesRx4Ctrl6;         /* 0x1EC(RW): */
    volatile AMBA_SERDES_RX_CTRL7_REG_s             SerdesRx4Ctrl7;         /* 0x1F0(RW): */
    volatile AMBA_SERDES_RX_CTRL8_REG_s             SerdesRx4Ctrl8;         /* 0x1F4(RW): */
    volatile AMBA_SERDES_RX_CTRL9_REG_s             SerdesRx4Ctrl9;         /* 0x1F8(RW): */
    volatile AMBA_SERDES_RX_CTRL10_REG_s            SerdesRx4Ctrl10;        /* 0x1FC(RW): */
    volatile AMBA_SERDES_RX_CTRL11_REG_s            SerdesRx4Ctrl11;        /* 0x200(RW): */
    volatile AMBA_SERDES_RX_CTRL12_REG_s            SerdesRx4Ctrl12;        /* 0x204(RW): */
    volatile AMBA_SERDES_RX_CTRL13_REG_s            SerdesRx4Ctrl13;        /* 0x208(RW): */
    volatile AMBA_SERDES_RX_CTRL14_REG_s            SerdesRx4Ctrl14;        /* 0x20C(RW): */
    volatile AMBA_SERDES_RX_CTRL0_REG_s             SerdesRx5Ctrl0;         /* 0x210(RW): */
    volatile AMBA_SERDES_RX_CTRL1_REG_s             SerdesRx5Ctrl1;         /* 0x214(RW): */
    volatile AMBA_SERDES_RX_CTRL2_REG_s             SerdesRx5Ctrl2;         /* 0x218(RW): */
    volatile AMBA_SERDES_RX_CTRL3_REG_s             SerdesRx5Ctrl3;         /* 0x21C(RW): */
    volatile AMBA_SERDES_RX_CTRL4_REG_s             SerdesRx5Ctrl4;         /* 0x220(RW): */
    volatile AMBA_SERDES_RX_CTRL5_REG_s             SerdesRx5Ctrl5;         /* 0x224(RW): */
    volatile AMBA_SERDES_RX_CTRL6_REG_s             SerdesRx5Ctrl6;         /* 0x228(RW): */
    volatile AMBA_SERDES_RX_CTRL7_REG_s             SerdesRx5Ctrl7;         /* 0x22C(RW): */
    volatile AMBA_SERDES_RX_CTRL8_REG_s             SerdesRx5Ctrl8;         /* 0x230(RW): */
    volatile AMBA_SERDES_RX_CTRL9_REG_s             SerdesRx5Ctrl9;         /* 0x234(RW): */
    volatile AMBA_SERDES_RX_CTRL10_REG_s            SerdesRx5Ctrl10;        /* 0x238(RW): */
    volatile AMBA_SERDES_RX_CTRL11_REG_s            SerdesRx5Ctrl11;        /* 0x23C(RW): */
    volatile AMBA_SERDES_RX_CTRL12_REG_s            SerdesRx5Ctrl12;        /* 0x240(RW): */
    volatile AMBA_SERDES_RX_CTRL13_REG_s            SerdesRx5Ctrl13;        /* 0x244(RW): */
    volatile AMBA_SERDES_RX_CTRL14_REG_s            SerdesRx5Ctrl14;        /* 0x248(RW): */
    volatile AMBA_SERDES_RX_CTRL0_REG_s             SerdesRx6Ctrl0;         /* 0x24C(RW): */
    volatile AMBA_SERDES_RX_CTRL1_REG_s             SerdesRx6Ctrl1;         /* 0x250(RW): */
    volatile AMBA_SERDES_RX_CTRL2_REG_s             SerdesRx6Ctrl2;         /* 0x254(RW): */
    volatile AMBA_SERDES_RX_CTRL3_REG_s             SerdesRx6Ctrl3;         /* 0x258(RW): */
    volatile AMBA_SERDES_RX_CTRL4_REG_s             SerdesRx6Ctrl4;         /* 0x25C(RW): */
    volatile AMBA_SERDES_RX_CTRL5_REG_s             SerdesRx6Ctrl5;         /* 0x260(RW): */
    volatile AMBA_SERDES_RX_CTRL6_REG_s             SerdesRx6Ctrl6;         /* 0x264(RW): */
    volatile AMBA_SERDES_RX_CTRL7_REG_s             SerdesRx6Ctrl7;         /* 0x268(RW): */
    volatile AMBA_SERDES_RX_CTRL8_REG_s             SerdesRx6Ctrl8;         /* 0x26C(RW): */
    volatile AMBA_SERDES_RX_CTRL9_REG_s             SerdesRx6Ctrl9;         /* 0x270(RW): */
    volatile AMBA_SERDES_RX_CTRL10_REG_s            SerdesRx6Ctrl10;        /* 0x274(RW): */
    volatile AMBA_SERDES_RX_CTRL11_REG_s            SerdesRx6Ctrl11;        /* 0x278(RW): */
    volatile AMBA_SERDES_RX_CTRL12_REG_s            SerdesRx6Ctrl12;        /* 0x27C(RW): */
    volatile AMBA_SERDES_RX_CTRL13_REG_s            SerdesRx6Ctrl13;        /* 0x280(RW): */
    volatile AMBA_SERDES_RX_CTRL14_REG_s            SerdesRx6Ctrl14;        /* 0x284(RW): */
    volatile AMBA_SERDES_RX_CTRL0_REG_s             SerdesRx7Ctrl0;         /* 0x288(RW): */
    volatile AMBA_SERDES_RX_CTRL1_REG_s             SerdesRx7Ctrl1;         /* 0x28C(RW): */
    volatile AMBA_SERDES_RX_CTRL2_REG_s             SerdesRx7Ctrl2;         /* 0x290(RW): */
    volatile AMBA_SERDES_RX_CTRL3_REG_s             SerdesRx7Ctrl3;         /* 0x294(RW): */
    volatile AMBA_SERDES_RX_CTRL4_REG_s             SerdesRx7Ctrl4;         /* 0x298(RW): */
    volatile AMBA_SERDES_RX_CTRL5_REG_s             SerdesRx7Ctrl5;         /* 0x29C(RW): */
    volatile AMBA_SERDES_RX_CTRL6_REG_s             SerdesRx7Ctrl6;         /* 0x2A0(RW): */
    volatile AMBA_SERDES_RX_CTRL7_REG_s             SerdesRx7Ctrl7;         /* 0x2A4(RW): */
    volatile AMBA_SERDES_RX_CTRL8_REG_s             SerdesRx7Ctrl8;         /* 0x2A8(RW): */
    volatile AMBA_SERDES_RX_CTRL9_REG_s             SerdesRx7Ctrl9;         /* 0x2AC(RW): */
    volatile AMBA_SERDES_RX_CTRL10_REG_s            SerdesRx7Ctrl10;        /* 0x2B0(RW): */
    volatile AMBA_SERDES_RX_CTRL11_REG_s            SerdesRx7Ctrl11;        /* 0x2B4(RW): */
    volatile AMBA_SERDES_RX_CTRL12_REG_s            SerdesRx7Ctrl12;        /* 0x2B8(RW): */
    volatile AMBA_SERDES_RX_CTRL13_REG_s            SerdesRx7Ctrl13;        /* 0x2BC(RW): */
    volatile AMBA_SERDES_RX_CTRL14_REG_s            SerdesRx7Ctrl14;        /* 0x2C0(RW): */
    volatile AMBA_SERDES_RX_CTRL0_REG_s             SerdesRx8Ctrl0;         /* 0x2C4(RW): */
    volatile AMBA_SERDES_RX_CTRL1_REG_s             SerdesRx8Ctrl1;         /* 0x2C8(RW): */
    volatile AMBA_SERDES_RX_CTRL2_REG_s             SerdesRx8Ctrl2;         /* 0x2CC(RW): */
    volatile AMBA_SERDES_RX_CTRL3_REG_s             SerdesRx8Ctrl3;         /* 0x2D0(RW): */
    volatile AMBA_SERDES_RX_CTRL4_REG_s             SerdesRx8Ctrl4;         /* 0x2D4(RW): */
    volatile AMBA_SERDES_RX_CTRL5_REG_s             SerdesRx8Ctrl5;         /* 0x2D8(RW): */
    volatile AMBA_SERDES_RX_CTRL6_REG_s             SerdesRx8Ctrl6;         /* 0x2DC(RW): */
    volatile AMBA_SERDES_RX_CTRL7_REG_s             SerdesRx8Ctrl7;         /* 0x2E0(RW): */
    volatile AMBA_SERDES_RX_CTRL8_REG_s             SerdesRx8Ctrl8;         /* 0x2E4(RW): */
    volatile AMBA_SERDES_RX_CTRL9_REG_s             SerdesRx8Ctrl9;         /* 0x2E8(RW): */
    volatile AMBA_SERDES_RX_CTRL10_REG_s            SerdesRx8Ctrl10;        /* 0x2EC(RW): */
    volatile AMBA_SERDES_RX_CTRL11_REG_s            SerdesRx8Ctrl11;        /* 0x2F0(RW): */
    volatile AMBA_SERDES_RX_CTRL12_REG_s            SerdesRx8Ctrl12;        /* 0x2F4(RW): */
    volatile AMBA_SERDES_RX_CTRL13_REG_s            SerdesRx8Ctrl13;        /* 0x2F8(RW): */
    volatile AMBA_SERDES_RX_CTRL14_REG_s            SerdesRx8Ctrl14;        /* 0x2FC(RW): */
    volatile AMBA_SERDES_RX_CTRL0_REG_s             SerdesRx9Ctrl0;         /* 0x300(RW): */
    volatile AMBA_SERDES_RX_CTRL1_REG_s             SerdesRx9Ctrl1;         /* 0x304(RW): */
    volatile AMBA_SERDES_RX_CTRL2_REG_s             SerdesRx9Ctrl2;         /* 0x308(RW): */
    volatile AMBA_SERDES_RX_CTRL3_REG_s             SerdesRx9Ctrl3;         /* 0x30C(RW): */
    volatile AMBA_SERDES_RX_CTRL4_REG_s             SerdesRx9Ctrl4;         /* 0x310(RW): */
    volatile AMBA_SERDES_RX_CTRL5_REG_s             SerdesRx9Ctrl5;         /* 0x314(RW): */
    volatile AMBA_SERDES_RX_CTRL6_REG_s             SerdesRx9Ctrl6;         /* 0x318(RW): */
    volatile AMBA_SERDES_RX_CTRL7_REG_s             SerdesRx9Ctrl7;         /* 0x31C(RW): */
    volatile AMBA_SERDES_RX_CTRL8_REG_s             SerdesRx9Ctrl8;         /* 0x320(RW): */
    volatile AMBA_SERDES_RX_CTRL9_REG_s             SerdesRx9Ctrl9;         /* 0x324(RW): */
    volatile AMBA_SERDES_RX_CTRL10_REG_s            SerdesRx9Ctrl10;        /* 0x328(RW): */
    volatile AMBA_SERDES_RX_CTRL11_REG_s            SerdesRx9Ctrl11;        /* 0x32C(RW): */
    volatile AMBA_SERDES_RX_CTRL12_REG_s            SerdesRx9Ctrl12;        /* 0x330(RW): */
    volatile AMBA_SERDES_RX_CTRL13_REG_s            SerdesRx9Ctrl13;        /* 0x334(RW): */
    volatile AMBA_SERDES_RX_CTRL14_REG_s            SerdesRx9Ctrl14;        /* 0x338(RW): */
    volatile AMBA_SERDES_RX_CTRL0_REG_s             SerdesRx10Ctrl0;        /* 0x33C(RW): */
    volatile AMBA_SERDES_RX_CTRL1_REG_s             SerdesRx10Ctrl1;        /* 0x340(RW): */
    volatile AMBA_SERDES_RX_CTRL2_REG_s             SerdesRx10Ctrl2;        /* 0x344(RW): */
    volatile AMBA_SERDES_RX_CTRL3_REG_s             SerdesRx10Ctrl3;        /* 0x348(RW): */
    volatile AMBA_SERDES_RX_CTRL4_REG_s             SerdesRx10Ctrl4;        /* 0x34C(RW): */
    volatile AMBA_SERDES_RX_CTRL5_REG_s             SerdesRx10Ctrl5;        /* 0x350(RW): */
    volatile AMBA_SERDES_RX_CTRL6_REG_s             SerdesRx10Ctrl6;        /* 0x354(RW): */
    volatile AMBA_SERDES_RX_CTRL7_REG_s             SerdesRx10Ctrl7;        /* 0x358(RW): */
    volatile AMBA_SERDES_RX_CTRL8_REG_s             SerdesRx10Ctrl8;        /* 0x35C(RW): */
    volatile AMBA_SERDES_RX_CTRL9_REG_s             SerdesRx10Ctrl9;        /* 0x360(RW): */
    volatile AMBA_SERDES_RX_CTRL10_REG_s            SerdesRx10Ctrl10;       /* 0x364(RW): */
    volatile AMBA_SERDES_RX_CTRL11_REG_s            SerdesRx10Ctrl11;       /* 0x368(RW): */
    volatile AMBA_SERDES_RX_CTRL12_REG_s            SerdesRx10Ctrl12;       /* 0x36C(RW): */
    volatile AMBA_SERDES_RX_CTRL13_REG_s            SerdesRx10Ctrl13;       /* 0x370(RW): */
    volatile AMBA_SERDES_RX_CTRL14_REG_s            SerdesRx10Ctrl14;       /* 0x374(RW): */
    volatile AMBA_SERDES_RX_CTRL0_REG_s             SerdesRx11Ctrl0;        /* 0x378(RW): */
    volatile AMBA_SERDES_RX_CTRL1_REG_s             SerdesRx11Ctrl1;        /* 0x37C(RW): */
    volatile AMBA_SERDES_RX_CTRL2_REG_s             SerdesRx11Ctrl2;        /* 0x380(RW): */
    volatile AMBA_SERDES_RX_CTRL3_REG_s             SerdesRx11Ctrl3;        /* 0x384(RW): */
    volatile AMBA_SERDES_RX_CTRL4_REG_s             SerdesRx11Ctrl4;        /* 0x388(RW): */
    volatile AMBA_SERDES_RX_CTRL5_REG_s             SerdesRx11Ctrl5;        /* 0x38C(RW): */
    volatile AMBA_SERDES_RX_CTRL6_REG_s             SerdesRx11Ctrl6;        /* 0x390(RW): */
    volatile AMBA_SERDES_RX_CTRL7_REG_s             SerdesRx11Ctrl7;        /* 0x394(RW): */
    volatile AMBA_SERDES_RX_CTRL8_REG_s             SerdesRx11Ctrl8;        /* 0x398(RW): */
    volatile AMBA_SERDES_RX_CTRL9_REG_s             SerdesRx11Ctrl9;        /* 0x39C(RW): */
    volatile AMBA_SERDES_RX_CTRL10_REG_s            SerdesRx11Ctrl10;       /* 0x3A0(RW): */
    volatile AMBA_SERDES_RX_CTRL11_REG_s            SerdesRx11Ctrl11;       /* 0x3A4(RW): */
    volatile AMBA_SERDES_RX_CTRL12_REG_s            SerdesRx11Ctrl12;       /* 0x3A8(RW): */
    volatile AMBA_SERDES_RX_CTRL13_REG_s            SerdesRx11Ctrl13;       /* 0x3AC(RW): */
    volatile AMBA_SERDES_RX_CTRL14_REG_s            SerdesRx11Ctrl14;       /* 0x3B0(RW): */
    volatile AMBA_SLVSEC_PHY_OBSV0_REG_s            SlvsEcPhy0Obsv0;        /* 0x3B4(RW): */
    volatile AMBA_SLVSEC_PHY_OBSV1_REG_s            SlvsEcPhy0Obsv1;        /* 0x3B8(RW): */
    volatile AMBA_SLVSEC_PHY_OBSV2_REG_s            SlvsEcPhy0Obsv2;        /* 0x3BC(RW): */
    volatile AMBA_SLVSEC_PHY_OBSV3_REG_s            SlvsEcPhy0Obsv3;        /* 0x3C0(RW): */
    volatile AMBA_SLVSEC_PHY_OBSV0_REG_s            SlvsEcPhy1Obsv0;        /* 0x3C4(RW): */
    volatile AMBA_SLVSEC_PHY_OBSV1_REG_s            SlvsEcPhy1Obsv1;        /* 0x3C8(RW): */
    volatile AMBA_SLVSEC_PHY_OBSV2_REG_s            SlvsEcPhy1Obsv2;        /* 0x3CC(RW): */
    volatile AMBA_SLVSEC_PHY_OBSV3_REG_s            SlvsEcPhy1Obsv3;        /* 0x3D0(RW): */
    volatile AMBA_SLVSEC_PHY_OBSV0_REG_s            SlvsEcPhy2Obsv0;        /* 0x3D4(RW): */
    volatile AMBA_SLVSEC_PHY_OBSV1_REG_s            SlvsEcPhy2Obsv1;        /* 0x3D8(RW): */
    volatile AMBA_SLVSEC_PHY_OBSV2_REG_s            SlvsEcPhy2Obsv2;        /* 0x3DC(RW): */
    volatile AMBA_SLVSEC_PHY_OBSV3_REG_s            SlvsEcPhy2Obsv3;        /* 0x3E0(RW): */
    volatile AMBA_SLVSEC_PHY_OBSV0_REG_s            SlvsEcPhy3Obsv0;        /* 0x3E4(RW): */
    volatile AMBA_SLVSEC_PHY_OBSV1_REG_s            SlvsEcPhy3Obsv1;        /* 0x3E8(RW): */
    volatile AMBA_SLVSEC_PHY_OBSV2_REG_s            SlvsEcPhy3Obsv2;        /* 0x3EC(RW): */
    volatile AMBA_SLVSEC_PHY_OBSV3_REG_s            SlvsEcPhy3Obsv3;        /* 0x3F0(RW): */
    volatile AMBA_SLVSEC_PHY_OBSV0_REG_s            SlvsEcPhy4Obsv0;        /* 0x3F4(RW): */
    volatile AMBA_SLVSEC_PHY_OBSV1_REG_s            SlvsEcPhy4Obsv1;        /* 0x3F8(RW): */
    volatile AMBA_SLVSEC_PHY_OBSV2_REG_s            SlvsEcPhy4Obsv2;        /* 0x3FC(RW): */
    volatile AMBA_SLVSEC_PHY_OBSV3_REG_s            SlvsEcPhy4Obsv3;        /* 0x400(RW): */
    volatile AMBA_SLVSEC_PHY_OBSV0_REG_s            SlvsEcPhy5Obsv0;        /* 0x404(RW): */
    volatile AMBA_SLVSEC_PHY_OBSV1_REG_s            SlvsEcPhy5Obsv1;        /* 0x408(RW): */
    volatile AMBA_SLVSEC_PHY_OBSV2_REG_s            SlvsEcPhy5Obsv2;        /* 0x40C(RW): */
    volatile AMBA_SLVSEC_PHY_OBSV3_REG_s            SlvsEcPhy5Obsv3;        /* 0x410(RW): */
    volatile AMBA_SLVSEC_PHY_OBSV0_REG_s            SlvsEcPhy6Obsv0;        /* 0x414(RW): */
    volatile AMBA_SLVSEC_PHY_OBSV1_REG_s            SlvsEcPhy6Obsv1;        /* 0x418(RW): */
    volatile AMBA_SLVSEC_PHY_OBSV2_REG_s            SlvsEcPhy6Obsv2;        /* 0x41C(RW): */
    volatile AMBA_SLVSEC_PHY_OBSV3_REG_s            SlvsEcPhy6Obsv3;        /* 0x420(RW): */
    volatile AMBA_SLVSEC_PHY_OBSV0_REG_s            SlvsEcPhy7Obsv0;        /* 0x424(RW): */
    volatile AMBA_SLVSEC_PHY_OBSV1_REG_s            SlvsEcPhy7Obsv1;        /* 0x428(RW): */
    volatile AMBA_SLVSEC_PHY_OBSV2_REG_s            SlvsEcPhy7Obsv2;        /* 0x42C(RW): */
    volatile AMBA_SLVSEC_PHY_OBSV3_REG_s            SlvsEcPhy7Obsv3;        /* 0x430(RW): */
    volatile AMBA_SLVSEC_PHY_OBSV0_REG_s            SlvsEcPhy8Obsv0;        /* 0x434(RW): */
    volatile AMBA_SLVSEC_PHY_OBSV1_REG_s            SlvsEcPhy8Obsv1;        /* 0x438(RW): */
    volatile AMBA_SLVSEC_PHY_OBSV2_REG_s            SlvsEcPhy8Obsv2;        /* 0x43C(RW): */
    volatile AMBA_SLVSEC_PHY_OBSV3_REG_s            SlvsEcPhy8Obsv3;        /* 0x440(RW): */
    volatile AMBA_SLVSEC_PHY_OBSV0_REG_s            SlvsEcPhy9Obsv0;        /* 0x444(RW): */
    volatile AMBA_SLVSEC_PHY_OBSV1_REG_s            SlvsEcPhy9Obsv1;        /* 0x448(RW): */
    volatile AMBA_SLVSEC_PHY_OBSV2_REG_s            SlvsEcPhy9Obsv2;        /* 0x44C(RW): */
    volatile AMBA_SLVSEC_PHY_OBSV3_REG_s            SlvsEcPhy9Obsv3;        /* 0x450(RW): */
    volatile AMBA_SLVSEC_PHY_OBSV0_REG_s            SlvsEcPhy10Obsv0;       /* 0x454(RW): */
    volatile AMBA_SLVSEC_PHY_OBSV1_REG_s            SlvsEcPhy10Obsv1;       /* 0x458(RW): */
    volatile AMBA_SLVSEC_PHY_OBSV2_REG_s            SlvsEcPhy10Obsv2;       /* 0x45C(RW): */
    volatile AMBA_SLVSEC_PHY_OBSV3_REG_s            SlvsEcPhy10Obsv3;       /* 0x460(RW): */
    volatile AMBA_SLVSEC_PHY_OBSV0_REG_s            SlvsEcPhy11Obsv0;       /* 0x464(RW): */
    volatile AMBA_SLVSEC_PHY_OBSV1_REG_s            SlvsEcPhy11Obsv1;       /* 0x468(RW): */
    volatile AMBA_SLVSEC_PHY_OBSV2_REG_s            SlvsEcPhy11Obsv2;       /* 0x46C(RW): */
    volatile AMBA_SLVSEC_PHY_OBSV3_REG_s            SlvsEcPhy11Obsv3;       /* 0x470(RW): */
    volatile AMBA_SLVSEC_PHY_CTRL0_REG_s            SlvsEcPhy0Ctrl0;        /* 0x474(RW): */
    volatile AMBA_SLVSEC_PHY_CTRL1_REG_s            SlvsEcPhy0Ctrl1;        /* 0x478(RW): */
    volatile AMBA_SLVSEC_PHY_CTRL2_REG_s            SlvsEcPhy0Ctrl2;        /* 0x47C(RW): */
    volatile AMBA_SLVSEC_PHY_CTRL0_REG_s            SlvsEcPhy1Ctrl0;        /* 0x480(RW): */
    volatile AMBA_SLVSEC_PHY_CTRL1_REG_s            SlvsEcPhy1Ctrl1;        /* 0x484(RW): */
    volatile AMBA_SLVSEC_PHY_CTRL2_REG_s            SlvsEcPhy1Ctrl2;        /* 0x488(RW): */
    volatile AMBA_SLVSEC_PHY_CTRL0_REG_s            SlvsEcPhy2Ctrl0;        /* 0x48C(RW): */
    volatile AMBA_SLVSEC_PHY_CTRL1_REG_s            SlvsEcPhy2Ctrl1;        /* 0x490(RW): */
    volatile AMBA_SLVSEC_PHY_CTRL2_REG_s            SlvsEcPhy2Ctrl2;        /* 0x494(RW): */
    volatile AMBA_SLVSEC_PHY_CTRL0_REG_s            SlvsEcPhy3Ctrl0;        /* 0x498(RW): */
    volatile AMBA_SLVSEC_PHY_CTRL1_REG_s            SlvsEcPhy3Ctrl1;        /* 0x49C(RW): */
    volatile AMBA_SLVSEC_PHY_CTRL2_REG_s            SlvsEcPhy3Ctrl2;        /* 0x4A0(RW): */
    volatile AMBA_SLVSEC_PHY_CTRL0_REG_s            SlvsEcPhy4Ctrl0;        /* 0x4A4(RW): */
    volatile AMBA_SLVSEC_PHY_CTRL1_REG_s            SlvsEcPhy4Ctrl1;        /* 0x4A8(RW): */
    volatile AMBA_SLVSEC_PHY_CTRL2_REG_s            SlvsEcPhy4Ctrl2;        /* 0x4AC(RW): */
    volatile AMBA_SLVSEC_PHY_CTRL0_REG_s            SlvsEcPhy5Ctrl0;        /* 0x4B0(RW): */
    volatile AMBA_SLVSEC_PHY_CTRL1_REG_s            SlvsEcPhy5Ctrl1;        /* 0x4B4(RW): */
    volatile AMBA_SLVSEC_PHY_CTRL2_REG_s            SlvsEcPhy5Ctrl2;        /* 0x4B8(RW): */
    volatile AMBA_SLVSEC_PHY_CTRL0_REG_s            SlvsEcPhy6Ctrl0;        /* 0x4BC(RW): */
    volatile AMBA_SLVSEC_PHY_CTRL1_REG_s            SlvsEcPhy6Ctrl1;        /* 0x4C0(RW): */
    volatile AMBA_SLVSEC_PHY_CTRL2_REG_s            SlvsEcPhy6Ctrl2;        /* 0x4C4(RW): */
    volatile AMBA_SLVSEC_PHY_CTRL0_REG_s            SlvsEcPhy7Ctrl0;        /* 0x4C8(RW): */
    volatile AMBA_SLVSEC_PHY_CTRL1_REG_s            SlvsEcPhy7Ctrl1;        /* 0x4CC(RW): */
    volatile AMBA_SLVSEC_PHY_CTRL2_REG_s            SlvsEcPhy7Ctrl2;        /* 0x4D0(RW): */
    volatile AMBA_SLVSEC_PHY_CTRL0_REG_s            SlvsEcPhy8Ctrl0;        /* 0x4D4(RW): */
    volatile AMBA_SLVSEC_PHY_CTRL1_REG_s            SlvsEcPhy8Ctrl1;        /* 0x4D8(RW): */
    volatile AMBA_SLVSEC_PHY_CTRL2_REG_s            SlvsEcPhy8Ctrl2;        /* 0x4DC(RW): */
    volatile AMBA_SLVSEC_PHY_CTRL0_REG_s            SlvsEcPhy9Ctrl0;        /* 0x4E0(RW): */
    volatile AMBA_SLVSEC_PHY_CTRL1_REG_s            SlvsEcPhy9Ctrl1;        /* 0x4E4(RW): */
    volatile AMBA_SLVSEC_PHY_CTRL2_REG_s            SlvsEcPhy9Ctrl2;        /* 0x4E8(RW): */
    volatile AMBA_SLVSEC_PHY_CTRL0_REG_s            SlvsEcPhy10Ctrl0;       /* 0x4EC(RW): */
    volatile AMBA_SLVSEC_PHY_CTRL1_REG_s            SlvsEcPhy10Ctrl1;       /* 0x4F0(RW): */
    volatile AMBA_SLVSEC_PHY_CTRL2_REG_s            SlvsEcPhy10Ctrl2;       /* 0x4F4(RW): */
    volatile AMBA_SLVSEC_PHY_CTRL0_REG_s            SlvsEcPhy11Ctrl0;       /* 0x4F8(RW): */
    volatile AMBA_SLVSEC_PHY_CTRL1_REG_s            SlvsEcPhy11Ctrl1;       /* 0x4FC(RW): */
    volatile AMBA_SLVSEC_PHY_CTRL2_REG_s            SlvsEcPhy11Ctrl2;       /* 0x500(RW): */
    volatile AMBA_DCPHY_RCT_OBSV0_REG_s             DCphy0RctObsv0;         /* 0x504(RW): */
    volatile AMBA_DCPHY_RCT_OBSV1_REG_s             DCphy0RctObsv1;         /* 0x508(RW): */
    volatile AMBA_DCPHY_RCT_OBSV2_REG_s             DCphy0RctObsv2;         /* 0x50C(RW): */
    volatile AMBA_DCPHY_RCT_OBSV0_REG_s             DCphy1RctObsv0;         /* 0x510(RW): */
    volatile AMBA_DCPHY_RCT_OBSV1_REG_s             DCphy1RctObsv1;         /* 0x514(RW): */
    volatile AMBA_DCPHY_RCT_OBSV2_REG_s             DCphy1RctObsv2;         /* 0x518(RW): */
    volatile UINT32                                 DCphy0RctCtrl0;         /* 0x51C(RW): AMBA_DCPHY_RCT_CTRL0_REG_s */
    volatile AMBA_DCPHY_RCT_CTRL1_REG_s             DCphy0RctCtrl1;         /* 0x520(RW): */
    volatile AMBA_DCPHY_RCT_CTRL2_REG_s             DCphy0RctCtrl2;         /* 0x524(RW): */
    volatile UINT32                                 DCphy1RctCtrl0;         /* 0x528(RW): AMBA_DCPHY_RCT_CTRL0_REG_s */
    volatile AMBA_DCPHY_RCT_CTRL1_REG_s             DCphy1RctCtrl1;         /* 0x52C(RW): */
    volatile AMBA_DCPHY_RCT_CTRL2_REG_s             DCphy1RctCtrl2;         /* 0x530(RW): */
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
