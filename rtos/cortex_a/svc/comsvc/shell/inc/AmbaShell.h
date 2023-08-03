/**
 *  @file AmbaShell.h
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
 *  @details Header file for Ambarella Shell functions.
 *
 */

#ifndef AMBA_SHELL_H
#define AMBA_SHELL_H

#ifndef AMBA_TYPES_H
#include <AmbaTypes.h>
#endif

/* SHELL_ERR_START definition */
#define SHELL_ERR_START             0x91AA0000U
/* SHELL_ERR_SUCCESS definition */
#define SHELL_ERR_SUCCESS           (0U)
/* SHELL_ERR_INVALID_PARAMETER definition */
#define SHELL_ERR_INVALID_PARAMETER (SHELL_ERR_START + 0x01U)
/* SHELL_ERR_RESOURCE_CREATE definition */
#define SHELL_ERR_RESOURCE_CREATE   (SHELL_ERR_START + 0x02U)
/* SHELL_ERR_MUTEX_LOCK definition */
#define SHELL_ERR_MUTEX_LOCK        (SHELL_ERR_START + 0x10U)
/* SHELL_ERR_MUTEX_UNLOCK definition */
#define SHELL_ERR_MUTEX_UNLOCK      (SHELL_ERR_START + 0x11U)
/* SHELL_ERR_FILE_OPEN definition */
#define SHELL_ERR_FILE_OPEN         (SHELL_ERR_START + 0x20U)
/* SHELL_ERR_FILE_WRITE definition */
#define SHELL_ERR_FILE_WRITE        (SHELL_ERR_START + 0x21U)
/* SHELL_ERR_CMD_PARSE definition */
#define SHELL_ERR_CMD_PARSE         (SHELL_ERR_START + 0x30U)
/* SHELL_ERR_CMD_EXEC definition */
#define SHELL_ERR_CMD_EXEC          (SHELL_ERR_START + 0x31U)
/* SHELL_ERR_CMD_FAIL definition */
#define SHELL_ERR_CMD_FAIL          (SHELL_ERR_START + 0x3FU)

/* SHELL_ERR_KEY_CODE definition */
#define SHELL_ERR_KEY_CODE          (SHELL_ERR_START + 0xA0U)

/* SHELL_ERR_FAIL definition */
#define SHELL_ERR_FAIL              (SHELL_ERR_START + 0x0FFU)


/* AMBA_SHELL_MAX_CMD_LINE_SIZE definition */
#define AMBA_SHELL_MAX_CMD_LINE_SIZE    (256U)
/* AMBA_SHELL_MAX_DIR_LENGTH definition */
#define AMBA_SHELL_MAX_DIR_LENGTH       (256U)
/* AMBA_SHELL_MAX_ARG_VECTOR definition */
#define AMBA_SHELL_MAX_ARG_VECTOR       (32U)
/* AMBA_SHELL_MAX_ARG_LENGTH definition */
#define AMBA_SHELL_MAX_ARG_LENGTH       (32U)
/* AMBA_SHELL_MAX_CMD_HISTORY_SIZE definition */
#define AMBA_SHELL_MAX_CMD_HISTORY_SIZE (1024)

/* AMBA_SHELL_GET_CHAR_f definition */
typedef UINT32 (*AMBA_SHELL_GET_CHAR_f)(UINT32 RxDataSize, char *pRxDataBuf, UINT32 TimeOut);
/* AMBA_SHELL_PUT_CHAR_f definition */
typedef UINT32 (*AMBA_SHELL_PUT_CHAR_f)(UINT32 TxDataSize, const char *pTxDataBuf, UINT32 TimeOut);
/* AMBA_SHELL_PRINT_f definition */
typedef void (*AMBA_SHELL_PRINT_f)(const char *String);
/* AMBA_SHELL_PROC_f definition */
typedef void (*AMBA_SHELL_PROC_f)(UINT32 ArgCount, char * const *pArgVector, AMBA_SHELL_PRINT_f PrintFunc);

/*
 * SHELL: AMBA_SHELL_COMMAND_s, SHELL command structure
 */
typedef struct AMBA_SHELL_COMMAND_t {
    const char         *pName;              /* Command name */
    AMBA_SHELL_PROC_f   MainFunc;           /* Main function of the command */
    struct AMBA_SHELL_COMMAND_t  *pNext;    /* Pointer to the next command */
} AMBA_SHELL_COMMAND_s;

/*
 * SHELL: AMBA_SHELL_CONFIG_s, SHELL configure structure
 */
typedef struct {
    UINT32                CliTaskPriority;
    UINT32                CliTaskSmpCoreSet;
    UINT8                *pCliTaskStack;
    UINT32                CliTaskStackSize;
    UINT32                ExecTaskPriority;
    UINT32                ExecTaskSmpCoreSet;
    UINT8                *pExecTaskStack;
    UINT32                ExecTaskStackSize;
    AMBA_SHELL_GET_CHAR_f GetCharFunc;
    AMBA_SHELL_PUT_CHAR_f PutCharFunc;
} AMBA_SHELL_CONFIG_s;

/*
 * Defined in AmbaShell.c
 */
UINT32 AmbaShell_Init(const AMBA_SHELL_CONFIG_s *pConfig);
UINT32 AmbaShell_CommandRegister(AMBA_SHELL_COMMAND_s *pNewCmd);
const char *AmbaShell_CurrentWorkDirGet(void);
void   AmbaShell_CurrentWorkDirSet(const char *WorkingDir);
const AMBA_SHELL_COMMAND_s *AmbaShell_CommandListGet(void);
UINT32 AmbaShell_ExecCommand(char *pCmdLine);
#if defined(CONFIG_APP_FLOW_CARDV_AONI)
int AmbaShell_EnableEcho(int enable);
UINT32 AmbaShell_ExecThirdCommand(const char *cmdline);
#endif
#endif
