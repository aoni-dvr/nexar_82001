/**
 *  @file AmbaCT_OcCbTuner.h
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
#ifndef AMBA_CT_OCCB_TUNER_H
#define AMBA_CT_OCCB_TUNER_H
#include "AmbaTypes.h"
#include "AmbaCT_OcCbTunerIF.h"
#include "AmbaCalib_Def.h"
#include "AmbaCalib_OcCbIF.h"
#include "AmbaDSP_ImageFilter.h"
#include "AmbaCalib_DetectionIF.h"



typedef struct {
    void *pWorkingBuf;
    SIZE_t WorkingBufSize;
    void *pPatDetWorkingBuf;
    SIZE_t PatDetWorkingBufSize;
    void *pOrganizeCBCornerWorkingBuf;
    SIZE_t OrganizeCBCornerWorkingBufSize;
    void *pOCFinderWorkingBuf;
    SIZE_t OCFinderWorkingBufSize;
    void *pLdccWorkingBuf;
    SIZE_t LdccWorkingBufSize;
    UINT32 (*FeedSrcFunc)(SIZE_t SrcBufSize, UINT8 *pSrc);
    UINT32 (*PatDetDebugReport)(const char *pOutputPrefix, UINT8 *pSrcImg, const AMBA_CAL_SIZE_s *pImgSize, UINT32 CornerNum, const AMBA_CAL_POINT_DB_2D_s *pSubPixCorners, const INT16 *pCorner2dMap);
} AMBA_CT_OCCB_INIT_CFG_s;

typedef enum {
    AMBA_CT_OCCB_TUNER_SYSTEM_INFO = 0,
    AMBA_CT_OCCB_TUNER_SRC_INFO,
    AMBA_CT_OCCB_TUNER_PAT_TYPE,
    AMBA_CT_OCCB_TUNER_PAT_DET,
    AMBA_CT_OCCB_TUNER_PAT_CIRCLE_DET,
    AMBA_CT_OCCB_TUNER_ORGANIZE_CORNER,
    AMBA_CT_OCCB_TUNER_CAMERA,
    AMBA_CT_OCCB_TUNER_OPTICAL_CENTER,
    AMBA_CT_OCCB_TUNER_SEARCH_RANGE,
    AMBA_CT_OCCB_TUNER_WORLD_MAP,
    AMBA_CT_OCCB_TUNER_OPTIMIZED_GRID_NUM,
    AMBA_CT_OCCB_TUNER_MAX,
} AMBA_CT_OCCB_TUNER_GROUP_e;




void AmbaCT_OcCbGetWorkingBufSize(SIZE_t *pSize);
UINT32 AmbaCT_OcCbTunerInit(const AMBA_CT_OCCB_INIT_CFG_s *pInitCfg);
UINT8 AmbaCT_OcCbGetGroupStatus(UINT8 GroupId);

void AmbaCT_OcCbGetSystemInfo(AMBA_CT_OCCB_TUNER_SYSTEM_s *pData);
void AmbaCT_OcCbSetSystemInfo(const AMBA_CT_OCCB_TUNER_SYSTEM_s *pData);
void AmbaCT_OcCbGetSrcInfo(AMBA_CT_OCCB_TUNER_SRC_s *pData);
void AmbaCT_OcCbSetSrcInfo(const AMBA_CT_OCCB_TUNER_SRC_s *pData);
void AmbaCT_OcCbGetPatTypeCfg(UINT32 *pData);
void AmbaCT_OcCbSetPatTypeCfg(const UINT32 *pData);
void AmbaCT_OcCbGetPatDetCfg(AMBA_CT_OCCB_TUENR_PAT_DET_CHK_s *pData);
void AmbaCT_OcCbSetPatDetCfg(const AMBA_CT_OCCB_TUENR_PAT_DET_CHK_s *pData);
void AmbaCT_OcCbGetPatDetCircleCfg(AMBA_CT_OCCB_TUENR_PAT_DET_CIR_s *pData);
void AmbaCT_OcCbSetPatDetCircleCfg(const AMBA_CT_OCCB_TUENR_PAT_DET_CIR_s *pData);
void AmbaCT_OcCbGetOrganizeCornerCfg(AMBA_CAL_OCCB_ORGANIZE_CFG_s *pData);
void AmbaCT_OcCbSetOrganizeCornerCfg(const AMBA_CAL_OCCB_ORGANIZE_CFG_s *pData);
void AmbaCT_OcCbGetCamera(AMBA_CT_OCCB_CAMERA_s *pData);
void AmbaCT_OcCbSetCamera(const AMBA_CT_OCCB_CAMERA_s *pData);
void AmbaCT_OcCbGetOpticalCenter(AMBA_CAL_POINT_DB_2D_s *pData);
void AmbaCT_OcCbSetOpticalCenter(const AMBA_CAL_POINT_DB_2D_s *pData);
void AmbaCT_OcCbGetSearchRange(AMBA_CAL_SIZE_s *pData);
void AmbaCT_OcCbSetSearchRange(const AMBA_CAL_SIZE_s *pData);
void AmbaCT_OcCbGetWorldMapCfg(AMBA_CT_OCCB_TUENR_World_Map_s *pData);
void AmbaCT_OcCbSetWorldMapCfg(const AMBA_CT_OCCB_TUENR_World_Map_s *pData);
void AmbaCT_OcCbGetOptimizedGridNum(AMBA_CT_OCCB_USED_GRID_SIZE_s *pData);
void AmbaCT_OcCbSetOptimizedGridNum(const AMBA_CT_OCCB_USED_GRID_SIZE_s *pData);
UINT32 AmbaCT_OcCbExecute(void);
#endif
