/**
 *  @file AmbaIntrinsics_A32.h
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
 *  @details Definitions & Constants for ARM VMSAv8-32 Cache and Branch Predictor Maintenance Operations
 *
 */

#ifndef AMBA_INTRINSICS_A32_H
#define AMBA_INTRINSICS_A32_H

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
 */

/* Instruction cache maintenance instructions */
void AmbaCache_BrPredInvAll(void);
void AmbaCache_BrPredInvAllIS(void);
void AmbaCache_BrPredInvMva(UINT32 x);
void AmbaCache_InstInvAll(void);
void AmbaCache_InstInvAllIS(void);
void AmbaCache_InstInvMvaU(UINT32 x);

/* Data cache maintenance instructions */
void AmbaCache_DataCleanInvSetWayA32(ULONG x);
void AmbaCache_DataCleanSetWayA32(ULONG x);
void AmbaCache_DataInvSetWayA32(ULONG x);
void AmbaCache_DataCleanInvMvaCA32(ULONG x);
void AmbaCache_DataCleanMvaCA32(ULONG x);
void AmbaCache_DataCleanMvaUA32(ULONG x);
void AmbaCache_DataInvMvaCA32(ULONG x);

/* Exception handling instructions */
void AmbaASM_WriteVectorBaseAddrA32(UINT32 x);

/* TLB maintenance instructions */
void AmbaTLB_InvInstTlbAll(void);
void AmbaTLB_InvInstTlbMva(UINT32 x);
void AmbaTLB_InvInstTlbAsid(UINT32 x);
void AmbaTLB_InvDataTlbAll(void);
void AmbaTLB_InvDataTlbMva(UINT32 x);
void AmbaTLB_InvDataTlbAsid(UINT32 x);
void AmbaTLB_InvUniTlbAll(void);
void AmbaTLB_InvUniTlbAllIS(void);
void AmbaTLB_InvUniTlbAllHyp(void);
void AmbaTLB_InvUniTlbAllHypIS(void);
void AmbaTLB_InvUniTlbAllNoSeNoHyp(void);
void AmbaTLB_InvUniTlbAllNoSeNoHypIS(void);
void AmbaTLB_InvUniTlbAsid(UINT32 x);
void AmbaTLB_InvUniTlbAsidIS(UINT32 x);
void AmbaTLB_InvTlbIpaStg2A32(UINT32 x);
void AmbaTLB_InvTlbIpaStg2ISA32(UINT32 x);
void AmbaTLB_InvTlbIpaStg2LastLvA32(UINT32 x);
void AmbaTLB_InvTlbIpaStg2LastLvISA32(UINT32 x);
void AmbaTLB_InvUniTlbMva(UINT32 x);
void AmbaTLB_InvUniTlbMvaIS(UINT32 x);
void AmbaTLB_InvUniTlbMvaHyp(UINT32 x);
void AmbaTLB_InvUniTlbMvaHypIS(UINT32 x);
void AmbaTLB_InvUniTlbMvaLastLv(UINT32 x);
void AmbaTLB_InvUniTlbMvaLastLvIS(UINT32 x);
void AmbaTLB_InvUniTlbMvaLastLvHyp(UINT32 x);
void AmbaTLB_InvUniTlbMvaLastLvHypIS(UINT32 x);

/* System control instructions */
void AmbaASM_WriteAuxCtrlA32(UINT32 x);
void AmbaASM_WriteAuxCtrl2(UINT32 x);
UINT32 AmbaASM_ReadAuxCtrlA32(void);
UINT32 AmbaASM_ReadAuxCtrl2(void);

/* Virtual memory control instructions */
void AmbaASM_WriteSysCtrlA32(UINT32 x);
void AmbaASM_WriteTlbBaseCtrlA32(UINT32 x);
void AmbaASM_WriteTlbBase0_32(UINT32 x);
void AmbaASM_WriteTlbBase1_32(UINT32 x);
void AmbaASM_WriteTlbBase0_64(UINT64 x);
void AmbaASM_WriteTlbBase1_64(UINT64 x);
void AmbaASM_WriteAuxMemAttrIndir0(UINT32 x);
void AmbaASM_WriteAuxMemAttrIndir1(UINT32 x);
void AmbaASM_WriteMemAttrIndir0(UINT32 x);
void AmbaASM_WriteMemAttrIndir1(UINT32 x);
void AmbaASM_WriteDomainAccessCtrl(UINT32 x);
void AmbaASM_WriteContextID(UINT32 x);
UINT32 AmbaASM_ReadSysCtrlA32(void);
UINT32 AmbaASM_ReadTlbBaseCtrlA32(void);
UINT32 AmbaASM_ReadTlbBase0_32(void);
UINT32 AmbaASM_ReadTlbBase1_32(void);
UINT64 AmbaASM_ReadTlbBase0_64(void);
UINT64 AmbaASM_ReadTlbBase1_64(void);
UINT32 AmbaASM_ReadAuxMemAttrIndir0(void);
UINT32 AmbaASM_ReadAuxMemAttrIndir1(void);
UINT32 AmbaASM_ReadMemAttrIndir0(void);
UINT32 AmbaASM_ReadMemAttrIndir1(void);
UINT32 AmbaASM_ReadDomainAccessCtrl(void);
UINT32 AmbaASM_ReadContextID(void);

/* Implementation defined instructions */
void AmbaASM_WriteCpuAuxCtrl(UINT64 x);
void AmbaASM_WriteCpuExtCtrl(UINT64 x);
void AmbaASM_WriteCpuMemErr(UINT64 x);
UINT64 AmbaASM_ReadCpuAuxCtrl(void);
UINT64 AmbaASM_ReadCpuExtCtrl(void);
UINT64 AmbaASM_ReadCpuMemErr(void);

#endif  /* __ASM__ */

#endif  /* AMBA_INTRINSICS_A32_H */
