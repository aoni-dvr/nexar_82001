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

#include "AmbaRTSL_GPIO.h"
#include "AmbaRTSL_GPIO_Ctrl.h"
#if defined(CONFIG_ENABLE_AMBALINK)
#include "AmbaIPC_Lock.h"
#endif

static AMBA_KAL_MUTEX_t AmbaGpioMutex[AMBA_NUM_GPIO_GROUP];

#if defined(CONFIG_ENABLE_AMBALINK)
static UINT32 GPIO_Lock(UINT32 PinNo, UINT32 *pFlags);
#else
static UINT32 GPIO_Lock(UINT32 PinNo, const UINT32 *pFlags);
#endif
static UINT32 GPIO_Unlock(UINT32 PinNo, UINT32 Flags);

/**
 *  AmbaGPIO_DrvEntry - Initializes driver-wide data structures and resources
 *  @return error code
 *  @note this function is an internal using only API
 */
UINT32 AmbaGPIO_DrvEntry(void)
{
    static char AmbaGpioMutexName[16] = "AmbaGpioMutex";
    UINT32 i, RetVal = GPIO_ERR_NONE;

    for (i = 0U; i < AMBA_NUM_GPIO_GROUP; i++) {
        if (AmbaKAL_MutexCreate(&AmbaGpioMutex[i], AmbaGpioMutexName) != KAL_ERR_NONE) {
            RetVal = GPIO_ERR_UNEXPECTED;
            break;
        }
    }

    AmbaRTSL_GpioInit();

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

    if (pDefaultParam != NULL) {
        AmbaRTSL_GpioLoadDefaultVals(pDefaultParam);
    } else {
        RetVal = GPIO_ERR_ARG;
    }

    return RetVal;
}

/**
 *  AmbaGPIO_SetFuncGPI - Configure GPIO pin as general purpose input function
 *  @param[in] PinID A gpio pin id
 *  @return error code
 */
UINT32 AmbaGPIO_SetFuncGPI(UINT32 PinID)
{
    UINT32 PinNo = PinID & 0xfffU;
    UINT32 RetVal = GPIO_ERR_NONE;
    UINT32 Flags = 0U;

    if (PinNo >= AMBA_NUM_GPIO_PIN) {
        RetVal = GPIO_ERR_ARG;
    } else {
        RetVal = GPIO_Lock(PinNo, &Flags);

        if (RetVal != GPIO_ERR_NONE) {
            RetVal = GPIO_ERR_MUTEX;
        } else {
            RetVal = AmbaRTSL_GpioSetFuncGPI(PinID);

            if (GPIO_Unlock(PinNo, Flags) != GPIO_ERR_NONE) {
                /* If code reaches here, the system should be corrupted. Overwite the previous return value because it is meaningless. */
                RetVal = GPIO_ERR_UNEXPECTED;
            }
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
    UINT32 Flags = 0U;

    if (PinNo >= AMBA_NUM_GPIO_PIN) {
        RetVal = GPIO_ERR_ARG;
    } else {
        RetVal = GPIO_Lock(PinNo, &Flags);

        if (RetVal != GPIO_ERR_NONE) {
            RetVal = GPIO_ERR_MUTEX;
        } else {
            RetVal = AmbaRTSL_GpioSetFuncGPO(PinID, PinLevel);

            if (GPIO_Unlock(PinNo, Flags) != GPIO_ERR_NONE) {
                /* If code reaches here, the system should be corrupted. Overwite the previous return value because it is meaningless. */
                RetVal = GPIO_ERR_UNEXPECTED;
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
    UINT32 PinNo = PinID & 0xfffU;
    UINT32 RetVal = GPIO_ERR_NONE;
    UINT32 Flags = 0U;

    if (PinNo >= AMBA_NUM_GPIO_PIN) {
        RetVal = GPIO_ERR_ARG;
    } else {
        RetVal = GPIO_Lock(PinNo, &Flags);

        if (RetVal != GPIO_ERR_NONE) {
            RetVal = GPIO_ERR_MUTEX;
        } else {
            RetVal = AmbaRTSL_GpioSetFuncAlt(PinID);

            if (GPIO_Unlock(PinNo, Flags) != GPIO_ERR_NONE) {
                /* If code reaches here, the system should be corrupted. Overwite the previous return value because it is meaningless. */
                RetVal = GPIO_ERR_UNEXPECTED;
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
    UINT32 PinNo = PinID & 0xfffU;
    UINT32 RetVal = GPIO_ERR_NONE;
    UINT32 Flags = 0U;

    if (PinNo >= AMBA_NUM_GPIO_PIN) {
        RetVal = GPIO_ERR_ARG;
    } else {
        RetVal = GPIO_Lock(PinNo, &Flags);

        if (RetVal != GPIO_ERR_NONE) {
            RetVal = GPIO_ERR_MUTEX;
        } else {
            RetVal = AmbaRTSL_GpioSetDriveStrength(PinID, DriveStrength);

            if (GPIO_Unlock(PinNo, Flags) != GPIO_ERR_NONE) {
                /* If code reaches here, the system should be corrupted. Overwite the previous return value because it is meaningless. */
                RetVal = GPIO_ERR_UNEXPECTED;
            }
        }
    }

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
    UINT32 Flags = 0U;

    if (PinNo >= AMBA_NUM_GPIO_PIN) {
        RetVal = GPIO_ERR_ARG;
    } else {
        RetVal = GPIO_Lock(PinNo, &Flags);

        if (RetVal != GPIO_ERR_NONE) {
            RetVal = GPIO_ERR_MUTEX;
        } else {
            RetVal = AmbaRTSL_GpioSetPullUpOrDown(PinID, PullUpOrDown);

            if (GPIO_Unlock(PinNo, Flags) != GPIO_ERR_NONE) {
                /* If code reaches here, the system should be corrupted. Overwite the previous return value because it is meaningless. */
                RetVal = GPIO_ERR_UNEXPECTED;
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
    UINT32 PinNo = PinID & 0xfffU;
    UINT32 RetVal = GPIO_ERR_NONE;
    UINT32 Flags = 0U;

    if (PinNo >= AMBA_NUM_GPIO_PIN) {
        RetVal = GPIO_ERR_ARG;
    } else {
        RetVal = GPIO_Lock(PinNo, &Flags);

        if (RetVal != GPIO_ERR_NONE) {
            RetVal = GPIO_ERR_MUTEX;
        } else {
            RetVal = AmbaRTSL_GpioIntSetType(PinID, IntConfig);

            if (GPIO_Unlock(PinNo, Flags) != GPIO_ERR_NONE) {
                /* If code reaches here, the system should be corrupted. Overwite the previous return value because it is meaningless. */
                RetVal = GPIO_ERR_UNEXPECTED;
            }
        }
    }

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
    UINT32 PinNo = PinID & 0xfffU;
    UINT32 RetVal = GPIO_ERR_NONE;
    UINT32 Flags = 0U;

    if (PinNo >= AMBA_NUM_GPIO_PIN) {
        RetVal = GPIO_ERR_ARG;
    } else {
        RetVal = GPIO_Lock(PinNo, &Flags);

        if (RetVal != GPIO_ERR_NONE) {
            RetVal = GPIO_ERR_MUTEX;
        } else {
            RetVal = AmbaRTSL_GpioIntHookHandler(PinID, IntFunc, IntFuncArg);

            if (GPIO_Unlock(PinNo, Flags) != GPIO_ERR_NONE) {
                /* If code reaches here, the system should be corrupted. Overwite the previous return value because it is meaningless. */
                RetVal = GPIO_ERR_UNEXPECTED;
            }
        }
    }

    return RetVal;
}

/**
 *  AmbaGPIO_IntEnable - Enable interrupt of the specified pin
 *  @param[in] PinID A gpio pin id
 *  @return error code
 */
UINT32 AmbaGPIO_IntEnable(UINT32 PinID)
{
    UINT32 PinNo = PinID & 0xfffU;
    UINT32 RetVal = GPIO_ERR_NONE;
    UINT32 Flags = 0U;

    if (PinNo >= AMBA_NUM_GPIO_PIN) {
        RetVal = GPIO_ERR_ARG;
    } else {
        RetVal = GPIO_Lock(PinNo, &Flags);

        if (RetVal != GPIO_ERR_NONE) {
            RetVal = GPIO_ERR_MUTEX;
        } else {
            RetVal = AmbaRTSL_GpioIntEnable(PinID);

            if (GPIO_Unlock(PinNo, Flags) != GPIO_ERR_NONE) {
                /* If code reaches here, the system should be corrupted. Overwite the previous return value because it is meaningless. */
                RetVal = GPIO_ERR_UNEXPECTED;
            }
        }
    }

    return RetVal;
}

/**
 *  AmbaGPIO_IntDisable - Disable interrupt of the specified pin
 *  @param[in] PinID A gpio pin id
 *  @return error code
 */
UINT32 AmbaGPIO_IntDisable(UINT32 PinID)
{
    UINT32 PinNo = PinID & 0xfffU;
    UINT32 RetVal = GPIO_ERR_NONE;
    UINT32 Flags = 0U;

    if (PinNo >= AMBA_NUM_GPIO_PIN) {
        RetVal = GPIO_ERR_ARG;
    } else {
        RetVal = GPIO_Lock(PinNo, &Flags);

        if (RetVal != GPIO_ERR_NONE) {
            RetVal = GPIO_ERR_MUTEX;
        } else {
            RetVal = AmbaRTSL_GpioIntDisable(PinID);

            if (GPIO_Unlock(PinNo, Flags) != GPIO_ERR_NONE) {
                /* If code reaches here, the system should be corrupted. Overwite the previous return value because it is meaningless. */
                RetVal = GPIO_ERR_UNEXPECTED;
            }
        }
    }

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
    UINT32 Flags = 0U;

    if ((PinNo >= AMBA_NUM_GPIO_PIN) || (pPinInfo == NULL)) {
        RetVal = GPIO_ERR_ARG;
    } else {
        RetVal = GPIO_Lock(PinNo, &Flags);

        if (RetVal != GPIO_ERR_NONE) {
            RetVal = GPIO_ERR_MUTEX;
        } else {
            RetVal = AmbaRTSL_GpioGetPinInfo(PinID, pPinInfo);

            if (GPIO_Unlock(PinNo, Flags) != GPIO_ERR_NONE) {
                /* If code reaches here, the system should be corrupted. Overwite the previous return value because it is meaningless. */
                RetVal = GPIO_ERR_UNEXPECTED;
            }
        }
    }

    return RetVal;
}

/**
 *  GPIO_Lock - Acquire mutex protection
 *  @param[in] PinNo GPIO pin number
 *  @return error code
 */
#if defined(CONFIG_ENABLE_AMBALINK)
static UINT32 GPIO_Lock(UINT32 PinNo, UINT32 *pFlags)
#else
static UINT32 GPIO_Lock(UINT32 PinNo, const UINT32 *pFlags)
#endif
{
    UINT32 GpioGroup = PinNo >> 5U;
    UINT32 RetVal = GPIO_ERR_NONE;

#if defined(CONFIG_ENABLE_AMBALINK)
    (void)AmbaIPC_SpinLockIrqSave(AMBA_IPC_SPINLOCK_GPIO, pFlags);
#else
    (void)pFlags;
#endif

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
static UINT32 GPIO_Unlock(UINT32 PinNo, UINT32 Flags)
{
    UINT32 GpioGroup = PinNo >> 5U;
    UINT32 RetVal = GPIO_ERR_NONE;

#if defined(CONFIG_ENABLE_AMBALINK)
    (void)AmbaIPC_SpinUnlockIrqRestore(AMBA_IPC_SPINLOCK_GPIO, Flags);
#else
    (void)Flags;
#endif
    if (GpioGroup >= (UINT32)AMBA_NUM_GPIO_GROUP) {
        RetVal = GPIO_ERR_ARG;
    } else if (AmbaKAL_MutexGive(&AmbaGpioMutex[GpioGroup]) != KAL_ERR_NONE) {
        RetVal = GPIO_ERR_MUTEX;
    } else {
        RetVal = GPIO_ERR_NONE;
    }

    return RetVal;
}

