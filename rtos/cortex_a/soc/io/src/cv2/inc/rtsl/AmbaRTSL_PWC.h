/**
 *  @file AmbaRTSL_PWC.h
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
 *  @details Definitions & Constants for PWC RTSL APIs
 *
 */

#ifndef AMBA_RTSL_PWC_H
#define AMBA_RTSL_PWC_H

#ifndef AMBA_TYPES_H
#include "AmbaTypes.h"
#endif

/*
 * Defined in AmbaRTSL_PWC.c
 */
void AmbaRTSL_PwcInit(void);
void AmbaRTSL_PwcReboot(void);

void AmbaRTSL_PwcConfigPowerSeq(UINT32 Delay0, UINT32 Delay1, UINT32 Delay2, UINT32 Delay3);
void AmbaRTSL_PwcTrigPowerDownSeq(void);

void AmbaRTSL_PwcForcePseq3State(UINT32 State);
void AmbaRTSL_PwcSetPowerLossState(void);
void AmbaRTSL_PwcClearPowerLossState(void);
void AmbaRTSL_PwcClrAlarmClkState(void);
void AmbaRTSL_PwcWriteScratchpad(UINT32 Status);

void AmbaRTSL_PwcSetAlarmClk(UINT32 TimeDelay);
void AmbaRTSL_PwcDismissAlarmClk(void);

UINT32 AmbaRTSL_PwcGetWakeUpReason(void);
UINT32 AmbaRTSL_PwcGetBootDevice(void);
UINT32 AmbaRTSL_PwcCheckSwReset(void);

#endif /* AMBA_RTSL_PWC_H */
