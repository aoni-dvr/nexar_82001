/**
 *  @file AmbaShell_Commands.c
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
 *  @details header file of shell command.
 *
 */

#ifndef AMBA_SHELL_COMMANDS_H
#define AMBA_SHELL_COMMANDS_H

#ifndef AMBA_TYPES_H
#include <AmbaTypes.h>
#endif

#ifndef AMBA_VER_H
#include <AmbaVer.h>
#endif

#ifndef AMBA_UTILITY_H
#include <AmbaUtility.h>
#endif
#ifndef AMBA_SHELL_H
#include <AmbaShell.h>
#endif

typedef void (*AMBA_SHELL_LIBINFO_SET_f)(AMBA_VerInfo_s *pVerInfo);

/*
 * SHELL: AMBA SHELL lib info structure
 */
typedef struct {
    const char               *pLibName;
    AMBA_SHELL_LIBINFO_SET_f  FuncSet;
} AMBA_SHELL_LIBINFO;

/*
 * SHELL: AMBA SHELL link info structure
 */
typedef struct {
    const char *pLinkTarget;
    const char *pLinkMachine;
    const char *pLinkDate;
} AMBA_SHELL_LINKINFO;

#ifdef __cplusplus
extern "C" {
#endif

void AmbaShell_CommandCat(UINT32 ArgCount, char * const * pArgVector, AMBA_SHELL_PRINT_f PrintFunc);
void AmbaShell_CommandChangeDir(UINT32 ArgCount, char * const * pArgVector, AMBA_SHELL_PRINT_f PrintFunc);
void AmbaShell_CommandCRC32Sum(UINT32 ArgCount, char * const * pArgVector, AMBA_SHELL_PRINT_f PrintFunc);
void AmbaShell_CommandClear(UINT32 ArgCount, char * const * pArgVector, AMBA_SHELL_PRINT_f PrintFunc);
void AmbaShell_CommandCopy(UINT32 ArgCount, char * const * pArgVector, AMBA_SHELL_PRINT_f PrintFunc);
void AmbaShell_CommandDiag(UINT32 ArgCount, char * const * pArgVector, AMBA_SHELL_PRINT_f PrintFunc);
void AmbaShell_CommandDate(UINT32 ArgCount, char * const * pArgVector, AMBA_SHELL_PRINT_f PrintFunc);
void AmbaShell_CommandEcho(UINT32 ArgCount, char * const * pArgVector, AMBA_SHELL_PRINT_f PrintFunc);
void AmbaShell_CommandFalse(UINT32 ArgCount, char * const * pArgVector, AMBA_SHELL_PRINT_f PrintFunc);
void AmbaShell_CommandRandvrfy(UINT32 ArgCount, char * const * pArgVector, AMBA_SHELL_PRINT_f PrintFunc);
void AmbaShell_CommandFormat(UINT32 ArgCount, char * const * pArgVector, AMBA_SHELL_PRINT_f PrintFunc);
void AmbaShell_CommandEject(UINT32 ArgCount, char * const * pArgVector, AMBA_SHELL_PRINT_f PrintFunc);
void AmbaShell_CommandHelp(UINT32 ArgCount, char * const * pArgVector, AMBA_SHELL_PRINT_f PrintFunc);
void AmbaShell_CommandListDir(UINT32 ArgCount, char * const * pArgVector, AMBA_SHELL_PRINT_f PrintFunc);
void AmbaShell_CommandMakeDir(UINT32 ArgCount, char * const * pArgVector, AMBA_SHELL_PRINT_f PrintFunc);
void AmbaShell_CommandReadLong(UINT32 ArgCount, char * const * pArgVector, AMBA_SHELL_PRINT_f PrintFunc);
void AmbaShell_CommandReadWord(UINT32 ArgCount, char * const * pArgVector, AMBA_SHELL_PRINT_f PrintFunc);
void AmbaShell_CommandReadByte(UINT32 ArgCount, char * const * pArgVector, AMBA_SHELL_PRINT_f PrintFunc);
void AmbaShell_CommandWriteLong(UINT32 ArgCount, char * const * pArgVector, AMBA_SHELL_PRINT_f PrintFunc);
void AmbaShell_CommandWriteWord(UINT32 ArgCount, char * const * pArgVector, AMBA_SHELL_PRINT_f PrintFunc);
void AmbaShell_CommandWriteByte(UINT32 ArgCount, char * const * pArgVector, AMBA_SHELL_PRINT_f PrintFunc);
void AmbaShell_CommandMove(UINT32 ArgCount, char * const * pArgVector, AMBA_SHELL_PRINT_f PrintFunc);
void AmbaShell_CommandMisc(UINT32 ArgCount, char * const * pArgVector, AMBA_SHELL_PRINT_f PrintFunc);
void AmbaShell_CommandPrintWorkDir(UINT32 ArgCount, char * const * pArgVector, AMBA_SHELL_PRINT_f PrintFunc);
void AmbaShell_CommandSleep(UINT32 ArgCount, char * const * pArgVector, AMBA_SHELL_PRINT_f PrintFunc);
void AmbaShell_CommandProcessStatus(UINT32 ArgCount, char * const * pArgVector, AMBA_SHELL_PRINT_f PrintFunc);
void AmbaShell_CommandReboot(UINT32 ArgCount, char * const * pArgVector, AMBA_SHELL_PRINT_f PrintFunc);
void AmbaShell_CommandRemoveDir(UINT32 ArgCount, char * const * pArgVector, AMBA_SHELL_PRINT_f PrintFunc);
void AmbaShell_CommandRemove(UINT32 ArgCount, char * const *pArgVector, AMBA_SHELL_PRINT_f PrintFunc);
void AmbaShell_CommandSysInfo(UINT32 ArgCount, char * const *pArgVector, AMBA_SHELL_PRINT_f PrintFunc);
void AmbaShell_SdShmoo(UINT32 ArgCount, char * const * pArgVector, AMBA_SHELL_PRINT_f PrintFunc);
void AmbaShell_CommandTrue(UINT32 ArgCount, char * const * pArgVector, AMBA_SHELL_PRINT_f PrintFunc);
void AmbaShell_CommandVolumn(UINT32 ArgCount, char * const * pArgVector, AMBA_SHELL_PRINT_f PrintFunc);
void AmbaShell_CommandFsThruput(UINT32 ArgCount, char * const * pArgVector, AMBA_SHELL_PRINT_f PrintFunc);

void AmbaShell_CommandDisplayMsg(UINT32 ArgCount, char * const * pArgVector, AMBA_SHELL_PRINT_f PrintFunc);
void AmbaShell_CommandSaveBinary(UINT32 ArgCount, char * const * pArgVector, AMBA_SHELL_PRINT_f PrintFunc);
void AmbaShell_CommandLoadBinary(UINT32 ArgCount, char * const * pArgVector, AMBA_SHELL_PRINT_f PrintFunc);
void AmbaShell_CommandSaveImage(UINT32 ArgCount, char * const * pArgVector, AMBA_SHELL_PRINT_f PrintFunc);
void AmbaShell_CommandVersion(UINT32 Argc, char * const * pArgv, AMBA_SHELL_PRINT_f PrintFunc);

void   AmbaShell_CommandVersionInit(AMBA_SHELL_LIBINFO *pLibInfo, UINT32 InfoNumber, AMBA_SHELL_LINKINFO *pLinkInfo);
UINT32 AmbaShell_CommandDspLogInit(const void *pDspDebugLogBuf, UINT32 DspDebugLogSize);

void AmbaShell_CommandUsb(UINT32 ArgCount, char * const * pArgVector, AMBA_SHELL_PRINT_f PrintFunc);
void AmbaShell_CommandAtf(UINT32 ArgCount, char * const * pArgVector, AMBA_SHELL_PRINT_f PrintFunc);
void AmbaShell_CommandIoStat(UINT32 ArgCount, char * const * pArgVector, AMBA_SHELL_PRINT_f PrintFunc);

#if defined(CONFIG_SOC_CV2FS)
void AmbaShell_CommandFuSa(UINT32 ArgCount, char * const * pArgVector, AMBA_SHELL_PRINT_f PrintFunc);
#endif

#if defined(CONFIG_QNX) || defined(CONFIG_LINUX)
void AmbaShell_CommandExec(UINT32 ArgCount, char * const * pArgVector, AMBA_SHELL_PRINT_f PrintFunc);
#endif

#ifdef __cplusplus
}
#endif

#endif
