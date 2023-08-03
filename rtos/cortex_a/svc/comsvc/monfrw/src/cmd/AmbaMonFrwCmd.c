/**
 *  @file AmbaMonFrwCmd.c
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
 *  @details AmbaShell Monitor Framework Command
 *
 */

#include "AmbaTypes.h"
#include "AmbaKAL.h"
#include "AmbaShell.h"

#include "AmbaMonFrwCmdVar.h"
#include "AmbaMonFrwCmd_Def.h"

#include "AmbaMonFrwCmd.h"
#include "AmbaMonFrwCmd_Table.c"

#define OK_UL  ((UINT32) 0U)
#define NG_UL  ((UINT32) 1U)

static AMBA_KAL_MUTEX_t AmbaMonFrwCmd_UserListMutex;

/**
 *  Amba monitor framework command init
 *  @return error code
 */
UINT32 AmbaMonFrwCmd_Init(void)
{
    UINT32 RetCode = OK_UL;
    UINT32 FuncRetCode;

    AmbaMonFrwCmd_PrintEnable(SF_PRINT_FLAG_MSG | SF_PRINT_FLAG_ERR);

    FuncRetCode = AmbaKAL_MutexCreate(&AmbaMonFrwCmd_UserListMutex, NULL);
    if (FuncRetCode != KAL_ERR_NONE) {
        RetCode = NG_UL;
    }

    return RetCode;
}

/**
 *  Amba monitor framework command user add
 *  @param[in] pCmd pointer to command
 *  @return error code
 */
UINT32 AmbaMonFrwCmd_UserAdd(AMBA_MON_FRW_CMD_s *pCmd)
{
    UINT32 RetCode = OK_UL;
    UINT32 FuncRetCode;
    AMBA_MON_FRW_CMD_s *List = pAmbaMonFrwCmd_UserList;

    /* mutex tak e*/
    FuncRetCode = AmbaKAL_MutexTake(&AmbaMonFrwCmd_UserListMutex, AMBA_KAL_WAIT_FOREVER);
    if (FuncRetCode == KAL_ERR_NONE) {
        pCmd->Magic = 0xCafeU;
        if (List == NULL) {
            pAmbaMonFrwCmd_UserList = pCmd;
        } else {
            /* last find */
            while (List->pNext != NULL) {
                /* next */
                List = List->pNext;
            }
            /* hook to tailer */
            List->pNext = pCmd;
        }
        /* end point */
        pCmd->pNext = NULL;
        /* mutex give */
        FuncRetCode = AmbaKAL_MutexGive(&AmbaMonFrwCmd_UserListMutex);
        if (FuncRetCode != KAL_ERR_NONE) {
            /* */
        }
    } else {
        /* mutex fail */
        RetCode = NG_UL;
    }

    return RetCode;
}

/**
 *  @private
 *  Amba shell monitor framework command entry
 *  @param[in] Argc argument count
 *  @param[in] Argv pointer to the argument string pointer
 *  @param[in] PrintFunc amba shell print function
 *  @note this function is intended for internal use only
 */
void ambsh_monfrw(UINT32 Argc, char * const *Argv, AMBA_SHELL_PRINT_f PrintFunc)
{
    UINT32 CmdIdx = 0U;
    const AMBA_MON_FRW_CMD_s *List = AmbaMonFrwCmd_List;

    if (Argc > 1U) {
        while ((List != NULL) && (List->Magic == AMBSH_MAGIC)) {
            /* valid */
            if (sfvar_strcmp(Argv[1], List->pName) == OK_UL) {
                /* debug msg */
                AmbaMonFrwCmd_Print(SF_PRINT_FLAG_DBG, "amba monfrw cmd idx", CmdIdx);
                /* found */
                List->Proc(Argc-1U, &(Argv[1U]), PrintFunc);
                break;
            } else {
                /* next */
                List = List->pNext;
            }
            /* cmd idx */
            CmdIdx++;
        }
    }
}

/**
 *  Amba shell monitor framework command load
 *  @param[in] Argc argument count
 *  @param[in] Argv pointer to the argument string pointer
 *  @param[in] PrintFunc amba shell print function
 *  @note this function is intended for internal use only
 */
static void ambsh_monfrw_load(UINT32 Argc, char * const *Argv, AMBA_SHELL_PRINT_f PrintFunc)
{
    UINT32 i, CmdNum;
    AMBA_MON_FRW_CMD_s *List = AmbaMonFrwCmd_List;

    (void) Argc;
    (void) Argv;

    if (PrintFunc == NULL) {
        /* */
    }

    /* cmd num get */
    CmdNum = (UINT32) (sizeof(AmbaMonFrwCmd_List)/sizeof(AMBA_MON_FRW_CMD_s));
    /* debug msg */
    AmbaMonFrwCmd_Print(SF_PRINT_FLAG_MSG, "amba monfrw cmd load", CmdNum);

    /* monfrw cmd */
    for (i = 0U; i < (CmdNum-1U); i++) {
        if (List[i].Magic != AMBSH_MAGIC) {
            /* invalid */
            break;
        }
        /* link */
        List[i].pNext = &(List[i+1U]);
    }

    /* monfrw user cmd link */
    if (i == (CmdNum - 1U)) {
        List[i].pNext = pAmbaMonFrwCmd_UserList;
        List = List[i].pNext;
        CmdNum = 0U;
        while (List != NULL) {
            if (List->Magic != AMBSH_MAGIC) {
                /* invalid */
                break;
            } else {
                /* next */
                List = List->pNext;
                CmdNum++;
            }
        }
        /* debug msg */
        AmbaMonFrwCmd_Print(SF_PRINT_FLAG_MSG, "amba monfrw usr cmd load", CmdNum);
    }
}

/**
 *  Amba shell monitor framework command print enable
 *  @param[in] Argc argument count
 *  @param[in] Argv pointer to the argument string pointer
 *  @param[in] PrintFunc amba shell print function
 *  @note this function is intended for internal use only
 */
static void ambsh_monfrw_prt_en(UINT32 Argc, char * const *Argv, AMBA_SHELL_PRINT_f PrintFunc)
{
    if (PrintFunc == NULL) {
        /* */
    }

    if (Argc > 1U) {
        UINT32 Flag;

        Flag = sfvar_atou(Argv[1]);

        AmbaMonFrwCmd_PrintEnable(Flag);
        AmbaMonFrwCmd_PrintEx(SF_PRINT_FLAG_MSG, "amba monfrw print enable", Flag, 16U);
    } else {
        AmbaMonFrwCmd_PrintStr(SF_PRINT_FLAG_MSG, "usage: [flag]");
    }
}

