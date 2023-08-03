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

#ifndef AMBA_CSL_GPIO_H
#define AMBA_CSL_GPIO_H

#ifndef AMBA_GPIO_DEF_H
#include "AmbaGPIO_Def.h"
#endif

#ifndef AMBA_REG_RCT_H
#include "AmbaReg_RCT.h"
#endif

#if defined(CONFIG_SOC_CV2)
#ifndef AMBA_REG_MISC_H
#include "AmbaReg_MISC.h"
#endif
#elif defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)
#ifndef AMBA_REG_SCRATCHPAD_S_H
#include "AmbaReg_ScratchpadS.h"
#endif
#else
#ifndef AMBA_REG_SCRATCHPAD_NS_H
#include "AmbaReg_ScratchpadNS.h"
#endif
#endif

#include "AmbaReg_GPIO.h"
#include "AmbaReg_IoMux.h"

/*
 * Inline Function Definitions
 */
static inline UINT32 AmbaCSL_GpioGetPinStateEnable(UINT32 PinGroup, UINT32 BitPos)
{
    return ((pAmbaGPIO_Reg[PinGroup]->PinMask >> (BitPos)) & (UINT32)1U);
}
static inline UINT32 AmbaCSL_GpioGetPinLevel(UINT32 PinGroup, UINT32 BitPos)
{
    return ((pAmbaGPIO_Reg[PinGroup]->PinLevel >> (BitPos)) & (UINT32)1U);
}
static inline UINT32 AmbaCSL_GpioGetPinIsOutput(UINT32 PinGroup, UINT32 BitPos)
{
    return ((pAmbaGPIO_Reg[PinGroup]->PinDirection >> (BitPos)) & (UINT32)1U);
}

static inline void AmbaCSL_GpioSetOutput(UINT32 PinGroup, UINT32 BitPos)
{
    pAmbaGPIO_Reg[PinGroup]->PinDirection |= ((UINT32)1U << (BitPos));
}
static inline void AmbaCSL_GpioSetInput(UINT32 PinGroup, UINT32 BitPos)
{
    pAmbaGPIO_Reg[PinGroup]->PinDirection &= ~((UINT32)1U << (BitPos));
}
static inline void AmbaCSL_GpioSetEnable(UINT32 PinGroup, UINT32 BitPos)
{
    pAmbaGPIO_Reg[PinGroup]->PinMask |= ((UINT32)1U << (BitPos));
}
static inline void AmbaCSL_GpioSetDisable(UINT32 PinGroup, UINT32 BitPos)
{
    pAmbaGPIO_Reg[PinGroup]->PinMask &= ~((UINT32)1U << (BitPos));
}
static inline void AmbaCSL_GpioSetHighLv(UINT32 PinGroup, UINT32 BitPos)
{
    pAmbaGPIO_Reg[PinGroup]->PinLevel |= ((UINT32)1U << (BitPos));
}
static inline void AmbaCSL_GpioSetLowLv(UINT32 PinGroup, UINT32 BitPos)
{
    pAmbaGPIO_Reg[PinGroup]->PinLevel &= ~((UINT32)1U << (BitPos));
}

static inline UINT32 AmbaCSL_GpioGetIntGrpEnable(UINT32 PinGroup)
{
    return pAmbaGPIO_Reg[PinGroup]->IntEnable;
}
static inline UINT32 AmbaCSL_GpioGetIntEnable(UINT32 PinGroup, UINT32 BitPos)
{
    return (pAmbaGPIO_Reg[PinGroup]->IntEnable & ((UINT32)1U << (BitPos)));
}
static inline UINT32 AmbaCSL_GpioGetIntState(UINT32 PinGroup, UINT32 BitPos)
{
    return (pAmbaGPIO_Reg[PinGroup]->IntStatus & ((UINT32)1U << (BitPos)));
}
static inline UINT32 AmbaCSL_GpioGetIntType(UINT32 PinGroup, UINT32 BitPos)
{
    return (pAmbaGPIO_Reg[PinGroup]->IntTrigType0 & ((UINT32)1U << (BitPos)));
}
static inline UINT32 AmbaCSL_GpioGetIntRate(UINT32 PinGroup, UINT32 BitPos)
{
    return (pAmbaGPIO_Reg[PinGroup]->IntTrigType1 & ((UINT32)1U << (BitPos)));
}
static inline UINT32 AmbaCSL_GpioGetIntMode(UINT32 PinGroup, UINT32 BitPos)
{
    return (pAmbaGPIO_Reg[PinGroup]->IntTrigType2 & ((UINT32)1U << (BitPos)));
}

static inline void AmbaCSL_GpioEnableInt(UINT32 PinGroup, UINT32 BitPos)
{
    pAmbaGPIO_Reg[PinGroup]->IntEnable |= ((UINT32)1U << (BitPos));
}
static inline void AmbaCSL_GpioDisableInt(UINT32 PinGroup, UINT32 BitPos)
{
    pAmbaGPIO_Reg[PinGroup]->IntEnable &= ~((UINT32)1U << (BitPos));
}
static inline void AmbaCSL_GpioWriteEndOfInt(UINT32 PinGroup, UINT32 BitPos)
{
    pAmbaGPIO_Reg[PinGroup]->IntClear |= ((UINT32)1U << (BitPos));
}
static inline void AmbaCSL_GpioSetIntLevelTrigger(UINT32 PinGroup, UINT32 BitPos)
{
    pAmbaGPIO_Reg[PinGroup]->IntTrigType0 |= ((UINT32)1U << (BitPos));
}
static inline void AmbaCSL_GpioSetIntEdgeTrigger(UINT32 PinGroup, UINT32 BitPos)
{
    pAmbaGPIO_Reg[PinGroup]->IntTrigType0 &= ~((UINT32)1U << (BitPos));
}
static inline void AmbaCSL_GpioSetIntAnyEdge(UINT32 PinGroup, UINT32 BitPos)
{
    pAmbaGPIO_Reg[PinGroup]->IntTrigType1 |= ((UINT32)1U << (BitPos));
}
static inline void AmbaCSL_GpioSetIntByConfig(UINT32 PinGroup, UINT32 BitPos)
{
    pAmbaGPIO_Reg[PinGroup]->IntTrigType1 &= ~((UINT32)1U << (BitPos));
}
static inline void AmbaCSL_GpioSetIntOnRisingOrHigh(UINT32 PinGroup, UINT32 BitPos)
{
    pAmbaGPIO_Reg[PinGroup]->IntTrigType2 |= ((UINT32)1U << (BitPos));
}
static inline void AmbaCSL_GpioSetIntOnFallingOrLow(UINT32 PinGroup, UINT32 BitPos)
{
    pAmbaGPIO_Reg[PinGroup]->IntTrigType2 &= ~((UINT32)1U << (BitPos));
}

static inline void AmbaCSL_GpioEnablePinGrp(UINT32 PinGroup)
{
    pAmbaGPIO_Reg[PinGroup]->Enable.Enable = (0x1U);
}
static inline void AmbaCSL_GpioClearPinGrpInt(UINT32 PinGroup)
{
    pAmbaGPIO_Reg[PinGroup]->IntClear = (0xffffffffU);
}

static inline UINT32 AmbaCSL_GpioGetPinGrpMask(UINT32 PinGroup)
{
    return pAmbaGPIO_Reg[PinGroup]->PinMask;
}

static inline void AmbaCSL_GpioSetPinGrpAltFunc(UINT32 PinGroup, UINT32 Val)
{
    pAmbaGPIO_Reg[PinGroup]->PinAltFuncSel = Val;
}
static inline void AmbaCSL_GpioSetPinGrpDirection(UINT32 PinGroup, UINT32 Val)
{
    pAmbaGPIO_Reg[PinGroup]->PinDirection = Val;
}
static inline void AmbaCSL_GpioSetPinGrpMask(UINT32 PinGroup, UINT32 Val)
{
    pAmbaGPIO_Reg[PinGroup]->PinMask = Val;
}
static inline void AmbaCSL_GpioSetPinGrpState(UINT32 PinGroup, UINT32 Val)
{
    pAmbaGPIO_Reg[PinGroup]->PinLevel = Val;
}

#if defined(CONFIG_SOC_CV22) || defined(CONFIG_SOC_CV2) || defined(CONFIG_SOC_CV25)
static inline void AmbaCSL_SetDebounceGpio0(UINT8 Enable)
{
    pAmbaRCT_Reg->GpioDebounceSelect.EnableGpio0Debounce = Enable;
}
static inline void AmbaCSL_SetDebounceGpio1(UINT8 Enable)
{
    pAmbaRCT_Reg->GpioDebounceSelect.EnableGpio1Debounce = Enable;
}
#endif

/*
 * Defined in AmbaCSL_GPIO.c
 */
void AmbaCSL_GpioSetPinGrpFunc(UINT32 PinGroup, const UINT32 *pPinGroupFunc);
void AmbaCSL_GpioSetPinFunc(UINT32 PinGroup, UINT32 BitPos, UINT32 PinFunc);
UINT32 AmbaCSL_GpioGetPinFunc(UINT32 PinGroup, UINT32 BitPos);

void AmbaCSL_GpioSetPinGrpPullCtrl(UINT32 PinGroup, UINT32 PullEnable, UINT32 PullSelect);
void AmbaCSL_GpioSetPinPullCtrl(UINT32 PinGroup, UINT32 BitPos, UINT32 PullCtrl);
UINT32 AmbaCSL_GpioGetPinPullCtrl(UINT32 PinGroup, UINT32 BitPos);

void AmbaCSL_GpioSetPinGrpDriveStr(UINT32 PinGroup, const UINT32 *pPinGroupDriveStrength);
void AmbaCSL_GpioSetPinDriveStr(UINT32 PinGroup, UINT32 BitPos, UINT32 DriveStrength);
UINT32 AmbaCSL_GpioGetPinDriveStr(UINT32 PinGroup, UINT32 BitPos);

#endif /* AMBA_CSL_GPIO_H */
