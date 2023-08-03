/**
 *  @file AmbaImgFrwCmdApp.c
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
 *  @details AmbaShell Image Framework Command App
 *
 */

#include "AmbaTypes.h"
#include "AmbaKAL.h"
#include "AmbaVIN_Def.h"
#include "AmbaShell.h"

#include "AmbaDSP_ImageFilter.h"

#include "AmbaImg_External_CtrlFunc.h"

#include "AmbaImgFramework.h"
#include "AmbaImgMain.h"
#include "AmbaImgMain_Internal.h"

#include "AmbaImgMainStatistics.h"
#include "AmbaImgMainSync.h"
#include "AmbaImgMainAe.h"
#include "AmbaImgMainAwb.h"
#include "AmbaImgMainAdj.h"
#include "AmbaImgMainAf.h"
#include "AmbaImgMainAvm.h"
#include "AmbaImgMainFov.h"
#include "AmbaImgMainEis.h"

#include "AmbaImgFrwCmdApp_Def.h"
#include "AmbaImgFrwCmdApp.h"
#include "AmbaImgFrwCmdApp_Table.c"

#define OK_UL  ((UINT32) 0U)
#define NG_UL  ((UINT32) 1U)

/**
 *  Amba image framework app command attach
 *  @return error code
 */
UINT32 AmbaImgFrwCmdApp_Attach(void)
{
    UINT32 RetCode = OK_UL;
    UINT32 FuncRetCode;
    static AMBA_IMG_FRW_CMD_s AmbaImgFrwCmdApp = { AMBSH_MAGIC, "app", ambsh_imgfrw_app, NULL };

    FuncRetCode = AmbaImgFrwCmd_Add(&AmbaImgFrwCmdApp);
    if (FuncRetCode != OK_UL) {
        RetCode = NG_UL;
    }

    return RetCode;
}

/**
 *  Amba shell image framework app command entry
 *  @param[in] Argc argument count
 *  @param[in] Argv pointer to the argument string pointer
 *  @param[in] PrintFunc amba shell print function
 *  @note this function is intended for internal use only
 */
static void ambsh_imgfrw_app(UINT32 Argc, char * const *Argv, AMBA_SHELL_PRINT_f PrintFunc)
{
    UINT32 CmdIdx = 0U;
    const AMBA_IMG_FRW_CMD_s *List = AmbaImgFrwCmdApp_List;

    if (Argc > 1U) {
        while ((List != NULL) && (List->Magic == AMBSH_MAGIC)) {
            /* valid */
            if (fvar_strcmp(Argv[1], List->pName) == OK_UL) {
                /* debug msg */
                AmbaImgFrwCmd_Print(F_PRINT_FLAG_DBG, "imgfrw cmd app idx", CmdIdx);
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
 *  Amba shell image framework app command load
 *  @param[in] Argc argument count
 *  @param[in] Argv pointer to the argument string pointer
 *  @param[in] PrintFunc amba shell print function
 *  @note this function is intended for internal use only
 */
static void ambsh_imgfrw_app_load(UINT32 Argc, char * const *Argv, AMBA_SHELL_PRINT_f PrintFunc)
{
    UINT32 i, CmdNum;
    AMBA_IMG_FRW_CMD_s *List = AmbaImgFrwCmdApp_List;

    (void) Argc;
    (void) Argv;

    if (PrintFunc == NULL) {
        /* */
    }

    /* cmd num get */
    CmdNum = (UINT32) (sizeof(AmbaImgFrwCmdApp_List)/sizeof(AMBA_IMG_FRW_CMD_s));
    /* debug msg */
    AmbaImgFrwCmd_Print(F_PRINT_FLAG_MSG, "imgfrw cmd app load", CmdNum);

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
 *  Amba shell image framework app command print enable
 *  @param[in] Argc argument count
 *  @param[in] Argv pointer to the argument string pointer
 *  @param[in] PrintFunc amba shell print function
 *  @note this function is intended for internal use only
 */
static void ambsh_imgfrw_app_prt_en(UINT32 Argc, char * const *Argv, AMBA_SHELL_PRINT_f PrintFunc)
{
    if (PrintFunc == NULL) {
        /* */
    }

    if (Argc > 1U) {
        UINT32 Flag;

        Flag = fvar_atou(Argv[1]);

        AmbaImgFrwCmd_PrintEx(F_PRINT_FLAG_MSG, "image print enable", Flag, 16U);
        AmbaImgPrint_Enable(Flag);
    } else {
        AmbaImgFrwCmd_PrintStr(F_PRINT_FLAG_MSG, "usage: [flag]");
    }
}

/**
 *  Amba shell image framework app command debug enable
 *  @param[in] Argc argument count
 *  @param[in] Argv pointer to the argument string pointer
 *  @param[in] PrintFunc amba shell print function
 *  @note this function is intended for internal use only
 */
static void ambsh_imgfrw_app_dbg_en(UINT32 Argc, char * const *Argv, AMBA_SHELL_PRINT_f PrintFunc)
{
    if (PrintFunc == NULL) {
        /* */
    }

    if (Argc > 3U) {
        UINT32 i, j;
        UINT32 VinFlag;
        UINT32 ChainFlag;
        UINT32 Count;
        UINT32 FovFlag = 0U;
        UINT32 EisFlag = 0U;

        VinFlag = fvar_atou(Argv[1]);
        ChainFlag = fvar_atou(Argv[2]);
        Count = fvar_atou(Argv[3]);

        AmbaImgFrwCmd_PrintEx(F_PRINT_FLAG_MSG, "image debug vin flag", VinFlag, 16U);
        AmbaImgFrwCmd_PrintEx(F_PRINT_FLAG_MSG, "image debug chain flag", ChainFlag, 16U);
        AmbaImgFrwCmd_Print(F_PRINT_FLAG_MSG, "image debug count", Count);

        if (Argc > 4U) {
            FovFlag = fvar_atou(Argv[4]);
            AmbaImgFrwCmd_PrintEx(F_PRINT_FLAG_MSG, "image debug fov flag", FovFlag, 16U);
        }
        if (Argc > 5U) {
            EisFlag = fvar_atou(Argv[5]);
            AmbaImgFrwCmd_PrintEx(F_PRINT_FLAG_MSG, "image debug eis flag", EisFlag, 16U);
        }

        /* disable */
        AmbaImgMain_DebugCount = 0U;
        /* still disable */
        AmbaImgMain_DebugStillTiming = 0U;
        /* statistics reset */
        AmbaImgMainStatistics_DebugCnt = 0U;
        for (i = 0U; i < AMBA_IMG_NUM_VIN_CHANNEL; i++) {
            if ((VinFlag & (((UINT32) 1U) << i)) > 0U) {
                /* sync reset */
                AmbaImgMainSync_DebugCount[i] = 0U;
                for (j = 0U; j < AMBA_IMG_NUM_VIN_ALGO; j++) {
                    if ((ChainFlag & (((UINT32) 1U) << j)) > 0U) {
                        /* aaa reset */
                        AmbaImgMainAe_DebugCount[i][j] = 0U;
                        AmbaImgMainAwb_DebugCount[i][j] = 0U;
                        AmbaImgMainAdj_DebugCount[i][j] = 0U;
                        AmbaImgMainAf_DebugCount[i][j] = 0U;
                        /* query reset */
                        AmbaImgMainAe_QueryCount[i][j] = 0U;
                    }
                }
            }
        }
        /* avm reset */
        AmbaImgMainAvm_DebugCount = 0U;
        /* fov reset */
        if (Argc > 4U) {
            for (i = 0U; i <= AmbaImgMainFov_LastIndex; i++) {
                if ((FovFlag & (((UINT32) 1U) << i)) > 0U) {
                    AmbaImgMainFov_DebugCount[i] = 0U;
                }
            }
        }
        /* eis reset */
        if (Argc > 5U) {
            for (i = 0U; i <= AmbaImgMainEis_LastIndex; i++) {
                if ((EisFlag & (((UINT32) 1U) << i)) > 0U) {
                    AmbaImgMainEis_DebugCount[i] = 0U;
                }
            }
        }
        /* enable/disable */
        AmbaImgMain_DebugCount = Count & 0x7FFFFFFFU;
        AmbaImgMain_DebugStillTiming = Count >> 31U;
    } else {
        AmbaImgFrwCmd_PrintStr(F_PRINT_FLAG_MSG, "usage: [vin_flag][chain_flag][count]");
    }
}

/**
 *  Amba shell image framework app command ae lock
 *  @param[in] Argc argument count
 *  @param[in] Argv pointer to the argument string pointer
 *  @param[in] PrintFunc amba shell print function
 *  @note this function is intended for internal use only
 */
static void ambsh_imgfrw_app_ae_lock(UINT32 Argc, char * const *Argv, AMBA_SHELL_PRINT_f PrintFunc)
{
    UINT32 FuncRetCode;

    if (PrintFunc == NULL) {
        /* */
    }

    if (Argc > 1U) {
        UINT32 VinId;

        VinId = fvar_atou(Argv[1]);

        AmbaImgFrwCmd_Print(F_PRINT_FLAG_MSG, "image ae lock", VinId);
        FuncRetCode = AmbaImgMainAe_Lock(VinId);
        if (FuncRetCode != OK_UL) {
            /* */
        }
    } else {
        AmbaImgFrwCmd_PrintStr(F_PRINT_FLAG_MSG, "usage: [vin_id]");
    }
}

/**
 *  Amba shell image framework app command ae lock wait
 *  @param[in] Argc argument count
 *  @param[in] Argv pointer to the argument string pointer
 *  @param[in] PrintFunc amba shell print function
 *  @note this function is intended for internal use only
 */
static void ambsh_imgfrw_app_ae_lock_wait(UINT32 Argc, char * const *Argv, AMBA_SHELL_PRINT_f PrintFunc)
{
    UINT32 FuncRetCode;

    if (PrintFunc == NULL) {
        /* */
    }

    if (Argc > 1U) {
        UINT32 VinId;

        VinId = fvar_atou(Argv[1]);

        AmbaImgFrwCmd_Print(F_PRINT_FLAG_MSG, "image ae lock wait", VinId);
        FuncRetCode = AmbaImgMainAe_LockWait(VinId);
        if (FuncRetCode != OK_UL) {
            /* */
        }
    } else {
        AmbaImgFrwCmd_PrintStr(F_PRINT_FLAG_MSG, "usage: [vin_id]");
    }
}

/**
 *  Amba shell image framework app command ae unlock
 *  @param[in] Argc argument count
 *  @param[in] Argv pointer to the argument string pointer
 *  @param[in] PrintFunc amba shell print function
 *  @note this function is intended for internal use only
 */
static void ambsh_imgfrw_app_ae_unlock(UINT32 Argc, char * const *Argv, AMBA_SHELL_PRINT_f PrintFunc)
{
    UINT32 FuncRetCode;

    if (PrintFunc == NULL) {
        /* */
    }

    if (Argc > 1U) {
        UINT32 VinId;

        VinId = fvar_atou(Argv[1]);

        AmbaImgFrwCmd_Print(F_PRINT_FLAG_MSG, "image ae unlock", VinId);
        FuncRetCode = AmbaImgMainAe_Unlock(VinId);
        if (FuncRetCode != OK_UL) {
            /* */
        }
    } else {
        AmbaImgFrwCmd_PrintStr(F_PRINT_FLAG_MSG, "usage: [vin_id]");
    }
}

/**
 *  Amba shell image framework app command awb lock
 *  @param[in] Argc argument count
 *  @param[in] Argv pointer to the argument string pointer
 *  @param[in] PrintFunc amba shell print function
 *  @note this function is intended for internal use only
 */
static void ambsh_imgfrw_app_awb_lock(UINT32 Argc, char * const *Argv, AMBA_SHELL_PRINT_f PrintFunc)
{
    UINT32 FuncRetCode;

    if (PrintFunc == NULL) {
        /* */
    }

    if (Argc > 1U) {
        UINT32 VinId;

        VinId = fvar_atou(Argv[1]);

        AmbaImgFrwCmd_Print(F_PRINT_FLAG_MSG, "image awb lock", VinId);
        FuncRetCode = AmbaImgMainAwb_Lock(VinId);
        if (FuncRetCode != OK_UL) {
            /* */
        }
    } else {
        AmbaImgFrwCmd_PrintStr(F_PRINT_FLAG_MSG, "usage: [vin_id]");
    }
}

/**
 *  Amba shell image framework app command awb lock wait
 *  @param[in] Argc argument count
 *  @param[in] Argv pointer to the argument string pointer
 *  @param[in] PrintFunc amba shell print function
 *  @note this function is intended for internal use only
 */
static void ambsh_imgfrw_app_awb_lock_wait(UINT32 Argc, char * const *Argv, AMBA_SHELL_PRINT_f PrintFunc)
{
    UINT32 FuncRetCode;

    if (PrintFunc == NULL) {
        /* */
    }

    if (Argc > 1U) {
        UINT32 VinId;

        VinId = fvar_atou(Argv[1]);

        AmbaImgFrwCmd_Print(F_PRINT_FLAG_MSG, "image awb lock wait", VinId);
        FuncRetCode = AmbaImgMainAwb_LockWait(VinId);
        if (FuncRetCode != OK_UL) {
            /* */
        }
    } else {
        AmbaImgFrwCmd_PrintStr(F_PRINT_FLAG_MSG, "usage: [vin_id]");
    }
}

/**
 *  Amba shell image framework app command awb unlock
 *  @param[in] Argc argument count
 *  @param[in] Argv pointer to the argument string pointer
 *  @param[in] PrintFunc amba shell print function
 *  @note this function is intended for internal use only
 */
static void ambsh_imgfrw_app_awb_unlock(UINT32 Argc, char * const *Argv, AMBA_SHELL_PRINT_f PrintFunc)
{
    UINT32 FuncRetCode;

    if (PrintFunc == NULL) {
        /* */
    }

    if (Argc > 1U) {
        UINT32 VinId;

        VinId = fvar_atou(Argv[1]);

        AmbaImgFrwCmd_Print(F_PRINT_FLAG_MSG, "image awb unlock", VinId);
        FuncRetCode = AmbaImgMainAwb_Unlock(VinId);
        if (FuncRetCode != OK_UL) {
            /* */
        }
    } else {
        AmbaImgFrwCmd_PrintStr(F_PRINT_FLAG_MSG, "usage: [vin_id]");
    }
}

/**
 *  Amba shell image framework app command adj lock
 *  @param[in] Argc argument count
 *  @param[in] Argv pointer to the argument string pointer
 *  @param[in] PrintFunc amba shell print function
 *  @note this function is intended for internal use only
 */
static void ambsh_imgfrw_app_adj_lock(UINT32 Argc, char * const *Argv, AMBA_SHELL_PRINT_f PrintFunc)
{
    UINT32 FuncRetCode;

    if (PrintFunc == NULL) {
        /* */
    }

    if (Argc > 1U) {
        UINT32 VinId;

        VinId = fvar_atou(Argv[1]);

        AmbaImgFrwCmd_Print(F_PRINT_FLAG_MSG, "image adj lock", VinId);
        FuncRetCode = AmbaImgMainAdj_Lock(VinId);
        if (FuncRetCode != OK_UL) {
            /* */
        }
    } else {
        AmbaImgFrwCmd_PrintStr(F_PRINT_FLAG_MSG, "usage: [vin_id]");
    }
}

/**
 *  Amba shell image framework app command adj lock wait
 *  @param[in] Argc argument count
 *  @param[in] Argv pointer to the argument string pointer
 *  @param[in] PrintFunc amba shell print function
 *  @note this function is intended for internal use only
 */
static void ambsh_imgfrw_app_adj_lock_wait(UINT32 Argc, char * const *Argv, AMBA_SHELL_PRINT_f PrintFunc)
{
    UINT32 FuncRetCode;

    if (PrintFunc == NULL) {
        /* */
    }

    if (Argc > 1U) {
        UINT32 VinId;

        VinId = fvar_atou(Argv[1]);

        AmbaImgFrwCmd_Print(F_PRINT_FLAG_MSG, "image adj lock wait", VinId);
        FuncRetCode = AmbaImgMainAdj_LockWait(VinId);
        if (FuncRetCode != OK_UL) {
            /* */
        }
    } else {
        AmbaImgFrwCmd_PrintStr(F_PRINT_FLAG_MSG, "usage: [vin_id]");
    }
}

/**
 *  Amba shell image framework app command adj unlock
 *  @param[in] Argc argument count
 *  @param[in] Argv pointer to the argument string pointer
 *  @param[in] PrintFunc amba shell print function
 *  @note this function is intended for internal use only
 */
static void ambsh_imgfrw_app_adj_unlock(UINT32 Argc, char * const *Argv, AMBA_SHELL_PRINT_f PrintFunc)
{
    UINT32 FuncRetCode;

    if (PrintFunc == NULL) {
        /* */
    }

    if (Argc > 1U) {
        UINT32 VinId;

        VinId = fvar_atou(Argv[1]);

        AmbaImgFrwCmd_Print(F_PRINT_FLAG_MSG, "image adj unlock", VinId);
        FuncRetCode = AmbaImgMainAdj_Unlock(VinId);
        if (FuncRetCode != OK_UL) {
            /* */
        }
    } else {
        AmbaImgFrwCmd_PrintStr(F_PRINT_FLAG_MSG, "usage: [vin_id]");
    }
}
#ifdef CONFIG_BUILD_IMGFRW_SMC
/**
 *  Amba shell image framework app command ae smc on
 *  @param[in] Argc argument count
 *  @param[in] Argv pointer to the argument string pointer
 *  @param[in] PrintFunc amba shell print function
 *  @note this function is intended for internal use only
 */
static void ambsh_imgfrw_app_ae_smc_on(UINT32 Argc, char * const *Argv, AMBA_SHELL_PRINT_f PrintFunc)
{
    UINT32 FuncRetCode;

    if (PrintFunc == NULL) {
        /* */
    }

    if (Argc > 1U) {
        UINT32 VinId;

        VinId = fvar_atou(Argv[1]);

        AmbaImgFrwCmd_Print(F_PRINT_FLAG_MSG, "image ae smc on", VinId);
        FuncRetCode = AmbaImgMainAe_SmcOn(VinId);
        if (FuncRetCode != OK_UL) {
            /* */
        }
    } else {
        AmbaImgFrwCmd_PrintStr(F_PRINT_FLAG_MSG, "usage: [vin_id]");
    }
}

/**
 *  Amba shell image framework app command ae smc off
 *  @param[in] Argc argument count
 *  @param[in] Argv pointer to the argument string pointer
 *  @param[in] PrintFunc amba shell print function
 *  @note this function is intended for internal use only
 */
static void ambsh_imgfrw_app_ae_smc_off(UINT32 Argc, char * const *Argv, AMBA_SHELL_PRINT_f PrintFunc)
{
    UINT32 FuncRetCode;

    if (PrintFunc == NULL) {
        /* */
    }

    if (Argc > 1U) {
        UINT32 VinId;

        VinId = fvar_atou(Argv[1]);

        AmbaImgFrwCmd_Print(F_PRINT_FLAG_MSG, "image ae smc off", VinId);
        FuncRetCode = AmbaImgMainAe_SmcOff(VinId);
        if (FuncRetCode != OK_UL) {
            /* */
        }
    } else {
        AmbaImgFrwCmd_PrintStr(F_PRINT_FLAG_MSG, "usage: [vin_id]");
    }
}
#endif
/**
 *  Amba shell image framework app command ae stable
 *  @param[in] Argc argument count
 *  @param[in] Argv pointer to the argument string pointer
 *  @param[in] PrintFunc amba shell print function
 *  @note this function is intended for internal use only
 */
static void ambsh_imgfrw_app_ae_stable_wait(UINT32 Argc, char * const *Argv, AMBA_SHELL_PRINT_f PrintFunc)
{
    UINT32 FuncRetCode;

    if (PrintFunc == NULL) {
        /* */
    }

    if (Argc > 3U) {
        UINT32 VinId;
        UINT32 ChainId;
        UINT32 Timeout;

        VinId = fvar_atou(Argv[1]);
        ChainId = fvar_atou(Argv[2]);
        Timeout = fvar_atou(Argv[3]);

        AmbaImgFrwCmd_Print2(F_PRINT_FLAG_MSG, "ae stable wait...", VinId, ChainId);
        AmbaImgFrwCmd_PrintEx(F_PRINT_FLAG_MSG, "timeout", Timeout, 16U);
        FuncRetCode = AmbaImgMainAe_StableWait(VinId, ChainId, Timeout);
        if (FuncRetCode == OK_UL) {
            AmbaImgFrwCmd_Print2(F_PRINT_FLAG_MSG, "ae stable", VinId, ChainId);
        } else {
            AmbaImgFrwCmd_Print2(F_PRINT_FLAG_MSG, "ae stable wait fail", VinId, ChainId);
        }
    } else {
        AmbaImgFrwCmd_PrintStr(F_PRINT_FLAG_MSG, "usage: [vin id][chain id][timeout]");
    }
}

/**
 *  Amba shell image framework app command statistics mask
 *  @param[in] Argc argument count
 *  @param[in] Argv pointer to the argument string pointer
 *  @param[in] PrintFunc amba shell print function
 *  @note this function is intended for internal use only
 */
static void ambsh_imgfrw_app_stat_mask(UINT32 Argc, char * const *Argv, AMBA_SHELL_PRINT_f PrintFunc)
{
    if (PrintFunc == NULL) {
        /* */
    }

    if (Argc > 1U) {
        UINT32 Mask;

        Mask = fvar_atou(Argv[1]);

        AmbaImgFrwCmd_PrintEx(F_PRINT_FLAG_MSG, "image statistics mask", Mask, 16U);
        AmbaImgMainStatistics_MaskSet(Mask);
    } else {
        AmbaImgFrwCmd_PrintStr(F_PRINT_FLAG_MSG, "usage: [mask]");
    }
}

/**
 *  Amba shell image framework app command statistics inter
 *  @param[in] Argc argument count
 *  @param[in] Argv pointer to the argument string pointer
 *  @param[in] PrintFunc amba shell print function
 *  @note this function is intended for internal use only
 */
static void ambsh_imgfrw_app_stat_inter(UINT32 Argc, char * const *Argv, AMBA_SHELL_PRINT_f PrintFunc)
{
    UINT32 FuncRetCode;

    if (PrintFunc == NULL) {
        /* */
    }

    if (Argc > 2U) {
        UINT32 VinId;
        UINT32 Enable;

        VinId = fvar_atou(Argv[1]);
        Enable = fvar_atou(Argv[2]);

        AmbaImgFrwCmd_PrintEx2(F_PRINT_FLAG_MSG, "image statistics inter", VinId, 10U, Enable, 16U);
        FuncRetCode = AmbaImgStatistics_Inter(VinId, Enable);
        if (FuncRetCode != OK_UL) {
            /* */
        }
    } else {
        AmbaImgFrwCmd_PrintStr(F_PRINT_FLAG_MSG, "usage: [vin id][enable]");
    }
}

/**
 *  Amba shell image framework app command sync enable
 *  @param[in] Argc argument count
 *  @param[in] Argv pointer to the argument string pointer
 *  @param[in] PrintFunc amba shell print function
 *  @note this function is intended for internal use only
 */
static void ambsh_imgfrw_app_sync_en(UINT32 Argc, char * const *Argv, AMBA_SHELL_PRINT_f PrintFunc)
{
    if (PrintFunc == NULL) {
        /* */
    }

    if (Argc > 2U) {
        UINT32 VinId;
        UINT32 Enable;

        VinId = fvar_atou(Argv[1]);
        Enable = fvar_atou(Argv[2]);

        AmbaImgFrwCmd_PrintEx2(F_PRINT_FLAG_MSG, "image sync enable", VinId, 10U, Enable, 16U);
        AmbaImgMainSync_Enable(VinId, Enable);
    } else {
        AmbaImgFrwCmd_PrintStr(F_PRINT_FLAG_MSG, "usage: [vin id][flag]");
    }
}

/**
 *  Amba shell image framework app command sync margin
 *  @param[in] Argc argument count
 *  @param[in] Argv pointer to the argument string pointer
 *  @param[in] PrintFunc amba shell print function
 *  @note this function is intended for internal use only
 */
static void ambsh_imgfrw_app_sync_margin(UINT32 Argc, char * const *Argv, AMBA_SHELL_PRINT_f PrintFunc)
{
    if (PrintFunc == NULL) {
        /* */
    }

    if (Argc > 3U) {
        UINT32 VinId;
        UINT32 Margin;
        UINT32 Mode;

        VinId = fvar_atou(Argv[1]);
        Margin = fvar_atou(Argv[2]);
        Mode = fvar_atou(Argv[3]);

        AmbaImgFrwCmd_PrintEx2(F_PRINT_FLAG_MSG, "image sync margin", VinId, 10U, Margin, 10U);
        AmbaImgFrwCmd_PrintEx(F_PRINT_FLAG_MSG, "margin mode", Mode, 16U);
        AmbaImgMainSync_Margin(VinId, Margin, Mode);
    } else {
        AmbaImgFrwCmd_PrintStr(F_PRINT_FLAG_MSG, "usage: [vin id][margin][mode]");
    }
}
#ifndef CONFIG_BUILD_IMGFRW_EIS
/**
 *  Amba shell image framework app command eis dummy parameters
 *  @param[in] Argc argument count
 *  @param[in] Argv pointer to the argument string pointer
 *  @param[in] PrintFunc amba shell print function
 *  @note this function is intended for internal use only
 */
static void ambsh_imgfrw_app_eis_dmy_param(UINT32 Argc, char * const *Argv, AMBA_SHELL_PRINT_f PrintFunc)
{
    UINT32 FuncRetCode;

    if (PrintFunc == NULL) {
        /* */
    }

    if (Argc > 10U) {
        UINT32 EisIdx;
        AMBA_IMG_MAIN_EIS_PARAM_s DummyEisParam = {0};
        /* eis index */
        EisIdx = fvar_atou(Argv[1]);
        /* dzoom */
        DummyEisParam.DZoom.Factor = fvar_atou(Argv[2]);
        DummyEisParam.DZoom.ShiftXFactor = fvar_atou(Argv[3]);
        DummyEisParam.DZoom.ShiftYFactor = fvar_atou(Argv[4]);
        DummyEisParam.DZoom.ShiftXDir = fvar_atoi(Argv[5]);
        DummyEisParam.DZoom.ShiftYDir = fvar_atoi(Argv[6]);
        /* dummy range */
        DummyEisParam.DummyRange.XFactor = fvar_atou(Argv[7]);
        DummyEisParam.DummyRange.YFactor = fvar_atou(Argv[8]);
        /* warp shift */
        DummyEisParam.WarpShift.XFactor = fvar_atou(Argv[9]);
        DummyEisParam.WarpShift.YFactor = fvar_atou(Argv[10]);

        if (EisIdx < AMBA_IMG_NUM_EIS_CHANNEL) {
            char Str[11];

            AmbaImgFrwCmd_Print(F_PRINT_FLAG_MSG, "image eis dmy param", EisIdx);
            AmbaImgFrwCmd_Print(F_PRINT_FLAG_MSG, "dzoom factor", DummyEisParam.DZoom.Factor);
            AmbaImgFrwCmd_Print(F_PRINT_FLAG_MSG, "dzoom shift x factor", DummyEisParam.DZoom.ShiftXFactor);
            AmbaImgFrwCmd_Print(F_PRINT_FLAG_MSG, "dzoom shift y factor", DummyEisParam.DZoom.ShiftYFactor);
            fvar_itoa(DummyEisParam.DZoom.ShiftXDir, Str, 10U, 8U, (UINT32) FVAR_LEADING_NONE);
            AmbaImgFrwCmd_PrintStr(F_PRINT_FLAG_MSG, Str);
            fvar_itoa(DummyEisParam.DZoom.ShiftYDir, Str, 10U, 8U, (UINT32) FVAR_LEADING_NONE);
            AmbaImgFrwCmd_PrintStr(F_PRINT_FLAG_MSG, Str);
            AmbaImgFrwCmd_Print(F_PRINT_FLAG_MSG, "dummy range x factor", DummyEisParam.DummyRange.XFactor);
            AmbaImgFrwCmd_Print(F_PRINT_FLAG_MSG, "dummy range y factor", DummyEisParam.DummyRange.YFactor);
            AmbaImgFrwCmd_Print(F_PRINT_FLAG_MSG, "warp shift x factor", DummyEisParam.WarpShift.XFactor);
            AmbaImgFrwCmd_Print(F_PRINT_FLAG_MSG, "warp shift y factor", DummyEisParam.WarpShift.YFactor);

            FuncRetCode = AmbaImgMainEis_ParamUpdatePut(EisIdx, &DummyEisParam);
            if (FuncRetCode != OK_UL) {
                /* */
            }
        }
    } else {
        AmbaImgFrwCmd_PrintStr(F_PRINT_FLAG_MSG, "usage: [eis idex][dzoom factor][dzoom shift x factor][dzoom shift y factor][dzoom shift x dir][dzoom shift y dir][dummy range x factor][dummy range y factor][warp shift x factor][warp shift y factor]");
    }
}
/**
 *  Amba shell image framework app command eis dummy manual shift
 *  @param[in] Argc argument count
 *  @param[in] Argv pointer to the argument string pointer
 *  @param[in] PrintFunc amba shell print function
 *  @note this function is intended for internal use only
 */
static void ambsh_imgfrw_app_eis_dmy_ms(UINT32 Argc, char * const *Argv, AMBA_SHELL_PRINT_f PrintFunc)
{
    UINT32 FuncRetCode;

    if (PrintFunc == NULL) {
        /* */
    }

    if (Argc > 2U) {
        UINT32 EisIdx;
        AMBA_IMG_MAIN_EIS_WARP_SHIFT_s DummyWarpShift = {0};

        EisIdx = fvar_atou(Argv[1]);
        DummyWarpShift.Mode = fvar_atou(Argv[2]);

        if ((DummyWarpShift.Mode == IMG_EIS_WARP_SHIFT_MANUAL) && (Argc > 4U)) {
            DummyWarpShift.Vector.X = fvar_atoi(Argv[3]);
            DummyWarpShift.Vector.Y = fvar_atoi(Argv[4]);
        }

        if ((DummyWarpShift.Mode == IMG_EIS_WARP_SHIFT_LINE) && (Argc > 8U)) {
            DummyWarpShift.StepFactor = fvar_atoi(Argv[3]);
            DummyWarpShift.AccumulatedFactor = fvar_atoi(Argv[4]);
            DummyWarpShift.Point[0].X = fvar_atoi(Argv[5]);
            DummyWarpShift.Point[0].Y = fvar_atoi(Argv[6]);
            DummyWarpShift.Point[1].X = fvar_atoi(Argv[7]);
            DummyWarpShift.Point[1].Y = fvar_atoi(Argv[8]);
        }

        if (EisIdx < AMBA_IMG_NUM_EIS_CHANNEL) {
            char Str[11];

            AmbaImgFrwCmd_Print2(F_PRINT_FLAG_MSG, "image eis dmy ms", EisIdx, DummyWarpShift.Mode);
            fvar_itoa(DummyWarpShift.Vector.X, Str, 10U, 8U, (UINT32) FVAR_LEADING_NONE);
            AmbaImgFrwCmd_PrintStr(F_PRINT_FLAG_MSG, Str);
            fvar_itoa(DummyWarpShift.Vector.Y, Str, 10U, 8U, (UINT32) FVAR_LEADING_NONE);
            AmbaImgFrwCmd_PrintStr(F_PRINT_FLAG_MSG, Str);
            fvar_itoa(DummyWarpShift.StepFactor, Str, 10U, 8U, (UINT32) FVAR_LEADING_NONE);
            AmbaImgFrwCmd_PrintStr(F_PRINT_FLAG_MSG, Str);
            fvar_itoa(DummyWarpShift.AccumulatedFactor, Str, 10U, 8U, (UINT32) FVAR_LEADING_NONE);
            AmbaImgFrwCmd_PrintStr(F_PRINT_FLAG_MSG, Str);
            fvar_itoa(DummyWarpShift.Point[0].X, Str, 10U, 8U, (UINT32) FVAR_LEADING_NONE);
            AmbaImgFrwCmd_PrintStr(F_PRINT_FLAG_MSG, Str);
            fvar_itoa(DummyWarpShift.Point[0].Y, Str, 10U, 8U, (UINT32) FVAR_LEADING_NONE);
            AmbaImgFrwCmd_PrintStr(F_PRINT_FLAG_MSG, Str);
            fvar_itoa(DummyWarpShift.Point[1].X, Str, 10U, 8U, (UINT32) FVAR_LEADING_NONE);
            AmbaImgFrwCmd_PrintStr(F_PRINT_FLAG_MSG, Str);
            fvar_itoa(DummyWarpShift.Point[1].Y, Str, 10U, 8U, (UINT32) FVAR_LEADING_NONE);
            AmbaImgFrwCmd_PrintStr(F_PRINT_FLAG_MSG, Str);

            FuncRetCode = AmbaImgMainEis_WarpUpdatePut(EisIdx, &DummyWarpShift);
            if (FuncRetCode != OK_UL) {
                /* */
            }
        }
    } else {
        AmbaImgFrwCmd_PrintStr(F_PRINT_FLAG_MSG, "usage: [eis idex][mode:0]");
        AmbaImgFrwCmd_PrintStr(F_PRINT_FLAG_MSG, "       [eis idex][mode:1][shift x][shift y]");
        AmbaImgFrwCmd_PrintStr(F_PRINT_FLAG_MSG, "       [eis idex][mode:2][setp factor][accumulated factor][point1 x][point1 y][point2 x][point2 y]");
    }
}
#endif
