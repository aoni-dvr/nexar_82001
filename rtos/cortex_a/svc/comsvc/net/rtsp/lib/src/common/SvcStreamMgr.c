/**
 *  @file SvcStreamMgr.c
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
 *  @details stream manager module
 *
 */

#include "AmbaTypes.h"
#include <AmbaKAL.h>
#include "AmbaPrint.h"
#include "SvcRtspServer.h"
#include "SvcStreamMgr.h"
#include "AmbaUtility.h"
#include "AmbaWrap.h"
#include "AmbaSvcWrap.h"
#include "AmbaAudio_AENC.h"
#include "AmbaAudio_AIN.h"
#include "AmbaAudio_AOUT.h"
#include "AmbaAudio_ADEC.h"
#include "AmbaAudioEfx.h"
#include "AmbaAudioBuf.h"
#include "AmbaAudio.h"
#include "AmbaPlayer.h"

static SVC_STREAM_MGR_s* StrmMgr;

static VOID StrmMgrLiveEos(UINT32 Hdlr)
{
    UINT32 i, j;
    AMBA_STREAM_s* Strm;
    UINT32 LiveFifoRemain = 0U;
    UINT32 LiveStreamerRemain = 0U;

    /* find streamer who is Live stream and have Fifo matched */
    for (i = 0U; i < StrmMgr->MaxStreamer; i++) {
        Strm = &StrmMgr->StreamList[i];
        if (AmbaKAL_MutexTake(&Strm->Mtx, AMBA_KAL_WAIT_FOREVER) != OK) {
            AmbaPrint_PrintUInt5("StrmMgrLiveEos: Mutex acquire fail", \
                            0U, 0U, 0U, 0U, 0U);
            continue;
        }

        if ((Strm->IsLive==1U) && (Strm->FifoHndlr==Hdlr)) {
            Strm->FifoHndlr = 0U;
            Strm->hCodec = 0U;
            Strm->Codec = 0;
        } else if ((Strm->IsLive==1U) && (Strm->FifoHndlr!=0U)) {
            LiveFifoRemain++;
        } else {
            // Do nothing
        }
        (VOID)AmbaKAL_MutexGive(&Strm->Mtx);
    }

    /* check if there is any liveGroup need to be deleted */
    for (i = 0U; i < (UINT32)StrmMgr->MaxLiveGroup; i++) {
        if (AmbaKAL_MutexTake(&StrmMgr->LiveGroup[i].Mtx, AMBA_KAL_WAIT_FOREVER) != OK) {
            AmbaPrint_PrintUInt5("StrmMgrLiveEos: Mutex acquire fail", \
                            0U, 0U, 0U, 0U, 0U);
            continue;
        }

        if (AmbaUtility_StringLength(StrmMgr->LiveGroup[i].Name) != 0U) {
            LiveStreamerRemain = 0U;
            for (j = 0U; j < StrmMgr->MaxStreamerPerLiveGroup; j++) {
                Strm = StrmMgr->LiveGroup[i].Stms[j];
                if (Strm == NULL){
                    continue;
                }
                if (AmbaKAL_MutexTake(&Strm->Mtx, AMBA_KAL_WAIT_FOREVER) != OK) {
                    AmbaPrint_PrintUInt5("StrmMgrLiveEos: Mutex acquire fail", \
                                    0U, 0U, 0U, 0U, 0U);
                    continue;
                }

                if ((Strm->FifoHndlr==0U) && (Strm->Codec == 0U)) {
                    StrmMgr->LiveGroup[i].Stms[j] = NULL;
                } else {
                    LiveStreamerRemain++;
                }
                (VOID)AmbaKAL_MutexGive(&Strm->Mtx);
            }
            if (LiveStreamerRemain == 0U) {
                StrmMgr->LiveGroup[i].Name[0] = '\0';
            }
        }
        (VOID)AmbaKAL_MutexGive(&StrmMgr->LiveGroup[i].Mtx);
    }

    if (LiveFifoRemain == 0U) {
        StrmMgr->LiveState = SVC_RTSP_FIFO_EVENT_END;
    }
}

static void StrmMgrParseVpsSpsPps(AMBA_STREAM_s* Strm, UINT8* Inbuf, UINT32 Insize, UINT32 NeedVPS)
{
#define H265_VPS_TYPE 32U
#define H265_SPS_TYPE 33U
#define H265_PPS_TYPE 34U
#define H264_SPS_TYPE 7U
#define H264_PPS_TYPE 8U
    SVC_STREAM_VIDEO_PARAM_s *Info;
    UINT8 *LastStart, *ptrU8, *Buf;
    const UINT8 *Limit;
    const UINT8 *Base;
    UINT32 W, BufLen;
    UINT32 NaluType;
    const UINT32* ptrU32;
    UINT32 VpsType, SpsType, PpsType;
    char WorkingBuf[(SVC_RTSP_MAX_SPS_LENGTH*2U) + SVC_RTSP_MAX_PPS_LENGTH] = {'\0'};
    UINT32 RetVal = OK;
    ULONG SrcUL, SrcULnd, SrcULrd;
    UINT32 Offset;

    Base = Strm->pBufferBase;
    Limit = Strm->pBufferLimit;
    Info = &Strm->Param.Video;

    if ((Info->PpsLen != 0U) && (Info->Updated != 0U)) {
        /* already got VPS & SPS & PPS */
        RetVal = ERR_NA;
    } else {
        if (NeedVPS == 1U) {
            VpsType = H265_VPS_TYPE;
            SpsType = H265_SPS_TYPE;
            PpsType = H265_PPS_TYPE;
        } else {
            VpsType = 0U;
            SpsType = H264_SPS_TYPE;
            PpsType = H264_PPS_TYPE;
        }

        AmbaMisra_TypeCast(&SrcUL, &Inbuf);
        SrcULnd = SrcUL;
        SrcUL += (SVC_RTSP_MAX_SPS_LENGTH * 2U);
        SrcUL += SVC_RTSP_MAX_PPS_LENGTH;

        AmbaMisra_TypeCast(&SrcULrd, &Limit);
        /* check if our vps/sps/pps cross ring-buffer boundary */
        if (SrcUL > SrcULrd) {
            Offset = SrcULrd-SrcULnd;
            if (AmbaWrap_memcpy(WorkingBuf, Inbuf, SrcULrd-SrcULnd)!= 0U) { }
            if (AmbaWrap_memcpy(&(WorkingBuf[Offset]), Base, ((SVC_RTSP_MAX_SPS_LENGTH*2U)+SVC_RTSP_MAX_PPS_LENGTH) - Offset)!= 0U) { }

            Buf = (UINT8 *)WorkingBuf;
            BufLen = sizeof(WorkingBuf);
        } else {
            Buf = Inbuf;
            BufLen = Insize;
        }

        /* normal case, find start code */
        AmbaMisra_TypeCast(&ptrU32, &Buf);
        if ((*ptrU32 != 0x01000000U) && (((*ptrU32) & 0x00FFFFFFU) != 0x010000U)) {
            AmbaPrint_PrintUInt5("StrmMgrParseVpsSpsPps: cannot find start code", \
                            0U, 0U, 0U, 0U, 0U);
            RetVal = ERR_NA;
        } else {
            /* skip the start code */
            LastStart = ((*ptrU32)==0x01000000U)? &Buf[4] : &Buf[3];
            ptrU8 = LastStart;

            /* h265 - NAL = F(1) | Type(6) | LayerId(6) | TID(3)
               h264 - NAL = F(1) | NRI(2) | Type(5) */
            NaluType = (NeedVPS==1U) ? (((UINT32)*ptrU8 >> 1U) & 0x3fU) : ((UINT32)(*ptrU8) & 0x1fU);

            AmbaMisra_TypeCast(&SrcUL, &ptrU8);
            AmbaMisra_TypeCast(&SrcULnd, &Buf);
            SrcULnd += BufLen;
            SrcULnd -= 4U;
            while (SrcUL < SrcULnd) {
                W = ((((UINT32)ptrU8[0]<<24U) | ((UINT32)ptrU8[1]<<16U)) | ((UINT32)ptrU8[2]<<8U)) | (UINT32)ptrU8[3];

                if ((W==0x00000001U) || ((W & 0xffffff00U)==0x00000100U)) {
                    AmbaMisra_TypeCast(&SrcUL, &ptrU8);
                    AmbaMisra_TypeCast(&SrcULrd, &LastStart);

                    if ((NaluType==VpsType) && (NeedVPS==1U)) {
                        Info->VpsLen = SrcUL - SrcULrd;
                        if (AmbaWrap_memcpy(Info->Vps, LastStart, Info->VpsLen)!= 0U) { }
                    } else if (NaluType == SpsType) {
                        Info->SpsLen = SrcUL - SrcULrd;
                        if (AmbaWrap_memcpy(Info->Sps, LastStart, Info->SpsLen)!= 0U) { }
                    } else if (NaluType == PpsType) {
                        Info->PpsLen = SrcUL - SrcULrd;
                        if (AmbaWrap_memcpy(Info->Pps, LastStart, Info->PpsLen)!= 0U) { }
                    } else {
                        //do nothing
                    }

                    if ((((Info->VpsLen!=0U) || (NeedVPS==0U)) \
                        && (Info->SpsLen != 0U)) && (Info->PpsLen != 0U)) {
                        Info->Updated = 1U;
                        break;
                    }

                    if(ptrU8[3] == (UINT8)0x01) {
                        AmbaMisra_TypeCast(&SrcUL, &ptrU8);
                        SrcUL += 4U;
                        AmbaMisra_TypeCast(&ptrU8, &SrcUL);
                    } else {
                        AmbaMisra_TypeCast(&SrcUL, &ptrU8);
                        SrcUL += 3U;
                        AmbaMisra_TypeCast(&ptrU8, &SrcUL);
                    }

                    LastStart = ptrU8;
                    if (NeedVPS == 1U) {
                        NaluType = ((UINT32)*ptrU8 >> 1U) & 0x3fU;
                    } else {
                        NaluType = ((UINT32)(*ptrU8) & 0x1fU);
                    }
                } else if(ptrU8[3] > (UINT8)1) {
                    AmbaMisra_TypeCast(&SrcUL, &ptrU8);
                    SrcUL += 4U;
                    AmbaMisra_TypeCast(&ptrU8, &SrcUL);
                } else if (ptrU8[2] > (UINT8)1){
                    AmbaMisra_TypeCast(&SrcUL, &ptrU8);
                    SrcUL += 3U;
                    AmbaMisra_TypeCast(&ptrU8, &SrcUL);
                } else if (ptrU8[1] > (UINT8)1){
                    AmbaMisra_TypeCast(&SrcUL, &ptrU8);
                    SrcUL += 2U;
                    AmbaMisra_TypeCast(&ptrU8, &SrcUL);
                } else {
                    AmbaMisra_TypeCast(&SrcUL, &ptrU8);
                    SrcUL += 1U;
                    AmbaMisra_TypeCast(&ptrU8, &SrcUL);
                }
                AmbaMisra_TypeCast(&SrcUL, &ptrU8);
            }
        }
    }
    (VOID)RetVal;
}

static AMBA_STREAM_s* StrmMgrGetSteamerSlot(void)
{
    UINT32 i;
    AMBA_STREAM_s *ptrStrm = NULL;

    for (i=0U; i < StrmMgr->MaxStreamer ; i++) {
        if (AmbaKAL_MutexTake(&StrmMgr->StreamList[i].Mtx, AMBA_KAL_WAIT_FOREVER) != OK) {
            AmbaPrint_PrintUInt5("StrmMgrGetSteamerSlot: Mutex acquire fail", \
                            0U, 0U, 0U, 0U, 0U);
            continue;
        }

        if ((StrmMgr->StreamList[i].FifoHndlr==0U) && (StrmMgr->StreamList[i].Codec==0U)) {
            (VOID)AmbaKAL_MutexGive(&StrmMgr->StreamList[i].Mtx);
            ptrStrm = &StrmMgr->StreamList[i];
            break;
        }
        (VOID)AmbaKAL_MutexGive(&StrmMgr->StreamList[i].Mtx);
    }

    return ptrStrm;
}

static AMBA_STREAM_s* StrmMgrLockStreamerByHandler(UINT32 Hdlr)
{
    UINT32 i, Index = 0xFFFFU;
    AMBA_STREAM_s* Strm = NULL;

    for (i = 0U; i < StrmMgr->MaxStreamer; i++) {
        Strm = &StrmMgr->StreamList[i];
        if (AmbaKAL_MutexTake(&Strm->Mtx, AMBA_KAL_WAIT_FOREVER) != OK) {
            AmbaPrint_PrintUInt5("StrmMgrLockStreamerByHandler: Mutex acquire fail", \
                            0U, 0U, 0U, 0U, 0U);
            continue;
        }

        if (Hdlr == Strm->FifoHndlr) {
            Index = i;
            break;
        } else {
            (VOID)AmbaKAL_MutexGive(&Strm->Mtx);
        }
    }

    if (Index == 0xFFFFU) {
        AmbaPrint_PrintUInt5("StrmMgrLockStreamerByHandler: Unknown Hdlr %ptrU8. Skip", \
                        Hdlr, 0U, 0U, 0U, 0U);
        Strm = NULL;
    }

    return Strm;
}

static UINT32 StrmMgrLockStreamer(AMBA_STREAM_s* Strm)
{
    UINT32 RetVal = OK;

    if (AmbaKAL_MutexTake(&Strm->Mtx, AMBA_KAL_WAIT_FOREVER) != OK) {
        AmbaPrint_PrintUInt5("StrmMgrLockStreamer: Mutex acquire fail", \
                        0U, 0U, 0U, 0U, 0U);
        RetVal = ERR_NA;
    }
    return RetVal;
}

static void StrmMgrUnlockStreamer(AMBA_STREAM_s* Strm)
{
    (VOID)AmbaKAL_MutexGive(&Strm->Mtx);
}

static void StrmMgrPlaybackTimer(UINT32 Hdlr)
{
    SVC_STREAM_MSG_S Msg;
    UINT32 RetVal;

    Msg.Event = STREAMER_MSG_PERIODIC_DATA_READY;
    Msg.Info = Hdlr;
    RetVal = AmbaKAL_MsgQueueSend(&StrmMgr->MsgQueue, &Msg, AMBA_KAL_NO_WAIT);

    if (RetVal != OK) {
        AmbaPrint_PrintUInt5("StrmMgrPlaybackTimer: send DATA_READY message fail", \
                        0U, 0U, 0U, 0U, 0U);
    }
}

static UINT32 StrmMgrRegisterTimer(AMBA_STREAM_s* Strm)
{
    static char SvcStreamer[] = "SvcStreamer Timer";
    UINT32 RetVal = OK;

    if ((Strm->TimerReg!=1U) && (Strm->Enable==1U)) {
        if (Strm->IsLive == 0U) {
            Strm->Deviation = (DOUBLE)0;
            RetVal = AmbaKAL_TimerCreate(&Strm->Timer, SvcStreamer, \
                        StrmMgrPlaybackTimer, Strm->FifoHndlr, \
                        Strm->TimerTick, Strm->TimerTick, AMBA_KAL_AUTO_START);

            if (OK != RetVal) {
                AmbaPrint_PrintUInt5("[StreamerMgr] playback stream Timer register fail", \
                                0U, 0U, 0U, 0U, 0U);
                RetVal = ERR_NA;
            } else {
                Strm->TimerReg = 1;
            }
        }
    }

    return RetVal;
}

static UINT32 StrmMgrUnregisterTimer(AMBA_STREAM_s* Strm)
{
    UINT32 RetVal = OK;

    if ((Strm->IsLive==0U) && (Strm->TimerReg==1U)) {
        if (OK != AmbaKAL_TimerDelete(&Strm->Timer)) {
            AmbaPrint_PrintUInt5("[StreamerMgr] playback stream Timer unregister fail", \
                            0U, 0U, 0U, 0U, 0U);
            RetVal = ERR_NA;
        } else {
            Strm->TimerReg = 0U;
        }
    }

    return RetVal;
}

static UINT32 StrmMgrFeedEosFrame(UINT32 Hdlr)
{
    AMBA_STREAM_s* Strm;
    SVC_RTSP_FRAME_DESC_s Desc;
    UINT32 RetVal = OK;

    Strm = StrmMgrLockStreamerByHandler(Hdlr);
    if (Strm == NULL) {
        RetVal = ERR_NA;
    } else {
        Desc.Type = SVC_RTSP_FRAME_TYPE_EOS;
        Desc.Size = SVC_RTSP_FRAME_MARK_EOS;
        if (Strm->Enable == 1U) {
            Strm->StreamFunc(&Desc, Strm->StreamCtx);
        }

        StrmMgrUnlockStreamer(Strm);
    }

    return RetVal;
}

static UINT32 StrmMgrProcessFrame(const SVC_STREAM_MSG_S* Msg)
{
    AMBA_STREAM_s* Strm;
    SVC_RTSP_FRAME_DESC_s Desc = Msg->Desc;
    UINT8 *pSrcU8;
    UINT32 RetVal = OK;
    UINT32 Hdlr = Msg->Info;
    ULONG DescStartAddr;
    UINT32 NeedVPS = 0U;

    Strm = StrmMgrLockStreamerByHandler(Hdlr);
    if (Strm == NULL) {
        RetVal = ERR_NA;
    } else {
        if ((Strm->Param.Video.Updated == 0U) && (Strm->TrackType == SVC_MEDIA_TRACK_TYPE_VIDEO)) {
            AmbaPrint_PrintUInt5("[StreamerMgr] Desc->Type = %u , Desc->Size = %u", \
                            Desc.Type, Desc.Size, 0U, 0U, 0U);
            if ((Desc.Type == SVC_RTSP_FRAME_TYPE_IDR_FRAME) && (Desc.Size != SVC_RTSP_FRAME_MARK_EOS)) {
                if ((Strm->MediaID == AMBA_FORMAT_MID_H264) || (Strm->MediaID == AMBA_FORMAT_MID_AVC) ||
                    (Strm->MediaID == AMBA_FORMAT_MID_H265) || (Strm->MediaID == AMBA_FORMAT_MID_HVC)) {
                    AmbaPrint_PrintUInt5("[StreamerMgr] parse Sps Pps for video(%x)", \
                                    0U, 0U, 0U, 0U, 0U);\
                    DescStartAddr = Desc.StartAddr;
                    AmbaMisra_TypeCast(&pSrcU8, &DescStartAddr);
                    if ((Strm->MediaID == AMBA_FORMAT_MID_H265) || (Strm->MediaID == AMBA_FORMAT_MID_HVC)) {
                        NeedVPS = 1U;
                    }
                    StrmMgrParseVpsSpsPps(Strm, pSrcU8, Desc.Size, NeedVPS);
                }
            }
        }

        if (Strm->Enable == 1U) {
            Strm->StreamFunc(&Desc, Strm->StreamCtx);
        }

        if(Desc.Size == SVC_RTSP_FRAME_MARK_EOS){
            AmbaPrint_PrintUInt5("%s: Fifo Event size is EOS mark!(%x)", \
                            Desc.Size, 0U, 0U, 0U, 0U);
            StrmMgrLiveEos(Strm->FifoHndlr);
        }

        StrmMgrUnlockStreamer(Strm);
    }

    return RetVal;
}

static UINT32 StrmMgrPeriodicProcessFrame(const SVC_STREAM_MSG_S* Msg)
{
    AMBA_STREAM_s* Strm;
    UINT32 Hdlr = Msg->Info;
    SVC_RTSP_FRAME_DESC_s Desc = Msg->Desc;
    UINT32 RetVal = OK;

    Strm = StrmMgrLockStreamerByHandler(Hdlr);
    if (Strm == NULL) {
        RetVal = ERR_NA;
    } else {
        if (Strm->TimerReg == 0U) {
            StrmMgrUnlockStreamer(Strm);
            RetVal = ERR_NA;
        } else {
            /* don't do process frame if deviation larger than one frame period */
            if (Strm->Deviation > (DOUBLE)Strm->TimerTick) {
                Strm->Deviation -= (DOUBLE)Strm->TimerTick;
                StrmMgrUnlockStreamer(Strm);
                RetVal = ERR_NA;
            } else {
                Strm->Deviation += Strm->DeviationPerFrame;
            }

            if (ERR_NA != RetVal) {
                if (Strm->Enable == 1U) {
                    StrmMgrUnlockStreamer(Strm);
                    if (StrmMgrLockStreamer(Strm) != OK) {
                        RetVal = ERR_NA;
                    }

                    if (ERR_NA != RetVal) {
                        Strm->StreamFunc(&Desc, Strm->StreamCtx);
                    }
                }

                if (ERR_NA != RetVal) {
                    StrmMgrUnlockStreamer(Strm);
                }
            }
        }
    }

    return RetVal;
}

static inline VOID StrmMgrCopyTrackInfo(AMBA_STREAM_s* Strm, const SVC_RTSP_MEDIA_TRACK_CFG_s* Trk, UINT32 TrackID)
{
    AmbaMisra_TypeCast(&(Strm->hCodec), &(Trk->hCodec));
    Strm->Codec = Trk->Codec;
    Strm->TrackID = TrackID;
    Strm->MediaID = Trk->nMediaId;
    Strm->TimeScale = Trk->nTimeScale;
    Strm->TickPerFrame = Trk->nTimePerFrame;
    Strm->TrackType = Trk->nTrackType;
    Strm->pBufferBase = Trk->pBufferBase;
    Strm->pBufferLimit = Trk->pBufferLimit;
    Strm->IsLive = 1U;

    if (Strm->TrackType == SVC_MEDIA_TRACK_TYPE_VIDEO) {
        Strm->Param.Video.M = Trk->Info.Video.nM;
        Strm->Param.Video.N = Trk->Info.Video.nN;
        Strm->Param.Video.GOPSize = Trk->Info.Video.nGOPSize;
        Strm->Param.Video.IRCycle = Trk->Info.Video.nIRCycle;
        Strm->Param.Video.RecoveryFrameCnt = Trk->Info.Video.nRecoveryFrameCnt;
        Strm->Param.Video.Updated = 0U;
    } else if (Strm->TrackType == SVC_MEDIA_TRACK_TYPE_AUDIO) {
        Strm->Param.Audio.Default= Trk->Info.Audio.Default;
        Strm->Param.Audio.SampleRate= Trk->Info.Audio.SampleRate;
        Strm->Param.Audio.Channels= Trk->Info.Audio.Channels;
        Strm->Param.Audio.BitsPerSample= Trk->Info.Audio.BitsPerSample;
    } else {
        // Do nothing
    }
}

static UINT32 StrmMgrGetHandler(UINT32 Codec)
{
    UINT32 RetVal;

    if (Codec == 0x1U) {
        RetVal = 0x1U;
    } else if (Codec == 0x2U) {
        RetVal = 0x2U;
    } else {
        RetVal = 0U;
    }
    (VOID)AmbaKAL_TaskSleep(500U);

    return RetVal;
}

static INT32 StrmMgrSwitchSessionImpl(UINT32 StreamIndex, UINT32 GroupIndex)
{
    UINT32 i, j, RetVal;
    INT32 Status = -1;
    AMBA_STREAM_s* Strm = NULL;
    SVC_RTSP_MOVIE_INFO_CFG_s MInfo;

    if (AmbaWrap_memset(&MInfo, 0, sizeof(SVC_RTSP_MOVIE_INFO_CFG_s))!= 0U) { }
    RetVal = SvcRtspStrmMgrGetMediaInfo(NULL, StreamIndex, &MInfo);
    if ((RetVal != OK) || (MInfo.nTrack == 0U)) {
        AmbaPrint_PrintUInt5("StrmMgrSwitchSessionImpl: no track in MediaInfo", \
                        0U, 0U, 0U, 0U, 0U);
        RetVal = ERR_NA;
    } else if (MInfo.nTrack > StrmMgr->MaxStreamerPerLiveGroup) {
        MInfo.nTrack = StrmMgr->MaxStreamerPerLiveGroup;
        AmbaPrint_PrintUInt5("[StreamerMgr] No enough space to create all tracks in stream(%u)", \
                        StreamIndex, 0U, 0U, 0U, 0U);
    } else {
        // Do nothing
    }

    if (ERR_NA != RetVal) {
        for (i = 0U; i < MInfo.nTrack; i++) {
            for (j = 0U; j < StrmMgr->MaxStreamerPerLiveGroup; j++) {
                Strm = StrmMgr->LiveGroup[GroupIndex].Stms[j];
                RetVal = AmbaKAL_MutexTake(&Strm->Mtx, AMBA_KAL_WAIT_FOREVER);
                if ((Strm == NULL) || (RetVal != OK)) {
                    AmbaPrint_PrintUInt5("StrmMgrSwitchSessionImpl: Mutex acquire fail", \
                                    0U, 0U, 0U, 0U, 0U);
                    continue;
                }

                if ((Strm->TrackID == i) && (Strm->MediaID == MInfo.Track[i].nMediaId)) {
                    break;
                } else {
                    (VOID)AmbaKAL_MutexGive(&Strm->Mtx);
                }
            }

            if (NULL != Strm) {
                Strm->FifoHndlr = StrmMgrGetHandler(MInfo.Track[i].Codec);
                if (Strm->FifoHndlr == 0U) {
                    Status = -1;
                    (VOID)AmbaKAL_MutexGive(&Strm->Mtx);
                    break;
                } else {
                    StrmMgrCopyTrackInfo(Strm, &MInfo.Track[i], i);
                    StrmMgr->LiveGroup[GroupIndex].Stms[i] = Strm;
                    Status = 0;
                    (VOID)AmbaKAL_MutexGive(&Strm->Mtx);
                }
            }
        }
    }

    return Status;
}

static UINT32 StrmMgrStartSwitchSession(const SVC_RTSP_MEDIA_STREAM_ITEM_LIST_s* SList)
{
    UINT32 GroupIndex, StreamIndex;
    INT32 Status = 0, CmpRslt;
    UINT32 ProcessedStream[SVC_RTSP_STREAMLIST_MAX_ENTRY] = {0U};
    UINT32 RetVal = OK;

    if (AmbaWrap_memset(ProcessedStream, 0, sizeof(UINT32)*SVC_RTSP_STREAMLIST_MAX_ENTRY)!= 0U) {
        /* do nothing */
    }

    GroupIndex = 0U;
    while ((GroupIndex < StrmMgr->MaxLiveGroup) && (RetVal != ERR_NA)) {
        if (AmbaKAL_MutexTake(&StrmMgr->LiveGroup[GroupIndex].Mtx, AMBA_KAL_WAIT_FOREVER) != OK) {
            AmbaPrint_PrintUInt5("StrmMgrStartSwitchSession: Mutex acquire fail", \
                            0U, 0U, 0U, 0U, 0U);
            RetVal = ERR_NA;
            break;
        }

        for (StreamIndex = 0U; StreamIndex < SList->Amount; StreamIndex++) {
            /* find match, fill mediainfo and fifoHndlr, record matched StreamItem */
            CmpRslt = AmbaUtility_StringCompare(SList->StreamItemList[StreamIndex].Name, StrmMgr->LiveGroup[GroupIndex].Name, AmbaUtility_StringLength(SList->StreamItemList[StreamIndex].Name));
            if ((CmpRslt == 0) && (SList->StreamItemList[StreamIndex].Active == 1U)) {
                Status = StrmMgrSwitchSessionImpl(StreamIndex, GroupIndex);
                ProcessedStream[StreamIndex] = 1U;
                break;
            }
        }

        if (StreamIndex == (UINT32)SList->Amount) {
            StrmMgr->LiveGroup[GroupIndex].Name[0] = '\0';
            (VOID)AmbaKAL_MutexGive(&StrmMgr->LiveGroup[GroupIndex].Mtx);
        } else {
            (VOID)AmbaKAL_MutexGive(&StrmMgr->LiveGroup[GroupIndex].Mtx);
            if (Status < 0) {
                RetVal = ERR_NA;
            } else {
                AmbaPrint_PrintStr5("[StreamerMgr] Resume stream (%s) success",
                    StrmMgr->LiveGroup[GroupIndex].Name, NULL, NULL, NULL, NULL);
            }
        }
        GroupIndex++;
    }

    if (ERR_NA != RetVal) {
        /* for the rest of streamer, find available group and create them */
        StreamIndex = 0U;
        while (((StreamIndex < (UINT32)SList->Amount) && (ProcessedStream[StreamIndex] == 0U)) &&
                ((SList->StreamItemList[StreamIndex].Active == 1U) && (RetVal != ERR_NA))) {

            for (GroupIndex = 0U; (GroupIndex<StrmMgr->MaxLiveGroup); GroupIndex++) {
                if (RetVal == ERR_NA) {
                    continue;
                }

                if (AmbaKAL_MutexTake(&StrmMgr->LiveGroup[GroupIndex].Mtx, AMBA_KAL_WAIT_FOREVER) != OK) {
                    AmbaPrint_PrintUInt5("StrmMgrStartSwitchSession: Mutex acquire fail", \
                                    0U, 0U, 0U, 0U, 0U);
                    RetVal = ERR_NA;
                    continue;
                }

                if(AmbaUtility_StringLength(StrmMgr->LiveGroup[GroupIndex].Name) == 0U){
                    Status = StrmMgrSwitchSessionImpl(StreamIndex, GroupIndex);
                    (VOID)AmbaKAL_MutexGive(&StrmMgr->LiveGroup[GroupIndex].Mtx);

                    if (Status >= 0) {
                        AmbaPrint_PrintStr5("[StreamerMgr] Resume stream (%s) success",
                            StrmMgr->LiveGroup[GroupIndex].Name, NULL, NULL, NULL, NULL);
                        continue;
                    } else {
                        break;
                    }
                }

                (VOID)AmbaKAL_MutexGive(&StrmMgr->LiveGroup[GroupIndex].Mtx);
            }
            StreamIndex++;
        }

        if (ERR_NA != RetVal) {
            if (Status >= 0) {
                AmbaPrint_PrintStr5("[StreamerMgr] Resume from Switch Session for all stream",
                                    NULL, NULL, NULL, NULL, NULL);
                StrmMgr->LiveState = SVC_RTSP_FIFO_EVENT_START;
                RetVal = OK;
            } else {
                RetVal = ERR_NA;
            }
        }
    }

    return RetVal;
}

static UINT32 StrmMgrStartEncode(void)
{
    UINT32 i, GroupIndex, StreamIndex, RetVal = OK, RetVal2 = OK;
    INT32 Status = -1;
    AMBA_STREAM_s* Strm;
    SVC_RTSP_MEDIA_STREAM_ITEM_LIST_s SList;
    SVC_RTSP_MOVIE_INFO_CFG_s    MInfo;

    if (AmbaWrap_memset(&SList, 0, sizeof(SVC_RTSP_MEDIA_STREAM_ITEM_LIST_s))!= 0U) { }
    if (AmbaWrap_memset(&MInfo, 0, sizeof(SVC_RTSP_MOVIE_INFO_CFG_s))!= 0U) { }

    if (StrmMgr->LiveState == SVC_RTSP_FIFO_EVENT_START) {
        AmbaPrint_PrintStr5("[StreamerMgr] live streams are already inited, ignore START_ENCODE request",
                            NULL, NULL, NULL, NULL, NULL);
        RetVal = ERR_NA;
    } else {
        RetVal = SvcRtspStrmMgrGetStreamList(NULL, 0U, &SList);

        if ((RetVal != OK) || (SList.Amount < 1U)) {
            AmbaPrint_PrintUInt5("StrmMgrStartEncode: no stream in streamList", \
                            0U, 0U, 0U, 0U, 0U);
            RetVal = ERR_NA;
        } else {
            if (StrmMgr->LiveState == SVC_RTSP_FIFO_EVENT_SWITCHENCSESSION) {
                RetVal = StrmMgrStartSwitchSession(&SList);
            } else {
                StreamIndex = 0U;
                GroupIndex = 0U;
                for (; StreamIndex < SList.Amount; StreamIndex++) {
                    AmbaPrint_PrintUInt5("Amount %d", SList.Amount, 0, 0, 0, 0);
                    if (SList.StreamItemList[StreamIndex].Active == 0U) {
                        continue;
                    }

                    if (GroupIndex >= StrmMgr->MaxLiveGroup) {
                        AmbaPrint_PrintUInt5("[StreamerMgr] No enough space for all Active live stream", \
                                        0U, 0U, 0U, 0U, 0U);
                    } else {
                        RetVal = SvcRtspStrmMgrGetMediaInfo(NULL, StreamIndex, &MInfo);
                        if ((RetVal != OK) || (MInfo.nTrack == 0U)) {
                            AmbaPrint_PrintUInt5("StrmMgrStartEncode: no track in MediaInfo", \
                                            0U, 0U, 0U, 0U, 0U);
                            continue;
                        } else if (MInfo.nTrack > StrmMgr->MaxStreamerPerLiveGroup) {
                            MInfo.nTrack = StrmMgr->MaxStreamerPerLiveGroup;

                            AmbaPrint_PrintUInt5("[StreamerMgr] No enough space to create all tracks in stream(%u)", \
                                            StreamIndex, 0U, 0U, 0U, 0U);
                        } else {
                            // Do nothing
                        }

                        if (AmbaKAL_MutexTake(&StrmMgr->LiveGroup[GroupIndex].Mtx, AMBA_KAL_WAIT_FOREVER) != OK) {
                            AmbaPrint_PrintUInt5("StrmMgrStartEncode: Mutex acquire fail", \
                                            0U, 0U, 0U, 0U, 0U);
                            continue;
                        }

                        AmbaPrint_PrintUInt5("nTrack %d", MInfo.nTrack, 0, 0, 0, 0);

                        RetVal2 = OK;
                        for (i=0U; i < MInfo.nTrack; i++) {
                            if (OK != RetVal2) {
                                break;
                            }

                            Strm = StrmMgrGetSteamerSlot();
                            if (Strm == NULL) {
                                AmbaPrint_PrintUInt5("StrmMgrStartEncode: no available Streamer instance", \
                                                0U, 0U, 0U, 0U, 0U);
                                Status = -1;
                                RetVal2 = ERR_NA;
                                continue;
                            }

                            if (AmbaKAL_MutexTake(&Strm->Mtx, AMBA_KAL_WAIT_FOREVER) != OK) {
                                AmbaPrint_PrintUInt5("StrmMgrStartEncode: Mutex acquire fail", \
                                                0U, 0U, 0U, 0U, 0U);
                                Status = -1;
                                RetVal2 = ERR_NA;
                                continue;
                            }
                            AmbaPrint_PrintUInt5("i %d, Codec %d", i, MInfo.Track[i].Codec, 0, 0, 0);
                            Strm->FifoHndlr = StrmMgrGetHandler(MInfo.Track[i].Codec);

                            if (Strm->FifoHndlr == 0U) {
                                Status = -1;
                                AmbaPrint_PrintUInt5("StrmMgrStartEncode: virtual Fifo create fail", \
                                                0U, 0U, 0U, 0U, 0U);
                                (VOID)AmbaKAL_MutexGive(&Strm->Mtx);
                                RetVal2 = ERR_NA;
                                continue;
                            } else {
                                StrmMgrCopyTrackInfo(Strm, &MInfo.Track[i], i);
                                StrmMgr->LiveGroup[GroupIndex].Stms[i] = Strm; //bind streamer into LiveGroup
                                Status = 0;
                                (VOID)AmbaKAL_MutexGive(&Strm->Mtx);
                            }
                        }

                        if (Status >= 0) {
                            AmbaUtility_StringCopy(StrmMgr->LiveGroup[GroupIndex].Name, STREAM_MGR_MAX_STREAM_NAME_LENGTH, SList.StreamItemList[StreamIndex].Name);
                            AmbaPrint_PrintStr5("[StreamerMgr] Start stream (%s) success",
                                                StrmMgr->LiveGroup[GroupIndex].Name, NULL, NULL, NULL, NULL);
                            (VOID)AmbaKAL_MutexGive(&StrmMgr->LiveGroup[GroupIndex].Mtx);
                            GroupIndex++;
                        } else {
                            (VOID)AmbaKAL_MutexGive(&StrmMgr->LiveGroup[GroupIndex].Mtx);
                            break;
                        }
                    }
                }

                if (Status >= 0) {
                    AmbaPrint_PrintStr5("[StreamerMgr] Start all live streams success",
                                        StrmMgr->LiveGroup[GroupIndex].Name, NULL, NULL, NULL, NULL);
                    StrmMgr->LiveState = SVC_RTSP_FIFO_EVENT_START;
                    RetVal = OK;
                } else {
                    AmbaPrint_PrintUInt5("StrmMgrStartEncode fail", \
                                    0U, 0U, 0U, 0U, 0U);
                    RetVal = ERR_NA;
                }
            }
        }
    }

    return RetVal;
}

static UINT32 StrmMgrStopEncode(void)
{
    UINT32 i, j;
    AMBA_STREAM_s* Strm;
    UINT32 RetVal = OK;

    if (StrmMgr->LiveState != SVC_RTSP_FIFO_EVENT_START) {
        AmbaPrint_PrintStr5("[StreamerMgr] live streams are not inited, ignore STOP_ENCODE request",
                            NULL, NULL, NULL, NULL, NULL);
        RetVal = ERR_NA;
    } else {
        for (i = 0U; i < StrmMgr->MaxLiveGroup; i++) {
            if (AmbaKAL_MutexTake(&StrmMgr->LiveGroup[i].Mtx, AMBA_KAL_WAIT_FOREVER) != OK) {
                AmbaPrint_PrintUInt5("StrmMgrStopEncode: Mutex acquire fail", \
                                0U, 0U, 0U, 0U, 0U);
                continue;
            }

            if (AmbaUtility_StringLength(StrmMgr->LiveGroup[i].Name) != 0U) {
                for (j = 0U; j < StrmMgr->MaxStreamerPerLiveGroup; j++) {
                    Strm = StrmMgr->LiveGroup[i].Stms[j];
                    if (Strm == NULL){
                        continue;
                    }

                    if (AmbaKAL_MutexTake(&Strm->Mtx, AMBA_KAL_WAIT_FOREVER) != OK) {
                        AmbaPrint_PrintUInt5("StrmMgrStopEncode: Mutex acquire fail", \
                                        0U, 0U, 0U, 0U, 0U);
                        continue;
                    }

                    Strm->FifoHndlr = 0U;
                    Strm->hCodec = 0U;
                    Strm->Codec = 0U;

                    (VOID)AmbaKAL_MutexGive(&Strm->Mtx);
                    StrmMgr->LiveGroup[i].Stms[j] = NULL;
                }

                StrmMgr->LiveGroup[i].Name[0] = '\0';
            }

            (VOID)AmbaKAL_MutexGive(&StrmMgr->LiveGroup[i].Mtx);
        }

        StrmMgr->LiveState = SVC_RTSP_FIFO_EVENT_END;
    }

    return RetVal;
}

static UINT32 StrmMgrSwitchSession(void)
{
    UINT32 i, j;
    AMBA_STREAM_s* Strm;
    UINT32 RetVal = OK;

    if (StrmMgr->LiveState != SVC_RTSP_FIFO_EVENT_START) {
        AmbaPrint_PrintUInt5("[StreamerMgr] live streams are not inited, ignore SWITCH_SESSION request", \
                        0U, 0U, 0U, 0U, 0U);
        RetVal = ERR_NA;
    } else {
        for (i = 0U; i < StrmMgr->MaxLiveGroup; i++) {
            if (AmbaKAL_MutexTake(&StrmMgr->LiveGroup[i].Mtx, AMBA_KAL_WAIT_FOREVER) != OK) {
                AmbaPrint_PrintUInt5("StrmMgrSwitchSession: Mutex acquire fail", \
                                0U, 0U, 0U, 0U, 0U);
                continue;
            }

            if (AmbaUtility_StringLength(StrmMgr->LiveGroup[i].Name) != 0U) {
                for (j = 0U; j < StrmMgr->MaxStreamerPerLiveGroup; j++) {
                    Strm = StrmMgr->LiveGroup[i].Stms[j];
                    if (Strm == NULL){
                        continue;
                    }

                    if (AmbaKAL_MutexTake(&Strm->Mtx, AMBA_KAL_WAIT_FOREVER) != OK) {
                        AmbaPrint_PrintUInt5("StrmMgrSwitchSession: Mutex acquire fail", \
                                        0U, 0U, 0U, 0U, 0U);
                        continue;
                    }

                    Strm->FifoHndlr = 0U;
                    Strm->hCodec = 0U;
                    Strm->Codec = 0U;

                    (VOID)AmbaKAL_MutexGive(&Strm->Mtx);
                }
            }
            (VOID)AmbaKAL_MutexGive(&StrmMgr->LiveGroup[i].Mtx);
        }

        StrmMgr->LiveState = SVC_RTSP_FIFO_EVENT_SWITCHENCSESSION;
    }

    return RetVal;
}

static void* StrmMgrTaskEntry(void *Input)
{
    UINT32 RetVal = OK;
    SVC_STREAM_MSG_S Msg;
    AMBA_STREAM_s* Strm;

    AmbaMisra_TouchUnused(Input);

    AmbaPrint_PrintStr5("[SVC] StreamerMgr started", NULL, NULL, NULL, NULL, NULL);
    while (RetVal == OK) {
        RetVal = AmbaKAL_MsgQueueReceive(&StrmMgr->MsgQueue, &Msg, AMBA_KAL_WAIT_FOREVER);
        if (RetVal != OK) {
            AmbaPrint_PrintUInt5("StrmMgrTaskEntry: MsgQueueReceive Error.", 0U, 0U, 0U, 0U, 0U);
            continue;
        }

        switch (Msg.Event) {
            case STREAMER_MSG_DATA_READY:
                (VOID)StrmMgrProcessFrame(&Msg);
                break;
            case STREAMER_MSG_PERIODIC_DATA_READY:
                (VOID)StrmMgrPeriodicProcessFrame(&Msg);
                break;
            case STREAMER_MSG_EOS:
                (VOID)StrmMgrFeedEosFrame(Msg.Info);
                StrmMgrLiveEos(Msg.Info);
                break;
            case STREAMER_MSG_ENABLE:
                AmbaMisra_TypeCast(&Strm, &Msg.Info);
                if (AmbaKAL_MutexTake(&Strm->Mtx, AMBA_KAL_WAIT_FOREVER) != OK) {
                    AmbaPrint_PrintUInt5("StrmMgrTaskEntry: Mutex acquire fail", \
                                    0U, 0U, 0U, 0U, 0U);
                } else {
                    if (Strm->Enable == 0U) {
                        Strm->Enable = 1U;
                    }

                    (VOID)AmbaKAL_MutexGive(&Strm->Mtx);
                }
                break;
            case STREAMER_MSG_DISABLE:
                AmbaMisra_TypeCast(&Strm, &Msg.Info);
                if (AmbaKAL_MutexTake(&Strm->Mtx, AMBA_KAL_WAIT_FOREVER) != OK) {
                    AmbaPrint_PrintUInt5("StrmMgrTaskEntry: Mutex acquire fail", \
                                    0U, 0U, 0U, 0U, 0U);
                } else {
                    (VOID)StrmMgrUnregisterTimer(Strm);
                    if (Strm->Enable == 1U) {
                        Strm->Enable = 0U;
                    }
                    (VOID)AmbaKAL_MutexGive(&Strm->Mtx);
                }
                break;
            case STREAMER_MSG_RESET_FIFO:
                AmbaMisra_TypeCast(&Strm, &Msg.Info);
                if (AmbaKAL_MutexTake(&Strm->Mtx, AMBA_KAL_WAIT_FOREVER) != OK) {
                    AmbaPrint_PrintUInt5("StrmMgrTaskEntry: Mutex acquire fail", \
                                    0U, 0U, 0U, 0U, 0U);
                } else {
                    (VOID)AmbaKAL_MutexGive(&Strm->Mtx);
                }
                break;
            case STREAMER_MSG_LIVE_STARTENC:
                (VOID)StrmMgrStartEncode();
                break;
            case STREAMER_MSG_LIVE_STOPENC:
                (VOID)StrmMgrStopEncode();
                break;
            case STREAMER_MSG_LIVE_SWITCHSESSION:
                (VOID)StrmMgrSwitchSession();
                break;
            default:
                // Do nothing
                break;
        }
    }

    return NULL;
}

UINT32 SvcRtspStrmNotify(UINT32 hndlr, const SVC_RTSP_FRAME_DESC_s *Desc)
{
    UINT32 RetVal;
    SVC_STREAM_MSG_S Msg;

    if (AmbaWrap_memset(&Msg, 0, sizeof(SVC_STREAM_MSG_S))!= 0U) { }
    Msg.Desc.SeqNum=  Desc->SeqNum;
    Msg.Desc.Pts=  Desc->Pts;
    Msg.Desc.Type=  Desc->Type;
    Msg.Desc.Completed=  Desc->Completed;
    Msg.Desc.Align = Desc->Align;
    Msg.Desc.StartAddr=  Desc->StartAddr;
    Msg.Desc.Size=  Desc->Size;
    Msg.Info = hndlr;

    if (SVC_RTSP_FRAME_TYPE_EOS == Desc->Type) {
        Msg.Event = STREAMER_MSG_EOS;
        RetVal = AmbaKAL_MsgQueueSend(&StrmMgr->MsgQueue, &Msg, AMBA_KAL_NO_WAIT);

        if (RetVal != OK) {
            AmbaPrint_PrintStr5("send EOS message fail", NULL, NULL, NULL, NULL, NULL);
            (VOID)StrmMgrFeedEosFrame(Msg.Info);
            StrmMgrLiveEos(Msg.Info);
        }
    } else {
        Msg.Event = STREAMER_MSG_DATA_READY;
        RetVal = AmbaKAL_MsgQueueSend(&StrmMgr->MsgQueue, &Msg, AMBA_KAL_NO_WAIT);

        if (RetVal != OK) {
            AmbaPrint_PrintUInt5("SvcRtspStrmNotify: send DATA_READY message fail, %u", RetVal, 0U, 0U, 0U, 0U);
        }
    }

    return RetVal;
}

UINT32 SvcRtspStrmGetVpsSpsPps(AMBA_STREAM_s* Strm,
                                            char *Vps, UINT32 *VpsLen,
                                            char *Sps, UINT32 *SpsLen,
                                            char *Pps, UINT32 *PpsLen)
{
    UINT32 RetVal = OK;

    if (((((Strm == NULL) || (Vps == NULL)) || (Sps == NULL)) || (Pps == NULL))
            || ((VpsLen == NULL) || ((SpsLen == NULL) || (PpsLen == NULL)))) {
        AmbaPrint_PrintUInt5("SvcRtspStrmGetVpsSpsPps: Invalid Input.", \
                        0U, 0U, 0U, 0U, 0U);
        RetVal = ERR_NA;
    } else {
        if (AmbaKAL_MutexTake(&Strm->Mtx, AMBA_KAL_WAIT_FOREVER) != OK) {
            AmbaPrint_PrintUInt5("SvcRtspStrmGetVpsSpsPps: Mutex acquire fail", \
                            0U, 0U, 0U, 0U, 0U);
            RetVal = ERR_NA;
        } else {
            if (Strm->Param.Video.Updated == 0U) {
                AmbaPrint_PrintUInt5("SvcRtspStrmGetVpsSpsPps: Vps Sps Pps not Updated yet", \
                                0U, 0U, 0U, 0U, 0U);
                (VOID)AmbaKAL_MutexGive(&Strm->Mtx);
                RetVal = ERR_NA;
            } else {
                *VpsLen = Strm->Param.Video.VpsLen;
                if (AmbaWrap_memcpy(Vps, Strm->Param.Video.Vps, *VpsLen)!= 0U) { }

                *SpsLen = Strm->Param.Video.SpsLen;
                if (AmbaWrap_memcpy(Sps, Strm->Param.Video.Sps, *SpsLen)!= 0U) { }

                *PpsLen = Strm->Param.Video.PpsLen;
                if (AmbaWrap_memcpy(Pps, Strm->Param.Video.Pps, *PpsLen)!= 0U) { }
                (VOID)AmbaKAL_MutexGive(&Strm->Mtx);
            }
        }
    }

    return RetVal;
}

UINT32 SvcRtspStrmGetSpsPps(AMBA_STREAM_s* Strm,
                                        char *Sps, UINT32 *SpsLen,
                                        char *Pps, UINT32 *PpsLen,
                                        UINT32 *profileLevelID)
{
    UINT32 RetVal = OK;

    if ((((Strm == NULL) || (Sps == NULL)) || ((SpsLen == NULL) || (Pps == NULL))) || \
        ((PpsLen == NULL) || (profileLevelID == NULL))) {
        AmbaPrint_PrintUInt5("SvcRtspStrmGetSpsPps: Invalid Input.", \
                        0U, 0U, 0U, 0U, 0U);
        RetVal = ERR_NA;
    } else {
        if (AmbaKAL_MutexTake(&Strm->Mtx, AMBA_KAL_WAIT_FOREVER) != OK) {
            AmbaPrint_PrintUInt5("SvcRtspStrmGetSpsPps: Mutex acquire fail", \
                            0U, 0U, 0U, 0U, 0U);
            RetVal = ERR_NA;
        } else {
            if (Strm->Param.Video.Updated == 0U) {
                AmbaPrint_PrintUInt5("SvcRtspStrmGetSpsPps: Sps Pps not Updated yet", \
                                0U, 0U, 0U, 0U, 0U);
                (VOID)AmbaKAL_MutexGive(&Strm->Mtx);
                RetVal = ERR_NA;
            } else {
                *SpsLen = Strm->Param.Video.SpsLen;
                if (AmbaWrap_memcpy(Sps, Strm->Param.Video.Sps, *SpsLen)!= 0U) { }

                *PpsLen = Strm->Param.Video.PpsLen;
                if (AmbaWrap_memcpy(Pps, Strm->Param.Video.Pps, *PpsLen)!= 0U) { }

                *profileLevelID = Strm->Param.Video.ProfileLevelId;
                (VOID)AmbaKAL_MutexGive(&Strm->Mtx);
            }
        }
    }

    return RetVal;
}

void SvcRtspStrmResetFifo(const AMBA_STREAM_s* Strm)
{
    UINT32 RetVal;
    SVC_STREAM_MSG_S Msg;

    Msg.Event = STREAMER_MSG_RESET_FIFO;
    AmbaMisra_TypeCast(&Msg.Info, &Strm);

    RetVal = AmbaKAL_MsgQueueSend(&StrmMgr->MsgQueue, &Msg, AMBA_KAL_NO_WAIT);
    if (RetVal != OK) {
        AmbaPrint_PrintUInt5("SvcRtspStrmResetFifo: send ENABLE message fail", \
                        0U, 0U, 0U, 0U, 0U);
    }
}

void SvcRtspStrmEnable(const AMBA_STREAM_s* Strm)
{
    UINT32 RetVal;
    SVC_STREAM_MSG_S Msg;

    Msg.Event = STREAMER_MSG_ENABLE;
    AmbaMisra_TypeCast(&Msg.Info, &Strm);

    RetVal = AmbaKAL_MsgQueueSend(&StrmMgr->MsgQueue, &Msg, AMBA_KAL_NO_WAIT);
    if (RetVal != OK) {
        AmbaPrint_PrintUInt5("SvcRtspStrmEnable: send ENABLE message fail", \
                        0U, 0U, 0U, 0U, 0U);
    }
}

void SvcRtspStrmDisable(const AMBA_STREAM_s* Strm)
{
    UINT32 RetVal;
    SVC_STREAM_MSG_S Msg;

    Msg.Event = STREAMER_MSG_DISABLE;
    AmbaMisra_TypeCast(&Msg.Info, &Strm);

    RetVal = AmbaKAL_MsgQueueSend(&StrmMgr->MsgQueue, &Msg, AMBA_KAL_NO_WAIT);
    if (RetVal != OK) {
        AmbaPrint_PrintUInt5("SvcRtspStrmDisable: send DISABLE message fail", \
                        0U, 0U, 0U, 0U, 0U);
    }
}

AMBA_STREAM_s* SvcRtspStrmCreate(void)
{
    AMBA_STREAM_s *Strm;

    Strm = StrmMgrGetSteamerSlot();
    if (Strm == NULL) {
        AmbaPrint_PrintUInt5("SvcRtspStrmCreate: no available Streamer instance", \
                        0U, 0U, 0U, 0U, 0U);
    }

    return Strm;
}

UINT32 SvcRtspStrmDelete(AMBA_STREAM_s* Strm)
{
    UINT32 RetVal = OK;

    if ((Strm == NULL) || (Strm->FifoHndlr == 0U)) {
        RetVal = ERR_NA;
    } else {
        if (AmbaKAL_MutexTake(&Strm->Mtx, AMBA_KAL_WAIT_FOREVER) != OK) {
            AmbaPrint_PrintUInt5("SvcRtspStrmDelete: Mutex acquire fail", \
                            0U, 0U, 0U, 0U, 0U);
            RetVal = ERR_NA;
        } else {
            AmbaPrint_PrintUInt5("[StreamerMgr] Delete Fifo 0x%x", \
                            Strm->FifoHndlr, 0U, 0U, 0U, 0U);
            Strm->FifoHndlr = 0;
            Strm->hCodec = 0;
            Strm->Codec = 0;
            (VOID)AmbaKAL_MutexGive(&Strm->Mtx);
        }
    }

    return RetVal;
}

UINT32 SvcRtspStrmReport(const AMBA_STREAM_s* Strm, UINT32 Event, void* Info)
{
    UINT32 RetVal = OK;

    AmbaMisra_TouchUnused(&Event);
    AmbaMisra_TouchUnused(Info);

    if ((Strm == NULL) || (Strm->FifoHndlr == 0U)) {
        RetVal = ERR_NA;
    }

    return RetVal;
}

UINT32 SvcRtspStrmChangeTickTimer(AMBA_STREAM_s* Strm, UINT32 PrevTick, UINT32 Tick)
{
    UINT32 TargetTick;
    UINT32 RetVal = OK;
    DOUBLE dTmp;
    UINT32 uTmp;

    if (Strm == NULL) {
        RetVal = ERR_NA;
    } else {
        if ((Strm->IsLive == 1U) || (PrevTick == Tick)) {
            RetVal = OK;
        } else {
            uTmp = (Tick*Strm->TickPerFrame);
            dTmp = (DOUBLE)uTmp;
            dTmp = dTmp / (DOUBLE)PrevTick;
            TargetTick = (UINT32)dTmp;

            (VOID)StrmMgrUnregisterTimer(Strm);
            Strm->TickPerFrame = TargetTick;

            uTmp = (Strm->TickPerFrame*1000U);
            dTmp = (DOUBLE)uTmp;
            dTmp = dTmp / (DOUBLE)Strm->TimeScale;
            Strm->TimerTick = (UINT32)dTmp;

            uTmp = (Strm->TickPerFrame*1000U);
            dTmp = (DOUBLE)uTmp;
            dTmp = dTmp / (DOUBLE)Strm->TimeScale;
            dTmp = dTmp - (DOUBLE)Strm->TimerTick;
            Strm->DeviationPerFrame = dTmp;

            {
                    char dst_str[128] = {'\0'};

                    if (AmbaWrap_memset(dst_str, 0, 128)!= 0U) { }
                    // (void)AmbaAdvSnPrint(dst_str, 128, "[StreamerMgr] Change time Tick from %u to %u. Deviation = %g", PrevTick, TargetTick, Strm->DeviationPerFrame);

                    {
                        char    *StrBuf = dst_str;
                        UINT32  BufSize = 128U;
                        UINT32  CurStrLen;

                        AmbaUtility_StringAppend(StrBuf, BufSize, "[StreamerMgr] Change time Tick from ");

                        CurStrLen = AmbaUtility_StringLength(StrBuf);
                        if (CurStrLen < BufSize) {
                            (void)AmbaUtility_UInt64ToStr(&(StrBuf[CurStrLen]), BufSize - CurStrLen, (UINT64)(PrevTick), 10U);
                        }
                        AmbaUtility_StringAppend(StrBuf, BufSize, " to ");

                        CurStrLen = AmbaUtility_StringLength(StrBuf);
                        if (CurStrLen < BufSize) {
                            (void)AmbaUtility_UInt64ToStr(&(StrBuf[CurStrLen]), BufSize - CurStrLen, (UINT64)(TargetTick), 10U);
                        }
                        AmbaUtility_StringAppend(StrBuf, BufSize, ". Deviation = ");

                        CurStrLen = AmbaUtility_StringLength(StrBuf);
                        if (CurStrLen < BufSize) {
                            (void)AmbaUtility_DoubleToStr(&(StrBuf[CurStrLen]), BufSize - CurStrLen, (DOUBLE)(Strm->DeviationPerFrame), 6U);
                        }
                        AmbaUtility_StringAppend(StrBuf, BufSize, "");
                    }

                    AmbaPrint_PrintStr5("%s, %s", __func__, dst_str, NULL, NULL, NULL);
            }
            (VOID)StrmMgrRegisterTimer(Strm);
        }
    }

    return RetVal;
}

AMBA_STREAM_s* SvcRtspStrmBind(SVC_RTSP_MEDIA_TRACK_CFG_s* Trk, AMBA_STREAM_s* Strm,
                                    STREAM_CB StreamFunc, void *Ctx)
{
    AMBA_STREAM_s *pStrm = NULL;

    if (((Trk == NULL) || (Strm == NULL)) || \
        ((StreamFunc == NULL) || (Ctx == NULL))) {
        AmbaPrint_PrintUInt5("SvcRtspStrmBind: Invalid Input", \
                        0U, 0U, 0U, 0U, 0U);
        pStrm = NULL;
    } else {
        if (AmbaKAL_MutexTake(&Strm->Mtx, AMBA_KAL_WAIT_FOREVER) != OK) {
            AmbaPrint_PrintUInt5("SvcRtspStrmBind: Mutex acquire fail", \
                            0U, 0U, 0U, 0U, 0U);
            pStrm = NULL;
        } else {
            Strm->StreamFunc = StreamFunc;
            Strm->StreamCtx = Ctx;
            Strm->Enable = 0U;
            Strm->IsLive = 1U;

            AmbaMisra_TypeCast(&Trk->hCodec, &Strm->hCodec);
            Trk->nTrackType = Strm->TrackType;
            Trk->nMediaId = Strm->MediaID;
            Trk->pBufferBase = Strm->pBufferBase;
            Trk->pBufferLimit = Strm->pBufferLimit;
            Trk->nTimeScale = Strm->TimeScale;
            Trk->nTimePerFrame = Strm->TickPerFrame;

            if (Trk->nTrackType == SVC_MEDIA_TRACK_TYPE_VIDEO) {
                Trk->Info.Video.nM = Strm->Param.Video.M;
                Trk->Info.Video.nN = Strm->Param.Video.N;
                Trk->Info.Video.nGOPSize= Strm->Param.Video.GOPSize;
                Trk->Info.Video.nIRCycle= Strm->Param.Video.IRCycle;
                Trk->Info.Video.nRecoveryFrameCnt= Strm->Param.Video.RecoveryFrameCnt;
            } else if (Trk->nTrackType == SVC_MEDIA_TRACK_TYPE_AUDIO) {
                Trk->Info.Audio.BitsPerSample = Strm->Param.Audio.BitsPerSample;
                Trk->Info.Audio.Default = Strm->Param.Audio.Default;
                Trk->Info.Audio.Channels = Strm->Param.Audio.Channels;
                Trk->Info.Audio.SampleRate = Strm->Param.Audio.SampleRate;
            } else {
                // Do nothing
            }

            (VOID)AmbaKAL_MutexGive(&Strm->Mtx);
            pStrm = Strm;
        }
    }

    return pStrm;
}

UINT32 SvcRtspStrmGetMediaID(const char* StreamName, SVC_STREAM_LIVE_MEDIA_s* MediaList)
{
    UINT32 i, j;
    UINT32 MediaCount = 0U;
    AMBA_STREAM_s* Strm;

    if ((StreamName == NULL) || (MediaList == NULL)) {
        AmbaPrint_PrintUInt5("SvcRtspStrmGetMediaID: Invalid Input", \
                        0U, 0U, 0U, 0U, 0U);
    } else {
        for (i = 0U; i < StrmMgr->MaxLiveGroup; i++) {
            if (AmbaKAL_MutexTake(&StrmMgr->LiveGroup[i].Mtx, AMBA_KAL_WAIT_FOREVER) != OK) {
                AmbaPrint_PrintUInt5("SvcRtspStrmGetMediaID: Mutex acquire fail", \
                                0U, 0U, 0U, 0U, 0U);
                continue;
            }

            if (AmbaUtility_StringCompare(StrmMgr->LiveGroup[i].Name, StreamName, AmbaUtility_StringLength(StreamName)) == 0) {
                MediaCount = 0U;
                for (j = 0U; j < StrmMgr->MaxStreamerPerLiveGroup; j++) {
                    Strm = StrmMgr->LiveGroup[i].Stms[j];
                    if (Strm == NULL){
                        continue;
                    }

                    if (AmbaKAL_MutexTake(&Strm->Mtx, AMBA_KAL_WAIT_FOREVER) != OK) {
                        AmbaPrint_PrintUInt5("SvcRtspStrmGetMediaID: Mutex acquire fail", \
                                        0U, 0U, 0U, 0U, 0U);
                        continue;
                    }

                    MediaList[MediaCount].MediaId= Strm->MediaID;
                    MediaList[MediaCount].Stm = Strm;
                    MediaCount++;

                    (VOID)AmbaKAL_MutexGive(&Strm->Mtx);
                }

                (VOID)AmbaKAL_MutexGive(&StrmMgr->LiveGroup[i].Mtx);
                break;
            }
            (VOID)AmbaKAL_MutexGive(&StrmMgr->LiveGroup[i].Mtx);
        }
    }

    return MediaCount;
}

UINT32 SvcStrmMgrNotify(UINT32 Notify)
{
    UINT32 RetVal;
    SVC_STREAM_MSG_S Msg;

    switch(Notify) {
        case SVC_RTSP_NOTIFY_STARTENC:
            Msg.Event = STREAMER_MSG_LIVE_STARTENC;
            break;
        case SVC_RTSP_NOTIFY_STOPENC:
            Msg.Event = STREAMER_MSG_LIVE_STOPENC;
            break;
        case SVC_RTSP_NOTIFY_SWITCHENCSESSION:
            Msg.Event = STREAMER_MSG_LIVE_SWITCHSESSION;
            break;
        default:
            // Do nothing
            break;

    }

    RetVal = AmbaKAL_MsgQueueSend(&StrmMgr->MsgQueue, &Msg, AMBA_KAL_NO_WAIT);
    if (RetVal != OK) {
        AmbaPrint_PrintUInt5("SvcStrmMgrNotify: send message fail", \
                        0U, 0U, 0U, 0U, 0U);
    }
    return RetVal;
}

UINT32 SvcRtspStrmMgrGetMediaInfo(void* Hdlr, UINT32 Event , const void* Info)
{
    return StrmMgr->cbMediaInfo(Hdlr, Event, Info);
}

UINT32 SvcRtspStrmMgrGetStreamList(void* Hdlr, UINT32 Event, const void* Info)
{
    return StrmMgr->cbStreamList(Hdlr, Event, Info);
}

UINT32 SvcRtspStrmMgrGetRequiredMemorySize(const SVC_STREAM_MGR_INIT_CFG_s *Cfg, UINT32* MemSize)
{
    UINT32 RetVal = OK;

    if ((Cfg == NULL) || (MemSize == NULL)) {
        AmbaPrint_PrintUInt5("SvcRtspStrmMgrGetRequiredMemorySize: Invalid Input", \
                        0U, 0U, 0U, 0U, 0U);
        RetVal = ERR_NA;
    } else {
        *MemSize = (ALIGN_32(Cfg->TaskInfo.StackSize) + ALIGN_32(Cfg->GuardSize)) + \
                (ALIGN_32(sizeof(SVC_STREAM_MGR_s)) + ALIGN_32(Cfg->MsgQueueNumber * sizeof(SVC_STREAM_MSG_S))) + \
                (ALIGN_32(Cfg->NumMaxStreamer * sizeof(AMBA_STREAM_s)) + ALIGN_32(Cfg->NumMaxActiveLiveGroup * sizeof(SVC_STREAM_GROUP_s)))
                 + 1024U;
    }

    return RetVal;
}

UINT32 SvcRtspStrmMgrGetInitDefaultCfg(SVC_STREAM_MGR_INIT_CFG_s *DefaultCfg)
{
    UINT32 RetVal = OK;

    if (DefaultCfg == NULL) {
        AmbaPrint_PrintUInt5("SvcRtspStrmMgrGetInitDefaultCfg: Invalid Input", \
                        0U, 0U, 0U, 0U, 0U);
        RetVal = ERR_NA;
    } else {
        if (AmbaWrap_memset(DefaultCfg, 0, sizeof(SVC_STREAM_MGR_INIT_CFG_s))!= 0U) { }

        DefaultCfg->TaskInfo.Priority = 63;
        DefaultCfg->TaskInfo.StackSize = STREAM_MGR_STACK_SIZE;
        DefaultCfg->TaskInfo.CoreSelection = STREAM_MGR_DEFAULT_CORE;
        DefaultCfg->GuardSize = STREAM_MGR_GUARD_SIZE;
        DefaultCfg->MsgQueueNumber = STREAM_MGR_QUEUE_NUMBER;
        DefaultCfg->NumMaxStreamer = STREAM_MGR_MAX_STREAMERS;
        DefaultCfg->NumMaxActiveLiveGroup = STREAM_MGR_MAX_ACTIVE_LIVE_STREAM_GROUP;
        DefaultCfg->MemoryPoolAddr = NULL;
        DefaultCfg->MemoryPoolSize = (ALIGN_32(STREAM_MGR_STACK_SIZE) + ALIGN_32(DefaultCfg->GuardSize)) + \
        (ALIGN_32(sizeof(SVC_STREAM_MGR_s)) + ALIGN_32(DefaultCfg->MsgQueueNumber * sizeof(SVC_STREAM_MSG_S))) + \
        (ALIGN_32(DefaultCfg->NumMaxStreamer * sizeof(AMBA_STREAM_s)) + ALIGN_32(DefaultCfg->NumMaxActiveLiveGroup * sizeof(SVC_STREAM_GROUP_s))) + \
        1024U;
    }

    return RetVal;
}

UINT32 SvcRtspStrmMgrInit(const SVC_STREAM_MGR_INIT_CFG_s *Cfg)
{
    UINT8 *Addr;
    const UINT8 *PoolEnd;
    UINT32 Er, i;
    static char MsgQueName[] = "StreamMgr_MsgQue";
    UINT32 RetVal = OK;
    ULONG SrcUL, SrcULnd;
    char TskName[] = "StreamerMgrTask";

    if (((Cfg == NULL) || (Cfg->MemoryPoolAddr == NULL)) || (Cfg->MemoryPoolSize == 0U)) {
        AmbaPrint_PrintUInt5("SvcRtspStrmMgrInit: Invalid Input", \
                        0U, 0U, 0U, 0U, 0U);
        RetVal = ERR_NA;
    } else {
        Addr = Cfg->MemoryPoolAddr;
        PoolEnd = &(Cfg->MemoryPoolAddr[Cfg->MemoryPoolSize]);
        if (AmbaWrap_memset(Addr, 0, Cfg->MemoryPoolSize)!= 0U) { }

        AmbaMisra_TypeCast(&SrcUL, &Addr);
        SrcUL = ALIGN_32(SrcUL);
        AmbaMisra_TypeCast(&StrmMgr, &SrcUL);

        Addr = &(Addr[ALIGN_32(sizeof(SVC_STREAM_MGR_s))]);
        AmbaMisra_TypeCast(&SrcUL, &Addr);
        AmbaMisra_TypeCast(&SrcULnd, &PoolEnd);
        if (SrcUL > SrcULnd) {
            AmbaPrint_PrintUInt5("SvcRtspStrmMgrInit: no enough memory space for stream manager data structure.", \
                            0U, 0U, 0U, 0U, 0U);
            RetVal = ERR_NA;
        } else {
            if ((Cfg->cbMediaInfo == NULL) || \
                (Cfg->cbStreamList == NULL)) {
                AmbaPrint_PrintUInt5("SvcRtspStrmMgrInit: no callback assigned, abort", \
                                0U, 0U, 0U, 0U, 0U);
                RetVal = ERR_NA;
            } else {
                StrmMgr->cbMediaInfo = Cfg->cbMediaInfo;
                StrmMgr->cbStreamList = Cfg->cbStreamList;
                StrmMgr->MaxLiveGroup = Cfg->NumMaxActiveLiveGroup;
                StrmMgr->MaxStreamerPerLiveGroup = STREAM_MGR_MAX_STREAMER_PER_GROUP;
                StrmMgr->MaxStreamer = Cfg->NumMaxStreamer;

                AmbaMisra_TypeCast(&StrmMgr->StreamList, &Addr);
                Addr = &(Addr[StrmMgr->MaxStreamer * sizeof(AMBA_STREAM_s)]);

                AmbaMisra_TypeCast(&SrcUL, &Addr);
                AmbaMisra_TypeCast(&SrcULnd, &PoolEnd);
                if (SrcUL > SrcULnd) {
                    AmbaPrint_PrintUInt5("SvcRtspStrmMgrInit: no enough memory space for Streamer pool", \
                                    0U, 0U, 0U, 0U, 0U);
                    RetVal = ERR_NA;
                } else {
                    AmbaMisra_TypeCast(&StrmMgr->LiveGroup, &Addr);

                    Addr = &(Addr[(StrmMgr->MaxLiveGroup * sizeof(SVC_STREAM_GROUP_s))]);
                    AmbaMisra_TypeCast(&SrcUL, &Addr);
                    AmbaMisra_TypeCast(&SrcULnd, &PoolEnd);
                    if (SrcUL > SrcULnd) {
                        AmbaPrint_PrintUInt5("SvcRtspStrmMgrInit: no enough memory space for Streamer group pool", \
                                        0U, 0U, 0U, 0U, 0U);
                        RetVal = ERR_NA;
                    } else {
                        StrmMgr->GuardSize = Cfg->GuardSize;
                        StrmMgr->GuardAddr = Addr;
                        if(StrmMgr->GuardSize > 0U){
                            for (i=0U; i<StrmMgr->GuardSize; i++){
                                Addr[i] = (UINT8)0xc4;
                            }

                            Addr = &(Addr[StrmMgr->GuardSize]);
                        }

                        AmbaMisra_TypeCast(&SrcUL, &Addr);
                        AmbaMisra_TypeCast(&SrcULnd, &PoolEnd);
                        if (SrcUL > SrcULnd) {
                            AmbaPrint_PrintUInt5("SvcRtspStrmMgrInit: no enough memory space for stream manager guard area", \
                                            0U, 0U, 0U, 0U, 0U);
                            RetVal = ERR_NA;
                        } else {
                            AmbaMisra_TypeCast(&SrcUL, &Addr);
                            SrcUL = ALIGN_32(SrcUL);
                            AmbaMisra_TypeCast(&StrmMgr->Stack, &SrcUL);

                            Addr = &(Addr[Cfg->TaskInfo.StackSize]);
                            AmbaMisra_TypeCast(&SrcUL, &Addr);
                            AmbaMisra_TypeCast(&SrcULnd, &PoolEnd);
                            if (SrcUL > SrcULnd) {
                                AmbaPrint_PrintUInt5("SvcRtspStrmMgrInit: no enough memory space for stream manager Stack", \
                                                0U, 0U, 0U, 0U, 0U);
                                RetVal = ERR_NA;
                            } else {
                                AmbaMisra_TypeCast(&SrcUL, &Addr);
                                SrcUL = ALIGN_32(SrcUL);
                                AmbaMisra_TypeCast(&StrmMgr->MsgPool, &SrcUL);

                                Addr = &(Addr[Cfg->MsgQueueNumber * sizeof(SVC_STREAM_MSG_S)]);
                                AmbaMisra_TypeCast(&SrcUL, &Addr);
                                AmbaMisra_TypeCast(&SrcULnd, &PoolEnd);
                                if (SrcUL > SrcULnd) {
                                    AmbaPrint_PrintUInt5("SvcRtspStrmMgrInit: no enough memory space for stream manager message queue", \
                                                    0U, 0U, 0U, 0U, 0U);
                                    RetVal = ERR_NA;
                                } else {
                                    Er = AmbaKAL_MsgQueueCreate(&StrmMgr->MsgQueue, MsgQueName, sizeof(SVC_STREAM_MSG_S), StrmMgr->MsgPool, sizeof(SVC_STREAM_MSG_S)*Cfg->MsgQueueNumber);
                                    if (Er != OK) {
                                        AmbaPrint_PrintUInt5("SvcRtspStrmMgrInit: StreamerMgr Message Queue Create Fail", \
                                                        0U, 0U, 0U, 0U, 0U);
                                        RetVal = ERR_NA;
                                    } else {
                                        for (i=0U; i < StrmMgr->MaxStreamer; i++) {
                                            if (AmbaKAL_MutexCreate(&StrmMgr->StreamList[i].Mtx, NULL) != OK) {
                                                AmbaPrint_PrintUInt5("SvcRtspStrmMgrInit: Streamers Mutex Create Fail!", \
                                                                0U, 0U, 0U, 0U, 0U);
                                               RetVal = ERR_NA;
                                               break;
                                            }
                                        }

                                        if (ERR_NA != RetVal) {
                                            for (i=0U; i < StrmMgr->MaxLiveGroup; i++) {
                                                if (AmbaKAL_MutexCreate(&StrmMgr->LiveGroup[i].Mtx, NULL) != OK) {
                                                    AmbaPrint_PrintUInt5("SvcRtspStrmMgrInit: Streamers Mutex Create Fail!", \
                                                                    0U, 0U, 0U, 0U, 0U);
                                                    RetVal = ERR_NA;
                                                    break;
                                                }
                                            }

                                            if (ERR_NA != RetVal) {
                                                Er = AmbaKAL_TaskCreate(&StrmMgr->Task, TskName, Cfg->TaskInfo.Priority, \
                                                     StrmMgrTaskEntry, NULL, StrmMgr->Stack, Cfg->TaskInfo.StackSize, AMBA_KAL_AUTO_START);
                                                if (Er != OK) {
                                                    AmbaPrint_PrintUInt5("SvcRtspStrmMgrInit: StreamerMgr Task Create Fail!", \
                                                                    0U, 0U, 0U, 0U, 0U);
                                                    RetVal = ERR_NA;
                                                }
                                            }
                                        }

                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    return RetVal;
}

UINT32 SvcStrmMgrRelease(void)
{
    UINT32 i;
    UINT32 RetVal = OK;

    (VOID)StrmMgrStopEncode();

    for (i=0U; i < StrmMgr->MaxStreamer ; i++) {
        if (AmbaKAL_MutexTake(&StrmMgr->StreamList[i].Mtx, AMBA_KAL_WAIT_FOREVER) != OK) {
            AmbaPrint_PrintUInt5("SvcStrmMgrRelease: Mutex acquire fail", \
                            0U, 0U, 0U, 0U, 0U);
            continue;
        }

        if ((StrmMgr->StreamList[i].FifoHndlr != 0U) && (StrmMgr->StreamList[i].Codec != 0U) && \
            (StrmMgr->StreamList[i].IsLive == 0U)) {
            if (StrmMgr->StreamList[i].TimerReg == 1U) {
                (VOID)AmbaKAL_TimerDelete(&StrmMgr->StreamList[i].Timer);
            }
        }

        (VOID)AmbaKAL_MutexGive(&StrmMgr->StreamList[i].Mtx);
    }

    RetVal = AmbaKAL_TaskTerminate(&StrmMgr->Task);
    if ((StrmMgr == NULL) || (RetVal != OK)) {
        RetVal = ERR_NA;
    } else {
        if (AmbaKAL_TaskDelete(&StrmMgr->Task) != OK) {
            RetVal = ERR_NA;
        } else {
            for (i=0U; i < StrmMgr->MaxLiveGroup; i++) {
                if (AmbaKAL_MutexDelete(&StrmMgr->LiveGroup[i].Mtx) != OK) {
                    AmbaPrint_PrintUInt5("SvcStrmMgrRelease: Live group Delete Fail!", \
                                    0U, 0U, 0U, 0U, 0U);
                    RetVal = ERR_NA;
                    break;
                }
            }

            if (ERR_NA != RetVal) {
                for (i=0U; i < StrmMgr->MaxStreamer; i++) {
                    if (AmbaKAL_MutexDelete(&StrmMgr->StreamList[i].Mtx) != OK) {
                        AmbaPrint_PrintUInt5("SvcStrmMgrRelease: Streamers Mutex Delete Fail!", \
                                        0U, 0U, 0U, 0U, 0U);
                        RetVal = ERR_NA;
                        break;
                    }
                }

                if (ERR_NA != RetVal) {
                    if (AmbaKAL_MsgQueueDelete(&StrmMgr->MsgQueue) != OK) {
                        RetVal = ERR_NA;
                    }
                }
            }
        }
    }

    return RetVal;
}

