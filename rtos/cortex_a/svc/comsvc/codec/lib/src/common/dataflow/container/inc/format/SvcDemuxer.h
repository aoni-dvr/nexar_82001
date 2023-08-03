/**
 * @file SvcDemuxer.h
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
#ifndef DEMUXER_H
#define DEMUXER_H

/**
 * Demuxer flow implementation
 *
 * The implementation of the Demuxer module and Demuxer handler.
 * The primary function of the Demuxer module is to handle Demuxer pipes.
 * Each Demuxer pipe includes multiple formats and their media information objects.
 * Demuxer module will process the Demuxer pipes which are added to it.
 *
 * The Demuxer module includes the following functions:
 * 1. Initialize the Demuxer module
 * 2. Create a Demuxer pipe
 * 3. Delete a Demuxer pipe
 * 4. Add a Demuxer pipe to Demuxer
 * 5. Remove a Demuxer pipe from Demuxer
 * 6. Other Demuxer related functions
 */

#include "SvcFIFO.h"
#include "format/SvcFormat.h"

#define SVC_DEMUXER_MAX_FORMAT_PER_PIPE     (2U)    /**< The number of Format handlers in a pipe */
#define SVC_DEMUXER_MAX_PIPE                (16U)   /**< The maximum number of pipes held in the Demuxer module (The value includes the number of new tasks.) */


/**
 * Demuxer event
 */
#define SVC_DEMUXER_EVENT_NONE              (0x00U) /**< No event */
#define SVC_DEMUXER_EVENT_REACH_END         (0x01U) /**< The event raised when Demuxer completes a demuxing */

#define SVC_DEMUXER_FORMAT_TYPE_MP4         (0U)    /**< MP4 container */
#define SVC_DEMUXER_FORMAT_TYPE_FMP4        (1U)    /**< FMP4 container */

/**
 * The configuration for initializing the Demuxer module
 */
typedef struct {
    UINT8 *Buffer;                  /**< The work buffer of the Demuxer module */
    UINT32 BufferSize;              /**< The size of the work buffer */
    UINT8 MaxPipe;                  /**< The maximum number of pipes held in the Demuxer module */
} SVC_DEMUXER_INIT_CFG_s;

/**
 * The configuration for initializing a Demuxer pipe
 */
typedef struct {
    SVC_DMX_FORMAT_HDLR_s *Format[SVC_DEMUXER_MAX_FORMAT_PER_PIPE]; /**< The Format handlers in a pipe (See SVC_DMX_FORMAT_HDLR_s.) */
    SVC_MEDIA_INFO_s *Media[SVC_DEMUXER_MAX_FORMAT_PER_PIPE];       /**< The Media Info objects in a pipe (See SVC_MEDIA_INFO_s.) */
    UINT8 FormatCount;      /**< The number of Format handlers in a pipe */
    UINT8 Speed;            /**< Demuxing speed (e.g., 1, 2, 4, 8, and 16) */
} SVC_DEMUXER_PIPE_CFG_s;

/**
 * Demuxer pipe handler
 */
typedef struct {
    UINT8 PipeId;   /**< The pipe ID */
    UINT8 Resv[3];  /**< Reserved */
} SVC_DEMUXER_PIPE_HDLR_s;

/**
 * Get the default configuration for initializing the Demuxer module.
 *
 * @param [out] Config The returned configuration
 * @return 0 - OK, others - DEMUXER_ERR_XXXX
 */
UINT32 SvcDemuxer_GetInitDefaultCfg(SVC_DEMUXER_INIT_CFG_s *Config);

/**
 * Get the required buffer size for initializing the Demuxer module.
 *
 * @param [in] MaxPipe The maximum number of Demuxer pipes
 * @param [out] BufferSize The required working buffer size
 * @return 0 - OK, others - DEMUXER_ERR_XXXX
 */
UINT32 SvcDemuxer_GetInitBufferSize(UINT8 MaxPipe, UINT32 *BufferSize);

/**
 * Initialize the Demuxer module.
 *
 * @param [in] Config The configuration used to initialize the Demuxer module
 * @return 0 - OK, others - DEMUXER_ERR_XXXX
 */
UINT32 SvcDemuxer_Init(SVC_DEMUXER_INIT_CFG_s *Config);

/**
 * Get the default configuration for creating Demuxer pipes.
 *
 * @param [out] Config The returned configuration
 * @return 0 - OK, others - DEMUXER_ERR_XXXX
 */
UINT32 SvcDemuxer_GetDefaultCfg(SVC_DEMUXER_PIPE_CFG_s *Config);

/**
 * Create a Demuxer pipe.
 *
 * @param [in] Config The configuration used to create a Demuxer pipe
 * @param [out] Pipe The created pipe
 * @return 0 - OK, others - DEMUXER_ERR_XXXX
 * @note Media types of Demuxer pipes indicate the kinds of media that will be processed. An image type
 * cannot appear concurrently with any other types; however, movie and sound types can appear with each other.
 */
UINT32 SvcDemuxer_Create(SVC_DEMUXER_PIPE_CFG_s *Config, SVC_DEMUXER_PIPE_HDLR_s **Pipe);

/**
 * Delete a Demuxer pipe.
 *
 * @param [in] Pipe The Demuxer pipe being deleted
 * @return 0 - OK, others - DEMUXER_ERR_XXXX
 */
UINT32 SvcDemuxer_Delete(SVC_DEMUXER_PIPE_HDLR_s *Pipe);

/**
 * Start a Demuxer pipe.
 *
 * @param [in] Pipe The Demuxer pipe being started
 * @return 0 - OK, others - DEMUXER_ERR_XXXX
 */
UINT32 SvcDemuxer_Start(SVC_DEMUXER_PIPE_HDLR_s *Pipe);

/**
 * Stop a Demuxer pipe.
 *
 * @param [in] Pipe The Demuxer pipe being stopped
 * @return 0 - OK, others - DEMUXER_ERR_XXXX
 */
UINT32 SvcDemuxer_Stop(SVC_DEMUXER_PIPE_HDLR_s *Pipe);

/**
 * Process a Demuxer pipe.
 *
 * @param [in] Pipe The Demuxer pipe being stopped
 * @param [in] FrameNumber The number of frames is processed in the default track.
 * @param [out] Event The event happened in this process
 * @return 0 - OK, others - DEMUXER_ERR_XXXX
 */
UINT32 SvcDemuxer_Process(SVC_DEMUXER_PIPE_HDLR_s *Pipe, UINT8 FrameNumber, UINT8 *Event);

/**
 * Seek and set the start time of demuxing.
 *
 * @param [in] Pipe The Demuxer pipe
 * @param [in] TargetTime The reference time being sought
 * @param [in] Direction Seek direction
 * @param [in] Speed The demuxing speed after seeking.
 * @return 0 - OK, others - DEMUXER_ERR_XXXX
 */
UINT32 SvcDemuxer_Seek(SVC_DEMUXER_PIPE_HDLR_s *Pipe, UINT32 TargetTime, UINT8 Direction, UINT32 Speed);

/**
 * Feed a frame into a FIFO.
 *
 * @param [in] Format The Format handler
 * @param [in] TrackId The ID of a track that the new frame is fed into its FIFO
 * @param [in] TargetTime The reference time of the frame
 * @param [in] FrameType The type of the frame
 * @return SVC_FORMAT_EVENT_e
 */
UINT32 SvcDemuxer_FeedFrame(SVC_DMX_FORMAT_HDLR_s *Format, UINT8 TrackId, UINT32 TargetTime, UINT8 FrameType);

/**
 * Get the type of a container format.
 *
 * @param [in] Stream The I/O stream
 * @param [out] FormatType The returned format type
 * @return 0 - OK, others - DEMUXER_ERR_XXXX
 */
UINT32 SvcDemuxer_GetFormatType(SVC_STREAM_HDLR_s *Stream, UINT8 *FormatType);

#endif /* DEMUXER_H_ */
