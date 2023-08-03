/**
 *  @file AmbaReg_Codec.h
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
 *  @details Definitions & Constants for B8 Codec Control Registers
 *
 */

#ifndef AMBA_B8_REG_CODEC_H
#define AMBA_B8_REG_CODEC_H

/*-----------------------------------------------------------------------------------*\
 * B8 Codec : Control Register
\*-----------------------------------------------------------------------------------*/
typedef struct {
    UINT32  Enable:                 1;      /* [0] 0: Disable, 1: Enable */
    UINT32  EncodeMode:             1;      /* [1] 0: post-processor+raw-decompressor, 1: pre-processor+raw-compressor */
    UINT32  LegacyCmp:              1;      /* [2] 0: B8 compression, 1: Legacy compression or No compression */
    UINT32  Reserved0:              1;      /* [3] Reserved */
    UINT32  SwReset:                1;      /* [4] 0: Normal operation, 1: Software Reset(shall be used only when code_en=0) */
    UINT32  Reserved1:              1;      /* [5] Reserved */
    UINT32  LegacyDecmpProtocol:    1;      /* [6] 0: Modified IDSP protocol, 1: IDSP protocol */
    UINT32  ClkDisEnc:              1;      /* [7] 0: Enable clock, 1: Disable operating clock for encode path */
    UINT32  ClkDisDec:              1;      /* [8] 0: Enable clock, 1: Disable operating clock for decode path */
    UINT32  Stream01EnDec:          1;      /* [9] 0: Disable, 1: Enable (Decoder Stream 0&1 Enable, for Error Info. FIFO) */
    UINT32  Stream23EnDec:          1;      /* [10] 0: Disable, 1: Enable (Decoder Stream 2&3 Enable, for Error Info. FIFO) */
    UINT32  Reserved2:              21;     /* [31:11] Reserved */
} B8_CODEC_CTRL_REG_s;

/*-----------------------------------------------------------------------------------*\
 * B8 Codec : Sensor Control Register
 \*-----------------------------------------------------------------------------------*/
typedef struct {
    UINT32  Bayer:                  2;      /* [1:0] Color filter array (0~3) */
    UINT32  Reserved0:              2;      /* [3:2] Reserved */
    UINT32  NumSensorMinus1:        1;      /* [4] Number of sensor */
    UINT32  Reserved1:              3;      /* [7:5] Reserved */
    UINT32  NumExpMinus1:           2;      /* [9:8] Number of pictures from a sensor with different exposures*/
    UINT32  Reserved2:              6;      /* [15:10] Reserved */
    UINT32  SplitHorizontal:        1;      /* [16] 0: Data coded as one picture, 1: Data coded as two side-by-side pictures */
    UINT32  Reserved3:              3;      /* [19:17] Reserved */
    UINT32  InputBitMinus8:         4;      /* [23:20] 0: 8-bit, ..., 8: 16-bit (only 8-bit ~ 14-bit allowd in B8) */
    UINT32  NumFarSensorMinus1:     1;      /* [24] 0: One sensor, 1: Two sensors */
    UINT32  Reserved4:              3;      /* [27:25] Reserved */
    UINT32  RightPicture:           2;      /* [29:28] 1: Right picture, 1: Left/Independent picture(bit 28: VIN0, bit 29: VIN1) */
    UINT32  Reserved5:              2;      /* [31:30] Reserved */
} B8_CODEC_SENSOR_CTRL_REG_s;

/*-----------------------------------------------------------------------------------*\
 * B8 Codec : Picture size Register
\*-----------------------------------------------------------------------------------*/
typedef struct {
    UINT32  WidthDiv4Minus1:        12;      /* [11:0] Picture width of one exposure picture (Max. 4095) */
    UINT32  Reserved0:              4;       /* [15:12] Reserved */
    UINT32  HeightDiv2Minus1:       13;      /* [28:16] Total line number to be processed (including necessary dummy data) */
    UINT32  Reserved1:              3;       /* [31:29] Reserved */
} B8_CODEC_PIC_SIZE_REG_s;

/*-----------------------------------------------------------------------------------*\
 * B8 Codec : Set number Register
\*-----------------------------------------------------------------------------------*/
typedef struct {
    UINT32  SetNum:                 1;       /* [0] Register set number of next frame */
    UINT32  AutoUpdateDis:          1;       /* [1] */
    UINT32  Reserved0:              30;      /* [31:2] Reserved */
} B8_CODEC_SET_NUM_REG_s;

/*-----------------------------------------------------------------------------------*\
 * B8 Codec : Debug select Register
\*-----------------------------------------------------------------------------------*/
typedef struct {
    UINT32  DebugSel:               6;       /* [5:0] Debug status selection */
    UINT32  Reserved0:              2;       /* [7:6] */
    UINT32  InstSel:                4;       /* [11:8] Instance selection for duplicated SRAM */
    UINT32  Reserved1:              20;      /* [31:12] */
} B8_CODEC_DBG_SEL_REG_s;

/*-----------------------------------------------------------------------------------*\
 * B8 Codec : Bit stream interleave Register
\*-----------------------------------------------------------------------------------*/
typedef struct {
    UINT32  SegLengthMinus1:        9;       /* [8:0] Bit stream segment length (15~511) */
    UINT32  Reserved0:              7;       /* [15:9] */
    UINT32  BsPackerSel:            4;       /* [19:16] Packer selection for bit streams */
    UINT32  Reserved1:              12;      /* [31:20] */
} B8_CODEC_BS_INTERLEAVE_REG_s;

/*-----------------------------------------------------------------------------------*\
 * B8 Codec : Error status Register
\*-----------------------------------------------------------------------------------*/
typedef struct {
    UINT32  ErrorCount:             6;       /* [5:0] Error count of err_info (0~32) */
    UINT32  ErrorCntSat:            1;       /* [6] 0: Less than or equal to 32 errors, 1: More than 32 errors (only first 32 errors are stored in err_info) */
    UINT32  Reserved0:              1;       /* [7] */
    UINT32  ErrorBank:              2;       /* [9:8] Read bank number (0~2) */
    UINT32  Reserved1:              22;      /* [31:10] */
} B8_CODEC_ERR_STATUS_REG_s;

/*-----------------------------------------------------------------------------------*\
 * B8 Codec : Segment Control Register
 \*-----------------------------------------------------------------------------------*/
typedef struct {
    UINT32  SegBlkMinus1:           12;      /* [11:0] Max. number of 4x1 block in a segment. (Range: 7~4095) */
    UINT32  Reserved0:              20;      /* [31:12] Reserved */
} B8_CODEC_SEGMENT_CTRL_REG_s;

/*-----------------------------------------------------------------------------------*\
 * B8 Codec : Blank Control 0 Register
 \*-----------------------------------------------------------------------------------*/
typedef struct {
    UINT32  ActiveHeightDiv2Minus1: 13;      /* [12:0] Active Region Height (Range: 1~8191) */
    UINT32  Reserved0:              3;       /* [15:13] Reserved */
    UINT32  Blank0Div2:             13;      /* [28:16] Blanking Line of Exposure 0 (Range: 0~8191) */
    UINT32  Reserved1:              3;       /* [31:29] Reserved */
} B8_CODEC_BLANK_CTRL0_REG_s;

/*-----------------------------------------------------------------------------------*\
 * B8 Codec : Blank Control 1 Register
 \*-----------------------------------------------------------------------------------*/
typedef struct {
    UINT32  Blank1Div2:             13;      /* [12:0] Blanking Line of Exposure 1 (Range: 0~8191) */
    UINT32  Reserved0:              3;       /* [15:13] Reserved */
    UINT32  Blank2Div2:             13;      /* [28:16] Blanking Line of Exposure 2 (Range: 0~8191) */
    UINT32  Reserved1:              3;       /* [31:29] Reserved */
} B8_CODEC_BLANK_CTRL1_REG_s;

/*-----------------------------------------------------------------------------------*\
 * B8 Codec : Misc Control 1 Register
 \*-----------------------------------------------------------------------------------*/
typedef struct {
    UINT32  ForceDepackerOnExt:     1;       /* [0] 1: Force CFA Codec to receive data from Depacker even internal buffer is full (This field is moved from dummy[0]) */
    UINT32  ExpSplitSegEn:          1;       /* [1] 1: Block at Exposure boundary is segment end (This field is moved from dummy[1]) */
    UINT32  Reserved0:              2;       /* [3:2] Reserved */
    UINT32  BlankVpredDis:          1;       /* [4] 1: Disable Vertical Prediction with Pixels in Blanking area (supported after B8) */
    UINT32  VbAdjustPlus:           1;       /* [5] Virtual buffer fullness adjustment method. 0: decrease, 1: increase */
    UINT32  Reserved1:              26;      /* [31:6] Reserved */
} B8_CODEC_MISC_CTRL_REG_s;

/*-----------------------------------------------------------------------------------*\
 * B8 Codec : Set0/1 add Register
\*-----------------------------------------------------------------------------------*/
typedef struct {
    UINT32  Value:                  17;      /* [16:0] Value */
    UINT32  Reserved0:              15;      /* [31:17] */
} B8_CODEC_SET_ADD_REG_s;

/*-----------------------------------------------------------------------------------*\
 * B8 Codec : Set0/1 mul Register
\*-----------------------------------------------------------------------------------*/
typedef struct {
    UINT32  Value:                  15;      /* [14:0] Value */
    UINT32  Reserved0:              17;      /* [31:15] */
} B8_CODEC_SET_MUL_REG_s;

/*-----------------------------------------------------------------------------------*\
 * B8 Codec : Set0/1 coding configuration Register
\*-----------------------------------------------------------------------------------*/
typedef struct {
    UINT32  LinesPerStrips:         14;      /* [13:0] Line number in a strip(slice), 0: no striping */
    UINT32  Reserved0:              2;       /* [15:14] */
    UINT32  LogVbSize:              5;       /* [20:16] Base 2 Logrithm of virtual buffer size (bitwidth). u5; 20~24; other values are not allowed */
    UINT32  Reserved1:              3;       /* [23:21] */
    UINT32  HufSelect:              1;       /* [24] 0: low bitrate, 1: high bitrate */
    UINT32  Reserved2:              7;       /* [31:25] */
} B8_CODEC_SET_CODING_CFG_REG_s;

/*-----------------------------------------------------------------------------------*\
 * B8 Codec : Set0/1 virtual buffer fullness Register
\*-----------------------------------------------------------------------------------*/
typedef struct {
    UINT32  VbFullness:             24;      /* [23:0] Virtual buffer fullness (u24; 0~2^log_vb_size-1) */
    UINT32  Reserved0:              8;       /* [31:24] */
} B8_CODEC_SET_VB_FULLNESS_REG_s;

/*-----------------------------------------------------------------------------------*\
 * B8 Codec : Set0/1 virtual buffer bitrate Register
\*-----------------------------------------------------------------------------------*/
typedef struct {
    UINT32  VbRate:                 14;      /* [13:0] Target bitrate of a block4x1 */
    UINT32  Reserved0:              2;       /* [15:14] */
    UINT32  VbRateTopOfSlice:       14;      /* [29:16] Target bitrate of a block4x1 at top of slice */
    UINT32  Reserved1:              2;       /* [31:30] */
} B8_CODEC_SET_VB_RATE_s;

/*-----------------------------------------------------------------------------------*\
 * B8 Codec : Set0/1 virtual buffer fullness adjustment Register
\*-----------------------------------------------------------------------------------*/
typedef struct {
    UINT32  Value:                  24;      /* [23:0] Value */
    UINT32  Reserved0:              8;       /* [31:24] */
} B8_CODEC_SET_VB_ADJ_REG_s;

/*-----------------------------------------------------------------------------------*\
 * B8 Codec : Set0/1 PP configuration Register
\*-----------------------------------------------------------------------------------*/
typedef struct {
    UINT32  NoPp:                   1;       /* [0] 0: Pre/Post processing enabled, 1: Pre/Post processing disabled */
    UINT32  Reserved0:              31;      /* [31:1] */
} B8_CODEC_SET_PP_CFG_s;

/*-----------------------------------------------------------------------------------*\
 * B8 Codec : Set0/1 faster to top offset Register
\*-----------------------------------------------------------------------------------*/
typedef struct {
    UINT32  Offset1:                15;      /* [14:0] Offset1 */
    UINT32  Reserved0:              1;       /* [15] */
    UINT32  Offset2:                15;      /* [30:16] Offset2 */
    UINT32  Reserved1:              1;       /* [31] */
} B8_CODEC_SET_OFFSET_REG_s;

/*-----------------------------------------------------------------------------------*\
 * B8 Codec : Set0/1 tone mapping Register
\*-----------------------------------------------------------------------------------*/
typedef struct {
    UINT32  Even:                   14;      /* [13:0] for even pixels */
    UINT32  Reserved0:              2;       /* [15:14] */
    UINT32  Odd:                    14;      /* [29:16] for odd pixels */
    UINT32  Reserved1:              2;       /* [31:30] */
} B8_CODEC_SET_TONE_REG_s;

/*-----------------------------------------------------------------------------------*\
 * B8 Codec : Set0/1 quantization Register
\*-----------------------------------------------------------------------------------*/
typedef struct {
    UINT32  Shift:                  4;       /* [3:0] shift value of quantization (0~14)*/
    UINT32  Add0:                   13;      /* [16:4] Valid when encode_mode = 1 */
    UINT32  Add1:                   13;      /* [29:17] Valid when encode_mode = 1 */
    UINT32  Reserved0:              2;       /* [31:30] */
} B8_CODEC_SET_Q_REG_s;

/*-----------------------------------------------------------------------------------------------*\
 * B8 Codec : All Registers
\*-----------------------------------------------------------------------------------------------*/
typedef struct {
    volatile B8_CODEC_CTRL_REG_s              Ctrl;                      /* 0x00(RW): Codec control */
    volatile B8_CODEC_SENSOR_CTRL_REG_s       SensorCtrl;                /* 0x04(RW): Sensor control */
    volatile B8_CODEC_PIC_SIZE_REG_s          PictureSize;               /* 0x08(RW): Picture size */
    volatile B8_CODEC_SET_NUM_REG_s           SetNum;                    /* 0x0C(RW): Setting number of next picture */
    volatile UINT32                           InterruptStatus;           /* 0x10(RWC): Interrupt status */
    volatile UINT32                           InterruptMask;             /* 0x14(RW): Interrupt mask(0: allowed, 1: masked) */
    volatile UINT32                           Debug;                     /* 0x18(RO): Debug Status */
    volatile B8_CODEC_DBG_SEL_REG_s           DebugSel;                  /* 0x1C(RW): Debug selection */
    volatile B8_CODEC_BS_INTERLEAVE_REG_s     BitStreamInterleave;       /* 0x20(RW): Bit stream interleave */
    volatile B8_CODEC_ERR_STATUS_REG_s        ErrorStatus0;              /* 0x24(RO): Error status */
    volatile UINT32                           ErrorInfo0;                /* 0x28(RO): Error information */
    volatile UINT32                           Reserved0;                 /* 0x2C: Reserved */
    volatile UINT32                           Dummy;                     /* 0x30: Dummy Register (B6A dummy[1:0] are moved to Misc control (Offset 0x48) */
    volatile UINT32                           Reserved1[3];              /* 0x34-0x3C: Reserved */
    volatile B8_CODEC_SEGMENT_CTRL_REG_s      SegCtrl;                   /* 0x40(RW): Segment control */
    volatile B8_CODEC_BLANK_CTRL0_REG_s       BlankCtrl0;                /* 0x44(RW): Blank control 0 */
    volatile B8_CODEC_BLANK_CTRL1_REG_s       BlankCtrl1;                /* 0x48(RW): Blank control 1 */
    volatile B8_CODEC_MISC_CTRL_REG_s         MiscCtrl;                  /* 0x4C(RW): Misc control */
    volatile UINT32                           Reserved2[124];            /* 0x50-0x23C: Reserved */

    volatile B8_CODEC_SET_ADD_REG_s           Set0AddBlue0;              /* 0x240(RW): Set 0 add blue 0 */
    volatile B8_CODEC_SET_ADD_REG_s           Set0AddGreen0;             /* 0x244(RW): Set 0 add green 0 */
    volatile B8_CODEC_SET_ADD_REG_s           Set0AddRed0;               /* 0x248(RW): Set 0 add red 0 */
    volatile B8_CODEC_SET_MUL_REG_s           Set0MulBlue0;              /* 0x24C(RW): Set 0 mul blue 0 */
    volatile B8_CODEC_SET_MUL_REG_s           Set0MulGreen0;             /* 0x250(RW): Set 0 mul green 0 */
    volatile B8_CODEC_SET_MUL_REG_s           Set0MulRed0;               /* 0x254(RW): Set 0 mul red 0 */
    volatile B8_CODEC_SET_ADD_REG_s           Set0AddBlue1;              /* 0x258(RW): Set 0 add blue 1 */
    volatile B8_CODEC_SET_ADD_REG_s           Set0AddGreen1;             /* 0x25C(RW): Set 0 add green 1 */
    volatile B8_CODEC_SET_ADD_REG_s           Set0AddRed1;               /* 0x260(RW): Set 0 add red 1 */
    volatile B8_CODEC_SET_MUL_REG_s           Set0MulBlue1;              /* 0x264(RW): Set 0 mul blue 1 */
    volatile B8_CODEC_SET_MUL_REG_s           Set0MulGreen1;             /* 0x268(RW): Set 0 mul green 1 */
    volatile B8_CODEC_SET_MUL_REG_s           Set0MulRed1;               /* 0x26C(RW): Set 0 mul red 1 */
    volatile B8_CODEC_SET_ADD_REG_s           Set0AddBlue2;              /* 0x270(RW): Set 0 add blue 2 */
    volatile B8_CODEC_SET_ADD_REG_s           Set0AddGreen2;             /* 0x274(RW): Set 0 add green 2 */
    volatile B8_CODEC_SET_ADD_REG_s           Set0AddRed2;               /* 0x278(RW): Set 0 add red 2 */
    volatile B8_CODEC_SET_MUL_REG_s           Set0MulBlue2;              /* 0x27C(RW): Set 0 mul blue 2 */
    volatile B8_CODEC_SET_MUL_REG_s           Set0MulGreen2;             /* 0x280(RW): Set 0 mul green 2 */
    volatile B8_CODEC_SET_MUL_REG_s           Set0MulRed2;               /* 0x284(RW): Set 0 mul red 2 */
    volatile B8_CODEC_SET_CODING_CFG_REG_s    Set0CodingCfg;             /* 0x288(RW): Set 0 coding configuration */
    volatile B8_CODEC_SET_VB_FULLNESS_REG_s   Set0VbFullness;            /* 0x28C(RW): Set 0 virtual buffer fullness */
    volatile B8_CODEC_SET_VB_RATE_s           Set0VbRate0;               /* 0x290(RW): Set 0 virtual buffer bitrate 0 */
    volatile B8_CODEC_SET_VB_RATE_s           Set0VbRate1;               /* 0x294(RW): Set 0 virtual buffer bitrate 1 */
    volatile B8_CODEC_SET_VB_RATE_s           Set0VbRate2;               /* 0x298(RW): Set 0 virtual buffer bitrate 2 */
    volatile B8_CODEC_SET_VB_ADJ_REG_s        Set0VbAdjLeft;             /* 0x29C(RW): Set 0 virtual buffer fullness adjustment */
    volatile B8_CODEC_SET_VB_ADJ_REG_s        Set0VbAdjTop;              /* 0x2A0(RW): Set 0 virtual buffer fullness adjustment */
    volatile B8_CODEC_SET_PP_CFG_s            Set0PpCfg;                 /* 0x2A4(RW): Set 0 PP configuration */
    volatile B8_CODEC_SET_OFFSET_REG_s        Set0OffsetBlue0;           /* 0x2A8(RW): Set 0 faster to top offset blue 0 */
    volatile B8_CODEC_SET_OFFSET_REG_s        Set0OffsetBlue1;           /* 0x2AC(RW): Set 0 faster to top offset blue 1 */
    volatile B8_CODEC_SET_OFFSET_REG_s        Set0OffsetBlue2;           /* 0x2B0(RW): Set 0 faster to top offset blue 2 */
    volatile B8_CODEC_SET_OFFSET_REG_s        Set0OffsetGreen0;          /* 0x2B4(RW): Set 0 faster to top offset green 0 */
    volatile B8_CODEC_SET_OFFSET_REG_s        Set0OffsetGreen1;          /* 0x2B8(RW): Set 0 faster to top offset green 1 */
    volatile B8_CODEC_SET_OFFSET_REG_s        Set0OffsetGreen2;          /* 0x2BC(RW): Set 0 faster to top offset green 2 */
    volatile B8_CODEC_SET_OFFSET_REG_s        Set0OffsetRed0;            /* 0x2C0(RW): Set 0 faster to top offset red 0 */
    volatile B8_CODEC_SET_OFFSET_REG_s        Set0OffsetRed1;            /* 0x2C4(RW): Set 0 faster to top offset red 1 */
    volatile B8_CODEC_SET_OFFSET_REG_s        Set0OffsetRed2;            /* 0x2C8(RW): Set 0 faster to top offset red 2 */
    volatile B8_CODEC_SET_VB_ADJ_REG_s        Set0VbAdjNoLeftPred;       /* 0x2CC(RW): Set 0 virtual buffer fullness adjustment */
    volatile UINT32                           Reserved3[76];             /* 0x2D0-0x3FC: Reserved */
    volatile B8_CODEC_SET_TONE_REG_s          Set0Table0Tone[129];       /* 0x400-0x600(RW): Set 0 tab0 tone 0~128 */
    volatile B8_CODEC_SET_TONE_REG_s          Set0Table1Tone[129];       /* 0x604-0x804(RW): Set 0 tab1 tone 0~128 */
    volatile B8_CODEC_SET_TONE_REG_s          Set0Table2Tone[129];       /* 0x808-0xA08(RW): Set 0 tab2 tone 0~128 */
    volatile UINT32                           Reserved4[125];            /* 0xA0C-0xBFC: Reserved */
    volatile B8_CODEC_SET_Q_REG_s             Set0Q[256];                /* 0xC00-0xFFC(RW): Set 0 Quantization 0~255 */

    volatile UINT32                           Reserved5[64];             /* 0x1000-0x10FC: Reserved */
    volatile UINT32                           DebugSignal[64];           /* 0x1100-0x11FC(RO): Debug signal 0~63 */
    volatile UINT32                           Reserved6[16];             /* 0x1200-0x123C: Reserved */

    volatile B8_CODEC_SET_ADD_REG_s           Set1AddBlue0;              /* 0x1240(RW): Set 1 add blue 0 */
    volatile B8_CODEC_SET_ADD_REG_s           Set1AddGreen0;             /* 0x1244(RW): Set 1 add green 0 */
    volatile B8_CODEC_SET_ADD_REG_s           Set1AddRed0;               /* 0x1248(RW): Set 1 add red 0 */
    volatile B8_CODEC_SET_MUL_REG_s           Set1MulBlue0;              /* 0x124C(RW): Set 1 mul blue 0 */
    volatile B8_CODEC_SET_MUL_REG_s           Set1MulGreen0;             /* 0x1250(RW): Set 1 mul green 0 */
    volatile B8_CODEC_SET_MUL_REG_s           Set1MulRed0;               /* 0x1254(RW): Set 1 mul red 0 */
    volatile B8_CODEC_SET_ADD_REG_s           Set1AddBlue1;              /* 0x1258(RW): Set 1 add blue 1 */
    volatile B8_CODEC_SET_ADD_REG_s           Set1AddGreen1;             /* 0x125C(RW): Set 1 add green 1 */
    volatile B8_CODEC_SET_ADD_REG_s           Set1AddRed1;               /* 0x1260(RW): Set 1 add red 1 */
    volatile B8_CODEC_SET_MUL_REG_s           Set1MulBlue1;              /* 0x1264(RW): Set 1 mul blue 1 */
    volatile B8_CODEC_SET_MUL_REG_s           Set1MulGreen1;             /* 0x1268(RW): Set 1 mul green 1 */
    volatile B8_CODEC_SET_MUL_REG_s           Set1MulRed1;               /* 0x126C(RW): Set 1 mul red 1 */
    volatile B8_CODEC_SET_ADD_REG_s           Set1AddBlue2;              /* 0x1270(RW): Set 1 add blue 2 */
    volatile B8_CODEC_SET_ADD_REG_s           Set1AddGreen2;             /* 0x1274(RW): Set 1 add green 2 */
    volatile B8_CODEC_SET_ADD_REG_s           Set1AddRed2;               /* 0x1278(RW): Set 1 add red 2 */
    volatile B8_CODEC_SET_MUL_REG_s           Set1MulBlue2;              /* 0x127C(RW): Set 1 mul blue 2 */
    volatile B8_CODEC_SET_MUL_REG_s           Set1MulGreen2;             /* 0x1280(RW): Set 1 mul green 2 */
    volatile B8_CODEC_SET_MUL_REG_s           Set1MulRed2;               /* 0x1284(RW): Set 1 mul red 2 */
    volatile B8_CODEC_SET_CODING_CFG_REG_s    Set1CodingCfg;             /* 0x1288(RW): Set 1 coding configuration */
    volatile B8_CODEC_SET_VB_FULLNESS_REG_s   Set1VbFullness;            /* 0x128C(RW): Set 1 virtual buffer fullness */
    volatile B8_CODEC_SET_VB_RATE_s           Set1VbRate0;               /* 0x1290(RW): Set 1 virtual buffer bitrate 0 */
    volatile B8_CODEC_SET_VB_RATE_s           Set1VbRate1;               /* 0x1294(RW): Set 1 virtual buffer bitrate 1 */
    volatile B8_CODEC_SET_VB_RATE_s           Set1VbRate2;               /* 0x1298(RW): Set 1 virtual buffer bitrate 2 */
    volatile B8_CODEC_SET_VB_ADJ_REG_s        Set1VbAdjLeft;             /* 0x129C(RW): Set 1 virtual buffer fullness adjustment */
    volatile B8_CODEC_SET_VB_ADJ_REG_s        Set1VbAdjTop;              /* 0x12A0(RW): Set 1 virtual buffer fullness adjustment */
    volatile B8_CODEC_SET_PP_CFG_s            Set1PpCfg;                 /* 0x12A4(RW): Set 1 PP configuration */
    volatile B8_CODEC_SET_OFFSET_REG_s        Set1OffsetBlue0;           /* 0x12A8(RW): Set 1 faster to top offset blue 0 */
    volatile B8_CODEC_SET_OFFSET_REG_s        Set1OffsetBlue1;           /* 0x12AC(RW): Set 1 faster to top offset blue 1 */
    volatile B8_CODEC_SET_OFFSET_REG_s        Set1OffsetBlue2;           /* 0x12B0(RW): Set 1 faster to top offset blue 2 */
    volatile B8_CODEC_SET_OFFSET_REG_s        Set1OffsetGreen0;          /* 0x12B4(RW): Set 1 faster to top offset green 0 */
    volatile B8_CODEC_SET_OFFSET_REG_s        Set1OffsetGreen1;          /* 0x12B8(RW): Set 1 faster to top offset green 1 */
    volatile B8_CODEC_SET_OFFSET_REG_s        Set1OffsetGreen2;          /* 0x12BC(RW): Set 1 faster to top offset green 2 */
    volatile B8_CODEC_SET_OFFSET_REG_s        Set1OffsetRed0;            /* 0x12C0(RW): Set 1 faster to top offset red 0 */
    volatile B8_CODEC_SET_OFFSET_REG_s        Set1OffsetRed1;            /* 0x12C4(RW): Set 1 faster to top offset red 1 */
    volatile B8_CODEC_SET_OFFSET_REG_s        Set1OffsetRed2;            /* 0x12C8(RW): Set 1 faster to top offset red 2 */
    volatile B8_CODEC_SET_VB_ADJ_REG_s        Set1VbAdjNoLeftPred;       /* 0x12CC(RW): Set 1 virtual buffer fullness adjustment */
    volatile UINT32                           Reserved7[76];             /* 0x12D0-0x3FC: Reserved */
    volatile B8_CODEC_SET_TONE_REG_s          Set1Table0Tone[129];       /* 0x1400-0x600(RW): Set 1 tab0 tone 0~128 */
    volatile B8_CODEC_SET_TONE_REG_s          Set1Table1Tone[129];       /* 0x1604-0x804(RW): Set 1 tab1 tone 0~128 */
    volatile B8_CODEC_SET_TONE_REG_s          Set1Table2Tone[129];       /* 0x1808-0xA08(RW): Set 1 tab2 tone 0~128 */
    volatile UINT32                           Reserved8[125];            /* 0x1A0C-0xBFC: Reserved */
    volatile B8_CODEC_SET_Q_REG_s             Set1Q[256];                /* 0x1C00-0xFFC: Set 1 Quantization 0~255 */
} B8_CODEC_REG_s;

#endif /* AMBA_B8_REG_CODEC_H */
