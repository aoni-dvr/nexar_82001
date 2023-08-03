/*
 * Copyright (c) 2020 Ambarella International LP
 *
 * This file and its contents ("Software") are protected by intellectual
 * property rights including, without limitation, U.S. and/or foreign
 * copyrights. This Software is also the confidential and proprietary
 * information of Ambarella International LP and its licensors. You may not use, reproduce,
 * disclose, distribute, modify, or otherwise prepare derivative works of this
 * Software or any portion thereof except pursuant to a signed license agreement
 * or nondisclosure agreement with Ambarella International LP or its authorized affiliates.
 * In the absence of such an agreement, you agree to promptly notify and return
 * this Software to Ambarella International LP.
 *
 * THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF NON-INFRINGEMENT,
 * MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL AMBARELLA INTERNATIONAL LP OR ITS AFFILIATES BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; COMPUTER FAILURE OR MALFUNCTION; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef AMBA_REG_RCT_H
#define AMBA_REG_RCT_H

#ifndef AMBA_TYPES_H
#include "AmbaTypes.h"
#endif

/**
 *  Definitions for power-on configuration
 */
typedef struct {
    UINT32  Reserved0:                  1;      /* [0] Unused */
    UINT32  ClkConfig:                  3;      /* [3:1] Power-On Reset IDSP/Core/DDR/Vision clock configuration */
    UINT32  BootMode:                   2;      /* [5:4] Boot mode select [1:0] */
    UINT32  SecureBoot:                 1;      /* [6] 0 = Non-Secure boot, 1 = Secure boot */
    UINT32  Reserved1:                  1;      /* [7] Unused */
    UINT32  BootBypass:                 1;      /* [8] 1 = Bypass fetching boot code from external device */
    UINT32  Reserved2:                  1;      /* [9] Unused */
    UINT32  UsbBoot:                    1;      /* [10] (from PIN) 0 = Non-USB boot, 1 = USB boot */
    UINT32  Reserved3:                  2;      /* [12:11] Unused */
    UINT32  BootOption:                 8;      /* [20:13] Boot mode option [7:0] */
    UINT32  Reserved4:                  2;      /* [22:21] Unused */
    UINT32  ClkSource:                  2;      /* [24:23] (from OTP) Clock source for core & ddr clock: 0 = Normal, 1 = Ref clock, 2 = Reserved, 3 = Reserved */
    UINT32  Reserved5:                  1;      /* [25] Unused */
    UINT32  PerifClkSource:             1;      /* [26] (from OTP) Clock source of UsbHost/Sd/Nand controller (must < 466Mhz). 0 = gclk_core/2, 1 = gclk_core */
    UINT32  Reserved6:                  4;      /* [30:27] Unused */
    UINT32  SysConfigSource:            1;      /* [31] (from PIN) 0 = sys_config derived from pins (except bit[23,24,26]), 1 = sys_config derived from OTP (except bit[10,31]) */
} AMBA_SYS_CONFIG_REG_s;

/**
 *  Definitions for software reset contorl
 */
typedef struct {
    UINT32  ResetChip:                  1;      /* [0] 1 = Reset entire chip (except RCT), 0 = De-assert reset */
    UINT32  ResetDll0:                  1;      /* [1] 0 = Reset ddrio0, 1 = De-assert reset */
    UINT32  ResetDll1:                  1;      /* [2] 0 = Reset ddrio1, 1 = De-assert reset */
    UINT32  ResetVout:                  1;      /* [3] 0 = Reset vout, 1 = De-assert reset */
    UINT32  Reserved1:                  28;     /* [31:4] Reserved */
} AMBA_SYS_RESET0_REG_s;

typedef struct {
    UINT32  DdrcReset:                  1;      /* [0] 1 = reset level signal for ddrc */
    UINT32  Axi0Reset:                  1;      /* [1] 1 = generate a reset pulse for axi0 */
    UINT32  Axi1Reset:                  1;      /* [2] 1 = generate a reset pulse for axi1 */
    UINT32  IdspReset:                  1;      /* [3] 1 = reset level signal for idsp */
    UINT32  Vdsp1Reset:                 1;      /* [4] 1 = reset level signal for vdsp1 */
    UINT32  Reserved0:                  2;      /* [6:5] Reserved */
    UINT32  StorcReset:                 1;      /* [7] 1 = reset level signal for storc */
    UINT32  Vmem0Reset:                 1;      /* [8] 1 = reset level signal for vmem0 */
    UINT32  Vp0Reset:                   1;      /* [9] 1 = reset level signal for vp0 */
    UINT32  Reserved1:                  22;     /* [31:10] Reserved */
} AMBA_SYS_RESET1_REG_s;

typedef struct {
    UINT32  FlashReset:                 1;      /* [0] 1 = Reset flash controller, 0 = De-assert reset */
    UINT32  Reserved0:                  2;      /* [2:1] Reserved */
    UINT32  FioReset:                   1;      /* [3] 1 = Reset fio controller, 0 = De-assert reset */
    UINT32  Reserved1:                  28;     /* [31:4] Reserved */
} AMBA_NAND_RESET_REG_s;

typedef struct {
    UINT32  WdtResetOccur:              1;      /* [0] 0 = Not WDT reset, 1 = WDT reset. */
    UINT32  Ca53Wdt0ResetOccur:         1;      /* [1] CA53 core0 WDT state: 0 = Not WDT reset, 1 = WDT reset. */
    UINT32  Ca53Wdt1ResetOccur:         1;      /* [2] CA53 core1 WDT state: 0 = Not WDT reset, 1 = WDT reset. */
    UINT32  Ca53Wdt2ResetOccur:         1;      /* [3] CA53 core2 WDT state: 0 = Not WDT reset, 1 = WDT reset. */
    UINT32  Ca53Wdt3ResetOccur:         1;      /* [4] CA53 core3 WDT state: 0 = Not WDT reset, 1 = WDT reset. */
    UINT32  Cr52WdtResetOccur:          1;      /* [5] CR52 WDT state: 0 = Not WDT reset, 1 = WDT reset. (Write 1 to clear) */
    UINT32  Reserved:                   26;     /* [31:6] Reserved */
} AMBA_WDT_RESET_STATE_REG_s;

typedef struct {
    UINT32  HostReset:                  1;      /* [0] Soft reset for Host Controller */
    UINT32  DeviceReset:                1;      /* [1] Soft reset for Device Controller */
    UINT32  Reserved:                   30;     /* [31:2] Reserved */
} AMBA_USB_RESET_CTRL_REG_s;

/**
 *  Definitions for clock source tree configuration
 */
typedef struct {
    UINT32  ClkRefSel:                  1;      /* [0] PLL reference clock selection: 0 = use 24 MHz crystal, 1 = use external clock source */
    UINT32  Reserved:                   31;     /* [31:1] Reserved */
} AMBA_PLL_REF_CLK_SELECT_REG_s;

typedef struct {
    UINT32  ExtClkSrc:                  2;      /* [1:0] 2'b01 = xx_clk_si, 2'b10 = muxed_lvds_spclk / 4, others = no clock */
    UINT32  Reserved:                   30;     /* [31:2] Reserved */
} AMBA_PLL_EXT_CLK_SRC_REG_s;

typedef struct {
    UINT32  AudioClkSelect:             1;      /* [0] 0 = gclk_au is from on-chip pll output, 1 = gclk_au is from external pin IOPAD_APB_CLK_AU_C */
    UINT32  Reserved:                   31;     /* [31:1] Reserved */
} AMBA_AUDIO_EXT_CLK_CTRL_REG_s;

typedef struct {
    UINT32  ClkSrc:                     2;      /* [1:0] CAN clock source: 0 = clk_ref, 1 = pll_out_enet, 2 = gclk_ahb, 3 = pll_out_sd */
    UINT32  Reserved:                   30;     /* [31:2] Reserved */
} AMBA_CAN_CLK_SRC_SELECT_REG_s;

typedef struct {
    UINT32  ClkSrc:                     2;      /* [1:0] 0 = Default, 1 = Use ref_clk, 2/3 = Reserved */
    UINT32  Reserved:                   30;     /* [31:2] reserved */
} AMBA_CORTEX_CLK_SRC_SELECT_REG_s;

typedef struct {
    UINT32  ClkSrc:                     2;      /* [1:0] 0 = pll_out_fex, 1 = pll_out_enet, 2 = clk_ref, 3 = Reserved */
    UINT32  Reserved:                   30;     /* [31:2] reserved */
} AMBA_IST_CLK_SRC_SELECT_REG_s;

typedef struct {
    UINT32  ClkSrc:                     2;      /* [1:0] 0 = pll_out_fex, 1 = pll_out_enet, 2 = clk_ref, 3 = Reserved */
    UINT32  Reserved:                   30;     /* [31:2] reserved */
} AMBA_LBIST_CLK_SRC_SELECT_REG_s;

typedef struct {
    UINT32  ClkSrc:                     2;      /* [1:0] PWM clock source: 0 = gclk_apb, 1 = gclk_cortex, 2 = pll_out_enet, 3 = clk_ref */
    UINT32  Reserved:                   30;     /* [31:2] Reserved */
} AMBA_PWM_CLK_SRC_SELECT_REG_s;

typedef struct {
    UINT32  Sd0ClkSrc:                  1;      /* [0] gclk_sd48 clock divider source, 0 = pll_out_sd, 1 = pll_out_core */
    UINT32  Reserved0:                  1;      /* [1] reserved */
    UINT32  Sdio0ClkSrc:                1;      /* [2] gclk_sdio0 clock divider source, 0 = pll_out_sd, 1 = pll_out_core */
    UINT32  Reserved1:                  29;     /* [31:3] reserved */
} AMBA_SD_CLK_SRC_SELECT_REG_s;

typedef struct {
    UINT32  ClkSrc:                     1;      /* [0] SPI clock source: 1 = gclk_core, others = no clock */
    UINT32  Reserved:                   31;     /* [31:1] Reserved */
} AMBA_SPI_CLK_REF_SELECT_REG_s;

typedef struct {
    UINT32  ClkSrc:                     2;      /* [1:0] SPI_Nor clock source: 2'b00 = pll_out_enet, 2'b01 = gclk_core, 2'b10 = gclk_ssi, 2'b01 = clk_ref */
    UINT32  Reserved:                   30;     /* [31:2] Reserved */
} AMBA_SPI_NOR_CLK_REF_SELECT_REG_s;

typedef struct {
    UINT32  ClkSrc:                     2;      /* [1:0] UART clock source: 0 = clk_ref, 1 = gclk_core, 2 = pll_out_enet, 3 = pll_out_sd */
    UINT32  Reserved:                   30;     /* [31:2] Reserved */
} AMBA_UART_CLK_SRC_SELECT_REG_s;

typedef struct {
    UINT32  VinRefClkSelect:            1;      /* [0] vin_ref_clk_sel */
    UINT32  Reserved:                   31;     /* [31:1] Reserved */
} AMBA_VIN_CLK_SELECT_REG_s;

typedef struct {
    UINT32  Vout0BypassSelect:          1;      /* [0] vout_a_clk_ref_sel */
    UINT32  ComboPhy0ClkSelect:         1;      /* [1] mipi_dsi_a_clk_in_sel */
    UINT32  Vout0ClkSelect:             1;      /* [2] vout_clk_vo_a_sel */
    UINT32  Reserved0:                  5;      /* [7:3] Reserved */
    UINT32  Vout1BypassSelect:          1;      /* [8] vout_b_clk_ref_sel */
    UINT32  ComboPhy1ClkSelect:         1;      /* [9] mipi_dsi_b_clk_in_sel */
    UINT32  Vout1ClkSelect:             1;      /* [10] vout_clk_vo_b_sel */
    UINT32  Reserved1:                  21;     /* [31:11] Reserved */
} AMBA_VOUT_CLK_SELECT_REG_s;

/* Muxed LVDS Clock Selection Register */
typedef struct {
    UINT32  ClkSrc:                     3;      /* [2:0] 0x1 = spclk_c[0], 0x2 = spclk_c[1], 0x4 = spclk_c[2] */
    UINT32  Reserved:                   29;     /* [31:3] Reserved */
} AMBA_MUX_LVDS_SPCLK_SELECT_REG_s;

/* Muxed MIPI RX Byte Clock HS Selection */
typedef struct {
    UINT32  ClkSrc:                     3;      /* [2:0] 0x1 = mipi_rxbyteclkhs[0], 0x2 = mipi_rxbyteclkhs[1], 0x4 = mipi_rxbyteclkhs[2] */
    UINT32  Reserved:                   29;     /* [31:3] Reserved */
} AMBA_MUX_MIPI_RX_BYTE_CLK_SELECT_REG_s;

/**
 *  Definitions for clock related configuration
 */

/* pll Control Registers */
typedef struct {
    UINT32  WriteEnable:                1;      /* [0] Write 0 and then write 1 to make the new pll configurations take effect */
    UINT32  Reserved0:                  1;      /* [1] Shall be zero. */
    UINT32  PllBypass:                  1;      /* [2] 1 = Bypass pll logic and use CLK_REF instead */
    UINT32  PllOpMode:                  1;      /* [3] 0 = integer-N mode, 1 = fractional-N mode */
    UINT32  PllReset:                   1;      /* [4] Write 0 and then write 1 to trigger pll reset */
    UINT32  PowerDown:                  1;      /* [5] 1 = Gate pll output (Only for Vin and Vout pll) */
    UINT32  HaltVCO:                    1;      /* [6] 1 = Halt VCO oscillation */
    UINT32  TristateLoopFilter:         1;      /* [7] 1 = Enable tristate loop filter */
    UINT32  PllToutAsync:               4;      /* [11:8] Selection for pll_tout_async */
    UINT32  Sdiv:                       4;      /* [15:12] SDIV = Slow Divider */
    UINT32  Sout:                       4;      /* [19:16] SOUT = Scaler for PLL Output */
    UINT32  PllLock:                    1;      /* [20] 1 = Do not switch to CLK_REF when the new pll configurations take effect */
    UINT32  Reserved1:                  1;      /* [21] Shall be zero */
    UINT32  DsmReset:                   1;      /* [22] 1 = Reset delta-sigma modulator */
    UINT32  Reserved2:                  1;      /* [23] Shall be zero */
    UINT32  IntegerPart:                7;      /* [30:24] Integer part of either integer-N or fractional-N*/
    UINT32  Reserved3:                  1;      /* [31] Shall be zero */
} AMBA_PLL_CTRL_REG_s;

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
    UINT32  BypassJDiv:                 1;      /* [20] Bypass JDIV */
    UINT32  BypassJDout:                1;      /* [21] Bypass JDOUT */
    UINT32  Reserved3:                  10;     /* [31:22] */
} AMBA_PLL_CTRL3_REG_s;

/* Clock Divider Registers */
typedef struct {
    UINT32  ClkDivider:                 24;     /* [23:0] Divider */
    UINT32  Reset:                      1;      /* [24] 1 = Reset divider, 0 = De-assert divider reset */
    UINT32  Reserved:                   7;      /* [31:25] Reserved */
} AMBA_PLL_DIVIDER_24BIT_REG_s;

typedef struct {
    UINT32  ClkDivider:                 16;     /* [15:0] Integer Div Value */
    UINT32  Reset:                      1;      /* [16] 1 = Reset divider, 0 = De-assert divider reset */
    UINT32  Reserved:                   15;     /* [31:17] Reserved */
} AMBA_PLL_DIVIDER_16BIT_REG_s;

typedef struct {
    UINT32  ClkDivider:                 8;      /* [7:0] Integer Div Value */
    UINT32  Reset:                      1;      /* [8] 1 = Reset divider, 0 = De-assert divider reset */
    UINT32  Reserved:                   23;     /* [31:9] Reserved */
} AMBA_PLL_DIVIDER_8BIT_REG_s;

typedef struct {
    UINT32  WriteEnable:                1;      /* [0] Write 0 and then write 1 to make new divider value take effect */
    UINT32  Reserved0:                  3;      /* [3:1] Reserved */
    UINT32  ClkDivider:                 4;      /* [7:4] Integer divider value */
    UINT32  Reserved1:                  24;     /* [31:8] Reserved */
} AMBA_PLL_DIVIDER_4BIT_REG_s;

typedef struct {
    UINT32  AudioPostScaler:            1;      /* [0] Audio post-scaler */
    UINT32  SensorPostScaler:           1;      /* [1] Sensor post-scaler (gclk_so) */
    UINT32  Reserved0:                  4;      /* [5:2] */
    UINT32  Vin0PostScaler:             1;      /* [6] Vin0 post-scaler (gclk_so_vin0) */
    UINT32  Sensor2PostScaler:          1;      /* [7] Sensor2 post-scaler (gclk_so2) */
    UINT32  IdspPostScaler:             1;      /* [8] IDSP post-scaler */
    UINT32  Reserved1:                  2;      /* [10:9] */
    UINT32  Audio1PostScaler:           1;      /* [11] Audio_ch1 post-scaler */
    UINT32  Reserved2:                  1;      /* [12] */
    UINT32  AudioPreScaler:             1;      /* [13] Audio pll pre-scaler */
    UINT32  SensorPreScaler:            1;      /* [14] Sensor pll pre-scaler */
    UINT32  Vout0PreScaler:             1;      /* [15] Vout0/Video_A pll pre-scaler */
    UINT32  Vout1PreScaler:             1;      /* [16] Vout1/Video_B pll pre-scaler */
    UINT32  Reserved3:                  1;      /* [17] */
    UINT32  DdrAlt:                     1;      /* [18] Ddr Alt */
    UINT32  Reserved4:                  2;      /* [20:19] */
    UINT32  SysCnt0PostScaler:          1;      /* [21] 1 = disable clock output from post-scaler */
    UINT32  SysCnt1PostScaler:          1;      /* [22] 1 = disable clock output from post-scaler */
    UINT32  Vin1PostScaler:             1;      /* [23] Vin1 post-scaler (gclk_so_vin1) */
    UINT32  Enet0PostScaler:            1;      /* [24] Ethernet post-scaler (gclk_enet) */
    UINT32  Enet1PostScaler:            1;      /* [25] Ethernet post-scaler (gclk_enet1) */
    UINT32  Reserved5:                  6;      /* [31:26] */
} AMBA_PLL_SCALER_CTRL_REG_s;

typedef struct {
    UINT32  Audio0PostScaler:           1;      /* [0] 1 = disable clock output from post-scaler */
    UINT32  SensorPostScaler:           1;      /* [1] 1 = disable clock output from post-scaler */
    UINT32  Reserved0:                  4;      /* [5:2] */
    UINT32  Vin0PostScaler:             1;      /* [6] 1 = disable clock output from post-scaler */
    UINT32  Sensor2PostScaler:          1;      /* [7] 1 = disable clock output from post-scaler */
    UINT32  IdspPostScaler:             1;      /* [8] 1 = disable clock output from post-scaler */
    UINT32  Cortex0Clk:                 1;      /* [9] 1 = disable gclk_cortex0 */
    UINT32  Reserved1:                  2;      /* [11:10] */
    UINT32  Cortex1Clk:                 1;      /* [12] 1 = disable gclk_cortex1 */
    UINT32  Reserved2:                  8;      /* [20:13] */
    UINT32  SysCnt0PostScaler:          1;      /* [21] 1 = disable clock output from post-scaler */
    UINT32  SysCnt1PostScaler:          1;      /* [22] 1 = disable clock output from post-scaler */
    UINT32  Vin1PostScaler:             1;      /* [23] 1 = disable clock output from post-scaler */
    UINT32  Enet0PostScaler:            1;      /* [24] 1 = disable clock output from post-scaler */
    UINT32  Enet1PostScaler:            1;      /* [25] 1 = disable clock output from post-scaler */
    UINT32  Reserved3:                  6;      /* [31:26] */
} AMBA_CLK_DISABLE_REG_s;

/* Pll Infomation Registers */
typedef struct {
    UINT32  Vout0:                      1;      /* [0] 1 = Pll vout0/video_a is valid (output clock shall be as expected) */
    UINT32  Vin1:                       1;      /* [1] 1 = Pll vin1/sensor2 is valid (output clock shall be as expected) */
    UINT32  Vision:                     1;      /* [2] 1 = Pll vision is valid (output clock shall be as expected) */
    UINT32  Cortex0:                    1;      /* [3] 1 = Pll cortex0/cr52 is valid (output clock shall be as expected) */
    UINT32  vin0:                       1;      /* [4] 1 = Pll vin0/sensor is valid (output clock shall be as expected) */
    UINT32  Idsp:                       1;      /* [5] 1 = Pll idsp is valid (output clock shall be as expected) */
    UINT32  Ddr:                        1;      /* [6] 1 = Pll ddr is valid (output clock shall be as expected) */
    UINT32  Core:                       1;      /* [7] 1 = Pll core is valid (output clock shall be as expected) */
    UINT32  Audio:                      1;      /* [8] 1 = Pll audio is valid (output clock shall be as expected) */
    UINT32  Vout1:                      1;      /* [9] 1 = Pll vout1/video_b is valid (output clock shall be as expected) */
    UINT32  Cortex1:                    1;      /* [10] 1 = Pll cortex1/ca53 is valid (output clock shall be as expected) */
    UINT32  Reserved0:                  1;      /* [11] */
    UINT32  Fex:                        1;      /* [12] 1 = Pll fex is valid (output clock shall be as expected) */
    UINT32  Sd:                         1;      /* [13] 1 = Pll sd is valid (output clock shall be as expected) */
    UINT32  Enet:                       1;      /* [14] 1 = Pll enet is valid (output clock shall be as expected) */
    UINT32  Reserved1:                  1;      /* [15] */
    UINT32  Nand:                       1;      /* [16] 1 = Pll nand is valid (output clock shall be as expected) */
    UINT32  Reserved2:                  15;     /* [31:17] */
} AMBA_PLL_VALID_STATUS_REG_s;

/*
 * RCT: Clock Enable Control Register
 */
typedef struct {
    UINT32  Reserved0:                  1;      /* [0] */
    UINT32  EnableMEMD:                 1;      /* [1] 0 = disable, 1 = enable */
    UINT32  Reserved1:                  1;      /* [2] Reserved */
    UINT32  EnableMDENC:                1;      /* [3] 0 = disable, 1 = enable */
    UINT32  EnableTSFM:                 1;      /* [4] 0 = disable, 1 = enable */
    UINT32  EnableMDDEC:                1;      /* [5] 0 = disable, 1 = enable */
    UINT32  EnableCODE:                 1;      /* [6] 0 = disable, 1 = enable */
    UINT32  EnableORCW:                 1;      /* [7] 0 = disable, 1 = enable */
    UINT32  EnableSMEM:                 1;      /* [8] 0 = disable, 1 = enable */
    UINT32  Reserved2:                  1;      /* [9] Reserved */
    UINT32  EnableAXI0:                 1;      /* [10] 0 = disable, 1 = enable */
    UINT32  EnableAXI1:                 1;      /* [11] 0 = disable, 1 = enable */
    UINT32  EnableIDSP:                 1;      /* [12] 0 = disable, 1 = enable */
    UINT32  EnableCORE:                 1;      /* [13] 0 = disable, 1 = enable */
    UINT32  Reserved3:                  4;      /* [17:14] Reserved */
    UINT32  EnableOL2C:                 1;      /* [18] 0 = disable, 1 = enable */
    UINT32  Reserved4:                  1;      /* [19] Reserved */
    UINT32  EnableVORC:                 1;      /* [20] 0 = disable, 1 = enable */
    UINT32  Reserved5:                  3;      /* [23:21] Reserved */
    UINT32  EnableVMEM0:                1;      /* [24] 0 = disable, 1 = enable */
    UINT32  Reserved6:                  1;      /* [25] Reserved */
    UINT32  EnableVP0:                  1;      /* [26] 0 = disable, 1 = enable */
    UINT32  Reserved7:                  1;      /* [27] Reserved */
    UINT32  EnableDBSE:                 1;      /* [28] 0 = disable, 1 = enable */
    UINT32  Reserved8:                  3;      /* [31:29] Reserved */
} AMBA_CLK_ENABLE_CTRL_REG_s;

typedef struct {
    UINT32  PllObsvSelect:              5;      /* [4:0] Selection of pll signal for observation */
    UINT32  PllObsvEnable:              1;      /* [5] 1 = Enable pll signal observation on xx_clk_si pin (the observable frequency is 1/16 of the actual signal) */
    UINT32  Reserved0:                  2;      /* [7:6] */
    UINT32  MipiLvdsObsvCtrl:           2;      /* [9:8] MIPI/LVDS clock observed on xx_vd0_clk. 2'bx0 = fn_clock, 2'b01 = muxed spclk_c, 2'b11 = muxed RxByteClkHS */
    UINT32  Reserved1:                  22;     /* [31:10] */
} AMBA_PLL_CLK_OBSV_REG_s;

/* Ring Oscillator Registers */
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

typedef struct {
    UINT32  RoscSampleCount:            24;     /* [23:0] rosc_sample_count */
    UINT32  Reserved:                   7;      /* [30:24] reserved */
    UINT32  ClkRefDivdRst:              1;      /* [31] clk_ref_divisor_rst_l */
} AMBA_RING_OSC_OBSV_REG_s;

/**
 *  Definitions for io pad control
 */
typedef struct {
    UINT32  ClkSiDir:                   1;      /* [0] clk_si pad is: 0 = output mode, 1 = input mode */
    UINT32  Reserved:                   31;     /* [31:1] */
} AMBA_IO_PAD_CTRL0_REG_s;

typedef struct {
    UINT32  Reserved0:                  3;      /* [2:0] Reserved */
    UINT32  InputType:                  1;      /* [3] 0 = CMOS, 1 = Schmitt input */
    UINT32  Reserved1:                  28;     /* [31:4] Reserved */
} AMBA_IO_CTRL_REG_s;

typedef struct {
    UINT32  Vout0ClkSel:                1;      /* [0] Derive gclk_vo_a from gclk_vo_b */
    UINT32  Vout1ClkSel:                1;      /* [1] Derive gclk_vo_b from gclk_vo_a */
    UINT32  Reserved0:                  30;     /* [31:2] Reserved */
} AMBA_VOUT_CLOCK_SEL_REG_s;

typedef struct {
    UINT32  Reserved0:                  2;      /* [1:0] */
    UINT32  SdHsPol:                    1;      /* [2] SD HS polarity */
    UINT32  Sdio0HsPol:                 1;      /* [3] SDIO0 HS polarity */
    UINT32  Reserved1:                  1;      /* [4] */
    UINT32  Enet0RefClkDir:             1;      /* [5] Ethernet reference clock 0 (xx_enet_clk_rx) direction */
    UINT32  Enet1RefClkDir:             1;      /* [6] Ethernet reference clock 1 (xx_enet1_clk_rx) direction */
    UINT32  Reserved2:                  25;     /* [31:7] */
} AMBA_IO_PAD_CTRL1_REG_s;

/**
 *  Definitions for analog parameters of each PHY
 */
typedef struct {
    UINT32  Reserved0:                  1;      /* [0] Reserved */
    UINT32  UsbPhySuspendDevice:        1;      /* [1] 0 = suspend USB PHY, 1 = suspend controlled by USB device controller */
    UINT32  UsbPhyNeverSuspend:         1;      /* [2] 0 = Allow suspend USB PHY, 1 = Never suspend USB PHY */
    UINT32  Reserved1:                  9;      /* [11:3] Reserved */
    UINT32  UsbPhySuspendHost:          1;      /* [12] 0 = suspend USB PHY, 1 = suspend controlled by USB host controller */
    UINT32  Reserved2:                  19;     /* [31:13] Reserved */
} AMBA_POWER_CTRL_REG_s;

/**
 *  Definitions for ENET
 */
typedef struct {
    UINT32  Enet0Select:                1;      /* [0] Ethernet selection */
    UINT32  Enet0PhyIntfSelect:         1;      /* [1] PHY interface selection. 0 = RGMII, 1 = RMII */
    UINT32  Enet0ClkSrcSelect:          1;      /* [2] 0 = Use xx_enet_clk_rx, 1 = Use gclk_enet */
    UINT32  Reserved0:                  1;      /* [3] Reserved */
    UINT32  Enet1Select:                1;      /* [4] Ethernet selection */
    UINT32  Enet1PhyIntfSelect:         1;      /* [5] PHY interface selection. 0 = RGMII, 1 = RMII */
    UINT32  Enet1ClkSrcSelect:          1;      /* [6] 0 = Use xx_enet1_clk_rx, 1 = Use gclk_enet1 */
    UINT32  Reserved1:                  25;     /* [31:7] Reserved */
} AMBA_ENET_CLK_CONFIG_REG_s;

/**
 *  Definitions for SD
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

typedef struct {
    UINT32  Sd0PhyStatus:               16;     /* [15:0] sd_rct_phy_sts[15:0] */
    UINT32  Sd1PhyStatus:               16;     /* [31:16] sdio_rct_phy_sts[15:0] */
} AMBA_SD_PHY_OBSERV0_REG_s;

/**
 *  Definitions for USB
 */
typedef struct {
    UINT32  OpMode:                     2;      /* [1:0] Op Mode */
    UINT32  DmPullDown:                 1;      /* [2] D- Pull down */
    UINT32  DpPullDown:                 1;      /* [3] D+ Pull down */
    UINT32  BatteryChargeModeEnable:    1;      /* [4] Battery Charge Mode Enable */
    UINT32  Reserved:                   27;     /* [31:5] Reserved */
} AMBA_USB_CHARGE_CTRL_REG_s;

/*
 * Definitions for Random Number Generator
 */
typedef struct {
    UINT32  PowerDown:                  1;      /* [0] Rng_pd */
    UINT32  Reset:                      1;      /* [1] Rng_rst */
    UINT32  VcoStop:                    1;      /* [2] Rng_vcostop */
    UINT32  Select:                     1;      /* [3] Rng_sel */
    UINT32  TrimEnable:                 1;      /* [4] Rng_trim_en */
    UINT32  ErrCheck:                   1;      /* [5] Rng_err_check */
    UINT32  Reserved0:                  2;      /* [7:6] Reserved */
    UINT32  Range:                      2;      /* [9:8] Rng_range */
    UINT32  Trim1:                      3;      /* [12:10] Rng_trim1 */
    UINT32  Trim2:                      3;      /* [15:13] Rng_trim2 */
    UINT32  Reserved1:                  16;     /* [31:16] Reserved */
} AMBA_RNG_RCT_CTRL_REG_s;

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
        UINT32  ErrSotSyncHS_11_8:          4;      /* [27:24] ErrSotSyncHS[11:8] */
        UINT32  Reserved0:                  4;      /* [31:28] Reserved */
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
        UINT32  RxStateDataOut:             3;      /* [2:0] Rx_state_data_o, selected data lane's rx state */
        UINT32  Reserved0:                  1;      /* [3] Reserved */
        UINT32  RxStateClkOut:              4;      /* [7:4] Rx_state_clk_o, selected clock lane's rx state */
        UINT32  Reserved1:                  24;     /* [31:8] Reserved */
    } Bits;
} AMBA_DPHY_OBSERVE4_REG_u;

typedef union {
    UINT32  Data;

    struct {
        UINT32  ErrControl_11_8:            4;      /* [3:0] ErrControl[11:8] */
        UINT32  Reserved0:                  4;      /* [7:4] Reserved */
        UINT32  RxValidHS_11_8:             4;      /* [11:8] RxValidHS[11:8] */
        UINT32  Reserved1:                  4;      /* [15:12] Reserved */
        UINT32  ErrSotHS_11_8:              4;      /* [19:16] ErrSotHS[11:8] */
        UINT32  Reserved2:                  4;      /* [23:20] Reserved */
        UINT32  ErrSotSyncHS:               8;      /* [31:24] ErrSotSyncHS[7:0] */
    } Bits;
} AMBA_DPHY_OBSERVE5_REG_u;

typedef union {
    UINT32  Data;

    struct {
        UINT32  RxActiveHS_11_8:            4;      /* [3:0] RxActiveHS[11:8] */
        UINT32  Reserved0:                  4;      /* [7:4] Reserved */
        UINT32  RxStopState_11_8:           4;      /* [11:8] RxStopState[11:8] */
        UINT32  Reserved1:                  4;      /* [15:12] Reserved */
        UINT32  RxSyncHS_11_8:              4;      /* [19:16] RxSyncHS[11:8] */
        UINT32  Reserved2:                  4;      /* [23:20] Reserved */
        UINT32  RxUlpsActiveNot_11_8:       4;      /* [27:24] RxUlpsActiveNot[11:8] */
        UINT32  Reserved3:                  4;      /* [31:28] Reserved */
    } Bits;
} AMBA_DPHY_OBSERVE6_REG_u;

typedef union {
    UINT32  Data;

    struct {
        UINT32  DlyMatchMode:               2;      /* [1:0] dly_match_mode */
        UINT32  Reserved0:                  4;      /* [5:2] Reserved */
        UINT32  DphyRst0:                   1;      /* [6] reset mipi dphy digital part */
        UINT32  DphyRst1:                   1;      /* [7] reset mipi dphy digital part */
        UINT32  DphyRst2:                   1;      /* [8] reset mipi dphy digital part */
        UINT32  RstDphyAfe:                 3;      /* [11:9] reset mipi dphy analog front end */
        UINT32  NoVbn:                      1;      /* [12] no_vbn */
        UINT32  Reserved1:                  3;      /* [15:13] Reserved */
        UINT32  BitMode0:                   2;      /* [17:16] bit_mode0[1:0] */
        UINT32  BitMode1:                   2;      /* [19:18] bit_mode1[1:0] */
        UINT32  BitMode2:                   2;      /* [21:20] bit_mode2[1:0] */
        UINT32  Reserved2:                  2;      /* [23:22] Reserved */
        UINT32  ForceR100Off:               3;      /* [26:24] force_r100_off[2:0] (for clock lanes) */
        UINT32  Reserved3:                  1;      /* [27] Reserved */
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
        UINT32  Reserved2:                  11;     /* [19:9] Reserved */
        UINT32  DataR100Off:                12;     /* [31:20] data_r100_off[11:0] */
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
        UINT32  ForceRxMode8:               1;      /* [8] ForceRxMode8 */
        UINT32  ForceRxMode9:               1;      /* [9] ForceRxMode9 */
        UINT32  ForceRxMode10:              1;      /* [10] ForceRxMode10 */
        UINT32  ForceRxMode11:              1;      /* [11] ForceRxMode11 */
        UINT32  Reserved0:                  4;      /* [15:12] Reserved */
        UINT32  ForceClkHs0:                1;      /* [16] ForceClkHs0, force MIPI Rx clock channel to go into HS state */
        UINT32  ForceClkHs1:                1;      /* [17] ForceClkHs1, force MIPI Rx clock channel to go into HS state */
        UINT32  ForceClkHs2:                1;      /* [18] ForceClkHs2, force MIPI Rx clock channel to go into HS state */
        UINT32  Reserved1:                  1;      /* [19] Reserved */
        UINT32  ForceMipiHs0:               1;      /* [20] force_mipi_hs0, force MIPI Rx data channel to go into HS state */
        UINT32  ForceMipiHs1:               1;      /* [21] force_mipi_hs1, force MIPI Rx data channel to go into HS state */
        UINT32  ForceMipiHs2:               1;      /* [22] force_mipi_hs2, force MIPI Rx data channel to go into HS state */
        UINT32  Reserved2:                  9;      /* [31:23] Reserved */
    } Bits;
} AMBA_DPHY_CTRL2_REG_u;

typedef union {
    UINT32  Data;

    struct {
        UINT32  LvCmosMode:                 12;     /* [11:0] LvCmos Mode */
        UINT32  Reserved0:                  4;      /* [15:12] Reserved */
        UINT32  LvCmosSpClk:                3;      /* [18:16] LvCmos_mode_spclk */
        UINT32  Reserved1:                  1;      /* [19] Reserved */
        UINT32  MipiMode:                   3;      /* [22:20] mipi_mode */
        UINT32  SafeEn:                     1;      /* [23] safe_en */
        UINT32  SafeClr:                    1;      /* [24] safe_clr */
        UINT32  Reserved2:                  7;      /* [31:25] Reserved */
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
        UINT32  RxStateDataSelect:          5;      /* [4:0] rx_state_data_sel (MIPI RX Only. channel selection of data lane's rx_state) */
        UINT32  RxStateClkSelect:           2;      /* [6:5] rx_state_clk_sel (MIPI RX Only. channel selection of clock lane's rx_state) */
        UINT32  Reserved0:                  1;      /* [7] Reserved */
        UINT32  DataRctDebSelect:           5;      /* [12:8] data_rct_deb_sel (MIPI RX Only. channel selection of data lane's data output) */
        UINT32  RstnRctDeb:                 1;      /* [13] rst_n_rct_deb (MIPI RX Only. rst of rct mipi debug mode) */
        UINT32  Reserved1:                  18;     /* [31:14] Reserved */
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
        UINT32  Reserved0:                  14;     /* [13:0] Reserved */
        UINT32  LockstepGated:              12;     /* [25:14] lockstep_gated (gate lockstep signal if that channel is not used) */
        UINT32  LbOutSel:                   4;      /* [29:26] lb_outsel (lb output select signal) */
        UINT32  LbEn:                       1;      /* [30] lb_en (lb enable signal) */
        UINT32  LbClr:                      1;      /* [31] lb_clr (lb clear signal) */
    } Bits;
} AMBA_DPHY_CTRL12_REG_u;

typedef union {
    UINT32  Data;

    struct {
        UINT32  Reserved0:                  24;     /* [23:0] Reserved */
        UINT32  CfgRefClkSelect:            1;      /* [24] cfg_ref_clk_sel */
        UINT32  CfgMipiMode:                1;      /* [25] cfg_mipi_mode */
        UINT32  CfgTestPattern:             2;      /* [27:26] cfg_test_pattern */
        UINT32  Reserved1:                  3;      /* [30:28] Reserved */
        UINT32  Rst_l:                      1;      /* [31] rst_l */
    } Bits;
} AMBA_PHY_BIST_CTRL11_REG_u;

typedef union {
    UINT32  Data;

    struct {
        UINT32  CfgLaneEnable:              12;     /* [11:0] cfg_lane_enable */
        UINT32  Reserved0:                  20;     /* [31:12] Reserved */
    } Bits;
} AMBA_PHY_BIST_CTRL12_REG_u;

typedef union {
    UINT32  Data;

    struct {
        UINT32  DbgRegEotFail:              12;     /* [11:0] dbg_reg_eot_fail */
        UINT32  Reserved0:                  20;     /* [31:12] Reserved */
    } Bits;
} AMBA_PHY_BIST_OBSV11_REG_u;

typedef union {
    UINT32  Data;

    struct {
        UINT32  DbgRegEotPass:              12;     /* [11:0] dbg_reg_eot_pass */
        UINT32  Reserved0:                  20;     /* [31:12] Reserved */
    } Bits;
} AMBA_PHY_BIST_OBSV12_REG_u;

typedef union {
    UINT32  Data;

    struct {
        UINT32  DbgRegSotFail:              12;     /* [11:0] dbg_reg_sot_fail */
        UINT32  Reserved0:                  20;     /* [31:12] Reserved */
    } Bits;
} AMBA_PHY_BIST_OBSV13_REG_u;

typedef union {
    UINT32  Data;

    struct {
        UINT32  DbgRegSotPass:              12;     /* [11:0] dbg_reg_sot_pass */
        UINT32  Reserved0:                  20;     /* [31:12] Reserved */
    } Bits;
} AMBA_PHY_BIST_OBSV14_REG_u;

typedef union {
    UINT32  Data;

    struct {
        UINT32  DbgRegTestPatternFail:      12;     /* [11:0] dbg_reg_test_pattern_fail */
        UINT32  Reserved0:                  20;     /* [31:12] Reserved */
    } Bits;
} AMBA_PHY_BIST_OBSV15_REG_u;

typedef union {
    UINT32  Data;

    struct {
        UINT32  DbgRegTestPatternPass:      12;     /* [11:0] dbg_reg_test_patttern_pass */
        UINT32  Reserved0:                  20;     /* [31:12] Reserved */
    } Bits;
} AMBA_PHY_BIST_OBSV16_REG_u;

typedef union {
    UINT32  Data;

    struct {
        UINT32  PhyBistFail:                12;     /* [11:0] phy_bist_fail */
        UINT32  Reserved0:                  20;     /* [31:12] Reserved */
    } Bits;
} AMBA_PHY_BIST_OBSV17_REG_u;

typedef union {
    UINT32  Data;

    struct {
        UINT32  PhyBistDone:                12;     /* [11:0] phy_bist_done */
        UINT32  Reserved0:                  20;     /* [31:12] Reserved */
    } Bits;
} AMBA_PHY_BIST_OBSV18_REG_u;

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
 * RCT: USB PHY Control Registers
 */
typedef struct {
    UINT32  Reserved0:                  2;      /* [1:0] */
    UINT32  UsbPhySoftReset:            1;      /* [2] USB PHY soft reset: 0 - disable, 1 - enable */
    UINT32  RefClkSel:                  2;      /* [4:3] 2'b1x = PLL uses CLKCORE as ref, 2'b01 = XO block uses external clock, 2'b00 = XO block uses the clock from crystal */
    UINT32  Reserved1:                  1;      /* [5] Reserved */
    UINT32  Commononn:                  1;      /* [6] */
    UINT32  CompdisTune:                3;      /* [9:7] */
    UINT32  OtgTune:                    3;      /* [12:10] */
    UINT32  SqrxTune:                   3;      /* [15:13] */
    UINT32  TxFslsTune:                 4;      /* [19:16] */
    UINT32  TxRiseTune:                 2;      /* [21:20] */
    UINT32  TxvRefTune:                 4;      /* [25:22] */
    UINT32  TxHsxvTune:                 2;      /* [27:26] */
    UINT32  AteReset:                   1;      /* [28] */
    UINT32  Reserved2:                  1;      /* [29] Reserved */
    UINT32  SleepM:                     1;      /* [30] SleepM (used only when bit[29] is set to 1) */
    UINT32  Reserved3:                  1;      /* [31] */
} AMBA_USB_PHY_CTRL0_REG_s;

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

typedef struct {
    UINT32  Reserved0:                  1;      /* [0] Reserved */
    UINT32  DcdEnable:                  1;      /* [1] 1 = Enable pull-up on D+ and pull-down on D- for data contact detect */
    UINT32  DataSrcVoltageEnable:       1;      /* [2] 1 = Enable data source voltage for battery charging */
    UINT32  DataDetectVoltageEnable:    1;      /* [3] 1 = Enable data detect voltage for battery charging attach/connect detection */
    UINT32  ChargeSrcSelect:            1;      /* [4] Data source voltage is sourced onto 0= D+/DP, 1=D-/DM and sunk from 0= D-/DM, 1= D+/DP */
    UINT32  VbusValidSelect:            1;      /* [5] 0 = Use internal session valid comparator, 1 = Use external signal */
    UINT32  Reserved1:                  3;      /* [8:6] Reserved */
    UINT32  VbusValidEnable:            1;      /* [9] 1 = Enable VBUS valid comparator */
    UINT32  OtgDisable:                 1;      /* [10] 1 = Power down OTG block */
    UINT32  TxPreempPulseTune:          1;      /* [11] */
    UINT32  TxPreempampTune:            2;      /* [13:12] */
    UINT32  TxResistanceTune:           2;      /* [15:14] */
    UINT32  Reserved2:                  16;     /* [31:16] Reserved */
} AMBA_USB_PHY_CTRL1_REG_s;

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

#define USBPHY_SEL_REG_USB0SEL_SHIFT        (0UL)
#define USBPHY_SEL_REG_USB0SEL_MASK         (0x01UL << USBPHY_SEL_REG_USB0SEL_SHIFT)

#define USB_RSTCTRL_REG_HOST_RST_SHIFT      (0UL)
#define USB_RSTCTRL_REG_DEVICE_RST_SHIFT    (1UL)

#define USB_RSTCTRL_REG_HOST_RST_MASK       (1UL << USB_RSTCTRL_REG_HOST_RST_SHIFT)
#define USB_RSTCTRL_REG_DEVICE_RST_MASK     (1UL << USB_RSTCTRL_REG_DEVICE_RST_SHIFT)

/*
 * RCT: Clock Monitor Control 1:15 (en:upper bound/lower bound)
 */
typedef struct {
    UINT32  LowerBound:                15;      /* [14:0] Lower bound value */
    UINT32  Reserved0:                  1;      /* [15] Reserved */
    UINT32  UpperBound:                15;      /* [30:16] Upper bound value */
    UINT32  Enable:                     1;      /* [31] Enable */
} AMBA_RCT_CLKMON_CTRL_1_15_REG_s;

/*
 * RCT: Clock Monitor Control 1:12 (en:upper bound/lower bound)
 */
typedef struct {
    UINT32  LowerBound:                12;      /* [11:0] Lower bound value */
    UINT32  Reserved0:                  4;      /* [15:12] Reserved */
    UINT32  UpperBound:                12;      /* [27:16] Upper bound value */
    UINT32  Reserved1:                  3;      /* [30:28] Reserved */
    UINT32  Enable:                     1;      /* [31] Enable */
} AMBA_RCT_CLKMON_CTRL_1_12_REG_s;

/*
 * RCT: Clock Monitor Control 1:10 (en:upper bound/lower bound)
 */
typedef struct {
    UINT32  LowerBound:                10;      /* [9:0] Lower bound value */
    UINT32  Reserved0:                  6;      /* [15:10] Reserved */
    UINT32  UpperBound:                10;      /* [25:16] Upper bound value */
    UINT32  Reserved1:                  5;      /* [30:26] Reserved */
    UINT32  Enable:                     1;      /* [31] Enable */
} AMBA_RCT_CLKMON_CTRL_1_10_REG_s;

/*
 * RCT: Clock Monitor Observation 1:15
 */
typedef struct {
    UINT32  CycleCount:                15;      /* [14:0] Lower bound value */
    UINT32  Reserved:                  17;      /* [31:15] Reserved */
} AMBA_RCT_CLKMON_OBSER_1_15_REG_s;

/*
 * RCT: Clock Monitor Observation 1:12
 */
typedef struct {
    UINT32  CycleCount:                12;      /* [11:0] Lower bound value */
    UINT32  Reserved:                  20;      /* [31:12] Reserved */
} AMBA_RCT_CLKMON_OBSER_1_12_REG_s;

/*
 * RCT: Power Monitor Control 1
 */
typedef struct {
    UINT32  LowGearDetUVLevSel:         4;      /* [3:0] Low gear Detector UV level select control  */
    UINT32  LowGearDetOVLevSel:         4;      /* [7:4] Low gear Detector OV level select control */
    UINT32  HighGearDet1UVLevSel:       4;      /* [11:8] High gear Detector 1 UV level select control */
    UINT32  HighGearDet1OVLevSel:       4;      /* [15:12] High gear Detector 1 OV level select control */
    UINT32  HighGearDet2UVLevSel:       4;      /* [19:16] High gear Detector 2 UV level select control */
    UINT32  HighGearDet2OVLevSel:       4;      /* [23:20] High gear Detector 2 OV level select control */
    UINT32  ClearErrorb:                1;      /* [24] clear PSMON SF errorb outputcontrol */
    UINT32  Reserved:                   7;      /* [31:25] Reserved */
} AMBA_PS_MON_CTRL1_REG_s;

/*
 * RCT: Power Monitor Control 2
 */
typedef struct {
    UINT32  vsenselosel:                4;      /* [3:0] low gear vsense select control  */
    UINT32  vsensehi1sel:               4;      /* [7:4] High gear 1 vsense select control  */
    UINT32  vsensehi2sel:               4;      /* [11:8] High gear 2 vsense select control  */
    UINT32  pd_declo:                   1;      /* [12] DetectorLo power down control  */
    UINT32  pd_dechi1:                  1;      /* [13] DetectorHi1 power down control  */
    UINT32  pd_dechi2:                  1;      /* [14] DetectorHi2 power down control  */
    UINT32  pd_ref:                     1;      /* [15] vrefgen power down control  */
    UINT32  abist_en:                   1;      /* [16] PSMON ABIST enable control  */
    UINT32  abist_sel:                  1;      /* [17] PSMON ABIST select control  */
    UINT32  abist_man:                  1;      /* [18] PSMON manual mode control  */
    UINT32  abist_rstn:                 1;      /* [19] PSMON logic resetn control  */
    UINT32  abist_window:               2;      /* [21:20] ABIST comparator settle time window control  */
    UINT32  abist_toggle:               1;      /* [22] PSMON ABIST check pattern toggle time number control  */
    UINT32  pm_disable:                 1;      /* [23] PSMON disable control  */
    UINT32  sf_rstn:                    1;      /* [24] PSMON Stick Function reset function resetn control  */
    UINT32  pm_rstn:                    1;      /* [25] PSMON digital module top resetn control  */
    UINT32  sf_wait:                    2;      /* [27:26] PSMON SF waiting time for analog part settle down  */
    UINT32  pd_psmon:                   1;      /* [28] PSMON whole analog part power down  */
    UINT32  sf_enable:                  1;      /* [29] PSMON SF disable control  */
    UINT32  abist_brek:                 1;      /* [30] ignore ABIST state, directly SF outputs  */
    UINT32  uvov_swap:                  1;      /* [31] PSMON UV/OV sensing voltage swap  */
} AMBA_PS_MON_CTRL2_REG_s;

/*
 * RCT: Power Monitor Observation
 */
typedef struct {
    UINT32  nvovin_lo:                  1;      /* [0] Detector LO OV comparator output */
    UINT32  nvuvin_lo:                  1;      /* [1] Detector LO UV comparator output */
    UINT32  nvovin_hi1:                 1;      /* [2] Detector Hi1 OV comparator output */
    UINT32  nvuvin_hi1:                 1;      /* [3] Detector Hi1 UV comparator output */
    UINT32  nvovin_hi2:                 1;      /* [4] Detector Hi2 OV comparator output */
    UINT32  nvuvin_hi2:                 1;      /* [5] Detector Hi2 UV comparator output */
    UINT32  nvov_lo_sf:                 1;      /* [6] Detector Lo OV comparator output after SF */
    UINT32  nvuv_lo_sf:                 1;      /* [7] Detector Lo UV comparator output after SF */
    UINT32  nvov_hi1_sf:                1;      /* [8] Detector Hi1 OV comparator output after SF */
    UINT32  nvuv_hi1_sf:                1;      /* [9] Detector Hi1 UV comparator output */
    UINT32  nvov_hi2_sf:                1;      /* [10] Detector Hi2 OV comparator output after SF */
    UINT32  nvuv_hi2_sf:                1;      /* [11] Detector Hi2 UV comparator output after SF */
    UINT32  pd_declo_obsv:              1;      /* [12] Detecter LO internal power down */
    UINT32  pd_dechi1_obsv:             1;      /* [13] Detecter Hi1 internal power down */
    UINT32  pd_dechi2_obsv:             1;      /* [14] Detecter Hi2 internal power down */
    UINT32  pd_psmon:                   1;      /* [15] PSMON Analog Part power down */
    UINT32  abist_state:                3;      /* [18:16] PSMON Analog Part power down */
    UINT32  pm_errorb:                  1;      /* [19] pm_errorb observation */
    UINT32  abist_en_abist:             1;      /* [20] ABIST Enable control */
    UINT32  pasmon_abist_finish:        1;      /* [21] ABIST finish */
    UINT32  psmon_abist_pass:           1;      /* [22] ABIST pass */
    UINT32  abist_sel_obsv:             1;      /* [23] ABIST abist select control internal signal */
    UINT32  abist_window:               2;      /* [25:24] ABIST comparator settle time window control */
    UINT32  abist_toggle:               1;      /* [26] ABIST toggle time number control */
    UINT32  abist_break:                1;      /* [27] ABIST break control */
    UINT32  sf_wait:                    2;      /* [29:28] SF wait time for analog settle control */
    UINT32  sf_enable:                  1;      /* [30] SF enable control */
    UINT32  abist_enable:               1;      /* [31] ABIST enable control */
} AMBA_PS_MON_OBSV_REG_s;


/*
 * RCT: All Registers
 */
typedef struct {
    volatile AMBA_PLL_CTRL_REG_s                    CorePllCtrl;            /* 0x000(RW): Core pll main configuration */
    volatile UINT32                                 CorePllFraction;        /* 0x004(RW): Core pll fractional part of fractional-N mode */
    volatile UINT32                                 Reserved0;              /* 0x008: Reserved */
    volatile AMBA_PLL_DIVIDER_16BIT_REG_s           Sd0ClkDivider;          /* 0x00C(RW): SD0/SD clock divider (LSB 17-bit) */
    volatile UINT32                                 Reserved1;              /* 0x010: Reserved */
    volatile AMBA_PLL_CTRL_REG_s                    Vout1PllCtrl;           /* 0x014(RW): Vout1/Video_B pll main configuration */
    volatile UINT32                                 Vout1PllFraction;       /* 0x018(RW): Vout1/Video_B pll fractional part of fractional-N mode */
    volatile AMBA_PLL_DIVIDER_4BIT_REG_s            Vout1PllPrescaler;      /* 0x01C(RW): Vout1/Video_B pll pre-scaler (LSB 8-bit) */
    volatile AMBA_PLL_DIVIDER_16BIT_REG_s           Vout1PllPostscaler;     /* 0x020(RW): Vout1/Video_B pll post-scaler (LSB 17 bits) */
    volatile AMBA_PLL_CTRL_REG_s                    SensorPllCtrl;          /* 0x024(RW): Vin0/Sensor pll main configuration */
    volatile UINT32                                 SensorPllFraction;      /* 0x028(RW): Vin0/Sensor pll fractional part of fractional-N mode */
    volatile AMBA_PLL_VALID_STATUS_REG_s            PllValidStatus;         /* 0x02C(RO): Valid flags of all the plls */
    volatile AMBA_PLL_DIVIDER_4BIT_REG_s            SensorPllPostscaler;    /* 0x030(RW): Vin0/Sensor pll post-scaler (LSB 8-bit) */
    volatile AMBA_SYS_CONFIG_REG_s                  SysConfig;              /* 0x034(RO): System configuration */
    volatile AMBA_PLL_DIVIDER_24BIT_REG_s           UartApbClkDivider;      /* 0x038(RW): UART_APB clock divider (LSB 25-bit) */
    volatile AMBA_PLL_DIVIDER_24BIT_REG_s           SpiMasterClkDivider;    /* 0x03C(RW): SPI master clock divider (LSB 25-bit) */
    volatile UINT32                                 Reserved2[3];           /* 0x040-0x048: Reserved */
    volatile AMBA_PLL_DIVIDER_4BIT_REG_s            SensorPllPrescaler;     /* 0x04C(RW): Vin0/Sensor pll pre-scaler (LSB 8-bit) */
    volatile AMBA_POWER_CTRL_REG_s                  AnalogPowerCtrl;        /* 0x050(RW): Analog power control */
    volatile AMBA_PLL_CTRL_REG_s                    AudioPllCtrl;           /* 0x054(RW): Audio pll main configuration */
    volatile UINT32                                 AudioPllFraction;       /* 0x058(RW): Audio pll fractional part of fractional-N mode */
    volatile AMBA_PLL_DIVIDER_4BIT_REG_s            AudioPllPostscaler;     /* 0x05C(RW): Audio pll post-scaler (LSB 8-bit) */
    volatile AMBA_PLL_DIVIDER_4BIT_REG_s            AudioPllPrescaler;      /* 0x060(RW): Audio pll pre-scaler (LSB 8-bit) */
    volatile UINT32                                 Reserved3;              /* 0x064: Reserved */
    volatile UINT32                                 SysReset0;              /* 0x068(RW): System software reset control 0 */
    volatile UINT32                                 Reserved4[2];           /* 0x06C-0x070: Reserved */
    volatile UINT32                                 FioReset;               /* 0x074(WO): Flash controller reset control */
    volatile UINT32                                 WdtResetState;          /* 0x078(RW): Watchdog Timer Reset State (Write Protected by 0x260) */
    volatile UINT32                                 Reserved5;              /* 0x07C: Reserved */
    volatile AMBA_PLL_DIVIDER_24BIT_REG_s           PlugInDetectClkDivider; /* 0x080(RW): SD Card detect/USB VBUS detect debouncing clock divider (LSB 25-bit) */
    volatile AMBA_PLL_DIVIDER_24BIT_REG_s           PwmClkDivider;          /* 0x084(RW): PWM clock divider (LSB 25-bit) */
    volatile UINT32                                 Reserved6;              /* 0x088: Reserved */
    volatile UINT32                                 ClkEnableCtrl;          /* 0x08C(RW): Clock enable control for all clusters */
    volatile UINT32                                 Reserved7[5];           /* 0x090-0x0A0: Reserved */
    volatile AMBA_PLL_REF_CLK_SELECT_REG_s          AudioRefClkCtrl0;       /* 0x0A4(RW): Audio pll reference clock configuration */
    volatile AMBA_AUDIO_EXT_CLK_CTRL_REG_s          AudioExtClkCtrl;        /* 0x0A8(RW): Control if using external clock for audio */
    volatile AMBA_PLL_REF_CLK_SELECT_REG_s          Vout1RefClkCtrl0;       /* 0x0AC(RW): Vout1/Video_B pll reference clock configuration */
    volatile UINT32                                 Reserved8;              /* 0x0B0: Reserved */
    volatile AMBA_PLL_EXT_CLK_SRC_REG_s             AudioRefClkCtrl1;       /* 0x0B4(RW): External clock source selection for audio reference clock */
    volatile AMBA_PLL_EXT_CLK_SRC_REG_s             Vout1RefClkCtrl1;       /* 0x0B8(RW): External clock source selection for vout1/video_b reference clock */
    volatile AMBA_IO_PAD_CTRL0_REG_s                IoPadCtrl0;             /* 0x0BC(RW): I/O pad control register 0 */
    volatile AMBA_PLL_CTRL_REG_s                    Vout0PllCtrl;           /* 0x0C0(RW): Vout0/Video_A pll main configuration */
    volatile UINT32                                 Vout0PllFraction;       /* 0x0C4(RW): Vout0/Video_A pll fractional part of fractional-N mode */
    volatile AMBA_PLL_DIVIDER_4BIT_REG_s            Vout0PllPrescaler;      /* 0x0C8(RW): Vout0/Video_A pll pre-scaler (LSB 8-bit) */
    volatile AMBA_PLL_DIVIDER_16BIT_REG_s           Vout0PllPostscaler;     /* 0x0CC(RW): Vout0/Video_A pll post-scaler (LSB 17 bits) */
    volatile AMBA_PLL_EXT_CLK_SRC_REG_s             Vout0RefClkCtrl1;       /* 0x0D0(RW): External clock source selection for vout0/video_a reference clock */
    volatile UINT32                                 Reserved9;              /* 0x0D4: Reserved */
    volatile AMBA_PLL_REF_CLK_SELECT_REG_s          Vout0RefClkCtrl0;       /* 0x0D8(RW): Vout0/Video_A pll reference clock configuration */
    volatile AMBA_PLL_CTRL_REG_s                    DdrPllCtrl;             /* 0x0DC(RW): DDR pll main configuration */
    volatile UINT32                                 DdrPllFraction;         /* 0x0E0(RW): DDR pll fractional part of fractional-N mode */
    volatile AMBA_PLL_CTRL_REG_s                    IdspPllCtrl;            /* 0x0E4(RW): IDSP pll main configuration */
    volatile UINT32                                 IdspPllFraction;        /* 0x0E8(RW): IDSP pll fractional part of fractional-N mode */
    volatile AMBA_PLL_DIVIDER_24BIT_REG_s           SpiSlaveClkDivider;     /* 0x0EC(RW): SPI slave clock divider (LSB 25 bits) */
    volatile UINT32                                 Reserved10[4];          /* 0x0F0-0x0FC: Reserved */

    volatile UINT32                                 CorePllCtrl2;           /* 0x100(RW): Core pll sub configuration */
    volatile UINT32                                 CorePllCtrl3;           /* 0x104(RW): Core pll misc configuration */
    volatile UINT32                                 IdspPllCtrl2;           /* 0x108(RW): IDSP pll sub configuration */
    volatile UINT32                                 IdspPllCtrl3;           /* 0x10C(RW): IDSP pll misc configuration */
    volatile UINT32                                 DdrPllCtrl2;            /* 0x110(RW): DDR pll sub configuration */
    volatile UINT32                                 DdrPllCtrl3;            /* 0x114(RW): DDR pll misc configuration */
    volatile UINT32                                 Reserved11;             /* 0x118: Reserved */
    volatile UINT32                                 SensorPllCtrl2;         /* 0x11C(RW): Vin0/Sensor pll sub configuration */
    volatile UINT32                                 SensorPllCtrl3;         /* 0x120(RW): Vin0/Sensor pll misc configuration */
    volatile UINT32                                 AudioPllCtrl2;          /* 0x124(RW): Audio pll sub configuration */
    volatile UINT32                                 Reserved12;             /* 0x128: Reserved */
    volatile UINT32                                 AudioPllCtrl3;          /* 0x12C(RW): Audio pll misc configuration */
    volatile UINT32                                 Vout1PllCtrl2;          /* 0x130(RW): Vout1/Video_B pll sub configuration */
    volatile UINT32                                 Vout1PllCtrl3;          /* 0x134(RW): Vout1/Video_B pll misc configuration */
    volatile UINT32                                 Reserved13;             /* 0x138: Reserved */
    volatile UINT32                                 Vout0PllCtrl2;          /* 0x13C(RW): Vout0/Video_A pll sub configuration */
    volatile UINT32                                 Vout0PllCtrl3;          /* 0x140(RW): Vout0/Video_A pll misc configuration */
    volatile UINT32                                 Reserved14;             /* 0x144: Reserved */
    volatile UINT32                                 DdrCalibClkDivider;     /* 0x148(RW): Clock divider to generate ddr calibration clock (LSB 5-bit) */
    volatile UINT32                                 Reserved15[10];         /* 0x14C-0x170: Reserved */
    volatile UINT32                                 CorePllDebug;           /* 0x174(RO): Debug status of core pll */
    volatile UINT32                                 IdspPllDebug;           /* 0x178(RO): Debug status of idsp pll */
    volatile UINT32                                 DdrPllDebug;            /* 0x17C(RO): Debug status of ddr pll */
    volatile UINT32                                 SensorPllDebug;         /* 0x180(RO): Debug status of vin0/sensor pll */
    volatile UINT32                                 AudioPllDebug;          /* 0x184(RO): Debug status of audio pll */
    volatile UINT32                                 Vout1PllDebug;          /* 0x188(RO): Debug status of vout1/video_b pll */
    volatile UINT32                                 Vout0PllDebug;          /* 0x18C(RO): Debug status of vout0/video_a pll */
    volatile UINT32                                 Reserved16[3];          /* 0x190-0x198: Reserved */
    volatile UINT32                                 SpiMasterClkSrcSelect;  /* 0x19C(RW): SSI master clock source selection (LSB 2-bit) */
    volatile UINT32                                 Reserved17;             /* 0x1A0: Reserved */
    volatile AMBA_RNG_RCT_CTRL_REG_s                RngCtrl;                /* 0x1A4(RW): Random number generator control */
    volatile UINT32                                 Reserved18[7];          /* 0x1A8-0x1C0: Reserved */
    volatile UINT32                                 PwmClkSrcSelect;        /* 0x1C4(RW): PWM clock source selection (LSB 2-bit) */
    volatile AMBA_UART_CLK_SRC_SELECT_REG_s         UartApbClkSrcSelect;    /* 0x1C8(RW): UART_APB clock source selection (LSB 2-bit) */
    volatile UINT32                                 Reserved19[2];          /* 0x1CC-0x1D0: Reserved */
    volatile AMBA_VOUT_CLOCK_SEL_REG_s              VoutClockSelect;        /* 0x1D4(RW): Pll selection for each vout clock */
    volatile UINT32                                 Reserved20[2];          /* 0x1D8-0x1DC: Reserved */
    volatile AMBA_PLL_CLK_OBSV_REG_s                ClkObsvCtrl;            /* 0x1E0(RW): Enables observation of pll outputs on xx_clk_si pin (LSB 10-bit) */
    volatile UINT32                                 Reserved21[4];          /* 0x1E4-0x1F0: Reserved */
    volatile AMBA_PLL_DIVIDER_4BIT_REG_s            IdspPllPostscaler;      /* 0x1F4(RW): IDSP pll post-scaler (LSB 8-bit) */
    volatile UINT32                                 Reserved22;             /* 0x1F8: Reserved */
    volatile UINT32                                 IoCtrlMisc;             /* 0x1FC(RW): IO Control for all the pads other than 0x200-0x218 (without drive strength & pull-up/down control) */

    volatile UINT32                                 Reserved23;             /* 0x200: Reserved */
    volatile UINT32                                 IoCtrlSmio[4];          /* 0x204-0x210(RW): IO Control for SMIO (without drive strength & pull-up/down control) */
    volatile UINT32                                 Reserved24;             /* 0x214: Reserved */
    volatile AMBA_IO_CTRL_REG_s                     IoCtrlVin;              /* 0x218(RW): IO Control for VIN output pins (without drive strength & pull-up/down control) */
    volatile AMBA_IO_PAD_CTRL1_REG_s                IoPadCtrl1;             /* 0x21C(RW): I/O pad control register 1 */
    volatile UINT32                                 DdrClkDebugDiv;         /* 0x220(RW): Divider of clk_ddr_init */
    volatile UINT32                                 DdrDividerReset;        /* 0x224(RW): DDRIO clock divider reset control */
    volatile AMBA_SYS_RESET1_REG_s                  SysReset1;              /* 0x228(RW): System software reset 1 */
    volatile UINT32                                 Reserved25;             /* 0x22C: Reserved */
    volatile AMBA_PLL_DIVIDER_4BIT_REG_s            Vin0ClkDivider;         /* 0x230(RW): Vin0 clock divider */
    volatile UINT32                                 Reserved26[8];          /* 0x234-0x250: Reserved */
    volatile UINT32                                 RctTimer0Counter;       /* 0x254(RO): Free running timer/counter on reference clock */
    volatile UINT32                                 RctTimer0Ctrl;          /* 0x258(RW): RCT timer control (LSB 2-bit) */
    volatile AMBA_PLL_VALID_STATUS_REG_s            PllRawValidStatus;      /* 0x25C(RO): Raw valid flags of all the plls */
    volatile UINT32                                 WdtStateClearProtect;   /* 0x260(RW): Protection to clear each flag in WdtResetState */
    volatile AMBA_PLL_CTRL_REG_s                    Cortex0PllCtrl;         /* 0x264(RW): Cortex0/CR52 pll main configuration */
    volatile UINT32                                 Cortex0PllFraction;     /* 0x268(RW): Cortex0/CR52 pll fractional part of fractional-N mode */
    volatile UINT32                                 Cortex0PllCtrl2;        /* 0x26C(RW): Cortex0/CR52 pll sub configuration */
    volatile UINT32                                 Cortex0PllCtrl3;        /* 0x270(RW): Cortex0/CR52 pll misc configuration */
    volatile UINT32                                 Cortex0PllDebug;        /* 0x274(RO): Debug status of cortex0/cr52 pll */
    volatile AMBA_PLL_CTRL_REG_s                    Cortex1PllCtrl;         /* 0x278(RW): Cortex1/CA53 pll main configuration */
    volatile UINT32                                 Cortex1PllFraction;     /* 0x27C(RW): Cortex1/CA53 pll fractional part of fractional-N mode */
    volatile UINT32                                 Cortex1PllCtrl2;        /* 0x280(RW): Cortex1/CA53 pll sub configuration */
    volatile UINT32                                 Cortex1PllCtrl3;        /* 0x284(RW): Cortex1/CA53 pll misc configuration */
    volatile UINT32                                 Cortex1PllDebug;        /* 0x288(RO): Debug status of cortex1/ca53 pll */
    volatile UINT32                                 Reserved27[7];          /* 0x28C-0x2A4: Reserved */
    volatile AMBA_PLL_DIVIDER_8BIT_REG_s            GtxClkDivider;          /* 0x2A8(RW): Gtx clock divider */
    volatile UINT32                                 GtxClkDivSrcSelect;     /* 0x2AC(RW): Gtx clock divider source select */
    volatile UINT32                                 GtxExtClk;              /* 0x2B0(RW): External/Internal clock selection for Gtx */
    volatile UINT32                                 Reserved28[3];          /* 0x2B4-0x2BC: Reserved */
    volatile UINT32                                 UsbPhySelect;           /* 0x2C0(RW): Mode selection of usb phy */
    volatile UINT32                                 UsbPhyCtrl0;            /* 0x2C4(RW): Usb phy control register 0 */
    volatile UINT32                                 UsbPhyCtrl1;            /* 0x2C8(RW): Usb phy control register 1 */
    volatile UINT32                                 UsbReset;               /* 0x2CC(RW): Usb phy reset control */
    volatile UINT32                                 Reserved29;             /* 0x2D0: Reserved */
    volatile AMBA_USB_CHARGE_CTRL_REG_s             UsbChargeCtrl;          /* 0x2D4(RW): Usb phy battery charging control */
    volatile AMBA_CLK_DISABLE_REG_s                 ClkDisableCtrl;         /* 0x2D8(RW): Clocks disable control */
    volatile UINT32                                 CoreClkSrcSelect;       /* 0x2DC(RW): Core clock source selection (LSB 2-bit) */
    volatile UINT32                                 Reserved30;             /* 0x2E0: Reserved */
    volatile UINT32                                 DdrClkSrcSelect;        /* 0x2E4(RW): DDR clock source selection (LSB 2-bit) */
    volatile UINT32                                 Reserved31[6];          /* 0x2E8-0x2FC: Reserved */

    volatile UINT32                                 OtpSysConfig[2];        /* 0x300-0x304(RO): Read port for sysconfig data from OTP */
    volatile UINT32                                 Reserved32[3];          /* 0x308-0x310: Reserved */
    volatile UINT32                                 Gpio0DriveStrength[2];  /* 0x314-0x318(RW): GPIO Pin 0-31 Driving Strength */
    volatile UINT32                                 Gpio1DriveStrength[2];  /* 0x31C-0x320(RW): GPIO Pin 32-63 Driving Strength */
    volatile UINT32                                 Gpio2DriveStrength[2];  /* 0x324-0x328(RW): GPIO Pin 64-95 Driving Strength */
    volatile UINT32                                 Gpio3DriveStrength[2];  /* 0x32C-0x330(RW): GPIO Pin 96-127 Driving Strength */
    volatile UINT32                                 Reserved33[5];          /* 0x334-0x344: Reserved */
    volatile AMBA_RCT_CLKMON_OBSER_1_12_REG_s       ClkMonGclkDbgCount;     /* 0x348(RW): Counting value of gclk_dbg clock monitor (LSB 12-bit) */
    volatile AMBA_RCT_CLKMON_CTRL_1_12_REG_s        ClkMonGclkDbgCtrl;      /* 0x34C(RW): Configuration of gclk_dbg clock monitor */
    volatile UINT32                                 SafetyErrorState;       /* 0x350(RW): Current safety error state */
    volatile UINT32                                 SafetyErrorMask;        /* 0x354(RW): Mask of safety error state */
    volatile UINT32                                 SafetyErrorClear;       /* 0x358(RW): Clear safety error state */
    volatile UINT32                                 SafetyErrorInject;      /* 0x35C(RW): Inject safety error */
    volatile AMBA_RCT_CLKMON_OBSER_1_12_REG_s       ClkMonMiscCoreCount;    /* 0x360(RO): Counting value of pll_out_core clock monitor */
    volatile AMBA_RCT_CLKMON_OBSER_1_12_REG_s       ClkMonMiscSDCount;      /* 0x364(RO): Counting value of pll_out_sd clock monitor */
    volatile AMBA_RCT_CLKMON_OBSER_1_12_REG_s       ClkMonMiscENETCount;    /* 0x368(RO): Counting value of pll_out_enet clock monitor */
    volatile AMBA_RCT_CLKMON_OBSER_1_12_REG_s       ClkMonMiscAudioCount;   /* 0x36C(RO): Counting value of pll_out_audio clock monitor */
    volatile AMBA_RCT_CLKMON_OBSER_1_12_REG_s       ClkMonMiscGclkSOCount;  /* 0x370(RO): Counting value of gclk_so clock monitor */
    volatile AMBA_RCT_CLKMON_OBSER_1_12_REG_s       ClkMonMiscDDRCount;     /* 0x374(RO): Counting value of pll_out_ddr clock monitor */
    volatile AMBA_RCT_CLKMON_OBSER_1_12_REG_s       ClkMonMiscGclkSO2Count; /* 0x378(RO): Counting value of gclk_so2 clock monitor */
    volatile AMBA_RCT_CLKMON_OBSER_1_15_REG_s       ClkMonMiscVideoACount;  /* 0x37C(RO): Counting value of pll_out_video_a clock monitor */
    volatile AMBA_RCT_CLKMON_OBSER_1_15_REG_s       ClkMonMiscVideoBCount;  /* 0x380(RO): Counting value of pll_out_video_b clock monitor */
    volatile AMBA_RCT_CLKMON_OBSER_1_12_REG_s       ClkMonMiscCortex0Count; /* 0x384(RO): Counting value of pll_out_cortex0 clock monitor */
    volatile AMBA_RCT_CLKMON_OBSER_1_12_REG_s       ClkMonMiscCortex1Count; /* 0x388(RO): Counting value of pll_out_cortex1 clock monitor */
    volatile AMBA_RCT_CLKMON_OBSER_1_12_REG_s       ClkMonMiscFEXCount;     /* 0x38C(RO): Counting value of pll_out_cortex1 clock monitor */
    volatile AMBA_RCT_CLKMON_OBSER_1_12_REG_s       ClkMonMiscVisionCount;  /* 0x390(RO): Counting value of pll_out_vision clock monitor */
    volatile AMBA_RCT_CLKMON_OBSER_1_12_REG_s       ClkMonMiscNANDCount;    /* 0x394(RO): Counting value of pll_out_nand clock monitor */
    volatile AMBA_RCT_CLKMON_OBSER_1_12_REG_s       ClkMonMiscIDSPCount;    /* 0x398(RO): Counting value of pll_out_idsp clock monitor */
    volatile AMBA_RCT_CLKMON_OBSER_1_12_REG_s       ClkMonMiscDDRInitCount; /* 0x39C(RO): Counting value of ddr_init clock monitor */
    volatile AMBA_RCT_CLKMON_OBSER_1_12_REG_s       ClkMonMiscSOVIN0Count;  /* 0x3A0(RO): Counting value of gclk_so_vin0 clock monitor */
    volatile AMBA_RCT_CLKMON_OBSER_1_12_REG_s       ClkMonMiscSOVIN1Count;  /* 0x3A4(RO): Counting value of gclk_so_vin1 clock monitor */
    volatile AMBA_RCT_CLKMON_CTRL_1_10_REG_s        ClkMonPllOutCoreCtrl;   /* 0x3A8(RW): Configuration of pll_out_core clock monitor */
    volatile AMBA_RCT_CLKMON_CTRL_1_12_REG_s        ClkMonPllOutSDCtrl;     /* 0x3AC(RW): Configuration of pll_out_sd clock monitor */
    volatile AMBA_RCT_CLKMON_CTRL_1_12_REG_s        ClkMonPllOutENETCtrl;   /* 0x3B0(RW): Configuration of pll_out_enet clock monitor */
    volatile AMBA_RCT_CLKMON_CTRL_1_12_REG_s        ClkMonPllOutAudioCtrl;  /* 0x3B4(RW): Configuration of pll_out_audio clock monitor */
    volatile AMBA_RCT_CLKMON_CTRL_1_12_REG_s        ClkMonGclkSOCtrl;       /* 0x3B8(RW): Configuration of gclk_so clock monitor */
    volatile AMBA_RCT_CLKMON_CTRL_1_12_REG_s        ClkMonPllOutDDRCtrl;    /* 0x3BC(RW): Configuration of pll_out_ddr clock monitor */
    volatile AMBA_RCT_CLKMON_CTRL_1_12_REG_s        ClkMonGclkSO2Ctrl;      /* 0x3C0(RW): Configuration of gclk_so2 clock monitor */
    volatile AMBA_RCT_CLKMON_CTRL_1_15_REG_s        ClkMonPllOutVideoACtrl; /* 0x3C4(RW): Configuration of pll_out_video_a clock monitor */
    volatile AMBA_RCT_CLKMON_CTRL_1_15_REG_s        ClkMonPllOutVideoBCtrl; /* 0x3C8(RW): Configuration of pll_out_video_b clock monitor */
    volatile AMBA_RCT_CLKMON_CTRL_1_12_REG_s        ClkMonPllOutCortex0Ctrl;/* 0x3CC(RW): Configuration of pll_out_cortex0 clock monitor */
    volatile AMBA_RCT_CLKMON_CTRL_1_12_REG_s        ClkMonPllOutCortex1Ctrl;/* 0x3D0(RW): Configuration of pll_out_cortex1 clock monitor */
    volatile AMBA_RCT_CLKMON_CTRL_1_12_REG_s        ClkMonPllOutFEXCtrl;    /* 0x3D4(RW): Configuration of pll_out_fex clock monitor */
    volatile AMBA_RCT_CLKMON_CTRL_1_12_REG_s        ClkMonPllOutVisionCtrl; /* 0x3D8(RW): Configuration of pll_out_vision clock monitor */
    volatile AMBA_RCT_CLKMON_CTRL_1_12_REG_s        ClkMonPllOutNANDCtrl;   /* 0x3DC(RW): Configuration of pll_out_nand clock monitor */
    volatile AMBA_RCT_CLKMON_CTRL_1_12_REG_s        ClkMonPllOutIDSPCtrl;   /* 0x3E0(RW): Configuration of pll_out_idsp clock monitor */
    volatile AMBA_RCT_CLKMON_CTRL_1_12_REG_s        ClkMonPllOutDDRInitCtrl;/* 0x3E4(RW): Configuration of ddr_init clock monitor */
    volatile AMBA_RCT_CLKMON_CTRL_1_12_REG_s        ClkMonGclkSOVIN0Ctrl;   /* 0x3E8(RW): Configuration of gclk_so_vin0 clock monitor */
    volatile AMBA_RCT_CLKMON_CTRL_1_12_REG_s        ClkMonGclkSOVIN1Ctrl;   /* 0x3EC(RW): Configuration of pll_out_vin1 clock monitor */
    volatile UINT32                                 Reserved34[2];          /* 0x3F0-0x3F4: Reserved */
    volatile AMBA_PLL_SCALER_CTRL_REG_s             PllScalerBypass;        /* 0x3F8(RW): Misc pll divider bypass control */
    volatile AMBA_PLL_SCALER_CTRL_REG_s             PllScalerReset;         /* 0x3FC(RW): Misc pll divider reset control */

    volatile UINT32                                 Reserved35[12];         /* 0x400-0x42C: Reserved */
    volatile AMBA_PLL_DIVIDER_16BIT_REG_s           Sd1ClkDivider;          /* 0x430(RW): SD1/SDIO0 Clock Divider (LSB 17-bit) */
    volatile UINT32                                 Reserved36;             /* 0x434: Reserved */
    volatile UINT32                                 Gpio4DriveStrength[2];  /* 0x438-0x43C: GPIO Pin 128-159 Driving Strength */
    volatile UINT32                                 Gpio5DriveStrength[2];  /* 0x440-0x444: GPIO Pin 160-181 Driving Strength */
    volatile UINT32                                 Reserved37[2];          /* 0x448-0x44C: Reserved */
    volatile UINT32                                 MiscDriveStrength[2];   /* 0x450-0x454(RW): Misc Pin (clk_au[1], detect_vbus[2], clk_si[3], clk_si1[4]) Driving Strength */
    volatile UINT32                                 Reserved38[11];         /* 0x458-0x480: Reserved */
    volatile AMBA_MUX_LVDS_SPCLK_SELECT_REG_s       MuxedLvdsSpclk;         /* 0x484(RW): External Reference Clock Selection for muxed_lvds_spclk */
    volatile AMBA_MUX_MIPI_RX_BYTE_CLK_SELECT_REG_s MuxedMipiRxByteClkHS;   /* 0x488(RW): External Reference Clock Selection for muxed_RxByteClkHS */
    volatile UINT32                                 Reserved39[2];          /* 0x48C-0x490: Reserved */
    volatile UINT32                                 RctTimer1Counter;       /* 0x494(RO): Free running timer/counter on reference clock */
    volatile UINT32                                 RctTimer1Ctrl;          /* 0x498(RW): RCT timer control (LSB 2-bit) */
    volatile UINT32                                 Reserved40[4];          /* 0x49C-0x4A8: Reserved */
    volatile AMBA_PLL_CTRL_REG_s                    SdPllCtrl;              /* 0x4AC(RW): SD pll main configuration */
    volatile UINT32                                 SdPllFraction;          /* 0x4B0(RW): SD pll fractional part of fractional-N mode */
    volatile UINT32                                 SdPllCtrl2;             /* 0x4B4(RW): SD pll sub configuration */
    volatile UINT32                                 SdPllCtrl3;             /* 0x4B8(RW): SD pll misc configuration */
    volatile UINT32                                 SdPllDebug;             /* 0x4BC(RO): Debug status of sd pll */
    volatile AMBA_SD_PHY_CTRL0_REG_s                Sd0PhyCtrl0;            /* 0x4C0(RW): SD0/SD phy control 0 */
    volatile AMBA_SD_PHY_CTRL1_REG_s                Sd0PhyCtrl1;            /* 0x4C4(RW): SD0/SD phy control 1 */
    volatile AMBA_SD_PHY_CTRL0_REG_s                Sd1PhyCtrl0;            /* 0x4C8(RW): SD1/SDIO0 phy control 0 */
    volatile AMBA_SD_PHY_CTRL1_REG_s                Sd1PhyCtrl1;            /* 0x4CC(RW): SD1/SDIO0 phy control 1 */
    volatile AMBA_SD_CLK_SRC_SELECT_REG_s           SdClkSrcSelect;         /* 0x4D0(RW): SD clock source selection */
    volatile UINT32                                 Reserved41[6];          /* 0x4D4-0x4E8: Reserved */
    volatile AMBA_SPI_CLK_REF_SELECT_REG_s          SpiSlaveRefClkSelect;   /* 0x4EC(RW): SPI slave clock reference selection (LSB 1-bit) */
    volatile AMBA_SD_PHY_OBSERV0_REG_s              SdPhyObsv0;             /* 0x4F0(RO): SD PHY Observation */
    volatile UINT32                                 Reserved42;             /* 0x4F4: Reserved */
    volatile AMBA_CORTEX_CLK_SRC_SELECT_REG_s       Cortex0ClkSrcSelect;    /* 0x4F8(RW): Clock source for cortex0/cr52 (LSB 2-bit) */
    volatile AMBA_CORTEX_CLK_SRC_SELECT_REG_s       Cortex1ClkSrcSelect;    /* 0x4FC(RW): Clock source for cortex1/ca53 (LSB 2-bit) */

    volatile UINT32                                 Reserved43[6];          /* 0x500-0x514: Reserved */
    volatile AMBA_PLL_DIVIDER_24BIT_REG_s           SpiNorClkDivider;       /* 0x518(RW): Control register for SPI-NOR divider (LSB 25 bits) */
    volatile AMBA_SPI_NOR_CLK_REF_SELECT_REG_s      SpiNorRefClkSelect;     /* 0x51C(RW): SPI-NOR clock reference selection (LSB 2-bit) */
    volatile AMBA_PLL_CTRL_REG_s                    EnetPllCtrl;            /* 0x520(RW): Ethernet pll main configuration */
    volatile UINT32                                 EnetPllFraction;        /* 0x524(RW): Ethernet pll fractional part of fractional-N mode */
    volatile UINT32                                 EnetPllCtrl2;           /* 0x528(RW): Ethernet pll sub configuration */
    volatile UINT32                                 EnetPllCtrl3;           /* 0x52C(RW): Ethernet pll misc configuration */
    volatile UINT32                                 EnetPllDebug;           /* 0x530(RO): Debug status of ethernet pll */
    volatile AMBA_PLL_DIVIDER_4BIT_REG_s            Enet0ClkDivider;        /* 0x534(RW): Ethernet0 clock divider */
    volatile UINT32                                 OtpWaferInfo[2];        /* 0x538-0x53C(RO): Read port for wafer info data from OTP */
    volatile AMBA_PLL_DIVIDER_4BIT_REG_s            Enet1ClkDivider;        /* 0x540(RW): Ethernet1 clock divider */
    volatile UINT32                                 Reserved44[10];         /* 0x544-0x568: Reserved */
    volatile AMBA_CAN_CLK_SRC_SELECT_REG_s          CanClkSrcSelect;        /* 0x56C(RW): CAN clock source selection (LSB 2-bit) */
    volatile AMBA_PLL_DIVIDER_8BIT_REG_s            CanClkDivider;          /* 0x570(RW): CAN clock divider */
    volatile AMBA_PLL_DIVIDER_4BIT_REG_s            Vin1ClkDivider;         /* 0x574(RW): Vin1 clock divider */
    volatile AMBA_PLL_DIVIDER_4BIT_REG_s            Sensor2PllPostscaler;   /* 0x578(RW): Vin1/Sensor2 pll post-scaler (LSB 8-bit) */
    volatile UINT32                                 Reserved45[2];          /* 0x57C-0X580: Reserved */
    volatile UINT32                                 MipiDsi0Ctrl0;          /* 0x584(RW): Vout0/Video_A mipi dsi interface control */
    volatile UINT32                                 MipiDsi0Ctrl1;          /* 0x588(RW): Vout0/Video_A mipi dsi interface control */
    volatile UINT32                                 MipiDsi0Ctrl2;          /* 0x58C(RW): Vout0/Video_A mipi dsi interface control */
    volatile AMBA_MIPI_DSI_CTRL3_REG_s              MipiDsi0Ctrl3;          /* 0x590(RW): Vout0/Video_A mipi dsi interface control */
    volatile AMBA_MIPI_DSI_CTRL4_REG_s              MipiDsi0Ctrl4;          /* 0x594(RW): Vout0/Video_A mipi dsi interface control */
    volatile UINT32                                 MipiDsi0Ctrl5;          /* 0x598(RW): Vout0/Video_A mipi dsi interface control */
    volatile AMBA_MIPI_DSI_AUX_CTRL_REG_s           MipiDsi0AuxCtrl;        /* 0x59C(RW): Vout0/Video_A mipi dsi interface aux control */
    volatile UINT32                                 MipiDsi0Obsv[3];        /* 0x5A0-0x5A8(RW): Vout0/Video_A mipi dsi interface observation */
    volatile UINT32                                 Reserved46[2];          /* 0x5AC-0X5B0: Reserved */
    volatile UINT32                                 MipiDsi1Ctrl0;          /* 0x5B4(RW): Vout1/Video_B mipi dsi interface control */
    volatile UINT32                                 MipiDsi1Ctrl1;          /* 0x5B8(RW): Vout1/Video_B mipi dsi interface control */
    volatile UINT32                                 MipiDsi1Ctrl2;          /* 0x5BC(RW): Vout1/Video_B mipi dsi interface control */
    volatile AMBA_MIPI_DSI_CTRL3_REG_s              MipiDsi1Ctrl3;          /* 0x5C0(RW): Vout1/Video_B mipi dsi interface control */
    volatile AMBA_MIPI_DSI_CTRL4_REG_s              MipiDsi1Ctrl4;          /* 0x5C4(RW): Vout1/Video_B mipi dsi interface control */
    volatile UINT32                                 MipiDsi1Ctrl5;          /* 0x5C8(RW): Vout1/Video_B mipi dsi interface control */
    volatile AMBA_MIPI_DSI_AUX_CTRL_REG_s           MipiDsi1AuxCtrl;        /* 0x5CC(RW): Vout1/Video_B mipi dsi interface aux control */
    volatile UINT32                                 MipiDsi1Obsv[3];        /* 0x5D0-0x5D8(RW): Vout1/Video_B mipi dsi interface observation */
    volatile UINT32                                 Reserved47[9];          /* 0x5DC-0X5FC: Reserved */

    volatile UINT32                                 Reserved48[13];         /* 0x600-0x630: Reserved */
    volatile UINT32                                 UsbPhyCtrl2;            /* 0x634(RW): Usb phy control register 2 */
    volatile UINT32                                 Reserved49;             /* 0x638: Reserved */
    volatile UINT32                                 UsbPhyCtrl3;            /* 0x63C(RW): Usb phy control register 3 */
    volatile UINT32                                 Reserved50[9];          /* 0x640-0x660: Reserved */
    volatile AMBA_SD_PHY_CTRL2_REG_s                Sd0PhyCtrl2;            /* 0x664(RW): SD0/SD phy control 2 */
    volatile AMBA_SD_PHY_CTRL2_REG_s                Sd1PhyCtrl2;            /* 0x668(RW): SD1/SDIO0 phy control 2 */
    volatile UINT32                                 OtpChipId;              /* 0x66C(RO): Read port for chip id data from OTP */
    volatile UINT32                                 Reserved51[24];         /* 0x670-0x6CC: Reserved */
    volatile AMBA_PLL_CTRL_REG_s                    FexPllCtrl;             /* 0x6D0(RW): Fex pll main configuration */
    volatile UINT32                                 FexPllFraction;         /* 0x6D4(RW): Fex pll fractional part of fractional-N mode */
    volatile UINT32                                 FexPllCtrl2;            /* 0x6D8(RW): Fex pll sub configuration */
    volatile UINT32                                 FexPllCtrl3;            /* 0x6DC(RW): Fex pll misc configuration */
    volatile UINT32                                 FexPllDebug;            /* 0x6E0(RO): Debug status of Fex pll */
    volatile AMBA_PLL_CTRL_REG_s                    VisionPllCtrl;          /* 0x6E4(RW): Vision pll main configuration */
    volatile UINT32                                 VisionPllFraction;      /* 0x6E8(RW): Vision pll fractional part of fractional-N mode */
    volatile UINT32                                 VisionPllCtrl2;         /* 0x6EC(RW): Vision pll sub configuration */
    volatile UINT32                                 VisionPllCtrl3;         /* 0x6F0(RW): Vision pll misc configuration */
    volatile UINT32                                 VisionPllObsv;          /* 0x6F4(RO): Vision pll output observation */
    volatile AMBA_PLL_CTRL_REG_s                    NandPllCtrl;            /* 0x6F8(RW): Nand pll main configuration */
    volatile UINT32                                 NandPllFraction;        /* 0x6FC(RW): Nand pll fractional part of fractional-N mode */

    volatile UINT32                                 NandPllCtrl2;           /* 0x700(RW): Nand pll sub configuration */
    volatile UINT32                                 NandPllCtrl3;           /* 0x704(RW): Nand pll misc configuration */
    volatile UINT32                                 NandPllObsv;            /* 0x708(RO): Nand pll output observation */
    volatile AMBA_RING_OSC_CTRL_REG_s               RingOscCtrl;            /* 0x70C(RW): Ring oscillator ctrl */
    volatile AMBA_RING_OSC_OBSV_REG_s               RingOscObsv;            /* 0x710(RO): Ring oscillator observation */
    volatile AMBA_PLL_DIVIDER_24BIT_REG_s           Uart0ClkDivider;        /* 0x714(RW): Uart0 clock divider */
    volatile AMBA_PLL_DIVIDER_24BIT_REG_s           Uart1ClkDivider;        /* 0x718(RW): Uart1 clock divider */
    volatile AMBA_PLL_DIVIDER_24BIT_REG_s           Uart2ClkDivider;        /* 0x71C(RW): Uart2 clock divider */
    volatile AMBA_PLL_DIVIDER_24BIT_REG_s           Uart3ClkDivider;        /* 0x720(RW): Uart3 clock divider */
    volatile UINT32                                 Reserved52[2];          /* 0x724-0x728: Reserved */
    volatile AMBA_UART_CLK_SRC_SELECT_REG_s         Uart0ClkSrcSelect;      /* 0x72C(RW): Uart0 clock source selection */
    volatile AMBA_UART_CLK_SRC_SELECT_REG_s         Uart1ClkSrcSelect;      /* 0x730(RW): Uart1 clock source selection */
    volatile AMBA_UART_CLK_SRC_SELECT_REG_s         Uart2ClkSrcSelect;      /* 0x734(RW): Uart2 clock source selection */
    volatile AMBA_UART_CLK_SRC_SELECT_REG_s         Uart3ClkSrcSelect;      /* 0x738(RW): Uart3 clock source selection */
    volatile UINT32                                 Reserved53[2];          /* 0x73C-0x740: Reserved */
    volatile AMBA_VOUT_CLK_SELECT_REG_s             VoutClkSelect;          /* 0x744(RW): Vout clock selection register */
    volatile AMBA_VIN_CLK_SELECT_REG_s              VinClkSelect;           /* 0x748(RW): Vin clock selection register */
    volatile AMBA_PLL_CTRL_REG_s                    Sensor2PllCtrl;         /* 0x74C(RW): Vin1/Sensor2 pll main configuration */
    volatile UINT32                                 Sensor2PllCtrl2;        /* 0x750(RW): Vin1/Sensor2 pll sub configuration */
    volatile UINT32                                 Sensor2PllCtrl3;        /* 0x754(RW): Vin1/Sensor2 pll misc configuration */
    volatile UINT32                                 Sensor2PllFraction;     /* 0x758(RW): Vin1/Sensor2 pll fractional part of fractional-N mode */
    volatile UINT32                                 Sensor2PllDebug;        /* 0x75C(RO): Debug status of Vin1/Sensor2 pll */
    volatile UINT32                                 Reserved54[4];          /* 0x760-0x76C: Reserved */
    volatile UINT32                                 OtpT2VData;             /* 0x770: Obersvation register for OTP T2V Data out */
    volatile UINT32                                 OtpWriteData;           /* 0x774: Obersvation register for OTP Write Data out partition */
    volatile UINT32                                 Reserved55[2];          /* 0x778-0x77C: Reserved */
    volatile UINT32                                 DataRetention[5];       /* 0x780-0x790(RW): General purpose storage to retain data through system software reset */
    volatile AMBA_PLL_DIVIDER_4BIT_REG_s            SysCount0ClkDivider;    /* 0x794(RW): System counter0(for generic timer of cr52) clock divider */
    volatile UINT32                                 Vin1ClkSelect;          /* 0x798(RW): Pll selection for each vin1 clock */
    volatile AMBA_ENET_CLK_CONFIG_REG_s             EnetClkConfig;          /* 0x79C(RW): Enet clock configuration */
    volatile AMBA_PLL_DIVIDER_4BIT_REG_s            SysCount1ClkDivider;    /* 0x7A0(RW): System counter1(for generic timer of ca53) clock divider */
    volatile AMBA_LBIST_CLK_SRC_SELECT_REG_s        LbistClkSrcSelect;      /* 0x7A4(RW): LBIST clock source selection */
    volatile AMBA_PLL_DIVIDER_8BIT_REG_s            LbistClkDivider;        /* 0x7A8(RW): LBIST clock divider */
    volatile AMBA_IST_CLK_SRC_SELECT_REG_s          IstClkSrcSelect;        /* 0x7AC(RW): IST clock source selection */
    volatile AMBA_PLL_DIVIDER_8BIT_REG_s            IstClkDivider;          /* 0x7B0(RW): IST clock divider */
    volatile UINT32                                 IstReset;               /* 0x7B4(RW): IST reset control */
    volatile UINT32                                 Reserved56[2];          /* 0x7B8-0x7BC: Reserved */
    volatile AMBA_PS_MON_CTRL1_REG_s                PsMon0Ctrl1;            /* 0x7C0(RW): Power supply monitor 0 control1 registers */
    volatile AMBA_PS_MON_CTRL2_REG_s                PsMon0Ctrl2;            /* 0x7C4(RW): Power supply monitor 0 control2 registers */
    volatile AMBA_PS_MON_OBSV_REG_s                 PsMon0Obsv;             /* 0x7C8(RW): Power supply monitor 0 observation registers */
    volatile AMBA_PS_MON_CTRL1_REG_s                PsMon1Ctrl1;            /* 0x7CC(RW): Power supply monitor 1 control1 registers */
    volatile AMBA_PS_MON_CTRL2_REG_s                PsMon1Ctrl2;            /* 0x7D0(RW): Power supply monitor 1 control2 registers */
    volatile AMBA_PS_MON_OBSV_REG_s                 PsMon1Obsv;             /* 0x7D4(RW): Power supply monitor 1 observation registers */
    volatile AMBA_PS_MON_CTRL1_REG_s                PsMon2Ctrl1;            /* 0x7D8(RW): Power supply monitor 2 control1 registers */
    volatile AMBA_PS_MON_CTRL2_REG_s                PsMon2Ctrl2;            /* 0x7DC(RW): Power supply monitor 2 control2 registers */
    volatile AMBA_PS_MON_OBSV_REG_s                 PsMon2Obsv;             /* 0x7E0(RW): Power supply monitor 2 observation registers */
    volatile AMBA_PS_MON_CTRL1_REG_s                PsMon3Ctrl1;            /* 0x7E4(RW): Power supply monitor 3 control1 registers */
    volatile AMBA_PS_MON_CTRL2_REG_s                PsMon3Ctrl2;            /* 0x7E8(RW): Power supply monitor 3 control2 registers */
    volatile AMBA_PS_MON_OBSV_REG_s                 PsMon3Obsv;             /* 0x7EC(RW): Power supply monitor 3 observation registers */
    volatile AMBA_PS_MON_CTRL1_REG_s                PsMon4Ctrl1;            /* 0x7F0(RW): Power supply monitor 4 control1 registers */
    volatile AMBA_PS_MON_CTRL2_REG_s                PsMon4Ctrl2;            /* 0x7F4(RW): Power supply monitor 4 control2 registers */
    volatile AMBA_PS_MON_OBSV_REG_s                 PsMon4Obsv;             /* 0x7F8(RW): Power supply monitor 4 observation registers */
    volatile AMBA_PS_MON_CTRL1_REG_s                PsMon5Ctrl1;            /* 0x7FC(RW): Power supply monitor 5 control1 registers */
    volatile AMBA_PS_MON_CTRL2_REG_s                PsMon5Ctrl2;            /* 0x800(RW): Power supply monitor 5 control2 registers */
    volatile AMBA_PS_MON_OBSV_REG_s                 PsMon5Obsv;             /* 0x804(RW): Power supply monitor 5 observation registers */
    volatile AMBA_PS_MON_CTRL1_REG_s                PsMon6Ctrl1;            /* 0x808(RW): Power supply monitor 6 control1 registers */
    volatile AMBA_PS_MON_CTRL2_REG_s                PsMon6Ctrl2;            /* 0x80C(RW): Power supply monitor 6 control2 registers */
    volatile AMBA_PS_MON_OBSV_REG_s                 PsMon6Obsv;             /* 0x810(RW): Power supply monitor 6 observation registers */
    volatile AMBA_PS_MON_CTRL1_REG_s                PsMon7Ctrl1;            /* 0x814(RW): Power supply monitor 7 control1 registers */
    volatile AMBA_PS_MON_CTRL2_REG_s                PsMon7Ctrl2;            /* 0x818(RW): Power supply monitor 7 control2 registers */
    volatile AMBA_PS_MON_OBSV_REG_s                 PsMon7Obsv;             /* 0x81C(RW): Power supply monitor 7 observation registers */
    volatile AMBA_PS_MON_CTRL1_REG_s                PsMon8Ctrl1;            /* 0x820(RW): Power supply monitor 8 control1 registers */
    volatile AMBA_PS_MON_CTRL2_REG_s                PsMon8Ctrl2;            /* 0x824(RW): Power supply monitor 8 control2 registers */
    volatile AMBA_PS_MON_OBSV_REG_s                 PsMon8Obsv;             /* 0x828(RW): Power supply monitor 8 observation registers */
    volatile AMBA_PS_MON_CTRL1_REG_s                PsMon9Ctrl1;            /* 0x82C(RW): Power supply monitor 9 control1 registers */
    volatile AMBA_PS_MON_CTRL2_REG_s                PsMon9Ctrl2;            /* 0x830(RW): Power supply monitor 9 control2 registers */
    volatile AMBA_PS_MON_OBSV_REG_s                 PsMon9Obsv;             /* 0x834(RW): Power supply monitor 9 observation registers */
    volatile AMBA_PS_MON_CTRL1_REG_s                PsMon10Ctrl1;           /* 0x838(RW): Power supply monitor 10 control1 registers */
    volatile AMBA_PS_MON_CTRL2_REG_s                PsMon10Ctrl2;           /* 0x83C(RW): Power supply monitor 10 control2 registers */
    volatile AMBA_PS_MON_OBSV_REG_s                 PsMon10Obsv;            /* 0x840(RW): Power supply monitor 10 observation registers */
    volatile AMBA_PS_MON_CTRL1_REG_s                PsMon11Ctrl1;           /* 0x844(RW): Power supply monitor 11 control1 registers */
    volatile AMBA_PS_MON_CTRL2_REG_s                PsMon11Ctrl2;           /* 0x848(RW): Power supply monitor 11 control2 registers */
    volatile AMBA_PS_MON_OBSV_REG_s                 PsMon11Obsv;            /* 0x84C(RW): Power supply monitor 11 observation registers */
    volatile AMBA_PS_MON_CTRL1_REG_s                PsMon12Ctrl1;           /* 0x850(RW): Power supply monitor 12 control1 registers */
    volatile AMBA_PS_MON_CTRL2_REG_s                PsMon12Ctrl2;           /* 0x854(RW): Power supply monitor 12 control2 registers */
    volatile AMBA_PS_MON_OBSV_REG_s                 PsMon12Obsv;            /* 0x858(RW): Power supply monitor 12 observation registers */
    volatile AMBA_PS_MON_CTRL1_REG_s                PsMon13Ctrl1;           /* 0x85C(RW): Power supply monitor 13 control1 registers */
    volatile AMBA_PS_MON_CTRL2_REG_s                PsMon13Ctrl2;           /* 0x860(RW): Power supply monitor 13 control2 registers */
    volatile AMBA_PS_MON_OBSV_REG_s                 PsMon13Obsv;            /* 0x864(RW): Power supply monitor 13 observation registers */
    volatile AMBA_PS_MON_CTRL1_REG_s                PsMon14Ctrl1;           /* 0x868(RW): Power supply monitor 14 control1 registers */
    volatile AMBA_PS_MON_CTRL2_REG_s                PsMon14Ctrl2;           /* 0x86C(RW): Power supply monitor 14 control2 registers */
    volatile AMBA_PS_MON_OBSV_REG_s                 PsMon14Obsv;            /* 0x870(RW): Power supply monitor 14 observation registers */
    volatile AMBA_PS_MON_CTRL1_REG_s                PsMonLv0Ctrl1;          /* 0x874(RW): Power supply monitor lv0 control1 registers */
    volatile AMBA_PS_MON_CTRL2_REG_s                PsMonLv0Ctrl2;          /* 0x878(RW): Power supply monitor lv0 control2 registers */
    volatile AMBA_PS_MON_OBSV_REG_s                 PsMonLv0Obsv;           /* 0x87C(RW): Power supply monitor lv0 observation registers */
    volatile AMBA_PS_MON_CTRL1_REG_s                PsMonLv1Ctrl1;          /* 0x880(RW): Power supply monitor lv1 control1 registers */
    volatile AMBA_PS_MON_CTRL2_REG_s                PsMonLv1Ctrl2;          /* 0x884(RW): Power supply monitor lv1 control2 registers */
    volatile AMBA_PS_MON_OBSV_REG_s                 PsMonLv1Obsv;           /* 0x888(RW): Power supply monitor lv1 observation registers */
    volatile UINT32                                 Reserved57;             /* 0x88C: Reserved */
    volatile AMBA_PS_MON_CTRL1_REG_s                PsMon15Ctrl1;           /* 0x890(RW): Power supply monitor 15 control1 registers */
    volatile AMBA_PS_MON_CTRL2_REG_s                PsMon15Ctrl2;           /* 0x894(RW): Power supply monitor 15 control2 registers */
    volatile AMBA_PS_MON_OBSV_REG_s                 PsMon15Obsv;            /* 0x898(RW): Power supply monitor 15 observation registers */
} AMBA_RCT_REG_s;

/*
 * MIPI/VIN PHY Registers
 */
typedef struct {
    volatile AMBA_DPHY_OBSERVE0_REG_u               Dphy0Obsv0;             /* 0x000(RO): MIPI DPHY0 Observation (32-bit) */
    volatile AMBA_DPHY_OBSERVE1_REG_u               Dphy0Obsv1;             /* 0x004(RO): MIPI DPHY0 Observation Rx and Error related (32-bit) */
    volatile AMBA_DPHY_OBSERVE2_REG_u               Dphy0Obsv2;             /* 0x008(RO): MIPI DPHY0 Observation Rx related (32-bit) */
    volatile UINT32                                 Dphy0Obsv3;             /* 0x00C(RWC): MIPI DPHY0 Observation shifted received data output of data lane (32-bit) */
    volatile AMBA_DPHY_OBSERVE4_REG_u               Dphy0Obsv4;             /* 0x010(RWC): MIPI DPHY0 Observation Rx state related (32-bit) */
    volatile AMBA_DPHY_OBSERVE5_REG_u               Dphy0Obsv5;             /* 0x014(RO): MIPI DPHY0 Observation Rx and Errorrelated (32-bit) */
    volatile AMBA_DPHY_OBSERVE6_REG_u               Dphy0Obsv6;             /* 0x018(RO): MIPI DPHY0 Observation Rx related (32-bit) */
    volatile UINT32                                 Dphy0Obsv7;             /* 0x01C(RO): MIPI DPHY0 Observation dphy_rct_lb_errcnt0 (32-bit) */
    volatile UINT32                                 Dphy0Obsv8;             /* 0x020(RO): MIPI DPHY0 Observation dphy_rct_lb_errcnt1 (32-bit) */
    volatile UINT32                                 Dphy0Obsv9;             /* 0x024(RO): MIPI DPHY0 Observation dphy_rct_lb_errcnt2 (32-bit) */
    volatile AMBA_DPHY_CTRL0_REG_u                  Dphy0Ctrl0;             /* 0x028(RW): MIPI DPHY0 Control 0 */
    volatile AMBA_DPHY_CTRL1_REG_u                  Dphy0Ctrl1;             /* 0x02C(RW): MIPI DPHY0 Control 1 */
    volatile AMBA_DPHY_CTRL2_REG_u                  Dphy0Ctrl2;             /* 0x030(RW): MIPI DPHY0 Control 2 */
    volatile AMBA_DPHY_CTRL3_REG_u                  Dphy0Ctrl3;             /* 0x034(RW): MIPI DPHY0 Control 3 */
    volatile AMBA_DPHY_CTRL4_REG_u                  Dphy0Ctrl4;             /* 0x038(RW): MIPI DPHY0 Control 4 */
    volatile AMBA_DPHY_CTRL5_REG_u                  Dphy0Ctrl5;             /* 0x03C(RW): MIPI DPHY0 Control 5 */
    volatile AMBA_DPHY_CTRL6_REG_u                  Dphy0Ctrl6;             /* 0x040(RW): MIPI DPHY0 Control 6 */
    volatile AMBA_DPHY_CTRL7_REG_u                  Dphy0Ctrl7;             /* 0x044(RW): MIPI DPHY0 Control 7 */
    volatile AMBA_DPHY_CTRL8_REG_u                  Dphy0Ctrl8;             /* 0x048(RW): MIPI DPHY0 Control 8 */
    volatile AMBA_DPHY_CTRL9_REG_u                  Dphy0Ctrl9;             /* 0x04C(RW): MIPI DPHY0 Control 9 */
    volatile AMBA_DPHY_CTRL10_REG_u                 Dphy0Ctrl10;            /* 0x050(RW): MIPI DPHY0 Control 10 */
    volatile UINT32                                 Dphy0LbByteCnt;         /* 0x054(RW): MIPI DPHY0 Control 11 (lb counter) */
    volatile AMBA_DPHY_CTRL12_REG_u                 Dphy0Ctrl12;            /* 0x058(RW): MIPI DPHY0 Control 12 (Controls for lb and safety gated signal) */
    volatile AMBA_PHY_BIST_CTRL11_REG_u             MipiPhyBistCtrl11;      /* 0x05C(RW): Control register for mipi_phy_bist */
    volatile AMBA_PHY_BIST_CTRL12_REG_u             MipiPhyBistCtrl12;      /* 0x060(RW): Control register for mipi_phy_bist */
    volatile AMBA_PHY_BIST_OBSV11_REG_u             MipiPhyBistObsv11;      /* 0x064(RO): Observe register for mipi_phy_bist */
    volatile AMBA_PHY_BIST_OBSV12_REG_u             MipiPhyBistObsv12;      /* 0x068(RO): Observe register for mipi_phy_bist */
    volatile AMBA_PHY_BIST_OBSV13_REG_u             MipiPhyBistObsv13;      /* 0x06C(RO): Observe register for mipi_phy_bist */
    volatile AMBA_PHY_BIST_OBSV14_REG_u             MipiPhyBistObsv14;      /* 0x070(RO): Observe register for mipi_phy_bist */
    volatile AMBA_PHY_BIST_OBSV15_REG_u             MipiPhyBistObsv15;      /* 0x074(RO): Observe register for mipi_phy_bist */
    volatile AMBA_PHY_BIST_OBSV16_REG_u             MipiPhyBistObsv16;      /* 0x078(RO): Observe register for mipi_phy_bist */
    volatile AMBA_PHY_BIST_OBSV17_REG_u             MipiPhyBistObsv17;      /* 0x07C(RO): Observe register for mipi_phy_bist */
    volatile AMBA_PHY_BIST_OBSV18_REG_u             MipiPhyBistObsv18;      /* 0x080(RO): Observe register for mipi_phy_bist */
} AMBA_MIPI_REG_s;

/*
 * Defined in AmbaMmioBase.asm
 */
#ifdef CONFIG_QNX
extern AMBA_RCT_REG_s * pAmbaRCT_Reg;
#else
extern AMBA_RCT_REG_s *const pAmbaRCT_Reg;
#endif
extern AMBA_MIPI_REG_s * pAmbaMIPI_Reg;

#ifdef CONFIG_VIN_ASIL
/*
 * Defined in AmbaCSL_VIN_S.c
 */
extern AMBA_MIPI_REG_s AmbaMIPI_Mem;
#endif

#endif /* AMBA_REG_RCT_H */
