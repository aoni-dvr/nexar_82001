/**
 * @file AmbaReg_HDMI.h
 *
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
 *
 */

#ifndef AMBA_REG_HDMI_H
#define AMBA_REG_HDMI_H

#ifndef AMBA_TYPES_H
#include "AmbaTypes.h"
#endif

/*
 * HDMI: Interrupt Flag Register
 */
typedef struct {
    UINT32  VSyncActive:                1;      /* [0] 1 = VSync active edge is detected */
    UINT32  HotPlugAttach:              1;      /* [1] 1 = Hot Plug Detect pin goes high (hot plug is detected) */
    UINT32  HotPlugDetach:              1;      /* [2] 1 = Hot Plug Detect pin goes low (unplug is detected) */
    UINT32  CecRx:                      1;      /* [3] 1 = CEC frame Rx completed */
    UINT32  CecTxFail:                  1;      /* [4] 1 = CEC frame Tx failed (after retry failed) */
    UINT32  CecTxDone:                  1;      /* [5] 1 = CEC frame Tx succeeded */
    UINT32  Reserved:                   7;      /* [12:6] Reserved */
    UINT32  RxSensed:                   1;      /* [13] 1 = Pull-up is detected on TMDS clock lane (the sink leaves standby) */
    UINT32  I2sRxFifoEmpty:             1;      /* [14] 1 = I2S Rx FIFO is empty */
    UINT32  I2sRxFifoFull:              1;      /* [15] 1 = I2S Rx FIFO is full */
    UINT32  I2sRxFifoOver:              1;      /* [16] 1 = I2S Rx FIFO is overrun */
    UINT32  I2sRxThInt:                 1;      /* [17] 1 = I2S Rx data count is reached the threshold */
    UINT32  I2sRxIdle:                  1;      /* [18] 1 = I2S Rx is idle */
    UINT32  CTSChange:                  1;      /* [19] 1 = CTS value is changed */
    UINT32  AsyncFifoFull:              1;      /* [20] 1 = Serializer asynchronous FIFO full (write) */
    UINT32  AsyncFifoEmpty:             1;      /* [21] 1 = Serializer asynchronous FIFO empty (read) */
    UINT32  AsyncFifoUnderflow:         1;      /* [22] 1 = Serializer asynchronous FIFO underflow (below lower bound) */
    UINT32  AsyncFifoOverflow:          1;      /* [23] 1 = Serializer asynchronous FIFO overflow (exceed upper bound) */
    UINT32  CoreIdle:                   1;      /* [24] 1 = HDMI/DVI signal encoder is idle */
    UINT32  RxNotSensed:                1;      /* [25] 1 = No pull-up is detected on TMDS clock lane (the sink enters standby) */
    UINT32  Reserved1:                  6;      /* [31:26] Reserved */
} AMBA_HDMI_INT_FLAG_REG_s;

/*
 * HDMI: Signal Encoder Control Register
 */
/*
 * HDMI: Signal Encoder Control Register: CoreCtrl[1:0]
 */
#define HDMI_CORE_NONE                      0U       /* Disabled */
#define HDMI_CORE_DVI                       2U       /* Enabled with DVI mode (No YCC, No Audio, No InfoFrames) */
#define HDMI_CORE_HDMI                      3U       /* Enabled with HDMI mode */

typedef struct {
    UINT32  Core:                       1;      /* [0] 0 = DVI protocol, 1 = HDMI protocol */
    UINT32  Enable:                     1;      /* [1] 1 = Enable HDMI/DVI Signal Encoder */
    UINT32  Reserved:                   30;     /* [31:2] Reserved */
} AMBA_HDMI_CORE_CTRL_REG_s;

/*
 * HDMI: Clock Gating Control Register
 */
typedef struct {
    UINT32  HdmiTxClock:                1;      /* [0] 1 = Enable HDMI Signal Encoder clock to HDMI Tx logic */
    UINT32  HdcpClock:                  1;      /* [1] 1 = Enable HDCP clock to HDMI Tx logic */
    UINT32  CecClock:                   1;      /* [2] 1 = Enable CEC clock to HDMI Tx logic */
    UINT32  Reserved:                   29;     /* [31:3] Reserved */
} AMBA_HDMI_CLK_CTRL_REG_s;

/*
 * HDMI: Software Reset Register
 */
typedef struct {
    UINT32  Reset:                      1;      /* [0] 0 = Reset, 1 = Reset release */
    UINT32  Reserved:                   31;     /* [31:1] Reserved */
} AMBA_HDMI_RESET_REG_s;

/*
 * HDMI: Real Time Status Register
 */
typedef struct {
    UINT32  Reserved:                   1;      /* [0] Reserved */
    UINT32  HotPlugDetected:            1;      /* [1] 1 = Hot Plug Detect (HPD) pin is high */
    UINT32  Reserved1:                  1;      /* [2] Reserved */
    UINT32  CecRx:                      1;      /* [3] 1 = CEC Rx interrupt is asserted */
    UINT32  CecTxFail:                  1;      /* [4] 1 = CEC Tx interrupt on failure is asserted */
    UINT32  CecTxOk:                    1;      /* [5] 1 = CEC Tx interrupt on success is asserted */
    UINT32  Reserved2:                  7;      /* [12:6] Reserved */
    UINT32  RxSensed:                   1;      /* [13] 1 = TMDS clock lane is pulled-up (the sink has left standby) */
    UINT32  I2sRxFifoEmpty:             1;      /* [14] 1 = I2S Rx FIFO is empty */
    UINT32  I2sRxFifoFull:              1;      /* [15] 1 = I2S Rx FIFO is full */
    UINT32  I2sRxFifoOver:              1;      /* [16] 1 = I2S Rx FIFO is overrun */
    UINT32  I2sRxThInt:                 1;      /* [17] 1 = I2S Rx data count is reached the threshold */
    UINT32  I2sRxIdle:                  1;      /* [18] 1 = I2S Rx is idle */
    UINT32  CtsChange:                  1;      /* [19] 1 = CTS value is changed */
    UINT32  AsyncFifoFull:              1;      /* [20] 1 = Asynchronous FIFO full (write) */
    UINT32  AsyncFifoEmpty:             1;      /* [21] 1 = Asynchronous FIFO empty (read) */
    UINT32  AsyncFifoUnderflow:         1;      /* [22] 1 = Asynchronous FIFO underflow (below lower bound) */
    UINT32  AsyncFifoOverflow:          1;      /* [23] 1 = Asynchronous FIFO overflow (exceed upper bound) */
    UINT32  EncoderIdle:                1;      /* [24] 1 = HDMI/DVI signal encoder is idle */
    UINT32  Reserved3:                  7;      /* [31:25] Reserved */
} AMBA_HDMI_STATUS_REG_s;

/*
 * HDMI: Data Island Register for Audio Clock Frequency Adjustment
 */
/*
 * HDMI: Data Island Register for Audio Clock Frequency Adjustment: AudioClkAdjust[2:0]
 */
#define HDMI_ACLK_ADJ_128FS                 0U       /* CTS = (PCLK * N)/(128 * fs) */
#define HDMI_ACLK_ADJ_256FS                 1U       /* CTS = (PCLK * N)/(256 * fs) */
#define HDMI_ACLK_ADJ_384FS                 2U       /* CTS = (PCLK * N)/(384 * fs) */
#define HDMI_ACLK_ADJ_512FS                 3U       /* CTS = (PCLK * N)/(512 * fs) */
#define HDMI_ACLK_ADJ_768FS                 4U       /* CTS = (PCLK * N)/(768 * fs) */
#define HDMI_ACLK_ADJ_192FS                 5U       /* CTS = (PCLK * N)/(192 * fs) */
#define HDMI_ACLK_ADJ_64FS                  6U       /* CTS = (PCLK * N)/(64 * fs) */
#define HDMI_ACLK_ADJ_32FS                  7U       /* CTS = (PCLK * N)/(32 * fs) */
#define NUM_HDMI_ACLK_ADJ                   8U

typedef struct {
    UINT32  Mode:                       3;      /* [2:0] 0-7 = Indicator to factor of (audio clock frequency)/(audio sample rate) */
    UINT32  Reserved:                   29;     /* [31:3] Reserved */
} AMBA_HDMI_DI_ACLK_ADJUST_REG_s;

/*
 * HDMI: Data Island Register for Audio Clock CTS and N Values Control
 */
typedef struct {
    UINT32  ProgrammableCTS:            1;      /* [0] 0 = CTS values are determined by hardware, 1 = CTS values are programmed by software */
    UINT32  PacketEnable:               1;      /* [1] 1 = Enable Audio Clock Regeneration packet */
    UINT32  Reserved:                   30;     /* [31:2] Reserved */
} AMBA_HDMI_DI_PACKET_ACR_CTRL_REG_s;

/*
 * HDMI: Data Island Register for Audio Clock Regeneration N/CTS Value
 */
typedef struct {
    UINT32  Value:                      20;     /* [19:0] Current N or CTS (Cycle Time Stamp) value */
    UINT32  Reserved:                   12;     /* [31:20] Reserved */
} AMBA_HDMI_DI_PACKET_ACR_VAL_REG_s;

/*
 * HDMI: Data Island Audio Sample Packet Register for Audio Sample Source
 */
typedef struct {
    UINT32  SamplePresent0:             1;      /* [0] 1 = Assign the 1st audio sample of the packet from SAI rx channel 0 */
    UINT32  SamplePresent1:             1;      /* [1] 1 = Assign the 2nd audio sample of the packet from SAI rx channel 1 */
    UINT32  Reserved:                   2;      /* [3:2] Reserved */
    UINT32  SampleFlatline0:            1;      /* [4] 0 = SAI channel 0 represents "flatline" samples */
    UINT32  SampleFlatline1:            1;      /* [5] 0 = SAI channel 1 represents "flatline" samples */
    UINT32  Reserved1:                  26;     /* [31:6] Reserved */
} AMBA_HDMI_DI_AUDIO_SAMPLE_CTRL_REG_s;

/*
 * HDMI: Data Island Audio Sample Packet Register for Audio Sample Layout
 */
/*
 * HDMI: Data Island Audio Sample Packet Register for Audio Sample Layout: AudioSampleLayout[0]
 */
#define HDMI_AUDIO_LAYOUT_2CH_PER_SAMPLE    0U       /* 4 samples per audio sample packet */
#define HDMI_AUDIO_LAYOUT_8CH_PER_SAMPLE    1U       /* 1 sample per audio sample packet */

typedef struct {
    UINT32  Layout:                     1;      /* [0] 0 = Layout 0 (supports 2 channels), 1 = Layout 1 (supports 3 to 8 channels) */
    UINT32  Reserved:                   31;     /* [31:1] Reserved */
} AMBA_HDMI_DI_AUDIO_SAMPLE_LAYOUT_REG_s;

/*
 * HDMI: Data Island Packet Transmit Control Register
 */
#define HDMI_PACKET_TX_DISABLE          0U          /* Do not send packet */
#define HDMI_PACKET_TX_NO_REPEAT        1U          /* Send packet only once */
#define HDMI_PACKET_TX_REPEAT           3U          /* Send packet every V-Sync */

typedef struct {
    UINT32  SendGeneralCtrlPacket:      1;      /* [0] 1 = Enable general control packet transmission */
    UINT32  RepeatGeneralCtrlPacket:    1;      /* [1] 1 = Send general control packet in VBLANK period of every frame */
    UINT32  SendAcpPacket:              1;      /* [2] 1 = Enable ACP packet transmission */
    UINT32  RepeatAcpPacket:            1;      /* [3] 1 = Send ACP packet in VBLANK period of every frame */
    UINT32  SendIsrcPacket:             1;      /* [4] 1 = Enable ISRC packet transmission */
    UINT32  RepeatIsrcPacket:           1;      /* [5] 1 = Send ISRC packet in VBLANK period of every frame */
    UINT32  SendAviPacket:              1;      /* [6] 1 = Enable AVI infoframe packet transmission */
    UINT32  RepeatAviPacket:            1;      /* [7] 1 = Send AVI infoframe packet in VBLANK period of every frame */
    UINT32  SendSpdPacket:              1;      /* [8] 1 = Enable SPD infoframe packet transmission */
    UINT32  RepeatSpdPacket:            1;      /* [9] 1 = Send SPD infoframe packet in VBLANK period of every frame */
    UINT32  SendAudioPacket:            1;      /* [10] 1 = Enable audio infoframe packet transmission */
    UINT32  RepeatAudioPacket:          1;      /* [11] 1 = Send audio infoframe packet in VBLANK period of every frame */
    UINT32  SendMpegPacket:             1;      /* [12] 1 = Enable MPEG source infoframe packet transmission */
    UINT32  RepeatMpegPacket:           1;      /* [13] 1 = Send MPEG source infoframe packet in VBLANK period of every frame */
    UINT32  SendGamutPacket:            1;      /* [14] 1 = Enable Gamut metadata packet transmission */
    UINT32  RepeatGamutPacket:          1;      /* [15] 1 = Send Gamut metadata packet in VBLANK period of every frame */
    UINT32  SendVendorPacket:           1;      /* [16] 1 = Enable vendor specific infoframe packet transmission */
    UINT32  RepeatVendorPacket:         1;      /* [17] 1 = Send vendor specific infoframe packet in VBLANK period of every frame */
    UINT32  Reserved:                   13;     /* [30:18] Reserved */
    UINT32  UpdatePackets:              1;      /* [31] 1 = The new contents of ACP/ISRC/Gamut/InfoFrame packets will be sent after next VSync */
} AMBA_HDMI_DI_PACKET_TX_CTRL_REG_s;

/*
 * HDMI: Data Island Packet General Control Register
 */
typedef struct {
    UINT32  SetAVMUTE:                  1;      /* [0] 1 = Set AVMUTE flag */
    UINT32  Reserved:                   3;      /* [3:1] Reserved */
    UINT32  ClearAVMUTE:                1;      /* [4] 1 = Clear AVMUTE flag */
    UINT32  Reserved1:                  3;      /* [7:5] Reserved */
    UINT32  ColorDepth:                 4;      /* [11:8] 0 = Not specified, 4 = 24 bits, 5 = 30 bits, 6 = 36 bits, 7 = 48 bits per pixel */
    UINT32  PixlePackingPhase:          4;      /* [15:12] 0 = Phase 4, 1 = Phase 1, 2 = Phase 2, 3 = Phase 3 */
    UINT32  DefaultPhase:               1;      /* [16] 1 = The first pixel of some cases shall always use pixel packing phase 0 */
    UINT32  Reserved2:                  15;     /* [31:17] Reserved */
} AMBA_HDMI_DI_PACKET_GENERAL_CTRL_REG_s;

/*
 * HDMI: Data Island Packet Header Register
 */
typedef struct {
    UINT32  Byte0:                      8;      /* [7:0] Packet header byte 0 */
    UINT32  Byte1:                      8;      /* [15:8] Packet header byte 1  */
    UINT32  Byte2:                      8;      /* [23:16] Packet header byte 2 */
    UINT32  Reserved:                   8;      /* [31:24] Reserved */
} AMBA_HDMI_DI_PACKET_HEADER_REG_s;

/*
 * HDMI: Data Island Packet Body (Subpacket) Register
 */
typedef struct {
    UINT32  Byte0:                      8;      /* [7:0] Subpacket byte 0 */
    UINT32  Byte1:                      8;      /* [15:8] Subpacket byte 1 */
    UINT32  Byte2:                      8;      /* [23:16] Subpacket byte 2 */
    UINT32  Byte3:                      8;      /* [31:24] Subpacket byte 3 */
} AMBA_HDMI_DI_PACKET_SUBPACKET_0_REG_s;

typedef struct {
    UINT32  Byte4:                      8;      /* [7:0] Subpacket byte 4 */
    UINT32  Byte5:                      8;      /* [15:8] Subpacket byte 5 */
    UINT32  Byte6:                      8;      /* [23:16] Subpacket byte 6 */
    UINT32  Reserved:                   8;      /* [31:24] Reserved */
} AMBA_HDMI_DI_PACKET_SUBPACKET_4_REG_s;

/*
 * HDMI: Data Island Serial Audio Interface Format Selection Register
 */
/*
 * HDMI: Data Island Serial Audio Interface Format Selection Register: I2sInterface[2:0]
 */
#define HDMI_I2S_FORMAT_LEFT_JUSTIFIED      0U       /* Left-Justified data format */
#define HDMI_I2S_FORMAT_RIGHT_JUSTIFIED     1U       /* Right-Justified data format */
#define HDMI_I2S_FORMAT_MSB_EXTENDED        2U       /* MSB extended data format */
#define HDMI_I2S_FORMAT_I2S                 3U       /* Philips I2S data format */

typedef struct {
    UINT32  Format:                     3;      /* [2:0] 0 = Left-Justified, 1 = Right-Justified, 2 = MSB extended, 4 = I2S mode */
    UINT32  Reserved:                   29;     /* [31:3] Reserved */
} AMBA_HDMI_DI_I2S_INTERFACE_REG_s;

/*
 * HDMI: Data Island I2S Signal Configuration Register
 */
/*
 * HDMI: Data Island I2S Signal Configuration Register: I2sSignalFmt[0]
 */
#define HDMI_I2S_CHAN_WS_LOW_FIRST          0U       /* Channel with Word Select signal low first */
#define HDMI_I2S_CHAN_WS_HIGH_FIRST         1U       /* Channel with Word Select signal high first */
/*
 *HDMI: Data Island I2S Signal Configuration Register: I2sSignalFmt[1]
 */
#define HDMI_I2S_BEHAVIOR_SLAVE             0U       /* Acting as slave device */
#define HDMI_I2S_BEHAVIOR_MASTER            1U       /* Acting as master device */
/*
 *HDMI: Data Island I2S Signal Configuration Register: I2sSignalFmt[2]
 */
#define HDMI_I2S_WORD_MSB_FIRST             0U       /* Audio word is MSB first */
#define HDMI_I2S_WORD_LSB_FIRST             1U       /* Audio word is LSB first */

typedef struct {
    UINT32  ChannelOrder:               1;      /* [0] 0 = Recieve first word at word select 0, 1 = Recieve first word at word select 1 */
    UINT32  Behavior:                   1;      /* [1] 0 = Slave mode, 1 = Master mode (provide the necessary clock signals SCK and WS) */
    UINT32  BitOrder:                   1;      /* [2] 0 = MSB first, 1 = LSB first */
    UINT32  Reserved:                   29;     /* [31:3] Reserved */
} AMBA_HDMI_DI_I2S_SIGNAL_CONFIG_REG_s;

/*
 * HDMI: Data Island I2S Word Length Register
 */
typedef struct {
    UINT32  LengthMinus1:               5;      /* [4:0] Word length minus 1 */
    UINT32  Reserved:                   27;     /* [31:5] Reserved */
} AMBA_HDMI_DI_I2S_WORD_LENGTH_REG_s;

/*
 * HDMI: Data Island I2S Word Position Register
 */
typedef struct {
    UINT32  IgnoredBits:                5;      /* [4:0] Delayed/shifted bits between the audio word and WS/LRCK transition */
    UINT32  Reserved:                   27;     /* [31:5] Reserved */
} AMBA_HDMI_DI_I2S_WORD_POSITION_REG_s;

/*
 * HDMI: Data Island I2S Slot Count Register for DSP mode
 */
typedef struct {
    UINT32  SlotCount:                  5;      /* [4:0] Data slot count after each word select strobe */
    UINT32  Reserved:                   27;     /* [31:5] Reserved */
} AMBA_HDMI_DI_I2S_SLOT_COUNT_REG_s;

/*
 * HDMI: Data Island I2S Receiver FIFO Threshold Register
 */
typedef struct {
    UINT32  Threshold:                  8;      /* [7:0] Receiver FIFO threshold */
    UINT32  Reserved:                   24;     /* [31:8] Reserved */
} AMBA_HDMI_DI_I2S_RX_FIFO_REG_s;

/*
 * HDMI: Data Island I2S Data Sampling Control Register
 */
/*
 * HDMI: Data Island I2S Data Sampling Control Register: I2sSampleCtrl[5]
 */
#define HDMI_I2S_CLOCK_SAMPLE_ON_POSITIVE_EDGE  0U   /* Rising edge of serial clock */
#define HDMI_I2S_CLOCK_SAMPLE_ON_NEGATIVE_EDGE  1U   /* Fallng edge of serial clock */

typedef struct {
    UINT32  Reserved:                   5;      /* [4:0] Reserved */
    UINT32  SampleClkEdge:              1;      /* [5] 1 = Sample data on I2S clock falling edge */
    UINT32  Reserved1:                  26;     /* [31:6] Reserved */
} AMBA_HDMI_DI_I2S_CLOCK_CTRL_REG_s;

/*
 * HDMI: Data Island I2S RX Control Register
 */
typedef struct {
    UINT32  Reset:                      1;      /* [0] 1 = Clean Receiver FIFO */
    UINT32  Enable:                     1;      /* [1] 1 = Enable receiver */
    UINT32  Reserved:                   30;     /* [31:2] Reserved */
} AMBA_HDMI_DI_I2S_CTRL_REG_s;

/*
 * HDMI: Data Island I2S Receiver FIFO Data Register
 */
typedef struct {
    UINT32  Data:                       24;     /* [23:0] Receiver FIFO data */
    UINT32  Reserved:                   8;      /* [31:24] Reserved */
} AMBA_HDMI_DI_I2S_RX_FIFO_DATA_REG_s;

/*
 * HDMI: Data Island I2S Receiver FIFO Level Register
 */
typedef struct {
    UINT32  Level:                      8;      /* [7:0] Receiver FIFO level */
    UINT32  Reserved:                   24;     /* [31:8] Reserved */
} AMBA_HDMI_DI_I2S_RX_FIFO_LEVEL_REG_s;

/*
 * HDMI: Data Island I2S Clock Control Register
 */
typedef struct {
    UINT32  Disable:                    1;      /* [0] Disable I2S receiver */
    UINT32  Reserved:                   31;     /* [31:1] Reserved */
} AMBA_HDMI_DI_I2S_CLK_CTRL_REG_s;

/*
 * HDMI: Data Island Control Options Register
 */
typedef struct {
    UINT32  AudioLeftValid:             1;      /* [0] Left Channel Valid Bit of Audio Packet */
    UINT32  AudioRightValid:            1;      /* [1] Right Channel Valid Bit of Audio Packet */
    UINT32  SPDSendControl:             1;      /* [2] 0 = Send once per field, 1 = Send once per frame of interlaced video mode */
    UINT32  CTSSoftwareMode:            1;      /* [3] 1 = Programmed CTS value will only be sent once */
    UINT32  NCTSPriority:               1;      /* [4] 1 = N/CTS packet has higher priority than audio packet */
    UINT32  I2sRxEnable:                1;      /* [5] 0 = Combined with HDMI internal state machine, 1 = By software programmed */
    UINT32  AudioMode:                  1;      /* [6] 0 = L-PCM, 1 = HBR (High Bit Rate) mode */
    UINT32  AudioPacketDeliverMode:     1;      /* [7] 0 = Receive and then sent received audio packets, 1 = Send audio packets ASAP */
    UINT32  Reserved:                   24;     /* [31:8] Reserved */
} AMBA_HDMI_DI_PACKET_MISC_REG_s;

/*
 * HDMI: Video Signal Timing Register
 */
typedef struct {
    UINT32  Size:                       14;     /* [13:0] Number of lines or number of pixels */
    UINT32  Reserved:                   18;     /* [31:14] Reserved */
} AMBA_HDMI_VIDEO_TIMING_REG_s;

/*
 * HDMI: Video Signal Control Register
 */
typedef struct {
    UINT32  VsyncPolarity:              1;      /* [0] 1 = Negative polarity */
    UINT32  HsyncPolarity:              1;      /* [1] 1 = Negative polarity */
    UINT32  ScanFormat:                 2;      /* [3:2] 0 = Progressive/3D non-field alternatived modes, 1 = Interlaced, 2 = 3D field alternative */
    UINT32  DeepColorMode:              2;      /* [5:4] 0 = 8-bit mode, 1 = 10-bit mode, 2 = 12-bit mode */
    UINT32  ForceSendGeneralCtrlPkt:    1;      /* [6] 1 = Force sending General Control Packet in 8-bit deep color mode */
    UINT32  Reserved:                   25;     /* [31:7] Reserved */
} AMBA_HDMI_VIDEO_CTRL_REG_s;

/*
 * HDMI: Video Signal Detection Control Register
 */
typedef struct {
    UINT32  DetectVsync:                1;      /* [0] 1 = Enable VSync detection */
    UINT32  Reserved:                   31;     /* [31:1] Reserved */
} AMBA_HDMI_VIDEO_DETECTION_CTRL_REG_s;

/*
 * HDMI: Test Mode Control Register
 */
typedef struct {
    UINT32  I2SRxFifoAccess:            1;      /* [0] 1 = Enable FIFO readout though AHB bus */
    UINT32  VideoDataSource:            1;      /* [1] 0 = From VOUT, 1 = Internal pattern generator */
    UINT32  VideoPatternType:           1;      /* [2] 0 = 256-pixel width grey bars, 1 = Programmable background color */
    UINT32  AudioDataSource:            1;      /* [3] 0 = From external source, 1 = Internally generated tone */
    UINT32  Reserved:                   4;      /* [7:4] Reserved */
    UINT32  BgcolorBlue:                8;      /* [15:8] Blue constituent of background color */
    UINT32  BgcolorGreen:               8;      /* [23:16] Green constituent of background color */
    UINT32  Bgcolorred:                 8;      /* [31:24] Red constituent of background color */
} AMBA_HDMI_TEST_MODE_REG_s;

/*
 * HDMI: Asynchronous FIFO Usage Level Indicator Register
 */
typedef struct {
    UINT32  Level:                      5;      /* [4:0] Current usage level */
    UINT32  Reserved:                   3;      /* [7:5] Reserved */
    UINT32  EmptyLevel:                 5;      /* [12:8] Minimum usage level for FIFO empty detection */
    UINT32  Reserved1:                  3;      /* [15:13] Reserved */
    UINT32  FullLevel:                  5;      /* [20:16] Maximum usage level for FIFO full detection */
    UINT32  Reserved2:                  3;      /* [23:21] Reserved */
    UINT32  LowerBound:                 4;      /* [27:24] Lower bound for FIFO underflow detection */
    UINT32  UpperBound:                 4;      /* [31:28] Upper bound for FIFO overflow detection */
} AMBA_HDMI_AFIFO_LEVEL_REG_s;

/*
 * HDMI: Asynchronous FIFO Control Register
 */
typedef struct {
    UINT32  Enable:                     1;      /* [0] Enable asynchronous FIFO */
    UINT32  Reserved:                   31;     /* [31:1] Reserved */
} AMBA_HDMI_AFIFO_CTRL_REG_s;

/*
 * HDMI: Debug Control Register
 */
typedef struct {
    UINT32  BypassAsyncFIFO:            1;      /* [0] 1 = Bypass asynchronous FIFO */
    UINT32  VideoDataSource:            2;      /* [2:1] 0 = VOUT sync and video, 1 = Internal sync and video, 2 = VOUT sync and internal video */
    UINT32  Reserved:                   1;      /* [3] Reserved */
    UINT32  GateBlueChannel:            1;      /* [4] 1 = Gate blue channel of video data */
    UINT32  GateGreenChannel:           1;      /* [5] 1 = Gate green channel of video data */
    UINT32  GateRedChannel:             1;      /* [6] 1 = Gate red channel of video data */
    UINT32  Reserved1:                  1;      /* [7] Reserved */
    UINT32  ColorChannelSwap:           3;      /* [10:8] 0 = RGB, 1 = RBG, 2 = GRB, 3 = GBR, 4 = BRG, 5 = BGR to R'G'B' */
    UINT32  Reserved2:                  1;      /* [11] Reserved */
    UINT32  InvalidAudioPacket:         1;      /* [12] 1 = Enable sending invalid audio packet if no audio sample during horizontal blanking */
    UINT32  Reserved3:                  19;     /* [31:13] Reserved */
} AMBA_HDMI_DEBUG_CTRL_REG_s;

/*
 * HDMI: Scrambler Control Register
 */
typedef struct {
    UINT32  Enable:                     1;      /* [0] 1 = Enable Scrambler */
    UINT32  Reserved:                   31;     /* [31:1] Reserved */
} AMBA_HDMI_SCRAMBLER_CTRL_REG_s;

/*
 * HDMI: HDCP Cryptography Engine Control Register
 */
/*
 * HDMI: HDCP Cryptography Engine Control Register: HdcpCtrl[4]
 */
#define HDMI_HDCP_OESS                      0       /* Oritinal Encryption Status Signaling */
#define HDMI_HDCP_EESS                      1       /* Enhanced Encryption Status Signaling */

typedef struct {
    UINT32  Reserved:                   4;      /* [3:0] Reserved */
    UINT32  EncryptionStatusSignal:     1;      /* [4] 1 = EESS, 0 = OESS */
    UINT32  Reserved1:                  27;     /* [31:5] Reserved */
} AMBA_HDCP_CTRL_REG_s;

/*
 *  HDMI: CEC Control Register
 */
typedef struct {
    UINT32  Reserved:                   13;     /* [12:0] Reserved */
    UINT32  CecArbitration:             1;      /* [13] 1 = Establish CEC arbitration for Tx */
    UINT32  TxRetryTiming:              1;      /* [14] 1 = CEC Tx enters the retry routine only at the nominal sample point */
    UINT32  RxIntTiming:                1;      /* [15] 1 = CEC Rx interrupt is asserted only when logical address of frame and of device are same */
    UINT32  RxAckTiming:                1;      /* [16] 1 = CEC Rx sends ack without waiting for EOM (End of Message) */
    UINT32  StartBitFreeTiming:         3;      /* [19:17] 0 = Default, 2 = 3~5 bits, 3 = 5~7 bits, 4 = 7~10 bits, 5 = More than 10 bits */
    UINT32  TxInitiatorResend:          1;      /* [20] 0 = Assert, 1 = Resend on illegal initiator logical address */
    UINT32  RxAckOnReject:              1;      /* [21] 1 = CEC Rx sends ack if data is rejected */
    UINT32  RxBlockNumMinus1:           4;      /* [25:22] Number of blocks within one Rx frame minus 1 */
    UINT32  TxBlockNumMinus1:           4;      /* [29:26] Number of blocks within one Tx frame minus 1 */
    UINT32  EnableTx:                   1;      /* [30] 1 = Enable a Tx frame transmission */
    UINT32  Reset:                      1;      /* [31] 1 = Release CEC reset */
} AMBA_CEC_CTRL_REG_s;

/*
 * HDMI: CEC Status Register
 */
/*
 * HDMI: CEC Status Register: RxStatus[2:0]
 */
#define HDMI_CEC_STATUS_RX_IDLE_7_10        (0U)     /* Idle for 7 - 10 bit periods */
#define HDMI_CEC_STATUS_RX_IDLE_0_3         (1U)     /* Idle for 0 - 3 bit periods */
#define HDMI_CEC_STATUS_RX_IDLE_3_5         (2U)     /* Idle for 3 - 5 bit periods */
#define HDMI_CEC_STATUS_RX_IDLE_5_7         (3U)     /* Idle for 5 - 7 bit periods */
#define HDMI_CEC_STATUS_RX_BUSY             (4U)     /* Busy */
#define HDMI_CEC_STATUS_RX_PROTECTED        (5U)     /* Protected when Tx is sending data */
#define HDMI_CEC_STATUS_RX_IDLE_10          (6U)     /* Idle for 10+ bit periods */

/*
 * HDMI: CEC Status Register: TxStatus[14:12]
 */
#define HDMI_CEC_STATUS_TX_IDLE             (1U)     /* Idle */
#define HDMI_CEC_STATUS_TX_NORMAL_MSG       (2U)     /* Sending normal message */
#define HDMI_CEC_STATUS_TX_ACK_MSG          (3U)     /* Sending ack message */
#define HDMI_CEC_STATUS_TX_LOWBIT_MSG       (4U)     /* Sending lowbit message */
#define HDMI_CEC_STATUS_TX_PROTECTED        (5U)     /* Protected when Rx is receiving data */

typedef struct {
    UINT32  RxStatus:                   3;      /* [2:0] 1 = Idle1, 2 = Idle2, 3 = Idle3, 0 = Idle4, 6 = Idle5, 4 = Busy, 5 = Tx period */
    UINT32  Reserved:                   3;      /* [5:3] Reserved */
    UINT32  ResetOK:                    1;      /* [6] 1 = Reset signal of CEC register is valid */
    UINT32  RxFail:                     1;      /* [7] 1 = CEC Rx mode fails */
    UINT32  TxNormalStatus:             3;      /* [10:8] 1 = Sending start, 2 = Sending block, 3 = Waiting for ACK (Acknowledge), 4 = Idle */
    UINT32  Reserved1:                  1;      /* [11] Reserved */
    UINT32  TxStatus:                   3;      /* [14:12] 1 = Idle, 2 = Sending normal, 3 = Sending ack, 4 = Sending lowbit, 5 = Rx period */
    UINT32  Reserved2:                  17;     /* [31:15] Reserved */
} AMBA_CEC_STATUS_REG_s;

/*
 * HDMI: CEC Bit Timing Pattern Register
 */
typedef struct {
    UINT32  Period0:                    10;     /* [9:0] Number of CEC base clock cycles for low level period */
    UINT32  Reserved:                   6;      /* [15:10] Reserved */
    UINT32  PeriodAll:                  10;     /* [25:16] Number of CEC base clock cycles for the whole period */
    UINT32  Reserved1:                  6;      /* [31:26] Reserved */
} AMBA_CEC_BIT_TIMING_PATTERN_REG_s;

/*
 * HDMI: CEC Clock Divider Register
 */
typedef struct {
    UINT32  Divider:                    13;     /* [12:0] CEC base clock freq = AHB clock freq / Divider */
    UINT32  Reserved:                   19;     /* [31:13] Reserved */
} AMBA_CEC_CLOCK_DIVIDER_REG_s;

/*
 * HDMI: CEC Tx/Rx Data Register
 */
typedef struct {
    UINT32  Block3:                     8;      /* [7:0] 4th/8th/12th/16th block data */
    UINT32  Block2:                     8;      /* [15:8] 3rd/7th/11th/15th block data */
    UINT32  Block1:                     8;      /* [23:16] 2nd/6th/10th/14th block data */
    UINT32  Block0:                     8;      /* [31:24] 1st/5th/9th/13th block data */
} AMBA_CEC_DATA_REG_s;

/*
 * HDMI: CEC Control 2 Register
 */
typedef struct {
    UINT32  LogicalAddress:             4;      /* [3:0] The current device logical address */
    UINT32  Reserved:                   4;      /* [7:4] Reserved */
    UINT32  MaxRetries:                 5;      /* [12:8] The maximum retry count */
    UINT32  Reserved1:                  3;      /* [15:13] Reserved */
    UINT32  FirstFreeTime:              12;     /* [27:16] The 1st free time node for the 3-bit period */
    UINT32  Reserved2:                  4;      /* [31:28] Reserved */
} AMBA_CEC_CTRL_2_REG_s;

/*
 * HDMI: CEC Tolerance Timing Pattern Register
 */
typedef struct {
    UINT32  Tolerance350us:             10;     /* [9:0] Tolerance timing for 0.35 ms */
    UINT32  Reserved:                   6;      /* [15:10] Reserved */
    UINT32  Tolerance200us:             10;     /* [25:16] Tolerance timing for 0.2 ms */
    UINT32  Reserved1:                  6;      /* [31:26] Reserved */
} AMBA_CEC_TOLE_TIMING_PATTERN_REG_s;

/*
 * HDMI: CEC Low Bit Timing Pattern Register
 */
typedef struct {
    UINT32  MinDuration:                10;     /* [9:0] 1.4 * bit pattern period */
    UINT32  Reserved:                   6;      /* [15:10] Reserved */
    UINT32  MaxDuration:                10;     /* [25:16] 1.6 * bit pattern period */
    UINT32  Reserved1:                  6;      /* [31:26] Reserved */
} AMBA_CEC_LOW_BIT_TIMING_PATTERN_REG_s;

/*
 * HDMI: CEC Miscellaneous Pattern Register
 */
typedef struct {
    UINT32  NominalSamplePoint:         9;      /* [8:0] The cycle count definition of the nominal sample point */
    UINT32  Reserved:                   7;      /* [15:9] Reserved */
    UINT32  MaxGlitchCount:             10;     /* [25:16] CEC Rx uses it to determine if the signal is a glitch */
    UINT32  Reserved1:                  6;      /* [31:26] Reserved */
} AMBA_CEC_MISC_PATTERN_REG_s;

/*
 * HDMI: All Registers
 */
typedef struct {
    volatile UINT32  PktHeader;                  /* +0x00: Packet Header */
    volatile UINT32  PktBody0;                   /* +0x04: Packet Body (Bytes 0-3) */
    volatile UINT32  PktBody1;                   /* +0x08: Packet Body (Bytes 4-6) */
    volatile UINT32  PktBody2;                   /* +0x0C: Packet Body (Bytes 7-10) */
    volatile UINT32  PktBody3;                   /* +0x10: Packet Body (Bytes 11-13) */
    volatile UINT32  PktBody4;                   /* +0x14: Packet Body (Bytes 14-17) */
    volatile UINT32  PktBody5;                   /* +0x18: Packet Body (Bytes 18-20) */
    volatile UINT32  PktBody6;                   /* +0x1C: Packet Body (Bytes 21-24) */
    volatile UINT32  PktBody7;                   /* +0x20: Packet Body (Bytes 25-27) */
} AMBA_HDMI_DATA_ISLAND_PACKET_REG_s;

typedef struct {
    volatile AMBA_HDMI_INT_FLAG_REG_s               IntEnable;                  /* 0x000: Interrupt Enable Register */
    volatile UINT32                                 IntStatus;                  /* 0x004: Interrupt Status Register */
    volatile UINT32                                 CoreCtrl;                   /* 0x008: HDMI/DVI Signal Encoder Control Register */
    volatile AMBA_HDMI_CLK_CTRL_REG_s               ClkCtrl;                    /* 0x00c: Clock Control Register */
    volatile AMBA_HDMI_RESET_REG_s                  CoreReset;                  /* 0x010: HDMI/DVI Signal Encoder Soft Reset Register */
    volatile AMBA_HDMI_STATUS_REG_s                 Status;                     /* 0x014: Real Time Status Register */
    volatile UINT32                                 Reserved[58];               /* Reserved */
    volatile AMBA_HDMI_DI_ACLK_ADJUST_REG_s         AudioClkAdjust;             /* 0x100: Audio Clock Frequency Adjustment Register */
    volatile AMBA_HDMI_DI_PACKET_ACR_CTRL_REG_s     AudioClkRegenCtrl;          /* 0x104: Audio Clock Regeneration Control Register */
    volatile AMBA_HDMI_DI_PACKET_ACR_VAL_REG_s      AudioClkRegenN;             /* 0x108: Current N Value for Audio Clock Regeneration */
    volatile AMBA_HDMI_DI_PACKET_ACR_VAL_REG_s      AudioClkRegenCTS;           /* 0x10c: Current CTS Value for Audio Clock Regeneration */
    volatile AMBA_HDMI_DI_AUDIO_SAMPLE_CTRL_REG_s   AudioSampleCtrl;            /* 0x110: Audio Sample Source Selection Register */
    volatile UINT32                                 AudioChannelStatus[6];      /* 0x114-0x128 Audio Channel Status Register (192-bit) */
    volatile AMBA_HDMI_DI_AUDIO_SAMPLE_LAYOUT_REG_s AudioSampleLayout;          /* 0x12c: Audio Sample Layout Selection Register */
    volatile AMBA_HDMI_DI_PACKET_TX_CTRL_REG_s      PacketTxCtrl;               /* 0x130: Packet Transmit Control Register */
    volatile AMBA_HDMI_DI_PACKET_GENERAL_CTRL_REG_s GeneralCtrlPacket;          /* 0x134: General Control Packet Register */
    AMBA_HDMI_DATA_ISLAND_PACKET_REG_s              PacketACP;                  /* 0x138-0x158: ACP Packet */
    AMBA_HDMI_DATA_ISLAND_PACKET_REG_s              PacketISRC1;                /* 0x15C-0x17C: ISRC1 Packet */
    AMBA_HDMI_DATA_ISLAND_PACKET_REG_s              PacketISRC2;                /* 0x180-0x1A0: ISRC2 Packet */
    AMBA_HDMI_DATA_ISLAND_PACKET_REG_s              PacketInfoFrameAVI;         /* 0x1A4-0x1C4: AVI InfoFrame Packet */
    AMBA_HDMI_DATA_ISLAND_PACKET_REG_s              PacketInfoFrameSPD;         /* 0x1C8-0x1E8: Source Product Descriptor InfoFrame Packet */
    AMBA_HDMI_DATA_ISLAND_PACKET_REG_s              PacketInfoFrameAudio;       /* 0x1EC-0x20C: Audio InfoFrame Packet */
    AMBA_HDMI_DATA_ISLAND_PACKET_REG_s              PacketInfoFrameMPEG;        /* 0x210-0x230: MPEG Source InfoFrame Packet */
    AMBA_HDMI_DATA_ISLAND_PACKET_REG_s              PacketGamut;                /* 0x234-0x254: Gamut Metadata Packet */
    AMBA_HDMI_DATA_ISLAND_PACKET_REG_s              PacketInfoFrameVendor;      /* 0x258-0x278: Vendor-Specific Source InfoFrame Packet */
    volatile UINT32                                 Reserved1[18];              /* Reserved */
    volatile AMBA_HDMI_DI_I2S_INTERFACE_REG_s       I2sInterface;               /* 0x2C4: Serial Audio Interface Format Selection Register */
    volatile UINT32                                 I2sSignalFmt;               /* 0x2C8: I2S Signal Configuration Register */
    volatile AMBA_HDMI_DI_I2S_WORD_LENGTH_REG_s     I2sWordLength;              /* 0x2CC: I2S Word Length Register */
    volatile AMBA_HDMI_DI_I2S_WORD_POSITION_REG_s   I2sWordPosition;            /* 0x2D0: I2S Word Position Register */
    volatile AMBA_HDMI_DI_I2S_SLOT_COUNT_REG_s      I2sSlotCount;               /* 0x2D4: I2S Slot Count Register for DSP Mode */
    volatile AMBA_HDMI_DI_I2S_RX_FIFO_REG_s         I2sRxFifoTh;                /* 0x2D8: I2S Receiver FIFO Threshold Register */
    volatile AMBA_HDMI_DI_I2S_CLOCK_CTRL_REG_s      I2sClockCtrl;               /* 0x2DC: I2S Clock Control Register */
    volatile AMBA_HDMI_DI_I2S_CTRL_REG_s            I2sCtrl;                    /* 0x2E0: I2S Control Register */
    volatile AMBA_HDMI_DI_I2S_RX_FIFO_DATA_REG_s    I2sRxData0;                 /* 0x2E4: I2S0 Receiver FIFO Data Register */
    volatile AMBA_HDMI_DI_I2S_RX_FIFO_DATA_REG_s    I2sRxData1;                 /* 0x2E8: I2S1 Receiver FIFO Data Register */
    volatile UINT32                                 Reserved2[2];               /* Reserved */
    volatile AMBA_HDMI_DI_I2S_RX_FIFO_LEVEL_REG_s   I2sFifoLevel;               /* 0x2F4: I2S Receiver FIFO Level Register */
    volatile AMBA_HDMI_DI_I2S_CLK_CTRL_REG_s        I2sClkCtrl;                 /* 0x2F8: I2S Clock Control Register */
    volatile AMBA_HDMI_DI_PACKET_MISC_REG_s         DIPacketMiscCtrl;           /* 0x2Fc: Data Island Control Options Register */
    volatile UINT32                                 Reserved3[24];              /* Reserved */
    volatile AMBA_HDMI_VIDEO_TIMING_REG_s           VideoVSyncFrontPorch;       /* 0x360: Video Vertical Sync Front Porch Register */
    volatile AMBA_HDMI_VIDEO_TIMING_REG_s           VideoVSyncPulseWidth;       /* 0x364: Video Vertical Sync Pulse Width Register */
    volatile AMBA_HDMI_VIDEO_TIMING_REG_s           VideoVSyncBackPorch;        /* 0x368: Video Vertical Sync Back Porch Register */
    volatile AMBA_HDMI_VIDEO_TIMING_REG_s           VideoHSyncFrontPorch;       /* 0x36C: Video Horizontal Sync Front Porch Register */
    volatile AMBA_HDMI_VIDEO_TIMING_REG_s           VideoHSyncPulseWidth;       /* 0x370: Video Horizontal Sync Pulse Width Register */
    volatile AMBA_HDMI_VIDEO_TIMING_REG_s           VideoHSyncBackPorch;        /* 0x374: Video Horizontal Sync Back Porch Register */
    volatile AMBA_HDMI_VIDEO_TIMING_REG_s           VideoActiveLines;           /* 0x378: Video Active Lines Register */
    volatile AMBA_HDMI_VIDEO_TIMING_REG_s           VideoActivePixels;          /* 0x37C: Video Active Pixels Register */
    volatile AMBA_HDMI_VIDEO_CTRL_REG_s             VideoCtrl;                  /* 0x380: Video Control Register */
    volatile AMBA_HDMI_VIDEO_DETECTION_CTRL_REG_s   VideoDetectCtrl;            /* 0x384: Video Vertical Sync Detection Control Register */
    volatile AMBA_HDMI_TEST_MODE_REG_s              TestMode;                   /* 0x388: Test Mode Setting Register */
    volatile AMBA_HDMI_AFIFO_LEVEL_REG_s            AsyncFifoLevel;             /* 0x38C: Serializer Asynchronous FIFO Usage Level Indicator Register */
    volatile AMBA_HDMI_AFIFO_CTRL_REG_s             AsyncFifoCtrl;              /* 0x390: Serializer Asynchronous FIFO Control Register */
    volatile AMBA_HDMI_DEBUG_CTRL_REG_s             DebugMode;                  /* 0x394: Debug Mode Control Register */
    volatile AMBA_HDMI_SCRAMBLER_CTRL_REG_s         ScramblerCtrl;              /* 0x398: Scrambler Ctrl for EMI/RFI Reduction */
    volatile UINT32                                 Reserved4[25];              /* Reserved */
    volatile AMBA_HDCP_CTRL_REG_s                   HdcpCtrl;                   /* 0x400: HDCP Control Register */
    volatile UINT32                                 Reserved5[127];             /* Reserved */
    volatile AMBA_CEC_CTRL_REG_s                    CecCtrl;                    /* 0x600: CEC Control Register */
    volatile AMBA_CEC_STATUS_REG_s                  CecStatus;                  /* 0x604: CEC Status Register */
    volatile AMBA_CEC_BIT_TIMING_PATTERN_REG_s      CecRxStartBitPtrn;          /* 0x608: CEC Rx Pattern for Start Bit Register */
    volatile AMBA_CEC_BIT_TIMING_PATTERN_REG_s      CecRxZeroBitPtrn;           /* 0x60C: CEC Rx Pattern for Zero Bit Register */
    volatile AMBA_CEC_BIT_TIMING_PATTERN_REG_s      CecRxOneBitPtrn;            /* 0x610: CEC Rx Pattern for One Bit Register */
    volatile AMBA_CEC_CLOCK_DIVIDER_REG_s           CecClkDivider;              /* 0x614: CEC Clock Divider Register */
    volatile UINT32                                 CecTxData0;                 /* 0x618: CEC Tx Data Block 0-3 Register */
    volatile UINT32                                 CecTxData4;                 /* 0x61C: CEC Tx Data Block 4-7 Register */
    volatile UINT32                                 CecTxData8;                 /* 0x620: CEC Tx Data Block 8-11 Register */
    volatile UINT32                                 CecTxData12;                /* 0x624: CEC Tx Data Block 12-15 Register */
    volatile UINT32                                 CecRxData0;                 /* 0x628: CEC Rx Data Block 0-3 Register */
    volatile UINT32                                 CecRxData4;                 /* 0x62C: CEC Rx Data Block 4-7 Register */
    volatile UINT32                                 CecRxData8;                 /* 0x630: CEC Rx Data Block 8-11 Register */
    volatile UINT32                                 CecRxData12;                /* 0x634: CEC Rx Data Block 12-15 Register */
    volatile AMBA_CEC_CTRL_2_REG_s                  CecCtrl2;                   /* 0x638: CEC Control 2 Register */
    volatile AMBA_CEC_TOLE_TIMING_PATTERN_REG_s     CecToleLowerBoundary;       /* 0x63C: CEC Lower Boundary Validation Range pattern Register */
    volatile AMBA_CEC_LOW_BIT_TIMING_PATTERN_REG_s  CecLowBitPtrn;              /* 0x640: CEC Pattern for 'LOW' Bit Register */
    volatile AMBA_CEC_TOLE_TIMING_PATTERN_REG_s     CecToleUpperBoundary;       /* 0x644: CEC Upper Boundary Validation Range pattern Register */
    volatile AMBA_CEC_MISC_PATTERN_REG_s            CecMiscPtrn;                /* 0x648: CEC Miscellaneous Patterns Register */
    volatile AMBA_CEC_BIT_TIMING_PATTERN_REG_s      CecTxStartBitPtrn;          /* 0x64C: CEC Tx Pattern Start Bit Register */
    volatile AMBA_CEC_BIT_TIMING_PATTERN_REG_s      CecTxZeroBitPtrn;           /* 0x650: CEC Tx Pattern Zero Bit Register */
    volatile AMBA_CEC_BIT_TIMING_PATTERN_REG_s      CecTxOneBitPtrn;            /* 0x654: CEC Tx Pattern One Bit Register */
    volatile UINT32                                 Reserved6[42];              /* Reserved */
} AMBA_HDMI_REG_s;

/*
 * Defined in AmbaMmioBase.asm
 */
extern AMBA_HDMI_REG_s *pAmbaHDMI_Reg;

#endif /* AMBA_REG_HDMI_H */
