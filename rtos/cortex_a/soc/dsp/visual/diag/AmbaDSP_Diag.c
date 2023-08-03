/**
*  @file AmbaDSP_Diag.c
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
*  @details Amba DSP Driver Diagnosis APIs
*
*/

#include "AmbaDSP_Diag.h"
#include "AmbaDSP_ArmComm.h"
#include "AmbaDSP_Cmds.h"
#include "AmbaDSP_ContextUtility.h"
#include "AmbaDSP_Log.h"
#include "dsp_diag_priv_api.h"

//#define DEBUG_DIAG_LOG

static inline void Diag_PrintErrLine(UINT32 Rval, const char *fn, UINT32 LineNum)
{
    if (Rval != 0U) {
        AmbaPrint_PrintStr5("%s got NG result", fn, NULL, NULL, NULL, NULL);
        AmbaPrint_PrintUInt5("    Rval = 0x%x @ %d", Rval, LineNum, 0U, 0U, 0U);
    }
}

static inline UINT32 AmbaDSP_DiagCmdReset(void)
{
    UINT32 Rval;
    const UINT32 ResetVal = 0xFFFFFFFFU;
    UINT32 *pVoid;
    ULONG cmdAddr;

    Rval = DSP_GetFirstCmdAddr(AMBA_DSP_SYNC_CMD_BUF_VDSP0, &cmdAddr);
    Diag_PrintErrLine(Rval, __func__, __LINE__);
    if (Rval == OK) {
        dsp_osal_typecast(&pVoid, &cmdAddr);
        Rval = dsp_osal_memset(pVoid, (INT32)ResetVal, sizeof(UINT32));
        Diag_PrintErrLine(Rval, __func__, __LINE__);
    }

    Rval = DSP_GetFirstCmdAddr(AMBA_DSP_SYNC_CMD_BUF_VDSP1, &cmdAddr);
    Diag_PrintErrLine(Rval, __func__, __LINE__);
    if (Rval == OK) {
        dsp_osal_typecast(&pVoid, &cmdAddr);
        Rval = dsp_osal_memset(pVoid, (INT32)ResetVal, sizeof(UINT32));
        Diag_PrintErrLine(Rval, __func__, __LINE__);
    }
    return Rval;
}

static inline UINT32 AmbaDSP_DiagMsgReset(void)
{
    UINT32 Rval;
    const UINT32 ResetVal = 0xFFFFFFFFU;
    UINT32 *pVoid;
    ULONG msgAddr;

    Rval = DSP_GetNextMsgAddr(AMBA_DSP_SYNC_CMD_BUF_VDSP0, &msgAddr);
    Diag_PrintErrLine(Rval, __func__, __LINE__);
    if (Rval == OK) {
        dsp_osal_typecast(&pVoid, &msgAddr);
        Rval = dsp_osal_memset(pVoid, (INT32)ResetVal, sizeof(UINT32));
        Diag_PrintErrLine(Rval, __func__, __LINE__);
    }

    Rval = DSP_GetNextMsgAddr(AMBA_DSP_SYNC_CMD_BUF_VDSP1, &msgAddr);
    Diag_PrintErrLine(Rval, __func__, __LINE__);
    if (Rval == OK) {
        dsp_osal_typecast(&pVoid, &msgAddr);
        Rval = dsp_osal_memset(pVoid, (INT32)ResetVal, sizeof(UINT32));
        Diag_PrintErrLine(Rval, __func__, __LINE__);
    }
    return Rval;
}

static inline UINT32 AmbaDSP_DiagOverflowCmdBuf(void)
{
    UINT32 Rval = DSP_ERR_0001;
    UINT8 CmdBufferId = 0U; void *CmdBufferAddr = NULL;
    cmd_vproc_echo_t *pCmd = HL_DefCtxCmdBufPtrVpcEcho;
    UINT32 i;

    for (i = 0U; i < (DSP_MAX_NUM_GENERAL_CMD * AMBA_MAX_NUM_DSP_GENERAL_CMD_BUF); i++) {
        HL_AcqCmdBuffer(&CmdBufferId, &CmdBufferAddr);
        dsp_osal_typecast(&pCmd, &CmdBufferAddr);
        pCmd->channel_id = 0U;
        pCmd->grp_cmd_buf_id = 0U;

        Rval = AmbaHL_CmdVprocEchoCmd(AMBA_DSP_CMD_VCAP_NORMAL_WRITE, pCmd);
        HL_RelCmdBuffer(CmdBufferId);
        if (Rval != OK) {
            // Internal write command error: buffer is full
            if (Rval == DSP_ERR_0007) {
                Rval = OK;
            }
            break;
        }
    }
    return Rval;
}

static inline UINT32 AmbaDSP_DiagOverflowMsgBuf(void)
{
    UINT32 Rval;
    UINT32 ReadPtrNew, LogMode, PrintMode, MaxMsgNum, *pReadPtr, *pWritePtr;

    // Enable AMBALLLOG_TYPE_DBG
    Rval = AmbaLL_LogGetMode(&LogMode, &PrintMode);
    Diag_PrintErrLine(Rval, __func__, __LINE__);
    PrintMode |= AMBALLLOG_TYPE_DBG;
    Rval = AmbaLL_LogSetMode(LogMode, PrintMode);
    Diag_PrintErrLine(Rval, __func__, __LINE__);

    Rval = DSP_GetMsgPtrData(AMBA_DSP_SYNC_CMD_BUF_VDSP0, &MaxMsgNum, &pReadPtr, &pWritePtr);
    Diag_PrintErrLine(Rval, __func__, __LINE__);
    if (Rval == OK) {
        ReadPtrNew = (*pWritePtr + DEFAULT_MSGBUF_OVERFLOW_ASSERT - 1U) % MaxMsgNum;
#ifdef DEBUG_DIAG_LOG
        AmbaPrint_PrintUInt5(" [DSP Diag] OverflowMsgBuf[%d] ReadPtr[%d->%d] WritePtr[%d]"
            , AMBA_DSP_SYNC_CMD_BUF_VDSP0, *pReadPtr, ReadPtrNew, *pWritePtr, 0U);
#endif
        *pReadPtr = ReadPtrNew;
    }

    Rval = DSP_GetMsgPtrData(AMBA_DSP_SYNC_CMD_BUF_VDSP1, &MaxMsgNum, &pReadPtr, &pWritePtr);
    Diag_PrintErrLine(Rval, __func__, __LINE__);
    if (Rval == OK) {
        ReadPtrNew = (*pWritePtr + DEFAULT_MSGBUF_OVERFLOW_ASSERT - 1U) % MaxMsgNum;
#ifdef DEBUG_DIAG_LOG
        AmbaPrint_PrintUInt5(" [DSP Diag] OverflowMsgBuf[%d] ReadPtr[%d->%d] WritePtr[%d]"
            , AMBA_DSP_SYNC_CMD_BUF_VDSP0, *pReadPtr, ReadPtrNew, *pWritePtr, 0U);
#endif
        *pReadPtr = ReadPtrNew;
    }
    return Rval;
}

/**
* Trigger diagnosis
* @param [in]  DiagId Diagnostic index, see DSPDIAG_CASE_NUM
* @return 0 - OK, others - ErrorCode
*/
UINT32 dsp_diag_case(UINT32 DiagId)
{
    UINT32 Rval;

    switch (DiagId) {
        case DSPDIAG_CMD_RESET:
            {
                Rval = AmbaDSP_DiagCmdReset();
            }
            break;
        case DSPDIAG_MSG_RESET:
            {
                Rval = AmbaDSP_DiagMsgReset();
            }
            break;
        case DSPDIAG_CMD_OVERFLOW:
            {
                Rval = AmbaDSP_DiagOverflowCmdBuf();
            }
            break;
        case DSPDIAG_MSG_OVERFLOW:
            {
                Rval = AmbaDSP_DiagOverflowMsgBuf();
            }
            break;
        case DSPDIAG_STOP_UCODE:
            {
                extern UINT32 dsp_stop(UINT32 CmdType);
                Rval = dsp_stop(AMBA_DSP_CMD_VDSP_NORMAL_WRITE);
            }
            break;
        default:
            Rval = DSP_ERR_0001;
            AmbaPrint_PrintUInt5("[Err @ AmbaDiagCase] Unexpected case id [0x%X]", DiagId, 0U, 0U, 0U, 0U);
            break;
    }
    return Rval;
}

