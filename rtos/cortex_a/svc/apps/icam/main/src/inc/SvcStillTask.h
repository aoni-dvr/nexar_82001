/**
 *  @file SvcStillTask.h
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
 *  @details svc still task header file
 *
 */

#ifndef SVC_STILL_TASK_H
#define SVC_STILL_TASK_H

#define SVC_STILL_CAP      (1U)
#define SVC_STILL_PROC     (2U)
#define SVC_STILL_ENC      (4U)
#define SVC_STILL_MUX      (8U)

typedef struct {
    UINT8 SaveYUV;
    UINT8 SaveJPEG;
} SVC_STL_RAWENC_CTRL_s;

UINT32 SvcStillTask_Create(UINT32 TaskSelect);
UINT32 SvcStillTask_Delete(UINT32 TaskSelect);
UINT32 SvcStillTask_Capture(const SVC_CAP_CTRL_s *pCtrl);

#ifndef CONFIG_SOC_H22
UINT32 SvcStillTask_GetIkCtxId(UINT8 IsHiso, UINT8 CeNeeded, UINT32 *pIkCtxId);
UINT32 SvcStillTask_GetCapBuffer(AMBA_DSP_RAW_BUF_s *pRawInfo, AMBA_DSP_BUF_s *pCeInfo, UINT8 IsHiso, UINT8 AlignOutputHeight);
UINT32 SvcStillTask_RawEncode(const SVC_STL_RAWENC_CTRL_s *pCtrl, AMBA_DSP_YUV_IMG_BUF_s *pYuvInfo, AMBA_DSP_ENC_PIC_RDY_s *pJpgInfo);
UINT32 SvcStillTask_RawEncodeAmage(AMBA_DSP_YUV_IMG_BUF_s *pYuvInfo, AMBA_DSP_ENC_PIC_RDY_s *pJpgInfo);
#endif

#endif /* SVC_STILL_TASK_H */
