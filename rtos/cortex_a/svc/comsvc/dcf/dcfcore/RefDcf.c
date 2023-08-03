/**
 *  @file RefDcf.c
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
 *  @details DCF Main Module
 *
 */
#include "RefDcf_Util.h"
#include "RefDcfImpl.h"
#include <AmbaDef.h>
#include <AmbaMisraFix.h>

#define DCF_DEFAULT_MAX_HDLR            (2U)
#define DCF_DEFAULT_MAX_ROOT_PER_HDLR   (5U)
#define DCF_DEFAULT_MAX_DIR_NUM         (999U)
#define DCF_DEFAULT_MAX_DIR_PER_HDLR    (900U)
#define DCF_DEFAULT_MAX_FILE_PER_HDLR   (200000U)
#define DCF_DEFAULT_MAX_DIR_PER_DNUM    (8U)

typedef struct {
    AMBA_KAL_MUTEX_t Mutex;
    REF_DCF_HDLR_IMPL_s *Hdlr;
    UINT8 MaxHdlr;
    UINT8 MaxRootPerHdlr;
    UINT32 MaxDnum;
} REF_DCF_s;

static REF_DCF_s g_Dcf = {0};

static inline void Dcf_Perror(const char *Func, UINT32 Line, const char *Message)
{
    char LineStr[8];
    (void)AmbaUtility_UInt32ToStr(LineStr, sizeof(LineStr), Line, 10);
    AmbaPrint_PrintStr5("[Error]%s:%s %s", Func, LineStr, Message, NULL, NULL);
}

static inline UINT8 IS_DCF_HDLR(const REF_DCF_HDLR_s *Hdlr)
{
    UINT8 Rval = 0U;
    if (Hdlr != NULL) {
        UINT32 HdlrU32, HdlrBaseU32;
        AmbaMisra_TypeCast32(&HdlrU32, &Hdlr);
        AmbaMisra_TypeCast32(&HdlrBaseU32, &g_Dcf.Hdlr);
        if (HdlrU32 >= HdlrBaseU32) {
            if (HdlrU32 < (HdlrBaseU32 + (g_Dcf.MaxHdlr * sizeof(REF_DCF_HDLR_IMPL_s)))) {
                if (((HdlrU32 - HdlrBaseU32) % sizeof(REF_DCF_HDLR_IMPL_s)) == 0U) {
                    Rval = 1U;
                }
            }
        }
    }
    return Rval;
}

UINT8 RefDcf_IsValidDnum(const UINT32 Dnum)
{
    UINT8 Rval = 0U;
    if ((Dnum != ID_NOT_EXISTED) && (Dnum <= g_Dcf.MaxDnum)) {
        Rval = 1U;
    }
    return Rval;
}

// return UINT32 for misra c check
static inline UINT32 SET_ROOT_DIRTY(const REF_DCF_ROOT_INFO_s *Root)
{
    Root->DnumDirty[0] |= 0x0000000000000001ULL;
    return DCF_OK;
}

// return UINT32 for misra c check
static inline UINT32 CLR_ROOT_DIRTY(const REF_DCF_ROOT_INFO_s *Root)
{
    Root->DnumDirty[0] &= (~0x0000000000000001ULL);
    return DCF_OK;
}

static inline UINT8 IS_ROOT_DIRTY(const REF_DCF_ROOT_INFO_s *Root)
{
    UINT8 Rval = 0U;
    if ((Root->DnumDirty[0] & 0x0000000000000001ULL) != 0ULL) {
        Rval = 1U;
    }
    return Rval;
}

static inline UINT32 RefDcf_SetRootDnumDirty(const REF_DCF_ROOT_INFO_s *Root, UINT32 Dnum)
{
    UINT32 Rval = DCF_OK;
    if (RefDcf_IsValidDnum(Dnum) != 0U) {
        Root->DnumDirty[Dnum / 64U] |= (0x0000000000000001ULL << (Dnum % 64U));
        // set the root as dirty
        Rval = SET_ROOT_DIRTY(Root);
    } else {
        Dcf_Perror(__FUNCTION__, __LINE__, "invalid Dnum");
        Rval = DCF_ERR_GENERAL_ERROR;
    }
    return Rval;
}

static UINT32 RefDcf_GetInitBufferSize_ParamCheck(const REF_DCF_INIT_CFG_s *Config)
{
    UINT32 Rval = DCF_OK;
    if ((Config->MaxDnum == 0UL) || (Config->MaxHdlr == 0U) || (Config->MaxRootPerHdlr == 0U) || (Config->MaxDirPerDnum == 0U)) {
        Dcf_Perror(__FUNCTION__, __LINE__, "Invalid argument!");
        Rval = DCF_ERR_INVALID_ARG;
    }
    return Rval;

}

static inline UINT32 ROOT_DIRTY_SIZE(UINT32 MaxDnum)
{
    return (GetAlignedValU32(MaxDnum + 1U, 64U) / 8U);
}

UINT32 RefDcf_GetInitBufferSize(const REF_DCF_INIT_CFG_s *Config, UINT32 *BufferSize)
{
    UINT32 Rval = RefDcf_GetInitBufferSize_ParamCheck(Config);
    if (Rval == DCF_OK) {
        // core => hdlr
        UINT32 Size;
        Rval = RefDcf_GetCoreBufferSize(Config->MaxDirPerDnum, &Size);
        if (Rval == DCF_OK) {
           Size += (UINT32)Config->MaxHdlr * sizeof(REF_DCF_HDLR_IMPL_s);
           Size += (UINT32)Config->MaxHdlr * (UINT32)Config->MaxRootPerHdlr * sizeof(REF_DCF_ROOT_INFO_s);
           // keep the bit mask of each dnum, mark all bit as 0 at init and make the bits as 1 in ScanRoots() if the corresponding dnum is valid
           Size += (UINT32)Config->MaxHdlr * (UINT32)Config->MaxRootPerHdlr * ROOT_DIRTY_SIZE(Config->MaxDnum);
        }
        if (Rval == DCF_OK) {
             *BufferSize = GetAlignedValU32(Size, AMBA_CACHE_LINE_SIZE);
        }
    }
    return Rval;
}

/**
 * Get the default configuration for initializing the DCF module.
 * @param [out] Config The returned configuration of the DCF module
 * @return 0 - OK, others - DCF_ERR_XXX
 */
UINT32 RefDcf_GetInitDefaultCfg(REF_DCF_INIT_CFG_s *Config)
{
    UINT32 Rval;
    if (Config == NULL) {
        Dcf_Perror(__FUNCTION__, __LINE__, "Invalid argument!");
        Rval = DCF_ERR_INVALID_ARG;
    } else {
        (void)AmbaWrap_memset(Config, 0, sizeof(REF_DCF_INIT_CFG_s));
        Config->MaxHdlr = DCF_DEFAULT_MAX_HDLR;
        Config->MaxRootPerHdlr = DCF_DEFAULT_MAX_ROOT_PER_HDLR;
        Config->MaxDnum = DCF_DEFAULT_MAX_DIR_NUM;
        Config->MaxDirPerDnum = DCF_DEFAULT_MAX_DIR_PER_DNUM;
        Rval = RefDcf_GetInitBufferSize(Config, &Config->BufferSize);
    }
    return Rval;
}

static UINT32 RefDcf_InitImpl(const REF_DCF_INIT_CFG_s *Config)
{
    UINT32 GetBufSize;
    UINT32 Rval = RefDcf_GetInitBufferSize(Config, &GetBufSize);
    if (Rval == DCF_OK) {
        if (Config->BufferSize != GetBufSize) {
            Dcf_Perror(__FUNCTION__, __LINE__, "Invalid argument!");
            Rval = DCF_ERR_INVALID_ARG;
        } else {
            // core => hdlr => table
            UINT32 Size;
            Rval = RefDcf_GetCoreBufferSize(Config->MaxDirPerDnum, &Size);
            if (Rval == DCF_OK) {
                // 1. Init DCF Core
                Rval = RefDcf_InitCore(Config->MaxDirPerDnum, Config->Buffer, Size);
                if (Rval == DCF_OK) {
                    UINT32 AddrU32, i, j;
                    REF_DCF_HDLR_IMPL_s *Hdlr;
                    AmbaMisra_TypeCast32(&AddrU32, &Config->Buffer);
                    AddrU32 += Size;
                    // 2. Init DCF handlers
                    g_Dcf.MaxHdlr = Config->MaxHdlr;
                    Size = (UINT32)g_Dcf.MaxHdlr * sizeof(REF_DCF_HDLR_IMPL_s);
                    AmbaMisra_TypeCast32(&g_Dcf.Hdlr, &AddrU32);
                    AddrU32 += Size;
                    // 3. Allocate DCF roots of all handlers
                    g_Dcf.MaxRootPerHdlr = Config->MaxRootPerHdlr;
                    Size = (UINT32)g_Dcf.MaxRootPerHdlr * sizeof(REF_DCF_ROOT_INFO_s);
                    for (i = 0; i < g_Dcf.MaxHdlr; i++) {
                        Hdlr = &g_Dcf.Hdlr[i];
                        Hdlr->State = DCF_STATE_UNUSED;
                        AmbaMisra_TypeCast32(&Hdlr->RootInfo, &AddrU32);
                        AddrU32 += Size;
                    }
                    // 4. Allocate DCF roots's DnumDirty mask
                    g_Dcf.MaxDnum = Config->MaxDnum;
                    Size = ROOT_DIRTY_SIZE(g_Dcf.MaxDnum);
                    for (i = 0; i < g_Dcf.MaxHdlr; i++) {
                        Hdlr = &g_Dcf.Hdlr[i];
                        for (j = 0; j < g_Dcf.MaxRootPerHdlr; j++) {
                            AmbaMisra_TypeCast32(&Hdlr->RootInfo[j].DnumDirty, &AddrU32);
                            AddrU32 += Size;
                        }
                    }
                }
            }
        }
    }
    return Rval;
}

/**
 * Initialize the DCF module.
 * @param [in] Config The configuration used to initialize the DCF module
 * @return 0 - OK, others - DCF_ERR_XXX
 */
UINT32 RefDcf_Init(const REF_DCF_INIT_CFG_s *Config)
{
    UINT32 Rval = DCF_OK;
    UINT32 AddrU32;
    AmbaMisra_TypeCast32(&AddrU32, &Config->Buffer);
    if ((Config == NULL) || (Config->Buffer == NULL) || ((AddrU32 % AMBA_CACHE_LINE_SIZE) != 0U) || (Config->MaxHdlr == 0U) || (Config->MaxRootPerHdlr == 0U) || (Config->MaxDirPerDnum == 0U)) {
        Dcf_Perror(__FUNCTION__, __LINE__, "Invalid argument!");
        Rval = DCF_ERR_INVALID_ARG;
    } else {
        static UINT8 DcfInit = 0U;
        if (DcfInit == 0U) {
            static char MutexName[32] = "RefDcf_Init";
            Rval = K2D(AmbaKAL_MutexCreate(&g_Dcf.Mutex, MutexName));
            if (Rval == DCF_OK) {
                Rval = RefDcf_InitImpl(Config);
                if (Rval == DCF_OK) {
                    DcfInit = 1U;
                }
            } else {
                Dcf_Perror(__FUNCTION__, __LINE__, "cannot create mutex");
            }
        } else {
            Dcf_Perror(__FUNCTION__, __LINE__, "already initiated");
            Rval = DCF_ERR_GENERAL_ERROR;
        }
    }
    return Rval;
}

/**
 * Get the default configuration for creating a DCF handler.
 * @param [out] Config The returned configuration of a DCF handler
 * @return 0 - OK, others - DCF_ERR_XXX
 */
UINT32 RefDcf_GetDefaultCfg(REF_DCF_CFG_s *Config)
{
    UINT32 Rval = DCF_OK;
    if (Config == NULL) {
        Dcf_Perror(__FUNCTION__, __LINE__, "Invalid argument!");
        Rval = DCF_ERR_INVALID_ARG;
    } else {
        (void)AmbaWrap_memset(Config, 0, sizeof(REF_DCF_CFG_s));
        Config->MaxDir = DCF_DEFAULT_MAX_DIR_PER_HDLR;
        Config->MaxFile = DCF_DEFAULT_MAX_FILE_PER_HDLR;
    }
    return Rval;
}

static UINT32 RefDcf_CreateImpl(REF_DCF_FILTER_s *Filter, const REF_DCF_TABLE_s *Table, UINT32 MaxDir, UINT32 MaxFile, REF_DCF_HDLR_IMPL_s **Hdlr)
{
    UINT32 Rval = DCF_OK;
    UINT32 i;
    REF_DCF_HDLR_IMPL_s *DcfHdlr;
    for (i = 0; i < g_Dcf.MaxHdlr; i++) {
        DcfHdlr = &g_Dcf.Hdlr[i];
        if (DcfHdlr->State == DCF_STATE_UNUSED) {
            Rval = RefDcf_CreateTable(MaxDir, MaxFile, Table, &DcfHdlr->Table);
            if (Rval == DCF_OK) {
                UINT32 j;
                for (j = 0; j < g_Dcf.MaxRootPerHdlr; j++) {
                    DcfHdlr->RootInfo[j].Used = 0U;
                }
                DcfHdlr->Filter = Filter;
                DcfHdlr->CurDcfDnum = ID_NOT_EXISTED;
                DcfHdlr->CurDcfId = ID_NOT_EXISTED;
                DcfHdlr->State = DCF_STATE_IDLE;
                *Hdlr = DcfHdlr;
            }
            break;
        }
    }
    if (i == g_Dcf.MaxHdlr) {
        Dcf_Perror(__FUNCTION__, __LINE__, "Get Hdlr fail!");
        Rval = DCF_ERR_GENERAL_ERROR;
    }
    return Rval;
}

/**
 * Create a DCF handler. The system can have multiple DCF handlers.
 * @param [in] Config The default configuration for creating a DCF handler
 * @param [out] Hdlr The returned DCF handler
 * @return 0 - OK, others - DCF_ERR_XXX
 */
UINT32 RefDcf_Create(const REF_DCF_CFG_s *Config, REF_DCF_HDLR_s **Hdlr)
{
    UINT32 Rval;
    if ((Config == NULL) || (Hdlr == NULL) || (Config->Filter == NULL) || (Config->Table == NULL) || (Config->MaxDir == 0U) || (Config->MaxFile == 0U)) {
        Dcf_Perror(__FUNCTION__, __LINE__, "Invalid argument!");
        Rval = DCF_ERR_INVALID_ARG;
    } else {
        Rval = K2D(AmbaKAL_MutexTake(&g_Dcf.Mutex, DCF_TIMEOUT_MUTEX));
        if (Rval == DCF_OK) {
            REF_DCF_HDLR_IMPL_s *DcfHdlr;
            Rval = RefDcf_CreateImpl(Config->Filter, Config->Table, Config->MaxDir, Config->MaxFile, &DcfHdlr);
            if (Rval == DCF_OK) {
                *Hdlr = &DcfHdlr->Hdlr;
            }
            if (AmbaKAL_MutexGive(&g_Dcf.Mutex) != OK) {
                Dcf_Perror(__FUNCTION__, __LINE__, "cannot give mutex");
                Rval = DCF_ERR_GENERAL_ERROR;
            }
        } else {
            Dcf_Perror(__FUNCTION__, __LINE__, "cannot take mutex");
        }
    }
    return Rval;
}

static UINT32 RefDcf_DeleteImpl(REF_DCF_HDLR_IMPL_s *Hdlr)
{
    UINT32 Rval;
    if (Hdlr->State >= DCF_STATE_IDLE) {
        REF_DCF_TABLE_HDLR_s * const Table = Hdlr->Table;
        Rval = RefDcf_DeleteTable(Table->Func, Table);
        if (Rval == DCF_OK) {
            Hdlr->State = DCF_STATE_UNUSED;
            Hdlr->Table = NULL; // set table as NULL, to avoid cache problem in GetHdlrByTable()
        }
    } else {
        Dcf_Perror(__FUNCTION__, __LINE__, "not in used");
        Rval = DCF_ERR_GENERAL_ERROR;
    }
    return Rval;
}

/**
 * Delete a DCF handler.
 * @param [in] Hdlr The DCF handler being deleted
 * @return 0 - OK, others - DCF_ERR_XXX
 */
UINT32 RefDcf_Delete(REF_DCF_HDLR_s *Hdlr)
{
    UINT32 Rval = DCF_OK;
    if (IS_DCF_HDLR(Hdlr) == 0U) {
        Dcf_Perror(__FUNCTION__, __LINE__, "Invalid argument!");
        Rval = DCF_ERR_INVALID_ARG;
    } else {
        AmbaMisra_TouchUnused(&Hdlr->Resv[0]);
        Rval = K2D(AmbaKAL_MutexTake(&g_Dcf.Mutex, DCF_TIMEOUT_MUTEX));
        if (Rval == DCF_OK) {
            REF_DCF_HDLR_IMPL_s *DcfHdlr;
            AmbaMisra_TypeCast32(&DcfHdlr, &Hdlr);
            Rval = RefDcf_DeleteImpl(DcfHdlr);
            if (AmbaKAL_MutexGive(&g_Dcf.Mutex) != OK) {
                Dcf_Perror(__FUNCTION__, __LINE__, "cannot give mutex");
                Rval = DCF_ERR_GENERAL_ERROR;
            }
        } else {
            Dcf_Perror(__FUNCTION__, __LINE__, "cannot take mutex");
        }
    }
    return Rval;
}

static inline REF_DCF_HDLR_IMPL_s *GetHdlrByTable(const REF_DCF_TABLE_HDLR_s *Table)
{
    static REF_DCF_HDLR_IMPL_s *Cache = NULL;
    REF_DCF_HDLR_IMPL_s *Rval = NULL;
    if (Cache != NULL) {
        if (Cache->State >= DCF_STATE_IDLE) {
            if (Cache->Table == Table) {
                Rval = Cache;
            }
        } else {
            Cache = NULL;
        }
    }
    if (Rval == NULL) {
        UINT32 i;
        REF_DCF_HDLR_IMPL_s *Temp;
        for (i = 0; i < g_Dcf.MaxHdlr; i++) {
            Temp = &g_Dcf.Hdlr[i];
            if (Temp->State >= DCF_STATE_IDLE) {
                if (Temp->Table == Table) {
                    Rval = Temp;
                    break;
                }
            }
        }
    }
    Cache = Rval;   // update cache
    return Rval;
}

static UINT32 DirInfo(const REF_DCF_TABLE_HDLR_s *Table, const char *Name, UINT32 Dnum)
{
    UINT32 Rval = DCF_OK;
    if ((Table == NULL) || (Name == NULL) || (RefDcf_IsValidDnum(Dnum) == 0U)) {
        Dcf_Perror(__FUNCTION__, __LINE__, "incorrect parameter!");
        Rval = DCF_ERR_INVALID_ARG;
    } else {
        const REF_DCF_HDLR_IMPL_s * const Hdlr = GetHdlrByTable(Table);
        if (Hdlr != NULL) {
            UINT32 i;
            const REF_DCF_ROOT_INFO_s *Root;
            // TODO: cache the root?
            // find root
            for (i = 0; i < g_Dcf.MaxRootPerHdlr; i++) {
                Root = &Hdlr->RootInfo[i];
                if (Root->Used != 0U) {
                    //AmbaPrint_PrintStr5("%s(%s) : %s", __FUNCTION__, Name, Root->Path, NULL, NULL);
                    if (RefDcf_CompareRoot(Root, Name) != 0U) {
                        // set the Dnum as dirty
                        Rval = RefDcf_SetRootDnumDirty(Root, Dnum);
                        break;
                    }
                }
            }
            if (i == g_Dcf.MaxRootPerHdlr) {
                Dcf_Perror(__FUNCTION__, __LINE__, "No such root!");
                Rval = DCF_ERR_GENERAL_ERROR;
            }
        } else {
            Dcf_Perror(__FUNCTION__, __LINE__, "No such handler!");
            Rval = DCF_ERR_GENERAL_ERROR;
        }
    }
    return Rval;
}

static UINT32 PathCompare(const char *pString1, const char *pString2)
{
    UINT32 Count = 0;
    do {
        if ((pString1[Count] != pString2[Count]) || (pString1[Count] == '\0') || (pString2[Count] == '\0')) {
            break;
        }
        Count++;
    } while (Count < AMBA_DCF_MAX_FILENAME_LENGTH);
    return Count;
}

static UINT32 RefDcf_AddRootImpl(REF_DCF_HDLR_IMPL_s *Hdlr, char *Path)
{
    UINT32 Rval = DCF_OK;
    if ((Hdlr->State == DCF_STATE_IDLE) || (Hdlr->State == DCF_STATE_ROOT) || (Hdlr->State == DCF_STATE_DONE)) {
        UINT32 i;
        REF_DCF_ROOT_INFO_s *Temp;
        REF_DCF_ROOT_INFO_s *Root = NULL;
        const UINT32 Len = AmbaUtility_StringLength(Path);
        // find free root info, also check path conflict
        for (i = 0; i < g_Dcf.MaxRootPerHdlr; i++) {
            Temp = &Hdlr->RootInfo[i];
            if (Temp->Used == 0U) {
                if (Root == NULL) {
                    // keep the first free root
                    Root = Temp;
                }
            } else {
                // check path conflict
                const UINT32 CurLen = AmbaUtility_StringLength(Temp->Path);
                UINT32 Ret = PathCompare(Path, Temp->Path);
                if ((Ret == Len) || (Ret == CurLen)) {
                    Dcf_Perror(__FUNCTION__, __LINE__, "root conflict!");
                    Rval = DCF_ERR_GENERAL_ERROR;
                }
            }
            if (Rval != DCF_OK) {
                break;
            }
        }
        if (Rval == DCF_OK) {
            if (Root != NULL) {
                Hdlr->State = DCF_STATE_ROOT;   // set state to root
                AmbaUtility_StringCopy(Root->Path, AMBA_DCF_MAX_FILENAME_LENGTH, Path);
                Root->Path[AMBA_DCF_MAX_FILENAME_LENGTH - 1U] = '\0';
                // clear dirty mask
                (void)AmbaWrap_memset(Root->DnumDirty, 0, ROOT_DIRTY_SIZE(g_Dcf.MaxDnum));
                Root->Used = 1U;
                Rval = RefDcf_ScanDirToTable(Hdlr->Filter, Hdlr->Table, Path, DirInfo);
                if (Rval != DCF_OK) {
                    Root->Used = 0U; // failed, release the root
                }
            } else {
                Dcf_Perror(__FUNCTION__, __LINE__, "No free root slot!");
                Rval = DCF_ERR_GENERAL_ERROR;
            }
        }
    } else {
        Dcf_Perror(__FUNCTION__, __LINE__, "Incorrect state!");
        Rval = DCF_ERR_GENERAL_ERROR;
    }
    return Rval;
}

/**
 * Add a DCF root into a DCF handler. This only scan sub-directories into the DCF table.
 * @param [in] Hdlr The DCF handler
 * @param [in] Path The path of a DCF root being added (e.g., c:\DCIM)
 * @return 0 - OK, others - DCF_ERR_XXX
 */
UINT32 RefDcf_AddRoot(REF_DCF_HDLR_s *Hdlr, char *Path)
{
    UINT32 Rval = DCF_OK;
    if ((IS_DCF_HDLR(Hdlr) == 0U) || (Path == NULL)) {
        Dcf_Perror(__FUNCTION__, __LINE__, "Invalid argument!");
        Rval = DCF_ERR_INVALID_ARG;
    } else {
        AmbaMisra_TouchUnused(&Hdlr->Resv[0]);
        Rval = K2D(AmbaKAL_MutexTake(&g_Dcf.Mutex, DCF_TIMEOUT_MUTEX));
        if (Rval == DCF_OK) {
            REF_DCF_HDLR_IMPL_s *DcfHdlr;
            AmbaMisra_TypeCast32(&DcfHdlr, &Hdlr);
            Rval = RefDcf_AddRootImpl(DcfHdlr, Path);
            if (AmbaKAL_MutexGive(&g_Dcf.Mutex) != OK) {
                Dcf_Perror(__FUNCTION__, __LINE__, "cannot give mutex");
                Rval = DCF_ERR_GENERAL_ERROR;
            }
        } else {
            Dcf_Perror(__FUNCTION__, __LINE__, "cannot take mutex");
        }
    }
    return Rval;
}

static UINT32 RefDcf_RemoveRootImpl(const REF_DCF_HDLR_IMPL_s *Hdlr, char *Path)
{
    UINT32 Rval = DCF_OK;
    if (Hdlr->State >= DCF_STATE_ROOT) {
        UINT32 i;
        const UINT32 Len = AmbaUtility_StringLength(Path);
        REF_DCF_ROOT_INFO_s *Root;
        for (i = 0; i < g_Dcf.MaxRootPerHdlr; i++) {
            Root = &Hdlr->RootInfo[i];
            if (Root->Used != 0U) {
                if (AmbaUtility_StringCompare(Path, Root->Path, Len) == 0) {
                    // trick: Dnum set to ID_NOT_EXISTED to tell Core it is from RemoveRoot */
                    Rval = RefDcf_RemoveDirectoryFromTable(Hdlr->Table, ID_NOT_EXISTED, Path);
                    Root->Used = 0U;
                    break;
                }
            }
        }
        if (i == g_Dcf.MaxRootPerHdlr) {
            Dcf_Perror(__FUNCTION__, __LINE__, "No such root!");
            Rval = DCF_ERR_GENERAL_ERROR;
        }
    } else {
        Dcf_Perror(__FUNCTION__, __LINE__, "Incorrect state!");
        Rval = DCF_ERR_GENERAL_ERROR;
    }
    return Rval;
}

/**
 * Remove a DCF root from a DCF handler.
 * @param [in] Hdlr The DCF handler
 * @param [in] Path The path of a DCF root being removed (e.g., c:\DCIM)
 * @return 0 - OK, others - DCF_ERR_XXX
 */
UINT32 RefDcf_RemoveRoot(REF_DCF_HDLR_s *Hdlr, char *Path)
{
    UINT32 Rval;
    if ((IS_DCF_HDLR(Hdlr) == 0U) || (Path == NULL)) {
        Dcf_Perror(__FUNCTION__, __LINE__, "Invalid argument!");
        Rval = DCF_ERR_INVALID_ARG;
    } else {
        AmbaMisra_TouchUnused(&Hdlr->Resv[0]);
        Rval = K2D(AmbaKAL_MutexTake(&g_Dcf.Mutex, DCF_TIMEOUT_MUTEX));
        if (Rval == DCF_OK) {
            const REF_DCF_HDLR_IMPL_s *DcfHdlr;
            AmbaMisra_TypeCast32(&DcfHdlr, &Hdlr);
            Rval = RefDcf_RemoveRootImpl(DcfHdlr, Path);
            if (AmbaKAL_MutexGive(&g_Dcf.Mutex) != OK) {
                Dcf_Perror(__FUNCTION__, __LINE__, "cannot give mutex");
                Rval = DCF_ERR_GENERAL_ERROR;
            }
        } else {
            Dcf_Perror(__FUNCTION__, __LINE__, "cannot take mutex");
        }
    }
    return Rval;
}

static UINT32 RefDcf_ScanImpl(REF_DCF_HDLR_IMPL_s *Hdlr, UINT32 Dnum)
{
    UINT32 Rval = DCF_OK;
    if (Hdlr->State >= DCF_STATE_ROOT) {
        if (Hdlr->State != DCF_STATE_DONE) {
            UINT32 i;
            const REF_DCF_ROOT_INFO_s *Root;
            Hdlr->State = DCF_STATE_SCAN;
            // check all roots
            for (i = 0; i < g_Dcf.MaxRootPerHdlr; i++) {
                Root = &Hdlr->RootInfo[i];
                if (Root->Used != 0U){
                    if (IS_ROOT_DIRTY(Root) != 0U) {
                        // the root is dirty
                        UINT8 IsDirty = 0U;
                        // clear root dirty bit first
                        Rval = CLR_ROOT_DIRTY(Root);
                        if (Rval == DCF_OK) {
                            Rval = RefDcf_CheckRootDnumDirty(Root, Dnum, &IsDirty);
                        }
                        if (Rval == DCF_OK) {
                            if (IsDirty != 0U) {
                                // the Dnum is dirty
                                Rval = RefDcf_ScanFileToTable(Root, Hdlr->Filter, Hdlr->Table, Dnum);
                                if (Rval == DCF_OK) {
                                    Rval = RefDcf_ClearRootDnumDirty(Root, Dnum);
                                }
                            }
                        }
                        if (Rval == DCF_OK) {
                            // set root's dirty bit if any Dnum is dirty
                            const UINT32 Size = ROOT_DIRTY_SIZE(g_Dcf.MaxDnum) / sizeof(UINT64);
                            UINT32 j;
                            for (j = 0; j < Size; j++) {
                                if (Root->DnumDirty[j] != 0ULL) {
                                    Rval = SET_ROOT_DIRTY(Root);   // set root dirty bit
                                    break;
                                }
                            }
                        }
                    }
                }
                if (Rval != DCF_OK) {
                    break;
                }
            }
            if (Rval == DCF_OK) {
                // set hdlr's state to done if all roots are clean
                for (i = 0; i < g_Dcf.MaxRootPerHdlr; i++) {
                    Root = &Hdlr->RootInfo[i];
                    if (Root->Used != 0U) {
                        if (IS_ROOT_DIRTY(Root) != 0U) {
                            break;
                        }
                    }
                }
                if (i == g_Dcf.MaxRootPerHdlr) {
                    Hdlr->State = DCF_STATE_DONE;
                    //AmbaPrint_PrintStr5("%s done", __FUNCTION__, NULL, NULL, NULL, NULL);
                }
            }
        }
    } else {
        Dcf_Perror(__FUNCTION__, __LINE__, "Incorrect state!");
        Rval = DCF_ERR_GENERAL_ERROR;
    }
    return Rval;
}

UINT32 RefDcf_Scan(REF_DCF_HDLR_s *Hdlr, UINT32 Dnum)
{
    UINT32 Rval;
    if ((IS_DCF_HDLR(Hdlr) == 0U) || (RefDcf_IsValidDnum(Dnum) == 0U)) {
        Dcf_Perror(__FUNCTION__, __LINE__, "Invalid argument!");
        Rval = DCF_ERR_INVALID_ARG;
    } else {
        AmbaMisra_TouchUnused(&Hdlr->Resv[0]);
        Rval = K2D(AmbaKAL_MutexTake(&g_Dcf.Mutex, DCF_TIMEOUT_MUTEX));
        if (Rval == DCF_OK) {
            REF_DCF_HDLR_IMPL_s *DcfHdlr;
            AmbaMisra_TypeCast32(&DcfHdlr, &Hdlr);
            Rval = RefDcf_ScanImpl(DcfHdlr, Dnum);
            if (AmbaKAL_MutexGive(&g_Dcf.Mutex) != OK) {
                Dcf_Perror(__FUNCTION__, __LINE__, "cannot give mutex");
                Rval = DCF_ERR_GENERAL_ERROR;
            }
        } else {
            Dcf_Perror(__FUNCTION__, __LINE__, "cannot take mutex");
        }
    }
    return Rval;
}

static UINT32 RefDcf_GetFirstIdImpl(REF_DCF_HDLR_IMPL_s *Hdlr, UINT32 *Id)
{
    UINT32 Rval;
    if (Hdlr->State >= DCF_STATE_SCAN) {
        Rval = RefDcf_GetFirstIdFromTable(Hdlr, Id);
        if (Rval == DCF_OK) {
            Hdlr->CurDcfId = *Id;
        }
    } else {
        Dcf_Perror(__FUNCTION__, __LINE__, "incorrect state");
        Rval = DCF_ERR_GENERAL_ERROR;
    }
    return Rval;
}

/**
 * Get the first ID.
 * @param [in] Hdlr The DCF handler
 * @param [out] IdThe ID of the last DCF object
 * @return 0 - OK, others - DCF_ERR_XXX
 */
UINT32 RefDcf_GetFirstId(REF_DCF_HDLR_s *Hdlr, UINT32 *Id)
{
    UINT32 Rval;
    if ((IS_DCF_HDLR(Hdlr) == 0U) || (Id == NULL)) {
        Dcf_Perror(__FUNCTION__, __LINE__, "Invalid argument!");
        Rval = DCF_ERR_INVALID_ARG;
    } else {
        AmbaMisra_TouchUnused(&Hdlr->Resv[0]);
        Rval = K2D(AmbaKAL_MutexTake(&g_Dcf.Mutex, DCF_TIMEOUT_MUTEX));
        if (Rval == DCF_OK) {
            REF_DCF_HDLR_IMPL_s *DcfHdlr;
            AmbaMisra_TypeCast32(&DcfHdlr, &Hdlr);
            Rval = RefDcf_GetFirstIdImpl(DcfHdlr, Id);
            if (AmbaKAL_MutexGive(&g_Dcf.Mutex) != OK) {
                Dcf_Perror(__FUNCTION__, __LINE__, "cannot give mutex");
                Rval = DCF_ERR_GENERAL_ERROR;
            }
        } else {
            Dcf_Perror(__FUNCTION__, __LINE__, "cannot take mutex");
        }
    }
    return Rval;
}

static UINT32 RefDcf_GetLastIdImpl(REF_DCF_HDLR_IMPL_s *Hdlr, UINT32 *Id)
{
    UINT32 Rval;
    if (Hdlr->State >= DCF_STATE_SCAN) {
        Rval = RefDcf_GetLastIdFromTable(Hdlr, Id);
        if (Rval == DCF_OK) {
            Hdlr->CurDcfId = *Id;
        }
    } else {
        Dcf_Perror(__FUNCTION__, __LINE__, "incorrect state");
        Rval = DCF_ERR_GENERAL_ERROR;
    }
    return Rval;
}

/**
 * Get the last ID.
 * @param [in] Hdlr The DCF handler
 * @param [out] Id The ID of the last DCF object
 * @return 0 - OK, others - DCF_ERR_XXX
 */
UINT32 RefDcf_GetLastId(REF_DCF_HDLR_s *Hdlr, UINT32 *Id)
{
    UINT32 Rval;
    if ((IS_DCF_HDLR(Hdlr) == 0U) || (Id == NULL)) {
        Dcf_Perror(__FUNCTION__, __LINE__, "Invalid argument!");
        Rval = DCF_ERR_INVALID_ARG;
    } else {
        AmbaMisra_TouchUnused(&Hdlr->Resv[0]);
        Rval = K2D(AmbaKAL_MutexTake(&g_Dcf.Mutex, DCF_TIMEOUT_MUTEX));
        if (Rval == DCF_OK) {
            REF_DCF_HDLR_IMPL_s *DcfHdlr;
            AmbaMisra_TypeCast32(&DcfHdlr, &Hdlr);
            Rval = RefDcf_GetLastIdImpl(DcfHdlr, Id);
            if (AmbaKAL_MutexGive(&g_Dcf.Mutex) != OK) {
                Dcf_Perror(__FUNCTION__, __LINE__, "cannot give mutex");
                Rval = DCF_ERR_GENERAL_ERROR;
            }
        } else {
            Dcf_Perror(__FUNCTION__, __LINE__, "cannot take mutex");
        }
    }
    return Rval;
}

static UINT32 RefDcf_GetNextIdImpl(REF_DCF_HDLR_IMPL_s *Hdlr, UINT32 *Id)
{
    UINT32 Rval;
    if (Hdlr->State >= DCF_STATE_SCAN) {
        if (Hdlr->CurDcfId == ID_NOT_EXISTED) {
            Dcf_Perror(__FUNCTION__, __LINE__, "no reference ID, please call RefDcf_GetFirstId() or RefDcf_GetLastId() first");
            Rval = DCF_ERR_GENERAL_ERROR;
        } else {
            UINT32 Dnum;
            const REF_DCF_FILTER_s * const Filter = Hdlr->Filter;
            Rval = Filter->IdToDnum(Hdlr->CurDcfId, &Dnum);
            if (Rval == DCF_OK) {
                UINT32 Fnum;
                Rval = Filter->IdToFnum(Hdlr->CurDcfId, &Fnum);
                if (Rval == DCF_OK) {
                    Rval = RefDcf_GetNextIdFromTable(Hdlr, Dnum, Fnum, Id);
                    if (Rval == DCF_OK) {
                        Hdlr->CurDcfId = *Id;
                    }
                } else {
                    Dcf_Perror(__FUNCTION__, __LINE__, "IdToFnum failed");
                }
            } else {
                Dcf_Perror(__FUNCTION__, __LINE__, "IdToDnum failed");
            }
        }
    } else {
        Dcf_Perror(__FUNCTION__, __LINE__, "incorrect state");
        Rval = DCF_ERR_GENERAL_ERROR;
    }
    return Rval;
}

/**
 * Get the next nth object ID from the current ID (No Cycle).
 * @param [in] Hdlr The DCF handler
 * @param [out] Id The ID of the next nth DCF object
 * @return 0 - OK, others - DCF_ERR_XXX
 */
UINT32 RefDcf_GetNextId(REF_DCF_HDLR_s *Hdlr, UINT32 *Id)
{
    UINT32 Rval;
    if ((IS_DCF_HDLR(Hdlr) == 0U) || (Id == NULL)) {
        Dcf_Perror(__FUNCTION__, __LINE__, "Invalid argument!");
        Rval = DCF_ERR_INVALID_ARG;
    } else {
        AmbaMisra_TouchUnused(&Hdlr->Resv[0]);
        Rval = K2D(AmbaKAL_MutexTake(&g_Dcf.Mutex, DCF_TIMEOUT_MUTEX));
        if (Rval == DCF_OK) {
            REF_DCF_HDLR_IMPL_s *DcfHdlr;
            AmbaMisra_TypeCast32(&DcfHdlr, &Hdlr);
            Rval = RefDcf_GetNextIdImpl(DcfHdlr, Id);
            if (AmbaKAL_MutexGive(&g_Dcf.Mutex) != OK) {
                Dcf_Perror(__FUNCTION__, __LINE__, "cannot give mutex");
                Rval = DCF_ERR_GENERAL_ERROR;
            }
        } else {
            Dcf_Perror(__FUNCTION__, __LINE__, "cannot take mutex");
        }
    }
    return Rval;
}

static UINT32 RefDcf_GetPrevIdImpl(REF_DCF_HDLR_IMPL_s *Hdlr, UINT32 *Id)
{
    UINT32 Rval;
    if (Hdlr->State >= DCF_STATE_SCAN) {
        if (Hdlr->CurDcfId == ID_NOT_EXISTED) {
            Dcf_Perror(__FUNCTION__, __LINE__, "no reference ID, please call RefDcf_GetFirstId() or RefDcf_GetLastId() first");
            Rval = DCF_ERR_GENERAL_ERROR;
        } else {
            UINT32 Dnum;
            const REF_DCF_FILTER_s * const Filter = Hdlr->Filter;
            Rval = Filter->IdToDnum(Hdlr->CurDcfId, &Dnum);
            if (Rval == DCF_OK) {
                UINT32 Fnum;
                Rval = Filter->IdToFnum(Hdlr->CurDcfId, &Fnum);
                if (Rval == DCF_OK) {
                    Rval = RefDcf_GetPrevIdFromTable(Hdlr, Dnum, Fnum, Id);
                    if (Rval == DCF_OK) {
                        Hdlr->CurDcfId = *Id;
                    }
                } else {
                    Dcf_Perror(__FUNCTION__, __LINE__, "IdToFnum failed");
                }
            } else {
                Dcf_Perror(__FUNCTION__, __LINE__, "IdToDnum failed");
            }
        }
    } else {
        Dcf_Perror(__FUNCTION__, __LINE__, "incorrect state");
        Rval = DCF_ERR_GENERAL_ERROR;
    }
    return Rval;
}

/**
 * Get the previous nth object ID from the current ID (No Cycle).
 * @param [in] Hdlr The DCF handler
 * @param [out] Id The ID of the previous nth DCF object
 * @return 0 - OK, others - DCF_ERR_XXX
 */
UINT32 RefDcf_GetPrevId(REF_DCF_HDLR_s *Hdlr, UINT32 *Id)
{
    UINT32 Rval;
    if ((IS_DCF_HDLR(Hdlr) == 0U) || (Id == NULL)) {
        Dcf_Perror(__FUNCTION__, __LINE__, "Invalid argument!");
        Rval = DCF_ERR_INVALID_ARG;
    } else {
        AmbaMisra_TouchUnused(&Hdlr->Resv[0]);
        Rval = K2D(AmbaKAL_MutexTake(&g_Dcf.Mutex, DCF_TIMEOUT_MUTEX));
        if (Rval == DCF_OK) {
            REF_DCF_HDLR_IMPL_s *DcfHdlr;
            AmbaMisra_TypeCast32(&DcfHdlr, &Hdlr);
            Rval = RefDcf_GetPrevIdImpl(DcfHdlr, Id);
            if (AmbaKAL_MutexGive(&g_Dcf.Mutex) != OK) {
                Dcf_Perror(__FUNCTION__, __LINE__, "cannot give mutex");
                Rval = DCF_ERR_GENERAL_ERROR;
            }
        } else {
            Dcf_Perror(__FUNCTION__, __LINE__, "cannot take mutex");
        }
    }
    return Rval;
}

static UINT32 RefDcf_GetFileListImpl(const REF_DCF_HDLR_IMPL_s *Hdlr, UINT32 Id, REF_DCF_FILE_s *FileList, UINT32 MaxFile, UINT32 *Count)
{
    UINT32 Rval;
    if (Hdlr->State >= DCF_STATE_SCAN) {
        UINT32 Dnum;
        const REF_DCF_FILTER_s * const Filter = Hdlr->Filter;
        Rval = Filter->IdToDnum(Id, &Dnum);
        if (Rval == DCF_OK) {
            UINT8 IsDirty = 0U;
            Rval = RefDcf_CheckDnumDirty(Hdlr, Dnum, &IsDirty);
            if (Rval == DCF_OK) {
                if (IsDirty == 0U) {
                    UINT32 Fnum;
                    Rval = Filter->IdToFnum(Id, &Fnum);
                    if (Rval == DCF_OK) {
                        Rval = RefDcf_GetFileListFromTable(Hdlr->Table, Dnum, Fnum, FileList, MaxFile, Count);
                    } else {
                        Dcf_Perror(__FUNCTION__, __LINE__, "IdToFnum failed");
                    }
                } else {
                    Dcf_Perror(__FUNCTION__, __LINE__, "Not ready");
                    Rval = DCF_ERR_NOT_READY;
                }
            }
        } else {
            Dcf_Perror(__FUNCTION__, __LINE__, "IdToDnum failed");
        }
    } else {
        Dcf_Perror(__FUNCTION__, __LINE__, "incorrect state");
        Rval = DCF_ERR_GENERAL_ERROR;
    }
    return Rval;
}

/**
 * Get the list of files with a specified ID.
 * @param [in] Hdlr The DCF handler
 * @param [in] Id The object ID
 * @param [out] FileList The file list
 * @param [in] MaxFile Max file count
 * @param [out] Count The file count
 * @return 0 - OK, others - DCF_ERR_XXX
 */
UINT32 RefDcf_GetFileList(REF_DCF_HDLR_s *Hdlr, UINT32 Id, REF_DCF_FILE_s *FileList, UINT32 MaxFile, UINT32 *Count)
{
    UINT32 Rval;
    if ((IS_DCF_HDLR(Hdlr) == 0U) || (Id == ID_NOT_EXISTED) || (FileList == NULL) || (MaxFile == 0U) || (Count == NULL)) {
        Dcf_Perror(__FUNCTION__, __LINE__, "Invalid argument!");
        Rval = DCF_ERR_INVALID_ARG;
    } else {
        AmbaMisra_TouchUnused(&Hdlr->Resv[0]);
        Rval = K2D(AmbaKAL_MutexTake(&g_Dcf.Mutex, DCF_TIMEOUT_MUTEX));
        if (Rval == DCF_OK) {
            const REF_DCF_HDLR_IMPL_s *DcfHdlr;
            AmbaMisra_TypeCast32(&DcfHdlr, &Hdlr);
            Rval = RefDcf_GetFileListImpl(DcfHdlr, Id, FileList, MaxFile, Count);
            if (AmbaKAL_MutexGive(&g_Dcf.Mutex) != OK) {
                Dcf_Perror(__FUNCTION__, __LINE__, "cannot give mutex");
                Rval = DCF_ERR_GENERAL_ERROR;
            }
        } else {
            Dcf_Perror(__FUNCTION__, __LINE__, "cannot take mutex");
        }
    }
    return Rval;
}

static UINT32 RefDcf_GetFirstDnumImpl(REF_DCF_HDLR_IMPL_s *Hdlr, UINT32 *Dnum)
{
    UINT32 Rval;
    if (Hdlr->State >= DCF_STATE_ROOT) {
        if (Hdlr->State == DCF_STATE_ROOT) {
            Hdlr->State = DCF_STATE_SCAN;
        }
        Rval = RefDcf_GetFirstDnumFromTable(Hdlr->Table, Dnum);
        if ((Rval == DCF_OK) || (Rval == DCF_ERR_OBJECT_NOT_EXISTED)) {
            Hdlr->CurDcfDnum = *Dnum;
        }
    } else {
        Dcf_Perror(__FUNCTION__, __LINE__, "incorrect state");
        Rval = DCF_ERR_GENERAL_ERROR;
    }
    return Rval;
}

/**
 * Get the first directory number.
 * @param [in] Hdlr The DCF handler
 * @param [out] Dnum The first directory number
 * @return 0 - OK, others - DCF_ERR_XXX
 */
UINT32 RefDcf_GetFirstDnum(REF_DCF_HDLR_s *Hdlr, UINT32 *Dnum)
{
    UINT32 Rval;
    if ((IS_DCF_HDLR(Hdlr) == 0U) || (Dnum == NULL)) {
        Dcf_Perror(__FUNCTION__, __LINE__, "Invalid argument!");
        Rval = DCF_ERR_INVALID_ARG;
    } else {
        AmbaMisra_TouchUnused(&Hdlr->Resv[0]);
        Rval = K2D(AmbaKAL_MutexTake(&g_Dcf.Mutex, DCF_TIMEOUT_MUTEX));
        if (Rval == DCF_OK) {
            REF_DCF_HDLR_IMPL_s *DcfHdlr;
            AmbaMisra_TypeCast32(&DcfHdlr, &Hdlr);
            Rval = RefDcf_GetFirstDnumImpl(DcfHdlr, Dnum);
            if (AmbaKAL_MutexGive(&g_Dcf.Mutex) != OK) {
                Dcf_Perror(__FUNCTION__, __LINE__, "cannot give mutex");
                Rval = DCF_ERR_GENERAL_ERROR;
            }
        } else {
            Dcf_Perror(__FUNCTION__, __LINE__, "cannot take mutex");
        }
    }
    return Rval;
}

static UINT32 RefDcf_GetLastDnumImpl(REF_DCF_HDLR_IMPL_s *Hdlr, UINT32 *Dnum)
{
    UINT32 Rval;
    if (Hdlr->State >= DCF_STATE_ROOT) {
        if (Hdlr->State == DCF_STATE_ROOT) {
            Hdlr->State = DCF_STATE_SCAN;
        }
        Rval = RefDcf_GetLastDnumFromTable(Hdlr->Table, Dnum);
        if ((Rval == DCF_OK) || (Rval == DCF_ERR_OBJECT_NOT_EXISTED)) {
            Hdlr->CurDcfDnum = *Dnum;
        }
    } else {
        Dcf_Perror(__FUNCTION__, __LINE__, "incorrect state");
        Rval = DCF_ERR_GENERAL_ERROR;
    }
    return Rval;
}

/**
 * Get the last directory number.
 * @param [in] Hdlr The DCF handler
 * @param [out] Dnum The last directory number
 * @return 0 - OK, others - DCF_ERR_XXX
 */
UINT32 RefDcf_GetLastDnum(REF_DCF_HDLR_s *Hdlr, UINT32 *Dnum)
{
    UINT32 Rval;
    if ((IS_DCF_HDLR(Hdlr) == 0U) || (Dnum == NULL)) {
        Dcf_Perror(__FUNCTION__, __LINE__, "Invalid argument!");
        Rval = DCF_ERR_INVALID_ARG;
    } else {
        AmbaMisra_TouchUnused(&Hdlr->Resv[0]);
        Rval = K2D(AmbaKAL_MutexTake(&g_Dcf.Mutex, DCF_TIMEOUT_MUTEX));
        if (Rval == DCF_OK) {
            REF_DCF_HDLR_IMPL_s *DcfHdlr;
            AmbaMisra_TypeCast32(&DcfHdlr, &Hdlr);
            Rval = RefDcf_GetLastDnumImpl(DcfHdlr, Dnum);
            if (AmbaKAL_MutexGive(&g_Dcf.Mutex) != OK) {
                Dcf_Perror(__FUNCTION__, __LINE__, "cannot give mutex");
                Rval = DCF_ERR_GENERAL_ERROR;
            }
        } else {
            Dcf_Perror(__FUNCTION__, __LINE__, "cannot take mutex");
        }
    }
    return Rval;
}

static UINT32 RefDcf_GetNextDnumImpl(REF_DCF_HDLR_IMPL_s *Hdlr, UINT32 *Dnum)
{
    UINT32 Rval;
    if (Hdlr->State >= DCF_STATE_SCAN) {
        if (Hdlr->CurDcfDnum == ID_NOT_EXISTED) {
            Dcf_Perror(__FUNCTION__, __LINE__, "no reference Dnum, please call RefDcf_GetFirstDnum() or RefDcf_GetLastDnum() first");
            Rval = DCF_ERR_GENERAL_ERROR;
        } else {
            Rval = RefDcf_GetNextDnumFromTable(Hdlr->Table, Hdlr->CurDcfDnum, Dnum);
            if (Rval == DCF_OK) {
                Hdlr->CurDcfDnum = *Dnum;
            }
        }
    } else {
        Dcf_Perror(__FUNCTION__, __LINE__, "incorrect state");
        Rval = DCF_ERR_GENERAL_ERROR;
    }
    return Rval;
}

/**
 * Get the next nth directory number from the current directory (No Cycle).
 * @param [in] Hdlr The DCF handler
 * @param [out] Dnum The number of the next nth directory
 * @return 0 - OK, others - DCF_ERR_XXX
 */
UINT32 RefDcf_GetNextDnum(REF_DCF_HDLR_s *Hdlr, UINT32 *Dnum)
{
    UINT32 Rval;
    if ((IS_DCF_HDLR(Hdlr) == 0U) || (Dnum == NULL)) {
        Dcf_Perror(__FUNCTION__, __LINE__, "Invalid argument!");
        Rval = DCF_ERR_INVALID_ARG;
    } else {
        AmbaMisra_TouchUnused(&Hdlr->Resv[0]);
        Rval = K2D(AmbaKAL_MutexTake(&g_Dcf.Mutex, DCF_TIMEOUT_MUTEX));
        if (Rval == DCF_OK) {
            REF_DCF_HDLR_IMPL_s *DcfHdlr;
            AmbaMisra_TypeCast32(&DcfHdlr, &Hdlr);
            Rval = RefDcf_GetNextDnumImpl(DcfHdlr, Dnum);
            if (AmbaKAL_MutexGive(&g_Dcf.Mutex) != OK) {
                Dcf_Perror(__FUNCTION__, __LINE__, "cannot give mutex");
                Rval = DCF_ERR_GENERAL_ERROR;
            }
        } else {
            Dcf_Perror(__FUNCTION__, __LINE__, "cannot take mutex");
        }
    }
    return Rval;
}

static UINT32 RefDcf_GetPrevDnumImpl(REF_DCF_HDLR_IMPL_s *Hdlr, UINT32 *Dnum)
{
    UINT32 Rval;
    if (Hdlr->State >= DCF_STATE_SCAN) {
        if (Hdlr->CurDcfDnum == ID_NOT_EXISTED) {
            Dcf_Perror(__FUNCTION__, __LINE__, "no reference Dnum, please call RefDcf_GetFirstDnum() or RefDcf_GetLastDnum() first");
            Rval = DCF_ERR_GENERAL_ERROR;
        } else {
            Rval = RefDcf_GetPrevDnumFromTable(Hdlr->Table, Hdlr->CurDcfDnum, Dnum);
            if (Rval == DCF_OK) {
                Hdlr->CurDcfDnum = *Dnum;
            }
        }
    } else {
        Dcf_Perror(__FUNCTION__, __LINE__, "incorrect state");
        Rval = DCF_ERR_GENERAL_ERROR;
    }
    return Rval;
}

/**
 * Get the previous nth directory number from the current directory (No Cycle).
 * @param [in] Hdlr The DCF handler
 * @param [out] Dnum The number of the previous nth directory
 * @return 0 - OK, others - DCF_ERR_XXX
 */
UINT32 RefDcf_GetPrevDnum(REF_DCF_HDLR_s *Hdlr, UINT32 *Dnum)
{
    UINT32 Rval;
    if ((IS_DCF_HDLR(Hdlr) == 0U) || (Dnum == NULL)) {
        Dcf_Perror(__FUNCTION__, __LINE__, "Invalid argument!");
        Rval = DCF_ERR_INVALID_ARG;
    } else {
        AmbaMisra_TouchUnused(&Hdlr->Resv[0]);
        Rval = K2D(AmbaKAL_MutexTake(&g_Dcf.Mutex, DCF_TIMEOUT_MUTEX));
        if (Rval == DCF_OK) {
            REF_DCF_HDLR_IMPL_s *DcfHdlr;
            AmbaMisra_TypeCast32(&DcfHdlr, &Hdlr);
            Rval = RefDcf_GetPrevDnumImpl(DcfHdlr, Dnum);
            if (AmbaKAL_MutexGive(&g_Dcf.Mutex) != OK) {
                Dcf_Perror(__FUNCTION__, __LINE__, "cannot give mutex");
                Rval = DCF_ERR_GENERAL_ERROR;
            }
        } else {
            Dcf_Perror(__FUNCTION__, __LINE__, "cannot take mutex");
        }
    }
    return Rval;
}

static UINT32 RefDcf_GetDirectoryListImpl(const REF_DCF_HDLR_IMPL_s *Hdlr, UINT32 Dnum, REF_DCF_DIR_s *DirList, UINT32 MaxDir, UINT32 *Count)
{
    UINT32 Rval;
    if (Hdlr->State >= DCF_STATE_SCAN) {
        Rval = RefDcf_GetDirectoryListFromTable(Hdlr->Table, Dnum, DirList, MaxDir, Count);
    } else {
        Dcf_Perror(__FUNCTION__, __LINE__, "incorrect state");
        Rval = DCF_ERR_GENERAL_ERROR;
    }
    return Rval;
}

/**
 * Get the list of directories with a specified directory number.
 * @param [in] Hdlr The DCF handler
 * @param [in] Dnum The directory number
 * @param [out] DirList The directory list
 * @param [in] MaxDir Max directory count
 * @param [out] Count The directory count
 * @return 0 - OK, others - DCF_ERR_XXX
 */
UINT32 RefDcf_GetDirectoryList(REF_DCF_HDLR_s *Hdlr, UINT32 Dnum, REF_DCF_DIR_s *DirList, UINT32 MaxDir, UINT32 *Count)
{
    UINT32 Rval;
    if ((IS_DCF_HDLR(Hdlr) == 0U) || (Dnum == ID_NOT_EXISTED) || (DirList == NULL) || (MaxDir == 0U) || (Count == NULL)) {
        Dcf_Perror(__FUNCTION__, __LINE__, "Invalid argument!");
        Rval = DCF_ERR_INVALID_ARG;
    } else {
        AmbaMisra_TouchUnused(&Hdlr->Resv[0]);
        Rval = K2D(AmbaKAL_MutexTake(&g_Dcf.Mutex, DCF_TIMEOUT_MUTEX));
        if (Rval == DCF_OK) {
            const REF_DCF_HDLR_IMPL_s *DcfHdlr;
            AmbaMisra_TypeCast32(&DcfHdlr, &Hdlr);
            Rval = RefDcf_GetDirectoryListImpl(DcfHdlr, Dnum, DirList, MaxDir, Count);
            if (AmbaKAL_MutexGive(&g_Dcf.Mutex) != OK) {
                Dcf_Perror(__FUNCTION__, __LINE__, "cannot give mutex");
                Rval = DCF_ERR_GENERAL_ERROR;
            }
        } else {
            Dcf_Perror(__FUNCTION__, __LINE__, "cannot take mutex");
        }
    }
    return Rval;
}

static UINT8 IsValidPath(const REF_DCF_HDLR_IMPL_s *Hdlr, const char *Name)
{
    UINT8 Rval = 0U;
    const REF_DCF_ROOT_INFO_s *Root;
    UINT32 Len;
    UINT32 i;
    for (i = 0; i < g_Dcf.MaxRootPerHdlr; i++) {
        Root = &Hdlr->RootInfo[i];
        if (Root->Used != 0U) {
            Len = AmbaUtility_StringLength(Root->Path);
            if (AmbaUtility_StringCompare(Root->Path, Name, Len) == 0) {
                if ((Name[Len] == '\\') || (Name[Len] == '/')) {
                    Rval = 1U;
                    break;
                }
            }
        }
    }
    return Rval;
}

static UINT32 RefDcf_AddDirectoryImpl(const REF_DCF_HDLR_IMPL_s *Hdlr, char *Name)
{
    UINT32 Rval;
    if (Hdlr->State >= DCF_STATE_SCAN) {
        if (IsValidPath(Hdlr, Name) != 0U) {
            UINT32 Dnum;
            Rval = Hdlr->Filter->NameToDnum(Name, &Dnum);
            if (Rval == DCF_OK) {
                Rval = RefDcf_AddDirectoryToTable(Hdlr->Table, Dnum, Name);
            } else {
                Dcf_Perror(__FUNCTION__, __LINE__, "NameToDnum failed");
            }
        } else {
            Dcf_Perror(__FUNCTION__, __LINE__, "invalid name");
            Rval = DCF_ERR_INVALID_ARG;
        }
    } else {
        Dcf_Perror(__FUNCTION__, __LINE__, "incorrect state");
        Rval = DCF_ERR_GENERAL_ERROR;
    }
    return Rval;
}

/**
 * Remove a directory from a DCF handler.
 * @param [in] Hdlr The DCF handler
 * @param [in] Name The directory name
 * @return 0 - OK, others - DCF_ERR_XXX
 */
UINT32 RefDcf_AddDirectory(REF_DCF_HDLR_s *Hdlr, char *Name)
{
    UINT32 Rval;
    if ((IS_DCF_HDLR(Hdlr) == 0U) || (Name == NULL)) {
        Dcf_Perror(__FUNCTION__, __LINE__, "Invalid argument!");
        Rval = DCF_ERR_INVALID_ARG;
    } else {
        AmbaMisra_TouchUnused(&Hdlr->Resv[0]);
        Rval = K2D(AmbaKAL_MutexTake(&g_Dcf.Mutex, DCF_TIMEOUT_MUTEX));
        if (Rval == DCF_OK) {
            const REF_DCF_HDLR_IMPL_s *DcfHdlr;
            AmbaMisra_TypeCast32(&DcfHdlr, &Hdlr);
            Rval = RefDcf_AddDirectoryImpl(DcfHdlr, Name);
            if (AmbaKAL_MutexGive(&g_Dcf.Mutex) != OK) {
                Dcf_Perror(__FUNCTION__, __LINE__, "cannot give mutex");
                Rval = DCF_ERR_GENERAL_ERROR;
            }
        } else {
            Dcf_Perror(__FUNCTION__, __LINE__, "cannot take mutex");
        }
    }
    return Rval;
}

static UINT32 RefDcf_RemoveDirectoryImpl(const REF_DCF_HDLR_IMPL_s *Hdlr, char *Name)
{
    UINT32 Rval;
    if (Hdlr->State >= DCF_STATE_SCAN) {
        if (IsValidPath(Hdlr, Name) != 0U) {
            UINT32 Dnum;
            Rval = Hdlr->Filter->NameToDnum(Name, &Dnum);
            if (Rval == DCF_OK) {
                Rval = RefDcf_RemoveDirectoryFromTable(Hdlr->Table, Dnum, Name);
            } else {
                Dcf_Perror(__FUNCTION__, __LINE__, "NameToDnum failed");
            }
        } else {
            Dcf_Perror(__FUNCTION__, __LINE__, "invalid name");
            Rval = DCF_ERR_INVALID_ARG;
        }
    } else {
        Dcf_Perror(__FUNCTION__, __LINE__, "incorrect state");
        Rval = DCF_ERR_GENERAL_ERROR;
    }
    return Rval;
}

/**
 * Remove a directory from a DCF handler.
 * @param [in] Hdlr The DCF handler
 * @param [in] Name The directory name
 * @return 0 - OK, others - DCF_ERR_XXX
 */
UINT32 RefDcf_RemoveDirectory(REF_DCF_HDLR_s *Hdlr, char *Name)
{
    UINT32 Rval;
    if ((IS_DCF_HDLR(Hdlr) == 0U) || (Name == NULL)) {
        Dcf_Perror(__FUNCTION__, __LINE__, "Invalid argument!");
        Rval = DCF_ERR_INVALID_ARG;
    } else {
        AmbaMisra_TouchUnused(&Hdlr->Resv[0]);
        Rval = K2D(AmbaKAL_MutexTake(&g_Dcf.Mutex, DCF_TIMEOUT_MUTEX));
        if (Rval == DCF_OK) {
            const REF_DCF_HDLR_IMPL_s *DcfHdlr;
            AmbaMisra_TypeCast32(&DcfHdlr, &Hdlr);
            Rval = RefDcf_RemoveDirectoryImpl(DcfHdlr, Name);
            if (AmbaKAL_MutexGive(&g_Dcf.Mutex) != OK) {
                Dcf_Perror(__FUNCTION__, __LINE__, "cannot give mutex");
                Rval = DCF_ERR_GENERAL_ERROR;
            }
        } else {
            Dcf_Perror(__FUNCTION__, __LINE__, "cannot take mutex");
        }
    }
    return Rval;
}

static UINT32 RefDcf_AddFileImpl(const REF_DCF_HDLR_IMPL_s *Hdlr, char *Name)
{
    UINT32 Rval;
    if (Hdlr->State >= DCF_STATE_SCAN) {
        const REF_DCF_FILTER_s * const Filter = Hdlr->Filter;
        if (IsValidPath(Hdlr, Name) != 0U) {
            UINT32 Id;
            Rval = Filter->NameToId(Name, &Id);
            if (Rval == DCF_OK) {
                UINT32 Dnum;
                Rval = Filter->IdToDnum(Id, &Dnum);
                if (Rval == DCF_OK) {
                    UINT8 IsDirty = 0U;
                    Rval = RefDcf_CheckDnumDirty(Hdlr, Dnum, &IsDirty);
                    if (Rval == DCF_OK) {
                        if (IsDirty == 0U) {
                            UINT32 Fnum;
                            Rval = Filter->IdToFnum(Id, &Fnum);
                            if (Rval == DCF_OK) {
                                Rval = RefDcf_AddFileToTable(Hdlr->Table, Dnum, Fnum, Name);
                            } else {
                                Dcf_Perror(__FUNCTION__, __LINE__, "IdToFnum failed");
                            }
                        } else {
                            Dcf_Perror(__FUNCTION__, __LINE__, "Not ready");
                            Rval = DCF_ERR_NOT_READY;
                        }
                    }
                } else {
                    Dcf_Perror(__FUNCTION__, __LINE__, "IdToDnum failed");
                }
            } else {
                Dcf_Perror(__FUNCTION__, __LINE__, "NameToId failed");
            }
        } else {
            Dcf_Perror(__FUNCTION__, __LINE__, "invalid name");
            Rval = DCF_ERR_INVALID_ARG;
        }
    } else {
        Dcf_Perror(__FUNCTION__, __LINE__, "incorrect state");
        Rval = DCF_ERR_GENERAL_ERROR;
    }
    return Rval;
}

/**
 * Add a file to a DCF handler.
 * @param [in] Hdlr The DCF handler
 * @param [in] Name The file name
 * @return 0 - OK, others - DCF_ERR_XXX
 */
UINT32 RefDcf_AddFile(REF_DCF_HDLR_s *Hdlr, char *Name)
{
    UINT32 Rval;
    if ((IS_DCF_HDLR(Hdlr) == 0U) || (Name == NULL)) {
        Dcf_Perror(__FUNCTION__, __LINE__, "Invalid argument!");
        Rval = DCF_ERR_INVALID_ARG;
    } else {
        AmbaMisra_TouchUnused(&Hdlr->Resv[0]);
        Rval = K2D(AmbaKAL_MutexTake(&g_Dcf.Mutex, DCF_TIMEOUT_MUTEX));
        if (Rval == DCF_OK) {
            const REF_DCF_HDLR_IMPL_s *DcfHdlr;
            AmbaMisra_TypeCast32(&DcfHdlr, &Hdlr);
            Rval = RefDcf_AddFileImpl(DcfHdlr, Name);
            if (AmbaKAL_MutexGive(&g_Dcf.Mutex) != OK) {
                Dcf_Perror(__FUNCTION__, __LINE__, "cannot give mutex");
                Rval = DCF_ERR_GENERAL_ERROR;
            }
        } else {
            Dcf_Perror(__FUNCTION__, __LINE__, "cannot take mutex");
        }
    }
    return Rval;
}

static UINT32 RefDcf_RemoveFileImpl(const REF_DCF_HDLR_IMPL_s *Hdlr, char *Name)
{
    UINT32 Rval;
    if (Hdlr->State >= DCF_STATE_SCAN) {
        const REF_DCF_FILTER_s * const Filter = Hdlr->Filter;
        if (IsValidPath(Hdlr, Name) != 0U) {
            UINT32 Id;
            Rval = Filter->NameToId(Name, &Id);
            if (Rval == DCF_OK) {
                UINT32 Dnum;
                Rval = Filter->IdToDnum(Id, &Dnum);
                if (Rval == DCF_OK) {
                    UINT8 IsDirty = 0U;
                    Rval = RefDcf_CheckDnumDirty(Hdlr, Dnum, &IsDirty);
                    if (Rval == DCF_OK) {
                        if (IsDirty == 0U) {
                            UINT32 Fnum;
                            Rval = Filter->IdToFnum(Id, &Fnum);
                            if (Rval == DCF_OK) {
                                Rval = RefDcf_RemoveFileFromTable(Hdlr->Table, Dnum, Fnum, Name);
                            } else {
                                Dcf_Perror(__FUNCTION__, __LINE__, "IdToFnum failed");
                            }
                        } else {
                            Dcf_Perror(__FUNCTION__, __LINE__, "Not ready");
                            Rval = DCF_ERR_NOT_READY;
                        }
                    }
                } else {
                    Dcf_Perror(__FUNCTION__, __LINE__, "IdToDnum failed");
                }
            } else {
                Dcf_Perror(__FUNCTION__, __LINE__, "NameToId failed");
            }
        } else {
            Dcf_Perror(__FUNCTION__, __LINE__, "invalid name");
            Rval = DCF_ERR_INVALID_ARG;
        }
    } else {
        Dcf_Perror(__FUNCTION__, __LINE__, "incorrect state");
        Rval = DCF_ERR_GENERAL_ERROR;
    }
    return Rval;
}

/**
 * Remove a file from a DCF handler.
 * @param [in] Hdlr The DCF handler
 * @param [in] Name The file name
 * @return 0 - OK, others - DCF_ERR_XXX
 */
UINT32 RefDcf_RemoveFile(REF_DCF_HDLR_s *Hdlr, char *Name)
{
    UINT32 Rval;
    if ((IS_DCF_HDLR(Hdlr) == 0U) || (Name == NULL)) {
        Dcf_Perror(__FUNCTION__, __LINE__, "Invalid argument!");
        Rval = DCF_ERR_INVALID_ARG;
    } else {
        AmbaMisra_TouchUnused(&Hdlr->Resv[0]);
        Rval = K2D(AmbaKAL_MutexTake(&g_Dcf.Mutex, DCF_TIMEOUT_MUTEX));
        if (Rval == DCF_OK) {
            const REF_DCF_HDLR_IMPL_s *DcfHdlr;
            AmbaMisra_TypeCast32(&DcfHdlr, &Hdlr);
            Rval = RefDcf_RemoveFileImpl(DcfHdlr, Name);
            if (AmbaKAL_MutexGive(&g_Dcf.Mutex) != OK) {
                Dcf_Perror(__FUNCTION__, __LINE__, "cannot give mutex");
                Rval = DCF_ERR_GENERAL_ERROR;
            }
        } else {
            Dcf_Perror(__FUNCTION__, __LINE__, "cannot take mutex");
        }
    }
    return Rval;
}

static UINT32 RefDcf_CheckIdValidImpl(const REF_DCF_HDLR_IMPL_s *Hdlr, UINT32 Id, UINT8 *Valid)
{
    UINT32 Dnum;
    const REF_DCF_FILTER_s * const Filter = Hdlr->Filter;
    UINT32 Rval = Filter->IdToDnum(Id, &Dnum);
    if (Rval == DCF_OK) {
        UINT32 Fnum;
        Rval = Filter->IdToFnum(Id, &Fnum);
        if (Rval == DCF_OK) {
            Rval = RefDcf_TableCheckIdValid(Hdlr->Table, Dnum, Fnum, Valid);;
        } else {
            Dcf_Perror(__FUNCTION__, __LINE__, "IdToFnum failed");
        }
    } else {
        Dcf_Perror(__FUNCTION__, __LINE__, "IdToDnum failed");
    }
    return Rval;
}

/**
 * Check if an object is in a DCF handler.
 * @param [in] Hdlr The DCF handler
 * @param [in] Id The object ID
 * @param [out] Valid The ID is valid or not
 * @return 0 - OK, others - DCF_ERR_XXX
 */
UINT32 RefDcf_CheckIdValid(REF_DCF_HDLR_s *Hdlr, UINT32 Id, UINT8 *Valid)
{
    UINT32 Rval;
    if ((IS_DCF_HDLR(Hdlr) == 0U) || (Id == ID_NOT_EXISTED) || (Valid == NULL)) {
        Dcf_Perror(__FUNCTION__, __LINE__, "Invalid argument!");
        Rval = DCF_ERR_INVALID_ARG;
    } else {
        AmbaMisra_TouchUnused(&Hdlr->Resv[0]);
        Rval = K2D(AmbaKAL_MutexTake(&g_Dcf.Mutex, DCF_TIMEOUT_MUTEX));
        if (Rval == DCF_OK) {
            const REF_DCF_HDLR_IMPL_s *DcfHdlr;
            AmbaMisra_TypeCast32(&DcfHdlr, &Hdlr);
            Rval = RefDcf_CheckIdValidImpl(DcfHdlr, Id, Valid);
            if (AmbaKAL_MutexGive(&g_Dcf.Mutex) != OK) {
                Dcf_Perror(__FUNCTION__, __LINE__, "cannot give mutex");
                Rval = DCF_ERR_GENERAL_ERROR;
            }
        } else {
            Dcf_Perror(__FUNCTION__, __LINE__, "cannot take mutex");
        }
    }
    return Rval;
}

UINT32 RefDcf_CheckDnumDirty(const REF_DCF_HDLR_IMPL_s *Hdlr, UINT32 Dnum, UINT8 *IsDirty)
{
    UINT32 Rval = DCF_OK;
    UINT8 Result = 0U;
    UINT32 i;
    const REF_DCF_ROOT_INFO_s *Root;
    *IsDirty = 0U;
    if (Hdlr->State != DCF_STATE_DONE) {
        for (i = 0; i < g_Dcf.MaxRootPerHdlr; i++) {
            Root = &Hdlr->RootInfo[i];
            if (Root->Used != 0U) {
                if (IS_ROOT_DIRTY(Root) != 0U) {
                    Rval = RefDcf_CheckRootDnumDirty(Root, Dnum, &Result);
                    if ((Rval != DCF_OK) || (Result != 0U)) {
                        *IsDirty = 1U;
                        break;
                    }
                }
            }
        }
    }
    return Rval;
}

