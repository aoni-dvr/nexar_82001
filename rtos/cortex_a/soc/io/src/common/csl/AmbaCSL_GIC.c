/**
 *  @file AmbaCSL_GIC.c
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
 *  @details CoreLink GIC-400 CSL driver
 *
 */

#include "AmbaTypes.h"
#include "AmbaDef.h"
#include "AmbaMisraFix.h"
#include "AmbaWrap.h"

#include "AmbaCSL_GIC.h"

/**
 *  AmbaCSL_GicSetIntGroup - Set the interrupt group
 *  @param[in] IntID Interrupt ID
 *  @param[in] IntGroup Interrupt Group
 */
void AmbaCSL_GicSetIntGroup(UINT32 IntID, UINT32 IntGroup)
{
    volatile UINT32 *pWorkUINT32 = &(pAmbaGIC_DistReg->IntGroup[IntID / 32U]);
    UINT32 BitMask = (UINT32)0x1U << (IntID & 0x1fU), TmpUINT32 = *pWorkUINT32;

    if (IntGroup == 0U) {
        /* Group 0 */
        TmpUINT32 = ClearBits(TmpUINT32, BitMask);
    } else {
        /* Group 1 */
        TmpUINT32 = SetBits(TmpUINT32, BitMask);
    }

    *pWorkUINT32 = TmpUINT32;
}

/**
 *  AmbaCSL_GicSetIntEnable - Enable the Interrupt
 *  @param[in] IntID Interrupt ID
 */
void AmbaCSL_GicSetIntEnable(UINT32 IntID)
{
    volatile UINT32 *pWorkUINT32 = &(pAmbaGIC_DistReg->IntSetEnable[IntID / 32U]);
    UINT32 BitMask = (UINT32)0x1U << (IntID & 0x1fU);

    /* write 1 to the bit: Enable the forwarding of the interrupt to any CPU interface */
    *pWorkUINT32 = BitMask;
}

/**
 *  AmbaCSL_GicClearIntEnable - Disable the Interrupt
 *  @param[in] IntID Interrupt ID
 */
void AmbaCSL_GicClearIntEnable(UINT32 IntID)
{
    volatile UINT32 *pWorkUINT32 = &(pAmbaGIC_DistReg->IntClearEnable[IntID / 32U]);
    UINT32 BitMask = (UINT32)0x1U << (IntID & 0x1fU);

    /* write 1 to the bit: Disable the forwarding of the interrupt to any CPU interface */
    *pWorkUINT32 = BitMask;
}

/**
 *  AmbaCSL_GicSetIntPending - Set the status of the corresponding peripheral interrupt to pending
 *  @param[in] IntID Interrupt ID
 */
void AmbaCSL_GicSetIntPending(UINT32 IntID)
{
    volatile UINT32 *pWorkUINT32 = &(pAmbaGIC_DistReg->IntSetPending[IntID / 32U]);
    UINT32 BitMask = (UINT32)0x1U << (IntID & 0x1fU);

    /* write 1 to the bit: Set the status of the corresponding peripheral interrupt to pending */
    *pWorkUINT32 = BitMask;
}

/**
 *  AmbaCSL_GicClearIntPending - Clear the pending status of the corresponding peripheral interrupt
 *  @param[in] IntID Interrupt ID
 */
void AmbaCSL_GicClearIntPending(UINT32 IntID)
{
    volatile UINT32 *pWorkUINT32 = &(pAmbaGIC_DistReg->IntClearPending[IntID / 32U]);
    UINT32 BitMask = (UINT32)0x1U << (IntID & 0x1fU);

    /* write 1 to the bit: Clear the status of the corresponding peripheral interrupt to pending */
    *pWorkUINT32 = BitMask;
}

/**
 *  AmbaCSL_GicSetIntActive - Set the status of the corresponding peripheral interrupt to active
 *  @param[in] IntID Interrupt ID
 */
void AmbaCSL_GicSetIntActive(UINT32 IntID)
{
    volatile UINT32 *pWorkUINT32 = &(pAmbaGIC_DistReg->IntSetActive[IntID / 32U]);
    UINT32 BitMask = (UINT32)0x1U << (IntID & 0x1fU);

    /* write 1 to the bit: Set the status of the corresponding peripheral interrupt to pending */
    *pWorkUINT32 = BitMask;
}

/**
 *  AmbaCSL_GicClearIntActive - Clear the active status of the corresponding peripheral interrupt
 *  @param[in] IntID Interrupt ID
 */
void AmbaCSL_GicClearIntActive(UINT32 IntID)
{
    volatile UINT32 *pWorkUINT32 = &(pAmbaGIC_DistReg->IntClearActive[IntID / 32U]);
    UINT32 BitMask = (UINT32)0x1U << (IntID & 0x1fU);

    /* write 1 to the bit: Clear the status of the corresponding peripheral interrupt to pending */
    *pWorkUINT32 = BitMask;
}

/**
 *  AmbaCSL_GicSetIntPriority - Set the priority of the corresponding interrupt
 *  @param[in] IntID Interrupt ID
 *  @param[in] Priority Priority of the interrupt
 */
void AmbaCSL_GicSetIntPriority(UINT32 IntID, UINT32 Priority)
{
    volatile UINT32 *pWorkUINT32 = &(pAmbaGIC_DistReg->IntPriority[IntID / 4U]);
    UINT32 TmpUINT32 = *pWorkUINT32;
    UINT32 TmpLeftShift = (IntID % 4U) << 3U;

    /* Set the priority of the corresponding interrupt */
    TmpUINT32 = ClearBits(TmpUINT32, (UINT32)0xffU << TmpLeftShift);
    *pWorkUINT32 = TmpUINT32 | ((Priority & 0xffU) << TmpLeftShift);
}

/**
 *  AmbaCSL_GicSetIntTargetCore - Set the Target Core of the corresponding interrupt
 *  @param[in] IntID Interrupt ID
 *  @param[in] CpuTargets CPU ID bit mask (0x00-0xff)
 */
void AmbaCSL_GicSetIntTargetCore(UINT32 IntID, UINT32 CpuTargets)
{
    volatile UINT32 *pWorkUINT32 = &(pAmbaGIC_DistReg->IntTarget[IntID / 4U]);
    UINT32 TmpUINT32 = *pWorkUINT32;
    UINT32 TmpLeftShift = (IntID % 4U) << 3U;

    /* Set the Target Core of the corresponding interrupt */
    TmpUINT32 = ClearBits(TmpUINT32, (UINT32)0xffU << TmpLeftShift);
    *pWorkUINT32 = TmpUINT32 | ((CpuTargets & 0xffU) << TmpLeftShift);
}

/**
 *  AmbaCSL_GicSetIntConfig - Configure the corresponding interrupt as edge-triggered or level-sensitive
 *  @param[in] IntID Interrupt ID
 *  @param[in] IntConfig Level-sensitive or Edge-triggered
 */
void AmbaCSL_GicSetIntConfig(UINT32 IntID, UINT32 TriggerType)
{
    volatile UINT32 *pWorkUINT32 = &(pAmbaGIC_DistReg->IntConfig[IntID / 16U]);
    UINT32 TmpUINT32 = *pWorkUINT32;
    UINT32 TmpLeftShift = (IntID % 16U) << 1U;

    TmpUINT32 = ClearBits(TmpUINT32, (UINT32)0x3U << TmpLeftShift);
    *pWorkUINT32 = TmpUINT32 | ((TriggerType << 1U) << TmpLeftShift);
}

/**
 *  AmbaCSL_GicSGI - Generate a Software Interrupt (Write Only register !!)
 *  @param[in] IntID SGI interrupt ID
 *  @param[in] IrqType IRQ or FIQ
 *  @param[in] SgiType CPU targets of SGI
 *  @param[in] CpuTargetList CPU target list
 */
void AmbaCSL_GicSGI(UINT32 IntID, UINT32 IrqType, UINT32 SgiType, UINT32 CpuTargetList)
{
    AMBA_GIC_DIST_SGI_REG_s WorkSgiRegValue = {0};
    UINT32 TmpUINT32;

    /*
     *  since Software Generated Interrupt Register (ICDSGIR) is a Write Only register,
     *  we can't use bit-operation !!
     */
    WorkSgiRegValue.SgiIntID = (UINT8)IntID;                       /* the Software Interrupt ID */
    if (IrqType == INT_TYPE_FIQ) {
        /* Group 0 interrupts are Secure interrupts. Group 0 interrupts could be either FIQs or IRQs. */
        WorkSgiRegValue.Attribute = 0U;                            /* 0 = for Secure only; 1 = for Non-secure only */
    } else {
        /* Group 1 interrupts are Non-secure interrupts. Group 1 interrupts are always IRQs. */
        WorkSgiRegValue.Attribute = 1U;                            /* 0 = for Secure only; 1 = for Non-secure only */
    }
    WorkSgiRegValue.CpuTargetListFilter = (UINT8)SgiType;          /* CPU Target List Filter */
    WorkSgiRegValue.CpuTargetList = (UINT8)CpuTargetList;          /* CPU Target List When TargetList Filter = 0b00 */

    if (AmbaWrap_memcpy(&TmpUINT32, &WorkSgiRegValue, 4U) == 0U ) {
        pAmbaGIC_DistReg->SGI = TmpUINT32;
    }
}
