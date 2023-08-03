/**
 *  @file AmbaIntrinsics.h
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
 *  @details Definitions & Constants of Intrinsic Functions
 *
 */

#ifndef AMBA_INTRINSICS_H
#define AMBA_INTRINSICS_H

#ifndef AMBA_TYPES_H
#include "AmbaTypes.h"
#endif
#ifndef AMBA_CACHE_DEF_H
#include "AmbaCache_Def.h"
#endif

#ifdef CONFIG_ARM32
#include "AmbaIntrinsics_A32.h"
#endif

#ifdef CONFIG_ARM64
#include "AmbaIntrinsics_A64.h"
#endif

#ifndef __ASSEMBLY__

void AMBA_DMB(void);    /* Data Memory Barrier */
void AMBA_DSB(void);    /* Data Synchronization Barrier */
void AMBA_ISB(void);    /* Instruction Synchronization Barrier */
void AMBA_SEVL(void);   /* Set Event Locally */
void AMBA_SEV(void);    /* Set Event */
void AMBA_WFE(void);    /* Wait For Event */
void AMBA_WFI(void);    /* Wait for Interrupt */

#endif  /* #ifndef __ASSEMBLY__ */

/*
 *  Defined in AmbaBoot.asm
 */
UINT32 AMBA_get_CPSR(void);        /* This function returns the content of the Current Program Status Register (CPSR). */
void AMBA_set_CPSR(UINT32 PsrVal); /* This function assigns the given value to the Current Program Status Register (CPSR). */

UINT32 AmbaHvcCall(UINT32 Param0, UINT32 Param1, UINT32 Param2, UINT32 Param3);
#ifdef CONFIG_ARM64
UINT32 AmbaSmcCall(ULONG Param0, ULONG Param1, ULONG Param2, ULONG Param3, ULONG Param4);
#else
UINT32 AmbaSmcCallA32(ULONG Param0, ULONG Param1, ULONG Param2, ULONG Param3, ULONG Param4);
#define AmbaSmcCall AmbaSmcCallA32
#endif

#endif  /* AMBA_INTRINSICS_H */
