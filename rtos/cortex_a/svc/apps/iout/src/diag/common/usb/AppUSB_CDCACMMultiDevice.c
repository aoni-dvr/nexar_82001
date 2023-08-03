/**
 *  @file AppUSB_CDCACMMultiDevice.c
 *
 *  @copyright Copyright (c) 2021 Ambarella, Inc.
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
 *  @details USB CDC-ACM Multi-Instance Class with single instance for APP/MW.
 */
/**
 * \page page1 CDC-ACM Multi-Instance Device application
*/
#include "AppUSB.h"
#ifndef APPUSB_UTIL_H
#include "AppUSB_Util.h"
#endif
#include "AppUSB_SoftwareFifo.h"
#include "AmbaUSB_ErrCode.h"
#include <AmbaMisraFix.h>
#include <AmbaUtility.h>

//#define ENBALE_CDC_ACM_DEBUG

#define CDC_ACM_MAX_INSTANCE_NUMBER (2U)

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
    AMBA_KAL_TASK_t SendTask;
    AMBA_KAL_TASK_t RecvTask;
    AMBA_KAL_TASK_t ShellTask;
    AMBA_KAL_EVENT_FLAG_t TransferFlag;
    AMBA_KAL_MUTEX_t Mutex;
    UINT32 TaskArg;
    UINT8  SendTaskStack[SEND_TASK_STACK_SIZE];
    UINT8  RecvTaskStack[RECV_TASK_STACK_SIZE];
    UINT8  ShellTaskStack[SHELL_TASK_STACK_SIZE];
    UINT8  SendBuf[SEND_BUFSIZE];
    UINT8  RecvBuf[RECV_BUFSIZE];
    char SendTaskName[64];
    char RecvTaskName[64];
    char ShellTaskName[64];
    INT32 RxFifoID;
    INT32 TxFifoID;
} CDC_ACM_MULTI_PARAM_s;

static CDC_ACM_MULTI_PARAM_s cdc_acm_multi_param[CDC_ACM_MAX_INSTANCE_NUMBER];

static CdcAcmMultiNotify cdc_acm_notify_cb = NULL;

static UINT32 send_uart_data(UINT32 InstanceID, CDC_ACM_MULTI_PARAM_s *Param)
{
    ULONG  actual_length = 0;
    UINT8 *send_buffer  = Param->SendBuf;
    UINT32 uret         = 0;
    INT32  tx_data_size;

    if (AppUsb_SwFifoOpen(Param->TxFifoID, 'r') >= 0) {
        tx_data_size = AppUsb_SwFifoRead(Param->TxFifoID, send_buffer, (INT32)Param->SendBlockSize, 5);
        if (tx_data_size > 0) {
            uret = AppCdcAcmMultid_Write(InstanceID, send_buffer, (UINT32)tx_data_size, &actual_length, 1000);
        }
        if (AppUsb_SwFifoClose(Param->TxFifoID, 'r') < 0) {
            // ignore error
        }
    }
    return uret;
}

static void *send_task_func(void *Arg)
{
    UINT32 uret;
    UINT32 is_open;
    UINT32 instance_id = *(UINT32 *)Arg;
    CDC_ACM_MULTI_PARAM_s *param;

    if (instance_id < CDC_ACM_MAX_INSTANCE_NUMBER) {
        param = &cdc_acm_multi_param[instance_id];
        while (param->TaskRunning == 1U) {
            uret = AppCdcAcmMultid_IsTerminalOpen(instance_id, &is_open);
            //AppUsb_PrintUInt5("[Send][%d]: is_open = %d", instance_id, is_open, 0, 0, 0);
            if ((uret == USB_ERR_SUCCESS) && (is_open == 1U)) {
                uret = send_uart_data(instance_id, param);

                if (uret == UER_REQUEST_EMPTY) {
                    //DBG_MSG("%s(): Bulk Send Timeout.", __FUNCTION__);
                    continue;
                }

                if (uret != USB_ERR_SUCCESS) {
                    // something wrong, break.
                    break;
                }

            }
            AppUsb_TaskSleep(param->SendInterval);
        }

        if (AmbaKAL_EventFlagSet(&param->TransferFlag, SEND_TASK_TERMINATE) != 0U) {
            // ignore error
        }
    } else {
        AppUsb_PrintUInt5("send_task_func(): invalid instance id %d", instance_id, 0, 0, 0, 0);
    }

    return NULL;
}

static void *recv_task_func(void *Arg)
{
    UINT32 uret;
    ULONG  bytes_recv   = 0;
    UINT32 is_open;
    UINT8 *recv_buffer;
    UINT32 instance_id = *(UINT32 *)Arg;
    CDC_ACM_MULTI_PARAM_s *param;

    if (instance_id < CDC_ACM_MAX_INSTANCE_NUMBER) {
        param = &cdc_acm_multi_param[instance_id];
        recv_buffer = param->RecvBuf;
        while (param->TaskRunning == 1U) {
            uret = AppCdcAcmMultid_IsTerminalOpen(instance_id, &is_open);
            if ((uret == USB_ERR_SUCCESS) && (is_open == 1U)) {
                uret = AppCdcAcmMultid_Read(instance_id, recv_buffer, RECV_BUFSIZE, &bytes_recv, 1000);
                if ((uret != USB_ERR_SUCCESS) && (uret != USB_ERR_TRANSFER_TIMEOUT)) {
                    // something wrong, break
                    AppUsb_PrintUInt5("[%d] something wrong 0x%x", instance_id, uret, 0, 0, 0);
                    break;
                } else {
                    if (uret != UER_REQUEST_EMPTY) {
                        // normal mode, push to rxfifo.
                        if (param->RxFifoID >= 0) {
                            if (AppUsb_SwFifoOpen(param->RxFifoID, 'w') >= 0) {
                                if (AppUsb_SwFifoWrite(param->RxFifoID, recv_buffer, (INT32)bytes_recv, 1000) < 0) {
                                    // ignore error
                                }
                                if (AppUsb_SwFifoClose(param->RxFifoID, 'w') < 0) {
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
                        if (param->TxFifoID >= 0) {
                            if (AppUsb_SwFifoOpen(param->TxFifoID, 'w') >= 0) {
                                if (AppUsb_SwFifoWrite(param->TxFifoID, recv_buffer, (INT32)bytes_recv, 1000) < 0) {
                                    // ignore error
                                }
                                if (AppUsb_SwFifoClose(param->TxFifoID, 'w') < 0) {
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
        if (AmbaKAL_EventFlagSet(&param->TransferFlag, RECV_TASK_TERMINATE) != 0U) {
            // ignore error
        }

    } else {
        AppUsb_PrintUInt5("recv_task_func(): invalid instance id %d", instance_id, 0, 0, 0, 0);
    }
    return NULL;
}

static INT32 usb_data_write(INT32 TxFifoID, INT32 len, const UINT8 *buf)
{
    INT32 nret = 0;

    if (TxFifoID >= 0) {
        nret = AppUsb_SwFifoOpen(TxFifoID, 'w');
        if (nret >= 0) {
            nret = AppUsb_SwFifoWrite(TxFifoID, buf, len, 1000);
            if (AppUsb_SwFifoClose(TxFifoID, 'w') != 0) {
                // something wrong when close fifo
            }
        }
    }
    return nret;
}

static INT32 usb_data_read(INT32 RxFifoID, INT32 len, UINT8 *buf, UINT32 timeout)
{
    INT32 nret = 0;

    if (RxFifoID >= 0) {
        nret = AppUsb_SwFifoOpen(RxFifoID, 'r');
        if (nret >= 0) {
            nret = AppUsb_SwFifoRead(RxFifoID, buf, len, timeout);
            if (AppUsb_SwFifoClose(RxFifoID, 'r') != 0) {
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
    UINT32 instance_id = *(UINT32 *)Arg;
    CDC_ACM_MULTI_PARAM_s *param;

    if (instance_id < CDC_ACM_MAX_INSTANCE_NUMBER) {
        param = &cdc_acm_multi_param[instance_id];
        while (param->TaskRunning == 1U) {
            uret = AppCdcAcmMultid_IsTerminalOpen(instance_id, &is_open);
            if ((uret == USB_ERR_SUCCESS) && (is_open == 1U)) {
                // read data
                nret = usb_data_read(param->RxFifoID, 1, buf, 1000);
                if (nret > 0) {
                    if ((buf[0] == 0x0DU) || (buf[0] == 0x0AU)) {
                        if (idx > 0) {
                            if (usb_data_write(param->TxFifoID, (INT32)sizeof(str), str) <= 0) {
                                // ignore error
                            }
                            if (usb_data_write(param->TxFifoID, idx, cmd) <= 0) {
                                // ignore error
                            }
                            if (usb_data_write(param->TxFifoID, 3, line_break_str) <= 0) {
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
    } else {
        AppUsb_PrintUInt5("sehll_task_func(): invalid instance id %d", instance_id, 0, 0, 0, 0);
    }

    return NULL;
}

static void init_zero(void)
{
    UINT32 i;
    for (i = 0; i < CDC_ACM_MAX_INSTANCE_NUMBER; i++) {
        cdc_acm_multi_param[i].SendInterval  = 200;
        cdc_acm_multi_param[i].SendBlockSize = 1000;
        cdc_acm_multi_param[i].TaskRunning = 0;
        cdc_acm_multi_param[i].TaskArg  = i;
        cdc_acm_multi_param[i].RxFifoID = FIFO_ERR_INVALID_ID;
        cdc_acm_multi_param[i].TxFifoID = FIFO_ERR_INVALID_ID;

        AppUsb_MemoryZeroSet(&cdc_acm_multi_param[i].SendTask, sizeof(AMBA_KAL_TASK_t));
        AppUsb_MemoryZeroSet(&cdc_acm_multi_param[i].RecvTask, sizeof(AMBA_KAL_TASK_t));
        AppUsb_MemoryZeroSet(&cdc_acm_multi_param[i].ShellTask, sizeof(AMBA_KAL_TASK_t));
        AppUsb_MemoryZeroSet(&cdc_acm_multi_param[i].TransferFlag, sizeof(AMBA_KAL_EVENT_FLAG_t));
        AppUsb_MemoryZeroSet(&cdc_acm_multi_param[i].Mutex, sizeof(AMBA_KAL_MUTEX_t));

        AmbaUtility_StringCopy(cdc_acm_multi_param[i].SendTaskName, sizeof(cdc_acm_multi_param[i].SendTaskName),  "AmbaUSBD_RefCdcAcmSend");
        AppUsb_StringAppendUInt32(cdc_acm_multi_param[i].SendTaskName, sizeof(cdc_acm_multi_param[i].SendTaskName), i, 10);

        AmbaUtility_StringCopy(cdc_acm_multi_param[i].RecvTaskName, sizeof(cdc_acm_multi_param[i].RecvTaskName), "AmbaUSBD_RefCdcAcmRecv");
        AppUsb_StringAppendUInt32(cdc_acm_multi_param[i].RecvTaskName, sizeof(cdc_acm_multi_param[i].RecvTaskName), i, 10);

        AmbaUtility_StringCopy(cdc_acm_multi_param[i].ShellTaskName, sizeof(cdc_acm_multi_param[i].ShellTaskName), "AmbaUSBD_RefCdcAcmShell");
        AppUsb_StringAppendUInt32(cdc_acm_multi_param[i].ShellTaskName, sizeof(cdc_acm_multi_param[i].ShellTaskName), i, 10);

    }
    return;
}

/** \addtogroup APPLICATION_APIS
 *  @{
 * */
/**
 * Register a callback function to be notified when CDC-ACM device class is in run or stop state.
*/
UINT32 AppCdcAcmMultid_NotificationRegister(CdcAcmMultiNotify CallBackFunc)
{
    cdc_acm_notify_cb = CallBackFunc;
    return 0;
}

/**
 * Start CDC-ACM device application. It will create related OS resources.
*/
UINT32 AppCdcAcmMultid_Start(void)
{

    static char eventflag_name[] = "flag_cdc_acm_transfer";
    static char mutex_name[] = "cdc_acm_mutex";

    UINT32 uret     = USB_ERR_SUCCESS;
    UINT32 i;
    INT32 fifo_size;
    CDC_ACM_MULTI_PARAM_s *param;

    AppUsb_SwFifoInitZero();
    init_zero();

    for (i = 0; i < CDC_ACM_MAX_INSTANCE_NUMBER; i++) {

        param = &cdc_acm_multi_param[i];

        fifo_size = (INT32)param->SendBlockSize;

        param->RxFifoID = AppUsb_SwFifoCreate(fifo_size + 10);
        param->TxFifoID = AppUsb_SwFifoCreate(fifo_size + 10);

        if ((param->RxFifoID == -1) || (param->TxFifoID == -1)) {
            AppUsb_PrintUInt5("[%d] Error: TXFIFO %d, RXFIFO %d", i, param->TxFifoID, param->RxFifoID, 0, 0);
        }

        if (param->TransferFlag.tx_event_flags_group_id == 0U) {
            uret = AmbaKAL_EventFlagCreate(&param->TransferFlag, eventflag_name);
            if (uret != 0U) {
                AppUsb_PrintUInt5("[%d] creat TransferFlag fail 0x%2x\n", i, uret, 0, 0, 0);
                uret = USB_ERR_FAIL;
            }
        }

        if (uret == USB_ERR_SUCCESS) {
            if (param->Mutex.tx_mutex_id == 0U) {
                uret = AmbaKAL_MutexCreate(&param->Mutex, mutex_name);
                if (uret != 0U) {
                    AppUsb_PrintUInt5("[%d] creat cdc_acm_mutex fail 0x%2x\n", i, uret, 0, 0, 0);
                    uret = USB_ERR_FAIL;
                }
            }
        }

        if (uret == USB_ERR_SUCCESS) {
            param->TaskRunning = 1;
            uret = AmbaKAL_EventFlagSet(&param->TransferFlag, CDC_ACM_CLASS_RUNNING);
            if (uret != 0U) {
                uret = USB_ERR_FAIL;
            } else {
                uret = AmbaKAL_TaskCreate(&param->SendTask, param->SendTaskName, SEND_TASK_PRIORITY,
                                          send_task_func, &param->TaskArg, param->SendTaskStack,
                                          SEND_TASK_STACK_SIZE, AUTO_START);
                if (uret != 0U) {
                    AppUsb_PrintUInt5("[%d] creat cdc_acm_task fail 0x%2x\n", i, uret, 0, 0, 0);
                    uret = USB_ERR_FAIL;
                } else {
                    AppUsb_TaskSleep(100);
                    uret = AmbaKAL_TaskCreate(&param->RecvTask, param->RecvTaskName, RECV_TASK_PRIORITY,
                                              recv_task_func, &param->TaskArg, param->RecvTaskStack,
                                              RECV_TASK_STACK_SIZE, AUTO_START);
                    if (uret != 0U) {
                        AppUsb_PrintUInt5("[%d] creat cdc_acm_rcv_task fail 0x%2x\n", i, uret, 0, 0, 0);
                        uret = USB_ERR_FAIL;
                    } else {
                        uret = AmbaKAL_TaskCreate(&param->ShellTask, param->ShellTaskName, SHELL_TASK_PRIORITY,
                                                  sehll_task_func, &param->TaskArg, param->ShellTaskStack,
                                                  SHELL_TASK_STACK_SIZE, AUTO_START);
                        if (uret != 0U) {
                            AppUsb_PrintUInt5("[%d] creat cdc_acm_shell_task fail 0x%2x\n", i, uret, 0, 0, 0);
                            uret = USB_ERR_FAIL;
                        } else {
                            AppUsb_Print("==============================================");
                            AppUsb_PrintUInt5("AppCdcAcmd_Start(): [%d] switch to CDC_ACM mode.", i, 0, 0, 0, 0);
                            AppUsb_PrintUInt5("AppCdcAcmd_Start(): [%d] Broken Shell starts.", i, 0, 0, 0, 0);
                            AppUsb_Print("==============================================");
                            if (cdc_acm_notify_cb != NULL) {
                                cdc_acm_notify_cb(1);
                            }
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
UINT32 AppCdcAcmMultid_Stop(void)
{
    INT32  func_nret;
    UINT32 uret = USB_ERR_SUCCESS;
    UINT32 func_uret;
    UINT32 actual_flags;
    UINT32 i;
    CDC_ACM_MULTI_PARAM_s *param;

    for (i = 0; i < CDC_ACM_MAX_INSTANCE_NUMBER; i++) {

        param = &cdc_acm_multi_param[i];

        param->TaskRunning = 0;

        AppUsb_Print("==============================================");
        AppUsb_PrintUInt5("AppCdcAcmMultid_Stop(): [%d] switch back to UART mode.", i, 0, 0, 0, 0);
        AppUsb_Print("==============================================");
        if (cdc_acm_notify_cb != NULL) {
            cdc_acm_notify_cb(0);
        }

        /* wait for usb cdc task to stop */
        //todo: timeout should be replaced by AMBA_KAL_WAIT_FOREVER
        // To pass misra-c "unsigned integer literal without a 'U' suffix" issue, use 0xFFFFFFFFU instead of AMBA_KAL_WAIT_FOREVER.
        // AMBA_KAL_AND_CLEAR: AnyOrAll = 1; AutoClear = 1;
        func_uret = AmbaKAL_EventFlagGet(&param->TransferFlag, SEND_TASK_TERMINATE | RECV_TASK_TERMINATE,
                                         1U, 1U, &actual_flags, 0xFFFFFFFFU);

        if (func_uret != 0U) {
            uret = USB_ERR_FAIL;
        }

        /* Terminate and delete Task*/
        func_uret = AmbaKAL_TaskTerminate(&param->SendTask);
        if (func_uret != 0U) {
            uret = USB_ERR_FAIL;
        }
        func_uret = AmbaKAL_TaskDelete(&param->SendTask);
        if (func_uret != 0U) {
            uret = USB_ERR_FAIL;
        }
        param->SendTask.tx_thread_id = 0;

        func_uret = AmbaKAL_TaskTerminate(&param->RecvTask);
        if (func_uret != 0U) {
            uret = USB_ERR_FAIL;
        }
        func_uret = AmbaKAL_TaskDelete(&param->RecvTask);
        if (func_uret != 0U) {
            uret = USB_ERR_FAIL;
        }
        param->RecvTask.tx_thread_id = 0;

        func_uret = AmbaKAL_TaskTerminate(&param->SendTask);
        if (func_uret != 0U) {
            uret = USB_ERR_FAIL;
        }
        func_uret = AmbaKAL_TaskDelete(&param->SendTask);
        if (func_uret != 0U) {
            uret = USB_ERR_FAIL;
        }
        param->SendTask.tx_thread_id = 0;

        /* delete mutex */
        if (param->Mutex.tx_mutex_id != 0U) {
            func_uret = AmbaKAL_MutexDelete(&param->Mutex);
            if (func_uret != 0U) {
                uret = USB_ERR_FAIL;
            }
            param->Mutex.tx_mutex_id = 0;
        }

        /* delete flag */
        func_uret = AmbaKAL_EventFlagDelete(&param->TransferFlag);
        if (func_uret != 0U) {
            uret = USB_ERR_FAIL;
        }
        param->TransferFlag.tx_event_flags_group_id = 0;

        func_nret = AppUsb_SwFifoDestroy(param->TxFifoID);
        if (func_nret != 0) {
            uret = USB_ERR_FAIL;
        }
        func_nret = AppUsb_SwFifoDestroy(param->RxFifoID);
        if (func_nret != 0) {
            uret = USB_ERR_FAIL;
        }
        param->TxFifoID = FIFO_ERR_INVALID_ID;
        param->RxFifoID = FIFO_ERR_INVALID_ID;
    }
    return uret;
}

/** @} */
