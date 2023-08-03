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

#ifndef AMBA_RTSL_I2S_H
#define AMBA_RTSL_I2S_H

#ifndef AMBA_I2S_DEF_H
#include "AmbaI2S_Def.h"
#endif

/*
 * Defined in AmbaRTSL_I2S.c
 */
UINT32 AmbaRTSL_I2sInit(void);
UINT32 AmbaRTSL_I2sConfig(UINT32 I2sPortIdx, const AMBA_I2S_CTRL_s *pCtrl);
UINT32 AmbaRTSL_I2sTxTrigger(UINT32 I2sPortIdx, UINT32 Cmd);
UINT32 AmbaRTSL_I2sRxTrigger(UINT32 I2sPortIdx, UINT32 Cmd);
UINT32 AmbaRTSL_I2sTxResetFifo(UINT32 I2sPortIdx);
UINT32 AmbaRTSL_I2sRxResetFifo(UINT32 I2sPortIdx);
UINT32 AmbaRTSL_I2sTxGetDmaAddress(UINT32 I2sPortIdx, void **pTxDmaAddress);
UINT32 AmbaRTSL_I2sRxGetDmaAddress(UINT32 I2sPortIdx, void **pRxDmaAddress);
UINT32 AmbaRTSL_I2sTxWriteFifo(UINT32 I2sPortIdx, UINT32 Data);
UINT32 AmbaRTSL_I2sTxGetFifoStatus(UINT32 I2sPortIdx, UINT32 *pStatus);

#endif /* AMBA_RTSL_I2S_H */
