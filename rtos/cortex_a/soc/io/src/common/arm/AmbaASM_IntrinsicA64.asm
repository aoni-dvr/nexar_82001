/**
 *  @file AmbaASM_IntrinsicA64.asm
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
 *  @details ASM code for Intrinsic functions
 *
 */

    .section code
    .text

    .globl AMBA_DMB
    .globl AMBA_DSB
    .globl AMBA_ISB
    .globl AMBA_SEVL
    .globl AMBA_SEV
    .globl AMBA_WFE
    .globl AMBA_WFI
    .globl __HVC
    .globl __SMC
    .globl AMBA_enable_interrupt
    .globl AMBA_disable_interrupt
    .globl AMBA_enable_irq
    .globl AMBA_disable_irq
    .globl AMBA_enable_fiq
    .globl AMBA_disable_fiq
    .globl AMBA_get_interrupt_state
    .globl AMBA_set_interrupt_state
# Implementation defined instructions
    .globl AmbaASM_WriteCpuAuxCtrl
    .globl AmbaASM_WriteCpuExtCtrl
    .globl AmbaASM_WriteCpuMemErr
    .globl AmbaASM_WriteCachSizeSelReg
    .globl AmbaASM_ReadCpuAuxCtrl
    .globl AmbaASM_ReadCpuExtCtrl
    .globl AmbaASM_ReadCpuMemErr
    .globl AmbaASM_ReadCacheSizeIdReg
# Identification instructions
    .globl AmbaASM_ReadMultiprocessorAffinity
# Instruction cache maintenance instructions
    .globl AmbaCache_InstInvAll
    .globl AmbaCache_InstInvAllIS
    .globl AmbaCache_InstInvMvaU
# Data cache maintenance instructions
    .globl AmbaCache_DataCleanInvSetWay
    .globl AmbaCache_DataCleanSetWay
    .globl AmbaCache_DataInvSetWay
    .globl AmbaCache_DataCleanInvMvaC
    .globl AmbaCache_DataCleanMvaC
    .globl AmbaCache_DataCleanMvaU
    .globl AmbaCache_DataInvMvaC
    .globl AmbaCache_DataZeroMva
# Exception handling instructions
    .globl AmbaASM_WriteVectorBaseAddr
# Generic counter timer instructions
    .globl AmbaASM_ReadTimerFreq
    .globl AmbaASM_ReadPhysTimerCount
    .globl AmbaASM_WriteTimerFreq
    .globl AmbaASM_WritePhysTimerCtrl
    .globl AmbaASM_WritePhysTimerCompareVal
# Address translation instructions
    .globl AmbaAT_Stg12EL0Read
    .globl AmbaAT_Stg12EL1Read
    .globl AmbaAT_Stg1EL0Read
    .globl AmbaAT_Stg1EL1Read
    .globl AmbaAT_Stg1EL2Read
    .globl AmbaAT_Stg1EL3Read
    .globl AmbaAT_ReadPAR
    .globl AmbaAT_Stg12EL0Write
    .globl AmbaAT_Stg12EL1Write
    .globl AmbaAT_Stg1EL0Write
    .globl AmbaAT_Stg1EL1Write
    .globl AmbaAT_Stg1EL2Write
    .globl AmbaAT_Stg1EL3Write
    .globl AmbaAT_WritePAR
# TLB maintenance instructions (By EL)
    .globl AmbaTLB_InvTlbAll
    .globl AmbaTLB_InvTlbAllIS
    .globl AmbaTLB_InvTlbVa
    .globl AmbaTLB_InvTlbVaIS
    .globl AmbaTLB_InvTlbVaLastLv
    .globl AmbaTLB_InvTlbVaLastLvIS
# TLB maintenance instructions (EL1 only)
    .globl AmbaTLB_InvTlbAsid
    .globl AmbaTLB_InvTlbAsidIS
    .globl AmbaTLB_InvTlbIpaStg2
    .globl AmbaTLB_InvTlbIpaStg2IS
    .globl AmbaTLB_InvTlbIpaStg2LastLv
    .globl AmbaTLB_InvTlbIpaStg2LastLvIS
    .globl AmbaTLB_InvTlbVaAllAsid
    .globl AmbaTLB_InvTlbVaAllAsidIS
    .globl AmbaTLB_InvTlbVaAllAsidLastLv
    .globl AmbaTLB_InvTlbVaAllAsidLastLvIS
    .globl AmbaTLB_InvTlbVmidStg1
    .globl AmbaTLB_InvTlbVmidStg1IS
    .globl AmbaTLB_InvTlbVmidStg12
    .globl AmbaTLB_InvTlbVmidStg12IS
# System control instructions
    .globl AmbaASM_WriteAuxCtrl
    .globl AmbaASM_WriteCoprocessorAccessCtrl
    .globl AmbaASM_ReadAuxCtrl
    .globl AmbaASM_ReadCoprocessorAccessCtrl
    .globl AmbaASM_ReadCurrentEL
# Virtual memory control instructions
    .globl AmbaASM_WriteSysCtrl
    .globl AmbaASM_WriteTlbBaseCtrl
    .globl AmbaASM_WriteTlbBase0
    .globl AmbaASM_WriteTlbBase1
    .globl AmbaASM_WriteAuxMemAttrIndir
    .globl AmbaASM_WriteMemAttrIndir
    .globl AmbaASM_WriteDomainAccessCtrl
    .globl AmbaASM_WriteContextID
    .globl AmbaASM_ReadSysCtrl
    .globl AmbaASM_ReadTlbBaseCtrl
    .globl AmbaASM_ReadTlbBase0
    .globl AmbaASM_ReadTlbBase1
    .globl AmbaASM_ReadAuxMemAttrIndir
    .globl AmbaASM_ReadMemAttrIndir
    .globl AmbaASM_ReadDomainAccessCtrl
    .globl AmbaASM_ReadContextID

#-----------------------------------------------------------------------------
# Name:     AMBA_DMB
# Purpose:  Perform 'Data Memory Barrier'
#-----------------------------------------------------------------------------
AMBA_DMB:
    DMB SY
    RET

#-----------------------------------------------------------------------------
# Name:     AMBA_DSB
# Purpose:  Perform 'Data Synchronization Barrier'
#-----------------------------------------------------------------------------
AMBA_DSB:
    DSB SY
    RET

#-----------------------------------------------------------------------------
# Name:     AMBA_ISB
# Purpose:  Perform 'Instruction Synchronization Barrier'
#-----------------------------------------------------------------------------
AMBA_ISB:
    ISB SY
    RET

#-----------------------------------------------------------------------------
# Name:     AMBA_SEVL
# Purpose:  Perform 'Set Event Locally.' Actually 5 NOPs are performed.
#-----------------------------------------------------------------------------
AMBA_SEVL:
    NOP
    NOP
    NOP
    NOP
    NOP
    RET

#-----------------------------------------------------------------------------
# Name:     AMBA_SEV
# Purpose:  Perform 'Set Event'
#-----------------------------------------------------------------------------
AMBA_SEV:
    SEV
    RET

#-----------------------------------------------------------------------------
# Name:     AMBA_WFE
# Purpose:  Perform 'Wait For Event'
#-----------------------------------------------------------------------------
AMBA_WFE:
    WFE
    RET

#-----------------------------------------------------------------------------
# Name:     AMBA_WFI
# Purpose:  Perform 'Wait For Interrupt'
#-----------------------------------------------------------------------------
AMBA_WFI:
    WFI
    RET

#-----------------------------------------------------------------------------
# Name:     AMBA_enable_interrupt
# Purpose:  enable interrupt.
#-----------------------------------------------------------------------------
AMBA_enable_interrupt:
    MSR DAIFClr, #3
    RET

#-----------------------------------------------------------------------------
# Name:     AMBA_disable_interrupt
# Purpose:  disable interrupt.
#-----------------------------------------------------------------------------
AMBA_disable_interrupt:
    MSR DAIFSet, #3
    RET

#-----------------------------------------------------------------------------
# Name:     AMBA_enable_irq
# Purpose:  enable IRQ.
#-----------------------------------------------------------------------------
AMBA_enable_irq:
    MSR DAIFClr, #2
    RET

#-----------------------------------------------------------------------------
# Name:     AMBA_disable_irq
# Purpose:  disable IRQ.
#-----------------------------------------------------------------------------
AMBA_disable_irq:
    MSR DAIFSet, #2
    RET

#-----------------------------------------------------------------------------
# Name:     AMBA_enable_fiq
# Purpose:  enable FIQ.
#-----------------------------------------------------------------------------
AMBA_enable_fiq:
    MSR DAIFClr, #1
    RET

#-----------------------------------------------------------------------------
# Name:     AMBA_disable_fiq
# Purpose:  enable FIQ.
#-----------------------------------------------------------------------------
AMBA_disable_fiq:
    MSR DAIFSet, #1
    RET

#-----------------------------------------------------------------------------
# Name:     AMBA_get_interrupt_state
# Purpose:  Perform DAIF (Debug, SError, IRQ, and FIQ) read
# Return:   X0
#-----------------------------------------------------------------------------
AMBA_get_interrupt_state:
    MRS X0, DAIF
    RET

#-----------------------------------------------------------------------------
# Name:     AMBA_set_interrupt_state
# Purpose:  Perform DAIF (Debug, SError, IRQ, and FIQ) write
# Return:   X0
#-----------------------------------------------------------------------------
AMBA_set_interrupt_state:
    MSR DAIF, X0
    RET

#-----------------------------------------------------------------------------
# Name:     AmbaASM_WriteCpuAuxCtrl
# Purpose:  Performs S3_1_C15_C2_0 (EL1 CPU Auxiliary Control Register) write.
# Return:   X0
#-----------------------------------------------------------------------------
AmbaASM_WriteCpuAuxCtrl:
    MSR S3_1_C15_C2_0, X0
    RET

#-----------------------------------------------------------------------------
# Name:     AmbaASM_WriteCpuExtCtrl
# Purpose:  Performs S3_1_C15_C2_1 (EL1 CPU Extended Control Register) write.
# Return:   X0
#-----------------------------------------------------------------------------
AmbaASM_WriteCpuExtCtrl:
    MSR S3_1_C15_C2_1, X0
    RET

#-----------------------------------------------------------------------------
# Name:     AmbaASM_WriteCpuMemErr
# Purpose:  Performs S3_1_C15_C2_2 (EL1 CPU Memory Error Syndrome Register) write.
# Return:   X0
#-----------------------------------------------------------------------------
AmbaASM_WriteCpuMemErr:
    MSR S3_1_C15_C2_2, X0
    RET

#-----------------------------------------------------------------------------
# Name:     AmbaASM_ReadCpuAuxCtrl
# Purpose:  Performs S3_1_C15_C2_0 (EL1 CPU Auxiliary Control Register) read.
# Return:   X0
#-----------------------------------------------------------------------------
AmbaASM_ReadCpuAuxCtrl:
    MRS X0, S3_1_C15_C2_0
    RET

#-----------------------------------------------------------------------------
# Name:     AmbaASM_ReadCpuExtCtrl
# Purpose:  Performs S3_1_C15_C2_1 (EL1 CPU Extended Control Register) read.
# Return:   X0
#-----------------------------------------------------------------------------
AmbaASM_ReadCpuExtCtrl:
    MRS X0, S3_1_C15_C2_1
    RET

#-----------------------------------------------------------------------------
# Name:     AmbaASM_ReadCpuMemErr
# Purpose:  Performs S3_1_C15_C2_2 (EL1 CPU Memory Error Syndrome Register) read.
# Return:   X0
#-----------------------------------------------------------------------------
AmbaASM_ReadCpuMemErr:
    MRS X0, S3_1_C15_C2_2
    RET

#-----------------------------------------------------------------------------
# Name:     AmbaASM_ReadMultiprocessorAffinity
# Purpose:  Performs MPIDR_EL1 (Multiprocessor Affinity Register) read.
# Return:   X0
#-----------------------------------------------------------------------------
AmbaASM_ReadMultiprocessorAffinity:
    MRS X0, MPIDR_EL1
    RET

#-----------------------------------------------------------------------------
# Name:     AmbaCache_InstInvAll
# Purpose:  Performs "IC IALLU" (Invalidate all to Point of Unification)
# Params:   None
#-----------------------------------------------------------------------------
AmbaCache_InstInvAll:
    #SYS 0, C7, C5, 0, X0
    IC IALLU
    RET

#-----------------------------------------------------------------------------
# Name:     AmbaCache_InstInvAllIS
# Purpose:  Performs "IC IALLUIS" (Invalidate all to Point of Unification, Inner Shareable)
# Params: None
#-----------------------------------------------------------------------------
AmbaCache_InstInvAllIS:
    SYS 0, C7, C1, 0, X0
    RET

#-----------------------------------------------------------------------------
# Name:     AmbaCache_InstInvMvaU
# Purpose:  Performs "IC IVAU" (Invalidate by virtual address to Point of Unification)
# Return:   X0
#-----------------------------------------------------------------------------
AmbaCache_InstInvMvaU:
    SYS 3, C7, C5, 1, X0
    RET

#-----------------------------------------------------------------------------
# Name:     AmbaCache_DataCleanInvSetWay
# Purpose:  Performs "DC CISW" (Clean and invalidate by set/way)
# Return:   X0
#-----------------------------------------------------------------------------
AmbaCache_DataCleanInvSetWay:
    SYS 0, C7, C14, 2, X0
    RET

#-----------------------------------------------------------------------------
# Name:     AmbaCache_DataCleanSetWay
# Purpose:  Performs "DC CSW" (Clean by set/way)
# Return:   X0
#-----------------------------------------------------------------------------
AmbaCache_DataCleanSetWay:
    SYS 0, C7, C10, 2, X0
    RET

#-----------------------------------------------------------------------------
# Name:     AmbaCache_DataInvSetWay
# Purpose:  Performs "DC ISW" (Invalidate by set/way)
# Return:   X0
#-----------------------------------------------------------------------------
AmbaCache_DataInvSetWay:
    SYS 0, C7, C6, 2, X0
    RET

#-----------------------------------------------------------------------------
# Name:     AmbaCache_DataCleanInvMvaC
# Purpose:  Performs "DC CIVAC" (Clean and invalidate by virtual address to
#           Point of Coherency)
# Return:   X0
#-----------------------------------------------------------------------------
AmbaCache_DataCleanInvMvaC:
    SYS 3, C7, C14, 1, X0
    RET

#-----------------------------------------------------------------------------
# Name:     AmbaCache_DataCleanMvaC
# Purpose:  Performs "DC CVAC" (Clean by virtual address to Point of Coherency)
# Return:   X0
#-----------------------------------------------------------------------------
AmbaCache_DataCleanMvaC:
    SYS 3, C7, C10, 1, X0
    RET

#-----------------------------------------------------------------------------
# Name:     AmbaCache_DataCleanMvaU
# Purpose:  Performs "DC CVAC" (Clean by virtual address to Point of Coherency)
# Return:   X0
#-----------------------------------------------------------------------------
AmbaCache_DataCleanMvaU:
    SYS 3, C7, C11, 1, X0
    RET

#-----------------------------------------------------------------------------
# Name:     AmbaCache_DataInvMvaC
# Purpose:  Performs "DC IVAC" (Invalidate by virtual address to Point of Coherency)
# Return:   X0
#-----------------------------------------------------------------------------
AmbaCache_DataInvMvaC:
    SYS 0, C7, C6, 1, X0
    RET

#-----------------------------------------------------------------------------
# Name:     AmbaCache_DataZeroMva
# Purpose:  Performs "DC ZVA" (Zero data cache by address)
# Return:   X0
#-----------------------------------------------------------------------------
AmbaCache_DataZeroMva:
    SYS 3, C7, C4, 2, X0
    RET

#-----------------------------------------------------------------------------
# Name:     AmbaASM_ReadCacheSizeIdReg
# Purpose:  Performs CCSIDR_EL1 (Current Cache Size ID Register) read.
# Params:   X0
#-----------------------------------------------------------------------------
AmbaASM_ReadCacheSizeIdReg:
    MRS X0, CCSIDR_EL1
    RET

#-----------------------------------------------------------------------------
# Name:     AmbaASM_WriteCachSizeSelReg
# Purpose:  Performs CSSELR_EL1 (Selects the current Cache Size ID Register) write.
# Params:   W0, X1
#-----------------------------------------------------------------------------
AmbaASM_WriteCachSizeSelReg:
    MSR CSSELR_EL1, X0
    RET

#-----------------------------------------------------------------------------
# Name:     AmbaASM_WriteVectorBaseAddr
# Purpose:  Performs VBAR_ELx (Vector Base Address Register, ELx) write.
# Params:   W0, X1
#-----------------------------------------------------------------------------
AmbaASM_WriteVectorBaseAddr:
    CMP W0, #0x2
    B.GT 3f
    B.EQ 2f
1:  MSR VBAR_EL1, X1
    RET
2:  MSR VBAR_EL2, X1
    RET
3:  MSR VBAR_EL3, X1
    RET

#-----------------------------------------------------------------------------
# Name:     AmbaASM_ReadTimerFreq
# Purpose:  Performs CNTFRQ_EL0 (Counter-timer Frequency register, EL0) read.
# Return:   X0
#-----------------------------------------------------------------------------
AmbaASM_ReadTimerFreq:
    MRS X0, CNTFRQ_EL0
    RET

#-----------------------------------------------------------------------------
# Name:     AmbaASM_ReadPhysTimerCount
# Purpose:  Performs CNTPCT_EL0 (Counter-timer Physical Count register, EL0) read.
# Return:   X0
#-----------------------------------------------------------------------------
AmbaASM_ReadPhysTimerCount:
    MRS X0, CNTPCT_EL0
    RET

#-----------------------------------------------------------------------------
# Name:     AmbaASM_WriteTimerFreq
# Purpose:  Performs CNTFRQ_EL0 (Counter-timer Frequency register, EL0) write.
# Return:   X0
#-----------------------------------------------------------------------------
AmbaASM_WriteTimerFreq:
    MSR CNTFRQ_EL0, X0
    RET

#-----------------------------------------------------------------------------
# Name:     AmbaASM_WritePhysTimerCtrl
# Purpose:  Performs CNTP_CTL_EL0 (Counter-timer Physical Timer Control register) write.
# Return:   X0
#-----------------------------------------------------------------------------
AmbaASM_WritePhysTimerCtrl:
    MSR CNTP_CTL_EL0, X0
    RET

#-----------------------------------------------------------------------------
# Name:     AmbaASM_WritePhysTimerCompareVal
# Purpose:  Performs CNTP_CVAL_EL0 (Counter-timer Physical Timer CompareValue register) write.
# Params:   X0
#-----------------------------------------------------------------------------
AmbaASM_WritePhysTimerCompareVal:
    MSR CNTP_CVAL_EL0, X0
    RET

#-----------------------------------------------------------------------------
# Name:     AmbaAT_Stg12EL0Read
# Purpose:  Performs "AT S12E0R" (Address Translate Stages 1 and 2 EL0) read
# Return:   X0
#-----------------------------------------------------------------------------
AmbaAT_Stg12EL0Read:
    SYS 4, C7, C8, 6, X0
    RET

#-----------------------------------------------------------------------------
# Name:     AmbaAT_Stg12EL0Write
# Purpose:  Performs "AT S12E0W" (Address Translate Stages 1 and 2 EL0) write
# Return:   X0
#-----------------------------------------------------------------------------
AmbaAT_Stg12EL0Write:
    SYS 4, C7, C8, 7, X0
    RET

#-----------------------------------------------------------------------------
# Name:     AmbaAT_Stg12EL1Read
# Purpose:  Performs "AT S12E1R" (Address Translate Stages 1 and 2 EL1) read
# Return:   X0
#-----------------------------------------------------------------------------
AmbaAT_Stg12EL1Read:
    SYS 4, C7, C8, 4, X0
    RET

#-----------------------------------------------------------------------------
# Name:     AmbaAT_Stg12EL1Write
# Purpose:  Performs "AT S12E1W" (Address Translate Stages 1 and 2 EL1) write
# Return:   X0
#-----------------------------------------------------------------------------
AmbaAT_Stg12EL1Write:
    SYS 4, C7, C8, 5, X0
    RET

#-----------------------------------------------------------------------------
# Name:     AmbaAT_Stg1EL0Read
# Purpose:  Performs "AT S1E0R" (Address Translate Stage 1 EL0) read
# Return:   X0
#-----------------------------------------------------------------------------
AmbaAT_Stg1EL0Read:
    SYS 0, C7, C8, 2, X0
    RET

#-----------------------------------------------------------------------------
# Name:     AmbaAT_Stg1EL0Write
# Purpose:  Performs "AT S1E0W" (Address Translate Stage 1 EL0) write
# Return:   X0
#-----------------------------------------------------------------------------
AmbaAT_Stg1EL0Write:
    SYS 0, C7, C8, 3, X0
    RET

#-----------------------------------------------------------------------------
# Name:     AmbaAT_Stg1EL1Read
# Purpose:  Performs "AT S1E1R" (Address Translate Stage 1 EL1) read
# Return:   X0
#-----------------------------------------------------------------------------
AmbaAT_Stg1EL1Read:
    SYS 0, C7, C8, 0, X0
    RET

#-----------------------------------------------------------------------------
# Name:     AmbaAT_Stg1EL1Write
# Purpose:  Performs "AT S1E1W" (Address Translate Stage 1 EL1) write
# Return:   X0
#-----------------------------------------------------------------------------
AmbaAT_Stg1EL1Write:
    SYS 0, C7, C8, 1, X0
    RET

#-----------------------------------------------------------------------------
# Name:     AmbaAT_Stg1EL2Read
# Purpose:  Performs "AT S1E2R" (Address Translate Stages 1 EL2) read
# Return:   X0
#-----------------------------------------------------------------------------
AmbaAT_Stg1EL2Read:
    SYS 4, C7, C8, 0, X0
    RET

#-----------------------------------------------------------------------------
# Name:     AmbaAT_Stg1EL2Write
# Purpose:  Performs "AT S1E2W" (Address Translate Stages 1 EL2) write
# Return:   X0
#-----------------------------------------------------------------------------
AmbaAT_Stg1EL2Write:
    SYS 4, C7, C8, 1, X0
    RET

#-----------------------------------------------------------------------------
# Name:     AmbaAT_Stg1EL3Read
# Purpose:  Performs "AT S1E3R" (Address Translate Stages 1 EL3) read
# Return:   X0
#-----------------------------------------------------------------------------
AmbaAT_Stg1EL3Read:
    SYS 6, C7, C8, 0, X0
    RET

#-----------------------------------------------------------------------------
# Name:     AmbaAT_Stg1EL3Write
# Purpose:  Performs "AT S1E3W" (Address Translate Stages 1 EL3) write
# Return:   X0
#-----------------------------------------------------------------------------
AmbaAT_Stg1EL3Write:
    SYS 6, C7, C8, 1, X0
    RET

#-----------------------------------------------------------------------------
# Name:     AmbaAT_WritePAR
# Purpose:  Performs "PAR_EL1" (Physical Address Register EL1) write
# Return:   X0
#-----------------------------------------------------------------------------
AmbaAT_WritePAR:
    MSR PAR_EL1, X0
    RET

#-----------------------------------------------------------------------------
# Name:     AmbaAT_ReadPAR
# Purpose:  Performs "PAR_EL1" (Physical Address Register EL1) read
# Return:   X0
#-----------------------------------------------------------------------------
AmbaAT_ReadPAR:
    MRS X0, PAR_EL1
    RET

#-----------------------------------------------------------------------------
# Name:     AmbaTLB_InvTlbAll
# Purpose:  Performs "TLBI ALLEx" (TLB Invalidate All, ELx) write
# Return:   W0, X1
#-----------------------------------------------------------------------------
AmbaTLB_InvTlbAll:
    CMP W0, #0x2
    B.GT 3f
    B.EQ 2f
1:  tlbi	vmalle1    //since TLB Invalidate All is not support in EL1, we use vmalle1 in EL1
    dsb	sy
    isb
    RET
2:  SYS 4, C8, C7, 0
    RET
3:  SYS 6, C8, C7, 0
    RET

#-----------------------------------------------------------------------------
# Name:     AmbaTLB_InvTlbAllIS
# Purpose:  Performs "TLBI ALLExIS" (TLB Invalidate All, ELx, Inner Shareable) write
# Return:   W0, X1
#-----------------------------------------------------------------------------
AmbaTLB_InvTlbAllIS:
    CMP W0, #0x2
    B.GT 3f
    B.EQ 2f
1:  SYS 4, C8, C3, 4, X1
    RET
2:  SYS 4, C8, C3, 0, X1
    RET
3:  SYS 6, C8, C3, 0, X1
    RET

#-----------------------------------------------------------------------------
# Name:     AmbaTLB_InvTlbVa
# Purpose:  Performs "TLBI VAEx" (TLB Invalidate by VA, ELx) write
# Params:   W0, X1
#-----------------------------------------------------------------------------
AmbaTLB_InvTlbVa:
    CMP W0, #0x2
    B.GT 3f
    B.EQ 2f
1:  SYS 0, C8, C7, 1, X1
    RET
2:  SYS 4, C8, C7, 1, X1
    RET
3:  SYS 6, C8, C7, 1, X1
    RET

#-----------------------------------------------------------------------------
# Name:     AmbaTLB_InvTlbVaIS
# Purpose:  Performs "TLBI VAExIS" (TLB Invalidate by VA, ELx, Inner Shareable) write
# Params:   W0, X1
#-----------------------------------------------------------------------------
AmbaTLB_InvTlbVaIS:
    CMP W0, #0x2
    B.GT 3f
    B.EQ 2f
1:  SYS 0, C8, C3, 1, X1
    RET
2:  SYS 4, C8, C3, 1, X1
    RET
3:  SYS 6, C8, C3, 1, X1
    RET

#-----------------------------------------------------------------------------
# Name:     AmbaTLB_InvTlbVaLastLv
# Purpose:  Performs "TLBI VALEx" (TLB Invalidate by VA, Last level, ELx) write
# Params:   W0, X1
#-----------------------------------------------------------------------------
AmbaTLB_InvTlbVaLastLv:
    CMP W0, #0x2
    B.GT 3f
    B.EQ 2f
1:  SYS 0, C8, C7, 5, X1
    RET
2:  SYS 4, C8, C7, 5, X1
    RET
3:  SYS 6, C8, C7, 5, X1
    RET

#-----------------------------------------------------------------------------
# Name:     AmbaTLB_InvTlbVaLastLvIS
# Purpose:  Performs "TLBI VALExIS" (TLB Invalidate by VA, Last level, ELx, Inner Shareable) write
# Params:   W0, X1
#-----------------------------------------------------------------------------
AmbaTLB_InvTlbVaLastLvIS:
    CMP W0, #0x2
    B.GT 3f
    B.EQ 2f
1:  SYS 0, C8, C3, 5, X1
    RET
2:  SYS 4, C8, C3, 5, X1
    RET
3:  SYS 6, C8, C3, 5, X1
    RET

#-----------------------------------------------------------------------------
# Name:     AmbaTLB_InvTlbAsid
# Purpose:  Performs "TLBI ASIDE1" (TLB Invalidate by ASID, EL1) write
# Params:   X0
#-----------------------------------------------------------------------------
AmbaTLB_InvTlbAsid:
    SYS 0, C8, C7, 2, X0
    RET

#-----------------------------------------------------------------------------
# Name:     AmbaTLB_InvTlbAsidIS
# Purpose:  Performs "TLBI ASIDE1IS" (TLB Invalidate by ASID, EL1, Inner Shareable) write
# Params:   X0
#-----------------------------------------------------------------------------
AmbaTLB_InvTlbAsidIS:
    SYS 0, C8, C3, 2, X0
    RET

#-----------------------------------------------------------------------------
# Name:     AmbaTLB_InvTlbIpaStg2
# Purpose:  Performs "TLBI IPAS2E1" (TLB Invalidate by Intermediate Physical Address, Stage 2, EL1) write
# Params:   X0
#-----------------------------------------------------------------------------
AmbaTLB_InvTlbIpaStg2:
    SYS 4, C8, C4, 1, X0
    RET

#-----------------------------------------------------------------------------
# Name:     AmbaTLB_InvTlbIpaStg2IS
# Purpose:  Performs "TLBI IPAS2E1IS" (TLB Invalidate by Intermediate Physical Address, Stage 2, EL1, Inner Shareable) write
# Params:   X0
#-----------------------------------------------------------------------------
AmbaTLB_InvTlbIpaStg2IS:
    SYS 4, C8, C0, 1, X0
    RET

#-----------------------------------------------------------------------------
# Name:     AmbaTLB_InvTlbIpaStg2LastLv
# Purpose:  Performs "TLBI IPAS2LE1" (TLB Invalidate by Intermediate Physical Address, Stage 2, Last level, EL1) write
# Params:   X0
#-----------------------------------------------------------------------------
AmbaTLB_InvTlbIpaStg2LastLv:
    SYS 4, C8, C4, 5, X0
    RET

#-----------------------------------------------------------------------------
# Name:     AmbaTLB_InvTlbIpaStg2LastLvIS
# Purpose:  Performs "TLBI IPAS2LE1IS" (TLB Invalidate by Intermediate Physical Address, Stage 2, Last level, EL1, Inner Shareable) write
# Params:   X0
#-----------------------------------------------------------------------------
AmbaTLB_InvTlbIpaStg2LastLvIS:
    SYS 4, C8, C0, 5, X0
    RET

#-----------------------------------------------------------------------------
# Name:     AmbaTLB_InvTlbVaAllAsid
# Purpose:  Performs "TLBI VAAE1" (TLB Invalidate by VA, All ASID, EL1) write
# Params:   X0
#-----------------------------------------------------------------------------
AmbaTLB_InvTlbVaAllAsid:
    SYS 0, C8, C7, 3, X0
    RET

#-----------------------------------------------------------------------------
# Name:     AmbaTLB_InvTlbVaAllAsidIS
# Purpose:  Performs "TLBI VAAE1IS" (TLB Invalidate by VA, All ASID, EL1, 1, Inner Shareable) write
# Params:   X0
#-----------------------------------------------------------------------------
AmbaTLB_InvTlbVaAllAsidIS:
    SYS 0, C8, C3, 3, X0
    RET

#-----------------------------------------------------------------------------
# Name:     AmbaTLB_InvTlbVaAllAsidLastLv
# Purpose:  Performs "TLBI VAALE1" (TLB Invalidate by VA, All ASID, Last level, EL1) write
# Params:   X0
#-----------------------------------------------------------------------------
AmbaTLB_InvTlbVaAllAsidLastLv:
    SYS 0, C8, C7, 7, X0
    RET

#-----------------------------------------------------------------------------
# Name:     AmbaTLB_InvTlbVaAllAsidLastLvIS
# Purpose:  Performs "TLBI VAALE1IS" (TLB Invalidate by VA, All ASID, Last level, EL1, Inner Shareable) write
# Params:   X0
#-----------------------------------------------------------------------------
AmbaTLB_InvTlbVaAllAsidLastLvIS:
    SYS 0, C8, C3, 7, X0
    RET

#-----------------------------------------------------------------------------
# Name:     AmbaTLB_InvTlbVmidStg1
# Purpose:  Performs "TLBI VMALLE1" ( TLB Invalidate by VMID, All at stage 1, EL1) write
#-----------------------------------------------------------------------------
AmbaTLB_InvTlbVmidStg1:
    SYS 0, C8, C7, 0, X0
    RET

#-----------------------------------------------------------------------------
# Name:     AmbaTLB_InvTlbVmidStg1IS
# Purpose:  Performs "TLBI VMALLE1IS" ( TLB Invalidate by VMID, All at stage 1, EL1, Inner Shareable) write
#-----------------------------------------------------------------------------
AmbaTLB_InvTlbVmidStg1IS:
    SYS 0, C8, C3, 0, X0
    RET

#-----------------------------------------------------------------------------
# Name:     AmbaTLB_InvTlbVmidStg12
# Purpose:  Performs "TLBI VMALLS12E1" (TLB Invalidate by VMID, All at Stage 1 and 2, EL1) write
#-----------------------------------------------------------------------------
AmbaTLB_InvTlbVmidStg12:
    SYS 4, C8, C7, 6, X0
    RET

#-----------------------------------------------------------------------------
# Name:     AmbaTLB_InvTlbVmidStg12IS
# Purpose:  Performs "TLBI VMALLS12E1IS" (TLB Invalidate by VMID, All at Stage 1 and 2, EL1, Inner Shareable) write
#-----------------------------------------------------------------------------
AmbaTLB_InvTlbVmidStg12IS:
    SYS 4, C8, C3, 6, X0
    RET

#-----------------------------------------------------------------------------
# Name:     AmbaASM_WriteAuxCtrl
# Purpose:  Performs ACTLR_EL1 (Auxiliary Control Register, EL1) write.
# Params:   X0
#-----------------------------------------------------------------------------
AmbaASM_WriteAuxCtrl:
    MSR ACTLR_EL1, X0
    RET

#-----------------------------------------------------------------------------
# Name:     AmbaASM_WriteCoprocessorAccessCtrl
# Purpose:  Performs CPACR_EL1 (Architectural Feature Access Control Register, EL1) write.
# Params:   X0
#-----------------------------------------------------------------------------
AmbaASM_WriteCoprocessorAccessCtrl:
    MSR CPACR_EL1, X0
    RET

#-----------------------------------------------------------------------------
# Name:     AmbaASM_ReadAuxCtrl
# Purpose:  Performs ACTLR_EL1 (Auxiliary Control Register, EL1) read.
# Return:   X0
#-----------------------------------------------------------------------------
AmbaASM_ReadAuxCtrl:
    MRS X0, ACTLR_EL1
    RET

#-----------------------------------------------------------------------------
# Name:     AmbaASM_ReadCoprocessorAccessCtrl
# Purpose:  Performs CPACR_EL1 (Architectural Feature Access Control Register, EL1) read.
# Return:   X0
#-----------------------------------------------------------------------------
AmbaASM_ReadCoprocessorAccessCtrl:
    MRS X0, CPACR_EL1
    RET

#-----------------------------------------------------------------------------
# Name:     AmbaASM_ReadCurrentEL
# Purpose:  Performs CurrentEL (Current Exception Level) read.
# Return:   X0
#-----------------------------------------------------------------------------
AmbaASM_ReadCurrentEL:
    MRS X0, CurrentEL
    RET

#-----------------------------------------------------------------------------
# Name:     AmbaASM_WriteSysCtrl
# Purpose:  Performs SCTLR_ELx (System Control Register, ELx) write.
# Return:   X0
#-----------------------------------------------------------------------------
AmbaASM_WriteSysCtrl:
    CMP W0, #0x2
    B.GT 3f
    B.EQ 2f
1:  MSR SCTLR_EL1, X0
    DSB SY
    ISB
    RET
2:  MSR SCTLR_EL2, X0
    DSB SY
    ISB
    RET
3:  MSR SCTLR_EL3, X0
    DSB SY
    ISB
    RET

#-----------------------------------------------------------------------------
# Name:     AmbaASM_WriteTlbBaseCtrl
# Purpose:  Performs TCR_ELx (Translation Control Register, ELx) write.
# Params:   W0, X1, X2
#-----------------------------------------------------------------------------
AmbaASM_WriteTlbBaseCtrl:
    CMP W0, #0x2
    B.GT 3f
    B.EQ 2f
1:  MSR TCR_EL1, X1
    RET
2:  MSR TCR_EL2, X2
    RET
3:  MSR TCR_EL3, X2
    RET

#-----------------------------------------------------------------------------
# Name:     AmbaASM_WriteTlbBase0
# Purpose:  Performs TTBR0_ELx ( Translation Table Base Register 0, ELx) write.
# Params:   W0, X1
#-----------------------------------------------------------------------------
AmbaASM_WriteTlbBase0:
    CMP W0, #0x2
    B.GT 3f
    B.EQ 2f
1:  MSR TTBR0_EL1, X1
    RET
2:  MSR TTBR0_EL2, X1
    RET
3:  MSR TTBR0_EL3, X1
    RET

#-----------------------------------------------------------------------------
# Name:     AmbaASM_WriteTlbBase1
# Purpose:  Performs TTBR1_EL1 (Translation Table Base Register 1, EL1) write.
# Params:   X0
#-----------------------------------------------------------------------------
AmbaASM_WriteTlbBase1:
    MSR TTBR1_EL1, X0
    RET

#-----------------------------------------------------------------------------
# Name:     AmbaASM_WriteAuxMemAttrIndir
# Purpose:  Performs AMAIR_ELx (Auxiliary Memory Attribute Indir Register, ELx) write.
# Params:   W0, X1
#-----------------------------------------------------------------------------
AmbaASM_WriteAuxMemAttrIndir:
    CMP W0, #0x2
    B.GT 3f
    B.EQ 2f
1:  MSR AMAIR_EL1, X1
    RET
2:  MSR AMAIR_EL2, X1
    RET
3:  MSR AMAIR_EL3, X1
    RET

#-----------------------------------------------------------------------------
# Name:     AmbaASM_WriteMemAttrIndir
# Purpose:  Performs MAIR_ELx (Memory Attribute Indir Register, ELx) write.
# Params:   W0, X1
#-----------------------------------------------------------------------------
AmbaASM_WriteMemAttrIndir:
    CMP W0, #0x2
    B.GT 3f
    B.EQ 2f
1:  MSR MAIR_EL1, X1
    RET
2:  MSR MAIR_EL2, X1
    RET
3:  MSR MAIR_EL3, X1
    RET

#-----------------------------------------------------------------------------
# Name:     AmbaASM_WriteDomainAccessCtrl
# Purpose:  Performs DACR32_EL2 (Domain Access Control Register, EL2) write.
# Params:   X0
#-----------------------------------------------------------------------------
AmbaASM_WriteDomainAccessCtrl:
    MSR DACR32_EL2, X0
    RET

#-----------------------------------------------------------------------------
# Name:     AmbaASM_WriteContextID
# Purpose:  Performs CONTEXTIDR_EL1 (Context ID Register, EL1) write.
# Params:   X0
#-----------------------------------------------------------------------------
AmbaASM_WriteContextID:
    MSR CONTEXTIDR_EL1, X0
    RET

#-----------------------------------------------------------------------------
# Name:     AmbaASM_ReadSysCtrl
# Purpose:  Performs SCTLR_ELx (System Control Register, ELx) read.
# Params:   W0
# Return:   X0
#-----------------------------------------------------------------------------
AmbaASM_ReadSysCtrl:
    CMP W0, #0x2
    B.GT 3f
    B.EQ 2f
1:  MRS X0, SCTLR_EL1
    RET
2:  MRS X0, SCTLR_EL2
    RET
3:  MRS X0, SCTLR_EL3
    RET

#-----------------------------------------------------------------------------
# Name:     AmbaASM_ReadTlbBaseCtrl
# Purpose:  Performs TCR_ELx (Translation Control Register, ELx) read.
# Params:   W0
# Return:   X0
#-----------------------------------------------------------------------------
AmbaASM_ReadTlbBaseCtrl:
    CMP W0, #0x2
    B.GT 3f
    B.EQ 2f
1:  MRS X0, TCR_EL1
    RET
2:  MRS X0, TCR_EL2
    RET
3:  MRS X0, TCR_EL3
    RET

#-----------------------------------------------------------------------------
# Name:     AmbaASM_ReadTlbBase0
# Purpose:  Performs TTBR0_ELx (Translation Table Base Register 0, ELx) read.
# Params:   W0
# Return:   X0
#-----------------------------------------------------------------------------
AmbaASM_ReadTlbBase0:
    CMP W0, #0x2
    B.GT 3f
    B.EQ 2f
1:  MRS X0, TTBR0_EL1
    RET
2:  MRS X0, TTBR0_EL2
    RET
3:  MRS X0, TTBR0_EL3
    RET

#-----------------------------------------------------------------------------
# Name:     AmbaASM_ReadTlbBase1
# Purpose:  Performs TTBR1_EL1 (Translation Table Base Register 1, EL1) read.
# Return:   X0
#-----------------------------------------------------------------------------
AmbaASM_ReadTlbBase1:
    MRS X0, TTBR1_EL1
    RET

#-----------------------------------------------------------------------------
# Name:     AmbaASM_ReadAuxMemAttrIndir
# Purpose:  Performs AMAIR_ELx (Auxiliary Memory Attribute Indir Register, ELx) read.
# Params:   W0
# Return:   X0
#-----------------------------------------------------------------------------
AmbaASM_ReadAuxMemAttrIndir:
    CMP W0, #0x2
    B.GT 3f
    B.EQ 2f
1:  MRS X0, AMAIR_EL1
    RET
2:  MRS X0, AMAIR_EL2
    RET
3:  MRS X0, AMAIR_EL3
    RET

#-----------------------------------------------------------------------------
# Name:     AmbaASM_ReadMemAttrIndir
# Purpose:  Performs MAIR_ELx (Memory Attribute Indir Register, ELx) read.
# Params:   W0
# Return:   X0
#-----------------------------------------------------------------------------
AmbaASM_ReadMemAttrIndir:
    CMP W0, #0x2
    B.GT 3f
    B.EQ 2f
1:  MRS X0, MAIR_EL1
    RET
2:  MRS X0, MAIR_EL2
    RET
3:  MRS X0, MAIR_EL3
    RET

#-----------------------------------------------------------------------------
# Name:     AmbaASM_ReadDomainAccessCtrl
# Purpose:  Performs DACR32_EL2 ( Domain Access Control Register, EL2) read.
# Return:   X0
#-----------------------------------------------------------------------------
AmbaASM_ReadDomainAccessCtrl:
    MRS X0, DACR32_EL2
    RET

#-----------------------------------------------------------------------------
# Name:     AmbaASM_ReadContextID
# Purpose:  Performs CONTEXTIDR_EL1 (Context ID Register, EL1) read.
# Return:   X0
#-----------------------------------------------------------------------------
AmbaASM_ReadContextID:
    MRS X0, CONTEXTIDR_EL1
    RET
