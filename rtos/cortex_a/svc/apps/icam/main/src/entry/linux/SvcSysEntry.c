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
#include <signal.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/prctl.h>

#include "AmbaTypes.h"
#include "AmbaKAL.h"
#include "AmbaMisraFix.h"
#include "AmbaFS.h"
#include "AmbaShell.h"
#include "AmbaShell_Commands.h"
#include "AmbaDrvEntry.h"
#include "AmbaSvcWrap.h"
#include "AmbaUtility.h"

#include "SvcCmd.h"
#include "SvcWdt.h"
#include "SvcClock.h"
#include "SvcCmdSys.h"
#include "SvcCmdDSP.h"
#include "SvcCmdPbk.h"
#include "SvcErrCode.h"
#include "SvcLog.h"
#include "SvcTaskList.h"
#include "SvcShell.h"
#include "SvcInitTask.h"
#include "SvcFwUpdate.h"
#include "iCamTimeProfile.h"
#include "iCamRecovery.h"
#include "SvcTiming.h"
#ifdef CONFIG_BUILD_CV
#include "SvcCvMainTask.h"
#include "SvcCvCtrlTask.h"
#endif

#define SVC_LOG_MAIN                "MAIN"

volatile sig_atomic_t  g_SigNum = 0;

static int fdShell = -1;

static UINT32 CharGet(UINT32 RxDataSize, char *pRxDataBuf, UINT32 TimeOut)
{
    AmbaMisra_TouchUnused(&TimeOut);

    return (UINT32)read(fdShell, pRxDataBuf, RxDataSize);
}

static UINT32 CharPut(UINT32 TxDataSize, const char *pTxDataBuf, UINT32 TimeOut)
{
    AmbaMisra_TouchUnused(&TimeOut);

    return (UINT32)write(fdShell, pTxDataBuf, TxDataSize);
}

static void ShellInit(void)
{
    #define SHELL_UART      "/dev/ttyS1"

    #define SHELL_CLI_STACK_SIZE        (0x1000U)
    #define SHELL_EXEC_STACK_SIZE       (0x320000U)

    extern void  AmbaSensorUT_TestCmd(UINT32 ArgCount, char * const * pArgVector, AMBA_SHELL_PRINT_f PrintFunc);
#ifdef CONFIG_ENABLE_AMBALINK
    extern void AmbaShell_CommandAmbaLink(UINT32 argc, char * const * argv, AMBA_SHELL_PRINT_f PrintFunc);
#endif

    static UINT8 CliTaskStack[SHELL_CLI_STACK_SIZE] GNU_SECTION_NOZEROINIT;
    static UINT8 ExecTaskStack[SHELL_EXEC_STACK_SIZE] GNU_SECTION_NOZEROINIT;

    static AMBA_SHELL_COMMAND_s BasicCmds[] = {
        {.pName = "help",    .MainFunc = AmbaShell_CommandHelp,          .pNext = NULL},
        {.pName = "readl",   .MainFunc = AmbaShell_CommandReadLong,      .pNext = NULL},
        {.pName = "writel",  .MainFunc = AmbaShell_CommandWriteLong,     .pNext = NULL},
        {.pName = "sleep",   .MainFunc = AmbaShell_CommandSleep,         .pNext = NULL},
        {.pName = "eject",   .MainFunc = AmbaShell_CommandEject,         .pNext = NULL},
        {.pName = "sensor",  .MainFunc = AmbaSensorUT_TestCmd,           .pNext = NULL},
        {.pName = "dmesg",   .MainFunc = SvcCmd_DisplayMsg,              .pNext = NULL},
        //{.pName = "gpio",    .MainFunc = SvcCmd_Gpio,                    .pNext = NULL},
        //{.pName = "cat",     .MainFunc = AmbaShell_CommandCat,           .pNext = NULL},
        {.pName = "cd",      .MainFunc = AmbaShell_CommandChangeDir,     .pNext = NULL},
        //{.pName = "cp",      .MainFunc = AmbaShell_CommandCopy,          .pNext = NULL},
        {.pName = "format",  .MainFunc = AmbaShell_CommandFormat,        .pNext = NULL},
        {.pName = "ls",      .MainFunc = AmbaShell_CommandListDir,       .pNext = NULL},
        //{.pName = "mkdir",   .MainFunc = AmbaShell_CommandMakeDir,       .pNext = NULL},
        {.pName = "mv",      .MainFunc = AmbaShell_CommandMove,          .pNext = NULL},
        //{.pName = "rm",      .MainFunc = AmbaShell_CommandRemove,        .pNext = NULL},
        {.pName = "vol",     .MainFunc = AmbaShell_CommandVolumn,        .pNext = NULL},
        {.pName = "savebin", .MainFunc = AmbaShell_CommandSaveBinary,    .pNext = NULL},
        {.pName = "reboot",  .MainFunc = AmbaShell_CommandReboot,        .pNext = NULL},
        //{.pName = "sdshmoo", .MainFunc = AmbaShell_SdShmoo,              .pNext = NULL},
        {.pName = "thruput", .MainFunc = AmbaShell_CommandFsThruput,     .pNext = NULL},
        {.pName = "date",    .MainFunc = AmbaShell_CommandDate,          .pNext = NULL},
        {.pName = "sysinfo", .MainFunc = AmbaShell_CommandSysInfo,       .pNext = NULL},

#if defined(CONFIG_ICAM_EEPROM_USED)
        {.pName = "eeprom",  .MainFunc = AmbaEEPRomUT_TestCmd,           .pNext = NULL},
#endif
#ifdef CONFIG_ENABLE_AMBALINK
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
        if (tcgetattr(fdShell, &Tio) == 0U) {
            cfsetispeed(&Tio, B115200);
            cfsetospeed(&Tio, B115200);
            // we need to use non-canonical mode and no echo to fit
            // ThreadX Shell implementation.
            Tio.c_lflag &= ~(ECHO | ECHOE | ICANON);
            tcsetattr(fdShell, TCSANOW, &Tio);
        } else {
            SvcLog_NG(SVC_LOG_MAIN, "## fail to get termio info", 0U, 0U);
        }

        CmdNum = sizeof(BasicCmds) / sizeof(AMBA_SHELL_COMMAND_s);
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

        AmbaSvcWrap_MisraMemset(&Config, 0, sizeof(Config));
        Config.CliTaskPriority    = SVC_SHELL_CLI_TASK_PRI;
        Config.CliTaskSmpCoreSet  = SVC_SHELL_CLI_TASK_CPU_BITS;
        Config.pCliTaskStack      = CliTaskStack;
        Config.CliTaskStackSize   = SHELL_CLI_STACK_SIZE;
        Config.ExecTaskPriority   = SVC_SHELL_EXEC_TASK_PRI;
        Config.ExecTaskSmpCoreSet = SVC_SHELL_EXEC_TASK_CPU_BITS;
        Config.pExecTaskStack     = ExecTaskStack;
        Config.ExecTaskStackSize  = SHELL_EXEC_STACK_SIZE;
        Config.GetCharFunc        = CharGet;
        Config.PutCharFunc        = CharPut;
        Rval = AmbaShell_Init(&Config);
        if (SHELL_ERR_SUCCESS != Rval) {
            SvcLog_NG(SVC_LOG_MAIN, "## fail to init shell", 0U, 0U);
        }
    } else {
        SvcLog_NG(SVC_LOG_MAIN, "fail to open uart port", 0U, 0U);
    }

    SvcLog_DBG(SVC_LOG_MAIN, "## shell init end", 0U, 0U);
}

static void PreDrvEntry(void)
{
    if (AmbaSYS_DrvEntry() != OK) {
        SvcLog_NG(SVC_LOG_MAIN, "## fail to AmbaSYS_DrvEntry()", 0U, 0U);
    }

    if (AmbaDRAMC_DrvEntry() != OK) {
        SvcLog_NG(SVC_LOG_MAIN, "## fail to AmbaDRAMC_DrvEntry()", 0U, 0U);
    }
}

static void PostDrvEntry(void)
{
    /* module probe */
#if defined (CONFIG_BUILD_CV)
    if(access( "/dev/ambacv", F_OK) == -1) {
        if (system("modprobe ambacv_sdk") != 0) {
            SvcLog_NG(SVC_LOG_MAIN, "## fail to modprobe ambacv_sdk", 0U, 0U);
        }
    }
#endif
    if (access("/dev/ambadsp", F_OK) == -1) {
        if (system("modprobe ambadsp_sdk") != 0) {
            SvcLog_NG(SVC_LOG_MAIN, "## fail to modprobe ambadsp_sdk", 0U, 0U);
        }
    }

    if (AmbaPIO_DrvEntry() != OK) {
        SvcLog_NG(SVC_LOG_MAIN, "## fail to AmbaPIO_DrvEntry()", 0U, 0U);
    }

    if (AmbaTMR_DrvEntry() != OK) {
        SvcLog_NG(SVC_LOG_MAIN, "## fail to AmbaTMR_DrvEntry()", 0U, 0U);
    }

    if (AmbaI2C_DrvEntry() != OK) {
        SvcLog_NG(SVC_LOG_MAIN, "## fail to AmbaI2C_DrvEntry()", 0U, 0U);
    }

    if (AmbaSD_DrvEntry() != OK) {
        SvcLog_NG(SVC_LOG_MAIN, "## fail to AmbaSD_DrvEntry()", 0U, 0U);
    }

    //if (AmbaSPI_DrvEntry() != OK) {
    //    SvcLog_NG(SVC_LOG_MAIN, "## fail to AmbaSPI_DrvEntry()", 0U, 0U);
    //}

    if (AmbaVIN_DrvEntry() != OK) {
        SvcLog_NG(SVC_LOG_MAIN, "## fail to AmbaVIN_DrvEntry()", 0U, 0U);
    }

    if (AmbaVout_DrvEntry() != OK) {
        SvcLog_NG(SVC_LOG_MAIN, "## fail to AmbaVout_DrvEntry()", 0U, 0U);
    }

    if (AmbaRTC_DrvEntry() != OK) {
        SvcLog_NG(SVC_LOG_MAIN, "## fail to AmbaRTC_DrvEntry()", 0U, 0U);
    }

    if (AmbaPWM_DrvEntry() != OK) {
        SvcLog_NG(SVC_LOG_MAIN, "## fail to AmbaPWM_DrvEntry()", 0U, 0U);
    }

#if defined(CONFIG_ICAM_HDMI_EXIST)
    if (AmbaHDMI_DrvEntry() != OK) {
        SvcLog_NG(SVC_LOG_MAIN, "## fail to AmbaHDMI_DrvEntry()", 0U, 0U);
    }
#endif

#if defined(CONFIG_ICAM_CVBS_EXIST)
    if (AmbaCVBS_DrvEntry() != OK) {
        SvcLog_NG(SVC_LOG_MAIN, "## fail to AmbaCVBS_DrvEntry()", 0U, 0U);
    }
#endif

#ifdef CONFIG_ICAM_AUDIO_USED
    if(access( "/dev/snd", F_OK) == -1) {
        int Rval;

        printf("Install Auido...\n");
        Rval = system("modprobe snd-soc-core pmdown_time=300");
        if (Rval != 0) {
            printf("modprobe snd-soc-core fail %d.\n", Rval);
        }
        Rval = system("modprobe snd-soc-ambarella");
        if (Rval != 0) {
            printf("modprobe snd-soc-ambarella fail %d.\n", Rval);
        }
        Rval = system("modprobe snd-soc-ambarella-i2s");
        if (Rval != 0) {
            printf("modprobe snd-soc-ambarella-i2s fail %d.\n", Rval);
        }
#ifdef CONFIG_AUDIO_AK4951EN
        Rval = system("modprobe snd-soc-ak4951-amb");
        if (Rval != 0) {
            printf("modprobe snd-soc-ak4951-amb fail %d.\n", Rval);
        }
#endif
#ifdef CONFIG_AUDIO_NAU8822
        Rval = system("modprobe snd-soc-nau8822");
        if (Rval != 0) {
            printf("modprobe snd-soc-nau8822 fail %d.\n", Rval);
        }
#endif
        Rval = system("modprobe snd-soc-ambdummy");
        if (Rval != 0) {
            printf("modprobe snd-soc-ambdummy fail %d.\n", Rval);
        }
        Rval = system("modprobe snd-soc-simple-card");
        if (Rval != 0) {
            printf("modprobe snd-soc-simple-card fail %d.\n", Rval);
        }
        Rval = system("amba_audio&");
        if (Rval != 0) {
            printf("amba_audio fail %d.\n", Rval);
        }
    }
#endif

    /* dirty pages flushing setting, for i/o performance improvement */
    {
        /* enable laptop mode */
        if (system("echo 1 > /proc/sys/vm/laptop_mode") != 0) {
            SvcLog_NG(SVC_LOG_MAIN, "## fail to echo /proc/sys/vm/laptop_mode", 0U, 0U);
        }

        /* i/o will be blocked when when the dirty pages reach 80% system memory */
        if (system("echo 80 > /proc/sys/vm/dirty_ratio") != 0) {
            SvcLog_NG(SVC_LOG_MAIN, "## fail to echo /proc/sys/vm/dirty_ratio", 0U, 0U);
        }

        /* background monitor will be actived for every 10 * 10ms = 100ms */
        if (system("echo 10 > /proc/sys/vm/dirty_writeback_centisecs") != 0) {
            SvcLog_NG(SVC_LOG_MAIN, "## fail to echo /proc/sys/vm/dirty_writeback_centisecs", 0U, 0U);
        }

        /* backgroud task will start to flush dirty data when its size > CONFIG_AMBA_VFS_BANK_SIZE */
        {
        #ifdef CONFIG_AMBA_VFS_BANK_SIZE
            UINT32    Bytes = (UINT32)CONFIG_AMBA_VFS_BANK_SIZE;
        #else
            UINT32    Bytes = 0x200000;
        #endif

            UINT32    CurStrLen;
            char      Cmd[128U] = {0};

            AmbaUtility_StringAppend(Cmd, (UINT32)sizeof(Cmd), "echo ");

            CurStrLen = AmbaUtility_StringLength(Cmd);
            if (CurStrLen < (UINT32)sizeof(Cmd)) {
                CurStrLen = AmbaUtility_UInt32ToStr(&(Cmd[CurStrLen]), (UINT32)sizeof(Cmd) - CurStrLen, Bytes, 10U);
            }

            AmbaUtility_StringAppend(Cmd, (UINT32)sizeof(Cmd), " > /proc/sys/vm/dirty_background_bytes");

            if (system(Cmd) != 0) {
                SvcLog_NG(SVC_LOG_MAIN, "## fail to echo /proc/sys/vm/laptop_mode", 0U, 0U);
            }
        }
    }
}

static void SignalHandler(int SigNum)
{
    if ((SigNum == SIGINT) || (SigNum == SIGTERM) || (SigNum == SIGSEGV)) {
        g_SigNum = SigNum;
    }
}

static void SignalInit(void)
{
    struct sigaction  NewAct;

    NewAct.sa_handler = SignalHandler;
    sigemptyset(&NewAct.sa_mask);
    NewAct.sa_flags = 0;

    if (sigaction(SIGINT, &NewAct, NULL) == -1) {
        perror("fail to hook SIGINT\n");
    }

    if (sigaction(SIGTERM, &NewAct, NULL) == -1) {
        perror("fail to hook SIGTERM\n");
    }

    if (sigaction(SIGSEGV, &NewAct, NULL) == -1) {
        perror("fail to hook SIGTERM\n");
    }

    printf("## SignalInit\n");
}

static int iCamEntry(void)
{
    /* signal handler initialization */
    SignalInit();

    /* process restart handling */
    iCamRecovery_Proc();

    #if defined(CONFIG_ICAM_TIMING_LOG)
    /* time profile initialization */
    iCamTimeProfile_Init();
    iCamTimeProfile_Set(SVC_TIME_POSTOS_CONFIG);
    #endif
    /* init drivers */
    PreDrvEntry();

    #if defined(CONFIG_SVC_ENABLE_WDT)
    /* AmbaWDT_DrvEntry will also stop the watchdog which is started in bootloader,
       and make a clean start here. */
    if (AmbaWDT_DrvEntry() != OK) {
        SvcLog_NG(SVC_LOG_MAIN, "## fail to AmbaWDT_DrvEntry()", 0U, 0U);
    } else {
        SvcLog_OK(SVC_LOG_MAIN, "## AmbaWDT_DrvEntry init OK, pause watchdog", 0U, 0U);
    }
    #endif

    SvcClock_Init(CONFIG_ICAM_CLK_LINE);
    SvcClock_Dump();

    SvcWdt_AppWdtStart();
    SvcWdt_AppWdtTaskCreate();

    PostDrvEntry();

    /* create init task */
    SvcInitTask_Create();

    /* init shell */
    ShellInit();

    while (1) {
        if ((g_SigNum == SIGINT) || (g_SigNum == SIGTERM) || (g_SigNum == SIGSEGV)) {
            SvcLog_DBG(SVC_LOG_MAIN, "Receive SIG %u", (UINT32)g_SigNum, 0U);

            #ifdef CONFIG_BUILD_CV
            /* stop cv here */
            {
                UINT32 Err;

                //SvcCvMainTask_Ctrl("schdr_reset", NULL);

                Err = SvcCvCtrlTask_Stop();
                if (Err != SVC_OK) {
                    SvcLog_NG(SVC_LOG_MAIN, "SvcCvMainTask_Ctrl failed", Err, 0U);
                }

                Err = SvcCvMainTask_Stop();
                if (Err != SVC_OK) {
                    SvcLog_NG(SVC_LOG_MAIN, "SvcCvMainTask_Stop failed", Err, 0U);
                }
            }
            #endif

            SvcLog_DBG(SVC_LOG_MAIN, "Terminate the process", 0U, 0U);
            AmbaPrint_Flush();

            break;
        } else {
            AmbaKAL_TaskSleep(10000U);
        }
    }

    return 0;
}

int main(int argc, char **argv)
{
    int   Rval = 0;
    pid_t cpid = fork();

    AmbaMisra_TouchUnused(&argc);
    AmbaMisra_TouchUnused(argv);

    if (cpid == 0) {
        /* child process */
        AmbaKAL_TaskSleep(2U);

        fprintf(stderr, "amba_svc is started, pid %u\n", getpid());

        Rval = iCamEntry();

        fprintf(stderr, "amba_svc is going to be terminated, pid %u\n", getpid());
    } else if (cpid > 0) {
        /* parent process */
        static char ProcessName[] = "amba_par";
        int         Status = 0, wpid;

        /* rename parent process name */
        prctl(PR_SET_NAME, ProcessName);
        strncpy(argv[0], ProcessName, strlen(argv[0]));

        wpid = wait(&Status);

        fprintf(stderr, "amba_svc is already terminated, pid %u, status %d\n", wpid, Status);
    } else {
        fprintf(stderr, "fork failed, cpid = %d\n", cpid);
    }

    return Rval;
}
