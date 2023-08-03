/**
 *  @file AmbaSYS.c
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
 *  @details System Control APIs
 *
 */

#include "AmbaTypes.h"
#include "AmbaKAL.h"

#include "AmbaMisraFix.h"

#include "AmbaDrvEntry.h"
#include "AmbaSYS.h"
#include "AmbaSYS_Ctrl.h"
#include "AmbaSpiNOR_Ctrl.h"

#include "AmbaRTSL_PLL.h"
#include "AmbaRTSL_PWC.h"
#include "AmbaRTSL_ORC.h"
#include "AmbaRTSL_WDT.h"
#include "AmbaCSL_Scratchpad.h"

#ifdef CONFIG_BOOT_CORE_SRTOS
__attribute__((section(".data"))) UINT32 AmbaRtosCoreId = (UINT32)CONFIG_BOOT_CORE_SRTOS;
#else
__attribute__((section(".data"))) UINT32 AmbaRtosCoreId = (UINT32)0;
#endif

static UINT32 AmbaSysMaxIdleCycleTime = 500U;   /* Half-second by default */

static AMBA_KAL_MUTEX_t AmbaSysMutex;
static UINT32 WakeUpReason = 0U;

/**
 *  AmbaSYS_DrvEntry - Initializes driver-wide data structures and resources
 *  @return error code
 *  @note this function is an internal using only API
 */
UINT32 AmbaSYS_DrvEntry(void)
{
    static char AmbaSysMutexName[16] = "AmbaSysMutex";
    UINT32 RetVal = SYS_ERR_NONE;
    UINT32 SwReset = 0U;

    if (AmbaKAL_MutexCreate(&AmbaSysMutex, AmbaSysMutexName) != KAL_ERR_NONE) {
        RetVal = SYS_ERR_UNEXPECTED;
    }

    SwReset = AmbaRTSL_PwcCheckSwReset();
    WakeUpReason |= (SwReset << 0x6U);

    return RetVal;
}

/**
 *  AmbaSYS_GetRtosCoreId - This function used to get the info about which core executing RTOS
 *  @pCoreId[out] Pointer to store core ID
 *  @return error code
 */
UINT32 AmbaSYS_GetRtosCoreId(UINT32 *pCoreId)
{
    *pCoreId = AmbaRtosCoreId;

    return OK;
}

/**
 *  AmbaSYS_SetExtInputClkInfo - The function is used to assign the specified external clock frequency value.
 *  @param[in] ExtClkID External clock source ID
 *  @param[in] Freq Clock frequency in Hz.
 *  @return error code
 */
UINT32 AmbaSYS_SetExtInputClkInfo(UINT32 ExtClkID, UINT32 Freq)
{
    AMBA_PLL_EXT_CLK_CONFIG_s ExtClkConfig;
    UINT32 RetVal = OK;

    (void)AmbaRTSL_PllGetExtClkConfig(&ExtClkConfig);

    if (ExtClkID == AMBA_SYS_EXT_CLK_AUDIO0) {
        ExtClkConfig.ExtAudioClkFreq = Freq;
    } else if (ExtClkID == AMBA_SYS_EXT_CLK_AUDIO1) {
        ExtClkConfig.ExtAudio1ClkFreq = Freq;
    } else if (ExtClkID == AMBA_SYS_EXT_CLK_ETHERNET) {
        ExtClkConfig.ExtEthernetClkFreq = Freq;
    } else if (ExtClkID == AMBA_SYS_EXT_CLK_SENSOR_IN) {
        ExtClkConfig.ExtSensorInputClkFreq = Freq;
    } else if (ExtClkID == AMBA_SYS_EXT_CLK_SENSOR_SPCLK0) {
        ExtClkConfig.SpclkMuxSelect = AMBA_PLL_MUX_SPCLK_C0;
        ExtClkConfig.ExtMuxLvdsSpclkClkFreq = Freq;
    } else if (ExtClkID == AMBA_SYS_EXT_CLK_SENSOR_SPCLK1) {
        ExtClkConfig.SpclkMuxSelect = AMBA_PLL_MUX_SPCLK_C1;
        ExtClkConfig.ExtMuxLvdsSpclkClkFreq = Freq;
    } else if (ExtClkID == AMBA_SYS_EXT_CLK_SENSOR_SPCLK2) {
        ExtClkConfig.SpclkMuxSelect = AMBA_PLL_MUX_SPCLK_C2;
        ExtClkConfig.ExtMuxLvdsSpclkClkFreq = Freq;
    } else if (ExtClkID == AMBA_SYS_EXT_CLK_SENSOR_SPCLK3) {
        ExtClkConfig.SpclkMuxSelect = AMBA_PLL_MUX_SPCLK_C3;
        ExtClkConfig.ExtMuxLvdsSpclkClkFreq = Freq;
    } else if (ExtClkID == AMBA_SYS_EXT_CLK_SENSOR_SPCLK4) {
        ExtClkConfig.SpclkMuxSelect = AMBA_PLL_MUX_SPCLK_C4;
        ExtClkConfig.ExtMuxLvdsSpclkClkFreq = Freq;
    } else if (ExtClkID == AMBA_SYS_EXT_CLK_SENSOR_SPCLK5) {
        ExtClkConfig.SpclkMuxSelect = AMBA_PLL_MUX_SPCLK_C5;
        ExtClkConfig.ExtMuxLvdsSpclkClkFreq = Freq;
    } else {
        RetVal = SYS_ERR_ARG;
    }

    (void)AmbaRTSL_PllSetExtClkConfig(&ExtClkConfig);

    return RetVal;
}

/**
 *  AmbaSYS_SetClkFreq - The function is used to set the specified clock frequency.
 *  @param[in] ClkID Clock ID
 *  @param[in] DesiredFreq Desired clock frequency in Hz.
 *  @param[out] pActualFreq The actual clock frequency in Hz.
 *  @return error code
 */
UINT32 AmbaSYS_SetClkFreq(UINT32 ClkID, UINT32 DesiredFreq, UINT32 * pActualFreq)
{
    UINT32 RetVal = SYS_ERR_NONE;
    UINT32 ActualFreq = 0U;

    if (AmbaKAL_MutexTake(&AmbaSysMutex, KAL_WAIT_FOREVER) != KAL_ERR_NONE) {
        RetVal = SYS_ERR_MUTEX;
    } else {
        if (ClkID == AMBA_SYS_CLK_CORE) {
            (void)AmbaRTSL_PllSetCoreClk(DesiredFreq);
            ActualFreq = AmbaRTSL_PllGetCoreClk();
        } else if (ClkID == AMBA_SYS_CLK_IDSP) {
            (void)AmbaRTSL_PllSetIdspClk(DesiredFreq);
            ActualFreq = AmbaRTSL_PllGetIdspClk();
        } else if (ClkID == AMBA_SYS_CLK_VISION) {
            (void)AmbaRTSL_PllSetVisionClk(DesiredFreq);
            ActualFreq = AmbaRTSL_PllGetVisionClk();
        } else if (ClkID == AMBA_SYS_CLK_FEX) {
            (void)AmbaRTSL_PllSetFexClk(DesiredFreq);
            ActualFreq = AmbaRTSL_PllGetFexClk();
        } else if (ClkID == AMBA_SYS_CLK_AUD_0) {
            (void)AmbaRTSL_PllFineAdjAudioClk(DesiredFreq);
            ActualFreq = AmbaRTSL_PllGetAudioClk();
        } else if (ClkID == AMBA_SYS_CLK_VID_IN0) {
            (void)AmbaRTSL_PllFineAdjSensor0Clk(DesiredFreq);
            ActualFreq = AmbaRTSL_PllGetSensor0Clk();
        } else if (ClkID == AMBA_SYS_CLK_VID_IN1) {
            /* gclk_so_pip is from enet pll */
            (void)AmbaRTSL_PllFineAdjEthernetClk(DesiredFreq);
            ActualFreq = AmbaRTSL_PllGetEthernetClk();
        } else if (ClkID == AMBA_SYS_CLK_VID_OUT0) {
            (void)AmbaRTSL_PllFineAdjVoutLcdClk(DesiredFreq);
            ActualFreq = AmbaRTSL_PllGetVoutLcdClk();
        } else if (ClkID == AMBA_SYS_CLK_VID_OUT1) {
            (void)AmbaRTSL_PllFineAdjVoutTvClk(DesiredFreq);
            ActualFreq = AmbaRTSL_PllGetVoutTvClk();
        } else if (ClkID == AMBA_SYS_CLK_REF_OUT0) {
            (void)AmbaRTSL_PllFineAdjSensor0Clk(DesiredFreq);
            ActualFreq = AmbaRTSL_PllGetSensor0Clk();
        } else if (ClkID == AMBA_SYS_CLK_REF_OUT1) {
            (void)AmbaRTSL_PllFineAdjSensor1Clk(DesiredFreq);
            ActualFreq = AmbaRTSL_PllGetSensor1Clk();
        } else {
            RetVal = SYS_ERR_ARG;
        }

        if ((pActualFreq != NULL) && (RetVal == SYS_ERR_NONE)) {
            *pActualFreq = ActualFreq;
        }

        if (AmbaKAL_MutexGive(&AmbaSysMutex) != KAL_ERR_NONE) {
            /* If code reaches here, the system should be corrupted. Overwite the previous return value because it is meaningless. */
            RetVal = SYS_ERR_UNEXPECTED;
        }
    }

    return RetVal;
}

/**
 *  AmbaSYS_GetSubClkFreq - The function is used to get the selected clock frequency.
 *  @param[in] ClkID Clock ID
 *  @param[out] pFreq The actual clock frequency in Hz.
 *  @return error code
 */
static UINT32 AmbaSYS_GetSubClkFreq(UINT32 ClkID, UINT32 * pFreq)
{
    UINT32 RetVal = SYS_ERR_NONE;

    switch (ClkID) {
    case AMBA_SYS_CLK_CORE:
        *pFreq = AmbaRTSL_PllGetCoreClk();
        break;
    case AMBA_SYS_CLK_CORTEX:
        *pFreq = AmbaRTSL_PllGetCortexClk();
        break;
    case AMBA_SYS_CLK_DRAM:
        *pFreq = AmbaRTSL_PllGetDramClk();
        break;
    case AMBA_SYS_CLK_IDSP:
        *pFreq = AmbaRTSL_PllGetIdspClk();
        break;
    case AMBA_SYS_CLK_VISION:
        *pFreq = AmbaRTSL_PllGetVisionClk();
        break;
    case AMBA_SYS_CLK_FEX:
        *pFreq = AmbaRTSL_PllGetFexClk();
        break;
    case AMBA_SYS_CLK_DSP_SYS:
        *pFreq = AmbaRTSL_PllGetAudioClk();
        break;
    case AMBA_SYS_CLK_AUD_0:
        *pFreq = AmbaRTSL_PllGetAudioClk();
        break;
    case AMBA_SYS_CLK_VID_IN0:
        *pFreq = AmbaRTSL_PllGetSensor0Clk();
        break;
    case AMBA_SYS_CLK_VID_IN1:
        /* gclk_so_pip is from enet pll */
        *pFreq = AmbaRTSL_PllGetEthernetClk();
        break;
    case AMBA_SYS_CLK_VID_OUT0:
        *pFreq = AmbaRTSL_PllGetVoutLcdClk();
        break;
    case AMBA_SYS_CLK_VID_OUT1:
        *pFreq = AmbaRTSL_PllGetVoutTvClk();
        break;
    case AMBA_SYS_CLK_REF_OUT0:
        *pFreq = AmbaRTSL_PllGetSensor0Clk();
        break;
    case AMBA_SYS_CLK_REF_OUT1:
        *pFreq = AmbaRTSL_PllGetSensor1Clk();
        break;
    default:
        RetVal = SYS_ERR_ARG;
        break;
    }

    return RetVal;
}

/**
 *  AmbaSYS_GetClkFreq - The function is used to get the specified clock frequency.
 *  @param[in] ClkID Clock ID
 *  @param[out] pFreq The actual clock frequency in Hz.
 *  @return error code
 */
UINT32 AmbaSYS_GetClkFreq(UINT32 ClkID, UINT32 * pFreq)
{
    UINT32 RetVal = SYS_ERR_NONE;

    if (pFreq == NULL) {
        RetVal = SYS_ERR_ARG;
    } else {
        if (AmbaKAL_MutexTake(&AmbaSysMutex, KAL_WAIT_FOREVER) != KAL_ERR_NONE) {
            RetVal = SYS_ERR_MUTEX;
        } else {
            RetVal = AmbaSYS_GetSubClkFreq(ClkID, pFreq);

            if (AmbaKAL_MutexGive(&AmbaSysMutex) != KAL_ERR_NONE) {
                /* If code reaches here, the system should be corrupted. Overwite the previous return value because it is meaningless. */
                RetVal = SYS_ERR_UNEXPECTED;
            }
        }
    }

    return RetVal;
}

/**
 *  AmbaSYS_SetIoClkFreq - Set IO frequency
 *  @param[in] ClkID Clock ID
 *  @param[in] DesiredFreq Target IO clock frequency
 *  @param[out] pActualFreq The actual IO clock frequency
 *  @return error code
 */
UINT32 AmbaSYS_SetIoClkFreq(UINT32 ClkID, UINT32 DesiredFreq, UINT32 * pActualFreq)
{
    UINT32 RetVal = SYS_ERR_NONE;

    if (ClkID < (UINT32)AMBA_CLK_NUM) {
        RetVal = AmbaRTSL_PllSetIOClk(ClkID, DesiredFreq, pActualFreq);
    } else {
        RetVal = SYS_ERR_ARG;
    }

    return RetVal;
}

/**
 *  AmbaSYS_GetIoClkFreq - Get IO frequency
 *  @param[in] ClkID Clock ID
 *  @param[out] pFreq The actual IO clock frequency
 *  @return error code
 */
UINT32 AmbaSYS_GetIoClkFreq(UINT32 ClkID, UINT32 * pFreq)
{
    UINT32 RetVal = SYS_ERR_NONE;

    if ((pFreq != NULL) && (ClkID < (UINT32)AMBA_CLK_NUM)) {
        RetVal = AmbaRTSL_PllGetIOClk(ClkID, pFreq);
    } else {
        RetVal = SYS_ERR_ARG;
    }

    return RetVal;
}

/**
 *  AmbaSYS_SetMaxIdleCycleTime - The function is used to set the maximum idle cycle time.
 *  @param[in] MaxIdleCycleTime The maximum idle cycle time value in system tick (ms)
 *  @return error code
 */
UINT32 AmbaSYS_SetMaxIdleCycleTime(UINT32 MaxIdleCycleTime)
{
    AmbaSysMaxIdleCycleTime = MaxIdleCycleTime;
    return SYS_ERR_NONE;
}

/**
 *  AmbaSYS_GetClkFreq - The function is used to get the maximum idle cycle time.
 *  @param[out] pMaxIdleCycleTime The maximum idle cycle time value in system tick (ms)
 *  @return error code
 */
UINT32 AmbaSYS_GetMaxIdleCycleTime(UINT32 *pMaxIdleCycleTime)
{
    UINT32 RetVal = SYS_ERR_NONE;

    if (pMaxIdleCycleTime == NULL) {
        RetVal = SYS_ERR_ARG;
    } else {
        *pMaxIdleCycleTime = AmbaSysMaxIdleCycleTime;
    }

    return RetVal;
}

/**
 *  AmbaSYS_Reboot - The function is used to reboot the system.
 *  @return error code
 */
UINT32 AmbaSYS_Reboot(void)
{
    UINT32 BootMode, RetVal;

    /* Restore some hardware settings to make boot code is able to be reloaded successfully. */
    /* If a hardware register could be reset by soc soft-reset, it needs not to be programmed here. */
    RetVal = AmbaSYS_GetBootMode(&BootMode);
    if (RetVal == SYS_ERR_NONE) {
        if (BootMode == AMBA_SYS_BOOT_FROM_SPI_NOR) {
            AmbaSPINOR_RestoreRebootClkSetting();
        }
    }

    /* Trigger soc soft-reset */
    AmbaRTSL_PwcReboot();

    return RetVal;
}

/**
 *  AmbaSYS_ChangePowerMode - The function kicks off a power mode change
 *  @param[in] PowerMode Power mode
 *  @return error code
 */
UINT32 AmbaSYS_ChangePowerMode(UINT32 PowerMode)
{
    UINT32 RetVal = SYS_ERR_NONE;

    if (AmbaKAL_MutexTake(&AmbaSysMutex, KAL_WAIT_FOREVER) != KAL_ERR_NONE) {
        RetVal = SYS_ERR_MUTEX;
    } else {
        if (PowerMode == 0U) {
            RetVal = SYS_ERR_IMPL;  /* TODO */
        } else if (PowerMode == 1U) {
            RetVal = SYS_ERR_IMPL;  /* TODO */
        } else {
            RetVal = SYS_ERR_ARG;
        }

        if (AmbaKAL_MutexGive(&AmbaSysMutex) != KAL_ERR_NONE) {
            /* If code reaches here, the system should be corrupted. Overwite the previous return value because it is meaningless. */
            RetVal = SYS_ERR_UNEXPECTED;
        }
    }

    return RetVal;
}

/**
 *  AmbaSYS_ConfigPowerSequence - The function is used to configure one of the power sequence intervals.
 *  @param[in] PwrSeq0 Power sequence interval 0
 *  @param[in] PwrSeq1 Power sequence interval 1
 *  @param[in] PwrSeq2 Power sequence interval 2
 *  @param[in] PwrSeq3 Power sequence interval 3
 *  @return error code
 */
UINT32 AmbaSYS_ConfigPowerSequence(UINT32 PwrSeq0, UINT32 PwrSeq1, UINT32 PwrSeq2, UINT32 PwrSeq3)
{
    AmbaRTSL_PwcConfigPowerSeq(PwrSeq0, PwrSeq1, PwrSeq2, PwrSeq3);

    return SYS_ERR_NONE;
}

/**
 *  AmbaSYS_TrigPowerDownSequence - The function is used to trigger power-down sequence.
 *  @param[in] Option Force PSEQ3 pin state or not.
 *  @return error code
 */
UINT32 AmbaSYS_TrigPowerDownSequence(UINT32 Option)
{
    UINT32 RetVal = SYS_ERR_NONE;

    if (Option >= 2U) {
        RetVal = SYS_ERR_ARG;
    } else {
        AmbaRTSL_PwcForcePseq3State(Option);
        AmbaRTSL_PwcTrigPowerDownSeq();
    }

    return RetVal;
}

/**
 *  AmbaSYS_SetWakeUpAlarm - The function is used to wake up system with RTC service.
 *  @param[in] Countdown Number of seconds before triggering the power-up sequence.
 *  @return error code
 */
UINT32 AmbaSYS_SetWakeUpAlarm(UINT32 Countdown)
{
    AmbaRTSL_PwcSetAlarmClk(Countdown);

    return SYS_ERR_NONE;
}

/**
 *  AmbaSYS_DismissWakeUpAlarm - The function is used to dismiss the wakeup alarm.
 *  @return error code
 */
UINT32 AmbaSYS_DismissWakeUpAlarm(void)
{
    AmbaRTSL_PwcDismissAlarmClk();

    return SYS_ERR_NONE;
}

/**
 *  AmbaSYS_ClearWakeUpInfo - The function is used to clear all the information about system wakeup.
 *  @return error code
 */
UINT32 AmbaSYS_ClearWakeUpInfo(void)
{
    AmbaRTSL_PwcClrAlarmClkState();

    return SYS_ERR_NONE;
}

/**
 *  AmbaSYS_GetBootMode - The function returns the system wakeup information.
 *  @param[out] pBootMode System wakeup information
 *  @return error code
 */
UINT32 AmbaSYS_GetBootMode(UINT32 * pBootMode)
{
    UINT32 RetVal = SYS_ERR_NONE;

    if (pBootMode == NULL) {
        RetVal = SYS_ERR_ARG;
    } else {
        *pBootMode = AmbaRTSL_PwcGetBootDevice();
    }

    return RetVal;
}

/**
 *  AmbaSYS_EnableFeature - The function enables the specified feature support by enabling its clock source.
 *  @param[in] SysFeature System feature
 *  @return error code
 */
UINT32 AmbaSYS_EnableFeature(UINT32 SysFeature)
{
    UINT32 RetVal = SYS_ERR_NONE;

    if (AmbaKAL_MutexTake(&AmbaSysMutex, KAL_WAIT_FOREVER) != KAL_ERR_NONE) {
        RetVal = SYS_ERR_MUTEX;
    } else {
        AmbaRTSL_PllClkEnable(SysFeature);

        if (AmbaKAL_MutexGive(&AmbaSysMutex) != KAL_ERR_NONE) {
            RetVal = SYS_ERR_UNEXPECTED;
        }
    }

    return RetVal;
}

/**
 *  AmbaSYS_DisableFeature - The function disables the specified feature support by disabling its clock source.
 *  @param[in] SysFeature System feature
 *  @return error code
 */
UINT32 AmbaSYS_DisableFeature(UINT32 SysFeature)
{
    UINT32 RetVal = SYS_ERR_NONE;

    if (AmbaKAL_MutexTake(&AmbaSysMutex, KAL_WAIT_FOREVER) != KAL_ERR_NONE) {
        RetVal = SYS_ERR_MUTEX;
    } else {
        AmbaRTSL_PllClkDisable(SysFeature);

        if (AmbaKAL_MutexGive(&AmbaSysMutex) != KAL_ERR_NONE) {
            RetVal = SYS_ERR_UNEXPECTED;
        }
    }

    return RetVal;
}

/**
 *  AmbaSYS_CheckFeature - The function checks the specified feature support by the clock source enabled or not.
 *  @param[in] SysFeature System feature
 *  @param[out] pEnable - Enable Flag, 0 = not enabled, 1 = enabled
 *  @return error code
 */
UINT32 AmbaSYS_CheckFeature(UINT32 SysFeature, UINT32 *pEnable)
{
    UINT32 RetVal = SYS_ERR_NONE;

    if (pEnable != NULL) {
        AmbaRTSL_PllClkCheck(SysFeature, pEnable);
    } else {
        RetVal = SYS_ERR_ARG;
    }

    return RetVal;
}

/**
 *  AmbaSYS_GetOrcTimer - The function returns the system wakeup information.
 *  @param[out] pBootMode System wakeup information
 *  @return error code
 */
UINT32 AmbaSYS_GetOrcTimer(UINT32 *pOrcTimer)
{
    UINT32 RetVal = SYS_ERR_NONE;

    if (pOrcTimer != NULL) {
        *pOrcTimer = AmbaRTSL_GetOrcTimer();
    } else {
        RetVal = SYS_ERR_ARG;
    }
    return RetVal;
}

/**
 *  AmbaSYS_JtagOn - enable JTAG during secure boot.
 *  @return error code
 */
UINT32 AmbaSYS_JtagOn(void)
{
    AmbaCSL_ScratchpadJtagOn();

    return 0U;
}

UINT32 AmbaSYS_GetWakeUpInfo(UINT32 * pWakeUpInfo)
{
    WakeUpReason |= (AmbaRTSL_WdtGetResetStatus() << 0x7U);
    *pWakeUpInfo = WakeUpReason;

    return 0U;
}