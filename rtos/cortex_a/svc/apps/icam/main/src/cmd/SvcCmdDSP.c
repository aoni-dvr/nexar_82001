/**
 *  @file SvcCmdDSP.c
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
 *  @details svc dsp command functions
 *
 */

#include "AmbaMisraFix.h"
#include "AmbaShell.h"
#include "AmbaDSP_Liveview.h"
#include "AmbaUtility.h"

#include "SvcErrCode.h"
#include "SvcWrap.h"
#include "SvcLog.h"
#include "SvcCmd.h"
#include "SvcUtil.h"
#include "SvcLiveview.h"
#include "SvcLiveviewTask.h"
#include "SvcLivMon.h"
#include "SvcCmdDSP.h"
#include "SvcDSP.h"
#if defined(CONFIG_ICAM_BIST_UCODE)
#include "SvcUcBIST.h"
#endif

#define SVC_LOG_CMDDSP      "CMDDSP"

extern void AmbaDSP_CmdShow(UINT32 CmdCode, UINT8 On);
extern void AmbaDSP_CmdShowCat(UINT32 CmdCat, UINT8 On);
extern void AmbaDSP_CmdShowAll(UINT8 On);
extern void AmbaDSP_MsgShowAll(UINT8 On);
extern void AmbaDSP_MsgShow(UINT32 MsgCode, UINT8 On);

extern UINT32 AmbaDSP_SetDebugLevel(UINT32 Module, UINT32 Level, UINT8 CmdType);
extern UINT32 AmbaDSP_SetDebugThread(UINT32 ThreadValid, UINT32 ThreadMask, UINT8 CmdType);

extern UINT32 AmbaDSP_Stop(UINT32 CmdType);

static void CmdDSPUsage(AMBA_SHELL_PRINT_f PrintFunc)
{
    PrintFunc("svc_dsp commands:\n");
    PrintFunc(" cmd_all [on|off]:    print all command parameter\n");
    PrintFunc(" cmd_liv [on|off]:    print liveview command parameter\n");
    PrintFunc(" cmd_enc [on|off]:    print encoder command parameter\n");
    PrintFunc(" cmd_dec [on|off]:    print decoder command parameter\n");
    PrintFunc(" cmd_vout [on|off]:   print vout command parameter\n");
    PrintFunc(" cmd [on|off] [code]: print certain command parameter\n");
    PrintFunc(" msg [on|off] [code|all]: print certain/all message\n");
    PrintFunc(" ====================================================\n");
    PrintFunc(" dbg_lvl [level]:    set dsp log debug level\n");
    PrintFunc(" dbg_thd [thd_mask]: set dsp log thread disable mask, bit0 means thread0,\n");
    PrintFunc("                     etc; 1-disable, 0-enable\n");
    PrintFunc(" ====================================================\n");
    PrintFunc(" liv_ctrl [fov_bits] [on|off]: liveview control\n");
    PrintFunc(" liv_mon [on] [timescale] [numtick]: liveview monitor\n");
    PrintFunc("         [off|dump]\n");
    PrintFunc(" ====================================================\n");
    PrintFunc(" initdata: show dsp initial configuration\n");
    PrintFunc(" dump_liv: show liveview configuration\n");
#if defined(CONFIG_ICAM_BIST_UCODE)
    PrintFunc(" dump_bist: show ucode BIST information\n");
#endif
    PrintFunc(" boot_chk: check if DSP boot done\n");
    PrintFunc(" assert: force DSP assertion\n");
}

static void CmdDSPEntry(UINT32 ArgCount, char * const *pArgVector, AMBA_SHELL_PRINT_f PrintFunc)
{
    UINT32  Enable, Value = 0U, Rval = SVC_NG;

    if (1U < ArgCount) {
        if (0 == SvcWrap_strcmp("liv_ctrl", pArgVector[1U])) {
            if (3U < ArgCount) {
                UINT32              i, NumFov;
                SVC_LIV_FOV_CTRL_s  LivCtrl = {0};

                if (OK == SvcWrap_strtoul(pArgVector[2U], &Value)) {
                    SvcUtil_BitsToArr(AMBA_DSP_MAX_VIEWZONE_NUM, Value, &NumFov, LivCtrl.FovIDArr);
                    if (0 == SvcWrap_strcmp("on", pArgVector[3U])) {
                        Enable = 1U;
                    } else {
                        Enable = 0U;
                    }

                    for (i = 0; i < NumFov; i++) {
                        LivCtrl.EnableArr[i] = (UINT8)Enable;
                    }
                    SvcLiveview_Ctrl(NumFov, &LivCtrl);
                    Rval = OK;
                }
            }
        } else if (0 == SvcWrap_strcmp("liv_mon", pArgVector[1U])) {
            if (2U < ArgCount) {
                UINT32  Value1 = 0U;

                if (0 == SvcWrap_strcmp("on", pArgVector[2U])) {
                    if (4U < ArgCount) {
                        (void)SvcWrap_strtoul(pArgVector[3U], &Value);
                        (void)SvcWrap_strtoul(pArgVector[4U], &Value1);

                        SvcLivMon_Enable(Value, Value1);
                        Rval = OK;
                    }
                } else if (0 == SvcWrap_strcmp("off", pArgVector[2U])) {
                    SvcLivMon_Disable();
                    Rval = OK;
                } else if (0 == SvcWrap_strcmp("dump", pArgVector[2U])) {
                    SvcLivMon_Dump(PrintFunc);
                    Rval = OK;
                } else {
                    /* do nothing */
                }
            }
        } else if (0 == SvcWrap_strcmp("cmd", pArgVector[1U])) {
            if (3U < ArgCount) {
                if (0 == SvcWrap_strcmp("on", pArgVector[2U])) {
                    Enable = 1U;
                } else {
                    Enable = 0U;
                }

                if (SVC_OK == SvcWrap_strtoul(pArgVector[3U], &Value)) {
                    AmbaDSP_CmdShow(Value, (UINT8)Enable);
                    Rval = OK;
                }
            }
        } else if (0 == SvcWrap_strcmp("msg", pArgVector[1U])) {
            if (3U < ArgCount) {
                if (0 == SvcWrap_strcmp("on", pArgVector[2U])) {
                    Enable = 1U;
                } else {
                    Enable = 0U;
                }

                if (0 == SvcWrap_strcmp("all", pArgVector[3U])) {
                    AmbaDSP_MsgShowAll((UINT8)Enable);
                    Rval = OK;
                } else {
                    if (SVC_OK == SvcWrap_strtoul(pArgVector[3U], &Value)) {
                        AmbaDSP_MsgShow(Value, (UINT8)Enable);
                        Rval = OK;
                    }
                }
            }
        } else if (0 == SvcWrap_strcmp("cmd_all", pArgVector[1U])) {
            if (2U < ArgCount) {
                if (0 == SvcWrap_strcmp("on", pArgVector[2U])) {
                    Enable = 1U;
                } else {
                    Enable = 0U;
                }
                AmbaDSP_CmdShowAll((UINT8)Enable);
                Rval = OK;
            }
#if defined(CONFIG_THREADX)
        } else if (0 == SvcWrap_strcmp("initdata", pArgVector[1U])) {
            extern void DSP_DumpDspInitDataInfo(void);
            DSP_DumpDspInitDataInfo();
            Rval = OK;
        } else if (0 == SvcWrap_strcmp("cmd_liv", pArgVector[1U])) {
            if (2U < ArgCount) {
                if (0 == SvcWrap_strcmp("on", pArgVector[2U])) {
                    Enable = 1U;
                } else {
                    Enable = 0U;
                }
                AmbaDSP_CmdShowCat(0U, (UINT8)Enable);
                AmbaDSP_CmdShowCat(1U, (UINT8)Enable);
                AmbaDSP_CmdShowCat(8U, (UINT8)Enable);
                AmbaDSP_CmdShowCat(11U, (UINT8)Enable);
                AmbaDSP_CmdShowCat(15U, (UINT8)Enable);
                Rval = OK;
            }
        } else if (0 == SvcWrap_strcmp("cmd_enc", pArgVector[1U])) {
            if (2U < ArgCount) {
                if (0 == SvcWrap_strcmp("on", pArgVector[2U])) {
                    Enable = 1U;
                } else {
                    Enable = 0U;
                }
                AmbaDSP_CmdShowCat(2U, (UINT8)Enable);
                Rval = OK;
            }
        } else if (0 == SvcWrap_strcmp("cmd_dec", pArgVector[1U])) {
            if (2U < ArgCount) {
                if (0 == SvcWrap_strcmp("on", pArgVector[2U])) {
                    Enable = 1U;
                } else {
                    Enable = 0U;
                }
                AmbaDSP_CmdShowCat(3U, (UINT8)Enable);
                AmbaDSP_CmdShowCat(4U, (UINT8)Enable);
                AmbaDSP_CmdShowCat(5U, (UINT8)Enable);
                AmbaDSP_CmdShowCat(6U, (UINT8)Enable);
                Rval = OK;
            }
        } else if (0 == SvcWrap_strcmp("cmd_vout", pArgVector[1U])) {
            if (2U < ArgCount) {
                if (0 == SvcWrap_strcmp("on", pArgVector[2U])) {
                    Enable = 1U;
                } else {
                    Enable = 0U;
                }
                AmbaDSP_CmdShowCat(7U, (UINT8)Enable);
                AmbaDSP_CmdShowCat(10U, (UINT8)Enable);
                Rval = OK;
            }
#endif
        } else if (0 == SvcWrap_strcmp("dbg_lvl", pArgVector[1U])) {
            if (2U < ArgCount) {
                if (SVC_OK == SvcWrap_strtoul(pArgVector[2U], &Value)) {
                    Value = AmbaDSP_SetDebugLevel(0xFFFFFFFFU, Value, 0U);
                    AmbaMisra_TouchUnused(&Value);
                    Rval = OK;
                }
            }
        } else if (0 == SvcWrap_strcmp("dbg_thd", pArgVector[1U])) {
            if (2U < ArgCount) {
                if (SVC_OK == SvcWrap_strtoul(pArgVector[2U], &Value)) {
#if defined(CONFIG_SOC_CV5) || defined (CONFIG_SOC_CV52)
                    Value = AmbaDSP_SetDebugThread(0xFFU, Value, 0U);
#else
                    Value = AmbaDSP_SetDebugThread(1U, Value, 0U);
#endif
                    AmbaMisra_TouchUnused(&Value);
                    Rval = OK;
                }
            }
        } else if (0 == SvcWrap_strcmp("dump_liv", pArgVector[1U])) {
            SvcLiveview_Dump(PrintFunc);
            Rval = OK;
#if defined(CONFIG_ICAM_BIST_UCODE)
        } else if (0 == SvcWrap_strcmp("dump_bist", pArgVector[1U])) {
            SvcUcBIST_Dump(PrintFunc);
            Rval = OK;
#endif
#if defined(CONFIG_THREADX)
        } else if (0 == SvcWrap_strcmp("ll_stop", pArgVector[1U])) {
            extern UINT32 AmbaLL_LogEnable(UINT8 Enable);
            Rval = AmbaLL_LogEnable(1);
            if (Rval == OK) {
                SvcLog_OK(SVC_LOG_CMDDSP, "AmbaLL_LogEnable 1 success", 0, 0);
            } else {
                SvcLog_NG(SVC_LOG_CMDDSP, "AmbaLL_LogEnable 1 fail", 0, 0);
            }
        } else if (0 == SvcWrap_strcmp("ll_show", pArgVector[1U])) {
            if (SVC_OK == SvcWrap_strtoul(pArgVector[2U], &Value)) {
                extern void AmbaLL_LogShow(UINT32 Entry);
                UINT16 Module = (UINT16)(SSP_ERR_BASE >> 16U);
                Rval = AmbaPrint_ModuleSetAllowList(Module, 1);
                if (Rval == OK) {
                    SvcLog_OK(SVC_LOG_CMDDSP, "AmbaPrint_ModuleSetAllowList 0x%x %u", Module, 1);
                } else {
                    SvcLog_NG(SVC_LOG_CMDDSP, "AmbaPrint_ModuleSetAllowList 0x%x return %u", Module, Rval);
                }
                AmbaLL_LogShow(Value);
            }
#endif
        } else if (0 == SvcWrap_strcmp("boot_chk", pArgVector[1U])) {
            if (SvcDSP_IsBootDone() == 1U) {
                PrintFunc(" DSP boot done\n");
            } else {
                PrintFunc(" DSP not boot done\n");
            }
            Rval = OK;
        } else if (0 == SvcWrap_strcmp("assert", pArgVector[1U])) {
            Rval  = AmbaDSP_Stop(0U);
            Rval |= AmbaDSP_Stop(1U);
            if (Rval != 0U) {
                PrintFunc(" AmbaDSP_Stop fail\n");
            }
            Rval = OK;
        } else {
            /* do nothing */
        }
    }

    if (Rval != OK) {
        CmdDSPUsage(PrintFunc);
    }
}

/**
* install svc dsp command
* @return none
*/
void SvcCmdDSP_Install(void)
{
    AMBA_SHELL_COMMAND_s  SvcCmdDsp;

    UINT32  Rval;

    SvcCmdDsp.pName    = "svc_dsp";
    SvcCmdDsp.MainFunc = CmdDSPEntry;
    SvcCmdDsp.pNext    = NULL;

    Rval = SvcCmd_CommandRegister(&SvcCmdDsp);
    if (SHELL_ERR_SUCCESS != Rval) {
        SvcLog_NG(SVC_LOG_CMDDSP, "## fail to install svc dsp command", 0U, 0U);
    }
}

/**
* control of dsp command log
* @param [in] Enable 0 or 1
* @return none
*/
void SvcCmdDSP_CmdLogCtrl(UINT32 Enable)
{
    AmbaDSP_CmdShowAll((UINT8)Enable);
    AmbaDSP_CmdShow(0x0400000AU, 0U);
    AmbaDSP_CmdShow(0x0400000CU, 0U);
    AmbaDSP_CmdShow(0x0400000EU, 0U);
    AmbaDSP_CmdShow(0x05000004U, 0U);
    AmbaDSP_CmdShow(0x05000006U, 0U);
    AmbaDSP_CmdShow(0x06000005U, 0U);
}
