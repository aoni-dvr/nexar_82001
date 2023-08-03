/**
 *  @file AmbaCT_OcTuner.h
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
#ifndef AMBA_CT_OC_TUNER_H
#define AMBA_CT_OC_TUNER_H
#include "AmbaTypes.h"
#include "AmbaCalib_Def.h"
#include "AmbaCalib_OcIF.h"
#include "AmbaDSP_ImageFilter.h"
#include "AmbaCT_OcTunerIF.h"

#define AMBA_CT_OC_MAX_RAW_SIZE (4000U*3000U)

typedef struct {
    void *pWorkingBuf;
    SIZE_t WorkingBufSize;
    void *pCalibWorkingBuf;
    SIZE_t CalibWorkingBufSize;
    UINT32 (*CbFeedRawBuf)(SIZE_t RawBufSize, UINT16 *pRaw);
} AMBA_CT_OC_INIT_CFG_s;

typedef enum {
    AMBA_CT_OC_TUNER_SYSTEM_INFO = 0,
    AMBA_CT_OC_TUNER_RAW_INFO,
    AMBA_CT_OC_TUNER_VIN,
    AMBA_CT_OC_TUNER_ELLIPSE_INFO,
    AMBA_CT_OC_TUNER_MAX
} AMBA_CT_OC_TUNER_GROUP_e;

void AmbaCT_OcGetWorkingBufSize(SIZE_t *pSize);
UINT32 AmbaCT_OcTunerInit(const AMBA_CT_OC_INIT_CFG_s *pInitCfg);
UINT8 AmbaCT_OcGetGroupStatus(UINT8 GroupId);
void AmbaCT_OcGetSystemInfo(AMBA_CT_OC_TUNER_SYSTEM_s *pData);
void AmbaCT_OcSetSystemInfo(const AMBA_CT_OC_TUNER_SYSTEM_s *pData);
void AmbaCT_OcGetRawInfo(AMBA_CT_OC_TUNER_RAW_INFO_s *pData);
void AmbaCT_OcSetRawInfo(const AMBA_CT_OC_TUNER_RAW_INFO_s *pData);
void AmbaCT_OcGetVin(AMBA_CT_OC_TUNER_VIN_s *pData);
void AmbaCT_OcSetVin(const AMBA_CT_OC_TUNER_VIN_s *pData);
void AmbaCT_OcGetEllipseInfo(AMBA_CT_OC_TUNER_ELLIPSE_INFO_s *pData);
void AmbaCT_OcSetEllipseInfo(const AMBA_CT_OC_TUNER_ELLIPSE_INFO_s *pData);
UINT32 AmbaCT_OcExecute(void);

#endif
