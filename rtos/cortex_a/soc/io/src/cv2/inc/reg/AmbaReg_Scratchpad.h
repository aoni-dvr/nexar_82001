/**
 *  @file AmbaReg_Scratchpad.h
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
 *  @details Definitions & Constants for AHB Scratchpad Control APIs
 *
 */

#ifndef AMBA_REG_SCRATCHPAD_H
#define AMBA_REG_SCRATCHPAD_H

#ifndef AMBA_TYPES_H
#include "AmbaTypes.h"
#endif

/*
 * Scratchpad: USB Host Control Register
 */
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
    UINT32  SdbFlowCtrl:            1;      /* [31] 1 = enable flow control for ethernet controller */
} AMBA_USB_HOST_CTRL_REG_s;

/*
 * Scratchpad: USB EHCI Sideband Information
 */
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

/*
 * Scratchpad: USB OHCI Sideband Information
 */
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

/*
 * Scratchpad: General Control Register
 */
typedef struct {
    UINT32  Reserved0:              1;      /* [0] Reserved */
    UINT32  EnetReset:              1;      /* [1] (RW) Hard resets Ethernet controller */
    UINT32  EnetTxClkEnable:        1;      /* [2] (RW) Ethernet Tx clock enable mode */
    UINT32  CortexPeripheralSelect: 2;      /* [4:3] (RW) Cortex peripheral selection */
    UINT32  NorSpiSwReset:          1;      /* [5] (RW) Nor SPI software reset */
    UINT32  Reserved1:              7;      /* [12:6] Reserved */
    UINT32  OverCurrentPolarity:    1;      /* [13] (RW) 1 = the polarity of app_prt_ovr_cur_i of the USB host will be inverted */
    UINT32  Reserved2:              2;      /* [15:14] Reserved */
    UINT32  FlashClkEnable:         1;      /* [16] (RW) 1 = enable Flash controller clock */
    UINT32  Reserved3:              1;      /* [17] Reserved */
    UINT32  ResetFiosChan0:         1;      /* [18] (RW) 1 = Reset fios_top channel 0 */
    UINT32  Reserved4:              1;      /* [19] Reserved */
    UINT32  DirectAccessMode:       1;      /* [20] (RW) Direct Access Mode */
    UINT32  Reserved5:              2;      /* [22:21] Reserved */
    UINT32  Enet2ndRefClkSelect:    1;      /* [23] (RW) Ethernet PHY 2nd Reference Clock Freq Selection: 0 - 25MHz, 1 - 50MHz */
    UINT32  Reserved6:              1;      /* [24] Reserved */
    UINT32  I2sReset:               1;      /* [25] (RW) I2S software reset */
    UINT32  EnetResetCtrlBypass:    1;      /* [26] (RW) Ethernet reset control bypass */
    UINT32  Reserved7:              1;      /* [27] Reserved */
    UINT32  RctVicSdxcCardDetect:   1;      /* [28] (R) SDXC Card Detection Post Debouncing Signal */
    UINT32  Reserved8:              1;      /* [29] Reserved */
    UINT32  RctVicSdCardDetect:     1;      /* [30] (R) SD Card Detection Post Debouncing Signal */
    UINT32  EnetClkPolarity:        1;      /* [31] (RW) Ethernet Output Clock Phase Selection: 0 - non-inverted, 1 - inverted */
} AMBA_AHB_CTRL_REG_s;

/*
 * Scratchpad: AXI Interruput FLAG Register
 */
typedef struct {
    UINT32  SoftwareIrq:            14;      /* [13:0] AMBA to AXI Software IRQ */
    UINT32  Reserved:               18;      /* [31:14] */
} AMBA_AHB_IRQ_FLAG_REG_s;

/*
 * Scratchpad: Software Reset Register
 */
typedef struct {
    UINT32  ResetClkDivRGMII:       1;      /* [0] Ethernet RGMII Clock Divider Soft Reset */
    UINT32  Reserved:               31;     /* [31:1] */
} AMBA_AHB_SOFT_RESET_REG_s;

/*
 * Scratchpad: DMA Channel Selection Register
 */
typedef struct {
    UINT32  Chan0Select:            6;      /* [5:0] DMA channel 0 assignment */
    UINT32  Reserved0:              2;      /* [7:6] */
    UINT32  Chan1Select:            6;      /* [13:8] DMA channel 1 assignment */
    UINT32  Reserved1:              2;      /* [15:14] */
    UINT32  Chan2Select:            6;      /* [21:16] DMA channel 2 assignment */
    UINT32  Reserved2:              2;      /* [23:22] */
    UINT32  Chan3Select:            6;      /* [29:24] DMA channel 3 assignment */
    UINT32  Reserved3:              2;      /* [31:30] */
} AMBA_DMA_CHANNEL_SELECT0_REG_s;

typedef struct {
    UINT32  Chan4Select:            6;      /* [5:0] DMA channel 4 assignment */
    UINT32  Reserved0:              2;      /* [7:6] */
    UINT32  Chan5Select:            6;      /* [13:8] DMA channel 5 assignment */
    UINT32  Reserved1:              2;      /* [15:14] */
    UINT32  Chan6Select:            6;      /* [21:16] DMA channel 6 assignment */
    UINT32  Reserved2:              2;      /* [23:22] */
    UINT32  Chan7Select:            6;      /* [29:24] DMA channel 7 assignment */
    UINT32  Reserved3:              2;      /* [31:30] */
} AMBA_DMA_CHANNEL_SELECT1_REG_s;

/*
 * Scratchpad: Random Number Generator Control Register
 */
typedef struct {
    UINT32  SampleComplete:         1;      /* [0] 1 = Assert interrupt at samping completed */
    UINT32  StartSample:            1;      /* [1] Write 1 to start sampling. Read for busy status (0 = Done, 1 = Busy) */
    UINT32  Reserved0:              2;      /* [3:2] Reserved */
    UINT32  SampleRate:             2;      /* [5:4] 0 = 32K cycles, 1 = 64K cycles, 2 = 128K cycles, 3 = 256K cycles */
    UINT32  Reserved1:              26;     /* [31:6] Reserved */
} AMBA_RANDOM_NUMBER_GEN_CTRL_REG_s;

/*
 * Scratchpad: VIN Sync Delay Control Register
 */
typedef struct {
    UINT32  VsyncPolarity:          1;      /* [0] 0 = Active low (low means sync active), 1 = Active high (high means sync active) */
    UINT32  HsyncPolarity:          1;      /* [1] 0 = Active low (low means sync active), 1 = Active high (high means sync active) */
    UINT32  VsyncExtSelect:         1;      /* [2] 0 = Use master_vsync, 1 = Use vsync_ext */
    UINT32  HsyncExtSelect:         1;      /* [3] 0 = Use master_hsync, 1 = Use hsync_ext */
    UINT32  VsyncPipSelect:         1;      /* [4] 0 = from master_vsync, 1 = from pip vsync */
    UINT32  HsyncPipSelect:         1;      /* [5] 0 = from master_hsync, 1 = from pip hsync */
    UINT32  Reserved:               26;     /* [31:6] Reserved */
} AMBA_VIN_SYNC_DELAY_CTRL_REG_s;

/*
 * Scratchpad: VIN Sync Delay Configure Register
 */
typedef struct {
    UINT32  DelayMult0:             3;      /* [2:0] Delay multiplier for vsdelay[0] */
    UINT32  DelayCount0:            10;     /* [12:3] Delay counter for vsdelay[0] */
    UINT32  Reserved0:              3;      /* [15:13] Reserved */
    UINT32  DelayMult1:             3;      /* [18:16] Delay multiplier for vsdelay[1] */
    UINT32  DelayCount1:            10;     /* [28:19] Delay counter for vsdelay[1] */
    UINT32  Reserved1:              3;      /* [31:29] Reserved */
} AMBA_VIN_SYNC_DELAY_CONFIG0_REG_s;

typedef struct {
    UINT32  DelayMult2:             3;      /* [2:0] Delay multiplier for vsdelay[2] */
    UINT32  DelayCount2:            10;     /* [12:3] Delay counter for vsdelay[2] */
    UINT32  Reserved0:              3;      /* [15:13] Reserved */
    UINT32  DelayMult3:             3;      /* [18:16] Delay multiplier for vsdelay[3] */
    UINT32  DelayCount3:            10;     /* [28:19] Delay counter for vsdelay[3] */
    UINT32  Reserved1:              3;      /* [31:29] Reserved */
} AMBA_VIN_SYNC_DELAY_CONFIG1_REG_s;

typedef struct {
    UINT32  DelayMult4:             3;      /* [2:0] Delay multiplier for vsdelay[4] */
    UINT32  DelayCount4:            10;     /* [12:3] Delay counter for vsdelay[4] */
    UINT32  Reserved:               19;     /* [31:13] Reserved */
} AMBA_VIN_SYNC_DELAY_CONFIG2_REG_s;

/*
 * Scratchpad: HDMI Audio Control Register
 */
typedef struct {
    UINT32  HdmiAudClkSrc:          1;      /* [0] 0 = clk_au (4 ch), 1 = clk_au_1ch (2 ch) */
    UINT32  Reserved:               31;     /* [31:1] Reserved */
} AMBA_HDMI_AUD_CTRL_REG_s;

/*
 * Scratchpad: All Registers
 */
typedef struct {
    volatile AMBA_USB_HOST_CTRL_REG_s           UsbHostCtrl;            /* 0x000(RW): USB Host and MAC Control Register */
    volatile AMBA_USB_EHCI_SIDEBAND_REG_s       UsbEhciSideBand;        /* 0x004(RO): USB ECHI Sideband Information Register */
    volatile AMBA_USB_OHCI_SIDEBAND_REG_s       UsbOhciSideBand;        /* 0x008(RO): USB OHCI Sideband Information Register */
    volatile AMBA_AHB_CTRL_REG_s                AhbCtrl;                /* 0x00C(RW): AHB General Control Register */
    volatile AMBA_AHB_IRQ_FLAG_REG_s            AhbIntSet;              /* 0x010(RW): AHB IRQ Set [13:0] */
    volatile AMBA_AHB_IRQ_FLAG_REG_s            AhbIntClear;            /* 0x014(RW): AHB IRQ Clear [13:0] */
    volatile UINT32                             AhbScratchpad[4];       /* 0x018-0x024(RW): General Purpose Scratchpad Data Registers */
    volatile AMBA_AHB_SOFT_RESET_REG_s          AhbSoftReset;           /* 0x028(RW): AHB Soft Reset Register */
    volatile UINT32                             Reserved0;              /* 0x02C: Reserved */
    volatile AMBA_DMA_CHANNEL_SELECT0_REG_s     Dma0ChanSelect0;        /* 0x030(RW): DMA-0 Channel Selection 0 Register */
    volatile AMBA_DMA_CHANNEL_SELECT1_REG_s     Dma0ChanSelect1;        /* 0x034(RW): DMA-0 Channel Selection 1 Register */
    volatile AMBA_DMA_CHANNEL_SELECT0_REG_s     Dma1ChanSelect0;        /* 0x038(RW): DMA-1 Channel Selection 0 Register */
    volatile AMBA_DMA_CHANNEL_SELECT1_REG_s     Dma1ChanSelect1;        /* 0x03C(RW): DMA-1 Channel Selection 1 Register */
    volatile AMBA_RANDOM_NUMBER_GEN_CTRL_REG_s  RngCtrl;                /* 0x040(RW): Random Number Generation Control Register */
    volatile UINT32                             RngData[4];             /* 0x044-0x50(RW): Sampled Random Number Data Registers */
    volatile AMBA_VIN_SYNC_DELAY_CTRL_REG_s     VinSyncDelayCtrl;       /* 0x054(RW): VIN Sync Delay Control Register */
    volatile AMBA_VIN_SYNC_DELAY_CONFIG0_REG_s  VinSyncDelayConfig0;    /* 0x058(RW): VIN Sync Delay Configuration 0 Register */
    volatile AMBA_VIN_SYNC_DELAY_CONFIG1_REG_s  VinSyncDelayConfig1;    /* 0x05C(RW): VIN Sync Delay Configuration 1 Register */
    volatile AMBA_VIN_SYNC_DELAY_CONFIG2_REG_s  VinSyncDelayConfig2;    /* 0x060(RW): VIN Sync Delay Configuration 2 Register */
    volatile UINT32                             OtpCtrl;                /* 0x064(RW): OTP Read Access Enable Register (LSB 1-bit) */
    volatile UINT32                             JtagCtrl;               /* 0x068(RW): JTAG Enable Register (LSB 1-bit) */
    volatile UINT32                             GmiiData;               /* 0x06C(RW): GMII Data Register */
    volatile UINT32                             GmiiAddr;               /* 0x070(RW): GMII Adress Register */
    volatile AMBA_HDMI_AUD_CTRL_REG_s           HdmiAudCtrl;            /* 0x074(RW): HDMI Audio Control Register */
} AMBA_SCRATCHPAD_REG_s;

/*
 * Defined in AmbaMmioBase.asm
 */
#if defined(CONFIG_QNX) || defined(CONFIG_LINUX)
extern AMBA_SCRATCHPAD_REG_s *pAmbaScratchpadS_Reg;
#else
extern AMBA_SCRATCHPAD_REG_s *const pAmbaScratchpadS_Reg;
#endif

#endif /* AMBA_REG_SCRATCHPAD_H */
