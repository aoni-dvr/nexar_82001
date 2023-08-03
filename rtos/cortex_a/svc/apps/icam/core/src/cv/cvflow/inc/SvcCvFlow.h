/**
 *  @file SvcCvFlow.h
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
 *  @details Header of SvcCvFlow
 *
 */

#ifndef SVC_CV_FLOW_H
#define SVC_CV_FLOW_H

#define SVC_CV_FLOW_CHAN_MAX            (16U)       /* Software constraint */

#define SVC_CV_FLOW_INPUT_MAX           (3U)        /* Software constraint */
#define SVC_CV_FLOW_OUTPUT_PORT_MAX     (2U)        /* Software constraint. Output port of dirver. It doesn't need to match Flexidag. */
#define SVC_CV_FLOW_CFG_PARAM_MAX       (4U)        /* Software constraint. Other config if any */
#define SVC_CV_FLOW_OUTPUT_PORT0        (0U)
#define SVC_CV_FLOW_OUTPUT_PORT1        (1U)

#define SVC_CV_FLOW_EVENT_MAX           (4U)        /* Software constraint */

typedef UINT32 (*SVC_CV_FLOW_CALLBACK_f)(UINT32 CvFlowChan, void *pInput);
typedef UINT32 (*SVC_CV_FLOW_CONTROL_CALLBACK_f)(UINT32 CvFlowChan, UINT32 CtrlType, const void *pInput);
typedef UINT32 (*SVC_CV_FLOW_DEBUG_CALLBACK_f)(UINT32 CvFlowChan, const char *pCmd, UINT32 Param0, UINT32 Param1, UINT32 Param2);
typedef void   (*SVC_CV_FLOW_EVENT_f)(UINT32 CvFlowChan, UINT32 OutputTag, void *pOutput);
typedef void   (*SVC_CV_FLOW_STATUS_EVENT_f)(UINT32 CvFlowChan, UINT32 Event, void *pInfo);

typedef struct /* _SVC_CV_FLOW_CALLBACK_INFO_s_ */ {
    SVC_CV_FLOW_CALLBACK_f Init;
    SVC_CV_FLOW_CALLBACK_f Config;
    SVC_CV_FLOW_CALLBACK_f Load;
    SVC_CV_FLOW_CALLBACK_f Start;
    SVC_CV_FLOW_CONTROL_CALLBACK_f Control;
    SVC_CV_FLOW_CALLBACK_f Stop;
    SVC_CV_FLOW_DEBUG_CALLBACK_f Debug;
} SVC_CV_FLOW_OBJ_s;

typedef struct {
    UINT16 Index;                               /* Example: Pyramid scale index */
    UINT32 StartX;                              /* User configurable */
    UINT32 StartY;                              /* User configurable */
    UINT32 Width;                               /* Fixed value for most of implementation */
    UINT32 Height;                              /* Fixed value for most of implementation */
} SVC_CV_ROI_s;

typedef struct {
    UINT32  NumCat;
    UINT32  Width;
    UINT32  Height;
} SVC_CV_MASK_s;

#define SVC_CV_MAX_ROI                          (3U)    /* Software constraint */
typedef struct {
    UINT16 NumRoi;                              /* Depend on driver and Flexidag */
    SVC_CV_ROI_s Roi[SVC_CV_MAX_ROI];
} SVC_CV_ROI_INFO_s;

typedef struct {
    UINT32  NumRoi;
    SVC_CV_MASK_s Roi[SVC_CV_MAX_ROI];
} SVC_CV_MASK_INFO_s;

#define SVC_CV_DATA_SRC_PYRAMID                 (0U)
#define SVC_CV_DATA_SRC_MAIN_YUV                (1U)
#define SVC_CV_DATA_SRC_YUV_STRM                (2U)
#define SVC_CV_DATA_SRC_EXT_DATA                (3U)
#define SVC_CV_DATA_SRC_RAW_DATA                (4U)
#define SVC_CV_DATA_SRC_PROC_RAW                (5U)
#define SVC_CV_DATA_SRC_MAIN_Y12                (6U)
#define SVC_CV_DATA_SRC_PYRAMID_EX_SCALE        (7U)
typedef struct {
    UINT16 DataSrc;                             /* Main / pyramid */
    UINT32 StrmId;
    UINT16 FrameWidth;                          /* Depend on driver or Flexidag */
    UINT16 FrameHeight;                         /* Depend on driver or Flexidag */
    SVC_CV_ROI_INFO_s RoiInfo;
} SVC_CV_INPUT_INFO_s;

typedef struct {
    UINT16 InputNum;
    SVC_CV_INPUT_INFO_s Input[SVC_CV_FLOW_INPUT_MAX];
    UINT32 SrcRate;

    UINT8  EnableFeeder;                        /* Enable ImgFeeder utility. Depend on driver. */
    UINT32 FeederID;
    UINT32 FeederFlag;
} SVC_CV_INPUT_CFG_s;

typedef struct {
    UINT32 OutputTag[SVC_CV_FLOW_OUTPUT_PORT_MAX];  /* User Tags that will be passed to output callback */
} SVC_CV_OUTPUT_CFG_s;

/* SvcCvFlow_Config */
typedef struct {
    UINT32 CvModeID;
    SVC_CV_INPUT_CFG_s InputCfg;
    SVC_CV_OUTPUT_CFG_s OutputCfg;
    UINT32 PrivFlag;
    UINT32 Config[SVC_CV_FLOW_CFG_PARAM_MAX];
} SVC_CV_FLOW_CFG_s;

#define SVC_CV_FLOW_MAX_BIN_NUM                 (5U)
#define SVC_CV_FLOW_MAX_FILE_PATH               (64U)

#define SVC_CV_FLOW_STORAGE_TYPE_NUM            (2U)
#define SVC_CV_FLOW_STORAGE_TYPE_ROMFS          (0U)
#define SVC_CV_FLOW_STORAGE_TYPE_SD             (1U)

/* SvcCvFlow_Load */
typedef struct {
    UINT32 AlgoIdx;
    UINT32 NumBin;
#define SVC_CV_FLOW_FLEXI_BIN                   (0U)
#define SVC_CV_FLOW_LICENSE_BIN                 (1U)
#define SVC_CV_FLOW_EXT_BIN                     (2U)
    UINT32 BinType[SVC_CV_FLOW_MAX_BIN_NUM];
    char   *pFileName[SVC_CV_FLOW_MAX_BIN_NUM];
    UINT32 StorageType;
} SVC_CV_FLOW_LOAD_INFO_s;

UINT32 SvcCvFlow_Hook(UINT32 CvFlowChan, SVC_CV_FLOW_OBJ_s *pCvObj);
UINT32 SvcCvFlow_UnHook(UINT32 CvFlowChan);

UINT32 SvcCvFlow_Register(UINT32 CvFlowChan, SVC_CV_FLOW_EVENT_f pEventCallback, UINT32 *pRegisterID);
UINT32 SvcCvFlow_RegisterEx(UINT32 CvFlowChan, UINT32 OutputPort, SVC_CV_FLOW_EVENT_f pEventCallback, UINT32 *pRegisterID);
UINT32 SvcCvFlow_Unregister(UINT32 CvFlowChan, UINT32 RegisterID);

UINT32 SvcCvFlow_Init(UINT32 CvFlowChan);
UINT32 SvcCvFlow_Config(UINT32 CvFlowChan, void *pConfig);
UINT32 SvcCvFlow_Load(UINT32 CvFlowChan, void *pInfo);
UINT32 SvcCvFlow_Control(UINT32 CvFlowChan, UINT32 CtrlType, const void *pCtrl);
UINT32 SvcCvFlow_Start(UINT32 CvFlowChan, void *pInput);
UINT32 SvcCvFlow_Stop(UINT32 CvFlowChan, void *pInput);
UINT32 SvcCvFlow_Debug(UINT32 CvFlowChan, const char *pCmd, UINT32 Param0, UINT32 Param1, UINT32 Param2);

UINT32 SvcCvFlow_Issue(UINT32 CvFlowChan, UINT32 OutputPort, UINT32 OutputTag, void *pOutput);

UINT32 SvcCvFlow_StatusEventRegister(UINT32 CvFlowChan, SVC_CV_FLOW_STATUS_EVENT_f pEventCallback, UINT32 *pRegisterID);
UINT32 SvcCvFlow_StatusEventUnregister(UINT32 CvFlowChan, UINT32 RegisterID);
UINT32 SvcCvFlow_StatusEventIssue(UINT32 CvFlowChan, UINT32 Event, void *pInfo);

#endif /* SVC_CV_FLOW_H */
