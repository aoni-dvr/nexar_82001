/**
 * @file DemuxerImpl.h
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
#ifndef DEMUXERIMPL_H
#define DEMUXERIMPL_H

#include "format/SvcDemuxer.h"

#define DEMUXER_OK                  OK                  /**< Execution OK */
#define DEMUXER_ERR_INVALID_ARG     DEMUXER_ERR_0000    /**< Invalid argument */
#define DEMUXER_ERR_FATAL_ERROR     DEMUXER_ERR_0001    /**< Fatal error*/
#define DEMUXER_ERR_IO_ERROR        DEMUXER_ERR_0002    /**< IO error */

/**
 * The state of Demuxer pipe (in UINT8)
 */
#define SVC_DEMUXER_STATE_IDLE             (0x00U) /**< The state to indicate that the Demuxer pipe is idle */
#define SVC_DEMUXER_STATE_START            (0x01U) /**< The state to indicate that the Demuxer pipe starts demuxing */
#define SVC_DEMUXER_STATE_RUNNING          (0x02U) /**< The state to indicate that the Demuxer pipe is running */
#define SVC_DEMUXER_STATE_STOPPING         (0x03U) /**< The state to indicate that the Demuxer pipe is changing its state from running to stop */
#define SVC_DEMUXER_STATE_END              (0x10U) /**< The state to indicate that the Demuxer pipe is stopped */
#define SVC_DEMUXER_STATE_ERROR            (0xF0U) /**< The state to indicate that the Demuxer pipe has encountered an error */

static inline UINT32 W2D(UINT32 Ret)
{
    UINT32 Rval = DEMUXER_OK;
    if (Ret != OK) {
        Rval = DEMUXER_ERR_FATAL_ERROR;
    }
    return Rval;
}

static inline UINT32 K2D(UINT32 Ret)
{
    UINT32 Rval = DEMUXER_OK;
    if (Ret != OK) {
        Rval = DEMUXER_ERR_FATAL_ERROR;
    }
    return Rval;
}

static inline UINT32 S2D(UINT32 Ret)
{
    UINT32 Rval = DEMUXER_OK;
    if (Ret != OK) {
        if (Ret == STREAM_ERR_0000) {
            Rval = DEMUXER_ERR_INVALID_ARG;
        } else if (Ret == STREAM_ERR_0002) {
            Rval = DEMUXER_ERR_IO_ERROR;
        } else {
            Rval = DEMUXER_ERR_FATAL_ERROR;
        }
    }
    return Rval;
}

/**
 * The implementation of a demuxer pipe
 */
typedef struct {
    SVC_DEMUXER_PIPE_HDLR_s Hdlr;  /**< The handler used by external API */
    AMBA_KAL_MUTEX_t Mutex;         /**< Mutex */
    UINT8 Used;                     /**< Used flag */
    UINT8 State;                    /**< The state (SVC_DEMUXER_STATE_IDLE, SVC_DEMUXER_STATE_START, ...) */
    UINT8 FormatCount;              /**< The number of Format handlers in a pipe */
    SVC_DMX_FORMAT_HDLR_s *Format[SVC_DEMUXER_MAX_FORMAT_PER_PIPE];   /**< Format handlers in a pipe (See SVC_DMX_FORMAT_HDLR_s.) */
} SVC_DEMUXER_PIPE_HDLR_IMPL_s;

/**
 * Update information of the track
 *
 * @param [in] Track The track information
 * @param [in] FrameCount The frame counter
 * @param [in] Offset Offset of DTS
 */
void SvcDemuxer_UpdateTrack(SVC_MEDIA_TRACK_GENERAL_INFO_s *Track, INT32 FrameCount, INT32 Offset);

/**
 * Print out the pipe information
 *
 * @param [in] Pipe pipe The demuxer pipe
 * @param [in] Prefix The prefix string
 */
void SvcDemuxer_PrintPipeInfo(SVC_DEMUXER_PIPE_HDLR_IMPL_s *Pipe, const char *Prefix);

/**
 * Start the demuxer pipe
 *
 * @param [in] Pipe The demuxer pipe
 * @return 0 - OK, others - DEMUXER_ERR_XXXX
 */
UINT32 SvcDemuxer_StartPipe(SVC_DEMUXER_PIPE_HDLR_IMPL_s *Pipe);

/**
 * Entry when pipe end
 *
 * @param [in] Pipe The demuxer pipe
 * @return 0 - OK, others - DEMUXER_ERR_XXXX
 */
UINT32 SvcDemuxer_OnPipeEnd(SVC_DEMUXER_PIPE_HDLR_IMPL_s *Pipe);

/**
 * Entry when pipe got errors
 *
 * @param [in] Pipe The demuxer pipe
 * @return 0 - OK, others - DEMUXER_ERR_XXXX
 */
UINT32 SvcDemuxer_OnPipeError(SVC_DEMUXER_PIPE_HDLR_IMPL_s *Pipe);

/**
 * Initialize the related parameters of movie object
 *
 * @param [in] Format The format handler
 * @param [in] Func The interface of format operations
 * @param [in] Stream The stream handler
 */
void SvcDemuxer_InitMovieDmxHdlr(SVC_DMX_FORMAT_HDLR_s *Format, SVC_DMX_FORMAT_s *Func, SVC_STREAM_HDLR_s *Stream);

/**
 * Initialize the related parameters of image object
 *
 * @param [in] Format The format handler
 * @param [in] Func The interface of format operations
 * @param [in] Stream The stream handler
 */
void SvcDemuxer_InitImageDmxHdlr(SVC_DMX_FORMAT_HDLR_s *Format, SVC_DMX_FORMAT_s *Func, SVC_STREAM_HDLR_s *Stream);

/**
 * Process function of the demuxer pipe (Movie)
 *
 * @param [in] Pipe The demuxer pipe
 * @param [in] TargetTime The target time of the media
 * @param [out] Event The event got for this processing.
 * @return 0 - OK, others - DEMUXER_ERR_XXXX
 */
UINT32 SvcDemuxer_ProcessMovie(SVC_DEMUXER_PIPE_HDLR_IMPL_s *Pipe, UINT64 TargetTime, UINT8 *Event);

/**
 * Process function of the demuxer pipe (Image)
 *
 * @param [in] Pipe The demuxer pipe
 * @param [in] FrameNo The format handler
 * @param [out] Event The event got for this processing
 * @return 0 - OK, others - DEMUXER_ERR_XXXX
 */
UINT32 SvcDemuxer_ProcessImage(SVC_DEMUXER_PIPE_HDLR_IMPL_s *Pipe, UINT8 FrameNo, UINT8 *Event);

#endif

