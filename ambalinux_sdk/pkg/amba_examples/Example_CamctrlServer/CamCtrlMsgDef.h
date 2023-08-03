/**
 *  @file CamCtrlMsgDef.h
 *
 *  @copyright Copyright (c) 2017 Ambarella, Inc.
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
 *  @details Header of CamCtrl Message definition (For CV TASK)
 *
 */

#ifndef __CAMCTRL_MSG_DEF_H__
#define __CAMCTRL_MSG_DEF_H__

#define TAG_CAMCTRL (0xF0)
#define CAMCTRL_MSG(TYPE, ID) ((TAG_CAMCTRL<<24)|(TYPE<<16)|ID)

enum _CAMCTRL_MSG_TYPE_e_ {
    CAMCTRL_MSGTYPE_SYSTEM = 0,
    CAMCTRL_MSGTYPE_IMAGE,
    CAMCTRL_MSGTYPE_ENC,
    CAMCTRL_MSGTYPE_ROI,
    CAMCTRL_MSGTYPE_WARP,
    CAMCTRL_MSGTYPE_SENSOR,
    CAMCTRL_MSGTYPE_DISP,
    CAMCTRL_MSGTYPE_PLATFORM,

    CAMCTRL_MSGTYPE_NUM
};

//System Control Message
#define CAMCTRL_SYS_SET_IDSP_INIT_CONFIG            CAMCTRL_MSG(CAMCTRL_MSGTYPE_SYSTEM, 0x0001)
#define CAMCTRL_SYS_SET_LIVEVIEW_CONFIG             CAMCTRL_MSG(CAMCTRL_MSGTYPE_SYSTEM, 0x0002)
#define CAMCTRL_SYS_ENABLE_LIVEVIEW                 CAMCTRL_MSG(CAMCTRL_MSGTYPE_SYSTEM, 0x0003)
#define CAMCTRL_SYS_ENABLE_HDR                      CAMCTRL_MSG(CAMCTRL_MSGTYPE_SYSTEM, 0x0004)
#define CAMCTRL_SYS_FEED_NEXT_RAW                   CAMCTRL_MSG(CAMCTRL_MSGTYPE_SYSTEM, 0x0005)
#define CAMCTRL_SYS_STOP_RAW_FEED                   CAMCTRL_MSG(CAMCTRL_MSGTYPE_SYSTEM, 0x0006)
#define CAMCTRL_SYS_SET_FRAME_INIT_CONFIG           CAMCTRL_MSG(CAMCTRL_MSGTYPE_SYSTEM, 0x0007)
#define CAMCTRL_SYS_SET_DELAY_VSYNC                 CAMCTRL_MSG(CAMCTRL_MSGTYPE_SYSTEM, 0x0008)
#define CAMCTRL_SYS_START_VIN_CAPTURE               CAMCTRL_MSG(CAMCTRL_MSGTYPE_SYSTEM, 0x0009)
#define CAMCTRL_SYS_SET_DSPWORKBUF                  CAMCTRL_MSG(CAMCTRL_MSGTYPE_SYSTEM, 0x000a)
#define CAMCTRL_SYS_SET_DISPOUTPUT                  CAMCTRL_MSG(CAMCTRL_MSGTYPE_SYSTEM, 0x000b)
#define CAMCTRL_SYS_SET_INPUTFILEPATH               CAMCTRL_MSG(CAMCTRL_MSGTYPE_SYSTEM, 0x000c)
#define CAMCTRL_SYS_SET_INPUTSTARTID                CAMCTRL_MSG(CAMCTRL_MSGTYPE_SYSTEM, 0x000d)
#define CAMCTRL_SYS_SET_PYRAMID_INIT_CONFIG         CAMCTRL_MSG(CAMCTRL_MSGTYPE_SYSTEM, 0x000e)
#define CAMCTRL_SYS_SET_LANEDETECTION_INIT_CONFIG   CAMCTRL_MSG(CAMCTRL_MSGTYPE_SYSTEM, 0x000f)
#define CAMCTRL_SYS_SET_PYRAMID_BUFFER_INFO         CAMCTRL_MSG(CAMCTRL_MSGTYPE_SYSTEM, 0x0010)
#define CAMCTRL_SYS_SET_LANEDETECTION_BUFFER_INFO   CAMCTRL_MSG(CAMCTRL_MSGTYPE_SYSTEM, 0x0011)
#define CAMCTRL_SYS_SET_CAMCTRL_TASK_AMOUNT         CAMCTRL_MSG(CAMCTRL_MSGTYPE_SYSTEM, 0x0012)
#define CAMCTRL_SYS_SET_MIPICSI_MODE                CAMCTRL_MSG(CAMCTRL_MSGTYPE_SYSTEM, 0x0013)
#define CAMCTRL_SYS_SET_INPUTOFFSET                 CAMCTRL_MSG(CAMCTRL_MSGTYPE_SYSTEM, 0x0014)

//Image Control Message
#define CAMCTRL_IMG_SET_EVBIAS                      CAMCTRL_MSG(CAMCTRL_MSGTYPE_IMAGE, 0x0001)
#define CAMCTRL_IMG_SET_DECOLOR                     CAMCTRL_MSG(CAMCTRL_MSGTYPE_IMAGE, 0x0002)
#define CAMCTRL_IMG_SET_SCENE                       CAMCTRL_MSG(CAMCTRL_MSGTYPE_IMAGE, 0x0003)
#define CAMCTRL_IMG_SET_SLOWSHUTTER                 CAMCTRL_MSG(CAMCTRL_MSGTYPE_IMAGE, 0x0004)
#define CAMCTRL_IMG_SET_SHARPNESS                   CAMCTRL_MSG(CAMCTRL_MSGTYPE_IMAGE, 0x0005)
#define CAMCTRL_IMG_SET_COLORTEMP                   CAMCTRL_MSG(CAMCTRL_MSGTYPE_IMAGE, 0x0006)

//Encoder control Message
#define CAMCTRL_ENC_SET_RESOLUTION                  CAMCTRL_MSG(CAMCTRL_MSGTYPE_ENC, 0x0001)
#define CAMCTRL_ENC_SET_BITRATE                     CAMCTRL_MSG(CAMCTRL_MSGTYPE_ENC, 0x0002)
#define CAMCTRL_ENC_SET_FRAMERATE                   CAMCTRL_MSG(CAMCTRL_MSGTYPE_ENC, 0x0003)

//ROI Control Message
#define CAMCTRL_ROI_UPDATE_PYRAMID                  CAMCTRL_MSG(CAMCTRL_MSGTYPE_ROI, 0x0001)
#define CAMCTRL_ROI_UPDATE_AEROI                    CAMCTRL_MSG(CAMCTRL_MSGTYPE_ROI, 0x0002)
#define CAMCTRL_ROI_SET_ZOOMSTEP                    CAMCTRL_MSG(CAMCTRL_MSGTYPE_ROI, 0x0003)
#define CAMCTRL_ROI_SET_PAN                         CAMCTRL_MSG(CAMCTRL_MSGTYPE_ROI, 0x0004)
#define CAMCTRL_ROI_SET_WEIGHTING                   CAMCTRL_MSG(CAMCTRL_MSGTYPE_ROI, 0x0005)

//WARP Contol Message
#define CAMCTRL_WARP_ENABLE                         CAMCTRL_MSG(CAMCTRL_MSGTYPE_WARP, 0x0001)
#define CAMCTRL_WARP_UPDATE                         CAMCTRL_MSG(CAMCTRL_MSGTYPE_WARP, 0x0002)

//DISP Control command
#define CAMCTRL_DISP_ENABLE                         CAMCTRL_MSG(CAMCTRL_MSGTYPE_DISP, 0x0001)
#define CAMCTRL_DISP_DRAW_PIXEL                     CAMCTRL_MSG(CAMCTRL_MSGTYPE_DISP, 0x0002)
#define CAMCTRL_DISP_DRAW_LINE                      CAMCTRL_MSG(CAMCTRL_MSGTYPE_DISP, 0x0003)
#define CAMCTRL_DISP_DRAW_RECT                      CAMCTRL_MSG(CAMCTRL_MSGTYPE_DISP, 0x0004)

#define CAMCTRL_DISP_FLUSH                          CAMCTRL_MSG(CAMCTRL_MSGTYPE_DISP, 0x0006)
#define CAMCTRL_DISP_CLEAR                          CAMCTRL_MSG(CAMCTRL_MSGTYPE_DISP, 0x0007)
#define CAMCTRL_DISP_MULTI_CMD                      CAMCTRL_MSG(CAMCTRL_MSGTYPE_DISP, 0x0008)
#define CAMCTRL_DISP_FLUSH_CAPSEQ                   CAMCTRL_MSG(CAMCTRL_MSGTYPE_DISP, 0x0009)

#define CAMCTRL_DISP_SETCHARCONFIG                  CAMCTRL_MSG(CAMCTRL_MSGTYPE_DISP, 0x0010)
#define CAMCTRL_DISP_DRAWSTRING                     CAMCTRL_MSG(CAMCTRL_MSGTYPE_DISP, 0x0011)

#define CAMCTRL_DISP_SET_EXTOSDCLUT                 CAMCTRL_MSG(CAMCTRL_MSGTYPE_DISP, 0x0021)
#define CAMCTRL_DISP_SET_EXTOSDBUFINFO              CAMCTRL_MSG(CAMCTRL_MSGTYPE_DISP, 0x0022)
#define CAMCTRL_DISP_UPDATE_EXTBUF                  CAMCTRL_MSG(CAMCTRL_MSGTYPE_DISP, 0x0023)

#define CAMCTRL_DISP_SET_ROIWINDOW                  CAMCTRL_MSG(CAMCTRL_MSGTYPE_DISP, 0xA000)
#define CAMCTRL_DISP_SET_WARPINFO                   CAMCTRL_MSG(CAMCTRL_MSGTYPE_DISP, 0xA001)
#define CAMCTRL_DISP_DRAW_LDRESULT                  CAMCTRL_MSG(CAMCTRL_MSGTYPE_DISP, 0xA002)

//PLATFORM Control command
#define CAMCTRL_PLATFORM_REBOOT                     CAMCTRL_MSG(CAMCTRL_MSGTYPE_PLATFORM, 0x0001)

/*************************************/
/* Flags definition */
/*************************************/
#define CAMCTRLFLAG_LIVEVIEW_DUAL_INPUT        (0x00000001) //Set for dual vin
#define CAMCTRLFLAG_LIVEVIEW_YUV_INPUT         (0x00000002) //Set for YUV; Clear for RAW
#define CAMCTRLFLAG_LIVEVIEW_TV_OUTPUT         (0x00000004) //Set for TV; Clear for LCD
#define CAMCTRLFLAG_LIVEVIEW_MIPICSI           (0x00000008) //Set for MIPICSI; Clear for HDMI
#define CAMCTRLFLAG_LIVEVIEW_ENBLE_3A          (0x00000010) //Set for enable; Clear for disable
#define CAMCTRLFLAG_LIVEVIEW_NOCHANGE          (0x80000000) //Set for Keep original setting. (ignore the CtrlFlag value in conf command)

/*************************************/
/* Common Data Strcutures definition */
/*************************************/
#define CV_MAX_PARAM_SIZE (1016)
typedef struct _CV_Request_s_ {
    unsigned int Msg;
    unsigned int ParamSize;
    unsigned char Param[CV_MAX_PARAM_SIZE];
} CV_Request_s;

typedef struct DSP_WINDOW_s_{
    unsigned short  OffsetX; /* Horizontal offset of the window */
    unsigned short  OffsetY; /* Vertical offset of the window */
    unsigned short  Width; /* Number of pixels per line in the window */
    unsigned short  Height; /* Number of lines in the window */
} DSP_WINDOW_s;

typedef struct YUV_IMG_BUF_SETTING_s_{
    unsigned int DataFmt; /* YUV Data format: 0->420 or 1->422. */
    unsigned int Pitch; /* YUV data buffer pitch */
    DSP_WINDOW_s  Window; /* Window position and size */
} YUV_IMG_BUF_SETTING_s;

#define AMBA_DSP_MAX_HIER   (13)
typedef struct PYRAMID_CONFIG_s_{
    unsigned short HierBit; /* Bit-wise to enable which layer output shall be on. */
    unsigned short IsPolySqrt2; /* Use Square2 as 2nd layer rescale factor. */
    unsigned short HierPolyWidth; /* Use when IsPolySqrt2 == FALSE */
    unsigned short HierPolyHeight;
    unsigned short HierTag; /* Tag of this modification */

    // Reserve for 64-bit alignment
    unsigned short Reserved;
    unsigned int Reserved32;

    DSP_WINDOW_s HierCropWindow[AMBA_DSP_MAX_HIER]; /* config layer output croping */
} PYRAMID_CONFIG_s;

typedef struct LANE_DETECT_CONFIG_s_{
    unsigned short HierSource; /* lane detect source. */
    unsigned short Width; /* final lane detect output width */
    unsigned short Height; /* final lane detect output height */

    // Reserve for 64-bits alignment
    unsigned short Reserve;

    DSP_WINDOW_s CropWindow; /* lane detect active fov */
} LANE_DETECT_CONFIG_s;

#define MAX_NUM_FOV  8
typedef struct CV_Frame_Init_Data_s_{
    unsigned int fovid;                                /* index of input fov */
    PYRAMID_CONFIG_s pyramid_config;                   /* Configuration for Pyramid hierarchical levels. */
    LANE_DETECT_CONFIG_s lanedection_config;           /* Configuration for Lane dection. */
    YUV_IMG_BUF_SETTING_s pyramid_frame_setting;       /* Frame buffer setting for Pyramid.       *
                                                        * The Pitch should be ALIGN(width,32).    *
                                                        * The OffsetX and OffsetY should be 0.    */

    YUV_IMG_BUF_SETTING_s lanedection_frame_setting;   /* Frame buffer setting for Lane dection. *
                                                        * The Pitch should be ALIGN(width,32).   *
                                                        * The OffsetX and OffsetY should be 0,   *
                                                        * Width should be Final_Width and        *
                                                        * Height should be Final_Height.         */
} CV_Frame_Init_Data_s;

typedef struct vp_frame_list_descriptor_{
    unsigned int fov_id;                            /* fov for this frame list */
    unsigned int frame_list_table_addr;     /* address for pyramid frame list table */
    unsigned int frame_list_table_size;     /* size of pyramid frame list table (Amount of Entries)*/
    unsigned int frame_buffer_length;       /* length of each pyramid frame buffer */
    unsigned int lanedection_frame_list_table_addr; /* address for lanedection frame list table */
    unsigned int lanedection_frame_list_table_size; /* size of lanedection frame list table */
    unsigned int lanedect_frame_buffer_length;      /* length of each lanedection frame buffer */
    unsigned int meta_list_table_addr;        /* address for metadata list table */
    unsigned int meta_list_table_size;        /* size of metadata list table */
    unsigned int meta_buffer_length;         /* length of each metadata buffer */
} vp_frame_list_descriptor;

typedef struct CV_Dsp_Init_Data_s_ {
    unsigned int async_cmd_queue_addr; /* async cmd queue address */
    unsigned int async_cmd_queue_size; /* async cmd queue size */
    unsigned int vp_msg_queue_cb_addr; /* --> dsp_msg_q_info_t for vp */
    unsigned int num_frame_list_descriptor; /* number of input fov */
    vp_frame_list_descriptor frame_list_descriptor[MAX_NUM_FOV];
} CV_Dsp_Init_Data_s;

typedef struct AEROI_SETTING_s_ {
    unsigned int FovId;
    unsigned int Token;
    unsigned int X;
    unsigned int Y;
    unsigned int Width;
    unsigned int Height;
} AEROI_SETTING_s;

typedef struct CV_Roi_AeroiSetting_s_ {
    unsigned int NumChanges;
    AEROI_SETTING_s AeroiSetting[MAX_NUM_FOV];
} CV_Roi_AeroiSetting_s;

typedef struct WARP_SETTING_s_ {
    unsigned int FovId;
    unsigned int Token;
    unsigned int WarpInfoAddr;
} WARP_SETTING_s;

typedef struct CV_Warp_WarpSetting_s_ {
    unsigned int NumChanges;
    WARP_SETTING_s WarpSetting[MAX_NUM_FOV];
} CV_Warp_WarpSetting_s;

typedef struct _OSD_Command_s_ {
    unsigned int Msg;
    unsigned int Channel;
    unsigned int X1;
    unsigned int Y1;
    unsigned int X2;
    unsigned int Y2;
    unsigned int Color;
    unsigned int Thickness;
} OSD_Command_s;

#define MAX_NUM_CMD 30
typedef struct _CV_OSD_CommandSequence_s_ {
    unsigned int CommandNum; /* number of command */
    OSD_Command_s Cmd[MAX_NUM_CMD];
} CV_OSD_CommandSequence_s;

#endif /* __CAMCTRL_MSG_DEF_H__ */

