/**
*  @file iCamRecovery.c
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
*  @details icam recovery
*
*/

#include "AmbaTypes.h"
#include "AmbaMisraFix.h"
#include "AmbaShell.h"
#include "AmbaDSP.h"
#include "AmbaDSP_VideoEnc.h"

#include "SvcLog.h"
#include "SvcErrCode.h"
#include "SvcSysEvent.h"
#include "SvcDSP.h"
#include "SvcTaskList.h"
#include "iCamRecovery.h"

#define SVC_LOG_RECV        "RECV"

static void RecRecovery(void)
{
#if defined(CONFIG_ICAM_RECORD_USED)
    UINT8   StopOptArr[CONFIG_ICAM_MAX_REC_STRM] = {0};
    UINT16  StrmIdxArr[CONFIG_ICAM_MAX_REC_STRM];
    UINT32  i, NumStrm = 0U, Data0 = 0xFFFFFFFFU, Data1 = 0U;
    UINT64  RawSeq[AMBA_DSP_MAX_STREAM_NUM] = {0};

    if (SvcDSP_IsCleanBoot() == SVCDSP_OP_DIRTYBOOT) {
        for (i = 0U; i < (UINT32)CONFIG_ICAM_MAX_REC_STRM; i++) {
            Data0 = 0xFFFFFFFFU;
            if (AmbaDSP_GetStatus(DSP_STATUS_TYPE_ENC_MODE, i, &Data0, &Data1) != OK) {
                SvcLog_DBG(SVC_LOG_RECV, "## fail to AmbaDSP_GetStatus", 0U, 0U);
            }

            if (Data0 == 1U) {
                StopOptArr[i] = AMBA_DSP_VIDEO_ENC_STOP_IMMEDIATELY;
                StrmIdxArr[i] = i;
                NumStrm++;
            }
        }

        if (0U < NumStrm) {
            if (AmbaDSP_VideoEncStop(NumStrm, StrmIdxArr, StopOptArr, RawSeq) != OK) {
                SvcLog_DBG(SVC_LOG_RECV, "## fail to stop encoder", 0U, 0U);
            }
        }
    }
#endif
}

static void LiveviewRecovery(void)
{
    if (SvcDSP_IsCleanBoot() == SVCDSP_OP_DIRTYBOOT) {
        SvcSysEvent_Reset();
        SvcDSP_DspMsgTaskCreate(SVC_DSP_MSG_TASK_PRI, SVC_DSP_MSG_TASK_CPU_BITS);
    }
}




/**
 *  Set value of status to temporarily memory
 *  @return none
 */
void iCamRecovery_Proc(void)
{
    /* stop encoders if running */
    SvcLog_DBG(SVC_LOG_RECV, "## RecRecovery ...", 0U, 0U);
    RecRecovery();

    /* liveview recovery if running */
    SvcLog_DBG(SVC_LOG_RECV, "## LiveviewRecovery ...", 0U, 0U);
    LiveviewRecovery();
}
