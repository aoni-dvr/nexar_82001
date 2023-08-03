/**
 *  @file AmbaPCIE_Utility.c
 *
 *  Copyright (c) 2021 Ambarella International LP
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
 *  @details PCIe utility functions.
 */

#include "AmbaPCIE_Drv.h"
#ifdef PCIE_PRINT_SUPPORT
#include "../../../../svc/comsvc/print/AmbaPrint.h"
#endif

/** Wrapper function for C log2. */
void PCIE_UtilityLog2(DOUBLE x, void *pV)
{
    if (AmbaWrap_log2(x, pV) != 0U) {
        // action TDB
    }
}

/** Wrapper function for C memory copy. */
void PCIE_UtilityMemoryCopy(void *pDst, const void *pSrc, SIZE_t num)
{
    if (AmbaWrap_memcpy(pDst, pSrc, num) != 0U) {
        // action TDB
    }
}

/** Wrapper function for C memory set. */
void PCIE_UtilityMemorySet(void *ptr, INT32 v, SIZE_t n)
{
    if (AmbaWrap_memset(ptr, v, n) != 0U) {
        // action TDB
    }
}

UINT64 PCIE_UtilityVp2U64(const void* Ptr)
{
    const void *tmp_ptr = Ptr;
    UINT64 addr_ret;

    if (AmbaWrap_memcpy(&addr_ret, &tmp_ptr, sizeof(void *)) != 0U) {
        // action TBD
    }

    return addr_ret;
}

void  *PCIE_UtilityU64AddrToPtr(UINT64 Addr)
{
    void *ptr;

#if defined(CONFIG_ARM32)
    UINT32 u32_addr = (UINT32)Addr;
    if (AmbaWrap_memcpy(&ptr, &u32_addr, sizeof(void *)) != 0U) {
        // action TBD
    }
#else
    if (AmbaWrap_memcpy(&ptr, &Addr, sizeof(void *)) != 0U) {
        // action TBD
    }
#endif
    return ptr;

}

UINT32 *PCIE_UtilityU64AddrToU32Ptr(UINT64 Addr)
{
    UINT32 *ptr_ret;
    const void *vptr = PCIE_UtilityU64AddrToPtr(Addr);
    if (AmbaWrap_memcpy(&ptr_ret, &vptr, sizeof(void *)) != 0U) {
        // action TBD
    }
    return ptr_ret;
}

UINT8* PCIE_UtilityVp2U8p(const void* Ptr)
{
    const void *src_ptr = Ptr;
    UINT8 *ptr_ret;
    if (AmbaWrap_memcpy(&ptr_ret, &src_ptr, sizeof(void *)) != 0U) {
        // action TBD
    }
    return ptr_ret;
}

PCIE_xd_desc *PCIE_UtilityVoidP2XdDescP(const void* VoidPtr)
{
    const void *src_ptr = VoidPtr;
    PCIE_xd_desc *ptr_ret;

    if (AmbaWrap_memcpy(&ptr_ret, &src_ptr, sizeof(void *)) != 0U) {
        // action TBD
    }

    return ptr_ret;
}

UINT32 *PCIE_UtilityXdDescP2U32p(const PCIE_xd_desc *Ptr)
{
    const PCIE_xd_desc *src_ptr = Ptr;
    UINT32 *ptr_ret;

    if (AmbaWrap_memcpy(&ptr_ret, &src_ptr, sizeof(PCIE_xd_desc *)) != 0U) {
        // action TBD
    }

    return ptr_ret;
}

UINT64 PCIE_UtilityXdDescP2U64(const PCIE_xd_desc *Ptr)
{
    const PCIE_xd_desc *src_ptr = Ptr;
    UINT64 addr_ret;

    if (AmbaWrap_memcpy(&addr_ret, &src_ptr, sizeof(PCIE_xd_desc *)) != 0U) {
        // action TBD
    }

    return addr_ret;
}

void PCIE_RegPtrWrite8(volatile UINT8 *Ptr, UINT8 Value)
{
    if (Ptr != NULL) {
        *Ptr = Value;
    }
}

void PCIE_RegPtrWrite16(volatile UINT16 *Ptr, UINT16 Value)
{
    if (Ptr != NULL) {
        *Ptr = Value;
    }
}

void PCIE_RegPtrWrite32(volatile UINT32 *Ptr, UINT32 Value)
{
    if (Ptr != NULL) {
        *Ptr = Value;
    }
}

UINT8 PCIE_RegPtrRead8(const volatile UINT8 *Ptr)
{
    UINT8 ret_val;
    if (Ptr != NULL) {
        ret_val = *Ptr;
    } else {
        ret_val = 0;
    }
    return ret_val;
}

UINT16 PCIE_RegPtrRead16(const volatile UINT16 *Ptr)
{
    UINT16 ret_val;
    if (Ptr != NULL) {
        ret_val = *Ptr;
    } else {
        ret_val = 0;
    }
    return ret_val;
}

UINT32 PCIE_RegPtrRead32(const volatile UINT32 *Ptr)
{
    UINT32 ret_val;
    if (Ptr != NULL) {
        ret_val = *Ptr;
    } else {
        ret_val = 0;
    }
    return ret_val;
}

void PCIE_RegAddrWrite8(UINT64 Address, UINT8 Value)
{
    volatile UINT8 *uint8_ptr;
    const void *vptr = PCIE_UtilityU64AddrToPtr(Address);
    if (AmbaWrap_memcpy(&uint8_ptr, &vptr, sizeof(void *)) == 0U) {
        *uint8_ptr = Value;
    }
}

void PCIE_RegAddrWrite16(UINT64 Address, UINT16 Value)
{
    volatile UINT16 *uint16_ptr;
    const void *vptr = PCIE_UtilityU64AddrToPtr(Address);
    if (AmbaWrap_memcpy(&uint16_ptr, &vptr, sizeof(void *)) == 0U) {
        *uint16_ptr = Value;
    }
}

void PCIE_RegAddrWrite32(UINT64 Address, UINT32 Value)
{
    volatile UINT32 *uint32_ptr;
    const void *vptr = PCIE_UtilityU64AddrToPtr(Address);
    if (AmbaWrap_memcpy(&uint32_ptr, &vptr, sizeof(void *)) == 0U) {
        *uint32_ptr = Value;
    }
}

UINT8 PCIE_RegAddrRead8(UINT64 Address)
{
    const volatile UINT8 *uint8_ptr;
    const void *vptr = PCIE_UtilityU64AddrToPtr(Address);
    UINT8 uret = 0;
    if (AmbaWrap_memcpy(&uint8_ptr, &vptr, sizeof(void *)) == 0U) {
        uret = *uint8_ptr;
    }
    return uret;
}

UINT16 PCIE_RegAddrRead16(UINT64 Address)
{
    const volatile UINT16 *uint16_ptr;
    const void *vptr = PCIE_UtilityU64AddrToPtr(Address);
    UINT16 uret = 0;
    if (AmbaWrap_memcpy(&uint16_ptr, &vptr, sizeof(void *)) == 0U) {
        uret = *uint16_ptr;
    }
    return uret;
}

UINT32 PCIE_RegAddrRead32(UINT64 Address)
{
    const volatile UINT32 *uint32_ptr;
    const void *vptr = PCIE_UtilityU64AddrToPtr(Address);
    UINT32 uret = 0;
    if (AmbaWrap_memcpy(&uint32_ptr, &vptr, sizeof(void *)) == 0U) {
        uret = *uint32_ptr;
    }
    return uret;
}

UINT32 PCIE_RegFldRead(UINT32 Mask, UINT32 Shift, UINT32 RegValue)
{
    UINT32 Value = (RegValue & Mask) >> Shift;
    return (Value);
}

UINT32 PCIE_RegFldWrite(UINT32 Mask, UINT32 Shift, UINT32 RegValue, UINT32 Value)
{
    UINT32 NewValue = (Value << Shift) & Mask;
    NewValue = (RegValue & ~Mask) | NewValue;
    return (NewValue);
}

UINT32 PCIE_RegFldSet(UINT32 Width, UINT32 Mask, UINT32 IsWoclr, UINT32 RegValue)
{
    UINT32 NewValue = RegValue;
    if ((Width == 1U) && (IsWoclr == 0U)) {
        NewValue |= Mask;
    }
    return (NewValue);
}

UINT32 PCIE_RegFldClear(UINT32 Width, UINT32 Mask, UINT32 IsWoset, UINT32 IsWoclr,  UINT32 RegValue)
{
    UINT32 NewValue = RegValue;
    if ((Width == 1U) && (IsWoset == 0U)) {
        NewValue = (NewValue & ~Mask) | ((IsWoclr != 0U) ? Mask : 0U);
    }
    return (NewValue);
}


UINT32 PCIE_UtilityQueueCreate(AMBA_KAL_MSG_QUEUE_t *pMsgQueue, UINT8 *pMsgQueueBase, UINT32 MsgSize, UINT32 MaxNumMsg)
{
    UINT32 uret;

    if (AmbaKAL_MsgQueueCreate(pMsgQueue,
                               NULL,
                               MsgSize,
                               pMsgQueueBase,
                               MaxNumMsg * MsgSize) == 0U) {
        uret = PCIE_ERR_SUCCESS;
    } else {
        uret = PCIE_ERR_FAIL;
    }
    return uret;
}

UINT32 PCIE_UtilityQueueDelete(AMBA_KAL_MSG_QUEUE_t *pMsgQueue)
{
    UINT32 uret;

    if (AmbaKAL_MsgQueueDelete(pMsgQueue) == 0U) {
        uret = PCIE_ERR_SUCCESS;
    } else {
        uret = PCIE_ERR_FAIL;
    }
    return uret;
}

UINT32 PCIE_UtilityQueueIsrSend(AMBA_KAL_MSG_QUEUE_t *pMsgQueue, const PCIE_ISR_REQUEST_s* Request, UINT32 Timeout)
{
    UINT32 uret;
    void  *dest_ptr;
    const PCIE_ISR_REQUEST_s *src_ptr = Request;

    PCIE_UtilityMemoryCopy(&dest_ptr, &src_ptr, sizeof(void*));
    if (AmbaKAL_MsgQueueSend(pMsgQueue, dest_ptr, Timeout) == 0U) {
        uret = PCIE_ERR_SUCCESS;
    } else {
        uret = PCIE_ERR_FAIL;
    }
    return uret;
}

UINT32 PCIE_UtilityQueueIsrRecv(AMBA_KAL_MSG_QUEUE_t *pMsgQueue, const PCIE_ISR_REQUEST_s* Request, UINT32 Timeout)
{
    UINT32 uret;
    void  *dest_ptr;
    const PCIE_ISR_REQUEST_s *src_ptr = Request;

    PCIE_UtilityMemoryCopy(&dest_ptr, &src_ptr, sizeof(void*));
    if (AmbaKAL_MsgQueueReceive(pMsgQueue, dest_ptr, Timeout) == 0U) {
        uret = PCIE_ERR_SUCCESS;
    } else {
        uret = PCIE_ERR_FAIL;
    }
    return uret;
}


void PCIE_UtilityPrint(const char *Str)
{
#ifdef PCIE_PRINT_SUPPORT
    AmbaPrint_PrintStr5(Str, NULL, NULL, NULL, NULL, NULL);
#endif
}

void PCIE_UtilityPrintUInt5(const char *FmtStr, UINT32 Arg1, UINT32 Arg2, UINT32 Arg3, UINT32 Arg4, UINT32 Arg5)
{
#ifdef PCIE_PRINT_SUPPORT
    AmbaPrint_PrintUInt5(FmtStr, Arg1, Arg2, Arg3, Arg4, Arg5);
#endif
}

void PCIE_UtilityPrintInt5(const char *FmtStr, INT32 Arg1, INT32 Arg2, INT32 Arg3, INT32 Arg4, INT32 Arg5)
{
#ifdef PCIE_PRINT_SUPPORT
    AmbaPrint_PrintInt5(FmtStr, Arg1, Arg2, Arg3, Arg4, Arg5);
#endif
}

void PCIE_UtilityPrintStr5(const char *FmtStr, const char *Arg1, const char *Arg2, const char *Arg3, const char *Arg4, const char *Arg5)
{
#ifdef PCIE_PRINT_SUPPORT
    AmbaPrint_PrintStr5(FmtStr, Arg1, Arg2, Arg3, Arg4, Arg5);
#endif
}
