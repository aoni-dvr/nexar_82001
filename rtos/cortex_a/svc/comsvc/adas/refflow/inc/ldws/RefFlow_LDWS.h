/**
 *  @file RefFlow_LDWS.h
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
 *  @details header file of LDWS reference flow
 *
 */

#ifndef REF_FLOW_LDWS_H
#define REF_FLOW_LDWS_H

#include "RefFlow_Common.h"
#include "AmbaSLD_LaneDetect.h"
#include "AmbaSR_Lane.h"
#include "AmbaWS_LDWS.h"

#ifndef AMBA_TYPES_H
#include "AmbaTypes.h"
#endif

/** refer to AMBA_SLD_PROC_INPUT_DATA_s */
#define RF_LDWS_DFMT_DETECT_IN_V000         (0x00000100U) /** 0x00000100 to 0x000001FF: Reserved for AMBA_SLD_PROC_INPUT_DATA_s */
#define RF_LDWS_DFMT_DETECT_IN              (RF_FUNC_LDWS | RF_LDWS_DFMT_DETECT_IN_V000)
/** refer to AMBA_SLD_PROC_INPUT_DATA_s -> pSegBufInfo.pBuf */
#define RF_LDWS_DFMT_DETECT_IN_BUF_V000     (0x00000200U) /** 0x00000200 to 0x000002FF: Reserved for AMBA_SLD_PROC_INPUT_DATA_s -> pSegBufInfo.pBuf */
#define RF_LDWS_DFMT_DETECT_IN_BUF          (RF_FUNC_LDWS | RF_LDWS_DFMT_DETECT_IN_BUF_V000)
/** refer to AMBA_SLD_LANE_PNT_DATA_s */
#define RF_LDWS_DFMT_SLD_OUT_V000           (0x00000300U) /** 0x00000300 to 0x000003FF: Reserved for AMBA_SLD_LANE_PNT_DATA_s */
#define RF_LDWS_DFMT_SLD_OUT                (RF_FUNC_LDWS | RF_LDWS_DFMT_SLD_OUT_V000)
/** refer to AMBA_SR_LANE_FUNC_DATA_s */
#define RF_LDWS_DFMT_SR_OUT_V000            (0x00000400U) /** 0x00000400 to 0x000004FF: Reserved for AMBA_SR_LANE_FUNC_DATA_s */
#define RF_LDWS_DFMT_SR_OUT                 (RF_FUNC_LDWS | RF_LDWS_DFMT_SR_OUT_V000)
/** refer to AMBA_LDWS_DEPARTURE_INFO_s */
#define RF_LDWS_DFMT_DETECT_OUT_V000        (0x00000500U) /** 0x00000500 to 0x000005FF: Reserved for AMBA_LDWS_DEPARTURE_INFO_s */
#define RF_LDWS_DFMT_DETECT_OUT             (RF_FUNC_LDWS | RF_LDWS_DFMT_DETECT_OUT_V000)
/** refer to REF_FLOW_LDWS_CFG_s */
#define RF_LDWS_DFMT_REFFLOW_CFG_V000       (0x00000600U) /** 0x00000600 to 0x000006FF: Reserved for REF_FLOW_LDWS_CFG_s */
#define RF_LDWS_DFMT_REFFLOW_CFG            (RF_FUNC_LDWS | RF_LDWS_DFMT_REFFLOW_CFG_V000)
/** refer to AMBA_SR_LANE_RECONSTRUCT_INFO_s */
#define RF_LDWS_DFMT_MULTI_LANE_OUT_V000    (0x00000700U) /** 0x00000700 to 0x000007FF: Reserved for AMBA_SR_LANE_RECONSTRUCT_INFO_s */
#define RF_LDWS_DFMT_MULTI_LANE_OUT         (RF_FUNC_LDWS | RF_LDWS_DFMT_MULTI_LANE_OUT_V000)

typedef struct {
    AMBA_CAL_EM_CALC_COORD_CFG_s CalibCfg;              /**< Calibration config, please refer to AmbaCalib_EmirrorDef.h */
    AMBA_SLD_CFG_DATA_s SldCfg;                         /**< Config for SLD, please refer to AmbaSLD_LaneDetect.h*/
    AMBA_LDWS_CFG_DATA_s LdwsCfg;                       /**< Config for LDWS, please refer to AmbaWS_LDWS.h*/
    REF_FLOW_LOG_FP LogFp;                              /**< Regist callback function for logging data */
} REF_FLOW_LDWS_CFG_s;


typedef struct {
    AMBA_SLD_PROC_INPUT_DATA_s SldInData;               /**< Segmentation lane detection input data*/
    AMBA_SR_CANBUS_TRANSFER_DATA_s CanbusTrData;        /**< Canbus information */
} REF_FLOW_LDWS_IN_DATA_s;

typedef struct {
    AMBA_LDWS_DEPARTURE_INFO_s LdwsDepRes;              /**< Lane departure warning system result */
} REF_FLOW_LDWS_OUT_DATA_s;

UINT32 RefFlowLDWS_GetDefaultCfg(UINT32 SegBufImgW, UINT32 SegBufImgH, UINT32 *WrkBufSize, REF_FLOW_LDWS_CFG_s *pRfLdwsCfg);

UINT32 RefFlowLDWS_Init(REF_FLOW_LDWS_CFG_s *pRfLdwsCfg, UINT8 *pWrkBufAddr);

UINT32 RefFlowLDWS_Process(REF_FLOW_LDWS_IN_DATA_s* pRfLdwsInData, REF_FLOW_LDWS_OUT_DATA_s* pRfLdwsOutData);


#endif
