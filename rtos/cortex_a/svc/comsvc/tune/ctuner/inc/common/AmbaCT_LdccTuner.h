/**
 *  @file AmbaCT_LdccTuner.h
 *
 * Copyright (c) 2020 Ambarella International LP
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
 */
#ifndef AMBA_CT_LDCC_TUNER_H
#define AMBA_CT_LDCC_TUNER_H

#include "AmbaTypes.h"
#include "AmbaCT_LdccTunerIF.h"
typedef enum {
    AMBA_CT_LDCC_TUNER_SYSTEM_INFO = 0,
    AMBA_CT_LDCC_TUNER_SRC_INFO,
    AMBA_CT_LDCC_TUNER_SP_PAT_CIRCLE_DET,
    AMBA_CT_LDCC_TUNER_MP_PAT_CIRCLE_DET,
    AMBA_CT_LDCC_TUNER_MP_PAT_SEPARATE,
    AMBA_CT_LDCC_TUNER_MP_PAT_DET_CENTER_ROI,
    AMBA_CT_LDCC_TUNER_SP_ORGANIZE_CORNER,
    AMBA_CT_LDCC_TUNER_MP_ORGANIZE_CORNER,
    AMBA_CT_LDCC_TUNER_CAMERA,
    AMBA_CT_LDCC_TUNER_OPTICAL_CENTER,
    AMBA_CT_LDCC_TUNER_MP_PAT_LAYOUT_CFG,
    AMBA_CT_LDCC_TUNER_SP_OC_USED_RANGE,
    AMBA_CT_LDCC_TUNER_SP_CAL_DISTOR_CFG,
    AMBA_CT_LDCC_TUNER_MP_CAL_DISTOR_CFG,
    AMBA_CT_LDCC_TUNER_MAX,
} AMBA_CT_LDCC_TUNER_GROUP_e;


void AmbaCT_LdccGetSystem(AMBA_CT_LDCC_SYSTEM_s *pData);
void AmbaCT_LdccSetSystem(const AMBA_CT_LDCC_SYSTEM_s *pData);
void AmbaCT_LdccGetSrcInfo(AMBA_CT_LDCC_SRC_s *pData);
void AmbaCT_LdccSetSrcInfo(const AMBA_CT_LDCC_SRC_s *pData);
void AmbaCT_LdccGetSinglePatDetCircleCfg(AMBA_CT_LDCC_PAT_DET_CIR_s *pData);
void AmbaCT_LdccSetSinglePatDetCircleCfg(const AMBA_CT_LDCC_PAT_DET_CIR_s *pData);
void AmbaCT_LdccGetMultiPatDetCircleCfg(AMBA_CT_LDCC_PAT_DET_CIR_s *pData);
void AmbaCT_LdccSetMultiPatDetCircleCfg(const AMBA_CT_LDCC_PAT_DET_CIR_s *pData);
void AmbaCT_LdccGetMultiPatDetCenterROI(AMBA_CAL_ROI_s *pData);
void AmbaCT_LdccSetMultiPatDetCenterROI(const AMBA_CAL_ROI_s *pData);
void AmbaCT_LdccGetSinglePlaneOrganizeCornerCfg(AMBA_CAL_OCCB_ORGANIZE_CFG_s *pData);
void AmbaCT_LdccSetSinglePlaneOrganizeCornerCfg(const AMBA_CAL_OCCB_ORGANIZE_CFG_s *pData);
void AmbaCT_LdccGetMultiPlaneOrganizeCornerCfg(UINT32 ArrayIndex, AMBA_CAL_OCCB_ORGANIZE_CFG_s *pData);
void AmbaCT_LdccSetMultiPlaneOrganizeCornerCfg(UINT32 ArrayIndex, const AMBA_CAL_OCCB_ORGANIZE_CFG_s *pData);
void AmbaCT_LdccGetCamera(AMBA_CT_LDCC_CAMERA_s *pData);
void AmbaCT_LdccSetCamera(const AMBA_CT_LDCC_CAMERA_s *pData);
void AmbaCT_LdccGetOpticalCenter(AMBA_CT_LDCC_OPTICAL_CENTER_s *pData);
void AmbaCT_LdccSetOpticalCenter(const AMBA_CT_LDCC_OPTICAL_CENTER_s *pData);
void AmbaCT_LdccGetPatternLayoutCfg(UINT32 ArrayIndex, AMBA_CT_LDCC_PAT_LAYOUT_CFG_s **pData);
void AmbaCT_LdccSetPatternLayoutCfg(UINT32 ArrayIndex, const AMBA_CT_LDCC_PAT_LAYOUT_CFG_s *pData);
void AmbaCT_LdccGetSPOCUsedRange(AMBA_CAL_OCCB_USED_GRID_SIZE_s *pData);
void AmbaCT_LdccSetSPOCUsedRange(const AMBA_CAL_OCCB_USED_GRID_SIZE_s *pData);
void AmbaCT_LdccGetSPCalDistor(AMBA_CT_LDCC_SP_CAL_DISTOR_CFG_s *pData);
void AmbaCT_LdccSetSPCalDistor(const AMBA_CT_LDCC_SP_CAL_DISTOR_CFG_s *pData);
void AmbaCT_LdccGetMPCalDistor(AMBA_CT_LDCC_MP_CAL_DISTOR_CFG_s *pData);
void AmbaCT_LdccSetMPCalDistor(const AMBA_CT_LDCC_MP_CAL_DISTOR_CFG_s *pData);


UINT8 AmbaCT_LdccGetGroupStatus(UINT8 GroupId);
void AmbaCT_LdccGetWorkingBufSize(SIZE_t *pSize);
UINT32 AmbaCT_LdccTunerInit(const AMBA_CT_LDCC_INIT_CFG_s *pInitCfg);
UINT32 AmbaCT_LdccExecute(void);
#endif
