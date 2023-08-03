/**
 *  @file AmbaB8Reg_PwmEnc.h
 *
 *  @copyright Copyright (c) 2015 Ambarella, Inc.
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
 *  @details Definitions & Constants for B6 PWM Encoder Control Registers
 *
 */

#ifndef AMBA_B8_REG_PWM_ENC_H
#define AMBA_B8_REG_PWM_ENC_H

/*-----------------------------------------------------------------------------------*\
 * B6 PWM ENC : Encoder Control Register
\*-----------------------------------------------------------------------------------*/
typedef struct {
    UINT32  Enable:                 1;      /* [0] PWM Encoder enable */
    UINT32  NodeID:                 3;      /* [3:1] */
    UINT32  ReplayTimes:            3;      /* [6:4] */
    UINT32  Reserved0:              1;      /* [7] Reserved */
    UINT32  DupPktNum:              4;      /* [11:8] */
    UINT32  IgnoreNgChEn:           1;      /* [12] */
    UINT32  StopAtError:            1;      /* [13] */
    UINT32  Reserved1:              2;      /* [15:14] Reserved */
    UINT32  MultiCastID:            12;     /* [27:16] */
    UINT32  FsyncDupPkt:            3;      /* [30:28] */
    UINT32  EncIdle:                1;      /* [31]  */
} B8_PWM_ENC_CTRL_REG_s;

/*-----------------------------------------------------------------------------------*\
 * B6 PWN ENC : Credit Config 0 Register
\*-----------------------------------------------------------------------------------*/
typedef struct {
    UINT32  Ch0CreditPwr:           3;      /* [2:0] */
    UINT32  Reserved0:              29;     /* [31:3] Reserved */
} B8_PWM_ENC_CREDIT_CFG0_REG_s;

/*-----------------------------------------------------------------------------------*\
 * B6 PWM ENC : Reset Control 0 Register
\*-----------------------------------------------------------------------------------*/
typedef struct {
    UINT32  TxReset0:               1;      /* [0]  ch-0 */
    UINT32  Reserved0:              11;     /* [11:1] Reserved */
    UINT32  TxReset12:              1;      /* [12] ch-12 */
    UINT32  Reserved1:              3;      /* [15:13] Reserved */
    UINT32  RportFifoReset0:        1;      /* [16] ch-0 */
    UINT32  Reserved2:              11;     /* [27:17] Reserved */
    UINT32  RportFifoReset12:       1;      /* [28] ch-12 */
    UINT32  Reserved3:              2;      /* [30:29] Reserved */
    UINT32  AwportFifoReset:        1;      /* [31] */
} B8_PWM_ENC_RESET_CTRL0_REG_s;

/*-----------------------------------------------------------------------------------*\
 * B6 PWN ENC : Credit Config 1 Register
\*-----------------------------------------------------------------------------------*/
typedef struct {
    UINT32  Reserved0:              6;      /* [5:0] Reserved */
    UINT32  Ch12CreditPwr:          3;      /* [8:6] */
    UINT32  Reserved1:              7;      /* [15:9] */
    UINT32  LsChunkSize:            3;      /* [18:16] */
    UINT32  HsChunkSize:            3;      /* [21:19] */
    UINT32  Reserved2:              10;     /* [31:22] */
} B8_PWM_ENC_CREDIT_CFG1_REG_s;

/*-----------------------------------------------------------------------------------*\
 * B6 PWM ENC : AW Port FIFO Config Register
\*-----------------------------------------------------------------------------------*/
typedef struct {
    UINT32  LsAwportNearEmptyTh:    8;      /* [7:0] The threshold to trigger ls_awport_fifo_near_empty interrupt status */
    UINT32  Reserved0:              8;      /* [15:8] Reserved */
    UINT32  Hsc0AwportNearEmptyTh:  8;      /* [23:16] The threshold to trigger hsc0_awport_fifo_near_empty interrupt status */
    UINT32  Reserved1:              8;      /* [31:24] Reserved */
} B8_PWM_ENC_AWPORT_FIFO_CFG_REG_s;


/*-----------------------------------------------------------------------------------*\
 * B6 PWM ENC : Reset Control 1 Register
\*-----------------------------------------------------------------------------------*/
typedef struct {
    UINT32  SendRstPkt0:            1;      /* [0] 0 */
    UINT32  Reserved0:              11;     /* [11:1] Reserved */
    UINT32  SendRstPkt12:           1;      /* [12] 12 */
    UINT32  Reserved1:              19;     /* [31:13] Reserved */
} B8_PWM_ENC_RESET_CTRL1_REG_s;

/*-----------------------------------------------------------------------------------*\
 * B6 PWM ENC : Address and Command Port Register
\*-----------------------------------------------------------------------------------*/
typedef struct {
    UINT32  CmdPort:                15;     /* [14:0] Command port */
    UINT32  Syncmode:               1;      /* [15] Synchronization mode */
    UINT32  AddrPOrt:               16;     /* [31:16] Address port */
} B8_PWM_ENC_ADDR_CMD_PORT_REG_s;

/*-----------------------------------------------------------------------------------*\
 * B6 PWM ENC : Rport Status Register
\*-----------------------------------------------------------------------------------*/
typedef struct {
    UINT32  Ch0RportFifoLevel:      7;      /* [6:0] The number of used entries in RPORT_FIFO */
    UINT32  Reserved0:              1;      /* [7] Reserved */
    UINT32  Ch0RportFifoNearFullTh: 7;      /* [14:8] The threshold to trigger rport_fifo_near_full interrupt status */
    UINT32  Reserved1:              17;     /* [31:15] Reserved */
} B8_PWM_ENC_RPORT_FIFO_STATUS0_REG_s;

typedef struct {
    UINT32  Ch12RportFifoLevel:     7;     /* [6:0] The number of used entries in RPORT_FIFO */
    UINT32  Reserved0:              1;     /* [7] Reserved */
    UINT32  Ch12RportFifoNearFullTh:7;     /* [14:8] The threshold to trigger rport_fifo_near_full interrupt status */
    UINT32  Reserved1:              17;    /* [31:15] Reserved */
} B8_PWM_ENC_RPORT_FIFO_STATUS6_REG_s;

/*-----------------------------------------------------------------------------------*\
 * B6 PWM ENC : Delay Meter Register
\*-----------------------------------------------------------------------------------*/
typedef struct {
    UINT32  Ch0DelayMeter:          12;     /* [11:0] Round trip delay meter (in ticks of TX_SymbolClock) */
    UINT32  Reserved0:              3;      /* [14:12] */
    UINT32  Ch0DelayMeterEn:        1;      /* [15] 1 = Enable delay meter for channel 0 */
    UINT32  Reserved1:              16;     /* [31:16] */
} B8_PWM_ENC_DELAY_METER_REG_s;

/*-----------------------------------------------------------------------------------*\
 * B6 PWM ENC : Awport Status Register
\*-----------------------------------------------------------------------------------*/
typedef struct {
    UINT32  LsAwportFifoSpace:      8;      /* [7:0] The number of unused entries in AWPORT_FIFO for LS channel */
    UINT32  Reserved0:              8;      /* [15:8] Reserved */
    UINT32  HscAwportFifoSpace:     8;      /* [23:16] The number of unused entries in AWPORT_FIFO for HS channel 0 */
    UINT32  Reserved1:              8;      /* [31:24] Reserved */
} B8_PWM_ENC_AWPORT_FIFO_STATUS_REG_s;

/*-----------------------------------------------------------------------------------*\
 * B6 PWM ENC : Delay Time Register
\*-----------------------------------------------------------------------------------*/
typedef struct {
    UINT32  Ch0DelayTime:           12;     /* [11:0] The delay time for channel #0 to send each packet */
    UINT32  Reserved:               20;     /* [31:12] */
} B8_PWM_ENC_DELAY_TIME0_REG_s;

typedef struct {
    UINT32  Ch12DelayTime:          12;     /* [11:0] The delay time for channel #12 to send each packet */
    UINT32  Reserved:               20;     /* [31:12] */
} B8_PWM_ENC_DELAY_TIME6_REG_s;

/*-----------------------------------------------------------------------------------*\
 * B6 PWM ENC : Interrupt Status Register
\*-----------------------------------------------------------------------------------*/
typedef struct {
    UINT32  RportFifoNearFull:       2;     /* [1:0] 1-0 */
    UINT32  Reserved0:               10;    /* [11:2] */
    UINT32  RportFifoNearFull12:     1;     /* [12] 12 */
    UINT32  Reserved1:               1;     /* [13] */
    UINT32  LsAwportFifoNearEmpty:   1;     /* [14] */
    UINT32  Hsc0AwportFifoNearEmpty: 1;     /* [15] */
    UINT32  CmdDone:                 2;     /* [17:16] 1-0 */
    UINT32  Reserved2:               10;    /* [27:18] */
    UINT32  CmdDone12:               1;     /* [28] 12 */
    UINT32  Reserved3:               1;     /* [29] */
    UINT32  Reserved4:               1;     /* [30] */
    UINT32  PwmEncDone:              1;     /* [31] */
} B8_PWM_ENC_INT_SATUS_REG_s;

typedef struct {
    UINT32  RportFifoOverflow0:      1;     /* [0] 0 */
    UINT32  Reserved0:               11;    /* [11:1] Reserved */
    UINT32  RportFifoOverflow12:     1;     /* [12] 12 */
    UINT32  Reserved1:               1;     /* [13] Reserved */
    UINT32  LsAwportFifoOverflow:    1;     /* [14] */
    UINT32  Hs0AwportFifoOverflow:   1;     /* [15] */
    UINT32  ReplayFail0:             1;     /* [16] 0 */
    UINT32  Reserved2:               11;    /* [27:17] Reserved */
    UINT32  ReplayFail12:            1;     /* [28] 12 */
    UINT32  Reserved3:               1;     /* [29] Reserved */
    UINT32  DataLtLength:            1;     /* [30] */
    UINT32  DataGtLength:            1;     /* [31] */
} B8_PWM_ENC_ERROR_INT_SATUS0_REG_s;

typedef struct {
    UINT32  SeqIDError0:             1;     /* [0] 0 */
    UINT32  Reserved0:               11;    /* [11:1] Reserved */
    UINT32  SeqIDError12:            1;     /* [12] 12 */
    UINT32  Reserved1:               3;     /* [15:13] Reserved */
    UINT32  NoisyConn0:              1;     /* [16] 0 */
    UINT32  Reserved2:               11;    /* [27:17] Reserved */
    UINT32  NoisyConn12:             1;     /* [28] 12 */
    UINT32  Reserved3:               1;     /* [29] Reserved */
    UINT32  DepackerAfifoOverflow:   1;     /* [30] This bit is available in A9A only */
    UINT32  CidError:                1;     /* [31] */
} B8_PWM_ENC_ERROR_INT_SATUS1_REG_s;

typedef struct {
    UINT32  LSC0DisableAck:         1;      /* [0]*/
    UINT32  Reserved0:              11;     /* [11:1] */
    UINT32  HSC0DisableAck:         1;      /* [12] */
    UINT32  Reserved1:              19;     /* [31:13]  */
} B8_PWM_ENC_CTRL1_REG_s;

/*-----------------------------------------------------------------------------------*\
 * B6 PWM ENC : Channel Status Register
\*-----------------------------------------------------------------------------------*/
typedef struct {
    UINT32  LastCommand:            15;     /* [14:0] The last executed command */
    UINT32  LastCommandAddr:        17;     /* [31:15] The address of the last executed command */
} B8_PWM_ENC_CHANNEL_CMD_STATUS_REG_s;

typedef struct {
    UINT32  TotSentPacketCount:      24;    /* [23:0] Total number of packets */
    UINT32  TotNakPacketCount:       7;     /* [30:24] Total number of NAK packets received */
    UINT32  QualityCounterEnable:    1;     /* [31] 1 = Enable packet counter */
} B8_PWM_ENC_CHANNEL_QUALITY_REG_s;

/*-----------------------------------------------------------------------------------*\
 * B6 PWM ENC : Timer Threshold Register
\*-----------------------------------------------------------------------------------*/
typedef struct {
    UINT32  Ch0TimeOut:             16;     /* [15:0] Timeout threshold of channel 0 */
    UINT32  Ch1TimeOut:             16;     /* [31:16] Timeout threshold of channel 1 */
} B8_PWM_ENC_TIMER_THRESHOLD0_REG_s;

typedef struct {
    UINT32  Ch12TimeOut:            16;     /* [15:0] Timeout threshold of channel 12 */
    UINT32  Reserved:               16;     /* [31:16] Reserved */
} B8_PWM_ENC_TIMER_THRESHOLD6_REG_s;

/*-----------------------------------------------------------------------------------------------*\
 * B6 PWM ENC : All Registers
\*-----------------------------------------------------------------------------------------------*/
typedef struct {
    volatile B8_PWM_ENC_CTRL_REG_s                  Ctrl;                /* 0x00(RW): Encoder Control */
    volatile B8_PWM_ENC_CREDIT_CFG0_REG_s           CreditCfg0;          /* 0x04(RW): Credit Config 0 */
    volatile B8_PWM_ENC_RESET_CTRL0_REG_s           ResetCtrl0;          /* 0x08(RWC): Reset Control 0 */
    volatile B8_PWM_ENC_AWPORT_FIFO_CFG_REG_s       AwportFifoCfg;       /* 0x0C(RW): AW Port FIFO Config */
    volatile B8_PWM_ENC_ADDR_CMD_PORT_REG_s         AddrCmdPort;         /* 0x10(WO): Address and Command Port */
    volatile UINT32                                 WriteDataPort;       /* 0x14(WO): Write Data Port */
    volatile UINT32                                 ReadDataPort0;       /* 0x18(RO): Read Data Port for Channel 0 */
    volatile UINT32                                 Reserved0[11];       /* 0x1C-0x44(RO) */
    volatile UINT32                                 ReadDataPort12;      /* 0x48(RW): Read Data Port for Channel 12 */
    volatile UINT32                                 Reserved1;           /* 0x4C */
    volatile B8_PWM_ENC_RPORT_FIFO_STATUS0_REG_s    ReadPortStatus0;     /* 0x50(RW): Read Port FIFO Status 0 */
    volatile UINT32                                 Reserved2[5];        /* 0x54-64 */
    volatile B8_PWM_ENC_RPORT_FIFO_STATUS6_REG_s    ReadPortStatus6;     /* 0x68(RO): Read Port FIFO Status 6 */
    volatile B8_PWM_ENC_DELAY_METER_REG_s           DelayMeter0;         /* 0x6C(RW): Delay Meter 0 */
    volatile UINT32                                 Reserved3[6];        /* 0x70-84 */
    volatile B8_PWM_ENC_DELAY_TIME0_REG_s           DelayTime0;          /* 0x88(RW): Delay Time 0 */
    volatile UINT32                                 Reserved4[5];        /* 0x8C-9C */
    volatile B8_PWM_ENC_DELAY_TIME6_REG_s           DelayTime6;          /* 0xA0(RW): Delay Meter 6 */
    volatile B8_PWM_ENC_INT_SATUS_REG_s             NormalIntStatus;     /* 0xA4(RWC): Normal Interrupt Status */
    volatile B8_PWM_ENC_ERROR_INT_SATUS0_REG_s      ErrorIntStatus0;     /* 0xA8(RWC): Error Interrupt Status 0 */
    volatile B8_PWM_ENC_ERROR_INT_SATUS1_REG_s      ErrorIntStatus1;     /* 0xAC(RWC): Error Interrupt Status 1 */
    volatile UINT32                                 NormalIntMask;       /* 0xB0(RW): Normal Interrupt Mask */
    volatile UINT32                                 ErrorIntMask0;       /* 0xB4(RW): Error Interrupt Mask 0 */
    volatile UINT32                                 ErrorIntMask1;       /* 0xB8(RW): Error Interrupt Mask 1 */
    volatile B8_PWM_ENC_TIMER_THRESHOLD0_REG_s      TimeOut0;            /* 0xBC(RW): Timout 0 Values */
    volatile UINT32                                 Reserved5[5];        /* 0xC0-D0 */
    volatile B8_PWM_ENC_TIMER_THRESHOLD6_REG_s      TimeOut6;            /* 0xD4(RW): Timout 6 Values */
    volatile UINT32                                 Reserved6[2];        /* 0xD8-DC */
    volatile B8_PWM_ENC_CREDIT_CFG1_REG_s           CreditCfg1;          /* 0xE0(RW): Credit Config 1 */
    volatile B8_PWM_ENC_RESET_CTRL1_REG_s           ResetCtrl1;          /* 0xE4(RWC): Reset Control 1 */
    volatile B8_PWM_ENC_AWPORT_FIFO_STATUS_REG_s    AwportFifoStatus;    /* 0xE8(RO): AW Port FIFO Status */
    volatile B8_PWM_ENC_CTRL1_REG_s                 Ctrl1;               /* 0xEC(RW): Encoder Control 1 */
    volatile UINT32                                 NormalInt;           /* 0xF0(RO): Normal Interrupt */
    volatile UINT32                                 ErrorlInt0;          /* 0xF4(RO): Error Interrupt 0 */
    volatile UINT32                                 ErrorlInt1;          /* 0xF8(RO): Error Interrupt 1 */
    volatile UINT32                                 Dummy1;              /* 0xFC(RW): Dummy */
    volatile B8_PWM_ENC_CHANNEL_CMD_STATUS_REG_s    Channel0CmdStatus;   /* 0x100(RO): Channl 0 Command Status */
    volatile UINT32                                 Reserved7;           /* 0x104 */
    volatile B8_PWM_ENC_CHANNEL_QUALITY_REG_s       Channel0Quality;     /* 0x108(RWC): Channl 0 Quality Status */
    volatile UINT32                                 Reserved8[3];        /* 0x10C-0x114 */
    volatile B8_PWM_ENC_CHANNEL_QUALITY_REG_s       Channel1Quality;     /* 0x118(RWC): Channl 1 Quality Status */
    volatile UINT32                                 Reserved9[43];       /* 0x11C-0x1C4 */
    volatile B8_PWM_ENC_CHANNEL_QUALITY_REG_s       Channel12Quality;    /* 0x1C8(RWC): Channl 12 Quality Status */
} B8_PWM_ENC_REG_s;

#endif /* AMBA_B8_REG_PWM_ENC_H */
