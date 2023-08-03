/**
 *  @file AmbaReg_RCT.h
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
 *  @details Definitions & Constants for B8 RCT (Reset, Clock and Test) Control Registers
 *
 */

#ifndef AMBA_B8_REG_RCT_H
#define AMBA_B8_REG_RCT_H

/*-----------------------------------------------------------------------------------------------*\
 * B8 RCT: PLL Core/Video/Sensor/Phy Control Register for PLL Integer configurations
\*-----------------------------------------------------------------------------------------------*/
typedef struct {
    UINT32  WriteEnable:                1;      /* [0]: Write Enable: 1 - enable */
    UINT32  Reserved0:                  1;      /* [1]: Reserved */
    UINT32  Bypass:                     1;      /* [2]: BYPASS: 0 - disable, 1 - enable */
    UINT32  FractionalMode:             1;      /* [3]: Integer Mode: 0 - integer mode, 1 - fractional mode */
    UINT32  ForcePllReset:              1;      /* [4]: Force PLL reset: 0 - disable, 1 - enable */
    UINT32  PowerDown:                  1;      /* [5]: Power Down the PLL: 0 - disable, 1 - enable */
    UINT32  HaltVCO:                    1;      /* [6]: Halt VCO oscillation */
    UINT32  TurnOffPhaseDetector:       1;      /* [7]: Turn off phase detector */
    UINT32  PllToutAsync:               4;      /* [11:8]: Select for pll_tout_async */
    UINT32  Sdiv:                       4;      /* [15:12]: SDIV = Slow Divider */
    UINT32  Sout:                       4;      /* [19:16]: SOUT = Scaler for PLL Output */
    UINT32  Reserved1:                  2;      /* [21:20]: Reserved */
    UINT32  DsmReset:                   1;      /* [22]: Dsm Reset */
    UINT32  Reserved2:                  1;      /* [23]: Reserved */
    UINT32  IntProg:                    7;      /* [30:24]: INTPROG[6:0] */
    UINT32  Reserved3:                  1;      /* [31]: Reserved */
} B8_PLL_CTRL_REG_s;

/*-----------------------------------------------------------------------------------------------*\
 * B8 RCT: PLL Control2 Control Register
\*-----------------------------------------------------------------------------------------------*/
typedef struct {
    UINT32  CtrlAbility:                16;     /* [15:0]: lcpll_creg_ctrl (Control Ability Register) */
    UINT32  ChargePumpCurrent:          8;      /* [23:16]: lcpll_icp_ctrl */
    UINT32  Reserved0:                  8;      /* [31:24]: Reserved */
} B8_PLL_CTRL2_REG_s;

/*-----------------------------------------------------------------------------------------------*\
 * B8 RCT: PLL Control3 Control Register
\*-----------------------------------------------------------------------------------------------*/
typedef struct {
    UINT32  VcoSlow:                    1;      /* [0]: 0 - normal, 1 - 3x gain (Not Used) */
    UINT32  VcoRange:                   2;      /* [2:1]: VCO range */
    UINT32  VcoClamp:                   2;      /* [4:3]: VCO clamp */
    UINT32  DsmDitherEnable:            1;      /* [5]: DSM dither enable */
    UINT32  Reserved0:                  1;      /* [6]: Reserved */
    UINT32  DsmDitherGain:              2;      /* [8:7]: DSM dither gain */
    UINT32  Reserved1:                  4;      /* [12:9]: Reserved */
    UINT32  LpfRz:                      4;      /* [16:13]: adjust the zeros of the loop filter for stability control */
    UINT32  IrefCtrl:                   3;      /* [19:17]: It controls the reference current which impacts charge pump current, thus loop dynamics */
    UINT32  BypassJDiv:                 1;      /* [20]: Bypass JDIV */
    UINT32  BypassJDout:                1;      /* [21]: Bypass JDOUT */
    UINT32  SelI15UB:                   1;      /* [22]: It is used inside PLL */
    UINT32  Reserved2:                  9;      /* [31:23]: Reserved */
} B8_PLL_CTRL3_REG_s;

/*-----------------------------------------------------------------------------------------------*\
 * B8 RCT: PLL Observe Register
\*-----------------------------------------------------------------------------------------------*/
typedef struct {
    UINT32  PrbsOut:                    3;      /* [2:0]: */
    UINT32  DsmB:                       4;      /* [6:3]: */
    UINT32  DsmUnderflow:               1;      /* [7]: */
    UINT32  DsmOverflow:                1;      /* [8]: */
    UINT32  CregObs:                    16;     /* [24:9]: */
    UINT32  RefClkDet:                  1;      /* [25]: */
    UINT32  FbClkDet:                   1;      /* [26]: */
    UINT32  LockInst:                   1;      /* [27]: */
    UINT32  Reserved0:                  2;      /* [29:28]: Reserved */
    UINT32  FreqLock:                   1;      /* [30]: */
    UINT32  Reserved1:                  1;      /* [31]: Reserved */
} B8_PLL_OBSV_REG_s;

/*-----------------------------------------------------------------------------------------------*\
 * B8 RCT: SW PHY Scaler Register
\*-----------------------------------------------------------------------------------------------*/
typedef struct {
    UINT32  SwPhyDivider:               10;     /* [9:0]: SW Phy divider */
    UINT32  Reserved0:                  6;      /* [15:10]: Reserved */
    UINT32  SwPhyClkSelect:             1;      /* [16]: SW Phy clock select */
    UINT32  Reserved1:                  15;     /* [31:17]: Reserved */
} B8_PLL_SWPHY_SCALER_REG_s;

/*-----------------------------------------------------------------------------------------------*\
 * B8 RCT: Post Scaler Register
\*-----------------------------------------------------------------------------------------------*/
typedef struct {
    UINT32  PostDivider:                4;      /* [3:0]: PLL post divider (out_clk = in_clk / (scaler_core_post_div + 1)) */
    UINT32  PostEnable:                 1;      /* [4]: Enable */
    UINT32  Reserved0:                  27;     /* [31:5]: Reserved */
} B8_PLL_CORE_POST_SCALER_REG_s;

/*-----------------------------------------------------------------------------------------------*\
 * B8 RCT: PLL TC Control Register
\*-----------------------------------------------------------------------------------------------*/
typedef struct {
    UINT32  TcCtrl:                     8;      /* [7:0]: */
    UINT32  TcForceCode:                5;      /* [12:8]: */
    UINT32  Reserved0:                  8;      /* [20:13]: Reserved*/
    UINT32  TcSelIp:                    1;      /* [21]: */
    UINT32  Reserved1:                  2;      /* [23:22]: Reserved*/
    UINT32  TcStartForce:               1;      /* [24]: */
    UINT32  TcStartAuto:                1;      /* [25]: */
    UINT32  Reserved2:                  6;      /* [31:26]: Reserved */
} B8_PLL_TC_CTRL_REG_s;

/*-----------------------------------------------------------------------------------------------*\
 * B8 RCT: PLL TC Observe Register
\*-----------------------------------------------------------------------------------------------*/
typedef struct {
    UINT32  Reserved0:                  12;     /* [11:0]: Reserved */
    UINT32  TcRctErr:                   1;      /* [12]: */
    UINT32  TcRctAck:                   1;      /* [13]: */
    UINT32  Reserved1:                  2;      /* [15:14]: */
    UINT32  TcTxTermCtrl:               5;      /* [20:16]: */
    UINT32  TcRxTermCtrl:               5;      /* [25:21]: */
    UINT32  Reserved2:                  2;      /* [27:26]: Reserved */
    UINT32  TcRctCodeMon:               4;      /* [31:28]: */
} B8_PLL_TC_OBSV_REG_s;

/*-----------------------------------------------------------------------------------------------*\
 * B8 RCT: Sensor Post Scaler
\*-----------------------------------------------------------------------------------------------*/
typedef struct {
    UINT32  SoIntPostDivider:           5;      /* [4:0]: PLL post divider for gclk_si */
    UINT32  SoVinPostDivider:           5;      /* [9:5]: PLL post divider for gclk_so_vin  */
    UINT32  Reserved0:                  22;     /* [31:10]: Reserved */
} B8_PLL_SENSOR_POST_SCALER_REG_s;

/*-----------------------------------------------------------------------------------------------*\
 * B8 RCT: Sensor Pre Scaler Register
\*-----------------------------------------------------------------------------------------------*/
typedef struct {
    UINT32  PreEnable:                  1;      /* [0]: Write enable */
    UINT32  PreDivider:                 6;      /* [6:1]: PLL pre divider */
    UINT32  Reserved0:                  1;      /* [7]: Reserved */
    UINT32  SensorRefSel:               3;      /* [10:8] pll_sensor_ref_sel */
    UINT32  Reserved1:                  21;     /* [31:11]: Reserved */
} B8_PLL_SENSOR_PRE_SCALER_REG_s;

/*-----------------------------------------------------------------------------------------------*\
 * B8 RCT: Video Pre Scaler Register
\*-----------------------------------------------------------------------------------------------*/
typedef struct {
    UINT32  PreEnable:                  1;      /* [0]: Write enable */
    UINT32  PreDivider:                 6;      /* [6:1]: PLL pre divider */
    UINT32  Reserved0:                  1;      /* [7]: Reserved */
    UINT32  VoRefSel:                   3;      /* [10:8] pll_vo_ref_sel */
    UINT32  Reserved1:                  21;     /* [31:11]: Reserved */
} B8_PLL_VO_PRE_SCALER_REG_s;


/*-----------------------------------------------------------------------------------------------*\
 * B8 RCT: Video Post Scaler Register
\*-----------------------------------------------------------------------------------------------*/
typedef struct {
    UINT32  PostDivider0:               5;      /* [4:0]: scaler_vo0_post_div*/
    UINT32  PostEnable0:                1;      /* [5]: scaler_vo0_post_en */
    UINT32  Reserved0:                  2;      /* [7:6]: Reserved */
    UINT32  PostDivider1:               5;      /* [12:8]: scaler_vo1_post_div */
    UINT32  PostEnable1:                1;      /* [13]: scaler_vo1_post_en */
    UINT32  Reserved1:                  18;     /* [31:14]: Reserved */
} B8_PLL_VO_POST_SCALER_REG_s;

/*-----------------------------------------------------------------------------------------------*\
 * B8 RCT: System Config Register
\*-----------------------------------------------------------------------------------------------*/
typedef struct {
    UINT32  SysConfig:                  32;     /* [31:0]: */
} B8_SYS_CONFIG_REG_s;

/*-----------------------------------------------------------------------------------------------*\
 * B8 RCT: SSI Clock Divider
\*-----------------------------------------------------------------------------------------------*/
typedef struct {
    UINT32  Div:                        12;     /* [11:0]: */
    UINT32  Reserved0:                  12;     /* [23:12]: Reserved*/
    UINT32  DivReset:                   1;      /* [24]: */
    UINT32  Reserved1:                  7;      /* [31:25]: Reserved */
} B8_CG_SSI_REG_s;

/*-----------------------------------------------------------------------------------------------*\
 * B8 RCT: VOUT Clock Divider
\*-----------------------------------------------------------------------------------------------*/
typedef struct {
    UINT32  Div:                        6;      /* [5:0]: */
    UINT32  Reserved0:                  18;     /* [23:6]: Reserved*/
    UINT32  DivReset:                   1;      /* [24]: */
    UINT32  Reserved1:                  7;      /* [31:25]: Reserved */
} B8_CG_VOUT_REG_s;

/*-----------------------------------------------------------------------------------------------*\
 * B8 RCT: VOUTF Clock Divider
\*-----------------------------------------------------------------------------------------------*/
typedef struct {
    UINT32  Div:                        4;      /* [3:0]: */
    UINT32  Reserved0:                  20;     /* [23:4]: Reserved*/
    UINT32  DivReset:                   1;      /* [24]: */
    UINT32  Reserved1:                  7;      /* [31:25]: Reserved */
} B8_CG_VOUTF_REG_s;

/*-----------------------------------------------------------------------------------------------*\
 * B8 RCT: PWM Clock Divider
\*----------- -----------------------------------------------------------------------------------*/
typedef struct {
    UINT32  Div:                        24;     /* [23:0]: */
    UINT32  DivReset:                   1;      /* [24]: */
    UINT32  Reserved0:                  7;      /* [31:25]: Reserved */
} B8_CG_PWM_REG_s;

/*-----------------------------------------------------------------------------------------------*\
 * B8 RCT: PWM Clock Select Register
\*-----------------------------------------------------------------------------------------------*/
typedef struct {
    UINT32  ClkSrcSelect:               1;      /* [0]: */
    UINT32  Reserved0:                  31;     /* [31:1]: Reserved */
} B8_PWM_CLK_SELECT_REG_s;

/*-----------------------------------------------------------------------------------------------*\
 * B8 RCT: System Reset Register
\*-----------------------------------------------------------------------------------------------*/
typedef struct {
    UINT32  DLL:                        1;      /* [0]: */
    UINT32  PhyTx:                      1;      /* [1]: */
    UINT32  PhyRx:                      1;      /* [2]: */
    UINT32  Dsi0:                       1;      /* [3]: */
    UINT32  Mipi:                       1;      /* [4]: */
    UINT32  Vin0Master:                 1;      /* [5]: */
    UINT32  Vin0Idsp:                   1;      /* [6]: */
    UINT32  Vin0:                       1;      /* [7]: */
    UINT32  Vin1Master:                 1;      /* [8]: */
    UINT32  Vin1Idsp:                   1;      /* [9]: */
    UINT32  Vin1:                       1;      /* [10]: */
    UINT32  Reset:                      1;      /* [11]: */
    UINT32  Reserved0:                  5;      /* [16:12] Reserved */
    UINT32  LvdsPhy:                    1;      /* [17] */
    UINT32  Dsi1:                       1;      /* [18] */
    UINT32  Reserved1:                  13;     /* [31:19] Reserved */
} B8_SYSTEM_RESET_REG_s;

/*-----------------------------------------------------------------------------------------------*\
 * B8 RCT: PLL Lock Status Register
\*-----------------------------------------------------------------------------------------------*/
typedef struct {
    UINT32  Core:                       1;      /* [0]:*/
    UINT32  Reserved0:                  1;      /* [1]: Reserved */
    UINT32  Sensor:                     1;      /* [2]:*/
    UINT32  Video:                      1;      /* [3]:*/
    UINT32  Reserved1:                  1;      /* [4]: */
    UINT32  MPhyRx0:                    1;      /* [5]: */
    UINT32  Reserved2:                  2;      /* [7:6]: Reserved */
    UINT32  MPhyTx0:                    1;      /* [8]: */
    UINT32  Lock2Ms:                    1;      /* [9]: */
    UINT32  Reserved3:                  22;     /* [31:10]: Reserved */
} B8_PLL_LOCK_STATUS_REG_s;

/*-----------------------------------------------------------------------------------------------*\
 * B8 RCT: PLL Control2 Control Register
\*-----------------------------------------------------------------------------------------------*/
typedef struct {
    UINT32  Reserved0:                  8;      /* [7:0]: Reserved */
    UINT32  Div2_8:                     1;      /* [8]: high frequency div2 */
    UINT32  Div2_9:                     1;      /* [9]: high frequency div2 */
    UINT32  Reserved1:                  1;      /* [10]: Reserved */
    UINT32  Div2_11:                    1;      /* [11]: high frequency div2 */
    UINT32  PassPllDivHS:               1;      /* [12]: pass pll_div_HS */
    UINT32  BypassMDiv:                 1;      /* [13]: Bypass mdiv */
    UINT32  VcoType:                    1;      /* [14]: vco_type, 0 : single end VCO 1: differential VCO */
    UINT32  Reserved2:                  1;      /* [15]: Reserved */
    UINT32  ChargePumpCurrent:          8;      /* [23:16]: icp_ctrl */
    UINT32  DsmTypeSel:                 2;      /* [25:24]: dsm_type_sel */
    UINT32  Reserved3:                  6;      /* [31:26]: Reserved */
} B8_PLL_MPHY_CTRL2_REG_s;

/*-----------------------------------------------------------------------------------------------*\
 * B8 RCT: PLL Control3 Control Register
\*-----------------------------------------------------------------------------------------------*/
typedef struct {
    UINT32  VcoRange:                   3;      /* [2:0]: VCO range */
    UINT32  VcoClamp:                   2;      /* [4:3]: VCO clamp */
    UINT32  AcmEn:                      1;      /* [5]: Enable the active current source in vco */
    UINT32  VcoOpIctrl:                 2;      /* [7:6]: Amplifier current control */
    UINT32  ResComp:                    2;      /* [9:8]: Compensation resistor */
    UINT32  VcoResCtrl:                 2;      /* [11:10]: Depends on freq (degenerationresistor) */
    UINT32  Reserved0:                  1;      /* [12]: Reserved */
    UINT32  LpfRz:                      4;      /* [16:13]: adjust the zeros of the loop filter for stability control */
    UINT32  IrefCtrl:                   3;      /* [19:17]: It controls the reference current which impacts charge pump current, thus loop dynamics */
    UINT32  BypassJDiv:                 1;      /* [20]: Bypass JDIV */
    UINT32  BypassJDout:                1;      /* [21]: Bypass JDOUT */
    UINT32  DsmDitherGain:              2;      /* [23:22]: DSM dither gain  */
    UINT32  Reserved1:                  8;      /* [31:24]: Reserved */
} B8_PLL_MPHY_CTRL3_REG_s;

/*-----------------------------------------------------------------------------------------------*\
 * B8 RCT: PLL Software Reset Register
\*-----------------------------------------------------------------------------------------------*/
typedef struct {
    UINT32  RstLEccDec0_3:              4;      /* [3:0]: */
    UINT32  RstLEccEnc0_5:              6;      /* [9:4]: */
    UINT32  RstLDepacker0_3:            4;      /* [13:10]: */
    UINT32  RstLEccDec4_5:              2;      /* [15:14]: */
    UINT32  RstLEccEnc6_7:              2;      /* [17:16]: */
    UINT32  RstLDepacker4_5:            2;      /* [19:18]: */
    UINT32  Reserved0:                  12;     /* [31:20]: */
} B8_AHB_SW_RST_0_REG_s;

typedef struct {
    UINT32  RstLPwmEnc:                 1;      /* [0]: */
    UINT32  RstLVoutf:                  1;      /* [1]: */
    UINT32  RstLSpi:                    1;      /* [2]: */
    UINT32  RstLIdspWrap:               1;      /* [3]: */
    UINT32  RstLIdc:                    2;      /* [5:4]: */
    UINT32  RstLPwmDec:                 1;      /* [6]: */
    UINT32  RstLPacker:                 2;      /* [8:7]: */
    UINT32  RstLLsTx:                   4;      /* [12:9]: Software reset ls_tx channel 0 to 3*/
    UINT32  RstLLsRx:                   2;      /* [14:13]: */
    UINT32  RstLVout0:                  1;      /* [15]: */
    UINT32  RstLLsTxDiv:                1;      /* [16]: */
    UINT32  RstLFsync:                  1;      /* [17]: */
    UINT32  RstLLsTx1:                  2;      /* [19:18]: Software reset ls_tx channel 4 to 5*/
    UINT32  RstLVout1:                  1;      /* [20]: */
    UINT32  Reserved0:                  11;     /* [31:21]: */
} B8_AHB_SW_RST_1_REG_s;

/*-----------------------------------------------------------------------------------------------*\
 * B8 RCT: Clock Gating Control Register
\*-----------------------------------------------------------------------------------------------*/
typedef struct {
    UINT32  DelimiterClkEn:             1;      /* [0]: 1 = Enable Delimiter Clock */
    UINT32  Depacker0ClkEn:             1;      /* [1]: 1 = Enable Depacker0 Clock */
    UINT32  Depacker1ClkEn:             1;      /* [2]: 1 = Enable Depacker1 Clock */
    UINT32  Depacker2ClkEn:             1;      /* [3]: 1 = Enable Depacker2 Clock */
    UINT32  Depacker3ClkEn:             1;      /* [4]: 1 = Enable Depacker3 Clock */
    UINT32  IdcsAhbmClkEn:              1;      /* [5]: 1 = Enable I2C Slave Clock */
    UINT32  PwmEncoderClkEn:            1;      /* [6]: 1 = Enable PWM Encoder Clock */
    UINT32  PwmDecoderClkEn:            1;      /* [7]: 1 = Enable PWM Decoder Clock */
    UINT32  SsiahbmClkEn:               1;      /* [8]: 1 = Enable SPI Slave Clock */
    UINT32  UartahbmClkEn:              1;      /* [9]: 1 = Enable UART Slave Clock */
    UINT32  VicClkEn:                   1;      /* [10]: 1 = Enable VIC Clock */
    UINT32  VoutfClkEn:                 1;      /* [11]: 1 = Enable VOUT Formatter Clock*/
    UINT32  CfgVinClkEn:                1;      /* [12]: 1 = Enable CFG VIN Clock */
    UINT32  CfgPrescaleClkEn:           1;      /* [13]: 1 = Enable CFG Prescale Clock */
    UINT32  CfgDecompClkEn:             1;      /* [14]: 1 = Enable CFG DeCompressor Clock */
    UINT32  CfgCompClkEn:               1;      /* [15]: 1 = Enable CFG Compressor Clock */
    UINT32  IdspDecompClkEn:            8;      /* [23:16]: 1 = Enable IDSP DeCompressor Clock */
    UINT32  IdspCompClkEn:              1;      /* [24]: 1 = Enable IDSP Compressor Clock */
    UINT32  GpioClkEn:                  1;      /* [25]: 1 = Enable GPIO Clock */
    UINT32  SsiAhb2spClkEn:             1;      /* [26]: 1 = Enable SPI Clock */
    UINT32  I2cMuxClkEn:                1;      /* [27]: 1 = Enable I2C Mux Clock */
    UINT32  Idc0ClkEn:                  1;      /* [28]: 1 = Enable I2C0 Clock */
    UINT32  Idc1ClkEn:                  1;      /* [29]: 1 = Enable I2C1 Clock */
    UINT32  IomuxClkEn:                 1;      /* [30]: 1 = Enable I/O Mux Clock */
    UINT32  Reserved0:                  1;      /* [31]: */
} B8_CLK_GATE_REG_s;

typedef struct {
    UINT32  DepackerClkEn:              9;      /* [8:0]: 1 = Enable Depacker Clock */
    UINT32  EccEncClkEn:                9;      /* [17:9]: 1 = Enable ECC Encoder Clock */
    UINT32  EccDecClkEn:                9;      /* [26:18]: 1 = Enable ECC Decoder Clock */
    UINT32  Reserved0:                  5;      /* [31:27]: */
} B8_CLK_GATE1_REG_s;

typedef struct {
    UINT32  LsTxClkEn:                  8;      /* [7:0]: 1 = Enable LS Tx Clock */
    UINT32  LsRxClkEn:                  4;      /* [11:8]: 1 = Enable LS Rx Clock */
    UINT32  IdspVinfClkEn:              8;      /* [19:12]: 1 = Enable VINF Clock */
    UINT32  IdspVinClkEn:               2;      /* [21:20]: 1 = Enable VIN Clock */
    UINT32  IdspPrescalerClkEn:         2;      /* [23:22]: 1 = Enable IDSP Prescaler Clock */
    UINT32  VoutClkEn:                  1;      /* [24]: 1 = Enable Vout Clock */
    UINT32  IdspCodecClkEn:             1;      /* [25]: 1 = Enable IDSP Codec Clock */
    UINT32  IdspMergerClkEn:            1;      /* [26]: 1 = Enable IDSP Merger Clock */
    UINT32  Reserved0:                  5;      /* [31:27]: */
} B8_CLK_GATE2_REG_s;

/*-----------------------------------------------------------------------------------------------*\
 * B8 RCT: Clock Observe Register
\*-----------------------------------------------------------------------------------------------*/
typedef struct {
    UINT32  PllOutMuxSel:               4;      /* [3:0]: */
    UINT32  PllOutMuxSel3:              2;      /* [5:4]: */
    UINT32  PllOutMuxSel2:              2;      /* [7:6]: */
    UINT32  PllOutMuxSel1:              2;      /* [9:8]: */
    UINT32  PllOutMuxSel0:              2;      /* [11:10]: */
    UINT32  PllOutMuxEn:                1;      /* [12]: */
    UINT32  Reserved0:                  19;     /* [31:13]: */
} B8_CLK_OBSERVE_REG_s;

/*-----------------------------------------------------------------------------------------------*\
 * B8 RCT: Sclr Register
\*-----------------------------------------------------------------------------------------------*/
typedef struct {
    UINT32  Reserved0:                  1;      /* [0]: */
    UINT32  Vo:                         1;      /* [1]: */
    UINT32  Reserved1:                  1;      /* [2]: */
    UINT32  Core:                       1;      /* [3]: */
    UINT32  VoPre:                      1;      /* [4]: */
    UINT32  Reserved2:                  27;     /* [31:5]: */
} B8_SCLR_REG_s;

/*-----------------------------------------------------------------------------------------------*\
 * B8 RCT: PWM Force LS Mode Registers
\*-----------------------------------------------------------------------------------------------*/
typedef struct {
    UINT32  PwrForceLsMode:             2;      /* [1:0]: */
    UINT32  Reserved0:                  30;     /* [31:2]: */
} B8_RCT_PWR_FORCE_LS_REG_s;

/*-----------------------------------------------------------------------------------------------*\
 * B8 RCT: EFUSE Registers
\*-----------------------------------------------------------------------------------------------*/
typedef struct {
    UINT32  Enable:                     1;      /* [0]: */
    UINT32  Reserved0:                  31;     /* [31:1]: */
} B8_EFUSE_CTRL_REG_s;

/*-----------------------------------------------------------------------------------------------*\
 * B8 RCT: Interrupt Registers
\*-----------------------------------------------------------------------------------------------*/
typedef struct {
    UINT32  RdDone:                     1;      /* [0]: */
    UINT32  WrDone:                     1;      /* [1]: */
    UINT32  Reserved0:                  30;     /* [31:2]: */
} B8_RCT_INT_STATUS_REG_s;

typedef struct {
    UINT32  RdDoneEn:                   1;      /* [0]: */
    UINT32  WrDoneEn:                   1;      /* [1]: */
    UINT32  Reserved0:                  30;     /* [31:2]: */
} B8_RCT_INT_ENABLE_REG_s;

/*-----------------------------------------------------------------------------------------------*\
 * B8 RCT: I/O Pad Registers
\*-----------------------------------------------------------------------------------------------*/
typedef struct {
    UINT32  Select:                     22;     /* [21:0]: */
    UINT32  Reserved0:                  10;     /* [31:22]: */
} B8_IOPAD_REG_s;

typedef struct {
    UINT32  Reserved0:                  2;      /* [1:0]: */
    UINT32  InputSelect:                1;      /* [2]: 0: CMOS input, 1: Schmitt Trigger input */
    UINT32  SlewRate:                   1;      /* [3]: 0: Fast Slew, 1: Slow Slew */
    UINT32  Reserved1:                  28;     /* [31:4]: */
} B8_IOPAD_MISC_CTRL_REG_s;

/*-----------------------------------------------------------------------------------------------*\
 * B8 RCT : All Registers
\*-----------------------------------------------------------------------------------------------*/
typedef struct {
    volatile B8_PLL_CTRL_REG_s                  CorePllCtrl;            /* 0x000(RW): Core PLL configurations */
    volatile UINT32                             CorePllFraction;        /* 0x004(RW): Core PLL fractional configurations */
    volatile B8_PLL_CTRL2_REG_s                 CorePllCtrl2;           /* 0x008(RW): Core PLL configurations */
    volatile B8_PLL_CTRL3_REG_s                 CorePllCtrl3;           /* 0x00C(RW): Core PLL configurations */
    volatile B8_PLL_OBSV_REG_s                  CorePllObsv;            /* 0x010(RO): Core PLL Observation*/
    volatile B8_PLL_SWPHY_SCALER_REG_s          CorePllSwPhyScaler;     /* 0x014(RW): */
    volatile B8_PLL_CORE_POST_SCALER_REG_s      CorePllPostScaler;      /* 0x018(RW): */
    volatile UINT32                             Reserved0[5];           /* 0x01C-0x02C: Reserved */
    volatile B8_PLL_TC_CTRL_REG_s               TcPllCtrl;              /* 0x030(RW): TC Control */
    volatile B8_PLL_TC_OBSV_REG_s               TcPllObserve;           /* 0x034(RO): TC Observation */
    volatile UINT32                             Reserved1[2];           /* 0x038-0x03C: Reserved */
    volatile B8_PLL_CTRL_REG_s                  SensorPllCtrl;          /* 0x040(RW): Sensor PLL configurations */
    volatile UINT32                             SensorPllFraction;      /* 0x044(RW): Sensor PLL fractional configurations */
    volatile B8_PLL_CTRL2_REG_s                 SensorPllCtrl2;         /* 0x048(RW): Sensor PLL configurations */
    volatile B8_PLL_CTRL3_REG_s                 SensorPllCtrl3;         /* 0x04C(RW): Sensor PLL configurations */
    volatile B8_PLL_OBSV_REG_s                  SensorPllObsv;          /* 0x050(RO): Sensor PLL Observation */
    volatile UINT32                             RefClkDivider;          /* 0x054(RW): Not used */
    volatile B8_PLL_SENSOR_POST_SCALER_REG_s    SensorPllPostScaler;    /* 0x058(RW): */
    volatile B8_PLL_SENSOR_PRE_SCALER_REG_s     SensorPllPreScaler;     /* 0x05C: Reserved */
    volatile B8_PLL_CTRL_REG_s                  VideoPllCtrl;           /* 0x060(RW): Vout PLL configurations */
    volatile UINT32                             VideoPllFraction;       /* 0x064(RW): Vout PLL fractional configurations */
    volatile B8_PLL_CTRL2_REG_s                 VideoPllCtrl2;          /* 0x068(RW): Vout PLL configurations */
    volatile B8_PLL_CTRL3_REG_s                 VideoPllCtrl3;          /* 0x06C(RW): Vout PLL configurations */
    volatile B8_PLL_OBSV_REG_s                  VideoPllObsv;           /* 0x070(RO): Vout PLL Observation*/
    volatile B8_PLL_VO_PRE_SCALER_REG_s         VideoPllPreScaler;      /* 0x074(RW): Vout PLL PreScaler */
    volatile B8_PLL_VO_POST_SCALER_REG_s        VideoPllPostScaler;     /* 0x078(RW): Vout PLL PostScaler */
    volatile B8_SYS_CONFIG_REG_s                SysConfig;              /* 0x07C(RW): System configurations */
    volatile UINT32                             Reserved3;              /* 0x080: Reserved */
    volatile B8_CG_SSI_REG_s                    CgSsi;                  /* 0x084(RW): Clock divider for SPI */
    volatile UINT32                             Reserved4[2];           /* 0x088-0x08C: Reserved */
    volatile B8_CG_VOUT_REG_s                   CgVout1;                /* 0x090(RW): */
    volatile B8_CG_VOUTF_REG_s                  CgVoutf;                /* 0x094(RW): */
    volatile B8_CG_VOUT_REG_s                   CgVout0;                /* 0x098(RW): */
    volatile B8_CG_PWM_REG_s                    CgPwm;                  /* 0x09C(RW): */
    volatile B8_PWM_CLK_SELECT_REG_s            PwmClkSel;              /* 0x0A0(RW): */
    volatile UINT32                             Reserved5[2];           /* 0x0A4-0x0A8: Reserved */
    volatile B8_SYSTEM_RESET_REG_s              SysReset;               /* 0x0AC(RW): System Reset */
    volatile B8_PLL_LOCK_STATUS_REG_s           PllLockStatus;          /* 0x0B0(RO): */
    volatile UINT32                             Reserved6[3];           /* 0x0B4-0x0BC: Reserved */
    volatile B8_PLL_CTRL_REG_s                  MPhyTx0PllCtrl;         /* 0x0C0(RW): MPhy Tx PLL configurations */
    volatile UINT32                             MPhyTx0PllFraction;     /* 0x0C4(RW): MPhy Tx PLL fractional configurations */
    volatile B8_PLL_MPHY_CTRL2_REG_s            MPhyTx0PllCtrl2;        /* 0x0C8(RW): MPhy Tx PLL configurations */
    volatile B8_PLL_MPHY_CTRL3_REG_s            MPhyTx0PllCtrl3;        /* 0x0CC(RW): MPhy Tx PLL configurations */
    volatile B8_PLL_OBSV_REG_s                  MPhyTx0PllObsv;         /* 0x0D0(RO): MPhy Tx PLL Observation*/
    volatile UINT32                             Reserved7[9];           /* 0x0D4-0x0F4: Reserved */
    volatile B8_AHB_SW_RST_0_REG_s              AhbSwReset0;            /* 0x0F8(RW): */
    volatile B8_AHB_SW_RST_1_REG_s              AhbSwReset1;            /* 0x0FC(RW): */
    volatile UINT32                             ChipInfo;               /* 0x100(RW): */
    volatile UINT32                             Reserved8[12];          /* 0x104-0x130: Reserved */
    volatile B8_CLK_GATE_REG_s                  ClkGate;                /* 0x134(RW): Clock gate Configurations */
    volatile B8_CLK_GATE1_REG_s                 ClkGate1;               /* 0x138(RW): Clock gate Configurations */
    volatile B8_CLK_GATE2_REG_s                 ClkGate2;               /* 0x13C(RW): Clock gate Configurations */
    volatile UINT32                             Reserved9[4];           /* 0x140-0x14C: Reserved */
    volatile B8_RCT_PWR_FORCE_LS_REG_s          PwrForceLsMode;         /* 0x150(RW): PWM Configurations */
    volatile B8_CLK_OBSERVE_REG_s               ClkObserve;             /* 0x154(RW): Clock Observation */
    volatile UINT32                             Reserved10;             /* 0x158: Reserved */
    volatile B8_SCLR_REG_s                      SclrBypass;             /* 0x15C(RW): */
    volatile B8_SCLR_REG_s                      SclrSoftReset;          /* 0x160(RW): */
    volatile B8_SCLR_REG_s                      SclrDisableClk;         /* 0x164(RW): */
    volatile UINT32                             EfuseFsourceDelay;      /* 0x168(RW): */
    volatile UINT32                             EfuseReadData2;         /* 0x16C(RO): Read eFuse Configuration bit 64 to 95 */
    volatile UINT32                             EfuseReadData1;         /* 0x170(RO): Read eFuse Configuration bit 32 to 63 */
    volatile UINT32                             EfuseReadData0;         /* 0x174(RO): Read eFuse Configuration bit 0 to 31 */
    volatile UINT32                             EfuseChipIdHigh;        /* 0x178(RO): Read eFuse ChipID bit 32 to 45 */
    volatile UINT32                             EfuseChipIdLow;         /* 0x17C(RO): Read eFuse ChipID bit 0 to 31 */
    volatile B8_RCT_INT_STATUS_REG_s            RctIntStatus;           /* 0x180(RW): Interrupt Status Configurations */
    volatile B8_RCT_INT_ENABLE_REG_s            RctIntEnable;           /* 0x184(RW): Interrupt Enable Configurations */
    volatile UINT32                             EfusePvalId1;           /* 0x188(RW): */
    volatile UINT32                             EfusePvalId0;           /* 0x18C(RW): */
    volatile UINT32                             Reserved11;             /* 0x190: Reserved */
    volatile UINT32                             EfuseWriteData2;        /* 0x194(RW): Content write to eFuse Configuration bit 64 to 95 */
    volatile UINT32                             EfuseWriteData1;        /* 0x198(RW): Content write to eFuse Configuration bit 32 to 63 */
    volatile UINT32                             EfuseWriteData0;        /* 0x19C(RW): Content write to eFuse Configuration bit 0 to 31 */
    volatile UINT32                             Reserved12[4];          /* 0x1A0-1AC: Reserved */
    volatile B8_EFUSE_CTRL_REG_s                EfuseWriteCtrl;         /* 0x1B0(RW): Efuse Write Control */
    volatile B8_EFUSE_CTRL_REG_s                EfuseReadCtrl;          /* 0x1B4(RW): Efuse Read Control */
    volatile UINT32                             Reserved13[2];          /* 0x1B8-0x1BC: Reserved */
    volatile B8_IOPAD_REG_s                     IoPadPullEn;            /* 0x1C0(RW): Pull enable, 0: no pull */
    volatile B8_IOPAD_REG_s                     IoPadPullSel;           /* 0x1C4(RW): Pull select, 0: pull low, 1:pull high */
    volatile UINT32                             Reserved14[2];          /* 0x1C8-0X1CC: Reserved */
    volatile B8_IOPAD_REG_s                     IoPadDriveStrength0;    /* 0x1D0(RW): Driving strength bit0. 0: 2mA, 1: 4mA */
    volatile B8_IOPAD_REG_s                     IoPadDriveStrength1;    /* 0x1D4(RW): Driving strength bit1. 2: 8mA, 3: 12mA */
    volatile UINT32                             Reserved15[2];          /* 0x1D8-0x1DC: Reserved */
    volatile B8_IOPAD_MISC_CTRL_REG_s           IoPadMiscCtrl;          /* 0x1E0(RW): */
    volatile UINT32                             Reserved16[19];         /* 0x1E4-0x22C: Reserved */
    volatile B8_PLL_CTRL_REG_s                  MPhyRxPllCtrl;          /* 0x230(RW): MPhy Rx PLL configurations */
    volatile UINT32                             MPhyRxPllFraction;      /* 0x234(RW): MPhy Rx PLL fractional configurations */
    volatile B8_PLL_CTRL2_REG_s                 MPhyRxPllCtrl2;         /* 0x238(RW): MPhy Rx PLL configurations */
    volatile B8_PLL_CTRL3_REG_s                 MPhyRxPllCtrl3;         /* 0x23C(RW): MPhy Rx PLL configurations */
    volatile B8_PLL_OBSV_REG_s                  MPhyRxPllObsv;          /* 0x240(RO): MPhy Rx PLL Observation*/
    volatile UINT32                             Reserved17[351];        /* 0x244-0x7BC: Reserved */
    volatile UINT32                             ExtraMarginAdj1;        /* 0x7C0(RW): */
    volatile UINT32                             ExtraMarginAdj2;        /* 0x7C4(RW): */
} B8_RCT_REG_s;

#endif /* AMBA_B8_REG_RCT_H */
