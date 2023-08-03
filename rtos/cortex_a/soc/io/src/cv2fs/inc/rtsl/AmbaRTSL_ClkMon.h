/**
 *  @file AmbaRTSL_ClkMon.h
 *
 *  @copyright Copyright (c) 2020 Ambarella, Inc.
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
 *  @details Definitions & Constants for CLOCK MONITOR RTSL APIs
 *
 */

#ifndef AMBA_RTSL_CLKMON_H
#define AMBA_RTSL_CLKMON_H

#ifndef AMBA_SYS_DEF_H
#include "AmbaSYS_Def.h"
#endif

#ifndef AMBA_RTSL_CLKMON_H
#include "AmbaRTSL_ClkMon_Def.h"
#endif

#ifndef AMBA_RTSL_PLL_DEF_H
#include "AmbaRTSL_PLL_Def.h"
#endif

/*
 * Defined in AmbaRTSL_ClkMon.c
 */
UINT32 AmbaRTSL_ClkMonGetCount(UINT32 ClkMonID, UINT32 *pClkCountsVal);
UINT32 AmbaRTSL_ClkMonSetCtrlManual(UINT32 ClkMonID, UINT8 Enable, UINT16 UpperBound, UINT16 LowerBound);
UINT32 AmbaRTSL_ClkMonGetCtrl(UINT32 ClkMonID, UINT8 *Enable, UINT16 *UpperBound, UINT16 *LowerBound);

#endif /* AMBA_RTSL_CLKMON_H */
