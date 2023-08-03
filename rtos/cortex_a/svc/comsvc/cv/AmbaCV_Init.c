/**
 *  @file AmbaCV_Init.c
 *
 *  Copyright (c) 2020 Ambarella International LP
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
 *  @details AmbaCV Shell Command APIs
 *
 */

#include "AmbaTypes.h"
#include "AmbaKAL.h"
#include "AmbaFS.h"
#include "AmbaShell.h"
#include "AmbaUtility.h"
#include "AmbaCache.h"
#include "AmbaNAND.h"
#include "cvapi_protection.h"
#include "schdr_api.h"
#include "cvapi_ambacv_flexidag.h"
#ifdef CONFIG_CV_FLEXIDAG_UT
#include "cvapi_flexidag_unittest.h"
#endif
#include "AmbaCV_Init.h"

AMBA_SHELL_PRINT_f ShellPrintFunc;

extern INT32 VPutilsTestEntry(INT32 argc, char **argv, AMBA_SHELL_PRINT_f PrintFunc);

#ifdef CONFIG_AMBALINK_BOOT_OS
extern UINT32 AmbaIPC_CVProtectionSvcInit(void);
#endif

void cv_test_help(AMBA_SHELL_PRINT_f PrintFunc)
{
    PrintFunc("Supported test commands:\n");
    PrintFunc("    dmsg     [mode] [num_lines]\n");
    PrintFunc("        [mode]: 0x1 for sod scheduler log\n");
    PrintFunc("                0x2 for sod cvtask log\n");
    PrintFunc("                0x4 for vp scheduler log\n");
    PrintFunc("                0x8 for vp cvtask log\n");
    PrintFunc("        [num_lines] 0 means as many as possible\n");
    PrintFunc("    log_console [flags]  set log output to console module\n");
    PrintFunc("                [flags]: 0x0    Disable log out CVTask and SCHDR module to console\n");
    PrintFunc("                [flags]: 0x1    Only enable log out SCHDR module to console\n");
    PrintFunc("                [flags]: 0x2    Only enable log out CVTask module to console\n");
    PrintFunc("                [flags]: 0x3    enable log out CVTask and SCHDR module to console\n");
    PrintFunc("    log_flush    flush log from buffer\n");
    PrintFunc("    vpstatus [OPTIONS]\n");
    PrintFunc("        -v arg[0|1]: vp instance [vp0|vp1]\n");
    PrintFunc("        -s arg[0|1]: summary of  [vorc|vp]\n");
    PrintFunc("        -n arg: name of the register to read\n");
    PrintFunc("        -d arg: file name of the debug registers\n");
    PrintFunc("    flexidag schdr start [schdr bin path]\n");
    PrintFunc("    flexidag schdr stop\n");
    PrintFunc("    flexidag schdr version\n");
    PrintFunc("    flexidag schdr loading\n");
    PrintFunc("    flexidag schdr autorun\n");
    PrintFunc("    flexidag log_console [handle]\n");
    PrintFunc("    flexidag cfg slot [num]  \n");
    PrintFunc("    flexidag cfg cpu_map [bit_map] \n");
    PrintFunc("    flexidag cfg log_level [level] \n");
    PrintFunc("    flexidag cfg boot_cfg [cfg] \n");
    PrintFunc("    flexidag cfg autorun_intval [intval] \n");
    PrintFunc("    flexidag cfg autorun_fex_cfg [cfg] \n");
    PrintFunc("    flexidag perf start \n");
    PrintFunc("    flexidag perf stop \n");
    PrintFunc("    flexidag perf dump [path] \n");
#ifdef CONFIG_AMBALINK_BOOT_OS
    PrintFunc("    flexidag protection svc\n");
#endif
#ifdef CONFIG_CV_FLEXIDAG_UT
    RefCV_TestHelp(PrintFunc);
#endif
}

static int32_t Load_Binary(char *pBuf, int32_t Size, void* pToken)
{
    const char *name;
    AMBA_FS_FILE *fp = NULL;
    INT32 Fsize = 0, Ret = 0;
    UINT64 fpos = 0U;
    UINT32 op_res = 0U;
    UINT32 ret = 0U;

    AmbaMisra_TypeCast(&name, &pToken);
    ret |= AmbaFS_FileOpen(name, "rb", &fp);
    if (fp == NULL) {
        //AmbaPrint("Cannot open file %s", name);
        ShellPrintFunc("Cannot open file ");
        ShellPrintFunc(name);
        ShellPrintFunc("\n");

        Fsize = -1;
    } else {
        ret |= AmbaFS_FileSeek(fp, 0LL, AMBA_FS_SEEK_END);
        ret |= AmbaFS_FileTell(fp, &fpos);
        op_res = (UINT32)(fpos & 0xFFFFFFFFU);
        Fsize = (INT32)op_res;

        if (pBuf != NULL) {
            if (Fsize > Size) {
                //AmbaPrint("pBuf is too small for file %s", name);
                ShellPrintFunc("pBuf is too small for file ");
                ShellPrintFunc(name);
                ShellPrintFunc("\n");

                Ret = -2;
            } else {
                ret |= AmbaFS_FileSeek(fp, 0LL, AMBA_FS_SEEK_START);
                ret |= AmbaFS_FileRead(pBuf, 1, (UINT32)Fsize, fp, &op_res);
            }
        }

        ret |= AmbaFS_FileClose(fp);
        if (Ret != 0) {
            Fsize = Ret;
        } else {
            ShellPrintFunc("Load ");
            ShellPrintFunc(name);
            ShellPrintFunc(" success\n");
        }
    }

    AmbaMisra_TouchUnused(pToken);
    if(ret != 0U) {
        ShellPrintFunc("[ERROR] Load_Binary fail ");
    }
    return Fsize;
}

static INT32 flexidag_slot_num = 8;
#ifdef CONFIG_AMBALINK_BOOT_OS
static INT32 flexidag_cpu_map = 0x1;
#else
static INT32 flexidag_cpu_map = 0xE;
#endif
static INT32 flexidag_log_level = LVL_DEBUG;
static INT32 flexidag_boot_cfg = 1 + FLEXIDAG_ENABLE_BOOT_CFG;
static INT32 autorun_interval = 0;
static INT32 autorun_fex_cfg = 0;
static char schdr_path[256];
static char cvtable_name[256];
static UINT32 is_cvtable = 0U;

static void set_boot_cfg(UINT32 ArgCount, char * const * pArgVector, AMBA_SHELL_PRINT_f PrintFunc)
{
    UINT32 Rval = 0U;

    (void) ArgCount;
    if ( AmbaUtility_StringCompare(pArgVector[3], "slot", 4U) == 0) {
        UINT32 value = 0U;

        Rval = AmbaUtility_StringToUInt32(pArgVector[4],&value);
        flexidag_slot_num = (INT32)value;
    } else if ( AmbaUtility_StringCompare(pArgVector[3], "cpu_map", 7U) == 0) {
        UINT32 value = 0U;

        Rval = AmbaUtility_StringToUInt32(pArgVector[4],&value);
        flexidag_cpu_map = (INT32)value;
    } else if ( AmbaUtility_StringCompare(pArgVector[3], "log_level", 9U) == 0) {
        UINT32 value = 0U;

        Rval = AmbaUtility_StringToUInt32(pArgVector[4],&value);
        flexidag_log_level = (INT32)value;
    } else if ( AmbaUtility_StringCompare(pArgVector[3], "boot_cfg", 8U) == 0) {
        UINT32 value = 0U;

        Rval = AmbaUtility_StringToUInt32(pArgVector[4],&value);
        flexidag_boot_cfg = (INT32)value + FLEXIDAG_ENABLE_BOOT_CFG;
    } else if ( AmbaUtility_StringCompare(pArgVector[3], "autorun_intval", 14U) == 0) {
        UINT32 value = 0U;

        Rval = AmbaUtility_StringToUInt32(pArgVector[4],&value);
        autorun_interval = (INT32)value;
    } else if ( AmbaUtility_StringCompare(pArgVector[3], "autorun_fex_cfg", 15U) == 0) {
        UINT32 value = 0U;

        Rval = AmbaUtility_StringToUInt32(pArgVector[4],&value);
        autorun_fex_cfg = (INT32)value;
    } else {
        cv_test_help(PrintFunc);
    }
    (void) Rval;
}

UINT32 AmbaShell_CommandSchdrStart(void)
{
    UINT32 Rval = 0U;
    uint32_t schdr_state;
    UINT32 ret = 0U;
    INT32 ret1 = 0;
    UINT32 flag = 0U;

    ret |= AmbaCV_SchdrState(&schdr_state);
    if(schdr_state == FLEXIDAG_SCHDR_OFF) {
        AMBA_CV_STANDALONE_SCHDR_CFG_s cfg;

        if(flexidag_boot_cfg != 0) {
            ret |= AmbaCV_SchdrLoad(schdr_path);
            ret |= AmbaCV_SchdrCheckVisorc(FLEXIDAG_SCHDR_VP_BIN);
            if((is_cvtable == 1U)) {
                ret |= AmbaCV_SchdrAddCvtable(cvtable_name, Load_Binary);
            }
            (void) ret;
        }
        cfg.flexidag_slot_num = flexidag_slot_num;
        cfg.cavalry_slot_num = 0;
        cfg.cpu_map = flexidag_cpu_map;
        cfg.log_level = flexidag_log_level;

#ifdef CONFIG_CV_EXTERNAL_MSG_THREAD
        flag = (SCHDR_FASTBOOT | SCHDR_NO_INTERNAL_THREAD);
        cfg.flag = (INT32) flag;
#else
        flag = (SCHDR_FASTBOOT);
        cfg.flag = (INT32) flag;
#endif
        cfg.boot_cfg = flexidag_boot_cfg;
        cfg.autorun_interval = autorun_interval;
        cfg.autorun_fex_cfg = autorun_fex_cfg;

        ret |= AmbaCV_StandAloneSchdrStart(&cfg);
        ret1 = AmbaCV_ProtectInit((UINT32)flexidag_cpu_map);
    } else {
        AMBA_CV_FLEXIDAG_SCHDR_CFG_s cfg;

        cfg.cpu_map = flexidag_cpu_map;
        cfg.log_level = flexidag_log_level;
        ret |= AmbaCV_FlexidagSchdrStart(&cfg);
        ret1 = AmbaCV_ProtectInit((UINT32)flexidag_cpu_map);
    }

    (void) ret;
    if((ret != 0U) || (ret1 != 0)) {
        ShellPrintFunc("[ERROR] AmbaShell_CommandFlexidag fail ");
        Rval = 1U;
    }
    return Rval;
}

static void AmbaShell_CommandFlexidag(UINT32 ArgCount, char * const * pArgVector, AMBA_SHELL_PRINT_f PrintFunc)
{
    UINT32 Rval = 0U;
    UINT32 ret = 0U;

    if (AmbaUtility_StringCompare(pArgVector[2], "schdr", 5U) == 0) {
        if (AmbaUtility_StringCompare(pArgVector[3], "start", 5U) == 0) {
            AmbaUtility_StringCopy(schdr_path, sizeof(schdr_path), pArgVector[4]);
            if((ArgCount == 6U)) {
                AmbaUtility_StringCopy(cvtable_name, sizeof(cvtable_name), pArgVector[5]);
                is_cvtable = 1U;
            }
            ret = AmbaShell_CommandSchdrStart();
        } else if ( (AmbaUtility_StringCompare(pArgVector[3], "stop", 4U) == 0) && (ArgCount == 4U)) {
            ret |= AmbaCV_ProtectDeInit();
            ret |= AmbaCV_SchdrShutdown(0);
        } else {
            AmbaShell_CommandFlexidagSchdrUtils(ArgCount, pArgVector, PrintFunc);
        }
    } else if ( (AmbaUtility_StringCompare(pArgVector[2], "cfg", 3U) == 0) && (ArgCount == 5U)) {
        set_boot_cfg(ArgCount,&pArgVector[0], PrintFunc);
#ifdef CONFIG_CV_FLEXIDAG_UT
    } else if (AmbaUtility_StringCompare(pArgVector[2], "run", 3U) == 0) {
        RefCV_UtCommand(ArgCount - 3U, &pArgVector[3], PrintFunc);
#endif
#ifdef CONFIG_AMBALINK_BOOT_OS
    } else if ( (AmbaUtility_StringCompare(pArgVector[2], "protection", 10U) == 0) && (ArgCount == 4U)) {
        if (AmbaUtility_StringCompare(pArgVector[3], "svc", 3U) == 0) {
            ret |= AmbaIPC_CVProtectionSvcInit();
        }
#endif

    } else {
        AmbaShell_CommandFlexidagUtils(ArgCount, pArgVector, PrintFunc);
    }

    (void) Rval;
    if(ret != 0U) {
        ShellPrintFunc("[ERROR] AmbaShell_CommandFlexidag fail ");
    }
}

static void AmbaShell_CommandCvLogConsole(UINT32 ArgCount, char * const * pArgVector)
{
    UINT32 Rval = 0U;
    UINT32 ValU32 = 0U;
    UINT32 ret = 0U;

    (void) ArgCount;
    Rval = AmbaUtility_StringToUInt32(pArgVector[2],&ValU32);
    if(ValU32 == 0x3U) {
        ret |= AmbaPrint_ModuleSetAllowList(SCHDR_PRINT_MODULE_ID, (UINT8)1U);
        ret |= AmbaPrint_ModuleSetAllowList(CVTASK_PRINT_MODULE_ID, (UINT8)1U);
    } else if(ValU32 == 0x2U) {
        ret |= AmbaPrint_ModuleSetAllowList(SCHDR_PRINT_MODULE_ID, (UINT8)0U);
        ret |= AmbaPrint_ModuleSetAllowList(CVTASK_PRINT_MODULE_ID, (UINT8)1U);
    } else if(ValU32 == 0x1U) {
        ret |= AmbaPrint_ModuleSetAllowList(SCHDR_PRINT_MODULE_ID, (UINT8)1U);
        ret |= AmbaPrint_ModuleSetAllowList(CVTASK_PRINT_MODULE_ID, (UINT8)0U);
    } else {
        ret |= AmbaPrint_ModuleSetAllowList(SCHDR_PRINT_MODULE_ID, (UINT8)0U);
        ret |= AmbaPrint_ModuleSetAllowList(CVTASK_PRINT_MODULE_ID, (UINT8)0U);
    }
    (void) Rval;
    if(ret != 0U) {
        ShellPrintFunc("[ERROR] AmbaShell_CommandCvLogConsole fail\n");
    }
}

void AmbaShell_CommandAmbaCV(UINT32 ArgCount, char * const * pArgVector, AMBA_SHELL_PRINT_f PrintFunc);
void AmbaShell_CommandAmbaCV(UINT32 ArgCount, char * const * pArgVector, AMBA_SHELL_PRINT_f PrintFunc)
{
    UINT32 Rval = 0U;
    UINT32 ret = 0U;
    INT32 ret1 = 0;

    if (ArgCount >= 2U) {
        ShellPrintFunc = PrintFunc;
        if ((AmbaUtility_StringCompare(pArgVector[1], "dmsg", 4U) == 0) && (ArgCount >= 3U)) {
            UINT32 mode = 0U;
            UINT32 lines = 0U;

            Rval = AmbaUtility_StringToUInt32(pArgVector[2],&mode);
            if (ArgCount >= 4U) {
                Rval = AmbaUtility_StringToUInt32(pArgVector[3],&lines);
            }
            if((mode & 0x1U) != 0U) {
                ret |= AmbaCV_SchdrDumpLog(FLEXIDAG_SCHDR_SOD_BIN, NULL, lines);
            }
            if((mode & 0x4U) != 0U) {
                ret |= AmbaCV_SchdrDumpLog(FLEXIDAG_SCHDR_VP_BIN, NULL, lines);
            }
        } else if (AmbaUtility_StringCompare(pArgVector[1], "vpstatus", 8U) == 0) {
            //char * const *argv = &pArgVector[1];
            char *argv[UTIL_MAX_STR_TOKEN_NUMBER];
            UINT32 i;
            uint32_t schdr_state;

            for(i = 0U; i < (ArgCount-1U); i++) {
                argv[i] = pArgVector[i+1U];
                //PrintFunc("src: ");
                //PrintFunc(pArgVector[i+1U]);
                //PrintFunc("\n\rdst: ");
                //PrintFunc(argv[i]);
                //PrintFunc("\n\r");
            }

            ret |= AmbaCV_SchdrState(&schdr_state);
            if(schdr_state == 1U) {
                ret |= AmbaCV_ProtectDeInit();
                ret |= AmbaCV_SchdrShutdown(0);
            }
            ret1 = VPutilsTestEntry((INT32)ArgCount - 1, argv, PrintFunc);
        } else if ((AmbaUtility_StringCompare(pArgVector[1], "log_console", 11U) == 0) && (ArgCount == 3U)) {
            AmbaShell_CommandCvLogConsole(ArgCount, pArgVector);
        } else if ((AmbaUtility_StringCompare(pArgVector[1], "log_flush", 9U) == 0) && (ArgCount == 2U)) {
            AmbaPrint_StopAndFlush();
        } else if ((AmbaUtility_StringCompare(pArgVector[1], "shutdown", 8U) == 0) && (ArgCount == 2U)) {
            ShellPrintFunc("'shutdown' will stop supporting, please use 'flexidag schdr stop'\n");
            ret |= AmbaCV_ProtectDeInit();
            ret |= AmbaCV_SchdrShutdown(0);
        } else if (AmbaUtility_StringCompare(pArgVector[1], "flexidag", 8U) == 0) {
            AmbaShell_CommandFlexidag(ArgCount, pArgVector, PrintFunc);
        } else {
            cv_test_help(PrintFunc);
        }
    } else {
        cv_test_help(PrintFunc);
    }

    (void) Rval;
    if((ret != 0U) || (ret1 != 0)) {
        ShellPrintFunc("[ERROR] AmbaShell_CommandCvLogConsole fail ");
    }
}

