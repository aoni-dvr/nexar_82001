/*
* Copyright (c) 2017-2018 Ambarella, Inc.
*
* This file and its contents ("Software") are protected by intellectual property rights including,
* without limitation, U.S. and/or foreign copyrights.  This Software is also the confidential and
* proprietary information of Ambarella, Inc. and its licensors.  You may not use, reproduce, disclose,
* distribute, modify, or otherwise prepare derivative works of this Software or any portion thereof
* except pursuant to a signed license agreement or nondisclosure agreement with Ambarella, Inc. or
* its authorized affiliates.  In the absence of such an agreement, you agree to promptly notify and
* return this Software to Ambarella, Inc.
*
* THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
* TO, THE IMPLIED WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE
* ARE DISCLAIMED. IN NO EVENT SHALL AMBARELLA, INC. OR ITS AFFILIATES BE LIABLE FOR ANY DIRECT, INDIRECT,
* INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
* OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; COMPUTER FAILURE OR MALFUNCTION; OR
* BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
* LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
* SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
#ifndef CVAPI_CAMCTRL_INTERFACE_H_FILE
#define CVAPI_CAMCTRL_INTERFACE_H_FILE

#define CAMCTRL_TASKNAME      "ARM_CAMCTRL"

#define TAG_CAMCTRL (0xF0000000U)

#define CAMCTRL_MSGTYPE_SYSTEM      (0x00000000U)
#define CAMCTRL_MSGTYPE_IMAGE       (0x00010000U)
#define CAMCTRL_MSGTYPE_ENC         (0x00020000U)
#define CAMCTRL_MSGTYPE_ROI         (0x00030000U)
#define CAMCTRL_MSGTYPE_WARP        (0x00040000U)
#define CAMCTRL_MSGTYPE_SENSOR      (0x00050000U)
#define CAMCTRL_MSGTYPE_DISP        (0x00060000U)
#define CAMCTRL_MSGTYPE_PLATFORM    (0x00070000U)


//System Control Message
#define CAMCTRL_SYS_SET_IDSP_INIT_CONFIG            (TAG_CAMCTRL | CAMCTRL_MSGTYPE_SYSTEM | 0x0001U)
#define CAMCTRL_SYS_SET_LIVEVIEW_CONFIG             (TAG_CAMCTRL | CAMCTRL_MSGTYPE_SYSTEM | 0x0002U)
#define CAMCTRL_SYS_ENABLE_LIVEVIEW                 (TAG_CAMCTRL | CAMCTRL_MSGTYPE_SYSTEM | 0x0003U)
#define CAMCTRL_SYS_ENABLE_HDR                      (TAG_CAMCTRL | CAMCTRL_MSGTYPE_SYSTEM | 0x0004U)
#define CAMCTRL_SYS_FEED_NEXT_RAW                   (TAG_CAMCTRL | CAMCTRL_MSGTYPE_SYSTEM | 0x0005U)
#define CAMCTRL_SYS_STOP_RAW_FEED                   (TAG_CAMCTRL | CAMCTRL_MSGTYPE_SYSTEM | 0x0006U)
#define CAMCTRL_SYS_SET_FRAME_INIT_CONFIG           (TAG_CAMCTRL | CAMCTRL_MSGTYPE_SYSTEM | 0x0007U)
#define CAMCTRL_SYS_SET_DELAY_VSYNC                 (TAG_CAMCTRL | CAMCTRL_MSGTYPE_SYSTEM | 0x0008U)
#define CAMCTRL_SYS_START_VIN_CAPTURE               (TAG_CAMCTRL | CAMCTRL_MSGTYPE_SYSTEM | 0x0009U)
#define CAMCTRL_SYS_SET_DSPWORKBUF                  (TAG_CAMCTRL | CAMCTRL_MSGTYPE_SYSTEM | 0x000aU)
#define CAMCTRL_SYS_SET_DISPOUTPUT                  (TAG_CAMCTRL | CAMCTRL_MSGTYPE_SYSTEM | 0x000bU)
#define CAMCTRL_SYS_SET_INPUTFILEPATH               (TAG_CAMCTRL | CAMCTRL_MSGTYPE_SYSTEM | 0x000cU)
#define CAMCTRL_SYS_SET_INPUTSTARTID                (TAG_CAMCTRL | CAMCTRL_MSGTYPE_SYSTEM | 0x000dU)
#define CAMCTRL_SYS_SET_PYRAMID_INIT_CONFIG         (TAG_CAMCTRL | CAMCTRL_MSGTYPE_SYSTEM | 0x000eU)
#define CAMCTRL_SYS_SET_LANEDETECTION_INIT_CONFIG   (TAG_CAMCTRL | CAMCTRL_MSGTYPE_SYSTEM | 0x000fU)
#define CAMCTRL_SYS_SET_PYRAMID_BUFFER_INFO         (TAG_CAMCTRL | CAMCTRL_MSGTYPE_SYSTEM | 0x0010U)
#define CAMCTRL_SYS_SET_LANEDETECTION_BUFFER_INFO   (TAG_CAMCTRL | CAMCTRL_MSGTYPE_SYSTEM | 0x0011U)
#define CAMCTRL_SYS_SET_CAMCTRL_TASK_AMOUNT         (TAG_CAMCTRL | CAMCTRL_MSGTYPE_SYSTEM | 0x0012U)
#define CAMCTRL_SYS_SET_MIPICSI_MODE                (TAG_CAMCTRL | CAMCTRL_MSGTYPE_SYSTEM | 0x0013U)
#define CAMCTRL_SYS_SET_INPUTOFFSET                 (TAG_CAMCTRL | CAMCTRL_MSGTYPE_SYSTEM | 0x0014U)
#define CAMCTRL_SYS_SET_REC_MODE                    (TAG_CAMCTRL | CAMCTRL_MSGTYPE_SYSTEM | 0x0015U)

//Image Control Message
#define CAMCTRL_IMG_SET_EVBIAS                      (TAG_CAMCTRL | CAMCTRL_MSGTYPE_IMAGE | 0x0001U)
#define CAMCTRL_IMG_SET_DECOLOR                     (TAG_CAMCTRL | CAMCTRL_MSGTYPE_IMAGE | 0x0002U)
#define CAMCTRL_IMG_SET_SCENE                       (TAG_CAMCTRL | CAMCTRL_MSGTYPE_IMAGE | 0x0003U)
#define CAMCTRL_IMG_SET_SLOWSHUTTER                 (TAG_CAMCTRL | CAMCTRL_MSGTYPE_IMAGE | 0x0004U)
#define CAMCTRL_IMG_SET_SHARPNESS                   (TAG_CAMCTRL | CAMCTRL_MSGTYPE_IMAGE | 0x0005U)
#define CAMCTRL_IMG_SET_COLORTEMP                   (TAG_CAMCTRL | CAMCTRL_MSGTYPE_IMAGE | 0x0006U)

//Encoder control Message
#define CAMCTRL_ENC_SET_RESOLUTION                  (TAG_CAMCTRL | CAMCTRL_MSGTYPE_ENC | 0x0001U)
#define CAMCTRL_ENC_SET_BITRATE                     (TAG_CAMCTRL | CAMCTRL_MSGTYPE_ENC | 0x0002U)
#define CAMCTRL_ENC_SET_FRAMERATE                   (TAG_CAMCTRL | CAMCTRL_MSGTYPE_ENC | 0x0003U)

//ROI Control Message
#define CAMCTRL_ROI_UPDATE_PYRAMID                  (TAG_CAMCTRL | CAMCTRL_MSGTYPE_ROI | 0x0001U)
#define CAMCTRL_ROI_UPDATE_AEROI                    (TAG_CAMCTRL | CAMCTRL_MSGTYPE_ROI | 0x0002U)
#define CAMCTRL_ROI_SET_ZOOMSTEP                    (TAG_CAMCTRL | CAMCTRL_MSGTYPE_ROI | 0x0003U)
#define CAMCTRL_ROI_SET_PAN                         (TAG_CAMCTRL | CAMCTRL_MSGTYPE_ROI | 0x0004U)
#define CAMCTRL_ROI_SET_WEIGHTING                   (TAG_CAMCTRL | CAMCTRL_MSGTYPE_ROI | 0x0005U)

//WARP Contol Message
#define CAMCTRL_WARP_ENABLE                         (TAG_CAMCTRL | CAMCTRL_MSGTYPE_WARP | 0x0001U)
#define CAMCTRL_WARP_UPDATE                         (TAG_CAMCTRL | CAMCTRL_MSGTYPE_WARP | 0x0002U)

//DISP Control command
#define CAMCTRL_DISP_ENABLE                         (TAG_CAMCTRL | CAMCTRL_MSGTYPE_DISP | 0x0001U)
#define CAMCTRL_DISP_DRAW_PIXEL                     (TAG_CAMCTRL | CAMCTRL_MSGTYPE_DISP | 0x0002U)
#define CAMCTRL_DISP_DRAW_LINE                      (TAG_CAMCTRL | CAMCTRL_MSGTYPE_DISP | 0x0003U)
#define CAMCTRL_DISP_DRAW_RECT                      (TAG_CAMCTRL | CAMCTRL_MSGTYPE_DISP | 0x0004U)
#define CAMCTRL_DISP_FLUSH                          (TAG_CAMCTRL | CAMCTRL_MSGTYPE_DISP | 0x0006U)
#define CAMCTRL_DISP_CLEAR                          (TAG_CAMCTRL | CAMCTRL_MSGTYPE_DISP | 0x0007U)
#define CAMCTRL_DISP_MULTI_CMD                      (TAG_CAMCTRL | CAMCTRL_MSGTYPE_DISP | 0x0008U)
#define CAMCTRL_DISP_FLUSH_CAPSEQ                   (TAG_CAMCTRL | CAMCTRL_MSGTYPE_DISP | 0x0009U)
#define CAMCTRL_DISP_FLUSH_CAPTS                    (TAG_CAMCTRL | CAMCTRL_MSGTYPE_DISP | 0x000AU)

#define CAMCTRL_DISP_SETCHARCONFIG                  (TAG_CAMCTRL | CAMCTRL_MSGTYPE_DISP | 0x0010U)
#define CAMCTRL_DISP_DRAWSTRING                     (TAG_CAMCTRL | CAMCTRL_MSGTYPE_DISP | 0x0011U)

#define CAMCTRL_DISP_SET_EXTOSDCLUT                 (TAG_CAMCTRL | CAMCTRL_MSGTYPE_DISP | 0x0021U)
#define CAMCTRL_DISP_SET_EXTOSDBUFINFO              (TAG_CAMCTRL | CAMCTRL_MSGTYPE_DISP | 0x0022U)
#define CAMCTRL_DISP_UPDATE_EXTBUF                  (TAG_CAMCTRL | CAMCTRL_MSGTYPE_DISP | 0x0023U)
#define CAMCTRL_DISP_RELEASE_EXTOSD                 (TAG_CAMCTRL | CAMCTRL_MSGTYPE_DISP | 0x0024U)
#define CAMCTRL_DISP_UPDATE_EXTBUF_SYNC             (TAG_CAMCTRL | CAMCTRL_MSGTYPE_DISP | 0x0025U)

#define CAMCTRL_DISP_SET_ROIWINDOW                  (TAG_CAMCTRL | CAMCTRL_MSGTYPE_DISP | 0xA000U)
#define CAMCTRL_DISP_SET_WARPINFO                   (TAG_CAMCTRL | CAMCTRL_MSGTYPE_DISP | 0xA001U)
#define CAMCTRL_DISP_DRAW_LDRESULT                  (TAG_CAMCTRL | CAMCTRL_MSGTYPE_DISP | 0xA002U)

//PLATFORM Control command
#define CAMCTRL_PLATFORM_REBOOT                     (TAG_CAMCTRL | CAMCTRL_MSGTYPE_PLATFORM | 0x0001U)

/*************************************/
/* Flags definition */
/*************************************/
#define CAMCTRLFLAG_LIVEVIEW_DUAL_INPUT        (0x00000001U) //Set for dual vin
#define CAMCTRLFLAG_LIVEVIEW_YUV_INPUT         (0x00000002U) //Set for YUV; Clear for RAW
#define CAMCTRLFLAG_LIVEVIEW_TV_OUTPUT         (0x00000004U) //Set for TV; Clear for LCD
#define CAMCTRLFLAG_LIVEVIEW_MIPICSI           (0x00000008U) //Set for MIPICSI; Clear for HDMI
#define CAMCTRLFLAG_LIVEVIEW_ENBLE_3A          (0x00000010U) //Set for enable; Clear for disable
#define CAMCTRLFLAG_LIVEVIEW_NOCHANGE          (0x80000000U) //Set for Keep original setting. (ignore the CtrlFlag value in conf command)

/*************************************/
/* Common Data Strcutures definition */
/*************************************/
#define CV_MAX_PARAM_SIZE (1016U)
typedef struct {
    uint32_t Msg;
    uint32_t ParamSize;
    uint8_t Param[CV_MAX_PARAM_SIZE];
} CV_Request_s;

typedef struct {
    uint16_t  OffsetX; /* Horizontal offset of the window */
    uint16_t  OffsetY; /* Vertical offset of the window */
    uint16_t  Width; /* Number of pixels per line in the window */
    uint16_t  Height; /* Number of lines in the window */
} DSP_WINDOW_s;

typedef struct {
    uint32_t DataFmt; /* YUV Data format: 0->420 or 1->422. */
    uint32_t Pitch; /* YUV data buffer pitch */
    DSP_WINDOW_s  Window; /* Window position and size */
} YUV_IMG_BUF_SETTING_s;

#define AMBA_DSP_MAX_HIER   (13U)
typedef struct {
    uint16_t HierBit; /* Bit-wise to enable which layer output shall be on. */
    uint16_t IsPolySqrt2; /* Use Square2 as 2nd layer rescale factor. */
    uint16_t HierPolyWidth; /* Use when IsPolySqrt2 == FALSE */
    uint16_t HierPolyHeight;
    uint16_t HierTag; /* Tag of this modification */

    // Reserve for 64-bit alignment
    uint16_t Reserved;
    uint32_t Reserved32;

    DSP_WINDOW_s HierCropWindow[AMBA_DSP_MAX_HIER]; /* config layer output croping */
} PYRAMID_CONFIG_s;

typedef struct {
    uint16_t HierSource; /* lane detect source. */
    uint16_t Width; /* final lane detect output width */
    uint16_t Height; /* final lane detect output height */

    // Reserve for 64-bits alignment
    uint16_t Reserve;

    DSP_WINDOW_s CropWindow; /* lane detect active fov */
} LANE_DETECT_CONFIG_s;

#define MAX_NUM_FOV (8U)
typedef struct {
    uint32_t Msg;
    uint32_t ParamSize;
    uint32_t fovid;
    PYRAMID_CONFIG_s pyramid_config;
    LANE_DETECT_CONFIG_s lanedection_config;
    YUV_IMG_BUF_SETTING_s pyramid_frame_setting;
    YUV_IMG_BUF_SETTING_s lanedection_frame_setting;
} Camctrl_Sys_Set_Frame_Init_Config_s;

typedef struct {
    uint32_t Msg;
    uint32_t ParamSize;
    uint32_t fovid;
    PYRAMID_CONFIG_s pyramid_config;
} Camctrl_Sys_Set_Pyramid_Init_Config_s;

typedef struct {
    uint32_t Msg;
    uint32_t ParamSize;
    uint32_t fovid;
    LANE_DETECT_CONFIG_s lanedection_config;
} Camctrl_Sys_Set_Lanedection_Init_Config_s;

typedef struct {
    uint32_t Msg;
    uint32_t ParamSize;
    uint32_t fovid;
    YUV_IMG_BUF_SETTING_s pyramid_frame_setting;
} Camctrl_Sys_Set_Pyramid_BufferInfo_s;

typedef struct {
    uint32_t Msg;
    uint32_t ParamSize;
    uint32_t fovid;
    YUV_IMG_BUF_SETTING_s lanedection_frame_setting;
} Camctrl_Sys_Set_Lanedection_BufferInfo_s;

typedef struct {
    uint32_t fov_id;
    uint32_t frame_list_table_addr;
    uint32_t frame_list_table_size;
    uint32_t frame_buffer_length;
    uint32_t lanedection_frame_list_table_addr;
    uint32_t lanedection_frame_list_table_size;
    uint32_t lanedect_frame_buffer_length;
    uint32_t meta_list_table_addr;
    uint32_t meta_list_table_size;
    uint32_t meta_buffer_length;
} vp_frame_list_descriptor;

typedef struct {
    uint32_t Msg;
    uint32_t ParamSize;
    uint32_t async_cmd_queue_addr;
    uint32_t async_cmd_queue_size;
    uint32_t vp_msg_queue_cb_addr;
    uint32_t num_frame_list_descriptor;
    vp_frame_list_descriptor frame_list_descriptor[MAX_NUM_FOV];
} Camctrl_Sys_Set_Idsp_Init_Config_s;

typedef struct {
    uint32_t Msg;
    uint32_t ParamSize;
    uint32_t InputMode;
    uint32_t ResoultionID;
    uint32_t RawFileAmount;
    uint32_t RawPeriod;
    uint32_t CtrlFlag;
} Camctrl_Sys_Set_Liveview_Config_s;

typedef struct {
    uint32_t Msg;
    uint32_t ParamSize;
    uint32_t Enable;
} Camctrl_Sys_Set_Liveview_Enable_s;

typedef struct {
    uint32_t Msg;
    uint32_t ParamSize;
} Camctrl_Sys_Feed_Next_Raw_s;

typedef struct {
    uint32_t Msg;
    uint32_t ParamSize;
} Camctrl_Sys_Stop_Raw_Feed_s;

typedef struct {
    uint32_t HierIdx;
    uint32_t HierEnable;
    uint32_t OffsetX;
    uint32_t OffsetY;
    uint32_t Width;
    uint32_t Heidth;
} Roi_Pyramids_s;

typedef struct {
    uint32_t Msg;
    uint32_t ParamSize;
    uint32_t ViewZoneID;
    uint32_t HierTag;
    uint32_t NumHier;
    Roi_Pyramids_s Pyramid[1];
} Camctrl_Roi_Update_Pyramid_s;

typedef struct {
    uint32_t Msg;
    uint32_t ParamSize;
    uint32_t Amount;
} Camctrl_Sys_Set_Camctrl_Task_Amount_s;

typedef struct {
    uint32_t FovId;
    uint32_t Token;
    uint32_t X;
    uint32_t Y;
    uint32_t Width;
    uint32_t Height;
} AEROI_SETTING_s;

typedef struct {
    uint32_t Msg;
    uint32_t ParamSize;
    uint32_t NumChanges;
    AEROI_SETTING_s AeroiSetting[MAX_NUM_FOV];
} Camctrl_Roi_Update_Aeroi_s;

typedef struct {
    uint32_t FovId;
    uint32_t Token;
    uint32_t WarpInfoAddr;
} WARP_SETTING_s;

typedef struct {
    uint32_t Msg;
    uint32_t ParamSize;
    uint32_t NumChanges;
    WARP_SETTING_s WarpSetting[MAX_NUM_FOV];
} Camctrl_Warp_Update_Warptable_s;

typedef struct {
    uint32_t Msg;
    uint32_t Channel;
    uint32_t X1;
    uint32_t Y1;
    uint32_t X2;
    uint32_t Y2;
    uint32_t Color;
    uint32_t Thickness;
} OSD_Command_s;

#define MAX_NUM_CMD (30U)
typedef struct {
    uint32_t CommandNum; /* number of command */
    OSD_Command_s Cmd[MAX_NUM_CMD];
} CamCtrl_OSD_CommandSequence_s;

typedef struct {
    uint32_t Msg;
    uint32_t ParamSize;
    uint32_t Channel;
    uint32_t Enable;
} CV_OSD_Enable_s;

typedef struct {
    uint32_t Msg;
    uint32_t ParamSize;
    uint32_t Channel;
    uint32_t X;
    uint32_t Y;
    uint32_t Color;
} CV_OSD_DrawPixel_s;

typedef struct {
    uint32_t Msg;
    uint32_t ParamSize;
    uint32_t Channel;
    uint32_t X1;
    uint32_t Y1;
    uint32_t X2;
    uint32_t Y2;
    uint32_t Color;
    uint32_t Thickness;
} CV_OSD_DrawLine_s;

typedef struct {
    uint32_t Msg;
    uint32_t ParamSize;
    uint32_t Channel;
    uint32_t X1;
    uint32_t Y1;
    uint32_t X2;
    uint32_t Y2;
    uint32_t Color;
    uint32_t Thickness;
} CV_OSD_DrawRect_s;

typedef struct {
    uint32_t Msg;
    uint32_t ParamSize;
    uint32_t Channel;
} CV_OSD_Flush_s;

typedef struct {
    uint32_t Msg;
    uint32_t ParamSize;
    uint32_t Channel;
} CV_OSD_Clear_s;

typedef struct {
    uint32_t Msg;
    uint32_t ParamSize;
    uint32_t Channel;
    uint32_t CapSeq;
} CV_OSD_FlushCapSeq_s;

typedef struct {
    uint32_t Msg;
    uint32_t ParamSize;
    uint32_t Channel;
    uint32_t CapTs;
} CV_OSD_FlushCapTs_s;

typedef struct {
    uint32_t Msg;
    uint32_t ParamSize;
    uint32_t Channel;
    uint32_t AttributeData;
    uint32_t AttributeBits;
    uint32_t FontFaceIdx;
    uint32_t FontPixelWidth;
    uint32_t FontPixelHeight;
} CV_OSD_SetCharConfig_s;

#define CV_OSD_DRAWSTR_MAX_LEN (12U)
typedef struct {
    uint32_t Msg;
    uint32_t ParamSize;
    uint32_t Channel;
    uint32_t X;
    uint32_t Y;
    uint32_t Color;
    char String[CV_OSD_DRAWSTR_MAX_LEN];
} CV_OSD_DrawString_s;

typedef struct {
    uint32_t Msg;
    uint32_t ParamSize;
    uint32_t Channel;
    uint32_t CLUTAddr;
} CV_OSD_SetExtCLUT_s;

typedef struct {
    uint32_t Msg;
    uint32_t ParamSize;
    uint32_t Channel;
    uint32_t PixelFormat;
    uint32_t BufferPitch;
    uint32_t BufferWidth;
    uint32_t BufferHeight;
    uint32_t BufferSize;
    uint32_t WindowOffsetX;
    uint32_t WindowOffsetY;
    uint32_t WindowWidth;
    uint32_t WindowHeight;
    uint32_t Interlace;
} CV_OSD_SetExtBufInfo_s;

typedef struct {
    uint32_t Msg;
    uint32_t ParamSize;
    uint32_t Channel;
    uint32_t OSDBufAddr;
} CV_OSD_UpdateExtBuf_s;

typedef struct {
    uint32_t Msg;
    uint32_t ParamSize;
    uint32_t Channel;
    uint32_t OSDBufAddr;
    uint32_t CapSeq;
    uint32_t CapTs;
} CV_OSD_UpdateExtBuf_Sync_s;

typedef struct {
    uint32_t Msg;
    uint32_t ParamSize;
    uint32_t Channel;
} CV_OSD_ReleaseExtOSD_s;

#endif /* CVAPI_CAMCTRL_INTERFACE_H_FILE */
