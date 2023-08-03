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
#include "hw/ambarella_pwm.h"
#include "AmbaTypes.h"
#include "AmbaDef.h"
#include "AmbaKAL.h"
#include "AmbaWrap.h"
#include "AmbaPWM.h"

#define AmbaWrap_memset memset

static AMBA_KAL_MUTEX_t AmbaPwmMutex[AMBA_NUM_PWM_CHANNEL] GNU_SECTION_NOZEROINIT;
#if defined(CONFIG_SOC_CV2)
static AMBA_KAL_MUTEX_t AmbaPwmStepMutex[AMBA_NUM_PWM_STEPPER_CHANNEL] GNU_SECTION_NOZEROINIT;
#endif

/**
 *  AmbaPWM_DrvEntry - Initializes driver-wide data structures and resources
 *  @return error code
 *  @note this function is an internal using only API
 */
UINT32 AmbaPWM_DrvEntry(void)
{
    UINT32 i, RetVal = PWM_ERR_NONE;

    (void)AmbaWrap_memset(AmbaPwmMutex, 0x0, sizeof(AmbaPwmMutex));

    for (i = 0U; i < AMBA_NUM_PWM_CHANNEL; i++) {
        if (AmbaKAL_MutexCreate(&AmbaPwmMutex[i], NULL) != KAL_ERR_NONE) {
            RetVal = PWM_ERR_UNEXPECTED;
            break;
        }
    }

#if defined(CONFIG_SOC_CV2)
    if (RetVal == PWM_ERR_NONE) {
        (void)AmbaWrap_memset(AmbaPwmStepMutex, 0x0, sizeof(AmbaPwmStepMutex));

        for (i = 0U; i < AMBA_NUM_PWM_STEPPER_CHANNEL; i++) {
            if (AmbaKAL_MutexCreate(&AmbaPwmStepMutex[i], NULL) != KAL_ERR_NONE) {
                RetVal = PWM_ERR_UNEXPECTED;
            }
        }
    }
#endif

    return RetVal;
}

/**
 *  AmbaPWM_Config - The function programs the base clock frequency which is used as the time unit of duty cycle and period.
 *  @param[in] PinPairID Pin (or pin pair) ID
 *  @param[in] BaseFreq PWM signal configuration
 *  @return error code
 */
UINT32 AmbaPWM_Config(UINT32 PinPairID, UINT32 BaseFreq)
{
    UINT32 RetVal = PWM_ERR_NONE;
    int fd, err = 0;
    pwm_ch_t pwmCtrl;

    if ((PinPairID >= AMBA_NUM_PWM_CHANNEL) || (BaseFreq == 0U)) {
        RetVal = PWM_ERR_ARG;
    } else {
        RetVal = AmbaKAL_MutexTake(&AmbaPwmMutex[PinPairID], KAL_WAIT_NEVER);
        if ((RetVal != KAL_ERR_NONE)) {
            RetVal = PWM_ERR_MUTEX;
        } else {
            fd = open("/dev/pwm", O_RDWR);
            if (fd == -1) {
                RetVal = PWM_ERR_UNEXPECTED;
            } else {
                pwmCtrl.PwmCh = PinPairID;
                pwmCtrl.Freq = BaseFreq;

                err = devctl(fd, DCMD_PWM_SET_CONFIG, &pwmCtrl, sizeof(pwm_ch_t), NULL);
                if (err) {
                    RetVal = PWM_ERR_UNEXPECTED;
                }

                close(fd);
            }

            if (AmbaKAL_MutexGive(&AmbaPwmMutex[PinPairID]) != KAL_ERR_NONE) {
                /* If code reaches here, the system should be corrupted. Overwite the previous return value because it is meaningless. */
                RetVal = PWM_ERR_UNEXPECTED;
            }
        }
    }

    return RetVal;
}

/**
 *  AmbaPWM_Start - The function starts the PWM output with the specified duty cycle.
 *  @param[in] PinPairID Pin (or pin pair) ID
 *  @param[in] Period Number of base clock ticks to form a PWM wave (0~65535)
 *  @param[in] Duty PWM signal configuration
 *  @return error code
 */
UINT32 AmbaPWM_Start(UINT32 PinPairID, UINT32 Period, UINT32 Duty)
{
    UINT32 RetVal = PWM_ERR_NONE;
    int fd, err = 0;
    pwm_ch_t pwmCtrl;

    if ((PinPairID >= AMBA_NUM_PWM_CHANNEL) || (Period == 0U) || (Duty > Period)) {
        RetVal = PWM_ERR_ARG;
    } else {
        RetVal = AmbaKAL_MutexTake(&AmbaPwmMutex[PinPairID], KAL_WAIT_NEVER);
        if ((RetVal != KAL_ERR_NONE)) {
            RetVal = PWM_ERR_MUTEX;
        } else {
            fd = open("/dev/pwm", O_RDWR);
            if (fd == -1) {
                RetVal = PWM_ERR_UNEXPECTED;
            } else {
                pwmCtrl.PwmCh = PinPairID;
                pwmCtrl.Period = Period;
                pwmCtrl.Duty = Duty;

                err = devctl(fd, DCMD_PWM_START, &pwmCtrl, sizeof(pwm_ch_t), NULL);
                if (err) {
                    RetVal = PWM_ERR_UNEXPECTED;
                }

                close(fd);
            }

            if (AmbaKAL_MutexGive(&AmbaPwmMutex[PinPairID]) != KAL_ERR_NONE) {
                /* If code reaches here, the system should be corrupted. Overwite the previous return value because it is meaningless. */
                RetVal = PWM_ERR_UNEXPECTED;
            }
        }
    }

    return RetVal;
}

/**
 *  AmbaPWM_Stop - This function stops the PWM output.
 *  @param[in] PinPairID Pin (or pin pair) ID
 *  @return error code
 */
UINT32 AmbaPWM_Stop(UINT32 PinPairID)
{
    UINT32 RetVal = PWM_ERR_NONE;
    int fd, err = 0;
    pwm_ch_t pwmCtrl;

    if (PinPairID >= AMBA_NUM_PWM_CHANNEL) {
        RetVal = PWM_ERR_ARG;
    } else {
        RetVal = AmbaKAL_MutexTake(&AmbaPwmMutex[PinPairID], KAL_WAIT_NEVER);
        if ((RetVal != KAL_ERR_NONE)) {
            RetVal = PWM_ERR_MUTEX;
        } else {
            fd = open("/dev/pwm", O_RDWR);
            if (fd == -1) {
                RetVal = PWM_ERR_UNEXPECTED;
            } else {
                pwmCtrl.PwmCh = PinPairID;

                err = devctl(fd, DCMD_PWM_STOP, &pwmCtrl, sizeof(pwm_ch_t), NULL);
                if (err) {
                    RetVal = PWM_ERR_UNEXPECTED;
                }

                close(fd);
            }

            if (AmbaKAL_MutexGive(&AmbaPwmMutex[PinPairID]) != KAL_ERR_NONE) {
                /* If code reaches here, the system should be corrupted. Overwite the previous return value because it is meaningless. */
                RetVal = PWM_ERR_UNEXPECTED;
            }
        }
    }

    return RetVal;
}

/**
 *  AmbaPWM_GetStatus - Get PWM-0/1/2/3/4 status
 *  @param[in] PinPairID Pin (or pin pair) ID
 *  @param[out] pPwmStatus PWM Base frequency (Hz)
 *  @return error code
 */
UINT32 AmbaPWM_GetInfo(UINT32 PinPairID, UINT32 *pActualBaseFreq)
{
    UINT32 RetVal = PWM_ERR_NONE;
    int fd, err = 0;
    pwm_ch_t pwmCtrl;

    if ((PinPairID >= AMBA_NUM_PWM_CHANNEL) || (pActualBaseFreq == NULL)) {
        RetVal = PWM_ERR_ARG;
    } else {
        fd = open("/dev/pwm", O_RDWR);
        if (fd == -1) {
            RetVal = PWM_ERR_UNEXPECTED;
        } else {
            pwmCtrl.PwmCh = PinPairID;

            err = devctl(fd, DCMD_PWM_GET_INFO, &pwmCtrl, sizeof(pwm_ch_t), NULL);
            if (err) {
                RetVal = PWM_ERR_UNEXPECTED;
            }

            close(fd);

            *pActualBaseFreq = pwmCtrl.ActualBaseFreq;
        }
    }

    return RetVal;
}

#if defined(CONFIG_SOC_CV2)
/**
 *  AmbaPWM_StepConfig - This function configures a group of PWM pins as stepping mode
 *  @param[in] PinGrpID Stepper channel number
 *  @param[in] BaseFreq Base frequency (Hz)
 *  @param[in] PulseWidth Number of base clock ticks to form a PWM wave.
 *  @return error code
 */
UINT32 AmbaPWM_StepConfig(UINT32 PinGrpID, UINT32 BaseFreq, UINT32 PulseWidth)
{
    AMBA_KAL_MUTEX_t *pMutex = &AmbaPwmStepMutex[PinGrpID];
    UINT32 RetVal = PWM_ERR_NONE;
    int fd, err = 0;
    pwm_ch_t pwmCtrl;

    if ((PinGrpID >= AMBA_NUM_PWM_STEPPER_CHANNEL) || (BaseFreq == 0U) || (PulseWidth == 0U)) {
        RetVal = PWM_ERR_ARG;
    } else {
        if (AmbaKAL_MutexTake(pMutex, KAL_WAIT_NEVER) != KAL_ERR_NONE) {
            RetVal = PWM_ERR_MUTEX;
        } else {
            fd = open("/dev/pwm", O_RDWR);
            if (fd == -1) {
                RetVal = PWM_ERR_UNEXPECTED;
            } else {
                pwmCtrl.PinGrpID = PinGrpID;
                pwmCtrl.BaseFreq = BaseFreq;
                pwmCtrl.PulseWidth = PulseWidth;

                err = devctl(fd, DCMD_PWM_SET_STEPCONFIG, &pwmCtrl, sizeof(pwm_ch_t), NULL);
                if (err) {
                    RetVal = PWM_ERR_UNEXPECTED;
                }

                close(fd);
            }

            if (AmbaKAL_MutexGive(pMutex) != KAL_ERR_NONE) {
                /* If code reaches here, the system should be corrupted. Overwite the previous return value because it is meaningless. */
                RetVal = PWM_ERR_UNEXPECTED;
            }
        }
    }

    return RetVal;
}

/**
 *  AmbaPWM_StepSetDuty - This function configures the next pulse of a pin belong to the specified pin group
 *  @param[in] PinGrpID Stepper channel number
 *  @param[in] PinID The index of a pin within the pin group.
 *  @param[in] Duty Number of base clock ticks to form the signal high state duration of a PWM wave.
 *  @return error code
 */
UINT32 AmbaPWM_StepSetDuty(UINT32 PinGrpID, UINT32 PinID, UINT32 Duty)
{
    AMBA_KAL_MUTEX_t *pMutex = &AmbaPwmStepMutex[PinGrpID];
    UINT32 RetVal = PWM_ERR_NONE;
    int fd, err = 0;
    pwm_ch_t pwmCtrl;

    if ((PinGrpID >= AMBA_NUM_PWM_STEPPER_CHANNEL) || (PinID >= AMBA_NUM_PWM_STEPPER_CHANNEL_PIN) || (Duty == 0U)) {
        RetVal = PWM_ERR_ARG;
    } else {
        if (AmbaKAL_MutexTake(pMutex, KAL_WAIT_NEVER) != KAL_ERR_NONE) {
            RetVal = PWM_ERR_MUTEX;
        } else {
            fd = open("/dev/pwm", O_RDWR);
            if (fd == -1) {
                RetVal = PWM_ERR_UNEXPECTED;
            } else {
                pwmCtrl.PinGrpID = PinGrpID;
                pwmCtrl.PinID = PinID;
                pwmCtrl.Duty = Duty;

                err = devctl(fd, DCMD_PWM_SET_STEPDUTY, &pwmCtrl, sizeof(pwm_ch_t), NULL);
                if (err) {
                    RetVal = PWM_ERR_UNEXPECTED;
                }

                close(fd);
            }

            if (AmbaKAL_MutexGive(pMutex) != KAL_ERR_NONE) {
                /* If code reaches here, the system should be corrupted. Overwite the previous return value because it is meaningless. */
                RetVal = PWM_ERR_UNEXPECTED;
            }
        }
    }

    return RetVal;
}

/**
 *  AmbaPWM_StepAct - The function generates the next pulse waves of the specified PWM pin group.
 *  @param[in] PinGrpID Stepper channel number
 *  @return error code
 */
UINT32 AmbaPWM_StepAct(UINT32 PinGrpID)
{
    AMBA_KAL_MUTEX_t *pMutex = &AmbaPwmStepMutex[PinGrpID];
    UINT32 RetVal = PWM_ERR_NONE;
    int fd, err = 0;
    pwm_ch_t pwmCtrl;

    if (PinGrpID >= AMBA_NUM_PWM_STEPPER_CHANNEL) {
        RetVal = PWM_ERR_ARG;
    } else {
        if (AmbaKAL_MutexTake(pMutex, KAL_WAIT_NEVER) != KAL_ERR_NONE) {
            RetVal = PWM_ERR_MUTEX;
        } else {
            fd = open("/dev/pwm", O_RDWR);
            if (fd == -1) {
                RetVal = PWM_ERR_UNEXPECTED;
            } else {
                pwmCtrl.PinGrpID = PinGrpID;

                err = devctl(fd, DCMD_PWM_SET_STEP_ACT, &pwmCtrl, sizeof(pwm_ch_t), NULL);
                if (err) {
                    RetVal = PWM_ERR_UNEXPECTED;
                }

                close(fd);
            }

            if (AmbaKAL_MutexGive(pMutex) != KAL_ERR_NONE) {
                /* If code reaches here, the system should be corrupted. Overwite the previous return value because it is meaningless. */
                RetVal = PWM_ERR_UNEXPECTED;
            }
        }
    }

    return RetVal;
}

/**
 *  AmbaPWM_StepGetInfo - Get steper status
 *  @param[in] PinGrpID Stepper channel number
 *  @param[out] pActualBaseFreq Base frequency
 *  @return error code
 */
UINT32 AmbaPWM_StepGetInfo(UINT32 PinGrpID, UINT32 *pActualBaseFreq)
{
    UINT32 RetVal = PWM_ERR_NONE;
    int fd, err = 0;
    pwm_ch_t pwmCtrl;

    if ((PinGrpID >= AMBA_NUM_PWM_STEPPER_CHANNEL) || (pActualBaseFreq == NULL)) {
        RetVal = PWM_ERR_ARG;
    } else {
        fd = open("/dev/pwm", O_RDWR);
        if (fd == -1) {
            RetVal = PWM_ERR_UNEXPECTED;
        } else {
            pwmCtrl.PinGrpID = PinGrpID;

            err = devctl(fd, DCMD_PWM_GET_STEP_STATUS, &pwmCtrl, sizeof(pwm_ch_t), NULL);
            if (err) {
                RetVal = PWM_ERR_UNEXPECTED;
            }

            close(fd);

            pwmCtrl.ActualBaseFreq = pwmCtrl.ActualBaseFreq;
        }
    }

    return RetVal;
}
#endif
