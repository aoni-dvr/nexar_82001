/**
*  @file SSD.h
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
*   @details The SSD-like algorithm for flexidag
*
*/

/******************************************************************************
 *  SCA spec - SSD
 ******************************************************************************
 *  [Description]
 *      - This SCA takes a SSD-like flexidag as input and do NMS/fusion
 *        at ARM side. The output result is detected bounding boxes.
 *
 *  [Flexidag Info]
 *      - Number : 1
 *      - FD#1
 *          - 01000_mnet_ssd_adas_cf
 *
 *  [Output Format]
 *      - CVALGO_SSD_OUTPUT_s
 *
 *  [Extend Config]
 *      - pExtQueryCfg   : None
 *      - pExtCreateCfg  : None
 *      - pExtDeleteCfg  : None
 *      - pExtFeedCfg    : None
 *      - pExtRegcbCfg   : None
 *
 *  [Supported Control]
 *      - CTRL_TYPE_ROI     : pCtrlParam = amba_roi_config_t
 *      - CTRL_TYPE_DMSG    : pCtrlParam = NULL
 *      - CTRL_TYPE_SSD_NMS : pCtrlParam = SSD_NMS_CTRL_s
 */

#ifndef SSD_H
#define SSD_H

#include "SvcCvAlgo.h"
#include "CvCommFlexi.h"

#include "ArmSsdFusion.h"

#define CTRL_TYPE_SSD_NMS         (CTRL_TYPE_USER_BASE + 1U)    // param = SSD_NMS_CTRL_s

#define MAX_PBOX_FN_LEN           (128U)
#define MAX_CVALGO_SSD_BOX_NUM    (100U)

typedef struct {
    UINT32  RoiWidth;       // The size of selected pyramid yuv
    UINT32  RoiHeight;
    UINT32  RoiStartX;
    UINT32  RoiStartY;
    UINT32  NetworkWidth;   // The input size of the network
    UINT32  NetworkHeight;
    UINT32  VisWidth;       // The size of normalized output
    UINT32  VisHeight;
} SSD_WIN_SETTING_s;

typedef struct {
    char      PriorBox[MAX_PBOX_FN_LEN];
    UINT32    ClassNum;
    UINT32    ModelType;
    SSD_WIN_SETTING_s  Win;
} SSD_NMS_CTRL_s;

typedef struct {
    UINT32 Class;
    FLOAT  Score;
    UINT32 X;        // The X,Y,W,H is based on (VisWidth, VisHeight) domain
    UINT32 Y;
    UINT32 W;
    UINT32 H;
} CVALGO_SSD_BOX_s;

typedef struct {
    UINT32               BoxNum;
    CVALGO_SSD_BOX_s     Box[MAX_CVALGO_SSD_BOX_NUM];
} CVALGO_SSD_OUTPUT_s;

/******************************************************************************
 *  Defined in SSD.c
 ******************************************************************************/
extern SVC_CV_ALGO_OBJ_s CvAlgoObj_SSD;   // output struct = CVALGO_SSD_OUTPUT_s

#endif /* SSD_H */

