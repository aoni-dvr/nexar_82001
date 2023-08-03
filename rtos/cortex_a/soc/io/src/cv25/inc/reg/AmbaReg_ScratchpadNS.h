/**
 *  @file AmbaReg_ScratchpadNS.h
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
 *  @details Definitions & Constants for Non-Secure Scratchpad Control APIs
 *
 */

#ifndef AMBA_REG_SCRATCHPAD_NS_H
#define AMBA_REG_SCRATCHPAD_NS_H

#ifndef AMBA_TYPES_H
#include "AmbaTypes.h"
#endif

/*
 * Non-Secure Scratchpad: Non-GPIO Pull-Up/Pull-Down Enable Register
 */
typedef struct {
    UINT32  Reserved0:              1;      /* [0] */
    UINT32  Enable1:                1;      /* [1] 1 = Enable pull-up/pull-down function for clk_au */
    UINT32  Enable2:                1;      /* [2] 1 = Enable pull-up/pull-down function for detect_vbus */
    UINT32  Enable3:                1;      /* [3] 1 = Enable pull-up/pull-down function for clk_si */
    UINT32  Reserved1:              28;     /* [31:4] */
} AMBA_NON_AMBA_GPIO_PULL_ENABLE_REG_s;

/*
 * Non-Secure Scratchpad: Non-GPIO Pull-Up/Pull-Down Selection Register
 */
typedef struct {
    UINT32  Reserved0:              1;      /* [0] */
    UINT32  Direction1:             1;      /* [1] 0 = Pull-down, 1 = Pull-up function for clk_au */
    UINT32  Direction2:             1;      /* [2] 0 = Pull-down, 1 = Pull-up function for detect_vbus */
    UINT32  Direction3:             1;      /* [3] 0 = Pull-down, 1 = Pull-up function for clk_si */
    UINT32  Reserved1:              28;     /* [31:4] */
} AMBA_NON_AMBA_GPIO_PULL_SELECT_REG_s;

/*
 * Non-Secure Scratchpad: GPIO Pull-Up/Pull-Down Enable Register
 */
typedef struct {
    UINT32  Enable0:                1;      /* [0] 1 = Enable pull-up/pull-down function for GPIO[0|32|64|96|128|160|192] */
    UINT32  Enable1:                1;      /* [1] 1 = Enable pull-up/pull-down function for GPIO[1|33|65|97|129|161|193] */
    UINT32  Enable2:                1;      /* [2] 1 = Enable pull-up/pull-down function for GPIO[2|34|66|98|130|162|194] */
    UINT32  Enable3:                1;      /* [3] 1 = Enable pull-up/pull-down function for GPIO[3|35|67|99|131|163|195] */
    UINT32  Enable4:                1;      /* [4] 1 = Enable pull-up/pull-down function for GPIO[4|36|68|100|132|164|196] */
    UINT32  Enable5:                1;      /* [5] 1 = Enable pull-up/pull-down function for GPIO[5|37|69|101|133|165|197] */
    UINT32  Enable6:                1;      /* [6] 1 = Enable pull-up/pull-down function for GPIO[6|38|70|102|134|166|198] */
    UINT32  Enable7:                1;      /* [7] 1 = Enable pull-up/pull-down function for GPIO[7|39|71|103|135|167|199] */
    UINT32  Enable8:                1;      /* [8] 1 = Enable pull-up/pull-down function for GPIO[8|40|72|104|136|168|200] */
    UINT32  Enable9:                1;      /* [9] 1 = Enable pull-up/pull-down function for GPIO[9|41|73|105|137|169|201] */
    UINT32  Enable10:               1;      /* [10] 1 = Enable pull-up/pull-down function for GPIO[10|42|74|106|138|170|202] */
    UINT32  Enable11:               1;      /* [11] 1 = Enable pull-up/pull-down function for GPIO[11|43|75|107|139|171|203] */
    UINT32  Enable12:               1;      /* [12] 1 = Enable pull-up/pull-down function for GPIO[12|44|76|108|140|172|204] */
    UINT32  Enable13:               1;      /* [13] 1 = Enable pull-up/pull-down function for GPIO[13|45|77|109|141|173|205] */
    UINT32  Enable14:               1;      /* [14] 1 = Enable pull-up/pull-down function for GPIO[14|46|78|110|142|174|206] */
    UINT32  Enable15:               1;      /* [15] 1 = Enable pull-up/pull-down function for GPIO[15|47|79|111|143|175|207] */
    UINT32  Enable16:               1;      /* [16] 1 = Enable pull-up/pull-down function for GPIO[16|48|80|112|144|176|208] */
    UINT32  Enable17:               1;      /* [17] 1 = Enable pull-up/pull-down function for GPIO[17|49|81|113|145|177|209] */
    UINT32  Enable18:               1;      /* [18] 1 = Enable pull-up/pull-down function for GPIO[18|50|82|114|146|178|210] */
    UINT32  Enable19:               1;      /* [19] 1 = Enable pull-up/pull-down function for GPIO[19|51|83|115|147|179|211] */
    UINT32  Enable20:               1;      /* [20] 1 = Enable pull-up/pull-down function for GPIO[20|52|84|116|148|180|212] */
    UINT32  Enable21:               1;      /* [21] 1 = Enable pull-up/pull-down function for GPIO[21|53|85|117|149|181|213] */
    UINT32  Enable22:               1;      /* [22] 1 = Enable pull-up/pull-down function for GPIO[22|54|86|118|150|182|214] */
    UINT32  Enable23:               1;      /* [23] 1 = Enable pull-up/pull-down function for GPIO[23|55|87|119|151|183|215] */
    UINT32  Enable24:               1;      /* [24] 1 = Enable pull-up/pull-down function for GPIO[24|56|88|120|152|184|216] */
    UINT32  Enable25:               1;      /* [25] 1 = Enable pull-up/pull-down function for GPIO[25|57|89|121|153|185|217] */
    UINT32  Enable26:               1;      /* [26] 1 = Enable pull-up/pull-down function for GPIO[26|58|90|122|154|186|218] */
    UINT32  Enable27:               1;      /* [27] 1 = Enable pull-up/pull-down function for GPIO[27|59|91|123|155|187|219] */
    UINT32  Enable28:               1;      /* [28] 1 = Enable pull-up/pull-down function for GPIO[28|60|92|124|156|188|220] */
    UINT32  Enable29:               1;      /* [29] 1 = Enable pull-up/pull-down function for GPIO[29|61|93|125|157|189|221] */
    UINT32  Enable30:               1;      /* [30] 1 = Enable pull-up/pull-down function for GPIO[30|62|94|126|158|190|222] */
    UINT32  Enable31:               1;      /* [31] 1 = Enable pull-up/pull-down function for GPIO[31|63|95|127|159|191|223] */
} AMBA_GPIO_PULL_ENABLE_REG_s;

/*
 * Non-Secure Scratchpad: GPIO Pull-Up/Pull-Down Selection Register
 */
typedef struct {
    UINT32  Direction0:             1;      /* [0] 0 = Pull-down, 1 = Pull-up function for GPIO[0|32|64|96|128|160|192] */
    UINT32  Direction1:             1;      /* [1] 0 = Pull-down, 1 = Pull-up function for GPIO[1|33|65|97|129|161|193] */
    UINT32  Direction2:             1;      /* [2] 0 = Pull-down, 1 = Pull-up function for GPIO[2|34|66|98|130|162|194] */
    UINT32  Direction3:             1;      /* [3] 0 = Pull-down, 1 = Pull-up function for GPIO[3|35|67|99|131|163|195] */
    UINT32  Direction4:             1;      /* [4] 0 = Pull-down, 1 = Pull-up function for GPIO[4|36|68|100|132|164|196] */
    UINT32  Direction5:             1;      /* [5] 0 = Pull-down, 1 = Pull-up function for GPIO[5|37|69|101|133|165|197] */
    UINT32  Direction6:             1;      /* [6] 0 = Pull-down, 1 = Pull-up function for GPIO[6|38|70|102|134|166|198] */
    UINT32  Direction7:             1;      /* [7] 0 = Pull-down, 1 = Pull-up function for GPIO[7|39|71|103|135|167|199] */
    UINT32  Direction8:             1;      /* [8] 0 = Pull-down, 1 = Pull-up function for GPIO[8|40|72|104|136|168|200] */
    UINT32  Direction9:             1;      /* [9] 0 = Pull-down, 1 = Pull-up function for GPIO[9|41|73|105|137|169|201] */
    UINT32  Direction10:            1;      /* [10] 0 = Pull-down, 1 = Pull-up function for GPIO[10|42|74|106|138|170|202] */
    UINT32  Direction11:            1;      /* [11] 0 = Pull-down, 1 = Pull-up function for GPIO[11|43|75|107|139|171|203] */
    UINT32  Direction12:            1;      /* [12] 0 = Pull-down, 1 = Pull-up function for GPIO[12|44|76|108|140|172|204] */
    UINT32  Direction13:            1;      /* [13] 0 = Pull-down, 1 = Pull-up function for GPIO[13|45|77|109|141|173|205] */
    UINT32  Direction14:            1;      /* [14] 0 = Pull-down, 1 = Pull-up function for GPIO[14|46|78|110|142|174|206] */
    UINT32  Direction15:            1;      /* [15] 0 = Pull-down, 1 = Pull-up function for GPIO[15|47|79|111|143|175|207] */
    UINT32  Direction16:            1;      /* [16] 0 = Pull-down, 1 = Pull-up function for GPIO[16|48|80|112|144|176|208] */
    UINT32  Direction17:            1;      /* [17] 0 = Pull-down, 1 = Pull-up function for GPIO[17|49|81|113|145|177|209] */
    UINT32  Direction18:            1;      /* [18] 0 = Pull-down, 1 = Pull-up function for GPIO[18|50|82|114|146|178|210] */
    UINT32  Direction19:            1;      /* [19] 0 = Pull-down, 1 = Pull-up function for GPIO[19|51|83|115|147|179|211] */
    UINT32  Direction20:            1;      /* [20] 0 = Pull-down, 1 = Pull-up function for GPIO[20|52|84|116|148|180|212] */
    UINT32  Direction21:            1;      /* [21] 0 = Pull-down, 1 = Pull-up function for GPIO[21|53|85|117|149|181|213] */
    UINT32  Direction22:            1;      /* [22] 0 = Pull-down, 1 = Pull-up function for GPIO[22|54|86|118|150|182|214] */
    UINT32  Direction23:            1;      /* [23] 0 = Pull-down, 1 = Pull-up function for GPIO[23|55|87|119|151|183|215] */
    UINT32  Direction24:            1;      /* [24] 0 = Pull-down, 1 = Pull-up function for GPIO[24|56|88|120|152|184|216] */
    UINT32  Direction25:            1;      /* [25] 0 = Pull-down, 1 = Pull-up function for GPIO[25|57|89|121|153|185|217] */
    UINT32  Direction26:            1;      /* [26] 0 = Pull-down, 1 = Pull-up function for GPIO[26|58|90|122|154|186|218] */
    UINT32  Direction27:            1;      /* [27] 0 = Pull-down, 1 = Pull-up function for GPIO[27|59|91|123|155|187|219] */
    UINT32  Direction28:            1;      /* [28] 0 = Pull-down, 1 = Pull-up function for GPIO[28|60|92|124|156|188|220] */
    UINT32  Direction29:            1;      /* [29] 0 = Pull-down, 1 = Pull-up function for GPIO[29|61|93|125|157|189|221] */
    UINT32  Direction30:            1;      /* [30] 0 = Pull-down, 1 = Pull-up function for GPIO[30|62|94|126|158|190|222] */
    UINT32  Direction31:            1;      /* [31] 0 = Pull-down, 1 = Pull-up function for GPIO[31|63|95|127|159|191|223] */
} AMBA_GPIO_PULL_SELECT_REG_s;

/*
 * Non-Secure Scratchpad: DMA Channel Selection Register
 */
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

/*
 * Non-Secure Scratchpad: General Control Register
 */
typedef struct {
    UINT32  EnetRxClkPolarity:      1;      /* [0] Polarity of enet_clk_rx: 0 = non-inverted, 1 = inverted */
    UINT32  EnetReset:              1;      /* [1] (RW) Hard resets Ethernet controller */
    UINT32  EnetTxClkEnable:        1;      /* [2] (RW) Ethernet Tx clock enable mode */
    UINT32  CortexPeripheralSelect: 2;      /* [4:3] (RW) Cortex peripheral selection */
    UINT32  NorSpiSwReset:          1;      /* [5] (RW) Nor SPI software reset */
    UINT32  Reserved1:              7;      /* [12:6] Reserved */
    UINT32  OverCurrentPolarity:    1;      /* [13] (RW) 1 = the polarity of app_prt_ovr_cur_i of the USB host will be inverted */
    UINT32  Reserved2:              2;      /* [15:14] Reserved */
    UINT32  FlashClkEnable:         1;      /* [16] (RW) 1 = enable Flash controller clock */
    UINT32  DmicReset:              1;      /* [17] (RW) 1 = Reset DMIC */
    UINT32  ResetFiosChan0:         1;      /* [18] (RW) 1 = Reset fios_top channel 0 */
    UINT32  Reserved3:              1;      /* [19] Reserved */
    UINT32  DirectAccessMode:       1;      /* [20] (RW) Direct Access Mode */
    UINT32  Reserved4:              2;      /* [22:21] Reserved */
    UINT32  Enet2ndRefClkSelect:    1;      /* [23] (RW) Ethernet PHY 2nd Reference Clock Freq Selection: 0 - 25MHz, 1 - 50MHz */
    UINT32  DmicI2sSelect:          1;      /* [24] (RW) Selection for i2s_sd_i: 0 = Legacy I2S input, 1 = From DMIC */
    UINT32  I2sReset:               1;      /* [25] (RW) 1 = Reset I2S */
    UINT32  EnetResetCtrlBypass:    1;      /* [26] (RW) Ethernet reset control bypass */
    UINT32  Reserved5:              1;      /* [27] Reserved */
    UINT32  RctVicSd2CardDetect:    1;      /* [28] (R) SD2/SDIO1 Card Detection Post Debouncing Signal */
    UINT32  RctVicSd1CardDetect:    1;      /* [29] (R) SD1/SDIO0 Card Detection Post Debouncing Signal */
    UINT32  RctVicSd0CardDetect:    1;      /* [30] (R) SD0/SD Card Detection Post Debouncing Signal */
    UINT32  EnetClkPolarity:        1;      /* [31] (RW) Ethernet Output Clock Phase Selection: 0 = non-inverted, 1 = inverted */
} AMBA_AHB_CTRL_REG_s;

/*
 * Non-Secure Scratchpad: AXI Interruput FLAG Register
 */
typedef struct {
    UINT32  SoftwareIrq:            14;      /* [13:0] AMBA to AXI Software IRQ */
    UINT32  Reserved:               18;      /* [31:14] */
} AMBA_AHB_IRQ_FLAG_REG_s;

/*
 * Non-Secure Scratchpad: Software Reset Register
 */
typedef struct {
    UINT32  ResetClkDivRGMII:       1;      /* [0] Ethernet RGMII Clock Divider Soft Reset */
    UINT32  Reserved:               31;     /* [31:1] */
} AMBA_AHB_SOFT_RESET_REG_s;

/*
 * Non-Secure Scratchpad: USB Host Control Register
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
 * Non-Secure Scratchpad: USB EHCI Sideband Information
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
 * Non-Secure Scratchpad: USB OHCI Sideband Information
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
 * Non-Secure Scratchpad: All Registers
 */
typedef struct {
    volatile UINT32                             Reserved0[2];           /* 0x000-0x004: Reserved */
    volatile AMBA_NON_AMBA_GPIO_PULL_ENABLE_REG_s    NonGpioPullEnable;      /* 0x008(RW): Pull-Up/Pull-Down Enable Registers For Non-GPIO Pins */
    volatile AMBA_NON_AMBA_GPIO_PULL_SELECT_REG_s    NonGpioPullSelect;      /* 0x00C(RW): Pull-Up/Pull-Down Function Select Registers For Non-GPIO Pins */
    volatile UINT32                             Reserved1;              /* 0x010: Reserved */
    volatile UINT32                             GpioPullEnable[4];      /* 0x014-0x020(RW): Pull-Up/Pull-Down Enable Registers For GPIO Pins */
    volatile UINT32                             Reserved2[3];           /* 0x024-0x02C: Reserved */
    volatile UINT32                             GpioPullSelect[4];      /* 0x030-0x03C(RW): Pull-Up/Pull-Down Function Select Registers For GPIO Pins */
    volatile UINT32                             Reserved3[4];           /* 0x040-0x04C: Reserved */
    volatile AMBA_DMA_CHANNEL_SELECT0_REG_s     Dma0ChanSelect0;        /* 0x050(RW): DMA-0 Channel Selection 0 Register */
    volatile AMBA_DMA_CHANNEL_SELECT1_REG_s     Dma0ChanSelect1;        /* 0x054(RW): DMA-0 Channel Selection 1 Register */
    volatile AMBA_DMA_CHANNEL_SELECT0_REG_s     Dma1ChanSelect0;        /* 0x058(RW): DMA-1 Channel Selection 0 Register */
    volatile AMBA_DMA_CHANNEL_SELECT1_REG_s     Dma1ChanSelect1;        /* 0x05C(RW): DMA-1 Channel Selection 1 Register */
    volatile AMBA_AHB_CTRL_REG_s                AhbCtrl;                /* 0x060(RW): AHB General Control Register */
    volatile AMBA_AHB_IRQ_FLAG_REG_s            SoftIrqSet;             /* 0x064(RW): AXI Soft IRQ Set [13:0] */
    volatile AMBA_AHB_IRQ_FLAG_REG_s            SoftIrqClear;           /* 0x068(RW): AXI Soft IRQ Clear [13:0] */
    volatile UINT32                             AhbScratchpad[4];       /* 0x06C-0x078(RW): General Purpose Scratchpad Data Registers */
    volatile AMBA_AHB_SOFT_RESET_REG_s          AhbSoftReset;           /* 0x07C(RW): AHB Soft Reset Register */
    volatile UINT32                             Reserved4[4];           /* 0x080-0x8C: Reserved */
    volatile AMBA_USB_HOST_CTRL_REG_s           UsbHostCtrl;            /* 0x090(RW): USB Host and MAC Control Register */
    volatile AMBA_USB_EHCI_SIDEBAND_REG_s       UsbEhciSideBand;        /* 0x094(RW): USB ECHI Sideband Information Register */
    volatile AMBA_USB_OHCI_SIDEBAND_REG_s       UsbOhciSideBand;        /* 0x098(RW): USB OHCI Sideband Information Register */
    volatile UINT32                             Reserved5;              /* 0x09C: Reserved */
    volatile UINT32                             GmiiData;               /* 0x0A0(RW): GMII Data Register */
    volatile UINT32                             GmiiAddr;               /* 0x0A4(RW): GMII Adress Register */
    volatile UINT32                             Reserved6[2];           /* 0x0A8-0xAC: Reserved */
    volatile UINT32                             SecurityError[3];       /* 0x0B0-0x0B8(RW1C): Security Access Error Status Register */
    volatile UINT32                             Reserved7;              /* 0x0BC: Reserved */
} AMBA_SCRATCHPAD_NS_REG_s;

/*
 * Defined in AmbaMmioBase.asm
 */
#ifdef CONFIG_LINUX
extern AMBA_SCRATCHPAD_NS_REG_s *pAmbaScratchpadNS_Reg;
#else
extern AMBA_SCRATCHPAD_NS_REG_s *const pAmbaScratchpadNS_Reg;
#endif

#endif /* AMBA_REG_SCRATCHPAD_NS_H */
