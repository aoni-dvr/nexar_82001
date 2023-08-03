/**
 *  @file AmbaReg_VOUT.h
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
 *  @details Definitions & Constants for B8 VOUT Control Registers
 *
 */

#ifndef AMBA_B8_REG_VOUT_H
#define AMBA_B8_REG_VOUT_H

/*-----------------------------------------------------------------------------------*\
 * B8 VOUT : Control Register
\*-----------------------------------------------------------------------------------*/
typedef struct {
    UINT32  FixedFormatSelect:          5;  /* [4:0] Fixed Format Select (Fixed_format_select should be 0) */
    UINT32  InterlaceEnable:            1;  /* [5] Interlace Enable (No interlace mode) */
    UINT32  Yuv422Input:                1;  /* [6] 0 - RGB format or YUV 444 format. 1 - YUV 422 format */
    UINT32  GammaEnable:                1;  /* [7] Enable bit for gamma tablet */
    UINT32  HalfPixelShiftEnable:       1;  /* [8] Enable bit for half pixel shift module */
    UINT32  HalfPixelShiftEvenLines:    1;  /* [9] Apply half-pixel shift to 0 - odd lines, 1 - even lines */
    UINT32  Format3D:                   2;  /* [11:10] 3D Format enable (0: Output is 2D) */
    UINT32  FpdFourthLaneEn:            1;  /* [12] FPD Link 4th lane Enable */
    UINT32  FpdReverseOrder:            1;  /* [13] */
    UINT32  FpdMsbSelectRW:             1;  /* [14] Select one from two FPD modes, depends on panel */
    UINT32  YuvReverse:                 1;  /* [15] 1 - luma & chroma input is in reversed mode */
    UINT32  FpdDualPorts:               1;  /* [16] 0 ¡V1 fpd port, 4 lane output, 1 - 2 fpd ports, 8 lane output */
    UINT32  FpdCtlOnSecondPort:         1;  /* [17] */
    UINT32  Reserved:                   2;  /* [19:18] Reserved */
    UINT32  MipiSyncEndEn:              1;  /* [20] MIPI Sync End Enable */
    UINT32  MipiEotpEn:                 1;  /* [21] MIPI DSI EoTP Enable */
    UINT32  MipiLaneNum:                2;  /* [23:22] Number of MIPI Lanes, 0: 1 lane, 1: 2 lanes , 2: 4 lanes , 3: 8 lanes */
    UINT32  MipiCsiEnable:              1;  /* [24] 1 : CSI mode, 0: DSI mode */
    UINT32  VoutVoutSyncEnable:         1;  /* [25] VOUT-VOUT Sync Enable */
    UINT32  VinVoutSyncEnable:          1;  /* [26] VIN-VOUT Sync Enable */
    UINT32  DigitalOutputEnable:        1;  /* [27] VIN-VOUT Sync Enable */
    UINT32  FixedLineTiming:            1;  /* [28] 0¡VVariable Horizontal Blanking time, 1-Fixed Horizontal blanking same as B6A */
    UINT32  VariableStartPointEn:       1;  /* [29] 0-use active_region_start_h/v, 1-use adjusted_start_h/v */
    UINT32  LatchRegCmd:                1;  /* [30] Mipi dsi command configuration (0: no buffer, 1: buffer) */
    UINT32  SwReset:                    1;  /* [31] Set of 1 resets the Display Section. */
} B8_VOUT_CTRL_REG_s;

typedef struct {
    UINT32  Enable:                 1;      /* [0] Only need to program once for the first frame. Following frames can be generated automatically */
    UINT32  Reserved0:              31;     /* [31:1] Reserved */
} B8_VOUT_FRAME_ENABLE_REG_s;

/*-----------------------------------------------------------------------------------*\
 * B8 VOUT : Status Register
\*-----------------------------------------------------------------------------------*/
typedef struct {
    UINT32  Reserved0:              1;      /* [0] Reserved */
    UINT32  MipiBackgroundOutput:   1;      /* [1] Vout is underflow and start outputting background color */
    UINT32  CsiBackgroundFinish:    1;      /* [2] Csi background for bringing up is finished */
    UINT32  DisplayCmdComplete:     1;      /* [3] */
    UINT32  SfifoCntClkCore:        13;     /* [16:4] For software pll use, tune the clock to try to make the clock vo lock with clk vo in A9A */
    UINT32  AsyncFifoCntClkCore:    9;      /* [25:17] For software pll use, tune the clock to try to make the clock vo lock with clk vo in A9A */
    UINT32  MipiFifoOverflow:       1;      /* [26] */
    UINT32  Reserved1:              2;      /* [28:27] Reserved */
    UINT32  DigitalUnderflow:       1;      /* [29] 1 if an underflow is detected on the digital output */
    UINT32  Reserved2:              1;      /* [30] Reserved */
    UINT32  ResetComplete:          1;      /* [31] This bit is set to 1 when a reset has completed and the section is ready to be reprogrammed */
} B8_VOUT_STATUS_REG_s;

/*-----------------------------------------------------------------------------------*\
 * B8 VOUT : Size/Start/End/Offset Register
\*-----------------------------------------------------------------------------------*/
typedef struct {
    UINT32  FrameHeight:            14;     /* [13:0] Number of clock cycles per line (including blanking). Set to n-1 */
    UINT32  Reserved0:              2;      /* [15:14] Reserved */
    UINT32  FrameWidth:             14;     /* [29:16] Number of lines per frame (including blanking). Set to n-1 */
    UINT32  Reserved1:              2;      /* [31:30] Reserved */
} B8_VOUT_FRAME_SIZE_REG_s;

typedef struct {
    UINT32  StartRow:               14;     /* [13:0] The active region will start on this row */
    UINT32  Reserved0:              2;      /* [15:14] Reserved */
    UINT32  StartColumn:            14;     /* [29:16] The active region will start on this column */
    UINT32  Reserved1:              2;      /* [31:30] Reserved */
} B8_VOUT_ACTIVE_START_REG_s;

typedef struct {
    UINT32  EndRow:                 14;     /* [13:0] The active region will end on this row */
    UINT32  Reserved0:              2;      /* [15:14] Reserved */
    UINT32  EndColumn:              14;     /* [29:16] The active region will end on this column */
    UINT32  Reserved1:              2;      /* [31:30] Reserved */
} B8_VOUT_ACTIVE_END_REG_s;

typedef struct {
    UINT32  EndColumn:              14;     /* [13:0] Hsync end column */
    UINT32  Reserved0:              2;      /* [15:14] Reserved */
    UINT32  StartColumn:            14;     /* [29:16] Hsync start column */
    UINT32  Reserved1:              2;      /* [31:30] Reserved */
} B8_VOUT_HSYNC_CTRL_REG_s;

typedef struct {
    UINT32  Row:                    14;     /* [13:0] Vsync start/end row */
    UINT32  Reserved0:              2;      /* [15:14] Reserved */
    UINT32  Column:                 14;     /* [29:16] Vsync start/end column */
    UINT32  Reserved1:              2;      /* [31:30] Reserved */
} B8_VOUT_VSYNC_CTRL_REG_s;


typedef struct {
    UINT32  AdjStartV:              14;     /* [13:0] When variable_start_point_en is high, vout will start v_cnt at this number */
    UINT32  Reserved0:              2;      /* [15:14] Reserved */
    UINT32  AdjStartH:              14;     /* [29:16] When variable_start_point_en is high, vout will start h_cnt at this number */
    UINT32  Reserved1:              2;      /* [31:30] Reserved */
} B8_VOUT_ADJUSTED_START_REG_s;

/*-----------------------------------------------------------------------------------*\
 * B8 VOUT : Background Register
\*-----------------------------------------------------------------------------------*/
typedef struct {
    UINT32  Cr:                     8;      /* [7:0] Value of Cr component for background color */
    UINT32  Cb:                     8;      /* [15:8] Value of Cb component for background color */
    UINT32  Y:                      8;      /* [23:16] Value of Y component for background color */
    UINT32  Reserved0:              8;      /* [31:24] Reserved */
} B8_VOUT_BACKGROUND_REG_s;

typedef struct {
    UINT32  Enable:                 1;      /* [0] 1: Enable background before frame started for csi mode */
    UINT32  Reserved0:              31;     /* [31:1] Reserved */
} B8_VOUT_CSI_BACKGROUND_REG_s;

/*-----------------------------------------------------------------------------------------------*\
 * B8 VOUT: Display Color Space Converter Parameter Register
\*-----------------------------------------------------------------------------------------------*/
typedef struct {
    UINT32   CoefA0:                 13; /* [12:0] Signed 2.10 bits */
    UINT32   Reserved:               3;  /* [15:13] Reserved */
    UINT32   CoefA1:                 13; /* [28:16] Signed 2.10 bits */
    UINT32   Reserved1:              3;  /* [31:29] Reserved */
} B8_VOUT_CSC_PARAM_0_REG_s;

typedef struct {
    UINT32   CoefA2:                 13; /* [12:0] Signed 2.10 bits */
    UINT32   Reserved:               3;  /* [15:13] Reserved */
    UINT32   CoefA3:                 13; /* [28:16] Signed 2.10 bits */
    UINT32   Reserved1:              3;  /* [31:29] Reserved */
} B8_VOUT_CSC_PARAM_1_REG_s;

typedef struct {
    UINT32   CoefA4:                 13; /* [12:0] Signed 2.10 bits */
    UINT32   Reserved:               3;  /* [15:13] Reserved */
    UINT32   CoefA5:                 13; /* [28:16] Signed 2.10 bits */
    UINT32   Reserved1:              3;  /* [31:29] Reserved */
} B8_VOUT_CSC_PARAM_2_REG_s;

typedef struct {
    UINT32   CoefA6:                 13; /* [12:0] Signed 2.10 bits */
    UINT32   Reserved:               3;  /* [15:13] Reserved */
    UINT32   CoefA7:                 13; /* [28:16] Signed 2.10 bits */
    UINT32   Reserved1:              3;  /* [31:29] Reserved */
} B8_VOUT_CSC_PARAM_3_REG_s;

typedef struct {
    UINT32   CoefA8:                 13; /* [12:0] Signed 2.10 bits */
    UINT32   Reserved:               3;  /* [15:13] Reserved */
    UINT32   ConstB0:                15; /* [30:16] Signed 10 bits */
    UINT32   Reserved1:              1;  /* [31] Reserved */
} B8_VOUT_CSC_PARAM_4_REG_s;

typedef struct {
    UINT32   ConstB1:                15; /* [14:0] Signed 10 bits */
    UINT32   Reserved:               1;  /* [15] Reserved */
    UINT32   ConstB2:                15; /* [30:16] Signed 10 bits */
    UINT32   Reserved1:              1;  /* [31] Reserved */
} B8_VOUT_CSC_PARAM_5_REG_s;

typedef struct {
    UINT32  Output0ClampLow:        12; /* [11:0] The minimum allowed value for color component 0 */
    UINT32  Reserved:               4;  /* [15:12] Reserved */
    UINT32  Output0ClampHigh:       12; /* [27:16] The maxiumum allowed value for color component 0 */
    UINT32  Reserved1:              4;  /* [31:28] Reserved */
} B8_VOUT_CSC_PARAM_6_REG_s;

typedef struct {
    UINT32  Output1ClampLow:        12; /* [11:0] The minimum allowed value for color component 1 */
    UINT32  Reserved:               4;  /* [15:12] Reserved */
    UINT32  Output1ClampHigh:       12; /* [27:16] The maxiumum allowed value for color component 1 */
    UINT32  Reserved1:              4;  /* [31:28] Reserved */
} B8_VOUT_CSC_PARAM_7_REG_s;

typedef struct {
    UINT32  Output2ClampLow:        12; /* [11:0] The minimum allowed value for color component 2 */
    UINT32  Reserved:               4;  /* [15:12] Reserved */
    UINT32  Output2ClampHigh:       12; /* [27:16] The maxiumum allowed value for color component 2 */
    UINT32  Reserved1:              4;  /* [31:28] Reserved */
} B8_VOUT_CSC_PARAM_8_REG_s;

/*-----------------------------------------------------------------------------------*\
 * B8 VOUT : Dither Register
\*-----------------------------------------------------------------------------------*/
typedef struct {
    UINT32  Enable:                 1;      /* [0] 0: Dither disabled (round only), 1: Dither enabled */
    UINT32  Format:                 2;      /* [2:1] 0: Dither term is 0.4, 1: Dither term is s+0.4,2: Dither term is s+1.4, 3: Dither term is s+2.4 */
    UINT32  Reserved0:              29;     /* [31:3] Reserved */
} B8_VOUT_DITHER_REG_s;

/*-----------------------------------------------------------------------------------*\
 * B8 VOUT : MIPI DSI/CSI Register
\*-----------------------------------------------------------------------------------*/
typedef struct {
    UINT32  DsiHbpWc:               14;     /* [13:0] Number of bytes in the payload of hbp blanking packet */
    UINT32  DsiHbpUseBlank:         1;      /* [14] At back porch during active lines, 1: Use blanking packet, 2: Do not use blanking, go low power */
    UINT32  Reserved0:              1;      /* [15] Reserved */
    UINT32  DsiSyncWc:              14;     /* [29:16] Number of bytes in the payload of sync blanking packet */
    UINT32  DsiSyncUseBlank:        1;      /* [30] At sync position(between sync start and sync end, ignored when send_sync == 0), 1: Use blanking packet, 0: Do not use blanking, go low power */
    UINT32  Reserved1:              1;      /* [31] Reserved */
} B8_VOUT_HBP_SYNC_REG_s;

typedef struct {
    UINT32  DsiHfpWc:               14;     /* [13:0] Number of bytes in the payload of hfp blanking packet */
    UINT32  DsiHfpUseBlank:         1;      /* [14] At front porch during active lines: 1: Use blanking packet, 2: Do not use blanking, go low power */
    UINT32  Reserved0:              1;      /* [15] Reserved */
    UINT32  DsiVblankWc:            14;     /* [29:16] Number of bytes in the payload of vblank blanking packet */
    UINT32  DsiVblankUseBlank:      1;      /* [30] After sync events at vblank lines: 1: Use blanking packet, 0: Do not use blanking, go low power */
    UINT32  Reserved1:              1;      /* [31] Reserved */
} B8_VOUT_HFP_VB_REG_s;

typedef struct {
    UINT32  TimingN:                16;     /* [15:0] When commang_timing == 1, this number represents at how many lines after vsync, the command will be sent */
    UINT32  Timing:                 2;      /* [17:16] 0: Send the command at next available time, 1: Send during Vblank, n lines after vsync start (n[15:0])*/
    UINT32  Type:                   2;      /* [19:18] 0: Short packet command, 1: Norma long packet command, 2: DCS long packet command */
    UINT32  Mode:                   1;      /* [20] 0: Send command with low power mode, 1: Send command with high speed mode */
    UINT32  Reserved0:              11;     /* [31:21] Reserved */
} B8_VOUT_MIPI_COMMAND_CTRL_REG_s;

typedef struct {
    UINT32  Byte_0_4_8_12:          8;      /* [7:0] First Byte of MIPI Long Command */
    UINT32  Byte_1_5_9_13:          8;      /* [15:8] Second Byte of MIPI Long Command */
    UINT32  Byte_2_6_10_14:         8;      /* [23:16] Third Byte of MIPI Long Command */
    UINT32  Byte_3_7_11_15:         8;      /* [31:24] Fourth Byte of MIPI Long Command */
} B8_VOUT_MIPI_COMMAND_BYTE_REG_s;

typedef struct {
    UINT32  DsiCommandHeader1:      8;      /* [7:0] For short DCS command: parameters, For short other command: parameter 1, For long DCS command: wc_1 */
    UINT32  DsiCommandHeader0:      8;      /* [15:8] For short DCS command: command type hex, For short other command: parameter 0, For long DCS command: wc_0 */
    UINT32  DsiCommandDi:           6;      /* [21:16] The Data Identifier of the sending command */
    UINT32  Reserved0:              2;      /* [23:22] Reserved */
    UINT32  DsDcsLongType:          8;      /* [31:24] When sending DCS long command, this is the first byte(command type) in payload */
} B8_VOUT_MIPI_COMMAND_PARAM_REG_s;

/*-----------------------------------------------------------------------------------*\
 * B8 VOUT : SFIFO Control Register
\*-----------------------------------------------------------------------------------*/
typedef struct {
    UINT32  InSfifoLowWater:        13;     /* [12:0] Input sync fifo can start output input data to downstream when the fifo cnt is larger than this value */
    UINT32  Reserved0:              3;      /* [15:13] Reserved */
    UINT32  InSfifoHighWater:       13;     /* [28:16] Input sync fifo can start output input data to downstream when the fifo cnt is larger than this value */
    UINT32  InSfifoRefresh:         1;      /* [29] 0: The sfifo_high/low_water only take effect on the 1st frame after reset, 1: Every frame, the sfifo would wait
                                                       the counter reaches the threshold defined by sfifo_high/low water and then start the downstream */
    UINT32  Reserved1:              2;      /* [31:30] Reserved */
} B8_VOUT_SFIFO_CTRL_REG_s;

/*-----------------------------------------------------------------------------------*\
 * B8 VOUT : Output Mode Register
\*-----------------------------------------------------------------------------------*/
typedef struct {
    UINT32  HsyncPoLarity:               1;    /* [0] 0: HSync asserted low, 1: HSync asserted high */
    UINT32  VsyncPoLarity:               1;    /* [1] 0: VSync asserted low, 1: VSync asserted high */
    UINT32  ClockOutputDivider:          1;    /* [2] 0: Output clock same as CLK_VO_B input, 1: Output clock same as internally divided clock */
    UINT32  ClockDividerEnable:          1;    /* [3] 0: Internal clock divider disabled, 1: Internal clock divider enabled */
    UINT32  ClockEdgeSelect:             1;    /* [4] 0: Digital output values valid on rising edge of output clock, 1: Digital output values valid on falling edge of output clock */
    UINT32  ClockDisable:                1;    /* [5] 0: Normal with digital clock output enabled if VOUTD_control[27] is asserted
                                                      1: Disables the digital clock output to reduce EMI regardless of VOUTD_control[27] */
    UINT32  ClockDividerPatternWidth:    7;    /* [12:6] Set to n-1 where n is the number of valid bits in the clock pattern registers */
    UINT32  MipiLineTiming:              1;    /* [13] 0: Disable line sync packets, 1: Enable line sync packets (set to 1 in DSI) */
    UINT32  MipiLineCount:               1;    /* [14] 0: Disable MIPI line count, 1: Enable MIPI line count (always to 0 in DSI) */
    UINT32  MipiFrameCount:              1;    /* [15] 0: Disable MIPI frame count, 1: Enable MIPI frame count (always to 0 in DSI) */
    UINT32  MipiSendBlankLines:          1;    /* [16] 0: Disable MIPI output packet in Vblank, 1: Enable MIPI output packet in Vblank (always set to 0 in DSI mode) */
    UINT32  MipiLineTimingAll:           1;    /* [17] 0: Disable line sync start and end in Vblank, 1: Enable line sync start and end in Vblank (always set to 1 in DSI mode) */
    UINT32  MipiEccOrderReserved1:       1;    /* [18] Please set to 0 */
    UINT32  HvldPoLarity:                1;    /* [19] 0: VD1_HVLD asserted low, 1: VD1_HVLD asserted high */
    UINT32  Reserved0:                   1;    /* [20] Reserved */
    UINT32  ColorSeqEvenLines:           3;    /* [23:21] Color Sequence Even Lines */
    UINT32  ColorSeqOddLines:            3;    /* [26:24] Color Sequence Odd Lines */
    UINT32  OutputMode:                  4;    /* [30:27] Digital Output Mode */
    UINT32  Reserved1:                   1;    /* [31] Reserved */
} B8_VOUT_MODE_REG_s;

/*-----------------------------------------------------------------------------------------------*\
 * B8 VOUT : All Registers
\*-----------------------------------------------------------------------------------------------*/
typedef struct {
    volatile B8_VOUT_CTRL_REG_s                 Ctrl;               /* 0x00(RW): Display section control */
    volatile B8_VOUT_STATUS_REG_s               Status;             /* 0x04(RWC):Status register */
    volatile B8_VOUT_FRAME_SIZE_REG_s           FrameSize;          /* 0x08(RW): Defines HV Region for all outputs */
    volatile B8_VOUT_ACTIVE_START_REG_s         ActiveRegionStart;  /* 0x0C(RW): Start of active region in top field */
    volatile B8_VOUT_ACTIVE_END_REG_s           ActiveRegionEnd;    /* 0x10(RW): End of active region in top field */
    volatile B8_VOUT_BACKGROUND_REG_s           BackGround;         /* 0x14(W) : Color to display if enable deadline missed */
    volatile B8_VOUT_MODE_REG_s                 OutputMode;         /* 0x18(RW): Controls Digital Output Options */
    volatile B8_VOUT_HSYNC_CTRL_REG_s           HsyncCtrl;          /* 0x1C(RW): Controls Hsync for digital output */
    volatile B8_VOUT_VSYNC_CTRL_REG_s           VsyncStart;         /* 0x20(RW): Defines Start of Vsync in top field */
    volatile B8_VOUT_VSYNC_CTRL_REG_s           VsyncEnd;           /* 0x24(RW): Defines End of Vsync in top field */
    volatile B8_VOUT_DITHER_REG_s               DitherSetting;      /* 0x28(RW): Dithering settings for digital output */
    volatile UINT32                             DitherSeed;         /* 0x2c(RW): Dithering Seed */
    volatile UINT32                             FpdSpecialControl;  /* 0x30(RW): Some special features in FPD mode */
    volatile B8_VOUT_HBP_SYNC_REG_s             HbpSync;            /* 0x34(RW): Defines HBP and Hsync blanking */
    volatile B8_VOUT_HFP_VB_REG_s               MipiHfpVb;          /* 0x38(RW): Defines HFP and vertical blanking */
    volatile UINT32                             MipiPhyCtlStatus0;  /* 0x3c(RW) */
    volatile UINT32                             MipiPhyCtlStatus1;  /* 0x40(RW) */
    volatile B8_VOUT_ADJUSTED_START_REG_s       AdjustedStart;      /* 0x44(RW): Adjust VOUT internal cntrs start counting position to allow VOUT catch up with input stream */
    volatile B8_VOUT_SFIFO_CTRL_REG_s           InputSyncFifoCtrl;  /* 0x48(RW): Control when to start output the input to downstream through input sync fifo */
    volatile B8_VOUT_CSI_BACKGROUND_REG_s       CsiBackGroundEn;    /* 0x4c(RW): Enable background generation for mipi csi mode */
    volatile B8_VOUT_CSC_PARAM_0_REG_s          CscParam0;          /* 0x50(RW) */
    volatile B8_VOUT_CSC_PARAM_1_REG_s          CscParam1;          /* 0x54(RW) */
    volatile B8_VOUT_CSC_PARAM_2_REG_s          CscParam2;          /* 0x58(RW) */
    volatile B8_VOUT_CSC_PARAM_3_REG_s          CscParam3;          /* 0x5C(RW) */
    volatile B8_VOUT_CSC_PARAM_4_REG_s          CscParam4;          /* 0x60(RW) */
    volatile B8_VOUT_CSC_PARAM_5_REG_s          CscParam5;          /* 0x64(RW) */
    volatile B8_VOUT_CSC_PARAM_6_REG_s          CscParam6;          /* 0x68(RW) */
    volatile B8_VOUT_CSC_PARAM_7_REG_s          CscParam7;          /* 0x6C(RW) */
    volatile B8_VOUT_CSC_PARAM_8_REG_s          CscParam8;          /* 0x70(RW) */
    volatile B8_VOUT_FRAME_ENABLE_REG_s         FrameEnable;        /* 0x74(RW) Only need to program once for the first frame */
    volatile UINT32                             Reserved1[2];       /* 0x78-0x7c */
    volatile B8_VOUT_MIPI_COMMAND_PARAM_REG_s   MipiCmdParam;       /* 0x80(RW): Defines MIPI command header and data type */
    volatile B8_VOUT_MIPI_COMMAND_BYTE_REG_s    MipiCmdLongWord0;   /* 0x84(RW): First word of long MIPI command */
    volatile B8_VOUT_MIPI_COMMAND_BYTE_REG_s    MipiCmdLongWord1;   /* 0x88(RW): Second word of long MIPI command */
    volatile B8_VOUT_MIPI_COMMAND_BYTE_REG_s    MipiCmdLongWord2;   /* 0x8c(RW): Third word of long MIPI command */
    volatile B8_VOUT_MIPI_COMMAND_BYTE_REG_s    MipiCmdLongWord3;   /* 0x90(RW): Fourth word of long MIPI command */
    volatile B8_VOUT_MIPI_COMMAND_CTRL_REG_s    MipiCmdCtrl;        /* 0x94(RW): Defines MIPI command type and mode */
    volatile UINT32                             Reserved2[26];      /* 0x98-0xfc */
    volatile UINT32                             GammaValue[256];    /* 0x100-0x4fc(RW): Gamma table programming */
} B8_VOUT_REG_s;

#endif /* AMBA_B8_REG_VOUT_H */
