/**
 *  @file SvcLdwsTaskV2.h
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
 *  @details Svc LDWS V2 task header file
 *
 */

#ifndef SVC_LDWS_TASK_V2_H
#define SVC_LDWS_TASK_V2_H

#define SVC_LDWS_V2_SEG_WIDTH   (320U)
#define SVC_LDWS_V2_SEG_HEIGHT  (128U)
//
#define SVC_LD_V2_WARNING_LOWER_BOUND  (30U)  /* km/hr */
//
//#define SEG_CLASS_CURB 2
//#define SEG_CLASS_LANEMARKING_Y 3
//#define SEG_CLASS_LANEMARKING_W 4
//#define SEG_CLASS_LANEMARKING_R 5
//
//typedef struct {
//    UINT32 RawCapPts;
//    UINT32 RawCapLastPts;
//    UINT64 RawCapPts64;
//    UINT64 RawCapPtsBase;
//} SVC_LDWS_CV_TIMER_s;
//
//#if defined(CONFIG_ICAM_PROJECT_ADAS_DVR) && defined(CONFIG_BUILD_AMBA_ADAS)
//typedef struct {
//    UINT32 LaneInfoIsAssign;
//    UINT32 LKAInfoIsAssign;
//    UINT32 LKADebugFlag;
//    AMBA_SR_LANE_RECONSTRUCT_INFO_s LaneInfo;
//    AMBA_AP_LKA_RESULT_s            LKAInfo;
//    AMBA_AP_LKA_CONFIG_s            LKAConfigInfo;
//    AMBA_SR_CANBUS_TRANSFER_DATA_s  CanbusTrData;
//} SVC_LDWS_V2_OUT_s;
//#endif

UINT32 SvcLdwsTaskV2_Init(void);
UINT32 SvcLdwsTaskV2_Start(void);
UINT32 SvcLdwsTaskV2_Stop(void);
UINT32 SvcLdwsTaskV2_GetStatus(UINT32 *pEnable);
void   SvcLdwsTaskV2_EnableAutoCal(UINT32 Enable);
UINT32 SvcLdwsTaskV2_GetAutoCalStatus(void);
void   SvcLdwsTaskV2_EnableLKA(UINT32 Enable);
void SvcLdwsTaskV2_LKADebugEnable(UINT32 Enable);
void SvcLdwsTaskV2_GetLanInfo(AMBA_SR_LANE_RECONSTRUCT_INFO_s *pLanInfo);
void SvcLdwsTaskV2_GetLKAfo(AMBA_AP_LKA_RESULT_s *pLKAInfo);

#endif /* SVC_LDWS_TASK_V2_H */
