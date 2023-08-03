/**
 *  @file AmbaPCIE_Utility.h
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
 *  @details PCIe utility header file.
 */
#ifndef AMBAPCIE_UTILITY_H_
#define AMBAPCIE_UTILITY_H_

#include <AmbaWrap.h>
#include <AmbaMisraFix.h>
#include <AmbaKAL.h>

#define PCIE_PRINT_SUPPORT


typedef struct {
    UINT32             IntID;
} PCIE_ISR_REQUEST_s;

void PCIE_UtilityLog2(DOUBLE x, void *pV);
void PCIE_UtilityMemoryCopy(void *pDst, const void *pSrc, SIZE_t num);
void PCIE_UtilityMemorySet(void *ptr, INT32 v, SIZE_t n);

UINT64 PCIE_UtilityVp2U64(const void* Ptr);
void  *PCIE_UtilityU64AddrToPtr(UINT64 Addr);
UINT32 *PCIE_UtilityU64AddrToU32Ptr(UINT64 Addr);
UINT8* PCIE_UtilityVp2U8p(const void* Ptr);
PCIE_xd_desc *PCIE_UtilityVoidP2XdDescP(const void* VoidPtr);
UINT32 *PCIE_UtilityXdDescP2U32p(const PCIE_xd_desc *Ptr);
UINT64 PCIE_UtilityXdDescP2U64(const PCIE_xd_desc *Ptr);

void PCIE_RegPtrWrite8(volatile UINT8 *Ptr, UINT8 Value);
void PCIE_RegPtrWrite16(volatile UINT16 *Ptr, UINT16 Value);
void PCIE_RegPtrWrite32(volatile UINT32 *Ptr, UINT32 Value);

UINT8 PCIE_RegPtrRead8(const volatile UINT8 *Ptr);
UINT16 PCIE_RegPtrRead16(const volatile UINT16 *Ptr);
UINT32 PCIE_RegPtrRead32(const volatile UINT32 *Ptr);

void PCIE_RegAddrWrite8(UINT64 Address, UINT8 Value);
void PCIE_RegAddrWrite16(UINT64 Address, UINT16 Value);
void PCIE_RegAddrWrite32(UINT64 Address, UINT32 Value);

UINT8 PCIE_RegAddrRead8(UINT64 Address);
UINT16 PCIE_RegAddrRead16(UINT64 Address);
UINT32 PCIE_RegAddrRead32(UINT64 Address);

UINT32 PCIE_RegFldRead(UINT32 Mask, UINT32 Shift, UINT32 RegValue);
UINT32 PCIE_RegFldWrite(UINT32 Mask, UINT32 Shift, UINT32 RegValue, UINT32 Value);
UINT32 PCIE_RegFldSet(UINT32 Width, UINT32 Mask, UINT32 IsWoclr, UINT32 RegValue);
UINT32 PCIE_RegFldClear(UINT32 Width, UINT32 Mask, UINT32 IsWoset, UINT32 IsWoclr,  UINT32 RegValue);

UINT32 PCIE_UtilityQueueCreate(AMBA_KAL_MSG_QUEUE_t *pMsgQueue, UINT8 *pMsgQueueBase, UINT32 MsgSize, UINT32 MaxNumMsg);
UINT32 PCIE_UtilityQueueDelete(AMBA_KAL_MSG_QUEUE_t *pMsgQueue);
UINT32 PCIE_UtilityQueueIsrSend(AMBA_KAL_MSG_QUEUE_t *pMsgQueue, const PCIE_ISR_REQUEST_s* Request, UINT32 Timeout);
UINT32 PCIE_UtilityQueueIsrRecv(AMBA_KAL_MSG_QUEUE_t *pMsgQueue, const PCIE_ISR_REQUEST_s* Request, UINT32 Timeout);

void PCIE_UtilityPrint(const char *Str);
void PCIE_UtilityPrintUInt5(const char *FmtStr, UINT32 Arg1, UINT32 Arg2, UINT32 Arg3, UINT32 Arg4, UINT32 Arg5);
void PCIE_UtilityPrintInt5(const char *FmtStr, INT32 Arg1, INT32 Arg2, INT32 Arg3, INT32 Arg4, INT32 Arg5);
void PCIE_UtilityPrintStr5(const char *FmtStr, const char *Arg1, const char *Arg2, const char *Arg3, const char *Arg4, const char *Arg5);


/****************************************************************************/
/****************************************************************************/
/*                                MACROS                                    */
/****************************************************************************/
/****************************************************************************/

/** ECAM address shift for the Bus number
 */
#define S_ECAM_B                                       20

/** ECAM address shift for the Device number
 */
#define S_ECAM_D                                       15

/** ECAM address shift for the Function number
 */
#define S_ECAM_F                                       12

/** Form the ECAM address
 */
static inline UINT64 ECAMADDR(
    UINT64 base,
    UINT64 bus,
    UINT64 device,
    UINT64 function,
    UINT64 reg)
{
    return  ((base)                            | \
             (((bus)      & 0xffU ) << S_ECAM_B) | \
             (((device)   & 0x1fU ) << S_ECAM_D) | \
             (((function) & 0x7U  ) << S_ECAM_F) | \
             (((reg)      & 0xfffU)            ) );
}

static inline UINT64 ARI_ECAMADDR(
    UINT64 base,
    UINT64 bus,
    UINT64 function,
    UINT64 reg)
{
    /* Convert bus/function to integer offset */
    return  ((base)                            | \
             (((bus)      & 0xffUL ) << S_ECAM_B) | \
             (((function) & 0xffUL ) << S_ECAM_F) | \
             (((reg)      & 0xfffUL)            ) );
}

static inline UINT32 * ConvertUintptrToUint32ptr(UINT64 input)
{
    UINT32 *ptr_ret;
    const void *vptr = PCIE_UtilityU64AddrToPtr(input);
    if (AmbaWrap_memcpy(&ptr_ret, &vptr, sizeof(void *)) != 0U) {
        // action TBD
    }
    return ptr_ret;
}

/****************************************************************************/
/*   Functions for uDMA                                                     */
/****************************************************************************/
static inline void SetControlBit(UINT8 *dst, UINT8 bitNum)
{

    /* Restrict shift value to 8 bits */
    if (bitNum < 8U) {
        *dst |= (UINT8)(1U << bitNum);
    }
    return;
}

static inline void ClearControlBit(UINT8 *dst, UINT8 bitNum)
{
    /* Restrict shift value to 8 bits */
    if (bitNum < 8U) {
        *dst &= ~(1U << bitNum);
    }
    return;
}

static inline void ClearReservedBits(UINT8 *p_ctrl_bits)
{
    /* Clear reserved_0 bits */
    ClearControlBit(p_ctrl_bits, 3);
    ClearControlBit(p_ctrl_bits, 4);

    /* Clear reserved_1 bits */
    ClearControlBit(p_ctrl_bits, 6);
    ClearControlBit(p_ctrl_bits, 7);
    return;
}
static inline UINT8 SetUDMA_ControlBits(
    UINT8 *           p_ctrl_bits,
    PCIE_UdmaContinuity continuity_value,
    PCIE_Bool           interrupt_value,
    PCIE_Bool           continue_on_value)
{
    /* set/clear interrupt bit */
    if (interrupt_value == PCIE_TRUE) {
        SetControlBit(p_ctrl_bits,0);
    } else {
        ClearControlBit(p_ctrl_bits,0);
    }
    /* Set/clear continuity bits */
    if (continuity_value == PCIE_READ_WRITE) {
        ClearControlBit(p_ctrl_bits, 1);
        ClearControlBit(p_ctrl_bits, 2);
    }
    if (continuity_value == PCIE_PREFETCH) {
        SetControlBit(p_ctrl_bits, 1);
        ClearControlBit(p_ctrl_bits, 2);
    }
    if (continuity_value == PCIE_POSTWRITE) {
        ClearControlBit(p_ctrl_bits, 1);
        SetControlBit(p_ctrl_bits, 2);
    }

    /* Set/clear continue_on bit*/
    if (continue_on_value == PCIE_TRUE) {
        SetControlBit(p_ctrl_bits, 5);
    } else {
        ClearControlBit(p_ctrl_bits, 5);
    }
    /* clear reserved_0 and reserved_1 bits */
    ClearReservedBits(p_ctrl_bits);

    return (*p_ctrl_bits);
}

static inline PCIE_xd_desc * ConvertUintptrToXdDesc(UINT64 input)
{
    PCIE_xd_desc *ptr_ret;
    const void *vptr = PCIE_UtilityU64AddrToPtr(input);

    if (AmbaWrap_memcpy(&ptr_ret, &vptr, sizeof(void *)) != 0U) {
        // action TBD
    }
    return ptr_ret;
}

static inline PCIE_xd_desc * ConvertUint32ptrToXdDesc(const UINT32 * input)
{
    const UINT32 *src_ptr = input;
    PCIE_xd_desc *ptr_ret;

    if (AmbaWrap_memcpy(&ptr_ret, &src_ptr, sizeof(UINT32 *)) != 0U) {
        // action TBD
    }
    return ptr_ret;
}

/* parasoft-end-suppress METRICS-36-3 */
static inline UINT64 ConvertUint32ptrToUintptr(const UINT32 * input)
{
    const UINT32 *src_ptr = input;
    UINT64 val_ret;

    if (AmbaWrap_memcpy(&val_ret, &src_ptr, sizeof(UINT32 *)) != 0U) {
        // action TBD
    }
    return val_ret;
}

static inline UINT64 ConvertXdDescToUintptr(const PCIE_xd_desc * input)
{
    const PCIE_xd_desc *src_ptr = input;
    const void  *tmp_ptr;
    UINT64 addr_ret;

    // convert data pointer to void pointer
    if (AmbaWrap_memcpy(&tmp_ptr, &src_ptr, sizeof(PCIE_xd_desc *)) != 0U ) {
        // action TBD
    }
    if (AmbaWrap_memcpy(&addr_ret, &tmp_ptr, sizeof(UINT64)) != 0U ) {
        // action TBD
    }
#if defined(CONFIG_ARM32)
    addr_ret &= 0xFFFFFFFFU;
#endif
    return addr_ret;
}

static inline UINT64 ConvertTdCfgToUintptr(const PCIE_td_cfg* input)
{
    const PCIE_td_cfg *src_ptr = input;
    const void  *tmp_ptr;
    UINT64 addr_ret;

    // convert data pointer to void pointer
    if (AmbaWrap_memcpy(&tmp_ptr, &src_ptr, sizeof(PCIE_td_cfg *)) != 0U ) {
        // action TBD
    }
    if (AmbaWrap_memcpy(&addr_ret, &tmp_ptr, sizeof(UINT64)) != 0U ) {
        // action TBD
    }
#if defined(CONFIG_ARM32)
    addr_ret &= 0xFFFFFFFFU;
#endif
    return addr_ret;
}


/****************************************************************************/
/* Macro to modify trafic class bits for AXI wrapper descriptor 0          */
/****************************************************************************/
static inline UINT32 AxiWrapObRegTrafficClassDataMod (UINT32 destination, UINT8 source)
{
    UINT32 ret_value;
    ret_value = ((destination) & ~0x000E0000U) | (((UINT32)source << 17U) & 0x000E0000U);
    return (ret_value);
}

#endif /* AMBAPCIE_UTILITY_H_ */
