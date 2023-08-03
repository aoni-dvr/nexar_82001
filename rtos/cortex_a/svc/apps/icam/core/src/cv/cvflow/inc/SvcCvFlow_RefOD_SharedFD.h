/**
 *  @file SvcCvFlow_RefOD_SharedFD.h
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
 *  @details Header of FlexiDAG based Reference Object Detection with shared FlexiDAG
 *
 */

#ifndef SVC_CV_FLOW_REF_OD_SHFD_H
#define SVC_CV_FLOW_REF_OD_SHFD_H

/* RefOD mode */
#define REFOD_FDAG_OPEN_NET_1                   (0U)
#define REFOD_SHFD_NUM_MODE                     (1U)

/* NN input dimension */
#define REFOD_SHFD_OPEN_NET_WIDTH               (1024U)
#define REFOD_SHFD_OPEN_NET_HEIGHT              (640U)

/* CtrlType */

/* Output Type */
#define REFOD_SHFD_OUTPUT_TYPE_DETBOX           (0U)    /* SVC_CV_DETRES_BBX_LIST_s */
#define REFOD_SHFD_OUTPUT_TYPE_OSDBUF           (1U)    /* SVC_CV_OUT_BUF_s */
#define REFOD_SHFD_OUTPUT_TYPE_SSD_DETBOX       (2U)    /* SVC_CV_DETRES_BBX_LIST_s */

/* Bin Type */
#define REFOD_SHFD_BIN_TYPE_PRIOR_BOX           (0U)

typedef struct {
    UINT16  MaxResult;         /* max bounding box result */
    UINT16  ScoreTh;           /* score threshold */
} SVC_CV_FLOW_REFOD_SHFD_NMS_PARAM_s;

#define REFOD_SHFD_MAX_NMS_CONFIG               (13U)
typedef struct {
    UINT16  NumConfig;         /* Number of config */
    SVC_CV_FLOW_REFOD_SHFD_NMS_PARAM_s Param[REFOD_SHFD_MAX_NMS_CONFIG];
} SVC_CV_FLOW_REFOD_SHFD_NMS_CFG_s;

#define REFOD_SHFD_MAX_SCENE_MODE               (1U)
typedef struct {
    UINT16  NetWidth;
    UINT16  NetHeight;
    SVC_CV_FLOW_REFOD_SHFD_NMS_CFG_s *pNmsCfg[REFOD_SHFD_MAX_SCENE_MODE];
} SVC_CV_FLOW_REFOD_SHFD_CFG_s;

#define REFOD_SHFD_MAX_ALGO_NUM                 (1U)
typedef struct {
    UINT32  ProfID;
    SVC_CV_FLOW_ALGO_GROUP_s     AlgoGrp;
    UINT8   OutputType;
    UINT8   OutputBufIdx;
    SVC_CV_FLOW_REFOD_SHFD_CFG_s      Cfg[REFOD_SHFD_MAX_ALGO_NUM];
} SVC_CV_FLOW_REFOD_SHFD_MODE_INFO_s;

/*-----------------------------------------------------------------------------------------------*\
 *  Defined in SvcCvFlow_REFOD_SHFD.c
\*-----------------------------------------------------------------------------------------------*/
extern SVC_CV_FLOW_OBJ_s SvcCvFlow_RefOD_SHFD_Obj;

extern const SVC_CV_FLOW_REFOD_SHFD_MODE_INFO_s SvcCvFlow_RefOD_SHFD_ModeInfo[REFOD_SHFD_NUM_MODE];

#endif /* SVC_CV_FLOW_REF_OD_SHFD_H */
