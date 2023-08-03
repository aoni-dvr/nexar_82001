/**
 *  @file AmbaASM_IntrinsicA32.asm
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
    .globl AmbaASM_ReadCpuAuxCtrl
    .globl AmbaASM_ReadCpuExtCtrl
    .globl AmbaASM_ReadCpuMemErr
# Identification instructions
    .globl AmbaASM_ReadMultiprocessorAffinity
# Instruction cache maintenance instructions
    .globl AmbaCache_BrPredInvAll
    .globl AmbaCache_BrPredInvAllIS
    .globl AmbaCache_BrPredInvMva
    .globl AmbaCache_InstInvAll
    .globl AmbaCache_InstInvAllIS
    .globl AmbaCache_InstInvMvaU
# Data cache maintenance instructions
    .globl AmbaCache_DataCleanInvSetWayA32
    .globl AmbaCache_DataCleanSetWayA32
    .globl AmbaCache_DataInvSetWayA32
    .globl AmbaCache_DataCleanInvMvaCA32
    .globl AmbaCache_DataCleanMvaCA32
    .globl AmbaCache_DataCleanMvaUA32
    .globl AmbaCache_DataInvMvaCA32
# Exception handling instructions
    .globl AmbaASM_WriteVectorBaseAddrA32
    .globl AmbaASM_WriteHypVectorBaseAddr
    .globl AmbaASM_WriteMonVectorBaseAddr
# Generic counter timer instructions
    .globl AmbaASM_ReadTimerFreq
    .globl AmbaASM_ReadPhysTimerCount
    .globl AmbaASM_WriteTimerFreq
    .globl AmbaASM_WritePhysTimerCtrl
    .globl AmbaASM_WritePhysTimerCompareVal
# Address translation instructions
    .globl AmbaAT_Stg12NonSecurePrivRead
    .globl AmbaAT_Stg12NonSecurePrivWrite
    .globl AmbaAT_Stg12NonSecureUnPrivRead
    .globl AmbaAT_Stg12NonSecureUnPrivWrite
    .globl AmbaAT_Stg1CurrentStatePrivRead
    .globl AmbaAT_Stg1CurrentStatePrivWrite
    .globl AmbaAT_Stg1CurStatUnPrivRead
    .globl AmbaAT_Stg1CurStatUnPrivWrite
    .globl AmbaAT_Stg1CurrentStateHyperRead
    .globl AmbaAT_Stg1CurrentStateHyperWrite
    .globl AmbaAT_WritePAR_32
    .globl AmbaAT_WritePAR_64
    .globl AmbaAT_ReadPAR_32
    .globl AmbaAT_ReadPAR_64
# TLB maintenance instructions
    .globl AmbaTLB_InvInstTlbAll
    .globl AmbaTLB_InvInstTlbMva
    .globl AmbaTLB_InvInstTlbAsid
    .globl AmbaTLB_InvDataTlbAll
    .globl AmbaTLB_InvDataTlbMva
    .globl AmbaTLB_InvDataTlbAsid
    .globl AmbaTLB_InvUniTlbAll
    .globl AmbaTLB_InvUniTlbAllIS
    .globl AmbaTLB_InvUniTlbAllHyp
    .globl AmbaTLB_InvUniTlbAllHypIS
    .globl AmbaTLB_InvUniTlbAllNoSeNoHyp
    .globl AmbaTLB_InvUniTlbAllNoSeNoHypIS
    .globl AmbaTLB_InvUniTlbAsid
    .globl AmbaTLB_InvUniTlbAsidIS
    .globl AmbaTLB_InvTlbIpaStg2A32
    .globl AmbaTLB_InvTlbIpaStg2ISA32
    .globl AmbaTLB_InvTlbIpaStg2LastLvA32
    .globl AmbaTLB_InvTlbIpaStg2LastLvISA32
    .globl AmbaTLB_InvUniTlbMva
    .globl AmbaTLB_InvUniTlbMvaIS
    .globl AmbaTLB_InvUniTlbMvaHyp
    .globl AmbaTLB_InvUniTlbMvaHypIS
    .globl AmbaTLB_InvUniTlbMvaLastLv
    .globl AmbaTLB_InvUniTlbMvaLastLvIS
    .globl AmbaTLB_InvUniTlbMvaLastLvHyp
    .globl AmbaTLB_InvUniTlbMvaLastLvHypIS
# System control instructions
    .globl AmbaASM_WriteAuxCtrlA32
    .globl AmbaASM_WriteAuxCtrl2
    .globl AmbaASM_WriteCoprocessorAccessCtrl
    .globl AmbaASM_ReadAuxCtrlA32
    .globl AmbaASM_ReadAuxCtrl2
    .globl AmbaASM_ReadCoprocessorAccessCtrl
# Virtual memory control instructions
    .globl AmbaASM_WriteSysCtrlA32
    .globl AmbaASM_WriteTlbBaseCtrlA32
    .globl AmbaASM_WriteTlbBase0_32
    .globl AmbaASM_WriteTlbBase1_32
    .globl AmbaASM_WriteTlbBase0_64
    .globl AmbaASM_WriteTlbBase1_64
    .globl AmbaASM_WriteAuxMemAttrIndir0
    .globl AmbaASM_WriteAuxMemAttrIndir1
    .globl AmbaASM_WriteMemAttrIndir0
    .globl AmbaASM_WriteMemAttrIndir1
    .globl AmbaASM_WriteDomainAccessCtrl
    .globl AmbaASM_WriteContextID
    .globl AmbaASM_ReadSysCtrlA32
    .globl AmbaASM_ReadTlbBaseCtrlA32
    .globl AmbaASM_ReadTlbBase0_32
    .globl AmbaASM_ReadTlbBase1_32
    .globl AmbaASM_ReadTlbBase0_64
    .globl AmbaASM_ReadTlbBase1_64
    .globl AmbaASM_ReadAuxMemAttrIndir0
    .globl AmbaASM_ReadAuxMemAttrIndir1
    .globl AmbaASM_ReadMemAttrIndir0
    .globl AmbaASM_ReadMemAttrIndir1
    .globl AmbaASM_ReadDomainAccessCtrl
    .globl AmbaASM_ReadContextID

#-----------------------------------------------------------------------------
# Name:     AMBA_DMB
# Purpose:  Perform 'Data Memory Barrier'
#-----------------------------------------------------------------------------
AMBA_DMB:
    DMB SY
    BX  LR

#-----------------------------------------------------------------------------
# Name:     AMBA_DSB
# Purpose:  Perform 'Data Synchronization Barrier'
#-----------------------------------------------------------------------------
AMBA_DSB:
    DSB SY
    BX  LR

#-----------------------------------------------------------------------------
# Name:     AMBA_ISB
# Purpose:  Perform 'Instruction Synchronization Barrier'
#-----------------------------------------------------------------------------
AMBA_ISB:
    ISB SY
    BX  LR

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
    BX  LR

#-----------------------------------------------------------------------------
# Name:     AMBA_SEV
# Purpose:  Perform 'Set Event'
#-----------------------------------------------------------------------------
AMBA_SEV:
    SEV
    BX  LR

#-----------------------------------------------------------------------------
# Name:     AMBA_WFE
# Purpose:  Perform 'Wait For Event'
#-----------------------------------------------------------------------------
AMBA_WFE:
    WFE
    BX  LR

#-----------------------------------------------------------------------------
# Name:     AMBA_WFI
# Purpose:  Perform 'Wait For Interrupt'
#-----------------------------------------------------------------------------
AMBA_WFI:
    WFI
    BX  LR

#-----------------------------------------------------------------------------
# Name:     AMBA_enable_interrupt
# Purpose:  enable interrupt.
#-----------------------------------------------------------------------------
AMBA_enable_interrupt:
    CPSIE IF
    BX  LR

#-----------------------------------------------------------------------------
# Name:     AMBA_disable_interrupt
# Purpose:  disable interrupt.
#-----------------------------------------------------------------------------
AMBA_disable_interrupt:
    CPSID IF
    BX  LR

#-----------------------------------------------------------------------------
# Name:     AMBA_enable_irq
# Purpose:  enable IRQ.
#-----------------------------------------------------------------------------
AMBA_enable_irq:
    CPSIE I
    BX  LR

#-----------------------------------------------------------------------------
# Name:     AMBA_disable_irq
# Purpose:  disable IRQ.
#-----------------------------------------------------------------------------
AMBA_disable_irq:
    CPSID I
    BX  LR

#-----------------------------------------------------------------------------
# Name:     AMBA_enable_fiq
# Purpose:  enable FIQ.
#-----------------------------------------------------------------------------
AMBA_enable_fiq:
    CPSIE F
    BX  LR

#-----------------------------------------------------------------------------
# Name:     AMBA_disable_fiq
# Purpose:  enable FIQ.
#-----------------------------------------------------------------------------
AMBA_disable_fiq:
    CPSID F
    BX  LR

#-----------------------------------------------------------------------------
# Name:     AMBA_get_interrupt_state
# Purpose:  Perform CPSR (The Current Program Status Register) read
# Return:   R0
#-----------------------------------------------------------------------------
AMBA_get_interrupt_state:
    MRS R0, CPSR
    BX  LR

#-----------------------------------------------------------------------------
# Name:     AMBA_set_interrupt_state
# Purpose:  Perform CPSR (The Current Program Status Register) write
#           Onyl set IF fields (bit [6:7]) in CPSR.
# Params:   R0
#-----------------------------------------------------------------------------
AMBA_set_interrupt_state:
    MRS R1, CPSR
    BIC R1, R1, #192
    AND R0, R0, #192
    ORR R1, R1, R0
    MSR cpsr_c, R1
    BX  LR

#-----------------------------------------------------------------------------
# Name:     AmbaASM_WriteCpuAuxCtrl
# Purpose:  Performs CPUACTLR (CPU Auxiliary Control Register) write.
# Params:
#    R0 = Low UINT32 part of UINT64
#    R1 = High UINT32 part of UINT64
#-----------------------------------------------------------------------------
AmbaASM_WriteCpuAuxCtrl:
  MCRR  15, 0, R0, R1, CR15
  BX    LR

#-----------------------------------------------------------------------------
# Name:     AmbaASM_ReadCpuAuxCtrl
# Purpose:  Performs CPUACTLR (CPU Auxiliary Control Register) read.
# Return:
#    R0 = Low UINT32 part of UINT64
#    R1 = High UINT32 part of UINT64
#-----------------------------------------------------------------------------
AmbaASM_ReadCpuAuxCtrl:
    MRRC  15, 0, R0, R1, CR15
    BX    LR

#-----------------------------------------------------------------------------
# Name:     AmbaASM_WriteCpuExtCtrl
# Purpose:  Performs CPUECTLR (CPU Extended Control Register) write.
# Params:
#    R0 = Low UINT32 part of UINT64
#    R1 = High UINT32 part of UINT64
#-----------------------------------------------------------------------------
AmbaASM_WriteCpuExtCtrl:
    MCRR    15, 1, R0, R1, CR15
    BX      LR

#-----------------------------------------------------------------------------
# Name:     AmbaASM_ReadCpuExtCtrl
# Purpose:  Performs CPUECTLR (CPU Extended Control Register) read.
# Return:
#    R0 = Low UINT32 part of UINT64
#    R1 = High UINT32 part of UINT64
#-----------------------------------------------------------------------------
AmbaASM_ReadCpuExtCtrl:
    MRRC  15, 1, R0, R1, CR15
    BX    LR

#-----------------------------------------------------------------------------
# Name:     AmbaASM_WriteCpuMemErr
# Purpose:  Performs CPUMERRSR (CPU Memory Error Syndrome Register) write.
# Params:
#    R0 = Low UINT32 part of UINT64
#    R1 = High UINT32 part of UINT64
#-----------------------------------------------------------------------------
AmbaASM_WriteCpuMemErr:
  MCRR  15, 2, R0, R1, CR15
  BX    LR

#-----------------------------------------------------------------------------
# Name:     AmbaASM_ReadCpuMemErr
# Purpose:  Performs CPUMERRSR (CPU Memory Error Syndrome Register) read.
# Return:
#    R0 = Low UINT32 part of UINT64
#    R1 = High UINT32 part of UINT64
#-----------------------------------------------------------------------------
AmbaASM_ReadCpuMemErr:
    MRRC  15, 2, R0, R1, CR15
    BX    LR

#-----------------------------------------------------------------------------
# Name:     AmbaASM_ReadMultiprocessorAffinity
# Purpose:  Performs MPIDR (Multiprocessor Affinity Register) read.
# Return:
#    R0
#-----------------------------------------------------------------------------
AmbaASM_ReadMultiprocessorAffinity:
    MRC   15, 0, R0, CR0, CR0, 5
    BX    LR

#-----------------------------------------------------------------------------
# Name:     AmbaCache_BrPredInvAll
# Purpose:  Performs BPIALL (Invalidate all entries from branch predictors) write.
# Return:   None
#-----------------------------------------------------------------------------
AmbaCache_BrPredInvAll:
    MCR   15, 0, R0, CR7, CR5, 6
    BX    LR

#-----------------------------------------------------------------------------
# Name:     AmbaCache_BrPredInvAllIS
# Purpose:  Performs BPIALLIS (Invalidate all entries from branch predictors Inner Shareable) write.
# Return:   None
#-----------------------------------------------------------------------------
AmbaCache_BrPredInvAllIS:
    MCR   15, 0, R0, CR7, CR1, 6
    BX    LR

#-----------------------------------------------------------------------------
# Name:     AmbaCache_BrPredInvMva
# Purpose:  Performs BPIMVA (Invalidate VA from branch predictors) write.
# Param:    R0: Virtual Address x
#-----------------------------------------------------------------------------
AmbaCache_BrPredInvMva:
    MCR   15, 0, R0, CR7, CR5, 7
    BX    LR

#-----------------------------------------------------------------------------
# Name:     AmbaCache_InstInvAll
# Purpose:  Performs ICIALLU (Invalidate all Instruction Caches to PoU) write.
# Return:   None
#-----------------------------------------------------------------------------
AmbaCache_InstInvAll:
    MCR   15, 0, R0, CR7, CR5, 0
    BX    LR

#-----------------------------------------------------------------------------
# Name:     AmbaCache_InstInvAllIS
# Purpose:  Performs ICIALLUIS (Invalidate all instruction caches Inner Shareable to PoU) write.
# Return:   None
#-----------------------------------------------------------------------------
AmbaCache_InstInvAllIS:
    MCR   15, 0, R0, CR7, CR1, 0
    BX    LR

#-----------------------------------------------------------------------------
# Name:     AmbaCache_InstInvMvaU
# Purpose:  Performs ICIMVAU (Invalidate Instruction Caches by VA to PoU) write.
# Param:    R0: Virtual Address x
#-----------------------------------------------------------------------------
AmbaCache_InstInvMvaU:
    MCR   15, 0, R0, CR7, CR5, 1
    BX    LR

#-----------------------------------------------------------------------------
# Name:     AmbaCache_DataCleanInvSetWayA32
# Purpose:  Performs DCCISW (Clean and invalidate data cache line by set/way) write.
# Param:    R0: SetWay & Level x
#-----------------------------------------------------------------------------
AmbaCache_DataCleanInvSetWayA32:
    MCR   15, 0, R0, CR7, CR14, 2
    BX    LR

#-----------------------------------------------------------------------------
# Name:     AmbaCache_DataCleanSetWayA32
# Purpose:  Performs DCCSW (Clean data cache line by set/way) write.
# Param:    R0: SetWay & Level x
#-----------------------------------------------------------------------------
AmbaCache_DataCleanSetWayA32:
    MCR   15, 0, R0, CR7, CR10, 2
    BX    LR

#-----------------------------------------------------------------------------
# Name:     AmbaCache_DataInvSetWayA32
# Purpose:  Performs DCISW (Invalidate data cache line by set/way) write.
# Param:    R0: Virtual Address x
#-----------------------------------------------------------------------------
AmbaCache_DataInvSetWayA32:
    MCR   15, 0, R0, CR7, CR6, 2
    BX    LR

#-----------------------------------------------------------------------------
# Name:     AmbaCache_DataCleanInvMvaCA32
# Purpose:  Performs DCCIMVAC (Clean and invalidate data cache line by VA to PoC) write.
# Param:    R0: Virtual Address x
#-----------------------------------------------------------------------------
AmbaCache_DataCleanInvMvaCA32:
    MCR   15, 0, R0, CR7, CR14, 1
    BX    LR

#-----------------------------------------------------------------------------
# Name:     AmbaCache_DataCleanMvaCA32
# Purpose:  Performs DCCMVAC (Clean data cache line by VA to PoC) write.
# Param:    R0: Virtual Address x
#-----------------------------------------------------------------------------
AmbaCache_DataCleanMvaCA32:
    mcr   15, 0, r0, cr7, cr10, 1
    bx    lr

#-----------------------------------------------------------------------------
# Name:     AmbaCache_DataCleanMvaUA32
# Purpose:  Performs DCCMVAU (Clean data cache line by VA to PoU) write.
# Param:    R0: Virtual Address x
#-----------------------------------------------------------------------------
AmbaCache_DataCleanMvaUA32:
    MCR   15, 0, R0, CR7, CR11, 1
    BX    LR

#-----------------------------------------------------------------------------
# Name:     AmbaCache_DataInvMvaCA32
# Purpose:  Performs DCIMVAC (Invalidate data cache line by VA to PoC) write.
# Param:    R0: Virtual Address x
#-----------------------------------------------------------------------------
AmbaCache_DataInvMvaCA32:
    MCR   15, 0, R0, CR7, CR6, 1
    BX    LR

#-----------------------------------------------------------------------------
# Name:     AmbaASM_WriteVectorBaseAddrA32
# Purpose:  Performs VBAR (Vector Base Address Register) write.
# Param:    R0: Vector Base Address x
#-----------------------------------------------------------------------------
AmbaASM_WriteVectorBaseAddrA32:
    MCR   15, 0, R0, CR12, CR0, 0
    BX    LR

#-----------------------------------------------------------------------------
# Name:     AmbaASM_WriteHypVectorBaseAddr
# Purpose:  Performs HVBAR (Hyper Vector Base Address Register) write.
# Param:    R0: Vector Base Address x
#-----------------------------------------------------------------------------
AmbaASM_WriteHypVectorBaseAddr:
    MCR   15, 4, R0, CR12, CR0, 0
    BX    LR

#-----------------------------------------------------------------------------
# Name:     AmbaASM_WriteMonVectorBaseAddr
# Purpose:  Performs MVBAR (Monitor Vector Base Address Register) write.
# Param:    R0: Vector Base Address x
#-----------------------------------------------------------------------------
AmbaASM_WriteMonVectorBaseAddr:
    MCR   15, 0, R0, CR12, CR0, 1
    BX    LR

#-----------------------------------------------------------------------------
# Name:     AmbaASM_ReadTimerFreq
# Purpose:  Performs CNTFRQ (Timer Counter Frequency Register) read.
# Return:   R0
#-----------------------------------------------------------------------------
AmbaASM_ReadTimerFreq:
    MRC 15, 0, R0, CR14, CR0, 0
    BX  LR

#-----------------------------------------------------------------------------
# Name:     AmbaASM_ReadPhysTimerCount
# Purpose:  Performs CNTPCT (Counter-timer Physical Count register) read.
# Return:
#    R0 = Low UINT32 part of UINT64
#    R1 = High UINT32 part of UINT64
#-----------------------------------------------------------------------------
AmbaASM_ReadPhysTimerCount:
    MRRC  15, 0, R0, R1, CR14
    BX    LR

#-----------------------------------------------------------------------------
# Name:     AmbaASM_WriteTimerFreq
# Purpose:  Performs CNTFRQ (Timer Counter Frequency Register) write.
# Param:    R0: TimerFrequency x
#-----------------------------------------------------------------------------
AmbaASM_WriteTimerFreq:
    MCR   15, 0, R0, CR14, CR0, 0
    BX    LR

#-----------------------------------------------------------------------------
# Name:     AmbaASM_WritePhysTimerCtrl
# Purpose:  Performs CNTP_CTL (Physical Timer Control register) write.
# Param:    R0:
#-----------------------------------------------------------------------------
AmbaASM_WritePhysTimerCtrl:
    MCR   15, 0, R0, CR14, CR2, 1
    BX    LR

#-----------------------------------------------------------------------------
# Name:     AmbaASM_WritePhysTimerCompareVal
# Purpose:  Performs CNTP_CVAL (Counter-timer Physical Timer CompareValue) write.
# Params:
#    R0 = Low UINT32 part of UINT64
#    R1 = High UINT32 part of UINT64
#-----------------------------------------------------------------------------
AmbaASM_WritePhysTimerCompareVal:
    MCRR   15, 2, R0, R1, CR14
    BX  LR

#-----------------------------------------------------------------------------
# Name:     AmbaAT_Stg12NonSecurePrivRead
# Purpose:  Performs ATS12NSOPR (Address Translate Stages 1 and 2 Non-secure Only PL1 Read) write
# Param:    R0 (Input address for translation)
#-----------------------------------------------------------------------------
AmbaAT_Stg12NonSecurePrivRead:
    MCR   15, 0, R0, CR7, CR8, 4
    BX    LR

#-----------------------------------------------------------------------------
# Name:     AmbaAT_Stg12NonSecurePrivWrite
# Purpose:  Performs ATS12NSOPW (Address Translate Stages 1 and 2 Non-secure Only PL1 Write) write.
# Param:    R0 (Input address for translation)
#-----------------------------------------------------------------------------
AmbaAT_Stg12NonSecurePrivWrite:
    MCR   15, 0, R0, CR7, CR8, 5
    BX    LR

#-----------------------------------------------------------------------------
# Name:     AmbaAT_Stg12NonSecureUnPrivRead
# Purpose:  Performs ATS12NSOUR (Address Translate Stages 1 and 2 Non-secure Only Unprivileged Read) write
# Param:    R0 (Input address for translation)
#-----------------------------------------------------------------------------
AmbaAT_Stg12NonSecureUnPrivRead:
    MCR   15, 0, R0, CR7, CR8, 6
    BX    LR

#-----------------------------------------------------------------------------
# Name:     AmbaAT_Stg12NonSecureUnPrivWrite
# Purpose:  Performs ATS12NSOUW (Address Translate Stages 1 and 2 Non-secure Only Unprivileged Write) write.
# Param:    R0 (Input address for translation)
#-----------------------------------------------------------------------------
AmbaAT_Stg12NonSecureUnPrivWrite:
    MCR   15, 0, R0, CR7, CR8, 7
    BX    LR

#-----------------------------------------------------------------------------
# Name:     AmbaAT_Stg1CurrentStatePrivRead
# Purpose:  Performs ATS1CPR (Address Translate Stage 1 Current state PL1 Read) write
# Param:    R0 (Input address for translation)
#-----------------------------------------------------------------------------
AmbaAT_Stg1CurrentStatePrivRead:
    MCR   15, 0, R0, CR7, CR8, 0
    BX    LR

#-----------------------------------------------------------------------------
# Name:     AmbaAT_Stg1CurrentStatePrivWrite
# Purpose:  Performs ATS1CPW (Address Translate Stage 1 Current state PL1 Write) write.
# Param:    R0 (Input address for translation)
#-----------------------------------------------------------------------------
AmbaAT_Stg1CurrentStatePrivWrite:
    MCR   15, 0, R0, CR7, CR8, 1
    BX    LR

#-----------------------------------------------------------------------------
# Name:     AmbaAT_Stg1CurStatUnPrivRead
# Purpose:  Performs ATS1CUR (Address Translate Stage 1 Current state Unprivileged Read) write
# Param:    R0 (Input address for translation)
#-----------------------------------------------------------------------------
AmbaAT_Stg1CurStatUnPrivRead:
    MCR   15, 0, R0, CR7, CR8, 2
    BX    LR

#-----------------------------------------------------------------------------
# Name:     AmbaAT_Stg1CurStatUnPrivWrite
# Purpose:  Performs ATS1CUW (Address Translate Stage 1 Current state Unprivileged Write) write.
# Param:    R0 (Input address for translation)
#-----------------------------------------------------------------------------
AmbaAT_Stg1CurStatUnPrivWrite:
    MCR   15, 0, R0, CR7, CR8, 3
    BX    LR

#-----------------------------------------------------------------------------
# Name:     AmbaAT_Stg1CurrentStateHyperRead
# Purpose:  Performs ATS1HR (Address Translate Stage 1 Hyp mode Read) read.
# Param:    R0 (Input address for translation)
#-----------------------------------------------------------------------------
AmbaAT_Stg1CurrentStateHyperRead:
    MCR   15, 4, R0, CR7, CR8, 0
    BX    LR

#-----------------------------------------------------------------------------
# Name:     AmbaAT_Stg1CurrentStateHyperWrite
# Purpose:  Performs ATS1HW write.
# Param:    R0 (Input address for translation)
#-----------------------------------------------------------------------------
AmbaAT_Stg1CurrentStateHyperWrite:
    MCR   15, 4, R0, CR7, CR8, 1
    BX    LR

#-----------------------------------------------------------------------------
# Name:     AmbaAT_WritePAR_32
# Purpose:  Performs PAR (Physical Address Register 32-bit) write.
# Param:    R0
#-----------------------------------------------------------------------------
AmbaAT_WritePAR_32:
    MCR   15, 0, R0, CR7, CR4, 0
    BX    LR

#-----------------------------------------------------------------------------
# Name:     AmbaAT_WritePAR_64
# Purpose:  Performs PAR(Physical Address Register 64-bit) write.
# Params:
#    R0 = Low UINT32 part of UINT64
#    R1 = High UINT32 part of UINT64
#-----------------------------------------------------------------------------
AmbaAT_WritePAR_64:
    MCRR  15, 0, R0, R1, CR7
    BX    LR

#-----------------------------------------------------------------------------
# Name:     AmbaAT_ReadPAR_32
# Purpose:  Performs PAR(Physical Address Register 32-bit) read.
# Param:    R0
#-----------------------------------------------------------------------------
AmbaAT_ReadPAR_32:
    MRC   15, 0, R0, CR7, CR4, 0
    BX    LR

#-----------------------------------------------------------------------------
# Name:     AmbaAT_ReadPAR_64
# Purpose:  Performs PAR(Physical Address Register 64-bit) read.
# Return:
#    R0 = Low UINT32 part of UINT64
#    R1 = High UINT32 part of UINT64
#-----------------------------------------------------------------------------
AmbaAT_ReadPAR_64:
    MRRC  15, 0, R0, R1, CR7
    BX    LR

#-----------------------------------------------------------------------------
# Name:     AmbaTLB_InvInstTlbAll
# Purpose:  Performs ITLBIALL (Instruction TLB Invalidate All) write.
# Param:    R0
#-----------------------------------------------------------------------------
AmbaTLB_InvInstTlbAll:
    MCR   15, 0, R0, CR8, CR5, 0
    BX    LR

#-----------------------------------------------------------------------------
# Name:     AmbaTLB_InvInstTlbMva
# Purpose:  Performs ITLBIMVA (Instruction TLB Invalidate by VA) write.
# Param:    R0
#-----------------------------------------------------------------------------
AmbaTLB_InvInstTlbMva:
    MCR   15, 0, R0, CR8, CR5, 1
    BX    LR

#-----------------------------------------------------------------------------
# Name:     AmbaTLB_InvInstTlbAsid
# Purpose:  Performs ITLBIASID (Instruction TLB Invalidate by ASID match) write.
# Param:    R0
#-----------------------------------------------------------------------------
AmbaTLB_InvInstTlbAsid:
    MCR   15, 0, R0, CR8, CR5, 2
    BX    LR

#-----------------------------------------------------------------------------
# Name:     AmbaTLB_InvDataTlbAll
# Purpose:  Performs DTLBIALL (Data TLB Invalidate All) write.
#-----------------------------------------------------------------------------
AmbaTLB_InvDataTlbAll:
    MCR   15, 0, R0, CR8, CR6, 0
    BX    LR

#-----------------------------------------------------------------------------
# Name:     AmbaTLB_InvDataTlbMva
# Purpose:  Performs DTLBIMVA (Data TLB Invalidate by VA) write.
# Param:    R0:
#-----------------------------------------------------------------------------
AmbaTLB_InvDataTlbMva:
    MCR   15, 0, R0, CR8, CR6, 1
    BX    LR

#-----------------------------------------------------------------------------
# Name:     AmbaTLB_InvDataTlbAsid
# Purpose:  Performs DTLBIASID (Data TLB Invalidate by ASID match) write.
# Param:    R0
#-----------------------------------------------------------------------------
AmbaTLB_InvDataTlbAsid:
    MCR   15, 0, R0, CR8, CR6, 2
    BX    LR

#-----------------------------------------------------------------------------
# Name:     AmbaTLB_InvUniTlbAll
# Purpose:  Performs TLBIALL (TLB Invalidate All) write.
# Param:    None
#-----------------------------------------------------------------------------
AmbaTLB_InvUniTlbAll:
    MCR   15, 0, R0, CR8, CR7, 0
    BX    LR

#-----------------------------------------------------------------------------
# Name:     AmbaTLB_InvUniTlbAllIS
# Purpose:  Performs TLBIALLIS (TLB Invalidate All, Inner Shareable) write.
# Param:    None
#-----------------------------------------------------------------------------
AmbaTLB_InvUniTlbAllIS:
    MCR   15, 0, R0, CR8, CR3, 0
    BX    LR

#-----------------------------------------------------------------------------
# Name:     AmbaTLB_InvUniTlbAllHyp
# Purpose:  Performs TLBIALLH (TLB Invalidate All, Hyp mode) write.
# Param:    None
#-----------------------------------------------------------------------------
AmbaTLB_InvUniTlbAllHyp:
    MCR   15, 4, R0, CR8, CR7, 0
    BX    LR

#-----------------------------------------------------------------------------
# Name:     AmbaTLB_InvUniTlbAllHypIS
# Purpose:  Performs TLBIALLHIS (TLB Invalidate All, Hyp mode, Inner Shareable) write.
# Param:    None
#-----------------------------------------------------------------------------
AmbaTLB_InvUniTlbAllHypIS:
    MCR   15, 4, R0, CR8, CR3, 0
    BX    LR

#-----------------------------------------------------------------------------
# Name:     AmbaTLB_InvUniTlbAllNoSeNoHyp
# Purpose:  Performs TLBIALLNSNH (TLB Invalidate All, Non-Secure Non-Hyp) write.
# Param:    None
#-----------------------------------------------------------------------------
AmbaTLB_InvUniTlbAllNoSeNoHyp:
    MCR   15, 4, R0, CR8, CR7, 4
    BX    LR

#-----------------------------------------------------------------------------
# Name:     AmbaTLB_InvUniTlbAllNoSeNoHypIS
# Purpose:  Performs TLBIALLNSNHIS (TLB Invalidate All, Non-Secure Non-Hyp, Inner Shareable) write.
# Param:    None
#-----------------------------------------------------------------------------
AmbaTLB_InvUniTlbAllNoSeNoHypIS:
    MCR   15, 4, R0, CR8, CR3, 4
    BX    LR

#-----------------------------------------------------------------------------
# Name:     AmbaTLB_InvUniTlbAsid
# Purpose:  Performs TLBIASID (TLB Invalidate by ASID match) write.
# Param:    R0
#-----------------------------------------------------------------------------
AmbaTLB_InvUniTlbAsid:
    MCR   15, 0, R0, CR8, CR7, 2
    BX    LR

#-----------------------------------------------------------------------------
# Name:     AmbaTLB_InvUniTlbAsidIS
# Purpose:  Performs TLBIASIDIS (TLB Invalidate by ASID match, Inner Shareable) write.
# Param:    R0
#-----------------------------------------------------------------------------
AmbaTLB_InvUniTlbAsidIS:
    MCR   15, 0, R0, CR8, CR3, 2
    BX    LR

#-----------------------------------------------------------------------------
# Name:     AmbaTLB_InvTlbIpaStg2A32
# Purpose:  Performs TLBIIPAS2 (TLB Invalidate by Intermediate Physical Address, Stage 2) write.
# Param:    R0
#-----------------------------------------------------------------------------
AmbaTLB_InvTlbIpaStg2A32:
    MCR   15, 4, R0, CR8, CR4, 1
    BX    LR

#-----------------------------------------------------------------------------
# Name:     AmbaTLB_InvTlbIpaStg2ISA32
# Purpose:  Performs TLBIIPAS2IS (TLB Invalidate by Intermediate Physical Address, Stage 2, Inner Shareable) write.
# Param:    R0
#-----------------------------------------------------------------------------
AmbaTLB_InvTlbIpaStg2ISA32:
    MCR   15, 4, R0, CR8, CR0, 1
    BX    LR

#-----------------------------------------------------------------------------
# Name:     AmbaTLB_InvTlbIpaStg2LastLvA32
# Purpose:  Performs TLBIIPAS2L (TLB Invalidate by Intermediate Physical Address, Stage 2, Last level) write.
# Param:    R0
#-----------------------------------------------------------------------------
AmbaTLB_InvTlbIpaStg2LastLvA32:
    MCR   15, 4, R0, CR8, CR4, 5
    BX    LR

#-----------------------------------------------------------------------------
# Name:     AmbaTLB_InvTlbIpaStg2LastLvISA32
# Purpose:  Performs TLBIIPAS2LIS (TLB Invalidate by Intermediate Physical Address, Stage 2, Last level, Inner Shareable) write.
# Param:    R0
#-----------------------------------------------------------------------------
AmbaTLB_InvTlbIpaStg2LastLvISA32:
    MCR   15, 4, R0, CR8, CR0, 5
    BX    LR

#-----------------------------------------------------------------------------
# Name:     AmbaTLB_InvUniTlbMva
# Purpose:  Performs TLBIMVA (TLB Invalidate by VA) write.
# Param:    R0
#-----------------------------------------------------------------------------
AmbaTLB_InvUniTlbMva:
    MCR   15, 4, R0, CR8, CR7, 1
    BX    LR

#-----------------------------------------------------------------------------
# Name:     AmbaTLB_InvUniTlbMvaIS
# Purpose:  Performs TLBIMVAIS (TLB Invalidate by VA, Inner Shareable) write.
# Param:    R0
#-----------------------------------------------------------------------------
AmbaTLB_InvUniTlbMvaIS:
    MCR   15, 0, R0, CR8, CR3, 1
    BX    LR

#-----------------------------------------------------------------------------
# Name:     AmbaTLB_InvUniTlbMvaHyp
# Purpose:  Performs TLBIMVAH (TLB Invalidate by VA, Hyp mode) write.
# Param:    R0
#-----------------------------------------------------------------------------
AmbaTLB_InvUniTlbMvaHyp:
    MCR   15, 4, R0, CR8, CR7, 1
    BX    LR

#-----------------------------------------------------------------------------
# Name:     AmbaTLB_InvUniTlbMvaHypIS
# Purpose:  Performs TLBIMVAHIS (TLB Invalidate by VA, Hyp mode, Inner Shareable) write.
# Param:    R0
#-----------------------------------------------------------------------------
AmbaTLB_InvUniTlbMvaHypIS:
    MCR   15, 4, R0, CR8, CR3, 1
    BX    LR

#-----------------------------------------------------------------------------
# Name:     AmbaTLB_InvUniTlbMvaLastLv
# Purpose:  Performs TLBIMVAL (TLB Invalidate by VA, Last level) write.
# Param:    R0
#-----------------------------------------------------------------------------
AmbaTLB_InvUniTlbMvaLastLv:
    MCR   15, 0, R0, CR8, CR7, 5
    BX    LR

#-----------------------------------------------------------------------------
# Name:     AmbaTLB_InvUniTlbMvaLastLvIS
# Purpose:  Performs TLBIMVALIS (TLB Invalidate by VA, Last level, Inner Shareable) write.
# Param:    R0
#-----------------------------------------------------------------------------
AmbaTLB_InvUniTlbMvaLastLvIS:
    MCR   15, 0, R0, CR8, CR3, 5
    BX    LR

#-----------------------------------------------------------------------------
# Name:     AmbaTLB_InvUniTlbMvaLastLvHyp
# Purpose:  Performs TLBIMVALH (TLB Invalidate by VA, Last level, Hyp mode) write.
# Param:    R0
#-----------------------------------------------------------------------------
AmbaTLB_InvUniTlbMvaLastLvHyp:
    MCR   15, 4, R0, CR8, CR7, 5
    BX    LR

#-----------------------------------------------------------------------------
# Name:     AmbaTLB_InvUniTlbMvaLastLvHypIS
# Purpose:  Performs TLBIMVALHIS ( TLB Invalidate by VA, Last level, Hyp mode, Inner Shareable) write.
# Param:    R0
#-----------------------------------------------------------------------------
AmbaTLB_InvUniTlbMvaLastLvHypIS:
    MCR   15, 4, R0, CR8, CR3, 5
    BX    LR

#-----------------------------------------------------------------------------
# Name:     AmbaASM_WriteAuxCtrlA32
# Purpose:  Performs ACTLR (Auxiliary Control Register) write.
# Param:    R0
#-----------------------------------------------------------------------------
AmbaASM_WriteAuxCtrlA32:
    MCR   15, 0, R0, CR1, CR0, 1
    BX    LR

#-----------------------------------------------------------------------------
# Name:     AmbaASM_WriteAuxCtrl2
# Purpose:  Performs ACTLR2 (Auxiliary Control Register 2) write.
# Param:    R0
#-----------------------------------------------------------------------------
AmbaASM_WriteAuxCtrl2:
    MCR   15, 0, R0, CR1, CR0, 3
    BX    LR

#-----------------------------------------------------------------------------
# Name:     AmbaASM_WriteCoprocessorAccessCtrl
# Purpose:  Performs CPACR (Architectural Feature Access Control Register) write.
# Param:    R0
#-----------------------------------------------------------------------------
AmbaASM_WriteCoprocessorAccessCtrl:
    MCR   15, 0, R0, CR1, CR0, 2
    BX    LR

#-----------------------------------------------------------------------------
# Name:     AmbaASM_ReadAuxCtrlA32
# Purpose:  Performs ACTLR (Auxiliary Control Register) read.
# Return:   R0
#-----------------------------------------------------------------------------
AmbaASM_ReadAuxCtrlA32:
    MRC   15, 0, R0, CR1, CR0, 1
    BX    LR

#-----------------------------------------------------------------------------
# Name:     AmbaASM_ReadAuxCtrl2
# Purpose:  Performs ACTLR2 (Auxiliary Control Register 2) read.
# Return:   R0
#-----------------------------------------------------------------------------
AmbaASM_ReadAuxCtrl2:
    MRC   15, 0, R0, CR1, CR0, 3
    BX    LR

#-----------------------------------------------------------------------------
# Name:     UINT32AmbaASM_ReadCoprocessorAccessCtrl
# Purpose:  Performs CPACR (Architectural Feature Access Control Register) read.
# Return:   R0
#-----------------------------------------------------------------------------
UINT32AmbaASM_ReadCoprocessorAccessCtrl:
    MRC   15, 0, R0, CR1, CR0, 2
    BX    LR

#-----------------------------------------------------------------------------
# Name:     AmbaASM_WriteSysCtrlA32
# Purpose:  Performs SCTLR (System Control Register) write.
# Param:    R0
#-----------------------------------------------------------------------------
AmbaASM_WriteSysCtrlA32:
    MCR   15, 0, R0, CR1, CR0, 0
    BX    LR

#-----------------------------------------------------------------------------
# Name:     AmbaASM_WriteTlbBaseCtrlA32
# Purpose:  Performs TTBCR (Translation Table Base Control Register) write.
# Param:    R0
#-----------------------------------------------------------------------------
AmbaASM_WriteTlbBaseCtrlA32:
    MCR   15, 0, R0, CR2, CR0, 2
    BX    LR

#-----------------------------------------------------------------------------
# Name:     AmbaASM_WriteTlbBase0_32
# Purpose:  Performs TTBR0 (Translation Table Base Register 0, 32-bit) write.
# Param:    R0
#-----------------------------------------------------------------------------
AmbaASM_WriteTlbBase0_32:
    MCR   15, 0, R0, CR2, CR0, 0
    BX    LR

#-----------------------------------------------------------------------------
# Name:     AmbaASM_WriteTlbBase1_32
# Purpose:  Performs TTBR1 (Translation Table Base Register 1, 32-bit) write.
# Param:    R0
#-----------------------------------------------------------------------------
AmbaASM_WriteTlbBase1_32:
    MCR   15, 0, R0, CR2, CR0, 1
    BX    LR

#-----------------------------------------------------------------------------
# Name:     AmbaASM_WriteTlbBase0_64
# Purpose:  Performs  TTBR0 ((Translation Table Base Register 0, 64-bit) write.
# Params:
#    R0 = Low UINT32 part of UINT64
#    R1 = High UINT32 part of UINT64
#-----------------------------------------------------------------------------
AmbaASM_WriteTlbBase0_64:
  MCRR  15, 0, R0, R1, CR2
  BX    LR

#-----------------------------------------------------------------------------
# Name:     AmbaASM_WriteTlbBase1_64
# Purpose:  Performs  TTBR1 (Translation Table Base Register 1, 64-bit) write.
# Params:
#    R0 = Low UINT32 part of UINT64
#    R1 = High UINT32 part of UINT64
#-----------------------------------------------------------------------------
AmbaASM_WriteTlbBase1_64:
  MCRR  15, 1, R0, R1, CR2
  BX    LR

#-----------------------------------------------------------------------------
# Name:     AmbaASM_WriteAuxMemAttrIndir0
# Purpose:  Performs AMAIR0 (Auxiliary Memory Attribute Indir Register 0) write.
# Param:    R0
#-----------------------------------------------------------------------------
AmbaASM_WriteAuxMemAttrIndir0:
    MCR   15, 0, R0, CR10, CR3, 0
    BX    LR

#-----------------------------------------------------------------------------
# Name:     AmbaASM_WriteAuxMemAttrIndir1
# Purpose:  Performs AMAIR1 (Auxiliary Memory Attribute Indir Register 1) write.
# Param:    R0
#-----------------------------------------------------------------------------
AmbaASM_WriteAuxMemAttrIndir1:
    MCR   15, 0, R0, CR10, CR3, 1
    BX    LR

#-----------------------------------------------------------------------------
# Name:     AmbaASM_WriteMemAttrIndir0
# Purpose:  Performs MAIR0 (Memory Attribute Indir Register 0) or PRRR write.
# Param:    R0
#-----------------------------------------------------------------------------
AmbaASM_WriteMemAttrIndir0:
    MCR   15, 0, R0, CR10, CR2, 0
    BX    LR

#-----------------------------------------------------------------------------
# Name:     AmbaASM_WriteMemAttrIndir1
# Purpose:  Performs MAIR1 (Memory Attribute Indir Register 1) or NMRR write.
# Param:    R0
#-----------------------------------------------------------------------------
AmbaASM_WriteMemAttrIndir1:
    MCR   15, 0, R0, CR10, CR2, 1
    BX    LR

#-----------------------------------------------------------------------------
# Name:     AmbaASM_WriteDomainAccessCtrl
# Purpose:  Performs DACR (Domain Access Control Register) write.
# Param:    R0
#-----------------------------------------------------------------------------
AmbaASM_WriteDomainAccessCtrl:
    MCR   15, 0, R0, CR3, CR0, 0
    BX    LR

#-----------------------------------------------------------------------------
# Name:     AmbaASM_WriteContextID
# Purpose:  Performs CONTEXTIDR (Context ID Register) write.
# Param:    R0
#-----------------------------------------------------------------------------
AmbaASM_WriteContextID:
    MCR   15, 0, R0, CR13, CR0, 1
    BX    LR

#-----------------------------------------------------------------------------
# Name:     AmbaASM_ReadSysCtrlA32
# Purpose:  Performs SCTLR (System Control Register) read.
# Return:   R0
#-----------------------------------------------------------------------------
AmbaASM_ReadSysCtrlA32:
    MRC   15, 0, R0, CR1, CR0, 0
    BX    LR

#-----------------------------------------------------------------------------
# Name:     AmbaASM_ReadTlbBaseCtrlA32
# Purpose:  Performs TTBCR (Translation Table Base Control Register) read.
# Return:   R0
#-----------------------------------------------------------------------------
AmbaASM_ReadTlbBaseCtrlA32:
    MRC   15, 0, R0, CR2, CR0, 2
    BX    LR

#-----------------------------------------------------------------------------
# Name:     AmbaASM_ReadTlbBase0_32
# Purpose:  Performs TTBR0 (Translation Table Base Register 0, 32-bit) read.
# Return:   R0
#-----------------------------------------------------------------------------
AmbaASM_ReadTlbBase0_32:
    MRC   15, 0, R0, CR2, CR0, 0
    BX    LR

#-----------------------------------------------------------------------------
# Name:     AmbaASM_ReadTlbBase1_32
# Purpose:  Performs TTBR1 (Translation Table Base Register 1, 32-bit) read.
# Return:   R0
#-----------------------------------------------------------------------------
AmbaASM_ReadTlbBase1_32:
    MRC   15, 0, R0, CR2, CR0, 1
    BX    LR

#-----------------------------------------------------------------------------
# Name:     AmbaASM_ReadTlbBase0_64
# Purpose:  Performs TTBR0 (Translation Table Base Register 0, 64-bit) read.
# Return:
#    R0 = Low UINT32 part of UINT64
#    R1 = High UINT32 part of UINT64
#-----------------------------------------------------------------------------
AmbaASM_ReadTlbBase0_64:
    MRRC  15, 0, R0, R1, CR2
    BX    LR

#-----------------------------------------------------------------------------
# Name:     AmbaASM_ReadTlbBase1_64
# Purpose:  Performs TTBR1 (Translation Table Base Register 1, 64-bit) read.
# Return:
#    R0 = Low UINT32 part of UINT64
#    R1 = High UINT32 part of UINT64
#-----------------------------------------------------------------------------
AmbaASM_ReadTlbBase1_64:
    MRRC  15, 1, R0, R1, CR2
    BX    LR

#-----------------------------------------------------------------------------
# Name:     AmbaASM_ReadAuxMemAttrIndir0
# Purpose:  Performs AMAIR0 (Auxiliary Memory Attribute Indir Register 0) read.
# Return:   R0
#-----------------------------------------------------------------------------
AmbaASM_ReadAuxMemAttrIndir0:
    MRC   15, 0, R0, CR10, CR3, 0
    BX    LR

#-----------------------------------------------------------------------------
# Name:     AmbaASM_ReadAuxMemAttrIndir1
# Purpose:  Performs AMAIR1 (Auxiliary Memory Attribute Indir Register 1) read.
# Return:   R0
#-----------------------------------------------------------------------------
AmbaASM_ReadAuxMemAttrIndir1:
    MRC   15, 0, R0, CR10, CR3, 1
    BX    LR

#-----------------------------------------------------------------------------
# Name:     AmbaASM_ReadMemAttrIndir0
# Purpose:  Performs MAIR0 (Memory Attribute Indir Register 0) or PRRR read.
# Return:   R0
#-----------------------------------------------------------------------------
AmbaASM_ReadMemAttrIndir0:
    MRC   15, 0, R0, CR10, CR2, 0
    BX    LR

#-----------------------------------------------------------------------------
# Name:     AmbaASM_ReadMemAttrIndir1
# Purpose:  Performs MAIR1 (Memory Attribute Indir Register 1) or NMRR read.
# Return:   R0
#-----------------------------------------------------------------------------
AmbaASM_ReadMemAttrIndir1:
    MRC   15, 0, R0, CR10, CR2, 1
    BX    LR

#-----------------------------------------------------------------------------
# Name:     AmbaASM_ReadDomainAccessCtrl
# Purpose:  Performs DACR (Domain Access Control Register) read.
# Return:   R0
#-----------------------------------------------------------------------------
AmbaASM_ReadDomainAccessCtrl:
    MRC   15, 0, R0, CR3, CR0, 1
    BX    LR

#-----------------------------------------------------------------------------
# Name:     AmbaASM_ReadContextID
# Purpose:  Performs CONTEXTIDR (Context ID Register) read.
# Return:   R0
#-----------------------------------------------------------------------------
AmbaASM_ReadContextID:
    MRC   15, 0, R0, CR13, CR0, 1
    BX    LR
