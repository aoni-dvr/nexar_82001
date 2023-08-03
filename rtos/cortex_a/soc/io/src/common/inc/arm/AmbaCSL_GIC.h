/**
 *  @file AmbaCSL_GIC.h
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
 *  @details Definitions & Constants for CoreLink GIC-400 CSL APIs
 *
 */

#ifndef AMBA_CSL_GIC_H
#define AMBA_CSL_GIC_H

#ifndef AMBA_INT_DEF_H
#include "AmbaINT_Def.h"
#endif

#include "AmbaReg_GIC.h"

/* Each CPU interface can see up to 1020 interrupts. */
#define AMBA_MAX_NUM_GIC_INT                1020U

/* CortexA53: b00111 = the distributor provides 256 interrupts, 224 external interrupt lines.*/
#define AMBA_MAX_NUM_CA53_GIC_INT           256U

#define AMBA_MAX_NUM_CA53_GIC_GROUP         16U

/*
 * Inline Function Definitions
 */
static inline UINT32 AmbaCSL_GicGetNumINT(void)
{
    return (((UINT32)pAmbaGIC_DistReg->IntCtrlType.IntLinesNumber + 1U) * 32U);
}
static inline UINT32 AmbaCSL_GicGetNumCPU(void)
{
    return ((UINT32)pAmbaGIC_DistReg->IntCtrlType.CpuNumber + 1U);
}

static inline UINT32 AmbaCSL_GicGetIntGroup(UINT32 IntID)
{
    return ((pAmbaGIC_DistReg->IntGroup[(IntID >> 5U)] >> (IntID % 32U)) & 1U);
}
static inline UINT32 AmbaCSL_GicGetIntEnable(UINT32 IntID)
{
    return ((pAmbaGIC_DistReg->IntSetEnable[(IntID >> 5U)] >> (IntID % 32U)) & 1U);
}
static inline UINT32 AmbaCSL_GicGetIntPending(UINT32 IntID)
{
    return ((pAmbaGIC_DistReg->IntSetPending[(IntID >> 5U)] >> (IntID % 32U)) & 1U);
}
static inline UINT32 AmbaCSL_GicGetIntConfig(UINT32 IntID)
{
    return ((pAmbaGIC_DistReg->IntConfig[(IntID >> 4U)] >> (((IntID % 16U) << 1U) + 1U)) & 1U);
}
static inline UINT32 AmbaCSL_GicGetIntTarget(UINT32 IntID)
{
    return ((pAmbaGIC_DistReg->IntTarget[(IntID >> 2U)] >> ((IntID % 4U) << 3U)) & 0xffU);
}

/*
 * Defined in AmbaCSL_GIC.c
 */
void AmbaCSL_GicSetIntGroup(UINT32 IntID, UINT32 IntGroup);
void AmbaCSL_GicSetIntEnable(UINT32 IntID);
void AmbaCSL_GicClearIntEnable(UINT32 IntID);
void AmbaCSL_GicSetIntPending(UINT32 IntID);
void AmbaCSL_GicClearIntPending(UINT32 IntID);
void AmbaCSL_GicSetIntActive(UINT32 IntID);
void AmbaCSL_GicClearIntActive(UINT32 IntID);
void AmbaCSL_GicSetIntPriority(UINT32 IntID, UINT32 Priority);
void AmbaCSL_GicSetIntTargetCore(UINT32 IntID, UINT32 CpuTargets);
void AmbaCSL_GicSetIntConfig(UINT32 IntID, UINT32 TriggerType);
void AmbaCSL_GicSGI(UINT32 IntID, UINT32 IrqType, UINT32 SgiType, UINT32 CpuTargetList);

#endif /* AMBA_CSL_GIC_H */
