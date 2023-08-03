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

#include "AmbaTypes.h"

#include "AmbaCSL_GPIO.h"

/**
 *  AmbaCSL_GpioGetPinFunc - Get the function of a GPIO pin
 *  @param[in] PinGroup GPIO group ID
 *  @param[in] BitPos GPIO pin offset within group
 *  @return Pin function selection
 */
UINT32 AmbaCSL_GpioGetPinFunc(UINT32 PinGroup, UINT32 BitPos)
{
    const AMBA_IO_MUX_PIN_FUNC_REG_s *pGpioGroup = &pAmbaIOMUX_Reg->PinGroup[PinGroup];
    UINT32 PinFunc;
    UINT32 RetVal = 0x0U;

    PinFunc = ((pGpioGroup->FuncSelect0 >> BitPos) & 0x1U);
    PinFunc |= (((pGpioGroup->FuncSelect1 >> BitPos) & 0x1U) << 1U);
    PinFunc |= (((pGpioGroup->FuncSelect2 >> BitPos) & 0x1U) << 2U);

    if (PinFunc != 0U) {
        RetVal = (GPIO_FUNC_ALT1 + PinFunc - 1U);  /* Alternate Function 1 to 7 */
    } else {
        if (PinGroup < AMBA_NUM_GPIO_GROUP) {
            if (AmbaCSL_GpioGetPinIsOutput(PinGroup, BitPos) != 0U) {
                RetVal = GPIO_FUNC_GPO;
            } else {
                RetVal = GPIO_FUNC_GPI;
            }
        }
    }

    return RetVal;
}

/**
 *  AmbaCSL_GpioSetPinFunc - Set the function of a GPIO pin
 *  @param[in] PinGroup GPIO group ID
 *  @param[in] BitPos GPIO pin offset within group
 *  @param[in] PinFunc GPI or GPO or alternative function
 */
void AmbaCSL_GpioSetPinFunc(UINT32 PinGroup, UINT32 BitPos, UINT32 PinFunc)
{
    AMBA_IO_MUX_PIN_FUNC_REG_s *pGpioGroup = &pAmbaIOMUX_Reg->PinGroup[PinGroup];
    UINT32 BitMask = ((UINT32)0x1U << BitPos);
    UINT32 IoMuxFunc;

    if (PinFunc == GPIO_FUNC_GPI) {
        IoMuxFunc = 0U;
        if (PinGroup < AMBA_NUM_GPIO_GROUP) {
            AmbaCSL_GpioSetInput(PinGroup, BitPos);
        }
    } else if (PinFunc == GPIO_FUNC_GPO) {
        IoMuxFunc = 0U;
        if (PinGroup < AMBA_NUM_GPIO_GROUP) {
            AmbaCSL_GpioSetOutput(PinGroup, BitPos);
        }
    } else {
        IoMuxFunc = (PinFunc + 1U) - GPIO_FUNC_ALT1;
    }

    if ((IoMuxFunc & 0x1U) != 0U) {
        pGpioGroup->FuncSelect0 |= BitMask;
    } else {
        pGpioGroup->FuncSelect0 &= ~BitMask;
    }

    if ((IoMuxFunc & 0x2U) != 0U) {
        pGpioGroup->FuncSelect1 |= BitMask;
    } else {
        pGpioGroup->FuncSelect1 &= ~BitMask;
    }

    if ((IoMuxFunc & 0x4U) != 0U) {
        pGpioGroup->FuncSelect2 |= BitMask;
    } else {
        pGpioGroup->FuncSelect2 &= ~BitMask;
    }

    pAmbaIOMUX_Reg->RegUpdate.Update = 1U;
    pAmbaIOMUX_Reg->RegUpdate.Update = 0U;
}

/**
 *  AmbaCSL_GpioSetPinGrpFunc - Set the altenative functions for a group of GPIO pins
 *  @param[in] PinGroup GPIO group ID
 *  @param[in] pPinFuncs Grouped pin function selection
 */
void AmbaCSL_GpioSetPinGrpFunc(UINT32 PinGroup, const UINT32 *pPinGroupFunc)
{
    AMBA_IO_MUX_PIN_FUNC_REG_s *pGpioGroup = &pAmbaIOMUX_Reg->PinGroup[PinGroup];

    pGpioGroup->FuncSelect0 = pPinGroupFunc[0];
    pGpioGroup->FuncSelect1 = pPinGroupFunc[1];
    pGpioGroup->FuncSelect2 = pPinGroupFunc[2];

    pAmbaIOMUX_Reg->RegUpdate.Update = 1U;
    pAmbaIOMUX_Reg->RegUpdate.Update = 0U;
}

#if defined(CONFIG_SOC_CV2)
/**
 *  AmbaCSL_GpioGetPinPullCtrl - Get the pull status of a GPIO pin
 *  @param[in] PinGroup GPIO group ID
 *  @param[in] BitPos GPIO pin offset within group
 *  @return Pin pull-up/pull-down status
 */
UINT32 AmbaCSL_GpioGetPinPullCtrl(UINT32 PinGroup, UINT32 BitPos)
{
    UINT32 PullEnable = 0U, PullSelect = 0U;
    UINT32 RetVal = GPIO_PULL_DISABLE;

    if (PinGroup == AMBA_GPIO_GROUP0) {
        PullEnable = pAmbaMISC_Reg->GpioPullEnable0;
        PullSelect = pAmbaMISC_Reg->GpioPullSelect0;
    } else if (PinGroup == AMBA_GPIO_GROUP1) {
        PullEnable = pAmbaMISC_Reg->GpioPullEnable1;
        PullSelect = pAmbaMISC_Reg->GpioPullSelect1;
    } else if (PinGroup == AMBA_GPIO_GROUP2) {
        PullEnable = pAmbaMISC_Reg->GpioPullEnable2;
        PullSelect = pAmbaMISC_Reg->GpioPullSelect2;
    } else if (PinGroup == AMBA_GPIO_GROUP3) {
        PullEnable = pAmbaMISC_Reg->GpioPullEnable3;
        PullSelect = pAmbaMISC_Reg->GpioPullSelect3;
    } else if (PinGroup == AMBA_GPIO_GROUP4) {
        PullEnable = pAmbaMISC_Reg->GpioPullEnable4;
        PullSelect = pAmbaMISC_Reg->GpioPullSelect4;
    } else if (PinGroup == AMBA_GPIO_GROUP5) {
        PullEnable = pAmbaMISC_Reg->GpioPullEnable5;
        PullSelect = pAmbaMISC_Reg->GpioPullSelect5;
    } else {
        /* Should not happen!! */
    }

    if ((PullEnable & ((UINT32)1U << (BitPos))) != 0U) {
        if ((PullSelect & ((UINT32)1U << (BitPos))) != 0U) {
            RetVal = GPIO_PULL_UP;
        } else {
            RetVal = GPIO_PULL_DOWN;
        }
    }

    return RetVal;
}
#else
/**
 *  AmbaCSL_GpioGetPinPullCtrl - Get the pull status of a GPIO pin
 *  @param[in] PinGroup GPIO group ID
 *  @param[in] BitPos GPIO pin offset within group
 *  @return Pin pull-up/pull-down status
 */
UINT32 AmbaCSL_GpioGetPinPullCtrl(UINT32 PinGroup, UINT32 BitPos)
{
#if defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)
    UINT32 PullEnable = pAmbaScratchpadS_Reg->GpioPullEnable[PinGroup];
    UINT32 PullSelect = pAmbaScratchpadS_Reg->GpioPullSelect[PinGroup];
#else
    UINT32 PullEnable = pAmbaScratchpadNS_Reg->GpioPullEnable[PinGroup];
    UINT32 PullSelect = pAmbaScratchpadNS_Reg->GpioPullSelect[PinGroup];
#endif
    UINT32 RetVal = GPIO_PULL_DISABLE;

    if ((PullEnable & ((UINT32)1U << (BitPos))) != 0U) {
        if ((PullSelect & ((UINT32)1U << (BitPos))) != 0U) {
            RetVal = GPIO_PULL_UP;
        } else {
            RetVal = GPIO_PULL_DOWN;
        }
    }

    return RetVal;
}
#endif

/**
 *  AmbaCSL_GpioSetPinPullCtrl - Set the pull up/down control for a GPIO pin
 *  @param[in] PinGroup GPIO group ID
 *  @param[in] BitPos GPIO pin offset within group
 *  @param[in] PullCtrl Pull up/down control
 */
void AmbaCSL_GpioSetPinPullCtrl(UINT32 PinGroup, UINT32 BitPos, UINT32 PullCtrl)
{
#if defined(CONFIG_SOC_CV2)
    UINT32 PullEnable = 0U, PullSelect = 0U;

    if (PinGroup == AMBA_GPIO_GROUP0) {
        PullEnable = pAmbaMISC_Reg->GpioPullEnable0;
        PullSelect = pAmbaMISC_Reg->GpioPullSelect0;
    } else if (PinGroup == AMBA_GPIO_GROUP1) {
        PullEnable = pAmbaMISC_Reg->GpioPullEnable1;
        PullSelect = pAmbaMISC_Reg->GpioPullSelect1;
    } else if (PinGroup == AMBA_GPIO_GROUP2) {
        PullEnable = pAmbaMISC_Reg->GpioPullEnable2;
        PullSelect = pAmbaMISC_Reg->GpioPullSelect2;
    } else if (PinGroup == AMBA_GPIO_GROUP3) {
        PullEnable = pAmbaMISC_Reg->GpioPullEnable3;
        PullSelect = pAmbaMISC_Reg->GpioPullSelect3;
    } else if (PinGroup == AMBA_GPIO_GROUP4) {
        PullEnable = pAmbaMISC_Reg->GpioPullEnable4;
        PullSelect = pAmbaMISC_Reg->GpioPullSelect4;
    } else if (PinGroup == AMBA_GPIO_GROUP5) {
        PullEnable = pAmbaMISC_Reg->GpioPullEnable5;
        PullSelect = pAmbaMISC_Reg->GpioPullSelect5;
    } else {
        /* Should not happen!! */
    }
#elif defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)
    UINT32 PullEnable = pAmbaScratchpadS_Reg->GpioPullEnable[PinGroup];
    UINT32 PullSelect = pAmbaScratchpadS_Reg->GpioPullSelect[PinGroup];
#else
    UINT32 PullEnable = pAmbaScratchpadNS_Reg->GpioPullEnable[PinGroup];
    UINT32 PullSelect = pAmbaScratchpadNS_Reg->GpioPullSelect[PinGroup];
#endif

    if (PullCtrl == GPIO_PULL_DISABLE) {
        PullEnable &= ~((UINT32)1U << (BitPos));
    } else {
        PullEnable |= ((UINT32)1U << (BitPos));

        if (PullCtrl == GPIO_PULL_DOWN) {
            PullSelect &= ~((UINT32)1U << (BitPos));
        } else {
            PullSelect |= ((UINT32)1U << (BitPos));
        }
    }

    AmbaCSL_GpioSetPinGrpPullCtrl(PinGroup, PullEnable, PullSelect);
}

/**
 *  AmbaCSL_GpioSetPinGrpPullCtrl - Set the pull up/down control for a group of GPIO pins
 *  @param[in] PinGroup GPIO group ID
 *  @param[in] PullEnable Enable pull up/down for group of pins
 *  @param[in] PullSelect Pull up/down selection for group of pins
 */
void AmbaCSL_GpioSetPinGrpPullCtrl(UINT32 PinGroup, UINT32 PullEnable, UINT32 PullSelect)
{
#if defined(CONFIG_SOC_CV2)
    if (PinGroup == AMBA_GPIO_GROUP0) {
        pAmbaMISC_Reg->GpioPullEnable0 = PullEnable;
        pAmbaMISC_Reg->GpioPullSelect0 = PullSelect;
    } else if (PinGroup == AMBA_GPIO_GROUP1) {
        pAmbaMISC_Reg->GpioPullEnable1 = PullEnable;
        pAmbaMISC_Reg->GpioPullSelect1 = PullSelect;
    } else if (PinGroup == AMBA_GPIO_GROUP2) {
        pAmbaMISC_Reg->GpioPullEnable2 = PullEnable;
        pAmbaMISC_Reg->GpioPullSelect2 = PullSelect;
    } else if (PinGroup == AMBA_GPIO_GROUP3) {
        pAmbaMISC_Reg->GpioPullEnable3 = PullEnable;
        pAmbaMISC_Reg->GpioPullSelect3 = PullSelect;
    } else if (PinGroup == AMBA_GPIO_GROUP4) {
        pAmbaMISC_Reg->GpioPullEnable4 = PullEnable;
        pAmbaMISC_Reg->GpioPullSelect4 = PullSelect;
    } else if (PinGroup == AMBA_GPIO_GROUP5) {
        pAmbaMISC_Reg->GpioPullEnable5 = PullEnable;
        pAmbaMISC_Reg->GpioPullSelect5 = PullSelect;
    } else {
        /* Should not happen!! */
    }
#elif defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)
    pAmbaScratchpadS_Reg->GpioPullEnable[PinGroup] = PullEnable;
    pAmbaScratchpadS_Reg->GpioPullSelect[PinGroup] = PullSelect;
#else
    pAmbaScratchpadNS_Reg->GpioPullEnable[PinGroup] = PullEnable;
    pAmbaScratchpadNS_Reg->GpioPullSelect[PinGroup] = PullSelect;
#endif
}

/**
 *  AmbaCSL_GpioGetPinDriveStr - Get the driving strength of a GPIO pin
 *  @param[in] PinGroup GPIO group ID
 *  @param[in] BitPos GPIO pin offset within group
 *  @return Pin driving strength selection
 */
UINT32 AmbaCSL_GpioGetPinDriveStr(UINT32 PinGroup, UINT32 BitPos)
{
    UINT32 RegVal[2];
    UINT32 RetVal;

    if (PinGroup == AMBA_GPIO_GROUP0) {
        RegVal[0] = pAmbaRCT_Reg->Gpio0DriveStrength[0];
        RegVal[1] = pAmbaRCT_Reg->Gpio0DriveStrength[1];
    } else if (PinGroup == AMBA_GPIO_GROUP1) {
        RegVal[0] = pAmbaRCT_Reg->Gpio1DriveStrength[0];
        RegVal[1] = pAmbaRCT_Reg->Gpio1DriveStrength[1];
    } else if (PinGroup == AMBA_GPIO_GROUP2) {
        RegVal[0] = pAmbaRCT_Reg->Gpio2DriveStrength[0];
        RegVal[1] = pAmbaRCT_Reg->Gpio2DriveStrength[1];
#if defined(CONFIG_SOC_CV2FS) || defined(CONFIG_SOC_CV22FS) || defined(CONFIG_SOC_CV22) || defined(CONFIG_SOC_CV25) || defined(CONFIG_SOC_CV2) || defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)
    } else if (PinGroup == AMBA_GPIO_GROUP3) {
        RegVal[0] = pAmbaRCT_Reg->Gpio3DriveStrength[0];
        RegVal[1] = pAmbaRCT_Reg->Gpio3DriveStrength[1];
#endif
#if defined(CONFIG_SOC_CV22) || defined(CONFIG_SOC_CV2) || defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)
    } else if (PinGroup == AMBA_GPIO_GROUP4) {
        RegVal[0] = pAmbaRCT_Reg->Gpio4DriveStrength[0];
        RegVal[1] = pAmbaRCT_Reg->Gpio4DriveStrength[1];
#endif
#if defined(CONFIG_SOC_CV2)
    } else if (PinGroup == AMBA_GPIO_GROUP5) {
        RegVal[0] = pAmbaRCT_Reg->Gpio5DriveStrength[0];
        RegVal[1] = pAmbaRCT_Reg->Gpio5DriveStrength[1];
#endif
    } else {
        /* Should not happen!! */
        RegVal[0] = 0U;
        RegVal[1] = 0U;
    }

    if ((RegVal[0] & ((UINT32)1U << (BitPos))) != 0U) {
        if ((RegVal[1] & ((UINT32)1U << (BitPos))) != 0U) {
            RetVal = GPIO_DRV_STR_L3;
        } else {
            RetVal = GPIO_DRV_STR_L2;
        }
    } else {
        if ((RegVal[1] & ((UINT32)1U << (BitPos))) != 0U) {
            RetVal = GPIO_DRV_STR_L1;
        } else {
            RetVal = GPIO_DRV_STR_L0;
        }
    }

    return RetVal;
}

/**
 *  AmbaCSL_GpioSetPinDriveStr - Set the driving strength for a GPIO pin
 *  @param[in] PinGroup GPIO group ID
 *  @param[in] BitPos GPIO pin offset within group
 *  @param[in] DriveStrength Pin driving strength selection
 */
void AmbaCSL_GpioSetPinDriveStr(UINT32 PinGroup, UINT32 BitPos, UINT32 DriveStrength)
{
    UINT32 RegVal[2];

    if (PinGroup == AMBA_GPIO_GROUP0) {
        RegVal[0] = pAmbaRCT_Reg->Gpio0DriveStrength[0];
        RegVal[1] = pAmbaRCT_Reg->Gpio0DriveStrength[1];
    } else if (PinGroup == AMBA_GPIO_GROUP1) {
        RegVal[0] = pAmbaRCT_Reg->Gpio1DriveStrength[0];
        RegVal[1] = pAmbaRCT_Reg->Gpio1DriveStrength[1];
    } else if (PinGroup == AMBA_GPIO_GROUP2) {
        RegVal[0] = pAmbaRCT_Reg->Gpio2DriveStrength[0];
        RegVal[1] = pAmbaRCT_Reg->Gpio2DriveStrength[1];
#if defined(CONFIG_SOC_CV2FS) || defined(CONFIG_SOC_CV22FS) || defined(CONFIG_SOC_CV22) || defined(CONFIG_SOC_CV25) || defined(CONFIG_SOC_CV2) || defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)
    } else if (PinGroup == AMBA_GPIO_GROUP3) {
        RegVal[0] = pAmbaRCT_Reg->Gpio3DriveStrength[0];
        RegVal[1] = pAmbaRCT_Reg->Gpio3DriveStrength[1];
#endif
#if defined(CONFIG_SOC_CV2) || defined(CONFIG_SOC_CV22) || defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)
    } else if (PinGroup == AMBA_GPIO_GROUP4) {
        RegVal[0] = pAmbaRCT_Reg->Gpio4DriveStrength[0];
        RegVal[1] = pAmbaRCT_Reg->Gpio4DriveStrength[1];
#endif
#if defined(CONFIG_SOC_CV2)
    } else if (PinGroup == AMBA_GPIO_GROUP5) {
        RegVal[0] = pAmbaRCT_Reg->Gpio5DriveStrength[0];
        RegVal[1] = pAmbaRCT_Reg->Gpio5DriveStrength[1];
#endif
    } else {
        /* Should not happen!! */
        RegVal[0] = 0U;
        RegVal[1] = 0U;
    }

    if (DriveStrength == GPIO_DRV_STR_L0) {
        RegVal[0] &= ~((UINT32)1U << (BitPos));
        RegVal[1] &= ~((UINT32)1U << (BitPos));
    } else if (DriveStrength == GPIO_DRV_STR_L1) {
        RegVal[0] &= ~((UINT32)1U << (BitPos));
        RegVal[1] |= ((UINT32)1U << (BitPos));
    } else if (DriveStrength == GPIO_DRV_STR_L2) {
        RegVal[0] |= ((UINT32)1U << (BitPos));
        RegVal[1] &= ~((UINT32)1U << (BitPos));
    } else if (DriveStrength == GPIO_DRV_STR_L3) {
        RegVal[0] |= ((UINT32)1U << (BitPos));
        RegVal[1] |= ((UINT32)1U << (BitPos));
    } else {
        /* Should not happen!! */
    }

    AmbaCSL_GpioSetPinGrpDriveStr(PinGroup, RegVal);
}

/**
 *  AmbaCSL_GpioSetPinGrpDriveStr - Set the driving strength for a group of GPIO pins
 *  @param[in] PinGroup GPIO group ID
 *  @param[in] pPinGroupDriveStrength: Grouped pin driving strength selection
 */
void AmbaCSL_GpioSetPinGrpDriveStr(UINT32 PinGroup, const UINT32 *pPinGroupDriveStrength)
{
    if (PinGroup == AMBA_GPIO_GROUP0) {
        pAmbaRCT_Reg->Gpio0DriveStrength[0] = pPinGroupDriveStrength[0];
        pAmbaRCT_Reg->Gpio0DriveStrength[1] = pPinGroupDriveStrength[1];
    } else if (PinGroup == AMBA_GPIO_GROUP1) {
        pAmbaRCT_Reg->Gpio1DriveStrength[0] = pPinGroupDriveStrength[0];
        pAmbaRCT_Reg->Gpio1DriveStrength[1] = pPinGroupDriveStrength[1];
    } else if (PinGroup == AMBA_GPIO_GROUP2) {
        pAmbaRCT_Reg->Gpio2DriveStrength[0] = pPinGroupDriveStrength[0];
        pAmbaRCT_Reg->Gpio2DriveStrength[1] = pPinGroupDriveStrength[1];
#if defined(CONFIG_SOC_CV2FS) || defined(CONFIG_SOC_CV22FS) || defined(CONFIG_SOC_CV22) || defined(CONFIG_SOC_CV25) || defined(CONFIG_SOC_CV2) || defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)
    } else if (PinGroup == AMBA_GPIO_GROUP3) {
        pAmbaRCT_Reg->Gpio3DriveStrength[0] = pPinGroupDriveStrength[0];
        pAmbaRCT_Reg->Gpio3DriveStrength[1] = pPinGroupDriveStrength[1];
#endif
#if defined(CONFIG_SOC_CV2) || defined(CONFIG_SOC_CV22) || defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)
    } else if (PinGroup == AMBA_GPIO_GROUP4) {
        pAmbaRCT_Reg->Gpio4DriveStrength[0] = pPinGroupDriveStrength[0];
        pAmbaRCT_Reg->Gpio4DriveStrength[1] = pPinGroupDriveStrength[1];
#endif
#if defined(CONFIG_SOC_CV2)
    } else if (PinGroup == AMBA_GPIO_GROUP5) {
        pAmbaRCT_Reg->Gpio5DriveStrength[0] = pPinGroupDriveStrength[0];
        pAmbaRCT_Reg->Gpio5DriveStrength[1] = pPinGroupDriveStrength[1];
#endif
    } else {
        /* Should not happen!! */
    }
}
