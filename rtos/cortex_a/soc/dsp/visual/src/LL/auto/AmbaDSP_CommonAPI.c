/**
*  @file AmbaDSP_CommonAPI.c
*
 * Copyright (c) 2020 Ambarella International LP
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
*  @details Implement of Common APIs
*
*/

#include "AmbaDSP.h"
#if defined(CONFIG_THREADX)
#include "AmbaMMU.h"
#include "AmbaCache.h"
#include "AmbaMemProt.h"
#endif
#include "AmbaDSP_Log.h"
#include "AmbaDSP_CommonAPI.h"

#if 0
//move to AmbaDSP_VideoEncode.c for fixing Coverity
/**
 * Is aligned 16?
 */
UINT32 IsAligned16(UINT16 In)
{
    UINT32 Rval = 0U;
    if ((In & 0xFU) == 0x0U) {
        Rval = 1U;
    }

    return Rval;

}
#endif

/**
 * Is aligned 4?
 */
UINT32 IsAligned4(UINT16 In)
{
    UINT32 Rval = 0U;

    if ((In & 0x3U) == 0x0U) {
        Rval = 1U;
    }

    return Rval;
}


/**
 * Is aligned 8?
 */
#if 0
UINT32 IsAligned8(UINT16 In)
{
    UINT32 Rval = 0U;

    if ((In & 0x7U) == 0x0U) {
        Rval = 1U;
    }

    return Rval;
}
#endif

static void LL_NullPrint(const char *fn)
{
    AmbaLL_LogStr5("[%s]Null Input", fn, NULL, NULL, NULL, NULL);
}
static void LL_RangePrint(const char *fn)
{
    AmbaLL_LogStr5("[%s] idx exceed", fn, NULL, NULL, NULL, NULL);
}

/* Alignment */
UINT32 ALIGN_NUM(const UINT32 Src, const UINT32 Num)
{
    return (((Src) + (Num-1U)) & (0xFFFFFFFFU - (Num-1U)));
}

UINT16 ALIGN_NUM16(const UINT16 Src, const UINT16 Num)
{
    return (((Src) + (Num-1U)) & (0xFFFFU - (Num-1U)));
}

UINT64 ALIGN_NUM64(const UINT64 Src, const UINT64 Num)
{
    return (((Src) + (Num-1U)) & (0xFFFFFFFFFFFFFFFFU - (Num-1U)));
}

UINT8 IS_ALIGN_NUM(const UINT32 Num, const UINT32 A)
{
    return ((Num & (A-1U)) > 0U)? 0U: 1U;
}

/* MAX */
UINT16 MAX2_16(const UINT16 A, const UINT16 B)
{
    return (((A)>(B))? (A):(B));
}

UINT32 MAX2(const UINT32 A, const UINT32 B)
{
    return (((A)>(B))? (A):(B));
}

#if 0
UINT32 MIN2(const UINT32 A, const UINT32 B)
{
    return (((A)>(B))? (B):(A));
}

UINT32 MAX3(const UINT32 A, const UINT32 B, const UINT32 C)
{
    return (MAX2((A),MAX2(B,C)));
}

UINT32 MIN3(const UINT32 A, const UINT32 B, const UINT32 C)
{
    return (MIN2((A),MIN2(B,C)));
}
#endif

/* Truncate */
UINT32 TRUNCATE(UINT32 Num, UINT32 Mask)
{
    return ((Num) & (0xFFFFFFFFU - (Mask-1U)));
}

UINT16 TRUNCATE_16(UINT16 Num, UINT16 Mask)
{
    return ((Num) & (0xFFFFU - (Mask-1U)));
}

#if 0
UINT32 CLIP(UINT32 Max, UINT32 Min, UINT32 A)
{
    return (((A) > (Max))? (Min): (((A) < (Min))? (Min): (A)));
}
#endif

/* BitOperation */
void DSP_Idx2Bit(UINT8 Idx, UINT32 *Bit)
{
    if (Bit == NULL) {
        LL_NullPrint(__func__);
    } else if (Idx > 31U) {
        LL_RangePrint(__func__);
    } else {
        *Bit = (UINT32)1UL << Idx;
    }
}

void DSP_Bit2Idx(UINT32 Bit, UINT32 *Idx)
{
    if (Idx == NULL) {
        LL_NullPrint(__func__);
    } else {
        UINT32 i;

        for (i=0; i<31U; i++) {
            if (((Bit>>i) & 0x1U) == 1U) {
                *Idx = i;
                break;
            }
        }
    }
}

void DSP_Bit2U16Idx(UINT32 Bit, UINT16 *Idx)
{
    if (Idx == NULL) {
        LL_NullPrint(__func__);
    } else {
        UINT32 i;

        for (i=0; i<31U; i++) {
            if (((Bit>>i) & 0x1U) == 1U) {
                *Idx = (UINT16)i;
                break;
            }
        }
    }
}

void DSP_ReverseBit2U16Idx(UINT32 Bit, UINT16 *Idx)
{
    if (Idx == NULL) {
        LL_NullPrint(__func__);
    } else {
        UINT32 i;

        for (i=0; i<32U; i++) {
            if (((Bit>>(31U-i)) & 0x1U) == 1U) {
                *Idx = (UINT16)(31U - i);
                break;
            }
        }
    }
}

void DSP_Bit2Cnt(UINT32 Bit, UINT32 *Cnt)
{
    if (Cnt == NULL) {
        LL_NullPrint(__func__);
    } else {
        UINT8 i;

        *Cnt = 0U;
        for (i=0; i<31U; i++) {
            if (((Bit>>i) & 0x1U) == 1U) {
                *Cnt += 1U;
            }
        }
    }
}

void DSP_Bit2U16Cnt(UINT32 Bit, UINT16 *Cnt)
{
    if (Cnt == NULL) {
        LL_NullPrint(__func__);
    } else {
        UINT8 i;

        *Cnt = 0U;
        for (i=0; i<31U; i++) {
            if (((Bit>>i) & 0x1U) == 1U) {
                *Cnt += 1U;
            }
        }
    }
}

UINT32 DSP_GetBit(UINT32 Bit, UINT32 Idx, UINT32 Num)
{
    UINT32 Value = 0;
    UINT32 BitMask = 0x0U;

    if (Idx > 31U) {
        LL_NullPrint(__func__);
    } else if (Num > 31U) {
        LL_RangePrint(__func__);
    } else {
        if (Num == 31U) {
            BitMask = 0xFFFFFFFFU;
        } else {
            BitMask = (UINT32)((0x1UL<<Num) - 0x1UL);
        }

        Value = (Bit>>Idx) & BitMask;
    }
    return Value;
}

void DSP_SetBit(UINT32 *Bit, UINT32 Idx)
{
    UINT32 BitMask = 0x0U;

    if (Bit == NULL) {
        LL_NullPrint(__func__);
    } else if (Idx > 31U) {
        LL_RangePrint(__func__);
    } else {
        BitMask = (UINT32)0x1U <<Idx;
        *Bit |= BitMask;
    }
}

void DSP_ClearBit(UINT32 *Bit, UINT32 Idx)
{
    UINT32 BitMask = 0x0U;

    if (Bit == NULL) {
        LL_NullPrint(__func__);
    } else if (Idx > 31U) {
        LL_RangePrint(__func__);
    } else {
        BitMask = (UINT32)0x1U << Idx;
        *Bit &= (~BitMask);
    }
}
void DSP_ClearU32ArrayBit(UINT32 *Bit, UINT32 Idx, UINT8 Length)
{
    UINT32 BitMask = 0x0U, ClearBit, ArrayIdx;

    if (Bit == NULL) {
        LL_NullPrint(__func__);
    } else if (Idx > ((32U*(UINT32)Length) - 1U)) {
        LL_RangePrint(__func__);
    } else {
        ClearBit = Idx%32U;
        ArrayIdx = Idx/32U;
        BitMask = (UINT32)0x1U << ClearBit;
        if (ArrayIdx < Length) {
            Bit[ArrayIdx] &= (~BitMask);
        }
    }
}

UINT32 DSP_GetU16Bit(UINT16 Bit, UINT32 Idx, UINT32 Num)
{
    UINT32 Value = 0;
    UINT32 BitMask = 0x0U;

    if (Idx > 15U) {
        LL_NullPrint(__func__);
    } else if (Num > 16U) {
        LL_RangePrint(__func__);
    } else {
        if (Num == 16U) {
            BitMask = 0xFFFFU;
        } else {
            BitMask = ((UINT32)0x1U<<Num) - 0x1U;
        }

        Value = ((UINT32)Bit >> Idx) & BitMask;
    }
    return Value;
}

void DSP_SetU16Bit(UINT16 *Bit, UINT32 Idx)
{
    UINT16 BitMask = 0x0U;

    if (Bit == NULL) {
        LL_NullPrint(__func__);
    } else if (Idx > 15U) {
        LL_RangePrint(__func__);
    } else {
        BitMask = (UINT16)((UINT32)0x1U << Idx);
        *Bit |= BitMask;
    }
}

void DSP_ClearU16Bit(UINT16 *Bit, UINT32 Idx)
{
    UINT16 BitMask = 0x0U;

    if (Bit == NULL) {
        LL_NullPrint(__func__);
    } else if (Idx > 15U) {
        LL_RangePrint(__func__);
    } else {
        BitMask = (UINT16)((UINT32)0x1U << Idx);
        *Bit &= (~BitMask);
    }
}

UINT32 DSP_GetU8Bit(UINT8 Bit, UINT32 Idx, UINT32 Num)
{
    UINT32 Value = 0;
    UINT32 BitMask = 0x0U;

    if (Idx > 7U) {
        LL_NullPrint(__func__);
    } else if (Num > 8U) {
        LL_RangePrint(__func__);
    } else {
        if (Num == 8U) {
            BitMask = 0xFFU;
        } else {
            BitMask = ((UINT32)0x1U<<Num) - 0x1U;
        }

        Value = ((UINT32)Bit >> Idx) & BitMask;
    }
    return Value;
}

void DSP_SetU8Bit(UINT8 *Bit, UINT32 Idx)
{
    UINT8 BitMask = 0x0U;

    if (Bit == NULL) {
        LL_NullPrint(__func__);
    } else if (Idx > 7U) {
        LL_RangePrint(__func__);
    } else {
        BitMask = (UINT8)((UINT32)0x1U << Idx);
        *Bit |= BitMask;
    }
}

void DSP_ClearU8Bit(UINT8 *Bit, UINT32 Idx)
{
    UINT8 BitMask = 0x0U;

    if (Bit == NULL) {
        LL_NullPrint(__func__);
    } else if (Idx > 7U) {
        LL_RangePrint(__func__);
    } else {
        BitMask = (UINT8)((UINT32)0x1U << Idx);
        *Bit &= (~BitMask);
    }
}

void DSP_FindEmptyBit(UINT32 *pNumArray, UINT16 MaskLength, UINT16 *pIdx, UINT8 FindOnly, UINT8 ArrayDepth)
{
    UINT32 i, j, Found = 0U;
    UINT16 Mask = 0U;

    if ((pNumArray == NULL) ||
        (pIdx == NULL)) {
        LL_NullPrint(__func__);
    } else if (MaskLength > 128U) {
        LL_RangePrint(__func__);
    } else {

        for (i=0U; i<ArrayDepth; i++) {
            if (MaskLength > ((i*32U)+31U)) {
                Mask = 31U;
            } else {
                Mask = MaskLength%32U;
            }

            for (j=0U; j<Mask; j++) {
                if (((pNumArray[i]>>j) & 0x1U) == 0U) {
                    Found = 1U;
                    *pIdx = (UINT16)((i*32U) + j);
                    if (FindOnly == 0U) {
                        DSP_SetBit(&pNumArray[i], j);
                    }
                    break;
                }
            }
            if (Found == 1U) {
                break;
            }
        }
    }
}

UINT32 DSP_GetU64Msb(UINT64 U64Val)
{
    return (UINT32)((U64Val >> 32U) & 0xFFFFFFFFU);
}

UINT32 DSP_GetU64Lsb(UINT64 U64Val)
{
    return (UINT32)(U64Val & 0xFFFFFFFFU);
}

#if 0
//move to AmbaDSP_ApiCheck.c for fixing Coverity
UINT32 DSP_Num2Mask(UINT32 Num)
{
    UINT32 i, Mask = 0U;
    for (i=0U; i<Num; i++) {
        DSP_SetBit(&Mask, i);
    }
    return Mask;
}
#endif

UINT32 DSP_GetPageSize(void)
{
    /*
     * PageSize comes from DDR Spec,
     * LPDDR4 generally is 4KByte(4096Byte)
     * if we embedded two Dram(dual channel, like cv2), we have double PageSize(4096*2)
     * To get PageSize, we can (1)via RTSL API(TBD) (2)via regsiter 0xDFFE0804, B[3:2], 0 means 1KB, 1 means 2KB, 4 means 4KB
     */
    UINT32 Size, Data;

    Size = dsp_osal_get_dram_page();
    Data = DSP_GetBit(Size, 2U, 2U);
    if (Data == 0U) {
        Size = 1024U;
    } else if (Data == 1U) {
        Size = 2048U;
    } else {
        Size = 4096U;
    }

    return Size;
}

UINT32 IsInsideRange(const ULONG Addr, const ULONG RangeStart, const UINT32 RangeSize)
{
    UINT32 Rval;
    ULONG RangeEnd = RangeStart + (ULONG)RangeSize;

    if ((Addr >= RangeStart) &&
        (Addr <= RangeEnd)) {
        Rval = 1U;
    } else {
        Rval = 0U;
    }
    return Rval;
}

#if 0
UINT32 IsValidAddr(const void* In)
{
    UINT32 Rval = 0U;

    if (In != NULL) {
        Rval = 1U;
    }

    return Rval;
}

UINT32 IsValidU64Addr(const UINT64* In)
{
    UINT32 Rval = 0U;

    if (In != NULL) {
        Rval = 1U;
    }

    return Rval;
}
#endif

UINT32 IsValidU32Addr(const UINT32 In)
{
    UINT32 Rval = 0U;

    if (In != 0U) {
        Rval = 1U;
    }

    return Rval;
}

UINT32 IsValidULAddr(const ULONG In)
{
    UINT32 Rval = 0U;

    if (In != 0U) {
        Rval = 1U;
    }

    return Rval;
}

void LL_PrintErrLine(UINT32 Rval, const char *fn, UINT32 LineNum)
{
    if (Rval != 0U) {
        AmbaLL_LogStr5("%s got NG result", fn, NULL, NULL, NULL, NULL);
        AmbaLL_LogUInt5("    Rval = 0x%x @ %d", Rval, LineNum, 0U, 0U, 0U);
    }
}

#if 0
//move to AmbaDSP_VideoEncode.c for fixing Coverity
UINT32 Gcd(const UINT32 M, const UINT32 N)
{
    UINT32 Tmp = 1U;
    UINT32 Val0 = M;
    UINT32 Val1 = N;

    while (Tmp != 0U) {
        Tmp = Val0 % Val1;
        Val0 = Val1;
        Val1 = Tmp;
    }
    return Val0;
}
#endif

void DSP_FillErrline(UINT32 Rval, UINT32 *pErrLine, UINT32 LineNum)
{
       if (Rval != DSP_ERR_NONE) {
           *pErrLine = LineNum;
       }
}

