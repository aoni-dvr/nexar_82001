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
#include "Amba_pwm.h"

AMBA_PWM_REG_s * pAmbaPWM_Reg[3];
AMBA_PWM_STEPPER_REG_s * pAmbaPwmStep_Reg;

int Amba_Pwm_Init(void)
{
    uintptr_t virt_base;

    virt_base = mmap_device_io(AMBA_PWM0_SIZE, AMBA_PWM0_BASE);
    if (virt_base == (uintptr_t) MAP_FAILED) {
        perror("mmap_device_io");
        return -1;
    }
    pAmbaPWM_Reg[0] = (AMBA_PWM_REG_s *)virt_base;

    virt_base = mmap_device_io(AMBA_PWM1_SIZE, AMBA_PWM1_BASE);
    if (virt_base == (uintptr_t) MAP_FAILED) {
        perror("mmap_device_io");
        return -1;
    }
    pAmbaPWM_Reg[1] = (AMBA_PWM_REG_s *)virt_base;

    virt_base = mmap_device_io(AMBA_PWM2_SIZE, AMBA_PWM2_BASE);
    if (virt_base == (uintptr_t) MAP_FAILED) {
        perror("mmap_device_io");
        return -1;
    }
    pAmbaPWM_Reg[2] = (AMBA_PWM_REG_s *)virt_base;

#if defined(CONFIG_SOC_CV2)
    virt_base = mmap_device_io(AMBA_STEP_SIZE, AMBA_STEP_BASE);
    if (virt_base == (uintptr_t) MAP_FAILED) {
        perror("mmap_device_io");
        return -1;
    }
    pAmbaPwmStep_Reg = (AMBA_PWM_STEPPER_REG_s *)virt_base;
#endif
    AmbaRTSL_PwmInit();

    return 0;
}


void Amba_Pwm_Fini(void)
{
    munmap_device_io(AMBA_PWM0_BASE, AMBA_PWM0_SIZE);
    munmap_device_io(AMBA_PWM1_BASE, AMBA_PWM1_SIZE);
    munmap_device_io(AMBA_PWM2_BASE, AMBA_PWM2_SIZE);
#if defined(CONFIG_SOC_CV2)
    munmap_device_io(AMBA_STEP_BASE, AMBA_STEP_SIZE);
#endif
}

int Amba_Pwm_Start(UINT32 PwmCh, UINT32 Period, UINT32 Duty)
{
    return (int)AmbaRTSL_PwmStart(PwmCh, Period, Duty);
}

int Amba_Pwm_Set_Config(UINT32 PwmCh, UINT32 Freq)
{
    return (int)AmbaRTSL_PwmSetConfig(PwmCh, Freq);
}

int Amba_Pwm_Stop(UINT32 PwmCh)
{
    AmbaRTSL_PwmStop(PwmCh);
    return 0;
}

int Amba_Pwm_Get_Info(UINT32 PwmCh, UINT32 *pActualBaseFreq)
{
    return (int)AmbaRTSL_PwmGetInfo(PwmCh, pActualBaseFreq);
}

#if defined(CONFIG_SOC_CV2)
int Amba_Pwm_Set_StepConfig(UINT32 PinGrpID, UINT32 BaseFreq, UINT32 PulseWidth)
{
    return (int)AmbaRTSL_PwmStepSetConfig(PinGrpID, BaseFreq, PulseWidth);
}

int Amba_Pwm_Set_StepSetDuty(UINT32 PinGrpID, UINT32 PinID, UINT32 Duty)
{
    return (int)AmbaRTSL_PwmStepSetDuty(PinGrpID, PinID, Duty);
}

int Amba_Pwm_StepAct(UINT32 PinGrpID)
{
    return (int)AmbaRTSL_PwmStepAct(PinGrpID);
}

int Amba_Pwm_StepGetStatus(UINT32 PinGrpID, UINT32 *pActualBaseFreq)
{
    return (int)AmbaRTSL_PwmStepGetStatus(PinGrpID, pActualBaseFreq);
}
#endif
