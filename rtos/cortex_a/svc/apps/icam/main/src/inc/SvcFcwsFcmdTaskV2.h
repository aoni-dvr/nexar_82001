/**
 *  @file SvcFcwsFcmdTaskV2.h
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
 *  @details Svc FCWS and FCMD task header file (V2)
 *
 */

#ifndef SVC_FCWS_FCMD_TASK_V2_H
#define SVC_FCWS_FCMD_TASK_V2_H

#define SVC_FC_RELATIVE_SPEED_STRIDE_V2  (5)     /* m/s per level*/
#define SVC_FC_TTC_M_V2                  (2400)  /* Each level for the unit meter */
#define SVC_FC_WARNING_LOWER_BOUND_V2    (30)     /* km/hr */
#define FC_IN_DATA_V2_3D_MAX             (150U)

#define FC_ACC_ACTIVESPEED    (18.0)
#define FC_MAX_ACCELERATION   (2.0)
#define FC_MIN_ACCELERATION   (-3.5)

typedef struct {
    UINT32 MsgCode;
    UINT32 Source;
    UINT32 CaptureTime;
    UINT32 FrameNum;
    UINT32 BbxAmount;
    SVC_CV_DETRES_BBX_WIN_s *Bbx;
    const char ** class_name;
    UINT32 Chan;
    UINT32 CvType;
} SVC_FCWS_FCMD_BBX_LIST_V2_s;

typedef struct {
    REF_FLOW_FC_PRE_PROC_IN_V2_s  *pPreProcIn;
    REF_FLOW_FC_PRE_PROC_OUT_V2_s *pPreProcOut;
} SVC_FC_PROC_IN_DATA_V2_s;

typedef struct {
    UINT32 Chan;
    UINT32 CvType;
    SVC_CV_PERCEPTION_OUTPUT_s *pPcptOut;
} SVC_FC_PROC_2Stage_INFO_s;

typedef struct {
    SVC_CV_DETRES_BBX_LIST_s BbxOut;
    AMBA_SR_SROBJECT_DATA_s  SROut;
    REF_FLOW_FC_OUT_DATA_s   FCOut;
    UINT32 Chan;
    UINT32 CvType;
    REF_FLOW_FC_SR_IMAGE_3D_MODEL_s Info3Dbbx;
    SVC_CV_PERCEPTION_OUTPUT_s PcptOut;
} SVC_FC_DRAW_INFO_V2_s;

UINT32 SvcFcTaskV2_Init(void);
UINT32 SvcFcTaskV2_Start(void);
UINT32 SvcFcTaskV2_Stop(void);
UINT32 SvcFcTaskV2_GetStatus(UINT32 *pEnable);

void SvcFcTaskV2_WarningIcanEnable(void);
void SvcFcTaskV2_WarningIcanDisable(void);

#endif /* SVC_FCWS_FCMD_TASK_V2_H */
