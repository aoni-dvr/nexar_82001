/**
 * @file SvcExifDmx.h
 *
 * Copyright (c) 2020 Ambarella International LP
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
#ifndef SVC_EXIF_DMX_H
#define SVC_EXIF_DMX_H

#include "format/SvcFormat.h"

/**
 * The configuration for initializing the ExifDmx module
 */
typedef struct {
    UINT8 *Buffer;          /**< The work buffer of the ExifDmx module */
    UINT32 BufferSize;      /**< The size of the work buffer */
    UINT8 MaxHdlr;          /**< The maximum number of ExifDmx handlers */
} SVC_EXIF_DMX_INIT_CFG_s;

/**
 * The configuration of ExifDmx handlers
 */
typedef struct {
    SVC_STREAM_HDLR_s *Stream;  /**< Stream handler */
} SVC_EXIF_DMX_CFG_s;

/**
 * Get the required buffer size for initializing the ExifDmx module.
 * @param [in] MaxHdlr The maximum number of ExifDmx handlers
 * @param [out] BufferSize The work buffer size of the Fmp4Dmx module.
 * @return 0 - OK, others - DEMUXER_ERR_XXXX
 */
UINT32 SvcExifDmx_GetInitBufferSize(UINT8 MaxHdlr, UINT32 *BufferSize);

/**
 * Get the default configuration for initializing the ExifDmx module.
 * @param [out] Config The returned configuration
 * @return 0 - OK, others - DEMUXER_ERR_XXXX
 */
UINT32 SvcExifDmx_GetInitDefaultCfg(SVC_EXIF_DMX_INIT_CFG_s *Config);

/**
 * Initialize the ExifDmx module.
 * @param [in] Config The configuration used to initialize the module
 * @return 0 - OK, others - DEMUXER_ERR_XXXX
 */
UINT32 SvcExifDmx_Init(const SVC_EXIF_DMX_INIT_CFG_s *Config);

/**
 * Get the default configuration of an ExifDmx handler.
 * @param [out] Config The returned configuration
 * @return 0 - OK, others - DEMUXER_ERR_XXXX
 */
UINT32 SvcExifDmx_GetDefaultCfg(SVC_EXIF_DMX_CFG_s *Config);

/**
 * Create an ExifDmx handler.
 * @param [in] Config The configuration used to create an ExifDmx handler
 * @param [out] Hdlr The returned ExifDmx handler
 * @return 0 - OK, others - DEMUXER_ERR_XXXX
 */
UINT32 SvcExifDmx_Create(const SVC_EXIF_DMX_CFG_s *Config, SVC_DMX_FORMAT_HDLR_s **Hdlr);

/**
 * Delete an ExifDmx handler.
 * @param [in] Hdlr The ExifDmx handler being deleted
 * @return 0 - OK, others - DEMUXER_ERR_XXXX
 */
UINT32 SvcExifDmx_Delete(SVC_DMX_FORMAT_HDLR_s *Hdlr);

/**
 * Parse media data from a I/O stream, and pack the data into a Media Info object.
 * @param [in,out] Media The returned Media Info object
 * @param [in] Stream The I/O stream
 * @return 0 - OK, others - DEMUXER_ERR_XXXX
 */
UINT32 SvcExifDmx_Parse(SVC_MEDIA_INFO_s *Media, SVC_STREAM_HDLR_s *Stream);

#endif /* SVC_EXIF_DMX_H */
