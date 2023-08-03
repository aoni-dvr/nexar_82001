/**
 * @file IndexCache.h
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
#ifndef INDEX_CACHE_H
#define INDEX_CACHE_H

#define INDEX_MODE_WRITE        (0U)    /**< Write only mode */
#define INDEX_MODE_READ_INDEX   (1U)    /**< Read index mode (read index data from an Index file) */
#define INDEX_MODE_READ_STREAM  (2U)    /**< Read stream mode (read index data from a file stream) */

#define INDEX_CACHE_MAX_ITEM_PER_TRACK  (8U)    /**< The max number of the items of the index implementation. */
#define INDEX_CACHE_MAX_CACHED_COUNT    (512U)  /**< at least two GOP, and a GOP's size cannot be larger than 255 */

/**
 *  Index Cache Item Configuration
 */
typedef struct {
    UINT8 BytePerIndex; /**< Byte per index */
    UINT32 IndexCount;  /**< Index count */
    UINT64 FileOffset;  /**< File offset */
} INDEX_CACHE_ITEM_CFG_s;

/**
 *  Index Cache Track Configuration
 */
typedef struct {
    UINT8 ItemCount;        /**< Item count */
    UINT32 MaxCachedIndex;  /**< Max cached index */
    INDEX_CACHE_ITEM_CFG_s Item[INDEX_CACHE_MAX_ITEM_PER_TRACK];    /**< Items configuration */
} INDEX_CACHE_TRACK_CFG_s;

/**
 *  Index Cache Configuration
 */
typedef struct {
    UINT8 Mode;                 /**< Mode */
    UINT8 TrackCount;           /**< Track count */
    SVC_STREAM_HDLR_s *Stream;  /**< Stream handler */
    INDEX_CACHE_TRACK_CFG_s Track[SVC_FORMAT_MAX_TRACK_PER_MEDIA];  /**< Index cache tracks configuration */
} INDEX_CACHE_CFG_s;

/**
 *  Index Cache Item Information
 */
typedef struct {
    UINT8 BytePerIndex;                     /**< Byte per index */
    UINT64 Buffer[INDEX_CACHE_MAX_CACHED_COUNT];    /**< Index Buffer (CO64 needs UINT64) */
    UINT32 Index;                           /**< Index Buffer unit read/write number */
    UINT32 IndexCount;                      /**< Total index number (FrameCount) */
    UINT32 IndexStart;                      /**< Index Buffer read/write start number */
    UINT32 IndexEnd;                        /**< Index Buffer read/write end number */
    UINT64 FileOffset;                      /**< Index File Offset */
} INDEX_CACHE_ITEM_INFO_s;

/**
 * Index Cache Track Information
 */
typedef struct {
    UINT8 ItemCount;        /**< Item count */
    UINT32 MaxCachedIndex;  /**< Max cached index */
    INDEX_CACHE_ITEM_INFO_s Item[INDEX_CACHE_MAX_ITEM_PER_TRACK];   /**< Index cache item information */
} INDEX_CACHE_TRACK_INFO_s;

/**
 * Index Cache Handler
 */
typedef struct {
    UINT8 Mode;                 /**< Mode */
    UINT8 TrackCount;           /**< Track count */
    SVC_STREAM_HDLR_s *Stream;  /**< Stream handler */
    INDEX_CACHE_TRACK_INFO_s Track[SVC_FORMAT_MAX_TRACK_PER_MEDIA]; /**< Track information */
} INDEX_CACHE_HDLR_s;

/**
 * Create an IndexCache handler.
 *
 * @param [in] Hdlr IndexCache handler
 * @param [in] Config Configuration
 * @return 0 - OK, others - FORMAT_ERR_XXXX
 */
UINT32 IndexCache_Create(INDEX_CACHE_HDLR_s *Hdlr, const INDEX_CACHE_CFG_s *Config);

/**
 * Delete an IndexCache handler.
 *
 * @param [in] Hdlr IndexCache handler
 * @return 0 - OK, others - FORMAT_ERR_XXXX
 */
UINT32 IndexCache_Delete(INDEX_CACHE_HDLR_s *Hdlr);

/**
 * Read from IndexCache handler.
 *
 * @param [in] Hdlr IndexCache handler
 * @param [in] TrackId Track id
 * @param [in] ItemId Item id
 * @param [in] Index Index
 * @param [in] Direction Direction (SVC_FORMAT_DIR_FORWARD / SVC_FORMAT_DIR_BACKWARD)
 * @param [in] Data Destination buffer
 * @return 0 - OK, others - FORMAT_ERR_XXXX
 */
UINT32 IndexCache_Read(INDEX_CACHE_HDLR_s *Hdlr, UINT8 TrackId, UINT8 ItemId, UINT32 Index, UINT8 Direction, void *Data);

#endif

