/**
 *  @file WarningIcon_ZS095BH301A8H3_BII.c
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

#ifndef WARN_ICON_ZS095BH301A8H3_H      /* Dir 4.10 */
#define WARN_ICON_ZS095BH301A8H3_H

#define SVC_WARN_ICON_WIDTH  (60U)
#define SVC_WARN_ICON_HEIGHT (60U)
#define SVC_WARN_ICON_SIZE   ((SVC_WARN_ICON_WIDTH * SVC_WARN_ICON_HEIGHT) << SVC_OSD_PIXEL_SIZE_SFT)

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

static SVC_WARN_ICON_CFG_s g_WarnIconCfg[SVC_WARN_ICON_TYPE_NUM] = {
/* EMR */
{
    .WarnIconNum = 4U,
    .WarnIcon = {
        [SVC_WARN_ICON_BSD_L] = {
            .Vout            = VOUT_IDX_B,
            .IconArea        = { 5U, 475U, 65U, 535U },
            .IconDataNum     = 1U,
            .pIconData       = &WarnIconBSDLeftFrame,
            .IconFrmSeqNum   = 1U,
            .pIconFrmSeq     = WarnIconBSDLeftFrameSeq,
            .IconFrmInterval = 300U,
        },
        [SVC_WARN_ICON_BSD_R] = {
            .Vout            = VOUT_IDX_B,
            .IconArea        = { 895U, 475U, 955U, 535U },
            .IconDataNum     = 1U,
            .pIconData       = &WarnIconBSDRightFrame,
            .IconFrmSeqNum   = 1U,
            .pIconFrmSeq     = WarnIconBSDRightFrameSeq,
            .IconFrmInterval = 300U,
        },
        [SVC_WARN_ICON_BSD_R1_L] = {
            .Vout            = VOUT_IDX_B,
            .IconArea        = { 5U, 475U, 65U, 535U },
            .IconDataNum     = 1U,
            .pIconData       = &WarnIconBSDR1LeftFrame,
            .IconFrmSeqNum   = 1U,
            .pIconFrmSeq     = WarnIconBSDR1LeftFrameSeq,
            .IconFrmInterval = 300U,
        },
        [SVC_WARN_ICON_BSD_R1_R] = {
            .Vout            = VOUT_IDX_B,
            .IconArea        = { 895U, 475U, 955U, 535U },
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
    .WarnIconNum = 0U,
    .WarnIcon = {
        [SVC_WARN_ICON_0] = {
            .Vout            = 0xFFU,
            .IconArea        = {0},
            .IconDataNum     = 0U,
            .pIconData       = NULL,
            .IconFrmSeqNum   = 0U,
            .pIconFrmSeq     = NULL,
            .IconFrmInterval = 0U,
        },
        [SVC_WARN_ICON_1] = {
            .Vout            = 0xFFU,
            .IconArea        = {0},
            .IconDataNum     = 0U,
            .pIconData       = NULL,
            .IconFrmSeqNum   = 0U,
            .pIconFrmSeq     = NULL,
            .IconFrmInterval = 0U,
        },
        [SVC_WARN_ICON_2] = {
            .Vout            = 0xFFU,
            .IconArea        = {0},
            .IconDataNum     = 0U,
            .pIconData       = NULL,
            .IconFrmSeqNum   = 0U,
            .pIconFrmSeq     = NULL,
            .IconFrmInterval = 0U,
        },
        [SVC_WARN_ICON_3] = {
            .Vout            = 0xFFU,
            .IconArea        = {0},
            .IconDataNum     = 0U,
            .pIconData       = NULL,
            .IconFrmSeqNum   = 0U,
            .pIconFrmSeq     = NULL,
            .IconFrmInterval = 0U,
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
};

#endif /* WARN_ICON_ZS095BH301A8H3_H */
