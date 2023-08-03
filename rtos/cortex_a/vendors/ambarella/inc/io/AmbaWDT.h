/**
 *  @file AmbaWDT.h
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
 *  @details Definitions & Constants for WatchDog Timer Middleware APIs
 *
 */

#ifndef AMBA_WDT_H
#define AMBA_WDT_H

#ifndef AMBA_WDT_DEF_H
#include "AmbaWDT_Def.h"
#endif

/*
 * Defined in AmbaWDT.c
 */
UINT32 AmbaWDT_HookTimeOutHandler(AMBA_WDT_ISR_f ExpireFunc, UINT32 ExpireFuncArg);
UINT32 AmbaWDT_Start(UINT32 CountDown, UINT32 ResetIrqPulseWidth);
UINT32 AmbaWDT_Stop(void);
UINT32 AmbaWDT_Feed(void);
UINT32 AmbaWDT_GetInfo(AMBA_WDT_INFO_s * pWdtInfo);
UINT32 AmbaWDT_ClearStatus(void);

#if defined(CONFIG_SOC_CV2FS) || defined(CONFIG_SOC_CV22FS)
UINT32 AmbaWDT_HookHandler(UINT32 ChId, AMBA_WDT_ISR_f ExpireFunc, UINT32 ExpireFuncArg);
UINT32 AmbaWDT_StartByChId(UINT32 ChId, UINT32 CountDown, UINT32 ResetIrqPulseWidth);
UINT32 AmbaWDT_FeedByChId(UINT32 ChId);
UINT32 AmbaWDT_StopByChId(UINT32 ChId);
UINT32 AmbaWDT_GetInfoByChId(UINT32 ChId, AMBA_WDT_INFO_s * pWdtInfo);
UINT32 AmbaWDT_SetPattern(void);
#endif

#endif /* AMBA_WDT_H */
