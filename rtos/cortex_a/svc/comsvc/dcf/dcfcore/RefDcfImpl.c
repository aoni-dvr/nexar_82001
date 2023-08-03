/**
 *  @file RefDcfImpl.c
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
 *  @details DCF Internal Functions
 *
 */
#include "RefDcf_Util.h"
#include "RefDcfImpl.h"
#include <AmbaKAL.h>
#include <AmbaDef.h>
#include <AmbaMisraFix.h>


typedef struct {
    UINT32 MaxDirPerDnum;
    REF_DCF_DIR_s *DirList; // only is used in Scan, so no need mutex protection
} REF_DCF_CORE_s;

static REF_DCF_CORE_s g_DcfCore GNU_SECTION_NOZEROINIT;

static inline void Dcf_Perror(const char *Func, UINT32 Line, const char *Message)
{
    char LineStr[8];
    (void)AmbaUtility_UInt32ToStr(LineStr, sizeof(LineStr), Line, 10);
    AmbaPrint_PrintStr5("[Error]%s:%s %s", Func, LineStr, Message, NULL, NULL);
}

UINT32 RefDcf_GetCoreBufferSize(UINT32 MaxDirPerDnum, UINT32 *BufferSize)
{
    UINT32 Rval = DCF_OK;
    if ((MaxDirPerDnum == 0U) || (BufferSize == NULL)) {
        Dcf_Perror(__FUNCTION__, __LINE__, "Invalid argument!");
        Rval = DCF_ERR_INVALID_ARG;
    } else {
        *BufferSize = GetAlignedValU32(MaxDirPerDnum * sizeof(REF_DCF_DIR_s), 4U);
    }
    return Rval;
}

UINT32 RefDcf_InitCore(UINT8 MaxDirPerDnum, const UINT8 *Buffer, UINT32 BufferSize)
{
    UINT32 Rval;
    if ((MaxDirPerDnum == 0U) || (Buffer == NULL)) {
        Dcf_Perror(__FUNCTION__, __LINE__, "Invalid argument!");
        Rval = DCF_ERR_INVALID_ARG;
    } else {
        static UINT8 DcfCoreInit = 0U;
        if (DcfCoreInit == 0U) {
            UINT32 GetBufSize;
            Rval = RefDcf_GetCoreBufferSize(MaxDirPerDnum, &GetBufSize);
            if (Rval == DCF_OK) {
                // DirList
                UINT32 AddrU32;
                AmbaMisra_TypeCast32(&AddrU32, &Buffer);
                if ((BufferSize == GetBufSize) && ((AddrU32 % 4U) == 0U)) {
                    // to make sure buffer is 4-bye aligned, to avoid ARM optimization problem
                    g_DcfCore.MaxDirPerDnum = MaxDirPerDnum;
                    AmbaMisra_TypeCast32(&g_DcfCore.DirList, &AddrU32);
                    DcfCoreInit = 1U;
                } else {
                    Dcf_Perror(__FUNCTION__, __LINE__, "Invalid argument!");
                    Rval = DCF_ERR_INVALID_ARG;
                }
            }
        } else {
            Dcf_Perror(__FUNCTION__, __LINE__, "already initiated");
            Rval = DCF_ERR_GENERAL_ERROR;
        }
    }
    return Rval;
}

UINT32 RefDcf_CreateTable(UINT32 MaxDir, UINT32 MaxFile, const REF_DCF_TABLE_s *Func, REF_DCF_TABLE_HDLR_s **Table)
{
    // only called by RefDcf_Create()
    UINT32 Rval;
    if ((MaxDir == 0U) || (MaxFile == 0U) || (Func == NULL) || (Table == NULL)) {
        Dcf_Perror(__FUNCTION__, __LINE__, "Invalid argument!");
        Rval = DCF_ERR_INVALID_ARG;
    } else {
        Rval = Func->Create(MaxDir, MaxFile, Table);
        if (Rval != DCF_OK) {
            Dcf_Perror(__FUNCTION__, __LINE__, "cannot create table");
        }
    }
    return Rval;
}

UINT32 RefDcf_DeleteTable(const REF_DCF_TABLE_s *Func, REF_DCF_TABLE_HDLR_s *Table)
{
    // only called by RefDcf_Delete()
    UINT32 Rval;
    if ((Func == NULL) || (Table == NULL)) {
        Dcf_Perror(__FUNCTION__, __LINE__, "Invalid argument!");
        Rval = DCF_ERR_INVALID_ARG;
    } else {
        Rval = Func->Delete(Table);
        if (Rval != DCF_OK) {
            Dcf_Perror(__FUNCTION__, __LINE__, "cannot delete table");
        }
    }
    return Rval;
}

UINT32 RefDcf_ScanDirToTable(const REF_DCF_FILTER_s *Filter, REF_DCF_TABLE_HDLR_s *Table, char *Path, REF_DCF_DIR_INFO_FP DirInfo)
{
    UINT32 Rval;
    if ((Filter == NULL) || (Table == NULL) || (Path == NULL) || (DirInfo == NULL)) {
        Dcf_Perror(__FUNCTION__, __LINE__, "Invalid argument!");
        Rval = DCF_ERR_INVALID_ARG;
    } else {
        Rval = Filter->ScanDirectory(Path, Table, DirInfo);
        if (Rval != DCF_OK) {
            Dcf_Perror(__FUNCTION__, __LINE__, "cannot scan directory");
        }
    }
    return Rval;
}

UINT32 RefDcf_ScanFileToTable(const REF_DCF_ROOT_INFO_s *Root, const REF_DCF_FILTER_s *Filter, REF_DCF_TABLE_HDLR_s *Table, UINT32 Dnum)
{
    UINT32 Rval;
    if ((Filter == NULL) || (Table == NULL) || (Dnum == ID_NOT_EXISTED)) {
        Dcf_Perror(__FUNCTION__, __LINE__, "Invalid argument!");
        Rval = DCF_ERR_INVALID_ARG;
    } else {
        UINT32 Count;
        Rval = Table->Func->GetDirectoryList(Table, Dnum, g_DcfCore.DirList, g_DcfCore.MaxDirPerDnum, &Count);
        if (Rval == DCF_OK) {
            UINT32 i;
            char *Path;
            for (i = 0; i < Count; i++) {
                Path = g_DcfCore.DirList[i].Name;
                // only scan the directories of the specified root
                if (RefDcf_CompareRoot(Root, Path) != 0U) {
                    Rval = Filter->ScanFile(Path, Table);
                    if (Rval != DCF_OK) {
                        Dcf_Perror(__FUNCTION__, __LINE__, "scan file failed");
                        break;
                    }
                }
            }
            // LOG_INFO("Scan files in Dir %u: %s", Dnum, Rval == DCF_OK ? "OK" : "NG"); // fix me
        } else {
            Dcf_Perror(__FUNCTION__, __LINE__, "GetDirectoryList failed");
        }
        if (Rval == DCF_ERR_OBJECT_NOT_EXISTED) {
            Rval = DCF_OK;
        }
    }
    return Rval;
}

UINT32 RefDcf_AddDirectoryToTable(REF_DCF_TABLE_HDLR_s *Table, UINT32 Dnum, char *Name)
{
    UINT32 Rval = DCF_OK;
    if ((Table == NULL) || (Dnum == ID_NOT_EXISTED) || (Name == NULL)) {
        Dcf_Perror(__FUNCTION__, __LINE__, "Invalid argument!");
        Rval = DCF_ERR_INVALID_ARG;
    } else {
        Rval = Table->Func->AddDirectory(Table, Dnum, Name);
        if (Rval != DCF_OK) {
            Dcf_Perror(__FUNCTION__, __LINE__, "add directory failed");
        }
    }
    return Rval;
}

UINT32 RefDcf_RemoveDirectoryFromTable(REF_DCF_TABLE_HDLR_s *Table, UINT32 Dnum, char *Name)
{
    UINT32 Rval = DCF_OK;
    if ((Table == NULL) || (Name == NULL)) {
        Dcf_Perror(__FUNCTION__, __LINE__, "Invalid argument!");
        Rval = DCF_ERR_INVALID_ARG;
    } else {
        // trick: if Dnum is ID_NOT_EXISTED, it must be from RemoveRoot(), so it is ok to go ahead directly
        if (Dnum == ID_NOT_EXISTED) {
#if 0
            AmbaPrint_PrintStr5("%s : called by RefDcf_RemoveRoot()", __FUNCTION__, NULL, NULL, NULL, NULL);
#endif
        }
        Rval = Table->Func->RemoveDirectory(Table, Name);
        if (Rval != DCF_OK) {
            Dcf_Perror(__FUNCTION__, __LINE__, "remove directory failed");
        }
    }
    return Rval;
}

UINT32 RefDcf_AddFileToTable(REF_DCF_TABLE_HDLR_s *Table, UINT32 Dnum, UINT32 Fnum, char *Name)
{
    UINT32 Rval;
    if ((Table == NULL) || (Dnum == ID_NOT_EXISTED) || (Fnum == ID_NOT_EXISTED) || (Name == NULL)) {
        Dcf_Perror(__FUNCTION__, __LINE__, "Invalid argument!");
        Rval = DCF_ERR_INVALID_ARG;
    } else {
        Rval = Table->Func->AddFile(Table, Dnum, Fnum, Name);
        if (Rval != DCF_OK) {
            Dcf_Perror(__FUNCTION__, __LINE__, "add file failed");
        }
    }
    return Rval;
}

UINT32 RefDcf_RemoveFileFromTable(REF_DCF_TABLE_HDLR_s *Table, UINT32 Dnum, UINT32 Fnum, char *Name)
{
    UINT32 Rval;
    if ((Table == NULL) || (Dnum == ID_NOT_EXISTED) || (Fnum == ID_NOT_EXISTED) || (Name == NULL)) {
        Dcf_Perror(__FUNCTION__, __LINE__, "Invalid argument!");
        Rval = DCF_ERR_INVALID_ARG;
    } else {
        Rval = Table->Func->RemoveFile(Table, Dnum, Fnum, Name);
        if (Rval != DCF_OK) {
            Dcf_Perror(__FUNCTION__, __LINE__, "remove file failed");
        }
    }
    return Rval;
}

UINT32 RefDcf_GetFirstDnumFromTable(REF_DCF_TABLE_HDLR_s *Table, UINT32 *First)
{
    UINT32 Rval;
    if ((Table == NULL) || (First == NULL)) {
        Dcf_Perror(__FUNCTION__, __LINE__, "Invalid argument!");
        Rval = DCF_ERR_INVALID_ARG;
    } else {
        Rval = Table->Func->GetFirstDnum(Table, First);;
        if (Rval != DCF_OK) {
            if (Rval == DCF_ERR_OBJECT_NOT_EXISTED) {
                *First = ID_NOT_EXISTED;
            } else {
                Dcf_Perror(__FUNCTION__, __LINE__, "cannot get first Dnum");
            }
        }
    }
    return Rval;
}

UINT32 RefDcf_GetLastDnumFromTable(REF_DCF_TABLE_HDLR_s *Table, UINT32 *Last)
{
    UINT32 Rval;
    if ((Table == NULL) || (Last == NULL)) {
        Dcf_Perror(__FUNCTION__, __LINE__, "Invalid argument!");
        Rval = DCF_ERR_INVALID_ARG;
    } else {
        Rval = Table->Func->GetLastDnum(Table, Last);
        if (Rval != DCF_OK) {
            if (Rval == DCF_ERR_OBJECT_NOT_EXISTED) {
                *Last = ID_NOT_EXISTED;
            } else {
                Dcf_Perror(__FUNCTION__, __LINE__, "cannot get last Dnum");
            }
        }
    }
    return Rval;
}

UINT32 RefDcf_GetNextDnumFromTable(REF_DCF_TABLE_HDLR_s *Table, UINT32 Dnum, UINT32 *Next)
{
    UINT32 Rval;
    if ((Table == NULL) || (Dnum == ID_NOT_EXISTED) || (Next == NULL)) {
        Dcf_Perror(__FUNCTION__, __LINE__, "Invalid argument!");
        Rval = DCF_ERR_INVALID_ARG;
    } else {
        Rval = Table->Func->GetNextDnum(Table, Dnum, Next);
        if (Rval != DCF_OK) {
            if (Rval == DCF_ERR_OBJECT_NOT_EXISTED) {
                *Next = ID_NOT_EXISTED;
            } else {
                Dcf_Perror(__FUNCTION__, __LINE__, "cannot get next Dnum");
            }
        }
    }
    return Rval;
}

UINT32 RefDcf_GetPrevDnumFromTable(REF_DCF_TABLE_HDLR_s *Table, UINT32 Dnum, UINT32 *Prev)
{
    UINT32 Rval;
    if ((Table == NULL) || (Dnum == ID_NOT_EXISTED) || (Prev == NULL)) {
        Dcf_Perror(__FUNCTION__, __LINE__, "Invalid argument!");
        Rval = DCF_ERR_INVALID_ARG;
    } else {
        Rval = Table->Func->GetPrevDnum(Table, Dnum, Prev);
        if (Rval != DCF_OK) {
            if (Rval == DCF_ERR_OBJECT_NOT_EXISTED) {
                *Prev = ID_NOT_EXISTED;
            } else {
                Dcf_Perror(__FUNCTION__, __LINE__, "cannot get prev Dnum");
            }
        }
    }
    return Rval;
}

UINT32 RefDcf_GetFirstIdFromTable(const REF_DCF_HDLR_IMPL_s *Hdlr, UINT32 *First)
{
    UINT32 Rval;
    if ((Hdlr == NULL) || (Hdlr->Filter == NULL) || (Hdlr->Table == NULL) || (First == NULL)) {
        Dcf_Perror(__FUNCTION__, __LINE__, "Invalid argument!");
        Rval = DCF_ERR_INVALID_ARG;
    } else {
        UINT32 Dnum;
        REF_DCF_TABLE_HDLR_s * const Table = Hdlr->Table;
        const REF_DCF_TABLE_s *Func = Table->Func;
        Rval = Func->GetFirstDnum(Table, &Dnum);
        if (Rval == DCF_OK) {
            UINT32 Fnum = ID_NOT_EXISTED;
            do {
                UINT8 IsDirty = 0U;
                Rval = RefDcf_CheckDnumDirty(Hdlr, Dnum, &IsDirty);
                if (Rval == DCF_OK) {
                    if (IsDirty != 0U) {
                        Rval = DCF_ERR_NOT_READY;
                    } else {
                        Rval = Func->GetFirstFnum(Table, Dnum, &Fnum);
                        if (Rval == DCF_OK) {
                            break;
                        }
                        if (Rval == DCF_ERR_OBJECT_NOT_EXISTED) {
                            Rval = Func->GetNextDnum(Table, Dnum, &Dnum);
                            if ((Rval != DCF_OK) && (Rval != DCF_ERR_OBJECT_NOT_EXISTED)) {
                                Dcf_Perror(__FUNCTION__, __LINE__, "get next Dnum failed");
                            }
                        } else {
                            Dcf_Perror(__FUNCTION__, __LINE__, "get first Fnum failed");
                        }
                    }
                }
            } while (Rval == DCF_OK);
            if (Rval == DCF_OK) {
                // LOG_INFO("DirNum: %d, FileNum: %d", Dnum, Fnum); // fix me
                Rval = Hdlr->Filter->GetId(Dnum, Fnum, First);
                if (Rval == DCF_OK) {
                    // LOG_INFO("The first ID: %u", First); // fix me
                } else {
                    Dcf_Perror(__FUNCTION__, __LINE__, "invalid ID");
                }
            }
        } else {
            if (Rval != DCF_ERR_OBJECT_NOT_EXISTED) {
                Dcf_Perror(__FUNCTION__, __LINE__, "get first Dnum failed");
            }
        }
        if (Rval == DCF_ERR_OBJECT_NOT_EXISTED) {
            *First = ID_NOT_EXISTED;
        }
    }
    return Rval;
}

UINT32 RefDcf_GetLastIdFromTable(const REF_DCF_HDLR_IMPL_s *Hdlr, UINT32 *Last)
{
    UINT32 Rval;
    if ((Hdlr == NULL) || (Hdlr->Filter == NULL) || (Hdlr->Table == NULL) || (Last == NULL)) {
        Dcf_Perror(__FUNCTION__, __LINE__, "Invalid argument!");
        Rval = DCF_ERR_INVALID_ARG;
    } else {
        UINT32 Dnum;
        REF_DCF_TABLE_HDLR_s * const Table = Hdlr->Table;
        const REF_DCF_TABLE_s *Func = Table->Func;
        Rval = Func->GetLastDnum(Table, &Dnum);
        if (Rval == DCF_OK) {
            UINT32 Fnum = ID_NOT_EXISTED;
            do {
                UINT8 IsDirty = 0U;
                Rval = RefDcf_CheckDnumDirty(Hdlr, Dnum, &IsDirty);
                if (Rval == DCF_OK) {
                    if (IsDirty != 0U) {
                        Rval = DCF_ERR_NOT_READY;
                    } else {
                        Rval = Func->GetLastFnum(Table, Dnum, &Fnum);
                        if (Rval == DCF_OK) {
                            break;
                        }
                        if (Rval == DCF_ERR_OBJECT_NOT_EXISTED) {
                            Rval = Func->GetPrevDnum(Table, Dnum, &Dnum);
                            if ((Rval != DCF_OK) && (Rval != DCF_ERR_OBJECT_NOT_EXISTED)) {
                                Dcf_Perror(__FUNCTION__, __LINE__, "get prev Dnum failed");
                            }
                        } else {
                            Dcf_Perror(__FUNCTION__, __LINE__, "get first Fnum failed");
                        }
                    }
                }
            } while (Rval == DCF_OK);
            if (Rval == DCF_OK) {
                // LOG_INFO("DirNum: %d, FileNum: %d", Dnum, Fnum); // fix me
                Rval = Hdlr->Filter->GetId(Dnum, Fnum, Last);
                if (Rval == DCF_OK) {
                    // LOG_INFO("The first ID: %u", Last); // fix me
                } else {
                    Dcf_Perror(__FUNCTION__, __LINE__, "invalid ID");
                }
            }
        } else {
            if (Rval != DCF_ERR_OBJECT_NOT_EXISTED) {
                Dcf_Perror(__FUNCTION__, __LINE__, "get last Dnum failed");
            }
        }
        if (Rval == DCF_ERR_OBJECT_NOT_EXISTED) {
            *Last = ID_NOT_EXISTED;
        }
    }
    return Rval;
}

UINT32 RefDcf_GetNextIdFromTable(const REF_DCF_HDLR_IMPL_s *Hdlr, UINT32 Dnum, UINT32 Fnum, UINT32 *Next)
{
    UINT32 Rval;
    if ((Hdlr == NULL) || (Hdlr->Filter == NULL) || (Hdlr->Table == NULL) || (Next == NULL)) {
        Dcf_Perror(__FUNCTION__, __LINE__, "Invalid argument!");
        Rval = DCF_ERR_INVALID_ARG;
    } else {
        UINT8 IsDirty = 0U;
        Rval = RefDcf_CheckDnumDirty(Hdlr, Dnum, &IsDirty);
        if (Rval == DCF_OK) {
            if (IsDirty != 0U) {
                Rval = DCF_ERR_NOT_READY;
            } else {
                REF_DCF_TABLE_HDLR_s * const Table = Hdlr->Table;
                const REF_DCF_TABLE_s *Func = Table->Func;
                Rval = Func->GetNextFnum(Table, Dnum, Fnum, &Fnum);
                if (Rval != DCF_OK) {
                    if (Rval == DCF_ERR_OBJECT_NOT_EXISTED) {
                        do {
                            Rval = Func->GetNextDnum(Table, Dnum, &Dnum);
                            if (Rval == DCF_OK) {
                                Rval = RefDcf_CheckDnumDirty(Hdlr, Dnum, &IsDirty);
                                if (Rval == DCF_OK) {
                                    if (IsDirty != 0U) {
                                        Rval = DCF_ERR_NOT_READY;
                                    } else {
                                        Rval = Func->GetFirstFnum(Table, Dnum, &Fnum);
                                        if (Rval == DCF_OK) {
                                            break;
                                        }
                                        if (Rval == DCF_ERR_OBJECT_NOT_EXISTED) {
                                            Rval = DCF_OK;  // continue to next Dnum
                                        } else {
                                            Dcf_Perror(__FUNCTION__, __LINE__, "GetFirstFnum failed");
                                        }
                                    }
                                }
                            } else {
                                if (Rval != DCF_ERR_OBJECT_NOT_EXISTED) {
                                    Dcf_Perror(__FUNCTION__, __LINE__, "get prev Dnum failed");
                                }
                            }
                        } while (Rval == DCF_OK);
                    } else {
                        Dcf_Perror(__FUNCTION__, __LINE__, "GetNextFnum failed");
                    }
                }
            }
        }
        if (Rval == DCF_OK) {
            Rval = Hdlr->Filter->GetId(Dnum, Fnum, Next);
            if (Rval == DCF_OK) {
                // LOG_INFO("The next ID: %u", Next);
            } else {
                Dcf_Perror(__FUNCTION__, __LINE__, "invalid ID");
            }
        } else {
            if (Rval == DCF_ERR_OBJECT_NOT_EXISTED) {
                *Next = ID_NOT_EXISTED;
            }
        }
    }
    return Rval;
}

UINT32 RefDcf_GetPrevIdFromTable(const REF_DCF_HDLR_IMPL_s *Hdlr, UINT32 Dnum, UINT32 Fnum, UINT32 *Prev)
{
    UINT32 Rval;
    if ((Hdlr == NULL) || (Hdlr->Filter == NULL) || (Hdlr->Table == NULL) || (Prev == NULL)) {
        Dcf_Perror(__FUNCTION__, __LINE__, "Invalid argument!");
        Rval = DCF_ERR_INVALID_ARG;
    } else {
        UINT8 IsDirty = 0U;
        Rval = RefDcf_CheckDnumDirty(Hdlr, Dnum, &IsDirty);
        if (Rval == DCF_OK) {
            if (IsDirty != 0U) {
                Rval = DCF_ERR_NOT_READY;
            } else {
                REF_DCF_TABLE_HDLR_s * const Table = Hdlr->Table;
                const REF_DCF_TABLE_s *Func = Table->Func;
                Rval = Func->GetPrevFnum(Table, Dnum, Fnum, &Fnum);
                if (Rval != DCF_OK) {
                    if (Rval == DCF_ERR_OBJECT_NOT_EXISTED) {
                        do {
                            Rval = Func->GetPrevDnum(Table, Dnum, &Dnum);
                            if (Rval == DCF_OK) {
                                Rval = RefDcf_CheckDnumDirty(Hdlr, Dnum, &IsDirty);
                                if (Rval == DCF_OK) {
                                    if (IsDirty != 0U) {
                                        Rval = DCF_ERR_NOT_READY;
                                    } else {
                                        Rval = Func->GetLastFnum(Table, Dnum, &Fnum);
                                        if (Rval == DCF_OK) {
                                            break;
                                        } else {
                                            if (Rval == DCF_ERR_OBJECT_NOT_EXISTED) {
                                                Rval = DCF_OK;  // continue to prev Dnum
                                            } else {
                                                Dcf_Perror(__FUNCTION__, __LINE__, "GetLastFnum failed");
                                            }
                                        }
                                    }
                                }
                            } else {
                                if (Rval != DCF_ERR_OBJECT_NOT_EXISTED) {
                                    Dcf_Perror(__FUNCTION__, __LINE__, "get prev Dnum failed");
                                }
                            }
                        } while (Rval == DCF_OK);
                    } else {
                        Dcf_Perror(__FUNCTION__, __LINE__, "GetNextFnum failed");
                    }
                }
            }
        }
        if (Rval == DCF_OK) {
            Rval = Hdlr->Filter->GetId(Dnum, Fnum, Prev);
            if (Rval == DCF_OK) {
                // LOG_INFO("The prev ID: %u", Prev);
            } else {
                Dcf_Perror(__FUNCTION__, __LINE__, "invalid ID");
            }
        } else {
            if (Rval == DCF_ERR_OBJECT_NOT_EXISTED) {
                *Prev = ID_NOT_EXISTED;
            }
        }
    }
    return Rval;
}

UINT32 RefDcf_GetDirectoryListFromTable(REF_DCF_TABLE_HDLR_s *Table, UINT32 Dnum, REF_DCF_DIR_s *DirList, UINT32 MaxDir, UINT32 *Count)
{
    UINT32 Rval;
    if ((Table == NULL) || (Dnum == ID_NOT_EXISTED) || (DirList == NULL) || (MaxDir == 0U) || (MaxDir > g_DcfCore.MaxDirPerDnum) || (Count == NULL)) {
        Dcf_Perror(__FUNCTION__, __LINE__, "Invalid argument!");
        Rval = DCF_ERR_INVALID_ARG;
    } else {
        Rval = Table->Func->GetDirectoryList(Table, Dnum, DirList, MaxDir, Count);
        if (Rval != DCF_OK) {
            Dcf_Perror(__FUNCTION__, __LINE__, "cannot GetDirectoryList");
        }
    }
    return Rval;
}

UINT32 RefDcf_GetFileListFromTable(REF_DCF_TABLE_HDLR_s *Table, UINT32 Dnum, UINT32 Fnum, REF_DCF_FILE_s *FileList, UINT32 MaxFile, UINT32 *Count)
{
    UINT32 Rval;
    if ((Table == NULL) || (Dnum == ID_NOT_EXISTED) || (Fnum == ID_NOT_EXISTED) || (FileList == NULL) || (MaxFile == 0U) || (Count == NULL)) {
        Dcf_Perror(__FUNCTION__, __LINE__, "Invalid argument!");
        Rval = DCF_ERR_INVALID_ARG;
    } else {
        Rval = Table->Func->GetFileList(Table, Dnum, Fnum, FileList, MaxFile, Count);
        if (Rval != DCF_OK) {
            Dcf_Perror(__FUNCTION__, __LINE__, "cannot GetFileList");
        }
    }
    return Rval;
}

UINT32 RefDcf_TableCheckIdValid(REF_DCF_TABLE_HDLR_s *Table, UINT32 Dnum, UINT32 Fnum, UINT8 *Valid)
{
    UINT32 Rval;
    if ((Table == NULL) || (Dnum == ID_NOT_EXISTED) || (Fnum == ID_NOT_EXISTED) || (Valid == NULL)) {
        Dcf_Perror(__FUNCTION__, __LINE__, "Invalid argument!");
        Rval = DCF_ERR_INVALID_ARG;
    } else {
        Rval = Table->Func->CheckIdValid(Table, Dnum, Fnum, Valid);
        if (Rval != DCF_OK) {
            Dcf_Perror(__FUNCTION__, __LINE__, "cannot CheckIdValid");
        }
    }
    return Rval;
}

UINT8 RefDcf_CompareRoot(const REF_DCF_ROOT_INFO_s *Root, const char *Name)
{
    UINT8 Rval = 0U;
    const UINT32 Len = AmbaUtility_StringLength(Root->Path);
    if (AmbaUtility_StringCompare(Root->Path, Name, Len) == 0) {
        Rval = 1U;
    }
    return Rval;
}

UINT32 RefDcf_ClearRootDnumDirty(const REF_DCF_ROOT_INFO_s *Root, UINT32 Dnum)
{
    UINT32 Rval = DCF_OK;
    if (RefDcf_IsValidDnum(Dnum) != 0U) {
        Root->DnumDirty[Dnum / 64U] &= ~(0x0000000000000001ULL << (Dnum % 64U));
    } else {
        Dcf_Perror(__FUNCTION__, __LINE__, "invalid Dnum");
        Rval = DCF_ERR_GENERAL_ERROR;
    }
    return Rval;
}

UINT32 RefDcf_CheckRootDnumDirty(const REF_DCF_ROOT_INFO_s *Root, UINT32 Dnum, UINT8 *IsDirty)
{
    UINT32 Rval = DCF_OK;
    if (RefDcf_IsValidDnum(Dnum) != 0U) {
        if ((Root->DnumDirty[Dnum / 64U] & (0x0000000000000001ULL << (Dnum % 64U))) != 0U) {
            *IsDirty = 1U;
        } else {
            *IsDirty = 0U;
        }
    } else {
        Dcf_Perror(__FUNCTION__, __LINE__, "invalid Dnum");
        Rval = DCF_ERR_GENERAL_ERROR;
    }
    return Rval;
}

