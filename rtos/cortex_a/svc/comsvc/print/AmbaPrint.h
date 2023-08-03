/**
 *  @file AmbaPrint.h
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
 *  THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 *  WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL
 *  AMBARELLA, INC. OR ITS AFFILIATES BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 *  DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 *  COMPUTER FAILURE OR MALFUNCTION; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 *  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 *  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *  @details Header file for Ambarella Print System functions.
 *
 */

#ifndef AMBA_PRINT_H
#define AMBA_PRINT_H

#ifndef AMBA_TYPES_H
#include <AmbaTypes.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

#define PRINT_ERR_START             0x91990000U
#define PRINT_ERR_SUCCESS           (0U)
#define PRINT_ERR_INVALID_PARAMETER (PRINT_ERR_START + 0x01U)
#define PRINT_ERR_RESOURCE_CREATE   (PRINT_ERR_START + 0x02U)
#define PRINT_ERR_MUTEX_LOCK_FAIL   (PRINT_ERR_START + 0x03U)
#define PRINT_ERR_MUTEX_UNLOCK_FAIL (PRINT_ERR_START + 0x04U)
#define PRINT_ERR_DEV_UNAVAILABLE   (PRINT_ERR_START + 0x05U)

typedef void (*AMBA_PRINT_PUT_CHAR_f)(UINT32 TxDataSize, const char *pTxDataBuf, UINT32 TimeOut);
typedef INT32 (*AMBA_PRINT_GET_DSP_MSG_f)(char *pMsgData);

typedef struct {
    UINT32   TaskPriority;       // Print task priority
    UINT32   SmpCoreSet;         // A given set of CPUs on the SMP system allowed for print task execution
    UINT32   PrintBufferSize;    // Byte size of ready to print string buffer
    UINT32   LogBufferSize;      // Byte size of ready to log string buffer
    UINT8   *pPrintBuffer;       // Pointer to ready to print string buffer
    UINT8   *pLogBuffer;         // Pointer to ready to log string buffer
    UINT32   TaskStackSize;      // Print task stack size
    UINT8   *pTaskStack;         // Print task stack
    AMBA_PRINT_PUT_CHAR_f PutCharFunc; // Function pointer to output message strings
} AMBA_PRINT_CONFIG_s;

UINT32 AmbaPrint_Init(const AMBA_PRINT_CONFIG_s *pPrintConfig);
void AmbaPrint_PrintStr5(const char *pFmt, const char *pArg1, const char *pArg2, const char *pArg3, const char *pArg4, const char *pArg5);
void AmbaPrint_PrintULL5(const char *pFmt, UINT64 Arg1, UINT64 Arg2, UINT64 Arg3, UINT64 Arg4, UINT64 Arg5);
void AmbaPrint_PrintUInt5(const char *pFmt, UINT32 Arg1, UINT32 Arg2, UINT32 Arg3, UINT32 Arg4, UINT32 Arg5);
void AmbaPrint_PrintInt5(const char *pFmt, INT32 Arg1, INT32 Arg2, INT32 Arg3, INT32 Arg4, INT32 Arg5);
void AmbaPrint_Flush(void);

// new APIs
#define PRINT_COND_YES   (1U)
#define PRINT_COND_NO    (0U)

void   AmbaPrint_StopAndFlush(void);
void   AmbaPrint_ModulePrintStr5(UINT16 ModuleID, const char *pFmt, const char *pArg1, const char *pArg2, const char *pArg3, const char *pArg4, const char *pArg5);
void   AmbaPrint_ModulePrintULL5(UINT16 ModuleID, const char *pFmt, UINT64 Arg1, UINT64 Arg2, UINT64 Arg3, UINT64 Arg4, UINT64 Arg5);
void   AmbaPrint_ModulePrintUInt5(UINT16 ModuleID, const char *pFmt, UINT32 Arg1, UINT32 Arg2, UINT32 Arg3, UINT32 Arg4, UINT32 Arg5);
void   AmbaPrint_ModulePrintInt5(UINT16 ModuleID, const char *pFmt, INT32 Arg1, INT32 Arg2, INT32 Arg3, INT32 Arg4, INT32 Arg5);
UINT32 AmbaPrint_ModuleSetAllowList(UINT16 ModuleID, UINT8 Enable);

#ifdef __cplusplus
}
#endif
#endif
