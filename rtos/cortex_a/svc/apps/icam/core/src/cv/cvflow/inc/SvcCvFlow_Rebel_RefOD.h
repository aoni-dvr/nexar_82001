/**
 *  @file SvcCvFlow_Rebel_RefOD.h
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
 *  @details Header of FlexiDAG based Reference Object Detection (Rebel case)
 *
 */

#ifndef SVC_CV_FLOW_REBEL_REF_OD_H
#define SVC_CV_FLOW_REBEL_REF_OD_H

#define SVC_CV_DETRES_BBX_REBEL                 (0x0007U)

#define REBEL_FDAG_LINUX_OD_RESERVE1        (0U)
#define REBEL_FDAG_LINUX_OD_RESERVE2        (1U)
#define REBEL_FDAG_LINUX_OD_RESERVE3        (2U)
#define REBEL_FDAG_LINUX_OPENOD             (3U)
#define REBEL_NUM_MODE_OD                   (4U)  //Need to sync REFOD_NUM_MODE value


/* Output Type */
#define REBEL_OUTPUT_TYPE_DETBOX            (3U)    /* SVC_CV_DETRES_BBX_LIST_s */


typedef struct {
    UINT32 x;
    UINT32 y;
} SVC_POINT_2D_s;

typedef struct {
    UINT32 score;                 /* confidence score                   */
    UINT32 field;                 /* field ID, assigned by step1        */
    UINT32 class;                 /* class ID, assigned by step4        */
    UINT32 track;                 /* track ID, assigned by tracker      */
    INT32  bb_start_row;
    INT32  bb_start_col;
    UINT32 bb_height_m1;
    UINT32 bb_width_m1;

    UINT8 ObjectID;
    FLOAT Confidence;
    FLOAT PositionX;
    FLOAT PositionY;
    FLOAT VelocityX;
    FLOAT VelocityY;
    UINT8 DirectionStatus;
    UINT8 MotionStatus;
    FLOAT Length;
    FLOAT Width;
    UINT8 TurnSignal;
    UINT8 BrakeLamp;
    UINT8 Classification;
    UINT8 LaneAssignment;
    SVC_POINT_2D_s NearestPoint;
    SVC_POINT_2D_s DepthPointX;
    SVC_POINT_2D_s DepthPointY;
} amba_od_candidate_rebel_t;

typedef struct {
    UINT16 Cat;
    UINT16 Fid;
    UINT32 ObjectId;
    UINT32 Score;
    UINT16 X;
    UINT16 Y;
    UINT16 W;
    UINT16 H;
    UINT8  ObjectID;
    FLOAT  Confidence;
    FLOAT  PositionX;
    FLOAT  PositionY;
    FLOAT  VelocityX;
    FLOAT  VelocityY;
    UINT8  DirectionStatus;
    UINT8  MotionStatus;
    FLOAT  Length;
    FLOAT  Width;
    UINT8  TurnSignal;
    UINT8  BrakeLamp;
    UINT8  Classification;
    UINT8  LaneAssignment;
    SVC_POINT_2D_s NearestPoint;
    SVC_POINT_2D_s DepthPointX;
    SVC_POINT_2D_s DepthPointY;
} SVC_CV_DETRES_BBX_WIN_REBEL_s;

typedef struct {
    UINT32 MsgCode;
    UINT32 Source;
    UINT32 CaptureTime;
    UINT32 FrameNum;
    UINT32 BbxAmount;
    SVC_CV_DETRES_BBX_WIN_REBEL_s Bbx[SVC_CV_DETRES_MAX_BBX_NUM];
    const char ** class_name;
    void   *pCvOut;
} SVC_CV_DETRES_BBX_LIST_REBEL_s;

typedef struct {
    UINT8   FlagValidTransferData;
    UINT32  CapTS;
    DOUBLE  angular_yaw_rate;
    DOUBLE  acceleration_long;
    DOUBLE  acceleration_lat;
    DOUBLE  vehicle_speed;
    DOUBLE  reserved_1;
    UINT16  reserved_2;
    UINT8   CANBusTurnLightStatus;
    UINT8   CANBusGearStatus;
    DOUBLE  TransferSpeed;
    UINT32  WheelDir;
    DOUBLE  TransferWheelAngle;
    DOUBLE  TransferWheelAngleSpeed;
} Rebel_SR_CANBUS_TRANSFER_DATA_s;

#endif /* SVC_CV_FLOW_REBEL_REF_OD_H */
