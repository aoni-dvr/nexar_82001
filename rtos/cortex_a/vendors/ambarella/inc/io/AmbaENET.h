/**
 * @file AmbaENET.h
 * API prototype
 *
 * @ingroup enet
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
 */

#ifndef AMBA_ENET_H
#define AMBA_ENET_H

#include "AmbaENET_Def.h"

UINT32 AmbaEnet_SetConfig(const UINT32 Idx, AMBA_ENET_CONFIG_s *pEnetConfig);
UINT32 AmbaEnet_GetConfig(const UINT32 Idx, AMBA_ENET_CONFIG_s **ppEnetConfig);
UINT32 AmbaEnet_Init(AMBA_ENET_CONFIG_s *pEnetConfig);
UINT32 AmbaEnet_Tx(const UINT32 Idx, const UINT16 TxLen);
UINT32 AmbaEnet_TxWait(const UINT32 Idx, const UINT16 TxLen, UINT32 *pSec, UINT32 *pNs);
UINT32 AmbaEnet_SetupTxDesc(const UINT32 Idx, const void *Addr, const UINT16 Len, UINT8 FS, UINT8 LS, UINT8 CacheClean);
UINT32 AmbaEnet_GetTxBuf(const UINT32 Idx, void **Addr);
UINT32 AmbaEnet_GetRxBuf(const UINT32 Idx, void **Addr);
UINT32 AmbaEnet_SetPPS(UINT32 Idx, const UINT32 PPSINT, const UINT32 PPSWIDTH);
UINT32 AmbaEnet_SetTargetTsPPS(UINT32 Idx, const UINT32 Sec, const UINT32 Ns, UINT8 PPSCMD);
UINT32 AmbaEnet_SetTargetTsIrq(UINT32 Idx, const UINT32 Sec, const UINT32 Ns);
UINT32 AmbaEnet_GetRxTs(UINT32 Idx, UINT32 *pSec, UINT32 *pNs);
UINT32 AmbaEnet_PhcSetTs(UINT32 Idx, const UINT32 Sec, const UINT32 Ns);
UINT32 AmbaEnet_PhcUpdaTs(UINT32 Idx, UINT8 ADDSUB, const UINT32 Sec, const UINT32 Ns);
UINT32 AmbaEnet_PhcAdjFreq(UINT32 Idx, INT32 ppb);
UINT32 AmbaEnet_McastRxHash(UINT32 Idx, const UINT8 *Addr);
UINT32 AmbaEnet_IfUp(const UINT32 Idx, UINT32 *Speed);
UINT32 AmbaEnet_IfDown(const UINT32 Idx);
UINT32 AmbaEnet_PhyRead(UINT32 Idx, UINT32 PhyAddr, UINT32 Offset, UINT32 *Value);
UINT32 AmbaEnet_PhyWrite(UINT32 Idx, UINT32 PhyAddr, UINT32 Offset, UINT32 Value);

#endif /* AMBA_ENET_H */
