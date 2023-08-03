/**
 * @file main.c
 *  RPMSG test app: echo
 *
 * @defgroup ipc title
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
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include <fcntl.h>

#include <unistd.h>

#include <sys/types.h>

#include <time.h>

#ifdef CONFIG_QNX
#include <devctl.h>
#include <sys/stat.h>
#include <sys/procmgr.h>
#endif // CONFIG_QNX

#include "AmbaTypes.h"

#include "AmbaIPC.h"

INT32 echo_MsgHandler(AMBA_IPC_HANDLE IpcHandle, AMBA_IPC_MSG_CTRL_s *pMsgCtrl)
{
    printf("%s, %d, handle: %p, Len: %d, Msg: %s\n", __func__, __LINE__,
           IpcHandle, pMsgCtrl->Length, (char *)(pMsgCtrl->pMsgData));
    AmbaIPC_Send(IpcHandle, (char *)(pMsgCtrl->pMsgData),  pMsgCtrl->Length);
    return 0;
}

int32_t main(int32_t argc, char *argv[])
{
    AMBA_IPC_HANDLE hdl = 0;
    char chname[32] = "g2_echo";
    int opt;

    while ((opt = getopt(argc, argv, "hfvn:")) != -1) {
        switch (opt) {
        case 'h':
            fprintf (stderr, "    -f        runs in foreground to allow stdin \n");
            fprintf (stderr, "    -v        enable logging \n");
            fprintf (stderr, "    -n g2_echo    specify channel name \n");
            opt = -1;
            break;
        case 'f':
            {
                extern int rpmsg_api_close_stdin;
                rpmsg_api_close_stdin = 0;
            }
            break;
        case 'v':
            {
                extern int rpmsg_api_verbose;
                rpmsg_api_verbose = 1;
            }
            break;
        case 'n':
            strcpy(chname, optarg);
            break;
        default: /* '?' */
            fprintf (stderr, "    -f    runs in foreground to allow stdin \n");
            fprintf (stderr, "    -v    enable logging \n");
            fprintf (stderr, "    -n    channel name \n");
            exit(0);
        }
    }

    printf("channel name %s\n", chname);

    hdl = AmbaIPC_Alloc(RPMSG_DEV_AMBA, chname, echo_MsgHandler);
    AmbaIPC_RegisterChannel(hdl, NULL);
    AmbaIPC_Send(hdl, chname, strlen(chname));

    //procmgr_daemon(EXIT_SUCCESS, (PROCMGR_DAEMON_NODEVNULL | PROCMGR_DAEMON_NOCLOSE));
    /* wait for remote message, never exit */
    while (1) {
        sleep(60);
    }

    return 0;
}
