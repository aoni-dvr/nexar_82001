/**
 *  @file AmbaRTSL_DRAMC.c
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
 *  @details DRAM Controller/Arbiter RTSL APIs
 *
 */

#include "AmbaTypes.h"
#include "AmbaMisraFix.h"

#include "AmbaRTSL_GIC.h"
#include "AmbaRTSL_DRAMC.h"
#include "AmbaCSL_DRAMC.h"
#include "AmbaCSL_DDRC.h"

#if defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)
#include "AmbaCortexA76.h"
#else
#include "AmbaCortexA53.h"
#endif

#if defined(CONFIG_DEVICE_TREE_SUPPORT)
#include <AmbaIOUtility.h>
#endif

AMBA_DRAMC_REG_s *pAmbaDRAMC_Reg;
#if defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)
static ULONG AttPageSize = AMBA_DRAM_ATT_PAGE_SIZE;
#endif
static UINT32 AttPageOffset = 18U;
static UINT32 DramAttInfo[8192] __attribute__((section(".bss.noinit")));
static ULONG ClientBase[NUM_DRAM_CLIENT] = {0U};
static UINT32 AccessVitual = 0U;

/**
 *  DRAMC_IrqHandler - Interrupt handler for dram controller
 *  @param[in] IntID Interrupt ID
 *  @param[in] UserArg Optional argument of the interrupt handler
 */
#if !defined(CONFIG_QNX)
#pragma GCC push_options
#pragma GCC target("general-regs-only")
static void DRAMC_IrqHandler(UINT32 IntID, UINT32 UserArg)
{
    static UINT32 SegFault;
    static UINT32 FaultAddress;
    static UINT32 FaultClient;

    SegFault = pAmbaDRAMC_Reg->SegFault;
    FaultAddress = pAmbaDRAMC_Reg->FaultAddr;
    FaultClient = pAmbaDRAMC_Reg->FaultClient;

    (void)(&SegFault);
    (void)(&FaultAddress);
    (void)(&FaultClient);
    (void)(&IntID);
    (void)(&UserArg);
}
#pragma GCC pop_options
#endif

#if defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)
static void DramC_InitCV5(void)
{
    UINT32 Tmp = 0U;
    const ULONG DramSize[9] = {2UL, 3UL, 4UL, 6UL, 8UL, 12UL, 16UL, 24UL, 32UL}; /* Gbs*/

    // Granularity = 1KB
    pAmbaDRAMC_Reg->DramPriority[0] = 0x002d0069;
    pAmbaDRAMC_Reg->DramPriority[1] = 0x002d0069;
    pAmbaDRAMC_Reg->DramPriority[2] = 0x002d0007;
    pAmbaDRAMC_Reg->DramPriority[3] = 0x002d0049;

    pAmbaDRAMC_Reg->DramPriority[4] = 0x002d0049;
    pAmbaDRAMC_Reg->DramPriority[5] = 0x002d004a;
    pAmbaDRAMC_Reg->DramPriority[6] = 0x002d004a;
    pAmbaDRAMC_Reg->DramPriority[7] = 0x002d004a;

    pAmbaDRAMC_Reg->DramPriority[8] = 0x002d004a;
    pAmbaDRAMC_Reg->DramPriority[9] = 0x002d004a;
    pAmbaDRAMC_Reg->DramPriority[10] = 0x002d004a;
    pAmbaDRAMC_Reg->DramPriority[11] = 0x002d0009;

    pAmbaDRAMC_Reg->DramPriority[12] = 0x002d0009;
    pAmbaDRAMC_Reg->DramPriority[13] = 0x002d0009;
    pAmbaDRAMC_Reg->DramPriority[14] = 0x002d0009;
    pAmbaDRAMC_Reg->DramPriority[15] = 0x002d0003;

    pAmbaDRAMC_Reg->DramPriority[16] = 0x002d000b;
    pAmbaDRAMC_Reg->DramPriority[17] = 0x002d000b;
    pAmbaDRAMC_Reg->DramPriority[18] = 0x002d000b;
    pAmbaDRAMC_Reg->DramPriority[19] = 0x002d000b;

    pAmbaDRAMC_Reg->DramPriority[20] = 0x002d0007;
    pAmbaDRAMC_Reg->DramPriority[21] = 0x103e1108;
    pAmbaDRAMC_Reg->DramPriority[22] = 0x103e1108;
    pAmbaDRAMC_Reg->DramPriority[23] = 0x002d0057;

    pAmbaDRAMC_Reg->DramPriority[24] = 0x002d0005;
    pAmbaDRAMC_Reg->SemeWrDramPriority = 0x103f220f;
    pAmbaDRAMC_Reg->SemeRdDramPriority = 0x103f220f;

    pAmbaDRAMC_Reg->DramThrottleDln = 0x800;

    pAmbaDRAMC_Reg->Reserved3[3] = 0x103;
    pAmbaDRAMC_Reg->Reserved3[4] = 0x108;
    pAmbaDRAMC_Reg->DramThrottleEnet0 = 0x20703;
    pAmbaDRAMC_Reg->DramThrottleEnet1 = 0x20703;
    pAmbaDRAMC_Reg->DramThrottleUSB3 = 0x103;
    pAmbaDRAMC_Reg->DramThrottlePCIE = 0x103;
    pAmbaDRAMC_Reg->DramThrottleFDMA = 0x103;
    pAmbaDRAMC_Reg->DramThrottleSDAXI0 = 0x103;
    pAmbaDRAMC_Reg->Reserved4[5] = 0x170;
    pAmbaDRAMC_Reg->Reserved4[6] = 0x170;
    pAmbaDRAMC_Reg->Reserved4[7] = 0x130;

    pAmbaDRAMC_Reg->DramThrottleRW = 0x1400C20;
    pAmbaDRAMC_Reg->DramThrottleBANK = 0x1401030;
    pAmbaDRAMC_Reg->DramThrottleBG = 0x88;

    Tmp = AmbaCSL_DdrcGetDramSize(0);
    if (Tmp < 9U) {
        AttPageSize = (DramSize[Tmp] * 2UL);
        if (0x0U != pAmbaDDRC_Reg[0]->Config0.DuelDieEn) {
            AttPageSize = AttPageSize * 2U;
        }
        AttPageSize = ((AttPageSize << 27U) >> 14U);
        (void)AmbaRTSL_DramcGetHostNum(&Tmp);
        AttPageSize = AttPageSize * Tmp;
        if (AttPageSize > ((ULONG)1U << (ULONG)AttPageOffset)) {
            while (0x0U == (((ULONG)1U << (ULONG)AttPageOffset) & AttPageSize)) {
                AttPageOffset++;
            }
        }
    }
}
#endif

#if defined(CONFIG_SOC_CV2)
static void DramC_InitCV2(void)
{
    pAmbaDRAMC_Reg->DramThrottleDeadline = 0x200U;   /* DRAM_THROTTLE_DLN */

    pAmbaDRAMC_Reg->DramPriority[0] = 0x002a0036U;       /* AXI/Cortex cluster 0 */
    pAmbaDRAMC_Reg->DramPriority[1] = 0x002a0006U;       /* ARM_DMA0 */
    pAmbaDRAMC_Reg->DramPriority[2] = 0x002a0006U;       /* ARM_DMA1 */
    pAmbaDRAMC_Reg->DramPriority[3] = 0x002a0017U;       /* ENET */

    pAmbaDRAMC_Reg->DramPriority[4] = 0x002a0017U;       /* Flash DMA (FDMA) */
    pAmbaDRAMC_Reg->DramPriority[5] = 0x002a0006U;       /* CAN Controller 0 */
    pAmbaDRAMC_Reg->DramPriority[6] = 0x002a0006U;       /* CAN Controller 1 */
    pAmbaDRAMC_Reg->DramPriority[7] = 0x002a0000U;       /* GDMA */

    pAmbaDRAMC_Reg->DramPriority[8] = 0x002a0017U;       /* SDXC0 */
    pAmbaDRAMC_Reg->DramPriority[9] = 0x002a0017U;       /* SDXC1 */
    pAmbaDRAMC_Reg->DramPriority[10] = 0x002a0006U;      /* USB20 (device) */
    pAmbaDRAMC_Reg->DramPriority[11] = 0x002a0006U;      /* USB20 (host) */

    pAmbaDRAMC_Reg->DramPriority[12] = 0x002a0008U;      /* OrcME */
    pAmbaDRAMC_Reg->DramPriority[13] = 0x002a0008U;      /* OrcCode */
    pAmbaDRAMC_Reg->DramPriority[14] = 0x002a0004U;      /* OrcVP */
    pAmbaDRAMC_Reg->DramPriority[15] = 0x002a0004U;      /* OrcL2 Cache */

    pAmbaDRAMC_Reg->DramPriority[16] = 0x003b0005U;      /* SMEM */
    pAmbaDRAMC_Reg->DramPriority[17] = 0x002a0002U;      /* VMEM0 */
    pAmbaDRAMC_Reg->DramPriority[18] = 0x002a0002U;      /* FEX */
    pAmbaDRAMC_Reg->DramPriority[19] = 0x002a0002U;      /* BMEM */

    pAmbaDRAMC_Reg->SemeDramPriority = 0x003b0009U;      /* SMEM hi_priority */

    pAmbaDRAMC_Reg->DramThrottle[3] = 0x20703U;         /* DRAM_USAGE_TARGET ENET (r:1.17%, w:1.17%) */
    pAmbaDRAMC_Reg->DramThrottle[4] = 0x103U;           /* DRAM_USAGE_TARGET FDMA (1.17%) */
    pAmbaDRAMC_Reg->DramThrottle[8] = 0x103U;           /* DRAM_USAGE_TARGET SDXC0 (1.17%) */
    pAmbaDRAMC_Reg->DramThrottle[9] = 0x103U;           /* DRAM_USAGE_TARGET SDXC1 (1.17%) */
    pAmbaDRAMC_Reg->DramThrottle[16] = 0x1e0U;          /* DRAM_USAGE_TARGET SMEM (87.5%) */

    pAmbaDRAMC_Reg->RwThrottle = 0x30;      // rw throttle mandatory = 48
    pAmbaDRAMC_Reg->BankThrottle = 0x20;    // bank throttle mandatory = 32
}
#endif

#if defined(CONFIG_SOC_CV22)
static void DramC_InitCV22(void)
{
    pAmbaDRAMC_Reg->DramThrottleDeadline = 0x200U;  /* DRAM_THROTTLE_DLN */

    pAmbaDRAMC_Reg->DramPriority[0] = 0x002a0036U;   /* AXI/Cortex cluster 0 */
    pAmbaDRAMC_Reg->DramPriority[1] = 0x002a0006U;   /* ARM_DMA0 */
    pAmbaDRAMC_Reg->DramPriority[2] = 0x002a0006U;   /* ARM_DMA1 */
    pAmbaDRAMC_Reg->DramPriority[3] = 0x002a0017U;   /* ENET */

    pAmbaDRAMC_Reg->DramPriority[4] = 0x002a0017U;   /* Flash DMA (FDMA) */
    pAmbaDRAMC_Reg->DramPriority[5] = 0x002a0006U;   /* CAN Controller */
    pAmbaDRAMC_Reg->DramPriority[6] = 0x002a0000U;   /* GDMA */
    pAmbaDRAMC_Reg->DramPriority[7] = 0x002a0017U;   /* SDXC0 */

    pAmbaDRAMC_Reg->DramPriority[8] = 0x002a0017U;   /* SDXC1 */
    pAmbaDRAMC_Reg->DramPriority[9] = 0x002a0006U;   /* USB20 (device) */
    pAmbaDRAMC_Reg->DramPriority[10] = 0x002a0006U;  /* USB20 (host) */
    pAmbaDRAMC_Reg->DramPriority[11] = 0x002a0008U;  /* OrcME */

    pAmbaDRAMC_Reg->DramPriority[12] = 0x002a0008U;  /* OrcCode */
    pAmbaDRAMC_Reg->DramPriority[13] = 0x002a0004U;  /* OrcVP */
    pAmbaDRAMC_Reg->DramPriority[14] = 0x002a0004U;  /* OrcL2 Cache */
    pAmbaDRAMC_Reg->DramPriority[15] = 0x003b0005U;  /* SMEM */

    pAmbaDRAMC_Reg->DramPriority[16] = 0x002a0002U;  /* VMEM0 */

    pAmbaDRAMC_Reg->SemeDramPriority = 0x003b0009U;  /* SMEM hi_priority */

    pAmbaDRAMC_Reg->DramThrottle[3] = 0x20703U;     /* DRAM_USAGE_TARGET ENET (r:1.17%, w:1.17%) */
    pAmbaDRAMC_Reg->DramThrottle[4] = 0x103U;       /* DRAM_USAGE_TARGET FDMA (1.17%) */
    pAmbaDRAMC_Reg->DramThrottle[7] = 0x103U;       /* DRAM_USAGE_TARGET SDXC0 (1.17%) */
    pAmbaDRAMC_Reg->DramThrottle[8] = 0x103U;       /* DRAM_USAGE_TARGET SDXC1 (1.17%) */
    pAmbaDRAMC_Reg->DramThrottle[15] = 0x1e0U;      /* DRAM_USAGE_TARGET SMEM (87.5%) */

    pAmbaDRAMC_Reg->RwThrottle = 0x30U;
    pAmbaDRAMC_Reg->BankThrottle = 0x20U;
}
#endif

#if defined(CONFIG_SOC_CV25)
static void DramC_InitCV25(void)
{
    pAmbaDRAMC_Reg->DramThrottleDeadline = 0x200U;   /* DRAM_THROTTLE_DLN */

    pAmbaDRAMC_Reg->DramPriority[0] = 0x002a0036U;   /* AXI/Cortex cluster 0 */
    pAmbaDRAMC_Reg->DramPriority[1] = 0x002a0006U;   /* ARM_DMA0 */
    pAmbaDRAMC_Reg->DramPriority[2] = 0x002a0006U;   /* ARM_DMA1 */
    pAmbaDRAMC_Reg->DramPriority[3] = 0x002a0017U;   /* ENET */

    pAmbaDRAMC_Reg->DramPriority[4] = 0x002a0017U;   /* Flash DMA (FDMA) */
    pAmbaDRAMC_Reg->DramPriority[5] = 0x002a0006U;   /* CAN Controller */
    pAmbaDRAMC_Reg->DramPriority[6] = 0x002a0000U;   /* GDMA */

    pAmbaDRAMC_Reg->DramPriority[7] = 0x002a0017U;   /* SDXC0 */
    pAmbaDRAMC_Reg->DramPriority[8] = 0x002a0017U;   /* SDXC1 */
    pAmbaDRAMC_Reg->DramPriority[9] = 0x002a0017U;   /* SDXC2 */

    pAmbaDRAMC_Reg->DramPriority[10] = 0x002a0006U;  /* USB20 (device) */
    pAmbaDRAMC_Reg->DramPriority[11] = 0x002a0006U;  /* USB20 (host) */

    pAmbaDRAMC_Reg->DramPriority[12] = 0x002a0008U;  /* OrcME */
    pAmbaDRAMC_Reg->DramPriority[13] = 0x002a0008U;  /* OrcCode */
    pAmbaDRAMC_Reg->DramPriority[14] = 0x002a0004U;  /* OrcVP */
    pAmbaDRAMC_Reg->DramPriority[15] = 0x002a0004U;  /* OrcL2 Cache */
    pAmbaDRAMC_Reg->DramPriority[16] = 0x003b0005U;  /* SMEM */
    pAmbaDRAMC_Reg->DramPriority[17] = 0x002a0002U;  /* VMEM0 */

    pAmbaDRAMC_Reg->SemeDramPriority = 0x003b0009U;  /* SMEM hi_priority */

    pAmbaDRAMC_Reg->DramThrottle[3] = 0x20703U;      /* DRAM_USAGE_TARGET ENET (r:1.17%, w:1.17%) */
    pAmbaDRAMC_Reg->DramThrottle[4] = 0x103U;        /* DRAM_USAGE_TARGET FDMA (1.17%) */
    pAmbaDRAMC_Reg->DramThrottle[7] = 0x103U;        /* DRAM_USAGE_TARGET SDXC0 (1.17%) */
    pAmbaDRAMC_Reg->DramThrottle[8] = 0x103U;        /* DRAM_USAGE_TARGET SDXC1 (1.17%) */
    pAmbaDRAMC_Reg->DramThrottle[9] = 0x103U;        /* DRAM_USAGE_TARGET SDXC2 (1.17%) */
    pAmbaDRAMC_Reg->DramThrottle[16] = 0x1e0U;       /* DRAM_USAGE_TARGET SMEM (87.5%) */

    pAmbaDRAMC_Reg->RwThrottle = 0x30U;
    pAmbaDRAMC_Reg->BankThrottle = 0x20U;
}
#endif

#if defined(CONFIG_SOC_CV28)
static void DramC_InitCV28(void)
{
#if defined(AMBA_FWPROG) || !defined(CONFIG_XEN_SUPPORT)
    pAmbaDRAMC_Reg->DramThrottleDeadline = 0x200U;        /* DRAM_THROTTLE_DLN */

    pAmbaDRAMC_Reg->DramPriority[0] = 0x002a0036U;   /* AXI/Cortex cluster 0 */
    pAmbaDRAMC_Reg->DramPriority[1] = 0x002a0006U;   /* ARM_DMA0 */
    pAmbaDRAMC_Reg->DramPriority[2] = 0x002a0006U;   /* ARM_DMA1 */
    pAmbaDRAMC_Reg->DramPriority[3] = 0x002a0017U;   /* ENET */

    pAmbaDRAMC_Reg->DramPriority[4] = 0x002a0017U;   /* Flash DMA (FDMA) */
    pAmbaDRAMC_Reg->DramPriority[5] = 0x002a0006U;   /* CAN Controller */
    pAmbaDRAMC_Reg->DramPriority[6] = 0x002a0000U;   /* GDMA */

    pAmbaDRAMC_Reg->DramPriority[7] = 0x002a0017U;   /* SDXC0 */
    pAmbaDRAMC_Reg->DramPriority[8] = 0x002a0017U;   /* SDXC1 */
    pAmbaDRAMC_Reg->DramPriority[9] = 0x002a0017U;   /* SDXC2 */

    pAmbaDRAMC_Reg->DramPriority[10] = 0x002a0006U;  /* USB20 (device) */
    pAmbaDRAMC_Reg->DramPriority[11] = 0x002a0006U;  /* USB20 (host) */

    pAmbaDRAMC_Reg->DramPriority[12] = 0x002a0008U;  /* OrcME */
    pAmbaDRAMC_Reg->DramPriority[13] = 0x002a0008U;  /* OrcCode */
    pAmbaDRAMC_Reg->DramPriority[14] = 0x002a0004U;  /* OrcVP */
    pAmbaDRAMC_Reg->DramPriority[15] = 0x002a0004U;  /* OrcL2 Cache */
    pAmbaDRAMC_Reg->DramPriority[16] = 0x003b0005U;  /* SMEM */
    pAmbaDRAMC_Reg->DramPriority[17] = 0x002a0002U;  /* VMEM0 */

    pAmbaDRAMC_Reg->SemeDramPriority = 0x003b0009U;  /* SMEM hi_priority */

    pAmbaDRAMC_Reg->DramThrottle[3] = 0x20703U;      /* DRAM_USAGE_TARGET ENET (r:1.17%, w:1.17%) */
    pAmbaDRAMC_Reg->DramThrottle[4] = 0x103U;        /* DRAM_USAGE_TARGET FDMA (1.17%) */
    pAmbaDRAMC_Reg->DramThrottle[7] = 0x103U;        /* DRAM_USAGE_TARGET SDXC0 (1.17%) */
    pAmbaDRAMC_Reg->DramThrottle[8] = 0x103U;        /* DRAM_USAGE_TARGET SDXC1 (1.17%) */
    pAmbaDRAMC_Reg->DramThrottle[9] = 0x103U;        /* DRAM_USAGE_TARGET SDXC2 (1.17%) */
    pAmbaDRAMC_Reg->DramThrottle[16] = 0x1e0U;       /* DRAM_USAGE_TARGET SMEM (87.5%) */

    pAmbaDRAMC_Reg->RwThrottle = 0x30U;
    pAmbaDRAMC_Reg->BankThrottle = 0x20U;
#endif // CONFIG_XEN_SUPPORT
}
#endif

#if defined(CONFIG_SOC_H32)
static void DramC_InitH32(void)
{
    pAmbaDRAMC_Reg->DramThrottleDeadline = 0x200U;        /* DRAM_THROTTLE_DLN */

    pAmbaDRAMC_Reg->DramPriority[0] = 0x00280035U;         /* AXI/Cortex cluster 0 */
    pAmbaDRAMC_Reg->DramPriority[1] = 0x00280005U;         /* ARM_DMA0 */
    pAmbaDRAMC_Reg->DramPriority[2] = 0x00280005U;         /* ARM_DMA1 */
    pAmbaDRAMC_Reg->DramPriority[3] = 0x00280016U;         /* ENET */
    pAmbaDRAMC_Reg->DramPriority[4] = 0x00280016U;         /* Flash DMA (FDMA) */
    pAmbaDRAMC_Reg->DramPriority[5] = 0x00280000U;         /* GDMA */
    pAmbaDRAMC_Reg->DramPriority[6] = 0x00280016U;         /* SDXC0 */
    pAmbaDRAMC_Reg->DramPriority[7] = 0x00280016U;         /* SDXC1 */
    pAmbaDRAMC_Reg->DramPriority[8] = 0x00280016U;         /* SDXC2 */
    pAmbaDRAMC_Reg->DramPriority[9] = 0x00280005U;         /* USB20 (device) */
    pAmbaDRAMC_Reg->DramPriority[10] = 0x00280005U;        /* USB20 (host) */
    pAmbaDRAMC_Reg->DramPriority[11] = 0x00290006U;        /* OrcME */
    pAmbaDRAMC_Reg->DramPriority[12] = 0x00290006U;        /* OrcCode */
    pAmbaDRAMC_Reg->DramPriority[13] = 0x00390004U;        /* SMEM */

    pAmbaDRAMC_Reg->SemeDramPriority = 0x003a0007;        /* SMEM hi_priority */

    pAmbaDRAMC_Reg->DramThrottle[3] = 0x20703U;      /* DRAM_USAGE_TARGET ENET (r:1.17%, w:1.17%) */
    pAmbaDRAMC_Reg->DramThrottle[4] = 0x103U;        /* DRAM_USAGE_TARGET FDMA (1.17%) */
    pAmbaDRAMC_Reg->DramThrottle[6] = 0x103U;        /* DRAM_USAGE_TARGET SDXC0 (1.17%) */
    pAmbaDRAMC_Reg->DramThrottle[7] = 0x103U;        /* DRAM_USAGE_TARGET SDXC1 (1.17%) */
    pAmbaDRAMC_Reg->DramThrottle[8] = 0x103U;        /* DRAM_USAGE_TARGET SDXC2 (1.17%) */
    pAmbaDRAMC_Reg->DramThrottle[13] = 0x1e0U;       /* DRAM_USAGE_TARGET SMEM (87.5%) */

    pAmbaDRAMC_Reg->RwThrottle = 0x30U;
    pAmbaDRAMC_Reg->BankThrottle = 0x20U;
}
#endif

/**
 *  AmbaRTSL_DramcInit - DRAM controller initializations
 */
void AmbaRTSL_DramcInit(void)
{
#if !defined(CONFIG_QNX)
    ULONG base_addr;
    INT32 offset = 0;
    (void) offset;

#if defined(CONFIG_SOC_CV2FS)
    base_addr = AMBA_CA53_DRAM_CTRL_BASE_ADDR;
#elif defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)
    base_addr = AMBA_CORTEX_A76_DRAM_CTRL_BASE_ADDR;
#else
    base_addr = AMBA_CORTEX_A53_DRAM_CTRL_BASE_ADDR;
#endif
    AmbaMisra_TypeCast(&pAmbaDRAMC_Reg, &base_addr);

#if defined(CONFIG_DEVICE_TREE_SUPPORT)
    base_addr = IO_UtilityFDTPropertyU32Quick(offset, "ambarella,dram", "reg", 0U);
#if defined (CONFIG_SOC_CV5)|| defined (CONFIG_SOC_CV52)
    base_addr = base_addr | AMBA_CORTEX_A76_DRAM_CONFIG_PHYS_BASE_ADDR;
#endif
    if ( base_addr != 0U ) {
        AmbaMisra_TypeCast(&pAmbaDRAMC_Reg, &base_addr);
    }
#endif

#endif

#if defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)
    DramC_InitCV5();
#endif

#if defined(CONFIG_SOC_CV2)
    DramC_InitCV2();
#endif

#if defined(CONFIG_SOC_CV22)
    DramC_InitCV22();
#endif

#if defined(CONFIG_SOC_CV25)
    DramC_InitCV25();
#endif

#if defined(CONFIG_SOC_CV28)
    DramC_InitCV28();
#endif

#if defined(CONFIG_SOC_H32)
    DramC_InitH32();
#endif
}

/**
 *  AmbaRTSL_DramcIrqInit - DRAM interrupt initializations
 */
void AmbaRTSL_DramcIrqInit(void)
{
#if !defined(CONFIG_QNX)
    AMBA_INT_CONFIG_s IntConfig;
    UINT32 IntID;

    /* Hook interrupt handler */
    IntConfig.TriggerType  = INT_TRIG_HIGH_LEVEL;
    IntConfig.IrqType      = INT_TYPE_FIQ;      /* Since Linux does not suuport GIC group1 interrupt handling, set it as FIQ for AmbaLink. */
    IntConfig.CpuTargets   = 0x01U;             /* Target cores */

#if defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)
    IntID = AMBA_INT_SPI_ID136_DRAM_ERROR;
#else
    IntID = AMBA_INT_SPI_ID137_DRAM_ERROR;
#endif
    (void)AmbaRTSL_GicIntConfig(IntID, &IntConfig, DRAMC_IrqHandler, 0U);
    (void)AmbaRTSL_GicIntEnable(IntID);
#endif
}

/**
 *  AmbaRTSL_DramInitAtt - DRAM controller setup all ATT default value
 *  [TODO] Per AUTOSAR requirement, the default config shall be passed as argument.
 *         That means, AmbaRTSL_DramSetupAtt and AmbaRTSL_DramEnableVirtClient shall be combined into Init function in the future.
 *         The Init function could be called AmbaMemProt_Init which is implemented by above two funcions and MMU api.
 *         The AmbaRTSL_DramInitAtt could be gone if AmbaMemProt_Init is implemented.
 */
void AmbaRTSL_DramInitAtt(void)
{
    UINT32 i, Page;

    /* setup default values into ATT */
    for(Page = 0U; Page < (AMBA_DRAM_ATT_LAST_PAGE_ID + 1U); Page += 2U) {
        /* Each hardware register contains even and odd pages mapping */
        pAmbaDRAMC_Reg->Att[Page >> 1U] = (Page) | ((Page + 1U) << AMBA_DRAM_ATT_REG_OFFSET);
        DramAttInfo[Page >> 1U] = (Page) | ((Page + 1U) << AMBA_DRAM_ATT_REG_OFFSET);
    }

    /* setup client attribute, default disable all client's vitaul attribute */
    pAmbaDRAMC_Reg->AccessVitual = 0x0U;

    /* setup client access permission, default set all client could access all entries */
    for(i = 0U; i < NUM_DRAM_CLIENT; i++) {
        pAmbaDRAMC_Reg->VpnBase[i] = 0U;
        pAmbaDRAMC_Reg->VpnBound[i] = AMBA_DRAM_ATT_LAST_PAGE_ID;
    }
}

/**
 *  AmbaRTSL_DramSetupAtt - DRAM controller setup page index to specified ATT entry
 *  @param[in] NumMemRegion number of memory regions
 *  @param[in] pMemRegion pointer to a list of memory region maps
 *  @return error code
 */
UINT32 AmbaRTSL_DramSetupAtt(UINT32 NumMemRegion, const AMBA_DRAM_ATT_INFO_s *pMemRegion)
{
    UINT32 RetVal = DRAMC_ERR_NONE;

#if defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)
    UINT32 VirtPage, PhysPage, NumPage;
    UINT32 PPN[2], RegVal = 0U;
    UINT32 i;

    if ((NumMemRegion == 0U) || (pMemRegion == NULL)) {
        RetVal = DRAMC_ERR_ARG;
    } else {
        for (i = 0U; i < NumMemRegion; i++) {
            VirtPage = (UINT32)(pMemRegion[i].VirtAddr / (ULONG)AttPageSize);
            PhysPage = (UINT32)(pMemRegion[i].PhysAddr / (ULONG)AttPageSize);
            NumPage  = (UINT32)(pMemRegion[i].Size / (ULONG)AttPageSize);
            while (NumPage > 0UL) {
                RegVal = pAmbaDRAMC_Reg->Att[VirtPage >> 1U];
                if (0x0U != (VirtPage & 0x1U)) {
                    PPN[0] = RegVal & 0x3FFFU;
                    PPN[1] = PhysPage & 0x3FFFU;
                } else {
                    PPN[0] = PhysPage & 0x3FFFU;
                    PPN[1] = (RegVal >> AMBA_DRAM_ATT_REG_OFFSET) & 0x3FFFU;
                }
                RegVal = PPN[0] | (PPN[1] << AMBA_DRAM_ATT_REG_OFFSET);
                pAmbaDRAMC_Reg->Att[VirtPage >> 1U] = RegVal;
                DramAttInfo[VirtPage >> 1U] = RegVal;
                VirtPage++;
                PhysPage++;
                NumPage--;
            }
        }
    }
#else
    (void) NumMemRegion;
    (void) pMemRegion;
#endif
    return RetVal;
}

/**
 *  AmbaRTSL_DramQueryAttV2P - It is used to get the translated physical address of specified virtual address
 *  @param[in] VirtAddr the virtual address
 *  @param[in] DramClient the client id of the virtual address
 *  @param[out] pPhysAddr the translated physical address
 *  @return error code
 */
UINT32 AmbaRTSL_DramQueryAttV2P(ULONG VirtAddr, UINT32 DramClient, ULONG *pPhysAddr)
{
    UINT32 VirtPage;
    ULONG PhysPage;
    UINT32 RegVal, RetVal = DRAMC_ERR_NONE;

    if ((pPhysAddr == NULL) || (DramClient >= AMBA_NUM_DRAM_CLIENT)) {
        RetVal = DRAMC_ERR_ARG;
    } else if (((AccessVitual) & ((UINT32)1U << DramClient)) == 0x0U) {
        /* This client is not set as virtual client */
        *pPhysAddr = VirtAddr;
        RetVal = DRAMC_ERR_NONE;
    }  else {
        VirtPage = (UINT32)((VirtAddr + ClientBase[DramClient]) >> (ULONG)AttPageOffset);
        /* Search from hardware register */
        RegVal = DramAttInfo[VirtPage >> 1U];
        if (0x0U != (VirtPage & 0x1U)) {
            PhysPage = ((ULONG)RegVal >> (ULONG)AMBA_DRAM_ATT_REG_OFFSET) & 0x3FFFUL;
        } else {
            PhysPage = (ULONG)RegVal & 0x3FFFUL;
        }

        *pPhysAddr = (PhysPage << (ULONG)AttPageOffset) | (VirtAddr & (((ULONG)1U << (ULONG)AttPageOffset) - (ULONG)1U));
    }

    return RetVal;
}

/**
 *  AmbaRTSL_DramQueryAttP2V - It is used to get the translated virtual address of specified physical address
 *  @param[in] PhysAddr the physical address
 *  @param[in] ClientId Client ID
 *  @param[out] pVirtAddr the translated virtual address
 */
UINT32 AmbaRTSL_DramQueryAttP2V(ULONG PhysAddr, UINT32 ClientId, ULONG *pVirtAddr)
{
    UINT32 Min, Max, PhysPage = (UINT32)(PhysAddr >> (ULONG)AttPageOffset);
    UINT32 i, RegVal, RetVal;

    if ((pVirtAddr == NULL) || (PhysAddr == 0x0U) || (ClientId >= NUM_DRAM_CLIENT)) {
        RetVal = DRAMC_ERR_ARG;
    } else if (((AccessVitual) & ((UINT32)1U << ClientId)) == 0x0U) {
        /* This client is not set as virtual client, phyical == virtual */
        *pVirtAddr = PhysAddr;
        RetVal = DRAMC_ERR_NONE;
    } else {
        Min = pAmbaDRAMC_Reg->VpnBase[ClientId];
        Max = pAmbaDRAMC_Reg->VpnBound[ClientId];
        if (Min > Max) {
            RetVal = DRAMC_ERR_UNEXPECTED;
        } else {
            /* Search from hardware register */
            RetVal = DRAMC_ERR_AT;

            /* check even entry */
            for (i = ((Min + 1U) & ~0x1U); i <= Max; i += 2U) {
                RegVal = (DramAttInfo[i >> 1U] & 0x3fffU);
                if (PhysPage == RegVal) {
                    *pVirtAddr = (((ULONG)i - (ULONG)Min) << AttPageOffset) | (PhysAddr & (((ULONG)1U << (ULONG)AttPageOffset) - (ULONG)1U));
                    RetVal = DRAMC_ERR_NONE;
                    break;
                }
            }

            /* check odd entry */
            if (RetVal != DRAMC_ERR_NONE) {
                for (i = (Min | 0x1U); i <= Max; i += 2U) {
                    RegVal = DramAttInfo[i >> 1U];
                    RegVal = ((RegVal >> AMBA_DRAM_ATT_REG_OFFSET) & 0x3FFFU);
                    if (PhysPage == RegVal) {
                        *pVirtAddr = (((ULONG)i - (ULONG)Min) << AttPageOffset) | (PhysAddr & (((ULONG)1U << (ULONG)AttPageOffset) - (ULONG)1U));
                        RetVal = DRAMC_ERR_NONE;
                        break;
                    }
                }
            }
        }
    }

    return RetVal;
}

/**
 *  AmbaRTSL_DramConfigAttSecure - It is used to mark a part of ATT as secure region. The remaining parts of ATT are all treated as non-secure regions.
 *  @param[in] VirtAddr virtual base address of secure region
 *  @param[in] Size number of bytes reserved for secure region
 *  @return error code
 */
UINT32 AmbaRTSL_DramConfigAttSecure(ULONG VirtAddr, ULONG Size)
{
    UINT32 StartPage = (UINT32)(VirtAddr >> AttPageOffset);
    UINT32 NumPage = (UINT32)Size >> AttPageOffset;
    UINT32 RetVal = DRAMC_ERR_NONE;

    if ((StartPage + NumPage) > 0x4000U) {
        RetVal = DRAMC_ERR_ARG;
    } else {
        pAmbaDRAMC_Reg->AttSecureBase = StartPage;
        pAmbaDRAMC_Reg->AttSecureLimit = StartPage + NumPage - 1U;
    }

    return RetVal;
}

/**
 *  AmbaRTSL_DramEnableVirtClient - DRAM controller setup VPN segment for specified client and enable its virtual attribute
 *  @param[in] ClientId DRAM client ID
 *  @param[in] VirtAddr virtual base address of the specified dram client
 *  @param[in] Size number of bytes reserved for the specified dram client
 *  @return error code
 */
UINT32 AmbaRTSL_DramEnableVirtClient(UINT32 ClientId, ULONG VirtAddr, ULONG Size)
{
    UINT32 RetVal = DRAMC_ERR_NONE;
#if defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)
    UINT32 StartPage = (UINT32)(VirtAddr >> AttPageOffset);
    UINT32 NumPage = (UINT32)(Size >> AttPageOffset);

    if ((ClientId >= NUM_DRAM_CLIENT) || ((StartPage + NumPage) > 0x4000U) || ((0x0U == (StartPage + NumPage))) || (0x0U == NumPage)) {
        RetVal = DRAMC_ERR_ARG;
    } else {
        pAmbaDRAMC_Reg->VpnBase[ClientId] = StartPage;
        pAmbaDRAMC_Reg->VpnBound[ClientId] = StartPage + NumPage - 1U;
        pAmbaDRAMC_Reg->AccessVitual = (pAmbaDRAMC_Reg->AccessVitual) | ((UINT32)1U << ClientId);
        AccessVitual = (pAmbaDRAMC_Reg->AccessVitual);
#if defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)
        pAmbaDRAMC_Reg->DramClientOffset[ClientId - 2U] = (UINT32)((VirtAddr) >> 16U);
        ClientBase[ClientId] = VirtAddr;
#endif
    }
#else
    (void)ClientId;
    (void)VirtAddr;
    (void)Size;
#endif
    return RetVal;
}

/**
 *  AmbaRTSL_DramDisableVirtClient - DRAM controller disable its vitual attribute
 *  @param[in] ClientId DRAM client ID
 *  @return error code
 */
UINT32 AmbaRTSL_DramDisableVirtClient(UINT32 ClientId)
{
    UINT32 RetVal = DRAMC_ERR_NONE;

    if (ClientId >= NUM_DRAM_CLIENT) {
        RetVal = DRAMC_ERR_ARG;
    } else {
        pAmbaDRAMC_Reg->AccessVitual = (pAmbaDRAMC_Reg->AccessVitual) & (~((UINT32)1U << (ClientId + (UINT32)1U)));
    }

    return RetVal;
}

/**
 *  AmbaRTSL_DramGetClientInfo - DRAM controller get the VPN segment for specified client
 *  @param[in] ClientId DRAM client ID
 *  @param[out] pVirtAddr virtual base address of the specified dram client
 *  @param[out] pSize number of bytes reserved for the specified dram client
 *  @return error code
 */
UINT32 AmbaRTSL_DramGetClientInfo(UINT32 ClientId, ULONG *pVirtAddr, ULONG *pSize)
{
    UINT32 StartPage, EndPage;
    ULONG Temp;
    UINT32 RetVal = DRAMC_ERR_NONE;

    if ((ClientId >= NUM_DRAM_CLIENT) || (pVirtAddr == NULL) || (pSize == NULL)) {
        RetVal = DRAMC_ERR_ARG;
    } else {
        if ((AccessVitual & ((UINT32)1U << ClientId)) == 0U) {
            /* physical client */
            *pVirtAddr = 0U;
            *pSize = 0UL;
            RetVal = DRAMC_ERR_AT;
        } else {
            /* virtual client */
            StartPage = pAmbaDRAMC_Reg->VpnBase[ClientId];
            EndPage = pAmbaDRAMC_Reg->VpnBound[ClientId];
            *pVirtAddr = (ULONG)StartPage << AttPageOffset;
            Temp = ((ULONG)EndPage + (ULONG)1U) - (ULONG)StartPage;
            *pSize = Temp << AttPageOffset;
        }
    }

    return RetVal;
}

void AmbaRTSL_DramcEnableStatisCtrl(void)
{
    UINT32 Count = 0U;
    pAmbaDRAMC_Reg->DramStatisCtrl = 0x2U; //Reset, When reset is done, the read value will be changed to 0(OFF)
    while ((pAmbaDRAMC_Reg->DramStatisCtrl != 0x0U) && (Count < 2000000U)) {
        Count ++;
    }
    pAmbaDRAMC_Reg->DramStatisCtrl = 0x1U; //Enable
}

void AmbaRTSL_DramcGetStatisInfo(AMBA_DRAMC_STATIS_s *StatisData)
{
#if defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)
    UINT32 Index;
    UINT32 Div = 1U;

    pAmbaDRAMC_Reg->DramStatisCtrl = 0x0U; //Disable
    if (0x6U == (pAmbaDRAMC_Reg->DramMode & 0x6U)) {
        Div = 2U;
    }

    for (Index = 0U; Index < 32U; Index++) {
        StatisData->ClientRequestStatis[Index] = pAmbaDRAMC_Reg->ClientRequestStatis[Index];
        StatisData->ClientBurstStatis[Index] = pAmbaDRAMC_Reg->ClientBurstStatis[Index];
        StatisData->ClientMaskWriteStatis[Index] = pAmbaDRAMC_Reg->ClientMaskWriteStatis[Index]/Div;
    }
#else
    UINT32 Index;

    pAmbaDRAMC_Reg->DramStatisCtrl = 0x0U; //Disable

    for(Index = 0U; Index < 32U; Index++) {
        StatisData->ClientRequestStatis[Index] = pAmbaDRAMC_Reg->ClientRequestStatis[Index];
        StatisData->ClientBurstStatis[Index] = pAmbaDRAMC_Reg->ClientBurstStatis[Index];
        StatisData->ClientMaskWriteStatis[Index] = pAmbaDRAMC_Reg->ClientMaskWriteStatis[Index];
    }
#endif

}

#if defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)
UINT32 AmbaRTSL_DramcGetHostNum(UINT32 *pHostNum)
{
    UINT32 RetVal = DRAMC_ERR_NONE;

    if (pHostNum == NULL) {
        RetVal = DRAMC_ERR_ARG;
    } else {
        if (0x6U == (pAmbaDRAMC_Reg->DramMode & 0x6U)) {
            *pHostNum = 2U;
        } else {
            *pHostNum = 1U;
        }
    }

    return RetVal;
}

UINT32 AmbaRTSL_DramcGetHostStartId(UINT32 *pHostStartId)
{
    UINT32 RetVal = DRAMC_ERR_NONE;

    if (pHostStartId == NULL) {
        RetVal = DRAMC_ERR_ARG;
    } else {
        if (0x0U != (pAmbaDRAMC_Reg->DramMode & 0x2U)) {
            *pHostStartId = 0U;
        } else if (0x0U != (pAmbaDRAMC_Reg->DramMode & 0x4U)) {
            *pHostStartId = 1U;
        } else {
            RetVal = DRAMC_ERR_ARG;
        }
    }

    return RetVal;
}
#endif
