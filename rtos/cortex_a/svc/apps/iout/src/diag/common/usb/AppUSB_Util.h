/**
 *  @file AppUSB_Util.h
 *
 *  @copyright Copyright (c) 2020 Ambarella, Inc.
 *
 *  This file and its contents ("Software") are protected by intellectual property rights including, without limitation,
 *  U.S. and/or foreign copyrights.  This Software is also the confidential and proprietary information of Ambarella, Inc.
 *  and its licensors.  You may not use, reproduce, disclose, distribute, modify, or otherwise prepare derivative works
 *  of this Software or any portion thereof except pursuant to a signed license agreement or nondisclosure agreement with
 *  Ambarella, Inc. or its authorized affiliates.  In the absence of such an agreement, you agree to promptly notify and
 *  return this Software to Ambarella, Inc.
 *
 *  THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 *  WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL
 *  AMBARELLA, INC. OR ITS AFFILIATES BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 *  DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 *  COMPUTER FAILURE OR MALFUNCTION; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 *  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 *  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *  @details Utility functions for USB diag command.
 */
#ifndef APPUSB_UTIL_H
#define APPUSB_UTIL_H

#ifndef AMBA_FS_H
#include "AmbaFS.h"
#endif
#include "AmbaPrint.h"
#include <AmbaWrap.h>

VOID  AppUsb_MemoryZeroSet(void *pDest, UINT32 Size);
VOID  AppUsb_MemorySet(void *pDest, INT32 Value, UINT32 Size);
VOID  AppUsb_MemoryCopy(void *pDest, const void *pSrc, SIZE_t Size);
UINT32 AppUsb_TaskCreate(
    AMBA_KAL_TASK_t *pTask,
    char *           pTaskName,
    UINT32           Priority,
    void *(*         EntryFunction)(void *Arg),
    void *           EntryArg,
    void *           pStackBase,
    UINT32           StackByteSize,
    UINT32           AutoStart,
    UINT32           AffinityMask);
VOID   AppUsb_TaskSleep(UINT32 Ms);
UINT32 AppUsb_TaskResume(AMBA_KAL_TASK_t *pTask);
UINT32 AppUsb_TaskSuspend(AMBA_KAL_TASK_t *pTask);
UINT32 AppUsb_TaskDelete(AMBA_KAL_TASK_t *pTask, UINT32 Suspended);
UINT32 AppUsb_StringPrintStr(char *Buffer, const char *FmtString, UINT32 Count, const char * const Arg[]);
UINT32 AppUsb_UInt32ToStr(char *Buffer, UINT32 BufSize, UINT32 Value, UINT32 Radix);
UINT32 AppUsb_StringLength(const char *pString);
VOID   AppUsb_StringCat(char *pDestination, UINT32 BufferSize, const char *Source);
VOID   AppUsb_StringAppendUInt32(char *Buffer, UINT32 BufferSize, UINT32 Value, UINT32 Radix);
VOID   AppUsb_StringToUINT32(const char *pString, UINT32 *pValue);
VOID  *AppUsb_U8p2Vp(const UINT8* Ptr);
UINT8 *AppUsb_Vp2U8p(const VOID* Ptr);
VOID   AppUsb_Ascii2Unicode(const char *pAsciiStr, WCHAR *pUnicodeStr);
SIZE_t AppUsb_WStrLen(const WCHAR *WString);
void AppUsb_PrintStr5(const char *pFmt, const char *pArg1, const char *pArg2, const char *pArg3, const char *pArg4, const char *pArg5);
void AppUsb_PrintUInt5(const char *pFmt, UINT32 pArg1, UINT32 pArg2, UINT32 pArg3, UINT32 pArg4, UINT32 pArg5);
void AppUsb_PrintInt5(const char *pFmt, INT32 pArg1, INT32 pArg2, INT32 pArg3, INT32 pArg4, INT32 pArg5);
void AppUsb_Print(const char *pFmt);
#endif

