/**
 * @file AmbaSR_ObjBD.h
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
 *  @details AmbaSR_ObjBD.h
 *
 */
#ifndef AMBASR_OBJDB_H
#define AMBASR_OBJDB_H
#include "AmbaTypes.h"
#include "AmbaSR_ObjPriority.h"

#define AMBA_SR_MAX_PITCH 1280U
#define AMBA_SR_HOR_MODE 0U
#define AMBA_SR_VER_MODE 1U

typedef struct {
    UINT16 ObjStartX;
    UINT16 ObjStartY;
    UINT16 ObjPitch;
    UINT16 ObjWidth;
    UINT16 ObjHeight;
    UINT8 *pObjTable; // Full table of Obj
    AMBA_CAL_POINT_DB_2D_s Out2D[AMBA_SR_MAX_PITCH];  // the target point of wheel in table
} AMBA_SR_OBJ_SEG_INFO;


typedef struct {
    UINT32 ObjNum;
    UINT32 ValidID[AMBA_SR_MAX_OBJECT_NUM];
} AMBA_SR_OP_VALID_2D_3D_DATA_s;

typedef struct {
    UINT32 ObjNum;
    UINT32 ValidID[AMBA_SR_MAX_OBJECT_NUM];
} AMBA_SR_OP_VALID_SEG_DATA_s;

typedef struct {
    AMBA_SR_OP_VALID_SEG_DATA_s SegData;
    AMBA_SR_OP_VALID_2D_3D_DATA_s TwoD3DData;
} AMBA_SR_OP_VALID_DATA_s;

typedef struct {
    UINT32 ProcessMode;
    UINT32 UsePreData;
} AMBA_SR_CTRL_PARAMS_s;

typedef struct {
    UINT32 SegObjNum;
    UINT32 SegVehicle0Num;
    UINT32 SegScooter0Num;
    UINT32 SegPerson0Num;
    UINT32 ThreeDObjNum;
    UINT32 ThreeDVehicle0Num;
    UINT32 ThreeDScooter0Num;
    UINT32 ThreeDPerson0Num;
} AMBA_SR_CFG_PROCESS_OBJ_NUM;


typedef struct {
    AMBA_CAL_POINT_DB_3D_s Coordinate;
} AMBA_CAL_POINT_WORLD_OUT_s;

UINT32 AmbaSR_GetImproveTargets(const AMBA_SR_CFG_PROCESS_OBJ_NUM *pCfgObjNum, const AMBA_SR_OP_PROC_OUT_s *pProcessData,
                                AMBA_SR_OP_VALID_DATA_s *pValidID, UINT32 Mode);
UINT32 AmbaSR_CarEstSeg(const AMBA_SR_OBJ_INFO_s *pCarObjInfo, AMBA_SR_OBJ_SEG_INFO *pObjNNInfo,
                        const AMBA_CAL_EM_CALC_COORD_CFG_V1_s *pCfg, AMBA_SR_WORLD_3D_OBJECT_s *p3DObject);
UINT32 AmbaSR_Est3DObj(const void *pHandler, const AMBA_CAL_EM_CALC_COORD_CFG_V1_s *pCfg, const AMBA_SR_NN_3D_OBJ_INFO_s *pObjInfo,
                       AMBA_SR_WORLD_3D_OBJECT_s *p3DObject, AMBA_SR_IMAGE_3D_MODEL_s *p3DImgObj, const AMBA_SR_CTRL_PARAMS_s *pCtrlParams);
UINT32 AmbaSR_ChkObjInHistory(const AMBA_SR_OBJ_INFO_s *pCarObjInfo);

#endif



