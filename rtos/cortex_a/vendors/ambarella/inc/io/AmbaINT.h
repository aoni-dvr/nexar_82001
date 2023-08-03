/**
 *  @file AmbaINT.h
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
 *  @details Definitions & Constants for Interrupt Controller APIs
 *
 */

#ifndef AMBA_INT_H
#define AMBA_INT_H

#ifndef AMBA_INT_DEF_H
#include "AmbaINT_Def.h"
#endif

/*
 * Defined in AmbaINT.c
 */
UINT32 AmbaINT_GlobalMaskEnable(void);
UINT32 AmbaINT_GlobalMaskDisable(void);
UINT32 AmbaINT_Config(UINT32 Irq, const AMBA_INT_CONFIG_s *pIntConfig, AMBA_INT_ISR_f IntFunc, UINT32 IntFuncArg);
UINT32 AmbaINT_Enable(UINT32 Irq);
UINT32 AmbaINT_Disable(UINT32 Irq);
UINT32 AmbaINT_SendSGI(UINT32 Irq, UINT32 SgiType, UINT32 CpuTargets);
UINT32 AmbaINT_GetInfo(UINT32 Irq, UINT32 * pGlobalIntEnable, AMBA_INT_INFO_s * pIrqInfo);

#endif  /* AMBA_INT_H */
