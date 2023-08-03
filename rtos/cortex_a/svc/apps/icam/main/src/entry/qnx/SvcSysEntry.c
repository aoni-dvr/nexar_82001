/**
 *  @file SvcSysEntry.c
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
 *  @details Entry point of SVC
 *
 */

#include <stdio.h>
#include <termios.h>

#include "AmbaTypes.h"
#include "AmbaKAL.h"
#include "AmbaMisraFix.h"
#include "AmbaShell.h"
#include "AmbaShell_Commands.h"
#include "AmbaDrvEntry.h"

#include "SvcCmd.h"
#include "SvcCmdSys.h"
#include "SvcCmdDSP.h"
#include "SvcCmdPbk.h"
#include "SvcShell.h"
#include "SvcInitTask.h"
#include "SvcLog.h"
#include "SvcFwUpdate.h"

#define SVC_LOG_MAIN                "MAIN"

static int fdShell = -1;

static UINT32 CharGet(UINT32 RxDataSize, char *pRxDataBuf, UINT32 TimeOut)
{
    AmbaMisra_TouchUnused(&TimeOut);

    return (UINT32)read(fdShell, pRxDataBuf, RxDataSize);
}

static  UINT32 CharPut(UINT32 TxDataSize, const char *pTxDataBuf, UINT32 TimeOut)
{
    AmbaMisra_TouchUnused(&TimeOut);

    return (UINT32)write(fdShell, pTxDataBuf, TxDataSize);
}

static void ShellInit(void)
{
    #define SHELL_UART      "/dev/ser1"
    #define SHELL_EXEC_STACK_SIZE       (0x320000U)

    extern void  AmbaSensorUT_TestCmd(UINT32 ArgCount, char * const * pArgVector, AMBA_SHELL_PRINT_f PrintFunc);
#if defined(CONFIG_ENABLE_AMBALINK) && !defined(CONFIG_QNX)
    extern void AmbaShell_CommandAmbaLink(UINT32 argc, char * const * argv, AMBA_SHELL_PRINT_f PrintFunc);
#endif
    static UINT8 ExecTaskStack[SHELL_EXEC_STACK_SIZE] GNU_SECTION_NOZEROINIT;

    static const AMBA_SHELL_COMMAND_s BasicCmds[] = {
        {.pName = "help",    .MainFunc = AmbaShell_CommandHelp,          .pNext = NULL},
        {.pName = "cat",     .MainFunc = AmbaShell_CommandCat,           .pNext = NULL},
        {.pName = "cd",      .MainFunc = AmbaShell_CommandChangeDir,     .pNext = NULL},
        {.pName = "cp",      .MainFunc = AmbaShell_CommandCopy,          .pNext = NULL},
        {.pName = "format",  .MainFunc = AmbaShell_CommandFormat,        .pNext = NULL},
        {.pName = "ls",      .MainFunc = AmbaShell_CommandListDir,       .pNext = NULL},
        {.pName = "mkdir",   .MainFunc = AmbaShell_CommandMakeDir,       .pNext = NULL},
        {.pName = "mv",      .MainFunc = AmbaShell_CommandMove,          .pNext = NULL},
        {.pName = "rm",      .MainFunc = AmbaShell_CommandRemove,        .pNext = NULL},
        {.pName = "vol",     .MainFunc = AmbaShell_CommandVolumn,        .pNext = NULL},
        {.pName = "readl",   .MainFunc = AmbaShell_CommandReadLong,      .pNext = NULL},
        {.pName = "writel",  .MainFunc = AmbaShell_CommandWriteLong,     .pNext = NULL},
        {.pName = "savebin", .MainFunc = AmbaShell_CommandSaveBinary,    .pNext = NULL},
        {.pName = "dmesg",   .MainFunc = SvcCmd_DisplayMsg,              .pNext = NULL},
        {.pName = "sleep",   .MainFunc = AmbaShell_CommandSleep,         .pNext = NULL},
        {.pName = "reboot",  .MainFunc = AmbaShell_CommandReboot,        .pNext = NULL},
        {.pName = "sdshmoo", .MainFunc = AmbaShell_SdShmoo,              .pNext = NULL},
        {.pName = "thruput", .MainFunc = AmbaShell_CommandFsThruput,     .pNext = NULL},
        {.pName = "date",    .MainFunc = AmbaShell_CommandDate,          .pNext = NULL},
        {.pName = "exec",    .MainFunc = AmbaShell_CommandExec,          .pNext = NULL},
        {.pName = "sysinfo", .MainFunc = AmbaShell_CommandSysInfo,       .pNext = NULL},

        {.pName = "sensor",  .MainFunc = AmbaSensorUT_TestCmd,           .pNext = NULL},
#if defined(CONFIG_ICAM_EEPROM_USED)
        {.pName = "eeprom",  .MainFunc = AmbaEEPRomUT_TestCmd,           .pNext = NULL},
#endif
        {.pName = "gpio",    .MainFunc = SvcCmd_Gpio,                    .pNext = NULL},

#if defined(CONFIG_ENABLE_AMBALINK) && !defined(CONFIG_QNX)
        {.pName = "ambalink", .MainFunc = AmbaShell_CommandAmbaLink,     .pNext = NULL},
#endif
#ifdef CONFIG_ICAM_USB_USED
        { .pName = "usb",   .MainFunc = AmbaShell_CommandUsb,            .pNext = NULL },
#endif
    };

    UINT32               i, Rval, CmdNum;
    AMBA_SHELL_CONFIG_s  Config;
    struct termios       Tio;

    SvcLog_DBG(SVC_LOG_MAIN, "## shell init begin", 0U, 0U);

    /* init amba shell */
    fdShell = open(SHELL_UART, O_RDWR);
    if (fdShell != -1) {
        if (tcgetattr(fdShell, &Tio) == 0) {
            // we need to use non-canonical mode and no echo to fit
            // ThreadX Shell implementation.
            Tio.c_lflag &= ~(ECHO | ECHOE | ICANON);
            if (0 > tcsetattr(fdShell, TCSANOW, &Tio)) {
                SvcLog_NG(SVC_LOG_MAIN, "## fail to tcsetattr()", 0U, 0U);
            }
        } else {
            SvcLog_NG(SVC_LOG_MAIN, "## fail to get termio info", 0U, 0U);
        }

        CmdNum = (UINT32)sizeof(BasicCmds) / (UINT32)sizeof(AMBA_SHELL_COMMAND_s);
        for (i = 0; i < CmdNum; i++) {
            Rval = SvcCmd_CommandRegister(&BasicCmds[i]);
            if (SHELL_ERR_SUCCESS != Rval) {
                SvcLog_NG(SVC_LOG_MAIN, "## fail to register cmd", 0U, 0U);
            }
        }
        SvcCmdDSP_Install();            /* install svc dsp command */
        SvcCmdSys_Install();            /* install svc sys command */
#ifdef CONFIG_ICAM_PLAYBACK_USED
        SvcCmdPbk_Install();
#endif

        SvcFwUpdate_CmdInstall();

        Config.pExecTaskStack     = ExecTaskStack;
        Config.ExecTaskStackSize  = SHELL_EXEC_STACK_SIZE;
        Config.GetCharFunc = CharGet;
        Config.PutCharFunc = CharPut;
        Rval = AmbaShell_Init(&Config);
        if (SHELL_ERR_SUCCESS != Rval) {
            SvcLog_NG(SVC_LOG_MAIN, "## fail to init shell", 0U, 0U);
        }
    } else {
        SvcLog_NG(SVC_LOG_MAIN, "fail to open uart port", 0U, 0U);
    }

    SvcLog_DBG(SVC_LOG_MAIN, "## shell init end", 0U, 0U);
}

static void DrvEntry(void)
{
    if (AmbaSYS_DrvEntry() != OK) {
        SvcLog_NG(SVC_LOG_MAIN, "## fail to AmbaSYS_DrvEntry()", 0U, 0U);
    }

    #if defined(CONFIG_SVC_ENABLE_WDT)
    if (AmbaWDT_DrvEntry() != OK) {
        SvcLog_NG(SVC_LOG_MAIN, "## fail to AmbaWDT_DrvEntry()", 0U, 0U);
    }
    #endif

#if defined(CONFIG_ICAM_ADC_BUTTON_USED)
    if (AmbaADC_DrvEntry() != OK) {
        SvcLog_NG(SVC_LOG_MAIN, "## fail to AmbaADC_DrvEntry()", 0U, 0U);
    }
#endif

#if defined(CONFIG_ICAM_HDMI_EXIST)
    if (AmbaHDMI_DrvEntry() != OK) {
        SvcLog_NG(SVC_LOG_MAIN, "## fail to AmbaHDMI_DrvEntry()", 0U, 0U);
    }
#endif

    if (AmbaI2C_DrvEntry() != OK) {
        SvcLog_NG(SVC_LOG_MAIN, "## fail to AmbaI2C_DrvEntry()", 0U, 0U);
    }

    //if (AmbaPWM_DrvEntry() != OK) {
    //    SvcLog_NG(SVC_LOG_MAIN, "## fail to AmbaPWM_DrvEntry()", 0U, 0U);
    //}

    if (AmbaRTC_DrvEntry() != OK) {
        SvcLog_NG(SVC_LOG_MAIN, "## fail to AmbaRTC_DrvEntry()", 0U, 0U);
    }

    if (AmbaSPI_DrvEntry() != OK) {
        SvcLog_NG(SVC_LOG_MAIN, "## fail to AmbaSPI_DrvEntry()", 0U, 0U);
    }

    if (AmbaVout_DrvEntry() != OK) {
        SvcLog_NG(SVC_LOG_MAIN, "## fail to AmbaVout_DrvEntry()", 0U, 0U);
    }

    if (AmbaVIN_DrvEntry() != OK) {
        SvcLog_NG(SVC_LOG_MAIN, "## fail to AmbaVIN_DrvEntry()", 0U, 0U);
    }
}

int main(int argc, char **argv)
{
    UINT32 Rval = 0U;

    AmbaMisra_TouchUnused(&argc);
    AmbaMisra_TouchUnused(argv);

    /* init drivers */
    DrvEntry();

    /* create init task */
    SvcInitTask_Create();

    /* init shell */
    ShellInit();

    while(argv != NULL) {
        if (0U == AmbaKAL_TaskSleep(10000U)) {
            AmbaMisra_TouchUnused(&Rval);
        }
        if (Rval != 0U) {
            break;
        }
    }

    return 0;
}
