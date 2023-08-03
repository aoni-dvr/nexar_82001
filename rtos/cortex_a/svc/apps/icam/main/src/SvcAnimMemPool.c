/**
 *  @file SvcAnimMemPool.c
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
 *  @details Implementation of anim memory pool management api
 *
 */

#include <AmbaTypes.h>
#include <AmbaKAL.h>
#include <AmbaSYS.h>
#include <AmbaUtility.h>

#include <AmbaWrap.h>
#include <AmbaPrint.h>
#include <AmbaMisraFix.h>

#include "SvcWrap.h"
#include "SvcErrCode.h"
#include "SvcAnimMemPool.h"

#define SVC_ANIM_MEM_POOL_MAX_MEM_POOL_NUM (256U)
#define SVC_ANIM_MAX_NAME_LENGTH           (64U)

typedef struct {
    UINT8 State;
    UINT32 Addr;
    UINT32 AlignAddr;
    UINT32 Size;
} SVC_ANIM_MEM_POOL_BLOCK_INFO_s;

typedef struct {
    UINT8 State;
    UINT32 Addr;
    UINT32 Size;
    UINT32 AvailableSize;
    UINT16 BlocklNum;
    SVC_ANIM_MEM_POOL_BLOCK_INFO_s BlockInfo[SVC_ANIM_MEM_POOL_MAX_MEM_POOL_NUM];
} SVC_ANIM_MEM_POOL_STATE_INFO_s;

typedef struct {
    AMBA_KAL_MUTEX_t Mutex;
    UINT8 MemPoolNum;
    SVC_ANIM_MEM_POOL_STATE_INFO_s MemPool[SVC_ANIM_MEM_POOL_MAX_POOL_NUM];
} SVC_ANIM_MEM_POOL_MGR_s;

static SVC_ANIM_MEM_POOL_MGR_s SvcAnimMemPoolMgr = {0};

static UINT32 SvcAnimMemPool_CheckRegion(const SVC_ANIM_MEM_POOL_INIT_CFG_s *Cfg)
{
    UINT32 Rval = AmbaKAL_MutexTake(&SvcAnimMemPoolMgr.Mutex, AMBA_KAL_WAIT_FOREVER);
    if (Rval == SVC_OK) {
        if (Cfg->MemPoolNum > SVC_ANIM_MEM_POOL_MAX_POOL_NUM) {
            AmbaPrint_PrintUInt5("Cfg->MemPoolNum > SVC_ANIM_MEM_POOL_MAX_POOL_NUM", 0U, 0U, 0U, 0U, 0U);
            Rval = SVC_NG;
        }
        if (AmbaKAL_MutexGive(&SvcAnimMemPoolMgr.Mutex) != SVC_OK) {
            AmbaPrint_PrintStr5("%s, AmbaKAL_MutexGive() fail!", __func__, NULL, NULL, NULL, NULL);
            Rval = SVC_NG;
        }
    } else {
        AmbaPrint_PrintStr5("%s, AmbaKAL_MutexTake() fail!", __func__, NULL, NULL, NULL, NULL);
    }
    return Rval;
}

UINT32 SvcAnimMemPool_Init(const SVC_ANIM_MEM_POOL_INIT_CFG_s *Cfg)
{
    static char MutexName[SVC_ANIM_MAX_NAME_LENGTH] = "SvcAnimMemPoolMutex";
    UINT8 i = 0U;
    UINT32 Rval = AmbaKAL_MutexCreate(&SvcAnimMemPoolMgr.Mutex, MutexName);
    if (Rval == SVC_OK) {
        Rval = SvcAnimMemPool_CheckRegion(Cfg);
        if (Rval == SVC_OK) {
            SvcAnimMemPoolMgr.MemPoolNum = Cfg->MemPoolNum;
            for (i = 0U; i < SvcAnimMemPoolMgr.MemPoolNum; i++) {
                SvcAnimMemPoolMgr.MemPool[i].State = 1U;
                AmbaMisra_TypeCast(&SvcAnimMemPoolMgr.MemPool[i].Addr, &Cfg->MemPool[i].BufferAddr);
                SvcAnimMemPoolMgr.MemPool[i].Size = Cfg->MemPool[i].BufferSize;
                SvcAnimMemPoolMgr.MemPool[i].AvailableSize = Cfg->MemPool[i].BufferSize;
            }
        } else {
            AmbaPrint_PrintStr5("%s, SvcAnimMemPool_CheckRegion() failed", __func__, NULL, NULL, NULL, NULL);
        }
    } else {
        AmbaPrint_PrintStr5("%s, AmbaKAL_MutexCreate() failed", __func__, NULL, NULL, NULL, NULL);
    }
    return Rval;
}

UINT32 SvcAnimMemPool_Allocate(UINT8 MemPool, UINT32 Alignment, void **Buffer, void **BufferRaw, UINT32 Size)
{
    UINT16 i = 0U;
    UINT32 Rval = AmbaKAL_MutexTake(&SvcAnimMemPoolMgr.Mutex, AMBA_KAL_WAIT_FOREVER);
    if (Rval == SVC_OK) {
        if (MemPool < SvcAnimMemPoolMgr.MemPoolNum) {
            if (SvcAnimMemPoolMgr.MemPool[MemPool].State == 1U) {
                if (SvcAnimMemPoolMgr.MemPool[MemPool].BlocklNum < SVC_ANIM_MEM_POOL_MAX_MEM_POOL_NUM) { /* Check block number */
                    for (i = 0U; i < SVC_ANIM_MEM_POOL_MAX_MEM_POOL_NUM; i++) {
                        if (SvcAnimMemPoolMgr.MemPool[MemPool].BlockInfo[i].State == 0U) {
                            UINT32 AlignedAddr, Padding = 0U;
                            SvcAnimMemPoolMgr.MemPool[MemPool].BlockInfo[i].Addr = (SvcAnimMemPoolMgr.MemPool[MemPool].Addr + (SvcAnimMemPoolMgr.MemPool[MemPool].Size - SvcAnimMemPoolMgr.MemPool[MemPool].AvailableSize));
                            AlignedAddr = SvcAnimMemPoolMgr.MemPool[MemPool].BlockInfo[i].Addr;
                            if (Alignment != 0U) {
                                Padding = Alignment - (AlignedAddr % Alignment);
                                if (Padding != Alignment) {
                                    AlignedAddr += Padding;
                                }
                            }
                            SvcAnimMemPoolMgr.MemPool[MemPool].BlockInfo[i].AlignAddr = AlignedAddr;
                            SvcAnimMemPoolMgr.MemPool[MemPool].BlockInfo[i].Size = Size + (AlignedAddr - SvcAnimMemPoolMgr.MemPool[MemPool].BlockInfo[i].Addr);
                            if (SvcAnimMemPoolMgr.MemPool[MemPool].BlockInfo[i].Size <= SvcAnimMemPoolMgr.MemPool[MemPool].AvailableSize) {
                                SvcAnimMemPoolMgr.MemPool[MemPool].BlockInfo[i].State = 1U;
                                SvcAnimMemPoolMgr.MemPool[MemPool].AvailableSize -= SvcAnimMemPoolMgr.MemPool[MemPool].BlockInfo[i].Size;
                                SvcAnimMemPoolMgr.MemPool[MemPool].BlocklNum++;
                                AmbaMisra_TypeCast(Buffer, &SvcAnimMemPoolMgr.MemPool[MemPool].BlockInfo[i].AlignAddr);
                                AmbaMisra_TypeCast(BufferRaw, &SvcAnimMemPoolMgr.MemPool[MemPool].BlockInfo[i].Addr);
                            } else {
                                AmbaPrint_PrintUInt5("require buffer size(%u) > MemPool buffer available size(%u)", SvcAnimMemPoolMgr.MemPool[MemPool].BlockInfo[i].Size, SvcAnimMemPoolMgr.MemPool[MemPool].AvailableSize, 0U, 0U, 0U);
                                Rval = SVC_NG;
                            }
                            break;
                        }
                    }
                } else {
                    AmbaPrint_PrintUInt5("mem pool used number(%u) > mem pool max number(%u)", SvcAnimMemPoolMgr.MemPool[MemPool].BlocklNum, SvcAnimMemPoolMgr.MemPool[MemPool].BlocklNum, 0U, 0U, 0U);
                    Rval = SVC_NG;
                }
            } else {
                AmbaPrint_PrintUInt5("RegionNo(%u) > RegionNum(%u)", MemPool, SvcAnimMemPoolMgr.MemPoolNum, 0U, 0U, 0U);
                Rval = SVC_NG;
            }
        } else {
            AmbaPrint_PrintUInt5("RegionNo(%u) uninitial!", MemPool, 0U, 0U, 0U, 0U);
            Rval = SVC_NG;
        }
        if (AmbaKAL_MutexGive(&SvcAnimMemPoolMgr.Mutex) != SVC_OK) {
            AmbaPrint_PrintStr5("%s, AmbaKAL_MutexGive() fail!", __func__, NULL, NULL, NULL, NULL);
            Rval = SVC_NG;
        }
    } else {
        AmbaPrint_PrintStr5("%s, AmbaKAL_MutexTake() fail!", __func__, NULL, NULL, NULL, NULL);
    }
    return Rval;
}

UINT32 SvcAnimMemPool_Show(void)
{
    UINT8 i = 0U;
    UINT32 Rval = AmbaKAL_MutexTake(&SvcAnimMemPoolMgr.Mutex, AMBA_KAL_WAIT_FOREVER);
    if (Rval == SVC_OK) {
        for (i = 0U; i < SvcAnimMemPoolMgr.MemPoolNum; i++) {
            UINT8 j = 0;
            AmbaPrint_PrintUInt5("Region [%u], (0x%X, 0x%X, %u, %u)", i, SvcAnimMemPoolMgr.MemPool[i].Addr,
                                                                    (SvcAnimMemPoolMgr.MemPool[i].Addr + SvcAnimMemPoolMgr.MemPool[i].Size - 1U),
                                                                    SvcAnimMemPoolMgr.MemPool[i].Size, SvcAnimMemPoolMgr.MemPool[i].AvailableSize);
            for (j = 0; j < SvcAnimMemPoolMgr.MemPool[i].BlocklNum; j++) {
                AmbaPrint_PrintUInt5("Region [%u][%u], (0x%X, 0x%X, %u)", i, j, SvcAnimMemPoolMgr.MemPool[i].BlockInfo[j].Addr,
                                                                (SvcAnimMemPoolMgr.MemPool[i].BlockInfo[j].Addr + SvcAnimMemPoolMgr.MemPool[i].BlockInfo[j].Size - 1U),
                                                                SvcAnimMemPoolMgr.MemPool[i].BlockInfo[j].Size);
            }
        }
        if (AmbaKAL_MutexGive(&SvcAnimMemPoolMgr.Mutex) != SVC_OK) {
            AmbaPrint_PrintStr5("%s, AmbaKAL_MutexGive() fail!", __func__, NULL, NULL, NULL, NULL);
            Rval = SVC_NG;
        }
    } else {
        AmbaPrint_PrintStr5("%s, AmbaKAL_MutexTake() fail!", __func__, NULL, NULL, NULL, NULL);
    }
    return Rval;
}

