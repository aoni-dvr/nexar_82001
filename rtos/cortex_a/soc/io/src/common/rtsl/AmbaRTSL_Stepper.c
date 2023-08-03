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
#include "AmbaMisraFix.h"

#include "AmbaRTSL_Stepper.h"
#include "AmbaCSL_Stepper.h"

#if defined(CONFIG_QNX)
#include "hw/ambarella_clk.h"
#include "Amba_pwm.h"
#define AmbaMisra_TypeCast32(a, b) memcpy((void *)a, (void *)b, 4)
#else
#include "AmbaRTSL_GIC.h"
#include "AmbaRTSL_PLL.h"
#endif

typedef struct {
    UINT32 PulseWidth;
    UINT32 ActualSampleFreq;
} AMBA_PWM_STEP_STATUS_s;

static AMBA_PWM_STEP_STATUS_s PwmStepStatus[AMBA_NUM_PWM_STEPPER_CHANNEL];

/* Call back functions when completed transactions */
static void (*AmbaMotorIsrCallBack)(UINT32 ChanNo);
#ifndef CONFIG_QNX
static void Motor_Isr(UINT32 IntID, UINT32 IsrArg);
#endif

#if defined(CONFIG_QNX)
/**
 *  STEPPER_GetMontorClk - Get the motor clock frequency
 */
static UINT32 STEPPER_GetMontorClk(UINT32 *Frequency)
{
    UINT32 RetVal = PWM_ERR_NONE;
    int fd;
    clk_freq_t ClkFreq;

    fd = open("/dev/clock", O_RDWR);

    if (fd == -1) {
        RetVal = PWM_ERR_ARG;
    } else {
        ClkFreq.id = AMBA_CLK_MOTOR;
        ClkFreq.freq = *Frequency;

        if (devctl(fd, DCMD_CLOCK_GET_FREQ, &ClkFreq, sizeof(ClkFreq), NULL) != EOK) {
            RetVal = PWM_ERR_ARG;
        }

        close(fd);
    }

    printf("%s err: %d \n", __FUNCTION__, RetVal);

    return RetVal;
}

/**
 *  STEPPER_SetMontorClk - Set the Montor clock frequency
 */
static UINT32 STEPPER_SetMontorClk(UINT32 Frequency)
{
    UINT32 RetVal = PWM_ERR_NONE;
    int fd;
    clk_freq_t ClkFreq;

    fd = open("/dev/clock", O_RDWR);

    if (fd == -1) {
        RetVal = PWM_ERR_ARG;
    } else {
        ClkFreq.id = AMBA_CLK_MOTOR;
        ClkFreq.freq = Frequency;

        if (devctl(fd, DCMD_CLOCK_SET_FREQ, &ClkFreq, sizeof(ClkFreq), NULL) != EOK) {
            RetVal = PWM_ERR_ARG;
        }

        close(fd);
    }

    if (RetVal) {
        printf("%s err: %d \n", __FUNCTION__, RetVal);
    }

    return RetVal;
}
#else
/**
 *  AmbaRTSL_PwmStepInit - PWM driver initialization
 */
void AmbaRTSL_PwmStepInit(void)
{
    UINT32 i;
    AMBA_INT_CONFIG_s IntConfig = {0};

    (void)AmbaRTSL_PllSetMotorClk(AmbaRTSL_PllGetClkRefFreq());

    IntConfig.TriggerType = INT_TRIG_HIGH_LEVEL;            /* Sensitivity type */
    IntConfig.IrqType = INT_TYPE_IRQ;                       /* Interrupt type: IRQ or FIQ */
    IntConfig.CpuTargets = 0x01U;                           /* Target cores */
    (void)AmbaRTSL_GicIntConfig(AMBA_INT_SPI_ID46_MOTOR, &IntConfig, Motor_Isr, 0U);
    (void)AmbaRTSL_GicIntEnable(AMBA_INT_SPI_ID46_MOTOR);

    for (i = 0U; i < AMBA_NUM_PWM_STEPPER_CHANNEL; i++) {
        PwmStepStatus[i].ActualSampleFreq = 0U;
        PwmStepStatus[i].PulseWidth = 0U;
    }
}
#endif

/**
 *  AmbaRTSL_PwmStepHookIntHandler - Hook ISR
 *  @param[in] IntFunc Interrupt service routine
 */
void AmbaRTSL_PwmStepHookIntHandler(AMBA_PWM_STEP_ISR_f IntFunc)
{
    AmbaMotorIsrCallBack = IntFunc;
}

/**
 *  AmbaRTSL_PwmStepSetConfig - Configure a PWM encoder
 *  @param[in] PinGrpID PWM channel number
 *  @param[in] BaseFreq Base frequency (Hz)
 *  @param[in] PulseWidth Number of base clock ticks to form a PWM wave.
 *  @return error code
 */
UINT32 AmbaRTSL_PwmStepSetConfig(UINT32 PinGrpID, UINT32 BaseFreq, UINT32 PulseWidth)
{
    UINT32 RetVal = PWM_ERR_NONE;
    volatile AMBA_PWM_STEPPER_MOTOR_REG_s *pStepperReg = NULL;
    AMBA_PWM_STEP_CTRL_REG_s StepperCtrl = {0U};
    UINT32 Divider = 0U;
    UINT32 Tmp = 0U;
#if defined(CONFIG_QNX)
    UINT32 Freq = 0U;
#endif

    if ((PinGrpID >= AMBA_NUM_PWM_STEPPER_CHANNEL) || (BaseFreq == 0U) || (PulseWidth == 0U)) {
        RetVal = PWM_ERR_ARG;
    } else {
#if defined(CONFIG_QNX)
        (void)STEPPER_SetMontorClk(24000000U);
        (void)STEPPER_GetMontorClk(&Freq);
        Divider = (Freq / (BaseFreq << 1U)) - 1U;
        PwmStepStatus[PinGrpID].ActualSampleFreq = (Freq / (Divider + 1U)) >> 1U;
#else
        (void)AmbaRTSL_PllSetMotorClk(AmbaRTSL_PllGetClkRefFreq());

        Divider = (AmbaRTSL_PllGetMotorClk() / (BaseFreq << 1U)) - 1U;
        PwmStepStatus[PinGrpID].ActualSampleFreq = (AmbaRTSL_PllGetMotorClk() / (Divider + 1U)) >> 1U;
#endif
        PwmStepStatus[PinGrpID].PulseWidth = PulseWidth;

        pStepperReg = AmbaCSL_StepperGetReg(PinGrpID);

        StepperCtrl.ClkDivider = (UINT16)Divider;
        StepperCtrl.PatternSize = (UINT8)PulseWidth;
        StepperCtrl.UseLastPinState = 0U;
        StepperCtrl.LastPinState = 0U;
        StepperCtrl.Reset = 1U;

        AmbaMisra_TypeCast32(&Tmp, &StepperCtrl);
        pStepperReg->Ctrl = Tmp;
    }

    return RetVal;
}

/**
 *  AmbaRTSL_PwmStepSetDuty - This function configures the next pulse of a pin belong to the specified pin group
 *  @param[in] PinGrpID Stepper channel number
 *  @param[in] PinID The index of a pin within the pin group.
 *  @param[in] Duty Number of base clock ticks to form the signal high state duration of a PWM wave.
 *  @return error code
 */
UINT32 AmbaRTSL_PwmStepSetDuty(UINT32 PinGrpID, UINT32 PinID, UINT32 Duty)
{
    UINT32 RetVal = PWM_ERR_NONE;
    volatile AMBA_PWM_STEPPER_MOTOR_REG_s *pStepperReg = NULL;
    UINT32 MsbPattern;
    UINT32 LsbPattern;

    if ((PinGrpID >= AMBA_NUM_PWM_STEPPER_CHANNEL) || (PinID >= AMBA_NUM_PWM_STEPPER_CHANNEL_PIN) || (Duty == 0U)) {
        RetVal = PWM_ERR_ARG;
    } else {
        pStepperReg = AmbaCSL_StepperGetReg(PinGrpID);

        MsbPattern = 0U;
        LsbPattern = 0U;
        if (Duty > 32U) {
            MsbPattern = 0xffffffffU;
            LsbPattern = (0xffffffffU >> (Duty - 32U));
            LsbPattern = LsbPattern ^ 0xffffffffU;
        } else {
            MsbPattern = ((UINT32)1U << (32U - Duty)) - 1U;
            MsbPattern = MsbPattern ^ 0xffffffffU;
            LsbPattern = 0U;
        }
        pStepperReg->Pattern[PinID].Data[0] = MsbPattern;
        pStepperReg->Pattern[PinID].Data[1] = LsbPattern;
    }

    return RetVal;
}

/**
 *  AmbaRTSL_PwmStepAct - The function generates the next pulse waves of the specified PWM pin group.
 *  @param[in] PinGrpID Stepper channel number
 *  @return error code
 */
UINT32 AmbaRTSL_PwmStepAct(UINT32 PinGrpID)
{
    UINT32 RetVal = PWM_ERR_NONE;
    AMBA_PWM_STEP_COUNT_REG_s StepperCount = {0U};
    volatile AMBA_PWM_STEPPER_MOTOR_REG_s *pStepperReg = NULL;
    UINT32 Tmp = 0U;

    if (PinGrpID >= AMBA_NUM_PWM_STEPPER_CHANNEL) {
        RetVal = PWM_ERR_ARG;
    } else {
        pStepperReg = AmbaCSL_StepperGetReg(PinGrpID);

        StepperCount.Rewind = 0;
        StepperCount.RepeatFirst = 0;
        StepperCount.RepeatLast = 0;
        StepperCount.NumPhase = (UINT16)(PwmStepStatus[PinGrpID].PulseWidth - 1U);

        AmbaMisra_TypeCast32(&Tmp, &StepperCount);
        pStepperReg->Count = Tmp;
    }

    return RetVal;
}

/**
 *  AmbaRTSL_PwmStepGetStatus - Get steper status
 *  @param[in] PinGrpID Stepper channel number
 *  @param[out] pActualBaseFreq Base frequency
 *  @return error code
 */
UINT32 AmbaRTSL_PwmStepGetStatus(UINT32 PinGrpID, UINT32 *pActualBaseFreq)
{
    UINT32 RetVal = PWM_ERR_NONE;

    if ((PinGrpID >= AMBA_NUM_PWM_STEPPER_CHANNEL) || (pActualBaseFreq == NULL)) {
        RetVal = PWM_ERR_ARG;
    } else {
        *pActualBaseFreq = PwmStepStatus[PinGrpID].ActualSampleFreq;
    }

    return RetVal;
}

#ifndef CONFIG_QNX
/**
 *  Motor_Isr - Pwm/Stepper/MicroStepper completed ISR
 *  @param[in] IntID Interrupt ID
 *  @param[in] IsrArg Optional argument of interrupt handler
 */
#pragma GCC push_options
#pragma GCC target("general-regs-only")
static void Motor_Isr(UINT32 IntID, UINT32 IsrArg)
{
    AMBA_PWM_STEP_INT_STATUS_REG_s IsrStatus;

    IsrStatus = pAmbaPwmStep_Reg->StepperIntStatus;

    if (IsrStatus.StepperAEndInt == 1U) {
        AmbaCSL_StepperAIntDisable();
        if ( AmbaMotorIsrCallBack != NULL ) {
            AmbaMotorIsrCallBack(AMBA_PWM_STEPPER_CHANNEL_A);
        }
    }
    if (IsrStatus.StepperBEndInt == 1U) {
        AmbaCSL_StepperBIntDisable();
        if ( AmbaMotorIsrCallBack != NULL ) {
            AmbaMotorIsrCallBack(AMBA_PWM_STEPPER_CHANNEL_B);
        }
    }
    if (IsrStatus.StepperCEndInt == 1U) {
        AmbaCSL_StepperCIntDisable();
        if ( AmbaMotorIsrCallBack != NULL ) {
            AmbaMotorIsrCallBack(AMBA_PWM_STEPPER_CHANNEL_C);
        }
    }

    AmbaMisra_TouchUnused(&IntID);
    AmbaMisra_TouchUnused(&IsrArg);
}
#pragma GCC pop_options
#endif