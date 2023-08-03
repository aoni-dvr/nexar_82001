/**
 * @file util.c
 *
 * Copyright (c) 2019 Ambarella International LP
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
 * This file includes sample code and is only for internal testing and evaluation.  If you
 * distribute this sample code (whether in source, object, or binary code form), it will be
 * without any warranty or indemnity protection from Ambarella International LP or its affiliates.
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
 *
 */
#include "avdecc.h"
#include "avdecc_private.h"

void Avdecc_PrepareEth(const void *Buffer, UINT8 Vlan, const UINT8 *DstMac, const UINT8 *SrcMac)
{
    ETH_VLAN_HDR_s *VLanHdlr;
    ETH_HDR_s *EthHdlr;
    if (Vlan != 0U) { // TODO: VLAN
        AmbaMisra_TypeCast(&VLanHdlr, &Buffer);
        if (AmbaWrap_memcpy(VLanHdlr->dst, DstMac, (sizeof(UINT8) * 6U))!= 0U) { }
        if (AmbaWrap_memcpy(VLanHdlr->src, SrcMac, (sizeof(UINT8) * 6U))!= 0U) { }
        VLanHdlr->type = AVDECC_HTONS(0x22F0U);
    } else {
        AmbaMisra_TypeCast(&EthHdlr, &Buffer);
        if (AmbaWrap_memcpy(EthHdlr->dst, DstMac, (sizeof(UINT8) * 6U))!= 0U) { }
        if (AmbaWrap_memcpy(EthHdlr->src, SrcMac, (sizeof(UINT8) * 6U))!= 0U) { }
        EthHdlr->type = AVDECC_HTONS(0x22F0U);
    }
}

UINT16 Avdecc_GetStreamVlanId(void)
{
    return 0x4567u;
}

/* Generate Entity ID */
UINT64 Avdecc_GenEntityId(const UINT8 *Mac, UINT16 StreamId)
{
    UINT8 i;
    UINT64 EntityId = 0U;
    for (i = 0; i < 3U; i++) {
        EntityId *= 0x100U;
        EntityId += Mac[i];
    }
    EntityId *= 0x10000U;
    EntityId += StreamId;
    for (; i < 6U; i++) {
        EntityId *= 0x100U;
        EntityId += Mac[i];
    }
    return EntityId;
}

void Avdecc_EntityId2Mac(UINT64 EntityId, UINT8 *Mac)
{
    UINT64 Tmp = EntityId;//(EntityId >> 16U);
    Mac[5] = (UINT8)((Tmp >> 56U) & (0x00000000000000FFU));
    Mac[4] = (UINT8)((Tmp >> 48U) & (0x00000000000000FFU));
    Mac[3] = (UINT8)((Tmp >> 40U) & (0x00000000000000FFU));
    Mac[2] = (UINT8)((Tmp >> 16U) & (0x00000000000000FFU));
    Mac[1] = (UINT8)((Tmp >> 8U) & (0x00000000000000FFU));
    Mac[0] = (UINT8)(Tmp & (0x00000000000000FFU));
}

UINT16 Avdecc_GetUniqueId(UINT64 EntityId)
{
    return ((UINT16)((EntityId >> 24U) & (0x00000000000000FFU)));
}

UINT8 Avdecc_DoTerminate(void)
{
    UINT8 Rval = FALSE;
#if 0 // TODO
    if (AvdeccMgr->doTerminate != 0u) {
        v = TRUE;
    }
#endif
    return Rval;
}

UINT8 Avdecc_IsVlan(void)
{
    UINT8 v = FALSE;
#if 0 // TODO
    if (AvdeccMgr->IsSupportVPN != 0u) {
        v = TRUE;
    }
#endif
    return v;
}

