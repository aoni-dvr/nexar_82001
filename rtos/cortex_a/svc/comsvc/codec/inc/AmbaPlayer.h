/**
*  @file AmbaPlayer.h
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
*  @details amba player
*
*/
#ifndef AMBA_PLAYER_H
#define AMBA_PLAYER_H

/**
 * Movie Info object
 */
typedef struct {
    #define AMBA_FORMAT_MID_H264    (0x01U)             /* SVC_FORMAT_MID_xxx */
    #define AMBA_FORMAT_MID_AVC     (0x02U)
    #define AMBA_FORMAT_MID_H265    (0x04U)
    #define AMBA_FORMAT_MID_HVC     (0x05U)
    #define AMBA_FORMAT_MID_AAC     (0x21U)
    #define AMBA_FORMAT_MID_PCM     (0x22U)
    #define AMBA_FORMAT_MID_LPCM    (0x28U)
    UINT8   MediaId;
    UINT16  Width;
    UINT16  Height;
} AMBA_VIDEO_TRACK_INFO_s;

typedef struct {
    #define AMBA_MAX_VIDEO_TRACK_PER_MEDIA      (4U)        /* SVC_FORMAT_MAX_VIDEO_TRACK_PER_MEDIA */
    AMBA_VIDEO_TRACK_INFO_s  VideoTrack[AMBA_MAX_VIDEO_TRACK_PER_MEDIA];
    UINT8                    VideoTrackCount;
    UINT8                    AudioTrackCount;
    UINT8                    TextTrackCount;
} AMBA_MOVIE_INFO_s;

typedef struct {
    ULONG                       VBitsBufBase;  /* video */
    UINT32                      VBitsBufSize;
    ULONG                       ABitsBufBase;  /* audio */
    UINT32                      ABitsBufSize;
    ULONG                       TBitsBufBase;  /* text */
    UINT32                      TBitsBufSize;
    ULONG                       YuvBufBase;    /* video still dec output */
    UINT32                      YuvBufSize;
} AMBA_PLAYER_BITSBUF_INIT_s;

typedef struct {
    ULONG                       BufBase;
    UINT32                      BufSize;
    ULONG                       Rp;  /* read pointer */
    ULONG                       Wp;  /* write pointer */
} AMBA_PLAYER_BITSBUF_INFO_s;

typedef struct {
    UINT8                       VoutIdx;
    UINT8                       VoutRotateFlip;
    AMBA_DSP_WINDOW_s           VoutWindow;
    UINT32                      AspectRatioX;
    UINT32                      AspectRatioY;
    UINT32                      IsInterlace;
} AMBA_PLAYER_DISP_INIT_s;

typedef struct {
    ULONG                       BufAddr;
    UINT32                      BufSize;
} AMBA_PLAYER_BUF_s;

typedef struct {
    UINT32                      PlayerNum;
    AMBA_PLAYER_BITSBUF_INIT_s  BitsBuf[CONFIG_AMBA_PLAYER_MAX_NUM];
    UINT32                      DispNum;
    AMBA_PLAYER_DISP_INIT_s     DispInfo[AMBA_DSP_MAX_VOUT_NUM];
    AMBA_PLAYER_BUF_s           PlayerBuf;
    UINT32                      TaskPriority;
    UINT32                      TaskCpuBits;
} AMBA_PLAYER_INIT_s;

typedef struct {
    UINT32                      VoutBits;
    UINT32                      ZoomRatio;
    UINT32                      ZoomCenterX;
    UINT32                      ZoomCenterY;
    UINT32                      RotateFlip;
} AMBA_PLAYER_DISP_CTRL_s;

/* Op: 0-normal, 1-mute */
typedef void (*AMBA_PLAYER_MUTE_CTRL_t)(UINT32 Chan, UINT32 Op);

typedef struct {
    UINT32                      PlayerId;
    UINT32                      FeedType;
    UINT32                      Direction;
    UINT32                      Speed;
    UINT32                      Seamless;   /* 0: Disable, 1: Enable */
    UINT32                      Pause;      /* 1: in pause status */
    AMBA_PLAYER_DISP_CTRL_s     Disp;
    AMBA_MOVIE_INFO_s           ExpMov;
    UINT32                      LatestTime; /* ms */
    AMBA_DSP_STLDEC_STATUS_s    StillDecInfo;
    AMBA_AOUT_CTRL_t            pCbAoutCtrl;
    AMBA_PLAYER_MUTE_CTRL_t     pCbMuteCtrl;

#define AMBA_PLAYER_PRIV_SIZE   (0x6100U)
    UINT8                       PrivData[AMBA_PLAYER_PRIV_SIZE];
} AMBA_PLAYER_s;

typedef void (*AMBA_PLAYER_STATUS_NOTIFY_f)(AMBA_PLAYER_s *pPlayer, UINT32 Status, void *pParam);
#define AMBA_PLAYER_STATUS_PLAYER_START    (0x01U)
#define AMBA_PLAYER_STATUS_PLAYER_STOP     (0x02U)
#define AMBA_PLAYER_STATUS_FEED_UPDATE     (0x03U)
#define AMBA_PLAYER_STATUS_FEED_DONE       (0x04U)
#define AMBA_PLAYER_STATUS_PLAYER_EOS      (0x05U)
#define AMBA_PLAYER_STATUS_VID_FRAME_INFO  (0x06U)
#define AMBA_PLAYER_STATUS_VID_STATUS      (0x07U)

typedef struct {
    /* info from player to user */
    UINT32  FrameType;
#define AMBA_PLAYER_FRAME_TYPE_IDR    (0x01U)
#define AMBA_PLAYER_FRAME_TYPE_I      (0x02U)
#define AMBA_PLAYER_FRAME_TYPE_P      (0x03U)
#define AMBA_PLAYER_FRAME_TYPE_B      (0x04U)
#define AMBA_PLAYER_FRAME_TYPE_AUDIO  (0x05U)
#define AMBA_PLAYER_FRAME_TYPE_TEXT   (0x06U)

    ULONG   SrcBufBase;
    UINT32  SrcBufSize;
    ULONG   SrcDataAddr;
    UINT32  SrcDataSize;

    /* info back from user to player */
    UINT32  DstDataSize;
} AMBA_PLAYER_DECRYPT_s;

typedef void (*AMBA_PLAYER_DECRYPT_f)(AMBA_PLAYER_s *pPlayer, AMBA_PLAYER_DECRYPT_s *pDecrypt);
typedef struct {
    UINT32                       Enable;     /* 0: Disable, 1: Enable */
    UINT32                       TrackId;
} AMBA_PLAYER_TRACK_s;

typedef struct {
    UINT32                       FeedType;

#define AMBA_PLAYER_FTYPE_FRAME            (0x01U)
#define AMBA_PLAYER_FTYPE_MOVIE            (0x02U)

    UINT32                       Speed;

#define AMBA_PLAYER_SPEED_01X              (AMBA_DSP_VIDDEC_SPEED_01X)
#define AMBA_PLAYER_SPEED_02X              (AMBA_DSP_VIDDEC_SPEED_02X)
#define AMBA_PLAYER_SPEED_04X              (AMBA_DSP_VIDDEC_SPEED_04X)
#define AMBA_PLAYER_SPEED_08X              (AMBA_DSP_VIDDEC_SPEED_08X)
#define AMBA_PLAYER_SPEED_16X              (AMBA_DSP_VIDDEC_SPEED_16X)
#define AMBA_PLAYER_SPEED_HALF             (AMBA_DSP_VIDDEC_SPEED_1_2X)
#define AMBA_PLAYER_SPEED_FOURTH           (AMBA_DSP_VIDDEC_SPEED_1_4X)
#define AMBA_PLAYER_SPEED_EIGHTH           (AMBA_DSP_VIDDEC_SPEED_1_8X)
#define AMBA_PLAYER_SPEED_SIXTEENTH        (AMBA_DSP_VIDDEC_SPEED_1_16X)

    UINT32                       Direction;

#define AMBA_PLAYER_DIR_FORWARD            (0x0U)
#define AMBA_PLAYER_DIR_BACKWARD           (0x1U)

    UINT32                       StartTime;   /* ms */
    UINT32                       PreloadSize; /* ms, preload how many milli second data to bitsbuf */
    AMBA_PLAYER_TRACK_s          Video;
    AMBA_PLAYER_TRACK_s          Audio;
    AMBA_PLAYER_TRACK_s          Text;
    UINT32                       Seamless;    /* 0: Disable, 1: Enable */
    void                         *pStreamHdlr;
    AMBA_PLAYER_STATUS_NOTIFY_f  pFunc;       /* NULL if no need to use cb func */
    AMBA_PLAYER_DECRYPT_f        pDecryptFunc; /* NULL if no need to decrypt bitsdata */
    UINT32                       AudioCodecIndex;
} AMBA_PLAYER_CREATE_s;


UINT32 AmbaPlayer_EvalMemSize(UINT32 PlayerNum, UINT32 *pSize);
UINT32 AmbaPlayer_Init(const AMBA_PLAYER_INIT_s *pInit);

UINT32 AmbaPlayer_Parse(AMBA_PLAYER_s *pPlayer, void *pStreamHdlr, const char *FileName, UINT32 MediaType);

#define AMBA_PLAYER_MTYPE_IMAGE             (0x01U)
#define AMBA_PLAYER_MTYPE_MOVIE             (0x02U)

UINT32 AmbaPlayer_Create(AMBA_PLAYER_s *pPlayer, const AMBA_PLAYER_CREATE_s *pCreate);
UINT32 AmbaPlayer_Delete(AMBA_PLAYER_s *pPlayer);
UINT32 AmbaPlayer_Start(UINT16 PlayerNum, AMBA_PLAYER_s *pPlayerArr[]);
UINT32 AmbaPlayer_Stop(UINT16 PlayerNum, AMBA_PLAYER_s *pPlayerArr[]);

UINT32 AmbaPlayer_TrickPlay(UINT16 PlayerNum, AMBA_PLAYER_s *pPlayerArr[], UINT32 Operation);

#define AMBA_PLAYER_TRICK_PAUSE             (0x01U)
#define AMBA_PLAYER_TRICK_RESUME            (0x02U)
#define AMBA_PLAYER_TRICK_STEP              (0x03U)

UINT32 AmbaPlayer_Control(AMBA_PLAYER_s *pPlayer, UINT32 CtrlType, void *pParam);

#define AMBA_PLAYER_CTRL_INFO_GET           (0x01U)    /* output: (UINT32 *pPlayerBits) */
#define AMBA_PLAYER_CTRL_TEXT_BITSINFO_GET  (0x02U)    /* output: (AMBA_PLAYER_BITSBUF_INFO_s *pInfo) */
#define AMBA_PLAYER_CTRL_DISP_CTRL          (0x03U)    /* input:  (AMBA_PLAYER_DISP_CTRL_s *pDisp) */
#define AMBA_PLAYER_CTRL_SEAMLESS           (0x04U)    /* input:  (char *FileName) */
#define AMBA_PLAYER_CTRL_SEAMLESS           (0x04U)    /* input/output: (AMBA_PLAYER_DECRYPT_s *pDecrypt)*/

UINT32 AmbaPlayer_DecOneFrame(AMBA_PLAYER_s *pPlayer, AMBA_DSP_STLDEC_STATUS_s *pInfo);

#endif  /* AMBA_PLAYER_H */
