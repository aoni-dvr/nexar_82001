/**
 *  @file AmbaVOUT_Def.h
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
 *  @details Common Definitions & Constants for Video Output APIs
 *
 */

#ifndef AMBA_VOUT_DEF_H
#define AMBA_VOUT_DEF_H

#ifndef AMBA_TYPES_H
#include "AmbaTypes.h"
#endif
#ifndef AMBA_ERROR_CODE_H
#include "AmbaErrorCode.h"
#endif

#define AMBA_VOUT_CHANNEL0          0U
#define AMBA_VOUT_CHANNEL1          1U
#define AMBA_VOUT_CHANNEL2          2U

#define AMBA_VOUT_DISP_BUF_SIZE     512U
#define AMBA_VOUT_TVENC_BUF_SIZE    512U
#define AMBA_VOUT_CSC_BUF_SIZE      36U

#define VOUT_CSC_DATA_COUNT         18U

#define VOUT_ERR_0000               (VOUT_ERR_BASE)
#define VOUT_ERR_0001               (VOUT_ERR_BASE + 0x1U)
#define VOUT_ERR_0002               (VOUT_ERR_BASE + 0x2U)
#define VOUT_ERR_0003               (VOUT_ERR_BASE + 0x3U)
#define VOUT_ERR_0004               (VOUT_ERR_BASE + 0x4U)
#define VOUT_ERR_0005               (VOUT_ERR_BASE + 0x5U)
#define VOUT_ERR_0006               (VOUT_ERR_BASE + 0x6U)
#define VOUT_ERR_00FF               (VOUT_ERR_BASE + 0XFFU)      /* Unexpected error */

/* VOUT error values */
#define VOUT_ERR_NONE               0U
#define VOUT_MODULE_ID              ((UINT16)(VOUT_ERR_BASE >> 16U))
#define VOUT_ERR_ARG                VOUT_ERR_0000
#define VOUT_ERR_MUTEX              VOUT_ERR_0001
#define VOUT_ERR_PROTOCOL           VOUT_ERR_0002
#define VOUT_ERR_DEV_CTL            VOUT_ERR_0003
#define VOUT_ERR_OPEN_FILE          VOUT_ERR_0004
#define VOUT_ERR_OSERR              VOUT_ERR_0005
#define VOUT_ERR_INVALID_API        VOUT_ERR_0006
#define VOUT_ERR_UNEXPECTED         VOUT_ERR_00FF

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
/* Manufacturer Command Set: B0h to FFh */

/* Data Identifier (ID) (Virtual channel ID + Packet data type) */
#define MIPI_DSI_PKT_DCS_SHORTWRITE0        0x05U   /* Short write. 0 parameter */
#define MIPI_DSI_PKT_DCS_SHORTWRITE1        0x15U   /* Short write. 1 parameter */
#define MIPI_DSI_PKT_DCS_READ               0x06U   /* Read */
#define MIPI_DSI_PKT_GENERIC_NONIMANGE_DATA 0x29U   /* Generic non-image data (long) */
#define MIPI_DSI_PKT_DCS_LONGWRITE          0x39U   /* Long write */

/* BTA request enable can be from RCT or VOUT */
#define MIPI_DSI_BTA_ENABLE_FROM_RCT        0x0U
#define MIPI_DSI_BTA_ENABLE_FROM_VOUT       0x1U
#define NUM_MIPI_DSI_BTA_ENABLE             0x2U

/* MIPI DSI PHY tx modes */
#define MIPI_DSI_PHY_MODE_DSI               0x0U    /* MIPI DSI mode */
#define MIPI_DSI_PHY_MODE_CSI               0x0U    /* MIPI CSI mode */
#define MIPI_DSI_PHY_MODE_FPD               0x1U    /* FPD mode */
#define MIPI_DSI_PHY_MODE_SLVS              0x2U    /* Serial SLVS mode */
#define NUM_MIPI_DSI_PHY_MODE               0x3U

#define VOUT_SIGNAL_EDGE_LOW_TO_HIGH        0U      /* Positive/rising edge */
#define VOUT_SIGNAL_EDGE_HIGH_TO_LOW        1U      /* Negative/falling edge */
#define VOUT_SIGNAL_ACTIVE_LOW              0U      /* signal will be performing its function when its logic level is 0 */
#define VOUT_SIGNAL_ACTIVE_HIGH             1U      /* signal will be performing its function when its logic level is 1 */
#define VOUT_NUM_SIGNAL_MODE                2U

/* Definitions for VOUT Digital YCC modes */
#define VOUT_YCC_MODE_BT601_24BIT           0U      /* BT.601, YCC 4:4:4, 24-bpp, 1 cycle per pixel */
#define VOUT_YCC_MODE_BT601_16BIT           1U      /* BT.601, YCC 4:2:2, 16-bpp, 1 cycle per pixel */
#define VOUT_YCC_MODE_BT601_8BIT            2U      /* BT.601, YCC 4:2:2, 16-bpp, 2 cycle per pixel */
#define VOUT_YCC_MODE_BT656_16BIT           3U      /* BT.656, YCC 4:2:2, 16-bpp, 1 cycle per pixel */
#define VOUT_YCC_MODE_BT656_8BIT            4U      /* BT.656, YCC 4:2:2, 16-bpp, 2 cycle per pixel (480i/576i only) */
#define VOUT_NUM_YCC_MODE                   5U

#define VOUT_YCC_MODE_ORDER_CBYCRY          0U
#define VOUT_YCC_MODE_ORDER_YCBCR           0U
#define VOUT_NUM_YCC_MODE_ORDER             1U

/* Definitions for VOUT Digital RGB modes */
#define VOUT_RGB_MODE_888_SINGLE            0U      /* RGB 8-8-8, 24-bpp, 3 cycle per pixel, 3X horizontal subsampling by picking up single color component from each pixel */
#define VOUT_RGB_MODE_888                   1U      /* RGB 8-8-8, 24-bpp, 3 cycle per pixel */
#define VOUT_RGB_MODE_888_DUMMY             2U      /* RGB 8-8-8, 24-bpp, 4 cycle per pixel (with 1 dummy cycle) */
#define VOUT_RGB_MODE_565                   3U      /* RGB 5-6-5, 16-bpp, 1 cycle per pixel */
#define VOUT_NUM_RGB_MODE                   4U

#define VOUT_RGB_MODE_ORDER_RGB             0U
#define VOUT_RGB_MODE_ORDER_RBG             1U
#define VOUT_RGB_MODE_ORDER_GRB             2U
#define VOUT_RGB_MODE_ORDER_GBR             3U
#define VOUT_RGB_MODE_ORDER_BRG             4U
#define VOUT_RGB_MODE_ORDER_BGR             5U
#define VOUT_NUM_RGB_MODE_ORDER             6U

/* Definitions for VOUT FPD-LINK modes */
#define VOUT_FPD_LINK_MODE_24BIT            0U      /* RGB 8-8-8, 24-bpp, 4 data lanes */
#define VOUT_FPD_LINK_MODE_18BIT            1U      /* RGB 6-6-6, 18-bpp, 3 data lanes */
#define VOUT_NUM_FPD_LINK_MODE              2U

#define VOUT_FPD_LINK_ORDER_LSB             0U      /* JEIDA format */
#define VOUT_FPD_LINK_ORDER_MSB             1U      /* SPWG/PSWG/VESA format */
#define VOUT_NUM_FPD_LINK_ORDER             2U

/* Definitions for VOUT HDMI modes */
#define VOUT_HDMI_MODE_RGB888_24BIT         0U      /* RGB 8-8-8, 24-bpp, 8-bit per color component */
#define VOUT_HDMI_MODE_YCC444_24BIT         1U      /* YCC 4:4:4, 24-bpp, 8-bit per color component */
#define VOUT_HDMI_MODE_YCC422_24BIT         2U      /* YCC 4:2:2, 24-bpp, 12-bit per color component */
#define VOUT_HDMI_MODE_YCC420_24BIT         3U      /* YCC 4:2:0, 24-bpp, 8-bit per color component */
#define VOUT_NUM_HDMI_MODE                  4U

#define VOUT_MIPI_INTERFACE_CSI             0U
#define VOUT_MIPI_INTERFACE_DSI             1U

/* Definitions for VOUT MIPI CSI-2 modes */
#define VOUT_MIPI_CSI_MODE_422_1LANE        0U
#define VOUT_MIPI_CSI_MODE_422_2LANE        1U
#define VOUT_MIPI_CSI_MODE_422_4LANE        3U
#define VOUT_MIPI_CSI_MODE_RAW8_1LANE       4U
#define VOUT_MIPI_CSI_MODE_RAW8_2LANE       5U
#define VOUT_MIPI_CSI_MODE_RAW8_4LANE       7U
#define VOUT_NUM_MIPI_CSI_MODE              8U

#define VOUT_MIPI_CSI_MODE_ORDER_CBYCRY     0U
#define VOUT_NUM_MIPI_CSI_MODE_ORDER        1U

/* Definitions for VOUT MIPI DSI modes */
#define VOUT_MIPI_DSI_MODE_422_1LANE        0U      /* YCC 4:2:2, 16-bpp, 2 pixels packed in 4 bytes, 1 data lane */
#define VOUT_MIPI_DSI_MODE_422_2LANE        1U      /* YCC 4:2:2, 16-bpp, 2 pixels packed in 4 bytes, 2 data lane */
#define VOUT_MIPI_DSI_MODE_422_4LANE        3U      /* YCC 4:2:2, 16-bpp, 2 pixels packed in 4 bytes, 4 data lane */
#define VOUT_MIPI_DSI_MODE_565_1LANE        4U      /* RGB 5-6-5, 16-bpp, 1 pixels packed in 2 bytes, 1 data lane */
#define VOUT_MIPI_DSI_MODE_565_2LANE        5U      /* RGB 5-6-5, 16-bpp, 1 pixels packed in 2 bytes, 2 data lane */
#define VOUT_MIPI_DSI_MODE_565_4LANE        7U      /* RGB 5-6-5, 16-bpp, 1 pixels packed in 2 bytes, 4 data lane */
#define VOUT_MIPI_DSI_MODE_666_1LANE        8U      /* RGB 6-6-6, 18-bpp, 3 pixels packed in 9 bytes, 1 data lane */
#define VOUT_MIPI_DSI_MODE_666_2LANE        9U      /* RGB 6-6-6, 18-bpp, 3 pixels packed in 9 bytes, 2 data lane */
#define VOUT_MIPI_DSI_MODE_666_4LANE        11U     /* RGB 6-6-6, 18-bpp, 3 pixels packed in 9 bytes, 4 data lane */
#define VOUT_MIPI_DSI_MODE_888_1LANE        12U     /* RGB 8-8-8, 24-bpp, 1 pixels packed in 3 bytes, 1 data lane */
#define VOUT_MIPI_DSI_MODE_888_2LANE        13U     /* RGB 8-8-8, 24-bpp, 1 pixels packed in 3 bytes, 2 data lane */
#define VOUT_MIPI_DSI_MODE_888_4LANE        15U     /* RGB 8-8-8, 24-bpp, 1 pixels packed in 3 bytes, 4 data lane */
#define VOUT_NUM_MIPI_DSI_MODE              16U

#define VOUT_MIPI_DSI_MODE_ORDER_CBYCRY     0U
#define VOUT_MIPI_DSI_MODE_ORDER_RGB        0U
#define VOUT_NUM_MIPI_DSI_MODE_ORDER        1U

#define VOUT_DSI_CMD_MAX_PARAM_NUM          16U

/* Definitions for VOUT MIPI PHY Control modes */
#define VOUT_PHY_MIPI_DPHY_POWER_UP         0U
#define VOUT_PHY_MIPI_DPHY_POWER_DOWN       1U
#define VOUT_NUM_PHY_PARAM                  2U

/* Definitions for VOUT MIPI D-PHY Clock modes */
#define VOUT_MIPI_DPHY_NONCONT_CLK          0U      /* Non-continuous clock */
#define VOUT_MIPI_DPHY_CONT_CLK             1U      /* Continuous clock */
#define VOUT_NUM_MIPI_DPHY_CLK_MODE         2U

#define VOUT_CVBS_IRE_PARAM_CLAMP           0U
#define VOUT_CVBS_IRE_PARAM_BLACK           1U
#define VOUT_CVBS_IRE_PARAM_BLANK           2U
#define VOUT_CVBS_IRE_PARAM_SYNC            3U
#define VOUT_CVBS_IRE_PARAM_OUTGAIN         4U
#define VOUT_CVBS_IRE_PARAM_OUTOFFSET       5U
#define VOUT_CVBS_IRE_PARAM_OUTMIN          6U
#define VOUT_CVBS_IRE_PARAM_OUTMAX          7U
#define VOUT_NUM_CVBS_IRE_PARAM             8U

#define CSC_TYPE_IDENTITY           0U
#define CSC_TYPE_BT601              1U
#define CSC_TYPE_BT709              2U
#define NUM_CSC_TYPE                3U

#define CSC_YCC_LIMIT_2_RGB_LIMIT   0U
#define CSC_YCC_LIMIT_2_RGB_FULL    1U
#define CSC_YCC_FULL_2_RGB_LIMIT    2U
#define CSC_YCC_FULL_2_RGB_FULL     3U
#define CSC_RGB_LIMIT_2_YCC_LIMIT   4U
#define CSC_RGB_LIMIT_2_YCC_FULL    5U
#define CSC_RGB_FULL_2_YCC_LIMIT    6U
#define CSC_RGB_FULL_2_YCC_FULL     7U
#define NUM_CSC_OPTION              8U

#define VOUT_MMIO_DISP0     (0U)
#define VOUT_MMIO_DISP1     (1U)
#define VOUT_MMIO_DISP2     (2U)

typedef struct {
    UINT8   HsTrail;
    UINT8   ClkTrail;
    UINT8   HsPrepare;
    UINT8   ClkPrepare;
    UINT8   HsZero;
    UINT8   ClkZero;
    UINT8   HsLpx;
    UINT8   InitTx;
} AMBA_VOUT_MIPI_TIMING_PARAM_s;

typedef struct {
    UINT32 PixelClkFreq;
    UINT32 DisplayMethod;
    UINT16 HsyncFrontPorch;
    UINT16 HsyncPulseWidth;
    UINT16 HsyncBackPorch;
    UINT16 VsyncFrontPorch;
    UINT16 VsyncPulseWidth;
    UINT16 VsyncBackPorch;
    UINT16 ActivePixels;
    UINT16 ActiveLines;
} AMBA_VOUT_DISPLAY_TIMING_CONFIG_s;

typedef struct {
    UINT32 PixelClkFreq;    /* Htotal * Vtotal * FrameRate */
    UINT16 Htotal;
    UINT16 Vtotal;
    UINT16 ActivePixels;
    UINT16 ActiveLines;
    UINT8  Interlace;       /* 1 - Interlace; 0 - Progressive */
} AMBA_VOUT_FRAME_TIMING_CONFIG_s;

typedef struct {
    UINT8 ExtClkSampleEdge;
    UINT8 ExtLineSyncPolarity;
    UINT8 ExtFrameSyncPolarity;
    UINT8 ExtDataValidPolarity;
} AMBA_VOUT_DATA_LATCH_CONFIG_s;

typedef struct {
    UINT32 YccMode;
    UINT32 ColorOrder;
    UINT8 ExtClkSampleEdge;
    UINT8 ExtLineSyncPolarity;
    UINT8 ExtFrameSyncPolarity;
    UINT8 ExtDataValidPolarity;
} AMBA_VOUT_DIGITAL_YCC_CONFIG_s;

typedef struct {
    UINT32 RgbMode;
    UINT32 ColorOrder;
    UINT8 ExtClkSampleEdge;
    UINT8 ExtLineSyncPolarity;
    UINT8 ExtFrameSyncPolarity;
    UINT8 ExtDataValidPolarity;
} AMBA_VOUT_DIGITAL_RGB_CONFIG_s;

typedef struct {
    UINT32 FpdLinkMode;
    UINT32 ColorOrder;
} AMBA_VOUT_FPD_LINK_CONFIG_s;

typedef struct {
    UINT32 HdmiMode;
    UINT16 LineSyncPolarity;
    UINT16 FrameSyncPolarity;
} AMBA_VOUT_HDMI_CONFIG_s;

typedef struct {
    UINT32 MipiCsiMode;
    UINT32 ColorOrder;
} AMBA_VOUT_MIPI_CSI_CONFIG_s;

typedef struct {
    UINT32 MipiDsiMode;
    UINT32 ColorOrder;
} AMBA_VOUT_MIPI_DSI_CONFIG_s;

typedef struct {
    FLOAT Coef[3][3];
    FLOAT Offset[3];
    UINT16 MinVal[3];
    UINT16 MaxVal[3];
} AMBA_VOUT_CSC_MATRIX_s;

typedef struct {
    UINT32 LenHSA;
    UINT32 LenHBP;
    UINT32 LenHFP;
    UINT32 LenBLLP;
} AMBA_VOUT_MIPI_DSI_BLANK_PACKET_CTRL_s;

typedef struct {
    UINT32  Parameter:          8;      /* [7:0] Parameter */
    UINT32  CommandType:        8;      /* [15:8] Command type */
    UINT32  DataType:           6;      /* [21:16] The data type (ID) of the sending command */
    UINT32  Reserved:           10;     /* [31:22] Reserved */
} AMBA_MIPI_DCS_SHORT_COMMAND_HEADER_s;

typedef struct {
    UINT32  WordCount1:         8;      /* [7:0] Word count 1 (wc_1) */
    UINT32  WordCount0:         8;      /* [15:8] Word count 0 (wc_0), pending wc-1 bytes */
    UINT32  DataType:           6;      /* [21:16] The data type (ID) of the sending command */
    UINT32  Reserved:           2;      /* [23:22] Reserved */
    UINT32  CommandType:        8;      /* [31:24] Command type */
} AMBA_MIPI_DCS_LONG_COMMAND_HEADER_s;

#if defined(CONFIG_QNX) || defined(CONFIG_LINUX)
typedef struct {
    UINT32 Reserved0[32];              /* Reserved */
    UINT32 TvEncReg32;                 /* TV Encoder Phase Increment for Fsc generation ROM (Byte 0) Register */
    UINT32 TvEncReg33;                 /* TV Encoder Phase Increment for Fsc generation ROM (Byte 1) Register */
    UINT32 TvEncReg34;                 /* TV Encoder Phase Increment for Fsc generation ROM (Byte 2) Register */
    UINT32 TvEncReg35;                 /* TV Encoder Phase Increment for Fsc generation ROM (Byte 3) Register */
    UINT32 TvEncReg36;                 /* TV Encoder Subcarrier to Horizontal Phase Offset Adjust Register */
    UINT32 TvEncReg37;                 /* TV Encoder Subcarrier to Horizontal Phase Offset Adjust Register */
    UINT32 TvEncReg38;                 /* TV Encoder Subcarrier to Horizontal Phase Offset Adjust Register */
    UINT32 TvEncReg39;                 /* TV Encoder Subcarrier to Horizontal Phase Offset Adjust Register */
    UINT32 TvEncReg40;                 /* TV Encoder Control Register */
    UINT32 Reserved1;                  /* Reserved */
    UINT32 TvEncReg42;                 /* TV Encoder Black Level Adjust Regiser */
    UINT32 TvEncReg43;                 /* TV Encoder Blank Level Adjust Regiser */
    UINT32 TvEncReg44;                 /* TV Encoder Clamp Incoming Black Level To 0 Prior To Gain Adjustment Regiser */
    UINT32 TvEncReg45;                 /* TV Encoder Programmable Sync Level Regiser */
    UINT32 TvEncReg46;                 /* TV Encoder Luma Mode Regiser */
    UINT32 TvEncReg47;                 /* TV Encoder DAC Control Regiser */
    UINT32 Reserved2[2];               /* Reserved */
    UINT32 TvEncReg50;                 /* TV Encoder Negative Burst Amplitude Value for NTSC Regiser */
    UINT32 TvEncReg51;                 /* TV Encoder Positive Burst Amplitude Value for PAL to obtain +/- 135 Degree Phase Regiser */
    UINT32 TvEncReg52;                 /* TV Encoder Chroma Mode Regiser */
    UINT32 Reserved3[3];               /* Reserved */
    UINT32 TvEncReg56;                 /* TV Encoder Sync Mode Regiser */
    UINT32 TvEncReg57;                 /* TV Encoder Vertical Sync Offset (MSB) Regiser */
    UINT32 TvEncReg58;                 /* TV Encoder Vertical Sync Offset (LSB) Regiser */
    UINT32 TvEncReg59;                 /* TV Encoder Horizontal Sync Offset (MSB) Regiser */
    UINT32 TvEncReg60;                 /* TV Encoder Horizontal Sync Offset (LSB) Regiser */
    UINT32 TvEncReg61;                 /* TV Encoder Horizontal Half-Line Pixel Count (MSB) Regiser */
    UINT32 TvEncReg62;                 /* TV Encoder Horizontal Half-Line Pixel Count (LSB) Regiser */
    UINT32 Reserved4[2];               /* Reserved */
    UINT32 TvEncReg65;                 /* TV Encoder Odd Field Closed Captioned Data (MSB) Regiser */
    UINT32 TvEncReg66;                 /* TV Encoder Odd Field Closed Captioned Data (LSB) Regiser */
    UINT32 TvEncReg67;                 /* TV Encoder Even Field Closed Captioned Data (MSB) Regiser */
    UINT32 TvEncReg68;                 /* TV Encoder Even Field Closed Captioned Data (LSB) Regiser */
    UINT32 TvEncReg69;                 /* TV Encoder Closed Caption Control Regiser */
    UINT32 Reserved5[26];              /* Reserved */
    UINT32 TvEncReg96;                 /* TV Encoder SIN/COS ROM Test Control Regiser */
    UINT32 TvEncReg97;                 /* TV Encoder Luma Test Control Regiser */
    UINT32 Reserved6;                  /* Reserved */
    UINT32 TvEncReg99;                 /* TV Encoder Croma Test Control Regiser */
    UINT32 Reserved7[20];              /* Reserved */
    UINT32 TvEncReg120;                /* TV Encoder Horizontal Active Start Register */
    UINT32 TvEncReg121;                /* TV Encoder Horizontal Active End  Register */
    UINT32 Reserved8[6];               /* Reserved */
} AMBA_VOUT_TVENC_CONTROL_s;

typedef struct {
    UINT32 DispCtrl;               /* Display Control Register */
    UINT32 DispFrmSize;            /* Display Frame Size Register */
    UINT32 DispFldHeight;          /* Display Field Height Register */
    UINT32 DispTopActiveStart;     /* Display Active Region Start 0 Register */
    UINT32 DispTopActiveEnd;       /* Display Active Region End 0 Register */
    UINT32 DispBtmActiveStart;     /* Display Active Region Start 1 Register */
    UINT32 DispBtmActiveEnd;       /* Display Active Region End 1 Register */
    UINT32 DispBackgroundColor;    /* Display Background Color Register */
    UINT32 AnalogOutputMode;       /* Analog Output Mode Register */
    UINT32 AnalogHSync;            /* Analog Output HSync Control Register */
    UINT32 AnalogVSyncTopStart;    /* First Line of VSync in Top Field/Progressive Frame */
    UINT32 AnalogVSyncTopEnd;      /* Last Line of VSync in Top Field/Progressive Frame */
    UINT32 AnalogVSyncBtmStart;    /* First Line of VSync in Bottom Field */
    UINT32 AnalogVSyncBtmEnd;      /* Last Line of VSync in Bottom Field */
    UINT32 AnalogCSC0;             /* Color Scaling Setting for Analog Output Registers */
    UINT32 AnalogCSC1;             /* Color Scaling Setting for Analog Output Registers */
    UINT32 AnalogCSC2;             /* Color Scaling Setting for Analog Output Registers */
    UINT32 AnalogCSC3;             /* Color Scaling Setting for Analog Output Registers */
    UINT32 AnalogCSC4;             /* Color Scaling Setting for Analog Output Registers */
    UINT32 AnalogCSC5;             /* Color Scaling Setting for Analog Output Registers */
    UINT32 AnalogCSC6;             /* Post Scaling Setting for Analog Output Registers */
    UINT32 AnalogCSC7;             /* Post Scaling Setting for Analog Output Registers */
    UINT32 AnalogCSC8;             /* Post Scaling Setting for Analog Output Registers */
} AMBA_VOUT_DISPLAY_CVBS_CONFIG_s;
#endif

/*
 * Inline Function Definitions
 */
static inline UINT32 AmbaVout_MipiGetRawMode(UINT32 VoutMode)
{
    return ((VoutMode << 3U) >> 3U);
}

/* MIPI DPHY continuous/non-continuous clock mode */
static inline void AmbaVout_MipiSetDphyContClk(UINT32 *pVoutMode)
{
    if (pVoutMode != NULL) {
        *pVoutMode |= 0x80000000U;
    }
}
static inline void AmbaVout_MipiSetDphyNonContClk(UINT32 *pVoutMode)
{
    if (pVoutMode != NULL) {
        *pVoutMode &= ~0x80000000U;
    }
}
static inline UINT32 AmbaVout_MipiGetDphyClkMode(UINT32 VoutMode)
{
    return ((VoutMode & 0x80000000U) >> 31U);
}

/* MIPI Eotp Mode */
static inline void AmbaVout_MipiEnableEotpMode(UINT32 *pVoutMode)
{
    if (pVoutMode != NULL) {
        *pVoutMode |= 0x40000000U;
    }
}
static inline void AmbaVout_MipiDisableEotpMode(UINT32 *pVoutMode)
{
    if (pVoutMode != NULL) {
        *pVoutMode &= ~0x40000000U;
    }
}
static inline UINT32 AmbaVout_MipiGetEotpMode(UINT32 VoutMode)
{
    return ((VoutMode & 0x40000000U) >> 30U);
}

/* MIPI CSI bypass */
static inline void AmbaVout_MipiCsiSetBypass(UINT32 *pVoutMode)
{
    if (pVoutMode != NULL) {
        *pVoutMode |= 0x20000000U;
    }
}
static inline UINT32 AmbaVout_MipiCsiGetBypass(UINT32 VoutMode)
{
    return ((VoutMode & 0x20000000U) >> 29U);
}

#endif /* AMBA_VOUT_DEF_H */
