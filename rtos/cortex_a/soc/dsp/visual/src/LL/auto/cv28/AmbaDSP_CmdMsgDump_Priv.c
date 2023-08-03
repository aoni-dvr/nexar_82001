/**
*  @file AmbaDSP_CmdMsgDump_Priv.c
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
*  @details DSP private command message dump.
*
*/

#include "AmbaDSP_Log.h"
#include "AmbaDSP_Buffers.h"
#include "AmbaDSP_CmdsSend.h"
#include "AmbaDSP_CommonAPI.h"
#include "AmbaDSP_CmdMsgDump.h"

#if defined (CONFIG_ENABLE_DSP_COMMAND_PARM_DBG) || defined (CONFIG_LINUX)
void CmdWriteDump(const char* x, UINT32 CmdCode, UINT32 WrType)
{
    UINT32 Rval;
    char SrcString[128U];
    const char WriteModeName[11][9] = {
        [ 0] = {"DEFAULT "},
        [ 1] = {"VDSP    "},
        [ 2] = {"VDSPGRP "},
        [ 3] = {"VCAP    "},
        [ 4] = {"VCAPGRP "},
        [ 5] = {"VCAP2   "},
        [ 6] = {"VCAP2GRP"},
        [ 7] = {"ASYNC   "},
        [ 8] = {"VCAP3   "},
        [ 9] = {"VCAP3GRP"},
        [10] = {"NOP     "},
};

    Rval = dsp_osal_memset(SrcString, 0, sizeof(char)*128U);
    LL_PrintErrLine(Rval, __func__, __LINE__);
    Rval = dsp_osal_str_copy(SrcString, 128U, x);
    LL_PrintErrLine(Rval, __func__, __LINE__);
    Rval = dsp_osal_str_append(SrcString, 128U, " 0x");
    LL_PrintErrLine(Rval, __func__, __LINE__);
    Rval = dsp_osal_str_append_u32(SrcString, 128U, CmdCode, 16U);
    LL_PrintErrLine(Rval, __func__, __LINE__);
    Rval = dsp_osal_str_append(SrcString, 128U, " [");
    LL_PrintErrLine(Rval, __func__, __LINE__);
    Rval = dsp_osal_str_append(SrcString, 128U, WriteModeName[WrType]);
    LL_PrintErrLine(Rval, __func__, __LINE__);
    Rval = dsp_osal_str_append(SrcString, 128U, "]");
    LL_PrintErrLine(Rval, __func__, __LINE__);

    AmbaLL_Log((UINT32)AMBALLLOG_TYPE_WRAPPER, SrcString, 0U, 0U, 0U);
    return;
}
#endif

