/**
 * @file AmbaLink_AmbaShell.c
 *
 * Copyright (c) 2020 Ambarella International LP
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
 * @details The module of Ambarella Network Support
 *
 */

#include "AmbaShell_Commands.h"
#include "AmbaPrint.h"
#include "AmbaLink.h"
#include "AmbaIPC_Lock.h"

#ifdef CONFIG_AMBALINK_RPMSG_HIBER
extern int HiberTestEntry(UINT32 argc, char * const * argv, AMBA_SHELL_PRINT_f PrintFunc);
#endif // CONFIG_AMBALINK_RPMSG_HIBER

/*----------------------------------------------------------------------------*\
 *  AmbaLink_Test
\*----------------------------------------------------------------------------*/
static void LinkTestUsage(AMBA_SHELL_PRINT_f PrintFunc)
{
    PrintFunc("ambalink init \n");
#if defined(CONFIG_AMBALINK_BOOT_OS)
    PrintFunc("ambalink os \n");
    PrintFunc("ambalink g1app \n");
#endif
    PrintFunc("ambalink g2app \n");
    PrintFunc("ambalink ipc \n");
    PrintFunc("ambalink lock \n");
}

void AmbaShell_CommandAmbaLink(UINT32 argc, char * const * argv, AMBA_SHELL_PRINT_f PrintFunc);
void AmbaShell_CommandAmbaLink(UINT32 argc, char * const * argv, AMBA_SHELL_PRINT_f PrintFunc)
{
    if (argc < 2U) {
        LinkTestUsage(PrintFunc);
    }
    //ambalink ipc rpmsg test
    //ambalink ipc g2 test
    //ambalink ipc rpc clnt exec2 /usr/local/share/script/usb_console.sh
    //ambalink ipc rpc svc pt +
    //ambalink ipc rpc clnt exec2 amba_pt
    //ambalink ipc rpc clnt pt ps
    else if (0 == AmbaUtility_StringCompare(argv[1], "ipc", AmbaUtility_StringLength("ipc"))) {
        extern INT32 IpcTestEntry(UINT32 argc, char * const * argv, AMBA_SHELL_PRINT_f PrintFunc);
        (void)IpcTestEntry(argc - 1U, &argv[1], PrintFunc);
    }
    //ambalink lock m 1
    //ambalink lock m 0
    //ambalink lock s 1
    //ambalink lock s 0
    else if (0 == AmbaUtility_StringCompare(argv[1], "lock", AmbaUtility_StringLength("lock"))) {
        if (argc == 4U) {
            extern INT32 AmbaIPC_LockTest(const char *ml, const char *action, AMBA_SHELL_PRINT_f PrintFunc);
            (void)AmbaIPC_LockTest(argv[2], argv[3], PrintFunc);
        }
        else {
            PrintFunc("lock AMBA_IPC_MUTEX_NAND:\n");
            PrintFunc("  ambalink lock m 1\n");
            PrintFunc("unlock AMBA_IPC_MUTEX_NAND:\n");
            PrintFunc("  ambalink lock m 0\n");
            PrintFunc("lock spinlock id 0:\n");
            PrintFunc("  ambalink lock s 1\n");
            PrintFunc("unlock spinlock id 0:\n");
            PrintFunc("  ambalink lock s 0\n");
        }
    }
    //ambalink init
    //ambalink g2app
    //ambalink os
    //ambalink g1app
    else if (argc == 2U) {
        if (0 == AmbaUtility_StringCompare(argv[1], "init", AmbaUtility_StringLength("init"))) {
            (void) AmbaPrint_ModuleSetAllowList(AMBALINK_MODULE_ID, 1U);
            (void) AmbaIPC_Init();
            AmbaPrint_PrintStr5("AmbaIPC_Init() done", NULL, NULL, NULL, NULL, NULL);
        }
#if defined(CONFIG_AMBALINK_BOOT_OS)
        else if (0 == AmbaUtility_StringCompare(argv[1], "os", AmbaUtility_StringLength("os"))) {
            AmbaLink_OS();
#if defined(CONFIG_OPENAMP)
            (void) AmbaLink_RpmsgInit(RPMSG_DEV_OAMP);
#elif !defined(CONFIG_AMBALINK_RPMSG_G2)
            (void) AmbaLink_RpmsgInit(RPMSG_DEV_AMBA);
#endif
        }
        else if (0 == AmbaUtility_StringCompare(argv[1], "g1app", AmbaUtility_StringLength("g1app"))) {
            extern void AmbaLink_StartBasicRpmsgApp(void);
            AmbaLink_StartBasicRpmsgApp();
            AmbaPrint_PrintStr5("AmbaLink_StartBasicRpmsgApp() done", NULL, NULL, NULL, NULL, NULL);
        }
#endif
#ifdef CONFIG_AMBALINK_RPMSG_G2
        else if (0 == AmbaUtility_StringCompare(argv[1], "g2app", AmbaUtility_StringLength("g2app"))) {
            {
                extern INT32 AmbaIPC_G2Echo(void);
                (void)AmbaIPC_G2Echo();
                AmbaPrint_PrintStr5("AmbaIPC_G2Echo() done", NULL, NULL, NULL, NULL, NULL);
            }
        }
#endif
        else if (0 == AmbaUtility_StringCompare(argv[1], "config", AmbaUtility_StringLength("config"))) {
            extern INT32 AmbaLink_ShowConfig(void);
            (void)AmbaLink_ShowConfig();
        }
        else if (0 == AmbaUtility_StringCompare(argv[1], "v2boot", AmbaUtility_StringLength("v2boot"))) {
            extern INT32 AmbaLink_BootV2(UINT32 TimeOut);
            (void)AmbaLink_BootV2(10000);
        }
        else if (0 == AmbaUtility_StringCompare(argv[1], "dump", AmbaUtility_StringLength("dump"))) {
            extern void AmbaIPC_dump(void);
            AmbaIPC_dump();
        }
#if defined(CONFIG_AMBALINK_RPMSG_ASIL)
        //ambalink gosafe
        else if (0 == AmbaUtility_StringCompare(argv[1], "gosafe", AmbaUtility_StringLength("gosafe"))) {
            /* A53: notify R52 CEHU */
            /* R52: tell A53 to enter safe state */
            extern UINT32 AmbaSafety_IPCSetSafeState(UINT32 State);
            (void)AmbaSafety_IPCSetSafeState(1U);
        }
#endif
        /* Self-test for OpenAMP */
#define OMAP_TEST (0)
#if OMAP_TEST
        else if (0 == AmbaUtility_StringCompare(argv[1], "oamp_t", AmbaUtility_StringLength("oamp_t"))) {
            extern void AmbaOamp_slave_test(void);
            AmbaOamp_slave_test();
        }
#endif
        else {
            LinkTestUsage(PrintFunc);
        }
#ifdef CONFIG_AMBALINK_RPMSG_HIBER
    } else if (argc >= 2 && AmbaUtility_StringCompare(argv[1], "hiber", 5) == 0) {
        HiberTestEntry(argc -1, &argv[1], PrintFunc);
        AmbaPrint_ModulePrintStr5(AMBALINK_MODULE_ID, "HiberTestEntry done", NULL, NULL, NULL, NULL, NULL);
        AmbaPrint_Flush();
#endif // CONFIG_AMBALINK_RPMSG_HIBER
    } else {
        LinkTestUsage(PrintFunc);
    }
    AmbaPrint_Flush();
}
