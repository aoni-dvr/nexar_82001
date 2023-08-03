/**
 *  @file AmbaPrint.c
 *
 * Copyright (c) [2020] Ambarella International LP
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
 *  THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 *  WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL
 *  AMBARELLA, INC. OR ITS AFFILIATES BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 *  DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 *  COMPUTER FAILURE OR MALFUNCTION; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 *  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 *  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *  @details Ambarella Print System functions.
 *
 */

#include <stdio.h>
#include <pthread.h>
#include <sys/neutrino.h>
#include <inttypes.h>
#include <AmbaTypes.h>
#include <AmbaKAL.h>
#include <AmbaPrint.h>
#include <ambarella_mprint.h>
#include <AmbaUtility.h>
#include <sys/slog.h>
#include <sys/slogcodes.h>

#define PRINT_MODE         0
#define MODULE_PRINT_MODE  1
#define MODULE_ID_NA       0xFFFF

static mqd_t msg_queue = -1;

static int c_rfd = -1;                   // region file descriptor
static struct print_region *c_shm;  // caller share memory pointer
static UINT8 *c_ring_buf;

typedef enum {
    MESSAGE_TO_LOG_BUFFER = 0,
    MESSAGE_TO_PRINT_BUFFER,
    MESSAGE_TO_BOTH
} MESSAGE_DESTINATION_e;

static char print_line_buffer[LINE_BUFFER_LENGTH];

static void library_init(void) __attribute__((constructor));
static void library_destroy(void) __attribute__((destructor));

static void print_shm_open(void)
{
    // Only open once to improve performance
    if (c_rfd == -1) {
        int rfd;


        /* Create shared memory object and set its size */
        c_rfd = shm_open(SHM_PRINT_CTRL, O_RDWR, S_IRUSR | S_IWUSR);
        if (c_rfd == -1) {
            /* Handle error */;
            printf("error");
        }

        /* Map shared memory object */
        c_shm = mmap(NULL, sizeof(struct print_region),
                    PROT_READ | PROT_WRITE, MAP_SHARED, c_rfd, 0);
        if (c_shm == MAP_FAILED) {
            /* Handle error */;
            printf("error");
        }

        // For ring buffer
        /* Create shared memory object and set its size */
        rfd = shm_open(SHM_PRINT_BUF, O_RDWR, S_IRUSR | S_IWUSR);
        if (rfd == -1) {
        /* Handle error */;
            printf("error");
        }

        /* Map shared memory object */
        c_ring_buf = mmap(NULL, c_shm->ring_buf_size,
                   PROT_READ | PROT_WRITE, MAP_SHARED, rfd, 0);
        if (c_ring_buf == MAP_FAILED) {
            /* Handle error */;
            printf("error");
        }
    }
}

static void print_shm_close(void)
{
    //close(c_rfd);
}

static void print_system_count_get(UINT32 *Count)
{
    AmbaKAL_GetSysTickCount(Count);
}

void Print_Flush(void)
{

}

void library_init(void)
{

}

void library_destroy(void)
{

}

static void print_cpu_id_get(UINT32 *CpuID)
{
    AmbaKAL_GetSmpCpuID(CpuID);
    return;
}

static void destination_print(MESSAGE_DESTINATION_e MessageDestination, char *Buffer, UINT32 rIdx, UINT32 wIdx, UINT32 KickOff)
{
    unsigned int prio = MQ_PRIO_MAX - 1;

    // it mean ring buffer overflow
    if (KickOff == 0xFFFFFFFFU) {
        return;
    }

    if (msg_queue == -1) {
        msg_queue = mq_open(MQ_PRINT, O_RDWR | O_NONBLOCK , S_IRWXU | S_IRWXG | S_IRWXO, NULL );
        if (msg_queue == -1) {
           perror ("mq_open()");
        }
    }

    switch (MessageDestination) {
        case MESSAGE_TO_BOTH:
            // send to QNX system logger
            if (c_shm->burst_enable == 0U) {
                slogf(_SLOG_SETCODE(_SLOGC_CHAR, 0), _SLOG_INFO, Buffer);
            }
            /* no break */
        case MESSAGE_TO_PRINT_BUFFER:
            if (c_shm->burst_enable == 0U) {
                amba_print_data_t msg;
                msg.rIdx = rIdx;
                msg.wIdx = wIdx;
                mq_send(msg_queue, (char*)&msg, sizeof(amba_print_data_t), prio);
            } else {
                if ((c_shm->msg_index == 0) && (KickOff == 0)) {
                    // store the first entry
                    c_shm->msg.rIdx = rIdx;
                    c_shm->msg_index++;
                } else if ((c_shm->msg_index != 0) && (KickOff == 1)) {
                    // store the last entry
                    c_shm->msg.wIdx = wIdx;
                    mq_send(msg_queue, (char*)&(c_shm->msg), sizeof(amba_print_data_t), prio);
                    c_shm->msg_index = 0;;
                } else {
                    c_shm->msg_index++;
                    c_shm->msg.wIdx = wIdx;
                }
            }

            break;
        case MESSAGE_TO_LOG_BUFFER:
            // send to QNX system logger
            if (c_shm->burst_enable == 0U) {
                slogf(_SLOG_SETCODE(_SLOGC_CHAR, 0), _SLOG_INFO, Buffer);
            }
            break;
        default:
            // do nothing
            break;
        }

}

static void print_footer_set(char *Buffer, UINT32 BufferSize)
{
    AmbaUtility_StringAppend(Buffer, BufferSize, "\r\n");
}

static UINT32 print_header_set(char *Buffer, UINT32 BufferSize, UINT32 CpuId, UINT32 SysTick)
{
    static const char *pCpuCoreName[4] = {"CPU0", "CPU1", "CPU2", "CPU3"};
    UINT32 header_size;
    char   int_string[UTIL_MAX_INT_STR_LEN];

    Buffer[0] = '\0';

    /* Msg prefix: System time */
    AmbaUtility_StringAppend(Buffer, BufferSize, "[");

    header_size = AmbaUtility_UInt32ToStr(int_string, UTIL_MAX_INT_STR_LEN, SysTick, 10);
    if (header_size < 8U) {
        UINT32 i;
        for (i = 0; i < (8U - header_size); i++) {
            AmbaUtility_StringAppend(Buffer, BufferSize, "0");
        }
    }
    AmbaUtility_StringAppend(Buffer, BufferSize, int_string);
    AmbaUtility_StringAppend(Buffer, BufferSize, "]");

    /* Msg prefix: Processor name */
    AmbaUtility_StringAppend(Buffer, BufferSize, "[");
    AmbaUtility_StringAppend(Buffer, BufferSize, pCpuCoreName[CpuId]);
    AmbaUtility_StringAppend(Buffer, BufferSize, "] ");

    return AmbaUtility_StringLength(Buffer);
}

static void Print_MessageCopy(UINT8 *pDestination, const char *pSource, UINT32 Size)
{
    UINT32 i;
    for (i = 0; i < Size; i++) {
        pDestination[i] = (UINT8)pSource[i];
    }
}

static void Print_InsertMsg(UINT32 MsgSize, const char *pMsg, MESSAGE_DESTINATION_e MessageDestination, UINT32 *rIdx, UINT32 *wIdx, UINT32 *KickOff)
{
    UINT32 Head;
    UINT32 Tmp;
    UINT32 IsOverflow;

    if ((pMsg == NULL) || (MsgSize == 0U)) {
        // no action
    } else {
        // lock mutex
        if (pthread_mutex_lock(&c_shm->mutex_write) != EOK) {
            //no action
        } else {
            if ((MessageDestination == MESSAGE_TO_PRINT_BUFFER) || (MessageDestination == MESSAGE_TO_BOTH)) {
                /* Check if ring buffer overflow */
                IsOverflow = 0;
                // if w >= r,  (w + N) / M > 1 and (w + N) % M >= r (NG)
                if (c_shm->ring_w_idx >= c_shm->ring_r_idx) {
                    UINT32 div = (c_shm->ring_w_idx + MsgSize) / c_shm->ring_buf_size;
                    UINT32 mod = (c_shm->ring_w_idx + MsgSize) % c_shm->ring_buf_size;
                    if ((div > 0) && (mod >= c_shm->ring_r_idx)) {
                        IsOverflow = 1;
                    }
                // if w < r,  (w + N)  >= r (NG)
                } else {
                    if ((c_shm->ring_w_idx + MsgSize) >= c_shm->ring_r_idx) {
                        IsOverflow = 1;
                    }
                }

                /* Update to Print circular buffer in memory */
                if (IsOverflow == 1) {
                    printf("\n ring buffer overflow \n");
                    *rIdx = 0;
                    *wIdx = 0;
                    *KickOff = 0xFFFFFFFF;
                } else {
                    Head = c_shm->ring_w_idx;
                    if ((Head + MsgSize) >= c_shm->ring_buf_size) {
                        /* Wrap around circular buffer */
                        Tmp = c_shm->ring_buf_size - Head;
                        Print_MessageCopy(&c_ring_buf[Head], &pMsg[0], Tmp);
                        Print_MessageCopy(&c_ring_buf[0], &pMsg[Tmp], MsgSize - Tmp);
                        c_shm->ring_w_idx = MsgSize - Tmp;
                    } else {
                        /* No wrap */
                        Print_MessageCopy(&c_ring_buf[Head], pMsg, MsgSize);
                        c_shm->ring_w_idx = Head + MsgSize;
                    }
                    *rIdx = Head; // first buf address for print process
                    *wIdx = c_shm->ring_w_idx;

                    // check the kick-off strategy
                    if (c_shm->burst_enable == 0U) {
                        *KickOff = 1; // alway kikk off
                    } else {
                        c_shm->ring_buf_accumulate+= MsgSize;
                        if (c_shm->ring_buf_accumulate > (c_shm->ring_buf_size/(c_shm->ring_buf_flush_level*10))) {
                            c_shm->ring_buf_accumulate = 0;
                            *KickOff = 1;
                        } else {
                            *KickOff = 0;
                        }
                    }
                }

            }

            if ((MessageDestination == MESSAGE_TO_LOG_BUFFER) || (MessageDestination == MESSAGE_TO_BOTH)) {
                /* Update to Log circular buffer in memory */
                Head = c_shm->log_w_idx;
                if ((Head + MsgSize) >= c_shm->log_buf_size) {
                    /* Wrap around circular buffer */
                    Tmp = (c_shm->log_buf_size - 1U) - Head;
                    c_shm->log_w_idx = MsgSize - Tmp;
                    Print_MessageCopy(&c_shm->log_buf[Head], &pMsg[0], Tmp);
                    Print_MessageCopy(&c_shm->log_buf[0], &pMsg[Tmp], MsgSize - Tmp);
                } else {
                    /* No wrap */
                    c_shm->log_w_idx = Head + MsgSize;
                    Print_MessageCopy(&c_shm->log_buf[Head], pMsg, MsgSize);
                }
            }

            // unlock mutex
            if (pthread_mutex_unlock(&c_shm->mutex_write) != EOK) {
                // ignore error
            }
        }
    }

    return;
}

static UINT8 is_in_allow_list(UINT16 ModuleID)
{
    UINT16 idx_main = ModuleID / 32U;
    UINT16 shift    = ModuleID % 32U;
    UINT32 value    = (c_shm->module_allow_list[idx_main] >> shift) & 0x01U;
    UINT8  ret = PRINT_COND_NO;
    if (value != 0U) {
        ret = PRINT_COND_YES;
    }
    return ret;
}

static UINT32 AmbaPrint_ModuleSetAllowList_Impl(UINT16 ModuleID, UINT8 Enable)
{
    UINT32 uret = PRINT_ERR_SUCCESS;
    if (Enable > 1U) {
        uret = PRINT_ERR_INVALID_PARAMETER;
    } else {
        if (pthread_mutex_lock(&c_shm->mutex_print) == EOK) {
            UINT16 idx_main = ModuleID / 32U;
            UINT16 shift    = ModuleID % 32U;
            UINT32 value    = 1UL << shift;

            if (Enable == PRINT_COND_NO) {
                c_shm->module_allow_list[idx_main] &= ~value;
            } else {
                c_shm->module_allow_list[idx_main] |= value;
            }
            if (pthread_mutex_unlock(&c_shm->mutex_print) != EOK) {
                uret = PRINT_ERR_MUTEX_UNLOCK_FAIL;
            }
        } else {
            uret = PRINT_ERR_MUTEX_LOCK_FAIL;
        }
    }
    return uret;
}

UINT32 AmbaPrint_ModuleSetAllowList(UINT16 ModuleID, UINT8 Enable)
{
    print_shm_open();
    AmbaPrint_ModuleSetAllowList_Impl(ModuleID, Enable);
    print_shm_close();

    return EXIT_SUCCESS;
}

static void PrintStr5_Impl(UINT8 IsModulePrint,
                           UINT16 ModuleID,
                           const char *pFmt,
                           const char *pArg1,
                           const char *pArg2,
                           const char *pArg3,
                           const char *pArg4,
                           const char *pArg5)
{
    UINT32       CpuId, SysTick;

    print_cpu_id_get(&CpuId);
    print_system_count_get(&SysTick);
    print_shm_open();

    if (c_shm->flag_print_stop == PRINT_COND_NO) {

        MESSAGE_DESTINATION_e message_destination = MESSAGE_TO_PRINT_BUFFER;
        if (IsModulePrint) {
            UINT8 is_allowed = is_in_allow_list(ModuleID);
            if (is_allowed == PRINT_COND_YES) {
                message_destination = MESSAGE_TO_BOTH;
            } else {
                message_destination = MESSAGE_TO_LOG_BUFFER;
            }
        }

        if (pthread_mutex_lock(&c_shm->mutex_print) == EOK) {

            const char *args[5];
            UINT32  uret;
            UINT32  string_length;
            UINT32  rIdx, wIdx, KickOff;

            string_length = print_header_set(print_line_buffer, LINE_BUFFER_LENGTH, CpuId, SysTick);

            if (string_length < LINE_BUFFER_LENGTH) {
                args[0] = pArg1;
                args[1] = pArg2;
                args[2] = pArg3;
                args[3] = pArg4;
                args[4] = pArg5;

                uret = AmbaUtility_StringPrintStr(&print_line_buffer[string_length], LINE_BUFFER_LENGTH - string_length, pFmt, 5, args);
                if (uret == 0U) {
                    // ignore it?
                }

                print_footer_set(print_line_buffer, LINE_BUFFER_LENGTH);
                // footer length = 2;
                Print_InsertMsg(string_length + uret + 2, print_line_buffer, message_destination, &rIdx, &wIdx, &KickOff);
                destination_print(message_destination, print_line_buffer, rIdx, wIdx, KickOff);
            } else {
                // overflow
            }

            if (pthread_mutex_unlock(&c_shm->mutex_print) != EOK) {
                /* make misra happy */
            }
        }
    }
    print_shm_close();
}

void AmbaPrint_PrintStr5(const char *pFmt, const char *pArg1, const char *pArg2, const char *pArg3, const char *pArg4, const char *pArg5)
{
    PrintStr5_Impl(PRINT_MODE, MODULE_ID_NA, pFmt, pArg1, pArg2, pArg3, pArg4, pArg5);
}

void AmbaPrint_ModulePrintStr5(UINT16 ModuleID, const char *pFmt, const char *pArg1, const char *pArg2, const char *pArg3, const char *pArg4, const char *pArg5)
{
    PrintStr5_Impl(MODULE_PRINT_MODE, ModuleID, pFmt, pArg1, pArg2, pArg3, pArg4, pArg5);
}

static void PrintULL5_Impl(UINT8 IsModulePrint,
                           UINT16 ModuleID,
                           const char *pFmt,
                           UINT64 Arg1,
                           UINT64 Arg2,
                           UINT64 Arg3,
                           UINT64 Arg4,
                           UINT64 Arg5)
{
    UINT32       CpuId, SysTick;

    print_cpu_id_get(&CpuId);
    print_system_count_get(&SysTick);
    print_shm_open();

    if (c_shm->flag_print_stop == PRINT_COND_NO) {

        MESSAGE_DESTINATION_e message_destination = MESSAGE_TO_PRINT_BUFFER;
        if (IsModulePrint) {
            UINT8 is_allowed = is_in_allow_list(ModuleID);
            if (is_allowed == PRINT_COND_YES) {
                message_destination = MESSAGE_TO_BOTH;
            } else {
                message_destination = MESSAGE_TO_LOG_BUFFER;
            }
        }

        if (pthread_mutex_lock(&c_shm->mutex_print) == EOK) {

            UINT64  args[5];
            UINT32  uret;
            UINT32  string_length;
            UINT32  rIdx, wIdx, KickOff;

            string_length = print_header_set(print_line_buffer, LINE_BUFFER_LENGTH, CpuId, SysTick);

            if (string_length < LINE_BUFFER_LENGTH) {
                args[0] = Arg1;
                args[1] = Arg2;
                args[2] = Arg3;
                args[3] = Arg4;
                args[4] = Arg5;
                uret = AmbaUtility_StringPrintUInt64(&print_line_buffer[string_length], LINE_BUFFER_LENGTH - string_length, pFmt, 5, args);
                if (uret > 0U) {
                    // ignore it?
                }

                print_footer_set(print_line_buffer, LINE_BUFFER_LENGTH);
                // footer length = 2;
                Print_InsertMsg(string_length + uret + 2, print_line_buffer, message_destination, &rIdx, &wIdx, &KickOff);
                destination_print(message_destination, print_line_buffer, rIdx, wIdx, KickOff);
            } else {
                // overflow
            }

            // Release the Mutex
            if (pthread_mutex_unlock(&c_shm->mutex_print) != EOK) {
                /* make misra happy */
            }
        }
    }
    print_shm_close();
}

void AmbaPrint_PrintULL5(const char *pFmt, UINT64 Arg1, UINT64 Arg2, UINT64 Arg3, UINT64 Arg4, UINT64 Arg5)
{
    PrintULL5_Impl(PRINT_MODE, MODULE_ID_NA, pFmt, Arg1, Arg2, Arg3, Arg4, Arg5);
}

void AmbaPrint_ModulePrintULL5(UINT16 ModuleID, const char *pFmt, UINT64 Arg1, UINT64 Arg2, UINT64 Arg3, UINT64 Arg4, UINT64 Arg5)
{
    PrintULL5_Impl(MODULE_PRINT_MODE, ModuleID, pFmt, Arg1, Arg2, Arg3, Arg4, Arg5);
}

static void PrintUInt5_Impl(UINT8 IsModulePrint,
                            UINT16 ModuleID,
                            const char *pFmt,
                            UINT32 Arg1,
                            UINT32 Arg2,
                            UINT32 Arg3,
                            UINT32 Arg4,
                            UINT32 Arg5)
{
    UINT32       CpuId, SysTick;

    print_cpu_id_get(&CpuId);
    print_system_count_get(&SysTick);
    print_shm_open();

    if (c_shm->flag_print_stop == PRINT_COND_NO) {

        MESSAGE_DESTINATION_e message_destination = MESSAGE_TO_PRINT_BUFFER;
        if (IsModulePrint) {
            UINT8 is_allowed = is_in_allow_list(ModuleID);
            if (is_allowed == PRINT_COND_YES) {
                message_destination = MESSAGE_TO_BOTH;
            } else {
                message_destination = MESSAGE_TO_LOG_BUFFER;
            }
        }

        if (pthread_mutex_lock(&c_shm->mutex_print) == EOK) {

            UINT32  args[5];
            UINT32  uret;
            UINT32  string_length;
            UINT32  rIdx, wIdx, KickOff;

            string_length = print_header_set(print_line_buffer, LINE_BUFFER_LENGTH, CpuId, SysTick);

            if (string_length < LINE_BUFFER_LENGTH) {
                args[0] = Arg1;
                args[1] = Arg2;
                args[2] = Arg3;
                args[3] = Arg4;
                args[4] = Arg5;
                uret = AmbaUtility_StringPrintUInt32(&print_line_buffer[string_length], LINE_BUFFER_LENGTH - string_length, pFmt, 5, args);
                if (uret > 0U) {
                    // ignore it?
                }

                print_footer_set(print_line_buffer, LINE_BUFFER_LENGTH);
                // footer length = 2;
                Print_InsertMsg(string_length + uret + 2, print_line_buffer, message_destination, &rIdx, &wIdx, &KickOff);
                destination_print(message_destination, print_line_buffer, rIdx, wIdx, KickOff);
            } else {
                // overflow
            }

            // Release the Mutex
            if (pthread_mutex_unlock(&c_shm->mutex_print) != EOK) {
                /* make misra happy */
            }
        }
    }
    print_shm_close();
}

void AmbaPrint_PrintUInt5(const char *pFmt, UINT32 Arg1, UINT32 Arg2, UINT32 Arg3, UINT32 Arg4, UINT32 Arg5)
{
    PrintUInt5_Impl(PRINT_MODE, MODULE_ID_NA, pFmt, Arg1, Arg2, Arg3, Arg4, Arg5);
}

void AmbaPrint_ModulePrintUInt5(UINT16 ModuleID, const char *pFmt, UINT32 Arg1, UINT32 Arg2, UINT32 Arg3, UINT32 Arg4, UINT32 Arg5)
{
    PrintUInt5_Impl(MODULE_PRINT_MODE, ModuleID, pFmt, Arg1, Arg2, Arg3, Arg4, Arg5);
}

static void PrintInt5_Impl(UINT8 IsModulePrint,
                           UINT16 ModuleID,
                           const char *pFmt,
                           INT32 Arg1,
                           INT32 Arg2,
                           INT32 Arg3,
                           INT32 Arg4,
                           INT32 Arg5)
{
    UINT32       CpuId, SysTick;

    print_cpu_id_get(&CpuId);
    print_system_count_get(&SysTick);
    print_shm_open();

    if (c_shm->flag_print_stop == PRINT_COND_NO) {

        MESSAGE_DESTINATION_e message_destination = MESSAGE_TO_PRINT_BUFFER;
        if (IsModulePrint) {
            UINT8 is_allowed = is_in_allow_list(ModuleID);
            if (is_allowed == PRINT_COND_YES) {
                message_destination = MESSAGE_TO_BOTH;
            } else {
                message_destination = MESSAGE_TO_LOG_BUFFER;
            }
        }

        // Take the Mutex
        if (pthread_mutex_lock(&c_shm->mutex_print) == EOK) {

            INT32   args[5];
            UINT32  uret;
            UINT32  string_length;
            UINT32  rIdx, wIdx, KickOff;

            string_length = print_header_set(print_line_buffer, LINE_BUFFER_LENGTH, CpuId, SysTick);

            if (string_length < LINE_BUFFER_LENGTH) {
                args[0] = Arg1;
                args[1] = Arg2;
                args[2] = Arg3;
                args[3] = Arg4;
                args[4] = Arg5;
                uret = AmbaUtility_StringPrintInt32(&print_line_buffer[string_length], LINE_BUFFER_LENGTH - string_length, pFmt, 5, args);
                if (uret > 0U) {
                    // ignore it?
                }

                print_footer_set(print_line_buffer, LINE_BUFFER_LENGTH);
                // footer length = 2;
                Print_InsertMsg(string_length + uret + 2, print_line_buffer, message_destination, &rIdx, &wIdx, &KickOff);
                destination_print(message_destination, print_line_buffer, rIdx, wIdx, KickOff);
            } else {
                // overflow
            }

            // Release the Mutex
            if (pthread_mutex_unlock(&c_shm->mutex_print) != EOK) {
                /* make misra happy */
            }
        }
    }
    print_shm_close();
}

void AmbaPrint_PrintInt5(const char *pFmt, INT32 Arg1, INT32 Arg2, INT32 Arg3, INT32 Arg4, INT32 Arg5)
{
    PrintInt5_Impl(PRINT_MODE, MODULE_ID_NA, pFmt, Arg1, Arg2, Arg3, Arg4, Arg5);
}

void AmbaPrint_ModulePrintInt5(UINT16 ModuleID, const char *pFmt, INT32 Arg1, INT32 Arg2, INT32 Arg3, INT32 Arg4, INT32 Arg5)
{
    PrintInt5_Impl(MODULE_PRINT_MODE, ModuleID, pFmt, Arg1, Arg2, Arg3, Arg4, Arg5);
}

void AmbaPrint_StopAndFlush(void)
{
    print_shm_open();
    c_shm->flag_print_stop = PRINT_COND_YES;
    print_shm_close();
}

void AmbaPrint_Flush(void)
{
    // do nothing
}


UINT32 AmbaPrint_Init(const AMBA_PRINT_CONFIG_s *pPrintConfig)
{
    (void)pPrintConfig;

    return 0;
}
