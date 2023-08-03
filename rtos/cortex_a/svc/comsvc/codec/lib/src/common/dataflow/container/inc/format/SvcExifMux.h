/**
 * @file SvcExifMux.h
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
#ifndef SVC_EXIF_MUX_H
#define SVC_EXIF_MUX_H

/**
 * EXIF Muxing/Demuxing Format module implementation
 *
 * The implementation of EXIF Muxing/Demuxing Format module
 * Users can use ExifMux to add data into the picture and use ExifDmx to parse data from a picture.
 *
 * The ExifMux/ExifDmx module include below function implementation:
 * 1. Initialize ExifMux/ExifDmx
 * 2. Create ExifMux/ExifDmx handler
 * 3. Delete ExifMux/ExifDmx handler
 */

#include "format/SvcFormat.h"

/**
 * The configuration for initializing the ExifMux module
 */
typedef struct {
    UINT8 *Buffer;          /**< The work buffer of the ExifMux module */
    UINT8 *HeaderBuffer;    /**< The header buffer of the ExifMux module. */
                            /**< If stream allows DMA copy and the size exceeds AMBA_CFS_DMA_SIZE_THRESHOLD, */
                            /**< the buffer should be non-cached. */
    UINT32 BufferSize;      /**< The size of the work buffer */
    UINT32 HeaderBufferSize;/**< The header size of the module */
    UINT8 MaxHdlr;          /**< The maximum number of ExifMux handlers */
} SVC_EXIF_MUX_INIT_CFG_s;

/**
 * Exif tag configuration
 */
typedef struct {
    UINT8 *Data;        /**< Data */
    UINT32 Value;       /**< Value */
    UINT32 Count;       /**< Count */
    UINT16 Tag;         /**< Tag */
    UINT16 Type;        /**< Tag type */
    UINT8 Set;          /**< Set */
} SVC_CFG_TAG_s;

/**
 * Exif tag information
 */
typedef struct {
    UINT16 Ifd0Tags;            /**< The number of Ifd0 tags */
    UINT16 ExifIfdTags;         /**< The number of ExifIfd tags */
    UINT16 IntIfdTags;          /**< The number of IntIfd tags */
    UINT16 Ifd1Tags;            /**< The number of Ifd1 tags */
    UINT16 GpsIfdTags;          /**< The number of GPSIfd tags */
    SVC_CFG_TAG_s Ifd0[SVC_IFD0_TOTAL_TAGS];    /**< Ifd0 tags (See SVC_CFG_TAG_s.) */
    SVC_CFG_TAG_s ExifIfd[SVC_EXIF_TOTAL_TAGS]; /**< ExtIfd tags (See SVC_CFG_TAG_s.) */
    SVC_CFG_TAG_s IntIfd[SVC_IntIFD_TOTAL_TAGS];/**< IntIfd (See SVC_CFG_TAG_s.) */
    SVC_CFG_TAG_s Ifd1[SVC_IFD1_TOTAL_TAGS];    /**< Ifd1 tags (See SVC_CFG_TAG_s.) */
    SVC_CFG_TAG_s GpsIfd[SVC_GPS_TOTAL_TAGS];   /**< GPSIfd tags (See SVC_CFG_TAG_s.) */
} SVC_CFG_TAG_INFO_s;

/**
 * The configuration of Exif muxer handlers
 */
typedef struct {
    SVC_STREAM_HDLR_s *Stream;          /**< Stream handler */
    SVC_CFG_TAG_INFO_s SetTagInfo;      /**< Tag information */
    UINT8 Endian;                       /**< The value indicating that the Exif header is big endian or little endian (big endian: 0x00, little endian: 0x01) */
} SVC_EXIF_MUX_CFG_s;

/**
 * Get the required buffer size for initializing the ExifMux module.
 * @param [in] MaxHdlr The maximum number of ExifMux handlers
 * @param [in] HeaderBufferSize The size of the Exif header buffer for all handlers
 * @param [out] BufferSize The required buffer size.
 * @return 0 - OK, others - MUXER_ERR_XXXX
 */
UINT32 SvcExifMux_GetInitBufferSize(UINT8 MaxHdlr, UINT32 HeaderBufferSize, UINT32 *BufferSize);

/**
 * Get the default configuration for initializing the ExifMux module.
 * @param [out] Config The returned configuration
 * @return 0 - OK, others - MUXER_ERR_XXXX
 */
UINT32 SvcExifMux_GetInitDefaultCfg(SVC_EXIF_MUX_INIT_CFG_s *Config);

/**
 * Initialize the ExifMux module.
 * @param [in] Config The configuration used to initialize the module
 * @return 0 - OK, others - MUXER_ERR_XXXX
 * */
UINT32 SvcExifMux_Init(const SVC_EXIF_MUX_INIT_CFG_s *Config);

/**
 * Get the default configuration of an ExifMux handler.
 * @param [out] Config The returned configuration
 * @return 0 - OK, others - MUXER_ERR_XXXX
 */
UINT32 SvcExifMux_GetDefaultCfg(SVC_EXIF_MUX_CFG_s *Config);

/**
 * Create an ExifMux handler.
 * @param [in] Config The configuration used to create an ExifMux handler
 * @param [out] Hdlr The returned ExifMux handler
 * @return 0 - OK, others - MUXER_ERR_XXXX
 */
UINT32 SvcExifMux_Create(const SVC_EXIF_MUX_CFG_s *Config, SVC_MUX_FORMAT_HDLR_s **Hdlr);

/**
 * Delete an ExifMux handler.
 * @param [in] Hdlr The ExifMux handler being deleted
 * @return 0 - OK, others - MUXER_ERR_XXXX
 */
UINT32 SvcExifMux_Delete(SVC_MUX_FORMAT_HDLR_s *Hdlr);

#endif /* SVC_EXIF_MUX_H */
