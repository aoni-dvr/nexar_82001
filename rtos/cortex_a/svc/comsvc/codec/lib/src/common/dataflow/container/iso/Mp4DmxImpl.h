/**
 * @file Mp4DmxImpl.h
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
#ifndef MP4_DMX_IMPL_H
#define MP4_DMX_IMPL_H

#include "../demuxer/DemuxerImpl.h"
#include "../demuxer/FrameFeeder.h"
#include "Mp4Idx.h"
#include "IsoDmx.h"

/**
 *  Process of demuxing a mp4 movie.
 *
 *  @param [in] Movie Movie information
 *  @param [in] Stream Stream handler
 *  @param [in] Mp4Idx Mp4 index
 *  @param [in] FeedInfo Frame feeder information
 *  @param [in] TargetTime Target time to process
 *  @param [in] Direction Direction of process
 *  @param [in] End Indicating the end of process
 *  @param [out] Event The returned process event
 *  @return 0 - OK, others - FORMAT_ERR_XXXX
 */
UINT32 Mp4Dmx_Process(SVC_MOVIE_INFO_s *Movie, SVC_STREAM_HDLR_s *Stream, MP4_IDX_s *Mp4Idx, SVC_FRAME_FEEDER_INFO_s *FeedInfo, UINT64 TargetTime, UINT8 Direction, UINT8 End, UINT8 *Event);

/**
 *  Parse movie information of a mp4 clip.
 *
 *  @param [out] Movie Movie information
 *  @param [in] Stream Stream handler
 *  @return 0 - OK, others - FORMAT_ERR_XXXX
 */
UINT32 Mp4Dmx_Parse(SVC_MOVIE_INFO_s *Movie, SVC_STREAM_HDLR_s *Stream);

/**
 *  Open a mp4 demuxing format handler.
 *
 *  @param [in] Hdlr Demuxing format handler
 *  @param [in] Mp4Idx Mp4 index
 *  @param [in] MaxIdxNum Maximum index number
 *  @param [in] FeedInfo Frame feeder information
 *  @return 0 - OK, others - FORMAT_ERR_XXXX
 */
UINT32 Mp4Dmx_Open(SVC_DMX_FORMAT_HDLR_s *Hdlr, MP4_IDX_s *Mp4Idx, UINT32 MaxIdxNum, SVC_FRAME_FEEDER_INFO_s *FeedInfo);

/**
 *  Close a mp4 demuxing format handler.
 *
 *  @param [in] Mp4Idx Mp4 index
 *  @return 0 - OK, others - FORMAT_ERR_XXXX
 */
UINT32 Mp4Dmx_Close(MP4_IDX_s *Mp4Idx);

/**
 *  Seek and set the start time of demuxing.
 *
 *  @param [in] Hdlr Demuxing format handler
 *  @param [in] Mp4Idx Mp4 index
 *  @param [in] FeedInfo Frame feeder information
 *  @param [in] TargetTime Target time to process
 *  @param [in] Direction Direction of process
 *  @param [in] Speed Processing speed after seeking
 *  @return 0 - OK, others - FORMAT_ERR_XXXX
 */
UINT32 Mp4Dmx_Seek(SVC_DMX_FORMAT_HDLR_s *Hdlr, MP4_IDX_s *Mp4Idx, SVC_FRAME_FEEDER_INFO_s *FeedInfo, UINT64 TargetTime, UINT32 Direction, UINT32 Speed);

/**
 *  Feeding a frame from the mp4 file.
 *
 *  @param [in] Hdlr Demuxing format handler
 *  @param [in] Mp4Idx Mp4 index
 *  @param [in] TrackId Track id
 *  @param [in] TargetTime The time of the target frame
 *  @param [in] FrameType Frame type
 *  @return 0 - OK, others - FORMAT_ERR_XXXX
 */
UINT32 Mp4Dmx_FeedFrame(SVC_DMX_FORMAT_HDLR_s *Hdlr, MP4_IDX_s *Mp4Idx, UINT8 TrackId, UINT64 TargetTime, UINT8 FrameType);

#endif

