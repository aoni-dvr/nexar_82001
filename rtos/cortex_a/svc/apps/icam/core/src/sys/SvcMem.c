/**
*  @file SvcMem.c
 *
 * Copyright (c) [2020] Ambarella International LP
 *
 * This file and its contents ("Software") are protected by intellectual
 * property rights including, without limitation, U.S. and/or foreign
 * copyrights. This Software is also the confidential and proprietary
 * information of Ambarella International LP and its licensors. You may not use, reproduce,
 * disclose, distribute, modify, or otherwise prepare derivative works of this
 * Software or any portion thereof except pursuant to a signed license agreement
 * or nondisclosure agreement with Ambarella International LP or its authorized affiliates.
 * In the absence of such an agreement, you agree to promptly notify and return
 * this Software to Ambarella International LP.
 *
 * This file includes sample code and is only for internal testing and evaluation.  If you
 * distribute this sample code (whether in source, object, or binary code form), it will be
 * without any warranty or indemnity protection from Ambarella International LP or its affiliates.
 *
 * THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF NON-INFRINGEMENT,
 * MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL AMBARELLA INTERNATIONAL LP OR ITS AFFILIATES BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; COMPUTER FAILURE OR MALFUNCTION; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
*
*  @details memory layout functions
*
*/

#include "AmbaKAL.h"
#include "AmbaDef.h"
#include "AmbaPrint.h"
#include "AmbaWrap.h"
#include "AmbaMemProt.h"
#include "AmbaMMU.h"
#include "AmbaSvcWrap.h"

#include "SvcErrCode.h"
#include "SvcLog.h"
#include "SvcMem.h"

static SVC_MEM_s *g_pMem = NULL;

static UINT32 MapUpdate(SVC_MEM_s *pMem, SVC_MEM_MAP_s *pMap, ULONG *pCurrAddr)
{
    UINT32                i, r, Rval = SVC_OK, Left;
    UINT32                ReqIdx = 0U, ReqNum, PoolId;
    SVC_MEM_REQ_s         *pReq;
    const SVC_MEM_POOL_s  *pPool;

    for (i = 0U; i < SVC_MEM_REG_MAX; i++) {
        if (pMap->ReqNum[i] == 0U) {
            continue;
        }

        switch (i) {
        case SVC_MEM_REG_NC:
            PoolId = SVC_MEM_TYPE_NC;
            break;
        case SVC_MEM_REG_CV:
            PoolId = SVC_MEM_TYPE_CV;
            break;
        #if defined(SVC_MEM_TYPE_DSP)
        case SVC_MEM_REG_DSP:
            PoolId = SVC_MEM_TYPE_DSP;
            break;
        #else
        case SVC_MEM_REG_DSP:
            PoolId = SVC_MEM_TYPE_CA;
            break;
        #endif
        #if defined(SVC_MEM_TYPE_IO)
        case SVC_MEM_REG_IO:
            PoolId = SVC_MEM_TYPE_IO;
            break;
        #else
        case SVC_MEM_REG_IO:
            PoolId = SVC_MEM_TYPE_CA;
            break;
        #endif
        case SVC_MEM_REG_CA:
        default:
            PoolId = SVC_MEM_TYPE_CA;
            break;
        }

        pPool = &(pMem->Pool[PoolId]);
        pMap->RegBase[i] = pCurrAddr[PoolId];

        ReqNum = pMap->ReqNum[i];
        pReq = pMap->pReqArr[i];

        for (r = 0U; r < ReqNum; r++) {
            pCurrAddr[PoolId] = AmbaSvcWrap_GetAlignedAddr(pCurrAddr[PoolId], pReq[r].Alignment);
            Left = (UINT32)(pPool->BaseAddr + pPool->Size - pCurrAddr[PoolId]);

            if (pReq[r].ReqSize == SVC_MEM_REQ_LEFT) {
                pReq[r].ReqSize = GetRoundDownValU32(Left, pReq[r].Alignment);
                pReq[r].ReqSize *= pReq[r].Alignment;
            }

            if (pReq[r].ReqSize <= Left) {
                pReq[r].ReqPriv = pCurrAddr[PoolId];

                pCurrAddr[PoolId] += pReq[r].ReqSize;
            } else {
                Left = pReq[r].ReqSize - Left;
                Rval = SVC_NG;

                if (i == SVC_MEM_REG_NC) {
                    SvcLog_NG(SVC_LOG_SYS, "## [%u] non-cache short 0x%X", pReq[r].MemId, Left);
                } else if (i == SVC_MEM_REG_CA) {
                    SvcLog_NG(SVC_LOG_SYS, "## [%u] cache short 0x%X", pReq[r].MemId, Left);
                    SvcLog_NG(SVC_LOG_SYS, "## please increase CONFIG_SVC_CACHE_HEAP_SIZE", 0U, 0U);
                } else if (i == SVC_MEM_REG_CV) {
                    SvcLog_NG(SVC_LOG_SYS, "## [%u] cv user short 0x%X", pReq[r].MemId, Left);
                    SvcLog_NG(SVC_LOG_SYS, "## please increase CONFIG_CV_MEM_SIZE", 0U, 0U);
                } else if (i == SVC_MEM_REG_DSP) {
                    SvcLog_NG(SVC_LOG_SYS, "## [%u] dsp user short 0x%X", pReq[r].MemId, Left);
                    SvcLog_NG(SVC_LOG_SYS, "## please increase", 0U, 0U);
                    SvcLog_NG(SVC_LOG_SYS, "##  CONFIG_DSP_DATA_BUF_SIZE (CONFIG_DEVICE_TREE_SUPPORT=y) else", 0U, 0U);
                    SvcLog_NG(SVC_LOG_SYS, "##  CONFIG_SVC_CACHE_HEAP_SIZE", 0U, 0U);
                } else {
                    SvcLog_NG(SVC_LOG_SYS, "## [%u] io user short 0x%X", pReq[r].MemId, Left);
                    SvcLog_NG(SVC_LOG_SYS, "## please increase", 0U, 0U);
                    SvcLog_NG(SVC_LOG_SYS, "##  CONFIG_SVC_IO_HEAP_SIZE (CONFIG_PIO_MEM_ALLOC=y) else", 0U, 0U);
                    SvcLog_NG(SVC_LOG_SYS, "##  CONFIG_SVC_CACHE_HEAP_SIZE", 0U, 0U);
                }
                break;
            }
        }

        if (Rval == SVC_OK) {
            pMap->RegSize[i] = (UINT32)(pCurrAddr[PoolId] - pMap->RegBase[i]);
        } else {
            break;
        }
        ReqIdx += ReqNum;
    }

    AmbaMisra_TouchUnused(pMem);
    return Rval;
}

/**
* initialization of memory pool
* @param [in] pMem info block of memory
* @return none
*/
void SvcMem_Init(SVC_MEM_s *pMem)
{
    UINT32  i;

    for (i = 0U; i < SVC_MEM_TYPE_MAX; i++) {
        pMem->Pool[i].CurrAddr = pMem->Pool[i].BaseAddr;
    }

    if (SVC_OK != AmbaWrap_memset(&(pMem->Priv), 0, sizeof(SVC_MEM_PRIV_s))) {
        SvcLog_NG(SVC_LOG_SYS, "## Fail to memset", 0U, 0U);
    }

    g_pMem = pMem;
}

/**
* configuration of fixed memory pool
* @param [in] pMem info block of fixed memory pool
* @param [in] pMap map of fixed memory pool
* @return 0-OK, 1-NG
*/
UINT32 SvcMem_FixedMapConfig(SVC_MEM_s *pMem, SVC_MEM_MAP_s *pMap)
{
    UINT32  i, Rval;
    ULONG   CurrAddr[SVC_MEM_TYPE_MAX];

    for (i = 0U; i < SVC_MEM_TYPE_MAX; i++) {
        CurrAddr[i] = pMem->Pool[i].CurrAddr;
    }

    Rval = MapUpdate(pMem, pMap, CurrAddr);
    if (Rval == SVC_OK) {
        pMem->Priv.pFixedMap = pMap;

        for (i = 0U; i < SVC_MEM_TYPE_MAX; i++) {
            pMem->Pool[i].CurrAddr = CurrAddr[i];
        }
    }

    return Rval;
}

/**
* configuration of shared memory pool
* @param [in] pMem info block of shared memory pool
* @param [in] MapNum number of shared memory pool map
* @param [in] pMapArr map array of shared memory pool
* @return 0-OK, 1-NG
*/
UINT32 SvcMem_SharedMapConfig(SVC_MEM_s *pMem, UINT32 MapNum, SVC_MEM_MAP_s *pMapArr)
{
    UINT32               i, MapIdx, Rval;
    ULONG                CurrAddr[SVC_MEM_TYPE_MAX];
    ULONG                MaxSize[SVC_MEM_TYPE_MAX];
    const SVC_MEM_MAP_s  *pFixedMap;

    if (AmbaWrap_memset(CurrAddr, 0, sizeof(CurrAddr)) != SVC_OK) {
        SvcLog_NG(SVC_LOG_SYS, "## Fail to memset", 0U, 0U);
    }

    if (AmbaWrap_memset(MaxSize, 0, sizeof(MaxSize)) != SVC_OK) {
        SvcLog_NG(SVC_LOG_SYS, "## Fail to memset", 0U, 0U);
    }

    pFixedMap = pMem->Priv.pFixedMap;
    if (pFixedMap != NULL) {
        pMem->Pool[SVC_MEM_TYPE_CA].CurrAddr  = pFixedMap->RegBase[SVC_MEM_REG_CA] +
                                                pFixedMap->RegSize[SVC_MEM_REG_CA];
        pMem->Pool[SVC_MEM_TYPE_NC].CurrAddr  = pFixedMap->RegBase[SVC_MEM_REG_NC] +
                                                pFixedMap->RegSize[SVC_MEM_REG_NC];
        pMem->Pool[SVC_MEM_TYPE_CV].CurrAddr  = pFixedMap->RegBase[SVC_MEM_REG_CV] +
                                                pFixedMap->RegSize[SVC_MEM_REG_CV];
        #if defined(SVC_MEM_TYPE_DSP)
        pMem->Pool[SVC_MEM_TYPE_DSP].CurrAddr = pFixedMap->RegBase[SVC_MEM_REG_DSP] +
                                                pFixedMap->RegSize[SVC_MEM_REG_DSP];
        #endif
        #if defined(SVC_MEM_TYPE_IO)
        pMem->Pool[SVC_MEM_TYPE_IO].CurrAddr  = pFixedMap->RegBase[SVC_MEM_REG_IO] +
                                                pFixedMap->RegSize[SVC_MEM_REG_IO];
        #endif
    }

    Rval = SVC_NG;
    for (MapIdx = 0U; MapIdx < MapNum; MapIdx++) {
        for (i = 0U; i < SVC_MEM_TYPE_MAX; i++) {
            CurrAddr[i] = pMem->Pool[i].CurrAddr;
        }

        Rval = MapUpdate(pMem, &(pMapArr[MapIdx]), CurrAddr);
        if (Rval == SVC_OK) {
            if (AmbaWrap_memset(MaxSize, 0, sizeof(MaxSize)) != SVC_OK) {
                SvcLog_NG(SVC_LOG_SYS, "## Fail to memset", 0U, 0U);
            }

            for (i = 0U; i < SVC_MEM_TYPE_MAX; i++) {
                MaxSize[i] = (((MaxSize[i]) > (CurrAddr[i])) ? (MaxSize[i]) : (CurrAddr[i]));
            }
        } else {
            break;
        }
    }

    if (Rval == SVC_OK) {
        for (i = 0U; i < SVC_MEM_TYPE_MAX; i++) {
            pMem->Pool[i].CurrAddr = MaxSize[i];
        }
        pMem->Priv.pSharedMapArr = pMapArr;
        pMem->Priv.SharedMapNum = MapNum;
    }

    return Rval;
}

/**
* map switch of shared memory pool
* @param [in] pMem info block of shared memory pool
* @param [in] MapIdx index of map
* @return none
*/
void SvcMem_SharedMapSwitch(SVC_MEM_s *pMem, UINT32 MapIdx)
{
    pMem->Priv.SharedMapIdx = MapIdx;
}

/**
* tranlate virtual address to physical address
* @param [in] VirtAddr virtual address
* @param [out] pPhysAddr physical address
* @return 0-OK, 1-NG
*/
UINT32 SvcMem_VirtToPhys(ULONG VirtAddr, ULONG *pPhysAddr)
{
#if defined(CONFIG_LINUX)
    UINT32 i, Rval = SVC_NG;
    ULONG EndAddr;
    const SVC_MEM_POOL_s *pMemPool;

    if (g_pMem != NULL) {
        for (i = 0; i < SVC_MEM_TYPE_MAX; i++) {
            pMemPool = &g_pMem->Pool[i];
            if (pMemPool->Size != 0U) {
                EndAddr = pMemPool->BaseAddr + pMemPool->Size - 1U;
                if ((VirtAddr >= pMemPool->BaseAddr) && (VirtAddr <= EndAddr)) {
                    *pPhysAddr = (VirtAddr - pMemPool->BaseAddr) + pMemPool->PhyBaseAddr;
                    Rval = SVC_OK;
                    break;
                }
            }
        }

        if (Rval != SVC_OK) {
            SvcLog_NG(SVC_LOG_SYS, "SvcMem_VirtToPhys failed", 0U, 0U);
        }
    } else {
        SvcLog_NG(SVC_LOG_SYS, "Please invoke SvcMem_Init first", 0U, 0U);
    }

    return Rval;
#else
    AmbaMisra_TouchUnused(g_pMem);
    AmbaMisra_TouchUnused(pPhysAddr);
    return AmbaMMU_VirtToPhys(VirtAddr, pPhysAddr);
#endif
}

/**
* tranlate physical address to virtual address
* @param [in] pPhysAddr physical address
* @param [out] VirtAddr virtual address
* @return 0-OK, 1-NG
*/
UINT32 SvcMem_PhysToVirt(ULONG PhysAddr, ULONG *pVirtAddr)
{
#if defined(CONFIG_LINUX)
    UINT32 i, Rval = SVC_NG;
    ULONG EndAddr;
    const SVC_MEM_POOL_s *pMemPool;

    if (g_pMem != NULL) {
        for (i = 0; i < SVC_MEM_TYPE_MAX; i++) {
            pMemPool = &g_pMem->Pool[i];
            if (pMemPool->Size != 0U) {
                EndAddr = pMemPool->PhyBaseAddr + pMemPool->Size - 1U;
                if ((PhysAddr >= pMemPool->PhyBaseAddr) && (PhysAddr <= EndAddr)) {
                    *pVirtAddr = (PhysAddr - pMemPool->PhyBaseAddr) + pMemPool->BaseAddr;
                    Rval = SVC_OK;
                    break;
                }
            }
        }

        if (Rval != SVC_OK) {
            SvcLog_NG(SVC_LOG_SYS, "SvcMem_PhysToVirt failed", 0U, 0U);
        }
    } else {
        SvcLog_NG(SVC_LOG_SYS, "Please invoke SvcMem_Init first", 0U, 0U);
    }

    return Rval;
#else
    AmbaMisra_TouchUnused(g_pMem);
    AmbaMisra_TouchUnused(pVirtAddr);
    return AmbaMMU_PhysToVirt(PhysAddr, pVirtAddr);
#endif
}

