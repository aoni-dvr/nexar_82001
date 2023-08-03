/**
 *  @file AmbaCSL_CLKMON.h
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
 *  @details Definitions & Constants for Clock Monitor CSL APIs
 *
 */

#ifndef AMBA_CSL_CLKMON_H
#define AMBA_CSL_CLKMON_H

#ifndef AMBA_RTSL_PLL_DEF_H
#include "AmbaRTSL_PLL_Def.h"
#endif

#ifndef AMBA_REG_RCT_H
#include "AmbaReg_RCT.h"
#endif

/*
 * Inline Function Definitions
 */

typedef struct {
    UINT16  LowerBound;     /* lower bound of compare value */
    UINT16  UpperBound;     /* upper bound of compare value */
    UINT8   ClkMonEnable;   /* clock monitor enable */
} AMBA_CLK_MON_CTRL_REGS_VAL_s;

/*
 * Defined in AmbaCSL_ClkMon.c
 */
void AmbaCSL_ClkMonSetCtrl(UINT32 ClkMonID, AMBA_CLK_MON_CTRL_REGS_VAL_s *pNewRegsVal);
void AmbaCSL_ClkMonGetCtrl(UINT32 ClkMonID, AMBA_CLK_MON_CTRL_REGS_VAL_s *pNewRegsVal);
UINT32 AmbaCSL_ClkMonGetCount(UINT32 ClkMonID);

#endif /* AMBA_CSL_CLKMON_H */
