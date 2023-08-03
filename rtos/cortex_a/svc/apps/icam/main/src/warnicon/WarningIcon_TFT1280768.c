/**
 *  @file WarningIcon_TFT1280768.c
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

#ifndef WARN_ICON_TFT1280768_H      /* Dir 4.10 */
#define WARN_ICON_TFT1280768_H

#define SVC_WARN_ICON_WIDTH  (60U)
#define SVC_WARN_ICON_HEIGHT (60U)
#define SVC_WARN_ICON_SIZE   ((SVC_WARN_ICON_WIDTH * SVC_WARN_ICON_HEIGHT) << SVC_OSD_PIXEL_SIZE_SFT)

// static UINT8 WarnIconBMP[SVC_WARN_ICON_NUM][SVC_WARN_ICON_SIZE];

static SVC_WARN_ICON_CFG_s g_WarnIconCfg[SVC_WARN_ICON_TYPE_NUM] = {
/* EMR */
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

#endif /* WARN_ICON_TFT1280768_H */
