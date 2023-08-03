/**
 *  @file AmbaIntrinsics_A64.h
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
 *  @details Definitions & Constants for ARM VMSAv8-64 Cache Maintenance Operations
 *
 */

#ifndef AMBA_INTRINSICS_A64_H
#define AMBA_INTRINSICS_A64_H

#ifndef AMBA_TYPES_H
#include "AmbaTypes.h"
#endif

#ifndef __ASM__

void AMBA_enable_interrupt(void);
void AMBA_disable_interrupt(void);
void AMBA_enable_irq(void);
void AMBA_disable_irq(void);
void AMBA_enable_fiq(void);
void AMBA_disable_fiq(void);
UINT32 AMBA_get_interrupt_state(void);
void AMBA_set_interrupt_state(UINT32 x);

/*
 * PoU = to Point of Unification, PoC = to Point of Coherence, IS = Inner Shareable.
 *
 * The idea behind PoU and PoC to have way of referring to a point in the memory hierarchy in a processor agnostic way.
 * PoU is the point in the memory system where the data, instruction and table walk (MMU) interfaces of this core see the same copy of a location.
 * PoC is where this core and an other master (e.g. a DMA) see the same copy.
 *
 * IS means Inner Shareable
 */

/* Instruction cache maintenance instructions */
void AmbaCache_InstInvAll(void);
void AmbaCache_InstInvAllIS(void);
void AmbaCache_InstInvMvaU(UINT32 x);

/* Data cache maintenance instructions */
void AmbaCache_DataCleanInvSetWay(ULONG x);
void AmbaCache_DataCleanSetWay(ULONG x);
void AmbaCache_DataInvSetWay(ULONG x);
void AmbaCache_DataCleanInvMvaC(ULONG x);
void AmbaCache_DataCleanMvaC(ULONG x);
void AmbaCache_DataCleanMvaU(ULONG x);
void AmbaCache_DataInvMvaC(ULONG x);
void AmbaCache_DataZeroMva(ULONG x);

/* Exception handling instructions */
void AmbaASM_WriteVectorBaseAddr(UINT32 EL, ULONG VBAR);

/* TLB maintenance instructions (By EL) */
void AmbaTLB_InvTlbAll(UINT32 EL);
void AmbaTLB_InvTlbAllIS(UINT32 EL);
void AmbaTLB_InvTlbVa(UINT32 EL, UINT64 VA);
void AmbaTLB_InvTlbVaIS(UINT32 EL, UINT64 VA);
void AmbaTLB_InvTlbVaLastLv(UINT32 EL, UINT64 VA);
void AmbaTLB_InvTlbVaLastLvIS(UINT32 EL, UINT64 VA);
/* TLB maintenance instructions (Only EL1) */
void AmbaTLB_InvTlbAsid(UINT64 x);
void AmbaTLB_InvTlbAsidIS(UINT64 x);
void AmbaTLB_InvTlbIpaStg2(UINT64 x);
void AmbaTLB_InvTlbIpaStg2IS(UINT64 x);
void AmbaTLB_InvTlbIpaStg2LastLv(UINT64 x);
void AmbaTLB_InvTlbIpaStg2LastLvIS(UINT64 x);
void AmbaTLB_InvTlbVaAllAsid(UINT64 x);
void AmbaTLB_InvTlbVaAllAsidIS(UINT64 x);
void AmbaTLB_InvTlbVaAllAsidLastLv(UINT64 x);
void AmbaTLB_InvTlbVaAllAsidLastLvIS(UINT64 x);
void AmbaTLB_InvTlbVmidStg1(void);
void AmbaTLB_InvTlbVmidStg1IS(void);
void AmbaTLB_InvTlbVmidStg12(void);
void AmbaTLB_InvTlbVmidStg12IS(void);

/* System control instructions */
void AmbaASM_WriteAuxCtrl(UINT64 x);
UINT64 AmbaASM_ReadAuxCtrl(void);
UINT32 AmbaASM_ReadCurrentEL(void);

/* Virtual memory control instructions */
void AmbaASM_WriteSysCtrl(UINT32 EL, UINT32 SCTLR);

void AmbaASM_WriteTlbBaseCtrl(UINT32 EL, UINT64 TCR64, UINT32 TCR32);
void AmbaASM_WriteTlbBase0(UINT32 EL, UINT64 TTBR);
void AmbaASM_WriteTlbBase1(UINT64 TTBR);
void AmbaASM_WriteAuxMemAttrIndir(UINT32 EL, UINT64 AMAIR);
void AmbaASM_WriteMemAttrIndir(UINT32 EL, UINT64 MAIR);
void AmbaASM_WriteDomainAccessCtrl(UINT32 x);
void AmbaASM_WriteContextID(UINT32 x);
UINT32 AmbaASM_ReadSysCtrl(UINT32 EL);
UINT64 AmbaASM_ReadTlbBaseCtrl(UINT32 EL);
UINT64 AmbaASM_ReadTlbBase0(UINT32 EL);
UINT64 AmbaASM_ReadTlbBase1(void);
UINT64 AmbaASM_ReadAuxMemAttrIndir(UINT32 EL);
UINT64 AmbaASM_ReadMemAttrIndir(UINT32 EL);
UINT32 AmbaASM_ReadDomainAccessCtrl(void);
UINT32 AmbaASM_ReadContextID(void);

/* Implementation defined instructions */
void AmbaASM_WriteCpuAuxCtrl(UINT64 x);
void AmbaASM_WriteCpuExtCtrl(UINT64 x);
void AmbaASM_WriteCpuMemErr(UINT64 x);
void AmbaASM_WriteCachSizeSelReg(UINT64 x);
UINT64 AmbaASM_ReadCpuAuxCtrl(void);
UINT64 AmbaASM_ReadCpuExtCtrl(void);
UINT64 AmbaASM_ReadCpuMemErr(void);
UINT64 AmbaASM_ReadCacheSizeIdReg(void);

#endif  /* __ASM__ */

#endif  /* AMBA_INTRINSICS_A64_H */
