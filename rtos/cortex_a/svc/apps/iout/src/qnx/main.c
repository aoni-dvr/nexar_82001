/**
 *  @file main.c
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
 *  THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 *  WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL
 *  AMBARELLA, INC. OR ITS AFFILIATES BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 *  DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 *  COMPUTER FAILURE OR MALFUNCTION; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 *  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 *  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *  @details Main function for SVC application.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <AmbaUtility.h>
#include <AmbaShell.h>
#include <AmbaShell_Commands.h>
#include <AmbaDrvEntry.h>
#include <unistd.h>
#include <termios.h>
#include <ctype.h>
#include "AmbaIOUTDiag.h"

static int fd = -1;

static UINT32 char_get(UINT32 RxDataSize, char *pRxDataBuf, UINT32 TimeOut)
{
    (void)TimeOut;
    return (UINT32)read(fd, pRxDataBuf, RxDataSize);
}

/* AMBA_SHELL_PUT_CHAR_f definition */
static  UINT32 char_put(UINT32 TxDataSize, const char *pTxDataBuf, UINT32 TimeOut)
{
    (void)TimeOut;
    return (UINT32)write(fd, pTxDataBuf, TxDataSize);
}

static AMBA_SHELL_COMMAND_s shell_commands[] = {
    {.pName = "help",     .MainFunc = AmbaShell_CommandHelp, .pNext = NULL},
    {.pName = "date",     .MainFunc = AmbaShell_CommandDate, .pNext = NULL},
    {.pName = "cd",       .MainFunc = AmbaShell_CommandChangeDir, .pNext = NULL},
    {.pName = "ls",       .MainFunc = AmbaShell_CommandListDir, .pNext = NULL},
    {.pName = "cat",      .MainFunc = AmbaShell_CommandCat, .pNext = NULL},
    {.pName = "copy",     .MainFunc = AmbaShell_CommandCopy, .pNext = NULL},
    {.pName = "format",   .MainFunc = AmbaShell_CommandFormat, .pNext = NULL},
    {.pName = "mkdir",    .MainFunc = AmbaShell_CommandMakeDir, .pNext = NULL},
    {.pName = "mv",       .MainFunc = AmbaShell_CommandMove, .pNext = NULL},
    {.pName = "readl",    .MainFunc = AmbaShell_CommandReadLong, .pNext = NULL},
    {.pName = "writel",   .MainFunc = AmbaShell_CommandWriteLong, .pNext = NULL},
    {.pName = "reboot",   .MainFunc = AmbaShell_CommandReboot, .pNext = NULL},
    {.pName = "rm",       .MainFunc = AmbaShell_CommandRemove, .pNext = NULL},
    {.pName = "sleep",    .MainFunc = AmbaShell_CommandSleep, .pNext = NULL},
    {.pName = "thruput",  .MainFunc = AmbaShell_CommandFsThruput, .pNext = NULL},
    {.pName = "exec",     .MainFunc = AmbaShell_CommandExec, .pNext = NULL},
    {.pName = "diag",     .MainFunc = AmbaIOUTDiag_CmdDiag, .pNext = NULL},

//    {.pName = "ps", .MainFunc = AmbaShell_CommandProcessStatus, .pNext = NULL},
};

static void usage(void)
{
    printf("usage:\n");
    printf("amba_iout -p [uart_port]\n");
    printf("    [uart_port]: 1/2/3...\n");
}

static int options(int argc, char *argv[])
{
    int cmd_opt = 0;
    unsigned long port_idx = 1;
    static char port_name[50];
    unsigned int flag_exit = 0;

    while ((optind < argc) && (flag_exit == 0U)) {
        cmd_opt = getopt(argc, argv, "p::");

        /* End condition always first */
        if (cmd_opt == -1) {
            break;
        }

        switch(cmd_opt) {
        case 'p':
            port_idx = strtoul(optarg, NULL, 0);
            if (port_idx == 0U) {
                port_idx = 1;
            }
            break;
        default:
            usage();
            flag_exit = 1;
            break;
        }
    }

    if (flag_exit == 0U) {
        snprintf(port_name, 50, "/dev/ser%ld", port_idx);
        fd = open(port_name, O_RDWR);
        if (fd == -1) {
            printf("could not open %s\n", port_name);
        }
    }
    return fd;
}

int main(int argc, char *argv[])
{
#define SHELL_TEST_EXEC_BUF_SIZE          (0x60000U)
    static UINT8 ShellTestExecTaskStack[SHELL_TEST_EXEC_BUF_SIZE] GNU_SECTION_NOZEROINIT;

    AMBA_SHELL_CONFIG_s shell_config;
    UINT32 uret;
    UINT32 command_count = sizeof(shell_commands) / sizeof(AMBA_SHELL_COMMAND_s);
    UINT32 i;

    (void)argc;
    (void)argv;

    (void)AmbaDrvEntry();

    fd = options(argc, argv);
    if (fd != -1) {
        struct termios tio;
        if (tcgetattr(fd, &tio) == 0) {
            // we need to use non-canonical mode and no echo to fit
            // ThreadX Shell implementation.
            tio.c_lflag &= ~(ECHO | ECHOE | ICANON);
            tcsetattr(fd, TCSANOW, &tio);
        } else {
            printf("can't get termio info.\n");
        }

        shell_config.pExecTaskStack = ShellTestExecTaskStack;
        shell_config.ExecTaskStackSize = SHELL_TEST_EXEC_BUF_SIZE;
        shell_config.GetCharFunc = char_get;
        shell_config.PutCharFunc = char_put;

        for (i = 0; i < command_count; i++) {
            (void)AmbaShell_CommandRegister(&shell_commands[i]);
        }

        uret = AmbaShell_Init(&shell_config);
        if (uret != 0U) {
            printf("could not initialize AmbaShell.\n");
        } else {
            // the shell thread is executed, main thread should not exit.
            while (1) {
                sleep(1);
            }
        }
    }
    return 0;
}
