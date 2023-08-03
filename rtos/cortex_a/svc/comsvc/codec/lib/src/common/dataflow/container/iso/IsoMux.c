/**
 * @file IsoMux.c
 *
 * Copyright (c) 2019 Ambarella International LP
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
 * This file includes sample code and is only for internal testing and evaluation.  If you
 * distribute this sample code (whether in source, object, or binary code form), it will be
 * without any warranty or indemnity protection from Ambarella International LP or its affiliates.
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
 *
 */
#include "IsoMux.h"
#include "../Muxer.h"
#include "../FormatAPI.h"
#include "../ByteOp.h"
#include "../H264.h"
#include "../H265.h"
#include <AmbaRTC.h>


/* Box types, structure
 *
 *                      free
 *                      mdat
 * stsd
 * stsc
 * stts
 * stco
 * stsz
 * stss
 * ctts
 *      stbl
 * dref
 *      dinf
 *      vmhd / gmhd /smhd
 *          minf
 *          hdlr
 *          mdhd
 *              mdia
 *          elst
 *              edts
 *              tref
 *              tkhd
 *                  trak
 *                  udta
 *                  mvhd
 *                      moov
 *                      ftyp
 *
 */
static UINT32 IsoMux_PutMatrix(SVC_STREAM_HDLR_s *Stream, UINT32 Width, UINT32 Height, UINT32 Rotate)
{
    UINT8 ParamNo;
    UINT32 Rval = FORMAT_OK;
    UINT32 RotationParam[9];
    /*
        TODO: Find a better way to initialize array. The following initialization causes MisraC error (Rule 9.1).
        UINT32 RotationParam[9] = {0x10000U, 0U, 0U, 0U, 0x10000U, 0U, 0U, 0U, 0x40000000U};
     */
    RotationParam[0] = 0x10000U;
    RotationParam[1] = 0U;
    RotationParam[2] = 0U;
    RotationParam[3] = 0U;
    RotationParam[4] = 0x10000U;
    RotationParam[5] = 0U;
    RotationParam[6] = 0U;
    RotationParam[7] = 0U;
    RotationParam[8] = 0x40000000U;
    switch (Rotate) {
    case SVC_ISO_ROTATION_ROTATE270:
        RotationParam[0] = 0U;
        RotationParam[1] = 0x10000U;
        RotationParam[2] = 0U;
        RotationParam[3] = 0xFFFF0000U;
        RotationParam[4] = 0U;
        RotationParam[5] = 0U;
        RotationParam[6] = (Height << 16U);
        RotationParam[7] = 0U;
        RotationParam[8] = 0x40000000U;
        break;
    case SVC_ISO_ROTATION_ROTATE180:
        RotationParam[0] = 0xFFFF0000U;
        RotationParam[1] = 0U;
        RotationParam[2] = 0U;
        RotationParam[3] = 0U;
        RotationParam[4] = 0xFFFF0000U;
        RotationParam[5] = 0U;
        RotationParam[6] = (Width << 16U);
        RotationParam[7] = (Height << 16U);
        RotationParam[8] = 0x40000000U;
        break;
    case SVC_ISO_ROTATION_ROTATE90:
        RotationParam[0] = 0U;
        RotationParam[1] = 0xFFFF0000U;
        RotationParam[2] = 0U;
        RotationParam[3] = 0x10000U;
        RotationParam[4] = 0U;
        RotationParam[5] = 0U;
        RotationParam[6] = 0U;
        RotationParam[7] = (Width << 16U);
        RotationParam[8] = 0x40000000U;
        break;
    case SVC_ISO_ROTATION_NONE:
        break;
    default:
        AmbaPrint_ModulePrintStr5(SVC_MUXER_PRINT_MODULE_ID, "%s Unsupported rotation angle!", __func__, NULL, NULL, NULL, NULL);
        Rval = FORMAT_ERR_INVALID_ARG;
        break;
    }
    if (Rval == FORMAT_OK) {
        for (ParamNo = 0; ParamNo < 9U; ParamNo++) {
            Rval = SvcFormat_PutBe32(Stream, RotationParam[ParamNo]);
            if (Rval != FORMAT_OK) {
            //    LOG_ERR("SvcFormat_PutBe32() error!");
                break;
            }
        }
    }
    return Rval;
}

static UINT32 IsoMux_PutBuffer8(SVC_STREAM_HDLR_s *Stream, const UINT8 *Buffer, UINT32 Count)
{
    UINT32 Rval = FORMAT_OK;
    UINT32 i;
    for (i = 0; i < Count; i++) {
        Rval = SvcFormat_PutByte(Stream, Buffer[i]);
        if (Rval != FORMAT_OK) {
            break;
        }
    }
    return Rval;
}

static UINT32 IsoMux_PutBufferBe16(SVC_STREAM_HDLR_s *Stream, const UINT16 *Buffer, UINT32 Count)
{
    UINT32 Rval = FORMAT_OK;
    UINT32 i;
    for (i = 0; i < Count; i++) {
        Rval = SvcFormat_PutBe16(Stream, Buffer[i]);
        if (Rval != FORMAT_OK) {
            break;
        }
    }
    return Rval;
}

static UINT32 IsoMux_PutBufferBe32(SVC_STREAM_HDLR_s *Stream, const UINT32 *Buffer, UINT32 Count)
{
    UINT32 Rval = FORMAT_OK;
    UINT32 i;
    for (i = 0; i < Count; i++) {
        Rval = SvcFormat_PutBe32(Stream, Buffer[i]);
        if (Rval != FORMAT_OK) {
            break;
        }
    }
    return Rval;
}

/**
 * ISO/IEC 14496-3 Table 1.16 Sampling Frequency Index
 */
static UINT32 IsoMux_AAC_Idx(UINT32 Freq)
{
    UINT32 Rval = 0xFFU;
    if (Freq == 48000U) {
        Rval = 3U;
    } else if (Freq == 44100U) {
        Rval = 4U;
    } else if (Freq == 32000U) {
        Rval = 5U;
    } else if (Freq == 24000U) {
        Rval = 6U;
    } else if (Freq == 22050U) {
        Rval = 7U;
    } else if (Freq == 16000U) {
        Rval = 8U;
    } else if (Freq == 12000U) {
        Rval = 9U;
    } else if (Freq == 11025U) {
        Rval = 10U;
    } else if (Freq == 8000U) {
        Rval = 11U;
    } else {
        /* Do nothing */
    }
    return Rval;
}

/**
 * Generate AAC decoder information
 */
static UINT32 IsoMux_AAC_Info(UINT32 Samples, UINT32 Channel)
{
    UINT32 Info = 0x1000;
    UINT32 Sample = IsoMux_AAC_Idx(Samples);
    Info |= (Sample << 7);
    Info |= (Channel << 3);
    return Info;
}

#if 0
static UINT32 IsoMux_PASP_Write(const ISO_STSD_VIDEO_BOX_s *Stsd, SVC_STREAM_HDLR_s *Stream)
{
    UINT32 Rval;
    /* VideoEntry.Pasp.Box.Size */
    Rval = SvcFormat_PutBe32(Stream, Stsd->VideoEntry.PaspEntry.Box.Size);
    if (Rval == FORMAT_OK) {
        /* VideoEntry.Pasp.Box.Type */
        Rval = SvcFormat_PutBe32(Stream, Stsd->VideoEntry.PaspEntry.Box.Type);
        if (Rval == FORMAT_OK) {
            /* VideoEntry.Pasp.HSpacing */
            Rval = SvcFormat_PutBe32(Stream, Stsd->VideoEntry.PaspEntry.HSpacing);
            if (Rval == FORMAT_OK) {
                /* VideoEntry.Pasp.VSpacing */
                Rval = SvcFormat_PutBe32(Stream, Stsd->VideoEntry.PaspEntry.VSpacing);
            }
        }
    }
    return Rval;
}

static UINT32 IsoMux_PASP_Init(ISO_STSD_VIDEO_BOX_s *Stsd,  const SVC_VIDEO_TRACK_INFO_s *Video)
{
    Stsd->VideoEntry.PaspEntry.Box.Size = 16;
    Stsd->VideoEntry.PaspEntry.Box.Type = TAG_PASP;

    /* The default pixel AR is 1 1. */
    if ((Video->PixelArX == 0U) && (Video->PixelArY == 0U)) {
        Stsd->VideoEntry.PaspEntry.HSpacing = 1;
        Stsd->VideoEntry.PaspEntry.VSpacing = 1;
    } else {
        Stsd->VideoEntry.PaspEntry.HSpacing = Video->PixelArX;
        Stsd->VideoEntry.PaspEntry.VSpacing = Video->PixelArY;
    }

    Stsd->VideoEntry.Box.Size += Stsd->VideoEntry.PaspEntry.Box.Size;
    Stsd->FullBox.Box.Size += Stsd->VideoEntry.PaspEntry.Box.Size;

    return FORMAT_OK;
}
#endif

static UINT32 IsoMux_AvccHeader_Write(const ISO_STSD_VIDEO_BOX_s *Stsd, SVC_STREAM_HDLR_s *Stream)
{
    UINT32 Rval;
    /* sps */
    /* VideoEntry.Avcc.SPSNum */
    Rval = SvcFormat_PutByte(Stream, Stsd->VideoEntry.DecCfg.Avcc.SPSNum);
    if (Rval == FORMAT_OK) {
        /* VideoEntry.Avcc.SPSLength */
        Rval = SvcFormat_PutBe16(Stream, Stsd->VideoEntry.DecCfg.Avcc.SPSLength);
    }
    if (Rval == FORMAT_OK) {
        Rval = IsoMux_PutBuffer8(Stream, Stsd->VideoEntry.DecCfg.Avcc.SPSCtx, Stsd->VideoEntry.DecCfg.Avcc.SPSLength);
    }
    /* pps */
    if (Rval == FORMAT_OK) {
        /* VideoEntry.Avcc.PPSNum */
        Rval = SvcFormat_PutByte(Stream, Stsd->VideoEntry.DecCfg.Avcc.PPSNum);
    }
    if (Rval == FORMAT_OK) {
        /* VideoEntry.Avcc.PPSLength */
        Rval = SvcFormat_PutBe16(Stream, Stsd->VideoEntry.DecCfg.Avcc.PPSLength);
    }
    if (Rval == FORMAT_OK) {
        Rval = IsoMux_PutBuffer8(Stream, Stsd->VideoEntry.DecCfg.Avcc.PPSCtx, Stsd->VideoEntry.DecCfg.Avcc.PPSLength);
    }
    return Rval;
}

static UINT32 IsoMux_VideoAVCC_Write(const ISO_STSD_VIDEO_BOX_s *Stsd, SVC_STREAM_HDLR_s *Stream)
{
    UINT32 Rval;
    /**< Put Avcc */
    /* VideoEntry.Avcc.Box.Size */
    Rval = SvcFormat_PutBe32(Stream, Stsd->VideoEntry.DecCfg.Avcc.Box.Size);
    if (Rval == FORMAT_OK) {
        /* VideoEntry.Avcc.Box.Type */
        Rval = SvcFormat_PutBe32(Stream, Stsd->VideoEntry.DecCfg.Avcc.Box.Type);
    }
    if (Rval == FORMAT_OK) {
        /* Avcc extension within AVC1 */
        /* VideoEntry.Avcc.ConfigVersion */
        Rval = SvcFormat_PutByte(Stream, Stsd->VideoEntry.DecCfg.Avcc.ConfigVersion);
    }
    if (Rval == FORMAT_OK) {
        /* VideoEntry.Avcc.AVCProfileIndication */
        Rval = SvcFormat_PutByte(Stream, Stsd->VideoEntry.DecCfg.Avcc.SPSCtx[1]);
    }
    if (Rval == FORMAT_OK) {
        /* VideoEntry.Avcc.ProfileCompatibility */
        Rval = SvcFormat_PutByte(Stream, Stsd->VideoEntry.DecCfg.Avcc.SPSCtx[2]);
    }
    if (Rval == FORMAT_OK) {
        /* VideoEntry.Avcc.AVCLevelIndication */
        Rval = SvcFormat_PutByte(Stream, Stsd->VideoEntry.DecCfg.Avcc.SPSCtx[3]);
    }
    if (Rval == FORMAT_OK) {
        /* VideoEntry.Avcc.NALULength */
        Rval = SvcFormat_PutByte(Stream, Stsd->VideoEntry.DecCfg.Avcc.NALULength);
    }
    if (Rval == FORMAT_OK) {
        Rval = IsoMux_AvccHeader_Write(Stsd, Stream);
    }
    return Rval;
}
static UINT32 IsoMux_HvccHeader_Write(const ISO_STSD_VIDEO_BOX_s *Stsd, SVC_STREAM_HDLR_s *Stream)
{
    /*vps*/
    UINT32 Rval = SvcFormat_PutByte(Stream, 0xA0);
    if (Rval == FORMAT_OK) {
        /* VideoEntry.Avcc.SPSNum */
        Rval = SvcFormat_PutBe16(Stream, Stsd->VideoEntry.DecCfg.Hvcc.VPSNum);
    }
    if (Rval == FORMAT_OK) {
        /* VideoEntry.Avcc.SPSLength */
        Rval = SvcFormat_PutBe16(Stream, Stsd->VideoEntry.DecCfg.Hvcc.VPSLength);
    }
    if (Rval == FORMAT_OK) {
        Rval = IsoMux_PutBuffer8(Stream, Stsd->VideoEntry.DecCfg.Hvcc.VPSCtx, (UINT32)Stsd->VideoEntry.DecCfg.Hvcc.VPSLength);
    }
    /*sps*/
    if (Rval == FORMAT_OK) {
        Rval = SvcFormat_PutByte(Stream, 0xA1);
    }
    if (Rval == FORMAT_OK) {
        /* VideoEntry.Avcc.SPSNum */
        Rval = SvcFormat_PutBe16(Stream, Stsd->VideoEntry.DecCfg.Hvcc.SPSNum);
    }
    if (Rval == FORMAT_OK) {
        /* VideoEntry.Avcc.SPSLength */
        Rval = SvcFormat_PutBe16(Stream, Stsd->VideoEntry.DecCfg.Hvcc.SPSLength);
    }
    if (Rval == FORMAT_OK) {
        Rval = IsoMux_PutBuffer8(Stream, Stsd->VideoEntry.DecCfg.Hvcc.SPSCtx, (UINT32)Stsd->VideoEntry.DecCfg.Hvcc.SPSLength);
    }
    /*pps*/
    if (Rval == FORMAT_OK) {
        Rval = SvcFormat_PutByte(Stream, 0xA2);
    }
    if (Rval == FORMAT_OK) {
        /* VideoEntry.Avcc.PPSNum */
        Rval = SvcFormat_PutBe16(Stream, Stsd->VideoEntry.DecCfg.Hvcc.PPSNum);
    }
    if (Rval == FORMAT_OK) {
        /* VideoEntry.Avcc.PPSLength */
        Rval = SvcFormat_PutBe16(Stream, Stsd->VideoEntry.DecCfg.Hvcc.PPSLength);
    }
    if (Rval == FORMAT_OK) {
        Rval = IsoMux_PutBuffer8(Stream, Stsd->VideoEntry.DecCfg.Hvcc.PPSCtx, (UINT32)Stsd->VideoEntry.DecCfg.Hvcc.PPSLength);
    }
    return Rval;
}

static UINT32 IsoMux_VideoHVCC_Write(const ISO_STSD_VIDEO_BOX_s *Stsd, SVC_STREAM_HDLR_s *Stream)
{
    UINT32 Rval;
    UINT8 Tmp8;
    UINT16 Tmp16;
    /* Put Hvcc */
    /* VideoEntry.Hvcc.Box.Size */
    Rval = SvcFormat_PutBe32(Stream, Stsd->VideoEntry.DecCfg.Hvcc.Box.Size);
    if (Rval == FORMAT_OK) {
        /* VideoEntry.Hvcc.Box.Type */
        Rval = SvcFormat_PutBe32(Stream, Stsd->VideoEntry.DecCfg.Hvcc.Box.Type);
    }
    /* Hvcc extension within HVC1 */
    /* unsigned int(8) configurationVersion = 1; */
    if (Rval == FORMAT_OK) {
        Rval = SvcFormat_PutByte(Stream, Stsd->VideoEntry.DecCfg.Hvcc.ConfigVersion);
    }
    /* generalconfiguration 12 byte*/
    if (Rval == FORMAT_OK) {
        Rval = IsoMux_PutBuffer8(Stream, Stsd->VideoEntry.DecCfg.Hvcc.Generalconfiguration, SVC_FORMAT_MAX_VPS_GEN_CFG_LENGTH);
    }
    /* bit(4) reserved = 1111; unsigned int(12) min_spatial_segmentation_idc = 0; */
    if (Rval == FORMAT_OK) {
        Tmp16 = 0xf000U;
        Tmp16 |= 0U;
        Rval = SvcFormat_PutBe16(Stream, Tmp16);
    }
    /* bit(6) reserved = 111111; unsigned int(2) parallelismType = 0; */
    if (Rval == FORMAT_OK) {
        Tmp8 = 0xfcU;
        Tmp8 |= 0U;
        Rval = SvcFormat_PutByte(Stream, Tmp8);
    }
    /* bit(6) reserved = 111111; unsigned int(2) chromaFormat;*/
    if (Rval == FORMAT_OK) {
        Tmp8 = 0xfcU;
        Tmp8 |= Stsd->VideoEntry.DecCfg.Hvcc.ChromaFormat;
        Rval = SvcFormat_PutByte(Stream, Tmp8);
    }
    /* bit(5) reserved = 11111; unsigned int(3) bitDepthLumaMinus8; */
    if (Rval == FORMAT_OK) {
        Tmp8 = 0xf8U;
        Tmp8 |= Stsd->VideoEntry.DecCfg.Hvcc.BitDepthLumaMinus8;
        Rval = SvcFormat_PutByte(Stream, Tmp8);
    }
    /* bit(5) reserved = 11111; unsigned int(3) bitDepthChromaMinus8; */
    if (Rval == FORMAT_OK) {
        Tmp8 = 0xf8U;
        Tmp8 |= Stsd->VideoEntry.DecCfg.Hvcc.BitDepthChromaMinus8;
        Rval = SvcFormat_PutByte(Stream, Tmp8);
    }
    /* bit(16) avgFrameRate = 0; */
    if (Rval == FORMAT_OK) {
        Rval = SvcFormat_PutBe16(Stream, 0);
    }
    /*
     * bit(2) constantFrameRate;
     * bit(3) numTemporalLayers = MAX(hvcc->numTemporalLayers, vps/sps_max_sub_layers_minus1 + 1);
     * bit(1) temporalIdNested;
     * unsigned int(2) lengthSizeMinusOne = 3;
     */
    if (Rval == FORMAT_OK) {
        Rval = SvcFormat_PutByte(Stream, (((Stsd->VideoEntry.DecCfg.Hvcc.NumTemporalLayers & 0x07U) << 3U) |
                                    (Stsd->VideoEntry.DecCfg.Hvcc.TemporalIdNested << 2U) | 0x03U));
    }
    /* unsigned int(8) numOfArrays; */
    if (Rval == FORMAT_OK) {
        Rval = SvcFormat_PutByte(Stream, Stsd->VideoEntry.DecCfg.Hvcc.NumofArray);
    }
    if (Rval == FORMAT_OK) {
        Rval = IsoMux_HvccHeader_Write(Stsd, Stream);
    }
    return Rval;
}

static UINT32 IsoMux_VideoSampleCoding_Write(const ISO_STSD_VIDEO_BOX_s *Stsd, SVC_STREAM_HDLR_s *Stream, const SVC_VIDEO_TRACK_INFO_s *Track)
{
    UINT32 Rval;
    if (Track->Info.MediaId == SVC_FORMAT_MID_AVC) {
        /* Put Avc1 */
        Rval = IsoMux_VideoAVCC_Write(Stsd, Stream);
    } else if (Track->Info.MediaId == SVC_FORMAT_MID_HVC) {
        /* Put hvc1 */
        Rval = IsoMux_VideoHVCC_Write(Stsd, Stream);
    } else {
        AmbaPrint_ModulePrintStr5(SVC_MUXER_PRINT_MODULE_ID, "%s Not supported!", __func__, NULL, NULL, NULL, NULL);
        Rval = FORMAT_ERR_INVALID_ARG;
    }
    return Rval;
}

static UINT32 IsoMux_VideoSampleEntry_Write(ISO_STSD_VIDEO_BOX_s *Stsd, SVC_STREAM_HDLR_s *Stream, const SVC_VIDEO_TRACK_INFO_s *Track)
{
    /* VideoEntry.Box.Size */
    UINT32 Rval = SvcFormat_PutBe32(Stream, Stsd->VideoEntry.Box.Size);
    if (Rval == FORMAT_OK) {
        /* VideoEntry.Box.Type */
        Rval = SvcFormat_PutBe32(Stream, Stsd->VideoEntry.Box.Type);
    }
    if (Rval == FORMAT_OK) {
        /* VideoEntry.Reserv6 */
        Rval = IsoMux_PutBuffer8(Stream, Stsd->VideoEntry.Reserv6, 6U);
    }
    if (Rval == FORMAT_OK) {
        /* VideoEntry.DataRefIdx */
        Rval = SvcFormat_PutBe16(Stream, Stsd->VideoEntry.DataRefIdx);
    }
    if (Rval == FORMAT_OK) {
        /* VideoEntry.Version */
        Rval = SvcFormat_PutBe16(Stream, Stsd->VideoEntry.Version);
    }
    if (Rval == FORMAT_OK) {
        /* VideoEntry.Revison */
        Rval = SvcFormat_PutBe16(Stream, Stsd->VideoEntry.Revison);
    }
    if (Rval == FORMAT_OK) {
        /* VideoEntry.Predefined */
        Rval = IsoMux_PutBufferBe32(Stream, Stsd->VideoEntry.Predefined, 3U);
    }
    if (Rval == FORMAT_OK) {
        /* VideoEntry.Width */
        Rval = SvcFormat_PutBe16(Stream, Stsd->VideoEntry.Width);
    }
    if (Rval == FORMAT_OK) {
        /* VideoEntry.Height */
        Rval = SvcFormat_PutBe16(Stream, Stsd->VideoEntry.Height);
    }
    if (Rval == FORMAT_OK) {
        /* VideoEntry.HorRes */
        Rval = SvcFormat_PutBe32(Stream, Stsd->VideoEntry.HorRes);
    }
    if (Rval == FORMAT_OK) {
        /* VideoEntry.VerRes */
        Rval = SvcFormat_PutBe32(Stream, Stsd->VideoEntry.VerRes);
    }
    if (Rval == FORMAT_OK) {
        /* VideoEntry.Reserv4 */
        Rval = IsoMux_PutBufferBe32(Stream, &Stsd->VideoEntry.Reserv4, 1U);
    }
    if (Rval == FORMAT_OK) {
        /* VideoEntry.FrameCount */
        Rval = SvcFormat_PutBe16(Stream, Stsd->VideoEntry.FrameCount);
    }
    /* VideoEntry.Name */
    if (Rval == FORMAT_OK) {
        UINT32 count;
        Rval = S2F(Stream->Func->Write(Stream, ISO_STSD_MAX_ENC_NAME_LEN, (UINT8 *)Stsd->VideoEntry.Name, &count));
        if (Rval == FORMAT_OK) {
            if (count < ISO_STSD_MAX_ENC_NAME_LEN) {
                AmbaPrint_ModulePrintStr5(SVC_MUXER_PRINT_MODULE_ID, "%s Stream->Func->Write() failed!", __func__, NULL, NULL, NULL, NULL);
                Rval = FORMAT_ERR_IO_ERROR;
            }
        } else {
            AmbaPrint_ModulePrintStr5(SVC_MUXER_PRINT_MODULE_ID, "%s Stream->Func->Write() failed!", __func__, NULL, NULL, NULL, NULL);
        }
    }
    if (Rval == FORMAT_OK) {
        /* VideoEntry.Depth */
        Rval = SvcFormat_PutBe16(Stream, Stsd->VideoEntry.Depth);
    }
    if (Rval == FORMAT_OK) {
        /* VideoEntry.Color */
        Rval = SvcFormat_PutBe16(Stream, Stsd->VideoEntry.Color);
    }
    if (Rval == FORMAT_OK) {
        Rval = IsoMux_VideoSampleCoding_Write(Stsd, Stream, Track);
    }
    return Rval;
}

static UINT32 IsoMux_VideoSTSD_Write(ISO_STSD_VIDEO_BOX_s *Stsd, SVC_STREAM_HDLR_s *Stream, const SVC_VIDEO_TRACK_INFO_s *Track)
{
    UINT32 Rval;
    /* Put Stsd */
    /* Stsd.FullBox.Box.Size */
    Rval = SvcFormat_PutBe32(Stream, Stsd->FullBox.Box.Size);
    if (Rval == FORMAT_OK) {
        /* Stsd.FullBox.Box.Type */
        Rval = SvcFormat_PutBe32(Stream, Stsd->FullBox.Box.Type);
    }
    if (Rval == FORMAT_OK) {
        /* Stsd.FullBox.Version */
        Rval = SvcFormat_PutBe32(Stream, 0);
    }
    if (Rval == FORMAT_OK) {
        /* Stsd.EntryCount */
        Rval = SvcFormat_PutBe32(Stream, Stsd->EntryCount);
    }
    if (Rval == FORMAT_OK) {
        Rval = IsoMux_VideoSampleEntry_Write(Stsd, Stream, Track);
    }
#if 0
    /**< Put PASP */
    if (Rval == FORMAT_OK) {
        Rval = IsoMux_PASP_Write(Stsd, Stream);
    }
#endif
    return Rval;
}

static UINT32 IsoMux_VideoAVCC_Init(ISO_STSD_VIDEO_BOX_s *Stsd, const SVC_VIDEO_TRACK_INFO_s *Video, const SVC_ISO_VIDEO_TRACK_INFO_s *VideoInfo)
{
    UINT32 Rval = FORMAT_OK;
    const char EncName[ISO_STSD_MAX_ENC_NAME_LEN] = {
    (char)21, 'A', 'm', 'b', 'a', 'r', 'e', 'l', 'l', 'a', ' ', 'A', 'V',
    'C', ' ', 'e', 'n', 'c', 'o', 'd', 'e', 'r', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0'};

    Stsd->VideoEntry.Box.Size = 54;
    Stsd->VideoEntry.Box.Type = TAG_AVC1;
    Stsd->VideoEntry.DataRefIdx = 1;
    Stsd->VideoEntry.Width = Video->Width;
    Stsd->VideoEntry.Height = Video->Height;
    if ((Video->PixelArX != 0U) && (Video->PixelArY != 0U)) {
        if (((Stsd->VideoEntry.Width * Video->PixelArY) % Video->PixelArX) == 0U) {
            Rval = FORMAT_ERR_INVALID_ARG;
        } else {
            Stsd->VideoEntry.Width = (Stsd->VideoEntry.Width * Video->PixelArY) / Video->PixelArX;
        }
    }

    if (Rval == FORMAT_OK) {
    //    LOG_INFO("Width = %u, Height = %u, PixelArX = %u, PixelArY = %u",Video->Width, Video->Height, Video->PixelArX, Video->PixelArY);
        Stsd->VideoEntry.HorRes = 0x00480000;
        Stsd->VideoEntry.VerRes = 0x00480000;
        Stsd->VideoEntry.FrameCount = 1;
        AmbaUtility_StringCopy(Stsd->VideoEntry.Name, ISO_STSD_MAX_ENC_NAME_LEN, EncName);   // no need to ensure null terminated
        Stsd->VideoEntry.Depth = 0x0018;
        Stsd->VideoEntry.Color = 0xffff;

        Stsd->VideoEntry.DecCfg.Avcc.Box.Size = 19;
        Stsd->VideoEntry.DecCfg.Avcc.Box.Type = TAG_AVCC;
        Stsd->VideoEntry.DecCfg.Avcc.ConfigVersion = 1;
        Stsd->VideoEntry.DecCfg.Avcc.NALULength = 0xFF;
        Stsd->VideoEntry.DecCfg.Avcc.SPSNum = 0xE1;
        Stsd->VideoEntry.DecCfg.Avcc.SPSLength = VideoInfo->Avc.SPSLen;
        Rval = W2F(AmbaWrap_memcpy(Stsd->VideoEntry.DecCfg.Avcc.SPSCtx, VideoInfo->Avc.SPS, sizeof(Stsd->VideoEntry.DecCfg.Avcc.SPSCtx)));
        if (Rval == FORMAT_OK) {
            Stsd->VideoEntry.DecCfg.Avcc.PPSNum = 1;
            Stsd->VideoEntry.DecCfg.Avcc.PPSLength = VideoInfo->Avc.PPSLen;
            Rval = W2F(AmbaWrap_memcpy(Stsd->VideoEntry.DecCfg.Avcc.PPSCtx, VideoInfo->Avc.PPS, sizeof(Stsd->VideoEntry.DecCfg.Avcc.PPSCtx)));
        }
        if (Rval == FORMAT_OK) {
            UINT16 Tmp16;
            Tmp16 = Stsd->VideoEntry.DecCfg.Avcc.SPSLength + Stsd->VideoEntry.DecCfg.Avcc.PPSLength;
            Stsd->VideoEntry.DecCfg.Avcc.Box.Size += (UINT32)Tmp16;
            Stsd->VideoEntry.Box.Size +=  (ISO_STSD_MAX_ENC_NAME_LEN + Stsd->VideoEntry.DecCfg.Avcc.Box.Size);
            Stsd->FullBox.Box.Size += Stsd->VideoEntry.Box.Size;
        }
    }
    return Rval;
}

static UINT32 IsoMux_VideoHVCC_Init(ISO_STSD_VIDEO_BOX_s *Stsd, const SVC_VIDEO_TRACK_INFO_s *Video, const SVC_ISO_VIDEO_TRACK_INFO_s *VideoInfo)
{
    UINT32 Rval = FORMAT_OK;
    const char EncName[ISO_STSD_MAX_ENC_NAME_LEN] = {
    (char)21, 'A', 'm', 'b', 'a', 'r', 'e', 'l', 'l', 'a', ' ', 'H', 'E', 'V',
    'C', ' ', 'e', 'n', 'c', 'o', 'd', 'e', 'r', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0'};

    Stsd->VideoEntry.Box.Size = 54;
    Stsd->VideoEntry.Box.Type = TAG_HVC1;
    Stsd->VideoEntry.DataRefIdx = 1;
    Stsd->VideoEntry.Width = Video->Width;
    Stsd->VideoEntry.Height = Video->Height;
    if ((Video->PixelArX != 0U) && (Video->PixelArY != 0U)) {
        if (((Stsd->VideoEntry.Width * Video->PixelArY) % Video->PixelArX) == 0U) {
            Rval = FORMAT_ERR_INVALID_ARG;
        } else {
            Stsd->VideoEntry.Width = (Stsd->VideoEntry.Width * Video->PixelArY) / Video->PixelArX;
        }
    }
    if (Rval == FORMAT_OK) {
    //    LOG_INFO("Width = %u, Height = %u, PixelArX = %u, PixelArY = %u",Video->Width, Video->Height, Video->PixelArX, Video->PixelArY);
        Stsd->VideoEntry.HorRes = 0x00480000;
        Stsd->VideoEntry.VerRes = 0x00480000;
        Stsd->VideoEntry.FrameCount = 1;
        AmbaUtility_StringCopy(Stsd->VideoEntry.Name, ISO_STSD_MAX_ENC_NAME_LEN, EncName);   // no need to ensure null terminated
        Stsd->VideoEntry.Depth = 0x0018;
        Stsd->VideoEntry.Color = 0xffff;

        Stsd->VideoEntry.DecCfg.Hvcc.Box.Size = 46;
        Stsd->VideoEntry.DecCfg.Hvcc.Box.Type = TAG_HVCC;
        Stsd->VideoEntry.DecCfg.Hvcc.ConfigVersion = 1;
        Rval = W2F(AmbaWrap_memcpy(Stsd->VideoEntry.DecCfg.Hvcc.Generalconfiguration, &VideoInfo->Hvc, SVC_FORMAT_MAX_VPS_GEN_CFG_LENGTH));
        if (Rval == FORMAT_OK) {
            Stsd->VideoEntry.DecCfg.Hvcc.ChromaFormat = VideoInfo->Hvc.ChromaFormat;
            Stsd->VideoEntry.DecCfg.Hvcc.BitDepthLumaMinus8 = VideoInfo->Hvc.BitDepthLumaMinus8;
            Stsd->VideoEntry.DecCfg.Hvcc.BitDepthChromaMinus8 = VideoInfo->Hvc.BitDepthChromaMinus8;
            Stsd->VideoEntry.DecCfg.Hvcc.NumTemporalLayers = VideoInfo->Hvc.NumTemporalLayers;
            Stsd->VideoEntry.DecCfg.Hvcc.TemporalIdNested = VideoInfo->Hvc.TemporalIdNested;
            Stsd->VideoEntry.DecCfg.Hvcc.NumofArray = 3; /* vps + sps + pps*/
            Stsd->VideoEntry.DecCfg.Hvcc.VPSNum = 1;
            Stsd->VideoEntry.DecCfg.Hvcc.VPSLength = VideoInfo->Hvc.VPSLen;
            Rval = W2F(AmbaWrap_memcpy(Stsd->VideoEntry.DecCfg.Hvcc.VPSCtx, VideoInfo->Hvc.VPS, sizeof(Stsd->VideoEntry.DecCfg.Hvcc.VPSCtx)));
        }
        if (Rval == FORMAT_OK) {
            Stsd->VideoEntry.DecCfg.Hvcc.SPSNum = 1;
            Stsd->VideoEntry.DecCfg.Hvcc.SPSLength = VideoInfo->Hvc.SPSLen;
            Rval = W2F(AmbaWrap_memcpy(Stsd->VideoEntry.DecCfg.Hvcc.SPSCtx, VideoInfo->Hvc.SPS, sizeof(Stsd->VideoEntry.DecCfg.Hvcc.SPSCtx)));
        }
        if (Rval == FORMAT_OK) {
            Stsd->VideoEntry.DecCfg.Hvcc.PPSNum = 1;
            Stsd->VideoEntry.DecCfg.Hvcc.PPSLength = VideoInfo->Hvc.PPSLen;
            Rval = W2F(AmbaWrap_memcpy(Stsd->VideoEntry.DecCfg.Hvcc.PPSCtx, VideoInfo->Hvc.PPS, sizeof(Stsd->VideoEntry.DecCfg.Hvcc.PPSCtx)));
        }
        if (Rval == FORMAT_OK) {
            UINT16 Tmp16;
            Tmp16 = Stsd->VideoEntry.DecCfg.Hvcc.VPSLength + Stsd->VideoEntry.DecCfg.Hvcc.SPSLength + Stsd->VideoEntry.DecCfg.Hvcc.PPSLength;
            Stsd->VideoEntry.DecCfg.Hvcc.Box.Size += (UINT32)Tmp16;
            Stsd->VideoEntry.Box.Size +=  (ISO_STSD_MAX_ENC_NAME_LEN + Stsd->VideoEntry.DecCfg.Hvcc.Box.Size);
            Stsd->FullBox.Box.Size += Stsd->VideoEntry.Box.Size;
        }
    }

    return Rval;
}

static UINT32 IsoMux_VideoSTSD_Init(ISO_STSD_VIDEO_BOX_s *Stsd, const SVC_VIDEO_TRACK_INFO_s *Video, const SVC_ISO_VIDEO_TRACK_INFO_s *VideoInfo, UINT32 MediaId)
{
    UINT32 Rval;
    Stsd->FullBox.Box.Size = 16;
    Stsd->FullBox.Box.Type = TAG_STSD;
    Stsd->EntryCount = 1;
    // TODO: multi entry;
    if (MediaId == SVC_FORMAT_MID_AVC) {
        Rval = IsoMux_VideoAVCC_Init(Stsd,  Video, VideoInfo);
    } else if(MediaId == SVC_FORMAT_MID_HVC) {
        Rval = IsoMux_VideoHVCC_Init(Stsd,  Video, VideoInfo);
    } else {
        AmbaPrint_ModulePrintStr5(SVC_MUXER_PRINT_MODULE_ID, "%s Not supported!", __func__, NULL, NULL, NULL, NULL);
        Rval = FORMAT_ERR_INVALID_ARG;
    }
#if 0
    if (Rval == FORMAT_OK) {
        Rval = IsoMux_PASP_Init(Stsd, Video);
    }
#endif
    return Rval;
}

static UINT32 IsoMux_AudioESDS_AACDecCfg(const ISO_STSD_AUDIO_BOX_s *Stsd, SVC_STREAM_HDLR_s *Stream)
{
    UINT32 Rval = SvcFormat_PutByte(Stream, 4);
    if (Rval == FORMAT_OK) {
        Rval = SvcFormat_PutBe16(Stream, 0x8080);
    }
    if (Rval == FORMAT_OK) {
        Rval = SvcFormat_PutByte(Stream, 22);
    }
    if (Rval == FORMAT_OK) {
        Rval = SvcFormat_PutByte(Stream, 0x40);
        // AAC
    }
    if (Rval == FORMAT_OK) {
        Rval = SvcFormat_PutByte(Stream, 0x15);
        // Audio Stream
    }
    if (Rval == FORMAT_OK) {
        Rval = SvcFormat_PutByte(Stream, 0);
    }
    if (Rval == FORMAT_OK) {
        Rval = SvcFormat_PutBe16(Stream, 8192);
        // buffer size
    }
    if (Rval == FORMAT_OK) {
        Rval = SvcFormat_PutBe32(Stream, Stsd->AudioEntry.Esds.MaxBrate);
        // max bitrate
    }
    if (Rval == FORMAT_OK) {
        Rval = SvcFormat_PutBe32(Stream, Stsd->AudioEntry.Esds.AvgBrate);
        // avg bitrate
    }
    if (Rval == FORMAT_OK) {
        //Decoder specific info descriptor takes 9 bytes
        Rval = SvcFormat_PutByte(Stream, 5);
    }
    if (Rval == FORMAT_OK) {
        Rval = SvcFormat_PutBe16(Stream, 0x8080);
    }
    if (Rval == FORMAT_OK) {
        Rval = SvcFormat_PutByte(Stream, 5);
    }
    if (Rval == FORMAT_OK) {
        Rval = SvcFormat_PutBe16(Stream, Stsd->AudioEntry.Esds.AACinfo);
    }
    if (Rval == FORMAT_OK) {
        Rval = SvcFormat_PutBe16(Stream, 0x0000);
    }
    if (Rval == FORMAT_OK) {
        Rval = SvcFormat_PutByte(Stream, 0x00);
    }
    return Rval;
}

static UINT32 IsoMux_AudioESDS_AAC(const ISO_STSD_AUDIO_BOX_s *Stsd, SVC_STREAM_HDLR_s *Stream)
{
    UINT32 Rval = SvcFormat_PutBe32(Stream, Stsd->AudioEntry.Esds.Box.Size);
    if (Rval == FORMAT_OK) {
        Rval = SvcFormat_PutBe32(Stream, Stsd->AudioEntry.Esds.Box.Type);
        //esds, compatible to part 14 - MOV
    }
    if (Rval == FORMAT_OK) {
        Rval = SvcFormat_PutBe32(Stream, 0);
    }
    if (Rval == FORMAT_OK) {
        /* ES descriptor takes 38 bytes */
        Rval = SvcFormat_PutByte(Stream, 3);
    }
    if (Rval == FORMAT_OK) {
        Rval = SvcFormat_PutBe16(Stream, 0x8080);
    }
    if (Rval == FORMAT_OK) {
        Rval = SvcFormat_PutByte(Stream, 34);
    }
    if (Rval == FORMAT_OK) {
        Rval = SvcFormat_PutBe16(Stream, 0);
    }
    if (Rval == FORMAT_OK) {
        Rval = SvcFormat_PutByte(Stream, 0);
    }
    if (Rval == FORMAT_OK) {
        //Decoder config descriptor takes 26 bytes (include decoder specific info)
        Rval = IsoMux_AudioESDS_AACDecCfg(Stsd, Stream);
    }
    if (Rval == FORMAT_OK) {
        //SL descriptor takes 5 bytes
        Rval = SvcFormat_PutByte(Stream, 6);
    }
    if (Rval == FORMAT_OK) {
        Rval = SvcFormat_PutBe16(Stream, 0x8080);
    }
    if (Rval == FORMAT_OK) {
        Rval = SvcFormat_PutByte(Stream, 1);
    }
    if (Rval == FORMAT_OK) {
        Rval = SvcFormat_PutByte(Stream, 2);
    }
    return Rval;
}

static UINT32 IsoMux_AudioSTSD_AAC(const ISO_STSD_AUDIO_BOX_s *Stsd, SVC_STREAM_HDLR_s *Stream)
{
    UINT32 Rval;
    Rval = SvcFormat_PutBe16(Stream, Stsd->AudioEntry.Channels);
    if (Rval == FORMAT_OK) {
        Rval = SvcFormat_PutBe16(Stream, Stsd->AudioEntry.BitsPerSample);
    }
    if (Rval == FORMAT_OK) {
        Rval = SvcFormat_PutBe16(Stream, 0xfffe);
        // pre_defined, for QT sound.
    }
    if (Rval == FORMAT_OK) {
        Rval = SvcFormat_PutBe16(Stream, 0);
    }
    if (Rval == FORMAT_OK) {
        Rval = SvcFormat_PutBe32(Stream, Stsd->AudioEntry.SampleRate << 16);
        //sample rate
    }
    if (Rval == FORMAT_OK) {
        Rval = IsoMux_AudioESDS_AAC(Stsd, Stream);
    }
    return Rval;
}

static UINT32 IsoMux_AudioESDS_ADPCM(const ISO_STSD_AUDIO_BOX_s *Stsd, SVC_STREAM_HDLR_s *Stream, UINT32 SamplesPerBlock, UINT32 BlockAlign, UINT32 AvgBytesPerSec)
{
    /* ES descriptor takes 38 bytes */
    UINT32 Rval = SvcFormat_PutBe32(Stream, Stsd->AudioEntry.Esds.Box.Size);
        // wave size
    if (Rval == FORMAT_OK) {
        Rval = SvcFormat_PutBe32(Stream, Stsd->AudioEntry.Esds.Box.Type);
        // WAVE
    }
    if (Rval == FORMAT_OK) {
        Rval = SvcFormat_PutBe32(Stream, 12);
        // wave size
    }
    if (Rval == FORMAT_OK) {
        Rval = SvcFormat_PutBe32(Stream, TAG_FRMA);
    }
    if (Rval == FORMAT_OK) {
        Rval = SvcFormat_PutBe32(Stream, Stsd->AudioEntry.Box.Type);
    }
    if (Rval == FORMAT_OK) {
        Rval = SvcFormat_PutBe32(Stream, 28);
    }
    if (Rval == FORMAT_OK) {
        Rval = SvcFormat_PutBe32(Stream, Stsd->AudioEntry.Box.Type);
    }
    if (Rval == FORMAT_OK) {
        Rval = SvcFormat_PutLe16(Stream, 0x11);
        // wFormatTag, WAVE_FORMAT_DVI_ADPCM = 0x11
    }
    if (Rval == FORMAT_OK) {
        Rval = SvcFormat_PutLe16(Stream, Stsd->AudioEntry.Channels);
    }
    if (Rval == FORMAT_OK) {
        Rval = SvcFormat_PutLe32(Stream, Stsd->AudioEntry.SampleRate);
    }
    if (Rval == FORMAT_OK) {
        Rval = SvcFormat_PutLe32(Stream, AvgBytesPerSec);
        // AvgBytesPerSec
    }
    if (Rval == FORMAT_OK) {
        Rval = SvcFormat_PutLe16(Stream, (UINT16)BlockAlign);
        // BlockAlign, = BytePerBlock
    }
    if (Rval == FORMAT_OK) {
        Rval = SvcFormat_PutLe16(Stream, 4);
        // wBitsPerSample
    }
    if (Rval == FORMAT_OK) {
        Rval = SvcFormat_PutLe16(Stream, 2);
        // cbSize
    }
    if (Rval == FORMAT_OK) {
        Rval = SvcFormat_PutLe16(Stream, (UINT16)SamplesPerBlock);
    }
    if (Rval == FORMAT_OK) {
        Rval = SvcFormat_PutBe32(Stream, 8);
    }
    if (Rval == FORMAT_OK) {
        Rval = SvcFormat_PutBe32(Stream, 0);
    }
    return Rval;
}
static UINT32 IsoMux_AudioSTSD_ADPCM(const ISO_STSD_AUDIO_BOX_s *Stsd, SVC_STREAM_HDLR_s *Stream)
{
    UINT32 Rval;
    UINT32 SamplesPerBlock = 0, BlockAlign = 0, AvgBytesPerSec = 0;
    Rval = SvcFormat_PutBe16(Stream, Stsd->AudioEntry.Channels);
    //channel
    if (Rval == FORMAT_OK) {
        Rval = SvcFormat_PutBe16(Stream, Stsd->AudioEntry.BitsPerSample);
        // sample size
    }
    if (Rval == FORMAT_OK) {
        Rval = SvcFormat_PutBe16(Stream, 0xffff);
        // compression ID
    }
    if (Rval == FORMAT_OK) {
        Rval = SvcFormat_PutBe16(Stream, 0);
        // packet size
    }
    if (Rval == FORMAT_OK) {
        Rval = SvcFormat_PutBe32(Stream, Stsd->AudioEntry.SampleRate << 16);
        //sample rate
    }
    if (Rval == FORMAT_OK) {
        SamplesPerBlock = Stsd->AudioEntry.SamplesPerChunk;
        BlockAlign = (((SamplesPerBlock-1U)*4U*Stsd->AudioEntry.Channels)/8U) + (4U*(UINT32)Stsd->AudioEntry.Channels); // BytePerBlock
        AvgBytesPerSec = (Stsd->AudioEntry.SampleRate * BlockAlign) / SamplesPerBlock;

    //    LOG_INFO("SamplesPerBlock = %u, BlockAlign = %u, AvgBytesPerSec = %u", SamplesPerBlock, BlockAlign, AvgBytesPerSec);

        Rval = SvcFormat_PutBe32(Stream, SamplesPerBlock);
        // sample per packet
    }
    if (Rval == FORMAT_OK) {
        Rval = SvcFormat_PutBe32(Stream, BlockAlign);
        // Bytes per packet
    }
    if (Rval == FORMAT_OK) {
        Rval = SvcFormat_PutBe32(Stream, BlockAlign);
        // Bytes per frame
    }
    if (Rval == FORMAT_OK) {
        Rval = SvcFormat_PutBe32(Stream, 2);
        // Bytes per sample
    }
    if (Rval == FORMAT_OK) {
        Rval = IsoMux_AudioESDS_ADPCM(Stsd, Stream, SamplesPerBlock, BlockAlign, AvgBytesPerSec);
    }
    return Rval;
}

static UINT32 IsoMux_AudioSTSD_LPCM(const ISO_STSD_AUDIO_BOX_s *Stsd, SVC_STREAM_HDLR_s *Stream)
{
    UINT32 Rval = SvcFormat_PutBe32(Stream, 0x00030010);
    // reserved; always 3 and 16
    if (Rval == FORMAT_OK) {
        Rval = SvcFormat_PutBe32(Stream, 0xFFFE0000U);
        // always -2 and 0
    }
    if (Rval == FORMAT_OK) {
        Rval = SvcFormat_PutBe32(Stream, 65536);
        // always 65536
    }
    if (Rval == FORMAT_OK) {
        Rval = SvcFormat_PutBe32(Stream, Stsd->AudioEntry.Box.Size);
        // sizeof(SoundDescriptionV2)
    }
    if (Rval == FORMAT_OK) {
        DOUBLE Samples = (DOUBLE)Stsd->AudioEntry.SampleRate;
        UINT8 SampleBuf[8] = {0};
        Rval = W2F(AmbaWrap_memcpy(SampleBuf, &Samples, sizeof(SampleBuf)));
        if (Rval == FORMAT_OK) {
            INT32 i;
            for (i = 7; i >= 0; i--) {
                // audioSampleRate in 64-bit floating-point number
                Rval = SvcFormat_PutByte(Stream, SampleBuf[i]);
                if (Rval != FORMAT_OK) {
                    break;
                }
            }
        }
    }
    if (Rval == FORMAT_OK) {
        Rval = SvcFormat_PutBe32(Stream, Stsd->AudioEntry.Channels);
        // channel
    }
    if (Rval == FORMAT_OK) {
        Rval = SvcFormat_PutBe32(Stream, 0x7f000000);
        // always 0x7f000000
    }
    if (Rval == FORMAT_OK) {
        Rval = SvcFormat_PutBe32(Stream, Stsd->AudioEntry.BitsPerSample);
        // constBitsPerChannel
    }
    if (Rval == FORMAT_OK) {
        Rval = SvcFormat_PutBe32(Stream, 0xC);
        // format Specific flags
    }
    if (Rval == FORMAT_OK) {
        Rval = SvcFormat_PutBe32(Stream, ((UINT32)Stsd->AudioEntry.BitsPerSample>>3) * Stsd->AudioEntry.Channels);
        // constBytesPerAudioPacket
    }
    if (Rval == FORMAT_OK) {
        Rval = SvcFormat_PutBe32(Stream, 1);
        // constLPCMFramesPerAudioPacket
    }
    return Rval;
}

static UINT32 IsoMux_AudioSTSD_IN24(const ISO_STSD_AUDIO_BOX_s *Stsd, SVC_STREAM_HDLR_s *Stream)
{
    UINT32 Rval = SvcFormat_PutBe32(Stream, Stsd->AudioEntry.Esds.Box.Size);
    // wave size
    if (Rval == FORMAT_OK) {
        Rval = SvcFormat_PutBe32(Stream, Stsd->AudioEntry.Esds.Box.Type);
        // WAVE
    }
    if (Rval == FORMAT_OK) {
        Rval = SvcFormat_PutBe32(Stream, 1);
        // wave size
    }
    if (Rval == FORMAT_OK) {
        Rval = SvcFormat_PutBe32(Stream, TAG_FRMA);
    }
    if (Rval == FORMAT_OK) {
        Rval = SvcFormat_PutBe32(Stream, Stsd->AudioEntry.Box.Type);
    }
    if (Rval == FORMAT_OK) {
        /* wave information box. taking 10 bytes */
        Rval = SvcFormat_PutBe32(Stream, 10);
    }
    if (Rval == FORMAT_OK) {
        Rval = SvcFormat_PutBe32(Stream, TAG_ENDA);
    }
    if (Rval == FORMAT_OK) {
        Rval = SvcFormat_PutBe16(Stream, 0x01);
        // wFormatTag, PCM = 0x01
    }
    if (Rval == FORMAT_OK) {
        Rval = SvcFormat_PutBe32(Stream, 8);
        // termination
    }
    if (Rval == FORMAT_OK) {
        Rval = SvcFormat_PutBe32(Stream, 0);
        // termination
    }
    return Rval;
}

static UINT32 IsoMux_AudioSTSD_SOWT(const ISO_STSD_AUDIO_BOX_s *Stsd, SVC_STREAM_HDLR_s *Stream)
{
    UINT32 Rval = SvcFormat_PutBe16(Stream, Stsd->AudioEntry.Channels);
    //channel
    if (Rval == FORMAT_OK) {
        Rval = SvcFormat_PutBe16(Stream, Stsd->AudioEntry.BitsPerSample);
        // sample size
    }
    if (Rval == FORMAT_OK) {
        Rval = SvcFormat_PutBe16(Stream, 0xffff);
        // compression ID
    }
    if (Rval == FORMAT_OK) {
        Rval = SvcFormat_PutBe16(Stream, 0);
        // packet size
    }
    if (Rval == FORMAT_OK) {
        Rval = SvcFormat_PutBe32(Stream, Stsd->AudioEntry.SampleRate << 16);
        //sample rate
    }
    if (Rval == FORMAT_OK) {
        Rval = SvcFormat_PutBe32(Stream, 1);
        // sample per packet
    }
    if (Rval == FORMAT_OK) {
        Rval = SvcFormat_PutBe32(Stream, (UINT32)Stsd->AudioEntry.BitsPerSample>>3);
        // Bytes per packet
    }
    if (Rval == FORMAT_OK) {
        Rval = SvcFormat_PutBe32(Stream, ((UINT32)Stsd->AudioEntry.BitsPerSample>>3) * Stsd->AudioEntry.Channels);
        // Bytes per sample
    }
    if (Rval == FORMAT_OK) {
        Rval = SvcFormat_PutBe32(Stream, 2);
        // sample per sample?
    }
    if ((Stsd->AudioEntry.Box.Type == TAG_IN24) && (Rval == FORMAT_OK)) {
        Rval = IsoMux_AudioSTSD_IN24(Stsd, Stream);
    }
    return Rval;
}

static UINT32 IsoMux_AudioSTSD_PCM(const ISO_STSD_AUDIO_BOX_s *Stsd, SVC_STREAM_HDLR_s *Stream)
{
    UINT32 Rval = FORMAT_OK;
    if (Stsd->AudioEntry.Box.Type == TAG_LPCM) { /* LPCM */
        Rval = IsoMux_AudioSTSD_LPCM(Stsd, Stream);
    } else { /* SOWT TWOS IN24 */
        Rval = IsoMux_AudioSTSD_SOWT(Stsd, Stream);
    }
    return Rval;
}

static UINT32 IsoMux_AudioSTSD_Write(const ISO_STSD_AUDIO_BOX_s *Stsd, SVC_STREAM_HDLR_s *Stream, const SVC_AUDIO_TRACK_INFO_s *Track)
{
    UINT32 Rval = FORMAT_OK;
    /* Put Stsd */
    /* Stsd.FullBox.Box.Size */
    Rval = SvcFormat_PutBe32(Stream, Stsd->FullBox.Box.Size);
    if (Rval == FORMAT_OK) {
        /* Stsd.FullBox.Box.Type */
        Rval = SvcFormat_PutBe32(Stream, Stsd->FullBox.Box.Type);
    }
    if (Rval == FORMAT_OK) {
        /* Stsd.FullBox.Version, Flag */
        Rval = SvcFormat_PutBe32(Stream, 0);
    }
    if (Rval == FORMAT_OK) {
        /* Stsd.EntryCount */
        Rval = SvcFormat_PutBe32(Stream, Stsd->EntryCount);
    }
    if (Rval == FORMAT_OK) {
        Rval = SvcFormat_PutBe32(Stream, Stsd->AudioEntry.Box.Size);
    }
    if (Rval == FORMAT_OK) {
        Rval = SvcFormat_PutBe32(Stream, Stsd->AudioEntry.Box.Type);
        // mp4a, ms0011
    }
    if (Rval == FORMAT_OK) {
        Rval = SvcFormat_PutBe32(Stream, 0);
    }
    if (Rval == FORMAT_OK) {
        Rval = SvcFormat_PutBe16(Stream, 0);
    }
    if (Rval == FORMAT_OK) {
        Rval = SvcFormat_PutBe16(Stream, 1);
    }
    if (Rval == FORMAT_OK) {
        Rval = SvcFormat_PutBe16(Stream, Stsd->AudioEntry.Version);
    }
    if (Rval == FORMAT_OK) {
        Rval = SvcFormat_PutBe16(Stream, 0);
    }
    if (Rval == FORMAT_OK) {
        Rval = SvcFormat_PutBe32(Stream, 0);
    }

    if (Rval == FORMAT_OK) {
        if (Track->Info.MediaId == SVC_FORMAT_MID_AAC) {
            Rval = IsoMux_AudioSTSD_AAC(Stsd, Stream);
        } else if (Track->Info.MediaId == SVC_FORMAT_MID_ADPCM) {
            Rval = IsoMux_AudioSTSD_ADPCM(Stsd, Stream);
        } else if ((Track->Info.MediaId == SVC_FORMAT_MID_PCM) || (Track->Info.MediaId == SVC_FORMAT_MID_LPCM)) {
            Rval = IsoMux_AudioSTSD_PCM(Stsd, Stream);
        } else {
            AmbaPrint_ModulePrintStr5(SVC_MUXER_PRINT_MODULE_ID, "%s Not supported!", __func__, NULL, NULL, NULL, NULL);
            Rval = FORMAT_ERR_INVALID_ARG;
        }
    }
    return Rval;
}

static UINT32 IsoMux_AudioSTSD_Init(ISO_STSD_AUDIO_BOX_s *Stsd, const SVC_AUDIO_TRACK_INFO_s *Audio, UINT32 SamplesPerChunk, UINT32 MediaId)
{
    UINT32 Rval = FORMAT_OK;
    Stsd->FullBox.Box.Size = 16;
    Stsd->FullBox.Box.Type = TAG_STSD;
    Stsd->EntryCount = 1;
    Stsd->AudioEntry.Version = 0;
    Stsd->AudioEntry.Channels = Audio->Channels;
    Stsd->AudioEntry.BitsPerSample = Audio->BitsPerSample;
    Stsd->AudioEntry.SampleRate = Audio->SampleRate;
    Stsd->AudioEntry.SamplesPerChunk = SamplesPerChunk;
    if ((MediaId == SVC_FORMAT_MID_ADPCM) || (MediaId == SVC_FORMAT_MID_PCM) || (MediaId == SVC_FORMAT_MID_LPCM)) { // TODO: Porting from A7L, check the pcm settings and types.
        Stsd->AudioEntry.Version = 1;
        if (Stsd->AudioEntry.SampleRate < 65536U) {
            Stsd->AudioEntry.Box.Size = 52;
            if (MediaId == SVC_FORMAT_MID_ADPCM) {
                Stsd->AudioEntry.Box.Type = TAG_MS0011; /*MS0011**/
                Stsd->AudioEntry.Esds.Box.Size = 56;
                Stsd->AudioEntry.Esds.Box.Type = TAG_WAVE;
            } else {
                if (Stsd->AudioEntry.BitsPerSample == 16U) { /*16bits**/
                    if (MediaId == SVC_FORMAT_MID_PCM) {
                        Stsd->AudioEntry.Box.Type = TAG_SOWT; /*sowt signed linear PCM in little endian format (PCM)**/
                    } else { // MediaId == SVC_FORMAT_MID_LPCM
                        Stsd->AudioEntry.Box.Type = TAG_TWOS; /*twos signed linear PCM in big endian format (LPCM) from Wes**/
                    }
                } else if (Stsd->AudioEntry.BitsPerSample == 24U) { // 24bits
                    Stsd->AudioEntry.Box.Type = TAG_IN24; /*in24**/
                    Stsd->AudioEntry.Esds.Box.Size = 38;
                    Stsd->AudioEntry.Esds.Box.Type = TAG_WAVE;
                } else {
                    AmbaPrint_ModulePrintStr5(SVC_MUXER_PRINT_MODULE_ID, "%s Not Support : Please check Audio BitsPerSample setting.", __func__, NULL, NULL, NULL, NULL);
                    Rval = FORMAT_ERR_INVALID_ARG;
                }
            }
            if (Rval == FORMAT_OK) {
                Stsd->AudioEntry.Box.Size += Stsd->AudioEntry.Esds.Box.Size;
            }
        } else { // TODO
            Stsd->AudioEntry.Version = 2;
            Stsd->AudioEntry.Box.Size = 72;
            Stsd->AudioEntry.Box.Type = TAG_LPCM; /* LPCM**/
        }
    } else if (MediaId == SVC_FORMAT_MID_AAC) {
        Stsd->AudioEntry.Box.Size = 36;
        Stsd->AudioEntry.Box.Type = TAG_MP4A; /* MP4A**/
        Stsd->AudioEntry.Esds.Box.Size = 50;
        Stsd->AudioEntry.Esds.Box.Type = TAG_ESDS;
        Stsd->AudioEntry.Esds.MaxBrate = Audio->Bitrate;
        Stsd->AudioEntry.Esds.AvgBrate = Audio->Bitrate;
        Stsd->AudioEntry.Esds.AACinfo = (UINT16)IsoMux_AAC_Info(Stsd->AudioEntry.SampleRate, Stsd->AudioEntry.Channels);
        Stsd->AudioEntry.Box.Size += Stsd->AudioEntry.Esds.Box.Size;
    } else {
        AmbaPrint_ModulePrintStr5(SVC_MUXER_PRINT_MODULE_ID, "%s Not supported!", __func__, NULL, NULL, NULL, NULL);
        Rval = FORMAT_ERR_INVALID_ARG;
    }
    if (Rval == FORMAT_OK) {
        Stsd->FullBox.Box.Size += Stsd->AudioEntry.Box.Size;
    //    LOG_INFO("[%u]: SampleRate = %u, Frame_Size = %u , BitsPerSample = %u, Channels = %u", MediaId, Audio->SampleRate,
    //            SamplesPerChunk, Audio->BitsPerSample, Audio->Channels);
    }
    return Rval;
}

static UINT32 IsoMux_TextSTSD_Text(STSD_TEXT_ENTRY_s *TextEntry, SVC_STREAM_HDLR_s *Stream)
{
    /* TextEntry.Box.Size */
    UINT32 Rval = SvcFormat_PutBe32(Stream, TextEntry->Box.Size);
    if (Rval == FORMAT_OK) {
        /* TextEntry.Box.Type */
        Rval = SvcFormat_PutBe32(Stream, TextEntry->Box.Type);
    }
    if (Rval == FORMAT_OK) {
        /* TextEntry.Reserv6 */
        Rval = IsoMux_PutBuffer8(Stream, TextEntry->Reserv6, 6U);
    }
    if (Rval == FORMAT_OK) {
        /* TextEntry.DataRefIdx */
        Rval = SvcFormat_PutBe16(Stream, TextEntry->DataRefIdx);
    }
    if (Rval == FORMAT_OK) {
        /* TextEntry.DisFlags */
        Rval = SvcFormat_PutBe32(Stream, TextEntry->DisFlags);
    }
    if (Rval == FORMAT_OK) {
        /* TextEntry.TextJust */
        Rval = SvcFormat_PutBe32(Stream, TextEntry->TextJust);
    }
    if (Rval == FORMAT_OK) {
        /* TextEntry.BgColor[3] */
        Rval = IsoMux_PutBufferBe16(Stream, TextEntry->BgColor, 3U);
    }
    if (Rval == FORMAT_OK) {
        /* TextEntry.TextBox */
        Rval = SvcFormat_PutBe64(Stream, TextEntry->TextBox);
    }
    if (Rval == FORMAT_OK) {
        /* TextEntry.Reserv8 */
        Rval = IsoMux_PutBuffer8(Stream, TextEntry->Reserv8, 8U);
    }
    if (Rval == FORMAT_OK) {
        /* TextEntry.FontNumber */
        Rval = SvcFormat_PutBe16(Stream, TextEntry->FontNumber);
    }
    if (Rval == FORMAT_OK) {
        /* TextEntry.FontFace */
        Rval = SvcFormat_PutBe16(Stream, TextEntry->FontFace);
    }
    if (Rval == FORMAT_OK) {
        /* TextEntry.Reserv3 */
        Rval = IsoMux_PutBuffer8(Stream, TextEntry->Reserv3, 3U);
    }
    if (Rval == FORMAT_OK) {
        /* TextEntry.FgColor[3] */
        Rval = IsoMux_PutBufferBe16(Stream, TextEntry->FgColor, 3U);
    }
    if (Rval == FORMAT_OK) {
        UINT32 Count;
        Rval = S2F(Stream->Func->Write(Stream, ISO_STSD_MAX_TEXT_NAME_LEN, (UINT8 *)TextEntry->TextName, &Count));
        if (Rval == FORMAT_OK) {
            if (Count != ISO_STSD_MAX_TEXT_NAME_LEN) {
                AmbaPrint_ModulePrintStr5(SVC_MUXER_PRINT_MODULE_ID, "%s Stream->Func->Write() failed!", __func__, NULL, NULL, NULL, NULL);
                Rval = FORMAT_ERR_IO_ERROR;
            }
        } else {
            AmbaPrint_ModulePrintStr5(SVC_MUXER_PRINT_MODULE_ID, "%s Stream->Func->Write() failed!", __func__, NULL, NULL, NULL, NULL);
        }
    }
    return Rval;
}

static UINT32 IsoMux_TextSTSD_MP4S(const STSD_TEXT_ENTRY_s *TextEntry, SVC_STREAM_HDLR_s *Stream)
{
    /* TextEntry.Box.Size */
    UINT32 Rval = SvcFormat_PutBe32(Stream, TextEntry->Box.Size);
    if (Rval == FORMAT_OK) {
        /* TextEntry.Box.Type */
        Rval = SvcFormat_PutBe32(Stream, TextEntry->Box.Type);
    }
    if (Rval == FORMAT_OK) {
        Rval = SvcFormat_PutBe32(Stream, 0);
    }
    if (Rval == FORMAT_OK) {
        Rval = SvcFormat_PutBe16(Stream, 0);
    }
    if (Rval == FORMAT_OK) {
        Rval = SvcFormat_PutBe16(Stream, 0x01);
    }
    return Rval;
}

static UINT32 IsoMux_TextSTSD_Write(ISO_STSD_TEXT_BOX_s *Stsd, SVC_STREAM_HDLR_s *Stream, const SVC_TEXT_TRACK_INFO_s *Track)
{
    UINT32 Rval;
    /* Put Stsd */
    /* Stsd.FullBox.Box.Size */
    Rval = SvcFormat_PutBe32(Stream, Stsd->FullBox.Box.Size);
    if (Rval == FORMAT_OK) {
        /* Stsd.FullBox.Box.Type */
        Rval = SvcFormat_PutBe32(Stream, Stsd->FullBox.Box.Type);
    }
    if (Rval == FORMAT_OK) {
        /* Stsd.FullBox.Version, Flag */
        Rval = SvcFormat_PutBe32(Stream, 0);
    }
    if (Rval == FORMAT_OK) {
        /* Stsd.EntryCount */
        Rval = SvcFormat_PutBe32(Stream, Stsd->EntryCount);
    }
    if (Rval == FORMAT_OK) {
        if (Track->Info.MediaId == SVC_FORMAT_MID_TEXT) {
            Rval = IsoMux_TextSTSD_Text(&Stsd->TextEntry, Stream);
        } else if (Track->Info.MediaId == SVC_FORMAT_MID_MP4S) {
            Rval = IsoMux_TextSTSD_MP4S(&Stsd->TextEntry, Stream);
        } else {
            AmbaPrint_ModulePrintStr5(SVC_MUXER_PRINT_MODULE_ID, "%s Not supported!", __func__, NULL, NULL, NULL, NULL);
            Rval = FORMAT_ERR_INVALID_ARG;
        }
    }
    return Rval;
}

static UINT32 IsoMux_TextSTSD_Init(ISO_STSD_TEXT_BOX_s *Stsd, UINT32 MediaId)
{
    UINT32 Rval = FORMAT_OK;
    const char Txtname[ISO_STSD_MAX_TEXT_NAME_LEN] = {'G', 'e', 'n', 'e', 'v', 'a'};
    Stsd->FullBox.Box.Size = 16;
    Stsd->FullBox.Box.Type = TAG_STSD;
    Stsd->EntryCount = 1;
    if (MediaId == SVC_FORMAT_MID_TEXT) {
        Stsd->TextEntry.Box.Size = 65;
        Stsd->TextEntry.Box.Type = TAG_TEXT;
        Stsd->TextEntry.DataRefIdx = 1;
        Stsd->TextEntry.DisFlags = 0x00006000;
        Stsd->TextEntry.TextJust = 1;
        Stsd->TextEntry.BgColor[0] = 0;
        Stsd->TextEntry.BgColor[1] = 0;
        Stsd->TextEntry.BgColor[2] = 0;
        Stsd->TextEntry.TextBox = 0;
        Stsd->TextEntry.FontNumber = 0;
        Stsd->TextEntry.FontFace = 0;
        Stsd->TextEntry.FgColor[0] = 0;
        Stsd->TextEntry.FgColor[1] = 0;
        Stsd->TextEntry.FgColor[2] = 0;
        AmbaUtility_StringCopy(Stsd->TextEntry.TextName, ISO_STSD_MAX_TEXT_NAME_LEN, Txtname);   // no need to ensure null terminated
        Stsd->FullBox.Box.Size += Stsd->TextEntry.Box.Size;
    } else if (MediaId == SVC_FORMAT_MID_MP4S) {
        Stsd->TextEntry.Box.Size = 16;
        Stsd->TextEntry.Box.Type = TAG_MP4S;
        Stsd->FullBox.Box.Size += Stsd->TextEntry.Box.Size;
    } else {
        AmbaPrint_ModulePrintStr5(SVC_MUXER_PRINT_MODULE_ID, "%s Not supported!", __func__, NULL, NULL, NULL, NULL);
        Rval = FORMAT_ERR_INVALID_ARG;
    }
    return Rval;
}

static UINT32 IsoMux_STSC_Write(const ISO_STSC_BOX_s *Stsc, SVC_STREAM_HDLR_s *Stream)
{
    UINT32 Rval;
    /* Stsc.FullBox.Box.Size */
    Rval = SvcFormat_PutBe32(Stream, Stsc->FullBox.Box.Size);
    if (Rval == FORMAT_OK) {
        /* Stsc.FullBox.Box.Type */
        Rval = SvcFormat_PutBe32(Stream, Stsc->FullBox.Box.Type);
    }
    if (Rval == FORMAT_OK) {
        /* Stsc.FullBox.Version */
        Rval = SvcFormat_PutByte(Stream, Stsc->FullBox.Version);
    }
    if (Rval == FORMAT_OK) {
        /* Stsc.FullBox.Flags[0] */
        Rval = SvcFormat_PutByte(Stream, Stsc->FullBox.Flags[0]);
    }
    if (Rval == FORMAT_OK) {
        /* Stsc.FullBox.Flags[1] */
        Rval = SvcFormat_PutByte(Stream, Stsc->FullBox.Flags[1]);
    }
    if (Rval == FORMAT_OK) {
        /* Stsc.FullBox.Flags[2] */
        Rval = SvcFormat_PutByte(Stream, Stsc->FullBox.Flags[2]);
    }
    if (Rval == FORMAT_OK) {
        /* Stsc.EntryCount */
        Rval = SvcFormat_PutBe32(Stream, Stsc->EntryCount);
    }
    if ((Stsc->EntryCount > 0U) && (Rval == FORMAT_OK)) {
        /* Stsc.StscTable.FirstChunk */
        Rval = SvcFormat_PutBe32(Stream, Stsc->StscTable.FirstChunk);
        if (Rval == FORMAT_OK) {
            /* Stsc.StscTable.SamPerChunk */
            Rval = SvcFormat_PutBe32(Stream, Stsc->StscTable.SamPerChunk);
        }
        if (Rval == FORMAT_OK) {
            /* Stsc.StscTable.SamDescIdx */
            Rval = SvcFormat_PutBe32(Stream, Stsc->StscTable.SamDescIdx);
        }
    }
    return Rval;
}

static void IsoMux_STSC_Init(ISO_STSC_BOX_s *Stsc, UINT32 SamplesPerChunk, UINT32 EntryCount)
{
    Stsc->FullBox.Box.Size = 16;
    Stsc->FullBox.Box.Type = TAG_STSC;
    if (EntryCount > 0U) {
        Stsc->EntryCount = EntryCount;
        // TODO: Multi entry
        Stsc->StscTable.FirstChunk = 1;
        Stsc->StscTable.SamDescIdx = 1;
        Stsc->StscTable.SamPerChunk = SamplesPerChunk;
        Stsc->FullBox.Box.Size += Stsc->EntryCount *12U;
    }
}

static UINT32 IsoMux_VideoSTTS_Write(const ISO_STTS_BOX_s *Stts, SVC_STREAM_HDLR_s *Stream)
{
    UINT32 Rval;
    /* Stts.FullBox.Box.Size */
    Rval = SvcFormat_PutBe32(Stream, Stts->FullBox.Box.Size);
    if (Rval == FORMAT_OK) {
        /* Stts.FullBox.Box.Type */
        Rval = SvcFormat_PutBe32(Stream, Stts->FullBox.Box.Type);
    }
    if (Rval == FORMAT_OK) {
        /* Stts.FullBox.Version */
        Rval = SvcFormat_PutByte(Stream, Stts->FullBox.Version);
    }
    if (Rval == FORMAT_OK) {
        /* Stts.FullBox.Flags[0] */
        Rval = SvcFormat_PutByte(Stream, Stts->FullBox.Flags[0]);
    }
    if (Rval == FORMAT_OK) {
        /* Stts.FullBox.Flags[1] */
        Rval = SvcFormat_PutByte(Stream, Stts->FullBox.Flags[1]);
    }
    if (Rval == FORMAT_OK) {
        /* Stts.FullBox.Flags[2] */
        Rval = SvcFormat_PutByte(Stream, Stts->FullBox.Flags[2]);
    }
    if (Rval == FORMAT_OK) {
        /* Stts.EntryCount */
        Rval = SvcFormat_PutBe32(Stream, Stts->EntryCount);
    }
    if ((Stts->EntryCount > 0U) && (Rval == FORMAT_OK)) {
        //FIX ME
        #if 0
        Rval = IsoMux_WriteSTTS(Stream, trackId, Stts->ItemId, Stts->TmpOff, Stts->EntryCount, Stts->SttsTable.SampleCount, Stts->SttsTable.SampleDelta);
        if (Rval != FORMAT_OK){
            LOG_ERR("IsoIdx_WriteSTTS() error!");
        }
        #endif
    }
    return Rval;
}

static void IsoMux_VideoSTTS_Init(ISO_STTS_BOX_s *Stts, UINT32 EntryCount, UINT32 TmpOff, UINT32 SampleCount, UINT32 SampleDelta, UINT8 ItemId)
{
    Stts->FullBox.Box.Size = 16;
    Stts->FullBox.Box.Type = TAG_STTS;
    if (EntryCount > 0U) {
        Stts->EntryCount = EntryCount;
        Stts->TmpOff = TmpOff;
        Stts->ItemId = ItemId;
        Stts->EntryUnit = (UINT32)sizeof(STTS_ENTRY_s);
        Stts->SttsTable.SampleCount = SampleCount;
        Stts->SttsTable.SampleDelta = SampleDelta;
        Stts->FullBox.Box.Size += Stts->EntryCount * Stts->EntryUnit;
    }
}

static UINT32 IsoMux_STTS_Write(const ISO_STTS_BOX_s *Stts, SVC_STREAM_HDLR_s *Stream)
{
    UINT32 Rval;
    /* Stts.FullBox.Box.Size */
    Rval = SvcFormat_PutBe32(Stream, Stts->FullBox.Box.Size);
    if (Rval == FORMAT_OK) {
        /* Stts.FullBox.Box.Type */
        Rval = SvcFormat_PutBe32(Stream, Stts->FullBox.Box.Type);
    }
    if (Rval == FORMAT_OK) {
        /* Stts.FullBox.Version */
        Rval = SvcFormat_PutByte(Stream, Stts->FullBox.Version);
    }
    if (Rval == FORMAT_OK) {
        /* Stts.FullBox.Flags[0] */
        Rval = SvcFormat_PutByte(Stream, Stts->FullBox.Flags[0]);
    }
    if (Rval == FORMAT_OK) {
        /* Stts.FullBox.Flags[1] */
        Rval = SvcFormat_PutByte(Stream, Stts->FullBox.Flags[1]);
    }
    if (Rval == FORMAT_OK) {
        /* Stts.FullBox.Flags[2] */
        Rval = SvcFormat_PutByte(Stream, Stts->FullBox.Flags[2]);
    }
    if (Rval == FORMAT_OK) {
        /* Stts.EntryCount */
        Rval = SvcFormat_PutBe32(Stream, Stts->EntryCount);
    }
    if ((Stts->EntryCount > 0U) && (Rval == FORMAT_OK)) {
        /* Stts.SttsTable.SampleCount */
        Rval = SvcFormat_PutBe32(Stream, Stts->SttsTable.SampleCount);
        if (Rval == FORMAT_OK) {
            /* Stts.SttsTable.SampleDelta */
            Rval = SvcFormat_PutBe32(Stream, Stts->SttsTable.SampleDelta);
        }
    }
    return Rval;
}

static void IsoMux_STTS_Init(ISO_STTS_BOX_s *Stts, UINT32 SampleCount, UINT32 SampleDelta, UINT32 MediaId, UINT8 ItemId, UINT32 EntryCount)
{
    Stts->FullBox.Box.Size = 16;
    Stts->FullBox.Box.Type = TAG_STTS;
    if (EntryCount > 0U) {
        Stts->EntryCount = EntryCount;
        Stts->ItemId = ItemId;
        Stts->EntryUnit = (UINT32)sizeof(STTS_ENTRY_s);
        Stts->SttsTable.SampleCount = SampleCount;
        Stts->SttsTable.SampleDelta = SampleDelta;
        if ((MediaId == SVC_FORMAT_MID_ADPCM) || (MediaId == SVC_FORMAT_MID_PCM) || (MediaId == SVC_FORMAT_MID_LPCM)) {
            Stts->SttsTable.SampleCount = SampleCount * SampleDelta;
            Stts->SttsTable.SampleDelta = 1;
        }
        Stts->FullBox.Box.Size += Stts->EntryCount * Stts->EntryUnit;
    }
}

static UINT32 IsoMux_STCO_Write(const ISO_STCO_BOX_s *Stco, SVC_STREAM_HDLR_s *Stream)
{
    UINT32 Rval;
    /* Stco.FullBox.Box.Size */
    Rval = SvcFormat_PutBe32(Stream, Stco->FullBox.Box.Size);
    if (Rval == FORMAT_OK) {
        /* Stco.FullBox.Box.Type */
        Rval = SvcFormat_PutBe32(Stream, Stco->FullBox.Box.Type);
    }
    if (Rval == FORMAT_OK) {
        /* Stco.FullBox.Version */
        Rval = SvcFormat_PutByte(Stream, Stco->FullBox.Version);
    }
    if (Rval == FORMAT_OK) {
        /* Stco.FullBox.Flags[0] */
        Rval = SvcFormat_PutByte(Stream, Stco->FullBox.Flags[0]);
    }
    if (Rval == FORMAT_OK) {
        /* Stco.FullBox.Flags[1] */
        Rval = SvcFormat_PutByte(Stream, Stco->FullBox.Flags[1]);
    }
    if (Rval == FORMAT_OK) {
        /* Stco.FullBox.Flags[2] */
        Rval = SvcFormat_PutByte(Stream, Stco->FullBox.Flags[2]);
    }
    if (Rval == FORMAT_OK) {
        /* Stco.EntryCount */
        Rval = SvcFormat_PutBe32(Stream, Stco->EntryCount);
    }
    if ((Stco->EntryCount > 0U) && (Rval == FORMAT_OK)) {
        //FIX ME
        #if 0
        Rval = IsoMux_Index_Write(Stream, trackId, Stco->ItemId, Stco->TmpOff, Stco->EntryCount, Stco->EntryUnit, 1U, clusterOff);
        if (Rval != FORMAT_OK) {
            LOG_ERR("IsoMux_Index_Write() error!");
        }
        #endif
    }
    return Rval;
}

static void IsoMux_STCO_Init(ISO_STCO_BOX_s *Stco, UINT32 EntryCount, UINT32 TmpOff, UINT8 ItemId)
{
    Stco->FullBox.Box.Size = 16;
    Stco->FullBox.Box.Type = TAG_STCO;
    if (EntryCount > 0U) {
        Stco->EntryUnit = 4;
        Stco->EnableCO64 = 0U;
        Stco->EntryCount = EntryCount;
        Stco->TmpOff = TmpOff;
        Stco->ItemId = ItemId;
        Stco->FullBox.Box.Size += Stco->EntryCount * Stco->EntryUnit;
    }
}

static UINT32 IsoMux_STSZ_Write(const ISO_STSZ_BOX_s *Stsz, SVC_STREAM_HDLR_s *Stream, UINT32 MediaId)
{
    UINT32 Rval;
    /* Stsz.FullBox.Box.Size */
    Rval = SvcFormat_PutBe32(Stream, Stsz->FullBox.Box.Size);
    if (Rval == FORMAT_OK) {
        /* Stsz.FullBox.Box.Type */
        Rval = SvcFormat_PutBe32(Stream, Stsz->FullBox.Box.Type);
    }
    if (Rval == FORMAT_OK) {
        /* Stsz.FullBox.Version */
        Rval = SvcFormat_PutByte(Stream, Stsz->FullBox.Version);
    }
    if (Rval == FORMAT_OK) {
        /* Stsz.FullBox.Flags[0] */
        Rval = SvcFormat_PutByte(Stream, Stsz->FullBox.Flags[0]);
    }
    if (Rval == FORMAT_OK) {
        /* Stsz.FullBox.Flags[1] */
        Rval = SvcFormat_PutByte(Stream, Stsz->FullBox.Flags[1]);
    }
    if (Rval == FORMAT_OK) {
        /* Stsz.FullBox.Flags[2] */
        Rval = SvcFormat_PutByte(Stream, Stsz->FullBox.Flags[2]);
    }
    if (Rval == FORMAT_OK) {
        /* Stsz.SampleSize */
        Rval = SvcFormat_PutBe32(Stream, Stsz->SampleSize);
    }
    if (Rval == FORMAT_OK) {
        /* Stsz.SampleCount */
        Rval = SvcFormat_PutBe32(Stream, Stsz->SampleCount);
    }
    if ((Stsz->SampleCount > 0U) && (Rval == FORMAT_OK)) {
        if ((MediaId != SVC_FORMAT_MID_ADPCM) && (MediaId != SVC_FORMAT_MID_PCM) && (MediaId != SVC_FORMAT_MID_LPCM)) {
            #if 0
            Rval = IsoMux_Index_Write(Stream, trackId, Stsz->ItemId, Stsz->TmpOff, Stsz->SampleCount, Stsz->EntryUnit, 0U, 0);
            if (Rval != FORMAT_OK){
                LOG_ERR("IsoMux_Index_Write() error!");
            }
            #endif
        }
    }
    return Rval;
}

static void IsoMux_STSZ_Init(ISO_STSZ_BOX_s *Stsz, UINT32 EntryCount, UINT32 TmpOff, UINT32 MediaId,  UINT8 ItemId)
{
    Stsz->FullBox.Box.Size = 20;
    Stsz->FullBox.Box.Type = TAG_STSZ;
    if (EntryCount > 0U) {
        Stsz->ItemId = ItemId;
        Stsz->SampleCount = EntryCount;
        if ((MediaId == SVC_FORMAT_MID_ADPCM) || (MediaId == SVC_FORMAT_MID_PCM) || (MediaId == SVC_FORMAT_MID_LPCM)) {
            Stsz->SampleSize = 1;
        } else {
            Stsz->SampleSize = 0;
            Stsz->TmpOff = TmpOff;
            Stsz->EntryUnit = 4;
            Stsz->FullBox.Box.Size += Stsz->SampleCount * Stsz->EntryUnit;
        }
    }
}

static UINT32 IsoMux_STSS_Write(const ISO_STSS_BOX_s *Stss, SVC_STREAM_HDLR_s *Stream)
{
    UINT32 Rval;
    /* Stss.FullBox.Box.Size */
    Rval = SvcFormat_PutBe32(Stream, Stss->FullBox.Box.Size);
    if (Rval == FORMAT_OK) {
        /* Stss.FullBox.Box.Type */
        Rval = SvcFormat_PutBe32(Stream, Stss->FullBox.Box.Type);
    }
    if (Rval == FORMAT_OK) {
        /* Stss.FullBox.Version */
        Rval = SvcFormat_PutByte(Stream, Stss->FullBox.Version);
    }
    if (Rval == FORMAT_OK) {
        /* Stss.FullBox.Flags[0] */
        Rval = SvcFormat_PutByte(Stream, Stss->FullBox.Flags[0]);
    }
    if (Rval == FORMAT_OK) {
        /* Stss.FullBox.Flags[1] */
        Rval = SvcFormat_PutByte(Stream, Stss->FullBox.Flags[1]);
    }
    if (Rval == FORMAT_OK) {
        /* Stss.FullBox.Flags[2] */
        Rval = SvcFormat_PutByte(Stream, Stss->FullBox.Flags[2]);
    }
    if (Rval == FORMAT_OK) {
        /* Stss.EntryCount */
        Rval = SvcFormat_PutBe32(Stream, Stss->EntryCount);
    }
    if (Rval == FORMAT_OK) {
        #if 0
        Rval = IsoMux_Index_Write(Stream, trackId, Stss->ItemId, Stss->TmpOff, Stss->EntryCount, Stss->EntryUnit, 0U, 0);
        if (Rval != FORMAT_OK){
            LOG_ERR("IsoMux_Index_Write() error!");
        }
        #endif
    }
    return Rval;
}

static void IsoMux_STSS_Init(ISO_STSS_BOX_s *Stss, UINT32 EntryCount, UINT32 TmpOff, UINT8 ItemId)
{
    Stss->FullBox.Box.Size = 16;
    Stss->FullBox.Box.Type = TAG_STSS;
    if (EntryCount > 0U) {
        Stss->EntryCount = EntryCount;
        Stss->ItemId = ItemId;
        Stss->TmpOff = TmpOff;
        Stss->EntryUnit = 4;
        Stss->FullBox.Box.Size += Stss->EntryCount * Stss->EntryUnit;
    }
}

static UINT32 IsoMux_CTTS_Write(const ISO_CTTS_BOX_s *Ctts, SVC_STREAM_HDLR_s *Stream)
{
    UINT32 Rval;
    /* Ctts.FullBox.Box.Size */
    Rval = SvcFormat_PutBe32(Stream, Ctts->FullBox.Box.Size);
    if (Rval == FORMAT_OK) {
        /* Ctts.FullBox.Box.Type */
        Rval = SvcFormat_PutBe32(Stream, Ctts->FullBox.Box.Type);
    }
    if (Rval == FORMAT_OK) {
        /* Ctts.FullBox.Version */
        Rval = SvcFormat_PutByte(Stream, Ctts->FullBox.Version);
    }
    if (Rval == FORMAT_OK) {
        /* Ctts.FullBox.Flags[0] */
        Rval = SvcFormat_PutByte(Stream, Ctts->FullBox.Flags[0]);
    }
    if (Rval == FORMAT_OK) {
        /* Ctts.FullBox.Flags[1] */
        Rval = SvcFormat_PutByte(Stream, Ctts->FullBox.Flags[1]);
    }
    if (Rval == FORMAT_OK) {
        /* Ctts.FullBox.Flags[2] */
        Rval = SvcFormat_PutByte(Stream, Ctts->FullBox.Flags[2]);
    }
    if (Rval == FORMAT_OK) {
        /* Ctts.EntryCount */
        Rval = SvcFormat_PutBe32(Stream, Ctts->EntryCount);
    }
    if ((Ctts->EntryCount > 0U) && (Rval == FORMAT_OK)) {
    //FIX ME
    #if 0
        Rval = IsoMux_Index_Write(Stream, trackId, Ctts->ItemId, Ctts->TmpOff, Ctts->EntryCount, Ctts->EntryUnit, 0U, 0);
        if (Rval != FORMAT_OK){
            LOG_ERR("IsoMux_Index_Write() error!");
        }
    #endif
    }
    return Rval;
}

static void IsoMux_CTTS_Init(ISO_CTTS_BOX_s *Ctts, UINT32 EntryCount, UINT32 TmpOff,  UINT8 ItemId)
{
    Ctts->FullBox.Box.Size = 16;
    Ctts->FullBox.Box.Type = TAG_CTTS;
    if (EntryCount > 0U) {
        Ctts->EntryCount = EntryCount;
        Ctts->ItemId = ItemId;
        Ctts->TmpOff = TmpOff;
        Ctts->EntryUnit = (UINT32)sizeof(CTTS_ENTRY_s);
        Ctts->FullBox.Box.Size += Ctts->EntryCount * Ctts->EntryUnit;
    }
}

static UINT32 IsoMux_VideoSTBL_Write(ISO_STBL_VIDEO_BOX_s *Stbl, SVC_STREAM_HDLR_s *Stream, const SVC_VIDEO_TRACK_INFO_s *Track)
{
    UINT32 Rval;
    /* Stbl.Box.Size */
    Rval = SvcFormat_PutBe32(Stream, Stbl->Box.Size);
    if (Rval == FORMAT_OK) {
        /* Stbl.Box.Type */
        Rval = SvcFormat_PutBe32(Stream, Stbl->Box.Type);
    }
    /* Write STSD BOX*/
    if (Rval == FORMAT_OK) {
        Rval = IsoMux_VideoSTSD_Write(&Stbl->StsdAtom, Stream, Track);
    }
    /* Write STTS BOX*/
    if (Rval == FORMAT_OK) {
        Rval = IsoMux_VideoSTTS_Write(&Stbl->SttsAtom, Stream);
    }
    /* Write STSC BOX*/
    if (Rval == FORMAT_OK) {
        Rval = IsoMux_STSC_Write(&Stbl->StscAtom, Stream);
    }
    /* Write CTTS BOX*/
    if ((Stbl->StssAtom.EntryCount > 0U) && (Rval == FORMAT_OK)) {    /* fragmented MP4 has no CTTS */
        if (Track->M > 1U) {
            Rval = IsoMux_CTTS_Write(&Stbl->CttsAtom, Stream);
        }
    }
    /* Write STSZ BOX*/
    if (Rval == FORMAT_OK) {
        Rval = IsoMux_STSZ_Write(&Stbl->StszAtom, Stream, Track->Info.MediaId);
    }
    /* Write STCO BOX*/
    if (Rval == FORMAT_OK) {
        Rval = IsoMux_STCO_Write(&Stbl->StcoAtom, Stream);
    }
    if ((Stbl->StssAtom.EntryCount > 0U) && (Rval == FORMAT_OK)) {    /* fragmented MP4 has no STSS */
        /* Write STSS BOX*/
        Rval = IsoMux_STSS_Write(&Stbl->StssAtom, Stream);
    }
    return Rval;
}

static UINT32 IsoMux_VideoSTBL_Init(ISO_STBL_VIDEO_BOX_s *Stbl, const SVC_VIDEO_TRACK_INFO_s *Video, const SVC_ISO_VIDEO_TRACK_INFO_s *VideoInfo, UINT8 Dummy)
{
    UINT32 Rval;
    const SVC_MEDIA_TRACK_GENERAL_INFO_s * Track = &Video->Info;
    Stbl->Box.Size = 8;
    Stbl->Box.Type = TAG_STBL;
    Rval = IsoMux_VideoSTSD_Init(&Stbl->StsdAtom, Video, VideoInfo, Track->MediaId);
    if (Rval == FORMAT_OK) {
        Stbl->Box.Size += Stbl->StsdAtom.FullBox.Box.Size;
        if (Dummy == 0U) {
            IsoMux_STSC_Init(&Stbl->StscAtom, 1, 1);
            Stbl->Box.Size += Stbl->StscAtom.FullBox.Box.Size;
            IsoMux_STSZ_Init(&Stbl->StszAtom, Track->FrameCount, (UINT32)VideoInfo->StszOffset, Track->MediaId, 0U);
            Stbl->Box.Size += Stbl->StszAtom.FullBox.Box.Size;
            IsoMux_STCO_Init(&Stbl->StcoAtom, Track->FrameCount, (UINT32)VideoInfo->StcoOffset, 1U);
            Stbl->Box.Size += Stbl->StcoAtom.FullBox.Box.Size;
            IsoMux_STSS_Init(&Stbl->StssAtom, VideoInfo->KeyFrameCount, (UINT32)VideoInfo->StssOffset, 2U);
            Stbl->Box.Size += Stbl->StssAtom.FullBox.Box.Size;
            IsoMux_VideoSTTS_Init(&Stbl->SttsAtom, VideoInfo->SttsCount, (UINT32)VideoInfo->SttsOffset, Track->FrameCount, (UINT32)NORMALIZE_TO_TIMESCALE(Track->TimeScale, Track->TimePerFrame, Track->OrigTimeScale), 3U);
            Stbl->Box.Size += Stbl->SttsAtom.FullBox.Box.Size;
            if (Video->M > 1U) {
                IsoMux_CTTS_Init(&Stbl->CttsAtom, Track->FrameCount, (UINT32)VideoInfo->CttsOffset, 4U);
                Stbl->Box.Size += Stbl->CttsAtom.FullBox.Box.Size;
            }
        } else {
            IsoMux_STSC_Init(&Stbl->StscAtom, 0, 0);
            Stbl->Box.Size += Stbl->StscAtom.FullBox.Box.Size;
            IsoMux_STSZ_Init(&Stbl->StszAtom, 0, 0, 0, 0);
            Stbl->Box.Size += Stbl->StszAtom.FullBox.Box.Size;
            IsoMux_STCO_Init(&Stbl->StcoAtom, 0, 0, 0);
            Stbl->Box.Size += Stbl->StcoAtom.FullBox.Box.Size;
            IsoMux_STTS_Init(&Stbl->SttsAtom, 0, 0, 0, 0, 0);
            Stbl->Box.Size += Stbl->SttsAtom.FullBox.Box.Size;
        }
    }
    return Rval;
}

static UINT32 IsoMux_AudioSTBL_Write(const ISO_STBL_AUDIO_BOX_s *Stbl, SVC_STREAM_HDLR_s *Stream, const SVC_AUDIO_TRACK_INFO_s *Track)
{
    UINT32 Rval;
    /* Stbl.Box.Size */
    Rval = SvcFormat_PutBe32(Stream, Stbl->Box.Size);
    if (Rval == FORMAT_OK) {
        /* Stbl.Box.Type */
        Rval = SvcFormat_PutBe32(Stream, Stbl->Box.Type);
    }
    /* Write STSD BOX*/
    if (Rval == FORMAT_OK) {
        Rval = IsoMux_AudioSTSD_Write(&Stbl->StsdAtom, Stream, Track);
        if (Rval != FORMAT_OK) {
            AmbaPrint_ModulePrintStr5(SVC_MUXER_PRINT_MODULE_ID, "%s IsoMux_AudioSTSD_Write() error!", __func__, NULL, NULL, NULL, NULL);
        }
    }
    /* Write STTS BOX*/
    if (Rval == FORMAT_OK) {
        Rval = IsoMux_STTS_Write(&Stbl->SttsAtom, Stream);
        if (Rval != FORMAT_OK) {
            AmbaPrint_ModulePrintStr5(SVC_MUXER_PRINT_MODULE_ID, "%s IsoMux_STTS_Write() error!", __func__, NULL, NULL, NULL, NULL);
        }
    }
    /* Write STSC BOX*/
    if (Rval == FORMAT_OK) {
        Rval = IsoMux_STSC_Write(&Stbl->StscAtom, Stream);
        if (Rval != FORMAT_OK) {
            AmbaPrint_ModulePrintStr5(SVC_MUXER_PRINT_MODULE_ID, "%s IsoMux_STSC_Write() error!", __func__, NULL, NULL, NULL, NULL);
        }
    }
    /* Write STSZ BOX*/
    if (Rval == FORMAT_OK) {
        Rval = IsoMux_STSZ_Write(&Stbl->StszAtom, Stream, Track->Info.MediaId);
        if (Rval != FORMAT_OK) {
            AmbaPrint_ModulePrintStr5(SVC_MUXER_PRINT_MODULE_ID, "%s IsoMux_STSZ_Write() error!", __func__, NULL, NULL, NULL, NULL);
        }
    }
    /* Write STCO BOX*/
    if (Rval == FORMAT_OK) {
        Rval = IsoMux_STCO_Write(&Stbl->StcoAtom, Stream);
        if (Rval != FORMAT_OK) {
            AmbaPrint_ModulePrintStr5(SVC_MUXER_PRINT_MODULE_ID, "%s IsoMux_STCO_Write(() error!", __func__, NULL, NULL, NULL, NULL);
        }
    }
    return Rval;
}

static UINT32 IsoMux_AudioSTBL_Init(ISO_STBL_AUDIO_BOX_s *Stbl, const SVC_AUDIO_TRACK_INFO_s *Audio, const SVC_ISO_AUDIO_TRACK_INFO_s *AudioInfo, UINT8 Dummy)
{
    UINT32 Rval;
    const SVC_MEDIA_TRACK_GENERAL_INFO_s * Track = &Audio->Info;
    UINT32 SamplesPerChunk = (UINT32)NORMALIZE_TO_TIMESCALE(Track->TimeScale, Track->TimePerFrame, Track->OrigTimeScale);
    Stbl->Box.Size = 8;
    Stbl->Box.Type = TAG_STBL;
    Rval = IsoMux_AudioSTSD_Init(&Stbl->StsdAtom, Audio, SamplesPerChunk, Track->MediaId);
    if (Rval == FORMAT_OK) {
        Stbl->Box.Size += Stbl->StsdAtom.FullBox.Box.Size;
        if (Dummy == 0U) {
            if ((Track->MediaId == SVC_FORMAT_MID_ADPCM) || (Track->MediaId == SVC_FORMAT_MID_PCM) || (Track->MediaId == SVC_FORMAT_MID_LPCM)) {
                IsoMux_STSC_Init(&Stbl->StscAtom, SamplesPerChunk, 1);
            } else {
                IsoMux_STSC_Init(&Stbl->StscAtom, 1, 1);
            }
            Stbl->Box.Size += Stbl->StscAtom.FullBox.Box.Size;
            if ((Track->MediaId == SVC_FORMAT_MID_ADPCM) || (Track->MediaId == SVC_FORMAT_MID_PCM) || (Track->MediaId == SVC_FORMAT_MID_LPCM)) {
                IsoMux_STSZ_Init(&Stbl->StszAtom, SamplesPerChunk * Track->FrameCount, (UINT32)AudioInfo->StszOffset, Track->MediaId, 0U);
            } else {
                IsoMux_STSZ_Init(&Stbl->StszAtom, Track->FrameCount, (UINT32)AudioInfo->StszOffset, Track->MediaId, 0U);
            }
            Stbl->Box.Size += Stbl->StszAtom.FullBox.Box.Size;
            IsoMux_STCO_Init(&Stbl->StcoAtom, Track->FrameCount, (UINT32)AudioInfo->StcoOffset, 1U);
            Stbl->Box.Size += Stbl->StcoAtom.FullBox.Box.Size;
            IsoMux_STTS_Init(&Stbl->SttsAtom, Track->FrameCount, SamplesPerChunk, Track->MediaId, 2, 1U);
            Stbl->Box.Size += Stbl->SttsAtom.FullBox.Box.Size;
        } else {
            IsoMux_STSC_Init(&Stbl->StscAtom, 0, 0);
            Stbl->Box.Size += Stbl->StscAtom.FullBox.Box.Size;
            IsoMux_STSZ_Init(&Stbl->StszAtom, 0, 0, 0, 0);
            Stbl->Box.Size += Stbl->StszAtom.FullBox.Box.Size;
            IsoMux_STCO_Init(&Stbl->StcoAtom, 0, 0, 0);
            Stbl->Box.Size += Stbl->StcoAtom.FullBox.Box.Size;
            IsoMux_STTS_Init(&Stbl->SttsAtom, 0, 0, 0, 0, 0);
            Stbl->Box.Size += Stbl->SttsAtom.FullBox.Box.Size;
        }
    }
    return Rval;
}

static UINT32 IsoMux_TextSTBL_Write(ISO_STBL_TEXT_BOX_s *Stbl, SVC_STREAM_HDLR_s *Stream, SVC_TEXT_TRACK_INFO_s *Track)
{
    UINT32 Rval;
    /* Stbl.Box.Size */
    Rval = SvcFormat_PutBe32(Stream, Stbl->Box.Size);
    if (Rval == FORMAT_OK) {
        /* Stbl.Box.Type */
        Rval = SvcFormat_PutBe32(Stream, Stbl->Box.Type);
    }
    /* Write STSD BOX*/
    if (Rval == FORMAT_OK) {
        Rval = IsoMux_TextSTSD_Write(&Stbl->StsdAtom, Stream, Track);
        if (Rval != FORMAT_OK) {
            AmbaPrint_ModulePrintStr5(SVC_MUXER_PRINT_MODULE_ID, "%s IsoMux_TextSTSD_Write(() error!", __func__, NULL, NULL, NULL, NULL);
        }
    }
    /* Write STTS BOX*/
    if (Rval == FORMAT_OK) {
        Rval = IsoMux_STTS_Write(&Stbl->SttsAtom, Stream);
        if (Rval != FORMAT_OK) {
            AmbaPrint_ModulePrintStr5(SVC_MUXER_PRINT_MODULE_ID, "%s IsoMux_STTS_Write() error!", __func__, NULL, NULL, NULL, NULL);
        }
    }
    /* Write STSC BOX*/
    if (Rval == FORMAT_OK) {
        Rval = IsoMux_STSC_Write(&Stbl->StscAtom, Stream);
        if (Rval != FORMAT_OK) {
            AmbaPrint_ModulePrintStr5(SVC_MUXER_PRINT_MODULE_ID, "%s IsoMux_STSC_Write() error!", __func__, NULL, NULL, NULL, NULL);
        }
    }
    /* Write STSZ BOX*/
    if (Rval == FORMAT_OK) {
        Rval = IsoMux_STSZ_Write(&Stbl->StszAtom, Stream, Track->Info.MediaId);
        if (Rval != FORMAT_OK) {
            AmbaPrint_ModulePrintStr5(SVC_MUXER_PRINT_MODULE_ID, "%s IsoMux_STSZ_Write() error!", __func__, NULL, NULL, NULL, NULL);
        }
    }
    /* Write STCO BOX*/
    if (Rval == FORMAT_OK) {
        Rval = IsoMux_STCO_Write(&Stbl->StcoAtom, Stream);
        if (Rval != FORMAT_OK) {
            AmbaPrint_ModulePrintStr5(SVC_MUXER_PRINT_MODULE_ID, "%s IsoMux_STCO_Write() error!", __func__, NULL, NULL, NULL, NULL);
        }
    }
    AmbaMisra_TouchUnused(Track);
    return Rval;
}

static UINT32 IsoMux_TextSTBL_Init(ISO_STBL_TEXT_BOX_s *Stbl, const SVC_TEXT_TRACK_INFO_s *Text, const SVC_ISO_TEXT_TRACK_INFO_s *TextInfo, UINT8 Dummy)
{
    UINT32 Rval;
    const SVC_MEDIA_TRACK_GENERAL_INFO_s * const track = &Text->Info;
    Stbl->Box.Size = 8;
    Stbl->Box.Type = TAG_STBL;
    Rval = IsoMux_TextSTSD_Init(&Stbl->StsdAtom, track->MediaId);
    if (Rval == FORMAT_OK) {
        Stbl->Box.Size += Stbl->StsdAtom.FullBox.Box.Size;
        if (Dummy == 0U) {
            IsoMux_STSC_Init(&Stbl->StscAtom, 1, 1);
            Stbl->Box.Size += Stbl->StscAtom.FullBox.Box.Size;
            IsoMux_STSZ_Init(&Stbl->StszAtom, track->FrameCount, (UINT32)TextInfo->StszOffset, track->MediaId, 0U);
            Stbl->Box.Size += Stbl->StszAtom.FullBox.Box.Size;
            IsoMux_STCO_Init(&Stbl->StcoAtom, track->FrameCount, (UINT32)TextInfo->StcoOffset, 1U);
            Stbl->Box.Size += Stbl->StcoAtom.FullBox.Box.Size;
            IsoMux_STTS_Init(&Stbl->SttsAtom, track->FrameCount, (UINT32)NORMALIZE_TO_TIMESCALE(track->TimeScale, track->TimePerFrame, track->OrigTimeScale), track->MediaId, 2U, 1U);
            Stbl->Box.Size += Stbl->SttsAtom.FullBox.Box.Size;
        } else {
            IsoMux_STSC_Init(&Stbl->StscAtom, 0, 0);
            Stbl->Box.Size += Stbl->StscAtom.FullBox.Box.Size;
            IsoMux_STSZ_Init(&Stbl->StszAtom, 0, 0, 0, 0);
            Stbl->Box.Size += Stbl->StszAtom.FullBox.Box.Size;
            IsoMux_STCO_Init(&Stbl->StcoAtom, 0, 0, 0);
            Stbl->Box.Size += Stbl->StcoAtom.FullBox.Box.Size;
            IsoMux_STTS_Init(&Stbl->SttsAtom, 0, 0, 0, 0, 0);
            Stbl->Box.Size += Stbl->SttsAtom.FullBox.Box.Size;
        }
    }
    return Rval;
}

static UINT32 IsoMux_DREF_Write(const ISO_DREF_BOX_s *Dref, SVC_STREAM_HDLR_s *Stream)
{
    UINT32 Rval;
    /* Dref.FullBox.Box.Size */
    Rval = SvcFormat_PutBe32(Stream, Dref->FullBox.Box.Size);
    if (Rval == FORMAT_OK) {
        /* Dref.FullBox.Box.Type */
        Rval = SvcFormat_PutBe32(Stream, Dref->FullBox.Box.Type);
    }
    if (Rval == FORMAT_OK) {
        /* Dref.FullBox.Version */
        Rval = SvcFormat_PutByte(Stream, Dref->FullBox.Version);
    }
    if (Rval == FORMAT_OK) {
        /* Dref.FullBox.Flags[0] */
        Rval = SvcFormat_PutByte(Stream, Dref->FullBox.Flags[0]);
    }
    if (Rval == FORMAT_OK) {
        /* Dref.FullBox.Flags[1] */
        Rval = SvcFormat_PutByte(Stream, Dref->FullBox.Flags[1]);
    }
    if (Rval == FORMAT_OK) {
        /* Dref.FullBox.Flags[2] */
        Rval = SvcFormat_PutByte(Stream, Dref->FullBox.Flags[2]);
    }
    if (Rval == FORMAT_OK) {
        /* Dref.EntryCount */
        Rval = SvcFormat_PutBe32(Stream, Dref->EntryCount);
    }
    if (Rval == FORMAT_OK) {
        /* Dref.DrefTable.Box.Size */
        Rval = SvcFormat_PutBe32(Stream, Dref->DrefTable.Box.Size);
    }
    if (Rval == FORMAT_OK) {
        /* Dref.DrefTable.Box.Type */
        Rval = SvcFormat_PutBe32(Stream, Dref->DrefTable.Box.Type);
    }
    if (Rval == FORMAT_OK) {
        /* Dref.DrefTable.Version */
        Rval = SvcFormat_PutByte(Stream, Dref->DrefTable.Version);
    }
    if (Rval == FORMAT_OK) {
        /* Dref.DrefTable.Flags[0] */
        Rval = SvcFormat_PutByte(Stream, Dref->DrefTable.Flags[0]);
    }
    if (Rval == FORMAT_OK) {
        /* Dref.DrefTable.Flags[1] */
        Rval = SvcFormat_PutByte(Stream, Dref->DrefTable.Flags[1]);
    }
    if (Rval == FORMAT_OK) {
        /* Dref.DrefTable.Flags[2] */
        Rval = SvcFormat_PutByte(Stream, Dref->DrefTable.Flags[2]);
    }
    return Rval;
}
static void IsoMux_DREF_Init(ISO_DREF_BOX_s *Dref, UINT32 TrackType)
{
    Dref->FullBox.Box.Size = 16;
    Dref->FullBox.Box.Type = TAG_DREF;
    Dref->EntryCount = 1;
    Dref->DrefTable.Box.Size = 12;
    Dref->DrefTable.Box.Type = TAG_URL_;
    if (TrackType == SVC_MEDIA_TRACK_TYPE_TEXT) {
        Dref->DrefTable.Box.Type = TAG_ALIS;
    }
    Dref->DrefTable.Flags[2] = 1;
    Dref->FullBox.Box.Size += Dref->EntryCount * Dref->DrefTable.Box.Size;
}

static UINT32 IsoMux_DINF_Write(const ISO_DINF_BOX_s *Dinf, SVC_STREAM_HDLR_s *Stream)
{
    UINT32 Rval;
    /* Stss.FullBox.Box.Size */
    Rval = SvcFormat_PutBe32(Stream, Dinf->Box.Size);
    if (Rval == FORMAT_OK) {
        /* Stss.FullBox.Box.Type */
        Rval = SvcFormat_PutBe32(Stream, Dinf->Box.Type);
    }
    /* Write DREF BOX*/
    if (Rval == FORMAT_OK) {
        Rval = IsoMux_DREF_Write(&Dinf->DrefAtom, Stream);
        if (Rval != FORMAT_OK) {
            AmbaPrint_ModulePrintStr5(SVC_MUXER_PRINT_MODULE_ID, "%s IsoMux_DREF_Write() error!", __func__, NULL, NULL, NULL, NULL);
        }
    }
    return Rval;
}

static void IsoMux_DINF_Init(ISO_DINF_BOX_s *Dinf, UINT32 TrackType)
{
    Dinf->Box.Size = 8;
    Dinf->Box.Type = TAG_DINF;
    /* Initial DREF BOX*/
    IsoMux_DREF_Init(&Dinf->DrefAtom, TrackType);
    Dinf->Box.Size += Dinf->DrefAtom.FullBox.Box.Size;
}

static UINT32 IsoMux_VMHD_Write(const ISO_VMHD_BOX_s *Vmhd, SVC_STREAM_HDLR_s *Stream)
{
    UINT32 Rval;
    /* Vmhd.FullBox.Box.Size */
    Rval = SvcFormat_PutBe32(Stream, Vmhd->FullBox.Box.Size);
    if (Rval == FORMAT_OK) {
        /* Vmhd.FullBox.Box.Type */
        Rval = SvcFormat_PutBe32(Stream, Vmhd->FullBox.Box.Type);
    }
    if (Rval == FORMAT_OK) {
        /* Vmhd.FullBox.Version */
        Rval = SvcFormat_PutByte(Stream, Vmhd->FullBox.Version);
    }
    if (Rval == FORMAT_OK) {
        /* Vmhd.FullBox.Flags[0] */
        Rval = SvcFormat_PutByte(Stream, Vmhd->FullBox.Flags[0]);
    }
    if (Rval == FORMAT_OK) {
        /* Vmhd.FullBox.Flags[1] */
        Rval = SvcFormat_PutByte(Stream, Vmhd->FullBox.Flags[1]);
    }
    if (Rval == FORMAT_OK) {
        /* Vmhd.FullBox.Flags[2] */
        Rval = SvcFormat_PutByte(Stream, Vmhd->FullBox.Flags[2]);
    }
    if (Rval == FORMAT_OK) {
        /* Vmhd.GraphicMode */
        Rval = SvcFormat_PutBe16(Stream, Vmhd->GraphicMode);
    }
    if (Rval == FORMAT_OK) {
        /* Vmhd.Opcolor[0] */
        Rval = SvcFormat_PutBe16(Stream, Vmhd->Opcolor[0]);
    }
    if (Rval == FORMAT_OK) {
        /* Vmhd.Opcolor[1] */
        Rval = SvcFormat_PutBe16(Stream, Vmhd->Opcolor[1]);
    }
    if (Rval == FORMAT_OK) {
        /* Vmhd.Opcolor[2] */
        Rval = SvcFormat_PutBe16(Stream, Vmhd->Opcolor[2]);
    }
    return Rval;
}

static void IsoMux_VMHD_Init(ISO_VMHD_BOX_s *Vmhd)
{
    Vmhd->FullBox.Box.Size = 20;
    Vmhd->FullBox.Box.Type = TAG_VMHD;
    Vmhd->FullBox.Flags[2] = 1;
}

static UINT32 IsoMux_SMHD_Write(const ISO_SMHD_BOX_s *Smhd, SVC_STREAM_HDLR_s *Stream)
{
    UINT32 Rval;
    /* Smhd.FullBox.Box.Size */
    Rval = SvcFormat_PutBe32(Stream, Smhd->FullBox.Box.Size);
    if (Rval == FORMAT_OK) {
        /* Smhd.FullBox.Box.Type */
        Rval = SvcFormat_PutBe32(Stream, Smhd->FullBox.Box.Type);
    }
    if (Rval == FORMAT_OK) {
        /* Smhd.FullBox.Version */
        Rval = SvcFormat_PutByte(Stream, Smhd->FullBox.Version);
    }
    if (Rval == FORMAT_OK) {
        /* Smhd.FullBox.Flags[0] */
        Rval = SvcFormat_PutByte(Stream, Smhd->FullBox.Flags[0]);
    }
    if (Rval == FORMAT_OK) {
        /* Smhd.FullBox.Flags[1] */
        Rval = SvcFormat_PutByte(Stream, Smhd->FullBox.Flags[1]);
    }
    if (Rval == FORMAT_OK) {
        /* Smhd.FullBox.Flags[2] */
        Rval = SvcFormat_PutByte(Stream, Smhd->FullBox.Flags[2]);
    }
    if (Rval == FORMAT_OK) {
        /* Smhd.Balanced */
        Rval = SvcFormat_PutBe16(Stream, Smhd->Balanced);
    }
    if (Rval == FORMAT_OK) {
        /* Smhd.Reserv2 */
        Rval = SvcFormat_PutBe16(Stream, Smhd->Reserv2);
    }
    return Rval;
}

static void IsoMux_SMHD_Init(ISO_SMHD_BOX_s *Smhd)
{
    Smhd->FullBox.Box.Size = 16;
    Smhd->FullBox.Box.Type = TAG_SMHD;
    Smhd->FullBox.Flags[0] = 0;
    Smhd->FullBox.Flags[1] = 0;
    Smhd->FullBox.Flags[2] = 0;
}

static UINT32 IsoMux_GMHD_Write(const ISO_GMHD_BOX_s *Gmhd, SVC_STREAM_HDLR_s *Stream, UINT8 TrackType)
{
    UINT32 Rval;
    /* Gmhd.Box.Size */
    Rval = SvcFormat_PutBe32(Stream, Gmhd->Box.Size);
    if (Rval == FORMAT_OK) {
        /* Gmhd.Box.Type */
        Rval = SvcFormat_PutBe32(Stream, Gmhd->Box.Type);
    }
    if (Rval == FORMAT_OK) {
        Rval = SvcFormat_PutBe32(Stream, Gmhd->GminAtom.FullBox.Box.Size);
    }
    if (Rval == FORMAT_OK) {
        Rval = SvcFormat_PutBe32(Stream, Gmhd->GminAtom.FullBox.Box.Type);
    }
    if (Rval == FORMAT_OK) {
        Rval = SvcFormat_PutByte(Stream, Gmhd->GminAtom.FullBox.Version);
    }
    if (Rval == FORMAT_OK) {
        Rval = SvcFormat_PutByte(Stream, Gmhd->GminAtom.FullBox.Flags[0]);
    }
    if (Rval == FORMAT_OK) {
        Rval = SvcFormat_PutByte(Stream, Gmhd->GminAtom.FullBox.Flags[1]);
    }
    if (Rval == FORMAT_OK) {
        Rval = SvcFormat_PutByte(Stream, Gmhd->GminAtom.FullBox.Flags[2]);
    }
    if (Rval == FORMAT_OK) {
        Rval = SvcFormat_PutBe16(Stream, Gmhd->GminAtom.GraphicMode);
    }
    if (Rval == FORMAT_OK) {
        Rval = SvcFormat_PutBe16(Stream, Gmhd->GminAtom.Opcolor[0]);
    }
    if (Rval == FORMAT_OK) {
        Rval = SvcFormat_PutBe16(Stream, Gmhd->GminAtom.Opcolor[1]);
    }
    if (Rval == FORMAT_OK) {
        Rval = SvcFormat_PutBe16(Stream, Gmhd->GminAtom.Opcolor[2]);
    }
    if (Rval == FORMAT_OK) {
        Rval = SvcFormat_PutBe16(Stream, Gmhd->GminAtom.Balanced);
    }
    if (Rval == FORMAT_OK) {
        Rval = SvcFormat_PutBe16(Stream, Gmhd->GminAtom.Reserv2);
    }
    if (Rval == FORMAT_OK) {
        Rval = SvcFormat_PutBe32(Stream, Gmhd->HdlrAtom.TextHdlr.Box.Size);
    }
    if (Rval == FORMAT_OK) {
        Rval = SvcFormat_PutBe32(Stream, Gmhd->HdlrAtom.TextHdlr.Box.Type);
    }
    if (Rval == FORMAT_OK) {
        if (TrackType == SVC_FORMAT_MID_TEXT) {
            UINT16 Data[18] = {0x0001, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
                                0x0001, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
                                0x4000, 0x0000};
            UINT32 Count;
            Rval = S2F(Stream->Func->Write(Stream, 36, (UINT8 *)&Data[0], &Count));
            if (Rval == FORMAT_OK) {
                if (Count != 36U) {
                    AmbaPrint_ModulePrintStr5(SVC_MUXER_PRINT_MODULE_ID, "%s Stream->Func->Write() failed!", __func__, NULL, NULL, NULL, NULL);
                    Rval = FORMAT_ERR_IO_ERROR;
                }
            } else {
                AmbaPrint_ModulePrintStr5(SVC_MUXER_PRINT_MODULE_ID, "%s Stream->Func->Write() failed!", __func__, NULL, NULL, NULL, NULL);
            }
        } else {
            AmbaPrint_ModulePrintStr5(SVC_MUXER_PRINT_MODULE_ID, "%s Not supported!", __func__, NULL, NULL, NULL, NULL);
            Rval = FORMAT_ERR_INVALID_ARG;
        }
    }
    return Rval;
}

static UINT32 IsoMux_GMHD_Init(ISO_GMHD_BOX_s *Gmhd, UINT8 TrackType)
{
    UINT32 Rval = FORMAT_OK;
    Gmhd->Box.Size = 8;
    Gmhd->Box.Type = TAG_GMHD;
    Gmhd->GminAtom.FullBox.Box.Size = 24;
    Gmhd->GminAtom.FullBox.Box.Type = TAG_GMIN;
    Gmhd->GminAtom.FullBox.Version = 0;
    Gmhd->GminAtom.FullBox.Flags[0] = 0;
    Gmhd->GminAtom.FullBox.Flags[1] = 0;
    Gmhd->GminAtom.FullBox.Flags[2] = 0;
    Gmhd->GminAtom.GraphicMode = 64;
    Gmhd->GminAtom.Opcolor[0] = 0x8000;
    Gmhd->GminAtom.Opcolor[1] = 0x8000;
    Gmhd->GminAtom.Opcolor[2] = 0x8000;
    Gmhd->GminAtom.Balanced = 0;
    Gmhd->Box.Size += Gmhd->GminAtom.FullBox.Box.Size;
    if (TrackType == SVC_FORMAT_MID_TEXT) {
        Gmhd->HdlrAtom.TextHdlr.Box.Size = 44;
        Gmhd->HdlrAtom.TextHdlr.Box.Type = TAG_TEXT;
        Gmhd->Box.Size += Gmhd->HdlrAtom.TextHdlr.Box.Size;
    } else {
        AmbaPrint_ModulePrintStr5(SVC_MUXER_PRINT_MODULE_ID, "%s Not supported!", __func__, NULL, NULL, NULL, NULL);
        Rval = FORMAT_ERR_INVALID_ARG;
    }
    return Rval;
}

static UINT32 IsoMux_NMHD_Write(const ISO_NMHD_BOX_s *Nmhd, SVC_STREAM_HDLR_s *Stream)
{
    UINT32 Rval;
    /* Nmhd.FullBox.Box.Size */
    Rval = SvcFormat_PutBe32(Stream, Nmhd->FullBox.Box.Size);
    if (Rval == FORMAT_OK) {
        /* Nmhd.FullBox.Box.Type */
        Rval = SvcFormat_PutBe32(Stream, Nmhd->FullBox.Box.Type);
    }
    if (Rval == FORMAT_OK) {
        /* Nmhd.FullBox.Version */
        Rval = SvcFormat_PutByte(Stream, Nmhd->FullBox.Version);
    }
    if (Rval == FORMAT_OK) {
        /* Nmhd.FullBox.Flags[0] */
        Rval = SvcFormat_PutByte(Stream, Nmhd->FullBox.Flags[0]);
    }
    if (Rval == FORMAT_OK) {
        /* Nmhd.FullBox.Flags[1] */
        Rval = SvcFormat_PutByte(Stream, Nmhd->FullBox.Flags[1]);
    }
    if (Rval == FORMAT_OK) {
        /* Nmhd.FullBox.Flags[2] */
        Rval = SvcFormat_PutByte(Stream, Nmhd->FullBox.Flags[2]);
    }
    return Rval;
}

static void IsoMux_NMHD_Init(ISO_NMHD_BOX_s *Nmhd)
{
    Nmhd->FullBox.Box.Size = 12;
    Nmhd->FullBox.Box.Type = TAG_NMHD;
    Nmhd->FullBox.Version = 0;
    Nmhd->Balanced = 0;
    Nmhd->Reserv2 = 0;
}

static UINT32 IsoMux_VideoMINF_Write(ISO_MINF_BOX_s *Minf, SVC_STREAM_HDLR_s *Stream, const SVC_VIDEO_TRACK_INFO_s *Track)
{
    UINT32 Rval;
    /* Minf.Box.Size */
    Rval = SvcFormat_PutBe32(Stream, Minf->Box.Size);
    if (Rval == FORMAT_OK) {
        /* Minf.Box.Type */
        Rval = SvcFormat_PutBe32(Stream, Minf->Box.Type);
    }
    /* Write VMHD BOX*/
    if (Rval == FORMAT_OK) {
        Rval = IsoMux_VMHD_Write(&Minf->MediaAtom.VmhdAtom, Stream);
        if (Rval != FORMAT_OK) {
            AmbaPrint_ModulePrintStr5(SVC_MUXER_PRINT_MODULE_ID, "%s IsoMux_VMHD_Write() error!", __func__, NULL, NULL, NULL, NULL);
        }
    }
    if (Rval == FORMAT_OK) {
        Minf->Box.Size += Minf->MediaAtom.VmhdAtom.FullBox.Box.Size;
        /* Write DINF BOX*/
        Rval = IsoMux_DINF_Write(&Minf->DinfAtom, Stream);
        if (Rval != FORMAT_OK) {
            AmbaPrint_ModulePrintStr5(SVC_MUXER_PRINT_MODULE_ID, "%s IsoMux_DINF_Write() error!", __func__, NULL, NULL, NULL, NULL);
        }
    }
    if (Rval == FORMAT_OK) {
        Minf->Box.Size += Minf->DinfAtom.Box.Size;
        /* Write Video STBL BOX*/
        Rval = IsoMux_VideoSTBL_Write(&Minf->StblAtom.VideoStbl1, Stream, Track);
        if (Rval != FORMAT_OK) {
            AmbaPrint_ModulePrintStr5(SVC_MUXER_PRINT_MODULE_ID, "%s IsoMux_VideoSTBL_Write() error!", __func__, NULL, NULL, NULL, NULL);
        }
    }
    if (Rval == FORMAT_OK) {
        Minf->Box.Size += Minf->StblAtom.VideoStbl1.Box.Size;
    }

    return Rval;
}

static UINT32 IsoMux_AudioMINF_Write(ISO_MINF_BOX_s *Minf, SVC_STREAM_HDLR_s *Stream, const SVC_AUDIO_TRACK_INFO_s *Track)
{
    UINT32 Rval;
    /* Minf.Box.Size */
    Rval = SvcFormat_PutBe32(Stream, Minf->Box.Size);
    if (Rval == FORMAT_OK) {
        /* Minf.Box.Type */
        Rval = SvcFormat_PutBe32(Stream, Minf->Box.Type);
    }
    /* Write SMHD Box */
    if (Rval == FORMAT_OK) {
        Rval = IsoMux_SMHD_Write(&Minf->MediaAtom.SmhdAtom, Stream);
    }
    if (Rval == FORMAT_OK) {
        Minf->Box.Size += Minf->MediaAtom.SmhdAtom.FullBox.Box.Size;
        /* Write DINF BOX*/
        Rval = IsoMux_DINF_Write(&Minf->DinfAtom, Stream);
    }
    if (Rval == FORMAT_OK) {
        Minf->Box.Size += Minf->DinfAtom.Box.Size;
        /* Write Audio STBL BOX*/
        Rval = IsoMux_AudioSTBL_Write(&Minf->StblAtom.AudioStbl, Stream, Track);
        Minf->Box.Size += Minf->StblAtom.AudioStbl.Box.Size;
    }
    return Rval;
}

static UINT32 IsoMux_TextMINF_Write(ISO_MINF_BOX_s *Minf, SVC_STREAM_HDLR_s *Stream, SVC_TEXT_TRACK_INFO_s *Text)
{
    const SVC_MEDIA_TRACK_GENERAL_INFO_s * Track = &Text->Info;
    UINT32 Rval;
    /* Minf.Box.Size */
    Rval = SvcFormat_PutBe32(Stream, Minf->Box.Size);
    if (Rval == FORMAT_OK) {
        /* Minf.Box.Type */
        Rval = SvcFormat_PutBe32(Stream, Minf->Box.Type);
    }
    if (Rval == FORMAT_OK) {
        if (Track->MediaId == SVC_FORMAT_MID_TEXT) {
            /* Write GMHD Box */
            Rval = IsoMux_GMHD_Write(&Minf->MediaAtom.GmhdAtom, Stream, Track->MediaId);
            if (Rval != FORMAT_OK) {
                AmbaPrint_ModulePrintStr5(SVC_MUXER_PRINT_MODULE_ID, "%s IsoMux_GMHD_Write() error!", __func__, NULL, NULL, NULL, NULL);
            } else {
                Minf->Box.Size += Minf->MediaAtom.GmhdAtom.Box.Size;
            }
        } else if (Track->MediaId == SVC_FORMAT_MID_MP4S) {
            /* Write NMHD Box */
            Rval = IsoMux_NMHD_Write(&Minf->MediaAtom.NmhdAtom, Stream);
            if (Rval != FORMAT_OK) {
                AmbaPrint_ModulePrintStr5(SVC_MUXER_PRINT_MODULE_ID, "%s IsoMux_GMHD_Write() error!", __func__, NULL, NULL, NULL, NULL);
            } else {
                Minf->Box.Size += Minf->MediaAtom.NmhdAtom.FullBox.Box.Size;
            }
        } else {
            AmbaPrint_ModulePrintStr5(SVC_MUXER_PRINT_MODULE_ID, "%s Incorrect media ID!", __func__, NULL, NULL, NULL, NULL);
            Rval = FORMAT_ERR_INVALID_ARG;
        }
    }
    /* Write DINF BOX*/
    if (Rval == FORMAT_OK) {
        Rval = IsoMux_DINF_Write(&Minf->DinfAtom, Stream);
        if (Rval != FORMAT_OK) {
            AmbaPrint_ModulePrintStr5(SVC_MUXER_PRINT_MODULE_ID, "%s IsoMux_DINF_Write() error!", __func__, NULL, NULL, NULL, NULL);
        }
    }
    if (Rval == FORMAT_OK) {
        Minf->Box.Size += Minf->DinfAtom.Box.Size;
        /* Write Text STBL BOX*/
        Rval = IsoMux_TextSTBL_Write(&Minf->StblAtom.TextStbl, Stream, Text);
        if (Rval != FORMAT_OK) {
            AmbaPrint_ModulePrintStr5(SVC_MUXER_PRINT_MODULE_ID, "%s IsoMux_TextSTBL_Write() error!", __func__, NULL, NULL, NULL, NULL);
        }
    }
    if (Rval == FORMAT_OK) {
        Minf->Box.Size += Minf->StblAtom.TextStbl.Box.Size;
    }

    return Rval;
}

static UINT32 IsoMux_VideoMINF_Init(ISO_MINF_BOX_s *Minf, const SVC_VIDEO_TRACK_INFO_s *Track, UINT8 TrackIdx, const SVC_ISO_PRIV_INFO_s *PrivInfo, UINT8 Dummy)
{
    UINT32 Rval;
    const SVC_ISO_VIDEO_TRACK_INFO_s * const VideoInfo = &PrivInfo->VideoTrack[TrackIdx];
    Minf->Box.Size = 8;
    Minf->Box.Type = TAG_MINF;
    /* Initial VMHD BOX*/
    IsoMux_VMHD_Init(&Minf->MediaAtom.VmhdAtom);
    Minf->Box.Size += Minf->MediaAtom.VmhdAtom.FullBox.Box.Size;
    /* Initial Video STBL BOX*/
    Rval = IsoMux_VideoSTBL_Init(&Minf->StblAtom.VideoStbl1, Track, VideoInfo, Dummy);
    if (Rval == FORMAT_OK) {
        Minf->Box.Size += Minf->StblAtom.VideoStbl1.Box.Size;

        /* Initial DINF BOX*/
        IsoMux_DINF_Init(&Minf->DinfAtom, SVC_MEDIA_TRACK_TYPE_TEXT);
        Minf->Box.Size += Minf->DinfAtom.Box.Size;
    }
    return Rval;
}

static UINT32 IsoMux_AudioMINF_Init(ISO_MINF_BOX_s *Minf, const SVC_AUDIO_TRACK_INFO_s *Track, UINT8 TrackIdx, const SVC_ISO_PRIV_INFO_s *PrivInfo, UINT8 Dummy)
{
    UINT32 Rval;
    const SVC_ISO_AUDIO_TRACK_INFO_s *AudioInfo = &PrivInfo->AudioTrack[TrackIdx];
    Minf->Box.Size = 8;
    Minf->Box.Type = TAG_MINF;
    /* Initial SMHD BOX */
    IsoMux_SMHD_Init(&Minf->MediaAtom.SmhdAtom);
    Minf->Box.Size += Minf->MediaAtom.SmhdAtom.FullBox.Box.Size;
    /* Initial Audio STBL BOX */
    Rval = IsoMux_AudioSTBL_Init(&Minf->StblAtom.AudioStbl, Track, AudioInfo, Dummy);
    if (Rval == FORMAT_OK) {
        Minf->Box.Size += Minf->StblAtom.AudioStbl.Box.Size;

        /* Initial DINF BOX*/
        IsoMux_DINF_Init(&Minf->DinfAtom, SVC_MEDIA_TRACK_TYPE_AUDIO);
        Minf->Box.Size += Minf->DinfAtom.Box.Size;
    }
    return Rval;
}
static UINT32 IsoMux_TextMINF_Init(ISO_MINF_BOX_s *Minf, const SVC_TEXT_TRACK_INFO_s *Track, UINT8 TrackIdx, const SVC_ISO_PRIV_INFO_s *PrivInfo, UINT8 Dummy)
{
    UINT32 Rval = FORMAT_OK;
    const SVC_ISO_TEXT_TRACK_INFO_s *TextInfo = &PrivInfo->TextTrack[TrackIdx];
    Minf->Box.Size = 8;
    Minf->Box.Type = TAG_MINF;
    if (Track->Info.MediaId == SVC_FORMAT_MID_TEXT) {
        /* Initial GMHD BOX */
        Rval = IsoMux_GMHD_Init(&Minf->MediaAtom.GmhdAtom, Track->Info.MediaId);
        if (Rval == FORMAT_OK) {
            Minf->Box.Size += Minf->MediaAtom.GmhdAtom.Box.Size;
        }
    } else if (Track->Info.MediaId == SVC_FORMAT_MID_MP4S) {
        /* Initial NMHD BOX */
        IsoMux_NMHD_Init(&Minf->MediaAtom.NmhdAtom);
        Minf->Box.Size += Minf->MediaAtom.NmhdAtom.FullBox.Box.Size;
    } else {
        AmbaPrint_ModulePrintStr5(SVC_MUXER_PRINT_MODULE_ID, "%s Incorrect media ID!", __func__, NULL, NULL, NULL, NULL);
        Rval = FORMAT_ERR_INVALID_ARG;
    }
    /* Initial Text STBL BOX */
    if (Rval == FORMAT_OK) {
        Rval = IsoMux_TextSTBL_Init(&Minf->StblAtom.TextStbl, Track, TextInfo, Dummy);
    }
    if (Rval == FORMAT_OK) {
        Minf->Box.Size += Minf->StblAtom.TextStbl.Box.Size;

        /* Initial DINF BOX*/
        IsoMux_DINF_Init(&Minf->DinfAtom, SVC_MEDIA_TRACK_TYPE_TEXT);
        Minf->Box.Size += Minf->DinfAtom.Box.Size;
    }
    return Rval;
}
static UINT32 IsoMux_HDLR_Write(ISO_HDLR_BOX_s *Hdlr, SVC_STREAM_HDLR_s *Stream)
{
    UINT32 Rval;
    /* Hdlr.FullBox.Box.Size */
    Rval = SvcFormat_PutBe32(Stream, Hdlr->FullBox.Box.Size);
    if (Rval == FORMAT_OK) {
        /* Hdlr.FullBox.Box.Type */
        Rval = SvcFormat_PutBe32(Stream, Hdlr->FullBox.Box.Type);
    }
    if (Rval == FORMAT_OK) {
        /* Hdlr.FullBox.Version */
        Rval = SvcFormat_PutByte(Stream, Hdlr->FullBox.Version);
    }
    if (Rval == FORMAT_OK) {
        /* Hdlr.FullBox.Flags[0] */
        Rval = SvcFormat_PutByte(Stream, Hdlr->FullBox.Flags[0]);
    }
    if (Rval == FORMAT_OK) {
        /* Hdlr.FullBox.Flags[1] */
        Rval = SvcFormat_PutByte(Stream, Hdlr->FullBox.Flags[1]);
    }
    if (Rval == FORMAT_OK) {
        /* Hdlr.FullBox.Flags[2] */
        Rval = SvcFormat_PutByte(Stream, Hdlr->FullBox.Flags[2]);
    }
    if (Rval == FORMAT_OK) {
        /* Hdlr.ComType */
        Rval = SvcFormat_PutBe32(Stream, Hdlr->ComType);
    }
    if (Rval == FORMAT_OK) {
        /* Hdlr.ComSubType */
        Rval = SvcFormat_PutBe32(Stream, Hdlr->ComSubType);
    }
    if (Rval == FORMAT_OK) {
        /* Hdlr.ComManufacturer */
        Rval = SvcFormat_PutBe32(Stream, Hdlr->ComManufacturer);
    }
    if (Rval == FORMAT_OK) {
        /* Hdlr.ComFlags */
        Rval = SvcFormat_PutBe32(Stream, Hdlr->ComFlags);
    }
    if (Rval == FORMAT_OK) {
        /* Hdlr.ComFlagMask */
        Rval = SvcFormat_PutBe32(Stream, Hdlr->ComFlagMask);
    }
    if (Rval == FORMAT_OK) {
        UINT32 Count;
        Rval = S2F(Stream->Func->Write(Stream, ISO_HDLR_MAX_COM_NAME_LEN, (UINT8 *)Hdlr->ComName, &Count));
        if (Rval == FORMAT_OK) {
            if (Count != ISO_HDLR_MAX_COM_NAME_LEN) {
                AmbaPrint_ModulePrintStr5(SVC_MUXER_PRINT_MODULE_ID, "%s Stream->Func->Write() failed!", __func__, NULL, NULL, NULL, NULL);
                Rval = FORMAT_ERR_IO_ERROR;
            }
        } else {
            AmbaPrint_ModulePrintStr5(SVC_MUXER_PRINT_MODULE_ID, "%s Stream->Func->Write() failed!", __func__, NULL, NULL, NULL, NULL);
        }
    }
    return Rval;
}

static UINT32 IsoMux_HDLR_Init(ISO_HDLR_BOX_s *Hdlr, UINT32 TrackType, UINT32 MediaId)
{
    UINT32 Rval = FORMAT_OK;
    const char VideoComName[ISO_HDLR_MAX_COM_NAME_LEN] = {(char)13, 'A', 'm', 'b', 'a', 'r', 'e', 'l', 'l', 'a', ' ', 'A', 'V', 'C'};
    const char AudioComName[ISO_HDLR_MAX_COM_NAME_LEN] = {(char)13, 'A', 'm', 'b', 'a', 'r', 'e', 'l', 'l', 'a', ' ', 'A', 'A', 'C'};
    const char TextComName[ISO_HDLR_MAX_COM_NAME_LEN] = {(char)13, 'A', 'm', 'b', 'a', 'r', 'e', 'l', 'l', 'a', ' ', 'E', 'X', 'T'};
    Hdlr->FullBox.Box.Size = 32;
    Hdlr->FullBox.Box.Type = TAG_HDLR;
    if (TrackType == SVC_MEDIA_TRACK_TYPE_VIDEO) {
        AmbaUtility_StringCopy(Hdlr->ComName, ISO_HDLR_MAX_COM_NAME_LEN, VideoComName);   // no need to ensure NULL terminated
        Hdlr->ComSubType = TAG_VIDE;
    } else if (TrackType == SVC_MEDIA_TRACK_TYPE_AUDIO) {
        AmbaUtility_StringCopy(Hdlr->ComName, ISO_HDLR_MAX_COM_NAME_LEN, AudioComName);   // no need to ensure NULL terminated
        Hdlr->ComSubType = TAG_SOUN;
    } else {
        AmbaUtility_StringCopy(Hdlr->ComName, ISO_HDLR_MAX_COM_NAME_LEN, TextComName);    // no need to ensure NULL terminated
        if (MediaId == SVC_FORMAT_MID_MP4S) {
            Hdlr->ComSubType = TAG_SDSM;
        } else {
            Hdlr->ComSubType = TAG_TEXT;
        }
    }
    Hdlr->FullBox.Box.Size += ISO_HDLR_MAX_COM_NAME_LEN;
    return Rval;
}

static UINT32 IsoMux_MDHD_Write(const ISO_MDHD_BOX_s *Mdhd, SVC_STREAM_HDLR_s *Stream)
{
    UINT32 Rval;
    /* Mdhd.FullBox.Box.Size */
    Rval = SvcFormat_PutBe32(Stream, Mdhd->FullBox.Box.Size);
    if (Rval == FORMAT_OK) {
        /* Mdhd.FullBox.Box.Type */
        Rval = SvcFormat_PutBe32(Stream, Mdhd->FullBox.Box.Type);
    }
    if (Rval == FORMAT_OK) {
        /* Mdhd.FullBox.Version */
        Rval = SvcFormat_PutByte(Stream, Mdhd->FullBox.Version);
    }
    if (Rval == FORMAT_OK) {
        /* Mdhd.FullBox.Flags[0] */
        Rval = SvcFormat_PutByte(Stream, Mdhd->FullBox.Flags[0]);
    }
    if (Rval == FORMAT_OK) {
        /* Mdhd.FullBox.Flags[1] */
        Rval = SvcFormat_PutByte(Stream, Mdhd->FullBox.Flags[1]);
    }
    if (Rval == FORMAT_OK) {
        /* Mdhd.FullBox.Flags[2] */
        Rval = SvcFormat_PutByte(Stream, Mdhd->FullBox.Flags[2]);
    }
    if (Rval == FORMAT_OK) {
        /* Mdhd.CreateTime */
        Rval = SvcFormat_PutBe32(Stream, Mdhd->CreateTime);
    }
    if (Rval == FORMAT_OK) {
        /* Mdhd.ModifyTime */
        Rval = SvcFormat_PutBe32(Stream, Mdhd->ModifyTime);
    }
    if (Rval == FORMAT_OK) {
        /* Mdhd.TimeScale */
        Rval = SvcFormat_PutBe32(Stream, Mdhd->TimeScale);
    }
    if (Rval == FORMAT_OK) {
        /* Mdhd.Duration */
        Rval = SvcFormat_PutBe32(Stream, Mdhd->Duration);
    }
    if (Rval == FORMAT_OK) {
        /* Mdhd.Language */
        Rval = SvcFormat_PutBe16(Stream, Mdhd->Language);
    }
    if (Rval == FORMAT_OK) {
        /* Mdhd.Quality */
        Rval = SvcFormat_PutBe16(Stream, Mdhd->Quality);
    }
    return Rval;
}

static UINT32 IsoMux_MDHD_Init(ISO_MDHD_BOX_s *Mdhd, const SVC_MEDIA_TRACK_GENERAL_INFO_s *Track, const SVC_MEDIA_INFO_s *Media)
{
    Mdhd->FullBox.Box.Size = 32;
    Mdhd->FullBox.Box.Type = TAG_MDHD;
    Mdhd->CreateTime = Media->CreationTime;
    Mdhd->ModifyTime = Media->ModificationTime;
    Mdhd->Duration = (UINT32)NORMALIZE_TO_TIMESCALE(Track->TimeScale, Track->NextDTS - Track->InitDTS, Track->OrigTimeScale);
    Mdhd->TimeScale = Track->OrigTimeScale;
    return FORMAT_OK;
}

static UINT32 IsoMux_VideoMDIA_Write(ISO_MDIA_BOX_s *Mdia, SVC_STREAM_HDLR_s *Stream, const SVC_VIDEO_TRACK_INFO_s *Track)
{
    UINT32 Rval;
    /* Mdia.FullBox.Box.Size */
    Rval = SvcFormat_PutBe32(Stream, Mdia->Box.Size);
    if (Rval == FORMAT_OK) {
        /* Mdia.FullBox.Box.Type */
        Rval = SvcFormat_PutBe32(Stream, Mdia->Box.Type);
    }
    /* Write MDHD BOX*/
    if (Rval == FORMAT_OK) {
        Rval = IsoMux_MDHD_Write(&Mdia->MdhdAtom, Stream);
    }
    /* Write HDLR BOX*/
    if (Rval == FORMAT_OK) {
        Rval = IsoMux_HDLR_Write(&Mdia->HdlrAtom, Stream);
    }
    /* Write MINF BOX*/
    if (Rval == FORMAT_OK) {
        Rval = IsoMux_VideoMINF_Write(&Mdia->MinfAtom, Stream, Track);
    }
    return Rval;
}

static UINT32 IsoMux_AudioMDIA_Write(ISO_MDIA_BOX_s *Mdia, SVC_STREAM_HDLR_s *Stream, const SVC_AUDIO_TRACK_INFO_s *Track)
{
    UINT32 Rval = FORMAT_OK;
    /* Mdia.FullBox.Box.Size */
    Rval = SvcFormat_PutBe32(Stream, Mdia->Box.Size);
    if (Rval == FORMAT_OK) {
        /* Mdia.FullBox.Box.Type */
        Rval = SvcFormat_PutBe32(Stream, Mdia->Box.Type);
    }
    /* Write MDHD BOX*/
    if (Rval == FORMAT_OK) {
        Rval = IsoMux_MDHD_Write(&Mdia->MdhdAtom, Stream);
        if (Rval != FORMAT_OK) {
            AmbaPrint_ModulePrintStr5(SVC_MUXER_PRINT_MODULE_ID, "%s IsoMux_MDHD_Write() error!", __func__, NULL, NULL, NULL, NULL);
        }
    }
    /* Write HDLR BOX*/
    if (Rval == FORMAT_OK) {
        Rval = IsoMux_HDLR_Write(&Mdia->HdlrAtom, Stream);
        if (Rval != FORMAT_OK) {
            AmbaPrint_ModulePrintStr5(SVC_MUXER_PRINT_MODULE_ID, "%s IsoMux_HDLR_Write() error!", __func__, NULL, NULL, NULL, NULL);
        }
    }
    /* Write MINF BOX*/
    if (Rval == FORMAT_OK) {
        Rval = IsoMux_AudioMINF_Write(&Mdia->MinfAtom, Stream, Track);
        if (Rval != FORMAT_OK) {
            AmbaPrint_ModulePrintStr5(SVC_MUXER_PRINT_MODULE_ID, "%s IsoMux_MINF_Write() error!", __func__, NULL, NULL, NULL, NULL);
        }
    }
    return Rval;
}

static UINT32 IsoMux_TextMDIA_Write(ISO_MDIA_BOX_s *Mdia, SVC_STREAM_HDLR_s *Stream, SVC_TEXT_TRACK_INFO_s *Track)
{
    UINT32 Rval;
    /* Mdia.FullBox.Box.Size */
    Rval = SvcFormat_PutBe32(Stream, Mdia->Box.Size);
    if (Rval == FORMAT_OK) {
        /* Mdia.FullBox.Box.Type */
        Rval = SvcFormat_PutBe32(Stream, Mdia->Box.Type);
    }
    /* Write MDHD BOX*/
    if (Rval == FORMAT_OK) {
        Rval = IsoMux_MDHD_Write(&Mdia->MdhdAtom, Stream);
        if (Rval != FORMAT_OK) {
            AmbaPrint_ModulePrintStr5(SVC_MUXER_PRINT_MODULE_ID, "%s IsoMux_MDHD_Write() error!", __func__, NULL, NULL, NULL, NULL);
        }
    }
    /* Write HDLR BOX*/
    if (Rval == FORMAT_OK) {
        Rval = IsoMux_HDLR_Write(&Mdia->HdlrAtom, Stream);
        if (Rval != FORMAT_OK) {
            AmbaPrint_ModulePrintStr5(SVC_MUXER_PRINT_MODULE_ID, "%s IsoMux_HDLR_Write() error!", __func__, NULL, NULL, NULL, NULL);
        }
    }
    /* Write MINF BOX*/
    if (Rval == FORMAT_OK) {
        Rval = IsoMux_TextMINF_Write(&Mdia->MinfAtom, Stream, Track);
        if (Rval != FORMAT_OK) {
            AmbaPrint_ModulePrintStr5(SVC_MUXER_PRINT_MODULE_ID, "%s IsoMux_MINF_Write() error!", __func__, NULL, NULL, NULL, NULL);
        }
    }
    return Rval;
}

static UINT32 IsoMux_VideoMDIA_Init(ISO_MDIA_BOX_s *Mdia, const SVC_VIDEO_TRACK_INFO_s *TrackInfo, UINT8 TrackIdx, const SVC_MEDIA_INFO_s *Media, const SVC_ISO_PRIV_INFO_s *PrivInfo, UINT8 Dummy)
{
    UINT32 Rval;
    Mdia->Box.Size = 8;
    Mdia->Box.Type = TAG_MDIA;
    /* Initial MDHD BOX*/
    Rval = IsoMux_MDHD_Init(&Mdia->MdhdAtom, &TrackInfo->Info, Media);
    if (Rval != FORMAT_OK) {
        AmbaPrint_ModulePrintStr5(SVC_MUXER_PRINT_MODULE_ID, "%s IsoMux_MDHD_Init() error!", __func__, NULL, NULL, NULL, NULL);
    } else {
        Mdia->Box.Size += Mdia->MdhdAtom.FullBox.Box.Size;
        /* Initial HDLR BOX*/
        Rval = IsoMux_HDLR_Init(&Mdia->HdlrAtom, SVC_MEDIA_TRACK_TYPE_VIDEO, TrackInfo->Info.MediaId);
        if (Rval != FORMAT_OK) {
            AmbaPrint_ModulePrintStr5(SVC_MUXER_PRINT_MODULE_ID, "%s IsoMux_HDLR_Init() error!", __func__, NULL, NULL, NULL, NULL);
        }
    }
    if (Rval == FORMAT_OK) {
        Mdia->Box.Size += Mdia->HdlrAtom.FullBox.Box.Size;
        /* Initial MINF BOX*/
        Rval = IsoMux_VideoMINF_Init(&Mdia->MinfAtom, TrackInfo, TrackIdx, PrivInfo, Dummy);
        if (Rval != FORMAT_OK) {
            AmbaPrint_ModulePrintStr5(SVC_MUXER_PRINT_MODULE_ID, "%s IsoMux_MINF_Init() error!", __func__, NULL, NULL, NULL, NULL);
        }
    }
    if (Rval == FORMAT_OK) {
        Mdia->Box.Size += Mdia->MinfAtom.Box.Size;
    }
    return Rval;
}

static UINT32 IsoMux_AudioMDIA_Init(ISO_MDIA_BOX_s *Mdia, const SVC_AUDIO_TRACK_INFO_s *TrackInfo, UINT8 TrackIdx, const SVC_MEDIA_INFO_s *Media, const SVC_ISO_PRIV_INFO_s *PrivInfo, UINT8 Dummy)
{
    UINT32 Rval;
    Mdia->Box.Size = 8;
    Mdia->Box.Type = TAG_MDIA;
    /* Initial MDHD BOX*/
    Rval = IsoMux_MDHD_Init(&Mdia->MdhdAtom, &TrackInfo->Info, Media);
    if (Rval != FORMAT_OK) {
        AmbaPrint_ModulePrintStr5(SVC_MUXER_PRINT_MODULE_ID, "%s IsoMux_MDHD_Init() error!", __func__, NULL, NULL, NULL, NULL);
    } else {
        Mdia->Box.Size += Mdia->MdhdAtom.FullBox.Box.Size;
        /* Initial HDLR BOX*/
        Rval = IsoMux_HDLR_Init(&Mdia->HdlrAtom, SVC_MEDIA_TRACK_TYPE_AUDIO, TrackInfo->Info.MediaId);
        if (Rval != FORMAT_OK) {
            AmbaPrint_ModulePrintStr5(SVC_MUXER_PRINT_MODULE_ID, "%s IsoMux_HDLR_Init() error!", __func__, NULL, NULL, NULL, NULL);
        }
    }
    if (Rval == FORMAT_OK) {
        Mdia->Box.Size += Mdia->HdlrAtom.FullBox.Box.Size;
        /* Initial MINF BOX*/
        Rval = IsoMux_AudioMINF_Init(&Mdia->MinfAtom, TrackInfo, TrackIdx, PrivInfo, Dummy);
        if (Rval != FORMAT_OK) {
            AmbaPrint_ModulePrintStr5(SVC_MUXER_PRINT_MODULE_ID, "%s IsoMux_MINF_Init() error!", __func__, NULL, NULL, NULL, NULL);
        }
    }
    if (Rval == FORMAT_OK) {
        Mdia->Box.Size += Mdia->MinfAtom.Box.Size;
    }
    return Rval;
}

static UINT32 IsoMux_TextMDIA_Init(ISO_MDIA_BOX_s *Mdia, const SVC_TEXT_TRACK_INFO_s *TrackInfo, UINT8 TrackIdx, const SVC_MEDIA_INFO_s *Media, const SVC_ISO_PRIV_INFO_s *PrivInfo, UINT8 Dummy)
{
    UINT32 Rval;
    Mdia->Box.Size = 8;
    Mdia->Box.Type = TAG_MDIA;
    /* Initial MDHD BOX*/
    Rval = IsoMux_MDHD_Init(&Mdia->MdhdAtom, &TrackInfo->Info, Media);
    if (Rval != FORMAT_OK) {
        AmbaPrint_ModulePrintStr5(SVC_MUXER_PRINT_MODULE_ID, "%s IsoMux_MDHD_Init() error!", __func__, NULL, NULL, NULL, NULL);
    } else {
        Mdia->Box.Size += Mdia->MdhdAtom.FullBox.Box.Size;
        /* Initial HDLR BOX*/
        Rval = IsoMux_HDLR_Init(&Mdia->HdlrAtom, SVC_MEDIA_TRACK_TYPE_TEXT, TrackInfo->Info.MediaId);
        if (Rval != FORMAT_OK) {
            AmbaPrint_ModulePrintStr5(SVC_MUXER_PRINT_MODULE_ID, "%s IsoMux_HDLR_Init() error!", __func__, NULL, NULL, NULL, NULL);
        }
    }
    if (Rval == FORMAT_OK) {
        Mdia->Box.Size += Mdia->HdlrAtom.FullBox.Box.Size;
        /* Initial MINF BOX*/
        Rval = IsoMux_TextMINF_Init(&Mdia->MinfAtom, TrackInfo, TrackIdx, PrivInfo, Dummy);
        if (Rval != FORMAT_OK) {
            AmbaPrint_ModulePrintStr5(SVC_MUXER_PRINT_MODULE_ID, "%s IsoMux_MINF_Init() error!", __func__, NULL, NULL, NULL, NULL);
        }
    }
    if (Rval == FORMAT_OK) {
        Mdia->Box.Size += Mdia->MinfAtom.Box.Size;
    }
    return Rval;
}

/* refer to ffmpeg to remove edts box.
 * fix video double length in QT.
 * duration also need to set 0.
 */
#if 0
static UINT32 IsoMux_ELST_Write(const ISO_ELST_BOX_s *elst, SVC_STREAM_HDLR_s *stream)
{
    UINT32 Rval;
    /* elst.FullBox.Box.Size */
    Rval = SvcFormat_PutBe32(stream, elst->FullBox.Box.Size);
    if (Rval == FORMAT_OK) {
        /* elst.FullBox.Box.Type */
        Rval = SvcFormat_PutBe32(stream, elst->FullBox.Box.Type);
    }
    if (Rval == FORMAT_OK) {
        /* elst.FullBox.Version */
        Rval = SvcFormat_PutByte(stream, elst->FullBox.Version);
    }
    if (Rval == FORMAT_OK) {
        /* elst.FullBox.nFlag[0] */
        Rval = SvcFormat_PutByte(stream, elst->FullBox.Flags[0]);
    }
    if (Rval == FORMAT_OK) {
        /* elst.FullBox.nFlag[1] */
        Rval = SvcFormat_PutByte(stream, elst->FullBox.Flags[1]);
    }
    if (Rval == FORMAT_OK) {
        /* elst.FullBox.nFlag[2] */
        Rval = SvcFormat_PutByte(stream, elst->FullBox.Flags[2]);
    }
    if (Rval == FORMAT_OK) {
        /* elst.EntryCount */
        Rval = SvcFormat_PutBe32(stream, elst->EntryCount);
    }
    if ((elst->EntryCount > 1U) && (Rval == FORMAT_OK)) {
        /* elst.ElstTable.TrackDuration */
        Rval = SvcFormat_PutBe32(stream, elst->ElstTable1.SegmentDuration);
        if (Rval == FORMAT_OK) {
            /* elst.ElstTable.MediaTime */
            Rval = SvcFormat_PutBe32(stream, elst->ElstTable1.MediaTime);
        }
        if (Rval == FORMAT_OK) {
            /* elst.ElstTable.MediaRate */
            Rval = SvcFormat_PutBe32(stream, elst->ElstTable1.MediaRate);
        }
    }
    if (Rval == FORMAT_OK) {
        /* elst.ElstTable.TrackDuration */
        Rval = SvcFormat_PutBe32(stream, elst->ElstTable2.SegmentDuration);
    }
    if (Rval == FORMAT_OK) {
        /* elst.ElstTable.MediaTime */
        Rval = SvcFormat_PutBe32(stream, elst->ElstTable2.MediaTime);
    }
    if (Rval == FORMAT_OK) {
        /* elst.ElstTable.MediaRate */
        Rval = SvcFormat_PutBe32(stream, elst->ElstTable2.MediaRate);
    }
    return Rval;
}

static void IsoMux_ELST_Init(ISO_ELST_BOX_s *elst, UINT32 emptyEdit, UINT32 segmentDuration)
{
    elst->FullBox.Box.Size = 16;
    elst->FullBox.Box.Type = TAG_ELST;
    elst->EntryCount = 1;
    elst->ElstTable2.SegmentDuration = segmentDuration;
    elst->ElstTable2.MediaTime = 0;
    elst->ElstTable2.MediaRate = 0x10000;
    if (emptyEdit != 0U) {
        elst->EntryCount++;
        elst->ElstTable1.SegmentDuration = emptyEdit;
        elst->ElstTable1.MediaTime = 0;
        elst->ElstTable1.MediaRate = 0x10000;
    }
    elst->FullBox.Box.Size += elst->EntryCount * 12U;
}

static UINT32 IsoMux_EDTS_Write(const ISO_EDTS_BOX_s *edts, SVC_STREAM_HDLR_s *stream)
{
    UINT32 Rval;
    /* edts.Box.Size */
    Rval = SvcFormat_PutBe32(stream, edts->Box.Size);
    if (Rval == FORMAT_OK) {
        /* edts.Box.Type */
        Rval = SvcFormat_PutBe32(stream, edts->Box.Type);
    }
    /* Write ELST BOX*/
    if (Rval == FORMAT_OK) {
        Rval = IsoMux_ELST_Write(&edts->ElstAtom, stream);
        if (Rval != FORMAT_OK) {
            AmbaPrint_ModulePrintStr5(SVC_MUXER_PRINT_MODULE_ID, "%s IsoMux_ELST_Write() error!", __func__, NULL, NULL, NULL, NULL);
        }
    }
    return Rval;
}

static void IsoMux_EDTS_Init(ISO_EDTS_BOX_s *edts, const SVC_MEDIA_TRACK_GENERAL_INFO_s *track, UINT64 minInitDTS, UINT32 timeScale, UINT32 trickRecDivisor)
{
    UINT32 EmptyEdit;
    UINT32 SegmentDuration;
    edts->Box.Size = 8;
    edts->Box.Type = TAG_EDTS;
    /* Initial ELST BOX*/
    SegmentDuration = (UINT32)((UINT64)(track->NextDTS - track->InitDTS) * timeScale / track->TimeScale * trickRecDivisor);
    EmptyEdit = (UINT32)((UINT64)(track->InitDTS - minInitDTS) * timeScale / track->TimeScale * trickRecDivisor);
    IsoMux_ELST_Init(&edts->ElstAtom, EmptyEdit, SegmentDuration);
    edts->Box.Size += edts->ElstAtom.FullBox.Box.Size;
}
#endif

static UINT32 IsoMux_TKHD_Write(const ISO_TKHD_BOX_s *Tkhd, SVC_STREAM_HDLR_s *Stream)
{
    UINT32 Rval;
    /* Tkhd.FullBox.Box.Size */
    Rval = SvcFormat_PutBe32(Stream, Tkhd->FullBox.Box.Size);
    if (Rval == FORMAT_OK) {
        /* Tkhd.FullBox.Box.Type */
        Rval = SvcFormat_PutBe32(Stream, Tkhd->FullBox.Box.Type);
    }
    if (Rval == FORMAT_OK) {
        /* Tkhd.FullBox.Version */
        Rval = SvcFormat_PutByte(Stream, Tkhd->FullBox.Version);
    }
    if (Rval == FORMAT_OK) {
        /* Tkhd.FullBox.Flags */
        Rval = IsoMux_PutBuffer8(Stream, Tkhd->FullBox.Flags, 3U);
    }
    if (Rval == FORMAT_OK) {
        /* Tkhd.CreateTime */
        Rval = SvcFormat_PutBe32(Stream, Tkhd->CreateTime);
    }
    if (Rval == FORMAT_OK) {
        /* Tkhd.ModifyTime */
        Rval = SvcFormat_PutBe32(Stream, Tkhd->ModifyTime);
    }
    if (Rval == FORMAT_OK) {
        /* Tkhd.TrackID */
        Rval = SvcFormat_PutBe32(Stream, Tkhd->TrackID);
    }
    if (Rval == FORMAT_OK) {
        /* Tkhd.Reserv4 */
        Rval = SvcFormat_PutBe32(Stream, Tkhd->Reserv4);
    }
    if (Rval == FORMAT_OK) {
        /* Tkhd.Duration */
        Rval = SvcFormat_PutBe32(Stream, Tkhd->Duration);
    }
    if (Rval == FORMAT_OK) {
        /* Tkhd.Reserv8 */
        Rval = IsoMux_PutBufferBe32(Stream, Tkhd->Reserv8, 2U);
    }
    if (Rval == FORMAT_OK) {
        /* Tkhd.Layer */
        Rval = SvcFormat_PutBe16(Stream, Tkhd->Layer);
    }
    if (Rval == FORMAT_OK) {
        /* Tkhd.Group */
        Rval = SvcFormat_PutBe16(Stream, Tkhd->Group);
    }
    if (Rval == FORMAT_OK) {
        /* Tkhd.Volume */
        Rval = SvcFormat_PutBe16(Stream, Tkhd->Volume);
    }
    if (Rval == FORMAT_OK) {
        /* Tkhd.Reserv2 */
        Rval = SvcFormat_PutBe16(Stream, Tkhd->Reserv2);
    }
    if (Rval == FORMAT_OK) {
        /* Tkhd.Matrix */
        Rval = IsoMux_PutMatrix(Stream, Tkhd->Width, Tkhd->Height, Tkhd->Rotation);
        if (Rval != FORMAT_OK) {
            AmbaPrint_ModulePrintStr5(SVC_MUXER_PRINT_MODULE_ID, "%s IsoMux_PutMatrix() error!", __func__, NULL, NULL, NULL, NULL);
        }
    }
    if (Rval == FORMAT_OK) {
        UINT32 Width, Height;
        if ((Tkhd->Rotation == SVC_ISO_ROTATION_ROTATE90) || (Tkhd->Rotation == SVC_ISO_ROTATION_ROTATE270)) {
            Width = Tkhd->Height;
            Height = Tkhd->Width;
        } else {
            Width = Tkhd->Width;
            Height = Tkhd->Height;
        }
        /* Tkhd.Width */
        Rval = SvcFormat_PutBe32(Stream, Width << 16);
        if (Rval == FORMAT_OK) {
            /* Tkhd.Height */
            Rval = SvcFormat_PutBe32(Stream, Height << 16);
        }
    }
    return Rval;
}

static void IsoMux_VideoTKHD_Init(ISO_TKHD_BOX_s *Tkhd, const SVC_VIDEO_TRACK_INFO_s *Track, UINT8 TrackIdx, const SVC_MEDIA_INFO_s *Media)
{
    Tkhd->FullBox.Box.Size = 92;
    Tkhd->FullBox.Box.Type = TAG_TKHD;
    Tkhd->FullBox.Flags[2] = 7;
    Tkhd->CreateTime = Media->CreationTime;
    Tkhd->ModifyTime = Media->ModificationTime;
    Tkhd->Rotation = Track->Rotation;
    /* refer to ffmpeg.
     * fix video double length issue in QT.
     * edts also needs to be removed.
     */
    Tkhd->Duration = 0;
    Tkhd->TrackID = (UINT32)TrackIdx + 1U;
    Tkhd->Width = Track->Width;
    Tkhd->Height = Track->Height;
}

static void IsoMux_AudioTKHD_Init(ISO_TKHD_BOX_s *Tkhd, const SVC_AUDIO_TRACK_INFO_s *Track, UINT8 TrackIdx, const SVC_MEDIA_INFO_s *Media)
{
    const SVC_MOVIE_INFO_s *Movie = NULL;
    AmbaMisra_TypeCast(&Movie, &Media);
    Tkhd->FullBox.Box.Size = 92;
    Tkhd->FullBox.Box.Type = TAG_TKHD;
    Tkhd->FullBox.Flags[2] = 7;
    Tkhd->CreateTime = Media->CreationTime;
    Tkhd->ModifyTime = Media->ModificationTime;
    Tkhd->Rotation = SVC_ISO_ROTATION_NONE;
    /* refer to ffmpeg.
     * fix video double length issue in QT.
     * edts also needs to be removed.
     */
    Tkhd->Duration = 0;
    Tkhd->TrackID = (UINT32)TrackIdx + 1U + Movie->VideoTrackCount;
    Tkhd->Volume = Track->Volume;
}

static void IsoMux_TextTKHD_Init(ISO_TKHD_BOX_s *Tkhd, UINT8 TrackIdx, const SVC_MEDIA_INFO_s *Media)
{
    const SVC_MOVIE_INFO_s *Movie = NULL;
    AmbaMisra_TypeCast(&Movie, &Media);
    Tkhd->FullBox.Box.Size = 92;
    Tkhd->FullBox.Box.Type = TAG_TKHD;
    Tkhd->FullBox.Flags[2] = 7;
    Tkhd->CreateTime = Media->CreationTime;
    Tkhd->ModifyTime = Media->ModificationTime;
    Tkhd->Rotation = SVC_ISO_ROTATION_NONE;
    /* refer to ffmpeg.
     * fix video double length issue in QT.
     * edts also needs to be removed.
     */
    Tkhd->Duration = 0;
    Tkhd->TrackID = (UINT32)TrackIdx + 1U + Movie->VideoTrackCount + Movie->AudioTrackCount;
    Tkhd->Layer = 65535;
}

/**
 *  Write a trak box to stream for video track.
 *
 *  @param [in] Trak Trak box
 *  @param [in] Track Video track information
 *  @param [in] Stream Stream handler
 *  @return 0 - OK, others - FORMAT_ERR_XXXX
 */
UINT32 IsoMux_VideoTRAK_Write(ISO_TRACK_BOX_s *Trak, const SVC_VIDEO_TRACK_INFO_s *Track, SVC_STREAM_HDLR_s *Stream)
{
    UINT32 Rval;
    /* Vmhd.FullBox.Box.Size */
    Rval = SvcFormat_PutBe32(Stream, Trak->Box.Size);
    if (Rval == FORMAT_OK) {
        /* Vmhd.FullBox.Box.Type */
        Rval = SvcFormat_PutBe32(Stream, Trak->Box.Type);
    }
    /* Write TKHD BOX*/
    if (Rval == FORMAT_OK) {
        Rval = IsoMux_TKHD_Write(&Trak->Info.Video.TkhdAtom, Stream);
        if (Rval != FORMAT_OK) {
            AmbaPrint_ModulePrintStr5(SVC_MUXER_PRINT_MODULE_ID, "%s IsoMux_TKHD_Write() error!", __func__, NULL, NULL, NULL, NULL);
        }
    }
    /* Write MDIA BOX*/
    if (Rval == FORMAT_OK) {
        Rval = IsoMux_VideoMDIA_Write(&Trak->Info.Video.MdiaAtom, Stream, Track);
        if (Rval != FORMAT_OK) {
            AmbaPrint_ModulePrintStr5(SVC_MUXER_PRINT_MODULE_ID, "%s IsoMux_MDIA_Write() error!", __func__, NULL, NULL, NULL, NULL);
        }
    }

    return Rval;
}

/**
 *  Initialize a trak box for video track.
 *
 *  @param [out] IsoTrack Trak box
 *  @param [in] Track Video track information
 *  @param [in] TrackIdx Track index
 *  @param [in] Media Media information
 *  @param [in] PrivInfo Private ISO information
 *  @param [in] Dummy Indicating that the content is dummy or not (for fmp4 format)
 *  @return 0 - OK, others - FORMAT_ERR_XXXX
 */
UINT32 IsoMux_VideoTRAK_Init(ISO_TRACK_BOX_s *IsoTrack, const SVC_VIDEO_TRACK_INFO_s *Track, UINT8 TrackIdx, const SVC_MEDIA_INFO_s *Media, const SVC_ISO_PRIV_INFO_s *PrivInfo, UINT8 Dummy)
{
    UINT32 Rval;
    IsoTrack->Box.Size = 8;
    IsoTrack->Box.Type = TAG_TRAK;
    IsoTrack->TrackType = SVC_MEDIA_TRACK_TYPE_VIDEO;
    /* Initial TKHD BOX*/
    IsoMux_VideoTKHD_Init(&IsoTrack->Info.Video.TkhdAtom, Track, TrackIdx, Media);
    IsoTrack->Box.Size += IsoTrack->Info.Video.TkhdAtom.FullBox.Box.Size;
    /* Initial MDIA BOX*/
    Rval = IsoMux_VideoMDIA_Init(&IsoTrack->Info.Video.MdiaAtom, Track, TrackIdx, Media, PrivInfo, Dummy);
    if (Rval != FORMAT_OK) {
        AmbaPrint_ModulePrintStr5(SVC_MUXER_PRINT_MODULE_ID, "%s IsoMux_MDIA_Init() error!", __func__, NULL, NULL, NULL, NULL);
    } else {
        IsoTrack->Box.Size += IsoTrack->Info.Video.MdiaAtom.Box.Size;
    }
    return Rval;
}

/**
 *  Write a trak box to stream for audio track.
 *
 *  @param [in] Trak Trak box
 *  @param [in] Track Audio track information
 *  @param [in] Stream Stream handler
 *  @return 0 - OK, others - FORMAT_ERR_XXXX
 */
UINT32 IsoMux_AudioTRAK_Write(ISO_TRACK_BOX_s *Trak, const SVC_AUDIO_TRACK_INFO_s *Track, SVC_STREAM_HDLR_s *Stream)
{
    UINT32 Rval;
    /* Trak.FullBox.Box.Size */
    Rval = SvcFormat_PutBe32(Stream, Trak->Box.Size);
    if (Rval == FORMAT_OK) {
        /* Trak.FullBox.Box.Type */
        Rval = SvcFormat_PutBe32(Stream, Trak->Box.Type);
    }
    /* Write TKHD BOX*/
    if (Rval == FORMAT_OK) {
        Rval = IsoMux_TKHD_Write(&Trak->Info.Audio.TkhdAtom, Stream);
        if (Rval != FORMAT_OK) {
            AmbaPrint_ModulePrintStr5(SVC_MUXER_PRINT_MODULE_ID, "%s IsoMux_TKHD_Write() error!", __func__, NULL, NULL, NULL, NULL);
        }
    }
    /* Write MDIA BOX*/
    if (Rval == FORMAT_OK) {
        Rval = IsoMux_AudioMDIA_Write(&Trak->Info.Audio.MdiaAtom, Stream, Track);
        if (Rval != FORMAT_OK) {
            AmbaPrint_ModulePrintStr5(SVC_MUXER_PRINT_MODULE_ID, "%s IsoMux_MDIA_Write() error!", __func__, NULL, NULL, NULL, NULL);
        }
    }
    return Rval;
}

/**
 *  Initialize a trak box for audio track.
 *
 *  @param [out] IsoTrack Trak box
 *  @param [in] Track Audio track information
 *  @param [in] TrackIdx Track index
 *  @param [in] Media Media information
 *  @param [in] PrivInfo Private ISO information
 *  @param [in] Dummy Indicating that the content is dummy or not (for fmp4 format)
 *  @return 0 - OK, others - FORMAT_ERR_XXXX
 */
UINT32 IsoMux_AudioTRAK_Init(ISO_TRACK_BOX_s *IsoTrack, const SVC_AUDIO_TRACK_INFO_s *Track, UINT8 TrackIdx, const SVC_MEDIA_INFO_s *Media, const SVC_ISO_PRIV_INFO_s *PrivInfo, UINT8 Dummy)
{
    UINT32 Rval = FORMAT_OK;
    IsoTrack->Box.Size = 8;
    IsoTrack->Box.Type = TAG_TRAK;
    IsoTrack->TrackType = SVC_MEDIA_TRACK_TYPE_AUDIO;
    /* Initial TKHD BOX*/
    IsoMux_AudioTKHD_Init(&IsoTrack->Info.Audio.TkhdAtom, Track, TrackIdx, Media);
    IsoTrack->Box.Size += IsoTrack->Info.Audio.TkhdAtom.FullBox.Box.Size;
    /* Initial MDIA BOX*/
    Rval = IsoMux_AudioMDIA_Init(&IsoTrack->Info.Audio.MdiaAtom, Track, TrackIdx, Media, PrivInfo, Dummy);
    if (Rval != FORMAT_OK) {
        AmbaPrint_ModulePrintStr5(SVC_MUXER_PRINT_MODULE_ID, "%s IsoMux_MDIA_Init() error!", __func__, NULL, NULL, NULL, NULL);
    } else {
        IsoTrack->Box.Size += IsoTrack->Info.Audio.MdiaAtom.Box.Size;
    }
    return Rval;
}

/**
 *  Write a trak box to stream for text track.
 *
 *  @param [in] Trak Trak box
 *  @param [in] Track Text track information
 *  @param [in] Stream Stream handler
 *  @return 0 - OK, others - FORMAT_ERR_XXXX
 */
UINT32 IsoMux_TextTRAK_Write(ISO_TRACK_BOX_s *Trak, SVC_TEXT_TRACK_INFO_s *Track, SVC_STREAM_HDLR_s *Stream)
{
    UINT32 Rval;
    /* Trak.FullBox.Box.Size */
    Rval = SvcFormat_PutBe32(Stream, Trak->Box.Size);
    if (Rval == FORMAT_OK) {
        /* Trak.FullBox.Box.Type */
        Rval = SvcFormat_PutBe32(Stream, Trak->Box.Type);
    }
    /* Write TKHD BOX*/
    if (Rval == FORMAT_OK) {
        Rval = IsoMux_TKHD_Write(&Trak->Info.Text.TkhdAtom, Stream);
        if (Rval != FORMAT_OK) {
            AmbaPrint_ModulePrintStr5(SVC_MUXER_PRINT_MODULE_ID, "%s IsoMux_TKHD_Write() error!", __func__, NULL, NULL, NULL, NULL);
        }
    }
    /* Write MDIA BOX*/
    if (Rval == FORMAT_OK) {
        Rval = IsoMux_TextMDIA_Write(&Trak->Info.Text.MdiaAtom, Stream, Track);
        if (Rval != FORMAT_OK) {
            AmbaPrint_ModulePrintStr5(SVC_MUXER_PRINT_MODULE_ID, "%s IsoMux_MDIA_Write() error!", __func__, NULL, NULL, NULL, NULL);
        }
    }
    return Rval;
}

/**
 *  Initialize a trak box for text track.
 *
 *  @param [out] IsoTrack Trak box
 *  @param [in] Track Text track information
 *  @param [in] TrackIdx Track index
 *  @param [in] Media Media information
 *  @param [in] PrivInfo Private ISO information
 *  @param [in] Dummy Indicating that the content is dummy or not (for fmp4 format)
 *  @return 0 - OK, others - FORMAT_ERR_XXXX
 */
UINT32 IsoMux_TextTRAK_Init(ISO_TRACK_BOX_s *IsoTrack, const SVC_TEXT_TRACK_INFO_s *Track, UINT8 TrackIdx, const SVC_MEDIA_INFO_s *Media, const SVC_ISO_PRIV_INFO_s *PrivInfo, UINT8 Dummy)
{
    UINT32 Rval;
    IsoTrack->Box.Size = 8;
    IsoTrack->Box.Type = TAG_TRAK;
    IsoTrack->TrackType = SVC_MEDIA_TRACK_TYPE_TEXT;
    /* Initial TKHD BOX*/
    IsoMux_TextTKHD_Init(&IsoTrack->Info.Text.TkhdAtom, TrackIdx, Media);
    IsoTrack->Box.Size += IsoTrack->Info.Text.TkhdAtom.FullBox.Box.Size;
    /* Initial MDIA BOX*/
    Rval = IsoMux_TextMDIA_Init(&IsoTrack->Info.Text.MdiaAtom, Track, TrackIdx, Media, PrivInfo, Dummy);
    if (Rval != FORMAT_OK) {
        AmbaPrint_ModulePrintStr5(SVC_MUXER_PRINT_MODULE_ID, "%s IsoMux_MDIA_Init() error!", __func__, NULL, NULL, NULL, NULL);
    } else {
        IsoTrack->Box.Size += IsoTrack->Info.Text.MdiaAtom.Box.Size;
    }
    return Rval;
}

/**
 *  Write an udta box to stream.
 *
 *  @param [in] Udta Udta box
 *  @param [in] Stream Stream handler
 *  @param [in] Media Media information
 *  @return 0 - OK, others - FORMAT_ERR_XXXX
 */
UINT32 IsoMux_UDTA_Write(const ISO_UDTA_BOX_s *Udta, SVC_STREAM_HDLR_s *Stream, SVC_MEDIA_INFO_s *Media)
{
    UINT32 Rval = FORMAT_OK;
    const INT32 Size = SvcFormat_GetAmbaBoxSize(Media);
    if (Size <= 0) {
        AmbaPrint_ModulePrintStr5(SVC_MUXER_PRINT_MODULE_ID, "%s Wrong value of Amba box size!", __func__, NULL, NULL, NULL, NULL);
        Rval = FORMAT_ERR_GENERAL_ERROR;
    } else {
        /* Udta.Box.Size */
        Rval = SvcFormat_PutBe32(Stream, Udta->Box.Size);
    }
    if (Rval == FORMAT_OK) {
        /* Udta.Box.Type */
        Rval = SvcFormat_PutBe32(Stream, Udta->Box.Type);
    }
    if (Rval == FORMAT_OK) {
        /* Amba.Size */
        Rval = SvcFormat_PutBe32(Stream, 8U + (UINT32)Size);
    }
    if (Rval == FORMAT_OK) {
        /* Amba.Type */
        Rval = SvcFormat_PutBe32(Stream, TAG_AMBA);
    }
    if (Rval == FORMAT_OK) {
        if (Media->MediaType == SVC_MEDIA_INFO_MOVIE) {
            const SVC_MOVIE_INFO_s *TmpMovie;
            UINT32 i;
            AmbaMisra_TypeCast(&TmpMovie, &Media);
            Rval = SvcFormat_PutAmbaBox(Stream, &TmpMovie->MediaInfo); /* Write Amba Box */
            if (Rval == FORMAT_OK) {
                if ((Udta->UserDataSize != 0U) && (Udta->UserData != NULL)) {
                    for(i = 0; i < Udta->UserDataSize; i++) {
                        Rval = SvcFormat_PutByte(Stream, Udta->UserData[i]);
                        if (Rval != FORMAT_OK) {
                            break;
                        }
                    }
                }
            }
        } else {
            AmbaPrint_ModulePrintStr5(SVC_MUXER_PRINT_MODULE_ID, "%s Only support Movie!", __func__, NULL, NULL, NULL, NULL);
            Rval = FORMAT_ERR_INVALID_ARG;
        }
    }
    AmbaMisra_TouchUnused(Media);
    return Rval;
}

/**
 *  Initialize an udta box.
 *
 *  @param [out] Udta Udta box
 *  @param [in] Media Media information
 *  @return 0 - OK, others - FORMAT_ERR_XXXX
 */
UINT32 IsoMux_UDTA_Init(ISO_UDTA_BOX_s *Udta, SVC_MEDIA_INFO_s *Media)
{
    UINT32 Rval = FORMAT_OK;
    INT32 Size;
    SvcFormat_InitAmbaBoxVersion(Media);
    Size = SvcFormat_GetAmbaBoxSize(Media);
    if (Size <= 0) {
        AmbaPrint_ModulePrintStr5(SVC_MUXER_PRINT_MODULE_ID, "%s Wrong value of Amba box size!", __func__, NULL, NULL, NULL, NULL);
        Rval = FORMAT_ERR_GENERAL_ERROR;
    } else {
        Udta->Box.Size = 8;
        Udta->Box.Type = TAG_UDTA;
        Udta->Box.Size += (8U + (UINT32)Size);
#if defined(CONFIG_APP_FLOW_CARDV_AONI)
        {
            extern void UpdateVideoMetadata(unsigned char *data, unsigned int *out_len);            
            UpdateVideoMetadata(Udta->UserData, &Udta->UserDataSize);
        }
#endif
        if(Udta->UserDataSize != 0U) {
            Udta->Box.Size += Udta->UserDataSize;
        }
    }
    return Rval;
}

/**
 *  Write a mvhd box to stream.
 *
 *  @param [in] Mvhd Mvhd box
 *  @param [in] Stream Stream handler
 *  @return 0 - OK, others - FORMAT_ERR_XXXX
 */
UINT32 IsoMux_MVHD_Write(const ISO_MVHD_BOX_s *Mvhd, SVC_STREAM_HDLR_s *Stream)
{
    UINT32 Rval;
    /* Mvhd.FullBox.Box.Size */
    Rval = SvcFormat_PutBe32(Stream, Mvhd->FullBox.Box.Size);
    if (Rval == FORMAT_OK) {
        /* Mvhd.FullBox.Box.Type */
        Rval = SvcFormat_PutBe32(Stream, Mvhd->FullBox.Box.Type);
    }
    if (Rval == FORMAT_OK) {
        /* Mvhd.FullBox.Version */
        Rval = SvcFormat_PutByte(Stream, Mvhd->FullBox.Version);
    }
    if (Rval == FORMAT_OK) {
        /* Mvhd.FullBox.Flags */
        Rval = IsoMux_PutBuffer8(Stream, Mvhd->FullBox.Flags, 3U);
    }
    if (Rval == FORMAT_OK) {
        /* Mvhd.CreateTime */
        Rval = SvcFormat_PutBe32(Stream, Mvhd->CreateTime);
    }
    if (Rval == FORMAT_OK) {
        /* Mvhd.ModifyTime */
        Rval = SvcFormat_PutBe32(Stream, Mvhd->ModifyTime);
    }
    if (Rval == FORMAT_OK) {
        /* Mvhd.TimeScale */
        Rval = SvcFormat_PutBe32(Stream, Mvhd->TimeScale);
    }
    if (Rval == FORMAT_OK) {
        /* Mvhd.Duration */
        Rval = SvcFormat_PutBe32(Stream, Mvhd->Duration);
    }
    if (Rval == FORMAT_OK) {
        /* Mvhd.PreferRate */
        Rval = SvcFormat_PutBe32(Stream, Mvhd->PreferRate);
    }
    if (Rval == FORMAT_OK) {
        /* Mvhd.Volume */
        Rval = SvcFormat_PutBe16(Stream, Mvhd->Volume);
    }
    if (Rval == FORMAT_OK) {
        /* Mvhd.Reserv2 */
        Rval = SvcFormat_PutBe16(Stream, Mvhd->Reserv2);
    }
    if (Rval == FORMAT_OK) {
        /* Mvhd.Reserv4 */
        Rval = IsoMux_PutBufferBe32(Stream, Mvhd->Reserv4, 2U);
    }
    if (Rval == FORMAT_OK) {
        /* Mvhd.Matrix */
        Rval = IsoMux_PutMatrix(Stream, Mvhd->Width, Mvhd->Height, Mvhd->Rotation);
    }
    if (Rval == FORMAT_OK) {
        /* Mvhd.nPreDefined */
        Rval = SvcFormat_PutBe32(Stream, Mvhd->PreviewTime);
    }
    if (Rval == FORMAT_OK) {
        /* Mvhd.nPreDefined */
        Rval = SvcFormat_PutBe32(Stream, Mvhd->PreviewDuration);
    }
    if (Rval == FORMAT_OK) {
        /* Mvhd.nPreDefined */
        Rval = SvcFormat_PutBe32(Stream, Mvhd->PosterTime);
    }
    if (Rval == FORMAT_OK) {
        /* Mvhd.nPreDefined */
        Rval = SvcFormat_PutBe32(Stream, Mvhd->SelectionTime);
    }
    if (Rval == FORMAT_OK) {
        /* Mvhd.nPreDefined */
        Rval = SvcFormat_PutBe32(Stream, Mvhd->SelectionDuration);
    }
    if (Rval == FORMAT_OK) {
        /* Mvhd.nPreDefined */
        Rval = SvcFormat_PutBe32(Stream, Mvhd->CurrentTime);
    }
    if (Rval == FORMAT_OK) {
        /* Mvhd.NextTrackID */
        Rval = SvcFormat_PutBe32(Stream, Mvhd->NextTrackID);
    }
    return Rval;
}

/**
 *  Initialize a mvhd box.
 *
 *  @param [out] Mvhd Mvhd box
 *  @param [in] Media Media information
 *  @param [out] MinInitDTS Minimum initial DTS, NULL menas dummy for FMP4
 *  @return 0 - OK, others - FORMAT_ERR_XXXX
 */
UINT32 IsoMux_MVHD_Init(ISO_MVHD_BOX_s *Mvhd, SVC_MEDIA_INFO_s *Media, UINT64 *MinInitDTS)
{
    UINT32 Rval = FORMAT_OK;
    UINT8 i = 0;
    UINT32 TmpOrgTimeScale = 0, TmpTimeScale = 0;
    UINT64 TmpNextDTS = 0;
    UINT64 TmpInitDTS = SVC_FORMAT_MAX_TIMESTAMP;
    SVC_MOVIE_INFO_s *Movie = NULL;
    const SVC_MEDIA_TRACK_GENERAL_INFO_s *Track;
    UINT8 TrackCount = 0;

    AmbaMisra_TypeCast(&Movie, &Media);

    if (Media->MediaType == SVC_MEDIA_INFO_MOVIE) {
        TrackCount = Movie->VideoTrackCount + Movie->AudioTrackCount + Movie->TextTrackCount;
        for (i = 0; i < Movie->VideoTrackCount; i++) {/* Calculate the duration */
            Track = &Movie->VideoTrack[i].Info;
            if (Track->OrigTimeScale > TmpTimeScale) {
                TmpOrgTimeScale = Track->OrigTimeScale;
                TmpTimeScale = Track->TimeScale;
            }
            if (Track->NextDTS >= TmpNextDTS) {
                TmpNextDTS = Track->NextDTS;
            }
            if (Track->InitDTS <= TmpInitDTS) {
                TmpInitDTS = Track->InitDTS;
            }
        }
        for (i = 0; i < Movie->AudioTrackCount; i++) {/* Calculate the duration */
            Track = &Movie->AudioTrack[i].Info;
            if (Track->OrigTimeScale > TmpTimeScale) {
                TmpOrgTimeScale = Track->OrigTimeScale;
                TmpTimeScale = Track->TimeScale;
            }
            if (Track->NextDTS >= TmpNextDTS) {
                TmpNextDTS = Track->NextDTS;
            }
            if (Track->InitDTS <= TmpInitDTS) {
                TmpInitDTS = Track->InitDTS;
            }
        }
        for (i = 0; i < Movie->TextTrackCount; i++) {/* Calculate the duration */
            Track = &Movie->TextTrack[i].Info;
            if (Track->OrigTimeScale > TmpTimeScale) {
                TmpOrgTimeScale = Track->OrigTimeScale;
                TmpTimeScale = Track->TimeScale;
            }
            if (Track->NextDTS >= TmpNextDTS) {
                TmpNextDTS = Track->NextDTS;
            }
            if (Track->InitDTS <= TmpInitDTS) {
                TmpInitDTS = Track->InitDTS;
            }
        }
    } else {
        AmbaPrint_ModulePrintStr5(SVC_MUXER_PRINT_MODULE_ID, "%s Only support Movie!", __func__, NULL, NULL, NULL, NULL);
        Rval = FORMAT_ERR_INVALID_ARG;
    }
    if (Rval == FORMAT_OK) {
        Mvhd->FullBox.Box.Size = 108U;
        Mvhd->FullBox.Box.Type = TAG_MVHD;
        Mvhd->CreateTime = Media->CreationTime;
        Mvhd->ModifyTime = Media->ModificationTime;
        if (MinInitDTS != NULL) {
            *MinInitDTS = TmpInitDTS;
#if defined(CONFIG_APP_FLOW_CARDV_AONI)
            (void)TmpOrgTimeScale;
            Mvhd->TimeScale = TmpTimeScale;
#else
            Mvhd->TimeScale = TmpOrgTimeScale;
#endif

            Mvhd->Duration = (UINT32)NORMALIZE_TO_TIMESCALE((UINT64)TmpTimeScale, TmpNextDTS - TmpInitDTS, (UINT64)TmpOrgTimeScale);
        } else {
            /* refer to ffmpeg.
             * fix video double length issue in QT.
             * edts also needs to be removed.
             */
            if (Mvhd->TimeScale == 0U) {
                Mvhd->TimeScale = 1000U;
            }
            Mvhd->Duration = 0U;
        }
        Mvhd->PreferRate = 0x10000U;
        Mvhd->Volume = 0x100U;
        Mvhd->Width = Movie->VideoTrack[0].Width;
        Mvhd->Height = Movie->VideoTrack[0].Height;
        Mvhd->Rotation = Movie->VideoTrack[0].Rotation;
        Mvhd->NextTrackID = (UINT32)TrackCount + 1U;

        Movie->MvhdTimeScale = Mvhd->TimeScale;
    }
    AmbaMisra_TouchUnused(Media);
    return Rval;
}

static UINT32 IsoMux_FTYP_Write(const ISO_FTYP_s *Ftyp, SVC_STREAM_HDLR_s *Stream)
{
    UINT32 Rval;
    Rval = SvcFormat_PutBe32(Stream, Ftyp->Box.Size);
    if (Rval == FORMAT_OK) {
        Rval = SvcFormat_PutBe32(Stream, Ftyp->Box.Type);
    }
    if (Rval == FORMAT_OK) {
        Rval = SvcFormat_PutBe32(Stream, Ftyp->MajorBrand);
    }
    if (Rval == FORMAT_OK) {
        Rval = SvcFormat_PutBe32(Stream, Ftyp->MinorVersion);
    }
    if (Rval == FORMAT_OK) {
        Rval = SvcFormat_PutBe32(Stream, Ftyp->CompatibleBrands[0]);
    }
    if (Rval == FORMAT_OK) {
        Rval = SvcFormat_PutBe32(Stream, Ftyp->CompatibleBrands[1]);
    }
    if (Rval == FORMAT_OK) {
        Rval = SvcFormat_PutBe32(Stream, Ftyp->CompatibleBrands[2]);
    }
    if (Rval == FORMAT_OK) {
        Rval = SvcFormat_PutBe32(Stream, Ftyp->CompatibleBrands[3]);
    }
    return Rval;
}

static UINT32 IsoMux_FTYP_Init(ISO_FTYP_s *Ftyp)
{
    Ftyp->Box.Size = 32;
    Ftyp->Box.Type = TAG_FTYP;

    if (Ftyp->MediaId == SVC_FORMAT_MID_HVC) {
        /* refer to MP4BOX */
        Ftyp->MajorBrand = TAG_iso4;
        Ftyp->MinorVersion = 0;
        Ftyp->CompatibleBrands[0] = TAG_iso4;
        Ftyp->CompatibleBrands[1] = TAG_HVC1;
    } else {
        /* refer to A7L */
        Ftyp->MajorBrand = TAG_AVC1;
        Ftyp->MinorVersion = 0;
        Ftyp->CompatibleBrands[0] = TAG_AVC1;
        Ftyp->CompatibleBrands[1] = TAG_isom;
    }

    Ftyp->CompatibleBrands[2] = 0;
    Ftyp->CompatibleBrands[3] = 0;
    return Ftyp->Box.Size;
}

/**
 *  Get the size of a ftyp box.
 *
 *  @return Ftyp box size
 */
UINT32 IsoMux_GetFtypSize(void)
{
    ISO_FTYP_s Ftyp;
    Ftyp.MediaId = SVC_FORMAT_MID_HVC; // don't care, for getting Ftyp box Size
    return IsoMux_FTYP_Init(&Ftyp);
}

/**
 *  Write a ftyp box to stream.
 *
 *  @param [in] Media Media information
 *  @param [in] Stream Stream handler
 *  @return 0 - OK, others - FORMAT_ERR_XXXX
 */
UINT32 IsoMux_MP4_PutFtyp(const SVC_MEDIA_INFO_s *Media, SVC_STREAM_HDLR_s *Stream)
{
    UINT32 Rval = FORMAT_ERR_GENERAL_ERROR;
    const SVC_VIDEO_TRACK_INFO_s *DefTrack;
    SVC_MOVIE_INFO_s *Movie;
    UINT8 Tmp;
    AmbaMisra_TypeCast(&Movie, &Media);
    DefTrack = SvcFormat_GetDefaultVideoTrack(Movie->VideoTrack, Movie->VideoTrackCount, &Tmp);
    if (DefTrack != NULL) {
        ISO_FTYP_s Ftyp;
        UINT32 FtypeSize;
        Ftyp.MediaId = DefTrack->Info.MediaId;
        FtypeSize = IsoMux_FTYP_Init(&Ftyp);
        AmbaMisra_TouchUnused(&FtypeSize);
        Rval = IsoMux_FTYP_Write(&Ftyp, Stream);
    }
    return Rval;
}

/**
 *  Parse NAL units
 *
 *  @param [in] Size Size
 *  @param [in] BufferOffset Buffer offset
 *  @param [in] BufferBase Pointer to the buffer base
 *  @param [in] BufferSize Buffer size
 *  @param [in,out] Track Video track information
 *  @param [in,out] TrackInfo ISO video track information
 *  @return 0 - OK, others - FORMAT_ERR_XXXX
 */
UINT32 IsoMux_ParseNalUnits(UINT32 Size, UINT32 BufferOffset, const UINT8 *BufferBase, UINT32 BufferSize, SVC_VIDEO_TRACK_INFO_s *Track, SVC_ISO_VIDEO_TRACK_INFO_s *TrackInfo)
{
    UINT32 Rval = FORMAT_OK;
    if (Track->Info.MediaId == SVC_FORMAT_MID_AVC) {
        if ((TrackInfo->Avc.SPSLen == 0U) || (TrackInfo->Avc.PPSLen == 0U)) {
            SVC_H264_DEC_CONFIG_s H264 = {0};
            Rval = SvcH264_GetAvcConfig(Size, BufferOffset, BufferBase, BufferSize, &H264);
            if (Rval == FORMAT_OK) {
                TrackInfo->Avc.NALULength = H264.NALULength;
                TrackInfo->Avc.SPSLen = H264.SPSLen;
                TrackInfo->Avc.PPSLen = H264.PPSLen;
                Rval = W2F(AmbaWrap_memcpy(TrackInfo->Avc.SPS, H264.SPS, sizeof(TrackInfo->Avc.SPS)));
                if (Rval == FORMAT_OK) {
                    Rval = W2F(AmbaWrap_memcpy(TrackInfo->Avc.PPS, H264.PPS, sizeof(TrackInfo->Avc.PPS)));
                }
                if (Rval == FORMAT_OK) {
                    Track->PixelArX = H264.SarWidth;
                    Track->PixelArY = H264.SarHeight;
                    AmbaPrint_ModulePrintUInt5(SVC_MUXER_PRINT_MODULE_ID, "[TRACK_INFO]timescale=%u, timeperframe=%u  [H264_INFO]time_scale=%u, num_units_in_tick=%u", Track->Info.TimeScale, Track->Info.TimePerFrame, H264.TimeScale, H264.NumUnitsInTick, 0U);
                }
            }
        }
    } else if (Track->Info.MediaId == SVC_FORMAT_MID_HVC) {
        if ((TrackInfo->Hvc.VPSLen == 0U) || (TrackInfo->Hvc.SPSLen == 0U) ||(TrackInfo->Hvc.PPSLen == 0U)) {
            SVC_H265_DEC_CONFIG_s H265 = {0};
            Rval = SvcH265_GetHvcConfig(Size, BufferOffset, BufferBase, BufferSize, &H265);
            if (Rval == FORMAT_OK) {
                Rval = W2F(AmbaWrap_memcpy(TrackInfo->Hvc.Generalconfiguration, H265.Generalconfiguration, sizeof(TrackInfo->Hvc.Generalconfiguration)));
                if (Rval == FORMAT_OK) {
                    TrackInfo->Hvc.ChromaFormat = H265.ChromaFormat;
                    TrackInfo->Hvc.BitDepthLumaMinus8 = H265.BitDepthLumaMinus8;
                    TrackInfo->Hvc.BitDepthChromaMinus8 = H265.BitDepthChromaMinus8;
                    TrackInfo->Hvc.NumTemporalLayers = H265.NumTemporalLayers;
                    TrackInfo->Hvc.TemporalIdNested = H265.TemporalIdNested;
                    TrackInfo->Hvc.NumofArray = H265.NumofArray;
                    TrackInfo->Hvc.NALULength = H265.NALULength;
                    TrackInfo->Hvc.PicSizeInCtbsY = H265.PicSizeInCtbsY;
                    TrackInfo->Hvc.VPSLen = H265.VPSLen;
                    TrackInfo->Hvc.SPSLen = H265.SPSLen;
                    TrackInfo->Hvc.PPSLen = H265.PPSLen;
                    Rval = W2F(AmbaWrap_memcpy(TrackInfo->Hvc.VPS, H265.VPS, sizeof(TrackInfo->Hvc.VPS)));
                }
                if (Rval == FORMAT_OK) {
                    Rval = W2F(AmbaWrap_memcpy(TrackInfo->Hvc.SPS, H265.SPS, sizeof(TrackInfo->Hvc.SPS)));
                }
                if (Rval == FORMAT_OK) {
                    Rval = W2F(AmbaWrap_memcpy(TrackInfo->Hvc.PPS, H265.PPS, sizeof(TrackInfo->Hvc.PPS)));
                }
                if (Rval == FORMAT_OK) {
                    Track->PixelArX = H265.SarWidth;
                    Track->PixelArY = H265.SarHeight;
                    AmbaPrint_ModulePrintUInt5(SVC_MUXER_PRINT_MODULE_ID, "[TRACK_INFO]timescale=%u, timeperframe=%u  [H265_INFO]time_scale=%u, num_units_in_tick=%u", Track->Info.TimeScale, Track->Info.TimePerFrame, H265.TimeScale, H265.NumUnitsInTick, 0U);
                }
            }
        }
    } else {
        AmbaPrint_ModulePrintStr5(SVC_MUXER_PRINT_MODULE_ID, "%s Not support this media id!", __func__, NULL, NULL, NULL, NULL);
        Rval = FORMAT_ERR_IO_ERROR;
    }

    return Rval;
}

