/**
 *  @file AmbaCalib_DetectionDef.h
 *
 * Copyright (c) 2020 Ambarella International LP
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
 */
#ifndef AMBA_CALIB_DETECTION_DEF_H
#define AMBA_CALIB_DETECTION_DEF_H

#define CAL_DET_CB_MAX_CORNER_NUM (7260U)
#define CAL_DET_CB_IMG_MAX_SIZE (3840U * 2176U)
#define CAL_DET_CB_MAX_CANDIDATE_CORNER_NUM (CAL_DET_CB_IMG_MAX_SIZE / 9U)
#define CAL_DET_MAX_ELLIPSE_SAMPLE (256U * 1024U)

#define AMBA_CAL_DET_PTN_TYPE_BLACK (0U)
#define AMBA_CAL_DET_PTN_TYPE_RED (1U)
#define AMBA_CAL_DET_PTN_TYPE_MAX (2U)

typedef struct {
    UINT16 Size;
    UINT16 Type;
} AMBA_CAL_DET_CORNER_TYPE_s;

typedef struct {
    AMBA_CAL_POINT_DB_2D_s *pCornerPosList;
    AMBA_CAL_DET_CORNER_TYPE_s *pCornersTypeList;
    UINT32 CornerNum;
} AMBA_CAL_DET_CORNER_LIST_s;
#endif
