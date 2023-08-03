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

#ifndef AMBA_RTSL_GDMA_H
#define AMBA_RTSL_GDMA_H

#ifndef AMBA_GDMA_DEF_H
#include "AmbaGDMA_Def.h"
#endif

/* GDMA OPCODE */
#define GDMA_OPCODE_LINEAR_COPY          (0U)
#define GDMA_OPCODE_2D_COPY_ONE_SRC      (1U)
#define GDMA_OPCODE_2D_COPY_TWO_SRC      (2U)
#define GDMA_OPCODE_ALPHA_WITH_PREMUL    (3U)
#define GDMA_OPCODE_RUN_LEN_DECODE       (4U)
#define GDMA_OPCODE_ALPHA_WITHOUT_PREMUL (5U)

/*
 * Defined in AmbaRTSL_GDMA.c
 */
void AmbaRTSL_GdmaInit(void);
void AmbaRTSL_GdmaHookIntHandler(AMBA_GDMA_ISR_f pIsr);

UINT32 AmbaRTSL_GdmaGetNumAvails(void);
UINT32 AmbaRTSL_GdmaLinearCopy(const AMBA_GDMA_LINEAR_s * pLinearBlit);
UINT32 AmbaRTSL_GdmaBlockCopy(const AMBA_GDMA_BLOCK_s * pBlockBlit);
UINT32 AmbaRTSL_GdmaColorKeying(const AMBA_GDMA_BLOCK_s * pBlockBlit, UINT32 TransparentColor);
UINT32 AmbaRTSL_GdmaAlphaBlending(const AMBA_GDMA_BLOCK_s * pBlockBlit, UINT32 AlphaVal, UINT32 BlendMode);

#endif /* AMBA_RTSL_GDMA_H */
