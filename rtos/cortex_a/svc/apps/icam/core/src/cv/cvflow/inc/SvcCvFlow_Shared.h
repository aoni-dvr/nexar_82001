/**
*  @file SvcCvFlow_Shared.h
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
*  @details Header for SVC CvFlow shared function
*/

#ifndef SVC_CV_FLOW_SHARED_H
#define SVC_CV_FLOW_SHARED_H

#define SVC_CV_FLOW_MAX_REC_FRAME                   (10U)

#define SVC_CV_FLOW_TIME_STAMP0                     (0U)
#define SVC_CV_FLOW_TIME_STAMP1                     (1U)
#define SVC_CV_FLOW_TIME_STAMP2                     (2U)
#define SVC_CV_FLOW_MAX_TIME_STAMP                  (3U)
typedef struct {
    UINT8   MaxRecFrm;                              /* [in] Max of frames recorded */
    UINT8   DSRate;                                 /* [in] Downsampling rate */
    UINT64  ProcStartTime;                          /* [in] Time stamp of start process */

    UINT32  ProcTimeMA[SVC_CV_FLOW_MAX_TIME_STAMP - 1U];  /* [out] Moving average processing time in microsecond */
    UINT32  FrameTimeMA;                            /* [out] Moving average frame time in microsecond */

    UINT32  TotalFrm;                               /* [internal] Total of recorded frame */
    UINT8   CurFrmIdx;                              /* [internal] Current frame index */
    UINT64  TimeStamp[SVC_CV_FLOW_MAX_REC_FRAME][SVC_CV_FLOW_MAX_TIME_STAMP]; /* [internal] Time stamp of process */
    UINT64  ProcTime[SVC_CV_FLOW_MAX_REC_FRAME][SVC_CV_FLOW_MAX_TIME_STAMP - 1U];  /* [internal] Processing time */
} SVC_CV_FRAME_TIME_CALC_s;

void SvcCvFlow_GetTimeStamp(const char *pModuleName, UINT64 *pTimeStamp);
void SvcCvFlow_CalcFrameTime(const char *pModuleName, UINT32 NumTS, const UINT64 *pTimeStamp, SVC_CV_FRAME_TIME_CALC_s *pInfo);

#endif  /* SVC_CV_FLOW_SHARED_H */
