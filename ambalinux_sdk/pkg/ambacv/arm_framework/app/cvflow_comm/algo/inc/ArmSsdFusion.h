/**
*  @file ArmSsdFusion.h
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
*   @details The SSD fusion functions
*
*/

#ifndef ARM_SSD_FUSION_H
#define ARM_SSD_FUSION_H

#define CLASS_DET_REGION   (255)

/*
 *  MODEL_TYPE_CAFFE :
 *    - The arrangement of priorbox
 *      [prior_loc(x1,y1,x2,y2), prior_loc(x1,y1,x2,y2), ..., prior_var(x,y,w,h), prior_var(x,y,w,h), ...]
 *    - The arrangement of location output
 *      [det_loc(x,y,w,h), det_loc(x,y,w,h), ...]
 *
 *  MODEL_TYPE_TF :
 *    - The arrangement of priorbox
 *      [prior_loc(y1,x1,y2,x2), prior_loc(y1,x1,y2,x2), ..., prior_var(y,x,h,w), prior_var(y,x,h,w), ...]
 *    - The arrangement of location output
 *      [det_loc(y,x,h,w), det_loc(y,x,h,w), ...]
 */

#define MODEL_TYPE_CAFFE    (0U)
#define MODEL_TYPE_TF       (1U)
#define MAX_MODEL_TYPE_NUM  (2U)

typedef struct {
    FLOAT* pPriorbox;
    FLOAT* pLoc;
    FLOAT* pConf;
} NMS_INPUT_s;

typedef struct {
    UINT32 PriorBoxNum;
    UINT32 ClassNum;
    UINT32 TopK;
    UINT32 NmsTopK;
    FLOAT  ConfThrd;
    FLOAT  NmsThrd;
    UINT32 ModelType;
} NMS_CFG_s;

typedef struct {
    UINT32 Class;
    FLOAT  Score;
    FLOAT  Xmin;
    FLOAT  Ymin;
    FLOAT  Xmax;
    FLOAT  Ymax;
} NMS_BOX_s;

typedef struct {
    UINT32    NmsBoxNum;
    NMS_BOX_s *pNmsBox;
} NMS_OUTPUT_s;

typedef struct {
    UINT32 DecBoxIdx;
    FLOAT  Score;
} SORT_BOX_s;

typedef struct {
    UINT32 RoiWidth;
    UINT32 RoiHeight;

    UINT32 RoiStartX;
    UINT32 RoiStartY;

    UINT32 NetworkWidth;
    UINT32 NetworkHeight;

    UINT32 VisWidth;   // The output window domain (It could be osd or raw)
    UINT32 VisHeight;

    UINT8  ShowDetRegion;
    FLOAT  VisThrd;
} VIS_CFG_s;


typedef struct {
    UINT32 Class;
    FLOAT  Score;
    UINT32 X;      // The X,Y,W,H is based on (vis_w,vis_h) domain
    UINT32 Y;
    UINT32 W;
    UINT32 H;
} VIS_BOX_s;

typedef struct {
    UINT32    VisBoxNum;
    VIS_BOX_s *pVisBox;
} VIS_OUTPUT_s;

UINT32 ArmSsdFusion_NmsCalc(NMS_INPUT_s NmsInput, NMS_CFG_s NmsCfg, NMS_OUTPUT_s *pNmsOutput);
UINT32 ArmSsdFusion_VisCalc(NMS_OUTPUT_s NmsOutput, VIS_CFG_s VisCfg, VIS_OUTPUT_s *pVisOutput);

#endif  /* ARM_SSD_FUSION_H */
