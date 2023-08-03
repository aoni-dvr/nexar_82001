/**
 *  @file RefFlow_BSD.h
 *
 * Copyright (c) 2019 Ambarella International LP
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
 *  @details header file of BSD reference flow
 *
 */

#ifndef REF_FLOW_BSD_H
#define REF_FLOW_BSD_H

#include "RefFlow_Common.h"
#ifndef AMBA_TYPES_H
#include "AmbaTypes.h"
#endif
#include "AmbaOD_Tracker.h"
#include "AmbaSR_ObjType.h"
#include "AmbaSR_BbxConvDef.h"
#include "AmbaWS_BSD.h"


/** Define for channel  */
#define RF_BSD_CHANNEL_LEFT       (0U)
#define RF_BSD_CHANNEL_RIGHT      (1U)
#define RF_BSD_CHANNEL_NUM        (2U)

/** Define for maximum object number */
#define RF_BSD_MAX_OBJ_NUM        (64U)
/** Define for maximum neural network category number */
#define RF_BSD_MAX_NN_CAT_NUM     (64U)
/** Define for maximum warning system config number */
#define RF_BSD_MAX_WS_CFG_NUM     (8U)

/** Enable or disable to set object type as warning target. refer to TypeFlag in REF_FLOW_BSD_WARN_OBJ_TYPE_s */
#define RF_BSD_WARN_TYPE_FLAG_DISABLE  (0U)
#define RF_BSD_WARN_TYPE_FLAG_ENABLE   (1U)

/******************************************************************/
/** Data Type for log                                             */
/******************************************************************/
/** refer to AMBA_OD_2DBBX_LIST_s (input) */
#define RF_BSD_DFMT_2DBBX_IN_V000          (0x00000100U) /**< 0x00000100 to 0x000001FF: Reserved for AMBA_OD_2DBBX_LIST_s (input) */
#define RF_BSD_DFMT_2DBBX_IN               (RF_FUNC_BSD | RF_BSD_DFMT_2DBBX_IN_V000)
/** refer to AMBA_SR_CANBUS_TRANSFER_DATA_s (input) */
#define RF_BSD_DFMT_CANBUS_TR_V000         (0x00000200U) /**< 0x00000200 to 0x000002FF: Reserved for AMBA_SR_CANBUS_TRANSFER_DATA_s (input) */
#define RF_BSD_DFMT_CANBUS_TR              (RF_FUNC_BSD | RF_BSD_DFMT_CANBUS_TR_V000)

/** refer to AMBA_OD_2DBBX_LIST_s (output) */
#define RF_BSD_DFMT_2DBBX_OUT_V000         (0x00000300U) /**< 0x00000300 to 0x000003FF: Reserved for RF_BSD_DFMT_2DBBX_OUT_VER (output) */
#define RF_BSD_DFMT_2DBBX_OUT              (RF_FUNC_BSD | RF_BSD_DFMT_2DBBX_OUT_V000)
/** refer to AMBA_SR_SROBJECT_DATA_s (output) */
#define RF_BSD_DFMT_SR_V000                (0x00000400U) /**< 0x00000400 to 0x000004FF: Reserved for AMBA_SR_SROBJECT_DATA_s (output) */
#define RF_BSD_DFMT_SR                     (RF_FUNC_BSD | RF_BSD_DFMT_SR_V000)
/** refer to REF_FLOW_BSD_OUT_DATA_s (output) */
#define RF_BSD_DFMT_BSD_OUT_V000           (0x00000500U) /**< 0x00000500 to 0x000005FF: Reserved for REF_FLOW_BSD_OUT_DATA_s (output) */
#define RF_BSD_DFMT_BSD_OUT                (RF_FUNC_BSD | RF_BSD_DFMT_BSD_OUT_V000)
/** refer to AMBA_WS_BSD_DATA_s (config) */
#define RF_BSD_DFMT_WS_CFG_V000            (0x00000600U) /**< 0x00000600 to 0x000006FF: Reserved for AMBA_WS_BSD_DATA_s (config) */
#define RF_BSD_DFMT_WS_CFG                 (RF_FUNC_BSD | RF_BSD_DFMT_WS_CFG_V000)
/** refer to AMBA_OD_2DBBX_LIST_s (output, stabilized 2D bbox for visualization) */
#define RF_BSD_DFMT_2DBBX_STBL_OUT_V000    (0x00000700U) /**< 0x00000700 to 0x000007FF: Reserved for AMBA_OD_2DBBX_LIST_s (output) */
#define RF_BSD_DFMT_2DBBX_STBL_OUT         (RF_FUNC_BSD | RF_BSD_DFMT_2DBBX_STBL_OUT_V000)


typedef struct {
    UINT8 TypeFlag[4U];          /**< Contral which object type should be set as warning target. Ex: TypeFlag[SR_OBJ_TYPE_VEHICLE_0] = RF_BSD_TYPE_FLAG_ENABLE */
} REF_FLOW_BSD_WARN_OBJ_TYPE_s;

typedef struct {
    AMBA_CAL_EM_CALC_COORD_CFG_s CalibCfg[RF_BSD_CHANNEL_NUM];            /**< Calibration config, please refer to AmbaCalib_EmirrorDef.h */
    AMBA_SR_ROI_s ODRoi[RF_BSD_CHANNEL_NUM];                              /**< Object detection network Roi, coordinate should be same as BBox's coordinate, please refer to AmbaSR_BbxConvDef.h */
    UINT32 NNCatNum;                                                      /**< Number of neural network category*/
    AMBA_SR_NN_CAT_TO_OBJ_TYPE_s NNCat2ObjType[RF_BSD_MAX_NN_CAT_NUM];    /**< Pair of neural network category and object type, please refer to AmbaSR_ObjType.h*/
    UINT32  TSClock;                                                      /**< Clock of time stamp */
    AMBA_COMPACT_CAR_SPEC_s SelfCarSpec;                                  /**< Self car specification, please refer to Amba_Vehicles.h */
    UINT32 WsBsdCfgNum;                                                   /**< Config number for AMBA_WS_BSD_DATA_s (only support 1 config now) */
    AMBA_WS_BSD_DATA_s WsBsdCfg[RF_BSD_MAX_WS_CFG_NUM];                   /**< Config for WS_BSD, please refer to AmbaWS_BSD.h (only support 1 config now) */
    REF_FLOW_BSD_WARN_OBJ_TYPE_s WsWarnObjType[RF_BSD_MAX_WS_CFG_NUM];    /**< Setting for selecting object type for warning target (only support 1 config now)(invalid in current version) */
    REF_FLOW_LOG_FP LogFp;                                                /**< Register callback function for logging data */
} REF_FLOW_BSD_CFG_s;


typedef struct {
    AMBA_OD_2DBBX_LIST_s BbxList;                      /**< Bounding box information from object detection neural network */
    AMBA_SR_CANBUS_TRANSFER_DATA_s CanbusTrData;       /**< Canbus information */
} REF_FLOW_BSD_IN_DATA_s;

typedef struct {
    UINT32 WsBsdCfgNum;                                        /**< Config number for AMBA_WS_BSD_DATA_s */
    AMBA_WS_BSD_STATUS_s WsBsdStat[RF_BSD_MAX_WS_CFG_NUM];     /**< Blind spot detection result */
} REF_FLOW_BSD_OUT_DATA_s;



UINT32 RefFlowBSD_GetDefaultCfg(UINT32 *WrkBufSize, REF_FLOW_BSD_CFG_s *pRfBsdCfg);

UINT32 RefFlowBSD_Init(const REF_FLOW_BSD_CFG_s *pRfBsdCfg, UINT8 *pWrkBufAddr);

UINT32 RefFlowBSD_Process(UINT32 Channel, const REF_FLOW_BSD_IN_DATA_s* pBsdInData, REF_FLOW_BSD_OUT_DATA_s* pBsdOutData);

UINT32 RefFlowBSD_DeInit(void);

#endif
