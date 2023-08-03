/**
 *  @file AmbaShell_Test.c
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
 *  @details Test Application for Ambarella Shell.
 *
 */

#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <AmbaUtility.h>
#include <AmbaShell.h>
#include <AmbaShell_Commands.h>
#include <unistd.h>
#include <termios.h>

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
    {"help", AmbaShell_CommandHelp, NULL},
    {"date", AmbaShell_CommandDate, NULL},
    {"cd", AmbaShell_CommandChangeDir, NULL},
    {"ls", AmbaShell_CommandListDir, NULL},
    {"cat", AmbaShell_CommandCat, NULL},
    {"copy", AmbaShell_CommandCopy, NULL},
    {"format", AmbaShell_CommandFormat, NULL},
    {"mkdir", AmbaShell_CommandMakeDir, NULL},
    {"mv", AmbaShell_CommandMove, NULL},
    {"readl", AmbaShell_CommandReadLong, NULL},
    {"writel", AmbaShell_CommandWriteLong, NULL},
    {"reboot", AmbaShell_CommandReboot, NULL},
    {"rm", AmbaShell_CommandRemove, NULL},
    {"sleep", AmbaShell_CommandSleep, NULL},
    {"thruput", AmbaShell_CommandFsThruput, NULL},
    {"exec", AmbaShell_CommandExec, NULL},

//    {"ps", AmbaShell_CommandProcessStatus, NULL},
};

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

    fd = open("/dev/ser2", O_RDWR);
    if (fd == -1) {
        printf("could not open /dev/ser2.\n");
    } else {
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
