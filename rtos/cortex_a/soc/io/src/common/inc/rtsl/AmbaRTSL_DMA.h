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

#ifndef AMBA_RTSL_DMA_H
#define AMBA_RTSL_DMA_H

#ifndef AMBA_DMA_DEF_H
#include "AmbaDMA_Def.h"
#endif

/*
 * Defined in AmbaRTSL_DMA.c
 */
typedef void (*AMBA_DMA_ISR_CALLBACK_f)(UINT32 DmaChanNo);

void AmbaRTSL_DmaInit(void);
void AmbaRTSL_DmaHookIntHandler(AMBA_DMA_ISR_CALLBACK_f pIsr);

UINT32 AmbaRTSL_DmaTransfer(UINT32 DmaChanNo, AMBA_DMA_DESC_s *pDmaDesc);
UINT32 AmbaRTSL_DmaStop(UINT32 DmaChanNo);
UINT32 AmbaRTSL_DmaGetTrfByteCount(UINT32 DmaChanNo, UINT32 *pActualSize);

UINT32 AmbaRTSL_DmaChanAllocate(UINT32 DmaChanType, UINT32 *pDmaChanNo);
UINT32 AmbaRTSL_DmaChanRelease(UINT32 DmaChanNo);
UINT32 AmbaRTSL_DmaSetChanType(UINT32 DmaChanNo, UINT32 DmaChanType);
UINT32 AmbaRTSL_DmaGetIrqStatus(UINT32 DmaChanNo);
UINT32 AmbaRTSL_DmaGetIntCount(UINT32 DmaChanNo);
void AmbaRTSL_DmaResetIntCount(UINT32 DmaChanNo);

#endif /* AMBA_RTSL_DMA_H */
