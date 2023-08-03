/**
 *  @file AmbaReg_Packer.h
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
 *  @details Definitions & Constants for B6 PACKER Control Registers
 *
 */

#ifndef AMBA_B8_REG_PACKER_H
#define AMBA_B8_REG_PACKER_H

/*-----------------------------------------------------------------------------------*\
 * B6 PACKER : Packet Control Register
\*-----------------------------------------------------------------------------------*/
typedef struct {
    UINT32  IntPktEn:               1;      /* [0] Interrupt Packet enable */
    UINT32  PelPktEn:               1;      /* [1] Pixel Data Packet enable */
    UINT32  CtlPktEn:               1;      /* [2] Control Packet enable */
    UINT32  Reserved0:              5;      /* [7:3] */
    UINT32  IntPriority:            1;      /* [8] Interrupt Packet Priority */
    UINT32  DisEarlyTerminate:      1;      /* [9] 0: Enable early termination of PEL packet when pel_buf is empty */
    UINT32  Reserved1:              2;      /* [11:10] */
    UINT32  PacketCntEn:            1;      /* [12] Packet Counter enable */
    UINT32  Reserved2:              3;      /* [15:13] */
    UINT32  DelayIntPkt:            2;      /* [17:16] Delay Interrupt Status Packet Transmission */
    UINT32  DelayCtlPkt:            2;      /* [19:18] Delay Control Packet Transmission */
    UINT32  DelayAckPkt:            2;      /* [21:20] Delay ACK Packet Transmission */
    UINT32  Reserved3:              2;      /* [23:22] */
    UINT32  AckInterval:            6;      /* [29:24] Minimal Interval between ACK and Control Packets */
    UINT32  Reserved4:              2;      /* [31:30] */
} B8_PACKER_PACKET_CTRL_REG_s;

/*-----------------------------------------------------------------------------------*\
 * B6 PACKER : Pixel Control Register
\*-----------------------------------------------------------------------------------*/
typedef struct {
    UINT32  PelDepth:               2;      /* [1:0] Pixel Depth. 0= 8-bit; 1= 10-bit; 2= 12-bit; 3= 14-bit */
    UINT32  Reserved0:              6;      /* [7:2] */
    UINT32  PelCmpEnable:           1;      /* [8] Compression Data Mode enable */
    UINT32  PelEccEnable:           1;      /* [9] Extra ECC bytes in pixel data packet enable */
    UINT32  Reserved1:              22;     /* [31:10] */
} B8_PACKER_PIXEL_CTRL_REG_s;

/*-----------------------------------------------------------------------------------*\
 * B6 PACKER : Arbiter Control Register
\*-----------------------------------------------------------------------------------*/
typedef struct {
    UINT32  PelBufThd:              7;      /* [6:0] Pixel buffer threshold */
    UINT32  Reserved0:              9;      /* [15:7] */
    UINT32  AckIntervalExp:         3;      /* [18:16] Multiplier of ACK interval */
    UINT32  Reserved1:              9;      /* [27:19] */
    UINT32  SinglePktEn:            1;      /* [28] Single Packet Mode enable */
    UINT32  Reserved2:              3;      /* [31:29] */
} B8_PACKER_ARBITER_CTRL_REG_s;

/*-----------------------------------------------------------------------------------*\
 * B6 PACKER : Packet Length Register
\*-----------------------------------------------------------------------------------*/
typedef struct {
    UINT32  MinBytePkt:             16;     /* [15:0] Minimal byte number in a pixel data packet */
    UINT32  MaxBytePkt:             16;     /* [31:16] Maximal byte number in a pixel data packet */
} B8_PACKER_PACKET_LENGTH_REG_s;

/*-----------------------------------------------------------------------------------*\
 * B6 PACKER : Interrupt Status Packet Period Register
\*-----------------------------------------------------------------------------------*/
typedef struct {
    UINT32  IntPktPeriod:           24;     /* [23:0] Interrupt Status Packet Period (in clk_idsp cycle) */
    UINT32  Reserved0:              8;      /* [31:24] */
} B8_PACKER_INT_PKT_PERIOD_REG_s;

/*-----------------------------------------------------------------------------------*\
 * B6 PACKER : Test Control Register
\*-----------------------------------------------------------------------------------*/
typedef struct {
    UINT32  TestMode:               1;      /* [0] 0: Normal mode, 1: Auto pattern generation mode */
    UINT32  Reserved0:              1;      /* [1] */
    UINT32  StatusSrc:              1;      /* [2] 0: packer_st, 1: pkt_cntk */
    UINT32  Reserved1:              1;      /* [3] */
    UINT32  TestDone:               1;      /* [4] 0: Test pattern generation not done (not start or on-going), 1: Test pattern generation done */
    UINT32  Reserved2:              3;      /* [7:5] */
    UINT32  TestSampleRate:         4;      /* [11:8] Sample generation rate, 2 pixels are generated every (test_sample_rate + 1) cycle */
    UINT32  Reserved3:              4;      /* [15:12] */
    UINT32  TestFrmCnt:             10;     /* [25:16] 0: Unlimited, Non-0: Number of test frame to be generated */
    UINT32  Reserved4:              2;      /* [27:26] */
    UINT32  TestPatType:            1;      /* [28] Test pattern type, 0: (x+y)%256, 1: test_color */
    UINT32  Reserved5:              3;      /* [31:29] */
} B8_PACKER_TEST_CTRL_REG_s;

/*-----------------------------------------------------------------------------------*\
 * B6 PACKER : Test Active Picture Register
\*-----------------------------------------------------------------------------------*/
typedef struct {
    UINT32  TestActiveWidth:        16;     /* [15:0] Active Picture Width = test_act_w + 2, this value shall be an even number */
    UINT32  TestActiveHeight:       16;     /* [31:16] Active Picture Height = test_act_h + 1 */
} B8_PACKER_TEST_ACTIVE_REG_s;

/*-----------------------------------------------------------------------------------*\
 * B6 PACKER : Test Timing Control Register
\*-----------------------------------------------------------------------------------*/
typedef struct {
    UINT32  TestLineCycleCnt:       16;     /* [15:0] Cycle Count per Line (including horizontal blank) in unit of operating clock cycle */
    UINT32  TestPicLineCnt:         16;     /* [31:16] Total Picture Line Count (including vertical blank) */
} B8_PACKER_TEST_TIMING_CTRL_REG_s;

/*-----------------------------------------------------------------------------------*\
 * B6 PACKER : PACKER Interrupt Status Register
\*-----------------------------------------------------------------------------------*/
typedef struct {
    UINT32  AckOverflow:            1;      /* [0] 1: Ack queue overflow */
    UINT32  Reserved:               31;     /* [31:1] */
} B8_PACKER_INT_STATUS_REG_s;

/*-----------------------------------------------------------------------------------------------*\
 * B6 PACKER : All Registers
\*-----------------------------------------------------------------------------------------------*/
typedef struct {
    volatile B8_PACKER_PACKET_CTRL_REG_s        PacketCtrl;       /* 0x00(RW): Packet Control */
    volatile B8_PACKER_PIXEL_CTRL_REG_s         PixelCtrl;        /* 0x04(RW): Pixel Control */
    volatile UINT32                             Reserved0;        /* 0x08 */
    volatile B8_PACKER_ARBITER_CTRL_REG_s       ArbiterCtrl;      /* 0x0C(RW): Arbiter Control */
    volatile B8_PACKER_PACKET_LENGTH_REG_s      PacketLength;     /* 0x10(RW): Packet Length */
    volatile B8_PACKER_INT_PKT_PERIOD_REG_s     IntPktPeriod;     /* 0x14(RW): Interrupt Status Packet Period */
    volatile UINT32                             Reserved1[2];     /* 0x18-0x1C */
    volatile UINT32                             Status;           /* 0x20(RO): Packer Status */
    volatile B8_PACKER_INT_STATUS_REG_s         PktIntStatus;     /* 0x24(RWC): Packer Interrupt Status */
    volatile UINT32                             PktIntMask;       /* 0x28(RW): Packer Interrupt Mask */
    volatile UINT32                             Reserved2;        /* 0x2C */
    volatile UINT32                             IntStatus0;       /* 0x30(RO): Interrupt Status 0 */
    volatile UINT32                             IntStatus1;       /* 0x34(RO): Interrupt Status 1 */
    volatile UINT32                             Reserved3[2];     /* 0x38-0x3C */
    volatile B8_PACKER_TEST_CTRL_REG_s          TestCtrl;         /* 0x40(RW) Test Control */
    volatile B8_PACKER_TEST_ACTIVE_REG_s        TestActivePic;    /* 0x44(RW) Test Active Picture */
    volatile B8_PACKER_TEST_TIMING_CTRL_REG_s   TestTimingCtrl;   /* 0x48(RW) Test Time Control */
    volatile UINT32                             Reserved4[9];     /* 0x4C-0x6C */
    volatile UINT32                             TestPatternValue; /* 0x70(RW): Test pattern value(available when test_pattern_type = 1) */
} B8_PACKER_REG_s;
#endif /* AMBA_B8_REG_PACKER_H */
