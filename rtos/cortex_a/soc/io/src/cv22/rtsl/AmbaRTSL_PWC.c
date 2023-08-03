/**
 *  @file AmbaRTSL_PWC.c
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
 *  @details PWC RTSL APIs
 *
 */

#include "AmbaTypes.h"
#include "AmbaDef.h"
#include "AmbaSYS.h"

#include "AmbaRTSL_PLL.h"
#include "AmbaRTSL_PWC.h"
#include "AmbaCSL_PWC.h"
#include "AmbaCSL_RCT.h"
#include "AmbaCSL_RTC.h"
#include "AmbaCSL_USB.h"
#include "AmbaCSL_WDT.h"

/**
 *  AmbaRTSL_PwcInit - Initialize PWC controller
 */
void AmbaRTSL_PwcInit(void)
{
    /* Reset Power Down bit to 0 */
    AmbaCSL_PwcPowerDownCtrl(0U);

    /* Reset to hardware default delay values */
    AmbaRTSL_PwcConfigPowerSeq(32U, 32U, 32U, 32U);

    /* Reset wakeup by alarm enable */
    AmbaRTSL_PwcDismissAlarmClk();
}

/**
 *  AmbaRTSL_PwcReboot - Reboot the system
 */
void AmbaRTSL_PwcReboot(void)
{
    AmbaCSL_WdtClearWdtResetStatus();   /* Clear WDT reset status otherwise it will be kept. */
    AmbaCSL_RctSetUsbHostSoftReset(1U); /* If USB host is turned on, need to reset it to make chip soft reset work */
    AmbaCSL_RctChipSoftReset();
}

/**
 *  AmbaRTSL_PwcConfigPowerSeq - Set delays for the PWC (Power Control Circuits)
 *  @param[in] Delay0 A delay between PWC_WKUP  and PWC_PSEQ1 Signals (in ms)
 *  @param[in] Delay1 A delay between PWC_PSEQ1 and PWC_PSEQ2 Signals (in ms)
 *  @param[in] Delay2 A delay between PWC_PSEQ2 and PWC_PSEQ3 Signals (in ms)
 *  @param[in] Delay3 A delay between PWC_PSEQ3 and PWC_RSTOB Signals (in ms)
 */
void AmbaRTSL_PwcConfigPowerSeq(UINT32 Delay0, UINT32 Delay1, UINT32 Delay2, UINT32 Delay3)
{
    AmbaCSL_PwcSetPseq1Delay(Delay0);
    AmbaCSL_PwcSetPseq2Delay(Delay1);
    AmbaCSL_PwcSetPseq3Delay(Delay2);
    AmbaCSL_PwcSetPseq4Delay(Delay3);
    AmbaCSL_PwcTogglePCRST();
}

/**
 *  AmbaRTSL_PwcTrigPowerDownSeq - Force Power Down Sequence
 */
void AmbaRTSL_PwcTrigPowerDownSeq(void)
{
    /* PSEQ3: High to Low; PSEQ2: High to Low; PSEQ1: High to Low */
    AmbaCSL_PwcPowerDownCtrl(1U);
}

/**
 *  AmbaRTSL_PwcForcePseq3State - Force PSEQ3 to high or low
 *  @param[in] State the state of PSEQ3: 1 - high; 0 - low
 */
void AmbaRTSL_PwcForcePseq3State(UINT32 State)
{
    if (State != 0U) {
        AmbaCSL_PwcSetScratchpad(PWC_SCRATCHPAD_PSEQ3_PIN_CTRL);
    } else {
        AmbaCSL_PwcClearScratchpad(PWC_SCRATCHPAD_PSEQ3_PIN_CTRL);
    }
}

/**
 *  AmbaRTSL_PwcSetPowerLossState - Set the state/flag of Lost Power
 */
void AmbaRTSL_PwcSetPowerLossState(void)
{
    AmbaCSL_PwcSetScratchpad(PWC_SCRATCHPAD_POWER_LOSS);
}

/**
 *  AmbaRTSL_PwcClearPowerLossState - Clear the state/flag of Lost Power
 */
void AmbaRTSL_PwcClearPowerLossState(void)
{
    AmbaCSL_PwcClearScratchpad(PWC_SCRATCHPAD_POWER_LOSS);
}

/**
 *  AmbaRTSL_PwcClrAlarmClkState - Clear the state to indicate the system is waked up by alarm
 */
void AmbaRTSL_PwcClrAlarmClkState(void)
{
    AmbaCSL_PwcClearScratchpad(PWC_SCRATCHPAD_WAKEUP_ALARM_OCCUR);
}

/**
 *  AmbaRTSL_PwcWriteScratchpad - Set User-Defined status (4-bit)
 *  @param[in] Status User-Defined status (4-bit)
 */
void AmbaRTSL_PwcWriteScratchpad(UINT32 Status)
{
    if ((Status & 0x1U) != 0U) {
        AmbaCSL_PwcSetScratchpad(PWC_SCRATCHPAD_RESERVED0);
    } else {
        AmbaCSL_PwcClearScratchpad(PWC_SCRATCHPAD_RESERVED0);
    }

    if ((Status & 0x2U) != 0U) {
        AmbaCSL_PwcSetScratchpad(PWC_SCRATCHPAD_RESERVED1);
    } else {
        AmbaCSL_PwcClearScratchpad(PWC_SCRATCHPAD_RESERVED1);
    }

    if ((Status & 0x4U) != 0U) {
        AmbaCSL_PwcSetScratchpad(PWC_SCRATCHPAD_RESERVED2);
    } else {
        AmbaCSL_PwcClearScratchpad(PWC_SCRATCHPAD_RESERVED2);
    }
}

/**
 *  AmbaRTSL_PwcSetAlarmClk - PWC Wakeup by Alarm
 *  @param[in] TimeDelay The value after that to generate PSEQ[n] (low to high)
 */
void AmbaRTSL_PwcSetAlarmClk(UINT32 TimeDelay)
{
    UINT32 WorkUINT32 = AmbaCSL_RtcGetCurTime();

    /* REG_ALAT<31:0> = REG_CURT<31:0> + TimeDelay */
    AmbaCSL_RtcSetAlarmTime(WorkUINT32 + GetMaxValU32(1U, TimeDelay));
    AmbaCSL_RtcSetCurTime(WorkUINT32);  /* rewrite current time */

    AmbaCSL_PwcSetScratchpad(PWC_SCRATCHPAD_WAKEUP_ALARM_ENA);
}

/**
 *  AmbaRTSL_PwcDismissAlarmClk - Disable Wakeup By Alarm function
 */
void AmbaRTSL_PwcDismissAlarmClk(void)
{
    AmbaCSL_PwcClearScratchpad(PWC_SCRATCHPAD_WAKEUP_ALARM_ENA);
}

/**
 *  AmbaRTSL_PwcGetWakeUpReason - Get Wake-Up reasons.
 *  @return Wake-Up reason
 */
UINT32 AmbaRTSL_PwcGetWakeUpReason(void)
{
    UINT32 WakeUpReason;
    UINT32 RegVal = AmbaCSL_PwcReadScratchpad();

    /* The WakeupByAlarm state will only keep one second.
       Therefore we read it from RTC Status Register as early as possible. (i.e. bootstrap/bootloader)
       And then save it to PWC Status Register as a software backup. */
    /* This function is designed for RTOS so that we get the information from the software backup information. */
    if ((RegVal & PWC_SCRATCHPAD_WAKEUP_ALARM_OCCUR) != 0U) {
        WakeUpReason = AMBA_SYS_WAKEUP_ALARM;
    } else if (AmbaCSL_RtcGetPwcWakeupState() != 0U) {
        WakeUpReason = AMBA_SYS_WAKEUP_PWC_WKUP;
    } else if (AmbaCSL_PwcGetWakeUp1() != 0U) {
        WakeUpReason = AMBA_SYS_WAKEUP_PWC_WKUP1;
    } else if (AmbaCSL_PwcGetWakeUp2() != 0U) {
        WakeUpReason = AMBA_SYS_WAKEUP_PWC_WKUP2;
    } else if (AmbaCSL_PwcGetWakeUp3() != 0U) {
        WakeUpReason = AMBA_SYS_WAKEUP_PWC_WKUP3;
    } else {
        WakeUpReason = AMBA_SYS_WAKEUP_NONE;
    }

    return WakeUpReason;
}

/**
 *  AmbaRTSL_PwcGetBootDevice - Get boot device type
 *  @return Boot device ID
 */
UINT32 AmbaRTSL_PwcGetBootDevice(void)
{
    UINT32 BootDevice;

    if (pAmbaRCT_Reg->SysConfig.UsbBoot != 0U) {
        /* For debug purpose */
#if defined(CONFIG_ENABLE_NAND_BOOT)
        BootDevice = AMBA_SYS_BOOT_FROM_NAND;
#elif defined(CONFIG_ENABLE_SPINAND_BOOT)
        BootDevice = AMBA_SYS_BOOT_FROM_SPI_NAND;
#elif defined(CONFIG_ENABLE_EMMC_BOOT)
        BootDevice = AMBA_SYS_BOOT_FROM_EMMC;
#elif defined(CONFIG_ENABLE_SPINOR_BOOT)
        BootDevice = AMBA_SYS_BOOT_FROM_SPI_NOR;
#else
        BootDevice = AMBA_SYS_BOOT_FROM_USB;
#endif
    } else {
        /* For normal case */
        switch (pAmbaRCT_Reg->SysConfig.BootMode) {
        case 0U:
            BootDevice = AMBA_SYS_BOOT_FROM_SPI_NOR;
            break;

        case 1U:
            if (pAmbaRCT_Reg->SysConfig.BootOption1 != 0U) {
                BootDevice = AMBA_SYS_BOOT_FROM_SPI_NAND;
            } else {
                BootDevice = AMBA_SYS_BOOT_FROM_NAND;
            }
            break;

        case 2U:
            BootDevice = AMBA_SYS_BOOT_FROM_EMMC;
            break;

        case 3U:
        default:
            BootDevice = AMBA_SYS_BOOT_FROM_EEPROM;
            break;
        }
    }

    return BootDevice;
}
