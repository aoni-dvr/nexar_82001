/**
 *  @file RfcnProc.h
 *
 *  @copyright Copyright (c) 2018 Ambarella, Inc.
 *
 *  This file and its contents ("Software") are protected by intellectual property rights including, without limitation,
 *  U.S. and/or foreign copyrights.  This Software is also the confidential and proprietary information of Ambarella, Inc.
 *  and its licensors.  You may not use, reproduce, disclose, distribute, modify, or otherwise prepare derivative works
 *  of this Software or any portion thereof except pursuant to a signed license agreement or nondisclosure agreement with
 *  Ambarella, Inc. or its authorized affiliates.  In the absence of such an agreement, you agree to promptly notify and
 *  return this Software to Ambarella, Inc.
 *
 *  THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 *  WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL
 *  AMBARELLA, INC. OR ITS AFFILIATES BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 *  DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 *  COMPUTER FAILURE OR MALFUNCTION; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 *  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 *  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *  @details The RFCN process algorithm
 *
 */

#ifndef RFCN_PROC_H
#define RFCN_PROC_H

#include "SvcCvAlgo.h"

#define CTRL_TYPE_RFCN_PROC_PARAM   (CTRL_TYPE_USER_BASE + 1U)
#define MAX_CVALGO_RFCN_BOX_NUM     (200U)

typedef struct {
    UINT32 ImgHeight;
    UINT32 ImgWidth;
    UINT32 RoiWidth;       // The width of selected pyramid yuv
    UINT32 RoiHeight;      // The height of selected pyramid yuv
    UINT32 RoiStartX;      // The start x of selected pyramid yuv
    UINT32 RoiStartY;      // The start y of selected pyramid yuv
    UINT32 NetworkWidth;   // The input width of CNN
    UINT32 NetworkHeight;  // The input height of CNN
    UINT32 VisWidth;       // The width of normalized output
    UINT32 VisHeight;      // The height of normalized output
    UINT8 OsdEnable;
} RFCN_PROC_PARAM_s;

typedef struct {
    UINT32 Class;
    FLOAT  Score;
    UINT32 Xmin;
    UINT32 Ymin;
    UINT32 Xmax;
    UINT32 Ymax;
} CVALGO_RFCN_BOX_s;

typedef struct {
    UINT32 BoxNum;
    CVALGO_RFCN_BOX_s Box[MAX_CVALGO_RFCN_BOX_NUM + 1U];
} CVALGO_RFCN_OUTPUT_s;

/******************************************************************************
 *  Defined in FrcnnProc.c
 ******************************************************************************/
extern SVC_CV_ALGO_OBJ_s CvAlgoObj_RfcnProc;

#endif /* RFCN_PROC_H */
