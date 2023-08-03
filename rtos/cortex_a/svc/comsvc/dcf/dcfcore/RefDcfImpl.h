/**
 *  @file RefDcfImpl.h
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
 *  @details Header File of DCF Internal Functions
 *
 */
#ifndef REF_DCF_IMPL_H
#define REF_DCF_IMPL_H

#include "RefDcf.h"
#include <AmbaCache_Def.h>

#define DCF_TIMEOUT_MUTEX       (30U * 1000U)
#define DCF_TIMEOUT_EVTFLAG     (30U * 1000U)

#define DCF_STATE_UNUSED    (0U)
#define DCF_STATE_IDLE      (1U)
#define DCF_STATE_ROOT      (2U)    // set to root once add root
#define DCF_STATE_SCAN      (3U)    // set to scan since the first scan/firstd/lastd
#define DCF_STATE_DONE      (4U)    // set to done after all dnum are scanned

typedef struct {
    char Path[AMBA_DCF_MAX_FILENAME_LENGTH]; /**< path name */
    UINT64 *DnumDirty;  /**< Dnum mask (size = MaxDnum + 1), allocated at RefDcf_Init(), initialized as {0}. Bit 0 is for the entire root */
    UINT8 Used;
} REF_DCF_ROOT_INFO_s;

typedef struct {
    REF_DCF_HDLR_s Hdlr;
    REF_DCF_FILTER_s *Filter;   /**< DCF name filter */
    REF_DCF_TABLE_HDLR_s *Table;
    REF_DCF_ROOT_INFO_s *RootInfo;
    UINT32 CurDcfId;    /**< current DCF object Id */
    UINT32 CurDcfDnum;  /**< current DCF directory number */
    UINT8 State;
} REF_DCF_HDLR_IMPL_s;

UINT32 RefDcf_GetCoreBufferSize(UINT32 MaxDirPerDnum, UINT32 *BufferSize);

UINT32 RefDcf_InitCore(UINT8 MaxDirPerDnum, const UINT8 *Buffer, UINT32 BufferSize);

UINT32 RefDcf_CreateTable(UINT32 MaxDir, UINT32 MaxFile, const REF_DCF_TABLE_s *Func, REF_DCF_TABLE_HDLR_s **Table);

UINT32 RefDcf_DeleteTable(const REF_DCF_TABLE_s *Func, REF_DCF_TABLE_HDLR_s *Table);

UINT32 RefDcf_ScanDirToTable(const REF_DCF_FILTER_s *Filter, REF_DCF_TABLE_HDLR_s *Table, char *Path, REF_DCF_DIR_INFO_FP DirInfo);

UINT32 RefDcf_ScanFileToTable(const REF_DCF_ROOT_INFO_s *Root, const REF_DCF_FILTER_s *Filter, REF_DCF_TABLE_HDLR_s *Table, UINT32 Dnum);

UINT32 RefDcf_AddDirectoryToTable(REF_DCF_TABLE_HDLR_s *Table, UINT32 Dnum, char *Name);

UINT32 RefDcf_RemoveDirectoryFromTable(REF_DCF_TABLE_HDLR_s *Table, UINT32 Dnum, char *Name);

UINT32 RefDcf_AddFileToTable(REF_DCF_TABLE_HDLR_s *Table, UINT32 Dnum, UINT32 Fnum, char *Name);

UINT32 RefDcf_RemoveFileFromTable(REF_DCF_TABLE_HDLR_s *Table, UINT32 Dnum, UINT32 Fnum, char *Name);

UINT32 RefDcf_GetFirstDnumFromTable(REF_DCF_TABLE_HDLR_s *Table, UINT32 *First);

UINT32 RefDcf_GetLastDnumFromTable(REF_DCF_TABLE_HDLR_s *Table, UINT32 *Last);

UINT32 RefDcf_GetNextDnumFromTable(REF_DCF_TABLE_HDLR_s *Table, UINT32 Dnum, UINT32 *Next);

UINT32 RefDcf_GetPrevDnumFromTable(REF_DCF_TABLE_HDLR_s *Table, UINT32 Dnum, UINT32 *Prev);

UINT32 RefDcf_GetFirstIdFromTable(const REF_DCF_HDLR_IMPL_s *Hdlr, UINT32 *First);

UINT32 RefDcf_GetLastIdFromTable(const REF_DCF_HDLR_IMPL_s *Hdlr, UINT32 *Last);

UINT32 RefDcf_GetNextIdFromTable(const REF_DCF_HDLR_IMPL_s *Hdlr, UINT32 Dnum, UINT32 Fnum, UINT32 *Next);

UINT32 RefDcf_GetPrevIdFromTable(const REF_DCF_HDLR_IMPL_s *Hdlr, UINT32 Dnum, UINT32 Fnum, UINT32 *Prev);

UINT32 RefDcf_GetDirectoryListFromTable(REF_DCF_TABLE_HDLR_s *Table, UINT32 Dnum, REF_DCF_DIR_s *DirList, UINT32 MaxDir, UINT32 *Count);

UINT32 RefDcf_GetFileListFromTable(REF_DCF_TABLE_HDLR_s *Table, UINT32 Dnum, UINT32 Fnum, REF_DCF_FILE_s *FileList, UINT32 MaxFile, UINT32 *Count);

UINT32 RefDcf_TableCheckIdValid(REF_DCF_TABLE_HDLR_s *Table, UINT32 Dnum, UINT32 Fnum, UINT8 *Valid);

UINT8 RefDcf_CompareRoot(const REF_DCF_ROOT_INFO_s *Root, const char *Name);

UINT8 RefDcf_IsValidDnum(const UINT32 Dnum);

UINT32 RefDcf_ClearRootDnumDirty(const REF_DCF_ROOT_INFO_s *Root, UINT32 Dnum);

UINT32 RefDcf_CheckRootDnumDirty(const REF_DCF_ROOT_INFO_s *Root, UINT32 Dnum, UINT8 *IsDirty);

UINT32 RefDcf_CheckDnumDirty(const REF_DCF_HDLR_IMPL_s *Hdlr, UINT32 Dnum, UINT8 *IsDirty);

#endif

