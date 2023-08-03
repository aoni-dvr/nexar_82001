/**
 *  @file AmbaHDMI_Info.c
 *
 *  @copyright Copyright (c) 2019 Ambarella, Inc.
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
 *  @details EDID and InfoFrame Data Control APIs
 *
 */

#if defined(CONFIG_QNX)
#include "hw/ambarella_clk.h"
#endif

#include "AmbaTypes.h"
#include "AmbaMisraFix.h"
#include "AmbaWrap.h"

#include "AmbaI2S.h"
#include "AmbaHDMI.h"
#include "AmbaHDMI_Ctrl.h"
#include "AmbaVOUT.h"
#include "AmbaRTSL_PLL.h"
#include "AmbaRTSL_HDMI.h"
#include "AmbaRTSL_VOUT.h"

static UINT32 I2sChanNo = AMBA_I2S_CHANNEL0; /* Default to use I2S channel 0 as HDMI audio src */
static AMBA_HDMI_SINK_INFO_s AmbaHdmiSinkInfo GNU_SECTION_NOZEROINIT;


/**
 *  AmbaHDMI_TxCopySinkInfo - Duplicate sink info data structure
 *  @param[out] pSinkInfo HDMI sink info
 *  @return error code
 */
UINT32 AmbaHDMI_TxCopySinkInfo(AMBA_HDMI_SINK_INFO_s *pSinkInfo)
{
    const AMBA_HDMI_SINK_INFO_s *pSrcSinkInfo = &AmbaHdmiSinkInfo;
    const UINT32 *pSrc;
    UINT32 *pDst, Size;
    UINT32 RetVal = HDMI_ERR_NONE;

    if (pSinkInfo == NULL) {
        RetVal = HDMI_ERR_ARG;
    } else {
        pSinkInfo->SrcPhysicalAddr = pSrcSinkInfo->SrcPhysicalAddr;
        AmbaMisra_TypeCast(&pDst, &pSinkInfo);
        AmbaMisra_TypeCast(&pSrc, &pSrcSinkInfo);
        Size = (UINT32)sizeof(AMBA_HDMI_SINK_INFO_s) & ~0x3U;
        while (Size != 0U) {
            *pDst = *pSrc;
            pDst++;
            pSrc++;
            Size -= (UINT32)sizeof(UINT32);
        }
    }

    return RetVal;
}

/**
 *  HDMI_ParseHdmi14bVSDB - Parse HDMI 1.4b Vendor-Specific Data Block
 *  @param[in] pHdmiVSDB pointer to HDMI VSDB within CEA-861 Extension block
 *  @return error code
 */
static UINT32 HDMI_ParseHdmi14bVSDB(const UINT8 *pHdmiVSDB)
{
    AMBA_HDMI_VIDEO_DESCRIPTOR_s VicDesc;
    UINT32 Offset, LengthVIC;
    UINT32 i, RetVal = HDMI_ERR_NONE;

    AmbaHdmiSinkInfo.SrcPhysicalAddr = (((UINT32)pHdmiVSDB[4]) << 8U) | ((UINT32)pHdmiVSDB[5]);

    if ((pHdmiVSDB[8] & HDMI_VSDB_HDMI_VIDEO_PRESENT) != 0x00U) {
        RetVal = HDMI_ERR_NONE;
    } else {
        if ((pHdmiVSDB[8] & HDMI_VSDB_LATENCY_FIELDS_PRESENT) != 0x00U) {
            if ((pHdmiVSDB[8] & HDMI_VSDB_I_LATENCY_FIELDS_PRESENT) != 0x00U) {
                Offset = 4U;
            } else {
                Offset = 2U;
            }
        } else {
            Offset = 0U;
        }

        LengthVIC = (UINT32)pHdmiVSDB[10U + Offset] >> 5U;    /* HDMI_VIC_LEN */
        Offset += 11U;

        /* Supported deep color formats */
        if ((pHdmiVSDB[6] & HDMI_VSDB_DC_30BIT_PRESENT) != 0x00U) {
            AmbaHdmiSinkInfo.DeepColorCapability |= 0x01U;
        }
        if ((pHdmiVSDB[6] & HDMI_VSDB_DC_36BIT_PRESENT) != 0x00U) {
            AmbaHdmiSinkInfo.DeepColorCapability |= 0x02U;
        }
        if ((pHdmiVSDB[6] & HDMI_VSDB_DC_48BIT_PRESENT) != 0x00U) {
            AmbaHdmiSinkInfo.DeepColorCapability |= 0x04U;
        }

        /* If DC_Y444 is set, then YCbCr 4:4:4 is supported for all modes indicated by the DC_XXbit flags.*/
        if ((pHdmiVSDB[6] & HDMI_VSDB_DC_Y444_PRESENT) != 0x00U) {
            AmbaHdmiSinkInfo.DeepColorCapability = (AmbaHdmiSinkInfo.DeepColorCapability << 4U) | AmbaHdmiSinkInfo.DeepColorCapability;
        }

        /* Supported 4K x 2K video timings */
        AmbaHdmiSinkInfo.VideoInfoBuf[AmbaHdmiSinkInfo.NumVideoInfo] = 0U;
        if (AmbaWrap_memcpy(&VicDesc, &AmbaHdmiSinkInfo.VideoInfoBuf[AmbaHdmiSinkInfo.NumVideoInfo], sizeof(AMBA_HDMI_VIDEO_DESCRIPTOR_s)) != ERR_NONE) {
            RetVal = HDMI_ERR_UNEXPECTED;
        }
        for (i = 0U; i < LengthVIC; i ++) {
            VicDesc.Valid2D = 0U;
            switch (pHdmiVSDB[Offset + i]) {
            case 0x01U:
                VicDesc.FormatID = HDMI_VIC_2160P30;
                VicDesc.Valid2D = 1U;
                break;
            case 0x02U:
                VicDesc.FormatID = HDMI_VIC_2160P25;
                VicDesc.Valid2D = 1U;
                break;
            case 0x03U:
                VicDesc.FormatID = HDMI_VIC_2160P24;
                VicDesc.Valid2D = 1U;
                break;
            case 0x04U:
                VicDesc.FormatID = HDMI_VIC_2160P24_ULTRAWIDE;
                VicDesc.Valid2D = 1U;
                break;
            default:
                /* Do nothing!! */
                break;
            }

            if (VicDesc.Valid2D == 1U) {
                if (AmbaWrap_memcpy(&AmbaHdmiSinkInfo.VideoInfoBuf[AmbaHdmiSinkInfo.NumVideoInfo], &VicDesc, sizeof(AMBA_HDMI_VIDEO_DESCRIPTOR_s)) != ERR_NONE) {
                    RetVal = HDMI_ERR_UNEXPECTED;
                }
                AmbaHdmiSinkInfo.NumVideoInfo++;
            }
        }
    }

    return RetVal;
}

/**
 *  HDMI_ParseHdmiForumVSDB - Parse HDMI Forum Vendor-Specific Data Block
 *  @param[in] pHdmiVSDB pointer to HDMI VSDB within CEA-861 Extension block
 *  @return error code
 */
static UINT32 HDMI_ParseHdmiForumVSDB(const UINT8 *pHdmiVSDB)
{
    AmbaHdmiSinkInfo.ScdcPresent = ((UINT32)pHdmiVSDB[6] >> 7U);

    return HDMI_ERR_NONE;
}

/**
 *  HDMI_ParseDTD - Parse EDID 18 Byte detailed timing descriptor
 *  @param[in] pDescriptor pointer to 18 bytes detailed timing descriptor
 */
static void HDMI_ParseDTD(const UINT8 *pDescriptor)
{
    AMBA_HDMI_VIDEO_TIMING_s *pVideoTimings = &(AmbaHdmiSinkInfo.DtdBuf[AmbaHdmiSinkInfo.NumDTD]);
    AMBA_HDMI_VIDEO_DESCRIPTOR_s VicDesc;
    UINT16 H_Blanking, V_Blanking;

    pVideoTimings->Vic4_3 = 0U;
    pVideoTimings->Vic16_9 = 0U;
    pVideoTimings->Vic64_27 = 0U;
    pVideoTimings->Vic256_135 = 0U;
    pVideoTimings->PixelRepeat = 0U;
    pVideoTimings->FieldRate = 0U;

    /* Detailed Timing Descriptor */
    pVideoTimings->PixelClock   = (((UINT32)pDescriptor[1] << 8U) | (UINT32)pDescriptor[0]) * 10U;
    pVideoTimings->ActivePixels = (((UINT16)pDescriptor[4] >> 4U) << 8U) | (UINT16)pDescriptor[2];
    pVideoTimings->ActiveLines  = (((UINT16)pDescriptor[7] >> 4U) << 8U) | (UINT16)pDescriptor[5];
    H_Blanking = (((UINT16)pDescriptor[4] & 0xFU) << 8U) | (UINT16)pDescriptor[3];
    V_Blanking = (((UINT16)pDescriptor[7] & 0xFU) << 8U) | (UINT16)pDescriptor[6];
    pVideoTimings->HsyncFrontPorch = ((((UINT16)pDescriptor[11] >> 6U) & 0x3U) << 8U) | (UINT16)pDescriptor[8];
    pVideoTimings->HsyncPulseWidth = ((((UINT16)pDescriptor[11] >> 4U) & 0x3U) << 8U) | (UINT16)pDescriptor[9];
    pVideoTimings->HsyncBackPorch = (UINT16)H_Blanking - (pVideoTimings->HsyncFrontPorch + pVideoTimings->HsyncPulseWidth);
    pVideoTimings->VsyncFrontPorch = ((((UINT16)pDescriptor[11] >> 2U) & 0x3U) << 4U) | ((UINT16)pDescriptor[10] >> 4U);
    pVideoTimings->VsyncPulseWidth = ((((UINT16)pDescriptor[11]) & 0x3U) << 4U) | ((UINT16)pDescriptor[10] & 0xfU);
    pVideoTimings->VsyncBackPorch = (UINT16)V_Blanking - (pVideoTimings->VsyncFrontPorch + pVideoTimings->VsyncPulseWidth);

    if ((pDescriptor[17] & 0x80U) != 0U) {
        pVideoTimings->ScanFormat = AMBA_VIDEO_SCAN_INTERLACED;
        pVideoTimings->ActiveLines <<= 1U;
    } else {
        pVideoTimings->ScanFormat = AMBA_VIDEO_SCAN_PROGRESSIVE;
    }

    if ((pDescriptor[17] & 0x10U) != 0U) {
        /* Digital Sync Signal */
        switch (pDescriptor[17] & 0x0EU) {
        case 0x08U:
        case 0x04U:  /* treated as 0x00 */
        case 0x00U:
            pVideoTimings->SyncPolarity = AMBA_VIDEO_SYNC_H_NEG_V_NEG;
            break;
        case 0x0EU:
        case 0x06U:  /* treated as 0x02 */
        case 0x02U:
            pVideoTimings->SyncPolarity = AMBA_VIDEO_SYNC_H_POS_V_POS;
            break;
        case 0x0CU:
            pVideoTimings->SyncPolarity = AMBA_VIDEO_SYNC_H_NEG_V_POS;
            break;
        case 0x0AU:
            pVideoTimings->SyncPolarity = AMBA_VIDEO_SYNC_H_POS_V_NEG;
            break;
        default:
            /* Do nothing!! */
            break;
        }
    }

    /* Try to identify the video format */
    (void)AmbaRTSL_HdmiVicTag(pVideoTimings);

    AmbaHdmiSinkInfo.VideoInfoBuf[AmbaHdmiSinkInfo.NumVideoInfo] = 0U;
    if (AmbaWrap_memcpy(&VicDesc, &AmbaHdmiSinkInfo.VideoInfoBuf[AmbaHdmiSinkInfo.NumVideoInfo], sizeof(AMBA_HDMI_VIDEO_DESCRIPTOR_s)) != ERR_NONE) {
        /* misrac */
    }
    VicDesc.FormatID = (UINT16)(HDMI_VIC_DTD0 + (UINT16)AmbaHdmiSinkInfo.NumDTD);
    if (VicDesc.FormatID < NUM_HDMI_VIC) {
        VicDesc.Valid2D = 1U;
        if (AmbaWrap_memcpy(&AmbaHdmiSinkInfo.VideoInfoBuf[AmbaHdmiSinkInfo.NumVideoInfo], &VicDesc, sizeof(AMBA_HDMI_VIDEO_DESCRIPTOR_s)) != ERR_NONE) {
            /* misrac */
        }
        AmbaHdmiSinkInfo.NumVideoInfo++;
    }

    AmbaHdmiSinkInfo.NumDTD++;
}

/**
 *  HDMI_ParseEdidVendorProductID - Parse EDID 10 Byte Vendor & Product ID
 *  @param[in] pEdidStruct pointer to EDID block 0
 */
static void HDMI_ParseEdidVendorProductID(const EDID_FORMAT_s *pEdidStruct)
{
    AMBA_HDMI_SINK_PRODUCT_INFO_s *pProductInfo = &(AmbaHdmiSinkInfo.SinkProductInfo);
    UINT16 IdManufacturerName = ((UINT16)pEdidStruct->IdManufacturerName[0] << 8U) | pEdidStruct->IdManufacturerName[1];

    pProductInfo->ManufacturerName[0] = (((IdManufacturerName >> 10U) - 1U) & 0x1FU) + 'A';
    pProductInfo->ManufacturerName[1] = (((IdManufacturerName >> 5U) - 1U) & 0x1FU) + 'A';
    pProductInfo->ManufacturerName[2] = (((IdManufacturerName) - 1U) & 0x1FU) + 'A';
    pProductInfo->WeekOfManufacture = pEdidStruct->WeekOfManufacture;
    pProductInfo->YearOfManufacture = 1990U + pEdidStruct->YearOfManufacture;
    pProductInfo->ProductCode[0] = pEdidStruct->IdProductCode[0];
    pProductInfo->ProductCode[1] = pEdidStruct->IdProductCode[1];
    pProductInfo->SerialNumber[0] = pEdidStruct->IdSerialNumber[0];
    pProductInfo->SerialNumber[1] = pEdidStruct->IdSerialNumber[1];
    pProductInfo->SerialNumber[2] = pEdidStruct->IdSerialNumber[2];
    pProductInfo->SerialNumber[3] = pEdidStruct->IdSerialNumber[3];
}

/**
 *  HDMI_ParseEdidDescriptor - Parse EDID 18 Byte descriptor
 *  @param[in] pDescriptor:    pointer to 18 bytes descriptor
 */
static void HDMI_ParseEdidDescriptor(const UINT8 *pDescriptor)
{
    if ((pDescriptor[0] == 0x00U) && (pDescriptor[1] == 0x00U)) {
        /* Display Descriptors */
    } else {
        /* Detailed Timing Descriptor */
        HDMI_ParseDTD(pDescriptor);
    }
}

/**
 *  HDMI_ParseCea861TagExtended - Parse CEA-861 block with extended tag
 *  @param[in] pDataBlock pointer to CEA-861 Data Block
 */
static void HDMI_ParseCea861TagExtended(const UINT8 *pDataBlock)
{
    switch (pDataBlock[1]) {
    case 0U:     /* Video Capability Data Block (VCDB) */
        if ((pDataBlock[2] & 0x40U) != 0U) {   /* RGB is selectable quantization range */
            AmbaHdmiSinkInfo.QuantRangeCapability |= 0x01U;
        }
        if ((pDataBlock[2] & 0x80U) != 0U) {   /* YCC is selectable quantization range */
            AmbaHdmiSinkInfo.QuantRangeCapability |= 0x10U;
        }
        break;
    case 1U:     /* Vendor-Specific Video Data Block */
    case 5U:     /* Colorimetry Data Block */
    case 17U:    /* Vendor-Specific Audio Data Block */
    default:
        /* Do nothing!! */
        break;
    }
}

/**
 *  HDMI_ParseCea861Tag - Parse CEA-861 block accodring to CEA-861 tag
 *  @param[in] pDataBlock pointer to CEA-861 Data Block
 *  @param[in] Tag CEA-861 tag
 *  @param[in] Length length of CEA-861 Data Block
 *  @return A flag to indicate HDMI is supported or not
 */
static UINT32 HDMI_ParseCea861Tag(const UINT8 *pDataBlock, UINT32 Tag, UINT32 Length)
{
    AMBA_HDMI_VIDEO_DESCRIPTOR_s VicDesc;
    AMBA_HDMI_AUDIO_DESCRIPTOR_s AicDesc;
    UINT32 IeeeOui, AIC;
    UINT32 i, HdmiSupportFlag = 0U;
    UINT8 VIC[32];

    switch (Tag) {
    case EDID_CEA861_TAG_EXTENDED:
        HDMI_ParseCea861TagExtended(pDataBlock);
        break;

    case EDID_CEA861_TAG_VENDOR:
        /* Check IEEE OUI of Vendor-Specific Data Block (VSDB) */
        IeeeOui = ((UINT32)pDataBlock[3] << 16U) |
                  ((UINT32)pDataBlock[2] << 8U)  |
                  ((UINT32)pDataBlock[1]);
        if (IeeeOui == IEEE_OUI_HDMI_LICENSING_LLC) {
            (void)HDMI_ParseHdmi14bVSDB(pDataBlock);
        } else if (IeeeOui == 0xC45DD8U) {
            (void)HDMI_ParseHdmiForumVSDB(pDataBlock);
        } else {
            break;
        }
        HdmiSupportFlag = 1U;
        break;

    case EDID_CEA861_TAG_VIDEO:
        for (i = 0U; i < Length; i ++) {
            AmbaHdmiSinkInfo.VideoInfoBuf[AmbaHdmiSinkInfo.NumVideoInfo] = 0U;
            if (AmbaWrap_memcpy(&VicDesc, &AmbaHdmiSinkInfo.VideoInfoBuf[AmbaHdmiSinkInfo.NumVideoInfo], sizeof(AMBA_HDMI_VIDEO_DESCRIPTOR_s)) != ERR_NONE) {
                /* misrac */
            }
            VIC[i] = pDataBlock[i + 1U] & 0x7fU;    /* 7-bit SVD */
            if (VIC[i] <= 64U) {
                VicDesc.Native = (UINT8)(pDataBlock[i + 1U] >> 7U);
            } else {
                VIC[i] = pDataBlock[i + 1U];        /* 8-bit SVD (CEA-861-F) */
            }
            VicDesc.FormatID = VIC[i];
            VicDesc.Valid2D = 1U;
            if (AmbaWrap_memcpy(&AmbaHdmiSinkInfo.VideoInfoBuf[AmbaHdmiSinkInfo.NumVideoInfo], &VicDesc, sizeof(AMBA_HDMI_VIDEO_DESCRIPTOR_s)) != ERR_NONE) {
                /* misrac */
            }
            AmbaHdmiSinkInfo.NumVideoInfo++;
        }
        VIC[i] = 0x00;
        break;

    case EDID_CEA861_TAG_AUDIO:
        for (i = 0U; i < Length; i += 3U) {
            AIC = ((UINT32)pDataBlock[i + 1U] >> 3U) & 0xfU;
            AmbaHdmiSinkInfo.AudioInfoBuf[AIC] = 0U;
            if (AmbaWrap_memcpy(&AicDesc, &AmbaHdmiSinkInfo.AudioInfoBuf[AIC], sizeof(AMBA_HDMI_AUDIO_DESCRIPTOR_s)) != ERR_NONE) {
                /* misrac */
            }
            if (AIC == 0xFU) {
                /* Audio Format Code Extension */
                AIC = ((UINT32)pDataBlock[i + 3U] >> 3U) + HDMI_AIC_EXT;
                AicDesc.Detail = 0U;
            } else {
                AicDesc.Detail |= pDataBlock[i + 3U];
            }
            AicDesc.SampleRate |= pDataBlock[i + 2U];
            AicDesc.MaxNumChan = (pDataBlock[i + 1U] & 0x7U) + 1U;
            if (AmbaWrap_memcpy(&AmbaHdmiSinkInfo.AudioInfoBuf[AIC], &AicDesc, sizeof(AMBA_HDMI_AUDIO_DESCRIPTOR_s)) != ERR_NONE) {
                /* misrac */
            }
        }
        break;

    default:
        /* Do nothing!! */
        break;
    }

    return HdmiSupportFlag;
}

/**
 *  HDMI_ParseCea861DTD - Parse CEA-861 extention to check declared DTDs
 *  @param[in] pCea861Edid pointer to CEA-861 Extension EDID Block
 *  @param[in] Offset DTD offset in CEA-861 Extension EDID Block
 *  @param[in] NumOfNativeDTD number of native DTDs
 */
static void HDMI_ParseCea861DTD(const EDID_CEA861_EXT_V3_FORMAT_s *pCea861Edid, UINT32 Offset, UINT8 NumOfNativeDTD)
{
    UINT32 i, j;
    UINT32 LoopBreak = 0U;
    UINT32 ParseOffset = Offset;
    UINT32 ParseNumOfNativeDTD = NumOfNativeDTD;

    for (i = 0U; i < 6U; i ++) {
        if ((ParseOffset + 18U) <= 123U) {
            if ((pCea861Edid->Data[ParseOffset] == 0U) && (pCea861Edid->Data[ParseOffset + 1U] == 0U)) {
                /* It is not a detailed timing descriptor */
                LoopBreak = 1U;
                for (j = 2U; j < 18U; j++) {
                    if (pCea861Edid->Data[ParseOffset + j] != 0U) {
                        LoopBreak = 0U;
                        break;
                    }
                }

            } else {
                HDMI_ParseDTD(&pCea861Edid->Data[ParseOffset]);

                if (ParseNumOfNativeDTD > 0U) {
                    AmbaHdmiSinkInfo.VideoInfoBuf[AmbaHdmiSinkInfo.NumVideoInfo - 1U] |= 0x8000U; /* set native flag */
                    ParseNumOfNativeDTD --;
                }
            }
            ParseOffset += 18U;
        } else {
            LoopBreak = 1U;
        }
        if (LoopBreak == 1U) {
            /* Unused descriptor locations in CEA Extension or
               ParseOffset + 18 > 123 */
            break;
        }
    }
}

/**
 *  HDMI_ParseCea861Extension - Parse CEA-861 extention to check if the display device supports features specific to HDMI
 *  @param[in] pCea861Edid pointer to CEA-861 Extension EDID Block
 *  @return A flag to indicate HDMI is supported or not
 */
static UINT32 HDMI_ParseCea861Extension(const EDID_CEA861_EXT_V3_FORMAT_s *pCea861Edid)
{
    AMBA_HDMI_AUDIO_DESCRIPTOR_s AicDesc;
    const UINT8 *pDataBlock;
    UINT32 Offset, Length, Tag;
    UINT8 NumOfNativeDTD;
    UINT32 HdmiSupportFlag = 0U;

    /* Extract supported video timings/audio sample rate */
    Offset = (UINT32)pCea861Edid->OffsetDTD - 4U;

    if (pCea861Edid->RevisionNumber == 1U) {
        NumOfNativeDTD = 0U;     /* we don't trust the EDID value */
    } else {
        NumOfNativeDTD = pCea861Edid->NumOfDTD & 0xFU;

        /* In order to ensure YCbCr interoperability between any two YCbCr-capable devices, a DTV monitor that
           supports either type of YCbCr pixel data (4:2:2 or 4:4:4) should support both types and therefore would
           set both bits 4 and 5 of byte 3. */
        if ((pCea861Edid->NumOfDTD & (EDID_CEA861_YUV_422_SUPPORTED | EDID_CEA861_YUV_444_SUPPORTED)) != 0U) {
            AmbaHdmiSinkInfo.ColorFormatCapability |= 0x30U; /* Both YCC422 and YCC444 are supported */
        }
    }

    HDMI_ParseCea861DTD(pCea861Edid, Offset, NumOfNativeDTD);

    AmbaHdmiSinkInfo.AudioInfoBuf[HDMI_AIC_LPCM] = 0U;
    if ((pCea861Edid->NumOfDTD & EDID_CEA861_BASIC_AUDIO_SUPPORTED) != 0U) {
        if (AmbaWrap_memcpy(&AicDesc, &AmbaHdmiSinkInfo.AudioInfoBuf[HDMI_AIC_LPCM], sizeof(AMBA_HDMI_AUDIO_DESCRIPTOR_s)) != ERR_NONE) {
            /* misrac */
        }
        AicDesc.MaxNumChan = 2U;
        AicDesc.SampleRate = (UINT8)(HDMI_AUD_FS_32K | HDMI_AUD_FS_44P1K | HDMI_AUD_FS_48K);
        AicDesc.Detail = HDMI_AUD_LPCM_16BIT;
        if (AmbaWrap_memcpy(&AmbaHdmiSinkInfo.AudioInfoBuf[HDMI_AIC_LPCM], &AicDesc, sizeof(AMBA_HDMI_AUDIO_DESCRIPTOR_s)) != ERR_NONE) {
            /* misrac */
        }
    }

    Offset = 0U;
    while (Offset < pCea861Edid->OffsetDTD) {   /* Search VSDB in data block collection */
        pDataBlock = &pCea861Edid->Data[Offset];
        Offset ++;

        Tag = (UINT32)pDataBlock[0] >> 5U;
        Length = (UINT32)pDataBlock[0] & 0x1fU;

        HdmiSupportFlag |= HDMI_ParseCea861Tag(pDataBlock, Tag, Length);

        Offset += Length;
    }

    return HdmiSupportFlag;
}

/**
 *  AmbaHDMI_TxParseEDID - Parse EDID to check if the display device supports features specific to HDMI
 *  @param[in] pEDID EDID buffer
 *  @param[out] pEdidDetect Indicate if the HDMI tag is found in EDID
 *  @return error code
 */
UINT32 AmbaHDMI_TxParseEDID(const UINT8 pEDID[EDID_NUM_BLOCKS][EDID_BLOCK_SIZE], UINT32 *pEdidDetect)
{
    const EDID_CEA861_EXT_V3_FORMAT_s *pEdidCea861Struct;
    const EDID_FORMAT_s *pEdidStruct;
    UINT32 i, RetVal = HDMI_ERR_NONE;
    const UINT8 *pEdidBlock;
    UINT32 HdmiSupport = 0U;

    if (AmbaWrap_memset(&AmbaHdmiSinkInfo, 0x0, sizeof(AMBA_HDMI_SINK_INFO_s)) == ERR_NONE) {

        pEdidBlock = pEDID[0];
        AmbaMisra_TypeCast(&pEdidStruct, &pEdidBlock);

        if (!((pEdidStruct->VersionNumber == 1U) && (pEdidStruct->RevisionNumber >= 3U))) {
            /* CEA-861 is not supported by the display device. Only DVI mode is available. */
            RetVal = HDMI_ERR_EDID;
        } else if (pEdidStruct->ExtensionBlockCount == 0U) {
            /* No Extension Block for CEA-861 Series Timing. Only DVI mode is available. */
            RetVal = HDMI_ERR_EDID;
        } else {
            AmbaHdmiSinkInfo.SrcPhysicalAddr = 0U;
            AmbaHdmiSinkInfo.DeepColorCapability = 0U;
            AmbaHdmiSinkInfo.QuantRangeCapability = 0U;
            AmbaHdmiSinkInfo.ColorFormatCapability = 0U;
            AmbaHdmiSinkInfo.NumDTD = 0U;
            AmbaHdmiSinkInfo.NumVideoInfo = 0U;
            AmbaHdmiSinkInfo.NumAudioInfo = NUM_HDMI_AIC;

            HDMI_ParseEdidVendorProductID(pEdidStruct);

            for (i = 0U; i < 4U; i ++) {
                HDMI_ParseEdidDescriptor(pEdidStruct->Descriptor[i]);
            }

            /* If bit 7 at address 14h(VideoInputDefinition) = 1, then bits 4 & 3 at address 18h(FeatureSupport) defines the YCbCr supportting */
            if ((pEdidStruct->VideoInputDefinition & 0x80U) != 0U) {
                /* YCbCr 4:4:4 */
                if ((pEdidStruct->FeatureSupport & 0x08U) != 0U) {
                    AmbaHdmiSinkInfo.ColorFormatCapability |= 0x10U;
                }

                /* YCbCr 4:2:2 */
                if ((pEdidStruct->FeatureSupport & 0x10U) != 0U) {
                    AmbaHdmiSinkInfo.ColorFormatCapability |= 0x20U;
                }
            }

            /* EDID version 1, revision 4 */
            if ((pEdidStruct->VersionNumber == 1U) && (pEdidStruct->RevisionNumber == 4U)) {
                if ((pEdidStruct->FeatureSupport & 0x2U) != 0U) {
                    /* The preferred timing mode includes the native pixel format */
                    AmbaHdmiSinkInfo.VideoInfoBuf[HDMI_VIC_DTD0] |= 0x8000U;
                }
            }

            for (i = 1U; i <= pEdidStruct->ExtensionBlockCount; i ++) {
                pEdidBlock = pEDID[i];
                if (pEdidBlock[0] == EDID_EXT_CEA) {
                    AmbaMisra_TypeCast(&pEdidCea861Struct, &pEdidBlock);
                    if (HdmiSupport == 0U)  {
                        /* Search HDMI VSDB in the first CEA Extension */
                        HdmiSupport = HDMI_ParseCea861Extension(pEdidCea861Struct);
                    } else {
                        (void)HDMI_ParseCea861Extension(pEdidCea861Struct);
                    }
                }
            }
        }

        if (HdmiSupport != 0U) {
            /* The sink device is HDMI capable */
            *pEdidDetect = HDMI_EDID_HDMI;
        } else {
            /* The sink device is not HDMI capable */
            *pEdidDetect = HDMI_EDID_DVI;
        }
    } else {
        RetVal = HDMI_ERR_UNEXPECTED;
    }
    return RetVal;
}

/**
 *  AmbaHDMI_AudioChannelSelect - Selects target HDMI audio channel to be programmed
 *  @param[in] HDMIAudioChannel Target vout channel
 *  @note this function is an internal using only API
 */
void AmbaHDMI_AudioChannelSelect(UINT32 HDMIAudioChannel)
{
    I2sChanNo = HDMIAudioChannel;
    AmbaRTSL_HdmiSetAudioSrc(HDMIAudioChannel);
}

/**
 *  AmbaHDMI_TxConfigAudio - Configure audio
 *  @param[in] pVideoConfig pointer to video format configuration
 *  @param[in] pAudioConfig pointer to audio format configuration
 *  @return error code
 */
UINT32 AmbaHDMI_TxConfigAudio(const AMBA_HDMI_VIDEO_CONFIG_s *pVideoConfig, const AMBA_HDMI_AUDIO_CONFIG_s *pAudioConfig)
{
    const AMBA_HDMI_VIDEO_TIMING_s *pVideoTiming = &pVideoConfig->CustomDTD;
    UINT64 TmdsClock = 0ULL;
    UINT32 RetVal = HDMI_ERR_NONE;
    UINT32 AudioClkFreq;

    if (pVideoConfig->ColorDepth == HDMI_COLOR_DEPTH_24BPP) {
        TmdsClock = (UINT64)pVideoTiming->PixelClock * 1000ULL;

        switch (pVideoTiming->FieldRate) {
        case AMBA_VIDEO_FIELD_RATE_24HZ:
        case AMBA_VIDEO_FIELD_RATE_30HZ:
        case AMBA_VIDEO_FIELD_RATE_60HZ:
        case AMBA_VIDEO_FIELD_RATE_120HZ:
        case AMBA_VIDEO_FIELD_RATE_240HZ:
            TmdsClock = (UINT64) (TmdsClock * 1000U / 1001U);
            break;

        default:
            /* Do nothing!! */
            break;
        }

        if (I2sChanNo == AMBA_I2S_CHANNEL0) {
#if defined(CONFIG_QNX)
            (void)AmbaSYS_GetIoClkFreq(AMBA_CLK_AUDIO, &AudioClkFreq);
#else
#if defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)
            AudioClkFreq = AmbaRTSL_PllGetAudio3Clk();
#else
            AudioClkFreq = AmbaRTSL_PllGetAudioClk();
#endif
#endif
        } else if (I2sChanNo == AMBA_I2S_CHANNEL1) {
#if defined(CONFIG_QNX)
            (void)AmbaSYS_GetIoClkFreq(AMBA_CLK_AUDIO1, &AudioClkFreq);
#else
#if defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)
            AudioClkFreq = AmbaRTSL_PllGetAudio2Clk();
#elif defined(CONFIG_SOC_CV25) || defined(CONFIG_SOC_H32)
            AudioClkFreq = 0U;
#else
            AudioClkFreq = AmbaRTSL_PllGetAudio1Clk();
#endif
#endif
        } else {
            AudioClkFreq = 0U;
        }

        /* Configure Audio Source Info */
        RetVal = AmbaRTSL_HdmiConfigAudio(TmdsClock, AudioClkFreq, pAudioConfig);
    } else {
        RetVal = HDMI_ERR_ARG;
    }

    return RetVal;
}

/**
 *  AmbaHDMI_TxSendAudioInfoFrame - Fill Audio InfoFrame Packet
 *  @param[in] pPacket pointer to a packet buffer
 *  @param[in] pAudioConfig pointer to audio format configuration
 *  @param[in] TxMode Packet transmission frequency
 *  @return error code
 */
UINT32 AmbaHDMI_TxSendAudioInfoFrame(AMBA_HDMI_DATA_ISLAND_PACKET_s *pPacket, const AMBA_HDMI_AUDIO_CONFIG_s *pAudioConfig, UINT32 TxMode)
{
    AMBA_HDMI_PACKET_HEADER_s PktHeader = {0U};
    UINT32 NumAudioChan;
    UINT32 RetVal = HDMI_ERR_NONE;

    if (pAudioConfig != NULL) {
        AmbaRTSL_HdmiGetNumAudioChan(pAudioConfig->SpeakerMap, &NumAudioChan);

        /* Audio InfoFrame Packet */
        if (NumAudioChan > 0U) {
            PktHeader.Type = HDMI_INFOFRAME_AUDIO;
            PktHeader.InfoFrameFlag = 1U;
            PktHeader.Version = 0x01U;
            PktHeader.Length = HDMI_INFOFRAME_SIZE_AUDIO;
            pPacket->Payload[0] = (UINT8)NumAudioChan - 1U;
            pPacket->Payload[1] = 0x00U;
            pPacket->Payload[2] = 0x00U;
            pPacket->Payload[3] = (UINT8)pAudioConfig->SpeakerMap;
            pPacket->Payload[4] = 0x00U;
            pPacket->Payload[5] = 0x00U;
            pPacket->Payload[6] = 0x00U;
            pPacket->Payload[7] = 0x00U;
            pPacket->Payload[8] = 0x00U;
            pPacket->Payload[9] = 0x00U;
            if (AmbaWrap_memcpy(&pPacket->Header, &PktHeader, sizeof(PktHeader)) != ERR_NONE) {
                RetVal = HDMI_ERR_UNEXPECTED;
            }
            AmbaRTSL_HdmiDataIslandPackCtrl(pPacket, TxMode);
        }
    }

    return RetVal;
}

/**
 *  AmbaHDMI_TxSendAviInfoFrame - Fill AVI InfoFrame Packet
 *  @param[in] pPacket pointer to a packet buffer
 *  @param[in] pVideoConfig pointer to video format configuration
 *  @param[in] TxMode Packet transmission frequency
 *  @return error code
 */
UINT32 AmbaHDMI_TxSendAviInfoFrame(AMBA_HDMI_DATA_ISLAND_PACKET_s *pPacket, const AMBA_HDMI_VIDEO_CONFIG_s *pVideoConfig, UINT32 TxMode)
{
    AMBA_HDMI_PACKET_HEADER_s PktHeader = {0U};
    const AMBA_HDMI_VIDEO_TIMING_s *pVideoTiming;
    UINT32 RetVal = HDMI_ERR_NONE;
    UINT8 VideoIdCode;


    if (pVideoConfig == NULL) {
        RetVal = HDMI_ERR_ARG;
    } else {
        VideoIdCode = HDMI_GET_RAW_VIC(pVideoConfig->VideoIdCode);

        pVideoTiming = &pVideoConfig->CustomDTD;

        /* AVI InforFrame Packet */
        PktHeader.Type = HDMI_INFOFRAME_AVI;
        PktHeader.InfoFrameFlag = 1U;
#if defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)
        PktHeader.Version = 0x04U;
#else
        PktHeader.Version = 0x02U;
#endif
        PktHeader.Length = HDMI_INFOFRAME_SIZE_AVI;
        if (pVideoConfig->PixelFormat == HDMI_PIXEL_FORMAT_YCC_422) {
            pPacket->Payload[0] = 0x20U;
        } else if (pVideoConfig->PixelFormat == HDMI_PIXEL_FORMAT_YCC_444) {
            pPacket->Payload[0] = 0x40U;
        } else if (pVideoConfig->PixelFormat == HDMI_PIXEL_FORMAT_YCC_420) {
            pPacket->Payload[0] = 0x60U;
        } else {
            pPacket->Payload[0] = 0x00U;
        }
        pPacket->Payload[0] |= 0x11U;       /* AFD, Composed for overscan */
        if (VideoIdCode == pVideoTiming->Vic16_9) {
            pPacket->Payload[1] = 0x20U;
        } else if (VideoIdCode == pVideoTiming->Vic4_3) {
            pPacket->Payload[1] = 0x10U;
        } else {
            pPacket->Payload[1] = 0x00U;
        }
        pPacket->Payload[1] |= 0x08U;       /* Same as coded frame aspect ratio */
        if (pVideoTiming->ActiveLines >= 720U) {
            pPacket->Payload[1] |= 0x80U;   /* HD video, use BT.709 color space by default */
        } else {
            pPacket->Payload[1] |= 0x40U;   /* SD video, use BT.601 color space by default */
        }

        /*
         * RGB quantization range:
         * A source shall not send a non-zero Q value thata does not correspond to
         * the default range for the transmitted video format unless the sink
         * indicates suport for the Q bit in a Video Capabilities Data Block.
         */
        if (pVideoConfig->PixelFormat == HDMI_PIXEL_FORMAT_RGB_444) {
            pPacket->Payload[2] = (UINT8)((UINT16)pVideoConfig->QuantRange << 2U);
        } else {
            pPacket->Payload[2] = 0x00U;
        }

        if (((VideoIdCode == HDMI_VIC_2160P30) ||
             (VideoIdCode == HDMI_VIC_2160P25) ||
             (VideoIdCode == HDMI_VIC_2160P24) ||
             (VideoIdCode == HDMI_VIC_2160P24_ULTRAWIDE))) {
            pPacket->Payload[3] = 0x00U;   /* Extended video format */
        } else {
            pPacket->Payload[3] = VideoIdCode;
        }

        if (pVideoTiming->PixelRepeat != 0U) {
            pPacket->Payload[4] = 0x01U;   /* pixel data sent 2 times */
        } else {
            pPacket->Payload[4] = 0x00U;   /* No Pixel Repetition */
        }

        /* YCC quantization range */
        if (((pVideoConfig->QuantRange == HDMI_QRANGE_DEFAULT) && (VideoIdCode == HDMI_VIC_DMT0659)) ||
            (pVideoConfig->QuantRange == HDMI_QRANGE_FULL)) {
            pPacket->Payload[4] |= (0x01U << 6U);
        }

        pPacket->Payload[5] = 0x00U;
        pPacket->Payload[6] = 0x00U;
        pPacket->Payload[7] = 0x00U;
        pPacket->Payload[8] = 0x00U;
        pPacket->Payload[9] = 0x00U;
        pPacket->Payload[10] = 0x00U;
        pPacket->Payload[11] = 0x00U;
        pPacket->Payload[12] = 0x00U;

        if (AmbaWrap_memcpy(&pPacket->Header, &PktHeader, sizeof(PktHeader)) != ERR_NONE) {
            RetVal = HDMI_ERR_UNEXPECTED;
        }
        AmbaRTSL_HdmiDataIslandPackCtrl(pPacket, TxMode);
    }

    return RetVal;
}

/**
 *  AmbaHDMI_TxSendSpdInfoFrame - Fill SPD InfoFrame Packet
 *  @param[in] pPacket pointer to a packet buffer
 *  @param[in] pSPD pointer to Source Product Information
 *  @param[in] TxMode Packet transmission frequency
 *  @return error code
 */
UINT32 AmbaHDMI_TxSendSpdInfoFrame(AMBA_HDMI_DATA_ISLAND_PACKET_s *pPacket, const AMBA_HDMI_SOURCE_PRODUCT_INFO_s *pSPD, UINT32 TxMode)
{
    AMBA_HDMI_PACKET_HEADER_s PktHeader = {0U};
    UINT32 RetVal = HDMI_ERR_NONE;

    /* Source Product Description InfoFrame Packet */
    if ((pSPD != NULL) && (pSPD->SrcType < NUM_HDMI_SRC_TYPE)) {
        PktHeader.Type = HDMI_INFOFRAME_SPD;
        PktHeader.InfoFrameFlag = 1U;
        PktHeader.Version = 0x01U;
        PktHeader.Length = HDMI_INFOFRAME_SIZE_SPD;
        pPacket->Payload[0] = (UINT8)pSPD->VendorName[0];
        pPacket->Payload[1] = (UINT8)pSPD->VendorName[1];
        pPacket->Payload[2] = (UINT8)pSPD->VendorName[2];
        pPacket->Payload[3] = (UINT8)pSPD->VendorName[3];
        pPacket->Payload[4] = (UINT8)pSPD->VendorName[4];
        pPacket->Payload[5] = (UINT8)pSPD->VendorName[5];
        pPacket->Payload[6] = (UINT8)pSPD->VendorName[6];
        pPacket->Payload[7] = (UINT8)pSPD->VendorName[7];
        pPacket->Payload[8] = (UINT8)pSPD->ProductDescription[0];
        pPacket->Payload[9] = (UINT8)pSPD->ProductDescription[1];
        pPacket->Payload[10] = (UINT8)pSPD->ProductDescription[2];
        pPacket->Payload[11] = (UINT8)pSPD->ProductDescription[3];
        pPacket->Payload[12] = (UINT8)pSPD->ProductDescription[4];
        pPacket->Payload[13] = (UINT8)pSPD->ProductDescription[5];
        pPacket->Payload[14] = (UINT8)pSPD->ProductDescription[6];
        pPacket->Payload[15] = (UINT8)pSPD->ProductDescription[7];
        pPacket->Payload[16] = (UINT8)pSPD->ProductDescription[8];
        pPacket->Payload[17] = (UINT8)pSPD->ProductDescription[9];
        pPacket->Payload[18] = (UINT8)pSPD->ProductDescription[10];
        pPacket->Payload[19] = (UINT8)pSPD->ProductDescription[11];
        pPacket->Payload[20] = (UINT8)pSPD->ProductDescription[12];
        pPacket->Payload[21] = (UINT8)pSPD->ProductDescription[13];
        pPacket->Payload[22] = (UINT8)pSPD->ProductDescription[14];
        pPacket->Payload[23] = (UINT8)pSPD->ProductDescription[15];
        pPacket->Payload[24] = (UINT8)pSPD->SrcType;
        if (AmbaWrap_memcpy(&pPacket->Header, &PktHeader, sizeof(PktHeader)) != ERR_NONE) {
            RetVal = HDMI_ERR_UNEXPECTED;
        }
        AmbaRTSL_HdmiDataIslandPackCtrl(pPacket, TxMode);
    }

    return RetVal;
}

/**
 *  AmbaHDMI_TxSendVendorInfoFrame - Fill Vendor-Specific InfoFrame Packet
 *  @param[in] pPacket pointer to a packet buffer
 *  @param[in] pVideoConfig pointer to video format configuration
 *  @param[in] TxMode Packet transmission frequency
 *  @return error code
 */
UINT32 AmbaHDMI_TxSendVendorInfoFrame(AMBA_HDMI_DATA_ISLAND_PACKET_s *pPacket, const AMBA_HDMI_VIDEO_CONFIG_s *pVideoConfig, UINT32 TxMode)
{
    static UINT32 IeeeOui = IEEE_OUI_HDMI_LICENSING_LLC;
    AMBA_HDMI_PACKET_HEADER_s PktHeader = {0U};
    UINT8 IeeeOuiByte[4];
    UINT32 RetVal = HDMI_ERR_NONE;
    UINT8 VideoIdCode;

    if (pVideoConfig == NULL) {
        RetVal = HDMI_ERR_ARG;
    } else {
        VideoIdCode = HDMI_GET_RAW_VIC(pVideoConfig->VideoIdCode);

        if (AmbaWrap_memcpy(IeeeOuiByte, &IeeeOui, sizeof(UINT32)) != ERR_NONE) {
            RetVal = HDMI_ERR_UNEXPECTED;
        }

        /* HDMI 1.4b Vendor Specific InfoFrame Packet */
        PktHeader.Type = HDMI_INFOFRAME_VENDOR_SPECIFIC;
        PktHeader.InfoFrameFlag = 1U;
        PktHeader.Version = 0x01U;
        pPacket->Payload[0] = IeeeOuiByte[0];
        pPacket->Payload[1] = IeeeOuiByte[1];
        pPacket->Payload[2] = IeeeOuiByte[2];
        if (VideoIdCode == HDMI_VIC_2160P30) {
            PktHeader.Length = 5U;
            pPacket->Payload[3] = (0x01U << 5U);
            pPacket->Payload[4] = 1U;
        } else if (VideoIdCode == HDMI_VIC_2160P25) {
            PktHeader.Length = 5U;
            pPacket->Payload[3] = (0x01U << 5U);
            pPacket->Payload[4] = 2U;
        } else if (VideoIdCode == HDMI_VIC_2160P24) {
            PktHeader.Length = 5U;
            pPacket->Payload[3] = (0x01U << 5U);
            pPacket->Payload[4] = 3U;
        } else if (VideoIdCode == HDMI_VIC_2160P24_ULTRAWIDE) {
            PktHeader.Length = 5U;
            pPacket->Payload[3] = (0x01U << 5U);
            pPacket->Payload[4] = 4U;
        } else {
            /* No additional HDMI video format */
            PktHeader.Length = 4U;
            pPacket->Payload[3] = 0x00U;
        }

        if (AmbaWrap_memcpy(&pPacket->Header, &PktHeader, sizeof(PktHeader)) != ERR_NONE) {
            RetVal = HDMI_ERR_UNEXPECTED;
        }
        AmbaRTSL_HdmiDataIslandPackCtrl(pPacket, TxMode);
    }

    return RetVal;
}

/**
 *  HDMI_GetVideoQuantRange - Get video quantization range
 *  @param[in] pVideoConfig pointer to video format configuration
 *  @return Video quantization range
 */
static UINT32 HDMI_GetVideoQuantRange(const AMBA_HDMI_VIDEO_CONFIG_s *pVideoConfig)
{
    UINT32 QuantRange = HDMI_QRANGE_DEFAULT;
    UINT8 VideoIdCode;

    VideoIdCode = HDMI_GET_RAW_VIC(pVideoConfig->VideoIdCode);

    if ((pVideoConfig->QuantRange == HDMI_QRANGE_LIMITED) || (pVideoConfig->QuantRange == HDMI_QRANGE_FULL)) {
        QuantRange = pVideoConfig->QuantRange;
    } else {
        /* By default, limited range shall be used for all video formats defined in CEA-861-D,
           with the exception of VGA (640x480) format, which requires full range */
        if ((VideoIdCode == HDMI_VIC_CUSTOM) ||
            (VideoIdCode == HDMI_VIC_DMT0659)) {
            QuantRange = HDMI_QRANGE_FULL;
        } else {
            QuantRange = HDMI_QRANGE_LIMITED;
        }
    }

    return QuantRange;
}

/**
 *  Float2Int32 - Convert a floating-point value to an integer
 *  @param[in] WorkFLOAT A floating-point value
 *  @return An integer
 */
static inline INT32 Float2Int32(FLOAT WorkFLOAT)
{
    INT32 WorkINT;

    if (WorkFLOAT >= 0.0F) {
        WorkINT = (INT32)(WorkFLOAT);
    } else {
        WorkINT = 0 - (INT32)(-WorkFLOAT);
    }
    return WorkINT;
}

/**
 *  HDMI_CalculateCsc - Calculate color space conversion parameters for HDMI interface
 *  @param[in] pCscMatrix Color space conversion matrix
 *  @param[in] pVoutCscData Color space conversion data
 */
static void HDMI_CalculateCsc(const AMBA_VOUT_CSC_MATRIX_s *pCscMatrix, UINT16 *pHdmiCscData)
{
    pHdmiCscData[0] = (UINT16)Float2Int32(pCscMatrix->Coef[0][0] * 1024.0F);
    pHdmiCscData[1] = (UINT16)Float2Int32(pCscMatrix->Coef[0][1] * 1024.0F);
    pHdmiCscData[2] = (UINT16)Float2Int32(pCscMatrix->Coef[0][2] * 1024.0F);
    pHdmiCscData[3] = (UINT16)Float2Int32(pCscMatrix->Coef[1][0] * 1024.0F);
    pHdmiCscData[4] = (UINT16)Float2Int32(pCscMatrix->Coef[1][1] * 1024.0F);
    pHdmiCscData[5] = (UINT16)Float2Int32(pCscMatrix->Coef[1][2] * 1024.0F);
    pHdmiCscData[6] = (UINT16)Float2Int32(pCscMatrix->Coef[2][0] * 1024.0F);
    pHdmiCscData[7] = (UINT16)Float2Int32(pCscMatrix->Coef[2][1] * 1024.0F);
    pHdmiCscData[8] = (UINT16)Float2Int32(pCscMatrix->Coef[2][2] * 1024.0F);
    pHdmiCscData[9] = (UINT16)Float2Int32(pCscMatrix->Offset[0]);
    pHdmiCscData[10] = (UINT16)Float2Int32(pCscMatrix->Offset[1]);
    pHdmiCscData[11] = (UINT16)Float2Int32(pCscMatrix->Offset[2]);
    pHdmiCscData[12] = pCscMatrix->MinVal[0];
    pHdmiCscData[13] = pCscMatrix->MaxVal[0];
    pHdmiCscData[14] = pCscMatrix->MinVal[1];
    pHdmiCscData[15] = pCscMatrix->MaxVal[1];
    pHdmiCscData[16] = pCscMatrix->MinVal[2];
    pHdmiCscData[17] = pCscMatrix->MaxVal[2];
}

/**
 *  AmbaHDMI_ConfigDSP - Configure DSP
 *  @param[in] pVideoConfig  pointer to video format configuration
 *  @return error code
 */
UINT32 AmbaHDMI_ConfigDSP(const AMBA_HDMI_VIDEO_CONFIG_s *pVideoConfig)
{
    const AMBA_HDMI_VIDEO_TIMING_s *pDtd;
    AMBA_VOUT_CSC_MATRIX_s *pCscMatrix;
    AMBA_VOUT_DISPLAY_TIMING_CONFIG_s DispTiming;
    UINT32 HdmiMode, FrameSyncPolarity, LineSyncPolarity, System50Hz;
    UINT16 FrameWidth, FrameHeight;
    UINT16 MaxPixelValue, CscSelect;
    UINT32 RetVal = HDMI_ERR_NONE;
    FLOAT PixelClkFreq;
    UINT8 StrictFrameRate;
    UINT16 HdmiCscData[HDMI_CSC_DATA_SIZE];

    if ((pVideoConfig->VideoIdCode & HDMI_FRAME_RATE_STRICT) == 0U) {
        StrictFrameRate = 0U;
    } else {
        StrictFrameRate = 1U;
    }

    if (pVideoConfig->PixelFormat == HDMI_PIXEL_FORMAT_RGB_444) {
        /* YCC444 to RGB */
        HdmiMode = VOUT_HDMI_MODE_RGB888_24BIT;
        if (HDMI_GetVideoQuantRange(pVideoConfig) == HDMI_QRANGE_LIMITED) {
            CscSelect = CSC_YCC_FULL_2_RGB_LIMIT;
        } else {
            CscSelect = CSC_YCC_FULL_2_RGB_FULL;
        }

        /* Default colorimetry. SD: SMPTE 170M/ITR-R BT.601, HD: ITU-R BT.709 */
        if (pVideoConfig->CustomDTD.ActiveLines >= 720U) {
            (void)AmbaVout_GetCscMatrix(CSC_TYPE_BT709, (UINT32)CscSelect, &pCscMatrix);
        } else {
            (void)AmbaVout_GetCscMatrix(CSC_TYPE_BT601, (UINT32)CscSelect, &pCscMatrix);
        }
    } else {
        if (pVideoConfig->PixelFormat == HDMI_PIXEL_FORMAT_YCC_444) {
            /* YCC444 to YCC444 (8-bit) */
            HdmiMode = VOUT_HDMI_MODE_YCC444_24BIT;
            MaxPixelValue = (UINT16)(((UINT16)1U << 8U) - 1U);
        } else if (pVideoConfig->PixelFormat == HDMI_PIXEL_FORMAT_YCC_420) {
            /* YCC444 to YCC420 (8-bit) */
            HdmiMode = VOUT_HDMI_MODE_YCC420_24BIT;
            MaxPixelValue = (UINT16)(((UINT16)1U << 8U) - 1U);
        } else {
            /* YCC444 to YCC444/YCC422 (12-bit) */
            HdmiMode = VOUT_HDMI_MODE_YCC422_24BIT;
            MaxPixelValue = (UINT16)(((UINT16)1U << 12U) - 1U);
        }
        (void)AmbaVout_GetCscMatrix(CSC_TYPE_IDENTITY, 0U, &pCscMatrix);
        pCscMatrix->MaxVal[0] = MaxPixelValue;
        pCscMatrix->MaxVal[1] = MaxPixelValue;
        pCscMatrix->MaxVal[2] = MaxPixelValue;
    }

    if (pVideoConfig->CustomDTD.SyncPolarity == AMBA_VIDEO_SYNC_H_NEG_V_NEG) {
        LineSyncPolarity = VOUT_SIGNAL_ACTIVE_LOW;
        FrameSyncPolarity = VOUT_SIGNAL_ACTIVE_LOW;
    } else if (pVideoConfig->CustomDTD.SyncPolarity == AMBA_VIDEO_SYNC_H_NEG_V_POS) {
        LineSyncPolarity = VOUT_SIGNAL_ACTIVE_LOW;
        FrameSyncPolarity = VOUT_SIGNAL_ACTIVE_HIGH;
    } else if (pVideoConfig->CustomDTD.SyncPolarity == AMBA_VIDEO_SYNC_H_POS_V_NEG) {
        LineSyncPolarity = VOUT_SIGNAL_ACTIVE_HIGH;
        FrameSyncPolarity = VOUT_SIGNAL_ACTIVE_LOW;
    } else {
        LineSyncPolarity = VOUT_SIGNAL_ACTIVE_HIGH;
        FrameSyncPolarity = VOUT_SIGNAL_ACTIVE_HIGH;
    }

    pDtd = &pVideoConfig->CustomDTD;
    DispTiming.PixelClkFreq     = pDtd->PixelClock * 1000U;
    DispTiming.DisplayMethod    = pDtd->ScanFormat;
    DispTiming.HsyncFrontPorch  = pDtd->HsyncFrontPorch;
    DispTiming.HsyncPulseWidth  = pDtd->HsyncPulseWidth;
    DispTiming.HsyncBackPorch   = pDtd->HsyncBackPorch;
    DispTiming.VsyncFrontPorch  = pDtd->VsyncFrontPorch;
    DispTiming.VsyncPulseWidth  = pDtd->VsyncPulseWidth;
    DispTiming.VsyncBackPorch   = pDtd->VsyncBackPorch;
    DispTiming.ActivePixels     = pDtd->ActivePixels;
    DispTiming.ActiveLines      = pDtd->ActiveLines;

    if (pVideoConfig->PixelFormat == HDMI_PIXEL_FORMAT_YCC_420) {
        /* Restore to orignal size when configure VOUT */
        DispTiming.HsyncFrontPorch  *= 2U;
        DispTiming.HsyncPulseWidth  *= 2U;
        DispTiming.HsyncBackPorch   *= 2U;
        DispTiming.ActivePixels     *= 2U;
    }

    FrameWidth = pDtd->HsyncFrontPorch + pDtd->HsyncPulseWidth + pDtd->HsyncBackPorch + pDtd->ActivePixels;
    FrameHeight = pDtd->VsyncFrontPorch + pDtd->VsyncPulseWidth + pDtd->VsyncBackPorch + pDtd->ActiveLines;

    if (pDtd->ScanFormat == AMBA_VIDEO_SCAN_INTERLACED) {
        FrameHeight += pDtd->VsyncFrontPorch + pDtd->VsyncPulseWidth + pDtd->VsyncBackPorch + 1U;
    }

    System50Hz = DispTiming.PixelClkFreq % (6U * (UINT32)FrameWidth * (UINT32)FrameHeight);
    if ((System50Hz == 0U) && (DispTiming.ActivePixels != 4096U) && (StrictFrameRate == 0U)) {
        PixelClkFreq = (FLOAT)DispTiming.PixelClkFreq / 1.001f;
        DispTiming.PixelClkFreq = (UINT32)PixelClkFreq;
    }

    /* Config HDMI PLL clock */
#if defined(CONFIG_QNX)
    (void)AmbaSYS_SetIoClkFreq(AMBA_CLK_VOUTTV, DispTiming.PixelClkFreq, NULL);
#else
#if defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)
    (void)AmbaRTSL_PllSetVoutCClkHint(AMBA_PLL_VOUT_TV_HDMI, 10U);
    (void)AmbaRTSL_PllSetVoutCClk(DispTiming.PixelClkFreq);
#else
    (void)AmbaRTSL_PllSetVoutTvClkHint(AMBA_PLL_VOUT_TV_HDMI, 10U);
    (void)AmbaRTSL_PllSetVoutTvClk(DispTiming.PixelClkFreq);
#endif
#endif

    HDMI_CalculateCsc(pCscMatrix, HdmiCscData);
    (void)AmbaRTSL_VoutHdmiSetCsc(HdmiCscData, HdmiMode);

    (void)AmbaRTSL_VoutHdmiEnable(HdmiMode, FrameSyncPolarity, LineSyncPolarity, &DispTiming);

    return RetVal;
}

