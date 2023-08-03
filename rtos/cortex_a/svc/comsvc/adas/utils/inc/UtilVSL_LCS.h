/**
 *  @file UtilVSL_LCS.h
 *
 * Copyright (c) 2021 Ambarella International LP
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
 *  @details Header file of the utility for LCS visualization
 *
 */

#ifndef UTIL_VSL_LCS_H
#define UTIL_VSL_LCS_H


#include "AmbaAP_LCS.h"
#include "UtilVSL_Common.h"

UINT32 UtilVSL_DrawLcsPathPlan(UINT8* pBuffer, UINT32 BufWidth, UINT32 BufHeight, const AMBA_SEG_CROP_INFO_s* pCropInfo, const AMBA_AP_LCS_RESULT_s* pLcsData);

UINT32 UtilVSL_DrawLcsGradualPathPlan(UINT8** pRgbBuffer, UINT32 BufWidth, UINT32 BufHeight, const AMBA_SEG_CROP_INFO_s* pCropInfo, const AMBA_AP_LCS_RESULT_s* pLcsData);

UINT32 UtilVSL_DrawLcsPathPlanScore2Alpha(UINT8* pAlphaBuffer, UINT32 BufWidth, UINT32 BufHeight, const AMBA_SEG_CROP_INFO_s* pCropInfo, const AMBA_AP_LCS_RESULT_s* pLcsData);

UINT32 UtilVSL_DrawLcsPathPlanWithRgba(UINT8* pBuffer, UINT8* pRgbaBuffer, UINT32 BufWidth, UINT32 BufHeight, const AMBA_SEG_CROP_INFO_s* pCropInfo, const AMBA_AP_LCS_RESULT_s* pLcsData);

UINT32 UtilVSL_DrawBEVLcsPath(UINT8 *pBuffer, const UTIL_VSL_BEV_CFG_s* pBevCfg, const AMBA_AP_LCS_RESULT_s* pLcsData);

UINT32 UtilVSL_DrawLcsPathScore(UINT8* pBuffer, UINT32 BufWidth, UINT32 BufHeight, const AMBA_SEG_CROP_INFO_s* pCropInfo, const AMBA_AP_LCS_RESULT_s* pLcsData);

UINT32 UtilVSL_DrawCanbusTurnLight(UINT8* pBuffer, UINT32 BufWidth, UINT32 BufHeight, const AMBA_SR_CANBUS_TRANSFER_DATA_s* pCanbusInfo);

UINT32 UtilVSL_DrawLcsSteeringInfo(UINT8 *pBuffer, UINT32 BufWidth, UINT32 BufHeight, const AMBA_SEG_CROP_INFO_s* pCropInfo, const AMBA_AP_LCS_STEER_INFO_s* pSteerInfo);

UINT32 UtilVSL_DrawCanbusSteering(UINT8* pBuffer, UINT32 BufWidth, UINT32 BufHeight, const AMBA_SR_CANBUS_TRANSFER_DATA_s* pCanbusInfo, const AMBA_VEHICLE_PHYSICAL_CTRL_s* pPhySpec);

UINT32 UtilVSL_DrawLcsAccInfo(UINT8 *pBuffer, UINT32 BufWidth, UINT32 BufHeight, const AMBA_SR_CANBUS_TRANSFER_DATA_s* pCanbusInfo, const AMBA_AP_LCS_GAS_BRAKE_INFO_s* pGBInfo, const AMBA_AP_LCS_ACCELERATION_CFG_s* pAccCfg);

UINT32 UtilVSL_DrawLcsTargetInfo(UINT8 *pBuffer, UINT32 BufWidth, UINT32 BufHeight, UINT32 SrcWidth, UINT32 SrcHeight, const AMBA_OD_2DBBX_LIST_s *pBbxList, UINT32 TargetID);

#endif
