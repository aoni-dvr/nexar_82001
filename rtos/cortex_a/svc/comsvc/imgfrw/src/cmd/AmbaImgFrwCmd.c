/**
 *  @file AmbaImgFrwCmd.c
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
 *  @details AmbaShell Image Framework Command
 *
 */

#include "AmbaTypes.h"
#include "AmbaKAL.h"
#include "AmbaShell.h"

#include "AmbaImgFrwCmdVar.h"
#include "AmbaImgFrwCmd_Def.h"

#include "AmbaImgFrwCmdSensor.h"
#include "AmbaImgFrwCmdSystem.h"

#include "AmbaImgFrwCmd.h"
#include "AmbaImgFrwCmd_Table.c"

#define OK_UL  ((UINT32) 0U)
#define NG_UL  ((UINT32) 1U)

static AMBA_KAL_MUTEX_t AmbaImgFrwCmd_UserListMutex;

/**
 *  Amba image framework command init
 *  @return error code
 */
UINT32 AmbaImgFrwCmd_Init(void)
{
    UINT32 RetCode = OK_UL;
    UINT32 FuncRetCode;

    AmbaImgFrwCmd_PrintEnable(F_PRINT_FLAG_MSG | F_PRINT_FLAG_ERR);

    FuncRetCode = AmbaKAL_MutexCreate(&AmbaImgFrwCmd_UserListMutex, NULL);
    if (FuncRetCode != KAL_ERR_NONE) {
        RetCode = NG_UL;
    }

    return RetCode;
}

/**
 *  Amba image framework command user add
 *  @param[in] pCmd pointer to command
 *  @return error code
 */
UINT32 AmbaImgFrwCmd_UserAdd(AMBA_IMG_FRW_CMD_s *pCmd)
{
    UINT32 RetCode = OK_UL;
    UINT32 FuncRetCode;
    AMBA_IMG_FRW_CMD_s *List = pAmbaImgFrwCmd_UserList;

    /* mutex tak e*/
    FuncRetCode = AmbaKAL_MutexTake(&AmbaImgFrwCmd_UserListMutex, AMBA_KAL_WAIT_FOREVER);
    if (FuncRetCode == KAL_ERR_NONE) {
        pCmd->Magic = 0xCafeU;
        if (List == NULL) {
            pAmbaImgFrwCmd_UserList = pCmd;
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
        FuncRetCode = AmbaKAL_MutexGive(&AmbaImgFrwCmd_UserListMutex);
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
 *  Amba shell image framework command entry
 *  @param[in] Argc argument count
 *  @param[in] Argv pointer to the argument string pointer
 *  @param[in] PrintFunc amba shell print function
 *  @note this function is intended for internal use only
 */
void ambsh_imgfrw(UINT32 Argc, char * const *Argv, AMBA_SHELL_PRINT_f PrintFunc)
{
    UINT32 CmdIdx = 0U;
    const AMBA_IMG_FRW_CMD_s *List = AmbaImgFrwCmd_List;

    if (Argc > 1U) {
        while ((List != NULL) && (List->Magic == AMBSH_MAGIC)) {
            /* valid */
            if (fvar_strcmp(Argv[1], List->pName) == OK_UL) {
                /* debug msg */
                AmbaImgFrwCmd_Print(F_PRINT_FLAG_DBG, "imgfrw cmd idx", CmdIdx);
                /* found */
                List->Proc(Argc-1U, &(Argv[1]), PrintFunc);
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
 *  Amba shell image framework command load
 *  @param[in] Argc argument count
 *  @param[in] Argv pointer to the argument string pointer
 *  @param[in] PrintFunc amba shell print function
 *  @note this function is intended for internal use only
 */
static void ambsh_imgfrw_load(UINT32 Argc, char * const *Argv, AMBA_SHELL_PRINT_f PrintFunc)
{
    UINT32 i, CmdNum;
    AMBA_IMG_FRW_CMD_s *List = AmbaImgFrwCmd_List;

    (void) Argc;
    (void) Argv;

    if (PrintFunc == NULL) {
        /* */
    }

    /* cmd num get */
    CmdNum = (UINT32) (sizeof(AmbaImgFrwCmd_List)/sizeof(AMBA_IMG_FRW_CMD_s));
    /* debug msg */
    AmbaImgFrwCmd_Print(F_PRINT_FLAG_MSG, "imgfrw cmd load", CmdNum);

    /* imgfrw cmd */
    for (i = 0U; i < (CmdNum-1U); i++) {
        if (List[i].Magic != AMBSH_MAGIC) {
            /* invalid */
            break;
        }
        /* link */
        List[i].pNext = &(List[i+1U]);
    }

    /* imgfrw user cmd link */
    if (i == (CmdNum - 1U)) {
        List[i].pNext = pAmbaImgFrwCmd_UserList;
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
        AmbaImgFrwCmd_Print(F_PRINT_FLAG_MSG, "imgfrw usr cmd load", CmdNum);
    }
}

/**
 *  Amba shell image framework command print enable
 *  @param[in] Argc argument count
 *  @param[in] Argv pointer to the argument string pointer
 *  @param[in] PrintFunc amba shell print function
 *  @note this function is intended for internal use only
 */
static void ambsh_imgfrw_prt_en(UINT32 Argc, char * const *Argv, AMBA_SHELL_PRINT_f PrintFunc)
{
    if (PrintFunc == NULL) {
        /* */
    }

    if (Argc > 1U) {
        UINT32 Flag;

        Flag = fvar_atou(Argv[1]);

        AmbaImgFrwCmd_PrintEnable(Flag);
        AmbaImgFrwCmd_PrintEx(F_PRINT_FLAG_MSG, "imgfrw print enable", Flag, 16U);
    } else {
        AmbaImgFrwCmd_PrintStr(F_PRINT_FLAG_MSG, "usage: [flag]");
    }
}

