/**
 * @file AmbaCFS_Search.c
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
#include "AmbaCFS_Util.h"
#include "AmbaCFS_Search.h"
#include <AmbaPrint.h>

/**
 *  Check whether the DTA is been used for search.
 *  @param [in] FsSearch            The FS Search
 *  @return 1 - TRUE, 0 - FALSE
 */
UINT8 AmbaCFS_SearchIsSearching(const AMBA_CFS_FS_SEARCH_s *FsSearch)
{
    /* Must be called with AmbaCFS_Lock */
    return ((FsSearch->Status & AMBA_CFS_FS_SEARCH_STATUS_VALID) != 0U) ? 1U : 0U;
}

#if 0
/**
 *  Print FS Search
 *  @param [in] FsSearch            FS Search
 */
static void AmbaCFS_SearchPrintFsSearch(const AMBA_CFS_FS_SEARCH_s *FsSearch)
{
    if (FsSearch != NULL) {
        ULONG FsSearchUL;
        const AMBA_FS_DTA_t *Dta = &FsSearch->Dta;
        AmbaMisra_TypeCast(&FsSearchUL, &FsSearch);
        AmbaPrint_ModulePrintUInt5(AMBA_CFS_PRINT_MODULE_ID, "FS Search: %p", FsSearchUL, 0, 0, 0, 0);
        AmbaPrint_ModulePrintUInt5(AMBA_CFS_PRINT_MODULE_ID, "------------------------------------", 0, 0, 0, 0, 0);
        AmbaPrint_ModulePrintUInt5(AMBA_CFS_PRINT_MODULE_ID, "FileSize           : %u", (UINT32) Dta->FileSize, 0, 0, 0, 0);
        AmbaPrint_ModulePrintUInt5(AMBA_CFS_PRINT_MODULE_ID, "Date               : %hu", Dta->Date, 0, 0, 0, 0);
        AmbaPrint_ModulePrintUInt5(AMBA_CFS_PRINT_MODULE_ID, "Time               : %hu", Dta->Time, 0, 0, 0, 0);
        AmbaPrint_ModulePrintUInt5(AMBA_CFS_PRINT_MODULE_ID, "Attribute          : %hu", Dta->Attribute, 0, 0, 0, 0);
        AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID,  "FileName           : %s", Dta->FileName, NULL, NULL, NULL, NULL);
        AmbaPrint_ModulePrintUInt5(AMBA_CFS_PRINT_MODULE_ID, "------------------------------------", 0, 0, 0, 0, 0);
    }
}
#endif

/**
 *  Search the first file via AmbaFS
 *  I/O Error handling for User: Need to handle CFS_ERR_FS.
 *  @param [in] Name                Filename
 *  @param [in] Attr                Attribute
 *  @param [out] Search             The FS search
 *  @return 0 - OK, CFS_ERR_FATAL - Fatal error, others - Error
 */
static UINT32 AmbaCFS_SearchAmbaFsFSFirst(const char *Name, UINT8 Attr, AMBA_CFS_FS_SEARCH_s *Search)
{
    UINT32 Ret;
    AMBA_FS_DTA_t *Dta = &Search->Dta;
    Ret = CFS_F2C(AmbaFS_FileSearchFirst(Name, Attr, Dta));
    if ((Ret == CFS_OK) || (Ret == CFS_ERR_OBJ_UNAVAILABLE)) {
        /* Do nothing */
    } else {
        AmbaPrint_ModulePrintUInt5(AMBA_CFS_PRINT_MODULE_ID, "AmbaCFS_SearchAmbaFsFSFirst: [ERROR] AmbaFS_FileSearchFirst fail. ErrNum: %p", AmbaCFS_UtilGetAmbaFSError(), 0, 0, 0, 0);
    }
    return Ret;
}

/**
 *  Search the next file via AmbaFS
 *  I/O Error handling for User: Need to handle CFS_ERR_FS.
 *  @param [in,out] Search          The FS search
 *  @return 0 - OK, CFS_ERR_FATAL - Fatal error, others - Error
 */
static UINT32 AmbaCFS_SearchAmbaFsFSNext(AMBA_CFS_FS_SEARCH_s *Search)
{
    UINT32 Ret;
    AMBA_FS_DTA_t *Dta = &Search->Dta;
    Ret = CFS_F2C(AmbaFS_FileSearchNext(Dta));
    if ((Ret == CFS_OK) || (Ret == CFS_ERR_OBJ_UNAVAILABLE)) {
        /* Do nothing */
    } else {
        AmbaPrint_ModulePrintUInt5(AMBA_CFS_PRINT_MODULE_ID, "AmbaCFS_SearchAmbaFsFSNext: [ERROR] AmbaFS_FileSearchNext fail. ErrNum: %p", AmbaCFS_UtilGetAmbaFSError(), 0, 0, 0, 0);
    }
    return Ret;
}

/**
 *  Finish the file search via AmbaFS
 *  I/O Error handling for User: Need to handle CFS_ERR_FS.
 *  @param [in,out] Search          The FS search
 *  @return 0 - OK, CFS_ERR_FATAL - Fatal error, others - Error
 */
static UINT32 AmbaCFS_SearchAmbaFsFSFinish(AMBA_CFS_FS_SEARCH_s *Search)
{
    UINT32 Ret;
    Ret = CFS_F2C(AmbaFS_FileSearchFinish(&Search->Dta));
    if (Ret != CFS_OK) {
        AmbaPrint_ModulePrintUInt5(AMBA_CFS_PRINT_MODULE_ID, "AmbaCFS_SearchAmbaFsFSFinish: [ERROR] AmbaFS_FileSearchFinish fail. ErrNum: %p", AmbaCFS_UtilGetAmbaFSError(), 0U, 0U, 0U, 0U);
    }
    return Ret;
}

/**
 *  Search the first file
 *  I/O Error handling for User: Need to handle CFS_ERR_FS.
 *  @param [in] Name                Filename
 *  @param [in] Attr                Attribute
 *  @param [out] FsSearch           The FS search
 *  @return 0 - OK, CFS_ERR_FATAL - Fatal error, others - Error
 */
UINT32 AmbaCFS_SearchFileFirst(const char *Name, UINT8 Attr, AMBA_CFS_FS_SEARCH_s *FsSearch)
{
    /* Must be called with AmbaCFS_Lock */
    UINT32 Ret = CFS_ERR_API;
    if (AmbaCFS_SearchIsSearching(FsSearch) == 0U) {
        /*AmbaCFS_SearchPrintFsSearch(FsSearch);*/
        Ret = AmbaCFS_SearchAmbaFsFSFirst(Name, Attr, FsSearch);
        if (Ret == CFS_OK) {
            FsSearch->Status |= AMBA_CFS_FS_SEARCH_STATUS_VALID;
        }
    } else {
        AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] FsSearch is not finished", __func__, NULL, NULL, NULL, NULL);
    }
    return Ret;
}

/**
 *  Search the next file
 *  I/O Error handling for User: Need to handle CFS_ERR_FS.
 *  @param [in,out] FsSearch        The FS search
 *  @return 0 - OK, CFS_ERR_FATAL - Fatal error, others - Error
 */
UINT32 AmbaCFS_SearchFileNext(AMBA_CFS_FS_SEARCH_s *FsSearch)
{
    /* Must be called with AmbaCFS_Lock */
    UINT32 Ret = CFS_ERR_API;
    if (AmbaCFS_SearchIsSearching(FsSearch) == 1U) {
        /*AmbaCFS_SearchPrintFsSearch(FsSearch);*/
        Ret = AmbaCFS_SearchAmbaFsFSNext(FsSearch);
        /*AmbaCFS_SearchPrintFsSearch(FsSearch);*/
    } else {
        AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] FsSearch is invalid", __func__, NULL, NULL, NULL, NULL);
    }
    return Ret;
}

/**
 *  Finish the file search
 *  I/O Error handling for User: Need to handle CFS_ERR_FS.
 *  @param [in,out] FsSearch        The FS search
 *  @return 0 - OK, CFS_ERR_FATAL - Fatal error, others - Error
 */
UINT32 AmbaCFS_SearchFileFinish(AMBA_CFS_FS_SEARCH_s *FsSearch)
{
    /* Must be called with AmbaCFS_Lock */
    UINT32 Ret = CFS_OK;
    if (AmbaCFS_SearchIsSearching(FsSearch) == 1U) {
        Ret = AmbaCFS_SearchAmbaFsFSFinish(FsSearch);
        if (Ret == CFS_OK) {
            FsSearch->Status &= ~AMBA_CFS_FS_SEARCH_STATUS_VALID;
        }
    } else {
        /* Already finished. Could happen when cache enabled and cache hit. Do nothing and return OK. */
    }
    return Ret;
}

/**
 *  Copy data from FsSearch to FileSearch
 *  @param [out] FileSearch         Search DTA
 *  @param [in] FsSearch            The FS search
 *  @return 0 - OK, CFS_ERR_FATAL - Fatal error, others - Error
 */
UINT32 AmbaCFS_SearchCopyDtaToSearch(AMBA_CFS_DTA_s *FileSearch, const AMBA_CFS_FS_SEARCH_s *FsSearch)
{
    /* Must be called with AmbaCFS_Lock */
    UINT32 Ret = CFS_OK;
    if (FileSearch != NULL) {
        const AMBA_FS_DTA_t *Dta = &FsSearch->Dta;
        FileSearch->FileSize = Dta->FileSize;
        FileSearch->Date = Dta->Date;
        FileSearch->Time = Dta->Time;
        FileSearch->Attribute = Dta->Attribute;
        AmbaCFS_UtilCopyFileName(FileSearch->FileName, (const char *)Dta->FileName);
    } else {
        AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] FileSearch is NULL", __func__, NULL, NULL, NULL, NULL);
        Ret = CFS_ERR_API;
    }
    return Ret;
}
