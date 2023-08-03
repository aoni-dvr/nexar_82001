/**
*  @file SvcCvFlow_Comm.h
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
*  @details Header for SVC CvFlow Common Structure/Output
*
*/

#ifndef SVC_CV_FLOW_COMMON_H
#define SVC_CV_FLOW_COMMON_H

#include "cvapi_memio_interface.h"
/*-----------------------------------------------------------------------------------------------*\
 *  Definition for Common Message for SvcCvFlow_Issue
\*-----------------------------------------------------------------------------------------------*/
#define SVC_CV_OSD_BUFFER                       (0x1000U)
#define SVC_CV_DETRES_BBX                       (0x1001U)
#define SVC_CV_DETRES_SEG                       (0x1002U)
#define SVC_CV_STEREO_OUTPUT                    (0x1003U)
#define SVC_CV_STEREO_AUTO_CALIB                (0x1004U)
#define SVC_CV_PERCEPTION_OUTPUT                (0x1005U)
#define SVC_CV_RAW_OUTPUT                       (0x1006U)
#define SVC_CV_FEX_OUTPUT                       (0x1007U)
#define SVC_CV_STIXEL_OUTPUT                    (0x1008U)

typedef struct {
    void  *pBuf;
    UINT32 BufSize;
} SVC_CV_DATA_BUF_s;

#define SVC_CV_MAX_DATA_BUF  (8U)
typedef struct {
    UINT32 MsgCode;
    UINT32 Source;
    UINT8  NumData;
    SVC_CV_DATA_BUF_s  Data[SVC_CV_MAX_DATA_BUF];
} SVC_CV_DATA_OUTPUT_s;

/* SVC_CV_OSD_BUFFER */
typedef struct {
    UINT32 MsgCode;
    UINT32 Source;
    ULONG  Addr;
} SVC_CV_OUT_BUF_s;

typedef struct {
    UINT16 Cat;
    UINT16 Fid;
    UINT32 ObjectId;
    UINT32 Score;
    UINT16 X;
    UINT16 Y;
    UINT16 W;
    UINT16 H;
} SVC_CV_DETRES_BBX_WIN_s;

/* SVC_CV_DETRES_BBX */
#define SVC_CV_DETRES_MAX_BBX_NUM   (150U)
typedef struct {
    UINT32 MsgCode;
    UINT32 Source;
    UINT32 CaptureTime;
    UINT32 FrameNum;
    UINT32 BbxAmount;
    SVC_CV_DETRES_BBX_WIN_s Bbx[SVC_CV_DETRES_MAX_BBX_NUM];
    const char ** class_name;
    void   *pCvOut;
} SVC_CV_DETRES_BBX_LIST_s;

/* SVC_CV_DETRES_SEG */
typedef struct {
    UINT32 MsgCode;
    UINT32 Source;
    UINT8* pBuf;     /**< Buffer address */
    UINT32 Pitch;    /**< Buffer pitch which is the width to change to next image row */
    UINT32 Width;    /**< Image width */
    UINT32 Height;   /**< Image height */
    UINT32 FrameNum;
    UINT32 CaptureTime;
    void   *pCvOut;
} SVC_CV_DETRES_SEG_s;

/* SVC_CV_STEREO_OUTPUT */
typedef struct {
    UINT32 MsgCode;
    UINT32 Type;
    UINT32 StereoID;
    UINT32 FrameSetID;
    UINT32 FrameNum;
    UINT32 CaptureTime;
    ULONG  DataAddr;
    memio_source_recv_picinfo_t *pPicInfo;
} SVC_CV_STEREO_OUTPUT_s;

typedef struct {
    UINT32 Enable     :1;
    UINT32 ListUpdate :1;
    UINT32 Reserved   :30;
    UINT8  PcptType;
    UINT8  MaxObj;
    UINT8  NumObj;
    UINT8  *pObjIdxList;
    void   *pCvOut;
    const char ** class_name;
} SVC_CV_PERCEPTION_DATA_s;

/* SVC_CV_PERCEPTION_OUTPUT */
#define SVC_CV_MAX_PCPT_PROC        (10U)
#define SVC_CV_MAX_PCPT_OBJ_INDEX   (64U)   /* Max number of index per list */
typedef struct {
    UINT32 MsgCode;
    SVC_CV_DETRES_BBX_LIST_s  *pBbxList;
    UINT32                    PcptBits;
    SVC_CV_PERCEPTION_DATA_s  PcptData[SVC_CV_MAX_PCPT_PROC];
    UINT32                    NumPcpt;
} SVC_CV_PERCEPTION_OUTPUT_s;

/*-----------------------------------------------------------------------------------------------*\
 *  Definition for Common Status Event for SvcCvFlow_StatusEventIssue
\*-----------------------------------------------------------------------------------------------*/
#define SVC_CV_EVT_IPC_CHAN_READY               (0x1000U)

/*-----------------------------------------------------------------------------------------------*\
 *  Definition for Common Ctrl Type for SvcCvFlow_Control
\*-----------------------------------------------------------------------------------------------*/
#define SVC_CV_CTRL_ENABLE                      (0x1000U)
#define SVC_CV_CTRL_SET_SRC_RATE                (0x1001U)
#define SVC_CV_CTRL_SEND_EXT_YUV                (0x1002U)
#define SVC_CV_CTRL_SET_ROI_INFO                (0x1003U)
#define SVC_CV_CTRL_GET_ROI_INFO                (0x1004U)
#define SVC_CV_CTRL_GET_MASK_INFO               (0x1005U)
#define SVC_CV_CTRL_ENABLE_FRAME_SYNC           (0x1006U)
#define SVC_CV_CTRL_QUERY_FRAME_QUEUE_INFO      (0x1007U)
#define SVC_CV_CTRL_SEND_FRAME_SYNC             (0x1008U)
#define SVC_CV_CTRL_SET_SCECNE_MODE             (0x1009U)
#define SVC_CV_CTRL_ENABLE_TUNING_MODE          (0x100AU)
#define SVC_CV_CTRL_SET_SRC_STRMID              (0x100BU)
#define SVC_CV_CTRL_SET_META_DATA               (0x100CU)
#define SVC_CV_CTRL_GET_OUTPUT_FRAME_TIME       (0x100DU)

/* SVC_CV_CTRL_SET_SCECNE_MODE */
#define SVC_CV_SCENE_MODE_DAY                   (0x0)
#define SVC_CV_SCENE_MODE_NIGHT                 (0x1)

/* SVC_CV_CTRL_SET_META_DATA */
typedef struct {
    UINT8  Type;
    void   *pAddr;
    UINT32 Size;
    UINT32 BufSize;
} SVC_CV_META_DATA_s;

typedef struct {
    UINT8  NumData;
    SVC_CV_META_DATA_s *pList;
} SVC_CV_META_DATA_INFO_s;

/* SVC_CV_CTRL_GET_OUTPUT_FRAME_TIME */
typedef struct {
    UINT32  ProcTimeFD;
    UINT32  ProcTimeAll;
    UINT32  FrameTime;
} SVC_CV_FRAME_TIME_INFO_s;

#endif  /* SVC_CV_FLOW_COMMON_H */
