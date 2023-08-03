/**
 *  @file RefFlow_PSD.h
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
 *  @details header file of PSD reference flow
 *
 */


#ifndef REF_FLOW_PSD_H
#define REF_FLOW_PSD_H

#include "RefFlow_Common.h"
#include "AmbaPSD_ParkingSpaceDetect.h"
#include "AmbaPSD_CornerTrace.h"

#ifndef AMBA_TYPES_H
#include "AmbaTypes.h"
#endif



/******************************************************************/
/** Data Type for log                                             */
/******************************************************************/
/** refer to AMBA_PSD_PROC_INPUT_DATA_s */
#define RF_PSD_DFMT_DETECT_IN_V000         (0x00000100U) /** 0x00000100 to 0x000001FF: Reserved for AMBA_PSD_PROC_INPUT_DATA_s */
#define RF_PSD_DFMT_DETECT_IN             (RF_FUNC_PSD | RF_PSD_DFMT_DETECT_IN_V000)
/** refer to AMBA_PSD_PROC_INPUT_DATA_s -> SegBufInfo.pBuf */
#define RF_PSD_DFMT_DETECT_IN_BUF_V000     (0x00000200U) /** 0x00000200 to 0x000002FF: Reserved for AMBA_PSD_PROC_INPUT_DATA_s -> SegBufInfo.pBuf */
#define RF_PSD_DFMT_DETECT_IN_BUF         (RF_FUNC_PSD | RF_PSD_DFMT_DETECT_IN_BUF_V000)
/** refer to AMBA_PSD_PROC_OUTPUT_DATA_s */
#define RF_PSD_DFMT_DETECT_OUT_V000        (0x00000300U) /** 0x00000300 to 0x000003FF: Reserved for AMBA_PSD_PROC_OUTPUT_DATA_s */
#define RF_PSD_DFMT_DETECT_OUT            (RF_FUNC_PSD | RF_PSD_DFMT_DETECT_OUT_V000)


/******************************************************************/
/** Other define                                                  */
/******************************************************************/
/** Define for fisheye camera channel index */
#define RF_PSD_CHANNEL_FRONT                      (AMBA_PSD_CHANNEL_FRONT)
#define RF_PSD_CHANNEL_BACK                       (AMBA_PSD_CHANNEL_BACK)
#define RF_PSD_CHANNEL_LEFT                       (AMBA_PSD_CHANNEL_LEFT)
#define RF_PSD_CHANNEL_RIGHT                      (AMBA_PSD_CHANNEL_RIGHT)
#define RF_PSD_CHANNEL_MAX_NUM                    (AMBA_PSD_CHANNEL_MAX_NUM)


typedef struct {
    AMBA_PSD_CFG_s PsdCfg;                                                  /**< PSD config */
    AMBA_CAL_AVM_CALIB_DATA_s CalibData[RF_PSD_CHANNEL_MAX_NUM];            /**< Avm Calibration data */
    AMBA_CAL_AVM_PARK_AST_CFG_V1_s PACfgV1[RF_PSD_CHANNEL_MAX_NUM];         /**< AVM Park Ast Cfg V1 */
    REF_FLOW_LOG_FP LogFp;                                                  /**< Register callback function for logging data */
} REF_FLOW_PSD_CFG_s;

typedef struct {
    AMBA_PSD_PROC_INPUT_DATA_s PsdInData;         /**< PSD process input data */
} REF_FLOW_PSD_IN_DATA_s;

typedef struct {
    AMBA_PSD_PROC_OUTPUT_DATA_s PsdOutData;         /**< PSD process output data */
    AMBA_PSD_PROC_OUTPUT_DATA_s PostTrackerData;    /**< Parking space data with posted tracker (invalid at current version) */
} REF_FLOW_PSD_OUT_DATA_s;


UINT32 RefFlowPSD_GetDefaultCfg(UINT32 SegBufImgW, UINT32 SegBufImgH, UINT32 *WrkBufSize, REF_FLOW_PSD_CFG_s *pRefPsdCfg);

UINT32 RefFlowPSD_Init(REF_FLOW_PSD_CFG_s *pRefPsdCfg, UINT8 *pWrkBufAddr);

UINT32 RefFlowPSD_Process(UINT32 Channel, const REF_FLOW_PSD_IN_DATA_s *pRefPsdInData, REF_FLOW_PSD_OUT_DATA_s *pRefPsdOutData);


#endif
