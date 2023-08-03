/**
 *  @file AmbaShell_KeyCode.h
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
 *  @details Header file for Ambarella Shell functions for converting KeyCode.
 *
 */

#ifndef AMBA_SHELL_KEY_CODE_H
#define AMBA_SHELL_KEY_CODE_H

#ifndef AMBA_TYPES_H
#include <AmbaTypes.h>
#endif

/* shell key definitions */
typedef enum {
    AMBA_SHELL_KEY_NUM_LOCK = 0,
    AMBA_SHELL_KEY_TAB,
    AMBA_SHELL_KEY_BACKSPACE,
    AMBA_SHELL_KEY_CTRL_C,
    AMBA_SHELL_KEY_UP_ARROW,
    AMBA_SHELL_KEY_DOWN_ARROW,
    AMBA_SHELL_KEY_RIGHT_ARROW,
    AMBA_SHELL_KEY_LEFT_ARROW,
    AMBA_SHELL_KEY_INSERT,
    AMBA_SHELL_KEY_DELETE,
    AMBA_SHELL_KEY_HOME,
    AMBA_SHELL_KEY_END,
    AMBA_SHELL_KEY_PAGE_UP,
    AMBA_SHELL_KEY_PAGE_DOWN,
    AMBA_SHELL_KEY_F1,
    AMBA_SHELL_KEY_F2,
    AMBA_SHELL_KEY_F3,
    AMBA_SHELL_KEY_F4,
    AMBA_SHELL_KEY_F5,
    AMBA_SHELL_KEY_F6,
    AMBA_SHELL_KEY_F7,
    AMBA_SHELL_KEY_F8,
    AMBA_SHELL_KEY_F9,
    AMBA_SHELL_KEY_F10,
    AMBA_SHELL_KEY_F11,
    AMBA_SHELL_KEY_F12,

    AMBA_SHELL_KEY_DIVIDE,
    AMBA_SHELL_KEY_MULTIPLY,
    AMBA_SHELL_KEY_SUBTRACT,
    AMBA_SHELL_KEY_ADD,

    AMBA_SHELL_NUM_KEY
} AMBA_SHELL_KEY_CODE_e;

/*
 * SHELL: AMBA SHELL key code structure
 */
typedef struct {
    const char    *pSeq;
    UINT32         NumChar;
    AMBA_SHELL_KEY_CODE_e Code;
} AMBA_SHELL_KEY_SEQ_s;

/*
 * Defined in AmbaShell_KryCode.c
 */
void AmbaShell_ResetKeyCode(void);
UINT32 AmbaShell_GetKeyCode(char ChValue, AMBA_SHELL_KEY_CODE_e *pKeyCode);

#endif
