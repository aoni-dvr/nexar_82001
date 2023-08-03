/**
*  @file SvcCalibAdas.h
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
*  @details svc calibration ADAS
*
*/

#ifndef SVC_CALIB_ADAS_H
#define SVC_CALIB_ADAS_H

#define SVC_CALIB_ADAS_ID    (10U)
#define SVC_CALIB_ADAS_VERSION            (0x20190307U)
#define SVC_CALIB_ADAS_VERSION_V1         (0x20210501U)

typedef struct {
    UINT32 Enable;
    UINT32 Type;
    UINT32 Version;
} SVC_CALIB_ADAS_TABLE_HEADER_s;

typedef struct {
    SVC_CALIB_ADAS_TABLE_HEADER_s    Header;
    AMBA_CAL_EM_CALC_COORD_CFG_s     AdasCfg;
    AMBA_CAL_EM_CALC_COORD_CFG_V1_s  AdasCfgV1;
    AMBA_CAL_EM_CALIB_INFO_DATA_V1_s AdasInfoDataV1;
    AMBA_CAL_EM_CURVED_SURFACE_CFG_s AdasExtData01;
    AMBA_CAL_EM_CAM_CALIB_DATA_s     AdasExtData02;
    AMBA_CAL_AVM_CALIB_DATA_s        AdasAvmCalibCfg;
    AMBA_CAL_AVM_PARK_AST_CFG_V1_s   AdasAvmPaCfg;
    AMBA_CAL_AVM_2D_CFG_V2_s         AdasAvm2DCfg;//don't use cam data, cam data please use AdasAvmPaCfg
    AMBA_CAL_AVM_MV_PERSPET_V1_CFG_s AdasExtData03;
    AMBA_CAL_AVM_MV_EQ_DISTANCE_s    AdasExtData04;
    AMBA_CAL_AVM_MV_LDC_CFG_s        AdasExtData05;
    AMBA_CAL_AVM_MV_FLOOR_CFG_s      AdasExtData06;
    AMBA_CAL_LENS_DST_REAL_EXPECT_s  AdasExtData07;
    DOUBLE AdasExtData08[MAX_LENS_DISTO_TBL_LEN];
    DOUBLE AdasExtData09[MAX_LENS_DISTO_TBL_LEN];
} SVC_CALIB_ADAS_NAND_TABLE_s;

typedef struct {
    UINT32                           CalibVer;
    AMBA_CAL_EM_CALC_COORD_CFG_s     AdasCfg;
    AMBA_CAL_EM_CALC_COORD_CFG_V1_s  AdasCfgV1;
    AMBA_CAL_EM_CALIB_INFO_DATA_V1_s AdasInfoDataV1;
} SVC_CALIB_ADAS_INFO_GET_s;


UINT32 SvcCalib_AdasCmdFunc(UINT32 CmdID, void *pParam1, void *pParam2, void *pParam3, void *pParam4);
UINT32 SvcCalib_AdasShellFunc(UINT32 ArgCount, char * const *pArgVector);

#define SVC_CALIB_ADAS_TYPE_FRONT    (1U)
#define SVC_CALIB_ADAS_TYPE_LEFT     (2U)
#define SVC_CALIB_ADAS_TYPE_RIGHT    (3U)
#define SVC_CALIB_ADAS_TYPE_BACK     (4U)
#define SVC_CALIB_ADAS_TYPE_AVM_F    (5U)
#define SVC_CALIB_ADAS_TYPE_AVM_B    (6U)
#define SVC_CALIB_ADAS_TYPE_AVM_L    (7U)
#define SVC_CALIB_ADAS_TYPE_AVM_R    (8U)
#define SVC_CALIB_ADAS_TYPE_NUM      (9U)


UINT32 SvcCalib_AdasCfgGet(UINT32 Type, void *pCfg);
UINT32 SvcCalib_AdasCfgGetV1(UINT32 Type, void *pCfg);
UINT32 SvcCalib_AdasCfgSet(UINT32 Type, void *pCfg);
UINT32 SvcCalib_AdasCfgSetV1(UINT32 Type, void *pCfg, void *pCfg2);
UINT32 SvcCalib_AdasDbgMsg(UINT32 Enable);
UINT32 SvcCalib_AdasCfgGetCalVer(UINT32 Type);

#endif /* SVC_CALIB_ADAS_H */
