/**
 *  @file AmbaRTSL_GIC.h
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
 *  @details Definitions & Constants for CoreLink GIC-400 RTSL APIs
 *
 */

#ifndef AMBA_RTSL_GIC_H
#define AMBA_RTSL_GIC_H

#ifndef AMBA_INT_DEF_H
#include "AmbaINT_Def.h"
#endif

/*
 * Defined in AmbaRTSL_GIC.c
 */
void AmbaRTSL_GicInit(void);
void AmbaRTSL_GicInitCorePrivate(void);
void AmbaRTSL_GicIntGlobalEnable(void);
void AmbaRTSL_GicIntGlobalDisable(void);
UINT32 AmbaRTSL_GicIntGetGlobalState(void);

UINT32 AmbaRTSL_GicIntConfig(UINT32 IntID, const AMBA_INT_CONFIG_s *pIntConfig, AMBA_INT_ISR_f IntFunc, UINT32 IntFuncArg);

void AmbaRTSL_GicSetCpuTarget(UINT32 IntID, UINT32 CpuTargetList);
UINT32 AmbaRTSL_GicIntEnable(UINT32 IntID);
UINT32 AmbaRTSL_GicIntDisable(UINT32 IntID);
UINT32 AmbaRTSL_GicSendSGI(UINT32 IntID, UINT32 IrqType, UINT32 SgiType, UINT32 CpuTargetList);

void AmbaRTSL_GicIsrIRQ(void);
void AmbaRTSL_GicIsrFIQ(void);
UINT32 AmbaRTSL_GicGetIntInfo(UINT32 IntID, AMBA_INT_INFO_s *pIntInfo);
UINT32 AmbaRTSL_GicGetIntConfig(UINT32 IntID);

AMBA_INT_ISR_f AmbaRTSL_GicGetISR(UINT32 IntID);
UINT32 AmbaRTSL_GicSetISR(UINT32 IntID, AMBA_INT_ISR_f IsrEntry);

void AmbaINT_SetProfileFunc(AMBA_INT_ISR_PROFILE_f EntryCb, AMBA_INT_ISR_PROFILE_f ExitCb);

#endif /* AMBA_RTSL_GIC_H */
