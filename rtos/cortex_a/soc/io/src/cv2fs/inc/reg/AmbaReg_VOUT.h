/*
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
 */

#ifndef AMBA_REG_VOUT_H
#define AMBA_REG_VOUT_H

#ifndef AMBA_TYPES_H
#include "AmbaTypes.h"
#endif

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
typedef struct {
    UINT32  Enable:                 1;  /* [0] 1 = Enable clock signal */
    UINT32  Reserved:               31; /* [31:1] Reserved */
} AMBA_VOUT_CLK_ENABLE_REG_s;

/*
 * VOUT: Clock Disable Control Register
 */
typedef struct {
    UINT32  Disable:                1;  /* [0] 1 = Disable clock signal */
    UINT32  Reserved:               31; /* [31:1] Reserved */
} AMBA_VOUT_CLK_DISABLE_REG_s;

/* VOUT: Error Control
 */
typedef struct {
    UINT32  Clear:                  1;  /* [0] 1 = Clear vout safety error */
    UINT32  InjErr:                 1;  /* [1] 1 = Assert vout safety error to CEHU */
    UINT32  Reserved:               30; /* [31:2] Reserved */
} AMBA_VOUT_ERR_CTRL_REG_s;

/* VOUT: Error Status Bits
 */
typedef struct {
    UINT32  TopMiscErr:             1;  /* [0] 1 = Error detected in other top level modules, like sync counter interface, reset controller, ahb_interface */
    UINT32  OsdScalerErr:           1;  /* [1] 1 = Error detected in osd_rescale module */
    UINT32  MixerAErr:              1;  /* [2] 1 = Error detected in mixer_a module */
    UINT32  MixerBErr:              1;  /* [3] 1 = Error detected in mixer_b module */
    UINT32  DisplayAErr:            1;  /* [4] 1 = Error detected in display_a module */
    UINT32  DisplayBErr:            1;  /* [5] 1 = Error detected in display_b module */
    UINT32  SmemIfErr:              1;  /* [6] 1 = Error detected in smem interface module */
    UINT32  Reserved:               25; /* [31:7] Reserved */
} AMBA_VOUT_ERR_BIT_REG_s;

/*
 * VOUT: Mixer Section 0/1 Control Register
 */
typedef struct {
    UINT32  Input444:               1;  /* [0] 0 = YC 4:2:2, 1 = YC 4:4:4 or RGB */
    UINT32  CscEnable:              2;  /* [2:1] 0 = CSC disabled, 1 = CSC enabled for video, 2 = CSC enabled for OSD */
    UINT32  VideoFlip:              1;  /* [3] 0 = Normal mode, 1 = Video data is horizontally reversed */
    UINT32  Reserved:               27; /* [30:4] Reserved */
    UINT32  Reset:                  1;  /* [31] 1 = Reset the mixer section */
} AMBA_VOUTM_CTRL_REG_s;

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
    UINT32  Reserved0:              2;  /* [15:14] Reserved */
    UINT32  NumActivePixels:        14; /* [29:16] Number of pixels per line minus 1 */
    UINT32  Reserved1:              2;  /* [31:30] Reserved */
} AMBA_VOUTM_ACTIVE_SIZE_REG_s;

/*
 * VOUT: Mixer Section 0/1 Video/OSD Region Location Register
 */
typedef struct {
    UINT32  LineLocation:           14; /* [13:0] Starting/Ending line of the region */
    UINT32  Reserved0:              2;  /* [15:14] Reserved */
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

/*
 * VOUT: Display Section 0/1 Control Register
 */
typedef struct {
    UINT32  FixedFormatSelect:      5;  /* [4:0] 0 = Software configurable, 1-12 = Fixed/Standard format */
    UINT32  Interlaced:             1;  /* [5] 0 = Progressive, 1 = Interlaced */
    UINT32  FlipMode:               1;  /* [6] 1 = Enable horizontal flip */
    UINT32  GammaMapping:           1;  /* [7] 1 = Enable gamma mapping */
    UINT32  HalfPixShift:           1;  /* [8] 1 = Enable half-pixel shift */
    UINT32  HalfPixShiftLines:      1;  /* [9] Apply half-pixel shift to 0 = Odd lines, 1 = Even lines */
    UINT32  Format3D:               2;  /* [11:10] 0 = 2D, 1 = 3D side by side, 2 = 3D top and bottom, 3 = 3D frame packed */
    UINT32  Fpd4thLaneEnable:       1;  /* [12] 1 = Enable 4th output lane in FPD link */
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
    UINT32  DigitalOutput:          1;  /* [27] 1 = Enable digital output */
    UINT32  Reserved2:              3;  /* [30:28] Reserved */
    UINT32  ResetSection:           1;  /* [31] 1 = Display section software reset */
} AMBA_VOUTD_CTRL_REG_s;

/*
 * VOUT: Display Section 0/1 Status Register
 */
typedef struct {
    UINT32  Reserved:               4;  /* [3:0] Reserved */
    UINT32  GammaCfgReady:          1;  /* [4] 1 = Gamma table is safe to write */
    UINT32  MipiFifoOverflow:       1;  /* [5] Mipi FIFO overflow has occurred */
    UINT32  WatchdogTimeout:        1;  /* [6] One frame takes longer to finish than the programmed watchdog timer threshold*/
    UINT32  Reserved1:              22; /* [28:7] Reserved */
    UINT32  DigitalUnderflow:       1;  /* [29] 1 = Digital output underflow is detected */
    UINT32  Reserved2:              1;  /* [30] Reserved */
    UINT32  ResetComplete:          1;  /* [31] 1 = Display section is ready to be reprogrammed */
} AMBA_VOUTD_STATUS_REG_s;

/*
 * VOUT: Display Section 0/1 Output Frame Size Register
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
 * VOUT: Display Section 0/1 Active Region Register
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
 * VOUT: Display Output Mode Register
 */
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
    UINT32  OutputMode:             4;  /* [30:27] 0-8 = Reserved, 9-14 = MIPI modes, 15 = FPD-LINK */
    UINT32  Reserved1:              1;  /* [31] Reserved */
} AMBA_VOUTD_DIGITAL_OUTPUT_MODE_REG_s;

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
 * VOUT: Display Color Space Converter Parameter Register
 */
typedef struct {
    UINT32   CoefA0:                 13; /* [12:0] Signed 2.10 bits */
    UINT32   Reserved:               3;  /* [15:13] Reserved */
    UINT32   CoefA1:                 13; /* [28:16] Signed 2.10 bits */
    UINT32   Reserved1:              3;  /* [31:29] Reserved */
} AMBA_VOUTD_DIGITAL_CSC_PARAM_0_REG_s;

typedef struct {
    UINT32   CoefA2:                 13; /* [12:0] Signed 2.10 bits */
    UINT32   Reserved:               3;  /* [15:13] Reserved */
    UINT32   CoefA3:                 13; /* [28:16] Signed 2.10 bits */
    UINT32   Reserved1:              3;  /* [31:29] Reserved */
} AMBA_VOUTD_DIGITAL_CSC_PARAM_1_REG_s;

typedef struct {
    UINT32   CoefA4:                 13; /* [12:0] Signed 2.10 bits */
    UINT32   Reserved:               3;  /* [15:13] Reserved */
    UINT32   CoefA5:                 13; /* [28:16] Signed 2.10 bits */
    UINT32   Reserved1:              3;  /* [31:29] Reserved */
} AMBA_VOUTD_DIGITAL_CSC_PARAM_2_REG_s;

typedef struct {
    UINT32   CoefA6:                 13; /* [12:0] Signed 2.10 bits */
    UINT32   Reserved:               3;  /* [15:13] Reserved */
    UINT32   CoefA7:                 13; /* [28:16] Signed 2.10 bits */
    UINT32   Reserved1:              3;  /* [31:29] Reserved */
} AMBA_VOUTD_DIGITAL_CSC_PARAM_3_REG_s;

typedef struct {
    UINT32   CoefA8:                 13; /* [12:0] Signed 2.10 bits */
    UINT32   Reserved:               3;  /* [15:13] Reserved */
    UINT32   ConstB0:                15; /* [30:16] Signed 10 bits */
    UINT32   Reserved1:              1;  /* [31] Reserved */
} AMBA_VOUTD_DIGITAL_CSC_PARAM_4_REG_s;

typedef struct {
    UINT32   ConstB1:                15; /* [14:0] Signed 10 bits */
    UINT32   Reserved:               1;  /* [15] Reserved */
    UINT32   ConstB2:                15; /* [30:16] Signed 10 bits */
    UINT32   Reserved1:              1;  /* [31] Reserved */
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
typedef struct {
    UINT32  SmemInput:              1;  /* [0] 0 = Disable SMEM input, input from Mixer, 1 = Enable SMEM input */
    UINT32  ColorRes:               1;  /* [1] 0 = YUV422 input, 1 = YUV444 or RGB input */
    UINT32  Reserved:               30; /* [31:2] Reserved */
} AMBA_VOUTD_INPUT_CONFIG_REG_s;

/*
 * OSD Rescale: Filter Control Register
 */
typedef struct {
    UINT32  Enable:                 1;  /* [0] Enable both horizontal and vertical resampling */
    UINT32  VCoefShift:             1;  /* [1] Number format of vertical coefficient is: 0 = S0.7, 1 = S1.6 */
    UINT32  Reserved:               1;  /* [2] Reserved */
    UINT32  HCoefShift:             1;  /* [3] Number format of horizontal coefficient is: 0 = S0.7, 1 = S1.6 */
    UINT32  InputSelect:            2;  /* [5:4] 0 = Mixer A CLUT, 1 = Mixer A Direct, 2 = Mixer B CLUT, 3 = Mixer B Direct */
    UINT32  Reserved1:              26; /* [31:6] Reserved */
} AMBA_OSD_RESCALE_CTRL_REG_s;

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
    volatile UINT32                                 Reserved[9];                /* 0xA1C-0xA3C: Reserved */
    volatile AMBA_VOUT_ERR_CTRL_REG_s               EcrCtrl;                    /* 0xA40(RW): Final Error Valid and Clear Error Register */
    volatile UINT32                                 EcrErr;                     /* 0xA44(RW): Error Bits Register */
    volatile UINT32                                 EcrMask;                    /* 0xA48(RW): Error Bits Mask Register */
    volatile UINT32                                 Reserved1[45];              /* 0xA4C-0xAFC: Reserved */
} AMBA_VOUT_TOP_REG_s;

/*
 * VOUT: Mixer Registers
 */
typedef struct {
    volatile AMBA_VOUTM_CTRL_REG_s                  Ctrl;                       /* 0x200/0x500(RW): Mixer Section 0/1 Control Register */
    volatile AMBA_VOUTM_STATUS_REG_s                Status;                     /* 0x204/0x504(RO): Mixer Section 0/1 Status Register */
    volatile AMBA_VOUTM_ACTIVE_SIZE_REG_s           ActiveSize;                 /* 0x208/0x508(RW): Mixer Section 0/1 Active Region Size Register */
    volatile AMBA_VOUTM_REGION_LOCATION_REG_s       VideoWinStart;              /* 0x20C/0x50C(RW): Mixer Section 0/1 Video Region Start Location Register */
    volatile AMBA_VOUTM_REGION_LOCATION_REG_s       VideoWinEnd;                /* 0x210/0x510(RW): Mixer Section 0/1 Video Region End Location Register */
    volatile UINT32                                 Reserved0[2];               /* Reserved */
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

typedef struct {
    volatile UINT32         Header;                     /* 0xB00/0xE00(RW): Header registers of mipi command */
    volatile UINT32         Param[4];                   /* 0xB04-0xB10/0xE04-0xE10(RW): Mipi command parameter 0 */
    volatile UINT32         Ctrl;                       /* 0xB14/0xE14(RW): Mipi command control */
    volatile UINT32         Reserved[58];               /* 0xB18-0xBFC/0xE18-0xEFC: Reserved */
} AMBA_VOUT_MIPI_DSI_COMMAND_REG_s;

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
    volatile UINT32                                 Reserved0[2];               /* Reserved */
    volatile UINT32                                 MipiMaxFrameCount;          /* 0x344/0x644(RW): For MIPI CSI Frame Count. (1, 2, 3, 4, N-1, N) */
    volatile UINT32                                 Reserved1[3];               /* Reserved */
    volatile AMBA_VOUTD_DIGITAL_CSC_PARAM_0_REG_s   DigitalCSC0;                /* 0x354/0x654(WO): Color-Space-Conversion Setting for Digital Output Registers */
    volatile AMBA_VOUTD_DIGITAL_CSC_PARAM_1_REG_s   DigitalCSC1;                /* 0x358/0x658(WO): Color-Space-Conversion Setting for Digital Output Registers */
    volatile AMBA_VOUTD_DIGITAL_CSC_PARAM_2_REG_s   DigitalCSC2;                /* 0x35C/0x65C(WO): Color-Space-Conversion Setting for Digital Output Registers */
    volatile AMBA_VOUTD_DIGITAL_CSC_PARAM_3_REG_s   DigitalCSC3;                /* 0x360/0x660(WO): Color-Space-Conversion Setting for Digital Output Registers */
    volatile AMBA_VOUTD_DIGITAL_CSC_PARAM_4_REG_s   DigitalCSC4;                /* 0x364/0x664(WO): Color-Space-Conversion Setting for Digital Output Registers */
    volatile AMBA_VOUTD_DIGITAL_CSC_PARAM_5_REG_s   DigitalCSC5;                /* 0x368/0x668(WO): Color-Space-Conversion Setting for Digital Output Registers */
    volatile AMBA_VOUTD_DIGITAL_CSC_PARAM_6_REG_s   DigitalCSC6;                /* 0x36C/0x66C(WO): Color-Space-Conversion Setting for Digital Output Registers */
    volatile AMBA_VOUTD_DIGITAL_CSC_PARAM_7_REG_s   DigitalCSC7;                /* 0x370/0x670(WO): Color-Space-Conversion Setting for Digital Output Registers */
    volatile AMBA_VOUTD_DIGITAL_CSC_PARAM_8_REG_s   DigitalCSC8;                /* 0x374/0x674(WO): Color-Space-Conversion Setting for Digital Output Registers */
    volatile UINT32                                 WatchdogTimeout;            /* 0x378/0x678(RW): Threshold Value for Vout Display Frame Time */
    volatile UINT32                                 DummyAccessCore;            /* 0x37C/0x67C(RW): Core Clock Domain Reset Rest Register */
    volatile UINT32                                 DummyAccessClkvo;           /* 0x380/0x680(RW): Clk_vo Domain Reset Test Register */
    volatile UINT32                                 DummyAccessByteclk;         /* 0x384/0x684(RW): Mipi Byteclock Domain Reset Test Register */
    volatile UINT32                                 Reserved2[51];              /* Reserved */
    volatile AMBA_VOUTD_DITHER_CTRL_REG_s           DitherCtrl;                 /* 0x454/0x754(RW): Dithering Settings for Digital Output */
    volatile UINT32                                 DitherSeed;                 /* 0x458/0x758(RW): Dithering Seed */
    volatile AMBA_VOUTD_VOUT_VOUT_SYNC_REG_s        VoutSyncFromVoutB;          /* 0x45C/0x75C(RW): Sync Signal Receiving from Display Section 1 Control Register */
    volatile AMBA_VOUTD_MIPI_BLANK_CTRL_REG_s       MipiBlankCtrl0;             /* 0x460/0x760(RW): HBP and HSync Blankings */
    volatile AMBA_VOUTD_MIPI_BLANK_CTRL_REG_s       MipiBlankCtrl1;             /* 0x464/0x764(RW): HFP and Vertical Blankings */
    volatile UINT32                                 Reserved3[6];               /* Reserved */
    volatile AMBA_VOUTD_INPUT_CONFIG_REG_s          InputConfig;                /* 0x480/0x780(RW): Input Source and Color Resolution */
    volatile AMBA_VOUT_INPUT_SYNC_CTRL_REG_s        InputSyncCounterCtrl;       /* 0x484/0x784(RW): Control Input Synchronization Register */
    volatile AMBA_VOUT_OUTPUT_SYNC_CTRL_REG_s       OutputSyncCounterCtrl;      /* 0x488/0x788(RW): Control Output Synchronization Register */
    volatile UINT32                                 StreamCtrl;                 /* 0x48C/0x78C(RW): Input SMEM Channel Settings */
    volatile UINT32                                 ForceBackground;            /* 0x490/0x790(RW): Force to output background color */
    volatile UINT32                                 Reserved4;                  /* Reserved */
    volatile UINT32                                 OutputEnable;               /* 0x498/0x798(RW): Enable Output of one frame Register */
    volatile UINT32                                 LumaChecksum;               /* 0x49C/0x79C(RO): Luma Checksum */
    volatile UINT32                                 ChromaChecksum;             /* 0x4A0/0x7A0(RO): Chroma Checksum */
    volatile UINT32                                 Reserved5[23];              /* 0x4A4-0x4FC/0x7A4-0x7FC: Reserved */
} AMBA_VOUT_DISPLAY_CONFIG_REG_s;

/*
 * VOUT: All Blocks
 */
typedef struct {
    AMBA_VOUT_MIXER_REG_s                           MixerSection0;              /* 0x200-0x2FC: Mixer Section 0 control */
    AMBA_VOUT_DISPLAY_CONFIG_REG_s                  DisplaySection0;            /* 0x300-0x4FC: Display Section 0 control */
    AMBA_VOUT_MIXER_REG_s                           MixerSection1;              /* 0x500-0x5FC: Mixer Section 1 control */
    AMBA_VOUT_DISPLAY_CONFIG_REG_s                  DisplaySection1;            /* 0x600-0x7FC: Display Section 1 control */
    AMBA_VOUT_OSD_RESCALE_REG_s                     OsdRescaler;                /* 0x800-0x9FC: Controls OSD Rescaler */
    AMBA_VOUT_TOP_REG_s                             TopLevelCtrl;               /* 0xA00-0xAFC: Top level reset, and clock enables */
    AMBA_VOUT_MIPI_DSI_COMMAND_REG_s                MipiDsiCmd0;                /* 0xB00-0xB14: MIPI DSI Command 0 */
    AMBA_VOUT_MIPI_DSI_COMMAND_REG_s                MipiDsiCmd1;                /* 0xE00-0xE14: MIPI DSI Command 1 */
} AMBA_VOUT_REG_s;

/*
 * Defined in AmbaMmioBase.asm
 */
#ifdef CONFIG_QNX
extern AMBA_VOUT_REG_s *pAmbaVout_Reg;

extern AMBA_VOUT_MIXER_REG_s *             pAmbaVoutMixer0_Reg;
extern AMBA_VOUT_DISPLAY_CONFIG_REG_s *    pAmbaVoutDisplay0_Reg;
extern AMBA_VOUT_MIXER_REG_s *             pAmbaVoutMixer1_Reg;
extern AMBA_VOUT_DISPLAY_CONFIG_REG_s *    pAmbaVoutDisplay1_Reg;
extern AMBA_VOUT_OSD_RESCALE_REG_s *       pAmbaVoutOsdRescale_Reg;
extern AMBA_VOUT_TOP_REG_s *               pAmbaVoutTop_Reg;
extern AMBA_VOUT_MIPI_DSI_COMMAND_REG_s *  pAmbaVoutMipiDsiCmd0_Reg;
extern AMBA_VOUT_MIPI_DSI_COMMAND_REG_s *  pAmbaVoutMipiDsiCmd1_Reg;
#else
extern AMBA_VOUT_REG_s *const pAmbaVout_Reg;

extern AMBA_VOUT_MIXER_REG_s *             const pAmbaVoutMixer0_Reg;
extern AMBA_VOUT_DISPLAY_CONFIG_REG_s *    const pAmbaVoutDisplay0_Reg;
extern AMBA_VOUT_MIXER_REG_s *             const pAmbaVoutMixer1_Reg;
extern AMBA_VOUT_DISPLAY_CONFIG_REG_s *    const pAmbaVoutDisplay1_Reg;
extern AMBA_VOUT_OSD_RESCALE_REG_s *       const pAmbaVoutOsdRescale_Reg;
extern AMBA_VOUT_TOP_REG_s *               const pAmbaVoutTop_Reg;
extern AMBA_VOUT_MIPI_DSI_COMMAND_REG_s *  const pAmbaVoutMipiDsiCmd0_Reg;
extern AMBA_VOUT_MIPI_DSI_COMMAND_REG_s *  const pAmbaVoutMipiDsiCmd1_Reg;
#endif
#endif /* AMBA_REG_VOUT_H */
