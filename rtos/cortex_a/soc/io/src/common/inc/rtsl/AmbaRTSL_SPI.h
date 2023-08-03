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

#ifndef AMBA_RTSL_SPI_H
#define AMBA_RTSL_SPI_H

#ifndef AMBA_SPI_DEF_H
#include "AmbaSPI_Def.h"
#endif

#ifdef CONFIG_QNX
#define AMBA_SPI_REF_CLK            24000000U
#endif

typedef void (*AMBA_SPI_ISR_CALLBACK_f)(UINT32 SpiChanNo);

/*
 * Defined in AmbaRTSL_SPI.c
 */
UINT32 AmbaRTSL_SpiInit(AMBA_SPI_ISR_CALLBACK_f CallBackFunc, AMBA_SPI_ISR_CALLBACK_f SlaveCallBackFunc);

UINT32 AmbaRTSL_SpiMasterConfig(UINT32 MasterID, UINT32 SlaveMask, const AMBA_SPI_CONFIG_s *pConfig);
UINT32 AmbaRTSL_SpiMasterTransferD8(UINT32 MasterID, UINT32 BufSize, const UINT8 *pTxBuf, UINT8 *pRxBuf);
UINT32 AmbaRTSL_SpiMasterTransferD16(UINT32 MasterID, UINT32 BufSize, const UINT16 *pTxBuf, UINT16 *pRxBuf);
UINT32 AmbaRTSL_SpiMasterDmaTranD8(UINT32 MasterID, UINT32 BufSize, const UINT8 *pTxBuf, UINT8 *pRxBuf);
UINT32 AmbaRTSL_SpiMasterDmaTranD16(UINT32 MasterID, UINT32 BufSize, const UINT16 *pTxBuf, UINT16 *pRxBuf);
UINT32 AmbaRTSL_SpiMasterGetInfo(UINT32 MasterID, UINT32 *pNumSlaves, UINT32 *pStatus, UINT32 *pTransCount);
void AmbaRTSL_SpiMasterStop(UINT32 MasterID);

UINT32 AmbaRTSL_SpiSlaveConfig(UINT32 SlaveID, const AMBA_SPI_CONFIG_s * pConfig);
UINT32 AmbaRTSL_SpiSlaveTransferD8(UINT32 SlaveID, UINT32 BufSize, const UINT8 *pTxBuf, UINT8 *pRxBuf);
UINT32 AmbaRTSL_SpiSlaveTransferD16(UINT32 SlaveID, UINT32 BufSize, const UINT16 *pTxBuf, UINT16 *pRxBuf);
UINT32 AmbaRTSL_SpiSlaveDmaTranD8(UINT32 SlaveID, UINT32 BufSize, const UINT8 *pTxBuf, UINT8 *pRxBuf);
UINT32 AmbaRTSL_SpiSlaveDmaTranD16(UINT32 SlaveID, UINT32 BufSize, const UINT16 *pTxBuf, UINT16 *pRxBuf);
UINT32 AmbaRTSL_SpiSlaveGetInfo(UINT32 SlaveID, UINT32 *pStatus, UINT32 *pTransCount);
void AmbaRTSL_SpiSlaveStop(UINT32 SlaveID);

void AmbaRTSL_SpiMasterGetPortAddr(UINT32 MasterID, ULONG *pDataPortAddr);
void AmbaRTSL_SpiSlaveGetPortAddr(UINT32 SlaveID, ULONG *pDataPortAddr);

UINT32 AmbaRTSL_SpiSlaveReadFifo(UINT32 SlaveID);
UINT32 AmbaRTSL_SpiSlaveGetReadSize(UINT32 SlaveID);

void SPI_MasterISR(UINT32 Irq, UINT32 SpiChanNo);
void SPI_SlaveISR(UINT32 Irq, UINT32 SpiChanNo);

#endif /* AMBA_RTSL_SPI_H */
