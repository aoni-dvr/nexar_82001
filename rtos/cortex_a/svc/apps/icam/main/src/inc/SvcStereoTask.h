/**
 *  @file SvcStereoTask.h
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
 *  @details svc stereo task header
 *
 */

#ifndef SVC_STEREO_TASK_H
#define SVC_STEREO_TASK_H

#define SVC_STEREO_MAX_INPUT_PER_CHAN       (2U)
#define SVC_STEREO_MAX_DATA_PER_INPUT       (6U)
#define SVC_STEREO_MAX_FILE_PATH            (64U)

#define SVC_STEREO_OSD_DISABLE               (0x0U)
#define SVC_STEREO_OSD_ENABLE                (0x1U)

typedef struct {
    UINT32  CvFlowChan;
} SVC_STEREO_CONFIG_s;

typedef struct {
    UINT32  Width;
    UINT32  Height;
    UINT32  Pitch;
    UINT32  ScaleIdx;
} SVC_STEREO_IMG_INFO_s;

typedef struct {
  SVC_STEREO_IMG_INFO_s Img;
} SVC_STEREO_DATA_INFO_s;

typedef struct {
    UINT32  Reserved;
} SVC_STEREO_START_s;

UINT32 SvcStereoTask_Init(void);
UINT32 SvcStereoTask_Config(void);
UINT32 SvcStereoTask_Start(void);
UINT32 SvcStereoTask_Stop(void);
UINT32 SvcStereoTask_GetStatus(UINT32 *pEnable);
UINT32 SvcStereoTask_Ctrl(const char *pCmd, void *pParam, UINT32 Value);
UINT32 SvcStereoTask_SetRateCtrl(const UINT32 *RateDivisor);
UINT32 SvcStereoTask_EnableOSD(UINT32 Enable);
UINT32 SvcStereoTask_QueryBufSize(UINT32 *pMemSize);
void SvcStereoTask_DebugEnable(UINT32 DebugEnable);

UINT32 SvcStereoTask_FileIn_Config(const SVC_STEREO_CONFIG_s *pConfig);
UINT32 SvcStereoTask_StartSpuFexBinary(const char* InFile);
UINT32 SvcStereoTask_StartFusionBinary(UINT32 InputNum, const char* InFile1, const char* InFile2, const char* InFile3);
void SvcStereoTask_SetOsdEnable(UINT32 Enable);

#endif /* SVC_STEREO_TASK_H */
