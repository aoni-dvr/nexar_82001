/**
 *  @file SvcBuffer.c
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
 *  @details svc buffer map
 *
 */

#include "AmbaTypes.h"
#include "AmbaMisraFix.h"
#include "AmbaUtility.h"
#include "AmbaPrint.h"
#include "AmbaWrap.h"
#include "AmbaMemProt.h"

#if defined(CONFIG_THREADX)
#include "bsp.h"
#endif

#include "SvcMem.h"
#include "SvcErrCode.h"
#include "SvcLog.h"
#include "SvcWrap.h"
#include "SvcPlat.h"
#include "SvcBuffer.h"

#define SVC_LOG_BUF     "BUF"

//#define CONFIG_DRAM_ATT_USED    (1U)

#if defined(CONFIG_DRAM_ATT_USED)

#define ATT_REG_MAP_BASE            (CONFIG_DDR_SIZE)

#define ATT_REG_DSP_BINARY          (0x0U)  /* dsp binary                               */
#define ATT_REG_DSP_PROT_BUF        (0x1U)  /* dsp protocol buffer                      */
#define ATT_REG_DSP_DATA_FIXED      (0x2U)  /* dsp data buffer (fixed)                  */
#define ATT_REG_DSP_DATA_SHARED     (0x3U)  /* dsp data buffer (shared)                 */
#define ATT_REG_DSP_CV_USER         (0x4U)  /* cv user buffer (access by dsp/cv)        */
#define ATT_REG_CV_MEM              (0x5U)  /* cv memory buffer (overlap with cv user)  */
#define ATT_REG_MAX                 (0x6U)

#define ATT_CLI_DSP_ORCCODE         (0x0U)
#define ATT_CLI_DSP_ORCME           (0x1U)
#define ATT_CLI_DSP_SMEM            (0x2U)
#define ATT_CLI_CV_ORCVP            (0x3U)
#define ATT_CLI_CV_ORCL2            (0x4U)
#define ATT_CLI_CV_VMEM             (0x5U)
#define ATT_CLI_MAX                 (0x6U)

static AMBA_DRAM_ATT_INFO_s         g_AttRegMap[ATT_REG_MAX] GNU_SECTION_NOZEROINIT;
static AMBA_DRAM_ATT_CLIENT_INFO_s  g_AttClients[ATT_CLI_MAX] GNU_SECTION_NOZEROINIT;
#endif

static SVC_MEM_s      g_SvcMem GNU_SECTION_NOZEROINIT;
static SVC_MEM_MAP_s  *g_pFixedMM GNU_SECTION_NOZEROINIT;
static SVC_MEM_MAP_s  *g_pSharedMM GNU_SECTION_NOZEROINIT;
static UINT32         g_SvcFreeSpaceState[SVC_MEM_TYPE_MAX] GNU_SECTION_NOZEROINIT;

static void SvcWrapPrnFunc(const char *pModule, const char *pFormat, UINT32 Arg1, UINT32 Arg2)
{
    if ((pModule != NULL) && (pFormat != NULL)) {
        AmbaPrint_PrintUInt5(pFormat, Arg1, Arg2, 0U, 0U, 0U);
    }
}

static inline void PRINTF(const char *pFmt, ULONG x1, ULONG x2, ULONG x3, ULONG x4, ULONG x5)
{
#define DBG_PRN_NATIVE      (0U)

#if DBG_PRN_NATIVE
    printf(pFmt, x1, x2, x3, x4, x5);
    printf("\n");
#else
    SVC_WRAP_PRINT_s SvcWrapPrn;

    if (AmbaWrap_memset(&SvcWrapPrn, 0, sizeof(SvcWrapPrn)) == SVC_OK) {
        SvcWrapPrn.pProc = SvcWrapPrnFunc;
        SvcWrapPrn.pStrFmt = pFmt;
        SvcWrapPrn.Argv[SvcWrapPrn.Argc].Uint64 = (UINT64)x1; SvcWrapPrn.Argc ++;
        SvcWrapPrn.Argv[SvcWrapPrn.Argc].Uint64 = (UINT64)x2; SvcWrapPrn.Argc ++;
        SvcWrapPrn.Argv[SvcWrapPrn.Argc].Uint64 = (UINT64)x3; SvcWrapPrn.Argc ++;
        SvcWrapPrn.Argv[SvcWrapPrn.Argc].Uint64 = (UINT64)x4; SvcWrapPrn.Argc ++;
        SvcWrapPrn.Argv[SvcWrapPrn.Argc].Uint64 = (UINT64)x5; SvcWrapPrn.Argc ++;

        SvcWrap_Print("", &SvcWrapPrn);
    }
#endif
}

static void MemMapDump(void)
{
    UINT32               i, m, r;
    ULONG                FreeSize, Base, End;
    const SVC_MEM_MAP_s  *pMap;
    const SVC_MEM_REQ_s  *pReq;
    const SVC_MEM_s      *pMem = &g_SvcMem;

    /* dump memory pool */
    PRINTF("\n[MemPool]", 0U, 0U, 0U, 0U, 0U);
    for (i = SVC_MEM_TYPE_NC; i < SVC_MEM_TYPE_MAX; i++) {
        if (pMem->Pool[i].Size == 0U) {
            continue;
        }

        FreeSize = pMem->Pool[i].BaseAddr;
        FreeSize += pMem->Pool[i].Size;
        FreeSize -= pMem->Pool[i].CurrAddr;
        if (SVC_MEM_TYPE_NC == i) {
            PRINTF("  [NonCache]", 0U, 0U, 0U, 0U, 0U);
        } else if (SVC_MEM_TYPE_CA == i) {
            PRINTF("  [Cache]", 0U, 0U, 0U, 0U, 0U);
        } else if (SVC_MEM_TYPE_CV == i) {
            PRINTF("  [CvUsr]", 0U, 0U, 0U, 0U, 0U);
        #if defined(SVC_MEM_TYPE_DSP)
        } else if (SVC_MEM_TYPE_DSP == i) {
            PRINTF("  [DspUsr]", 0U, 0U, 0U, 0U, 0U);
        #endif
        } else {
        #if defined(SVC_MEM_TYPE_IO)
            PRINTF("  [IO]", 0U, 0U, 0U, 0U, 0U);
        #else
            /* do nothing */
        #endif
        }

        PRINTF("    %p ~ %p, free 0x%08X, (PhyBase %p)"
                                , pMem->Pool[i].BaseAddr
                                , (pMem->Pool[i].BaseAddr + pMem->Pool[i].Size - 1U)
                                , FreeSize
                                , pMem->Pool[i].PhyBaseAddr
                                , 0U);
    }

    /* dump fixed map */
    PRINTF("[MemRegion]", 0U, 0U, 0U, 0U, 0U);
    PRINTF("  [Fixed]", 0U, 0U, 0U, 0U, 0U);

    pMap = pMem->Priv.pFixedMap;
    for (i = 0U; i < SVC_MEM_REG_MAX; i++) {
        if (pMap->RegSize[i] == 0U) {
            continue;
        }
        pReq = pMap->pReqArr[i];

        Base = pMap->RegBase[i];
        End = pMap->RegBase[i] + pMap->RegSize[i] - 1U;
        if (i == SVC_MEM_REG_DSP) {
            PRINTF("    [DSP_%p ~ %p]", Base, End, 0U, 0U, 0U);
        } else if (i == SVC_MEM_REG_IO) {
            PRINTF("    [IO_%p ~ %p]", Base, End, 0U, 0U, 0U);
        } else if (i == SVC_MEM_REG_CA) {
            PRINTF("    [CA_%p ~ %p]", Base, End, 0U, 0U, 0U);
        } else if (i == SVC_MEM_REG_NC) {
            PRINTF("    [NC_%p ~ %p]", Base, End, 0U, 0U, 0U);
        } else {
            PRINTF("    [CV_%p ~ %p]", Base, End, 0U, 0U, 0U);
        }

        for (r = 0U; r < pMap->ReqNum[i]; r++) {
            if (pReq[r].ReqSize == 0U) {
                continue;
            }

            PRINTF("      (%02d) %p ~ %p, align(%u), size(%u)"
                                        , pReq[r].MemId
                                        ,  pReq[r].ReqPriv
                                        , (pReq[r].ReqPriv + pReq[r].ReqSize - 1U)
                                        , pReq[r].Alignment
                                        , pReq[r].ReqSize);
        }
    }

    /* dump shared map */
    PRINTF("  [Shared]", 0U, 0U, 0U, 0U, 0U);

    pMap = pMem->Priv.pSharedMapArr;
    for (m = 0U; m < pMem->Priv.SharedMapNum; m++) {
        for (i = 0U; i < SVC_MEM_REG_MAX; i++) {
            if (pMap[m].RegSize[i] == 0U) {
                continue;
            }
            pReq = pMap[m].pReqArr[i];

            Base = pMap[m].RegBase[i];
            End = pMap[m].RegBase[i] + pMap[m].RegSize[i] - 1U;

            if (i == SVC_MEM_REG_DSP) {
                PRINTF("    [Map%02d-DSP_%p ~ %p]", m, Base, End, 0U, 0U);
            } else if (i == SVC_MEM_REG_IO) {
                PRINTF("    [Map%02d-IO_%p ~ %p]", m, Base, End, 0U, 0U);
            } else if (i == SVC_MEM_REG_CA) {
                PRINTF("    [Map%02d-CA_%p ~ %p]", m, Base, End, 0U, 0U);
            } else if (i == SVC_MEM_REG_NC) {
                PRINTF("    [Map%02d-NC_%p ~ %p]", m, Base, End, 0U, 0U);
            } else {
                PRINTF("    [Map%02d-CV_%p ~ %p]", m, Base, End, 0U, 0U);
            }

            for (r = 0U; r < pMap[m].ReqNum[i]; r++) {
                if (pReq[r].ReqSize == 0U) {
                    continue;
                }

                PRINTF("      (%02d) %p ~ %p, align(%u), size(%u)"
                                        , pReq[r].MemId
                                        ,  pReq[r].ReqPriv
                                        , (pReq[r].ReqPriv + pReq[r].ReqSize - 1U)
                                        , pReq[r].Alignment
                                        , pReq[r].ReqSize);
            }
        }
    }

#if defined(CONFIG_DRAM_ATT_USED)
    /* dump ATT regions */
    PRINTF("[ATT]", 0U, 0U, 0U, 0U, 0U);
    PRINTF("  [Regions]", 0U, 0U, 0U, 0U, 0U);
    for (i = 0U; i < ATT_REG_MAX; i++) {
        Base = g_AttRegMap[i].VirtAddr;
        End = g_AttRegMap[i].VirtAddr + g_AttRegMap[i].Size - 1U;
        PRINTF("  (%02d) AttAddr(%p ~ %p), size(0x%08X)", i, Base, End, g_AttRegMap[i].Size, 0U);

        Base = g_AttRegMap[i].PhysAddr;
        End = g_AttRegMap[i].PhysAddr + g_AttRegMap[i].Size - 1U;
        PRINTF("       PhyAddr(%p ~ %p)", Base, End, 0U, 0U, 0U);
    }

    PRINTF("  [Clients]", 0U, 0U, 0U, 0U, 0U);
    for (i = 0U; i < ATT_CLI_MAX; i++) {
        Base = g_AttClients[i].VirtAddr;
        End = g_AttClients[i].VirtAddr + g_AttClients[i].Size - 1U;
        PRINTF("  (%02d_%02d) AttAddr(%p ~ %p), size(0x%08X)"
                                , i
                                , g_AttClients[i].ClientID
                                , Base
                                , End
                                , g_AttClients[i].Size);
    }
#endif
}

#if defined(CONFIG_DRAM_ATT_USED)
static void AttClientConfig(void)
{
    UINT32  i, Rval;
    UINT64  DspAttSize = 0U, CvAttSize = 0U;
    ULONG   DspAttBase = 0U, CvAttBase = 0U;

    if (AmbaWrap_memset(g_AttRegMap, 0, sizeof(g_AttRegMap)) != SVC_OK) {
        SvcLog_NG(SVC_LOG_BUF, "fail to do memset", 0U, 0U);
    }

    if (AmbaWrap_memset(g_AttClients, 0, sizeof(g_AttClients)) != SVC_OK) {
        SvcLog_NG(SVC_LOG_BUF, "fail to do memset", 0U, 0U);
    }

    /* configure ATT memory map */
    {
    #if 1
        g_AttRegMap[0].VirtAddr = ATT_REG_MAP_BASE;
        g_AttRegMap[0].PhysAddr = 0U;
        g_AttRegMap[0].Size     = CONFIG_DDR_SIZE;

        DspAttBase = ATT_REG_MAP_BASE;
        DspAttSize = CONFIG_DDR_SIZE;
        CvAttBase  = ATT_REG_MAP_BASE;
        CvAttSize  = CONFIG_DDR_SIZE;
    #else
        extern void *__ucode_start, *__ucode_end;
        extern void *__dsp_cache_buf_start, *__dsp_buf_end;
        extern void *__cv_rtos_user_start, *__cv_rtos_user_end;
        extern void *__cv_start, *__cv_end;

        ULONG       Base, End, AttBase = ATT_REG_MAP_BASE, Size;
        const void  *pAddr;

        DspAttBase = AttBase;
        /* ATT_REG_DSP_BINARY */
        pAddr = &__ucode_start;
        AmbaMisra_TypeCast(&Base, &pAddr);
        pAddr = &__ucode_end;
        AmbaMisra_TypeCast(&End, &pAddr);
        Size = End - Base;

        g_AttRegMap[ATT_REG_DSP_BINARY].VirtAddr = AttBase;
        g_AttRegMap[ATT_REG_DSP_BINARY].PhysAddr = Base;
        g_AttRegMap[ATT_REG_DSP_BINARY].Size     = Size;
        AttBase += Size;

        /* ATT_REG_DSP_PROT_BUF */
        pAddr = &__dsp_cache_buf_start;
        AmbaMisra_TypeCast(&Base, &pAddr);
        pAddr = &__dsp_buf_end;
        AmbaMisra_TypeCast(&End, &pAddr);
        Size = End - Base;

        g_AttRegMap[ATT_REG_DSP_PROT_BUF].VirtAddr = AttBase;
        g_AttRegMap[ATT_REG_DSP_PROT_BUF].PhysAddr = Base;
        g_AttRegMap[ATT_REG_DSP_PROT_BUF].Size     = Size;
        AttBase += Size;

        /* ATT_REG_DSP_DATA_FIXED */
        if (g_pFixedMM != NULL) {
            Size = g_pFixedMM->RegSize[SVC_MEM_REG_DSP];

            g_AttRegMap[ATT_REG_DSP_DATA_FIXED].VirtAddr = AttBase;
            g_AttRegMap[ATT_REG_DSP_DATA_FIXED].PhysAddr = g_pFixedMM->RegBase[SVC_MEM_REG_DSP];
            g_AttRegMap[ATT_REG_DSP_DATA_FIXED].Size     = Size;
            AttBase += Size;
        }

        /* ATT_REG_DSP_DATA_SHARED */
        if (g_pSharedMM != NULL) {
            Size = g_pSharedMM->RegSize[SVC_MEM_REG_DSP];

            g_AttRegMap[ATT_REG_DSP_DATA_SHARED].VirtAddr = AttBase;
            g_AttRegMap[ATT_REG_DSP_DATA_SHARED].PhysAddr = g_pSharedMM->RegBase[SVC_MEM_REG_DSP];
            g_AttRegMap[ATT_REG_DSP_DATA_SHARED].Size     = Size;
            AttBase += Size;
        }

        /* ATT_REG_DSP_CV_USER */
        pAddr = &__cv_rtos_user_start;
        AmbaMisra_TypeCast(&Base, &pAddr);
        pAddr = &__cv_rtos_user_end;
        AmbaMisra_TypeCast(&End, &pAddr);
        Size = End - Base;

        g_AttRegMap[ATT_REG_DSP_CV_USER].VirtAddr = AttBase;
        g_AttRegMap[ATT_REG_DSP_CV_USER].PhysAddr = Base;
        g_AttRegMap[ATT_REG_DSP_CV_USER].Size     = Size;
        AttBase += Size;

        DspAttSize = AttBase - DspAttBase;

        CvAttBase = AttBase;
        /* ATT_REG_CV_MEM */
        pAddr = &__cv_start;
        AmbaMisra_TypeCast(&Base, &pAddr);
        pAddr = &__cv_end;
        AmbaMisra_TypeCast(&End, &pAddr);
        Size = End - Base;

        g_AttRegMap[ATT_REG_CV_MEM].VirtAddr = AttBase;
        g_AttRegMap[ATT_REG_CV_MEM].PhysAddr = Base;
        g_AttRegMap[ATT_REG_CV_MEM].Size     = Size;
        AttBase += Size;

        CvAttSize = AttBase - CvAttBase;
    #endif
    }

    /* configure ATT clients */
    {
        /* dsp clients */
        g_AttClients[ATT_CLI_DSP_ORCCODE].ClientID = AMBA_DRAM_CLIENT_ORCCODE;
        g_AttClients[ATT_CLI_DSP_ORCCODE].VirtAddr = DspAttBase;
        g_AttClients[ATT_CLI_DSP_ORCCODE].Size     = DspAttSize;

        g_AttClients[ATT_CLI_DSP_ORCME].ClientID = AMBA_DRAM_CLIENT_ORCME;
        g_AttClients[ATT_CLI_DSP_ORCME].VirtAddr = DspAttBase;
        g_AttClients[ATT_CLI_DSP_ORCME].Size     = DspAttSize;

        g_AttClients[ATT_CLI_DSP_SMEM].ClientID = AMBA_DRAM_CLIENT_SMEM;
        g_AttClients[ATT_CLI_DSP_SMEM].VirtAddr = DspAttBase;
        g_AttClients[ATT_CLI_DSP_SMEM].Size     = DspAttSize;

        /* cv clients */
        g_AttClients[ATT_CLI_CV_ORCVP].ClientID = AMBA_DRAM_CLIENT_ORCVP;
        g_AttClients[ATT_CLI_CV_ORCVP].VirtAddr = CvAttBase;
        g_AttClients[ATT_CLI_CV_ORCVP].Size     = CvAttSize;

        g_AttClients[ATT_CLI_CV_ORCL2].ClientID = AMBA_DRAM_CLIENT_ORCL2;
        g_AttClients[ATT_CLI_CV_ORCL2].VirtAddr = CvAttBase;
        g_AttClients[ATT_CLI_CV_ORCL2].Size     = CvAttSize;

        g_AttClients[ATT_CLI_CV_VMEM].ClientID = AMBA_DRAM_CLIENT_VMEM;
        g_AttClients[ATT_CLI_CV_VMEM].VirtAddr = CvAttBase;
        g_AttClients[ATT_CLI_CV_VMEM].Size     = CvAttSize;
    }

#if 1
    Rval = AmbaMemProt_Init(1U, g_AttRegMap) ;
#else
    Rval = AmbaMemProt_Init(ATT_REG_MAX, g_AttRegMap) ;
#endif
    if (Rval == SVC_OK) {
        for (i = 0U; i < ATT_CLI_MAX; i++) {
            Rval = AmbaMemProt_Enable(g_AttClients[i].ClientID,
                                      g_AttClients[i].VirtAddr,
                                      g_AttClients[i].Size);
            if (Rval != SVC_OK) {
                SvcLog_NG(SVC_LOG_BUF, "## fail to config att client(), (0x%X)", i, Rval);
            }
        }
    } else {
        SvcLog_NG(SVC_LOG_BUF, "## fail to config att regions, (0x%X)", Rval, 0U);
    }
}
#endif

/**
* initialization of icam memory buffer
* @param [in] pFixedMM fixed memory map
* @return none
*/
void SvcBuffer_Init(SVC_MEM_MAP_s *pFixedMM)
{
    if (AmbaWrap_memset(&g_SvcMem, 0, sizeof(SVC_MEM_s)) != SVC_OK) {
        SvcLog_NG(SVC_LOG_BUF, "fail to do memset", 0U, 0U);
    }

    if ((g_pPlatCbEntry != NULL) && (g_pPlatCbEntry->pCbBufPoolInit != NULL)) {
        g_pPlatCbEntry->pCbBufPoolInit(&g_SvcMem);
    }

    /* initialize cache/non-cache pool */
    SvcMem_Init(&g_SvcMem);

    /* config fixed memory map */
    if (NULL != pFixedMM) {
        if (SVC_OK != SvcMem_FixedMapConfig(&g_SvcMem, pFixedMM)) {
            SvcLog_NG(SVC_LOG_BUF, "## fail to config fixed memory map", 0U, 0U);
        } else {
            g_pFixedMM = pFixedMM;
        }
    }

    MemMapDump();

    if (AmbaWrap_memset(g_SvcFreeSpaceState, 0, sizeof(g_SvcFreeSpaceState)) != SVC_OK) {
        SvcLog_NG(SVC_LOG_BUF, "fail to do memset", 0U, 0U);
    }
}

/**
* initialization of icam cv memory buffer
* @param [in] pFixedMM fixed memory map
* @return none
*/
extern void SvcBuffer_CV_Init(void);
void SvcBuffer_CV_Init(void)
{

    if ((g_pPlatCbEntry != NULL) && (g_pPlatCbEntry->pCbBufPoolCvInit != NULL)) {
        g_pPlatCbEntry->pCbBufPoolCvInit(&g_SvcMem);
    }
}

/**
* configuration of icam shared memory buffer
* @param [in] SharedMMNum number of shared memory map
* @param [in] pSharedMMArr array of shared memory map
* @return none
*/
void SvcBuffer_Config(UINT32 SharedMMNum, SVC_MEM_MAP_s *pSharedMMArr)
{
    /* config shared memory map */
    if ((SharedMMNum != 0U) && (pSharedMMArr != NULL)) {
        if (SVC_OK != SvcMem_SharedMapConfig(&g_SvcMem, SharedMMNum, pSharedMMArr)) {
            SvcLog_NG(SVC_LOG_BUF, "## fail to config shared memory map", 0U, 0U);
        } else {
            g_pSharedMM = pSharedMMArr;
        }
    }

#if defined(CONFIG_DRAM_ATT_USED)
    /* ATT client configureation */
    AttClientConfig();
#endif

    MemMapDump();
}

/**
* request of memory buffer
* @param [in] MapType type of memory buffer, fixed or shared
* @param [in] MemId id of memory buffer
* @param [out] pBase base of memory buffer
* @param [out] pSize size of memory buffer
* @return 0-OK, 1-NG
*/
UINT32 SvcBuffer_Request(UINT32 MapType, UINT32 MemId, ULONG *pBase, UINT32 *pSize)
{
    UINT32               Rval = SVC_OK, RegIdx, MemIdx;
    UINT8                Match = 0U;
    const SVC_MEM_MAP_s  *pMemMap;
    const SVC_MEM_REQ_s  *pReq;

    if (MapType == SVC_BUFFER_FIXED) {
        pMemMap = g_pFixedMM;
    } else if (MapType == SVC_BUFFER_SHARED) {
        pMemMap = g_pSharedMM;
    } else {
        pMemMap = NULL;
    }

    if (pMemMap != NULL) {
        /* Check if the assigned memory is initialized */
        for (RegIdx = 0U; RegIdx < SVC_MEM_REG_MAX; RegIdx++) {
            pReq = pMemMap->pReqArr[RegIdx];
            for (MemIdx = 0U; MemIdx < pMemMap->ReqNum[RegIdx]; MemIdx++) {
                if (MemId == pReq[MemIdx].MemId) {
                    Match = 1U;

                    *pBase = pReq[MemIdx].ReqPriv;
                    *pSize = pReq[MemIdx].ReqSize;
                    break;
                }
            }

            if (0U < Match) {
                break;
            }
        }

        if (Match == 0U) {
            Rval = SVC_NG;
            SvcLog_DBG(SVC_LOG_BUF, "No match", 0U, 0U);
        }
    } else {
        Rval = SVC_NG;
        SvcLog_NG(SVC_LOG_BUF, "Buffer not Ready. MapType %d, required %d", MapType, MemId);
    }

    return Rval;
}

/**
* lock of free memory buffer
* @param [in] MemPoolType type of memory pool, cache or non-cache
* @param [out] pBase base of memory buffer
* @param [out] pSize size of memory buffer
* @return 0-OK, 1-NG
*/
UINT32 SvcBuffer_LockFreeSpace(UINT32 MemPoolType, ULONG *pBase, UINT32 *pSize)
{
    UINT32 Rval = SVC_OK;

    if (MemPoolType >= SVC_MEM_TYPE_MAX) {
        Rval = SVC_NG;
        SvcLog_NG(SVC_LOG_BUF, "Invalid memory pool type(0x%x)!", MemPoolType, 0U);
    } else {
        const SVC_MEM_POOL_s *pMemPool = &(g_SvcMem.Pool[MemPoolType]);

        if (pSize != NULL) {
            *pSize = (UINT32)(pMemPool->BaseAddr + pMemPool->Size - pMemPool->CurrAddr);
        }

        if (pBase != NULL) {
            if (g_SvcFreeSpaceState[MemPoolType] > 0U) {
                Rval = SVC_NG;
                SvcLog_DBG(SVC_LOG_BUF, "Memory pool(0x%x) free space has been locked! 0x%x", MemPoolType, g_SvcFreeSpaceState[MemPoolType]);
            } else {
                *pBase = pMemPool->CurrAddr;
                g_SvcFreeSpaceState[MemPoolType] ++;
            }
        }
    }

    return Rval;
}

/**
* un-lock of free memory buffer
* @param [in] MemPoolType type of memory pool, cache or non-cache
* @return 0-OK, 1-NG
*/
UINT32 SvcBuffer_UnLockFreeSpace(UINT32 MemPoolType)
{
    UINT32 Rval = SVC_OK;

    if (MemPoolType >= SVC_MEM_TYPE_MAX) {
        Rval = SVC_NG;
        SvcLog_NG(SVC_LOG_BUF, "Invalid memory pool type(0x%x)!", MemPoolType, 0U);
    } else {
        g_SvcFreeSpaceState[MemPoolType] = 0U;
    }
    return Rval;
}

/**
* cache/non-cache checking of buffer
* @param [in] Addr address of buffer
* @param [in] Size size of buffer
* @return 0-non-cache, 1-cache
*/
UINT32 SvcBuffer_CheckCached(ULONG Addr, UINT32 Size)
{
    UINT32                IsCache = 1U;
    const SVC_MEM_POOL_s  *pPool = &(g_SvcMem.Pool[SVC_MEM_TYPE_NC]);

    if (pPool->Size != 0U) {
        if ((pPool->BaseAddr <= Addr) && (Addr < (pPool->BaseAddr + pPool->Size))) {
            IsCache = 0U;
        }
    }

    AmbaMisra_TouchUnused(&Size);
    return IsCache;
}

/**
* Get memory pool type
* @param [in] Addr address of buffer
* @param [in] Size size of buffer
* @return pool type
*/
UINT32 SvcBuffer_GetMemPoolType(ULONG Addr, UINT32 Size)
{
    UINT32                MemPoolType = SVC_MEM_TYPE_MAX, i;
    const SVC_MEM_POOL_s  *pPool;

    for (i = 0U; i < SVC_MEM_TYPE_MAX; i++) {
        pPool = &(g_SvcMem.Pool[i]);
        if (pPool->Size != 0U) {
            if ((pPool->BaseAddr <= Addr) && (Addr < (pPool->BaseAddr + pPool->Size))) {
                if ((Addr + Size) <= (pPool->BaseAddr + pPool->Size)) {
                    MemPoolType = i;
                    break;
                }
            }
        }
    }

    return MemPoolType;
}

/**
* virtual address to physical address
* @param [in] VirtAddr virtual address
* @param [out] pPhysAddr physical address
* @return 0-OK, 1-NG
*/
UINT32 SvcBuffer_Vir2Phys(ULONG VirtAddr, ULONG *pPhysAddr)
{
    ULONG                 PhysAddr;
    UINT32                i, Rval = SVC_NG;
    const SVC_MEM_POOL_s  *pPool;

    for (i = 0U; i < SVC_MEM_TYPE_MAX; i++) {
        pPool = &(g_SvcMem.Pool[i]);
        if (pPool->Size != 0U) {
            if ((pPool->BaseAddr <= VirtAddr) && (VirtAddr < (pPool->BaseAddr + pPool->Size))) {
                PhysAddr   = pPool->PhyBaseAddr;
                PhysAddr  += (VirtAddr - pPool->BaseAddr);

                *pPhysAddr = PhysAddr;

                Rval = SVC_OK;
                break;
            }
        }
    }

    return Rval;
}

/**
* status dump of icam memory buffer
* @return 0-OK, 1-NG
*/
void SvcBuffer_Show(void)
{
    MemMapDump();
}
