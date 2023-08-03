/**
 *  @file AmbaRPCStreamWrap.c
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
 *  @details svc stream
 *
 */

#include "AmbaDef.h"
#include "AmbaMisraFix.h"
#include "AmbaUtility.h"
#include "AmbaWrap.h"
#include "AmbaVfs.h"
#include "SvcStream.h"
#include "AmbaIPC_Stream.h"
#include "AmbaLink.h"
#include "AmbaCodecCom.h"
#include "AmbaStreamWrap.h"

/**
* initialization of ipc stream wrap
* @param [in] pfnGetFree function to get free space size
* @return none
*/
void AmbaStreamWrap_Init(PFN_GET_FREE_CB pfnGetFree)
{
    UINT32 Err = 0;

    AmbaMisra_TouchUnused(&pfnGetFree);

    AmbaPrint_ModulePrintUInt5(AMBA_CODEC_PRINT_MODULE_ID, "This is from AmbaRPCStreamWrap.c!!!! AmbaStreamWrap_Init()", 0U, 0U, 0U, 0U, 0U);
    Err = AmbaPrint_ModuleSetAllowList(AMBALINK_MODULE_ID,1);
    if (Err != 0U) {
        AmbaPrint_ModulePrintUInt5(AMBA_CODEC_PRINT_MODULE_ID, "AmbaPrint_ModuleSetAllowList() failed %u", Err, 0U, 0U, 0U, 0U);
    }

    Err = AmbaIPC_StreamInit();
    if (Err != 0U) {
        AmbaPrint_ModulePrintUInt5(AMBA_CODEC_PRINT_MODULE_ID, "AmbaIPC_StreamInit() failed %u", Err, 0U, 0U, 0U, 0U);
    }
}

/**
* handler get of ipc stream wrap
* @param [out] ppvHdlr handler of stream
* @return 0-OK, 1-NG
*/
UINT32 AmbaStreamWrap_GetHdlr(void **ppvHdlr)
{
    UINT32  Rval = STREAM_ERR_0000, Err;

    Err = AmbaIPC_StreamCreate(ppvHdlr);
    if (Err != 0U) {
        AmbaPrint_ModulePrintUInt5(AMBA_CODEC_PRINT_MODULE_ID, "AmbaIPC_StreamCreate() failed %u", Err, 0U, 0U, 0U, 0U);
    } else {
        Rval = OK;
    }

    return Rval;
}

#if defined(CONFIG_ICAM_ENABLE_REGRESSION_FLOW)
/**
* handler delete of ipc stream wrap
* @param [in] pvHdlr handler of stream
* @return 0-OK, 1-NG
*/
UINT32 AmbaStreamWrap_DeleteHdlr(void *pvHdlr)
{
    UINT32  Rval = STREAM_ERR_0000, Err;

    Err = AmbaIPC_StreamDelete(pvHdlr);
    if (Err != 0U) {
        AmbaPrint_ModulePrintUInt5(AMBA_CODEC_PRINT_MODULE_ID, "AmbaIPC_StreamDelete() failed %u", Err, 0U, 0U, 0U, 0U);
    } else {
        Rval = OK;
    }
    return Rval;
}
#endif
