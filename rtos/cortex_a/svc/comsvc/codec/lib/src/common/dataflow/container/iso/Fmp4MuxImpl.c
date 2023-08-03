/**
 * @file Fmp4MuxImpl.c
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
#include "Fmp4MuxImpl.h"
#include "../FormatAPI.h"
#include "../ByteOp.h"
#include "../MemByteOp.h"
#include "../H264.h"
#include "../H265.h"
#include <AmbaRTC.h>


static UINT32 Fmp4Mux_MVEX_Write(const ISO_MVEX_BOX_s *Mvex, SVC_STREAM_HDLR_s *Stream, UINT8 TrackCount, SVC_MEDIA_INFO_s *Media)
{
    UINT32 Rval = SvcFormat_PutBe32(Stream, Mvex->Box.Size);
    if (Rval == FORMAT_OK) {
        Rval = SvcFormat_PutBe32(Stream, Mvex->Box.Type);
        if (Rval == FORMAT_OK) {
            if (Mvex->EnableMehd > 0U) {
                SVC_MOVIE_INFO_s *TmpMovie;
                AmbaMisra_TypeCast(&TmpMovie, &Media);

                Rval = S2F(Stream->Func->GetPos(Stream, &(TmpMovie->MehdPos)));
                TmpMovie->MehdSize = Mvex->MehdAtom.FullBox.Box.Size;

                if (Rval == FORMAT_OK) {
                    Rval = SvcFormat_PutBe32(Stream, Mvex->MehdAtom.FullBox.Box.Size);
                }
                if (Rval == FORMAT_OK) {
                    Rval = SvcFormat_PutBe32(Stream, Mvex->MehdAtom.FullBox.Box.Type);
                }
                if (Rval == FORMAT_OK) {
                    Rval = SvcFormat_PutByte(Stream, Mvex->MehdAtom.FullBox.Version);
                }
                if (Rval == FORMAT_OK) {
                    Rval = SvcFormat_PutByte(Stream, Mvex->MehdAtom.FullBox.Flags[0]);
                }
                if (Rval == FORMAT_OK) {
                    Rval = SvcFormat_PutByte(Stream, Mvex->MehdAtom.FullBox.Flags[1]);
                }
                if (Rval == FORMAT_OK) {
                    Rval = SvcFormat_PutByte(Stream, Mvex->MehdAtom.FullBox.Flags[2]);
                }
                if (Rval == FORMAT_OK) {
                    Rval = SvcFormat_PutBe32(Stream, Mvex->MehdAtom.FragmentDuration);
                }
            }
        }
        if (Rval == FORMAT_OK) {
            UINT8 i;
            for (i = 0; i < TrackCount; i++) {
                Rval = SvcFormat_PutBe32(Stream, Mvex->TrexAtom[i].FullBox.Box.Size);
                if (Rval == FORMAT_OK) {
                    Rval = SvcFormat_PutBe32(Stream, Mvex->TrexAtom[i].FullBox.Box.Type);
                }
                if (Rval == FORMAT_OK) {
                    Rval = SvcFormat_PutBe32(Stream, 0);
                }
                if (Rval == FORMAT_OK) {
                    Rval = SvcFormat_PutBe32(Stream, Mvex->TrexAtom[i].TrackId);
                }
                if (Rval == FORMAT_OK) {
                    Rval = SvcFormat_PutBe32(Stream, Mvex->TrexAtom[i].DefaultSampleDescriptionIndex);
                }
                if (Rval == FORMAT_OK) {
                    Rval = SvcFormat_PutBe32(Stream, Mvex->TrexAtom[i].DefaultSampleDuration);
                }
                if (Rval == FORMAT_OK) {
                    Rval = SvcFormat_PutBe32(Stream, Mvex->TrexAtom[i].DefaultSampleSize);
                }
                if (Rval == FORMAT_OK) {
                    Rval = SvcFormat_PutBe32(Stream, Mvex->TrexAtom[i].DefaultSampleFlags);
                }
                if (Rval != FORMAT_OK) {
                    break;
                }
            }
        }
    }
    return Rval;
}

static UINT32 Fmp4Mux_MVEX_Init(ISO_MVEX_BOX_s *Mvex, const SVC_MEDIA_INFO_s *Media)
{
    UINT32                               i, Count = 0;
    const SVC_MEDIA_TRACK_GENERAL_INFO_s *Track;
    const SVC_MOVIE_INFO_s               *Movie;
    ISO_TREX_BOX_s                       *TrexAtom;
    ISO_MEHD_BOX_s                       *MehdAtom;

    AmbaMisra_TypeCast(&Movie, &Media);
    Mvex->Box.Size = 8;
    Mvex->Box.Type = TAG_MVEX;

    /* Initial MEHD BOX */
    if (Mvex->EnableMehd > 0U) {
        MehdAtom = &Mvex->MehdAtom;

        MehdAtom->FullBox.Box.Size     = 16U;
        MehdAtom->FullBox.Box.Type     = TAG_MEHD;
        MehdAtom->FullBox.Version      = 0U;
        MehdAtom->FullBox.Flags[0]     = 0U;
        MehdAtom->FullBox.Flags[1]     = 0U;
        MehdAtom->FullBox.Flags[2]     = 0U;

        Mvex->Box.Size += MehdAtom->FullBox.Box.Size;
    }

    /* Initial TREX BOX*/
    for (i = 0; i < Movie->VideoTrackCount; i++) {
        Track = &Movie->VideoTrack[i].Info;
        TrexAtom = &Mvex->TrexAtom[Count];
        TrexAtom->FullBox.Box.Size = 32;
        TrexAtom->FullBox.Box.Type = TAG_TREX;
        TrexAtom->TrackId = Count + 1U;
        TrexAtom->DefaultSampleDescriptionIndex = 1;
        TrexAtom->DefaultSampleDuration = (UINT32)NORMALIZE_TO_TIMESCALE(Track->TimeScale, Track->TimePerFrame, Track->OrigTimeScale);
        Mvex->Box.Size += TrexAtom->FullBox.Box.Size;
        Count++;
    }
    for (i = 0 ; i < Movie->AudioTrackCount; i++) {
        Track = &Movie->AudioTrack[i].Info;
        TrexAtom = &Mvex->TrexAtom[Count];
        TrexAtom->FullBox.Box.Size = 32;
        TrexAtom->FullBox.Box.Type = TAG_TREX;
        TrexAtom->TrackId = Count + 1U;
        TrexAtom->DefaultSampleDescriptionIndex = 1;
        TrexAtom->DefaultSampleDuration = (UINT32)NORMALIZE_TO_TIMESCALE(Track->TimeScale, Track->TimePerFrame, Track->OrigTimeScale);
        Mvex->Box.Size += TrexAtom->FullBox.Box.Size;
        Count++;
    }
    for (i = 0 ; i < Movie->TextTrackCount; i++) {
        Track = &Movie->TextTrack[i].Info;
        TrexAtom = &Mvex->TrexAtom[Count];
        TrexAtom->FullBox.Box.Size = 32;
        TrexAtom->FullBox.Box.Type = TAG_TREX;
        TrexAtom->TrackId = Count + 1U;
        TrexAtom->DefaultSampleDescriptionIndex = 1;
        TrexAtom->DefaultSampleDuration = (UINT32)NORMALIZE_TO_TIMESCALE(Track->TimeScale, Track->TimePerFrame, Track->OrigTimeScale);
        Mvex->Box.Size += TrexAtom->FullBox.Box.Size;
        Count++;
    }
    return FORMAT_OK;
}

static UINT32 Fmp4Mux_MOOV_Write(ISO_MOOV_BOX_s *Moov, SVC_STREAM_HDLR_s *Stream, SVC_MEDIA_INFO_s *Media)
{
    UINT32 Rval = SvcFormat_PutBe32(Stream, Moov->Box.Size);
    if (Rval == FORMAT_OK) {
        Rval = SvcFormat_PutBe32(Stream, Moov->Box.Type);
        if (Rval == FORMAT_OK) {
            /* Write MVHD BOX*/
            Rval = IsoMux_MVHD_Write(&Moov->MvhdAtom, Stream);
            if (Rval == FORMAT_OK) {
                /* Write MVEX BOX*/
                Rval = Fmp4Mux_MVEX_Write(&Moov->MvexAtom, Stream, Moov->TrackCount, Media);
                if (Rval == FORMAT_OK) {
                    /* Write UDTA BOX*/
                    Rval = IsoMux_UDTA_Write(&Moov->UdtaAtom, Stream, Media);
                    if (Rval == FORMAT_OK) {
                        UINT8 i;
                        SVC_MOVIE_INFO_s *Movie = NULL;
                        UINT8 Count = 0;
                        ISO_TRACK_BOX_s *Trak;
                        /* Write TRAK BOX*/
                        AmbaMisra_TypeCast(&Movie, &Media);
                        for (i = 0; i < Movie->VideoTrackCount; i++){
                            const SVC_VIDEO_TRACK_INFO_s *VideoTrack = &Movie->VideoTrack[i];
                            Trak = &Moov->TrackAtom[Count];
                            Count++;
                            Rval = IsoMux_VideoTRAK_Write(Trak, VideoTrack, Stream);
                            if (Rval != FORMAT_OK) {
                                break;
                            }
                        }
                        if (Rval == FORMAT_OK) {
                            for (i = 0; i < Movie->AudioTrackCount; i++){
                                const SVC_AUDIO_TRACK_INFO_s *AudioTrack = &Movie->AudioTrack[i];
                                Trak = &Moov->TrackAtom[Count];
                                Count++;
                                Rval = IsoMux_AudioTRAK_Write(Trak, AudioTrack, Stream);
                                if (Rval != FORMAT_OK) {
                                    break;
                                }
                            }
                        }
                        if (Rval == FORMAT_OK) {
                            for (i = 0; i < Movie->TextTrackCount; i++){
                                SVC_TEXT_TRACK_INFO_s *TextTrack = &Movie->TextTrack[i];
                                Trak = &Moov->TrackAtom[Count];
                                Count++;
                                Rval = IsoMux_TextTRAK_Write(Trak, TextTrack, Stream);
                                if (Rval != FORMAT_OK) {
                                    break;
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    return Rval;
}

static UINT32 Fmp4Mux_MOOV_Init(ISO_MOOV_BOX_s *Moov, SVC_MEDIA_INFO_s *Media)
{
    UINT32 Rval;
    const SVC_MOVIE_INFO_s *Movie;

    AmbaMisra_TypeCast(&Movie, &Media);
    Moov->Box.Size = 8;
    Moov->Box.Type = TAG_MOOV;
    Moov->TrackCount = Movie->VideoTrackCount + Movie->AudioTrackCount + Movie->TextTrackCount;
    /* Initial MVHD BOX*/
    Rval = IsoMux_MVHD_Init(&Moov->MvhdAtom, Media, NULL);
    if (Rval == FORMAT_OK) {
        Moov->Box.Size += Moov->MvhdAtom.FullBox.Box.Size;
        /* Initial MVEX BOX*/
        Rval = Fmp4Mux_MVEX_Init(&Moov->MvexAtom, Media);
        if (Rval == FORMAT_OK) {
            Moov->Box.Size += Moov->MvexAtom.Box.Size;
            /* Initial UDTA BOX*/
            Rval = IsoMux_UDTA_Init(&Moov->UdtaAtom, Media);
            if (Rval == FORMAT_OK) {
                UINT8 i;
                UINT8 Count = 0;
                const SVC_ISO_PRIV_INFO_s * const IsoInfo = &Movie->IsoInfo;
                const SVC_VIDEO_TRACK_INFO_s *VideoTrack;
                const SVC_AUDIO_TRACK_INFO_s *AudioTrack;
                const SVC_TEXT_TRACK_INFO_s  *TextTrack;
                ISO_TRACK_BOX_s *Trak;
                Moov->Box.Size += Moov->UdtaAtom.Box.Size;
                for (i = 0; i < Movie->VideoTrackCount; i++) {
                    VideoTrack = &Movie->VideoTrack[i];
                    Trak = &Moov->TrackAtom[Count];
                    Count++;
                    Rval = IsoMux_VideoTRAK_Init(Trak, VideoTrack, i, Media, IsoInfo, 1U);
                    if (Rval != FORMAT_OK) {
                        break;
                    }
                    Moov->Box.Size += Trak->Box.Size;
                }
                for (i = 0; i < Movie->AudioTrackCount; i++) {
                    AudioTrack = &Movie->AudioTrack[i];
                    Trak = &Moov->TrackAtom[Count];
                    Count++;
                    Rval = IsoMux_AudioTRAK_Init(Trak, AudioTrack, i, Media, IsoInfo, 1U);
                    if (Rval != FORMAT_OK) {
                        break;
                    }
                    Moov->Box.Size += Trak->Box.Size;
                }
                for (i = 0; i < Movie->TextTrackCount; i++) {
                    TextTrack = &Movie->TextTrack[i];
                    Trak = &Moov->TrackAtom[Count];
                    Count++;
                    Rval = IsoMux_TextTRAK_Init(Trak, TextTrack, i, Media, IsoInfo, 1U);
                    if (Rval != FORMAT_OK) {
                        break;
                    }
                    Moov->Box.Size += Trak->Box.Size;
                }
            }
        }
    }
    return Rval;
}

static UINT32 Fmp4Mux_PutMoov(SVC_MEDIA_INFO_s *Media, SVC_STREAM_HDLR_s *Stream, const void *MoovAtom)
{
    UINT32 Rval;
    ISO_MOOV_BOX_s *Moov;
    AmbaMisra_TypeCast(&Moov, &MoovAtom);
    Rval = Fmp4Mux_MOOV_Init(Moov, Media);
    if (Rval == FORMAT_OK) {
        Rval = Fmp4Mux_MOOV_Write(Moov, Stream, Media);
    }
    return Rval;
}

static UINT32 Fmp4Mux_TFHD_Write(const ISO_TFHD_BOX_s *Tfhd, SVC_STREAM_HDLR_s *Stream)
{
    UINT32 Rval = SvcFormat_PutBe32(Stream, Tfhd->FullBox.Box.Size);
    if (Rval == FORMAT_OK) {
        Rval = SvcFormat_PutBe32(Stream, Tfhd->FullBox.Box.Type);
    }
    if (Rval == FORMAT_OK) {
        Rval = SvcFormat_PutByte(Stream, Tfhd->FullBox.Version);
    }
    if (Rval == FORMAT_OK) {
        Rval = SvcFormat_PutByte(Stream, Tfhd->FullBox.Flags[0]);
    }
    if (Rval == FORMAT_OK) {
        Rval = SvcFormat_PutByte(Stream, Tfhd->FullBox.Flags[1]);
    }
    if (Rval == FORMAT_OK) {
        Rval = SvcFormat_PutByte(Stream, Tfhd->FullBox.Flags[2]);
    }
    if (Rval == FORMAT_OK) {
        Rval = SvcFormat_PutBe32(Stream, Tfhd->TrackId);
    }
    if (Rval == FORMAT_OK) {
        Rval = SvcFormat_PutBe64(Stream, Tfhd->BaseDataOffset);
    }
    if (Rval == FORMAT_OK) {
        Rval = SvcFormat_PutBe32(Stream, Tfhd->DefaultSampleDuration);
    }
    if (Rval == FORMAT_OK) {
        Rval = SvcFormat_PutBe32(Stream, Tfhd->DefaultSampleSize);
    }
    if (Rval == FORMAT_OK) {
        Rval = SvcFormat_PutBe32(Stream, Tfhd->DefaultSampleFlags);
    }
    return Rval;
}

static UINT32 Fmp4Mux_TFDT_Write(const ISO_TFDT_BOX_s *Tfdt, SVC_STREAM_HDLR_s *Stream)
{
    UINT32 Rval = SvcFormat_PutBe32(Stream, Tfdt->FullBox.Box.Size);
    if (Rval == FORMAT_OK) {
        Rval = SvcFormat_PutBe32(Stream, Tfdt->FullBox.Box.Type);
    }
    if (Rval == FORMAT_OK) {
        Rval = SvcFormat_PutByte(Stream, Tfdt->FullBox.Version);
    }
    if (Rval == FORMAT_OK) {
        Rval = SvcFormat_PutByte(Stream, Tfdt->FullBox.Flags[0]);
    }
    if (Rval == FORMAT_OK) {
        Rval = SvcFormat_PutByte(Stream, Tfdt->FullBox.Flags[1]);
    }
    if (Rval == FORMAT_OK) {
        Rval = SvcFormat_PutByte(Stream, Tfdt->FullBox.Flags[2]);
    }
    if (Rval == FORMAT_OK) {
        Rval = SvcFormat_PutBe32(Stream, Tfdt->BaseMediaDecodeTime);
    }
    return Rval;
}

static UINT32 Fmp4Mux_TRUN_Write(const ISO_TRUN_BOX_s *Trun, SVC_STREAM_HDLR_s *Stream, UINT32 SampleCount)
{
    UINT32 Rval = SvcFormat_PutBe32(Stream, Trun->FullBox.Box.Size);
    if (Rval == FORMAT_OK) {
        Rval = SvcFormat_PutBe32(Stream, Trun->FullBox.Box.Type);
    }
    if (Rval == FORMAT_OK) {
        Rval = SvcFormat_PutByte(Stream, Trun->FullBox.Version);
    }
    if (Rval == FORMAT_OK) {
        Rval = SvcFormat_PutByte(Stream, Trun->FullBox.Flags[0]);
    }
    if (Rval == FORMAT_OK) {
        Rval = SvcFormat_PutByte(Stream, Trun->FullBox.Flags[1]);
    }
    if (Rval == FORMAT_OK) {
        Rval = SvcFormat_PutByte(Stream, Trun->FullBox.Flags[2]);
    }
    if (Rval == FORMAT_OK) {
        /* refer to ffmpeg. Let one trun includes many samples. */
        Rval = SvcFormat_PutBe32(Stream, Trun->SampleCount);
    }
    if (Rval == FORMAT_OK) {
        Rval = SvcFormat_PutBe32(Stream, Trun->DataOffset);
    }
    if (Rval == FORMAT_OK) {
        if (Trun->FirstSampleFlags != 0U) {
            Rval = SvcFormat_PutBe32(Stream, Trun->FirstSampleFlags);
        }
    }
    if (Rval == FORMAT_OK) {
        UINT32 i;//, Count = 0;
        ISO_TRUN_SAMPLE_s const *Sample;
        for (i = 0; i < SampleCount; i++) {
            Sample = &Trun->Samples[i];
            if ((Trun->FullBox.Flags[1] & 0x01U) != 0U) {
                Rval = SvcFormat_PutBe32(Stream, Sample->SampleDuration);
            }
            if (Rval == FORMAT_OK) {
                Rval = SvcFormat_PutBe32(Stream, Sample->SampleSize);
            }
            if (Rval == FORMAT_OK) {
                if ((Trun->FullBox.Flags[1] & 0x08U) != 0U) {
                    Rval = SvcFormat_PutBe32(Stream, Sample->SampleCompositionTimeOffset);
                }
            }
            if (Rval != FORMAT_OK) {
                break;
            }
            //AmbaPrint_ModulePrintUInt5(SVC_MUXER_PRINT_MODULE_ID, "The %u-th Frame CTS=%u, size=%u, DataOffset=%u", i, Sample->SampleCompositionTimeOffset, Sample->SampleSize, Trun->DataOffset, 0U);
            //Count += Trun->Samples[i].SampleSize;
        }
    }
    return Rval;
}

static UINT32 Fmp4Mux_TRAF_Write(const ISO_TRAF_BOX_s *Traf, SVC_STREAM_HDLR_s *Stream)
{
    UINT32 Rval = SvcFormat_PutBe32(Stream, Traf->Box.Size);
    if (Rval == FORMAT_OK) {
        Rval = SvcFormat_PutBe32(Stream, Traf->Box.Type);
    }
    if (Rval == FORMAT_OK) {
        Rval = Fmp4Mux_TFHD_Write(&Traf->TfhdAtom, Stream);
    }
    if (Rval == FORMAT_OK) {
        /* refer to ffmpeg and add tfdt box. */
        Rval = Fmp4Mux_TFDT_Write(&Traf->TfdtAtom, Stream);
    }
    if (Rval == FORMAT_OK) {
        Rval = Fmp4Mux_TRUN_Write(&Traf->TrunAtom, Stream, Traf->SampleCount);
    }
    return Rval;
}

static UINT32 Fmp4Mux_MOOF_Write(const ISO_MOOF_BOX_s *Moof, SVC_STREAM_HDLR_s *Stream, UINT8 TrackCount)
{
    UINT32 Rval = SvcFormat_PutBe32(Stream, Moof->Box.Size);
    if (Rval == FORMAT_OK) {
        Rval = SvcFormat_PutBe32(Stream, Moof->Box.Type);
        if (Rval == FORMAT_OK) {
            Rval = SvcFormat_PutBe32(Stream, Moof->MfhdAtom.FullBox.Box.Size);
            if (Rval == FORMAT_OK) {
                Rval = SvcFormat_PutBe32(Stream, Moof->MfhdAtom.FullBox.Box.Type);
                if (Rval == FORMAT_OK) {
                    Rval = SvcFormat_PutBe32(Stream, 0);
                    if (Rval == FORMAT_OK) {
                        Rval = SvcFormat_PutBe32(Stream, Moof->MfhdAtom.SequenceNum);
                        if (Rval == FORMAT_OK) {
                            UINT8 i;
                            for (i = 0; i < TrackCount; i++) {
                                //AmbaPrint_ModulePrintUInt5(SVC_MUXER_PRINT_MODULE_ID, "MOOF[%u] : track[%u], decode_time=%u, data_offset=%u, duration=%u", Moof->MfhdAtom.SequenceNum, i, Moof->TrafAtom[i].TfdtAtom.BaseMediaDecodeTime, (UINT32)Moof->TrafAtom[i].TfhdAtom.BaseDataOffset, Moof->TrafAtom[i].TfhdAtom.DefaultSampleDuration);
                                Rval = Fmp4Mux_TRAF_Write(&Moof->TrafAtom[i], Stream);
                                if (Rval != FORMAT_OK) {
                                    break;
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    return Rval;
}

static UINT32 GetCTTS(ISO_TRUN_SAMPLE_s const *IP, UINT16 M, UINT32 Idx)
{
    // frame:     I/P,  B0, B1
    // DTS  =       0,   I, I+B0
    // PTS  =   B0+B1,   0, B0
    // CT   = B0+B1+I, 0+I, B0+I
    // CTTS = B0+B1+I,   0, 0
    // only need to deal with I or P
    UINT32 Rval = 0;
    if (Idx == 0U) {
        // I/P frame, CTTS is the sum of I/P/B frames' durations
        UINT32 i;
        for (i = 0; i < M; i++) {
            Rval += IP[i].SampleDuration;
        }
    }
    return Rval;
}

static void Fmp4Mux_MOOF_Init(ISO_MOOF_BOX_s *Moof, const FRAGMENT_QUEUE_s *QueFragment, UINT32 SeqNum, UINT8 TrackCount, const UINT32 *PopSampleCount)
{
    UINT8 i;
    ISO_TRAF_BOX_s *TrafAtom;
    FRAGMENT_TRACK_s const *Traf;
    //Fmp4_Print("%s(%u, %u) : %u %u %u", __func__, SeqNum, TrackCount, QueFragment->Traf[0].SampleCount, (TrackCount > 0) ? QueFragment->Traf[1].SampleCount : 0, (TrackCount > 1) ? QueFragment->Traf[2].SampleCount : 0);
    Moof->Box.Size = 8U;
    Moof->Box.Type = TAG_MOOF;
    /* Initial MFHD BOX*/
    Moof->MfhdAtom.FullBox.Box.Size = 16U;
    Moof->MfhdAtom.FullBox.Box.Type = TAG_MFHD;
    Moof->MfhdAtom.SequenceNum = SeqNum + 1U;    /* start at 1. */
    Moof->Box.Size += Moof->MfhdAtom.FullBox.Box.Size;
    /* Initial TRAF BOX*/
    for (i = 0U; i < TrackCount; i++) {
        UINT32 j;
        Traf = &QueFragment->Traf[i];
        TrafAtom = &Moof->TrafAtom[i];
        TrafAtom->Box.Size = 8U;
        TrafAtom->Box.Type = TAG_TRAF;
        /* Initial TFHD BOX, flag = 0x0000001, BaseDataOffset is present*/
        TrafAtom->TfhdAtom.FullBox.Box.Size = 36U;
        TrafAtom->TfhdAtom.FullBox.Box.Type = TAG_TFHD;
        TrafAtom->TfhdAtom.FullBox.Version = 0U;
        /* defalut-sample-flags-present is on,
           default-sample-size-present is on,
           default-sampel-duration-present is on,
           base-data-offset-present is on.
        */
        TrafAtom->TfhdAtom.FullBox.Flags[2] = 0x39U;
        TrafAtom->TfhdAtom.TrackId = (UINT32)i + 1U;
        TrafAtom->TfhdAtom.BaseDataOffset = QueFragment->MoofOffset - SVC_ISO_BOX_SIZE_TAG_SIZE;
        TrafAtom->TfhdAtom.DefaultSampleDuration = Traf->SampleDuration;
        if (Traf->TrackType == SVC_MEDIA_TRACK_TYPE_VIDEO) {
            /* refer to FFMPEG
             * sample depends on others.
             * sample is difference sample.
             */
            TrafAtom->TfhdAtom.DefaultSampleFlags = 0x01010000U;
        } else {
            /* refer to FFMPEG
             * sample depends does not depends on others. */
            TrafAtom->TfhdAtom.DefaultSampleFlags = 0x02000000U;
        }
        TrafAtom->TfhdAtom.DefaultSampleSize = Traf->SampleSize[0];
        TrafAtom->SampleCount = Traf->SampleCount;
        TrafAtom->Box.Size += TrafAtom->TfhdAtom.FullBox.Box.Size;

        /* tfdt box*/
        TrafAtom->TfdtAtom.FullBox.Box.Size = 16U;
        TrafAtom->TfdtAtom.FullBox.Box.Type = TAG_TFDT;
        TrafAtom->TfdtAtom.FullBox.Version = 0U;
        /* The sample count is the total sample counts of (n-1) run. */
        if (Traf->AdjustedSampleCount == 0U) {
            // TrafAtom->TfdtAtom.BaseMediaDecodeTime = PopSampleCount[i] * TrafAtom->TfhdAtom.DefaultSampleDuration;

            /* the tracks decode time may not be 0 after splitting, so use BaseMediaDecodeTime to record the correct decode time */
            TrafAtom->TfdtAtom.BaseMediaDecodeTime = (UINT32)Traf->BaseMediaDecodeTime;
            AmbaMisra_TouchUnused(&PopSampleCount);
        } else {
            TrafAtom->TfdtAtom.BaseMediaDecodeTime = (UINT32)Traf->BaseMediaDecodeTime;
        }
        TrafAtom->Box.Size += TrafAtom->TfdtAtom.FullBox.Box.Size;
        /* Initial TRUN BOX, flag = 0x000201, DataOffset & SampleSize are present*/
        /* 0x000001: data-offset-present, 0x000100: sample-duration-present, 0x000200: sample-size-present */
        TrafAtom->TrunAtom.FullBox.Box.Size = 20U;
        TrafAtom->TrunAtom.FullBox.Box.Type = TAG_TRUN;
        TrafAtom->TrunAtom.FullBox.Version = 0U;
        if (Traf->AdjustedSampleCount == 0U) {
            TrafAtom->TrunAtom.FullBox.Flags[1] = 2U;
        } else {
            TrafAtom->TrunAtom.FullBox.Flags[1] = 3U;
        }
        /*video track*/
        if (Traf->TrackType == SVC_MEDIA_TRACK_TYPE_VIDEO) {
            if (Traf->M > 1U) {
                // has B frame, enable sample_composition_time_offsets_present
                TrafAtom->TrunAtom.FullBox.Flags[1] |= 0x08U;
            }
            TrafAtom->TrunAtom.FullBox.Flags[2] = 5U;
            TrafAtom->TrunAtom.FirstSampleFlags = 0x2000000U;
            TrafAtom->TrunAtom.FullBox.Box.Size += 4U; /*first sample flag*/
        } else {
            TrafAtom->TrunAtom.FullBox.Flags[2] = 1U;
            TrafAtom->TrunAtom.FirstSampleFlags = 0U;
        }
        TrafAtom->TrunAtom.SampleCount = TrafAtom->SampleCount;
        for (j = 0; j < TrafAtom->SampleCount; j++) {
            // always fill sample duration to TRUN atom, could be used for calculating CTS
            if (Traf->AdjustedSampleCount != 0U) {
                // adjust, add box size
                TrafAtom->TrunAtom.Samples[j].SampleDuration = Traf->AdjustedSampleDuration[j];
                TrafAtom->TrunAtom.FullBox.Box.Size += 4U;
            } else {
                // no adjust, just set to Traf->SampleDuration for CTS calculation
                TrafAtom->TrunAtom.Samples[j].SampleDuration = Traf->SampleDuration;
            }
        }
        for (j = 0; j < TrafAtom->SampleCount; j++) {
            ISO_TRUN_SAMPLE_s *SampleAtom = &TrafAtom->TrunAtom.Samples[j];
            SampleAtom->SampleSize = Traf->SampleSize[j];
            TrafAtom->TrunAtom.FullBox.Box.Size += 4U;
            if ((TrafAtom->TrunAtom.FullBox.Flags[1] & 0x08U) != 0U) {
                if ((TrafAtom->SampleCount % Traf->M) != 0U) {
                    // must be the 1st GOP in CloseGOP mode
                    UINT32 Idx = 0;
                    if (j == 0U) {
                        // IDR
                        SampleAtom->SampleCompositionTimeOffset = TrafAtom->TrunAtom.Samples[j].SampleDuration;
                    } else {
                        //UINT32 Idx = (j - 1) % Traf->M;
                        Idx = (j - 1U) % Traf->M;
                        SampleAtom->SampleCompositionTimeOffset = GetCTTS(&TrafAtom->TrunAtom.Samples[j - Idx], Traf->M, Idx);
                    }
                } else {
                    UINT32 Idx = j % Traf->M;
                    SampleAtom->SampleCompositionTimeOffset = GetCTTS(&TrafAtom->TrunAtom.Samples[j - Idx], Traf->M, Idx);
                }
                TrafAtom->TrunAtom.FullBox.Box.Size += 4U;
            }
        }
        TrafAtom->Box.Size += TrafAtom->TrunAtom.FullBox.Box.Size;
        Moof->Box.Size += TrafAtom->Box.Size;
    }
    // add MOOF size to DataOffset
    for (i = 0; i < TrackCount; i++) {
        TrafAtom = &Moof->TrafAtom[i];
        Traf = &QueFragment->Traf[i];
        TrafAtom->TrunAtom.DataOffset = Traf->SampleDataOffset + Moof->Box.Size + SVC_ISO_BOX_SIZE_TAG_SIZE;
    }
}

static UINT32 Fmp4Mux_PutMoof(const FRAGMENT_QUEUE_s *QueFragment, SVC_STREAM_HDLR_s *Stream, UINT8 TrackCount, const UINT32 *PopSampleCount)
{
    ISO_MOOF_BOX_s Moof = {0};
    Fmp4Mux_MOOF_Init(&Moof, QueFragment, QueFragment->SequenceNum, TrackCount, PopSampleCount);
    return Fmp4Mux_MOOF_Write(&Moof, Stream, TrackCount);
}

static UINT32 Fmp4Mux_PutHead(SVC_MEDIA_INFO_s *Media, SVC_STREAM_HDLR_s *Stream, const ISO_MOOV_BOX_s *MoovAtom)
{
    /* Put Ftyp */
    UINT32 Rval = IsoMux_MP4_PutFtyp(Media, Stream);
    if (Rval == FORMAT_OK) {
        /* Put Moov */
        Rval = Fmp4Mux_PutMoov(Media, Stream, MoovAtom);
#if 0
        if (Rval == FORMAT_OK) {
            UINT64 MoovEnd;
            Rval = S2F(Stream->Func->GetPos(Stream, &MoovEnd));
            if (Rval != FORMAT_OK) {
                AmbaPrint_ModulePrintStr5(SVC_MUXER_PRINT_MODULE_ID, "%s Stream->Func->GetPos() failed!", __func__, NULL, NULL, NULL, NULL);
            }
        }
#endif
    }
    return Rval;
}

static UINT32 GetOffset(const void *Base, const void *Addr)
{
    ULONG AddrUL, BaseUL, OffsetUL;
    AmbaMisra_TypeCast(&AddrUL, &Addr);
    AmbaMisra_TypeCast(&BaseUL, &Base);
    OffsetUL = AddrUL - BaseUL;
    return (UINT32)OffsetUL;
}

static UINT32 Fmp4Mux_WriteVideoFrame(const SVC_VIDEO_TRACK_INFO_s *Video, SVC_STREAM_HDLR_s *Stream, const SVC_FIFO_BITS_DESC_s *Frame, UINT32 *WriteSize)
{
    UINT32 Rval;
    const SVC_MEDIA_TRACK_GENERAL_INFO_s *Track = &Video->Info;
    UINT8 *BufferBase = Track->BufferBase;
    UINT32 AddrOffset = GetOffset(BufferBase, Frame->StartAddr);
    if (Track->MediaId == SVC_FORMAT_MID_AVC) {
        Rval = SvcH264_WriteStream(Stream, Frame->Size, AddrOffset, BufferBase, Track->BufferSize, WriteSize, SVC_H264_ALIGN_SIZE);
    } else if (Track->MediaId == SVC_FORMAT_MID_HVC) {
        Rval = SvcH265_WriteStream(Stream, Frame->Size, AddrOffset, BufferBase, Track->BufferSize, WriteSize, 1);
    } else {
        AmbaPrint_ModulePrintStr5(SVC_MUXER_PRINT_MODULE_ID, "%s incorrect media ID!", __func__, NULL, NULL, NULL, NULL);
        Rval = FORMAT_ERR_GENERAL_ERROR;
    }
    return Rval;
}

static UINT32 Fmp4Mux_GetMoofSize(const FRAGMENT_QUEUE_s *QueFragment, UINT32 SeqNum, UINT8 TrackCount, const UINT32 *PopSampleCount)
{
    ISO_MOOF_BOX_s Moof = {0};
    Fmp4Mux_MOOF_Init(&Moof, QueFragment, SeqNum, TrackCount, PopSampleCount);
    //Fmp4_Print("%s(%u, %u) : %u", __func__, SeqNum, TrackCount, Moof.Box.Size);
    return Moof.Box.Size;
}

static UINT32 Fmp4MuxQueue_Push(FRAGMENT_INFO_s *Fmp4Mux)
{
    UINT8 TrackCount = Fmp4Mux->VideoTrackCount + Fmp4Mux->AudioTrackCount + Fmp4Mux->TextTrackCount;
    FRAGMENT_QUEUE_s * const Que = &Fmp4Mux->QueFragment;
    UINT32 Rval = AmbaWrap_memcpy(Que->Traf, Fmp4Mux->Traf, (UINT32)sizeof(FRAGMENT_TRACK_s) * SVC_FORMAT_MAX_TRACK_PER_MEDIA);
    if (Rval == FORMAT_OK) {
        Que->SequenceNum = Fmp4Mux->SequenceNum;
        Que->MoofOffset = Fmp4Mux->BaseDataOffset;
        Que->MdatSize = Fmp4Mux->MdatSize;
        Que->MdatOffset = Que->MoofOffset + Fmp4Mux_GetMoofSize(Que, Que->SequenceNum, TrackCount, Fmp4Mux->LastPopSampleCount);
        Fmp4Mux->BaseDataOffset = Que->MdatOffset + Que->MdatSize + SVC_ISO_BOX_SIZE_TAG_SIZE;
        //AmbaPrint_ModulePrintUInt5(SVC_MUXER_PRINT_MODULE_ID, "Fmp4MuxQueue_Push() : seq=%u offset=%u, size=%u BaseDataOffset=%u", Que->SequenceNum, (UINT32)Que->MdatOffset, Que->MdatSize, (UINT32)Fmp4Mux->BaseDataOffset, 0);
        Fmp4Mux->SequenceNum++;
    }
    return Rval;
}

static UINT32 Fmp4MuxQueue_Pop(FRAGMENT_INFO_s *Fmp4Mux, SVC_STREAM_HDLR_s *Stream)
{
    UINT32 Rval, i;
    UINT8 TrackCount = Fmp4Mux->VideoTrackCount + Fmp4Mux->AudioTrackCount + Fmp4Mux->TextTrackCount;
    const FRAGMENT_QUEUE_s * Que = &Fmp4Mux->QueFragment;
    for (i = 0; i < TrackCount; i++) {
        Fmp4Mux->LastPopSampleCount[i] = Fmp4Mux->PopSampleCount[i];
        Fmp4Mux->PopSampleCount[i] += Que->Traf[i].SampleCount;
    }
    //AmbaPrint_ModulePrintUInt5(SVC_MUXER_PRINT_MODULE_ID, "Fmp4MuxQueue_Pop() : seq=%u offset=%u, size=%u BaseDataOffset=%u", Que->SequenceNum, (UINT32)Que->MdatOffset, Que->MdatSize, (UINT32)Fmp4Mux->BaseDataOffset, 0);
    Rval = Fmp4Mux_PutMoof(Que, Stream, TrackCount, Fmp4Mux->LastPopSampleCount);
    if (Rval == FORMAT_OK) {
        Rval = SvcFormat_PutBe32(Stream, Que->MdatSize + SVC_ISO_BOX_SIZE_TAG_SIZE);
        if (Rval == FORMAT_OK) {
            Rval = SvcFormat_PutBe32(Stream, TAG_MDAT);
            if (Rval == FORMAT_OK) {
                // write MDAT box
            }
        }
    }
    return Rval;
}

static UINT32 Fmp4Mux_LogVideoFrame(FRAGMENT_INFO_s *Fmp4Mux, UINT8 TrackIdx, const SVC_FIFO_BITS_DESC_s *Frame)
{
    UINT32 Rval;
    SVC_VIDEO_TRACK_INFO_s *Video = &Fmp4Mux->VideoTrack[TrackIdx];
    SVC_MEDIA_TRACK_GENERAL_INFO_s *Log = &Video->Info;
    UINT32 FrameSize = 0U;
    UINT32 AddrOffset = GetOffset(Log->BufferBase, Frame->StartAddr);
    if (Log->MediaId == SVC_FORMAT_MID_AVC) {
        Rval = SvcH264_GetFrameSize(Frame->Size, AddrOffset, Log->BufferBase, Log->BufferSize, SVC_H264_ALIGN_SIZE, &FrameSize);
    } else {
        Rval = SvcH265_GetFrameSize(Frame->Size, AddrOffset, Log->BufferBase, Log->BufferSize, 1U, &FrameSize);
    }
    if (Rval == FORMAT_OK) {
        FRAGMENT_TRACK_s * const Traf = &Fmp4Mux->Traf[TrackIdx];
        if (Video->SliceIdx == 0U) {
            Traf->SampleSize[Traf->SampleCount] = FrameSize;
        } else {
            Traf->SampleSize[Traf->SampleCount] += FrameSize;
        }
        if (Frame->Completed != 0U) {
            if (Traf->SampleCount == 0U) {
                Traf->TrackType = SVC_MEDIA_TRACK_TYPE_VIDEO;
                Traf->M = Video->M;
                Traf->SampleDuration = (UINT32)NORMALIZE_TO_TIMESCALE(Log->TimeScale, Log->TimePerFrame, Log->OrigTimeScale);
                Traf->SampleDataOffset = Fmp4Mux->MdatSize;
            }
            Fmp4Mux->MdatSize += Traf->SampleSize[Traf->SampleCount];
#if 0
            if (Frame1->Type == SVC_FIFO_TYPE_IDR_FRAME) {
                INT32 i;
                AmbaPrint("Log sequence[%u] frame[%u] DTS = %llu BaseDataOffset = %llu MdatSize = %u", Fmp4Mux->SequenceNum, Log->FrameCount, Log->DTS, Fmp4Mux->BaseDataOffset, Fmp4Mux->MdatSize);
                for (i = 0; i < Fmp4Mux->TrackCount; i++) {
                    AmbaPrint("Log track[%u] Frame[%u] = DTS = %llu", i, Fmp4Mux->Track[i].FrameCount, Fmp4Mux->Track[i].DTS);
                }
            }
#endif
            Traf->SampleCount++;
            SvcFormat_UpdateTrack(Log, 1);
        }
        SvcFormat_UpdateFrameSliceIdx(Frame, Video);
        Fmp4Mux->EntryLogged[TrackIdx]++;
    } else {
        AmbaPrint_ModulePrintStr5(SVC_MUXER_PRINT_MODULE_ID, "%s incorrect frame size!", __func__, NULL, NULL, NULL, NULL);
        Rval = FORMAT_ERR_GENERAL_ERROR;
    }
    return Rval;
}

/**
 * usually used to get current progress (forward), so use floor as default
 */
static inline UINT64 CAPTURE_TIME_TO_DTS(UINT64 CaptureTimeScale, UINT64 CaptureTime, UINT64 TimeScale)
{
    return ((CaptureTime / CaptureTimeScale) * TimeScale) + (((CaptureTime % CaptureTimeScale) * TimeScale) / CaptureTimeScale);
}

static UINT32 Fmp4Mux_LogVideoEvent(FRAGMENT_INFO_s *Fmp4Mux, UINT8 TrackIdx, SVC_STREAM_HDLR_s *Stream, const SVC_MEDIA_TRACK_GENERAL_INFO_s *DefLog, const SVC_FIFO_BITS_DESC_s *Frame, UINT32 MaxDuration, UINT64 MaxSize, UINT8 *IsComplete)
{
    UINT32 Rval = FORMAT_OK;
    SVC_VIDEO_TRACK_INFO_s *Video = &Fmp4Mux->VideoTrack[TrackIdx];
    SVC_MEDIA_TRACK_GENERAL_INFO_s *Log = &Video->Info;
    // check event
    if ((Fmp4Mux->Event != FORMAT_EVENT_NONE) && (Log->DTS >= DefLog->NextDTS) && (Log->FrameCount > 0U)) {
        //AmbaPrint_ModulePrintStr5(SVC_MUXER_PRINT_MODULE_ID, "%s Reach Event", __func__, NULL, NULL, NULL, NULL);
        if (Fmp4Mux->Event == FORMAT_EVENT_REACH_END) {
            // cannnot stop on B
            if (Frame->FrameType <= SVC_FIFO_TYPE_P_FRAME) {
                Log->DTS = SVC_FORMAT_MAX_TIMESTAMP;
            }
        } else {
            // reach limit, can only stop on IDR
            if (Frame->FrameType == SVC_FIFO_TYPE_IDR_FRAME) {
                Log->DTS = SVC_FORMAT_MAX_TIMESTAMP;
            }
        }
    }
    if (Log->DTS < SVC_FORMAT_MAX_TIMESTAMP) {
        if ((Frame->FrameType == SVC_FIFO_TYPE_IDR_FRAME) && (Video->SliceIdx == 0U)) {
            FRAGMENT_TRACK_s * const Traf = &Fmp4Mux->Traf[TrackIdx];

            if (Video->CaptureTimeMode == 1U) {
                if (Traf->SampleCount == 0U) {
                    Traf->BaseMediaDecodeTime = (Log->DTS * (UINT64)Log->OrigTimeScale) / (UINT64)Log->TimeScale;
                } else {
                    // hit the next gop, check capture time stamp.
                    if (Traf->AdjustedSampleCount != Traf->SampleCount) {
                        UINT32 GopSize = Traf->SampleCount - Traf->AdjustedSampleCount;
                        UINT64 CaptureDTS = CAPTURE_TIME_TO_DTS((UINT64)Video->CaptureTimeScale, Frame->CaptureTimeStamp - Video->InitCaptureTime, (UINT64)Log->TimeScale) - Video->PastDTS + Log->InitDTS;
                        UINT32 Ratio = (Log->TimeScale / Log->OrigTimeScale) * GopSize;
                        UINT32 SampleDuration = Traf->SampleDuration;
                        UINT32 i;
                        if (CaptureDTS >= Log->DTS) {
                            UINT64 Delta = (CaptureDTS - Log->DTS) / (UINT64)Ratio;
                            SampleDuration += (UINT32)Delta;
                            Log->DTS += (Delta * (UINT64)Ratio);
                        } else {
                            UINT64 Delta = (Log->DTS - CaptureDTS) / (UINT64)Ratio;
                            SampleDuration -= (UINT32)Delta;
                            Log->DTS -= (Delta * (UINT64)Ratio);
                        }
                        Log->NextDTS = Log->DTS;
                        for (i = Traf->AdjustedSampleCount; i < Traf->SampleCount; i++) {
                            Traf->AdjustedSampleDuration[i] = SampleDuration;
                        }
                        Traf->AdjustedSampleCount = Traf->SampleCount;
                    }
                }
            } else {
                if (Traf->SampleCount == 0U) {
                    Traf->BaseMediaDecodeTime = (Log->DTS * (UINT64)Log->OrigTimeScale) / (UINT64)Log->TimeScale;
                }
            }
            if (Video->IsDefault == 1U) {
                UINT8 Limit = 0U;
                /* check limit, but inaccurate on Stream size (must consider the unwritten frames) */
                Rval = SvcFormat_CheckLimit(Log, Stream, MaxDuration, MaxSize, &Limit);
                if (Rval == FORMAT_OK) {
                    if (Limit == 1U) {
                        Log->DTS = SVC_FORMAT_MAX_TIMESTAMP;
                        Fmp4Mux->Event = FORMAT_EVENT_REACH_LIMIT;
                    } else {
                        /* hit the next gop so finish the current run. */
                        if (Traf->SampleCount != 0U) {
                            *IsComplete = 1U;
                        }
                    }
                }
            } else {
                if (Log->DTS >= DefLog->DTS) {
                    *IsComplete = 1U;
                }
            }
        }
    }
    return Rval;
}

static UINT32 Fmp4Mux_LogVideoEos(FRAGMENT_INFO_s *Fmp4Mux, UINT8 TrackIdx)
{
    UINT32 Rval = FORMAT_OK;
    SVC_VIDEO_TRACK_INFO_s *Video = &Fmp4Mux->VideoTrack[TrackIdx];
    SVC_MEDIA_TRACK_GENERAL_INFO_s *Log = &Video->Info;
    Log->DTS = SVC_FORMAT_MAX_TIMESTAMP;
    if (Video->CaptureTimeMode == 1U) {
        FRAGMENT_TRACK_s * const Traf = &Fmp4Mux->Traf[TrackIdx];
        UINT32 i;
        for (i = Traf->AdjustedSampleCount; i < Traf->SampleCount; i++) {
            Traf->AdjustedSampleDuration[i] = Traf->SampleDuration;
        }
        Traf->AdjustedSampleCount = Traf->SampleCount;
    }
    if (Video->IsDefault == 1U) {
        AmbaPrint_ModulePrintStr5(SVC_MUXER_PRINT_MODULE_ID, "%s Reach EOS", __func__, NULL, NULL, NULL, NULL);
        Fmp4Mux->Event = FORMAT_EVENT_REACH_END;
    } else {
        if (Fmp4Mux->Event != FORMAT_EVENT_REACH_END) {
            AmbaPrint_ModulePrintStr5(SVC_MUXER_PRINT_MODULE_ID, "%s Error! Should not reach EOS earlier than default video track!", __func__, NULL, NULL, NULL, NULL);
            Rval = FORMAT_ERR_GENERAL_ERROR;
        }
    }
    return Rval;
}

static UINT32 Fmp4Mux_PeekVideoFrame(const FRAGMENT_INFO_s *Fmp4Mux, UINT8 TrackIdx, SVC_FIFO_BITS_DESC_s **Frame)
{
    const UINT32 Pos = Fmp4Mux->EntryLogged[TrackIdx] - Fmp4Mux->EntryWritten[TrackIdx];
    const SVC_FIFO_HDLR_s * Fifo = Fmp4Mux->VideoTrack[TrackIdx].Info.Fifo;
    SVC_FIFO_BITS_DESC_s *Frame1;
    UINT32 Rval = F2F(SvcFIFO_PeekEntry(Fifo, &Frame1, Pos));
    if (Rval == FORMAT_OK) {
        if ((Frame1->Size > 0U) && (Frame1->Size <= SVC_FIFO_MARK_EOS)) {
            *Frame = Frame1;
        } else {
            AmbaPrint_ModulePrintStr5(SVC_MUXER_PRINT_MODULE_ID, "%s incorrect frame size!", __func__, NULL, NULL, NULL, NULL);
            Rval = FORMAT_ERR_GENERAL_ERROR;
        }
    } else {
        if (Rval != FORMAT_ERR_DATA_EMPTY) {
            AmbaPrint_ModulePrintStr5(SVC_MUXER_PRINT_MODULE_ID, "%s SvcFIFO_PeekEntry() failed!", __func__, NULL, NULL, NULL, NULL);
        }
    }
    return Rval;
}

static UINT32 Fmp4Mux_LogVideo(FRAGMENT_INFO_s *Fmp4Mux, UINT8 TrackIdx, SVC_STREAM_HDLR_s *Stream, UINT32 MaxDuration, UINT64 MaxSize, UINT8 *IsComplete)
{
    UINT32 Rval;
    SVC_FIFO_BITS_DESC_s *Frame;
    *IsComplete = 0U;
    Rval = Fmp4Mux_PeekVideoFrame(Fmp4Mux, TrackIdx, &Frame);
    if (Rval == FORMAT_OK) {
        if (SVC_FORMAT_IS_EOS_FRAME(Frame) == 1U) {
            Rval = Fmp4Mux_LogVideoEos(Fmp4Mux, TrackIdx);
        } else {
            UINT8 Tmp;
            const SVC_VIDEO_TRACK_INFO_s *DefTrack = SvcFormat_GetDefaultVideoTrack(Fmp4Mux->VideoTrack, Fmp4Mux->VideoTrackCount, &Tmp);
            if (DefTrack != NULL) {
                const SVC_MEDIA_TRACK_GENERAL_INFO_s *Log = &Fmp4Mux->VideoTrack[TrackIdx].Info;
                // check event
                Rval = Fmp4Mux_LogVideoEvent(Fmp4Mux, TrackIdx, Stream, &DefTrack->Info/*DefLog*/, Frame, MaxDuration, MaxSize, IsComplete);
                // go ahead if DTS < MAX
                if ((Rval == FORMAT_OK) && (Log->DTS < SVC_FORMAT_MAX_TIMESTAMP) && (*IsComplete == 0U)) {
                    Rval = Fmp4Mux_LogVideoFrame(Fmp4Mux, TrackIdx, Frame);
                }
            } else {
                Rval = FORMAT_ERR_GENERAL_ERROR;
            }
        }
    } else if (Rval == FORMAT_ERR_DATA_EMPTY) {
        if (Fmp4Mux->VideoTrack[0].CaptureTimeMode == 1U) {
            *IsComplete = 1U;
        }
    } else {
        AmbaMisra_TouchUnused(&Rval);   // misra-c
    }
    return Rval;
}

static UINT32 Fmp4Mux_LogAudio(FRAGMENT_INFO_s *Fmp4Mux, UINT8 TrackIdx, const SVC_MOVIE_INFO_s *Movie, UINT8 *IsComplete)
{
    UINT32 Rval = FORMAT_OK;
    UINT8 Tmp;
    SVC_AUDIO_TRACK_INFO_s *Audio = &Fmp4Mux->AudioTrack[TrackIdx];
    SVC_MEDIA_TRACK_GENERAL_INFO_s *Log = &Audio->Info;
    const SVC_VIDEO_TRACK_INFO_s *DefTrack = SvcFormat_GetDefaultVideoTrack(Fmp4Mux->VideoTrack, Fmp4Mux->VideoTrackCount, &Tmp);
    if (DefTrack != NULL) {
        const SVC_MEDIA_TRACK_GENERAL_INFO_s *DefLog = &DefTrack->Info;
        /* Note: check if audio is ahead default track before peeking next frame. (inconsistent with Video) */
        /*       -> for meeting SVC's feeding rules. */
        *IsComplete = 0U;
        // check event
        if ((Fmp4Mux->Event != FORMAT_EVENT_NONE) && (Log->DTS >= DefLog->NextDTS) && (Log->FrameCount > 0U)) {
            //AmbaPrint_ModulePrintUInt5(SVC_MUXER_PRINT_MODULE_ID, "Fmp4Mux_LogAudio Reach Event %u %u", Fmp4Mux->Event, TrackIdx, 0, 0, 0);
            Log->DTS = SVC_FORMAT_MAX_TIMESTAMP;
        }
        // go ahead if DTS < MAX
        if (Log->DTS < SVC_FORMAT_MAX_TIMESTAMP) {
            if (Log->DTS >= DefLog->DTS) {
                *IsComplete = 1U;
            } else {
                const UINT32 Pos = Log->FrameCount - Movie->AudioTrack[TrackIdx].Info.FrameCount;
                const SVC_FIFO_HDLR_s *Fifo = Log->Fifo;
                SVC_FIFO_BITS_DESC_s *Frame;
                Rval = F2F(SvcFIFO_PeekEntry(Fifo, &Frame, Pos));
                if (Rval == FORMAT_OK) {
                    if ((Frame->Size > 0U) && (Frame->Size <= SVC_FIFO_MARK_EOS)) {
                        if (SVC_FORMAT_IS_EOS_FRAME(Frame) == 1U) {
                            Log->DTS = SVC_FORMAT_MAX_TIMESTAMP;
                            if (Fmp4Mux->Event == FORMAT_EVENT_REACH_END) {
                                AmbaPrint_ModulePrintStr5(SVC_MUXER_PRINT_MODULE_ID, "%s Reach EOS", __func__, NULL, NULL, NULL, NULL);
                            } else {
                                AmbaPrint_ModulePrintStr5(SVC_MUXER_PRINT_MODULE_ID, "%s Error! Should not reach EOS earlier than default video track!", __func__, NULL, NULL, NULL, NULL);
                                Rval = FORMAT_ERR_GENERAL_ERROR;
                            }
                        } else {
                            FRAGMENT_TRACK_s * const Traf = &Fmp4Mux->Traf[Fmp4Mux->VideoTrackCount + TrackIdx];
                            Traf->SampleSize[Traf->SampleCount] = (UINT32)Frame->Size;
                            if (Traf->SampleCount == 0U) {
                                Traf->TrackType = SVC_MEDIA_TRACK_TYPE_AUDIO;
                                Traf->SampleDataOffset = Fmp4Mux->MdatSize;
                                Traf->SampleDuration = (UINT32)NORMALIZE_TO_TIMESCALE(Log->TimeScale, Log->TimePerFrame, Log->OrigTimeScale);
                                Traf->BaseMediaDecodeTime = (Log->DTS * (UINT64)Log->OrigTimeScale) / (UINT64)Log->TimeScale;
                            }
                            Fmp4Mux->MdatSize += Traf->SampleSize[Traf->SampleCount];
                            Traf->SampleCount++;
                            SvcFormat_UpdateTrack(Log, 1);
                            Fmp4Mux->AudioEntryLogged[TrackIdx]++;
                        }
                    } else {
                        AmbaPrint_ModulePrintStr5(SVC_MUXER_PRINT_MODULE_ID, "%s incorrect frame size!", __func__, NULL, NULL, NULL, NULL);
                        Rval = FORMAT_ERR_GENERAL_ERROR;
                    }
                } else {
                    if (Rval != FORMAT_ERR_DATA_EMPTY) {
                        AmbaPrint_ModulePrintStr5(SVC_MUXER_PRINT_MODULE_ID, "%s SvcFIFO_PeekEntry() failed!", __func__, NULL, NULL, NULL, NULL);
                    } else {
                        if (Movie->VideoTrack[0].CaptureTimeMode == 1U) {
                            *IsComplete = 1U;
                        }
                    }
                }
            }
        }
    } else {
        Rval = FORMAT_ERR_GENERAL_ERROR;
    }
    return Rval;
}

static UINT32 Fmp4Mux_LogText(FRAGMENT_INFO_s *Fmp4Mux, UINT8 TrackIdx, const SVC_MOVIE_INFO_s *Movie, UINT8 *IsComplete)
{
    UINT32 Rval = FORMAT_OK;
    UINT8 Tmp;
    SVC_TEXT_TRACK_INFO_s *Text = &Fmp4Mux->TextTrack[TrackIdx];
    SVC_MEDIA_TRACK_GENERAL_INFO_s *Log = &Text->Info;
    const SVC_VIDEO_TRACK_INFO_s *DefTrack = SvcFormat_GetDefaultVideoTrack(Fmp4Mux->VideoTrack, Fmp4Mux->VideoTrackCount, &Tmp);
    if (DefTrack != NULL) {
        const SVC_MEDIA_TRACK_GENERAL_INFO_s *DefLog = &DefTrack->Info;
        /* Note: check if text is ahead default track before peeking next frame. (inconsistent with Video) */
        /*       -> for meeting SVC's feeding rules. */
        *IsComplete = 0U;
        // check event
        if ((Fmp4Mux->Event != FORMAT_EVENT_NONE) && (Log->DTS >= DefLog->NextDTS) && (Log->FrameCount > 0U)) {
            //AmbaPrint_ModulePrintUInt5(SVC_MUXER_PRINT_MODULE_ID, "Fmp4Mux_LogText Reach Event %u %u", Fmp4Mux->Event, TrackIdx, 0, 0, 0);
            Log->DTS = SVC_FORMAT_MAX_TIMESTAMP;
        }
        // go ahead if DTS < MAX
        if (Log->DTS < SVC_FORMAT_MAX_TIMESTAMP) {
            if (Log->DTS >= DefLog->DTS) {
                *IsComplete = 1U;
            } else {
                const UINT32 Pos = Log->FrameCount - Movie->TextTrack[TrackIdx].Info.FrameCount;
                const SVC_FIFO_HDLR_s * Fifo = Log->Fifo;
                SVC_FIFO_BITS_DESC_s *Frame;
                Rval = F2F(SvcFIFO_PeekEntry(Fifo, &Frame, Pos));
                if (Rval == FORMAT_OK) {
                    if ((Frame->Size > 0U) && (Frame->Size <= SVC_FIFO_MARK_EOS)) {
                        if (SVC_FORMAT_IS_EOS_FRAME(Frame) == 1U) {
                            Log->DTS = SVC_FORMAT_MAX_TIMESTAMP;
                            if (Fmp4Mux->Event == FORMAT_EVENT_REACH_END) {
                                AmbaPrint_ModulePrintStr5(SVC_MUXER_PRINT_MODULE_ID, "%s Reach EOS", __func__, NULL, NULL, NULL, NULL);
                            } else {
                                AmbaPrint_ModulePrintStr5(SVC_MUXER_PRINT_MODULE_ID, "%s Error! Should not reach EOS earlier than default video track!", __func__, NULL, NULL, NULL, NULL);
                                Rval = FORMAT_ERR_GENERAL_ERROR;
                            }
                        } else {
                            FRAGMENT_TRACK_s * const Traf = &Fmp4Mux->Traf[Fmp4Mux->VideoTrackCount + Fmp4Mux->AudioTrackCount + TrackIdx];
                            if (Log->MediaId == SVC_FORMAT_MID_TEXT) {    /* see QT spec TEXT media, add 16-bit length to specifies the number of bytes of Text. */
                                Traf->SampleSize[Traf->SampleCount] = (UINT32)Frame->Size + 2U;
                            } else {
                                Traf->SampleSize[Traf->SampleCount] = (UINT32)Frame->Size;
                            }
                            if (Traf->SampleCount == 0U) {
                                Traf->TrackType = SVC_MEDIA_TRACK_TYPE_TEXT;
                                Traf->SampleDataOffset = Fmp4Mux->MdatSize;
                                Traf->SampleDuration = (UINT32)NORMALIZE_TO_TIMESCALE(Log->TimeScale, Log->TimePerFrame, Log->OrigTimeScale);
                                Traf->BaseMediaDecodeTime = (Log->DTS * (UINT64)Log->OrigTimeScale) / (UINT64)Log->TimeScale;
                            }
                            Fmp4Mux->MdatSize += Traf->SampleSize[Traf->SampleCount];
                            Traf->SampleCount++;
                            SvcFormat_UpdateTrack(Log, 1);
                            Fmp4Mux->TextEntryLogged[TrackIdx]++;
                        }
                    } else {
                        AmbaPrint_ModulePrintStr5(SVC_MUXER_PRINT_MODULE_ID, "%s incorrect frame size!", __func__, NULL, NULL, NULL, NULL);
                        Rval = FORMAT_ERR_GENERAL_ERROR;
                    }
                } else {
                    if (Rval != FORMAT_ERR_DATA_EMPTY) {
                        AmbaPrint_ModulePrintStr5(SVC_MUXER_PRINT_MODULE_ID, "%s SvcFIFO_PeekEntry() failed!", __func__, NULL, NULL, NULL, NULL);
                    } else {
                        if (Movie->VideoTrack[0].CaptureTimeMode == 1U) {
                            *IsComplete = 1U;
                        }
                    }
                }
            }
        }
    } else {
        Rval = FORMAT_ERR_GENERAL_ERROR;
    }
    return Rval;
}

static UINT32 Fmp4Mux_LogOtherVideo(FRAGMENT_INFO_s *Fmp4Mux, UINT8 TrackIdx)
{
    // only return OK or error
    UINT32 Rval = FORMAT_OK;
    UINT8 IsComplete = 0U;
    const SVC_MEDIA_TRACK_GENERAL_INFO_s *Log = &Fmp4Mux->VideoTrack[TrackIdx].Info;
    while ((Log->DTS != SVC_FORMAT_MAX_TIMESTAMP) && (IsComplete != 1U)) {
        Rval = Fmp4Mux_LogVideo(Fmp4Mux, TrackIdx, NULL, 0, 0, &IsComplete);
        if (Rval == FORMAT_ERR_DATA_EMPTY) {
            Rval = FORMAT_OK;
        }
        if (Rval != FORMAT_OK) {
            break;
        }
    }
    return Rval;
}

static UINT32 Fmp4Mux_LogOtherAudio(FRAGMENT_INFO_s *Fmp4Mux, const SVC_MOVIE_INFO_s *Movie, UINT8 TrackIdx)
{
    // only return OK or error
    UINT32 Rval = FORMAT_OK;
    UINT8 IsComplete = 0U;
    const SVC_MEDIA_TRACK_GENERAL_INFO_s *Log = &Fmp4Mux->AudioTrack[TrackIdx].Info;
    while ((Log->DTS != SVC_FORMAT_MAX_TIMESTAMP) && (IsComplete != 1U)) {
        Rval = Fmp4Mux_LogAudio(Fmp4Mux, TrackIdx, Movie, &IsComplete);
        if (Rval == FORMAT_ERR_DATA_EMPTY) {
            Rval = FORMAT_OK;
        }
        if (Rval != FORMAT_OK) {
            break;
        }
    }
    return Rval;
}

static UINT32 Fmp4Mux_LogOtherText(FRAGMENT_INFO_s *Fmp4Mux, const SVC_MOVIE_INFO_s *Movie, UINT8 TrackIdx)
{
    // only return OK or error
    UINT32 Rval = FORMAT_OK;
    UINT8 IsComplete = 0U;
    const SVC_MEDIA_TRACK_GENERAL_INFO_s *Log = &Fmp4Mux->TextTrack[TrackIdx].Info;
    while ((Log->DTS != SVC_FORMAT_MAX_TIMESTAMP) && (IsComplete != 1U)) {
        Rval = Fmp4Mux_LogText(Fmp4Mux, TrackIdx, Movie, &IsComplete);
        if (Rval == FORMAT_ERR_DATA_EMPTY) {
            Rval = FORMAT_OK;
        }
        if (Rval != FORMAT_OK) {
            break;
        }
    }
    return Rval;
}

static UINT32 Fmp4Mux_LogOtherTracks(FRAGMENT_INFO_s *Fmp4Mux, const SVC_MOVIE_INFO_s *Movie, UINT8 DefTrackIdx)
{
    UINT32 Rval = FORMAT_OK;
    UINT8 i;
    for (i = 0; i < Fmp4Mux->VideoTrackCount; i++) {
        if (i != DefTrackIdx) {
            Rval = Fmp4Mux_LogOtherVideo(Fmp4Mux, i);
            if (Rval != FORMAT_OK) {
                break;
            }
        }
    }
    if (Rval == FORMAT_OK) {
        for (i = 0; i < Fmp4Mux->AudioTrackCount; i++) {
            Rval = Fmp4Mux_LogOtherAudio(Fmp4Mux, Movie, i);
            if (Rval != FORMAT_OK) {
                break;
            }
        }
    }
    if (Rval == FORMAT_OK) {
        for (i = 0; i < Fmp4Mux->TextTrackCount; i++) {
            Rval = Fmp4Mux_LogOtherText(Fmp4Mux, Movie, i);
            if (Rval != FORMAT_OK) {
                break;
            }
        }
    }
    return Rval;
}

static UINT32 Fmp4Mux_ProcessVideo(FRAGMENT_INFO_s *Fmp4Mux, SVC_MOVIE_INFO_s *Movie, UINT8 TrackIdx, SVC_STREAM_HDLR_s *Stream, UINT8 *IsComplete)
{
    UINT32 Rval = FORMAT_OK;
    SVC_VIDEO_TRACK_INFO_s *Video = &Movie->VideoTrack[TrackIdx];
    SVC_MEDIA_TRACK_GENERAL_INFO_s *Track = &Video->Info;
    const SVC_MEDIA_TRACK_GENERAL_INFO_s *Log = &Fmp4Mux->VideoTrack[TrackIdx].Info;
    *IsComplete = 0U;
    if (Track->DTS == Log->NextDTS) {
        if (Log->DTS == SVC_FORMAT_MAX_TIMESTAMP) {
            //AmbaPrint_ModulePrintUInt5(SVC_MUXER_PRINT_MODULE_ID, "Fmp4Mux_ProcessVideo Event %u: %u %u %u %u", Fmp4Mux->Event, TrackIdx, (UINT32)Log->DTS, (UINT32)Log->NextDTS, (UINT32)Track->DTS);
            Track->DTS = SVC_FORMAT_MAX_TIMESTAMP;
        } else {
            *IsComplete = 1U;
        }
    } else {
        const SVC_FIFO_HDLR_s * const Fifo = Track->Fifo;
        SVC_FIFO_BITS_DESC_s *Frame1;
        Rval = F2F(SvcFIFO_PeekEntry(Fifo, &Frame1, 0));
        if (Rval == FORMAT_OK) {
            if ((Frame1->Size > 0U) && (Frame1->Size < SVC_FIFO_MARK_EOS)) {
                if ((Frame1->FrameType == SVC_FIFO_TYPE_IDR_FRAME) && (Video->SliceIdx == 0U)) {
                    /* process default track first so please put default track at track 0. */
                    if (Video->IsDefault == 1U) {
                        /* put moof before writing mdat. */
                        Rval = Fmp4MuxQueue_Pop(Fmp4Mux, Stream);
                    }
                }
                if (Rval == FORMAT_OK) {
                    UINT32 WriteSize = 0;
                    if (Video->SliceIdx == 0U) {
                        /* Write Frame 1 to Stream */
                        Rval = Fmp4Mux_WriteVideoFrame(Video, Stream, Frame1, &WriteSize);
                    } else {
                        UINT8 * const BufferBase = Track->BufferBase;
                        UINT32 AddrOffset = GetOffset(BufferBase, Frame1->StartAddr);
                        if (Track->MediaId == SVC_FORMAT_MID_AVC) {
                            Rval = SvcH264_WriteStream(Stream, Frame1->Size, AddrOffset, BufferBase, Track->BufferSize, &WriteSize, SVC_H264_ALIGN_SIZE);
                        } else if (Track->MediaId == SVC_FORMAT_MID_HVC) {
                            Rval = SvcH265_WriteStream(Stream, Frame1->Size, AddrOffset, BufferBase, Track->BufferSize, &WriteSize, 1);
                        } else {
                            AmbaPrint_ModulePrintStr5(SVC_MUXER_PRINT_MODULE_ID, "%s incorrect mediaId!", __func__, NULL, NULL, NULL, NULL);
                            Rval = FORMAT_ERR_GENERAL_ERROR;
                        }
                    }
                    if (Rval == FORMAT_OK) {
                        if (Frame1->Completed == 1U) {
                            SvcFormat_UpdateTrack(Track, 1);
                            if (Video->CaptureTimeMode == 1U) {
                                const FRAGMENT_QUEUE_s * const Que = &Fmp4Mux->QueFragment;
                                UINT32 SliceNum = (UINT32)Video->SliceIdx + 1U;
                                UINT32 FrmIdx = Que->Traf[TrackIdx].SampleCount;
                                FrmIdx -= ((Fmp4Mux->EntryLogged[TrackIdx] - (Fmp4Mux->EntryWritten[TrackIdx] - Video->SliceIdx)) / SliceNum);
                                Track->DTS = Track->DTS + NORMALIZE_TO_TIMESCALE(Track->OrigTimeScale, Que->Traf[TrackIdx].AdjustedSampleDuration[FrmIdx], Track->TimeScale) - Track->TimePerFrame;
                                Track->NextDTS = Track->DTS;
                            }
                        }
                        SvcFormat_UpdateFrameSliceIdx(Frame1, Video);
                        Fmp4Mux->EntryWritten[TrackIdx]++;
                        Rval = F2F(SvcFIFO_RemoveEntry(Fifo, 1));
                        if (Rval != FORMAT_OK) {
                            AmbaPrint_ModulePrintStr5(SVC_MUXER_PRINT_MODULE_ID, "%s SvcFIFO_RemoveEntry() failed!", __func__, NULL, NULL, NULL, NULL);
                        }
                    }
                }
            } else {
                AmbaPrint_ModulePrintStr5(SVC_MUXER_PRINT_MODULE_ID, "%s incorrect frame size", __func__, NULL, NULL, NULL, NULL);
                Rval = FORMAT_ERR_GENERAL_ERROR;
            }
        } else {
            if (Rval == FORMAT_ERR_DATA_EMPTY) {
                /* can not be data empty */
                Rval = FORMAT_ERR_GENERAL_ERROR;
            }
            AmbaPrint_ModulePrintStr5(SVC_MUXER_PRINT_MODULE_ID, "%s SvcFIFO_PeekEntry() failed!", __func__, NULL, NULL, NULL, NULL);
        }
    }
    return Rval;
}

static UINT32 Fmp4Mux_ProcessAudio(FRAGMENT_INFO_s *Fmp4Mux, SVC_MOVIE_INFO_s *Movie, UINT8 TrackIdx, SVC_STREAM_HDLR_s *Stream, UINT8 *IsComplete)
{
    UINT32 Rval = FORMAT_OK;
    SVC_AUDIO_TRACK_INFO_s *Audio = &Movie->AudioTrack[TrackIdx];
    SVC_MEDIA_TRACK_GENERAL_INFO_s *Track = &Audio->Info;
    const SVC_MEDIA_TRACK_GENERAL_INFO_s *Log = &Fmp4Mux->AudioTrack[TrackIdx].Info;
    *IsComplete = 0U;
    if (Track->DTS == Log->NextDTS) {
        if (Log->DTS == SVC_FORMAT_MAX_TIMESTAMP) {
            //AmbaPrint_ModulePrintUInt5(SVC_MUXER_PRINT_MODULE_ID, "Fmp4Mux_ProcessAudio Event %u: %u %u %u %u", Fmp4Mux->Event, TrackIdx, (UINT32)Log->DTS, (UINT32)Log->NextDTS, (UINT32)Track->DTS);
            Track->DTS = SVC_FORMAT_MAX_TIMESTAMP;
        } else {
            *IsComplete = 1U;
        }
    } else {
        const SVC_FIFO_HDLR_s * const Fifo = Track->Fifo;
        SVC_FIFO_BITS_DESC_s *Frame;
        Rval = F2F(SvcFIFO_PeekEntry(Fifo, &Frame, 0));
        if (Rval == FORMAT_OK) {
            if ((Frame->Size > 0U) && (Frame->Size < SVC_FIFO_MARK_EOS)) {
                UINT8 *BufferBase = Track->BufferBase;
                UINT32 BufferSize = Track->BufferSize;
                UINT32 FramePos = GetOffset(BufferBase, Frame->StartAddr);
            //    LOG_INFO("fifo[%u %u] Audio %u %llu %u %llu", Frame->SeqNum, Track->FrameCount, Frame->Type, Track->DTS, Frame->Size, Stream->Func->GetPos(Stream));
                Rval = SvcFormat_WriteStream(Stream, Frame->Size, FramePos, BufferBase, BufferSize);
                if (Rval == FORMAT_OK) {
                    SvcFormat_UpdateTrack(Track, 1);
                    Fmp4Mux->AudioEntryWritten[TrackIdx]++;
                    Rval = F2F(SvcFIFO_RemoveEntry(Fifo, 1));
                    if (Rval != FORMAT_OK) {
                        AmbaPrint_ModulePrintStr5(SVC_MUXER_PRINT_MODULE_ID, "%s SvcFIFO_RemoveEntry() failed!", __func__, NULL, NULL, NULL, NULL);
                    }
                }
            } else {
                AmbaPrint_ModulePrintStr5(SVC_MUXER_PRINT_MODULE_ID, "%s incorrect frame size", __func__, NULL, NULL, NULL, NULL);
                Rval = FORMAT_ERR_GENERAL_ERROR;
            }
        } else {
            if (Rval == FORMAT_ERR_DATA_EMPTY) {
                /* can not be data empty */
                Rval = FORMAT_ERR_GENERAL_ERROR;
            }
            AmbaPrint_ModulePrintStr5(SVC_MUXER_PRINT_MODULE_ID, "%s SvcFIFO_PeekEntry() failed!", __func__, NULL, NULL, NULL, NULL);
        }
    }
    AmbaMisra_TouchUnused(Fmp4Mux);
    return Rval;
}

static UINT32 Fmp4Mux_ProcessText(FRAGMENT_INFO_s *Fmp4Mux, SVC_MOVIE_INFO_s *Movie, UINT8 TrackIdx, SVC_STREAM_HDLR_s *Stream, UINT8 *IsComplete)
{
    UINT32 Rval = FORMAT_OK;
    SVC_TEXT_TRACK_INFO_s *Text = &Movie->TextTrack[TrackIdx];
    SVC_MEDIA_TRACK_GENERAL_INFO_s *Track = &Text->Info;
    const SVC_MEDIA_TRACK_GENERAL_INFO_s *Log = &Fmp4Mux->TextTrack[TrackIdx].Info;
    *IsComplete = 0U;
    if (Track->DTS == Log->NextDTS) {
        if (Log->DTS == SVC_FORMAT_MAX_TIMESTAMP) {
            //AmbaPrint_ModulePrintUInt5(SVC_MUXER_PRINT_MODULE_ID, "Fmp4Mux_ProcessText Event %u: %u %u %u %u", Fmp4Mux->Event, TrackIdx, (UINT32)Log->DTS, (UINT32)Log->NextDTS, (UINT32)Track->DTS);
            Track->DTS = SVC_FORMAT_MAX_TIMESTAMP;
        } else {
            *IsComplete = 1U;
        }
    } else {
        const SVC_FIFO_HDLR_s * const Fifo = Track->Fifo;
        SVC_FIFO_BITS_DESC_s *Frame;
        Rval = F2F(SvcFIFO_PeekEntry(Fifo, &Frame, 0));
        if (Rval == FORMAT_OK) {
            if ((Frame->Size > 0U) && (Frame->Size < SVC_FIFO_MARK_EOS)) {
                UINT8 *BufferBase = Track->BufferBase;
                UINT32 BufferSize = Track->BufferSize;
                UINT32 FramePos = GetOffset(BufferBase, Frame->StartAddr);
            //    LOG_INFO("fifo[%u %u] Text %u %llu %u %llu", Frame->SeqNum, Track->FrameCount, Frame->Type, Track->DTS, Frame->Size, Stream->Func->GetPos(Stream));
                if (Track->MediaId == SVC_FORMAT_MID_TEXT) {    /* see QT spec TEXT media, add 16-bit length to specifies the number of bytes of Text. */
                    Rval = SvcFormat_PutBe16(Stream, (UINT16)Frame->Size);
                    if (Rval == FORMAT_OK) {
                        Rval = SvcFormat_WriteStream(Stream, Frame->Size, FramePos, BufferBase, BufferSize);
                    }
                } else {
                    Rval = SvcFormat_WriteStream(Stream, Frame->Size, FramePos, BufferBase, BufferSize);
                }
                if (Rval == FORMAT_OK) {
                    SvcFormat_UpdateTrack(Track, 1);
                    Fmp4Mux->TextEntryWritten[TrackIdx]++;
                    Rval = F2F(SvcFIFO_RemoveEntry(Fifo, 1));
                    if (Rval != FORMAT_OK) {
                        AmbaPrint_ModulePrintStr5(SVC_MUXER_PRINT_MODULE_ID, "%s SvcFIFO_RemoveEntry() failed!", __func__, NULL, NULL, NULL, NULL);
                    }
                }
            } else {
                AmbaPrint_ModulePrintStr5(SVC_MUXER_PRINT_MODULE_ID, "%s incorrect frame size", __func__, NULL, NULL, NULL, NULL);
                Rval = FORMAT_ERR_GENERAL_ERROR;
            }
        } else {
            if (Rval == FORMAT_ERR_DATA_EMPTY) {
                /* can not be data empty */
                Rval = FORMAT_ERR_GENERAL_ERROR;
            }
            AmbaPrint_ModulePrintStr5(SVC_MUXER_PRINT_MODULE_ID, "%s SvcFIFO_PeekEntry() failed!", __func__, NULL, NULL, NULL, NULL);
        }
    }
    AmbaMisra_TouchUnused(Fmp4Mux);
    return Rval;
}

static UINT32 Fmp4Mux_ProcessVideoTrack(FRAGMENT_INFO_s *Fmp4Mux, SVC_MOVIE_INFO_s *Movie, UINT8 TrackIdx, SVC_STREAM_HDLR_s *Stream)
{
    UINT32 Rval = FORMAT_OK;
    UINT8 IsComplete = 0U;
    const SVC_MEDIA_TRACK_GENERAL_INFO_s *Track = &Movie->VideoTrack[TrackIdx].Info;
    while ((Track->DTS < SVC_FORMAT_MAX_TIMESTAMP) && (IsComplete != 1U)) {
        Rval = Fmp4Mux_ProcessVideo(Fmp4Mux, Movie, TrackIdx, Stream, &IsComplete);
        /* It is used to identify whether a gop begins or ends. */
        if (Rval != FORMAT_OK) {
            break;
        }
    }
    return Rval;
}

static UINT32 Fmp4Mux_ProcessAudioTrack(FRAGMENT_INFO_s *Fmp4Mux, SVC_MOVIE_INFO_s *Movie, UINT8 TrackIdx, SVC_STREAM_HDLR_s *Stream)
{
    UINT32 Rval = FORMAT_OK;
    UINT8 IsComplete = 0U;
    const SVC_MEDIA_TRACK_GENERAL_INFO_s *Track = &Movie->AudioTrack[TrackIdx].Info;
    while ((Track->DTS < SVC_FORMAT_MAX_TIMESTAMP) && (IsComplete != 1U)) {
        Rval = Fmp4Mux_ProcessAudio(Fmp4Mux, Movie, TrackIdx, Stream, &IsComplete);
        /* It is used to identify whether a gop begins or ends. */
        if (Rval != FORMAT_OK) {
            break;
        }
    }
    return Rval;
}

static UINT32 Fmp4Mux_ProcessTextTrack(FRAGMENT_INFO_s *Fmp4Mux, SVC_MOVIE_INFO_s *Movie, UINT8 TrackIdx, SVC_STREAM_HDLR_s *Stream)
{
    UINT32 Rval = FORMAT_OK;
    UINT8 IsComplete = 0U;
    const SVC_MEDIA_TRACK_GENERAL_INFO_s *Track = &Movie->TextTrack[TrackIdx].Info;
    while ((Track->DTS < SVC_FORMAT_MAX_TIMESTAMP) && (IsComplete != 1U)) {
        Rval = Fmp4Mux_ProcessText(Fmp4Mux, Movie, TrackIdx, Stream, &IsComplete);
        /* It is used to identify whether a gop begins or ends. */
        if (Rval != FORMAT_OK) {
            break;
        }
    }
    return Rval;
}

static UINT32 Fmp4Mux_ProcessMovieTracks(FRAGMENT_INFO_s *Fmp4Mux, SVC_MOVIE_INFO_s *Movie, SVC_STREAM_HDLR_s *Stream)
{
    UINT32 Rval = FORMAT_OK;
    UINT8 i;
    for (i = 0; i < Movie->VideoTrackCount; i++) {
        Rval = Fmp4Mux_ProcessVideoTrack(Fmp4Mux, Movie, i, Stream);
        if (Rval != FORMAT_OK) {
            break;
        }
    }
    if (Rval == FORMAT_OK) {
        for (i = 0; i < Movie->AudioTrackCount; i++) {
            Rval = Fmp4Mux_ProcessAudioTrack(Fmp4Mux, Movie, i, Stream);
            if (Rval != FORMAT_OK) {
                break;
            }
        }
    }
    if (Rval == FORMAT_OK) {
        for (i = 0; i < Movie->TextTrackCount; i++) {
            Rval = Fmp4Mux_ProcessTextTrack(Fmp4Mux, Movie, i, Stream);
            if (Rval != FORMAT_OK) {
                break;
            }
        }
    }
    return Rval;
}

static UINT32 ParseVideoInfo(FRAGMENT_INFO_s *Fmp4Mux, SVC_MOVIE_INFO_s *Movie, SVC_STREAM_HDLR_s *Stream, const ISO_MOOV_BOX_s *MoovAtom)
{
    UINT32 Rval = FORMAT_OK;
    if (Fmp4Mux->Parsed == 0U) {
        UINT32 i = 0;
        SVC_FIFO_BITS_DESC_s *Frame;
        SVC_VIDEO_TRACK_INFO_s *Video;
        const SVC_MEDIA_TRACK_GENERAL_INFO_s *Track;
        while (i < Fmp4Mux->VideoTrackCount) {
            Video = &Fmp4Mux->VideoTrack[i];
            Track = &Video->Info;
            Rval = F2F(SvcFIFO_PeekEntry(Track->Fifo, &Frame, 0));
            if (Rval == FORMAT_OK) {
                /* Tina : parse non-VCL here */
                UINT32 FramePos = GetOffset(Track->BufferBase, Frame->StartAddr);
                Rval = IsoMux_ParseNalUnits(Frame->Size, FramePos, Track->BufferBase, Track->BufferSize, Video, &Movie->IsoInfo.VideoTrack[i]);
            }
            if (Rval != FORMAT_OK) {
                // if data_empty, also should leave instead of waiting
                // unless, if user trigger the first process too early, would lead to deadlock (UT case only)
                break;
            }
            i++;
        }
        if (Rval == FORMAT_OK) {
            Rval = Fmp4Mux_PutHead(&Movie->MediaInfo, Stream, MoovAtom);
            if (Rval == FORMAT_OK) {
                Fmp4Mux->BaseDataOffset += IsoMux_GetFtypSize();
                Fmp4Mux->BaseDataOffset += (UINT64)MoovAtom->Box.Size + SVC_ISO_BOX_SIZE_TAG_SIZE; /* mdat header size */
                Fmp4Mux->Parsed = 1U;
            }
        }
    }
    return Rval;
}

static UINT32 Fmp4Mux_SaveFinalProgress(FRAGMENT_INFO_s *Fmp4Mux, SVC_MOVIE_INFO_s *Movie)
{
    UINT32 Rval;
    /* keep track in temp, sync track with log */
#if 0
    AmbaPrint_ModulePrintUInt5(SVC_MUXER_PRINT_MODULE_ID, "Fmp4Mux_ProcessMovie : V(%u/%u, %u/%u, %u)", Movie->VideoTrack[0].Info.FrameCount, Fmp4Mux->VideoTrack[0].Info.FrameCount, (UINT32)Movie->VideoTrack[0].Info.DTS, (UINT32)Fmp4Mux->VideoTrack[0].Info.DTS, (UINT32)Fmp4Mux->VideoTrack[0].Info.NextDTS);
    AmbaPrint_ModulePrintUInt5(SVC_MUXER_PRINT_MODULE_ID, "Fmp4Mux_ProcessMovie : A(%u/%u, %u/%u, %u)", Movie->AudioTrack[0].Info.FrameCount, Fmp4Mux->AudioTrack[0].Info.FrameCount, (UINT32)Movie->AudioTrack[0].Info.DTS, (UINT32)Fmp4Mux->AudioTrack[0].Info.DTS, (UINT32)Fmp4Mux->AudioTrack[0].Info.NextDTS);
#endif
    Rval = W2F(AmbaWrap_memcpy(Fmp4Mux->TempVideoTrack, Movie->VideoTrack, sizeof(Fmp4Mux->TempVideoTrack)));
    if (Rval == FORMAT_OK) {
        Rval = W2F(AmbaWrap_memcpy(Fmp4Mux->TempAudioTrack, Movie->AudioTrack, sizeof(Fmp4Mux->TempAudioTrack)));
    }
    if (Rval == FORMAT_OK) {
        Rval = W2F(AmbaWrap_memcpy(Fmp4Mux->TempTextTrack, Movie->TextTrack, sizeof(Fmp4Mux->TempTextTrack)));
    }
    if (Rval == FORMAT_OK) {
        Rval = W2F(AmbaWrap_memcpy(Movie->VideoTrack, Fmp4Mux->VideoTrack, sizeof(Movie->VideoTrack)));
    }
    if (Rval == FORMAT_OK) {
        Rval = W2F(AmbaWrap_memcpy(Movie->AudioTrack, Fmp4Mux->AudioTrack, sizeof(Movie->AudioTrack)));
    }
    if (Rval == FORMAT_OK) {
        Rval = W2F(AmbaWrap_memcpy(Movie->TextTrack, Fmp4Mux->TextTrack, sizeof(Movie->TextTrack)));
    }
    return Rval;
}

static UINT32 Fmp4Mux_ProcessMovie(FRAGMENT_INFO_s *Fmp4Mux, SVC_MOVIE_INFO_s *Movie, SVC_STREAM_HDLR_s *Stream, const ISO_MOOV_BOX_s *MoovAtom, UINT64 TargetTime, UINT32 MaxDuration, UINT64 MaxSize, UINT8 *Event)
{
    UINT32 Rval = FORMAT_OK;
    Rval = ParseVideoInfo(Fmp4Mux, Movie, Stream, MoovAtom);
    if (Rval == FORMAT_OK) {
        const SVC_VIDEO_TRACK_INFO_s *Video;
        UINT8 DefTrackIdx;
        *Event = FORMAT_EVENT_NONE;
        Video = SvcFormat_GetDefaultVideoTrack(Movie->VideoTrack, Movie->VideoTrackCount, &DefTrackIdx);
        if (Video != NULL) {
            const SVC_MEDIA_TRACK_GENERAL_INFO_s *Log = &Fmp4Mux->VideoTrack[DefTrackIdx].Info;
            UINT64 TargetDTS;
            if (TargetTime == SVC_FORMAT_MAX_TIMESTAMP) {
                TargetDTS = SVC_FORMAT_MAX_TIMESTAMP;
            } else {
                TargetDTS = SVC_FORMAT_TIME_TO_DTS_FLOOR(TargetTime, Video->Info.TimeScale);
            }
            //AmbaPrint_ModulePrintUInt5(SVC_MUXER_PRINT_MODULE_ID, "ProcessMovie : %u", (UINT32)TargetDTS, 0, 0, 0, 0);
            /* App controls the progress of logging default track.
             * The basic unit of a run is based on one gop of the default video track.
             */
            while ((Log->DTS < TargetDTS) && (Rval == FORMAT_OK)) {
                UINT8 IsComplete = 0U;
                //AmbaPrint_PrintUInt5("Fmp4Mux_ProcessMovie: TargetDTS=%u, Log->DTS=%u, Track->DTS=%u", (UINT32)TargetDTS, (UINT32)Log->DTS, (UINT32)Movie->VideoTrack[DefTrackIdx].Info.DTS, 0, 0);
                /* Log the default track first. */
                while ((Log->DTS < TargetDTS) && (IsComplete != 1U)) {
                    Rval = Fmp4Mux_LogVideo(Fmp4Mux, DefTrackIdx, Stream, MaxDuration, MaxSize, &IsComplete);
                    if (Rval != FORMAT_OK) {
                        break;
                    }
                }
                if ((Rval == FORMAT_OK) && ((Log->DTS == SVC_FORMAT_MAX_TIMESTAMP) || (IsComplete == 1U))) {
                    /* Log the remaining tracks. */
                    Rval = Fmp4Mux_LogOtherTracks(Fmp4Mux, Movie, DefTrackIdx);
                    if (Rval == FORMAT_OK) {
                        /* After logging all the tracks, start to push logs. */
                        Rval = Fmp4MuxQueue_Push(Fmp4Mux);
                        if (Rval == FORMAT_OK) {
                            Fmp4Mux->MdatSize = 0;
                            Rval = W2F(AmbaWrap_memset(Fmp4Mux->Traf, 0, sizeof(FRAGMENT_TRACK_s) * SVC_FORMAT_MAX_TRACK_PER_MEDIA));
                            if (Rval == FORMAT_OK) {
                                /* Process all the tracks when the default track does not reach the EOS or Limit.
                                 * The final run will be processed at the closing phase.
                                 */
                                if (Fmp4Mux->Event == FORMAT_EVENT_NONE) {
                                    Rval = Fmp4Mux_ProcessMovieTracks(Fmp4Mux, Movie, Stream);
                                } else {
                                    /* ignore the procss the final run so return the Event got from logging tracks. */
                                    *Event = Fmp4Mux->Event;
                                    Rval = Fmp4Mux_SaveFinalProgress(Fmp4Mux, Movie);
                                }
                            }
                        }
                    }
                }
            }
        } else {
            Rval = FORMAT_ERR_GENERAL_ERROR;
        }
    }
    if (Rval == FORMAT_ERR_DATA_EMPTY) {
        // data empty is meaningless to muxer
        Rval = FORMAT_OK;
    }
    return Rval;
}

/**
 *  Process the Fmp4 muxing format.
 *
 *  @param [in] Fmp4Mux Fmp4 fragment information
 *  @param [in] Media Media information
 *  @param [in] Stream Stream handler
 *  @param [in] MoovAtom Moov atom
 *  @param [in] TargetTime Target time to process
 *  @param [in] MaxDuration Maximum duration of a clip
 *  @param [in] MaxSize Maximum size of a clip
 *  @param [out] Event The returned process event
 *  @return 0 - OK, others - FORMAT_ERR_XXXX
 */
UINT32 Fmp4Mux_ProcessImpl(FRAGMENT_INFO_s *Fmp4Mux, SVC_MEDIA_INFO_s *Media, SVC_STREAM_HDLR_s *Stream, ISO_MOOV_BOX_s *MoovAtom, UINT64 TargetTime, UINT32 MaxDuration, UINT64 MaxSize, UINT8 *Event)
{
    UINT32 Rval;
    if (Media->MediaType == SVC_MEDIA_INFO_MOVIE) {
        SVC_MOVIE_INFO_s *Movie;
        AmbaMisra_TypeCast(&Movie, &Media);
        Rval = Fmp4Mux_ProcessMovie(Fmp4Mux, Movie, Stream, MoovAtom, TargetTime, MaxDuration, MaxSize, Event);
    } else {
        AmbaPrint_ModulePrintUInt5(SVC_MUXER_PRINT_MODULE_ID, "incorrect Media type %u", Media->MediaType, 0U, 0U, 0U, 0U);
        Rval = FORMAT_ERR_INVALID_ARG;
    }
    AmbaMisra_TouchUnused(Media);
    AmbaMisra_TouchUnused(MoovAtom);
    return Rval;
}

static UINT32 Fmp4Mux_CheckFrameRateLimit(const SVC_MOVIE_INFO_s *Movie)
{
    UINT32 Rval = FORMAT_OK;
    UINT8 i;
    for (i = 0; i < Movie->VideoTrackCount; i++) {
        const SVC_MEDIA_TRACK_GENERAL_INFO_s *Track = &Movie->VideoTrack[i].Info;
        UINT32 FramePerSec = (Track->TimeScale + (Track->TimePerFrame - 1U)) / Track->TimePerFrame;
        if (FramePerSec > SVC_ISO_MAX_VIDEO_FPS) {
            AmbaPrint_ModulePrintUInt5(SVC_MUXER_PRINT_MODULE_ID, "Track[%u] FPS %u is larger than %u", i, FramePerSec, SVC_ISO_MAX_VIDEO_FPS, 0U, 0U);
            Rval = FORMAT_ERR_GENERAL_ERROR;
            break;
        }
    }
    for (i = 0; i < Movie->AudioTrackCount; i++) {
        const SVC_MEDIA_TRACK_GENERAL_INFO_s *Track = &Movie->AudioTrack[i].Info;
        UINT32 FramePerSec = (Track->TimeScale + (Track->TimePerFrame - 1U)) / Track->TimePerFrame;
        if (FramePerSec > SVC_ISO_MAX_AUDIO_FPS) {
            AmbaPrint_ModulePrintUInt5(SVC_MUXER_PRINT_MODULE_ID, "Track[%u] FPS %u is larger than %u", i, FramePerSec, SVC_ISO_MAX_AUDIO_FPS, 0U, 0U);
            Rval = FORMAT_ERR_GENERAL_ERROR;
            break;
        }
    }
    for (i = 0; i < Movie->TextTrackCount; i++) {
        const SVC_MEDIA_TRACK_GENERAL_INFO_s *Track = &Movie->TextTrack[i].Info;
        UINT32 FramePerSec = (Track->TimeScale + (Track->TimePerFrame - 1U)) / Track->TimePerFrame;
        if (FramePerSec > SVC_ISO_MAX_TEXT_FPS) {
            AmbaPrint_ModulePrintUInt5(SVC_MUXER_PRINT_MODULE_ID, "Track[%u] FPS %u is larger than %u", i, FramePerSec, SVC_ISO_MAX_TEXT_FPS, 0U, 0U);
            Rval = FORMAT_ERR_GENERAL_ERROR;
            break;
        }
    }
    return Rval;
}

/**
 *  Open the Fmp4 muxing format.
 *
 *  @param [in] Fmp4Mux Fmp4 fragment information
 *  @param [in] Hdlr Muxing format handler
 *  @return 0 - OK, others - FORMAT_ERR_XXXX
 */
UINT32 Fmp4Mux_OpenImpl(FRAGMENT_INFO_s *Fmp4Mux, const SVC_MUX_FORMAT_HDLR_s *Hdlr)
{
    UINT32 Rval = FORMAT_OK;
    const SVC_MEDIA_INFO_s *Media = Hdlr->Media;
    if (Media->MediaType == SVC_MEDIA_INFO_MOVIE) {
        SVC_MOVIE_INFO_s *Movie = NULL;
        const SVC_VIDEO_TRACK_INFO_s *DefTrack;
        SVC_ISO_PRIV_INFO_s *PrivInfo;
        UINT8 Tmp;
        AmbaMisra_TypeCast(&Movie, &Media);
        PrivInfo = &Movie->IsoInfo;
        Rval = W2F(AmbaWrap_memset(PrivInfo, 0, sizeof(SVC_ISO_PRIV_INFO_s)));
        if (Rval == FORMAT_OK) {
            Rval = W2F(AmbaWrap_memset(Fmp4Mux, 0, sizeof(FRAGMENT_INFO_s)));
        }
        if (Rval == FORMAT_OK) {
            Rval = Fmp4Mux_CheckFrameRateLimit(Movie);
        }
        if (Rval == FORMAT_OK) {
            PrivInfo->FtypSize = IsoMux_GetFtypSize();
            DefTrack = SvcFormat_GetDefaultVideoTrack(Movie->VideoTrack, Movie->VideoTrackCount, &Tmp);
            if (DefTrack != NULL) {
                const UINT32 MaxTimePerFrame = DefTrack->Info.TimePerFrame * DefTrack->GOPSize;
                UINT32 i;
                const SVC_MEDIA_TRACK_GENERAL_INFO_s *Track;
                Fmp4Mux->VideoTrackCount = Movie->VideoTrackCount;
                Fmp4Mux->AudioTrackCount = Movie->AudioTrackCount;
                Fmp4Mux->TextTrackCount = Movie->TextTrackCount;
                Fmp4Mux->BaseDataOffset = 0;
                Fmp4Mux->MdatSize = 0;
                Fmp4Mux->SequenceNum = 0;
                Rval = W2F(AmbaWrap_memcpy(Fmp4Mux->VideoTrack, Movie->VideoTrack, sizeof(Fmp4Mux->VideoTrack)));
                if (Rval == FORMAT_OK) {
                    Rval = W2F(AmbaWrap_memcpy(Fmp4Mux->AudioTrack, Movie->AudioTrack, sizeof(Fmp4Mux->AudioTrack)));
                }
                if (Rval == FORMAT_OK) {
                    Rval = W2F(AmbaWrap_memcpy(Fmp4Mux->TextTrack, Movie->TextTrack, sizeof(Fmp4Mux->TextTrack)));
                }
#if 0
                AmbaPrint_ModulePrintUInt5(SVC_MUXER_PRINT_MODULE_ID, "Fmp4Mux_OpenImpl : V(%u/%u, %u/%u, %u)", Movie->VideoTrack[0].Info.FrameCount, Fmp4Mux->VideoTrack[0].Info.FrameCount, (UINT32)Movie->VideoTrack[0].Info.DTS, (UINT32)Fmp4Mux->VideoTrack[0].Info.DTS, (UINT32)Fmp4Mux->VideoTrack[0].Info.NextDTS);
                AmbaPrint_ModulePrintUInt5(SVC_MUXER_PRINT_MODULE_ID, "Fmp4Mux_OpenImpl : A(%u/%u, %u/%u, %u)", Movie->AudioTrack[0].Info.FrameCount, Fmp4Mux->AudioTrack[0].Info.FrameCount, (UINT32)Movie->AudioTrack[0].Info.DTS, (UINT32)Fmp4Mux->AudioTrack[0].Info.DTS, (UINT32)Fmp4Mux->AudioTrack[0].Info.NextDTS);
#endif
                if (Rval == FORMAT_OK) {
                    for (i = 0; i < Movie->VideoTrackCount; i++) {
                        Track = &Movie->VideoTrack[i].Info;
                        if (Track->TimePerFrame > MaxTimePerFrame) {
                            AmbaPrint_ModulePrintUInt5(SVC_MUXER_PRINT_MODULE_ID, "Track[%u] %u is larger than %u", i, Track->TimePerFrame, MaxTimePerFrame, 0U, 0U);
                            Rval = FORMAT_ERR_GENERAL_ERROR;
                            break;
                        }
                    }
                    for (i = 0; i < Movie->AudioTrackCount; i++) {
                        Track = &Movie->AudioTrack[i].Info;
                        if (Track->TimePerFrame > MaxTimePerFrame) {
                            AmbaPrint_ModulePrintUInt5(SVC_MUXER_PRINT_MODULE_ID, "Track[%u] %u is larger than %u", i, Track->TimePerFrame, MaxTimePerFrame, 0U, 0U);
                            Rval = FORMAT_ERR_GENERAL_ERROR;
                            break;
                        }
                    }
                    for (i = 0; i < Movie->TextTrackCount; i++) {
                        Track = &Movie->TextTrack[i].Info;
                        if (Track->TimePerFrame > MaxTimePerFrame) {
                            AmbaPrint_ModulePrintUInt5(SVC_MUXER_PRINT_MODULE_ID, "Track[%u] %u is larger than %u", i, Track->TimePerFrame, MaxTimePerFrame, 0U, 0U);
                            Rval = FORMAT_ERR_GENERAL_ERROR;
                            break;
                        }
                    }
                }
            } else {
                AmbaPrint_ModulePrintStr5(SVC_MUXER_PRINT_MODULE_ID, "%s No default video track!", __func__, NULL, NULL, NULL, NULL);
                Rval = FORMAT_ERR_INVALID_ARG;
            }
        }
    } else {
        AmbaPrint_ModulePrintStr5(SVC_MUXER_PRINT_MODULE_ID, "%s Only support movie now!", __func__, NULL, NULL, NULL, NULL);
        Rval = FORMAT_ERR_INVALID_ARG;
    }
    return Rval;
}

static UINT32 Fmp4Mux_TrimAbort(FRAGMENT_QUEUE_s *Que, SVC_MOVIE_INFO_s *Movie)
{
    UINT32 Rval = FORMAT_OK;
    UINT8 Tmp;
    const SVC_VIDEO_TRACK_INFO_s * const DefVideo = SvcFormat_GetDefaultVideoTrack(Movie->VideoTrack, Movie->VideoTrackCount, &Tmp);
    if (DefVideo != NULL) {
        const SVC_MEDIA_TRACK_GENERAL_INFO_s * const DefTrack = &DefVideo->Info;
        SVC_MEDIA_TRACK_GENERAL_INFO_s *Track;
        UINT64 OriNexDTS[SVC_FORMAT_MAX_TRACK_PER_MEDIA] = {0ULL};
        UINT32 OriFrameCount[SVC_FORMAT_MAX_TRACK_PER_MEDIA] = {0U};
        UINT32 OriSampleCount[SVC_FORMAT_MAX_TRACK_PER_MEDIA] = {0U};
        UINT32 RemoveSamples = 0;
        UINT32 VideoRemoveSamples = 0;  /* for CaptureTimeMode */
        UINT8 i, TrackId, TrackCount;
        FRAGMENT_TRACK_s *Traf;
        /* Trimming is not executed when
         * (1) It needs to update the last second moof, but we don't support it currently.
         * (2) Moof is not enlarged and the free space is less than 8 bytes (2(sample counts)*4) after trimming.
         */
        for (i = 0; i < Movie->VideoTrackCount; i++) {
            SVC_VIDEO_TRACK_INFO_s *Video = &Movie->VideoTrack[i];
            UINT16 M = Video->M;
            Track = &Video->Info;
            TrackId = i;
            Traf = &Que->Traf[TrackId];
            /* record the original value to avoid trimming failed. */
            OriNexDTS[TrackId] = Track->NextDTS;
            OriFrameCount[TrackId] = Track->FrameCount;
            OriSampleCount[TrackId] = Traf->SampleCount;
            while ((Track->FrameCount > M) && (Traf->SampleCount > M)) {
                UINT32 j;
                if (Track->NextDTS <= DefTrack->NextDTS) {
                    break;
                }
                for (j = 0; j < M; j++) {
                    Track->FrameCount--;
                    Traf->SampleCount--;
                    if (Movie->VideoTrack[i].CaptureTimeMode == 0U) {
                        /* assume there is no VFR for easily implementation. */
                        Track->NextDTS -= Track->TimePerFrame;
                    } else {
                        UINT64 SampleDuration = Traf->AdjustedSampleDuration[Traf->SampleCount];
                        Track->NextDTS -= NORMALIZE_TO_TIMESCALE(Track->OrigTimeScale, SampleDuration, Track->TimeScale);
                        VideoRemoveSamples++;
                    }
                    RemoveSamples++;
                }
            }
            AmbaPrint_ModulePrintUInt5(SVC_MUXER_PRINT_MODULE_ID, "Fmp4Mux_TrimAbort : Track[%u] = %u %u, Ori=%u %u", i, Track->FrameCount, (UINT32)Track->NextDTS, OriFrameCount[TrackId], (UINT32)OriNexDTS[TrackId]);
            //AmbaPrint_ModulePrintUInt5(SVC_MUXER_PRINT_MODULE_ID, "Fmp4Mux_TrimAbort : Track[%u] = %u %u, Ori=%u %u", i, Track->FrameCount, (UINT32)Track->NextDTS, OriFrameCount[TrackId], (UINT32)OriNexDTS[TrackId]);
        }
        for (i = 0; i < Movie->AudioTrackCount; i++) {
            Track = &Movie->AudioTrack[i].Info;
            TrackId = i + Movie->VideoTrackCount;
            Traf = &Que->Traf[TrackId];
            /* record the original value to avoid trimming failed. */
            OriNexDTS[TrackId] = Track->NextDTS;
            OriFrameCount[TrackId] = Track->FrameCount;
            OriSampleCount[TrackId] = Traf->SampleCount;
            while ((Track->FrameCount > 1U) && (Traf->SampleCount > 1U)) {
                if (Track->NextDTS <= DefTrack->NextDTS) {
                    break;
                }
                Track->FrameCount--;
                Traf->SampleCount--;
                Track->NextDTS -= Track->TimePerFrame;
                RemoveSamples++;
            }
            AmbaPrint_ModulePrintUInt5(SVC_MUXER_PRINT_MODULE_ID, "Fmp4Mux_TrimAbort : Track[%u] = %u %u, Ori=%u %u", i, Track->FrameCount, (UINT32)Track->NextDTS, OriFrameCount[TrackId], (UINT32)OriNexDTS[TrackId]);
        }
        for (i = 0; i < Movie->TextTrackCount; i++) {
            Track = &Movie->TextTrack[i].Info;
            TrackId = i + Movie->VideoTrackCount + Movie->AudioTrackCount;
            Traf = &Que->Traf[TrackId];
            /* record the original value to avoid trimming failed. */
            OriNexDTS[TrackId] = Track->NextDTS;
            OriFrameCount[TrackId] = Track->FrameCount;
            OriSampleCount[TrackId] = Traf->SampleCount;
            while ((Track->FrameCount > 1U) && (Traf->SampleCount > 1U)) {
                if (Track->NextDTS <= DefTrack->NextDTS) {
                    break;
                }
                Track->FrameCount--;
                Traf->SampleCount--;
                Track->NextDTS -= Track->TimePerFrame;
                RemoveSamples++;
            }
            AmbaPrint_ModulePrintUInt5(SVC_MUXER_PRINT_MODULE_ID, "Fmp4Mux_TrimAbort : Track[%u] = %u %u, Ori=%u %u", i, Track->FrameCount, (UINT32)Track->NextDTS, OriFrameCount[TrackId], (UINT32)OriNexDTS[TrackId]);
        }
        if (RemoveSamples < 2U) {
            // remove less than 2 samples, cannot put free box, restore and abort
            //AmbaPrint_ModulePrintStr5(SVC_MUXER_PRINT_MODULE_ID, "[%s] Too few samples to be removed! Recover!", __func__, NULL, NULL, NULL, NULL);
            for (i = 0; i < Movie->VideoTrackCount; i++) {
                Track = &Movie->VideoTrack[i].Info;
                TrackId = i;
                Track->NextDTS = OriNexDTS[TrackId];
                Track->FrameCount = OriFrameCount[TrackId];
                Que->Traf[TrackId].SampleCount = OriSampleCount[TrackId];
            }
            for (i = 0; i < Movie->AudioTrackCount; i++) {
                Track = &Movie->AudioTrack[i].Info;
                TrackId = i + Movie->VideoTrackCount;
                Track->NextDTS = OriNexDTS[TrackId];
                Track->FrameCount = OriFrameCount[TrackId];
                Que->Traf[TrackId].SampleCount = OriSampleCount[TrackId];
            }
            for (i = 0; i < Movie->TextTrackCount; i++) {
                Track = &Movie->TextTrack[i].Info;
                TrackId = i + Movie->VideoTrackCount + Movie->AudioTrackCount;
                Track->NextDTS = OriNexDTS[TrackId];
                Track->FrameCount = OriFrameCount[TrackId];
                Que->Traf[TrackId].SampleCount = OriSampleCount[TrackId];
            }
            RemoveSamples = 0;  // reset RemmoveSamples
        }
        // update SampleDataOffset due to the size change of Moof
        TrackCount = Movie->VideoTrackCount + Movie->AudioTrackCount + Movie->TextTrackCount;
        for (i = 0; i < TrackCount; i++) {
            Traf = &Que->Traf[i];
            Traf->SampleDataOffset += (RemoveSamples * 4U);
            Traf->SampleDataOffset += (VideoRemoveSamples * 4U); /* for SampleDuration */
        }
    } else {
        Rval = FORMAT_ERR_GENERAL_ERROR;
    }
    return Rval;
}

static UINT32 Fmp4Mux_Trim(FRAGMENT_INFO_s *Fmp4Mux)
{
    UINT32 Rval = FORMAT_OK;
    UINT8 Tmp;
    const SVC_VIDEO_TRACK_INFO_s * const DefVideo = SvcFormat_GetDefaultVideoTrack(Fmp4Mux->VideoTrack, Fmp4Mux->VideoTrackCount, &Tmp);
    if (DefVideo != NULL) {
        const SVC_MEDIA_TRACK_GENERAL_INFO_s * const DefTrack = &DefVideo->Info;
        SVC_MEDIA_TRACK_GENERAL_INFO_s *Track;
        FRAGMENT_QUEUE_s *Que = &Fmp4Mux->QueFragment;
        /* trimming before write the last gop in normal stop case and the last moof is not poped yet. */
        UINT32 RemoveSize = 0U;
        UINT8 i, TrackId;
        FRAGMENT_TRACK_s *Traf;
        //AmbaPrint_ModulePrintUInt5(SVC_MUXER_PRINT_MODULE_ID, "%u DefTrack DTS=%u, NextDTS=%u", __LINE__, (UINT32)DefTrack->DTS, (UINT32)DefTrack->NextDTS, 0, 0);
        for (i = 0; i < Fmp4Mux->VideoTrackCount; i++) {
            SVC_VIDEO_TRACK_INFO_s *Video = &Fmp4Mux->VideoTrack[i];
            UINT16 M = Video->M;
            /* using log to do trimming cause the final media is not processed. */
            Track = &Video->Info;
            TrackId = i;
            Traf = &Que->Traf[TrackId];
            /* The sample data offset should shift cause the previous track trim the frames. */
            Traf->SampleDataOffset -= RemoveSize;
            //AmbaPrint_ModulePrintUInt5(SVC_MUXER_PRINT_MODULE_ID, "Track[%u] = %u %u, NextDTS=%u", i, Track->FrameCount, Que->Traf[i].SampleCount, (UINT32)Track->NextDTS, 0);
            while ((Track->FrameCount > M) && (Traf->SampleCount > M)) { // cannot trim to 0 frame
                UINT32 j;
                if (Track->NextDTS <= DefTrack->NextDTS) {
                    break;
                }
                for (j = 0; j < M; j++) {
                    Track->FrameCount--;
                    Traf->SampleCount--;
                    if (Video->CaptureTimeMode == 0U) {
                        /* assume there is no VFR for easily implementation. */
                        Track->NextDTS -= Track->TimePerFrame;
                    } else {
                        UINT64 SampleDuration = Traf->AdjustedSampleDuration[Traf->SampleCount];
                        Track->NextDTS -= NORMALIZE_TO_TIMESCALE(Track->OrigTimeScale, SampleDuration, Track->TimeScale);
                    }
                    Que->MdatSize -= Traf->SampleSize[Traf->SampleCount];
                    RemoveSize += Traf->SampleSize[Traf->SampleCount];
                }
            }
#if 0
            AmbaPrint_ModulePrintUInt5(SVC_MUXER_PRINT_MODULE_ID, "Fmp4Mux_Trim : Track[%u] = %u %u, NextDTS=%u, RemoveSize=%u", i, Track->FrameCount, Traf->SampleCount, (UINT32)Track->NextDTS, RemoveSize);
#endif
            if (Track->NextDTS > DefTrack->NextDTS) {
                AmbaPrint_ModulePrintStr5(SVC_MUXER_PRINT_MODULE_ID, "%s Did not trim to normal clip becuase of avoiding empty track", __func__, NULL, NULL, NULL, NULL);
                break;
            }
        }
        for (i = 0; i < Fmp4Mux->AudioTrackCount; i++) {
            /* using log to do trimming cause the final media is not processed. */
            Track = &Fmp4Mux->AudioTrack[i].Info;
            TrackId = i + Fmp4Mux->VideoTrackCount;
            Traf = &Que->Traf[TrackId];
            /* The sample data offset should shift cause the previous track trim the frames. */
            Traf->SampleDataOffset -= RemoveSize;
            //Fmp4_Print("Track[%u] = %u %u", i, Track->FrameCount, Que->Traf[i].SampleCount);
            while ((Track->FrameCount > 1U) && (Traf->SampleCount > 1U)) { // cannot trim to 0 frame
                if (Track->NextDTS <= DefTrack->NextDTS) {
                    break;
                }
                Track->FrameCount--;
                Traf->SampleCount--;
                Track->NextDTS -= Track->TimePerFrame;
                Que->MdatSize -= Traf->SampleSize[Traf->SampleCount];
                RemoveSize += Traf->SampleSize[Traf->SampleCount];
            }
#if 0
            AmbaPrint_ModulePrintUInt5(SVC_MUXER_PRINT_MODULE_ID, "Fmp4Mux_Trim : Track[%u] = %u %u, NextDTS=%u, RemoveSize=%u", i, Track->FrameCount, Traf->SampleCount, (UINT32)Track->NextDTS, RemoveSize);
#endif
            if (Track->NextDTS > DefTrack->NextDTS) {
                AmbaPrint_ModulePrintStr5(SVC_MUXER_PRINT_MODULE_ID, "%s Did not trim to normal clip becuase of avoiding empty track", __func__, NULL, NULL, NULL, NULL);
                break;
            }
        }
        for (i = 0; i < Fmp4Mux->TextTrackCount; i++) {
            /* using log to do trimming cause the final media is not processed. */
            Track = &Fmp4Mux->TextTrack[i].Info;
            TrackId = i + Fmp4Mux->VideoTrackCount + Fmp4Mux->AudioTrackCount;
            Traf = &Que->Traf[TrackId];
            /* The sample data offset should shift cause the previous track trim the frames. */
            Traf->SampleDataOffset -= RemoveSize;
            //Fmp4_Print("Track[%u] = %u %u", i, Track->FrameCount, Que->Traf[i].SampleCount);
            while ((Track->FrameCount > 1U) && (Traf->SampleCount > 1U)) { // cannot trim to 0 frame
                if (Track->NextDTS <= DefTrack->NextDTS) {
                    break;
                }
                Track->FrameCount--;
                Traf->SampleCount--;
                Track->NextDTS -= Track->TimePerFrame;
                Que->MdatSize -= Traf->SampleSize[Traf->SampleCount];
                RemoveSize += Traf->SampleSize[Traf->SampleCount];
            }
#if 0
            AmbaPrint_ModulePrintUInt5(SVC_MUXER_PRINT_MODULE_ID, "Fmp4Mux_Trim : Track[%u] = %u %u, NextDTS=%u, RemoveSize=%u", i, Track->FrameCount, Traf->SampleCount, (UINT32)Track->NextDTS, RemoveSize);
#endif
            if (Track->NextDTS > DefTrack->NextDTS) {
                AmbaPrint_ModulePrintStr5(SVC_MUXER_PRINT_MODULE_ID, "%s Did not trim to normal clip becuase of avoiding empty track", __func__, NULL, NULL, NULL, NULL);
                break;
            }
        }
    } else {
        Rval = FORMAT_ERR_GENERAL_ERROR;
    }
    return Rval;
}

static UINT32 Fmp4Mux_Finalize(const FRAGMENT_QUEUE_s *Que, SVC_STREAM_HDLR_s *Stream, UINT8 TrackCount, const UINT32 *LastPopSampleCount, UINT64 MoofStart, UINT64 EndPos)
{
    UINT32 Rval = S2F(Stream->Func->Seek(Stream, (INT64)MoofStart, SVC_STREAM_SEEK_START));
    if (Rval == FORMAT_OK) {
        Rval = Fmp4Mux_PutMoof(Que, Stream, TrackCount, LastPopSampleCount);
        if (Rval == FORMAT_OK) {
            UINT64 MoofEnd;
            Rval = S2F(Stream->Func->GetPos(Stream, &MoofEnd));
            if (Rval == FORMAT_OK) {
                const UINT32 Diff = (UINT32)((Que->MdatOffset - SVC_ISO_BOX_SIZE_TAG_SIZE) - MoofEnd);
#if 0
                AmbaPrint_ModulePrintUInt5(SVC_MUXER_PRINT_MODULE_ID, "MoofStart=%u, MoofEnd=%u Diff=%u", (UINT32)MoofStart, (UINT32)MoofEnd, Diff, 0, 0);
#endif
                if (Diff >= SVC_ISO_BOX_SIZE_TAG_SIZE) {
                    //Fmp4_Print("PutFree : %u", (UINT32)(MdatStart - MoofEnd));
                    Rval = SvcFormat_PutBe32(Stream, Diff);
                    if (Rval == FORMAT_OK) {
                        Rval = SvcFormat_PutBe32(Stream, TAG_FREE);
                    }
                } else {
                    if (Diff != 0U) {
                        AmbaPrint_ModulePrintStr5(SVC_MUXER_PRINT_MODULE_ID, "%s fatal error!", __func__, NULL, NULL, NULL, NULL);
                        Rval = FORMAT_ERR_GENERAL_ERROR;
                    }
                }
                if (Rval == FORMAT_OK) {
                    Rval = S2F(Stream->Func->Seek(Stream, (INT64)EndPos, SVC_STREAM_SEEK_START));
                    if (Rval != FORMAT_OK) {
                        AmbaPrint_ModulePrintStr5(SVC_MUXER_PRINT_MODULE_ID, "%s Stream->Func->Seek() failed!", __func__, NULL, NULL, NULL, NULL);
                    }
                }
            } else {
                AmbaPrint_ModulePrintStr5(SVC_MUXER_PRINT_MODULE_ID, "%s Stream->Func->GetPos() fail!", __func__, NULL, NULL, NULL, NULL);
            }
        }
    } else {
        AmbaPrint_ModulePrintStr5(SVC_MUXER_PRINT_MODULE_ID, "%s Stream->Func->Seek() fail!", __func__, NULL, NULL, NULL, NULL);
    }
    return Rval;
}

static UINT32 Fmp4Mux_GetTailSize(const SVC_MOVIE_INFO_s *Movie)
{
    // size|AMBA|frag|dst_0|count_0|dst_1|count_1|...
    UINT8 Type, MainVer, SubVer;
    UINT32 Size = 12U + ((8U + 4U) * ((UINT32) Movie->VideoTrackCount + Movie->AudioTrackCount + Movie->TextTrackCount));
    SvcFormat_GetAmbaBoxVersion(&Movie->MediaInfo, &Type, &MainVer, &SubVer);
    if (MainVer == 3U) {
        Size += 4U;
        if (SubVer == 1U) {
            Size += 1U;
        }
    }
    return Size;
}

#define MAX_TAIL_SIZE (12U + (12U * SVC_FORMAT_MAX_TRACK_PER_MEDIA) + 4U + 1U)  /**< Maximum tail size */
static UINT32 Fmp4Mux_PutTail(const FRAGMENT_INFO_s *Fmp4Mux, const SVC_MOVIE_INFO_s *Movie, SVC_STREAM_HDLR_s *Stream, UINT32 Size)
{
    UINT8 TailBuffer[MAX_TAIL_SIZE];
    UINT32 Offset = 0U;
    UINT32 Rval = SvcFormat_PutMemBe32(&Offset, TailBuffer, MAX_TAIL_SIZE, Size);
    if (Rval == FORMAT_OK) {
        Rval = SvcFormat_PutMemBe32(&Offset, TailBuffer, MAX_TAIL_SIZE, TAG_AMBA);
        if (Rval == FORMAT_OK) {
            Rval = SvcFormat_PutMemBe32(&Offset, TailBuffer, MAX_TAIL_SIZE, Fmp4Mux->SequenceNum);
            if (Rval == FORMAT_OK) {
                UINT32 i;
                const SVC_MEDIA_TRACK_GENERAL_INFO_s *Track;
                // put frame count and duration (normalized to original duration)
                for (i = 0; i < Movie->VideoTrackCount; i++) {
                    Track = &Movie->VideoTrack[i].Info;
                    Rval = SvcFormat_PutMemBe64(&Offset, TailBuffer, MAX_TAIL_SIZE, NORMALIZE_TO_TIMESCALE(Track->TimeScale, Track->NextDTS - Track->InitDTS, Track->OrigTimeScale));
                    if (Rval == FORMAT_OK) {
                        Rval = SvcFormat_PutMemBe32(&Offset, TailBuffer, MAX_TAIL_SIZE, Track->FrameCount);
                    }
                    if (Rval != FORMAT_OK) {
                        break;
                    }
                }
                if (Rval == FORMAT_OK) {
                    for (i = 0; i < Movie->AudioTrackCount; i++) {
                        Track = &Movie->AudioTrack[i].Info;
                        Rval = SvcFormat_PutMemBe64(&Offset, TailBuffer, MAX_TAIL_SIZE, NORMALIZE_TO_TIMESCALE(Track->TimeScale, Track->NextDTS - Track->InitDTS, Track->OrigTimeScale));
                        if (Rval == FORMAT_OK) {
                            Rval = SvcFormat_PutMemBe32(&Offset, TailBuffer, MAX_TAIL_SIZE, Track->FrameCount);
                        }
                        if (Rval != FORMAT_OK) {
                            break;
                        }
                    }
                }
                if (Rval == FORMAT_OK) {
                    for (i = 0; i < Movie->TextTrackCount; i++) {
                        Track = &Movie->TextTrack[i].Info;
                        Rval = SvcFormat_PutMemBe64(&Offset, TailBuffer, MAX_TAIL_SIZE, NORMALIZE_TO_TIMESCALE(Track->TimeScale, Track->NextDTS - Track->InitDTS, Track->OrigTimeScale));
                        if (Rval == FORMAT_OK) {
                            Rval = SvcFormat_PutMemBe32(&Offset, TailBuffer, MAX_TAIL_SIZE, Track->FrameCount);
                        }
                        if (Rval != FORMAT_OK) {
                            break;
                        }
                    }
                }
            }
        }
    }
    if (Rval == OK) {
        UINT8 Type, MainVer, SubVer;
        SvcFormat_GetAmbaBoxVersion(&Movie->MediaInfo, &Type, &MainVer, &SubVer);
        if (SubVer == 0U) {
            /* v3.0, [BoxHeadr][ClipInfo][U32: CRC(BoxHeader + ClipInfo)] */
            UINT32 CRC = AmbaUtility_Crc32(TailBuffer, Size - 4U);
            Rval = SvcFormat_PutMemBe32(&Offset, TailBuffer, MAX_TAIL_SIZE, CRC);
        } else {
            /* v3.1, [BoxHeadr][ClipInfo][U32: CRC(BoxHeader+ClipInfo+CreateTime)][U8: 1(Validity)] */
            /* Note: In order to ensure if the end of a clip is valid or not, */
            /*       stream must guarantee that the last byte of the file is 0 when file alignment. */
            UINT32 TmpOffset = Offset;
            Rval = SvcFormat_PutMemBe32(&TmpOffset, TailBuffer, MAX_TAIL_SIZE, Movie->MediaInfo.CreationTime);
            if (Rval == FORMAT_OK) {
                UINT32 CRC = AmbaUtility_Crc32(TailBuffer, Size - 1U);
                Rval = SvcFormat_PutMemBe32(&Offset, TailBuffer, MAX_TAIL_SIZE, CRC);
                if (Rval == FORMAT_OK) {
                    Rval = SvcFormat_PutMemByte(&Offset, TailBuffer, MAX_TAIL_SIZE, 1U);
                }
            }
        }
        if (Rval == OK) {
            Rval = SvcFormat_WriteStream(Stream, Size, 0U, TailBuffer, MAX_TAIL_SIZE);
        }
    }
    return Rval;
}

static UINT32 Fmp4Mux_PutFree(const FRAGMENT_INFO_s *Fmp4Mux, const SVC_MOVIE_INFO_s *Movie, SVC_STREAM_HDLR_s *Stream, UINT64 EndPos)
{
    /* put freebox at the tail (for file size alignment) */
    UINT32 Rval = FORMAT_OK;
    UINT64 ClipEnd;
    Rval = S2F(Stream->Func->GetLength(Stream, &ClipEnd));
    if (Rval == FORMAT_OK) {
        const UINT32 TailSize = Fmp4Mux_GetTailSize(Movie);
        if (TailSize > 0U) {
            // always put free box with media info....
            if ((ClipEnd - EndPos) >= (SVC_ISO_BOX_SIZE_TAG_SIZE + (UINT64)TailSize)) {
                Rval = SvcFormat_PutBe32(Stream, (UINT32)(ClipEnd - EndPos));
                if (Rval == FORMAT_OK) {
                    Rval = SvcFormat_PutBe32(Stream, TAG_FREE);
                }
            } else {    /*  0 < file_end - moov_end < TailSize */
                /* put FREE first, to let the clip occupy an extra alignmenet unit */
                Rval = SvcFormat_PutBe32(Stream, 0);
                if (Rval == FORMAT_OK) {
                    Rval = SvcFormat_PutBe32(Stream, TAG_FREE);
                    if (Rval == FORMAT_OK) {
                        Rval = SvcFormat_PutPadding(Stream, TailSize);
                        if (Rval == FORMAT_OK) {
                            Rval = S2F(Stream->Func->GetLength(Stream, &ClipEnd));
                            if (Rval == FORMAT_OK) {
                                Rval = S2F(Stream->Func->Seek(Stream, (INT64)EndPos, SVC_STREAM_SEEK_START));
                                if (Rval == FORMAT_OK) {
                                    Rval = SvcFormat_PutBe32(Stream, (UINT32)(ClipEnd - EndPos));
                                } else {
                                    AmbaPrint_ModulePrintStr5(SVC_MUXER_PRINT_MODULE_ID, "%s Stream->Func->Seek() failed!", __func__, NULL, NULL, NULL, NULL);
                                }
                            } else {
                                AmbaPrint_ModulePrintStr5(SVC_MUXER_PRINT_MODULE_ID, "%s Stream->Func->GetLength() failed!", __func__, NULL, NULL, NULL, NULL);
                            }
                        }
                    }
                }
            }
        } else {
            AmbaPrint_ModulePrintStr5(SVC_MUXER_PRINT_MODULE_ID, "%s TailSize is 0!", __func__, NULL, NULL, NULL, NULL);
            Rval = FORMAT_ERR_GENERAL_ERROR;
        }
        if (Rval == FORMAT_OK) {
            Rval = S2F(Stream->Func->Seek(Stream, (-1 * (INT64)TailSize), SVC_STREAM_SEEK_END));
            if (Rval == FORMAT_OK) {
                Rval = Fmp4Mux_PutTail(Fmp4Mux, Movie, Stream, TailSize);
            } else {
                AmbaPrint_ModulePrintStr5(SVC_MUXER_PRINT_MODULE_ID, "%s Stream->Func->Seek() failed!", __func__, NULL, NULL, NULL, NULL);
            }
        }
    } else {
        AmbaPrint_ModulePrintStr5(SVC_MUXER_PRINT_MODULE_ID, "%s Stream->Func->GetLength() failed!", __func__, NULL, NULL, NULL, NULL);
    }
    return Rval;
}

static UINT32 Fmp4Mux_ProcessFinalVideo(FRAGMENT_INFO_s *Fmp4Mux, SVC_MOVIE_INFO_s *Movie, SVC_STREAM_HDLR_s *Stream)
{
    UINT32 Rval = FORMAT_OK;
    UINT8 i, IsComplete;
    for (i = 0; i < Movie->VideoTrackCount; i++) {
        const SVC_MEDIA_TRACK_GENERAL_INFO_s *Track = &Movie->VideoTrack[i].Info;
        while (Track->DTS < SVC_FORMAT_MAX_TIMESTAMP) {
            Rval = Fmp4Mux_ProcessVideo(Fmp4Mux, Movie, i, Stream, &IsComplete);
            if ((Rval != FORMAT_OK) || (IsComplete > 0U)) {
                break;
            }
        }
        if (Rval == FORMAT_OK) {
            // could need to remove some more frames (the last frame of other tracks are not processed due to trim)
            UINT32 Diff = Fmp4Mux->EntryLogged[i] - Fmp4Mux->EntryWritten[i];
#if 0
            AmbaPrint_ModulePrintUInt5(SVC_MUXER_PRINT_MODULE_ID, "Fmp4Mux_ProcessFinalMedia: Video %u %u", Fmp4Mux->EntryLogged[i], Fmp4Mux->EntryWritten[i], 0, 0, 0);
#endif
            if (Diff > 0U) {
                Rval = F2F(SvcFIFO_RemoveEntry(Track->Fifo, Diff));
                if (Rval != FORMAT_OK) {
                    AmbaPrint_ModulePrintStr5(SVC_MUXER_PRINT_MODULE_ID, "%s SvcFIFO_RemoveEntry failed!", __func__, NULL, NULL, NULL, NULL);
                }
            }
        }
        if (Rval != FORMAT_OK) {
            break;
        }
    }
    return Rval;
}

static UINT32 Fmp4Mux_ProcessFinalAudio(FRAGMENT_INFO_s *Fmp4Mux, SVC_MOVIE_INFO_s *Movie, SVC_STREAM_HDLR_s *Stream)
{
    UINT32 Rval = FORMAT_OK;
    UINT8 i, IsComplete;
    for (i = 0; i < Movie->AudioTrackCount; i++) {
        const SVC_MEDIA_TRACK_GENERAL_INFO_s *Track = &Movie->AudioTrack[i].Info;
        while (Track->DTS < SVC_FORMAT_MAX_TIMESTAMP) {
            Rval = Fmp4Mux_ProcessAudio(Fmp4Mux, Movie, i, Stream, &IsComplete);
            if ((Rval != FORMAT_OK) || (IsComplete > 0U)) {
                break;
            }
        }
        if (Rval == FORMAT_OK) {
            // could need to remove some more frames (the last frame of other tracks are not processed due to trim)
            UINT32 Diff = Fmp4Mux->AudioEntryLogged[i] - Fmp4Mux->AudioEntryWritten[i];
#if 0
            AmbaPrint_ModulePrintUInt5(SVC_MUXER_PRINT_MODULE_ID, "Fmp4Mux_ProcessFinalMedia: Audio %u %u", Fmp4Mux->AudioEntryLogged[i], Fmp4Mux->AudioEntryWritten[i], 0, 0, 0);
#endif
            if (Diff > 0U) {
                Rval = F2F(SvcFIFO_RemoveEntry(Track->Fifo, Diff));
                if (Rval != FORMAT_OK) {
                    AmbaPrint_ModulePrintStr5(SVC_MUXER_PRINT_MODULE_ID, "%s SvcFIFO_RemoveEntry failed!", __func__, NULL, NULL, NULL, NULL);
                }
            }
        }
        if (Rval != FORMAT_OK) {
            break;
        }
    }
    return Rval;
}

static UINT32 Fmp4Mux_ProcessFinalText(FRAGMENT_INFO_s *Fmp4Mux, SVC_MOVIE_INFO_s *Movie, SVC_STREAM_HDLR_s *Stream)
{
    UINT32 Rval = FORMAT_OK;
    UINT8 i, IsComplete;
    for (i = 0; i < Movie->TextTrackCount; i++) {
        const SVC_MEDIA_TRACK_GENERAL_INFO_s *Track = &Movie->TextTrack[i].Info;
        while (Track->DTS < SVC_FORMAT_MAX_TIMESTAMP) {
            Rval = Fmp4Mux_ProcessText(Fmp4Mux, Movie, i, Stream, &IsComplete);
            if ((Rval != FORMAT_OK) || (IsComplete > 0U)) {
                break;
            }
        }
        if (Rval == FORMAT_OK) {
            // could need to remove some more frames (the last frame of other tracks are not processed due to trim)
            UINT32 Diff = Fmp4Mux->TextEntryLogged[i] - Fmp4Mux->TextEntryWritten[i];
#if 0
            AmbaPrint_ModulePrintUInt5(SVC_MUXER_PRINT_MODULE_ID, "Fmp4Mux_ProcessFinalMedia: Text %u %u", Fmp4Mux->TextEntryLogged[i], Fmp4Mux->TextEntryWritten[i], 0, 0, 0);
#endif
            if (Diff > 0U) {
                Rval = F2F(SvcFIFO_RemoveEntry(Track->Fifo, Diff));
                if (Rval != FORMAT_OK) {
                    AmbaPrint_ModulePrintStr5(SVC_MUXER_PRINT_MODULE_ID, "%s SvcFIFO_RemoveEntry failed!", __func__, NULL, NULL, NULL, NULL);
                }
            }
        }
        if (Rval != FORMAT_OK) {
            break;
        }
    }
    return Rval;
}

static UINT32 Fmp4Mux_ProcessFinalMedia(FRAGMENT_INFO_s *Fmp4Mux, SVC_MOVIE_INFO_s *Movie, SVC_STREAM_HDLR_s *Stream)
{
    UINT32 Rval = Fmp4Mux_ProcessFinalVideo(Fmp4Mux, Movie, Stream);

    if (Rval == FORMAT_OK) {
        Rval = Fmp4Mux_ProcessFinalAudio(Fmp4Mux, Movie, Stream);
    }
    if (Rval == FORMAT_OK) {
        Rval = Fmp4Mux_ProcessFinalText(Fmp4Mux, Movie, Stream);
    }
    return Rval;
}

static UINT8 Fmp4Mux_IsAbort(const FRAGMENT_INFO_s *Fmp4Mux)
{
    UINT8 Rval = 1U;
    // in auto split case, DTS will be restored...so use Event to check
    if (Fmp4Mux->Event != FORMAT_EVENT_NONE) {
        Rval = 0U;
    }
    return Rval;
}

static UINT32 Fmp4Mux_SaveProgress(FRAGMENT_INFO_s *Fmp4Mux, SVC_MOVIE_INFO_s *Movie)
{
    UINT32 Rval = FORMAT_OK;
    UINT8 i;
    if (Fmp4Mux->Event != FORMAT_EVENT_NONE) {
        // restore track
#if 0
        AmbaPrint_ModulePrintUInt5(SVC_MUXER_PRINT_MODULE_ID, "Fmp4Mux_CloseImpl restore : V(%u/%u, %u/%u, %u)", Movie->VideoTrack[0].Info.FrameCount, Fmp4Mux->VideoTrack[0].Info.FrameCount, (UINT32)Movie->VideoTrack[0].Info.DTS, (UINT32)Fmp4Mux->VideoTrack[0].Info.DTS, (UINT32)Fmp4Mux->VideoTrack[0].Info.NextDTS);
        AmbaPrint_ModulePrintUInt5(SVC_MUXER_PRINT_MODULE_ID, "Fmp4Mux_CloseImpl restore : A(%u/%u, %u/%u, %u)", Movie->AudioTrack[0].Info.FrameCount, Fmp4Mux->AudioTrack[0].Info.FrameCount, (UINT32)Movie->AudioTrack[0].Info.DTS, (UINT32)Fmp4Mux->AudioTrack[0].Info.DTS, (UINT32)Fmp4Mux->AudioTrack[0].Info.NextDTS);
#endif
        Rval = W2F(AmbaWrap_memcpy(Movie->VideoTrack, Fmp4Mux->TempVideoTrack, sizeof(Movie->VideoTrack)));
        if (Rval == FORMAT_OK) {
            Rval = W2F(AmbaWrap_memcpy(Movie->AudioTrack, Fmp4Mux->TempAudioTrack, sizeof(Movie->AudioTrack)));
        }
        if (Rval == FORMAT_OK) {
            Rval = W2F(AmbaWrap_memcpy(Movie->TextTrack, Fmp4Mux->TempTextTrack, sizeof(Movie->TextTrack)));
        }
    }
    // keep log in temp
    if (Rval == FORMAT_OK) {
        Rval = W2F(AmbaWrap_memcpy(Fmp4Mux->TempVideoTrack, Fmp4Mux->VideoTrack, sizeof(Fmp4Mux->TempVideoTrack)));
        for (i = 0; i < Movie->VideoTrackCount; i++) {
            Fmp4Mux->TempVideoTrack[i].Info.DTS = Fmp4Mux->TempVideoTrack[i].Info.NextDTS;
        }
    }
    if (Rval == FORMAT_OK) {
        Rval = W2F(AmbaWrap_memcpy(Fmp4Mux->TempAudioTrack, Fmp4Mux->AudioTrack, sizeof(Fmp4Mux->TempAudioTrack)));
        for (i = 0; i < Movie->AudioTrackCount; i++) {
            Fmp4Mux->TempAudioTrack[i].Info.DTS = Fmp4Mux->TempAudioTrack[i].Info.NextDTS;
        }
    }
    if (Rval == FORMAT_OK) {
        Rval = W2F(AmbaWrap_memcpy(Fmp4Mux->TempTextTrack, Fmp4Mux->TextTrack, sizeof(Fmp4Mux->TempTextTrack)));
        for (i = 0; i < Movie->TextTrackCount; i++) {
            Fmp4Mux->TempTextTrack[i].Info.DTS = Fmp4Mux->TempTextTrack[i].Info.NextDTS;
        }
    }
    return Rval;
}

static UINT32 Fmp4Mux_RestoreProgress(const FRAGMENT_INFO_s *Fmp4Mux, SVC_MOVIE_INFO_s *Movie)
{
    // sync track with temp (log)
    UINT32 Rval = W2F(AmbaWrap_memcpy(Movie->VideoTrack, Fmp4Mux->TempVideoTrack, sizeof(Movie->VideoTrack)));
    if (Rval == FORMAT_OK) {
        Rval = W2F(AmbaWrap_memcpy(Movie->AudioTrack, Fmp4Mux->TempAudioTrack, sizeof(Movie->AudioTrack)));
    }
    if (Rval == FORMAT_OK) {
        Rval = W2F(AmbaWrap_memcpy(Movie->TextTrack, Fmp4Mux->TempTextTrack, sizeof(Movie->TextTrack)));
    }
    return Rval;
}

/**
 *  Close the Fmp4 muxing format.
 *
 *  @param [in] Fmp4Mux Fmp4 fragment information
 *  @param [in] Hdlr Muxing format handler
 *  @param [in] Mode Close mode (SVC_MUX_FORMAT_CLOSE_XXXX)
 *  @return 0 - OK, others - FORMAT_ERR_XXXX
 */
UINT32 Fmp4Mux_CloseImpl(FRAGMENT_INFO_s *Fmp4Mux, const SVC_MUX_FORMAT_HDLR_s *Hdlr, UINT8 Mode)
{
    UINT32 Rval = FORMAT_OK;
    const SVC_MEDIA_INFO_s *Media = Hdlr->Media;
    if (Media->MediaType == SVC_MEDIA_INFO_MOVIE) {
        SVC_STREAM_HDLR_s * const Stream = Hdlr->Stream;
        SVC_MOVIE_INFO_s *Movie;
        AmbaMisra_TypeCast(&Movie, &Media);
        Rval = Fmp4Mux_SaveProgress(Fmp4Mux, Movie);
#if 0
        AmbaPrint_ModulePrintUInt5(SVC_MUXER_PRINT_MODULE_ID, "Fmp4Mux_CloseImpl begin : V(%u/%u, %u/%u, %u)", Movie->VideoTrack[0].Info.FrameCount, Fmp4Mux->VideoTrack[0].Info.FrameCount, (UINT32)Movie->VideoTrack[0].Info.DTS, (UINT32)Fmp4Mux->VideoTrack[0].Info.DTS, (UINT32)Fmp4Mux->VideoTrack[0].Info.NextDTS);
        AmbaPrint_ModulePrintUInt5(SVC_MUXER_PRINT_MODULE_ID, "Fmp4Mux_CloseImpl begin : A(%u/%u, %u/%u, %u)", Movie->AudioTrack[0].Info.FrameCount, Fmp4Mux->AudioTrack[0].Info.FrameCount, (UINT32)Movie->AudioTrack[0].Info.DTS, (UINT32)Fmp4Mux->AudioTrack[0].Info.DTS, (UINT32)Fmp4Mux->AudioTrack[0].Info.NextDTS);
#endif
        if (Rval == FORMAT_OK) {
            if ((Mode <= SVC_MUX_FORMAT_CLOSE_NOT_END)) {  // CLOSE_DEFAULT or CLOSE_NOT_END
                UINT64 EndPos = 0;
                if (Fmp4Mux_IsAbort(Fmp4Mux) != 0U) {
                    Rval = S2F(Stream->Func->GetPos(Stream, &EndPos));
                    if (Rval == FORMAT_OK) {
                        if (EndPos == 0ULL) {
                            AmbaPrint_ModulePrintStr5(SVC_MUXER_PRINT_MODULE_ID, "%s EndPos failed!", __func__, NULL, NULL, NULL, NULL);
                            Rval = FORMAT_ERR_GENERAL_ERROR;
                        }
                    } else {
                        AmbaPrint_ModulePrintStr5(SVC_MUXER_PRINT_MODULE_ID, "%s Stream->Func->GetPos() failed!", __func__, NULL, NULL, NULL, NULL);
                    }
                    if (Rval == FORMAT_OK) {
                        if (Mode == SVC_MUX_FORMAT_CLOSE_DEFAULT) {
                            FRAGMENT_QUEUE_s *Que = &Fmp4Mux->QueFragment;
                            const UINT8 TrackCount = Fmp4Mux->VideoTrackCount + Fmp4Mux->AudioTrackCount + Fmp4Mux->TextTrackCount;
                            const UINT32 OriMoofSize = Fmp4Mux_GetMoofSize(Que, Que->SequenceNum, (UINT8)TrackCount, Fmp4Mux->LastPopSampleCount);
                            const UINT64 OriMoofStart = Que->MdatOffset - SVC_ISO_BOX_SIZE_TAG_SIZE - OriMoofSize;
                            Rval = Fmp4Mux_TrimAbort(Que, Movie);
                            if (Rval == FORMAT_OK) {
                                Rval = Fmp4Mux_Finalize(Que, Stream, TrackCount, Fmp4Mux->LastPopSampleCount, OriMoofStart, EndPos);
                            }
                        }
                    }
                } else {
                    if (Mode == SVC_MUX_FORMAT_CLOSE_DEFAULT) {
                        Rval = Fmp4Mux_Trim(Fmp4Mux);
                    }
                    if (Rval == FORMAT_OK) {
                        Rval = Fmp4Mux_ProcessFinalMedia(Fmp4Mux, Movie, Stream);
                        if (Rval == FORMAT_OK) {
                            Rval = S2F(Stream->Func->GetPos(Stream, &EndPos));
                            if (Rval == FORMAT_OK) {
                                if (EndPos == 0ULL) {
                                    AmbaPrint_ModulePrintStr5(SVC_MUXER_PRINT_MODULE_ID, "%s EndPos failed!", __func__, NULL, NULL, NULL, NULL);
                                    Rval = FORMAT_ERR_GENERAL_ERROR;
                                }
                            } else {
                                AmbaPrint_ModulePrintStr5(SVC_MUXER_PRINT_MODULE_ID, "%s Stream->Func->GetPos() failed!", __func__, NULL, NULL, NULL, NULL);
                            }
                        }
                    }
                }
                if (Rval == FORMAT_OK) {
                    if (SvcFormat_HasEmptyTrack(Media) == 0U) {
                        Rval = Fmp4Mux_PutFree(Fmp4Mux, Movie, Stream, EndPos);
                    } else {
                        AmbaPrint_ModulePrintStr5(SVC_MUXER_PRINT_MODULE_ID, "%s SvcFormat_HasEmptyTrack() failed!", __func__, NULL, NULL, NULL, NULL);
                        Rval = FORMAT_ERR_GENERAL_ERROR;
                    }
                }
            }
            if (Rval == FORMAT_OK) {
                Rval = Fmp4Mux_RestoreProgress(Fmp4Mux, Movie);
            }
        } else {
            AmbaPrint_ModulePrintStr5(SVC_MUXER_PRINT_MODULE_ID, "%s Fmp4Mux_SaveProgress() failed!", __func__, NULL, NULL, NULL, NULL);
        }
#if 0
        AmbaPrint_ModulePrintUInt5(SVC_MUXER_PRINT_MODULE_ID, "Fmp4Mux_CloseImpl end : V(%u/%u, %u/%u, %u)", Movie->VideoTrack[0].Info.FrameCount, Fmp4Mux->VideoTrack[0].Info.FrameCount, (UINT32)Movie->VideoTrack[0].Info.DTS, (UINT32)Fmp4Mux->VideoTrack[0].Info.DTS, (UINT32)Fmp4Mux->VideoTrack[0].Info.NextDTS);
        AmbaPrint_ModulePrintUInt5(SVC_MUXER_PRINT_MODULE_ID, "Fmp4Mux_CloseImpl end : A(%u/%u, %u/%u, %u)", Movie->AudioTrack[0].Info.FrameCount, Fmp4Mux->AudioTrack[0].Info.FrameCount, (UINT32)Movie->AudioTrack[0].Info.DTS, (UINT32)Fmp4Mux->AudioTrack[0].Info.DTS, (UINT32)Fmp4Mux->AudioTrack[0].Info.NextDTS);
#endif
    } else {
        AmbaPrint_ModulePrintStr5(SVC_MUXER_PRINT_MODULE_ID, "%s Only support movie now", __func__, NULL, NULL, NULL, NULL);
        Rval = FORMAT_ERR_GENERAL_ERROR;
    }
    return Rval;
}

