/**
 *  @file amba_camsvc.c
 *
 *  @copyright Copyright (c) 2018 Ambarella, Inc.
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
 *  @details Implementation of Ambarella Cam Service API test
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

//#include "cvtask_api.h"

#include "aipc_user.h"
#include "AmbaIPC_RpcProg_RT_CamSvc.h"

#include "camsvc.h"

#define PrintFunc printf

#define ErrMsg(fmt,...) fprintf(stderr, "%s: "fmt"\n",__FUNCTION__,##__VA_ARGS__)
#define Debug(fmt,...)  printf("%s: "fmt"\n",__FUNCTION__,##__VA_ARGS__)


#define NG (-1)
#define OK (0)

typedef struct ambaipc_clnt_s {
    int host;
    int prog;
    int ver;
    int timeout;
} ambaipc_clnt_t;

static ambaipc_clnt_t camsvc_prog =
{
    RT_CAMSVC_HOST,
    RT_CAMSVC_PROG_ID,
    RT_CAMSVC_VER,
    RT_CAMSVC_DEFULT_TIMEOUT
};

static CLIENT_ID_t clnt_camsvc_handler = 0;

static int CheckClientId(void)
{
    int status = 0;

    if (clnt_camsvc_handler == 0) {
        clnt_camsvc_handler = ambaipc_clnt_create(camsvc_prog.host,
                                              camsvc_prog.prog,
                                              camsvc_prog.ver);
        if (clnt_camsvc_handler==0) {
            ErrMsg("Fail to create client.");
            status = -1;
        } else {
            status = 0;
        }
    }

    return status;
}

uint32_t CamSvc_GetAeCurrLv(uint32_t ViewID, uint16_t *pLvNo)
{
    int status;
    unsigned int in;
    RT_CamSvc_AeCurrLv_s res;
    uint32_t rval = 1;

    if (pLvNo == NULL) {
        ErrMsg("Invalid Input. pLvNo=%p", pLvNo);
        return 1;
    }

    status = CheckClientId();
    if (status == 0) {
        in = ViewID;
        status = ambaipc_clnt_call(clnt_camsvc_handler, RT_CAMSVC_FUNC_GETAECURRLV,
                &in, sizeof(unsigned int),
                &res, sizeof(RT_CamSvc_AeCurrLv_s),
                camsvc_prog.timeout);
        if (status != 0) {
            ErrMsg("fail to do ambaipc_clnt_call(). %d", status);
        } else {
            rval = res.Res;
            *pLvNo = res.CurrLv;
        }
    } else {
        ErrMsg("Fail to create client.");
    }

    return rval;
}

int main(int Argc, char *Argv[])
{
    int Rval = NG;
    int ShowHelp = 1;

    if (Argc >= 2) {
        if (strcmp(Argv[1], "aelv") == OK) {
            uint32_t ViewID, Res;
            uint16_t LvNo;

            if (Argc >= 3) {
                ViewID = atoi(Argv[2]);

                Res = CamSvc_GetAeCurrLv(ViewID, &LvNo);
                PrintFunc("CamSvc_GetAeCurrLv() for ViewID %u returns %u, LvNo:%u\n", ViewID, Res, LvNo);
                if (Res == 0) {
                    Rval = OK;
                }
                ShowHelp = 0;
            }
        }
    }

    if(ShowHelp) {
        PrintFunc("Usage: %s [FUNCTION]\n",Argv[0]);
        PrintFunc("Function:\n");
        PrintFunc("\t aelv [ViewID]\n");
    }

    return Rval;
}

