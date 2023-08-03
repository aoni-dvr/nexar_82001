/**
*  @file ArmMemPool.c
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
*   @details The Arm memory pool management utility
*
*/

#include <stdio.h>
#include <unistd.h>         /* for usleep() */

/* Amba header */
#include "AmbaTypes.h"

/* Arm header */
#include "ArmMemPool.h"
#include "ArmStdC.h"
#include "ArmMutex.h"
#include "ArmErrCode.h"
#include "ArmLog.h"

#define ARM_LOG_MEM_POOL      "ArmUtil_MemPool"

#define MAX_MEM_BLK_NUM       (256)
#define MAX_MEM_POOL_NUM      (32)
#define ALIGN4K(x)            (((x) + 4095U) & 0xFFFFF000U)
#define CHECK_INTERVAL        (5000U)
#define MEM_POOL_WAIT         (0x00000001U)  /* Wait for mem release  */
#define INIT_MEM_BASE_ADDR    (0xFFFFFFFFU)

/*
 *  The memory management
 *  BaseAddr : Point to the top of the buffer
 *  CurAddr  : Point to the bottom of allocated buffer
 *
 *                 buffer
 *          --------------------   <--- BaseAddr
 *         |                    |
 *         |--------------------|
 *         | / / / / / / / / / /|  <== Allocated buffer
 *         |--------------------|
 *         |                    |
 *         |--------------------|
 *         | / / / / / / / / / /|  <== Allocated buffer
 *         |--------------------|  <--- CurAddr
 *         |                    |
 *         |                    |
 *         |                    |
 *          --------------------
 */


typedef struct {
    ULONG             StartAddr;
    UINT32            AllocSize;
} ARM_MEM_BLK_s;

typedef struct {
    flexidag_memblk_t Buf;
    unsigned long     DiffOfVirAndPhy;
    ULONG             BaseAddr;
    UINT32            TotalSize;
    ARM_MEM_BLK_s     MemBlk[MAX_MEM_BLK_NUM];
    ULONG             CurAddr;
    UINT32            MaxSizeFromBase;
    UINT32            MaxSizeFromCur;
    ArmMutex_t        Mutex;
} ARM_MEM_CTRL_s;

static pthread_mutex_t g_ArmMemCtrl_Lock = PTHREAD_MUTEX_INITIALIZER; // protect BaseAddr
static ARM_MEM_CTRL_s g_ArmMemCtrl[MAX_MEM_POOL_NUM] = {0};

/*---------------------------------------------------------------------------*\
 * static functions
\*---------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: UpdateMemCtrl
 *
 *  @Description:: Update CurAddr, MaxSizeFromBase and MaxSizeFromCur
 *
 *  @Input      ::
 *    PoolId:      The memory pool ID
 *
 *  @Output     :: None
 *
 *  @Return     :: None
\*-----------------------------------------------------------------------------------------------*/
static void UpdateMemCtrl(UINT32 PoolId)
{
    ARM_MEM_CTRL_s *pMemCtrl = &g_ArmMemCtrl[PoolId];
    ULONG  MinAllocAddr = 0U, MaxAllocAddr = 0U;

    /* 1. Get the range of allcated buffer (between MinAllocAddr to MaxAllocAddr) */
    for (UINT32 i = 0U; i < MAX_MEM_BLK_NUM; i ++) {
        if (pMemCtrl->MemBlk[i].AllocSize != 0U) {
            ULONG start = pMemCtrl->MemBlk[i].StartAddr;
            ULONG end = pMemCtrl->MemBlk[i].StartAddr + pMemCtrl->MemBlk[i].AllocSize;
            if ((MinAllocAddr == 0U) || (start < MinAllocAddr)) {
                MinAllocAddr = start;
            }
            if ((MaxAllocAddr == 0U) || (end > MaxAllocAddr)) {
                MaxAllocAddr = end;
            }
        }
    }

    /* 2. Calculate CurAddr, MaxSizeFromBase and MaxSizeFromCur */
    if (MinAllocAddr == 0U) {   // no allocated buffer
        pMemCtrl->CurAddr = pMemCtrl->BaseAddr;
        pMemCtrl->MaxSizeFromBase = pMemCtrl->MaxSizeFromCur = pMemCtrl->TotalSize;
    } else {
        pMemCtrl->CurAddr = MaxAllocAddr;
        pMemCtrl->MaxSizeFromBase = MinAllocAddr - pMemCtrl->BaseAddr;
        pMemCtrl->MaxSizeFromCur  = pMemCtrl->BaseAddr + pMemCtrl->TotalSize - pMemCtrl->CurAddr;
    }

}

/*---------------------------------------------------------------------------*\
 * APIs
\*---------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: ArmMemPool_Create
 *
 *  @Description:: Create a memory pool to manage the given buffer
 *
 *  @Input      ::
 *    pInBuf:      The given buffer to manage
 *
 *  @Output     ::
 *    pPoolId:     The memory pool ID
 *
 *  @Return     ::
 *    UINT32:      ARM_OK(0)/ARM_NG(0xFFFFFFFF)
\*-----------------------------------------------------------------------------------------------*/
UINT32 ArmMemPool_Create(flexidag_memblk_t *pInBuf, UINT32 *pPoolId)
{
    UINT32 Rval = ARM_OK;
    UINT32 Idx  = 0U;
    ARM_MEM_CTRL_s *pMemCtrl;

    /* 1. Sanity check for input parameters */
    if (pInBuf == NULL) {
        ArmLog_ERR(ARM_LOG_MEM_POOL, "## ArmMemPool_Create fail (pInBuf is null)", 0U, 0U);
        Rval = ARM_NG;
    } else if (pPoolId == NULL) {
        ArmLog_ERR(ARM_LOG_MEM_POOL, "## ArmMemPool_Create fail (pPoolId is null)", 0U, 0U);
        Rval = ARM_NG;
    }
    if (Rval == ARM_OK) {
        if ((pInBuf->pBuffer == NULL) || (pInBuf->buffer_size == 0U)) {
            ArmLog_ERR(ARM_LOG_MEM_POOL, "## ArmMemPool_Create fail (pBuffer is null or buffer_size is 0)", 0U, 0U);
            Rval = ARM_NG;
        }
    }

    /* 2. Find available memory pool */
    if (Rval == ARM_OK) {
        (void) pthread_mutex_lock(&g_ArmMemCtrl_Lock); // for multi-threading
        for (Idx = 0U; Idx < MAX_MEM_POOL_NUM; Idx++) {
            if (g_ArmMemCtrl[Idx].BaseAddr == 0U) {
                g_ArmMemCtrl[Idx].BaseAddr = INIT_MEM_BASE_ADDR;
                break;
            }
        }
        (void) pthread_mutex_unlock(&g_ArmMemCtrl_Lock);


        if (MAX_MEM_POOL_NUM == Idx) {
            ArmLog_ERR(ARM_LOG_MEM_POOL, "## ArmMemPool_Create fail (No more empty memory pool)", 0U, 0U);
            Rval = ARM_NG;
        }
    }

    /* 3. Fill buffer info to ARM_MEM_CTRL_s */
    if (Rval == ARM_OK) {

        pMemCtrl = &g_ArmMemCtrl[Idx];
        {
            char MutexName[16] = "ArmMemCtrlMut";
            (void) ArmMutex_Create(&pMemCtrl->Mutex, MutexName);
        }

        (void) ArmStdC_memcpy(&(pMemCtrl->Buf), pInBuf, sizeof(flexidag_memblk_t));
        (void) pthread_mutex_lock(&g_ArmMemCtrl_Lock);
        pMemCtrl->BaseAddr = pMemCtrl->CurAddr = pInBuf->buffer_daddr;
        (void) pthread_mutex_unlock(&g_ArmMemCtrl_Lock);
        pMemCtrl->TotalSize = pMemCtrl->MaxSizeFromBase = pMemCtrl->MaxSizeFromCur = pInBuf->buffer_size;
        pMemCtrl->DiffOfVirAndPhy = (unsigned long)(pInBuf->pBuffer) - pInBuf->buffer_daddr;

        *pPoolId = Idx;

        ArmLog_DBG(ARM_LOG_MEM_POOL, "ArmMemPool_Create(): PoolId = %u, IsCached = %u", Idx, pInBuf->buffer_cacheable);
        ArmLog_DBG(ARM_LOG_MEM_POOL, "                     StartAddr=0x%x, Size=%uMB", pMemCtrl->BaseAddr, pMemCtrl->TotalSize>>20);
    }

    return Rval;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MemPoolAllocate
 *
 *  @Description:: Allocate buffer
 *
 *  @Input      ::
 *    PoolId:      The memory pool ID
 *    ReqSize:     The requested buffer size
 *
 *  @Output     ::
 *    pOutBuf:     Pointer to the output buffer
 *
 *  @Return     ::
 *    UINT32:      ARM_OK(0)/ARM_NG(0xFFFFFFFF)/MEM_POOL_WAIT(1)
\*-----------------------------------------------------------------------------------------------*/
static inline UINT32 MemPoolAllocate(UINT32 PoolId, UINT32 ReqSize, flexidag_memblk_t *pOutBuf)
{
    UINT32 Rval = ARM_OK;
    ARM_MEM_CTRL_s *pMemCtrl = &g_ArmMemCtrl[PoolId];
    UINT32 AllocatedSize = ALIGN4K(ReqSize);
    UINT32 MemBlkIdx = 0U;

    ArmMutex_Take(&pMemCtrl->Mutex);
    /* 1. Check if there is enough space */
    if ((AllocatedSize > pMemCtrl->MaxSizeFromBase) && (AllocatedSize > pMemCtrl->MaxSizeFromCur)) {
        //ArmLog_WARN(ARM_LOG_MEM_POOL, "MemPoolAllocate (Not enough buf to allocate %u bytes)", AllocatedSize, 0U);
        Rval = MEM_POOL_WAIT;
    }

    /* 2. Get avaliable MemBlk slot */
    if (Rval == ARM_OK) {
        MemBlkIdx = MAX_MEM_BLK_NUM;
        for (UINT32 i = 0U; i < MAX_MEM_BLK_NUM; i ++) {
            if (pMemCtrl->MemBlk[i].StartAddr == 0U) {  // empty slot
                MemBlkIdx = i;
                break;
            }
        }
        if (MemBlkIdx == MAX_MEM_BLK_NUM) {
            ArmLog_ERR(ARM_LOG_MEM_POOL, "## MemPoolAllocate fail (Can't get avaliable MemBlk slot)", 0U, 0U);
            Rval = ARM_NG;
        }
    }

    /* 3. Allocated memory */
    if (Rval == ARM_OK)  {
        if (pMemCtrl->MaxSizeFromCur >= AllocatedSize) {
            pMemCtrl->MemBlk[MemBlkIdx].StartAddr = pMemCtrl->CurAddr;
            pMemCtrl->MemBlk[MemBlkIdx].AllocSize = AllocatedSize;
        } else if (pMemCtrl->MaxSizeFromBase >= AllocatedSize) {
            pMemCtrl->MemBlk[MemBlkIdx].StartAddr = pMemCtrl->BaseAddr + pMemCtrl->MaxSizeFromBase - AllocatedSize;
            pMemCtrl->MemBlk[MemBlkIdx].AllocSize = AllocatedSize;
        }

        //ArmLog_DBG(ARM_LOG_MEM_POOL, "Allocate buf=0x%x, size=%u", pMemCtrl->MemBlk[MemBlkIdx].StartAddr, pMemCtrl->MemBlk[MemBlkIdx].AllocSize);
        UpdateMemCtrl(PoolId);

        /* 4. Fill flexidag_memblk_t */
        {
            unsigned long temp;
            /* Virtual = diff + Physical */
            temp = pMemCtrl->DiffOfVirAndPhy + pMemCtrl->MemBlk[MemBlkIdx].StartAddr;
            pOutBuf->pBuffer          = (char *) temp;
            pOutBuf->buffer_daddr     = pMemCtrl->MemBlk[MemBlkIdx].StartAddr;
            pOutBuf->buffer_cacheable = pMemCtrl->Buf.buffer_cacheable;
            pOutBuf->buffer_size      = pMemCtrl->MemBlk[MemBlkIdx].AllocSize;
        }
    }

    ArmMutex_Give(&pMemCtrl->Mutex);

    return Rval;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: ArmMemPool_Allocate
 *
 *  @Description:: Allocate buffer
 *
 *  @Input      ::
 *    PoolId:      The memory pool ID
 *    ReqSize:     The requested buffer size
 *
 *  @Output     ::
 *    pOutBuf:     Pointer to the output buffer
 *
 *  @Return     ::
 *    UINT32:      ARM_OK(0)/ARM_NG(0xFFFFFFFF)
\*-----------------------------------------------------------------------------------------------*/
UINT32 ArmMemPool_Allocate(UINT32 PoolId, UINT32 ReqSize, flexidag_memblk_t *pOutBuf)
{
    UINT32 Rval = ARM_OK;

    /* 1. Sanity check for input parameters */
    if (pOutBuf == NULL) {
        ArmLog_ERR(ARM_LOG_MEM_POOL, "## ArmMemPool_Allocate fail (pOutBuf is null)", 0U, 0U);
        Rval = ARM_NG;
    } else if (PoolId >= MAX_MEM_POOL_NUM) {
        ArmLog_ERR(ARM_LOG_MEM_POOL, "## ArmMemPool_Allocate fail (invalid PoolId %u) ", PoolId, 0U);
        Rval = ARM_NG;
    }

    if (Rval == ARM_OK) {
        ARM_MEM_CTRL_s *pMemCtrl = &g_ArmMemCtrl[PoolId];
        UINT32 AllocatedSize = ALIGN4K(ReqSize);
        UINT32 MemBlkIdx = 0U;

        ArmMutex_Take(&pMemCtrl->Mutex);
        /* 2. Check if there is enough space */
        if (Rval == ARM_OK) {
            if ((AllocatedSize > pMemCtrl->MaxSizeFromBase) && (AllocatedSize > pMemCtrl->MaxSizeFromCur)) {
                ArmLog_ERR(ARM_LOG_MEM_POOL, "## ArmMemPool_Allocate fail (Not enough buf to allocate %u bytes)", AllocatedSize, 0U);
                Rval = ARM_NG;
            }
        }

        /* 3. Get avaliable MemBlk slot */
        if (Rval == ARM_OK) {
            MemBlkIdx = MAX_MEM_BLK_NUM;
            for (UINT32 i = 0U; i < MAX_MEM_BLK_NUM; i ++) {
                if (pMemCtrl->MemBlk[i].StartAddr == 0U) {  // empty slot
                    MemBlkIdx = i;
                    break;
                }
            }
            if (MemBlkIdx == MAX_MEM_BLK_NUM) {
                ArmLog_ERR(ARM_LOG_MEM_POOL, "## ArmMemPool_Allocate fail (Can't get avaliable MemBlk slot)", 0U, 0U);
                Rval = ARM_NG;
            }
        }

        /* 4. Allocated memory */
        if (Rval == ARM_OK)  {
            if (pMemCtrl->MaxSizeFromCur >= AllocatedSize) {
                pMemCtrl->MemBlk[MemBlkIdx].StartAddr = pMemCtrl->CurAddr;
                pMemCtrl->MemBlk[MemBlkIdx].AllocSize = AllocatedSize;
            } else if (pMemCtrl->MaxSizeFromBase >= AllocatedSize) {
                pMemCtrl->MemBlk[MemBlkIdx].StartAddr = pMemCtrl->BaseAddr + pMemCtrl->MaxSizeFromBase - AllocatedSize;
                pMemCtrl->MemBlk[MemBlkIdx].AllocSize = AllocatedSize;
            }

            //ArmLog_DBG(ARM_LOG_MEM_POOL, "Allocate buf=0x%x, size=%u", pMemCtrl->MemBlk[MemBlkIdx].StartAddr, pMemCtrl->MemBlk[MemBlkIdx].AllocSize);
            UpdateMemCtrl(PoolId);

            /* 5. Fill flexidag_memblk_t */
            {
                unsigned long temp;
                /* Virtual = diff + Physical */
                temp = pMemCtrl->DiffOfVirAndPhy + pMemCtrl->MemBlk[MemBlkIdx].StartAddr;
                pOutBuf->pBuffer          = (char *) temp;
                pOutBuf->buffer_daddr     = pMemCtrl->MemBlk[MemBlkIdx].StartAddr;
                pOutBuf->buffer_cacheable = pMemCtrl->Buf.buffer_cacheable;
                pOutBuf->buffer_size      = pMemCtrl->MemBlk[MemBlkIdx].AllocSize;
            }
        }

        ArmMutex_Give(&pMemCtrl->Mutex);
    }

    return Rval;

}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: ArmMemPool_AllocateBlock
 *
 *  @Description:: Allocate buffer
 *
 *  @Input      ::
 *    PoolId:      The memory pool ID
 *    ReqSize:     The requested buffer size
 *
 *  @Output     ::
 *    pOutBuf:     Pointer to the output buffer
 *
 *  @Return     ::
 *    UINT32:      ARM_OK(0)/ARM_NG(0xFFFFFFFF)
\*-----------------------------------------------------------------------------------------------*/
UINT32 ArmMemPool_AllocateBlock(UINT32 PoolId, UINT32 ReqSize, flexidag_memblk_t *pOutBuf)
{
    UINT32 Rval = ARM_OK;

    /* Sanity check for input parameters */
    if (pOutBuf == NULL) {
        ArmLog_ERR(ARM_LOG_MEM_POOL, "## ArmMemPool_AllocateBlock fail (pOutBuf is null)", 0U, 0U);
        Rval = ARM_NG;
    } else if (PoolId >= MAX_MEM_POOL_NUM) {
        ArmLog_ERR(ARM_LOG_MEM_POOL, "## ArmMemPool_AllocateBlock fail (invalid PoolId %u) ", PoolId, 0U);
        Rval = ARM_NG;
    }

    if (ARM_OK == Rval) {
        do {
            Rval = MemPoolAllocate(PoolId, ReqSize, pOutBuf);
            if ((ARM_OK == Rval) || (ARM_NG == Rval)) {
                break;
            }
            usleep(CHECK_INTERVAL);
        } while(1);
    }

    return Rval;

}


/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: ArmMemPool_Free
 *
 *  @Description:: Free buffer
 *
 *  @Input      ::
 *    PoolId:      The memory pool ID
 *    pReqBuf:     Pointer to the buffer
 *
 *  @Output     :: None
 *
 *  @Return     ::
 *    UINT32:      ARM_OK(0)/ARM_NG(0xFFFFFFFF)
\*-----------------------------------------------------------------------------------------------*/
UINT32 ArmMemPool_Free(UINT32 PoolId, flexidag_memblk_t *pBuf)
{
    UINT32 Rval = ARM_OK;

    /* 1. Sanity check for input parameters */
    if (pBuf == NULL) {
        ArmLog_ERR(ARM_LOG_MEM_POOL, "## pBuf is null", 0U, 0U);
        Rval = ARM_NG;
    } else if (PoolId >= MAX_MEM_POOL_NUM) {
        ArmLog_ERR(ARM_LOG_MEM_POOL, "## PoolId is invalid", PoolId, 0U);
        Rval = ARM_NG;
    }

    if (Rval == ARM_OK) {
        ARM_MEM_CTRL_s *pMemCtrl = &g_ArmMemCtrl[PoolId];
        UINT32 MemBlkIdx = 0U;

        ArmMutex_Take(&pMemCtrl->Mutex);
        /* 2. Find the freed memblk slot */
        MemBlkIdx = MAX_MEM_BLK_NUM;
        for (UINT32 i = 0U; i < MAX_MEM_BLK_NUM; i ++) {
            if ((pMemCtrl->MemBlk[i].AllocSize == pBuf->buffer_size) &&
                (pMemCtrl->MemBlk[i].StartAddr == pBuf->buffer_daddr)) {  // the freed slot
                MemBlkIdx = i;
                break;
            }
        }
        if (MemBlkIdx == MAX_MEM_BLK_NUM) {
            ArmLog_ERR(ARM_LOG_MEM_POOL, "## Can't find MemBlk slot for buf 0x%x size %u", pBuf->buffer_daddr, pBuf->buffer_size);
            Rval = ARM_NG;
        }

        /* 3. Free buffer */
        if (Rval == ARM_OK) {
            pMemCtrl->MemBlk[MemBlkIdx].StartAddr = 0U;
            pMemCtrl->MemBlk[MemBlkIdx].AllocSize = 0U;

            //ArmLog_DBG(ARM_LOG_MEM_POOL, "Free buf=0x%x, size=%u", pBuf->buffer_daddr, pBuf->buffer_size);
            UpdateMemCtrl(PoolId);
        }

        ArmMutex_Give(&pMemCtrl->Mutex);
    }

    return Rval;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: ArmMemPool_Delete
 *
 *  @Description:: Delete a memory pool
 *
 *  @Input      ::
 *    PoolId:      The memory pool ID
 *
 *  @Output     :: None
 *
 *  @Return     ::
 *    UINT32:      ARM_OK(0)/ARM_NG(0xFFFFFFFF)
\*-----------------------------------------------------------------------------------------------*/
UINT32 ArmMemPool_Delete(UINT32 PoolId)
{
    UINT32 Rval = ARM_OK;
    ARM_MEM_CTRL_s *pMemCtrl;

    /* 1. Sanity check for input parameters */
    if (PoolId >= MAX_MEM_POOL_NUM) {
        ArmLog_ERR(ARM_LOG_MEM_POOL, "## PoolId is invalid", PoolId, 0U);
        Rval = ARM_NG;
    }

    /* 2. Delete the mem pool (memset ARM_MEM_CTRL_s) */
    if (Rval == ARM_OK) {
        pMemCtrl = &g_ArmMemCtrl[PoolId];

        (void) ArmMutex_Delete(&pMemCtrl->Mutex);

        (void) pthread_mutex_lock(&g_ArmMemCtrl_Lock);
        (void) ArmStdC_memset(pMemCtrl, 0, sizeof(ARM_MEM_CTRL_s));
        (void) pthread_mutex_unlock(&g_ArmMemCtrl_Lock);

        ArmLog_DBG(ARM_LOG_MEM_POOL, "ArmMem_DeletePool(): PoolId = %u", PoolId, 0U);
    }

    return Rval;
}

