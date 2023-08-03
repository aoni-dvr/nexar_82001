/**
 * @file AmbaCFS_Search.h
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
#ifndef AMBACFS_SEARCH_H
#define AMBACFS_SEARCH_H

#include <AmbaCFS.h>
#include <AmbaFS.h>

/**
 *  Status of FS Search
 */
#define AMBA_CFS_FS_SEARCH_STATUS_VALID     (1U)    /**< DTA of AMBA_CFS_FS_SEARCH_s was used to search and succeeded */

/**
 * FS Search
 */
typedef struct {
    AMBA_FS_DTA_t Dta;                              /**< The real DTA */
    UINT32 Status;                                  /**< Status of the FS search */
} AMBA_CFS_FS_SEARCH_s;

/**
 *  Seach the first file
 *  @param [in] Name                Filename
 *  @param [in] Attr                Attribute
 *  @param [out] FsSearch           The FS search
 *  @return 0 - OK, CFS_ERR_FATAL - Fatal error, others - Error
 */
UINT32 AmbaCFS_SearchFileFirst(const char *Name, UINT8 Attr, AMBA_CFS_FS_SEARCH_s *FsSearch);

/**
 *  Seach the next file
 *  @param [in,out] FsSearch        The FS search
 *  @return 0 - OK, CFS_ERR_FATAL - Fatal error, others - Error
 */
UINT32 AmbaCFS_SearchFileNext(AMBA_CFS_FS_SEARCH_s *FsSearch);

/**
 *  Finish the file search
 *  @param [in,out] FsSearch        The FS search
 *  @return 0 - OK, CFS_ERR_FATAL - Fatal error, others - Error
 */
UINT32 AmbaCFS_SearchFileFinish(AMBA_CFS_FS_SEARCH_s *FsSearch);

/**
 *  Copy data from FsSearch to FileSearch
 *  @param [out] FileSearch         Search DTA
 *  @param [in] FsSearch            The FS search
 *  @return 0 - OK, CFS_ERR_FATAL - Fatal error, others - Error
 */
UINT32 AmbaCFS_SearchCopyDtaToSearch(AMBA_CFS_DTA_s *FileSearch, const AMBA_CFS_FS_SEARCH_s *FsSearch);

/**
 *  Check whether the DTA is been used for search.
 *  @param [in] FsSearch            The FS Search
 *  @return 1 - TRUE, 0 - FALSE
 */
UINT8 AmbaCFS_SearchIsSearching(const AMBA_CFS_FS_SEARCH_s *FsSearch);

#endif /* AMBACFS_SEARCH_H */
