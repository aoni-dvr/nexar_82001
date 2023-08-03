/**
 * @file AmbaCFS_ActiveDta.c
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
#include "AmbaCFS_ActiveDta.h"
#include <AmbaPrint.h>

#define AMBA_CFS_MAX_SEARCH_DEFAULT (8U)    /**< Max number of active DTA */

/**
 *  Whether Active DTA is valid.
 *  @param [in] ActiveDtaPool       The Active DTA Pool (Internal data. No need to check)
 *  @param [in] ActiveDta           The Active DTA
 *  @param [in] DirEntry            The directory entry that should be binded with ActiveDta
 *  @return 1 - TRUE, 0 - FALSE
 */
static UINT8 AmbaCFS_ActiveDtaIsValid(const AMBA_CFS_ACTIVE_DTA_POOL_s *ActiveDtaPool, const AMBA_CFS_ACTIVE_DTA_s *ActiveDta, const AMBA_CFS_DTA_s *DirEntry)
{
    /* Must be called with AmbaCFS_Lock */
    UINT8 Ret = 0U;
    const AMBA_CFS_ACTIVE_DTA_s *BufferBase = ActiveDtaPool->ActiveDta;
    ULONG AddrUL;
    ULONG BufferBaseUL;
    const UINT32 Size = (UINT32)sizeof(AMBA_CFS_ACTIVE_DTA_s);
    const UINT32 MaxNum = ActiveDtaPool->MaxSearch;
    const void *ActiveDtaPtr = ActiveDta;
    AmbaMisra_TypeCast(&AddrUL, &ActiveDta);
    AmbaMisra_TypeCast(&BufferBaseUL, &BufferBase);
    if (AddrUL < BufferBaseUL) {
        AmbaPrint_ModulePrintUInt5(AMBA_CFS_PRINT_MODULE_ID, "AmbaCFS_ActiveDtaIsValid: [ERROR] Address too small. ActiveDta %p Base %p", (UINT32)AddrUL, (UINT32)BufferBaseUL, 0U, 0U, 0U);
    } else if (((AddrUL - BufferBaseUL) / Size) >= MaxNum) {
        AmbaPrint_ModulePrintUInt5(AMBA_CFS_PRINT_MODULE_ID, "AmbaCFS_ActiveDtaIsValid: [ERROR] Address too large. ActiveDta %p Base %p Size %p MaxNum %u", (UINT32)AddrUL, (UINT32)BufferBaseUL, Size, MaxNum, 0U);
    } else if (((AddrUL - BufferBaseUL) % Size) != 0U) {
        AmbaPrint_ModulePrintUInt5(AMBA_CFS_PRINT_MODULE_ID, "AmbaCFS_ActiveDtaIsValid: [ERROR] Address not aligned. ActiveDta %p Base %p Size %p MaxNum %u", (UINT32)AddrUL, (UINT32)BufferBaseUL, Size, MaxNum, 0U);
    } else if (ActiveDta->IsUsed == 0U) {
        AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] ActiveDta is not used", __func__, NULL, NULL, NULL, NULL);
    } else if (ActiveDta->DirEntry == NULL) {
        AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] DirEntry is NULL", __func__, NULL, NULL, NULL, NULL);
    } else if (ActiveDta->DirEntry->SearchInfo != ActiveDtaPtr) {
        AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] DirEntry is not binding with ActiveDta", __func__, NULL, NULL, NULL, NULL);
    } else if (ActiveDta->DirEntry != DirEntry) {
        /*
         * This check is redundant in AmbaCFS_ActiveDtaRelease (because the input is from ActiveDta->DirEntry) but necessary in AmbaCFS_ActiveDtaGetFromDta.
         * Consider this case:
         *     DirEntry1 and ActiveDta1 are binding together
         *     Call AmbaCFS_ActiveDtaGetFromDta using an invalid DirEntry2 with its SearchInfo pointing to ActiveDta1
         *     ==> This check will find out the error
         */
        AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] ActiveDta is not binding with DirEntry", __func__, NULL, NULL, NULL, NULL);
    } else {
        Ret = 1U;
    }
    return Ret;
}

/**
 *  Get required buffer size
 *  @param [in] MaxSearch           The max unfinalized search operations. See AmbaCFS_FileSearchFirst().
 *  @return Required buffer size
 */
UINT32 AmbaCFS_ActiveDtaGetInitBufferSize(UINT32 MaxSearch)
{
    return (MaxSearch * (UINT32)sizeof(AMBA_CFS_ACTIVE_DTA_s));
}

/**
 *  Get the default configuration for initializing the Active DTA.
 *  @param [out] Config             The returned configuration of the Active DTA
 *  @return 0 - OK, others - Error
 */
void AmbaCFS_ActiveDtaGetInitDefaultCfg(AMBA_CFS_INIT_CFG_s *Config)
{
    Config->MaxSearch = AMBA_CFS_MAX_SEARCH_DEFAULT;
}

/**
 *  Check initialization config.
 *  @param [in] Buffer              The work buffer
 *  @param [in] BufferSize          The size of the work buffer
 *  @param [in] MaxSearch           The max unfinalized search operations. See AmbaCFS_FileSearchFirst().
 *  @param [in] ActiveDtaPool       The Active DTA Pool
 *  @return 0 - OK, others - Error
 */
static UINT32 AmbaCFS_ActiveDtaInit_ParamCheck(const UINT8 *Buffer, UINT32 BufferSize, UINT32 MaxSearch, const AMBA_CFS_ACTIVE_DTA_POOL_s *ActiveDtaPool)
{
    UINT32 Ret = CFS_ERR_ARG;
    if (Buffer != NULL) {
        if (MaxSearch > 0U) {
            if (BufferSize == AmbaCFS_ActiveDtaGetInitBufferSize(MaxSearch)) {
                if (ActiveDtaPool != NULL) {
                    Ret = CFS_OK;
                } else {
                    AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] ActiveDtaPool is NULL", __func__, NULL, NULL, NULL, NULL);
                }
            } else {
                AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] BufferSize is incorrect", __func__, NULL, NULL, NULL, NULL);
            }
        } else {
            AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] MaxSearch is incorrect", __func__, NULL, NULL, NULL, NULL);
        }
    } else {
        AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] Buffer is NULL", __func__, NULL, NULL, NULL, NULL);
    }
    return Ret;
}

/**
 *  Initialize Active DTA.
 *  @param [in] Buffer              The work buffer
 *  @param [in] BufferSize          The size of the work buffer
 *  @param [in] MaxSearch           The max unfinalized search operations. See AmbaCFS_FileSearchFirst().
 *  @param [out] ActiveDtaPool      The Active DTA Pool
 *  @return 0 - OK, CFS_ERR_FATAL - Fatal error, others - Error
 */
UINT32 AmbaCFS_ActiveDtaInit(UINT8 *Buffer, UINT32 BufferSize, UINT32 MaxSearch, AMBA_CFS_ACTIVE_DTA_POOL_s *ActiveDtaPool)
{
    UINT32 Ret = AmbaCFS_ActiveDtaInit_ParamCheck(Buffer, BufferSize, MaxSearch, ActiveDtaPool);
    if (Ret == CFS_OK) {
        AmbaUtility_MemorySetU8(Buffer, 0, BufferSize);
        AmbaMisra_TypeCast(&ActiveDtaPool->ActiveDta, &Buffer);
        ActiveDtaPool->MaxSearch = MaxSearch;
    }
    return Ret;
}

/**
 *  Allocate an Active DTA and set the binding.
 *  @param [in] ActiveDtaPool       The Active DTA Pool
 *  @param [in,out] DirEntry        Address of a directory entry record. Set the binding if succeeded.
 *  @return The returned Active DTA. NULL - Error
 */
AMBA_CFS_ACTIVE_DTA_s *AmbaCFS_ActiveDtaAlloc(const AMBA_CFS_ACTIVE_DTA_POOL_s *ActiveDtaPool, AMBA_CFS_DTA_s *DirEntry)
{
    /* Must be called with AmbaCFS_Lock */
    UINT8 IsDirEntryExist = 0U;
    AMBA_CFS_ACTIVE_DTA_s *RetActiveDta = NULL;
    /*
        It's less efficient to scan all the ActiveDtaPool.
        However, MaxSearch would not be large because it takes a lot of spaces.
        So it should be fast to scan the entire ActiveDtaPool.
     */
    for (UINT32 i = 0U; i < ActiveDtaPool->MaxSearch; ++i) {
        AMBA_CFS_ACTIVE_DTA_s *ActiveDta = &ActiveDtaPool->ActiveDta[i];

        if (ActiveDta->IsUsed == 1U) {
            /* Check the binding for all valid ActiveDta. Should not find DirEntry. */
            if (ActiveDta->DirEntry == DirEntry) {
                /* DirEntry already exists. DirEntry should be finished before calling another "Search First". */
                IsDirEntryExist = 1U;
                break;
            }
        } else {
            if (RetActiveDta == NULL) {
                /* Return the first available ActiveDta. Do not break this "for loop" because all the binding should be checked. */
                RetActiveDta = ActiveDta;
            }
        }
    }

    if (IsDirEntryExist == 1U) {
        AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] Search is not finished", __func__, NULL, NULL, NULL, NULL);
        RetActiveDta = NULL; /* Error. Return NULL. */
    } else {
        if (RetActiveDta == NULL) {
            /* One of the ActiveDta should be finished before starting a new search. */
            /* Or increase MaxSearch in AmbaCFS_Init */
            AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] No ActiveDta available. Finish one of the search or increase MaxSearch.", __func__, NULL, NULL, NULL, NULL);
            /* Error. Return NULL. */
        } else {
            /* Initialize Active DTA */
            AmbaUtility_MemorySetU8((UINT8 *) RetActiveDta, 0, (UINT32)sizeof(AMBA_CFS_ACTIVE_DTA_s));
            RetActiveDta->IsUsed = 1U;
            /* Set the binding of ActiveDta and DirEntry */
            DirEntry->SearchInfo = RetActiveDta;
            RetActiveDta->DirEntry = DirEntry;
        }
    }
    return RetActiveDta;
}

/**
 *  Release an Active DTA and break the binding.
 *  @param [in] ActiveDtaPool       The Active DTA Pool
 *  @param [in,out] ActiveDta       The released Active DTA. Break the binding if succeeded.
 *  @return 0 - OK, CFS_ERR_FATAL - Fatal error, others - Error
 */
UINT32 AmbaCFS_ActiveDtaRelease(const AMBA_CFS_ACTIVE_DTA_POOL_s *ActiveDtaPool, AMBA_CFS_ACTIVE_DTA_s *ActiveDta)
{
    /* Must be called with AmbaCFS_Lock */
    UINT32 Ret = CFS_ERR_API;
    if (AmbaCFS_ActiveDtaIsValid(ActiveDtaPool, ActiveDta, ActiveDta->DirEntry) == 1U) {
        if (AmbaCFS_SearchIsSearching(&ActiveDta->FsSearch) == 0U) {
            AMBA_CFS_DTA_s *DirEntry = ActiveDta->DirEntry;
            /* Break the binding of ActiveDta and DirEntry */
            DirEntry->SearchInfo = NULL;
            ActiveDta->DirEntry = NULL;
            /* Release Active DTA */
            ActiveDta->IsUsed = 0U;
            Ret = CFS_OK;
        } else {
            AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] Search is not finished", __func__, NULL, NULL, NULL, NULL);
        }
    } else {
        AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] ActiveDta is not valid", __func__, NULL, NULL, NULL, NULL);
    }
    return Ret;
}

/**
 *  Find the Active DTA that is binding with the DirEntry.
 *  @param [in] ActiveDtaPool       The Active DTA Pool
 *  @param [in] DirEntry            The directory entry
 *  @return The returned Active DTA, NULL - Not found
 */
AMBA_CFS_ACTIVE_DTA_s *AmbaCFS_ActiveDtaGetFromDta(const AMBA_CFS_ACTIVE_DTA_POOL_s *ActiveDtaPool, const AMBA_CFS_DTA_s *DirEntry)
{
    /* Must be called with AmbaCFS_Lock */
    AMBA_CFS_ACTIVE_DTA_s *Ret = NULL;
    AMBA_CFS_ACTIVE_DTA_s *ActiveDta = NULL;
    AmbaMisra_TypeCast(&ActiveDta, &DirEntry->SearchInfo); /* It's already confirmed that DirEntry != NULL */
    if (AmbaCFS_ActiveDtaIsValid(ActiveDtaPool, ActiveDta, DirEntry) == 1U) {
        /* Return ActiveDta. */
        Ret = ActiveDta;
    } else {
        AmbaPrint_ModulePrintStr5(AMBA_CFS_PRINT_MODULE_ID, "%s: [ERROR] ActiveDta is not valid", __func__, NULL, NULL, NULL, NULL);
    }
    return Ret;
}

/**
 *  Set the cache info in Active DTA.
 *  @param [in] ActiveDta           The Active DTA
 *  @param [in] CachedSearch        Cached search
 *  @param [in] CachedNode          Cached node
 */
void AmbaCFS_ActiveDtaSetCacheInfo(AMBA_CFS_ACTIVE_DTA_s *ActiveDta, void *CachedSearch, void *CachedNode)
{
    /* Must be called with AmbaCFS_Lock */
    ActiveDta->CachedSearch = CachedSearch;
    ActiveDta->CachedNode = CachedNode;
}

/**
 *  Set the search pattern in Active DTA.
 *  @param [in] ActiveDta           The Active DTA
 *  @param [in] RegExp              Regular expression
 *  @param [in] Attr                Search attributes
 */
void AmbaCFS_ActiveDtaSetSearchPattern(AMBA_CFS_ACTIVE_DTA_s *ActiveDta, const char *RegExp, UINT8 Attr)
{
    /* Must be called with AmbaCFS_Lock */
    AmbaCFS_UtilCopyFileName(ActiveDta->RegExp, RegExp);
    ActiveDta->Attr = Attr;
}

