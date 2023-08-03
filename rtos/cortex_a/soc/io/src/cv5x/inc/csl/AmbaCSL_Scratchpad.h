/**
 *  @file AmbaCSL_Scratchpad.h
 *
 *  @copyright Copyright (c) 2021 Ambarella, Inc.
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

static inline void AmbaCSL_SetVsdelaySrcMsync0(void)
{
    pAmbaScratchpadNS_Reg->VsDelayConfig.HsyncVsyncSrc = 0U;
}
static inline void AmbaCSL_SetVsdelaySrcMsync1(void)
{
    pAmbaScratchpadNS_Reg->VsDelayConfig.HsyncVsyncSrc = 1U;
}
static inline void AmbaCSL_SetVsdelayVsSrcExt(void)
{
    pAmbaScratchpadNS_Reg->VsDelayConfig.SelSourceVsync = 1U;
}
static inline void AmbaCSL_SetVsdelayHsSrcExt(void)
{
    pAmbaScratchpadNS_Reg->VsDelayConfig.SelSourceHsync = 1U;
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
static inline void AmbaCSL_SetVsdelayVsync2WidthSrc(UINT32 Val)
{
    pAmbaScratchpadNS_Reg->VsDelayConfig.SelWidthSrcVsync2 = (UINT8)Val;
}
static inline void AmbaCSL_SetVsdelayVsync3WidthSrc(UINT32 Val)
{
    pAmbaScratchpadNS_Reg->VsDelayConfig.SelWidthSrcVsync3 = (UINT8)Val;
}

static inline void AmbaCSL_SetVsdelayVsync0Dly(UINT32 Val)
{
    pAmbaScratchpadNS_Reg->VsDelayVPulse0 = (UINT32)Val;
}
static inline void AmbaCSL_SetVsdelayVsync1Dly(UINT32 Val)
{
    pAmbaScratchpadNS_Reg->VsDelayVPulse1 = (UINT32)Val;
}
static inline void AmbaCSL_SetVsdelayVsync2Dly(UINT32 Val)
{
    pAmbaScratchpadNS_Reg->VsDelayVPulse2 = (UINT32)Val;
}
static inline void AmbaCSL_SetVsdelayVsync3Dly(UINT32 Val)
{
    pAmbaScratchpadNS_Reg->VsDelayVPulse3 = (UINT32)Val;
}

static inline void AmbaCSL_SetVsdelayVsyncPolInput(UINT32 Val)
{
    pAmbaScratchpadNS_Reg->VsDelayConfig.VsyncPolarityInput = (UINT8)Val;
}
static inline void AmbaCSL_SetVsdelayHsyncPolInput(UINT32 Val)
{
    pAmbaScratchpadNS_Reg->VsDelayConfig.HsyncPolarityInput = (UINT8)Val;
}
static inline void AmbaCSL_SetVsdelayVsync0PolOutput(UINT32 Val)
{
    pAmbaScratchpadNS_Reg->VsDelayConfig.Vsync0PolarityOutput = (UINT8)Val;
}
static inline void AmbaCSL_SetVsdelayVsync1PolOutput(UINT32 Val)
{
    pAmbaScratchpadNS_Reg->VsDelayConfig.Vsync1PolarityOutput = (UINT8)Val;
}
static inline void AmbaCSL_SetVsdelayVsync2PolOutput(UINT32 Val)
{
    pAmbaScratchpadNS_Reg->VsDelayConfig.Vsync2PolarityOutput = (UINT8)Val;
}
static inline void AmbaCSL_SetVsdelayVsync3PolOutput(UINT32 Val)
{
    pAmbaScratchpadNS_Reg->VsDelayConfig.Vsync3PolarityOutput = (UINT8)Val;
}
static inline void AmbaCSL_SetVsdelayHsyncPolOutput(UINT32 Val)
{
    pAmbaScratchpadNS_Reg->VsDelayConfig.HsyncPolarityOutput = (UINT8)Val;
}

static inline void AmbaCSL_SetVsdelayVsync0DlyClk(UINT32 Val)
{
    pAmbaScratchpadNS_Reg->VsDelayVClk0.Vsync0DelayClk = (UINT16)Val;
}
static inline void AmbaCSL_SetVsdelayVsync1DlyClk(UINT32 Val)
{
    pAmbaScratchpadNS_Reg->VsDelayVClk0.Vsync1DelayClk = (UINT16)Val;
}
static inline void AmbaCSL_SetVsdelayVsync2DlyClk(UINT32 Val)
{
    pAmbaScratchpadNS_Reg->VsDelayVClk1.Vsync2DelayClk = (UINT16)Val;
}
static inline void AmbaCSL_SetVsdelayVsync3DlyClk(UINT32 Val)
{
    pAmbaScratchpadNS_Reg->VsDelayVClk1.Vsync3DelayClk = (UINT16)Val;
}
static inline void AmbaCSL_SetVsdelayHsyncDlyClk(UINT32 Val)
{
    pAmbaScratchpadNS_Reg->VsDelayHClk.HsyncDelayClk = (UINT16)Val;
}

static inline void AmbaCSL_SetVsdelayVsync0Width(UINT32 Val)
{
    pAmbaScratchpadNS_Reg->VsDelaySelWidthV0.Vsync0Width = (UINT16)Val;
}
static inline void AmbaCSL_SetVsdelayVsync1Width(UINT32 Val)
{
    pAmbaScratchpadNS_Reg->VsDelaySelWidthV0.Vsync1Width = (UINT16)Val;
}
static inline void AmbaCSL_SetVsdelayVsync2Width(UINT32 Val)
{
    pAmbaScratchpadNS_Reg->VsDelaySelWidthV1.Vsync2Width = (UINT16)Val;
}
static inline void AmbaCSL_SetVsdelayVsync3Width(UINT32 Val)
{
    pAmbaScratchpadNS_Reg->VsDelaySelWidthV1.Vsync3Width = (UINT16)Val;
}
static inline void AmbaCSL_SetVsdelayHsyncWidth(UINT32 Val)
{
    pAmbaScratchpadNS_Reg->VsDelaySelWidthH.HsyncWidth = (UINT16)Val;
}

/* CLK_AU Pad disable */
static inline void AmbaCSL_DisableClkAuPad(UINT32 Val)
{
    pAmbaScratchpadNS_Reg->DisableClkAuPad = Val;
}

/*
 * Defined in AmbaCSL_SKPD.c
 */
UINT32 AmbaCSL_SkpdGetScratchpadData(UINT32 Index);
void AmbaCSL_SkpdSetScratchpadData(UINT32 Index, UINT32 Data);

UINT32 AmbaCSL_SetVsdelaySrc(UINT32 VsdelaySrc);
void AmbaCSL_SetVsdelayWidth(const UINT32 HsPulseWidth, const UINT32 *VsPulseWidth);

#endif /* AMBA_CSL_SCRATCHPAD_H */
