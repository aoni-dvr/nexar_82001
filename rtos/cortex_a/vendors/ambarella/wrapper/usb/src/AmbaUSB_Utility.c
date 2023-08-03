/**
 *  @file AmbaUSB_Utility.c
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
 *  @details USB utility functions.
 */

#include <AmbaTypes.h>
#define AMBA_KAL_SOURCE_CODE
#include <AmbaKAL.h>
#include <AmbaUSB_Generic.h>
#include <AmbaCache.h>
#include <AmbaUSB_ErrCode.h>
#include <AmbaUSBD_API.h>
#include <AmbaUSBH_API.h>
#include <AmbaUSB_System.h>
#include <AmbaUSB_Utility.h>
#include <AmbaWrap.h>

#if defined(CONFIG_DEVICE_TREE_SUPPORT)
#ifndef AMBAIO_UTILITY_H
#include <AmbaIOUtility.h>
#endif
#endif

static UINT32 usb_core_mask = AMBA_KAL_CPU_CORE_MASK;

/** Wrapper function for C memory copy. */
void USB_UtilityMemoryCopy(void *pDst, const void *pSrc, SIZE_t num)
{
    if (AmbaWrap_memcpy(pDst, pSrc, num) != 0U) {
        // action TDB
    }
}

/** Wrapper function for C memory set. */
void USB_UtilityMemorySet(void *ptr, INT32 v, SIZE_t n)
{
    if (AmbaWrap_memset(ptr, v, n) != 0U) {
        // action TDB
    }
}

void USB_UtilityCoreMaskSet(UINT32 CoreMask)
{
    usb_core_mask = CoreMask;
}

void USB_UtilityTaskSleep(UINT32 Ms)
{
    if (AmbaKAL_TaskSleep(Ms) == 0U) {
        // action TDB
    }
}

UINT32 USB_UtilityVoid2UInt32(const void* Ptr)
{
    //const void *tmp_ptr = Ptr;
    UINT32 addr_ret;

    //USB_UtilityMemoryCopy(&addr_ret, &tmp_ptr, sizeof(void*));
    addr_ret = IO_UtilityPtrToU32Addr(Ptr);

    return addr_ret;
}

UINT8* USB_UtilityConstP8ToP8(const UINT8* Ptr)
{
    const UINT8 *src_ptr = Ptr;
    UINT8 *ptr_ret;

    USB_UtilityMemoryCopy(&ptr_ret, &src_ptr, sizeof(UINT8*));

    return ptr_ret;
}

UINT32* USB_UtilityVoidP2UInt32P(const void* VoidPtr)
{
    const void *src_ptr = VoidPtr;
    UINT32 *ptr_ret;

    USB_UtilityMemoryCopy(&ptr_ret, &src_ptr, sizeof(void*));

    return ptr_ret;
}

UX_SLAVE_TRANSFER* USB_UtilityVoidP2UsbxSlaveXfer(const void* VoidPtr)
{
    const void *src_ptr = VoidPtr;
    UX_SLAVE_TRANSFER *ptr_ret;

    USB_UtilityMemoryCopy(&ptr_ret, &src_ptr, sizeof(void*));

    return ptr_ret;
}

UX_SLAVE_ENDPOINT* USB_UtilityVoidP2UsbxEp(const void* VoidPtr)
{
    const void *src_ptr = VoidPtr;
    UX_SLAVE_ENDPOINT *ptr_ret;

    USB_UtilityMemoryCopy(&ptr_ret, &src_ptr, sizeof(void*));

    return ptr_ret;
}

UINT8* USB_UtilityVirP8ToPhyP8(UINT8* VirPtr)
{
    UINT8 *ptr_ret = VirPtr;
    const void  *tmp_virt_ptr, *tmp_phy_ptr;

    // convert integer pointer to void pointer
    USB_UtilityMemoryCopy(&tmp_virt_ptr, &ptr_ret, sizeof(UINT8 *));

    tmp_phy_ptr = AmbaRTSL_USBVirt2Phys(tmp_virt_ptr);

    // convert void pointer to integer pointer
    USB_UtilityMemoryCopy(&ptr_ret, &tmp_phy_ptr, sizeof(UINT8 *));

    return ptr_ret;
}

UINT32 USB_UtilityVirP8ToPhyI32(const UINT8* VirPtr)
{
    UINT32 addr_ret;
    const void  *tmp_virt_ptr, *tmp_phy_ptr;

#ifdef CONFIG_ARM64
    UINT64 addr64;
    // (UINT8 *) to (void *)
    //tmp_virt_ptr = (void *)VirPtr;
    USB_UtilityMemoryCopy(&tmp_virt_ptr, &VirPtr, sizeof(void *));
    tmp_phy_ptr = AmbaRTSL_USBVirt2Phys(tmp_virt_ptr);
    // (void *) to (UINT64)
    //addr_ret = (UINT32)(UINT64)tmp_phy_ptr;
    USB_UtilityMemoryCopy(&addr64, &tmp_phy_ptr, sizeof(void *));
    addr_ret = (UINT32)addr64;
#else
    // (UINT8 *) to (void *)
    //tmp_virt_ptr = (void *)VirPtr;
    USB_UtilityMemoryCopy(&tmp_virt_ptr, &VirPtr, sizeof(void *));
    tmp_phy_ptr = AmbaRTSL_USBVirt2Phys(tmp_virt_ptr);
    // (void *) to (UINT32)
    //addr_ret = (UINT32)tmp_phy_ptr;
    USB_UtilityMemoryCopy(&addr_ret, &tmp_phy_ptr, sizeof(void *));
#endif
    return addr_ret;
}


UINT8* USB_UtilityPhyP8ToVirP8(UINT8* PhyPtr)
{
    UINT8 *ptr_ret = PhyPtr;
    const void  *tmp_virt_ptr, *tmp_phy_ptr;

    // convert integer pointer to void pointer
    USB_UtilityMemoryCopy(&tmp_phy_ptr, &ptr_ret, sizeof(UINT8*));

    tmp_virt_ptr = AmbaRTSL_USBPhys2Virt(tmp_phy_ptr);

    // convert void pointer to integer pointer
    USB_UtilityMemoryCopy(&ptr_ret, &tmp_virt_ptr, sizeof(UINT8*));

    return ptr_ret;
}

void* USB_UtilityVirVp2PhyVp(const void* VirPtr)
{
    void *ptr_ret;

    ptr_ret = AmbaRTSL_USBVirt2Phys(VirPtr);

    return ptr_ret;
}

void* USB_UtilityPhyVp2VirVp(const void* PhyPtr)
{
    void *ptr_ret;

    ptr_ret = AmbaRTSL_USBPhys2Virt(PhyPtr);

    return ptr_ret;
}

UDC_ENDPOINT_INFO_s* USB_UtilityVoidP2UdcEd(const void* VoidPtr)
{
    const void *src_ptr = VoidPtr;
    UDC_ENDPOINT_INFO_s *ptr_ret;

    USB_UtilityMemoryCopy(&ptr_ret, &src_ptr, sizeof(void*));

    return ptr_ret;
}

void* USB_UtilityUdcEd2VoidP(const UDC_ENDPOINT_INFO_s* Ptr)
{
    const UDC_ENDPOINT_INFO_s *src_ptr = Ptr;
    void *ptr_ret;

    USB_UtilityMemoryCopy(&ptr_ret, &src_ptr, sizeof(UDC_ENDPOINT_INFO_s*));

    return ptr_ret;
}

AMBA_FS_FAT_MBR* USB_UtilityP8ToFsMbr(const UINT8* Ptr)
{
    const UINT8 *src_ptr = Ptr;
    AMBA_FS_FAT_MBR *ptr_ret;

    USB_UtilityMemoryCopy(&ptr_ret, &src_ptr, sizeof(UINT8*));

    return ptr_ret;
}

AMBA_FS_FAT_BS32* USB_UtilityP8ToFsBs32(const UINT8* Ptr)
{
    const UINT8 *src_ptr = Ptr;
    AMBA_FS_FAT_BS32 *ptr_ret;

    USB_UtilityMemoryCopy(&ptr_ret, &src_ptr, sizeof(UINT8*));

    return ptr_ret;
}

AMBA_FS_FAT_BS16* USB_UtilityP8ToFsBs16(const UINT8* Ptr)
{
    const UINT8 *src_ptr = Ptr;
    AMBA_FS_FAT_BS16 *ptr_ret;

    USB_UtilityMemoryCopy(&ptr_ret, &src_ptr, sizeof(UINT8*));

    return ptr_ret;
}

AMBA_FS_EXFAT_BS* USB_UtilityP8ToFsFxfatBs(const UINT8* Ptr)
{
    const UINT8 *src_ptr = Ptr;
    AMBA_FS_EXFAT_BS *ptr_ret;

    USB_UtilityMemoryCopy(&ptr_ret, &src_ptr, sizeof(UINT8*));

    return ptr_ret;
}

void* USB_UtilityP8ToVp(const UINT8* Ptr)
{
    const UINT8 *src_ptr = Ptr;
    void *ptr_ret;

    USB_UtilityMemoryCopy(&ptr_ret, &src_ptr, sizeof(UINT8*));

    return ptr_ret;
}

UINT8* USB_UtilityVp2P8(const void* Ptr)
{
    const void *src_ptr = Ptr;
    UINT8 *ptr_ret;

    USB_UtilityMemoryCopy(&ptr_ret, &src_ptr, sizeof(void*));

    return ptr_ret;
}

PF_DRV_TBL* USB_UtilityVoidP2PrFDrvTbl(const void* VoidPtr)
{
    const void *src_ptr = VoidPtr;
    PF_DRV_TBL *ptr_ret;

    USB_UtilityMemoryCopy(&ptr_ret, &src_ptr, sizeof(void*));

    return ptr_ret;
}

UINT32 USB_UtilityVirDataDesc2PhyI32(const UDC_DATA_DESC_s* VirPtr)
{
    const UDC_DATA_DESC_s *tmp_ptr = VirPtr;
    const void *tmp_virt_ptr, *tmp_phy_ptr;
    UINT32 addr_ret;

    // convert data pointer to void pointer
    USB_UtilityMemoryCopy(&tmp_virt_ptr, &tmp_ptr, sizeof(UDC_DATA_DESC_s *));

    tmp_phy_ptr = AmbaRTSL_USBVirt2Phys(tmp_virt_ptr);

    // convert void pointer to integer
    USB_UtilityMemoryCopy(&addr_ret, &tmp_phy_ptr, sizeof(UINT32));

    return addr_ret;
}

UINT32 USB_UtilityU8PtrToU32Addr(const UINT8* Ptr)
{
    UINT32 addr_ret;
    addr_ret = IO_UtilityPtrToU32Addr(Ptr);
    return addr_ret;
}

UINT8 *USB_UtilityU32AddrToU8Ptr(UINT32 Addr)
{
    UINT8 *u8ptr_ret;
    const void  *vptr;
    vptr = IO_UtilityU32AddrToPtr(Addr);
    USB_UtilityMemoryCopy(&u8ptr_ret, &vptr, sizeof(UINT32));
    return u8ptr_ret;
}

UINT32 *USB_UtilityU32AddrToU32Ptr(UINT32 Addr)
{
    UINT32 *u32ptr_ret;
    const void  *vptr;
    vptr = IO_UtilityU32AddrToPtr(Addr);
    USB_UtilityMemoryCopy(&u32ptr_ret, &vptr, sizeof(UINT32));
    return u32ptr_ret;
}

UINT32 USB_UtilityVirSetupDesc2PhyI32(const UDC_SETUP_DESC_s* VirPtr)
{
    const UDC_SETUP_DESC_s *tmp_ptr = VirPtr;
    const void  *tmp_virt_ptr, *tmp_phy_ptr;
    UINT32 addr_ret;

    // convert data pointer to void pointer
    USB_UtilityMemoryCopy(&tmp_virt_ptr, &tmp_ptr, sizeof(UDC_SETUP_DESC_s*));

    tmp_phy_ptr = AmbaRTSL_USBVirt2Phys(tmp_virt_ptr);

#ifdef CONFIG_ARM64
    // convert void pointer to integer
    addr_ret = IO_UtilityPtrToU32Addr(tmp_phy_ptr);
#else
    // convert void pointer to integer
    USB_UtilityMemoryCopy(&addr_ret, &tmp_phy_ptr, sizeof(UINT32));
#endif

    return addr_ret;
}

UDC_SETUP_DESC_s* USB_UtilityPhyI32ToVirSetupDesc(UINT32 PhyAddr)
{
    UDC_SETUP_DESC_s *ptr_ret;
    const void *tmp_phy_ptr, *tmp_virt_ptr;

#ifdef CONFIG_ARM64
    // convert addr to pointer
    tmp_phy_ptr = IO_UtilityU32AddrToPtr(PhyAddr);
    // physical to virtual
    tmp_virt_ptr = AmbaRTSL_USBPhys2Virt(tmp_phy_ptr);
    // convert void pointer to data pointer
    USB_UtilityMemoryCopy(&ptr_ret, &tmp_virt_ptr, sizeof(UDC_SETUP_DESC_s *));
#else
    // convert integer to to void pointer
    USB_UtilityMemoryCopy(&tmp_phy_ptr, &PhyAddr, sizeof(UINT32));

    tmp_virt_ptr = AmbaRTSL_USBPhys2Virt(tmp_phy_ptr);

    // convert void pointer to data pointer
    USB_UtilityMemoryCopy(&ptr_ret, &tmp_virt_ptr, sizeof(UDC_SETUP_DESC_s *));
#endif
    return ptr_ret;
}

UINT8 *USB_UtilityPhyI32ToVirU8P(UINT32 PhyAddr)
{
    UINT8 *ptr_ret;
    const void *tmp_phy_ptr, *tmp_virt_ptr;

#ifdef CONFIG_ARM64
    // convert addr to pointer
    tmp_phy_ptr = IO_UtilityU32AddrToPtr(PhyAddr);
    // physical to virtual
    tmp_virt_ptr = AmbaRTSL_USBPhys2Virt(tmp_phy_ptr);
    // convert void pointer to data pointer
    USB_UtilityMemoryCopy(&ptr_ret, &tmp_virt_ptr, sizeof(UINT8 *));
#else
    // convert integer to to void pointer
    USB_UtilityMemoryCopy(&tmp_phy_ptr, &PhyAddr, sizeof(UINT32));

    tmp_virt_ptr = AmbaRTSL_USBPhys2Virt(tmp_phy_ptr);

    // convert void pointer to data pointer
    USB_UtilityMemoryCopy(&ptr_ret, &tmp_virt_ptr, sizeof(UINT8 *));
#endif
    return ptr_ret;
}


UDC_DATA_DESC_s* USB_UtilityPhyI32ToVirDataDesc(UINT32 PhyAddr)
{
    UDC_DATA_DESC_s *ptr_ret;
    const void *tmp_virt_ptr, *tmp_phy_ptr;

#ifdef CONFIG_ARM64
    // convert addr to pointer
    tmp_phy_ptr = IO_UtilityU32AddrToPtr(PhyAddr);
    // physical to virtual
    tmp_virt_ptr = AmbaRTSL_USBPhys2Virt(tmp_phy_ptr);
    // convert void pointer to data pointer
    USB_UtilityMemoryCopy(&ptr_ret, &tmp_virt_ptr, sizeof(UDC_DATA_DESC_s *));
#else
    // convert integer to void pointer
    USB_UtilityMemoryCopy(&tmp_phy_ptr, &PhyAddr, sizeof(UINT32));

    tmp_virt_ptr = AmbaRTSL_USBPhys2Virt(tmp_phy_ptr);

    // convert void pointer to data pointer
    USB_UtilityMemoryCopy(&ptr_ret, &tmp_virt_ptr, sizeof(UDC_DATA_DESC_s *));
#endif
    return ptr_ret;
}

UX_HOST_CLASS_STORAGE* USB_UtilityVoidP2UxHStrg(const void* VoidPtr)
{
    const void *src_ptr = VoidPtr;
    UX_HOST_CLASS_STORAGE *ptr_ret;

    USB_UtilityMemoryCopy(&ptr_ret, &src_ptr, sizeof(void*));

    return ptr_ret;
}

UX_HOST_CLASS_STORAGE_MEDIA *USB_UtilityVoidP2UxHStrgMedia(const void* VoidPtr)
{
    const void *src_ptr = VoidPtr;
    UX_HOST_CLASS_STORAGE_MEDIA *ptr_ret;

    USB_UtilityMemoryCopy(&ptr_ret, &src_ptr, sizeof(void*));

    return ptr_ret;
}

void USB_UtilityCacheFlushSetupDesc(const UDC_SETUP_DESC_s *Ptr, const UINT32 Range)
{
    const void *dest_ptr;
    const UDC_SETUP_DESC_s *src_ptr = Ptr;
    UINT32 address;

    USB_UtilityMemoryCopy(&dest_ptr, &src_ptr, sizeof(void*));

#ifdef CONFIG_ARM64
    address = IO_UtilityPtrToU32Addr(dest_ptr);
#else
    AmbaMisra_TypeCast32(&address, &dest_ptr);
#endif

    (void)AmbaCache_DataFlush(address, Range);
}

void USB_UtilityCacheInvdSetupDesc(const UDC_SETUP_DESC_s *Ptr, const UINT32 Range)
{
    const void *dest_ptr;
    const UDC_SETUP_DESC_s *src_ptr = Ptr;
    UINT32 address;

    USB_UtilityMemoryCopy(&dest_ptr, &src_ptr, sizeof(void*));

#ifdef CONFIG_ARM64
    address = IO_UtilityPtrToU32Addr(dest_ptr);
#else
    AmbaMisra_TypeCast32(&address, &dest_ptr);
#endif

    (void)AmbaCache_DataInvalidate(address, Range);
}

void USB_UtilityCacheFlushDataDesc(const UDC_DATA_DESC_s *Ptr, const UINT32 Range)
{
    const void *dest_ptr;
    const UDC_DATA_DESC_s *src_ptr = Ptr;
    UINT32 address;

    USB_UtilityMemoryCopy(&dest_ptr, &src_ptr, sizeof(void*));

#ifdef CONFIG_ARM64
    address = IO_UtilityPtrToU32Addr(dest_ptr);
#else
    AmbaMisra_TypeCast32(&address, &dest_ptr);
#endif
    (void)AmbaCache_DataFlush(address, Range);
}

void USB_UtilityCacheInvdDataDesc(const UDC_DATA_DESC_s *Ptr, const UINT32 Range)
{
    const void *dest_ptr;
    const UDC_DATA_DESC_s *src_ptr = Ptr;
    UINT32 address;

    USB_UtilityMemoryCopy(&dest_ptr, &src_ptr, sizeof(void*));

#ifdef CONFIG_ARM64
    address = IO_UtilityPtrToU32Addr(dest_ptr);
#else
    AmbaMisra_TypeCast32(&address, &dest_ptr);
#endif

    (void)AmbaCache_DataInvalidate(address, Range);
}

void USB_UtilityCacheFlushUInt8(const UINT8 *Ptr, const UINT32 Range)
{
    const void *dest_ptr;
    const UINT8 *src_ptr = Ptr;
    UINT32 address;

    USB_UtilityMemoryCopy(&dest_ptr, &src_ptr, sizeof(void*));

#ifdef CONFIG_ARM64
    address = IO_UtilityPtrToU32Addr(dest_ptr);
#else
    AmbaMisra_TypeCast32(&address, &dest_ptr);
#endif

    (void)AmbaCache_DataFlush(address, Range);
}

void USB_UtilityCacheInvdUInt8(const UINT8 *Ptr, const UINT32 Range)
{
    const void *dest_ptr;
    const UINT8 *src_ptr = Ptr;
    UINT32 address;

    USB_UtilityMemoryCopy(&dest_ptr, &src_ptr, sizeof(void*));

#ifdef CONFIG_ARM64
    address = IO_UtilityPtrToU32Addr(dest_ptr);
#else
    AmbaMisra_TypeCast32(&address, &dest_ptr);
#endif

    (void)AmbaCache_DataInvalidate(address, Range);
}

UINT32 USB_UtilityQueueCreate(AMBA_KAL_MSG_QUEUE_t *pMsgQueue, UINT8 *pMsgQueueBase, UINT32 MsgSize, UINT32 MaxNumMsg)
{
    UINT32 uret;

    if (AmbaKAL_MsgQueueCreate(pMsgQueue,
                               NULL,
                               MsgSize,
                               pMsgQueueBase,
                               MaxNumMsg * MsgSize) == 0U) {
        uret = USB_ERR_SUCCESS;
    } else {
        uret = USB_ERR_FAIL;
    }
    return uret;
}

UINT32 USB_UtilityQueueDelete(AMBA_KAL_MSG_QUEUE_t *pMsgQueue)
{
    UINT32 uret;

    if (AmbaKAL_MsgQueueDelete(pMsgQueue) == 0U) {
        uret = USB_ERR_SUCCESS;
    } else {
        uret = USB_ERR_FAIL;
    }
    return uret;
}

UINT32 USB_UtilityQueueIsrSend(AMBA_KAL_MSG_QUEUE_t *pMsgQueue, const UDC_ISR_REQUEST_s* Request, UINT32 Timeout)
{
    UINT32 uret;
    void  *dest_ptr;
    const UDC_ISR_REQUEST_s *src_ptr = Request;

    USB_UtilityMemoryCopy(&dest_ptr, &src_ptr, sizeof(void*));
    if (AmbaKAL_MsgQueueSend(pMsgQueue, dest_ptr, Timeout) == 0U) {
        uret = USB_ERR_SUCCESS;
    } else {
        uret = USB_ERR_QUEUE_SEND_FAIL;
    }
    return uret;
}

UINT32 USB_UtilityQueueIsrRecv(AMBA_KAL_MSG_QUEUE_t *pMsgQueue, const UDC_ISR_REQUEST_s* Request, UINT32 Timeout)
{
    UINT32 uret;
    void  *dest_ptr;
    const UDC_ISR_REQUEST_s *src_ptr = Request;

    USB_UtilityMemoryCopy(&dest_ptr, &src_ptr, sizeof(void*));
    if (AmbaKAL_MsgQueueReceive(pMsgQueue, dest_ptr, Timeout) == 0U) {
        uret = USB_ERR_SUCCESS;
    } else {
        uret = USB_ERR_FAIL;
    }
    return uret;
}

UINT32 USB_UtilityQueueIsoSend(AMBA_KAL_MSG_QUEUE_t *pMsgQueue, const UDC_ISO_REQUEST_s* Request, UINT32 Timeout)
{
    UINT32 uret;
    void *dest_ptr;
    const UDC_ISO_REQUEST_s *src_ptr = Request;

    USB_UtilityMemoryCopy(&dest_ptr, &src_ptr, sizeof(void*));
    if (AmbaKAL_MsgQueueSend(pMsgQueue, dest_ptr, Timeout) == 0U) {
        uret = USB_ERR_SUCCESS;
    } else {
        uret = USB_ERR_FAIL;
    }
    return uret;
}

UINT32 USB_UtilityQueueIsoRecv(AMBA_KAL_MSG_QUEUE_t *pMsgQueue, const UDC_ISO_REQUEST_s* Request, UINT32 Timeout)
{
    UINT32 uret;
    void  *dest_ptr;
    const UDC_ISO_REQUEST_s *src_ptr = Request;

    USB_UtilityMemoryCopy(&dest_ptr, &src_ptr, sizeof(void*));
    if (AmbaKAL_MsgQueueReceive(pMsgQueue, dest_ptr, Timeout) == 0U) {
        uret = USB_ERR_SUCCESS;
    } else {
        uret = USB_ERR_FAIL;
    }
    return uret;
}

UINT32 USB_UtilityEventCreate(AMBA_KAL_EVENT_FLAG_t *pEventFlag)
{
    UINT32 uret;

    if (AmbaKAL_EventFlagCreate(pEventFlag, NULL) == 0U) {
        uret = USB_ERR_SUCCESS;
    } else {
        uret = USB_ERR_FAIL;
    }
    return uret;
}

UINT32 USB_UtilityEventDelete(AMBA_KAL_EVENT_FLAG_t *pEventFlag)
{
    UINT32 uret;

    if (AmbaKAL_EventFlagDelete(pEventFlag) == 0U) {
        uret = USB_ERR_SUCCESS;
    } else {
        uret = USB_ERR_FAIL;
    }
    return uret;
}

UINT32 USB_UtilityEventSend(AMBA_KAL_EVENT_FLAG_t *pEventFlag, UINT32 Flags)
{
    UINT32 uret;

    if (AmbaKAL_EventFlagSet(pEventFlag, Flags) == 0U) {
        uret = USB_ERR_SUCCESS;
    } else {
        uret = USB_ERR_EVENT_FLAG_SET_FAIL;
    }
    return uret;
}

UINT32 USB_UtilityEventRecv(AMBA_KAL_EVENT_FLAG_t *pEventFlag, UINT32 ReqFlags, UINT32 Option,
                            UINT32 *pActualFlags, UINT32 Timeout)
{
    UINT32 uret;
    UINT32 any_or_all = 0;
    UINT32 auto_clear = 0;

    if (Option == USB_KAL_AND) {
        any_or_all = 1;
        auto_clear = 0;
    } else if (Option == USB_KAL_OR) {
        any_or_all = 0;
        auto_clear = 0;
    } else if (Option == USB_KAL_AND_CLEAR) {
        any_or_all = 1;
        auto_clear = 1;
    } else {
        // USB_KAL_OR_CLEAR by default
        any_or_all = 0;
        auto_clear = 1;
    }

    if (AmbaKAL_EventFlagGet(pEventFlag, ReqFlags, any_or_all, auto_clear, pActualFlags, Timeout) == 0U) {
        uret = USB_ERR_SUCCESS;
    } else {
        uret = USB_ERR_FAIL;
    }
    return uret;
}

UINT32 USB_UtilityMutexCreate(AMBA_KAL_MUTEX_t *Mutex)
{
    UINT32 uret;

    if (AmbaKAL_MutexCreate(Mutex, NULL) == 0U) {
        uret = USB_ERR_SUCCESS;
    } else {
        uret = USB_ERR_MUTEX_CREATE_FAIL;
    }

    return uret;
}

UINT32 USB_UtilityMutexDelete(AMBA_KAL_MUTEX_t *Mutex)
{
    UINT32 uret;

    if (AmbaKAL_MutexDelete(Mutex) == 0U) {
        uret = USB_ERR_SUCCESS;
    } else {
        uret = USB_ERR_MUTEX_DELETE_FAIL;
    }

    return uret;
}

UINT32 USB_UtilityMutexTake(AMBA_KAL_MUTEX_t *Mutex, UINT32 Timeout)
{
    UINT32 uret;
    UINT32 func_ret = AmbaKAL_MutexTake(Mutex, Timeout);

    if (func_ret == 0U) {
        uret = USB_ERR_SUCCESS;
    } else if (func_ret == TX_NO_INSTANCE) {
        uret = USB_ERR_MUTEX_LOCK_TIMEOUT;
    } else {
        uret = USB_ERR_MUTEX_LOCK_FAIL;
    }

    return uret;
}

UINT32 USB_UtilityMutexGive(AMBA_KAL_MUTEX_t *Mutex)
{
    UINT32 uret;

    if (AmbaKAL_MutexGive(Mutex) == 0U) {
        uret = USB_ERR_SUCCESS;
    } else {
        uret = USB_ERR_FAIL;
    }

    return uret;
}

UINT32 USB_UtilityTaskCreate(
    AMBA_KAL_TASK_t *pTask,
    char *           pTaskName,
    UINT32           Priority,
    void *(*EntryFunction)(void *Arg),
    void *           EntryArg,
    void *           pStackBase,
    UINT32           StackByteSize,
    UINT32           AutoStart,
    UINT32           AffinityMask)
{
    // create taks with no start.
    UINT32 func_ret;
    UINT32 uret;

    // pass misra check
    AmbaMisra_TouchUnused(EntryArg);

    func_ret = AmbaKAL_TaskCreate(pTask,
                                  pTaskName,
                                  Priority,
                                  EntryFunction,
                                  EntryArg,
                                  pStackBase,
                                  StackByteSize,
                                  USB_KAL_DO_NOT_START);

    if (func_ret == TX_SUCCESS) {
#ifndef AMBA_KAL_NO_SMP
        UINT32 tmp_aff_mask = AffinityMask;
        tmp_aff_mask &= usb_core_mask;
        if (AmbaKAL_TaskSetSmpAffinity(pTask, tmp_aff_mask) != TX_SUCCESS) {
            uret = USB_ERR_FAIL;
        } else {
            if (AutoStart == USB_KAL_AUTO_START) {
                // kick task start.
                if (AmbaKAL_TaskResume(pTask) != TX_SUCCESS) {
                    uret = USB_ERR_FAIL;
                } else {
                    uret = USB_ERR_SUCCESS;
                }
            } else {
                uret = USB_ERR_SUCCESS;
            }
        }
#else
        (void)AffinityMask;
        (void)usb_core_mask;

        if (AutoStart == KAL_START_AUTO) {
            // kick task start.
            if (AmbaKAL_TaskResume(pTask) != TX_SUCCESS) {
                uret = USB_ERR_FAIL;
            } else {
                uret = USB_ERR_SUCCESS;
            }
        } else {
            uret = USB_ERR_SUCCESS;
        }
#endif
    } else {
        uret = USB_ERR_FAIL;
    }

    return uret;
}

UINT32 USB_UtilityTaskDelete(AMBA_KAL_TASK_t *pTask, UINT32 Suspended)
{
    UINT32 uret;
    UINT32 leave = 0;

    if (Suspended != 0U) {
        AMBA_KAL_TASK_INFO_s TaskInfo;
        UINT32 CheckCnt = 10;
        do {
            if (AmbaKAL_TaskQuery(pTask, &TaskInfo) == 0U) {
                // Hanle sleep and all other suspend state.
                if (TaskInfo.TaskState >= 5U) { // 5 is sleep state in KAL API
                    if (tx_thread_wait_abort(pTask) != (UINT32)TX_SUCCESS) {
                        // shall not be here.
                    }
                }
                CheckCnt--;
                if (CheckCnt == 0U) {
                    leave = 1;
                }
            } else {
                leave = 1;
            }

            if (leave != 0U) {
                break;
            }
            USB_UtilityTaskSleep(1);
        } while (TaskInfo.TaskState != 3U); // 3 is suspend state in KAL API
    }

    if (leave == 0U) {
        if (AmbaKAL_TaskTerminate(pTask) == 0U) {
            if (AmbaKAL_TaskDelete(pTask) == 0U) {
                uret = USB_ERR_SUCCESS;
            } else {
                uret = USB_ERR_FAIL;
            }
        } else {
            uret = USB_ERR_FAIL;
        }
    } else {
        uret = USB_ERR_FAIL;
    }

    return uret;
}

UINT32 USB_UtilitySemaphoreCreate(AMBA_KAL_SEMAPHORE_t *Sem, UINT32 InitCnt)
{
    UINT32 uret;

    if (AmbaKAL_SemaphoreCreate(Sem, NULL, InitCnt) == 0U) {
        uret = USB_ERR_SUCCESS;
    } else {
        uret = USB_ERR_FAIL;
    }

    return uret;
}

UINT32 USB_UtilitySemaphoreDelete(AMBA_KAL_SEMAPHORE_t *Sem)
{
    UINT32 uret;

    if (AmbaKAL_SemaphoreDelete(Sem) == 0U) {
        uret = USB_ERR_SUCCESS;
    } else {
        uret = USB_ERR_FAIL;
    }

    return uret;
}

UINT32 USB_UtilitySemaphoreTake(AMBA_KAL_SEMAPHORE_t *Sem, UINT32 Timeout)
{
    UINT32 uret;
    UINT32 func_ret = AmbaKAL_SemaphoreTake(Sem, Timeout);

    if (func_ret == 0U) {
        uret = USB_ERR_SUCCESS;
    } else if (func_ret == KAL_ERR_TIMEOUT) {
        uret = USB_ERR_SEMA_TAKE_TIMEOUT;
    } else {
        uret = USB_ERR_SEMA_TAKE_FAIL;
    }

    return uret;
}

UINT32 USB_UtilitySemaphoreGive(AMBA_KAL_SEMAPHORE_t *Sem)
{
    UINT32 uret;

    if (AmbaKAL_SemaphoreGive(Sem) == 0U) {
        uret = USB_ERR_SUCCESS;
    } else {
        uret = USB_ERR_FAIL;
    }

    return uret;
}

UINT32 USB_UtilitySemaphoreQuery(AMBA_KAL_SEMAPHORE_t *Sem, UINT32 *CurCnt)
{
    UINT32 uret;
    AMBA_KAL_SEMAPHORE_INFO_s semaphore_info;

    if (AmbaKAL_SemaphoreQuery(Sem, &semaphore_info) == 0U) {
        *CurCnt = semaphore_info.CurrentCount;
        uret = USB_ERR_SUCCESS;
    } else {
        uret = USB_ERR_FAIL;
    }

    return uret;
}

static char uint32_to_digit(UINT32 Value)
{
    static const char num_base16_full_usb[16] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};
    char cret = '0';

    if (Value < 16U) {
        cret = num_base16_full_usb[Value];
    }
    return cret;
}

UINT32 USB_UtilityInt32ToStr(char *pBuffer, UINT32 BufferSize, INT32 Value, UINT32 Radix)
{
    UINT32 StrLength = 0;
    INT32  t = Value;
    char *pWorkChar1, *pWorkChar2 = pBuffer, Tmp;
    UINT32 idx1 = 0, idx2 = 0, swap_len;
    UINT32 Base = Radix, ut, uTmp;

    if (Base != 16U) {
        Base = 10;
    }

    /* variable length of string */
    if (t == 0) {
        pWorkChar2[idx2] = '0';
        idx2++;
        pWorkChar2[idx2] = '\0';         /* attach the null terminator */
        StrLength        = 1;
    } else {
        //----------------------------------------------------------
        if ((t < 0) && (Base == 10U)) {
            t                = -t;
            pWorkChar2[idx2] = '-';
            idx2++;
            pWorkChar2[idx2 + 1U] = '\0';   /* attach the null terminator */
        }

        ut         = (UINT32)t;
        pWorkChar1 = pWorkChar2;
        idx1       = idx2;

        while (ut > 0U) {
            uTmp = ut % Base;
            pWorkChar1[idx1] = uint32_to_digit(uTmp);
            idx1++;
            if (idx1 >= (BufferSize - 1U)) {
                // buffer is too small
                break;
            }
            ut /= Base;
        }
        pWorkChar1[idx1] = '\0';        /* attach the null terminator */
        StrLength        = idx1;

        // change the order of the digits
        swap_len = (idx1 - idx2) / 2U;  /* n = lenght of the string / 2 */
        while (swap_len > 0U) {
            if (idx1 > 0U) {
                idx1--;
            }
            Tmp = pWorkChar1[idx1];
            pWorkChar1[idx1] = pWorkChar2[idx2];
            pWorkChar2[idx2] = Tmp;
            idx2++;
            swap_len--;
        }
    }

    /* make sure at least a \0 is added.*/
    pBuffer[BufferSize-1U] = '\0';
    return StrLength;
}

UINT32 USB_UtilityUInt32ToStr(char *pBuffer, UINT32 BufferSize, UINT32 Value, UINT32 Radix)
{
    UINT32 StrLength = 0;
    UINT32 ut = Value, uTmp;
    char *pWorkChar1, *pWorkChar2 = pBuffer, Tmp;
    UINT32 idx1 = 0, idx2 = 0, swap_len;
    UINT32 Base = Radix;

    if (Base != 16U) {
        Base = 10;
    }

    /* variable length of string */
    if (ut == 0U) {
        pWorkChar2[idx2] = '0';
        idx2++;
        pWorkChar2[idx2] = '\0';         /* attach the null terminator */
        StrLength        = 1;
    } else {
        pWorkChar1 = pWorkChar2;
        idx1       = idx2;

        while (ut > 0U) {
            uTmp = ut % Base;
            pWorkChar1[idx1] = uint32_to_digit(uTmp);
            idx1++;
            if (idx1 >= (BufferSize - 1U)) {
                // buffer is too small
                break;
            }
            ut /= Base;
        }
        pWorkChar1[idx1] = '\0';             /* attach the null terminator */
        StrLength        = idx1;

        // change the order of the digits
        swap_len = idx1 / 2U;  /* n = lenght of the string / 2 */
        while (swap_len > 0U) {
            if (idx1 > 0U) {
                idx1--;
            }
            Tmp = pWorkChar1[idx1];
            pWorkChar1[idx1] = pWorkChar2[idx2];
            pWorkChar2[idx2] = Tmp;
            idx2++;
            swap_len--;
        }

    }

    /* make sure at least a \0 is added.*/
    if (BufferSize > 0U) {
        /* make sure at least a \0 is added.*/
        pBuffer[BufferSize-1U] = '\0';
    }
    return StrLength;
}

/*! \brief Put an UINT32 value into an UINT8 array in little endian order.
 */
void USB_UtilityUInt32Put(UINT8 *Addr, UINT32 Value)
{
    UINT32 tmp = Value;

    Addr[0] = (UINT8)(tmp & 0xFFU);
    tmp     = Value >> 8U;
    Addr[1] = (UINT8)(tmp & 0xFFU);
    tmp     = Value >> 16U;
    Addr[2] = (UINT8)(tmp & 0xFFU);
    tmp     = Value >> 24U;
    Addr[3] = (UINT8)(tmp & 0xFFU);
}

void USB_UtilityMemcpyUInt8(UINT8 *Dest, const UINT8 *Src, UINT32 Size)
{
    UINT32 i;

    for (i = 0U; i < Size; i++) {
        Dest[i] = Src[i];
    }
}

/*! \brief Suspend caller itself.
 */
UINT32 USB_UtilityTaskSuspendSelf(void)
{
    UINT32 uret = USB_ERR_SUCCESS;

    AMBA_KAL_TASK_t *task;

    if (AmbaKAL_TaskIdentify(&task) != 0U) {
        uret = USB_ERR_NO_INSTANCE;
    } else {
        uret = AmbaKAL_TaskSuspend(task);
        if (uret != 0U) {
            uret = USB_ERR_FAIL;
        }
    }
    return uret;
}

#if defined(CONFIG_DEVICE_TREE_SUPPORT)
static void fdt_query_module(USB_MODULE_SETTING_s *ModuleSetting, const char *CompatibleStr)
{
    ModuleSetting->IntType = IO_UtilityFDTPropertyU32Quick(0, CompatibleStr, "interrupts", 0);
    ModuleSetting->IntPin  = IO_UtilityFDTPropertyU32Quick(0, CompatibleStr, "interrupts", 1) + 32UL;
    ModuleSetting->BaseAddress = IO_UtilityFDTPropertyU32Quick(0, CompatibleStr, "reg", 0);
}
static void fdt_query_module_phandle(USB_MODULE_SETTING_s *ModuleSetting, const char *CompatibleStr, const char *PhandleName)
{
    UINT32 phandle = IO_UtilityFDTPropertyU32Quick(0, CompatibleStr, PhandleName, 0);
    if (phandle != 0U) {
        INT32 offset = IO_UtilityFDTPHandleOffset(phandle);
        ModuleSetting->BaseAddress = IO_UtilityFDTPropertyU32(offset, "reg", 0);
    }
}

static void fdt_query_host_pins(USB_HARDWARE_SETTING_s *HardwareSettings)
{
#if defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)
    // get phandle of "pinctrl-0" in "cdns-usb3" node
    const char *compatible_name = "ambarella,cdns-usb3";
#else
    // get phandle of "pinctrl-0" in "usbphy" node
    const char *compatible_name = "ambarella,usbphy";
#endif
    UINT32 phandle_pinctrl_0 = IO_UtilityFDTPropertyU32Quick(0, compatible_name, "pinctrl-0", 0);
    if (phandle_pinctrl_0 != 0U) {
        INT32  offset_pinctrl_0 = IO_UtilityFDTPHandleOffset(phandle_pinctrl_0);
        UINT32 pin_oc  = IO_UtilityFDTPropertyU32(offset_pinctrl_0, "amb,pinmux-ids", 0); // OC pin is in index 0
        UINT32 pin_pwr = IO_UtilityFDTPropertyU32(offset_pinctrl_0, "amb,pinmux-ids", 1); // PWR pin is in index 0

        if (pin_oc != 0U) {
            HardwareSettings->HostPinOverCurrent = pin_oc;
        }

        if (pin_pwr != 0U) {
            HardwareSettings->HostPinPowerEnable = pin_pwr;
        }
    }
}

#if defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)
static void fdt_query_usb32_controller(USB_HARDWARE_SETTING_s *HardwareSettings)
{
    UINT64 base_addr     = IO_UtilityFDTPropertyU64Quick(0, "ambarella,cdns-usb3", "reg", 0);
    UINT32 interrupt_pin = IO_UtilityFDTPropertyU32Quick(0, "ambarella,cdns-usb3", "interrupts", 1);

    if (base_addr != 0U) {
        // in case usb32 controller address is defined, we have to update it
        HardwareSettings->Udc32.BaseAddress = base_addr + 0x4000U;
        HardwareSettings->Xhci.BaseAddress  = base_addr + 0x8000U;
    }

    if (interrupt_pin != 0U) {
        // in case DTSI have interrupt pin defined, we have to update it
        HardwareSettings->Udc32.IntPin = interrupt_pin + 32U;
        HardwareSettings->Xhci.IntPin  = interrupt_pin + 32U;
    }
}

#endif

#endif

const USB_HARDWARE_SETTING_s *USB_UtilityHWInfoQuery(void)
{
    static USB_HARDWARE_SETTING_s usb_hw_setting = {
#if defined(CONFIG_SOC_CV2FS) || defined(CONFIG_SOC_CV22FS)
        // UDC
        { 0xE0006000UL, 71UL + 32UL, 0},
        // OHCI
        { 0xE001E000UL, 70UL + 32UL, 0},
        // EHCI
        { 0xE001F000UL, 69UL + 32UL, 0},
        // UDC32
        { 0x00000000UL, 0UL + 32UL, 0},
        // XHCI
        { 0x00000000UL, 0UL + 32UL, 0},
        // PHY32
        { 0x00000000UL, 0UL + 32UL, 0},
        // RCT
        { 0xED080000UL, 0UL, 0},
#elif defined(CONFIG_SOC_CV22) || defined(CONFIG_SOC_CV25) || defined(CONFIG_SOC_CV28) || defined(CONFIG_SOC_H32)
        // UDC
        { 0xE0006000UL, 67UL + 32UL, 0},
        // OHCI
        { 0xE001E000UL, 66UL + 32UL, 0},
        // EHCI
        { 0xE001F000UL, 65UL + 32UL, 0},
        // UDC32
        { 0x00000000UL, 0UL + 32UL, 0},
        // XHCI
        { 0x00000000UL, 0UL + 32UL, 0},
        // PHY32
        { 0x00000000UL, 0UL + 32UL, 0},
        // RCT
        { 0xED080000UL, 0UL, 0},
#elif defined(CONFIG_SOC_CV2)
        // UDC
        { 0xE0006000UL, 67UL + 32UL, 0},
        // OHCI
        { 0xE0008000UL, 66UL + 32UL, 0},
        // EHCI
        { 0xE0009000UL, 65UL + 32UL, 0},
        // UDC32
        { 0x00000000UL, 0UL + 32UL, 0},
        // XHCI
        { 0x00000000UL, 0UL + 32UL, 0},
        // PHY32
        { 0x00000000UL, 0UL + 32UL, 0},
        // RCT
        { 0xED080000UL, 0UL, 0},
#elif defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)
        // UDC
        { 0x20E0006000UL, 67U + 32U, 0},
        // OHCI
        { 0x0000000000UL, 66U + 32U, 0},
        // EHCI
        { 0x0000000000UL, 65U + 32U, 0},
        // UDC32
        { 0x2020004000UL, 223U + 32U, 0},
        // XHCI
        { 0x2020008000UL, 223U + 32U, 0},
        // PHY32
        { 0x2030000000UL, 0U + 32U, 0},
        // RCT
        { 0x20ED080000UL, 0U, 0},
#else
#error "USB: unsupport chip version."
#endif
        0, // HostPinOverCurrent
        0, // HostPinPowerEnable
    };

#if defined(CONFIG_DEVICE_TREE_SUPPORT)
    static UINT32 flag_init = 0;
    if (flag_init == 0U) {
        fdt_query_module(&usb_hw_setting.Udc, "ambarella,udc");
        fdt_query_module(&usb_hw_setting.Ehci, "ambarella,ehci");
        fdt_query_module(&usb_hw_setting.Ohci, "ambarella,ohci");
        //fdt_query_module(&usb_hw_setting.Phy32, "ambarella,usb32_phy");
        fdt_query_module_phandle(&usb_hw_setting.Rct, "ambarella,udc", "amb,rct-regmap");
        fdt_query_host_pins(&usb_hw_setting);

#if defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)
        fdt_query_usb32_controller(&usb_hw_setting);
        // for CV5, the RCT and UDC address has only 32-bit
        // we need to extent it to 64-bit
        usb_hw_setting.Udc.BaseAddress += 0x2000000000UL;
        usb_hw_setting.Rct.BaseAddress += 0x2000000000UL;
#endif

        flag_init = 1;

        USB_UtilityPrintUInt64_5(USB_PRINT_FLAG_INFO, "UDC20 BaseAddr = 0x%X, IntPin = 0x%X, IntType  0x%X", usb_hw_setting.Udc.BaseAddress, usb_hw_setting.Udc.IntPin, usb_hw_setting.Udc.IntType, 0, 0);
        USB_UtilityPrintUInt64_5(USB_PRINT_FLAG_INFO, "EHCI BaseAddr  = 0x%X, IntPin = 0x%X, IntType  0x%X", usb_hw_setting.Ehci.BaseAddress, usb_hw_setting.Ehci.IntPin, usb_hw_setting.Ehci.IntType, 0, 0);
        USB_UtilityPrintUInt64_5(USB_PRINT_FLAG_INFO, "OHCI BaseAddr  = 0x%X, IntPin = 0x%X, IntType  0x%X", usb_hw_setting.Ohci.BaseAddress, usb_hw_setting.Ohci.IntPin, usb_hw_setting.Ohci.IntType, 0, 0);
        USB_UtilityPrintUInt64_5(USB_PRINT_FLAG_INFO, "RCT BaseAddr   = 0x%X, IntPin = 0x%X, IntType  0x%X", usb_hw_setting.Rct.BaseAddress, usb_hw_setting.Rct.IntPin, usb_hw_setting.Rct.IntType, 0, 0);
        USB_UtilityPrintUInt64_5(USB_PRINT_FLAG_INFO, "UDC32 BaseAddr = 0x%X, IntPin = 0x%X, IntType  0x%X", usb_hw_setting.Udc32.BaseAddress, usb_hw_setting.Xhci.IntPin, usb_hw_setting.Xhci.IntType, 0, 0);
        USB_UtilityPrintUInt64_5(USB_PRINT_FLAG_INFO, "XHCI BaseAddr  = 0x%X, IntPin = 0x%X, IntType  0x%X", usb_hw_setting.Xhci.BaseAddress, usb_hw_setting.Xhci.IntPin, usb_hw_setting.Xhci.IntType, 0, 0);
        USB_UtilityPrintUInt64_5(USB_PRINT_FLAG_INFO, "PHY32 BaseAddr = 0x%X, IntPin = 0x%X, IntType  0x%X", usb_hw_setting.Phy32.BaseAddress, usb_hw_setting.Xhci.IntPin, usb_hw_setting.Xhci.IntType, 0, 0);
        USB_UtilityPrintUInt5(USB_PRINT_FLAG_INFO, "OC pin = 0x%X, PWR pin = 0x%X", usb_hw_setting.HostPinOverCurrent, usb_hw_setting.HostPinPowerEnable, 0, 0, 0);

    }
#endif

    return &usb_hw_setting;
}

void USB_UtilityRTSLInit(void)
{
    const USB_HARDWARE_SETTING_s *hws = USB_UtilityHWInfoQuery();
    RTSL_USB_HW_SETTING_s rtsl_hws;
    rtsl_hws.Udc.BaseAddress = hws->Udc.BaseAddress;
    rtsl_hws.Udc.IntPin      = hws->Udc.IntPin;
    rtsl_hws.Udc.IntType     = hws->Udc.IntType;
    rtsl_hws.Ehci.BaseAddress = hws->Ehci.BaseAddress;
    rtsl_hws.Ehci.IntPin      = hws->Ehci.IntPin;
    rtsl_hws.Ehci.IntType     = hws->Ehci.IntType;
    rtsl_hws.Ohci.BaseAddress = hws->Ohci.BaseAddress;
    rtsl_hws.Ohci.IntPin      = hws->Ohci.IntPin;
    rtsl_hws.Ohci.IntType     = hws->Ohci.IntType;
    rtsl_hws.Rct.BaseAddress  = hws->Rct.BaseAddress;
    rtsl_hws.Rct.IntPin       = hws->Rct.IntPin;
    rtsl_hws.Rct.IntType      = hws->Rct.IntType;
    AmbaRTSL_UsbSetHwInfo(&rtsl_hws);
}

#define USB_PRINT_BUFFER_LEN          (512U)
static UINT32             usb_print_flags = 0;
static AMBA_USB_PRINT_f   usb_print_func= NULL;
static AMBA_KAL_MUTEX_t   usb_print_mutex;
static char               usb_print_buffer[USB_PRINT_BUFFER_LEN];
static UINT32             flag_usb_print_init = 0;

void AmbaUSB_SystemPrintFuncRegister(AMBA_USB_PRINT_f PrintFunc, UINT32 Flags)
{
    if (flag_usb_print_init == 0U) {
        if (USB_UtilityMutexCreate(&usb_print_mutex) == 0U) {
            usb_print_func      = PrintFunc;
            usb_print_flags     = Flags;
            flag_usb_print_init = 1;
        }
    }
}

UINT32 USB_UtilityPrintLevelSupport(UINT32 ControlFlags)
{
    UINT32 uret = 0;
    if ((usb_print_func != NULL) && (flag_usb_print_init == 1U)) {
        if ((ControlFlags & usb_print_flags) == ControlFlags) {
            uret = 1;
        }
    }
    return uret;
}

void USB_UtilityPrint(UINT32 ControlFlags, const char *Str)
{
    if ((usb_print_func != NULL) && (flag_usb_print_init == 1U)) {
        if ((ControlFlags & usb_print_flags) == ControlFlags) {
            usb_print_func(Str);
        }
    }
}

void USB_UtilityPrintUInt1(UINT32 ControlFlags, const char *FmtStr, UINT32 Arg1)
{
    USB_UtilityPrintUInt5(ControlFlags, FmtStr, Arg1, 0, 0, 0, 0);
}

void USB_UtilityPrintUInt5(UINT32 ControlFlags, const char *FmtStr, UINT32 Arg1, UINT32 Arg2, UINT32 Arg3, UINT32 Arg4, UINT32 Arg5)
{
    if ((usb_print_func != NULL) && (flag_usb_print_init == 1U)) {
        if ((ControlFlags & usb_print_flags) == ControlFlags) {
            if (USB_UtilityMutexTake(&usb_print_mutex, 1000) == 0U) {
                UINT32 args[5];
                UINT32 length;
                args[0] = Arg1;
                args[1] = Arg2;
                args[2] = Arg3;
                args[3] = Arg4;
                args[4] = Arg5;
                length = IO_UtilityStringPrintUInt32(usb_print_buffer, USB_PRINT_BUFFER_LEN, FmtStr, 5, args);
                if (length > 0U) {
                    usb_print_func(usb_print_buffer);
                }

                if (USB_UtilityMutexGive(&usb_print_mutex) != 0U) {
                    // ignore this error
                }
            }
        }
    }
}

void  USB_UtilityPrintUInt64_5(UINT32 ControlFlags, const char *FmtStr, UINT64 Arg1, UINT64 Arg2, UINT64 Arg3, UINT64 Arg4, UINT64 Arg5)
{
    if ((usb_print_func != NULL) && (flag_usb_print_init == 1U)) {
        if ((ControlFlags & usb_print_flags) == ControlFlags) {
            if (USB_UtilityMutexTake(&usb_print_mutex, 1000) == 0U) {
                UINT64  args[5];
                UINT32 length;
                args[0] = Arg1;
                args[1] = Arg2;
                args[2] = Arg3;
                args[3] = Arg4;
                args[4] = Arg5;
                length = IO_UtilityStringPrintUInt64(usb_print_buffer, USB_PRINT_BUFFER_LEN, FmtStr, 5, args);
                if (length > 0U) {
                    usb_print_func(usb_print_buffer);
                }

                if (USB_UtilityMutexGive(&usb_print_mutex) != 0U) {
                    // ignore this error
                }
            }
        }
    }
}

void  USB_UtilityPrintUInt64_1(UINT32 ControlFlags, const char *FmtStr, UINT64 Arg1)
{
    if ((usb_print_func != NULL) && (flag_usb_print_init == 1U)) {
        if ((ControlFlags & usb_print_flags) == ControlFlags) {
            if (USB_UtilityMutexTake(&usb_print_mutex, 1000) == 0U) {
                UINT64  args[2];
                UINT32 length;
                args[0] = Arg1;
                length = IO_UtilityStringPrintUInt64(usb_print_buffer, USB_PRINT_BUFFER_LEN, FmtStr, 1, args);
                if (length > 0U) {
                    usb_print_func(usb_print_buffer);
                }

                if (USB_UtilityMutexGive(&usb_print_mutex) != 0U) {
                    // ignore this error
                }
            }
        }
    }
}


void  USB_UtilityPrintStr5(UINT32 ControlFlags, const char *FmtStr, const char *Arg1, const char *Arg2, const char *Arg3, const char *Arg4, const char *Arg5)
{
    if ((usb_print_func != NULL) && (flag_usb_print_init == 1U)) {
        if ((ControlFlags & usb_print_flags) == ControlFlags) {
            if (USB_UtilityMutexTake(&usb_print_mutex, 1000) == 0U) {
                const char  *args[5];
                UINT32       length;
                args[0] = Arg1;
                args[1] = Arg2;
                args[2] = Arg3;
                args[3] = Arg4;
                args[4] = Arg5;
                length = IO_UtilityStringPrintStr(usb_print_buffer, USB_PRINT_BUFFER_LEN, FmtStr, 5, args);
                if (length > 0U) {
                    usb_print_func(usb_print_buffer);
                }

                if (USB_UtilityMutexGive(&usb_print_mutex) != 0U) {
                    // ignore this error
                }
            }
        }
    }
}

void USB_UtilityAssert(void)
{
    extern void AmbaAssert(void);
    AmbaAssert();
}

/**
 * @param Timeout the unit is ms.
 */
UINT32 USB_UtilityWaitForReg(UINT64 RegAddr, UINT32 Mask, UINT32 WaitFor, UINT32 Timeout)
{
    UINT32 counter = Timeout;
    UINT32 ret = 0;

    while ((IO_UtilityRegRead32(RegAddr) & Mask) != WaitFor) {
        // break loop when timeout value is 0
        if (counter == 0U) {
            ret = USB_ERR_TIMEOUT;
            break;
        }
        counter--;
        USB_UtilityTaskSleep(1);
    }
    return ret;
}


