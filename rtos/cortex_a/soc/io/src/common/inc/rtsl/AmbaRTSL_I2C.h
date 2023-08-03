/**
 *  @file AmbaRTSL_I2C.h
 *
 *  @copyright Copyright (c) 2019 Ambarella, Inc.
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
 *  @details Definitions & Constants for I2C RTSL APIs
 *
 */

#ifndef AMBA_RTSL_I2C_H
#define AMBA_RTSL_I2C_H

#ifndef AMBA_I2C_DEF_H
#include "AmbaI2C_Def.h"
#endif

/*
 * Defined in AmbaRTSL_I2C.c
 */
UINT32 AmbaRTSL_I2cInit(void);
UINT32 AmbaRTSL_I2cIntHookHandler(AMBA_I2C_ISR_f I2cMasterIsrCb);
UINT32 AmbaRTSL_I2cWrite(UINT32 I2cChanNo, UINT32 I2cSpeed, const AMBA_I2C_TRANSACTION_s *pTxTransaction);
UINT32 AmbaRTSL_I2cRead(UINT32 I2cChanNo, UINT32 I2cSpeed, const AMBA_I2C_TRANSACTION_s *pRxTransaction);
UINT32 AmbaRTSL_I2cReadAfterWrite(UINT32 I2cChanNo, UINT32 I2cSpeed, UINT32 NumTxTransaction, const AMBA_I2C_TRANSACTION_s *pTxTransaction,
                                  const AMBA_I2C_TRANSACTION_s *pRxTransaction);
UINT32 AmbaRTSL_I2cReadVarLength(UINT32 I2cChanNo, UINT32 I2cSpeed, UINT32 VarLenInfo, const AMBA_I2C_TRANSACTION_s *pRxTransaction);
UINT32 AmbaRTSL_I2cStop(UINT32 I2cChanNo);
UINT32 AmbaRTSL_I2cTerminate(UINT32 I2cChanNo);
UINT32 AmbaRTSL_I2cGetResponse(UINT32 I2cChanNo);
UINT32 AmbaRTSL_I2cGetActualSize(UINT32 I2cChanNo);
UINT32 AmbaRTSL_I2cGetFinalState(UINT32 I2cChanNo);

UINT32 AmbaRTSL_I2cSlaveStart(UINT32 SlaveAddr, AMBA_I2C_SLAVE_WRITE_ISR_f WriteIntFunc, AMBA_I2C_SLAVE_READ_ISR_f ReadIntFunc, AMBA_I2C_SLAVE_BREAK_ISR_f BreakIntFunc);
UINT32 AmbaRTSL_I2cSlaveStop(void);

#if defined(CONFIG_APP_FLOW_CARDV_AONI)
int AmbaRTSL_I2cGetIrqStatus(UINT32 I2cChanNo);
void AmbaRTSL_I2cClearIrqStatus(UINT32 I2cChanNo);
void AmbaRTSL_I2cMasterIntHandler(UINT32 Irq, UINT32 I2cChanNo);
#endif
#endif /* AMBA_RTSL_I2C_H */
