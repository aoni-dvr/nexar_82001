/**
 *  @file WarningIcon_TV.c
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
 *  @details svc bub warning icon setting
 *
 */

#ifndef WARN_ICON_TV_H      /* Dir 4.10 */
#define WARN_ICON_TV_H

#define SVC_WARN_ICON_WIDTH  (60U)
#define SVC_WARN_ICON_HEIGHT (60U)
#define SVC_WARN_ICON_SIZE   ((SVC_WARN_ICON_WIDTH * SVC_WARN_ICON_HEIGHT) << SVC_OSD_PIXEL_SIZE_SFT)

#ifdef CONFIG_ICAM_REBEL_USAGE
static const UINT32 WarningIcon_ADAS_VOUT = VOUT_IDX_A;
#else
static const UINT32 WarningIcon_ADAS_VOUT = VOUT_IDX_B;
#endif

#if defined CONFIG_ICAM_VOUTB_OSD_BUF_FHD
#define SVC_WARN_ICON_OSD_BUF_W    (1920U)
#define SVC_WARN_ICON_OSD_BUF_H    (1080U)
#elif defined CONFIG_ICAM_VOUTB_OSD_BUF_HD
#define SVC_WARN_ICON_OSD_BUF_W    (1280U)
#define SVC_WARN_ICON_OSD_BUF_H     (720U)
#else
#define SVC_WARN_ICON_OSD_BUF_W    (960U)
#define SVC_WARN_ICON_OSD_BUF_H    (540U)
#endif


static UINT8 WarnIconBSDFrameBuf[SVC_WARN_ICON_SIZE] GNU_SECTION_NOZEROINIT;
static SVC_WARN_ICON_FRAME_s WarnIconBSDLeftFrame = {
    .IconName = "icon_bsd_warn.bmp",
    //.pIcon    = WarnIconBSDFrameBuf,
    .IconInfo   = {{5U,0U,0U,60U,60U},(60U*60U)<<2,WarnIconBSDFrameBuf},
};
static UINT32 WarnIconBSDLeftFrameSeq[1U] = { 0U };
static SVC_WARN_ICON_FRAME_s WarnIconBSDRightFrame = {
    .IconName = "icon_bsd_warn.bmp",
    //.pIcon    = WarnIconBSDFrameBuf,
    .IconInfo   = {{5U,0U,0U,60U,60U},(60U*60U)<<2,WarnIconBSDFrameBuf},
};
static UINT32 WarnIconBSDRightFrameSeq[1U] = { 0U };
static UINT8 WarnIconBSDR1FrameBuf[SVC_WARN_ICON_SIZE] GNU_SECTION_NOZEROINIT;
static SVC_WARN_ICON_FRAME_s WarnIconBSDR1LeftFrame = {
    .IconName = "icon_bsd_r1_warn.bmp",
    //.pIcon    = WarnIconBSDR1FrameBuf,
    .IconInfo   = {{5U,0U,0U,60U,60U},(60U*60U)<<2,WarnIconBSDR1FrameBuf},
};
static UINT32 WarnIconBSDR1LeftFrameSeq[1U] = { 0U };
static SVC_WARN_ICON_FRAME_s WarnIconBSDR1RightFrame = {
    .IconName = "icon_bsd_r1_warn.bmp",
    //.pIcon    = WarnIconBSDR1FrameBuf,
    .IconInfo   = {{5U,0U,0U,60U,60U},(60U*60U)<<2,WarnIconBSDR1FrameBuf},
};
static UINT32 WarnIconBSDR1RightFrameSeq[1U] = { 0U };

static UINT8 WarnIconFCMDFrameBuf[SVC_WARN_ICON_SIZE * 6U] GNU_SECTION_NOZEROINIT;
static SVC_WARN_ICON_FRAME_s WarnIconFCMDFrames[6U] = {
    { 0U, "icon_fcmd_0_warning_0.bmp", { {5U,0U,0U,SVC_WARN_ICON_WIDTH,SVC_WARN_ICON_HEIGHT},(SVC_WARN_ICON_WIDTH*SVC_WARN_ICON_HEIGHT)<<2, &WarnIconFCMDFrameBuf[0U]                      } },
    { 0U, "icon_fcmd_0_warning_1.bmp", { {5U,0U,0U,SVC_WARN_ICON_WIDTH,SVC_WARN_ICON_HEIGHT},(SVC_WARN_ICON_WIDTH*SVC_WARN_ICON_HEIGHT)<<2, &WarnIconFCMDFrameBuf[SVC_WARN_ICON_SIZE]      } },
    { 0U, "icon_fcmd_0_warning_2.bmp", { {5U,0U,0U,SVC_WARN_ICON_WIDTH,SVC_WARN_ICON_HEIGHT},(SVC_WARN_ICON_WIDTH*SVC_WARN_ICON_HEIGHT)<<2, &WarnIconFCMDFrameBuf[SVC_WARN_ICON_SIZE * 2U] } },
    { 0U, "icon_fcmd_0_warning_3.bmp", { {5U,0U,0U,SVC_WARN_ICON_WIDTH,SVC_WARN_ICON_HEIGHT},(SVC_WARN_ICON_WIDTH*SVC_WARN_ICON_HEIGHT)<<2, &WarnIconFCMDFrameBuf[SVC_WARN_ICON_SIZE * 3U] } },
    { 0U, "icon_fcmd_0_warning_4.bmp", { {5U,0U,0U,SVC_WARN_ICON_WIDTH,SVC_WARN_ICON_HEIGHT},(SVC_WARN_ICON_WIDTH*SVC_WARN_ICON_HEIGHT)<<2, &WarnIconFCMDFrameBuf[SVC_WARN_ICON_SIZE * 4U] } },
    { 0U, "icon_fcmd_0_warning_5.bmp", { {5U,0U,0U,SVC_WARN_ICON_WIDTH,SVC_WARN_ICON_HEIGHT},(SVC_WARN_ICON_WIDTH*SVC_WARN_ICON_HEIGHT)<<2, &WarnIconFCMDFrameBuf[SVC_WARN_ICON_SIZE * 5U] } },
};
static UINT32 WarnIconFCMDFrameSeq[6U] = { 0U, 1U, 2U, 3U, 4U, 5U };
static UINT8 WarnIconFCMD_2xFrameBuf[4U * SVC_WARN_ICON_SIZE * 6U] GNU_SECTION_NOZEROINIT;
static SVC_WARN_ICON_FRAME_s WarnIconFCMD_2xFrames[6U] = {
    { 0U, "icon_fcmd_0_warning_0_2x.bmp", { {5U,0U,0U,120U,120U},(120U*120U)<<2, &WarnIconFCMD_2xFrameBuf[0U]                      } },
    { 0U, "icon_fcmd_0_warning_1_2x.bmp", { {5U,0U,0U,120U,120U},(120U*120U)<<2, &WarnIconFCMD_2xFrameBuf[4U * SVC_WARN_ICON_SIZE]      } },
    { 0U, "icon_fcmd_0_warning_2_2x.bmp", { {5U,0U,0U,120U,120U},(120U*120U)<<2, &WarnIconFCMD_2xFrameBuf[4U * SVC_WARN_ICON_SIZE * 2U] } },
    { 0U, "icon_fcmd_0_warning_3_2x.bmp", { {5U,0U,0U,120U,120U},(120U*120U)<<2, &WarnIconFCMD_2xFrameBuf[4U * SVC_WARN_ICON_SIZE * 3U] } },
    { 0U, "icon_fcmd_0_warning_4_2x.bmp", { {5U,0U,0U,120U,120U},(120U*120U)<<2, &WarnIconFCMD_2xFrameBuf[4U * SVC_WARN_ICON_SIZE * 4U] } },
    { 0U, "icon_fcmd_0_warning_5_2x.bmp", { {5U,0U,0U,120U,120U},(120U*120U)<<2, &WarnIconFCMD_2xFrameBuf[4U * SVC_WARN_ICON_SIZE * 5U] } },
};
static UINT8 WarnIconFCWSFrameBufR1[SVC_WARN_ICON_SIZE * 5U] GNU_SECTION_NOZEROINIT;
static UINT8 WarnIconFCWSFrameBufR2[SVC_WARN_ICON_SIZE * 5U] GNU_SECTION_NOZEROINIT;
static SVC_WARN_ICON_FRAME_s WarnIconFCWSFramesR2[5U] = {
    { 0U, "icon_fcws_0_warning_0.bmp", { {5U,0U,0U,SVC_WARN_ICON_WIDTH,SVC_WARN_ICON_HEIGHT},(SVC_WARN_ICON_WIDTH*SVC_WARN_ICON_HEIGHT)<<2, &WarnIconFCWSFrameBufR1[0U]                     } },
    { 0U, "icon_fcws_0_warning_1.bmp", { {5U,0U,0U,SVC_WARN_ICON_WIDTH,SVC_WARN_ICON_HEIGHT},(SVC_WARN_ICON_WIDTH*SVC_WARN_ICON_HEIGHT)<<2, &WarnIconFCWSFrameBufR1[SVC_WARN_ICON_SIZE]     } },
    { 0U, "icon_fcws_0_warning_2.bmp", { {5U,0U,0U,SVC_WARN_ICON_WIDTH,SVC_WARN_ICON_HEIGHT},(SVC_WARN_ICON_WIDTH*SVC_WARN_ICON_HEIGHT)<<2, &WarnIconFCWSFrameBufR1[SVC_WARN_ICON_SIZE * 2U]} },
    { 0U, "icon_fcws_0_warning_3.bmp", { {5U,0U,0U,SVC_WARN_ICON_WIDTH,SVC_WARN_ICON_HEIGHT},(SVC_WARN_ICON_WIDTH*SVC_WARN_ICON_HEIGHT)<<2, &WarnIconFCWSFrameBufR1[SVC_WARN_ICON_SIZE * 3U]} },
    { 0U, "icon_fcws_0_warning_4.bmp", { {5U,0U,0U,SVC_WARN_ICON_WIDTH,SVC_WARN_ICON_HEIGHT},(SVC_WARN_ICON_WIDTH*SVC_WARN_ICON_HEIGHT)<<2, &WarnIconFCWSFrameBufR1[SVC_WARN_ICON_SIZE * 4U]} },
};
static SVC_WARN_ICON_FRAME_s WarnIconFCWSFramesR1[5U] = {
    { 0U, "icon_fcws_1_warning_0.bmp", { {5U,0U,0U,SVC_WARN_ICON_WIDTH,SVC_WARN_ICON_HEIGHT},(SVC_WARN_ICON_WIDTH*SVC_WARN_ICON_HEIGHT)<<2, &WarnIconFCWSFrameBufR2[0U]                      } },
    { 0U, "icon_fcws_1_warning_1.bmp", { {5U,0U,0U,SVC_WARN_ICON_WIDTH,SVC_WARN_ICON_HEIGHT},(SVC_WARN_ICON_WIDTH*SVC_WARN_ICON_HEIGHT)<<2, &WarnIconFCWSFrameBufR2[SVC_WARN_ICON_SIZE]      } },
    { 0U, "icon_fcws_1_warning_2.bmp", { {5U,0U,0U,SVC_WARN_ICON_WIDTH,SVC_WARN_ICON_HEIGHT},(SVC_WARN_ICON_WIDTH*SVC_WARN_ICON_HEIGHT)<<2, &WarnIconFCWSFrameBufR2[SVC_WARN_ICON_SIZE * 2U] } },
    { 0U, "icon_fcws_1_warning_3.bmp", { {5U,0U,0U,SVC_WARN_ICON_WIDTH,SVC_WARN_ICON_HEIGHT},(SVC_WARN_ICON_WIDTH*SVC_WARN_ICON_HEIGHT)<<2, &WarnIconFCWSFrameBufR2[SVC_WARN_ICON_SIZE * 3U] } },
    { 0U, "icon_fcws_1_warning_4.bmp", { {5U,0U,0U,SVC_WARN_ICON_WIDTH,SVC_WARN_ICON_HEIGHT},(SVC_WARN_ICON_WIDTH*SVC_WARN_ICON_HEIGHT)<<2, &WarnIconFCWSFrameBufR2[SVC_WARN_ICON_SIZE * 4U] } },
};
static UINT32 WarnIconFCWSFrameSeq[8U] = { 0U, 1U, 2U, 3U, 4U, 3U, 2U, 1U };

static UINT8 WarnIconFCWS_2xFrameBufR1[4U * SVC_WARN_ICON_SIZE * 5U] GNU_SECTION_NOZEROINIT;
static UINT8 WarnIconFCWS_2xFrameBufR2[4U * SVC_WARN_ICON_SIZE * 5U] GNU_SECTION_NOZEROINIT;
static SVC_WARN_ICON_FRAME_s WarnIconFCWS_2xFramesR2[5U] = {
    { 0U, "icon_fcws_0_warning_0_2x.bmp", { {5U,0U,0U,120U,120U},(120U*120U)<<2, &WarnIconFCWS_2xFrameBufR1[0U]                     } },
    { 0U, "icon_fcws_0_warning_1_2x.bmp", { {5U,0U,0U,120U,120U},(120U*120U)<<2, &WarnIconFCWS_2xFrameBufR1[4U * SVC_WARN_ICON_SIZE]     } },
    { 0U, "icon_fcws_0_warning_2_2x.bmp", { {5U,0U,0U,120U,120U},(120U*120U)<<2, &WarnIconFCWS_2xFrameBufR1[4U * SVC_WARN_ICON_SIZE * 2U]} },
    { 0U, "icon_fcws_0_warning_3_2x.bmp", { {5U,0U,0U,120U,120U},(120U*120U)<<2, &WarnIconFCWS_2xFrameBufR1[4U * SVC_WARN_ICON_SIZE * 3U]} },
    { 0U, "icon_fcws_0_warning_4_2x.bmp", { {5U,0U,0U,120U,120U},(120U*120U)<<2, &WarnIconFCWS_2xFrameBufR1[4U * SVC_WARN_ICON_SIZE * 4U]} },
};
static SVC_WARN_ICON_FRAME_s WarnIconFCWS_2xFramesR1[5U] = {
    { 0U, "icon_fcws_1_warning_0_2x.bmp", { {5U,0U,0U,120U,120U},(120U*120U)<<2, &WarnIconFCWS_2xFrameBufR2[0U]                      } },
    { 0U, "icon_fcws_1_warning_1_2x.bmp", { {5U,0U,0U,120U,120U},(120U*120U)<<2, &WarnIconFCWS_2xFrameBufR2[4U * SVC_WARN_ICON_SIZE]      } },
    { 0U, "icon_fcws_1_warning_2_2x.bmp", { {5U,0U,0U,120U,120U},(120U*120U)<<2, &WarnIconFCWS_2xFrameBufR2[4U * SVC_WARN_ICON_SIZE * 2U] } },
    { 0U, "icon_fcws_1_warning_3_2x.bmp", { {5U,0U,0U,120U,120U},(120U*120U)<<2, &WarnIconFCWS_2xFrameBufR2[4U * SVC_WARN_ICON_SIZE * 3U] } },
    { 0U, "icon_fcws_1_warning_4_2x.bmp", { {5U,0U,0U,120U,120U},(120U*120U)<<2, &WarnIconFCWS_2xFrameBufR2[4U * SVC_WARN_ICON_SIZE * 4U] } },
};

static UINT8 WarnIconLDWSBGFrameBuf[SVC_WARN_ICON_SIZE] GNU_SECTION_NOZEROINIT;
static SVC_WARN_ICON_FRAME_s WarnIconLDWSBGFrame = {
    .IconName = "icon_ldws_0.bmp",
    //.pIcon    = WarnIconLDWSBGFrameBuf,
    .IconInfo   = {{5U,0U,0U,SVC_WARN_ICON_WIDTH,SVC_WARN_ICON_HEIGHT},(SVC_WARN_ICON_WIDTH*SVC_WARN_ICON_HEIGHT)<<2,WarnIconLDWSBGFrameBuf},
};
static UINT8 WarnIconLDWS_2xBGFrameBuf[4U * SVC_WARN_ICON_SIZE] GNU_SECTION_NOZEROINIT;
static SVC_WARN_ICON_FRAME_s WarnIconLDWS_2xBGFrame = {
    .IconName = "icon_ldws_0_2x.bmp",
    //.pIcon    = WarnIconLDWS_2xBGFrameBuf,
    .IconInfo   = {{5U,0U,0U,(SVC_WARN_ICON_WIDTH<<1),SVC_WARN_ICON_HEIGHT<<1},(((SVC_WARN_ICON_WIDTH<<1))*(SVC_WARN_ICON_HEIGHT<<1))<<2,WarnIconLDWS_2xBGFrameBuf},
};
static UINT8 WarnIconLDWSLeftFrameBuf[SVC_WARN_ICON_SIZE] GNU_SECTION_NOZEROINIT;
static SVC_WARN_ICON_FRAME_s WarnIconLDWSLeftFrame = {
    .IconName = "icon_ldws_0_warning_left.bmp",
    //.pIcon    = WarnIconLDWSLeftFrameBuf,
    .IconInfo   = {{5U,0U,0U,20U,60U},(20U*60U)<<2,WarnIconLDWSLeftFrameBuf},
};
static UINT32 WarnIconLDWSLeftFrameSeq[1U] = { 0U };
static UINT8 WarnIconLDWSRightFrameBuf[SVC_WARN_ICON_SIZE] GNU_SECTION_NOZEROINIT;
static SVC_WARN_ICON_FRAME_s WarnIconLDWSRightFrame = {
    .IconName = "icon_ldws_0_warning_right.bmp",
    //.pIcon    = WarnIconLDWSRightFrameBuf,
    .IconInfo   = {{5U,0U,0U,(20U),60U},((20U)*60U)<<2,WarnIconLDWSRightFrameBuf},
};
static UINT32 WarnIconLDWSRightFrameSeq[1U] = { 0U };

static UINT8 WarnIconLDWS_2xLeftFrameBuf[4U * SVC_WARN_ICON_SIZE] GNU_SECTION_NOZEROINIT;
static SVC_WARN_ICON_FRAME_s WarnIconLDWS_2xLeftFrame = {
    .IconName = "icon_ldws_0_warning_left_2x.bmp",
    //.pIcon    = WarnIconLDWSLeftFrameBuf,
    .IconInfo   = {{5U,0U,0U,40U,120U},(40U*120U)<<2,WarnIconLDWS_2xLeftFrameBuf},
};
static UINT8 WarnIconLDWS_2xRightFrameBuf[4U * SVC_WARN_ICON_SIZE] GNU_SECTION_NOZEROINIT;
static SVC_WARN_ICON_FRAME_s WarnIconLDWS_2xRightFrame = {
    .IconName = "icon_ldws_0_warning_right_2.bmp",
    //.pIcon    = WarnIconLDWSRightFrameBuf,
    .IconInfo   = {{5U,0U,0U,40U,120U},(40U*120U)<<2,WarnIconLDWS_2xRightFrameBuf},
};

static SVC_WARN_ICON_CFG_s g_WarnIconCfg[SVC_WARN_ICON_TYPE_NUM] = {
/* EMR */
{
    .WarnIconNum = 3U,
    .WarnIcon = {
        [SVC_WARN_ICON_BSD_L] = {
            .Vout            = VOUT_IDX_B,
            .IconArea        = { 360U, 360U, 420U, 420U },
            .IconDataNum     = 1U,
            .pIconData       = &WarnIconBSDLeftFrame,
            .IconFrmSeqNum   = 1U,
            .pIconFrmSeq     = WarnIconBSDLeftFrameSeq,
            .IconFrmInterval = 300U,
        },
        [SVC_WARN_ICON_BSD_R] = {
            .Vout            = VOUT_IDX_B,
            .IconArea        = { 360U, 300U, 420U, 360U },
            .IconDataNum     = 1U,
            .pIconData       = &WarnIconBSDRightFrame,
            .IconFrmSeqNum   = 1U,
            .pIconFrmSeq     = WarnIconBSDRightFrameSeq,
            .IconFrmInterval = 300U,
        },
        [SVC_WARN_ICON_BSD_R1_L] = {
            .Vout            = VOUT_IDX_B,
            .IconArea        = { 360U, 360U, 420U, 420U },
            .IconDataNum     = 1U,
            .pIconData       = &WarnIconBSDR1LeftFrame,
            .IconFrmSeqNum   = 1U,
            .pIconFrmSeq     = WarnIconBSDR1LeftFrameSeq,
            .IconFrmInterval = 300U,
        },
        [SVC_WARN_ICON_BSD_R1_R] = {
            .Vout            = VOUT_IDX_B,
            .IconArea        = { 360U, 300U, 420U, 360U },
            .IconDataNum     = 1U,
            .pIconData       = &WarnIconBSDR1RightFrame,
            .IconFrmSeqNum   = 1U,
            .pIconFrmSeq     = WarnIconBSDR1RightFrameSeq,
            .IconFrmInterval = 300U,
        },
        [SVC_WARN_ICON_4] = {
            .Vout            = 0xFFU,
            .IconArea        = {0},
            .IconDataNum     = 0U,
            .pIconData       = NULL,
            .IconFrmSeqNum   = 0U,
            .pIconFrmSeq     = NULL,
            .IconFrmInterval = 0U,
        },
        [SVC_WARN_ICON_5] = {
            .Vout            = 0xFFU,
            .IconArea        = {0},
            .IconDataNum     = 0U,
            .pIconData       = NULL,
            .IconFrmSeqNum   = 0U,
            .pIconFrmSeq     = NULL,
            .IconFrmInterval = 0U,
        },
        [SVC_WARN_ICON_6] = {
            .Vout            = 0xFFU,
            .IconArea        = {0},
            .IconDataNum     = 0U,
            .pIconData       = NULL,
            .IconFrmSeqNum   = 0U,
            .pIconFrmSeq     = NULL,
            .IconFrmInterval = 0U,
        },
        [SVC_WARN_ICON_7] = {
            .Vout            = 0xFFU,
            .IconArea        = {0},
            .IconDataNum     = 0U,
            .pIconData       = NULL,
            .IconFrmSeqNum   = 0U,
            .pIconFrmSeq     = NULL,
            .IconFrmInterval = 0U,
        },
        [SVC_WARN_ICON_8] = {
            .Vout            = 0xFFU,
            .IconArea        = {0},
            .IconDataNum     = 0U,
            .pIconData       = NULL,
            .IconFrmSeqNum   = 0U,
            .pIconFrmSeq     = NULL,
            .IconFrmInterval = 0U,
        },
        [SVC_WARN_ICON_9] = {
            .Vout            = 0xFFU,
            .IconArea        = {0},
            .IconDataNum     = 0U,
            .pIconData       = NULL,
            .IconFrmSeqNum   = 0U,
            .pIconFrmSeq     = NULL,
            .IconFrmInterval = 0U,
        },
        [SVC_WARN_ICON_10] = {
            .Vout            = 0xFFU,
            .IconArea        = {0},
            .IconDataNum     = 0U,
            .pIconData       = NULL,
            .IconFrmSeqNum   = 0U,
            .pIconFrmSeq     = NULL,
            .IconFrmInterval = 0U,
        },
        [SVC_WARN_ICON_11] = {
            .Vout            = 0xFFU,
            .IconArea        = {0},
            .IconDataNum     = 0U,
            .pIconData       = NULL,
            .IconFrmSeqNum   = 0U,
            .pIconFrmSeq     = NULL,
            .IconFrmInterval = 0U,
        },
    },
},
/* ADAS */
{
    .WarnIconNum = 12U,
    .WarnIcon = {
        [SVC_WARN_ICON_FCWS_R2] = {
            .Vout            = WarningIcon_ADAS_VOUT,
            .IconArea        = { SVC_WARN_ICON_WIDTH*3U, SVC_WARN_ICON_OSD_BUF_H-(SVC_WARN_ICON_HEIGHT<<1)-10U, SVC_WARN_ICON_WIDTH*4U, SVC_WARN_ICON_OSD_BUF_H-SVC_WARN_ICON_HEIGHT-10U },
            .IconDataNum     = 5U,
            .pIconData       = WarnIconFCWSFramesR2,
            .IconFrmSeqNum   = 8U,
            .pIconFrmSeq     = WarnIconFCWSFrameSeq,
            .IconFrmInterval = 100U,
        },
        [SVC_WARN_ICON_FCMD] = {
            .Vout            = WarningIcon_ADAS_VOUT,
            .IconArea        = { SVC_WARN_ICON_WIDTH*4U, SVC_WARN_ICON_OSD_BUF_H-(SVC_WARN_ICON_HEIGHT<<1)-10U, SVC_WARN_ICON_WIDTH*5U, SVC_WARN_ICON_OSD_BUF_H-SVC_WARN_ICON_HEIGHT-10U },
            .IconDataNum     = 6U,
            .pIconData       = WarnIconFCMDFrames,
            .IconFrmSeqNum   = 6U,
            .pIconFrmSeq     = WarnIconFCMDFrameSeq,
            .IconFrmInterval = 200U,
        },
        [SVC_WARN_ICON_LDWS] = {
            .Vout            = WarningIcon_ADAS_VOUT,
            .IconArea        = { SVC_WARN_ICON_WIDTH*5U, SVC_WARN_ICON_OSD_BUF_H-(SVC_WARN_ICON_HEIGHT<<1)-10U, SVC_WARN_ICON_WIDTH*6U, SVC_WARN_ICON_OSD_BUF_H-SVC_WARN_ICON_HEIGHT-10U },
            .IconDataNum     = 1U,
            .pIconData       = &WarnIconLDWSBGFrame,
            .IconFrmSeqNum   = 0U,
            .pIconFrmSeq     = NULL,
            .IconFrmInterval = 0U,
        },
        [SVC_WARN_ICON_LDWS_L] = {
            .Vout            = WarningIcon_ADAS_VOUT,
            .IconArea        = { (SVC_WARN_ICON_WIDTH*5U), ((SVC_WARN_ICON_OSD_BUF_H - (SVC_WARN_ICON_HEIGHT<<1)) -10U), ((SVC_WARN_ICON_WIDTH*5U) + (SVC_WARN_ICON_WIDTH/3U)), ((SVC_WARN_ICON_OSD_BUF_H-SVC_WARN_ICON_HEIGHT)-10U) },
            .IconDataNum     = 1U,
            .pIconData       = &WarnIconLDWSLeftFrame,
            .IconFrmSeqNum   = 1U,
            .pIconFrmSeq     = WarnIconLDWSLeftFrameSeq,
            .IconFrmInterval = 200U,
        },
        [SVC_WARN_ICON_LDWS_R] = {
            .Vout            = WarningIcon_ADAS_VOUT,
            .IconArea        = { ((SVC_WARN_ICON_WIDTH*5U)+((SVC_WARN_ICON_WIDTH/3U)*2U)), ((SVC_WARN_ICON_OSD_BUF_H-(SVC_WARN_ICON_HEIGHT<<1))-10U), (SVC_WARN_ICON_WIDTH*5U)+((SVC_WARN_ICON_WIDTH/3U)*2U) + (SVC_WARN_ICON_WIDTH/3U), ((SVC_WARN_ICON_OSD_BUF_H-SVC_WARN_ICON_HEIGHT)-10U) },
            .IconDataNum     = 1U,
            .pIconData       = &WarnIconLDWSRightFrame,
            .IconFrmSeqNum   = 1U,
            .pIconFrmSeq     = WarnIconLDWSRightFrameSeq,
            .IconFrmInterval = 200U,
        },
        [SVC_WARN_ICON_FCWS_R1] = {
            .Vout            = WarningIcon_ADAS_VOUT,
            .IconArea        = { SVC_WARN_ICON_WIDTH*3U, SVC_WARN_ICON_OSD_BUF_H-(SVC_WARN_ICON_HEIGHT<<1)-10U, SVC_WARN_ICON_WIDTH*4U, SVC_WARN_ICON_OSD_BUF_H-SVC_WARN_ICON_HEIGHT-10U },
            .IconDataNum     = 5U,
            .pIconData       = WarnIconFCWSFramesR1,
            .IconFrmSeqNum   = 8U,
            .pIconFrmSeq     = WarnIconFCWSFrameSeq,
            .IconFrmInterval = 100U,
        },
        [SVC_WARN_ICON_FCMD_2x] = {
            .Vout            = WarningIcon_ADAS_VOUT,
            .IconArea        = {((SVC_WARN_ICON_OSD_BUF_W-SVC_WARN_ICON_WIDTH)>>1), 60U, ((SVC_WARN_ICON_OSD_BUF_W-SVC_WARN_ICON_WIDTH)>>1) + (SVC_WARN_ICON_WIDTH<<1), 60U + (SVC_WARN_ICON_HEIGHT<<1) },
            .IconDataNum     = 6U,
            .pIconData       = WarnIconFCMD_2xFrames,
            .IconFrmSeqNum   = 6U,
            .pIconFrmSeq     = WarnIconFCMDFrameSeq,
            .IconFrmInterval = 200U,
        },
        [SVC_WARN_ICON_FCWS_R2_2x] = {
            .Vout            = WarningIcon_ADAS_VOUT,
            .IconArea        = {((SVC_WARN_ICON_OSD_BUF_W-SVC_WARN_ICON_WIDTH)>>1), 60U, ((SVC_WARN_ICON_OSD_BUF_W-SVC_WARN_ICON_WIDTH)>>1) + (SVC_WARN_ICON_WIDTH<<1), 60U + (SVC_WARN_ICON_HEIGHT<<1) },
            .IconDataNum     = 5U,
            .pIconData       = WarnIconFCWS_2xFramesR2,
            .IconFrmSeqNum   = 8U,
            .pIconFrmSeq     = WarnIconFCWSFrameSeq,
            .IconFrmInterval = 100U,
        },
        [SVC_WARN_ICON_FCWS_R1_2x] = {
            .Vout            = WarningIcon_ADAS_VOUT,
            .IconArea        = {((SVC_WARN_ICON_OSD_BUF_W-SVC_WARN_ICON_WIDTH)>>1), 60U, ((SVC_WARN_ICON_OSD_BUF_W-SVC_WARN_ICON_WIDTH)>>1) + (SVC_WARN_ICON_WIDTH<<1), 60U + (SVC_WARN_ICON_HEIGHT<<1) },
            .IconDataNum     = 5U,
            .pIconData       = WarnIconFCWS_2xFramesR1,
            .IconFrmSeqNum   = 8U,
            .pIconFrmSeq     = WarnIconFCWSFrameSeq,
            .IconFrmInterval = 100U,
        },
        [SVC_WARN_ICON_LDWS_2x] = {
            .Vout            = WarningIcon_ADAS_VOUT,
            .IconArea        = {((SVC_WARN_ICON_OSD_BUF_W-SVC_WARN_ICON_WIDTH)>>1), 60U, ((SVC_WARN_ICON_OSD_BUF_W-SVC_WARN_ICON_WIDTH)>>1) + (SVC_WARN_ICON_WIDTH<<1), 60U + (SVC_WARN_ICON_HEIGHT<<1) },
            .IconDataNum     = 1U,
            .pIconData       = &WarnIconLDWS_2xBGFrame,
            .IconFrmSeqNum   = 0U,
            .pIconFrmSeq     = NULL,
            .IconFrmInterval = 0U,
        },
        [SVC_WARN_ICON_LDWS_L_2x] = {
            .Vout            = WarningIcon_ADAS_VOUT,
            .IconArea        = {((SVC_WARN_ICON_OSD_BUF_W-SVC_WARN_ICON_WIDTH)>>1), 60U, ((SVC_WARN_ICON_OSD_BUF_W-SVC_WARN_ICON_WIDTH)>>1) + (((SVC_WARN_ICON_WIDTH<<1))/3U), 60U + (SVC_WARN_ICON_HEIGHT<<1) },
            .IconDataNum     = 1U,
            .pIconData       = &WarnIconLDWS_2xLeftFrame,
            .IconFrmSeqNum   = 1U,
            .pIconFrmSeq     = WarnIconLDWSLeftFrameSeq,
            .IconFrmInterval = 200U,
        },
        [SVC_WARN_ICON_LDWS_R_2x] = {
            .Vout            = WarningIcon_ADAS_VOUT,
            .IconArea        = {(((SVC_WARN_ICON_OSD_BUF_W-SVC_WARN_ICON_WIDTH)>>1))+((((SVC_WARN_ICON_WIDTH<<1))/3U)*2U), 60U, ((SVC_WARN_ICON_OSD_BUF_W-SVC_WARN_ICON_WIDTH)>>1) + (SVC_WARN_ICON_WIDTH<<1), 60U + (SVC_WARN_ICON_HEIGHT<<1) },
            .IconDataNum     = 1U,
            .pIconData       = &WarnIconLDWS_2xRightFrame,
            .IconFrmSeqNum   = 1U,
            .pIconFrmSeq     = WarnIconLDWSRightFrameSeq,
            .IconFrmInterval = 200U,
        },
    },
},
};

#endif /* WARN_ICON_TV_H */
