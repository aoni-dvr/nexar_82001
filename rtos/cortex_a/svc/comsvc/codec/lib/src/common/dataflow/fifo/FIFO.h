/**
 * @file FIFO.h
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

#ifndef FIFO_H
#define FIFO_H

#include <AmbaKAL.h>
#include "SvcFIFO.h"

#define SVC_FIFO_FLAG_PROCESS_WRITE    (0x00000001U)   /**< Event flag for synchronous write */
#define SVC_FIFO_FLAG_PROCESS_RESET    (0x00000010U)   /**< Event flag for reset */
#define SVC_FIFO_FLAG_PROCESS_CREATE   (0x00000100U)   /**< Event flag for create */

/**
 * Virtual fifo state
 */
#define SVC_FIFO_STATE_IDLE             (0x0U)     /**< The virtual fifo is waiting for the prerecorded frame */
#define SVC_FIFO_STATE_WAIT_FIRST_FRAME (0x1U)     /**< The virtual fifo is waiting for the first frame */
#define SVC_FIFO_STATE_READY            (0x2U)     /**< The virtual fifo is ready. */

/**
 * Info for SVC_FIFO_EVENT_WRITE_LINKED_FIFO event
 */
#define SVC_FIFO_SYNC_WRITE         (0x0U)         /**< Synchronous write */
#define SVC_FIFO_ASYNC_WRITE        (0x1U)         /**< Asynchronous write */
#define SVC_FIFO_CREATE_WITH_DATA   (0x2U)         /**< Copy prerecorded descriptors to a virtual fifo while creating */

/**
 *  Descriptor Queue
 */
typedef struct {
    SVC_FIFO_BITS_DESC_s *Queue;    /**< Address to the descriptor array */
    UINT32 QueueSize;    /**< Size of the queue */
    UINT32 NumEntries;   /**< Number of queue entries */
    UINT64 ReadPointer;  /**< Read pointer of the queue, never wrap around */
    UINT64 WritePointer; /**< Write pointer of the queue, never wrap around */
    UINT64 TotalWrite;   /**< Total number of written entries */
} AMBA_FIFO_QUEUE_s;

/**
 * Pending events
 */
typedef struct {
    UINT8 EventId;              /**< Event Id */
    SVC_FIFO_BITS_DESC_s Desc;  /**< SVC_FIFO_EVENT_WRITE_LINKED_FIFO: the descriptor to write. */
    UINT32 Info;                /**< SVC_FIFO_EVENT_REMOVE_LINKED_FIFO: number of entries to remove./ SVC_FIFO_EVENT_WRITE_LINKED_FIFO: Async Write or not. */
    UINT32 NextIdx;             /**< Index of the next command */
} SVC_FIFO_CMD_s;

/**
 * Cmd(Events) linked list
 */
typedef struct {
    UINT32 HeadIdx; /**< Head of commands in used */
    UINT32 TailIdx; /**< Tail of the link list */
} SVC_FIFO_CMD_LIST_s;

/**
 * Cmd(Events) pool
 */
typedef struct {
    AMBA_KAL_MUTEX_t Mutex; /**< Mutex */
    SVC_FIFO_CMD_s *Cmds;   /**< Command pool */
    UINT32 FreeIdx;         /**< Head of free space */
    UINT32 MaxCmdNum;       /**< Maximum number of commands */
} SVC_FIFO_CMD_POOL_s;

/**
 * Internal fifo hdlr
 */
typedef struct SVC_FIFO_HANDLER_s {
    // public
    SVC_FIFO_HDLR_s Public;         /**< public data, the pointer to the structure will return to app after create. */
    // private
    UINT8 Used;                     /**< If the fifo hdlr is in used. */
    struct SVC_FIFO_HANDLER_s *ParentBase;   /**< Pointer to baseFifo, virtual fifo only */
    AMBA_KAL_MUTEX_t Mutex;         /**< Mutex for the fifo. Lock this mutex when setting the values of fifo. */
    AMBA_FIFO_QUEUE_s DescQueue;    /**< Descriptor queue. (entry type SVC_FIFO_BITS_DESC_s) */
    SVC_FIFO_CALLBACK_f CbEvent;    /**< Callback function for fifo event */
    UINT8 AutoRemove;               /**< If AutoRemove is set, the base FIFO will remove entries which are no longer in any of its virtual FIFOs automatically. */
                                    /**< Otherwise, SvcFIFO_RemoveEntry must be called. */
    UINT32 TimeScale;               /**< TimeScale of the codec. */
    UINT64 StartTime;               /**< StartTime indicates the expected time of the first frame for a virtual fifo in ms. */
    UINT8 State;                    /**< Virtual fifo State */
    AMBA_KAL_EVENT_FLAG_t Flag;     /**< Flags for fifo hdlr */
    SVC_FIFO_CMD_LIST_s CmdList;    /**< A linked list of the pending events to be processed */
} SVC_FIFO_HANDLER_s;

/**
 * Variables for fifo module
 */
#pragma pack(4)
typedef struct {
    SVC_FIFO_HANDLER_s *BaseFifoHandler[SVC_FIFO_MAX_ACTIVE_FIFO];        /**< An array of pointers to base fifo. */
    SVC_FIFO_HANDLER_s *VirtualFifoHandler[SVC_FIFO_MAX_ACTIVE_FIFO];     /**< An array of pointers to virtual fifo. */
    UINT8 *MemPool;                 /**< Working space start address provided by upper level */
    UINT32 MemPoolSize;             /**< Working space total size */
    UINT32 NumMaxFifo;              /**< Max supported fifo number (virtual + base) */
    SVC_FIFO_CALLBACK_f CbEvent;    /**< Callback funtion to process write/remove/reset */
    SVC_FIFO_CMD_POOL_s CmdPool;    /**< Pending events pool */
    AMBA_KAL_MUTEX_t Mutex;         /**< Global Mutex */
} SVC_FIFO_VAR_s;
#pragma pack()

#endif /* FIFO_H */
