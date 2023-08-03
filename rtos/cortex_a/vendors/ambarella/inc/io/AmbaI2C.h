/**
 *  @file AmbaI2C.h
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
 *  @details Definitions & Constants for I2C Controller APIs
 *
 */

#ifndef AMBA_I2C_H
#define AMBA_I2C_H

#ifndef AMBA_I2C_DEF_H
#include "AmbaI2C_Def.h"
#endif

/*
 * Defined in AmbaI2C.c
 */
UINT32 AmbaI2C_MasterWrite(UINT32 MasterID, UINT32 BusSpeed, const AMBA_I2C_TRANSACTION_s *pTxTransaction, UINT32 *pActualTxSize, UINT32 TimeOut);
UINT32 AmbaI2C_MasterRead(UINT32 MasterID, UINT32 BusSpeed, const AMBA_I2C_TRANSACTION_s *pRxTransaction, UINT32 *pActualRxSize, UINT32 TimeOut);
UINT32 AmbaI2C_MasterReadAfterWrite(UINT32 MasterID, UINT32 BusSpeed, UINT32 NumTxTransaction, const AMBA_I2C_TRANSACTION_s * pTxTransaction, const AMBA_I2C_TRANSACTION_s * pRxTransaction, UINT32 * pActualSize, UINT32 TimeOut);
UINT32 AmbaI2C_MasterReadVarLength(UINT32 MasterID, UINT32 BusSpeed, UINT32 VarLenInfo, const AMBA_I2C_TRANSACTION_s *pRxTransaction, UINT32 *pActualRxSize, UINT32 TimeOut);
UINT32 AmbaI2C_MasterGetInfo(UINT32 MasterID, UINT32 BusSpeed, UINT32 * pActualBitRate);
#if defined(CONFIG_APP_FLOW_CARDV_AONI)
UINT32 AmbaI2C_MasterReadAfterWrite2(UINT32 MasterID, UINT32 BusSpeed, UINT32 NumTxTransaction, const AMBA_I2C_TRANSACTION_s * pTxTransaction, const AMBA_I2C_TRANSACTION_s * pRxTransaction, UINT32 * pActualSize, UINT32 TimeOut);
void AmbaI2C_CustomSetDelay(int ms);
#endif
UINT32 AmbaI2C_SlaveStart(UINT32 SlaveID, UINT32 SlaveAddr, AMBA_I2C_SLAVE_WRITE_ISR_f WriteIntFunc, AMBA_I2C_SLAVE_READ_ISR_f ReadIntFunc, AMBA_I2C_SLAVE_BREAK_ISR_f BreakIntFunc);
UINT32 AmbaI2C_SlaveStop(UINT32 SlaveID);

#endif /* AMBA_I2C_H */
