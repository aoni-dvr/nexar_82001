/**
 *  @file AmbaCalib_EmirrorAdaptiveIF.h
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
#ifndef AMBA_CALIB_EMIRROR_ADAPTIVE_API_H
#define AMBA_CALIB_EMIRROR_ADAPTIVE_API_H

#include "AmbaCalib_ErrNo.h"
#include "AmbaCalib_Def.h"
#include "AmbaCalib_EmirrorDef.h"
#include "AmbaCalib_EmaDef.h"

UINT32 AmbaCal_EmaSetPrintLv(UINT32 Level);

UINT32 AmbaCal_EmaGetRoiTableSize(const AMBA_CAL_EM_3IN1_VIEW_CFG_V1_s *pView, AMBA_CAL_SIZE_s *pSize);

UINT32 AmbaCal_EmaGetBlendTableSize(const AMBA_CAL_EM_3IN1_VIEW_CFG_V1_s *pView, AMBA_CAL_SIZE_s *pSize);

UINT32 AmbaCal_EmaGenRoiTable(const AMBA_CAL_EM_3IN1_VIEW_CFG_V1_s *pView, const AMBA_CAL_EMA_FILTER_RULE_s *pFilterRule, const AMBA_CAL_EMA_ROI_TBL_s *pRoiTbl);

UINT32 AmbaCal_EmaFindStitchLine(const AMBA_CAL_EM_3IN1_VIEW_CFG_V1_s *pView, AMBA_OD_2DBBX_LIST_s *pBbxInfo[3], const AMBA_CAL_EMA_ROI_TBL_s *pRoiTbl, const AMBA_CAL_ROI_s *pRestrictArea[AMBA_CAL_OVERLAP_AREA_NUM],
        AMBA_CAL_EMA_STITCH_LINE_INFO_s *pInfoPrev[AMBA_CAL_OVERLAP_AREA_NUM], AMBA_CAL_EMA_STITCH_LINE_INFO_s *pInfo[AMBA_CAL_OVERLAP_AREA_NUM]);

UINT32 AmbaCal_EmaGenBlendTable(AMBA_CAL_EMA_BLEND_CFG_s *pNewBlendCfg[AMBA_CAL_OVERLAP_AREA_NUM], const AMBA_CAL_EMA_BLEND_TBL_s *pTbl);

UINT32 AmbaCal_EmaGenRoiTableV2(const AMBA_CAL_EM_3IN1_VIEW_CFG_V1_s *pView, const AMBA_CAL_EMA_FILTER_RULE_V2_s *pFilterRule, const AMBA_CAL_EMA_ROI_TBL_s *pRoiTbl);

UINT32 AmbaCal_EmaFindStiLineV2(const AMBA_CAL_EM_3IN1_VIEW_CFG_V1_s *pView, AMBA_OD_2DBBX_LIST_s *pBbxInfo[3], const AMBA_CAL_EMA_ROI_TBL_s *pRoiTbl, const AMBA_CAL_ROI_s *pRestrictArea[AMBA_CAL_OVERLAP_AREA_NUM],
        AMBA_CAL_EMA_STITCH_LINE_INFO_s *pInfoPrev[AMBA_CAL_OVERLAP_AREA_NUM], UINT32 MaxStiLineOffset[AMBA_CAL_OVERLAP_AREA_NUM], UINT32 KeyBbxRoiMargin, UINT32 KeyBbxMissTolerance, AMBA_CAL_EMA_STITCH_LINE_INFO_s *pInfo[AMBA_CAL_OVERLAP_AREA_NUM]);

#endif
