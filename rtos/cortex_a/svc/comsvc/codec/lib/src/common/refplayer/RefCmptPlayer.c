/**
 *  @file RefCmptPlayer.c
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
 *  @details Reference compatible player
 *
 */

#include <AmbaShell.h>

#include <AmbaKAL.h>
#include <AmbaDSP.h>
#include <AmbaDSP_VideoDec.h>
#include <AmbaDSP_Event.h>
#include "SvcFile.h"

#include "AmbaSvcWrap.h"
#include "RefCmptDemuxerImpl.h"
#include "RefCmptPlayerImpl.h"
#include "AmbaCodecCom.h"
// #include "RefMemPool.h"

#define REF_CMPT_PLAYER_STATE_READY  (0U)
#define REF_CMPT_PLAYER_STATE_OPENED (1U) ///< Already played and stop with last frame on screen
#define REF_CMPT_PLAYER_STATE_RUN    (2U)
#define REF_CMPT_PLAYER_STATE_PAUSE  (3U)

#define REF_CMPT_PLAYER_MAX_PIPE                     (1U)                    /**< Max number of pipe */
#define REF_CMPT_PLAYER_MAX_VIDEO_WIDTH              (3840U)
#define REF_CMPT_PLAYER_MAX_VIDEO_HEIGHT             (2160U)

#define REF_CMPT_FLAG_FRAME_DISPLAY                  (0x00000001U)           /**< Event flag. A frame is displayed. */
#define REF_CMPT_FLAG_DECODE_IDLE                    (0x00000002U)           /**< Event flag. DSP decoder is in idle state. */
#define REF_CMPT_FLAG_PLAYBACK_EOS                   (0x00000004U)           /**< Event flag. Decoder has played EOS. */

#define REF_CMPT_DEBUG_FLAG_STATUS_UPDATE            (0x00000001U)           /**< Debug flag. Print AMBA_DSP_EVENT_VIDEO_DEC_STATUS_UPDATE_s. */
#define REF_CMPT_DEBUG_FLAG_PIC_INFO                 (0x00000002U)           /**< Debug flag. Print AMBA_DSP_EVENT_VIDEO_DEC_PIC_INFO_s. */

#define REF_CMPT_PLAYER_MUTEX_TIMEOUT                (10000U)                /**< Timeout of waiting mutex */
#define REF_CMPT_PLAYER_FLAG_TIMEOUT                 (2000U)                 /**< Timeout of waiting event flag */
#define REF_CMPT_PLAYER_WAIT_FRAME_TIMEOUT           (2000U)                 /**< Timeout of waiting displayed video frame */

/**
 *  Demuxer format info
 */
typedef struct {
    REF_PLAYER_DEMUXER_FILE_INFO_s FileInfo;                                        /**< File info. */
} REF_CMPT_PLAYER_DEMUXER_FORMAT_INFO_s;

/**
 *  Demuxer pipe info
 */
typedef struct {
    UINT8 IsCreated;                                                                /**< Whether Demuxer is created. */
    REF_CMPT_PLAYER_DEMUXER_FORMAT_INFO_s FormatInfo[AMBA_DEMUXER_MAX_FORMAT_PER_PIPE]; /**< Demuxer format info */
} REF_CMPT_PLAYER_DEMUXER_PIPE_INFO_s;

/**
 *  Player info
 */
typedef struct {
    UINT8 InitMode;                                                                 /**< Init mode. Ex: REF_PLAYER_VIDEO_INIT_MODE_PYRAMID. */
    UINT8 PipeNum;                                                                  /**< Number of pipes to run at the same time. */
    UINT8 PipeCount;                                                                /**< Current number of pipes */
    AMBA_KAL_MUTEX_t Mutex;                                                         /**< Mutex */
    AMBA_KAL_EVENT_FLAG_t VideoFlag;                                                /**< Video Decoder event flag */
    REF_CMPT_PLAYER_DEMUXER_PIPE_INFO_s PipeInfo[REF_CMPT_PLAYER_MAX_PIPE];           /**< Demuxer pipe info */
    // Video decoder
    UINT8 VideoDecState; // The state of video decoder
    UINT8 VideoDir; // Array of playback direction
    UINT8 DbgPrintDecodeEvent;
    UINT16 VideoSpeedIdx; // Array of playback speed. 0: Normal speed
    UINT16 VideoStreamIdx; ///< Array of decoder ID
    // Dummy demux
    REF_CMPT_DEMUX_HDLR_s* DmxHdlr;  ///< Dummy demux handler
    UINT8  *DecBsBufAddr;
    UINT32  DecBsBufSize;
    UINT8 IsUserDataSet;
    REF_CMPT_FORMAT_USER_DATA_s UserData;
} REF_CMPT_PLAYER_INFO_s;

/**
 * Ref Player init or not
 */
static UINT8 G_RefCmptPlayer_Init = 0U;                             /**< Demuxer init flag. */

static REF_CMPT_PLAYER_INFO_s G_RefCmptPlayerInfo = {0};

// Number of displayer
#if 0 // TODO Support display on Vout (InitMode == REF_PLAYER_VIDEO_INIT_MODE_NORMAL)
static UINT16 PostCtrlNum = 0U;
static AMBA_DSP_VIDDEC_POST_CTRL_s MultiPostCtrl[AMBA_DSP_MAX_VOUT_NUM] = {0};
#endif

static inline UINT32 K2A(UINT32 Ret)
{
    UINT32 Rval = REFCODE_OK;
    if (Ret != KAL_ERR_NONE) {
        Rval = REFCODE_GENERAL_ERR;
    }
    return Rval;
}

static inline UINT32 DSP2A(UINT32 Ret)
{
    UINT32 Rval = REFCODE_OK;
    if (Ret != OK) {
        Rval = REFCODE_GENERAL_ERR;
    }
    return Rval;
}

static inline UINT32 ST2A(UINT32 Ret)
{
    UINT32 Rval = REFCODE_OK;

    if (Ret != OK) {
        if (Ret == STREAM_ERR_0002) {
            Rval = REFCODE_IO_ERR;
        } else {
            Rval = REFCODE_GENERAL_ERR;
        }
    }

    return Rval;
}

/**
* is video running
* @return yes/no (1/0)
*/
UINT32 RefCmptPlayer_IsVideoRunning(void)
{
    UINT32 RetVal;

    // TODO Lock Mutex. Fix the dead lock in RefCmptPlayer_StopPipe -> RefCmptDemuxer_Stop
    if ((G_RefCmptPlayerInfo.VideoDecState == REF_CMPT_PLAYER_STATE_RUN) ||
        (G_RefCmptPlayerInfo.VideoDecState == REF_CMPT_PLAYER_STATE_PAUSE)) {
        RetVal = 1U;
    } else {
        RetVal = 0U;
    }

    return RetVal;
}

/**
 * Whether the data in raw buffer is running low.
 *
 * @param [in] dmxHdlr      dmxHdlr Dummy demux handler
 *
 * @return 0 - data is sufficient, others - data is running low
 */
static UINT32 RefCmptPlayer_RawBuffer_DataIsLow(const REF_CMPT_DEMUX_HDLR_s* dmxHdlr)
{
    UINT32 RemainSpace = 0;
    UINT32 Rval;

    //RefCmptDemuxer_Lock(dmxHdlr, AMBA_KAL_WAIT_FOREVER);
    Rval = RefCmptDemuxer_GetFreeSpace(dmxHdlr, &RemainSpace);
    //RefCmptDemuxer_UnLock(dmxHdlr);
    if (Rval == REFCODE_OK) {
        if ((G_RefCmptPlayerInfo.DecBsBufSize / 2U) < RemainSpace) {
            Rval = 1U; // Data is running low
        } else {
            Rval = 0U;
        }
    }
    return Rval;
}

/**
 * Check remain space in Raw Buffer.\n
 * Feed more frames when data is running low.
 *
 * @param [in] dmxHdlr      dmxHdlr Demux handler
 *
 * @return None
 */
static void RefCmptPlayer_CheckRemainBuffer(REF_CMPT_DEMUX_HDLR_s* dmxHdlr)
{
//    UINT32 ActualEvent; // Output value from AmbaKAL_EventFlagTake

    switch (G_RefCmptPlayerInfo.VideoDecState) {
    case REF_CMPT_PLAYER_STATE_READY:
    case REF_CMPT_PLAYER_STATE_RUN:
    case REF_CMPT_PLAYER_STATE_PAUSE:
        // Check whether to feed more frames when data is running low.
        if (RefCmptPlayer_RawBuffer_DataIsLow(dmxHdlr) == 1U) { // Data is running low
            (void) RefCmptDemuxer_Feed(dmxHdlr, 4U); // Feed more frames
        }
        break;
    default:
        // Do nothing
        break;
    }
}

static UINT32 RefCmptPlayer_DecStateEventHdlr(const void *pEventData)
{
    const AMBA_DSP_EVENT_VIDEO_DEC_STATUS_UPDATE_s *State;
//    UINT32 PtsIsChanged = 0; // Whether the displayed PTS is different from the previous one. Always 0 when the handler is not running.
    //AMBA_DSP_EVENT_VIDEO_DEC_STATUS_UPDATE_s DspDecState;
    static UINT8 decoder_err = 0;
    static UINT8 PrevDecodeState = DEC_STATE_INVALID;
//    static UINT64 LatestValidPts = 0; ///< The latest displayed PTS. Don't update it when DSP reports an invalid PTS.
//    static UINT64 LatestPts = 0; ///< The latest displayed PTS. Update it even if DSP reports an invalid PTS.
    UINT32 RetVal = REFCODE_OK;

    // AmbaPrint_PrintStr5("!!! %s: !!!", __func__, NULL, NULL, NULL, NULL);

    // Preliminary check
    if (pEventData == NULL) {
        RetVal = ERR_ARG;
    } else {
        // Get event data
        AmbaMisra_TypeCast(&State, &pEventData);

        /*
            Do not use NumOfDisplayPic to check new frames.
            Sometimes NumOfDisplayPic doesn't change when a new frame is displayed.
            Especially in "dec to vproc" mode.
         */

        /* Check DSP state changes to idle */
        if (State->DecState != PrevDecodeState) {
            if ((State->DecState == DEC_STATE_IDLE) ||
                (State->DecState == DEC_STATE_IDLE_WITH_LAST_PIC) ||
                (State->DecState == DEC_STATE_INVALID)) {
                RetVal = K2A(AmbaKAL_EventFlagSet(&G_RefCmptPlayerInfo.VideoFlag, REF_CMPT_FLAG_DECODE_IDLE));
                if (RetVal != REFCODE_OK) {
                    AmbaPrint_PrintStr5("%s: [ERROR] AmbaKAL_EventFlagSet DECODE_IDLE fail", __func__, NULL, NULL, NULL, NULL);
                }
            } else {
                RetVal = K2A(AmbaKAL_EventFlagClear(&G_RefCmptPlayerInfo.VideoFlag, REF_CMPT_FLAG_DECODE_IDLE));
                if (RetVal != REFCODE_OK) {
                    AmbaPrint_PrintStr5("%s: [ERROR] AmbaKAL_EventFlagClear DECODE_IDLE fail", __func__, NULL, NULL, NULL, NULL);
                }
            }
        }

        // if ((PrevDecodeState == DEC_STATE_INVALID) && (State->DecState != DEC_STATE_INVALID)) {
        //     RetVal = K2A(AmbaKAL_EventFlagSet(&G_RefCmptPlayerInfo.VideoFlag, REF_CMPT_FLAG_FRAME_DISPLAY));
        //     if (RetVal != REFCODE_OK) {
        //         AmbaPrint_PrintStr5("%s: [ERROR] AmbaKAL_EventFlagSet FRAME_DISPLAY fail", __func__, NULL, NULL, NULL, NULL);
        //     }
        // }
        PrevDecodeState = State->DecState;

        /* Check EOS */
        if (RetVal == REFCODE_OK) {
            if (State->EndOfStream != 0U) {
                RetVal = K2A(AmbaKAL_EventFlagSet(&G_RefCmptPlayerInfo.VideoFlag, REF_CMPT_FLAG_PLAYBACK_EOS));
                if (RetVal != REFCODE_OK) {
                    AmbaPrint_PrintStr5("%s: [ERROR] AmbaKAL_EventFlagSet PLAYBACK_EOS fail", __func__, NULL, NULL, NULL, NULL);
                }
            }
        }

        switch (G_RefCmptPlayerInfo.VideoDecState) {
        case REF_CMPT_PLAYER_STATE_READY:
            // Reset PTS
//            LatestValidPts = REF_CMPT_PLAYER_INVALID_PTS;
//            LatestPts = REF_CMPT_PLAYER_INVALID_PTS;

//            PtsIsChanged = 0; // Always 0 when the handler is neigher running nor pausing.
            break;
        case REF_CMPT_PLAYER_STATE_OPENED:
            // Retain LatestValidPts

//            PtsIsChanged = 0; // Always 0 when the handler is neigher running nor pausing.
            break;
        case REF_CMPT_PLAYER_STATE_RUN:
        case REF_CMPT_PLAYER_STATE_PAUSE:
            // Only update RP when running
            AmbaMisra_TypeCast(&G_RefCmptPlayerInfo.DmxHdlr->AvcRawReadPoint, &State->BitsNextReadAddr);
            break;
        default:
//            PtsIsChanged = 0; // Always 0 when the handler is neigher running nor pausing.
            break;
        }
        // for dbg

        if ((G_RefCmptPlayerInfo.DbgPrintDecodeEvent & REF_CMPT_DEBUG_FLAG_STATUS_UPDATE) != 0U) {
            static UINT64 PrePts[4] = { 0xDEADBEEFULL, 0xDEADBEEFULL, 0xDEADBEEFULL, 0xDEADBEEFULL };
            static UINT32 PreDno[4] = { 0xDEADBEEFUL, 0xDEADBEEFUL, 0xDEADBEEFUL, 0xDEADBEEFUL };
            static UINT8 PreStatus[4] = {0U};
            static UINT32 PreNextRd[4] = {0};
            static UINT32 PreErrState[4] = {0};
            static UINT32 PreEos[4] = {0};
            static UINT32 PreDispPic = 0U;

            if ((PrePts[State->DecoderId] != State->DisplayFramePTS) || \
                (PreDno[State->DecoderId] != State->NumOfDecodedPic) || \
                (PreStatus[State->DecoderId] != State->DecState) ||\
                (PreNextRd[State->DecoderId] != State->BitsNextReadAddr) ||\
                (PreErrState[State->DecoderId] != State->ErrState) || \
                (PreEos[State->DecoderId] != State->EndOfStream) || \
                (PreDispPic != State->NumOfDisplayPic))
            {
                AmbaPrint_PrintUInt5("===== VideoDSP DecState:%u DisplayFramePTS:%-7d BitsNextReadAddr:0x%08x NumOfDecodedPic:%u NumOfDisplayPic:%u",
                        (UINT32)State->DecState,
                        (UINT32)State->DisplayFramePTS,
                        State->BitsNextReadAddr,
                        State->NumOfDecodedPic,
                        State->NumOfDisplayPic);
                if ((PreErrState[State->DecoderId] == 0U) && (State->ErrState != 0U)) {
                    AmbaPrint_PrintUInt5("RefCmptPlayer_DecStateEventHdlr: ErrState:0x%X", (UINT32)State->ErrState, 0U, 0U, 0U, 0U);
                }
                if ((PreEos[State->DecoderId] == 0U) && (State->EndOfStream != 0U)) {
                    AmbaPrint_PrintUInt5("RefCmptPlayer_DecStateEventHdlr: DecoderId %u EndOfStream %u", State->DecoderId, (UINT32)State->EndOfStream, 0U, 0U, 0U);
                }
                PrePts[State->DecoderId] = State->DisplayFramePTS;
                PreDno[State->DecoderId] = State->NumOfDecodedPic;
                PreStatus[State->DecoderId] = State->DecState;
                PreNextRd[State->DecoderId] = State->BitsNextReadAddr;
                PreErrState[State->DecoderId] = State->ErrState;
                PreEos[State->DecoderId] = State->EndOfStream;
                PreDispPic = State->NumOfDisplayPic;
            }
        }

        if ((State->ErrState) >= (DEC_ERR_FATAL)) {
            // print first error
            if (decoder_err == 0U) {
                AmbaPrint_PrintUInt5("RefCmptPlayer_DecStateEventHdlr: [ERROR] Wrong clip at frame %u", State->NumOfDecodedPic, 0U, 0U, 0U, 0U);
                decoder_err = State->ErrState;
            }
        }

        RefCmptPlayer_CheckRemainBuffer(G_RefCmptPlayerInfo.DmxHdlr); // Feed frames when under threshold

        if (State->EndOfStream != 0U) {
            AmbaPrint_PrintUInt5("PLAYBACK EOS", 0U, 0U, 0U, 0U, 0U);
        }
    }
    return RetVal;
}

/**
 *  Picture info event callback handler.
 *  @param [in] EventData           Event info.
 *  @return 0 - OK, others - Error
 */
static UINT32 RefCmptPlayer_PicInfoEventCB(const void *EventData)
{
    UINT32 Rval = REFCODE_OK;
    const AMBA_DSP_EVENT_VIDEO_DEC_PIC_INFO_s *DecPicInfo;
    static UINT32 PrevPicNum = 0xFFFFFFFFU;

    // AmbaPrint_PrintStr5("!!! %s: !!!", __func__, NULL, NULL, NULL, NULL);

    /* Parameter check */
    if (EventData == NULL) {
        Rval = REFCODE_GENERAL_ERR;
    } else {
        AmbaMisra_TypeCast(&DecPicInfo, &EventData);

        if ((G_RefCmptPlayerInfo.DbgPrintDecodeEvent & REF_CMPT_DEBUG_FLAG_PIC_INFO) != 0U) {
            if (DecPicInfo->DataFmt == AMBA_DSP_YUV420) {
                AmbaPrint_PrintUInt5("===== DecPic YUV420 Num %u Y %p UV %p Width %u Height %u", DecPicInfo->Num, DecPicInfo->YAddr, DecPicInfo->UVAddr, DecPicInfo->Width, DecPicInfo->Height);
            } else {
                AmbaPrint_PrintUInt5("===== DecPic YUV422 Num %u Y %p UV %p Width %u Height %u", DecPicInfo->Num, DecPicInfo->YAddr, DecPicInfo->UVAddr, DecPicInfo->Width, DecPicInfo->Height);
            }
        }

        /* Check whether a new frame is displayed */
        if (DecPicInfo->Num != PrevPicNum) {
            Rval = K2A(AmbaKAL_EventFlagSet(&G_RefCmptPlayerInfo.VideoFlag, REF_CMPT_FLAG_FRAME_DISPLAY));
            if (Rval != REFCODE_OK) {
                AmbaPrint_PrintStr5("%s: [ERROR] AmbaKAL_EventFlagSet FRAME_DISPLAY fail", __func__, NULL, NULL, NULL, NULL);
            }
        }
        PrevPicNum = DecPicInfo->Num;
    }

    return Rval;
}

static UINT32 RefCmptPlayer_OpenDemuxer(const char* VideoPath)
{
    UINT32 Rval = REFCODE_OK;

    {
        /** Get target file name (.h264, .nhnt) */

        AmbaPrint_PrintStr5("Video decode: %s", VideoPath, NULL, NULL, NULL, NULL);

        Rval = RefCmptDemuxer_Open(G_RefCmptPlayerInfo.DmxHdlr, VideoPath);
        if (Rval != REFCODE_OK) {
            AmbaPrint_PrintStr5("%s: [ERROR] RefCmptDemuxer_Open fail", __func__, NULL, NULL, NULL, NULL);
        }
    }

    return Rval;
}

static UINT32 RefCmptPlayer_VideoCodecConfig(UINT32 BitsFormat)
{
    UINT32 Rval = REFCODE_OK;

    switch (G_RefCmptPlayerInfo.VideoDecState) {
    case REF_CMPT_PLAYER_STATE_READY: {
        AMBA_DSP_VIDDEC_STREAM_CONFIG_s DecStreamConfig;

            /* Setting Dec Cofnig */
        DecStreamConfig.MaxVideoBufferWidth = REF_CMPT_PLAYER_MAX_VIDEO_WIDTH;
        DecStreamConfig.MaxVideoBufferHeight = REF_CMPT_PLAYER_MAX_VIDEO_HEIGHT;
        AmbaMisra_TypeCast(&DecStreamConfig.BitsBufAddr, &G_RefCmptPlayerInfo.DecBsBufAddr);
        DecStreamConfig.BitsBufSize = G_RefCmptPlayerInfo.DecBsBufSize;
        if (BitsFormat == REF_CMPT_DEMUX_PB_H264) {
            DecStreamConfig.BitsFormat = AMBA_DSP_DEC_BITS_FORMAT_H264;
        } else {
            DecStreamConfig.BitsFormat = AMBA_DSP_DEC_BITS_FORMAT_H265;
        }
        if (G_RefCmptPlayerInfo.DmxHdlr->AvcUserData.nVideoWidth != 0U) {
            // DecStreamConfig.MaxFrameWidth = (UINT16) G_RefCmptPlayerInfo.DmxHdlr->AvcUserData.nVideoWidth;
            DecStreamConfig.MaxFrameWidth = (UINT16) REF_CMPT_PLAYER_MAX_VIDEO_WIDTH;
        }
        if (G_RefCmptPlayerInfo.DmxHdlr->AvcUserData.nVideoHeight != 0U) {
            // DecStreamConfig.MaxFrameHeight = (UINT16) G_RefCmptPlayerInfo.DmxHdlr->AvcUserData.nVideoHeight;
            DecStreamConfig.MaxFrameHeight = (UINT16) REF_CMPT_PLAYER_MAX_VIDEO_HEIGHT;
        }
        DecStreamConfig.StreamID = 0U;

        /* Setting Enc Config*/
        DecStreamConfig.XcodeMode = AMBA_DSP_XCODE_NONE;
        Rval = DSP2A(AmbaDSP_VideoDecConfig(1U, &DecStreamConfig));
        if (Rval != REFCODE_OK) {
            AmbaPrint_PrintStr5("%s: [ERROR] AmbaDSP_VideoDecConfig fail", __func__, NULL, NULL, NULL, NULL);
        }
    }
        break;
    default:
        // Do nothing
        break;
    }

    return Rval;
}

static UINT32 RefCmptPlayer_CreatePipe(UINT8 PipeId, UINT8 FormatCount, const REF_PLAYER_DEMUXER_FILE_INFO_s FileInfo[AMBA_DEMUXER_MAX_FORMAT_PER_PIPE], SVC_STREAM_HDLR_s **Hdlr)
{
    ULONG  Addr;
    UINT32 Rval = REFCODE_OK;
    UINT32 I;
    UINT32 BitsFormat = REF_CMPT_DEMUX_PB_H264;   // codec. 0: h264 1: hevc
    REF_CMPT_DEMUX_CREATE_CFG_s DemuxCfg;
    // TODO: Currently only support single video. Add check.
    // TODO: Support multiple video

    // Create a demuxer

    DemuxCfg.AvcRawBuffer = G_RefCmptPlayerInfo.DecBsBufAddr;

    AmbaMisra_TypeCast(&Addr, &(G_RefCmptPlayerInfo.DecBsBufAddr));
    Addr = (Addr + G_RefCmptPlayerInfo.DecBsBufSize) - 1UL;
    AmbaMisra_TypeCast(&DemuxCfg.AvcRawLimit, &Addr);

    Rval = RefCmptDemuxer_Create(&DemuxCfg, &G_RefCmptPlayerInfo.DmxHdlr, Hdlr);
    if (Rval != REFCODE_OK) {
        AmbaPrint_PrintStr5("%s: [ERROR] RefCmptDemuxer_Create fail", __func__, NULL, NULL, NULL, NULL);
    }

    if (Rval == REFCODE_OK) {
        Rval = RefCmptDemuxer_SetGopConfig(G_RefCmptPlayerInfo.DmxHdlr, G_RefCmptPlayerInfo.UserData.nIdrInterval, G_RefCmptPlayerInfo.UserData.nTimeScale, G_RefCmptPlayerInfo.UserData.nTimePerFrame, G_RefCmptPlayerInfo.UserData.nN, G_RefCmptPlayerInfo.UserData.nM);
        if (Rval != REFCODE_OK) {
            AmbaPrint_PrintStr5("%s: [ERROR] RefCmptDemuxer_SetGopConfig fail", __func__, NULL, NULL, NULL, NULL);
        }
    }

    for (I = 0U; I < FormatCount; ++I) {
        if (FileInfo[I].FormatType == REF_PLAYER_FORMAT_TYPE_CMPT) {
            Rval = RefCmptPlayer_OpenDemuxer(FileInfo[I].Prefix);
            if (Rval != REFCODE_OK) {
                AmbaPrint_PrintStr5("%s: [ERROR] RefCmptPlayer_OpenDemuxer fail", __func__, NULL, NULL, NULL, NULL);
            }
        } else {
            // Do nothing
        }
    }

    // VideoDecConfig
    if (Rval == REFCODE_OK) {
        if (G_RefCmptPlayerInfo.DmxHdlr->CodingType == REF_CMPT_DEMUX_PB_H264) {
            AmbaPrint_PrintUInt5("RefCmptPlayer_CreatePipe: H264", 0U, 0U, 0U, 0U, 0U);
            BitsFormat = REF_CMPT_DEMUX_PB_H264;
        } else {
            AmbaPrint_PrintUInt5("RefCmptPlayer_CreatePipe: H265", 0U, 0U, 0U, 0U, 0U);
            BitsFormat = REF_CMPT_DEMUX_PB_H265;
        }
        Rval = RefCmptPlayer_VideoCodecConfig(BitsFormat);
        if (Rval != REFCODE_OK) {
            AmbaPrint_PrintStr5("%s: [ERROR] RefCmptPlayer_VideoCodecConfig fail", __func__, NULL, NULL, NULL, NULL);
        }
    }

    AmbaMisra_TouchUnused(&PipeId);//TODO Use PipeId
    return Rval;
}

static UINT32 RefCmptPlayer_DeletePipe(UINT8 PipeId)
{
    UINT32 Rval = REFCODE_OK;

    Rval = RefCmptDemuxer_Close(G_RefCmptPlayerInfo.DmxHdlr);
    if (Rval != REFCODE_OK) {
        AmbaPrint_PrintStr5("%s: [ERROR] RefCmptDemuxer_Close fail", __func__, NULL, NULL, NULL, NULL);
    }

    // Delete demuxer
    if (Rval == REFCODE_OK) {
        Rval = RefCmptDemuxer_Delete(G_RefCmptPlayerInfo.DmxHdlr);
        if (Rval != REFCODE_OK) {
            AmbaPrint_PrintStr5("%s: [ERROR] RefCmptDemuxer_Delete fail", __func__, NULL, NULL, NULL, NULL);
        }
    }

    AmbaMisra_TouchUnused(&PipeId);//TODO Use PipeId
    return Rval;
}

#if 0 // TODO Support display on Vout (InitMode == REF_PLAYER_VIDEO_INIT_MODE_NORMAL)
static void RefCmptPlayer_FitSizeToWin(UINT32 VoutIdx) {

    FLOAT WidthPxlRatioOnDev, HeightPxlRatioOnDev;
    FLOAT DevPixelAr;
    FLOAT AH, AW, BH, BW, TH, TW;
    UINT32 AR16X9 = ((16UL << 8UL) | 9UL);
    UINT32 AR4X3 = ((4UL << 8UL) | 3UL);
    FLOAT ARA, ARB;

    AW = 1.0F * (FLOAT)SSPUTSystemInfo.PrevWidthA;
    AH = 1.0F * (FLOAT)SSPUTSystemInfo.PrevHeightA;
    BW = 1.0F * (FLOAT)SSPUTSystemInfo.PrevWidthB;
    BH = 1.0F * (FLOAT)SSPUTSystemInfo.PrevHeightB;


    if (VoutIdx == VOUT_IDX_A) {
        if (SSPUTSystemInfo.VoutAr[VoutIdx] == (UINT16)AR16X9) {
            // T30P61 -> 16:9
            DevPixelAr = ((16.0F * AH / AW) / 9.0F);
        } else if (SSPUTSystemInfo.VoutAr[VoutIdx] == (UINT16)AR4X3) {
            //winTek -> 4:3
            DevPixelAr = ((4.0F * AH / AW) / 3.0F);
        } else {
            DevPixelAr = AH / AW;
        }
    } else {
        //TV -> 16:9
        if (SSPUTSystemInfo.VoutAr[VoutIdx] == (UINT16)AR16X9) {
            // T30P61 -> 16:9
            DevPixelAr = ((16.0F * BH / BW) / 9.0F);
        } else if (SSPUTSystemInfo.VoutAr[VoutIdx] == (UINT16)AR4X3) {
            //winTek -> 4:3
            DevPixelAr = ((4.0F * BH / BW) / 3.0F);
        } else {
            DevPixelAr = BH / BW;
        }
    }

    WidthPxlRatioOnDev = 1.0F * ((FLOAT) DmxHdlr->AvcUserData.nVideoWidth) / DevPixelAr;
    HeightPxlRatioOnDev = 1.0F * ((FLOAT) DmxHdlr->AvcUserData.nVideoHeight);
    TH = ((FLOAT)MultiPostCtrl[VoutIdx].TargetWindow.Height);
    TW = ((FLOAT)MultiPostCtrl[VoutIdx].TargetWindow.Width);

    // Should 4Byte alignment
    ARA = (WidthPxlRatioOnDev / HeightPxlRatioOnDev);
    ARB = (TW / TH);
    if (ARA > ARB) {
        // black bar at top and bottom
        ARA = (TW * HeightPxlRatioOnDev / WidthPxlRatioOnDev);
        MultiPostCtrl[VoutIdx].TargetWindow.Height = (UINT16) ARA;
        MultiPostCtrl[VoutIdx].TargetWindow.Height = (MultiPostCtrl[VoutIdx].TargetWindow.Height >> 2U) << 2U;
        MultiPostCtrl[VoutIdx].VoutWindow.OffsetY = \
                (MultiPostCtrl[VoutIdx].VoutWindow.Height - MultiPostCtrl[VoutIdx].TargetWindow.Height);
        MultiPostCtrl[VoutIdx].VoutWindow.OffsetY >>= 1U;
        MultiPostCtrl[VoutIdx].VoutWindow.Height = MultiPostCtrl[VoutIdx].TargetWindow.Height;
    } else {
        // black bar at left and right
        ARA = (TH * WidthPxlRatioOnDev / HeightPxlRatioOnDev);
        MultiPostCtrl[VoutIdx].TargetWindow.Width = (UINT16)ARA;
        MultiPostCtrl[VoutIdx].TargetWindow.Width = (MultiPostCtrl[VoutIdx].TargetWindow.Width >> 2U) << 2U;
        MultiPostCtrl[VoutIdx].VoutWindow.OffsetX = \
                (MultiPostCtrl[VoutIdx].VoutWindow.Width - MultiPostCtrl[VoutIdx].TargetWindow.Width);
        MultiPostCtrl[VoutIdx].VoutWindow.OffsetX >>= 1U;
        MultiPostCtrl[VoutIdx].VoutWindow.Width = MultiPostCtrl[VoutIdx].TargetWindow.Width;
    }
    AmbaPrint_PrintUInt5("MultiPostCtrl[%d] width = %d height %d, DevPixelAr:%f",
            VoutIdx,
            (UINT32)MultiPostCtrl[VoutIdx].TargetWindow.Width,
            (UINT32)MultiPostCtrl[VoutIdx].TargetWindow.Height,
            (UINT32)DevPixelAr, 0U);
}

static UINT32 RefCmptPlayer_VideoPostCmd(void)
{
    UINT32 Rval;

    {
        UINT32 Idx = 0;
        if (SSPUTSystemInfo.Vout0Hooked == VOUT_HOOKED_ENABLED) {
            Idx = VOUT_IDX_A;
            MultiPostCtrl[Idx].InputWindow.Width = (UINT16) DmxHdlr->AvcUserData.nVideoWidth;
            MultiPostCtrl[Idx].InputWindow.Height = (UINT16) (DmxHdlr->AvcUserData.nVideoHeight >> SSPUTSystemInfo.FratePrevA.Interlace);
            MultiPostCtrl[Idx].InputWindow.OffsetX = 0U;
            MultiPostCtrl[Idx].InputWindow.OffsetY = 0U;

            if ((MultiPostCtrl[Idx].VoutWindow.Width == 0U) ||  (MultiPostCtrl[Idx].VoutWindow.Height == 0U)) {
                // Use default value
                MultiPostCtrl[Idx].VoutIdx = VOUT_IDX_A;
                MultiPostCtrl[Idx].InputWindow.OffsetX = 0U;
                MultiPostCtrl[Idx].InputWindow.OffsetY = 0U;
                MultiPostCtrl[Idx].TargetWindow.OffsetX = 0U;
                MultiPostCtrl[Idx].TargetWindow.OffsetY = 0U;
                MultiPostCtrl[Idx].TargetWindow.Width = SSPUTSystemInfo.PrevWidthA;
                MultiPostCtrl[Idx].TargetWindow.Height = SSPUTSystemInfo.PrevHeightA;
                MultiPostCtrl[Idx].VoutWindow.OffsetX = 0U;
                MultiPostCtrl[Idx].VoutWindow.OffsetY = 0U;
                MultiPostCtrl[Idx].VoutWindow.Width = SSPUTSystemInfo.PrevWidthA;
                MultiPostCtrl[Idx].VoutWindow.Height = SSPUTSystemInfo.PrevHeightA;

                if (SSPUTSystemInfo.FratePrevA.Interlace == 0U) {
                    RefCmptPlayer_FitSizeToWin(VOUT_IDX_A);
                }
                MultiPostCtrl[Idx].TargetWindow.Height = (MultiPostCtrl[Idx].TargetWindow.Height >> SSPUTSystemInfo.FratePrevA.Interlace);
                MultiPostCtrl[Idx].VoutWindow.Height = (MultiPostCtrl[Idx].VoutWindow.Height >> SSPUTSystemInfo.FratePrevA.Interlace);
                MultiPostCtrl[Idx].VoutWindow.OffsetY = (MultiPostCtrl[Idx].VoutWindow.OffsetY >> SSPUTSystemInfo.FratePrevA.Interlace);
            }
        }
        //TV
        if (SSPUTSystemInfo.Vout1Hooked == VOUT_HOOKED_ENABLED) {
            Idx = VOUT_IDX_B;
            MultiPostCtrl[Idx].InputWindow.Width = (UINT16) DmxHdlr->AvcUserData.nVideoWidth;
            MultiPostCtrl[Idx].InputWindow.Height =  (UINT16)(DmxHdlr->AvcUserData.nVideoHeight >> SSPUTSystemInfo.FratePrevB.Interlace);
            MultiPostCtrl[Idx].InputWindow.OffsetX = 0U;
            MultiPostCtrl[Idx].InputWindow.OffsetY = 0U;

            if ((MultiPostCtrl[Idx].VoutWindow.Width == 0U) ||  (MultiPostCtrl[Idx].VoutWindow.Height == 0U)) {
                // Use default value
                MultiPostCtrl[Idx].VoutIdx = VOUT_IDX_B;
                MultiPostCtrl[Idx].InputWindow.OffsetX = 0U;
                MultiPostCtrl[Idx].InputWindow.OffsetY = 0U;
                MultiPostCtrl[Idx].TargetWindow.OffsetX = 0U;
                MultiPostCtrl[Idx].TargetWindow.OffsetY = 0U;
                MultiPostCtrl[Idx].TargetWindow.Width = SSPUTSystemInfo.PrevWidthB;
                MultiPostCtrl[Idx].TargetWindow.Height = SSPUTSystemInfo.PrevHeightB;
                MultiPostCtrl[Idx].VoutWindow.OffsetX = 0U;
                MultiPostCtrl[Idx].VoutWindow.OffsetY = 0U;
                MultiPostCtrl[Idx].VoutWindow.Width = SSPUTSystemInfo.PrevWidthB;
                MultiPostCtrl[Idx].VoutWindow.Height = SSPUTSystemInfo.PrevHeightB;

                if (SSPUTSystemInfo.FratePrevB.Interlace == 0U) {
                    RefCmptPlayer_FitSizeToWin(VOUT_IDX_B);
                }
                MultiPostCtrl[Idx].TargetWindow.Height = (MultiPostCtrl[Idx].TargetWindow.Height >> SSPUTSystemInfo.FratePrevB.Interlace);
                MultiPostCtrl[Idx].VoutWindow.Height = (MultiPostCtrl[Idx].VoutWindow.Height >> SSPUTSystemInfo.FratePrevB.Interlace);
                MultiPostCtrl[Idx].VoutWindow.OffsetY = (MultiPostCtrl[Idx].VoutWindow.OffsetY >> SSPUTSystemInfo.FratePrevB.Interlace);
            }
        }
        Rval = RefCmptPlayer_VideoDecPostCtrl(0U);

        if (Rval != REFCODE_OK) {
            AmbaPrint_PrintStr5("%s: [ERROR] RefCmptPlayer_VideoDecPostCtrl fail", __func__, NULL, NULL, NULL, NULL);
        }
    }
        return Rval;
}
#endif

static UINT32 RefCmptPlayer_StartPipe(UINT8 PipeId, UINT32 StartTime, UINT8 Direction, UINT32 Speed)
{
#define REF_DECODE_SPEED_INDEX_NUM              (11U)           /**< Number of speed index */
    /**
     * Mapping of playback speed and speed index (such as AMBA_DSP_VIDDEC_SPEED_01X)
     */
    static const UINT32 G_SpeedMapping[REF_DECODE_SPEED_INDEX_NUM] = {
        256U,       /**< Playback speed 1x    */
        512U,       /**< Playback speed 2x    */
        1024U,      /**< Playback speed 4x    */
        2048U,      /**< Playback speed 8x    */
        4096U,      /**< Playback speed 16x   */
        8192U,      /**< Playback speed 32x   */
        16384U,     /**< Playback speed 64x   */
        128U,       /**< Playback speed 1/2x  */
        64U,        /**< Playback speed 1/4x  */
        32U,        /**< Playback speed 1/8x  */
        16U         /**< Playback speed 1/16x */
    };

    UINT32 Rval;
    UINT32 I;

    Rval = DSP2A(AmbaDSP_EventHandlerRegister(AMBA_DSP_EVENT_VIDEO_DEC_STATUS, RefCmptPlayer_DecStateEventHdlr));
    if (Rval != REFCODE_OK) {
        AmbaPrint_PrintStr5("%s: [ERROR] AmbaDSP_EventHandlerRegister AMBA_DSP_EVENT_VIDEO_DEC_STATUS fail", __func__, NULL, NULL, NULL, NULL);
    }

    if (Rval == REFCODE_OK) {
        Rval = DSP2A(AmbaDSP_EventHandlerRegister(AMBA_DSP_EVENT_VIDEO_DEC_PIC_INFO, RefCmptPlayer_PicInfoEventCB));
        if (Rval != REFCODE_OK) {
            AmbaPrint_PrintStr5("%s: [ERROR] AmbaDSP_EventHandlerRegister AMBA_DSP_EVENT_VIDEO_DEC_PIC_INFO fail", __func__, NULL, NULL, NULL, NULL);
        }
    }

    /** Pre-feed */
    AmbaPrint_PrintUInt5("RefCmptPlayer_StartPipe: StartTime = %d  Speed = 0x%3x  Direction = %u", StartTime, Speed, (UINT32)Direction, 0U, 0U);
    /* Unlock mutex before waiting */
    Rval = K2A(AmbaKAL_MutexGive(&G_RefCmptPlayerInfo.Mutex));
    if (Rval == REFCODE_OK) {
        Rval = RefCmptDemuxer_Start(G_RefCmptPlayerInfo.DmxHdlr, &StartTime, (UINT8)Direction, Speed);

        if (K2A(AmbaKAL_MutexTake(&G_RefCmptPlayerInfo.Mutex, REF_CMPT_PLAYER_MUTEX_TIMEOUT)) != REFCODE_OK) {
            AmbaPrint_PrintStr5("%s: [ERROR] AmbaKAL_MutexTake fail", __func__, NULL, NULL, NULL, NULL);
            Rval = REFCODE_GENERAL_ERR;
        }
    } else {
        AmbaPrint_PrintStr5("%s: [ERROR] AmbaKAL_MutexGive fail", __func__, NULL, NULL, NULL, NULL);
    }

#if 0 // TODO Support display on Vout (InitMode == REF_PLAYER_VIDEO_INIT_MODE_NORMAL)
    if (Rval == REFCODE_OK) {
        Rval = RefCmptPlayer_VideoPostCmd();
    }
#endif

    /*
     * Lock demuxer task
     * Avoid feeding frames when Decoder is not started.
     */
    if (Rval == REFCODE_OK) {
        Rval = RefCmptDemuxer_Lock(G_RefCmptPlayerInfo.DmxHdlr, REF_CMPT_PLAYER_MUTEX_TIMEOUT);
        if (Rval == REFCODE_OK) {
            /* Clear flag before decoder start */
            Rval = K2A(AmbaKAL_EventFlagClear(&G_RefCmptPlayerInfo.VideoFlag, (REF_CMPT_FLAG_FRAME_DISPLAY | REF_CMPT_FLAG_PLAYBACK_EOS)));
            if (Rval != REFCODE_OK) {
                AmbaPrint_PrintStr5("%s: [ERROR] AmbaKAL_EventFlagClear fail", __func__, NULL, NULL, NULL, NULL);
            }

            if (Rval == REFCODE_OK) {
                AMBA_DSP_VIDDEC_START_CONFIG_s StartConfig = {0};
                ULONG  WP, RP;
                UINT16 SpeedIdx = AMBA_DSP_VIDDEC_SPEED_01X;

                for (I = 0U; I < REF_DECODE_SPEED_INDEX_NUM; ++ I) {
                    if (G_SpeedMapping[I] == Speed) {
                        SpeedIdx = (UINT16)I;
                        break;
                    }
                }
                // Set speed and direction
                G_RefCmptPlayerInfo.VideoSpeedIdx = SpeedIdx;
                G_RefCmptPlayerInfo.VideoDir = (UINT8) Direction;

                StartConfig.FirstDisplayPTS = StartTime;
                StartConfig.SpeedIndex = G_RefCmptPlayerInfo.VideoSpeedIdx;
                StartConfig.Direction =  (UINT16) G_RefCmptPlayerInfo.VideoDir;
                AmbaMisra_TypeCast(&RP, &G_RefCmptPlayerInfo.DmxHdlr->AvcRawReadPoint);
                AmbaMisra_TypeCast(&WP, &G_RefCmptPlayerInfo.DmxHdlr->AvcRawWritePoint);
                StartConfig.PreloadDataSize = WP - RP;
                {
                    ULONG StartAddr = RP & 0xFFFFFFC0U;
                    ULONG EndAddr = (WP + 63U) & 0xFFFFFFC0U;
                    UINT32 FrmSize = EndAddr - StartAddr;
                    (void)AmbaSvcWrap_CacheClean(StartAddr, FrmSize);
                }
                Rval = DSP2A(AmbaDSP_VideoDecStart(1U, &G_RefCmptPlayerInfo.VideoStreamIdx, &StartConfig));
                if (Rval != REFCODE_OK) {
                    AmbaPrint_PrintStr5("%s: [ERROR] AmbaDSP_VideoDecStart fail", __func__, NULL, NULL, NULL, NULL);
                }
            }

            /* Wait AmbaDSP_VideoDecStart done */
            if (Rval == REFCODE_OK) {
                /* Unlock mutex before waiting */
                Rval = K2A(AmbaKAL_MutexGive(&G_RefCmptPlayerInfo.Mutex));
                if (Rval == REFCODE_OK) {
                    UINT32 ActualFlag = 0U;
                    /* Wait first frame */
                    Rval = K2A(AmbaKAL_EventFlagGet(&G_RefCmptPlayerInfo.VideoFlag, REF_CMPT_FLAG_FRAME_DISPLAY, 1U /* And */, 1U /* Clear */, &ActualFlag, REF_CMPT_PLAYER_WAIT_FRAME_TIMEOUT));
                    if (Rval != REFCODE_OK) {
                        AmbaPrint_PrintStr5("%s: [ERROR] No video frame displayed", __func__, NULL, NULL, NULL, NULL);
                    }

                    if (K2A(AmbaKAL_MutexTake(&G_RefCmptPlayerInfo.Mutex, REF_CMPT_PLAYER_MUTEX_TIMEOUT)) != REFCODE_OK) {
                        AmbaPrint_PrintStr5("%s: [ERROR] AmbaKAL_MutexTake fail", __func__, NULL, NULL, NULL, NULL);
                        Rval = REFCODE_GENERAL_ERR;
                    }
                } else {
                    AmbaPrint_PrintStr5("%s: [ERROR] AmbaKAL_MutexGive fail", __func__, NULL, NULL, NULL, NULL);
                }
            }

            if (Rval == REFCODE_OK) {
                G_RefCmptPlayerInfo.VideoDecState = REF_CMPT_PLAYER_STATE_RUN;
            }

            if (RefCmptDemuxer_UnLock(G_RefCmptPlayerInfo.DmxHdlr) != REFCODE_OK) {
                AmbaPrint_PrintStr5("%s: [ERROR] RefCmptDemuxer_UnLock fail", __func__, NULL, NULL, NULL, NULL);
                Rval = REFCODE_GENERAL_ERR;
            }
        } else {
            AmbaPrint_PrintStr5("%s: [ERROR] RefCmptDemuxer_Lock fail", __func__, NULL, NULL, NULL, NULL);
        }
    }

    AmbaMisra_TouchUnused(&PipeId);//TODO Use PipeId
    return Rval;
}

static UINT32 RefCmptPlayer_StopPipe(UINT8 PipeId, UINT32 StopOption)
{
    UINT32 Rval = REFCODE_OK;

    /** Close demuxer */
    Rval = RefCmptDemuxer_Stop(G_RefCmptPlayerInfo.DmxHdlr);
    if (Rval != REFCODE_OK) {
        AmbaPrint_PrintStr5("%s: [ERROR] RefCmptDemuxer_Stop fail", __func__, NULL, NULL, NULL, NULL);
    } else {
        UINT8 ShowLastFrame = (UINT8) StopOption;

        switch (G_RefCmptPlayerInfo.VideoDecState) {
        case REF_CMPT_PLAYER_STATE_OPENED:
        case REF_CMPT_PLAYER_STATE_RUN:
        case REF_CMPT_PLAYER_STATE_PAUSE:
            Rval = DSP2A(AmbaDSP_VideoDecStop(1U, &G_RefCmptPlayerInfo.VideoStreamIdx, &ShowLastFrame));
            if (Rval != REFCODE_OK) {
                AmbaPrint_PrintStr5("%s: [ERROR] AmbaDSP_VideoDecStop fail", __func__, NULL, NULL, NULL, NULL);
            }
            /* Wait AmbaDSP_VideoDecStop done */
            if (Rval == REFCODE_OK) {
                /* Unlock mutex before waiting */
                Rval = K2A(AmbaKAL_MutexGive(&G_RefCmptPlayerInfo.Mutex));
                if (Rval == REFCODE_OK) {
                    UINT32 ActualFlag = 0U;
                    /* Wait DSP idle */
                    Rval = K2A(AmbaKAL_EventFlagGet(&G_RefCmptPlayerInfo.VideoFlag, REF_CMPT_FLAG_DECODE_IDLE, 1U /* And */, 0U /* No Clear */, &ActualFlag, REF_CMPT_PLAYER_FLAG_TIMEOUT));
                    if (Rval == REFCODE_OK) {
                        AmbaPrint_PrintStr5("%s: Video Decode Idle", __func__, NULL, NULL, NULL, NULL);
                    } else {
                        AmbaPrint_PrintStr5("%s: [ERROR] AmbaKAL_EventFlagGet fail", __func__, NULL, NULL, NULL, NULL);
                    }

                    if (K2A(AmbaKAL_MutexTake(&G_RefCmptPlayerInfo.Mutex, REF_CMPT_PLAYER_MUTEX_TIMEOUT)) != REFCODE_OK) {
                        AmbaPrint_PrintStr5("%s: [ERROR] AmbaKAL_MutexTake fail", __func__, NULL, NULL, NULL, NULL);
                        Rval = REFCODE_GENERAL_ERR;
                    }
                } else {
                    AmbaPrint_PrintStr5("%s: [ERROR] AmbaKAL_MutexGive fail", __func__, NULL, NULL, NULL, NULL);
                }
            }
            if (Rval == REFCODE_OK) {
                G_RefCmptPlayerInfo.VideoDecState = REF_CMPT_PLAYER_STATE_READY;

                // Reset raw buffer
                G_RefCmptPlayerInfo.DmxHdlr->AvcRawReadPoint = G_RefCmptPlayerInfo.DmxHdlr->AvcRawBuffer;
                G_RefCmptPlayerInfo.DmxHdlr->AvcRawWritePoint = G_RefCmptPlayerInfo.DmxHdlr->AvcRawBuffer;
            }
            break;
        default:
            // Do nothing
            break;
        }
    }
    AmbaMisra_TouchUnused(&PipeId);//TODO Use PipeId
    return Rval;
}

static UINT32 RefCmptPlayer_PausePipe(UINT8 PipeId)
{
    UINT32 Rval = REFCODE_OK;
    UINT32 ActualFlag = 0U;

    /* Check EOS */
    if (K2A(AmbaKAL_EventFlagGet(&G_RefCmptPlayerInfo.VideoFlag, REF_CMPT_FLAG_PLAYBACK_EOS, 1U /* And */, 0U /* No Clear */, &ActualFlag, 1U)) == REFCODE_OK) {
        AmbaPrint_PrintStr5("%s: [ERROR] Not supported after EOS", __func__, NULL, NULL, NULL, NULL);
        Rval = REFCODE_GENERAL_ERR;
    }

    if (Rval == REFCODE_OK) {
        AMBA_DSP_VIDDEC_TRICKPLAY_s TrickPlay[AMBA_DSP_MAX_DEC_STREAM_NUM] = {0};

        TrickPlay[0].Operation = AMBA_DSP_VIDDEC_PAUSE;
        G_RefCmptPlayerInfo.VideoDecState = REF_CMPT_PLAYER_STATE_PAUSE;
        Rval = DSP2A(AmbaDSP_VideoDecTrickPlay(1U, &G_RefCmptPlayerInfo.VideoStreamIdx, TrickPlay));
        if (Rval != REFCODE_OK) {
            AmbaPrint_PrintStr5("%s: [ERROR] AmbaDSP_VideoDecTrickPlay fail", __func__, NULL, NULL, NULL, NULL);
        }
    }

    AmbaMisra_TouchUnused(&PipeId);//TODO Use PipeId
    return Rval;
}

static UINT32 RefCmptPlayer_ResumePipe(UINT8 PipeId)
{
    UINT32 Rval = REFCODE_OK;
    UINT32 ActualFlag = 0U;

    /* Check EOS */
    if (K2A(AmbaKAL_EventFlagGet(&G_RefCmptPlayerInfo.VideoFlag, REF_CMPT_FLAG_PLAYBACK_EOS, 1U /* And */, 0U /* No Clear */, &ActualFlag, 1U)) == REFCODE_OK) {
        AmbaPrint_PrintStr5("%s: [ERROR] Not supported after EOS", __func__, NULL, NULL, NULL, NULL);
        Rval = REFCODE_GENERAL_ERR;
    }

    if (Rval == REFCODE_OK) {
        /* Clear flag before video resume */
        Rval = K2A(AmbaKAL_EventFlagClear(&G_RefCmptPlayerInfo.VideoFlag, REF_CMPT_FLAG_FRAME_DISPLAY));
        if (Rval != REFCODE_OK) {
            AmbaPrint_PrintStr5("%s: [ERROR] AmbaKAL_EventFlagClear fail", __func__, NULL, NULL, NULL, NULL);
        }
    }

    if (Rval == REFCODE_OK) {
        AMBA_DSP_VIDDEC_TRICKPLAY_s TrickPlay[AMBA_DSP_MAX_DEC_STREAM_NUM] = {0};

        TrickPlay[0].Operation = AMBA_DSP_VIDDEC_RESUME;
        TrickPlay[0].ResumeDirection = G_RefCmptPlayerInfo.VideoDir;
        TrickPlay[0].ResumeSpeedIndex = (UINT8)G_RefCmptPlayerInfo.VideoSpeedIdx;
        Rval = DSP2A(AmbaDSP_VideoDecTrickPlay(1U, &G_RefCmptPlayerInfo.VideoStreamIdx, TrickPlay));
        if (Rval != REFCODE_OK) {
            AmbaPrint_PrintStr5("%s: [ERROR] AmbaDSP_VideoDecTrickPlay fail", __func__, NULL, NULL, NULL, NULL);
        }
    }

    /* Wait AmbaDSP_VideoDecTrickPlay done */
    if (Rval == REFCODE_OK) {
        /* Unlock mutex before waiting */
        Rval = K2A(AmbaKAL_MutexGive(&G_RefCmptPlayerInfo.Mutex));
        if (Rval == REFCODE_OK) {
            /* Wait first frame */
            Rval = K2A(AmbaKAL_EventFlagGet(&G_RefCmptPlayerInfo.VideoFlag, REF_CMPT_FLAG_FRAME_DISPLAY, 1U /* And */, 1U /* Clear */, &ActualFlag, REF_CMPT_PLAYER_WAIT_FRAME_TIMEOUT));
            if (Rval != REFCODE_OK) {
                AmbaPrint_PrintStr5("%s: [ERROR] No video frame displayed", __func__, NULL, NULL, NULL, NULL);
            }

            if (K2A(AmbaKAL_MutexTake(&G_RefCmptPlayerInfo.Mutex, REF_CMPT_PLAYER_MUTEX_TIMEOUT)) != REFCODE_OK) {
                AmbaPrint_PrintStr5("%s: [ERROR] AmbaKAL_MutexTake fail", __func__, NULL, NULL, NULL, NULL);
                Rval = REFCODE_GENERAL_ERR;
            }
        } else {
            AmbaPrint_PrintStr5("%s: [ERROR] AmbaKAL_MutexGive fail", __func__, NULL, NULL, NULL, NULL);
        }
    }

    if (Rval == REFCODE_OK) {
        G_RefCmptPlayerInfo.VideoDecState = REF_CMPT_PLAYER_STATE_RUN;
    }

    AmbaMisra_TouchUnused(&PipeId);//TODO Use PipeId
    return Rval;
}

static UINT32 RefCmptPlayer_StepPipe(UINT8 PipeId)
{
    UINT32 Rval = REFCODE_OK;
    UINT32 ActualFlag = 0U;

    /* Check EOS */
    if (K2A(AmbaKAL_EventFlagGet(&G_RefCmptPlayerInfo.VideoFlag, REF_CMPT_FLAG_PLAYBACK_EOS, 1U /* And */, 0U /* No Clear */, &ActualFlag, 1U)) == REFCODE_OK) {
        AmbaPrint_PrintStr5("%s: [ERROR] Not supported after EOS", __func__, NULL, NULL, NULL, NULL);
        Rval = REFCODE_GENERAL_ERR;
    }

    if (Rval == REFCODE_OK) {
        /* Clear flag before video step */
        Rval = K2A(AmbaKAL_EventFlagClear(&G_RefCmptPlayerInfo.VideoFlag, REF_CMPT_FLAG_FRAME_DISPLAY));
        if (Rval != REFCODE_OK) {
            AmbaPrint_PrintStr5("%s: [ERROR] AmbaKAL_EventFlagClear fail", __func__, NULL, NULL, NULL, NULL);
        }
    }

    if (Rval == REFCODE_OK) {
        AMBA_DSP_VIDDEC_TRICKPLAY_s TrickPlay[AMBA_DSP_MAX_DEC_STREAM_NUM] = {0};

        TrickPlay[0].Operation = AMBA_DSP_VIDDEC_STEP;
        Rval = DSP2A(AmbaDSP_VideoDecTrickPlay(1U, &G_RefCmptPlayerInfo.VideoStreamIdx, TrickPlay));
        if (Rval != REFCODE_OK) {
            AmbaPrint_PrintStr5("%s: [ERROR] AmbaDSP_VideoDecTrickPlay fail", __func__, NULL, NULL, NULL, NULL);
        }
    }

    /* Wait AmbaDSP_VideoDecTrickPlay done */
    if (Rval == REFCODE_OK) {
        /* Unlock mutex before waiting */
        Rval = K2A(AmbaKAL_MutexGive(&G_RefCmptPlayerInfo.Mutex));
        if (Rval == REFCODE_OK) {
            /* Wait first frame */
            Rval = K2A(AmbaKAL_EventFlagGet(&G_RefCmptPlayerInfo.VideoFlag, REF_CMPT_FLAG_FRAME_DISPLAY, 1U /* And */, 1U /* Clear */, &ActualFlag, REF_CMPT_PLAYER_WAIT_FRAME_TIMEOUT));
            if (Rval != REFCODE_OK) {
                AmbaPrint_PrintStr5("%s: [ERROR] No video frame displayed", __func__, NULL, NULL, NULL, NULL);
            }

            if (K2A(AmbaKAL_MutexTake(&G_RefCmptPlayerInfo.Mutex, REF_CMPT_PLAYER_MUTEX_TIMEOUT)) != REFCODE_OK) {
                AmbaPrint_PrintStr5("%s: [ERROR] AmbaKAL_MutexTake fail", __func__, NULL, NULL, NULL, NULL);
                Rval = REFCODE_GENERAL_ERR;
            }
        } else {
            AmbaPrint_PrintStr5("%s: [ERROR] AmbaKAL_MutexGive fail", __func__, NULL, NULL, NULL, NULL);
        }
    }

    AmbaMisra_TouchUnused(&PipeId);//TODO Use PipeId
    return Rval;
}

/**
* init player
* @param [in]  InitMode
* @param [in]  BitsBufAddr
* @param [in]  BitsBufSize
* @return ErrorCode
*/
UINT32 RefCmptPlayer_Init(UINT32 InitMode, UINT32 BitsBufAddr, UINT32 BitsBufSize)
{
    static char RefCmptPlayerMutex[] = "RefCmptPlayerMutex";
    static char RefCmptVideoFlag[] = "RefCmptVideoFlag";
    UINT32 Rval = REFCODE_OK;

    AmbaMisra_TouchUnused(&Rval);

    if (G_RefCmptPlayer_Init == 0U) {
        // Register DSP event
        // Rval = DSP2A(AmbaDSP_EventHandlerRegister(AMBA_DSP_EVENT_VIDEO_DEC_STATUS, RefCmptPlayer_DecStateEventHdlr));
        // if (Rval != REFCODE_OK) {
        //     AmbaPrint_PrintStr5("%s: [ERROR] AmbaDSP_EventHandlerRegister AMBA_DSP_EVENT_VIDEO_DEC_STATUS fail", __func__, NULL, NULL, NULL, NULL);
        // }

        // if (Rval == REFCODE_OK) {
        //     Rval = DSP2A(AmbaDSP_EventHandlerRegister(AMBA_DSP_EVENT_VIDEO_DEC_PIC_INFO, RefCmptPlayer_PicInfoEventCB));
        //     if (Rval != REFCODE_OK) {
        //         AmbaPrint_PrintStr5("%s: [ERROR] AmbaDSP_EventHandlerRegister AMBA_DSP_EVENT_VIDEO_DEC_PIC_INFO fail", __func__, NULL, NULL, NULL, NULL);
        //     }
        // }

        /* Allocate buffer */
        // if (Rval == REFCODE_OK) {
        //     void *RawBuffer, *Buffer;
        //     Rval = RefMemPool_Allocate(REF_MEM_POOL_REGION_CACHE, AMBA_CACHE_LINE_SIZE, &Buffer, &RawBuffer, REF_CMPT_PLAYER_VIDEO_BITS_BUFFER_SIZE);
        //     if (Rval == REFCODE_OK) {
        //         AmbaMisra_TypeCast(&G_RefCmptPlayerInfo.DecBsBufAddr, &Buffer);
        //     } else {
        //         AmbaPrint_PrintStr5("%s: [ERROR] RefMemPool_Allocate fail", __func__, NULL, NULL, NULL, NULL);
        //     }
        // }

        if (Rval == REFCODE_OK) {
            AmbaMisra_TypeCast(&G_RefCmptPlayerInfo.DecBsBufAddr, &BitsBufAddr);
            G_RefCmptPlayerInfo.DecBsBufSize = BitsBufSize;
        }

        if (Rval == REFCODE_OK) {
#define FILESTREAM_INIT_BUFFER_SIZE (8192U)
            static UINT8                FileBuffer[FILESTREAM_INIT_BUFFER_SIZE] GNU_SECTION_NOZEROINIT GNU_ALIGNED_CACHESAFE;
            SVC_FILE_STREAM_INIT_CFG_s  FileInitCfg; /* Will be initialized in AmbaFileStream_GetInitDefaultCfg */

            /* Init file stream */
            Rval = ST2A(SvcFileStream_GetInitDefaultCfg(&FileInitCfg));
            if (Rval == REFCODE_OK) {
                FileInitCfg.MaxHdlr = 16U; // 16U due to the limit MAX_STREAM_NUM in File.c
                Rval = ST2A(SvcFileStream_GetInitBufferSize(FileInitCfg.MaxHdlr, &FileInitCfg.BufferSize));
                if (Rval == REFCODE_OK) {
                    AmbaPrint_PrintUInt5("RefClip_FileStreamInit: BufferSize %u", FileInitCfg.BufferSize, 0, 0, 0, 0);
                    if (FileInitCfg.BufferSize <= sizeof(FileBuffer)) {
                        FileInitCfg.Buffer = FileBuffer;
                        Rval = ST2A(SvcFileStream_Init(&FileInitCfg));
                        if (Rval == REFCODE_OK) {
                            AmbaPrint_PrintStr5("%s> done", __func__, NULL, NULL, NULL, NULL);
                        } else {
                            AmbaPrint_PrintStr5("%s: [ERROR] AmbaFileStream_Init failed", __func__, NULL, NULL, NULL, NULL);
                        }
                    } else {
                        AmbaPrint_PrintUInt5("RefClip_FileStreamInit: [ERROR] File Stream BufferSize %u too large. Limit %u.", FileInitCfg.BufferSize, sizeof(FileBuffer), 0, 0, 0);
                        Rval = REFCODE_GENERAL_ERR;
                    }
                } else {
                    AmbaPrint_PrintStr5("%s: [ERROR] AmbaFileStream_GetRequiredBufferSize failed", __func__, NULL, NULL, NULL, NULL);
                }
            } else {
                AmbaPrint_PrintStr5("%s: [ERROR] AmbaFileStream_GetInitDefaultCfg failed", __func__, NULL, NULL, NULL, NULL);
            }
        }

        /* Create mutex */
        if (Rval == REFCODE_OK) {
            Rval = K2A(AmbaKAL_MutexCreate(&G_RefCmptPlayerInfo.Mutex, RefCmptPlayerMutex));
            if (Rval != REFCODE_OK) {
                AmbaPrint_PrintStr5("%s: [ERROR] AmbaKAL_MutexCreate fail", __func__, NULL, NULL, NULL, NULL);
            }
        }

        /* Create flag */
        if (Rval == REFCODE_OK) {
            Rval = K2A(AmbaKAL_EventFlagCreate(&G_RefCmptPlayerInfo.VideoFlag, RefCmptVideoFlag));
            if (Rval != REFCODE_OK) {
                AmbaPrint_PrintStr5("%s: [ERROR] AmbaKAL_EventFlagCreate fail", __func__, NULL, NULL, NULL, NULL);
            }
        }

        /* Init Demuxer */
        if (Rval == REFCODE_OK) {
            Rval = RefCmptDemuxer_Init();
            if (Rval != REFCODE_OK) {
                AmbaPrint_PrintStr5("%s: [ERROR] RefCmptDemuxer_Init fail", __func__, NULL, NULL, NULL, NULL);
            }
        }

        if (Rval == REFCODE_OK) {
            G_RefCmptPlayerInfo.InitMode = (UINT8)InitMode; // TODO Support display on Vout (InitMode == REF_PLAYER_VIDEO_INIT_MODE_NORMAL)
            G_RefCmptPlayerInfo.VideoDecState = REF_CMPT_PLAYER_STATE_READY;
            G_RefCmptPlayerInfo.VideoDir = 0U; /* 0: Forward */
            G_RefCmptPlayerInfo.DbgPrintDecodeEvent = 0U;
            G_RefCmptPlayerInfo.VideoSpeedIdx = 0U; /* 0: Normal speed */
            G_RefCmptPlayerInfo.VideoStreamIdx = 0U;
            G_RefCmptPlayerInfo.DmxHdlr = NULL;
            G_RefCmptPlayerInfo.IsUserDataSet = 0U;
            G_RefCmptPlayer_Init = 1U;
        }
    } else {
        AmbaPrint_PrintStr5("%s: Already Init", __func__, NULL, NULL, NULL, NULL);
    }
    return Rval;
}

/**
* set pipe num
* @param [in]  PipeNum
* @return ErrorCode
*/
UINT32 RefCmptPlayer_SetPipeNum(UINT8 PipeNum)
{
    UINT32 Rval = REFCODE_OK;
    if (G_RefCmptPlayer_Init == 0U) {
        AmbaPrint_PrintStr5("%s: [ERROR] Not Init", __func__, NULL, NULL, NULL, NULL);
        Rval = REFCODE_GENERAL_ERR;
    }
    if (Rval == REFCODE_OK) {
        Rval = K2A(AmbaKAL_MutexTake(&G_RefCmptPlayerInfo.Mutex, REF_CMPT_PLAYER_MUTEX_TIMEOUT));
        if (Rval == REFCODE_OK) {
            if (G_RefCmptPlayerInfo.PipeCount > 0U) {
                AmbaPrint_PrintStr5("%s: [ERROR] Cannot change PipeNum when pipe created", __func__, NULL, NULL, NULL, NULL);
                Rval = REFCODE_GENERAL_ERR;
            }
            if ((PipeNum == 0U) || (PipeNum > REF_CMPT_PLAYER_MAX_PIPE)) {
                AmbaPrint_PrintUInt5("RefCmptPlayer_Create: [ERROR] Invalid PipeNum %u. Limit %u", PipeNum, REF_CMPT_PLAYER_MAX_PIPE, 0U, 0U, 0U);
                Rval = REFCODE_GENERAL_ERR;
            }

            /* Set config */
            if (Rval == REFCODE_OK) {
                G_RefCmptPlayerInfo.PipeNum = PipeNum;
            }

            if (K2A(AmbaKAL_MutexGive(&G_RefCmptPlayerInfo.Mutex)) != REFCODE_OK) {
                AmbaPrint_PrintStr5("%s: [ERROR] AmbaKAL_MutexGive fail", __func__, NULL, NULL, NULL, NULL);
                Rval = REFCODE_GENERAL_ERR;
            }
        } else {
            AmbaPrint_PrintStr5("%s: [ERROR] AmbaKAL_MutexTake fail", __func__, NULL, NULL, NULL, NULL);
        }
    }
    return Rval;
}

/**
 *  Get free pipe.
 *  @param [in]  PlayerPipeId
 *  @return 0 - REFCODE_OK, others - Error
 */
static UINT32 RefCmptPlayer_GetFreePipe(UINT8 *PlayerPipeId)
{
    UINT32 Rval = REFCODE_GENERAL_ERR;
    UINT8 I;
    /* Mutex should be locked */
    for (I = 0U; I < REF_CMPT_PLAYER_MAX_PIPE; ++I) {
        if (G_RefCmptPlayerInfo.PipeInfo[I].IsCreated == 0U) {
            *PlayerPipeId = I;
            Rval = REFCODE_OK;
            break;
        }
    }
    if (Rval != REFCODE_OK) {
        AmbaPrint_PrintStr5("%s: [ERROR] No free pipe", __func__, NULL, NULL, NULL, NULL);
    }
    return Rval;
}

/**
* create player
* @param [in]  FormatCount
* @param [in]  FileInfo array
* @param [out] PipeId
* @param [in]  pHdlr stream handler
* @return ErrorCode
*/
UINT32 RefCmptPlayer_Create(UINT8 FormatCount, const REF_PLAYER_DEMUXER_FILE_INFO_s FileInfo[AMBA_DEMUXER_MAX_FORMAT_PER_PIPE], UINT8 *PipeId, void **pHdlr)
{
    UINT32 Rval = REFCODE_OK;
    UINT8 PlayerPipeId = 0U;
    UINT32 I;
    const void         *pvHdlr = *pHdlr;
    SVC_STREAM_HDLR_s  *Hdlr;

    AmbaMisra_TouchUnused(pHdlr);
    AmbaMisra_TouchUnused(&Hdlr);

    AmbaMisra_TypeCast(&Hdlr, &pvHdlr);

    if (G_RefCmptPlayer_Init == 0U) {
        AmbaPrint_PrintStr5("%s: [ERROR] Not Init", __func__, NULL, NULL, NULL, NULL);
        Rval = REFCODE_GENERAL_ERR;
    }
    if (Rval == REFCODE_OK) {
        Rval = K2A(AmbaKAL_MutexTake(&G_RefCmptPlayerInfo.Mutex, REF_CMPT_PLAYER_MUTEX_TIMEOUT));
        if (Rval == REFCODE_OK) {
            if (FormatCount > AMBA_DEMUXER_MAX_FORMAT_PER_PIPE) {
                AmbaPrint_PrintUInt5("RefCmptPlayer_Create: [ERROR] Invalid FormatCount %u", FormatCount, 0U, 0U, 0U, 0U);
                Rval = REFCODE_GENERAL_ERR;
            }
            if (G_RefCmptPlayerInfo.PipeCount >= G_RefCmptPlayerInfo.PipeNum) {
                AmbaPrint_PrintUInt5("RefCmptPlayer_Create: [ERROR] No pipe available. Current %u Limit %u", G_RefCmptPlayerInfo.PipeCount, G_RefCmptPlayerInfo.PipeNum, 0U, 0U, 0U);
                Rval = REFCODE_GENERAL_ERR;
            }
            if (G_RefCmptPlayerInfo.IsUserDataSet == 0U) {
                AmbaPrint_PrintStr5("%s: [ERROR] Set GOP first", __func__, NULL, NULL, NULL, NULL);
                Rval = REFCODE_GENERAL_ERR;
            }

            /* Get free pipe */
            if (Rval == REFCODE_OK) {
                Rval = RefCmptPlayer_GetFreePipe(&PlayerPipeId);
                if (Rval == REFCODE_OK) {
                    /* Set FormatInfo */
                    for (I = 0U; I < FormatCount; ++I) {
                        G_RefCmptPlayerInfo.PipeInfo[PlayerPipeId].FormatInfo[I].FileInfo = FileInfo[I];
                    }
                } else {
                    AmbaPrint_PrintStr5("%s: [ERROR] RefCmptPlayer_GetFreePipe failed", __func__, NULL, NULL, NULL, NULL);
                }
            }

            /* Create Demux */
            if (Rval == REFCODE_OK) {
                Rval = RefCmptPlayer_CreatePipe(PlayerPipeId, FormatCount, FileInfo, &Hdlr);
                if (Rval != REFCODE_OK) {
                    AmbaPrint_PrintStr5("%s: [ERROR] RefCmptPlayer_CreatePipe fail", __func__, NULL, NULL, NULL, NULL);
                }
            }

            /* Set config */
            if (Rval == REFCODE_OK) {
                G_RefCmptPlayerInfo.PipeCount++;
                G_RefCmptPlayerInfo.PipeInfo[PlayerPipeId].IsCreated = 1U;
                /* Set return value */
                *PipeId = PlayerPipeId;
            }

            if (K2A(AmbaKAL_MutexGive(&G_RefCmptPlayerInfo.Mutex)) != REFCODE_OK) {
                AmbaPrint_PrintStr5("%s: [ERROR] AmbaKAL_MutexGive fail", __func__, NULL, NULL, NULL, NULL);
                Rval = REFCODE_GENERAL_ERR;
            }
        }
    } else {
        AmbaPrint_PrintStr5("%s: [ERROR] AmbaKAL_MutexTake fail", __func__, NULL, NULL, NULL, NULL);
    }
    return Rval;
}

/**
* delete player
* @param [in]  PipeId
* @return ErrorCode
*/
UINT32 RefCmptPlayer_Delete(UINT8 PipeId)
{
    UINT32 Rval = REFCODE_OK;
    if (G_RefCmptPlayer_Init == 0U) {
        AmbaPrint_PrintStr5("%s: [ERROR] Not Init", __func__, NULL, NULL, NULL, NULL);
        Rval = REFCODE_GENERAL_ERR;
    }
    if (Rval == REFCODE_OK) {
        Rval = K2A(AmbaKAL_MutexTake(&G_RefCmptPlayerInfo.Mutex, REF_CMPT_PLAYER_MUTEX_TIMEOUT));
        if (Rval == REFCODE_OK) {
            if (PipeId >= REF_CMPT_PLAYER_MAX_PIPE) {
                AmbaPrint_PrintUInt5("RefCmptPlayer_Delete: [ERROR] Unexpected PipeId %u", PipeId, 0U, 0U, 0U, 0U);
                Rval = REFCODE_GENERAL_ERR;
            } else {
                if (G_RefCmptPlayerInfo.PipeInfo[PipeId].IsCreated == 0U) {
                    AmbaPrint_PrintStr5("%s: [ERROR] Not created", __func__, NULL, NULL, NULL, NULL);
                    Rval = REFCODE_GENERAL_ERR;
                }
                if (G_RefCmptPlayerInfo.PipeCount == 0U) {
                    AmbaPrint_PrintUInt5("RefCmptPlayer_Delete: [ERROR] Unexpected PipeCount %u", G_RefCmptPlayerInfo.PipeCount, 0U, 0U, 0U, 0U);
                    Rval = REFCODE_GENERAL_ERR;
                }
                /* Delete Demuxer */
                if (Rval == REFCODE_OK) {
                    Rval = RefCmptPlayer_DeletePipe(PipeId);
                    if (Rval != REFCODE_OK) {
                        AmbaPrint_PrintStr5("%s: [ERROR] RefCmptPlayer_DeletePipe fail", __func__, NULL, NULL, NULL, NULL);
                    }
                }
                /* Set config */
                if (Rval == REFCODE_OK) {
                    G_RefCmptPlayerInfo.PipeCount--;
                    G_RefCmptPlayerInfo.PipeInfo[PipeId].IsCreated = 0U;
                }
            }

            if (K2A(AmbaKAL_MutexGive(&G_RefCmptPlayerInfo.Mutex)) != REFCODE_OK) {
                AmbaPrint_PrintStr5("%s: [ERROR] AmbaKAL_MutexGive fail", __func__, NULL, NULL, NULL, NULL);
                Rval = REFCODE_GENERAL_ERR;
            }
        }
    } else {
        AmbaPrint_PrintStr5("%s: [ERROR] AmbaKAL_MutexTake fail", __func__, NULL, NULL, NULL, NULL);
    }
    return Rval;
}

/**
* start player
* @param [in]  PipeId
* @param [in]  StartTime
* @param [in]  Direction
* @param [in]  Speed
* @return ErrorCode
*/
UINT32 RefCmptPlayer_Start(UINT8 PipeId, UINT32 StartTime, UINT8 Direction, UINT32 Speed)
{
    UINT32 Rval = REFCODE_OK;
    if (G_RefCmptPlayer_Init == 0U) {
        AmbaPrint_PrintStr5("%s: [ERROR] Not Init", __func__, NULL, NULL, NULL, NULL);
        Rval = REFCODE_GENERAL_ERR;
    }
    if (Rval == REFCODE_OK) {
        Rval = K2A(AmbaKAL_MutexTake(&G_RefCmptPlayerInfo.Mutex, REF_CMPT_PLAYER_MUTEX_TIMEOUT));
        if (Rval == REFCODE_OK) {
            if (PipeId >= REF_CMPT_PLAYER_MAX_PIPE) {
                AmbaPrint_PrintUInt5("RefCmptPlayer_Start: [ERROR] Unexpected PipeId %u", PipeId, 0U, 0U, 0U, 0U);
                Rval = REFCODE_GENERAL_ERR;
            } else {
                if (G_RefCmptPlayerInfo.PipeInfo[PipeId].IsCreated == 0U) {
                    AmbaPrint_PrintStr5("%s: [ERROR] Not created", __func__, NULL, NULL, NULL, NULL);
                    Rval = REFCODE_GENERAL_ERR;
                }
            }
            if (G_RefCmptPlayerInfo.PipeCount != G_RefCmptPlayerInfo.PipeNum) {
                AmbaPrint_PrintUInt5("RefCmptPlayer_Start: [ERROR] PipeCount %u PipeNum %u", G_RefCmptPlayerInfo.PipeCount, G_RefCmptPlayerInfo.PipeNum, 0U, 0U, 0U);
                Rval = REFCODE_GENERAL_ERR;
            }
            if (StartTime != 0U) {
                AmbaPrint_PrintUInt5("RefCmptPlayer_Start: [ERROR] StartTime %u not supported", StartTime, 0U, 0U, 0U, 0U);
                Rval = REFCODE_GENERAL_ERR;
            }
            if (Speed > 256U) {
                AmbaPrint_PrintUInt5("RefCmptPlayer_Start: [ERROR] Speed %u not supported", Speed, 0U, 0U, 0U, 0U);
                Rval = REFCODE_GENERAL_ERR;
            }
            /* Start Player */
            if (Rval == REFCODE_OK) {
                Rval = RefCmptPlayer_StartPipe(PipeId, StartTime, Direction, Speed);
                if (Rval != REFCODE_OK) {
                    AmbaPrint_PrintStr5("%s: [ERROR] RefCmptPlayer_StartPipe fail", __func__, NULL, NULL, NULL, NULL);
                }
            }

            if (K2A(AmbaKAL_MutexGive(&G_RefCmptPlayerInfo.Mutex)) != REFCODE_OK) {
                AmbaPrint_PrintStr5("%s: [ERROR] AmbaKAL_MutexGive fail", __func__, NULL, NULL, NULL, NULL);
                Rval = REFCODE_GENERAL_ERR;
            }
        }
    } else {
        AmbaPrint_PrintStr5("%s: [ERROR] AmbaKAL_MutexTake fail", __func__, NULL, NULL, NULL, NULL);
    }
    return Rval;
}

/**
* stop player
* @param [in]  PipeId
* @return ErrorCode
*/
UINT32 RefCmptPlayer_Stop(UINT8 PipeId)
{
    UINT32 Rval = REFCODE_OK;
    if (G_RefCmptPlayer_Init == 0U) {
        AmbaPrint_PrintStr5("%s: [ERROR] Not Init", __func__, NULL, NULL, NULL, NULL);
        Rval = REFCODE_GENERAL_ERR;
    }
    if (Rval == REFCODE_OK) {
        Rval = K2A(AmbaKAL_MutexTake(&G_RefCmptPlayerInfo.Mutex, REF_CMPT_PLAYER_MUTEX_TIMEOUT));
        if (Rval == REFCODE_OK) {
            if (PipeId >= REF_CMPT_PLAYER_MAX_PIPE) {
                AmbaPrint_PrintUInt5("RefCmptPlayer_Stop: [ERROR] Unexpected PipeId %u", PipeId, 0U, 0U, 0U, 0U);
                Rval = REFCODE_GENERAL_ERR;
            } else {
                if (G_RefCmptPlayerInfo.PipeInfo[PipeId].IsCreated == 0U) {
                    AmbaPrint_PrintStr5("%s: [ERROR] Not created", __func__, NULL, NULL, NULL, NULL);
                    Rval = REFCODE_GENERAL_ERR;
                }
            }
            if (G_RefCmptPlayerInfo.PipeCount != G_RefCmptPlayerInfo.PipeNum) {
                AmbaPrint_PrintUInt5("RefCmptPlayer_Stop: [ERROR] PipeCount %u PipeNum %u", G_RefCmptPlayerInfo.PipeCount, G_RefCmptPlayerInfo.PipeNum, 0U, 0U, 0U);
                Rval = REFCODE_GENERAL_ERR;
            }
            /* Stop Player */
            if (Rval == REFCODE_OK) {
                Rval = RefCmptPlayer_StopPipe(PipeId, 0U);
                if (Rval != REFCODE_OK) {
                    AmbaPrint_PrintStr5("%s: [ERROR] RefCmptPlayer_StopPipe fail", __func__, NULL, NULL, NULL, NULL);
                }
            }

            if (K2A(AmbaKAL_MutexGive(&G_RefCmptPlayerInfo.Mutex)) != REFCODE_OK) {
                AmbaPrint_PrintStr5("%s: [ERROR] AmbaKAL_MutexGive fail", __func__, NULL, NULL, NULL, NULL);
                Rval = REFCODE_GENERAL_ERR;
            }
        }
    } else {
        AmbaPrint_PrintStr5("%s: [ERROR] AmbaKAL_MutexTake fail", __func__, NULL, NULL, NULL, NULL);
    }
    return Rval;
}

/**
* pause player
* @param [in]  PipeId
* @return ErrorCode
*/
UINT32 RefCmptPlayer_Pause(UINT8 PipeId)
{
    UINT32 Rval = REFCODE_OK;
    if (G_RefCmptPlayer_Init == 0U) {
        AmbaPrint_PrintStr5("%s: [ERROR] Not Init", __func__, NULL, NULL, NULL, NULL);
        Rval = REFCODE_GENERAL_ERR;
    }
    if (Rval == REFCODE_OK) {
        Rval = K2A(AmbaKAL_MutexTake(&G_RefCmptPlayerInfo.Mutex, REF_CMPT_PLAYER_MUTEX_TIMEOUT));
        if (Rval == REFCODE_OK) {
            if (PipeId >= REF_CMPT_PLAYER_MAX_PIPE) {
                AmbaPrint_PrintUInt5("RefCmptPlayer_Pause: [ERROR] Unexpected PipeId %u", PipeId, 0U, 0U, 0U, 0U);
                Rval = REFCODE_GENERAL_ERR;
            } else {
                if (G_RefCmptPlayerInfo.PipeInfo[PipeId].IsCreated == 0U) {
                    AmbaPrint_PrintStr5("%s: [ERROR] Not created", __func__, NULL, NULL, NULL, NULL);
                    Rval = REFCODE_GENERAL_ERR;
                }
            }
            if (G_RefCmptPlayerInfo.PipeCount != G_RefCmptPlayerInfo.PipeNum) {
                AmbaPrint_PrintUInt5("RefCmptPlayer_Pause: [ERROR] PipeCount %u PipeNum %u", G_RefCmptPlayerInfo.PipeCount, G_RefCmptPlayerInfo.PipeNum, 0U, 0U, 0U);
                Rval = REFCODE_GENERAL_ERR;
            }
            /* Pause Player */
            if (Rval == REFCODE_OK) {
                Rval = RefCmptPlayer_PausePipe(PipeId);
                if (Rval != REFCODE_OK) {
                    AmbaPrint_PrintStr5("%s: [ERROR] RefCmptPlayer_PausePipe fail", __func__, NULL, NULL, NULL, NULL);
                }
            }

            if (K2A(AmbaKAL_MutexGive(&G_RefCmptPlayerInfo.Mutex)) != REFCODE_OK) {
                AmbaPrint_PrintStr5("%s: [ERROR] AmbaKAL_MutexGive fail", __func__, NULL, NULL, NULL, NULL);
                Rval = REFCODE_GENERAL_ERR;
            }
        }
    } else {
        AmbaPrint_PrintStr5("%s: [ERROR] AmbaKAL_MutexTake fail", __func__, NULL, NULL, NULL, NULL);
    }
    return Rval;
}

/**
* resume player
* @param [in]  PipeId
* @return ErrorCode
*/
UINT32 RefCmptPlayer_Resume(UINT8 PipeId)
{
    UINT32 Rval = REFCODE_OK;
    if (G_RefCmptPlayer_Init == 0U) {
        AmbaPrint_PrintStr5("%s: [ERROR] Not Init", __func__, NULL, NULL, NULL, NULL);
        Rval = REFCODE_GENERAL_ERR;
    }
    if (Rval == REFCODE_OK) {
        Rval = K2A(AmbaKAL_MutexTake(&G_RefCmptPlayerInfo.Mutex, REF_CMPT_PLAYER_MUTEX_TIMEOUT));
        if (Rval == REFCODE_OK) {
            if (PipeId >= REF_CMPT_PLAYER_MAX_PIPE) {
                AmbaPrint_PrintUInt5("RefCmptPlayer_Resume: [ERROR] Unexpected PipeId %u", PipeId, 0U, 0U, 0U, 0U);
                Rval = REFCODE_GENERAL_ERR;
            } else {
                if (G_RefCmptPlayerInfo.PipeInfo[PipeId].IsCreated == 0U) {
                    AmbaPrint_PrintStr5("%s: [ERROR] Not created", __func__, NULL, NULL, NULL, NULL);
                    Rval = REFCODE_GENERAL_ERR;
                }
            }
            if (G_RefCmptPlayerInfo.PipeCount != G_RefCmptPlayerInfo.PipeNum) {
                AmbaPrint_PrintUInt5("RefCmptPlayer_Resume: [ERROR] PipeCount %u PipeNum %u", G_RefCmptPlayerInfo.PipeCount, G_RefCmptPlayerInfo.PipeNum, 0U, 0U, 0U);
                Rval = REFCODE_GENERAL_ERR;
            }
            /* Resume Player */
            if (Rval == REFCODE_OK) {
                Rval = RefCmptPlayer_ResumePipe(PipeId);
                if (Rval != REFCODE_OK) {
                    AmbaPrint_PrintStr5("%s: [ERROR] RefCmptPlayer_ResumePipe fail", __func__, NULL, NULL, NULL, NULL);
                }
            }

            if (K2A(AmbaKAL_MutexGive(&G_RefCmptPlayerInfo.Mutex)) != REFCODE_OK) {
                AmbaPrint_PrintStr5("%s: [ERROR] AmbaKAL_MutexGive fail", __func__, NULL, NULL, NULL, NULL);
                Rval = REFCODE_GENERAL_ERR;
            }
        }
    } else {
        AmbaPrint_PrintStr5("%s: [ERROR] AmbaKAL_MutexTake fail", __func__, NULL, NULL, NULL, NULL);
    }
    return Rval;
}

/**
* step player
* @param [in]  PipeId
* @return ErrorCode
*/
UINT32 RefCmptPlayer_Step(UINT8 PipeId)
{
    UINT32 Rval = REFCODE_OK;
    if (G_RefCmptPlayer_Init == 0U) {
        AmbaPrint_PrintStr5("%s: [ERROR] Not Init", __func__, NULL, NULL, NULL, NULL);
        Rval = REFCODE_GENERAL_ERR;
    }
    if (Rval == REFCODE_OK) {
        Rval = K2A(AmbaKAL_MutexTake(&G_RefCmptPlayerInfo.Mutex, REF_CMPT_PLAYER_MUTEX_TIMEOUT));
        if (Rval == REFCODE_OK) {
            if (PipeId >= REF_CMPT_PLAYER_MAX_PIPE) {
                AmbaPrint_PrintUInt5("RefCmptPlayer_Step: [ERROR] Unexpected PipeId %u", PipeId, 0U, 0U, 0U, 0U);
                Rval = REFCODE_GENERAL_ERR;
            } else {
                if (G_RefCmptPlayerInfo.PipeInfo[PipeId].IsCreated == 0U) {
                    AmbaPrint_PrintStr5("%s: [ERROR] Not created", __func__, NULL, NULL, NULL, NULL);
                    Rval = REFCODE_GENERAL_ERR;
                }
            }
            if (G_RefCmptPlayerInfo.PipeCount != G_RefCmptPlayerInfo.PipeNum) {
                AmbaPrint_PrintUInt5("RefCmptPlayer_Step: [ERROR] PipeCount %u PipeNum %u", G_RefCmptPlayerInfo.PipeCount, G_RefCmptPlayerInfo.PipeNum, 0U, 0U, 0U);
                Rval = REFCODE_GENERAL_ERR;
            }
            /* Step Player */
            if (Rval == REFCODE_OK) {
                Rval = RefCmptPlayer_StepPipe(PipeId);
                if (Rval != REFCODE_OK) {
                    AmbaPrint_PrintStr5("%s: [ERROR] RefCmptPlayer_StepPipe fail", __func__, NULL, NULL, NULL, NULL);
                }
            }

            if (K2A(AmbaKAL_MutexGive(&G_RefCmptPlayerInfo.Mutex)) != REFCODE_OK) {
                AmbaPrint_PrintStr5("%s: [ERROR] AmbaKAL_MutexGive fail", __func__, NULL, NULL, NULL, NULL);
                Rval = REFCODE_GENERAL_ERR;
            }
        }
    } else {
        AmbaPrint_PrintStr5("%s: [ERROR] AmbaKAL_MutexTake fail", __func__, NULL, NULL, NULL, NULL);
    }
    return Rval;
}

/**
* SetGopConfig
* @param [in]  IdrInterval
* @param [in]  TimeScale
* @param [in]  TimePerFrame
* @param [in]  N
* @param [in]  M
* @return ErrorCode
*/
UINT32 RefCmptPlayer_SetGopConfig(UINT32 IdrInterval, UINT32 TimeScale, UINT32 TimePerFrame, UINT32 N, UINT32 M)
{
    UINT32 Rval = REFCODE_OK;
    if (G_RefCmptPlayer_Init == 0U) {
        AmbaPrint_PrintStr5("%s: [ERROR] Not Init", __func__, NULL, NULL, NULL, NULL);
        Rval = REFCODE_GENERAL_ERR;
    }
    if (Rval == REFCODE_OK) {
        Rval = K2A(AmbaKAL_MutexTake(&G_RefCmptPlayerInfo.Mutex, REF_CMPT_PLAYER_MUTEX_TIMEOUT));
        if (Rval == REFCODE_OK) {
            if (IdrInterval == 0U) {
                AmbaPrint_PrintUInt5("RefCmptPlayer_SetGopConfig: [ERROR] Invalid IdrInterval %u", IdrInterval, 0U, 0U, 0U, 0U);
                Rval = REFCODE_GENERAL_ERR;
            }
            if (TimeScale == 0U) {
                AmbaPrint_PrintUInt5("RefCmptPlayer_SetGopConfig: [ERROR] Invalid TimeScale %u", TimeScale, 0U, 0U, 0U, 0U);
                Rval = REFCODE_GENERAL_ERR;
            }
            if (TimePerFrame == 0U) {
                AmbaPrint_PrintUInt5("RefCmptPlayer_SetGopConfig: [ERROR] Invalid TimePerFrame %u", TimePerFrame, 0U, 0U, 0U, 0U);
                Rval = REFCODE_GENERAL_ERR;
            }
            if (N == 0U) {
                AmbaPrint_PrintUInt5("RefCmptPlayer_SetGopConfig: [ERROR] Invalid N %u", N, 0U, 0U, 0U, 0U);
                Rval = REFCODE_GENERAL_ERR;
            }
            if (M == 0U) {
                AmbaPrint_PrintUInt5("RefCmptPlayer_SetGopConfig: [ERROR] Invalid M %u", M, 0U, 0U, 0U, 0U);
                Rval = REFCODE_GENERAL_ERR;
            }
            if (Rval == REFCODE_OK) {
                G_RefCmptPlayerInfo.UserData.nIdrInterval = IdrInterval;
                G_RefCmptPlayerInfo.UserData.nTimeScale = TimeScale;
                G_RefCmptPlayerInfo.UserData.nTimePerFrame = TimePerFrame;
                G_RefCmptPlayerInfo.UserData.nN = N;
                G_RefCmptPlayerInfo.UserData.nM = M;
                G_RefCmptPlayerInfo.IsUserDataSet = 1U;
            }

            if (K2A(AmbaKAL_MutexGive(&G_RefCmptPlayerInfo.Mutex)) != REFCODE_OK) {
                AmbaPrint_PrintStr5("%s: [ERROR] AmbaKAL_MutexGive fail", __func__, NULL, NULL, NULL, NULL);
                Rval = REFCODE_GENERAL_ERR;
            }
        }
    } else {
        AmbaPrint_PrintStr5("%s: [ERROR] AmbaKAL_MutexTake fail", __func__, NULL, NULL, NULL, NULL);
    }
    return Rval;
}

/**
* DecDebugFlag
* @param [in]  Flag flag
* @return ErrorCode
*/
UINT32 RefCmptPlayer_DecDebugFlag(UINT8 Flag)
{
    UINT32 Rval = REFCODE_OK;
    if (G_RefCmptPlayer_Init == 0U) {
        AmbaPrint_PrintStr5("%s: [ERROR] Not Init", __func__, NULL, NULL, NULL, NULL);
        Rval = REFCODE_GENERAL_ERR;
    }
    if (Rval == REFCODE_OK) {
        Rval = K2A(AmbaKAL_MutexTake(&G_RefCmptPlayerInfo.Mutex, REF_CMPT_PLAYER_MUTEX_TIMEOUT));
        if (Rval == REFCODE_OK) {
            /* Set flag */
            G_RefCmptPlayerInfo.DbgPrintDecodeEvent = Flag;

            if (K2A(AmbaKAL_MutexGive(&G_RefCmptPlayerInfo.Mutex)) != REFCODE_OK) {
                AmbaPrint_PrintStr5("%s: [ERROR] AmbaKAL_MutexGive fail", __func__, NULL, NULL, NULL, NULL);
                Rval = REFCODE_GENERAL_ERR;
            }
        }
    } else {
        AmbaPrint_PrintStr5("%s: [ERROR] AmbaKAL_MutexTake fail", __func__, NULL, NULL, NULL, NULL);
    }
    return Rval;
}

