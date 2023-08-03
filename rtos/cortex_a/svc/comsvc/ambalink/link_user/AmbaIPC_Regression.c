/**
 * @file AmbaIPC_Regression.c
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
 * @details Test/Reference code for AmbaIPC
 *
 */
#include "AmbaTypes.h"
#include "AmbaPrint.h"
#include "AmbaKAL.h"

#include "AmbaLink.h"
#include "AmbaLinkPrivate.h"
#include "AmbaIPC_List.h"
#include "AmbaIPC_Rpc.h"
#include "AmbaIPC_RpcProg_LU_Regression.h"

#include "AmbaIPC_Regression.h"

UINT32 AmbaIPC_RegrStart(void)
{
    CLIENT_ID_t clnt = NULL;
    UINT32 Rval = 0;

    clnt = AmbaIPC_ClientCreate(LU_REGRESSION_HOST, LU_REGRESSION_PROG_ID, LU_REGRESSION_VER);
    if (clnt == NULL) {
        AmbaPrint_PrintStr5("%s: Client creation failed\n",__FUNCTION__,NULL,NULL,NULL,NULL);
        Rval = 11;
    } else {
        AMBA_IPC_REPLY_STATUS_e status;
        UINT32 ret;

        status = AmbaIPC_ClientCall(clnt, LU_REGRESSION_FUNC_START,
                        NULL, 0,
                        &ret, sizeof(UINT32),
                        FOREVER);
        if (status != 0) {
            AmbaPrint_PrintUInt5("fail to do LU_REGRESSION_FUNC_START! (status=%u)",status,0,0,0,0);
            Rval = 12;
        } else {
            //AmbaPrint_PrintUInt5("start: status=%u, ret=%u",status, ret, 0, 0, 0);
            Rval = ret;
        }

        AmbaIPC_ClientDestroy(clnt);
    }

    return Rval;
}

UINT32 AmbaIPC_RegrStop(void)
{
    CLIENT_ID_t clnt = NULL;
    UINT32 Rval = 0;

    clnt = AmbaIPC_ClientCreate(LU_REGRESSION_HOST, LU_REGRESSION_PROG_ID, LU_REGRESSION_VER);
    if (clnt == NULL) {
        AmbaPrint_PrintStr5("%s: Client creation failed\n",__FUNCTION__,NULL,NULL,NULL,NULL);
        Rval = 11;
    } else {
        AMBA_IPC_REPLY_STATUS_e status;
        UINT32 ret;

        status = AmbaIPC_ClientCall(clnt, LU_REGRESSION_FUNC_STOP,
                        NULL, 0,
                        &ret, sizeof(UINT32),
                        FOREVER);
        if (status != 0) {
            AmbaPrint_PrintUInt5("fail to do LU_REGRESSION_FUNC_STOP! (status=%u)",status,0,0,0,0);
            Rval = 12;
        } else {
            //AmbaPrint_PrintUInt5("stop: status=%u, ret=%u",status, ret, 0, 0, 0);
            Rval = ret;
        }

        AmbaIPC_ClientDestroy(clnt);
    }

    return Rval;
}

UINT32 AmbaIPC_RegrWrite(UINT32 FrameId, UINT32 AlgoType, UINT32 PhyAddr, UINT32 Size)
{
    CLIENT_ID_t clnt = NULL;
    UINT32 Rval = 0;

    clnt = AmbaIPC_ClientCreate(LU_REGRESSION_HOST, LU_REGRESSION_PROG_ID, LU_REGRESSION_VER);
    if (clnt == NULL) {
        AmbaPrint_PrintStr5("%s: Client creation failed\n",__FUNCTION__,NULL,NULL,NULL,NULL);
        Rval = 11;
    } else {
        AMBA_IPC_REPLY_STATUS_e status;
        UINT32 ret;
        LU_REGRESSION_WRITERESULT_ARG_s RegrWriteRes = {0};

        RegrWriteRes.Index = FrameId;
        RegrWriteRes.Type = AlgoType;
        RegrWriteRes.Amount = 1;
        RegrWriteRes.Data[0].PhyAddr = PhyAddr;
        RegrWriteRes.Data[0].Size = Size;

        status = AmbaIPC_ClientCall(clnt, LU_REGRESSION_FUNC_WRTITERESULT,
                        &RegrWriteRes, sizeof(LU_REGRESSION_WRITERESULT_ARG_s),
                        &ret, sizeof(UINT32),
                        FOREVER);
        if (status != 0) {
            AmbaPrint_PrintUInt5("fail to do LU_REGRESSION_FUNC_WRTITERESULT! (status=%u)",status,0,0,0,0);
            Rval = 12;
        } else {
            //AmbaPrint_PrintUInt5("write: status=%u, ret=%u",status, ret, 0, 0, 0);
            Rval = ret;
        }

        AmbaIPC_ClientDestroy(clnt);
    }

    return Rval;
}

UINT32 AmbaIPC_RegrSetMemRegion(UINT32 PhyAddr, UINT32 Size)
{
    CLIENT_ID_t clnt = NULL;
    UINT32 Rval = 0;

    clnt = AmbaIPC_ClientCreate(LU_REGRESSION_HOST, LU_REGRESSION_PROG_ID, LU_REGRESSION_VER);
    if (clnt == NULL) {
        AmbaPrint_PrintStr5("%s: Client creation failed\n",__FUNCTION__,NULL,NULL,NULL,NULL);
        Rval = 11;
    } else {
        AMBA_IPC_REPLY_STATUS_e status;
        LU_REGRESSION_DATA_BLK_s DramRegion = {0};
        UINT32 ret;

        DramRegion.PhyAddr = PhyAddr;
        DramRegion.Size = Size;

        status = AmbaIPC_ClientCall(clnt, LU_REGRESSION_FUNC_SETMEMREGION,
                        &DramRegion, sizeof(LU_REGRESSION_DATA_BLK_s),
                        &ret, sizeof(UINT32),
                        FOREVER);
        if (status != 0) {
            AmbaPrint_PrintUInt5("fail to do LU_REGRESSION_FUNC_SETMEMREGION! (status=%u)",status,0,0,0,0);
            Rval = 12;
        } else {
            //AmbaPrint_PrintUInt5("SetMemRegion: status=%u, ret=%u",status, ret, 0, 0, 0);
            Rval = ret;
        }

        AmbaIPC_ClientDestroy(clnt);
    }

    return Rval;
}
