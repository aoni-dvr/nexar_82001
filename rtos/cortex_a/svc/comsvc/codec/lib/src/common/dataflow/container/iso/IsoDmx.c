/**
 * @file IsoDmx.c
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
#include "format/SvcIso.h"
#include "IsoDmx.h"
#include "../FormatAPI.h"
#include "../ByteOp.h"
#include "../H264.h"
#include "../H265.h"
#include "../demuxer/FrameFeeder.h"

/**
 *  Get the box header
 *
 *  @param [in] Stream Stream handler
 *  @param [out] BoxSize The size of the box
 *  @param [out] BoxTag The tag of the box
 *  @return 0 - OK, others - FORMAT_ERR_XXXX
 */
UINT32 IsoDmx_GetBoxHeader(SVC_STREAM_HDLR_s *Stream, UINT32 *BoxSize, UINT32 *BoxTag)
{
    UINT32 Rval = SvcFormat_GetBe32(Stream, BoxSize);
    if (Rval == FORMAT_OK) {
        Rval = SvcFormat_GetBe32(Stream, BoxTag);
        if (Rval == FORMAT_OK) {
            if ((*BoxSize < 8U) || (*BoxTag == 0xffffffffU)) {
                AmbaPrint_ModulePrintUInt5(SVC_DEMUXER_PRINT_MODULE_ID, "IsoDmx_GetBoxHeader box size (%u) or box tag (%u) error", *BoxSize, *BoxTag, 0U, 0U, 0U);
                Rval = FORMAT_ERR_INVALID_FORMAT;
            }
        } else {
            AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s SvcFormat_GetBe32() error!", __func__, NULL, NULL, NULL, NULL);
        }
    } else {
        AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s SvcFormat_GetBe32() error!", __func__, NULL, NULL, NULL, NULL);
    }
    return Rval;
}

static UINT32 IsoDmx_GetBuffer8(SVC_STREAM_HDLR_s *Stream, UINT8 *Buffer, UINT32 Count)
{
    UINT32 Rval = FORMAT_OK;
    UINT32 i;
    for (i = 0; i < Count; i++) {
        Rval = SvcFormat_GetByte(Stream, &Buffer[i]);
        if (Rval != FORMAT_OK) {
            break;
        }
    }
    return Rval;
}

static UINT32 IsoDmx_GetBufferBe16(SVC_STREAM_HDLR_s *Stream, UINT16 *Buffer, UINT32 Count)
{
    UINT32 Rval = FORMAT_OK;
    UINT32 i;
    for (i = 0; i < Count; i++) {
        Rval = SvcFormat_GetBe16(Stream, &Buffer[i]);
        if (Rval != FORMAT_OK) {
            break;
        }
    }
    return Rval;
}

static UINT32 IsoDmx_GetBufferBe32(SVC_STREAM_HDLR_s *Stream, UINT32 *Buffer, UINT32 Count)
{
    UINT32 Rval = FORMAT_OK;
    UINT32 i;
    for (i = 0; i < Count; i++) {
        Rval = SvcFormat_GetBe32(Stream, &Buffer[i]);
        if (Rval != FORMAT_OK) {
            break;
        }
    }
    return Rval;
}

static UINT32 IsoDmx_ParseMvhd(SVC_STREAM_HDLR_s *Stream, ISO_MVHD_BOX_s *Mvhd, UINT32 BoxSize)
{
    UINT32 Rval = FORMAT_OK;
    if (BoxSize == 108U) {
        UINT8 Tmp8[4];
        Rval = IsoDmx_GetBuffer8(Stream, Tmp8, 4U);
        if (Rval == FORMAT_OK) {
            Rval = SvcFormat_GetBe32(Stream, &Mvhd->CreateTime);
        } else {
            AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s SvcFormat_GetByte() error!", __func__, NULL, NULL, NULL, NULL);
        }
        if (Rval == FORMAT_OK) {
            Rval = SvcFormat_GetBe32(Stream, &Mvhd->ModifyTime);
        } else {
            AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s SvcFormat_GetBe32() error!", __func__, NULL, NULL, NULL, NULL);
        }
        if (Rval == FORMAT_OK) {
            Rval = SvcFormat_GetBe32(Stream, &Mvhd->TimeScale);
        } else {
            AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s SvcFormat_GetBe32() error!", __func__, NULL, NULL, NULL, NULL);
        }
        if (Rval == FORMAT_OK) {
            Rval = SvcFormat_GetBe32(Stream, &Mvhd->Duration);
        } else {
            AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s SvcFormat_GetBe32() error!", __func__, NULL, NULL, NULL, NULL);
        }
        if (Rval == FORMAT_OK) {
            Rval = SvcFormat_GetBe32(Stream, &Mvhd->PreferRate);
        } else {
            AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s SvcFormat_GetBe32() error!", __func__, NULL, NULL, NULL, NULL);
        }
        if (Rval == FORMAT_OK) {
            Rval = SvcFormat_GetBe16(Stream, &Mvhd->Volume);
        } else {
            AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s SvcFormat_GetBe32() error!", __func__, NULL, NULL, NULL, NULL);
        }
        if (Rval == FORMAT_OK) {
            Rval = SvcFormat_GetBe16(Stream, &Mvhd->Reserv2);
        } else {
            AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s SvcFormat_GetBe16() error!", __func__, NULL, NULL, NULL, NULL);
        }
        if (Rval == FORMAT_OK) {
            Rval = IsoDmx_GetBufferBe32(Stream, Mvhd->Reserv4, 2U);
        } else {
            AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s SvcFormat_GetBe16() error!", __func__, NULL, NULL, NULL, NULL);
        }
        if (Rval == FORMAT_OK) {
            Rval = IsoDmx_GetBufferBe32(Stream, Mvhd->Matrix, 9U);
        } else {
            AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s SvcFormat_GetBe32() error!", __func__, NULL, NULL, NULL, NULL);
        }
        if (Rval == FORMAT_OK) {
            Rval = SvcFormat_GetBe32(Stream, &Mvhd->PreviewTime);
        }
        if (Rval == FORMAT_OK) {
            Rval = SvcFormat_GetBe32(Stream, &Mvhd->PreviewDuration);
        } else {
            AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s SvcFormat_GetBe32() error!", __func__, NULL, NULL, NULL, NULL);
        }
        if (Rval == FORMAT_OK) {
            Rval = SvcFormat_GetBe32(Stream, &Mvhd->PosterTime);
        } else {
            AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s SvcFormat_GetBe32() error!", __func__, NULL, NULL, NULL, NULL);
        }
        if (Rval == FORMAT_OK) {
            Rval = SvcFormat_GetBe32(Stream, &Mvhd->SelectionTime);
        } else {
            AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s SvcFormat_GetBe32() error!", __func__, NULL, NULL, NULL, NULL);
        }
        if (Rval == FORMAT_OK) {
            Rval = SvcFormat_GetBe32(Stream, &Mvhd->SelectionDuration);
        } else {
            AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s SvcFormat_GetBe32() error!", __func__, NULL, NULL, NULL, NULL);
        }
        if (Rval == FORMAT_OK) {
            Rval = SvcFormat_GetBe32(Stream, &Mvhd->CurrentTime);
        } else {
            AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s SvcFormat_GetBe32() error!", __func__, NULL, NULL, NULL, NULL);
        }
        if (Rval == FORMAT_OK) {
            Rval = SvcFormat_GetBe32(Stream, &Mvhd->NextTrackID);
            if (Rval != FORMAT_OK) {
                AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s SvcFormat_GetBe32() error!", __func__, NULL, NULL, NULL, NULL);
            }
        } else {
            AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s SvcFormat_GetBe32() error!", __func__, NULL, NULL, NULL, NULL);
        }
    } else {
        AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s box size is wrong!", __func__, NULL, NULL, NULL, NULL);
        Rval = FORMAT_ERR_INVALID_FORMAT;
    }
    return Rval;
}

static UINT32 IsoDmx_ParseTkhd(SVC_STREAM_HDLR_s *Stream, ISO_TKHD_BOX_s *Tkhd, UINT32 BoxSize)
{
    UINT32 Rval = FORMAT_OK;
    if (BoxSize == 92U) {
        UINT8 Tmp8;
        UINT32 Tmp32 = 0U;
        Rval = SvcFormat_GetByte(Stream, &Tmp8);
        if (Rval == FORMAT_OK) {
            Rval = SvcFormat_GetByte(Stream, &Tmp8);
        } else {
            AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s SvcFormat_GetByte() error!", __func__, NULL, NULL, NULL, NULL);
        }
        if (Rval == FORMAT_OK) {
            Rval = SvcFormat_GetByte(Stream, &Tmp8);
        } else {
            AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s SvcFormat_GetByte() error!", __func__, NULL, NULL, NULL, NULL);
        }
        if (Rval == FORMAT_OK) {
            Rval = SvcFormat_GetByte(Stream, &Tmp8);
        } else {
            AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s SvcFormat_GetByte() error!", __func__, NULL, NULL, NULL, NULL);
        }
        if (Rval == FORMAT_OK) {
            Rval = SvcFormat_GetBe32(Stream, &Tkhd->CreateTime);
        } else {
            AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s SvcFormat_GetByte() error!", __func__, NULL, NULL, NULL, NULL);
        }
        if (Rval == FORMAT_OK) {
            Rval = SvcFormat_GetBe32(Stream, &Tkhd->ModifyTime);
        } else {
            AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s SvcFormat_GetBe32() error!", __func__, NULL, NULL, NULL, NULL);
        }
        if (Rval == FORMAT_OK) {
            Rval = SvcFormat_GetBe32(Stream, &Tkhd->TrackID);
        } else {
            AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s SvcFormat_GetBe32() error!", __func__, NULL, NULL, NULL, NULL);
        }
        if (Rval == FORMAT_OK) {
            Rval = SvcFormat_GetBe32(Stream, &Tmp32);
        } else {
            AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s SvcFormat_GetBe32() error!", __func__, NULL, NULL, NULL, NULL);
        }
        if (Rval == FORMAT_OK) {
            Rval = SvcFormat_GetBe32(Stream, &Tkhd->Duration);
        } else {
            AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s SvcFormat_GetBe32() error!", __func__, NULL, NULL, NULL, NULL);
        }
        if (Rval == FORMAT_OK) {
            Rval = SvcFormat_GetBe32(Stream, &Tmp32);
        } else {
            AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s SvcFormat_GetBe32() error!", __func__, NULL, NULL, NULL, NULL);
        }
        if (Rval == FORMAT_OK) {
            Rval = SvcFormat_GetBe32(Stream, &Tmp32);
        } else {
            AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s SvcFormat_GetBe32() error!", __func__, NULL, NULL, NULL, NULL);
        }
        if (Rval == FORMAT_OK) {
            UINT16 TmpArr16[4];
            Rval = IsoDmx_GetBufferBe16(Stream, TmpArr16, 4U);
        } else {
            AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s SvcFormat_GetBe32() error!", __func__, NULL, NULL, NULL, NULL);
        }
        if (Rval == FORMAT_OK) {
            UINT32 TmpArr32[9];
            Rval = IsoDmx_GetBufferBe32(Stream, TmpArr32, 9U);
        } else {
            AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s SvcFormat_GetBe16() error!", __func__, NULL, NULL, NULL, NULL);
        }
        if (Rval == FORMAT_OK) {
            Rval = SvcFormat_GetBe32(Stream, &Tmp32);
        } else {
            AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s SvcFormat_GetBe32() error!", __func__, NULL, NULL, NULL, NULL);
        }
        if (Rval == FORMAT_OK) {
            Tkhd->Width = Tmp32 >> 16U;
            Rval = SvcFormat_GetBe32(Stream, &Tmp32);
        } else {
            AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s SvcFormat_GetBe32() error!", __func__, NULL, NULL, NULL, NULL);
        }
        if (Rval == FORMAT_OK) {
            Tkhd->Height = Tmp32 >> 16U;
        } else {
            AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s SvcFormat_GetBe32() error!", __func__, NULL, NULL, NULL, NULL);
        }
    } else {
        AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s box size is wrong!", __func__, NULL, NULL, NULL, NULL);
        Rval = FORMAT_ERR_INVALID_FORMAT;
    }
    return Rval;
}

static UINT32 IsoDmx_ParseEdts(SVC_STREAM_HDLR_s *Stream, ISO_EDTS_BOX_s *Edts, UINT32 BoxSize)
{
    UINT32 Rval = FORMAT_OK;
    if ((BoxSize == 36U) || (BoxSize == 48U)) {
        UINT8 Tmp8;
        UINT32 Tmp32;
        Rval = SvcFormat_GetBe32(Stream, &Tmp32);
        if (Rval  == FORMAT_OK) {
            Rval = SvcFormat_GetBe32(Stream, &Tmp32);
        } else {
            AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s SvcFormat_GetBe32() error!", __func__, NULL, NULL, NULL, NULL);
        }
        if (Rval == FORMAT_OK) {
            Rval = SvcFormat_GetByte(Stream, &Tmp8);
        } else {
            AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s SvcFormat_GetBe32() error!", __func__, NULL, NULL, NULL, NULL);
        }
        if (Rval == FORMAT_OK) {
            Rval = SvcFormat_GetByte(Stream, &Tmp8);
        } else {
            AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s SvcFormat_GetByte() error!", __func__, NULL, NULL, NULL, NULL);
        }
        if (Rval == FORMAT_OK) {
            Rval = SvcFormat_GetByte(Stream, &Tmp8);
        } else {
            AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s SvcFormat_GetByte() error!", __func__, NULL, NULL, NULL, NULL);
        }
        if (Rval == FORMAT_OK) {
            Rval = SvcFormat_GetByte(Stream, &Tmp8);
        } else {
            AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s SvcFormat_GetByte() error!", __func__, NULL, NULL, NULL, NULL);
        }
        if (Rval == FORMAT_OK) {
            Rval = SvcFormat_GetBe32(Stream, &Edts->ElstAtom.EntryCount);
        } else {
            AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s SvcFormat_GetByte() error!", __func__, NULL, NULL, NULL, NULL);
        }
        if (Rval == FORMAT_OK) {
            Rval = SvcFormat_GetBe32(Stream, &Tmp32);
        } else {
            AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s SvcFormat_GetBe32() error!", __func__, NULL, NULL, NULL, NULL);
        }
        if (Rval == FORMAT_OK) {
            Edts->ElstAtom.ElstTable1.SegmentDuration = (Edts->ElstAtom.EntryCount == 1U) ? 0U : Tmp32;
            Rval = SvcFormat_GetBe32(Stream, &Edts->ElstAtom.ElstTable1.MediaTime);
        } else {
            AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s SvcFormat_GetBe32() error!", __func__, NULL, NULL, NULL, NULL);
        }
        if (Rval == FORMAT_OK) {
            Rval = SvcFormat_GetBe32(Stream, &Edts->ElstAtom.ElstTable1.MediaRate);
        } else {
            AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s SvcFormat_GetBe32() error!", __func__, NULL, NULL, NULL, NULL);
        }
        if (Rval == FORMAT_OK) {
            if (Edts->ElstAtom.EntryCount > 1U) {
                Rval = SvcFormat_GetBe32(Stream, &Tmp32);
                if (Rval == FORMAT_OK) {
                    Rval = SvcFormat_GetBe32(Stream, &Edts->ElstAtom.ElstTable2.MediaTime);
                } else {
                    AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s SvcFormat_GetBe32() error!", __func__, NULL, NULL, NULL, NULL);
                }
                if (Rval == FORMAT_OK) {
                    Rval = SvcFormat_GetBe32(Stream, &Edts->ElstAtom.ElstTable2.MediaRate);
                    if (Rval != FORMAT_OK) {
                        AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s SvcFormat_GetBe32() error!", __func__, NULL, NULL, NULL, NULL);
                    }
                } else {
                    AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s SvcFormat_GetBe32() error!", __func__, NULL, NULL, NULL, NULL);
                }
            }
        } else {
            AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s SvcFormat_GetBe32() error!", __func__, NULL, NULL, NULL, NULL);
        }
    } else {
        AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s box size is wrong!", __func__, NULL, NULL, NULL, NULL);
        Rval = FORMAT_ERR_INVALID_FORMAT;
    }
    return Rval;
}

static UINT32 IsoDmx_ParseMdhd(SVC_STREAM_HDLR_s *Stream, ISO_MDHD_BOX_s *Mdhd, UINT32 BoxSize)
{
    UINT32 Rval = FORMAT_OK;
    if (BoxSize == 32U) {
        UINT8 Tmp8;
        Rval = SvcFormat_GetByte(Stream, &Tmp8);
        if (Rval == FORMAT_OK) {
            Rval = SvcFormat_GetByte(Stream, &Tmp8);
        } else {
            AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s SvcFormat_GetByte() error!", __func__, NULL, NULL, NULL, NULL);
        }
        if (Rval == FORMAT_OK) {
            Rval = SvcFormat_GetByte(Stream, &Tmp8);
        } else {
            AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s SvcFormat_GetByte() error!", __func__, NULL, NULL, NULL, NULL);
        }
        if (Rval == FORMAT_OK) {
            Rval = SvcFormat_GetByte(Stream, &Tmp8);
        } else {
            AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s SvcFormat_GetByte() error!", __func__, NULL, NULL, NULL, NULL);
        }
        if (Rval == FORMAT_OK) {
            Rval = SvcFormat_GetBe32(Stream, &Mdhd->CreateTime);
        } else {
            AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s SvcFormat_GetByte() error!", __func__, NULL, NULL, NULL, NULL);
        }
        if (Rval == FORMAT_OK) {
            Rval = SvcFormat_GetBe32(Stream, &Mdhd->ModifyTime);
        } else {
            AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s SvcFormat_GetBe32() error!", __func__, NULL, NULL, NULL, NULL);
        }
        if (Rval == FORMAT_OK) {
            Rval = SvcFormat_GetBe32(Stream, &Mdhd->TimeScale);
        } else {
            AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s SvcFormat_GetBe32() error!", __func__, NULL, NULL, NULL, NULL);
        }
        if (Rval == FORMAT_OK) {
            Rval = SvcFormat_GetBe32(Stream, &Mdhd->Duration);
        } else {
            AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s SvcFormat_GetBe32() error!", __func__, NULL, NULL, NULL, NULL);
        }
        if (Rval == FORMAT_OK) {
            Rval = SvcFormat_GetBe16(Stream, &Mdhd->Language);
        } else {
            AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s SvcFormat_GetBe32() error!", __func__, NULL, NULL, NULL, NULL);
        }
        if (Rval == FORMAT_OK) {
            Rval = SvcFormat_GetBe16(Stream, &Mdhd->Quality);
            if (Rval != FORMAT_OK) {
                AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s SvcFormat_GetBe16() error!", __func__, NULL, NULL, NULL, NULL);
            }
        } else {
            AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s SvcFormat_GetBe16() error!", __func__, NULL, NULL, NULL, NULL);
        }
    } else {
        AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s box size is wrong!", __func__, NULL, NULL, NULL, NULL);
        Rval = FORMAT_ERR_INVALID_FORMAT;
    }
    return Rval;
}

static UINT32 IsoDmx_ParseHdlr(SVC_STREAM_HDLR_s *Stream, ISO_HDLR_BOX_s *Hdlr, UINT32 BoxSize)
{
    UINT32 Rval = FORMAT_OK;
    if (BoxSize > 32U) {
        UINT8 Tmp8;
        UINT32 Count;
        Rval = SvcFormat_GetByte(Stream, &Tmp8);
        if (Rval == FORMAT_OK) {
            Rval = SvcFormat_GetByte(Stream, &Tmp8);
        } else {
            AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s SvcFormat_GetByte() error!", __func__, NULL, NULL, NULL, NULL);
        }
        if (Rval == FORMAT_OK) {
            Rval = SvcFormat_GetByte(Stream, &Tmp8);
        } else {
            AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s SvcFormat_GetByte() error!", __func__, NULL, NULL, NULL, NULL);
        }
        if (Rval == FORMAT_OK) {
            Rval = SvcFormat_GetByte(Stream, &Tmp8);
        } else {
            AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s SvcFormat_GetByte() error!", __func__, NULL, NULL, NULL, NULL);
        }
        if (Rval == FORMAT_OK) {
            Rval = SvcFormat_GetBe32(Stream, &Hdlr->ComType);
        } else {
            AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s SvcFormat_GetByte() error!", __func__, NULL, NULL, NULL, NULL);
        }
        if (Rval == FORMAT_OK) {
            Rval = SvcFormat_GetBe32(Stream, &Hdlr->ComSubType);
        } else {
            AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s SvcFormat_GetBe32() error!", __func__, NULL, NULL, NULL, NULL);
        }
        if (Rval == FORMAT_OK) {
            Rval = SvcFormat_GetBe32(Stream, &Hdlr->ComManufacturer);
        } else {
            AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s SvcFormat_GetBe32() error!", __func__, NULL, NULL, NULL, NULL);
        }
        if (Rval == FORMAT_OK) {
            Rval = SvcFormat_GetBe32(Stream, &Hdlr->ComFlags);
        } else {
            AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s SvcFormat_GetBe32() error!", __func__, NULL, NULL, NULL, NULL);
        }
        if (Rval == FORMAT_OK) {
            Rval = SvcFormat_GetBe32(Stream, &Hdlr->ComFlagMask);
        } else {
            AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s SvcFormat_GetBe32() error!", __func__, NULL, NULL, NULL, NULL);
        }
        if (Rval == FORMAT_OK) {
            UINT32 NameSize = BoxSize - 32U;
            UINT32 ReadSize = (NameSize > ISO_HDLR_MAX_COM_NAME_LEN)? ISO_HDLR_MAX_COM_NAME_LEN : NameSize;
            Rval = S2F(Stream->Func->Read(Stream, ReadSize, (UINT8 *)Hdlr->ComName, &Count));
            if (Rval == FORMAT_OK) {
                if (Count == ReadSize) {
                    if (NameSize > ReadSize) {
                        Hdlr->ComName[ISO_HDLR_MAX_COM_NAME_LEN - 1U] = '\0';
                        Rval = S2F(Stream->Func->Seek(Stream, (INT64)NameSize - (INT64)ReadSize, SVC_STREAM_SEEK_CUR));
                        if (Rval != FORMAT_OK) {
                            AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s Seek fail!", __func__, NULL, NULL, NULL, NULL);
                        }
                    }
                } else {
                    AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s Read data from stream error!", __func__, NULL, NULL, NULL, NULL);
                    Rval = FORMAT_ERR_NOT_ENOUGH_BYTES;
                }
            } else {
                AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s Read data from stream error!", __func__, NULL, NULL, NULL, NULL);
            }
        } else {
            AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s SvcFormat_GetBe32() error!", __func__, NULL, NULL, NULL, NULL);
        }
    } else {
        AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s box size is wrong!", __func__, NULL, NULL, NULL, NULL);
        Rval = FORMAT_ERR_INVALID_FORMAT;
    }
    return Rval;
}

static UINT32 IsoDmx_ParseVmhd(SVC_STREAM_HDLR_s *Stream, ISO_VMHD_BOX_s *Vmhd, UINT32 BoxSize)
{
    UINT32 Rval = FORMAT_OK;
    if (BoxSize == 20U) {
        UINT8 Tmp8;
        Rval = SvcFormat_GetByte(Stream, &Tmp8);
        if (Rval == FORMAT_OK) {
            Rval = SvcFormat_GetByte(Stream, &Tmp8);
        } else {
            AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s SvcFormat_GetByte() error!", __func__, NULL, NULL, NULL, NULL);
        }
        if (Rval == FORMAT_OK) {
            Rval = SvcFormat_GetByte(Stream, &Tmp8);
        } else {
            AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s SvcFormat_GetByte() error!", __func__, NULL, NULL, NULL, NULL);
        }
        if (Rval == FORMAT_OK) {
            Rval = SvcFormat_GetByte(Stream, &Tmp8);
        } else {
            AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s SvcFormat_GetByte() error!", __func__, NULL, NULL, NULL, NULL);
        }
        if (Rval == FORMAT_OK) {
            Rval = SvcFormat_GetBe16(Stream, &Vmhd->GraphicMode);
        } else {
            AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s SvcFormat_GetByte() error!", __func__, NULL, NULL, NULL, NULL);
        }
        if (Rval == FORMAT_OK) {
            Rval = SvcFormat_GetBe16(Stream, &Vmhd->Opcolor[0]);
        } else {
            AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s SvcFormat_GetBe16() error!", __func__, NULL, NULL, NULL, NULL);
        }
        if (Rval == FORMAT_OK) {
            Rval = SvcFormat_GetBe16(Stream, &Vmhd->Opcolor[1]);
        } else {
            AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s SvcFormat_GetBe16() error!", __func__, NULL, NULL, NULL, NULL);
        }
        if (Rval == FORMAT_OK) {
            Rval = SvcFormat_GetBe16(Stream, &Vmhd->Opcolor[2]);
            if (Rval != FORMAT_OK) {
                AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s SvcFormat_GetBe16() error!", __func__, NULL, NULL, NULL, NULL);
            }
        } else {
            AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s SvcFormat_GetBe16() error!", __func__, NULL, NULL, NULL, NULL);
        }
    } else {
        AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s box size is wrong!", __func__, NULL, NULL, NULL, NULL);
        Rval = FORMAT_ERR_INVALID_FORMAT;
    }
    return Rval;
}

static UINT32 IsoDmx_ParseSmhd(SVC_STREAM_HDLR_s *Stream, ISO_SMHD_BOX_s *Smhd, UINT32 BoxSize)
{
    UINT32 Rval = FORMAT_OK;
    if (BoxSize == 16U) {
        UINT8 Tmp8;
        Rval = SvcFormat_GetByte(Stream, &Tmp8);
        if (Rval == FORMAT_OK) {
            Rval = SvcFormat_GetByte(Stream, &Tmp8);
        } else {
            AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s SvcFormat_GetByte() error!", __func__, NULL, NULL, NULL, NULL);
        }
        if (Rval == FORMAT_OK) {
            Rval = SvcFormat_GetByte(Stream, &Tmp8);
        } else {
            AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s SvcFormat_GetByte() error!", __func__, NULL, NULL, NULL, NULL);
        }
        if (Rval == FORMAT_OK) {
            Rval = SvcFormat_GetByte(Stream, &Tmp8);
        } else {
            AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s SvcFormat_GetByte() error!", __func__, NULL, NULL, NULL, NULL);
        }
        if (Rval == FORMAT_OK) {
            Rval = SvcFormat_GetBe16(Stream, &Smhd->Balanced);
        } else {
            AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s SvcFormat_GetByte() error!", __func__, NULL, NULL, NULL, NULL);
        }
        if (Rval == FORMAT_OK) {
            Rval = SvcFormat_GetBe16(Stream, &Smhd->Reserv2);
            if (Rval != FORMAT_OK) {
                AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s SvcFormat_GetBe16() error!", __func__, NULL, NULL, NULL, NULL);
            }
        } else {
            AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s SvcFormat_GetBe16() error!", __func__, NULL, NULL, NULL, NULL);
        }
    } else {
        AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s box size is wrong!", __func__, NULL, NULL, NULL, NULL);
        Rval = FORMAT_ERR_INVALID_FORMAT;
    }
    return Rval;
}

static UINT32 IsoDmx_ParseDinf(SVC_STREAM_HDLR_s *Stream, ISO_DINF_BOX_s *Dinf, UINT32 BoxSize)
{
    UINT32 Rval = FORMAT_OK;
    if (BoxSize == 36U) {
        UINT8 Tmp8;
        UINT32 Tmp32;
        Rval = SvcFormat_GetBe32(Stream, &Tmp32);
        if (Rval == FORMAT_OK) {
            Rval = SvcFormat_GetBe32(Stream, &Tmp32);
        } else {
            AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s SvcFormat_GetBe32() error!", __func__, NULL, NULL, NULL, NULL);
        }
        if (Rval == FORMAT_OK) {
            Rval = SvcFormat_GetByte(Stream, &Tmp8);
        } else {
            AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s SvcFormat_GetBe32() error!", __func__, NULL, NULL, NULL, NULL);
        }
        if (Rval == FORMAT_OK) {
            Rval = SvcFormat_GetByte(Stream, &Tmp8);
        } else {
            AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s SvcFormat_GetByte() error!", __func__, NULL, NULL, NULL, NULL);
        }
        if (Rval == FORMAT_OK) {
            Rval = SvcFormat_GetByte(Stream, &Tmp8);
        } else {
            AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s SvcFormat_GetByte() error!", __func__, NULL, NULL, NULL, NULL);
        }
        if (Rval == FORMAT_OK) {
            Rval = SvcFormat_GetByte(Stream, &Tmp8);
        } else {
            AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s SvcFormat_GetByte() error!", __func__, NULL, NULL, NULL, NULL);
        }
        if (Rval == FORMAT_OK) {
            Rval = SvcFormat_GetBe32(Stream, &Dinf->DrefAtom.EntryCount);
        } else {
            AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s SvcFormat_GetByte() error!", __func__, NULL, NULL, NULL, NULL);
        }
        if (Rval == FORMAT_OK) {
            Rval = SvcFormat_GetBe32(Stream, &Dinf->DrefAtom.DrefTable.Box.Size);
        } else {
            AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s SvcFormat_GetBe32() error!", __func__, NULL, NULL, NULL, NULL);
        }
        if (Rval == FORMAT_OK) {
            Rval = SvcFormat_GetBe32(Stream, &Dinf->DrefAtom.DrefTable.Box.Type);
        } else {
            AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s SvcFormat_GetBe32() error!", __func__, NULL, NULL, NULL, NULL);
        }
        if (Rval == FORMAT_OK) {
            Rval = SvcFormat_GetByte(Stream, &Dinf->DrefAtom.DrefTable.Version);
        } else {
            AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s SvcFormat_GetBe32() error!", __func__, NULL, NULL, NULL, NULL);
        }
        if (Rval == FORMAT_OK) {
            Rval = IsoDmx_GetBuffer8(Stream, Dinf->DrefAtom.DrefTable.Flags, 3U);
            if (Rval != FORMAT_OK) {
                AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s SvcFormat_GetByte() error!", __func__, NULL, NULL, NULL, NULL);
            }
        } else {
            AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s SvcFormat_GetByte() error!", __func__, NULL, NULL, NULL, NULL);
        }
    } else {
        AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s box size is wrong!", __func__, NULL, NULL, NULL, NULL);
        Rval = FORMAT_ERR_INVALID_FORMAT;
    }
    return Rval;
}

static UINT32 IsoDmx_ParseAvccSps(SVC_STREAM_HDLR_s *Stream, ISO_STSD_VIDEO_BOX_s *Stsd)
{
    UINT32 Rval = SvcFormat_GetByte(Stream, &Stsd->VideoEntry.DecCfg.Avcc.SPSNum);
    if (Rval == FORMAT_OK) {
        Rval = SvcFormat_GetBe16(Stream, &Stsd->VideoEntry.DecCfg.Avcc.SPSLength);
    }
    if (Rval == FORMAT_OK) {
        UINT16 i;
        for (i = 0U; i < Stsd->VideoEntry.DecCfg.Avcc.SPSLength; i++) {
            Rval = SvcFormat_GetByte(Stream, &Stsd->VideoEntry.DecCfg.Avcc.SPSCtx[i]);
            if (Rval != FORMAT_OK) {
                break;
            }
        }
    }
    return Rval;
}
static UINT32 IsoDmx_ParseAvccPps(SVC_STREAM_HDLR_s *Stream, ISO_STSD_VIDEO_BOX_s *Stsd)
{
    UINT32 Rval = SvcFormat_GetByte(Stream, &Stsd->VideoEntry.DecCfg.Avcc.PPSNum);
    if (Rval == FORMAT_OK) {
        Rval = SvcFormat_GetBe16(Stream, &Stsd->VideoEntry.DecCfg.Avcc.PPSLength);
    }
    if (Rval == FORMAT_OK) {
        UINT16 i;
        for (i = 0U; i < Stsd->VideoEntry.DecCfg.Avcc.PPSLength; i++) {
            Rval = SvcFormat_GetByte(Stream, &Stsd->VideoEntry.DecCfg.Avcc.PPSCtx[i]);
            if (Rval != FORMAT_OK) {
                break;
            }
        }
    }
    return Rval;
}

static UINT32 IsoDmx_ParseAvccHeader(SVC_STREAM_HDLR_s *Stream, ISO_STSD_VIDEO_BOX_s *Stsd, UINT16 *SarWidth, UINT16 *SarHeight)
{
    UINT32 Rval;
    /* sps */
    Rval = IsoDmx_ParseAvccSps(Stream, Stsd);
    if (Rval == FORMAT_OK) {
        /* pps */
        Rval = IsoDmx_ParseAvccPps(Stream, Stsd);
        if (Rval != FORMAT_OK) {
            AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s IsoDmx_ParseAvccPps() error!", __func__, NULL, NULL, NULL, NULL);
        }
    } else {
        AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s IsoDmx_ParseAvccSps() error!", __func__, NULL, NULL, NULL, NULL);
    }

    if (Rval == FORMAT_OK) {
        UINT8 H264HeaderBuffer[8U + SVC_FORMAT_MAX_SPS_LENGTH + SVC_FORMAT_MAX_PPS_LENGTH] = {0};
        const UINT8 H264Header[4] = {0U, 0U, 0U, 1U};
        UINT32 SPSPPSSize = 0U;
        Rval = W2F(AmbaWrap_memcpy(H264HeaderBuffer, H264Header, 4));
        if (Rval == FORMAT_OK) {
            SPSPPSSize += 4U;
            Rval = W2F(AmbaWrap_memcpy(&H264HeaderBuffer[SPSPPSSize], (UINT8 *)Stsd->VideoEntry.DecCfg.Avcc.SPSCtx, Stsd->VideoEntry.DecCfg.Avcc.SPSLength));
        }
        if (Rval == FORMAT_OK) {
            SPSPPSSize += Stsd->VideoEntry.DecCfg.Avcc.SPSLength;
            Rval = W2F(AmbaWrap_memcpy(&H264HeaderBuffer[SPSPPSSize], H264Header, 4));
        }
        if (Rval == FORMAT_OK) {
            SPSPPSSize += 4U;
            Rval = W2F(AmbaWrap_memcpy(&H264HeaderBuffer[SPSPPSSize], (UINT8 *)Stsd->VideoEntry.DecCfg.Avcc.PPSCtx, Stsd->VideoEntry.DecCfg.Avcc.PPSLength));
        }
        if (Rval == FORMAT_OK) {
            SPSPPSSize += Stsd->VideoEntry.DecCfg.Avcc.PPSLength;
            if (SPSPPSSize > sizeof(H264HeaderBuffer)) {
                AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s SPS PPS size error!", __func__, NULL, NULL, NULL, NULL);
                Rval = FORMAT_ERR_INVALID_FORMAT;
            } else {
                SVC_H264_SPS_s H264SPS = {0};
                SVC_H264_PPS_s H264PPS = {0};
                SVC_H264_SliceInfo_s H264SliceInfo = {0};
                Rval = SvcH264_HeaderParse(H264HeaderBuffer, SPSPPSSize, &H264SPS, &H264PPS, &H264SliceInfo);
                if (Rval == FORMAT_OK) {
                    *SarWidth = H264SPS.VuiParam.SarWidth;
                    *SarHeight = H264SPS.VuiParam.SarHeight;
                } else {
                    Rval = FORMAT_ERR_INVALID_FORMAT;
                    AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s SvcH264_HeaderParse() error!", __func__, NULL, NULL, NULL, NULL);
                }
            }
        }
    }
    return Rval;
}

static UINT32 IsoDmx_ParseVideoAvcc(SVC_STREAM_HDLR_s *Stream, ISO_STSD_VIDEO_BOX_s *Stsd, UINT16 *SarWidth, UINT16 *SarHeight)
{
    UINT32 Rval = FORMAT_OK;
    Rval = SvcFormat_GetByte(Stream, &Stsd->VideoEntry.DecCfg.Avcc.ConfigVersion);
    if (Rval == FORMAT_OK) {
        Rval = SvcFormat_GetByte(Stream, &Stsd->VideoEntry.DecCfg.Avcc.AVCProfileIndication);
    } else {
        AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s SvcFormat_GetByte() error!", __func__, NULL, NULL, NULL, NULL);
    }
    if (Rval == FORMAT_OK) {
        Rval = SvcFormat_GetByte(Stream, &Stsd->VideoEntry.DecCfg.Avcc.ProfileCompatibility);
    } else {
        AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s SvcFormat_GetByte() error!", __func__, NULL, NULL, NULL, NULL);
    }
    if (Rval == FORMAT_OK) {
        Rval = SvcFormat_GetByte(Stream, &Stsd->VideoEntry.DecCfg.Avcc.AVCLevelIndication);
    } else {
        AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s SvcFormat_GetByte() error!", __func__, NULL, NULL, NULL, NULL);
    }
    if (Rval == FORMAT_OK) {
        Rval = SvcFormat_GetByte(Stream, &Stsd->VideoEntry.DecCfg.Avcc.NALULength);
    } else {
        AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s SvcFormat_GetByte() error!", __func__, NULL, NULL, NULL, NULL);
    }
    if (Rval == FORMAT_OK) {
        Rval = IsoDmx_ParseAvccHeader(Stream, Stsd, SarWidth, SarHeight);
        if (Rval != FORMAT_OK) {
            AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s IsoDmx_ParseAvccHeader() error!", __func__, NULL, NULL, NULL, NULL);
        }
    } else {
        AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s SvcFormat_GetByte() error!", __func__, NULL, NULL, NULL, NULL);
    }
    return Rval;
}

static UINT32 IsoDmx_ParseHvccVps(SVC_STREAM_HDLR_s *Stream, ISO_STSD_VIDEO_BOX_s *Stsd)
{
    UINT8 Tmp8;
    UINT32 Rval = SvcFormat_GetByte(Stream, &Tmp8);
    if (Rval == FORMAT_OK) {
        Rval = SvcFormat_GetBe16(Stream, &Stsd->VideoEntry.DecCfg.Hvcc.VPSNum);
    }
    if (Rval == FORMAT_OK) {
        Rval = SvcFormat_GetBe16(Stream, &Stsd->VideoEntry.DecCfg.Hvcc.VPSLength);
    }
    if (Rval == FORMAT_OK) {
        UINT16 i;
        for (i = 0U; i < Stsd->VideoEntry.DecCfg.Hvcc.VPSLength; i++) {
            Rval = SvcFormat_GetByte(Stream, &Stsd->VideoEntry.DecCfg.Hvcc.VPSCtx[i]);
            if (Rval != FORMAT_OK) {
                break;
            }
        }
    }

    return Rval;
}

static UINT32 IsoDmx_ParseHvccSps(SVC_STREAM_HDLR_s *Stream, ISO_STSD_VIDEO_BOX_s *Stsd)
{
    UINT8 Tmp8;
    UINT32 Rval = SvcFormat_GetByte(Stream, &Tmp8);
    if (Rval == FORMAT_OK) {
        Rval = SvcFormat_GetBe16(Stream, &Stsd->VideoEntry.DecCfg.Hvcc.SPSNum);
    }
    if (Rval == FORMAT_OK) {
        Rval = SvcFormat_GetBe16(Stream, &Stsd->VideoEntry.DecCfg.Hvcc.SPSLength);
    }
    if (Rval == FORMAT_OK) {
        UINT16 i;
        for (i = 0U; i < Stsd->VideoEntry.DecCfg.Hvcc.SPSLength; i++) {
            Rval = SvcFormat_GetByte(Stream, &Stsd->VideoEntry.DecCfg.Hvcc.SPSCtx[i]);
            if (Rval != FORMAT_OK) {
                break;
            }
        }
    }
    return Rval;
}

static UINT32 IsoDmx_ParseHvccPps(SVC_STREAM_HDLR_s *Stream, ISO_STSD_VIDEO_BOX_s *Stsd)
{
    UINT8 Tmp8;
    UINT32 Rval = SvcFormat_GetByte(Stream, &Tmp8);
    if (Rval == FORMAT_OK) {
        Rval = SvcFormat_GetBe16(Stream, &Stsd->VideoEntry.DecCfg.Hvcc.PPSNum);
    }
    if (Rval == FORMAT_OK) {
        Rval = SvcFormat_GetBe16(Stream, &Stsd->VideoEntry.DecCfg.Hvcc.PPSLength);
    }
    if (Rval == FORMAT_OK) {
        UINT16 i;
        for (i = 0U; i < Stsd->VideoEntry.DecCfg.Hvcc.PPSLength; i++) {
            Rval = SvcFormat_GetByte(Stream, &Stsd->VideoEntry.DecCfg.Hvcc.PPSCtx[i]);
            if (Rval != FORMAT_OK) {
                break;
            }
        }
    }
    return Rval;
}

static UINT32 IsoDmx_ParseHvccHeader(SVC_STREAM_HDLR_s *Stream, ISO_STSD_VIDEO_BOX_s *Stsd, UINT16 *SarWidth, UINT16 *SarHeight)
{
    UINT32 Rval;
    /*vps*/
    Rval = IsoDmx_ParseHvccVps(Stream, Stsd);
    if (Rval == FORMAT_OK) {
        /*sps*/
        Rval = IsoDmx_ParseHvccSps(Stream, Stsd);
        if (Rval == FORMAT_OK) {
            /*pps*/
            Rval = IsoDmx_ParseHvccPps(Stream, Stsd);
            if (Rval != FORMAT_OK) {
                AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s IsoDmx_ParseHvccPps() error!", __func__, NULL, NULL, NULL, NULL);
            }
        } else {
            AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s IsoDmx_ParseHvccSps() error!", __func__, NULL, NULL, NULL, NULL);
        }
    } else {
        AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s IsoDmx_ParseHvccVps() error!", __func__, NULL, NULL, NULL, NULL);
    }
    if (Rval == FORMAT_OK) {
        UINT8 H265HeaderBuffer[12U + SVC_FORMAT_MAX_VPS_LENGTH + SVC_FORMAT_MAX_SPS_LENGTH + SVC_FORMAT_MAX_PPS_LENGTH] = {0};
        const UINT8 H265Header[4] = {0U, 0U, 0U, 1U};
        UINT32 VPSSPSPPSSize = 0U;
        Rval = W2F(AmbaWrap_memcpy(H265HeaderBuffer, H265Header, 4));
        if (Rval == FORMAT_OK) {
            VPSSPSPPSSize += 4U;
            Rval = W2F(AmbaWrap_memcpy(&H265HeaderBuffer[VPSSPSPPSSize], (UINT8 *)Stsd->VideoEntry.DecCfg.Hvcc.VPSCtx, Stsd->VideoEntry.DecCfg.Hvcc.VPSLength));
        }
        if (Rval == FORMAT_OK) {
            VPSSPSPPSSize += Stsd->VideoEntry.DecCfg.Hvcc.VPSLength;
            Rval = W2F(AmbaWrap_memcpy(&H265HeaderBuffer[VPSSPSPPSSize], H265Header, 4));
        }
        if (Rval == FORMAT_OK) {
            VPSSPSPPSSize += 4U;
            Rval = W2F(AmbaWrap_memcpy(&H265HeaderBuffer[VPSSPSPPSSize], (UINT8 *)Stsd->VideoEntry.DecCfg.Hvcc.SPSCtx, Stsd->VideoEntry.DecCfg.Hvcc.SPSLength));
        }
        if (Rval == FORMAT_OK) {
            VPSSPSPPSSize += Stsd->VideoEntry.DecCfg.Hvcc.SPSLength;
            Rval = W2F(AmbaWrap_memcpy(&H265HeaderBuffer[VPSSPSPPSSize], H265Header, 4));
        }
        if (Rval == FORMAT_OK) {
            VPSSPSPPSSize += 4U;
            Rval = W2F(AmbaWrap_memcpy(&H265HeaderBuffer[VPSSPSPPSSize], (UINT8 *)Stsd->VideoEntry.DecCfg.Hvcc.PPSCtx, Stsd->VideoEntry.DecCfg.Hvcc.PPSLength));
        }
        if (Rval == FORMAT_OK) {
            VPSSPSPPSSize += Stsd->VideoEntry.DecCfg.Hvcc.PPSLength;
            if (VPSSPSPPSSize > sizeof(H265HeaderBuffer)) {
                AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s VPS SPS PPS size error!", __func__, NULL, NULL, NULL, NULL);
                Rval = FORMAT_ERR_INVALID_FORMAT;
            } else {
                SVC_H265_VPS_s H265VPS = {0};
                SVC_H265_SPS_s H265SPS = {0};
                Rval = SvcH265_HeaderParse(H265HeaderBuffer, VPSSPSPPSSize, &H265VPS, &H265SPS);
                if (Rval == FORMAT_OK) {
                    Stsd->VideoEntry.DecCfg.Hvcc.PicSizeInCtbsY = H265SPS.PicSizeInCtbsY;  // for svc
                    *SarWidth = H265SPS.VuiParam.SarWidth;
                    *SarHeight = H265SPS.VuiParam.SarHeight;
                } else {
                    Rval = FORMAT_ERR_INVALID_FORMAT;
                    AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s SvcH265_HeaderParse() error!", __func__, NULL, NULL, NULL, NULL);
                }
            }
        } else {
            AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s AmbaWrap_memcpy() error!", __func__, NULL, NULL, NULL, NULL);
        }
    }
    return Rval;
}
static UINT32 IsoDmx_ParseVideoHvcc(SVC_STREAM_HDLR_s *Stream, ISO_STSD_VIDEO_BOX_s *Stsd, UINT16 *SarWidth, UINT16 *SarHeight)
{
    UINT16 Tmp16;
    UINT8 Tmp8;
    UINT32 Rval = SvcFormat_GetByte(Stream, &Stsd->VideoEntry.DecCfg.Hvcc.ConfigVersion);
    if (Rval == FORMAT_OK) {
        Rval = IsoDmx_GetBuffer8(Stream, Stsd->VideoEntry.DecCfg.Hvcc.Generalconfiguration, SVC_FORMAT_MAX_VPS_GEN_CFG_LENGTH);
    } else {
        AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s SvcFormat_GetByte() error!", __func__, NULL, NULL, NULL, NULL);
    }
    if (Rval == FORMAT_OK) {
        Rval = SvcFormat_GetBe16(Stream, &Tmp16);
    }
    if (Rval == FORMAT_OK) {
        Rval = SvcFormat_GetByte(Stream, &Tmp8);
    } else {
        AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s SvcFormat_GetBe16() error!", __func__, NULL, NULL, NULL, NULL);
    }
    if (Rval == FORMAT_OK) {
        Rval = SvcFormat_GetByte(Stream, &Stsd->VideoEntry.DecCfg.Hvcc.ChromaFormat);
    } else {
        AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s SvcFormat_GetByte() error!", __func__, NULL, NULL, NULL, NULL);
    }
    if (Rval == FORMAT_OK) {
        Rval = SvcFormat_GetByte(Stream, &Stsd->VideoEntry.DecCfg.Hvcc.BitDepthLumaMinus8);
    } else {
        AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s SvcFormat_GetByte() error!", __func__, NULL, NULL, NULL, NULL);
    }
    if (Rval == FORMAT_OK) {
        Rval = SvcFormat_GetByte(Stream, &Stsd->VideoEntry.DecCfg.Hvcc.BitDepthChromaMinus8);
    } else {
        AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s SvcFormat_GetByte() error!", __func__, NULL, NULL, NULL, NULL);
    }
    if (Rval == FORMAT_OK) {
        Rval = SvcFormat_GetBe16(Stream, &Tmp16);
    } else {
        AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s SvcFormat_GetByte() error!", __func__, NULL, NULL, NULL, NULL);
    }
    if (Rval == FORMAT_OK) {
        Rval = SvcFormat_GetByte(Stream, &Stsd->VideoEntry.DecCfg.Hvcc.NALULength);
    } else {
        AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s SvcFormat_GetBe16() error!", __func__, NULL, NULL, NULL, NULL);
    }
    if (Rval == FORMAT_OK) {
        Rval = SvcFormat_GetByte(Stream, &Tmp8);
    } else {
        AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s SvcFormat_GetByte() error!", __func__, NULL, NULL, NULL, NULL);
    }
    if (Rval == FORMAT_OK) {
        Rval = IsoDmx_ParseHvccHeader(Stream, Stsd, SarWidth, SarHeight);
        if (Rval != FORMAT_OK) {
            AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s IsoDmx_ParseHvccHeader() error!", __func__, NULL, NULL, NULL, NULL);
        }
    } else {
        AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s SvcFormat_GetByte() error!", __func__, NULL, NULL, NULL, NULL);
    }
    return Rval;
}

static UINT32 IsoDmx_ParseVideoSampleCoding(SVC_VIDEO_TRACK_INFO_s *Video, SVC_STREAM_HDLR_s *Stream, ISO_STSD_VIDEO_BOX_s *Stsd, UINT64 ParseStartPos, UINT64 ParseEndPos)
{
    UINT32 Rval = FORMAT_OK;
    UINT64 ParsePos = ParseStartPos;
    UINT64 RemainSize = ParseEndPos - ParseStartPos;
    do {
        UINT32 BoxSize, BoxTag;
        Rval = IsoDmx_GetBoxHeader(Stream, &BoxSize, &BoxTag);
        if (Rval == FORMAT_OK) {
            if (RemainSize < BoxSize){
                AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s IsoDmx_ParseVideoStsd() fail!", __func__, NULL, NULL, NULL, NULL);
                Rval = FORMAT_ERR_INVALID_FORMAT;
            } else {
                if (BoxTag == TAG_AVCC) {
                    Stsd->VideoEntry.DecCfg.Avcc.Box.Size = BoxSize;
                    Stsd->VideoEntry.DecCfg.Avcc.Box.Type = BoxTag;
                    Rval = IsoDmx_ParseVideoAvcc(Stream, Stsd, &Video->PixelArX, &Video->PixelArY);
                    if (Rval != FORMAT_OK){
                        AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s IsoDmx_ParseVideoAvcc() error!", __func__, NULL, NULL, NULL, NULL);
                    } else {
                        AmbaPrint_ModulePrintUInt5(SVC_DEMUXER_PRINT_MODULE_ID, "IsoDmx_ParseVideoStsd : w=%u, h=%u, ar_x=%u, ar_y=%u", Video->Width, Video->Height, Video->PixelArX, Video->PixelArY, 0U);
                        if ((Video->PixelArX != 0U) && (Video->PixelArY != 0U)) {
                            //K_ASSERT((video->Width * video->PixelArX) % video->PixelArY == 0);
                            Video->Width = (Video->Width * Video->PixelArX) / Video->PixelArY;
                        }
                        //AmbaPrint("%s : w=%u, h=%u", __func__, video->Width, video->Height);
                    }
                } else if (BoxTag == TAG_HVCC) {
                    Stsd->VideoEntry.DecCfg.Hvcc.Box.Size = BoxSize;
                    Stsd->VideoEntry.DecCfg.Hvcc.Box.Type = BoxTag;
                    Rval = IsoDmx_ParseVideoHvcc(Stream, Stsd, &Video->PixelArX, &Video->PixelArY);
                    if (Rval != FORMAT_OK){
                        AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s IsoDmx_ParseVideoHvcc() error!", __func__, NULL, NULL, NULL, NULL);
                    } else {
                        //AmbaPrint("%s : w=%u, h=%u, ar_x=%u, ar_y=%u", __func__, video->Width, video->Height, video->PixelArX, video->PixelArY);
                        if ((Video->PixelArX != 0U) && (Video->PixelArY != 0U)) {
                            //K_ASSERT((video->Width * video->PixelArX) % video->PixelArY == 0);
                            Video->Width = (Video->Width * Video->PixelArX) / Video->PixelArY;
                        }
                        //AmbaPrint("%s : w=%u, h=%u", __func__, video->Width, video->Height);
                    }
                } else {
                    Rval = S2F(Stream->Func->Seek(Stream, ((INT64) ParsePos + (INT64) BoxSize), SVC_STREAM_SEEK_START));
                    if (Rval != FORMAT_OK) {
                        AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s Seek fail!", __func__, NULL, NULL, NULL, NULL);
                    }
                }
                if (Rval == FORMAT_OK) {
                    ParsePos += BoxSize;
                    RemainSize -= BoxSize;
                    if (RemainSize == 0U) {
                        break;
                    }
                }
            }
        } else {
            AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s Get box header error!", __func__, NULL, NULL, NULL, NULL);
        }
    } while (Rval == FORMAT_OK);
    return Rval;
}

static UINT32 IsoDmx_ParseVideoSampleEntry(SVC_VIDEO_TRACK_INFO_s *Video, SVC_STREAM_HDLR_s *Stream, ISO_STSD_VIDEO_BOX_s *Stsd, UINT64 ParseStartPos, UINT64 ParseEndPos)
{
    UINT32 Rval = SvcFormat_GetBe32(Stream, &Stsd->VideoEntry.Box.Size);
    if (Rval == FORMAT_OK) {
        Rval = SvcFormat_GetBe32(Stream, &Stsd->VideoEntry.Box.Type);
    }
    if (Rval == FORMAT_OK) {
        UINT8 Tmp8[6];
        Rval = IsoDmx_GetBuffer8(Stream, Tmp8, 6U);
    }
    if (Rval == FORMAT_OK) {
        Rval = SvcFormat_GetBe16(Stream, &Stsd->VideoEntry.DataRefIdx);
    }
    if (Rval == FORMAT_OK) {
        Rval = SvcFormat_GetBe16(Stream, &Stsd->VideoEntry.Version);
    }
    if (Rval == FORMAT_OK) {
        Rval = SvcFormat_GetBe16(Stream, &Stsd->VideoEntry.Revison);
    }
    if (Rval == FORMAT_OK) {
        Rval = IsoDmx_GetBufferBe32(Stream, Stsd->VideoEntry.Predefined, 3U);
    }
    if (Rval == FORMAT_OK) {
        Rval = SvcFormat_GetBe16(Stream, &Stsd->VideoEntry.Width);
    }
    if (Rval == FORMAT_OK) {
        Video->Width = Stsd->VideoEntry.Width;
        Rval = SvcFormat_GetBe16(Stream, &Stsd->VideoEntry.Height);
    }
    if (Rval == FORMAT_OK) {
        Video->Height = Stsd->VideoEntry.Height;
        Rval = SvcFormat_GetBe32(Stream, &Stsd->VideoEntry.HorRes);
    }
    if (Rval == FORMAT_OK) {
        Rval = SvcFormat_GetBe32(Stream, &Stsd->VideoEntry.VerRes);
    }
    if (Rval == FORMAT_OK) {
        UINT32 Tmp32;
        Rval = SvcFormat_GetBe32(Stream, &Tmp32);
    }
    if (Rval == FORMAT_OK) {
        Rval = SvcFormat_GetBe16(Stream, &Stsd->VideoEntry.FrameCount);
    }
    if (Rval == FORMAT_OK) {
        UINT32 Count = 0U;
        Rval = S2F(Stream->Func->Read(Stream, ISO_STSD_MAX_ENC_NAME_LEN, (UINT8 *)Stsd->VideoEntry.Name, &Count));
        if (Rval == FORMAT_OK) {
            if (Count != ISO_STSD_MAX_ENC_NAME_LEN) {
                AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s Read data from stream error!", __func__, NULL, NULL, NULL, NULL);
                Rval = FORMAT_ERR_NOT_ENOUGH_BYTES;
            }
        } else {
            AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s Read data from stream error!", __func__, NULL, NULL, NULL, NULL);
        }
    }
    if (Rval == FORMAT_OK) {
        Rval = SvcFormat_GetBe16(Stream, &Stsd->VideoEntry.Depth);
    }
    if (Rval == FORMAT_OK) {
        Rval = SvcFormat_GetBe16(Stream, &Stsd->VideoEntry.Color);
    }
    if (Rval == FORMAT_OK) {
        Rval = IsoDmx_ParseVideoSampleCoding(Video, Stream, Stsd, ParseStartPos + 86U, ParseEndPos);
    }
    return Rval;
}

static UINT32 IsoDmx_ParseVideoStsd(SVC_VIDEO_TRACK_INFO_s *Video, SVC_STREAM_HDLR_s *Stream, ISO_STSD_VIDEO_BOX_s *Stsd, UINT64 ParseStartPos, UINT64 ParseEndPos)
{
    UINT8 Tmp8;
    UINT32 Rval = SvcFormat_GetByte(Stream, &Tmp8);
    if (Rval == FORMAT_OK) {
        Rval = SvcFormat_GetByte(Stream, &Tmp8);
    }
    if (Rval == FORMAT_OK) {
        Rval = SvcFormat_GetByte(Stream, &Tmp8);
    }
    if (Rval == FORMAT_OK) {
        Rval = SvcFormat_GetByte(Stream, &Tmp8);
    }
    if (Rval == FORMAT_OK) {
        Rval = SvcFormat_GetBe32(Stream, &Stsd->EntryCount);
    }
    if (Rval == FORMAT_OK) {
        Rval = IsoDmx_ParseVideoSampleEntry (Video, Stream, Stsd, ParseStartPos + 8U, ParseEndPos);
        if (Rval != FORMAT_OK) {
            AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s IsoDmx_ParseVideoSampleEntry fail!", __func__, NULL, NULL, NULL, NULL);
        }
    }
    if (Rval == FORMAT_OK) {
        Rval = S2F(Stream->Func->Seek(Stream, (INT64)ParseEndPos, SVC_STREAM_SEEK_START));
        if (Rval != FORMAT_OK) {
            AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s Seek fail!", __func__, NULL, NULL, NULL, NULL);
        }
    } else {
        AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s Parse fail!", __func__, NULL, NULL, NULL, NULL);
    }
    return Rval;
}

static UINT32 IsoDmx_ParseAudioAAC(SVC_STREAM_HDLR_s *Stream, ISO_STSD_AUDIO_BOX_s *Stsd, UINT32 BoxSize)
{
    UINT32 Rval = FORMAT_OK;
    if (BoxSize == 102U) {
        Rval = SvcFormat_GetBe16(Stream, &Stsd->AudioEntry.Channels);
        if (Rval == FORMAT_OK) {
            Rval = SvcFormat_GetBe16(Stream, &Stsd->AudioEntry.BitsPerSample);
        }
        if (Rval == FORMAT_OK) {
            Rval = SvcFormat_GetBe16(Stream, &Stsd->AudioEntry.CompressedID);
        }
        if (Rval == FORMAT_OK) {
            Rval = SvcFormat_GetBe16(Stream, &Stsd->AudioEntry.PacketSize);
        }
        if (Rval == FORMAT_OK) {
            UINT32 Tmp32 = 0U;
            Rval = SvcFormat_GetBe32(Stream, &Tmp32);
            if (Rval == FORMAT_OK) {
                Stsd->AudioEntry.SampleRate = Tmp32 >> 16U;
            }
        }
        /* skip to box end at IsoDmx_ParseAudioStsd */
        if (Rval != FORMAT_OK) {
            AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s SvcFormat_Get() error!", __func__, NULL, NULL, NULL, NULL);
        }
    } else {
        AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s box size is wrong!", __func__, NULL, NULL, NULL, NULL);
        Rval = FORMAT_ERR_INVALID_FORMAT;
    }
    return Rval;
}

static UINT32 IsoDmx_ParseAudioADPCM(SVC_STREAM_HDLR_s *Stream, ISO_STSD_AUDIO_BOX_s *Stsd, UINT32 BoxSize)
{
    UINT32 Rval = FORMAT_OK;
    if (BoxSize == 124U) {
        UINT32 Tmp32 = 0U;
        Rval = SvcFormat_GetBe16(Stream, &Stsd->AudioEntry.Channels);
        if (Rval == FORMAT_OK) {
            Rval = SvcFormat_GetBe16(Stream, &Stsd->AudioEntry.BitsPerSample);
        }
        if (Rval == FORMAT_OK) {
            Rval = SvcFormat_GetBe16(Stream, &Stsd->AudioEntry.CompressedID);
        }
        if (Rval == FORMAT_OK) {
            Rval = SvcFormat_GetBe16(Stream, &Stsd->AudioEntry.PacketSize);
        }
        if (Rval == FORMAT_OK) {
            Rval = SvcFormat_GetBe32(Stream, &Tmp32);
        }
        if (Rval == FORMAT_OK) {
            Stsd->AudioEntry.SampleRate = Tmp32 >> 16U;
            Rval = SvcFormat_GetBe32(Stream, &Stsd->AudioEntry.SamplesPerChunk);
        }
        if (Rval == FORMAT_OK) {
            // Bytes per packet
            Rval = SvcFormat_GetBe32(Stream, &Tmp32);
        }
        if (Rval == FORMAT_OK) {
            // Bytes per frame
            Rval = SvcFormat_GetBe32(Stream, &Stsd->AudioEntry.BytesPerFrame);
        }
        if (Rval == FORMAT_OK) {
            // Bytes per sample
            Rval = SvcFormat_GetBe32(Stream, &Tmp32);
        }
        /* skip to box end at IsoDmx_ParseAudioStsd */
        if (Rval != FORMAT_OK) {
            AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s SvcFormat_Get() error!", __func__, NULL, NULL, NULL, NULL);
        }
    } else {
        AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s box size is wrong!", __func__, NULL, NULL, NULL, NULL);
        Rval = FORMAT_ERR_INVALID_FORMAT;
    }
    return Rval;
}

static UINT32 IsoDmx_ParseAudioLPCM(SVC_STREAM_HDLR_s *Stream, ISO_STSD_AUDIO_BOX_s *Stsd, UINT32 BoxSize)
{
    UINT32 Rval;
    if (BoxSize == 88U) {
        UINT32 Tmp32;
        // reserved; always 3 and 16
        Rval = SvcFormat_GetBe32(Stream, &Tmp32);
        if (Rval == FORMAT_OK) {
            // always -2 and 0
            Rval = SvcFormat_GetBe32(Stream, &Tmp32);
        }
        if (Rval == FORMAT_OK) {
            // always 65536
            Rval = SvcFormat_GetBe32(Stream, &Tmp32);
        }
        if (Rval == FORMAT_OK) {
            // sizeof(SoundDescriptionV2)
            Rval = SvcFormat_GetBe32(Stream, &Tmp32);
        }
        if (Rval == FORMAT_OK) {
            INT32 i;
            UINT8 SampleBuf[8] = {0};
            // audioSampleRate in 64-bit floating-point number
            for (i = 7; i >= 0; i--) {
                Rval = SvcFormat_GetByte(Stream, &SampleBuf[i]);
                if (Rval != FORMAT_OK) {
                    break;
                }
            }
            if (Rval == FORMAT_OK) {
                DOUBLE Samples = 0.0;
                Rval = W2F(AmbaWrap_memcpy(&Samples, SampleBuf, sizeof(Samples)));
                if (Rval == FORMAT_OK) {
                    Stsd->AudioEntry.SampleRate = (UINT32)Samples;
                }
            }
        }
        if (Rval == FORMAT_OK) {
            // channel
            Rval = SvcFormat_GetBe32(Stream, &Tmp32);
        }
        if (Rval == FORMAT_OK) {
            Stsd->AudioEntry.Channels = (UINT16) Tmp32;
            // always 0x7f000000
            Rval = SvcFormat_GetBe32(Stream, &Tmp32);
        }
        if (Rval == FORMAT_OK) {
            // constBitsPerChannel
            Rval = SvcFormat_GetBe32(Stream, &Tmp32);
        }
        if (Rval == FORMAT_OK) {
            Stsd->AudioEntry.BitsPerSample = (UINT16) Tmp32;
            // format Specific flags
            Rval = SvcFormat_GetBe32(Stream, &Tmp32);
        }
        /* skip to box end at IsoDmx_ParseAudioStsd */
        if (Rval != FORMAT_OK) {
            AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s SvcFormat_Get() error!", __func__, NULL, NULL, NULL, NULL);
        }
    } else {
        AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s box size is wrong!", __func__, NULL, NULL, NULL, NULL);
        Rval = FORMAT_ERR_INVALID_FORMAT;
    }
    return Rval;
}

static UINT32 IsoDmx_ParseAudioSOWT(SVC_STREAM_HDLR_s *Stream, ISO_STSD_AUDIO_BOX_s *Stsd, UINT32 BoxSize)
{
    UINT32 Rval;
    if ((BoxSize == 68U) || (BoxSize == 106U) || (BoxSize == 52U)) {
        UINT32 Tmp32 = 0U;
        UINT16 Tmp16;
        //channel
        Rval = SvcFormat_GetBe16(Stream, &Stsd->AudioEntry.Channels);
        if (Rval == FORMAT_OK) {
            // sample size
            Rval = SvcFormat_GetBe16(Stream, &Stsd->AudioEntry.BitsPerSample);
        }
        if (Rval == FORMAT_OK) {
            // compression ID
            Rval = SvcFormat_GetBe16(Stream, &Tmp16);
        }
        if (Rval == FORMAT_OK) {
            // packet size
            Rval = SvcFormat_GetBe16(Stream, &Tmp16);
        }
        if (Rval == FORMAT_OK) {
            // sample rate
            Rval = SvcFormat_GetBe32(Stream, &Tmp32);
        }
        if (Rval == FORMAT_OK) {
            Stsd->AudioEntry.SampleRate = Tmp32 >> 16U;
            // sample per packet
            Rval = SvcFormat_GetBe32(Stream, &Tmp32);
        }
        if (Rval == FORMAT_OK) {
            // Bytes per packet
            Rval = SvcFormat_GetBe32(Stream, &Tmp32);
        }
        if (Rval == FORMAT_OK) {
            // Bytes per sample
            Rval = SvcFormat_GetBe32(Stream, &Tmp32);
        }
        if (Rval == FORMAT_OK) {
            // sample per sample?
            Rval = SvcFormat_GetBe32(Stream, &Tmp32);
        }
#if 0
        if (Rval == FORMAT_OK) {
            if (Stsd->AudioEntry.Box.Type == TAG_IN24) {
                Rval = IsoDmx_ParseAudioIN24(Stream, Stsd);
            }
        } else {
            AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s SvcFormat_GetBe32() error!", __func__, NULL, NULL, NULL, NULL);
        }
#endif
        /* skip to box end at IsoDmx_ParseAudioStsd */
        if (Rval != FORMAT_OK) {
            AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s SvcFormat_Get() error!", __func__, NULL, NULL, NULL, NULL);
        }
    } else {
        AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s box size is wrong!", __func__, NULL, NULL, NULL, NULL);
        Rval = FORMAT_ERR_INVALID_FORMAT;
    }
    return Rval;
}

static UINT32 IsoDmx_ParseAudioPCM(SVC_STREAM_HDLR_s *Stream, ISO_STSD_AUDIO_BOX_s *Stsd, UINT32 BoxSize)
{
    UINT32 Rval = FORMAT_OK;
    if (Stsd->AudioEntry.Box.Type == TAG_LPCM) { /**LPCM*/
        Rval = IsoDmx_ParseAudioLPCM(Stream, Stsd, BoxSize);
        if (Rval != FORMAT_OK) {
            AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s IsoDmx_ParseAudioLPCM() error!", __func__, NULL, NULL, NULL, NULL);
        }
    } else { /**SOWT TWOS IN24*/
        Rval = IsoDmx_ParseAudioSOWT(Stream, Stsd, BoxSize);
    }
    return Rval;
}

static UINT32 IsoDmx_ParseAudioStsd(SVC_STREAM_HDLR_s *Stream, ISO_STSD_AUDIO_BOX_s *Stsd, UINT32 BoxSize, UINT64 ParsePos)
{
    UINT16 Tmp16;
    UINT32 Tmp32;
    UINT32 Rval = SvcFormat_GetBe32(Stream, &Tmp32);
    if (Rval == FORMAT_OK) {
        Rval = SvcFormat_GetBe32(Stream, &Stsd->EntryCount);
    } else {
        AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s SvcFormat_GetBe32() error!", __func__, NULL, NULL, NULL, NULL);
    }
    if (Rval == FORMAT_OK) {
        Rval = SvcFormat_GetBe32(Stream, &Stsd->AudioEntry.Box.Size);
    } else {
        AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s SvcFormat_GetBe32() error!", __func__, NULL, NULL, NULL, NULL);
    }
    if (Rval == FORMAT_OK) {
        Rval = SvcFormat_GetBe32(Stream, &Stsd->AudioEntry.Box.Type);
    } else {
        AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s SvcFormat_GetBe32() error!", __func__, NULL, NULL, NULL, NULL);
    }
    if (Rval == FORMAT_OK) {
        Rval = SvcFormat_GetBe32(Stream, &Tmp32);
    } else {
        AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s SvcFormat_GetBe32() error!", __func__, NULL, NULL, NULL, NULL);
    }
    if (Rval == FORMAT_OK) {
        Rval = SvcFormat_GetBe16(Stream, &Tmp16);
    } else {
        AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s SvcFormat_GetBe32() error!", __func__, NULL, NULL, NULL, NULL);
    }
    if (Rval == FORMAT_OK) {
        Rval = SvcFormat_GetBe16(Stream, &Tmp16);
    } else {
        AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s SvcFormat_GetBe16() error!", __func__, NULL, NULL, NULL, NULL);
    }
    if (Rval == FORMAT_OK) {
        Rval = SvcFormat_GetBe32(Stream, &Tmp32);
    } else {
        AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s SvcFormat_GetBe16() error!", __func__, NULL, NULL, NULL, NULL);
    }
    if (Rval == FORMAT_OK) {
        Rval = SvcFormat_GetBe32(Stream, &Tmp32);
    } else {
        AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s SvcFormat_GetBe32() error!", __func__, NULL, NULL, NULL, NULL);
    }
    if (Rval == FORMAT_OK) {
        switch (Stsd->AudioEntry.Box.Type) {
        case TAG_MP4A:
            Rval = IsoDmx_ParseAudioAAC(Stream, Stsd, BoxSize);
            if (Rval != FORMAT_OK) {
                AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s IsoDmx_ParseAudioAAC() error!", __func__, NULL, NULL, NULL, NULL);
            }
            break;
        case TAG_MS0011:
            Rval = IsoDmx_ParseAudioADPCM(Stream, Stsd, BoxSize);
            if (Rval != FORMAT_OK) {
                AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s IsoDmx_ParseAudioADPCM() error!", __func__, NULL, NULL, NULL, NULL);
            }
            break;
        case TAG_LPCM:
        case TAG_SOWT:
        case TAG_TWOS:
        case TAG_IN24:
            Rval = IsoDmx_ParseAudioPCM(Stream, Stsd, BoxSize);
            if (Rval != FORMAT_OK) {
                AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s IsoDmx_ParseAudioPCM() error!", __func__, NULL, NULL, NULL, NULL);
            }
            break;
        default:
            AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s Unsupported audio format!", __func__, NULL, NULL, NULL, NULL);
            Rval = FORMAT_ERR_INVALID_FORMAT;
            break;
        }
    } else {
        AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s SvcFormat_GetBe32() error!", __func__, NULL, NULL, NULL, NULL);
    }
    if (Rval == FORMAT_OK) {
        Rval = S2F(Stream->Func->Seek(Stream, ((INT64) ParsePos + (INT64) BoxSize), SVC_STREAM_SEEK_START));
        if (Rval != FORMAT_OK) {
            AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s Seek fail!", __func__, NULL, NULL, NULL, NULL);
        }
    }
    return Rval;
}

static UINT32 IsoDmx_ParseTextText(SVC_STREAM_HDLR_s *Stream, STSD_TEXT_ENTRY_s *TextEntry, UINT32 BoxSize)
{
    UINT32 Rval;
    if ((BoxSize == (65U + 16U)) || (BoxSize == 75U)) {
        UINT8 Tmp8;
        Rval = SvcFormat_GetByte(Stream, &Tmp8);
        if (Rval == FORMAT_OK) {
            Rval = SvcFormat_GetByte(Stream, &Tmp8);
        }
        if (Rval == FORMAT_OK) {
            Rval = SvcFormat_GetByte(Stream, &Tmp8);
        }
        if (Rval == FORMAT_OK) {
            Rval = SvcFormat_GetByte(Stream, &Tmp8);
        }
        if (Rval == FORMAT_OK) {
            Rval = SvcFormat_GetBe16(Stream, &TextEntry->DataRefIdx);
        }
        if (Rval == FORMAT_OK) {
            Rval = SvcFormat_GetBe32(Stream, &TextEntry->DisFlags);
        }
        if (Rval == FORMAT_OK) {
            Rval = SvcFormat_GetBe32(Stream, &TextEntry->TextJust);
        }
        if (Rval == FORMAT_OK) {
            Rval = IsoDmx_GetBufferBe16(Stream, TextEntry->BgColor, 3U);
        }
        if (Rval == FORMAT_OK) {
            Rval = SvcFormat_GetBe64(Stream, &TextEntry->TextBox);
        }
        if (Rval == FORMAT_OK) {
            Rval = IsoDmx_GetBuffer8(Stream, TextEntry->Reserv8, 8U);
        }
        if (Rval == FORMAT_OK) {
            Rval = SvcFormat_GetBe16(Stream, &TextEntry->FontNumber);
        }
        if (Rval == FORMAT_OK) {
            Rval = SvcFormat_GetBe16(Stream, &TextEntry->FontFace);
        }
        if (BoxSize > 75U) {
            if (Rval == FORMAT_OK) {
                Rval = IsoDmx_GetBuffer8(Stream, TextEntry->Reserv3, 3U);
            }
            if (Rval == FORMAT_OK) {
                Rval = IsoDmx_GetBufferBe16(Stream, TextEntry->FgColor, 3U);
            }
            if (Rval == FORMAT_OK) {
                UINT32 Count;
                Rval = S2F(Stream->Func->Read(Stream, ISO_STSD_MAX_TEXT_NAME_LEN, (UINT8 *)TextEntry->TextName, &Count));
                if (Rval == FORMAT_OK) {
                    if (Count != ISO_STSD_MAX_TEXT_NAME_LEN) {
                        Rval = FORMAT_ERR_NOT_ENOUGH_BYTES;
                    }
                } else {
                    AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s Read data from stream error!", __func__, NULL, NULL, NULL, NULL);
                }
            }
        }
    } else {
        AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s box size is wrong!", __func__, NULL, NULL, NULL, NULL);
        Rval = FORMAT_ERR_INVALID_FORMAT;
    }
    return Rval;
}

static UINT32 IsoDmx_ParseTextMP4S(SVC_STREAM_HDLR_s *Stream, UINT32 BoxSize)
{
    UINT32 Rval;
    if (BoxSize == (16U + 16U)) {
        UINT8 Tmp8;
        UINT16 Tmp16;
        Rval = SvcFormat_GetByte(Stream, &Tmp8);
        if (Rval == FORMAT_OK) {
            Rval = SvcFormat_GetByte(Stream, &Tmp8);
        }
        if (Rval == FORMAT_OK) {
            Rval = SvcFormat_GetByte(Stream, &Tmp8);
        }
        if (Rval == FORMAT_OK) {
            Rval = SvcFormat_GetByte(Stream, &Tmp8);
        }
        if (Rval == FORMAT_OK) {
            Rval = SvcFormat_GetBe16(Stream, &Tmp16);
        }
        if (Rval == FORMAT_OK) {
            Rval = SvcFormat_GetBe16(Stream, &Tmp16);
        }
    } else {
        AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s box size is wrong!", __func__, NULL, NULL, NULL, NULL);
        Rval = FORMAT_ERR_INVALID_FORMAT;
    }
    return Rval;
}

static UINT32 IsoDmx_ParseTextStsd(SVC_STREAM_HDLR_s *Stream, ISO_STSD_TEXT_BOX_s *Stsd, UINT32 BoxSize, UINT64 ParsePos)
{
    UINT8 Tmp8;
    UINT32 Rval = SvcFormat_GetByte(Stream, &Tmp8);
    if (Rval == FORMAT_OK) {
        Rval = SvcFormat_GetByte(Stream, &Tmp8);
    } else {
        AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s SvcFormat_GetByte() error!", __func__, NULL, NULL, NULL, NULL);
    }
    if (Rval == FORMAT_OK) {
        Rval = SvcFormat_GetByte(Stream, &Tmp8);
    } else {
        AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s SvcFormat_GetByte() error!", __func__, NULL, NULL, NULL, NULL);
    }
    if (Rval == FORMAT_OK) {
        Rval = SvcFormat_GetByte(Stream, &Tmp8);
    } else {
        AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s SvcFormat_GetByte() error!", __func__, NULL, NULL, NULL, NULL);
    }
    if (Rval == FORMAT_OK) {
        Rval = SvcFormat_GetBe32(Stream, &Stsd->EntryCount);
    } else {
        AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s SvcFormat_GetByte() error!", __func__, NULL, NULL, NULL, NULL);
    }
    if (Rval == FORMAT_OK) {
        Rval = SvcFormat_GetBe32(Stream, &Stsd->TextEntry.Box.Size);
    } else {
        AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s SvcFormat_GetBe32() error!", __func__, NULL, NULL, NULL, NULL);
    }
    if (Rval == FORMAT_OK) {
        Rval = SvcFormat_GetBe32(Stream, &Stsd->TextEntry.Box.Type);
    } else {
        AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s SvcFormat_GetBe32() error!", __func__, NULL, NULL, NULL, NULL);
    }
    if (Rval == FORMAT_OK) {
        if (Stsd->TextEntry.Box.Type == TAG_TEXT) {
            Rval = IsoDmx_ParseTextText(Stream, &Stsd->TextEntry, BoxSize);
            if (Rval != FORMAT_OK) {
                AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s IsoDmx_ParseTextText error!", __func__, NULL, NULL, NULL, NULL);
            }
        } else if (Stsd->TextEntry.Box.Type == TAG_MP4S) {
            Rval = IsoDmx_ParseTextMP4S(Stream, BoxSize);
            if (Rval != FORMAT_OK) {
                AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s IsoDmx_ParseTextMP4S error!", __func__, NULL, NULL, NULL, NULL);
            }
        } else {
            AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s Not Support", __func__, NULL, NULL, NULL, NULL);
            Rval = FORMAT_ERR_INVALID_FORMAT;
        }
    } else {
        AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s SvcFormat_GetBe32() error!", __func__, NULL, NULL, NULL, NULL);
    }
    if (Rval == FORMAT_OK) {
        Rval = S2F(Stream->Func->Seek(Stream, ((INT64) ParsePos + (INT64) BoxSize), SVC_STREAM_SEEK_START));
        if (Rval != FORMAT_OK) {
            AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s Seek fail!", __func__, NULL, NULL, NULL, NULL);
        }
    }
    return Rval;
}

static UINT32 IsoDmx_ParseStts(SVC_STREAM_HDLR_s *Stream, UINT32 BoxSize, UINT32 *EntryCount, UINT32 *SampleDelta, UINT64 *SttsTable)
{
    UINT8 Tmp8;
    UINT32 Tmp32;
    UINT64 Tmp64 = 0ULL;
    UINT32 Rval = SvcFormat_GetByte(Stream, &Tmp8);
    if (Rval == FORMAT_OK) {
        Rval = SvcFormat_GetByte(Stream, &Tmp8);
    } else {
        AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s SvcFormat_GetByte() error!", __func__, NULL, NULL, NULL, NULL);
    }
    if (Rval == FORMAT_OK) {
        Rval = SvcFormat_GetByte(Stream, &Tmp8);
    } else {
        AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s SvcFormat_GetByte() error!", __func__, NULL, NULL, NULL, NULL);
    }
    if (Rval == FORMAT_OK) {
        Rval = SvcFormat_GetByte(Stream, &Tmp8);
    } else {
        AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s SvcFormat_GetByte() error!", __func__, NULL, NULL, NULL, NULL);
    }
    if (Rval == FORMAT_OK) {
        Rval = SvcFormat_GetBe32(Stream, EntryCount);
    } else {
        AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s SvcFormat_GetByte() error!", __func__, NULL, NULL, NULL, NULL);
    }
    if (Rval == FORMAT_OK) {
        Rval = S2F(Stream->Func->GetPos(Stream, &Tmp64));
    } else {
        AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s SvcFormat_GetBe32() error!", __func__, NULL, NULL, NULL, NULL);
    }
    if (Rval == FORMAT_OK) {
        *SttsTable = Tmp64;
        if (*EntryCount > 0U) {
            Rval = SvcFormat_GetBe32(Stream, &Tmp32);
            if (Rval == FORMAT_OK) {
                Rval = SvcFormat_GetBe32(Stream, SampleDelta);
            } else {
                AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s SvcFormat_GetBe32() error!", __func__, NULL, NULL, NULL, NULL);
            }
            if (Rval == FORMAT_OK) {
                if (*EntryCount > 1U) {
                    Rval = S2F(Stream->Func->Seek(Stream, ((INT64) *SttsTable + (INT64) BoxSize - 16), SVC_STREAM_SEEK_START));
                    if (Rval != FORMAT_OK) {
                        AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s Seek fail!", __func__, NULL, NULL, NULL, NULL);
                    }
                }
            } else {
                AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s SvcFormat_GetBe32() error!", __func__, NULL, NULL, NULL, NULL);
            }
        }
    } else {
        AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s Get file position error!", __func__, NULL, NULL, NULL, NULL);
    }
    return Rval;
}

static UINT32 IsoDmx_ParseStsc(SVC_STREAM_HDLR_s *Stream, ISO_STSC_BOX_s *Stsc, UINT64 ParseEndPos)
{
    UINT8 Tmp8;
    UINT32 Rval = SvcFormat_GetByte(Stream, &Tmp8);
    if (Rval == FORMAT_OK) {
        Rval = SvcFormat_GetByte(Stream, &Tmp8);
    } else {
        AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s SvcFormat_GetByte() error!", __func__, NULL, NULL, NULL, NULL);
    }
    if (Rval == FORMAT_OK) {
        Rval = SvcFormat_GetByte(Stream, &Tmp8);
    } else {
        AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s SvcFormat_GetByte() error!", __func__, NULL, NULL, NULL, NULL);
    }
    if (Rval == FORMAT_OK) {
        Rval = SvcFormat_GetByte(Stream, &Tmp8);
    } else {
        AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s SvcFormat_GetByte() error!", __func__, NULL, NULL, NULL, NULL);
    }
    if (Rval == FORMAT_OK) {
        Rval = SvcFormat_GetBe32(Stream, &Stsc->EntryCount);
    } else {
        AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s SvcFormat_GetByte() error!", __func__, NULL, NULL, NULL, NULL);
    }
    if (Rval == FORMAT_OK) {
        if (Stsc->EntryCount > 0U) {
            Rval = SvcFormat_GetBe32(Stream, &Stsc->StscTable.FirstChunk);
            if (Rval == FORMAT_OK) {
                Rval = SvcFormat_GetBe32(Stream, &Stsc->StscTable.SamPerChunk);
            } else {
                AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s SvcFormat_GetBe32() error!", __func__, NULL, NULL, NULL, NULL);
            }
            if (Rval == FORMAT_OK) {
                Rval = SvcFormat_GetBe32(Stream, &Stsc->StscTable.SamDescIdx);
                if (Rval != FORMAT_OK) {
                    AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s SvcFormat_GetBe32() error!", __func__, NULL, NULL, NULL, NULL);
                }
            } else {
                AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s SvcFormat_GetBe32() error!", __func__, NULL, NULL, NULL, NULL);
            }
        }
        if (Rval == FORMAT_OK) {
            // if (Stsc->EntryCount > 1U) {
            // svc's stsc box is larger with EntryCount = 0
            Rval = S2F(Stream->Func->Seek(Stream, (INT64)ParseEndPos, SVC_STREAM_SEEK_START));
            if (Rval != FORMAT_OK) {
                AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s Seek fail!", __func__, NULL, NULL, NULL, NULL);
            }
            //}
        }
    } else {
        AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s SvcFormat_GetBe32() error!", __func__, NULL, NULL, NULL, NULL);
    }
    return Rval;
}

static UINT32 IsoDmx_ParseCtts(SVC_STREAM_HDLR_s *Stream, UINT32 BoxSize, ISO_CTTS_BOX_s *Ctts, UINT64 *CttsTable)
{
    UINT8 Tmp8;
    UINT64 Tmp64 = 0ULL;
    UINT32 Rval = SvcFormat_GetByte(Stream, &Tmp8);
    if (Rval == FORMAT_OK) {
        Rval = SvcFormat_GetByte(Stream, &Tmp8);
    } else {
        AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s SvcFormat_GetByte() error!", __func__, NULL, NULL, NULL, NULL);
    }
    if (Rval == FORMAT_OK) {
        Rval = SvcFormat_GetByte(Stream, &Tmp8);
    } else {
        AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s SvcFormat_GetByte() error!", __func__, NULL, NULL, NULL, NULL);
    }
    if (Rval == FORMAT_OK) {
        Rval = SvcFormat_GetByte(Stream, &Tmp8);
    } else {
        AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s SvcFormat_GetByte() error!", __func__, NULL, NULL, NULL, NULL);
    }
    if (Rval == FORMAT_OK) {
        Rval = SvcFormat_GetBe32(Stream, &Ctts->EntryCount);
    } else {
        AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s SvcFormat_GetByte() error!", __func__, NULL, NULL, NULL, NULL);
    }
    if (Rval == FORMAT_OK) {
        Rval = S2F(Stream->Func->GetPos(Stream, &Tmp64));
    } else {
        AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s SvcFormat_GetBe32() error!", __func__, NULL, NULL, NULL, NULL);
    }
    if (Rval == FORMAT_OK) {
        *CttsTable = Tmp64;
        Rval = S2F(Stream->Func->Seek(Stream, ((INT64) *CttsTable + (INT64) BoxSize - 16), SVC_STREAM_SEEK_START));
        if (Rval != FORMAT_OK) {
            AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s Seek fail!", __func__, NULL, NULL, NULL, NULL);
        }
    } else {
        AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s Get file position error!", __func__, NULL, NULL, NULL, NULL);
    }
    return Rval;
}

static UINT32 IsoDmx_ParseStsz(SVC_STREAM_HDLR_s *Stream, UINT32 BoxSize, ISO_STSZ_BOX_s *Stsz, UINT64 *StszTable)
{
    UINT8 Tmp8;
    UINT64 Tmp64 = 0ULL;
    UINT32 Rval = SvcFormat_GetByte(Stream, &Tmp8);
    if (Rval == FORMAT_OK) {
        Rval = SvcFormat_GetByte(Stream, &Tmp8);
    } else {
        AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s SvcFormat_GetByte() error!", __func__, NULL, NULL, NULL, NULL);
    }
    if (Rval == FORMAT_OK) {
        Rval = SvcFormat_GetByte(Stream, &Tmp8);
    } else {
        AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s SvcFormat_GetByte() error!", __func__, NULL, NULL, NULL, NULL);
    }
    if (Rval == FORMAT_OK) {
        Rval = SvcFormat_GetByte(Stream, &Tmp8);
    } else {
        AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s SvcFormat_GetByte() error!", __func__, NULL, NULL, NULL, NULL);
    }
    if (Rval == FORMAT_OK) {
        Rval = SvcFormat_GetBe32(Stream, &Stsz->SampleSize);
    } else {
        AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s SvcFormat_GetByte() error!", __func__, NULL, NULL, NULL, NULL);
    }
    if (Rval == FORMAT_OK) {
        Rval = SvcFormat_GetBe32(Stream, &Stsz->SampleCount);
    } else {
        AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s SvcFormat_GetBe32() error!", __func__, NULL, NULL, NULL, NULL);
    }
    if (Rval == FORMAT_OK) {
        Rval = S2F(Stream->Func->GetPos(Stream, &Tmp64));
    } else {
        AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s SvcFormat_GetBe32() error!", __func__, NULL, NULL, NULL, NULL);
    }
    if (Rval == FORMAT_OK) {
        *StszTable = Tmp64;
        if (Stsz->SampleCount > 0U) {
            Rval = S2F(Stream->Func->Seek(Stream, ((INT64) *StszTable + (INT64) BoxSize - 20), SVC_STREAM_SEEK_START));
            if (Rval != FORMAT_OK) {
                AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s Seek fail!", __func__, NULL, NULL, NULL, NULL);
            }
        }
    } else {
        AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s Get file position error!", __func__, NULL, NULL, NULL, NULL);
    }
    return Rval;
}

static UINT32 IsoDmx_ParseStco(SVC_STREAM_HDLR_s *Stream, UINT32 BoxSize, ISO_STCO_BOX_s *Stco, UINT64 *StcoTable)
{
    UINT8 Tmp8;
    UINT64 Tmp64 = 0ULL;
    UINT32 Rval = SvcFormat_GetByte(Stream, &Tmp8);
    if (Rval == FORMAT_OK) {
        Rval = SvcFormat_GetByte(Stream, &Tmp8);
    } else {
        AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s SvcFormat_GetByte() error!", __func__, NULL, NULL, NULL, NULL);
    }
    if (Rval == FORMAT_OK) {
        Rval = SvcFormat_GetByte(Stream, &Tmp8);
    } else {
        AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s SvcFormat_GetByte() error!", __func__, NULL, NULL, NULL, NULL);
    }
    if (Rval == FORMAT_OK) {
        Rval = SvcFormat_GetByte(Stream, &Tmp8);
    } else {
        AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s SvcFormat_GetByte() error!", __func__, NULL, NULL, NULL, NULL);
    }
    if (Rval == FORMAT_OK) {
        Rval = SvcFormat_GetBe32(Stream, &Stco->EntryCount);
    } else {
        AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s SvcFormat_GetByte() error!", __func__, NULL, NULL, NULL, NULL);
    }
    if (Rval == FORMAT_OK) {
        Rval = S2F(Stream->Func->GetPos(Stream, &Tmp64));
    } else {
        AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s SvcFormat_GetBe32() error!", __func__, NULL, NULL, NULL, NULL);
    }
    if (Rval == FORMAT_OK) {
        *StcoTable = Tmp64;
        if (Stco->EntryCount > 0U) {
            Rval = S2F(Stream->Func->Seek(Stream, ((INT64) *StcoTable + (INT64) BoxSize - 16), SVC_STREAM_SEEK_START));
            if (Rval != FORMAT_OK) {
                AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s Seek fail!", __func__, NULL, NULL, NULL, NULL);
            }
        }
    } else {
        AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s Get file position error!", __func__, NULL, NULL, NULL, NULL);
    }
    return Rval;
}

static UINT32 IsoDmx_ParseStss(SVC_STREAM_HDLR_s *Stream, UINT32 BoxSize, ISO_STSS_BOX_s *Stss, UINT64 *SyncTable)
{
    UINT8 Tmp8;
    UINT64 Tmp64 = 0ULL;
    UINT32 Rval = SvcFormat_GetByte(Stream, &Tmp8);
    if (Rval == FORMAT_OK) {
        Rval = SvcFormat_GetByte(Stream, &Tmp8);
    } else {
        AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s SvcFormat_GetByte() error!", __func__, NULL, NULL, NULL, NULL);
    }
    if (Rval == FORMAT_OK) {
        Rval = SvcFormat_GetByte(Stream, &Tmp8);
    } else {
        AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s SvcFormat_GetByte() error!", __func__, NULL, NULL, NULL, NULL);
    }
    if (Rval == FORMAT_OK) {
        Rval = SvcFormat_GetByte(Stream, &Tmp8);
    } else {
        AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s SvcFormat_GetByte() error!", __func__, NULL, NULL, NULL, NULL);
    }
    if (Rval == FORMAT_OK) {
        Rval = SvcFormat_GetBe32(Stream, &Stss->EntryCount);
    } else {
        AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s SvcFormat_GetByte() error!", __func__, NULL, NULL, NULL, NULL);
    }
    if (Rval == FORMAT_OK) {
        Rval = S2F(Stream->Func->GetPos(Stream, &Tmp64));
    } else {
        AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s SvcFormat_GetBe32() error!", __func__, NULL, NULL, NULL, NULL);
    }
    if (Rval == FORMAT_OK) {
        *SyncTable = Tmp64;
        Rval = S2F(Stream->Func->Seek(Stream, ((INT64) *SyncTable + (INT64) BoxSize - 16), SVC_STREAM_SEEK_START));
        if (Rval != FORMAT_OK) {
            AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s Seek fail!", __func__, NULL, NULL, NULL, NULL);
        }
    } else {
        AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s Get file position error!", __func__, NULL, NULL, NULL, NULL);
    }
    return Rval;
}

static UINT32 IsoDmx_SetVideoStsdInfo(SVC_MEDIA_TRACK_GENERAL_INFO_s *Track, SVC_ISO_VIDEO_TRACK_INFO_s *IsoTrack, const ISO_STSD_VIDEO_BOX_s *VideoStsd)
{
    UINT32 Rval;
    if (VideoStsd->VideoEntry.Box.Type == TAG_AVC1) {
        Track->MediaId = SVC_FORMAT_MID_AVC;
        IsoTrack->Avc.NALULength = VideoStsd->VideoEntry.DecCfg.Avcc.NALULength;
        IsoTrack->Avc.SPSLen = VideoStsd->VideoEntry.DecCfg.Avcc.SPSLength;
        Rval = W2F(AmbaWrap_memcpy(IsoTrack->Avc.SPS, VideoStsd->VideoEntry.DecCfg.Avcc.SPSCtx, VideoStsd->VideoEntry.DecCfg.Avcc.SPSLength));
        if (Rval == FORMAT_OK) {
            IsoTrack->Avc.PPSLen = VideoStsd->VideoEntry.DecCfg.Avcc.PPSLength;
            Rval = W2F(AmbaWrap_memcpy(IsoTrack->Avc.PPS, VideoStsd->VideoEntry.DecCfg.Avcc.PPSCtx, VideoStsd->VideoEntry.DecCfg.Avcc.PPSLength));
        }
    } else if(VideoStsd->VideoEntry.Box.Type == TAG_HVC1) {
        Track->MediaId = SVC_FORMAT_MID_HVC;
        IsoTrack->Hvc.NALULength = VideoStsd->VideoEntry.DecCfg.Hvcc.NALULength;
        IsoTrack->Hvc.PicSizeInCtbsY = VideoStsd->VideoEntry.DecCfg.Hvcc.PicSizeInCtbsY;
        IsoTrack->Hvc.VPSLen = VideoStsd->VideoEntry.DecCfg.Hvcc.VPSLength;
        Rval = W2F(AmbaWrap_memcpy(IsoTrack->Hvc.VPS, VideoStsd->VideoEntry.DecCfg.Hvcc.VPSCtx, VideoStsd->VideoEntry.DecCfg.Hvcc.VPSLength));
        if (Rval == FORMAT_OK) {
            IsoTrack->Hvc.SPSLen = VideoStsd->VideoEntry.DecCfg.Hvcc.SPSLength;
            Rval = W2F(AmbaWrap_memcpy(IsoTrack->Hvc.SPS, VideoStsd->VideoEntry.DecCfg.Hvcc.SPSCtx, VideoStsd->VideoEntry.DecCfg.Hvcc.SPSLength));
        }
        if (Rval == FORMAT_OK) {
            IsoTrack->Hvc.PPSLen = VideoStsd->VideoEntry.DecCfg.Hvcc.PPSLength;
            Rval = W2F(AmbaWrap_memcpy(IsoTrack->Hvc.PPS, VideoStsd->VideoEntry.DecCfg.Hvcc.PPSCtx, VideoStsd->VideoEntry.DecCfg.Hvcc.PPSLength));
        }
    } else {
        AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s Unsupported media type!", __func__, NULL, NULL, NULL, NULL);
        Rval = FORMAT_ERR_INVALID_FORMAT;
    }
    return Rval;
}

static UINT32 IsoDmx_ParseVideoStblInfo(SVC_VIDEO_TRACK_INFO_s *Video, SVC_STREAM_HDLR_s *Stream, SVC_ISO_VIDEO_TRACK_INFO_s *IsoTrack, UINT32 BoxTag, UINT32 BoxSize, UINT64 ParsePos)
{
    UINT32 Rval;
    SVC_MEDIA_TRACK_GENERAL_INFO_s *Track = &Video->Info;
    if (BoxTag == TAG_STSD) {
        ISO_STSD_VIDEO_BOX_s VideoStsd;
        Rval = IsoDmx_ParseVideoStsd(Video, Stream, &VideoStsd, (ParsePos + SVC_ISO_BOX_SIZE_TAG_SIZE), (ParsePos + BoxSize));
        if (Rval == FORMAT_OK) {
            Rval = IsoDmx_SetVideoStsdInfo(Track, IsoTrack, &VideoStsd);
            if (Rval != FORMAT_OK) {
                AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s IsoDmx_SetVideoStsdInfo() error!", __func__, NULL, NULL, NULL, NULL);
            }
        } else {
            AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s IsoDmx_ParseVideoStsd() error!", __func__, NULL, NULL, NULL, NULL);
        }
    } else if (BoxTag == TAG_STTS) {
        Rval = IsoDmx_ParseStts(Stream, BoxSize, &IsoTrack->SttsCount, &Track->TimePerFrame, &IsoTrack->SttsOffset);
        if (Rval != FORMAT_OK) {
            AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s IsoDmx_ParseVideoStts() error!", __func__, NULL, NULL, NULL, NULL);
        }
    } else if (BoxTag == TAG_STSC) {
        ISO_STSC_BOX_s Stsc;
        Rval = IsoDmx_ParseStsc(Stream, &Stsc, (ParsePos + BoxSize));
        if (Rval != FORMAT_OK) {
            AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s IsoDmx_ParseStsc() error!", __func__, NULL, NULL, NULL, NULL);
        }
    } else if (BoxTag == TAG_CTTS) {
        ISO_CTTS_BOX_s Ctts;
        Rval = IsoDmx_ParseCtts(Stream, BoxSize, &Ctts, &IsoTrack->CttsOffset);
        if (Rval != FORMAT_OK) {
            AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s IsoDmx_ParseCtts() error!", __func__, NULL, NULL, NULL, NULL);
        }
    } else if (BoxTag == TAG_STSZ) {
        ISO_STSZ_BOX_s Stsz;
        Rval = IsoDmx_ParseStsz(Stream, BoxSize, &Stsz, &IsoTrack->StszOffset);
        if (Rval != FORMAT_OK) {
            AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s IsoDmx_ParseStsz() error!", __func__, NULL, NULL, NULL, NULL);
        }
    } else if (BoxTag == TAG_STCO) {
        ISO_STCO_BOX_s Stco = {0};
        Rval = IsoDmx_ParseStco(Stream, BoxSize, &Stco, &IsoTrack->StcoOffset);
        if (Rval == FORMAT_OK) {
            Track->FrameCount = Stco.EntryCount;
        } else {
            AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s IsoDmx_ParseStco() error!", __func__, NULL, NULL, NULL, NULL);
        }
    } else if (BoxTag == TAG_CO64) {
        AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s error! CO64 not supported.", __func__, NULL, NULL, NULL, NULL);
        Rval = FORMAT_ERR_INVALID_FORMAT;
    } else if (BoxTag == TAG_STSS) {
        ISO_STSS_BOX_s Stss;
        Rval = IsoDmx_ParseStss(Stream, BoxSize, &Stss, &IsoTrack->StssOffset);
        if (Rval == FORMAT_OK) {
            IsoTrack->KeyFrameCount = Stss.EntryCount;
        } else {
            AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s IsoDmx_ParseStss() error!", __func__, NULL, NULL, NULL, NULL);
        }
    } else {
        Rval = S2F(Stream->Func->Seek(Stream, ((INT64) ParsePos + (INT64) BoxSize), SVC_STREAM_SEEK_START));
        if (Rval != FORMAT_OK) {
            AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s Seek fail!", __func__, NULL, NULL, NULL, NULL);
        }
    }
    return Rval;
}

static UINT32 IsoDmx_ParseVideoStbl(SVC_VIDEO_TRACK_INFO_s *Video, SVC_STREAM_HDLR_s *Stream, SVC_ISO_VIDEO_TRACK_INFO_s *IsoTrack, UINT64 ParseStartPos, UINT64 ParseEndPos)
{
    UINT64 ParsePos = ParseStartPos;
    UINT64 RemainSize = ParseEndPos - ParseStartPos;
    UINT32 BoxSize;
    UINT32 BoxTag;
    UINT32 Rval = FORMAT_OK;
    do {
        Rval = IsoDmx_GetBoxHeader(Stream, &BoxSize, &BoxTag);
        if (Rval == FORMAT_OK) {
            if (RemainSize < BoxSize) {
                AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s IsoDmx_ParseStbl() fail!", __func__, NULL, NULL, NULL, NULL);
                Rval = FORMAT_ERR_INVALID_FORMAT;
            } else {
                Rval= IsoDmx_ParseVideoStblInfo(Video, Stream, IsoTrack, BoxTag, BoxSize, ParsePos);
                if (Rval == FORMAT_OK) {
                    ParsePos += BoxSize;
                    RemainSize -= BoxSize;
                    if (RemainSize == 0U) {
                        break;
                    }
                }
            }
        } else {
            AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s Get box header error!", __func__, NULL, NULL, NULL, NULL);
        }
    } while (Rval == FORMAT_OK);
    return Rval;
}

static UINT32 IsoDmx_GetAudioMediaId(UINT32 BoxType, UINT8 *MediaId)
{
    UINT32 Rval = FORMAT_OK;
    if (BoxType == TAG_MP4A) {
        *MediaId = (UINT8) SVC_FORMAT_MID_AAC;
    } else if (BoxType == TAG_MS0011) {
        *MediaId = (UINT8) SVC_FORMAT_MID_ADPCM;
    } else if ((BoxType == TAG_SOWT) || (BoxType == TAG_IN24)) {
        *MediaId = (UINT8) SVC_FORMAT_MID_PCM;
    } else if (BoxType == TAG_TWOS) {
        *MediaId = (UINT8) SVC_FORMAT_MID_LPCM;
    } else {
        AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s Unsupported media type!", __func__, NULL, NULL, NULL, NULL);
        Rval = FORMAT_ERR_INVALID_FORMAT;
    }
    return Rval;
}

static UINT32 IsoDmx_ParseAudioStblInfo(SVC_AUDIO_TRACK_INFO_s *Audio, SVC_STREAM_HDLR_s *Stream, SVC_ISO_AUDIO_TRACK_INFO_s *IsoTrack, UINT32 BoxTag, UINT32 BoxSize, UINT64 ParsePos)
{
    UINT32 Rval;
    SVC_MEDIA_TRACK_GENERAL_INFO_s *Track = &Audio->Info;
    if (BoxTag == TAG_STSD) {
        ISO_STSD_AUDIO_BOX_s AudioStsd;
        Rval = IsoDmx_ParseAudioStsd(Stream, &AudioStsd, BoxSize, ParsePos);
        if (Rval != FORMAT_OK) {
            AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s IsoDmx_ParseAudioStsd() error!", __func__, NULL, NULL, NULL, NULL);
        } else {
            Audio->SampleRate = AudioStsd.AudioEntry.SampleRate;
            Audio->Channels = (UINT8) AudioStsd.AudioEntry.Channels;
            Audio->BitsPerSample = (UINT8) AudioStsd.AudioEntry.BitsPerSample;
            Rval = IsoDmx_GetAudioMediaId(AudioStsd.AudioEntry.Box.Type, &Track->MediaId);
        }
    } else if (BoxTag == TAG_STTS) {
        UINT32 TimePerFrame;
        Rval = IsoDmx_ParseStts(Stream, BoxSize, &IsoTrack->SttsCount, &TimePerFrame, &IsoTrack->SttsOffset);
        if (Rval == FORMAT_OK) {
            if (Track->MediaId == SVC_FORMAT_MID_AAC) {
                Track->TimePerFrame = TimePerFrame;
            }
        } else {
            AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s IsoDmx_ParseVideoStts() error!", __func__, NULL, NULL, NULL, NULL);
        }
    } else if (BoxTag == TAG_STSC) {
        ISO_STSC_BOX_s Stsc;
        Rval = IsoDmx_ParseStsc(Stream, &Stsc, (ParsePos + BoxSize));
        if (Rval == FORMAT_OK) {
            if ((Track->MediaId == SVC_FORMAT_MID_ADPCM) || (Track->MediaId == SVC_FORMAT_MID_PCM) || (Track->MediaId == SVC_FORMAT_MID_LPCM)) {
                Track->TimePerFrame = Stsc.StscTable.SamPerChunk;
            }
        } else {
            AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s IsoDmx_ParseStsc() error!", __func__, NULL, NULL, NULL, NULL);
        }
    } else if (BoxTag == TAG_STSZ) {
        ISO_STSZ_BOX_s Stsz;
        Rval = IsoDmx_ParseStsz(Stream, BoxSize, &Stsz, &IsoTrack->StszOffset);
        if (Rval != FORMAT_OK) {
            AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s IsoDmx_ParseStsz() error!", __func__, NULL, NULL, NULL, NULL);
        }
    } else if (BoxTag == TAG_STCO) {
        ISO_STCO_BOX_s Stco = {0};
        Rval = IsoDmx_ParseStco(Stream, BoxSize, &Stco, &IsoTrack->StcoOffset);
        if (Rval == FORMAT_OK) {
            Track->FrameCount = Stco.EntryCount;
        } else {
            AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s IsoDmx_ParseStco() error!", __func__, NULL, NULL, NULL, NULL);
        }
    } else if (BoxTag == TAG_CO64) {
        AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s error! CO64 not supported.", __func__, NULL, NULL, NULL, NULL);
        Rval = FORMAT_ERR_INVALID_FORMAT;
    } else {
        Rval = S2F(Stream->Func->Seek(Stream, ((INT64) ParsePos + (INT64) BoxSize), SVC_STREAM_SEEK_START));
        if (Rval != FORMAT_OK) {
            AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s Seek fail!", __func__, NULL, NULL, NULL, NULL);
        }
    }
    return Rval;
}

static UINT32 IsoDmx_ParseAudioStbl(SVC_AUDIO_TRACK_INFO_s *Audio, SVC_STREAM_HDLR_s *Stream, SVC_ISO_AUDIO_TRACK_INFO_s *IsoTrack, UINT64 ParseStartPos, UINT64 ParseEndPos)
{
    UINT32 Rval = FORMAT_OK;
    UINT64 ParsePos = ParseStartPos;
    UINT64 RemainSize = ParseEndPos - ParseStartPos;
    UINT32 BoxSize;
    UINT32 BoxTag;
    do {
        Rval = IsoDmx_GetBoxHeader(Stream, &BoxSize, &BoxTag);
        if (Rval == FORMAT_OK) {
            if (RemainSize < BoxSize) {
                AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s IsoDmx_ParseStbl() fail!", __func__, NULL, NULL, NULL, NULL);
                Rval = FORMAT_ERR_INVALID_FORMAT;
            } else {
                Rval = IsoDmx_ParseAudioStblInfo(Audio, Stream, IsoTrack, BoxTag, BoxSize, ParsePos);
                if (Rval == FORMAT_OK) {
                    ParsePos += BoxSize;
                    RemainSize -= BoxSize;
                    if (RemainSize == 0U) {
                        break;
                    }
                }
            }
        } else {
            AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s Get box header error!", __func__, NULL, NULL, NULL, NULL);
        }
    } while (Rval == FORMAT_OK);
    return Rval;
}

static UINT32 IsoDmx_ParseTextStbl(SVC_TEXT_TRACK_INFO_s *Text, SVC_STREAM_HDLR_s *Stream, SVC_ISO_TEXT_TRACK_INFO_s *IsoTrack, UINT64 ParseStartPos, UINT64 ParseEndPos)
{
    UINT32 Rval = FORMAT_OK;
    UINT64 ParsePos = ParseStartPos;
    UINT64 RemainSize = ParseEndPos - ParseStartPos;
    UINT32 BoxSize;
    UINT32 BoxTag;
    SVC_MEDIA_TRACK_GENERAL_INFO_s *Track = &Text->Info;
    do {
        Rval = IsoDmx_GetBoxHeader(Stream, &BoxSize, &BoxTag);
        if (Rval == FORMAT_OK) {
            if (RemainSize < BoxSize ){
                AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s IsoDmx_ParseStbl() fail!", __func__, NULL, NULL, NULL, NULL);
                Rval = FORMAT_ERR_INVALID_FORMAT;
            } else {
                if (BoxTag == TAG_STSD) {
                    ISO_STSD_TEXT_BOX_s TextStsd;
                    Rval = IsoDmx_ParseTextStsd(Stream, &TextStsd, BoxSize, ParsePos);
                    if (Rval != FORMAT_OK) {
                        AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s IsoDmx_ParseTextStsd() error!", __func__, NULL, NULL, NULL, NULL);
                    } else {
                        if (TextStsd.TextEntry.Box.Type == TAG_TEXT) {
                            Track->MediaId = (UINT8) SVC_FORMAT_MID_TEXT;
                        } else if (TextStsd.TextEntry.Box.Type == TAG_MP4S) {
                            Track->MediaId = (UINT8) SVC_FORMAT_MID_MP4S;
                        } else {
                            AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s Unsupported media type!", __func__, NULL, NULL, NULL, NULL);
                            Rval = FORMAT_ERR_INVALID_FORMAT;
                        }
                    }
                } else if (BoxTag == TAG_STTS) {
                    Rval = IsoDmx_ParseStts(Stream, BoxSize, &IsoTrack->SttsCount, &Track->TimePerFrame, &IsoTrack->SttsOffset);
                    if (Rval != FORMAT_OK) {
                        AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s IsoDmx_ParseVideoStts() error!", __func__, NULL, NULL, NULL, NULL);
                    }
                } else if (BoxTag == TAG_STSC) {
                    ISO_STSC_BOX_s Stsc;
                    Rval = IsoDmx_ParseStsc(Stream, &Stsc, (ParsePos + BoxSize));
                    if (Rval != FORMAT_OK) {
                        AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s IsoDmx_ParseStsc() error!", __func__, NULL, NULL, NULL, NULL);
                    }
                } else if (BoxTag == TAG_STSZ) {
                    ISO_STSZ_BOX_s Stsz;
                    Rval = IsoDmx_ParseStsz(Stream, BoxSize, &Stsz, &IsoTrack->StszOffset);
                    if (Rval != FORMAT_OK) {
                        AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s IsoDmx_ParseStsz() error!", __func__, NULL, NULL, NULL, NULL);
                    }
                } else if (BoxTag == TAG_STCO) {
                    ISO_STCO_BOX_s Stco = {0};
                    Rval = IsoDmx_ParseStco(Stream, BoxSize, &Stco, &IsoTrack->StcoOffset);
                    if (Rval == FORMAT_OK) {
                        Track->FrameCount = Stco.EntryCount;
                    } else {
                        AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s IsoDmx_ParseStco() error!", __func__, NULL, NULL, NULL, NULL);
                    }
                } else if (BoxTag == TAG_CO64) {
                    AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s error! CO64 not supported.", __func__, NULL, NULL, NULL, NULL);
                    Rval = FORMAT_ERR_INVALID_FORMAT;
                } else {
                    Rval = S2F(Stream->Func->Seek(Stream, ((INT64) ParsePos + (INT64) BoxSize), SVC_STREAM_SEEK_START));
                    if (Rval != FORMAT_OK) {
                        AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s Seek fail!", __func__, NULL, NULL, NULL, NULL);
                    }
                }
                if (Rval == FORMAT_OK) {
                    ParsePos += BoxSize;
                    RemainSize -= BoxSize;
                    if (RemainSize == 0U) {
                        break;
                    }
                }
            }
        } else {
            AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s Get box header error!", __func__, NULL, NULL, NULL, NULL);
        }
    } while (Rval == FORMAT_OK);
    return Rval;
}

static UINT32 IsoDmx_ParseVideoMinf(SVC_VIDEO_TRACK_INFO_s *Track, SVC_STREAM_HDLR_s *Stream, SVC_ISO_VIDEO_TRACK_INFO_s *IsoTrack, UINT64 ParseStartPos, UINT64 ParseEndPos)
{
    UINT32 Rval = FORMAT_OK;
    UINT64 ParsePos = ParseStartPos;
    UINT64 RemainSize = ParseEndPos - ParseStartPos;
    UINT32 BoxSize;
    UINT32 BoxTag;
    ISO_VMHD_BOX_s Vmhd;
    ISO_SMHD_BOX_s Smhd;
    ISO_DINF_BOX_s Dinf;
    do {
        /* Get Box Size, Tag */
        Rval = IsoDmx_GetBoxHeader(Stream, &BoxSize, &BoxTag);
        if (Rval == FORMAT_OK) {
            if (RemainSize < BoxSize) {
                AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s IsoDmx_ParseMinf() fail!", __func__, NULL, NULL, NULL, NULL);
                Rval = FORMAT_ERR_INVALID_FORMAT;
            } else {
                if (BoxTag == TAG_VMHD) {
                    Rval = IsoDmx_ParseVmhd(Stream, &Vmhd, BoxSize);
                    if (Rval != FORMAT_OK) {
                        AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s IsoDmx_ParseVmhd() error!", __func__, NULL, NULL, NULL, NULL);
                    }
                } else if (BoxTag == TAG_SMHD) {
                    Rval = IsoDmx_ParseSmhd(Stream, &Smhd, BoxSize);
                    if (Rval != FORMAT_OK) {
                        AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s IsoDmx_ParseSmhd() error!", __func__, NULL, NULL, NULL, NULL);
                    }
                } else if (BoxTag == TAG_DINF) {
                    Rval = IsoDmx_ParseDinf(Stream, &Dinf, BoxSize);
                    if (Rval != FORMAT_OK) {
                        AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s IsoDmx_ParseDinf() error!", __func__, NULL, NULL, NULL, NULL);
                    }
                } else if (BoxTag == TAG_STBL) {
                    Rval = IsoDmx_ParseVideoStbl(Track, Stream, IsoTrack, (ParsePos + SVC_ISO_BOX_SIZE_TAG_SIZE), (ParsePos + BoxSize));
                    if (Rval != FORMAT_OK) {
                        AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s IsoDmx_ParseStbl() error!", __func__, NULL, NULL, NULL, NULL);
                    }
                } else {
                    Rval = S2F(Stream->Func->Seek(Stream, ((INT64) ParsePos + (INT64) BoxSize), SVC_STREAM_SEEK_START));
                    if (Rval != FORMAT_OK) {
                        AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s Seek fail!", __func__, NULL, NULL, NULL, NULL);
                    }
                }
                if (Rval == FORMAT_OK) {
                    ParsePos += BoxSize;
                    RemainSize -= BoxSize;
                    if (RemainSize == 0U) {
                        break;
                    }
                }
            }
        } else {
            AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s Get box header error!", __func__, NULL, NULL, NULL, NULL);
        }
    } while (Rval == FORMAT_OK);
    return Rval;
}

static UINT32 IsoDmx_ParseAudioMinf(SVC_AUDIO_TRACK_INFO_s *Track, SVC_STREAM_HDLR_s *Stream, SVC_ISO_AUDIO_TRACK_INFO_s *IsoTrack, UINT64 ParseStartPos, UINT64 ParseEndPos)
{
    UINT32 Rval = FORMAT_OK;
    UINT64 ParsePos = ParseStartPos;
    UINT64 RemainSize = ParseEndPos - ParseStartPos;
    UINT32 BoxSize;
    UINT32 BoxTag;
    ISO_VMHD_BOX_s Vmhd;
    ISO_SMHD_BOX_s Smhd;
    ISO_DINF_BOX_s Dinf;
    do {
        /* Get Box Size, Tag */
        Rval = IsoDmx_GetBoxHeader(Stream, &BoxSize, &BoxTag);
        if (Rval == FORMAT_OK) {
            if (RemainSize < BoxSize) {
                AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s IsoDmx_ParseMinf() fail!", __func__, NULL, NULL, NULL, NULL);
                Rval = FORMAT_ERR_INVALID_FORMAT;
            } else {
                if (BoxTag == TAG_VMHD) {
                    Rval = IsoDmx_ParseVmhd(Stream, &Vmhd, BoxSize);
                    if (Rval != FORMAT_OK) {
                        AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s IsoDmx_ParseVmhd() error!", __func__, NULL, NULL, NULL, NULL);
                    }
                } else if (BoxTag == TAG_SMHD) {
                    Rval = IsoDmx_ParseSmhd(Stream, &Smhd, BoxSize);
                    if (Rval != FORMAT_OK) {
                        AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s IsoDmx_ParseSmhd() error!", __func__, NULL, NULL, NULL, NULL);
                    }
                } else if (BoxTag == TAG_DINF) {
                    Rval = IsoDmx_ParseDinf(Stream, &Dinf, BoxSize);
                    if (Rval != FORMAT_OK) {
                        AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s IsoDmx_ParseDinf() error!", __func__, NULL, NULL, NULL, NULL);
                    }
                } else if (BoxTag == TAG_STBL) {
                    Rval = IsoDmx_ParseAudioStbl(Track, Stream, IsoTrack, (ParsePos + SVC_ISO_BOX_SIZE_TAG_SIZE), (ParsePos + BoxSize));
                    if (Rval != FORMAT_OK) {
                        AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s IsoDmx_ParseStbl() error!", __func__, NULL, NULL, NULL, NULL);
                    }
                } else {
                    Rval = S2F(Stream->Func->Seek(Stream, ((INT64) ParsePos + (INT64) BoxSize), SVC_STREAM_SEEK_START));
                    if (Rval != FORMAT_OK) {
                        AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s Seek fail!", __func__, NULL, NULL, NULL, NULL);
                    }
                }
                if (Rval == FORMAT_OK) {
                    ParsePos += BoxSize;
                    RemainSize -= BoxSize;
                    if (RemainSize == 0U) {
                        break;
                    }
                }
            }
        } else {
            AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s Get box header error!", __func__, NULL, NULL, NULL, NULL);
        }
    } while (Rval == FORMAT_OK);
    return Rval;
}

static UINT32 IsoDmx_ParseTextMinf(SVC_TEXT_TRACK_INFO_s *Track, SVC_STREAM_HDLR_s *Stream, SVC_ISO_TEXT_TRACK_INFO_s *IsoTrack, UINT64 ParseStartPos, UINT64 ParseEndPos)
{
    UINT32 Rval = FORMAT_OK;
    UINT64 ParsePos = ParseStartPos;
    UINT64 RemainSize = ParseEndPos - ParseStartPos;
    UINT32 BoxSize;
    UINT32 BoxTag;
    ISO_VMHD_BOX_s Vmhd;
    ISO_SMHD_BOX_s Smhd;
    ISO_DINF_BOX_s Dinf;
    do {
        /* Get Box Size, Tag */
        Rval = IsoDmx_GetBoxHeader(Stream, &BoxSize, &BoxTag);
        if (Rval == FORMAT_OK) {
            if (RemainSize < BoxSize) {
                AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s IsoDmx_ParseMinf() fail!", __func__, NULL, NULL, NULL, NULL);
                Rval = FORMAT_ERR_INVALID_FORMAT;
            } else {
                if (BoxTag == TAG_VMHD) {
                    Rval = IsoDmx_ParseVmhd(Stream, &Vmhd, BoxSize);
                    if (Rval != FORMAT_OK) {
                        AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s IsoDmx_ParseVmhd() error!", __func__, NULL, NULL, NULL, NULL);
                    }
                } else if (BoxTag == TAG_SMHD) {
                    Rval = IsoDmx_ParseSmhd(Stream, &Smhd, BoxSize);
                    if (Rval != FORMAT_OK) {
                        AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s IsoDmx_ParseSmhd() error!", __func__, NULL, NULL, NULL, NULL);
                    }
                } else if (BoxTag == TAG_DINF) {
                    Rval = IsoDmx_ParseDinf(Stream, &Dinf, BoxSize);
                    if (Rval != FORMAT_OK) {
                        AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s IsoDmx_ParseDinf() error!", __func__, NULL, NULL, NULL, NULL);
                    }
                } else if (BoxTag == TAG_STBL) {
                    Rval = IsoDmx_ParseTextStbl(Track, Stream, IsoTrack, (ParsePos + SVC_ISO_BOX_SIZE_TAG_SIZE), (ParsePos + BoxSize));
                    if (Rval != FORMAT_OK) {
                        AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s IsoDmx_ParseStbl() error!", __func__, NULL, NULL, NULL, NULL);
                    }
                } else {
                    Rval = S2F(Stream->Func->Seek(Stream, ((INT64) ParsePos + (INT64) BoxSize), SVC_STREAM_SEEK_START));
                    if (Rval != FORMAT_OK) {
                        AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s Seek fail!", __func__, NULL, NULL, NULL, NULL);
                    }
                }
                if (Rval == FORMAT_OK) {
                    ParsePos += BoxSize;
                    RemainSize -= BoxSize;
                    if (RemainSize == 0U) {
                        break;
                    }
                }
            }
        } else {
            AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s Get box header error!", __func__, NULL, NULL, NULL, NULL);
        }
    } while (Rval == FORMAT_OK);
    return Rval;
}

static UINT32 IsoDmx_ParseMinf(SVC_MEDIA_INFO_s *Media, SVC_STREAM_HDLR_s *Stream, SVC_ISO_PRIV_INFO_s *IsoDmx, UINT32 HdlrType, UINT64 ParsePos, UINT32 BoxSize, SVC_MEDIA_TRACK_GENERAL_INFO_s **Track)
{
    UINT32 Rval;
    SVC_MOVIE_INFO_s *Movie;
    AmbaMisra_TypeCast(&Movie, &Media);
    if (HdlrType == TAG_VIDE) {
        UINT8 TrackIdx = Movie->VideoTrackCount;
        *Track = &Movie->VideoTrack[TrackIdx].Info;
        Rval = IsoDmx_ParseVideoMinf(&Movie->VideoTrack[TrackIdx], Stream, &IsoDmx->VideoTrack[TrackIdx], (ParsePos + SVC_ISO_BOX_SIZE_TAG_SIZE), (ParsePos + BoxSize));
        if (Rval == FORMAT_OK) {
            Movie->VideoTrackCount++;
        }
    } else if (HdlrType == TAG_SOUN) {
        UINT8 TrackIdx = Movie->AudioTrackCount;
        *Track = &Movie->AudioTrack[TrackIdx].Info;
        Rval = IsoDmx_ParseAudioMinf(&Movie->AudioTrack[TrackIdx], Stream, &IsoDmx->AudioTrack[TrackIdx], (ParsePos + SVC_ISO_BOX_SIZE_TAG_SIZE), (ParsePos + BoxSize));
        if (Rval == FORMAT_OK) {
            Movie->AudioTrackCount++;
        }
    } else if ((HdlrType == TAG_TEXT) || (HdlrType == TAG_SDSM)) {
        UINT8 TrackIdx = Movie->TextTrackCount;
        *Track = &Movie->TextTrack[TrackIdx].Info;
        Rval = IsoDmx_ParseTextMinf(&Movie->TextTrack[TrackIdx], Stream, &IsoDmx->TextTrack[TrackIdx], (ParsePos + SVC_ISO_BOX_SIZE_TAG_SIZE), (ParsePos + BoxSize));
        if (Rval == FORMAT_OK) {
            Movie->TextTrackCount++;
        }
    } else {
        AmbaPrint_ModulePrintUInt5(SVC_DEMUXER_PRINT_MODULE_ID, "IsoDmx_ParseMdia Unsupported Hdlr.ComSubType (%x)!", HdlrType, 0U, 0U, 0U, 0U);
        Rval = FORMAT_ERR_INVALID_FORMAT;
    }
    AmbaMisra_TouchUnused(Media);
    return Rval;
}

static UINT32 IsoDmx_ParseMdia(SVC_MEDIA_INFO_s *Media, SVC_STREAM_HDLR_s *Stream, SVC_ISO_PRIV_INFO_s *IsoDmx, UINT64 ParseStartPos, UINT64 ParseEndPos, SVC_MEDIA_TRACK_GENERAL_INFO_s **Track)
{
    UINT32 Rval = FORMAT_OK;
    UINT64 ParsePos = ParseStartPos;
    UINT64 RemainSize = ParseEndPos - ParseStartPos;
    UINT32 BoxSize;
    UINT32 BoxTag;
    ISO_MDHD_BOX_s Mdhd;
    ISO_HDLR_BOX_s Hdlr;
    Rval = W2F(AmbaWrap_memset(&Mdhd, 0, sizeof(ISO_MDHD_BOX_s)));
    if (Rval == FORMAT_OK) {
        Rval = W2F(AmbaWrap_memset(&Hdlr, 0, sizeof(ISO_HDLR_BOX_s)));
        if (Rval == FORMAT_OK) {
            do {
                /* Get Box Size, Tag */
                Rval = IsoDmx_GetBoxHeader(Stream, &BoxSize, &BoxTag);
                if (Rval == FORMAT_OK) {
                    if (RemainSize < BoxSize) {
                        AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s IsoDmx_ParseTrak() fail!", __func__, NULL, NULL, NULL, NULL);
                        Rval = FORMAT_ERR_INVALID_FORMAT;
                    } else {
                        if (BoxTag == TAG_MDHD) {
                            Rval = IsoDmx_ParseMdhd(Stream, &Mdhd, BoxSize);
                        } else if (BoxTag == TAG_HDLR) {
                            Rval = IsoDmx_ParseHdlr(Stream, &Hdlr, BoxSize);
                        } else if (BoxTag == TAG_MINF) {
                            Rval = IsoDmx_ParseMinf(Media, Stream, IsoDmx, Hdlr.ComSubType, ParsePos, BoxSize, Track);
                        } else {
                            Rval = S2F(Stream->Func->Seek(Stream, ((INT64) ParsePos + (INT64) BoxSize), SVC_STREAM_SEEK_START));
                            if (Rval != FORMAT_OK) {
                                AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s Seek fail!", __func__, NULL, NULL, NULL, NULL);
                            }
                        }
                        if (Rval == FORMAT_OK) {
                            ParsePos += BoxSize;
                            RemainSize -= BoxSize;
                            if (RemainSize ==  0U) {
                                break;
                            }
                        } else {
                            AmbaPrint_ModulePrintUInt5(SVC_DEMUXER_PRINT_MODULE_ID, "IsoDmx_ParseMdia Parse Box %8X error!", BoxTag, 0U, 0U, 0U, 0U);
                        }
                    }
                } else {
                    AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s Get box header error!", __func__, NULL, NULL, NULL, NULL);
                }
            } while (Rval == FORMAT_OK);
        }
    }
    if (Rval == FORMAT_OK) {
        if (*Track != NULL) {
            (*Track)->TimeScale = Mdhd.TimeScale;
        }
    }
    AmbaMisra_TouchUnused(Media);
    return Rval;
}

static UINT32 IsoDmx_ParseTrak(SVC_MEDIA_INFO_s *Media, SVC_STREAM_HDLR_s *Stream, SVC_ISO_PRIV_INFO_s *IsoDmx, UINT64 ParseStartPos, UINT64 ParseEndPos, UINT32 TimeScale)
{
    UINT32 Rval;
    UINT64 ParsePos = ParseStartPos;
    UINT64 RemainSize = ParseEndPos - ParseStartPos;
    UINT32 BoxSize;
    UINT32 BoxTag;
    ISO_TKHD_BOX_s Tkhd;
    ISO_EDTS_BOX_s Edts;
    SVC_MEDIA_TRACK_GENERAL_INFO_s *Track = NULL;
    Rval = W2F(AmbaWrap_memset(&Tkhd, 0, sizeof(ISO_TKHD_BOX_s)));
    if (Rval == FORMAT_OK) {
        Rval = W2F(AmbaWrap_memset(&Edts, 0, sizeof(ISO_EDTS_BOX_s)));
        if (Rval == FORMAT_OK) {
            do {
                /** Get Box Size, Tag */
                Rval = IsoDmx_GetBoxHeader(Stream, &BoxSize, &BoxTag);
                if (Rval == FORMAT_OK) {
                    if (RemainSize < BoxSize) {
                        AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s IsoDmx_ParseTrak() fail!", __func__, NULL, NULL, NULL, NULL);
                        Rval = FORMAT_ERR_INVALID_FORMAT;
                    } else {
                        if (BoxTag == TAG_TKHD) {
                            Rval = IsoDmx_ParseTkhd(Stream, &Tkhd, BoxSize);
                            if (Rval != FORMAT_OK) {
                                AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s IsoDmx_ParseTkhd() error!", __func__, NULL, NULL, NULL, NULL);
                            }
                        } else if (BoxTag == TAG_EDTS) {
                            Rval = IsoDmx_ParseEdts(Stream, &Edts, BoxSize);
                            if (Rval != FORMAT_OK) {
                                AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s IsoDmx_ParseEdts() error!", __func__, NULL, NULL, NULL, NULL);
                            }
                        } else if (BoxTag == TAG_MDIA) {
                            Rval = IsoDmx_ParseMdia(Media, Stream, IsoDmx, (ParsePos + SVC_ISO_BOX_SIZE_TAG_SIZE), (ParsePos + BoxSize), &Track);
                            if (Rval != FORMAT_OK) {
                                AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s IsoDmx_ParseMedia() error!", __func__, NULL, NULL, NULL, NULL);
                            }
                        } else {
                            Rval = S2F(Stream->Func->Seek(Stream, ((INT64) ParsePos + (INT64) BoxSize), SVC_STREAM_SEEK_START));
                            if (Rval != FORMAT_OK) {
                                AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s Seek fail!", __func__, NULL, NULL, NULL, NULL);
                            }
                        }
                        if (Rval == FORMAT_OK) {
                            ParsePos += BoxSize;
                            RemainSize -= BoxSize;
                            if (RemainSize == 0U) {
                                break;
                            }
                        }
                    }
                } else {
                    AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s Get box header error!", __func__, NULL, NULL, NULL, NULL);
                }
            } while (Rval == FORMAT_OK);
            if (Rval == FORMAT_OK) {
                if (Track != NULL) {
                    Track->InitDTS = (UINT64)Edts.ElstAtom.ElstTable1.SegmentDuration * Track->TimeScale / TimeScale;
                    Track->DTS = (UINT64)Tkhd.Duration * Track->TimeScale / TimeScale;
                    Track->NextDTS = Track->DTS;
                } else {
                    AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s track is NULL!", __func__, NULL, NULL, NULL, NULL);
                    Rval = FORMAT_ERR_INVALID_FORMAT;
                }
            }
        }
    }
    return Rval;
}

static UINT32 IsoDmx_ParseUdta(SVC_STREAM_HDLR_s *Stream, UINT64 ParseStartPos, UINT64 ParseEndPos, UINT8 MediaType, SVC_AMBA_BOX_INFORMATION_s *AmbaboxInfo, ISO_UDTA_BOX_s *Udta)
{
    UINT32 Rval = FORMAT_OK;
    UINT64 ParsePos = ParseStartPos;
    UINT64 RemainSize = ParseEndPos - ParseStartPos;
    UINT32 BoxSize;
    UINT32 BoxTag;
    Udta->UserDataSize = 0;
    Udta->UserDataPos = 0;
    do {
        /** Get Box Size, Tag */
        Rval = IsoDmx_GetBoxHeader(Stream, &BoxSize, &BoxTag);
        if (Rval == FORMAT_OK) {
            if (RemainSize < BoxSize) {
                AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s IsoDmx_ParseUdta() fail!", __func__, NULL, NULL, NULL, NULL);
                Rval = FORMAT_ERR_INVALID_FORMAT;
            } else {
                if (BoxTag == TAG_AMBA) {
                    Rval = SvcFormat_ParseAmbaBox(Stream, MediaType, BoxSize, AmbaboxInfo);
                    if (Rval != FORMAT_OK) {
                        AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s SvcFormat_ParseAmbaBox() error!", __func__, NULL, NULL, NULL, NULL);
                    }
                } else {
                    if (Udta->UserDataPos == 0) {
                        Udta->UserDataPos = (INT64) ParsePos;
                    }
                    Rval = S2F(Stream->Func->Seek(Stream, ((INT64) ParsePos + (INT64) BoxSize), SVC_STREAM_SEEK_START));
                    if (Rval == FORMAT_OK) {
                        Udta->UserDataSize += BoxSize;
                    } else {
                        AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s Seek fail!", __func__, NULL, NULL, NULL, NULL);
                    }
                }
                if (Rval == FORMAT_OK) {
                    ParsePos += BoxSize;
                    RemainSize -= BoxSize;
                    if (RemainSize == 0U) {
                        break;
                    }
                }
            }
        } else {
            AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s Get box header error!", __func__, NULL, NULL, NULL, NULL);
        }
    } while(Rval == FORMAT_OK);
    return Rval;
}

static UINT32 IsoDmx_ParseMvex(SVC_STREAM_HDLR_s *Stream, UINT64 ParseStartPos, UINT64 ParseEndPos, UINT32 *TimePerFrame)
{
    UINT32 Rval = FORMAT_OK;
    UINT64 ParsePos = ParseStartPos;
    UINT64 RemainSize = ParseEndPos - ParseStartPos;
    UINT32 BoxSize, BoxTag;
    do {
        /** Get Box Size, Tag */
        Rval = IsoDmx_GetBoxHeader(Stream, &BoxSize, &BoxTag);
        if (Rval == FORMAT_OK) {
            if (RemainSize < BoxSize) {
                AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s IsoDmx_ParseMvex() fail!", __func__, NULL, NULL, NULL, NULL);
                Rval = FORMAT_ERR_INVALID_FORMAT;
            } else {
                if (BoxTag == TAG_TREX) {
                    UINT32 TmpU32, DefaultSampleDuration = 0U;
                    UINT8 TrackId = 0U;
                    Rval = SvcFormat_GetBe32(Stream, &TmpU32); /* VersionFlag */
                    if (Rval == FORMAT_OK) {
                        Rval = SvcFormat_GetBe32(Stream, &TmpU32);
                    } else {
                        AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s SvcFormat_GetBe32() error!", __func__, NULL, NULL, NULL, NULL);
                    }
                    if (Rval == FORMAT_OK) {
                        TrackId = (UINT8)TmpU32;
                        Rval = SvcFormat_GetBe32(Stream, &TmpU32); /* DefaultDescripIndex*/
                    } else {
                        AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s SvcFormat_GetBe32() error!", __func__, NULL, NULL, NULL, NULL);
                    }
                    if (Rval == FORMAT_OK) {
                        Rval = SvcFormat_GetBe32(Stream, &DefaultSampleDuration);
                    } else {
                        AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s SvcFormat_GetBe32() error!", __func__, NULL, NULL, NULL, NULL);
                    }
                    if (Rval == FORMAT_OK) {
                        Rval = SvcFormat_GetBe32(Stream, &TmpU32); /* DefaultSampleSize*/
                    } else {
                        AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s SvcFormat_GetBe32() error!", __func__, NULL, NULL, NULL, NULL);
                    }
                    if (Rval == FORMAT_OK) {
                        Rval = SvcFormat_GetBe32(Stream, &TmpU32); /* DefaultSampleFlags*/
                    } else {
                        AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s SvcFormat_GetBe32() error!", __func__, NULL, NULL, NULL, NULL);
                    }
                    if (Rval == FORMAT_OK) {
                        if ((TrackId <= SVC_FORMAT_MAX_TRACK_PER_MEDIA) && (TrackId > 0U)) {
                            TimePerFrame[TrackId - 1U] = DefaultSampleDuration;
                        } else {
                            AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s Incorrect TrackId!", __func__, NULL, NULL, NULL, NULL);
                            Rval = FORMAT_ERR_INVALID_FORMAT;
                        }
                    } else {
                        AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s SvcFormat_GetBe32() error!", __func__, NULL, NULL, NULL, NULL);
                    }
                } else {
                    Rval = S2F(Stream->Func->Seek(Stream, ((INT64) ParsePos + (INT64) BoxSize), SVC_STREAM_SEEK_START));
                    if (Rval != FORMAT_OK) {
                        AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s Seek fail!", __func__, NULL, NULL, NULL, NULL);
                    }
                }
                if (Rval == FORMAT_OK) {
                    ParsePos += BoxSize;
                    RemainSize -= BoxSize;
                    if (RemainSize == 0U) {
                        break;
                    }
                }
            }
        } else {
            AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s Get box header error!", __func__, NULL, NULL, NULL, NULL);
        }
    } while (Rval == FORMAT_OK);
    return Rval;
}

static UINT32 IsoDmx_MergeMediaInfoMw(SVC_MOVIE_INFO_s *Movie, const SVC_AMBA_BOX_INFORMATION_s *AmbaBox, UINT8 IsFmp4)
{
    UINT32 Rval = FORMAT_OK;
    SVC_MEDIA_TRACK_GENERAL_INFO_s *Track;
    const SVC_AMBA_BOX_TRACK_GENERAL_INFO_s *AmbaTrack;
    if (AmbaBox->MainVer >= 2U) {
        // double check with AMBA_BOX
        if ((Movie->VideoTrackCount != AmbaBox->VideoTrackCount) || (Movie->AudioTrackCount != AmbaBox->AudioTrackCount) || (Movie->TextTrackCount != AmbaBox->TextTrackCount)) {
            AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s TrackCount incorrect", __func__, NULL, NULL, NULL, NULL);
            Rval = FORMAT_ERR_INVALID_FORMAT;
        }
    }
    if (Rval == FORMAT_OK) {
        UINT8 i;
        for (i = 0; i < Movie->VideoTrackCount; i++) {
            Track = &Movie->VideoTrack[i].Info;
            AmbaTrack = &AmbaBox->VideoTrack[i].Info;
            if (Track->TimeScale != AmbaTrack->TimeScale) {
                AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s Video incorrect timescale", __func__, NULL, NULL, NULL, NULL);
                Rval = FORMAT_ERR_INVALID_FORMAT;
                break;
            }
            /* match, adjust to the precies values kept in ambabox */
            Track->TimeScale = AmbaTrack->TimeScale;
            Track->TimePerFrame = AmbaTrack->TimePerFrame;

            /* If does not have initial time, duation and frame count, get from ambabox */
            Track->InitDTS = AmbaTrack->InitDTS;
            if (IsFmp4 == 0U) {
                // no statistic data for FMP4 (from the tail)
                Track->NextDTS = Track->InitDTS + AmbaTrack->Duration;
                Track->DTS = Track->NextDTS;
                Track->FrameCount = AmbaTrack->FrameCount;
            }
        }
    }
    if (Rval == FORMAT_OK) {
        UINT8 i;
        for (i = 0; i < Movie->AudioTrackCount; i++) {
            SVC_AUDIO_TRACK_INFO_s *Audio;
            Audio = &Movie->AudioTrack[i];
            Track = &Audio->Info;
            AmbaTrack = &AmbaBox->AudioTrack[i].Info;
            if (Audio->SampleRate != AmbaTrack->TimeScale) {
                AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s Aduio incorrect timescale", __func__, NULL, NULL, NULL, NULL);
                Rval = FORMAT_ERR_INVALID_FORMAT;
                break;
            }
            /* audio has no time scale and time per frame, get from ambabox */
            Track->TimeScale = AmbaTrack->TimeScale;
            Track->TimePerFrame = AmbaTrack->TimePerFrame;

            /* If does not have initial time, duation and frame count, get from ambabox */
            Track->InitDTS = AmbaTrack->InitDTS;
            AmbaPrint_PrintUInt5("[DBG] Merge AudioTrack InitDts %u, TimeScale %u, Tpf %u", (UINT32)Track->InitDTS, Track->TimeScale, Track->TimePerFrame, 0U, 0U);
            if (IsFmp4 == 0U) {
                // no statistic data for FMP4 (from the tail)
                Track->NextDTS = Track->InitDTS + AmbaTrack->Duration;
                Track->DTS = Track->NextDTS;
                Track->FrameCount = AmbaTrack->FrameCount;
            }
        }
    }
    if (Rval == FORMAT_OK) {
        UINT8 i;
        for (i = 0; i < Movie->TextTrackCount; i++) {
            Track = &Movie->TextTrack[i].Info;
            AmbaTrack = &AmbaBox->TextTrack[i].Info;
            if (Track->TimeScale != AmbaTrack->TimeScale) {
                AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s Text incorrect timescale", __func__, NULL, NULL, NULL, NULL);
                Rval = FORMAT_ERR_INVALID_FORMAT;
                break;
            }
            /* match, adjust to the precies values kept in ambabox */
            Track->TimeScale = AmbaTrack->TimeScale;
            Track->TimePerFrame = AmbaTrack->TimePerFrame;

            /* If does not have initial time, duation and frame count, get from ambabox */
            Track->InitDTS = AmbaTrack->InitDTS;
            if (IsFmp4 == 0U) {
                // no statistic data for FMP4 (from the tail)
                Track->NextDTS = Track->InitDTS + AmbaTrack->Duration;
                Track->DTS = Track->NextDTS;
                Track->FrameCount = AmbaTrack->FrameCount;
            }
        }
    }
    return Rval;
}

static UINT32 IsoDmx_MergeMediaInfoSvc(SVC_MOVIE_INFO_s *Movie, const UINT32 *TimePerFrame, UINT8 IsFmp4)
{
    if (IsFmp4 == 1U) {
        UINT8 TrackIdx = 0U, i;
        for (i = 0; i < Movie->VideoTrackCount; i++) {
            Movie->VideoTrack[i].Info.TimePerFrame = TimePerFrame[TrackIdx];
            TrackIdx++;
        }
        for (i = 0; i < Movie->AudioTrackCount; i++) {
            Movie->AudioTrack[i].Info.TimePerFrame = TimePerFrame[TrackIdx];
            TrackIdx++;
        }
        for (i = 0; i < Movie->TextTrackCount; i++) {
            Movie->TextTrack[i].Info.TimePerFrame = TimePerFrame[TrackIdx];
            TrackIdx++;
        }
    }
    return FORMAT_OK;
}

static UINT32 IsoDmx_MergeMediaInfo(SVC_MEDIA_INFO_s *Media, const SVC_AMBA_BOX_INFORMATION_s *AmbaBox, const UINT32 *TimePerFrame, UINT8 IsFmp4)
{
    UINT32 Rval = FORMAT_OK;
    if (Media->MediaType == SVC_MEDIA_INFO_MOVIE) {
        SVC_MOVIE_INFO_s *Movie;
        AmbaMisra_TypeCast(&Movie, &Media);
        if (AmbaBox->Type == SVC_AMBA_BOX_TYPE_MW) {
            Rval = IsoDmx_MergeMediaInfoMw(Movie, AmbaBox, IsFmp4);
        } else {
            Rval = IsoDmx_MergeMediaInfoSvc(Movie, TimePerFrame, IsFmp4);
        }
    } else {
        AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s Unsupport media!", __func__, NULL, NULL, NULL, NULL);
        Rval = FORMAT_ERR_INVALID_FORMAT;
    }
    AmbaMisra_TouchUnused(Media);
    return Rval;
}

/**
 *  Parse a moov box
 *
 *  @param [in,out] Media Media information
 *  @param [in] Stream Stream handler
 *  @param [in,out] IsoDmx ISO private information
 *  @param [in] ParseStartPos Start position of the box
 *  @param [in] ParseEndPos End position of the box
 *  @param [in] IsFmp4 Indicating if the clip is a fmp4 or a traditional mp4
 *  @return 0 - OK, others - FORMAT_ERR_XXXX
 */
UINT32 IsoDmx_ParseMoov(SVC_MEDIA_INFO_s *Media, SVC_STREAM_HDLR_s *Stream, SVC_ISO_PRIV_INFO_s *IsoDmx, UINT64 ParseStartPos, UINT64 ParseEndPos, UINT8 IsFmp4)
{
    UINT32 Rval = FORMAT_OK;
    UINT64 ParsePos = ParseStartPos;
    UINT64 RemainSize = ParseEndPos - ParseStartPos;
    UINT32 BoxSize;
    UINT32 BoxTag;
    ISO_MVHD_BOX_s Mvhd;
    SVC_AMBA_BOX_INFORMATION_s AmbaBoxInfo = {0};
    SVC_MOVIE_INFO_s *Movie;
    UINT32 TimePerFrame[SVC_FORMAT_MAX_TRACK_PER_MEDIA] = {0};
    UINT8 TrakFound = 0U, UdtaFound = 0U;
    AmbaMisra_TypeCast(&Movie, &Media);
    Rval = W2F(AmbaWrap_memset(&Mvhd, 0, sizeof(ISO_MVHD_BOX_s)));
    if (Rval == FORMAT_OK) {
        do {
            /* Get Box Size, Tag */
            Rval = IsoDmx_GetBoxHeader(Stream, &BoxSize, &BoxTag);
            if (Rval == FORMAT_OK) {
                if (RemainSize < BoxSize) {
                    AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s Parse position incorrect!", __func__, NULL, NULL, NULL, NULL);
                    Rval = FORMAT_ERR_INVALID_FORMAT;
                } else {
                    if (BoxTag == TAG_MVHD) {
                        Rval = IsoDmx_ParseMvhd(Stream, &Mvhd, BoxSize);
                        if (Rval == FORMAT_OK) {
                            Media->CreationTime = Mvhd.CreateTime;
                            Media->ModificationTime = Mvhd.ModifyTime;
                        }
                    } else if (BoxTag == TAG_UDTA) {
                        ISO_UDTA_BOX_s Udta;
                        if (UdtaFound == 0U) {
                            UdtaFound = 1U;
                            AmbaBoxInfo.Type = (TrakFound == 1U)? SVC_AMBA_BOX_TYPE_SVC : SVC_AMBA_BOX_TYPE_MW;
                        }
                        Rval = IsoDmx_ParseUdta(Stream, (ParsePos + SVC_ISO_BOX_SIZE_TAG_SIZE), (ParsePos + BoxSize), Media->MediaType, &AmbaBoxInfo, &Udta);
                        if (Rval == FORMAT_OK) {
                            //Movie->VideoTrackCount = AmbaBoxInfo.VideoTrackCount;
                            //Movie->AudioTrackCount = AmbaBoxInfo.AudioTrackCount;
                            //Movie->TextTrackCount = AmbaBoxInfo.TextTrackCount;
                            Movie->UserDataSize = Udta.UserDataSize;
                            Movie->UserDataPos = Udta.UserDataPos;
                        }
                    } else if (BoxTag == TAG_TRAK) {
                        TrakFound = 1U;
                        Rval = IsoDmx_ParseTrak(Media, Stream, IsoDmx, (ParsePos + SVC_ISO_BOX_SIZE_TAG_SIZE), (ParsePos + BoxSize), Mvhd.TimeScale);
                    } else if (BoxTag == TAG_MVEX) {
                        // For svc sdk7 fmp4 clips
                        Rval = IsoDmx_ParseMvex(Stream, (ParsePos + SVC_ISO_BOX_SIZE_TAG_SIZE), (ParsePos + BoxSize), TimePerFrame);
                    } else {
                        Rval = S2F(Stream->Func->Seek(Stream, ((INT64) ParsePos + (INT64) BoxSize), SVC_STREAM_SEEK_START));
                        if (Rval != FORMAT_OK) {
                            AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s Seek failed!", __func__, NULL, NULL, NULL, NULL);
                        }
                    }
                    if (Rval == FORMAT_OK) {
                        ParsePos += BoxSize;
                        RemainSize -= BoxSize;
                        if (RemainSize == 0U) {
                            break;
                        }
                    }
                }
            } else {
                AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s Get box header error!", __func__, NULL, NULL, NULL, NULL);
            }
        } while (Rval == FORMAT_OK);
    }
    if (Rval == FORMAT_OK) {
        if (UdtaFound == 0U) {
            AmbaBoxInfo.Type = SVC_AMBA_BOX_TYPE_SVC;
        }
        Rval = IsoDmx_MergeMediaInfo(Media, &AmbaBoxInfo, TimePerFrame, IsFmp4);
        if (Rval == FORMAT_OK) {
            Rval = SvcFormat_GetMediaInfo(Media, &AmbaBoxInfo);
        }
    }
    return Rval;
}

/**
 *  Parse a ftyp box
 *
 *  @param [in] Stream Stream handler
 *  @param [out] IsoDmx ISO private information
 *  @param [in] ParseStartPos Start position of the box
 *  @param [in] ParseEndPos End position of the box
 *  @return 0 - OK, others - FORMAT_ERR_XXXX
 */
UINT32 IsoDmx_ParseFtyp(SVC_STREAM_HDLR_s *Stream, SVC_ISO_PRIV_INFO_s *IsoDmx, UINT64 ParseStartPos, UINT64 ParseEndPos)
{
    UINT32 Rval = FORMAT_OK;
    const UINT64 BoxSizeU64 = ParseEndPos - ParseStartPos;
    const UINT32 BoxSize = (UINT32) BoxSizeU64;
    if (BoxSize == 24U) {
        IsoDmx->FtypSize = BoxSize + SVC_ISO_BOX_SIZE_TAG_SIZE;
        Rval = S2F(Stream->Func->Seek(Stream, (INT64) ParseEndPos, SVC_STREAM_SEEK_START));
        if (Rval != FORMAT_OK) {
            AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s Seek fail!", __func__, NULL, NULL, NULL, NULL);
        }
    } else {
        AmbaPrint_ModulePrintUInt5(SVC_DEMUXER_PRINT_MODULE_ID, "IsoDmx_ParseFtyp incorrect BoxSize %u", BoxSize, 0U, 0U, 0U, 0U);
        Rval = FORMAT_ERR_INVALID_FORMAT;
    }
    return Rval;
}

static void IsoDmx_InitFeedVideoInfo(SVC_FRAME_FEEDER_INFO_s *FeedInfo, const SVC_VIDEO_TRACK_INFO_s *Track, SVC_ISO_VIDEO_TRACK_INFO_s *IsoVideo)
{
    if (Track->Info.MediaId == SVC_FORMAT_MID_AVC) {
        FeedInfo->Info.H264.SPS = IsoVideo->Avc.SPS;
        FeedInfo->Info.H264.SPSLen = IsoVideo->Avc.SPSLen;
        FeedInfo->Info.H264.PPS = IsoVideo->Avc.PPS;
        FeedInfo->Info.H264.PPSLen = IsoVideo->Avc.PPSLen;
    } else {
        FeedInfo->Info.H265.VPS = IsoVideo->Hvc.VPS;
        FeedInfo->Info.H265.VPSLen = IsoVideo->Hvc.VPSLen;
        FeedInfo->Info.H265.SPS = IsoVideo->Hvc.SPS;
        FeedInfo->Info.H265.SPSLen = IsoVideo->Hvc.SPSLen;
        FeedInfo->Info.H265.PPS = IsoVideo->Hvc.PPS;
        FeedInfo->Info.H265.PPSLen = IsoVideo->Hvc.PPSLen;
    }
}

static UINT32 IsoDmx_ProcessVideo(SVC_VIDEO_TRACK_INFO_s *Track, SVC_STREAM_HDLR_s *Stream, void *IsoIdx, SVC_FRAME_FEEDER_INFO_s *TrackFeedInfo, UINT8 TrackId, UINT8 End)
{
    INT32 NextFrameOffset;
    INT32 NextTimeOffset;
    UINT32 Rval = SvcFrameFeeder_ProcessVideo(TrackFeedInfo, Stream, Track, IsoIdx, TrackId, &NextFrameOffset, &NextTimeOffset, End);
    if (Rval == FORMAT_OK) {
        SvcDemuxer_UpdateTrack(&Track->Info, NextFrameOffset, NextTimeOffset);
    }
    return Rval;
}

static UINT32 IsoDmx_ProcessAudio(SVC_AUDIO_TRACK_INFO_s *Track, SVC_STREAM_HDLR_s *Stream, void *IsoIdx, SVC_FRAME_FEEDER_INFO_s *TrackFeedInfo, UINT8 TrackId, UINT8 End)
{
    INT32 NextFrameOffset;
    INT32 NextTimeOffset;
    UINT32 Rval = SvcFrameFeeder_ProcessAudio(TrackFeedInfo, Stream, Track, IsoIdx, TrackId, &NextFrameOffset, &NextTimeOffset, End);
    if (Rval == FORMAT_OK) {
        SvcDemuxer_UpdateTrack(&Track->Info, NextFrameOffset, NextTimeOffset);
    }
    return Rval;
}

static UINT32 IsoDmx_ProcessText(SVC_TEXT_TRACK_INFO_s *Track, SVC_STREAM_HDLR_s *Stream, void *IsoIdx, SVC_FRAME_FEEDER_INFO_s *TrackFeedInfo, UINT8 TrackId, UINT8 End)
{
    INT32 NextFrameOffset = 0;
    INT32 NextTimeOffset = 0;
    UINT32 Rval = SvcFrameFeeder_ProcessText(TrackFeedInfo, Stream, Track, IsoIdx, TrackId, &NextFrameOffset, &NextTimeOffset, End);
    if (Rval == FORMAT_OK) {
        SvcDemuxer_UpdateTrack(&Track->Info, NextFrameOffset, NextTimeOffset);
    }
    return Rval;
}

static UINT32 IsoDmx_ForwardMovie(SVC_MOVIE_INFO_s *Movie, SVC_STREAM_HDLR_s *Stream, void *IsoIdx, SVC_FRAME_FEEDER_INFO_s *FeedInfo, UINT64 TargetTime, UINT8 End, UINT8 *Event)
{
    UINT32 Rval = FORMAT_OK;
    const SVC_MEDIA_TRACK_GENERAL_INFO_s *Track = NULL;
    SVC_FRAME_FEEDER_INFO_s *TrackFeedInfo;
    UINT64 TargetDTS;
    UINT8 Done = 0U;
    if (TargetTime == SVC_FORMAT_MAX_TIMESTAMP) {
        TargetDTS = SVC_FORMAT_MAX_TIMESTAMP;
    } else {
        TargetDTS = SVC_FORMAT_TIME_TO_DTS_FLOOR(TargetTime, Movie->VideoTrack[0].Info.TimeScale);
    }
    do {
        UINT8 TrackType, TrackIdx;
        TrackType = SvcFormat_GetShortestTrack(Movie->VideoTrack, Movie->VideoTrackCount, Movie->AudioTrack, Movie->AudioTrackCount, Movie->TextTrack, Movie->TextTrackCount, &TrackIdx);  // also implies EOS check
        if (TrackType == 0U) {    /* All Track EOS */
            *Event = FORMAT_EVENT_REACH_END;
            Done = 1U;
        } else {
            if (TrackType == SVC_MEDIA_TRACK_TYPE_VIDEO) {
                SVC_VIDEO_TRACK_INFO_s *Video = &Movie->VideoTrack[TrackIdx];
                Track = &Video->Info;
                if (Track->DTS >= TargetDTS) {
                    Done = 1U;
                } else {
                    UINT8 TrackId = TrackIdx;
                    TrackFeedInfo = &FeedInfo[TrackId];
                    Rval = IsoDmx_ProcessVideo(Video, Stream, IsoIdx, TrackFeedInfo, TrackId, End);
                }
            } else if (TrackType == SVC_MEDIA_TRACK_TYPE_AUDIO) {
                SVC_AUDIO_TRACK_INFO_s *Audio = &Movie->AudioTrack[TrackIdx];
                Track = &Audio->Info;
                if (Track->DTS >= TargetDTS) {
                    Done = 1U;
                } else {
                    UINT8 TrackId = TrackIdx + Movie->VideoTrackCount;
                    TrackFeedInfo = &FeedInfo[TrackId];
                    Rval = IsoDmx_ProcessAudio(Audio, Stream, IsoIdx, TrackFeedInfo, TrackId, End);
                }
            } else {
                SVC_TEXT_TRACK_INFO_s *Text = &Movie->TextTrack[TrackIdx];
                Track = &Text->Info;
                if (Track->DTS >= TargetDTS) {
                    Done = 1U;
                } else {
                    UINT8 TrackId = TrackIdx + Movie->VideoTrackCount + Movie->AudioTrackCount;
                    TrackFeedInfo = &FeedInfo[TrackId];
                    Rval = IsoDmx_ProcessText(Text, Stream, IsoIdx, TrackFeedInfo, TrackId, End);
                }
            }
        }
    } while ((Rval == FORMAT_OK) && (Done == 0U));
    return Rval;
}

static UINT32 IsoDmx_BackwardMovie(SVC_MOVIE_INFO_s *Movie, SVC_STREAM_HDLR_s *Stream, void *IsoIdx, SVC_FRAME_FEEDER_INFO_s *FeedInfo, UINT64 TargetTime, UINT8 *Event)
{
    UINT32 Rval = FORMAT_OK;
    const SVC_MEDIA_TRACK_GENERAL_INFO_s *Track = NULL;
    SVC_FRAME_FEEDER_INFO_s *TrackFeedInfo;
    UINT64 TargetDTS;
    UINT8 Done = 0U;
    if (TargetTime == 0U) {
        TargetDTS = 0;
    } else {
        TargetDTS = SVC_FORMAT_TIME_TO_DTS(TargetTime, Movie->VideoTrack[0].Info.TimeScale);
    }
    do {
        UINT8 TrackType, TrackIdx;
        TrackType = SvcFormat_GetLongestTrack(Movie->VideoTrack, Movie->VideoTrackCount, Movie->AudioTrack, Movie->AudioTrackCount, Movie->TextTrack, Movie->TextTrackCount, &TrackIdx);   // also implies EOS check
        if (TrackType == 0U) {    /* All Track EOS */
            *Event = FORMAT_EVENT_REACH_END;
            Done = 1U;
        } else {
            if (TrackType == SVC_MEDIA_TRACK_TYPE_VIDEO) {
                SVC_VIDEO_TRACK_INFO_s *Video = &Movie->VideoTrack[TrackIdx];
                Track = &Video->Info;
                if (Track->DTS <= TargetDTS) {
                    Done = 1U;
                } else {
                    UINT8 TrackId = TrackIdx;
                    TrackFeedInfo = &FeedInfo[TrackId];
                    Rval = IsoDmx_ProcessVideo(Video, Stream, IsoIdx, TrackFeedInfo, TrackId, 0U);
                }
            } else if (TrackType == SVC_MEDIA_TRACK_TYPE_AUDIO) {
                SVC_AUDIO_TRACK_INFO_s *Audio = &Movie->AudioTrack[TrackIdx];
                Track = &Audio->Info;
                if (Track->DTS <= TargetDTS) {
                    Done = 1U;
                } else {
                    UINT8 TrackId = TrackIdx + Movie->VideoTrackCount;
                    TrackFeedInfo = &FeedInfo[TrackId];
                    Rval = IsoDmx_ProcessAudio(Audio, Stream, IsoIdx, TrackFeedInfo, TrackId, 0U);
                }
            } else {
                SVC_TEXT_TRACK_INFO_s *Text = &Movie->TextTrack[TrackIdx];
                Track = &Text->Info;
                if (Track->DTS <= TargetDTS) {
                    Done = 1U;
                } else {
                    UINT8 TrackId = TrackIdx + Movie->VideoTrackCount + Movie->AudioTrackCount;
                    TrackFeedInfo = &FeedInfo[TrackId];
                    Rval = IsoDmx_ProcessText(Text, Stream, IsoIdx, TrackFeedInfo, TrackId, 0U);
                }
            }
        }
    } while ((Rval == FORMAT_OK) && (Done == 0U));
    return Rval;
}

/**
 *  Process of demuxing a media.
 *
 *  @param [in] Media Media information
 *  @param [in] Stream Stream handler
 *  @param [in] IsoIdx ISO Index
 *  @param [in] FeedInfo Frame feeder information
 *  @param [in] TargetTime Target time to process
 *  @param [in] Direction Direction of process
 *  @param [in] End Indicating the end of process
 *  @param [out] Event The returned process event
 *  @return 0 - OK, others - FORMAT_ERR_XXXX
 */
UINT32 IsoDmx_Process(SVC_MEDIA_INFO_s *Media, SVC_STREAM_HDLR_s *Stream, void *IsoIdx, SVC_FRAME_FEEDER_INFO_s *FeedInfo, UINT64 TargetTime, UINT8 Direction, UINT8 End, UINT8 *Event)
{
    UINT32 Rval = FORMAT_OK;
    *Event = FORMAT_EVENT_NONE;
    if (Media->MediaType == SVC_MEDIA_INFO_MOVIE) {
        SVC_MOVIE_INFO_s *Movie;
        AmbaMisra_TypeCast(&Movie, &Media);
        if (Direction == SVC_FORMAT_DIR_FORWARD) {
            Rval = IsoDmx_ForwardMovie(Movie, Stream, IsoIdx, FeedInfo, TargetTime, End, Event);
        } else {
            Rval = IsoDmx_BackwardMovie(Movie, Stream, IsoIdx, FeedInfo, TargetTime, Event);
        }
        /* When find the format is invalid, set valid bit as false. */
        if ((Rval == FORMAT_ERR_INVALID_FORMAT) || (Rval == FORMAT_ERR_NOT_ENOUGH_BYTES)) {
            Rval = FORMAT_ERR_INVALID_FORMAT;
            Movie->MediaInfo.Valid = 0U;
        }
    } else {
        AmbaPrint_ModulePrintUInt5(SVC_DEMUXER_PRINT_MODULE_ID, "Process: incorrect media type %u", Media->MediaType, 0U, 0U, 0U, 0U);
        Rval = FORMAT_ERR_INVALID_FORMAT;
    }
    AmbaMisra_TouchUnused(Media);
    return Rval;
}

/**
 *  Initialize frame feeder information
 *
 *  @param [in] Movie Movie information
 *  @param [out] FeedInfo Frame feeder information
 *  @param [in] GetFrameInfo Function pointer to get frame information
 *  @param [in] Direction Direction of process
 *  @param [in] Speed Processing speed
 *  @return 0 - OK, others - FORMAT_ERR_XXXX
 */
UINT32 IsoDmx_InitFeedInfo(SVC_MOVIE_INFO_s *Movie, SVC_FRAME_FEEDER_INFO_s *FeedInfo, ISODMX_GET_FRAME_INFO_FP GetFrameInfo, UINT8 Direction, UINT8 Speed)
{
    UINT32 Rval = FORMAT_OK;
    UINT8 i, TrackId = 0;
    SVC_ISO_PRIV_INFO_s *IsoInfo = &Movie->IsoInfo;
    SVC_FORMAT_FEED_FRAME_FP_s FeedFrameFP;
    FeedFrameFP.FeedVideoFrame = SvcFormat_FeedVideoFrame;
    FeedFrameFP.FeedAudioFrame = SvcFormat_FeedAudioFrame;
    FeedFrameFP.FeedTextFrame = SvcFormat_FeedTextFrame;
    for (i = 0; i < Movie->VideoTrackCount; i++) {
        SVC_MEDIA_TRACK_GENERAL_INFO_s * const Track = &Movie->VideoTrack[i].Info;
        SVC_FRAME_FEEDER_INFO_s * const TrackFeedInfo = &FeedInfo[TrackId];
        SVC_ISO_VIDEO_TRACK_INFO_s * const IsoTrack = &IsoInfo->VideoTrack[i];
        IsoDmx_InitFeedVideoInfo(TrackFeedInfo, &Movie->VideoTrack[i], IsoTrack);
        Rval = SvcFrameFeeder_InitFeedInfo(TrackFeedInfo, Track, TrackId, GetFrameInfo, &FeedFrameFP, Track->DTS, Direction, Speed);
        if (Rval != FORMAT_OK) {
            break;
        }
        TrackId++;
    }
    if (Rval == FORMAT_OK) {
        for (i = 0; i < Movie->AudioTrackCount; i++) {
            SVC_MEDIA_TRACK_GENERAL_INFO_s * const Track = &Movie->AudioTrack[i].Info;
            SVC_FRAME_FEEDER_INFO_s * const TrackFeedInfo = &FeedInfo[TrackId];
            Rval = SvcFrameFeeder_InitFeedInfo(TrackFeedInfo, Track, TrackId, GetFrameInfo, &FeedFrameFP, Track->DTS, Direction, Speed);
            if (Rval != FORMAT_OK) {
                break;
            }
            TrackId++;
        }
    }

    if (Rval == FORMAT_OK) {
        for (i = 0; i < Movie->TextTrackCount; i++) {
            SVC_MEDIA_TRACK_GENERAL_INFO_s * const Track = &Movie->TextTrack[i].Info;
            SVC_FRAME_FEEDER_INFO_s * const TrackFeedInfo = &FeedInfo[TrackId];
            Rval = SvcFrameFeeder_InitFeedInfo(TrackFeedInfo, Track, TrackId, GetFrameInfo, &FeedFrameFP, Track->DTS, Direction, Speed);
            if (Rval != FORMAT_OK) {
                break;
            }
            TrackId++;
        }
    }
    return Rval;
}

static UINT32 CheckSeekParam(const SVC_DMX_FORMAT_HDLR_s *Hdlr, UINT32 Direction)
{
    UINT32 Rval;
    const SVC_MEDIA_INFO_s * const Media = Hdlr->Media;
    if ((Media->Valid != 1U) || (Media->MediaType != SVC_MEDIA_INFO_MOVIE)) {
        AmbaPrint_ModulePrintUInt5(SVC_DEMUXER_PRINT_MODULE_ID, "Seek: Invalid media %u or media type %u", Media->Valid, Media->MediaType, 0U, 0U, 0U);
        Rval = FORMAT_ERR_GENERAL_ERROR;
    } else if ((Direction != SVC_FORMAT_DIR_FORWARD) && (Direction != SVC_FORMAT_DIR_BACKWARD)) {
        AmbaPrint_ModulePrintUInt5(SVC_DEMUXER_PRINT_MODULE_ID, "Seek: incorrect Direction %u", Direction, 0U, 0U, 0U, 0U);
        Rval = FORMAT_ERR_GENERAL_ERROR;
    } else {
        Rval = FORMAT_OK;
    }
    return Rval;
}

static UINT32 IsoDmx_SeekDefaultVideo(SVC_MOVIE_INFO_s *Movie, void *IsoIdx, UINT64 TargetTime, UINT8 Direction, SVC_FORMAT_GET_FRAME_INFO_FP GetFrameInfo, const SVC_MEDIA_TRACK_GENERAL_INFO_s **DefTrack)
{
    UINT32 Rval = FORMAT_OK;
    UINT8 TrackId;
    SVC_VIDEO_TRACK_INFO_s *DefVideo = SvcFormat_GetDefaultVideoTrack(Movie->VideoTrack, Movie->VideoTrackCount, &TrackId);
    if (DefVideo != NULL) {
        const SVC_MEDIA_TRACK_GENERAL_INFO_s *TrackInfo = &DefVideo->Info;
        UINT64 TargetDTS;
        *DefTrack = &DefVideo->Info;
        if (Direction == SVC_FORMAT_DIR_FORWARD) {
            TargetDTS = TrackInfo->InitDTS + SVC_FORMAT_TIME_TO_DTS_FLOOR(TargetTime, TrackInfo->TimeScale);
        } else {
            TargetDTS = TrackInfo->InitDTS + SVC_FORMAT_TIME_TO_DTS(TargetTime, TrackInfo->TimeScale);
        }
        Rval = SvcFormat_SeekVideo(DefVideo, IsoIdx, TrackId, GetFrameInfo, Direction, TargetDTS);
    } else {
        Rval = FORMAT_ERR_GENERAL_ERROR;
    }
    return Rval;
}

static UINT32 IsoDmx_SeekOtherVideo(SVC_MOVIE_INFO_s *Movie, void *IsoIdx, SVC_FRAME_FEEDER_INFO_s *FeedInfo, UINT8 Direction, UINT32 Speed, SVC_FORMAT_GET_FRAME_INFO_FP GetFrameInfo, SVC_FORMAT_FEED_FRAME_FP_s feedFrameFP, const SVC_MEDIA_TRACK_GENERAL_INFO_s *DefTrack)
{
    UINT32 Rval = FORMAT_OK;
    UINT64 TargetDTS = DefTrack->DTS;
    UINT8 i, TrackId = 0;
    for (i = 0; i < Movie->VideoTrackCount; i++) {
        SVC_ISO_VIDEO_TRACK_INFO_s * const IsoTrack = &Movie->IsoInfo.VideoTrack[i];
        SVC_VIDEO_TRACK_INFO_s * const Video = &Movie->VideoTrack[i];
        SVC_MEDIA_TRACK_GENERAL_INFO_s * const Track = &Video->Info;
        if (Track != DefTrack) {
            if (DefTrack->Fifo == NULL) {
                Track->DTS = DefTrack->DTS;
            } else {
                Rval = SvcFormat_SeekVideo(Video, IsoIdx, TrackId, GetFrameInfo, Direction, TargetDTS);
            }
        }
        if (Rval == FORMAT_OK) {
            SVC_FRAME_FEEDER_INFO_s * const TrackFeedInfo = &FeedInfo[TrackId];
            /* Initial frame feeder */
            IsoDmx_InitFeedVideoInfo(TrackFeedInfo, Video, IsoTrack);
            Rval = SvcFrameFeeder_InitFeedInfo(TrackFeedInfo, Track, TrackId, GetFrameInfo, &feedFrameFP, Track->DTS, Direction, (UINT8) Speed);
        }
        if (Rval != FORMAT_OK) {
            break;
        }
        TrackId++;
    }
    return Rval;
}

static UINT32 IsoDmx_SeekOtherAudio(SVC_MOVIE_INFO_s *Movie, void *IsoIdx, SVC_FRAME_FEEDER_INFO_s *FeedInfo, UINT8 Direction, UINT32 Speed, SVC_FORMAT_GET_FRAME_INFO_FP GetFrameInfo, SVC_FORMAT_FEED_FRAME_FP_s feedFrameFP, const SVC_MEDIA_TRACK_GENERAL_INFO_s *DefTrack)
{
    UINT32 Rval = FORMAT_OK;
    UINT64 TargetDTS = DefTrack->DTS;
    UINT8 TrackId = Movie->VideoTrackCount;
    UINT8 i;
    for (i = 0; i < Movie->AudioTrackCount; i++) {
        SVC_AUDIO_TRACK_INFO_s * const Audio = &Movie->AudioTrack[i];
        SVC_MEDIA_TRACK_GENERAL_INFO_s * const Track = &Audio->Info;
        if (DefTrack->Fifo == NULL) {
            Track->DTS = DefTrack->DTS;
        } else {
            Rval = SvcFormat_SeekAudio(Audio, IsoIdx, TrackId, GetFrameInfo, Direction, TargetDTS);
        }
        if (Rval == FORMAT_OK) {
            SVC_FRAME_FEEDER_INFO_s * const TrackFeedInfo = &FeedInfo[TrackId];
            /* Initial frame feeder */
            Rval = SvcFrameFeeder_InitFeedInfo(TrackFeedInfo, Track, TrackId, GetFrameInfo, &feedFrameFP, Track->DTS, (UINT8) Direction, (UINT8) Speed);
        }
        if (Rval != FORMAT_OK) {
            break;
        }
        TrackId++;
    }
    return Rval;
}

static UINT32 IsoDmx_SeekOtherText(SVC_MOVIE_INFO_s *Movie, void *IsoIdx, SVC_FRAME_FEEDER_INFO_s *FeedInfo, UINT8 Direction, UINT32 Speed, SVC_FORMAT_GET_FRAME_INFO_FP GetFrameInfo, SVC_FORMAT_FEED_FRAME_FP_s feedFrameFP, const SVC_MEDIA_TRACK_GENERAL_INFO_s *DefTrack)
{
    UINT32 Rval = FORMAT_OK;
    UINT64 TargetDTS = DefTrack->DTS;
    UINT8 TrackId = Movie->VideoTrackCount + Movie->AudioTrackCount;
    UINT8 i;
    for (i = 0; i < Movie->TextTrackCount; i++) {
        SVC_TEXT_TRACK_INFO_s * const Text = &Movie->TextTrack[i];
        SVC_MEDIA_TRACK_GENERAL_INFO_s * const Track = &Text->Info;
        if (DefTrack->Fifo == NULL) {
            Track->DTS = DefTrack->DTS;
        } else {
            Rval = SvcFormat_SeekText(Text, IsoIdx, TrackId, GetFrameInfo, Direction, TargetDTS);
        }
        if (Rval == FORMAT_OK) {
            SVC_FRAME_FEEDER_INFO_s * const TrackFeedInfo = &FeedInfo[TrackId];
            /* Initial frame feeder */
            Rval = SvcFrameFeeder_InitFeedInfo(TrackFeedInfo, Track, TrackId, GetFrameInfo, &feedFrameFP, Track->DTS, Direction, (UINT8) Speed);
        }
        if (Rval != FORMAT_OK) {
            break;
        }
        TrackId++;
    }
    return Rval;
}

/**
 *  Seek and set the start time of demuxing.
 *
 *  @param [in] Hdlr Demuxing format handler
 *  @param [in] IsoIdx ISO index
 *  @param [in] FeedInfo Frame feeder information
 *  @param [in] TargetTime Target time to process
 *  @param [in] Direction Direction of process
 *  @param [in] Speed Processing speed after seeking
 *  @param [in] GetFrameInfo Function pointer to get frame information
 *  @return 0 - OK, others - FORMAT_ERR_XXXX
 */
UINT32 IsoDmx_Seek(SVC_DMX_FORMAT_HDLR_s *Hdlr, void *IsoIdx, SVC_FRAME_FEEDER_INFO_s *FeedInfo, UINT64 TargetTime, UINT32 Direction, UINT32 Speed, ISODMX_GET_FRAME_INFO_FP GetFrameInfo)
{
    UINT32 Rval = CheckSeekParam(Hdlr, Direction);
    if (Rval == FORMAT_OK) {
        const SVC_MEDIA_INFO_s * const Media = Hdlr->Media;
        const SVC_MEDIA_TRACK_GENERAL_INFO_s *DefTrack;
        SVC_MOVIE_INFO_s *Movie;
        AmbaMisra_TypeCast(&Movie, &Media);
        Rval = IsoDmx_SeekDefaultVideo(Movie, IsoIdx, TargetTime, (UINT8)Direction, GetFrameInfo, &DefTrack);
        if (Rval == FORMAT_OK) {
            SVC_FORMAT_FEED_FRAME_FP_s feedFrameFP;
            feedFrameFP.FeedVideoFrame = SvcFormat_FeedVideoFrame;
            feedFrameFP.FeedAudioFrame = SvcFormat_FeedAudioFrame;
            feedFrameFP.FeedTextFrame = SvcFormat_FeedTextFrame;
            Rval = IsoDmx_SeekOtherVideo(Movie, IsoIdx, FeedInfo, (UINT8)Direction, Speed, GetFrameInfo, feedFrameFP, DefTrack);
            if (Rval == FORMAT_OK) {
                Rval = IsoDmx_SeekOtherAudio(Movie, IsoIdx, FeedInfo, (UINT8)Direction, Speed, GetFrameInfo, feedFrameFP, DefTrack);
                if (Rval == FORMAT_OK) {
                    Rval = IsoDmx_SeekOtherText(Movie, IsoIdx, FeedInfo, (UINT8)Direction, Speed, GetFrameInfo, feedFrameFP, DefTrack);
                }
            }
        }
        /* When find the format is invalid, set valid bit as false. */
        if ((Rval == FORMAT_ERR_INVALID_FORMAT) || (Rval == FORMAT_ERR_NOT_ENOUGH_BYTES)) {
            Rval = FORMAT_ERR_INVALID_FORMAT;
            Movie->MediaInfo.Valid = 0U;
        }
    }
    AmbaMisra_TouchUnused(Hdlr);
    return Rval;
}

static UINT32 CheckFeedVideoFrameParam(const SVC_MOVIE_INFO_s *Movie, UINT8 TrackId, UINT8 FrameType)
{
    UINT32 Rval = FORMAT_OK;
    const SVC_VIDEO_TRACK_INFO_s *Video = &Movie->VideoTrack[TrackId];   // video track only, so just use TrackId
    const SVC_MEDIA_TRACK_GENERAL_INFO_s *Track = &Video->Info;
    if ((Movie->MediaInfo.Valid == 0U) || (Movie->MediaInfo.MediaType != SVC_MEDIA_INFO_MOVIE)) {
        AmbaPrint_ModulePrintUInt5(SVC_DEMUXER_PRINT_MODULE_ID, "CheckFeedVideoFrameParam: Invalid Media %u or Media type %u", Movie->MediaInfo.Valid, Movie->MediaInfo.MediaType, 0U, 0U, 0U);
        Rval = FORMAT_ERR_GENERAL_ERROR;
    }
    if (TrackId >= Movie->VideoTrackCount) {
        AmbaPrint_ModulePrintUInt5(SVC_DEMUXER_PRINT_MODULE_ID, "CheckFeedVideoFrameParam: incorrect TrackId %u", TrackId, 0U, 0U, 0U, 0U);
        Rval = FORMAT_ERR_GENERAL_ERROR;
    }
    if ((FrameType < SVC_FIFO_TYPE_IDR_FRAME) || (FrameType > SVC_FIFO_TYPE_P_FRAME)) {
        AmbaPrint_ModulePrintUInt5(SVC_DEMUXER_PRINT_MODULE_ID, "CheckFeedVideoFrameParam: incorrect frame type %u", FrameType, 0U, 0U, 0U, 0U);
        Rval = FORMAT_ERR_GENERAL_ERROR;
    }
    if ((Track->MediaId != SVC_FORMAT_MID_AVC) && (Track->MediaId != SVC_FORMAT_MID_HVC)) {
        AmbaPrint_ModulePrintUInt5(SVC_DEMUXER_PRINT_MODULE_ID, "CheckFeedVideoFrameParam incorrect media type %u", Video->Info.MediaId, 0U, 0U, 0U, 0U);
        Rval = FORMAT_ERR_GENERAL_ERROR;
    }
    return Rval;
}

/**
 *  Feeding a frame of the specified time.
 *
 *  @param [in] Hdlr Demuxing format handler
 *  @param [in] IsoIdx ISO index
 *  @param [in] TrackId Track id
 *  @param [in] TargetTime The time of the target frame
 *  @param [in] FrameType Frame type
 *  @param [in] GetFrameInfo Function pointer to get frame information
 *  @return 0 - OK, others - FORMAT_ERR_XXXX
 */
UINT32 IsoDmx_FeedFrame(SVC_DMX_FORMAT_HDLR_s *Hdlr, void *IsoIdx, UINT8 TrackId, UINT64 TargetTime, UINT8 FrameType, ISODMX_GET_FRAME_INFO_FP GetFrameInfo)
{
    UINT32 Rval;
    const SVC_MEDIA_INFO_s * const Media = Hdlr->Media;
    SVC_MOVIE_INFO_s *Movie;
    AmbaMisra_TypeCast(&Movie, &Media);
    Rval = CheckFeedVideoFrameParam(Movie, TrackId, FrameType);
    if (Rval == FORMAT_OK) {
        UINT8 TrackIdx = TrackId;   // video track only, so just use TrackId
        SVC_ISO_PRIV_INFO_s *IsoInfo = &Movie->IsoInfo;
        SVC_VIDEO_TRACK_INFO_s *Video = &Movie->VideoTrack[TrackIdx];
        SVC_MEDIA_TRACK_GENERAL_INFO_s *Track = &Video->Info;
        /* IDR: Feed the frame which's FrameInfo.DTS <= TargetTime */
        Rval = SvcFormat_SeekVideo(Video, IsoIdx, TrackId, GetFrameInfo, SVC_FORMAT_DIR_FORWARD, (Track->InitDTS + SVC_FORMAT_TIME_TO_DTS_FLOOR(TargetTime, Track->TimeScale)));
        if (Rval == FORMAT_OK) {
            /* Non-IDR: Check this search function later */
            if (FrameType != SVC_FIFO_TYPE_IDR_FRAME) {
                UINT32 i;
                SVC_FRAME_INFO_s FrameInfo;
                for (i = Track->FrameNo; i < Track->FrameCount; i++) {
                    Rval = GetFrameInfo(IsoIdx, TrackId, i, SVC_FORMAT_DIR_FORWARD, &FrameInfo);
                    if ((Rval != FORMAT_OK) || (FrameInfo.FrameType == FrameType)) {
                        break;
                    }
                }
                Track->FrameNo = i;
            }
            if (Rval == FORMAT_OK) {
                SVC_ISO_VIDEO_TRACK_INFO_s * const IsoVideo = &IsoInfo->VideoTrack[TrackIdx];
                SVC_FEED_FRAME_PARAM_s FeedParam;
                if (Track->MediaId == SVC_FORMAT_MID_AVC) {
                    FeedParam.SPS = IsoVideo->Avc.SPS;
                    FeedParam.PPS = IsoVideo->Avc.PPS;
                    FeedParam.SPSLen = IsoVideo->Avc.SPSLen;
                    FeedParam.PPSLen = IsoVideo->Avc.PPSLen;
                } else {
                    FeedParam.VPS = IsoVideo->Hvc.VPS;
                    FeedParam.SPS = IsoVideo->Hvc.SPS;
                    FeedParam.PPS = IsoVideo->Hvc.PPS;
                    FeedParam.VPSLen = IsoVideo->Hvc.VPSLen;
                    FeedParam.SPSLen = IsoVideo->Hvc.SPSLen;
                    FeedParam.PPSLen = IsoVideo->Hvc.PPSLen;
                }
                Rval = SvcFormat_FeedVideoFrame(Video, Hdlr->Stream, &FeedParam, IsoIdx, TrackId, GetFrameInfo, SVC_FORMAT_DIR_FORWARD, Track->FrameNo);
            }
        } else {
            AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s SvcFormat_SeekVideo failed", __func__, NULL, NULL, NULL, NULL);
        }
    } else {
        AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s CheckFeedVideoFrameParam failed", __func__, NULL, NULL, NULL, NULL);
    }
    AmbaMisra_TouchUnused(Hdlr);

    /* When find the format is invalid, set valid bit as false. */
    if ((Rval == FORMAT_ERR_INVALID_FORMAT) || (Rval == FORMAT_ERR_NOT_ENOUGH_BYTES)) {
        Movie->MediaInfo.Valid = 0U;
        Rval = FORMAT_ERR_INVALID_FORMAT;
    }
    return Rval;
}

