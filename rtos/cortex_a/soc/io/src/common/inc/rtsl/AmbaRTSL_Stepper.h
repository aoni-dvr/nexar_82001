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

#ifndef AMBA_RTSL_STEPPER_H
#define AMBA_RTSL_STEPPER_H

#ifndef AMBA_PWM_DEF_H
#include "AmbaPWM_Def.h"
#endif

typedef void (*AMBA_PWM_STEP_ISR_f)(UINT32 PwmStepChanNo);

/*
 * Defined in AmbaRTSL_Stepper.c
 */
void AmbaRTSL_PwmStepInit(void);
UINT32 AmbaRTSL_PwmStepSetConfig(UINT32 PinGrpID, UINT32 BaseFreq, UINT32 PulseWidth);
UINT32 AmbaRTSL_PwmStepSetDuty(UINT32 PinGrpID, UINT32 PinID, UINT32 Duty);
UINT32 AmbaRTSL_PwmStepAct(UINT32 PinGrpID);
UINT32 AmbaRTSL_PwmStepGetStatus(UINT32 PinGrpID, UINT32 *pActualBaseFreq);
void AmbaRTSL_PwmStepHookIntHandler(AMBA_PWM_STEP_ISR_f IntFunc);

#endif /* AMBA_RTSL_STEPPER_H */
