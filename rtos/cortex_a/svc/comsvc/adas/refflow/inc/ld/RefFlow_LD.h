/**
 *  @file RefFlow_LD.h
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
 *  @details header file of lane-base function reference flow, including LDWS and LKA
 *
 */

#ifndef REF_FLOW_LD_H
#define REF_FLOW_LD_H

#include "RefFlow_Common.h"
#include "AmbaSR_Lane.h"
#include "AmbaWS_LDWS.h"
#include "AmbaAP_LKA.h"

#ifndef AMBA_TYPES_H
#include "AmbaTypes.h"
#endif

/** refer to AMBA_SEG_BUF_INFO_s */
#define RF_LD_DFMT_DETECT_IN_V000         (0x00000100U) /** 0x00000100 to 0x000001FF: Reserved for AMBA_SEG_BUF_INFO_s */
#define RF_LD_DFMT_DETECT_IN              (RF_FUNC_LD | RF_LD_DFMT_DETECT_IN_V000)
/** refer to AMBA_SEG_BUF_INFO_s -> pBuf */
#define RF_LD_DFMT_DETECT_IN_BUF_V000     (0x00000200U) /** 0x00000200 to 0x000002FF: Reserved for AMBA_SEG_BUF_INFO_s -> pBuf */
#define RF_LD_DFMT_DETECT_IN_BUF          (RF_FUNC_LD | RF_LD_DFMT_DETECT_IN_BUF_V000)
/** refer to AMBA_SR_LANE_RECONSTRUCT_INFO_s */
#define RF_LD_DFMT_SR_LANE_OUT_V000       (0x00000300U) /** 0x00000300 to 0x000003FF: Reserved for AMBA_SR_LANE_RECONSTRUCT_INFO_s */
#define RF_LD_DFMT_SR_LANE_OUT            (RF_FUNC_LD | RF_LD_DFMT_SR_LANE_OUT_V000)
/** refer to AMBA_LDWS_DEPARTURE_INFO_s */
#define RF_LD_DFMT_LDWS_OUT_V000          (0x00000400U) /** 0x00000400 to 0x000004FF: Reserved for AMBA_LDWS_DEPARTURE_INFO_s */
#define RF_LD_DFMT_LDWS_OUT               (RF_FUNC_LD | RF_LD_DFMT_LDWS_OUT_V000)
/** refer to AMBA_AP_LKA_RESULT_s */
#define RF_LD_DFMT_LKA_OUT_V000           (0x00000500U) /** 0x00000500 to 0x000005FF: Reserved for AMBA_AP_LKA_RESULT_s */
#define RF_LD_DFMT_LKA_OUT                (RF_FUNC_LD | RF_LD_DFMT_LKA_OUT_V000)
/** refer to REF_FLOW_LD_CFG_s */
#define RF_LD_DFMT_REFFLOW_CFG_V000       (0x00000600U) /** 0x00000600 to 0x000006FF: Reserved for REF_FLOW_LD_CFG_s */
#define RF_LD_DFMT_REFFLOW_CFG            (RF_FUNC_LD | RF_LD_DFMT_REFFLOW_CFG_V000)

typedef struct {
    AMBA_CAL_EM_CALC_COORD_CFG_s CalibCfg;              /**< Calibration config, please refer to AmbaCalib_EmirrorDef.h */
    AMBA_SR_LANE_CFG_V2_s SrLaneCfg;                    /**< Config for SR Lane, please refer to AmbaSR_Lane.h*/
    AMBA_AP_LKA_CONFIG_s LkaCfg;                        /**< Config for LKA, please refer to AmbaAP_LKA.h*/
    AMBA_LDWS_CFG_DATA_s LdwsCfg;                       /**< Config for LDWS, please refer to AmbaWS_LDWS.h*/
    REF_FLOW_LOG_FP LogFp;                              /**< Regist callback function for logging data */
} REF_FLOW_LD_CFG_s;

typedef struct {
    AMBA_SEG_BUF_INFO_s SrLaneInData;                   /**< Segmentation lane detection input data */
    AMBA_SR_CANBUS_TRANSFER_DATA_s CanbusTrData;        /**< Canbus information */
} REF_FLOW_LD_IN_DATA_s;

typedef struct {
    AMBA_LDWS_DEPARTURE_INFO_s LdwsDepRes;              /**< Lane departure warning system result */
    AMBA_AP_LKA_RESULT_s LkaRes;                        /**< Steering information for lane keeping */
} REF_FLOW_LD_OUT_DATA_s;

typedef struct {
    AMBA_CAL_EM_CALC_COORD_CFG_V1_s CalibCfg;           /**< Calibration config, please refer to AmbaCalib_EmirrorDef.h */
    AMBA_CAL_EM_CALIB_INFO_DATA_V1_s CalibInfo;         /**< For supporting NN-3D data related work. Advance data for calibration. Generated by AmbaCal_EmGenCalibInfoV1() */
    AMBA_CAL_POINT_DB_2D_s FocalLength;                 /**< For supporting NN-3D data related work. Focal length of lens; unit: cellsize */
    AMBA_SR_LANE_CFG_V2_s SrLaneCfg;                    /**< Config for SR Lane, please refer to AmbaSR_Lane.h*/
    AMBA_AP_LKA_CONFIG_s LkaCfg;                        /**< Config for LKA, please refer to AmbaAP_LKA.h*/
    AMBA_LDWS_CFG_DATA_s LdwsCfg;                       /**< Config for LDWS, please refer to AmbaWS_LDWS.h*/
    REF_FLOW_LOG_FP LogFp;                              /**< Regist callback function for logging data */
} REF_FLOW_LD_CFG_V2_s;

typedef struct {
    AMBA_SR_LANE_QUERY_INFO_V2_s SrQrInfo;
} REF_FLOW_LD_QUERY_INFO_V2_s;

typedef struct {
    AMBA_SEG_BUF_INFO_s SrLaneCategoryBuf;              /**< Segmentation lane detection input data */
    AMBA_SEG_BUF_INFO_s SrLaneColorBuf;                 /**< Segmentation lane detection input data */
    AMBA_SEG_BUF_INFO_s SrLaneTypeBuf;                  /**< Segmentation lane detection input data */
    AMBA_SEG_BUF_INFO_s SrLaneInstanceBuf;              /**< Segmentation lane detection input data */
    AMBA_SR_CANBUS_TRANSFER_DATA_s CanbusTrData;        /**< Canbus information */
} REF_FLOW_LD_IN_DATA_V2_s;

typedef struct {
    AMBA_LDWS_DEPARTURE_INFO_s LdwsDepRes;              /**< Lane departure warning system result */
    AMBA_AP_LKA_RESULT_s LkaRes;                        /**< Steering information for lane keeping */
} REF_FLOW_LD_OUT_DATA_V2_s;

UINT32 RefFlowLD_GetDefaultCfg(UINT32 SegBufImgW, UINT32 SegBufImgH, UINT32 *pWrkBufSize, REF_FLOW_LD_CFG_s *pRfLdCfg);
UINT32 RefFlowLD_Init(REF_FLOW_LD_CFG_s *pRfLdCfg, UINT8 *pWrkBufAddr);
UINT32 RefFlowLD_Process(const REF_FLOW_LD_IN_DATA_s* pRfLdInData, REF_FLOW_LD_OUT_DATA_s* pRfLdOutData);

UINT32 RefFlowLD_GetDefaultCfgV2(REF_FLOW_LD_CFG_V2_s *pRfLdCfg);
UINT32 RefFlowLD_QueryV2(const REF_FLOW_LD_QUERY_INFO_V2_s* pQrInfo, UINT32 *pWrkBufSize);
UINT32 RefFlowLD_InitV2(REF_FLOW_LD_CFG_V2_s *pRfLdCfg, UINT8 *pWrkBufAddr);
UINT32 RefFlowLD_ProcessV2(const REF_FLOW_LD_IN_DATA_V2_s* pRfLdInData, REF_FLOW_LD_OUT_DATA_V2_s* pRfLdOutData);

#endif
