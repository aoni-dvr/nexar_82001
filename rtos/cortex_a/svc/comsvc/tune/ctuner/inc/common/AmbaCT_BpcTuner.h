/**
 *  @file AmbaCT_BpcTuner.h
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
#ifndef AMBA_CT_BPC_TUNER_H
#define AMBA_CT_BPC_TUNER_H
#include "AmbaTypes.h"
#include "AmbaCalib_Def.h"
#include "AmbaCalib_StaticBadPixelDef.h"
#include "AmbaCT_BpcTunerIF.h"
#define AMBA_CT_BPC_MAX_RAW_SIZE (4000U*3000U)
typedef enum {
    AMBA_CT_BPC_TUNER_SYSTEM_INFO = 0,
    AMBA_CT_BPC_TUNER_RAW_INFO,
    AMBA_CT_BPC_TUNER_VIN,
    AMBA_CT_BPC_TUNER_BPC,
    AMBA_CT_BPC_TUNER_OB,
    AMBA_CT_BPC_TUNER_MAX,
} AMBA_CT_BPC_TUNER_GROUP_e;

typedef struct {
    void *pWorkingBuf;
    SIZE_t WorkingBufSize;
    void *pCalibWorkingBuf;
    SIZE_t CalibWorkingBufSize;
    void *pSrcRawBuf;
    SIZE_t SrcRawBufSize;
    UINT32 SrcRawResolution;
    UINT32 (*FeedRawFunc)(const char *pFileName, UINT16 *pRawBuf, SIZE_t RawBufSize);
} AMBA_CT_BPC_INIT_CFG_s;



void AmbaCT_BpcGetSystemInfo(AMBA_CT_BPC_TUNER_SYSTEM_s *pData);
void AmbaCT_BpcSetSystemInfo(const AMBA_CT_BPC_TUNER_SYSTEM_s *pData);
void AmbaCT_BpcGetRawInfo(AMBA_CT_BPC_TUNER_RAW_INFO_s *pData);
void AmbaCT_BpcSetRawInfo(const AMBA_CT_BPC_TUNER_RAW_INFO_s *pData);
void AmbaCT_BpcGetVin(AMBA_CAL_VIN_SENSOR_GEOMETRY_s *pData);
void AmbaCT_BpcSetVin(const AMBA_CAL_VIN_SENSOR_GEOMETRY_s *pData);
void AmbaCT_BpcGetBpc(AMBA_CT_BPC_TUNER_BPC_s *pData);
void AmbaCT_BpcSetBpc(const AMBA_CT_BPC_TUNER_BPC_s *pData);
void AmbaCT_BpcGetOb(AMBA_CAL_BPC_OB_INFO_s *pData);
void AmbaCT_BpcSetOb(const AMBA_CAL_BPC_OB_INFO_s *pData);



void AmbaCT_BpcGetWorkingBufSize(SIZE_t *pSize);
UINT32 AmbaCT_BpcTunerInit(const AMBA_CT_BPC_INIT_CFG_s *pInitCfg);
UINT8 AmbaCT_BpcGetGroupStatus(UINT8 GroupId);
UINT32 AmbaCT_BpcExecute(void);
#endif
