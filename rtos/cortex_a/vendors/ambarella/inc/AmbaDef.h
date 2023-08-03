/**
 *  @file AmbaDef.h
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
 *  @details Inline function definitions
 *
 */

#ifndef AMBA_DEF_H
#define AMBA_DEF_H

#ifndef AMBA_TYPES_H
#include "AmbaTypes.h"
#endif

static inline UINT8  GetAbsValU8        (INT8  Val)
{
    return (((Val) >= 0) ? (UINT8) (Val) : (UINT8) (-(Val)));
}
static inline UINT16 GetAbsValU16       (INT16 Val)
{
    return (((Val) >= 0) ? (UINT16)(Val) : (UINT16)(-(Val)));
}
static inline UINT32 GetAbsValU32       (INT32 Val)
{
    return (((Val) >= 0) ? (UINT32)(Val) : (UINT32)(-(Val)));
}
static inline UINT64 GetAbsValU64       (INT64 Val)
{
    return (((Val) >= 0) ? (UINT64)(Val) : (UINT64)(-(Val)));
}

static inline UINT8  GetMaxValU8        (UINT8  Val0, UINT8  Val1)
{
    return (((Val0) > (Val1)) ? (Val0) : (Val1));
}
static inline UINT16 GetMaxValU16       (UINT16 Val0, UINT16 Val1)
{
    return (((Val0) > (Val1)) ? (Val0) : (Val1));
}
static inline UINT32 GetMaxValU32       (UINT32 Val0, UINT32 Val1)
{
    return (((Val0) > (Val1)) ? (Val0) : (Val1));
}
static inline UINT64 GetMaxValU64       (UINT64 Val0, UINT64 Val1)
{
    return (((Val0) > (Val1)) ? (Val0) : (Val1));
}
static inline FLOAT  GetMaxValFloat     (FLOAT  Val0, FLOAT  Val1)
{
    return (((Val0) > (Val1)) ? (Val0) : (Val1));
}
static inline DOUBLE GetMaxValDouble    (DOUBLE Val0, DOUBLE Val1)
{
    return (((Val0) > (Val1)) ? (Val0) : (Val1));
}

static inline UINT8  GetMinValU8        (UINT8  Val0, UINT8  Val1)
{
    return (((Val0) < (Val1)) ? (Val0) : (Val1));
}
static inline UINT16 GetMinValU16       (UINT16 Val0, UINT16 Val1)
{
    return (((Val0) < (Val1)) ? (Val0) : (Val1));
}
static inline UINT32 GetMinValU32       (UINT32 Val0, UINT32 Val1)
{
    return (((Val0) < (Val1)) ? (Val0) : (Val1));
}
static inline UINT64 GetMinValU64       (UINT64 Val0, UINT64 Val1)
{
    return (((Val0) < (Val1)) ? (Val0) : (Val1));
}
static inline FLOAT  GetMinValFloat     (FLOAT  Val0, FLOAT  Val1)
{
    return (((Val0) < (Val1)) ? (Val0) : (Val1));
}
static inline DOUBLE GetMinValDouble    (DOUBLE Val0, DOUBLE Val1)
{
    return (((Val0) < (Val1)) ? (Val0) : (Val1));
}

static inline UINT8  GetRoundUpValU8    (UINT8  Val, UINT8  NumDigits)
{
    return (0x0U != NumDigits) ? (((Val) + (NumDigits) - 0x1U) / (NumDigits)) : (Val);
}
static inline UINT16 GetRoundUpValU16   (UINT16 Val, UINT16 NumDigits)
{
    return (0x0U != NumDigits) ? (((Val) + (NumDigits) - 0x1U) / (NumDigits)) : (Val);
}
static inline UINT32 GetRoundUpValU32   (UINT32 Val, UINT32 NumDigits)
{
    return (0x0U != NumDigits) ? (((Val) + (NumDigits) - 0x1U) / (NumDigits)) : (Val);
}
static inline UINT64 GetRoundUpValU64   (UINT64 Val, UINT64 NumDigits)
{
    return (0x0U != NumDigits) ? (((Val) + (NumDigits) - 0x1U) / (NumDigits)) : (Val);
}

static inline UINT8  GetRoundDownValU8  (UINT8  Val, UINT8  NumDigits)
{
    return (0x0U != NumDigits) ? ((Val) / (NumDigits)) : (Val);
}
static inline UINT16 GetRoundDownValU16 (UINT16 Val, UINT16 NumDigits)
{
    return (0x0U != NumDigits) ? ((Val) / (NumDigits)) : (Val);
}
static inline UINT32 GetRoundDownValU32 (UINT32 Val, UINT32 NumDigits)
{
    return (0x0U != NumDigits) ? ((Val) / (NumDigits)) : (Val);
}
static inline UINT64 GetRoundDownValU64 (UINT64 Val, UINT64 NumDigits)
{
    return (0x0U != NumDigits) ? ((Val) / (NumDigits)) : (Val);
}

static inline UINT32 GetAlignedValU32   (UINT32 Val, UINT32 AlignBase)
{
    return (((Val) + (AlignBase - 1U)) & ~(AlignBase - 1U));
}
static inline UINT64 GetAlignedValU64   (UINT64 Val, UINT64 AlignBase)
{
    return (((Val) + (AlignBase - 1U)) & ~(AlignBase - 1U));
}

static inline UINT32 SetBits            (UINT32 Value, UINT32 Bits)
{
    return ((Value) |  (Bits));
}
static inline UINT32 ClearBits          (UINT32 Value, UINT32 Bits)
{
    return ((Value) & ~(Bits));
}
static inline UINT32 CheckBits          (UINT32 Value, UINT32 Bits)
{
    return ((Value) &  (Bits));
}
static inline UINT32 ToggleBits         (UINT32 Value, UINT32 Bits)
{
    return ((Value) ^  (Bits));
}

void AmbaAssert(void);
void AmbaDelayCycles(UINT32 Delays);

#endif  /* AMBA_DEF_H */
