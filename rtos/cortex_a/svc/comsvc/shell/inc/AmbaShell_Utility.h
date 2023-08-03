/**
 *  @file AmbaShell_Utility.h
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
 *  @details Header file of utility functions for Shell commands.
 *
 */

#ifndef AMBA_SHELL_UTILITY_H
#define AMBA_SHELL_UTILITY_H

#ifndef AMBA_TYPES_H
#include <AmbaTypes.h>
#endif

#ifndef AMBA_MISRA_FIX_H
#include <AmbaMisraFix.h>
#endif

#ifndef AMBA_UTILITY_H
#include <AmbaUtility.h>
#endif

#ifndef AMBA_SHELL_H
#include <AmbaShell.h>
#endif

#ifndef AMBA_KAL_H
#include <AmbaKAL.h>
#endif

#ifndef AMBA_FS_H
#include <AmbaFS.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

void AmbaShell_PrintUInt64(UINT64 Value, AMBA_SHELL_PRINT_f PrintFunc);
void AmbaShell_PrintUInt32(UINT32 Value, AMBA_SHELL_PRINT_f PrintFunc);
void AmbaShell_PrintHexUInt32(UINT32 Value, AMBA_SHELL_PRINT_f PrintFunc);
void AmbaShell_PrintHexUInt64(UINT64 Value, AMBA_SHELL_PRINT_f PrintFunc);
void AmbaShell_PrintChar(char Value, AMBA_SHELL_PRINT_f PrintFunc);

UINT32 AMBSH_CHKCWD(AMBA_SHELL_PRINT_f PrintFunc);
void AmbaShell_PrintSupportedCmds(AMBA_SHELL_PRINT_f PrintFunc);

UINT32 AmbaShell_FindRepeatSep(const char *pString);
UINT32 AmbaShell_FindWild(const char *pString);
void AmbaShell_StringCopyLength(char *pDest, SIZE_t DestSize, const char *pSrc, SIZE_t CopyLength);
UINT32 AmbaShell_IsVolumePath(const char *pFilePath);

#ifdef CONFIG_THREADX
// no PF_CHAR defined in QNX
char *AmbaShell_PFChar2Char(const PF_CHAR *pfc_ptr);
#endif

#ifdef __cplusplus
}
#endif

#endif
