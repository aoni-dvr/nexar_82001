/**
 * @file WrpFormatLib.c
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
#include "WrpFormatLib.h"

static inline void WrpFormatLib_Perror(const char *Func, UINT32 Line, const char *Message)
{
    char LineStr[8];
    (void)AmbaUtility_UInt32ToStr(LineStr, (UINT32)sizeof(LineStr), Line, 10);
    AmbaPrint_PrintStr5("[Error]%s:%s %s", Func, LineStr, Message, NULL, NULL);
}

static void WrpFormatLib_ResetMuxTrack(SVC_MEDIA_TRACK_GENERAL_INFO_s *Track)
{
    Track->FrameCount = 0;
    Track->FrameNo = 0;
}

/**
 * Reset muxer movie information
 *
 * @param [out] Movie Movie information
 */
void WrpFormatLib_ResetMuxMovieInfo(SVC_MOVIE_INFO_s *Movie)
{
    UINT32 i;
    for (i = 0; i < Movie->VideoTrackCount; i++) {
        WrpFormatLib_ResetMuxTrack(&Movie->VideoTrack[i].Info);
    }
    for (i = 0; i < Movie->AudioTrackCount; i++) {
        WrpFormatLib_ResetMuxTrack(&Movie->AudioTrack[i].Info);
    }
    for (i = 0; i < Movie->TextTrackCount; i++) {
        WrpFormatLib_ResetMuxTrack(&Movie->TextTrack[i].Info);
    }
}

static UINT8 WrpFormatLib_GetShortestVideoTrack(const SVC_VIDEO_TRACK_INFO_s *Tracks, UINT8 TrackCount)
{
    UINT64 DTS = SVC_FORMAT_MAX_TIMESTAMP;
    UINT8 i, Idx = TrackCount;
    // return the track with the min DTS
    for (i = 0U; i < TrackCount; i++) {
        const SVC_VIDEO_TRACK_INFO_s *p = &Tracks[i];
        UINT64 TmpDTS = p->Info.DTS;
        if (TmpDTS < DTS) {
            DTS = TmpDTS;
            Idx = i;
        } else {
            if ((TmpDTS == DTS) && (DTS != SVC_FORMAT_MAX_TIMESTAMP)) {
                // default track has the highest priority
                if (p->IsDefault == 1U) {
                    Idx = i;
                }
            }
        }
    }
    return Idx;
}

static UINT8 WrpFormatLib_GetShortestAudioTrack(const SVC_AUDIO_TRACK_INFO_s *Tracks, UINT8 TrackCount)
{
    UINT64 DTS = SVC_FORMAT_MAX_TIMESTAMP;
    UINT8 i, Idx = TrackCount;
    // return the track with the min DTS
    for (i = 0U; i < TrackCount; i++) {
        const SVC_AUDIO_TRACK_INFO_s *p = &Tracks[i];
        UINT64 TmpDTS = p->Info.DTS;
        if (TmpDTS < DTS) {
            DTS = TmpDTS;
            Idx = i;
        } else {
            if ((TmpDTS == DTS) && (DTS != SVC_FORMAT_MAX_TIMESTAMP)) {
                // default track has the highest priority
                if (p->IsDefault == 1U) {
                    Idx = i;
                }
            }
        }
    }
    return Idx;
}

static UINT8 WrpFormatLib_GetShortestTextTrack(const SVC_TEXT_TRACK_INFO_s *Tracks, UINT8 TrackCount)
{
    UINT64 DTS = SVC_FORMAT_MAX_TIMESTAMP;
    UINT8 i, Idx = TrackCount;
    // return the track with the min DTS
    for (i = 0U; i < TrackCount; i++) {
        const SVC_TEXT_TRACK_INFO_s *p = &Tracks[i];
        UINT64 TmpDTS = p->Info.DTS;
        if (TmpDTS < DTS) {
            DTS = TmpDTS;
            Idx = i;
        } else {
            if ((TmpDTS == DTS) && (DTS != SVC_FORMAT_MAX_TIMESTAMP)) {
                // default track has the highest priority
                if (p->IsDefault == 1U) {
                    Idx = i;
                }
            }
        }
    }
    return Idx;
}

/**
 * Get shortest track
 *
 * @param [in] VideoTracks Video track information
 * @param [in] VideoTrackCount Video track counter
 * @param [in] AudioTracks Audio track information
 * @param [in] AudioTrackCount Audio track counter
 * @param [in] TextTracks Text track information
 * @param [in] TextTrackCount Text track counter
 * @param [out] TrackIdx The returned track index
 * @return The type of the shortest track
 */
UINT8 WrpFormatLib_GetShortestTrack(const SVC_VIDEO_TRACK_INFO_s *VideoTracks, UINT8 VideoTrackCount, const SVC_AUDIO_TRACK_INFO_s *AudioTracks, UINT8 AudioTrackCount, const SVC_TEXT_TRACK_INFO_s *TextTracks, UINT8 TextTrackCount, UINT8 *TrackIdx)
{
    const SVC_MEDIA_TRACK_GENERAL_INFO_s *Track = NULL;
    UINT8 TrackType = 0U;

    if (VideoTracks != NULL) {
        UINT8 Idx = WrpFormatLib_GetShortestVideoTrack(VideoTracks, VideoTrackCount);
        if (Idx != VideoTrackCount) {
            Track = &VideoTracks[Idx].Info;
            TrackType = WRP_MEDIA_TRACK_TYPE_VIDEO;
            *TrackIdx = Idx;
        }
    }
    if (AudioTracks != NULL) {
        UINT8 Idx = WrpFormatLib_GetShortestAudioTrack(AudioTracks, AudioTrackCount);
        if (Idx != AudioTrackCount) {
            const SVC_MEDIA_TRACK_GENERAL_INFO_s *AudioTrack = &AudioTracks[Idx].Info;
            if ((Track == NULL) || (AudioTrack->DTS < Track->DTS)) {
                Track = AudioTrack;
                TrackType = WRP_MEDIA_TRACK_TYPE_AUDIO;
                *TrackIdx = Idx;
            }
        }
    }
    if (TextTracks != NULL) {
        UINT8 Idx = WrpFormatLib_GetShortestTextTrack(TextTracks, TextTrackCount);
        if (Idx != TextTrackCount) {
            const SVC_MEDIA_TRACK_GENERAL_INFO_s *TextTrack = &TextTracks[Idx].Info;
            if ((Track == NULL) || (TextTrack->DTS < Track->DTS)) {
                TrackType = WRP_MEDIA_TRACK_TYPE_TEXT;
                *TrackIdx = Idx;
            }
        }
    }
    return TrackType;
}

static UINT64 WrpFormatLib_GetGCD(UINT64 U, UINT64 V)
{
    UINT64 Rval = 0U;
    /* GCD(0,x) := x */
    if ((U == 0U) || (V == 0U)) {
        Rval = U | V;
    } else {
        UINT64 TempU = U, TempV = V;
        UINT32 Shift = 0U;
        /* Let Shift := lg K, where K is the greatest power of 2 dividing both U and V. */
        while (((TempU | TempV) & 1ULL) == 0ULL) {
            TempU >>= 1;
            TempV >>= 1;
            ++Shift;
        }
        while ((TempU & 1U) == 0U) {
            TempU >>= 1;
        }
        /* From here on, U is always odd. */
        do {
            while ((TempV & 1U) == 0U) {  /* Loop X */
                TempV >>= 1;
            }
            /* Now U and V are both odd, so diff(U, V) is even. Let U = min(U, V), V = diff(U, V)/2. */
            if (TempU < TempV) {
                TempV -= TempU;
            } else {
                UINT64 diff = TempU - TempV;
                TempU = TempV;
                TempV = diff;
            }
            TempV >>= 1;
        } while (TempV != 0U);
        Rval = TempU << Shift;
    }
    return Rval;
}

/**
 * a helper funciton to compute the LCM
 * @param [in] V the array of numbers
 * @param [in] Count the size of the array
 * @return the LCM of the numbers in v
 */
static UINT64 WrpFormatLib_GetLCM(const UINT64 *V, UINT32 Count)
{
    UINT32 i;
    UINT64 Rval = V[0];
    for (i = 1U; i < Count; i++) {
        UINT64 Ret = WrpFormatLib_GetGCD(Rval, V[i]);
        if (Ret != 0U) {
            Rval = (Rval * V[i]) / Ret;
        }
    }
    return Rval;
}

static UINT32 WrpFormatLib_GetTimeScaleLCM(const SVC_MEDIA_INFO_s *Media, UINT32 *TimeScaleLCM)
{
    UINT32 Rval = OK;
    UINT8 TrackCount = 0U;
    UINT64 TimeScale[SVC_FORMAT_MAX_VIDEO_TRACK_PER_MEDIA + SVC_FORMAT_MAX_AUDIO_TRACK_PER_MEDIA + SVC_FORMAT_MAX_TEXT_TRACK_PER_MEDIA + 1U] = {0};
    TimeScale[0] = *TimeScaleLCM;
    if (Media->MediaType == SVC_MEDIA_INFO_MOVIE) {
        UINT8 i;
        const SVC_MOVIE_INFO_s * Movie;
        AmbaMisra_TypeCast(&Movie, &Media);
        for (i = 0U; i < Movie->VideoTrackCount; i++) {
            TimeScale[TrackCount + i + 1U] = (UINT64)Movie->VideoTrack[i].Info.TimeScale;
        }
        TrackCount = Movie->VideoTrackCount;
        for (i = 0U; i < Movie->AudioTrackCount; i++) {
            TimeScale[TrackCount + i + 1U] = (UINT64)Movie->AudioTrack[i].Info.TimeScale;
        }
        TrackCount += Movie->AudioTrackCount;
        for (i = 0U; i < Movie->TextTrackCount; i++) {
            TimeScale[TrackCount + i + 1U] = (UINT64)Movie->TextTrack[i].Info.TimeScale;
        }
        TrackCount += Movie->TextTrackCount;
    } else {
        WrpFormatLib_Perror(__func__, __LINE__, "Wrong media type!");
        Rval = AMBA_MP4FMT_ERR_FATAL;
    }
    if (Rval == OK) {
        // compute the common Rate
        UINT64 LCM = WrpFormatLib_GetLCM(TimeScale, (UINT32)TrackCount + 1U);
        if (LCM > 0xFFFFFFFFU) {
            WrpFormatLib_Perror(__func__, __LINE__, "Wrong value of LCM!");
            Rval = AMBA_MP4FMT_ERR_FATAL;
        }
        *TimeScaleLCM = (UINT32)LCM;
    }
    return Rval;
}


static inline UINT64 FORMAT_DTS_NORMALIZE(UINT64 DTS, UINT32 TrackGcd, UINT32 Ratio)
{
    return (((DTS / (UINT64)TrackGcd) * Ratio) + (((DTS % (UINT64)TrackGcd) * Ratio) / TrackGcd));
}

static UINT32 WrpFormatLib_NormalizeMovie(SVC_MOVIE_INFO_s *Movie)
{
    UINT32 Rval = OK;
    UINT32 i;
    UINT32 GCD_V[SVC_FORMAT_MAX_VIDEO_TRACK_PER_MEDIA] = {0U};
    UINT32 GCD_A[SVC_FORMAT_MAX_AUDIO_TRACK_PER_MEDIA] = {0U};
    UINT32 GCD_T[SVC_FORMAT_MAX_TEXT_TRACK_PER_MEDIA] = {0U};
    // keep OrigTimeScale, reduce TimeScale and TimePerFrame
    for (i = 0U; i < Movie->VideoTrackCount; i++) {
        SVC_MEDIA_TRACK_GENERAL_INFO_s * const Track = &Movie->VideoTrack[i].Info;
        Track->OrigTimeScale = Track->TimeScale;
        /* Do not apply fraction reduction in CaptureTimeMode */
        /* Ensure that the normalized TimeScale is large enough and is a multiple of the original TimeScale. */
        /* E.g. Track1's GopSize = 11, frame rate = 1/11 --- reduction gcd --> 1/11
                Track2's GopSize = 29, frame rate = 700/2900 --- reduction gcd --> 7/29
                --> LCM: 77/319 --> since 77 is too small, make it greater than 1000.
                --> (77*13)/(319*13) = 1001/4147
                --> 4147 is not a multiple of 2900
                Delta_TPF = Delta_Total * (OriginalTimeScale / NormalizedTimeScale) / GopSize
                E.g. Delta_TPF_Ori = 208 * 2900/4147  / 29 = 5
                     Delta_TPF_Nor = 5 * 4147/2900 -> information lost
                     -> The normalized DTS and original DTS can't be matched.
                Issue might occurs. e.g. Trim on TPF. */
        if (Movie->VideoTrack[i].CaptureTimeMode == 0U) {
            GCD_V[i] = (UINT32)WrpFormatLib_GetGCD(Track->TimeScale, Track->TimePerFrame);
            if (GCD_V[i] == 0U) {
                WrpFormatLib_Perror(__func__, __LINE__, "Wrong value of GCD!");
                Rval = AMBA_MP4FMT_ERR_FATAL;
                break;
            }
            Track->TimeScale /= GCD_V[i];
            Track->TimePerFrame /= GCD_V[i];
        } else {
            // not to reduce because a small TimeScale could make frame rate hard to be adjusted
            GCD_V[i] = 1U;
        }
    }
    if (Rval == OK) {
        for (i = 0U; i < Movie->AudioTrackCount; i++) {
            SVC_MEDIA_TRACK_GENERAL_INFO_s * const Track = &Movie->AudioTrack[i].Info;
            Track->OrigTimeScale = Track->TimeScale;
            GCD_A[i] = (UINT32)WrpFormatLib_GetGCD(Track->TimeScale, Track->TimePerFrame);
            if (GCD_A[i] == 0U) {
                WrpFormatLib_Perror(__func__, __LINE__, "Wrong value of GCD!");
                Rval = AMBA_MP4FMT_ERR_FATAL;
                break;
            }
            Track->TimeScale /= GCD_A[i];
            Track->TimePerFrame /= GCD_A[i];
        }
    }
    if (Rval == OK) {
        for (i = 0U; i < Movie->TextTrackCount; i++) {
            SVC_MEDIA_TRACK_GENERAL_INFO_s * const Track = &Movie->TextTrack[i].Info;
            Track->OrigTimeScale = Track->TimeScale;
            GCD_T[i] = (UINT32)WrpFormatLib_GetGCD(Track->TimeScale, Track->TimePerFrame);
            if (GCD_T[i] == 0U) {
                WrpFormatLib_Perror(__func__, __LINE__, "Wrong value of GCD!");
                Rval = AMBA_MP4FMT_ERR_FATAL;
                break;
            }
            Track->TimeScale /= GCD_T[i];
            Track->TimePerFrame /= GCD_T[i];
        }
    }
    if (Rval == OK) {
        UINT32 TimeScaleLCM = 1000U;
        Rval = WrpFormatLib_GetTimeScaleLCM(&Movie->MediaInfo, &TimeScaleLCM);
        if (Rval == OK) {
            for (i = 0U; i < Movie->VideoTrackCount; i++) {
                SVC_MEDIA_TRACK_GENERAL_INFO_s * const Track = &Movie->VideoTrack[i].Info;
                UINT32 Ratio = TimeScaleLCM / Track->TimeScale;
                Track->FrameNo = 0U;
                Track->TimePerFrame *= Ratio;
                Track->TimeScale = TimeScaleLCM;
                Track->InitDTS = FORMAT_DTS_NORMALIZE(Track->InitDTS, GCD_V[i], Ratio);
                Track->DTS = FORMAT_DTS_NORMALIZE(Track->DTS, GCD_V[i], Ratio);
                Track->NextDTS = FORMAT_DTS_NORMALIZE(Track->NextDTS, GCD_V[i], Ratio);
            }
            for (i = 0U; i < Movie->AudioTrackCount; i++) {
                SVC_MEDIA_TRACK_GENERAL_INFO_s * const Track = &Movie->AudioTrack[i].Info;
                UINT32 Ratio = TimeScaleLCM / Track->TimeScale;
                Track->FrameNo = 0U;
                Track->TimePerFrame *= Ratio;
                Track->TimeScale = TimeScaleLCM;
                Track->InitDTS = FORMAT_DTS_NORMALIZE(Track->InitDTS, GCD_A[i], Ratio);
                Track->DTS = FORMAT_DTS_NORMALIZE(Track->DTS, GCD_A[i], Ratio);
                Track->NextDTS = FORMAT_DTS_NORMALIZE(Track->NextDTS, GCD_A[i], Ratio);
            }
            for (i = 0U; i < Movie->TextTrackCount; i++) {
                SVC_MEDIA_TRACK_GENERAL_INFO_s * const Track = &Movie->TextTrack[i].Info;
                UINT32 Ratio = TimeScaleLCM / Track->TimeScale;
                Track->FrameNo = 0U;
                Track->TimePerFrame *= Ratio;
                Track->TimeScale = TimeScaleLCM;
                Track->InitDTS = FORMAT_DTS_NORMALIZE(Track->InitDTS, GCD_T[i], Ratio);
                Track->DTS = FORMAT_DTS_NORMALIZE(Track->DTS, GCD_T[i], Ratio);
                Track->NextDTS = FORMAT_DTS_NORMALIZE(Track->NextDTS, GCD_T[i], Ratio);
            }
        }
    }
    return Rval;
}

/**
 * Normalize media's timescale
 *
 * @param [in,out] Media Media information
 * @return 0 - OK, others - AMBA_MP4FMT_ERR_XXXX
 */
UINT32 WrpFormatLib_NormalizeTimeScale(const SVC_MEDIA_INFO_s *Media)
{
    UINT32 Rval;
    if (Media->MediaType == SVC_MEDIA_INFO_MOVIE) {
        SVC_MOVIE_INFO_s *Movie;
        AmbaMisra_TypeCast(&Movie, &Media);
        Rval = WrpFormatLib_NormalizeMovie(Movie);
    } else {
        WrpFormatLib_Perror(__func__, __LINE__, "Invalid arguement!");
        Rval = AMBA_MP4FMT_ERR_INVALID_ARG;
    }
    return Rval;
}

