/**
 *  @file main.c
 *
 *  @copyright Copyright (c) 2021 Ambarella, Inc.
 *
 *  This file and its contents ("Software") are protected by intellectual property rights including, without limitation,
 *  U.S. and/or foreign copyrights.  This Software is also the confidential and proprietary information of Ambarella, Inc.
 *  and its licensors.  You may not use, reproduce, disclose, distribute, modify, or otherwise prepare derivative works
 *  of this Software or any portion thereof except pursuant to a signed license agreement or nondisclosure agreement with
 *  Ambarella, Inc. or its authorized affiliates.  In the absence of such an agreement, you agree to promptly notify and
 *  return this Software to Ambarella, Inc.
 *
 *  THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 *  WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL
 *  AMBARELLA, INC. OR ITS AFFILIATES BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 *  DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 *  COMPUTER FAILURE OR MALFUNCTION; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 *  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 *  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *  @details Linux IOUT main function
 *
 */

#include "AmbaTypes.h"
#include "AmbaKAL.h"
#include <AmbaShell.h>
#include <AmbaShell_Commands.h>
#include <AmbaShell_Utility.h>
#include "AmbaPrint.h"
#include "AmbaUtility.h"
#include "AmbaIOUTDiag.h"
#include "AmbaDrvEntry.h"
#include <unistd.h>
#include <termios.h>
#include "AmbaDDRC.h"

#if defined(CONFIG_A53_CONSOLE_AHB_2)
#define TERMINAL_NAME   "/dev/ttyS3"
#elif defined(CONFIG_A53_CONSOLE_AHB_1)
#define TERMINAL_NAME   "/dev/ttyS2"
#else   /* DEFAULT */
#define TERMINAL_NAME   "/dev/ttyS1"
#endif
static int terminal_fd = -1;

static UINT32 char_get(UINT32 RxDataSize, char *pRxDataBuf, UINT32 TimeOut)
{
    (void)TimeOut;
    return (UINT32)read(terminal_fd, pRxDataBuf, RxDataSize);
}

/* AMBA_SHELL_PUT_CHAR_f definition */
static  UINT32 char_put(UINT32 TxDataSize, const char *pTxDataBuf, UINT32 TimeOut)
{
    (void)TimeOut;
    return (UINT32)write(terminal_fd, pTxDataBuf, TxDataSize);
}

static void AmbaUserConsole_Write(UINT32 StringSize, const char *StringBuf, UINT32 TimeOut)
{
    char_put(StringSize, StringBuf, TimeOut);
}

static AMBA_SHELL_COMMAND_s shell_commands[] = {
//    {.pName = "cat",      .MainFunc = AmbaShell_CommandCat, .pNext = NULL},
    {.pName = "cd",       .MainFunc = AmbaShell_CommandChangeDir, .pNext = NULL},
//    {.pName = "cp",       .MainFunc = AmbaShell_CommandCopy, .pNext = NULL},
    {.pName = "date",     .MainFunc = AmbaShell_CommandDate, .pNext = NULL},
    {.pName = "diag",     .MainFunc = AmbaIOUTDiag_CmdDiag, .pNext = NULL},
    {.pName = "format",   .MainFunc = AmbaShell_CommandFormat, .pNext = NULL},
    {.pName = "eject",   .MainFunc = AmbaShell_CommandEject, .pNext = NULL},
    {.pName = "help",     .MainFunc = AmbaShell_CommandHelp, .pNext = NULL},
    {.pName = "ls",       .MainFunc = AmbaShell_CommandListDir, .pNext = NULL},
    {.pName = "mkdir",    .MainFunc = AmbaShell_CommandMakeDir, .pNext = NULL},
    {.pName = "mv",       .MainFunc = AmbaShell_CommandMove, .pNext = NULL},
//    {.pName = "ps",       .MainFunc = AmbaShell_CommandProcessStatus, .pNext = NULL},
    {.pName = "randvrfy", .MainFunc = AmbaShell_CommandRandvrfy, .pNext = NULL},
    {.pName = "readl",    .MainFunc = AmbaShell_CommandReadLong, .pNext = NULL},
//    {.pName = "reboot",   .MainFunc = AmbaShell_CommandReboot, .pNext = NULL},
    {.pName = "rm",       .MainFunc = AmbaShell_CommandRemove, .pNext = NULL},
//    {.pName = "sdshmoo",  .MainFunc = AmbaShell_SdShmoo, .pNext = NULL},
    {.pName = "sleep",    .MainFunc = AmbaShell_CommandSleep, .pNext = NULL},
//    {.pName = "sysinfo",  .MainFunc = AmbaShell_CommandSysInfo, .pNext = NULL},
    {.pName = "thruput",  .MainFunc = AmbaShell_CommandFsThruput, .pNext = NULL},
//    {.pName = "ver",      .MainFunc = AmbaShell_CommandVersion, .pNext = NULL},
//    {.pName = "vol",      .MainFunc = AmbaShell_CommandVolumn, .pNext = NULL},
    {.pName = "writel",   .MainFunc = AmbaShell_CommandWriteLong, .pNext = NULL},
    {.pName = "savebin",  .MainFunc = AmbaShell_CommandSaveBinary, .pNext = NULL},
//    {.pName = "atf",      .MainFunc = AmbaShell_CommandAtf,       .pNext = NULL },
//    {.pName = "iostat",   .MainFunc = AmbaShell_CommandIoStat,    .pNext = NULL},
//    {.pName = "sysinfo",  .MainFunc = AmbaShell_CommandSysInfo,   .pNext = NULL},
};

static uint32_t terminal_setup(void)
{
    uint32_t uret = 0;
    terminal_fd = open(TERMINAL_NAME, O_RDWR);
    if (terminal_fd == -1) {
        printf("could not open %s.\n", TERMINAL_NAME);
        uret = 1;
    } else {
        struct termios tio;
        if (tcgetattr(terminal_fd, &tio) == 0) {
            // set baudrate to 115200
            cfsetispeed(&tio, B115200);
            cfsetospeed(&tio, B115200);
            // we need to use non-canonical mode and no echo to fit
            // ThreadX Shell implementation.
            tio.c_lflag &= ~(ECHO | ECHOE | ICANON);
            tcsetattr(terminal_fd, TCSANOW, &tio);
            tcflush(terminal_fd, TCIOFLUSH);
        } else {
            printf("can't get termio info.\n");
            uret = 1;
        }
    }
    return uret;
}

#define UT_PRINT_BUF_SIZE               (262144U)
#define UT_PRINT_LOG_BUF_SIZE           (1048576U)
#define UT_PRINT_STACK_SIZE             (4096U)
#define UT_PRINT_TASK_CORE_MASK         (0x1U)

#define TASK_PRI_IOUT_SHELL_CLI     (AMBA_KAL_TASK_LOWEST_PRIORITY - 5U)
#define TASK_PRI_IOUT_SHELL_EXEC    (AMBA_KAL_TASK_LOWEST_PRIORITY - 6U)
#define TASK_PRI_IOUT_PRINT         (AMBA_KAL_TASK_LOWEST_PRIORITY)
#define SHELL_EXEC_BUFFER_SIZE      (0x40000U)

int main(int argc, char *argv[])
{
    AMBA_SHELL_CONFIG_s shell_config;
    UINT32 uret,Rval;
    UINT32 command_count = sizeof(shell_commands) / sizeof(AMBA_SHELL_COMMAND_s);
    UINT32 i;
    AMBA_PRINT_CONFIG_s PrintConfig = {0};
    static UINT8 UtSysInitPrintBuffer[UT_PRINT_BUF_SIZE] GNU_SECTION_NOZEROINIT;
    static UINT8 UtSysInitPrintLogBuffer[UT_PRINT_LOG_BUF_SIZE] GNU_SECTION_NOZEROINIT;
    static UINT8 UtSysInitPrintTaskStack[UT_PRINT_STACK_SIZE] GNU_SECTION_NOZEROINIT;
    static UINT8 AmbaSysInitCliTaskStack[4096U] __attribute__((section(".bss.noinit")));
    static UINT8 AmbaSysInitExecTaskStack[SHELL_EXEC_BUFFER_SIZE] __attribute__((section(".bss.noinit")));

    (void)argv;
    if (argc > 1) {
        if (daemon(1, 1) < 0) {
        }
    }

    /* module probe */
    if(access("/sys/module/ambavio_sdk", F_OK) == -1) {
        Rval = system("modprobe ambavio_sdk");
        if (Rval != 0) {
            printf("modprobe ambavio_sdk fail %d.\n", Rval);
        }
    }

    /* IO DRVs */
    AmbaSYS_DrvEntry();
    AmbaGPIO_DrvEntry();
    AmbaI2C_DrvEntry();
    AmbaSD_DrvEntry();
    AmbaSPI_DrvEntry();
    AmbaUART_DrvEntry();
    AmbaGDMA_DrvEntry();
    AmbaPIO_DrvEntry();
    AmbaPWM_DrvEntry();
    AmbaVIN_DrvEntry();
    AmbaWDT_DrvEntry();
#if defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)
    AmbaDRAMC_DrvEntry();
    AmbaDDRC_DrvEntry();
#endif

    /* Init AmbaPrint */
    PrintConfig.TaskPriority = AMBA_KAL_TASK_LOWEST_PRIORITY;
    PrintConfig.SmpCoreSet = UT_PRINT_TASK_CORE_MASK;
    PrintConfig.PrintBufferSize = UT_PRINT_BUF_SIZE;
    PrintConfig.LogBufferSize = UT_PRINT_LOG_BUF_SIZE;
    PrintConfig.pPrintBuffer = UtSysInitPrintBuffer;
    PrintConfig.pLogBuffer = UtSysInitPrintLogBuffer;
    PrintConfig.TaskStackSize = UT_PRINT_STACK_SIZE;
    PrintConfig.pTaskStack = UtSysInitPrintTaskStack;
    PrintConfig.PutCharFunc = AmbaUserConsole_Write;

    Rval = AmbaPrint_Init(&PrintConfig);
    if (Rval != OK) {
        printf("[UT] PrintInit fail[0x%X]", Rval);
    } else {
        AmbaPrint_PrintUInt5("[UT] Print init done", 0U, 0U, 0U, 0U, 0U);
    }

    if (terminal_setup() == 0U) {
        shell_config.GetCharFunc = char_get;
        shell_config.PutCharFunc = char_put;
        shell_config.CliTaskPriority = TASK_PRI_IOUT_SHELL_CLI;
        shell_config.CliTaskSmpCoreSet = 1U;
        shell_config.pCliTaskStack = AmbaSysInitCliTaskStack;
        shell_config.CliTaskStackSize = 4096U;
        shell_config.ExecTaskPriority = TASK_PRI_IOUT_SHELL_EXEC;
        shell_config.ExecTaskSmpCoreSet = 2U;
        shell_config.pExecTaskStack = AmbaSysInitExecTaskStack;
        shell_config.ExecTaskStackSize = SHELL_EXEC_BUFFER_SIZE;

        for (i = 0; i < command_count; i++) {
            (void)AmbaShell_CommandRegister(&shell_commands[i]);
        }

        uret = AmbaShell_Init(&shell_config);
        if (uret != 0U) {
            printf("could not initialize AmbaShell.\n");
        } else {
            while(1) {
                (void)AmbaKAL_TaskSleep(1000);
            }
        }
    }

    return 0;
}
