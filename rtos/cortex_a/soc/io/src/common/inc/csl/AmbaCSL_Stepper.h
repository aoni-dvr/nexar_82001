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

#ifndef AMBA_CSL_STEPPER_H
#define AMBA_CSL_STEPPER_H

#ifndef AMBA_PWM_DEF_H
#include "AmbaPWM_Def.h"
#endif

#include "AmbaReg_PWM.h"

static inline void AmbaCSL_StepperSetPatternMSB(AMBA_PWM_STEPPER_MOTOR_REG_s *pStepReg, UINT32 i, UINT32 d)
{
    pStepReg->Pattern[i].Data[0] = d;
}
static inline void AmbaCSL_StepperSetPatternLSB(AMBA_PWM_STEPPER_MOTOR_REG_s *pStepReg, UINT32 i, UINT32 d)
{
    pStepReg->Pattern[i].Data[1] = d;
}

static inline void AmbaCSL_StepperSetCount(AMBA_PWM_STEPPER_MOTOR_REG_s *pStepReg, UINT32 d)
{
    pStepReg->Count = d;
}

static inline void AmbaCSL_StepperAIntDisable(void)
{
    pAmbaPwmStep_Reg->StepperIntCtrl.StepperAEndIntEn = 0U;
}
static inline void AmbaCSL_StepperBIntDisable(void)
{
    pAmbaPwmStep_Reg->StepperIntCtrl.StepperBEndIntEn = 0U;
}
static inline void AmbaCSL_StepperCIntDisable(void)
{
    pAmbaPwmStep_Reg->StepperIntCtrl.StepperCEndIntEn = 0U;
}

/*
 * Defined in AmbaCSL_Stepper.c
 */
volatile AMBA_PWM_STEPPER_MOTOR_REG_s *AmbaCSL_StepperGetReg(UINT32 StepperChanNo);

#endif /* AMBA_CSL_STEPPER_H */

