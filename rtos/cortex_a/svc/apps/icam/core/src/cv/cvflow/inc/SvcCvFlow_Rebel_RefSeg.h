/**
 *  @file SvcCvFlow_Rebel_RefSeg.h
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
 *  @details Header of FlexiDAG based Reference Segmentation
 *
 */

#ifndef SVC_CV_FLOW_REBEL_REF_SEG_H
#define SVC_CV_FLOW_REBEL_REF_SEG_H

#define SVC_CV_DETRES_SEG_REBEL                 (0x0008U)

#define SVC_CV_DETRES_MAX_BBX_NUM        (150U)
#define SVC_CV_DETRES_MAX_LANE_NUM       (15U)  //ST Max Lane number
#define SVC_CV_DETRES_MAX_LANE_POINT_NUM (500U) //ST Max point number in each lane


/* Mode */
#define REBEL_FDAG_LINUX_SEG_RESERVE1    (0U)
#define REBEL_FDAG_LINUX_SEG_RESERVE2    (1U)
#define REBEL_FDAG_LINUX_OPENSEG         (2U)
#define REBEL_NUM_MODE_SEG               (3U) //Need to sync REFSEG_NUM_MODE value

/* Output Type */
#define REBEL_OUTPUT_ALGO_SEG            (2U)

typedef struct {
    UINT32 type;
    UINT32 length;
    UINT8  buff[320U * 1280U];
    UINT16 buff_height;
    UINT16 buff_width;
    UINT16 pyramid_id;
    UINT16 mask_start_col;
    UINT16 mask_start_row;
    UINT16 mask_width;
    UINT16 mask_height;
} SVC_CV_SEG_OUT_s;

typedef struct {
    SVC_POINT_2D_s point[500];
    UINT32 sampled_point_num;

    FLOAT ParameterC0;
    FLOAT ParameterC1;
    FLOAT ParameterC2;
    FLOAT ParameterC3;
    UINT8  Type;
    FLOAT  Range;
} SVC_CV_LANE_CANDIDATE_REBEL_s;

typedef struct {
    UINT32 MsgCode;
    UINT32 FrameNum;
    UINT32 CaptureTime;
    UINT32 TotalLaneNum;
    UINT32 SampledPointNum[SVC_CV_DETRES_MAX_LANE_NUM];
    SVC_POINT_2D_s point[SVC_CV_DETRES_MAX_LANE_NUM][SVC_CV_DETRES_MAX_LANE_POINT_NUM];
    FLOAT ParameterC0[SVC_CV_DETRES_MAX_LANE_NUM];
    FLOAT ParameterC1[SVC_CV_DETRES_MAX_LANE_NUM];
    FLOAT ParameterC2[SVC_CV_DETRES_MAX_LANE_NUM];
    FLOAT ParameterC3[SVC_CV_DETRES_MAX_LANE_NUM];
    UINT8  Type[SVC_CV_DETRES_MAX_LANE_NUM];
    FLOAT  Range[SVC_CV_DETRES_MAX_LANE_NUM];
    SVC_CV_SEG_OUT_s SegResult;
    void   *pCvOut;
} SVC_CV_DETRES_SEG_REBEL_s;

typedef struct {
    UINT32   type;
    UINT32   length;
    UINT32   num_objects;
    rptr32_t objects_offset;
    UINT32   frame_num;
    UINT32   capture_time;
} amba_lane_out_t;

#endif /* SVC_CV_FLOW_REBEL_REF_SEG_H */
