/**
*  @file ArmMemPool.h
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

#ifndef ARM_MEM_POOL_H
#define ARM_MEM_POOL_H

#include "AmbaTypes.h"
#include "cvapi_ambacv_flexidag.h"

// 128 alignment for ORC L2 cache
// 4K alignment for CMA page
static inline UINT32 ALIGN4K(UINT32 x)
{
    return (((x) + 4095U) & 0xFFFFF000U);
}

/******************************************************************************
 *  Defined in ArmMemPool.c
 ******************************************************************************/
UINT32 ArmMemPool_Create(const flexidag_memblk_t *pInBuf, UINT32 *pPoolId);
UINT32 ArmMemPool_Allocate(UINT32 PoolId, UINT32 ReqSize, flexidag_memblk_t *pOutBuf);
UINT32 ArmMemPool_AllocateBlock(UINT32 PoolId, UINT32 ReqSize, flexidag_memblk_t *pOutBuf, UINT32 PollingTime);
UINT32 ArmMemPool_Free(UINT32 PoolId, const flexidag_memblk_t *pBuf);
UINT32 ArmMemPool_CacheClean(flexidag_memblk_t *pBuf);
UINT32 ArmMemPool_Invalid(flexidag_memblk_t *pBuf);
UINT32 ArmMemPool_Delete(UINT32 PoolId);
UINT32 ArmMemPool_GetUsage(UINT32 PoolId, UINT32 *pUsedSize);

#endif /* ARM_MEM_POOL_H */

