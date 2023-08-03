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

#ifndef AMBA_CSL_I2S_H
#define AMBA_CSL_I2S_H

#ifndef AMBA_I2S_DEF_H
#include "AmbaI2S_Def.h"
#endif

#include "AmbaReg_I2S.h"

/*
 * Macro Definitions
 */
static inline void AmbaCSL_I2sConfigMode(AMBA_I2S_REG_s *pI2sReg, UINT32 d)
{
    pI2sReg->Mode = (d);
}
static inline void AmbaCSL_I2sSetWordLength(AMBA_I2S_REG_s *pI2sReg, UINT32 d)
{
    pI2sReg->Wlen = (d) - 1U;
}
static inline void AmbaCSL_I2sSetWordPosition(AMBA_I2S_REG_s *pI2sReg, UINT32 d)
{
    pI2sReg->Wpos = (d);
}
static inline void AmbaCSL_I2sConfigSlot(AMBA_I2S_REG_s *pI2sReg, UINT32 d)
{
    pI2sReg->Slot = (d);
}
static inline void AmbaCSL_I2sSetTxFifoTh(AMBA_I2S_REG_s *pI2sReg, UINT32 d)
{
    pI2sReg->TxFifoLth.Ft = (UINT8)((d) & 0x7FU);
}
static inline void AmbaCSL_I2sSetRxFifoTh(AMBA_I2S_REG_s *pI2sReg, UINT32 d)
{
    pI2sReg->RxFifoGth.Ft = (UINT8)((d) & 0x7FU);
}

static inline void AmbaCSL_I2sSetRxOrder(AMBA_I2S_REG_s *pI2sReg, UINT32 d)
{
    pI2sReg->RxCtrl.Order = (UINT8)((d) & 0x01U);
}
static inline void AmbaCSL_I2sSetRxLoopback(AMBA_I2S_REG_s *pI2sReg, UINT32 d)
{
    pI2sReg->RxCtrl.Loopback = (UINT8)((d) & 0x01U);
}
static inline void AmbaCSL_I2sSetRxWsInv(AMBA_I2S_REG_s *pI2sReg, UINT32 d)
{
    pI2sReg->RxCtrl.RxWsInv = (UINT8)((d) & 0x01U);
}
static inline void AmbaCSL_I2sSetRxWsMst(AMBA_I2S_REG_s *pI2sReg, UINT32 d)
{
    pI2sReg->RxCtrl.RxWsMst = (UINT8)(d & 0x01U);
}

static inline void AmbaCSL_I2sSetTxMute(AMBA_I2S_REG_s *pI2sReg, UINT32 d)
{
    pI2sReg->TxCtrl.Mute = (UINT8)((d) & 0x01U);
}
static inline void AmbaCSL_I2sSetTxUnison(AMBA_I2S_REG_s *pI2sReg, UINT32 d)
{
    pI2sReg->TxCtrl.Unison = (UINT8)((d) & 0x01U);
}
static inline void AmbaCSL_I2sSetTxOrder(AMBA_I2S_REG_s *pI2sReg, UINT32 d)
{
    pI2sReg->TxCtrl.Order = (UINT8)((d) & 0x01U);
}
static inline void AmbaCSL_I2sSetTxLoopback(AMBA_I2S_REG_s *pI2sReg, UINT32 d)
{
    pI2sReg->TxCtrl.Loopback = (UINT8)((d) & 0x01U);
}
static inline void AmbaCSL_I2sSetTxWsInv(AMBA_I2S_REG_s *pI2sReg, UINT32 d)
{
    pI2sReg->TxCtrl.TxWsInv = (UINT8)((d) & 0x01U);
}
static inline void AmbaCSL_I2sSetTxWsMst(AMBA_I2S_REG_s *pI2sReg, UINT32 d)
{
    pI2sReg->TxCtrl.TxWsMst = (UINT8)(d & 0x01U);
}

static inline void AmbaCSL_I2sSetNumChannels(AMBA_I2S_REG_s *pI2sReg, UINT32 d)
{
    pI2sReg->ChannelSelect.ChannelSelect = (UINT8)((d) & 0x03U);
}

static inline void AmbaCSL_I2sSetWsBclkOutput(AMBA_I2S_REG_s *pI2sReg, UINT32 d)
{
    pI2sReg->ClkCtrl.Soe = (UINT8)((d) & 0x01U);
    pI2sReg->ClkCtrl.Woe = (UINT8)((d) & 0x01U);
}
static inline void AmbaCSL_I2sSetClkRsp(AMBA_I2S_REG_s *pI2sReg, UINT32 d)
{
    pI2sReg->ClkCtrl.Rsp = (UINT8)((d) & 0x01U);
}
static inline void AmbaCSL_I2sSetClkTsp(AMBA_I2S_REG_s *pI2sReg, UINT32 d)
{
    pI2sReg->ClkCtrl.Tsp = (UINT8)((d) & 0x01U);
}

#if defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52) || defined(CONFIG_SOC_CV28) || defined(CONFIG_SOC_H32)
static inline void AmbaCSL_I2sFifoSetSource(AMBA_I2S_REG_s *pI2sReg, UINT32 d)
{
    pI2sReg->Init.enSrc = (UINT8)((d) & 0x01U);
}
#endif
static inline void AmbaCSL_I2sTxFifoReset(AMBA_I2S_REG_s *pI2sReg)
{
    pI2sReg->Init.Txfrst = 1U;
}
static inline void AmbaCSL_I2sRxFifoReset(AMBA_I2S_REG_s *pI2sReg)
{
    pI2sReg->Init.Rxfrst = 1U;
}
static inline void AmbaCSL_I2sFifoReset(AMBA_I2S_REG_s *pI2sReg)
{
    pI2sReg->Init.Frst = 1U;
}

static inline void AmbaCSL_I2sSetTxEnable(AMBA_I2S_REG_s *pI2sReg, UINT32 d)
{
    pI2sReg->Init.Te = (UINT8)((d) & 0x01U);
}
static inline void AmbaCSL_I2sSetRxEnable(AMBA_I2S_REG_s *pI2sReg, UINT32 d)
{
    pI2sReg->Init.Re = (UINT8)((d) & 0x01U);
}

static inline UINT32 AmbaCSL_I2sGetTxFifoStatus(const AMBA_I2S_REG_s *pI2sReg)
{
    return pI2sReg->TxFifoFlag.Fe;
}
static inline void AmbaCSL_I2sWriteTxFifo(AMBA_I2S_REG_s *pI2sReg, UINT32 d)
{
    pI2sReg->TxLeftData = d;
}

static inline void AmbaCSL_I2sSetRxEchoCtrl(AMBA_I2S_REG_s *pI2sReg, UINT32 d)
{
    pI2sReg->RxEcho.Echo = (UINT8)((d) & 0x01U);
}
static inline void AmbaCSL_I2sSet24BitMuxCtrl(AMBA_I2S_REG_s *pI2sReg, UINT32 d)
{
    pI2sReg->BitmuxMode24.Multi24En = (UINT8)((d) & 0x01U);
}

static inline void AmbaCSL_I2sSetTxShiftCtrl(AMBA_I2S_REG_s *pI2sReg, UINT32 d)
{
    pI2sReg->Shift.TxShiftEn = (UINT8)((d) & 0x01U);
}
static inline void AmbaCSL_I2sSetRxShiftCtrl(AMBA_I2S_REG_s *pI2sReg, UINT32 d)
{
    pI2sReg->Shift.RxShiftEn = (UINT8)((d) & 0x01U);
}
static inline void AmbaCSL_I2sSetTxShift16bCtrl(AMBA_I2S_REG_s *pI2sReg, UINT32 d)
{
    pI2sReg->Shift.TxShift16bEn = (UINT8)((d) & 0x01U);
}
static inline void AmbaCSL_I2sSetRxShift16bCtrl(AMBA_I2S_REG_s *pI2sReg, UINT32 d)
{
    pI2sReg->Shift.RxShift16bEn = (UINT8)((d) & 0x01U);
}

static inline UINT32 AmbaCSL_I2sGetMode(const AMBA_I2S_REG_s *pI2sReg)
{
    return pI2sReg->Mode;
}
static inline UINT32 AmbaCSL_I2sGetWordLength(const AMBA_I2S_REG_s *pI2sReg)
{
    return pI2sReg->Wlen;
}
static inline UINT32 AmbaCSL_I2sGetWordPosition(const AMBA_I2S_REG_s *pI2sReg)
{
    return pI2sReg->Wpos;
}
static inline UINT32 AmbaCSL_I2sGetTxWsInv(const AMBA_I2S_REG_s *pI2sReg)
{
    return pI2sReg->TxCtrl.TxWsInv;
}
static inline UINT32 AmbaCSL_I2sGetTxWsMst(const AMBA_I2S_REG_s *pI2sReg)
{
    return pI2sReg->TxCtrl.TxWsMst;
}
static inline UINT32 AmbaCSL_I2sGetTxOrder(const AMBA_I2S_REG_s *pI2sReg)
{
    return pI2sReg->TxCtrl.Order;
}
static inline UINT32 AmbaCSL_I2sGetClockPolarity(const AMBA_I2S_REG_s *pI2sReg)
{
    return pI2sReg->ClkCtrl.Tsp;
}

static inline void AmbaCSL_I2sSetWsEnable(AMBA_I2S_REG_s *pI2sReg, UINT32 d)
{
    pI2sReg->WsCtrl.WsEn = (UINT8)((d) & 0x01U);
}

static inline const volatile UINT32 * AmbaCSL_I2sGetTxDmaAddress(const AMBA_I2S_REG_s *pI2sReg)
{
    return &pI2sReg->TxLeftDataDma;
}
static inline const volatile UINT32 * AmbaCSL_I2sGetRxDmaAddress(const AMBA_I2S_REG_s *pI2sReg)
{
    return &pI2sReg->RxDataDma;
}

/*
 * Defined in AmbaCSL_I2S.c
 */
void AmbaCSL_I2sSetTxMono(AMBA_I2S_REG_s *pI2sReg, UINT32 Mono);

#endif /* AMBA_CSL_I2S_H */
