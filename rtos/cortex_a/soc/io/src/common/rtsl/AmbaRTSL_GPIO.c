/**
 *  @file AmbaRTSL_GPIO.c
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
 *  @details GPIO RTSL Device Driver
 *
 */

#include "AmbaTypes.h"
#include "AmbaMisraFix.h"
#include "AmbaWrap.h"

#include "AmbaRTSL_GIC.h"
#include "AmbaRTSL_GPIO.h"
#include "AmbaRTSL_GPIO_Ctrl.h"

#include "AmbaCSL_GPIO.h"

#if defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)
#include "AmbaCortexA76.h"
#else
#include "AmbaCortexA53.h"
#endif
#if defined(CONFIG_DEVICE_TREE_SUPPORT)
#include <AmbaIOUtility.h>
#endif

typedef struct {
    UINT32  PinFuncRegVal[3];               /* GPIO or alternate function */
    UINT32  PinIoTypeRegVal;                /* Input or output */
    UINT32  PinStateProtectRegVal;          /* Read-only or read-writeable */
    UINT32  PinStateRegVal;                 /* Logical zero or logical one */
    UINT32  PinPullFuncEnableRegVal;        /* Disable/enable internal resistance */
    UINT32  PinPullFuncTypeRegVal;          /* Internal resistance is pull-down or pull-up */
    UINT32  PinDriverStrengthRegVal[2];     /* Driving strength is 2mA/4mA/8mA/12mA */

    UINT32  IntFuncArg[32];                 /* Optional argument of each pin ISR */
    AMBA_GPIO_ISR_f IntFunc[32];            /* ISR of each pin */
} AMBA_GPIO_CTRL_s;

static AMBA_GPIO_CTRL_s AmbaGpioCtrl[AMBA_NUM_GPIO_GROUP] GNU_SECTION_NOZEROINIT;
#if !defined(CONFIG_QNX)
static const UINT32 AmbaGpioIntID[AMBA_NUM_GPIO_GROUP] = {
#if defined(CONFIG_SOC_CV2FS) || defined(CONFIG_SOC_CV22FS)
    [AMBA_GPIO_GROUP0] = AMBA_INT_SPI_ID085_GPIO_GROUP0,
    [AMBA_GPIO_GROUP1] = AMBA_INT_SPI_ID086_GPIO_GROUP1,
    [AMBA_GPIO_GROUP2] = AMBA_INT_SPI_ID087_GPIO_GROUP2,
    [AMBA_GPIO_GROUP3] = AMBA_INT_SPI_ID088_GPIO_GROUP3,
#elif defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)
    [AMBA_GPIO_GROUP0] = AMBA_INT_SPI_ID76_GPIO0,
    [AMBA_GPIO_GROUP1] = AMBA_INT_SPI_ID77_GPIO1,
    [AMBA_GPIO_GROUP2] = AMBA_INT_SPI_ID78_GPIO2,
    [AMBA_GPIO_GROUP3] = AMBA_INT_SPI_ID79_GPIO3,
    [AMBA_GPIO_GROUP4] = AMBA_INT_SPI_ID80_GPIO4,
#else
    [AMBA_GPIO_GROUP0] = AMBA_INT_SPI_ID75_GPIO0,
    [AMBA_GPIO_GROUP1] = AMBA_INT_SPI_ID76_GPIO1,
    [AMBA_GPIO_GROUP2] = AMBA_INT_SPI_ID77_GPIO2,
#if defined(CONFIG_SOC_CV2) || defined(CONFIG_SOC_CV22) || defined(CONFIG_SOC_CV25)
    [AMBA_GPIO_GROUP3] = AMBA_INT_SPI_ID78_GPIO3,
#endif
#if defined(CONFIG_SOC_CV2) || defined(CONFIG_SOC_CV22)
    [AMBA_GPIO_GROUP4] = AMBA_INT_SPI_ID79_GPIO4,
#endif //CONFIG_SOC_CV22
#if defined(CONFIG_SOC_CV2)
    [AMBA_GPIO_GROUP5] = AMBA_INT_SPI_ID80_GPIO5,
#endif
#endif
};
#endif

AMBA_GPIO_REG_s * pAmbaGPIO_Reg[AMBA_NUM_GPIO_GROUP];

#if !defined(CONFIG_QNX)
/**
 *  GPIO_IntHandler - ISR for one asserted line of a GPIO group
 *  @param[in] IntID Interrupt ID
 *  @param[in] PinGroup  GPIO pin group
 */
#pragma GCC push_options
#pragma GCC target("general-regs-only")

static void GPIO_IntHandler(UINT32 IntID, UINT32 PinGroup)
{
    const AMBA_GPIO_CTRL_s *pGpioCtrl = &AmbaGpioCtrl[PinGroup];
    UINT32 PinID;
    UINT32 i;

    AmbaMisra_TouchUnused(&IntID);

    for (i = 0U; i < 32U; i ++) {
        if (AmbaCSL_GpioGetIntState(PinGroup, i) != 0x0U) {
            PinID = ((PinGroup << 5U) + i);
            if (pGpioCtrl->IntFunc[i] != NULL) {
                pGpioCtrl->IntFunc[i](PinID, pGpioCtrl->IntFuncArg[i]);   /* invoke the ISR */
            }
            AmbaCSL_GpioWriteEndOfInt(PinGroup, i);
            break;
        }
    }
}
#pragma GCC pop_options
#endif

/**
 *  GPIO_ConfigDebounceGpio - For config debounced GPIO
 *  @param[in] PinID GPIO pin ID
 */
static void GPIO_ConfigDebounceGpio(UINT32 PinID)
{
#if !defined(CONFIG_SOC_CV5) && !defined(CONFIG_SOC_CV52)
    switch (PinID) {
    case GPIO_PIN_DEBOUNCE_GPIO0:
#if !defined(CONFIG_SOC_CV2FS) && !defined(CONFIG_SOC_CV28) && !defined(CONFIG_SOC_H32) && !defined(CONFIG_SOC_CV22FS)
        AmbaCSL_SetDebounceGpio0(1U);
        break;

    case GPIO_PIN_DEBOUNCE_GPIO1:
        AmbaCSL_SetDebounceGpio1(1U);
#endif
        break;

    default:
        /* shall not happen!! */
        break;
    }
#else
    /* CV5/CV52 */
    AmbaMisra_TouchUnused(&PinID);
#endif
    // For MISRA 2012 Rule 2.2, example use __asm__ ("nop")
    {
        volatile UINT32 x = 0;
        (void)x;
    }
}

/**
 *  AmbaRTSL_GpioInit - Init GPIO related buffer
 */
void AmbaRTSL_GpioInit(void)
{
#if !defined(CONFIG_QNX)
    if (AmbaWrap_memset(AmbaGpioCtrl, 0x0, sizeof(AmbaGpioCtrl)) != 0U) {
        /* For misra-c */
    }
#endif
}

/**
 *  AmbaRTSL_GpioLoadDefaultVals - Configure every GPIO pin as the desired function by default
 *  @param[in] pDefaultParam Default settings for every GPIO pin
 */
void AmbaRTSL_GpioLoadDefaultVals(const AMBA_GPIO_DEFAULT_s * pDefaultParam)
{
    AMBA_GPIO_CTRL_s *pGpioCtrl;
    UINT32 i;

#if !defined(CONFIG_QNX)
    ULONG base_addr;
    static const AMBA_INT_CONFIG_s IntConfig = {
        .TriggerType = INT_TRIG_HIGH_LEVEL,
        .IrqType = INT_TYPE_IRQ,
        .CpuTargets = 0x1U
    };

#if defined (CONFIG_SOC_CV2FS) || defined (CONFIG_SOC_CV22FS)
    base_addr = AMBA_CA53_GPIO0_BASE_ADDR;
    AmbaMisra_TypeCast(&pAmbaGPIO_Reg[0], &base_addr);
    base_addr = AMBA_CA53_GPIO1_BASE_ADDR;
    AmbaMisra_TypeCast(&pAmbaGPIO_Reg[1], &base_addr);
    base_addr = AMBA_CA53_GPIO2_BASE_ADDR;
    AmbaMisra_TypeCast(&pAmbaGPIO_Reg[2], &base_addr);
    base_addr = AMBA_CA53_GPIO3_BASE_ADDR;
    AmbaMisra_TypeCast(&pAmbaGPIO_Reg[3], &base_addr);
#elif defined (CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)
    base_addr = AMBA_CORTEX_A76_GPIO0_BASE_ADDR;
    AmbaMisra_TypeCast(&pAmbaGPIO_Reg[0], &base_addr);
    base_addr = AMBA_CORTEX_A76_GPIO1_BASE_ADDR;
    AmbaMisra_TypeCast(&pAmbaGPIO_Reg[1], &base_addr);
    base_addr = AMBA_CORTEX_A76_GPIO2_BASE_ADDR;
    AmbaMisra_TypeCast(&pAmbaGPIO_Reg[2], &base_addr);
    base_addr = AMBA_CORTEX_A76_GPIO3_BASE_ADDR;
    AmbaMisra_TypeCast(&pAmbaGPIO_Reg[3], &base_addr);
    base_addr = AMBA_CORTEX_A76_GPIO4_BASE_ADDR;
    AmbaMisra_TypeCast(&pAmbaGPIO_Reg[4], &base_addr);
#else
    base_addr = AMBA_CORTEX_A53_GPIO0_BASE_ADDR;
    AmbaMisra_TypeCast(&pAmbaGPIO_Reg[0], &base_addr);
    base_addr = AMBA_CORTEX_A53_GPIO1_BASE_ADDR;
    AmbaMisra_TypeCast(&pAmbaGPIO_Reg[1], &base_addr);
    base_addr = AMBA_CORTEX_A53_GPIO2_BASE_ADDR;
    AmbaMisra_TypeCast(&pAmbaGPIO_Reg[2], &base_addr);
#ifdef CONFIG_SOC_CV22
    base_addr = AMBA_CORTEX_A53_GPIO3_BASE_ADDR;
    AmbaMisra_TypeCast(&pAmbaGPIO_Reg[3], &base_addr);
    base_addr = AMBA_CORTEX_A53_GPIO4_BASE_ADDR;
    AmbaMisra_TypeCast(&pAmbaGPIO_Reg[4], &base_addr);
#endif
#ifdef CONFIG_SOC_CV25
    base_addr = AMBA_CORTEX_A53_GPIO3_BASE_ADDR;
    AmbaMisra_TypeCast(&pAmbaGPIO_Reg[3], &base_addr);
#endif
#ifdef CONFIG_SOC_CV2
    base_addr = AMBA_CORTEX_A53_GPIO3_BASE_ADDR;
    AmbaMisra_TypeCast(&pAmbaGPIO_Reg[3], &base_addr);
    base_addr = AMBA_CORTEX_A53_GPIO4_BASE_ADDR;
    AmbaMisra_TypeCast(&pAmbaGPIO_Reg[4], &base_addr);
    base_addr = AMBA_CORTEX_A53_GPIO5_BASE_ADDR;
    AmbaMisra_TypeCast(&pAmbaGPIO_Reg[5], &base_addr);
#endif

#endif

#if defined(CONFIG_DEVICE_TREE_SUPPORT)
    for (i = 0U ; i < AMBA_NUM_GPIO_GROUP ; i++) {
        base_addr = IO_UtilityFDTPropertyU32Quick(0, "ambarella,pinctrl", "reg", i * 2U);
        if ( base_addr != 0U ) {
#if defined (CONFIG_SOC_CV5)|| defined (CONFIG_SOC_CV52)
            base_addr = base_addr | AMBA_CORTEX_A76_APB_VIRT_BASE_ADDR;
#endif
            AmbaMisra_TypeCast(&pAmbaGPIO_Reg[i], &base_addr);
        }
    }
#endif
#endif

    pGpioCtrl = (AMBA_GPIO_CTRL_s *) & (AmbaGpioCtrl[0U]);
    if (pDefaultParam != NULL) {
        for (i = 0U; i < AMBA_NUM_GPIO_GROUP; i++) {
            pGpioCtrl->PinFuncRegVal[0U] = pDefaultParam->PinGroupRegVal[i].PinFuncRegVal[0U];
            pGpioCtrl->PinFuncRegVal[1U] = pDefaultParam->PinGroupRegVal[i].PinFuncRegVal[1U];
            pGpioCtrl->PinFuncRegVal[2U] = pDefaultParam->PinGroupRegVal[i].PinFuncRegVal[2U];
            pGpioCtrl->PinIoTypeRegVal = pDefaultParam->PinGroupRegVal[i].PinIoTypeRegVal;
            pGpioCtrl->PinStateProtectRegVal = pDefaultParam->PinGroupRegVal[i].PinStateProtectRegVal;
            pGpioCtrl->PinStateRegVal = pDefaultParam->PinGroupRegVal[i].PinStateRegVal;
            pGpioCtrl->PinPullFuncEnableRegVal = pDefaultParam->PinGroupRegVal[i].PinPullFuncEnableRegVal;
            pGpioCtrl->PinPullFuncTypeRegVal = pDefaultParam->PinGroupRegVal[i].PinPullFuncTypeRegVal;
            pGpioCtrl->PinDriverStrengthRegVal[0U] = pDefaultParam->PinGroupRegVal[i].PinDriverStrengthRegVal[0U];
            pGpioCtrl->PinDriverStrengthRegVal[1U] = pDefaultParam->PinGroupRegVal[i].PinDriverStrengthRegVal[1U];
            pGpioCtrl++;
        }
    } else {
        for (i = 0U; i < AMBA_NUM_GPIO_GROUP; i++) {
            pGpioCtrl->PinIoTypeRegVal = 0xffffffffU;
            pGpioCtrl->PinStateProtectRegVal = 0xffffffffU;
            pGpioCtrl->PinStateRegVal = 0x00000000U;
            pGpioCtrl->PinDriverStrengthRegVal[0U] = 0xffffffffU;
            pGpioCtrl->PinDriverStrengthRegVal[1U] = 0x00000000U;
            pGpioCtrl++;
        }
    }

    pGpioCtrl = (AMBA_GPIO_CTRL_s *) & (AmbaGpioCtrl[0U]);
    for (i = 0U; i < AMBA_NUM_GPIO_GROUP; i++) {
        AmbaCSL_GpioEnablePinGrp(i);
        AmbaCSL_GpioSetPinGrpAltFunc(i, 0U);
        AmbaCSL_GpioSetPinGrpFunc(i, pGpioCtrl->PinFuncRegVal);
        AmbaCSL_GpioSetPinGrpDirection(i, pGpioCtrl->PinIoTypeRegVal);
        AmbaCSL_GpioSetPinGrpMask(i, pGpioCtrl->PinStateProtectRegVal);
        AmbaCSL_GpioSetPinGrpState(i, pGpioCtrl->PinStateRegVal);
        AmbaCSL_GpioSetPinGrpPullCtrl(i, pGpioCtrl->PinPullFuncEnableRegVal, pGpioCtrl->PinPullFuncTypeRegVal);
        AmbaCSL_GpioSetPinGrpDriveStr(i, pGpioCtrl->PinDriverStrengthRegVal);
        AmbaCSL_GpioClearPinGrpInt(i);

#if !defined(CONFIG_QNX)
        /* Enable the Interrupt */
        (void)AmbaRTSL_GicIntConfig(AmbaGpioIntID[i], &IntConfig, GPIO_IntHandler, i);
        (void)AmbaRTSL_GicIntEnable(AmbaGpioIntID[i]);
#endif
        pGpioCtrl++;
    }
}

/**
 *  AmbaRTSL_GpioSetEnable - Config GPIO pin to be reachable when getting state
 *  @param[in] PinID GPIO pin ID
 *  @return error code
 */
UINT32 AmbaRTSL_GpioSetEnable(UINT32 PinID)
{
    UINT32 PinNo = PinID & 0xfffU;
    UINT32 RegPinGroup = PinNo >> 5U;
    UINT32 RegBitPosition = PinNo & 0x1fU;
    UINT32 RetVal = GPIO_ERR_NONE;

    if (PinNo >= AMBA_NUM_GPIO_PIN) {
        RetVal = GPIO_ERR_ARG;
    } else {
        AmbaCSL_GpioSetEnable(RegPinGroup, RegBitPosition);
    }

    return RetVal;
}

/**
 *  AmbaRTSL_GpioSetFuncGPI - Configure GPIO pin as general purpose input function
 *  @param[in] PinID GPIO pin ID
 *  @return error code
 */
UINT32 AmbaRTSL_GpioSetFuncGPI(UINT32 PinID)
{
    UINT32 PinNo = PinID & 0xfffU;
    UINT32 RegPinGroup = PinNo >> 5U;
    UINT32 RegBitPosition = PinNo & 0x1fU;
    UINT32 RetVal = GPIO_ERR_NONE;

    if (PinNo >= AMBA_NUM_GPIO_PIN) {
        RetVal = GPIO_ERR_ARG;
    } else {
        AmbaCSL_GpioSetPinFunc(RegPinGroup, RegBitPosition, GPIO_FUNC_GPI);
        AmbaCSL_GpioSetEnable(RegPinGroup, RegBitPosition);
    }

    return RetVal;
}

/**
 *  AmbaRTSL_GpioSetFuncGPO - Configure GPIO pin as general purpose output function
 *  @param[in] PinID GPIO pin ID
 *  @param[in] PinLevel Output voltage level
 *  @return error code
 */
UINT32 AmbaRTSL_GpioSetFuncGPO(UINT32 PinID, UINT32 PinLevel)
{
    UINT32 PinNo = PinID & 0xfffU;
    UINT32 RegPinGroup = PinNo >> 5U;
    UINT32 RegBitPosition = PinNo & 0x1fU;
    UINT32 SavedPinGrpMask, RetVal = GPIO_ERR_NONE;

    if (PinNo >= AMBA_NUM_GPIO_PIN) {
        RetVal = GPIO_ERR_ARG;
    } else {
        AmbaCSL_GpioSetPinFunc(RegPinGroup, RegBitPosition, GPIO_FUNC_GPO);

        SavedPinGrpMask = AmbaCSL_GpioGetPinGrpMask(RegPinGroup);
        AmbaCSL_GpioSetPinGrpMask(RegPinGroup, (UINT32)((UINT32)1U << RegBitPosition));

        if (PinLevel == GPIO_LV_HIGH) {
            AmbaCSL_GpioSetHighLv(RegPinGroup, RegBitPosition);
        } else {
            AmbaCSL_GpioSetLowLv(RegPinGroup, RegBitPosition);
        }

        AmbaCSL_GpioSetPinGrpMask(RegPinGroup, SavedPinGrpMask);
        AmbaCSL_GpioSetEnable(RegPinGroup, RegBitPosition);
    }

    return RetVal;
}

/**
 *  AmbaRTSL_GpioSetFuncAlt - Configure the specified pin as an alternate function
 *  @param[in] PinID GPIO pin ID
 *  @return error code
 */
UINT32 AmbaRTSL_GpioSetFuncAlt(UINT32 PinID)
{
    UINT32 PinNo = PinID & 0xfffU;
    UINT32 RegPinGroup = PinNo >> 5U;
    UINT32 RegBitPosition = PinNo & 0x1fU;
    UINT32 AltFunc = GPIO_GET_ALT_FUNC(PinID);
    UINT32 PinFunc;
    UINT32 RetVal = GPIO_ERR_NONE;

    if ((PinNo >= AMBA_NUM_GPIO_PIN) || (AltFunc == 0U)) {
        RetVal = GPIO_ERR_ARG;
    } else {
        PinFunc = AltFunc + GPIO_FUNC_ALT1 - 1U;

        /* Deny the access to GPIO pin level state */
        AmbaCSL_GpioSetDisable(RegPinGroup, RegBitPosition);

        if (PinFunc >= AMBA_NUM_GPIO_FUNC) {
            GPIO_ConfigDebounceGpio(PinID);  /* For RCT controlled GPIO */
        } else {
            AmbaCSL_GpioSetPinFunc(RegPinGroup, RegBitPosition, PinFunc);
        }
    }

    return RetVal;
}

/**
 *  AmbaRTSL_GpioSetDriveStrength - Set drive strength of GPIO pin
 *  @param[in] PinID GPIO pin ID
 *  @param[in] PinDriveStrength I/O drive strength selection
 *  @return error code
 */
UINT32 AmbaRTSL_GpioSetDriveStrength(UINT32 PinID, UINT32 PinDriveStrength)
{
    UINT32 PinNo = PinID & 0xfffU;
    UINT32 RegPinGroup = PinNo >> 5U;
    UINT32 RegBitPosition = PinNo & 0x1fU;
    UINT32 RetVal = GPIO_ERR_NONE;

    if (PinNo >= AMBA_NUM_GPIO_PIN) {
        RetVal = GPIO_ERR_ARG;
    } else {
        AmbaCSL_GpioSetPinDriveStr(RegPinGroup, RegBitPosition, PinDriveStrength);
    }

    return RetVal;
}

/**
 *  AmbaRTSL_GpioSetPullUpOrDown - disables/enables the pull-up/pull-down function of the specified pin
 *  @param[in] PinID GPIO pin ID
 *  @param[in] PullCtrl Pin pull up/down control
 *  @return error code
 */
UINT32 AmbaRTSL_GpioSetPullUpOrDown(UINT32 PinID, UINT32 PullUpOrDown)
{
    UINT32 PinNo = PinID & 0xfffU;
    UINT32 RegPinGroup = PinNo >> 5U;
    UINT32 RegBitPosition = PinNo & 0x1fU;
    UINT32 RetVal = GPIO_ERR_NONE;

    if (PinNo >= AMBA_NUM_GPIO_PIN) {
        RetVal = GPIO_ERR_ARG;
    } else {
        AmbaCSL_GpioSetPinPullCtrl(RegPinGroup, RegBitPosition, PullUpOrDown);
    }

    return RetVal;
}

/**
 *  AmbaRTSL_GpioGetPinInfo - Query the current GPIO line configuration
 *  @param[in] PinID GPIO pin ID
 *  @param[out] pPinInfo GPIO pin current state
 *  @return error code
 */
UINT32 AmbaRTSL_GpioGetPinInfo(UINT32 PinID, AMBA_GPIO_INFO_s *pPinInfo)
{
    UINT32 PinNo = PinID & 0xfffU;
    UINT32 RegPinGroup = PinNo >> 5U;
    UINT32 RegBitPosition = PinNo & 0x1fU;
    UINT32 RetVal = GPIO_ERR_NONE;

    if ((PinNo >= AMBA_NUM_GPIO_PIN) || (pPinInfo == NULL)) {
        RetVal = GPIO_ERR_ARG;
    } else {
        pPinInfo->PinFunc = AmbaCSL_GpioGetPinFunc(RegPinGroup, RegBitPosition);

        if (AmbaCSL_GpioGetPinStateEnable(RegPinGroup, RegBitPosition) == 0x0U) {
            pPinInfo->PinState = GPIO_LV_UNKNOWN;
        } else {
            if (AmbaCSL_GpioGetPinLevel(RegPinGroup, RegBitPosition) == 0x0U) {
                pPinInfo->PinState = GPIO_LV_LOW;
            } else {
                pPinInfo->PinState = GPIO_LV_HIGH;
            }
        }

        pPinInfo->PinPullUpOrDown = AmbaCSL_GpioGetPinPullCtrl(RegPinGroup, RegBitPosition);
        pPinInfo->PinDriveStrength = AmbaCSL_GpioGetPinDriveStr(RegPinGroup, RegBitPosition);

        if (AmbaCSL_GpioGetIntEnable(RegPinGroup, RegBitPosition) == 0x0U) {
            pPinInfo->IntType = GPIO_INT_NO_INTERRUPT;
        } else {
            if (AmbaCSL_GpioGetIntType(RegPinGroup, RegBitPosition) == 0x0U) {
                if (AmbaCSL_GpioGetIntRate(RegPinGroup, RegBitPosition) != 0x0U) {
                    pPinInfo->IntType = GPIO_INT_BOTH_EDGE_TRIGGER;
                } else {
                    if (AmbaCSL_GpioGetIntMode(RegPinGroup, RegBitPosition) != 0x0U) {
                        pPinInfo->IntType = GPIO_INT_RISING_EDGE_TRIGGER;
                    } else {
                        pPinInfo->IntType = GPIO_INT_FALLING_EDGE_TRIGGER;
                    }
                }
            } else {
                if (AmbaCSL_GpioGetIntMode(RegPinGroup, RegBitPosition) == 0x0U) {
                    pPinInfo->IntType = GPIO_INT_LOW_LEVEL_TRIGGER;
                } else {
                    pPinInfo->IntType = GPIO_INT_HIGH_LEVEL_TRIGGER;
                }
            }
        }
        pPinInfo->IntFunc = AmbaGpioCtrl[RegPinGroup].IntFunc[RegBitPosition];
    }

    return RetVal;
}

/**
 *  AmbaRTSL_GpioCheckFunc - Check if the specified pin is configured for the specified function
 *  @param[in] PinID GPIO pin ID
 *  @return error code
 */
UINT32 AmbaRTSL_GpioCheckFunc(UINT32 PinID)
{
    AMBA_GPIO_INFO_s PinInfo;
    UINT32 RetVal;

    RetVal = AmbaRTSL_GpioGetPinInfo(PinID, &PinInfo);
    if (RetVal == GPIO_ERR_NONE) {
        if (PinInfo.PinFunc != GPIO_GET_ALT_FUNC(PinID)) {
            RetVal = GPIO_ERR_FUNC;
        }
    }

    return RetVal;
}

/**
 *  AmbaRTSL_GpioIntHookHandler - Hook GPIO Interrupt Service Routine (ISR)
 *  @param[in] PinID GPIO pin ID
 *  @param[in] IntFunc Interrupt service routine
 *  @param[in] IntFuncArg Optional argument of interrupt handler
 *  @return error code
 */
UINT32 AmbaRTSL_GpioIntHookHandler(UINT32 PinID, AMBA_GPIO_ISR_f IntFunc, UINT32 IntFuncArg)
{
    UINT32 PinNo = PinID & 0xfffU;
    UINT32 RegPinGroup = PinNo >> 5U;
    UINT32 RegBitPosition = PinNo & 0x1fU;
    UINT32 RetVal = GPIO_ERR_NONE;

    if ((PinNo >= AMBA_NUM_GPIO_PIN) || (IntFunc == NULL)) {
        RetVal = GPIO_ERR_ARG;
    } else {
        AmbaGpioCtrl[RegPinGroup].IntFunc[RegBitPosition] = IntFunc;  /* Register the ISR */
        AmbaGpioCtrl[RegPinGroup].IntFuncArg[RegBitPosition] = IntFuncArg;
    }

    return RetVal;
}

/**
 *  AmbaRTSL_GpioIntSetType - configures the interrupt generation type
 *  @param[in] PinID GPIO pin ID
 *  @param[in] IntConfig Interrupt generation type
 *  @return error code
 */
UINT32 AmbaRTSL_GpioIntSetType(UINT32 PinID, UINT32 IntType)
{
    UINT32 PinNo = PinID & 0xfffU;
    UINT32 RegPinGroup = PinNo >> 5U;
    UINT32 RegBitPosition = PinNo & 0x1fU;
    UINT32 RetVal = GPIO_ERR_NONE;

    if (PinNo >= AMBA_NUM_GPIO_PIN) {
        RetVal = GPIO_ERR_ARG;
    } else {
        switch (IntType) {
        case GPIO_INT_HIGH_LEVEL_TRIGGER:
            AmbaCSL_GpioSetIntLevelTrigger(RegPinGroup, RegBitPosition);
            AmbaCSL_GpioSetIntByConfig(RegPinGroup, RegBitPosition);
            AmbaCSL_GpioSetIntOnRisingOrHigh(RegPinGroup, RegBitPosition);
            break;
        case GPIO_INT_LOW_LEVEL_TRIGGER:
            AmbaCSL_GpioSetIntLevelTrigger(RegPinGroup, RegBitPosition);
            AmbaCSL_GpioSetIntByConfig(RegPinGroup, RegBitPosition);
            AmbaCSL_GpioSetIntOnFallingOrLow(RegPinGroup, RegBitPosition);
            break;
        case GPIO_INT_RISING_EDGE_TRIGGER:
            AmbaCSL_GpioSetIntEdgeTrigger(RegPinGroup, RegBitPosition);
            AmbaCSL_GpioSetIntByConfig(RegPinGroup, RegBitPosition);
            AmbaCSL_GpioSetIntOnRisingOrHigh(RegPinGroup, RegBitPosition);
            break;
        case GPIO_INT_FALLING_EDGE_TRIGGER:
            AmbaCSL_GpioSetIntEdgeTrigger(RegPinGroup, RegBitPosition);
            AmbaCSL_GpioSetIntByConfig(RegPinGroup, RegBitPosition);
            AmbaCSL_GpioSetIntOnFallingOrLow(RegPinGroup, RegBitPosition);
            break;
        case GPIO_INT_BOTH_EDGE_TRIGGER:
            AmbaCSL_GpioSetIntEdgeTrigger(RegPinGroup, RegBitPosition);
            AmbaCSL_GpioSetIntAnyEdge(RegPinGroup, RegBitPosition);
            break;
        default:
            RetVal = GPIO_ERR_ARG;
            break;
        }
    }

    return RetVal;
}

/**
 *  AmbaRTSL_GpioIntEnable - GPIO Enable Interrupt
 *  @param[in] PinID GPIO pin ID
 *  @return error code
 */
UINT32 AmbaRTSL_GpioIntEnable(UINT32 PinID)
{
    UINT32 PinNo = PinID & 0xfffU;
    UINT32 PinGroup = PinNo >> 5U;
    UINT32 RegBitPosition = PinNo & 0x1fU;
    UINT32 RetVal = GPIO_ERR_NONE;

    if (PinNo >= AMBA_NUM_GPIO_PIN) {
        RetVal = GPIO_ERR_ARG;
    } else {
        /* Enable interrupt to interrupt controller */
#if !defined(CONFIG_QNX)
        (void)AmbaRTSL_GicIntEnable(AmbaGpioIntID[PinGroup]);
#endif
        AmbaCSL_GpioWriteEndOfInt(PinGroup, RegBitPosition);
        AmbaCSL_GpioEnableInt(PinGroup, RegBitPosition);
    }

    return RetVal;
}

/**
 *  AmbaRTSL_GpioIntDisable - GPIO Disable Interrupt
 *  @param[in] PinID GPIO pin ID
 *  @return error code
 */
UINT32 AmbaRTSL_GpioIntDisable(UINT32 PinID)
{
    UINT32 PinNo = PinID & 0xfffU;
    UINT32 PinGroup = PinNo >> 5U;
    UINT32 RegBitPosition = PinNo & 0x1fU;
    UINT32 RetVal = GPIO_ERR_NONE;

    if (PinNo >= AMBA_NUM_GPIO_PIN) {
        RetVal = GPIO_ERR_ARG;
    } else {
        AmbaCSL_GpioDisableInt(PinGroup, RegBitPosition);

        /* Disable interrupt to interrupt controller if and only if there is no GPIO interrupt in same group enabled */
        if (AmbaCSL_GpioGetIntGrpEnable(PinGroup) == 0x0U) {
#if !defined(CONFIG_QNX)
            (void)AmbaRTSL_GicIntDisable(AmbaGpioIntID[PinGroup]);
#endif
        }
    }

    return RetVal;
}

/**
 *  AmbaRTSL_GpioIntClear - GPIO Clear Interrupt
 *  @param[in] PinID GPIO pin ID
 *  @return error code
 */
UINT32 AmbaRTSL_GpioIntClear(UINT32 PinID)
{
    UINT32 PinNo = PinID & 0xfffU;
    UINT32 PinGroup = PinNo >> 5U;
    UINT32 RegBitPosition = PinNo & 0x1fU;
    UINT32 RetVal = GPIO_ERR_NONE;

    if (PinNo >= AMBA_NUM_GPIO_PIN) {
        RetVal = GPIO_ERR_ARG;
    } else {
        AmbaCSL_GpioWriteEndOfInt(PinGroup, RegBitPosition);
    }

    return RetVal;
}
