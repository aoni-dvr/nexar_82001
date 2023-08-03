/**
 *  @file AmbaGPIO.c
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
 *  @details GPIO Middleware APIs
 *
 */

#include "AmbaTypes.h"
#include "AmbaKAL.h"

#include "AmbaDrvEntry.h"
#include "AmbaGPIO.h"

#include <linux/gpio.h>
#if defined(CONFIG_ENABLE_AMBALINK)
//#include "AmbaIPC_Lock.h"
#endif

static AMBA_KAL_MUTEX_t AmbaGpioMutex[AMBA_NUM_GPIO_GROUP];

static UINT32 GPIO_Lock(UINT32 PinNo);
static UINT32 GPIO_Unlock(UINT32 PinNo);

static char *pDeviceName[AMBA_NUM_GPIO_GROUP] = {
    "/dev/gpiochip0",
    "/dev/gpio-1",
    "/dev/gpio-2",
    "/dev/gpio-3"
};


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
    (void) pDefaultParam;
    return RetVal;
}

/**
 *  AmbaGPIO_SetFuncGPI - Configure GPIO pin as general purpose input function
 *  @param[in] PinID A gpio pin id
 *  @return error code
 */
UINT32 AmbaGPIO_SetFuncGPI(UINT32 PinID)
{
    UINT32 RetVal = GPIO_ERR_NONE;
    UINT32 PinNo = PinID & 0xfffU;
    //UINT32 RegPinGroup = PinNo >> 5U;
    //UINT32 RegBitPosition = PinNo & 0x1fU;
    int fd, err = 0U;
    struct gpiohandle_request handlereq;

    if (PinNo >= AMBA_NUM_GPIO_PIN) {
        RetVal = GPIO_ERR_ARG;
    } else {
        RetVal = GPIO_Lock(PinNo);
        if (RetVal != GPIO_ERR_NONE) {
            RetVal = GPIO_ERR_MUTEX;
        } else {

            handlereq.lineoffsets[0] = PinNo;
            handlereq.flags = GPIOHANDLE_REQUEST_INPUT;
            handlereq.lines = 1U;

            strcpy(handlereq.consumer_label, "gpio");

            fd = open(pDeviceName[0], O_RDWR);
            if ( fd == -1 ) {
                RetVal = GPIO_ERR_UNEXPECTED;
            } else {
                err = ioctl(fd, GPIO_GET_LINEHANDLE_IOCTL, &handlereq);
                if (err) {
                    RetVal = GPIO_ERR_GET_HANDLE;
                }
                close(fd);
                close(handlereq.fd);
            }
        }

        if (GPIO_Unlock(PinNo) != GPIO_ERR_NONE) {
            RetVal = GPIO_ERR_MUTEX;
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
    UINT32 RetVal = GPIO_ERR_NONE;
    UINT32 PinNo = PinID & 0xfffU;
    //UINT32 RegPinGroup = PinNo >> 5U;
    //UINT32 RegBitPosition = PinNo & 0x1fU;
    int fd, err = 0;
    struct gpiohandle_request req;
    struct gpiohandle_data data;

    if (PinNo >= AMBA_NUM_GPIO_PIN) {
        RetVal = GPIO_ERR_ARG;
    } else {
        RetVal = GPIO_Lock(PinNo);
        if (RetVal != GPIO_ERR_NONE) {
            RetVal = GPIO_ERR_MUTEX;
        } else {
            req.lineoffsets[0] = PinNo;
            req.flags = GPIOHANDLE_REQUEST_OUTPUT;
            req.lines = 1U;
            if ( PinLevel == AMBA_GPIO_LEVEL_HIGH ) {
                data.values[0] = 1U;
            } else {
                data.values[0] = 0U;
            }

            memcpy(req.default_values, &data, sizeof(req.default_values));
            strcpy(req.consumer_label, "gpio");

            fd = open(pDeviceName[0], O_RDWR);
            if ( fd == -1 ) {
                RetVal = GPIO_ERR_UNEXPECTED;
            } else {
                err = ioctl(fd, GPIO_GET_LINEHANDLE_IOCTL, &req);
                if (err) {
                    RetVal = GPIO_ERR_GET_HANDLE;
                }
                close(fd);
                close(req.fd);
            }
            if (GPIO_Unlock(PinNo) != GPIO_ERR_NONE) {
                RetVal = GPIO_ERR_MUTEX;
            }
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
    UINT32 RetVal = GPIO_ERR_NONE;
    UINT32 PinNo = PinID & 0xfffU;
    //UINT32 RegPinGroup = PinNo >> 5U;
    //UINT32 RegBitPosition = PinNo & 0x1fU;
    int fd, err = 0;
    struct gpiohandle_request req;

    if (PinNo >= AMBA_NUM_GPIO_PIN) {
        RetVal = GPIO_ERR_ARG;
    } else {
        RetVal = GPIO_Lock(PinNo);
        if (RetVal != GPIO_ERR_NONE) {
            RetVal = GPIO_ERR_MUTEX;
        } else {

            req.lineoffsets[0] = PinNo;
            req.flags = GPIOHANDLE_REQUEST_ACTIVE_LOW;

            strcpy(req.consumer_label, "gpio");

            fd = open(pDeviceName[0], O_RDWR);
            if ( fd == -1 ) {
                RetVal = GPIO_ERR_UNEXPECTED;
            } else {
                err = ioctl(fd, GPIO_GET_LINEHANDLE_IOCTL, &req);
                if (err) {
                    RetVal = GPIO_ERR_UNEXPECTED;
                }
                close(fd);
            }
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
    (void) PinID;
    (void) DriveStrength;
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
    UINT32 RetVal = GPIO_ERR_NONE;
    UINT32 PinNo = PinID & 0xfffU;
    //UINT32 RegPinGroup = PinNo >> 5U;
    //UINT32 RegBitPosition = PinNo & 0x1fU;
    int fd, err = 0;
    struct gpiohandle_request req;
    struct gpiohandle_data data;


    if (PinNo >= AMBA_NUM_GPIO_PIN) {
        RetVal = GPIO_ERR_ARG;
    } else {
        RetVal = GPIO_Lock(PinNo);
        if (RetVal != GPIO_ERR_NONE) {
            RetVal = GPIO_ERR_MUTEX;
        } else {
            req.lineoffsets[0] = PinNo;
            req.flags = GPIOHANDLE_REQUEST_OUTPUT;
            if ( PullUpOrDown == AMBA_GPIO_PULL_UP ) {
                data.values[0] = 1U;
            } else {
                data.values[0] = 0U;
            }


            memcpy(req.default_values, &data, sizeof(req.default_values));
            strcpy(req.consumer_label, "gpio");
            req.lines = 1U;

            fd = open(pDeviceName[0], O_RDWR);
            if ( fd == -1 ) {
                RetVal = GPIO_ERR_UNEXPECTED;
            } else {
                err = ioctl(fd, GPIO_GET_LINEHANDLE_IOCTL, &req);
                if (err) {
                    RetVal = GPIO_ERR_GET_HANDLE;
                }
                close(fd);
            }

            err = ioctl(req.fd, GPIOHANDLE_SET_LINE_VALUES_IOCTL, &data);
            if (err) {
                RetVal = GPIO_ERR_SET_HANDLE;
            }
            close(req.fd);

            if (GPIO_Unlock(PinNo) != GPIO_ERR_NONE) {
                RetVal = GPIO_ERR_MUTEX;
            }
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
    (void) PinID;
    (void) IntConfig;
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
    (void) PinID;
    (void) IntFunc;
    (void) IntFuncArg;
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
    (void) PinID;
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
    (void) PinID;
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
    UINT32 RetVal = GPIO_ERR_NONE;
    UINT32 PinNo = PinID & 0xfffU;
    UINT32 RegPinGroup = PinNo >> 5U;
    //UINT32 RegBitPosition = PinNo & 0x1fU;
    int fd, err = 0;
    struct gpioline_info info;

    fd = open(pDeviceName[RegPinGroup], O_RDWR);
    if ( fd == -1 ) {
        RetVal = GPIO_ERR_UNEXPECTED;
    } else {
        err = ioctl(fd, GPIO_GET_LINEINFO_IOCTL, &info);
        if (err) {
            RetVal = GPIO_ERR_UNEXPECTED;
        }
        close(fd);
    }

    pPinInfo->IntType = info.flags;

    return RetVal;
}

/**
 *  GPIO_Lock - Acquire mutex protection
 *  @param[in] PinNo GPIO pin number
 *  @return error code
 */
static UINT32 GPIO_Lock(UINT32 PinNo)
{
    UINT32 GpioGroup = PinNo >> 5U;
    UINT32 RetVal = GPIO_ERR_NONE;

    if (GpioGroup >= (UINT32)AMBA_NUM_GPIO_GROUP) {
        RetVal = GPIO_ERR_ARG;
    } else if (AmbaKAL_MutexTake(&AmbaGpioMutex[GpioGroup], KAL_WAIT_FOREVER) != KAL_ERR_NONE) {
        RetVal = GPIO_ERR_MUTEX;
    } else {
        RetVal = GPIO_ERR_NONE;
    }
    return RetVal;
}

/**
 *  GPIO_Unlock - Deacquire mutex protection
 *  @param[in] PinNo GPIO pin number
 *  @return error code
 */
static UINT32 GPIO_Unlock(UINT32 PinNo)
{
    UINT32 GpioGroup = PinNo >> 5U;
    UINT32 RetVal = GPIO_ERR_NONE;

    if (GpioGroup >= (UINT32)AMBA_NUM_GPIO_GROUP) {
        RetVal = GPIO_ERR_ARG;
    } else if (AmbaKAL_MutexGive(&AmbaGpioMutex[GpioGroup]) != KAL_ERR_NONE) {
        RetVal = GPIO_ERR_MUTEX;
    } else {
        RetVal = GPIO_ERR_NONE;
    }

    return RetVal;
}
