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

#ifndef AMBA_CSL_IRIF_H
#define AMBA_CSL_IRIF_H

#ifndef AMBA_IRIF_DEF_H
#include "AmbaIRIF_Def.h"
#endif

#include "AmbaReg_IRIF.h"

#define AMBA_IR_MAX_FIFO_ENTRY              64

/*
 * Inline Function Definitions
 */
static inline UINT32 AmbaCSL_IrGetDataCount(void)
{
    return (UINT32)pAmbaIR_Reg->Status.Count;
}
static inline UINT32 AmbaCSL_IrGetData(void)
{
    return (UINT32)pAmbaIR_Reg->Data.Data;
}

static inline void AmbaCSL_IrClearFifoOverflow(void)
{
    pAmbaIR_Reg->Ctrl.FifoOverflow = 1U;
}
static inline void AmbaCSL_IrClearInterrupt(void)
{
    pAmbaIR_Reg->Ctrl.IntStatus = 1U;
}

/*
 * Defined in AmbaCSL_IR.c
 */
void AmbaCSL_IrReset(void);

#endif /* AMBA_CSL_IRIF_H */
