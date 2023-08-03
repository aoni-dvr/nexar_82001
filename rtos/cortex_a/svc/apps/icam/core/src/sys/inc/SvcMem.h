/**
*  @file SvcMem.h
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

#ifndef SVC_MEM_H
#define SVC_MEM_H

/*
    fixed memory : allocate one time and won't release
    shared memory: allocate by application and could be reset
                   by different application

    cache           non-cache       cv-usr
    pool            pool            pool
    ----------      ----------      ----------
   |  fixed   |    |  fixed   |    |  fixed   |
   |  memory  |    |  memory  |    |  memory  |
    ----------      ----------      ----------
   |  shared  |    |  shared  |
   |  memory  |    |  memory  |
    ----------      ----------
*/

#define SVC_MEM_MAP_FIXED       (0U)
#define SVC_MEM_MAP_SHARED      (1U)

#define SVC_MEM_TYPE_NC         (0U)    /* non-cache           */
#define SVC_MEM_TYPE_CA         (1U)    /* cache               */
#define SVC_MEM_TYPE_CV         (2U)    /* cv user memory pool */
#if defined(CONFIG_DEVICE_TREE_SUPPORT)
#define SVC_MEM_TYPE_DSP        (3U)    /* dsp user memory pool */
#if defined(CONFIG_PIO_MEM_ALLOC)
#define SVC_MEM_TYPE_IO         (4U)    /* io user memory pool */
#define SVC_MEM_TYPE_MAX        (5U)
#else
#define SVC_MEM_TYPE_MAX        (4U)
#endif
#else
#define SVC_MEM_TYPE_MAX        (3U)
#endif

#define SVC_MEM_REG_DSP         (0U)
#define SVC_MEM_REG_IO          (1U)
#define SVC_MEM_REG_CA          (2U)
#define SVC_MEM_REG_NC          (3U)
#define SVC_MEM_REG_CV          (4U)
#define SVC_MEM_REG_MAX         (5U)

typedef struct {
    /* In: fill by user */
    UINT32  MemId;
    UINT32  Alignment;
    #define SVC_MEM_REQ_LEFT    (0xFFFFFFFFU)
    UINT32  ReqSize;

    /* Out */
    ULONG   ReqPriv;
} SVC_MEM_REQ_s;

typedef struct {
    /* in */
    UINT32          MapType;
    UINT32          ReqNum[SVC_MEM_REG_MAX];
    SVC_MEM_REQ_s*  pReqArr[SVC_MEM_REG_MAX];

    /* out */
    ULONG           RegBase[SVC_MEM_REG_MAX];
    UINT32          RegSize[SVC_MEM_REG_MAX];
} SVC_MEM_MAP_s;

typedef struct {
    SVC_MEM_MAP_s  *pFixedMap;

    SVC_MEM_MAP_s  *pSharedMapArr;
    UINT32         SharedMapNum;
    UINT32         SharedMapIdx;
} SVC_MEM_PRIV_s;

typedef struct {
    /* In: fill by user */
    ULONG   BaseAddr;
    UINT32  Size;

    ULONG   PhyBaseAddr;
    UINT32  Cacheable;
    /* Out */
    ULONG   CurrAddr;
} SVC_MEM_POOL_s;

typedef struct {
    SVC_MEM_POOL_s  Pool[SVC_MEM_TYPE_MAX];
    SVC_MEM_PRIV_s  Priv;
} SVC_MEM_s;

void   SvcMem_Init(SVC_MEM_s *pMem);

UINT32 SvcMem_FixedMapConfig(SVC_MEM_s *pMem, SVC_MEM_MAP_s *pMap);
UINT32 SvcMem_SharedMapConfig(SVC_MEM_s *pMem, UINT32 MapNum, SVC_MEM_MAP_s *pMapArr);
void   SvcMem_SharedMapSwitch(SVC_MEM_s *pMem, UINT32 MapIdx);

UINT32 SvcMem_VirtToPhys(ULONG VirtAddr, ULONG *pPhysAddr);
UINT32 SvcMem_PhysToVirt(ULONG PhysAddr, ULONG *pVirtAddr);

#endif  /* SVC_MEM_H */
