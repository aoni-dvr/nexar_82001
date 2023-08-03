/**
 * @file Fmp4DmxImpl.h
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
#ifndef FMP4_DMX_IMPL_H
#define FMP4_DMX_IMPL_H

#include "../demuxer/DemuxerImpl.h"
#include "../demuxer/FrameFeeder.h"
#include "Fmp4Idx.h"
#include "IsoDmx.h"

/**
 *  Process of demuxing a fmp4 movie.
 *
 *  @param [in] Movie Movie information
 *  @param [in] Stream Stream handler
 *  @param [in] Fmp4Idx Fmp4 index
 *  @param [in] FeedInfo Frame feeder information
 *  @param [in] TargetTime Target time to process
 *  @param [in] Direction Direction of process
 *  @param [in] End Indicating the end of process
 *  @param [out] Event The returned process event
 *  @return 0 - OK, others - FORMAT_ERR_XXXX
 */
UINT32 Fmp4Dmx_Process(SVC_MOVIE_INFO_s *Movie, SVC_STREAM_HDLR_s *Stream, FRAGMENT_IDX_s *Fmp4Idx, SVC_FRAME_FEEDER_INFO_s *FeedInfo, UINT64 TargetTime, UINT8 Direction, UINT8 End, UINT8 *Event);

/**
 *  Parse movie information of a fmp4 clip.
 *
 *  @param [out] Movie Movie information
 *  @param [in] Stream Stream handler
 *  @return 0 - OK, others - FORMAT_ERR_XXXX
 */
UINT32 Fmp4Dmx_Parse(SVC_MOVIE_INFO_s *Movie, SVC_STREAM_HDLR_s *Stream);

/**
 *  Open a fmp4 demuxing format handler.
 *
 *  @param [in] Hdlr Demuxing format handler
 *  @param [in] Fmp4Idx Fmp4 index
 *  @param [in] FeedInfo Frame feeder information
 *  @return 0 - OK, others - FORMAT_ERR_XXXX
 */
UINT32 Fmp4Dmx_Open(SVC_DMX_FORMAT_HDLR_s *Hdlr, FRAGMENT_IDX_s *Fmp4Idx, SVC_FRAME_FEEDER_INFO_s *FeedInfo);

/**
 *  Close a fmp4 demuxing format handler.
 *
 *  @param [in] Fmp4Idx Fmp4 index
 *  @return 0 - OK, others - FORMAT_ERR_XXXX
 */
UINT32 Fmp4Dmx_Close(FRAGMENT_IDX_s *Fmp4Idx);

/**
 *  Seek and set the start time of demuxing.
 *
 *  @param [in] Hdlr Demuxing format handler
 *  @param [in] Fmp4Idx Fmp4 index
 *  @param [in] FeedInfo Frame feeder information
 *  @param [in] TargetTime Target time to process
 *  @param [in] Direction Direction of process
 *  @param [in] Speed Processing speed after seeking
 *  @return 0 - OK, others - FORMAT_ERR_XXXX
 */
UINT32 Fmp4Dmx_Seek(SVC_DMX_FORMAT_HDLR_s *Hdlr, FRAGMENT_IDX_s *Fmp4Idx, SVC_FRAME_FEEDER_INFO_s *FeedInfo, UINT64 TargetTime, UINT32 Direction, UINT32 Speed);

/**
 *  Feeding a frame from the fmp4 file.
 *
 *  @param [in] Hdlr Demuxing format handler
 *  @param [in] Fmp4Idx Fmp4 index
 *  @param [in] TrackId Track id
 *  @param [in] TargetTime The time of the target frame
 *  @param [in] FrameType Frame type
 *  @return 0 - OK, others - FORMAT_ERR_XXXX
 */
UINT32 Fmp4Dmx_FeedFrame(SVC_DMX_FORMAT_HDLR_s *Hdlr, FRAGMENT_IDX_s *Fmp4Idx, UINT8 TrackId, UINT64 TargetTime, UINT8 FrameType);

#endif

