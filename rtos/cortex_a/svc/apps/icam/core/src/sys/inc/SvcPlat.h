/**
*  @file SvcPlat.h
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
*  @details svc platform dependent functions
*
*/

#ifndef SVC_PLAT_H
#define SVC_PLAT_H

#include "AmbaCFS.h"

typedef void (*PLAT_STG_MOUNT_t)(char Drive, UINT32 MountOp);

typedef void (*PLAT_ADRV_INIT_t)(UINT32 Chan, UINT32 EnableDMIC);
typedef void (*PLAT_AIN_CTRL_t)(UINT32 Chan, UINT32 Op);
typedef void (*PLAT_AOU_CTRL_t)(UINT32 Chan, UINT32 Op);
typedef void (*PLAT_ADRV_CTRL_t)(UINT32 Chan, UINT32 Op);

typedef void (*PLAT_BUF_POOL_t)(void *pMem);

typedef void (*PLAT_TIME_PROF_SET_t)(UINT32 ProfileId);
typedef void (*PLAT_TIME_PROF_GET_t)(void **ppTimeInfo);

typedef struct {
    /* storage mount/umount */
    PLAT_STG_MOUNT_t  pCbMountOp;

    /* ain/aout/adrv control */
    PLAT_ADRV_INIT_t  pCbADrvInit;
    #define SVC_PLAT_AIN_RESET      (0U)
    #define SVC_PLAT_AIN_ON         (1U)
    #define SVC_PLAT_AIN_OFF        (2U)
    PLAT_AIN_CTRL_t  pCbAinCtrl;
    #define SVC_PLAT_AOU_RESET      (0U)
    #define SVC_PLAT_AOU_ON         (1U)
    #define SVC_PLAT_AOU_OFF        (2U)
    PLAT_AOU_CTRL_t  pCbAoutCtrl;
    #define SVC_PLAT_ADRV_MUTE_ON   (0U)
    #define SVC_PLAT_ADRV_MUTE_OFF  (1U)
    PLAT_ADRV_CTRL_t  pCbADrvCtrl;

    /* cfs callback functions */
    AMBA_CFS_CHECK_CACHED_f      pCbCacheChk;
    AMBA_CFS_CACHE_CLEAN_f       pCbCacheCln;
    AMBA_CFS_CACHE_INVALIDATE_f  pCbCacheInv;
    AMBA_CFS_VIRT_TO_PHYS_f      pCbVirToPhy;

    /* buffer pool init */
    PLAT_BUF_POOL_t  pCbBufPoolInit;
    PLAT_BUF_POOL_t  pCbBufPoolCvInit;

    /* cache operations */
    UINT32 (*pAmbaCacheClean)(ULONG Addr, ULONG Size);
    UINT32 (*pAmbaCacheInvalidate)(ULONG Addr, ULONG Size);
    UINT32 (*pAmbaCacheFlush)(ULONG Addr, ULONG Size);

    /* time profile */
    PLAT_TIME_PROF_SET_t  pCbTimeProfleSet;
    PLAT_TIME_PROF_GET_t  pCbTimeProfleGet;
} SVC_PLAT_CB_s;

extern SVC_PLAT_CB_s  *g_pPlatCbEntry;

void SvcPlat_InitCB(SVC_PLAT_CB_s *pPlatCB);

UINT32 SvcPlat_CacheClean(ULONG Addr, ULONG Size);
UINT32 SvcPlat_CacheInvalidate(ULONG Addr, ULONG Size);
UINT32 SvcPlat_CacheFlush(ULONG Addr, ULONG Size);

#endif  /* SVC_UTIL_H */
