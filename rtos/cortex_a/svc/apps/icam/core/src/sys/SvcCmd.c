/**
*  @file SvcCmd.c
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
*  @details svc command functions
*
*/

#include "AmbaTypes.h"
#include "AmbaMisraFix.h"
#include "AmbaUtility.h"
#include "AmbaWrap.h"
#include "AmbaShell.h"

#include "SvcErrCode.h"
#include "SvcLog.h"
#include "SvcWrap.h"
#include "SvcCmd.h"

#define SVC_LOG_SHELL_CMD   "SCMD"

/**
* register of shell command
* @param [in] pCmd shell command
* @return 0-OK, 1-NG
*/
UINT32 SvcCmd_CommandRegister(const AMBA_SHELL_COMMAND_s *pCmd)
{
    static AMBA_SHELL_COMMAND_s SvcCmdList[SVC_CMD_MAX_ENTRY_NUM] GNU_SECTION_NOZEROINIT;
    static UINT32               SvcCmdNum = 0U;
    UINT32 RetVal = SVC_OK;

    if (pCmd == NULL) {
        RetVal = SVC_NG;
        SvcLog_NG(SVC_LOG_SHELL_CMD, "Failure to register shell command - input cmd should not null!", 0U, 0U);
    } else if ((SvcCmdNum + 1U) >= SVC_CMD_MAX_ENTRY_NUM) {
        RetVal = SVC_NG;
        SvcLog_NG(SVC_LOG_SHELL_CMD, "Failure to register shell command - cmd entry full!", 0U, 0U);
    } else {
        AMBA_SHELL_COMMAND_s *pSvcCmd;

        if (SvcCmdNum == 0U) {
            if (0U != AmbaWrap_memset(SvcCmdList, 0, sizeof(SvcCmdList))) {
                RetVal = SVC_NG;
                SvcLog_NG(SVC_LOG_SHELL_CMD, "Failure to register shell command - reset cmd list fail!", 0U, 0U);
            }
        }

        if (RetVal == SVC_OK) {
            UINT32 Idx;

            for (Idx = 0U; Idx < SvcCmdNum; Idx ++) {
                pSvcCmd = &(SvcCmdList[Idx]);

                if ((0 == SvcWrap_strcmp(pSvcCmd->pName, pCmd->pName)) ||
                    (pSvcCmd->MainFunc == pCmd->MainFunc)) {

                    RetVal = SVC_NG;
                    SVC_WRAP_PRINT "Failure to register shell command - the cmd - %s has been register!"
                        SVC_PRN_ARG_S SVC_LOG_SHELL_CMD
                        SVC_PRN_ARG_PROC SvcLog_NG
                        SVC_PRN_ARG_CSTR (pSvcCmd->pName) SVC_PRN_ARG_POST
                        SVC_PRN_ARG_E
                    break;
                }
            }
        }

        if (RetVal == SVC_OK) {
            const AMBA_SHELL_COMMAND_s *pShellCmdRoot = AmbaShell_CommandListGet();
            pSvcCmd = &(SvcCmdList[SvcCmdNum]);

            if (pShellCmdRoot != NULL) {
                UINT32 MaxCheckCnt = SVC_CMD_MAX_ENTRY_NUM + 10U;

                do {

                    if (pShellCmdRoot == pSvcCmd) {
                        RetVal = SVC_NG;
                        SvcLog_NG(SVC_LOG_SHELL_CMD, "Failure to register shell command - No.%d cmd entry is exist!!", SvcCmdNum, 0U);
                    } else {
                        pShellCmdRoot = pShellCmdRoot->pNext;
                    }

                    MaxCheckCnt --;
                    if (MaxCheckCnt == 0U) {
                        RetVal = SVC_NG;
                        SvcLog_NG(SVC_LOG_SHELL_CMD, "Failure to register shell command - Out-of check depth!!", 0U, 0U);
                    }

                    if (RetVal != SVC_OK) {
                        break;
                    }

                } while(pShellCmdRoot != NULL);
            }

            if (RetVal == SVC_OK) {
                if (0U != AmbaWrap_memcpy(pSvcCmd, pCmd, sizeof(AMBA_SHELL_COMMAND_s))) {
                    RetVal = SVC_NG;
                    SvcLog_NG(SVC_LOG_SHELL_CMD, "Failure to register shell command - copy cmd entry to list fail!", 0U, 0U);
                }
            }
        }

        if (RetVal == SVC_OK) {
            RetVal = AmbaShell_CommandRegister(&(SvcCmdList[SvcCmdNum]));
            if (RetVal != 0U) {
                SvcLog_NG(SVC_LOG_SHELL_CMD, "Failure to register shell command! ErrCode(0x%x)", RetVal, 0U);
                RetVal = SVC_NG;
            } else {
                SvcCmdNum ++;
                RetVal = SVC_OK;
            }
        }
    }

    return RetVal;
}
