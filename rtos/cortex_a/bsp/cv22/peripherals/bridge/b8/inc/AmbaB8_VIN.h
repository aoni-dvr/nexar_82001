/**
 *  @file AmbaB8_VIN.h
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
 *  @details Definitions & Constants for B6 VIN Control APIs
 *
 */

#ifndef AMBA_B8_VIN_H
#define AMBA_B8_VIN_H

#ifndef AMBA_B8_VIN_DEF_H
#include "AmbaB8_VIN_Def.h"
#endif

/* MIPI data type */
/* 0x00 to 0x07 - CSI-2 Synchronization Short Packet Data Types */
#define B8_VIN_MIPI_FRAME_START          0x00U   /* Frame Start Code */
#define B8_VIN_MIPI_FRAME_END            0x01U   /* Frame End Code */
#define B8_VIN_MIPI_LINE_START           0x02U   /* Line Start Code */
#define B8_VIN_MIPI_LINE_END             0x03U   /* Line End Code */
/* 0x08 to 0x0F - CSI-2 Generic Short Packet Data Types */
#define B8_VIN_MIPI_SHORT_PACKET1        0x08U   /* Generic Short Packet Code 1 */
#define B8_VIN_MIPI_SHORT_PACKET2        0x09U   /* Generic Short Packet Code 2 */
#define B8_VIN_MIPI_SHORT_PACKET3        0x0aU   /* Generic Short Packet Code 3 */
#define B8_VIN_MIPI_SHORT_PACKET4        0x0bU   /* Generic Short Packet Code 4 */
#define B8_VIN_MIPI_SHORT_PACKET5        0x0cU   /* Generic Short Packet Code 5 */
#define B8_VIN_MIPI_SHORT_PACKET6        0x0dU   /* Generic Short Packet Code 6 */
#define B8_VIN_MIPI_SHORT_PACKET7        0x0eU   /* Generic Short Packet Code 7 */
#define B8_VIN_MIPI_SHORT_PACKET8        0x0fU   /* Generic Short Packet Code 8 */
/* 0x10 to 0x17 - CSI-2 Generic Long Packet Data Types */
#define B8_VIN_MIPI_LONG_NULL            0x10U   /* Generic 8-bit Long Null */
#define B8_VIN_MIPI_LONG_BLANKING        0x11U   /* Generic 8-bit Long Blanking Data */
#define B8_VIN_MIPI_LONG_EMBEDDED        0x12U   /* Generic 8-bit Long Embedded Information */
/* 0x18 to 0x1F - CSI-2 YUV Data Types */
#define B8_VIN_MIPI_YUV420_8BIT          0x18U   /* YUV420 8-bit */
#define B8_VIN_MIPI_YUV420_10BIT         0x19U   /* YUV420 10-bit */
#define B8_VIN_MIPI_YUV422_8BIT          0x1EU   /* YUV422 8-bit */
#define B8_VIN_MIPI_YUV422_10BIT         0x1FU   /* YUV422 10-bit */
/* 0x20 to 0x27 - CSI-2 RGB Data Types */
#define B8_VIN_MIPI_RGB444               0x20U   /* RGB444 */
#define B8_VIN_MIPI_RGB655               0x21U   /* RGB655 */
#define B8_VIN_MIPI_RGB665               0x22U   /* RGB665 */
#define B8_VIN_MIPI_RGB666               0x23U   /* RGB666 */
#define B8_VIN_MIPI_RGB888               0x24U   /* RGB888 */
/* 0x28 to 0x2F - CSI-2 RAW Data Types */
#define B8_VIN_MIPI_RAW6                 0x28U   /* RAW6 */
#define B8_VIN_MIPI_RAW7                 0x29U   /* RAW7 */
#define B8_VIN_MIPI_RAW8                 0x2aU   /* RAW8 */
#define B8_VIN_MIPI_RAW10                0x2bU   /* RAW10 */
#define B8_VIN_MIPI_RAW12                0x2cU   /* RAW12 */
#define B8_VIN_MIPI_RAW14                0x2dU   /* RAW14 */
/* 0x30 to 0x37 - CSI-2 User Defined Byte-based Data Types */
#define B8_VIN_MIPI_USER_DEFINED1        0x30U   /* User Defined 8-bit Data Type 1 */
#define B8_VIN_MIPI_USER_DEFINED2        0x31U   /* User Defined 8-bit Data Type 2 */
#define B8_VIN_MIPI_USER_DEFINED3        0x32U   /* User Defined 8-bit Data Type 3 */
#define B8_VIN_MIPI_USER_DEFINED4        0x33U   /* User Defined 8-bit Data Type 4 */
#define B8_VIN_MIPI_USER_DEFINED5        0x34U   /* User Defined 8-bit Data Type 5 */
#define B8_VIN_MIPI_USER_DEFINED6        0x35U   /* User Defined 8-bit Data Type 6 */
#define B8_VIN_MIPI_USER_DEFINED7        0x36U   /* User Defined 8-bit Data Type 7 */
#define B8_VIN_MIPI_USER_DEFINED8        0x37U   /* User Defined 8-bit Data Type 8 */

typedef struct {
    UINT32                          NumActivePixels;        /* Horizontal active region width */
    UINT32                          NumActiveLines;         /* Vertical active region height */
    UINT32                          NumTotalPixels;         /* Horizontal total width (line_length_pck) */
    UINT32                          NumTotalLines;          /* Vertical total height (frame_length_lines) */
} B8_VIN_RX_HV_SYNC_s;

typedef struct {
    UINT8                           HsyncPolarity;          /* Indicate Line sync signal polarity */
    UINT8                           VsyncPolarity;          /* Indicate Frame sync signal polarity */
    UINT8                           FieldPolarity;          /* Indicate Field signal polarity */
} B8_VIN_DVP_SYNC_PIN_CONFIG_s;

typedef struct {
    B8_VIN_DVP_SYNC_PIN_CONFIG_s    SyncPinConfig;          /* HSync/VSync/Field signal polarity. */
} B8_VIN_DVP_SYNC_s;

typedef struct {
    UINT8                           SyncInterleaving;       /* 0=none; 1=2-lane interleaving; 2=4-lane interleaving */
    UINT8                           ITU656Type;             /* 1= ITU-656 type, don't care CustomSyncCode structure */
    struct {
        UINT8                       PatternAlign;           /* Sync code mask/patterns are: 0=LSB aligned; 1=MSB aligned */
        UINT16                      SyncCodeMask;           /* Sync Code Mask */

        struct {
            UINT32                  Sol:    1;              /* 1 = Enable detection of SOL sync codes */
            UINT32                  Eol:    1;              /* 1 = Enable detection of EOL sync codes */
            UINT32                  Sof:    1;              /* 1 = Enable detection of SOF sync codes */
            UINT32                  Eof:    1;              /* 1 = Enable detection of EOF sync codes */
            UINT32                  Sov:    1;              /* 1 = Enable detection of SOV sync codes */
            UINT32                  Eov:    1;              /* 1 = Enable detection of EOV sync codes */
        } DetectEnable;

        UINT16                      PatternSol;                 /* Start of active line */
        UINT16                      PatternEol;             /* End of active line */
        UINT16                      PatternSof;             /* Start of frame */
        UINT16                      PatternEof;             /* End of frame */
        UINT16                      PatternSov;             /* Start of vertical blanking line */
        UINT16                      PatternEov;             /* End of vertical blanking line */
    } CustomSyncCode;

} B8_VIN_SLVS_SYNC_CODE_s;

typedef struct {
    UINT8                       NumSplits;              /* number of splits to make */
    UINT16                      SplitWidth;             /* Split each input line to lines of SPLIT_WIDTH pixels(including intermediate HBLANK) */
} B8_VIN_SPILT_CTRL_s;

typedef struct {
    UINT8                           HsSettleTime;           /* D-PHY HS-SETTLE time */
    UINT8                           HsTermTime;             /* D-PHY HS-TERM time */
    UINT8                           ClkSettleTime;          /* D-PHY CLK-SETTLE time */
    UINT8                           ClkTermTime;            /* D-PHY CLK-TERM time */
    UINT8                           ClkMissTime;            /* D-PHY CLK-MISS time */
    UINT8                           RxInitTime;             /* D-PHY RX-INIT time */
} B8_VIN_MIPI_RX_CONFIG_s;

typedef struct {
    UINT8                           NumDataBits;            /* Bit depth of pixel data */
    UINT8                           NumDataLane;            /* Number of active data lanes */
    UINT8                           DataLaneSelect[10];     /* Logical to Physical Lane Mapping */
    B8_VIN_SLVS_SYNC_CODE_s         SyncDetectCtrl;         /* Sync code detection control */
    B8_VIN_RX_HV_SYNC_s             RxHvSyncCtrl;           /* Input H/V sync signal format (from sensor) */
    B8_VIN_SPILT_CTRL_s             SplitCtrl;              /* Vin split function control */
} B8_VIN_SLVS_CONFIG_s;

typedef struct {
    UINT32                          DataRate;               /* MIPI data rate per lane */
    UINT8                           NumDataBits;            /* Bit depth of pixel data */
    UINT8                           NumActiveLanes;         /* Number of active data lanes */
    UINT8                           DataType;               /* Data type of MIPI packet, defined as B8_VIN_MIPI_xxx */
    UINT8                           DataTypeMask;           /* Data type mask of MIPI packet */
    B8_VIN_RX_HV_SYNC_s             RxHvSyncCtrl;           /* Input H/V sync signal format (from sensor) */
    B8_VIN_MIPI_RX_CONFIG_s         MipiCtrl;               /* MIPI M-PHY configuration */
} B8_VIN_MIPI_CONFIG_s;

typedef struct {
    UINT8                           DvpWide;                /* 0=1-Pixel wide input; 1=2-Pixel wide input */
    UINT8                           DataLatchEdge;          /* 0U: Latch data on rising edge of clock(SDR),
                                                               1U: Latch data on falling edge of clock(SDR),
                                                               2U: Latch data on both rising and falling edge of clock(DDR) */
    UINT8                           ColorSpace;             /* Color space of the input data. 0U: RGB, 1U: YUVs */
    UINT8                           YuvOrder;               /* Pixel order */
    B8_VIN_DVP_SYNC_s               SyncDetectCtrl;         /* H/V sync detection control */
    B8_VIN_RX_HV_SYNC_s             RxHvSyncCtrl;           /* Input H/V sync signal format (from sensor) */
} B8_VIN_DVP_CONFIG_s;

typedef struct {
    UINT32  Period;                                      /* For Hsync: unit=cycle, for Vsync: unit=Hsync */
    UINT32  PulseWidth;                                  /* Unit=cycle */
    UINT8   Polarity;                                    /* 0: Active Low, 1: Active High */
} B8_VIN_SYNC_WAVEFORM_s;

typedef struct {
    B8_VIN_SYNC_WAVEFORM_s      HSync;               /* Waveform of Hsync output */
    B8_VIN_SYNC_WAVEFORM_s      VSync;               /* Waveform of Vsync output */
    UINT16                      HSyncDelayCycles;    /* Offset of Hsync pulse in cycles */
    UINT16                      VSyncDelayCycles;    /* Offset between Hsync and Vsync edges */
    UINT8                       ToggleHsyncInVblank; /* 1: toggle Hsync during Vblank */
} B8_VIN_MASTER_SYNC_CONFIG_s;


/* VIN Channel */
#define B8_MSYNC_CHANNEL0                       0U  /* B8 VIN Channel 0 */
#define B8_MSYNC_CHANNEL1                       1U  /* B8 VIN Channel 1 */
#define B8_NUM_MSYNC_CHANNEL                    2U  /* Number of B6 VIN Channel */

/* MSyncChan */
#define B8_VIN_CHANNEL0                         0U  /* B8 VIN Channel 0 */
#define B8_VIN_CHANNEL1                         1U  /* B8 VIN Channel 1 */
#define B8_NUM_VIN_CHANNEL                      2U  /* Number of B6 VIN Channel */

/* DelayedMSyncChan */
#define B8_DELAYED_MASTER_SYNC_CHANNEL0         0U  /* pin name: VSYNC0, HSYNC0 */
#define B8_DELAYED_MASTER_SYNC_CHANNEL1         1U  /* pin name: VSYNC1, HSYNC1 */
#define B8_NUM_DELAYED_MASTER_SYNC_CHANNEL      2U

/* delayed master sync source */
#define B8_DELAYED_MASTER_SYNC_SOURCE_VIN       0U  /* select vin_master_sync as source */
#define B8_DELAYED_MASTER_SYNC_SOURCE_PIP       1U  /* select pip_master_sync as source*/
#define B8_DELAYED_MASTER_SYNC_SOURCE_EXTERNAL  2U  /* select external sync as source*/

typedef struct {
    UINT8       VsyncSource;
    UINT8       HsyncSource;
    /* Vsync delay(unit: hsync) = (DelayTimeInHsync * DelayTimeMultiplier) */
    UINT16      DelayTimeInHsync;       /* Max. value: 1023 */
    UINT8       DelayTimeMultiplier;    /* Max. value: 7 */
} B8_VIN_DELAYED_MASTER_SYNC_CONFIG_s;

/*---------------------------------------------------------------------------*\
 * Defined in AmbaB8_VIN.c
\*---------------------------------------------------------------------------*/
UINT32 AmbaB8_VinReset(UINT32 ChipID, UINT32 Channel, UINT32 VinInterface, const B8_VIN_MIPI_DPHY_CTRL_s *pVinMipiDphyCtrl);
UINT32 AmbaB8_VinConfigSLVS(UINT32 ChipID, UINT32 Channel, const B8_VIN_SLVS_CONFIG_s *pVinSlvsConfig);
UINT32 AmbaB8_VinConfigMIPI(UINT32 ChipID, UINT32 Channel, const B8_VIN_MIPI_CONFIG_s *pVinMipiConfig);
UINT32 AmbaB8_VinConfigDVP(UINT32 ChipID, UINT32 Channel, const B8_VIN_DVP_CONFIG_s *pVinDvpConfig);
UINT32 AmbaB8_VinCaptureConfig(UINT32 ChipID, UINT32 Channel, const B8_VIN_WINDOW_s *pCaptureWindow);
UINT32 AmbaB8_VinConfigMasterSync(UINT32 ChipID, UINT32 MSyncChan, const B8_VIN_MASTER_SYNC_CONFIG_s *pMasterSyncConfig);
UINT32 AmbaB8_VinConfigDelayedMSync(UINT32 ChipID, UINT32 DelayedMSyncChan, const B8_VIN_DELAYED_MASTER_SYNC_CONFIG_s *pSyncConfig);
void AmbaB8_VinDPhyReset(UINT32 ChipID, UINT8 Reset);
UINT32 AmbaB8_VinWaitMipiSignalLock(UINT32 ChipID, UINT32 Channel, UINT32 TimeOut);

#endif  /* AMBA_B8_VIN_H */
