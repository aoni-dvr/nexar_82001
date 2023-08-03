/**
 * @file SvcFmp4Dmx.h
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
#ifndef SVC_FMP4_DMX_H
#define SVC_FMP4_DMX_H

#include "format/SvcFormat.h"

/**
 * The configuration for initializing Fmp4Dmx module
 */
typedef struct {
    UINT8 *Buffer;          /**< The work buffer of the Fmp4Dmx module */
    UINT32 BufferSize;      /**< The work buffer size of the module */
    UINT8 MaxHdlr;          /**< The maximum number of Fmp4Dmx handlers */
} SVC_FMP4_DMX_INIT_CFG_s;

/**
 * The configuration of Fmp4Dmx handlers
 */
typedef struct {
    SVC_STREAM_HDLR_s *Stream;          /**< Stream handler */
} SVC_FMP4_DMX_CFG_s;

/**
 * Get the default configuration for initializing the Fmp4Dmx module.
 *
 * @param [out] Config The returned configuration
 * @return 0 - OK, others - DEMUXER_ERR_XXXX
 */
UINT32 SvcFmp4Dmx_GetInitDefaultCfg(SVC_FMP4_DMX_INIT_CFG_s *Config);

/**
 * Get the required buffer size for initializing the Fmp4Dmx module.
 *
 * @param [in] MaxHdlr The maximum number of Fmp4Dmx handlers
 * @param [in] BufferSize The work buffer size of the Fmp4Dmx module.
 * @return 0 - OK, others - DEMUXER_ERR_XXXX
 */
UINT32 SvcFmp4Dmx_GetInitBufferSize(UINT8 MaxHdlr, UINT32 *BufferSize);

/**
 * Initialize the Fmp4Dmx module.
 *
 * @param [in] Config The configuration used to initialize the module
 * @return 0 - OK, others - DEMUXER_ERR_XXXX
 */
UINT32 SvcFmp4Dmx_Init(SVC_FMP4_DMX_INIT_CFG_s *Config);

/**
 * Get the default configuration of an Fmp4Dmx handler.
 *
 * @param [out] Config The returned configuration
 * @return 0 - OK, others - DEMUXER_ERR_XXXX
 */
UINT32 SvcFmp4Dmx_GetDefaultCfg(SVC_FMP4_DMX_CFG_s *Config);

/**
 * Create an Fmp4Dmx handler.
 *
 * @param [in] Config The configuration used to create an Fmp4Dmx handler
 * @param [out] Hdlr The returned Fmp4Dmx handler
 * @return 0 - OK, others - DEMUXER_ERR_XXXX
 */
UINT32 SvcFmp4Dmx_Create(SVC_FMP4_DMX_CFG_s *Config, SVC_DMX_FORMAT_HDLR_s **Hdlr);

/**
 * Delete an Fmp4Dmx handler.
 *
 * @param [in] Hdlr The Fmp4Dmx handler being deleted
 * @return 0 - OK, others - DEMUXER_ERR_XXXX
 */
UINT32 SvcFmp4Dmx_Delete(SVC_DMX_FORMAT_HDLR_s *Hdlr);

/**
 * Parse media data from a stream, and pack the data into a Media Info object. (See AMBA_DMX_FORMAT_PARSE_FP.)
 *
 * @param [in,out] Media The returned Media Info object
 * @param [in] Stream The I/O stream
 * @return 0 - OK, others - DEMUXER_ERR_XXXX
 */
UINT32 SvcFmp4Dmx_Parse(SVC_MEDIA_INFO_s *Media, SVC_STREAM_HDLR_s *Stream);

#endif
