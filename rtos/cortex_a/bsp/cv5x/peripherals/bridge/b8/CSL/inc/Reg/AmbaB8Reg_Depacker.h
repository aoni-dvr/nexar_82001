/**
 *  @file AmbaReg_Depacker.h
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
 *  @details Definitions & Constants for B6 Depacker Control Registers
 *
 */

#ifndef AMBA_B8_REG_DEPACKER_H
#define AMBA_B8_REG_DEPACKER_H

/*-----------------------------------------------------------------------------------*\
 * B6 Depacker : Depacker Control Register
\*-----------------------------------------------------------------------------------*/
typedef struct {
    UINT32  Enable:                 1;      /* [0] De-packer enable */
    UINT32  N2bEnable:              1;      /* [1] Set to 1 if N-to-B and cfa_codec are both enabled */
    UINT32  Reserved0:              30;     /* [31:2] */
} B8_DEPACKER_CTRL_REG_s;

/*-----------------------------------------------------------------------------------*\
 * B6 Depacker : Pixel Control Register
\*-----------------------------------------------------------------------------------*/
typedef struct {
    UINT32  PelMaxLength:           16;     /* [15:0] pel_max_length: default value is 2048 */
    UINT32  Reserved0:              16;     /* [31:16] */
} B8_DEPACKER_PEL_CTRL_REG_s;

/*-----------------------------------------------------------------------------------*\
 * B6 Depacker : Test Control Register
\*-----------------------------------------------------------------------------------*/
typedef struct {
    UINT32  TestMode:               2;      /* [1:0] Test mode */
    UINT32  Reserved0:              6;      /* [7:2] */
    UINT32  TestSampleRate:         4;      /* [11:8] Sample generation rate */
    UINT32  Reserved1:              4;      /* [15:12] */
    UINT32  TestFrmCnt:             10;     /* [25:16] Test Frame Count */
    UINT32  Reserved2:              2;      /* [27:26] */
    UINT32  TestPatType:            1;      /* [28] Test pattern type, 0: (x+y)%256, 1: test_color */
    UINT32  Reserved3:              3;      /* [31:29] */
} B8_DEPACKER_TEST_CTRL_REG_s;

/*-----------------------------------------------------------------------------------*\
 * B6 Depacker : Test Active Picture Register
\*-----------------------------------------------------------------------------------*/
typedef struct {
    UINT32  TestActiveWidth:        16;     /* [15:0] Test Pattern Active Picture Width */
    UINT32  TestActiveHeight:       16;     /* [31:16] Test Pattern Active Picture Height */
} B8_DEPACKER_TEST_ACTIVE_REG_s;

/*-----------------------------------------------------------------------------------*\
 * B6 PACKER : Test Timing Control Register
\*-----------------------------------------------------------------------------------*/
typedef struct {
    UINT32  TestLineCycleCnt:       16;     /* [15:0] Cycle Count per Line (including horizontal blank) in unit of operating clock cycle */
    UINT32  TestPicLineCnt:         16;     /* [31:16] Total Picture Line Count (including vertical blank) */
} B8_DEPACKER_TEST_TIMING_CTRL_REG_s;

/*-----------------------------------------------------------------------------------*\
 * B6 Depacker : Test Status Register
\*-----------------------------------------------------------------------------------*/
typedef struct {
    UINT32  TestStatus:             1;      /* [0] Pattern check status: asserted as check fail */
    UINT32  Reserved0:              3;      /* [3:1] */
    UINT32  TestDone:               1;      /* [4] 0: Test pattern generation/comparison not done, 1: Test pattern generation/comparison done */
    UINT32  Reserved1:              27;     /* [31:5] */
} B8_DEPACKER_TEST_STATUS_REG_s;

/*-----------------------------------------------------------------------------------*\
 * B6 Depacker : Statistics Control Register
\*-----------------------------------------------------------------------------------*/
typedef struct {
    UINT32  StatPktEn:              1;      /* [0] Packet Statistics enable */
    UINT32  WrapPassPkt:            1;      /* [1] No stop packet counter when stat_pass_pkt is 0xffffffff, stat_pass_pkt would wrap around */
    UINT32  Reserved0:              2;      /* [3:2] */
    UINT32  StatPelEn:              1;      /* [4] Pixel Statistics enable */
    UINT32  Reserved1:              27;     /* [31:5] */
} B8_DEPACKER_STAT_CTRL_REG_s;

/*-----------------------------------------------------------------------------------*\
 * B6 Depacker : Statistics Count Register
\*-----------------------------------------------------------------------------------*/
typedef struct {
    UINT32  PacketCount:            24;     /* [23:0] Packet Count */
    UINT32  Reserved0:              8;      /* [31:24] */
} B8_DEPACKER_STAT_COUNT_REG_s;

/*-----------------------------------------------------------------------------------*\
 * B6 Depacker : Sw Pll Register
\*-----------------------------------------------------------------------------------*/
typedef struct {
    UINT32  Enable:                 1;      /* [0] Sw Pll Enable */
    UINT32  CountingThreshold:      20;     /* [20:1] Sw Pll Counting Thresold */
    UINT32  Reserved0:              11;     /* [31:21] */
} B8_DEPACKER_SW_PLL_CTRL_REG_s;

/*-----------------------------------------------------------------------------------*\
 * B6 Depacker : Sw Pll Count Register
\*-----------------------------------------------------------------------------------*/
typedef struct {
    UINT32  Count:                  20;     /* [19:0] */
    UINT32  Reserved:               12;     /* [31:20] */
} B8_DEPACKER_SW_PLL_COUNT_REG_s;

/*-----------------------------------------------------------------------------------------------*\
 * B6 Depacker : All Registers
\*-----------------------------------------------------------------------------------------------*/
typedef struct {
    volatile B8_DEPACKER_CTRL_REG_s                 Ctrl;             /* 0x00(RW): Depacker Control */
    volatile UINT32                                 Reserved0[3];     /* 0x04-0x0C */
    volatile UINT32                                 IntStatus;        /* 0x10(RWC): Interrupt status of Depacker */
    volatile UINT32                                 Reserved1;        /* 0x14 */
    volatile UINT32                                 Status;           /* 0x18(RO): Debug register */
    volatile B8_DEPACKER_PEL_CTRL_REG_s             PixelCtrl;        /* 0x1C(RW): Pixel Control */
    volatile B8_DEPACKER_SW_PLL_CTRL_REG_s          SwPllCtrl;        /* 0x20(RW): Sw Pll Control */
    volatile UINT32                                 Reserved2;        /* 0x24 */
    volatile B8_DEPACKER_SW_PLL_COUNT_REG_s         SwPllCountNear;   /* 0x28(RO): Sw Pll Near Count */
    volatile B8_DEPACKER_SW_PLL_COUNT_REG_s         SwPllCountFar;    /* 0x2C(RO): Sw Pll Far Count */
    volatile UINT32                                 IntFar0;          /* 0x30(RWC): Interrupt status from corresponding Packer */
    volatile UINT32                                 IntFar1;          /* 0x34(RWC): Interrupt status from corresponding Packer */
    volatile UINT32                                 MaskIntFar0;      /* 0x38(RW): Interrupt mask for the interrupt status from corresponding Packer */
    volatile UINT32                                 MaskIntFar1;      /* 0x3C(RW): Interrupt mask for the interrupt status from corresponding Packer */
    volatile B8_DEPACKER_TEST_CTRL_REG_s            TestCtrl;         /* 0x40(RW): Test Control */
    volatile B8_DEPACKER_TEST_ACTIVE_REG_s          TestActivePic;    /* 0x44(RW): Test Active Picture */
    volatile B8_DEPACKER_TEST_TIMING_CTRL_REG_s     TestTimingCtrl;   /* 0x48(RW): Test Time Control */
    volatile B8_DEPACKER_TEST_STATUS_REG_s          TestStatus;       /* 0x4C(RO): Test Status */
    volatile B8_DEPACKER_STAT_CTRL_REG_s            StatCtrl;         /* 0x50(RW): Statistics Control */
    volatile UINT32                                 StatPassPkt;      /* 0x54(RO): Statistics Pass Packet */
    volatile B8_DEPACKER_STAT_COUNT_REG_s           StatErrPkt;       /* 0x58(RO): Statistics Error Packet */
    volatile B8_DEPACKER_STAT_COUNT_REG_s           StatCorrectPkt;   /* 0x5C(RO): Statistics Correct Packet */
    volatile UINT32                                 StatErrPel;       /* 0x60(RO): Statistics Error Pixel Count */
    volatile UINT32                                 Reserved3[3];     /* 0x64-6C */
    volatile UINT32                                 TestPatternValue; /* 0x70(RW): Test pattern value(available when test_pattern_type = 1) */
} B8_DEPACKER_REG_s;

#endif /* AMBA_B8_REG_DEPACKER_H */
