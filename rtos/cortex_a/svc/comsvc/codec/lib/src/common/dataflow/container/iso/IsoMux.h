/**
 * @file IsoMux.h
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
#ifndef ISO_MUX_H
#define ISO_MUX_H

#include "format/SvcFormat.h"
#include "IsoBoxDef.h"

/**
 *  Write a ftyp box to stream.
 *
 *  @param [in] Media Media information
 *  @param [in] Stream Stream handler
 *  @return 0 - OK, others - FORMAT_ERR_XXXX
 */
UINT32 IsoMux_MP4_PutFtyp(const SVC_MEDIA_INFO_s *Media, SVC_STREAM_HDLR_s *Stream);

/**
 *  Track duration formula = (MaxPTS - MiPTS) + (MaxPTS - SecondMaxPTS)
 *  Track's duration = sum of the durations of all of the track¡¦s edits,
 *                     include empty time of the track.
 *  Edts box = segment_duration present the initDTS, if initDTS != 0,
 *             the track have offset time need to skip present.
 */

/*  shared by MP4 and Fragmented MP4 */

/**
 *  Initialize an udta box.
 *
 *  @param [out] Udta Udta box
 *  @param [in] Media Media information
 *  @return 0 - OK, others - FORMAT_ERR_XXXX
 */
UINT32 IsoMux_UDTA_Init(ISO_UDTA_BOX_s *Udta, SVC_MEDIA_INFO_s *Media);

/**
 *  Write an udta box to stream.
 *
 *  @param [in] Udta Udta box
 *  @param [in] Stream Stream handler
 *  @param [in] Media Media information
 *  @return 0 - OK, others - FORMAT_ERR_XXXX
 */
UINT32 IsoMux_UDTA_Write(const ISO_UDTA_BOX_s *Udta, SVC_STREAM_HDLR_s *Stream, SVC_MEDIA_INFO_s *Media);

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
UINT32 IsoMux_VideoTRAK_Init(ISO_TRACK_BOX_s *IsoTrack, const SVC_VIDEO_TRACK_INFO_s *Track, UINT8 TrackIdx, const SVC_MEDIA_INFO_s *Media, const SVC_ISO_PRIV_INFO_s *PrivInfo, UINT8 Dummy);

/**
 *  Write a trak box to stream for video track.
 *
 *  @param [in] Trak Trak box
 *  @param [in] Track Video track information
 *  @param [in] Stream Stream handler
 *  @return 0 - OK, others - FORMAT_ERR_XXXX
 */
UINT32 IsoMux_VideoTRAK_Write(ISO_TRACK_BOX_s *Trak, const SVC_VIDEO_TRACK_INFO_s *Track, SVC_STREAM_HDLR_s *Stream);

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
UINT32 IsoMux_AudioTRAK_Init(ISO_TRACK_BOX_s *IsoTrack, const SVC_AUDIO_TRACK_INFO_s *Track, UINT8 TrackIdx, const SVC_MEDIA_INFO_s *Media, const SVC_ISO_PRIV_INFO_s *PrivInfo, UINT8 Dummy);

/**
 *  Write a trak box to stream for audio track.
 *
 *  @param [in] Trak Trak box
 *  @param [in] Track Audio track information
 *  @param [in] Stream Stream handler
 *  @return 0 - OK, others - FORMAT_ERR_XXXX
 */
UINT32 IsoMux_AudioTRAK_Write(ISO_TRACK_BOX_s *Trak, const SVC_AUDIO_TRACK_INFO_s *Track, SVC_STREAM_HDLR_s *Stream);

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
UINT32 IsoMux_TextTRAK_Init(ISO_TRACK_BOX_s *IsoTrack, const SVC_TEXT_TRACK_INFO_s *Track, UINT8 TrackIdx, const SVC_MEDIA_INFO_s *Media, const SVC_ISO_PRIV_INFO_s *PrivInfo, UINT8 Dummy);

/**
 *  Write a trak box to stream for text track.
 *
 *  @param [in] Trak Trak box
 *  @param [in] Track Text track information
 *  @param [in] Stream Stream handler
 *  @return 0 - OK, others - FORMAT_ERR_XXXX
 */
UINT32 IsoMux_TextTRAK_Write(ISO_TRACK_BOX_s *Trak, SVC_TEXT_TRACK_INFO_s *Track, SVC_STREAM_HDLR_s *Stream);

/**
 *  Initialize a mvhd box.
 *
 *  @param [out] Mvhd Mvhd box
 *  @param [in] Media Media information
 *  @param [out] MinInitDTS Minimum initial DTS
 *  @return 0 - OK, others - FORMAT_ERR_XXXX
 */
UINT32 IsoMux_MVHD_Init(ISO_MVHD_BOX_s *Mvhd, SVC_MEDIA_INFO_s *Media, UINT64 *MinInitDTS);

/**
 *  Write a mvhd box to stream.
 *
 *  @param [in] Mvhd Mvhd box
 *  @param [in] Stream Stream handler
 *  @return 0 - OK, others - FORMAT_ERR_XXXX
 */
UINT32 IsoMux_MVHD_Write(const ISO_MVHD_BOX_s *Mvhd, SVC_STREAM_HDLR_s *Stream);

/**
 *  Get the size of a ftyp box.
 *
 *  @return Ftyp box size
 */
UINT32 IsoMux_GetFtypSize(void);

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
UINT32 IsoMux_ParseNalUnits(UINT32 Size, UINT32 BufferOffset, const UINT8 *BufferBase, UINT32 BufferSize, SVC_VIDEO_TRACK_INFO_s *Track, SVC_ISO_VIDEO_TRACK_INFO_s *TrackInfo);

#endif
