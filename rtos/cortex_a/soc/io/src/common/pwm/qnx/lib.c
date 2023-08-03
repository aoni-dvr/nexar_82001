/*
 * $QNXLicenseC:
 * Copyright 2018, QNX Software Systems.
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

#include "proto.h"

int get_pwmfuncs(pwm_functions_t *functable, int tabsize)
{
    PWM_ADD_FUNC(functable, init, Amba_Pwm_Init, tabsize);
    PWM_ADD_FUNC(functable, fini, Amba_Pwm_Fini, tabsize);

    PWM_ADD_FUNC(functable, pwm_start, Amba_Pwm_Start, tabsize);
    PWM_ADD_FUNC(functable, pwm_set_config, Amba_Pwm_Set_Config, tabsize);
    PWM_ADD_FUNC(functable, pwm_set_stop, Amba_Pwm_Stop, tabsize);
    PWM_ADD_FUNC(functable, pwm_get_info, Amba_Pwm_Get_Info, tabsize);
#if defined(CONFIG_SOC_CV2)
    PWM_ADD_FUNC(functable, pwm_set_step_config, Amba_Pwm_Set_StepConfig, tabsize);
    PWM_ADD_FUNC(functable, pwm_step_set_duty, Amba_Pwm_Set_StepSetDuty, tabsize);
    PWM_ADD_FUNC(functable, pwm_step_act, Amba_Pwm_StepAct, tabsize);
    PWM_ADD_FUNC(functable, pwm_step_get_status, Amba_Pwm_StepGetStatus, tabsize);
#endif
    return 0;
}
