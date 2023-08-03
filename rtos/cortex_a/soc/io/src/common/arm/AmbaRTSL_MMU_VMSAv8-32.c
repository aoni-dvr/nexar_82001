/**
 *  @file AmbaRTSL_MMU_VMSAv8-32.c
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
 *  @details Memory Management Unit Control APIs
 *
 */

#include "AmbaTypes.h"
#include "AmbaIntrinsics.h"
#include "AmbaMisraFix.h"

#if defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)
#include "AmbaCortexA76.h"
#else
#include "AmbaCortexA53.h"
#endif
#include "AmbaDebugPort.h"

#include "AmbaMMU.h"
#include "AmbaRTSL_Cache.h"
#include "AmbaRTSL_CPU.h"
#include "AmbaRTSL_MMU.h"
#include "AmbaRTSL_MMU_Priv.h"
#include "AmbaRTSL_MMU_VMSAv8-32.h"

#ifdef CONFIG_XEN_SUPPORT
#include <AmbaXen.h>
#endif
//#define amba_xen_print(...)
//#define amba_xen_print_u32(...)

/* The page table needs to align on a 16 KB boundary no matter short or long descriptor mode */
#ifdef CONFIG_XEN_SUPPORT
/* Page size alignment at .lds */
static AMBA_TLB_S_DESC_s AmbaMmuPageTable __attribute__((section(".data.mmu_ptbl")));
#else
static AMBA_TLB_S_DESC_s AmbaMmuPageTable __attribute__((aligned(0x4000))) GNU_SECTION_NOZEROINIT;
#endif

static AMBA_TLB_S_DESC_s *pAmbaTlbDescS = NULL;
static AMBA_TLB_L_DESC_s *pAmbaTlbDescL = NULL;

static AMBA_MMU_MAP_INFO32_s AmbaMmuMmioMap[AMBA_MMU_NUM_MMIO_REGION];

static UINT32 MMU_GetAttrIndex(UINT32 MemAttr)
{
    UINT32 Ret;

    if (MemAttr == 0x00U) {         // Device-nGnRnE
        Ret = 0U;
    } else if (MemAttr == 0x04U) {  // Device-nGnRE
        Ret = 1U;
    } else if (MemAttr == 0x77U) {  // Normal Memory, Write-Back Cacheable
        /* For Cortex-A53, the memory is cached in the L1 Data cache and the L2 cache iff it is marked as Inner and Outer Write-Back Cacheable. */
        Ret = 3U;
    } else {                        // Normal Memory, Non-Cacheable
        /* Cortex-A53 doesn't support Write-Through Cacheable */
        Ret = 2U;
    }

    return Ret;
}
/**
 *  MMU_ZeroMemory - Zero memory. The AArch64 memset might use cache operation however data caching is not yet enabled here.
 *  @param[in] pBuf Pointer to the block of memory to fill zeros
 *  @param[in] Size Number of bytes to be set to zero
 */
static void MMU_ZeroMemory(const void *pBuf, UINT32 Size)
{
    UINT32 *pDWORD;
    UINT32 Tmp = Size;

    AmbaMisra_TypeCast32(&pDWORD, &pBuf);

    while (Tmp > 0U) {
        *pDWORD = 0U;
        Tmp -= 4U;
        pDWORD++;
    }
}

/**
 *  MMU_TlbReset - Initialize translation table
 */
static void MMU_TlbReset(void)
{
    AMBA_TLB_S_DESC_s *pPageTblBase = &AmbaMmuPageTable;
#if !defined(CONFIG_XEN_SUPPORT) || defined(AMBA_FWPROG)
    UINT32 Size;
#endif

    AmbaMisra_TypeCast32(&pAmbaTlbDescS, &pPageTblBase);
    AmbaMisra_TypeCast32(&pAmbaTlbDescL, &pPageTblBase);

    (void)AmbaRTSL_CacheCleanDataPtr(&pAmbaTlbDescS, sizeof(AMBA_TLB_S_DESC_s *));
    (void)AmbaRTSL_CacheCleanDataPtr(&pAmbaTlbDescL, sizeof(AMBA_TLB_L_DESC_s *));

#if !defined(CONFIG_XEN_SUPPORT) || defined(AMBA_FWPROG)
    /* There is no need to invalidate L2 table. Invalidate L1 table only to shorten boot flow.*/
    Size = AMBA_NUM_TLB_L1_SHORT_DESC;
    while (Size > 0U) {
        Size --;
        pPageTblBase->L1Desc[Size].Data = 0U;
    }
#endif // CONFIG_XEN_SUPPORT
    (void)AmbaRTSL_CacheCleanDataPtr(pPageTblBase->L1Desc, AMBA_NUM_TLB_L1_SHORT_DESC * sizeof(AMBA_TLB_S_DESC_L1_s));
}

/**
 *  MMU_TlbSetupL1Block - Setup long-descriptor L1 block descriptor. (stage 1)
 *  @param[in] pMapInfo Pointer to user-supplied MMU map entry.
 *  @return error code
 */
static UINT32 MMU_TlbSetupL1Block(const AMBA_MMU_MAP_INFO32_s *pMapInfo)
{
    AMBA_TLB_L_DESC_L1_BLK_s *pL1Desc, TmpL1Desc;
    AMBA_TLB_L_DESC_L1_BLK_s L1Desc = {0U};

    const AMBA_TLB_L_DESC_L1_s *pL1Tmp;
    UINT32 L1DescStart, L1DescEnd;
    UINT64 L1BlockBase;
    UINT32 L1DescID;
    UINT32 RetVal = MMU_ERR_NONE;
    AMBA_MMU_MAP_ATTR32_s MemAttrBits;

    AmbaMisra_TypeCast32(&MemAttrBits, &pMapInfo->MemAttr);

    L1DescStart = AMBA_TLB_GET_L1_BLOCK_ID(pMapInfo->VirtAddr);
    L1DescEnd   = AMBA_TLB_GET_L1_BLOCK_ID(pMapInfo->VirtAddr + pMapInfo->MemSize - 1U);
    L1BlockBase = (UINT64)pMapInfo->PhysAddr & AMBA_TLB_L1_BLOCK_BASE_MASK;

    if (L1DescEnd >= AMBA_NUM_TLB_L1_LONG_DESC) {
        RetVal = MMU_ERR_ARG;
    } else {
        L1Desc.DescType     = AMBA_TLB_L1_LONG_DESC_BLOCK;


        L1Desc.AttrIndex = (UINT8)MMU_GetAttrIndex(MemAttrBits.MemAttr);

        L1Desc.NonSecure    = MemAttrBits.NonSecure;
        L1Desc.AP           = MemAttrBits.S1AP;
        L1Desc.Shareability = MemAttrBits.Shareability;
        L1Desc.AccessFlag   = 1U;
        L1Desc.NotGlobal    = MemAttrBits.NotGlobal;
        L1Desc.Contiguous   = 0U;
        L1Desc.PrivExeNever = MemAttrBits.PXN;
        L1Desc.ExeNever     = MemAttrBits.UXN;

        /* pL1Desc = &pAmbaTlbDescL->L1Desc[L1DescStart]; */
        pL1Tmp = &pAmbaTlbDescL->L1Desc[L1DescStart];
        AmbaMisra_TypeCast32(&pL1Desc, &pL1Tmp);

        for (L1DescID = L1DescStart; L1DescID <= L1DescEnd; L1DescID++) {
            /* pL1Desc->OutputAddr = L1BlockBase >> AMBA_TLB_L1_BLOCK_ADDR_OFFSET; */
            AmbaMisra_TypeCast64(&TmpL1Desc, &L1BlockBase);
            L1Desc.OutputAddrL = TmpL1Desc.OutputAddrL;
            L1Desc.OutputAddrH = TmpL1Desc.OutputAddrH;
            L1BlockBase += (1ULL << AMBA_TLB_L1_BLOCK_ADDR_OFFSET);

            AmbaMisra_TypeCast64(pL1Desc, &L1Desc);
            pL1Desc++;
        }
        (void)AmbaRTSL_CacheFlushDataPtr(pL1Tmp, ((L1DescEnd - L1DescStart) + 1U) * sizeof(AMBA_TLB_L_DESC_L1_s));
    }

    return RetVal;
}

/**
 *  MMU_TlbSetupL2Block - Setup long-descriptor L2 block descriptor. (stage 1)
 *  @param[in] pMapInfo Pointer to user-supplied MMU map entry.
 *  @return error code
 */
static UINT32 MMU_TlbSetupL2Block(const AMBA_MMU_MAP_INFO32_s *pMapInfo)
{
    AMBA_TLB_L_DESC_L1_TBL_s *pL1Desc, TmpL1Desc;
    AMBA_TLB_L_DESC_L2_BLK_s *pL2Desc, TmpL2Desc;
    AMBA_TLB_L_DESC_L2_BLK_s L2Desc = {0U};

    const AMBA_TLB_L_DESC_L1_s *pL1Tmp;
    const AMBA_TLB_L_DESC_L2_s *pL2Tmp;
    UINT32 L2DescPhysAddr, L2DescVirtAddr;
    UINT32 L1DescStart, L1DescEnd;
    UINT32 L2DescStart, L2DescEnd;
    UINT64 L2BlockBase;
    UINT32 L1DescID, L2DescID;
    UINT32 RetVal = MMU_ERR_NONE;
    AMBA_MMU_MAP_ATTR32_s MemAttrBits;

    AmbaMisra_TypeCast32(&MemAttrBits, &pMapInfo->MemAttr);

    L1DescStart = AMBA_TLB_GET_L1_BLOCK_ID(pMapInfo->VirtAddr);
    L1DescEnd   = AMBA_TLB_GET_L1_BLOCK_ID(pMapInfo->VirtAddr + pMapInfo->MemSize - 1U);
    L2BlockBase = (UINT64)pMapInfo->PhysAddr & AMBA_TLB_L2_BLOCK_BASE_MASK;

    if (L1DescEnd >= AMBA_NUM_TLB_L1_LONG_DESC) {
        RetVal = MMU_ERR_ARG;
    } else {
        L2Desc.DescType = AMBA_TLB_L2_LONG_DESC_BLOCK;

        L2Desc.AttrIndex = (UINT8)MMU_GetAttrIndex(MemAttrBits.MemAttr);

        L2Desc.NonSecure    = MemAttrBits.NonSecure;
        L2Desc.AP           = MemAttrBits.S1AP;
        L2Desc.Shareability = MemAttrBits.Shareability;
        L2Desc.AccessFlag   = 1U;
        L2Desc.NotGlobal    = MemAttrBits.NotGlobal;
        L2Desc.Contiguous   = 0U;
        L2Desc.PrivExeNever = MemAttrBits.PXN;
        L2Desc.ExeNever     = MemAttrBits.UXN;

        for (L1DescID = L1DescStart; L1DescID <= L1DescEnd; L1DescID++) {
            pL1Tmp = &pAmbaTlbDescL->L1Desc[L1DescID];
            AmbaMisra_TypeCast32(&pL1Desc, &pL1Tmp);

            if (pL1Desc->DescType == AMBA_TLB_L1_LONG_DESC_BLOCK) {
                /* This region is already mapped by L1 Block descriptor. */
                break;
            } else if (pL1Desc->DescType == AMBA_TLB_L1_LONG_DESC_TABLE) {
                /* Use the table base address described in L1 Table descriptor. */
                L2DescPhysAddr = (UINT32)(pL1Tmp->Data & AMBA_TLB_L1_TABLE_BASE_MASK);
                (void)AmbaRTSL_MmuPhys32ToVirt32(L2DescPhysAddr, &L2DescVirtAddr);
                AmbaMisra_TypeCast32(&pL2Desc, &L2DescVirtAddr);
                (void)AmbaRTSL_CacheFlushDataPtr(&pL2Desc, sizeof(AMBA_TLB_L_DESC_L2_BLK_s *));
            } else {
                /* pL2Desc = pAmbaTlbDescL->L2Desc[L1DescID]; */
                pL2Tmp = pAmbaTlbDescL->L2Desc[L1DescID];
                AmbaMisra_TypeCast32(&pL2Desc, &pL2Tmp);
                MMU_ZeroMemory(pL2Desc, (AMBA_NUM_TLB_L2_LONG_DESC * sizeof(AMBA_TLB_L_DESC_L2_BLK_s)));
                (void)AmbaRTSL_CacheCleanDataPtr(pL2Desc, AMBA_NUM_TLB_L2_LONG_DESC * sizeof(AMBA_TLB_L_DESC_L2_BLK_s));

                /* Setup a new L1 Table descriptor. */
                AmbaMisra_TypeCast32(&L2DescVirtAddr, &pL2Desc);
                (void)AmbaRTSL_MmuVirt32ToPhys32(L2DescVirtAddr, &L2DescPhysAddr);
                /* pL1Desc->L2TableAddr = L2DescPhysAddr >> AMBA_TLB_L1_TABLE_ADDR_OFFSET; */
                AmbaMisra_TypeCast64(&TmpL1Desc, &L2DescPhysAddr);
                pL1Desc->L2TableAddrL = TmpL1Desc.L2TableAddrL;
                pL1Desc->L2TableAddrH = TmpL1Desc.L2TableAddrH;
                pL1Desc->DescType = AMBA_TLB_L1_LONG_DESC_TABLE;
                (void)AmbaRTSL_CacheCleanDataPtr(pL1Desc, sizeof(AMBA_TLB_L_DESC_L1_TBL_s));
            }

            if (L1DescID == L1DescStart) {
                L2DescStart = AMBA_TLB_GET_L2_BLOCK_ID(pMapInfo->VirtAddr);
            } else {
                L2DescStart = 0U;
            }

            if (L1DescID == L1DescEnd) {
                L2DescEnd = AMBA_TLB_GET_L2_BLOCK_ID(pMapInfo->VirtAddr + pMapInfo->MemSize - 1U);
            } else {
                L2DescEnd = AMBA_NUM_TLB_L2_LONG_DESC - 1U;
            }

            for (L2DescID = L2DescStart; L2DescID <= L2DescEnd; L2DescID++) {
                /* pL2DescTmp->OutputAddr = L2BlockBase >> AMBA_TLB_L2_BLOCK_ADDR_OFFSET; */
                AmbaMisra_TypeCast64(&TmpL2Desc, &L2BlockBase);
                L2Desc.OutputAddrL = TmpL2Desc.OutputAddrL;
                L2Desc.OutputAddrH = TmpL2Desc.OutputAddrH;
                L2BlockBase += (1ULL << AMBA_TLB_L2_BLOCK_ADDR_OFFSET);

                AmbaMisra_TypeCast64(&pL2Desc[L2DescID], &L2Desc);
            }
            (void)AmbaRTSL_CacheFlushDataPtr(&pL2Desc[L2DescStart], ((L2DescEnd - L2DescStart) + 1U) * sizeof(AMBA_TLB_L_DESC_L2_BLK_s));
        }
    }

    return RetVal;
}

/**
 *  MMU_TlbSetupL3Page - Setup long-descriptor L3 page descriptor. (stage 1)
 *  @param[in] pMapInfo Pointer to user-supplied MMU map entry.
 *  @return error code
 */
static UINT32 MMU_TlbSetupL3Page(const AMBA_MMU_MAP_INFO32_s *pMapInfo)
{
    AMBA_TLB_L_DESC_L1_TBL_s *pL1Desc, TmpL1Desc;
    AMBA_TLB_L_DESC_L2_TBL_s *pL2Desc, TmpL2Desc;
    AMBA_TLB_L_DESC_L3_PG_s *pL3Desc, TmpL3Desc;
    AMBA_TLB_L_DESC_L3_PG_s L3Desc = {0U};

    const AMBA_TLB_L_DESC_L1_s *pL1Tmp;
    const AMBA_TLB_L_DESC_L2_s *pL2Tmp;
    const AMBA_TLB_L_DESC_L3_s *pL3Tmp;
    UINT32 DescPhysAddr, DescVirtAddr;
    UINT32 L1DescStart, L1DescEnd;
    UINT32 L2DescStart, L2DescEnd;
    UINT32 L3DescStart, L3DescEnd;
    UINT64 L3PageBase;
    UINT32 L1DescID, L2DescID, L3DescID;
    UINT32 RetVal = MMU_ERR_NONE;
    AMBA_MMU_MAP_ATTR32_s MemAttrBits;

    AmbaMisra_TypeCast32(&MemAttrBits, &pMapInfo->MemAttr);

    L1DescStart = AMBA_TLB_GET_L1_BLOCK_ID(pMapInfo->VirtAddr);
    L1DescEnd   = AMBA_TLB_GET_L1_BLOCK_ID(pMapInfo->VirtAddr + pMapInfo->MemSize - 1U);
    L3PageBase  = (UINT64)pMapInfo->PhysAddr & AMBA_TLB_L3_PAGE_BASE_MASK;

    if (L1DescEnd >= AMBA_NUM_TLB_L1_LONG_DESC) {
        RetVal = MMU_ERR_ARG;
    } else {
        L3Desc.DescType      = AMBA_TLB_L3_LONG_DESC_PAGE;

        L3Desc.AttrIndex = (UINT8)MMU_GetAttrIndex(MemAttrBits.MemAttr);

        L3Desc.NonSecure     = MemAttrBits.NonSecure;
        L3Desc.AP            = MemAttrBits.S1AP;
        L3Desc.Shareability  = MemAttrBits.Shareability;
        L3Desc.AccessFlag    = 1U;
        L3Desc.NotGlobal     = MemAttrBits.NotGlobal;
        L3Desc.Contiguous    = 0U;
        L3Desc.PrivExeNever  = MemAttrBits.PXN;
        L3Desc.ExeNever      = MemAttrBits.UXN;

        for (L1DescID = L1DescStart; L1DescID <= L1DescEnd; L1DescID++) {
            pL1Tmp = &pAmbaTlbDescL->L1Desc[L1DescID];
            AmbaMisra_TypeCast32(&pL1Desc, &pL1Tmp);

            if (pL1Desc->DescType == AMBA_TLB_L1_LONG_DESC_BLOCK) {
                /* This region is already mapped by L1 Block descriptor. */
                break;
            } else if (pL1Desc->DescType == AMBA_TLB_L1_LONG_DESC_TABLE) {
                /* Use the table base address described in L1 Table descriptor. */
                DescPhysAddr = (UINT32)(pL1Tmp->Data & AMBA_TLB_L1_TABLE_BASE_MASK);
                (void)AmbaRTSL_MmuPhys32ToVirt32(DescPhysAddr, &DescVirtAddr);
                AmbaMisra_TypeCast32(&pL2Desc, &DescVirtAddr);
                (void)AmbaRTSL_CacheFlushDataPtr(&pL2Desc, sizeof(AMBA_TLB_L_DESC_L2_TBL_s *));
            } else {
                /* pL2Desc = pAmbaTlbDescL->L2Desc[L1DescID]; */
                pL2Tmp = pAmbaTlbDescL->L2Desc[L1DescID];
                AmbaMisra_TypeCast32(&pL2Desc, &pL2Tmp);
                MMU_ZeroMemory(pL2Desc, (AMBA_NUM_TLB_L2_LONG_DESC * sizeof(AMBA_TLB_L_DESC_L2_TBL_s)));
                (void)AmbaRTSL_CacheCleanDataPtr(pL2Desc, AMBA_NUM_TLB_L2_LONG_DESC * sizeof(AMBA_TLB_L_DESC_L2_TBL_s));

                /* Setup a new L1 Table descriptor. */
                AmbaMisra_TypeCast32(&DescVirtAddr, &pL2Desc);
                (void)AmbaRTSL_MmuVirt32ToPhys32(DescVirtAddr, &DescPhysAddr);
                /* pL1Desc->L2TableAddr = DescPhysAddr >> AMBA_TLB_L1_TABLE_ADDR_OFFSET; */
                AmbaMisra_TypeCast64(&TmpL1Desc, &DescPhysAddr);
                pL1Desc->L2TableAddrL = TmpL1Desc.L2TableAddrL;
                pL1Desc->L2TableAddrH = TmpL1Desc.L2TableAddrH;
                pL1Desc->DescType     = AMBA_TLB_L1_LONG_DESC_TABLE;
                (void)AmbaRTSL_CacheCleanDataPtr(pL1Desc, sizeof(AMBA_TLB_L_DESC_L1_TBL_s));
            }

            if (L1DescID == L1DescStart) {
                L2DescStart = AMBA_TLB_GET_L2_BLOCK_ID(pMapInfo->VirtAddr);
            } else {
                L2DescStart = 0U;
            }

            if (L1DescID == L1DescEnd) {
                L2DescEnd = AMBA_TLB_GET_L2_BLOCK_ID(pMapInfo->VirtAddr + pMapInfo->MemSize - 1U);
            } else {
                L2DescEnd = AMBA_NUM_TLB_L2_LONG_DESC - 1U;
            }

            for (L2DescID = L2DescStart; L2DescID <= L2DescEnd; L2DescID++) {
                pL2Tmp = &pAmbaTlbDescL->L2Desc[L1DescID][L2DescID];
                AmbaMisra_TypeCast32(&pL2Desc, &pL2Tmp);

                if (pL2Desc->DescType == AMBA_TLB_L2_LONG_DESC_BLOCK) {
                    /* This region is already mapped by L2 Block descriptor. */
                    break;
                } else if (pL2Desc->DescType == AMBA_TLB_L2_LONG_DESC_TABLE) {
                    /* Use the table base address described in L2 Table descriptor. */
                    DescPhysAddr = (UINT32)(pL2Tmp->Data & AMBA_TLB_L2_TABLE_BASE_MASK);
                    (void)AmbaRTSL_MmuPhys32ToVirt32(DescPhysAddr, &DescVirtAddr);
                    AmbaMisra_TypeCast32(&pL3Desc, &DescVirtAddr);
                    (void)AmbaRTSL_CacheFlushDataPtr(&pL3Desc, sizeof(AMBA_TLB_L_DESC_L3_PG_s *));
                } else {
                    pL3Tmp = pAmbaTlbDescL->L3Desc[L1DescID][L2DescID];
                    AmbaMisra_TypeCast32(&pL3Desc, &pL3Tmp);
                    MMU_ZeroMemory(pL3Desc, (UINT32)(AMBA_NUM_TLB_L3_LONG_DESC * sizeof(AMBA_TLB_L_DESC_L3_PG_s)));
                    (void)AmbaRTSL_CacheCleanDataPtr(pL3Desc, AMBA_NUM_TLB_L3_LONG_DESC * sizeof(AMBA_TLB_L_DESC_L3_PG_s));

                    /* Setup a new L2 Table descriptor. */
                    AmbaMisra_TypeCast32(&DescVirtAddr, &pL3Desc);
                    (void)AmbaRTSL_MmuVirt32ToPhys32(DescVirtAddr, &DescPhysAddr);

                    /* pL2Desc->L3TableAddr = DescPhysAddr >> AMBA_TLB_L2_TABLE_ADDR_OFFSET; */
                    AmbaMisra_TypeCast64(&TmpL2Desc, &DescPhysAddr);
                    pL2Desc->L3TableAddrL = TmpL2Desc.L3TableAddrL;
                    pL2Desc->L3TableAddrH = TmpL2Desc.L3TableAddrH;
                    pL2Desc->DescType     = AMBA_TLB_L2_LONG_DESC_TABLE;
                    (void)AmbaRTSL_CacheFlushDataPtr(pL2Desc, sizeof(AMBA_TLB_L_DESC_L2_TBL_s));
                }

                if (L2DescID == L2DescStart) {
                    L3DescStart = AMBA_TLB_GET_L3_PAGE_ID(pMapInfo->VirtAddr);
                } else {
                    L3DescStart = 0U;
                }

                if (L2DescID == L2DescEnd) {
                    L3DescEnd = AMBA_TLB_GET_L3_PAGE_ID(pMapInfo->VirtAddr + pMapInfo->MemSize - 1U);
                } else {
                    L3DescEnd = AMBA_NUM_TLB_L3_LONG_DESC - 1U;
                }

                for (L3DescID = L3DescStart; L3DescID <= L3DescEnd; L3DescID++) {
                    /* pL3DescTmp->OutputAddr = L3PageBase >> AMBA_TLB_L3_PAGE_ADDR_OFFSET; */
                    AmbaMisra_TypeCast64(&TmpL3Desc, &L3PageBase);
                    L3Desc.OutputAddrL = TmpL3Desc.OutputAddrL;
                    L3Desc.OutputAddrH = TmpL3Desc.OutputAddrH;
                    L3PageBase += (1ULL << AMBA_TLB_L3_PAGE_ADDR_OFFSET);

                    AmbaMisra_TypeCast64(&pL3Desc[L3DescID], &L3Desc);
                }
                (void)AmbaRTSL_CacheFlushDataPtr(&pL3Desc[L3DescStart], ((L3DescEnd - L3DescStart) + 1U) * sizeof(AMBA_TLB_L_DESC_L3_PG_s));
            }
        }
    }

    return RetVal;
}

/**
 *  MMU_TlbSetupL1Section - Setup short-descriptor L1 section descriptor.
 *  @param[in] pMapInfo Pointer to user-supplied MMU map entry.
 *  @return error code
 */
static UINT32 MMU_TlbSetupL1Section(const AMBA_MMU_MAP_INFO32_s *pMapInfo)
{
    AMBA_TLB_S_DESC_L1_SEC_s *pL1Desc;
    AMBA_TLB_S_DESC_L1_SEC_s TmpL1Desc = {0U};
    const AMBA_TLB_S_DESC_L1_s *pTmp = NULL;
    UINT32 L1DescStart, L1DescEnd;
    UINT32 L1SectBase;
    UINT32 L1DescID;
    UINT32 RetVal = MMU_ERR_NONE;
    AMBA_MMU_MAP_ATTR32_s MemAttrBits;

    AmbaMisra_TypeCast32(&MemAttrBits, &pMapInfo->MemAttr);

    L1DescStart = AMBA_TLB_GET_L1_SECT_ID(pMapInfo->VirtAddr);
    L1DescEnd   = AMBA_TLB_GET_L1_SECT_ID(pMapInfo->VirtAddr + pMapInfo->MemSize - 1U);
    L1SectBase  = AMBA_TLB_L1_SECTION_BASE(pMapInfo->PhysAddr);

    if (L1DescEnd >= AMBA_NUM_TLB_L1_SHORT_DESC) {
        RetVal = MMU_ERR_ARG;
    } else {
        pL1Desc = &TmpL1Desc;
        pL1Desc->DescType = AMBA_TLB_L1_SHORT_DESC_SECTION;

        if (MemAttrBits.MemAttr == 0x00U) {         // Device-nGnRnE
            pL1Desc->TEX        = 0U;
            pL1Desc->Cached     = 0U;
            pL1Desc->Buffered   = 0U;
        } else if (MemAttrBits.MemAttr == 0x04U) {  // Device-nGnRE
            pL1Desc->TEX        = (MemAttrBits.Shareability < 2U) ? 2U : 0U;
            pL1Desc->Cached     = 0U;
            pL1Desc->Buffered   = (MemAttrBits.Shareability < 2U) ? 0U : 1U;
        } else if (MemAttrBits.MemAttr == 0x77U) {  // Normal Memory, Write-Back Cacheable
            /* For Cortex-A53, the memory is cached in the L1 Data cache and the L2 cache iff it is marked as Inner and Outer Write-Back Cacheable. */
            pL1Desc->TEX        = 1U;
            pL1Desc->Cached     = 1U;
            pL1Desc->Buffered   = 1U;
        } else {                                    // Normal Memory, Non-Cacheable
            /* Cortex-A53 doesn't support Write-Through Cacheable */
            pL1Desc->TEX        = 1U;
            pL1Desc->Cached     = 0U;
            pL1Desc->Buffered   = 0U;
        }

        pL1Desc->Shared         = (MemAttrBits.Shareability < 2U) ? 0U : 1U;
        pL1Desc->AP             = (UINT8)(((UINT8)(MemAttrBits.S1AP) << (UINT8)1U) | (UINT8)0x1U);    /* AP[0] is access flag */
        pL1Desc->AP2            = (UINT8)(MemAttrBits.S1AP >> 1U);
        pL1Desc->NotGlobal      = MemAttrBits.NotGlobal;
        pL1Desc->NonSecure      = MemAttrBits.NonSecure;
        pL1Desc->ExeNever       = MemAttrBits.UXN;
        pL1Desc->Domain         = MemAttrBits.Domain;

        pTmp = &pAmbaTlbDescS->L1Desc[L1DescStart];
        AmbaMisra_TypeCast32(&pL1Desc, &pTmp);
        for (L1DescID = L1DescStart; L1DescID <= L1DescEnd; L1DescID++) {
            AmbaMisra_TypeCast32(pL1Desc, &TmpL1Desc);
            pL1Desc->BaseAddr = (UINT16)L1SectBase;
            pL1Desc++;
            L1SectBase++;
        }
        AmbaMisra_TypeCast32(&pL1Desc, &pTmp);
        (void)AmbaRTSL_CacheFlushDataPtr(pL1Desc, ((L1DescEnd - L1DescStart) + 1U) * sizeof(AMBA_TLB_S_DESC_L1_SEC_s));
    }

    return RetVal;
}

/**
 *  MMU_TlbSetupL2LargePage - Setup short-descriptor L2 large pagetables.
 *  @param[in] pMapInfo Pointer to user-supplied MMU map entry.
 *  @return error code
 */
static UINT32 MMU_TlbSetupL2LargePage(const AMBA_MMU_MAP_INFO32_s *pMapInfo)
{
    AMBA_TLB_S_DESC_L1_PG_TBL_s *pL1Desc;
    AMBA_TLB_S_DESC_L2_LARGE_PG_s *pL2Desc;
    AMBA_TLB_S_DESC_L2_LARGE_PG_s TmpL2Desc = {0};

    const AMBA_TLB_S_DESC_L1_s *pL1Tmp;
    AMBA_TLB_S_DESC_L2_s *pL2DescTmp;
    AMBA_TLB_S_DESC_L2_s L2DescTmp = {0};
    UINT32 DesAddr;
    UINT32 L1DescStart, L1DescEnd;
    UINT32 L2DescStart, L2DescEnd;
    UINT32 L2LargePageBase;
    UINT32 L1DescID, L2DescID;
    UINT32 RetVal = MMU_ERR_NONE;
    AMBA_MMU_MAP_ATTR32_s MemAttrBits;

    AmbaMisra_TypeCast32(&MemAttrBits, &pMapInfo->MemAttr);

    L1DescStart     = AMBA_TLB_GET_L1_SECT_ID(pMapInfo->VirtAddr);
    L1DescEnd       = AMBA_TLB_GET_L1_SECT_ID(pMapInfo->VirtAddr + pMapInfo->MemSize - 1U);
    L2LargePageBase = AMBA_TLB_L2_LARGE_PAGE_BASE(pMapInfo->PhysAddr);

    if (L1DescEnd >= AMBA_NUM_TLB_L1_SHORT_DESC) {
        RetVal = MMU_ERR_ARG;
    } else {
        pL2Desc = &TmpL2Desc;
        pL2Desc->DescType     = AMBA_TLB_L2_SHORT_DESC_LARGE_PAGE;

        if (MemAttrBits.MemAttr == 0x00U) {         // Device-nGnRnE
            pL2Desc->TEX      = 0U;
            pL2Desc->Cached   = 0U;
            pL2Desc->Buffered = 0U;
        } else if (MemAttrBits.MemAttr == 0x04U) {  // Device-nGnRE
            pL2Desc->TEX      = (MemAttrBits.Shareability < 2U) ? 2U : 0U;
            pL2Desc->Cached   = 0U;
            pL2Desc->Buffered = (MemAttrBits.Shareability < 2U) ? 0U : 1U;
        } else if (MemAttrBits.MemAttr == 0x77U) {  // Normal Memory, Write-Back Cacheable
            /* For Cortex-A53, the memory is cached in the L1 Data cache and the L2 cache iff it is marked as Inner and Outer Write-Back Cacheable. */
            pL2Desc->TEX      = 1U;
            pL2Desc->Cached   = 1U;
            pL2Desc->Buffered = 1U;
        } else {                                    // Normal Memory, Non-Cacheable
            /* Cortex-A53 doesn't support Write-Through Cacheable */
            pL2Desc->TEX      = 1U;
            pL2Desc->Cached   = 0U;
            pL2Desc->Buffered = 0U;
        }

        pL2Desc->Shared       = (MemAttrBits.Shareability < 2U) ? 0U : 1U;
        pL2Desc->AP           = (UINT8)(((UINT8)(MemAttrBits.S1AP) << 1U) | (UINT8)0x1U);    /* AP[0] is access flag */
        pL2Desc->AP2          = (UINT8)(MemAttrBits.S1AP >> 1U);
        pL2Desc->NotGlobal    = MemAttrBits.NotGlobal;
        pL2Desc->ExeNever     = MemAttrBits.UXN;

        for (L1DescID = L1DescStart; L1DescID <= L1DescEnd; L1DescID++) {
            pL1Tmp = &pAmbaTlbDescS->L1Desc[L1DescID];
            AmbaMisra_TypeCast32(&pL1Desc, &pL1Tmp);

            if (pL1Desc->DescType == AMBA_TLB_L1_SHORT_DESC_SECTION) {
                /* This region is already mapped by L1 Section/Supersection descriptor. */
                break;
            } else if (pL1Desc->DescType == AMBA_TLB_L1_SHORT_DESC_PAGE_TABLE) {
                /* Use the page table base address described in L1 Page Table descriptor. */
                DesAddr = (pL1Tmp->Data & AMBA_TLB_L1_PAGE_TABLE_BASE_MASK);
                AmbaMisra_TypeCast32(&pL2Desc, &DesAddr);
            } else {
                pL2DescTmp = pAmbaTlbDescS->L2Desc[L1DescID];
                AmbaMisra_TypeCast32(&pL2Desc, &pL2DescTmp);
                MMU_ZeroMemory(pL2Desc, AMBA_NUM_TLB_L2_SHORT_DESC * sizeof(AMBA_TLB_S_DESC_L2_LARGE_PG_s));
                (void)AmbaRTSL_CacheCleanDataPtr(pL2Desc, AMBA_NUM_TLB_L2_SHORT_DESC * sizeof(AMBA_TLB_S_DESC_L2_LARGE_PG_s));

                /* Setup a new L1 Page Table descriptor. */
                AmbaMisra_TypeCast32(&DesAddr, &pL2Desc);
                pL1Desc->DescType   = AMBA_TLB_L1_SHORT_DESC_PAGE_TABLE;
                pL1Desc->NonSecure  = MemAttrBits.NonSecure;
                pL1Desc->Domain     = MemAttrBits.Domain;
                pL1Desc->BaseAddr   = AMBA_TLB_L1_PAGE_TABLE_BASE(DesAddr);
                (void)AmbaRTSL_CacheFlushDataPtr(pL1Desc, sizeof(AMBA_TLB_S_DESC_L1_PG_TBL_s));
            }

            if (L1DescID == L1DescStart) {
                L2DescStart = AMBA_TLB_GET_L2_LARGE_PAGE_ID(pMapInfo->VirtAddr);
            } else {
                L2DescStart = 0U;
            }

            if (L1DescID == L1DescEnd) {
                L2DescEnd = AMBA_TLB_GET_L2_LARGE_PAGE_ID(pMapInfo->VirtAddr + pMapInfo->MemSize - 1U);
            } else {
                L2DescEnd = (AMBA_NUM_TLB_L2_SHORT_DESC >> 4U) - 1U;
            }

            AmbaMisra_TypeCast32(&pL2DescTmp, &pL2Desc);
            pL2DescTmp = &pL2DescTmp[L2DescStart];
            for (L2DescID = L2DescStart; L2DescID <= L2DescEnd; L2DescID++) {
                /* repeat in 16 consecutive memory locations */
                TmpL2Desc.BaseAddr = (UINT16)L2LargePageBase;
                AmbaMisra_TypeCast32(&L2DescTmp, &TmpL2Desc);
                pL2DescTmp->Data = L2DescTmp.Data;
                pL2DescTmp++;
                pL2DescTmp->Data = L2DescTmp.Data;
                pL2DescTmp++;
                pL2DescTmp->Data = L2DescTmp.Data;
                pL2DescTmp++;
                pL2DescTmp->Data = L2DescTmp.Data;
                pL2DescTmp++;
                pL2DescTmp->Data = L2DescTmp.Data;
                pL2DescTmp++;
                pL2DescTmp->Data = L2DescTmp.Data;
                pL2DescTmp++;
                pL2DescTmp->Data = L2DescTmp.Data;
                pL2DescTmp++;
                pL2DescTmp->Data = L2DescTmp.Data;
                pL2DescTmp++;
                pL2DescTmp->Data = L2DescTmp.Data;
                pL2DescTmp++;
                pL2DescTmp->Data = L2DescTmp.Data;
                pL2DescTmp++;
                pL2DescTmp->Data = L2DescTmp.Data;
                pL2DescTmp++;
                pL2DescTmp->Data = L2DescTmp.Data;
                pL2DescTmp++;
                pL2DescTmp->Data = L2DescTmp.Data;
                pL2DescTmp++;
                pL2DescTmp->Data = L2DescTmp.Data;
                pL2DescTmp++;
                pL2DescTmp->Data = L2DescTmp.Data;
                pL2DescTmp++;
                pL2DescTmp->Data = L2DescTmp.Data;
                pL2DescTmp++;

                L2LargePageBase++;
            }
            (void)AmbaRTSL_CacheFlushDataPtr(&pL2Desc[L2DescStart], ((L2DescEnd - L2DescStart) + 1U) * 16U * sizeof(AMBA_TLB_S_DESC_L2_LARGE_PG_s));
        }
    }

    return RetVal;
}

/**
 *  MMU_TlbSetupL2SmallPage - Setup short-descriptor L2 small pagetables.
 *  @param[in] pMapInfo Pointer to user-supplied MMU map entry.
 *  @return error code
 */
static UINT32 MMU_TlbSetupL2SmallPage(const AMBA_MMU_MAP_INFO32_s *pMapInfo)
{
    AMBA_TLB_S_DESC_L1_PG_TBL_s *pL1Desc;
    AMBA_TLB_S_DESC_L2_SMALL_PG_s *pL2Desc;
    AMBA_TLB_S_DESC_L2_SMALL_PG_s TmpL2Desc = {0};

    const AMBA_TLB_S_DESC_L1_s *pL1Tmp;
    AMBA_TLB_S_DESC_L2_s *pL2DescTmp;
    AMBA_TLB_S_DESC_L2_s L2DescTmp = {0};
    UINT32 DescPhysAddr = 0U, DescVirtAddr;
    UINT32 L1DescStart, L1DescEnd;
    UINT32 L2DescStart, L2DescEnd;
    UINT32 L2SmallPageBase;
    UINT32 L1DescID, L2DescID;
    UINT32 RetVal = MMU_ERR_NONE;
    AMBA_MMU_MAP_ATTR32_s MemAttrBits;

    AmbaMisra_TypeCast32(&MemAttrBits, &pMapInfo->MemAttr);

    L1DescStart     = AMBA_TLB_GET_L1_SECT_ID(pMapInfo->VirtAddr);
    L1DescEnd       = AMBA_TLB_GET_L1_SECT_ID(pMapInfo->VirtAddr + pMapInfo->MemSize - 1U);
    L2SmallPageBase = AMBA_TLB_L2_SMALL_PAGE_BASE(pMapInfo->PhysAddr);

    if (L1DescEnd >= AMBA_NUM_TLB_L1_SHORT_DESC) {
        RetVal = MMU_ERR_ARG;
    } else {
        pL2Desc = &TmpL2Desc;
        pL2Desc->DescType = (UINT8)((UINT8)AMBA_TLB_L2_SHORT_DESC_SMALL_PAGE | (UINT8)MemAttrBits.UXN);

        if (MemAttrBits.MemAttr == 0x00U) {         // Device-nGnRnE
            pL2Desc->TEX      = 0U;
            pL2Desc->Cached   = 0U;
            pL2Desc->Buffered = 0U;
        } else if (MemAttrBits.MemAttr == 0x04U) {  // Device-nGnRE
            pL2Desc->TEX      = (MemAttrBits.Shareability < 2U) ? 2U : 0U;
            pL2Desc->Cached   = 0U;
            pL2Desc->Buffered = (MemAttrBits.Shareability < 2U) ? 0U : 1U;
        } else if (MemAttrBits.MemAttr == 0x77U) {  // Normal Memory, Write-Back Cacheable
            /* For Cortex-A53, the memory is cached in the L1 Data cache and the L2 cache iff it is marked as Inner and Outer Write-Back Cacheable. */
            pL2Desc->TEX      = 1U;
            pL2Desc->Cached   = 1U;
            pL2Desc->Buffered = 1U;
        } else {                                    // Normal Memory, Non-Cacheable
            /* Cortex-A53 doesn't support Write-Through Cacheable */
            pL2Desc->TEX      = 1U;
            pL2Desc->Cached   = 0U;
            pL2Desc->Buffered = 0U;
        }

        pL2Desc->Shared    = (MemAttrBits.Shareability < 2U) ? 0U : 1U;
        pL2Desc->AP        = (UINT8)(((UINT8)(MemAttrBits.S1AP) << (UINT8)1U) | (UINT8)0x1U);    /* AP[0] is access flag */
        pL2Desc->AP2       = (UINT8)(MemAttrBits.S1AP >> 1U);
        pL2Desc->NotGlobal = MemAttrBits.NotGlobal;

        for (L1DescID = L1DescStart; L1DescID <= L1DescEnd; L1DescID++) {
            pL1Tmp = &pAmbaTlbDescS->L1Desc[L1DescID];
            AmbaMisra_TypeCast32(&pL1Desc, &pL1Tmp);

            if (pL1Desc->DescType == AMBA_TLB_L1_SHORT_DESC_SECTION) {
                /* This region is already mapped by L1 Section/Supersection descriptor. */
                break;
            } else if (pL1Desc->DescType == AMBA_TLB_L1_SHORT_DESC_PAGE_TABLE) {
                /* Use the page table base address described in L1 Page Table descriptor. */
                DescPhysAddr = (pAmbaTlbDescS->L1Desc[L1DescID].Data & AMBA_TLB_L1_PAGE_TABLE_BASE_MASK);
                (void)AmbaRTSL_MmuPhys32ToVirt32(DescPhysAddr, &DescVirtAddr);
                AmbaMisra_TypeCast32(&pL2Desc, &DescVirtAddr);
                (void)AmbaRTSL_CacheFlushDataPtr(&pL2Desc, sizeof(AMBA_TLB_S_DESC_L2_SMALL_PG_s *));
            } else {
                pL2DescTmp = pAmbaTlbDescS->L2Desc[L1DescID];
                AmbaMisra_TypeCast32(&pL2Desc, &pL2DescTmp);
                MMU_ZeroMemory(pL2Desc, AMBA_NUM_TLB_L2_SHORT_DESC * sizeof(AMBA_TLB_S_DESC_L2_SMALL_PG_s));
                (void)AmbaRTSL_CacheCleanDataPtr(pL2Desc, AMBA_NUM_TLB_L2_SHORT_DESC * sizeof(AMBA_TLB_S_DESC_L2_SMALL_PG_s));

                /* Setup a new L1 Page Table descriptor. */
                AmbaMisra_TypeCast32(&DescVirtAddr, &pL2Desc);
                (void)AmbaRTSL_MmuVirt32ToPhys32(DescVirtAddr, &DescPhysAddr);
                pL1Desc->DescType   = AMBA_TLB_L1_SHORT_DESC_PAGE_TABLE;
                pL1Desc->NonSecure  = MemAttrBits.NonSecure;
                pL1Desc->Domain     = MemAttrBits.Domain;
                pL1Desc->BaseAddr   = AMBA_TLB_L1_PAGE_TABLE_BASE(DescPhysAddr);
                (void)AmbaRTSL_CacheFlushDataPtr(pL1Desc, sizeof(AMBA_TLB_S_DESC_L1_PG_TBL_s));
            }

            if (L1DescID == L1DescStart) {
                L2DescStart = AMBA_TLB_GET_L2_SMALL_PAGE_ID(pMapInfo->VirtAddr);
            } else {
                L2DescStart = 0U;
            }

            if (L1DescID == L1DescEnd) {
                L2DescEnd = AMBA_TLB_GET_L2_SMALL_PAGE_ID(pMapInfo->VirtAddr + pMapInfo->MemSize - 1U);
            } else {
                L2DescEnd = AMBA_NUM_TLB_L2_SHORT_DESC - 1U;
            }

            AmbaMisra_TypeCast32(&pL2DescTmp,&pL2Desc);
            pL2DescTmp = &pL2DescTmp[L2DescStart];
            AmbaMisra_TypeCast32(&L2DescTmp, &TmpL2Desc);
            for (L2DescID = L2DescStart; L2DescID <= L2DescEnd; L2DescID++) {
                pL2DescTmp->Data = L2DescTmp.Data;
                pL2Desc[L2DescID].BaseAddr = L2SmallPageBase;
                pL2DescTmp++;
                L2SmallPageBase++;
            }
            (void)AmbaRTSL_CacheFlushDataPtr(&pL2Desc[L2DescStart], ((L2DescEnd - L2DescStart) + 1U) * sizeof(AMBA_TLB_S_DESC_L2_SMALL_PG_s));
        }
    }

    return RetVal;
}

/**
 *  MMU_TlbSetup - Setup MMU page table according to user-supplied MMU map entry.
 *  @param[in] pMapInfo Pointer to user-supplied MMU map entry.
 *  @return error code
 */
static UINT32 MMU_TlbSetup(AMBA_MMU_MAP_INFO32_s *pMapInfo)
{
    UINT32 TTBCR;
    UINT32 RetVal = MMU_ERR_NONE;
    AMBA_MMU_MAP_ATTR32_s MemAttrBits;

#ifndef AMBA_KAL_NO_SMP
    AmbaMisra_TypeCast32(&MemAttrBits, &pMapInfo->MemAttr);
    MemAttrBits.Shareability = 2U;  /* Make cacheable region as outer shareable. (Device and non-cacheable regions are already outer shareable.) */
    AmbaMisra_TypeCast32(&pMapInfo->MemAttr, &MemAttrBits);
#endif

    TTBCR = AmbaASM_ReadTlbBaseCtrlA32();

    if (0x0U != (TTBCR & ((UINT32)1U << 31U))) {
        /* TTBCR.EAE = 1. Use the 40-bit translation system, with the Long-descriptor translation table format. */
        if (MemAttrBits.DescType == AMBA_MMU_MAP_ATTR_L1_BLOCK) {
            RetVal = MMU_TlbSetupL1Block(pMapInfo);
        } else if (MemAttrBits.DescType == AMBA_MMU_MAP_ATTR_L2_BLOCK) {
            RetVal = MMU_TlbSetupL2Block(pMapInfo);
        } else if (MemAttrBits.DescType == AMBA_MMU_MAP_ATTR_L3_PAGE) {
            RetVal = MMU_TlbSetupL3Page(pMapInfo);
        } else {
            RetVal = MMU_ERR_ARG;
        }
    } else {
        /* TTBCR.EAE = 0. Use the 32-bit translation system, with the Short-descriptor translation table format. */
        if (MemAttrBits.DescType == AMBA_MMU_MAP_ATTR_SECTION) {
            RetVal = MMU_TlbSetupL1Section(pMapInfo);
        } else if (MemAttrBits.DescType == AMBA_MMU_MAP_ATTR_LARGE_PAGE) {
            RetVal = MMU_TlbSetupL2LargePage(pMapInfo);
        } else if (MemAttrBits.DescType == AMBA_MMU_MAP_ATTR_SMALL_PAGE) {
            RetVal = MMU_TlbSetupL2SmallPage(pMapInfo);
        } else {
            RetVal = MMU_ERR_ARG;
        }
    }

    return RetVal;
}

/**
 *  AmbaRTSL_MmuVirt32ToPhys32 - Convert virtual address to physical address.
 *  @param[in] VirtAddr virtual address
 *  @param[out] pPhysAddr pointer that store tranfered address
 *  @return error code
 */
UINT32 AmbaRTSL_MmuVirt32ToPhys32(UINT32 VirtAddr, UINT32 *pPhysAddr)
{
    const AMBA_MMU_MAP_INFO32_s *pMapInfo = AmbaMmuMmioMap;
    UINT32 NumRegion = AMBA_MMU_NUM_MMIO_REGION;
    UINT32 i, RetVal = MMU_ERR_NOTLB;
    UINT32 SysCtrlReg = AmbaRTSL_CpuReadSysCtrl();

    if (pPhysAddr == NULL) {
        RetVal = MMU_ERR_ARG;
    } else {
        /* search in ioMap first. */
        for (i = 0U; i < NumRegion; i++) {
            if ((VirtAddr >= pMapInfo->VirtAddr) &&
                (VirtAddr < (pMapInfo->VirtAddr + pMapInfo->MemSize))) {
                *pPhysAddr = (VirtAddr - pMapInfo->VirtAddr) + pMapInfo->PhysAddr;
                RetVal = MMU_ERR_NONE;
                break;
            }
            pMapInfo++;
        }

        if (RetVal == MMU_ERR_NOTLB) {
            /* search failed in ioMap and then search in user map. */
            pMapInfo = AmbaMmuUserConfig.pMemMap;
            NumRegion = AmbaMmuUserConfig.NumMemMap;

            if (pMapInfo != NULL) {
                for (i = 0U; i < NumRegion; i++) {
                    if ((VirtAddr >= pMapInfo->VirtAddr) &&
                        (VirtAddr < (pMapInfo->VirtAddr + pMapInfo->MemSize))) {
                        *pPhysAddr = (VirtAddr - pMapInfo->VirtAddr) + pMapInfo->PhysAddr;
                        RetVal = MMU_ERR_NONE;
                        break;
                    }
                    pMapInfo++;
                }
            }
        }
    }

    /* Although the TLB traversal is done, we still inform caller about the MMU is disabled. */
    if ((RetVal == MMU_ERR_NONE) && ((SysCtrlReg & 0x1U) == 0U)) {
        RetVal = MMU_ERR_NOMMU;
    }

    return RetVal;
}

/**
 *  AmbaRTSL_MmuPhys32ToVirt32 - Convert physical address to virtual address.
 *  @param[in] PhysAddr physical address
 *  @param[out] pVirtAddr pointer that store tranfered address
 *  @return error code
 */
UINT32 AmbaRTSL_MmuPhys32ToVirt32(UINT32 PhysAddr, UINT32 *pVirtAddr)
{
    const AMBA_MMU_MAP_INFO32_s *pMapInfo = AmbaMmuMmioMap;
    UINT32 NumRegion = AMBA_MMU_NUM_MMIO_REGION;
    UINT32 i, RetVal = MMU_ERR_NOTLB;
    UINT32 SysCtrlReg = AmbaRTSL_CpuReadSysCtrl();

    if (pVirtAddr == NULL) {
        RetVal = MMU_ERR_ARG;
    } else {
        /* search in ioMap first. */
        for (i = 0U; i < NumRegion; i++) {
            if ((PhysAddr >= pMapInfo->PhysAddr) &&
                (PhysAddr < (pMapInfo->PhysAddr + pMapInfo->MemSize))) {
                *pVirtAddr = (PhysAddr - pMapInfo->PhysAddr) + pMapInfo->VirtAddr;
                RetVal = MMU_ERR_NONE;
                break;
            }
            pMapInfo++;
        }

        if (RetVal == MMU_ERR_NOTLB) {
            /* search failed in ioMap and then search in user map. */
            pMapInfo = AmbaMmuUserConfig.pMemMap;
            NumRegion = AmbaMmuUserConfig.NumMemMap;

            if (pMapInfo != NULL) {
                for (i = 0U; i < NumRegion; i++) {
                    if ((PhysAddr >= pMapInfo->PhysAddr) &&
                        (PhysAddr < (pMapInfo->PhysAddr + pMapInfo->MemSize))) {
                        *pVirtAddr = (PhysAddr - pMapInfo->PhysAddr) + pMapInfo->VirtAddr;
                        RetVal = MMU_ERR_NONE;
                        break;
                    }
                    pMapInfo++;
                }
            }
        }
    }

    /* Although the TLB traversal is done, we still inform caller about the MMU is disabled. */
    if ((RetVal == MMU_ERR_NONE) && ((SysCtrlReg & 0x1U) == 0U)) {
        RetVal = MMU_ERR_NOMMU;
    }

    return RetVal;
}

/**
 *  AmbaRTSL_MmuQueryMapInfo32 - The function performs the translation of virtual memory addresses to physical addresses.
 *  @param[in] VirtAddr virtual address
 *  @param[out] pMemMap pointer to the mapping and attributes of a memory region
 *  @return error code
 */
UINT32 AmbaRTSL_MmuQueryMapInfo32(UINT32 VirtAddr, AMBA_MMU_MAP_INFO32_s * pMemMap)
{
    const AMBA_MMU_MAP_INFO32_s *pMapInfo = AmbaMmuMmioMap;
    UINT32 NumRegion = AMBA_MMU_NUM_MMIO_REGION;
    UINT32 i, RetVal = MMU_ERR_NOTLB;
    UINT32 SysCtrlReg = AmbaRTSL_CpuReadSysCtrl();

    if (pMemMap == NULL) {
        RetVal = MMU_ERR_ARG;
    } else {
        /* search in ioMap first. */
        for (i = 0U; i < NumRegion; i++) {
            if ((VirtAddr >= pMapInfo->VirtAddr) &&
                (VirtAddr < (pMapInfo->VirtAddr + pMapInfo->MemSize))) {
                pMemMap->VirtAddr = pMapInfo->VirtAddr;
                pMemMap->PhysAddr = pMapInfo->PhysAddr;
                pMemMap->MemSize = pMapInfo->MemSize;
                pMemMap->MemAttr = pMapInfo->MemAttr;
                RetVal = MMU_ERR_NONE;
                break;
            }
            pMapInfo++;
        }

        if (RetVal == MMU_ERR_NOTLB) {
            /* search failed in ioMap and then search in user map. */
            pMapInfo = AmbaMmuUserConfig.pMemMap;
            NumRegion = AmbaMmuUserConfig.NumMemMap;

            if (pMapInfo != NULL) {
                for (i = 0U; i < NumRegion; i++) {
                    if ((VirtAddr >= pMapInfo->VirtAddr) &&
                        (VirtAddr < (pMapInfo->VirtAddr + pMapInfo->MemSize))) {
                        pMemMap->VirtAddr = pMapInfo->VirtAddr;
                        pMemMap->PhysAddr = pMapInfo->PhysAddr;
                        pMemMap->MemSize = pMapInfo->MemSize;
                        pMemMap->MemAttr = pMapInfo->MemAttr;
                        RetVal = MMU_ERR_NONE;
                        break;
                    }
                    pMapInfo++;
                }
            }
        }
    }

    /* Although the TLB traversal is done, we still inform caller about the MMU is disabled. */
    if ((RetVal == MMU_ERR_NONE) && ((SysCtrlReg & 0x1U) == 0U)) {
        RetVal = MMU_ERR_NOMMU;
    }

    return RetVal;
}

/**
 *  AmbaRTSL_MmuCpuSetupA32 - Initialise the processor for turning the MMU on.
 *  @param[in] HighVectFlag A flag to indicate high vector or not
 *  @return error code
 */
UINT32 AmbaRTSL_MmuCpuSetupA32(UINT8 HighVectFlag)
{
#ifdef CONFIG_MMU_USE_SHORT_DESCRIPTOR  /* Use short-descriptor */
    const AMBA_TLB_S_DESC_s *pPageTblBase = &AmbaMmuPageTable;
    UINT32 TTBR, TTBCR, SCTLR, VBAR = (HighVectFlag != 0U) ? 0xFFFF0000U : 0x0U;

#if defined(CONFIG_XEN_SUPPORT) && !defined(AMBA_FWPROG)
    {
#if 0 // TODO
        extern UINT32 *__xen_zimage_header;
        extern UINT32 *__vector;
        VBAR += ((UINT32) &__vector - (UINT32) &__xen_zimage_header);
#else
        extern UINT32 __vector;
        const UINT32 *pTmp = &__vector;
        AmbaMisra_TypeCast(&VBAR, &pTmp);
#endif
    }
#endif

    /* Use the 32-bit translation system, with the Short-descriptor translation table format */
    TTBCR = 0U;                             /* The width of the base address is 16KB aligned */
    AmbaMisra_TypeCast32(&TTBR, &pPageTblBase);

    AmbaASM_WriteVectorBaseAddrA32(VBAR);      /* VBAR */

    /* Stage 1 translation control */
    AmbaASM_WriteTlbBaseCtrlA32(TTBCR);        /* TTBCR */
    AmbaASM_WriteTlbBase0_32(TTBR);         /* TTBR0 */

    SCTLR = AmbaASM_ReadSysCtrlA32();
    SCTLR |= ((UINT32)1U << 29U);           /* SCTLR.AFE = 1 -> Use access flag. */
    SCTLR &= ~((UINT32)1U << 28U);          /* SCTLR.TRE = 0 -> Not use PRRR & NMRR. */
    AmbaASM_WriteSysCtrlA32(SCTLR);            /* SCTLR */

    /* Access permission of domain 0 is defined in translation table short-descriptors. */
    AmbaASM_WriteDomainAccessCtrl(0x1U);

    AMBA_DSB();
    AMBA_ISB();

#else /* !defined(CONFIG_MMU_USE_SHORT_DESCRIPTOR). Use long-descriptor */
    const AMBA_TLB_S_DESC_s *pPageTblBase = &AmbaMmuPageTable;
    UINT64 TTBR;
    UINT32 TTBCR, MAIR, VBAR = (HighVectFlag != 0U) ? 0xFFFF0000U : 0x0U;

#if defined(CONFIG_XEN_SUPPORT)
    if (AmbaXen_is_guest()) {
        extern UINT32 *__xen_zimage_header;
        extern UINT32 *__vector;
        VBAR += ((UINT32) &__vector - (UINT32) &__xen_zimage_header);
    }
#endif

    TTBCR = 0U;                         /* Both T0SZ and T1SZ are all zeros. TTBR0 points to level 1 for all addresses */
    TTBCR |= ((UINT32)0x5U << 8U);      /* Outer & Inner Write-Back Write-Allocate Cacheable */
    TTBCR |= ((UINT32)0x1U << 31U);     /* Use the 40-bit translation system, with the Long-descriptor translation table format. */

    MAIR =  ((UINT32)0x00U);            /* Device-nGnRnE (ARMv7's Strongly-ordered) */
    MAIR |= ((UINT32)0x04U << 0x08U);   /* Device-nGnRE (ARMv7's Device) */
    MAIR |= ((UINT32)0x44U << 0x10U);   /* Normal Memory, Outer & Inner Non-Cacheable */
    MAIR |= ((UINT32)0x77U << 0x18U);   /* Normal Memory, Outer & Inner Write-back non-transient */

    AmbaMisra_TypeCast64(&TTBR, &pPageTblBase);
    AmbaASM_WriteVectorBaseAddrA32(VBAR);      /* VBAR */

    /* Stage 1 translation control */
    AmbaASM_WriteTlbBaseCtrlA32(TTBCR);        /* TTBCR */
    AmbaASM_WriteTlbBase0_64(TTBR);         /* TTBR0 */

    /* Memory region attributes */
    AmbaASM_WriteMemAttrIndirect0(MAIR); /* MAIR0 */

    AmbaMisra_TouchUnused(&MAIR);
#endif
    AmbaMisra_TouchUnused(&VBAR);
    AmbaMisra_TouchUnused(&TTBCR);

    return MMU_ERR_NONE;
}

/**
 *  AmbaRTSL_MmuTlbInvalidateAllA32 - Invalidate all entries in the TLB.
 */
void AmbaRTSL_MmuTlbInvalidateAllA32(void)
{
    AmbaTLB_InvUniTlbAll();
}

/**
 *  AmbaRTSL_MmuTlbConfigA32 - Initialize translation table by descriptors
 */
void AmbaRTSL_MmuTlbConfigA32(void)
{
    AMBA_MMU_MAP_INFO32_s *pMapInfo;
    UINT32 i, NumRegion;

    if (AmbaMmuUserConfig.UserPreConfigEntry != NULL) {
        (void)AmbaMmuUserConfig.UserPreConfigEntry();   /* invoke the init function of RAM map */
    }

    MMU_MmioMapInit(&AmbaMmuMmioMap[0]);

    if (AmbaMmuUserConfig.VectorAddr == 0x0U) {
        /* 0x00000000U: Low vector address */
        (void)AmbaRTSL_MmuCpuSetupA32(0U);
    } else {
        /* 0xFFFF0000U: High vector address */
        (void)AmbaRTSL_MmuCpuSetupA32(1U);
    }

    MMU_TlbReset();

#if !defined(CONFIG_XEN_SUPPORT)
    /* Setup RAM address map */
    pMapInfo = AmbaMmuUserConfig.pMemMap;               /* pointer to the base address of Memory Map Info */
    NumRegion = AmbaMmuUserConfig.NumMemMap;
    if (pMapInfo != NULL) {
        for (i = 0U; i < NumRegion; i++) {
            (void)MMU_TlbSetup(pMapInfo);
            pMapInfo++;
        }
    }
#endif

    /* Setup MMIO address map */
    pMapInfo = AmbaMmuMmioMap;
    NumRegion = AMBA_MMU_NUM_MMIO_REGION;
    for (i = 0U; i < NumRegion; i++) {
        (void)MMU_TlbSetup(pMapInfo);
        pMapInfo++;
    }

    AMBA_DMB();
}

/**
 *  AmbaRTSL_MmuRegLock - Change specified memory region to read only
 *  @param[in] RegionId region ID
 *  @return error code
 */
UINT32 AmbaRTSL_MmuRegLock(UINT32 RegionId)
{
    AMBA_MMU_MAP_INFO32_s *pMapInfo;
    UINT32 RetVal = MMU_ERR_NONE;

    if (RegionId < AmbaMmuUserConfig.NumMemMap) {
        AmbaCache_InstInvAll();
        AmbaRTSL_CacheFlushDataAll();
        AmbaRTSL_MmuTlbInvalidateAllA32();

        AMBA_DSB();
        AMBA_ISB();

        pMapInfo = &AmbaMmuUserConfig.pMemMap[RegionId];
        pMapInfo->MemAttr = pMapInfo->MemAttr & (~AMBA_MMU_MAP_CACHE_MEM_READ_WRITE_ATTR);
        pMapInfo->MemAttr = pMapInfo->MemAttr | AMBA_MMU_MAP_CACHE_MEM_READ_ONLY_ATTR;
        (void)MMU_TlbSetup(pMapInfo);

        /* Invalidate TLB if access flag enable of system control register is changed */
        AmbaRTSL_MmuTlbInvalidateAllA32();
    } else {
        RetVal = MMU_ERR_ARG;
    }

    return RetVal;
}

