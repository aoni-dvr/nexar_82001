/**
 *  @file AmbaCT_1DVigTuner.h
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
#ifndef AMBA_CT_1D_VIG_TUNER_H
#define AMBA_CT_1D_VIG_TUNER_H
#include "AmbaTypes.h"
#include "AmbaCalib_Def.h"
#include "AmbaCalib_VignetteCV2IF.h"
#include "AmbaCT_1DVigTunerIF.h"
#define AMBA_CT_1D_VIG_MAX_RAW_LEN (4000U*3000U)
typedef struct {
    void *pWorkingBuf;
    SIZE_t WorkingBufSize;
    void *pCalibWorkingBuf;
    SIZE_t CalibWorkingBufSize;
    UINT32 (*CbFeedRawBufFunc)(SIZE_t RawBufSize, UINT32 *pPitch, AMBA_CAL_ROI_s *pValidArea, UINT16 *pRaw);
    void *pSrcRawBuf;
    SIZE_t SrcRawBufSize;
    UINT32 (*pFeedRawFuncV1)(UINT32 CamId, void *pRawBuf, SIZE_t RawBufSize);
    UINT32 (*pFeedGainTbl)(UINT32 CamId, AMBA_CAL_1D_VIG_CALIB_DATA_V1_s *pCalibData);
} AMBA_CT_1D_VIG_INIT_CFG_s;

typedef enum {
    AMBA_CT_1D_VIG_TUNER_SYSTEM_INFO = 0,
    AMBA_CT_1D_VIG_TUNER_RAW_INFO,
    AMBA_CT_1D_VIG_TUNER_VIGNETTE,
    AMBA_CT_1D_VIG_TUNER_EVA_LOC,
    AMBA_CT_1D_VIG_TUNER_CALIB_INFO,
    AMBA_CT_1D_VIG_TUNER_MAX,
} AMBA_CT_1D_VIG_TUNER_GROUP_e;


void AmbaCT_1DVigGetWorkingBufSize(SIZE_t *pSize);
UINT32 AmbaCT_1DVigTunerInit(const AMBA_CT_1D_VIG_INIT_CFG_s *pInitCfg);
UINT8 AmbaCT_1DVigGetGroupStatus(UINT8 GroupId);
void AmbaCT_1DVigGetSystemInfo(AMBA_CT_1D_VIG_TUNER_SYSTEM_s *pData);
void AmbaCT_1DVigSetSystemInfo(const AMBA_CT_1D_VIG_TUNER_SYSTEM_s *pData);
void AmbaCT_1DVigGetRawInfo(UINT32 ArrayIndex, AMBA_CT_1D_VIG_TUNER_RAW_INFO_s *pData);
void AmbaCT_1DVigSetRawInfo(UINT32 ArrayIndex, const AMBA_CT_1D_VIG_TUNER_RAW_INFO_s *pData);
void AmbaCT_1DVigGetVignette(UINT32 ArrayIndex, AMBA_CT_1D_VIG_TUNER_VIGNETTE_s *pData);
void AmbaCT_1DVigSetVignette(UINT32 ArrayIndex, const AMBA_CT_1D_VIG_TUNER_VIGNETTE_s *pData);
void AmbaCT_1DVigGetEvaluationLoc(UINT32 ArrayIndex, AMBA_CT_1D_VIG_EVALUATION_LOC_s *pData);
void AmbaCT_1DVigSetEvaluationLoc(UINT32 ArrayIndex, const AMBA_CT_1D_VIG_EVALUATION_LOC_s *pData);
void AmbaCT_1DVigGetEvaluationCalibData(UINT32 ArrayIndex, AMBA_CT_1D_VIG_EVA_CALIB_DATA_s *pData);
void AmbaCT_1DVigSetEvaluationCalibData(UINT32 ArrayIndex, const AMBA_CT_1D_VIG_EVA_CALIB_DATA_s *pData);
UINT32 AmbaCT_1DVigGetActiveChanIdx(void);
UINT32 AmbaCT_1DVigExecute(void);
#endif
