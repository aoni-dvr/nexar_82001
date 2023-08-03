/**
*  @file AmbaMux_Def.h
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
*  @details amba muxer define
*
*/

#ifndef AMBA_MUX_DEF_H
#define AMBA_MUX_DEF_H

#define AMBA_RSC_TYPE_VIDEO          (0U)
#define AMBA_RSC_TYPE_AUDIO          (1U)
#define AMBA_RSC_TYPE_DATA           (2U)
#define AMBA_RSC_TYPE_NUM            (3U)

/* video subtype */
#define AMBA_RSC_VID_SUBTYPE_H264    (0U)
#define AMBA_RSC_VID_SUBTYPE_MJPG    (1U)
#define AMBA_RSC_VID_SUBTYPE_HEVC    (2U)
/* audio subtype */
#define AMBA_RSC_AUD_SUBTYPE_NONE    (0U)
#define AMBA_RSC_AUD_SUBTYPE_AAC     (1U)
#define AMBA_RSC_AUD_SUBTYPE_PCM     (2U)
/* data subtype */
#define AMBA_RSC_DATA_SUBTYPE_CAN    (0U)
#define AMBA_RSC_DATA_SUBTYPE_PTP    (1U)
#define AMBA_RSC_DATA_SUBTYPE_MAX    (2U)

/* Record Box Type Definition */
#define AMBA_RBX_TYPE_MP4            (0U)

/* SubType Definition */
#define AMBA_SUB_TYPE_FMP4           (0U)

#define AMBA_REC_MIA_VIDEO           (0U)
#define AMBA_REC_MIA_AUDIO           (1U)
#define AMBA_REC_MIA_DATA            (2U)
#define AMBA_REC_MIA_MAX             (3U)

typedef struct {
    UINT32  FileSplitTime;      /* minutes*/
    UINT32  IsTimeLapse;
    UINT32  IsHierGop;

    struct {
        UINT32  BitRate;
        UINT32  FrameRate;
        UINT32  FrameTick;

        /* video reserved config */
        #define MIA_RSV_VIDEO_M     (0U)
        #define MIA_RSV_VIDEO_N     (1U)
        #define MIA_RSV_VIDEO_IDRIT (2U)
        #define MIA_RSV_VIDEO_SLICE (3U)
        #define MIA_RSV_VIDEO_TILE  (4U)

        #define MIA_RSV_MAX         (5U)

        UINT32  RsvCfg[MIA_RSV_MAX];
    } MiaCfg[AMBA_REC_MIA_MAX];
} AMBA_REC_EVAL_s;

typedef struct {
    /* record source information */
    UINT32  SrcType;
    UINT32  SrcSubType;
    void    *pSrcData;
    ULONG   SrcBufBase;
    UINT32  SrcBufSize;
} AMBA_RSC_DESC_s;

typedef struct {
    void    *pRawData;
    char    *pText;
    UINT32  TexLen;
} AMBA_RSC_TEXT_DESC_s;

typedef struct {
    UINT32    BoxSize;

    #define UDTA_BOX_TYPE_SVCD  (0x73766364U)
    UINT32    BoxType;

    UINT32    IsEncrypt;    /* 0: non-encrypted clip, 1: encrypted clip*/
    UINT8     Rsv[116U];    /* reserve the space to keep the box size 128 Byte for future usage */
} AMBA_MP4_UDTA_SVCD_BOX_s;

#endif  /* AMBA_MUX_DEF_H */
