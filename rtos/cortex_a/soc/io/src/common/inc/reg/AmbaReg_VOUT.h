/**
 *  @file AmbaReg_VOUT.h
 *
 *  @copyright Copyright (c) 2022 Ambarella, Inc.
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
 *  @details Definitions & Constants for Video Output (VOUT) Control Registers
 *
 */

#ifndef AMBA_REG_VOUT_H
#define AMBA_REG_VOUT_H

#ifndef AMBA_TYPES_H
#include "AmbaTypes.h"
#endif

#define VOUT_MIXER0_REG_OFFSET    0x200U
#define VOUT_DISPLAY0_REG_OFFSET  0x300U
#define VOUT_MIXER1_REG_OFFSET    0x500U
#define VOUT_DISPLAY1_REG_OFFSET  0x600U
#define VOUT_OSD_REG_OFFSET       0x800U
#define VOUT_TOP_REG_OFFSET       0xA00U
#define VOUT_DSI_CMD0_REG_OFFSET  0xB00U
#define VOUT_DISPLAY2_REG_OFFSET  0xC00U
#define VOUT_DSI_CMD1_REG_OFFSET  0xE00U

/*
 * VOUT: Reset Control Register
 */
typedef struct {
    UINT32  Reserved:               31; /* [30:0] Reserved */
    UINT32  Reset:                  1;  /* [31] 1 = Reset entire VOUT block */
} AMBA_VOUT_RESET_REG_s;

/*
 * VOUT: Clock Enable Control Register
 */
#if defined(CONFIG_SOC_CV2) || defined(CONFIG_SOC_CV22) || defined(CONFIG_SOC_CV25)
typedef struct {
    UINT32  Enable:                 1;  /* [0] 1 = Enable clock signal */
    UINT32  MipiSelectCsi:          1;  /* [1] Set to 1 when mipi csi is running, otherwise 0 */
    UINT32  Reserved:               30; /* [31:2] Reserved */
} AMBA_VOUT_CLK_ENABLE_REG_s;
#else
typedef struct {
    UINT32  Enable:                 1;  /* [0] 1 = Enable clock signal */
    UINT32  Reserved:               31; /* [31:1] Reserved */
} AMBA_VOUT_CLK_ENABLE_REG_s;
#endif

#if defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)
/*
 * VOUT: Display C Clock Enable Control Register
 */
typedef struct {
    UINT32  Enable:                 2;  /* [1:0] 0 = Disable display_c clock, 1 = Enable display_c clock, 2 = Enable mixer_b drive display_c */
    UINT32  Reserved:               30; /* [31:2] Reserved */
} AMBA_VOUT_CLK_C_ENABLE_REG_s;
#endif

/*
 * VOUT: Clock Disable Control Register
 */
typedef struct {
    UINT32  Disable:                1;  /* [0] 1 = Disable clock signal */
    UINT32  Reserved:               31; /* [31:1] Reserved */
} AMBA_VOUT_CLK_DISABLE_REG_s;

/*
 * VOUT: TV Encoder Phase Increment for FSC Generation ROM Register
 */
typedef struct {
    UINT32  PhaseInc:               8;  /* [7:0] Phase increment */
    UINT32  Reserved:               24; /* [31:8] Reserved */
} AMBA_VOUT_TVENC_PHASE_INC_REG_s;

/*
 * VOUT: TV Encoder Subcarrier to Horizontal Phase Offset Adjust Register
 */
typedef struct {
    UINT32  SubCarrierToH:          8;  /* [7:0] Subcarrier to Horizontal Phase Offset Adjust */
    UINT32  Reserved:               24; /* [31:8] Reserved */
} AMBA_VOUT_TVENC_SUBCARRIER_TO_H_REG_s;

/*
 * VOUT: TV Encoder Control Register
 */
typedef struct {
    UINT32  ResetPhaseAcc:          1;  /* [0] 1 = Reset Phase Accumulator */
    UINT32  EnablePhaseOffset:      1;  /* [1] 1 = Enable phase offset in subcarrier generator */
    UINT32  InvertV:                1;  /* [2] 1 = Invert V component */
    UINT32  InvertU:                1;  /* [3] 1 = Invert U component */
    UINT32  Reserved:               28; /* [31:4] Reserved */
} AMBA_VOUT_TVENC_CTRL_REG_s;

/*
 * VOUT: TV Encoder Data Level Adjust Register
 */
typedef struct {
    UINT32  Level:                  8;  /* [7:0] Data Level */
    UINT32  Reserved:               24; /* [31:8] Reserved */
} AMBA_VOUT_TVENC_LEVEL_ADJUST_REG_s;

/*
 * VOUT: TV Encoder Luma Mode Register
 */
typedef struct {
    UINT32  LumaGain:               1;  /* [0] 0 = 1.184, 1 = 1.25 */
    UINT32  LumaLpfSelect0:         1;  /* [1] Luma LPF characteristic (Bit 0) */
    UINT32  LumaDelay:              3;  /* [4:2] Adjust Luma Delay (in unit of 24 MHz clocks) */
    UINT32  ColorBar:               1;  /* [5] 1 = Enable internal color bar generation */
    UINT32  LumaInterpolation:      2;  /* [7:6] 0 = LPF/Linear, 1 = LPF, 2 = Linear, 3 = Pixel Repeat */
    UINT32  Reserved:               24; /* [31:8] Reserved */
} AMBA_VOUT_TVENC_LUMA_MODE_REG_s;

/*
 * VOUT: TV Encoder DAC Control Register
 */
typedef struct {
    UINT32  PowerDownChroma:        1;  /* [0] 1 = Power down Chroma DAC */
    UINT32  PowerDownLuma:          1;  /* [1] 1 = Power down Luma DAC */
    UINT32  PowerDownCvbs:          1;  /* [2] 1 = Power down CVBS DAC */
    UINT32  BypassYUV:              1;  /* [3] 1 = Bypass YUV to DAC (function mode) */
    UINT32  Reserved:               28; /* [31:4] Reserved */
} AMBA_VOUT_TVENC_DAC_CTRL_REG_s;

/*
 * VOUT: TV Encoder Burst Amplitude Register
 */
typedef struct {
    UINT32  Amplitude:              8;  /* [7:0] Burst Amplitude Value */
    UINT32  Reserved:               24; /* [31:8] Reserved */
} AMBA_VOUT_TVENC_BURST_AMPLITUDE_REG_s;

/*
 * VOUT: TV Encoder Chroma Mode Register
 */
typedef struct {
    UINT32  ChromaGain:             1;  /* [0] 0 = U/V 0.95, 1 = U 1.065 / V 1.5 */
    UINT32  ChromaLpfSelect:        1;  /* [1] Chroma LPF characteristic */
    UINT32  Reserved:               30; /* [31:2] Reserved */
} AMBA_VOUT_TVENC_CHROMA_MODE_REG_s;

/*
 * VOUT: TV Encoder Sync Mode Register
 */
typedef struct {
    UINT32  TvStandard:             3;  /* [2:0] 0 = M/NTSC, 1 = B/PAL, 2 = M/PAL, 3 = N/PAL, 4 = CN/PAL */
    UINT32  InputFormat:            1;  /* [3] 0 = Single 8-bit CbYCrY, 1 = Separate 16-bit Y,CbCr */
    UINT32  SyncDirection:          1;  /* [4] 0 = H/V sync in, 1 = H/V sync out */
    UINT32  ForceD1:                1;  /* [5] 1 = Use D1 timing codes and force H/V sync out, 8-bit Single CbYCrY */
    UINT32  VsyncPhase:             1;  /* [6] Select Phase of Vsync in/out */
    UINT32  HsyncPhase:             1;  /* [7] Select Phase of Hsync in/out */
    UINT32  Reserved:               24; /* [31:8] Reserved */
} AMBA_VOUT_TVENC_SYNC_MODE_REG_s;

/*
 * VOUT: TV Encoder V Sync Offset Register
 */
typedef struct {
    UINT32  VsyncOffset1:           2;  /* [1:0] Veritcal sync offset (Bit[9:8]) */
    UINT32  Reserved:               2;  /* [3:2] Reserved */
    UINT32  FieldSyncPhase:         1;  /* [4] Select Phase of Field Sync in/out */
    UINT32  ExtFieldSync:           1;  /* [5] 1 = Use external V sync as Field Sync Input */
    UINT32  ClockPhase:             2;  /* [7:6] 6.75/13.5 MHz phase adjust (input sample vs. SYNC) */
    UINT32  Reserved1:              24; /* [31:8] Reserved */
} AMBA_VOUT_TVENC_VSYNC_OFFSET_MSB_REG_s;

typedef struct {
    UINT32  VsyncOffset0:           8;  /* [7:0] Vertical sync offset (Bit[7:0]) */
    UINT32  Reserved:               24; /* [31:8] Reserved */
} AMBA_VOUT_TVENC_VSYNC_OFFSET_LSB_REG_s;

/*
 * VOUT: TV Encoder H Sync Offset Register
 */
typedef struct {
    UINT32  HsyncOffset1:           3;  /* [2:0] Horizontal sync offset (Bit[10:8]) */
    UINT32  Reserved:               29; /* [31:3] Reserved */
} AMBA_VOUT_TVENC_HSYNC_OFFSET_MSB_REG_s;

typedef struct {
    UINT32  HsyncOffset0:           8;  /* [7:0] Horizontal sync offset (Bit[7:0]) */
    UINT32  Reserved:               24; /* [31:8] Reserved */
} AMBA_VOUT_TVENC_HSYNC_OFFSET_LSB_REG_s;

/*
 * VOUT: TV Encoder Horizontal Half-Line Pixel Count Register
 */
typedef struct {
    UINT32  NumPixelsHalfLine1:     2;  /* [1:0] Horizontal half-line pixel count (Bit[9:8]) */
    UINT32  Reserved:               30; /* [31:2] Reserved */
} AMBA_VOUT_TVENC_HALF_LINE_MSB_REG_s;

typedef struct {
    UINT32  NumPixelsHalfLine0:     8;  /* [7:0] Horizontal half-line pixel count (Bit[7:0]) */
    UINT32  Reserved:               24; /* [31:8] Reserved */
} AMBA_VOUT_TVENC_HALF_LINE_LSB_REG_s;

/*
 * VOUT: TV Encoder Closed Captioned Data Register
 */
typedef struct {
    UINT32  CcData:                 8;  /* [7:0] Closed captioned data */
    UINT32  Reserved:               24; /* [31:8] Reserved */
} AMBA_VOUT_TVENC_CC_DATA_REG_s;

/*
 * VOUT: TV Encoder Closed Caption Control Register
 */
typedef struct {
    UINT32  CcEnable:               2;  /* [1:0] 0 = Off, 1 = Odd Field Only, 2 = Even Field Only, 3 = Both Fields */
    UINT32  Reserved:               30; /* [31:2] Reserved */
} AMBA_VOUT_TVENC_CC_CTRL_REG_s;

/*
 * VOUT: TV Encoder ROM Test Register
 */
typedef struct {
    UINT32  SinCosTestEnable:       1;  /* [0] 1 = Enable SIN/COS ROM test */
    UINT32  SinCOsSelect:           1;  /* [1] Test output of ROM. 1 = SIN, 0 = COS */
    UINT32  Reserved:               30; /* [31:2] Reserved */
} AMBA_VOUT_TVENC_ROM_TEST_REG_s;

/*
 * VOUT: TV Encoder Luma Test Register
 */
typedef struct {
    UINT32  DigitalOutputTest:      2;  /* [1:0] 0 = normal, 1 = Y, 2 = CVBS, 3 = C */
    UINT32  DacTestEnable:          1;  /* [2] 1 = Enable DAC test */
    UINT32  SinCosTestEnable:       1;  /* [3] 1 = Enable SIN/COS test */
    UINT32  LumaGainOff:            1;  /* [4] 1 = Turn off luma gain for test bypass */
    UINT32  LumaLpfSelect1:         1;  /* [5] Luma LPF characteristic (Bit 1) */
    UINT32  LumaUpsampleBypass:     1;  /* [6] 1 = Luma upsample bypass */
    UINT32  Reserved:               25; /* [31:7] Reserved */
} AMBA_VOUT_TVENC_LUMA_TEST_REG_s;

/*
 * VOUT: TV Encoder Chroma Test Register
 */
typedef struct {
    UINT32  ChromaGainOff:          1;  /* [0] 1 = Turn off chroma gain for test bypass */
    UINT32  ChromaLpfBypass:        1;  /* [1] 1 = Chroma LPF bypass */
    UINT32  ChromaUpsampleBypass:   1;  /* [2] 1 = Chroma upsample bypass */
    UINT32  Reserved:               29; /* [31:3] Reserved */
} AMBA_VOUT_TVENC_CHROMA_TEST_REG_s;

/*
 * VOUT: Mixer Section 0/1 Control Register
 */
#if defined(CONFIG_SOC_CV2) || defined(CONFIG_SOC_CV22) || defined(CONFIG_SOC_CV25)
typedef struct {
    UINT32  Input444:               1;  /* [0] 0 = YC 4:2:2, 1 = YC 4:4:4 or RGB */
    UINT32  CscEnable:              2;  /* [2:1] 0 = CSC disabled, 1 = CSC enabled for video, 2 = CSC enabled for OSD */
    UINT32  VideoFlip:              1;  /* [3] 0 = Normal mode, 1 = Video data is horizontally reversed */
    UINT32  Reserved:               27; /* [30:4] Reserved */
    UINT32  Reset:                  1;  /* [31] 1 = Reset the mixer section */
} AMBA_VOUTM_CTRL_REG_s;
#else
typedef struct {
    UINT32  Input444:               1;  /* [0] 0 = YC 4:2:2, 1 = YC 4:4:4 or RGB */
    UINT32  CscEnable:              2;  /* [2:1] 0 = CSC disabled, 1 = CSC enabled for video, 2 = CSC enabled for OSD */
    UINT32  VideoFlip:              1;  /* [3] 0 = Normal mode, 1 = Video data is horizontally reversed */
    UINT32  Input420:               1;  /* [4] 0 = Input 422 video, 1 = Input 420 video */
    UINT32  Output420:              1;  /* [5] 0 = Output format based on mixer, 1 = Output 420 video */
    UINT32  Reserved:               25; /* [30:6] Reserved */
    UINT32  Reset:                  1;  /* [31] 1 = Reset the mixer section */
} AMBA_VOUTM_CTRL_REG_s;
#endif

/*
 * VOUT: Mixer Section 0/1 Status Register
 */
typedef struct {
    UINT32  Reserved:               31; /* [30:0] Reserved */
    UINT32  ResetComplete:          1;  /* [31] 1 = The mixer section reset is completed */
} AMBA_VOUTM_STATUS_REG_s;

/*
 * VOUT: Mixer Section 0/1 Active Region Size Register
 */
typedef struct {
    UINT32  NumActiveLines:         14; /* [13:0] Number of lines in the active region minus 1 */
    UINT32  Reserved:               2;  /* [15:14] Reserved */
    UINT32  NumActivePixels:        14; /* [29:16] Number of pixels per line minus 1 */
    UINT32  Reserved1:              2;  /* [31:30] Reserved */
} AMBA_VOUTM_ACTIVE_SIZE_REG_s;

/*
 * VOUT: Mixer Section 0/1 Video/OSD Region Location Register
 */
typedef struct {
    UINT32  LineLocation:           14; /* [13:0] Starting/Ending line of the region */
    UINT32  Reserved:               2;  /* [15:14] Reserved */
    UINT32  ColumnLocation:         14; /* [29:16] Starting/Ending column of the region */
    UINT32  Reserved1:              2;  /* [31:30] Reserved */
} AMBA_VOUTM_REGION_LOCATION_REG_s;

/*
 * VOUT: Mixer Section 0/1 OSD Control Register
 */
typedef struct {
    UINT32  GlobalAlpha:            8;  /* [7:0] The minimum allowed pixel alpha value */
    UINT32  PremultiplyAlpha:       1;  /* [8] 1 = OSD data premultiplied by alpha value */
    UINT32  ColorFormat:            5;  /* [13:9] 0-12 = 16-bit color, 27-31 = 32-bit color */
    UINT32  ByteOrder:              1;  /* [14] 0 = OSD data {[31:0]}, 1 = OSD data {[7:0], [15:8], [23:16], [31:24]} */
    UINT32  EnableColorKey:         1;  /* [15] 1 = Enable Color Key Transparency */
    UINT32  ColorKey:               16; /* [31:16] 16-bit color key as transparent color */
} AMBA_VOUTM_OSD_CTRL_REG_s;

/*
 * VOUT: Mixer Section 0/1 Color Space Converter Parameter Register
 */
typedef struct {
    UINT32  CoefA0:                 13; /* [12:0] Signed 2.10 bits */
    UINT32  Reserved:               3;  /* [15:13] Reserved */
    UINT32  CoefA1:                 13; /* [28:16] Signed 2.10 bits */
    UINT32  Reserved1:              3;  /* [31:29] Reserved */
} AMBA_VOUTM_CSC_PARAM_0_REG_s;

typedef struct {
    UINT32  CoefA2:                 13; /* [12:0] Signed 2.10 bits */
    UINT32  Reserved:               3;  /* [15:13] Reserved */
    UINT32  CoefA3:                 13; /* [28:16] Signed 2.10 bits */
    UINT32  Reserved1:              3;  /* [31:29] Reserved */
} AMBA_VOUTM_CSC_PARAM_1_REG_s;

typedef struct {
    UINT32  CoefA4:                 13; /* [12:0] Signed 2.10 bits */
    UINT32  Reserved:               3;  /* [15:13] Reserved */
    UINT32  CoefA5:                 13; /* [28:16] Signed 2.10 bits */
    UINT32  Reserved1:              3;  /* [31:29] Reserved */
} AMBA_VOUTM_CSC_PARAM_2_REG_s;

typedef struct {
    UINT32  CoefA6:                 13; /* [12:0] Signed 2.10 bits */
    UINT32  Reserved:               3;  /* [15:13] Reserved */
    UINT32  CoefA7:                 13; /* [28:16] Signed 2.10 bits */
    UINT32  Reserved1:              3;  /* [31:29] Reserved */
} AMBA_VOUTM_CSC_PARAM_3_REG_s;

typedef struct {
    UINT32  CoefA8:                 13; /* [12:0] Signed 2.10 bits */
    UINT32  Reserved:               3;  /* [15:13] Reserved */
    UINT32  ConstB0:                11; /* [26:16] Signed 10 bits */
    UINT32  Reserved1:              5;  /* [31:27] Reserved */
} AMBA_VOUTM_CSC_PARAM_4_REG_s;

typedef struct {
    UINT32  ConstB1:                11; /* [10:0] Signed 10 bits */
    UINT32  Reserved:               5;  /* [15:11] Reserved */
    UINT32  ConstB2:                11; /* [26:16] Signed 10 bits */
    UINT32  Reserved1:              5;  /* [31:27] Reserved */
} AMBA_VOUTM_CSC_PARAM_5_REG_s;

typedef struct {
    UINT32  Output0ClampLow:        8;  /* [7:0] The minimum allowed value for color component 0 (Y/G) */
    UINT32  Reserved:               8;  /* [15:8] Reserved */
    UINT32  Output0ClampHigh:       8;  /* [23:16] The maxiumum allowed value for color component 0 (Y/G) */
    UINT32  Reserved1:              8;  /* [31:24] Reserved */
} AMBA_VOUTM_CSC_PARAM_6_REG_s;

typedef struct {
    UINT32  Output1ClampLow:        8;  /* [7:0] The minimum allowed value for color component 1 (Cb/B) */
    UINT32  Reserved:               8;  /* [15:8] Reserved */
    UINT32  Output1ClampHigh:       8;  /* [23:16] The maxiumum allowed value for color component 1 (Cb/B) */
    UINT32  Reserved1:              8;  /* [31:24] Reserved */
} AMBA_VOUTM_CSC_PARAM_7_REG_s;

typedef struct {
    UINT32  Output2ClampLow:        8;  /* [7:0] The minimum allowed value for color component 2 (Cr/R) */
    UINT32  Reserved:               8;  /* [15:8] Reserved */
    UINT32  Output2ClampHigh:       8;  /* [23:16] The maxiumum allowed value for color component 2 (Cr/R) */
    UINT32  Reserved1:              8;  /* [31:24] Reserved */
} AMBA_VOUTM_CSC_PARAM_8_REG_s;

/*
 * VOUT: Mixer/Display Section 0/1 Background/Highlight Color Register
 */
typedef struct {
    UINT32  CR:                     8;  /* [7:0] Cr component of background color */
    UINT32  CB:                     8;  /* [15:8] Cb component of background color */
    UINT32  Y:                      8;  /* [23:16] Y component of background color */
    UINT32  Reserved:               8;  /* [31:24] Reserved */
} AMBA_VOUT_COLOR_REG_s;

/*
 * VOUT: Mixer Section 0/1 Highlight Threshold and Color Register
 */
typedef struct {
    UINT32  CR:                     8;  /* [7:0] Cr component of background color */
    UINT32  CB:                     8;  /* [15:8] Cb component of background color */
    UINT32  Y:                      8;  /* [23:16] Y component of background color */
    UINT32  Threshold:              8;  /* [31:24] Highlight the pixel with luma value higher than this threshold */
} AMBA_VOUTM_HIGHLIGHT_REG_s;

/*
 * VOUT: Display Sync Counter Control Register
 */
#if defined(CONFIG_SOC_CV2) || defined(CONFIG_SOC_CV22) || defined(CONFIG_SOC_CV25) || defined(CONFIG_SOC_CV28) || defined(CONFIG_SOC_H32)
typedef struct {
    UINT32  SyncEnable:             2;  /* [1:0] 1 = Enable input sync */
    UINT32  LineCount:              3;  /* [4:2] Log2 of lines to sync in line sync mode */
    UINT32  Count:                  6;  /* [10:5] Sync counter number for input video sync */
    UINT32  DecrementCtrl:          1;  /* [11] 0 = No decrement, 1 = Decrement counter after N lines received in line sync mode or EOF received in frame sync mode */
    UINT32  AudioSyncEnable:        1;  /* [12] 1 = Enable SLVS audio input sync */
    UINT32  AudioCount:             6;  /* [18:13] SLVS Audio Input Sync Counter Number */
    UINT32  AudioDecrementCtrl:     1;  /* [19] 0 = No decrement, 1 = Decrement counter after N lines received in line sync mode or EOF received in frame sync mode */
    UINT32  Reserved:               12; /* [31:20] Reserved */
} AMBA_VOUT_INPUT_SYNC_CTRL_REG_s;

typedef struct {
    UINT32  FrameSyncEnable:        1;  /* [0] 1 = Enable frame sync */
    UINT32  FrameCount:             6;  /* [6:1] Output Frame Sync Counter Number */
    UINT32  SyncEnable0v:           1;  /* [7] 1 = Enable 0v sync */
    UINT32  SyncCount0v:            6;  /* [13:8] 0v Sync Counter Number */
    UINT32  AudioFrameSyncEnable:   1;  /* [14] 1 = Enable audio frame sync */
    UINT32  AudioFrameSyncCount:    6;  /* [20:15] SLVS Audio Frame Sync Counter Number */
    UINT32  Reserved:               11; /* [31:21] Reserved */
} AMBA_VOUT_OUTPUT_SYNC_CTRL_REG_s;
#else
typedef struct {
    UINT32  SyncEnable:             2;  /* [1:0] 1 = Enable input sync */
    UINT32  LineCount:              3;  /* [4:2] Log2 of lines to sync in line sync mode */
    UINT32  Count:                  8;  /* [12:5] Sync counter number for input video sync */
    UINT32  DecrementCtrl:          1;  /* [13] 0 = No decrement, 1 = Decrement counter after N lines received in line sync mode or EOF received in frame sync mode */
    UINT32  Reserved:               18; /* [31:14] Reserved */
} AMBA_VOUT_INPUT_SYNC_CTRL_REG_s;

typedef struct {
    UINT32  FrameSyncEnable:        1;  /* [0] 1 = Enable frame sync */
    UINT32  FrameCount:             8;  /* [8:1] Output Frame Sync Counter Number */
    UINT32  SyncEnable0v:           1;  /* [9] 1 = Enable 0v sync */
    UINT32  SyncCount0v:            8;  /* [17:10] 0v Sync Counter Number */
    UINT32  Reserved:               14; /* [31:18] Reserved */
} AMBA_VOUT_OUTPUT_SYNC_CTRL_REG_s;
#endif

/*
 * VOUT: Display Section 0/1 Control Register
 */
#if defined(CONFIG_SOC_CV2) || defined(CONFIG_SOC_CV22) || defined(CONFIG_SOC_CV25)
typedef struct {
    UINT32  FixedFormatSelect:      5;  /* [4:0] 0 = Software configurable, 1-12 = Fixed/Standard format */
    UINT32  Interlaced:             1;  /* [5] 0 = Progressive, 1 = Interlaced */
    UINT32  FlipMode:               1;  /* [6] 1 = Enable horizontal flip */
    UINT32  GammaMapping:           1;  /* [7] 1 = Enable gamma mapping (Display 0 only) */
    UINT32  HalfPixShift:           1;  /* [8] 1 = Enable half-pixel shift (Display 0 only) */
    UINT32  HalfPixShiftLines:      1;  /* [9] Apply half-pixel shift to 0 = Odd lines, 1 = Even lines */
    UINT32  Format3D:               2;  /* [11:10] 0 = 2D, 1 = 3D side by side, 2 = 3D top and bottom, 3 = 3D frame packed */
    UINT32  Fpd4thLaneEnable:       1;  /* [12] 1 = Enable 4th output lane in FPD link (Display 0 only)  */
    UINT32  FpdOrderReverse:        1;  /* [13] */
    UINT32  FpdMsbSelect:           1;  /* [14] 0 = LSB on data lane 3(JEIDA), 1 = MSB on data lane 3(VESA) */
    UINT32  Reserved:               5;  /* [19:15] Reserved */
    UINT32  MipiSyncEndEnable:      1;  /* [20] 1 = Enable MIPI sync end */
    UINT32  MipiEotpEnable:         1;  /* [21] 1 = Enable MIPI DSI EoTP */
    UINT32  NumMipiLane:            2;  /* [23:22] 0 = 1 lane, 1 = 2 lanes, 3 = 4lanes */
    UINT32  Reserved1:              1;  /* [24] Reserved */
    UINT32  VoutVoutSync:           1;  /* [25] 1 = Enable vout-vout sync */
    UINT32  VinVoutSync:            1;  /* [26] 1 = Wait for vin sync */
    UINT32  DigitalOutput:          1;  /* [27] 1 = Enable digital output (Display 0 only) */
    UINT32  AnalogOutput:           1;  /* [28] 1 = Enable analog output */
    UINT32  HdmiOutput:             1;  /* [29] 1 = Enable HDMI output (Display 1 only) */
    UINT32  ResetDVE:               1;  /* [30] 1 = DVE software reset */
    UINT32  ResetSection:           1;  /* [31] 1 = Display section software reset */
} AMBA_VOUTD_CTRL_REG_s;
#elif defined(CONFIG_SOC_CV28)
typedef struct {
    UINT32  FixedFormatSelect:      5;  /* [4:0] 0 = Software configurable, 1-12 = Fixed/Standard format */
    UINT32  Interlaced:             1;  /* [5] 0 = Progressive, 1 = Interlaced */
    UINT32  FlipMode:               1;  /* [6] 1 = Enable horizontal flip */
    UINT32  GammaMapping:           1;  /* [7] 1 = Enable gamma mapping (Display 0 only) */
    UINT32  HalfPixShift:           1;  /* [8] 1 = Enable half-pixel shift (Display 0 only) */
    UINT32  HalfPixShiftLines:      1;  /* [9] Apply half-pixel shift to 0 = Odd lines, 1 = Even lines */
    UINT32  Format3D:               2;  /* [11:10] 0 = 2D, 1 = 3D side by side, 2 = 3D top and bottom, 3 = 3D frame packed */
    UINT32  Fpd4thLaneEnable:       1;  /* [12] 1 = Enable 4th output lane in FPD link (Display 0 only)  */
    UINT32  FpdOrderReverse:        1;  /* [13] */
    UINT32  FpdMsbSelect:           1;  /* [14] 0 = LSB on data lane 3(JEIDA), 1 = MSB on data lane 3(VESA) */
    UINT32  Reserved:               4;  /* [18:15] Reserved */
    UINT32  SelectCsi:              1;  /* [19] 0 = Not CSI frame, 1 = CSI frame */
    UINT32  MipiSyncEndEnable:      1;  /* [20] 1 = Enable MIPI sync end */
    UINT32  MipiEotpEnable:         1;  /* [21] 1 = Enable MIPI DSI EoTP */
    UINT32  NumMipiLane:            2;  /* [23:22] 0 = 1 lane, 1 = 2 lanes, 3 = 4lanes */
    UINT32  Reserved1:              1;  /* [24] Reserved */
    UINT32  VoutVoutSync:           1;  /* [25] 1 = Enable vout-vout sync */
    UINT32  VinVoutSync:            1;  /* [26] 1 = Wait for vin sync */
    UINT32  DigitalOutput:          1;  /* [27] 1 = Enable digital output (Display 0 only) */
    UINT32  AnalogOutput:           1;  /* [28] 1 = Enable analog output */
    UINT32  HdmiOutput:             1;  /* [29] 1 = Enable HDMI output (Display 1 only) */
    UINT32  ResetDVE:               1;  /* [30] 1 = DVE software reset */
    UINT32  ResetSection:           1;  /* [31] 1 = Display section software reset */
} AMBA_VOUTD_CTRL_REG_s;
#elif defined(CONFIG_SOC_H32)
typedef struct {
    UINT32  FixedFormatSelect:      5;  /* [4:0] 0 = Software configurable, 1-12 = Fixed/Standard format */
    UINT32  Interlaced:             1;  /* [5] 0 = Progressive, 1 = Interlaced */
    UINT32  FlipMode:               1;  /* [6] 1 = Enable horizontal flip */
    UINT32  GammaMapping:           1;  /* [7] 1 = Enable gamma mapping (Display 0 only) */
    UINT32  HalfPixShift:           1;  /* [8] 1 = Enable half-pixel shift (Display 0 only) */
    UINT32  HalfPixShiftLines:      1;  /* [9] Apply half-pixel shift to 0 = Odd lines, 1 = Even lines */
    UINT32  Format3D:               2;  /* [11:10] 0 = 2D, 1 = 3D side by side, 2 = 3D top and bottom, 3 = 3D frame packed */
    UINT32  Reserved:               13; /* [24:12] Reserved */
    UINT32  VoutVoutSync:           1;  /* [25] 1 = Enable vout-vout sync */
    UINT32  VinVoutSync:            1;  /* [26] 1 = Wait for vin sync */
    UINT32  DigitalOutput:          1;  /* [27] 1 = Enable digital output (Display 0 only) */
    UINT32  AnalogOutput:           1;  /* [28] 1 = Enable analog output (Display 1 only) */
    UINT32  HdmiOutput:             1;  /* [29] 1 = Enable HDMI output (Display 1 only) */
    UINT32  ResetDVE:               1;  /* [30] 1 = DVE software reset (Display 1 only) */
    UINT32  ResetSection:           1;  /* [31] 1 = Display section software reset */
} AMBA_VOUTD_CTRL_REG_s;
#else
typedef struct {
    UINT32  FixedFormatSelect:      5;  /* [4:0] 0 = Software configurable, 1-12 = Fixed/Standard format */
    UINT32  Interlaced:             1;  /* [5] 0 = Progressive, 1 = Interlaced */
    UINT32  FlipMode:               1;  /* [6] 1 = Enable horizontal flip */
    UINT32  GammaMapping:           1;  /* [7] 1 = Enable gamma mapping */
    UINT32  HalfPixShift:           1;  /* [8] 1 = Enable half-pixel shift */
    UINT32  HalfPixShiftLines:      1;  /* [9] Apply half-pixel shift to 0 = Odd lines, 1 = Even lines */
    UINT32  Reserved:               7;  /* [16:10] Reserved */
    UINT32  VirtChanId:             2;  /* [18:17] Virtual Channel ID */
    UINT32  SelectCsi:              1;  /* [19] 0 = Not CSI frame, 1 = CSI frame */
    UINT32  MipiSyncEndEnable:      1;  /* [20] 1 = Enable MIPI sync end */
    UINT32  MipiEotpEnable:         1;  /* [21] 1 = Enable MIPI DSI EoTP */
    UINT32  NumMipiLane:            2;  /* [23:22] 0 = 1 lane, 1 = 2 lanes, 3 = 4lanes */
    UINT32  Reserved1:              1;  /* [24] Reserved */
    UINT32  VoutVoutSync:           1;  /* [25] 1 = Enable vout-vout sync */
    UINT32  VinVoutSync:            1;  /* [26] 1 = Wait for vin sync */
    UINT32  DigitalOutput:          1;  /* [27] 1 = Enable digital output */
    UINT32  Reserved2:              3;  /* [30:28] Reserved */
    UINT32  ResetSection:           1;  /* [31] 1 = Display section software reset */
} AMBA_VOUTD_CTRL_REG_s;
#endif

#if defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)
/*
 * VOUT: Display Section 2 Control Register
 */
typedef struct {
    UINT32  FixedFormatSelect:      5;  /* [4:0] 0 = Software configurable, 1-12 = Fixed/Standard format */
    UINT32  Interlaced:             1;  /* [5] 0 = Progressive, 1 = Interlaced */
    UINT32  FlipMode:               1;  /* [6] 1 = Enable horizontal flip */
    UINT32  Reserved:               17; /* [23:7] Reserved */
    UINT32  Input420:               1;  /* [24] 0 = Disable 420 Input, 1 = Enable 420 Input */
    UINT32  Reserved1:              1;  /* [25] Reserved */
    UINT32  VinVoutSyncEnable:      1;  /* [26] 0 = Disable Vin-Vout sync, 1 = Enable Vin-Vout sync */
    UINT32  Reserved2:              1;  /* [27] Reserved */
    UINT32  AnalogOutput:           1;  /* [28] 0 = Disable analog output, 1 = Enable analog output */
    UINT32  HdmiOutput:             1;  /* [29] 0 = Disable HDMI output, 1 = Enable HDMI output */
    UINT32  SdtvEncReset:           1;  /* [30] 1 = SDTV encoder reset */
    UINT32  ResetSection:           1;  /* [31] 1 = Display section software reset */
} AMBA_VOUTDC_CTRL_REG_s;
#endif

/*
 * VOUT: Display Section 0/1 Status Register
 */
#if defined(CONFIG_SOC_CV2) || defined(CONFIG_SOC_CV22) || defined(CONFIG_SOC_CV25)
typedef struct {
    UINT32  HdmiField:              1;  /* [0] 0 = Odd field, 1 = Even field is outputting */
    UINT32  AnalogField:            1;  /* [1] 0 = Odd field, 1 = Even field is outputting */
    UINT32  DigitalField:           1;  /* [2] 0 = Odd field, 1 = Even field is outputting */
    UINT32  MipiError:              1;  /* [3] */
    UINT32  MipiFifoOverFlow:       1;  /* [4] */
    UINT32  GammaCfgReady:          1;  /* [5] 1 = Gamma table is safe to write (Display 0 only) */
    UINT32  Reserved:               21; /* [26:6] Reserved */
    UINT32  HdmiUnderflow:          1;  /* [27] 1 = HDMI output underflow is detected */
    UINT32  AnalogUnderflow:        1;  /* [28] 1 = Analog output underflow is detected */
    UINT32  DigitalUnderflow:       1;  /* [29] 1 = Digital output underflow is detected */
    UINT32  SdtvCfgReady:           1;  /* [30] 1 = DVE is ready to be reprogrammed */
    UINT32  ResetComplete:          1;  /* [31] 1 = Display section is ready to be reprogrammed */
} AMBA_VOUTD_STATUS_REG_s;
#elif defined(CONFIG_SOC_CV28)
typedef struct {
    UINT32  Reserved:               1;  /* [0] Reserved */
    UINT32  AnalogField:            1;  /* [1] 0 = Odd field, 1 = Even field is outputting */
    UINT32  Reserved1:              2;  /* [3:2] Reserved */
    UINT32  GammaCfgReady:          1;  /* [4] 1 = Gamma table is safe to write (Display 0 only) */
    UINT32  MipiFifoOverflow:       1;  /* [5] 1 = Mipi FIFO overflow has occurred */
    UINT32  WatchdogTimeout:        1;  /* [6] 1 = One frame takes longer to finish than the programmed watchdog timer threshold */
    UINT32  Reserved2:              21; /* [27:7] Reserved */
    UINT32  AnalogUnderflow:        1;  /* [28] 1 = Analog output underflow is detected */
    UINT32  DigitalUnderflow:       1;  /* [29] 1 = Digital output underflow is detected */
    UINT32  Reserved3:              1;  /* [30] Reserved */
    UINT32  ResetComplete:          1;  /* [31] 1 = Display section is ready to be reprogrammed */
} AMBA_VOUTD_STATUS_REG_s;
#elif defined(CONFIG_SOC_H32)
typedef struct {
    UINT32  HdmiField:              1;  /* [0] 0 = Odd field, 1 = Even field is outputting */
    UINT32  AnalogField:            1;  /* [1] 0 = Odd field, 1 = Even field is outputting */
    UINT32  DigitalField:           1;  /* [2] 0 = Odd field, 1 = Even field is outputting */
    UINT32  Reserved:               2;  /* [4:3] Reserved */
    UINT32  GammaCfgReady:          1;  /* [5] 1 = Gamma table is safe to write (Display 0 only) */
    UINT32  Reserved1:              19; /* [24:6] Reserved */
    UINT32  Bt656ShiftedRight:      1;  /* [25] 1 = an unwanted right shift has happened */
    UINT32  Bt656ShiftedLeft:       1;  /* [26] 1 = an unwanted left shift has happened */
    UINT32  HdmiUnderflow:          1;  /* [27] 1 = HDMI output underflow is detected */
    UINT32  AnalogUnderflow:        1;  /* [28] 1 = Analog output underflow is detected */
    UINT32  DigitalUnderflow:       1;  /* [29] 1 = Digital output underflow is detected */
    UINT32  SdtvCfgReady:           1;  /* [30] 1 = DVE is ready to be reprogrammed */
    UINT32  ResetComplete:          1;  /* [31] 1 = Display section is ready to be reprogrammed */
} AMBA_VOUTD_STATUS_REG_s;
#else
typedef struct {
    UINT32  Reserved:               4;  /* [3:0] Reserved */
    UINT32  GammaCfgReady:          1;  /* [4] 1 = Gamma table is safe to write */
    UINT32  MipiFifoOverflow:       1;  /* [5] 1 = Mipi FIFO overflow has occurred */
    UINT32  WatchdogTimeout:        1;  /* [6] 1 = One frame takes longer to finish than the programmed watchdog timer threshold */
    UINT32  Reserved1:              22; /* [28:7] Reserved */
    UINT32  DigitalUnderflow:       1;  /* [29] 1 = Digital output underflow is detected */
    UINT32  Reserved2:              1;  /* [30] Reserved */
    UINT32  ResetComplete:          1;  /* [31] 1 = Display section is ready to be reprogrammed */
} AMBA_VOUTD_STATUS_REG_s;
#endif

#if defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)
/*
 * VOUT: Display Section C Status Register
 */
typedef struct {
    UINT32  HdmiField:              1;  /* [0] 0 = Output odd field, 1 = Output even field */
    UINT32  AnalogField:            1;  /* [1] 0 = Output odd field, 1 = Output even field */
    UINT32  Reserved:               4;  /* [5:2] Reserved */
    UINT32  WatchdogTimeout:        1;  /* [6] One frame takes longer to finish than the programmed watchdog timer threshold*/
    UINT32  Reserved1:              20; /* [26:7] Reserved */
    UINT32  HdmiUnderflow:          1;  /* [27] 1 = HDMI output underflow is detected */
    UINT32  AnalogUnderflow:        1;  /* [28] 1 = Analog output underflow is detected */
    UINT32  Reserved2:              1;  /* [29] Reserved */
    UINT32  SdtvCfgReady:           1;  /* [30] 1 = SDTV configuration ready */
    UINT32  ResetComplete:          1;  /* [31] 1 = Display section is ready to be reprogrammed */
} AMBA_VOUTDC_STATUS_REG_s;
#endif

/*
 * VOUT: Display Section 0/1/2 Output Frame Size Register
 */
typedef struct {
    UINT32  FrameHeightFld0:        14; /* [13:0] Number of lines per frame/field 0 (including blanking) minus 1 */
    UINT32  Reserved:               2;  /* [15:14] Reserved */
    UINT32  FrameWidth:             14; /* [29:16] Number of clock cycles per line (including blanking) minus 1 */
    UINT32  Reserved1:              2;  /* [31:30] Reserved */
} AMBA_VOUTD_FRAME_SIZE_REG_s;

typedef struct {
    UINT32  FrameHeightFld1:        14; /* [13:0] Number of lines per field 1 (including blanking) minus 1 */
    UINT32  Reserved:               18; /* [31:14] Reserved */
} AMBA_VOUTD_FIELD_HEIGHT_REG_s;

/*
 * VOUT: Display Section 0/1/2 Active Region Register
 */
typedef struct {
    UINT32  StartRow:               14; /* [13:0] The first row of active region */
    UINT32  Reserved:               2;  /* [15:14] Reserved */
    UINT32  StartColumn:            14; /* [29:16] The first column of active region */
    UINT32  Reserved1:              2;  /* [31:30] Reserved */
} AMBA_VOUTD_ACTIVE_REGION_START_REG_s;

typedef struct {
    UINT32  EndRow:                 14; /* [13:0] The last row of active region */
    UINT32  Reserved:               2;  /* [15:14] Reserved */
    UINT32  EndColumn:              14; /* [29:16] The last column of active region */
    UINT32  Reserved1:              2;  /* [31:30] Reserved */
} AMBA_VOUTD_ACTIVE_REGION_END_REG_s;

/*
 * VOUT: Display Digital Output Mode Register
 */
#if defined(CONFIG_SOC_CV2) || defined(CONFIG_SOC_CV22) || defined(CONFIG_SOC_CV25) || defined(CONFIG_SOC_CV28)
typedef struct {
    UINT32  HSyncPolarity:          1;  /* [0] 0 = HSync asserted low, 1 = HSync asserted high */
    UINT32  VSyncPolarity:          1;  /* [1] 0 = VSync asserted low, 1 = VSync asserted high */
    UINT32  ClkOutputDivider:       1;  /* [2] 0 = Same as input clock, 1 = Internally divided clock */
    UINT32  ClkDividerEnable:       1;  /* [3] 1 = Enable internal clock divider */
    UINT32  ClkSampleEdge:          1;  /* [4] 0 = Data valid on rising edge, 1 = Data valid on falling edge */
    UINT32  ClkDisable:             1;  /* [5] 1 = Disable digital clock output to reduce EMI */
    UINT32  ClkDivPatternWidth:     7;  /* [12:6] The number of valid bits in clock pattern minus 1 */
    UINT32  MipiLineSync:           1;  /* [13] 1 = Enable MIPI line sync packets (set to 1 in DSI ) */
    UINT32  MipiLineCount:          1;  /* [14] 1 = Enable MIPI line number increments in line sync packets (always to 0 in DSI) */
    UINT32  MipiFrameCount:         1;  /* [15] 1 = Enable MIPI frame number increments in frame sync packets (always to 0 in DSI) */
    UINT32  MipiLineDataFrameBlank: 1;  /* [16] 1 = Enable MIPI line data packets sent in frame blanking (always to 0 in DSI) */
    UINT32  MipiLineSyncFrameBlank: 1;  /* [17] 1 = Enable MIPI line sync packets sent in frame blanking (always to 1 in DSI) */
    UINT32  MipiEccOrder:           1;  /* [18] Set to 0 */
    UINT32  HvldPolarity:           1;  /* [19] 0 = Pin VD1_HVLD asserted low, 1 = Pin VD1_HVLD asserted high */
    UINT32  Reserved:               1;  /* [20] Reserved */
    UINT32  ColorSeqEvenLines:      3;  /* [23:21] Set color output sequence for the even lines of the frame */
    UINT32  ColorSeqOddLines:       3;  /* [26:24] Set color output sequence for the odd lines of the frame */
    UINT32  OutputMode:             4;  /* [30:27] 0-3 = LCD modes, 4 = 656 output, 5-7 = 601 output modes, 8 = Bayer pattern, 9-14 = MIPI modes, 15 = FPD-LINK */
    UINT32  Reserved1:              1;  /* [31] Reserved */
} AMBA_VOUTD_DIGITAL_OUTPUT_MODE_REG_s;
#elif defined(CONFIG_SOC_H32)
typedef struct {
    UINT32  HSyncPolarity:          1;  /* [0] 0 = HSync asserted low, 1 = HSync asserted high */
    UINT32  VSyncPolarity:          1;  /* [1] 0 = VSync asserted low, 1 = VSync asserted high */
    UINT32  ClkOutputDivider:       1;  /* [2] 0 = Same as input clock, 1 = Internally divided clock */
    UINT32  ClkDividerEnable:       1;  /* [3] 1 = Enable internal clock divider */
    UINT32  ClkSampleEdge:          1;  /* [4] 0 = Data valid on rising edge, 1 = Data valid on falling edge */
    UINT32  ClkDisable:             1;  /* [5] 1 = Disable digital clock output to reduce EMI */
    UINT32  ClkDivPatternWidth:     7;  /* [12:6] The number of valid bits in clock pattern minus 1 */
    UINT32  Reserved:               6;  /* [18:13] Reserved */
    UINT32  HvldPolarity:           1;  /* [19] 0 = Pin VD1_HVLD asserted low, 1 = Pin VD1_HVLD asserted high */
    UINT32  Reserved1:              1;  /* [20] Reserved */
    UINT32  ColorSeqEvenLines:      3;  /* [23:21] Set color output sequence for the even lines of the frame */
    UINT32  ColorSeqOddLines:       3;  /* [26:24] Set color output sequence for the odd lines of the frame */
    UINT32  OutputMode:             4;  /* [30:27] 0-3 = LCD modes, 4 = 656 output mode, 5-7 = 601 output modes */
    UINT32  Reserved2:              1;  /* [31] Reserved */
} AMBA_VOUTD_DIGITAL_OUTPUT_MODE_REG_s;
#else
typedef struct {
    UINT32  HSyncPolarity:          1;  /* [0] 0 = HSync asserted low, 1 = HSync asserted high */
    UINT32  VSyncPolarity:          1;  /* [1] 0 = VSync asserted low, 1 = VSync asserted high */
    UINT32  Reserved:               11; /* [12:2] Reserved */
    UINT32  MipiLineSync:           1;  /* [13] 1 = Enable MIPI line sync packets (set to 1 in DSI ) */
    UINT32  MipiLineCount:          1;  /* [14] 1 = Enable MIPI line number increments in line sync packets (always to 0 in DSI) */
    UINT32  MipiFrameCount:         1;  /* [15] 1 = Enable MIPI frame number increments in frame sync packets (always to 0 in DSI) */
    UINT32  MipiLineDataFrameBlank: 1;  /* [16] 1 = Enable MIPI line data packets sent in frame blanking (always to 0 in DSI) */
    UINT32  MipiLineSyncFrameBlank: 1;  /* [17] 1 = Enable MIPI line sync packets sent in frame blanking (always to 1 in DSI) */
    UINT32  MipiEccOrder:           1;  /* [18] Set to 0 */
    UINT32  HvldPolarity:           1;  /* [19] 0 = Pin VD1_HVLD asserted low, 1 = Pin VD1_HVLD asserted high */
    UINT32  Reserved1:              1;  /* [20] Reserved */
    UINT32  ColorSeqEvenLines:      3;  /* [23:21] Set color output sequence for the even lines of the frame */
    UINT32  ColorSeqOddLines:       3;  /* [26:24] Set color output sequence for the odd lines of the frame */
    UINT32  OutputMode:             4;  /* [30:27] 0-8 = Reserved, 9-14 = MIPI modes, 15 = FPD-LINK */
    UINT32  Reserved2:              1;  /* [31] Reserved */
} AMBA_VOUTD_DIGITAL_OUTPUT_MODE_REG_s;
#endif

/*
 * VOUT: Display HSync Control Register
 */
typedef struct {
    UINT32  EndColumn:              14; /* [13:0] Last column of HSync */
    UINT32  Reserved:               2;  /* [15:14] Reserved */
    UINT32  StartColumn:            14; /* [29:16] First column of HSync */
    UINT32  Reserved1:              2;  /* [31:30] Reserved */
} AMBA_VOUTD_HSYNC_CTRL_REG_s;

/*
 * VOUT: Display VSync Start Register
 */
typedef struct {
    UINT32  StartRow:               14; /* [13:0] First row of VSync */
    UINT32  Reserved:               2;  /* [15:14] Reserved */
    UINT32  StartColumn:            14; /* [29:16] First column of VSync */
    UINT32  Reserved1:              2;  /* [31:30] Reserved */
} AMBA_VOUTD_VSYNC_START_REG_s;

/*
 * VOUT: Display VSync End Register
 */
typedef struct {
    UINT32  EndRow:                 14; /* [13:0] Last row of VSync */
    UINT32  Reserved:               2;  /* [15:14] Reserved */
    UINT32  EndColumn:              14; /* [29:16] Last column of VSync */
    UINT32  Reserved1:              2;  /* [31:30] Reserved */
} AMBA_VOUTD_VSYNC_END_REG_s;

/*
 * VOUT: Display BT.656 Digital Field Blanking Interval Register
 */
typedef struct {
    UINT32  EndRow:                 14; /* [13:0] First row of digital field blanking (V-bit) */
    UINT32  Reserved:               2;  /* [15:14] Reserved */
    UINT32  StartRow:               14; /* [29:16] Last column of digital field blanking (V-bit) */
    UINT32  Reserved1:              2;  /* [31:30] Reserved */
} AMBA_VOUTD_BT656_VBIT_REG_s;

/*
 * VOUT: Display BT.656 SAV Start Location Register
 */
typedef struct {
    UINT32  StartLocation:          14; /* [13:0] Horizontal start location of SAV code */
    UINT32  Reserved:               18; /* [31:14] Reserved */
} AMBA_VOUTD_BT656_SAV_REG_s;

/*
 * VOUT: Display Digital/HDMI Color Space Converter Parameter Register
 */
typedef struct {
    UINT32  CoefA0:                 13; /* [12:0] Signed 2.10 bits */
    UINT32  Reserved:               3;  /* [15:13] Reserved */
    UINT32  CoefA1:                 13; /* [28:16] Signed 2.10 bits */
    UINT32  Reserved1:              3;  /* [31:29] Reserved */
} AMBA_VOUTD_DIGITAL_CSC_PARAM_0_REG_s;

typedef struct {
    UINT32  CoefA2:                 13; /* [12:0] Signed 2.10 bits */
    UINT32  Reserved:               3;  /* [15:13] Reserved */
    UINT32  CoefA3:                 13; /* [28:16] Signed 2.10 bits */
    UINT32  Reserved1:              3;  /* [31:29] Reserved */
} AMBA_VOUTD_DIGITAL_CSC_PARAM_1_REG_s;

typedef struct {
    UINT32  CoefA4:                 13; /* [12:0] Signed 2.10 bits */
    UINT32  Reserved:               3;  /* [15:13] Reserved */
    UINT32  CoefA5:                 13; /* [28:16] Signed 2.10 bits */
    UINT32  Reserved1:              3;  /* [31:29] Reserved */
} AMBA_VOUTD_DIGITAL_CSC_PARAM_2_REG_s;

typedef struct {
    UINT32  CoefA6:                 13; /* [12:0] Signed 2.10 bits */
    UINT32  Reserved:               3;  /* [15:13] Reserved */
    UINT32  CoefA7:                 13; /* [28:16] Signed 2.10 bits */
    UINT32  Reserved1:              3;  /* [31:29] Reserved */
} AMBA_VOUTD_DIGITAL_CSC_PARAM_3_REG_s;

typedef struct {
    UINT32  CoefA8:                 13; /* [12:0] Signed 2.10 bits */
    UINT32  Reserved:               3;  /* [15:13] Reserved */
    UINT32  ConstB0:                15; /* [30:16] Signed 10 bits */
    UINT32  Reserved1:              1;  /* [31] Reserved */
} AMBA_VOUTD_DIGITAL_CSC_PARAM_4_REG_s;

typedef struct {
    UINT32  ConstB1:                15; /* [14:0] Signed 10 bits */
    UINT32  Reserved:               1;  /* [15] Reserved */
    UINT32  ConstB2:                15; /* [30:16] Signed 10 bits */
    UINT32  Reserved1:              1;  /* [31] Reserved */
} AMBA_VOUTD_DIGITAL_CSC_PARAM_5_REG_s;

typedef struct {
    UINT32  Output0ClampLow:        12; /* [11:0] The minimum allowed value for color component 0 */
    UINT32  Reserved:               4;  /* [15:12] Reserved */
    UINT32  Output0ClampHigh:       12; /* [27:16] The maxiumum allowed value for color component 0 */
    UINT32  Reserved1:              4;  /* [31:28] Reserved */
} AMBA_VOUTD_DIGITAL_CSC_PARAM_6_REG_s;

typedef struct {
    UINT32  Output1ClampLow:        12; /* [11:0] The minimum allowed value for color component 1 */
    UINT32  Reserved:               4;  /* [15:12] Reserved */
    UINT32  Output1ClampHigh:       12; /* [27:16] The maxiumum allowed value for color component 1 */
    UINT32  Reserved1:              4;  /* [31:28] Reserved */
} AMBA_VOUTD_DIGITAL_CSC_PARAM_7_REG_s;

typedef struct {
    UINT32  Output2ClampLow:        12; /* [11:0] The minimum allowed value for color component 2 */
    UINT32  Reserved:               4;  /* [15:12] Reserved */
    UINT32  Output2ClampHigh:       12; /* [27:16] The maxiumum allowed value for color component 2 */
    UINT32  Reserved1:              4;  /* [31:28] Reserved */
} AMBA_VOUTD_DIGITAL_CSC_PARAM_8_REG_s;

/*
 * VOUT: Display Analog Output Mode Register
 */
typedef struct {
    UINT32  HSyncPolarity:          1;  /* [0] 0 = HSync asserted low, 1 = HSync asserted high */
    UINT32  VSyncPolarity:          1;  /* [1] 0 = VSync asserted low, 1 = VSync asserted high */
    UINT32  Reserved:               30; /* [31:2] Reserved */
} AMBA_VOUTD_ANALOG_OUTPUT_MODE_REG_s;

/*
 * VOUT: Display Analog Vertical Blanking Information Control Register
 */
typedef struct {
    UINT32  ZeroLevel:              10; /* [9:0] Value to output when VBI bit is 0 */
    UINT32  OneLevel:               10; /* [19:10] Value to output when VBI bit is 1 */
    UINT32  RepeatCount:            7;  /* [26:20] 0 = Disable VBI, 1-127 = Output each VBI bit for n clocks */
    UINT32  SignalType:             1;  /* [27] 0 = HD mode/SD composite mode, 1 = SD component mode */
    UINT32  Reserved:               4;  /* [31:28] Reserved */
} AMBA_VOUTD_ANALOG_VBI_CTRL_REG_s;

/*
 * VOUT: Display Analog VBI Region Location Register
 */
typedef struct {
    UINT32  StartRowField0:         14; /* [13:0] Row to begin VBI insertion */
    UINT32  Reserved:               2;  /* [15:14] Reserved */
    UINT32  StartRowField1:         14; /* [29:16] Row to begin VBI insertion */
    UINT32  Reserved1:              2;  /* [31:30] Reserved */
} AMBA_VOUTD_ANALOG_VBI_START_V_REG_s;

typedef struct {
    UINT32  EndColumn:              14; /* [13:0] Column to end VBI insertion */
    UINT32  Reserved:               2;  /* [15:14] Reserved */
    UINT32  StartColumn:            14; /* [29:16] Column to begin VBI insertion */
    UINT32  Reserved1:              2;  /* [31:30] Reserved */
} AMBA_VOUTD_ANALOG_VBI_H_REG_s;

/*
 * VOUT: Display Analog Color Space Converter Parameter Register
 */
typedef struct {
    UINT32  CoefA0:                 13; /* [12:0] Signed 4.8 bits for HD output, Signed 2.10 bits for SD output */
    UINT32  Reserved:               3;  /* [15:13] Reserved */
    UINT32  CoefA4:                 13; /* [28:16] Signed 4.8 bits for HD output, Signed 2.10 bits for SD output */
    UINT32  Reserved1:              3;  /* [31:29] Reserved */
} AMBA_VOUTD_ANALOG_CSC_PARAM_0_REG_s;

typedef struct {
    UINT32  CoefA8:                 13; /* [12:0] Signed 4.8 bits for HD output, Signed 2.10 bits for SD output */
    UINT32  Reserved:               3;  /* [15:13] Reserved */
    UINT32  ConstB0:                15; /* [30:16] Signed 12 bits for HD output, Signed 10 bits otherwise */
    UINT32  Reserved1:              1;  /* [31] Reserved */
} AMBA_VOUTD_ANALOG_CSC_PARAM_1_REG_s;

typedef struct {
    UINT32  ConstB1:                15; /* [14:0] Signed 12 bits for HD output, Signed 10 bits otherwise */
    UINT32  Reserved:               1;  /* [15] Reserved */
    UINT32  ConstB2:                15; /* [30:16] Signed 12 bits for HD output, Signed 10 bits otherwise */
    UINT32  Reserved1:              1;  /* [31] Reserved */
} AMBA_VOUTD_ANALOG_CSC_PARAM_2_REG_s;

#if defined(CONFIG_SOC_CV2) || defined(CONFIG_SOC_CV22) || defined(CONFIG_SOC_CV25) || defined(CONFIG_SOC_H32)
typedef struct {
    UINT32  Output0ClampLow:        10; /* [9:0] The minimum allowed value for color component 0 */
    UINT32  Reserved:               6;  /* [15:10] Reserved */
    UINT32  Output0ClampHigh:       10; /* [25:16] The maxiumum allowed value for color component 0 */
    UINT32  Reserved1:              6;  /* [31:26] Reserved */
} AMBA_VOUTD_ANALOG_CSC_PARAM_3_REG_s;

typedef struct {
    UINT32  Output1ClampLow:        10; /* [9:0] The minimum allowed value for color component 1 */
    UINT32  Reserved:               6;  /* [15:10] Reserved */
    UINT32  Output1ClampHigh:       10; /* [25:16] The maxiumum allowed value for color component 1 */
    UINT32  Reserved1:              6;  /* [31:26] Reserved */
} AMBA_VOUTD_ANALOG_CSC_PARAM_4_REG_s;

typedef struct {
    UINT32  Output2ClampLow:        10; /* [9:0] The minimum allowed value for color component 2 */
    UINT32  Reserved:               6;  /* [15:10] Reserved */
    UINT32  Output2ClampHigh:       10; /* [25:16] The maxiumum allowed value for color component 2 */
    UINT32  Reserved1:              6;  /* [31:26] Reserved */
} AMBA_VOUTD_ANALOG_CSC_PARAM_5_REG_s;
#else
typedef struct {
    UINT32  Output0ClampLow:        12; /* [11:0] The minimum allowed value for color component 0 */
    UINT32  Reserved:               4;  /* [15:12] Reserved */
    UINT32  Output0ClampHigh:       12; /* [27:16] The maxiumum allowed value for color component 0 */
    UINT32  Reserved1:              4;  /* [31:28] Reserved */
} AMBA_VOUTD_ANALOG_CSC_PARAM_3_REG_s;

typedef struct {
    UINT32  Output1ClampLow:        12; /* [11:0] The minimum allowed value for color component 1 */
    UINT32  Reserved:               4;  /* [15:12] Reserved */
    UINT32  Output1ClampHigh:       12; /* [27:16] The maxiumum allowed value for color component 1 */
    UINT32  Reserved1:              4;  /* [31:28] Reserved */
} AMBA_VOUTD_ANALOG_CSC_PARAM_4_REG_s;

typedef struct {
    UINT32  Output2ClampLow:        12; /* [11:0] The minimum allowed value for color component 2 */
    UINT32  Reserved:               4;  /* [15:12] Reserved */
    UINT32  Output2ClampHigh:       12; /* [27:16] The maxiumum allowed value for color component 2 */
    UINT32  Reserved1:              4;  /* [31:28] Reserved */
} AMBA_VOUTD_ANALOG_CSC_PARAM_5_REG_s;
#endif

typedef struct {
    UINT32  Reserved:               16; /* [15:0] Reserved */
    UINT32  CoefA4:                 13; /* [28:16] Signed 2.10 bits */
    UINT32  Reserved1:              3;  /* [31:29] Reserved */
} AMBA_VOUTD_ANALOG_CSC_2_PARAM_0_REG_s;

typedef struct {
    UINT32  ConstB1:                15; /* [14:0] Signed 12 bits */
    UINT32  Reserved:               17; /* [31:15] Reserved */
} AMBA_VOUTD_ANALOG_CSC_2_PARAM_2_REG_s;

typedef struct {
    UINT32  Output1ClampLow:        10; /* [9:0] The minimum allowed value for color component 1 */
    UINT32  Reserved:               6;  /* [15:10] Reserved */
    UINT32  Output1ClampHigh:       10; /* [25:16] The maxiumum allowed value for color component 1 */
    UINT32  Reserved1:              6;  /* [31:26] Reserved */
} AMBA_VOUTD_ANALOG_CSC_2_PARAM_4_REG_s;

/*
 * VOUT: Display HDMI Output Mode Register
 */
typedef struct {
    UINT32  HSyncPolarity:          1;  /* [0] 0 = HSync asserted low, 1 = HSync asserted high */
    UINT32  VSyncPolarity:          1;  /* [1] 0 = VSync asserted low, 1 = VSync asserted high */
    UINT32  Reserved:               27; /* [28:2] Reserved */
    UINT32  OutputMode:             2;  /* [30:29] 0 = YCbCr 4:4:4 8-bit, 1 = RGB 24-bit, 2 = YCbCr 4:2:2 12-bit, 3 = YCbCr 4:2:0 8-bit */
    UINT32  Reserved1:              1;  /* [31] Reserved */
} AMBA_VOUTD_HDMI_OUTPUT_MODE_REG_s;

/*
 * VOUT: Display Dithering Control Register
 */
typedef struct {
    UINT32  Enable:                 1;  /* [0] 1 = Enable dithering */
    UINT32  Format:                 2;  /* [2:1] Dither term, 0 = 0.4, 1 = Seed+0.4, 2 = Seed+1.4, 3 = Seed+2.4 */
    UINT32  Reserved:               29; /* [31:3] Reserved */
} AMBA_VOUTD_DITHER_CTRL_REG_s;

/*
 * VOUT: Display Section Synchronization Register
 */
typedef struct {
    UINT32  StartRow:               14; /* [13:0] Row on which to start running Display Section */
    UINT32  Reserved:               2;  /* [15:14] Reserved */
    UINT32  FieldSelect:            1;  /* [16] Field on which to start running Display Section */
    UINT32  Reserved1:              15; /* [31:17] Reserved */
} AMBA_VOUTD_VOUT_VOUT_SYNC_REG_s;

/*
 * VOUT: Display Section Input config Register
 */
#if defined(CONFIG_SOC_CV2) || defined(CONFIG_SOC_CV22) || defined(CONFIG_SOC_CV25)
typedef struct {
    UINT32  SmemInput:              1;  /* [0] 0 = Disable SMEM input, input from Mixer, 1 = Enable SMEM input */
    UINT32  ColorRes:               1;  /* [1] 0 = YUV422 input, 1 = YUV444 or RGB input */
    UINT32  AudioEn:                1;  /* [2] 1 = Enable SLVS audio output (should only be set in Display B SLVS mode */
    UINT32  Reserved:               29; /* [31:3] Reserved */
} AMBA_VOUTD_INPUT_CONFIG_REG_s;
#else
typedef struct {
    UINT32  SmemInput:              1;  /* [0] 0 = Disable SMEM input, input from Mixer, 1 = Enable SMEM input */
    UINT32  ColorRes:               1;  /* [1] 0 = YUV422 input, 1 = YUV444/YUV420 or RGB input */
    UINT32  Reserved:               30; /* [31:2] Reserved */
} AMBA_VOUTD_INPUT_CONFIG_REG_s;
#endif

/*
 * OSD Rescale: Filter Control Register
 */
#if defined(CONFIG_SOC_CV2) || defined(CONFIG_SOC_CV22) || defined(CONFIG_SOC_CV25) || defined(CONFIG_SOC_CV28) || defined(CONFIG_SOC_H32)
typedef struct {
    UINT32  Enable:                 1;  /* [0] Enable both horizontal and vertical resampling */
    UINT32  VCoefShift:             1;  /* [1] Number format of vertical coefficient is: 0 = S0.7, 1 = S1.6 */
    UINT32  Reserved:               1;  /* [2] Reserved */
    UINT32  HCoefShift:             1;  /* [3] Number format of horizontal coefficient is: 0 = S0.7, 1 = S1.6 */
    UINT32  InputSelect:            2;  /* [5:4] 0 = Mixer A CLUT, 1 = Mixer A Direct, 2 = Mixer B CLUT, 3 = Mixer B Direct */
    UINT32  Reserved1:              26; /* [31:6] Reserved */
} AMBA_OSD_RESCALE_CTRL_REG_s;
#else
typedef struct {
    UINT32  Enable:                 1;  /* [0] Enable both horizontal and vertical resampling */
    UINT32  VCoefShift:             1;  /* [1] Number format of vertical coefficient is: 0 = S0.7, 1 = S1.6 */
    UINT32  Reserved:               1;  /* [2] Reserved */
    UINT32  HCoefShift:             1;  /* [3] Number format of horizontal coefficient is: 0 = S0.7, 1 = S1.6 */
    UINT32  InputSelect:            2;  /* [5:4] 0 = Mixer A CLUT, 1 = Mixer A Direct, 2 = Mixer B CLUT, 3 = Mixer B Direct */
    UINT32  Reserved1:              4;  /* [9:6] Reserved */
    UINT32  InputHeight:            11; /* [20:10] Number of line to rescale minus 1 */
    UINT32  InputWidth:             11; /* [31:21] Number of pixel to rescale minus 1 */
} AMBA_OSD_RESCALE_CTRL_REG_s;
#endif

/*
 * OSD Rescale: Filter Output Size Register
 */
typedef struct {
    UINT32  SizeMinus1:             14; /* [13:0] Horizontal/Vertical resampled size minus 1 */
    UINT32  Reserved:               18; /* [31:14] Reserved */
} AMBA_OSD_RESCALE_OUTPUT_SIZE_REG_s;

/*
 * OSD Rescale: Filter Phase Increment Register
 */
typedef struct {
    UINT32  PhaseInc:               16; /* [15:0] Horizontal/Vertical reasmping filter phase increment */
    UINT32  Reserved:               16; /* [31:16] Reserved */
} AMBA_OSD_RESCALE_PHASE_INC_REG_s;

/*
 * OSD Rescale: Filter Initial Phase Register
 */
typedef struct {
    UINT32  InitPhase:              28; /* [27:0] Horizontal/Vertical reasmping filter initial phase */
    UINT32  Reserved:               4;  /* [31:28] Reserved */
} AMBA_OSD_RESCALE_INIT_PHASE_REG_s;

/*
 * OSD Rescale: Filter Coefficient 0-3 Register
 */
typedef struct {
    UINT32  Coef0:                  8;  /* [7:0] Horizontal/Vertical reasmping filter tap coefficient 0 */
    UINT32  Coef1:                  8;  /* [15:8] Horizontal/Vertical reasmping filter tap coefficient 1 */
    UINT32  Coef2:                  8;  /* [23:16] Horizontal/Vertical reasmping filter tap coefficient 2 */
    UINT32  Coef3:                  8;  /* [31:24] Horizontal/Vertical reasmping filter tap coefficient 3 */
} AMBA_OSD_RESCALE_COEF_0_3_REG_s;

/*
 * OSD Rescale: Filter Coefficient 4-5 Register
 */
typedef struct {
    UINT32  Coef4:                  8;  /* [7:0] Horizontal/Vertical reasmping filter tap coefficient 4 */
    UINT32  Coef5:                  8;  /* [15:8] Horizontal/Vertical reasmping filter tap coefficient 5 */
    UINT32  Reserved:               16; /* [31:16] Reserved */
} AMBA_OSD_RESCALE_COEF_4_5_REG_s;

/*
 * OSD Rescale: Filter Dithering Control Register
 */
typedef struct {
    UINT32  Enable:                 1;  /* [0] 1 = Enable horizontal/vertical dithering */
    UINT32  AlphaEnable:            1;  /* [1] 1 = Dither Alpha, 0 = Round Alpha (no dithering) */
    UINT32  Format:                 2;  /* [3:2] Dither term, 0 = 0.4, 1 = Seed+0.4, 2 = Seed+1.4, 3 = Seed+2.4 */
    UINT32  Reserved:               28; /* [31:4] Reserved */
} AMBA_OSD_RESCALE_DITHER_CTRL_REG_s;

/*
 * OSD Rescale: Filter Dithering Seed Register
 */
typedef struct {
    UINT32  Format:                 31; /* [30:0] Random Seed for Dithering */
    UINT32  Reserved:               1;  /* [31] Reserved */
} AMBA_OSD_RESCALE_DITHER_SEED_REG_s;

#if !defined(CONFIG_SOC_H32)
/*
 * VOUT: MIPI Blank Control 0/1 Register
 */
typedef struct {
    UINT32  PayloadBytes0:          14; /* [13:0] Number of bytes in the payload of hbp/hfp blanking packet */
    UINT32  UseBlank:               1;  /* [14] 1 = Use blanking packet at back/front porch */
    UINT32  Reserved:               1;  /* [15] Reserved */
    UINT32  PayloadBytes1:          14; /* [29:16] Number of bytes in the payload of sync/vblank blanking packet */
    UINT32  SyncUseBlank:           1;  /* [30] 1 = use blanking packet at/after sync position */
    UINT32  Reserved1:              1;  /* [31] Reserved */
} AMBA_VOUTD_MIPI_BLANK_CTRL_REG_s;

/*
 * VOUT: Display0 MIPI Command Header Register
 */
typedef struct {
    UINT32  CommandHeader1:         8;  /* [7:0] For short DCS command: parameters; For short other command: parameter 1; For long DCS command: wc_1  */
    UINT32  CommandHeader0:         8;  /* [15:8] For short DCS command: command type hex; For short other command: parameter 0; For long DCS command: wc_0 */
    UINT32  DataIdentifier:         6;  /* [21:16] The data identifier of the sending command */
    UINT32  Reserved:               2;  /* [23:22] Reserved */
    UINT32  DcsLongType:            8;  /* [31:24] First byte (command type) in payload when sending DCS long command */
} AMBA_VOUTD_MIPI_COMMAND_HEADER_s;

/*
 * VOUT: Display0 MIPI Command Parameter Register
 */
typedef struct {
    UINT32  LongByte0:              8;  /* [7:0] Payload byte 0/4/8/12 */
    UINT32  LongByte1:              8;  /* [15:8] Payload byte 1/5/9/13 */
    UINT32  LongByte2:              8;  /* [23:16] Payload byte 2/6/10/14 */
    UINT32  LongByte3:              8;  /* [31:24] Payload byte 3/7/11/15 */
} AMBA_VOUTD_MIPI_COMMAND_PARA_s;

/*
 * VOUT: Display0 MIPI Command Control Register
 */
typedef struct {
    UINT32  LineNumber:             16; /* [15:0] When Timing = 1, this number represents at how many lines after vsync, the command will be sent */
    UINT32  SendTiming:             2;  /* [17:16] 0 = Send command at next available time, 1 = Send during Vblank, LineNumber lines after vsync start */
    UINT32  Type:                   2;  /* [19:18] 0 = Short packet command, 1 = Normal long packet command, 2 = DCS long packet command */
    UINT32  Mode:                   1;  /* [20] 0 = Low power mode, 1 = High speed mode */
    UINT32  Reserved:               11; /* [31:21] Reserved */
} AMBA_VOUTD_MIPI_COMMAND_CTRL_s;
#endif

/*
 * VOUT: TV Encoder Configuration Registers
 */
typedef struct {
    volatile UINT32     Reserved0[32];              /* 0x000-0x07C: Reserved */
    volatile UINT32     TvEncReg32;                 /* 0x080: TV Encoder Phase Increment for Fsc generation ROM (Byte 0) Register */
    volatile UINT32     TvEncReg33;                 /* 0x084: TV Encoder Phase Increment for Fsc generation ROM (Byte 1) Register */
    volatile UINT32     TvEncReg34;                 /* 0x088: TV Encoder Phase Increment for Fsc generation ROM (Byte 2) Register */
    volatile UINT32     TvEncReg35;                 /* 0x08C: TV Encoder Phase Increment for Fsc generation ROM (Byte 3) Register */
    volatile UINT32     TvEncReg36;                 /* 0x090: TV Encoder Subcarrier to Horizontal Phase Offset Adjust Register */
    volatile UINT32     TvEncReg37;                 /* 0x094: TV Encoder Subcarrier to Horizontal Phase Offset Adjust Register */
    volatile UINT32     TvEncReg38;                 /* 0x098: TV Encoder Subcarrier to Horizontal Phase Offset Adjust Register */
    volatile UINT32     TvEncReg39;                 /* 0x09C: TV Encoder Subcarrier to Horizontal Phase Offset Adjust Register */
    volatile UINT32     TvEncReg40;                 /* 0x0A0: TV Encoder Control Register */
    volatile UINT32     Reserved1;                  /* 0x0A4: Reserved */
    volatile UINT32     TvEncReg42;                 /* 0x0A8: TV Encoder Black Level Adjust Regiser */
    volatile UINT32     TvEncReg43;                 /* 0x0AC: TV Encoder Blank Level Adjust Regiser */
    volatile UINT32     TvEncReg44;                 /* 0x0B0: TV Encoder Clamp Incoming Black Level To 0 Prior To Gain Adjustment Regiser */
    volatile UINT32     TvEncReg45;                 /* 0x0B4: TV Encoder Programmable Sync Level Regiser */
    volatile UINT32     TvEncReg46;                 /* 0x0B8: TV Encoder Luma Mode Regiser */
    volatile UINT32     TvEncReg47;                 /* 0x0BC: TV Encoder DAC Control Regiser */
    volatile UINT32     Reserved2[2];               /* 0x0C0-0x0C4: Reserved */
    volatile UINT32     TvEncReg50;                 /* 0x0C8: TV Encoder Negative Burst Amplitude Value for NTSC Regiser */
    volatile UINT32     TvEncReg51;                 /* 0x0CC: TV Encoder Positive Burst Amplitude Value for PAL to obtain +/- 135 Degree Phase Regiser */
    volatile UINT32     TvEncReg52;                 /* 0x0D0: TV Encoder Chroma Mode Regiser */
    volatile UINT32     Reserved3[3];               /* 0x0D4-0x0DC: Reserved */
    volatile UINT32     TvEncReg56;                 /* 0x0E0: TV Encoder Sync Mode Regiser */
    volatile UINT32     TvEncReg57;                 /* 0x0E4: TV Encoder Vertical Sync Offset (MSB) Regiser */
    volatile UINT32     TvEncReg58;                 /* 0x0E8: TV Encoder Vertical Sync Offset (LSB) Regiser */
    volatile UINT32     TvEncReg59;                 /* 0x0EC: TV Encoder Horizontal Sync Offset (MSB) Regiser */
    volatile UINT32     TvEncReg60;                 /* 0x0F0: TV Encoder Horizontal Sync Offset (LSB) Regiser */
    volatile UINT32     TvEncReg61;                 /* 0x0F4: TV Encoder Horizontal Half-Line Pixel Count (MSB) Regiser */
    volatile UINT32     TvEncReg62;                 /* 0x0F8: TV Encoder Horizontal Half-Line Pixel Count (LSB) Regiser */
    volatile UINT32     Reserved4[2];               /* 0x0FC-0x100: Reserved */
    volatile UINT32     TvEncReg65;                 /* 0x104: TV Encoder Odd Field Closed Captioned Data (MSB) Regiser */
    volatile UINT32     TvEncReg66;                 /* 0x108: TV Encoder Odd Field Closed Captioned Data (LSB) Regiser */
    volatile UINT32     TvEncReg67;                 /* 0x10C: TV Encoder Even Field Closed Captioned Data (MSB) Regiser */
    volatile UINT32     TvEncReg68;                 /* 0x110: TV Encoder Even Field Closed Captioned Data (LSB) Regiser */
    volatile UINT32     TvEncReg69;                 /* 0x114: TV Encoder Closed Caption Control Regiser */
    volatile UINT32     Reserved5[26];              /* 0x118-0x17C: Reserved */
    volatile UINT32     TvEncReg96;                 /* 0x180: TV Encoder SIN/COS ROM Test Control Regiser */
    volatile UINT32     TvEncReg97;                 /* 0x184: TV Encoder Luma Test Control Regiser */
    volatile UINT32     Reserved6;                  /* 0x188: Reserved */
    volatile UINT32     TvEncReg99;                 /* 0x18C: TV Encoder Croma Test Control Regiser */
    volatile UINT32     Reserved7[20];              /* 0x190-0x1DC: Reserved */
    volatile UINT32     TvEncReg120;                /* 0x1E0: TV Encoder Horizontal Active Start Register */
    volatile UINT32     TvEncReg121;                /* 0x1E4: TV Encoder Horizontal Active End Register */
    volatile UINT32     Reserved8[6];               /* 0x1E8-0x1FC: Reserved */
} AMBA_VOUT_TVENC_CONTROL_REG_s;

/*
 * VOUT: Mixer Registers
 */
typedef struct {
    volatile AMBA_VOUTM_CTRL_REG_s                  Ctrl;                       /* 0x200/0x500(RW): Mixer Section 0/1 Control Register */
    volatile AMBA_VOUTM_STATUS_REG_s                Status;                     /* 0x204/0x504(RO): Mixer Section 0/1 Status Register */
    volatile AMBA_VOUTM_ACTIVE_SIZE_REG_s           ActiveSize;                 /* 0x208/0x508(RW): Mixer Section 0/1 Active Region Size Register */
    volatile AMBA_VOUTM_REGION_LOCATION_REG_s       VideoWinStart;              /* 0x20C/0x50C(RW): Mixer Section 0/1 Video Region Start Location Register */
    volatile AMBA_VOUTM_REGION_LOCATION_REG_s       VideoWinEnd;                /* 0x210/0x510(RW): Mixer Section 0/1 Video Region End Location Register */
    volatile UINT32                                 Reserved[2];                /* Reserved */
    volatile AMBA_VOUTM_REGION_LOCATION_REG_s       OsdWinStart;                /* 0x21C/0x51C(RW): Mixer Section 0/1 OSD Region Start Location Register */
    volatile AMBA_VOUTM_REGION_LOCATION_REG_s       OsdWinEnd;                  /* 0x220/0x520(RW): Mixer Section 0/1 OSD Region End Location Register */
    volatile AMBA_VOUT_COLOR_REG_s                  BackgroundColor;            /* 0x224/0x524(RW): Mixer Section 0/1 Background Color Register */
    volatile AMBA_VOUTM_HIGHLIGHT_REG_s             HighlightThreshold;         /* 0x228/0x528(RW): Mixer Section 0/1 Highlight Threshold Register */
    volatile AMBA_VOUTM_OSD_CTRL_REG_s              OsdCtrl;                    /* 0x22C/0x52C(RW): Mixer Section 0/1 OSD Control Register */
    volatile AMBA_VOUTM_CSC_PARAM_0_REG_s           OsdCSC0;                    /* 0x230/0x530(WO): Mixer Section 0/1 OSD Color-Space-Conversion Setting Register */
    volatile AMBA_VOUTM_CSC_PARAM_1_REG_s           OsdCSC1;                    /* 0x234/0x534(WO): Mixer Section 0/1 OSD Color-Space-Conversion Setting Register */
    volatile AMBA_VOUTM_CSC_PARAM_2_REG_s           OsdCSC2;                    /* 0x238/0x538(WO): Mixer Section 0/1 OSD Color-Space-Conversion Setting Register */
    volatile AMBA_VOUTM_CSC_PARAM_3_REG_s           OsdCSC3;                    /* 0x23C/0x53C(WO): Mixer Section 0/1 OSD Color-Space-Conversion Setting Register */
    volatile AMBA_VOUTM_CSC_PARAM_4_REG_s           OsdCSC4;                    /* 0x240/0x540(WO): Mixer Section 0/1 OSD Color-Space-Conversion Setting Register */
    volatile AMBA_VOUTM_CSC_PARAM_5_REG_s           OsdCSC5;                    /* 0x244/0x544(WO): Mixer Section 0/1 OSD Color-Space-Conversion Setting Register */
    volatile AMBA_VOUTM_CSC_PARAM_6_REG_s           OsdCSC6;                    /* 0x248/0x548(WO): Mixer Section 0/1 OSD Color-Space-Conversion Setting Register */
    volatile AMBA_VOUTM_CSC_PARAM_7_REG_s           OsdCSC7;                    /* 0x24C/0x54C(WO): Mixer Section 0/1 OSD Color-Space-Conversion Setting Register */
    volatile AMBA_VOUTM_CSC_PARAM_8_REG_s           OsdCSC8;                    /* 0x250/0x550(WO): Mixer Section 0/1 OSD Color-Space-Conversion Setting Register */
    volatile UINT32                                 Reserved1[43];              /* 0x254-0x2FC/0x554-0x5FC: Reserved */
} AMBA_VOUT_MIXER_REG_s;

#if defined(CONFIG_SOC_CV2) || defined(CONFIG_SOC_CV22) || defined(CONFIG_SOC_CV25)
/*
 * VOUT Display-0 Configuration Registers
 */
typedef struct {
    volatile AMBA_VOUTD_CTRL_REG_s                  DispCtrl;                   /* 0x300(RW): Display Section 0 Control Register */
    volatile UINT32                                 DispStatus;                 /* 0x304(RO): Display Section 0 Status Register */
    volatile AMBA_VOUTD_FRAME_SIZE_REG_s            DispFrmSize;                /* 0x308(RW): Display Section 0 Frame Size Register */
    volatile AMBA_VOUTD_FIELD_HEIGHT_REG_s          DispFldHeight;              /* 0x30C(RW): Display Section 0 Bottom Field Height Register */
    volatile AMBA_VOUTD_ACTIVE_REGION_START_REG_s   DispTopActiveStart;         /* 0x310(RW): Display Section 0 Active Region Start 0 Register */
    volatile AMBA_VOUTD_ACTIVE_REGION_END_REG_s     DispTopActiveEnd;           /* 0x314(RW): Display Section 0 Active Region End 0 Register */
    volatile AMBA_VOUTD_ACTIVE_REGION_START_REG_s   DispBtmActiveStart;         /* 0x318(RW): Display Section 0 Active Region Start 1 Register */
    volatile AMBA_VOUTD_ACTIVE_REGION_END_REG_s     DispBtmActiveEnd;           /* 0x31C(RW): Display Section 0 Active Region End 1 Register */
    volatile AMBA_VOUT_COLOR_REG_s                  DispBackgroundColor;        /* 0x320(WO): Display Section 0 Background Color Register */
    volatile AMBA_VOUTD_DIGITAL_OUTPUT_MODE_REG_s   DigitalOutputMode;          /* 0x324(RW): Digital Output Mode Register */
    volatile AMBA_VOUTD_HSYNC_CTRL_REG_s            DigitalHSync;               /* 0x328(RW): Digital Output HSync Control Register */
    volatile AMBA_VOUTD_VSYNC_START_REG_s           DigitalVSyncTopStart;       /* 0x32C(RW): First Line of VSync in Top Field/Progressive Frame */
    volatile AMBA_VOUTD_VSYNC_END_REG_s             DigitalVSyncTopEnd;         /* 0x330(RW): Last Line of VSync in Top Field/Progressive Frame */
    volatile AMBA_VOUTD_VSYNC_START_REG_s           DigitalVSyncBtmStart;       /* 0x334(RW): First Line of VSync in Bottom Field */
    volatile AMBA_VOUTD_VSYNC_END_REG_s             DigitalVSyncBtmEnd;         /* 0x338(RW): Last Line of VSync in Bottom Field */
    volatile AMBA_VOUTD_BT656_VBIT_REG_s            Bt656VBit;                  /* 0x33C(RW): Defines Start and End Row of V-Bit Assertion */
    volatile AMBA_VOUTD_BT656_SAV_REG_s             Bt656SAV;                   /* 0x340(RW): Defines Start Location of SAV Code */
    volatile UINT32                                 DigitalClkPattern[4];       /* 0x344-0x350(WO): Clock Divider Pattern Registers */
    volatile AMBA_VOUTD_DIGITAL_CSC_PARAM_0_REG_s   DigitalCSC0;                /* 0x354(WO): Color-Space-Conversion Setting for Digital Output Registers */
    volatile AMBA_VOUTD_DIGITAL_CSC_PARAM_1_REG_s   DigitalCSC1;                /* 0x358(WO): Color-Space-Conversion Setting for Digital Output Registers */
    volatile AMBA_VOUTD_DIGITAL_CSC_PARAM_2_REG_s   DigitalCSC2;                /* 0x35C(WO): Color-Space-Conversion Setting for Digital Output Registers */
    volatile AMBA_VOUTD_DIGITAL_CSC_PARAM_3_REG_s   DigitalCSC3;                /* 0x360(WO): Color-Space-Conversion Setting for Digital Output Registers */
    volatile AMBA_VOUTD_DIGITAL_CSC_PARAM_4_REG_s   DigitalCSC4;                /* 0x364(WO): Color-Space-Conversion Setting for Digital Output Registers */
    volatile AMBA_VOUTD_DIGITAL_CSC_PARAM_5_REG_s   DigitalCSC5;                /* 0x368(WO): Color-Space-Conversion Setting for Digital Output Registers */
    volatile AMBA_VOUTD_DIGITAL_CSC_PARAM_6_REG_s   DigitalCSC6;                /* 0x36C(WO): Color-Space-Conversion Setting for Digital Output Registers */
    volatile AMBA_VOUTD_DIGITAL_CSC_PARAM_7_REG_s   DigitalCSC7;                /* 0x370(WO): Color-Space-Conversion Setting for Digital Output Registers */
    volatile AMBA_VOUTD_DIGITAL_CSC_PARAM_8_REG_s   DigitalCSC8;                /* 0x374(WO): Color-Space-Conversion Setting for Digital Output Registers */
    volatile UINT32                                 Reserved[55];               /* Reserved */
    volatile AMBA_VOUTD_DITHER_CTRL_REG_s           DitherCtrl;                 /* 0x454(RW): Dithering Settings for Digital Output */
    volatile UINT32                                 DitherSeed;                 /* 0x458(RW): Dithering Seed */
    volatile AMBA_VOUTD_VOUT_VOUT_SYNC_REG_s        VoutSyncFromVoutB;          /* 0x45C(RW): Sync Signal Receiving from Display Section 1 Control Register */
    volatile AMBA_VOUTD_MIPI_BLANK_CTRL_REG_s       MipiBlankCtrl0;             /* 0x460(): HBP and HSync Blankings */
    volatile AMBA_VOUTD_MIPI_BLANK_CTRL_REG_s       MipiBlankCtrl1;             /* 0x464(): HFP and Vertical Blankings */
    volatile UINT32                                 Reserved1[6];               /* Reserved */
    volatile AMBA_VOUTD_INPUT_CONFIG_REG_s          InputConfig;                /* 0x480(): Input Source and Color Resolution */
    volatile AMBA_VOUT_INPUT_SYNC_CTRL_REG_s        InputSyncCounterCtrl;       /* 0x484(RW): Control Input Synchronization Register */
    volatile AMBA_VOUT_OUTPUT_SYNC_CTRL_REG_s       OutputSyncCounterCtrl;      /* 0x488(RW): Control Output Synchronization Register */
    volatile UINT32                                 StreamCtrl;                 /* 0x48C(RW): Input SMEM Channel Settings */
    volatile UINT32                                 Reserved2[2];               /* Reserved */
    volatile UINT32                                 OutputEnable;               /* 0x498(RW): Enable Output of one frame Register */
    volatile UINT32                                 Reserved3[25];              /* 0x49C-0x4FC: Reserved */
} AMBA_VOUT_DISPLAY0_CONFIG_REG_s;

/*
 * VOUT Display-1 Configuration Registers
 */
typedef struct {
    volatile AMBA_VOUTD_CTRL_REG_s                  DispCtrl;                   /* 0x600(RW): Display Section 1 Control Register */
    volatile UINT32                                 DispStatus;                 /* 0x604(RO): Display Section 1 Status Register */
    volatile AMBA_VOUTD_FRAME_SIZE_REG_s            DispFrmSize;                /* 0x608(RW): Display Section 1 Frame Size Register */
    volatile AMBA_VOUTD_FIELD_HEIGHT_REG_s          DispFldHeight;              /* 0x60C(RW): Display Section 1 Field Height Register */
    volatile AMBA_VOUTD_ACTIVE_REGION_START_REG_s   DispTopActiveStart;         /* 0x610(RW): Display Section 1 Active Region Start 0 Register */
    volatile AMBA_VOUTD_ACTIVE_REGION_END_REG_s     DispTopActiveEnd;           /* 0x614(RW): Display Section 1 Active Region End 0 Register */
    volatile AMBA_VOUTD_ACTIVE_REGION_START_REG_s   DispBtmActiveStart;         /* 0x618(RW): Display Section 1 Active Region Start 1 Register */
    volatile AMBA_VOUTD_ACTIVE_REGION_END_REG_s     DispBtmActiveEnd;           /* 0x61C(RW): Display Section 1 Active Region End 1 Register */
    volatile AMBA_VOUT_COLOR_REG_s                  DispBackgroundColor;        /* 0x620(WO): Display Section 1 Background Color Register */
    volatile AMBA_VOUTD_DIGITAL_OUTPUT_MODE_REG_s   DigitalOutputMode;          /* 0x624(RW): Digital Output Mode Register */
    volatile AMBA_VOUTD_HSYNC_CTRL_REG_s            DigitalHSync;               /* 0x628(RW): Digital Output HSync Control Register */
    volatile AMBA_VOUTD_VSYNC_START_REG_s           DigitalVSyncTopStart;       /* 0x62C(RW): First Line of VSync in Top Field/Progressive Frame */
    volatile AMBA_VOUTD_VSYNC_END_REG_s             DigitalVSyncTopEnd;         /* 0x630(RW): Last Line of VSync in Top Field/Progressive Frame */
    volatile AMBA_VOUTD_VSYNC_START_REG_s           DigitalVSyncBtmStart;       /* 0x634(RW): First Line of VSync in Bottom Field */
    volatile AMBA_VOUTD_VSYNC_END_REG_s             DigitalVSyncBtmEnd;         /* 0x638(RW): Last Line of VSync in Bottom Field */
    volatile AMBA_VOUTD_BT656_VBIT_REG_s            Bt656VBit;                  /* 0x63C(RW): Defines Start and End Row of V-Bit Assertion */
    volatile AMBA_VOUTD_BT656_SAV_REG_s             Bt656SAV;                   /* 0x640(RW): Defines Start Location of SAV Code */
    volatile UINT32                                 DigitalClkPattern[4];       /* 0x644-0x650(WO): Clock Divider Pattern Registers */
    volatile AMBA_VOUTD_DIGITAL_CSC_PARAM_0_REG_s   DigitalCSC0;                /* 0x654(WO): Color-Space-Conversion Setting for Digital Output Registers */
    volatile AMBA_VOUTD_DIGITAL_CSC_PARAM_1_REG_s   DigitalCSC1;                /* 0x658(WO): Color-Space-Conversion Setting for Digital Output Registers */
    volatile AMBA_VOUTD_DIGITAL_CSC_PARAM_2_REG_s   DigitalCSC2;                /* 0x65C(WO): Color-Space-Conversion Setting for Digital Output Registers */
    volatile AMBA_VOUTD_DIGITAL_CSC_PARAM_3_REG_s   DigitalCSC3;                /* 0x660(WO): Color-Space-Conversion Setting for Digital Output Registers */
    volatile AMBA_VOUTD_DIGITAL_CSC_PARAM_4_REG_s   DigitalCSC4;                /* 0x664(WO): Color-Space-Conversion Setting for Digital Output Registers */
    volatile AMBA_VOUTD_DIGITAL_CSC_PARAM_5_REG_s   DigitalCSC5;                /* 0x668(WO): Color-Space-Conversion Setting for Digital Output Registers */
    volatile AMBA_VOUTD_DIGITAL_CSC_PARAM_6_REG_s   DigitalCSC6;                /* 0x66C(WO): Color-Space-Conversion Setting for Digital Output Registers */
    volatile AMBA_VOUTD_DIGITAL_CSC_PARAM_7_REG_s   DigitalCSC7;                /* 0x670(WO): Color-Space-Conversion Setting for Digital Output Registers */
    volatile AMBA_VOUTD_DIGITAL_CSC_PARAM_8_REG_s   DigitalCSC8;                /* 0x674(WO): Color-Space-Conversion Setting for Digital Output Registers */
    volatile AMBA_VOUTD_ANALOG_OUTPUT_MODE_REG_s    AnalogOutputMode;           /* 0x678(RW): Analog Output Mode Register */
    volatile AMBA_VOUTD_HSYNC_CTRL_REG_s            AnalogHSync;                /* 0x67C(RW): Analog Output HSync Control Register */
    volatile AMBA_VOUTD_VSYNC_START_REG_s           AnalogVSyncTopStart;        /* 0x680(RW): First Line of VSync in Top Field/Progressive Frame */
    volatile AMBA_VOUTD_VSYNC_END_REG_s             AnalogVSyncTopEnd;          /* 0x684(RW): Last Line of VSync in Top Field/Progressive Frame */
    volatile AMBA_VOUTD_VSYNC_START_REG_s           AnalogVSyncBtmStart;        /* 0x688(RW): First Line of VSync in Bottom Field */
    volatile AMBA_VOUTD_VSYNC_END_REG_s             AnalogVSyncBtmEnd;          /* 0x68C(RW): Last Line of VSync in Bottom Field */
    volatile AMBA_VOUTD_ANALOG_VBI_CTRL_REG_s       AnalogVBICtrl;              /* 0x690(RW): Analog VBI Control Register */
    volatile AMBA_VOUTD_ANALOG_VBI_START_V_REG_s    AnalogVBIStartV;            /* 0x694(RW): Vertical Location of Analog VBI Insertion */
    volatile AMBA_VOUTD_ANALOG_VBI_H_REG_s          AnalogVBIH;                 /* 0x698(RW): Horizontal Location of Analog VBI Insertion */
    volatile UINT32                                 AnalogVBIData[12];          /* 0x69C-0x6C8(WO): Analog VBI Insertion Data */
    volatile AMBA_VOUTD_ANALOG_CSC_PARAM_0_REG_s    AnalogCSC0;                 /* 0x6CC(WO): Color Scaling Setting for Analog Output Registers */
    volatile AMBA_VOUTD_ANALOG_CSC_PARAM_1_REG_s    AnalogCSC1;                 /* 0x6D0(WO): Color Scaling Setting for Analog Output Registers */
    volatile AMBA_VOUTD_ANALOG_CSC_PARAM_2_REG_s    AnalogCSC2;                 /* 0x6D4(WO): Color Scaling Setting for Analog Output Registers */
    volatile AMBA_VOUTD_ANALOG_CSC_PARAM_3_REG_s    AnalogCSC3;                 /* 0x6D8(WO): Color Scaling Setting for Analog Output Registers */
    volatile AMBA_VOUTD_ANALOG_CSC_PARAM_4_REG_s    AnalogCSC4;                 /* 0x6DC(WO): Color Scaling Setting for Analog Output Registers */
    volatile AMBA_VOUTD_ANALOG_CSC_PARAM_5_REG_s    AnalogCSC5;                 /* 0x6E0(WO): Color Scaling Setting for Analog Output Registers */
    volatile AMBA_VOUTD_ANALOG_CSC_2_PARAM_0_REG_s  AnalogCSC6;               /* 0x6E4(WO): Color Scaling Setting for Analog Output Registers */
    volatile UINT32                                 Reserved1;                  /* 0x6E8: Reserved */
    volatile AMBA_VOUTD_ANALOG_CSC_2_PARAM_2_REG_s  AnalogCSC7;               /* 0x6EC(WO): Color Scaling Setting for Analog Output Registers */
    volatile UINT32                                 Reserved2;                  /* 0x6F0: Reserved */
    volatile AMBA_VOUTD_ANALOG_CSC_2_PARAM_4_REG_s  AnalogCSC8;               /* 0x6F4(WO): Color Scaling Setting for Analog Output Registers */
    volatile UINT32                                 Reserved3[3];               /* 0x6F8-0x700: Reserved */
    volatile AMBA_VOUTD_HDMI_OUTPUT_MODE_REG_s      HdmiOutputMode;             /* 0x704(RW): HDMI Output Mode Register */
    volatile AMBA_VOUTD_HSYNC_CTRL_REG_s            HdmiHSync;                  /* 0x708(RW): HDMI Output HSync Control Register */
    volatile AMBA_VOUTD_VSYNC_START_REG_s           HdmiVSyncTopStart;          /* 0x70C(RW): First Line of VSync in Top Field/Progressive Frame */
    volatile AMBA_VOUTD_VSYNC_END_REG_s             HdmiVSyncTopEnd;            /* 0x710(RW): Last Line of VSync in Top Field/Progressive Frame */
    volatile AMBA_VOUTD_VSYNC_START_REG_s           HdmiVSyncBtmStart;          /* 0x714(RW): First Line of VSync in Bottom Field */
    volatile AMBA_VOUTD_VSYNC_END_REG_s             HdmiVSyncBtmEnd;            /* 0x718(RW): Last Line of VSync in Bottom Field */
    volatile AMBA_VOUTD_DIGITAL_CSC_PARAM_0_REG_s   HdmiCSC0;                   /* 0x71C(WO): Color-Space-Conversion Setting for HDMI Output Registers */
    volatile AMBA_VOUTD_DIGITAL_CSC_PARAM_1_REG_s   HdmiCSC1;                   /* 0x720(WO): Color-Space-Conversion Setting for HDMI Output Registers */
    volatile AMBA_VOUTD_DIGITAL_CSC_PARAM_2_REG_s   HdmiCSC2;                   /* 0x724(WO): Color-Space-Conversion Setting for HDMI Output Registers */
    volatile AMBA_VOUTD_DIGITAL_CSC_PARAM_3_REG_s   HdmiCSC3;                   /* 0x728(WO): Color-Space-Conversion Setting for HDMI Output Registers */
    volatile AMBA_VOUTD_DIGITAL_CSC_PARAM_4_REG_s   HdmiCSC4;                   /* 0x72C(WO): Color-Space-Conversion Setting for HDMI Output Registers */
    volatile AMBA_VOUTD_DIGITAL_CSC_PARAM_5_REG_s   HdmiCSC5;                   /* 0x730(WO): Color-Space-Conversion Setting for HDMI Output Registers */
    volatile AMBA_VOUTD_DIGITAL_CSC_PARAM_6_REG_s   HdmiCSC6;                   /* 0x734(WO): Color-Space-Conversion Setting for HDMI Output Registers */
    volatile AMBA_VOUTD_DIGITAL_CSC_PARAM_7_REG_s   HdmiCSC7;                   /* 0x738(WO): Color-Space-Conversion Setting for HDMI Output Registers */
    volatile AMBA_VOUTD_DIGITAL_CSC_PARAM_8_REG_s   HdmiCSC8;                   /* 0x73C(WO): Color-Space-Conversion Setting for HDMI Output Registers */
    volatile UINT32                                 Reserved4[5];               /* 0x740-0x750: Reserved */
    volatile AMBA_VOUTD_DITHER_CTRL_REG_s           DitherCtrl;                 /* 0x754(RW): Dithering Settings for Digital Output */
    volatile UINT32                                 DitherSeed;                 /* 0x758(RW): Dithering Seed */
    volatile AMBA_VOUTD_VOUT_VOUT_SYNC_REG_s        VoutSyncFromVoutA;          /* 0x75C(RW): Sync Signal Receiving from Display Section 0 Control Register */
    volatile UINT32                                 StreamEnable;               /* 0x760(RW): Enable Input and Output Register */
    volatile AMBA_VOUT_INPUT_SYNC_CTRL_REG_s        InputSyncCounterCtrl;       /* 0x764(RW): Control Input Synchronization Register */
    volatile AMBA_VOUT_OUTPUT_SYNC_CTRL_REG_s       OutputSyncCounterCtrl;      /* 0x768(RW): Control Output Synchronization Register */
    volatile UINT32                                 StreamCtrl;                 /* 0x76C(RW): Control Output Streams Register */
    volatile UINT32                                 AudioStreamCtrl;            /* 0x770(RW): Defines SLVS Audio Input SMEM Channel Settings */
    volatile UINT32                                 AudioByteCount;             /* 0x774(RW): Defines Number of Bytes of SLVS Audio Input */
    volatile UINT32                                 OutputEnable;               /* 0x778(RW): Enable Output of one frame Register */
    volatile UINT32                                 Reserved5[33];              /* 0x77C-0x7FC: Reserved */
} AMBA_VOUT_DISPLAY1_CONFIG_REG_s;
#elif defined(CONFIG_SOC_CV28)
/*
 * VOUT Display-0 Configuration Registers
 */
typedef struct {
    volatile AMBA_VOUTD_CTRL_REG_s                  DispCtrl;                   /* 0x300(RW): Display Section 0 Control Register */
    volatile UINT32                                 DispStatus;                 /* 0x304(RO): Display Section 0 Status Register */
    volatile AMBA_VOUTD_FRAME_SIZE_REG_s            DispFrmSize;                /* 0x308(RW): Display Section 0 Frame Size Register */
    volatile AMBA_VOUTD_FIELD_HEIGHT_REG_s          DispFldHeight;              /* 0x30C(RW): Display Section 0 Bottom Field Height Register */
    volatile AMBA_VOUTD_ACTIVE_REGION_START_REG_s   DispTopActiveStart;         /* 0x310(RW): Display Section 0 Active Region Start 0 Register */
    volatile AMBA_VOUTD_ACTIVE_REGION_END_REG_s     DispTopActiveEnd;           /* 0x314(RW): Display Section 0 Active Region End 0 Register */
    volatile AMBA_VOUTD_ACTIVE_REGION_START_REG_s   DispBtmActiveStart;         /* 0x318(RW): Display Section 0 Active Region Start 1 Register */
    volatile AMBA_VOUTD_ACTIVE_REGION_END_REG_s     DispBtmActiveEnd;           /* 0x31C(RW): Display Section 0 Active Region End 1 Register */
    volatile AMBA_VOUT_COLOR_REG_s                  DispBackgroundColor;        /* 0x320(WO): Display Section 0 Background Color Register */
    volatile AMBA_VOUTD_DIGITAL_OUTPUT_MODE_REG_s   DigitalOutputMode;          /* 0x324(RW): Digital Output Mode Register */
    volatile AMBA_VOUTD_HSYNC_CTRL_REG_s            DigitalHSync;               /* 0x328(RW): Digital Output HSync Control Register */
    volatile AMBA_VOUTD_VSYNC_START_REG_s           DigitalVSyncTopStart;       /* 0x32C(RW): First Line of VSync in Top Field/Progressive Frame */
    volatile AMBA_VOUTD_VSYNC_END_REG_s             DigitalVSyncTopEnd;         /* 0x330(RW): Last Line of VSync in Top Field/Progressive Frame */
    volatile AMBA_VOUTD_VSYNC_START_REG_s           DigitalVSyncBtmStart;       /* 0x334(RW): First Line of VSync in Bottom Field */
    volatile AMBA_VOUTD_VSYNC_END_REG_s             DigitalVSyncBtmEnd;         /* 0x338(RW): Last Line of VSync in Bottom Field */
    volatile AMBA_VOUTD_BT656_VBIT_REG_s            Bt656VBit;                  /* 0x33C(RW): Defines Start and End Row of V-Bit Assertion */
    volatile AMBA_VOUTD_BT656_SAV_REG_s             Bt656SAV;                   /* 0x340(RW): Defines Start Location of SAV Code */
    volatile UINT32                                 DigitalClkPattern[4];       /* 0x344-0x350(WO): Clock Divider Pattern Registers */
    volatile AMBA_VOUTD_DIGITAL_CSC_PARAM_0_REG_s   DigitalCSC0;                /* 0x354(WO): Color-Space-Conversion Setting for Digital Output Registers */
    volatile AMBA_VOUTD_DIGITAL_CSC_PARAM_1_REG_s   DigitalCSC1;                /* 0x358(WO): Color-Space-Conversion Setting for Digital Output Registers */
    volatile AMBA_VOUTD_DIGITAL_CSC_PARAM_2_REG_s   DigitalCSC2;                /* 0x35C(WO): Color-Space-Conversion Setting for Digital Output Registers */
    volatile AMBA_VOUTD_DIGITAL_CSC_PARAM_3_REG_s   DigitalCSC3;                /* 0x360(WO): Color-Space-Conversion Setting for Digital Output Registers */
    volatile AMBA_VOUTD_DIGITAL_CSC_PARAM_4_REG_s   DigitalCSC4;                /* 0x364(WO): Color-Space-Conversion Setting for Digital Output Registers */
    volatile AMBA_VOUTD_DIGITAL_CSC_PARAM_5_REG_s   DigitalCSC5;                /* 0x368(WO): Color-Space-Conversion Setting for Digital Output Registers */
    volatile AMBA_VOUTD_DIGITAL_CSC_PARAM_6_REG_s   DigitalCSC6;                /* 0x36C(WO): Color-Space-Conversion Setting for Digital Output Registers */
    volatile AMBA_VOUTD_DIGITAL_CSC_PARAM_7_REG_s   DigitalCSC7;                /* 0x370(WO): Color-Space-Conversion Setting for Digital Output Registers */
    volatile AMBA_VOUTD_DIGITAL_CSC_PARAM_8_REG_s   DigitalCSC8;                /* 0x374(WO): Color-Space-Conversion Setting for Digital Output Registers */
    volatile AMBA_VOUTD_ANALOG_OUTPUT_MODE_REG_s    AnalogOutputMode;           /* 0x378(RW): Analog Output Mode Register */
    volatile AMBA_VOUTD_HSYNC_CTRL_REG_s            AnalogHSync;                /* 0x37C(RW): Analog Output HSync Control Register */
    volatile AMBA_VOUTD_VSYNC_START_REG_s           AnalogVSyncTopStart;        /* 0x380(RW): First Line of VSync in Top Field/Progressive Frame */
    volatile AMBA_VOUTD_VSYNC_END_REG_s             AnalogVSyncTopEnd;          /* 0x384(RW): Last Line of VSync in Top Field/Progressive Frame */
    volatile AMBA_VOUTD_VSYNC_START_REG_s           AnalogVSyncBtmStart;        /* 0x388(RW): First Line of VSync in Bottom Field */
    volatile AMBA_VOUTD_VSYNC_END_REG_s             AnalogVSyncBtmEnd;          /* 0x38C(RW): Last Line of VSync in Bottom Field */
    volatile AMBA_VOUTD_ANALOG_VBI_CTRL_REG_s       AnalogVBICtrl;              /* 0x390(RW): Analog VBI Control Register */
    volatile AMBA_VOUTD_ANALOG_VBI_START_V_REG_s    AnalogVBIStartV;            /* 0x394(RW): Vertical Location of Analog VBI Insertion */
    volatile AMBA_VOUTD_ANALOG_VBI_H_REG_s          AnalogVBIH;                 /* 0x398(RW): Horizontal Location of Analog VBI Insertion */
    volatile UINT32                                 AnalogVBIData[12];          /* 0x39C-0x3C8(WO): Analog VBI Insertion Data */
    volatile AMBA_VOUTD_ANALOG_CSC_PARAM_0_REG_s    AnalogCSC0;                 /* 0x3CC(WO): Color Scaling Setting for Analog Output Registers */
    volatile AMBA_VOUTD_ANALOG_CSC_PARAM_1_REG_s    AnalogCSC1;                 /* 0x3D0(WO): Color Scaling Setting for Analog Output Registers */
    volatile AMBA_VOUTD_ANALOG_CSC_PARAM_2_REG_s    AnalogCSC2;                 /* 0x3D4(WO): Color Scaling Setting for Analog Output Registers */
    volatile AMBA_VOUTD_ANALOG_CSC_PARAM_3_REG_s    AnalogCSC3;                 /* 0x3D8(WO): Color Scaling Setting for Analog Output Registers */
    volatile AMBA_VOUTD_ANALOG_CSC_PARAM_4_REG_s    AnalogCSC4;                 /* 0x3DC(WO): Color Scaling Setting for Analog Output Registers */
    volatile AMBA_VOUTD_ANALOG_CSC_PARAM_5_REG_s    AnalogCSC5;                 /* 0x3E0(WO): Color Scaling Setting for Analog Output Registers */
    volatile AMBA_VOUTD_ANALOG_CSC_2_PARAM_0_REG_s  AnalogCSC6;               /* 0x3E4(WO): Color Scaling Setting for Analog Output Registers */
    volatile UINT32                                 Reserved;                   /* 0x3E8:Reserved */
    volatile AMBA_VOUTD_ANALOG_CSC_2_PARAM_2_REG_s  AnalogCSC7;               /* 0x3EC(WO): Color Scaling Setting for Analog Output Registers */
    volatile UINT32                                 Reserved1;                  /* 0x3F0: Reserved */
    volatile AMBA_VOUTD_ANALOG_CSC_2_PARAM_4_REG_s  AnalogCSC8;               /* 0x3F4(WO): Color Scaling Setting for Analog Output Registers */
    volatile UINT32                                 Reserved2[23];              /* 0x3F8-0x450:Reserved */
    volatile AMBA_VOUTD_DITHER_CTRL_REG_s           DitherCtrl;                 /* 0x454(RW): Dithering Settings for Digital Output */
    volatile UINT32                                 DitherSeed;                 /* 0x458(RW): Dithering Seed */
    volatile AMBA_VOUTD_VOUT_VOUT_SYNC_REG_s        VoutSyncFromVoutB;          /* 0x45C(RW): Sync Signal Receiving from Display Section 1 Control Register */
    volatile AMBA_VOUTD_MIPI_BLANK_CTRL_REG_s       MipiBlankCtrl0;             /* 0x460(): HBP and HSync Blankings */
    volatile AMBA_VOUTD_MIPI_BLANK_CTRL_REG_s       MipiBlankCtrl1;             /* 0x464(): HFP and Vertical Blankings */
    volatile UINT32                                 Reserved3[6];               /* Reserved */
    volatile AMBA_VOUTD_INPUT_CONFIG_REG_s          InputConfig;                /* 0x480(): Input Source and Color Resolution */
    volatile AMBA_VOUT_INPUT_SYNC_CTRL_REG_s        InputSyncCounterCtrl;       /* 0x484(RW): Control Input Synchronization Register */
    volatile AMBA_VOUT_OUTPUT_SYNC_CTRL_REG_s       OutputSyncCounterCtrl;      /* 0x488(RW): Control Output Synchronization Register */
    volatile UINT32                                 StreamCtrl;                 /* 0x48C(RW): Input SMEM Channel Settings */
    volatile UINT32                                 Reserved4[2];               /* Reserved */
    volatile UINT32                                 OutputEnable;               /* 0x498(RW): Enable Output of one frame Register */
    volatile UINT32                                 Reserved5[25];              /* 0x49C-0x4FC: Reserved */
} AMBA_VOUT_DISPLAY0_CONFIG_REG_s;
#elif defined(CONFIG_SOC_H32)
/*
 * VOUT Display-0 Configuration Registers
 */
typedef struct {
    volatile AMBA_VOUTD_CTRL_REG_s                  DispCtrl;                   /* 0x300(RW): Display Section 0 Control Register */
    volatile UINT32                                 DispStatus;                 /* 0x304(RO): Display Section 0 Status Register */
    volatile AMBA_VOUTD_FRAME_SIZE_REG_s            DispFrmSize;                /* 0x308(RW): Display Section 0 Frame Size Register */
    volatile AMBA_VOUTD_FIELD_HEIGHT_REG_s          DispFldHeight;              /* 0x30C(RW): Display Section 0 Bottom Field Height Register */
    volatile AMBA_VOUTD_ACTIVE_REGION_START_REG_s   DispTopActiveStart;         /* 0x310(RW): Display Section 0 Active Region Start 0 Register */
    volatile AMBA_VOUTD_ACTIVE_REGION_END_REG_s     DispTopActiveEnd;           /* 0x314(RW): Display Section 0 Active Region End 0 Register */
    volatile AMBA_VOUTD_ACTIVE_REGION_START_REG_s   DispBtmActiveStart;         /* 0x318(RW): Display Section 0 Active Region Start 1 Register */
    volatile AMBA_VOUTD_ACTIVE_REGION_END_REG_s     DispBtmActiveEnd;           /* 0x31C(RW): Display Section 0 Active Region End 1 Register */
    volatile AMBA_VOUT_COLOR_REG_s                  DispBackgroundColor;        /* 0x320(WO): Display Section 0 Background Color Register */
    volatile AMBA_VOUTD_DIGITAL_OUTPUT_MODE_REG_s   DigitalOutputMode;          /* 0x324(RW): Digital Output Mode Register */
    volatile AMBA_VOUTD_HSYNC_CTRL_REG_s            DigitalHSync;               /* 0x328(RW): Digital Output HSync Control Register */
    volatile AMBA_VOUTD_VSYNC_START_REG_s           DigitalVSyncTopStart;       /* 0x32C(RW): First Line of VSync in Top Field/Progressive Frame */
    volatile AMBA_VOUTD_VSYNC_END_REG_s             DigitalVSyncTopEnd;         /* 0x330(RW): Last Line of VSync in Top Field/Progressive Frame */
    volatile AMBA_VOUTD_VSYNC_START_REG_s           DigitalVSyncBtmStart;       /* 0x334(RW): First Line of VSync in Bottom Field */
    volatile AMBA_VOUTD_VSYNC_END_REG_s             DigitalVSyncBtmEnd;         /* 0x338(RW): Last Line of VSync in Bottom Field */
    volatile AMBA_VOUTD_BT656_VBIT_REG_s            Bt656VBit;                  /* 0x33C(RW): Defines Start and End Row of V-Bit Assertion */
    volatile AMBA_VOUTD_BT656_SAV_REG_s             Bt656SAV;                   /* 0x340(RW): Defines Start Location of SAV Code */
    volatile UINT32                                 DigitalClkPattern[4];       /* 0x344-0x350(WO): Clock Divider Pattern Registers */
    volatile AMBA_VOUTD_DIGITAL_CSC_PARAM_0_REG_s   DigitalCSC0;                /* 0x354(WO): Color-Space-Conversion Setting for Digital Output Registers */
    volatile AMBA_VOUTD_DIGITAL_CSC_PARAM_1_REG_s   DigitalCSC1;                /* 0x358(WO): Color-Space-Conversion Setting for Digital Output Registers */
    volatile AMBA_VOUTD_DIGITAL_CSC_PARAM_2_REG_s   DigitalCSC2;                /* 0x35C(WO): Color-Space-Conversion Setting for Digital Output Registers */
    volatile AMBA_VOUTD_DIGITAL_CSC_PARAM_3_REG_s   DigitalCSC3;                /* 0x360(WO): Color-Space-Conversion Setting for Digital Output Registers */
    volatile AMBA_VOUTD_DIGITAL_CSC_PARAM_4_REG_s   DigitalCSC4;                /* 0x364(WO): Color-Space-Conversion Setting for Digital Output Registers */
    volatile AMBA_VOUTD_DIGITAL_CSC_PARAM_5_REG_s   DigitalCSC5;                /* 0x368(WO): Color-Space-Conversion Setting for Digital Output Registers */
    volatile AMBA_VOUTD_DIGITAL_CSC_PARAM_6_REG_s   DigitalCSC6;                /* 0x36C(WO): Color-Space-Conversion Setting for Digital Output Registers */
    volatile AMBA_VOUTD_DIGITAL_CSC_PARAM_7_REG_s   DigitalCSC7;                /* 0x370(WO): Color-Space-Conversion Setting for Digital Output Registers */
    volatile AMBA_VOUTD_DIGITAL_CSC_PARAM_8_REG_s   DigitalCSC8;                /* 0x374(WO): Color-Space-Conversion Setting for Digital Output Registers */
    volatile UINT32                                 Reserved[55];               /* Reserved */
    volatile AMBA_VOUTD_DITHER_CTRL_REG_s           DitherCtrl;                 /* 0x454(RW): Dithering Settings for Digital Output */
    volatile UINT32                                 DitherSeed;                 /* 0x458(RW): Dithering Seed */
    volatile AMBA_VOUTD_VOUT_VOUT_SYNC_REG_s        VoutSyncFromVoutB;          /* 0x45C(RW): Sync Signal Receiving from Display Section 1 Control Register */
    volatile UINT32                                 Reserved1[8];               /* 0x460-0x47C: Reserved */
    volatile AMBA_VOUTD_INPUT_CONFIG_REG_s          InputConfig;                /* 0x480(): Input Source and Color Resolution */
    volatile AMBA_VOUT_INPUT_SYNC_CTRL_REG_s        InputSyncCounterCtrl;       /* 0x484(RW): Control Input Synchronization Register */
    volatile AMBA_VOUT_OUTPUT_SYNC_CTRL_REG_s       OutputSyncCounterCtrl;      /* 0x488(RW): Control Output Synchronization Register */
    volatile UINT32                                 StreamCtrl;                 /* 0x48C(RW): Input SMEM Channel Settings */
    volatile UINT32                                 Reserved2[2];               /* Reserved */
    volatile UINT32                                 OutputEnable;               /* 0x498(RW): Enable Output of one frame Register */
    volatile UINT32                                 Reserved3[25];              /* 0x49C-0x4FC: Reserved */
} AMBA_VOUT_DISPLAY0_CONFIG_REG_s;

/*
 * VOUT Display-1 Configuration Registers
 */
typedef struct {
    volatile AMBA_VOUTD_CTRL_REG_s                  DispCtrl;                   /* 0x600(RW): Display Section 1 Control Register */
    volatile UINT32                                 DispStatus;                 /* 0x604(RO): Display Section 1 Status Register */
    volatile AMBA_VOUTD_FRAME_SIZE_REG_s            DispFrmSize;                /* 0x608(RW): Display Section 1 Frame Size Register */
    volatile AMBA_VOUTD_FIELD_HEIGHT_REG_s          DispFldHeight;              /* 0x60C(RW): Display Section 1 Field Height Register */
    volatile AMBA_VOUTD_ACTIVE_REGION_START_REG_s   DispTopActiveStart;         /* 0x610(RW): Display Section 1 Active Region Start 0 Register */
    volatile AMBA_VOUTD_ACTIVE_REGION_END_REG_s     DispTopActiveEnd;           /* 0x614(RW): Display Section 1 Active Region End 0 Register */
    volatile AMBA_VOUTD_ACTIVE_REGION_START_REG_s   DispBtmActiveStart;         /* 0x618(RW): Display Section 1 Active Region Start 1 Register */
    volatile AMBA_VOUTD_ACTIVE_REGION_END_REG_s     DispBtmActiveEnd;           /* 0x61C(RW): Display Section 1 Active Region End 1 Register */
    volatile AMBA_VOUT_COLOR_REG_s                  DispBackgroundColor;        /* 0x620(WO): Display Section 1 Background Color Register */
    volatile UINT32                                 Reserved0[21];              /* 0x624-0x674: Reserved */
    volatile AMBA_VOUTD_ANALOG_OUTPUT_MODE_REG_s    AnalogOutputMode;           /* 0x678(RW): Analog Output Mode Register */
    volatile AMBA_VOUTD_HSYNC_CTRL_REG_s            AnalogHSync;                /* 0x67C(RW): Analog Output HSync Control Register */
    volatile AMBA_VOUTD_VSYNC_START_REG_s           AnalogVSyncTopStart;        /* 0x680(RW): First Line of VSync in Top Field/Progressive Frame */
    volatile AMBA_VOUTD_VSYNC_END_REG_s             AnalogVSyncTopEnd;          /* 0x684(RW): Last Line of VSync in Top Field/Progressive Frame */
    volatile AMBA_VOUTD_VSYNC_START_REG_s           AnalogVSyncBtmStart;        /* 0x688(RW): First Line of VSync in Bottom Field */
    volatile AMBA_VOUTD_VSYNC_END_REG_s             AnalogVSyncBtmEnd;          /* 0x68C(RW): Last Line of VSync in Bottom Field */
    volatile AMBA_VOUTD_ANALOG_VBI_CTRL_REG_s       AnalogVBICtrl;              /* 0x690(RW): Analog VBI Control Register */
    volatile AMBA_VOUTD_ANALOG_VBI_START_V_REG_s    AnalogVBIStartV;            /* 0x694(RW): Vertical Location of Analog VBI Insertion */
    volatile AMBA_VOUTD_ANALOG_VBI_H_REG_s          AnalogVBIH;                 /* 0x698(RW): Horizontal Location of Analog VBI Insertion */
    volatile UINT32                                 AnalogVBIData[12];          /* 0x69C-0x6C8(WO): Analog VBI Insertion Data */
    volatile AMBA_VOUTD_ANALOG_CSC_PARAM_0_REG_s    AnalogCSC0;                 /* 0x6CC(WO): Color Scaling Setting for Analog Output Registers */
    volatile AMBA_VOUTD_ANALOG_CSC_PARAM_1_REG_s    AnalogCSC1;                 /* 0x6D0(WO): Color Scaling Setting for Analog Output Registers */
    volatile AMBA_VOUTD_ANALOG_CSC_PARAM_2_REG_s    AnalogCSC2;                 /* 0x6D4(WO): Color Scaling Setting for Analog Output Registers */
    volatile AMBA_VOUTD_ANALOG_CSC_PARAM_3_REG_s    AnalogCSC3;                 /* 0x6D8(WO): Color Scaling Setting for Analog Output Registers */
    volatile AMBA_VOUTD_ANALOG_CSC_PARAM_4_REG_s    AnalogCSC4;                 /* 0x6DC(WO): Color Scaling Setting for Analog Output Registers */
    volatile AMBA_VOUTD_ANALOG_CSC_PARAM_5_REG_s    AnalogCSC5;                 /* 0x6E0(WO): Color Scaling Setting for Analog Output Registers */
    volatile AMBA_VOUTD_ANALOG_CSC_2_PARAM_0_REG_s  AnalogCSC6;               /* 0x6E4(WO): Color Scaling Setting for Analog Output Registers */
    volatile UINT32                                 Reserved1;                  /* 0x6E8: Reserved */
    volatile AMBA_VOUTD_ANALOG_CSC_2_PARAM_2_REG_s  AnalogCSC7;               /* 0x6EC(WO): Color Scaling Setting for Analog Output Registers */
    volatile UINT32                                 Reserved2;                  /* 0x6F0: Reserved */
    volatile AMBA_VOUTD_ANALOG_CSC_2_PARAM_4_REG_s  AnalogCSC8;               /* 0x6F4(WO): Color Scaling Setting for Analog Output Registers */
    volatile UINT32                                 Reserved3[3];               /* 0x6F8-0x700: Reserved */
    volatile AMBA_VOUTD_HDMI_OUTPUT_MODE_REG_s      HdmiOutputMode;             /* 0x704(RW): HDMI Output Mode Register */
    volatile AMBA_VOUTD_HSYNC_CTRL_REG_s            HdmiHSync;                  /* 0x708(RW): HDMI Output HSync Control Register */
    volatile AMBA_VOUTD_VSYNC_START_REG_s           HdmiVSyncTopStart;          /* 0x70C(RW): First Line of VSync in Top Field/Progressive Frame */
    volatile AMBA_VOUTD_VSYNC_END_REG_s             HdmiVSyncTopEnd;            /* 0x710(RW): Last Line of VSync in Top Field/Progressive Frame */
    volatile AMBA_VOUTD_VSYNC_START_REG_s           HdmiVSyncBtmStart;          /* 0x714(RW): First Line of VSync in Bottom Field */
    volatile AMBA_VOUTD_VSYNC_END_REG_s             HdmiVSyncBtmEnd;            /* 0x718(RW): Last Line of VSync in Bottom Field */
    volatile AMBA_VOUTD_DIGITAL_CSC_PARAM_0_REG_s   HdmiCSC0;                   /* 0x71C(WO): Color-Space-Conversion Setting for HDMI Output Registers */
    volatile AMBA_VOUTD_DIGITAL_CSC_PARAM_1_REG_s   HdmiCSC1;                   /* 0x720(WO): Color-Space-Conversion Setting for HDMI Output Registers */
    volatile AMBA_VOUTD_DIGITAL_CSC_PARAM_2_REG_s   HdmiCSC2;                   /* 0x724(WO): Color-Space-Conversion Setting for HDMI Output Registers */
    volatile AMBA_VOUTD_DIGITAL_CSC_PARAM_3_REG_s   HdmiCSC3;                   /* 0x728(WO): Color-Space-Conversion Setting for HDMI Output Registers */
    volatile AMBA_VOUTD_DIGITAL_CSC_PARAM_4_REG_s   HdmiCSC4;                   /* 0x72C(WO): Color-Space-Conversion Setting for HDMI Output Registers */
    volatile AMBA_VOUTD_DIGITAL_CSC_PARAM_5_REG_s   HdmiCSC5;                   /* 0x730(WO): Color-Space-Conversion Setting for HDMI Output Registers */
    volatile AMBA_VOUTD_DIGITAL_CSC_PARAM_6_REG_s   HdmiCSC6;                   /* 0x734(WO): Color-Space-Conversion Setting for HDMI Output Registers */
    volatile AMBA_VOUTD_DIGITAL_CSC_PARAM_7_REG_s   HdmiCSC7;                   /* 0x738(WO): Color-Space-Conversion Setting for HDMI Output Registers */
    volatile AMBA_VOUTD_DIGITAL_CSC_PARAM_8_REG_s   HdmiCSC8;                   /* 0x73C(WO): Color-Space-Conversion Setting for HDMI Output Registers */
    volatile UINT32                                 Reserved4[5];               /* 0x740-0x750: Reserved */
    volatile AMBA_VOUTD_DITHER_CTRL_REG_s           DitherCtrl;                 /* 0x754(RW): Dithering Settings for Digital Output */
    volatile UINT32                                 DitherSeed;                 /* 0x758(RW): Dithering Seed */
    volatile AMBA_VOUTD_VOUT_VOUT_SYNC_REG_s        VoutSyncFromVoutA;          /* 0x75C(RW): Sync Signal Receiving from Display Section 0 Control Register */
    volatile UINT32                                 StreamEnable;               /* 0x760(RW): Enable Input and Output Register */
    volatile AMBA_VOUT_INPUT_SYNC_CTRL_REG_s        InputSyncCounterCtrl;       /* 0x764(RW): Control Input Synchronization Register */
    volatile AMBA_VOUT_OUTPUT_SYNC_CTRL_REG_s       OutputSyncCounterCtrl;      /* 0x768(RW): Control Output Synchronization Register */
    volatile UINT32                                 StreamCtrl;                 /* 0x76C(RW): Control Output Streams Register */
    volatile UINT32                                 AudioStreamCtrl;            /* 0x770(RW): Defines SLVS Audio Input SMEM Channel Settings */
    volatile UINT32                                 AudioByteCount;             /* 0x774(RW): Defines Number of Bytes of SLVS Audio Input */
    volatile UINT32                                 OutputEnable;               /* 0x778(RW): Enable Output of one frame Register */
    volatile UINT32                                 Reserved5[33];              /* 0x77C-0x7FC: Reserved */
} AMBA_VOUT_DISPLAY1_CONFIG_REG_s;
#else
/*
 * VOUT Display Configuration Registers
 */
typedef struct {
    volatile AMBA_VOUTD_CTRL_REG_s                  DispCtrl;                   /* 0x300/0x600(RW): Display Section Control Register */
    volatile UINT32                                 DispStatus;                 /* 0x304/0x604(RO): Display Section Status Register */
    volatile AMBA_VOUTD_FRAME_SIZE_REG_s            DispFrmSize;                /* 0x308/0x608(RW): Display Section Frame Size Register */
    volatile AMBA_VOUTD_FIELD_HEIGHT_REG_s          DispFldHeight;              /* 0x30C/0x60C(RW): Display Section Bottom Field Height Register */
    volatile AMBA_VOUTD_ACTIVE_REGION_START_REG_s   DispTopActiveStart;         /* 0x310/0x610(RW): Display Section Active Region Start 0 Register */
    volatile AMBA_VOUTD_ACTIVE_REGION_END_REG_s     DispTopActiveEnd;           /* 0x314/0x614(RW): Display Section Active Region End 0 Register */
    volatile AMBA_VOUTD_ACTIVE_REGION_START_REG_s   DispBtmActiveStart;         /* 0x318/0x618(RW): Display Section Active Region Start 1 Register */
    volatile AMBA_VOUTD_ACTIVE_REGION_END_REG_s     DispBtmActiveEnd;           /* 0x31C/0x61C(RW): Display Section Active Region End 1 Register */
    volatile AMBA_VOUT_COLOR_REG_s                  DispBackgroundColor;        /* 0x320/0x620(WO): Display Section Background Color Register */
    volatile AMBA_VOUTD_DIGITAL_OUTPUT_MODE_REG_s   DigitalOutputMode;          /* 0x324/0x624(RW): Digital Output Mode Register */
    volatile AMBA_VOUTD_HSYNC_CTRL_REG_s            DigitalHSync;               /* 0x328/0x628(RW): Digital Output HSync Control Register */
    volatile AMBA_VOUTD_VSYNC_START_REG_s           DigitalVSyncTopStart;       /* 0x32C/0x62C(RW): First Line of VSync in Top Field/Progressive Frame */
    volatile AMBA_VOUTD_VSYNC_END_REG_s             DigitalVSyncTopEnd;         /* 0x330/0x630(RW): Last Line of VSync in Top Field/Progressive Frame */
    volatile AMBA_VOUTD_VSYNC_START_REG_s           DigitalVSyncBtmStart;       /* 0x334/0x634(RW): First Line of VSync in Bottom Field */
    volatile AMBA_VOUTD_VSYNC_END_REG_s             DigitalVSyncBtmEnd;         /* 0x338/0x638(RW): Last Line of VSync in Bottom Field */
    volatile UINT32                                 Reserved[6];                /* Reserved */
    volatile AMBA_VOUTD_DIGITAL_CSC_PARAM_0_REG_s   DigitalCSC0;                /* 0x354/0x654(WO): Color-Space-Conversion Setting for Digital Output Registers */
    volatile AMBA_VOUTD_DIGITAL_CSC_PARAM_1_REG_s   DigitalCSC1;                /* 0x358/0x658(WO): Color-Space-Conversion Setting for Digital Output Registers */
    volatile AMBA_VOUTD_DIGITAL_CSC_PARAM_2_REG_s   DigitalCSC2;                /* 0x35C/0x65C(WO): Color-Space-Conversion Setting for Digital Output Registers */
    volatile AMBA_VOUTD_DIGITAL_CSC_PARAM_3_REG_s   DigitalCSC3;                /* 0x360/0x660(WO): Color-Space-Conversion Setting for Digital Output Registers */
    volatile AMBA_VOUTD_DIGITAL_CSC_PARAM_4_REG_s   DigitalCSC4;                /* 0x364/0x664(WO): Color-Space-Conversion Setting for Digital Output Registers */
    volatile AMBA_VOUTD_DIGITAL_CSC_PARAM_5_REG_s   DigitalCSC5;                /* 0x368/0x668(WO): Color-Space-Conversion Setting for Digital Output Registers */
    volatile AMBA_VOUTD_DIGITAL_CSC_PARAM_6_REG_s   DigitalCSC6;                /* 0x36C/0x66C(WO): Color-Space-Conversion Setting for Digital Output Registers */
    volatile AMBA_VOUTD_DIGITAL_CSC_PARAM_7_REG_s   DigitalCSC7;                /* 0x370/0x670(WO): Color-Space-Conversion Setting for Digital Output Registers */
    volatile AMBA_VOUTD_DIGITAL_CSC_PARAM_8_REG_s   DigitalCSC8;                /* 0x374/0x674(WO): Color-Space-Conversion Setting for Digital Output Registers */
    volatile UINT32                                 Reserved1[50];              /* Reserved */
    volatile UINT32                                 WatchdogTimeout;            /* 0x440/0x740(RW): Threshold Value for Vout Display Frame Time */
    volatile UINT32                                 Reserved2[4];               /* Reserved */
    volatile AMBA_VOUTD_DITHER_CTRL_REG_s           DitherCtrl;                 /* 0x454/0x754(RW): Dithering Settings for Digital Output */
    volatile UINT32                                 DitherSeed;                 /* 0x458/0x758(RW): Dithering Seed */
    volatile AMBA_VOUTD_VOUT_VOUT_SYNC_REG_s        VoutSyncFromVoutB;          /* 0x45C/0x75C(RW): Sync Signal Receiving from Display Section 1 Control Register */
    volatile AMBA_VOUTD_MIPI_BLANK_CTRL_REG_s       MipiBlankCtrl0;             /* 0x460/0x760(RW): HBP and HSync Blankings */
    volatile AMBA_VOUTD_MIPI_BLANK_CTRL_REG_s       MipiBlankCtrl1;             /* 0x464/0x764(RW): HFP and Vertical Blankings */
    volatile UINT32                                 MipiMaxFrameCount;          /* 0x468/0x768(RW): For MIPI CSI Frame Count. (1, 2, 3, 4, N-1, N) */
    volatile UINT32                                 Reserved3[5];               /* Reserved */
    volatile AMBA_VOUTD_INPUT_CONFIG_REG_s          InputConfig;                /* 0x480/0x780(RW): Input Source and Color Resolution */
    volatile AMBA_VOUT_INPUT_SYNC_CTRL_REG_s        InputSyncCounterCtrl;       /* 0x484/0x784(RW): Control Input Synchronization Register */
    volatile AMBA_VOUT_OUTPUT_SYNC_CTRL_REG_s       OutputSyncCounterCtrl;      /* 0x488/0x788(RW): Control Output Synchronization Register */
    volatile UINT32                                 StreamCtrl;                 /* 0x48C/0x78C(RW): Input SMEM Channel Settings */
    volatile UINT32                                 Reserved4[2];               /* Reserved */
    volatile UINT32                                 ForceBackground;            /* 0x498/0x798(RW): Force to output background color */
    volatile UINT32                                 OutputEnable;               /* 0x49C/0x79C(RW): Enable Output of one frame Register */
    volatile UINT32                                 Reserved5[2];               /* Reserved */
    volatile UINT32                                 MipiPhyCntrStatus;          /* 0x4A8/0x7A8(RW): Debug MIPI PHY Counter */
    volatile UINT32                                 MipiPhyCtrlStatus;          /* 0x4AC/0x7AC(RW): Debug MIPI PHY Status */
    volatile UINT32                                 Reserved6[20];              /* 0x4B0-0x4FC/0x7B0-0x7FC: Reserved */
} AMBA_VOUT_DISPLAY_CONFIG_REG_s;
#endif
/*
 * VOUT: OSD Rescaler Registers
 */
typedef struct {
    volatile AMBA_OSD_RESCALE_COEF_0_3_REG_s        Coef0;                      /* OSD Rescale: LPF Coefficient 0-3 */
    volatile AMBA_OSD_RESCALE_COEF_4_5_REG_s        Coef1;                      /* OSD Rescale: LPF Coefficient 4-5 */
} AMBA_VOUT_OSD_RESCALE_COEF_REG_s;

typedef struct {
    volatile AMBA_OSD_RESCALE_CTRL_REG_s            Ctrl;                       /* 0x800(RW): OSD Rescale Control Register */
    volatile AMBA_OSD_RESCALE_OUTPUT_SIZE_REG_s     Width;                      /* 0x804(RW): OSD Rescale Output Width Register */
    volatile AMBA_OSD_RESCALE_PHASE_INC_REG_s       HoriPhaseInc;               /* 0x808(RW): OSD Rescale Horizontal Phase Increment Register */
    volatile AMBA_OSD_RESCALE_INIT_PHASE_REG_s      HoriInitPhaseEven;          /* 0x80C(RW): OSD Rescale Horizontal Initial Phase for Even Lines Register */
    volatile AMBA_OSD_RESCALE_INIT_PHASE_REG_s      HoriInitPhaseOdd;           /* 0x810(RW): OSD Rescale Horizontal Initial Phase for Odd Lines Register */
    volatile AMBA_OSD_RESCALE_OUTPUT_SIZE_REG_s     Height;                     /* 0x814(RW): OSD Rescale Output Height Register */
    volatile AMBA_OSD_RESCALE_PHASE_INC_REG_s       VertPhaseInc;               /* 0x818(RW): OSD Rescale Vertical Phase Increment Register */
    volatile AMBA_OSD_RESCALE_INIT_PHASE_REG_s      VertInitPhase;              /* 0x81C(RW): OSD Rescale Vertical Initial Phase Register */
    volatile AMBA_VOUT_OSD_RESCALE_COEF_REG_s       HCoef[16];                  /* 0x820-0x89C(WO): OSD Rescale Horizontal Phase 0-16 Coefficients */
    volatile AMBA_VOUT_OSD_RESCALE_COEF_REG_s       VCoef[16];                  /* 0x8A0-0x91C(WO): OSD Rescale Vertical Phase 0-16 Coefficients */
    volatile AMBA_OSD_RESCALE_DITHER_CTRL_REG_s     HoriDitherCtrl;             /* 0x920(RW): OSD Rescale Horizontal Dithering Control Register */
    volatile AMBA_OSD_RESCALE_DITHER_SEED_REG_s     HoriDitherSeed;             /* 0x924(RW): OSD Rescale Horizontal Dithering Seed Register */
    volatile AMBA_OSD_RESCALE_DITHER_CTRL_REG_s     VertDitherCtrl;             /* 0x928(RW): OSD Rescale Vertical Dithering Control Register */
    volatile AMBA_OSD_RESCALE_DITHER_SEED_REG_s     VertDitherSeed;             /* 0x92C(RW): OSD Rescale Vertical Dithering Seed Register */
    volatile UINT32                                 Reserved[52];               /* 0x930-0x9FC: Reserved */
} AMBA_VOUT_OSD_RESCALE_REG_s;

/*
 * VOUT: Top-Level Registers
 */
typedef struct {
    volatile AMBA_VOUT_RESET_REG_s                  Reset;                      /* 0xA00(RW): Reset Entire VOUT Block Register */
    volatile AMBA_VOUT_CLK_ENABLE_REG_s             EnableSection0;             /* 0xA04(RW): Enable Clock to Mixer and Display 0 Register */
    volatile AMBA_VOUT_CLK_ENABLE_REG_s             EnableSection1;             /* 0xA08(RW): Enable Clock to Mixer and Display 1 Register */
    volatile AMBA_VOUT_CLK_ENABLE_REG_s             EnableOsdRescale;           /* 0xA0C(RW): Enable Clock to OSD Rescaler Register */
    volatile AMBA_VOUT_CLK_DISABLE_REG_s            DisableSmem;                /* 0xA10(RW): Disable Smem Interface */
    volatile AMBA_VOUT_CLK_DISABLE_REG_s            DisableRegConfig;           /* 0xA14(RW): Disable Register Configuration Clock Register */
    volatile AMBA_VOUT_CLK_DISABLE_REG_s            DisableMixer;               /* 0xA18(RW): Disable Mixer Datapath Clock Register */
#if defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)
    volatile AMBA_VOUT_CLK_C_ENABLE_REG_s           EnableSection2;             /* 0xA1C(RW): Enable Clock to Display 2 Register */
    volatile UINT32                                 Reserved[56];               /* 0xA20-0xAFC: Reserved */
#else
    volatile UINT32                                 Reserved[57];               /* 0xA1C-0xAFC: Reserved */
#endif
} AMBA_VOUT_TOP_REG_s;

#if !defined(CONFIG_SOC_H32)
/*
 * VOUT: MIPI Command Registers
 */
typedef struct {
    volatile UINT32         Header;                 /* 0xB00/0xE00(RW): Header registers of mipi command */
    volatile UINT32         Param[4];               /* 0xB04-0xB10/0xE04-0xE10(RW): Mipi command parameter 0 */
    volatile UINT32         Ctrl;                   /* 0xB14/0xE14(RW): Mipi command control */
    volatile UINT32         Reserved[58];           /* 0xB18-0xBFC/0xE18-0xEFC: Reserved */
} AMBA_VOUT_MIPI_DSI_COMMAND_REG_s;
#endif

#if defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)
/*
 * VOUT: Display-C Configuration Registers
 */
typedef struct {
    volatile AMBA_VOUTDC_CTRL_REG_s                 DispCtrl;                   /* 0xC00(RW): Display Section Control Register */
    volatile UINT32                                 DispStatus;                 /* 0xC04(RO): Display Section Status Register */
    volatile AMBA_VOUTD_FRAME_SIZE_REG_s            DispFrmSize;                /* 0xC08(RW): Display Section Frame Size Register */
    volatile AMBA_VOUTD_FIELD_HEIGHT_REG_s          DispFldHeight;              /* 0xC0C(RW): Display Section Bottom Field Height Register */
    volatile AMBA_VOUTD_ACTIVE_REGION_START_REG_s   DispTopActiveStart;         /* 0xC10(RW): Display Section Active Region Start 0 Register */
    volatile AMBA_VOUTD_ACTIVE_REGION_END_REG_s     DispTopActiveEnd;           /* 0xC14(RW): Display Section Active Region End 0 Register */
    volatile AMBA_VOUTD_ACTIVE_REGION_START_REG_s   DispBtmActiveStart;         /* 0xC18(RW): Display Section Active Region Start 1 Register */
    volatile AMBA_VOUTD_ACTIVE_REGION_END_REG_s     DispBtmActiveEnd;           /* 0xC1C(RW): Display Section Active Region End 1 Register */
    volatile AMBA_VOUT_COLOR_REG_s                  DispBackgroundColor;        /* 0xC20(WO): Display Section Background Color Register */
    volatile UINT32                                 Reserved[21];               /* Reserved */
    volatile AMBA_VOUTD_ANALOG_OUTPUT_MODE_REG_s    AnalogOutputMode;           /* 0xC78(RW): Controls Analog Output Options */
    volatile AMBA_VOUTD_HSYNC_CTRL_REG_s            AnalogHSync;                /* 0xC7C(RW): Controls HSync for Analog Output */
    volatile AMBA_VOUTD_VSYNC_START_REG_s           AnalogVSyncTopStart;        /* 0xC80(RW): Defines Start of VSync in Top Field Or Progressive Frame */
    volatile AMBA_VOUTD_VSYNC_END_REG_s             AnalogVSyncTopEnd;          /* 0xC84(RW): Defines End of VSync in Top Field Or Progressive Frame */
    volatile AMBA_VOUTD_VSYNC_START_REG_s           AnalogVSyncBtmStart;        /* 0xC88(RW): Defines Start of VSync in Bottom Field (Ignored for Progressive Output Formats) */
    volatile AMBA_VOUTD_VSYNC_END_REG_s             AnalogVSyncBtmEnd;          /* 0xC8C(RW): Defines End of VSync in Bottom Field (Ignored for Progressive Output Formats) */
    volatile AMBA_VOUTD_ANALOG_VBI_CTRL_REG_s       AnalogVBICtrl;              /* 0xC90(RW): VBI Settings */
    volatile AMBA_VOUTD_ANALOG_VBI_START_V_REG_s    AnalogVBIStartV;            /* 0xC94(RW): Start Location of VBI Insertion Vertically */
    volatile AMBA_VOUTD_ANALOG_VBI_H_REG_s          AnalogVBIH;                 /* 0xC98(RW): VBI Horizontal Region */
    volatile UINT32                                 AnalogVBIData[12];          /* 0xC9C-0xCC8(WO): VBI Data */
    volatile AMBA_VOUTD_ANALOG_CSC_PARAM_0_REG_s    AnalogCSC0;                 /* 0xCCC(WO): Color Scaling Setting for Analog Output */
    volatile AMBA_VOUTD_ANALOG_CSC_PARAM_1_REG_s    AnalogCSC1;                 /* 0xCD0(WO): Color Scaling Setting for Analog Output */
    volatile AMBA_VOUTD_ANALOG_CSC_PARAM_2_REG_s    AnalogCSC2;                 /* 0xCD4(WO): Color Scaling Setting for Analog Output */
    volatile AMBA_VOUTD_ANALOG_CSC_PARAM_3_REG_s    AnalogCSC3;                 /* 0xCD8(WO): Color Scaling Setting for Analog Output */
    volatile AMBA_VOUTD_ANALOG_CSC_PARAM_4_REG_s    AnalogCSC4;                 /* 0xCDC(WO): Color Scaling Setting for Analog Output */
    volatile AMBA_VOUTD_ANALOG_CSC_PARAM_5_REG_s    AnalogCSC5;                 /* 0xCE0(WO): Color Scaling Setting for Analog Output */
    volatile AMBA_VOUTD_ANALOG_CSC_2_PARAM_0_REG_s  AnalogCSC6;               /* 0xCE4(WO): Color Scaling Setting for Analog Output */
    volatile UINT32                                 Reserved1;                  /* Reserved */
    volatile AMBA_VOUTD_ANALOG_CSC_2_PARAM_2_REG_s  AnalogCSC7;               /* 0xCEC(WO): Color Scaling Setting for Analog Output */
    volatile UINT32                                 Reserved2;                  /* Reserved */
    volatile AMBA_VOUTD_ANALOG_CSC_2_PARAM_4_REG_s  AnalogCSC8;               /* 0xCF4(WO): Color Scaling Setting for Analog Output */
    volatile UINT32                                 Reserved3[3];               /* Reserved */
    volatile AMBA_VOUTD_HDMI_OUTPUT_MODE_REG_s      HdmiOutputMode;             /* 0xD04(RW): Controls HDMI Output Options */
    volatile AMBA_VOUTD_HSYNC_CTRL_REG_s            HdmiHSync;                  /* 0xD08(RW): Controls HSync for HDMI Output */
    volatile AMBA_VOUTD_VSYNC_START_REG_s           HdmiVSyncTopStart;          /* 0xD0C(RW): Defines Start of VSync in Top Field or Progressive Frame */
    volatile AMBA_VOUTD_VSYNC_END_REG_s             HdmiVSyncTopEnd;            /* 0xD10(RW): Defines End of VSync in Top Field or Progressive Frame */
    volatile AMBA_VOUTD_VSYNC_START_REG_s           HdmiVSyncBtmStart;          /* 0xD14(RW): Defines Start of VSync in Bottom Field (Ignored for Progressive Output Formats) */
    volatile AMBA_VOUTD_VSYNC_END_REG_s             HdmiVSyncBtmEnd;            /* 0xD18(RW): Defines End of VSync in Bottom Field (Ignored for Progressive Output Formats) */
    volatile AMBA_VOUTD_DIGITAL_CSC_PARAM_0_REG_s   HdmiCSC0;                   /* 0xD1C(RW): Color-Space-Conversion Setting for HDMI Output */
    volatile AMBA_VOUTD_DIGITAL_CSC_PARAM_1_REG_s   HdmiCSC1;                   /* 0xD20(RW): Color-Space-Conversion Setting for HDMI Output */
    volatile AMBA_VOUTD_DIGITAL_CSC_PARAM_2_REG_s   HdmiCSC2;                   /* 0xD24(RW): Color-Space-Conversion Setting for HDMI Output */
    volatile AMBA_VOUTD_DIGITAL_CSC_PARAM_3_REG_s   HdmiCSC3;                   /* 0xD28(RW): Color-Space-Conversion Setting for HDMI Output */
    volatile AMBA_VOUTD_DIGITAL_CSC_PARAM_4_REG_s   HdmiCSC4;                   /* 0xD2C(RW): Color-Space-Conversion Setting for HDMI Output */
    volatile AMBA_VOUTD_DIGITAL_CSC_PARAM_5_REG_s   HdmiCSC5;                   /* 0xD30(RW): Color-Space-Conversion Setting for HDMI Output */
    volatile AMBA_VOUTD_DIGITAL_CSC_PARAM_6_REG_s   HdmiCSC6;                   /* 0xD34(RW): Color-Space-Conversion Setting for HDMI Output */
    volatile AMBA_VOUTD_DIGITAL_CSC_PARAM_7_REG_s   HdmiCSC7;                   /* 0xD38(RW): Color-Space-Conversion Setting for HDMI Output */
    volatile AMBA_VOUTD_DIGITAL_CSC_PARAM_8_REG_s   HdmiCSC8;                   /* 0xD3C(RW): Color-Space-Conversion Setting for HDMI Output */
    volatile UINT32                                 WatchdogTimeout;            /* 0xD40(RW): Threshold Value for Vout Display Frame Time */
    volatile UINT32                                 Reserved4[15];              /* Reserved */
    volatile AMBA_VOUTD_INPUT_CONFIG_REG_s          InputConfig;                /* 0xD80(RW): Input Source and Color Resolution */
    volatile AMBA_VOUT_INPUT_SYNC_CTRL_REG_s        InputSyncCounterCtrl;       /* 0xD84(RW): Control Input Synchronization Register */
    volatile AMBA_VOUT_OUTPUT_SYNC_CTRL_REG_s       OutputSyncCounterCtrl;      /* 0xD88(RW): Control Output Synchronization Register */
    volatile UINT32                                 StreamCtrl;                 /* 0xD8C(RW): Input SMEM Channel Settings */
    volatile UINT32                                 Reserved5[2];               /* Reserved */
    volatile UINT32                                 ForceBackground;            /* 0xD98(RW): Force to output background color */
    volatile UINT32                                 OutputEnable;               /* 0xD9C(RW): Enable Output of one frame Register */
    volatile UINT32                                 Reserved6[24];              /* Reserved */
} AMBA_VOUT_DISPLAY_C_CONFIG_REG_s;
#endif

/*
 * VOUT: CVBS Configurations
 */
typedef struct {
    UINT32     DispCtrl;               /* Display Control Register */
    UINT32     DispFrmSize;            /* Display Frame Size Register */
    UINT32     DispFldHeight;          /* Display Field Height Register */
    UINT32     DispTopActiveStart;     /* Display Active Region Start 0 Register */
    UINT32     DispTopActiveEnd;       /* Display Active Region End 0 Register */
    UINT32     DispBtmActiveStart;     /* Display Active Region Start 1 Register */
    UINT32     DispBtmActiveEnd;       /* Display Active Region End 1 Register */
    UINT32     DispBackgroundColor;    /* Display Background Color Register */
    UINT32     AnalogOutputMode;       /* Analog Output Mode Register */
    UINT32     AnalogHSync;            /* Analog Output HSync Control Register */
    UINT32     AnalogVSyncTopStart;    /* First Line of VSync in Top Field/Progressive Frame */
    UINT32     AnalogVSyncTopEnd;      /* Last Line of VSync in Top Field/Progressive Frame */
    UINT32     AnalogVSyncBtmStart;    /* First Line of VSync in Bottom Field */
    UINT32     AnalogVSyncBtmEnd;      /* Last Line of VSync in Bottom Field */
    UINT32     AnalogCSC0;             /* Color Scaling Setting for Analog Output Registers */
    UINT32     AnalogCSC1;             /* Color Scaling Setting for Analog Output Registers */
    UINT32     AnalogCSC2;             /* Color Scaling Setting for Analog Output Registers */
    UINT32     AnalogCSC3;             /* Color Scaling Setting for Analog Output Registers */
    UINT32     AnalogCSC4;             /* Color Scaling Setting for Analog Output Registers */
    UINT32     AnalogCSC5;             /* Color Scaling Setting for Analog Output Registers */
    UINT32     AnalogCSC6;             /* Post Scaling Setting for Analog Output Registers */
    UINT32     AnalogCSC7;             /* Post Scaling Setting for Analog Output Registers */
    UINT32     AnalogCSC8;             /* Post Scaling Setting for Analog Output Registers */
} AMBA_VOUT_DISPLAY_CVBS_CONFIG_REG_s;

/*
 * VOUT: All Blocks
 */
#if defined(CONFIG_SOC_CV2) || defined(CONFIG_SOC_CV22) || defined(CONFIG_SOC_CV25)
typedef struct {
    AMBA_VOUT_TVENC_CONTROL_REG_s                   TvEncConfig;                /* 0x000-0x1FC: TV encoder configuration */
    AMBA_VOUT_MIXER_REG_s                           MixerSection0;              /* 0x200-0x2FC: Mixer Section 0 control */
    AMBA_VOUT_DISPLAY0_CONFIG_REG_s                 DisplaySection0;            /* 0x300-0x4FC: Display Section 0 control */
    AMBA_VOUT_MIXER_REG_s                           MixerSection1;              /* 0x500-0x5FC: Mixer Section 1 control */
    AMBA_VOUT_DISPLAY1_CONFIG_REG_s                 DisplaySection1;            /* 0x600-0x7FC: Display Section 1 control */
    AMBA_VOUT_OSD_RESCALE_REG_s                     OsdRescaler;                /* 0x800-0x9FC: Controls OSD Rescaler */
    AMBA_VOUT_TOP_REG_s                             TopLevelCtrl;               /* 0xA00-0xAFC: Top level reset, and clock enables */
    AMBA_VOUT_MIPI_DSI_COMMAND_REG_s                MipiDsiCmd;                 /* 0xB00-0xBFC: MIPI DSI Command */
} AMBA_VOUT_REG_s;
#elif defined(CONFIG_SOC_CV28)
typedef struct {
    AMBA_VOUT_TVENC_CONTROL_REG_s                   TvEncConfig;                /* 0x000-0x1FC: TV encoder configuration */
    AMBA_VOUT_MIXER_REG_s                           MixerSection0;              /* 0x200-0x2FC: Mixer Section 0 control */
    AMBA_VOUT_DISPLAY0_CONFIG_REG_s                 DisplaySection0;            /* 0x300-0x4FC: Display Section 0 control */
    AMBA_VOUT_OSD_RESCALE_REG_s                     OsdRescaler;                /* 0x800-0x9FC: Controls OSD Rescaler */
    AMBA_VOUT_TOP_REG_s                             TopLevelCtrl;               /* 0xA00-0xAFC: Top level reset, and clock enables */
    AMBA_VOUT_MIPI_DSI_COMMAND_REG_s                MipiDsiCmd;                 /* 0xB00-0xBFC: MIPI DSI Command */
} AMBA_VOUT_REG_s;
#elif defined(CONFIG_SOC_H32)
typedef struct {
    AMBA_VOUT_TVENC_CONTROL_REG_s                   TvEncConfig;                /* 0x000-0x1FC: TV encoder configuration */
    AMBA_VOUT_MIXER_REG_s                           MixerSection0;              /* 0x200-0x2FC: Mixer Section 0 control */
    AMBA_VOUT_DISPLAY0_CONFIG_REG_s                 DisplaySection0;            /* 0x300-0x4FC: Display Section 0 control */
    AMBA_VOUT_MIXER_REG_s                           MixerSection1;              /* 0x500-0x5FC: Mixer Section 1 control */
    AMBA_VOUT_DISPLAY1_CONFIG_REG_s                 DisplaySection1;            /* 0x600-0x7FC: Display Section 1 control */
    AMBA_VOUT_OSD_RESCALE_REG_s                     OsdRescaler;                /* 0x800-0x9FC: Controls OSD Rescaler */
    AMBA_VOUT_TOP_REG_s                             TopLevelCtrl;               /* 0xA00-0xAFC: Top level reset, and clock enables */
} AMBA_VOUT_REG_s;
#else
typedef struct {
    AMBA_VOUT_TVENC_CONTROL_REG_s                   TvEncConfig;                /* 0x000-0x1FC: NTSC/PAL encoder control */
    AMBA_VOUT_MIXER_REG_s                           MixerSection0;              /* 0x200-0x2FC: Mixer Section 0 control */
    AMBA_VOUT_DISPLAY_CONFIG_REG_s                  DisplaySection0;            /* 0x300-0x4FC: Display Section 0 control */
    AMBA_VOUT_MIXER_REG_s                           MixerSection1;              /* 0x500-0x5FC: Mixer Section 1 control */
    AMBA_VOUT_DISPLAY_CONFIG_REG_s                  DisplaySection1;            /* 0x600-0x7FC: Display Section 1 control */
    AMBA_VOUT_OSD_RESCALE_REG_s                     OsdRescaler;                /* 0x800-0x9FC: Controls OSD Rescaler */
    AMBA_VOUT_TOP_REG_s                             TopLevelCtrl;               /* 0xA00-0xAFC: Top level reset, and clock enables */
    AMBA_VOUT_MIPI_DSI_COMMAND_REG_s                MipiDsiCmd0;                /* 0xB00-0xB14: MIPI DSI Command 0 */
    AMBA_VOUT_DISPLAY_C_CONFIG_REG_s                DisplaySection2;            /* 0xC00-0xDFC: Display Section 2 control */
    AMBA_VOUT_MIPI_DSI_COMMAND_REG_s                MipiDsiCmd1;                /* 0xE00-0xE14: MIPI DSI Command 1 */
} AMBA_VOUT_REG_s;
#endif

/*
 * Defined in AmbaCSL_VOUT.c
 */
#if defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)
extern AMBA_VOUT_REG_s *                   pAmbaVout_Reg;
extern AMBA_VOUT_TVENC_CONTROL_REG_s *     pAmbaVoutTvEnc_Reg;
extern AMBA_VOUT_MIXER_REG_s *             pAmbaVoutMixer0_Reg;
extern AMBA_VOUT_DISPLAY_CONFIG_REG_s *    pAmbaVoutDisplay0_Reg;
extern AMBA_VOUT_MIXER_REG_s *             pAmbaVoutMixer1_Reg;
extern AMBA_VOUT_DISPLAY_CONFIG_REG_s *    pAmbaVoutDisplay1_Reg;
extern AMBA_VOUT_OSD_RESCALE_REG_s *       pAmbaVoutOsdRescale_Reg;
extern AMBA_VOUT_TOP_REG_s *               pAmbaVoutTop_Reg;
extern AMBA_VOUT_MIPI_DSI_COMMAND_REG_s *  pAmbaVoutMipiDsiCmd0_Reg;
extern AMBA_VOUT_DISPLAY_C_CONFIG_REG_s *  pAmbaVoutDisplay2_Reg;
extern AMBA_VOUT_MIPI_DSI_COMMAND_REG_s *  pAmbaVoutMipiDsiCmd1_Reg;
#else
extern AMBA_VOUT_REG_s *                   pAmbaVout_Reg;
extern AMBA_VOUT_TVENC_CONTROL_REG_s *     pAmbaVoutTvEnc_Reg;
extern AMBA_VOUT_MIXER_REG_s *             pAmbaVoutMixer0_Reg;
extern AMBA_VOUT_DISPLAY0_CONFIG_REG_s *   pAmbaVoutDisplay0_Reg;
#if !defined(CONFIG_SOC_CV28)
extern AMBA_VOUT_MIXER_REG_s *             pAmbaVoutMixer1_Reg;
extern AMBA_VOUT_DISPLAY1_CONFIG_REG_s *   pAmbaVoutDisplay1_Reg;
#endif
extern AMBA_VOUT_OSD_RESCALE_REG_s *       pAmbaVoutOsdRescale_Reg;
extern AMBA_VOUT_TOP_REG_s *               pAmbaVoutTop_Reg;
#if !defined(CONFIG_SOC_H32)
extern AMBA_VOUT_MIPI_DSI_COMMAND_REG_s *  pAmbaVoutMipiDsiCmd_Reg;
#endif
#endif

#endif /* AMBA_REG_VOUT_H */
