/**
 *  @file SvcCvFlow_AmbaOD.h
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
 *  @details Header of FlexiDAG based Ambarella Object Detection
 *
 */

#ifndef SVC_CV_FLOW_AMBA_OD_H
#define SVC_CV_FLOW_AMBA_OD_H

/* AmbaOD mode */
#define AMBAOD_FDAG_FC                          (0U)
#define AMBAOD_FDAG_FC_2SCALE                   (1U)
#define AMBAOD_FDAG_EMR                         (2U)
#define AMBAOD_FDAG_EMR_SMALL                   (3U)    /* 1024 x 512 */
#define AMBAOD_FDAG_PCPT_OD                     (4U)    /* OD(2D) */
#define AMBAOD_FDAG_PCPT_OD_2SC                 (5U)    /* OD(2 scale) */
#define AMBAOD_FDAG_PCPT_OD_DET                 (6U)    /* OD(2D) and Detections(key point, 3D bbx, and instance segment) */
#define AMBAOD_FDAG_PCPT_OD_2SC_DET             (7U)    /* OD(2 scale) and Detections(key point, 3D bbx, and instance segment) */
#define AMBAOD_FDAG_PCPT_OD_CLF                 (8U)    /* OD(2D) and classifiers */
#define AMBAOD_FDAG_PCPT_OD_2SC_CLF             (9U)    /* OD(2 scale) and classifiers */
#define AMBAOD_FDAG_PCPT_OD_DET_CLF             (10U)   /* OD(2D) Detections(key point, 3D bbx, and instance segment), and classifiers */
#define AMBAOD_FDAG_PCPT_OD_2SC_DET_CLF         (11U)   /* OD(2 scale) Detections(key point, 3D bbx, and instance segment), and classifiers */
#define AMBAOD_FDAG_EMR_1408x512                (12U)   /* 1408 x 512 */
#define AMBAOD_FDAG_PCPT_OD_RES1                (13U)   /* OD(896 x 512, 1 scale) */
#define AMBAOD_FDAG_PCPT_OD_RES1_2SC            (14U)   /* OD(896 x 512, 2 scale) */
#define AMBAOD_FDAG_PCPT_OD_RES1_DET            (15U)   /* OD(896 x 512, 1 scale) Detections(key point, 3D bbx, and instance segment) */
#define AMBAOD_FDAG_PCPT_OD_RES1_CLF            (16U)   /* OD(896 x 512, 1 scale) and classifiers  */
#define AMBAOD_FDAG_PCPT_OD_RES1_2SC_CLF        (17U)   /* OD(896 x 512, 2 scale) and classifiers */
#define AMBAOD_FDAG_PCPT_OD_RES1_DET_CLF        (18U)   /* OD(896 x 512, 1 scale) Detections(key point, 3D bbx, and instance segment), and classifiers */
#define AMBAOD_FDAG_PCPT_OD_RES1_2SC_DET_CLF    (19U)   /* OD(896 x 512, 2 scale) Detections(key point, 3D bbx, and instance segment), and classifiers */
#define AMBAOD_FDAG_EMR_896x512                 (20U)   /* 896 x 512 */
#define AMBAOD_NUM_MODE                         (21U)

/* NN input dimension */
#define AMBAOD_FC_NET_WIDTH                     (1280U)
#define AMBAOD_FC_NET_HEIGHT                    (640U)
#define AMBAOD_FC_NET_WIDTH_RES1                (896U)
#define AMBAOD_FC_NET_HEIGHT_RES1               (512U)
#define AMBAOD_EMR_NET_WIDTH                    (1024U)
#define AMBAOD_EMR_NET_HEIGHT                   (512U)

/* CtrlType */

/* Output Type */
#define AMBAOD_OUTPUT_TYPE_DETBOX               (0U)    /* SVC_CV_DETRES_BBX_LIST_s */
#define AMBAOD_OUTPUT_TYPE_PERCEPTION           (1U)    /* SVC_CV_DETRES_BBX_LIST_s */

/* Flow Ctrl */
#define AMBAOD_FLOW_2STAGE                      (1U)

/* Status */
#define SVC_CV_STATUS_AMBAOD_PCPT_PREPROC       (0U)

typedef struct {
    UINT16  MaxResult;         /* max bounding box result */
    UINT16  ScoreTh;           /* score threshold */
    UINT16  IOUTh;             /* IOU threshold */
    UINT16  IOMTh;             /* IOM threshold */
} SVC_CV_FLOW_AMBAOD_NMS_PARAM_s;

#define AMBAOD_MAX_NMS_CONFIG                   (16U)
typedef struct {
    UINT16  NumConfig;         /* Number of config */
    SVC_CV_FLOW_AMBAOD_NMS_PARAM_s Param[AMBAOD_MAX_NMS_CONFIG];
} SVC_CV_FLOW_AMBAOD_NMS_CFG_s;

#define AMBAOD_PERCEPTION_INIT                  (1U)
#define AMBAOD_PERCEPTION_RUN                   (2U)
typedef struct {
    UINT8  EnableKP;
    UINT8  Enable3D;
    UINT8  EnableMask;
    UINT16 OutBaseWidth;      /* output base of instance segment */
    UINT16 OutBaseHeight;     /* output base of instance segment */

    UINT8  EnableTS;
    UINT8  EnableSL;
    UINT8  EnableTLC;
    UINT8  EnableTLS;
    UINT8  EnableAR;
} SVC_CV_FLOW_AMBAOD_PCPT_FEATURE_s;

#define AMBAOD_MAX_SCENE_MODE                   (3U)
typedef struct {
    UINT16  NetWidth;
    UINT16  NetHeight;
    SVC_CV_FLOW_AMBAOD_NMS_CFG_s  *pNmsCfg[AMBAOD_MAX_SCENE_MODE];
    void  *pFeatureCtrl;
} SVC_CV_FLOW_AMBAOD_CFG_s;

#define AMBAOD_MAX_ALGO_NUM                     (SVC_CV_FLOW_MAX_ALGO_PER_GRP)
typedef struct {
    UINT32  ProfID;
    SVC_CV_FLOW_ALGO_GROUP_s      AlgoGrp;
    UINT8   OutputType;
    SVC_CV_FLOW_AMBAOD_CFG_s      Cfg[AMBAOD_MAX_ALGO_NUM];
    UINT32  FlowCtrl;
} SVC_CV_FLOW_AMBAOD_MODE_INFO_s;

#define SVC_AMBAOD_MAX_PCPT_PROC                (AMBANET_OD37_AR + 1U)

/*-----------------------------------------------------------------------------------------------*\
 *  Defined in SvcCvFlow_AmbaOD.c
\*-----------------------------------------------------------------------------------------------*/
extern SVC_CV_FLOW_OBJ_s SvcCvFlow_AmbaODObj;

extern const SVC_CV_FLOW_AMBAOD_MODE_INFO_s SvcCvFlow_AmbaOD_ModeInfo[AMBAOD_NUM_MODE];

#endif /* SVC_CV_FLOW_AMBA_OD_H */
