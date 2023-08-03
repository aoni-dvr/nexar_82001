/**
 *  @file AmbaMonFrwCmdApp.c
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
 *  @details AmbaShell Monitor Framework Command App
 *
 */

#include "AmbaTypes.h"
#include "AmbaKAL.h"
#include "AmbaVIN_Def.h"
#include "AmbaShell.h"

#include "AmbaDSP_Capability.h"
//#include "AmbaDSP_Event.h"
#include "AmbaDSP_EventInfo.h"

#include "AmbaMonFramework.h"
#include "AmbaMonDef.h"
#include "AmbaMonMain.h"
#include "AmbaMonMain_Internal.h"

#include "AmbaMonFrwCmdApp_Def.h"
#include "AmbaMonFrwCmdApp.h"
#include "AmbaMonFrwCmdApp_Table.c"

#define OK_UL  ((UINT32) 0U)
#define NG_UL  ((UINT32) 1U)

/**
 *  Amba monitor framework app command attach
 *  @return error code
 */
UINT32 AmbaMonFrwCmdApp_Attach(void)
{
    UINT32 RetCode = OK_UL;
    UINT32 FuncRetCode;
    static AMBA_MON_FRW_CMD_s AmbaMonFrwCmdApp = { AMBSH_MAGIC, "app", ambsh_monfrw_app, NULL };

    FuncRetCode = AmbaMonFrwCmd_Add(&AmbaMonFrwCmdApp);
    if (FuncRetCode != OK_UL) {
        RetCode = NG_UL;
    }

    return RetCode;
}

/**
 *  Amba shell monitor framework app command entry
 *  @param[in] Argc argument count
 *  @param[in] Argv pointer to the argument string pointer
 *  @param[in] PrintFunc amba shell print function
 *  @note this function is intended for internal use only
 */
static void ambsh_monfrw_app(UINT32 Argc, char * const *Argv, AMBA_SHELL_PRINT_f PrintFunc)
{
    UINT32 CmdIdx = 0U;
    const AMBA_MON_FRW_CMD_s *List = AmbaMonFrwCmdApp_List;

    if (Argc > 1U) {
        while ((List != NULL) && (List->Magic == AMBSH_MAGIC)) {
            /* valid */
            if (sfvar_strcmp(Argv[1], List->pName) == OK_UL) {
                /* debug msg */
                AmbaMonFrwCmd_Print(SF_PRINT_FLAG_DBG, "amba monfrw cmd app idx", CmdIdx);
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
 *  Amba shell monitor framework app command load
 *  @param[in] Argc argument count
 *  @param[in] Argv pointer to the argument string pointer
 *  @param[in] PrintFunc amba shell print function
 *  @note this function is intended for internal use only
 */
static void ambsh_monfrw_app_load(UINT32 Argc, char * const *Argv, AMBA_SHELL_PRINT_f PrintFunc)
{
    UINT32 i, CmdNum;
    AMBA_MON_FRW_CMD_s *List = AmbaMonFrwCmdApp_List;

    (void) Argc;
    (void) Argv;

    if (PrintFunc == NULL) {
        /* */
    }

    /* cmd num get */
    CmdNum = (UINT32) (sizeof(AmbaMonFrwCmdApp_List)/sizeof(AMBA_MON_FRW_CMD_s));
    /* debug msg */
    AmbaMonFrwCmd_Print(SF_PRINT_FLAG_MSG, "amba monfrw cmd app load", CmdNum);

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
 *  Amba shell monitor framework app command print enable
 *  @param[in] Argc argument count
 *  @param[in] Argv pointer to the argument string pointer
 *  @param[in] PrintFunc amba shell print function
 *  @note this function is intended for internal use only
 */
static void ambsh_monfrw_app_prt_en(UINT32 Argc, char * const *Argv, AMBA_SHELL_PRINT_f PrintFunc)
{
    if (PrintFunc == NULL) {
        /* */
    }

    if (Argc > 1U) {
        UINT32 Flag;

        Flag = sfvar_atou(Argv[1]);

        AmbaMonFrwCmd_PrintEx(SF_PRINT_FLAG_MSG, "amba monfrw print enable", Flag, 16U);
        AmbaMonPrint_Enable(Flag);
    } else {
        AmbaMonFrwCmd_PrintStr(SF_PRINT_FLAG_MSG, "usage: [flag]");
    }
}

/**
 *  Amba shell monitor framework app change detection enable
 *  @param[in] Argc argument count
 *  @param[in] Argv pointer to the argument string pointer
 *  @param[in] PrintFunc amba shell print function
 *  @note this function is intended for internal use only
 */
static void ambsh_monfrw_app_cd_en(UINT32 Argc, char * const *Argv, AMBA_SHELL_PRINT_f PrintFunc)
{
    if (PrintFunc == NULL) {
        /* */
    }

    if (Argc > 3U) {
        UINT32 Enable;
        UINT32 Interval;
        UINT32 Shift;

        Enable = sfvar_atou(Argv[1]);
        Interval = sfvar_atou(Argv[2]);
        Shift = sfvar_atou(Argv[3]);

        AmbaMonFrwCmd_Print(SF_PRINT_FLAG_MSG, "cd en", Enable);
        AmbaMonFrwCmd_Print(SF_PRINT_FLAG_MSG, "cd interval", Interval);
        AmbaMonFrwCmd_Print(SF_PRINT_FLAG_MSG, "cd shift", Shift);

#ifdef AMBA_MON_LISTEN_AAA_CHANGE_DETECTION
        AmbaMonListenAaa_CdEnable(Enable, Interval, Shift);
#else
        AmbaMonFrwCmd_PrintStr(SF_PRINT_FLAG_MSG, "not support!");
#endif
    } else {
        AmbaMonFrwCmd_PrintStr(SF_PRINT_FLAG_MSG, "usage: [en] [interval] [shift]");
    }
}

/**
 *  Amba shell monitor framework app command timing enable
 *  @param[in] Argc argument count
 *  @param[in] Argv pointer to the argument string pointer
 *  @param[in] PrintFunc amba shell print function
 *  @note this function is intended for internal use only
 */
static void ambsh_monfrw_app_tm_en(UINT32 Argc, char * const *Argv, AMBA_SHELL_PRINT_f PrintFunc)
{
    if (PrintFunc == NULL) {
        /* */
    }

    if (Argc > 1U) {
        UINT32 Flag;

        Flag = sfvar_atou(Argv[1]);

        AmbaMonFrwCmd_PrintEx(SF_PRINT_FLAG_MSG, "amba monfrw tm enable", Flag, 16U);
        AmbaMonMain_Info.Debug.Data = Flag;
    } else {
        AmbaMonFrwCmd_PrintStr(SF_PRINT_FLAG_MSG, "usage: [flag]");
    }
}

/**
 *  Amba shell monitor framework app command live view start
 *  @param[in] Argc argument count
 *  @param[in] Argv pointer to the argument string pointer
 *  @param[in] PrintFunc amba shell print function
 *  @note this function is intended for internal use only
 */
static void ambsh_monfrw_app_liv_start(UINT32 Argc, char * const *Argv, AMBA_SHELL_PRINT_f PrintFunc)
{
    UINT32 i;
    AMBA_MON_MAIN_CMD_MSG_s CmdMsg;

    if (PrintFunc == NULL) {
        /* */
    }

    if (Argc > 3U) {
        UINT32 VinSelect;
        UINT32 FovSelect;
        UINT32 VoutSelect;
        UINT32 TdSelect = 0U;
        UINT32 Skip;

        VinSelect = sfvar_atou(Argv[1]);
        FovSelect = sfvar_atou(Argv[2]);
        VoutSelect = sfvar_atou(Argv[3]);
        if (Argc > 4U) {
            TdSelect = sfvar_atou(Argv[4]);
        }

        AmbaMonFrwCmd_PrintEx(SF_PRINT_FLAG_MSG, " vin select", VinSelect, 16U);
        AmbaMonFrwCmd_PrintEx(SF_PRINT_FLAG_MSG, " fov select", FovSelect, 16U);
        AmbaMonFrwCmd_PrintEx(SF_PRINT_FLAG_MSG, " vout select", VoutSelect, 16U);
        if (Argc > 4U) {
            AmbaMonFrwCmd_PrintEx(SF_PRINT_FLAG_MSG, " td select", TdSelect, 16U);
        }

        /* fov latency skip */
        if (Argc > 5U) {
            /* skip get */
            Skip = sfvar_atou(Argv[5]);
            AmbaMonFrwCmd_PrintEx(SF_PRINT_FLAG_MSG, "amba monfrw fov latency skip", Skip, 10U);
            /* skip put */
            for (i = 0U; i < AMBA_MON_NUM_FOV_CHANNEL; i++) {
                AmbaMonMain_FovLatencySkip[i] = Skip;
            }
        }

        /* vout latency skip */
        if (Argc > 6U) {
            /* skip get */
            Skip = sfvar_atou(Argv[6]);
            AmbaMonFrwCmd_PrintEx(SF_PRINT_FLAG_MSG, "amba monfrw vout latency skip", Skip, 10U);
            /* skip put */
            for (i = 0U; i < AMBA_MON_NUM_VOUT_CHANNEL; i++) {
                AmbaMonMain_VoutLatencySkip[i] = Skip;
            }
        }

        CmdMsg.Ctx.Data = 0ULL;
        CmdMsg.Ctx.Liv.VinId = (UINT16) (VinSelect & 0xFFFFU);
        CmdMsg.Ctx.Liv.FovId = (UINT16) (FovSelect & 0xFFFFU);
        CmdMsg.Ctx.Liv.VoutId = (UINT8) (VoutSelect & 0xFFU);
        if (Argc > 4U) {
            CmdMsg.Ctx.Liv.TdId = (UINT16) (TdSelect & 0xFFFFU);
        }
        CmdMsg.Ctx.Liv.Cmd = (UINT8) AMBA_MON_CMD_START;
        AmbaMonFrwCmd_PrintStr(SF_PRINT_FLAG_MSG, "liv start...");
        AmbaMonMain_Config(CmdMsg.Ctx.Data);
    } else {
        AmbaMonFrwCmd_PrintStr(SF_PRINT_FLAG_MSG, "usage: [vin select] [fov select] [vout select]");
    }
}

/**
 *  Amba shell monitor framework app command live view stop
 *  @param[in] Argc argument count
 *  @param[in] Argv pointer to the argument string pointer
 *  @param[in] PrintFunc amba shell print function
 *  @note this function is intended for internal use only
 */
static void ambsh_monfrw_app_liv_stop(UINT32 Argc, char * const *Argv, AMBA_SHELL_PRINT_f PrintFunc)
{
    AMBA_MON_MAIN_CMD_MSG_s CmdMsg;

    if (PrintFunc == NULL) {
        /* */
    }

    if (Argc > 3U) {
        UINT32 VinSelect;
        UINT32 FovSelect;
        UINT32 VoutSelect;
        UINT32 TdSelect = 0U;

        VinSelect = sfvar_atou(Argv[1]);
        FovSelect = sfvar_atou(Argv[2]);
        VoutSelect = sfvar_atou(Argv[3]);
        if (Argc > 4U) {
            TdSelect = sfvar_atou(Argv[4]);
        }

        AmbaMonFrwCmd_PrintEx(SF_PRINT_FLAG_MSG, " vin select", VinSelect, 16U);
        AmbaMonFrwCmd_PrintEx(SF_PRINT_FLAG_MSG, " fov select", FovSelect, 16U);
        AmbaMonFrwCmd_PrintEx(SF_PRINT_FLAG_MSG, " vout select", VoutSelect, 16U);
        if (Argc > 4U) {
            AmbaMonFrwCmd_PrintEx(SF_PRINT_FLAG_MSG, " td select", TdSelect, 16U);
        }
        CmdMsg.Ctx.Data = 0ULL;
        CmdMsg.Ctx.Liv.VinId = (UINT16) (VinSelect & 0xFFFFU);
        CmdMsg.Ctx.Liv.FovId = (UINT16) (FovSelect & 0xFFFFU);
        CmdMsg.Ctx.Liv.VoutId = (UINT8) (VoutSelect & 0xFFU);
        if (Argc > 4U) {
            CmdMsg.Ctx.Liv.TdId = (UINT16) (TdSelect & 0xFFFFU);
        }
        CmdMsg.Ctx.Liv.Cmd = (UINT8) AMBA_MON_CMD_STOP;
        AmbaMonFrwCmd_PrintStr(SF_PRINT_FLAG_MSG, "liv stop...");
        AmbaMonMain_Config(CmdMsg.Ctx.Data);
    } else {
        AmbaMonFrwCmd_PrintStr(SF_PRINT_FLAG_MSG, "usage: [vin select] [fov select] [vout select]");
    }
}

/**
 *  Amba shell monitor framework app command wdog enable
 *  @param[in] Argc argument count
 *  @param[in] Argv pointer to the argument string pointer
 *  @param[in] PrintFunc amba shell print function
 *  @note this function is intended for internal use only
 */
static void ambsh_monfrw_app_wdog_en(UINT32 Argc, char * const *Argv, AMBA_SHELL_PRINT_f PrintFunc)
{
    if (PrintFunc == NULL) {
        /* */
    }

    if (Argc > 1U) {
        UINT32 Enable;

        Enable = sfvar_atou(Argv[1]);

        AmbaMonFrwCmd_PrintEx(SF_PRINT_FLAG_MSG, "amba monfrw wdog enable", Enable, 16U);
        if (Enable == 0U) {
            /* wdog active */
            AmbaMonMain_WatchdogInactive();
        } else {
            /* wdog inactive */
            AmbaMonMain_WatchdogActive();
        }
    } else {
        AmbaMonFrwCmd_PrintStr(SF_PRINT_FLAG_MSG, "usage: [enable]");
    }
}

/**
 *  Amba shell monitor framework app command vin enable
 *  @param[in] Argc argument count
 *  @param[in] Argv pointer to the argument string pointer
 *  @param[in] PrintFunc amba shell print function
 *  @note this function is intended for internal use only
 */
static void ambsh_monfrw_app_vin_en(UINT32 Argc, char * const *Argv, AMBA_SHELL_PRINT_f PrintFunc)
{
    if (PrintFunc == NULL) {
        /* */
    }

    if (Argc > 1U) {
        UINT32 Flag;

        Flag = sfvar_atou(Argv[1]);

        /* vin src en */
        AmbaMonFrwCmd_PrintEx(SF_PRINT_FLAG_MSG, "amba monfrw vin en", Flag, 16U);
        AmbaMonMain_VinSrcEn.Ctx.Data = Flag;
    } else {
        AmbaMonFrwCmd_PrintStr(SF_PRINT_FLAG_MSG, "usage: [flag]");
    }
}

/**
 *  Amba shell monitor framework app command dsp enable
 *  @param[in] Argc argument count
 *  @param[in] Argv pointer to the argument string pointer
 *  @param[in] PrintFunc amba shell print function
 *  @note this function is intended for internal use only
 */
static void ambsh_monfrw_app_dsp_en(UINT32 Argc, char * const *Argv, AMBA_SHELL_PRINT_f PrintFunc)
{
    UINT32 i;

    if (PrintFunc == NULL) {
        /* */
    }

    if (Argc > 1U) {
        UINT32 Flag;
        UINT32 Skip;

        Flag = sfvar_atou(Argv[1]);

        /* msg null debug */
        AmbaMonFrwCmd_PrintEx(SF_PRINT_FLAG_MSG, "amba monfrw dsp en", Flag, 16U);

        /* fov latency skip */
        if (Argc > 2U) {
            /* skip get */
            Skip = sfvar_atou(Argv[2]);
            AmbaMonFrwCmd_PrintEx(SF_PRINT_FLAG_MSG, "amba monfrw fov latency skip", Skip, 10U);
            /* skip put */
            for (i = 0U; i < AMBA_MON_NUM_FOV_CHANNEL; i++) {
                AmbaMonMain_FovLatencySkip[i] = Skip;
            }
        }

        /* vout latency skip */
        if (Argc > 3U) {
            /* skip get */
            Skip = sfvar_atou(Argv[3]);
            AmbaMonFrwCmd_PrintEx(SF_PRINT_FLAG_MSG, "amba monfrw vout latency skip", Skip, 10U);
            /* skip put */
            for (i = 0U; i < AMBA_MON_NUM_VOUT_CHANNEL; i++) {
                AmbaMonMain_VoutLatencySkip[i] = Skip;
            }
        }

        /* msg null debug put */
        AmbaMonMain_DspSrcEn.Ctx.Data = Flag;
    } else {
        AmbaMonFrwCmd_PrintStr(SF_PRINT_FLAG_MSG, "usage: [flag]");
    }
}

/**
 *  Amba shell monitor framework app command vout enable
 *  @param[in] Argc argument count
 *  @param[in] Argv pointer to the argument string pointer
 *  @param[in] PrintFunc amba shell print function
 *  @note this function is intended for internal use only
 */
static void ambsh_monfrw_app_vout_en(UINT32 Argc, char * const *Argv, AMBA_SHELL_PRINT_f PrintFunc)
{
    if (PrintFunc == NULL) {
        /* */
    }

    if (Argc > 1U) {
        UINT32 Flag;

        Flag = sfvar_atou(Argv[1]);

        /* vout src en */
        AmbaMonFrwCmd_PrintEx(SF_PRINT_FLAG_MSG, "amba monfrw vout en", Flag, 16U);
        AmbaMonMain_VoutSrcEn.Ctx.Data = Flag;
    } else {
        AmbaMonFrwCmd_PrintStr(SF_PRINT_FLAG_MSG, "usage: [flag]");
    }
}
