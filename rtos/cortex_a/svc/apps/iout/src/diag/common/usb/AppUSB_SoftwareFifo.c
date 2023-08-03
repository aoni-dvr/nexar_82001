/**
 *  @file AppUSB_SoftwareFifo.c
 *
 *  @copyright Copyright (c) 2019 Ambarella, Inc.
 *
 *  This file and its contents ("Software") are protected by intellectual property rights including, without limitation,
 *  U.S. and/or foreign copyrights.  This Software is also the confidential and proprietary information of Ambarella, Inc.
 *  and its licensors.  You may not use, reproduce, disclose, distribute, modify, or otherwise prepare derivative works
 *  of this Software or any portion thereof except pursuant to a signed license agreement or nondisclosure agreement with
 *  Ambarella, Inc. or its authorized affiliates.  In the absence of such an agreement, you agree to promptly notify and
 *  return this Software to Ambarella, Inc.
 *
 *  THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 *  WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL
 *  AMBARELLA, INC. OR ITS AFFILIATES BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 *  DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 *  COMPUTER FAILURE OR MALFUNCTION; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 *  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 *  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *  @details Software FIFO implementation for USB unit tests.
 */
/**
 * \page page5 Software FIFO
 * \section Introduction
 * Software FIFO is used by CDC-ACM application to serialize data input from USB Host and data output to USB Host. It should be thread-safe.
 * \section API sequence for FIFO write
 * - AppUsb_SwFifoOpen('w')
 * - AppUsb_SwFifoWrite
 * - AppUsb_SwFifoClose('w')
 * \section API sequence for FIFO read
 * - AppUsb_SwFifoOpen('r')
 * - AppUsb_SwFifoWrite
 * - AppUsb_SwFifoClose('r')
*/
#include "AppUSB.h"
#include "AppUSB_SoftwareFifo.h"
#ifndef AMBA_KAL_H
#include <AmbaKAL.h>
#endif

#define FIFO_MAX_ID              4
#define FIFO_MAX_BUFFER_SIZE     (1024)
#define FIFO_MODE_RD             1
#define FIFO_MODE_WR             2
#define FIFO_MODE_UNKNOWN        3
#define FIFO_FLG_READY_FOR_READ  0x00000001
#define FIFO_FLG_READY_FOR_WRITE 0x00000002
#define FIFO_WAIT_FOREVER        0xFFFFFFFFU

#define SWFIFO_TRUE   1
#define SWFIFO_FALSE  0

static INT32 int32_min(INT32 a, INT32 b)
{
    INT32 nret;

    if (a < b) {
        nret = a;
    } else {
        nret = b;
    }
    return nret;
}

//#define ENABLE_DEBUG_FIFO
//#define ENABLE_DEBUG_FIFO_FLG

typedef struct {
    INT32                 IdxRead;       //!< internal buffer read pointer
    INT32                 IdxWrite;      //!< internal buffer write pointer
    INT32                 IdxBegin;      //!< internal buffer begin index
    INT32                 IdxEnd;        //!< internal buffer end index
    INT32                 DataLength;    //!< FIFO data length in bytes
    AMBA_KAL_EVENT_FLAG_t EventFlag;       //!< flag for read/write synchronization
    AMBA_KAL_MUTEX_t      Mutex;           //!< mutex for read/write synchronization
    INT32                 ReadOpenCount;   //!< FIFO openning count for read
    INT32                 WriteOpenCount;  //!< FIFO openning count for write
    INT32                 FlagInUse;                    //!< FIFO is in usage
    UINT8                 Memory[FIFO_MAX_BUFFER_SIZE]; //!< internal buffer
} SWFIFO_t;

static SWFIFO_t fifo_instances[FIFO_MAX_ID] __attribute__((section(".bss.noinit")));
static INT32 flag_fifo_instance_init __attribute__((section(".bss.noinit")));
static AMBA_KAL_MUTEX_t fifo_mutex __attribute__((section(".bss.noinit")));

static INT32 fifo_data_size_get(const SWFIFO_t *FifoInstance)
{
    return (FifoInstance->DataLength);
}

static INT32 fifo_free_size_get(const SWFIFO_t *FifoInstance)
{
    return ((FifoInstance->IdxEnd - FifoInstance->IdxBegin) - FifoInstance->DataLength);
}

static INT32 fp_fifo_open(SWFIFO_t *FifoInstance, INT32 mode)
{
    if (mode == FIFO_MODE_RD) {
        FifoInstance->ReadOpenCount++;
    } else {
        FifoInstance->WriteOpenCount++;
    }

    return 0;
}

static INT32 fp_fifo_close(SWFIFO_t *FifoInstance, INT32 mode)
{
    INT32 nret = 0;

    if (mode == FIFO_MODE_RD) {
        if (FifoInstance->EventFlag.tx_event_flags_group_id != 0U) {
            (void)AmbaKAL_EventFlagSet(&FifoInstance->EventFlag, FIFO_FLG_READY_FOR_WRITE);
            //DEBUG_FLG("set_flg : WRITE");
        }
        FifoInstance->ReadOpenCount--;
    } else {
        if (FifoInstance->EventFlag.tx_event_flags_group_id != 0U) {
            (void)AmbaKAL_EventFlagSet(&FifoInstance->EventFlag, FIFO_FLG_READY_FOR_READ);
            //DEBUG_FLG("set_flg : READ");
        }
        FifoInstance->WriteOpenCount--;
    }

    if (FifoInstance->ReadOpenCount < 0) {
        FifoInstance->ReadOpenCount = 0;
        nret      = FIFO_ERR_FIFO_NOT_EXIST;
    }

    if (FifoInstance->WriteOpenCount < 0) {
        FifoInstance->WriteOpenCount = 0;
        nret      = FIFO_ERR_FIFO_NOT_EXIST;
    }

    return nret;
}

static INT32 fp_fifo_read(SWFIFO_t *FifoInstance, UINT8 *Buffer, INT32 Size)
{
    INT32 data_size;
    INT32 nret = 0;
    INT32 idx  = 0;

    data_size = fifo_data_size_get(FifoInstance);
    if (Size > data_size) {
        AppUsb_PrintUInt5("fp_fifo_read(): buffer size %d too small, require %d", (UINT32)Size, (UINT32)data_size, 0, 0, 0);
        nret = -1;
    } else {
        do {
            INT32 len = int32_min(FifoInstance->IdxEnd - FifoInstance->IdxRead, Size);
            AppUsb_MemoryCopy(&Buffer[idx], &(FifoInstance->Memory[FifoInstance->IdxRead]), (UINT32)len);
            FifoInstance->IdxRead += len;
            if (FifoInstance->IdxRead >= FifoInstance->IdxEnd) {
                FifoInstance->IdxRead = FifoInstance->IdxBegin;
            }
            idx        += len;
            Size       -= len;
            FifoInstance->DataLength -= len;
        } while (Size > 0);
    }

    return nret;
}

static INT32 fp_fifo_write(SWFIFO_t *FifoInstance, const UINT8 *Buffer, INT32 Size)
{
    INT32 free_size;
    INT32 nret = 0;
    INT32 idx  = 0;

    free_size = fifo_free_size_get(FifoInstance);
    if (Size > free_size) {
        AppUsb_PrintUInt5("fp_fifo_write(): free size %d too small, write %d", (UINT32)free_size, (UINT32)Size, 0, 0, 0);
        nret = -1;
    } else {
        do {
            INT32 len = int32_min(FifoInstance->IdxEnd - FifoInstance->IdxWrite, Size);
            AppUsb_MemoryCopy(&(FifoInstance->Memory[FifoInstance->IdxWrite]), &Buffer[idx], (UINT32)len);
            FifoInstance->IdxWrite += len;
            if (FifoInstance->IdxWrite >= FifoInstance->IdxEnd) {
                FifoInstance->IdxWrite = FifoInstance->IdxBegin;
            }
            idx        += len;
            Size       -= len;
            FifoInstance->DataLength += len;
        } while (Size > 0);
    }

    return nret;
}

static INT32 inner_fifo_wait(INT32 FifoID, char mode, UINT32 Timeout)
{
    SWFIFO_t *f;
    INT32  imode = 0;
    UINT32 flgptn;
    INT32  ercd;
    INT32  nret = 0;
    UINT32 func_uret = 0;

    //DEBUG_MSG("inner_fifo_wait (%d, %c, %d)", FifoID, mode, Timeout);

    f = &fifo_instances[FifoID];

    switch (mode) {
    case 'r':
    case 'R':
        imode = FIFO_MODE_RD;
        break;
    case 'w':
    case 'W':
        imode = FIFO_MODE_WR;
        break;
    default:
        imode = FIFO_MODE_UNKNOWN;
        break;
    }

    if (f->EventFlag.tx_event_flags_group_id != 0U) {
        if (imode == FIFO_MODE_RD) {
            //DEBUG_FLG("twai_flg : READ");
            // AMBA_KAL_AND:AnyOrAll = 1; AutoClear = 0;
            func_uret = AmbaKAL_EventFlagGet(&f->EventFlag, FIFO_FLG_READY_FOR_READ,
                                             1U, 0U, &flgptn, Timeout);
            ercd = 0;
        } else if (imode == FIFO_MODE_WR) {
            //DEBUG_FLG("twai_flg : WRITE");
            // AMBA_KAL_AND:AnyOrAll = 1; AutoClear = 0;
            func_uret = AmbaKAL_EventFlagGet(&f->EventFlag, FIFO_FLG_READY_FOR_WRITE,
                                             1U, 0U, &flgptn, Timeout);
            ercd = 0;
        } else {
            ercd = FIFO_ERR_INVALID_MODE;
            nret = FIFO_ERR_INVALID_MODE;
        }

        if (ercd != FIFO_ERR_INVALID_MODE) {
            if (func_uret != 0U) {
                nret = FIFO_ERR_TIMEOUT;
            }
        }
    } else {
        nret = FIFO_ERR_NO_INIT;
    }

    return nret;
}

static INT32 inner_fifo_read(INT32 FifoID, UINT8 *Buffer, INT32 Size)
{
    SWFIFO_t *f;
    INT32 len;
    INT32 data_size;
    INT32 rval = 0;

    //DEBUG_MSG("inner_fifo_read (%d, 0x%08X, %d)", FifoID, Buffer, Size);

    f = &fifo_instances[FifoID];
    if (f->Mutex.tx_mutex_id != 0U) {
        (void)AmbaKAL_MutexTake(&f->Mutex, FIFO_WAIT_FOREVER);
    }

    if ((Size < 0) || (Size > (f->IdxEnd - f->IdxBegin))) {
        rval = FIFO_ERR_ILLEGAL_SIZE;
    } else {
        data_size = fifo_data_size_get(f);
        len       = int32_min(data_size, Size);
        if (len > 0) {
            (void)fp_fifo_read(f, Buffer, len);
            (void)AmbaKAL_EventFlagSet(&f->EventFlag, FIFO_FLG_READY_FOR_WRITE);
            //DEBUG_FLG("set_flg : WRITE");
            if (data_size == len) {
                (void)AmbaKAL_EventFlagClear(&f->EventFlag, FIFO_FLG_READY_FOR_READ);
                //DEBUG_FLG("clr_flg : READ");
            }
            rval = len;
        } else if (len == 0) {
            rval = 0;
        } else {
            rval = FIFO_ERR_ILLEGAL_SIZE;
        }
    }

    if (f->Mutex.tx_mutex_id != 0U) {
        (void)AmbaKAL_MutexGive(&f->Mutex);
    }
    return rval;
}

static INT32 inner_fifo_write(INT32 FifoID, const UINT8 *Buffer, INT32 Size)
{
    SWFIFO_t *f;
    INT32 len;
    INT32 free_size;
    INT32 rval = 0;

    //DEBUG_MSG("inner_fifo_write (%d, 0x%08X, %d)", FifoID, Buffer, Size);

    f = &fifo_instances[FifoID];
    if (f->Mutex.tx_mutex_id != 0U) {
        (void)AmbaKAL_MutexTake(&f->Mutex, FIFO_WAIT_FOREVER);
    }

    if ((Size < 0) || (Size > (f->IdxEnd - f->IdxBegin))) {
        rval = FIFO_ERR_ILLEGAL_SIZE;
    } else {
        free_size = fifo_free_size_get(f);
        len       = int32_min(free_size, Size);
        if (len > 0) {
            (void)fp_fifo_write(f, Buffer, len);
            (void)AmbaKAL_EventFlagSet(&f->EventFlag, FIFO_FLG_READY_FOR_READ);
            //DEBUG_FLG("set_flg : READ");
            if (free_size == len) {
                (void)AmbaKAL_EventFlagClear(&f->EventFlag, FIFO_FLG_READY_FOR_WRITE);
                //DEBUG_FLG("clr_flg : WRITE");
            }
            rval = len;
        } else if (len == 0) {
            rval = 0;
        } else {
            rval = FIFO_ERR_ILLEGAL_SIZE;
        }
    }

    if (f->Mutex.tx_mutex_id != 0U) {
        (void)AmbaKAL_MutexGive(&f->Mutex);
    }
    return rval;
}

static INT32 inner_fifo_size(INT32 FifoID)
{
    const SWFIFO_t *f;

    f = &fifo_instances[FifoID];

    return (f->IdxEnd - f->IdxBegin);
}

/**
 * Test for ready of FIFO
 *
 * @param FifoID FIFO id
 * @param Mode 'r/R' for read, 'w/W' for write
 * @retval SWFIFO_TRUE  the fifo is ready for read or write (according to the Mode parameter)
 * @retval SWFIFO_FALSE the fifo is NOT ready for read or write (according to the Mode parameter)
 */
static INT32 inner_fifo_is_ready(INT32 FifoID, char Mode)
{
    SWFIFO_t *f;
    INT32 imode = 0;
    INT32 rval  = SWFIFO_FALSE;
    UINT32 func_uret  = 0;

    //DEBUG_MSG("fifo_is_closed (%d, %c)", FifoID, Mode);

    f = &fifo_instances[FifoID];
    if (f->Mutex.tx_mutex_id != 0U) {
        func_uret = AmbaKAL_MutexTake(&f->Mutex, FIFO_WAIT_FOREVER);
    }

    if (func_uret == 0U) {
        switch (Mode) {
        case 'r':
        case 'R':
            imode = FIFO_MODE_RD;
            break;
        case 'w':
        case 'W':
            imode = FIFO_MODE_WR;
            break;
        default:
            imode = FIFO_MODE_UNKNOWN;
            break;
        }

        if (imode == FIFO_MODE_RD) {
            if (f->WriteOpenCount == 0) {
                rval = SWFIFO_TRUE;
            }
        } else if (imode == FIFO_MODE_WR) {
            if (f->ReadOpenCount == 0) {
                rval = SWFIFO_TRUE;
            }
        } else {
            // how to process it?
            rval = SWFIFO_FALSE;
        }
    }

    if (f->Mutex.tx_mutex_id != 0U) {
        func_uret = AmbaKAL_MutexGive(&f->Mutex);
        if (func_uret != 0U) {
            // print error?
        }
    }

    return rval;
}

/** \addtogroup APPLICATION_APIS
 *  @{
 * */
/**
 * Initialize parameters to zero.
*/
void AppUsb_SwFifoInitZero(void)
{
    static INT32 init = 0;
    if (init == 0) {
        flag_fifo_instance_init = 0;
        AppUsb_MemoryZeroSet(&fifo_mutex, sizeof(fifo_mutex));
        init = 1;
    }
    return;
}
/**
 * Create a FIFO with FIFO size
 *
 * @param Size FIFO size
 * @return FIFO id for success, < 0 value for failure
 * @retval FIFO_ERR_GET_MEM_FAIL FIFO size is too large.
 * @retval FIFO_ERR_UNKNOWN create OS resources fail.
 */
INT32 AppUsb_SwFifoCreate(INT32 Size)
{
    SWFIFO_t *f;
    INT32 rval = 0;
    INT32 id;
    UINT32 func_uret = 0;

    static char sw_fifo_eventflag_name[] = "SwFifoEventFlg";
    static char sw_fifo_mutex_name[] = "SwFifoMutex";
    static char sw_fifo_rw_mutex_name[] = "SwFifoRwMutex";

    if (fifo_mutex.tx_mutex_id != 0U) {
        (void)AmbaKAL_MutexTake(&fifo_mutex, FIFO_WAIT_FOREVER);
    }

    if (flag_fifo_instance_init == 0) {
        /* Initialize all FIFO descriptors */
        AppUsb_MemoryZeroSet(fifo_instances, sizeof(SWFIFO_t) * (UINT32)FIFO_MAX_ID);
        flag_fifo_instance_init = 1;

        /* Create global mutex */
        func_uret = AmbaKAL_MutexCreate(&fifo_mutex, sw_fifo_mutex_name);
        if (func_uret != 0U) {
            AppUsb_PrintUInt5("AppUsb_SwFifoCreate(): can't create global mutex, code 0x%X", func_uret, 0, 0, 0, 0);
            rval = FIFO_ERR_UNKNOWN;
        }
    }

    if (Size > FIFO_MAX_BUFFER_SIZE) {
        rval = FIFO_ERR_GET_MEM_FAIL;
    }

    if (rval == 0) {
        f = fifo_instances;
        for (id = 0; id < FIFO_MAX_ID; id++) {
            if (f->FlagInUse == 0) {
                break;
            }
            f++;
        }

        if (id == FIFO_MAX_ID) {
            rval = FIFO_RUN_OUT_OF_ID;
        } else {
            f->IdxBegin  = 0;
            f->IdxRead     = f->IdxBegin;
            f->IdxWrite     = f->IdxBegin;
            f->IdxEnd    = f->IdxBegin + Size;
            f->FlagInUse = 1;

            /* Create an event flag */
            func_uret = AmbaKAL_EventFlagCreate(&f->EventFlag, sw_fifo_eventflag_name);
            if (func_uret != 0U) {
                AppUsb_PrintUInt5("AppUsb_SwFifoCreate(): can't create flag, code 0x%X", func_uret, 0, 0, 0, 0);
                rval = FIFO_ERR_UNKNOWN;
            } else {
                /* Set default to be ready for write */
                (void)AmbaKAL_EventFlagSet(&f->EventFlag, FIFO_FLG_READY_FOR_WRITE);
                //DEBUG_FLG("set_flg : WRITE");

                /* Create a mutex */
                func_uret = AmbaKAL_MutexCreate(&f->Mutex, sw_fifo_rw_mutex_name);
                if (func_uret != 0U) {
                    AppUsb_PrintUInt5("AppUsb_SwFifoCreate(): can't create rxmutex, code 0x%X", func_uret, 0, 0, 0, 0);
                    rval = FIFO_ERR_UNKNOWN;
                } else {
                    rval = id;
                }
            }
        }
        //DEBUG_MSG("%s(): FIFO ID %d created.", __func__, id);
    }

    if (fifo_mutex.tx_mutex_id != 0U) {
        (void)AmbaKAL_MutexGive(&fifo_mutex);
    }
    return rval;
}
/**
 * Destroy a FIFO
 * @param FifoID FIFO id
 * @return 0 for success, otherwise failure
 */
INT32 AppUsb_SwFifoDestroy(INT32 FifoID)
{
    SWFIFO_t *f;
    INT32 rval = 0;
    UINT32 func_uret = 0;

    if (fifo_mutex.tx_mutex_id != 0U) {
        (void)AmbaKAL_MutexTake(&fifo_mutex, FIFO_WAIT_FOREVER);
    }

    if (FifoID < 0) {
        // do nothing
    } else {
        f = &fifo_instances[FifoID];

        if (f->FlagInUse != 0) {
            /* Delete event flag */
            func_uret = AmbaKAL_EventFlagDelete(&f->EventFlag);
            if (func_uret != 0U) {
                // need to process it?
                rval = -1;
            }

            /* Delete mutex */
            func_uret = AmbaKAL_MutexDelete(&f->Mutex);
            if (func_uret != 0U) {
                // need to process it?
                rval = -1;
            }

            AppUsb_MemoryZeroSet(f, sizeof(SWFIFO_t));
        }
    }

    if (fifo_mutex.tx_mutex_id != 0U) {
        (void)AmbaKAL_MutexGive(&fifo_mutex);
    }

    return rval;
}
/**
 * Open a FIFO with a FIFO id
 *
 * @param FifoID  FIFO id
 * @param Mode    'r/R' for read, 'w/W' for write
 * @return 0 for success, otherwise failure
 */
INT32 AppUsb_SwFifoOpen(INT32 FifoID, char Mode)
{
    SWFIFO_t *f;
    INT32 imode = 0;
    INT32 rval  = 0;

    //DEBUG_MSG("fifo_open (%d, %c)", FifoID, Mode);

    if (fifo_mutex.tx_mutex_id != 0U) {
        (void)AmbaKAL_MutexTake(&fifo_mutex, FIFO_WAIT_FOREVER);
    }

    if (FifoID >= FIFO_MAX_ID) {
        rval = FIFO_ERR_INVALID_ID;
    } else {
        f = &fifo_instances[FifoID];

        switch (Mode) {
        case 'r':
        case 'R':
            imode = FIFO_MODE_RD;
            break;
        case 'w':
        case 'W':
            imode = FIFO_MODE_WR;
            break;
        default:
            rval = FIFO_ERR_INVALID_MODE;
            break;
        }

        if (rval == 0) {
            rval = fp_fifo_open(f, imode);
        }
    }

    if (fifo_mutex.tx_mutex_id != 0U) {
        (void)AmbaKAL_MutexGive(&fifo_mutex);
    }

    return rval;
}
/**
 * Close an opened FIFO
 *
 * @param FifoID FIFO id
 * @param mode   'r/R' for read, 'w/W' for write
 * @return 0 for success, otherwise failure
 */
INT32 AppUsb_SwFifoClose(INT32 FifoID, char Mode)
{
    SWFIFO_t *f;
    INT32 imode = 0;
    INT32 rval  = 0;

    //DEBUG_MSG("fifo_close (%d, %c)", FifoID, Mode);

    if (fifo_mutex.tx_mutex_id != 0U) {
        (void)AmbaKAL_MutexTake(&fifo_mutex, FIFO_WAIT_FOREVER);
    }

    if (flag_fifo_instance_init == 0) {
        rval = FIFO_ERR_FIFO_NOT_EXIST;
    } else {
        f = &fifo_instances[FifoID];

        switch (Mode) {
        case 'r':
        case 'R':
            imode = FIFO_MODE_RD;
            break;
        case 'w':
        case 'W':
            imode = FIFO_MODE_WR;
            break;
        default:
            rval = FIFO_ERR_INVALID_MODE;
            break;
        }
        if (rval == 0) {
            rval = fp_fifo_close(f, imode);
        }
    }

    if (fifo_mutex.tx_mutex_id != 0U) {
        (void)AmbaKAL_MutexGive(&fifo_mutex);
    }
    return rval;
}

/**
 * Read FIFO data
 *
 * @param FifoID  FIFO id
 * @param Buffer  Pointer to data area for data to be read
 * @param Size    Size for data to be read, in bytes
 * @param Timeout Timeout duration in millisecond,
 * @return   On success, the number of bytes read is returned.<br>
 *           On error, < 0 integer is returned
 */
INT32 AppUsb_SwFifoRead(INT32 FifoID, UINT8 *Buffer, INT32 Size, UINT32 Timeout)
{
    INT32 each_read_size;
    INT32 rval = 0;
    INT32 nleft;
    INT32 len;
    INT32 fifo_rp   = 0;
    INT32 retry     = 0;
    UINT32 is_break = 0;
    UINT32 tick_passed = 0;
    UINT32 tick_sleep = 50;

    //DEBUG_MSG("fifo_read (%d, 0x%08X, %d, %d)", FifoID, Buffer, Size, Timeout);

    if (Size <= 0) {
        rval = FIFO_ERR_PARAM;
    } else {
        nleft = Size;
        //fifo_rp = Buffer;

        each_read_size = inner_fifo_size(FifoID) / 2;
        while (nleft != 0) {
            if (is_break == 1U) {
                break;
            }
            rval = inner_fifo_wait(FifoID, 'r', Timeout);
            if (rval == FIFO_ERR_TIMEOUT) {
                // Timeout is normal for read.
                rval     = 0;
                is_break = 1;
                continue;
            }

            if (nleft <= each_read_size) {
                len = nleft;
            } else {
                len = each_read_size;
            }
            rval = inner_fifo_read(FifoID, &Buffer[fifo_rp], len);
            if (rval == 0) {
                if (inner_fifo_is_ready(FifoID, 'r') == SWFIFO_TRUE) {
                    if ((tick_passed < Timeout) || (Timeout == 0xFFFFFFFFU)) {
                        // fifo is ready but no data. sleep and go check if data is available
                        AppUsb_TaskSleep(tick_sleep);
                        tick_passed += tick_sleep;
                    } else {
                        rval     = 0;
                        is_break = 1;
                    }
                    continue;
                } else {
                    // Sleep this task to wait for fifo closed by another thread.
                    AppUsb_TaskSleep(5);
                    retry++;
                    if (retry > 100) {
                        AppUsb_PrintUInt5("AppUsb_SwFifoRead(): can't wait %d close even no data.!", (UINT32)FifoID, 0, 0, 0, 0);
                        is_break = 1;
                        continue;
                    }
                }
            } else if (rval < 0) {
                AppUsb_PrintUInt5("inner_fifo_read failed (%d)!", (UINT32)rval, 0, 0, 0, 0);
                is_break = 1;
                continue;
            } else {
                /* pass vcast checking */
            }
            nleft   -= rval;
            fifo_rp += rval;
        }
        if (rval >= 0) {
            rval = Size - nleft;
        }
    }

    return rval;
}

/**
 * Write data into a FIFO
 *
 * @param FifoID  FIFO id
 * @param Buffer  Pointer to data area for data to be written
 * @param Size    Size for data to be written, in bytes
 * @param Timeout Timeout duration in millisecond,
 * @return   On success, the number of bytes writen is returned.<br>
 *           On error, < 0 integer is returned
 */
INT32 AppUsb_SwFifoWrite(INT32 FifoID, const UINT8 *Buffer, INT32 Size, UINT32 Timeout)
{
    INT32 each_write_size;
    INT32 rval = 0;
    INT32 nleft;
    INT32 len;
    INT32 fifo_wp   = 0;
    INT32 retry     = 0;
    UINT32 is_break = 0;

    //DEBUG_MSG("fifo_write (%d, 0x%08X, %d, %d)", FifoID, Buffer, Size, Timeout);

    if (Size <= 0) {
        rval = FIFO_ERR_PARAM;
    } else {
        nleft = Size;
        //fifo_wp = (UINT8 *) Buffer;

        each_write_size = inner_fifo_size(FifoID) / 2;
        while (nleft != 0) {
            if (is_break == 1U) {
                break;
            }
            rval = inner_fifo_wait(FifoID, 'w', Timeout);
            if (rval == FIFO_ERR_TIMEOUT) {
                is_break = 1;
                continue;
            }

            if (nleft <= each_write_size) {
                len = nleft;
            } else {
                len = each_write_size;
            }
            rval = inner_fifo_write(FifoID, &Buffer[fifo_wp], len);
            if (rval == 0) {
                if (inner_fifo_is_ready(FifoID, 'w') == SWFIFO_TRUE) {
                    rval     = 0;
                    is_break = 1;
                    continue;
                } else {
                    // Sleep this task to wait for fifo closed by another thread.
                    AppUsb_TaskSleep(5);
                    retry++;
                    if (retry > 100) {
                        AppUsb_PrintUInt5("AppUsb_SwFifoWrite(): can't wait %d close even no data.!", (UINT32)FifoID, 0, 0, 0, 0);
                        is_break = 1;
                        continue;
                    }
                }
            } else if (rval < 0) {
                AppUsb_PrintUInt5("inner_fifo_write failed (%d)!", (UINT32)rval, 0, 0, 0, 0);
                is_break = 1;
                continue;
            } else {
                /* pass vcast check */
            }
            nleft   -= rval;
            fifo_wp += rval;
        }

        if (rval >= 0) {
            rval = Size - nleft;
        }
    }

    return rval;
}
/** @} */
