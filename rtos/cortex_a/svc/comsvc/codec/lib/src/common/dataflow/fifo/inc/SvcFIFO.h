/**
 * @file SvcFIFO.h
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
#ifndef SVC_FIFO_H
#define SVC_FIFO_H

/**
 * Bitstream descriptor manager
 * FIFO is used to manage bits buffer descriptors,
 * providing a way of communication between codecs and their users such as muxers/demuxers.
 * FIFO also gives users the ability of stream sharing.
 * Users could create more then one virtual fifo to read date from base fifo.
 */

#include <AmbaTypes.h>
#include <AmbaErrorCode.h>

#define SVC_FIFO_MAX_ACTIVE_FIFO    (64U)             /**< Maximum number of active FIFO handlers */
#define SVC_FIFO_MARK_EOS           (0x00FFFFFFU)     /**< The marker size of EOS frame */
#define SVC_FIFO_MARK_EOS_PAUSE     (0x00FFFFFEU)     /**< The marker size of PAUSE frame  */

/**
 * Error code
 */
#define FIFO_ERR_0000 (FIFO_ERR_BASE)           /**< Invalid argument */
#define FIFO_ERR_0001 (FIFO_ERR_BASE | 0x1U)    /**< Invalid operarion */
#define FIFO_ERR_0002 (FIFO_ERR_BASE | 0x2U)    /**< Insufficient entries in FIFO descriptor buffer */
#define FIFO_ERR_0003 (FIFO_ERR_BASE | 0x3U)    /**< FIFO descriptor buffer full */

/**
 * Fifo callback events (in UINT8)
 */
#define SVC_FIFO_EVENT_DATA_CONSUMED       (0x00U) /**< Indicate removal of entries */
#define SVC_FIFO_EVENT_DATA_READY          (0x01U) /**< Indicate that a new entry is ready */
#define SVC_FIFO_EVENT_GET_WRITE_POINT     (0x02U) /**< Get write pointer info from data consumer */
#define SVC_FIFO_EVENT_ERROR               (0x03U) /**< Error event */

#define SVC_FIFO_EVENT_WRITE_LINKED_FIFO   (0x04U) /**< Write entry event, need to be handled by SvcFIFO_ProcessEvent */
#define SVC_FIFO_EVENT_REMOVE_LINKED_FIFO  (0x05U) /**< Remove entry event, need to be handled by SvcFIFO_ProcessEvent */
#define SVC_FIFO_EVENT_RESET_FIFO          (0x06U) /**< Reset fifo event, need to be handled by SvcFIFO_ProcessEvent */

/**
 * Data type of an entry (in UINT8)
 */
#define SVC_FIFO_TYPE_MJPEG_FRAME      (0x00U) /**< MJPEG frame type */
#define SVC_FIFO_TYPE_IDR_FRAME        (0x01U) /**< IDR frame type */
#define SVC_FIFO_TYPE_I_FRAME          (0x02U) /**< I frame type */
#define SVC_FIFO_TYPE_P_FRAME          (0x03U) /**< P frame type */
#define SVC_FIFO_TYPE_B_FRAME          (0x04U) /**< B frame type */
#define SVC_FIFO_TYPE_JPEG_FRAME       (0x05U) /**< Jpeg frame type */
#define SVC_FIFO_TYPE_AUDIO_FRAME      (0x06U) /**< Audio frame type */
#define SVC_FIFO_TYPE_DECODE_MARK      (0x07U) /**< To push out all frames when feeding bitstream to dsp */
#define SVC_FIFO_TYPE_EOS              (0x08U) /**< EOS */
#define SVC_FIFO_TYPE_UNDEFINED        (0x09U) /**< Undefined type */
#define SVC_FIFO_TYPE_THUMBNAIL_FRAME  (0x0AU) /**< Jpeg thumbnail frame */
#define SVC_FIFO_TYPE_SCREENNAIL_FRAME (0x0BU) /**< Jpeg screennail frame */

/**
 * Bitstream descriptor
 */
typedef struct {
    UINT64 SeqNum;                  /**< Sequential number of bits buffer */
    UINT64 Pts;                     /**< Time stamp in ticks */
    UINT8 FrameType;                /**< Data type of the entry */
    UINT8 Completed;                /**< Specify if the buffer content is complete or not */
    UINT16 Align;                   /**< Data size alignment (in bytes, align = 2^n, n is a integer) */
    UINT8 *StartAddr;               /**< Start address of data */
    UINT32 Size;                    /**< Real data size (the size of an EOS frame is SVC_FIFO_MARK_EOS) */
    UINT64 CaptureTimeStamp;        /**< Capture time */
    UINT64 EncodeTimeStamp;         /**< Encode time */
} SVC_FIFO_BITS_DESC_s;

/**
 * Fifo callback event information
 */
typedef struct {
    UINT32 FrameNum;                /**< Number of frame removed (for SVC_FIFO_EVENT_DATA_CONSUMED) */
    SVC_FIFO_BITS_DESC_s *Desc;    /**< FIFO descriptor (for SVC_FIFO_EVENT_DATA_READY/SVC_FIFO_EVENT_GET_WRITE_POINT) */
    UINT32 ErrorCode;               /**< Error code (for SVC_FIFO_EVENT_ERROR) */
} SVC_FIFO_CALLBACK_INFO_s;

/**
 * Fifo handler
 */
typedef struct {
    UINT32 FifoId; /**< An unique id of the fifo */
} SVC_FIFO_HDLR_s;

/**
 * Fifo callback function
 */
typedef UINT32 (*SVC_FIFO_CALLBACK_f) (const SVC_FIFO_HDLR_s *Hdlr, UINT8 Event, SVC_FIFO_CALLBACK_INFO_s *Info);

/**
 * Fifo config structure
 */
typedef struct {
    UINT8 *MemoryPoolAddr;         /**< Buffer start address for fifo module including descriptor */
    UINT32 MemoryPoolSize;         /**< Size of buffer */
    UINT32 NumMaxFifo;             /**< Maximum supported fifo number */
    UINT32 NumMaxPendingEvent;     /**< Maximum number of pending events */
    SVC_FIFO_CALLBACK_f CbEvent;  /**< Callback funtion to process SVC_FIFO_EVENT_WRITE_LINKED_FIFO, SVC_FIFO_EVENT_REMOVE_LINKED_FIFO, and SVC_FIFO_EVENT_RESET_FIFO. */
} SVC_FIFO_INIT_CFG_s;

/**
 * Base fifo config
 */
typedef struct {
    UINT8 AutoRemove;   /**< If AutoRemove is set, the base FIFO will remove entries which are no longer in any of its virtual FIFOs automatically. */
                        /**< Otherwise, SvcFIFO_RemoveEntry must be called. */
    UINT32 TimeScale;   /**< Time scale of the codec that the fifo is working on */
} SVC_FIFO_BASE_HDLR_CFG_s;

/**
 * Virtual fifo config
 */
typedef struct {
    UINT64 StartTime;   /**< StartTime indicates the expected time of the first frame for a virtual fifo in ms. */
                        /**< The first frame will be the latest IDR frame before start time in Base fifo. */
                        /**< If there's no such frame in base fifo, the first frame would be the first upcoming IDR frame. */
} SVC_FIFO_VIRTUAL_HDLR_CFG_s;

/**
 * Fifo config
 */
typedef struct {
    UINT32 NumEntries;                          /**< Number of entries of a fifo */
    UINT8 *BufferAddr;                          /**< Start addres of the hdlr buffer */
    UINT32 BufferSize;                          /**< Size of the hdlr buffer */
    SVC_FIFO_HDLR_s *BaseHdlr;                  /**< Base fifo of the hdlr. If the created FIFO is a base handler, its BaseHdlr is NULL. */
    SVC_FIFO_CALLBACK_f CbEvent;                /**< The callback function for fifo event */
    SVC_FIFO_BASE_HDLR_CFG_s BaseCfg;           /**< Configuration for base fifo only */
    SVC_FIFO_VIRTUAL_HDLR_CFG_s VirtualCfg;     /**< Configuration for virtual fifo only */
} SVC_FIFO_CFG_s;

/**
 * Fifo information
 */
typedef struct {
    UINT64 AccumulateEntries;   /**< Total number of written entries */
    UINT32 AvailEntries;        /**< Number of entries with data */
} SVC_FIFO_INFO_s;

/**
 * Process a Fifo Event.
 *
 * @param [in] Fifo The fifo to operate
 * @param [in] EventId The id of the fifo event
 * @return OK or FIFO_ERR_XXXX
 */
UINT32 SvcFIFO_ProcessEvent(const SVC_FIFO_HDLR_s *Fifo, UINT8 EventId);

/**
 * Get the required buffer size.
 *
 * @param [in] NumMaxFifo Maximum number of handlers that will be used
 * @param [in] NumMaxPendingEvent Maximum number of pending events
 * @param [out] BufferSize The required buffer size
 * @return OK or FIFO_ERR_XXXX
 */
UINT32 SvcFIFO_GetInitBufferSize(UINT32 NumMaxFifo, UINT32 NumMaxPendingEvent, UINT32 *BufferSize);

/**
 * Get fifo module default config for initializing.
 *
 * @param [out] DefaultCfg Fifo module config
 * @return OK or FIFO_ERR_XXXX
 */
UINT32 SvcFIFO_GetInitDefaultCfg(SVC_FIFO_INIT_CFG_s *DefaultCfg);

/**
 * Initialize the fifo module.
 * The function should only be invoked once.
 * User MUST invoke this function before using fifo module.
 * The memory pool of the module will be provided by users.
 *
 * @param [in] Cfg Fifo module config
 * @return OK or FIFO_ERR_XXXX
 */
UINT32 SvcFIFO_Init(const SVC_FIFO_INIT_CFG_s *Cfg);

/**
 * Get the fifo default config.
 *
 * @param [out] DefaultCfg Fifo module config
 * @return OK or FIFO_ERR_XXXX
 */
UINT32 SvcFIFO_GetDefaultCfg(SVC_FIFO_CFG_s *DefaultCfg);

/**
 * Get the hdlr buffer size for a fifo handler.
 *
 * @param [in] NumEntries Number of entries
 * @param [out] BufferSize Buffer size
 * @return OK or FIFO_ERR_XXXX
 */
UINT32 SvcFIFO_GetHdlrBufferSize(UINT32 NumEntries, UINT32 *BufferSize);

/**
 * Create a base/virtual fifo.
 *
 * @param [in] Cfg The config of the fifo
 * @param [out] Fifo The created fifo handler
 * @return OK or FIFO_ERR_XXXX
 */
UINT32 SvcFIFO_Create(const SVC_FIFO_CFG_s *Cfg, SVC_FIFO_HDLR_s **Fifo);

/**
 * Delete a fifo.
 *
 * @param [in] Fifo The fifo to be deleted
 * @return OK or FIFO_ERR_XXXX
 */
UINT32 SvcFIFO_Delete(SVC_FIFO_HDLR_s *Fifo);

/**
 * Peek the entry in fifo by the given distance.
 * For example, there is three entries in current fifo.
 * (wp) [entry A] [entry B] [entry C] (rp)
 * SvcFIFO_PeekEntry(Fifo, &Desc, 0) will get entry C
 * SvcFIFO_PeekEntry(Fifo, &Desc, 1) will get entry B
 * SvcFIFO_PeekEntry(Fifo, &Desc, 2) will get entry A
 * SvcFIFO_PeekEntry(Fifo, &Desc, 3) will return failure
 *
 * @param [in] Fifo The fifo to operate
 * @param [out] Desc The peeked entry
 * @param [in] Index Index of the peeked entry
 * @return OK or FIFO_ERR_XXXX
 */
UINT32 SvcFIFO_PeekEntry(const SVC_FIFO_HDLR_s *Fifo, SVC_FIFO_BITS_DESC_s **Desc, UINT32 Index);

/**
 * Remove last NumEntries entries.
 * For example, there is three entries in current fifo.
 * (wp) [entry A] [entry B] [entry C] (rp)
 * After SvcFIFO_RemoveEntry(Fifo, 2)
 * it becomes
 * (wp) [entry A] (rp)
 * For base fifo, all its virtual fifos will be synced.
 * For virtaul fifo, the function only works on the input virtual fifo and its base fifo.
 *
 * @param [in] Fifo The Fifo to operate
 * @param [in] NumEntries Number of entry to remove
 * @return OK or FIFO_ERR_XXXX
 */
UINT32 SvcFIFO_RemoveEntry(const SVC_FIFO_HDLR_s *Fifo, UINT32 NumEntries);

/**
 * Prepare free space to write data.
 * User MUST use this function to get the correct descriptor of the write point.
 * The function can only be called by a virtual fifo.
 *
 * @param [in] Fifo The fifo to operate
 * @param [out] Desc The descriptor of the space
 * @return OK or FIFO_ERR_XXXX
 */
UINT32 SvcFIFO_PrepareEntry(const SVC_FIFO_HDLR_s *Fifo, SVC_FIFO_BITS_DESC_s *Desc);

/**
 * Write data to fifo.
 * For example, there is one entry in current fifo.
 * (wp) [entry A]  (rp)
 * After SvcFIFO_WriteEntry(Fifo, DescEntryB)
 * it becomes
 * (wp) [entry B][entry A] (rp)
 *
 * @param [in] Fifo The fifo to operate
 * @param [in] Desc The descriptor of the data to write
 * @param [in] Async Asynchronous write or not
 * @return OK or FIFO_ERR_XXXX
 */
UINT32 SvcFIFO_WriteEntry(const SVC_FIFO_HDLR_s *Fifo, const SVC_FIFO_BITS_DESC_s *Desc, UINT8 Async);

/**
 * Erase all data in fifo. Reset the read and write pointer.
 *
 * @param [in] Fifo The fifo to operate
 * @return OK or FIFO_ERR_XXXX
 */
UINT32 SvcFIFO_Reset(const SVC_FIFO_HDLR_s *Fifo);

/**
 * Get fifo info.
 *
 * @param [in] Fifo The fifo to operate
 * @param [out] Info The info of the fifo
 * @return OK or FIFO_ERR_XXXX
 */
UINT32 SvcFIFO_GetInfo(const SVC_FIFO_HDLR_s *Fifo, SVC_FIFO_INFO_s *Info);

#endif /* SVC_FIFO_H */
