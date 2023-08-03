/**
 *  @file SvcAdasAutoCal.h
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
 *  @details Svc auto calibration flow
 *
 */

#ifndef SVC_ADAS_AUTO_CAL_H
#define SVC_ADAS_AUTO_CAL_H

#define SVC_ADAS_AUTOCAL_MSG_DEPTH       (4U)

typedef struct {
    AMBA_SR_LANE_RECONSTRUCT_INFO_s LaneDetectionData;              /**< Current lane detection data */
    AMBA_SR_CANBUS_TRANSFER_DATA_s CanbusData;                      /**< Optional. Car can-bus data. It would help auto calibration to get better result */
} SVC_ADAS_AUTOCAL_LD_INFO_s;

typedef struct {
    UINT32 BufferIdx;
    ULONG  DataBufferAddr;
} SVC_ADAS_AUTOCAL_MSG_s;


void SvcAdasAutoCal_PrintDouble5(const char* pStrFmt, DOUBLE Arg1, DOUBLE Arg2, DOUBLE Arg3, DOUBLE Arg4, DOUBLE Arg5, UINT32 AfterPoint);
UINT32 SvcAdasAutoCalTask_Init(void);
UINT32 SvcAdasAutoCal_MsgSend(const SVC_ADAS_AUTOCAL_LD_INFO_s *pAdasAutoCalInput);

void SvcAdasAutoCal_SetupCam(const AMBA_CAL_CAM_s *pCam);
void SvcAdasAutoCal_SetupFocalLength(const DOUBLE FocalLength);
void SvcAdasAutoCal_SetupRawRoi(const AMBA_CAL_ROI_s *pRawRoi);
void SvcAdasAutoCal_SetupVoutView(const AMBA_CAL_ROI_s *pVoutView);
void SvcAdasAutoCal_SetupSegRoi(const AMBA_CAL_ROI_s *pSegRoi);
void SvcAdasAutoCal_SetupDefCalibPoints(const AMBA_CT_EM_CALIB_POINTS_s *InCalibPoints);
void SvcAdasAutoCal_SetupCurrCalInfo(const AMBA_AUTOCAL_CALIB_INFO_s *pCurrCalInfo);


#endif /* SVC_ADAS_AUTO_CAL_H */
