/**
 *  @file AmbaReg_ScratchpadNS.h
 *
 *  @copyright Copyright (c) 2020 Ambarella, Inc.
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
 *  @details Definitions & Constants for AHB Non-Secure Scratchpad Control APIs
 *
 */

#ifndef AMBA_REG_SCRATCHPAD_NS_H
#define AMBA_REG_SCRATCHPAD_NS_H

#ifndef AMBA_TYPES_H
#include "AmbaTypes.h"
#endif

/*-----------------------------------------------------------------------------------------------*\
 * Non-Secure Scratchpad: Lock Control Register
\*-----------------------------------------------------------------------------------------------*/
typedef struct {
    UINT32  SpSecLock:           1;     /* [0] Lock accesses to ahb_scratchpad_sec */
    UINT32  WriteLock:           1;     /* [1] Write lock for SpSecLock. Need to set this to 0 every time before writing to SpSecLock. */
    UINT32  TimeOutUpperBound:  10;     /* [11:2] Upper bound of the timeout mechanism. */
    UINT32  Reserved:           20;     /* [31:12] Reserved */
} AMBA_LOCK_CTRL_NS_REG_s;

/*-----------------------------------------------------------------------------------------------*\
 * Non-Secure Scratchpad: Non-GPIO Pull-Up/Pull-Down Enable Register
\*-----------------------------------------------------------------------------------------------*/
typedef struct {
    UINT32  Enable0:                1;      /* [0] 1 = Enable pull-up/pull-down function for ?? */
    UINT32  Enable1:                1;      /* [1] 1 = Enable pull-up/pull-down function for ?? */
    UINT32  Enable2:                1;      /* [2] 1 = Enable pull-up/pull-down function for ?? */
    UINT32  Enable3:                1;      /* [3] 1 = Enable pull-up/pull-down function for ?? */
    UINT32  Enable4:                1;      /* [4] 1 = Enable pull-up/pull-down function for ?? */
    UINT32  Enable5:                1;      /* [5] 1 = Enable pull-up/pull-down function for ?? */
    UINT32  Enable6:                1;      /* [6] 1 = Enable pull-up/pull-down function for ?? */
    UINT32  Reserved1:              25;     /* [31:7] */
} AMBA_NON_GPIO_PULL_ENABLE_REG_s;

/*-----------------------------------------------------------------------------------------------*\
 * Non-Secure Scratchpad: Non-GPIO Pull-Up/Pull-Down Selection Register
\*-----------------------------------------------------------------------------------------------*/
typedef struct {
    UINT32  Direction0:             1;      /* [0] 0 = Pull-down, 1 = Pull-up function for ?? */
    UINT32  Direction1:             1;      /* [1] 0 = Pull-down, 1 = Pull-up function for ?? */
    UINT32  Direction2:             1;      /* [2] 0 = Pull-down, 1 = Pull-up function for ?? */
    UINT32  Direction3:             1;      /* [3] 0 = Pull-down, 1 = Pull-up function for ?? */
    UINT32  Direction4:             1;      /* [4] 0 = Pull-down, 1 = Pull-up function for ?? */
    UINT32  Direction5:             1;      /* [5] 0 = Pull-down, 1 = Pull-up function for ?? */
    UINT32  Direction6:             1;      /* [6] 0 = Pull-down, 1 = Pull-up function for ?? */
    UINT32  Reserved1:              25;     /* [31:7] */
} AMBA_NON_GPIO_PULL_SELECT_REG_s;

/*-----------------------------------------------------------------------------------------------*\
 * Non-Secure Scratchpad: DMA Channel Selection Register
\*-----------------------------------------------------------------------------------------------*/
typedef struct {
    UINT32  Chan0Select:            5;      /* [4:0] DMA channel 0 assignment */
    UINT32  Reserved0:              3;      /* [7:5] */
    UINT32  Chan1Select:            5;      /* [12:8] DMA channel 1 assignment */
    UINT32  Reserved1:              3;      /* [15:13] */
    UINT32  Chan2Select:            5;      /* [20:16] DMA channel 2 assignment */
    UINT32  Reserved2:              3;      /* [23:21] */
    UINT32  Chan3Select:            5;      /* [28:24] DMA channel 3 assignment */
    UINT32  Reserved3:              3;      /* [31:29] */
} AMBA_DMA_CHANNEL_SELECT0_REG_s;

typedef struct {
    UINT32  Chan4Select:            5;      /* [4:0] DMA channel 4 assignment */
    UINT32  Reserved0:              3;      /* [7:5] */
    UINT32  Chan5Select:            5;      /* [12:8] DMA channel 5 assignment */
    UINT32  Reserved1:              3;      /* [15:13] */
    UINT32  Chan6Select:            5;      /* [20:16] DMA channel 6 assignment */
    UINT32  Reserved2:              3;      /* [23:21] */
    UINT32  Chan7Select:            5;      /* [28:24] DMA channel 7 assignment */
    UINT32  Reserved3:              3;      /* [31:29] */
} AMBA_DMA_CHANNEL_SELECT1_REG_s;

/*-----------------------------------------------------------------------------------------------*\
 * Non-Secure Scratchpad: General Control Register
\*-----------------------------------------------------------------------------------------------*/
typedef struct {
    UINT32  EnetRxClkPolarity:      1;      /* [0] (RW) Polarity of enet_clk_rx: 0 = non-inverted, 1 = inverted */
    UINT32  EnetReset:              1;      /* [1] (RW) Hard resets Ethernet controller */
    UINT32  EnetTxClkEnable:        1;      /* [2] (RW) Ethernet Tx clock enable mode */
    UINT32  Reserved1:              2;      /* [4:3] Reserved */
    UINT32  NorSpiSwReset:          1;      /* [5] (RW) Nor SPI software reset */
    UINT32  Uart0SwReset:           1;      /* [6] (RW) UART0 software reset */
    UINT32  Uart1SwReset:           1;      /* [7] (RW) UART1 software reset */
    UINT32  Uart2SwReset:           1;      /* [8] (RW) UART2 software reset */
    UINT32  Uart3SwReset:           1;      /* [9] (RW) UART3 software reset */
    UINT32  UartApbSwReset:         1;      /* [10] (RW) APB UART software reset */
    UINT32  Enet1RxClkPolarity:     1;      /* [11] (RW) Polarity of enet1_clk_rx: 0 = non-inverted, 1 = inverted */
    UINT32  Enet1Reset:             1;      /* [12] (RW) Hard resets Ethernet 1 controller */
    UINT32  OverCurrentPolarity:    1;      /* [13] (RW) 1 = the polarity of app_prt_ovr_cur_i of the USB host will be inverted */
    UINT32  Enet1TxClkEnable:       1;      /* [14] (RW) Ethernet 1 Tx clock enable mode */
    UINT32  Reserved2:              1;      /* [15] Reserved */
    UINT32  FlashClkEnable:         1;      /* [16] (RW) 1 = enable Flash controller clock */
    UINT32  Reserved3:              1;      /* [17] Reserved */
    UINT32  ResetFiosChan0:         1;      /* [18] (RW) 1 = Reset fios_top channel 0 */
    UINT32  Reserved4:              2;      /* [20:19] Reserved */
    UINT32  SbdFlowCtrl:            1;      /* [21] (RW) SideBand Flow Control */
    UINT32  Sbd1FlowCtrl:           1;      /* [22] (RW) SideBand 1 Flow Control */
    UINT32  Enet2ndRefClkSelect:    1;      /* [23] (RW) Ethernet PHY 2nd Reference Clock Freq Selection: 0 - 25MHz, 1 - 50MHz */
    UINT32  Reserved5:              1;      /* [24] Reserved */
    UINT32  I2sReset:               1;      /* [25] (RW) I2S software reset */
    UINT32  EnetResetCtrlBypass:    1;      /* [26] (RW) Ethernet reset control bypass */
    UINT32  Enet1ResetCtrlBypass:   1;      /* [27] (RW) Ethernet 1 reset control bypass */
    UINT32  Enet1ClkPolarity:       1;      /* [28] (RW) Ethernet 1 Output Clock Phase Selection: 0 = non-inverted, 1 = inverted */
    UINT32  RctVicSdio0CardDetect:  1;      /* [29] (R) SDIO 0 Card Detection Post Debouncing Signal */
    UINT32  RctVicSdCardDetect:     1;      /* [30] (R) SD Card Detection Post Debouncing Signal */
    UINT32  EnetClkPolarity:        1;      /* [31] (RW) Ethernet Output Clock Phase Selection: 0 = non-inverted, 1 = inverted */
} AMBA_AHB_CTRL_REG_s;

/*-----------------------------------------------------------------------------------------------*\
 * Non-Secure Scratchpad: AXI Interruput FLAG Register
\*-----------------------------------------------------------------------------------------------*/
typedef struct {
    UINT32  SoftwareIrq:            14;      /* [13:0] AMBA to AXI Software IRQ */
    UINT32  Reserved:               18;      /* [31:14] Reserved */
} AMBA_AHB_IRQ_FLAG_REG_s;

/*-----------------------------------------------------------------------------------------------*\
 * Non-Secure Scratchpad: Software Reset Register
\*-----------------------------------------------------------------------------------------------*/
typedef struct {
    UINT32  ResetClkDivRGMII:       1;      /* [0] Ethernet RGMII Clock Divider Soft Reset */
    UINT32  ResetClkDivRGMII1:      1;      /* [1] Ethernet 1 RGMII Clock Divider Soft Reset */
    UINT32  Reserved:               30;     /* [31:2] Reserved */
} AMBA_AHB_SOFT_RESET_REG_s;

/*-----------------------------------------------------------------------------------------------*\
 * Non-Secure Scratchpad: USB Host Control Register
\*-----------------------------------------------------------------------------------------------*/
typedef struct {
    UINT32  ResumeDisable:          1;      /* [0] Resume disable */
    UINT32  UtmiBackwardEnable:     1;      /* [1] UTMI backward enable */
    UINT32  BurstAlignEnable:       1;      /* [2] Burst alignment enable */
    UINT32  EnableIncr4:            1;      /* [3] 1 = Enable AHB burst type Incr4 */
    UINT32  EnableIncr8:            1;      /* [4] 1 = Enable AHB burst type Incr8 */
    UINT32  EnableIncr16:           1;      /* [5] 1 = Enable AHB burst type Incr16 */
    UINT32  AutoPortPowerDisable:   1;      /* [6] 1 = Auto port power disable on overcurrent */
    UINT32  SimulationMode:         1;      /* [7] Simulation mode */
    UINT32  WordIF:                 1;      /* [8] Word interface: 1 = 16-bit, 0 = 8-bit */
    UINT32  NextPowerStateValid:    1;      /* [9] Next power management state valid */
    UINT32  PowerStateValid:        1;      /* [10] Power state valid */
    UINT32  PowerState:             2;      /* [12:11] Power management */
    UINT32  FrameLenAdjEndPoint0:   2;      /* [14:13] Frame length adjustment for end-point 0 */
    UINT32  FrameLenAdjEndPoint1:   2;      /* [16:15] Frame length adjustment for end-point 1 */
    UINT32  FrameLenAdjEndPoint2:   2;      /* [18:17] Frame length adjustment for end-point 2 */
    UINT32  FrameLenAdjEndPoint3:   2;      /* [20:19] Frame length adjustment for end-point 3 */
    UINT32  FrameLenAdjEndPoint4:   2;      /* [22:21] Frame length adjustment for end-point 4 */
    UINT32  FrameLenAdjEndPoint5:   2;      /* [24:23] Frame length adjustment for end-point 5 */
    UINT32  FrameLenAdjHost:        6;      /* [30:25] Frame length adjustment for host */
    UINT32  Reserved1:              1;      /* [31] Reserved */
} AMBA_USB_HOST_CTRL_REG_s;

/*-----------------------------------------------------------------------------------------------*\
 * Non-Secure Scratchpad: USB EHCI Sideband Information
\*-----------------------------------------------------------------------------------------------*/
typedef struct {
    UINT32  NextPowerState:         2;      /* [1:0] Next power management state */
    UINT32  LpsmcState:             4;      /* [5:2] EHCI LPSMC state */
    UINT32  BufAccess:              1;      /* [6] EHCI Buffer access */
    UINT32  TransferCount:          11;     /* [17:7] Transfer count */
    UINT32  PowerStateAck:          1;      /* [18] Power state ACK */
    UINT32  PmeStatus:              1;      /* [19] PME status */
    UINT32  UsbStatus:              6;      /* [25:20] USB status */
    UINT32  UsbDeviceConnect:       1;      /* [26] USB device connection status */
    UINT32  Reserved:               5;      /* [31:27] */
} AMBA_USB_EHCI_SIDEBAND_REG_s;

/*-----------------------------------------------------------------------------------------------*\
 * Non-Secure Scratchpad: USB OHCI Sideband Information
\*-----------------------------------------------------------------------------------------------*/
typedef struct {
    UINT32  AppStartClk:            1;      /* [0] OHCI clock control signal */
    UINT32  CountSelect:            1;      /* [1] Count select */
    UINT32  AppIrq12:               1;      /* [2] External interrupt 12 */
    UINT32  AppIrq1:                1;      /* [3] External interrupt 1 */
    UINT32  AppIoHit:               1;      /* [4] Application I/O hit */
    UINT32  SuspendLegacy:          1;      /* [5] OHCI clock control signal */
    UINT32  RemoteWakeupEnable:     1;      /* [6] Remote wakeup enable */
    UINT32  DevRemoteWakeupEnable:  1;      /* [7] Device remote wake-up enable function */
    UINT32  GlobalSuspend:          1;      /* [8] Host controller is in global suspend state */
    UINT32  LegacyEmulationEnable:  1;      /* [9] OHCI legacy emulation enable */
    UINT32  LegacyIrq12:            1;      /* [10] OHCI legacy irq12 */
    UINT32  LegacyIrq1:             1;      /* [11] OHCI legacy irq1 */
    UINT32  StartOfFrame:           1;      /* [12] Host controller new frame */
    UINT32  SysMangementIrq:        1;      /* [13] HCI bus system management interrupt */
    UINT32  GeneralIrq:             1;      /* [14] HCI bus general interrupt */
    UINT32  RemoteWakeupStatus:     1;      /* [15] Remote wakup status */
    UINT32  BufAccess:              1;      /* [16] Host controller buffer access indicator */
    UINT32  CurrentConnectStatus:   2;      /* [18:17] Current connect status of each port */
    UINT32  OhciSuspend:            2;      /* [20:19] OHCI suspend */
    UINT32  OhciSpeed:              2;      /* [22:21] OHCI USB speed */
    UINT32  Reserved:               9;      /* [31:23] Reserved */
} AMBA_USB_OHCI_SIDEBAND_REG_s;

/*-----------------------------------------------------------------------------------------------*\
 * Non-Secure Scratchpad: Clock Monitor 1:15 (en:upper bound/lower bound)
\*-----------------------------------------------------------------------------------------------*/
typedef struct {
    UINT32  LowerBound:                15;      /* [14:0] Lower bound value */
    UINT32  Reserved0:                  1;      /* [15] Reserved */
    UINT32  UpperBound:                15;      /* [30:16] Upper bound value */
    UINT32  Enable:                     1;      /* [31] Enable */
} AMBA_SPDNS_CLKMON_1_15_REG_s;

/*-----------------------------------------------------------------------------------------------*\
 * Non-Secure Scratchpad: Clock Monitor 1:12 (en:upper bound/lower bound)
\*-----------------------------------------------------------------------------------------------*/
typedef struct {
    UINT32  LowerBound:                12;      /* [11:0] Lower bound value */
    UINT32  Reserved0:                  4;      /* [15:12] Reserved */
    UINT32  UpperBound:                12;      /* [27:16] Upper bound value */
    UINT32  Reserved1:                  3;      /* [30:28] Reserved */
    UINT32  Enable:                     1;      /* [31] Enable */
} AMBA_SPDNS_CLKMON_1_12_REG_s;

/*-----------------------------------------------------------------------------------------------*\
 * Non-Secure Scratchpad: Clock Monitor Cycle Count 15 (Cycle Count)
\*-----------------------------------------------------------------------------------------------*/
typedef struct {
    UINT32  CycleCount:                15;      /* [14:0] Cycle count */
    UINT32  Reserved0:                 17;      /* [31:15] Reserved */
} AMBA_SPDNS_CLKMON_COUNT_15_REG_s;


/*-----------------------------------------------------------------------------------------------*\
 * Non-Secure Scratchpad: Clock Monitor Cycle Count 12 (Cycle Count)
\*-----------------------------------------------------------------------------------------------*/
typedef struct {
    UINT32  CycleCount:                12;      /* [11:0] Cycle count */
    UINT32  Reserved0:                 20;      /* [31:12] Reserved */
} AMBA_SPDNS_CLKMON_COUNT_12_REG_s;

typedef struct {
    UINT32 AhbDma:                      1;      /* [0]  */
    UINT32 AhbBus:                      1;      /* [1]  */
    UINT32 AhbScratchpadNS:             1;      /* [2]  */
    UINT32 AhbVrom:                     1;      /* [3]  */
    UINT32 ApbBrg:                      1;      /* [4]  */
    UINT32 ApbDbg:                      1;      /* [5]  */
    UINT32 AhbNic400:                   1;      /* [6]  */
    UINT32 VsdelayValidClkCfg:          1;      /* [7]  */
    UINT32 VsdelayValidClk:             1;      /* [8]  */
    UINT32 Reserved0:                  23;      /* [31:9] Reserved */
} AMBA_SPDNS_INSERT_ERR_REG_s;

/*-----------------------------------------------------------------------------------------------*\
 * Non-Secure Scratchpad: VIN Sync Delay Configure Register
\*-----------------------------------------------------------------------------------------------*/
typedef struct {
    UINT32  UpdateConfig:           1;      /* [0] Set to 0 before changing parameters, 1 = after changing parameters */
    UINT32  VsdelayEnable:          1;      /* [1] 0 = disable (stall), 1 = enable vsdelay */
    UINT32  SelSourceVsync:         1;      /* [2] 0 = Use master_vsync, 1 = Use vsync_ext */
    UINT32  SelSourceHsync:         1;      /* [3] 0 and 1 are both = Use master_hsync */
    UINT32  VsyncPolarityInput:     1;      /* [4] 0 = Active low (low means sync active), 1 = Active high (high means sync active) */
    UINT32  HsyncPolarityInput:     1;      /* [5] 0 = Active low (low means sync active), 1 = Active high (high means sync active) */
    UINT32  Vsync0PolarityOutput:   1;      /* [6] 0 = Active low (low means sync active), 1 = Active high (high means sync active) */
    UINT32  Vsync1PolarityOutput:   1;      /* [7] 0 = Active low (low means sync active), 1 = Active high (high means sync active) */
    UINT32  HsyncPolarityOutput:    1;      /* [8] 0 = Active low (low means sync active), 1 = Active high (high means sync active) */
    UINT32  SelWidthSrcVsync0:      1;      /* [9] 0 = input as reference, 1 = width_v_0 */
    UINT32  SelWidthSrcVsync1:      1;      /* [10] 0 = input as reference, 1 = width_v_1 */
    UINT32  SelWidthSrcHsync:       1;      /* [11] 0 = input as reference, 1 = width_h */
    UINT32  HsyncVsyncSrc:          1;      /* [12] 0 = from vin0, 1 = from vin1 */
    UINT32  Reserved:              19;      /* [31:13] Reserved */
} AMBA_VIN_SYNC_DELAY_CONFIG_REG_s;

/*-----------------------------------------------------------------------------------------------*\
 * Non-Secure Scratchpad: VIN Sync Delay VS Clock Cycle Register
\*-----------------------------------------------------------------------------------------------*/
typedef struct {
    UINT32  Vsync0DelayClk:        16;      /* [15:0] Delay cycle for vsync0 (unit: clock cycle) */
    UINT32  Vsync1DelayClk:        16;      /* [31:16] Delay cycle for vsync1 (unit: clock cycle) */
} AMBA_VIN_SYNC_DELAY_VSDLYCLK_REG_s;

/*-----------------------------------------------------------------------------------------------*\
 * Non-Secure Scratchpad: VIN Sync Delay HS Clock Cycle Register
\*-----------------------------------------------------------------------------------------------*/
typedef struct {
    UINT32  HsyncDelayClk:         16;      /* [15:0] Delay cycle for hsync (unit: clock cycle) */
    UINT32  Reserved:              16;      /* [31:16] Reserved */
} AMBA_VIN_SYNC_DELAY_HSDLYCLK_REG_s;

/*-----------------------------------------------------------------------------------------------*\
 * Non-Secure Scratchpad: VIN Sync Delay VS Pulse Width Register
\*-----------------------------------------------------------------------------------------------*/
typedef struct {
    UINT32  Vsync0Width:           16;      /* [15:0] Pulse width for vsync0 (unit: clock cycle) */
    UINT32  Vsync1Width:           16;      /* [31:16] Pulse width for vsync1 (unit: clock cycle) */
} AMBA_VIN_SYNC_DELAY_VS_WIDTH_REG_s;

/*-----------------------------------------------------------------------------------------------*\
 * Non-Secure Scratchpad: VIN Sync Delay HS Pulse Width Register
\*-----------------------------------------------------------------------------------------------*/
typedef struct {
    UINT32  HsyncWidth:            16;      /* [15:0] Pulse width for hsync (unit: clock cycle) */
    UINT32  Reserved:              16;      /* [31:16] Reserved */
} AMBA_VIN_SYNC_DELAY_HS_WIDTH_REG_s;

/*-----------------------------------------------------------------------------------------------*\
 * Non-Secure Scratchpad: All Registers
\*-----------------------------------------------------------------------------------------------*/
typedef struct {
    volatile AMBA_LOCK_CTRL_NS_REG_s            LockCtrl;               /* 0x000(RW): Lock Control */
    volatile AMBA_LOCK_CTRL_NS_REG_s            ApbDbgLockCtrl;         /* 0x004(RW): APBDBG Lock Control */
    volatile AMBA_NON_GPIO_PULL_ENABLE_REG_s    NonGpioPullEnable;      /* 0x008(RW): Pull-Up/Pull-Down Enable Registers For Non-GPIO Pins */
    volatile AMBA_NON_GPIO_PULL_SELECT_REG_s    NonGpioPullSelect;      /* 0x00C(RW): Pull-Up/Pull-Down Function Select Registers For Non-GPIO Pins */
    volatile UINT32                             Reserved0;              /* 0x010: Reserved */
    volatile UINT32                             GpioPullEnable[4];      /* 0x014-0x020(RW): Pull-Up/Pull-Down Enable Registers For GPIO Pins */
    volatile UINT32                             Reserved1[3];           /* 0x024-0x02C: Reserved */
    volatile UINT32                             GpioPullSelect[4];      /* 0x030-0x03C(RW): Pull-Up/Pull-Down Function Select Registers For GPIO Pins */
    volatile UINT32                             Reserved2[4];           /* 0x040-0x04C: Reserved */
    volatile AMBA_DMA_CHANNEL_SELECT0_REG_s     Dma0ChanSelect0;        /* 0x050(RW): DMA-0 Channel Selection 0 Register */
    volatile AMBA_DMA_CHANNEL_SELECT1_REG_s     Dma0ChanSelect1;        /* 0x054(RW): DMA-0 Channel Selection 1 Register */
    volatile AMBA_DMA_CHANNEL_SELECT0_REG_s     Dma1ChanSelect0;        /* 0x058(RW): DMA-1 Channel Selection 0 Register */
    volatile AMBA_DMA_CHANNEL_SELECT1_REG_s     Dma1ChanSelect1;        /* 0x05C(RW): DMA-1 Channel Selection 1 Register */
    volatile AMBA_AHB_CTRL_REG_s                AhbCtrl;                /* 0x060(RW): AHB General Control Register */
    volatile AMBA_AHB_IRQ_FLAG_REG_s            SoftIrqSet;             /* 0x064(RW): AXI Soft IRQ Set [13:0] */
    volatile AMBA_AHB_IRQ_FLAG_REG_s            SoftIrqClear;           /* 0x068(RW): AXI Soft IRQ Clear [13:0] */
    volatile UINT32                             AhbScratchpad[4];       /* 0x06C-0x078(RW): General Purpose Scratchpad Data Registers */
    volatile AMBA_AHB_SOFT_RESET_REG_s          AhbSoftReset;           /* 0x07C(RW): AHB Soft Reset Register */
    volatile UINT32                             PtpTimeStampL;          /* 0x080(RO): ENET PTP timestamp signal lower word */
    volatile UINT32                             PtpTimeStampH;          /* 0x084(RO): ENET PTP timestamp signal higher word */
    volatile UINT32                             Ptp1TimeStampL;         /* 0x088(RO): ENET1 PTP timestamp signal lower word */
    volatile UINT32                             Ptp1TimeStampH;         /* 0x08C(RO): ENET1 PTP timestamp signal higher word */
    volatile AMBA_USB_HOST_CTRL_REG_s           UsbHostCtrl;            /* 0x090(RW): USB Host and MAC Control Register */
    volatile AMBA_USB_EHCI_SIDEBAND_REG_s       UsbEhciSideBand;        /* 0x094(RW): USB ECHI Sideband Information Register */
    volatile AMBA_USB_OHCI_SIDEBAND_REG_s       UsbOhciSideBand;        /* 0x098(RW): USB OHCI Sideband Information Register */
    volatile UINT32                             Reserved3;              /* 0x09C(RW): Reserved */
    volatile UINT32                             GmiiData;               /* 0x0A0(RW): GMII Data Register */
    volatile UINT32                             GmiiAddr;               /* 0x0A4(RW): GMII Adress Register */
    volatile UINT32                             Reserved4[2];           /* 0x0A8-0xAC: Reserved */
    volatile UINT32                             SecurityError[3];       /* 0x0B0-0x0B8(RW1C): Security Access Error Status Register */
    volatile UINT32                             Reserved6;              /* 0x0BC: Reserved */
    volatile UINT32                             CpuAccessError[3];      /* 0x0C0-0x0C8(RW1C): CPU Access Error Status Register */
    volatile UINT32                             Reserved7;              /* 0x0CC: Reserved */
    volatile UINT32                             AhbDummy;               /* 0x0D0(RW): For safety purposes. To detect if AHB reset source remains at inactive state with high effectiveness */
    volatile UINT32                             Axi0Dummy;              /* 0x0D4(RW): For safety purposes. To detect if AXI0 reset source remains at inactive state with high effectiveness */
    volatile AMBA_SPDNS_INSERT_ERR_REG_s        InsertError;            /* 0x0D8(RW): Insert error */
    volatile UINT32                             Reserved8;              /* 0x0DC: Reserved */
    volatile AMBA_SPDNS_CLKMON_1_15_REG_s       ClkMonGclkSsiCtrl;      /* 0x0E0(RW): Configuration of GCLK_SSI clock monitor */
    volatile AMBA_SPDNS_CLKMON_1_15_REG_s       ClkMonGclkSsi2Ctrl;     /* 0x0E4(RW): Configuration of GCLK_SSI2 clock monitor */
    volatile AMBA_SPDNS_CLKMON_1_12_REG_s       ClkMonGclkSsi3Ctrl;     /* 0x0E8(RW): Configuration of GCLK_SSI3 clock monitor */
    volatile AMBA_SPDNS_CLKMON_1_15_REG_s       ClkMonGclkUart0Ctrl;    /* 0x0EC(RW): Configuration of GCLK_UART0 clock monitor */
    volatile AMBA_SPDNS_CLKMON_1_15_REG_s       ClkMonGclkUart1Ctrl;    /* 0x0F0(RW): Configuration of GCLK_UART1 clock monitor */
    volatile AMBA_SPDNS_CLKMON_1_15_REG_s       ClkMonGclkUart2Ctrl;    /* 0x0F4(RW): Configuration of GCLK_UART2 clock monitor */
    volatile AMBA_SPDNS_CLKMON_1_15_REG_s       ClkMonGclkUart3Ctrl;    /* 0x0F8(RW): Configuration of GCLK_UART3 clock monitor */
    volatile AMBA_SPDNS_CLKMON_1_15_REG_s       ClkMonGclkUartApbCtrl;  /* 0x0FC(RW): Configuration of GCLK_UART_APB clock monitor */
    volatile AMBA_SPDNS_CLKMON_1_12_REG_s       ClkMonGclkCoreAhbCtrl;  /* 0x100(RW): Configuration of GCLK_CORE_AHB clock monitor */
    volatile AMBA_SPDNS_CLKMON_1_12_REG_s       ClkMonGclkCoreCtrl;     /* 0x104(RW): Configuration of GCLK_CORE clock monitor */
    volatile AMBA_SPDNS_CLKMON_1_12_REG_s       ClkMonGclkAhbCtrl;      /* 0x108(RW): Configuration of GCLK_AHB clock monitor */
    volatile AMBA_SPDNS_CLKMON_1_12_REG_s       ClkMonGclkApbCtrl;      /* 0x10C(RW): Configuration of GCLK_APB clock monitor */
    volatile AMBA_SPDNS_CLKMON_1_12_REG_s       ClkMonGclkDbgCtrl;      /* 0x110(RW): Configuration of GCLK_DBG clock monitor */
    volatile AMBA_SPDNS_CLKMON_1_12_REG_s       ClkMonGclkSdio0Ctrl;    /* 0x114(RW): Configuration of GCLK_SDIO0 clock monitor */
    volatile AMBA_SPDNS_CLKMON_1_12_REG_s       ClkMonGclkSd48Ctrl;     /* 0x118(RW): Configuration of GCLK_SD48 clock monitor */
    volatile AMBA_SPDNS_CLKMON_1_12_REG_s       ClkMonGclkNandCtrl;     /* 0x11C(RW): Configuration of GCLK_NAND clock monitor */
    volatile AMBA_SPDNS_CLKMON_1_12_REG_s       ClkMonGclkSysCnt0Ctrl;  /* 0x120(RW): Configuration of GCLK_SYS_CNT_0 clock monitor */
    volatile AMBA_SPDNS_CLKMON_1_12_REG_s       ClkMonGclkSysCnt1Ctrl;  /* 0x124(RW): Configuration of GCLK_SYS_CNT_1 clock monitor */
    volatile AMBA_SPDNS_CLKMON_1_15_REG_s       ClkMonGclkPwmCtrl;      /* 0x128(RW): Configuration of GCLK_PWM clock monitor */
    volatile AMBA_SPDNS_CLKMON_1_12_REG_s       ClkMonGclkCanCtrl;      /* 0x12C(RW): Configuration of GCLK_CAN clock monitor */
    volatile AMBA_SPDNS_CLKMON_1_12_REG_s       ClkMonGclkAuCtrl;       /* 0x130(RW): Configuration of GCLK_AU clock monitor */
    volatile AMBA_SPDNS_CLKMON_1_12_REG_s       ClkMonGclkGtxCtrl;      /* 0x134(RW): Configuration of GCLK_GTX clock monitor */
    volatile AMBA_SPDNS_CLKMON_1_12_REG_s       ClkMonGclkAxi1Ctrl;     /* 0x138(RW): Configuration of GCLK_AXI1 clock monitor */
    volatile AMBA_SPDNS_CLKMON_1_15_REG_s       ClkMonGclkVoACtrl;      /* 0x13C(RW): Configuration of GCLK_VO_A clock monitor */
    volatile AMBA_SPDNS_CLKMON_1_15_REG_s       ClkMonGclkVoBCtrl;      /* 0x140(RW): Configuration of GCLK_VO_B clock monitor */
    volatile AMBA_SPDNS_CLKMON_1_15_REG_s       ClkMonComphyACtrl;      /* 0x144(RW): Configuration of COMBOPHY_CLKA clock monitor */
    volatile AMBA_SPDNS_CLKMON_1_15_REG_s       ClkMonComphyBCtrl;      /* 0x148(RW): Configuration of COMBOPHY_CLKB clock monitor */
    volatile AMBA_SPDNS_CLKMON_1_12_REG_s       ClkMonGclkVisionCtrl;   /* 0x14C(RW): Configuration of GCLK_VISION clock monitor */
    volatile AMBA_SPDNS_CLKMON_1_12_REG_s       ClkMonGclkDramDiv2Ctrl; /* 0x150(RW): Configuration of GCLK_DRAM_DIV2 clock monitor */
    volatile AMBA_SPDNS_CLKMON_1_12_REG_s       ClkMonGclkFexCtrl;      /* 0x154(RW): Configuration of GCLK_FEX clock monitor */
    volatile AMBA_SPDNS_CLKMON_COUNT_15_REG_s   ClkMonMiscSsiCount;     /* 0x158(RO): Counting value of GCLK_SSI clock monitor */
    volatile AMBA_SPDNS_CLKMON_COUNT_12_REG_s   ClkMonMiscSsi2Count;    /* 0x15C(RO): Counting value of GCLK_SSI2 clock monitor */
    volatile AMBA_SPDNS_CLKMON_COUNT_12_REG_s   ClkMonMiscSsi3Count;    /* 0x160(RO): Counting value of GCLK_SSI3 clock monitor */
    volatile AMBA_SPDNS_CLKMON_COUNT_15_REG_s   ClkMonMiscUart0Count;   /* 0x164(RO): Counting value of GCLK_UART0 clock monitor */
    volatile AMBA_SPDNS_CLKMON_COUNT_15_REG_s   ClkMonMiscUart1Count;   /* 0x168(RO): Counting value of GCLK_UART1 clock monitor */
    volatile AMBA_SPDNS_CLKMON_COUNT_15_REG_s   ClkMonMiscUart2Count;   /* 0x16C(RO): Counting value of GCLK_UART2 clock monitor */
    volatile AMBA_SPDNS_CLKMON_COUNT_15_REG_s   ClkMonMiscUart3Count;   /* 0x170(RO): Counting value of GCLK_UART3 clock monitor */
    volatile AMBA_SPDNS_CLKMON_COUNT_15_REG_s   ClkMonMiscUartApbCount; /* 0x174(RO): Counting value of GCLK_UART_APB clock monitor */
    volatile AMBA_SPDNS_CLKMON_COUNT_12_REG_s   ClkMonMiscCoreAhbCount; /* 0x178(RO): Counting value of GCLK_CORE_AHB clock monitor */
    volatile AMBA_SPDNS_CLKMON_COUNT_12_REG_s   ClkMonMiscCoreCount;    /* 0x17C(RO): Counting value of GCLK_CORE clock monitor */
    volatile AMBA_SPDNS_CLKMON_COUNT_12_REG_s   ClkMonMiscAhbCount;     /* 0x180(RO): Counting value of GCLK_AHB clock monitor */
    volatile AMBA_SPDNS_CLKMON_COUNT_12_REG_s   ClkMonMiscApbCount;     /* 0x184(RO): Counting value of GCLK_APB clock monitor */
    volatile AMBA_SPDNS_CLKMON_COUNT_12_REG_s   ClkMonMiscDbgCount;     /* 0x188(RO): Counting value of GCLK_DBG clock monitor */
    volatile AMBA_SPDNS_CLKMON_COUNT_12_REG_s   ClkMonMiscSdio0Count;   /* 0x18C(RO): Counting value of GCLK_SDIO0 clock monitor */
    volatile AMBA_SPDNS_CLKMON_COUNT_12_REG_s   ClkMonMiscSd48Count;    /* 0x190(RO): Counting value of GCLK_SD48 clock monitor */
    volatile AMBA_SPDNS_CLKMON_COUNT_12_REG_s   ClkMonMiscNandCount;    /* 0x194(RO): Counting value of GCLK_NAND clock monitor */
    volatile AMBA_SPDNS_CLKMON_COUNT_12_REG_s   ClkMonMiscSysCnt0Count; /* 0x198(RO): Counting value of GCLK_SYS_CNT_0 clock monitor */
    volatile AMBA_SPDNS_CLKMON_COUNT_12_REG_s   ClkMonMiscSysCnt1Count; /* 0x19C(RO): Counting value of GCLK_SYS_CNT_1 clock monitor */
    volatile AMBA_SPDNS_CLKMON_COUNT_15_REG_s   ClkMonMiscPwmCount;     /* 0x1A0(RO): Counting value of GCLK_PWM clock monitor */
    volatile AMBA_SPDNS_CLKMON_COUNT_12_REG_s   ClkMonMiscCanCount;     /* 0x1A4(RO): Counting value of GCLK_CAN clock monitor */
    volatile AMBA_SPDNS_CLKMON_COUNT_12_REG_s   ClkMonMiscAuCount;      /* 0x1A8(RO): Counting value of GCLK_AU clock monitor */
    volatile AMBA_SPDNS_CLKMON_COUNT_12_REG_s   ClkMonMiscGtxCount;     /* 0x1AC(RO): Counting value of GCLK_GTX clock monitor */
    volatile AMBA_SPDNS_CLKMON_COUNT_12_REG_s   ClkMonMiscAxi1Count;    /* 0x1B0(RO): Counting value of GCLK_AXI1 clock monitor */
    volatile AMBA_SPDNS_CLKMON_COUNT_15_REG_s   ClkMonMiscVoACount;     /* 0x1B4(RO): Counting value of GCLK_VO_A clock monitor */
    volatile AMBA_SPDNS_CLKMON_COUNT_15_REG_s   ClkMonMiscVoBCount;     /* 0x1B8(RO): Counting value of GCLK_VO_B clock monitor */
    volatile AMBA_SPDNS_CLKMON_COUNT_15_REG_s   ClkMonMiscComphyACount; /* 0x1BC(RO): Counting value of COMBOPHY_CLKA clock monitor */
    volatile AMBA_SPDNS_CLKMON_COUNT_15_REG_s   ClkMonMiscComphyBCount; /* 0x1C0(RO): Counting value of COMBOPHY_CLKB clock monitor */
    volatile AMBA_SPDNS_CLKMON_COUNT_12_REG_s   ClkMonMisVisionCount;   /* 0x1C4(RO): Counting value of GCLK_VISION clock monitor */
    volatile AMBA_SPDNS_CLKMON_COUNT_12_REG_s   ClkMonMisDramDiv2Count; /* 0x1C8(RO): Counting value of GCLK_DRAM_DIV2 clock monitor */
    volatile AMBA_SPDNS_CLKMON_COUNT_12_REG_s   ClkMonMisFexCount;      /* 0x1CC(RO): Counting value of GCLK_FEX clock monitor */
    volatile AMBA_VIN_SYNC_DELAY_CONFIG_REG_s   VsDelayConfig;          /* 0x1D0(RW): VIN Sync Delay Configuration Register */
    volatile UINT32                             VsDelayVs0Pulse;        /* 0x1D4(RW): VSync0 Delay Period (unit: hsync) */
    volatile UINT32                             VsDelayVs1Pulse;        /* 0x1D8(RW): VSync1 Delay Period (unit: hsync) */
    volatile AMBA_VIN_SYNC_DELAY_VSDLYCLK_REG_s VsDelayVClock;          /* 0x1DC(RW): Vsync0/1 Delay Period */
    volatile AMBA_VIN_SYNC_DELAY_HSDLYCLK_REG_s VsDelayHClock;          /* 0x1E0(RW): Hsync Delay Period */
    volatile AMBA_VIN_SYNC_DELAY_VS_WIDTH_REG_s VsDelayWidthV;          /* 0x1E4(RW): Vsync0/1 Pulse Width */
    volatile AMBA_VIN_SYNC_DELAY_HS_WIDTH_REG_s VsDelayWidthH;          /* 0x1E8(RW): Hsync Pulse Width*/
    volatile UINT32                             SaftyEnable;            /* 0x1EC(RW): */
    volatile AMBA_SPDNS_CLKMON_1_12_REG_s       ClkMonGclk1DbgCtrl;     /* 0x1F0(RW): Configuration of AMBA1_GCLK_DBG clock monitor */
    volatile AMBA_SPDNS_CLKMON_1_12_REG_s       ClkMonGclk1CoreAhbCtrl; /* 0x1F4(RW): Configuration of AMBA1_GCLK_CORE_AHB clock monitor */
    volatile AMBA_SPDNS_CLKMON_1_12_REG_s       ClkMonGclk1CoreCtrl;    /* 0x1F8(RW): Configuration of AMBA1_GCLK_CORE clock monitor */
    volatile AMBA_SPDNS_CLKMON_1_12_REG_s       ClkMonGclk1AhbCtrl;     /* 0x1FC(RW): Configuration of AMBA1_GCLK_AHB clock monitor */
    volatile AMBA_SPDNS_CLKMON_1_12_REG_s       ClkMonGclk1Axi1Ctrl;    /* 0x200(RW): Configuration of AMBA1_GCLK_AXI1 clock monitor */
    volatile AMBA_SPDNS_CLKMON_COUNT_12_REG_s   ClkMonMis1DbgCount;     /* 0x204(RO): Counting value of AMBA1_GCLK_DBG clock monitor */
    volatile AMBA_SPDNS_CLKMON_COUNT_12_REG_s   ClkMonMis1CoreAhbCount; /* 0x208(RO): Counting value of AMBA1_GCLK_CORE_AHB clock monitor */
    volatile AMBA_SPDNS_CLKMON_COUNT_12_REG_s   ClkMonMis1CoreCount;    /* 0x20C(RO): Counting value of AMBA1_GCLK_CORE clock monitor */
    volatile AMBA_SPDNS_CLKMON_COUNT_12_REG_s   ClkMonMis1AhbCount;     /* 0x210(RO): Counting value of AMBA1_GCLK_AHB clock monitor */
    volatile AMBA_SPDNS_CLKMON_COUNT_12_REG_s   ClkMonMis1Axi1Count;    /* 0x214(RO): Counting value of AMBA1_GCLK_AXI1 clock monitor */
} AMBA_SCRATCHPAD_NS_REG_s;

/*
 * Defined in AmbaMmioBase.asm
 */
#ifdef CONFIG_QNX
extern AMBA_SCRATCHPAD_NS_REG_s *pAmbaScratchpadNS_Reg;
#else
extern AMBA_SCRATCHPAD_NS_REG_s *const pAmbaScratchpadNS_Reg;
#endif
#endif /* AMBA_REG_SCRATCHPAD_H */
