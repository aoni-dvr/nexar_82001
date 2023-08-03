/**
 * @file SvcStream.h
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
#ifndef CONTAINER_STREAM_H
#define CONTAINER_STREAM_H

/**
 * I/O stream implementation
 *
 * Stream module provides the APIs and interface to access I/O streams.
 */

#include "format/SvcFormatDef.h"

#define STREAM_ERR_0000 (STREAM_ERR_BASE)           /**< Invaid arguement */
#define STREAM_ERR_0001 (STREAM_ERR_BASE | 0x1U)    /**< Fatal error */
#define STREAM_ERR_0002 (STREAM_ERR_BASE | 0x2U)    /**< IO error */

#define SVC_STREAM_SEEK_CUR    AMBA_FS_SEEK_CUR    /**< The current position of a stream (seek to the position relative to current file position) */
#define SVC_STREAM_SEEK_START  AMBA_FS_SEEK_START  /**< The beginning of a stream (seek to the position relative to start position of file) */
#define SVC_STREAM_SEEK_END    AMBA_FS_SEEK_END    /**< The end of a stream (seek to the position relative to end position of file) */


/**
 *  Stream open mode
 */
#define SVC_STREAM_MODE_RDONLY (1U)    /**< Read only */
#define SVC_STREAM_MODE_WRONLY (2U)    /**< Write only */
#define SVC_STREAM_MODE_RDWR   (3U)    /**< Read write */
#define SVC_STREAM_MODE_WRRD   (4U)    /**< Write read */
#define SVC_STREAM_MODE_MAX    (5U)    /**< Max value (used to check the range of mode) */

struct SVC_STREAM_s;

/**
 * Stream handler
 */
typedef struct {
    struct SVC_STREAM_s *Func;   /**< Stream interface */
} SVC_STREAM_HDLR_s;

/**
 * Stream interface
 */
typedef struct SVC_STREAM_s {
    UINT32 (*Open)(SVC_STREAM_HDLR_s *Hdlr, const char *Name, UINT32 Mode);    /**< The interface to open a stream handler */
    UINT32 (*Close)(SVC_STREAM_HDLR_s *Hdlr);  /**< The interface to close a stream handler*/
    UINT32 (*Read)(SVC_STREAM_HDLR_s *Hdlr, UINT32 Size, UINT8 *Buffer, UINT32 *Count);    /**< The interface to read data from the stream handler */
    UINT32 (*Write)(SVC_STREAM_HDLR_s *Hdlr, UINT32 Size, UINT8 *Buffer, UINT32 *Count);   /**< The interface to write data to the stream handler */
    UINT32 (*Seek)(SVC_STREAM_HDLR_s *Hdlr, INT64 Pos, INT32 Orig);            /**< The interface to seek the stream handler */
    UINT32 (*GetPos)(SVC_STREAM_HDLR_s *Hdlr, UINT64 *Pos);                    /**< The interface to get the current position of the stream handler */
    UINT32 (*GetLength)(SVC_STREAM_HDLR_s *Hdlr, UINT64 *Size);                /**< The interface to get length of the stream handler */
    UINT32 (*GetFreeSpace)(SVC_STREAM_HDLR_s *Hdlr, UINT64 *Size);             /**< The interface to get the free space of the storage used by the stream handler */
    UINT32 (*Sync)(SVC_STREAM_HDLR_s *Hdlr);   /**< The interface to the function to sync a stream) */
    UINT32 (*Func)(SVC_STREAM_HDLR_s *Hdlr, UINT32 Cmd, UINT32 Param);         /**< The interface to execute special stream commands */
} SVC_STREAM_s;


#endif /* STREAM_H_ */

