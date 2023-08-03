/*
 * Copyright (c) 2020 Ambarella International LP
 *
 * This file and its contents ("Software") are protected by intellectual
 * property rights including, without limitation, U.S. and/or foreign
 * copyrights. This Software is also the confidential and proprietary
 * information of Ambarella International LP and its licensors. You may not use, reproduce,
 * disclose, distribute, modify, or otherwise prepare derivative works of this
 * Software or any portion thereof except pursuant to a signed license agreement
 * or nondisclosure agreement with Ambarella International LP or its authorized affiliates.
 * In the absence of such an agreement, you agree to promptly notify and return
 * this Software to Ambarella International LP.
 *
 * THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF NON-INFRINGEMENT,
 * MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL AMBARELLA INTERNATIONAL LP OR ITS AFFILIATES BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; COMPUTER FAILURE OR MALFUNCTION; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef AMBA_CSL_TMR_H
#define AMBA_CSL_TMR_H

#ifndef AMBA_TMR_DEF_H
#include "AmbaTMR_Def.h"
#endif

#include "AmbaReg_TMR.h"

/*
 * Defined in AmbaCSL_TMR.c
 */
void AmbaCSL_TmrSetConfig(UINT32 TimerID, UINT32 ClkSel, UINT32 IntCtrl);
void AmbaCSL_TmrSetEnable(UINT32 TimerID, UINT32 Enable);
void AmbaCSL_TmrSetCurrentVal(UINT32 TimerID, UINT32 Val);
void AmbaCSL_TmrSetReloadVal(UINT32 TimerID, UINT32 Val);
void AmbaCSL_TmrSetMatch0Val(UINT32 TimerID, UINT32 Val);
void AmbaCSL_TmrSetMatch1Val(UINT32 TimerID, UINT32 Val);
UINT32 AmbaCSL_TmrGetEnable(UINT32 TimerID);
UINT32 AmbaCSL_TmrGetCurrentVal(UINT32 TimerID);
UINT32 AmbaCSL_TmrGetReloadVal(UINT32 TimerID);

#endif /* AMBA_CSL_TMR_H */