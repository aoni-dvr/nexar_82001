/**
 *  @file RefFlow_RCTA.h
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
 *  @details header file of RCTA reference flow
 *
 */

#ifndef REF_FLOW_RCTA_H
#define REF_FLOW_RCTA_H

#include "RefFlow_Common.h"
#ifndef AMBA_WS_RCTA_H
#include "AmbaWS_RCTA.h"
#endif
#ifndef AMBA_SR_SEG_CROSS_VEHICLE_H
#include "AmbaSR_SegCrossVehicle.h"
#endif

/** Define for fisheye camera channel index */
#define RF_RCTA_CHANNEL_FRONT    (AMBA_SR_SCV_CHANNEL_FRONT)
#define RF_RCTA_CHANNEL_REAR     (AMBA_SR_SCV_CHANNEL_REAR)
#define RF_RCTA_CHANNEL_MAX_NUM  (AMBA_SR_SCV_CHANNEL_MAX_NUM)

/** refer to AMBA_SR_SCV_PROC_INPUT_DATA_s */
#define RF_RCTA_DFMT_DETECT_IN_V000         (0x00000100U) /** 0x00000100 to 0x000001FF: Reserved for AMBA_SR_SCV_PROC_INPUT_DATA_s */
#define RF_RCTA_DFMT_DETECT_IN              (RF_FUNC_RCTA | RF_RCTA_DFMT_DETECT_IN_V000)
/** refer to AMBA_SR_SCV_PROC_INPUT_DATA_s -> SegBufInfo.pBuf */
#define RF_RCTA_DFMT_DETECT_IN_BUF_V000     (0x00000200U) /** 0x00000200 to 0x000002FF: Reserved for AMBA_SR_SCV_PROC_INPUT_DATA_s -> SegBufInfo.pBuf */
#define RF_RCTA_DFMT_DETECT_IN_BUF          (RF_FUNC_RCTA | RF_RCTA_DFMT_DETECT_IN_BUF_V000)
/** refer to AMBA_SR_SCV_OBJECT_DATA_LIST_s */
#define RF_RCTA_DFMT_SCV_OUT_V000           (0x00000300U) /** 0x00000300 to 0x000003FF: Reserved for AMBA_SR_SCV_OBJECT_DATA_LIST_s */
#define RF_RCTA_DFMT_SCV_OUT                (RF_FUNC_RCTA | RF_RCTA_DFMT_SCV_OUT_V000)

typedef struct {
    AMBA_CAL_AVM_CALIB_DATA_s CalibData;        /**< AVM Calibration config, please refer to AmbaCalib_AVMIF.h */
    AMBA_CAL_AVM_PARK_AST_CFG_V1_s PACfgV1;     /**< AVM Parking Assistance Config, please refer to AmbaCalib_AVMIF.h */
    AMBA_SR_SCV_CFG_DATA_s ScvSRCfg;            /**< Config for SCV, please refer to AmbaSR_SegCrossVehicle.h */
    AMBA_WS_RCTA_CFG_DATA_s RctaWSCfg;          /**< Config for RCTA WS, please refer to AmbaWS_RCTA.h */
    REF_FLOW_LOG_FP LogFp;                      /**< Regist callback function for logging data */
} REF_FLOW_RCTA_CFG_s;


typedef struct {
    AMBA_SR_SCV_PROC_INPUT_DATA_s ScvInData;                /**< Segmentation cross vehicle detection input data */
    AMBA_SR_CANBUS_TRANSFER_DATA_s CanbusTrData;            /**< Canbus information */
} REF_FLOW_RCTA_IN_DATA_s;

typedef struct {
    AMBA_WS_RCTA_WARNING_INFO_s WarnInfoL;        /**< Warning information of left side(world) >*/
    AMBA_WS_RCTA_WARNING_INFO_s WarnInfoR;        /**< Warning information of right side(world) >*/
} REF_FLOW_RCTA_OUT_DATA_s;


UINT32 RefFlowRCTA_GetDefaultCfg(UINT32 SegBufImgW, UINT32 SegBufImgH, UINT32* pWrkBufSize, REF_FLOW_RCTA_CFG_s* pRfRctaCfg);

UINT32 RefFlowRCTA_Init(UINT32 Channel, REF_FLOW_RCTA_CFG_s* pRfRctaCfg, UINT8* pWrkBufAddr);

UINT32 RefFlowRCTA_Process(UINT32 Channel, const REF_FLOW_RCTA_IN_DATA_s* pRfRctaInData, REF_FLOW_RCTA_OUT_DATA_s* pRfRctaOutData);

#endif
