/**
 *  @file AmbaShell_KeyCode.c
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
 *  @details Ambarella Shell functions for converting KeyCode.
 *
 */

#include "AmbaShell_KeyCode.h"
#include <AmbaShell.h>
#include <AmbaUtility.h>

static UINT32 AmbaShell_CharSeqLen = 0;
void AmbaShell_ResetKeyCode(void)
{
    AmbaShell_CharSeqLen = 0;
}


UINT32 AmbaShell_GetKeyCode(char ChValue, AMBA_SHELL_KEY_CODE_e *pKeyCode)
{
    static char AmbaShell_CharSeqBuf[8];
    static const char code_key_up_arrow[]    = {'\x1b', '[', 'A', '\0'};
    static const char code_key_down_arrow[]  = {'\x1b', '[', 'B', '\0'};
    static const char code_key_right_arrow[] = {'\x1b', '[', 'C', '\0'};
    static const char code_key_left_arrow[]  = {'\x1b', '[', 'D', '\0'};

    static const char code_key_insert[]      = {'\x1b', '[', '2', '~', '\0'};
    static const char code_key_delete[]      = {'\x1b', '[', '3', '~', '\0'};
    static const char code_key_home[]        = {'\x1b', '[', '1', '~', '\0'};
    static const char code_key_end[]         = {'\x1b', '[', '4', '~', '\0'};
    static const char code_key_page_up[]     = {'\x1b', '[', '5', '~', '\0'};
    static const char code_key_page_down[]   = {'\x1b', '[', '6', '~', '\0'};
    static const char code_key_f1[]          = {'\x1b', '[', '1', '1', '~', '\0'};
    static const char code_key_f2[]          = {'\x1b', '[', '1', '2', '~', '\0'};
    static const char code_key_f3[]          = {'\x1b', '[', '1', '3', '~', '\0'};
    static const char code_key_f4[]          = {'\x1b', '[', '1', '4', '~', '\0'};
    static const char code_key_f5[]          = {'\x1b', '[', '1', '5', '~', '\0'};
    static const char code_key_f6[]          = {'\x1b', '[', '1', '7', '~', '\0'};
    static const char code_key_f7[]          = {'\x1b', '[', '1', '8', '~', '\0'};
    static const char code_key_f8[]          = {'\x1b', '[', '1', '9', '~', '\0'};
    static const char code_key_f9[]          = {'\x1b', '[', '2', '0', '~', '\0'};
    static const char code_key_f10[]         = {'\x1b', '[', '2', '1', '~', '\0'};
    static const char code_key_f11[]         = {'\x1b', '[', '2', '3', '~', '\0'};
    static const char code_key_f12[]         = {'\x1b', '[', '2', '4', '~', '\0'};

    static const char code_key_ctrl_c[]      = {'\x03', '\0'};
    static const char code_key_back_space[]  = {'\x08', '\0'};
    static const char code_key_tab[]         = {'\x09', '\0'};
    static const char code_key_delete2[]     = {'\x7F', '\0'};
    static const char code_key_add[]         = {'\x2C', '\0'};

    static const char code_key_num_lock[]    = {'\x1b', 'O', 'P', '\0'};
    static const char code_key_divide[]      = {'\x1b', 'O', 'Q', '\0'};
    static const char code_key_multiply[]    = {'\x1b', 'O', 'R', '\0'};
    static const char code_key_substract[]   = {'\x1b', 'O', 'S', '\0'};
    static const char code_key_add2[]        = {'\x1b', 'O', 'm', '\0'};

    /* ASCII code or ANSI escape code (or escape sequences) */
    static const AMBA_SHELL_KEY_SEQ_s AmbaShell_EscapeCode[] = {
        { code_key_up_arrow,     3, AMBA_SHELL_KEY_UP_ARROW     },
        { code_key_down_arrow,   3, AMBA_SHELL_KEY_DOWN_ARROW   },
        { code_key_right_arrow,  3, AMBA_SHELL_KEY_RIGHT_ARROW  },
        { code_key_left_arrow,   3, AMBA_SHELL_KEY_LEFT_ARROW   },

        { code_key_insert,    4, AMBA_SHELL_KEY_INSERT       },
        { code_key_delete,    4, AMBA_SHELL_KEY_DELETE       },
        { code_key_home,      4, AMBA_SHELL_KEY_HOME         },
        { code_key_end,       4, AMBA_SHELL_KEY_END          },
        { code_key_page_up,   4, AMBA_SHELL_KEY_PAGE_UP      },
        { code_key_page_down, 4, AMBA_SHELL_KEY_PAGE_DOWN    },
        { code_key_f1,  5, AMBA_SHELL_KEY_F1           },
        { code_key_f2,  5, AMBA_SHELL_KEY_F2           },
        { code_key_f3,  5, AMBA_SHELL_KEY_F3           },
        { code_key_f4,  5, AMBA_SHELL_KEY_F4           },
        { code_key_f5,  5, AMBA_SHELL_KEY_F5           },
        { code_key_f6,  5, AMBA_SHELL_KEY_F6           },
        { code_key_f7,  5, AMBA_SHELL_KEY_F7           },
        { code_key_f8,  5, AMBA_SHELL_KEY_F8           },
        { code_key_f9,  5, AMBA_SHELL_KEY_F9           },
        { code_key_f10, 5, AMBA_SHELL_KEY_F10          },
        { code_key_f11, 5, AMBA_SHELL_KEY_F11          },
        { code_key_f12, 5, AMBA_SHELL_KEY_F12          },

        { code_key_ctrl_c,     1, AMBA_SHELL_KEY_CTRL_C       },
        { code_key_back_space, 1, AMBA_SHELL_KEY_BACKSPACE    },
        { code_key_tab,        1, AMBA_SHELL_KEY_TAB          },
        { code_key_delete2,    1, AMBA_SHELL_KEY_DELETE       },
        { code_key_add,        1, AMBA_SHELL_KEY_ADD          },

        { code_key_num_lock,   3, AMBA_SHELL_KEY_NUM_LOCK     },
        { code_key_divide,     3, AMBA_SHELL_KEY_DIVIDE       },
        { code_key_multiply,   3, AMBA_SHELL_KEY_MULTIPLY     },
        { code_key_substract,  3, AMBA_SHELL_KEY_SUBTRACT     },
        { code_key_add2,       3, AMBA_SHELL_KEY_ADD          },
    };

    static SIZE_t NumKeyCode = sizeof(AmbaShell_EscapeCode) / sizeof(AmbaShell_EscapeCode[0]);
    UINT32 StringLen;
    const char *pString = &AmbaShell_CharSeqBuf[0];
    UINT32 RetVal = SHELL_ERR_KEY_CODE;
    char   char_esc = '\x1b';
    SIZE_t i;
    UINT32 j;

    if (ChValue == char_esc) {   /* Esc */
        AmbaShell_ResetKeyCode();
    }

    if (AmbaShell_CharSeqLen >= (sizeof(AmbaShell_CharSeqBuf) -1U)) {
        AmbaShell_ResetKeyCode();
    }

    AmbaShell_CharSeqBuf[AmbaShell_CharSeqLen] = ChValue;
    AmbaShell_CharSeqLen++;
    StringLen = AmbaShell_CharSeqLen;

    for (i = 0; i < NumKeyCode; i++) {
        if (StringLen < AmbaShell_EscapeCode[i].NumChar) {
            continue;
        }

        for (j = 0; j < StringLen; j++) {
            if (pString[j] != AmbaShell_EscapeCode[i].pSeq[j]) {
                break;
            }
        }

        if ((j == StringLen) && (pKeyCode != NULL)) {
            *pKeyCode = AmbaShell_EscapeCode[i].Code;
            AmbaShell_ResetKeyCode();
            RetVal = SHELL_ERR_SUCCESS;
        }
    }

    /* For non-escape sequence, we always treat it as valid key. */
    if ((RetVal == SHELL_ERR_KEY_CODE) && (StringLen == 1U) && (ChValue != char_esc) && (pKeyCode != NULL)) {
        *pKeyCode = AMBA_SHELL_NUM_KEY;
        AmbaShell_ResetKeyCode();
        RetVal = SHELL_ERR_SUCCESS;
    }

    return RetVal;
}
