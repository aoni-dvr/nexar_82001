/**
 * @file AmbaCFS_ActiveDta.h
 *
 * Copyright (c) 2019 Ambarella International LP
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
 */
#ifndef AMBACFS_ACTIVE_DTA_H
#define AMBACFS_ACTIVE_DTA_H

#include "AmbaCFS_Search.h"
#include <AmbaCFS.h>

/**
 * CFS Active DTA
 */
typedef struct {
    UINT32 IsUsed;                                  /**< This data is used. */
    AMBA_CFS_DTA_s *DirEntry;                       /**< The DTA that the search links to (bind search with DTA, to simplify code, to ensure one folder can have only one active search) */
    AMBA_CFS_FS_SEARCH_s FsSearch;                  /**< FS Search. */
    /* Member for (CacheEnable == 1U) only */
    void *CachedSearch;                             /**< The current search. NULL when CacheEnable == 0U. */
    void *CachedNode;                               /**< The current node. NULL when CacheEnable == 0U. */
    char RegExp[AMBA_CFS_MAX_FILENAME_LENGTH];      /**< Regular expression for file name. Not used when CacheEnable == 0U. */
    UINT8 Attr;                                     /**< Attributes searched for. Not used when CacheEnable == 0U. */
} AMBA_CFS_ACTIVE_DTA_s;

/**
 * CFS Active DTA Pool
 */
typedef struct {
    AMBA_CFS_ACTIVE_DTA_s *ActiveDta;               /**< Active DTA Buffer */
    UINT32 MaxSearch;                               /**< Number of data in Active DTA Buffer. Should be equal to AMBA_CFS_INIT_CFG_s.MaxSearch */
} AMBA_CFS_ACTIVE_DTA_POOL_s;


/**
 *  Get required buffer size
 *  @param [in] MaxSearch           The max unfinalized search operations. See AmbaCFS_FileSearchFirst().
 *  @return Required buffer size
 */
UINT32 AmbaCFS_ActiveDtaGetInitBufferSize(UINT32 MaxSearch);

/**
 *  Get the default configuration for initializing the Active DTA.
 *  @param [out] Config             The returned configuration of the Active DTA
 *  @return 0 - OK, others - Error
 */
void AmbaCFS_ActiveDtaGetInitDefaultCfg(AMBA_CFS_INIT_CFG_s *Config);

/**
 *  Initialize Active DTA.
 *  @param [in] Buffer              The work buffer
 *  @param [in] BufferSize          The size of the work buffer
 *  @param [in] MaxSearch           The max unfinalized search operations. See AmbaCFS_FileSearchFirst().
 *  @param [out] ActiveDtaPool      The Active DTA Pool
 *  @return 0 - OK, CFS_ERR_FATAL - Fatal error, others - Error
 */
UINT32 AmbaCFS_ActiveDtaInit(UINT8 *Buffer, UINT32 BufferSize, UINT32 MaxSearch, AMBA_CFS_ACTIVE_DTA_POOL_s *ActiveDtaPool);

/**
 *  Allocate an Active DTA and set the binding.
 *  @param [in] ActiveDtaPool       The Active DTA Pool
 *  @param [in,out] DirEntry        Address of a directory entry record. Set the binding if succeeded.
 *  @return The returned Active DTA. NULL - Error
 */
AMBA_CFS_ACTIVE_DTA_s *AmbaCFS_ActiveDtaAlloc(const AMBA_CFS_ACTIVE_DTA_POOL_s *ActiveDtaPool, AMBA_CFS_DTA_s *DirEntry);

/**
 *  Release an Active DTA and break the binding.
 *  @param [in] ActiveDtaPool       The Active DTA Pool
 *  @param [in,out] ActiveDta       The released Active DTA. Break the binding if succeeded.
 *  @return 0 - OK, CFS_ERR_FATAL - Fatal error, others - Error
 */
UINT32 AmbaCFS_ActiveDtaRelease(const AMBA_CFS_ACTIVE_DTA_POOL_s *ActiveDtaPool, AMBA_CFS_ACTIVE_DTA_s *ActiveDta);

/**
 *  Find the Active DTA that is binding with the DirEntry.
 *  @param [in] ActiveDtaPool       The Active DTA Pool
 *  @param [in] DirEntry            The directory entry
 *  @return The returned Active DTA, NULL - Not found
 */
AMBA_CFS_ACTIVE_DTA_s *AmbaCFS_ActiveDtaGetFromDta(const AMBA_CFS_ACTIVE_DTA_POOL_s *ActiveDtaPool, const AMBA_CFS_DTA_s *DirEntry);

/**
 *  Set the cache info in Active DTA.
 *  @param [in] ActiveDta           The Active DTA
 *  @param [in] CachedSearch        Cached search
 *  @param [in] CachedNode          Cached node
 */
void AmbaCFS_ActiveDtaSetCacheInfo(AMBA_CFS_ACTIVE_DTA_s *ActiveDta, void *CachedSearch, void *CachedNode);

/**
 *  Set the search pattern in Active DTA.
 *  @param [in] ActiveDta           The Active DTA
 *  @param [in] RegExp              Regular expression
 *  @param [in] Attr                Search attributes
 */
void AmbaCFS_ActiveDtaSetSearchPattern(AMBA_CFS_ACTIVE_DTA_s *ActiveDta, const char *RegExp, UINT8 Attr);

#endif /* AMBACFS_ACTIVE_DTA_H */
