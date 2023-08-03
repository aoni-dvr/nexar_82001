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

#ifndef AMBA_PWM_H
#define AMBA_PWM_H

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>
#include <sys/resmgr.h>
#include <sys/neutrino.h>
#include <sys/syspage.h>
#include <sys/mman.h>
#include <sys/procmgr.h>
#include <sys/iofunc.h>
#include <sys/dispatch.h>
#include <sys/slog.h>
#include <sys/slogcodes.h>
#include <hw/inout.h>
#include <hw/ambarella_pwm.h>

#include <arm/ambarella.h>
#include "AmbaRTSL_PWM.h"
#if defined(CONFIG_SOC_CV2)
#include "AmbaRTSL_Stepper.h"
#endif
#include "AmbaReg_PWM.h"
#include "../inc/AmbaTypes.h"
#include "AmbaReg_RCT.h"

/* -------------------------------------------------------------------------
 * PWM
 * -------------------------------------------------------------------------
 */
#if defined(CONFIG_SOC_CV2)
#define AMBA_PWM0_BASE               0xe4004000
#else
#define AMBA_PWM0_BASE               0xe4003000
#endif
#define AMBA_PWM0_SIZE               0x1000
#define AMBA_PWM1_BASE               0xe400c000
#define AMBA_PWM1_SIZE               0x1000
#define AMBA_PWM2_BASE               0xe400d000
#define AMBA_PWM2_SIZE               0x1000
#if defined(CONFIG_SOC_CV2)
#define AMBA_STEP_BASE               0xe4002000
#define AMBA_STEP_SIZE               0x1000
#endif

#define DEBUG 1

#ifdef DEBUG
#define DEBUG_DUMP(s)          printf(s)
#else
#define DEBUG_DUMP(...)
#endif

int pwm_io_devctl(resmgr_context_t *ctp, io_devctl_t *msg, RESMGR_OCB_T *ocb);

int Amba_Pwm_Start(UINT32 PwmCh, UINT32 Period, UINT32 Duty);
int Amba_Pwm_Set_Config(UINT32 PwmCh, UINT32 Freq);
int Amba_Pwm_Stop(UINT32 PwmCh);
int Amba_Pwm_Get_Info(UINT32 PwmCh, UINT32 *pActualBaseFreq);
#if defined(CONFIG_SOC_CV2)
int Amba_Pwm_Set_StepConfig(UINT32 PinGrpID, UINT32 BaseFreq, UINT32 PulseWidth);
int Amba_Pwm_Set_StepSetDuty(UINT32 PinGrpID, UINT32 PinID, UINT32 Duty);
int Amba_Pwm_StepAct(UINT32 PinGrpID);
int Amba_Pwm_StepGetStatus(UINT32 PinGrpID, UINT32 *pActualBaseFreq);
#endif
int Amba_Pwm_Init(void);
void Amba_Pwm_Fini(void);

#endif
