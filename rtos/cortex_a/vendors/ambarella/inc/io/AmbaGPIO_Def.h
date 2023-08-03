/**
 *  @file AmbaGPIO_Def.h
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
 *
 *  @details Common Definitions & Constants for GPIO Middleware APIs
 *
 */

#ifndef AMBA_GPIO_DEF_H
#define AMBA_GPIO_DEF_H

#ifndef AMBA_TYPES_H
#include "AmbaTypes.h"
#endif

#ifndef AMBA_ERROR_CODE_H
#include "AmbaErrorCode.h"
#endif

#ifndef AMBA_GPIO_PRIV_H
#include "AmbaGPIO_Priv.h"
#endif

#define GPIO_ERR_0000           (GPIO_ERR_BASE)             /* Invalid argument */
#define GPIO_ERR_0001           (GPIO_ERR_BASE + 0x1U)      /* Unable to do concurrency protection */
#define GPIO_ERR_0002           (GPIO_ERR_BASE + 0x2U)      /* Unexpected pin fuction */
#define GPIO_ERR_0003           (GPIO_ERR_BASE + 0x3U)      /* Unexpected IOCTL get_linehandle fuction */
#define GPIO_ERR_0004           (GPIO_ERR_BASE + 0x4U)      /* Unexpected IOCTL set_linehandle fuction */
#define GPIO_ERR_00FF           (GPIO_ERR_BASE + 0XFFU)     /* Unexpected error */

/* GPIO error values */
#define GPIO_ERR_NONE           OK
#define GPIO_ERR_ARG            GPIO_ERR_0000
#define GPIO_ERR_MUTEX          GPIO_ERR_0001
#define GPIO_ERR_FUNC           GPIO_ERR_0002
#define GPIO_ERR_GET_HANDLE     GPIO_ERR_0003
#define GPIO_ERR_SET_HANDLE     GPIO_ERR_0004
#define GPIO_ERR_UNEXPECTED     GPIO_ERR_00FF

/* GPIO interrupt control */
#define GPIO_INT_NO_INTERRUPT           (0U)
#define GPIO_INT_RISING_EDGE_TRIGGER    (1U)
#define GPIO_INT_FALLING_EDGE_TRIGGER   (2U)
#define GPIO_INT_BOTH_EDGE_TRIGGER      (3U)
#define GPIO_INT_HIGH_LEVEL_TRIGGER     (4U)
#define GPIO_INT_LOW_LEVEL_TRIGGER      (5U)

/*
 * GPIO line status structures
 */
#define GPIO_FUNC_GPI                   (0U)    /* General purpose input */
#define GPIO_FUNC_GPO                   (1U)    /* General purpose output */
#define GPIO_FUNC_ALT1                  (2U)    /* Alternate function 1 */
#define GPIO_FUNC_ALT2                  (3U)    /* Alternate function 2 */
#define GPIO_FUNC_ALT3                  (4U)    /* Alternate function 3 */
#define GPIO_FUNC_ALT4                  (5U)    /* Alternate function 4 */
#define GPIO_FUNC_ALT5                  (6U)    /* Alternate function 5 */
#define GPIO_FUNC_ALT6                  (7U)    /* Alternate function 6 */
#define GPIO_FUNC_ALT7                  (8U)    /* Alternate function 7 */
#define NUM_GPIO_FUNC                   (9U)

#define GPIO_LV_UNKNOWN                 (0U)    /* Pin level state is unknown */
#define GPIO_LV_LOW                     (1U)    /* Pin level state is low */
#define GPIO_LV_HIGH                    (2U)    /* Pin level state is high */

#define GPIO_PULL_DISABLE               (0U)    /* Disable Internal Pull Control */
#define GPIO_PULL_UP                    (1U)    /* Internal Pull Up */
#define GPIO_PULL_DOWN                  (2U)    /* Internal Pull Down */

#define GPIO_DRV_STR_L0                 (0U)    /* Drive strength level 0: weakest */
#define GPIO_DRV_STR_L1                 (1U)    /* Drive strength level 1: weaker */
#define GPIO_DRV_STR_L2                 (2U)    /* Drive strength level 2: stronger */
#define GPIO_DRV_STR_L3                 (3U)    /* Drive strength level 3: strongest */

/* Legacy definitions */
#define AMBA_GPIO_FUNC_GPI              GPIO_FUNC_GPI
#define AMBA_GPIO_FUNC_GPO              GPIO_FUNC_GPO
#define AMBA_GPIO_FUNC_ALT1             GPIO_FUNC_ALT1
#define AMBA_GPIO_FUNC_ALT2             GPIO_FUNC_ALT2
#define AMBA_GPIO_FUNC_ALT3             GPIO_FUNC_ALT3
#define AMBA_GPIO_FUNC_ALT4             GPIO_FUNC_ALT4
#define AMBA_GPIO_FUNC_ALT5             GPIO_FUNC_ALT5
#define AMBA_GPIO_FUNC_ALT6             GPIO_FUNC_ALT6
#define AMBA_GPIO_FUNC_ALT7             GPIO_FUNC_ALT7
#define AMBA_NUM_GPIO_FUNC              NUM_GPIO_FUNC

#define AMBA_GPIO_LEVEL_UNKNOWN         GPIO_LV_UNKNOWN
#define AMBA_GPIO_LEVEL_LOW             GPIO_LV_LOW
#define AMBA_GPIO_LEVEL_HIGH            GPIO_LV_HIGH

#define AMBA_GPIO_PULL_DISABLE          GPIO_PULL_DISABLE
#define AMBA_GPIO_PULL_UP               GPIO_PULL_UP
#define AMBA_GPIO_PULL_DOWN             GPIO_PULL_DOWN

#define AMBA_GPIO_DRIVE_STR_WEAKEST     GPIO_DRV_STR_L0
#define AMBA_GPIO_DRIVE_STR_WEAK        GPIO_DRV_STR_L1
#define AMBA_GPIO_DRIVE_STR_STRONG      GPIO_DRV_STR_L2
#define AMBA_GPIO_DRIVE_STR_STRONGEST   GPIO_DRV_STR_L3

typedef void (*AMBA_GPIO_ISR_f)(UINT32 GpioPinID, UINT32 UserArg);

typedef struct {
    UINT32  PinState;
    UINT32  PinFunc;
    UINT32  PinDriveStrength;
    UINT32  PinPullUpOrDown;
    UINT32  IntType;
    AMBA_GPIO_ISR_f IntFunc;
} AMBA_GPIO_INFO_s;

typedef struct {
    UINT32  PinFuncRegVal[3];               /* GPIO or alternate function */
    UINT32  PinIoTypeRegVal;                /* Input or output */
    UINT32  PinStateProtectRegVal;          /* Read-only or read-writeable */
    UINT32  PinStateRegVal;                 /* Logical zero or logical one, only valid when the pin is output */
    UINT32  PinPullFuncEnableRegVal;        /* Disable/enable internal resistance */
    UINT32  PinPullFuncTypeRegVal;          /* Internal resistance is pull-down or pull-up */
    UINT32  PinDriverStrengthRegVal[2];     /* Driving strength is 2mA/4mA/8mA/12mA */
} AMBA_GPIO_PIN_GROUP_CONFIG_s;

typedef struct {
    AMBA_GPIO_PIN_GROUP_CONFIG_s PinGroupRegVal[AMBA_NUM_GPIO_GROUP];
} AMBA_GPIO_DEFAULT_s;

static inline UINT32 GPIO_SET_PIN_FUNC(UINT32 FuncNo, UINT32 PinNo)
{
    return ((FuncNo << 12U) | PinNo);
}
static inline UINT32 GPIO_GET_ALT_FUNC(UINT32 PinNo)
{
    return (((PinNo) >> 12U) & 0xfU);
}

#endif /* AMBA_GPIO_DEF_H */
