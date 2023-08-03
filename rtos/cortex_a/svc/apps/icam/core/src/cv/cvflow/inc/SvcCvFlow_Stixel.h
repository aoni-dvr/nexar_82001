/**
 *  @file SvcCvFlow_Stixel.h
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
 *  @details Header of FlexiDAG based Stixel application
 *
 */

#ifndef SVC_CV_FLOW_STIXEL_H
#define SVC_CV_FLOW_STIXEL_H

#include "cvapi_svccvalgo_stixel.h"

/* Stixel mode */
#define STIXEL_MODE_1                           (0U)
#define STIXEL_NUM_MODE                         (1U)

/* NN input dimension */
#define STIXEL_OPEN_NET_WIDTH                   (1280U)
#define STIXEL_OPEN_NET_HEIGHT                  (640U)

/* CtrlType */
#define SVC_CV_CTRL_SEND_DSI_INPUT              (0x0U)
#define SVC_CV_CTRL_SET_ROAD_ESTIMATE           (0x1U)
#define SVC_CV_CTRL_GET_ROAD_ESTIMATE           (0x2U)

typedef struct {
    void    *pAddr;
    UINT32  Width;
    UINT32  Height;
    UINT32  Pitch;
    UINT32  FrameNum;
    UINT32  CaptureTime;
} SVC_CV_FLOW_STIXEL_DSI_INPUT_s;

/* Output Type */
#define STIXEL_OUTPUT_TYPE_STIXEL               (0U)

typedef struct {
    void                        *pStixelCfg;
    UINT16                      VDispOffsetY;
} SVC_CV_FLOW_STIXEL_CFG_s;

#define STIXEL_MAX_ALGO_NUM                     (SVC_CV_FLOW_MAX_ALGO_PER_GRP)
typedef struct {
    UINT32  ProfID;
    SVC_CV_FLOW_ALGO_GROUP_s     AlgoGrp;
    UINT8   OutputType;
    SVC_CV_FLOW_STIXEL_CFG_s     Cfg;
} SVC_CV_FLOW_STIXEL_MODE_INFO_s;

typedef struct {
    UINT32 Used;
    UINT16 RoadEstiMode;
    DOUBLE EstiCamHeight;
    DOUBLE EstiCamPitch;
} SVC_CV_FLOW_STIXEL_PREF_STIXEL_CFG_s;

typedef struct {
    UINT32 MsgCode;
    AMBA_CV_STIXEL_DET_OUT_s StixelOut;
    UINT32 FrameNum;
    UINT32 CaptureTime;
} SVC_CV_FLOW_STIXEL_OUTPUT_s;

/*-----------------------------------------------------------------------------------------------*\
 *  Defined in SvcCvFlow_Stixel.c
\*-----------------------------------------------------------------------------------------------*/
/**
 * CvFlowType: SVC_CV_FLOW_STIXEL
 * Arguments:
 * Init:       NULL
 * Config:     SVC_CV_FLOW_STIXEL_CONFIG_s
 * Load:       SVC_CV_FLOW_LOAD_INFO_s
 * Start:      NULL
 * Stop:       NULL
 * Control:    NULL
 * Output:     SVC_CV_DETRES_BBX_LIST_s
 */

extern SVC_CV_FLOW_OBJ_s SvcCvFlow_StixelObj;

extern const SVC_CV_FLOW_STIXEL_MODE_INFO_s SvcCvFlow_Stixel_ModeInfo[STIXEL_NUM_MODE];

#endif /* SVC_CV_FLOW_STIXEL_H */
