/**
 * @file SvcMp4Fmt.c
 *
 * Copyright (c) 2019 Ambarella International LP
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
 */

#include <AmbaWrap.h>
#include <AmbaMisraFix.h>
#include <AmbaUtility.h>
#include <AmbaDef.h>
#include <AmbaKAL.h>
#include <AmbaDSP_VideoEnc.h>
#include <AmbaDSP_EventInfo.h>
#include <AmbaAudio_AENC.h>

#include "AmbaMux.h"
#include "AmbaRscData.h"

#include "format/SvcFmp4Mux.h"

#include "SvcFIFO.h"
#include "stream/BufferStream.h"

#include "WrpFormatLib.h"

static inline void Wrapper_Perror(const char *Func, UINT32 Line, const char *Message)
{
    char LineStr[8];
    (void)AmbaUtility_UInt32ToStr(LineStr, (UINT32)sizeof(LineStr), Line, 10);\
    AmbaPrint_PrintStr5("[Error]%s:%s %s", Func, LineStr, Message, NULL, NULL);
}

#define WRP_FMT_MAX_NUM (16U)               /**< Maximum number of format handlers */

#define WRP_FMT_TRACK_MAX TRACK_TYPE_MAX    /**< Maximum tracks per format */

#if 0
#define SVC_FMT_STATE_INIT          (0U)
#endif
#define SVC_FMT_STATE_READY         (1U)    /**< State after creation */
#define SVC_FMT_STATE_RUN           (2U)    /**< State being processed */
#define SVC_FMT_STATE_REACH_GOP     (3U)    /**< State after FLUSH_REACH_GOP*/
#define SVC_FMT_STATE_REACH_LIMIT   (4U)    /**< State after FLUSH_REACH_LIMIT */
#define SVC_FMT_STATE_END           (5U)    /**< State after FLUSH_REACH_END */

#define ATOM_HDR_SIZE               (8U)    /**< Atom Size(4) + Atom Type(4) */

#define SAMPLE_DESC_TOLERANCE       (50U)   /**< Sample descriptors tolerance 50% */
#define SAMPLE_BRATE_TOLERANCE      (10U)   /**< Sample bitrate tolerance 10% */

/**
 *  Track information
 */
typedef struct {
    UINT8 TrackType;            /**< Track type */
    UINT8 MediaId;              /**< Media id */
    UINT8 Eos;                  /**< Eos flag */
    UINT64 FrameCnt;            /**< Frame counter */
    UINT32 NumEntries;          /**< Number of entries */
    UINT32 SkipCnt;             /**< number of frames to be skipped */
    SVC_FIFO_HDLR_s *BaseFifo;  /**< Base fifo */
    SVC_FIFO_HDLR_s *Fifo;      /**< Virtual fifo */
} WRP_TRACK_INFO_s;

/**
 *  Format information
 */
typedef struct {
    UINT8 State;            /**< State of the format handler */
    UINT8 FormatType;       /**< Format type */
    UINT8 TrackCount;       /**< Track counter */
    UINT64 WrittenSize;     /**< Written size */
    UINT64 RefVideoPts;             /**< Reference video PTS (Last IDI/I/P's PTS), for CheckVideoDesc */
    UINT64 InitVideoPts;            /**< Init video PTS, for CheckVideoDesc */
    UINT64 LastVideoPts;            /**< Last video PTS, for CheckVideoDesc */
    UINT64 LastVideoCaptureTime;    /**< Last video CaptureTimeStamp, for CheckVideoDesc */
    MP4FMT_CTRL_s *pCtrl;   /**< Also indicates if the hdlr is used */
    WRP_TRACK_INFO_s TrackInfo[WRP_FMT_TRACK_MAX];  /**< Track information */
    SVC_MUX_FORMAT_HDLR_s *Hdlr;    /**< Format handler */
    SVC_MOVIE_INFO_s Movie; /**< Movie information */
    AMBA_KAL_MUTEX_t Mutex; /**< Mutex */
} WRP_FORMAT_INFO_s;

/**
 *  Muxer information
 */
typedef struct {
    WRP_FORMAT_INFO_s FormatInfo[WRP_FMT_MAX_NUM];  /**< Format information */
} WRP_MUXER_INFO_s;

static WRP_MUXER_INFO_s g_MuxerInfo;// GNU_SECTION_NOZEROINIT;

static UINT32 FifoCB(const SVC_FIFO_HDLR_s *Hdlr, UINT8 Event, SVC_FIFO_CALLBACK_INFO_s *Info)
{
    AmbaMisra_TouchUnused(&Hdlr);
    AmbaMisra_TouchUnused(&Event);
    AmbaMisra_TouchUnused(&Info);
    AmbaMisra_TouchUnused(&Info->Desc);
    return OK;
}

static UINT32 CreateMutex(void)
{
    static char MutexName[WRP_FMT_MAX_NUM][16];
    UINT32 Rval = AMBA_MP4FMT_ERR_FATAL;
    UINT32 i;
    for (i = 0; i < WRP_FMT_MAX_NUM; i++) {
        WRP_FORMAT_INFO_s *FormatInfo = &g_MuxerInfo.FormatInfo[i];
        UINT32 Len = AmbaUtility_StringPrintUInt32(MutexName[i], (UINT32)sizeof(MutexName[i]), "Fmp4Wrp%02u", 1, &i);
        Rval = K2W(AmbaKAL_MutexCreate(&FormatInfo->Mutex, MutexName[i]));
        if (Rval != OK) {
            Wrapper_Perror(__func__, __LINE__, NULL);
            break;
        }
        AmbaMisra_TouchUnused(&Len);
    }
    return Rval;
}

/**
 *  initialize muxer.
 *
 *  @return 0 - OK, others - AMBA_MP4FMT_ERR_XXXX
 */
UINT32 AmbaMux_Init(void)
{
#define FMP4MUX_INIT_BUFFER_SIZE    (3145728)   /**< Fmp4 init buffer size */
    static UINT8 Fmp4Buffer[FMP4MUX_INIT_BUFFER_SIZE] GNU_SECTION_NOZEROINIT GNU_ALIGNED_CACHESAFE;
    static UINT8 IsInit = 0U;
    UINT32 Rval = OK;
    if (IsInit == 0U) {
        SVC_FMP4_MUX_INIT_CFG_s Fmp4InitCfg;

        Rval = WRAP2W(AmbaWrap_memset(&g_MuxerInfo, 0, sizeof(g_MuxerInfo)));  // State = SVC_FMT_STATE_INIT;
        if (Rval == OK) {
            /** Fragment MP4 Mux Init */
            Rval = M2W(SvcFmp4Mux_GetInitDefaultCfg(&Fmp4InitCfg));
            if (Rval == OK) {
                Fmp4InitCfg.MaxHdlr = WRP_FMT_MAX_NUM;
                Rval = M2W(SvcFmp4Mux_GetInitBufferSize(Fmp4InitCfg.MaxHdlr, &Fmp4InitCfg.BufferSize));
                if (Rval == OK) {
                    AmbaPrint_PrintUInt5("Fmp4Mux: %u / %u", Fmp4InitCfg.BufferSize, (UINT32)sizeof(Fmp4Buffer), 0, 0, 0);
                    if (Fmp4InitCfg.BufferSize < sizeof(Fmp4Buffer)) {
                        Fmp4InitCfg.Buffer = Fmp4Buffer;
                        Rval = M2W(SvcFmp4Mux_Init(&Fmp4InitCfg));
                        if (Rval == OK) {
                            Rval = S2W(SvcBufferStream_Init());
                            if (Rval == OK) {
                                Rval = CreateMutex();
                            } else {
                                Wrapper_Perror(__func__, __LINE__, NULL);
                            }
                        } else {
                            Wrapper_Perror(__func__, __LINE__, NULL);
                        }
                    } else {
                        Wrapper_Perror(__func__, __LINE__, NULL);
                        Rval = AMBA_MP4FMT_ERR_FATAL;
                    }
                } else {
                    Wrapper_Perror(__func__, __LINE__, NULL);
                }
            } else {
                Wrapper_Perror(__func__, __LINE__, NULL);
            }
        }
        IsInit = 1U;
    }
    return Rval;
}

static inline UINT32 GetFormat(WRP_FORMAT_INFO_s *FormatInfo)
{
    UINT32 Rval = K2W(AmbaKAL_MutexTake(&FormatInfo->Mutex, AMBA_KAL_WAIT_FOREVER));
    if (Rval != OK) {
        Wrapper_Perror(__func__, __LINE__, "AmbaKAL_MutexTake failed!");
    }
    return Rval;
}

static inline UINT32 GiveFormat(WRP_FORMAT_INFO_s *FormatInfo)
{
    UINT32 Rval = K2W(AmbaKAL_MutexGive(&FormatInfo->Mutex));
    if (Rval != OK) {
        Wrapper_Perror(__func__, __LINE__, "AmbaKAL_MutexGive failed!");
    }
    return Rval;
}

static UINT32 SvcFmp4Fmt_Create(MP4FMT_CTRL_s *pCtrl, WRP_FORMAT_INFO_s **FormatInfo)
{
    UINT32 Rval = AMBA_MP4FMT_ERR_FATAL;
    UINT8 i;
    for (i = 0; i < WRP_FMT_MAX_NUM; i++) {
        WRP_FORMAT_INFO_s *Fmt = &g_MuxerInfo.FormatInfo[i];
        Rval = K2W(AmbaKAL_MutexTake(&Fmt->Mutex, AMBA_KAL_WAIT_FOREVER));
        if (Rval == OK) {
            if (Fmt->pCtrl == NULL) {   // find unused FormatInfo
                SVC_STREAM_HDLR_s *Stream;
                SVC_BUFFER_STREAM_CFG_s StreamCfg = {pCtrl->pfnBufCopy};
                Rval = S2W(SvcBufferStream_Create(&StreamCfg, &Stream));
                if (Rval == OK) {
                    SVC_FMP4_MUX_CFG_s Fmp4Cfg;
                    Rval = M2W(SvcFmp4Mux_GetDefaultCfg(&Fmp4Cfg));
                    if (Rval == OK) {
                        Fmp4Cfg.Stream           = Stream;
                        Fmp4Cfg.UserData         = pCtrl->Mp4Hdr.UserData;
                        Fmp4Cfg.UserDataSize     = pCtrl->Mp4Hdr.UserDataSize;
                        Fmp4Cfg.EnableMehd       = pCtrl->Mp4Hdr.MehdCtrl.Enable;
                        Fmp4Cfg.MehdDefDuration  = pCtrl->Mp4Hdr.MehdCtrl.DefDuration;
                        Fmp4Cfg.MvhdTimeScale    = pCtrl->Mp4Hdr.MehdCtrl.TimeScale;
                        Rval = M2W(SvcFmp4Mux_Create(&Fmp4Cfg, &Fmt->Hdlr));
                        if (Rval == OK) {
                            if (Fmp4Cfg.UserDataSize > 0U) {
                                Rval = M2W(Fmt->Hdlr->Func->SetUserData(Fmt->Hdlr, Fmp4Cfg.UserDataSize, Fmp4Cfg.UserData));
                            }
                            if (Rval == OK) {
                                Fmt->pCtrl = pCtrl;
                                *FormatInfo = Fmt;
                            } else {
                                Wrapper_Perror(__func__, __LINE__, NULL);
                            }
                        } else {
                            Wrapper_Perror(__func__, __LINE__, NULL);
                        }
                    } else {
                        Wrapper_Perror(__func__, __LINE__, NULL);
                    }
                } else {
                    Wrapper_Perror(__func__, __LINE__, NULL);
                }
            } else {
                Rval = K2W(AmbaKAL_MutexGive(&Fmt->Mutex));
                if (Rval != OK) {
                    Wrapper_Perror(__func__, __LINE__, "AmbaKAL_MutexGive failed!");
                }
            }
        } else {
            Wrapper_Perror(__func__, __LINE__, "AmbaKAL_MutexTake failed!");
        }
        if ((*FormatInfo != NULL) || (Rval != OK)) {
            break;
        }
    }
    if (i == WRP_FMT_MAX_NUM) {
        Rval = AMBA_MP4FMT_ERR_FATAL;
        Wrapper_Perror(__func__, __LINE__, "Too many handlers!");
    }
    return Rval;
}

static UINT32 SvcFmp4Fmt_Delete(WRP_FORMAT_INFO_s *FormatInfo)
{
    SVC_MUX_FORMAT_HDLR_s *Format = FormatInfo->Hdlr;
    SVC_STREAM_HDLR_s *Stream = Format->Stream;
    UINT32 Rval = M2W(SvcFmp4Mux_Delete(Format));
    if (Rval == OK) {
        Rval = S2W(SvcBufferStream_Delete(Stream));
        if (Rval == OK) {
            UINT8 i;
            for (i = 0; i < WRP_FMT_TRACK_MAX; i++) {
                WRP_TRACK_INFO_s *TrackInfo = &FormatInfo->TrackInfo[i];
                if (TrackInfo->Fifo != NULL) {
                    Rval = F2W(SvcFIFO_Reset(TrackInfo->Fifo));
                    if (Rval == OK) {
                        Rval = F2W(SvcFIFO_Delete(TrackInfo->Fifo));
                        if (Rval == OK) {
                            Rval = F2W(SvcFIFO_Reset(TrackInfo->BaseFifo));
                            if (Rval == OK) {
                                Rval = F2W(SvcFIFO_Delete(TrackInfo->BaseFifo));
                                if (Rval == OK) {
                                    TrackInfo->Fifo = NULL;
                                    TrackInfo->BaseFifo = NULL;
                                } else {
                                    Wrapper_Perror(__func__, __LINE__, NULL);
                                }
                            } else {
                                Wrapper_Perror(__func__, __LINE__, NULL);
                            }
                        } else {
                            Wrapper_Perror(__func__, __LINE__, NULL);
                        }
                    } else {
                        Wrapper_Perror(__func__, __LINE__, NULL);
                    }
                }
                if (Rval != OK) {
                    break;
                }
            }
        } else {
            Wrapper_Perror(__func__, __LINE__, NULL);
        }
    } else {
        Wrapper_Perror(__func__, __LINE__, NULL);
    }
    return Rval;
}

static UINT32 ResetMovieInfo(WRP_FORMAT_INFO_s *FormatInfo, SVC_MOVIE_INFO_s *Movie)
{
    UINT32                          Rval = OK, Err, RemoveCnt;
    UINT64                          MinDTS;
    SVC_MEDIA_TRACK_GENERAL_INFO_s  *Track;
    SVC_FIFO_INFO_s                 FifoInfo;

    /* the init DTS of audio and text tracks should be >= video DTS */
    MinDTS = Movie->VideoTrack[0].Info.DTS;

    if (Movie->VideoTrackCount > 0U) {
        Track = &Movie->VideoTrack[0].Info;
        FormatInfo->TrackInfo[TRACK_TYPE_VIDEO].SkipCnt = 0U;

        while (Track->DTS < MinDTS) {
            Track->DTS += Track->TimePerFrame;
            FormatInfo->TrackInfo[TRACK_TYPE_VIDEO].SkipCnt++;
        }

        Track->DTS = (Track->DTS - MinDTS);
        Track->NextDTS = Track->DTS;
        Track->InitDTS = Track->NextDTS;

        Err = SvcFIFO_GetInfo(Track->Fifo, &FifoInfo);
        if (Err == OK) {
            RemoveCnt = FormatInfo->TrackInfo[TRACK_TYPE_VIDEO].SkipCnt;

            if (RemoveCnt > FifoInfo.AvailEntries) {
                RemoveCnt = FifoInfo.AvailEntries;
            }

            if (RemoveCnt > 0U) {
                Err = SvcFIFO_RemoveEntry(Track->Fifo, RemoveCnt);
                if (Err == OK) {
                    FormatInfo->TrackInfo[TRACK_TYPE_VIDEO].SkipCnt -= RemoveCnt;
                } else {
                    Wrapper_Perror(__func__, __LINE__, NULL);
                }
            }
        }
    }

    if (Movie->AudioTrackCount > 0U) {
        Track = &Movie->AudioTrack[0].Info;
        FormatInfo->TrackInfo[TRACK_TYPE_AUDIO].SkipCnt = 0U;

        while (Track->DTS < MinDTS) {
            Track->DTS += Track->TimePerFrame;
            FormatInfo->TrackInfo[TRACK_TYPE_AUDIO].SkipCnt++;
        }

        Track->DTS = (Track->DTS - MinDTS);
        Track->NextDTS = Track->DTS;
        Track->InitDTS = Track->NextDTS;

        Err = SvcFIFO_GetInfo(Track->Fifo, &FifoInfo);
        if (Err == OK) {
            RemoveCnt = FormatInfo->TrackInfo[TRACK_TYPE_AUDIO].SkipCnt;

            if (RemoveCnt > FifoInfo.AvailEntries) {
                RemoveCnt = FifoInfo.AvailEntries;
            }

            if (RemoveCnt > 0U) {
                Err = SvcFIFO_RemoveEntry(Track->Fifo, RemoveCnt);
                if (Err == OK) {
                    FormatInfo->TrackInfo[TRACK_TYPE_AUDIO].SkipCnt -= RemoveCnt;
                } else {
                    Wrapper_Perror(__func__, __LINE__, NULL);
                }
            }
        }
    }

    if (Movie->TextTrackCount > 0U) {
        Track = &Movie->TextTrack[0].Info;
        FormatInfo->TrackInfo[TRACK_TYPE_DATA].SkipCnt = 0U;

        while (Track->DTS < MinDTS) {
            Track->DTS += Track->TimePerFrame;
            FormatInfo->TrackInfo[TRACK_TYPE_DATA].SkipCnt++;
        }

        Track->DTS = (Track->DTS - MinDTS);
        Track->NextDTS = Track->DTS;
        Track->InitDTS = Track->NextDTS;

        Err = SvcFIFO_GetInfo(Track->Fifo, &FifoInfo);
        if (Err == OK) {
            RemoveCnt = FormatInfo->TrackInfo[TRACK_TYPE_DATA].SkipCnt;

            if (RemoveCnt > FifoInfo.AvailEntries) {
                RemoveCnt = FifoInfo.AvailEntries;
            }

            if (RemoveCnt > 0U) {
                Err = SvcFIFO_RemoveEntry(Track->Fifo, RemoveCnt);
                if (Err == OK) {
                    FormatInfo->TrackInfo[TRACK_TYPE_DATA].SkipCnt -= RemoveCnt;
                } else {
                    Wrapper_Perror(__func__, __LINE__, NULL);
                }
            }
        }
    }

    return Rval;
}

static UINT32 InitMovieInfo(WRP_FORMAT_INFO_s *FormatInfo)
{
    SVC_MOVIE_INFO_s *Movie = &FormatInfo->Movie;
    UINT32 Rval = WRAP2W(AmbaWrap_memset(Movie, 0, sizeof(SVC_MOVIE_INFO_s)));
    Movie->MediaInfo.MediaType = (UINT8)SVC_MEDIA_INFO_MOVIE;
    Movie->MediaInfo.Name[0] = '\0';
    return Rval;
}

static UINT32 CreateFifo(UINT32 NumEntries, WRP_TRACK_INFO_s *TrackInfo, ULONG *BufferBase, UINT32 *BufferSize)
{
    SVC_FIFO_CFG_s FifoCfg;
    UINT32 Rval = F2W(SvcFIFO_GetDefaultCfg(&FifoCfg));
    if (Rval == OK) {
        Rval = F2W(SvcFIFO_GetHdlrBufferSize(NumEntries, &FifoCfg.BufferSize));
        if (Rval == OK) {
            if ((FifoCfg.BufferSize * 2U) <= (*BufferSize)) { // for base + virtaul FIFO
                FifoCfg.NumEntries = NumEntries;
                FifoCfg.CbEvent = FifoCB;
                AmbaMisra_TypeCast(&FifoCfg.BufferAddr, BufferBase);
                FifoCfg.BaseHdlr = NULL;
                Rval = F2W(SvcFIFO_Create(&FifoCfg, &TrackInfo->BaseFifo));
                if (Rval == OK) {
                    *BufferBase += (ULONG)FifoCfg.BufferSize;
                    *BufferSize -= FifoCfg.BufferSize;
                    FifoCfg.BaseHdlr = TrackInfo->BaseFifo;
                    AmbaMisra_TypeCast(&FifoCfg.BufferAddr, BufferBase);
                    Rval = F2W(SvcFIFO_Create(&FifoCfg, &TrackInfo->Fifo));
                    if (Rval == OK) {
                        TrackInfo->NumEntries = NumEntries;
                        *BufferBase += (ULONG)FifoCfg.BufferSize;
                        *BufferSize -= FifoCfg.BufferSize;
                        AmbaPrint_PrintUInt5("CreateFifo(%u) B/V %u/%u NumEntries %u Size%u",
                            TrackInfo->TrackType, TrackInfo->BaseFifo->FifoId, TrackInfo->Fifo->FifoId, NumEntries, FifoCfg.BufferSize);
                    } else {
                        Wrapper_Perror(__func__, __LINE__, NULL);
                    }
                } else {
                    Wrapper_Perror(__func__, __LINE__, NULL);
                }
            } else {
                Rval = AMBA_MP4FMT_ERR_FATAL;
                Wrapper_Perror(__func__, __LINE__, "DescBuffer not enough");
                AmbaPrint_PrintUInt5("NumEntries %u Size %u / %u", NumEntries, FifoCfg.BufferSize, *BufferSize, 0U, 0U);
            }
        } else {
            Wrapper_Perror(__func__, __LINE__, NULL);
        }
    } else {
        Wrapper_Perror(__func__, __LINE__, NULL);
    }
    return Rval;
}

static UINT32 InitVideoTrackInfo(const MP4FMT_CTRL_s *Ctrl, WRP_TRACK_INFO_s *TrackInfo, SVC_MOVIE_INFO_s *MovieInfo)
{
    UINT32 Rval = OK;
    const AMBA_MP4FMT_VIDEO_CFG_s *VideoCfg = &Ctrl->Mp4Hdr.VideoCfg;
    TrackInfo->TrackType = WRP_MEDIA_TRACK_TYPE_VIDEO;
    if (VideoCfg->VideoCoding == AMBA_RSC_VID_SUBTYPE_H264) {
        TrackInfo->MediaId = SVC_FORMAT_MID_AVC;
    } else if (VideoCfg->VideoCoding == AMBA_RSC_VID_SUBTYPE_MJPG) {
        TrackInfo->MediaId = SVC_FORMAT_MID_MJPEG;
    } else if (VideoCfg->VideoCoding == AMBA_RSC_VID_SUBTYPE_HEVC) {
        TrackInfo->MediaId = SVC_FORMAT_MID_HVC;
    } else {
        Rval = AMBA_MP4FMT_ERR_FATAL;
        Wrapper_Perror(__func__, __LINE__, "VideoCoding not supported!");
    }
    TrackInfo->FrameCnt = 0U;
    TrackInfo->Eos = 0U;

    if (Rval == OK) {
        SVC_VIDEO_TRACK_INFO_s *VideoInfo = &MovieInfo->VideoTrack[0];
        MovieInfo->VideoTrackCount = 1U;

        VideoInfo->Info.MediaId = TrackInfo->MediaId;
        VideoInfo->Info.TimeScale = VideoCfg->TimeScale;
        VideoInfo->Info.OrigTimeScale = VideoInfo->Info.TimeScale;
        VideoInfo->Info.TimePerFrame = Ctrl->BufInfo.SampleTicks[TRACK_TYPE_VIDEO];
        VideoInfo->Info.BufferBase = NULL; // set when receiving the descriptor (ok to set before process)
        VideoInfo->Info.BufferSize = 0U; // set when receiving the descriptor (ok to set before process)
        VideoInfo->Info.Fifo = TrackInfo->Fifo;

        VideoInfo->IsDefault = (UINT8)1U;
        VideoInfo->Mode = (UINT8)SVC_VIDEO_MODE_P; /* TBD */
        VideoInfo->M = (UINT16)VideoCfg->M;
        VideoInfo->N = (UINT16)VideoCfg->N;
        VideoInfo->GOPSize = VideoCfg->N * VideoCfg->IdrInterval;
        VideoInfo->Width = (UINT16)VideoCfg->VdWidth;
        VideoInfo->Height = (UINT16)VideoCfg->VdHeight;
        VideoInfo->Rotation = (UINT8)VideoCfg->Orientation;//SVC_ISO_ROTATION_NONE;
#ifdef CONFIG_ENABLE_CAPTURE_TIME_MODE
        if ((VideoCfg->CaptureTimeMode == 1U) && (VideoCfg->M > 1U)) {
            Rval = AMBA_MP4FMT_ERR_FATAL;
            Wrapper_Perror(__func__, __LINE__, "CaptureTimeMode not supported with M > 1!");
        }
        VideoInfo->CaptureTimeMode = VideoCfg->CaptureTimeMode;
        VideoInfo->CaptureTimeScale = VideoCfg->CaptureTimeScale;
#else
        if (VideoCfg->CaptureTimeMode != 0U) {
            Rval = AMBA_MP4FMT_ERR_FATAL;
            Wrapper_Perror(__func__, __LINE__, "CaptureTimeMode not supported!");
        } else {
            VideoInfo->CaptureTimeMode = 0U;
        }
#endif
        VideoInfo->Info.InitDTS = 0U; // Video always comes first! (RecMaster will do the scheduling)
        VideoInfo->Info.NextDTS = VideoInfo->Info.InitDTS;
        VideoInfo->Info.DTS = VideoInfo->Info.NextDTS;
    }
    return Rval;
}

static UINT32 InitAudioTrackInfo(const MP4FMT_CTRL_s *Ctrl, WRP_TRACK_INFO_s *TrackInfo, SVC_MOVIE_INFO_s *MovieInfo, UINT32 InitDelay)
{
    UINT32 Rval = OK;
    const AMBA_MP4FMT_AUDIO_CFG_s *AudioCfg = &Ctrl->Mp4Hdr.AudioCfg;

    TrackInfo->TrackType = WRP_MEDIA_TRACK_TYPE_AUDIO;
    if (AudioCfg->AudioCoding == AMBA_RSC_AUD_SUBTYPE_AAC) {
        TrackInfo->MediaId = (UINT8)SVC_FORMAT_MID_AAC;
    } else if (AudioCfg->AudioCoding == AMBA_RSC_AUD_SUBTYPE_PCM) {
        TrackInfo->MediaId = (UINT8)SVC_FORMAT_MID_PCM;
    } else {
        Rval = AMBA_MP4FMT_ERR_FATAL;
        Wrapper_Perror(__func__, __LINE__, "AudioCoding not supported!");
    }
    TrackInfo->FrameCnt = 0U;
    TrackInfo->Eos = 0U;

    if (Rval == OK) {
        SVC_AUDIO_TRACK_INFO_s *AudioInfo = &MovieInfo->AudioTrack[0];
        MovieInfo->AudioTrackCount = 1U;

        AudioInfo->Info.MediaId = TrackInfo->MediaId;
        AudioInfo->Info.TimeScale = AudioCfg->AuSample;
        AudioInfo->Info.OrigTimeScale = AudioInfo->Info.TimeScale;
        AudioInfo->Info.TimePerFrame = Ctrl->BufInfo.SampleTicks[TRACK_TYPE_AUDIO];
        AudioInfo->Info.BufferBase = NULL; // set when receiving the descriptor (ok to set before process)
        AudioInfo->Info.BufferSize = 0U; // set when receiving the descriptor (ok to set before process)
        AudioInfo->Info.Fifo = TrackInfo->Fifo;

        AudioInfo->IsDefault = (UINT8)0U;
        AudioInfo->SampleRate = AudioCfg->AuSample;
        AudioInfo->Channels = (UINT8)AudioCfg->AuChannels;
        AudioInfo->BitsPerSample = (UINT8)AudioCfg->AuWBitsPerSample;
        AudioInfo->Bitrate = AudioCfg->AuBrate;
        AudioInfo->Volume = (UINT16)AudioCfg->AuVolume;
        AudioInfo->Endian = 0U;

        AudioInfo->Info.InitDTS = (((((UINT64)InitDelay * (UINT64)AudioInfo->Info.TimeScale) / 1000ULL) + ((UINT64)AudioInfo->Info.TimePerFrame / 2ULL)) / (UINT64)AudioInfo->Info.TimePerFrame) * (UINT64)AudioInfo->Info.TimePerFrame;
        AudioInfo->Info.NextDTS = AudioInfo->Info.InitDTS;
        AudioInfo->Info.DTS = AudioInfo->Info.NextDTS;
    }
    return Rval;
}

static UINT32 InitTextTrackInfo(const MP4FMT_CTRL_s *Ctrl, WRP_TRACK_INFO_s *TrackInfo, SVC_MOVIE_INFO_s *MovieInfo, UINT32 InitDelay)
{
    UINT32 Rval = OK;
    const AMBA_MP4FMT_VIDEO_CFG_s *VideoCfg = &Ctrl->Mp4Hdr.VideoCfg;
    SVC_TEXT_TRACK_INFO_s *TextInfo = &MovieInfo->TextTrack[0];

    TrackInfo->TrackType = WRP_MEDIA_TRACK_TYPE_TEXT;
    TrackInfo->MediaId = (UINT8)SVC_FORMAT_MID_TEXT;
    TrackInfo->FrameCnt = 0U;
    TrackInfo->Eos = 0U;

    MovieInfo->TextTrackCount = 1U;

    TextInfo->Info.MediaId = TrackInfo->MediaId;
    TextInfo->Info.TimeScale = VideoCfg->TimeScale;
    TextInfo->Info.OrigTimeScale = VideoCfg->TimeScale;
    TextInfo->Info.TimePerFrame = Ctrl->BufInfo.SampleTicks[TRACK_TYPE_DATA];
    TextInfo->Info.BufferBase = NULL; // set when receiving the descriptor (ok to set before process)
    TextInfo->Info.BufferSize = 0U; // set when receiving the descriptor (ok to set before process)
    TextInfo->Info.Fifo = TrackInfo->Fifo;

    TextInfo->IsDefault = (UINT8)0U;

    TextInfo->Info.InitDTS = (((((UINT64)InitDelay * (UINT64)TextInfo->Info.TimeScale) / 1000ULL) + ((UINT64)TextInfo->Info.TimePerFrame / 2ULL)) / (UINT64)TextInfo->Info.TimePerFrame) * (UINT64)TextInfo->Info.TimePerFrame;
    TextInfo->Info.NextDTS = TextInfo->Info.InitDTS;
    TextInfo->Info.DTS = TextInfo->Info.NextDTS;

    return Rval;
}

static UINT32 NormalizeTimeScale(SVC_MOVIE_INFO_s *Movie)
{
    UINT32 Rval = WrpFormatLib_NormalizeTimeScale(&Movie->MediaInfo);
    if (Rval == OK) {
        SVC_VIDEO_TRACK_INFO_s *Video = &Movie->VideoTrack[0];
        SVC_MEDIA_TRACK_GENERAL_INFO_s *Track = &Video->Info;
        /* video track */
        Track->NextDTS = Track->DTS;
        Track->InitDTS = Track->DTS;
        /* audio track */
        Track = &Movie->AudioTrack[0].Info;
        Track->NextDTS = Track->DTS;
        Track->InitDTS = Track->DTS;
        /* text track */
        Track = &Movie->TextTrack[0].Info;
        Track->NextDTS = Track->DTS;
        Track->InitDTS = Track->DTS;
    }
    return Rval;
}

static UINT32 SetMovieInfo(WRP_FORMAT_INFO_s *FormatInfo, const UINT32 *InitDelay)
{
    UINT32 Rval = OK;
    const MP4FMT_CTRL_s *Ctrl = FormatInfo->pCtrl;
    const AMBA_MP4FMT_VIDEO_CFG_s *VideoCfg = &Ctrl->Mp4Hdr.VideoCfg;
    ULONG BufferAddr = Ctrl->DescBase;
    UINT32 BufferSize = Ctrl->DescSize;
    UINT32 VNumEntries, i;
    VNumEntries = VideoCfg->N * VideoCfg->IdrInterval;
    FormatInfo->TrackCount = 0U;
    for (i = 0; i < WRP_FMT_TRACK_MAX; i++) {
        if ((Ctrl->TrafValid[i]) == 1U) {
            WRP_TRACK_INFO_s *TrackInfo;
            FormatInfo->TrackCount++;
            TrackInfo = &FormatInfo->TrackInfo[i];
            if (i == TRACK_TYPE_VIDEO) {
                UINT32 NumEntries = VNumEntries + 1U; // for an extra IDR
                if (VideoCfg->NumSlice > 1U) {
                    NumEntries *= VideoCfg->NumSlice;
                }
                if (VideoCfg->NumTile > 1U) {
                    NumEntries *= VideoCfg->NumTile;
                }
                NumEntries = NumEntries * (SAMPLE_DESC_TOLERANCE + 100U) / 100U;
                Rval = CreateFifo(NumEntries, TrackInfo, &BufferAddr, &BufferSize);
                if (Rval == OK) {
                    Rval = InitVideoTrackInfo(FormatInfo->pCtrl, TrackInfo, &FormatInfo->Movie);
                }
            } else if (i == TRACK_TYPE_AUDIO) {
                const AMBA_MP4FMT_AUDIO_CFG_s *AudioCfg = &Ctrl->Mp4Hdr.AudioCfg;
                UINT32 VFrate = GetRoundUpValU32(VideoCfg->TimeScale, Ctrl->BufInfo.SampleTicks[TRACK_TYPE_VIDEO]);
                UINT32 AFrate = GetRoundUpValU32(AudioCfg->AuSample, Ctrl->BufInfo.SampleTicks[TRACK_TYPE_AUDIO]);
                UINT32 Secs = GetRoundUpValU32(VNumEntries, VFrate);
                UINT32 NumEntries = (AFrate * Secs) * (SAMPLE_DESC_TOLERANCE + 100U) / 100U;
                Rval = CreateFifo(NumEntries, TrackInfo, &BufferAddr, &BufferSize);
                if (Rval == OK) {
                    Rval = InitAudioTrackInfo(FormatInfo->pCtrl, TrackInfo, &FormatInfo->Movie, InitDelay[i]);
                }
            } else {
                UINT32 NumEntries = VNumEntries * (SAMPLE_DESC_TOLERANCE + 100U) / 100U;   // same frame rate as video
                Rval = CreateFifo(NumEntries, TrackInfo, &BufferAddr, &BufferSize);
                if (Rval == OK) {
                    Rval = InitTextTrackInfo(FormatInfo->pCtrl, TrackInfo, &FormatInfo->Movie, InitDelay[i]);
                }
            }
            if (Rval != OK) {
                break;
            }
        }
    }
    if (Rval == OK) {
        Rval = NormalizeTimeScale(&FormatInfo->Movie);
    }
    return Rval;
}

static UINT32 CheckVideoFrameType(UINT8 FrameType, const SVC_VIDEO_TRACK_INFO_s *Video, UINT8 ClosedGop, UINT32 FrameCnt)
{
    UINT32 Rval = OK;
    UINT8 Type = PIC_FRAME_P;    // p frame as default
    if (FrameCnt == 0U) {
        Type = PIC_FRAME_IDR;
    } else {
        UINT32 FrameIdx = (ClosedGop == 1U)? (FrameCnt + ((UINT32)Video->M - 1U)) : FrameCnt;
        if ((FrameIdx % Video->GOPSize) == 0U) {
            Type = PIC_FRAME_IDR;
        } else if ((FrameIdx % (UINT32)Video->N) == 0U) {
            Type = PIC_FRAME_I;
        } else if ((FrameIdx % (UINT32)Video->M) == 0U) {
            Type = PIC_FRAME_P;
        } else {
            Type = PIC_FRAME_B;
        }
    }
    if (FrameType != (UINT32)Type) {
        Rval = AMBA_MP4FMT_ERR_FATAL;
        AmbaPrint_PrintUInt5("Incorrect FrameType (FrameType %u, M %u, N %u, ClosedGop %u, FrameCnt %u)", (UINT32)FrameType, (UINT32)Video->M, (UINT32)Video->N, (UINT32)ClosedGop, FrameCnt);
    }
    return Rval;
}

static UINT32 CheckVideoDesc(WRP_FORMAT_INFO_s *FormatInfo, const AMBA_DSP_ENC_PIC_RDY_s *PicInfo)
{
    UINT32 Rval = OK;
    if ((PicInfo->SliceIdx == 0U) && (PicInfo->TileIdx == 0U) && (PicInfo->PicSize != AMBA_DSP_ENC_END_MARK)) {
        if (FormatInfo->State != SVC_FMT_STATE_READY) {
            const SVC_VIDEO_TRACK_INFO_s *Video = &FormatInfo->Movie.VideoTrack[0];
            UINT64 TimePerFrame = (UINT64)FormatInfo->pCtrl->BufInfo.SampleTicks[TRACK_TYPE_VIDEO];
            /* Check PTS */
            if (Video->M == 1U) {
                if (PicInfo->Pts != (FormatInfo->LastVideoPts + TimePerFrame)) {
                    Rval = AMBA_MP4FMT_ERR_INVALID_ARG;
                    AmbaPrint_PrintUInt5("(%u) Incorrect Pts (Pts %u, LastPts %u, TimePerFrame %u)", __LINE__, (UINT32)PicInfo->Pts, (UINT32)FormatInfo->LastVideoPts, (UINT32)TimePerFrame, 0U);
                } else {
                    if (Video->CaptureTimeMode == 1U) {
                        /* Check CaptureTimeStamp */
                        TimePerFrame = TimePerFrame * Video->CaptureTimeScale / Video->Info.OrigTimeScale;
                        if ((PicInfo->CaptureTimeStamp <= FormatInfo->LastVideoCaptureTime) || (PicInfo->CaptureTimeStamp > (FormatInfo->LastVideoCaptureTime + (2U * TimePerFrame)))) {
                            Rval = AMBA_MP4FMT_ERR_INVALID_ARG;
                            AmbaPrint_PrintUInt5("(%u) Incorrect CaptureTimeStamp (TimeStamp %u, LastTimeStamp %u, TimePerFrame %u)", __LINE__, (UINT32)PicInfo->CaptureTimeStamp, (UINT32)FormatInfo->LastVideoCaptureTime, (UINT32)TimePerFrame, 0U);
                        }
                    }
                }
            } else {
                // with B frame
                const AMBA_MP4FMT_VIDEO_CFG_s *VideoCfg = &FormatInfo->pCtrl->Mp4Hdr.VideoCfg;
                UINT64 FrameCnt = FormatInfo->TrackInfo[AMBA_RSC_TYPE_VIDEO].FrameCnt / ((UINT64)VideoCfg->NumSlice * (UINT64)VideoCfg->NumTile);
                /* check frame type */
                Rval = CheckVideoFrameType(PicInfo->FrameType, Video, VideoCfg->ClosedGop, (UINT32)FrameCnt);
                if (Rval == OK) {
                    if (PicInfo->FrameType == PIC_FRAME_B) {
                        /* adjust InitPts for open gop*/
                        if (FormatInfo->InitVideoPts > PicInfo->Pts) {
                            FormatInfo->InitVideoPts  = PicInfo->Pts;
                        }
                        /* B's PTS = (FrameCnt - 1) * TPF */
                        if ((FormatInfo->InitVideoPts + ((UINT64)FrameCnt * (UINT64)TimePerFrame)) != (PicInfo->Pts + (UINT64)TimePerFrame)) {
                            Rval = AMBA_MP4FMT_ERR_INVALID_ARG;
                            AmbaPrint_PrintUInt5("(%u) Incorrect Pts (Pts %u, InitPts %u, TimePerFrame %u, FrameCnt %u)", __LINE__, (UINT32)PicInfo->Pts, (UINT32)FormatInfo->InitVideoPts, (UINT32)TimePerFrame, (UINT32)FormatInfo->TrackInfo[AMBA_RSC_TYPE_VIDEO].FrameCnt);
                        }
                    } else {
                        /* I/P + BBBB + I/P: Last I/P's PTS = Last B's PTS + TPF */
                        if (FormatInfo->RefVideoPts != (FormatInfo->LastVideoPts + (UINT64)TimePerFrame)) {
                            if ((FrameCnt != 1U) && (VideoCfg->ClosedGop != 1U)) {
                                Rval = AMBA_MP4FMT_ERR_INVALID_ARG;
                                AmbaPrint_PrintUInt5("(%u) Incorrect Pts (Pts %u, InitPts %u, TimePerFrame %u, FrameCnt %u)", __LINE__, (UINT32)PicInfo->Pts, (UINT32)FormatInfo->InitVideoPts, (UINT32)TimePerFrame, (UINT32)FormatInfo->TrackInfo[AMBA_RSC_TYPE_VIDEO].FrameCnt);
                            }
                        }
                        FormatInfo->RefVideoPts = PicInfo->Pts;
                    }
                }
            }
        } else {
            /* first IDR */
            FormatInfo->RefVideoPts= PicInfo->Pts;
            FormatInfo->InitVideoPts = PicInfo->Pts;
        }
        FormatInfo->LastVideoPts = PicInfo->Pts;
        FormatInfo->LastVideoCaptureTime = PicInfo->CaptureTimeStamp;
    }
    return Rval;
}

static UINT32 WriteFifoVideo(WRP_FORMAT_INFO_s *FormatInfo, const AMBA_RSC_DESC_s *pDesc, SVC_FIFO_BITS_DESC_s *FifoDesc)
{
    UINT32 Rval;
    const AMBA_DSP_ENC_PIC_RDY_s *PicInfo;
    AmbaMisra_TypeCast(&PicInfo, &pDesc->pSrcData);
    Rval = CheckVideoDesc(FormatInfo, PicInfo);
    if (Rval == OK) {
        SVC_MEDIA_TRACK_GENERAL_INFO_s *Track = &FormatInfo->Movie.VideoTrack[0].Info;
        ULONG BufferBase, BufferLimit;
        BufferBase = pDesc->SrcBufBase;
        BufferLimit = pDesc->SrcBufBase + (ULONG)pDesc->SrcBufSize;
        if (Track->BufferBase == NULL) {
            AmbaMisra_TypeCast(&Track->BufferBase, &BufferBase);
            Track->BufferSize = pDesc->SrcBufSize;
        }
        if ((PicInfo->PicSize != AMBA_DSP_ENC_END_MARK) && ((PicInfo->StartAddr < BufferBase) || (PicInfo->StartAddr >= BufferLimit))) {
            AmbaPrint_PrintUInt5("(%u) Incorrect StartAddr %x, Size %x, Base=%x, Limit=%x", __LINE__, (UINT32)PicInfo->StartAddr, PicInfo->PicSize, (UINT32)BufferBase, (UINT32)BufferLimit);
            Rval = AMBA_MP4FMT_ERR_FATAL;
        } else {
            const SVC_FIFO_HDLR_s * const Fifo = FormatInfo->TrackInfo[AMBA_RSC_TYPE_VIDEO].BaseFifo;
            FifoDesc->SeqNum = (UINT64)PicInfo->FrmNo;
            FifoDesc->Pts = PicInfo->Pts;
            FifoDesc->FrameType = PicInfo->FrameType;
            if ((PicInfo->NumTile <= 1U) || (PicInfo->TileIdx == (PicInfo->NumTile - 1U))) {
                if ((PicInfo->NumSlice <= 1U) || (PicInfo->SliceIdx == (PicInfo->NumSlice - 1U)) ) {
                    FifoDesc->Completed = 1U;
                }
            }
            AmbaMisra_TypeCast(&FifoDesc->StartAddr, &PicInfo->StartAddr);
            FifoDesc->Size = PicInfo->PicSize;
            FifoDesc->CaptureTimeStamp = PicInfo->CaptureTimeStamp;
            FifoDesc->EncodeTimeStamp = PicInfo->EncodeTimeStamp;
            if (PicInfo->PicSize == AMBA_DSP_ENC_END_MARK) {
                FifoDesc->FrameType = SVC_FIFO_TYPE_EOS; /* modify frame type to EOS */
                FifoDesc->Size = SVC_FIFO_MARK_EOS;
                FormatInfo->TrackInfo[AMBA_RSC_TYPE_VIDEO].Eos = 1U;
            }
            FormatInfo->TrackInfo[AMBA_RSC_TYPE_VIDEO].FrameCnt++;
            Rval = F2W(SvcFIFO_WriteEntry(Fifo, FifoDesc, 0U));
            if (Rval != OK) {
                Rval= AMBA_MP4FMT_ERR_FATAL;
                Wrapper_Perror(__func__, __LINE__, NULL);
            }
        }
    }
    return Rval;
}

static UINT32 CheckFifoSpace(const WRP_FORMAT_INFO_s *FormatInfo, const WRP_TRACK_INFO_s *TrackInfo)
{
    SVC_FIFO_INFO_s Info = {0};
    UINT32 Rval = F2W(SvcFIFO_GetInfo(TrackInfo->BaseFifo, &Info));
    if (Rval == OK) {
        if (Info.AvailEntries == TrackInfo->NumEntries){
            if (FormatInfo->TrackInfo[AMBA_RSC_TYPE_VIDEO].Eos == 1U) {
                // ok, skip the descriptor since it is enough.
                Rval = AMBA_MP4FMT_ERR_DATA_FULL;
            } else {
                Rval = AMBA_MP4FMT_ERR_FATAL;
                Wrapper_Perror(__func__, __LINE__, "desc queue full");
            }
        }
    } else {
        Wrapper_Perror(__func__, __LINE__, NULL);
    }
    return Rval;
}

static UINT32 WriteFifoAudio(WRP_FORMAT_INFO_s *FormatInfo, const AMBA_RSC_DESC_s *pDesc, SVC_FIFO_BITS_DESC_s *FifoDesc)
{
    UINT32 Rval;
    SVC_MEDIA_TRACK_GENERAL_INFO_s *Track = &FormatInfo->Movie.AudioTrack[0].Info;
    const AMBA_AENC_AUDIO_DESC_s *AudioDesc;
    ULONG BufferBase, BufferLimit, StartAddr = 0;
    BufferBase = pDesc->SrcBufBase;
    BufferLimit = pDesc->SrcBufBase + (ULONG)pDesc->SrcBufSize;
    AmbaMisra_TypeCast(&AudioDesc, &pDesc->pSrcData);
    if (Track->BufferBase == NULL) {
        AmbaMisra_TypeCast(&Track->BufferBase, &BufferBase);
        Track->BufferSize = pDesc->SrcBufSize;
    }
    AmbaMisra_TypeCast(&StartAddr, &(AudioDesc->pBufAddr));
    if ((AudioDesc->Eos != 1U) && ((StartAddr < BufferBase) || (StartAddr >= BufferLimit))) {
        AmbaPrint_PrintUInt5("(%u) Incorrect StartAddr %x, Base=%x, Limit=%x", __LINE__, (UINT32)StartAddr, (UINT32)BufferBase, (UINT32)BufferLimit, 0U);
        Rval = AMBA_MP4FMT_ERR_FATAL;
    } else {
        if (AudioDesc->Eos == 1U) {
            FifoDesc->SeqNum = FormatInfo->TrackInfo[AMBA_RSC_TYPE_AUDIO].FrameCnt;
            FifoDesc->Completed = 1;
            FifoDesc->FrameType = SVC_FIFO_TYPE_EOS;
            FifoDesc->Size = SVC_FIFO_MARK_EOS;
            FormatInfo->TrackInfo[AMBA_RSC_TYPE_AUDIO].Eos = 1U;
        } else {
            UINT64 TimePerFrame = FormatInfo->pCtrl->BufInfo.SampleTicks[AMBA_RSC_TYPE_AUDIO];
            FifoDesc->SeqNum = FormatInfo->TrackInfo[AMBA_RSC_TYPE_AUDIO].FrameCnt;
            FifoDesc->Align = 0;
            FifoDesc->Completed = 1;
            FifoDesc->Pts = FifoDesc->SeqNum * TimePerFrame;
            FifoDesc->CaptureTimeStamp = AudioDesc->AudioTicks; /* dummy */
            FifoDesc->FrameType = SVC_FIFO_TYPE_AUDIO_FRAME;
            FifoDesc->Size = AudioDesc->DataSize;
            FifoDesc->StartAddr = AudioDesc->pBufAddr;
        }
        FormatInfo->TrackInfo[AMBA_RSC_TYPE_AUDIO].FrameCnt++;
        Rval = CheckFifoSpace(FormatInfo, &FormatInfo->TrackInfo[AMBA_RSC_TYPE_AUDIO]);
        if (Rval == OK) {
            const SVC_FIFO_HDLR_s * const Fifo = FormatInfo->TrackInfo[AMBA_RSC_TYPE_AUDIO].BaseFifo;
            Rval = F2W(SvcFIFO_WriteEntry(Fifo, FifoDesc, 0U));
            if (Rval != OK) {
                Wrapper_Perror(__func__, __LINE__, NULL);
            }
        } else if (Rval == AMBA_MP4FMT_ERR_DATA_FULL) {
            Rval = OK;
        } else {
            // NG
        }
    }
    return Rval;
}

static UINT32 WriteFifoText(WRP_FORMAT_INFO_s *FormatInfo, const AMBA_RSC_DESC_s *pDesc, SVC_FIFO_BITS_DESC_s *FifoDesc)
{
    UINT32 Rval;
    SVC_MEDIA_TRACK_GENERAL_INFO_s *Track = &FormatInfo->Movie.TextTrack[0].Info;
    const AMBA_RSC_TEXT_DESC_s *TextDesc;
    const AMBA_DATG_s *DataG;
    ULONG BufferBase, BufferLimit, StartAddr = 0;
    BufferBase = pDesc->SrcBufBase;
    BufferLimit = pDesc->SrcBufBase + (ULONG)pDesc->SrcBufSize;
    AmbaMisra_TypeCast(&TextDesc, &pDesc->pSrcData);
    if (Track->BufferBase == NULL) {
        AmbaMisra_TypeCast(&Track->BufferBase, &BufferBase);
        Track->BufferSize = pDesc->SrcBufSize;
    }
    AmbaMisra_TypeCast(&StartAddr, &(TextDesc->pText));
    AmbaMisra_TypeCast(&DataG, &(TextDesc->pRawData));
    if ((DataG->Eos != 1U) && ((StartAddr < BufferBase) || (StartAddr >= BufferLimit))) {
        AmbaPrint_PrintUInt5("(%u) Incorrect StartAddr %x, Base=%x, Limit=%x", __LINE__, (UINT32)StartAddr, (UINT32)BufferBase, (UINT32)BufferLimit, 0U);
        Rval = AMBA_MP4FMT_ERR_FATAL;
    } else {
        if (DataG->Eos == 1U) {
            FifoDesc->SeqNum = FormatInfo->TrackInfo[AMBA_RSC_TYPE_DATA].FrameCnt;
            FifoDesc->Completed = 1;
            FifoDesc->FrameType = SVC_FIFO_TYPE_EOS;
            FifoDesc->Size = SVC_FIFO_MARK_EOS;
            FormatInfo->TrackInfo[AMBA_RSC_TYPE_DATA].Eos = 1U;
        } else {
            UINT64 TimePerFrame = FormatInfo->pCtrl->BufInfo.SampleTicks[AMBA_RSC_TYPE_DATA];
            FifoDesc->SeqNum = FormatInfo->TrackInfo[AMBA_RSC_TYPE_DATA].FrameCnt;
            FifoDesc->Align = 0;
            FifoDesc->Completed = 1;
            FifoDesc->Pts = FifoDesc->SeqNum * TimePerFrame;
            FifoDesc->CaptureTimeStamp = DataG->CapTime[TICK_TYPE_AUDIO];
            FifoDesc->FrameType = SVC_FIFO_TYPE_UNDEFINED;
            FifoDesc->Size = TextDesc->TexLen;
            AmbaMisra_TypeCast(&FifoDesc->StartAddr, &(TextDesc->pText));
        }
        FormatInfo->TrackInfo[AMBA_RSC_TYPE_DATA].FrameCnt++;
        Rval = CheckFifoSpace(FormatInfo, &FormatInfo->TrackInfo[AMBA_RSC_TYPE_DATA]);
        if (Rval == OK) {
            const SVC_FIFO_HDLR_s * const Fifo = FormatInfo->TrackInfo[AMBA_RSC_TYPE_DATA].BaseFifo;
            Rval = F2W(SvcFIFO_WriteEntry(Fifo, FifoDesc, 0U));
            if (Rval != OK) {
                Wrapper_Perror(__func__, __LINE__, NULL);
            }
        } else if (Rval == AMBA_MP4FMT_ERR_DATA_FULL) {
            Rval = OK;
        } else {
            // NG
        }
    }
    return Rval;
}

static UINT32 WriteFifo(WRP_FORMAT_INFO_s *FormatInfo, const AMBA_RSC_DESC_s *pDesc, SVC_FIFO_BITS_DESC_s *FifoDesc)
{
    UINT32 Rval = OK;
    if (pDesc->SrcType == AMBA_RSC_TYPE_VIDEO) {
        if (FormatInfo->TrackInfo[TRACK_TYPE_VIDEO].SkipCnt == 0U) {
            if ((FormatInfo->State != SVC_FMT_STATE_REACH_GOP) && (FormatInfo->State != SVC_FMT_STATE_REACH_LIMIT)) {
                Rval = WriteFifoVideo(FormatInfo, pDesc, FifoDesc);
            }
        } else {
            FormatInfo->TrackInfo[TRACK_TYPE_VIDEO].SkipCnt--;
        }
    } else if (pDesc->SrcType == AMBA_RSC_TYPE_AUDIO) {
        if (FormatInfo->TrackInfo[TRACK_TYPE_AUDIO].SkipCnt == 0U) {
            Rval = WriteFifoAudio(FormatInfo, pDesc, FifoDesc);
        } else {
            FormatInfo->TrackInfo[TRACK_TYPE_AUDIO].SkipCnt--;
        }
    } else {
        if (FormatInfo->TrackInfo[TRACK_TYPE_DATA].SkipCnt == 0U) {
            Rval = WriteFifoText(FormatInfo, pDesc, FifoDesc);
        } else {
            FormatInfo->TrackInfo[TRACK_TYPE_DATA].SkipCnt--;
        }
    }
    return Rval;
}

static UINT32 UpdateWrittenSize(WRP_FORMAT_INFO_s *FormatInfo, UINT32 *Size)
{
    SVC_STREAM_HDLR_s *Stream = FormatInfo->Hdlr->Stream;
    UINT64 TotalLength = 0U;
    UINT32 Rval = S2W(Stream->Func->GetLength(Stream, &TotalLength));
    if (Rval == OK) {
        UINT64 CurrentLength = TotalLength - FormatInfo->WrittenSize;
        FormatInfo->WrittenSize = TotalLength;
        *Size = (UINT32)CurrentLength;
    } else {
        Wrapper_Perror(__func__, __LINE__, NULL);
    }
    return Rval;
}


static UINT32 ConfigStreamBuffer(const WRP_FORMAT_INFO_s *FormatInfo)
{
    const MP4FMT_BUF_INFO_s *BufInfo = &FormatInfo->pCtrl->BufInfo;
    UINT32 Rval = S2W(SvcBufferStream_ConfigBuffer(FormatInfo->Hdlr->Stream, BufInfo->BufferBase, BufInfo->BufferSize));
    if (Rval != OK) {
        Wrapper_Perror(__func__, __LINE__, NULL);
    }
    return Rval;
}

static UINT32 SetCaptureTimeInfo(WRP_FORMAT_INFO_s *FormatInfo)
{
    const SVC_FIFO_HDLR_s * const Fifo = FormatInfo->TrackInfo[AMBA_RSC_TYPE_VIDEO].BaseFifo;
    SVC_FIFO_BITS_DESC_s *Desc;
    UINT32 Rval = F2W(SvcFIFO_PeekEntry(Fifo, &Desc, 0));
    if (Rval == OK) {
        SVC_VIDEO_TRACK_INFO_s *Video = &FormatInfo->Movie.VideoTrack[0];
        if (FormatInfo->State == SVC_FMT_STATE_READY) {
            Video->InitCaptureTime = Desc->CaptureTimeStamp;
            Video->PastDTS = 0U;
        } else {
            Video->PastDTS += (Video->Info.DTS - Video->Info.InitDTS);
        }
    } else {
        Wrapper_Perror(__func__, __LINE__, NULL);
    }
    return Rval;
}

static void UpdateMehdInfo(WRP_FORMAT_INFO_s *FormatInfo)
{
    UINT8                                 TrackIdx;
    DOUBLE                                FragDuration = 0.0;
    const SVC_MEDIA_TRACK_GENERAL_INFO_s  *Track;

    AmbaMisra_TouchUnused(&FormatInfo);

    if (FormatInfo->pCtrl->Mp4Hdr.MehdCtrl.Enable > 0U) {
        for (TrackIdx = 0U; TrackIdx < FormatInfo->Movie.VideoTrackCount; TrackIdx++) {
            if ((FormatInfo->Movie.VideoTrack[TrackIdx].IsDefault == 1U)) {
                Track = &(FormatInfo->Movie.VideoTrack[TrackIdx].Info);

                FragDuration = ((DOUBLE)Track->DTS / (DOUBLE)Track->TimeScale) * (DOUBLE)FormatInfo->Movie.MvhdTimeScale;
            }
        }

        FormatInfo->pCtrl->Mp4Hdr.MehdCtrl.FileOffset   = FormatInfo->Movie.MehdPos;
        FormatInfo->pCtrl->Mp4Hdr.MehdCtrl.FragDuration = (UINT32)FragDuration;
    }
}

static UINT32 ProcessDesc_FlushNone(WRP_FORMAT_INFO_s *FormatInfo, const SVC_FIFO_BITS_DESC_s *FifoDesc)
{
    UINT32 Rval = OK;
    UINT8 ProcessFrame = 0U;
    if ((FormatInfo->State == SVC_FMT_STATE_READY) || (FormatInfo->State == SVC_FMT_STATE_REACH_LIMIT)) {
        SVC_MOVIE_INFO_s *Movie = &FormatInfo->Movie;
        Movie->MediaInfo.CreationTime = FormatInfo->pCtrl->Mp4Hdr.CreationTime;
        Movie->MediaInfo.ModificationTime = FormatInfo->pCtrl->Mp4Hdr.ModificationTime;
        WrpFormatLib_ResetMuxMovieInfo(Movie);
        if (Movie->VideoTrack[0].CaptureTimeMode == 1U) {
            Rval = SetCaptureTimeInfo(FormatInfo);
        }
        if (Rval == OK) {
            if (FormatInfo->State == SVC_FMT_STATE_REACH_LIMIT) {   // auto split
                Rval = ResetMovieInfo(FormatInfo, Movie);
            }
        }
        if (Rval == OK) {
            SVC_MUX_FORMAT_HDLR_s *Format = FormatInfo->Hdlr;
            SVC_STREAM_HDLR_s *Stream = Format->Stream;
            Rval = S2W(Stream->Func->Open(Stream, Movie->MediaInfo.Name, SVC_STREAM_MODE_WRONLY));
            if (Rval == OK) {
                FormatInfo->WrittenSize = 0U;
                Rval = M2W(Format->Func->Open(Format));
                if (Rval == OK) {
                    Rval = ConfigStreamBuffer(FormatInfo);
                    if (Rval == OK) {
                        if ((FifoDesc->Completed == 1U) || (FormatInfo->State == SVC_FMT_STATE_REACH_LIMIT)) {
                            ProcessFrame = 1U;
                        }
                        FormatInfo->State = SVC_FMT_STATE_RUN;
                    }
                } else {
                    Wrapper_Perror(__func__, __LINE__, NULL);
                }
            } else {
                Wrapper_Perror(__func__, __LINE__, NULL);
            }
        }
    } else if (FormatInfo->State == SVC_FMT_STATE_REACH_GOP) {
        Rval = ConfigStreamBuffer(FormatInfo);
        if (Rval == OK) {
            FormatInfo->State = SVC_FMT_STATE_RUN;
        }
    } else if (FormatInfo->State == SVC_FMT_STATE_RUN) {
        if (FifoDesc->Completed == 1U) {
            ProcessFrame = 1U;
        }
    } else {
        Rval = AMBA_MP4FMT_ERR_FATAL;
        Wrapper_Perror(__func__, __LINE__, "Wrong State");
    }
    if (Rval == OK) {
        if (ProcessFrame == 1U) {
            UINT8 Event = SVC_MUXER_EVENT_NONE;
            SVC_MUX_FORMAT_HDLR_s *Format = FormatInfo->Hdlr;
            Rval = M2W(Format->Func->Process(Format, SVC_FORMAT_MAX_TIMESTAMP, &Event));
            if (Rval != OK) {
                Wrapper_Perror(__func__, __LINE__, NULL);
            }
        }
    }
    return Rval;
}

static UINT32 ProcessDesc_FlushGOP(WRP_FORMAT_INFO_s *FormatInfo, UINT32 *Size)
{
    UINT32 Rval = OK;
    if (FormatInfo->State == SVC_FMT_STATE_RUN) {
        UINT8 Event = SVC_MUXER_EVENT_NONE;
        SVC_MUX_FORMAT_HDLR_s *Format = FormatInfo->Hdlr;
        Rval = M2W(Format->Func->Process(Format, SVC_FORMAT_MAX_TIMESTAMP, &Event));
        if (Rval != OK) {
            Wrapper_Perror(__func__, __LINE__, "Wrong State");
        }
        FormatInfo->State = SVC_FMT_STATE_REACH_GOP;
    } else {
        Rval = AMBA_MP4FMT_ERR_FATAL;
        Wrapper_Perror(__func__, __LINE__, "AMBA_MP4FMT_FLUSH_REACH_GOP: incorrect state!");
    }
    if (Rval == OK) {
        Rval = UpdateWrittenSize(FormatInfo, Size);
    }
    return Rval;
}

static UINT32 ProcessDesc_FlushLimit(WRP_FORMAT_INFO_s *FormatInfo, UINT32 *Size)
{
    UINT32                Rval = OK;
    SVC_MUX_FORMAT_HDLR_s *Format = FormatInfo->Hdlr;

    if (FormatInfo->State == SVC_FMT_STATE_RUN) {
        UINT8 Event = SVC_MUXER_EVENT_NONE;
        Format->MaxDuration = 0;    // force split
        Rval = M2W(Format->Func->Process(Format, SVC_FORMAT_MAX_TIMESTAMP, &Event));
        if (Rval == OK) {
            if (Event == SVC_MUXER_EVENT_REACH_LIMIT) {
                Format->MaxDuration = 0xFFFFFFFFU;
                FormatInfo->State = SVC_FMT_STATE_REACH_LIMIT;
                //AmbaPrint_PrintUInt5("[DBG] ReachLimit!!!", 0U, 0U, 0U, 0U, 0U);
            } else {
                Rval = AMBA_MP4FMT_ERR_FATAL;
                Wrapper_Perror(__func__, __LINE__, "REACH LIMIT failed!");
            }
        } else {
            Rval = AMBA_MP4FMT_ERR_FATAL;
            Wrapper_Perror(__func__, __LINE__, "Wrong State");
        }
    } else {
        Rval = AMBA_MP4FMT_ERR_FATAL;
        Wrapper_Perror(__func__, __LINE__, "AMBA_MP4FMT_FLUSH_REACH_GOP/LIMIT: incorrect state!");
    }
    if (Rval == OK) {
        Rval = M2W(Format->Func->Close(Format, SVC_MUX_FORMAT_CLOSE_DEFAULT)); /* TBD: Close not end? */
        if (Rval == OK) {
            Rval = UpdateWrittenSize(FormatInfo, Size);
            if (Rval == OK) {
                SVC_STREAM_HDLR_s *Stream = Format->Stream;
                Rval = S2W(Stream->Func->Close(Stream));
                if (Rval != OK) {
                    Wrapper_Perror(__func__, __LINE__, NULL);
                }
                if (FormatInfo->pCtrl->Mp4Hdr.MehdCtrl.Enable > 0U) {
                    UpdateMehdInfo(FormatInfo);
                }
            }
        } else {
            Wrapper_Perror(__func__, __LINE__, NULL);
        }
    }
    return Rval;
}

static UINT32 ProcessDesc_FlushEnd(WRP_FORMAT_INFO_s *FormatInfo, const SVC_FIFO_BITS_DESC_s *FifoDesc, UINT32 *Size)
{
    UINT32 Rval = OK;
    SVC_MUX_FORMAT_HDLR_s *Format = FormatInfo->Hdlr;
    UINT64 TargetTime;
    UINT8 CloseMode;
    if (FifoDesc->FrameType == SVC_FIFO_TYPE_EOS) {
        TargetTime = SVC_FORMAT_MAX_TIMESTAMP; // process to end
        CloseMode = SVC_MUX_FORMAT_CLOSE_DEFAULT;
    } else if (FifoDesc->FrameType == SVC_FIFO_TYPE_IDR_FRAME) {
        TargetTime = SVC_FORMAT_MAX_TIMESTAMP; // process to limit like auto split
        Format->MaxDuration = 0x0U;
        CloseMode = SVC_MUX_FORMAT_CLOSE_DEFAULT;
    } else {
        TargetTime = 0U;   // drop the unfinished GOP
        CloseMode = SVC_MUX_FORMAT_CLOSE_NOT_END;
    }

    if (TargetTime == SVC_FORMAT_MAX_TIMESTAMP) {
        UINT8 Event = SVC_MUXER_EVENT_NONE;
        Rval = M2W(Format->Func->Process(Format, TargetTime, &Event));   // buffer will be prepared
        if (Rval == OK) {
            if ((Event != SVC_MUXER_EVENT_REACH_LIMIT) && (Event != SVC_MUXER_EVENT_REACH_END)) {
                Rval = AMBA_MP4FMT_ERR_FATAL;
                Wrapper_Perror(__func__, __LINE__, "AMBA_MP4FMT_FLUSH_REACH_END: incorrect event!");
            }
        } else {
            Wrapper_Perror(__func__, __LINE__, NULL);
        }
    }
    if (Rval == OK) {
        SVC_STREAM_HDLR_s *Stream = Format->Stream;
        Rval = M2W(Format->Func->Close(Format, CloseMode));
        if (Rval == OK) {
            Rval = UpdateWrittenSize(FormatInfo, Size);
            if (Rval == OK) {
                Rval = S2W(Stream->Func->Close(Stream));
                if (Rval == OK) {
                    if (FormatInfo->pCtrl->Mp4Hdr.MehdCtrl.Enable > 0U) {
                        UpdateMehdInfo(FormatInfo);
                    }
                    Rval = SvcFmp4Fmt_Delete(FormatInfo);
                    if (Rval == OK) {
                        FormatInfo->pCtrl = NULL;
                        FormatInfo->State = SVC_FMT_STATE_END;
                    }
                } else {
                    Wrapper_Perror(__func__, __LINE__, NULL);
                }
            }
        } else {
            Wrapper_Perror(__func__, __LINE__, NULL);
        }
    }
    return Rval;
}

static UINT32 SvcMP4Fmt_ProcessDesc(WRP_FORMAT_INFO_s *FormatInfo, const AMBA_RSC_DESC_s *SrcDesc, const SVC_FIFO_BITS_DESC_s *FifoDesc, UINT32 IsFlush, UINT32 *Size)
{
    UINT32 Rval = OK;
    if (IsFlush == AMBA_MP4FMT_FLUSH_NONE) {
        if (SrcDesc->SrcType == AMBA_RSC_TYPE_VIDEO) {
            if (FifoDesc->FrameType != SVC_FIFO_TYPE_EOS) {
#if defined(CONFIG_APP_FLOW_CARDV_AONI)
                extern void UpdateVideoMetadataStreamId(int streamId);
                AMBA_DSP_ENC_PIC_RDY_s         *pVDesc;
                AmbaMisra_TypeCast(&(pVDesc), &(SrcDesc->pSrcData));
                UpdateVideoMetadataStreamId(pVDesc->StreamId);
                //AmbaPrint_PrintUInt5("pVDesc->StreamId=%d", pVDesc->StreamId, 0U, 0U, 0U, 0U);
#endif

                Rval = ProcessDesc_FlushNone(FormatInfo, FifoDesc);
            } else {
                // wait to process when IsFlush is set (all track have received EOS)
            }
        }
    } else if (IsFlush == AMBA_MP4FMT_FLUSH_REACH_GOP) {
        if ((SrcDesc->SrcType == AMBA_RSC_TYPE_VIDEO) && (FifoDesc->FrameType == SVC_FIFO_TYPE_IDR_FRAME)) {
            Rval = ProcessDesc_FlushGOP(FormatInfo, Size);
        } else {
            Rval = AMBA_MP4FMT_ERR_FATAL;
            Wrapper_Perror(__func__, __LINE__, "AMBA_MP4FMT_FLUSH_REACH_GOP: incorrect frame type!");
        }
    } else if (IsFlush == AMBA_MP4FMT_FLUSH_REACH_LIMIT) {
        if ((SrcDesc->SrcType == AMBA_RSC_TYPE_VIDEO) && (FifoDesc->FrameType == SVC_FIFO_TYPE_IDR_FRAME)) {
            Rval = ProcessDesc_FlushLimit(FormatInfo, Size);
        } else {
            Rval = AMBA_MP4FMT_ERR_FATAL;
            Wrapper_Perror(__func__, __LINE__, "AMBA_MP4FMT_FLUSH_REACH_LIMIT: incorrect frame type!");
        }
    } else {
        /* IsFlush == AMBA_MP4FMT_FLUSH_REACH_END */
        Rval = ProcessDesc_FlushEnd(FormatInfo, FifoDesc, Size);
    }
    return Rval;
}

static inline UINT32 AmbaMux_SetInitDelay_ParamCheck(const MP4FMT_CTRL_s *pCtrl, const UINT32 *InitDelay)
{
    UINT32 Rval;
    if ((pCtrl == NULL) || (InitDelay == NULL)) {
        Rval = AMBA_MP4FMT_ERR_INVALID_ARG;
        Wrapper_Perror(__func__, __LINE__, "AudioCoding not supported!");
    } else {
        Rval = OK;
    }
    return Rval;
}

/**
 *  Set the initial delay time for each track.
 *  (For emergency event record, the initial diff will be lost.)
 *
 *  @param [in] pCtrl Format controller
 *  @param [in] InitDelay Initial delay in ms (An array of size TRACK_TYPE_MAX)
 *  @return 0 - OK, others - AMBA_MP4FMT_ERR_XXXX
 */
UINT32 AmbaMux_SetInitDelay(MP4FMT_CTRL_s *pCtrl, const UINT32 *InitDelay)
{
    /* Note: For emergency event record, the initial diff will be lost. (-> video track might be the longest) */
    UINT32 Rval = AmbaMux_SetInitDelay_ParamCheck(pCtrl, InitDelay);
    if (Rval == OK) {
        WRP_FORMAT_INFO_s *FormatInfo = NULL;
        UINT8 i;
        for (i = 0; i < WRP_FMT_MAX_NUM; i++) {
            FormatInfo = &g_MuxerInfo.FormatInfo[i];
            Rval = GetFormat(FormatInfo);
            if (Rval == OK) {
                if (FormatInfo->pCtrl != pCtrl) {
                    Rval = GiveFormat(FormatInfo);
                    FormatInfo = NULL;
                }
            }
            if ((FormatInfo != NULL) || (Rval != OK)) {
                break;
            }
        }
        if (Rval == OK) {
            if (FormatInfo == NULL) {
                Rval = SvcFmp4Fmt_Create(pCtrl, &FormatInfo);
                if (Rval == OK) {
                    Rval = InitMovieInfo(FormatInfo);
                    if (Rval == OK) {
                        Rval = SetMovieInfo(FormatInfo, InitDelay);
                        if (Rval == OK) {
                            FormatInfo->Hdlr->Media = &FormatInfo->Movie.MediaInfo;
                            FormatInfo->Hdlr->MaxDuration = 0xFFFFFFFFU;
                            FormatInfo->Hdlr->MaxSize = 0xFFFFFFFFFFFFFFFFULL;
                            FormatInfo->State = SVC_FMT_STATE_READY;    // INIT/END -> READY
                        }
                    }
                    Rval = GiveFormat(FormatInfo);
                }
            } else {
                Rval = GiveFormat(FormatInfo);
                Wrapper_Perror(__func__, __LINE__, "wrong state!");
            }
        }
    }
    return Rval;
}

static inline UINT32 AmbaMux_Proc_ParamCheck(const MP4FMT_CTRL_s *pCtrl, const AMBA_RSC_DESC_s *pDesc, UINT32 IsFlush)
{
    UINT32 Rval;
    if ((pCtrl == NULL) || (pDesc == NULL) || (IsFlush >= AMBA_MP4FMT_FLUSH_MAX)) {
        Rval = AMBA_MP4FMT_ERR_INVALID_ARG;
        Wrapper_Perror(__func__, __LINE__, "AudioCoding not supported!");
    } else if (pCtrl->TrafValid[pDesc->SrcType] == 0U) {
        Rval = AMBA_MP4FMT_ERR_INVALID_ARG;
        Wrapper_Perror(__func__, __LINE__, "Track Type is not valid!");
    } else {
        Rval = OK;
    }
    return Rval;
}

/**
 *  Process the muxing format controller.
 *
 *  @param [in] pCtrl Format controller
 *  @param [in] pDesc The incoming resource descriptor
 *  @param [in] IsFlush Flush option (AMBA_MP4FMT_FLUSH_XXXX)
 *  @param [out] Size The size of data that was flushed
 *  @return 0 - OK, others - AMBA_MP4FMT_ERR_XXXX
 */
UINT32 AmbaMux_Proc(MP4FMT_CTRL_s *pCtrl, const AMBA_RSC_DESC_s *pDesc, UINT32 IsFlush, UINT32 *Size)
{
    UINT32 Rval = AmbaMux_Proc_ParamCheck(pCtrl, pDesc, IsFlush);
    WRP_FORMAT_INFO_s *FormatInfo = NULL;

    *Size = 0U;

    if (Rval == OK) {
        UINT8 i;
        for (i = 0; i < WRP_FMT_MAX_NUM; i++) {
            WRP_FORMAT_INFO_s *Fmt = &g_MuxerInfo.FormatInfo[i];
            Rval = GetFormat(Fmt);
            if (Rval == OK) {
                if (Fmt->pCtrl == pCtrl) {
                    FormatInfo = Fmt;
                } else {
                    Rval = GiveFormat(Fmt);
                }
            }
            if ((FormatInfo != NULL) || (Rval != OK)) {
                break;
            }
        }
        if (Rval == OK) {
            if (FormatInfo == NULL) {
                Rval = SvcFmp4Fmt_Create(pCtrl, &FormatInfo);
                if (Rval == OK) {
                    const UINT32 InitDelay[TRACK_TYPE_MAX] = {0};
                    Rval = InitMovieInfo(FormatInfo);
                    if (Rval == OK) {
                        Rval = SetMovieInfo(FormatInfo, InitDelay);
                        if (Rval == OK) {
                            FormatInfo->Hdlr->Media = &FormatInfo->Movie.MediaInfo;
                            FormatInfo->Hdlr->MaxDuration = 0xFFFFFFFFU;
                            FormatInfo->Hdlr->MaxSize = 0xFFFFFFFFFFFFFFFFULL;
                            FormatInfo->State = SVC_FMT_STATE_READY;    // INIT/END -> READY
                        }
                    }
                }
            }
        }
    }
    if (Rval == OK) {
        SVC_FIFO_BITS_DESC_s FifoDesc = {0};
        Rval = WriteFifo(FormatInfo, pDesc, &FifoDesc);
        if (Rval == OK) {
            //AmbaPrint_PrintUInt5("[DBG] AmbaMux_Proc: Track %u FrameType %u SeqNum %u Pts %u", pDesc->SrcType, FifoDesc.FrameType, FifoDesc.SeqNum, FifoDesc.Pts, 0U);
            Rval = SvcMP4Fmt_ProcessDesc(FormatInfo, pDesc, &FifoDesc, IsFlush, Size);
        }
    }
    if (FormatInfo != NULL){
        UINT32 Error = GiveFormat(FormatInfo);
        if (Error != OK) {
            Rval = Error;
        }
    }
    return Rval;
}

static UINT32 AmbaMux_EvalDescSize_ParamCheck(UINT32 SubType, const AMBA_REC_EVAL_s *pEval, const UINT32 *DescBufferSize)
{
    UINT32 Rval;
    if ((pEval == NULL) || (DescBufferSize == NULL)) {
        Rval = AMBA_MP4FMT_ERR_INVALID_ARG;
    } else {
        Rval = OK;
    }
    AmbaMisra_TouchUnused(&SubType);
    return Rval;
}

/**
 *  Evaluate the size of the descriptor buffer.
 *
 *  @param [in] SubType Subtype of the foramt
 *  @param [in] pEval Evaluating information
 *  @param [out] DescBufferSize The returned descriptor buffer size
 *  @return 0 - OK, others - AMBA_MP4FMT_ERR_XXXX
 */
UINT32 AmbaMux_EvalDescSize(UINT32 SubType, const AMBA_REC_EVAL_s *pEval, UINT32 *DescBufferSize)
{
    UINT32 Rval = AmbaMux_EvalDescSize_ParamCheck(SubType, pEval, DescBufferSize);
    if (Rval == OK) {
        UINT32  i, TrafSamples, Secs, N, IdrTr;
        UINT32  CalcSize = 0U;

        N = pEval->MiaCfg[AMBA_REC_MIA_VIDEO].RsvCfg[MIA_RSV_VIDEO_N];
        IdrTr = pEval->MiaCfg[AMBA_REC_MIA_VIDEO].RsvCfg[MIA_RSV_VIDEO_IDRIT];
        TrafSamples = N * IdrTr;
        Secs = GetRoundUpValU32(TrafSamples, pEval->MiaCfg[AMBA_REC_MIA_VIDEO].FrameRate);

        for (i = 0U; i < AMBA_REC_MIA_MAX; i++) {
            if (0U != pEval->MiaCfg[i].FrameRate) {
                UINT32 NumEntries = pEval->MiaCfg[i].FrameRate * Secs;
                UINT32 BufferSize = 0U;
                if (i == AMBA_REC_MIA_VIDEO) {
                    UINT32 NumSlice = pEval->MiaCfg[AMBA_REC_MIA_VIDEO].RsvCfg[MIA_RSV_VIDEO_SLICE];
                    UINT32 NumTile =  pEval->MiaCfg[AMBA_REC_MIA_VIDEO].RsvCfg[MIA_RSV_VIDEO_TILE];
                    NumEntries += 1U;
                    if (NumSlice > 1U) {
                        NumEntries *= NumSlice;
                    }
                    if (NumTile > 1U) {
                        NumEntries *= NumTile;
                    }
                }
                NumEntries = NumEntries * (SAMPLE_DESC_TOLERANCE + 100U) / 100U;
                (void)SvcFIFO_GetHdlrBufferSize(NumEntries, &BufferSize);
                CalcSize += (BufferSize * 2U);  // for virtual + base fifo
                //AmbaPrint_PrintUInt5("NumEntries %u Size %u / %u", NumEntries, BufferSize, CalcSize, 0U, 0U);
            }
        }

        CalcSize = (UINT32)GetRoundUpValU64(CalcSize, (UINT64)AMBA_CACHE_LINE_SIZE) * (UINT32)AMBA_CACHE_LINE_SIZE;

        *DescBufferSize = CalcSize;

        AmbaPrint_PrintUInt5("AmbaMux_EvalDescSize ## eval. desc size (%u)", (UINT32)CalcSize, 0U, 0U, 0U, 0U);
    }
    return Rval;
}

static UINT32 CalcMoovSize(void)
{
    UINT8 i;
    UINT32 BoxSize = 0U;
    BoxSize += ATOM_HDR_SIZE;// MOOV header size
    /* MVHD box size */
    BoxSize += 108U;
    /* MVEX box size */
    BoxSize += (ATOM_HDR_SIZE + (32U * AMBA_REC_MIA_MAX)/* TREX */ );
    /* UDTA box size */
    BoxSize +=  (ATOM_HDR_SIZE + 384U /* AMBA */); // TBD + Udta->UserDataSize
    /* TRAK box size */
    for (i = 0U; i < AMBA_REC_MIA_MAX; i++){
        BoxSize +=  ATOM_HDR_SIZE;// TRAK header size
        /* TKHD box size */
        BoxSize += 92U;
        /* MDIA box size */
        {
            BoxSize +=  ATOM_HDR_SIZE;// MDIA header size
            /* MDHD box size */
            BoxSize += 32U;
            /* HDLR box size */
            BoxSize += (32U + 14U /* ISO_HDLR_MAX_COM_NAME_LEN */);
            /* MINF box size */
            {
                BoxSize +=  ATOM_HDR_SIZE;// MINF header size
                /* MVHD/SMHD/GMHD box size  */
                if (i == AMBA_REC_MIA_VIDEO) {
                    BoxSize +=  20U; //MVHD
                } else if (i == AMBA_REC_MIA_AUDIO) {
                    BoxSize +=  16U; // SMHD
                } else {
                    /* NOTE: GMHD for AMBA_FORMAT_MID_TEXT, NMHD for AMBA_FORMAT_MID_MP4S*/
                    BoxSize += (ATOM_HDR_SIZE + 24U /* GMIN */ + 44U /* TEXT */);
                }
                /* STBL box size (dummy) */
                {
                    BoxSize +=  (ATOM_HDR_SIZE + 16U /* STSC */ + 20U /* STSZ */ + 16U /* STCO */ + 16U /* STTS */);
                    /* STSD box size*/
                    if (i == AMBA_REC_MIA_VIDEO) {
                        /* AVCC */
                        // BoxSize +=  (16U + (54U + 19U + SVC_FORMAT_MAX_SPS_LENGTH + SVC_FORMAT_MAX_SPS_LENGTH + 32U));
                        /* HVCC */
                        BoxSize +=  (16U + (54U + 46U + SVC_FORMAT_MAX_VPS_LENGTH + SVC_FORMAT_MAX_SPS_LENGTH + SVC_FORMAT_MAX_SPS_LENGTH + 32U));
                    } else if (i == AMBA_REC_MIA_AUDIO) {
                        BoxSize +=  (16U + 52U + 56U); // PCM, AAC: (16U + 36U + 50U)
                    } else {
                        BoxSize +=  (16U + 65U);
                    }
                }
                /* DINF box size */
                BoxSize += (ATOM_HDR_SIZE + 16U /* DREF */ + 12U /* DREF table size */);
            }
        }
    }
    return BoxSize;
}

static UINT32 CalcMoofSize(UINT32 MaxTrafSamples)
{
    UINT32 BoxSize = 0U;
    /* MOOF header size */
    BoxSize += ATOM_HDR_SIZE;
    /* MFHD box size */
    BoxSize += 16U;
    /* TRAF box size */
    {
        UINT32 TrafBoxSize = 0U;
        TrafBoxSize += (ATOM_HDR_SIZE + 36U /* TFHD */ + 16U /* TFDT */ + (20U + 4U) /*TRUN*/);
        TrafBoxSize += (8U * MaxTrafSamples);/* TRUN */
        BoxSize += (TrafBoxSize * AMBA_REC_MIA_MAX);
    }
    return BoxSize;
}

static UINT32 AmbaMux_EvalFragSize_ParamCheck(UINT32 SubType, const AMBA_REC_EVAL_s *pEval, const MP4FMT_BUF_INFO_s *pBufInfo)
{
    UINT32 Rval;
    if ((pEval == NULL) || (pBufInfo == NULL)) {
        Rval = AMBA_MP4FMT_ERR_INVALID_ARG;
    } else {
        Rval = OK;
    }
    AmbaMisra_TouchUnused(&SubType);
    return Rval;
}

/**
 *  Evaluate the size of a fragment buffer.
 *
 *  @param [in] SubType Subtype of the foramt
 *  @param [in] pEval Evaluating information
 *  @param [out] pBufInfo The returned buffer information
 *  @return 0 - OK, others - AMBA_MP4FMT_ERR_XXXX
 */
UINT32 AmbaMux_EvalFragSize(UINT32 SubType, const AMBA_REC_EVAL_s *pEval, MP4FMT_BUF_INFO_s *pBufInfo)
{
    UINT32 Rval = AmbaMux_EvalFragSize_ParamCheck(SubType, pEval, pBufInfo);
    if (Rval == OK) {
        UINT32  i, MaxTrafSamples = 0U, TrafSamples, Secs, N, IdrTr;
        UINT64  CalcSize = 0U;
        UINT32 MetaSize, ElemTotalSize;
        N = pEval->MiaCfg[AMBA_REC_MIA_VIDEO].RsvCfg[MIA_RSV_VIDEO_N];
        IdrTr = pEval->MiaCfg[AMBA_REC_MIA_VIDEO].RsvCfg[MIA_RSV_VIDEO_IDRIT];
        TrafSamples = N * IdrTr;
        Secs = GetRoundUpValU32(TrafSamples, pEval->MiaCfg[AMBA_REC_MIA_VIDEO].FrameRate);

        for (i = 0U; i < AMBA_REC_MIA_MAX; i++) {
            if (0U != pEval->MiaCfg[i].BitRate) {
                TrafSamples = pEval->MiaCfg[i].FrameRate * Secs;
                MaxTrafSamples = GetMaxValU32(MaxTrafSamples, TrafSamples);
            }
        }

        /* (1) meta size */
        /* ftyp */
        CalcSize += 32U;
        /* moov */
        CalcSize += CalcMoovSize();
        /* moof */
        CalcSize += CalcMoofSize(MaxTrafSamples);
        /* free + mdat atom */
        CalcSize += (ATOM_HDR_SIZE * 2U);
        CalcSize = GetRoundUpValU64(CalcSize, (UINT64)AMBA_CACHE_LINE_SIZE) * (UINT64)AMBA_CACHE_LINE_SIZE;

        MetaSize = (UINT32)CalcSize;
#if !defined(CONFIG_APP_FLOW_CARDV_AONI)
        AmbaPrint_PrintUInt5("AmbaMux_EvalFragSize ## eval. desc size (%u)", (UINT32)CalcSize, 0U, 0U, 0U, 0U);
#endif
        /* (2) data size */
        ElemTotalSize = 0U;
        for (i = 0U; i < AMBA_REC_MIA_MAX; i++) {
            if (0U != pEval->MiaCfg[i].BitRate) {
                CalcSize = (UINT64)pEval->MiaCfg[i].BitRate * (UINT64)Secs;
                CalcSize = (CalcSize * (SAMPLE_BRATE_TOLERANCE + 100U) / 100U) / 8U;
                CalcSize = GetRoundUpValU64(CalcSize, (UINT64)AMBA_CACHE_LINE_SIZE) * (UINT64)AMBA_CACHE_LINE_SIZE;
#if !defined(CONFIG_APP_FLOW_CARDV_AONI)
                AmbaPrint_PrintUInt5("AmbaMux_EvalFragSize ## eval. media(%u) size (%u)", i, (UINT32)CalcSize, 0U, 0U, 0U);
#endif
                ElemTotalSize += (UINT32)CalcSize;
                pBufInfo->SampleTicks[i] = pEval->MiaCfg[i].FrameTick;
            }
        }
        pBufInfo->BufferSize = MetaSize + ElemTotalSize;
#if !defined(CONFIG_APP_FLOW_CARDV_AONI)
        AmbaPrint_PrintUInt5("AmbaMux_EvalFragSize ## eval. total element size (%u)", ElemTotalSize, 0U, 0U, 0U, 0U);
#endif
    }
    return Rval;
}

static UINT32 AmbaMux_EvalFileSize_ParamCheck(UINT32 SubType, const AMBA_REC_EVAL_s *pEval, const UINT32 *FileSize)
{
    UINT32 Rval;
    if ((pEval == NULL) || (FileSize == NULL)) {
        Rval = AMBA_MP4FMT_ERR_INVALID_ARG;
    } else {
        Rval = OK;
    }
    AmbaMisra_TouchUnused(&SubType);
    return Rval;
}

/**
 *  Evaluate the size of a file.
 *
 *  @param [in] SubType Subtype of the foramt
 *  @param [in] pEval Evaluating information
 *  @param [out] FileSize The returned file size
 *  @return 0 - OK, others - AMBA_MP4FMT_ERR_XXXX
 */
UINT32 AmbaMux_EvalFileSize(UINT32 SubType, const AMBA_REC_EVAL_s *pEval, UINT32 *FileSize)
{
    UINT32 Rval = AmbaMux_EvalFileSize_ParamCheck(SubType, pEval, FileSize);
    if (Rval == OK) {
        MP4FMT_BUF_INFO_s  BufInfo = {0};
        Rval = AmbaMux_EvalFragSize(SubType, pEval, &BufInfo);
        if (Rval == OK) {
            UINT32 FragNum, N, IdrTr;
#if defined(CONFIG_APP_FLOW_CARDV_AONI)
            UINT32 Secs = pEval->FileSplitTime;
#else
            UINT32 Secs = pEval->FileSplitTime * 60U;
#endif
            N = pEval->MiaCfg[AMBA_REC_MIA_VIDEO].RsvCfg[MIA_RSV_VIDEO_N];
            IdrTr = pEval->MiaCfg[AMBA_REC_MIA_VIDEO].RsvCfg[MIA_RSV_VIDEO_IDRIT];
            FragNum = GetRoundUpValU32(N * IdrTr, pEval->MiaCfg[AMBA_REC_MIA_VIDEO].FrameRate);
            FragNum = GetRoundUpValU32(Secs, FragNum);
            *FileSize = BufInfo.BufferSize * FragNum;
        }
    }
    return Rval;
}

