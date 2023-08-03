/**
 *  @file SvcCvFlow_CnnTestbed.h
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
 *  @details Implementation of CvFlow Driver to hook up cnn_testbed SvcCvAlgo examples
 *
 */

#ifndef SVC_CV_FLOW_CNN_TESTBED_H
#define SVC_CV_FLOW_CNN_TESTBED_H

#include "AmbaTypes.h"
#include "AmbaDSP_Liveview.h"

#include "SvcCvAlgo.h"
#include "SSD.h"
#include "SingleFD.h"

#include "SvcCvFlowProfile.h"
#include "SvcResCfg.h"
#include "SvcCvFlow.h"
#include "SvcCvFlowAlgoUtil.h"


/* CnnTestbed mode */
#define TESTBED_CVALGO_SSD                      (0U)
#define TESTBED_CVALGO_SEG                      (1U)
#define TESTBED_NUM_MODE                        (2U)

/* CtrlType */
#define TESTBED_CVALGO_CTRL_SET_SSD_NMS         (0x0000U)
#define TESTBED_CVALGO_CTRL_SET_SEG_WIN         (0x0001U)

typedef struct {
  UINT32  RoiWidth;
  UINT32  RoiHeight;
  UINT32  RoiStartX;
  UINT32  RoiStartY;
  UINT32  NetworkWidth;
  UINT32  NetworkHeight;
} SEG_WIN_CTRL_s;

typedef struct {
    // General settings
    SVC_CV_ALGO_OBJ_s       *pAlgoObj;
    UINT32                  TxRateCtrl;
    UINT32                  OsdPriority;

    // Settings for SSD CvAlgo
    SSD_NMS_CTRL_s          SsdNmsCtrl;

    // Settings for SingleFD CvAlgo (Segmentation)
    SEG_WIN_CTRL_s          SegWinCtrl;
} SVC_CV_FLOW_CNN_TESTBED_MODE_INFO_s;

/*-----------------------------------------------------------------------------------------------*\
 *  Defined in SvcCvFlow_CnnTestbed.c
\*-----------------------------------------------------------------------------------------------*/
/**
 * CvFlowType: SVC_CV_FLOW_CNN_TESTBED
 * Arguments:
 * Init:       NULL
 * Config:     SVC_CV_FLOW_CFG_s
 * Load:       SVC_CV_FLOW_LOAD_INFO_s
 * Start:      NULL
 * Stop:       NULL
 * Control:    NULL
 * Output:     SVC_CV_DETRES_BBX_LIST_s
 */

extern SVC_CV_FLOW_OBJ_s SvcCvFlow_CnnTestbedObj;

extern const SVC_CV_FLOW_CNN_TESTBED_MODE_INFO_s SvcCvFlow_CnnTestbed_ModeInfo[TESTBED_NUM_MODE];

#endif /* SVC_CV_FLOW_CNN_TESTBED_H */
