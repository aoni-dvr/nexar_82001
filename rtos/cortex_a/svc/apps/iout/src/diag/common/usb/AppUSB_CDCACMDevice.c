/**
 *  @file AppUSB_CDCACMDevice.c
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
 *  @details USB CDC-ACM Class with single instance for APP/MW.
 */
/**
 * \page page1 CDC-ACM Device application
*/
#include "AppUSB.h"
#ifndef APPUSB_UTIL_H
#include "AppUSB_Util.h"
#endif
#include "AppUSB_SoftwareFifo.h"
#include "AmbaUSB_ErrCode.h"
#include <AmbaMisraFix.h>

//#define ENBALE_CDC_ACM_DEBUG

#define SEND_TASK_STACK_SIZE  0x4000U
#define RECV_TASK_STACK_SIZE  0x1000U
#define SHELL_TASK_STACK_SIZE 0x1000U

#define SEND_BUFSIZE          4096U
#define RECV_BUFSIZE          512U

#define SEND_TASK_TERMINATE   0x01U
#define RECV_TASK_TERMINATE   0x02U
#define CDC_ACM_CLASS_RUNNING 0x04U

#define SEND_TASK_PRIORITY    255U
#define RECV_TASK_PRIORITY    160U
#define SHELL_TASK_PRIORITY   161U
#define UER_REQUEST_EMPTY     USB_ERR_TRANSFER_TIMEOUT

#define AUTO_START  1U

typedef struct {
    UINT32 SendInterval;
    UINT32 SendBlockSize;
    UINT32 TaskRunning;
} CDC_ACM_PARAM_s;

static AMBA_KAL_TASK_t cdc_acm_task __attribute__((section(".bss.noinit")));
static AMBA_KAL_TASK_t cdc_acm_rcv_task __attribute__((section(".bss.noinit")));
static AMBA_KAL_TASK_t cdc_acm_shell_task __attribute__((section(".bss.noinit")));
static AMBA_KAL_EVENT_FLAG_t flag_cdc_acm_transfer __attribute__((section(".bss.noinit")));
static AMBA_KAL_MUTEX_t cdc_acm_mutex __attribute__((section(".bss.noinit")));

static CDC_ACM_PARAM_s cdc_acm_param = {
    200,
    1000,
    0,
};

static INT32 rxfifo_id = FIFO_ERR_INVALID_ID;
static INT32 txfifo_id = FIFO_ERR_INVALID_ID;

static CdcAcmNotify cdc_acm_notify_cb = NULL;

static UINT32 send_uart_data(void)
{
    static UINT8 buffer[SEND_BUFSIZE] __attribute__ ((aligned(32))) __attribute__((section(".bss.noinit")));

    ULONG  actual_length = 0;
    UINT8 *send_buffer  = buffer;
    UINT32 uret         = 0;
    INT32  tx_data_size;

    if (AppUsb_SwFifoOpen(txfifo_id, 'r') >= 0) {
        tx_data_size = AppUsb_SwFifoRead(txfifo_id, send_buffer, (INT32)cdc_acm_param.SendBlockSize, 5);
        if (tx_data_size > 0) {
            uret = AmbaUSBD_CDCACMWrite(send_buffer, (UINT32)tx_data_size, &actual_length, 1000);
        }
        if (AppUsb_SwFifoClose(txfifo_id, 'r') < 0) {
            // ignore error
        }
    }
    return uret;
}

static void *send_task_func(void *Arg)
{
    UINT32 uret;
    UINT32 is_open;

    AmbaMisra_TouchUnused(Arg);

    //DBG_MSG("%s(): start", __FUNCTION__);

    while (cdc_acm_param.TaskRunning == 1U) {
        uret = AmbaUSBD_CDCACMIsTerminalOpen(&is_open);
        if ((uret == USB_ERR_SUCCESS) && (is_open == 1U)) {
            uret = send_uart_data();

            if (uret == UER_REQUEST_EMPTY) {
                //DBG_MSG("%s(): Bulk Send Timeout.", __FUNCTION__);
                continue;
            }

            if (uret != USB_ERR_SUCCESS) {
                // something wrong, break.
                break;
            }
        } else {
            AppUsb_TaskSleep(cdc_acm_param.SendInterval);
        }
    }

    if (AmbaKAL_EventFlagSet(&flag_cdc_acm_transfer, SEND_TASK_TERMINATE) != 0U) {
        // ignore error
    }

    //DBG_MSG("%s(): end", __FUNCTION__);
    return NULL;
}

static void *recv_task_func(void *Arg)
{
    static UINT8 buffer[RECV_BUFSIZE] __attribute__ ((aligned(32))) __attribute__((section(".bss.noinit")));

    UINT32 uret;
    ULONG  bytes_recv   = 0;
    UINT32 is_open;
    UINT8 *recv_buffer = buffer;

    AmbaMisra_TouchUnused(Arg);

    while (cdc_acm_param.TaskRunning == 1U) {
        uret = AmbaUSBD_CDCACMIsTerminalOpen(&is_open);
        if ((uret == USB_ERR_SUCCESS) && (is_open == 1U)) {
            uret = AmbaUSBD_CDCACMRead(recv_buffer, RECV_BUFSIZE, &bytes_recv, 1000);
            if ((uret != USB_ERR_SUCCESS) && (uret != USB_ERR_TRANSFER_TIMEOUT)) {
                // something wrong, break
                AppUsb_PrintUInt5("something wrong 0x%x", uret, 0, 0, 0, 0);
                break;
            } else {
                if (uret != UER_REQUEST_EMPTY) {
                    // normal mode, push to rxfifo.
                    if (rxfifo_id >= 0) {
                        if (AppUsb_SwFifoOpen(rxfifo_id, 'w') >= 0) {
                            if (AppUsb_SwFifoWrite(rxfifo_id, recv_buffer, (INT32)bytes_recv, 1000) < 0) {
                                // ignore error
                            }
                            if (AppUsb_SwFifoClose(rxfifo_id, 'w') < 0) {
                                // ignore error
                            }
                        }
                    }

                    if ((bytes_recv == 1U) && (recv_buffer[0] == 13U)) {
                        // convert "Carriage Return" to "\r\n"
                        recv_buffer[0] = 0x0DU; // '\r';
                        recv_buffer[1] = 0x0AU; // '\n';
                        bytes_recv     = 2;
                    }

                    // echo to terminal rx data -> tx data
                    if (txfifo_id >= 0) {
                        if (AppUsb_SwFifoOpen(txfifo_id, 'w') >= 0) {
                            if (AppUsb_SwFifoWrite(txfifo_id, recv_buffer, (INT32)bytes_recv, 1000) < 0) {
                                // ignore error
                            }
                            if (AppUsb_SwFifoClose(txfifo_id, 'w') < 0) {
                                // ignore error
                            }
                        }
                    }
                }
            }
        } else {
            AppUsb_TaskSleep(100);
        }
    }

    if (AmbaKAL_EventFlagSet(&flag_cdc_acm_transfer, RECV_TASK_TERMINATE) != 0U) {
        // ignore error
    }

    //DBG_MSG("%s(): end", __FUNCTION__);
    return NULL;
}

static INT32 usb_data_write(INT32 len, const UINT8 *buf)
{
    INT32 nret = 0;

    if (txfifo_id >= 0) {
        nret = AppUsb_SwFifoOpen(txfifo_id, 'w');
        if (nret >= 0) {
            nret = AppUsb_SwFifoWrite(txfifo_id, buf, len, 1000);
            if (AppUsb_SwFifoClose(txfifo_id, 'w') != 0) {
                // something wrong when close fifo
            }
        }
    }
    return nret;
}

static INT32 usb_data_read(INT32 len, UINT8 *buf, UINT32 timeout)
{
    INT32 nret = 0;

    if (rxfifo_id >= 0) {
        nret = AppUsb_SwFifoOpen(rxfifo_id, 'r');
        if (nret >= 0) {
            nret = AppUsb_SwFifoRead(rxfifo_id, buf, len, timeout);
            if (AppUsb_SwFifoClose(rxfifo_id, 'r') != 0) {
                // something wrong when close fifo
            }
        }
    }
    return nret;
}

// This shell task is for demo purpose.
// It just save command and display something.
static void *sehll_task_func(void *Arg)
{
    UINT8 buf[100];
    UINT8 cmd[100]     = { 0 };
    INT32 nret         = 0;
    INT32 idx          = 0;
    static UINT8 str[] = { 0x44, 0x6f, 0x6e, 0x27, 0x74, 0x20, 0x6b, 0x6e,
                           0x6f, 0x77, 0x20, 0x68, 0x6f, 0x77, 0x20, 0x74,
                           0x6f, 0x20, 0x70, 0x72, 0x6f, 0x63, 0x65, 0x73,
                           0x73, 0x20, 0x27
                         };            // "Don't know how to process '";
    static UINT8 line_break_str[] = { 0x27, 0x0D, 0x0A }; // "'\r\n"
    UINT32 is_open;
    UINT32 uret;

    AmbaMisra_TouchUnused(Arg);

    while (cdc_acm_param.TaskRunning == 1U) {
        uret = AmbaUSBD_CDCACMIsTerminalOpen(&is_open);
        if ((uret == USB_ERR_SUCCESS) && (is_open == 1U)) {

            // read data
            nret = usb_data_read(1, buf, 1000);
            if (nret > 0) {
                if ((buf[0] == 0x0DU) || (buf[0] == 0x0AU)) {
                    if (idx > 0) {
                        if (usb_data_write((INT32)sizeof(str), str) <= 0) {
                            // ignore error
                        }
                        if (usb_data_write(idx, cmd) <= 0) {
                            // ignore error
                        }
                        if (usb_data_write(3, line_break_str) <= 0) {
                            // ignore error
                        }
                    }
                    // flush cmd
                    idx = 0;
                } else {
                    if (idx < 99) {
                        cmd[idx] = buf[0];
                        idx++;
                    }
                }
            } else {
                AppUsb_TaskSleep(100);
            }
        } else {
            AppUsb_TaskSleep(100);
        }
    }

    //DBG_MSG("%s(): end", __FUNCTION__);
    return NULL;
}

static void init_zero(void)
{
    AppUsb_MemoryZeroSet(&cdc_acm_task, sizeof(cdc_acm_task));
    AppUsb_MemoryZeroSet(&cdc_acm_rcv_task, sizeof(cdc_acm_rcv_task));
    AppUsb_MemoryZeroSet(&cdc_acm_shell_task, sizeof(cdc_acm_shell_task));
    AppUsb_MemoryZeroSet(&flag_cdc_acm_transfer, sizeof(flag_cdc_acm_transfer));
    AppUsb_MemoryZeroSet(&cdc_acm_mutex, sizeof(cdc_acm_mutex));
    return;
}

/** \addtogroup APPLICATION_APIS
 *  @{
 * */
/**
 * Register a callback function to be notified when CDC-ACM device class is in run or stop state.
*/
UINT32 AppCdcAcmd_NotificationRegister(CdcAcmNotify CallBackFunc)
{
    cdc_acm_notify_cb = CallBackFunc;
    return 0;
}

/**
 * Start CDC-ACM device application. It will create related OS resources.
*/
UINT32 AppCdcAcmd_Start(void)
{
    static UINT8 send_task_stack[SEND_TASK_STACK_SIZE] __attribute__((section(".bss.noinit")));
    static UINT8 recv_task_stack[RECV_TASK_STACK_SIZE] __attribute__((section(".bss.noinit")));
    static UINT8 shell_task_stack[SHELL_TASK_STACK_SIZE] __attribute__((section(".bss.noinit")));

    static char send_task_name[]  = "AmbaUSBD_RefCdcAcmSend";
    static char recv_task_name[]  = "AmbaUSBD_RefCdcAcmRecv";
    static char shell_task_name[] = "AmbaUSBD_RefCdcAcmShell";

    static char eventflag_name[] = "flag_cdc_acm_transfer";
    static char mutex_name[] = "cdc_acm_mutex";

    UINT32 uret     = USB_ERR_SUCCESS;
    INT32 fifo_size = (INT32)cdc_acm_param.SendBlockSize;

    AppUsb_SwFifoInitZero();
    init_zero();

    rxfifo_id = AppUsb_SwFifoCreate(fifo_size + 10);
    txfifo_id = AppUsb_SwFifoCreate(fifo_size + 10);

    if (flag_cdc_acm_transfer.tx_event_flags_group_id == 0U) {
        uret = AmbaKAL_EventFlagCreate(&flag_cdc_acm_transfer, eventflag_name);
        if (uret != 0U) {
            AppUsb_PrintUInt5("creat flag_cdc_acm_transfer fail 0x%2x\n", uret, 0, 0, 0, 0);
            uret = USB_ERR_FAIL;
        }
    }

    if (uret == USB_ERR_SUCCESS) {
        if (cdc_acm_mutex.tx_mutex_id == 0U) {
            uret = AmbaKAL_MutexCreate(&cdc_acm_mutex, mutex_name);
            if (uret != 0U) {
                AppUsb_PrintUInt5("creat cdc_acm_mutex fail 0x%2x\n", uret, 0, 0, 0, 0);
                uret = USB_ERR_FAIL;
            }
        }
    }

    if (uret == USB_ERR_SUCCESS) {
        cdc_acm_param.TaskRunning = 1;
        uret = AmbaKAL_EventFlagSet(&flag_cdc_acm_transfer, CDC_ACM_CLASS_RUNNING);
        if (uret != 0U) {
            uret = USB_ERR_FAIL;
        } else {
            uret = AmbaKAL_TaskCreate(&cdc_acm_task, send_task_name, SEND_TASK_PRIORITY,
                                      send_task_func, NULL, send_task_stack,
                                      SEND_TASK_STACK_SIZE, AUTO_START);
            if (uret != 0U) {
                AppUsb_PrintUInt5("creat cdc_acm_task fail 0x%2x\n", uret, 0, 0, 0, 0);
                uret = USB_ERR_FAIL;
            } else {
                AppUsb_TaskSleep(100);
                uret = AmbaKAL_TaskCreate(&cdc_acm_rcv_task, recv_task_name, RECV_TASK_PRIORITY,
                                          recv_task_func, NULL, recv_task_stack,
                                          RECV_TASK_STACK_SIZE, AUTO_START);
                if (uret != 0U) {
                    AppUsb_PrintUInt5("creat cdc_acm_rcv_task fail 0x%2x\n", uret, 0, 0, 0, 0);
                    uret = USB_ERR_FAIL;
                } else {
                    uret = AmbaKAL_TaskCreate(&cdc_acm_shell_task, shell_task_name, SHELL_TASK_PRIORITY,
                                              sehll_task_func, NULL, shell_task_stack,
                                              SHELL_TASK_STACK_SIZE, AUTO_START);
                    if (uret != 0U) {
                        AppUsb_PrintUInt5("creat cdc_acm_shell_task fail 0x%2x\n", uret, 0, 0, 0, 0);
                        uret = USB_ERR_FAIL;
                    } else {
                        AppUsb_PrintUInt5("==============================================", 0, 0, 0, 0, 0);
                        AppUsb_PrintUInt5("AppCdcAcmd_Start(): switch to CDC_ACM mode.", 0, 0, 0, 0, 0);
                        AppUsb_PrintUInt5("AppCdcAcmd_Start(): Broken Shell starts.", 0, 0, 0, 0, 0);
                        AppUsb_PrintUInt5("==============================================", 0, 0, 0, 0, 0);
                        if (cdc_acm_notify_cb != NULL) {
                            cdc_acm_notify_cb(1);
                        }
                    }
                }
            }
        }
    }

    return uret;
}

/**
 * Stop CDC-ACM device application. It will release related OS resources.
*/
UINT32 AppCdcAcmd_Stop(void)
{
    INT32  func_nret;
    UINT32 uret = USB_ERR_SUCCESS;
    UINT32 func_uret;
    UINT32 actual_flags;

    cdc_acm_param.TaskRunning = 0;

    AppUsb_PrintUInt5("==============================================", 0, 0, 0, 0, 0);
    AppUsb_PrintUInt5("AppCdcAcmd_Stop(): switch back to UART mode.", 0, 0, 0, 0, 0);
    AppUsb_PrintUInt5("==============================================", 0, 0, 0, 0, 0);
    if (cdc_acm_notify_cb != NULL) {
        cdc_acm_notify_cb(0);
    }

    /* wait for usb cdc task to stop */
    //todo: timeout should be replaced by AMBA_KAL_WAIT_FOREVER
    // To pass misra-c "unsigned integer literal without a 'U' suffix" issue, use 0xFFFFFFFFU instead of AMBA_KAL_WAIT_FOREVER.
    // AMBA_KAL_AND_CLEAR: AnyOrAll = 1; AutoClear = 1;
    func_uret = AmbaKAL_EventFlagGet(&flag_cdc_acm_transfer, SEND_TASK_TERMINATE | RECV_TASK_TERMINATE,
                                     1U, 1U, &actual_flags, 0xFFFFFFFFU);

    if (func_uret != 0U) {
        uret = USB_ERR_FAIL;
    }

    /* Terminate and delete Task*/
    func_uret = AmbaKAL_TaskTerminate(&cdc_acm_task);
    if (func_uret != 0U) {
        uret = USB_ERR_FAIL;
    }
    func_uret = AmbaKAL_TaskDelete(&cdc_acm_task);
    if (func_uret != 0U) {
        uret = USB_ERR_FAIL;
    }
    cdc_acm_task.tx_thread_id = 0;

    func_uret = AmbaKAL_TaskTerminate(&cdc_acm_rcv_task);
    if (func_uret != 0U) {
        uret = USB_ERR_FAIL;
    }
    func_uret = AmbaKAL_TaskDelete(&cdc_acm_rcv_task);
    if (func_uret != 0U) {
        uret = USB_ERR_FAIL;
    }
    cdc_acm_rcv_task.tx_thread_id = 0;

    func_uret = AmbaKAL_TaskTerminate(&cdc_acm_shell_task);
    if (func_uret != 0U) {
        uret = USB_ERR_FAIL;
    }
    func_uret = AmbaKAL_TaskDelete(&cdc_acm_shell_task);
    if (func_uret != 0U) {
        uret = USB_ERR_FAIL;
    }
    cdc_acm_shell_task.tx_thread_id = 0;

    /* delete mutex */
    if (cdc_acm_mutex.tx_mutex_id != 0U) {
        func_uret = AmbaKAL_MutexDelete(&cdc_acm_mutex);
        if (func_uret != 0U) {
            uret = USB_ERR_FAIL;
        }
        cdc_acm_mutex.tx_mutex_id = 0;
    }

    /* delete flag */
    func_uret = AmbaKAL_EventFlagDelete(&flag_cdc_acm_transfer);
    if (func_uret != 0U) {
        uret = USB_ERR_FAIL;
    }
    flag_cdc_acm_transfer.tx_event_flags_group_id = 0;

    func_nret = AppUsb_SwFifoDestroy(txfifo_id);
    if (func_nret != 0) {
        uret = USB_ERR_FAIL;
    }
    func_nret = AppUsb_SwFifoDestroy(rxfifo_id);
    if (func_nret != 0) {
        uret = USB_ERR_FAIL;
    }
    rxfifo_id = FIFO_ERR_INVALID_ID;
    txfifo_id = FIFO_ERR_INVALID_ID;

    return uret;
}


/**
 * A test function for sending data. It is used for unittest. Don't use it in normal use case.
 * @param Timeout
*/
void AppCdcAcmd_TestSend(UINT32 Timeout)
{
    UINT32 uret;
    ULONG  actual_length;
    static UINT8 data[4] = {0x30, 0x31, 0x0D, 0x0A};
    uret = AmbaUSBD_CDCACMWrite(data, 4, &actual_length, Timeout);
    switch (uret) {
    case USB_ERR_TRANSFER_TIMEOUT:
        AppUsb_PrintUInt5("AppCdcAcmd_TestSend(): Timeout", 0, 0, 0, 0, 0);
        break;
    case USB_ERR_TRANSFER_BUS_RESET:
        AppUsb_PrintUInt5("AppCdcAcmd_TestSend(): Bus Reset", 0, 0, 0, 0, 0);
        break;
    case USB_ERR_SUCCESS:
        AppUsb_PrintUInt5("AppCdcAcmd_TestSend(): Success", 0, 0, 0, 0, 0);
        break;
    default:
        AppUsb_PrintUInt5("AppCdcAcmd_TestSend(): 0x%X", uret, 0, 0, 0, 0);
        break;
    }
    return;
}
/**
 * A test function for receive data. It is used for unittest. Don't use it in normal use case.
 * @param Timeout
*/
void AppCdcAcmd_TestReceive(UINT32 Timeout)
{
    UINT32 uret;
    ULONG  actual_length;
    static UINT8 data[4];
    uret = AmbaUSBD_CDCACMRead(data, 4, &actual_length, Timeout);
    switch (uret) {
    case USB_ERR_TRANSFER_TIMEOUT:
        AppUsb_PrintUInt5("AppCdcAcmd_TestReceive(): Timeout", 0, 0, 0, 0, 0);
        break;
    case USB_ERR_TRANSFER_BUS_RESET:
        AppUsb_PrintUInt5("AppCdcAcmd_TestReceive(): Bus Reset", 0, 0, 0, 0, 0);
        break;
    case USB_ERR_SUCCESS:
        AppUsb_PrintUInt5("AppCdcAcmd_TestSend(): Success", 0, 0, 0, 0, 0);
        break;
    default:
        AppUsb_PrintUInt5("AppCdcAcmd_TestReceive(): 0x%X", uret, 0, 0, 0, 0);
        break;
    }
    return;
}
/** @} */
