/**
 *  @file AmbaMonFrwCmdMain.c
 *
 *  Copyright (c) [2020] Ambarella International LP
 *
 *  This file and its contents ("Software") are protected by intellectual
 *  property rights including, without limitation, U.S. and/or foreign
 *  copyrights. This Software is also the confidential and proprietary
 *  information of Ambarella International LP and its licensors. You may not use, reproduce,
 *  disclose, distribute, modify, or otherwise prepare derivative works of this
 *  Software or any portion thereof except pursuant to a signed license agreement
 *  or nondisclosure agreement with Ambarella International LP or its authorized affiliates.
 *  In the absence of such an agreement, you agree to promptly notify and return
 *  this Software to Ambarella International LP.
 *
 *  This file includes sample code and is only for internal testing and evaluation.  If you
 *  distribute this sample code (whether in source, object, or binary code form), it will be
 *  without any warranty or indemnity protection from Ambarella International LP or its affiliates.
 *
 *  THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
 *  INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF NON-INFRINGEMENT,
 *  MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 *  IN NO EVENT SHALL AMBARELLA INTERNATIONAL LP OR ITS AFFILIATES BE LIABLE FOR ANY DIRECT,
 *  INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 *  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 *  LOSS OF USE, DATA, OR PROFITS; COMPUTER FAILURE OR MALFUNCTION; OR BUSINESS
 *  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 *  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 *  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *  POSSIBILITY OF SUCH DAMAGE.
 *
 *  @details AmbaShell Monitor Framework Command Main
 *
 */

#include "AmbaTypes.h"
#include "AmbaKAL.h"
#include "AmbaShell.h"

#include "AmbaMonFrwCmd_Def.h"
#include "AmbaMonFrwCmd.h"
#include "AmbaMonFrwCmdMain.h"

#define OK_UL  ((UINT32) 0U)
#define NG_UL  ((UINT32) 1U)

/**
 *  Amba monitor framework command attach
 *  @return error code
 */
UINT32 AmbaMonFrwCmd_Attach(void)
{
    UINT32 RetCode = OK_UL;
    UINT32 FuncRetCode;
    static AMBA_SHELL_COMMAND_s ambsh_monfrw_cmd = {
        .pName = "monfrw",
        .MainFunc = ambsh_monfrw,
        .pNext = NULL
    };

    FuncRetCode = AmbaMonFrwCmd_Init();
    if (FuncRetCode == OK_UL) {
        FuncRetCode = AmbaShell_CommandRegister(&ambsh_monfrw_cmd);
        if (FuncRetCode != SHELL_ERR_SUCCESS) {
            RetCode = NG_UL;
        }
    } else {
        RetCode = NG_UL;
    }

    return RetCode;
}

/**
 *  Amba monitor framework command add
 *  @param[in] pCmd pointer to the command information
 *  @return error code
 */
UINT32 AmbaMonFrwCmd_Add(AMBA_MON_FRW_CMD_s *pCmd)
{
    return AmbaMonFrwCmd_UserAdd(pCmd);
}
