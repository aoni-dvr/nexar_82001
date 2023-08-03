/**
 *  @file AmbaUSB_Utility.h
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
 *  @details USB utility functions header file.
 */

#ifndef AMBAUSB_UTILITY_H
#define AMBAUSB_UTILITY_H

#ifndef AMBA_FS_H
#include <AmbaFS.h>
#endif
#ifndef AMBA_FS_FORMAT_H
#include <AmbaFS_Format.h>
#endif
#ifndef UX_HOST_CLASS_STORAGE_H
#include <usbx/usbx_host_classes/ux_host_class_storage.h>
#endif
#include <AmbaIOUtility.h>

#define USB_NO_WAIT      ((UINT32)0UL)
#define USB_WAIT_FOREVER ((UINT32)0xFFFFFFFFUL)

#define USB_KAL_DO_NOT_START (0U)
#define USB_KAL_AUTO_START   (1U)

#define USB_KAL_AND                    TX_AND
#define USB_KAL_AND_CLEAR              TX_AND_CLEAR
#define USB_KAL_OR                     TX_OR
#define USB_KAL_OR_CLEAR               TX_OR_CLEAR

#define USB_PRINT_MODULE_ID        ((UINT16)(USB_ERR_BASE >> 16U))     /**< Module ID for AmbaPrint */

#define USB_PRINT_FLAG_ERROR           (0x00000000U)
#define USB_PRINT_FLAG_INFO            (0x00000001U)
#define USB_PRINT_FLAG_L1              (0x00000003U)
#define USB_PRINT_FLAG_L2              (0x00000007U)
#define USB_PRINT_FLAG_L3              (0x0000000FU)
#define USB_PRINT_FLAG_L4              (0x0000001FU)

#if defined(CONFIG_ENABLE_USB_DRV_DBG_MSG)
// Enable it for printing debug message
#define  USB_PRINT_SUPPORT
#endif

typedef struct {
    UINT64 BaseAddress;
    UINT32 IntPin;
    UINT32 IntType;
} USB_MODULE_SETTING_s;

typedef struct {
    USB_MODULE_SETTING_s Udc;   // UDC20 controller
    USB_MODULE_SETTING_s Ohci;  // OHCI controller
    USB_MODULE_SETTING_s Ehci;  // EHCI controller
    USB_MODULE_SETTING_s Udc32; // UDC32 controller
    USB_MODULE_SETTING_s Xhci;  // XHCI controller
    USB_MODULE_SETTING_s Phy32; // USB32 PHY for UDC32 and XHCI
    USB_MODULE_SETTING_s Rct;
    UINT32 HostPinOverCurrent;
    UINT32 HostPinPowerEnable;
} USB_HARDWARE_SETTING_s;

UINT32  USB_UtilityVoid2UInt32(const void* Ptr);
UINT32 *USB_UtilityVoidP2UInt32P(const void* VoidPtr);
UINT8  *USB_UtilityConstP8ToP8(const UINT8* Ptr);
UX_SLAVE_TRANSFER* USB_UtilityVoidP2UsbxSlaveXfer(const void* VoidPtr);
UX_SLAVE_ENDPOINT* USB_UtilityVoidP2UsbxEp(const void* VoidPtr);
UINT32 USB_UtilityU8PtrToU32Addr(const UINT8 *Ptr);
UINT8  *USB_UtilityU32AddrToU8Ptr(UINT32 Addr);
UINT32 *USB_UtilityU32AddrToU32Ptr(UINT32 Addr);
UINT8  *USB_UtilityVirP8ToPhyP8(UINT8* VirPtr);
UINT32 USB_UtilityVirP8ToPhyI32(const UINT8* VirPtr);
UINT8 *USB_UtilityPhyP8ToVirP8(UINT8* PhyPtr);
void  *USB_UtilityVirVp2PhyVp(const void* VirPtr);
void  *USB_UtilityPhyVp2VirVp(const void* PhyPtr);
UDC_ENDPOINT_INFO_s* USB_UtilityVoidP2UdcEd(const void* VoidPtr);
void* USB_UtilityUdcEd2VoidP(const UDC_ENDPOINT_INFO_s* Ptr);
UINT32 USB_UtilityVirDataDesc2PhyI32(const UDC_DATA_DESC_s* VirPtr);
UINT32 USB_UtilityVirSetupDesc2PhyI32(const UDC_SETUP_DESC_s* VirPtr);
UDC_SETUP_DESC_s* USB_UtilityPhyI32ToVirSetupDesc(UINT32 PhyAddr);
UDC_DATA_DESC_s* USB_UtilityPhyI32ToVirDataDesc(UINT32 PhyAddr);
UINT8 *USB_UtilityPhyI32ToVirU8P(UINT32 PhyAddr);
UX_HOST_CLASS_STORAGE* USB_UtilityVoidP2UxHStrg(const void* VoidPtr);
UX_HOST_CLASS_STORAGE_MEDIA* USB_UtilityVoidP2UxHStrgMedia(const void* VoidPtr);
PF_DRV_TBL* USB_UtilityVoidP2PrFDrvTbl(const void* VoidPtr);
AMBA_FS_FAT_MBR* USB_UtilityP8ToFsMbr(const UINT8* Ptr);
AMBA_FS_FAT_BS32* USB_UtilityP8ToFsBs32(const UINT8* Ptr);
AMBA_FS_FAT_BS16* USB_UtilityP8ToFsBs16(const UINT8* Ptr);
AMBA_FS_EXFAT_BS* USB_UtilityP8ToFsFxfatBs(const UINT8* Ptr);
void* USB_UtilityP8ToVp(const UINT8* Ptr);
UINT8* USB_UtilityVp2P8(const void* Ptr);
void USB_UtilityCacheFlushSetupDesc(const UDC_SETUP_DESC_s *Ptr, const UINT32 Range);
void USB_UtilityCacheInvdSetupDesc(const UDC_SETUP_DESC_s *Ptr, const UINT32 Range);
void USB_UtilityCacheFlushDataDesc(const UDC_DATA_DESC_s *Ptr, const UINT32 Range);
void USB_UtilityCacheInvdDataDesc(const UDC_DATA_DESC_s *Ptr, const UINT32 Range);
void USB_UtilityCacheFlushUInt8(const UINT8 *Ptr, const UINT32 Range);
void USB_UtilityCacheInvdUInt8(const UINT8 *Ptr, const UINT32 Range);
UINT32 USB_UtilityQueueCreate(AMBA_KAL_MSG_QUEUE_t *pMsgQueue, UINT8 *pMsgQueueBase, UINT32 MsgSize, UINT32 MaxNumMsg);
UINT32 USB_UtilityQueueDelete(AMBA_KAL_MSG_QUEUE_t *pMsgQueue);
UINT32 USB_UtilityQueueIsrSend(AMBA_KAL_MSG_QUEUE_t *pMsgQueue, const UDC_ISR_REQUEST_s* Request, UINT32 Timeout);
UINT32 USB_UtilityQueueIsrRecv(AMBA_KAL_MSG_QUEUE_t *pMsgQueue, const UDC_ISR_REQUEST_s* Request, UINT32 Timeout);
UINT32 USB_UtilityQueueIsoSend(AMBA_KAL_MSG_QUEUE_t *pMsgQueue, const UDC_ISO_REQUEST_s* Request, UINT32 Timeout);
UINT32 USB_UtilityQueueIsoRecv(AMBA_KAL_MSG_QUEUE_t *pMsgQueue, const UDC_ISO_REQUEST_s* Request, UINT32 Timeout);
UINT32 USB_UtilityEventCreate(AMBA_KAL_EVENT_FLAG_t *pEventFlag);
UINT32 USB_UtilityEventDelete(AMBA_KAL_EVENT_FLAG_t *pEventFlag);
UINT32 USB_UtilityEventSend(AMBA_KAL_EVENT_FLAG_t *pEventFlag, UINT32 Flags);
UINT32 USB_UtilityEventRecv(AMBA_KAL_EVENT_FLAG_t *pEventFlag, UINT32 ReqFlags, UINT32 Option,
                            UINT32 *pActualFlags, UINT32 Timeout);
UINT32 USB_UtilityMutexCreate(AMBA_KAL_MUTEX_t *Mutex);
UINT32 USB_UtilityMutexDelete(AMBA_KAL_MUTEX_t *Mutex);
UINT32 USB_UtilityMutexTake(AMBA_KAL_MUTEX_t *Mutex, UINT32 Timeout);
UINT32 USB_UtilityMutexGive(AMBA_KAL_MUTEX_t *Mutex);
UINT32 USB_UtilitySemaphoreCreate(AMBA_KAL_SEMAPHORE_t *Sem, UINT32 InitCnt);
UINT32 USB_UtilitySemaphoreDelete(AMBA_KAL_SEMAPHORE_t *Sem);
UINT32 USB_UtilitySemaphoreTake(AMBA_KAL_SEMAPHORE_t *Sem, UINT32 Timeout);
UINT32 USB_UtilitySemaphoreGive(AMBA_KAL_SEMAPHORE_t *Sem);
UINT32 USB_UtilitySemaphoreQuery(AMBA_KAL_SEMAPHORE_t *Sem, UINT32 *CurCnt);
void   USB_UtilityCoreMaskSet(UINT32 CoreMask);
void    USB_UtilityTaskSleep(UINT32 Ms);
UINT32 USB_UtilityTaskCreate(AMBA_KAL_TASK_t *pTask,
                             char *           pTaskName,
                             UINT32           Priority,
                             void *(*         EntryFunction)(void *Arg),
                             void *           EntryArg,
                             void *           pStackBase,
                             UINT32           StackByteSize,
                             UINT32           AutoStart,
                             UINT32           AffinityMask);
UINT32 USB_UtilityTaskDelete(AMBA_KAL_TASK_t *pTask, UINT32 Suspended);
UINT32 USB_UtilityTaskSuspendSelf(void);

void  USB_UtilityPrint(UINT32 ControlFlags, const char *Str);
void  USB_UtilityPrintUInt1(UINT32 ControlFlags, const char *FmtStr, UINT32 Arg1);
void  USB_UtilityPrintUInt5(UINT32 ControlFlags, const char *FmtStr, UINT32 Arg1, UINT32 Arg2, UINT32 Arg3, UINT32 Arg4, UINT32 Arg5);
void  USB_UtilityPrintStr5(UINT32 ControlFlags, const char *FmtStr, const char *Arg1, const char *Arg2, const char *Arg3, const char *Arg4, const char *Arg5);
void  USB_UtilityPrintUInt64_5(UINT32 ControlFlags, const char *FmtStr, UINT64 Arg1, UINT64 Arg2, UINT64 Arg3, UINT64 Arg4, UINT64 Arg5);
void  USB_UtilityPrintUInt64_1(UINT32 ControlFlags, const char *FmtStr, UINT64 Arg1);

UINT32 USB_UtilityInt32ToStr(char *pBuffer, UINT32 BufferSize, INT32 Value, UINT32 Radix);
UINT32 USB_UtilityUInt32ToStr(char *pBuffer, UINT32 BufferSize, UINT32 Value, UINT32 Radix);
void   USB_UtilityUInt32Put(UINT8 *Addr, UINT32 Value);

void   USB_UtilityMemcpyUInt8(UINT8 *Dest, const UINT8 *Src, UINT32 Size);
UINT32 USB_UtilityUXCode2AmbaCode(UINT32 Code);
void   USB_UtilityMemoryCopy(void *pDst, const void *pSrc, SIZE_t num);
void   USB_UtilityMemorySet(void *ptr, INT32 v, SIZE_t n);

UINT32 USB_UtilityWaitForReg(UINT64 RegAddr, UINT32 Mask, UINT32 WaitFor, UINT32 Timeout);

const USB_HARDWARE_SETTING_s *USB_UtilityHWInfoQuery(void);
void   USB_UtilityRTSLInit(void);

UINT32 USB_UtilityPrintLevelSupport(UINT32 ControlFlags);

void   USB_UtilityAssert(void);

#endif

