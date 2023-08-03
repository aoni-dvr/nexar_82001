/**
 *  @file AmbaUSB_ErrCode.h
 *
 *  Copyright (c) 2020 Ambarella International LP
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
 *  @details USB kernel error code header file.
 */

#ifndef AMBAUSB_ERRCODE_H
#define AMBAUSB_ERRCODE_H

#ifndef AMBA_ERROR_CODE_H
#include <AmbaErrorCode.h>
#endif

#define USB_ERR_START                                                   USB_ERR_BASE
#define USB_ERR_SUCCESS                                                 0U
#define USB_ERR_FAIL                                                    (USB_ERR_START + 0x0FFFFU)
#define USB_ERR_PARAMETER_INVALID                                       (USB_ERR_START + 0x0100U)
#define USB_ERR_PARAMETER_NULL                                          (USB_ERR_START + 0x0101U)
#define USB_ERR_COREMASK_ZERO                                           (USB_ERR_START + 0x0102U)
#define USB_ERR_NO_INIT                                                 (USB_ERR_START + 0x0103U)
#define USB_ERR_PARAMETER_ZERO                                          (USB_ERR_START + 0x0104U)
#define USB_ERR_NO_MEMORY_POOL                                          (USB_ERR_START + 0x0106U)
#define USB_ERR_MEMORY_ALLOCATE                                         (USB_ERR_START + 0x0107U)
#define USB_ERR_MEMORY_RELEASE                                          (USB_ERR_START + 0x0108U)
#define USB_ERR_INVALID_ENDPOINT_ADDRESS                                (USB_ERR_START + 0x0109U)
#define USB_ERR_NO_INSTANCE                                             (USB_ERR_START + 0x010AU)
#define USB_ERR_REACH_MAX_NUMBER                                        (USB_ERR_START + 0x010BU)
#define USB_ERR_MEDIA_NOT_READY                                         (USB_ERR_START + 0x010CU)
#define USB_ERR_MUTEX_LOCK_FAIL                                         (USB_ERR_START + 0x010DU)
#define USB_ERR_MUTEX_UNLOCK_FAIL                                       (USB_ERR_START + 0x010EU)
#define USB_ERR_MUTEX_LOCK_TIMEOUT                                      (USB_ERR_START + 0x010FU)
#define USB_ERR_DRV_MUTEX_DELETE_FAIL                                   (USB_ERR_START + 0x0110U)
#define USB_ERR_NO_PHY_ENDPOINT_AVAILABLE                               (USB_ERR_START + 0x0112U)
#define USB_ERR_VBUS_TASK_CREATE_FAIL                                   (USB_ERR_START + 0x0113U)
#define USB_ERR_VBUS_SEMA_CREATE_FAIL                                   (USB_ERR_START + 0x0114U)
#define USB_ERR_VBUS_ISR_HOOK_FAIL                                      (USB_ERR_START + 0x0116U)
#define USB_ERR_VBUS_TASK_SUSPEND_FAIL                                  (USB_ERR_START + 0x0117U)
#define USB_ERR_VBUS_TASK_TERMINATE_FAIL                                (USB_ERR_START + 0x0118U)
#define USB_ERR_VBUS_TASK_DELETE_FAIL                                   (USB_ERR_START + 0x0119U)
#define USB_ERR_VBUS_MUTEX_CREATE_FAIL                                  (USB_ERR_START + 0x011AU)
#define USB_ERR_VBUS_MUTEX_LOCK_FAIL                                    (USB_ERR_START + 0x011BU)
#define USB_ERR_VBUS_MUTEX_UNLOCK_FAIL                                  (USB_ERR_START + 0x011CU)
#define USB_ERR_VBUS_MUTEX_DELETE_FAIL                                  (USB_ERR_START + 0x011DU)
#define USB_ERR_VBUS_SEMA_DELETE_FAIL                                   (USB_ERR_START + 0x011EU)
//#define USB_ERR_USBD_ISR_HOOK_FAIL                                    (USB_ERR_START + 0x011FU)
#define USB_ERR_ISR_TASK_CREATE_FAIL                                    (USB_ERR_START + 0x011EU)
#define USB_ERR_ISR_TASK_TERMINATE_FAIL                                 (USB_ERR_START + 0x0121U)
#define USB_ERR_ISR_TASK_DELETE_FAIL                                    (USB_ERR_START + 0x0122U)
#define USB_ERR_ISR_QUEUE_CREATE_FAIL                                   (USB_ERR_START + 0x0121U)
#define USB_ERR_ISR_QUEUE_DELETE_FAIL                                   (USB_ERR_START + 0x0124U)
#define USB_ERR_CTRL_TASK_CREATE_FAIL                                   (USB_ERR_START + 0x0124U)
#define USB_ERR_CTRL_TASK_DELETE_FAIL                                   (USB_ERR_START + 0x0126U)
#define USB_ERR_CTRL_EVENTFLAG_CREATE_FAIL                              (USB_ERR_START + 0x0126U)
#define USB_ERR_CTRL_EVENTFLAG_DELETE_FAIL                              (USB_ERR_START + 0x0128U)
#define USB_ERR_RX_MUTEX_CREATE_FAIL                                    (USB_ERR_START + 0x0129U)
#define USB_ERR_RX_MUTEX_LOCK_FAIL                                      (USB_ERR_START + 0x012AU)
#define USB_ERR_RX_MUTEX_UNLOCK_FAIL                                    (USB_ERR_START + 0x012BU)
#define USB_ERR_RX_MUTEX_DELETE_FAIL                                    (USB_ERR_START + 0x012CU)
#define USB_ERR_RX_STATE_MUTEX_CREATE_FAIL                              (USB_ERR_START + 0x012DU)
#define USB_ERR_RX_STATE_MUTEX_LOCK_FAIL                                (USB_ERR_START + 0x012EU)
#define USB_ERR_RX_STATE_MUTEX_UNLOCK_FA                                (USB_ERR_START + 0x012FU)
#define USB_ERR_RX_STATE_MUTEX_DELETE_FAIL                              (USB_ERR_START + 0x0130U)
#define USB_ERR_TRANSFER_TIMEOUT                                        (USB_ERR_START + 0x0143U)
#define USB_ERR_TRANSFER_BUS_RESET                                      (USB_ERR_START + 0x0144U)
#define USB_ERR_INVALID_CLASS_ID                                        (USB_ERR_START + 0x0145U)
#define USB_ERR_DEVICE_BNA                                              (USB_ERR_START + 0x0147U)
#define USB_ERR_MTP_UNSUPPORTED_COMMAND                                 (USB_ERR_START + 0x0148U)
#define USB_ERR_TXFIFO_OVERFLOW                                         (USB_ERR_START + 0x0149U)
#define USB_ERR_BUF_NOT_CACHE_ALIGNED                                   (USB_ERR_START + 0x014AU)
#define USB_ERR_SEMA_TAKE_FAIL                                          (USB_ERR_START + 0x014BU)
#define USB_ERR_SEMA_GIVE_FAIL                                          (USB_ERR_START + 0x014CU)
#define USB_ERR_SEMA_TAKE_TIMEOUT                                       (USB_ERR_START + 0x014DU)
#define USB_ERR_HOST_IS_RUNNING                                         (USB_ERR_START + 0x014EU)
#define USB_ERR_DEVICE_IS_RUNNING                                       (USB_ERR_START + 0x014FU)
#define USB_ERR_ISR_TASK_SUSPEND_FAIL                                   (USB_ERR_START + 0x0150U)
#define USB_ERR_CTRL_TASK_SUSPEND_FAIL                                  (USB_ERR_START + 0x0151U)
#define USB_ERR_VBUS_TASK_NOT_RUNNING                                   (USB_ERR_START + 0x0164U)
#define USB_ERR_ISR_TASK_NOT_RUNNING                                    (USB_ERR_START + 0x0165U)
#define USB_ERR_CTRL_TASK_NOT_RUNNING                                   (USB_ERR_START + 0x0166U)
#define USB_ERR_RTSL_MUTEX_CREATE_FAIL                                  (USB_ERR_START + 0x0180U)
#define USB_ERR_RTSL_MUTEX_DELETE_FAIL                                  (USB_ERR_START + 0x0181U)
#define USB_ERR_EVENT_FLAG_SET_FAIL                                     (USB_ERR_START + 0x0200U)
#define USB_ERR_QUEUE_SEND_FAIL                                         (USB_ERR_START + 0x0210U)
#define USB_ERR_MUTEX_CREATE_FAIL                                       (USB_ERR_START + 0x0220U)
#define USB_ERR_MUTEX_DELETE_FAIL                                       (USB_ERR_START + 0x0221U)
#define USB_ERR_TIMEOUT                                                 (USB_ERR_START + 0x0222U)

#define USB_ERR_XHCI_NO_HALT                                            (USB_ERR_START + 0x0230U)
#define USB_ERR_XHCI_RESET                                              (USB_ERR_START + 0x0231U) //!< XHC is not reset
#define USB_ERR_XHCI_RESET_NO_HALT                                      (USB_ERR_START + 0x0232U) //!< XHC is not reset due to not halted
#define USB_ERR_XHCI_NOT_READY                                          (USB_ERR_START + 0x0233U) //!< XHC is not in ready state
#define USB_ERR_XHCI_CTX_SIZE_MISMATCH                                  (USB_ERR_START + 0x0234U) //!<
#define USB_ERR_XHCI_PAGE_SIZE_MISMATCH                                 (USB_ERR_START + 0x0235U) //!<
#define USB_ERR_XHCI_SCRATCHPAD_MISMATCH                                (USB_ERR_START + 0x0236U) //!<
#define USB_ERR_XHCI_NO_CMD_RING_ALIGN                                  (USB_ERR_START + 0x0236U) //!< XHC command ring address or size is not aligned
#define USB_ERR_XHCI_NO_EVT_RING_ALIGN                                  (USB_ERR_START + 0x0236U) //!< XHC event ring address or size is not aligned
#define USB_ERR_XHCI_NO_ERST_ALIGN                                      (USB_ERR_START + 0x0236U) //!< XHC Event Ring Segment Table address or size is not aligned
#define USB_ERR_XHCI_CONTROLLER_TIMEOUT                                 (USB_ERR_START + 0x0237U) //!<
#define USB_ERR_XHCI_INVALID_TRB_TYPE                                   (USB_ERR_START + 0x0238U) //!<
#define USB_ERR_XHCI_INVALID_TRB_CMPL_CODE                              (USB_ERR_START + 0x0239U) //!<
#define USB_ERR_XHCI_INVALID_TRB_SLOTID                                 (USB_ERR_START + 0x023AU) //!<
#define USB_ERR_XHCI_INVALID_TRB_PTR                                    (USB_ERR_START + 0x023BU) //!<
#define USB_ERR_XHCI_EP_NOT_RUNNING                                     (USB_ERR_START + 0x023CU) //!<
#define USB_ERR_XHCI_NO_CONNECT                                         (USB_ERR_START + 0x023DU) //!<
#define USB_ERR_XHCI_EP_NO_INIT                                         (USB_ERR_START + 0x023EU) //!<
#define USB_ERR_XHCI_EP_NO_CONFIG                                       (USB_ERR_START + 0x023FU) //!<
#define USB_ERR_XHCI_EP0_RUNNING                                        (USB_ERR_START + 0x0240U) //!<
#define USB_ERR_XHCI_SPEED_INVALID                                      (USB_ERR_START + 0x0241U) //!<
#define USB_ERR_XHCI_EP0_STATE_INVALID                                  (USB_ERR_START + 0x0242U) //!<
#define USB_ERR_XHCI_EP_IDX_INVALID                                     (USB_ERR_START + 0x0243U) //!<
#define USB_ERR_XHCI_DEV_DESC_INVALID                                   (USB_ERR_START + 0x0244U) //!<
#define USB_ERR_XHCI_NO_CALLBACK                                        (USB_ERR_START + 0x0245U) //!<
#define USB_ERR_XHCI_NO_TRANSFER_REQUEST                                (USB_ERR_START + 0x0246U) //!<
#define USB_ERR_XHCI_EVT_BABBLE                                         (USB_ERR_START + 0x0247U) //!< XHCI toggle bit mismatch for Event Ring
#define USB_ERR_XHCI_TIMEOUT                                            (USB_ERR_START + 0x0248U) //!<


#define USB_ERR_USBD_NO_CLASS_START_FUNCTION                            (USB_ERR_START + 0x0300U)
#define USB_ERR_USBD_NO_CLASS_STOP_FUNCTION                             (USB_ERR_START + 0x0301U)

#define USB_ERR_MSCD_ALREADY_MOUNTED                                    (USB_ERR_START + 0x0400U)

#define USB_ERR_HOST_START                                              (USB_ERR_START + 0x8400U)
#define USB_ERR_EHCI_NO_HCD_REGISTERED                                  (USB_ERR_HOST_START + 0x01U)
#define USB_ERR_HOST_CLASS_STARTED                                      (USB_ERR_HOST_START + 0x02U)

#define USB_ERR_HOST_STORAGE_START                                      (USB_ERR_START + 0x8700U)
#define USB_ERR_HSTRG_MUTEX_CREATE_FAIL                                 (USB_ERR_HOST_STORAGE_START + 0x00U)
#define USB_ERR_HSTRG_MUTEX_LOCK_FAIL                                   (USB_ERR_HOST_STORAGE_START + 0x01U)
#define USB_ERR_HSTRG_MUTEX_UNLOCK_FAIL                                 (USB_ERR_HOST_STORAGE_START + 0x02U)
#define USB_ERR_HSTRG_MUTEX_DELETE_FAIL                                 (USB_ERR_HOST_STORAGE_START + 0x03U)
#define USB_ERR_HSTRG_SEMAPHORE_LOCK_FAIL                               (USB_ERR_HOST_STORAGE_START + 0x04U)
#define USB_ERR_HSTRG_SEMAPHORE_UNLOCK_FAIL                             (USB_ERR_HOST_STORAGE_START + 0x05U)

#define USB_ERR_HOST_SIMPLE_START                                       (USB_ERR_START + 0x8800U)
#define USB_ERR_HOST_SIMPLE_MUTEX_CREATE_FAIL                           (USB_ERR_HOST_SIMPLE_START + 0x00U)
#define USB_ERR_HOST_SIMPLE_MUTEX_LOCK_FAIL                             (USB_ERR_HOST_SIMPLE_START + 0x01U)
#define USB_ERR_HOST_SIMPLE_MUTEX_UNLOCK_FAIL                           (USB_ERR_HOST_SIMPLE_START + 0x02U)
#define USB_ERR_HOST_SIMPLE_MUTEX_DELETE_FAIL                           (USB_ERR_HOST_SIMPLE_START + 0x03U)
#define USB_ERR_HOST_SIMPLE_SEMAPHORE_CREATE_FAIL                       (USB_ERR_HOST_SIMPLE_START + 0x04U)
#define USB_ERR_HOST_SIMPLE_SEMAPHORE_LOCK_FAIL                         (USB_ERR_HOST_SIMPLE_START + 0x05U)
#define USB_ERR_HOST_SIMPLE_SEMAPHORE_UNLOCK_FAIL                       (USB_ERR_HOST_SIMPLE_START + 0x06U)
#define USB_ERR_HOST_SIMPLE_SEMAPHORE_DELETE_FAIL                       (USB_ERR_HOST_SIMPLE_START + 0x07U)
#define USB_ERR_HOST_SIMPLE_EVENTFLAG_CREATE_FAIL                       (USB_ERR_HOST_SIMPLE_START + 0x08U)
#define USB_ERR_HOST_SIMPLE_EVENTFLAG_SET_FAIL                          (USB_ERR_HOST_SIMPLE_START + 0x09U)
#define USB_ERR_HOST_SIMPLE_EVENTFLAG_WAIT_FAIL                         (USB_ERR_HOST_SIMPLE_START + 0x0AU)
#define USB_ERR_HOST_SIMPLE_EVENTFLAG_DELETE_FAIL                       (USB_ERR_HOST_SIMPLE_START + 0x0BU)
#define USB_ERR_HOST_SIMPLE_TASK_CREATE_FAIL                            (USB_ERR_HOST_SIMPLE_START + 0x0CU)
#define USB_ERR_HOST_SIMPLE_TASK_TERMINATE_FAIL                         (USB_ERR_HOST_SIMPLE_START + 0x0DU)
#define USB_ERR_HOST_SIMPLE_TASK_DELETE_FAIL                            (USB_ERR_HOST_SIMPLE_START + 0x0EU)

#define USBX_ERR_START                                                  USBX_ERR_BASE
#define USBX_ERR_NO_MEMORY                                              (USBX_ERR_START + 0x101U)
#define USBX_ERR_NO_AVAILABLE_CLASS_ARRAY                               (USBX_ERR_START + 0x102U)
#define USBX_ERR_CLASS_ALREADY_INSTALLED                                (USBX_ERR_START + 0x103U)
#define USBX_ERR_MUTEX_ERROR                                            (USBX_ERR_START + 0x104U)
#define USBX_ERR_SEMAPHORE_ERROR                                        (USBX_ERR_START + 0x105U)
#define USBX_ERR_SYSTEM_INIT_FAIL                                       (USBX_ERR_START + 0x106U)
#define USBX_ERR_SYSTEM_DEINIT_FAIL                                     (USBX_ERR_START + 0x107U)
#define USBX_ERR_THREAD_ERROR                                           (USBX_ERR_START + 0x108U)
#define USBX_ERR_EVENT_ERROR                                            (USBX_ERR_START + 0x109U)

#define USBX_ERR_DEVICE_START                                           (USBX_ERR_START + 0x4000U)
#define USBX_ERR_NOT_CONFIGURED                                         (USBX_ERR_DEVICE_START + 0x001U)
#define USBX_ERR_MTP_NO_EVENT_ARRAY                                     (USBX_ERR_DEVICE_START + 0x002U)
#define USBX_ERR_MTP_EVENT_ARRAY_FULL                                   (USBX_ERR_DEVICE_START + 0x003U)
#define USBX_ERR_MTP_UNSUPPORTED_COMMAND                                (USBX_ERR_DEVICE_START + 0x004U)
#define USBX_ERR_CLASS_INTERFACE_DELETE_FAIL                            (USBX_ERR_DEVICE_START + 0x005U)
#define USBX_ERR_CTRL_ENDPOINT_DELETE_FAIL                              (USBX_ERR_DEVICE_START + 0x006U)
#define USBX_ERR_SEMAPHORE_UNLOCK_FAIL                                  (USBX_ERR_DEVICE_START + 0x007U)
#define USBX_ERR_CLASS_INIT_FAIL                                        (USBX_ERR_DEVICE_START + 0x008U)
#define USBX_ERR_CLASS_DEACTIVATE_FAIL                                  (USBX_ERR_DEVICE_START + 0x009U)
#define USBX_ERR_CLASS_DEINIT_FAIL                                      (USBX_ERR_DEVICE_START + 0x00AU)
#define USBX_ERR_DESCRIPTOR_CORRUPTED                                   (USBX_ERR_DEVICE_START + 0x00BU)
#define USBX_ERR_CLASS_REGISTER_FAIL                                    (USBX_ERR_DEVICE_START + 0x00CU)

#define USBX_ERR_ENDPOINT_POOL_ALLOCATE_FAIL                            (USBX_ERR_DEVICE_START + 0x18AU)
#define USBX_ERR_ENDPOINT_POOL_RELEASE_FAIL                             (USBX_ERR_DEVICE_START + 0x18BU)
#define USBX_ERR_INTERFACE_POOL_ALLOCATE_FAIL                           (USBX_ERR_DEVICE_START + 0x18CU)
#define USBX_ERR_INTERFACE_POOL_RELEASE_FAIL                            (USBX_ERR_DEVICE_START + 0x18DU)
#define USBX_ERR_CLASS_ARRAY_ALLOCATE_FAIL                              (USBX_ERR_DEVICE_START + 0x18EU)
#define USBX_ERR_CLASS_ARRAY_RELEASE_FAIL                               (USBX_ERR_DEVICE_START + 0x190U)

#define USBX_ERR_HOST_START                                             (USBX_ERR_START + 0x8000U)

#define USBX_ERR_HSTACK_INIT_NO_MEMORY                                  (USBX_ERR_HOST_START + 0x001U)
#define USBX_ERR_HSTACK_INIT_SEMAPHORE_ERROR                            (USBX_ERR_HOST_START + 0x002U)
#define USBX_ERR_HSTACK_INIT_THREAD_ERROR                               (USBX_ERR_HOST_START + 0x003U)
#define USBX_ERR_HSTACK_INIT_FAIL                                       (USBX_ERR_HOST_START + 0x00FU)

#define USBX_ERR_HSTACK_REG_ALREADY_INSTALLED                           (USBX_ERR_HOST_START + 0x011U)
#define USBX_ERR_HSTACK_REG_ARRAY_FULL                                  (USBX_ERR_HOST_START + 0x012U)
#define USBX_ERR_HSTACK_REG_FAIL                                        (USBX_ERR_HOST_START + 0x01FU)

#define USBX_ERR_HSTACK_DEINIT_SEMAPHORE_ERROR                          (USBX_ERR_HOST_START + 0x021U)
#define USBX_ERR_HSTACK_DEINIT_THREAD_ERROR                             (USBX_ERR_HOST_START + 0x022U)
#define USBX_ERR_HSTACK_DEINIT_FAIL                                     (USBX_ERR_HOST_START + 0x02FU)

#define USBX_ERR_HOST_CLASS_DEACTIVATE_FAIL                             (USBX_ERR_HOST_START + 0x006U)
#define USBX_ERR_HOST_REMOVE_DEVICE_RESOURCE_FAIL                       (USBX_ERR_HOST_START + 0x007U)

#define USBX_ERR_HOST_EHCI_START                                        (USBX_ERR_HOST_START + 0x8800U)
#define USBX_ERR_EHCI_INIT_NO_MEMORY                                    (USBX_ERR_HOST_EHCI_START + 0x01U)
#define USBX_ERR_EHCI_INIT_NO_AVAILABLE_ED                              (USBX_ERR_HOST_EHCI_START + 0x02U)
#define USBX_ERR_EHCI_INIT_FAIL                                         (USBX_ERR_HOST_EHCI_START + 0x0FU)

#define USBX_ERR_EHCI_STOP_NO_HCD_REGISTERED                            (USBX_ERR_HOST_EHCI_START + 0x11U)
#define USBX_ERR_EHCI_STOP_SEMAPHORE_ERROR                              (USBX_ERR_HOST_EHCI_START + 0x12U)
#define USBX_ERR_EHCI_STOP_FAIL                                         (USBX_ERR_HOST_EHCI_START + 0x1FU)

#define USBX_ERR_EHCI_DEINIT_NO_HCD_REGISTERED                          (USBX_ERR_HOST_EHCI_START + 0x21U)
#define USBX_ERR_EHCI_DEINIT_SEMAPHORE_ERROR                            (USBX_ERR_HOST_EHCI_START + 0x22U)
#define USBX_ERR_EHCI_DEINIT_FAIL                                       (USBX_ERR_HOST_EHCI_START + 0x2FU)

#define USBX_ERR_HOST_OHCI_START                                        (USBX_ERR_HOST_START + 0x8900U)
#define USBX_ERR_OHCI_INIT_NO_MEMORY                                    (USBX_ERR_HOST_OHCI_START + 0x01U)
#define USBX_ERR_OHCI_INIT_NO_AVAILABLE_ED                              (USBX_ERR_HOST_OHCI_START + 0x02U)
#define USBX_ERR_OHCI_INIT_CONTROLLER_FAIL                              (USBX_ERR_HOST_OHCI_START + 0x03U)
#define USBX_ERR_OHCI_INIT_FAIL                                         (USBX_ERR_HOST_OHCI_START + 0xFFU)

#define USBX_ERR_OHCI_STOP_NO_HCD_REGISTERED                            (USBX_ERR_HOST_OHCI_START + 0x11U)
#define USBX_ERR_OHCI_STOP_FAIL                                         (USBX_ERR_HOST_OHCI_START + 0x1FU)

#define USBX_ERR_OHCI_DEINIT_NO_HCD_REGISTERED                          (USBX_ERR_HOST_OHCI_START + 0x21U)
#define USBX_ERR_OHCI_DEINIT_FAIL                                       (USBX_ERR_HOST_OHCI_START + 0x2FU)

#define USBX_ERR_HOST_XHCI_START                                        (USBX_ERR_HOST_START + 0x8A00U)
#define USBX_ERR_XHCI_STOP_NO_HCD_REGISTERED                            (USBX_ERR_HOST_XHCI_START + 0x01U)
#define USBX_ERR_XHCI_STOP_FAIL                                         (USBX_ERR_HOST_XHCI_START + 0x02U)
#define USBX_ERR_XHCI_DEINIT_NO_HCD_REGISTERED                          (USBX_ERR_HOST_XHCI_START + 0x03U)
#define USBX_ERR_XHCI_DEINIT_SEMAPHORE_ERROR                            (USBX_ERR_HOST_XHCI_START + 0x04U)
#define USBX_ERR_XHCI_DEINIT_FAIL                                       (USBX_ERR_HOST_XHCI_START + 0x05U)

#define USBX_ERR_HOST_STORAGE_START                                     (USBX_ERR_START + 0xA000U)
#define USBX_ERR_HSTRG_SENSE_ERROR                                      (USBX_ERR_HOST_STORAGE_START + 0x001U)
#define USBX_ERR_HSTRG_TRANSFER_TIMEOUT                                 (USBX_ERR_HOST_STORAGE_START + 0x002U)
#define USBX_ERR_HSTRG_TRANSFER_FAIL                                    (USBX_ERR_HOST_STORAGE_START + 0x00FU)

#define USBX_ERR_FAIL                                                   (USBX_ERR_START + 0xFFFFU)


#define USB_ERR_0100       USB_ERR_PARAMETER_INVALID
#define USB_ERR_0101       USB_ERR_PARAMETER_NULL
#define USB_ERR_0102       USB_ERR_COREMASK_ZERO
#define USB_ERR_0103       USB_ERR_NO_INIT
#define USB_ERR_0104       USB_ERR_PARAMETER_ZERO
#define USB_ERR_0109       USB_ERR_INVALID_ENDPOINT_ADDRESS
#define USB_ERR_010A       USB_ERR_NO_INSTANCE
#define USB_ERR_010B       USB_ERR_REACH_MAX_NUMBER
#define USB_ERR_010C       USB_ERR_MEDIA_NOT_READY
#define USB_ERR_010D       USB_ERR_MUTEX_LOCK_FAIL
#define USB_ERR_010E       USB_ERR_MUTEX_UNLOCK_FAIL
#define USB_ERR_010F       USB_ERR_MUTEX_LOCK_TIMEOUT
#define USB_ERR_0112       USB_ERR_NO_PHY_ENDPOINT_AVAILABLE
#define USB_ERR_0113       USB_ERR_VBUS_TASK_CREATE_FAIL
#define USB_ERR_0114       USB_ERR_VBUS_SEMA_CREATE_FAIL
#define USB_ERR_0117       USB_ERR_VBUS_TASK_SUSPEND_FAIL
#define USB_ERR_011A       USB_ERR_VBUS_MUTEX_CREATE_FAIL
#define USB_ERR_011B       USB_ERR_VBUS_MUTEX_LOCK_FAIL
#define USB_ERR_011C       USB_ERR_VBUS_MUTEX_UNLOCK_FAIL
#define USB_ERR_011E       USB_ERR_ISR_TASK_CREATE_FAIL
#define USB_ERR_0121       USB_ERR_ISR_QUEUE_CREATE_FAIL
#define USB_ERR_0124       USB_ERR_CTRL_TASK_CREATE_FAIL
#define USB_ERR_0126       USB_ERR_CTRL_EVENTFLAG_CREATE_FAIL
#define USB_ERR_0129       USB_ERR_RX_MUTEX_CREATE_FAIL
#define USB_ERR_012D       USB_ERR_RX_STATE_MUTEX_CREATE_FAIL
#define USB_ERR_0131       USB_ERR_HSTRG_MUTEX_CREATE_FAIL
#define USB_ERR_0132       USB_ERR_HSTRG_MUTEX_LOCK_FAIL
#define USB_ERR_0133       USB_ERR_HSTRG_MUTEX_UNLOCK_FAIL
#define USB_ERR_0134       USB_ERR_HSTRG_MUTEX_DELETE_FAIL
#define USB_ERR_0143       USB_ERR_TRANSFER_TIMEOUT
#define USB_ERR_0144       USB_ERR_TRANSFER_BUS_RESET
#define USB_ERR_0145       USB_ERR_INVALID_CLASS_ID
#define USB_ERR_0147       USB_ERR_DEVICE_BNA
#define USB_ERR_014A       USB_ERR_BUF_NOT_CACHE_ALIGNED
#define USB_ERR_014E       USB_ERR_HOST_IS_RUNNING
#define USB_ERR_014F       USB_ERR_DEVICE_IS_RUNNING
#define USB_ERR_0150       USB_ERR_ISR_TASK_SUSPEND_FAIL
#define USB_ERR_0151       USB_ERR_CTRL_TASK_SUSPEND_FAIL
#define USB_ERR_0164       USB_ERR_VBUS_TASK_NOT_RUNNING
#define USB_ERR_0165       USB_ERR_ISR_TASK_NOT_RUNNING
#define USB_ERR_0166       USB_ERR_CTRL_TASK_NOT_RUNNING
#define USB_ERR_0180       USB_ERR_RTSL_MUTEX_CREATE_FAIL

#define USB_ERR_0200       USB_ERR_EVENT_FLAG_SET_FAIL
#define USB_ERR_0210       USB_ERR_QUEUE_SEND_FAIL
#define USB_ERR_0220       USB_ERR_MUTEX_CREATE_FAIL
#define USB_ERR_0221       USB_ERR_MUTEX_DELETE_FAIL

#define USB_ERR_0300       USB_ERR_USBD_NO_CLASS_START_FUNCTION
#define USB_ERR_0301       USB_ERR_USBD_NO_CLASS_STOP_FUNCTION

#define USB_ERR_0400       USB_ERR_MSCD_ALREADY_MOUNTED

#define USB_ERR_8401       USB_ERR_EHCI_NO_HCD_REGISTERED
#define USB_ERR_8402       USB_ERR_HOST_CLASS_STARTED

#define USB_ERR_8701       USB_ERR_HSTRG_MUTEX_LOCK_FAIL
#define USB_ERR_8702       USB_ERR_HSTRG_MUTEX_UNLOCK_FAIL
#define USB_ERR_8704       USB_ERR_HSTRG_SEMAPHORE_LOCK_FAIL
#define USB_ERR_8705       USB_ERR_HSTRG_SEMAPHORE_UNLOCK_FAIL

// usbx system errors
#define USBX_ERR_0101      USBX_ERR_NO_MEMORY
#define USBX_ERR_0102      USBX_ERR_NO_AVAILABLE_CLASS_ARRAY
#define USBX_ERR_0103      USBX_ERR_CLASS_ALREADY_INSTALLED
#define USBX_ERR_0104      USBX_ERR_MUTEX_ERROR
#define USBX_ERR_0105      USBX_ERR_SEMAPHORE_ERROR
#define USBX_ERR_0106      USBX_ERR_SYSTEM_INIT_FAIL
#define USBX_ERR_0107      USBX_ERR_SYSTEM_DEINIT_FAIL
#define USBX_ERR_0108      USBX_ERR_THREAD_ERROR
#define USBX_ERR_0109      USBX_ERR_EVENT_ERROR

// Device stack errors
#define USBX_ERR_1003      USBX_ERR_MTP_EVENT_ARRAY_FULL
#define USBX_ERR_1005      USBX_ERR_CLASS_INTERFACE_DELETE_FAIL
#define USBX_ERR_1006      USBX_ERR_CTRL_ENDPOINT_DELETE_FAIL

#define USBX_ERR_1008      USBX_ERR_CLASS_INIT_FAIL
#define USBX_ERR_1009      USBX_ERR_CLASS_DEACTIVATE_FAIL
#define USBX_ERR_100A      USBX_ERR_CLASS_DEINIT_FAIL
#define USBX_ERR_100B      USBX_ERR_DESCRIPTOR_CORRUPTED
#define USBX_ERR_100C      USBX_ERR_CLASS_REGISTER_FAIL

// Host stack errors
#define USBX_ERR_8001      USBX_ERR_HSTACK_INIT_NO_MEMORY
#define USBX_ERR_8002      USBX_ERR_HSTACK_INIT_SEMAPHORE_ERROR
#define USBX_ERR_8003      USBX_ERR_HSTACK_INIT_THREAD_ERROR
#define USBX_ERR_800F      USBX_ERR_HSTACK_INIT_FAIL

#define USBX_ERR_8011      USBX_ERR_HSTACK_REG_ALREADY_INSTALLED
#define USBX_ERR_8012      USBX_ERR_HSTACK_REG_ARRAY_FULL
#define USBX_ERR_801F      USBX_ERR_HSTACK_REG_FAIL
#define USBX_ERR_8021      USBX_ERR_HSTACK_DEINIT_SEMAPHORE_ERROR
#define USBX_ERR_8022      USBX_ERR_HSTACK_DEINIT_THREAD_ERROR
#define USBX_ERR_802F      USBX_ERR_HSTACK_DEINIT_FAIL

// Host EHCI errors
#define USBX_ERR_8801      USBX_ERR_EHCI_INIT_NO_MEMORY
#define USBX_ERR_8802      USBX_ERR_EHCI_INIT_NO_AVAILABLE_ED
#define USBX_ERR_880F      USBX_ERR_EHCI_INIT_FAIL
#define USBX_ERR_8811      USBX_ERR_EHCI_STOP_NO_HCD_REGISTERED
#define USBX_ERR_8812      USBX_ERR_EHCI_DEINIT_SEMAPHORE_ERROR
#define USBX_ERR_881F      USBX_ERR_EHCI_STOP_FAIL
#define USBX_ERR_8821      USBX_ERR_EHCI_DEINIT_NO_HCD_REGISTERED
#define USBX_ERR_8822      USBX_ERR_EHCI_DEINIT_SEMAPHORE_ERROR
#define USBX_ERR_882F      USBX_ERR_EHCI_DEINIT_FAIL

// Host OHCI errors
#define USBX_ERR_8901      USBX_ERR_OHCI_INIT_NO_MEMORY
#define USBX_ERR_8902      USBX_ERR_OHCI_INIT_NO_AVAILABLE_ED
#define USBX_ERR_8903      USBX_ERR_OHCI_INIT_CONTROLLER_FAIL
#define USBX_ERR_890F      USBX_ERR_OHCI_INIT_FAIL
#define USBX_ERR_8911      USBX_ERR_OHCI_STOP_NO_HCD_REGISTERED
#define USBX_ERR_891F      USBX_ERR_OHCI_STOP_FAIL
#define USBX_ERR_8921      USBX_ERR_OHCI_DEINIT_NO_HCD_REGISTERED
#define USBX_ERR_892F      USBX_ERR_OHCI_DEINIT_FAIL

// Host Storage errors
#define USBX_ERR_A001      USBX_ERR_HSTRG_SENSE_ERROR
#define USBX_ERR_A002      USBX_ERR_HSTRG_TRANSFER_TIMEOUT
#define USBX_ERR_A00F      USBX_ERR_HSTRG_TRANSFER_FAIL

#define USB_ERR_FFFF       USB_ERR_FAIL
#define USBX_ERR_FFFF      USBX_ERR_FAIL

#endif
