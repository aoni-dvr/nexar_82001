/**
 *  @file AmbaB8.h
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
 *  @details Definitions & Constants for B6 APIs
 *
 */

#ifndef AMBA_B8_H
#define AMBA_B8_H

#include "AmbaB8_Wrapper.h"
#include "AmbaB8_VIN.h"
#include "AmbaB8_VOUT.h"

#ifndef NULL
#define NULL (void*)0
#endif

//#define VLSI_DEBUG
//#define PHY_SHMOO
//#define BUILT_IN_SERDES
//#define B8_DEV_VERSION
#define B8_DBG_TEMP                         0U  /* remove after */

#if defined (VLSI_DEBUG) || defined (DFE_SEARCH) || defined (PHY_SHMOO)
#include "AmbaPrint.h"
#endif

#define B8_EXTRA_DELAY                      0U

/* ERROR CODE */
#define B8_ERR_NONE                         0U
#define B8_ERR_INVALID_API                  1U
#define B8_ERR_ARG                          2U
#define B8_ERR_SERDES_LINK                  3U
#define B8_ERR_COMMUNICATE                  4U   /* SPI to communicate with B6 fail*/
#define B8_ERR_MUTEX                        5U
#define B8_ERR_UNKNOWN_CHIP_VERSION         6U   /* unknown B6 chip version */
#define B8_ERR_TIMEOUT                      7U
#define B8_ERR_LOCK                         8U
#define B8_ERR_UNEXPECTED                   0xFFU

#define B8_CHANNEL0                         0U
#define B8_CHANNEL1                         1U
#define B8_CHANNEL2                         2U
#define B8_CHANNEL3                         3U
#define B8_CHANNEL4                         4U
#define B8_CHANNEL5                         5U
#define B8_NUM_CHANNEL                      6U

#define B8_MAX_NUM_B8N_ON_CHAN              1U
#define B8_MAX_NUM_B8F_ON_CHAN              2U
#define B8_MAX_NUM_B8D_ON_CHAN              1U
#define B8_MAX_NUM_B8_ON_CHAN               4U /* N, F0, F1, D0 */
#define B8_MAX_NUM_SUBCHIP                  4U

/*---------------------------------------------------------------------------*\
 * PIN Configuration
\*---------------------------------------------------------------------------*/
#define B8_SERDES_RATE_2P3G                 0U
#define B8_SERDES_RATE_2P59G                1U
#define B8_SERDES_RATE_3P02G                2U
#define B8_SERDES_RATE_3P45G                3U
#define B8_SERDES_RATE_3P60G                4U
#define B8_SERDES_RATE_3P74G                5U
#define B8_SERDES_RATE_3P88G                6U
#define B8_SERDES_RATE_4P03G                7U
#define B8_SERDES_RATE_4P17G                8U
#define B8_SERDES_RATE_4P32G                9U
#define B8_SERDES_RATE_4P46G                10U
#define B8_SERDES_RATE_5P04G                11U
#define B8_SERDES_RATE_5P47G                12U
#define B8_SERDES_RATE_5P76G                13U
#define B8_SERDES_RATE_6P04G                14U
#define B8_NUM_SERDES_RATE                  15U

/* SerDesPinCtrl */
#define B8_PIN_SERDES_CTRL_SINGLE           0U
#define B8_PIN_SERDES_CTRL_DOUBLE           1U

/* SensorPinCtrl */
#define B8_PIN_SENSOR_CTRL_SPI0             0U
#define B8_PIN_SENSOR_CTRL_SPI1             1U
#define B8_PIN_SENSOR_CTRL_I2C0             2U
#define B8_PIN_SENSOR_CTRL_I2C0_2           3U

/*-----------------------------------------------------------------------------------*\
 *   B8 ChipID(32bits)
 *  |   8 bits  |           20 bits                  |   4bits    |
 *  |------------------------------------------------|------------|
 *  |
    | select ID |           main ID                  |   sub ID   |
 *  |________________________________________________|____________|
 *
 *  e.g. ChipID for B8F0 on VIN = (B8_CHIP_ID_B8_ON_VIN | B8_SUB_ID_B8F0)
 *
\*-----------------------------------------------------------------------------------*/
#define B8_MAIN_CHIP_ID_B8_ON_VIN           (0x00000010U)
#define B8_MAIN_CHIP_ID_B8_ON_PIP           (0x00000020U)
#define B8_MAIN_CHIP_ID_B8_ON_PIP2          (0x00000040U)
#define B8_MAIN_CHIP_ID_B8_ON_PIP3          (0x00000080U)
#define B8_MAIN_CHIP_ID_B8_ON_PIP4          (0x00000100U)
#define B8_MAIN_CHIP_ID_B8_ON_PIP5          (0x00000200U)
#define B8_MAIN_CHIP_ID_B8_ON_VOUTA         (0x00001000U)
#define B8_MAIN_CHIP_ID_B8_ON_VOUTB         (0x00002000U)
#define B8_MAIN_CHIP_ID_B8_ON_VOUTC         (0x00004000U)
#define B8_MAIN_CHIP_ID_B8_ON_VOUTD         (0x00008000U)
#define B8_MAIN_CHIP_ID_B8NF_MASK           (0x00000ff0U)   /* B8N for video input */
#define B8_MAIN_CHIP_ID_B8ND_MASK           (0x0000f000U)   /* B8N for video output */
#define B8_MAIN_CHIP_ID_B8N_MASK            (0x000ffff0U)   /* B8N for video input/output */
#define B8_MAIN_CHIP_ID_BUILT_IN_B8NF       (0x00100000U)   /* built-in serdes */
#define B8_MAIN_CHIP_ID_BUILT_IN_B8ND       (0x00200000U)   /* built-in serdes */
#define B8_MAIN_CHIP_ID_BUILT_IN_MASK       (0x00f00000U)   /* built-in serdes for video input/output */
#define B8_MAIN_CHIP_ID_MASK                (0x00fffff0U)

#define B8_SELECT_CHIP_ID_B6F               (0x01000000U)   /* B6 as a far-side chip */
#define B8_SELECT_CHIP_ID_B6N               (0x10000000U)   /* B6 as a near-side chip */
#define B8_SELECT_CHIP_ID_F_MASK            (0x0f000000U)   /* chip select for far-side chip */
#define B8_SELECT_CHIP_ID_N_MASK            (0xf0000000U)   /* chip select for near-side chip */

#define B8_SUB_CHIP_ID_B8N                  (0x0U)
#define B8_SUB_CHIP_ID_B8F0                 (0x1U)
#define B8_SUB_CHIP_ID_B8F1                 (0x2U)
#define B8_SUB_CHIP_ID_B8F2                 (0x4U)
#define B8_SUB_CHIP_ID_B8F3                 (0x8U)
#define B8_SUB_CHIP_ID_MASK                 (0x0000000fU)

/* SensorID definition for sensors on B6N/B6F */
#define B8_SENSOR_ID_B8N_VIN                (0x00000001U)   /* sensor connects to B8N VIN */
#define B8_SENSOR_ID_B8N_PIP                (0x00000002U)   /* sensor connects to B8N VIN */

#define B8_SENSOR_ID_B8F0_VIN               (0x00000010U)   /* sensor connects to B8F(on B8N DES_P0 port) */
#define B8_SENSOR_ID_B8F0_PIP               (0x00000020U)   /* sensor connects to B8F(on B8N DES_P0 port) */

#define B8_SENSOR_ID_B8F1_VIN               (0x00000100U)   /* sensor connects to B8F(on B8N DES_P1 port) */
#define B8_SENSOR_ID_B8F1_PIP               (0x00000200U)   /* sensor connects to B8F(on B8N DES_P1 port) */

/*---------------------------------------------------------------------------*\
 * Video Configuration
\*---------------------------------------------------------------------------*/
#define B8_COMPRESS_NONE                    0U
#define B8_COMPRESS_4P75                    1U      /* 4.75 bits per pixel */
#define B8_COMPRESS_5P5                     2U      /* 5.5 bits per pixel */
#define B8_COMPRESS_5P75                    3U      /* 5.75 bits per pixel */
#define B8_COMPRESS_6P5                     4U      /* 6.5 bits per pixel */
#define B8_COMPRESS_6P75                    5U      /* 6.75 bits per pixel */
#define B8_COMPRESS_7P5                     6U      /* 7.5 bits per pixel */
#define B8_COMPRESS_7P75                    7U      /* 7.75 bits per pixel */
#define B8_COMPRESS_8P5                     8U      /* 8.5 bits per pixel */
#define B8_COMPRESS_8P75                    9U      /* 8.75 bits per pixel */
#define B8_COMPRESS_9P5                     10U     /* 9.5 bits per pixel */
#define B8_COMPRESS_9P75                    11U     /* 9.75 bits per pixel */
#define B8_COMPRESS_10P5                    12U     /* 10.5 bits per pixel */
#define B8_NUM_LEGACY_COMPRESS_RATIO        13U

#define B8_USE_CFA_CODEC                    0x100U  /* B6A CFA Codec */

/* VoutMode */
#define B8_VOUT_MODE_NONE                   0U
#define B8_VOUT_MODE_SLVS                   1U      /* SLVS */
#define B8_VOUT_MODE_CSI                    2U      /* MIPI CSI */
#define B8_VOUT_MODE_DSI                    3U      /* MIPI DSI */
#define B8_VOUT_MODE_FPD                    4U      /* FPD */

#define B8_VOUT_MIPI_2LANE                  2U      /* MIPI CSI-2 2 lanes */
#define B8_VOUT_MIPI_4LANE                  4U      /* MIPI CSI-2 4 lanes */

#define B8_VOUT_SLVS_1LANE                  1U      /* SLVS 1 lanes */
#define B8_VOUT_SLVS_2LANE                  2U      /* SLVS 2 lanes */
#define B8_VOUT_SLVS_4LANE                  4U      /* SLVS 4 lanes */
#define B8_VOUT_SLVS_8LANE                  8U      /* SLVS 8 lanes */
#define B8_VOUT_SLVS_10LANE                 10U     /* SLVS 10 lanes */

typedef struct {
    UINT32                  SerDesRate;             /* Configure SERDES Rate(bps), defined as B8_SERDES_RATE_xxx */
    UINT32                  SerDesPinCtrl;          /* For accessing B8F registers, 0U: single, 1U: differential defined as B8_PIN_SERDES_CTRL_xxx */
    UINT32                  SensorPinCtrl;          /* For accessing sensor registers, defined as B8_PIN_SENSOR_CTRL_xxx */
    UINT32                  VinInterface;           /* For VIN DPHY control */
    B8_VIN_MIPI_DPHY_CTRL_s *pVinMipiDphyCtrl;      /* For VIN MIPI DPHY control */
    UINT32                  SensorID;               /* defined as B8_SENSOR_ID_xxx */
} B8_PIN_CONFIG_s;

typedef struct {
    UINT32                  DataRate;               /* Sensor output data rate per lane */
    UINT16                  NumDataLanes;           /* Sensor active data channels */
    UINT16                  NumDataBits;            /* Sensor pixel data bit depth */
    UINT16                  LineLengthPck;          /* Number of pixel clock cycles per line */
    UINT16                  FrameLengthLines;       /* Number of lines per frame */
    UINT16                  OutputWidth;            /* Sensor output width */
    UINT16                  OutputHeight;           /* Sensor output height */
    UINT8                   NumPicturesPerFrame;    /* Number of pictures from a sensor with different exposures */
    UINT32                  BayerPattern;           /* 0 - RG, 1 -BG, 2 - GR, 3 - GB (should be same as AMBA_DSP_SENSOR_PATTERN_e) */
    FLOAT                   RowTime;                /* time interval of LineLengthPck (in seconds) */
} B8_SENSOR_INFO_s;

typedef struct {
    UINT16                  Enable;                 /* 0: Disable, Others: Enable */
    UINT32                  DitherRandomSeedX;
    UINT32                  DitherRandomSeedY;
} B8_DITHER_CTRL_s;

typedef struct {
    UINT32                  Ratio;                  /* decompress ratio, should match B6 configuration, defined as B8_COMPRESS_xxx */
    B8_DITHER_CTRL_s        DitherCtrl;             /* Dither control, should match B6 configuration */
    UINT16                  Offset;                 /* pre-subtract black level value, should match B6 configuration */
} B8_SERDES_COMPRESS_s;

typedef struct {
    UINT32                  SensorID;               /* defined as B8_SENSOR_ID_xxx */
    B8_SENSOR_INFO_s        SensorInfo;
    UINT32                  VinInterface;           /* Vin interface, 0U: LVDS, 1: LVCMOS, 2: MIPI, defined as B8_VIN_xxx */
    B8_VIN_WINDOW_s         VinCropWindow;          /* Vin capture window info */
    void*                   pVinConfig;             /* pointer to B8 Vin configuration */
    UINT32                  PrescalerWidth;         /* Sensor width after pre-scaler */
} B8_INPUT_INFO_s;

typedef struct {
    UINT32                  OutputMode;             /* 0U: unused, 1U: SLVS, 2U: CSI, 3U: DSI, 4U: FPD, defined as B8_VOUT_MODE_xxx */
    UINT32                  DataRate;               /* output data rate per lane */
    UINT16                  NumDataLanes;           /* active data channels */
    UINT16                  NumDataBits;            /* pixel data bit depth */
    UINT8                   ViewSwap;               /* only available for dual video inputs case.
                                                       0U(default): left= VIN0(Ch0), right= VIN1(ch1)
                                                       1U: left= VIN1(Ch1), right= VIN0(ch0)*/
} B8_OUTPUT_INFO_s;

typedef struct {
    UINT32                  SerDesRate;             /* SERDES Rate(bps), defined as B8_SERDES_RATE_xxx */
    B8_SERDES_COMPRESS_s    CompressCtrl;           /* Compression setting */
} B8_SERDES_INFO_s;

typedef struct {
    B8_SERDES_INFO_s        SerdesInfo;
    B8_INPUT_INFO_s         InputInfo;
    B8_OUTPUT_INFO_s        OutputInfo[AMBA_NUM_B8_VOUT_CHANNEL];
} B8_CONFIG_s;

/*---------------------------------------------------------------------------*\
 * Defined in AmbaB8.c
\*---------------------------------------------------------------------------*/
UINT32 AmbaB8_Init(UINT32 ChipID, const B8_PIN_CONFIG_s *pPinConfig);
UINT32 AmbaB8_Config(UINT32 ChipID, const B8_CONFIG_s *pB8Config);

UINT32 AmbaB8_GetLinkStatus(UINT32 ChipID);
UINT32 AmbaB8_GetCtrlIndex(UINT32 ChipID);
UINT32 AmbaB8_GetSubChipCount(UINT32 ChipID);

#endif /* AMBA_B8_H */
