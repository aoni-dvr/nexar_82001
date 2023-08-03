/**
*  @file AmbaADecFlow.h
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
*  @details amba audio decode flow
*
*/

#ifndef AMBA_ADEC_FLOW_H
#define AMBA_ADEC_FLOW_H

typedef struct {
    AMBA_AUDIO_OUTPUT_s          *pAoutInfo;
    AMBA_AUDIO_DEC_s             *pADecInfo;
    AMBA_AUDIO_SETUP_INFO_s      *pADecSetup;
} AMBA_ADEC_FLOW_INFO_s;

void AmbaADecFlow_InfoGet(AMBA_ADEC_FLOW_INFO_s *pInfo);

void   AmbaADecFlow_Setup(UINT32 Type, UINT32 Source, ULONG MPly, ADEC_DATA_HDLR_CB ADecCB, ULONG *pDecId);
void   AmbaADecFlow_DecStart(ULONG DecId);
void   AmbaADecFlow_OupStart(ULONG DecId, AMBA_AOUT_CTRL_t pCbAoutCtrl);
void   AmbaADecFlow_OupStop(ULONG DecId, AMBA_AOUT_CTRL_t pCbAoutCtrl);
void   AmbaADecFlow_OupPause(ULONG DecId, AMBA_AOUT_CTRL_t pCbAoutCtrl);
void   AmbaADecFlow_OupResume(ULONG DecId, AMBA_AOUT_CTRL_t pCbAoutCtrl);
void   AmbaADecFlow_Stop(ULONG DecId, UINT32 IsWaitAEos, AMBA_AOUT_CTRL_t pCbAoutCtrl);
void   AmbaADecFlow_Delete(ULONG DecId);
void   AmbaADecFlow_FeedTaskCreate(ULONG DecId);
void   AmbaADecFlow_FeedTaskDelete(ULONG DecId);
UINT32 AmbaADecFlow_UpdateBsBufRp(ULONG DecId, UINT8 *pBsAddr, UINT32 UpdateSize);
UINT32 AmbaADecFlow_WaitEvent(UINT32 Event, void *pParam);
void *AmbaADecFlow_GetDemuxCtrlHdlr(void);

#define AMBA_ADEC_EVENT_OUTPUT (1U) /* INPUT: frame number. It will return when the total output frame >= the input. */

#endif  /* AMBA_ADEC_FLOW_H */
