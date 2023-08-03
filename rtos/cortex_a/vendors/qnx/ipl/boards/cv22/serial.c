/*
 * $QNXLicenseC:
 * Copyright 2018, QNX Software Systems.
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

#include "ipl.h"
#include <hw/inout.h>
#include "ipl_ambarella.h"

unsigned char amba_uart_pollkey();
unsigned char amba_uart_getchar();
void amba_uart_putchar(unsigned char);

static const ser_dev amba_dev = {
        amba_uart_getchar,
        amba_uart_putchar,
        amba_uart_pollkey 
        };

unsigned char amba_uart_pollkey(void)
{
    if (AMBA_CONSOLE_SR & AMBA_SR_READ_STATUS_MASK)
        return 1;
    else
        return 0;
}

unsigned char amba_uart_getchar(void)
{
    return ((unsigned char) AMBA_CONSOLE_FIFO);
}

void UartGetTxEmpty(unsigned int *pTxEmpty)
{
    AMBA_UART_REG_s *pUartReg;
    pUartReg = (AMBA_UART_REG_s *)AMBA_CONSOLE_BASE;

    if ((pUartReg->LineStatus & 0x40U) != 0UL) {
        pUartReg->ResetCtrl = 4U;
        *pTxEmpty = 1UL;
    } else {
        *pTxEmpty = 0UL;
    }
}

void amba_uart_putchar(unsigned char data1)
{
    unsigned int TxEmpty = 0;
    do {
        (void)UartGetTxEmpty(&TxEmpty);
    } while (TxEmpty == 0U);
    AMBA_CONSOLE_FIFO = (unsigned short) data1;
}

void AMBA_init_serial()
{
    AMBA_UART_REG_s *pUartReg;
    unsigned int delay = 0xffff;
    unsigned int Divider = 0;

    unsigned int *pTmp;

    pTmp = (unsigned int *) (0Xed0801c8);
    *pTmp = 0U;
    pTmp = (unsigned int *) (0Xed080038);
    *pTmp = 1U;

    pUartReg = (AMBA_UART_REG_s *)AMBA_CONSOLE_BASE;

    pUartReg->IntCtrl = 0U;

    /* reset UART */
    pUartReg->ResetCtrl = 0x1U;   /* Reset UART */
    while (delay > 0) {
        (delay --);
    }
    pUartReg->ResetCtrl = 0x0U;

    Divider = AMBA_CONSOLE_BR << 4U;
    Divider = (UART_FREQ + (Divider >> 1U)) / Divider;
    /* enable Divisor Latch Access */
    pUartReg->LineCtrl.DLAB = 0x1UL;

    /* set Divisor Latch value (16-bit) */
    pUartReg->DataBuf = Divider & 0xffU;
    pUartReg->IntCtrl = (Divider >> 8U) & 0xffU;

    /* disable Divisor Latch Access */
    pUartReg->LineCtrl.DLAB = 0UL;

    pUartReg->LineCtrl.DataBits = 3U;
    pUartReg->LineCtrl.StopBits = 0U;
    pUartReg->LineCtrl.ParityEnable = 0U;
    pUartReg->LineCtrl.EvenParity = 0U;

    pUartReg->FifoCtrl = 0x31U;
    pUartReg->ResetCtrl = 6U;
    /* enable all interrupts except Tx FIFO empty */
    pUartReg->IntCtrl = 0x3dU;
    pUartReg->DmaCtrl &= ~0x1U;;

    init_serdev((ser_dev *)&amba_dev);
}

/*-----------------------------------------------------------------------------------------------*\
 * IOMUX: Pin Function Selection Register
\*-----------------------------------------------------------------------------------------------*/
typedef struct {
    volatile unsigned int     FuncSelect0;    /* bit[0] of function selection */
    volatile unsigned int     FuncSelect1;    /* bit[1] of function selection */
    volatile unsigned int     FuncSelect2;    /* bit[2] of function selection */
} AMBA_IO_MUX_PIN_FUNC_REG_s;
/*-----------------------------------------------------------------------------------------------*\
 * IOMUX: Register Taken Effect Control Register
\*-----------------------------------------------------------------------------------------------*/
typedef struct {
    volatile unsigned int  Update:             1;      /* [0] 1 = Make pin function selection registers take effect */
    volatile unsigned int  Reserved:           31;     /* [31:1] Reserved */
} AMBA_IO_MUX_REG_UPDATE_REG_s;
/*-----------------------------------------------------------------------------------------------*\
 * IOMUX: All Registers
\*-----------------------------------------------------------------------------------------------*/
typedef struct {
    AMBA_IO_MUX_PIN_FUNC_REG_s              PinGroup[6];    /* 0x000-0x044(RW): IOMUX Set 0-5 (GPIO 0-31,32-63,64-95,96-127,128-159,160-191) Pin Function Selection */
    volatile unsigned int                   Reserved[42];   /* 0x048-0x0EC(RW): Reserved */
    volatile AMBA_IO_MUX_REG_UPDATE_REG_s   RegUpdate;      /* 0x0F0(WO): Register Taken Effect Control */
} AMBA_IO_MUX_REG_s;

/*-----------------------------------------------------------------------------------------------*\
 * GPIO: Global Enable Register
\*-----------------------------------------------------------------------------------------------*/
typedef struct {
    unsigned int  Enable:             1;       /* [0] 0 = Disabled, 1 = Enabled */
    unsigned int  Reserved:           31;      /* [31:1] Reserved */
} AMBA_GPIO_GLOBAL_ENABLE_REG_s;

/*-----------------------------------------------------------------------------------------------*\
 * GPIO: All Registers
\*-----------------------------------------------------------------------------------------------*/
typedef struct {
    volatile unsigned int                         PinLevel;       /* 0x000(RW): Pin Data Register */
    volatile unsigned int                         PinDirection;   /* 0x004(RW): Pin Direction Register */
    volatile unsigned int                         IntTrigType0;   /* 0x008(RW): Interrupt Sensitivity Control Register */
    volatile unsigned int                         IntTrigType1;   /* 0x00C(RW): Interrupt on both Rising and Falling Data Edge Register */
    volatile unsigned int                         IntTrigType2;   /* 0x010(RW): Interrupt on Data Low/High Level (Falling/Rising Edge) Register */
    volatile unsigned int                         IntEnable;      /* 0x014(RW): Interrupt Enable (Unmask) Control Register */
    volatile unsigned int                         PinAltFuncSel;  /* 0x018(RW): Pin Alternate Function Selection Register */
    volatile unsigned int                         IntStatusRaw;   /* 0x01C(RO): Raw Interrupt Status Register */
    volatile unsigned int                         IntStatus;      /* 0x020(RO): Masked Interrupt Status Register */
    volatile unsigned int                         IntClear;       /* 0x024(WO): Interrupt Clear Register */
    volatile unsigned int                         PinMask;        /* 0x028(RW): Pin Data Enable Register */
    volatile AMBA_GPIO_GLOBAL_ENABLE_REG_s  Enable;         /* 0x02C(WO): Enable Register */
} AMBA_GPIO_REG_s;

/*-----------------------------------------------------------------------------------------------*\
 * GPIO: All Registers
\*-----------------------------------------------------------------------------------------------*/
#define AMBA_GPIO_GROUP0                0x0U    /* GPIO pin 0~31    */
#define AMBA_GPIO_GROUP1                0x1U    /* GPIO pin 32~63   */
#define AMBA_GPIO_GROUP2                0x2U    /* GPIO pin 64~95   */
#define AMBA_GPIO_GROUP3                0x3U    /* GPIO pin 96~127  */
#define AMBA_GPIO_GROUP4                0x4U    /* GPIO pin 128~159 */
#define AMBA_NUM_GPIO_GROUP             0x5U

/*-----------------------------------------------------------------------------------------------*\
 * Non-Secure Scratchpad: All Registers
\*-----------------------------------------------------------------------------------------------*/
typedef struct {
    volatile unsigned int                             Reserved0[5];           /* 0x000-0x010: Reserved */
    volatile unsigned int                             GpioPullEnable[5];      /* 0x014-0x024(RW): Pull-Up/Pull-Down Enable Registers For GPIO Pins */
    volatile unsigned int                             Reserved2[2];           /* 0x028-0x02C: Reserved */
    volatile unsigned int                             GpioPullSelect[5];      /* 0x030-0x040(RW): Pull-Up/Pull-Down Function Select Registers For GPIO Pins */
} AMBA_SCRATCHPAD_NS_REG_s;

typedef struct {
    unsigned int  PinFuncRegVal[3];               /* GPIO or alternate function */
    unsigned int  PinIoTypeRegVal;                /* Input or output */
    unsigned int  PinStateProtectRegVal;          /* Read-only or read-writeable */
    unsigned int  PinStateRegVal;                 /* Logical zero or logical one, only valid when the pin is output */
    unsigned int  PinPullFuncEnableRegVal;        /* Disable/enable internal resistance */
    unsigned int  PinPullFuncTypeRegVal;          /* Internal resistance is pull-down or pull-up */
    unsigned int  PinDriverStrengthRegVal[2];     /* Driving strength is 2mA/4mA/8mA/12mA */
} AMBA_GPIO_PIN_GROUP_CONFIG_s;

typedef struct {
    AMBA_GPIO_PIN_GROUP_CONFIG_s PinGroupRegVal[6];
} AMBA_GPIO_DEFAULT_s;

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
    }
};

AMBA_IO_MUX_REG_s *const pAmbaIOMUX_Reg = (AMBA_IO_MUX_REG_s *)0xec000000;
AMBA_GPIO_REG_s *const pAmbaGPIO_Reg[AMBA_NUM_GPIO_GROUP] = {
(AMBA_GPIO_REG_s *)0xec003000,
(AMBA_GPIO_REG_s *)0xec004000,
(AMBA_GPIO_REG_s *)0xec005000,
(AMBA_GPIO_REG_s *)0xec006000,
(AMBA_GPIO_REG_s *)0xec007000,
};
AMBA_SCRATCHPAD_NS_REG_s *const pAmbaScratchpadNS_Reg = (AMBA_SCRATCHPAD_NS_REG_s *)0xe0022000;

static inline void AmbaCSL_GpioEnablePinGrp(unsigned int PinGroup)                        { pAmbaGPIO_Reg[PinGroup]->Enable.Enable = (0x1U); }
static inline void AmbaCSL_GpioSetPinGrpAltFunc(unsigned int PinGroup, unsigned int Val)        { pAmbaGPIO_Reg[PinGroup]->PinAltFuncSel = Val; }
static inline void AmbaCSL_GpioSetPinGrpDirection(unsigned int PinGroup, unsigned int Val)      { pAmbaGPIO_Reg[PinGroup]->PinDirection = Val; }
static inline void AmbaCSL_GpioSetPinGrpMask(unsigned int PinGroup, unsigned int Val)           { pAmbaGPIO_Reg[PinGroup]->PinMask = Val; }
static inline void AmbaCSL_GpioSetPinGrpState(unsigned int PinGroup, unsigned int Val)          { pAmbaGPIO_Reg[PinGroup]->PinLevel = Val; }
static inline void AmbaCSL_GpioClearPinGrpInt(unsigned int PinGroup)                      { pAmbaGPIO_Reg[PinGroup]->IntClear = (0xffffffffU); }

void AmbaCSL_GpioSetPinGrpFunc(unsigned int PinGroup, const unsigned int *pPinGroupFunc)
{
    AMBA_IO_MUX_PIN_FUNC_REG_s *pGpioGroup = &pAmbaIOMUX_Reg->PinGroup[PinGroup];

    pGpioGroup->FuncSelect0 = pPinGroupFunc[0];
    pGpioGroup->FuncSelect1 = pPinGroupFunc[1];
    pGpioGroup->FuncSelect2 = pPinGroupFunc[2];

    pAmbaIOMUX_Reg->RegUpdate.Update = 1U;
    pAmbaIOMUX_Reg->RegUpdate.Update = 0U;
}

/**
 *  AmbaCSL_GpioSetPinGrpPullCtrl - Set the pull up/down control for a group of GPIO pins
 *  @param[in] PinGroup GPIO group ID
 *  @param[in] PullEnable Enable pull up/down for group of pins
 *  @param[in] PullSelect Pull up/down selection for group of pins
 */
void AmbaCSL_GpioSetPinGrpPullCtrl(unsigned int PinGroup, unsigned int PullEnable, unsigned int PullSelect)
{
    pAmbaScratchpadNS_Reg->GpioPullEnable[PinGroup] = PullEnable;
    pAmbaScratchpadNS_Reg->GpioPullSelect[PinGroup] = PullSelect;
}

void AMBA_init_gpio()
{
    unsigned int i;
    const AMBA_GPIO_DEFAULT_s * pDefaultParam = &GpioPinGrpConfig;

    for (i = 0U; i < AMBA_NUM_GPIO_GROUP; i++) {
        AmbaCSL_GpioEnablePinGrp(i);
        AmbaCSL_GpioSetPinGrpAltFunc(i, 0U);
        AmbaCSL_GpioSetPinGrpFunc(i, pDefaultParam->PinGroupRegVal[i].PinFuncRegVal);
        AmbaCSL_GpioSetPinGrpDirection(i, pDefaultParam->PinGroupRegVal[i].PinIoTypeRegVal);
        AmbaCSL_GpioSetPinGrpMask(i, pDefaultParam->PinGroupRegVal[i].PinStateProtectRegVal);
        AmbaCSL_GpioSetPinGrpState(i, pDefaultParam->PinGroupRegVal[i].PinStateRegVal);
        AmbaCSL_GpioSetPinGrpPullCtrl(i, pDefaultParam->PinGroupRegVal[i].PinPullFuncEnableRegVal, pDefaultParam->PinGroupRegVal[i].PinPullFuncTypeRegVal);
        AmbaCSL_GpioClearPinGrpInt(i);
    }
}

