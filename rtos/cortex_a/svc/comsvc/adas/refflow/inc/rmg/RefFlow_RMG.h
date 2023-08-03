/**
 *  @file RefFlow_RMG.h
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
 *
 *  @details header file of RMG reference flow
 *
 */

#ifndef REF_FLOW_RMG_H
#define REF_FLOW_RMG_H

#include "RefFlow_Common.h"

#include "AmbaSRRef_Flow.h"
#include "AmbaWS_RMG.h"


/** Define for maximum neural network category number */
#define RF_RMG_MAX_NN_CAT_NUM     (64U)


/** refer to AMBA_OD_2DBBX_LIST_s (input) */
#define RF_RMG_DFMT_2DBBX_IN_V000          (0x00000100U) /**< 0x00000100 to 0x000001FF: Reserved for AMBA_OD_2DBBX_LIST_s (input) */
#define RF_RMG_DFMT_2DBBX_IN               (RF_FUNC_RMG | RF_RMG_DFMT_2DBBX_IN_V000)
/** refer to AMBA_SR_CANBUS_TRANSFER_DATA_s (input) */
#define RF_RMG_DFMT_CANBUS_TR_V000         (0x00000200U) /**< 0x00000200 to 0x000002FF: Reserved for AMBA_SR_CANBUS_TRANSFER_DATA_s (input) */
#define RF_RMG_DFMT_CANBUS_TR              (RF_FUNC_RMG | RF_RMG_DFMT_CANBUS_TR_V000)


/** refer to AMBA_OD_2DBBX_LIST_s (output) */
#define RF_RMG_DFMT_2DBBX_OUT_V000         (0x00000300U) /**< 0x00000300 to 0x000003FF: Reserved for AMBA_OD_2DBBX_LIST_s (output) */
#define RF_RMG_DFMT_2DBBX_OUT              (RF_FUNC_RMG | RF_RMG_DFMT_2DBBX_OUT_V000)
/** refer to AMBA_SR_SROBJECT_DATA_s (output) */
#define RF_RMG_DFMT_SR_V000                (0x00000400U) /**< 0x00000400 to 0x000004FF: Reserved for AMBA_SR_SROBJECT_DATA_s (output) */
#define RF_RMG_DFMT_SR                     (RF_FUNC_RMG | RF_RMG_DFMT_SR_V000)
/** refer to AMBA_WS_RMG_WARN_OBJS_s (output) */
#define RF_RMG_DFMT_RMG_OUT_V000           (0x00000500U) /**< 0x00000500 to 0x000005FF: Reserved for AMBA_WS_RMG_WARN_OBJS_s (output) */
#define RF_RMG_DFMT_RMG_OUT                (RF_FUNC_RMG | RF_RMG_DFMT_RMG_OUT_V000)
/** refer to REF_FLOW_RMG_OUTPUT_DATA_s (output) */
#define RF_RMG_DFMT_RF_RMG_OUT_V000        (0x00000600U) /**< 0x00000600 to 0x000006FF: Reserved for REF_FLOW_RMG_OUTPUT_DATA_s (output) */
#define RF_RMG_DFMT_RF_RMG_OUT             (RF_FUNC_RMG | RF_RMG_DFMT_RF_RMG_OUT_V000)
/** refer to AMBA_OD_2DBBX_LIST_s (output) */
#define RF_RMG_DFMT_2DBBX_STBL_OUT_V000    (0x00000700U) /**< 0x00000700 to 0x000007FF: Reserved for AMBA_OD_2DBBX_LIST_s (output) */
#define RF_RMG_DFMT_2DBBX_STBL_OUT         (RF_FUNC_RMG | RF_RMG_DFMT_2DBBX_STBL_OUT_V000)

/** refer to AMBA_WS_RMG_CONFIG_s (config) */
#define RF_RMG_DFMT_WS_CFG_V000            (0x00000800U) /**< 0x00000800 to 0x000008FF: Reserved for AMBA_WS_RMG_CONFIG_s (config) */
#define RF_RMG_DFMT_WS_CFG                 (RF_FUNC_RMG | RF_RMG_DFMT_WS_CFG_V000)


typedef struct {
    UINT8* pWrkBufAddr;                                                   /**< Working buffer address */
    AMBA_SR_ROI_s ODRoi;                                                  /**< Object detection network Roi, coordinate should be same as BBox's coordinate, please refer to AmbaSR_BbxConvDef.h */
    AMBA_CAL_EM_CALC_COORD_CFG_s CalibCfg;                                /**< Calibration config, please refer to AmbaCalib_EmirrorDef.h */
    UINT32 NNCatNum;                                                      /**< Number of neural network category*/
    AMBA_SR_NN_CAT_TO_OBJ_TYPE_s NNCat2ObjType[RF_RMG_MAX_NN_CAT_NUM];    /**< Pair of neural network category and object type, please refer to AmbaSR_ObjType.h*/
    AMBA_COMPACT_CAR_SPEC_s SelfCarSpec;                                  /**< Self car specification, need "WheelBase" */
    UINT32  TSClock;                                                      /**< System Clock in KHz, time(ms) = TimeStamp/Clock */
    AMBA_WS_RMG_CONFIG_s RMGCfg;                                          /**< Config for WS_RMG, please refer to AmbaWS_RMG.h */
    REF_FLOW_LOG_FP LogFp;                                                /**< Regist callback function for logging data */
} REF_FLOW_RMG_CFG_s;

typedef struct {
    AMBA_OD_2DBBX_LIST_s BbxList;                      /**< Bounding box information from object detection neural network */
    AMBA_SR_CANBUS_TRANSFER_DATA_s CanbusTrData;       /**< Canbus information */
} REF_FLOW_RMG_INPUT_DATA_s;

typedef struct {
    AMBA_WS_RMG_WARN_OBJS_s RMGOut;                     /**< RMG result */
} REF_FLOW_RMG_OUTPUT_DATA_s;



UINT32 RefFlowRMG_GetWorkBufferSize(UINT32* pSize);

UINT32 RefFlowRMG_GetDefaultCfg(REF_FLOW_RMG_CFG_s* pCfg);

UINT32 RefFlowRMG_Init(const REF_FLOW_RMG_CFG_s* pCfg);

UINT32 RefFlowRMG_DeInit(void);

UINT32 RefFlowRMG_Process(const REF_FLOW_RMG_INPUT_DATA_s* pIn, REF_FLOW_RMG_OUTPUT_DATA_s* pOut);

#endif
