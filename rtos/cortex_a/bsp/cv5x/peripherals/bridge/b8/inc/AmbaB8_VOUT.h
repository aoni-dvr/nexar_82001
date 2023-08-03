/**
 *  @file AmbaB8_VOUT.h
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
 *  @details Definitions & Constants for B6 VOUT Control APIs
 *
 */

#ifndef AMBA_B8_VOUT_H
#define AMBA_B8_VOUT_H

#define B8_VOUT_CHANNEL0                    0U
#define B8_VOUT_CHANNEL1                    1U
#define AMBA_NUM_B8_VOUT_CHANNEL            2U

/* Definitions for vout digital output mode */
#define B8_VOUT_MIPI_RAW_10BIT              0U
#define B8_VOUT_MIPI_RAW_8BIT               9U
#define B8_VOUT_MIPI_RGB_24BIT              10U
#define B8_VOUT_MIPI_YUV_16BIT              11U
#define B8_VOUT_MIPI_RGB_16BIT              12U
#define B8_VOUT_MIPI_RGB_18BIT              13U
#define B8_VOUT_MIPI_RAW_12BIT              14U
#define B8_VOUT_FPD_LINK                    15U

/* Definitions for B8 VOUT MIPI CSI-2 modes */
#define B8_VOUT_MIPI_CSI_MODE_422_1LANE     0U
#define B8_VOUT_MIPI_CSI_MODE_422_2LANE     1U
#define B8_VOUT_MIPI_CSI_MODE_422_4LANE     2U
#define B8_VOUT_NUM_MIPI_CSI_MODE           3U

/* Definitions for B8 VOUT FPD-Link modes */
#define B8_VOUT_FPD_LINK_MODE_JEIDA         0U
#define B8_VOUT_FPD_LINK_MODE_VEISA         1U
#define B8_VOUT_NUM_FPD_LINK_MODE           2U

/* Definitions for B8 VOUT Source Select */
#define B8_VOUT_SOURCE_MERGER               0U
#define B8_VOUT_SOURCE_MERGER_CODEC         1U
#define B8_VOUT_SOURCE_BYPASS_VIN0          2U
#define B8_VOUT_SOURCE_BYPASS_VIN1          3U
#define AMBA_NUM_B8_VOUT_SOURCE             4U

#define B8_VOUT_MIPI_1_LANE                 0U
#define B8_VOUT_MIPI_2_LANE                 1U
#define B8_VOUT_MIPI_4_LANE                 2U
#define B8_VOUT_MIPI_8_LANE                 3U

/* Display Command Set (DCS) */
/* User Command Set: 00h to AFh */
#define MIPI_DCS_NOP                   (0x00U)
#define MIPI_DCS_SOFT_RESET            (0x01U)
#define MIPI_DCS_GET_RED_CHANNEL       (0x06U)
#define MIPI_DCS_GET_GREEN_CHANNEL     (0x07U)
#define MIPI_DCS_GET_BLUE_CHANNEL      (0x08U)
#define MIPI_DCS_GET_POWER_MODE        (0x0AU)
#define MIPI_DCS_GET_ADDRESS_MODE      (0x0BU)
#define MIPI_DCS_GET_PIXEL_FORMAT      (0x0CU)
#define MIPI_DCS_GET_DISPLAY_MODE      (0x0DU)
#define MIPI_DCS_GET_SIGNAL_MODE       (0x0EU)
#define MIPI_DCS_GET_DIAGNOSTIC_RESULT (0x0FU)
#define MIPI_DCS_ENTER_SLEEP_MODE      (0x10U)
#define MIPI_DCS_EXIT_SLEEP_MODE       (0x11U)
#define MIPI_DCS_ENTER_PARTIAL_MODE    (0x12U)
#define MIPI_DCS_ENTER_NORMAL_MODE     (0x13U)
#define MIPI_DCS_EXIT_INVERT_MODE      (0x20U)
#define MIPI_DCS_ENTER_INVERT_MODE     (0x21U)
#define MIPI_DCS_SET_GAMMA_CURVE       (0x26U)
#define MIPI_DCS_SET_DISPLAY_OFF       (0x28U)
#define MIPI_DCS_SET_DISPLAY_ON        (0x29U)
#define MIPI_DCS_SET_COLUMN_ADDRESS    (0x2AU)
#define MIPI_DCS_SET_PAGE_ADDRESS      (0x2BU)
#define MIPI_DCS_WRITE_MEMORY_START    (0x2CU)
#define MIPI_DCS_WRITE_LUT             (0x2DU)
#define MIPI_DCS_READ_MEMORY_START     (0x2EU)
#define MIPI_DCS_SET_PARTIAL_ROWS      (0x30U)
#define MIPI_DCS_SET_PARTIAL_COLUMNS   (0x31U)
#define MIPI_DCS_SET_SCROLL_AREA       (0x33U)
#define MIPI_DCS_SET_TEAR_OFF          (0x34U)
#define MIPI_DCS_SET_TEAR_ON           (0x35U)
#define MIPI_DCS_SET_ADDRESS_MODE      (0x36U)
#define MIPI_DCS_SET_SCROLL_START      (0x37U)
#define MIPI_DCS_EXIT_IDLE_MODE        (0x38U)
#define MIPI_DCS_ENTER_IDLE_MODE       (0x39U)
#define MIPI_DCS_SET_PIXEL_FORMAT      (0x3AU)
#define MIPI_DCS_WRITE_MEMORY_CONTINUE (0x3CU)
#define MIPI_DCS_READ_MEMORY_CONTINUE  (0x3EU)
#define MIPI_DCS_SET_TEAR_SCANLINE     (0x44U)
#define MIPI_DCS_GET_SCANLINE          (0x45U)
#define MIPI_DCS_READ_DDB_START        (0xA1U)
#define MIPI_DCS_READ_DDB_CONTINUE     (0xA8U)

typedef struct {
    UINT32 DataRate;                /* bit clock frequency */
    UINT16 NumDataLanes;            /* active data channels */
    UINT16 NumDataBits;             /* pixel data bit depth */
} B8_VOUT_DATA_INFO;

typedef struct {
    B8_VOUT_DATA_INFO Input;        /* Vout Input data info */
    B8_VOUT_DATA_INFO Output;       /* Vout Output data info */
    UINT32 Source;                  /* 0: merger w/o codec, 1:merger w/ codec, 2: bypass(vin), defined by B8_VOUT_SOURCE_xxx */
    UINT16 LineLengthPck;           /* Number of pixel clock cycles per line */
    UINT16 FrameLengthLines;        /* Number of lines per frame */
    UINT16 OutputWidth;             /* Valid pixels per line */
    UINT16 OutputHeight;            /* Valid lines per frame */
} B8_VOUT_CONFIG_s;

typedef struct {
    UINT32  PixelClock;             /* Pixel clock frequency */
    UINT8   PixelRepetition;        /* Pixel repetition factor */

    UINT16  Htotal;                 /* Number of columns per row */
    UINT16  Vtotal;                 /* Number of rows per field */

    UINT16  HsyncColStart;          /* Start column of Hsync pulse */
    UINT16  HsyncColEnd;            /* End column of Hsync pluse */

    UINT16  VsyncColStart;          /* Start column of Vsync pulse */
    UINT16  VsyncColEnd;            /* End column of Vsync pulse */
    UINT16  VsyncRowStart;          /* Start row of Vsync pulse */
    UINT16  VsyncRowEnd;            /* End row of Vsync pulse */

    UINT16  ActiveColStart;         /* Start column of active region */
    UINT16  ActiveColWidth;         /* End column of active region */
    UINT16  ActiveRowStart;         /* Start row of active region */
    UINT16  ActiveRowHeight;        /* End row of active region */
} AMBA_B8_VIDEO_TIMING_s;

typedef struct {
    UINT8   HsyncPolarity;      /* [0] 0: HSync asserted low, 1: HSync asserted high */
    UINT8   VsyncPolarity;      /* [1] 0: VSync asserted low, 1: VSync asserted high */
    UINT8   HvldPolarity;       /* [0] 0: VD1_HVLD asserted low, 1: VD1_HVLD asserted high */
} AMBA_B8_VOUT_POLARITY_CTRL_s;

typedef struct {
    UINT8   FourthLaneEnable;
    UINT8   BitOrderReverse;
    UINT8   FpdLinkMode;            /* 0: VESA, 1: JEIDA */
    UINT8   FpdDualPortEn;          /* 0: 1 fpd port, 4 lane output, 1: 2 fpd ports, 8 lane output */
    UINT8   FpdDualPortCtrl;        /* 1: {DE, VSYNC, HSYNS} will show on the senond port */
} AMBA_B8_VOUT_FPDLINK_CTRL_s;

typedef struct {
    /* Misc control */
    UINT8   MipiSyncEndEnable;
    UINT8   MipiEotpEnable;
    UINT8   MipiLaneNum;                    /* 0:1 lane, 1: 2 lanes, 2: 4lanes, 3: 8lanes */
    UINT8   MipiCsiEnable;
    /* Blankiing control */
    UINT16  PayloadSizeAtHsyncBackPorch;    /* The payload size of the blanking packet sent during hsync back porch.
                                                Negative if not use. */
    UINT16  PayloadSizeAtHsyncPulse;        /* The payload size of the blanking packet sent during hsync sync pulse.
                                                Negative if not use. */
    UINT16  PayloadSizeAtHsyncFrontPorch;   /* The payload size of the blanking packet sent during hsync front porch.
                                                Negative if not use. */
    UINT16  PayloadSizeAtVblankLines;       /* The payload size of the blanking packet sent during vblank lines.
                                                Negative if not use. */
} AMBA_B8_VOUT_MIPI_CTRL_s;

typedef struct {
    FLOAT Coef[3][3];
    FLOAT Offset[3];
    UINT16 MinVal[3];
    UINT16 MaxVal[3];
} AMBA_B8_VOUT_CSC_MATRIX_s;

typedef struct {
    UINT32                              VoutChannel;
    UINT8                               OutputMode;
    AMBA_B8_VIDEO_TIMING_s              VideoTiming;
    AMBA_B8_VOUT_POLARITY_CTRL_s        PolarityCtrl;
    AMBA_B8_VOUT_MIPI_CTRL_s            MipiCtrl;           /* Necessary when output MIPI */
    AMBA_B8_VOUT_FPDLINK_CTRL_s         FpdLinkCtrl;        /* Necessary when output FPD-Link */
    UINT16                              OutputDataChannal;  /* Output data channels */
    AMBA_B8_VOUT_CSC_MATRIX_s           *pCscMatrix;
} AMBA_B8_VOUT_DISPLAY_CONFIG_s;

typedef struct {
    UINT8 ClkPrepareCtrl;
    UINT8 ClkZeroCtrl;
    UINT8 ClkTrailCtrl;
    UINT8 HsPrepareCtrl;
    UINT8 HsZeroCtrl;
    UINT8 HsTrailCtrl;
    UINT8 HsLpxCtrl;
    UINT8 TxInitCtrl;
} B8_VOUT_MIPI_TX_CONFIG_s;

/*---------------------------------------------------------------------------*\
 * Defined in AmbaB8_VOUT.c
\*---------------------------------------------------------------------------*/
UINT32 AmbaB8_VoutEnable(UINT32 ChipID, UINT32 Channel);
UINT32 AmbaB8_VoutMipiCsiConfig(UINT32 ChipID, UINT32 Channel, const B8_VOUT_CONFIG_s *pVoutConfig);
UINT32 AmbaB8_VoutFpdLinkConfig(UINT32 ChipID, const AMBA_B8_VOUT_DISPLAY_CONFIG_s *pDisplayConfig);
UINT32 AmbaB8_VoutMipiDsiConfig(UINT32 ChipID, const AMBA_B8_VOUT_DISPLAY_CONFIG_s *pDisplayConfig);
UINT32 AmbaB8_VoutMipiDsiDcsWrite(UINT32 ChipID, UINT32 Channel, UINT32 DcsCmd, UINT32 NumParam, const UINT8 *pParam);
UINT32 AmbaB8_VoutSourceConfig(UINT32 ChipID, UINT32 VoutChannel, UINT32 VoutSource, const AMBA_B8_VOUT_DISPLAY_CONFIG_s *pDisplayConfig);

#endif  /* AMBA_B8_VOUT_H */
