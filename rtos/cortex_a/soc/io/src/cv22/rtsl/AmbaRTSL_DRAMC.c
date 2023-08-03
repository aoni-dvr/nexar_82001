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

/**
 *  DRAMC_IrqHandler - Interrupt handler for dram controller
 *  @param[in] IntID Interrupt ID
 *  @param[in] UserArg Optional argument of the interrupt handler
 */
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

    AmbaMisra_TouchUnused(&SegFault);
    AmbaMisra_TouchUnused(&FaultAddress);
    AmbaMisra_TouchUnused(&FaultClient);
    AmbaMisra_TouchUnused(&IntID);
    AmbaMisra_TouchUnused(&UserArg);
}
#pragma GCC pop_options

/**
 *  AmbaRTSL_DramcInit - DRAM controller initializations
 */
void AmbaRTSL_DramcInit(void)
{
#if !defined(CONFIG_LINUX)
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
#endif
}

/**
 *  AmbaRTSL_DramcIrqInit - DRAM interrupt initializations
 */
void AmbaRTSL_DramcIrqInit(void)
{
    AMBA_INT_CONFIG_s IntConfig;
    UINT32 IntID;

    /* Hook interrupt handler */
    IntConfig.TriggerType  = INT_TRIG_HIGH_LEVEL;
    IntConfig.IrqType      = INT_TYPE_FIQ;      /* Since Linux does not suuport GIC group1 interrupt handling, set it as FIQ for AmbaLink. */
    IntConfig.CpuTargets   = 0x01U;             /* Target cores */

    IntID = AMBA_INT_SPI_ID137_DRAM_ERROR;
    (void)AmbaRTSL_GicIntConfig(IntID, &IntConfig, DRAMC_IrqHandler, 0U);
    (void)AmbaRTSL_GicIntEnable(IntID);
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
    for(Page = 0UL; Page < (AMBA_DRAM_ATT_LAST_PAGE_ID + 1UL); Page += 2UL) {
        /* Each hardware register contains even and odd pages mapping */
        pAmbaDRAMC_Reg->Att[Page >> 1UL] = (Page) | ((Page + 1UL) << 14UL);
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
    UINT32 VirtPage, PhysPage, NumPage;
    UINT32 PPN[2], RegVal = 0U;
    UINT32 i, RetVal = DRAMC_ERR_NONE;

    if ((NumMemRegion == 0U) || (pMemRegion == NULL)) {
        RetVal = DRAMC_ERR_ARG;
    } else {
        for (i = 0U; i < NumMemRegion; i++) {
            VirtPage = pMemRegion[i].VirtAddr >> 18UL;
            PhysPage = pMemRegion[i].PhysAddr >> 18UL;
            NumPage  = (UINT32)(pMemRegion[i].Size >> 18ULL);
            while (NumPage > 0UL) {
                RegVal = pAmbaDRAMC_Reg->Att[VirtPage >> 1UL];
                if (0x0UL != (VirtPage & 0x1UL)) {
                    /* Keep old even page number [13:0] and update new odd page number [27:14] */
                    PPN[0] = RegVal & 0x3FFFUL;
                    PPN[1] = PhysPage & 0x3FFFUL;
                } else {
                    /* Keep old odd page number [27:14] and update new even page number [13:0] */
                    PPN[0] = PhysPage & 0x3FFFU;
                    PPN[1] = (RegVal >> 14UL) & 0x3FFFUL;
                }
                RegVal = PPN[0] | (PPN[1] << 14UL);
                pAmbaDRAMC_Reg->Att[VirtPage >> 1UL] = RegVal;
                VirtPage++;
                PhysPage++;
                NumPage--;
            }
        }
    }

    return RetVal;
}

/**
 *  AmbaRTSL_DramQueryAttV2P - It is used to get the translated physical address of specified virtual address
 *  @param[in] VirtAddr the virtual address
 *  @param[out] pPhysAddr the translated physical address
 *  @return error code
 */
UINT32 AmbaRTSL_DramQueryAttV2P(ULONG VirtAddr, UINT32 DramClient, ULONG *pPhysAddr)
{
    UINT32 PhysPage, VirtPage = VirtAddr >> 18UL;
    UINT32 RegVal, RetVal = DRAMC_ERR_NONE;

    (void)DramClient;
    if (pPhysAddr == NULL) {
        RetVal = DRAMC_ERR_ARG;
    } else {
        /* Search from hardware register */
        RegVal = pAmbaDRAMC_Reg->Att[VirtPage >> 1UL];
        if (0x0UL != (VirtPage & 0x1UL)) {
            PhysPage = (RegVal >> 14UL) & 0x3FFFUL;
        } else {
            PhysPage = RegVal & 0x3FFFUL;
        }

        *pPhysAddr = (PhysPage << 18UL) | (VirtAddr & 0x3FFFFUL);
    }

    return RetVal;
}

/**
 *  AmbaRTSL_DramQueryAttP2V - It is used to get the translated virtual address of specified physical address
 *  @param[in] ClientId Client ID
 *  @param[in] PhysAddr the physical address
 *  @param[out] pVirtAddr the translated virtual address
 */
UINT32 AmbaRTSL_DramQueryAttP2V(ULONG PhysAddr, UINT32 ClientId, ULONG *pVirtAddr)
{
    UINT32 Min, Max, PhysPage = PhysAddr >> 18UL;
    UINT32 i, RegVal, RetVal;

    if ((pVirtAddr == NULL) || (PhysAddr == 0x0U) || (ClientId >= NUM_DRAM_CLIENT)) {
        RetVal = DRAMC_ERR_ARG;
    } else if (((pAmbaDRAMC_Reg->AccessVitual) & (1UL << ClientId)) == 0x0U) {
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

            for (i = ((Min + 1U) & ~0x1UL); i <= Max; i += 2UL) {
                RegVal = pAmbaDRAMC_Reg->Att[i >> 1UL];
                if (PhysPage == (RegVal & 0x3FFFUL)) {
                    *pVirtAddr = (i << 18UL) | (PhysAddr & 0x3FFFFUL);
                    RetVal = DRAMC_ERR_NONE;
                    break;
                }
            }
            if (RetVal != DRAMC_ERR_NONE) {
                for (i = (Min | 0x1UL); i <= Max; i += 2UL) {
                    RegVal = pAmbaDRAMC_Reg->Att[i >> 1UL];
                    if (PhysPage == ((RegVal >> 14UL) & 0x3FFFUL)) {
                        *pVirtAddr = (i << 18UL) | (PhysAddr & 0x3FFFFUL);
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
    UINT32 StartPage = VirtAddr >> 18UL;
    UINT32 NumPage = Size >> 18UL;
    UINT32 RetVal = DRAMC_ERR_NONE;

    if ((StartPage + NumPage) > 0x4000UL) {
        RetVal = DRAMC_ERR_ARG;
    } else {
        pAmbaDRAMC_Reg->AttSecureBase = StartPage;
        pAmbaDRAMC_Reg->AttSecureLimit = StartPage + NumPage - 1UL;
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
    UINT32 StartPage = VirtAddr >> 18UL;
    UINT32 NumPage = (UINT32)(Size >> 18ULL);
    UINT32 RetVal = DRAMC_ERR_NONE;

    if ((ClientId >= NUM_DRAM_CLIENT) || ((StartPage + NumPage) > 0x4000UL) || ((0x0UL == (StartPage + NumPage)))) {
        RetVal = DRAMC_ERR_ARG;
    } else {
        pAmbaDRAMC_Reg->VpnBase[ClientId] = StartPage;
        pAmbaDRAMC_Reg->VpnBound[ClientId] = StartPage + NumPage - 1UL;
        pAmbaDRAMC_Reg->AccessVitual = (pAmbaDRAMC_Reg->AccessVitual) | (1UL << ClientId);
    }

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
        pAmbaDRAMC_Reg->AccessVitual = (pAmbaDRAMC_Reg->AccessVitual) & (~(1UL << (ClientId + 1UL)));
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
    UINT32 WorkUINT32;
    UINT32 StartPage, EndPage;
    UINT32 RetVal = DRAMC_ERR_NONE;

    if ((ClientId >= NUM_DRAM_CLIENT) || (pVirtAddr == NULL) || (pSize == NULL)) {
        RetVal = DRAMC_ERR_ARG;
    } else {
        if ((pAmbaDRAMC_Reg->AccessVitual & (1UL << ClientId)) == 0UL) {
            /* physical client */
            *pVirtAddr = 0UL;
            *pSize = 0UL;
            RetVal = DRAMC_ERR_AT;
        } else {
            /* virtual client */
            StartPage = pAmbaDRAMC_Reg->VpnBase[ClientId];
            EndPage = pAmbaDRAMC_Reg->VpnBound[ClientId];
            *pVirtAddr = StartPage << 18UL;
            WorkUINT32 = (EndPage + 1UL) - StartPage;
            *pSize = (UINT64)WorkUINT32 << 18ULL;
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
    UINT32 Index;

    pAmbaDRAMC_Reg->DramStatisCtrl = 0x0U; //Disable

    for(Index = 0U; Index < 32U; Index++) {
        StatisData->ClientRequestStatis[Index] = pAmbaDRAMC_Reg->ClientRequestStatis[Index];
        StatisData->ClientBurstStatis[Index] = pAmbaDRAMC_Reg->ClientBurstStatis[Index];
        StatisData->ClientMaskWriteStatis[Index] = pAmbaDRAMC_Reg->ClientMaskWriteStatis[Index];
    }
}
