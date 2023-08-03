/**
 * @file AmbaSR_NNPose3DDef.h
 *
 * Copyright (c) 2018 Ambarella International LP
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
 *
 *  @details AmbaSR_NNPose3DDef
 *
 */
#ifndef AMBASR_NNPOSE3DDEF_H
#define AMBASR_NNPOSE3DDEF_H
#include "AmbaTypes.h"
#include "AmbaOD_3DBbx.h"

typedef struct {
    AMBA_CAL_POINT_DB_2D_s FocalLength;             /* unit: pixel */
    AMBA_CAL_POINT_DB_2D_s OpticalCenter;
    AMBA_CAL_EM_CALC_COORD_CFG_V1_s CalibCfg;
    AMBA_CAL_EM_CALIB_INFO_DATA_V1_s CalibInfoData; /* Filed CameraEulerAngle should convert unit to rad */
} AMBA_SR_NN_POSE_3D_INIT_s;

typedef struct {
    AMBA_SR_OBJ_INFO_s Obj2DInfo;
    AMBA_OD_3DBBX_s ObjNN3DInfo;
} AMBA_SR_NN_3D_OBJ_INFO_s;
#endif
