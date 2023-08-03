/**
 *  @file AmbaCSL_Scratchpad.h
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
 *  @details Definitions & Constants for Scratchpad APIs
 *
 */

#ifndef AMBA_CSL_SCRATCHPAD_H
#define AMBA_CSL_SCRATCHPAD_H

#ifndef AMBA_REG_RCT_H
#include "AmbaReg_RCT.h"
#endif

#ifndef AMBA_REG_SCRATCHPAD_S_H
#include "AmbaReg_ScratchpadS.h"
#endif

#ifndef AMBA_REG_SCRATCHPAD_NS_H
#include "AmbaReg_ScratchpadNS.h"
#endif

/*
 * Inline Function Definitions
 */
static inline void AmbaCSL_SetEnet2ndRefClk(void)
{
    pAmbaScratchpadNS_Reg->AhbCtrl.Enet2ndRefClkSelect = 1U;
}

/* Scratchpad Data Register (SKPD)  */
#define NUM_SKPD_REG    (4U)

static inline void AmbaCSL_ScratchpadJtagOn(void)
{
    pAmbaScratchpadS_Reg->JtagCtrl = 1U;
}

static inline void AmbaCSL_SpdSafetyDisable(void)
{
    pAmbaScratchpadNS_Reg->SaftyEnable = 0x3ffff;
}

static inline void AmbaCSL_ScratchpadSWriteUnLock(void)
{
    pAmbaScratchpadS_Reg->LockCtrl.WriteLock = 0U;
    pAmbaScratchpadS_Reg->LockCtrl.SpSecLock = 0U;
}
static inline void AmbaCSL_ScratchpadNSWriteUnLock(void)
{
    pAmbaScratchpadNS_Reg->LockCtrl.WriteLock = 0U;
    pAmbaScratchpadNS_Reg->LockCtrl.SpSecLock = 0U;
}

/* Delayed Vsync Generator */
static inline void AmbaCSL_SetVsdelayEnable(void)
{
    pAmbaScratchpadNS_Reg->VsDelayConfig.VsdelayEnable = 1U;
}
static inline void AmbaCSL_SetVsdelayUpdateConfig(UINT32 Val)
{
    pAmbaScratchpadNS_Reg->VsDelayConfig.UpdateConfig = (UINT8)Val;
}
static inline void AmbaCSL_SetVsdelayDisable(void)
{
    pAmbaScratchpadNS_Reg->VsDelayConfig.VsdelayEnable = 0U;
}

static inline void AmbaCSL_SetVsdelayVsyncVin(void)
{
    pAmbaScratchpadNS_Reg->VsDelayConfig.HsyncVsyncSrc = 0U;
}
static inline void AmbaCSL_SetVsdelayVsyncPip(void)
{
    pAmbaScratchpadNS_Reg->VsDelayConfig.HsyncVsyncSrc = 1U;
}
static inline void AmbaCSL_SetVsdelayVsyncExt(void)
{
    pAmbaScratchpadNS_Reg->VsDelayConfig.SelSourceVsync = 1U;
}
static inline void AmbaCSL_SetVsdelayHsyncWidthSrc(UINT32 Val)
{
    pAmbaScratchpadNS_Reg->VsDelayConfig.SelWidthSrcHsync = (UINT8)Val;
}
static inline void AmbaCSL_SetVsdelayVsync0WidthSrc(UINT32 Val)
{
    pAmbaScratchpadNS_Reg->VsDelayConfig.SelWidthSrcVsync0 = (UINT8)Val;
}
static inline void AmbaCSL_SetVsdelayVsync1WidthSrc(UINT32 Val)
{
    pAmbaScratchpadNS_Reg->VsDelayConfig.SelWidthSrcVsync1 = (UINT8)Val;
}

static inline void AmbaCSL_SetVsdelayVsync0Dly(UINT32 Val)
{
    pAmbaScratchpadNS_Reg->VsDelayVs0Pulse = (UINT32)Val;
}
static inline void AmbaCSL_SetVsdelayVsync1Dly(UINT32 Val)
{
    pAmbaScratchpadNS_Reg->VsDelayVs1Pulse = (UINT32)Val;
}

static inline void AmbaCSL_SetVsdelayHsyncPolInput(UINT32 Val)
{
    pAmbaScratchpadNS_Reg->VsDelayConfig.HsyncPolarityInput = (UINT8)Val;
}
static inline void AmbaCSL_SetVsdelayVsyncPolInput(UINT32 Val)
{
    pAmbaScratchpadNS_Reg->VsDelayConfig.VsyncPolarityInput = (UINT8)Val;
}
static inline void AmbaCSL_SetVsdelayHsyncPolOutput(UINT32 Val)
{
    pAmbaScratchpadNS_Reg->VsDelayConfig.HsyncPolarityOutput = (UINT8)Val;
}
static inline void AmbaCSL_SetVsdelayVsync0PolOutput(UINT32 Val)
{
    pAmbaScratchpadNS_Reg->VsDelayConfig.Vsync0PolarityOutput = (UINT8)Val;
}
static inline void AmbaCSL_SetVsdelayVsync1PolOutput(UINT32 Val)
{
    pAmbaScratchpadNS_Reg->VsDelayConfig.Vsync1PolarityOutput = (UINT8)Val;
}

static inline void AmbaCSL_SetVsdelayHsyncDlyClk(UINT32 Val)
{
    pAmbaScratchpadNS_Reg->VsDelayHClock.HsyncDelayClk = (UINT16)Val;
}
static inline void AmbaCSL_SetVsdelayVsync0DlyClk(UINT32 Val)
{
    pAmbaScratchpadNS_Reg->VsDelayVClock.Vsync0DelayClk = (UINT16)Val;
}
static inline void AmbaCSL_SetVsdelayVsync1DlyClk(UINT32 Val)
{
    pAmbaScratchpadNS_Reg->VsDelayVClock.Vsync1DelayClk = (UINT16)Val;
}
static inline void AmbaCSL_SetVsdelayHsyncWidth(UINT32 Val)
{
    pAmbaScratchpadNS_Reg->VsDelayWidthH.HsyncWidth = (UINT16)Val;
}
static inline void AmbaCSL_SetVsdelayVsync0Width(UINT32 Val)
{
    pAmbaScratchpadNS_Reg->VsDelayWidthV.Vsync0Width = (UINT16)Val;
}
static inline void AmbaCSL_SetVsdelayVsync1Width(UINT32 Val)
{
    pAmbaScratchpadNS_Reg->VsDelayWidthV.Vsync1Width = (UINT16)Val;
}

/*
 * Defined in AmbaCSL_SKPD.c
 */
UINT32 AmbaCSL_SkpdGetScratchpadData(UINT32 Index);
void AmbaCSL_SkpdSetScratchpadData(UINT32 Index, UINT32 Data);
UINT32 AmbaCSL_SetVsdelaySrc(UINT32 VsdelaySrc);
void AmbaCSL_SetVsdelayWidth(const UINT32 HsPulseWidth, const UINT32 *VsPulseWidth);

#endif /* AMBA_CSL_SCRATCHPAD_H */
