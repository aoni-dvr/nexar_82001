/**
 *  @file AmbaDSP_MsgDispatcher.h
 *
 * Copyright (c) 2020 Ambarella International LP
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
 *  @details definition of DSP message dispatcher related APIs
 *
 */

#ifndef AMBA_DSP_MSG_DISPATCHER_H
#define AMBA_DSP_MSG_DISPATCHER_H

#include "AmbaDSP_Buffers.h"
#include "AmbaDSP_CommonAPI.h"
#include "AmbaDSP_EventInfo.h"

//FIXME DSP_VPROC_PIN_NUM should be removed
#define DSP_VPROC_PIN_NUM       (4U)

#define MAX_NUM_IDSP_CONFIG     (8U)
#define MAX_ENC_BUF_INFO_NUM    (16U)

#define DSP_DEFAULT_AUDIO_CLK   (12288U)

/*-----------------------------------------------------------------------------------------------*\
 * CAT_ENC (6)
\*-----------------------------------------------------------------------------------------------*/

typedef struct {
   ULONG  InputYAddr;
   ULONG  InputUVAddr;
   UINT16 InputYuvPitch;
   UINT16 Rsvd;
} AMBA_ENC_BUF_INFO_s;

typedef struct {
    UINT8               PrevEncodeState;
    UINT8               Rsvd[3U];
    UINT32              H264Count[DSP_VDSP_ENG_NUM];
    UINT32              H264ConsumedCount[DSP_VDSP_ENG_NUM];
    UINT32              HevcCount[DSP_VDSP_ENG_NUM];
    UINT32              HevcConsumedCount[DSP_VDSP_ENG_NUM];
    UINT32              JpegCount;
    UINT32              JpegConsumedCount;
    UINT64              CapturePtsBase;
    UINT32              CapturePts32;
    UINT64              CapturePts64;
    UINT64              CabacPtsBase;
    UINT32              CabacPts32;
    UINT64              CabacPts64;

    /* enc buffer info */
    UINT16              InfoWCnt;
    UINT16              InfoRCnt;
    AMBA_ENC_BUF_INFO_s BufInfo[MAX_ENC_BUF_INFO_NUM];

    /* AVC MV */
    ULONG               PrevMvAddr;
    DSP_POOL_DESC_s     MvPoolDesc;
    ULONG               MvAddr[MAX_ENC_BUF_INFO_NUM];
} AMBA_DSP_ENC_STRM_INFO_s;

typedef struct {
    AMBA_DSP_ENC_STRM_INFO_s Stream[AMBA_DSP_MAX_STREAM_NUM];
} AMBA_DSP_ENC_CHAN_INFO_s;

/* yuv_chroma_fmt_t */
#define DSP_YUV_MONO                    (0U)
#define DSP_YUV_420                     (1U)
#define DSP_YUV_422                     (2U)
#define DSP_YUV_444                     (3U)

/* Dsp reported Prof status (DSP_PROF_IDLE) */
#define DSP_REPORTED_PROF_INVALID       (0U)
#define DSP_REPORTED_PROF_CAMERA        (1U)
#define DSP_REPORTED_PROF_SAFETY        (253U)
#define DSP_REPORTED_PROF_2_IDLE        (254U)
#define DSP_REPORTED_PROF_IDLE          (255U)

/* SSP based Prof status */
#define DSP_PROF_STATUS_INVALID             (0U)
#define DSP_PROF_STATUS_CAMERA              (1U)
#define DSP_PROF_STATUS_PLAYBACK            (2U)
#define DSP_PROF_STATUS_YUV_INPUT           (3U)
#define DSP_PROF_STATUS_XCODE               (4U)
#define DSP_PROF_STATUS_DUPLEX              (5U)
#define DSP_PROF_STATUS_2_IDLE              (6U)
#define DSP_PROF_STATUS_IDLE                (7U)
#define DSP_PROF_STATUS_2_CAMERA            (8U)
#define DSP_PROF_STATUS_2_YUV_INPUT         (9U)
#define DSP_PROF_STATUS_2_PLAYBACK          (10U)
#define DSP_PROF_STATUS_SAFETY              (11U)
#define DSP_PROF_STATUS_NUM                 (12U)

/* DSP reported Vproc status */
#define DSP_REPORTED_VPROC_STATUS_INVALID   (0U)
#define DSP_REPORTED_VPROC_STATUS_TIMER     (10U)
#define DSP_REPORTED_VPROC_STATUS_ACTIVE    (20U)

/* SSP defined Vproc status */
#define DSP_VPROC_STATUS_INVALID            (0U)
#define DSP_VPROC_STATUS_TIMER              (1U)
#define DSP_VPROC_STATUS_ACTIVE             (2U)
//#define DSP_VPROC_STATUS_RUN2IDLE           (3U)
#define DSP_VPROC_STATUS_IDLE2RUN           (4U)
//#define DSP_VPROC_STATUS_INVALID2IDLE       (5U)
//#define DSP_VPROC_STATUS_INVALID2RUN        (6U)

#define DSP_VPROC_OUT_STRM_MAIN                 (0U)
#define DSP_VPROC_OUT_STRM_PREV_A               (1U) // or said Pin1
#define DSP_VPROC_OUT_STRM_PREV_B               (2U) // or said Pin2
#define DSP_VPROC_OUT_STRM_PREV_C               (3U) // or said Pin3
#define DSP_VPROC_OUT_STRM_PREV_LNDT            (1U)
#define DSP_VPROC_OUT_STRM_PREV_HIER            (3U)
#define NUM_DSP_VPROC_OUT_STRM                  (4U)

// VprocPin, order from smallest to largest
#define DSP_VPROC_PIN_PREVC     (0U)
#define DSP_VPROC_PIN_PREVA     (1U)
#define DSP_VPROC_PIN_PREVB     (2U)
#define DSP_VPROC_PIN_MAIN      (3U)
//FIXME DSP_VPROC_PIN_NUM should be declared here
//#define DSP_VPROC_PIN_NUM       (4U)

#define DSP_VPROC_PIN_NOT_SYNC_TO_ENC   (0U)
#define DSP_VPROC_PIN_A_SYNC_TO_ENC     (1U)
#define DSP_VPROC_PIN_B_SYNC_TO_ENC     (2U)
#define DSP_VPROC_PIN_C_SYNC_TO_ENC     (3U)

/* Raw-capture status */
#define DSP_RAW_CAP_STATUS_INVALID      (0U)
#define DSP_RAW_CAP_STATUS_TIMER        (10U)
#define DSP_RAW_CAP_STATUS_VIDEO        (20U)

#define VIN_INPUT_MODE_RGGB     (0U)
#define VIN_INPUT_MODE_RGBIR    (1U)
#define VIN_INPUT_MODE_RCCB     (2U)
#define VIN_INPUT_MODE_YUV      (32U)

/* SSP defined encode status */
#define DSP_ENC_STATUS_IDLE                 (0U)
#define DSP_ENC_STATUS_BUSY                 (1U)
#ifdef SUPPORT_ENCSTATUS_RESET
#define DSP_ENC_STATUS_RESET_INFO           (2U)
#endif

/* Enc format, enc_coding_type_t */
#define DSP_ENC_FMT_UNKNOWN     (0U)
#define DSP_ENC_FMT_H264        (1U)
#define DSP_ENC_FMT_H265        (2U)
#define DSP_ENC_FMT_JPEG        (3U)
#define DSP_ENC_FMT_NUM         (4U)

/* SSP defined decode status */
#define DSP_DEC_OPM_INVALID                 (0U)
#define DSP_DEC_OPM_IDLE                    (1U)
#define DSP_DEC_OPM_RUN                     (2U)
#define DSP_DEC_OPM_VDEC_IDLE               (3U)
#define DSP_DEC_OPM_RUN_2_IDLE              (4U)
#define DSP_DEC_OPM_RUN_2_VDEC_IDLE         (5U)
#define DSP_DEC_OPM_FLUSHING                (6U)
#define DSP_DEC_OPM_FREEZE                  (7U)
#define DSP_DEC_OPM_RUN_2_FREEZE            (8U)

#define DSP_DEC_ERR_LVL_NONE                (0U)
#define DSP_DEC_ERR_LVL_WARNING             (1U)
#define DSP_DEC_ERR_LVL_RECOVERABLE         (2U)
#define DSP_DEC_ERR_LVL_FATAL               (3U)

/* SSP defined vout status */
#define DSP_VOUT_LOCK_STATUS_OFF            (0U)
#define DSP_VOUT_LOCK_STATUS_ON             (1U)

typedef struct {
    UINT32      ProfCfgID;
    UINT8       ProfStatus;
    UINT8       RawCapStatus[DSP_VIN_MAX_NUM];
    UINT8       VprocStatus[AMBA_DSP_MAX_VIEWZONE_NUM];
    UINT8       EncStatus[AMBA_DSP_MAX_STREAM_NUM];
    UINT8       VoutLockStatus[NUM_VOUT_IDX];
    UINT8       DecStatus[AMBA_DSP_MAX_DEC_STREAM_NUM];
    UINT32      DspWorkArea;
    UINT32      DspWorkAreaSize;
    UINT32      JpegCnt;
} DSP_STATUS_s;

/*-----------------------------------------------------------------------------------------------*\
 * CAT_DPROC (8)
\*-----------------------------------------------------------------------------------------------*/
#define DSP_DPROC_STOP_NODE_NORMAL 0U
#define DSP_DPROC_STOP_MODE_EOSTRM 1U
#define DSP_DPROC_STOP_MODE_PAUSE  2U

/*-----------------------------------------------------------------------------------------------*\
 * CAT_POSTPROC (10)
\*-----------------------------------------------------------------------------------------------*/
#define DSP_PPVOUT_WIN_UPD_NONE 0U
#define DSP_PPVOUT_WIN_UPD_NEXT 1U
#define DSP_PPVOUT_WIN_UPD_CURR 2U

                                               //vout_0    vout_1
#define DSP_PPVOUT_STATE_NON_EXIST   0U        //off       off
#define DSP_PPVOUT_STATE_EXIST       1U        //on        off

/* ----------------------------------------------------------------------------
 *  DSP Event Flag related
 ---------------------------------------------------------------------------- */
#define AMBA_DSP_POSTP_VOUT_EXIST_FLAG_PATTERN   (1U)   //Internal

/* HIER_TIER */
#define DSP_HIER_0              (0U)
#define DSP_HIER_1              (1U)
#define DSP_HIER_2              (2U)
#define DSP_HIER_3              (3U)
#define DSP_HIER_4              (4U)
#define DSP_HIER_5              (5U)
#define DSP_HIER_6              (6U)
#define DSP_HIER_NUM            (13U) //inherent from CV1

extern const UINT16 DSP_VprocPinVprocMemTypeMap[DSP_VPROC_PIN_NUM];

/**
 * Wait VDSP event with timeout
 * @param [in] DspEventFlag Target DSP event flag that is waiting for
 * @param [in] WaitCount Number of specific event
 * @param [in] Timeout Timeout in msec
 * @return 0 - OK, others - ErrorCode
 */
extern UINT32 DSP_WaitVdspEvent(UINT32 DspEventFlag, UINT32 WaitCount, UINT32 Timeout);

/**
 * Wait DSP profile state changed with timeout
 * @param [in] ProfState Target DSP profile state that is waiting for
 * @param [in] TimeOutMs Timeout in msec
 * @return 0 - OK, others - ErrorCode
 */
extern UINT32 DSP_WaitProfState(UINT8 ProfState, UINT32 TimeOutMs);

/**
 * Wait DSP Vin state changed with timeout
 * @param [in] ProfState Target DSP Vin state that is waiting for
 * @param [in] TimeOutMs Timeout in msec
 * @return 0 - OK, others - ErrorCode
 */
extern UINT32 DSP_WaitVinState(UINT8 VinId, UINT8 VinState, UINT32 TimeOutMs);

/**
 * Wait DSP Vproc state changed with timeout
 * @param [in] ChId Target channel ID
 * @param [in] VprocState Target DSP Vproc state that is waiting for
 * @param [in] TimeOutMs Timeout in msec
 * @param [in] NotClearFlag 1-not clean, 0-clean flag before waiting for target DSP Vproc state
 * @return 0 - OK, others - ErrorCode
 */
extern UINT32 DSP_WaitVprocState(UINT8 ChId, UINT8 VprocState, UINT32 TimeOutMs, UINT8 NotClearFlag);

/**
 * Wait DSP Encoder state
 * @param [in] ChId Target channel ID
 * @param [in] EncState Target DSP Encoder state that is waiting for
 * @param [in] TimeOutMs Timeout in msec
 * @return 0 - OK, others - ErrorCode
 */
extern UINT32 DSP_WaitEncState(UINT32 ChId, UINT8 EncState, UINT32 TimeOutMs);

/**
 * Wait vout lock status
 * @param [in] VoutId vout index
 * @param [in] LockState Target Lock state to be wait
 * @param [in] TimeOutMs Timeout in msec
 * @return 0 - OK, others - ErrorCode
 */
extern UINT32 DSP_WaitVoutLock(UINT8 VoutId, UINT8 LockState, UINT32 TimeOutMs);

/**
 * Wait DSP Decoder state
 * @param [in] ChId Target channel ID
 * @param [in] DecState Target DSP Decoder state that is waiting for
 * @param [in] TimeOutMs Timeout in msec
 * @return 0 - OK, others - ErrorCode
 */
extern UINT32 DSP_WaitDecState(UINT32 ChId, UINT8 DecState, UINT32 TimeOutMs, UINT8 NotClearFlag);

/**
 * Set DSP profile state
 * @param [in] Status Profile state
 * @return Null
 */
extern void DSP_SetProfState(UINT8 Status);

/**
 * Get DSP profile state
 * @return DSP current profile state
 */
extern UINT8 DSP_GetProfState(void);

/**
 * Get DSP Vin state
 * @param [in] VinId Vin ID
 * @return DSP current Vin state
 */
extern UINT8 DSP_GetVinState(UINT8 VinId);

/**
 * Get DSP Vproc state
 * @param [in] ChId Channel ID
 * @return DSP current Vproc state
 */
extern UINT8 DSP_GetVprocState(UINT8 ChId);

/**
 * Get DSP encoder state
 * @param [in] ChId Channel ID
 * @return DSP current encoder state
 */
extern UINT8 DSP_GetEncState(UINT16 ChId);

/**
 * Get DSP decoder state
 * @param [in] ChId Channel ID
 * @return DSP current decoder state
 */
extern UINT8 DSP_GetDecState(UINT16 ChId);

/**
 * Message parser
 * @param [in] pStatusMsg Message from DSP
 * @return 0 - OK, others - ErrorCode
 */
extern UINT32 LL_MsgParser(const AMBA_DSP_MSG_STATUS_s *pStatusMsg);

/**
 * Wait VCAP event for the soecific VinIdx
 * @param [in] NumVin The number of VinId
 * @param [in] pVinIdx The target VinIdx of VCAP event that is waiting for
 * @param [in] WaitCount Number of specific event
 * @param [in] Timeout Timeout in msec
 * @return 0 - OK, others - ErrorCode
 */
extern UINT32 DSP_WaitVcapEvent(const UINT16 NumVin, const UINT16 *pVinIdx, UINT8 WaitCount, UINT32 Timeout);

extern void LL_SetMsgDispatcherBitsInfo(UINT16 EngId, ULONG BitsBase, UINT32 BitsSize);
extern void LL_SetMsgDispatcherDecoderUsingStatusEos(UINT8 Enable);

extern void LL_PrintBatchInfoId(UINT32 BatchInfoId);

#endif
