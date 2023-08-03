/**
 *  @file AmbaSPI.h
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
 *  @details Definitions & Constants for SPI Middleware APIs
 *
 */

#ifndef AMBA_SPI_H
#define AMBA_SPI_H

#ifndef AMBA_SPI_DEF_H
#include "AmbaSPI_Def.h"
#endif

/*
 * Defined in AmbaSPI.c
 */
UINT32 AmbaSPI_MasterTransferD8(UINT32 MasterID, UINT32 SlaveMask, const AMBA_SPI_CONFIG_s *pConfig,
                                UINT32 BufSize, const UINT8 *pTxBuf, UINT8 *pRxBuf, UINT32 *pActualSize, UINT32 TimeOut);
UINT32 AmbaSPI_MasterTransferD16(UINT32 MasterID, UINT32 SlaveMask, const AMBA_SPI_CONFIG_s *pConfig,
                                 UINT32 BufSize, const UINT16 *pTxBuf, UINT16 *pRxBuf, UINT32 *pActualSize, UINT32 TimeOut);
UINT32 AmbaSPI_MasterDmaTransferD8(UINT32 MasterID, UINT32 SlaveMask, const AMBA_SPI_CONFIG_s * pConfig,
                                   UINT32 BufSize, const UINT8 *pTxBuf, UINT8 *pRxBuf, UINT32 * pActualSize, UINT32 TimeOut);
UINT32 AmbaSPI_MasterDmaTransferD16(UINT32 MasterID, UINT32 SlaveMask, const AMBA_SPI_CONFIG_s * pConfig,
                                    UINT32 BufSize, const UINT16 *pTxBuf, UINT16 *pRxBuf, UINT32 * pActualSize, UINT32 TimeOut);
UINT32 AmbaSPI_MasterGetInfo(UINT32 MasterID, UINT32 * pNumSlaves, UINT32 * pStatus);

UINT32 AmbaSPI_SlaveConfig(UINT32 SlaveID, const AMBA_SPI_CONFIG_s * pConfig);
UINT32 AmbaSPI_SlaveTransferD8(UINT32 SlaveID, UINT32 BufSize, const UINT8 *pTxBuf, UINT8 *pRxBuf, UINT32 *pActualSize, UINT32 TimeOut);
UINT32 AmbaSPI_SlaveTransferD16(UINT32 SlaveID, UINT32 BufSize, const UINT16 *pTxBuf, UINT16 *pRxBuf, UINT32 *pActualSize, UINT32 TimeOut);
UINT32 AmbaSPI_SlaveDmaTransferD8(UINT32 SlaveID, UINT32 BufSize, const UINT8 *pTxBuf, UINT8 *pRxBuf, UINT32 * pActualSize, UINT32 TimeOut);
UINT32 AmbaSPI_SlaveDmaTransferD16(UINT32 SlaveID, UINT32 BufSize, const UINT16 *pTxBuf, UINT16 *pRxBuf, UINT32 * pActualSize, UINT32 TimeOut);
UINT32 AmbaSPI_SlaveGetInfo(UINT32 SlaveID, UINT32 *pStatus);

#endif /* AMBA_SPI_H */
