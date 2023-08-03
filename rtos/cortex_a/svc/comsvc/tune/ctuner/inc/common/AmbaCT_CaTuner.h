/**
 *  @file AmbaCT_CaTuner.h
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
#ifndef AMBA_CT_CA_TUNER_H
#define AMBA_CT_CA_TUNER_H
#include "AmbaCalib_CaDef.h"
#include "AmbaCalib_CaIF.h"
#include "AmbaCT_CaTunerIF.h"

typedef struct {
    void *pWorkingBuf;
    SIZE_t WorkingBufSize;
    void *pCalibWorkingBuf;
    SIZE_t CalibWorkingBufSize;
} AMBA_CT_CA_INIT_CFG_s;

typedef enum {
    AMBA_CT_CA_SYSTEM_INFO = 0,
    AMBA_CT_CA_LENS,
    AMBA_CT_CA_SENSOR,
    AMBA_CT_CA_OPTICAL_CENTER,
    AMBA_CT_CA_ADJUST,
    AMBA_CT_CA_VIN,
    AMBA_CT_CA_TILE_SIZE,
    AMBA_CT_CA_MAX,
} AMBA_CT_CA_TUNER_GROUP_e;

typedef struct {
    DOUBLE PositionX;
    DOUBLE PositionY;
    DOUBLE PositionZ;
    AMBA_CAL_ROTATION_e RotateType;
} AMBA_CT_CA_CAMERA_s;



void AmbaCT_CaGetSystem(AMBA_CT_CA_SYSTEM_s * pData);
void AmbaCT_CaSetSystem(const AMBA_CT_CA_SYSTEM_s *pData);
void AmbaCT_CaGetLens(AMBA_CT_CA_LENS_s * pData);
void AmbaCT_CaSetLens(const AMBA_CT_CA_LENS_s * pData);
void AmbaCT_CaGetSensor(AMBA_CAL_SENSOR_s *pData);
void AmbaCT_CaSetSensor(const AMBA_CAL_SENSOR_s *pData);
void AmbaCT_CaGetOpticalCenter(AMBA_CAL_POINT_DB_2D_s *pData);
void AmbaCT_CaSetOpticalCenter(const AMBA_CAL_POINT_DB_2D_s *pData);
void AmbaCT_CaGetVin(AMBA_CAL_VIN_SENSOR_GEOMETRY_s *pData);
void AmbaCT_CaSetVin(const AMBA_CAL_VIN_SENSOR_GEOMETRY_s *pData);
void AmbaCT_CaGetTileSize(AMBA_CAL_SIZE_s *pData);
void AmbaCT_CaSetTileSize(const AMBA_CAL_SIZE_s *pData);
void AmbaCT_CaGetAdjust(AMBA_CAL_CA_ADJUST_CFG_s *pData);
void AmbaCT_CaSetAdjust(const AMBA_CAL_CA_ADJUST_CFG_s * pData);

void AmbaCT_CaGetWorkingBufSize(SIZE_t *pSize);
UINT32 AmbaCT_CaTunerInit(const AMBA_CT_CA_INIT_CFG_s *pInitCfg);
UINT8 AmbaCT_CaGetGroupStatus(UINT8 GroupId);
UINT32 AmbaCT_CaExecute(void);
#endif
