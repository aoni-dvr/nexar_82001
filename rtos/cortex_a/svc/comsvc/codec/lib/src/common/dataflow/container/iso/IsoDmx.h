/**
 * @file IsoDmx.h
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
#ifndef ISO_DMX_H
#define ISO_DMX_H

#include "../demuxer/DemuxerImpl.h"
#include "../demuxer/FrameFeeder.h"
#include "IsoBoxDef.h"

/**
 *  Function pointer of getting the frame information
 */
typedef UINT32 (*ISODMX_GET_FRAME_INFO_FP)(void *IdxMgr, UINT8 TrackId, UINT32 FrameNo, UINT8 Direction, SVC_FRAME_INFO_s *FrameInfo);

/**
 *  Parse a ftyp box
 *
 *  @param [in] Stream Stream handler
 *  @param [out] IsoDmx ISO private information
 *  @param [in] ParseStartPos Start position of the box
 *  @param [in] ParseEndPos End position of the box
 *  @return 0 - OK, others - FORMAT_ERR_XXXX
 */
UINT32 IsoDmx_ParseFtyp(SVC_STREAM_HDLR_s *Stream, SVC_ISO_PRIV_INFO_s *IsoDmx, UINT64 ParseStartPos, UINT64 ParseEndPos);

/**
 *  Get the box header
 *
 *  @param [in] Stream Stream handler
 *  @param [out] BoxSize The size of the box
 *  @param [out] BoxTag The tag of the box
 *  @return 0 - OK, others - FORMAT_ERR_XXXX
 */
UINT32 IsoDmx_GetBoxHeader(SVC_STREAM_HDLR_s *Stream, UINT32 *BoxSize, UINT32 *BoxTag);

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
UINT32 IsoDmx_ParseMoov(SVC_MEDIA_INFO_s *Media, SVC_STREAM_HDLR_s *Stream, SVC_ISO_PRIV_INFO_s *IsoDmx, UINT64 ParseStartPos, UINT64 ParseEndPos, UINT8 IsFmp4);

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
UINT32 IsoDmx_Process(SVC_MEDIA_INFO_s *Media, SVC_STREAM_HDLR_s *Stream, void *IsoIdx, SVC_FRAME_FEEDER_INFO_s *FeedInfo, UINT64 TargetTime, UINT8 Direction, UINT8 End, UINT8 *Event);

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
UINT32 IsoDmx_InitFeedInfo(SVC_MOVIE_INFO_s *Movie, SVC_FRAME_FEEDER_INFO_s *FeedInfo, ISODMX_GET_FRAME_INFO_FP GetFrameInfo, UINT8 Direction, UINT8 Speed);

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
UINT32 IsoDmx_Seek(SVC_DMX_FORMAT_HDLR_s *Hdlr, void *IsoIdx, SVC_FRAME_FEEDER_INFO_s *FeedInfo, UINT64 TargetTime, UINT32 Direction, UINT32 Speed, ISODMX_GET_FRAME_INFO_FP GetFrameInfo);

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
UINT32 IsoDmx_FeedFrame(SVC_DMX_FORMAT_HDLR_s *Hdlr, void *IsoIdx, UINT8 TrackId, UINT64 TargetTime, UINT8 FrameType, ISODMX_GET_FRAME_INFO_FP GetFrameInfo);

#endif
