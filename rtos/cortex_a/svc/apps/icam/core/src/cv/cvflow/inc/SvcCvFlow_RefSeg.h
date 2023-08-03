/**
 *  @file SvcCvFlow_RefSeg.h
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

#ifndef SVC_CV_FLOW_REF_SEG_H
#define SVC_CV_FLOW_REF_SEG_H

/* RefSeg mode */
#define REFSEG_FDAG_OPEN_NET_1                  (0U)
#define REFSEG_FDAG_LINUX_SEGNET                (1U)    /* Linux SegNet example */
#define REFSEG_FDAG_LINUX_OPENSEG               (2U)    /* Linux OpenSeg example */
#define REFSEG_NUM_MODE                         (3U)


/* NN input dimension */
#define REFSEG_OPEN_NET_WIDTH                   (1280U)
#define REFSEG_OPEN_NET_HEIGHT                  (512U)
#define REFSEG_SEG_NET_WIDTH                    (480U)
#define REFSEG_SEG_NET_HEIGHT                   (360U)

/* CtrlType */

/* Output Type */
#define REFSEG_OUTPUT_TYPE_MASK_OUT             (0U)    /* SVC_CV_DETRES_SEG_s */
#define REFSEG_OUTPUT_TYPE_ALGO_LINUX           (1U)    /* SVC_CV_DETRES_SEG_s */

typedef struct {
    UINT16  NumCat;
    UINT16  BufWidth;
    UINT16  BufHeight;
} SVC_CV_FLOW_REFSEG_MASK_CFG_s;

typedef struct {
    UINT16  NetWidth;
    UINT16  NetHeight;
    SVC_CV_FLOW_REFSEG_MASK_CFG_s *pMaskCfg;
} SVC_CV_FLOW_REFSEG_CFG_s;

#define REFSEG_MAX_ALGO_NUM                     (SVC_CV_FLOW_MAX_ALGO_PER_GRP)
typedef struct {
    UINT32  ProfID;
    SVC_CV_FLOW_ALGO_GROUP_s       AlgoGrp;
    UINT8   OutputType;
    UINT8   OutputBufIdx;
    SVC_CV_FLOW_REFSEG_CFG_s       Cfg[REFSEG_MAX_ALGO_NUM];
} SVC_CV_FLOW_REFSEG_MODE_INFO_s;

/*-----------------------------------------------------------------------------------------------*\
 *  Defined in SvcCvFlow_RefSeg.c
\*-----------------------------------------------------------------------------------------------*/
extern SVC_CV_FLOW_OBJ_s SvcCvFlow_RefSegObj;

extern const SVC_CV_FLOW_REFSEG_MODE_INFO_s SvcCvFlow_RefSeg_ModeInfo[REFSEG_NUM_MODE];

#endif /* SVC_CV_FLOW_REF_SEG_H */
