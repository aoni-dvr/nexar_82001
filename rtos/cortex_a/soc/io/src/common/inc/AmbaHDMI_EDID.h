/*
 * Copyright 2020, Ambarella International LP
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

#ifndef AMBA_HDMI_EDID_H
#define AMBA_HDMI_EDID_H

#ifndef AMBA_TYPES_H
#include "AmbaTypes.h"
#endif

#define IEEE_OUI_HDMI_LICENSING_LLC         (0x000C03U)

/*
 * VESA Enhanced Extended Display Identification Data (E-EDID) Standard
 */
#define EDID_NUM_BLOCKS                     (256U)  /* Maximum number of EDID blocks (E-DDC) */
#define EDID_BLOCK_SIZE                     (128U)
#define EDID_SEGMENT_SIZE                   (2U * EDID_BLOCK_SIZE)

/*
 * CEA 861 Extension Block
 */
#define HDMI_VSDB_HDMI_VIDEO_PRESENT        0x20U
#define HDMI_VSDB_I_LATENCY_FIELDS_PRESENT  0x40U
#define HDMI_VSDB_LATENCY_FIELDS_PRESENT    0x80U

#define HDMI_VSDB_DC_Y444_PRESENT           0x08U
#define HDMI_VSDB_DC_30BIT_PRESENT          0x10U
#define HDMI_VSDB_DC_36BIT_PRESENT          0x20U
#define HDMI_VSDB_DC_48BIT_PRESENT          0x40U

#define EDID_CEA861_TAG_AUDIO               1U      /* Tag Code of Audio Data Block */
#define EDID_CEA861_TAG_VIDEO               2U      /* Tag Code of Video Data Block */
#define EDID_CEA861_TAG_VENDOR              3U      /* Tag Code of Vendor Specific Data Block (VSDB) */
#define EDID_CEA861_TAG_SPEAKER             4U      /* Tag Code of Speaker Allocation Data Block */
#define EDID_CEA861_TAG_VESA_DTC            5U      /* Tag Code of VESA DTC Data Block */
#define EDID_CEA861_TAG_EXTENDED            7U      /* Use Extended Tag */

#define EDID_CEA861_YUV_422_SUPPORTED       0x10U   /* Set if sink supports YCbCr 4:2:2 in addition to RGB */
#define EDID_CEA861_YUV_444_SUPPORTED       0x20U   /* Set if sink supports YCbCr 4:4:4 in addition to RGB */
#define EDID_CEA861_BASIC_AUDIO_SUPPORTED   0x40U   /* Set if sink supports basic audio (2 channel LPCM, fs 32/44.1/48 kHz) */
#define EDID_CEA861_UNDERSCAN_IT_BY_DEFAULT 0x80U   /* Set if sink underscans IT video formats by default */

typedef struct {
    UINT8 Tag;                              /* 00h: Tag (0x02) */
    UINT8 RevisionNumber;                   /* 01h: Revision Number (0x03) */
    UINT8 OffsetDTD;                        /* 02h: Byte offset of Detailed Timing Descriptors */
    UINT8 NumOfDTD;                         /* 03h: The number of Detailed Timing Descriptors with some indication flags */
    UINT8 Data[123];                        /* 04h-7Eh: Data Block Collection & DTDs & Padding */
    UINT8 Checksum;                         /* 7Fh: Checksum */
} EDID_CEA861_EXT_V3_FORMAT_s;

/*
 * Base EDID Version 1 Revision 3 Structure
 */
#define EDID_EXT_CEA                        0x02U   /* CEA 861 Series Extension */
#define EDID_EXT_VTB                        0x10U   /* Video Timing Block Extension */
#define EDID_EXT_DI                         0x40U   /* Display Information Extension */
#define EDID_EXT_LS                         0x50U   /* Localized String Extension */
#define EDID_EXT_DPVL                       0x60U   /* Digital Packet Video Link Extension */
#define EDID_EXT_BLOCK_MAP                  0xF0U   /* Extension Block Map */
#define EDID_EXT_MANUFACTURER               0xFFU   /* Extensions defined by the display manufacturer */

typedef struct {
    UINT8 HeaderInfo[8];                    /* 00h-07h: Header = (00 FF FF FF FF FF FF 00)h */
    UINT8 IdManufacturerName[2];            /* 08h-09h: ISA 3-character ID code */
    UINT8 IdProductCode[2];                 /* 0Ah-0Bh: Vendor assigned code */
    UINT8 IdSerialNumber[4];                /* 0Ch-0Fh: 32-bit Serial Number */
    UINT8 WeekOfManufacture;                /* 10h: Week number or Model Year Flag */
    UINT8 YearOfManufacture;                /* 11h: Manufacture Year of Model Year */
    UINT8 VersionNumber;                    /* 12h: EDID structure version number */
    UINT8 RevisionNumber;                   /* 13h: EDID structure revision number */
    UINT8 VideoInputDefinition;             /* 14h: Information for host to configure its video output */
    UINT8 HorizontalScreenSize;             /* 15h: Listed in cm. */
    UINT8 VerticalScreenSize;               /* 16h: Listed in cm. */
    UINT8 Gamma;                            /* 17h: Factory Default Value (FFh if stored in extension block) */
    UINT8 FeatureSupport;                   /* 18h: The field used to indicate support for various display features */
    UINT8 LowOrderBitsRedGreen;             /* 19h: Red-x   [1:0], Red-y   [1:0], Green-x [1:0], Green-y [1:0] */
    UINT8 LowOrderBitsBlueWhite;            /* 1Ah: Blue-x  [1:0], Blue-y  [1:0], White-x [1:0], White-y [1:0] */
    UINT8 HighOrderBitsRedX;                /* 1Bh: Red-x   [9:2] */
    UINT8 HighOrderBitsRedY;                /* 1Ch: Red-y   [9:2] */
    UINT8 HighOrderBitsGreenX;              /* 1Dh: Green-x [9:2] */
    UINT8 HighOrderBitsGreenY;              /* 1Eh: Green-y [9:2] */
    UINT8 HighOrderBitsBlueX;               /* 1Fh: Blue-x  [9:2] */
    UINT8 HighOrderBitsBlueY;               /* 20h: Blue-y  [9:2] */
    UINT8 HighOrderBitsWhiteX;              /* 21h: White-x [9:2] */
    UINT8 HighOrderBitsWhiteY;              /* 22h: White-y [9:2] */
    UINT8 EstablishedTimings[3];            /* 23h-25h: Optional indication of Factory Supported Video Timing Modes */
    UINT16 StandardTimings[8];              /* 26h-35h: Optional timing identifier of Factory Supported Video Timing Modes */
    UINT8 Descriptor[4][18];                /* 36h-7Dh: 18 Byte Detailed timing descriptor/Display descriptor/Dummy descriptor */
    UINT8 ExtensionBlockCount;              /* 7Eh: Number of 128-byte EDID Extension blocks to follow */
    UINT8 Checksum;                         /* 7Fh: The 1-byte makes the sum of all 128 bytes in this EDID block equal zero */
} EDID_FORMAT_s;

#endif /* AMBA_HDMI_EDID_H */
