/**
*  @file AmbaDSP_CommonAPI.h
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
*  @details Definitions & Constants for the APIs of SSP Common API
*
*/

#ifndef AMBADSP_COMMONAPI_H
#define AMBADSP_COMMONAPI_H

#if !defined (CONFIG_LINUX)
#include "AmbaKAL.h"
#endif
#include "AmbaTypes.h"
#include "dsp_osal.h"
#include "dsp_types.h"

//#define CHK_CACHE_ALIGN64

UINT32 IsAligned16(UINT16 In);
extern UINT32 IsAligned4(UINT16 In);
//extern UINT32 IsAligned8(UINT16 In);
//extern UINT32 IsValidAddr(const void* In);
//extern UINT32 IsValidU64Addr(const UINT64* In);
extern UINT32 IsValidU32Addr(const UINT32 In);
extern UINT32 IsValidULAddr(const ULONG In);
UINT32 Gcd(const UINT32 M, const UINT32 N);
extern UINT32 CalcCheckSum32Add(const UINT32 *pBuffer, UINT32 Size, UINT32 CrcIn);

/* Alignment */
extern UINT32 ALIGN_NUM(const UINT32 Src, const UINT32 Num);
extern UINT16 ALIGN_NUM16(const UINT16 Src, const UINT16 Num);
extern UINT64 ALIGN_NUM64(const UINT64 Src, const UINT64 Num);
extern UINT8 IS_ALIGN_NUM(const UINT32 Num, const UINT32 A);

/* MAX */
//extern UINT32 MAX2(const UINT32 A, const UINT32 B);
extern UINT16 MAX2_16(const UINT16 A, const UINT16 B);
//extern UINT32 MIN2(const UINT32 A, const UINT32 B);
//extern UINT32 MAX3(const UINT32 A, const UINT32 B, const UINT32 C);
//extern UINT32 MIN3(const UINT32 A, const UINT32 B, const UINT32 C);

/* Truncate */
UINT32 TRUNCATE(UINT32 Num, UINT32 Mask);
UINT16 TRUNCATE_16(UINT16 Num, UINT16 Mask);
//extern UINT32 CLIP(UINT32 Max, UINT32 Min, UINT32 A);

/* BitOperation */
extern void DSP_Idx2Bit(UINT8 Idx, UINT32 *Bit);
extern void DSP_U8Bit2U16Idx(UINT8 Bit, UINT16 *Idx);
extern void DSP_Bit2Idx(UINT32 Bit, UINT32 *Idx);
extern void DSP_Bit2U16Idx(UINT32 Bit, UINT16 *Idx);
extern void DSP_ReverseBit2U16Idx(UINT32 Bit, UINT16 *Idx);
extern void DSP_Bit2Cnt(UINT32 Bit, UINT32 *Cnt);
extern void DSP_Bit2U16Cnt(UINT32 Bit, UINT16 *Cnt);
extern UINT32 DSP_GetBit(UINT32 Bit, UINT32 Idx, UINT32 Num);
extern void DSP_SetBit(UINT32 *Bit, UINT32 Idx);
extern void DSP_ClearBit(UINT32 *Bit, UINT32 Idx);
extern UINT64 DSP_GetU64Bit(UINT64 Bit, UINT32 Idx, UINT32 Num);
extern void DSP_SetU64Bit(UINT64 *Bit, UINT32 Idx);
extern void DSP_ClearU64Bit(UINT64 *Bit, UINT32 Idx);
extern UINT32 DSP_GetU16Bit(UINT16 Bit, UINT32 Idx, UINT32 Num);
extern void DSP_SetU16Bit(UINT16 *Bit, UINT32 Idx);
extern void DSP_ClearU16Bit(UINT16 *Bit, UINT32 Idx);
extern UINT32 DSP_GetU8Bit(UINT8 Bit, UINT32 Idx, UINT32 Num);
extern void DSP_SetU8Bit(UINT8 *Bit, UINT32 Idx);
extern void DSP_ClearU8Bit(UINT8 *Bit, UINT32 Idx);
extern void DSP_ClearU32ArrayBit(UINT32 *Bit, UINT32 Idx, UINT8 Length);

/**
* Liveview GeoConfig update function
* @param [in]  pNumArray point to U32 array
* @param [in]  MaskLength total bitmask lenght
* @param [out] pIdx bit index of first empty bit
* @param [in]  FindOnly find empty bit without marking it
* @param [in]  ArrayDepth depth of aray in pNumArray
*/
extern void DSP_FindEmptyBit(UINT32 *pNumArray, UINT16 MaskLength, UINT16 *pIdx, UINT8 FindOnly, UINT8 ArrayDepth);
extern UINT32 DSP_GetU64Msb(UINT64 U64Val);
extern UINT32 DSP_GetU64Lsb(UINT64 U64Val);
UINT32 DSP_Num2Mask(UINT32 Num);

extern UINT32 DSP_GetPageSize(void);

/* Range Check */
extern UINT32 IsInsideRange(const ULONG Addr, const ULONG RangeStart, const UINT32 RangeSize);

/* Usefull window setting */
#define FHD_WIDTH       (1920U)
#define FHD_HEIGHT      (1080U)
#define HD_WIDTH        (1280U)
#define HD_HEIGHT       (720U)
#define UHD_WIDTH       (3840U)
#define UHD_HEIGHT      (2160U)
#define UHD_6M_WIDTH    (3200U)
#define UHD_6M_HEIGHT   (1800U)

typedef struct {
    UINT32 StartAddr;
    UINT32 Size;
    UINT32 Wp;
    UINT32 Rp;
} DSP_POOL_DESC_s;

/* Buffer request/release */
extern UINT32 DSP_ReqBuf(const DSP_POOL_DESC_s *BufPool, UINT32 ReqSize, UINT32 *NewWp, UINT32 FullnessCheck);
extern UINT32 DSP_RelBuf(const DSP_POOL_DESC_s *BufPool, UINT32 RelSize, UINT32 *NewRp, UINT32 FullnessCheck);

extern void LL_PrintErrLine(UINT32 Rval, const char *fn, UINT32 LineNum);
extern void DSP_FillErrline(UINT32 Rval, UINT32 *pErrLine, UINT32 LineNum);

#endif //AMBADSP_COMMONAPI_H
