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
#include "hw/gpio.h"
#include "Generic.h"

/**
 *  AmbaGPIO_DrvEntry - Initializes driver-wide data structures and resources
 *  @return error code
 *  @note this function is an internal using only API
 */
UINT32 AmbaGPIO_DrvEntry(void)
{
    UINT32 RetVal = GPIO_ERR_NONE;

    return RetVal;
}

/**
 *  AmbaGPIO_LoadDefaultRegVals - Configure every GPIO pin as the desired function by default
 *  @param[in] pDefaultParam Preference setting for every GPIO pin
 *  @return error code
 */
UINT32 AmbaGPIO_LoadDefaultRegVals(const AMBA_GPIO_DEFAULT_s * pDefaultParam)
{
    UINT32 RetVal = GPIO_ERR_NONE;

    (void)pDefaultParam;

    return RetVal;
}

/**
 *  AmbaGPIO_SetFuncGPI - Configure GPIO pin as general purpose input function
 *  @param[in] PinID A gpio pin id
 *  @return error code
 */
UINT32 AmbaGPIO_SetFuncGPI(UINT32 PinID)
{
    int fd, err = 0;
    gpio_pin_t PinCtrl;
    UINT32 RetVal = GPIO_ERR_NONE;

    if (PinID >= AMBA_NUM_GPIO_PIN) {
        RetVal = GPIO_ERR_ARG;
    } else {
        fd = open("/dev/gpio", O_RDWR);
        if (fd == -1) {
            RetVal = GPIO_ERR_UNEXPECTED;
        } else {
            PinCtrl.PinId = PinID;

            err = devctl(fd, DCMD_GPIO_SET_GPI, &PinCtrl, sizeof(gpio_pin_t), NULL);
            if (err) {
                RetVal = GPIO_ERR_UNEXPECTED;
            }

            close(fd);
        }
    }

    return RetVal;
}

/**
 *  AmbaGPIO_SetFuncGPO - Configure GPIO pin as general purpose output function
 *  @param[in] PinID A gpio pin id
 *  @param[in] PinLevel Logical output level of the gpio pin
 *  @return error code
 */
UINT32 AmbaGPIO_SetFuncGPO(UINT32 PinID, UINT32 PinLevel)
{
    UINT32 PinNo = PinID & 0xfffU;
    UINT32 RetVal = GPIO_ERR_NONE;
    int fd, err = 0;
    gpio_pin_t PinCtrl;

    if (PinNo >= AMBA_NUM_GPIO_PIN) {
        RetVal = GPIO_ERR_ARG;
    } else {
        fd = open("/dev/gpio", O_RDWR);
        if (fd == -1) {
            RetVal = GPIO_ERR_UNEXPECTED;
        } else {
            PinCtrl.PinId = PinNo;
            PinCtrl.PinValue = PinLevel;

            err = devctl(fd, DCMD_GPIO_SET_GPO, &PinCtrl, sizeof(gpio_pin_t), NULL);
            if (err) {
                RetVal = GPIO_ERR_UNEXPECTED;
            }

            close(fd);
        }
    }

    return RetVal;
}

/**
 *  AmbaGPIO_SetFuncAlt - Configure the specified pin as an alternate function
 *  @param[in] PinID A gpio pin id with selected alternate function
 *  @return error code
 */
UINT32 AmbaGPIO_SetFuncAlt(UINT32 PinID)
{
    UINT32 PinNo = PinID & 0xfffU;
    UINT32 RetVal = GPIO_ERR_NONE;
    int fd, err = 0;
    gpio_pin_t PinCtrl;

    if (PinNo >= AMBA_NUM_GPIO_PIN) {
        RetVal = GPIO_ERR_ARG;
    } else {
        fd = open("/dev/gpio", O_RDWR);
        if (fd == -1) {
            RetVal = GPIO_ERR_UNEXPECTED;
        } else {
            PinCtrl.PinId = PinID;

            err = devctl(fd, DCMD_GPIO_SET_FUNC, &PinCtrl, sizeof(gpio_pin_t), NULL);
            if (err) {
                RetVal = GPIO_ERR_UNEXPECTED;
            }

            close(fd);
        }
    }

    return RetVal;
}

/**
 *  AmbaGPIO_SetDriveStrength - Set drive strength of the specified pin
 *  @param[in] PinID A gpio pin id
 *  @param[in] DriveStrength Output driving strength of the gpio pin
 *  @return error code
 */
UINT32 AmbaGPIO_SetDriveStrength(UINT32 PinID, UINT32 DriveStrength)
{
    UINT32 RetVal = GPIO_ERR_NONE;

    (void)PinID;
    (void)DriveStrength;

    return RetVal;
}

/**
 *  AmbaGPIO_SetPullUpOrDown - Disables/Enables the pull-up/pull-down function of the specified pin
 *  @param[in] PinID A gpio pin id
 *  @param[in] PullUpOrDown Pull Up/Down control of the gpio pin
 *  @return error code
 */
UINT32 AmbaGPIO_SetPullUpOrDown(UINT32 PinID, UINT32 PullUpOrDown)
{
    UINT32 PinNo = PinID & 0xfffU;
    UINT32 RetVal = GPIO_ERR_NONE;
    int fd, err = 0;
    gpio_pin_t PinCtrl;

    if (PinNo >= AMBA_NUM_GPIO_PIN) {
        RetVal = GPIO_ERR_ARG;
    } else {
        fd = open("/dev/gpio", O_RDWR);
        if (fd == -1) {
            RetVal = GPIO_ERR_UNEXPECTED;
        } else {
            PinCtrl.PinId = PinID;
            PinCtrl.PinValue = PullUpOrDown;

            err = devctl(fd, DCMD_GPIO_SET_PULL, &PinCtrl, sizeof(gpio_pin_t), NULL);
            if (err) {
                RetVal = GPIO_ERR_UNEXPECTED;
            }

            close(fd);
        }
    }

    return RetVal;
}

/**
 *  AmbaGPIO_IntSetType - Configures the interrupt generation type
 *  @param[in] PinID A gpio pin id
 *  @param[in] IntConfig Interrupt generation type
 *  @return error code
 */
UINT32 AmbaGPIO_IntSetType(UINT32 PinID, UINT32 IntConfig)
{
    UINT32 RetVal = GPIO_ERR_NONE;

    (void)PinID;
    (void)IntConfig;

    return RetVal;
}

/**
 *  AmbaGPIO_IntHookHandler - Register an interrupt notifier for the specified pin
 *  @param[in] PinID A gpio pin id
 *  @param[in] IntFunc Interrupt service routine
 *  @param[in] IntFuncArg Optional argument of the interrupt service routine
 *  @return error code
 */
UINT32 AmbaGPIO_IntHookHandler(UINT32 PinID, AMBA_GPIO_ISR_f IntFunc, UINT32 IntFuncArg)
{
    UINT32 RetVal = GPIO_ERR_NONE;

    (void)PinID;
    (void)IntFunc;
    (void)IntFuncArg;

    return RetVal;
}

/**
 *  AmbaGPIO_IntEnable - Enable interrupt of the specified pin
 *  @param[in] PinID A gpio pin id
 *  @return error code
 */
UINT32 AmbaGPIO_IntEnable(UINT32 PinID)
{
    UINT32 RetVal = GPIO_ERR_NONE;

    (void)PinID;

    return RetVal;
}

/**
 *  AmbaGPIO_IntDisable - Disable interrupt of the specified pin
 *  @param[in] PinID A gpio pin id
 *  @return error code
 */
UINT32 AmbaGPIO_IntDisable(UINT32 PinID)
{
    UINT32 RetVal = GPIO_ERR_NONE;

    (void)PinID;

    return RetVal;
}

/**
 *  AmbaGPIO_GetInfo - Get the pin state and configuration
 *  @param[in] PinID A gpio pin id
 *  @param[out] pPinInfo Configuration of the gpio pin
 *  @return error code
 */
UINT32 AmbaGPIO_GetInfo(UINT32 PinID, AMBA_GPIO_INFO_s *pPinInfo)
{
    UINT32 PinNo = PinID & 0xfffU;
    UINT32 RetVal = GPIO_ERR_NONE;
    int fd, err = 0;
    gpio_pin_info_t PinInfo;

    if (PinNo >= AMBA_NUM_GPIO_PIN) {
        RetVal = GPIO_ERR_ARG;
    } else {
        fd = open("/dev/gpio", O_RDWR);
        if (fd == -1) {
            RetVal = GPIO_ERR_UNEXPECTED;
        } else {
            PinInfo.PinId = PinID;

            err = devctl(fd, DCMD_GPIO_GET_INFO, &PinInfo, sizeof(gpio_pin_info_t), NULL);
            if (err) {
                RetVal = GPIO_ERR_UNEXPECTED;
            } else {
                pPinInfo->PinState = PinInfo.PinInfo.PinState;
                pPinInfo->PinFunc = PinInfo.PinInfo.PinFunc;
                pPinInfo->PinDriveStrength = PinInfo.PinInfo.PinDriveStrength;
                pPinInfo->PinPullUpOrDown = PinInfo.PinInfo.PinPullUpOrDown;
                pPinInfo->IntType = PinInfo.PinInfo.IntType;
                pPinInfo->IntFunc = PinInfo.PinInfo.IntFunc;
            }

            close(fd);
        }
    }

    return RetVal;
}

