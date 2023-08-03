/**
 * @file SvcFmp4Mux.h
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
#ifndef SVC_FMP4_MUX_H
#define SVC_FMP4_MUX_H

/**
 * FMP4 Muxing/Demuxing Format implementation
 *
 * The implementation of Fragmented MP4 Muxing/Demuxing Format module
 * Users can use Fmp4Mux to add data into the movie, use Fmp4Dmx to parse data from a movie
 *
 * The Fmp4Mux/Fmp4Dmx module includes the following functions:
 * 1. Initiate the Fmp4Mux/Fmp4Dmx module
 * 2. Create Fmp4Mux/Fmp4Dmx handlers
 * 3. Delete Fmp4Mux/Fmp4Dmx handlers
 */

#include "format/SvcFormat.h"

/**
 *  The configuration for initializing Fmp4Mux module
 */
typedef struct {
    UINT8 *Buffer;      /**< The work buffer of the Fmp4Mux module */
    UINT32 BufferSize;  /**< The work buffer size of the module */
    UINT8 MaxHdlr;      /**< The maximum number of Fmp4Mux handlers */
} SVC_FMP4_MUX_INIT_CFG_s;

/**
 *  The configuration of Fmp4Mux handlers
 */
typedef struct {
    SVC_STREAM_HDLR_s *Stream;  /**< Stream handler */
    UINT8 *UserData;            /**< The user data buffer. */
                                /**< If stream allows DMA copy and the size exceeds AMBA_CFS_DMA_SIZE_THRESHOLD, */
                                /**< the buffer should be non-cached. */
    UINT32 UserDataSize;        /**< The size of user data buffer */

    UINT8  EnableMehd;
    UINT32 MehdDefDuration;
    UINT32 MvhdTimeScale;
} SVC_FMP4_MUX_CFG_s;

/**
 *  Get the required buffer size for initializing the Fmp4Mux module.
 *
 *  @param [in] MaxHdlr The maximum number of Fmp4Mux handlers
 *  @param [out] BufferSize The required buffer size.
 *  @return 0 - OK, others - MUXER_ERR_XXXX
 */
UINT32 SvcFmp4Mux_GetInitBufferSize(UINT8 MaxHdlr, UINT32 *BufferSize);

/**
 *  Get the default configuration for initializing the Fmp4Mux module.
 *
 *  @param [out] Config The returned configuration
 *  @return 0 - OK, others - MUXER_ERR_XXXX
 */
UINT32 SvcFmp4Mux_GetInitDefaultCfg(SVC_FMP4_MUX_INIT_CFG_s *Config);

/**
 *  Initialize the Fmp4Mux module.
 *
 *  @param [in] Config The configuration used to initialize the module
 *  @return 0 - OK, others - MUXER_ERR_XXXX
 */
UINT32 SvcFmp4Mux_Init(const SVC_FMP4_MUX_INIT_CFG_s *Config);

/**
 *  Get the default configuration of a Fmp4Mux handler.
 *
 *  @param [out] Config The returned configuration
 *  @return 0 - OK, others - MUXER_ERR_XXXX
 */
UINT32 SvcFmp4Mux_GetDefaultCfg(SVC_FMP4_MUX_CFG_s *Config);

/**
 *  Create an Fmp4Mux handler.
 *
 *  @param [in] Config The configuration used to create an Fmp4Mux handler
 *  @param [out] Hdlr The returned Fmp4Mux handler
 *  @return 0 - OK, others - MUXER_ERR_XXXX
 */
UINT32 SvcFmp4Mux_Create(const SVC_FMP4_MUX_CFG_s *Config, SVC_MUX_FORMAT_HDLR_s **Hdlr);

/**
 *  Delete a Fmp4Mux handler.
 *
 *  @param [in] Hdlr The Fmp4Mux handler being deleted
 *  @return 0 - OK, others - MUXER_ERR_XXXX
 */
UINT32 SvcFmp4Mux_Delete(SVC_MUX_FORMAT_HDLR_s *Hdlr);

#endif
