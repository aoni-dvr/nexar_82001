/**
 *  @file AmbaReg_DDRC.h
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
 *  @details Definitions & Constants for DDR Controller APIs.
 *
 */

#ifndef AMBA_REG_DDRC_H
#define AMBA_REG_DDRC_H

#ifndef AMBA_TYPES_H
#include "AmbaTypes.h"
#endif

/*
 * DDRC: DRAM Controller Register
 */
typedef struct {
    UINT32  Enable:             1;  /* [0]: DRAM access enable: 0 - disable, 1 - enable */
    UINT32  AutoRefreshEnable:  1;  /* [1]: DRAM auto refresh enable: 0 - disable, 1 - enable */
    UINT32  CkeOutputEnable:    1;  /* [2]: DRAM CKE output: 0 - disable, 1 - enable */
    UINT32  Reset:              1;  /* [3]: DRAM RESET# output: active low (0 = Trigger Reset), this value will be determined by pwc_ddrhost_sr_en */
    UINT32  IddqTestEnable:     1;  /* [4]: Enable Iddq test: 0 - disable, 1 - enable */
    UINT32  DeepPowerDown:      1;  /* [5]: Deep power-down enable; For LPDDR2, set this bit and reset DRAM_ENABLE will activate deep power down mode */
    UINT32  ChipSelect:         1;  /* [6]: Dual-die software chip select */
    UINT32  HighSkewEnable:     1;  /* [7]: Enable high skew tolerance for LPDDR2/3 */
    UINT32  HighSkewCounter:    2;  /* [9:8]: RTT optimization counter */
    UINT32  SelfRefreshCounter: 4;  /* [13:10]: Self-refresh counter to insert delay between SR down and up events, actual value is 2x */
    UINT32  Ddr3Odt:            1;  /* [14]: Fixes ODT for DDR3 */
    UINT32  MDdr3OdtOn:         3;  /* [17:15]: ODT on time in cycles (RoundUp(T_ODT_ON/TCK)) */
    UINT32  MDdr3OdtOff:        3;  /* [20:18]: ODT off time in cycles (RoundUp(T_ODT_OFF/TCK)) */
    UINT32  MDdr3PullUp:        1;  /* [21]: MDDR3 Pull up enable (affects MDDR3 hskew operation in DDRC) */
    UINT32  ReadtoWriteDelay:   6;  /* [27:22]: LPDDR4 Read-To-Write delay adjustment */
    UINT32  Reserved1:          1;  /* [28]: Reserved */
    UINT32  ChipSelectCtrl:     2;  /* [30:29]: 2'bx0 = turn off access to die 0, 2'b0x = turn off access to die 1 */
    UINT32  TrainChannelSelect: 1;  /* [31]: 0 = train channel-A, 1 = train channel-B */
} AMBA_DDRC_CTRL_REG_s;

/*
 * DDRC: DRAM Controller Config Register
 */
typedef struct {
    UINT32  Reserved0:          2;  /* [1:0]: Reserved */
    UINT32  PageSize:           2;  /* [3:2]: 0 = 1KB, 1 = 2KB, 2 = 4KB, 3 = Reserved */
    UINT32  BankSize:           1;  /* [4]: 0 = 4 banks, 1 = 8 banks */
    UINT32  DramSize:           3;  /* [7:5]: 0 = 256 Mb, 1 = 512 Mb, 2 = 1 Gb, 3 = 2 Gb, 4 = 4 Gb, 5 = 8 Gb, 6 = 16 Gb, Others = Reserved */
    UINT32  DqBusWidth:         1;  /* [8]: DRAM DQ Bus Width: 0 = 32-bit, 1 = 16-bit */
    UINT32  BurstMode:          1;  /* [9]: DDR burst mode: 0 = Burst of 8, 1 = Burst of 4 */
    UINT32  OdtConfig:          3;  /* [12:10]: DRAM controller ODT configuration: 0 = Disable, 1 = RZQ/4, 2 = RZQ/2, 3 = RZQ/6, 4 = RZQ/12, 5 = RZQ/8, Others = Reserved */
    UINT32  DqsInputEnable:     1;  /* [13]: 1 = Enable DQS input enable */
    UINT32  AccuMaskMode:       1;  /* [14]: 1:accumulated Mask mode(LPDDR4 only) */
    UINT32  SerialMode:         1;  /* [15]: 1 = DRAM command serializing mode */
    UINT32  MaxPostedRefCredit: 4;  /* [19:16]: Max number of posted auto-refreshes */
    UINT32  MinPostedRefCredit: 4;  /* [23:20]: Min number of posted auto-refreshes */
    UINT32  Reserved1:          1;  /* [24]: Reserved */
    UINT32  SwPadZqCtrl:        1;  /* [25]: 1 = the pad zq calibration is contorlled by software, 0 = The calibration is controlled by HW state machine that is triggered */
    UINT32  DuelDieEn:          1;  /* [26]: 0 = single die, 1 = dual die enable */
    UINT32  DramType:           3;  /* [29:27]: 6 = LPDDR4, Others = Reserved */
    UINT32  SelfRefreshClkOff:  1;  /* [30]: Controls the clock shut-off during self-refresh */
    UINT32  ReadenSel:          1;  /* [31]: 1: long readen */
} AMBA_DDRC_CONFIG_REG_s;

/*
 * DDRC: DRAM Controller Timing1 Register
 */
typedef struct {
    UINT32  TimeRRD:            5;  /* [4:0]: Row Activate to Row Activate Delay Time: ceil(tRRD/tCK) */
    UINT32  TimeRCD:            6;  /* [10:5]: Row Address to Column Address Delay Time: ceil(tRCD/tCK) */
    UINT32  TimeRP:             6;  /* [16:11]: Row Precharge Time: ceil(tRP/tCK) */
    UINT32  TimeRC:             8;  /* [24:17]: Row Cycle Time: ceil(tRC/tCK) */
    UINT32  TimeRAS:            7;  /* [31:25]: Active to Precharge Delay: ceil(tRAS/tCK) */
} AMBA_DDRC_TIMING1_REG_s;

/*
 * DDRC: DRAM Controller Timing2 Register
 */
typedef struct {
    UINT32  TimeRTP:            4;  /* [3:0]: Read to Precharge Delay Time: ceil(tRTP/tCK) - 2 */
    UINT32  TimeWR:             6;  /* [9:4]: Write Recovery Time: ceil(tWR/tCK) - 2 */
    UINT32  TimeRFC:            9;  /* [18:10]: Refresh Cycle Time: ceil(tRFC/tCK) - 2 */
    UINT32  TimeCL:             6;  /* [24:19]: CAS Read Latency: n means CL=n */
    UINT32  TimeWL:             6;  /* [30:25]: CAS Write Latency - ((LPDDR4) ? 0: 3) */
    UINT32  Reserved:           1;  /* [31]: Reserved */
} AMBA_DDRC_TIMING2_REG_s;

/*
 * DDRC: DRAM Timing3 Register
 */
typedef struct {
    UINT32  TimeREFI:           12; /* [11:0]: Average Periodic Refresh Interval: (tREFI/tCK) >> 4 */
    UINT32  TimeDPD:            17; /* [28:12]: Minimum deep power-down time, multiplied by 4 */
    UINT32  TimeDQS:            1;  /* [29]: DQS adjustment for LPDDR2/3, Adding 1 cycle to tR2W */
    UINT32  Reserved:           2;  /* [31:30]: Reserved */
} AMBA_DDRC_TIMING3_REG_s;

/*
 * DDRC: DRAM Timing4 Register
 */
typedef struct {
    UINT32  TimeFAW:            7;  /* [6:0]: Four Activate Window: ceil(tFAW/tCK) - 2 */
    UINT32  TimeXSR:            10; /* [16:7]: Exit Self Refresh to First Valid Command Time: ceil(tXSR/tCK) - 2 */
    UINT32  TimeWTR:            5;  /* [21:17]: Write to Read Delay: ceil(tWTR/tCK) - 2 */
    UINT32  PowerUpCount:       5;  /* [26:22]: Number of cycles during power up */
    UINT32  TimeOSCO:           2;  /* [28:27]: 0 = 60 cycles, 1 = 80 cycles, 2 = 100 cycles, 3 = 120 cycles */
    UINT32  HighSpeedMode:      1;  /* [29]: 0: upto 1.4GHz, 1: 1.4Ghz and beyond, Affect to dq_bus_idle to MRW and tC2C */
    UINT32  Reserved:           2;  /* [31:30]: Reserved */
} AMBA_DDRC_TIMING4_REG_s;

/*
 * DDRC: DRAM Controller Init Control Register
 */
typedef struct {
    UINT32  PreAllEnBusy:       1;  /* [0]: DRAM Precharge-All command enable(W)/busy(R) */
    UINT32  ImmRefEnBusy:       1;  /* [1]: Immediate refresh command enable(W)/busy(R) */
    UINT32  GetRttEnBusy:       1;  /* [2]: Get round-trip-time enable(W)/busy(R) */
    UINT32  DllRstEnBusy:       1;  /* [3]: DLL reset enable(W)/busy(R) */
    UINT32  ZqClbEnBusy:        1;  /* [4]: DRAM ZQ calibration command enable(W)/busy(R) */
    UINT32  PadClbEnBusy:       1;  /* [5]: Pads ZQ calibration command enable(W)/busy(R) */
    UINT32  PadClbShortEnBusy:  1;  /* [6]: Pads ZQ calibration command enable(W)/busy(R) (for Short Calibration) */
    UINT32  Reserved:           25; /* [31:7]: Reserved */
} AMBA_DDRC_INIT_CTL_REG_s;

/*
 * DDRC: DRAM Controller Mode Register
 */
typedef struct {
    UINT32  Data:               16; /* [15:0](RW): Value of MR# */
    UINT32  Addr:               8;  /* [23:16](RW): MR# indicator */
    UINT32  Write:              1;  /* [24](RW): 0 = MRR, 1 = MRW */
    UINT32  ChipSelect:         1;  /* [25](RW): Select 0 = die 0, 1 - die 1  */
    UINT32  ChanSelect:         1;  /* [26](RW): Select 0 = channel A, 1 = channel B (for LPDDR4 only) */
    UINT32  Reserved:           4;  /* [30:27]: Reserved */
    UINT32  Busy:               1;  /* [31](RO): 1 = busy on sending MRR/MRW command */
} AMBA_DDRC_MODE_REG_s;

/*
 * DDRC: DRAM Controller Self Refresh Register
 */
typedef struct {
    UINT32  AutoActiveDelay:    10; /* [9:0](RW): Delay = Multiple of 64 clock cycles */
    UINT32  AutoCalibThreshold: 6;  /* [15:10](RW): Threshold = 2**n number of clock cycles */
    UINT32  Reserved:           11; /* [26:16](RW): Reserved */
    UINT32  PwcSrEn:            1;  /* [27](RO): PWC self-refresh Enable status, used to check boot up from suspend-to-ram mode */
    UINT32  ActiveStatus:       1;  /* [28](RO): Self-refresh active status */
    UINT32  AutoCalibEnable:    1;  /* [29](RW): Self-refresh auto rtt calibration enable after wakeup */
    UINT32  AutoActiveEnable:   1;  /* [30](RW): Self-refresh auto-active enable */
    UINT32  ForceActiveEnable:  1;  /* [31](RW): Self-refresh forced-active enabled by SW */
} AMBA_DDRC_SELF_REFRESH_REG_s;

/*
 * DDRC: DRAM Controller DQS Sync Register
 */
typedef struct {
    UINT32  SwDqsSyncEnable:    1;  /* [0](RW): SW Controlled DQS synchronization enable */
    UINT32  SwDqsSyncCtl0:      4;  /* [4:1](RW): Value of the SW Controlled DQS_SYNC_CTL to ddrio (die0) */
    UINT32  SwDqsSyncCtl1:      4;  /* [8:5](RW): Value of the SW Controlled DQS_SYNC_CTL to ddrio (die1) */
    UINT32  AutoDqsSyncCfg:     3;  /* [11:9](RW): Synchronization of DQS to CLK process is done every 2^n number of refreshes */
    UINT32  SyncTrackingSel:    1;  /* [12](RW): 0 = Legacy Sync mode selected, 1 = Sync tracking mode selected. Mutually exclusive from HIGHSKEW_EN */
    UINT32  LongRttWin:         1;  /* [13](RW): 0 = Regular RTT window mode, 1 = Long RTT window mode. Affect to Sync/Sync track/Hiskew only for LPDDR4 */
    UINT32  SyncTrackWait:      4;  /* [17:14](RW): Minimum Sync_tracking_wait cycles, cycles = (sync_tracking_wait[13:10]+1)*8 cycles */
    UINT32  RoundTrip:          6;  /* [23:18](RO): Round-trip-time in unit of clock cycles */
    UINT32  HighSkewOffset:     4;  /* [27:24](RW): HIGHSKEW_FIFO's offset for read pointer When >3 , recommend to set ReadenSel =1*/
    UINT32  Reserved:           4;  /* [31:28](RO): Reserved */
} AMBA_DDRC_DQS_SYNC_REG_s;

/*
 * DDRC: DRAM Controller Pads Termination Register
 */
typedef struct {
    UINT32  PadsNoTerm:         1;  /* [0]: No termination select for pads: 1 - no termination */
    UINT32  PadsGddr3:          1;  /* [1]: Gddr3 select for pads */
    UINT32  PadsDDS:            3;  /* [4:2]: Driving Strength of pads */
    UINT32  PadsTerm:           3;  /* [7:5]: Termination Value of pads */
    UINT32  PadsForceImpn:      3;  /* [10:8]: The forced termination impedance code when PADS_FORCE_IMP is set */
    UINT32  PadsForceImpp:      3;  /* [13:11]: The forced termination impedance code when PADS_FORCE_IMP is set */
    UINT32  PadsZctlForce:      1;  /* [14]: When this bit is set, the ZCTRL_FORCE pin to DDRIO will be asserted */
    UINT32  PadsZctlReqShort:   1;  /* [15]: When this bit is set, the ZCTRL_REQ_SHORT pin to DDRIO will be asserted */
    UINT32  PadsZctlReqLong:    1;  /* [16]: When this bit is set, the ZCTRL_REQ_LONG pin to DDRIO will be asserted */
    UINT32  PadsZctlReset:      1;  /* [17]: When this bit is set, the ZCTRL_RESET pin to DDRIO will be asserted */
    UINT32  PadsZctlEnd:        1;  /* [18]: Monitored value of END in impedance calibration */
    UINT32  PadsPrevImppMon:    3;  /* [21:19]: Monitored value of IMPP code in impedance calibration at previous cycle */
    UINT32  PadsPrevImpnMon:    3;  /* [24:22]: Monitored value of IMPN code in impedance calibration at previous cycle */
    UINT32  PadsCurrImppMon:    3;  /* [27:25]: Monitored value of IMPP code in impedance calibration at current cycle */
    UINT32  PadsCurrImpnMon:    3;  /* [30:28]: Monitored value of IMPN code in impedance calibration at current cycle */
    UINT32  PadsZctlError:      1;  /* [31]: Monitored value of error in impedance calibration */
} AMBA_DDRC_PAD_TERM_REG_s;

/*
 * DDRC:  DRAM Controller ZQ Calibration Register
 */
typedef struct {
    UINT32  PadZqCalibEnable:   1;  /* [0]: This bit is set to enable ZQ calibration to DDRIO pads */
    UINT32  DdrZqCalibEnable:   1;  /* [1]: This bit is set to enable ZQ calibration to DDR memory */
    UINT32  DdrZqCsSel:         1;  /* [2]: When doing ZQ calibration to DDR memory, 0 - issue ZQCL command, 1 - issue ZQCS command */
    UINT32  ZqCalibRCfg:        5;  /* [7:3]: The ZQ calibration process is initiated every 2**n number of refreshes */
    UINT32  ZqcTimeSel:         2;  /* [9:8]: ZQC(L/S) Command Time Select: 0 = 512 tCK, 1 = 256 tCK, 2 = 64 tCK */
    UINT32  ZqcTimeSelPOR:      3;  /* [12:10]: Power up calibration time by cycle. 0 = 1100, 1 = 1200, 2 = 1400, 3 = 1600 cycles */
    UINT32  Reserved:           19; /* [31:13]: Reserved */
} AMBA_DDRC_ZQ_CALIB_REG_s;

/*
 * DDRC: DRAM Controller Byte Mapping Register
 */
typedef struct {
    UINT32  Byte0:              2;  /* [1:0]: 0 - DDRIO byte 0 is mapped to DDRC byte 0 and so on */
    UINT32  Byte1:              2;  /* [3:2]: 0 - DDRIO byte 1 is mapped to DDRC byte 0 and so on */
    UINT32  Byte2:              2;  /* [5:4]: 0 - DDRIO byte 2 is mapped to DDRC byte 0 and so on */
    UINT32  Byte3:              2;  /* [7:6]: 0 - DDRIO byte 3 is mapped to DDRC byte 0 and so on */
    UINT32  Reserved:           24; /* [31:8]: Reserved */
} AMBA_DDRC_BYTE_MAP_REG_s;

/*
 * DDRC: DRAM Controller Power Down Register
 */
typedef struct {
    UINT32  PowerDownEnable:    1;  /* [0]: Power-down mode enable */
    UINT32  Reserved:           3;  /* [3:1]: Reserved */
    UINT32  PowerDownDelay:     28; /* [31:4]: PowerDownDelay = multiple of 4 clock cycles */
} AMBA_DDRC_POWER_DOWN_REG_s;

/*
 * DDRC: DRAM Controller DLL Calibration Register
 */
typedef struct {
    UINT32  DllCalibEnable:     1;  /* [0]: Enable DLL calibration to DDRIO pads */
    UINT32  DllCalibRCfg:       5;  /* [5:1]: The DLL Calibration process is initiated every 2**n number of refreshes */
    UINT32  DllResetCount:      8;  /* [13:6]: Reset period for DLL calibration in DDRIO */
    UINT32  DllPostCount:       12; /* [25:14]: Wait period (post amble after reset) for DLL calibration in DDRIO */
    UINT32  Reserved:           6;  /* [31:26]: Reserved */
} AMBA_DDRC_DLL_CALIB_REG_s;

/*
 * DDRC: DRAM Controller Timing5 for LPDDR4 Training Register
 */
typedef struct {
    UINT32  TimeWtrl:           4;  /* [3:0]: DDR4 Read to write latency long for same bank groups */
    UINT32  TimeCcdl:           4;  /* [7:4]: DDR4 Cas command to Cas command latency same bank groups */
    UINT32  TimeCcds:           3;  /* [10:8]: DDR4 Cas command to Cas command latency different bank groups */
    UINT32  TimeRrds:           3;  /* [13:11]: DDR4 T_RRDS for different bank groups */
    UINT32  Ddr4SpitterDis:     1;  /* [14]: 1: DDR4 request splitter disabled and internal state machines are reset */
    UINT32  TimePowerDown:      5;  /* [19:15]: Number of cycles in the power-up sequence during power-down, self refresh, and so on. */
    UINT32  DlyOffset:          4;  /* [23:20]: Programmable register to capture the correct RTT */
    UINT32  TrainMode:          1;  /* [24]: Puts pipeline in training mode */
    UINT32  TrainModeState:     1;  /* [25]: Poll to check if the training mode is set */
    UINT32  TimeDateHoldTrain:  3;  /* [28:26]: Hold DQ bus after DSS pulse. ceil(tDHtrain/tCK) - 1 */
    UINT32  TimeDateSetupTrain: 3;  /* [31:29]: Setup DQ bus before DQS pulse. ceil(t DStrain/tCK) - 1 */
} AMBA_DDRC_TIMING5_REG_s;

/*
 * DDRC: DRAM Controller Pads Termination2 Register
 */
typedef struct {
    UINT32  AddrCtrlDds:        3;  /* [2:0]: Driving Strength of pads */
    UINT32  AddrCtrlModeNoTerm: 1;  /* [3]: No termination select for pads: 1 - no termination */
    UINT32  AddrCtrlModeRGDDR3: 1;  /* [4]: Gddr3 select for pads. Reserved */
    UINT32  AddrCtrlModeTerm:   3;  /* [7:5]: Termination Value of pads */
    UINT32  CtrlModePdds:       3;  /* [10:8]   */
    UINT32  CtrlModeLp4:        1;  /* [11]     */
    UINT32  AddrModePdds:       3;  /* [14:12]  */
    UINT32  AddrCtrlModeLp4:    1;  /* [15]     */
    UINT32  CaCtrlDds:          3;  /* [18:16]  */
    UINT32  CaCtrlModeNoterm:   1;  /* [19]     */
    UINT32  CaCtrlModeRGDDR3:   1;  /* [20]     */
    UINT32  CaCtrlModeTerm:     3;  /* [23:21]  */
    UINT32  CaModePdds:         3;  /* [26:24]  */
    UINT32  CaCtrlModeLp4:      1;  /* [27]     */
    UINT32  Reserved:           4;  /* [31:28]: Reserved */
} AMBA_DDRC_PAD_TERM2_REG_s;

/*
 * DDRC: DRAM Controller Pads Termination3 Register
 */
typedef struct {
    UINT32  VrefSel:            1;  /* [0]: Driving Strength of pads */
    UINT32  VrefSel0:           1;  /* [1]: No termination select for pads: 1 - no termination */
    UINT32  VrefSel1:           1;  /* [2]: Gddr3 select for pads. Reserved */
    UINT32  VrefSel2:           1;  /* [3]: Termination Value of pads */
    UINT32  VrefSel3:           1;  /* [4]: Termination Value of pads */
    UINT32  VrefSel4:           1;  /* [5]               */
    UINT32  Reserved0:          6;  /* [11:6]: Reserved  */
    UINT32  DqCtrlDds:          3;  /* [14:12]           */
    UINT32  DqCtrlModeNoTerm:   1;  /* [15]              */
    UINT32  DqCtrlModeRGDDR3:   1;  /* [16]              */
    UINT32  DqCtrlModeTerm:     3;  /* [19:17]           */
    UINT32  DqModePdds:         3;  /* [22:20]           */
    UINT32  DqCtrlModeLp4:      1;  /* [23]              */
    UINT32  IbiasCtrlEn:        1;  /* [24]              */
    UINT32  IbiasCtrl:          2;  /* [26:25]           */
    UINT32  Reserved1:          5;  /* [31:27]: Reserved */
} AMBA_DDRC_PAD_TERM3_REG_s;

/*
 * DDRC: DRAM Controller LPDDR4 U-instruction Register
 */
typedef struct {
    UINT32  ImmediateData:      7;  /* [6:0]: Immediate date, Exception : MRW : bits[7:0] : data, bits[13:8] : address */
    UINT32  Reserved:           21; /* [27:7]: Reserved */
    UINT32  Opcode:             4;  /* [31:28]: Opcode */
} AMBA_DDRC_MDDR4_U_INST_REG_s;

/*
 * DDRC: DRAM Controller LPDDR4 Training Ring Oscillator Count Register
 */
typedef struct {
    UINT32  StartCount:         16;  /* [15:0]: Start OSC count */
    UINT32  EndCount:           16;  /* [31:16]: End OSC count */
} AMBA_DDRC_MDDR4_TRN_OSC_CNT_REG_s;

/*
 * DDRC: DRAM Controller LPDDR4 Training Command Done Register
 */
typedef struct {
    UINT32  CmdDone:            1;  /* [0]: Command done */
    UINT32  Reserved:           31; /* [31:1]: Reserved */
} AMBA_DDRC_MDDR4_TRN_CMD_DONE_REG_s;

/*
 * DDRC: DRAM Controller LPDDR4 Training DQ Capture Register
 */
typedef struct {
    UINT32  DqCapture:          7;  /* [6:0]: Raw DQ capture */
    UINT32  Reserved:           25; /* [31:7]: Reserved */
} AMBA_DDRC_MDDR4_TRN_DQ_CPT_REG_s;

/*
 * DDRC: DRAM Controller LPDDR4 Training DQ Write Delay Register
 */
typedef struct {
    UINT32  Ctrl0:              7;  /* [6:0]: ddrc_ddrio_dq_wrdly_en_0 = (ddrc_ddrio_dq_wrdly_ctrl_0>0) */
    UINT32  Ctrl1:              7;  /* [13:7]: ddrc_ddrio_dq_wrdly_en_1 = (ddrc_ddrio_dq_wrdly_ctrl_1>0) */
    UINT32  Ctrl2:              7;  /* [20:14]: ddrc_ddrio_dq_wrdly_en_2 = (ddrc_ddrio_dq_wrdly_ctrl_2>0) */
    UINT32  Ctrl3:              7;  /* [27:21]: ddrc_ddrio_dq_wrdly_en_3 = (ddrc_ddrio_dq_wrdly_ctrl_3>0) */
    UINT32  DqWrDlyShield:      1;  /* [28]: Force to move bit[27:0] to ddrio */
    UINT32  Reserved:           1;  /* [29]: Reserved */
    UINT32  PathEnable:         1;  /* [30]: Raw dq capture */
    UINT32  IoSel:              1;  /* [31]: Raw dq capture */
} AMBA_DDRC_MDDR4_TRN_DQ_WR_DLY_REG_s;

/*
 * DDRC: DRAM Controller LPDDR4 Training DQS Write Delay Register
 */
typedef struct {
    UINT32  Enable0:            1;  /* [0] */
    UINT32  Ctrl0:              4;  /* [4:1] */
    UINT32  Enable1:            1;  /* [5] */
    UINT32  Ctrl1:              4;  /* [9:6] */
    UINT32  Enable2:            1;  /* [10] */
    UINT32  Ctrl2:              4;  /* [14:11] */
    UINT32  Enable3:            1;  /* [15] */
    UINT32  Ctrl3:              4;  /* [19:16] */
    UINT32  Reserved0:          4;  /* [23:20] */
    UINT32  DqWrdlyGap:         2;  /* [25:24] */
    UINT32  DqWrdlyOffset:      2;  /* [27:26] */
    UINT32  Reserved1:          4;  /* [31:28] */
} AMBA_DDRC_MDDR4_TRN_DQS_WR_DLY_REG_s;

/*
 * DDRC: DRAM Controller LPDDR4 Training CK/CA Write Delay Register
 */
typedef struct {
    UINT32  CkEnable0:          1;  /* [0] */
    UINT32  CkCtrl0:            4;  /* [4:1] */
    UINT32  CkEnable1:          1;  /* [5] */
    UINT32  CkCtrl1:            4;  /* [9:6] */
    UINT32  CaEnable0:          1;  /* [10] */
    UINT32  CaCtrl0:            7;  /* [17:11] */
    UINT32  CaEnable1:          1;  /* [18] */
    UINT32  CaCtrl1:            7;  /* [25:19] */
    UINT32  Reserved:           6;  /* [31:26]: Reserved */
} AMBA_DDRC_MDDR4_TRN_CK_CA_WR_DLY_REG_s;

/*
 * DDRC: DRAM Controller LPDDR4 Training DQ Read Delay Register
 */
typedef struct {
    UINT32  Enable0:            1;  /* [0] */
    UINT32  Ctrl0:              4;  /* [4:1] */
    UINT32  Enable1:            1;  /* [5] */
    UINT32  Ctrl1:              4;  /* [9:6] */
    UINT32  Enable2:            1;  /* [10] */
    UINT32  Ctrl2:              4;  /* [14:11] */
    UINT32  Enable3:            1;  /* [15] */
    UINT32  Ctrl3:              4;  /* [19:16] */
    UINT32  Reserved0:          4;  /* [23:20] */
    UINT32  DqWrdlyGap:         2;  /* [25:24] */
    UINT32  DqWrdlyOffset:      2;  /* [27:26] */
    UINT32  Reserved:           4;  /* [31:28]: Reserved */
} AMBA_DDRC_MDDR4_TRN_DQ_RD_DLY_REG_s;

/*
 * DDRC: DRAM Controller LPDDR4 Training Vref Register
 */
typedef struct {
    UINT32  CaA:                7;  /* [6:0]: Vref CA channel A */
    UINT32  DqA:                7;  /* [13:7]: Vref DQ channel A */
    UINT32  CaB:                7;  /* [20:14]: Vref CA channel B */
    UINT32  DqB:                7;  /* [27:21]: Vref DQ channel B */
    UINT32  Reserved:           4;  /* [31:28]: Reserved */
} AMBA_DDRC_MDDR4_TRN_VREF_REG_s;

/*
 * DDRC: DRAM Controller LPDDR4 Ring Oscillator Timer Register
 */
typedef struct {
    UINT32  Timer:              14; /* [13:0]: Timer for the ring oscillator start to end count collection */
    UINT32  Reserved:           18; /* [31:14]: Reserved */
} AMBA_DDRC_MDDR4_RING_OSC_TMR_REG_s;

/*
 * DDRC: DRAM Controller LPDDR4 VREF control Register
 */
typedef struct {
    UINT32  VselCaA:            1;  /* [0]:  */
    UINT32  FsbstCaA:           1;  /* [1]:  */
    UINT32  VselDqA:            1;  /* [2]:  */
    UINT32  FsbstDqA:           1;  /* [3]:  */
    UINT32  VselCaB:            1;  /* [4]:  */
    UINT32  FsbstCaB:           1;  /* [5]:  */
    UINT32  VselDqB:            1;  /* [6]:  */
    UINT32  FsbstDqB:           1;  /* [7]:  */
    UINT32  Reserved:           24; /* [31:8]: Reserved */
} AMBA_DDRC_MDDR4_VREF_CTRL_REG_s;

/*
 * DDRC: DRAM Controller LPDDR4 Mode Read Register
 */
typedef struct {
    UINT32  ReadData:           16; /* [15:0]: Data from reading a mode register */
    UINT32  Reserved:           16; /* [31:16]: Reserved */
} AMBA_DDRC_MDDR4_MODE_READ_REG_s;

/*
 * DDRC: DDRIO DLL Master and Slave Setting Adjustment Register
 */
typedef struct {
    UINT32  Read:                       8;      /* [7:0]: DLL_SEL0 */
    UINT32  Sync:                       8;      /* [15:8]: DLL_SEL1 */
    UINT32  Write:                      8;      /* [23:16]: DLL_SEL2 */
    UINT32  Reserved:                   8;      /* [31:24] Reserved */
} AMBA_DDRC_DDRIO_DLL_REG_s;

/*
 * DDRC: DDRIO DLL Misc. Control Register
 */
typedef struct {
    UINT32  Reserved0:                  15;     /* [14:0] Reserved */
    UINT32  SingleEnd:                  1;      /* [15] */
    UINT32  PueDQ:                      1;      /* [16] */
    UINT32  PdeDQ:                      1;      /* [17] */
    UINT32  NPueDQS:                    1;      /* [18] */
    UINT32  NPdeDQS:                    1;      /* [19] */
    UINT32  PPdeDQS:                    1;      /* [20] */
    UINT32  PPueDQS:                    1;      /* [21] */
    UINT32  CmosRcv:                    1;      /* [22] */
    UINT32  PueCMD:                     1;      /* [23] */
    UINT32  PdeCMD:                     1;      /* [24] */
    UINT32  NPueCK:                     1;      /* [25] */
    UINT32  NPdeCK:                     1;      /* [26] */
    UINT32  PPdeCK:                     1;      /* [27] */
    UINT32  PPueCK:                     1;      /* [28] */
    UINT32  DllPvtCtrl:                 1;      /* [29] 1 - sbc[15] is controlled by ddrc_rct_ddr_refresh_on */
    UINT32  Reserved1:                  2;      /* [31:30] Reserved */
} AMBA_DDRC_DLL_MISC_CTRL_REG_s;

/*
 * DDRC: DLL Shared Bus Control Register
 */
typedef struct {
    UINT32  SharedBusCtrl:              24;     /* [23:0] DLL_SBC */
    UINT32  Reserved:                   8;      /* [31:24] Reserved */
} AMBA_DDRC_DLL_SHARED_BUS_CTRL_REG_s;

/*
 * DDRC: DLL Status Register
 */
typedef struct {
    UINT32  Reserved0:                  8;      /* [7:0] Reserved */
    UINT32  DdrIoStCur:                 6;      /* [13:8] ddrio0_dll0_st_cur[5:0] */
    UINT32  Reserved1:                  2;      /* [15:14] Reserved */
    UINT32  DdrIoVfine:                 5;      /* [20:16] ddrio0_dll0_vfine[4:0] */
    UINT32  Reserved2:                  3;      /* [23:21] Reserved */
    UINT32  DdrIoScOut:                 3;      /* [26:24] ddrio0_dll0_sc_out[2:0] */
    UINT32  DdrIoDebugOut:              1;      /* [27] dll0_debug_out */
    UINT32  Reserved3:                  4;      /* [31:28] Reserved */
} AMBA_DDRC_DLL_STATUS_REG_s;

/*
 * DDRC: DdrIO DLL Sync Ctrl Select Registers
 */
typedef struct {
    UINT32  SyncCtrl:                   24;     /* [23:0] dll_sync_ctrl_selx */
    UINT32  Reserved1:                  8;      /* [31:24] Reserved */
} AMBA_DDRC_DLL_SYNC_CTRL_SEL_REG_s;

/*
 * DDRC: DLL Sync Status Observation0 Registers
 */
typedef struct {
    UINT32  StCurOut0:                  7;      /* [6:0] dll0_sync_st_cur_out */
    UINT32  StCurOut1:                  7;      /* [13:7] dll1_sync_st_cur_out */
    UINT32  StCurOut2:                  7;      /* [20:14] dll2_sync_st_cur_out */
    UINT32  StCurOut3:                  7;      /* [27:21] dll3_sync_st_cur_out */
    UINT32  Reserved1:                  4;      /* [31:28] Reserved */
} AMBA_DDRC_DLL_SYNC_OBSV0_REG_s;

/*
 * DDRC: DLL Sync Status Observation1 Registers
 */
typedef struct {
    UINT32  Msel3:                      1;      /* [0] dll0_sync_msel */
    UINT32  Msel2:                      1;      /* [1] dll1_sync_msel */
    UINT32  Msel1:                      1;      /* [2] dll2_sync_msel */
    UINT32  Msel0:                      1;      /* [3] dll3_sync_msel */
    UINT32  SyncDebugOut3:              1;      /* [4] dll0_sync_debug_out */
    UINT32  SyncDebugOut2:              1;      /* [5] dll1_sync_debug_out */
    UINT32  SyncDebugOut1:              1;      /* [6] dll2_sync_debug_out */
    UINT32  SyncDebugOut0:              1;      /* [7] dll3_sync_debug_out */
    UINT32  DllVshift:                  8;      /* [15:8] dll_vshift */
    UINT32  Reserved0:                  16;     /* [19:16] Reserved */
    UINT32  Dqs0Clk90:                  1;      /* [20] DQS0_CLK90 */
    UINT32  Dqs0Clk0:                   1;      /* [21] DQS0_CLK0 */
    UINT32  Dqs1Clk90:                  1;      /* [22] DQS1_CLK90 */
    UINT32  Dqs1Clk0:                   1;      /* [23] DQS1_CLK0 */
    UINT32  Dqs2Clk90:                  1;      /* [24] DQS2_CLK90 */
    UINT32  Dqs2Clk0:                   1;      /* [25] DQS2_CLK0 */
    UINT32  Dqs3Clk90:                  1;      /* [26] DQS3_CLK90 */
    UINT32  Dqs3Clk0:                   1;      /* [27] DQS3_CLK0 */
    UINT32  Dqs0SyncCtrl:               1;      /* [28] DQS0_SYNC_CTL */
    UINT32  Dqs1SyncCtrl:               1;      /* [29] DQS1_SYNC_CTL */
    UINT32  Dqs2SyncCtrl:               1;      /* [30] DQS2_SYNC_CTL */
    UINT32  Dqs3SyncCtrl:               1;      /* [31] DQS3_SYNC_CTL */
} AMBA_DDRC_DLL_SYNC_OBSV1_REG_s;

/*
 * DDRC: DLL CA Status Register
 */
typedef struct {
    UINT32  Reserved0:                  8;      /* [7:0] Reserved */
    UINT32  DdrCaStCur:                 6;      /* [13:8] ddrio0_dll0_st_cur[5:0] */
    UINT32  Reserved1:                  2;      /* [15:14] Reserved */
    UINT32  DdrCaVfine:                 5;      /* [20:16] ddrio0_dll0_vfine[4:0] */
    UINT32  Reserved2:                  3;      /* [23:21] Reserved */
    UINT32  DdrCaScOut:                 3;      /* [26:24] ddrio0_dll0_sc_out[2:0] */
    UINT32  DdrCaDebugOut:              1;      /* [27] dll0_debug_out */
    UINT32  Reserved3:                  4;      /* [31:28] Reserved */
} AMBA_DDRC_DLL_CA_STATUS_REG_s;

/*
 * DDRC: DLL Duty Ctrl (DQ) Register
 */
typedef struct {
    UINT32  DqDutyCtrlEn:               1;      /* [0] */
    UINT32  Reserved0:                  3;      /* [3:1] Reserved */
    UINT32  DqDutyCtrlN:                3;      /* [6:4] */
    UINT32  DqDutyCtrlP:                3;      /* [9:7] */
    UINT32  DqPreEmpEn:                 1;      /* [10] */
    UINT32  DqDutyYCtrlEn:              1;      /* [11] */
    UINT32  Reserved1:                  4;      /* [15:12] Reserved */
    UINT32  DqDutyYCtrlN:               3;      /* [18:16] */
    UINT32  DqDutyYCtrlP:               3;      /* [21:19] */
    UINT32  DqEqCapCtrl0:               2;      /* [23:22] */
    UINT32  DqEqCapCtrl1:               2;      /* [25:24] */
    UINT32  DqEqEn0:                    1;      /* [26] */
    UINT32  DqEqEn1:                    1;      /* [27] */
    UINT32  DqEqResCtrl0:               2;      /* [29:28] */
    UINT32  DqEqResCtrl1:               2;      /* [31:30] */
} AMBA_DDRC_DQ_DUTY_CTRL_REG_s;

/*
 * DDRC: DLL Duty Ctrl (DQS) Register
 */
typedef struct {
    UINT32  DqsDutyCtrlEn:              1;      /* [0] */
    UINT32  Reserved0:                  3;      /* [3:1] Reserved */
    UINT32  DqsDutyCtrlN:               3;      /* [6:4] */
    UINT32  Reserved1:                  1;      /* [7] Reserved */
    UINT32  DqsDutyCtrlP:               3;      /* [10:8] */
    UINT32  DqsDutyYCtrlEn:             1;      /* [11] */
    UINT32  DqsTestIrcv:                4;      /* [15:12] */
    UINT32  DqsDutyYCtrlN:              3;      /* [18:16] */
    UINT32  DqsDutyYCtrlP:              3;      /* [21:19] */
    UINT32  DqsEqCapCtrl0:              2;      /* [23:22] */
    UINT32  DqsEqCapCtrl1:              2;      /* [25:24] */
    UINT32  DqsEqEn0:                   1;      /* [26] */
    UINT32  DqsEqEn1:                   1;      /* [27] */
    UINT32  DqsEqResCtrl0:              2;      /* [29:28] */
    UINT32  DqsEqResCtrl1:              2;      /* [31:30] */
} AMBA_DDRC_DQS_DUTY_CTRL_REG_s;

/*
 * DDRC: DLL Duty Ctrl (CA) Register
 */
typedef struct {
    UINT32  CaDutyCtrlEn:               1;      /* [0] */
    UINT32  Reserved0:                  3;      /* [3:1] Reserved */
    UINT32  CaDutyCtrlN:                3;      /* [6:4] */
    UINT32  Reserved1:                  1;      /* [7] Reserved */
    UINT32  CkDutyCtrlP:                3;      /* [10:8] */
    UINT32  Reserved2:                  21;     /* [31:11] Reserved */
} AMBA_DDRC_CA_DUTY_CTRL_REG_s;

/*
 * DDRC: DLL Duty Ctrl (CK) Register
 */
typedef struct {
    UINT32  CkDutyCtrlEn:               1;      /* [0] */
    UINT32  Reserved0:                  3;      /* [3:1] Reserved */
    UINT32  CkDutyCtrlN:                3;      /* [6:4] */
    UINT32  Reserved1:                  1;      /* [7] Reserved */
    UINT32  CkDutyCtrlP:                3;      /* [10:8] */
    UINT32  Reserved2:                  1;      /* [11] Reserved */
    UINT32  CkTestIrcv:                 4;      /* [15:12] */
    UINT32  Reserved3:                  16;     /* [31:16] Reserved */
} AMBA_DDRC_CK_DUTY_CTRL_REG_s;

/*
 * DDRC: DRAM Controller All Registers
 */
typedef struct {
    volatile UINT32                         Ctrl;                   /* 0x000(RW): DRAM control */
    volatile AMBA_DDRC_CONFIG_REG_s         Config;                 /* 0x004(RW): DRAM config  */
    volatile UINT32                         Timing1;                /* 0x008(RW): DRAM timing1 */
    volatile UINT32                         Timing2;                /* 0x00C(RW): DRAM timing2 */
    volatile UINT32                         Timing3;                /* 0x010(RW): DRAM timing3 */
    volatile UINT32                         Timing4;                /* 0x014(RW): DRAM timing4 */
    volatile UINT32                         InitCtrl;               /* 0x018(RW): DRAM init control */
    volatile UINT32                         ModeReg;                /* 0x01C(RW): DRAM mode register */
    volatile UINT32                         SelfRefresh;            /* 0x020(RW): DRAM self refresh */
    volatile UINT32                         DqsSync;                /* 0x024(RO): DRAM DQS sync */
    volatile UINT32                         PadTerm;                /* 0x028(RW): DRAM Pad termination */
    volatile UINT32                         ZqCalib;                /* 0x02C(RW): DRAM ZQ calibration */
    volatile UINT32                         RsvdSpace;              /* 0x030(RW): Base address of the reserved DRAM space, must be 128-byte aligned */
    volatile UINT32                         ByteMap;                /* 0x034(RW): Mappping of DRAM DDRIO bytes and DDRC bytes */
    volatile UINT32                         DramPowerDownCtrl;      /* 0x038(RW): Power down */
    volatile UINT32                         DllCalib;               /* 0x03C(RW): DLL calibration */
    volatile UINT32                         DramDebug;              /* 0x040(RW): DRAM debug */
    volatile UINT32                         Ddr4CtrlTiming;         /* 0x044(RW): DDR4 control timing */
    volatile UINT32                         PadTerm2;               /* 0x048(RW): DRAM Pad termination */
    volatile UINT32                         PadTerm3;               /* 0x04C(RW): DRAM Pad termination */
    volatile UINT32                         MpcTrainWriteData[8];   /* 0x050-0x06C(RW): LPDDR4 MPC Training Write Data Register */
    volatile UINT32                         MpcTrainReadData[8];    /* 0x070-0x08C(RW): LPDDR4 MPC Training Read Data Register */
    volatile UINT32                         UInstruct;              /* 0x090(RW): LPDDR4 U-instruction Register */
    volatile UINT32                         TrnDataMask;            /* 0x094(RW): LPDDR4 Training Data Mask Register */
    volatile UINT32                         TrnRingOsciConut;       /* 0x098(RO): LPDDR4 Training Ring Oscillator Count Register */
    volatile UINT32                         TrnCommandDone;         /* 0x09C(RO/WC): LPDDR4 Training Command Done Register */
    volatile UINT32                         TrnDqCapture;           /* 0x0A0(RO): LPDDR4 Training DQ Capture Register */
    volatile UINT32                         TrnCs0DqWriteDly;       /* 0x0A4(RW): LPDDR4 Training DQ Write Delay for Die0 Register */
    volatile UINT32                         TrnCs0DqReadDly;        /* 0x0A8(RW): LPDDR4 Training DQ READ Delay for Die0 Register */
    volatile UINT32                         TrnCs0DqsWriteDly;      /* 0x0AC(RW): LPDDR4 Training DQS Write Delay for Die0 Register */
    volatile UINT32                         TrnCkCaWriteDly;        /* 0x0B0(RW): LPDDR4 Training CK/CA Write Delay Register */
    volatile UINT32                         TrnDqCaVref;            /* 0x0B4(RW): LPDDR4 Training DQ/CA VREF Register */
    volatile UINT32                         TrnSwScratchpad;        /* 0x0B8(RW): LPDDR4 Training Scratchpad Register */
    volatile UINT32                         RingOsciTimerConut;     /* 0x0BC(RW): LPDDR4 Ring Oscillator Timer Register */
    volatile UINT32                         Lpddr4VrefCtrl;         /* 0x0C0(RW): LPDDR4 VREF Control Register */
    volatile UINT32                         Lpddr4DmRead;           /* 0x0C4(RW): LPDDR4 Dm Read Register */
    volatile UINT32                         Lpddr4ModeRead;         /* 0x0C8(RO): LPDDR4 Mode Read Register */
    volatile UINT32                         Lpddr4MpcReadDelay;     /* 0x0CC(RW): LPDDR4 MPC Read Delay Register */
    volatile UINT32                         Reserved0[12];          /* 0x0D0-0x0FC: Reserved */
    volatile UINT32                         DdrioDll0;              /* 0x100(RW): DDRIO DLL0 */
    volatile UINT32                         DdrioDll1;              /* 0x104(RW): DDRIO DLL1 */
    volatile UINT32                         DdrioDll2;              /* 0x108(RW): DDRIO DLL2 */
    volatile UINT32                         DdrioDll3;              /* 0x10C(RW): DDRIO DLL3 */
    volatile UINT32                         DdrioDllCtrlMisc;       /* 0x110(RW): DDRIO DLL Ctrl Misc */
    volatile UINT32                         Reserved1[3];           /* 0x114-0x11C: Reserved */
    volatile UINT32                         DdrioDllCtrlSel0;       /* 0x120(RW): DDRIO DLL Ctrl Select0 */
    volatile UINT32                         DdrioDllCtrlSel1;       /* 0x124(RW): DDRIO DLL Ctrl Select1 */
    volatile UINT32                         DdrioDllCtrlSel2;       /* 0x128(RW): DDRIO DLL Ctrl Select2 */
    volatile UINT32                         DdrioDllCtrlSel3;       /* 0x12C(RW): DDRIO DLL Ctrl Select3 */
    volatile UINT32                         DdrioDllStatusSel0;     /* 0x130(RW): DDRIO DLL Status Select0 */
    volatile UINT32                         DdrioDllStatusSel1;     /* 0x134(RW): DDRIO DLL Status Select1 */
    volatile UINT32                         DdrioDllStatusSel2;     /* 0x138(RW): DDRIO DLL Status Select2 */
    volatile UINT32                         DdrioDllStatusSel3;     /* 0x13C(RW): DDRIO DLL Status Select3 */
    volatile UINT32                         DdrioCs0DllSyncCtlSel0; /* 0x140(RW): DDRIO DLL Sync Ctrl Select0 for Die0 Register */
    volatile UINT32                         DdrioCs0DllSyncCtlSel1; /* 0x144(RW): DDRIO DLL Sync Ctrl Select1 for Die0 Register */
    volatile UINT32                         DdrioCs0DllSyncCtlSel2; /* 0x148(RW): DDRIO DLL Sync Ctrl Select2 for Die0 Register */
    volatile UINT32                         DdrioCs0DllSyncCtlSel3; /* 0x14C(RW): DDRIO DLL Sync Ctrl Select3 for Die0 Register */
    volatile UINT32                         DdrioCs0DllSyncObsv0;   /* 0x150(RW): DDRIO DLL Sync Obsv0 for Die0 Register */
    volatile UINT32                         DdrioCs0DllSyncObsv1;   /* 0x154(RW): DDRIO DLL Sync Obsv1 for Die0 Register */
    volatile UINT32                         DdrioDllCAStatus;       /* 0x158(RW): DDRIO DLL Sync CA Status */
    volatile UINT32                         Reserved2;              /* 0x15C: Reserved */
    volatile UINT32                         DdrioDutyCtrlDq0;       /* 0x160(RW): DDRIO Duty Ctrl Byte0 (DQ) */
    volatile UINT32                         DdrioDutyCtrlDq1;       /* 0x164(RW): DDRIO Duty Ctrl Byte1 (DQ) */
    volatile UINT32                         DdrioDutyCtrlDq2;       /* 0x168(RW): DDRIO Duty Ctrl Byte2 (DQ) */
    volatile UINT32                         DdrioDutyCtrlDq3;       /* 0x16C(RW): DDRIO Duty Ctrl Byte3 (DQ) */
    volatile UINT32                         DdrioDutyCtrlDqs0;      /* 0x170(RW): DDRIO Duty Ctrl Byte0 (DQS) */
    volatile UINT32                         DdrioDutyCtrlDqs1;      /* 0x174(RW): DDRIO Duty Ctrl Byte1 (DQS) */
    volatile UINT32                         DdrioDutyCtrlDqs2;      /* 0x178(RW): DDRIO Duty Ctrl Byte2 (DQS) */
    volatile UINT32                         DdrioDutyCtrlDqs3;      /* 0x17C(RW): DDRIO Duty Ctrl Byte3 (DQS) */
    volatile UINT32                         DdrioDutyCtrlCaA;       /* 0x180(RW): DDRIO Duty Ctrl (CA_A) */
    volatile UINT32                         DdrioDutyCtrlCaB;       /* 0x184(RW): DDRIO Duty Ctrl (CA_B) */
    volatile UINT32                         DdrioDutyCtrlCkA;       /* 0x188(RW): DDRIO Duty Ctrl (CK_A) */
    volatile UINT32                         DdrioDutyCtrlCkB;       /* 0x18C(RW): DDRIO Duty Ctrl (CK_B) */
    volatile UINT32                         DdrioCs1DllSyncCtlSel0; /* 0x190(RW): DDRIO DLL Sync Ctrl Select0 for Die1 Register */
    volatile UINT32                         DdrioCs1DllSyncCtlSel1; /* 0x194(RW): DDRIO DLL Sync Ctrl Select1 for Die1 Register */
    volatile UINT32                         DdrioCs1DllSyncCtlSel2; /* 0x198(RW): DDRIO DLL Sync Ctrl Select2 for Die1 Register */
    volatile UINT32                         DdrioCs1DllSyncCtlSel3; /* 0x19C(RW): DDRIO DLL Sync Ctrl Select3 for Die1 Register */
    volatile UINT32                         DdrioCs1DllSyncObsv0;   /* 0x1A0(RW): DDRIO DLL Sync Obsv0 for Die1 Register */
    volatile UINT32                         DdrioCs1DllSyncObsv1;   /* 0x1A4(RW): DDRIO DLL Sync Obsv1 for Die1 Register */
    volatile UINT32                         Reserved3;              /* 0x1A8-0x1AC: Reserved */
    volatile UINT32                         TrnCs1DqWriteDly;       /* 0x1B4(RW): LPDDR4 Training DQ Write Delay for Die1 Register */
    volatile UINT32                         TrnCs1DqReadDly;        /* 0x1B4(RW): LPDDR4 Training DQ READ Delay for Die1 Register */
    volatile UINT32                         TrnCs1DqsWriteDly;      /* 0x1B8(RW): LPDDR4 Training DQS Write Delay for Die1 Register */
} AMBA_DDRC_REG_s;

/*
 * Defined in AmbaMmioBase.asm
 */
extern AMBA_DDRC_REG_s *const pAmbaDDRC_Reg[1U];

#endif /* AMBA_REG_DDRC_H */
