/**
 *  @file AmbaPlayer.c
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

#include "AmbaTypes.h"
#include "AmbaKAL.h"
#include "AmbaDSP_Event.h"
#include "AmbaDSP_VideoDec.h"
#include "AmbaDSP_StillDec_Def.h"
#include "AmbaDSP_StillDec.h"
#include "AmbaDMA_Def.h"
#include "AmbaDef.h"
#include "AmbaMisraFix.h"
#include "AmbaAudio_CODEC.h"
#include "AmbaCodecCom.h"
#include "AmbaDmux.h"
#include "AmbaBitsFifo.h"
#include "AmbaDMA_Def.h"
#include "AmbaAudio_AENC.h"
#include "AmbaAudio_AIN.h"
#include "AmbaAudio_ADEC.h"
#include "AmbaAudio_AOUT.h"
#include "AmbaAudioEfx.h"
#include "AmbaAudioBuf.h"
#include "AmbaAudioBSBuf.h"
#include "AmbaAudio.h"
#include "AmbaAudioBSBuf.h"
#include "AmbaADecFlow.h"
#include "AmbaSvcWrap.h"

#include "SvcFIFO.h"
#include "SvcStream.h"
#include "SvcFormatDef.h"
#include "SvcIso.h"
#include "SvcFormat.h"
#include "SvcDemuxer.h"
#include "DemuxerImpl.h"
#include "SvcFmp4Dmx.h"
#include "SvcMp4Dmx.h"
#include "AmbaMux_Def.h"
#include "AmbaPlayer.h"

#define AMBA_PLAYER_FLG_PRELOAD_DONE   (0x1U)
#define AMBA_PLAYER_FLG_DMX_DONE       (0x2U)
#define AMBA_PLAYER_FLG_VDEC_START     (0x4U)
#define AMBA_PLAYER_FLG_VDEC_STOP      (0x8U)
#define AMBA_PLAYER_FLG_VDEC_IDLE      (0x10U)
#define AMBA_PLAYER_FLG_VDEC_EOS       (0x20U)
#define AMBA_PLAYER_FLG_STLDEC_DONE    (0x40U)
#define AMBA_PLAYER_FLG_DMX_STOP       (0x80U)

#define AMBA_PLAYER_MSG_PRELOAD        (0x1U)
#define AMBA_PLAYER_MSG_FEED           (0x2U)
#define AMBA_PLAYER_MSG_STOP           (0x4U)

typedef struct {
    UINT32                     Event;
} AMBA_PLAYER_FEED_MSG_s;

typedef struct {
    UINT32                     Enable;
    UINT32                     TrackId;
    ULONG                      DecId;
    ULONG                      BitsBufBase;
    UINT32                     BitsBufSize;
    ULONG                      BitsBufWp;
    ULONG                      BitsBufRp;
    ULONG                      BitsBufUpdp;    /* for video track only, it will only be updated after I/P frames, no B frames */
    SVC_FIFO_HDLR_s            *pBaseFifoHdlr;
    SVC_FIFO_HDLR_s            *pVirtualFifoHdlr;
    UINT64                     StartDTS;
    UINT32                     FeedCount;
    UINT32                     DecodedCount;
} AMBA_PLAYER_TRACK_INFO_s;

typedef struct {
    UINT32                      PlayerId;
    UINT32                      FeedType;
    UINT32                      Direction;
    UINT32                      Speed;
    UINT32                      Seamless;
    UINT32                      Pause;
    AMBA_PLAYER_DISP_CTRL_s     Disp;
    AMBA_MOVIE_INFO_s           ExpMov;
    UINT32                      LatestTime;
    AMBA_DSP_STLDEC_STATUS_s    StillDecInfo;
    AMBA_AOUT_CTRL_t            pCbAoutCtrl;
    AMBA_PLAYER_MUTE_CTRL_t     pCbMuteCtrl;

    /* private */
    SVC_MOVIE_INFO_s            Mov;
    AMBA_DMUX_CTRL              DmxCtrl GNU_ALIGNED_CACHESAFE;
    AMBA_PLAYER_TRACK_INFO_s    Video;
    AMBA_PLAYER_TRACK_INFO_s    Audio;
    AMBA_PLAYER_TRACK_INFO_s    Text;
    AMBA_PLAYER_STATUS_NOTIFY_f pNotify;
    AMBA_PLAYER_DECRYPT_f       pDecryptFunc;
    UINT8                       ContainerType;
    UINT32                      StartTime;    /* in ms */
    SVC_STREAM_HDLR_s           *pStreamHdlr;
    UINT32                      MediaType;
    UINT32                      PreloadSize;  /* in ms */
    UINT64                      DispCount;
    UINT32                      AudioCodecIndex;
    UINT32                      IsEncrypt;

#define PLAYER_STACK_SIZE       (0x4000U)
    AMBA_KAL_TASK_t             Task;
    UINT8                       StackBuf[PLAYER_STACK_SIZE] GNU_ALIGNED_CACHESAFE;

#define PLAYER_MSG_NUM          (512U)
    AMBA_KAL_MSG_QUEUE_t        MsgQue;
    AMBA_PLAYER_FEED_MSG_s      MsgBuf[PLAYER_MSG_NUM];

    AMBA_KAL_EVENT_FLAG_t       Flag;
} AMBA_PLAYER_PLAYER_PRIV_s;

typedef struct {
    UINT32                      DispNum;
    AMBA_PLAYER_DISP_INIT_s     DispInfo[AMBA_DSP_MAX_VOUT_NUM];
    UINT32                      PlayerNum;
    UINT32                      PlayerBits;
    AMBA_PLAYER_PLAYER_PRIV_s   *Player[CONFIG_AMBA_PLAYER_MAX_NUM];
    AMBA_PLAYER_BITSBUF_INIT_s  BitsBuf[CONFIG_AMBA_PLAYER_MAX_NUM];
    UINT32                      TaskPriority;
    UINT32                      TaskCpuBits;
} AMBA_PLAYER_INFO_s;

static AMBA_PLAYER_INFO_s PlayerInfo GNU_SECTION_NOZEROINIT;

static UINT32 Invert_UInt32(UINT32 Input)
{
    UINT32 Rval = 0U, Err;
    UINT8  W[4];

    W[0] = (UINT8)((Input & 0xFF000000U) >> 24);
    W[1] = (UINT8)((Input & 0xFF0000U) >> 16);
    W[2] = (UINT8)((Input & 0xFF00U) >> 8);
    W[3] = (UINT8)(Input & 0xFFU);

    Err = AmbaWrap_memcpy(&Rval, W, sizeof(Rval));
    if (RECODER_OK != Err) {
        AmbaPrint_ModulePrintUInt5(AMBA_PLAYER_PRINT_MODULE_ID, "AmbaWrap_memcpy failed (%u)", Err, 0U, 0U, 0U, 0U);
    }

    return Rval;
}

static inline ULONG Alignment(ULONG Addr, ULONG Align)
{
    ULONG AlignMask = (Align - 1U);
    return ((Addr +  AlignMask) & (~AlignMask));
}

static UINT32 VDecConfig(UINT16 PlayerNum, AMBA_PLAYER_s *pPlayerArr[], UINT32 UseDefaultCfg)
{
    UINT32                             Rval = PLAYER_OK, i, Err, TrackId;
    UINT16                             DecNum = 0U;
    AMBA_DSP_VIDDEC_STREAM_CONFIG_s    VDecCfg[AMBA_DSP_MAX_DEC_STREAM_NUM] = {0U}, *pCfg;
    const SVC_VIDEO_TRACK_INFO_s       *pTrack;
    const AMBA_PLAYER_PLAYER_PRIV_s    *pPlayerPriv;

    AmbaMisra_TouchUnused(pPlayerArr);
    AmbaMisra_TouchUnused(&Rval);

    for (i = 0U; i < PlayerNum; i++) {
        pCfg = &(VDecCfg[DecNum]);
        Err = AmbaWrap_memset(pCfg, 0, sizeof(AMBA_DSP_VIDDEC_STREAM_CONFIG_s));
        if (Err != 0U) {
            AmbaPrint_ModulePrintUInt5(AMBA_PLAYER_PRINT_MODULE_ID, "AmbaWrap_memset failed (%u)", Err, 0U, 0U, 0U, 0U);
        }

        if (UseDefaultCfg == 1U) {
            pCfg->StreamID             = (UINT16)i;
            pCfg->BitsBufAddr          = PlayerInfo.BitsBuf[0U].VBitsBufBase;
            pCfg->BitsBufSize          = PlayerInfo.BitsBuf[0U].VBitsBufSize;
            pCfg->BitsFormat           = AMBA_DSP_DEC_BITS_FORMAT_H264;
            pCfg->MaxFrameWidth        = 3840U;
            pCfg->MaxFrameHeight       = 2160U;
            pCfg->MaxVideoBufferWidth  = 3840U;
            pCfg->MaxVideoBufferHeight = 2160U;
        } else {
            AmbaMisra_TypeCast(&pPlayerPriv, &(pPlayerArr[i]));

            pCfg->StreamID             = (UINT16)pPlayerPriv->Video.DecId;
            pCfg->BitsBufAddr          = pPlayerPriv->Video.BitsBufBase;
            pCfg->BitsBufSize          = pPlayerPriv->Video.BitsBufSize;

            TrackId = pPlayerPriv->Video.TrackId;
            pTrack  = &(pPlayerPriv->Mov.VideoTrack[TrackId]);

            if ((pTrack->Info.MediaId == SVC_FORMAT_MID_H264) || (pTrack->Info.MediaId == SVC_FORMAT_MID_AVC)) {
                pCfg->BitsFormat = AMBA_DSP_DEC_BITS_FORMAT_H264;
            } else if ((pTrack->Info.MediaId == SVC_FORMAT_MID_H265) || (pTrack->Info.MediaId == SVC_FORMAT_MID_HVC)){
                pCfg->BitsFormat = AMBA_DSP_DEC_BITS_FORMAT_H265;
            } else if (pTrack->Info.MediaId == SVC_FORMAT_MID_MJPEG){
                pCfg->BitsFormat = AMBA_DSP_DEC_BITS_FORMAT_JPEG;
            } else {
                AmbaPrint_ModulePrintUInt5(AMBA_PLAYER_PRINT_MODULE_ID, "Unknown video format (%u)", pTrack->Info.MediaId, 0U, 0U, 0U, 0U);
            }

            pCfg->MaxFrameWidth        = pTrack->Width;
            pCfg->MaxFrameHeight       = pTrack->Height;
            pCfg->MaxVideoBufferWidth  = pTrack->Width;
            pCfg->MaxVideoBufferHeight = pTrack->Height;
        }
        DecNum++;
    }

    if ((Rval == PLAYER_OK) && (DecNum > 0U)) {
        Err = AmbaDSP_VideoDecConfig(DecNum, VDecCfg);
        if (Err != 0U) {
            AmbaPrint_ModulePrintUInt5(AMBA_PLAYER_PRINT_MODULE_ID, "AmbaDSP_VideoDecConfig failed (%u)", Err, 0U, 0U, 0U, 0U);
            Rval = PLAYER_ERROR_GENERAL_ERROR;
        }
    }

    return Rval;
}

static void* FeedTask(void* EntryArg)
{
#define FEED_NUM      (4U)
    UINT32                        Err, LoopU = 1U, Flags = 0U, PreloadDone = 0U, PreloadThr = 0U;
    ULONG                         AStartAddr = 0U, TmpAddr, TmpUL;
    UINT8                         Event = SVC_DEMUXER_EVENT_NONE, Frame = 0U;
    AMBA_PLAYER_FEED_MSG_s        Msg;
    AMBA_PLAYER_PLAYER_PRIV_s     *pPlayerPriv;
    AMBA_DSP_VIDDEC_BITS_FIFO_s   VUpdate = {0U};

    AmbaMisra_TouchUnused(&Msg);
    AmbaMisra_TouchUnused(&LoopU);
    AmbaMisra_TouchUnused(EntryArg);
    AmbaMisra_TypeCast(&pPlayerPriv, &EntryArg);
    AmbaMisra_TouchUnused(&AStartAddr);
    AmbaMisra_TouchUnused(&TmpUL);

    while (LoopU == 1U) {
        Err = AmbaKAL_MsgQueueReceive(&(pPlayerPriv->MsgQue), &Msg, AMBA_KAL_WAIT_FOREVER);
        if (Err != KAL_ERR_NONE) {
            AmbaPrint_ModulePrintUInt5(AMBA_PLAYER_PRINT_MODULE_ID, "AmbaKAL_MsgQueueReceive failed (%u)", Err, 0U, 0U, 0U, 0U);
            LoopU = 0U;
        }

        if (Err == PLAYER_OK) {
            if (Msg.Event == AMBA_PLAYER_MSG_PRELOAD) {
                Frame       = FEED_NUM;
                PreloadDone = 0U;
                PreloadThr  = (pPlayerPriv->PreloadSize * pPlayerPriv->Mov.VideoTrack[pPlayerPriv->Video.TrackId].Info.TimeScale);
                PreloadThr /= (1000U * pPlayerPriv->Mov.VideoTrack[pPlayerPriv->Video.TrackId].Info.TimePerFrame);

                /* update audio feed pointer */
                if (pPlayerPriv->Audio.Enable == 1U) {
                    AStartAddr = pPlayerPriv->Audio.BitsBufWp;
                }
            } else if (Msg.Event == AMBA_PLAYER_MSG_FEED) {
                Frame       = FEED_NUM;
                PreloadDone = 1U;

                /* update video feed pointer */
                if (pPlayerPriv->Video.Enable == 1U) {
                    VUpdate.StartAddr = pPlayerPriv->Video.BitsBufUpdp;
                }

                /* update audio feed pointer */
                if (pPlayerPriv->Audio.Enable == 1U) {
                    AStartAddr = pPlayerPriv->Audio.BitsBufWp;
                }
            } else if (Msg.Event == AMBA_PLAYER_MSG_STOP) {
                Frame       = 1U;
                PreloadDone = 1U;

            } else {
                Err = PLAYER_ERROR_GENERAL_ERROR;
                AmbaPrint_ModulePrintUInt5(AMBA_PLAYER_PRINT_MODULE_ID, "Unknown msg event (%u)", Msg.Event, 0U, 0U, 0U, 0U);
            }
        }

        do {
            TmpAddr = pPlayerPriv->Video.BitsBufWp;

            if (Err == PLAYER_OK) {
                Err = AmbaDmux_Process(&(pPlayerPriv->DmxCtrl), Frame, &Event);
                if (Err != PLAYER_OK) {
                    AmbaPrint_ModulePrintUInt5(AMBA_PLAYER_PRINT_MODULE_ID, "AmbaDmux_Process failed (%u)", Err, 0U, 0U, 0U, 0U);
                }
            }

            /* break loop when feed size >= PreloadThr */
            if (PreloadDone == 0U) {
                if (TmpAddr == pPlayerPriv->Video.BitsBufWp) {
                    /* buffer is full */
                    AmbaPrint_ModulePrintUInt5(AMBA_PLAYER_PRINT_MODULE_ID, "Buffer full", 0U, 0U, 0U, 0U, 0U);
                    PreloadDone = 1U;
                }

                if ((pPlayerPriv->Video.FeedCount >= PreloadThr) ||
                    (Event == SVC_DEMUXER_EVENT_REACH_END)) {
                    PreloadDone = 1U;
                }
            }
        } while ((Err == PLAYER_OK) && (PreloadDone == 0U));

        if (Err == PLAYER_OK) {
            Flags = 0U;
            Err = AmbaKAL_EventFlagGet(&(pPlayerPriv->Flag), 0xFFFFFFFFU, AMBA_KAL_FLAGS_ANY,
                                       AMBA_KAL_FLAGS_CLEAR_NONE, &Flags, AMBA_KAL_NO_WAIT);
            if (Err != KAL_ERR_NONE) {
                /* nothing */
                Err = PLAYER_OK;
            }
        }

        if (Err == PLAYER_OK) {
            if (Msg.Event == AMBA_PLAYER_MSG_PRELOAD) {
                Err = AmbaKAL_EventFlagSet(&(pPlayerPriv->Flag), AMBA_PLAYER_FLG_PRELOAD_DONE);
                if (Err != KAL_ERR_NONE) {
                    AmbaPrint_ModulePrintUInt5(AMBA_PLAYER_PRINT_MODULE_ID, "AmbaKAL_EventFlagSet failed (%u)", Err, 0U, 0U, 0U, 0U);
                } else {
                    AmbaPrint_ModulePrintUInt5(AMBA_PLAYER_PRINT_MODULE_ID, "Player (%u) preload done!!", pPlayerPriv->PlayerId, 0U, 0U, 0U, 0U);
                }

                /* video doesn't need to update bitsinfo at preload stage */
                if (pPlayerPriv->Video.Enable == 1U) {
                    AmbaPrint_ModulePrintUInt5(AMBA_PLAYER_PRINT_MODULE_ID, "VPreload StartAddr 0x%x, Size %u, Num %u", (UINT32)pPlayerPriv->Video.BitsBufBase, (UINT32)pPlayerPriv->Video.BitsBufUpdp - (UINT32)pPlayerPriv->Video.BitsBufBase, pPlayerPriv->Video.FeedCount, 0U, 0U);

                    {
                        ULONG  AlignStart, AlignSize;

                        AlignStart = pPlayerPriv->Video.BitsBufBase & AMBA_CACHE_LINE_MASK;
                        AlignSize  = Alignment(pPlayerPriv->Video.BitsBufUpdp - 1U, AMBA_CACHE_LINE_SIZE) - AlignStart;
                        Err = AmbaSvcWrap_CacheClean(AlignStart, AlignSize);
                        if (Err != PLAYER_OK) {
                            AmbaPrint_ModulePrintUInt5(AMBA_PLAYER_PRINT_MODULE_ID, "AmbaSvcWrap_CacheClean failed (%u)", Err, 0U, 0U, 0U, 0U);
                        }
                    }
                }

#if defined(CONFIG_BUILD_COMAMBA_CODEC_AUDIO)
                /* audio update bits buff info */
                if (pPlayerPriv->Audio.Enable == 1U) {
                    if (AStartAddr != pPlayerPriv->Audio.BitsBufWp) {
                        UINT32 Size;
                        UINT8  *pAddr;

                        AmbaMisra_TypeCast(&pAddr, &AStartAddr);

                        if (pPlayerPriv->Audio.BitsBufWp > AStartAddr) {
                            TmpUL = pPlayerPriv->Audio.BitsBufWp - AStartAddr;
                            Size  =  (UINT32)TmpUL;
                        } else {
                            TmpUL = pPlayerPriv->Audio.BitsBufSize - (AStartAddr - pPlayerPriv->Audio.BitsBufWp);
                            Size  = (UINT32)TmpUL;
                        }

                        Err = AmbaADecFlow_UpdateBsBufRp(pPlayerPriv->Audio.DecId, pAddr, Size);
                        if (Err != PLAYER_OK) {
                            AmbaPrint_ModulePrintUInt5(AMBA_PLAYER_PRINT_MODULE_ID, "AmbaADecFlow_UpdateBsBufRp failed!", Err, 0U, 0U, 0U, 0U);
                        }

                        AmbaPrint_ModulePrintUInt5(AMBA_PLAYER_PRINT_MODULE_ID, "APreloadDataSize (%u)", Size, 0U, 0U, 0U, 0U);
                    }
                }
#endif
            } else if (Msg.Event == AMBA_PLAYER_MSG_FEED) {
                /* video update bits buff info */
                if (pPlayerPriv->Video.Enable == 1U) {
                    if ((0U < CheckBits(Flags, AMBA_PLAYER_FLG_VDEC_START)) && (0U == CheckBits(Flags, AMBA_PLAYER_FLG_VDEC_STOP))) {
                        if (VUpdate.StartAddr != pPlayerPriv->Video.BitsBufUpdp) {
                            UINT16 VDecId = (UINT16)pPlayerPriv->Video.DecId;

                            if (pPlayerPriv->Video.BitsBufUpdp == pPlayerPriv->Video.BitsBufBase) {
                                VUpdate.EndAddr = pPlayerPriv->Video.BitsBufBase + pPlayerPriv->Video.BitsBufSize - 1U;
                            } else {
                                VUpdate.EndAddr = pPlayerPriv->Video.BitsBufUpdp - 1U;
                            }

                            {
                                ULONG  AlignStart, AlignSize;

                                if (VUpdate.StartAddr < VUpdate.EndAddr) {
                                    AlignStart = VUpdate.StartAddr & AMBA_CACHE_LINE_MASK;
                                    AlignSize  =  Alignment(VUpdate.EndAddr, AMBA_CACHE_LINE_SIZE) - AlignStart;
                                    Err = AmbaSvcWrap_CacheClean(AlignStart, AlignSize);
                                    if (Err != PLAYER_OK) {
                                        AmbaPrint_ModulePrintUInt5(AMBA_PLAYER_PRINT_MODULE_ID, "AmbaSvcWrap_CacheClean failed (%u)", Err, 0U, 0U, 0U, 0U);
                                    }
                                } else {
                                    AlignStart = VUpdate.StartAddr & AMBA_CACHE_LINE_MASK;
                                    AlignSize  =  Alignment((pPlayerPriv->Video.BitsBufBase + pPlayerPriv->Video.BitsBufSize), AMBA_CACHE_LINE_SIZE) - AlignStart;
                                    Err = AmbaSvcWrap_CacheClean(AlignStart, AlignSize);
                                    if (Err != PLAYER_OK) {
                                        AmbaPrint_ModulePrintUInt5(AMBA_PLAYER_PRINT_MODULE_ID, "AmbaSvcWrap_CacheClean failed (%u)", Err, 0U, 0U, 0U, 0U);
                                    }

                                    AlignStart = pPlayerPriv->Video.BitsBufBase & AMBA_CACHE_LINE_MASK;
                                    AlignSize  =  Alignment(VUpdate.EndAddr, AMBA_CACHE_LINE_SIZE) - AlignStart;
                                    Err = AmbaSvcWrap_CacheClean(AlignStart, AlignSize);
                                    if (Err != PLAYER_OK) {
                                        AmbaPrint_ModulePrintUInt5(AMBA_PLAYER_PRINT_MODULE_ID, "AmbaSvcWrap_CacheClean failed (%u)", Err, 0U, 0U, 0U, 0U);
                                    }
                                }
                            }

                            Err = AmbaDSP_VideoDecBitsFifoUpdate(1U, &VDecId, &VUpdate);
                            if (Err != PLAYER_OK) {
                                AmbaPrint_ModulePrintUInt5(AMBA_PLAYER_PRINT_MODULE_ID, "AmbaDSP_VideoDecBitsFifoUpdate failed!", Err, 0U, 0U, 0U, 0U);
                            }
                        }
                    }
                }

#if defined(CONFIG_BUILD_COMAMBA_CODEC_AUDIO)
                /* audio update bits buff info */
                if (pPlayerPriv->Audio.Enable == 1U) {
                    if (AStartAddr != pPlayerPriv->Audio.BitsBufWp) {
                        UINT32 Size;
                        UINT8  *pAddr;

                        AmbaMisra_TypeCast(&pAddr, &AStartAddr);

                        if (pPlayerPriv->Audio.BitsBufWp > AStartAddr) {
                            TmpUL = pPlayerPriv->Audio.BitsBufWp - AStartAddr;
                            Size  = (UINT32)TmpUL;
                        } else {
                            TmpUL = pPlayerPriv->Audio.BitsBufSize - (AStartAddr - pPlayerPriv->Audio.BitsBufWp);
                            Size  = (UINT32)TmpUL;
                        }

                        {
                            ULONG  AlignStart, AlignSize;


                            if (pPlayerPriv->Audio.BitsBufWp > AStartAddr) {
                                AlignStart = AStartAddr & AMBA_CACHE_LINE_MASK;
                                AlignSize  =  Alignment(pPlayerPriv->Audio.BitsBufWp, AMBA_CACHE_LINE_SIZE) - AlignStart;
                                Err = AmbaSvcWrap_CacheClean(AlignStart, AlignSize);
                                if (Err != PLAYER_OK) {
                                    AmbaPrint_ModulePrintUInt5(AMBA_PLAYER_PRINT_MODULE_ID, "AmbaSvcWrap_CacheClean failed (%u)", Err, 0U, 0U, 0U, 0U);
                                }
                            } else {
                                AlignStart = AStartAddr & AMBA_CACHE_LINE_MASK;
                                AlignSize  =  Alignment((pPlayerPriv->Audio.BitsBufBase + pPlayerPriv->Audio.BitsBufSize), AMBA_CACHE_LINE_SIZE) - AlignStart;
                                Err = AmbaSvcWrap_CacheClean(AlignStart, AlignSize);
                                if (Err != PLAYER_OK) {
                                    AmbaPrint_ModulePrintUInt5(AMBA_PLAYER_PRINT_MODULE_ID, "AmbaSvcWrap_CacheClean failed (%u)", Err, 0U, 0U, 0U, 0U);
                                }

                                AlignStart = pPlayerPriv->Audio.BitsBufBase & AMBA_CACHE_LINE_MASK;
                                AlignSize  =  Alignment(pPlayerPriv->Audio.BitsBufWp, AMBA_CACHE_LINE_SIZE) - AlignStart;
                                Err = AmbaSvcWrap_CacheClean(AlignStart, AlignSize);
                                if (Err != PLAYER_OK) {
                                    AmbaPrint_ModulePrintUInt5(AMBA_PLAYER_PRINT_MODULE_ID, "AmbaSvcWrap_CacheClean failed (%u)", Err, 0U, 0U, 0U, 0U);
                                }
                            }
                        }

                        Err = AmbaADecFlow_UpdateBsBufRp(pPlayerPriv->Audio.DecId, pAddr, Size);
                        if (Err != PLAYER_OK) {
                            AmbaPrint_ModulePrintUInt5(AMBA_PLAYER_PRINT_MODULE_ID, "AmbaADecFlow_UpdateBsBufRp failed!", Err, 0U, 0U, 0U, 0U);
                        }
                    }
                }
#endif
            } else if (Msg.Event == AMBA_PLAYER_MSG_STOP) {
                Err = AmbaKAL_EventFlagSet(&(pPlayerPriv->Flag), AMBA_PLAYER_FLG_DMX_STOP);
                if (Err != KAL_ERR_NONE) {
                    AmbaPrint_ModulePrintUInt5(AMBA_PLAYER_PRINT_MODULE_ID, "AmbaKAL_EventFlagSet failed (%u)", Err, 0U, 0U, 0U, 0U);
                } else {
                    AmbaPrint_ModulePrintUInt5(AMBA_PLAYER_PRINT_MODULE_ID, "Demuxer stop", 0U, 0U, 0U, 0U, 0U);
                }
            } else {
                /**/
            }

            /* do notification after each feed done */
            if (pPlayerPriv->pNotify != NULL) {
                AMBA_PLAYER_s *pPlayer;

                AmbaMisra_TypeCast(&pPlayer, &pPlayerPriv);
                pPlayerPriv->pNotify(pPlayer, AMBA_PLAYER_STATUS_FEED_UPDATE, NULL);
            }
        }

        if (Err == PLAYER_OK) {
            /* stop feed if: 1. all frames are fed  2. video decoder stop */
            if ((Event == SVC_DEMUXER_EVENT_REACH_END) ||
                (0U < CheckBits(Flags, AMBA_PLAYER_FLG_VDEC_STOP)) ||
                ((pPlayerPriv->FeedType == AMBA_PLAYER_FTYPE_FRAME) && (Msg.Event == AMBA_PLAYER_MSG_STOP))) {
                if (0U == CheckBits(Flags, AMBA_PLAYER_FLG_DMX_DONE)) {
                    Err = AmbaKAL_EventFlagSet(&(pPlayerPriv->Flag), AMBA_PLAYER_FLG_DMX_DONE);
                    if (Err != KAL_ERR_NONE) {
                        AmbaPrint_ModulePrintUInt5(AMBA_PLAYER_PRINT_MODULE_ID, "AmbaKAL_EventFlagSet failed (%u)", Err, 0U, 0U, 0U, 0U);
                    } else {
                        AmbaPrint_ModulePrintUInt5(AMBA_PLAYER_PRINT_MODULE_ID, "Demuxer reach end, video total feed %u", pPlayerPriv->Video.FeedCount, 0U, 0U, 0U, 0U);
                    }

                    /* do feed done notification if all frames are already fed */
                    if ((pPlayerPriv->pNotify != NULL) && (0U == CheckBits(Flags, AMBA_PLAYER_FLG_VDEC_STOP))) {
                        AMBA_PLAYER_s *pPlayer;

                        AmbaMisra_TypeCast(&pPlayer, &pPlayerPriv);
                        pPlayerPriv->pNotify(pPlayer, AMBA_PLAYER_STATUS_FEED_DONE, &(pPlayerPriv->Video.FeedCount));
                    }
                }
            }
        }
    }

    return NULL;
}

static void VideoEndOfStream(AMBA_PLAYER_PLAYER_PRIV_s *pPlayerPriv)
{
#define HEVC_EOSTR_SIZE (6U)
#define AVC_EOSTR_SIZE  (5U)
    UINT8          HevcEos[HEVC_EOSTR_SIZE] = {0x00, 0x00, 0x00, 0x01, 0x4A, 0x00};
    UINT8          AvcEos[AVC_EOSTR_SIZE]   = {0x00, 0x00, 0x00, 0x01, 0x0B};
    UINT8          *pEos = NULL;
    UINT8          *pWrite;
    ULONG          EosSize = 0U, RSize, WSize;
    UINT32         Rval = PLAYER_OK, Err;

    if ((pPlayerPriv->Mov.VideoTrack[pPlayerPriv->Video.TrackId].Info.MediaId == SVC_FORMAT_MID_H265) ||
        (pPlayerPriv->Mov.VideoTrack[pPlayerPriv->Video.TrackId].Info.MediaId == SVC_FORMAT_MID_HVC)) {

        pEos    = HevcEos;
        EosSize = HEVC_EOSTR_SIZE;
    } else if ((pPlayerPriv->Mov.VideoTrack[pPlayerPriv->Video.TrackId].Info.MediaId == SVC_FORMAT_MID_H264) ||
               (pPlayerPriv->Mov.VideoTrack[pPlayerPriv->Video.TrackId].Info.MediaId == SVC_FORMAT_MID_AVC)) {

        pEos    = AvcEos;
        EosSize = AVC_EOSTR_SIZE;
    } else {
        AmbaPrint_ModulePrintUInt5(AMBA_PLAYER_PRINT_MODULE_ID, "Unknown MediaId (%u)", pPlayerPriv->Mov.VideoTrack[pPlayerPriv->Video.TrackId].Info.MediaId, 0U, 0U, 0U, 0U);
        Rval = PLAYER_ERROR_GENERAL_ERROR;
    }

    AmbaMisra_TouchUnused(pEos);
    AmbaMisra_TouchUnused(&pEos);

    if ((Rval == PLAYER_OK) && (pEos != NULL)) {
        if ((pPlayerPriv->Video.BitsBufUpdp + EosSize) < (pPlayerPriv->Video.BitsBufBase + pPlayerPriv->Video.BitsBufSize)) {
            AmbaMisra_TypeCast(&pWrite, &(pPlayerPriv->Video.BitsBufUpdp));
            Err = AmbaWrap_memcpy(pWrite, pEos, EosSize);
            if (Err != PLAYER_OK) {
                AmbaPrint_ModulePrintUInt5(AMBA_PLAYER_PRINT_MODULE_ID, "AmbaWrap_memcpy failed (%u)", Err, 0U, 0U, 0U, 0U);
            }
            pPlayerPriv->Video.BitsBufUpdp += EosSize;
        } else {
            WSize = (pPlayerPriv->Video.BitsBufBase + pPlayerPriv->Video.BitsBufSize) - pPlayerPriv->Video.BitsBufUpdp;
            RSize = EosSize - WSize;

            AmbaMisra_TypeCast(&pWrite, &(pPlayerPriv->Video.BitsBufUpdp));
            Err = AmbaWrap_memcpy(pWrite, pEos, WSize);
            if (Err != PLAYER_OK) {
                AmbaPrint_ModulePrintUInt5(AMBA_PLAYER_PRINT_MODULE_ID, "AmbaWrap_memcpy failed (%u)", Err, 0U, 0U, 0U, 0U);
            }
            pPlayerPriv->Video.BitsBufUpdp = pPlayerPriv->Video.BitsBufBase;

            AmbaMisra_TypeCast(&pWrite, &(pPlayerPriv->Video.BitsBufUpdp));
            Err = AmbaWrap_memcpy(pWrite, &(pEos[WSize]), RSize);
            if (Err != PLAYER_OK) {
                AmbaPrint_ModulePrintUInt5(AMBA_PLAYER_PRINT_MODULE_ID, "AmbaWrap_memcpy failed (%u)", Err, 0U, 0U, 0U, 0U);
            }
            pPlayerPriv->Video.BitsBufUpdp += RSize;
        }
    }

    AmbaPrint_ModulePrintUInt5(AMBA_PLAYER_PRINT_MODULE_ID, "Video end of stream feed done", 0U, 0U, 0U, 0U, 0U);
}

static void VideoEndOfSequence(AMBA_PLAYER_PLAYER_PRIV_s *pPlayerPriv)
{
#define HEVC_EOSE_SIZE (5U)
#define AVC_EOSE_SIZE  (5U)
    UINT8          HevcEos[HEVC_EOSE_SIZE] = {0x00, 0x00, 0x01, 0x48, 0x00};
    UINT8          AvcEos[AVC_EOSE_SIZE]   = {0x00, 0x00, 0x00, 0x01, 0x0A};
    UINT8          *pEos = NULL;
    UINT8          *pWrite;
    ULONG          EosSize = 0U, RSize, WSize;
    UINT32         Rval = PLAYER_OK, Err;

    if ((pPlayerPriv->Mov.VideoTrack[pPlayerPriv->Video.TrackId].Info.MediaId == SVC_FORMAT_MID_H265) ||
        (pPlayerPriv->Mov.VideoTrack[pPlayerPriv->Video.TrackId].Info.MediaId == SVC_FORMAT_MID_HVC)) {

        pEos    = HevcEos;
        EosSize = HEVC_EOSE_SIZE;
    } else if ((pPlayerPriv->Mov.VideoTrack[pPlayerPriv->Video.TrackId].Info.MediaId == SVC_FORMAT_MID_H264) ||
               (pPlayerPriv->Mov.VideoTrack[pPlayerPriv->Video.TrackId].Info.MediaId == SVC_FORMAT_MID_AVC)) {

        pEos    = AvcEos;
        EosSize = AVC_EOSE_SIZE;
    } else {
        AmbaPrint_ModulePrintUInt5(AMBA_PLAYER_PRINT_MODULE_ID, "Unknown MediaId (%u)", pPlayerPriv->Mov.VideoTrack[pPlayerPriv->Video.TrackId].Info.MediaId, 0U, 0U, 0U, 0U);
        Rval = PLAYER_ERROR_GENERAL_ERROR;
    }

    AmbaMisra_TouchUnused(pEos);
    AmbaMisra_TouchUnused(&pEos);

    if ((Rval == PLAYER_OK) && (pEos != NULL)) {
        if ((pPlayerPriv->Video.BitsBufUpdp + EosSize) < (pPlayerPriv->Video.BitsBufBase + pPlayerPriv->Video.BitsBufSize)) {
            AmbaMisra_TypeCast(&pWrite, &(pPlayerPriv->Video.BitsBufUpdp));
            Err = AmbaWrap_memcpy(pWrite, pEos, EosSize);
            if (Err != PLAYER_OK) {
                AmbaPrint_ModulePrintUInt5(AMBA_PLAYER_PRINT_MODULE_ID, "AmbaWrap_memcpy failed (%u)", Err, 0U, 0U, 0U, 0U);
            }
            pPlayerPriv->Video.BitsBufUpdp += EosSize;
        } else {
            WSize = (pPlayerPriv->Video.BitsBufBase + pPlayerPriv->Video.BitsBufSize) - pPlayerPriv->Video.BitsBufUpdp;
            RSize = EosSize - WSize;

            AmbaMisra_TypeCast(&pWrite, &(pPlayerPriv->Video.BitsBufUpdp));
            Err = AmbaWrap_memcpy(pWrite, pEos, WSize);
            if (Err != PLAYER_OK) {
                AmbaPrint_ModulePrintUInt5(AMBA_PLAYER_PRINT_MODULE_ID, "AmbaWrap_memcpy failed (%u)", Err, 0U, 0U, 0U, 0U);
            }
            pPlayerPriv->Video.BitsBufUpdp = pPlayerPriv->Video.BitsBufBase;

            AmbaMisra_TypeCast(&pWrite, &(pPlayerPriv->Video.BitsBufUpdp));
            Err = AmbaWrap_memcpy(pWrite, &(pEos[WSize]), RSize);
            if (Err != PLAYER_OK) {
                AmbaPrint_ModulePrintUInt5(AMBA_PLAYER_PRINT_MODULE_ID, "AmbaWrap_memcpy failed (%u)", Err, 0U, 0U, 0U, 0U);
            }
            pPlayerPriv->Video.BitsBufUpdp += RSize;
        }
    }
}

static UINT32 VideoBitsFifoCB(const SVC_FIFO_HDLR_s *pHdlr, UINT8 Event, SVC_FIFO_CALLBACK_INFO_s *Info)
{
    UINT32                       Rval = PLAYER_ERROR_GENERAL_ERROR, Err, i, IsBase = 0U;
    ULONG                        TmpUL;
    AMBA_PLAYER_PLAYER_PRIV_s    *pPlayerPriv;

    AmbaMisra_TouchUnused(&pPlayerPriv);
    AmbaMisra_TouchUnused(Info);

    for (i = 0U; i < PlayerInfo.PlayerNum; i++) {
        if (PlayerInfo.Player[i] != NULL) {
            if (PlayerInfo.Player[i]->Video.pBaseFifoHdlr == pHdlr) {
                pPlayerPriv = PlayerInfo.Player[i];
                IsBase = 1U;
                Rval   = PLAYER_OK;
            }

            if (PlayerInfo.Player[i]->Video.pVirtualFifoHdlr == pHdlr) {
                pPlayerPriv = PlayerInfo.Player[i];
                Rval   = PLAYER_OK;
            }
        }
    }

    if ((Rval == PLAYER_OK) && (IsBase == 1U)) {
        switch (Event) {
            case SVC_FIFO_EVENT_GET_WRITE_POINT:
                /* tell demuxer bitsbuf write pointer and free space size */
                AmbaMisra_TypeCast(&(Info->Desc->StartAddr), &(pPlayerPriv->Video.BitsBufWp));
                if (pPlayerPriv->Video.BitsBufWp >= pPlayerPriv->Video.BitsBufRp) {
                    TmpUL             = pPlayerPriv->Video.BitsBufSize + pPlayerPriv->Video.BitsBufRp - pPlayerPriv->Video.BitsBufWp;
                    Info->Desc->Size  = (UINT32)TmpUL;
                } else {
                    TmpUL             = pPlayerPriv->Video.BitsBufRp - pPlayerPriv->Video.BitsBufWp;
                    Info->Desc->Size  = (UINT32)TmpUL;
                }
                break;
            case SVC_FIFO_EVENT_DATA_READY:
                if (Info->Desc->FrameType == SVC_FIFO_TYPE_EOS) {
                    /* end of stream */
                    if (pPlayerPriv->Seamless == 0U) {
                        VideoEndOfStream(pPlayerPriv);
                    }
                } else if (Info->Desc->FrameType == SVC_FIFO_TYPE_DECODE_MARK) {
                    /* end of sequence */
                    VideoEndOfSequence(pPlayerPriv);
                } else if ((Info->Desc->FrameType == SVC_FIFO_TYPE_IDR_FRAME) ||
                           (Info->Desc->FrameType == SVC_FIFO_TYPE_I_FRAME)   ||
                           (Info->Desc->FrameType == SVC_FIFO_TYPE_P_FRAME)   ||
                           (Info->Desc->FrameType == SVC_FIFO_TYPE_B_FRAME)) {
                    ULONG StartAddr;

                    AmbaMisra_TypeCast(&(StartAddr), &(Info->Desc->StartAddr));

                    /* decrypt the bitstream data */
                    if ((pPlayerPriv->pDecryptFunc != NULL) && (pPlayerPriv->IsEncrypt == 1U)) {
                        AMBA_PLAYER_DECRYPT_s Decrypt = {0};
                        AMBA_PLAYER_s         *pPlayer;

                        AmbaMisra_TypeCast(&pPlayer, &pPlayerPriv);

                        if (Info->Desc->FrameType == SVC_FIFO_TYPE_IDR_FRAME) {
                            Decrypt.FrameType = AMBA_PLAYER_FRAME_TYPE_IDR;
                        } else if (Info->Desc->FrameType == SVC_FIFO_TYPE_I_FRAME) {
                            Decrypt.FrameType = AMBA_PLAYER_FRAME_TYPE_I;
                        } else if (Info->Desc->FrameType == SVC_FIFO_TYPE_P_FRAME) {
                            Decrypt.FrameType = AMBA_PLAYER_FRAME_TYPE_P;
                        } else {
                            Decrypt.FrameType = AMBA_PLAYER_FRAME_TYPE_B;
                        }

                        Decrypt.SrcBufBase   = pPlayerPriv->Video.BitsBufBase;
                        Decrypt.SrcBufSize   = pPlayerPriv->Video.BitsBufSize;
                        Decrypt.SrcDataAddr  = StartAddr;
                        Decrypt.SrcDataSize  = Info->Desc->Size;

                        pPlayerPriv->pDecryptFunc(pPlayer, &Decrypt);

                        Info->Desc->Size = Decrypt.DstDataSize;
                    }

                    /* update bitsbuf write pointer */
                    if ((StartAddr + Info->Desc->Size) < (pPlayerPriv->Video.BitsBufBase + pPlayerPriv->Video.BitsBufSize)) {
                        pPlayerPriv->Video.BitsBufWp = StartAddr + Info->Desc->Size;
                    } else {
                        pPlayerPriv->Video.BitsBufWp = (StartAddr + Info->Desc->Size) - pPlayerPriv->Video.BitsBufSize;
                    }

                    /* update pointer pointer for notifying DSP */
                    if (pPlayerPriv->Mov.VideoTrack[pPlayerPriv->Video.TrackId].M == 1U) {
                        pPlayerPriv->Video.BitsBufUpdp = pPlayerPriv->Video.BitsBufWp;
                    } else {
                        /* if M > 0, only update the pointer after all the b frames are fed
                           e.g., M = 3:    1. PBB => OK,
                                           2. PB  => Not completed                          */
                        if ((pPlayerPriv->Video.FeedCount % (UINT32)pPlayerPriv->Mov.VideoTrack[pPlayerPriv->Video.TrackId].M) == 0U) {
                            pPlayerPriv->Video.BitsBufUpdp = pPlayerPriv->Video.BitsBufWp;
                        }
                    }

                    pPlayerPriv->Video.FeedCount ++;
                } else {
                    AmbaPrint_ModulePrintUInt5(AMBA_PLAYER_PRINT_MODULE_ID, "Unknown frame type (%u)", Info->Desc->FrameType, 0U, 0U, 0U, 0U);
                }

                Err = SvcFIFO_RemoveEntry(pHdlr, 1U);
                if (Err != PLAYER_OK) {
                    AmbaPrint_ModulePrintUInt5(AMBA_PLAYER_PRINT_MODULE_ID, "SvcFIFO_RemoveEntry failed (%u)", Err, 0U, 0U, 0U, 0U);
                }
                break;
            default:
                /* nothing */
                break;
        }
    }

    return Rval;
}

static UINT32 AudioBitsFifoCB(const SVC_FIFO_HDLR_s *pHdlr, UINT8 Event, SVC_FIFO_CALLBACK_INFO_s *Info)
{
    UINT32                       Rval = PLAYER_ERROR_GENERAL_ERROR, Err, i, IsBase = 0U;
    ULONG                        TmpUL;
    AMBA_PLAYER_PLAYER_PRIV_s    *pPlayerPriv;

    AmbaMisra_TouchUnused(&pPlayerPriv);
    AmbaMisra_TouchUnused(Info);

    for (i = 0U; i < PlayerInfo.PlayerNum; i++) {
        if (PlayerInfo.Player[i] != NULL) {
            if (PlayerInfo.Player[i]->Audio.pBaseFifoHdlr == pHdlr) {
                pPlayerPriv = PlayerInfo.Player[i];
                IsBase = 1U;
                Rval   = PLAYER_OK;
            }

            if (PlayerInfo.Player[i]->Audio.pVirtualFifoHdlr == pHdlr) {
                pPlayerPriv = PlayerInfo.Player[i];
                Rval   = PLAYER_OK;
            }
        }
    }

    if ((Rval == PLAYER_OK) && (IsBase == 1U)) {
        switch (Event) {
            case SVC_FIFO_EVENT_GET_WRITE_POINT:
                /* tell demuxer bitsbuf write pointer and free space size */
                AmbaMisra_TypeCast(&(Info->Desc->StartAddr), &(pPlayerPriv->Audio.BitsBufWp));
                if (pPlayerPriv->Audio.BitsBufWp >= pPlayerPriv->Audio.BitsBufRp) {
                    TmpUL             = pPlayerPriv->Audio.BitsBufSize + pPlayerPriv->Audio.BitsBufRp - pPlayerPriv->Audio.BitsBufWp;
                    Info->Desc->Size  = (UINT32)TmpUL;
                } else {
                    TmpUL             = pPlayerPriv->Audio.BitsBufRp - pPlayerPriv->Audio.BitsBufWp;
                    Info->Desc->Size  = (UINT32)TmpUL;
                }
                break;
            case SVC_FIFO_EVENT_DATA_READY:
                if (Info->Desc->FrameType == SVC_FIFO_TYPE_EOS) {
                    /* end of stream */
                    AmbaPrint_ModulePrintUInt5(AMBA_PLAYER_PRINT_MODULE_ID, "Audio SVC_FIFO_TYPE_EOS", 0U, 0U, 0U, 0U, 0U);
                } else if (Info->Desc->FrameType == SVC_FIFO_TYPE_DECODE_MARK) {
                    /* end of sequence */
                    AmbaPrint_ModulePrintUInt5(AMBA_PLAYER_PRINT_MODULE_ID, "Audio SVC_FIFO_TYPE_DECODE_MARK", 0U, 0U, 0U, 0U, 0U);
                } else if ((Info->Desc->FrameType == SVC_FIFO_TYPE_AUDIO_FRAME)) {
                    /* update bitsbuf write pointer */
                    ULONG StartAddr;

                    AmbaMisra_TypeCast(&(StartAddr), &(Info->Desc->StartAddr));

                    /* decrypt the bitstream data */
                    if ((pPlayerPriv->pDecryptFunc != NULL) && (pPlayerPriv->IsEncrypt == 1U)) {
                        AMBA_PLAYER_DECRYPT_s Decrypt = {0};
                        AMBA_PLAYER_s         *pPlayer;

                        AmbaMisra_TypeCast(&pPlayer, &pPlayerPriv);

                        Decrypt.FrameType    = AMBA_PLAYER_FRAME_TYPE_AUDIO;
                        Decrypt.SrcBufBase   = pPlayerPriv->Video.BitsBufBase;
                        Decrypt.SrcBufSize   = pPlayerPriv->Video.BitsBufSize;
                        Decrypt.SrcDataAddr  = StartAddr;
                        Decrypt.SrcDataSize  = Info->Desc->Size;

                        pPlayerPriv->pDecryptFunc(pPlayer, &Decrypt);

                        Info->Desc->Size = Decrypt.DstDataSize;
                    }

                    if ((StartAddr + Info->Desc->Size) < (pPlayerPriv->Audio.BitsBufBase + pPlayerPriv->Audio.BitsBufSize)) {
                        pPlayerPriv->Audio.BitsBufWp = StartAddr + Info->Desc->Size;
                    } else {
                        pPlayerPriv->Audio.BitsBufWp = (StartAddr + Info->Desc->Size) - pPlayerPriv->Audio.BitsBufSize;
                    }

                    pPlayerPriv->Audio.FeedCount ++;
                } else {
                    AmbaPrint_ModulePrintUInt5(AMBA_PLAYER_PRINT_MODULE_ID, "Unknown frame type (%u)", Info->Desc->FrameType, 0U, 0U, 0U, 0U);
                }

                Err = SvcFIFO_RemoveEntry(pHdlr, 1U);
                if (Err != PLAYER_OK) {
                    AmbaPrint_ModulePrintUInt5(AMBA_PLAYER_PRINT_MODULE_ID, "SvcFIFO_RemoveEntry failed (%u)", Err, 0U, 0U, 0U, 0U);
                }
                break;
            default:
                /* nothing */
                break;
        }
    }

    return Rval;
}

static UINT32 TextBitsFifoCB(const SVC_FIFO_HDLR_s *pHdlr, UINT8 Event, SVC_FIFO_CALLBACK_INFO_s *Info)
{
    UINT32                       Rval = PLAYER_ERROR_GENERAL_ERROR, Err, i, IsBase = 0U;
    ULONG                        TmpUL;
    AMBA_PLAYER_PLAYER_PRIV_s    *pPlayerPriv;

    AmbaMisra_TouchUnused(&pPlayerPriv);
    AmbaMisra_TouchUnused(Info);

    for (i = 0U; i < PlayerInfo.PlayerNum; i++) {
        if (PlayerInfo.Player[i] != NULL) {
            if (PlayerInfo.Player[i]->Text.pBaseFifoHdlr == pHdlr) {
                pPlayerPriv = PlayerInfo.Player[i];
                IsBase = 1U;
                Rval   = PLAYER_OK;
            }

            if (PlayerInfo.Player[i]->Text.pVirtualFifoHdlr == pHdlr) {
                pPlayerPriv = PlayerInfo.Player[i];
                Rval   = PLAYER_OK;
            }
        }
    }

    if ((Rval == PLAYER_OK) && (IsBase == 1U)) {
        switch (Event) {
            case SVC_FIFO_EVENT_GET_WRITE_POINT:
                /* tell demuxer bitsbuf write pointer and free space size */
                AmbaMisra_TypeCast(&(Info->Desc->StartAddr), &(pPlayerPriv->Text.BitsBufWp));
                if (pPlayerPriv->Text.BitsBufWp >= pPlayerPriv->Text.BitsBufRp) {
                    TmpUL             = pPlayerPriv->Text.BitsBufSize + pPlayerPriv->Text.BitsBufRp - pPlayerPriv->Text.BitsBufWp;
                    Info->Desc->Size  = (UINT32)TmpUL;
                } else {
                    TmpUL             = pPlayerPriv->Text.BitsBufRp - pPlayerPriv->Text.BitsBufWp;
                    Info->Desc->Size  = (UINT32)TmpUL;
                }
                break;
            case SVC_FIFO_EVENT_DATA_READY:
                if (Info->Desc->FrameType == SVC_FIFO_TYPE_EOS) {
                    /* end of stream */
                    AmbaPrint_ModulePrintUInt5(AMBA_PLAYER_PRINT_MODULE_ID, "Text SVC_FIFO_TYPE_EOS", 0U, 0U, 0U, 0U, 0U);
                } else if (Info->Desc->FrameType == SVC_FIFO_TYPE_DECODE_MARK) {
                    /* end of sequence */
                    AmbaPrint_ModulePrintUInt5(AMBA_PLAYER_PRINT_MODULE_ID, "Text SVC_FIFO_TYPE_DECODE_MARK", 0U, 0U, 0U, 0U, 0U);
                } else if ((Info->Desc->FrameType == SVC_FIFO_TYPE_UNDEFINED)) {
                    /* update bitsbuf write pointer */
                    ULONG StartAddr;

                    AmbaMisra_TypeCast(&(StartAddr), &(Info->Desc->StartAddr));

                    /* decrypt the bitstream data */
                    if ((pPlayerPriv->pDecryptFunc != NULL) && (pPlayerPriv->IsEncrypt == 1U)) {
                        AMBA_PLAYER_DECRYPT_s Decrypt = {0};
                        AMBA_PLAYER_s         *pPlayer;

                        AmbaMisra_TypeCast(&pPlayer, &pPlayerPriv);

                        Decrypt.FrameType    = AMBA_PLAYER_FRAME_TYPE_TEXT;
                        Decrypt.SrcBufBase   = pPlayerPriv->Video.BitsBufBase;
                        Decrypt.SrcBufSize   = pPlayerPriv->Video.BitsBufSize;
                        Decrypt.SrcDataAddr  = StartAddr;
                        Decrypt.SrcDataSize  = Info->Desc->Size;

                        pPlayerPriv->pDecryptFunc(pPlayer, &Decrypt);

                        Info->Desc->Size = Decrypt.DstDataSize;
                    }

                    if ((StartAddr + Info->Desc->Size) < (pPlayerPriv->Text.BitsBufBase + pPlayerPriv->Text.BitsBufSize)) {
                        pPlayerPriv->Text.BitsBufWp = StartAddr + Info->Desc->Size;
                    } else {
                        pPlayerPriv->Text.BitsBufWp = (StartAddr + Info->Desc->Size) - pPlayerPriv->Text.BitsBufSize;
                    }

                    /* update read pointer, always set to (write pointer - 1)*/

                    if (pPlayerPriv->Text.BitsBufWp == pPlayerPriv->Text.BitsBufBase) {
                        pPlayerPriv->Text.BitsBufRp = (pPlayerPriv->Text.BitsBufBase + pPlayerPriv->Text.BitsBufSize) - 1U;
                    } else {
                        pPlayerPriv->Text.BitsBufRp = pPlayerPriv->Text.BitsBufWp - 1U;
                    }

                    pPlayerPriv->Text.FeedCount ++;
                } else {
                    AmbaPrint_ModulePrintUInt5(AMBA_PLAYER_PRINT_MODULE_ID, "Unknown frame type (%u)", Info->Desc->FrameType, 0U, 0U, 0U, 0U);
                }

                Err = SvcFIFO_RemoveEntry(pHdlr, 1U);
                if (Err != PLAYER_OK) {
                    AmbaPrint_ModulePrintUInt5(AMBA_PLAYER_PRINT_MODULE_ID, "SvcFIFO_RemoveEntry failed (%u)", Err, 0U, 0U, 0U, 0U);
                }
                break;
            default:
                /* nothing */
                break;
        }
    }

    return Rval;
}

static UINT32 VideoPicCB(const void *pEventData)
{
    UINT32                                 Rval = PLAYER_ERROR_GENERAL_ERROR, PlayerId;
    AMBA_DSP_EVENT_VIDDEC_PIC_EX_s         *pPicInfo;
    const AMBA_PLAYER_PLAYER_PRIV_s        *pPlayerPriv;

    AmbaMisra_TypeCast(&pPicInfo, &pEventData);
    AmbaMisra_TouchUnused(&pPlayerPriv);

    for (PlayerId = 0U; PlayerId < PlayerInfo.PlayerNum; PlayerId++) {
        if (PlayerInfo.Player[PlayerId]->Video.DecId == pPicInfo->DecoderId) {
            pPlayerPriv = PlayerInfo.Player[PlayerId];
            Rval        = PLAYER_OK;
            break;
        }
    }

    if (Rval == PLAYER_OK) {
        if (pPlayerPriv->pNotify != NULL) {
            AMBA_PLAYER_s *pPlayer;

            AmbaMisra_TypeCast(&pPlayer, &pPlayerPriv);
            pPlayerPriv->pNotify(pPlayer, AMBA_PLAYER_STATUS_VID_FRAME_INFO, pPicInfo);
        }
    }

    return PLAYER_OK;
}

static UINT32 VideoDecCB(const void *pEventData)
{
    UINT32                                           Err, Rval = PLAYER_ERROR_GENERAL_ERROR, PlayerId, Flags = 0U;
    AMBA_DSP_EVENT_VIDEO_DEC_STATUS_UPDATE_s         *pStatus;
    AMBA_PLAYER_PLAYER_PRIV_s                        *pPlayerPriv;

    AmbaMisra_TypeCast(&pStatus, &pEventData);
    AmbaMisra_TouchUnused(&pPlayerPriv);

    for (PlayerId = 0U; PlayerId < PlayerInfo.PlayerNum; PlayerId++) {
        if (PlayerInfo.Player[PlayerId]->Video.DecId == pStatus->DecoderId) {
            pPlayerPriv = PlayerInfo.Player[PlayerId];
            Rval        = PLAYER_OK;
            break;
        }
    }

    if (Rval == PLAYER_ERROR_GENERAL_ERROR) {
        AmbaPrint_ModulePrintUInt5(AMBA_PLAYER_PRINT_MODULE_ID, "No matched decoder id (%u)", pStatus->DecoderId, 0U, 0U, 0U, 0U);
    }

    if (Rval == PLAYER_OK) {
        if (pPlayerPriv->pNotify != NULL) {
            AMBA_PLAYER_s *pPlayer;

            AmbaMisra_TypeCast(&pPlayer, &pPlayerPriv);
            pPlayerPriv->pNotify(pPlayer, AMBA_PLAYER_STATUS_VID_STATUS, pStatus);
        }
    }

    /* get flag */
    if (Rval == PLAYER_OK) {
        Err = AmbaKAL_EventFlagGet(&(pPlayerPriv->Flag), 0xFFFFFFFFU, AMBA_KAL_FLAGS_ANY,
                                   AMBA_KAL_FLAGS_CLEAR_NONE, &Flags, AMBA_KAL_NO_WAIT);
        if (Err != KAL_ERR_NONE) {
            /* nothing */
        }
    }

    /* dec start notofication */
    if (Rval == PLAYER_OK) {
        UINT32 VideoStart = 0U;

        if (0U == CheckBits(Flags, AMBA_PLAYER_FLG_VDEC_START)) {
            if (pPlayerPriv->Direction == AMBA_PLAYER_DIR_FORWARD) {
                if ((0xFFFFFFFFU != pStatus->DisplayFramePTS) && (0U != pStatus->NumOfDisplayPic) && (pStatus->DecState == DEC_STATE_RUN)) {
                    VideoStart = 1U;
                }
            } else {
                if ((0U < pStatus->NumOfDecodedPic) && (pStatus->DecState == DEC_STATE_RUN)) {
                    VideoStart = 1U;
                }
            }
        }

        if (VideoStart == 1U) {
            Err = AmbaKAL_EventFlagSet(&(pPlayerPriv->Flag), AMBA_PLAYER_FLG_VDEC_START);
            if (Err != KAL_ERR_NONE) {
                AmbaPrint_ModulePrintUInt5(AMBA_PLAYER_PRINT_MODULE_ID, "AmbaKAL_EventFlagSet failed (%u)", Err, 0U, 0U, 0U, 0U);
            } else {
                AmbaPrint_ModulePrintUInt5(AMBA_PLAYER_PRINT_MODULE_ID, "VDec (%u) start!!", (UINT32)pPlayerPriv->Video.DecId, 0U, 0U, 0U, 0U);
                Flags = SetBits(Flags, AMBA_PLAYER_FLG_VDEC_START);
            }

            if (pPlayerPriv->pNotify != NULL) {
                AMBA_PLAYER_s *pPlayer;

                AmbaMisra_TypeCast(&pPlayer, &pPlayerPriv);
                pPlayerPriv->pNotify(pPlayer, AMBA_PLAYER_STATUS_PLAYER_START, NULL);
            }
        }
    }

    if (Rval == PLAYER_OK) {
        if (0U < CheckBits(Flags, AMBA_PLAYER_FLG_VDEC_START)) {
            if (pStatus->EndOfStream == 1U) {
                /* nothing */
            }

            /* update bits read pointer */
            if (pStatus->DecState == DEC_STATE_RUN) {
                pPlayerPriv->Video.BitsBufRp = pStatus->BitsNextReadAddr;
                pPlayerPriv->LatestTime      = (UINT32)(pStatus->DisplayFramePTS / 90ULL);
            /* dec done notification */
            } else if (((pStatus->DecState == DEC_STATE_IDLE) || (pStatus->DecState == DEC_STATE_IDLE_WITH_LAST_PIC))) {
                if (0U == CheckBits(Flags, AMBA_PLAYER_FLG_VDEC_IDLE)) {
                    Err = AmbaKAL_EventFlagSet(&(pPlayerPriv->Flag), AMBA_PLAYER_FLG_VDEC_IDLE);
                    if (Err != KAL_ERR_NONE) {
                        AmbaPrint_ModulePrintUInt5(AMBA_PLAYER_PRINT_MODULE_ID, "AmbaKAL_EventFlagSet failed (%u)", Err, 0U, 0U, 0U, 0U);
                    } else {
                        AmbaPrint_ModulePrintUInt5(AMBA_PLAYER_PRINT_MODULE_ID, "VDec (%u) Idle!!", (UINT32)pPlayerPriv->Video.DecId, 0U, 0U, 0U, 0U);
                    }
                }
            } else {
                /**/
            }

            /* video dec error status */
            if (pStatus->ErrState != DEC_ERR_NONE) {
                AmbaPrint_ModulePrintUInt5(AMBA_PLAYER_PRINT_MODULE_ID, "VDec (%u) error (%u)!!", (UINT32)pPlayerPriv->Video.DecId, pStatus->ErrState, 0U, 0U, 0U);
            }

            pPlayerPriv->DispCount     = pStatus->NumOfDisplayPic;
        }
    }

    /* send msg queue to trigger feeding task */
    if (Rval == PLAYER_OK) {
        if ((0U < CheckBits(Flags, AMBA_PLAYER_FLG_VDEC_START)) && (0U == CheckBits(Flags, AMBA_PLAYER_FLG_VDEC_STOP))) {
            if (0U == CheckBits(Flags, AMBA_PLAYER_FLG_DMX_DONE)) {
                if (pStatus->NumOfDecodedPic > 0U) {
                    if ((((pStatus->NumOfDecodedPic - pPlayerPriv->Video.DecodedCount) + 1U) >= (UINT32)FEED_NUM) ||
                        (pPlayerPriv->Direction == AMBA_PLAYER_DIR_BACKWARD) ||
                        ((pPlayerPriv->Speed <= AMBA_PLAYER_SPEED_16X) && (pPlayerPriv->Speed > AMBA_PLAYER_SPEED_01X))) {
                        AMBA_PLAYER_FEED_MSG_s  Msg;

                        Msg.Event = AMBA_PLAYER_MSG_FEED;

                        Err = AmbaKAL_MsgQueueSend(&(pPlayerPriv->MsgQue), &Msg, AMBA_KAL_NO_WAIT);
                        if (Err != KAL_ERR_NONE) {
                            AmbaPrint_ModulePrintUInt5(AMBA_PLAYER_PRINT_MODULE_ID, "AmbaKAL_MsgQueueSend failed (%u)", Err, 0U, 0U, 0U, 0U);
                        }

                        pPlayerPriv->Video.DecodedCount  = pStatus->NumOfDecodedPic;
                    }
                }
            }
        }
    }

    if (Rval == PLAYER_OK) {
        if ((0U < CheckBits(Flags, AMBA_PLAYER_FLG_VDEC_START)) && (0U < CheckBits(Flags, AMBA_PLAYER_FLG_DMX_DONE)) && (0U == CheckBits(Flags, AMBA_PLAYER_FLG_VDEC_EOS))) {
            if (pStatus->EndOfStream > 0U) {
                Err = AmbaKAL_EventFlagSet(&(pPlayerPriv->Flag), AMBA_PLAYER_FLG_VDEC_EOS);
                if (Err != KAL_ERR_NONE) {
                    AmbaPrint_ModulePrintUInt5(AMBA_PLAYER_PRINT_MODULE_ID, "AmbaKAL_EventFlagSet failed (%u)", Err, 0U, 0U, 0U, 0U);
                } else {
                    AmbaPrint_ModulePrintUInt5(AMBA_PLAYER_PRINT_MODULE_ID, "VDec (%u) dec done!!", (UINT32)pPlayerPriv->Video.DecId, 0U, 0U, 0U, 0U);
                }

                if (pPlayerPriv->pNotify != NULL) {
                    AMBA_PLAYER_s *pPlayer;

                    AmbaMisra_TypeCast(&pPlayer, &pPlayerPriv);
                    pPlayerPriv->pNotify(pPlayer, AMBA_PLAYER_STATUS_PLAYER_EOS, NULL);
                }
            }
        }
    }

    return PLAYER_OK;
}

static UINT32 StillDecCB(const void *pEventData)
{
    UINT32                          Rval = PLAYER_ERROR_GENERAL_ERROR, PlayerId, Err;
    const AMBA_DSP_STLDEC_STATUS_s  *pPicInfo;
    AMBA_PLAYER_PLAYER_PRIV_s       *pPlayerPriv;

    AmbaMisra_TypeCast(&pPicInfo, &pEventData);
    AmbaMisra_TouchUnused(&pPlayerPriv);

    for (PlayerId = 0U; PlayerId < PlayerInfo.PlayerNum; PlayerId++) {
        if (PlayerInfo.Player[PlayerId]->Video.DecId == pPicInfo->Index) {
            pPlayerPriv = PlayerInfo.Player[PlayerId];
            Rval        = PLAYER_OK;
            break;
        }
    }

    if (Rval == PLAYER_OK) {
        Err = AmbaWrap_memcpy(&(pPlayerPriv->StillDecInfo), pEventData, sizeof(AMBA_DSP_STLDEC_STATUS_s));
        if (Err != PLAYER_OK) {
            AmbaPrint_ModulePrintUInt5(AMBA_PLAYER_PRINT_MODULE_ID, "AmbaWrap_memcpy failed (%u)", Err, 0U, 0U, 0U, 0U);
        }
    }

    if (Rval == PLAYER_OK) {
        Err = AmbaKAL_EventFlagSet(&(pPlayerPriv->Flag), AMBA_PLAYER_FLG_STLDEC_DONE);
        if (Err != KAL_ERR_NONE) {
            AmbaPrint_ModulePrintUInt5(AMBA_PLAYER_PRINT_MODULE_ID, "AmbaKAL_EventFlagSet failed (%u)", Err, 0U, 0U, 0U, 0U);
        } else {
            AmbaPrint_ModulePrintUInt5(AMBA_PLAYER_PRINT_MODULE_ID, "StillDec (%u) done!!", pPicInfo->Index, 0U, 0U, 0U, 0U);
        }
    }

    return PLAYER_OK;
}

#if defined(CONFIG_BUILD_COMAMBA_CODEC_AUDIO)
static UINT32 AudioDecCB(void *pEventData)
{
    const AMBA_ADEC_AUDIO_DESC_s   *pDesc;
    AMBA_PLAYER_PLAYER_PRIV_s      *pPlayerPriv;
    UINT32                         Rval = PLAYER_ERROR_GENERAL_ERROR, i;
    ULONG                          Rp, DecId;

    AmbaMisra_TouchUnused(&pPlayerPriv);
    AmbaMisra_TouchUnused(pEventData);

    AmbaMisra_TypeCast(&pDesc, &pEventData);
    AmbaMisra_TypeCast(&DecId, &(pDesc->pHdlr));

    for (i = 0U; i < PlayerInfo.PlayerNum; i++) {
        if (PlayerInfo.Player[i]->Audio.Enable == 1U) {
            pPlayerPriv = PlayerInfo.Player[i];
            if (DecId == pPlayerPriv->Audio.DecId) {
                Rval = PLAYER_OK;
                break;
            }
        }
    }

    if (Rval == PLAYER_OK) {
        /* update read pointer */
        Rp = pPlayerPriv->Audio.BitsBufRp + pDesc->DataSize;

        if (Rp >= (pPlayerPriv->Audio.BitsBufBase + pPlayerPriv->Audio.BitsBufSize)) {
            Rp -= pPlayerPriv->Audio.BitsBufSize;
        }

        pPlayerPriv->Audio.BitsBufRp = Rp;

        /* if eos */
        if (pDesc->Eos == 1U) {
            AmbaPrint_ModulePrintUInt5(AMBA_PLAYER_PRINT_MODULE_ID, "ADec (%u) dec done!!", (UINT32)pPlayerPriv->Audio.DecId, 0U, 0U, 0U, 0U);
        }
    }

    return Rval;
}
#endif

static UINT32 AmbaPlayer_DispCtrl(AMBA_PLAYER_s *pPlayer, const AMBA_PLAYER_DISP_CTRL_s *pDisp)
{
    UINT32                          Err, Rval = PLAYER_OK, i;
    UINT16                          PostNum = 0U;
    AMBA_PLAYER_PLAYER_PRIV_s       *pPlayerPriv;
    const SVC_VIDEO_TRACK_INFO_s    *pVideo;
    AMBA_DSP_VIDDEC_POST_CTRL_s     *pPCtrl, VPostCtrl[AMBA_DSP_MAX_VOUT_NUM] = {0U};
    const AMBA_PLAYER_DISP_INIT_s   *pVout;

    UINT32                          DispW, DispH, CenterX = 0U, CenterY = 0U, CropW, CropH;
    UINT32                          InputW, InputH, CropWHalf, CropHHalf, Rotate = 0U;

    AmbaMisra_TouchUnused(&Rval);
    AmbaMisra_TouchUnused(&pVideo);
    AmbaMisra_TouchUnused(pPlayer);
    AmbaMisra_TypeCast(&pPlayerPriv, &pPlayer);

    pVideo  = &(pPlayerPriv->Mov.VideoTrack[pPlayerPriv->Video.TrackId]);

    for (i = 0U; i < PlayerInfo.DispNum; i++) {
        if (0U < CheckBits(pDisp->VoutBits, ((UINT32)1U << (UINT32)PlayerInfo.DispInfo[i].VoutIdx))) {
            pVout = &(PlayerInfo.DispInfo[i]);

            if ((pDisp->RotateFlip == AMBA_DSP_ROTATE_0) ||
                (pDisp->RotateFlip == AMBA_DSP_ROTATE_0_HORZ_FLIP) ||
                (pDisp->RotateFlip == AMBA_DSP_ROTATE_180) ||
                (pDisp->RotateFlip == AMBA_DSP_ROTATE_180_HORZ_FLIP)) {
                InputW     = pVideo->Width;
                InputH     = pVideo->Height;
                CenterX    = pDisp->ZoomCenterX;
                CenterY    = pDisp->ZoomCenterY;
            } else {
                InputW     = pVideo->Height;
                InputH     = pVideo->Width;
                CenterX    = pDisp->ZoomCenterY;
                CenterY    = pDisp->ZoomCenterX;
                Rotate     = 1U;
            }

            /* calculate display size */
            {
                DOUBLE InputRatio, Outputratio;

                InputRatio  = (DOUBLE)InputW / (DOUBLE)InputH;
                Outputratio = (DOUBLE)pVout->AspectRatioX / (DOUBLE)pVout->AspectRatioY;

                if (InputRatio < Outputratio) {
                    DispW = ((UINT32)pVout->VoutWindow.Height * InputW) / InputH;
                    DispH = (UINT32)pVout->VoutWindow.Height;
                } else if (InputRatio > Outputratio) {
                    DispW = (UINT32)pVout->VoutWindow.Width;
                    DispH = ((UINT32)pVout->VoutWindow.Width * InputH) / InputW;
                } else {
                    DispW = (UINT32)pVout->VoutWindow.Width;
                    DispH = (UINT32)pVout->VoutWindow.Height;
                }

                if (DispW > (UINT32)pVout->VoutWindow.Width) {
                    DispW = (UINT32)pVout->VoutWindow.Width;
                }

                if (DispH > (UINT32)pVout->VoutWindow.Height) {
                    DispH = (UINT32)pVout->VoutWindow.Height;
                }

                DispH  = ClearBits(DispH, 0x03U);
            }

            /* calculate crop window */
            {
                DOUBLE ZoomFactorX, ZoomFactorY, Temp;

                ZoomFactorX = (DOUBLE)pDisp->ZoomRatio / 100.0;
                ZoomFactorY = (DOUBLE)pDisp->ZoomRatio / 100.0;

                /* correct crop window */
                Temp   = (DOUBLE)InputW  / ZoomFactorX;
                CropW  = (UINT32)Temp;
                Temp   = (DOUBLE)InputH  / ZoomFactorY;
                CropH  = (UINT32)Temp;

                if (CropW >= (UINT32)InputW) {
                    CropW = InputW;
                }

                if (CropH >= InputH) {
                    CropH = InputH;
                }

                CropWHalf  = CropW / 2U;
                CropHHalf  = CropH / 2U;

                /* correct center x */
                if (((InputW - 1U) - CenterX) <= CropWHalf) {
                    CenterX = (InputW - 1U) - CropWHalf;
                }

                if (CenterX <= CropWHalf) {
                    CenterX = CropWHalf;
                }

                /* correct center y */
                if (((InputH - 1U) - CenterY) <= CropHHalf) {
                    CenterY = (InputH - 1U) - CropHHalf;
                }

                if (CenterY <= CropHHalf) {
                    CenterY = CropHHalf;
                }
            }

            pPCtrl = &(VPostCtrl[PostNum]);
            if (Rotate == 0U) {
                pPCtrl->InputWindow.Width    = (UINT16)CropW;
                pPCtrl->InputWindow.Height   = (UINT16)CropH;
                pPCtrl->InputWindow.OffsetX  = (UINT16)(CenterX - CropWHalf);
                pPCtrl->InputWindow.OffsetY  = (UINT16)(CenterY - CropHHalf);
            } else {
                pPCtrl->InputWindow.Width    = (UINT16)CropH;
                pPCtrl->InputWindow.Height   = (UINT16)CropW;
                pPCtrl->InputWindow.OffsetX  = (UINT16)(CenterY - CropHHalf);
                pPCtrl->InputWindow.OffsetY  = (UINT16)(CenterX - CropWHalf);
            }
            pPCtrl->TargetWindow.Width   = (UINT16)(DispW);
            pPCtrl->TargetWindow.Height  = (UINT16)(DispH);
            pPCtrl->TargetWindow.OffsetX = 0U;
            pPCtrl->TargetWindow.OffsetY = 0U;
            pPCtrl->TargetRotateFlip     = (UINT8)(pDisp->RotateFlip);
            pPCtrl->VoutIdx              = (pVout->VoutIdx);
            pPCtrl->VoutWindow.Width     = (UINT16)(DispW);
            pPCtrl->VoutWindow.Height    = (UINT16)(DispH);
            pPCtrl->VoutWindow.OffsetX   = (pVout->VoutWindow.Width - (UINT16)DispW) / 2U;
            pPCtrl->VoutWindow.OffsetY   = (pVout->VoutWindow.Height - (UINT16)DispH) / 2U;
            pPCtrl->VoutRotateFlip       = (UINT8)(pVout->VoutRotateFlip);

            pPCtrl->VoutWindow.OffsetY   = (pPCtrl->VoutWindow.OffsetY >> pVout->IsInterlace);
            pPCtrl->VoutWindow.Height    = (pPCtrl->VoutWindow.Height >> pVout->IsInterlace);

            PostNum++;
        }
    }

    if ((PostNum > 0U) && (Rval == PLAYER_OK)) {
        Err = AmbaDSP_VideoDecPostCtrl((UINT16)(pPlayerPriv->Video.DecId), PostNum, VPostCtrl);
        if (PLAYER_OK != Err) {
            AmbaPrint_ModulePrintUInt5(AMBA_PLAYER_PRINT_MODULE_ID, "AmbaDSP_VideoDecPostCtrl failed (%u)", Err, 0U, 0U, 0U, 0U);
            Rval = PLAYER_ERROR_GENERAL_ERROR;
        }
    }

    if ((PostNum > 0U) && (Rval == PLAYER_OK)) {
        pPlayerPriv->Disp.ZoomRatio   = pDisp->ZoomRatio;
        pPlayerPriv->Disp.RotateFlip  = pDisp->RotateFlip;
        if (Rotate == 0U) {
            pPlayerPriv->Disp.ZoomCenterX = CenterX;
            pPlayerPriv->Disp.ZoomCenterY = CenterY;
        } else {
            pPlayerPriv->Disp.ZoomCenterX = CenterY;
            pPlayerPriv->Disp.ZoomCenterY = CenterX;
        }
    }

    return Rval;
}

static void FillExpMov(const SVC_MOVIE_INFO_s *pSrc, AMBA_MOVIE_INFO_s *pDst)
{
    UINT32  i, Num;

    Num = GetMinValU32(AMBA_MAX_VIDEO_TRACK_PER_MEDIA, SVC_FORMAT_MAX_VIDEO_TRACK_PER_MEDIA);

    for (i = 0U; i < Num; i++) {
        pDst->VideoTrack[i].MediaId = pSrc->VideoTrack[i].Info.MediaId;
        pDst->VideoTrack[i].Width   = pSrc->VideoTrack[i].Width;
        pDst->VideoTrack[i].Height  = pSrc->VideoTrack[i].Height;
    }
    pDst->VideoTrackCount = pSrc->VideoTrackCount;
    pDst->AudioTrackCount = pSrc->AudioTrackCount;
    pDst->TextTrackCount  = pSrc->TextTrackCount;
}

static UINT32 SeamlessStart(AMBA_PLAYER_s *pPlayer, const char *FileName)
{
    UINT32                     Err, Rval = PLAYER_OK;
    AMBA_PLAYER_PLAYER_PRIV_s  *pPlayerPriv;

    AmbaMisra_TouchUnused(&Rval);
    AmbaMisra_TouchUnused(pPlayer);
    AmbaMisra_TypeCast(&pPlayerPriv, &pPlayer);

    if (Rval == PLAYER_OK) {
        if (pPlayerPriv->Seamless == 0U) {
            AmbaPrint_ModulePrintUInt5(AMBA_PLAYER_PRINT_MODULE_ID, "seamless playback is not enabled (%u)", pPlayerPriv->PlayerId, 0U, 0U, 0U, 0U);
            Rval = PLAYER_ERROR_GENERAL_ERROR;
        }
    }

    /* stop demuxer */
    if (Rval == PLAYER_OK) {
        Err = AmbaDmux_Stop(&(pPlayerPriv->DmxCtrl));
        if (Err != PLAYER_OK) {
            AmbaPrint_ModulePrintUInt5(AMBA_PLAYER_PRINT_MODULE_ID, "AmbaDmux_Stop failed (%u)", Err, 0U, 0U, 0U, 0U);
            Rval = PLAYER_ERROR_GENERAL_ERROR;
        }
    }

    if (Rval == PLAYER_OK) {
        AMBA_PLAYER_FEED_MSG_s  Msg;

        Msg.Event = AMBA_PLAYER_MSG_STOP;

        Err = AmbaKAL_MsgQueueSend(&(pPlayerPriv->MsgQue), &Msg, AMBA_KAL_NO_WAIT);
        if (Err != KAL_ERR_NONE) {
            AmbaPrint_ModulePrintUInt5(AMBA_PLAYER_PRINT_MODULE_ID, "AmbaKAL_MsgQueueSend failed (%u)", Err, 0U, 0U, 0U, 0U);
            Rval = PLAYER_ERROR_GENERAL_ERROR;
        }
    }

    if (Rval == PLAYER_OK) {
        UINT32 ActualFlags = 0U;
        Err = AmbaKAL_EventFlagGet(&(pPlayerPriv->Flag), (AMBA_PLAYER_FLG_DMX_DONE | AMBA_PLAYER_FLG_DMX_STOP), AMBA_KAL_FLAGS_ALL,
                                    KAL_FLAGS_CLEAR_NONE, &ActualFlags, AMBA_KAL_WAIT_FOREVER);
        if (Err != KAL_ERR_NONE) {
            AmbaPrint_ModulePrintUInt5(AMBA_PLAYER_PRINT_MODULE_ID, "AmbaKAL_EventFlagGet failed(%u/%u)", ActualFlags, Err, 0U, 0U, 0U);
            Rval = PLAYER_ERROR_GENERAL_ERROR;
        }
    }

    /* delete demuxer */
    if (Rval == PLAYER_OK) {
        Err = AmbaDmux_Delete(&(pPlayerPriv->DmxCtrl));
        if (Err != PLAYER_OK) {
            AmbaPrint_ModulePrintUInt5(AMBA_PLAYER_PRINT_MODULE_ID, "AmbaDmux_Delete failed (%u)", Err, 0U, 0U, 0U, 0U);
            Rval = PLAYER_ERROR_GENERAL_ERROR;
        }
    }

    /* update start dts */
    if (Rval == PLAYER_OK) {
        const SVC_MEDIA_TRACK_GENERAL_INFO_s    *pTrack;
        UINT64                                  TrackDuration;

        /* video */
        if (pPlayerPriv->Video.Enable == 1U) {
            pTrack = &(pPlayerPriv->Mov.VideoTrack[pPlayerPriv->Video.TrackId].Info);

            TrackDuration                = (UINT64)pTrack->TimePerFrame * (UINT64)pTrack->FrameCount;
            pPlayerPriv->Video.StartDTS += TrackDuration;
        }

        /* audio */
        if (pPlayerPriv->Audio.Enable == 1U) {
            pTrack = &(pPlayerPriv->Mov.AudioTrack[pPlayerPriv->Audio.TrackId - pPlayerPriv->Mov.VideoTrackCount].Info);

            TrackDuration                = (UINT64)pTrack->TimePerFrame * (UINT64)pTrack->FrameCount;
            pPlayerPriv->Audio.StartDTS += TrackDuration;
        }

        /* text */
        if (pPlayerPriv->Text.Enable == 1U) {
            pTrack = &(pPlayerPriv->Mov.TextTrack[(pPlayerPriv->Text.TrackId - pPlayerPriv->Mov.AudioTrackCount) - pPlayerPriv->Mov.VideoTrackCount].Info);

            TrackDuration                = (UINT64)pTrack->TimePerFrame * (UINT64)pTrack->FrameCount;
            pPlayerPriv->Text.StartDTS  += TrackDuration;
        }
    }

    /* parse the file */
    if (Rval == PLAYER_OK) {
        Err = AmbaDmux_Parse(AMBA_DMUX_MTYPE_MOVIE, FileName, pPlayerPriv->pStreamHdlr, &(pPlayerPriv->Mov), &(pPlayerPriv->ContainerType));
        if (Err != PLAYER_OK) {
            AmbaPrint_ModulePrintUInt5(AMBA_PLAYER_PRINT_MODULE_ID, "AmbaDmux_Parse failed (%u)", Err, 0U, 0U, 0U, 0U);
            Rval = PLAYER_ERROR_GENERAL_ERROR;
        } else {
            FillExpMov(&(pPlayerPriv->Mov), &(pPlayerPriv->ExpMov));
        }
    }

    /* encryption checking */
    if (Rval == PLAYER_OK) {
        UINT32                           Count = 0U;
        UINT8                            *pBuf;
        AMBA_MP4_UDTA_SVCD_BOX_s         SvcData = {0};
        SVC_STREAM_HDLR_s                *pHdlr;
        const AMBA_MP4_UDTA_SVCD_BOX_s   *pSvcData;

        pSvcData = &SvcData;
        AmbaMisra_TypeCast(&pBuf, &pSvcData);
        AmbaMisra_TouchUnused(&SvcData);

        AmbaMisra_TypeCast(&pHdlr, &(pPlayerPriv->pStreamHdlr));

        pPlayerPriv->IsEncrypt = 0U;

        if (pPlayerPriv->Mov.UserDataSize > 0U) {
            Err = pHdlr->Func->Open(pHdlr, FileName, SVC_STREAM_MODE_RDONLY);
            if (Err != PLAYER_OK) {
                AmbaPrint_ModulePrintUInt5(AMBA_PLAYER_PRINT_MODULE_ID, "pHdlr->open failed (%u)", Err, 0U, 0U, 0U, 0U);
                Rval = PLAYER_ERROR_GENERAL_ERROR;
            }

            if (Rval == PLAYER_OK) {
                Err = pHdlr->Func->Seek(pHdlr, pPlayerPriv->Mov.UserDataPos, SVC_STREAM_SEEK_START);
                if (Err != PLAYER_OK) {
                    AmbaPrint_ModulePrintUInt5(AMBA_PLAYER_PRINT_MODULE_ID, "pHdlr->Seek failed (%u)", Err, 0U, 0U, 0U, 0U);
                    Rval = PLAYER_ERROR_GENERAL_ERROR;
                }
            }

            if (Rval == PLAYER_OK) {
                Err = pHdlr->Func->Read(pHdlr, sizeof(AMBA_MP4_UDTA_SVCD_BOX_s), pBuf, &Count);
                if (Err != PLAYER_OK) {
                    AmbaPrint_ModulePrintUInt5(AMBA_PLAYER_PRINT_MODULE_ID, "pHdlr->Read failed (%u)", Err, 0U, 0U, 0U, 0U);
                    Rval = PLAYER_ERROR_GENERAL_ERROR;
                }
            }

            if (Rval == PLAYER_OK) {
                Err = pHdlr->Func->Close(pHdlr);
                if (Err != PLAYER_OK) {
                    AmbaPrint_ModulePrintUInt5(AMBA_PLAYER_PRINT_MODULE_ID, "pHdlr->Seek failed (%u)", Err, 0U, 0U, 0U, 0U);
                    Rval = PLAYER_ERROR_GENERAL_ERROR;
                }
            }

            if (Rval == PLAYER_OK) {
                if (UDTA_BOX_TYPE_SVCD == Invert_UInt32(SvcData.BoxType)) {
                    if (0U < Invert_UInt32(SvcData.IsEncrypt)) {
                        pPlayerPriv->IsEncrypt = 1U;
                        AmbaPrint_ModulePrintUInt5(AMBA_PLAYER_PRINT_MODULE_ID, "the clip is encrypted", 0U, 0U, 0U, 0U, 0U);
                    }
                } else {
                    AmbaPrint_ModulePrintUInt5(AMBA_PLAYER_PRINT_MODULE_ID, "Unknown box type 0x%x", SvcData.BoxType, 0U, 0U, 0U, 0U);
                }
            }
        }
    }

    /* reset player parameters */
    if (Rval == PLAYER_OK) {
        ULONG                             Addr;
        SVC_MEDIA_TRACK_GENERAL_INFO_s    *pTrack;

        /* video */
        if (pPlayerPriv->Video.Enable == 1U) {
            pTrack = &(pPlayerPriv->Mov.VideoTrack[pPlayerPriv->Video.TrackId].Info);

            pTrack->Fifo     = pPlayerPriv->Video.pVirtualFifoHdlr;

            Addr = pPlayerPriv->Video.BitsBufBase;
            AmbaMisra_TypeCast(&(pTrack->BufferBase), &Addr);

            pTrack->BufferSize = pPlayerPriv->Video.BitsBufSize;

            pTrack->DTS      = pPlayerPriv->Video.StartDTS;
            pTrack->NextDTS  = pTrack->DTS;
            pTrack->InitDTS  = pTrack->DTS;
        } else {
            pTrack = &(pPlayerPriv->Mov.VideoTrack[0].Info);

            pTrack->Fifo              = NULL;
            pTrack->DTS               = SVC_FORMAT_MAX_TIMESTAMP;
            pTrack->InitDTS           = pTrack->DTS;
        }

        /* audio */
        if (pPlayerPriv->Audio.Enable == 1U) {
            pTrack = &(pPlayerPriv->Mov.AudioTrack[pPlayerPriv->Audio.TrackId - pPlayerPriv->Mov.VideoTrackCount].Info);

            pTrack->Fifo     = pPlayerPriv->Audio.pVirtualFifoHdlr;

            Addr = pPlayerPriv->Audio.BitsBufBase;
            AmbaMisra_TypeCast(&(pTrack->BufferBase), &Addr);

            pTrack->BufferSize = pPlayerPriv->Audio.BitsBufSize;

            pTrack->DTS      = pPlayerPriv->Audio.StartDTS;
            pTrack->NextDTS  = pTrack->DTS;
            pTrack->InitDTS  = pTrack->DTS;
        } else {
            pTrack = &(pPlayerPriv->Mov.AudioTrack[0].Info);

            pTrack->Fifo              = NULL;
            pTrack->DTS               = SVC_FORMAT_MAX_TIMESTAMP;
            pTrack->InitDTS           = pTrack->DTS;
        }

        /* text */
        if (pPlayerPriv->Text.Enable == 1U) {
            pTrack = &(pPlayerPriv->Mov.TextTrack[(pPlayerPriv->Text.TrackId - pPlayerPriv->Mov.AudioTrackCount) - pPlayerPriv->Mov.VideoTrackCount].Info);

            pTrack->Fifo     = pPlayerPriv->Text.pVirtualFifoHdlr;

            Addr = pPlayerPriv->Text.BitsBufBase;
            AmbaMisra_TypeCast(&(pTrack->BufferBase), &Addr);

            pTrack->BufferSize = pPlayerPriv->Text.BitsBufSize;

            pTrack->DTS      = pPlayerPriv->Text.StartDTS;
            pTrack->NextDTS  = pTrack->DTS;
            pTrack->InitDTS  = pTrack->DTS;
        } else {
            pTrack = &(pPlayerPriv->Mov.TextTrack[0].Info);

            pTrack->Fifo              = NULL;
            pTrack->DTS               = SVC_FORMAT_MAX_TIMESTAMP;
            pTrack->InitDTS           = pTrack->DTS;
        }
    }

    /* create demuxer */
    if (Rval == PLAYER_OK) {
        AMBA_DMUX_CREATE_s  Dmux;

        Dmux.pMovInfo         = &(pPlayerPriv->Mov);
        Dmux.ContainerType    = pPlayerPriv->ContainerType;
        Dmux.StartTime        = 0U;
        Dmux.Direction        = pPlayerPriv->Direction;
        Dmux.pStreamHdlr      = pPlayerPriv->pStreamHdlr;

        if (pPlayerPriv->Speed == AMBA_PLAYER_SPEED_02X) {
            Dmux.Speed = 2U;
        } else if (pPlayerPriv->Speed == AMBA_PLAYER_SPEED_04X) {
            Dmux.Speed = 4U;
        } else if (pPlayerPriv->Speed == AMBA_PLAYER_SPEED_08X) {
            Dmux.Speed = 8U;
        } else if (pPlayerPriv->Speed == AMBA_PLAYER_SPEED_16X) {
            Dmux.Speed = 16U;
        } else {
            Dmux.Speed = 1U;
        }

        Err = AmbaDmux_Create(&(pPlayerPriv->DmxCtrl), &Dmux);
        if (Err != PLAYER_OK) {
            AmbaPrint_ModulePrintUInt5(AMBA_PLAYER_PRINT_MODULE_ID, "AmbaDmux_Create failed (%u)", Err, 0U, 0U, 0U, 0U);
            Rval = PLAYER_ERROR_GENERAL_ERROR;
        }
    }

    /* start demuxer */
    if (Rval == PLAYER_OK) {
        Err = AmbaDmux_Start(&(pPlayerPriv->DmxCtrl));
        if (Err != PLAYER_OK) {
            AmbaPrint_ModulePrintUInt5(AMBA_PLAYER_PRINT_MODULE_ID, "AmbaDmux_Start failed (%u)", Err, 0U, 0U, 0U, 0U);
            Rval = PLAYER_ERROR_GENERAL_ERROR;
        }
    }

    if (Rval == PLAYER_OK) {
        Err = AmbaKAL_EventFlagClear(&(pPlayerPriv->Flag), (AMBA_PLAYER_FLG_DMX_DONE | AMBA_PLAYER_FLG_DMX_STOP));
        if (Err != KAL_ERR_NONE) {
            AmbaPrint_ModulePrintUInt5(AMBA_PLAYER_PRINT_MODULE_ID, "AmbaKAL_EventFlagClear failed(%u/%u)", AMBA_PLAYER_FLG_DMX_DONE, Err, 0U, 0U, 0U);
            Rval = PLAYER_ERROR_GENERAL_ERROR;
        }
    }

    return Rval;
}

/**
* evaluate memory size
* @param [in]  PlayerNum max player number
* @param [out]  pSize size
* @return ErrorCode
*/
UINT32 AmbaPlayer_EvalMemSize(UINT32 PlayerNum, UINT32 *pSize)
{
    UINT32 Err, Rval = PLAYER_OK, RSize = 0U;

    AmbaMisra_TouchUnused(&Rval);

    if (Rval == PLAYER_OK) {
        UINT32 DmxSize = 0U;
        Err = AmbaDmux_EvalMemSize(PlayerNum, &DmxSize);
        if (Err == PLAYER_OK) {
            RSize += DmxSize;
        } else {
            AmbaPrint_ModulePrintUInt5(AMBA_PLAYER_PRINT_MODULE_ID, "AmbaDmux_EvalMemSize failed (%u)", Err, 0U, 0U, 0U, 0U);
            Rval = PLAYER_ERROR_GENERAL_ERROR;
        }
    }

    if (Rval == PLAYER_OK) {
        *pSize = RSize;
    }

    return Rval;
}

/**
* init player
* @param [in]  pInit init info
* @return ErrorCode
*/
UINT32 AmbaPlayer_Init(const AMBA_PLAYER_INIT_s *pInit)
{
    UINT32 Err, Rval = PLAYER_OK, i;

    AmbaMisra_TouchUnused(&Rval);

    if (Rval == PLAYER_OK) {
        if (pInit->DispNum > AMBA_DSP_MAX_VOUT_NUM) {
            AmbaPrint_ModulePrintUInt5(AMBA_PLAYER_PRINT_MODULE_ID, "Invalid DispNum (%u/%u)", pInit->DispNum, AMBA_DSP_MAX_VOUT_NUM, 0U, 0U, 0U);
            Rval = PLAYER_ERROR_ARG;
        }

        if (pInit->PlayerNum > (UINT32)CONFIG_AMBA_PLAYER_MAX_NUM) {
            AmbaPrint_ModulePrintUInt5(AMBA_PLAYER_PRINT_MODULE_ID, "Invalid PlayerNum (%u/%u)", pInit->PlayerNum, CONFIG_AMBA_PLAYER_MAX_NUM, 0U, 0U, 0U);
            Rval = PLAYER_ERROR_ARG;
        }
    }

    if (Rval == PLAYER_OK) {
        Err = AmbaWrap_memset(&PlayerInfo, 0, sizeof(AMBA_PLAYER_INFO_s));
        if (Err != 0U) {
            AmbaPrint_ModulePrintUInt5(AMBA_PLAYER_PRINT_MODULE_ID, "AmbaWrap_memset failed (%u)", Err, 0U, 0U, 0U, 0U);
        }
    }

    /* store info */
    if (Rval == PLAYER_OK) {
        PlayerInfo.PlayerNum  = pInit->PlayerNum;

        /* vout */
        PlayerInfo.DispNum = pInit->DispNum;
        for (i = 0U; i < PlayerInfo.DispNum; i++) {
            Err = AmbaWrap_memcpy(&(PlayerInfo.DispInfo[i]), &(pInit->DispInfo[i]), sizeof(AMBA_PLAYER_DISP_INIT_s));
            if (Err != PLAYER_OK) {
                AmbaPrint_ModulePrintUInt5(AMBA_PLAYER_PRINT_MODULE_ID, "AmbaWrap_memcpy failed (%u)", Err, 0U, 0U, 0U, 0U);
            }
        }

        /* task */
        PlayerInfo.TaskPriority = pInit->TaskPriority;
        PlayerInfo.TaskCpuBits  = pInit->TaskCpuBits;

        /* bitsbuff */
        for (i = 0U; i < PlayerInfo.PlayerNum; i++) {
            Err = AmbaWrap_memcpy(&(PlayerInfo.BitsBuf[i]), &(pInit->BitsBuf[i]), sizeof(AMBA_PLAYER_BITSBUF_INIT_s));
            if (Err != PLAYER_OK) {
                AmbaPrint_ModulePrintUInt5(AMBA_PLAYER_PRINT_MODULE_ID, "AmbaWrap_memcpy failed (%u)", Err, 0U, 0U, 0U, 0U);
            }
        }
    }

    /* config video decoder */
    if (Rval == PLAYER_OK) {
        Err = VDecConfig(1U, NULL, 1U);
        if (Err != PLAYER_OK) {
            AmbaPrint_ModulePrintUInt5(AMBA_PLAYER_PRINT_MODULE_ID, "VDecConfig failed (%u)", Err, 0U, 0U, 0U, 0U);
            Rval = PLAYER_ERROR_GENERAL_ERROR;
        }
    }

    /* demuxer */
    if (Rval == PLAYER_OK) {
        AMBA_DMUX_INIT_s Dmx;

        Dmx.DemuxNum         = PlayerInfo.PlayerNum;
        Dmx.DmxBuf.BufAddr   = pInit->PlayerBuf.BufAddr;
        Dmx.DmxBuf.BufSize   = pInit->PlayerBuf.BufSize;

        Err = AmbaDmux_Init(&Dmx);
        if (Err != PLAYER_OK) {
            AmbaPrint_ModulePrintUInt5(AMBA_PLAYER_PRINT_MODULE_ID, "AmbaDmux_Init failed (%u)", Err, 0U, 0U, 0U, 0U);
            Rval = PLAYER_ERROR_GENERAL_ERROR;
        }
    }

    return Rval;
}

/**
* parse media
* @param [in]  pPlayer player handler
* @param [in]  pStreamHdlr stream handler
* @param [in]  FileName FileName
* @param [in]  MediaType MediaType
* @return ErrorCode
*/
UINT32 AmbaPlayer_Parse(AMBA_PLAYER_s *pPlayer, void *pStreamHdlr, const char *FileName, UINT32 MediaType)
{
    UINT32                     Rval = PLAYER_OK, Err, ParseType;
    AMBA_PLAYER_PLAYER_PRIV_s  *pPlayerPriv;
    SVC_STREAM_HDLR_s          *pHdlr;

    AmbaMisra_TouchUnused(&Rval);
    AmbaMisra_TouchUnused(pPlayer);
    AmbaMisra_TouchUnused(&ParseType);
    AmbaMisra_TouchUnused(pStreamHdlr);

    AmbaMisra_TypeCast(&pPlayerPriv, &pPlayer);

    if (MediaType == AMBA_PLAYER_MTYPE_IMAGE) {
        ParseType = AMBA_DMUX_MTYPE_IMAGE;
    } else if (MediaType == AMBA_PLAYER_MTYPE_MOVIE) {
        ParseType = AMBA_DMUX_MTYPE_MOVIE;
    } else {
        AmbaPrint_ModulePrintUInt5(AMBA_PLAYER_PRINT_MODULE_ID, "Unknown parse type (%u)", MediaType, 0U, 0U, 0U, 0U);
        Rval = PLAYER_ERROR_ARG;
    }

    if (Rval == PLAYER_OK) {
        AmbaMisra_TypeCast(&pHdlr, &pStreamHdlr);

        Err = AmbaDmux_Parse(ParseType, FileName, pHdlr, &(pPlayerPriv->Mov), &(pPlayerPriv->ContainerType));
        if (Err != PLAYER_OK) {
            AmbaPrint_ModulePrintUInt5(AMBA_PLAYER_PRINT_MODULE_ID, "AmbaDmux_Parse failed (%u)", Err, 0U, 0U, 0U, 0U);
            Rval = PLAYER_ERROR_GENERAL_ERROR;
        } else {
            FillExpMov(&(pPlayerPriv->Mov), &(pPlayerPriv->ExpMov));
        }
    }

    /* encryption checking */
    if (Rval == PLAYER_OK) {
        UINT32                           Count = 0U;
        UINT8                            *pBuf;
        AMBA_MP4_UDTA_SVCD_BOX_s         SvcData = {0};
        const AMBA_MP4_UDTA_SVCD_BOX_s   *pSvcData;

        pSvcData = &SvcData;
        AmbaMisra_TypeCast(&pHdlr, &pStreamHdlr);
        AmbaMisra_TypeCast(&pBuf, &pSvcData);
        AmbaMisra_TouchUnused(&SvcData);

        pPlayerPriv->IsEncrypt = 0U;

        if (pPlayerPriv->Mov.UserDataSize > 0U) {
            Err = pHdlr->Func->Open(pHdlr, FileName, SVC_STREAM_MODE_RDONLY);
            if (Err != PLAYER_OK) {
                AmbaPrint_ModulePrintUInt5(AMBA_PLAYER_PRINT_MODULE_ID, "pHdlr->open failed (%u)", Err, 0U, 0U, 0U, 0U);
                Rval = PLAYER_ERROR_GENERAL_ERROR;
            }

            if (Rval == PLAYER_OK) {
                Err = pHdlr->Func->Seek(pHdlr, pPlayerPriv->Mov.UserDataPos, SVC_STREAM_SEEK_START);
                if (Err != PLAYER_OK) {
                    AmbaPrint_ModulePrintUInt5(AMBA_PLAYER_PRINT_MODULE_ID, "pHdlr->Seek failed (%u)", Err, 0U, 0U, 0U, 0U);
                    Rval = PLAYER_ERROR_GENERAL_ERROR;
                }
            }

            if (Rval == PLAYER_OK) {
                Err = pHdlr->Func->Read(pHdlr, sizeof(AMBA_MP4_UDTA_SVCD_BOX_s), pBuf, &Count);
                if (Err != PLAYER_OK) {
                    AmbaPrint_ModulePrintUInt5(AMBA_PLAYER_PRINT_MODULE_ID, "pHdlr->Read failed (%u)", Err, 0U, 0U, 0U, 0U);
                    Rval = PLAYER_ERROR_GENERAL_ERROR;
                }
            }

            if (Rval == PLAYER_OK) {
                Err = pHdlr->Func->Close(pHdlr);
                if (Err != PLAYER_OK) {
                    AmbaPrint_ModulePrintUInt5(AMBA_PLAYER_PRINT_MODULE_ID, "pHdlr->Seek failed (%u)", Err, 0U, 0U, 0U, 0U);
                    Rval = PLAYER_ERROR_GENERAL_ERROR;
                }
            }

            if (Rval == PLAYER_OK) {
                if (UDTA_BOX_TYPE_SVCD == Invert_UInt32(SvcData.BoxType)) {
                    if (0U < Invert_UInt32(SvcData.IsEncrypt)) {
                        pPlayerPriv->IsEncrypt = 1U;
                        AmbaPrint_ModulePrintUInt5(AMBA_PLAYER_PRINT_MODULE_ID, "the clip is encrypted", 0U, 0U, 0U, 0U, 0U);
                    }
                } else {
                    AmbaPrint_ModulePrintUInt5(AMBA_PLAYER_PRINT_MODULE_ID, "Unknown box type 0x%x", SvcData.BoxType, 0U, 0U, 0U, 0U);
                }
            }
        }
    }

    return Rval;
}

/**
* create player
* @param [in]  pPlayer player handler
* @param [in]  pCreate create info
* @return ErrorCode
*/
UINT32 AmbaPlayer_Create(AMBA_PLAYER_s *pPlayer, const AMBA_PLAYER_CREATE_s *pCreate)
{
    static UINT32              PlayerPrivSize = (UINT32)sizeof(AMBA_PLAYER_PLAYER_PRIV_s);
    UINT32                     Err, Rval = PLAYER_OK, i, Reserved = (UINT32)sizeof(AMBA_PLAYER_s);
    AMBA_PLAYER_PLAYER_PRIV_s  *pPlayerPriv;
    const void                 *pvHdlr = pCreate->pStreamHdlr;
    SVC_STREAM_HDLR_s          *pHdlr;

    AmbaMisra_TouchUnused(&Rval);
    AmbaMisra_TouchUnused(pPlayer);

    AmbaMisra_TypeCast(&pPlayerPriv, &pPlayer);
    AmbaMisra_TypeCast(&pHdlr, &pvHdlr);

    if (Rval == PLAYER_OK) {
        if (PlayerPrivSize > Reserved) {
            AmbaPrint_ModulePrintUInt5(AMBA_PLAYER_PRINT_MODULE_ID, "AMBA_PLAYER_s size not enough (%u/%u)", Reserved, PlayerPrivSize, 0U, 0U, 0U);
            Rval = PLAYER_ERROR_ARG;
        }
    }

    /* player info */
    if (Rval == PLAYER_OK) {
        Rval = PLAYER_ERROR_GENERAL_ERROR;
        for (i = 0U; i < PlayerInfo.PlayerNum; i++) {
            if (0U == CheckBits(PlayerInfo.PlayerBits, ((UINT32)1U << i))) {
                pPlayerPriv->PlayerId          = i;
                pPlayerPriv->pNotify           = pCreate->pFunc;
                pPlayerPriv->pDecryptFunc      = pCreate->pDecryptFunc;
                pPlayerPriv->Seamless          = pCreate->Seamless;
                pPlayerPriv->Direction         = pCreate->Direction;
                pPlayerPriv->Speed             = pCreate->Speed;
                pPlayerPriv->StartTime         = pCreate->StartTime;
                pPlayerPriv->pStreamHdlr       = pHdlr;
                pPlayerPriv->PreloadSize       = pCreate->PreloadSize;
                pPlayerPriv->FeedType          = pCreate->FeedType;
                pPlayerPriv->DispCount         = 0ULL;
                pPlayerPriv->AudioCodecIndex   = pCreate->AudioCodecIndex;

                pPlayerPriv->Video.DecId       = i;
                pPlayerPriv->Video.TrackId     = pCreate->Video.TrackId;
                pPlayerPriv->Video.BitsBufBase = PlayerInfo.BitsBuf[i].VBitsBufBase;
                pPlayerPriv->Video.BitsBufSize = PlayerInfo.BitsBuf[i].VBitsBufSize;
                pPlayerPriv->Video.BitsBufWp   = pPlayerPriv->Video.BitsBufBase;
                pPlayerPriv->Video.BitsBufRp   = pPlayerPriv->Video.BitsBufBase + pPlayerPriv->Video.BitsBufSize - 1U;
                pPlayerPriv->Video.BitsBufUpdp = pPlayerPriv->Video.BitsBufBase;
                pPlayerPriv->Video.FeedCount   = 0U;
                pPlayerPriv->Video.DecodedCount= 0U;

                pPlayerPriv->Audio.TrackId     = pCreate->Audio.TrackId;
                pPlayerPriv->Audio.BitsBufBase = PlayerInfo.BitsBuf[i].ABitsBufBase;
                pPlayerPriv->Audio.BitsBufSize = PlayerInfo.BitsBuf[i].ABitsBufSize;
                pPlayerPriv->Audio.BitsBufWp   = pPlayerPriv->Audio.BitsBufBase;
                pPlayerPriv->Audio.BitsBufRp   = pPlayerPriv->Audio.BitsBufBase + pPlayerPriv->Audio.BitsBufSize - 1U;
                pPlayerPriv->Audio.FeedCount   = 0U;
                pPlayerPriv->Audio.DecodedCount= 0U;

                pPlayerPriv->Text.DecId        = i;
                pPlayerPriv->Text.TrackId      = pCreate->Text.TrackId;
                pPlayerPriv->Text.BitsBufBase  = PlayerInfo.BitsBuf[i].TBitsBufBase;
                pPlayerPriv->Text.BitsBufSize  = PlayerInfo.BitsBuf[i].TBitsBufSize;
                pPlayerPriv->Text.BitsBufWp    = pPlayerPriv->Text.BitsBufBase;
                pPlayerPriv->Text.BitsBufRp    = pPlayerPriv->Text.BitsBufBase + pPlayerPriv->Text.BitsBufSize - 1U;
                pPlayerPriv->Text.FeedCount    = 0U;
                pPlayerPriv->Text.DecodedCount = 0U;

                Rval = PLAYER_OK;

                /* if the clip is encrypted, pDecryptFunc should not be null */
                if ((pPlayerPriv->IsEncrypt > 0U) && (pPlayerPriv->pDecryptFunc == NULL)) {
                    AmbaPrint_ModulePrintUInt5(AMBA_PLAYER_PRINT_MODULE_ID, "the clip is encrypted, please register decryption callback for player", 0U, 0U, 0U, 0U, 0U);
                    Rval = PLAYER_ERROR_GENERAL_ERROR;
                }
                break;
            }
        }

        if (Rval != PLAYER_OK) {
            AmbaPrint_ModulePrintUInt5(AMBA_PLAYER_PRINT_MODULE_ID, "No available player!!", 0U, 0U, 0U, 0U, 0U);
        }
    }

    /* create bits fifo */
    if (Rval == PLAYER_OK) {
        AMBA_BFIFO_CREATE_s FifoInfo;

        AmbaMisra_TouchUnused(&Rval);
        Err = AmbaWrap_memset(&FifoInfo, 0, sizeof(FifoInfo));
        if (Err != 0U) {
            AmbaPrint_ModulePrintUInt5(AMBA_PLAYER_PRINT_MODULE_ID, "AmbaWrap_memset failed (%u)", Err, 0U, 0U, 0U, 0U);
        }

        /* video */
        if ((pCreate->Video.Enable > 0U) && (pPlayerPriv->Mov.VideoTrackCount > 0U)) {
            /* base */
            if (Rval == PLAYER_OK) {
                FifoInfo.pBaseHdlr           = NULL;
                FifoInfo.CbEvent             = VideoBitsFifoCB;
                FifoInfo.BaseCfg.AutoRemove  = 1U;
                FifoInfo.BaseCfg.TimeScale   = 0U;
                Err = AmbaBitsFifo_Create(&FifoInfo, &(pPlayerPriv->Video.pBaseFifoHdlr));
                if (Err != PLAYER_OK) {
                    AmbaPrint_ModulePrintUInt5(AMBA_PLAYER_PRINT_MODULE_ID, "VBaseFifoHdlr create failed (%u)", Err, 0U, 0U, 0U, 0U);
                    Rval = PLAYER_ERROR_GENERAL_ERROR;
                }
            }
            /* virtal */
            if (Rval == PLAYER_OK) {
                FifoInfo.pBaseHdlr            = pPlayerPriv->Video.pBaseFifoHdlr;
                FifoInfo.CbEvent              = VideoBitsFifoCB;
                FifoInfo.BaseCfg.AutoRemove   = 1U;
                FifoInfo.BaseCfg.TimeScale   = 0U;
                Err = AmbaBitsFifo_Create(&FifoInfo, &(pPlayerPriv->Video.pVirtualFifoHdlr));
                if (Err != PLAYER_OK) {
                    AmbaPrint_ModulePrintUInt5(AMBA_PLAYER_PRINT_MODULE_ID, "VVirtualFifoHdlr create failed (%u)", Err, 0U, 0U, 0U, 0U);
                    Rval = PLAYER_ERROR_GENERAL_ERROR;
                }
            }
        }

        /* audio */
        if ((pCreate->Audio.Enable > 0U) && (pPlayerPriv->Mov.AudioTrackCount > 0U)) {
            /* base */
            if (Rval == PLAYER_OK) {
                FifoInfo.pBaseHdlr           = NULL;
                FifoInfo.CbEvent             = AudioBitsFifoCB;
                FifoInfo.BaseCfg.AutoRemove  = 1U;
                FifoInfo.BaseCfg.TimeScale   = 0U;
                Err = AmbaBitsFifo_Create(&FifoInfo, &(pPlayerPriv->Audio.pBaseFifoHdlr));
                if (Err != PLAYER_OK) {
                    AmbaPrint_ModulePrintUInt5(AMBA_PLAYER_PRINT_MODULE_ID, "ABaseFifoHdlr create failed (%u)", Err, 0U, 0U, 0U, 0U);
                    Rval = PLAYER_ERROR_GENERAL_ERROR;
                }
            }
            /* virtal */
            if (Rval == PLAYER_OK) {
                FifoInfo.pBaseHdlr           = pPlayerPriv->Audio.pBaseFifoHdlr;
                FifoInfo.CbEvent             = AudioBitsFifoCB;
                FifoInfo.BaseCfg.AutoRemove  = 1U;
                FifoInfo.BaseCfg.TimeScale   = 0U;
                Err = AmbaBitsFifo_Create(&FifoInfo, &(pPlayerPriv->Audio.pVirtualFifoHdlr));
                if (Err != PLAYER_OK) {
                    AmbaPrint_ModulePrintUInt5(AMBA_PLAYER_PRINT_MODULE_ID, "AVirtualFifoHdlr create failed (%u)", Err, 0U, 0U, 0U, 0U);
                    Rval = PLAYER_ERROR_GENERAL_ERROR;
                }
            }
        }

        /* text */
        if ((pCreate->Text.Enable > 0U) && (pPlayerPriv->Mov.TextTrackCount > 0U)) {
            /* base */
            if (Rval == PLAYER_OK) {
                FifoInfo.pBaseHdlr           = NULL;
                FifoInfo.CbEvent             = TextBitsFifoCB;
                FifoInfo.BaseCfg.AutoRemove  = 1U;
                FifoInfo.BaseCfg.TimeScale   = 0U;
                Err = AmbaBitsFifo_Create(&FifoInfo, &(pPlayerPriv->Text.pBaseFifoHdlr));
                if (Err != PLAYER_OK) {
                    AmbaPrint_ModulePrintUInt5(AMBA_PLAYER_PRINT_MODULE_ID, "TBaseFifoHdlr create failed (%u)", Err, 0U, 0U, 0U, 0U);
                    Rval = PLAYER_ERROR_GENERAL_ERROR;
                }
            }
            /* virtal */
            if (Rval == PLAYER_OK) {
                FifoInfo.pBaseHdlr           = pPlayerPriv->Text.pBaseFifoHdlr;
                FifoInfo.CbEvent             = TextBitsFifoCB;
                FifoInfo.BaseCfg.AutoRemove  = 1U;
                FifoInfo.BaseCfg.TimeScale   = 0U;
                Err = AmbaBitsFifo_Create(&FifoInfo, &(pPlayerPriv->Text.pVirtualFifoHdlr));
                if (Err != PLAYER_OK) {
                    AmbaPrint_ModulePrintUInt5(AMBA_PLAYER_PRINT_MODULE_ID, "TVirtualFifoHdlr create failed (%u)", Err, 0U, 0U, 0U, 0U);
                    Rval = PLAYER_ERROR_GENERAL_ERROR;
                }
            }
        }
    }

    /* fill info */
    if (Rval == PLAYER_OK) {
        ULONG                             Addr;
        SVC_MEDIA_TRACK_GENERAL_INFO_s    *pTrack;

        /* video */
        for (i = 0U; i < pPlayerPriv->Mov.VideoTrackCount; i++) {
            pTrack = &(pPlayerPriv->Mov.VideoTrack[i].Info);

            if ((pCreate->Video.Enable > 0U) && (pCreate->Video.TrackId == i)) {
                pPlayerPriv->Video.Enable = 1U;
                pTrack->Fifo          = pPlayerPriv->Video.pVirtualFifoHdlr;

                Addr = pPlayerPriv->Video.BitsBufBase;
                AmbaMisra_TypeCast(&(pTrack->BufferBase), &Addr);

                pTrack->BufferSize = pPlayerPriv->Video.BitsBufSize;

                pPlayerPriv->Video.StartDTS = 0ULL;
                pTrack->DTS                 = 0ULL;
                pTrack->InitDTS             = 0ULL;

                break;
            } else {
                pPlayerPriv->Video.Enable = 0U;
                pTrack->DTS               = SVC_FORMAT_MAX_TIMESTAMP;
                pTrack->InitDTS           = pTrack->DTS;
                pTrack->Fifo              = NULL;
            }
        }

        /* audio */
        for (i = 0U; i < pPlayerPriv->Mov.AudioTrackCount; i++) {
            pTrack = &(pPlayerPriv->Mov.AudioTrack[i].Info);

            if ((pCreate->Audio.Enable > 0U) && ((pCreate->Audio.TrackId - pPlayerPriv->Mov.VideoTrackCount) == i)) {
                pPlayerPriv->Audio.Enable = 1U;
                pTrack->Fifo          = pPlayerPriv->Audio.pVirtualFifoHdlr;

                Addr = pPlayerPriv->Audio.BitsBufBase;
                AmbaMisra_TypeCast(&(pTrack->BufferBase), &Addr);

                pTrack->BufferSize = pPlayerPriv->Audio.BitsBufSize;

                pPlayerPriv->Audio.StartDTS = 0ULL;
                pTrack->DTS                 = 0ULL;
                pTrack->InitDTS             = 0ULL;

                break;
            } else {
                pPlayerPriv->Audio.Enable = 0U;
                pTrack->DTS               = SVC_FORMAT_MAX_TIMESTAMP;
                pTrack->InitDTS           = pTrack->DTS;
                pTrack->Fifo              = NULL;
            }
        }

        /* text */
        for (i = 0U; i < pPlayerPriv->Mov.TextTrackCount; i++) {
            pTrack = &(pPlayerPriv->Mov.TextTrack[i].Info);

            if ((pCreate->Text.Enable > 0U) && (((pCreate->Text.TrackId - pPlayerPriv->Mov.AudioTrackCount) - pPlayerPriv->Mov.VideoTrackCount) == i)) {
                pPlayerPriv->Text.Enable = 1U;
                pTrack->Fifo         = pPlayerPriv->Text.pVirtualFifoHdlr;

                Addr = pPlayerPriv->Text.BitsBufBase;
                AmbaMisra_TypeCast(&(pTrack->BufferBase), &Addr);

                pTrack->BufferSize = pPlayerPriv->Text.BitsBufSize;

                pPlayerPriv->Text.StartDTS = 0ULL;
                pTrack->DTS                = 0ULL;
                pTrack->InitDTS            = 0ULL;

                break;
            } else {
                pPlayerPriv->Text.Enable = 0U;
                pTrack->DTS              = SVC_FORMAT_MAX_TIMESTAMP;
                pTrack->InitDTS          = pTrack->DTS;
                pTrack->Fifo              = NULL;
            }
        }
    }

#if defined(CONFIG_BUILD_COMAMBA_CODEC_AUDIO)
    /* setup audio decoder */
    if (Rval == PLAYER_OK) {
        if (pPlayerPriv->Audio.Enable == 1U) {
            UINT32 Type = 0U, Track;

            Track = pPlayerPriv->Audio.TrackId - pPlayerPriv->Mov.VideoTrackCount;

            if (pPlayerPriv->Mov.AudioTrack[Track].Info.MediaId == SVC_FORMAT_MID_AAC) {
                Type = AMBA_AUDIO_TYPE_AAC;
            } else if (pPlayerPriv->Mov.AudioTrack[Track].Info.MediaId == SVC_FORMAT_MID_PCM) {
                Type = AMBA_AUDIO_TYPE_PCM;
            } else {
                AmbaPrint_ModulePrintUInt5(AMBA_PLAYER_PRINT_MODULE_ID, "Unknown audio type (%u)", pPlayerPriv->Mov.AudioTrack[pPlayerPriv->Audio.TrackId].Info.MediaId, 0U, 0U, 0U, 0U);
                Rval = PLAYER_ERROR_GENERAL_ERROR;
            }

            if (Rval == PLAYER_OK) {
                AmbaADecFlow_Setup(Type, AMBA_AUDIO_SOURCE_MP4, 0U, AudioDecCB, &(pPlayerPriv->Audio.DecId));
            }
        }
    }
#endif

    /* create msg queue */
    if (Rval == PLAYER_OK) {
        static char Name[32];

        ULONG  Num;

        Num = pPlayerPriv->PlayerId;
        if (0U < AmbaUtility_UInt32ToStr(Name, 32U, (UINT32)Num, 16U)) {
            AmbaUtility_StringAppend(Name, 32, "Playback_Que");
        }

        Err = AmbaKAL_MsgQueueCreate(&(pPlayerPriv->MsgQue), Name, (UINT32)sizeof(AMBA_PLAYER_FEED_MSG_s), &(pPlayerPriv->MsgBuf[0]),
                                     PLAYER_MSG_NUM * (UINT32)sizeof(AMBA_PLAYER_FEED_MSG_s));
        if (Err != KAL_ERR_NONE) {
            AmbaPrint_ModulePrintUInt5(AMBA_PLAYER_PRINT_MODULE_ID, "AmbaKAL_MsgQueueCreate failed (%u)", Err, 0U, 0U, 0U, 0U);
            Rval = PLAYER_ERROR_GENERAL_ERROR;
        }
    }

    /* create feed task */
    if (Rval == PLAYER_OK) {
        static char      Name[] = "Playback_Task";

        Err = AmbaKAL_TaskCreate(&(pPlayerPriv->Task), Name, PlayerInfo.TaskPriority,
                                 FeedTask, pPlayerPriv, &(pPlayerPriv->StackBuf[0]), PLAYER_STACK_SIZE, 0U);
        if (Err != KAL_ERR_NONE) {
            AmbaPrint_ModulePrintUInt5(AMBA_PLAYER_PRINT_MODULE_ID, "AmbaKAL_TaskCreate failed (%u)", Err, 0U, 0U, 0U, 0U);
            Rval = PLAYER_ERROR_GENERAL_ERROR;
        }

        if (Rval == PLAYER_OK) {
            Err = AmbaKAL_TaskSetSmpAffinity(&(pPlayerPriv->Task), PlayerInfo.TaskCpuBits);
            if (Err != KAL_ERR_NONE) {
                AmbaPrint_ModulePrintUInt5(AMBA_PLAYER_PRINT_MODULE_ID, "AmbaKAL_TaskSetSmpAffinity failed (%u)", Err, 0U, 0U, 0U, 0U);
                Rval = PLAYER_ERROR_GENERAL_ERROR;
            }
        }

        if (Rval == PLAYER_OK) {
            Err = AmbaKAL_TaskResume(&(pPlayerPriv->Task));
            if (Err != KAL_ERR_NONE) {
                AmbaPrint_ModulePrintUInt5(AMBA_PLAYER_PRINT_MODULE_ID, "AmbaKAL_TaskResume failed (%u)", Err, 0U, 0U, 0U, 0U);
                Rval = PLAYER_ERROR_GENERAL_ERROR;
            }
        }
    }

    /* create flag */
    if (Rval == PLAYER_OK) {
        static char Name[] = "Playback_Flag";

        Err = AmbaKAL_EventFlagCreate(&(pPlayerPriv->Flag), Name);
        if (Err != KAL_ERR_NONE) {
            AmbaPrint_ModulePrintUInt5(AMBA_PLAYER_PRINT_MODULE_ID, "AmbaKAL_EventFlagCreate failed (%u)", Err, 0U, 0U, 0U, 0U);
            Rval = PLAYER_ERROR_GENERAL_ERROR;
        }
    }

    if (Rval == PLAYER_OK) {
        Err = AmbaKAL_EventFlagClear(&(pPlayerPriv->Flag), 0xFFFFFFFFU);
        if (Err != KAL_ERR_NONE) {
            AmbaPrint_ModulePrintUInt5(AMBA_PLAYER_PRINT_MODULE_ID, "AmbaKAL_EventFlagClear failed (%u)", Err, 0U, 0U, 0U, 0U);
            Rval = PLAYER_ERROR_GENERAL_ERROR;
        }
    }

    /* create demuxer */
    if (Rval == PLAYER_OK) {
        AMBA_DMUX_CREATE_s  Dmux;

        Dmux.pMovInfo         = &(pPlayerPriv->Mov);
        Dmux.ContainerType    = pPlayerPriv->ContainerType;
        Dmux.StartTime        = pCreate->StartTime;
        Dmux.Direction        = pCreate->Direction;
        Dmux.pStreamHdlr      = pHdlr;

        if (pCreate->Speed == AMBA_PLAYER_SPEED_02X) {
            Dmux.Speed = 2U;
        } else if (pCreate->Speed == AMBA_PLAYER_SPEED_04X) {
            Dmux.Speed = 4U;
        } else if (pCreate->Speed == AMBA_PLAYER_SPEED_08X) {
            Dmux.Speed = 8U;
        } else if (pCreate->Speed == AMBA_PLAYER_SPEED_16X) {
            Dmux.Speed = 16U;
        } else {
            Dmux.Speed = 1U;
        }

        Err = AmbaDmux_Create(&(pPlayerPriv->DmxCtrl), &Dmux);
        if (Err != PLAYER_OK) {
            AmbaPrint_ModulePrintUInt5(AMBA_PLAYER_PRINT_MODULE_ID, "AmbaDmux_Create failed (%u)", Err, 0U, 0U, 0U, 0U);
            Rval = PLAYER_ERROR_GENERAL_ERROR;
        }
    }

    /* start demuxer */
    if (Rval == PLAYER_OK) {
        Err = AmbaDmux_Start(&(pPlayerPriv->DmxCtrl));
        if (Err != PLAYER_OK) {
            AmbaPrint_ModulePrintUInt5(AMBA_PLAYER_PRINT_MODULE_ID, "AmbaDmux_Start failed (%u)", Err, 0U, 0U, 0U, 0U);
            Rval = PLAYER_ERROR_GENERAL_ERROR;
        }
    }

    /* reset disp info */
    if (Rval == PLAYER_OK) {
        pPlayerPriv->Disp.ZoomRatio   = 100U;
        pPlayerPriv->Disp.ZoomCenterX = (UINT32)pPlayerPriv->Mov.VideoTrack[pCreate->Video.TrackId].Width / 2U;
        pPlayerPriv->Disp.ZoomCenterY = (UINT32)pPlayerPriv->Mov.VideoTrack[pCreate->Video.TrackId].Height / 2U;
        pPlayerPriv->Disp.RotateFlip  = 0U;
        pPlayerPriv->Disp.VoutBits    = 0U;

        for (i = 0U; i < PlayerInfo.DispNum; i++) {
            pPlayerPriv->Disp.VoutBits = SetBits(pPlayerPriv->Disp.VoutBits, ((UINT32)1U << (UINT32)PlayerInfo.DispInfo[i].VoutIdx));
        }
    }

    if (Rval == PLAYER_OK) {
        if (PlayerInfo.PlayerBits == 0U) {
            Err = AmbaDSP_EventHandlerRegister(AMBA_DSP_EVENT_VIDEO_DEC_STATUS, VideoDecCB);
            if (Err != PLAYER_OK) {
                AmbaPrint_ModulePrintUInt5(AMBA_PLAYER_PRINT_MODULE_ID, "AmbaDSP_EventHandlerRegister failed (%u)", Err, 0U, 0U, 0U, 0U);
                Rval = PLAYER_ERROR_GENERAL_ERROR;
            }

            Err = AmbaDSP_EventHandlerRegister(AMBA_DSP_EVENT_VIDEO_DEC_PIC_INFO, VideoPicCB);
            if (Err != PLAYER_OK) {
                AmbaPrint_ModulePrintUInt5(AMBA_PLAYER_PRINT_MODULE_ID, "AmbaDSP_EventHandlerRegister failed (%u)", Err, 0U, 0U, 0U, 0U);
                Rval = PLAYER_ERROR_GENERAL_ERROR;
            }
        }
    }

    if (Rval == PLAYER_OK) {
        PlayerInfo.PlayerBits                    = SetBits(PlayerInfo.PlayerBits, ((UINT32)1U << pPlayerPriv->PlayerId));
        PlayerInfo.Player[pPlayerPriv->PlayerId] = pPlayerPriv;
    }

    /* preload */
    if (Rval == PLAYER_OK) {
        if (pPlayerPriv->FeedType == AMBA_PLAYER_FTYPE_MOVIE) {
            AMBA_PLAYER_FEED_MSG_s Msg;

            Msg.Event      = AMBA_PLAYER_MSG_PRELOAD;

            Err = AmbaKAL_MsgQueueSend(&(pPlayerPriv->MsgQue), &Msg, 1500U);
            if (Err != KAL_ERR_NONE) {
                AmbaPrint_ModulePrintUInt5(AMBA_PLAYER_PRINT_MODULE_ID, "AmbaKAL_MsgQueueSend failed (%u)", Err, 0U, 0U, 0U, 0U);
                Rval = PLAYER_ERROR_GENERAL_ERROR;
            }
        }
    }

    return Rval;
}

/**
* delete player
* @param [in]  pPlayer player handler
* @return ErrorCode
*/
UINT32 AmbaPlayer_Delete(AMBA_PLAYER_s *pPlayer)
{
    UINT32                     Err, Rval = PLAYER_OK;
    AMBA_PLAYER_PLAYER_PRIV_s  *pPlayerPriv;

    AmbaMisra_TouchUnused(&Rval);
    AmbaMisra_TouchUnused(pPlayer);
    AmbaMisra_TypeCast(&pPlayerPriv, &pPlayer);

    /* stop demuxer */
    if (Rval == PLAYER_OK) {
        Err = AmbaDmux_Stop(&(pPlayerPriv->DmxCtrl));
        if (Err != PLAYER_OK) {
            AmbaPrint_ModulePrintUInt5(AMBA_PLAYER_PRINT_MODULE_ID, "AmbaDmux_Stop failed (%u)", Err, 0U, 0U, 0U, 0U);
            Rval = PLAYER_ERROR_GENERAL_ERROR;
        }
    }

    if (Rval == PLAYER_OK) {
        AMBA_PLAYER_FEED_MSG_s  Msg;

        Msg.Event = AMBA_PLAYER_MSG_STOP;

        Err = AmbaKAL_MsgQueueSend(&(pPlayerPriv->MsgQue), &Msg, AMBA_KAL_NO_WAIT);
        if (Err != KAL_ERR_NONE) {
            AmbaPrint_ModulePrintUInt5(AMBA_PLAYER_PRINT_MODULE_ID, "AmbaKAL_MsgQueueSend failed (%u)", Err, 0U, 0U, 0U, 0U);
            Rval = PLAYER_ERROR_GENERAL_ERROR;
        }
    }

    if (Rval == PLAYER_OK) {
        UINT32 ActualFlags = 0U;
        Err = AmbaKAL_EventFlagGet(&(pPlayerPriv->Flag), (AMBA_PLAYER_FLG_DMX_DONE | AMBA_PLAYER_FLG_DMX_STOP), AMBA_KAL_FLAGS_ALL,
                                    AMBA_KAL_FLAGS_CLEAR_NONE, &ActualFlags, 5000U);
        if (Err != KAL_ERR_NONE) {
            AmbaPrint_ModulePrintUInt5(AMBA_PLAYER_PRINT_MODULE_ID, "AmbaKAL_EventFlagGet failed(%u/%u)", ActualFlags, Err, 0U, 0U, 0U);
            Rval = PLAYER_ERROR_GENERAL_ERROR;
        } else {
            if (pPlayerPriv->pNotify != NULL) {
                pPlayerPriv->pNotify(pPlayer, AMBA_PLAYER_STATUS_PLAYER_STOP, NULL);
            }
        }
    }

    /* delete demuxer */
    if (Rval == PLAYER_OK) {
        Err = AmbaDmux_Delete(&(pPlayerPriv->DmxCtrl));
        if (Err != PLAYER_OK) {
            AmbaPrint_ModulePrintUInt5(AMBA_PLAYER_PRINT_MODULE_ID, "AmbaDmux_Delete failed (%u)", Err, 0U, 0U, 0U, 0U);
            Rval = PLAYER_ERROR_GENERAL_ERROR;
        }
    }

    /* delete feed task */
    if (Rval == PLAYER_OK) {
        Err = AmbaKAL_TaskTerminate(&(pPlayerPriv->Task));
        if (Err != KAL_ERR_NONE) {
            AmbaPrint_ModulePrintUInt5(AMBA_PLAYER_PRINT_MODULE_ID, "AmbaKAL_TaskTerminate failed (%u)", Err, 0U, 0U, 0U, 0U);
            Rval = PLAYER_ERROR_GENERAL_ERROR;
        }

        if (Rval == PLAYER_OK) {
            Err = AmbaKAL_TaskDelete(&(pPlayerPriv->Task));
            if (Err != KAL_ERR_NONE) {
                AmbaPrint_ModulePrintUInt5(AMBA_PLAYER_PRINT_MODULE_ID, "AmbaKAL_TaskDelete failed (%u)", Err, 0U, 0U, 0U, 0U);
                Rval = PLAYER_ERROR_GENERAL_ERROR;
            }
        }
    }

    /* delete flag */
    if (Rval == PLAYER_OK) {
        Err = AmbaKAL_EventFlagDelete(&(pPlayerPriv->Flag));
        if (Err != KAL_ERR_NONE) {
            AmbaPrint_ModulePrintUInt5(AMBA_PLAYER_PRINT_MODULE_ID, "AmbaKAL_EventFlagDelete failed (%u)", Err, 0U, 0U, 0U, 0U);
            Rval = PLAYER_ERROR_GENERAL_ERROR;
        }
    }

    /* delete msg queue */
    if (Rval == PLAYER_OK) {
        Err = AmbaKAL_MsgQueueDelete(&(pPlayerPriv->MsgQue));
        if (Err != KAL_ERR_NONE) {
            AmbaPrint_ModulePrintUInt5(AMBA_PLAYER_PRINT_MODULE_ID, "AmbaKAL_MsgQueueDelete failed (%u)", Err, 0U, 0U, 0U, 0U);
            Rval = PLAYER_ERROR_GENERAL_ERROR;
        }
    }

    /* delete bits fifo */
    if (Rval == PLAYER_OK) {
        /* video */
        if (pPlayerPriv->Video.Enable > 0U) {
            /* base */
            Err = AmbaBitsFifo_Delete(pPlayerPriv->Video.pBaseFifoHdlr);
            if (Err != PLAYER_OK) {
                AmbaPrint_ModulePrintUInt5(AMBA_PLAYER_PRINT_MODULE_ID, "VBaseFifoHdlr delete failed (%u)", Err, 0U, 0U, 0U, 0U);
                Rval = PLAYER_ERROR_GENERAL_ERROR;
            }

            /* virtal */
            Err = AmbaBitsFifo_Delete(pPlayerPriv->Video.pVirtualFifoHdlr);
            if (Err != PLAYER_OK) {
                AmbaPrint_ModulePrintUInt5(AMBA_PLAYER_PRINT_MODULE_ID, "VVirtualFifoHdlr delete failed (%u)", Err, 0U, 0U, 0U, 0U);
                Rval = PLAYER_ERROR_GENERAL_ERROR;
            }
        }

#if defined(CONFIG_BUILD_COMAMBA_CODEC_AUDIO)
        /* audio */
        if (pPlayerPriv->Audio.Enable > 0U) {
            AmbaADecFlow_Delete(pPlayerPriv->Audio.DecId);

            /* base */
            Err = AmbaBitsFifo_Delete(pPlayerPriv->Audio.pBaseFifoHdlr);
            if (Err != PLAYER_OK) {
                AmbaPrint_ModulePrintUInt5(AMBA_PLAYER_PRINT_MODULE_ID, "ABaseFifoHdlr delete failed (%u)", Err, 0U, 0U, 0U, 0U);
                Rval = PLAYER_ERROR_GENERAL_ERROR;
            }

            /* virtal */
            Err = AmbaBitsFifo_Delete(pPlayerPriv->Audio.pVirtualFifoHdlr);
            if (Err != PLAYER_OK) {
                AmbaPrint_ModulePrintUInt5(AMBA_PLAYER_PRINT_MODULE_ID, "AVirtualFifoHdlr delete failed (%u)", Err, 0U, 0U, 0U, 0U);
                Rval = PLAYER_ERROR_GENERAL_ERROR;
            }
        }
#endif

        /* text */
        if (pPlayerPriv->Text.Enable > 0U) {
            /* base */
            Err = AmbaBitsFifo_Delete(pPlayerPriv->Text.pBaseFifoHdlr);
            if (Err != PLAYER_OK) {
                AmbaPrint_ModulePrintUInt5(AMBA_PLAYER_PRINT_MODULE_ID, "TBaseFifoHdlr delete failed (%u)", Err, 0U, 0U, 0U, 0U);
                Rval = PLAYER_ERROR_GENERAL_ERROR;
            }

            /* virtal */
            Err = AmbaBitsFifo_Delete(pPlayerPriv->Text.pVirtualFifoHdlr);
            if (Err != PLAYER_OK) {
                AmbaPrint_ModulePrintUInt5(AMBA_PLAYER_PRINT_MODULE_ID, "TVirtualFifoHdlr delete failed (%u)", Err, 0U, 0U, 0U, 0U);
                Rval = PLAYER_ERROR_GENERAL_ERROR;
            }
        }
    }

    if (Rval == PLAYER_OK) {
        PlayerInfo.PlayerBits = ClearBits(PlayerInfo.PlayerBits, ((UINT32)1U << pPlayerPriv->PlayerId));

        if (PlayerInfo.PlayerBits == 0U) {
            Err = AmbaDSP_EventHandlerUnRegister(AMBA_DSP_EVENT_VIDEO_DEC_STATUS, VideoDecCB);
            if (Err != PLAYER_OK) {
                AmbaPrint_ModulePrintUInt5(AMBA_PLAYER_PRINT_MODULE_ID, "AmbaDSP_EventHandlerUnRegister failed (%u)", Err, 0U, 0U, 0U, 0U);
                Rval = PLAYER_ERROR_GENERAL_ERROR;
            }

            Err = AmbaDSP_EventHandlerUnRegister(AMBA_DSP_EVENT_VIDEO_DEC_PIC_INFO, VideoPicCB);
            if (Err != PLAYER_OK) {
                AmbaPrint_ModulePrintUInt5(AMBA_PLAYER_PRINT_MODULE_ID, "AmbaDSP_EventHandlerUnRegister failed (%u)", Err, 0U, 0U, 0U, 0U);
                Rval = PLAYER_ERROR_GENERAL_ERROR;
            }
        }
    }

    return Rval;
}

/**
* start player
* @param [in]  PlayerNum num of player
* @param [in]  pPlayerArr[] player handler array
* @return ErrorCode
*/
UINT32 AmbaPlayer_Start(UINT16 PlayerNum, AMBA_PLAYER_s *pPlayerArr[])
{
    UINT32                          Err, Rval = PLAYER_OK, i, ActualFlags = 0U;
    UINT16                          Num = 0U, VDecId[CONFIG_AMBA_PLAYER_MAX_NUM] = {0U};
    AMBA_PLAYER_PLAYER_PRIV_s       *pPlayerPriv;
    AMBA_DSP_VIDDEC_START_CONFIG_s  VCfg[CONFIG_AMBA_PLAYER_MAX_NUM] = {0U};
    ULONG                           TmpUL;

    AmbaMisra_TouchUnused(&Rval);
    AmbaMisra_TouchUnused(pPlayerArr);

    for (i = 0U; i < PlayerNum; i++) {
        AmbaMisra_TypeCast(&pPlayerPriv, &(pPlayerArr[i]));
        if (0U < CheckBits(PlayerInfo.PlayerBits, ((UINT32)1U << pPlayerPriv->PlayerId))) {
            /* wait preload done */
            if (Rval == PLAYER_OK) {
                Err = AmbaKAL_EventFlagGet(&(pPlayerPriv->Flag), AMBA_PLAYER_FLG_PRELOAD_DONE, AMBA_KAL_FLAGS_ALL,
                                           AMBA_KAL_FLAGS_CLEAR_NONE, &ActualFlags, 5000U);
                if (Err != KAL_ERR_NONE) {
                    AmbaPrint_ModulePrintUInt5(AMBA_PLAYER_PRINT_MODULE_ID, "AmbaKAL_EventFlagGet failed(%u/%u)", AMBA_PLAYER_FLG_PRELOAD_DONE, Err, 0U, 0U, 0U);
                    Rval = PLAYER_ERROR_GENERAL_ERROR;
                    break;
                }
            }

            /* video */
            if (Rval == PLAYER_OK) {
                TmpUL                     = pPlayerPriv->Video.BitsBufUpdp - pPlayerPriv->Video.BitsBufBase;
                VCfg[Num].PreloadDataSize = (UINT32)TmpUL;
                VCfg[Num].SpeedIndex      = (UINT16)pPlayerPriv->Speed;
                VCfg[Num].Direction       = (UINT16)pPlayerPriv->Direction;
                VCfg[Num].FirstDisplayPTS = ((UINT64)pPlayerPriv->StartTime * 90000ULL) / 1000ULL;
                VDecId[Num]               = (UINT16)pPlayerPriv->Video.DecId;
                AmbaPrint_ModulePrintUInt5(AMBA_PLAYER_PRINT_MODULE_ID, "VPreloadDataSize (%u)", VCfg[Num].PreloadDataSize, 0U, 0U, 0U, 0U);
            }

#if defined(CONFIG_BUILD_COMAMBA_CODEC_AUDIO)
            /* audio dec start */
            if (Rval == PLAYER_OK) {
                if (pPlayerPriv->Audio.Enable == 1U) {
                    AmbaADecFlow_DecStart(pPlayerPriv->Audio.DecId);
                }
            }
#endif
            Num++;
        }
    }

    /* video dec start */
    if (Rval == PLAYER_OK) {
        Err = VDecConfig(PlayerNum, pPlayerArr, 0U);
        if (Err != PLAYER_OK) {
            AmbaPrint_ModulePrintUInt5(AMBA_PLAYER_PRINT_MODULE_ID, "VDecConfig failed (%u)", Err, 0U, 0U, 0U, 0U);
            Rval = PLAYER_ERROR_GENERAL_ERROR;
        }
    }

    if (Rval == PLAYER_OK) {
        Err = AmbaDSP_VideoDecStart(Num, VDecId, VCfg);
        if (Err != PLAYER_OK) {
            AmbaPrint_ModulePrintUInt5(AMBA_PLAYER_PRINT_MODULE_ID, "AmbaDSP_VideoDecStart failed (%u)", Err, 0U, 0U, 0U, 0U);
            Rval = PLAYER_ERROR_GENERAL_ERROR;
        }
    }

#if defined(CONFIG_BUILD_COMAMBA_CODEC_AUDIO)
    /* audio output start */
    if (Rval == PLAYER_OK) {
        for (i = 0U; i < PlayerNum; i++) {
            AmbaMisra_TypeCast(&pPlayerPriv, &(pPlayerArr[i]));
            if (0U < CheckBits(PlayerInfo.PlayerBits, ((UINT32)1U << pPlayerPriv->PlayerId))) {
                /* wait preload done */
                if (pPlayerPriv->Audio.Enable == 1U) {
                    Err = AmbaKAL_EventFlagGet(&(pPlayerPriv->Flag), AMBA_PLAYER_FLG_VDEC_START, AMBA_KAL_FLAGS_ALL,
                                                AMBA_KAL_FLAGS_CLEAR_NONE, &ActualFlags, 5000U);
                    if (Err != KAL_ERR_NONE) {
                        AmbaPrint_ModulePrintUInt5(AMBA_PLAYER_PRINT_MODULE_ID, "AmbaKAL_EventFlagGet failed(%u/%u)", AMBA_PLAYER_FLG_VDEC_START, Err, 0U, 0U, 0U);
                        Rval = PLAYER_ERROR_GENERAL_ERROR;
                        break;
                    }
                    AmbaADecFlow_OupStart(pPlayerPriv->Audio.DecId, pPlayerPriv->pCbAoutCtrl);
                }
            }
        }
    }
#endif

    return Rval;
}

/**
* stop player
* @param [in]  PlayerNum num of player
* @param [in]  pPlayerArr[] player handler array
* @return ErrorCode
*/
UINT32 AmbaPlayer_Stop(UINT16 PlayerNum, AMBA_PLAYER_s *pPlayerArr[])
{
    UINT32                          Err, Rval = PLAYER_OK, i;
    UINT16                          Num = 0U, VDecId[CONFIG_AMBA_PLAYER_MAX_NUM] = {0U};
    UINT8                           ShowLFrame[CONFIG_AMBA_PLAYER_MAX_NUM];
    AMBA_PLAYER_PLAYER_PRIV_s       *pPlayerPriv;

    AmbaMisra_TouchUnused(&Rval);
    AmbaMisra_TouchUnused(&pPlayerPriv);
    AmbaMisra_TouchUnused(pPlayerArr);

    Err = AmbaWrap_memset(ShowLFrame, 0, sizeof(ShowLFrame));
    if (Err != 0U) {
        AmbaPrint_ModulePrintUInt5(AMBA_PLAYER_PRINT_MODULE_ID, "AmbaWrap_memset failed (%u)", Err, 0U, 0U, 0U, 0U);
    }

    for (i = 0U; i < PlayerNum; i++) {
        AmbaMisra_TypeCast(&pPlayerPriv, &(pPlayerArr[i]));
        if (0U < CheckBits(PlayerInfo.PlayerBits, ((UINT32)1U << pPlayerPriv->PlayerId))) {

            /* video */
            if (Rval == PLAYER_OK) {
                VDecId[Num] = (UINT16)pPlayerPriv->Video.DecId;

                Err = AmbaKAL_EventFlagSet(&(pPlayerPriv->Flag), AMBA_PLAYER_FLG_VDEC_STOP);
                if (Err != PLAYER_OK) {
                    AmbaPrint_ModulePrintUInt5(AMBA_PLAYER_PRINT_MODULE_ID, "AmbaKAL_EventFlagClear failed (%u)", Err, 0U, 0U, 0U, 0U);
                    Rval = PLAYER_ERROR_GENERAL_ERROR;
                }

                if ((pPlayerPriv->Mov.VideoTrack[pPlayerPriv->Video.TrackId].Info.MediaId == SVC_FORMAT_MID_H265) ||
                    (pPlayerPriv->Mov.VideoTrack[pPlayerPriv->Video.TrackId].Info.MediaId == SVC_FORMAT_MID_HVC)) {
                }
            }

#if defined(CONFIG_BUILD_COMAMBA_CODEC_AUDIO)
            /* audio */
            if (pPlayerPriv->Audio.Enable == 1U) {
                AmbaADecFlow_Stop(pPlayerPriv->Audio.DecId, 0U, pPlayerPriv->pCbAoutCtrl);
            }
#endif
            Num++;
        }
    }

    /* video dec stop */
    if (Rval == PLAYER_OK) {
        Err = AmbaDSP_VideoDecStop(Num, VDecId, ShowLFrame);
        if (Err != PLAYER_OK) {
            AmbaPrint_ModulePrintUInt5(AMBA_PLAYER_PRINT_MODULE_ID, "AmbaDSP_VideoDecStop failed (%u)", Err, 0U, 0U, 0U, 0U);
            Rval = PLAYER_ERROR_GENERAL_ERROR;
        }
    }

    if (Rval == PLAYER_OK) {
        UINT32 ActualFlags = 0U;
        Err = AmbaKAL_EventFlagGet(&(pPlayerPriv->Flag), (AMBA_PLAYER_FLG_VDEC_IDLE | AMBA_PLAYER_FLG_VDEC_EOS), AMBA_KAL_FLAGS_ANY,
                                    AMBA_KAL_FLAGS_CLEAR_NONE, &ActualFlags, 5000U);
        if (Err != KAL_ERR_NONE) {
            AmbaPrint_ModulePrintUInt5(AMBA_PLAYER_PRINT_MODULE_ID, "AmbaKAL_EventFlagGet failed(%u/%u)", ActualFlags, Err, 0U, 0U, 0U);
            Rval = PLAYER_ERROR_GENERAL_ERROR;
        }
    }

    return Rval;
}

/**
* decode one frame (still decode)
* @param [in]  pPlayer player handler
* @param [out]  pInfo decode info
* @return ErrorCode
*/
UINT32 AmbaPlayer_DecOneFrame(AMBA_PLAYER_s *pPlayer, AMBA_DSP_STLDEC_STATUS_s *pInfo)
{
    UINT32                          Err, Rval = PLAYER_OK, ActualFlags = 0U;
    static UINT32                   IsRegister = 0U;
    AMBA_PLAYER_PLAYER_PRIV_s       *pPlayerPriv;

    AmbaMisra_TouchUnused(&Rval);
    AmbaMisra_TouchUnused(pPlayer);
    AmbaMisra_TypeCast(&pPlayerPriv, &pPlayer);

    if (Rval == PLAYER_OK) {
        if (pPlayerPriv->FeedType != AMBA_PLAYER_FTYPE_FRAME) {
            AmbaPrint_ModulePrintUInt5(AMBA_PLAYER_PRINT_MODULE_ID, "wrong FeedType (%u)", pPlayerPriv->StartTime, 0U, 0U, 0U, 0U);
            Rval = PLAYER_ERROR_GENERAL_ERROR;
        }
    }

    /* register still dec event handler */
    if (Rval == PLAYER_OK) {
        Err = AmbaDSP_EventHandlerRegister(AMBA_DSP_EVENT_STILL_DEC_STATUS, StillDecCB);
        if (Err != PLAYER_OK) {
            AmbaPrint_ModulePrintUInt5(AMBA_PLAYER_PRINT_MODULE_ID, "AmbaDSP_EventHandlerRegister failed (%u)", Err, 0U, 0U, 0U, 0U);
            Rval = PLAYER_ERROR_GENERAL_ERROR;
        } else {
            IsRegister = 1U;
        }
    }

    {
        ULONG AlignStart, AlignSize;

        AlignStart = PlayerInfo.BitsBuf[pPlayerPriv->PlayerId].VBitsBufBase & AMBA_CACHE_LINE_MASK;
        AlignSize  =  Alignment(AlignStart + PlayerInfo.BitsBuf[pPlayerPriv->PlayerId].VBitsBufSize, (UINT32)AMBA_CACHE_LINE_SIZE) - AlignStart;
        Err = AmbaSvcWrap_CacheClean(AlignStart, AlignSize);
        if (Err != PLAYER_OK) {
            AmbaPrint_ModulePrintUInt5(AMBA_PLAYER_PRINT_MODULE_ID, "AmbaSvcWrap_CacheClean failed (%u)", Err, 0U, 0U, 0U, 0U);
        }
    }

    /* feed frame to bits buff */
    if (Rval == PLAYER_OK) {
        Err = AmbaDmux_FeedFrame(&(pPlayerPriv->DmxCtrl), (UINT8)pPlayerPriv->Video.TrackId, pPlayerPriv->StartTime, SVC_FIFO_TYPE_IDR_FRAME);
        if (Err != PLAYER_OK) {
            AmbaPrint_ModulePrintUInt5(AMBA_PLAYER_PRINT_MODULE_ID, "AmbaDmux_FeedFrame failed (%u)", Err, 0U, 0U, 0U, 0U);
            Rval = PLAYER_ERROR_GENERAL_ERROR;
        }
#if defined (CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)
        VideoEndOfSequence(pPlayerPriv);
#endif
    }

    /* start still dec */
    if (Rval == PLAYER_OK) {
        AMBA_DSP_STLDEC_START_CONFIG_s  DecConfig;
        const SVC_VIDEO_TRACK_INFO_s    *pTrack;
        ULONG                           TmpUL;

        pTrack  = &(pPlayerPriv->Mov.VideoTrack[pPlayerPriv->Video.TrackId]);

        if ((pTrack->Info.MediaId == SVC_FORMAT_MID_H264) || (pTrack->Info.MediaId == SVC_FORMAT_MID_AVC)) {
            DecConfig.BitsFormat = AMBA_DSP_DEC_BITS_FORMAT_H264;
        } else if ((pTrack->Info.MediaId == SVC_FORMAT_MID_H265) || (pTrack->Info.MediaId == SVC_FORMAT_MID_HVC)){
            DecConfig.BitsFormat = AMBA_DSP_DEC_BITS_FORMAT_H265;
        } else if (pTrack->Info.MediaId == SVC_FORMAT_MID_MJPEG){
            DecConfig.BitsFormat = AMBA_DSP_DEC_BITS_FORMAT_JPEG;
        } else {
            AmbaPrint_ModulePrintUInt5(AMBA_PLAYER_PRINT_MODULE_ID, "Unknown media id (%u)", pTrack->Info.MediaId, 0U, 0U, 0U, 0U);
            Rval = PLAYER_ERROR_GENERAL_ERROR;
        }

        DecConfig.BitsAddr    = PlayerInfo.BitsBuf[pPlayerPriv->PlayerId].VBitsBufBase;
        TmpUL                 = pPlayerPriv->Video.BitsBufUpdp - DecConfig.BitsAddr;
        DecConfig.BitsSize    = (UINT32)TmpUL;
        DecConfig.YuvBufAddr  = PlayerInfo.BitsBuf[pPlayerPriv->PlayerId].YuvBufBase;
        DecConfig.YuvBufSize  = PlayerInfo.BitsBuf[pPlayerPriv->PlayerId].YuvBufSize;

#if defined (CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)
        DecConfig.ImageWidth       = pTrack->Width;
        DecConfig.ComplianceOption = 1U;
#endif
        {
            ULONG AlignStart, AlignSize;

            AlignStart = DecConfig.BitsAddr & AMBA_CACHE_LINE_MASK;
            AlignSize  =  Alignment(DecConfig.BitsAddr + DecConfig.BitsSize, (UINT32)AMBA_CACHE_LINE_SIZE) - AlignStart;
            Err = AmbaSvcWrap_CacheClean(AlignStart, AlignSize);
            if (Err != PLAYER_OK) {
                AmbaPrint_ModulePrintUInt5(AMBA_PLAYER_PRINT_MODULE_ID, "AmbaSvcWrap_CacheClean failed (%u)", Err, 0U, 0U, 0U, 0U);
            }

            AlignStart = DecConfig.YuvBufAddr & AMBA_CACHE_LINE_MASK;
            AlignSize  =  Alignment(DecConfig.YuvBufAddr + DecConfig.YuvBufSize, (UINT32)AMBA_CACHE_LINE_SIZE) - AlignStart;
            Err = AmbaSvcWrap_CacheFlush(AlignStart, AlignSize);
            if (Err != PLAYER_OK) {
                AmbaPrint_ModulePrintUInt5(AMBA_PLAYER_PRINT_MODULE_ID, "AmbaSvcWrap_CacheClean failed (%u)", Err, 0U, 0U, 0U, 0U);
            }
        }

        Err = AmbaDSP_StillDecStart((UINT16)pPlayerPriv->Video.DecId, &DecConfig);
        if (Err != PLAYER_OK) {
            AmbaPrint_ModulePrintUInt5(AMBA_PLAYER_PRINT_MODULE_ID, "AmbaDSP_StillDecStart failed (%u)", Err, 0U, 0U, 0U, 0U);
            Rval = PLAYER_ERROR_GENERAL_ERROR;
        }
    }

    /* wait still dec done */
    if (Rval == PLAYER_OK) {
        Err = AmbaKAL_EventFlagGet(&(pPlayerPriv->Flag), AMBA_PLAYER_FLG_STLDEC_DONE, AMBA_KAL_FLAGS_ALL,
                                    KAL_FLAGS_CLEAR_AUTO, &ActualFlags, 10000U);
        if (Err != KAL_ERR_NONE) {
            AmbaPrint_ModulePrintUInt5(AMBA_PLAYER_PRINT_MODULE_ID, "AmbaKAL_EventFlagGet failed(%u/%u)", ActualFlags, Err, 0U, 0U, 0U);
            Rval = PLAYER_ERROR_GENERAL_ERROR;
        }
    }

    /* copy still dec info */
    if (Rval == PLAYER_OK) {
        *pInfo = pPlayerPriv->StillDecInfo;
    }

    /* unregister still dec event handler */
    if (IsRegister == 1U) {
        Err = AmbaDSP_EventHandlerUnRegister(AMBA_DSP_EVENT_STILL_DEC_STATUS, StillDecCB);
        if (Err != PLAYER_OK) {
            AmbaPrint_ModulePrintUInt5(AMBA_PLAYER_PRINT_MODULE_ID, "AmbaDSP_EventHandlerUnRegister failed (%u)", Err, 0U, 0U, 0U, 0U);
            Rval = PLAYER_ERROR_GENERAL_ERROR;
        } else {
            IsRegister = 0U;
        }
    }

    return Rval;
}

/**
* player control
* @param [in]  pPlayer player handler
* @param [in]  CtrlType
* @param [in]  pParam Param
* @return ErrorCode
*/
UINT32 AmbaPlayer_Control(AMBA_PLAYER_s *pPlayer, UINT32 CtrlType, void *pParam)
{
    UINT32                         Rval = PLAYER_OK;
    const AMBA_PLAYER_DISP_CTRL_s  *pDisp;
    AMBA_PLAYER_PLAYER_PRIV_s      *pPlayerPriv;

    AmbaMisra_TouchUnused(pParam);
    AmbaMisra_TypeCast(&pPlayerPriv, &pPlayer);
    AmbaMisra_TouchUnused(pPlayerPriv);

    switch (CtrlType) {
        case AMBA_PLAYER_CTRL_INFO_GET:
            {
                UINT32 *pPlayerBits;

                AmbaMisra_TypeCast(&pPlayerBits, &pParam);
                *pPlayerBits = PlayerInfo.PlayerBits;
            }
            break;
        case AMBA_PLAYER_CTRL_TEXT_BITSINFO_GET:
            if (pPlayerPriv->Text.Enable == 1U) {
                AMBA_PLAYER_BITSBUF_INFO_s *pInfo;

                AmbaMisra_TypeCast(&pInfo, &pParam);
                pInfo->BufBase = pPlayerPriv->Text.BitsBufBase;
                pInfo->BufSize = pPlayerPriv->Text.BitsBufSize;
                pInfo->Rp      = pPlayerPriv->Text.BitsBufRp;
                pInfo->Wp      = pPlayerPriv->Text.BitsBufWp;
            } else {
                Rval = PLAYER_ERROR_GENERAL_ERROR;
            }
            break;
        case AMBA_PLAYER_CTRL_DISP_CTRL:
            {
                AmbaMisra_TypeCast(&pDisp, &pParam);
                Rval = AmbaPlayer_DispCtrl(pPlayer, pDisp);
            }
            break;
        case AMBA_PLAYER_CTRL_SEAMLESS:
            {
                char *FileName = NULL;
                AmbaMisra_TouchUnused(FileName);

                AmbaMisra_TypeCast(&FileName, &pParam);
                Rval = SeamlessStart(pPlayer, FileName);
            }
            break;
        default:
            {
                AmbaPrint_ModulePrintUInt5(AMBA_PLAYER_PRINT_MODULE_ID, "Invalid CtrlType (%u)", CtrlType, 0U, 0U, 0U, 0U);
                Rval = PLAYER_ERROR_ARG;
            }
            break;
    }

    return Rval;
}

/**
* trickplay
* @param [in]  PlayerNum num of player
* @param [in]  pPlayerArr[] player handler array
* @param [in]  Operation operation
* @return ErrorCode
*/
UINT32 AmbaPlayer_TrickPlay(UINT16 PlayerNum, AMBA_PLAYER_s *pPlayerArr[], UINT32 Operation)
{
    UINT32                          Err, Rval = PLAYER_OK;
    UINT16                          Num = 0U, i, VDecId[CONFIG_AMBA_PLAYER_MAX_NUM] = {0U};
    AMBA_PLAYER_PLAYER_PRIV_s       *pPlayerPriv;
    AMBA_DSP_VIDDEC_TRICKPLAY_s     Trick[CONFIG_AMBA_PLAYER_MAX_NUM] = {0U};

    AmbaMisra_TouchUnused(pPlayerArr);

    for (i = 0U; i < PlayerNum; i++) {
        AmbaMisra_TypeCast(&pPlayerPriv, &(pPlayerArr[i]));
        if (0U < CheckBits(PlayerInfo.PlayerBits, ((UINT32)1U << pPlayerPriv->PlayerId))) {
            switch (Operation) {
                case AMBA_PLAYER_TRICK_PAUSE:
                    Trick[Num].Operation = AMBA_DSP_VIDDEC_PAUSE;
                    pPlayerPriv->Pause   = 1U;
#if defined(CONFIG_BUILD_COMAMBA_CODEC_AUDIO)
                    if (pPlayerPriv->Audio.Enable == 1U) {
                        AmbaADecFlow_OupPause(pPlayerPriv->Audio.DecId, pPlayerPriv->pCbAoutCtrl);

                        if (pPlayerPriv->pCbMuteCtrl != NULL) {
                            pPlayerPriv->pCbMuteCtrl(pPlayerPriv->AudioCodecIndex, 1U);
                        }
                    }
#endif
                    break;
                case AMBA_PLAYER_TRICK_RESUME:
                    Trick[Num].Operation        = AMBA_DSP_VIDDEC_RESUME;
                    Trick[Num].ResumeDirection  = (UINT8)pPlayerPriv->Direction;
                    Trick[Num].ResumeSpeedIndex = (UINT8)pPlayerPriv->Speed;
                    pPlayerPriv->Pause          = 0U;
#if defined(CONFIG_BUILD_COMAMBA_CODEC_AUDIO)
                    if (pPlayerPriv->Audio.Enable == 1U) {
                        AmbaADecFlow_OupResume(pPlayerPriv->Audio.DecId, pPlayerPriv->pCbAoutCtrl);

                        if (pPlayerPriv->pCbMuteCtrl != NULL) {
                            pPlayerPriv->pCbMuteCtrl(pPlayerPriv->AudioCodecIndex, 0U);
                        }
                    }
#endif
                    break;
                case AMBA_PLAYER_TRICK_STEP:
                    Trick[Num].Operation = AMBA_DSP_VIDDEC_STEP;
#if defined(CONFIG_BUILD_COMAMBA_CODEC_AUDIO)
                    if (pPlayerPriv->Audio.Enable == 1U) {
                        AMBA_ADEC_FLOW_INFO_s           ADecInfo;
                        const SVC_MEDIA_TRACK_GENERAL_INFO_s  *pVTrack = &(pPlayerPriv->Mov.VideoTrack[pPlayerPriv->Video.TrackId].Info);
                        const SVC_MEDIA_TRACK_GENERAL_INFO_s  *pATrack = &(pPlayerPriv->Mov.AudioTrack[pPlayerPriv->Audio.TrackId - pPlayerPriv->Mov.VideoTrackCount].Info);
                        UINT64                          Temp;
                        UINT32                          Frame;

                        Temp  = (UINT64)pATrack->TimeScale * (UINT64)pVTrack->TimePerFrame * pPlayerPriv->DispCount;
                        Temp /= ((UINT64)pVTrack->TimeScale * (UINT64)pATrack->TimePerFrame);
                        Frame = (UINT32)Temp;

                        AmbaADecFlow_InfoGet(&ADecInfo);

                        if (ADecInfo.pAoutInfo->OutputCnt < Frame) {
                            AmbaADecFlow_OupResume(pPlayerPriv->Audio.DecId, pPlayerPriv->pCbAoutCtrl);
                            Err = AmbaADecFlow_WaitEvent(AMBA_ADEC_EVENT_OUTPUT, &Frame);
                            if (Err != PLAYER_OK) {
                                AmbaPrint_ModulePrintUInt5(AMBA_PLAYER_PRINT_MODULE_ID, "AmbaADecFlow_WaitEvent failed (%u)", Err, 0U, 0U, 0U, 0U);
                                Rval = PLAYER_ERROR_GENERAL_ERROR;
                            }
                            AmbaADecFlow_OupPause(pPlayerPriv->Audio.DecId, pPlayerPriv->pCbAoutCtrl);
                        }
                    }
#endif
                    break;
                default:
                    AmbaPrint_ModulePrintUInt5(AMBA_PLAYER_PRINT_MODULE_ID, "Invalid Operation (%u)", Operation, 0U, 0U, 0U, 0U);
                    Rval = PLAYER_ERROR_ARG;
                    break;
            }
            VDecId[Num] = (UINT16)pPlayerPriv->Video.DecId;
            Num++;
        }
    }

    if (Rval == PLAYER_OK) {
        Err = AmbaDSP_VideoDecTrickPlay(Num, VDecId, Trick);
        if (Err != PLAYER_OK) {
            AmbaPrint_ModulePrintUInt5(AMBA_PLAYER_PRINT_MODULE_ID, "AmbaDSP_VideoDecTrickPlay failed (%u)", Err, 0U, 0U, 0U, 0U);
            Rval = PLAYER_ERROR_GENERAL_ERROR;
        }
    }

    return Rval;
}
