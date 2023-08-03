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
#include "AmbaMisraFix.h"
#include "AmbaKAL.h"

#include "AmbaMisraFix.h"

#include "AmbaDrvEntry.h"
#include "AmbaSYS.h"
#include "AmbaSYS_Ctrl.h"
#include "AmbaSpiNOR_Ctrl.h"

#include "AmbaRTSL_GIC.h"
#include "AmbaRTSL_SD.h"

#include "AmbaRTSL_PLL.h"
#include "AmbaRTSL_PWC.h"
#include "AmbaRTSL_ORC.h"
#include "AmbaCSL_Scratchpad.h"

#ifdef CONFIG_BOOT_CORE_SRTOS
__attribute__((section(".data"))) UINT32 AmbaRtosCoreId = (UINT32)CONFIG_BOOT_CORE_SRTOS;
#else
__attribute__((section(".data"))) UINT32 AmbaRtosCoreId = (UINT32)0;
#endif

static UINT32 AmbaSysMaxIdleCycleTime = 500U;   /* Half-second by default */

static AMBA_KAL_MUTEX_t AmbaSysMutex;

/**
 *  AmbaSYS_DrvEntry - Initializes driver-wide data structures and resources
 *  @return error code
 *  @note this function is an internal using only API
 */
UINT32 AmbaSYS_DrvEntry(void)
{
    static char AmbaSysMutexName[16] = "AmbaSysMutex";
    UINT32 RetVal = SYS_ERR_NONE;

    if (AmbaKAL_MutexCreate(&AmbaSysMutex, AmbaSysMutexName) != KAL_ERR_NONE) {
        RetVal = SYS_ERR_UNEXPECTED;
    }

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

    if (AmbaKAL_MutexTake(&AmbaSysMutex, KAL_WAIT_FOREVER) != KAL_ERR_NONE) {
        RetVal = SYS_ERR_MUTEX;
    } else {
        (void)AmbaRTSL_PllGetExtClkConfig(&ExtClkConfig);

        if (ExtClkID == AMBA_SYS_EXT_CLK_AUDIO) {
            ExtClkConfig.ExtAudioClkFreq = Freq;
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
        } else {
            RetVal = SYS_ERR_ARG;
        }

        (void)AmbaRTSL_PllSetExtClkConfig(&ExtClkConfig);

        if (AmbaKAL_MutexGive(&AmbaSysMutex) != KAL_ERR_NONE) {
            /* If code reaches here, the system should be corrupted. Overwite the previous return value because it is meaningless. */
            RetVal = SYS_ERR_UNEXPECTED;
        }
    }

    return RetVal;
}

static UINT32 GetClkId(UINT32 ClkID)
{
    UINT32 RetVal;

    if (ClkID == AMBA_SYS_CLK_CORE) {
        RetVal = (UINT32) AMBA_CLK_CORE;
    } else if (ClkID == AMBA_SYS_CLK_IDSP) {
        RetVal = (UINT32) AMBA_CLK_IDSP;
    } else if (ClkID == AMBA_SYS_CLK_IDSPV) {
        RetVal = (UINT32) AMBA_CLK_IDSPV;
    } else if (ClkID == AMBA_SYS_CLK_CORTEX) {
        RetVal = (UINT32) AMBA_CLK_CORTEX;
    } else if (ClkID == AMBA_SYS_CLK_DRAM) {
        RetVal = (UINT32) AMBA_CLK_DRAM;
    } else if (ClkID == AMBA_SYS_CLK_DSU) {
        RetVal = (UINT32) AMBA_CLK_DRAM;
    } else if (ClkID == AMBA_SYS_CLK_DSP_SYS) {
        RetVal = (UINT32) AMBA_CLK_AUDIO;
    } else if (ClkID == AMBA_SYS_CLK_VISION) {
        RetVal = (UINT32) AMBA_CLK_VISION;
    } else if (ClkID == AMBA_SYS_CLK_AUD_0) {
        RetVal = (UINT32) AMBA_CLK_AUDIO;
    } else if (ClkID == AMBA_SYS_CLK_VID_IN0) {
        RetVal = (UINT32) AMBA_CLK_SENSOR0;
    } else if (ClkID == AMBA_SYS_CLK_VID_IN1) {
        RetVal = (UINT32) AMBA_CLK_SENSOR1;
    } else if (ClkID == AMBA_SYS_CLK_VID_OUT0) {
        RetVal = (UINT32) AMBA_CLK_VOUTLCD;
    } else if (ClkID == AMBA_SYS_CLK_VID_OUT1) {
        RetVal = (UINT32) AMBA_CLK_VOUTTV;
    } else if (ClkID == AMBA_SYS_CLK_VID_OUT2) {
        RetVal = (UINT32) AMBA_CLK_VOUTLCD;
    } else if (ClkID == AMBA_SYS_CLK_REF_OUT0) {
        RetVal = (UINT32) AMBA_CLK_SENSOR0;
    } else if (ClkID == AMBA_SYS_CLK_REF_OUT1) {
        RetVal = (UINT32) AMBA_CLK_SENSOR1;
    } else {
        RetVal = (UINT32) AMBA_CLK_NUM;
    }

    return RetVal;
}

static UINT32 SetCLkFreq(UINT32 ClkID, UINT32 DesiredFreq, UINT32 * pActualFreq)
{
    UINT32 RetVal = SYS_ERR_NONE;

    if (ClkID == AMBA_SYS_CLK_CORE) {
        (void)AmbaRTSL_PllSetCoreClk(DesiredFreq);
        *pActualFreq = AmbaRTSL_PllGetCoreClk();
    } else if (ClkID == AMBA_SYS_CLK_DRAM) {
        AmbaRTSL_PllSetDdrcClk(DesiredFreq);
        *pActualFreq = AmbaRTSL_PllGetDramClk();
    } else if (ClkID == AMBA_SYS_CLK_DSU) {
        (void)AmbaRTSL_PllSetDsuClk(DesiredFreq);
        *pActualFreq = AmbaRTSL_PllGetDsuClk();
    } else if (ClkID == AMBA_SYS_CLK_IDSP) {
        (void)AmbaRTSL_PllSetIdspClk(DesiredFreq);
        *pActualFreq = AmbaRTSL_PllGetIdspClk();
    } else if (ClkID == AMBA_SYS_CLK_IDSPV) {
        (void)AmbaRTSL_PllSetIdspvClk(DesiredFreq);
        *pActualFreq = AmbaRTSL_PllGetIdspvClk();
    } else if (ClkID == AMBA_SYS_CLK_VISION) {
        (void)AmbaRTSL_PllSetVisionClk(DesiredFreq);
        *pActualFreq = AmbaRTSL_PllGetVisionClk();
    } else if (ClkID == AMBA_SYS_CLK_AUD_0) {
        (void)AmbaRTSL_PllFineAdjAudioClk(DesiredFreq);
        *pActualFreq = AmbaRTSL_PllGetAudioClk();
    } else if (ClkID == AMBA_SYS_CLK_VID_IN0) {
        (void)AmbaRTSL_PllFineAdjSensor1Clk(DesiredFreq);
        *pActualFreq = AmbaRTSL_PllGetSensor1Clk();
    } else if (ClkID == AMBA_SYS_CLK_VID_IN1) {
        (void)AmbaRTSL_PllFineAdjSensor0Clk(DesiredFreq);
        *pActualFreq = AmbaRTSL_PllGetSensor0Clk();
    } else if (ClkID == AMBA_SYS_CLK_VID_OUT0) {
        (void)AmbaRTSL_PllFineAdjVoutAClk(DesiredFreq);
        *pActualFreq = AmbaRTSL_PllGetVoutAClk();
    } else if (ClkID == AMBA_SYS_CLK_VID_OUT1) {
        (void)AmbaRTSL_PllFineAdjVoutBClk(DesiredFreq);
        *pActualFreq = AmbaRTSL_PllGetVoutBClk();
    } else if (ClkID == AMBA_SYS_CLK_VID_OUT2) {
        (void)AmbaRTSL_PllFineAdjVoutCClk(DesiredFreq);
        *pActualFreq = AmbaRTSL_PllGetVoutCClk();
    } else if (ClkID == AMBA_SYS_CLK_REF_OUT0) {
        (void)AmbaRTSL_PllFineAdjSensor0Clk(DesiredFreq);
        *pActualFreq = AmbaRTSL_PllGetSensor0Clk();
    } else if (ClkID == AMBA_SYS_CLK_REF_OUT1) {
        (void)AmbaRTSL_PllFineAdjSensor1Clk(DesiredFreq);
        *pActualFreq = AmbaRTSL_PllGetSensor1Clk();
    } else {
        RetVal = SYS_ERR_ARG;
    }

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
    UINT32 ClockId = (UINT32) AMBA_CLK_NUM;

    if (AmbaKAL_MutexTake(&AmbaSysMutex, KAL_WAIT_FOREVER) != KAL_ERR_NONE) {
        RetVal = SYS_ERR_MUTEX;
    } else {
        /* make sure target clock is not over maximum clock value */
        ClockId = GetClkId(ClkID);
        RetVal = AmbaRTSL_PllGetMax(ClockId, &ActualFreq);
        if (RetVal == SYS_ERR_NONE) {
            if (DesiredFreq > ActualFreq) {
                RetVal = SYS_ERR_INVALIDCLK;
            }
        }

        if (RetVal == SYS_ERR_NONE) {
            RetVal = SetCLkFreq(ClkID, DesiredFreq, &ActualFreq);
            if ((pActualFreq != NULL) && (RetVal == SYS_ERR_NONE)) {
                *pActualFreq = ActualFreq;
            }
        }

        if (AmbaKAL_MutexGive(&AmbaSysMutex) != KAL_ERR_NONE) {
            /* If code reaches here, the system should be corrupted. Overwite the previous return value because it is meaningless. */
            RetVal = SYS_ERR_UNEXPECTED;
        }
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
            if (ClkID == AMBA_SYS_CLK_CORE) {
                *pFreq = AmbaRTSL_PllGetCoreClk();
            } else if (ClkID == AMBA_SYS_CLK_CORTEX) {
                *pFreq = AmbaRTSL_PllGetCortexClk();
            } else if (ClkID == AMBA_SYS_CLK_DRAM) {
                *pFreq = AmbaRTSL_PllGetDramClk();
            } else if (ClkID == AMBA_SYS_CLK_DSU) {
                *pFreq = AmbaRTSL_PllGetDsuClk();
            } else if (ClkID == AMBA_SYS_CLK_IDSP) {
                *pFreq = AmbaRTSL_PllGetIdspClk();
            } else if (ClkID == AMBA_SYS_CLK_IDSPV) {
                *pFreq = AmbaRTSL_PllGetIdspvClk();
            } else if (ClkID == AMBA_SYS_CLK_DSP_SYS) {
                *pFreq = AmbaRTSL_PllGetAudioClk();
            } else if (ClkID == AMBA_SYS_CLK_AUD_0) {
                *pFreq = AmbaRTSL_PllGetAudioClk();
            } else if (ClkID == AMBA_SYS_CLK_VID_IN0) {
                /* Return sensor clock in place of vin clock here.
                 * The clock frequency would be (Vin0 clock) / 4 if
                 * AMBA_PLL_FOURFOLD_GCLK_SO_VIN is defined. */
                *pFreq = AmbaRTSL_PllGetSensor1Clk();
            } else if (ClkID == AMBA_SYS_CLK_VID_IN1) {
                /* Return sensor clock in place of vin clock here.
                 * The clock frequency would be (Vin4 clock) / 4 if
                 * AMBA_PLL_FOURFOLD_GCLK_SO_VIN is defined. */
                *pFreq = AmbaRTSL_PllGetSensor0Clk();
            } else if (ClkID == AMBA_SYS_CLK_VID_OUT0) {
                *pFreq = AmbaRTSL_PllGetVoutAClk();
            } else if (ClkID == AMBA_SYS_CLK_VID_OUT1) {
                *pFreq = AmbaRTSL_PllGetVoutBClk();
            } else if (ClkID == AMBA_SYS_CLK_VID_OUT2) {
                *pFreq = AmbaRTSL_PllGetVoutCClk();
            } else if (ClkID == AMBA_SYS_CLK_REF_OUT0) {
                *pFreq = AmbaRTSL_PllGetSensor0Clk();
            } else if (ClkID == AMBA_SYS_CLK_REF_OUT1) {
                *pFreq = AmbaRTSL_PllGetSensor1Clk();
            } else if (ClkID == AMBA_SYS_CLK_VISION) {
                *pFreq = AmbaRTSL_PllGetVisionClk();
            } else {
                RetVal = SYS_ERR_ARG;
            }

            if (AmbaKAL_MutexGive(&AmbaSysMutex) != KAL_ERR_NONE) {
                /* If code reaches here, the system should be corrupted. Overwite the previous return value because it is meaningless. */
                RetVal = SYS_ERR_UNEXPECTED;
            }
        }
    }

    return RetVal;
}

UINT32 AmbaSYS_SetIoClkFreq(UINT32 ClkID, UINT32 DesiredFreq, UINT32 * pActualFreq)
{
    AmbaMisra_TouchUnused(&ClkID);
    AmbaMisra_TouchUnused(&DesiredFreq);
    AmbaMisra_TouchUnused(&pActualFreq);

    return SYS_ERR_NONE;
}

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
        } else if (BootMode == AMBA_SYS_BOOT_FROM_EMMC) {
            (void) AmbaRTSL_PllSetSd0Clk(24000000U);
            AmbaRTSL_SdClockEnable(0U, 1U);

            /* Disable the interrupt for avoiding the other emmc command operation */
            AmbaRTSL_GicIntGlobalDisable();

            if (AmbaRTSL_SdSendCMD0(0U, 0xF0F0F0F0U) != 0) {
                RetVal = SYS_ERR_UNEXPECTED;
            }
        } else {
            /* Nothing special */
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
    AmbaMisra_TouchUnused(&PwrSeq0);
    AmbaMisra_TouchUnused(&PwrSeq1);
    AmbaMisra_TouchUnused(&PwrSeq2);
    AmbaMisra_TouchUnused(&PwrSeq3);

    return SYS_ERR_SPRT;
}

/**
 *  AmbaSYS_TrigPowerDownSequence - The function is used to trigger power-down sequence.
 *  @param[in] Option Force PSEQ3 pin state or not.
 *  @return error code
 */
UINT32 AmbaSYS_TrigPowerDownSequence(UINT32 Option)
{
    AmbaMisra_TouchUnused(&Option);

    return SYS_ERR_SPRT;
}

/**
 *  AmbaSYS_SetWakeUpAlarm - The function is used to wake up system with RTC service.
 *  @param[in] Countdown Number of seconds before triggering the power-up sequence.
 *  @return error code
 */
UINT32 AmbaSYS_SetWakeUpAlarm(UINT32 Countdown)
{
    AmbaMisra_TouchUnused(&Countdown);

    return SYS_ERR_SPRT;
}

/**
 *  AmbaSYS_DismissWakeUpAlarm - The function is used to dismiss the wakeup alarm.
 *  @return error code
 */
UINT32 AmbaSYS_DismissWakeUpAlarm(void)
{
    return SYS_ERR_SPRT;
}

/**
 *  AmbaSYS_ClearWakeUpInfo - The function is used to clear all the information about system wakeup.
 *  @return error code
 */
UINT32 AmbaSYS_ClearWakeUpInfo(void)
{
    return SYS_ERR_SPRT;
}

/**
 *  AmbaSYS_GetWakeUpInfo - The function returns the system wakeup information.
 *  @param[out] pWakeUpInfo System wakeup information
 *  @return error code
 */
UINT32 AmbaSYS_GetWakeUpInfo(UINT32 * pWakeUpInfo)
{
    AmbaMisra_TouchUnused(pWakeUpInfo);

    return SYS_ERR_SPRT;
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
 *  AmbaSYS_CheckClkRange - The function checks the specified clock value is valid or not.
 *  @param[in] ClkId clock ID
 *  @param[in] DesiredFreq Target clock value
 *  @param[out] pValid - check result, when RetVal = SYS_ERR_NONE,  0 = not valid(over maximum value), 1 = valid
 *  @return error code
 */
UINT32 AmbaSYS_CheckClkRange(UINT32 ClkId, UINT32 DesiredFreq, UINT32 *pValid)
{
    UINT32 RetVal = SYS_ERR_NONE;
    UINT32 ClkMax = 0U;

    if ((ClkId < (UINT32) AMBA_CLK_NUM) && (pValid != NULL)) {
        if (AmbaRTSL_PllGetMax(ClkId, &ClkMax) == ERR_NONE) {
            if (ClkMax != 0x0U) {
                *pValid = (ClkMax > DesiredFreq) ? 1U : 0U;
            } else {
                *pValid = 1U;
            }
        }
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
