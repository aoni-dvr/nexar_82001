/**
 *  @file AmbaRTSL_PWM.c
 *
 *  @copyright Copyright (c) 2020 Ambarella International LP
 *
 *  This file and its contents ("Software") are protected by intellectual
 *  property rights including, without limitation, U.S. and/or foreign
 *  copyrights. This Software is also the confidential and proprietary
 *  information of Ambarella International LP and its licensors. You may not use, reproduce,
 *  disclose, distribute, modify, or otherwise prepare derivative works of this
 *  Software or any portion thereof except pursuant to a signed license agreement
 *  or nondisclosure agreement with Ambarella International LP or its authorized affiliates.
 *  In the absence of such an agreement, you agree to promptly notify and return
 *  this Software to Ambarella International LP.
 *
 *  THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
 *  INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF NON-INFRINGEMENT,
 *  MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 *  IN NO EVENT SHALL AMBARELLA INTERNATIONAL LP OR ITS AFFILIATES BE LIABLE FOR ANY DIRECT,
 *  INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 *  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 *  LOSS OF USE, DATA, OR PROFITS; COMPUTER FAILURE OR MALFUNCTION; OR BUSINESS
 *  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 *  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 *  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *  POSSIBILITY OF SUCH DAMAGE.
 *
 *  @details PWM RTSL APIs
 *
 */

#include "AmbaTypes.h"
#include "AmbaDef.h"

#include "AmbaRTSL_PWM.h"
#include "AmbaCSL_PWM.h"
#if defined(CONFIG_QNX)
#include "hw/ambarella_clk.h"
#include "Amba_pwm.h"
#else
#include "AmbaRTSL_PLL.h"
#endif

#define AMBA_PWM_DEFAULT_FREQ   3000000U

typedef struct {
    UINT32  PulseDivider;
    UINT16  OnTicks;                /* Output logic high duration ticks */
    UINT16  OffTicks;               /* Output logic low duration ticks */
} AMBA_PWM_CTRL_s;

static AMBA_PWM_CTRL_s AmbaPwmCtrl[AMBA_NUM_PWM_CHANNEL];

#if defined(CONFIG_QNX)
/**
 *  PWM_SetPwmClkConfig - Set the PWM clock configuration
 */
static UINT32 PWM_SetPwmClkConfig(UINT32 PwmClkConfig)
{
    UINT32 RetVal = PWM_ERR_NONE;
    int fd;
    clk_config_t ClkFreq;

    fd = open("/dev/clock", O_RDWR);

    if (fd == -1) {
        RetVal = PWM_ERR_ARG;
    } else {
        ClkFreq.id = AMBA_CLK_PWM;
        ClkFreq.config = PwmClkConfig;

        if (devctl(fd, DCMD_CLOCK_CLK_CONFIG, &ClkFreq, sizeof(clk_config_t), NULL) != EOK) {
            RetVal = PWM_ERR_ARG;
        }

        close(fd);
    }

    if (RetVal) {
        printf("%s err: %d \n", __FUNCTION__, RetVal);
    }

    return RetVal;
}

/**
 *  PWM_SetPwmClk - Set the PWM clock frequency
 */
static UINT32 PWM_SetPwmClk(UINT32 Frequency)
{
    UINT32 RetVal = PWM_ERR_NONE;
    int fd;
    clk_freq_t ClkFreq;

    fd = open("/dev/clock", O_RDWR);

    if (fd == -1) {
        RetVal = PWM_ERR_ARG;
    } else {
        ClkFreq.id = AMBA_CLK_PWM;
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

/**
 *  PWM_GetPwmClk - Get the PWM clock frequency
 */
static UINT32 PWM_GetPwmClk(UINT32 *Frequency)
{
    UINT32 RetVal = PWM_ERR_NONE;
    int fd;
    clk_freq_t ClkFreq;

    fd = open("/dev/clock", O_RDWR);

    if (fd == -1) {
        RetVal = PWM_ERR_ARG;
    } else {
        ClkFreq.id = AMBA_CLK_PWM;

        if (devctl(fd, DCMD_CLOCK_GET_FREQ, &ClkFreq, sizeof(ClkFreq), NULL) != EOK) {
            RetVal = PWM_ERR_ARG;
        }

        close(fd);

        *Frequency = ClkFreq.freq;
    }

    if (RetVal) {
        printf("%s err: %d \n", __FUNCTION__, RetVal);
    }

    return RetVal;
}
#endif
/**
 *  AmbaRTSL_PwmInit - Initialize PWM module
 */
void AmbaRTSL_PwmInit(void)
{
    //Set PWM source clock as ref clock
#if defined(CONFIG_QNX)
    (void)PWM_SetPwmClkConfig(3U);
    (void)PWM_SetPwmClk(AMBA_PWM_DEFAULT_FREQ);
#else
    (void)AmbaRTSL_PllSetPwmClkConfig(AMBA_PLL_PWM_CLK_REF);
    (void)AmbaRTSL_PllSetPwmClk(AMBA_PWM_DEFAULT_FREQ);
#endif
}

/**
 *  AmbaRTSL_PwmSetConfig - Set PWM base clock frequency
 *  @param[in] PinID PWM pin ID
 *  @param[in] BaseFreq PWM signal configuration
 *  @return error code
 */
UINT32 AmbaRTSL_PwmSetConfig(UINT32 PinID, UINT32 BaseFreq)
{
    UINT32 RetVal = PWM_ERR_NONE;
    UINT32 PulseDivider = 0;
    UINT32 PwmFrequency;

    if ((PinID >= AMBA_NUM_PWM_CHANNEL) || (BaseFreq == 0U)) {
        RetVal = PWM_ERR_ARG;
    } else {
        AmbaRTSL_PwmStop(PinID);

#if defined(CONFIG_QNX)
        PWM_GetPwmClk(&PwmFrequency);
#else
        PwmFrequency = AmbaRTSL_PllGetPwmClk();
#endif
        PulseDivider = GetMaxValU32(GetRoundUpValU32(PwmFrequency, BaseFreq), 1);
        AmbaPwmCtrl[PinID].PulseDivider = PulseDivider;
    }

    return RetVal;
}

/**
 *  AmbaRTSL_PwmStart - Start PWM signal generator
 *  @param[in] PinID PWM pin ID
 *  @param[in] Period Number of base clock ticks to form a PWM wave (0~65535)
 *  @param[in] Duty PWM signal configuration
 *  @return error code
 */
UINT32 AmbaRTSL_PwmStart(UINT32 PinID, UINT32 Period, UINT32 Duty)
{
    UINT32 RetVal = PWM_ERR_NONE;
    UINT32 PulseDivider = 0U;
    AMBA_PWM_CTRL_s *pPwmCtrl = &AmbaPwmCtrl[PinID];

    if ((PinID >= AMBA_NUM_PWM_CHANNEL) || (Period == 0U) || (Duty > Period) || (pPwmCtrl->PulseDivider == 0U)) {
        RetVal = PWM_ERR_ARG;
    } else {
        PulseDivider = pPwmCtrl->PulseDivider;
        pPwmCtrl->OnTicks = (UINT16)Duty;
        pPwmCtrl->OffTicks = (UINT16)(Period - Duty);

        switch (PinID) {
        case AMBA_PWM_CHANNEL0:
            AmbaCSL_Pwm0SetPulseDivider(PulseDivider);
            AmbaCSL_Pwm0ConfigPulsePeriod(pPwmCtrl->OnTicks, pPwmCtrl->OffTicks);
            AmbaCSL_Pwm0Enable();
            break;

        case AMBA_PWM_CHANNEL1:
            AmbaCSL_Pwm1SetPulseDivider(PulseDivider);
            AmbaCSL_Pwm01SetIndividual();
            AmbaCSL_Pwm1ConfigPulsePeriod(pPwmCtrl->OnTicks, pPwmCtrl->OffTicks);
            AmbaCSL_Pwm1Enable();
            break;

        case AMBA_PWM_CHANNEL2:
            AmbaCSL_Pwm2SetPulseDivider(PulseDivider);
            AmbaCSL_Pwm2ConfigPulsePeriod(pPwmCtrl->OnTicks, pPwmCtrl->OffTicks);
            AmbaCSL_Pwm2Enable();
            break;

        case AMBA_PWM_CHANNEL3:
            AmbaCSL_Pwm3SetPulseDivider(PulseDivider);
            AmbaCSL_Pwm23SetIndividual();
            AmbaCSL_Pwm3ConfigPulsePeriod(pPwmCtrl->OnTicks, pPwmCtrl->OffTicks);
            AmbaCSL_Pwm3Enable();
            break;

        case AMBA_PWM_CHANNEL4:
            AmbaCSL_Pwm4SetPulseDivider(PulseDivider);
            AmbaCSL_Pwm4ConfigPulsePeriod(pPwmCtrl->OnTicks, pPwmCtrl->OffTicks);
            AmbaCSL_Pwm4Enable();
            break;

        case AMBA_PWM_CHANNEL5:
            AmbaCSL_Pwm5SetPulseDivider(PulseDivider);
            AmbaCSL_Pwm45SetIndividual();
            AmbaCSL_Pwm5ConfigPulsePeriod(pPwmCtrl->OnTicks, pPwmCtrl->OffTicks);
            AmbaCSL_Pwm5Enable();
            break;

        case AMBA_PWM_CHANNEL6:
            AmbaCSL_Pwm6SetPulseDivider(PulseDivider);
            AmbaCSL_Pwm6ConfigPulsePeriod(pPwmCtrl->OnTicks, pPwmCtrl->OffTicks);
            AmbaCSL_Pwm6Enable();
            break;

        case AMBA_PWM_CHANNEL7:
            AmbaCSL_Pwm7SetPulseDivider(PulseDivider);
            AmbaCSL_Pwm67SetIndividual();
            AmbaCSL_Pwm7ConfigPulsePeriod(pPwmCtrl->OnTicks, pPwmCtrl->OffTicks);
            AmbaCSL_Pwm7Enable();
            break;

        case AMBA_PWM_CHANNEL8:
            AmbaCSL_Pwm8SetPulseDivider(PulseDivider);
            AmbaCSL_Pwm8ConfigPulsePeriod(pPwmCtrl->OnTicks, pPwmCtrl->OffTicks);
            AmbaCSL_Pwm8Enable();
            break;

        case AMBA_PWM_CHANNEL9:
            AmbaCSL_Pwm9SetPulseDivider(PulseDivider);
            AmbaCSL_Pwm89SetIndividual();
            AmbaCSL_Pwm9ConfigPulsePeriod(pPwmCtrl->OnTicks, pPwmCtrl->OffTicks);
            AmbaCSL_Pwm9Enable();
            break;

        case AMBA_PWM_CHANNEL10:
            AmbaCSL_Pwm10SetPulseDivider(PulseDivider);
            AmbaCSL_Pwm10ConfigPulsePeriod(pPwmCtrl->OnTicks, pPwmCtrl->OffTicks);
            AmbaCSL_Pwm10Enable();
            break;

        case AMBA_PWM_CHANNEL11:
            AmbaCSL_Pwm11SetPulseDivider(PulseDivider);
            AmbaCSL_Pwm1011SetIndividual();
            AmbaCSL_Pwm11ConfigPulsePeriod(pPwmCtrl->OnTicks, pPwmCtrl->OffTicks);
            AmbaCSL_Pwm11Enable();
            break;

        default:
            RetVal = PWM_ERR_ARG;
            break;
        }
    }

    return RetVal;
}

/**
 *  AmbaRTSL_PwmStop - Disable PWM to stop generating signal pulse
 *  @param[in] PinID PWM pin ID
 */
void AmbaRTSL_PwmStop(UINT32 PinID)
{
    AMBA_PWM_CTRL_s *pPwmCtrl = &AmbaPwmCtrl[PinID];

    if (PinID < AMBA_NUM_PWM_CHANNEL) {
        pPwmCtrl->PulseDivider = 0U;
    }

    switch (PinID) {
    case AMBA_PWM_CHANNEL0:
        AmbaCSL_Pwm0Disable();
        break;
    case AMBA_PWM_CHANNEL1:
        AmbaCSL_Pwm1Disable();
        break;
    case AMBA_PWM_CHANNEL2:
        AmbaCSL_Pwm2Disable();
        break;
    case AMBA_PWM_CHANNEL3:
        AmbaCSL_Pwm3Disable();
        break;
    case AMBA_PWM_CHANNEL4:
        AmbaCSL_Pwm4Disable();
        break;
    case AMBA_PWM_CHANNEL5:
        AmbaCSL_Pwm5Disable();
        break;
    case AMBA_PWM_CHANNEL6:
        AmbaCSL_Pwm6Disable();
        break;
    case AMBA_PWM_CHANNEL7:
        AmbaCSL_Pwm7Disable();
        break;
    case AMBA_PWM_CHANNEL8:
        AmbaCSL_Pwm8Disable();
        break;
    case AMBA_PWM_CHANNEL9:
        AmbaCSL_Pwm9Disable();
        break;
    case AMBA_PWM_CHANNEL10:
        AmbaCSL_Pwm10Disable();
        break;
    case AMBA_PWM_CHANNEL11:
        AmbaCSL_Pwm11Disable();
        break;
    default:
        /* do nothing */
        break;
    }
}

/**
 *  AmbaRTSL_PwmGetInfo - Get PWM configuration
 *  @param[in] PinID PWM pin ID
 *  @param[out] pActualBaseFreq PWM base frequency (Hz)
 *  @return error code
 */
UINT32 AmbaRTSL_PwmGetInfo(UINT32 PinID, UINT32 *pActualBaseFreq)
{
    UINT32 RetVal = PWM_ERR_NONE;
    UINT32 PwmFrequency;

    if ((PinID >= AMBA_NUM_PWM_CHANNEL) || (pActualBaseFreq == NULL)) {
        RetVal = PWM_ERR_ARG;
    } else {
#if defined(CONFIG_QNX)
        PWM_GetPwmClk(&PwmFrequency);
#else
        PwmFrequency = AmbaRTSL_PllGetPwmClk();
#endif
        if (AmbaPwmCtrl[PinID].PulseDivider != 0x0U) {
            *pActualBaseFreq = PwmFrequency / AmbaPwmCtrl[PinID].PulseDivider;
        }
    }

    return RetVal;
}

/**
 *  AmbaRTSL_PwmHalCoreFreqChg - Reset the current pwm source clk freq to default value
 */
void AmbaRTSL_PwmHalCoreFreqChg(void)
{
#if defined(CONFIG_QNX)
    (void)PWM_SetPwmClk(AMBA_PWM_DEFAULT_FREQ);
#else
    (void)AmbaRTSL_PllSetPwmClk(AMBA_PWM_DEFAULT_FREQ);
#endif
}
