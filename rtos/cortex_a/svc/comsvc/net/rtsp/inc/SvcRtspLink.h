/**
*  @file SvcRtspLink.h
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
*  @details RTSP Ambalink
*/


#ifndef SVC_RTSPLINK_H
#define SVC_RTSPLINK_H


#define RTSP_LINK_CMD_START     (0x00000001U)
#define RTSP_LINK_CMD_STOP      (0x00000002U)

#define RTSP_LINK_NOTIFY_VID    (0x00000001U)
#define RTSP_LINK_NOTIFY_AUD    (0x00000002U)
#if defined(CONFIG_APP_FLOW_CARDV_AONI)
#define RTSP_LINK_NOTIFY_GSENSOR (0x00000003U)
#endif

#define RTSP_LINK_AUD_AAC       (0x00000001U)
#define RTSP_LINK_AUD_PCM       (0x00000002U)

#define PBK_EOS_FLG_V   0x01U
#define PBK_EOS_FLG_A   0x02U
#define PBK_EOS_FLG_VA  (PBK_EOS_FLG_V|PBK_EOS_FLG_A)

typedef struct {
    UINT32                            StreamId;
    AMBA_DSP_VIDEO_ENC_STRM_CONFIG_s  VidInfo;
} SVC_EXAMFRAMESHARE_VID_CONFIG_s;

typedef struct {
    UINT32                        StreamId;
    UINT32                        Format;
    ULONG                         BitsBufBase;
    ULONG                         BitsBufSize;
    AMBA_AUDIO_ENC_CREATE_INFO_s  AudInfo;
} SVC_EXAMFRAMESHARE_AUD_CONFIG_s;

#if defined(CONFIG_APP_FLOW_CARDV_AONI)
typedef struct {
    UINT32  *pHdlr;
    UINT32  Eos;
    UINT32  DataSize;
    UINT8   *pBufAddr;
    UINT64  EncodedSamples;
} GSENSOR_DESC_s;
#endif

typedef struct {
    UINT8  inParam[128];        /**< The input param of playback OP */
    UINT8* resParam;            /**< The response param for playback OP */
} PLAYBACK_OP_INFO_s;

typedef enum {
    RTSP_PLAYBACK_OPEN = 1,             /**< open file for playback. */
    RTSP_PLAYBACK_PLAY,                 /**< start playback. */
    RTSP_PLAYBACK_STOP,                 /**< stop playback */
    RTSP_LIVE_PLAY,                     /**< start live. */
    RTSP_LIVE_STOP,                     /**< stop live */
    RTSP_PLAYBACK_RESET,                /**< reset playback */
    RTSP_PLAYBACK_PAUSE,                /**< pause playback */
    RTSP_PLAYBACK_RESUME,               /**< resume playback */
    RTSP_PLAYBACK_CONFIG,               /**< enabling playback stream */
    RTSP_PLAYBACK_GET_VID_FTIME,        /**< retrieve video time per frame */
    RTSP_PLAYBACK_GET_VID_TICK,         /**< retrieve video tick per frame */
    RTSP_PLAYBACK_GET_AUD_FTIME,        /**< retrieve audio time per frame */
    RTSP_PLAYBACK_GET_DURATION,         /**< retrieve clip duration */
    RTSP_PLAYBACK_SET_LIVE_BITRATE,     /**< set AVG bitrate */
    RTSP_PLAYBACK_GET_LIVE_BITRATE,     /**< get latest reported bitrate from BRC */
    RTSP_PLAYBACK_GET_LIVE_AVG_BITRATE, /**< get sensor setting */
    RTSP_PLAYBACK_SET_NET_BANDWIDTH,    /**< set bandwidth for BRC callback */
    RTSP_PLAYBACK_SEND_RR_STAT,         /**< send rr report */
    RTSP_PLAYBACK_SET_PARAMETER,        /**< send RTSP extend field */
    RTSP_PLAYBACK_GET_SPS_PPS           /**< get sps&pps of the specifid file or playback file */
#if defined(CONFIG_APP_FLOW_CARDV_AONI)
    ,RTSP_PLAYBACK_GSENSOR_PLAY,
    RTSP_PLAYBACK_GSENSOR_STOP,
#endif
} RTSP_PLAYBACK_OP_e;

typedef UINT32 (*SVC_RTSP_VIR2PHY_f)(ULONG VirtAddr, ULONG *pPhysAddr);
typedef UINT32 (*SVC_RTSP_AUD_HDLR2ID_f)(const UINT32 *pHdlr);


UINT32 SvcRtspLink_Create(void);
UINT32 SvcRtspLink_Command(UINT32 RecStreamId, UINT32 Cmd);
void   SvcRtspLink_SetInfo(UINT32 RecStreamId,
                           SVC_EXAMFRAMESHARE_VID_CONFIG_s  *pVidCfg,
                           SVC_EXAMFRAMESHARE_AUD_CONFIG_s  *pAudCfg,
                           SVC_RTSP_VIR2PHY_f               pVir2Phy,
                           SVC_RTSP_AUD_HDLR2ID_f           pAudHdlr2Id);
UINT32 SvcRtspLink_Notify(UINT32 RecStreamId, UINT32 Type, void *pDesc);
#if defined(CONFIG_APP_FLOW_CARDV_AONI)
UINT32 SvcRtspLink_GSensorNotify(void *pDesc);
#endif

#endif /* SVC_EXAMFRAMESHARERTSP_H */
