/**
 * @file AmbaSR_BbxConvDef.h
 *
 * Copyright (c) 2019 Ambarella International LP
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
 *  @details AmbaSR_Bbx_Conv_Def
 *
 */
#ifndef AMBA_SR_BBS_CONV_DEF_H
#define AMBA_SR_BBS_CONV_DEF_H
#include "AmbaTypes.h"
#include "AmbaCalib_Def.h"
#include "AmbaOD_2DBbx.h"

// #define SR_PRINTF(f_, ...)
#define AMBA_MAX_IDENT_CATEGORY_NUM (128U)

#define AMBA_BBX2WD_MAX_HORIZON_LEN (1920)

#define AMBA_3D_MODEL_CUBOID (0U) ///< 3D model type: cuboid
#define AMBA_3D_MODEL_LINE (1U)   ///< 3D model type: line
#define AMBA_3D_MODEL_POLE (2U)   ///< 3D model type: pole
#define AMBA_3D_MODEL_WALL (3U)   ///< 3D model type: wall
#define AMBA_3D_MODEL_DIR_CUBOID (4U)
#define AMBA_3D_MODEL_MAX_VETICES_NUM (8U) ///< maxmum vetices number

#define AMBA_3D_MD_CUBOID_VERTICE_CBL (0U) ///< close bottom left vertices of the cuboid
#define AMBA_3D_MD_CUBOID_VERTICE_CBR (1U) ///< close bottom right vertices of the cuboid
#define AMBA_3D_MD_CUBOID_VERTICE_FBL (2U) ///< far bottom left vertices of the cuboid
#define AMBA_3D_MD_CUBOID_VERTICE_FBR (3U) ///< far bottom right vertices of the cuboid
#define AMBA_3D_MD_CUBOID_VERTICE_CTL (4U) ///< close top left vertices of the cuboid
#define AMBA_3D_MD_CUBOID_VERTICE_CTR (5U) ///< close top right vertices of the cuboid
#define AMBA_3D_MD_CUBOID_VERTICE_FTL (6U) ///< far top left vertices of the cuboid
#define AMBA_3D_MD_CUBOID_VERTICE_FTR (7U) ///< far top right vertices of the cuboid

#define AMBA_3D_MD_DIRCUBE_VERTICE_FBL (0U) ///< front bottom left vertices of the cuboid
#define AMBA_3D_MD_DIRCUBE_VERTICE_FBR (1U) ///< front bottom right vertices of the cuboid
#define AMBA_3D_MD_DIRCUBE_VERTICE_RBR (2U) ///< rear bottom left vertices of the cuboid
#define AMBA_3D_MD_DIRCUBE_VERTICE_RBL (3U) ///< rear bottom right vertices of the cuboid
#define AMBA_3D_MD_DIRCUBE_VERTICE_FTL (4U) ///< front top left vertices of the cuboid
#define AMBA_3D_MD_DIRCUBE_VERTICE_FTR (5U) ///< front top right vertices of the cuboid
#define AMBA_3D_MD_DIRCUBE_VERTICE_RTR (6U) ///< rear top left vertices of the cuboid
#define AMBA_3D_MD_DIRCUBE_VERTICE_RTL (7U) ///< rear top right vertices of the cuboid

#define AMBA_3D_MD_POLE_VERTICE_B (0U) ///< bottom vertices of the pole
#define AMBA_3D_MD_POLE_VERTICE_T (1U) ///< top vertices of the pole

#define AMBA_3D_MD_WALL_VERTICE_CB (0U)
#define AMBA_3D_MD_WALL_VERTICE_CT (1U)
#define AMBA_3D_MD_WALL_VERTICE_FB (2U)
#define AMBA_3D_MD_WALL_VERTICE_FT (3U)

typedef struct {
    UINT32 StartX; /**< Start Position X */
    UINT32 StartY; /**< Start Position Y */
    UINT32 Width;  /**< Width*/
    UINT32 Height; /**< Height*/
} AMBA_SR_ROI_s;

typedef struct {
    AMBA_OD_2DBBX_s BbxInfo;
    AMBA_SR_ROI_s BbxDetROI;
} AMBA_SR_PASSTHROUGH_EST_s;

typedef struct {
    AMBA_OD_2DBBX_s BbxInfo;
    AMBA_SR_ROI_s BbxDetROI;
    UINT32 HorizonPos[AMBA_BBX2WD_MAX_HORIZON_LEN];
    DOUBLE T0;/**< 0 ~ 1 */
    DOUBLE T1;/**< 0 ~ 1 */
} AMBA_SR_COMBINE_PROJ_FOCAL_LEN_s;

typedef struct {
    AMBA_OD_2DBBX_s BbxInfo;
    AMBA_SR_ROI_s BbxDetROI;
    DOUBLE Angle;
} AMBA_SR_OBJ_INFO_s;

typedef struct {
    UINT32 IsOverMarginInfoValid; /**< Status of IsOverMargin. 1: Valid, 0: Invalid */
    struct {
        UINT32 Top; /**< Status of top side. 1: over margin*/
        UINT32 Bottom; /**< Status of bottom side. 1: over margin*/
        UINT32 Left;  /**< Status of left side. 1: over margin*/
        UINT32 Right;  /**< Status of right side. 1: over margin*/
    } IsOverMargin;
    UINT32 IsBbx2dProjectionInfoValid;
    struct {
        AMBA_CAL_POINT_DB_2D_s LeftProjectionPos;
        AMBA_CAL_POINT_DB_2D_s RightProjectionPos;
    } Bbx2dProjectionInfo;
} AMBA_SR_OBJ_STATUS_s;

typedef struct {
    UINT32 Length; /**< Object Length, unit:mm*/
    UINT32 Width; /**< Object Width, unit:mm*/
    UINT32 Height; /**< Object Height, unit:mm*/
} AMBA_SR_OBJECT_SIZE_s;

typedef struct {
    DOUBLE Length;
    DOUBLE Width;
    DOUBLE Height;
} AMBA_SR_OBJECT_SIZE_DB_s;

typedef struct {
    UINT32 ModelType; /**< 3D Model Type, please refer to AMBA_3D_MODEL_ */
    UINT32 VerticeStatus; /**< VerticeStatus, one bit means the status of one vertice. 1 is valided */
    AMBA_CAL_POINT_DB_3D_s Vertices[AMBA_3D_MODEL_MAX_VETICES_NUM]; /**< Vertices position of the 3d model*/
} AMBA_SR_WORLD_3D_MODEL_s;

typedef struct {
    UINT32 ModelType; /**< 3D Model Type, please refer to AMBA_3D_MODEL_ */
    UINT32 VerticeStatus; /**< VerticeStatus, one bit means the status of one vertice. 1 is valided */
    AMBA_CAL_POINT_DB_2D_s Vertices[AMBA_3D_MODEL_MAX_VETICES_NUM]; /**< Vertices position of the 3d model*/
} AMBA_SR_IMAGE_3D_MODEL_s;

typedef struct {
    UINT32 ObjId; /**< Object id. only valid if tracker applied */
    UINT32 ObjCat; /**< Object category */
    AMBA_SR_WORLD_3D_MODEL_s Model; /**< 3D Model, please refer to AMBA_SR_WORLD_3D_MODEL_s*/
    AMBA_SR_OBJ_STATUS_s Status; /**< Object Status*/
} AMBA_SR_WORLD_3D_OBJECT_s;

#endif
