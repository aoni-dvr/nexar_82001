
/**
 * @file AmbaSR_3DPosExt.h
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
 *  @details AmbaSR_3DPosExt.h
 *
 */
#ifndef AMBASR_3D_POS_EXT_H
#define AMBASR_3D_POS_EXT_H

#include "AmbaTypes.h"
#include "AmbaCalib_EmirrorDef.h"
#include "AmbaSR_BbxConvDef.h"
UINT32 AmbaSR_CarEstMethod(const AMBA_CAL_EM_CALC_COORD_CFG_V1_s *pCfg, const AMBA_SR_OBJ_INFO_s *pCarObjInfo);
UINT32 AmbaSR_CarEst2DTo3D(const AMBA_CAL_EM_CALC_COORD_CFG_V1_s *pCfg, const AMBA_SR_OBJ_INFO_s *pCarObjInfo,
                           AMBA_SR_WORLD_3D_OBJECT_s *p3DObject, AMBA_SR_IMAGE_3D_MODEL_s *p3DImgObj, UINT32 UsePrevData, UINT32 *pInvalidInput);

UINT32 SR_DirCubeToCF(const AMBA_SR_WORLD_3D_MODEL_s *p3DWorldObject, const AMBA_CAL_EM_CALC_COORD_CFG_V1_s *pCfg, const AMBA_SR_OBJ_INFO_s *pCarObjInfo);

#endif

