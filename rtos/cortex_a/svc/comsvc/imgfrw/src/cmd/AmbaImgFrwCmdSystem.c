/**
 *  @file AmbaImgFrwCmdSystem.c
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
 *  @details AmbaShell Image Framework Command System
 *
 */

#include "AmbaTypes.h"
#include "AmbaKAL.h"
#include "AmbaVIN_Def.h"
#include "AmbaShell.h"

#include "AmbaDSP_ImageFilter.h"

#include "AmbaImgChannel.h"
#include "AmbaImgSystem.h"

#include "AmbaImgFrwCmd_Def.h"
#include "AmbaImgFrwCmdVar.h"
#include "AmbaImgFrwCmdSystem.h"
#include "AmbaImgFrwCmdSystem_Table.c"

#define OK_UL  ((UINT32) 0U)
//#define NG_UL  ((UINT32) 1U)

/**
 *  @private
 *  Amba shell image framework system command entry
 *  @param[in] Argc argument count
 *  @param[in] Argv pointer to the argument string pointer
 *  @param[in] PrintFunc amba shell print function
 *  @note this function is intended for internal use only
 */
void ambsh_imgfrw_system(UINT32 Argc, char * const *Argv, AMBA_SHELL_PRINT_f PrintFunc)
{
    UINT32 CmdIdx = 0U;
    const AMBA_IMG_FRW_CMD_s *List = AmbaImgFrwCmdSystem_List;

    if (Argc > 1U) {
        while ((List != NULL) && (List->Magic == AMBSH_MAGIC)) {
            /* valid */
            if (fvar_strcmp(Argv[1], List->pName) == OK_UL) {
                /* debug msg */
                AmbaImgFrwCmd_Print(F_PRINT_FLAG_DBG, "imgfrw cmd system idx", CmdIdx);
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
 *  Amba shell image framework system command load
 *  @param[in] Argc argument count
 *  @param[in] Argv pointer to the argument string pointer
 *  @param[in] PrintFunc amba shell print function
 *  @note this function is intended for internal use only
 */
static void ambsh_imgfrw_system_load(UINT32 Argc, char * const *Argv, AMBA_SHELL_PRINT_f PrintFunc)
{
    UINT32 i, CmdNum;
    AMBA_IMG_FRW_CMD_s *List = AmbaImgFrwCmdSystem_List;

    (void) Argc;
    (void) Argv;

    if (PrintFunc == NULL) {
        /* */
    }

    /* cmd num get */
    CmdNum = (UINT32) (sizeof(AmbaImgFrwCmdSystem_List)/sizeof(AMBA_IMG_FRW_CMD_s));
    /* debug msg */
    AmbaImgFrwCmd_Print(F_PRINT_FLAG_MSG, "imgfrw cmd system load", CmdNum);

    for (i = 0U; i < (CmdNum-1U); i++) {
        if (List[i].Magic != AMBSH_MAGIC) {
            /* invalid */
            break;
        }
        /* link */
        List[i].pNext = &(List[i+1U]);
    }
}

/**
 *  Amba shell image framework system command debug enable
 *  @param[in] Argc argument count
 *  @param[in] Argv pointer to the argument string pointer
 *  @param[in] PrintFunc amba shell print function
 *  @note this function is intended for internal use only
 */
static void ambsh_imgfrw_system_dbg_en(UINT32 Argc, char * const *Argv, AMBA_SHELL_PRINT_f PrintFunc)
{
    if (PrintFunc == NULL) {
        /* */
    }

    if (Argc > 2U) {
        UINT32 FuncRetCode;
        UINT32 VinId;
        UINT32 Enable;

        VinId = fvar_atou(Argv[1]);
        Enable = fvar_atou(Argv[2]);

        if (VinId < AMBA_IMGFRW_NUM_VIN_CHANNEL) {
            AmbaImgFrwCmd_PrintEx2(F_PRINT_FLAG_MSG, "system debug enable", VinId, 10U, Enable, 16U);
            FuncRetCode = AmbaImgSystem_Debug(VinId, Enable);
            if (FuncRetCode != OK_UL) {
                AmbaImgFrwCmd_PrintEx2(F_PRINT_FLAG_ERR, "error: system debug enable", VinId, 10U, Enable, 16U);
            }
        }
    } else {
        AmbaImgFrwCmd_PrintStr(F_PRINT_FLAG_MSG, "usage: [vin_id] [enable]");
    }
}

