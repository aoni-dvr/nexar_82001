/*
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
 */

#ifndef AMBA_CSL_GDMA_H
#define AMBA_CSL_GDMA_H

#include "AmbaReg_GDMA.h"

/*
 * Inline Function Definitions
 */
static inline void AmbaCSL_GdmaSetSrc1Base(UINT32 Val)
{
    pAmbaGDMA_Reg->Src1Base = Val;
}
static inline void AmbaCSL_GdmaSetSrc2Base(UINT32 Val)
{
    pAmbaGDMA_Reg->Src2Base = Val;
}
static inline void AmbaCSL_GdmaSetDestBase(UINT32 Val)
{
    pAmbaGDMA_Reg->DestBase = Val;
}
static inline void AmbaCSL_GdmaSetSrc1Pitch(UINT32 Val)
{
    pAmbaGDMA_Reg->Src1Pitch = Val;
}
static inline void AmbaCSL_GdmaSetSrc2Pitch(UINT32 Val)
{
    pAmbaGDMA_Reg->Src2Pitch = Val;
}
static inline void AmbaCSL_GdmaSetDestPitch(UINT32 Val)
{
    pAmbaGDMA_Reg->DestPitch = Val;
}

static inline void AmbaCSL_GdmaSetTransparent(UINT32 Val)
{
    pAmbaGDMA_Reg->Transparent = Val;
}
static inline void AmbaCSL_GdmaSetAlpha(UINT32 Val)
{
    pAmbaGDMA_Reg->Alpha = Val;
}
static inline void AmbaCSL_GdmaGo(UINT32 Val)
{
    pAmbaGDMA_Reg->OpCode = Val;
}

static inline UINT32 AmbaCSL_GdmaGetNumPendingInstance(void)
{
    return pAmbaGDMA_Reg->QueueDepth;
}

/*
 * Defined in AmbaCSL_GDMA.c
 */
void AmbaCSL_GdmaSetWidth(UINT32 Val);
void AmbaCSL_GdmaSetHeight(UINT32 Val);
void AmbaCSL_GdmaSetPixelFormat(UINT32 PixelFormat);
void AmbaCSL_GdmaSetDramThrottle(UINT32 MaxOutstandingRequest);

#endif /* AMBA_CSL_GDMA_H */
