/**
 * @file DemuxerImage.c
 *
 * Copyright (c) 2019 Ambarella International LP
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
 */
#include "DemuxerImpl.h"
#include "../FormatAPI.h"

/**
 * Process function of the demuxer pipe (Image)
 *
 * @param [in] Pipe The demuxer pipe
 * @param [in] FrameNo The format handler
 * @param [out] Event The event got for this processing
 * @return 0 - OK, others - DEMUXER_ERR_XXXX
 */
UINT32 SvcDemuxer_ProcessImage(SVC_DEMUXER_PIPE_HDLR_IMPL_s *Pipe, UINT8 FrameNo, UINT8 *Event)
{
    UINT32 Rval = DEMUXER_OK;
    if (Pipe->State == SVC_DEMUXER_STATE_START) {
        Rval = SvcDemuxer_StartPipe(Pipe);
    } else {
        AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s Incorrect state!", __func__, NULL, NULL, NULL, NULL);
        Rval = DEMUXER_ERR_FATAL_ERROR;
    }
    if (Rval == DEMUXER_OK) {
        UINT32 i;
        const UINT32 FormatCount = Pipe->FormatCount;
        /* ask all formats to process (data empty implies error, because it has been checked before here) */
        for (i = 0; i < FormatCount; i++) {
            SVC_DMX_FORMAT_HDLR_s *Format = Pipe->Format[i];
            UINT8 TempEvent;
            Rval = Format->Func->Process(Format, (UINT64)FrameNo, &TempEvent);
            if (Rval == DEMUXER_OK) {
                if (TempEvent != SVC_DEMUXER_EVENT_NONE) {
                    Rval = DEMUXER_ERR_FATAL_ERROR;
                }
            }
            if (Rval != DEMUXER_OK) {
                AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s Format->Func->Process failed", __func__, NULL, NULL, NULL, NULL);
                break;
            }
        }
    }
    if (Rval == FORMAT_OK) {
        *Event = SVC_DEMUXER_EVENT_REACH_END;
        Rval = SvcDemuxer_OnPipeEnd(Pipe);
    }
    if (Rval != DEMUXER_OK) {
        UINT32 Ret = SvcDemuxer_OnPipeError(Pipe);
        if (Rval == DEMUXER_ERR_IO_ERROR) {
            // more critical issue
            if ((Ret != DEMUXER_OK) && (Ret != DEMUXER_ERR_IO_ERROR)) {
                Rval = Ret;
            }
        }
    }
    return Rval;
}

