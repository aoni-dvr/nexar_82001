/**
 *  @file AmbaSYS.h
 *
 *  Copyright (c) 2020 Ambarella International LP
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
 *  @details Constants and Definitions for System APIs
 *
 */

#ifndef AMBA_SYS_H
#define AMBA_SYS_H

#ifndef AMBA_SYS_DEF_H
#include "AmbaSYS_Def.h"
#endif

extern AMBA_SYS_USER_CALLBACKS_s  AmbaSysUserCallbacks;

/*
 * Defined in AmbaSYS.c
 */
UINT32 AmbaSYS_SetExtInputClkInfo(UINT32 ExtClkID, UINT32 Freq);
UINT32 AmbaSYS_SetClkFreq(UINT32 ClkID, UINT32 DesiredFreq, UINT32 * pActualFreq);
UINT32 AmbaSYS_GetClkFreq(UINT32 ClkID, UINT32 * pFreq);
UINT32 AmbaSYS_SetIoClkFreq(UINT32 ClkID, UINT32 DesiredFreq, UINT32 * pActualFreq);
UINT32 AmbaSYS_GetIoClkFreq(UINT32 ClkID, UINT32 * pFreq);
UINT32 AmbaSYS_CheckClkRange(UINT32 ClkId, UINT32 DesiredFreq, UINT32 *pValid);
UINT32 AmbaSYS_SetMaxIdleCycleTime(UINT32 MaxIdleCycleTime);
UINT32 AmbaSYS_GetMaxIdleCycleTime(UINT32 *pMaxIdleCycleTime);
UINT32 AmbaSYS_Reboot(void);
UINT32 AmbaSYS_ChangePowerMode(UINT32 PowerMode);
UINT32 AmbaSYS_ConfigPowerSequence(UINT32 PwrSeq0, UINT32 PwrSeq1, UINT32 PwrSeq2, UINT32 PwrSeq3);
UINT32 AmbaSYS_TrigPowerDownSequence(UINT32 Option);
UINT32 AmbaSYS_SetWakeUpAlarm(UINT32 Countdown);
UINT32 AmbaSYS_DismissWakeUpAlarm(void);
UINT32 AmbaSYS_ClearWakeUpInfo(void);
UINT32 AmbaSYS_GetWakeUpInfo(UINT32 * pWakeUpInfo);
UINT32 AmbaSYS_GetBootMode(UINT32 * pBootMode);
UINT32 AmbaSYS_GetRtosCoreId(UINT32 *pCoreId);
UINT32 AmbaSYS_EnableFeature(UINT32 SysFeature);
UINT32 AmbaSYS_DisableFeature(UINT32 SysFeature);
UINT32 AmbaSYS_CheckFeature(UINT32 SysFeature, UINT32 *pEnable);
UINT32 AmbaSYS_GetOrcTimer(UINT32 *pOrcTimer);
UINT32 AmbaSYS_JtagOn(void);

/*
 * Defined in AmbaSYS_DispInfo.c
 */
UINT32 AmbaSYS_DispInfo(UINT32 ModuleID, UINT32 FuncID, AMBA_SYS_LOG_f LogFunc);

#endif  /* AMBA_SYS_H */
