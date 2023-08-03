/*
 * $QNXLicenseC:
 * Copyright 2010, 2018, QNX Software Systems.
 * Copyright 2013, Adeneo Embedded.
 * Copyright 2020, Ambarella International LP
 *
 * Licensed under the Apache License, Version 2.0 (the "License"). You
 * may not reproduce, modify or distribute this software except in
 * compliance with the License. You may obtain a copy of the License
 * at: http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" basis,
 * WITHOUT WARRANTIES OF ANY KIND, either express or implied.
 *
 * This file may contain contributions from others, either as
 * contributors under the License or as licensors under other terms.
 * Please review this entire file for other proprietary rights or license
 * notices, as well as the QNX Development Suite License Guide at
 * http://licensing.qnx.com/license-guide/ for other information.
 * $
 */

#include <stdint.h>
#include <sys/mman.h>
#include <hw/inout.h>
#include <stdio.h>
#include "Amba_gpio.h"

const AMBA_GPIO_DEFAULT_s GpioPinGrpConfig = {
    .PinGroupRegVal = {
        [AMBA_GPIO_GROUP0] = {
            .PinFuncRegVal = {
                [0] = 0xffffff00U,                  /* GPIO pin [31:0]: GPIO(0) or Alternate functions */
                [1] = 0x00000000U,                  /* GPIO pin [31:0]: GPIO(0) or Alternate functions */
                [2] = 0x00000000U,                  /* GPIO pin [31:0]: GPIO(0) or Alternate functions */
            },
            .PinIoTypeRegVal = 0x000000ffU,         /* GPIO pin [31:0]: Input(0) or Output(1) pin */
            .PinStateProtectRegVal = 0xffffffffU,   /* GPIO pin [31:0]: Read-only(0) or Read-writeable(1) pin state */
            .PinStateRegVal = 0x00000000U,          /* GPIO pin [31:0]: Low(0) or High(1) pin state */
            .PinPullFuncEnableRegVal = 0x00000000U,
            .PinPullFuncTypeRegVal = 0x00000000U,
            .PinDriverStrengthRegVal = {
                [0] = 0xffffffffU,
                [1] = 0x00000000U
            }
        },

        [AMBA_GPIO_GROUP1] = {
            .PinFuncRegVal = {
                [0] = 0x003fffffU,                  /* GPIO pin [63:32]: GPIO(0) or Alternate functions */
                [1] = 0x00000000U,                  /* GPIO pin [63:32]: GPIO(0) or Alternate functions */
                [2] = 0xffc00000U,                  /* GPIO pin [63:32]: GPIO(0) or Alternate functions */
            },
            .PinIoTypeRegVal = 0x00000000U,         /* GPIO pin [63:32]: Input(0) or Output(1) pin */
            .PinStateProtectRegVal = 0xffffffffU,   /* GPIO pin [63:32]: Read-only(0) or Read-writeable(1) pin state */
            .PinStateRegVal =  0x00000000U,         /* GPIO pin [63:32]: Low(0) or High(1) pin state */
            .PinPullFuncEnableRegVal = 0x00000000U,
            .PinPullFuncTypeRegVal = 0x00000000U,
            .PinDriverStrengthRegVal = {
                [0] = 0xffffffffU,
                [1] = 0x00000000U
            }
        },

#if defined(CONFIG_ENABLE_SPINOR_BOOT)
        [AMBA_GPIO_GROUP2] = {
            .PinFuncRegVal = {
                [0] = 0x00ffe180U,                  /* GPIO pin [95:64]: GPIO(0) or Alternate functions */
                [1] = 0xffffffc0U,                  /* GPIO pin [95:64]: GPIO(0) or Alternate functions */
                [2] = 0x0000003bU,                  /* GPIO pin [95:64]: GPIO(0) or Alternate functions */
            },
            .PinIoTypeRegVal = 0x00000004U,         /* GPIO pin [95:64]: Input(0) or Output(1) pin */
            .PinStateProtectRegVal = 0xffffffffU,   /* GPIO pin [95:64]: Read-only(0) or Read-writeable(1) pin state */
            .PinStateRegVal = 0x00000000U,          /* GPIO pin [95:64]: Low(0) or High(1) pin state */
            .PinPullFuncEnableRegVal = 0x00000000U,
            .PinPullFuncTypeRegVal = 0x00000000U,
            .PinDriverStrengthRegVal = {
                [0] = 0xffffffffU,
                [1] = 0x00000000U
            }
        },
#else
        [AMBA_GPIO_GROUP2] = {
            .PinFuncRegVal = {
                [0] = 0x00000000U,                  /* GPIO pin [95:64]: GPIO(0) or Alternate functions */
                [1] = 0xffffffc0U,                  /* GPIO pin [95:64]: GPIO(0) or Alternate functions */
                [2] = 0x0000003bU,                  /* GPIO pin [95:64]: GPIO(0) or Alternate functions */
            },
            .PinIoTypeRegVal = 0x00000004U,         /* GPIO pin [95:64]: Input(0) or Output(1) pin */
            .PinStateProtectRegVal = 0xffffffffU,   /* GPIO pin [95:64]: Read-only(0) or Read-writeable(1) pin state */
            .PinStateRegVal = 0x00000000U,          /* GPIO pin [95:64]: Low(0) or High(1) pin state */
            .PinPullFuncEnableRegVal = 0x00000000U,
            .PinPullFuncTypeRegVal = 0x00000000U,
            .PinDriverStrengthRegVal = {
                [0] = 0xffffffffU,
                [1] = 0x00000000U
            }
        },
#endif
        [AMBA_GPIO_GROUP3] = {
            .PinFuncRegVal = {
                [0] = 0xfff98000U,                  /* GPIO pin [127:96]: GPIO(0) or Alternate functions */
                [1] = 0x00005fffU,                  /* GPIO pin [127:96]: GPIO(0) or Alternate functions */
                [2] = 0x00000000U,                  /* GPIO pin [127:96]: GPIO(0) or Alternate functions */
            },
            .PinIoTypeRegVal = 0x00020000U,         /* GPIO pin [127:96]: Input(0) or Output(1) pin */
            .PinStateProtectRegVal = 0xffffffffU,   /* GPIO pin [127:96]: Read-only(0) or Read-writeable(1) pin state */
            .PinStateRegVal = 0x00002000U,          /* GPIO pin [127:96]: Low(0) or High(1) pin state */
            .PinPullFuncEnableRegVal = 0x00000007U,
            .PinPullFuncTypeRegVal = 0x00000007U,
            .PinDriverStrengthRegVal = {
                [0] = 0xffffffffU,
                [1] = 0x00000000U
            }
        },

#if defined(CONFIG_SOC_CV2)
        [AMBA_GPIO_GROUP4] = {
            .PinFuncRegVal = {
                [0] = 0x2fffffffU,                  /* GPIO pin [159:128]: GPIO(0) or Alternate functions */
                [1] = 0x00000000U,                  /* GPIO pin [159:128]: GPIO(0) or Alternate functions */
                [2] = 0x00000000U,                  /* GPIO pin [159:128]: GPIO(0) or Alternate functions */
            },
            .PinIoTypeRegVal = 0xd0000000U,         /* GPIO pin [159:128]: Input(0) or Output(1) pin */
            .PinStateProtectRegVal = 0xffffffffU,   /* GPIO pin [159:128]: Read-only(0) or Read-writeable(1) pin state */
            .PinStateRegVal = 0x00000000U,          /* GPIO pin [159:128]: Low(0) or High(1) pin state */
            .PinPullFuncEnableRegVal = 0x00000000U,
            .PinPullFuncTypeRegVal = 0x00000000U,
            .PinDriverStrengthRegVal = {
                [0] = 0xffffffffU,
                [1] = 0x00000000U
            }
        }
#endif
    }
};

unsigned int AMBA_GPIO_PhyAddr[AMBA_NUM_GPIO_GROUP] = {
    AMBA_GPIO0_BASE,
    AMBA_GPIO1_BASE,
    AMBA_GPIO2_BASE,
    AMBA_GPIO3_BASE,
#if defined(CONFIG_SOC_CV2)
    AMBA_GPIO4_BASE,
    AMBA_GPIO5_BASE
#endif
};

static uintptr_t gpio_virt_base;
static uintptr_t IoMux_virt_base;
AMBA_GPIO_REG_s * pAmbaGPIO_Reg[AMBA_NUM_GPIO_GROUP] = {0U};
AMBA_IO_MUX_REG_s * pAmbaIOMUX_Reg;
AMBA_RCT_REG_s * pAmbaRCT_Reg;
#if defined(CONFIG_SOC_CV2FS) || defined(CONFIG_SOC_CV22FS)
AMBA_SCRATCHPAD_NS_REG_s * pAmbaScratchpadNS_Reg = (AMBA_SCRATCHPAD_NS_REG_s *)AMBA_CA53_SCRATCHPAD_NS_BASE_ADDR;
#else
AMBA_MISC_REG_s * pAmbaMISC_Reg;
#endif

int Amba_Gpio_Init(void)
{
#if defined(CONFIG_SOC_CV2FS) || defined(CONFIG_SOC_CV22FS)
    unsigned char i = 0;
    uintptr_t virt_base;

    for (i = 0; i < AMBA_NUM_GPIO_GROUP; i++) {
        gpio_virt_base = mmap_device_io(AMBA_GPIO_REGISTER_SIZE, AMBA_GPIO_PhyAddr[i]);
        if (gpio_virt_base == (uintptr_t) MAP_FAILED) {
            perror("mmap_device_io");
            return -1;
        }
        pAmbaGPIO_Reg[i] = (AMBA_GPIO_REG_s *)(gpio_virt_base);
    }

    IoMux_virt_base = mmap_device_io(AMBA_GPIO_REGISTER_SIZE, AMBA_CA53_IOMUX_BASE_ADDR);
    if (IoMux_virt_base == (uintptr_t) MAP_FAILED) {
        perror("mmap_device_io");
        return -1;
    }
    pAmbaIOMUX_Reg = (AMBA_IO_MUX_REG_s *)IoMux_virt_base;

    virt_base = mmap_device_io(AMBA_RCT_SIZE, AMBA_RCT_BASE);
    if (virt_base == (uintptr_t) MAP_FAILED) {
        perror("mmap_device_io");
        return -1;
    }
    pAmbaRCT_Reg = (AMBA_RCT_REG_s *)virt_base;

    virt_base = mmap_device_io(0x1000, AMBA_CA53_SCRATCHPAD_NS_BASE_ADDR);
    if (virt_base == (uintptr_t) MAP_FAILED) {
        perror("mmap_device_io");
        return -1;
    }
    pAmbaScratchpadNS_Reg = (AMBA_SCRATCHPAD_NS_REG_s *)virt_base;
#else
    unsigned char i = 0;
    uintptr_t virt_base;

    for (i = 0; i < AMBA_NUM_GPIO_GROUP; i++) {
        gpio_virt_base = mmap_device_io(AMBA_GPIO_REGISTER_SIZE, AMBA_GPIO_PhyAddr[i]);
        if (gpio_virt_base == (uintptr_t) MAP_FAILED) {
            perror("mmap_device_io");
            return -1;
        }
        pAmbaGPIO_Reg[i] = (AMBA_GPIO_REG_s *)(gpio_virt_base);
    }

    IoMux_virt_base = mmap_device_io(AMBA_GPIO_REGISTER_SIZE, AMBA_CORTEX_A53_IO_MUX_BASE_ADDR);
    if (IoMux_virt_base == (uintptr_t) MAP_FAILED) {
        perror("mmap_device_io");
        return -1;
    }
    pAmbaIOMUX_Reg = (AMBA_IO_MUX_REG_s *)IoMux_virt_base;

    virt_base = mmap_device_io(AMBA_GPIO_REGISTER_SIZE, AMBA_CORTEX_A53_MISC_BASE_ADDR);
    if (virt_base == (uintptr_t) MAP_FAILED) {
        perror("mmap_device_io");
        return -1;
    }
    pAmbaMISC_Reg = (AMBA_MISC_REG_s *)virt_base;

    virt_base = mmap_device_io(AMBA_RCT_SIZE, AMBA_RCT_BASE);
    if (virt_base == (uintptr_t) MAP_FAILED) {
        perror("mmap_device_io");
        return -1;
    }
    pAmbaRCT_Reg = (AMBA_RCT_REG_s *)virt_base;
#endif
    return 0;
}

void Amba_Gpio_Fini(void)
{
    unsigned int i = 0;

    for (i = 0; i < AMBA_NUM_GPIO_GROUP; i++) {
        munmap_device_io(AMBA_GPIO_PhyAddr[i], AMBA_GPIO_REGISTER_SIZE);
    }
#if defined(CONFIG_SOC_CV2FS) || defined(CONFIG_SOC_CV22FS)
    munmap_device_io((uintptr_t)pAmbaIOMUX_Reg, AMBA_GPIO_REGISTER_SIZE);
    munmap_device_io((uintptr_t)pAmbaRCT_Reg, AMBA_RCT_SIZE);
#else
    munmap_device_io((uintptr_t)AMBA_CORTEX_A53_IO_MUX_BASE_ADDR, AMBA_GPIO_REGISTER_SIZE);
    munmap_device_io((uintptr_t)AMBA_CORTEX_A53_MISC_BASE_ADDR, AMBA_GPIO_REGISTER_SIZE);
    munmap_device_io((uintptr_t)AMBA_RCT_BASE, AMBA_RCT_SIZE);
#endif
}

int Amba_Gpio_Set_Direction(unsigned int pin_number, unsigned int dir)
{
    if (dir) { /* Output Direction */
        AmbaRTSL_GpioSetFuncGPO(pin_number, 0x0);
    } else { /* Input Direction */
        AmbaRTSL_GpioSetFuncGPI(pin_number);
    }

    return 0;
}

int Amba_Gpio_Get_Direction(unsigned int pin_number)
{
    AMBA_GPIO_INFO_s PinInfo = {0};
    int Ret = 0;

    AmbaRTSL_GpioGetPinInfo(pin_number, &PinInfo);

    if (PinInfo.PinFunc == AMBA_GPIO_FUNC_GPI) {
        Ret = 0;
    } else {
        Ret = 1;
    }

    return Ret;
}

int Amba_Gpio_Set_Output_Enable(unsigned int pin_number, unsigned int value)
{
    AmbaRTSL_GpioSetFuncGPO((unsigned int)pin_number, value);

    return 0;
}

int Amba_Gpio_Get_Output_Enable(unsigned int pin_number)
{
    AMBA_GPIO_INFO_s PinInfo = {0};
    int Ret = 0;

    AmbaRTSL_GpioGetPinInfo(pin_number, &PinInfo);

    if (PinInfo.PinFunc == AMBA_GPIO_FUNC_GPO) {
        Ret = 1;
    } else {
        Ret = 0;
    }

    return Ret;
}

int Amba_Gpio_Get_Input(unsigned int pin_number)
{
    AMBA_GPIO_INFO_s PinInfo = {0};
    int Ret = 0;

    AmbaRTSL_GpioGetPinInfo(pin_number, &PinInfo);

    if (PinInfo.PinFunc == AMBA_GPIO_FUNC_GPI) {
        Ret = 1;
    } else {
        Ret = 0;
    }

    return Ret;
}

int Amba_Gpio_Get_Irq_Type(unsigned int pin_number)
{
    AMBA_GPIO_INFO_s PinInfo = {0};

    AmbaRTSL_GpioGetPinInfo(pin_number, &PinInfo);

    return PinInfo.IntType;
}

int Amba_Gpio_Set_Irq_Type(unsigned int pin_number, unsigned int irq_type)
{
    AmbaRTSL_GpioIntSetType(pin_number, irq_type);

    return 0;
}

int Amba_Gpio_Irq_Clear(unsigned int pin_number)
{

    AmbaRTSL_GpioIntClear(pin_number);

    return 0;
}

int Amba_Gpio_Get_Irq_Enable(unsigned int pin_number)
{
    AMBA_GPIO_INFO_s PinInfo = {0};
    int Ret = 0;

    AmbaRTSL_GpioGetPinInfo(pin_number, &PinInfo);

    if (PinInfo.IntType != GPIO_INT_NO_INTERRUPT) {
        Ret = 1;
    }

    return Ret;
}

int Amba_Gpio_Irq_Enable(unsigned int pin_number)
{

    AmbaRTSL_GpioIntEnable(pin_number);

    return 0;
}

int Amba_Gpio_Irq_Disable(unsigned int pin_number)
{
    AmbaRTSL_GpioIntDisable(pin_number);

    return 0;
}

int Amba_Get_Pin_Pull_Enable(unsigned int pin_number)
{
    AMBA_GPIO_INFO_s PinInfo = {0};
    int Ret = 0;

    AmbaRTSL_GpioGetPinInfo(pin_number, &PinInfo);

    if (PinInfo.PinPullUpOrDown != GPIO_PULL_DISABLE) {
        Ret = 1;
    }

    return Ret;
}

int Amba_Get_Pin_Pull_UpDown(unsigned int pin_number)
{
    AMBA_GPIO_INFO_s PinInfo = {0};
    int Ret = 0;

    AmbaRTSL_GpioGetPinInfo(pin_number, &PinInfo);

    if (PinInfo.PinPullUpOrDown == GPIO_PULL_UP) {
        Ret = 1;
    } else if (PinInfo.PinPullUpOrDown == GPIO_PULL_DOWN) {
        Ret = 0;
    } else {
        Ret = -1;
    }

    return Ret;

}

unsigned int Amba_Get_Pin_Func(unsigned int pin_number)
{
    AMBA_GPIO_INFO_s PinInfo = {0};

    AmbaRTSL_GpioGetPinInfo(pin_number, &PinInfo);

    return (int)PinInfo.PinFunc;
}

int Amba_Set_Pin_Func(unsigned int pin_number)
{
    AmbaRTSL_GpioSetFuncAlt(pin_number);

    return 0;
}

int Amba_Set_Pin_Pull_UpDown(unsigned int pin_number, unsigned int up_down)
{
    AmbaRTSL_GpioSetPullUpOrDown(pin_number, up_down);

    return 0;
}

