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

#ifndef AMBA_CSL_PWM_H
#define AMBA_CSL_PWM_H

#ifndef AMBA_PWM_DEF_H
#include "AmbaPWM_Def.h"
#endif

#include "AmbaReg_PWM.h"

#define AMBA_PWM_MAX_GYRO_VALUE             4096U

/*
 * Macro Definitions
 */
static inline void AmbaCSL_Pwm0Enable(void)
{
    pAmbaPWM_Reg[0]->Pwm0Enable.Enable = 1U;
}
static inline void AmbaCSL_Pwm1Enable(void)
{
    pAmbaPWM_Reg[0]->Pwm1Enable.Enable = 1U;
}
static inline void AmbaCSL_Pwm2Enable(void)
{
    pAmbaPWM_Reg[0]->Pwm2Enable.Enable = 1U;
}
static inline void AmbaCSL_Pwm3Enable(void)
{
    pAmbaPWM_Reg[0]->Pwm3Enable.Enable = 1U;
}
static inline void AmbaCSL_Pwm4Enable(void)
{
    pAmbaPWM_Reg[1]->Pwm0Enable.Enable = 1U;
}
static inline void AmbaCSL_Pwm5Enable(void)
{
    pAmbaPWM_Reg[1]->Pwm1Enable.Enable = 1U;
}
static inline void AmbaCSL_Pwm6Enable(void)
{
    pAmbaPWM_Reg[1]->Pwm2Enable.Enable = 1U;
}
static inline void AmbaCSL_Pwm7Enable(void)
{
    pAmbaPWM_Reg[1]->Pwm3Enable.Enable = 1U;
}
static inline void AmbaCSL_Pwm8Enable(void)
{
    pAmbaPWM_Reg[2]->Pwm0Enable.Enable = 1U;
}
static inline void AmbaCSL_Pwm9Enable(void)
{
    pAmbaPWM_Reg[2]->Pwm1Enable.Enable = 1U;
}
static inline void AmbaCSL_Pwm10Enable(void)
{
    pAmbaPWM_Reg[2]->Pwm2Enable.Enable = 1U;
}
static inline void AmbaCSL_Pwm11Enable(void)
{
    pAmbaPWM_Reg[2]->Pwm3Enable.Enable = 1U;
}

static inline void AmbaCSL_Pwm0Disable(void)
{
    pAmbaPWM_Reg[0]->Pwm0Enable.Enable = 0U;
}
static inline void AmbaCSL_Pwm1Disable(void)
{
    pAmbaPWM_Reg[0]->Pwm1Enable.Enable = 0U;
}
static inline void AmbaCSL_Pwm2Disable(void)
{
    pAmbaPWM_Reg[0]->Pwm2Enable.Enable = 0U;
}
static inline void AmbaCSL_Pwm3Disable(void)
{
    pAmbaPWM_Reg[0]->Pwm3Enable.Enable = 0U;
}
static inline void AmbaCSL_Pwm4Disable(void)
{
    pAmbaPWM_Reg[1]->Pwm0Enable.Enable = 0U;
}
static inline void AmbaCSL_Pwm5Disable(void)
{
    pAmbaPWM_Reg[1]->Pwm1Enable.Enable = 0U;
}
static inline void AmbaCSL_Pwm6Disable(void)
{
    pAmbaPWM_Reg[1]->Pwm2Enable.Enable = 0U;
}
static inline void AmbaCSL_Pwm7Disable(void)
{
    pAmbaPWM_Reg[1]->Pwm3Enable.Enable = 0U;
}
static inline void AmbaCSL_Pwm8Disable(void)
{
    pAmbaPWM_Reg[2]->Pwm0Enable.Enable = 0U;
}
static inline void AmbaCSL_Pwm9Disable(void)
{
    pAmbaPWM_Reg[2]->Pwm1Enable.Enable = 0U;
}
static inline void AmbaCSL_Pwm10Disable(void)
{
    pAmbaPWM_Reg[2]->Pwm2Enable.Enable = 0U;
}
static inline void AmbaCSL_Pwm11Disable(void)
{
    pAmbaPWM_Reg[2]->Pwm3Enable.Enable = 0U;
}

static inline void AmbaCSL_Pwm01SetIndividual(void)
{
    pAmbaPWM_Reg[0]->Pwm1Enable.Complementary = 0U;
}
static inline void AmbaCSL_Pwm23SetIndividual(void)
{
    pAmbaPWM_Reg[0]->Pwm3Enable.Complementary = 0U;
}
static inline void AmbaCSL_Pwm45SetIndividual(void)
{
    pAmbaPWM_Reg[1]->Pwm1Enable.Complementary = 0U;
}
static inline void AmbaCSL_Pwm67SetIndividual(void)
{
    pAmbaPWM_Reg[1]->Pwm3Enable.Complementary = 0U;
}
static inline void AmbaCSL_Pwm89SetIndividual(void)
{
    pAmbaPWM_Reg[2]->Pwm1Enable.Complementary = 0U;
}
static inline void AmbaCSL_Pwm1011SetIndividual(void)
{
    pAmbaPWM_Reg[2]->Pwm3Enable.Complementary = 0U;
}

static inline void AmbaCSL_Pwm0SetPulseDivider(UINT32 d)
{
    pAmbaPWM_Reg[0]->Pwm0Enable.Divider = (d) - 1U;
}
static inline void AmbaCSL_Pwm1SetPulseDivider(UINT32 d)
{
    pAmbaPWM_Reg[0]->Pwm1Enable.Divider = (d) - 1U;
}
static inline void AmbaCSL_Pwm2SetPulseDivider(UINT32 d)
{
    pAmbaPWM_Reg[0]->Pwm2Enable.Divider = (d) - 1U;
}
static inline void AmbaCSL_Pwm3SetPulseDivider(UINT32 d)
{
    pAmbaPWM_Reg[0]->Pwm3Enable.Divider = (d) - 1U;
}
static inline void AmbaCSL_Pwm4SetPulseDivider(UINT32 d)
{
    pAmbaPWM_Reg[1]->Pwm0Enable.Divider = (d) - 1U;
}
static inline void AmbaCSL_Pwm5SetPulseDivider(UINT32 d)
{
    pAmbaPWM_Reg[1]->Pwm1Enable.Divider = (d) - 1U;
}
static inline void AmbaCSL_Pwm6SetPulseDivider(UINT32 d)
{
    pAmbaPWM_Reg[1]->Pwm2Enable.Divider = (d) - 1U;
}
static inline void AmbaCSL_Pwm7SetPulseDivider(UINT32 d)
{
    pAmbaPWM_Reg[1]->Pwm3Enable.Divider = (d) - 1U;
}
static inline void AmbaCSL_Pwm8SetPulseDivider(UINT32 d)
{
    pAmbaPWM_Reg[2]->Pwm0Enable.Divider = (d) - 1U;
}
static inline void AmbaCSL_Pwm9SetPulseDivider(UINT32 d)
{
    pAmbaPWM_Reg[2]->Pwm1Enable.Divider = (d) - 1U;
}
static inline void AmbaCSL_Pwm10SetPulseDivider(UINT32 d)
{
    pAmbaPWM_Reg[2]->Pwm2Enable.Divider = (d) - 1U;
}
static inline void AmbaCSL_Pwm11SetPulseDivider(UINT32 d)
{
    pAmbaPWM_Reg[2]->Pwm3Enable.Divider = (d) - 1U;
}

static inline void AmbaCSL_Pwm0ConfigPulsePeriod(UINT32 OnTicks, UINT32 OffTicks)
{
    pAmbaPWM_Reg[0]->Pwm0CtrlBank = ((OnTicks - 1U) << 16U) | (OffTicks - 1U);
}
static inline void AmbaCSL_Pwm1ConfigPulsePeriod(UINT32 OnTicks, UINT32 OffTicks)
{
    pAmbaPWM_Reg[0]->Pwm1CtrlBank = ((OnTicks - 1U) << 16U) | (OffTicks - 1U);
}
static inline void AmbaCSL_Pwm2ConfigPulsePeriod(UINT32 OnTicks, UINT32 OffTicks)
{
    pAmbaPWM_Reg[0]->Pwm2CtrlBank = ((OnTicks - 1U) << 16U) | (OffTicks - 1U);
}
static inline void AmbaCSL_Pwm3ConfigPulsePeriod(UINT32 OnTicks, UINT32 OffTicks)
{
    pAmbaPWM_Reg[0]->Pwm3CtrlBank = ((OnTicks - 1U) << 16U) | (OffTicks - 1U);
}
static inline void AmbaCSL_Pwm4ConfigPulsePeriod(UINT32 OnTicks, UINT32 OffTicks)
{
    pAmbaPWM_Reg[1]->Pwm0CtrlBank = ((OnTicks - 1U) << 16U) | (OffTicks - 1U);
}
static inline void AmbaCSL_Pwm5ConfigPulsePeriod(UINT32 OnTicks, UINT32 OffTicks)
{
    pAmbaPWM_Reg[1]->Pwm1CtrlBank = ((OnTicks - 1U) << 16U) | (OffTicks - 1U);
}
static inline void AmbaCSL_Pwm6ConfigPulsePeriod(UINT32 OnTicks, UINT32 OffTicks)
{
    pAmbaPWM_Reg[1]->Pwm2CtrlBank = ((OnTicks - 1U) << 16U) | (OffTicks - 1U);
}
static inline void AmbaCSL_Pwm7ConfigPulsePeriod(UINT32 OnTicks, UINT32 OffTicks)
{
    pAmbaPWM_Reg[1]->Pwm3CtrlBank = ((OnTicks - 1U) << 16U) | (OffTicks - 1U);
}
static inline void AmbaCSL_Pwm8ConfigPulsePeriod(UINT32 OnTicks, UINT32 OffTicks)
{
    pAmbaPWM_Reg[2]->Pwm0CtrlBank = ((OnTicks - 1U) << 16U) | (OffTicks - 1U);
}
static inline void AmbaCSL_Pwm9ConfigPulsePeriod(UINT32 OnTicks, UINT32 OffTicks)
{
    pAmbaPWM_Reg[2]->Pwm1CtrlBank = ((OnTicks - 1U) << 16U) | (OffTicks - 1U);
}
static inline void AmbaCSL_Pwm10ConfigPulsePeriod(UINT32 OnTicks, UINT32 OffTicks)
{
    pAmbaPWM_Reg[2]->Pwm2CtrlBank = ((OnTicks - 1U) << 16U) | (OffTicks - 1U);
}
static inline void AmbaCSL_Pwm11ConfigPulsePeriod(UINT32 OnTicks, UINT32 OffTicks)
{
    pAmbaPWM_Reg[2]->Pwm3CtrlBank = ((OnTicks - 1U) << 16U) | (OffTicks - 1U);
}

#endif /* AMBA_CSL_PWM_H */
