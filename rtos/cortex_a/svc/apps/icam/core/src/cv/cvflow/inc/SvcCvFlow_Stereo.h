/**
 *  @file SvcCvFlow_Stereo.h
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
 *  @details Header of FlexiDAG based Reference Object Detection
 *
 */

#ifndef SVC_CV_FLOW_STEREO_H
#define SVC_CV_FLOW_STEREO_H
#include "SvcCvFlow_Comm.h"

/* STEREO mode */
#define STEREO_SCA_SP_FE_1                         (0U)
#define STEREO_SCA_SP_FS_3840x1920_L024_1          (1U)
#define STEREO_SCA_SP_FE_UT_1080P                  (2U)
#define STEREO_SCA_SP_FE_UT_4K                     (3U)
#define STEREO_SCA_FS_02_UT                        (4U)
#define STEREO_SCA_FS_024_UT                       (5U)
#define STEREO_SCA_LINUX_1                         (6U)
#define STEREO_NUM_MODE                            (7U)

/* CtrlType */
#define SVC_CV_CTRL_SEND_EXT_PIC_INFO           (0x0U)
#define SVC_CV_CTRL_SEND_FUSION_INFO            (0x1U)
#define SVC_CV_CTRL_GET_FUSION_INFO             (0x2U)

/* Output Type */
#define STEREO_OUT_TYPE_SPU_FEX                 (0U)
#define STEREO_OUT_TYPE_FUSION                  (1U)

/* MISC */
#define STEREO_FDAG_NUM_SPU_FEX_SCALE           (MAX_HALF_OCTAVES)

typedef struct {
    UINT8                   ScaleIdx;       /* Index of pyramid */
    UINT8                   Enable;
    UINT8                   RoiEnable;
    UINT16                  StartCol;
    UINT16                  StartRow;
    UINT16                  RoiWidth;
    UINT16                  RoiHeight;
    UINT8                   FusionDisparity;
} SVC_CV_FLOW_STEREO_SPU_SCALE_INFO_s;

typedef struct {
    UINT32                  NumScale;
    SVC_CV_FLOW_STEREO_SPU_SCALE_INFO_s *pScaleInfo;
} SVC_CV_FLOW_STEREO_SPU_CFG_s;

typedef struct {
    UINT8                   ScaleIdx;       /* Index of pyramid */
    UINT8                   Enable;
    UINT8                   RoiEnable;
    UINT16                  StartCol;
    UINT16                  StartRow;
    UINT16                  RoiWidth;
    UINT16                  RoiHeight;
    UINT16                  SecondaryRoiColOffset;
    UINT16                  SecondaryRoiRowOffset;
} SVC_CV_FLOW_STEREO_FEX_SCALE_INFO_s;

typedef struct {
    UINT32                  NumScale;
    SVC_CV_FLOW_STEREO_FEX_SCALE_INFO_s *pScaleInfo;
} SVC_CV_FLOW_STEREO_FEX_CFG_s;

typedef struct {
    UINT8                   ScaleIdx;       /* Index of pyramid */
} SVC_CV_FLOW_STEREO_FUS_INFO_s;

/* The config is for applicaiotn to know the output information. */
typedef struct {
    UINT32                  NumScale;
    SVC_CV_FLOW_STEREO_FUS_INFO_s *pFusInfo;
} SVC_CV_FLOW_STEREO_FUS_CFG_s;

#define STEREO_MAX_ALGO_NUM                      (2U)
typedef struct {
    UINT32  ProfID;
    SVC_CV_FLOW_ALGO_GROUP_s          AlgoGrp;
    UINT8   OutputType;
    SVC_CV_FLOW_STEREO_SPU_CFG_s      SpuCfg;
    SVC_CV_FLOW_STEREO_FEX_CFG_s      FexCfg;
    SVC_CV_FLOW_STEREO_FUS_CFG_s      FusCfg;
    UINT32  EnableFusion;
} SVC_CV_FLOW_STEREO_MODE_INFO_s;

typedef struct {
    SVC_CV_STEREO_OUTPUT_s      *pStereoOut;
    memio_source_recv_picinfo_t *pPicInfo;
} STEREO_RX1_MSG_s;

/*-----------------------------------------------------------------------------------------------*\
 *  Defined in SvcCvFlow_Stereo.c
\*-----------------------------------------------------------------------------------------------*/
/**
 * CvFlowType: SVC_CV_FLOW_REFOD
 * Arguments:
 * Init:       NULL
 * Config:     SVC_CV_FLOW_STEREO_CONFIG_s
 * Load:       SVC_CV_FLOW_LOAD_INFO_s
 * Start:      NULL
 * Stop:       NULL
 * Control:    NULL
 * Output:     SVC_CV_DETRES_BBX_LIST_s
 */

extern SVC_CV_FLOW_OBJ_s SvcCvFlow_StereoObj;

extern const SVC_CV_FLOW_STEREO_MODE_INFO_s SvcCvFlow_Stereo_ModeInfo[STEREO_NUM_MODE];

#endif /* SVC_CV_FLOW_REF_OD_H */
