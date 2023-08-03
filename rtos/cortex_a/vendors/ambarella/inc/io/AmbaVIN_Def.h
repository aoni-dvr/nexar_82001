/**
 *  @file AmbaVIN_Def.h
 *
 *  Copyright (c) 2020 Ambarella International LP
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
 *  @details Common Definitions & Constants for Video Input APIs
 *
 */
#ifndef AMBA_VIN_DEF_H
#define AMBA_VIN_DEF_H

#ifndef AMBA_TYPES_H
#include "AmbaTypes.h"
#endif
#ifndef AMBA_ERROR_CODE_H
#include "AmbaErrorCode.h"
#endif

#ifndef AMBA_VIN_PRIV_H
#include "AmbaVIN_Priv.h"
#endif

#define VIN_ERR_0000    (VIN_ERR_BASE)
#define VIN_ERR_0001    (VIN_ERR_BASE | 0x1U)
#define VIN_ERR_0002    (VIN_ERR_BASE | 0x2U)
#define VIN_ERR_0003    (VIN_ERR_BASE | 0x3U)
#define VIN_ERR_0004    (VIN_ERR_BASE | 0x4U)
#define VIN_ERR_0005    (VIN_ERR_BASE | 0x5U)
#define VIN_ERR_0006    (VIN_ERR_BASE | 0x6U)
#define VIN_ERR_0007    (VIN_ERR_BASE | 0x7U)

#define VIN_ERR_NONE        0U
#define VIN_MODULE_ID       ((UINT16)(VIN_ERR_BASE >> 16U))
#define VIN_ERR_ARG         VIN_ERR_0000    /* Invalid argument */
#define VIN_ERR_MUTEX       VIN_ERR_0001    /* Unable to do concurrency protection */
#define VIN_ERR_DEV_CTL     VIN_ERR_0002    /* error reported by QNX/Linux devctl()/ioctl() */
#define VIN_ERR_OPEN_FILE   VIN_ERR_0003    /* error reported by QNX/Linux open() */
#define VIN_ERR_UNEXPECTED  VIN_ERR_0004    /* Unexpected error */
#define VIN_ERR_OSERR       VIN_ERR_0005    /* System resource not available */
#define VIN_ERR_INVALID_API VIN_ERR_0006    /* Invalid API */
#define VIN_ERR_TIMEOUT     VIN_ERR_0007    /* Timeout occurred */

#define AMBA_VIN_TERMINATION_VALUE_MIN      1U
#define AMBA_VIN_TERMINATION_VALUE_MAX      11U

#define AMBA_VIN_MAIN_BUF_SIZE              128U

typedef struct {
    UINT8   HsSettleTime;                       /* MIPI HS-SETTLE time */
    UINT8   HsTermTime;                         /* MIPI HS-TERM time */
    UINT8   ClkSettleTime;                      /* MIPI CLK-SETTLE time */
    UINT8   ClkTermTime;                        /* MIPI CLK-TERM time */
    UINT8   ClkMissTime;                        /* MIPI CLK-MISS time */
    UINT8   RxInitTime;                         /* MIPI RX-INIT time */
} AMBA_VIN_MIPI_TIMING_PARAM_s;

/* ----------------------------------------------------------------------------------- */
typedef struct {
    UINT32  EnabledPin;                 /* Enabled data pins */
} AMBA_VIN_SLVS_PAD_CONFIG_s;

#define AMBA_VIN_MIPI_CLK_MODE_NON_CONTINUOUS   0U
#define AMBA_VIN_MIPI_CLK_MODE_CONTINUOUS       1U
typedef struct {
    UINT32  ClkMode;                    /* MIPI Clock Mode. 0U: non-continuous mode, 1U: continuous mode */
    UINT64  DateRate;                   /* Input data rate in bps per lane */
    UINT32  EnabledPin;                 /* Enabled data pins */
} AMBA_VIN_MIPI_PAD_CONFIG_s;

#define AMBA_VIN_DVP_PAD_TYPE_LVCMOS    0U
#define AMBA_VIN_DVP_PAD_TYPE_LVDS      1U
typedef struct {
    UINT8   PadType;                    /* Pad Type. 0U: Parallel LVCMOS, 1U: Parallel LVDS */
} AMBA_VIN_DVP_PAD_CONFIG_s;

typedef struct {
    UINT64  DataRate;                   /* Input data rate in bps per lane */
    UINT32  EnabledPin;                 /* Enabled data pins */
} AMBA_VIN_SLVSEC_PAD_CONFIG_s;

typedef struct {
    UINT8   NumSplits;                  /* Number of splits to make */
    UINT16  SplitWidth;                 /* Split each input line to lines of SPLIT_WIDTH pixels */
} AMBA_VIN_SPLIT_CTRL_s;

typedef struct {
    UINT32  NumActivePixels;    /* Active region width */
    UINT32  NumActiveLines;     /* Active region height */
    UINT32  NumTotalPixels;     /* Horizontal total width including valid pixels and blanking */
    UINT32  NumTotalLines;      /* Vertical total height including valid lines and blanking */
} AMBA_VIN_RX_HV_SYNC_s;

typedef struct {
    UINT8   Interlace;                  /* 1 - Interlace; 0 - Progressive */
    UINT32  TimeScale;                  /* time scale */
    UINT32  NumUnitsInTick;             /* Frames per Second = TimeScale / (NumUnitsInTick * (1 + Interlace)) */
} AMBA_VIN_FRAME_RATE_s;

#define AMBA_VIN_COLOR_SPACE_RGB        0U
#define AMBA_VIN_COLOR_SPACE_YUV        1U
#define AMBA_VIN_COLOR_SPACE_RGBIR      2U
#define AMBA_VIN_COLOR_SPACE_RCCC       3U

/* for RGB input */
#define AMBA_VIN_BAYER_PATTERN_RG       0U
#define AMBA_VIN_BAYER_PATTERN_BG       1U
#define AMBA_VIN_BAYER_PATTERN_GR       2U
#define AMBA_VIN_BAYER_PATTERN_GB       3U

/* for RGB-IR input */
#define AMBA_VIN_BAYER_PATTERN_RGGI     0x10U
#define AMBA_VIN_BAYER_PATTERN_IGGR     0x11U
#define AMBA_VIN_BAYER_PATTERN_GRIG     0x12U
#define AMBA_VIN_BAYER_PATTERN_GIRG     0x13U
#define AMBA_VIN_BAYER_PATTERN_BGGI     0x14U
#define AMBA_VIN_BAYER_PATTERN_IGGB     0x15U
#define AMBA_VIN_BAYER_PATTERN_GBIG     0x16U
#define AMBA_VIN_BAYER_PATTERN_GIBG     0x17U

/* for RCCC input */
#define AMBA_VIN_BAYER_PATTERN_RCCC     0x20U
#define AMBA_VIN_BAYER_PATTERN_CCCR     0x21U
#define AMBA_VIN_BAYER_PATTERN_CRCC     0x22U
#define AMBA_VIN_BAYER_PATTERN_CCRC     0x23U

/* for YUV input */
#define AMBA_VIN_YUV_ORDER_Y0_CB_Y1_CR  0U
#define AMBA_VIN_YUV_ORDER_CR_Y0_CB_Y1  1U
#define AMBA_VIN_YUV_ORDER_CB_Y0_CR_Y1  2U
#define AMBA_VIN_YUV_ORDER_Y0_CR_Y1_CB  3U

typedef struct {
    AMBA_VIN_FRAME_RATE_s   FrameRate;
    UINT8                   ColorSpace;     /* Color space of the input data. 0U: RGB, 1U: YUVs, 2U: RGB-IR, 3U: RCCC */
    UINT8                   BayerPattern;   /* Color coding of the first 2x2 pixels */
    UINT8                   YuvOrder;       /* Pixel order */
    UINT32                  NumDataBits;    /* Bit resolution of the input pixel data */
    UINT8                   NumSkipFrame;   /* Number of frames to be skipped if VIN configuration is changed */
    AMBA_VIN_RX_HV_SYNC_s   RxHvSyncCtrl;   /* Input H/V sync signal format. */
    AMBA_VIN_SPLIT_CTRL_s   SplitCtrl;      /* VIN split function */
    UINT32                  DelayedVsync;   /* The value of DelayedVsync indicates the delay after VSYNC to assert
                                               "IDSP_VIN_DELAYED_ VSYNC" interrupt. (Unit: Sensor output clock)*/
} AMBA_VIN_MAIN_CONFIG_s;

typedef struct {
    UINT8   PatternAlign;       /* 0U:  Sync code mask/patterns are aligned with LSB,
                                   1U:  Sync code mask/patterns are aligned with MSB.*/
    UINT16  SyncCodeMask;       /* Sync code mask for detection. bit[n] =1U: ignore bit[n] of sync code
                                   when compare sync code pattern*/
    UINT8   SolDetectEnable;    /* 1U: Enable detection of SOL sync code */
    UINT8   EolDetectEnable;    /* 1U: Enable detection of EOL sync code */
    UINT8   SofDetectEnable;    /* 1U: Enable detection of SOF sync code */
    UINT8   EofDetectEnable;    /* 1U: Enable detection of EOF sync code */
    UINT8   SovDetectEnable;    /* 1U: Enable detection of SOV sync code */
    UINT8   EovDetectEnable;    /* 1U: Enable detection of EOV sync code */

    UINT16  PatternSol;         /* Sync code pattern for Start of active line */
    UINT16  PatternEol;         /* Sync code pattern for End of active line */
    UINT16  PatternSof;         /* Sync code pattern for Start of frame */
    UINT16  PatternEof;         /* Sync code pattern for End of frame */
    UINT16  PatternSov;         /* Sync code pattern for Start of vertical blanking line */
    UINT16  PatternEov;         /* Sync code pattern for End of vertical blanking line */
} AMBA_VIN_CUSTOM_SYNC_CODE_s;

typedef struct {
    UINT8                       SyncInterleaving;   /* Sync code position. 0U: exists on each lane,
                                                       1U: 2-lane interleaving, 2U: 4-lane interleaving */
    UINT8                       ITU656Type;         /* 0U: not ITU-656 type sync, provide the sync code format via "CustomSyncCode",
                                                       1U: ITU-656 type */
    AMBA_VIN_CUSTOM_SYNC_CODE_s CustomSyncCode;     /* Description of customized sync code */

} AMBA_VIN_SLVS_SYNC_CODE_s;

typedef struct {
    AMBA_VIN_MAIN_CONFIG_s      Config;         /* VIN main configuration */
    UINT8                       NumActiveLanes; /* Number of active data lanes */
    UINT8                       *pLaneMapping;  /* logical to physical lane mapping information */
    AMBA_VIN_SLVS_SYNC_CODE_s   SyncDetectCtrl; /* Sync code detection control */
} AMBA_VIN_SLVS_CONFIG_s;

typedef struct {
    UINT8   VirtChanNum;         /* Number of VC channel */
    UINT8   VCPattern1stExp;     /* Equal to the VC pattern of the first exposure frame */
    UINT8   VCPattern2ndExp;     /* Equal to the VC pattern of the second exposure frame */
    UINT8   VCPattern3rdExp;     /* Equal to the VC pattern of the third exposure frame */
    UINT8   VCPattern4thExp;     /* Equal to the VC pattern of the fourth exposure frame */

    UINT16  Offset2ndExp;        /* the second exposure offset compared to the first exposure */
    UINT16  Offset3rdExp;        /* the third exposure offset compared to the first exposure */
    UINT16  Offset4thExp;        /* the fourth exposure offset compared to the first exposure */
} AMBA_VIN_VC_HDR_CONFIG_s;

typedef struct {
    AMBA_VIN_MAIN_CONFIG_s   Config;             /* VIN main configuration */
    UINT8                    NumActiveLanes;     /* Number of active data lanes */
    UINT8                    DataType;           /* Data type defined in MIPI CSI-2 specification. */
    UINT8                    DataTypeMask;       /* Data type mask of MIPI packet.
                                                    bit[n] =1U: ignore bit[n] of Data Type when checking Data Type */
    UINT8                    VirtChanHDREnable;  /* 0U: VC HDR is not used,
                                                    1U: VC HDR is used, provide the configuration via VirtChanHDRConfig */
    AMBA_VIN_VC_HDR_CONFIG_s VirtChanHDRConfig;  /* Description of VC HDR configuration */
} AMBA_VIN_MIPI_CONFIG_s;

#define AMBA_VIN_DVP_SYNC_LOCATE_LOWER  0U
#define AMBA_VIN_DVP_SYNC_LOCATE_UPPER  1U
#define AMBA_VIN_DVP_SYNC_LOCATE_BOTH   2U
typedef struct {
    UINT8                       SyncLocate;     /* Indicate the sync code position. 0U: on lower pixel only,
                                                   1U: on upper pixel only, 2U: on both pixel only */
    UINT8                       ITU656Type;     /* 0U: not ITU-656 type sync code, provide sync code format via "CustomSyncCode" */
    AMBA_VIN_CUSTOM_SYNC_CODE_s CustomSyncCode; /* Description of customized sync code */
} AMBA_VIN_DVP_EMB_SYNC_CONFIG_s;

#define AMBA_VIN_DVP_POLARITY_ACTIVE_HIGH   0U
#define AMBA_VIN_DVP_POLARITY_ACTIVE_LOW    1U

typedef struct {
    UINT8       HsyncPinSelect;     /* Indicate the physical pin for Hsync */
    UINT8       VsyncPinSelect;     /* Indicate the physical pin for Vsync */
    UINT8       FieldPinSelect;     /* Indicate the physical pin for Field */
    UINT8       HsyncPolarity;      /* Indicate Line sync signal polarity */
    UINT8       VsyncPolarity;      /* Indicate Frame sync signal polarity */
    UINT8       FieldPolarity;      /* Indicate Field signal polarity */
} AMBA_VIN_DVP_SYNC_PIN_CONFIG_s;

#define AMBA_VIN_DVP_SYNC_TYPE_BT601    0U
#define AMBA_VIN_DVP_SYNC_TYPE_EMB_SYNC 1U
typedef struct {
    UINT8                           SyncType;       /* 0U: recognize valid pixels/lines with dedicated HSync/Vsync pin.
                                                       1U: recognize valid pixels/lines with Embedded sync code. */
    AMBA_VIN_DVP_SYNC_PIN_CONFIG_s  SyncPinConfig;  /* HSync/VSync/Field pin configuration. */
    AMBA_VIN_DVP_EMB_SYNC_CONFIG_s  EmbSyncConfig;  /* Embedded sync code configuration. */
} AMBA_VIN_DVP_SYNC_s;

#define AMBA_VIN_DVP_WIDE_1_PIXEL   0U
#define AMBA_VIN_DVP_WIDE_2_PIXELS  1U

#define AMBA_VIN_DVP_LATCH_EDGE_RISING  0U
#define AMBA_VIN_DVP_LATCH_EDGE_FALLING 1U
#define AMBA_VIN_DVP_LATCH_EDGE_BOTH    2U
typedef struct {
    AMBA_VIN_MAIN_CONFIG_s  Config;         /* VIN main configuration */
    UINT8                   PadType;        /* DVP Pad type. 0U: Single-ended signal, 1U: differential signal */
    UINT8                   DvpWide;        /* Parallel input wide. 0U: 1 pixel per pixel clock cycle,
                                                                    1U: 2 pixels per pixel clock cycle. */
    UINT8                   DataLatchEdge;  /* 0U: Latch data on rising edge of clock(SDR),
                                               1U: Latch data on falling edge of clock(SDR),
                                               2U: Latch data on both rising and falling edge of clock(DDR) */
    AMBA_VIN_DVP_SYNC_s      SyncDetectCtrl; /* H/V sync detection control */
} AMBA_VIN_DVP_CONFIG_s;

#define AMBA_VIN_SLVSEC_LINK_TYPE_S  0U             /* Single link */
#define AMBA_VIN_SLVSEC_LINK_TYPE_D  1U             /* Dual link (pixel interleave) */
#define AMBA_VIN_SLVSEC_LINK_TYPE_DX 2U             /* Dual link (pixel interleave, swap) */
typedef struct {
    AMBA_VIN_MAIN_CONFIG_s      Config;             /* VIN main configuration */
    UINT8                       NumActiveLanes;     /* Number of active data lanes */
    UINT8                       LinkType;           /* SLVS-EC link type to indicate single LINK or dual LINK */
    UINT8                       EccOption;          /* 0=ECC disabled, 1U=1t ECC, 2U=2t ECC */
} AMBA_VIN_SLVSEC_CONFIG_s;

typedef struct {
    UINT32  Period;     /* Period of Sync waveform. For HSync, the unit is cycle. For Vsync, the unit is Hsync. */
    UINT32  PulseWidth; /* Sync pulse width (unit: cycle) */
    UINT8   Polarity;   /* 0U: Active Low, 1U: Active High */
} AMBA_VIN_SYNC_WAVEFORM_s;

typedef struct {
    UINT32                      RefClk;                 /* Reference clock rate */
    AMBA_VIN_SYNC_WAVEFORM_s    HSync;                  /* Hsync waveform configuration */
    AMBA_VIN_SYNC_WAVEFORM_s    VSync;                  /* Vsync waveform configuration */
    UINT16                      HSyncDelayCycles;       /* Offset of Hsync pulse in cycles */
    UINT16                      VSyncDelayCycles;       /* Offset between Hsync and Vsync edges */
    UINT8                       ToggleHsyncInVblank;    /* 0U: HSync is un-toggled during Vertical Blank period,
                                                           1U: HSync is toggled during Vertical Blank period */
} AMBA_VIN_MASTER_SYNC_CONFIG_s;

typedef struct {
    UINT8   NumActiveLanes;     /* Number of active data lanes */
    UINT8*  pPhyLaneMapping;    /* logical to physical lane mapping information */

} AMBA_VIN_LANE_REMAP_CONFIG_s;

typedef struct {
    UINT8          VirtChan;     /* Virtual channel identifier of MIPI packet */
    UINT8          VirtChanMask; /* Virtual channel identifier mask of MIPI packet.
                                    bit[n] =1U: ignore bit[n] of Virtual channel identifier when checking Virtual channel identifier */
} AMBA_VIN_MIPI_VC_CONFIG_s;

#define AMBA_VIN_PAD_MODE_SLVS          0U
#define AMBA_VIN_PAD_MODE_MIPI          1U
#define AMBA_VIN_PAD_MODE_DVP_LVCMOS    2U
#define AMBA_VIN_PAD_MODE_DVP_LVDS      3U

#define AMBA_VIN_DRIVE_STRENGTH_2MA     0U
#define AMBA_VIN_DRIVE_STRENGTH_4MA     1U
#define AMBA_VIN_DRIVE_STRENGTH_8MA     2U
#define AMBA_VIN_DRIVE_STRENGTH_12MA    3U

typedef struct {
    UINT16  OffsetX;                    /* Horizontal offset of the window */
    UINT16  OffsetY;                    /* Vertical offset of the window */
    UINT16  Width;                      /* Number of pixels per line in the window */
    UINT16  Height;                     /* Number of lines in the window */
} AMBA_VIN_WINDOW_s;

typedef struct {
    AMBA_VIN_FRAME_RATE_s   FrameRate;
    UINT8                   ColorSpace;     /* Color space of the input data. 0U: RGB, 1U: YUVs, 2U: RGB-IR, 3U: RCCC */
    UINT8                   BayerPattern;   /* Color coding of the first 2x2 pixels */
    UINT8                   YuvOrder;       /* Pixel order */
    UINT32                  NumDataBits;    /* Bit resolution of the input pixel data */
    UINT8                   NumSkipFrame;   /* Number of frames to be skipped if VIN configuration is changed */
} AMBA_VIN_INFO_s;

#endif /* _AMBA_VIN_DEF_H_ */
