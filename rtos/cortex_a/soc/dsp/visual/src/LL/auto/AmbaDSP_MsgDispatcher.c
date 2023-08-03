/**
 *  @file AmbaDSP_MsgDispatcher.c
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
 *  @details Implementation of DSP message dispatcher related APIs
 *
 */

#include "AmbaDSP_Log.h"
#include "AmbaDSP_EventCtrl.h"
#include "AmbaDSP_MsgDispatcher.h"
#include "AmbaDSP_Image3aStatistics.h"
#include "AmbaDSP_Ctrl.h"

/* ----------------------------------------------------------------------------
 *  Decode related
 ---------------------------------------------------------------------------- */
static UINT32 PpEosEent = 0U;
static UINT32 UseDecEos = 0U;
static UINT64 DspMsgDisplayPTS = 0U;

typedef struct {
    UINT32 PrevVprocStatCapSeqNumInit:1;
    UINT32 PrevAAAStatCapSeqNumInit:1;
    UINT32 PrevExtStatCapSeqNumInit:1;
    UINT32 PrevExtDataCapSeqNumInit:1;
    UINT32 rsvd:28;
} DSP_MSG_INIT_s;

static UINT32 DSP_PROF_StatePattern[DSP_PROF_STATUS_NUM] = {
    [DSP_PROF_STATUS_INVALID     ] = 1U,// << DSP_PROF_STATUS_INVALID,
    [DSP_PROF_STATUS_CAMERA      ] = 1U << DSP_PROF_STATUS_CAMERA,
    [DSP_PROF_STATUS_PLAYBACK    ] = 1U << DSP_PROF_STATUS_PLAYBACK,
    [DSP_PROF_STATUS_YUV_INPUT   ] = 1U << DSP_PROF_STATUS_YUV_INPUT,
    [DSP_PROF_STATUS_XCODE       ] = 1U << DSP_PROF_STATUS_XCODE,
    [DSP_PROF_STATUS_DUPLEX      ] = 1U << DSP_PROF_STATUS_DUPLEX,
    [DSP_PROF_STATUS_2_IDLE      ] = 1U << DSP_PROF_STATUS_2_IDLE,
    [DSP_PROF_STATUS_IDLE        ] = 1U << DSP_PROF_STATUS_IDLE,
    [DSP_PROF_STATUS_SAFETY      ] = 1U << DSP_PROF_STATUS_SAFETY,
};

static UINT32 DSP_VPROC_StatePattern[DSP_VPROC_STATUS_ACTIVE + 1U] = {
    [DSP_VPROC_STATUS_INVALID           ] = 1U,// << DSP_VPROC_STATUS_INVALID,
    [DSP_VPROC_STATUS_TIMER             ] = 1U << DSP_VPROC_STATUS_TIMER,
    [DSP_VPROC_STATUS_ACTIVE            ] = 1U << DSP_VPROC_STATUS_ACTIVE,
};

static UINT32 DSP_Enc_StatePattern[DSP_ENC_STATUS_BUSY + 1U] = {
    [DSP_ENC_STATUS_IDLE                    ] = 1U,// << DSP_ENC_STATUS_IDLE,
    [DSP_ENC_STATUS_BUSY                    ] = 1U << DSP_ENC_STATUS_BUSY,
};

static UINT32 DSP_Vout_StatePattern[DSP_VOUT_LOCK_STATUS_ON + 1U] = {
    [DSP_VOUT_LOCK_STATUS_OFF                    ] = 1U,// << DSP_ENC_STATUS_IDLE,
    [DSP_VOUT_LOCK_STATUS_ON                     ] = 1U << DSP_VOUT_LOCK_STATUS_ON,
};

static UINT32 DSP_Dec_StatePattern[DSP_DEC_OPM_RUN_2_FREEZE + 1U] = {
    [DSP_DEC_OPM_INVALID                    ] = 1U,// << DSP_DEC_OPM_INVALID,
    [DSP_DEC_OPM_IDLE                       ] = 1U << DSP_DEC_OPM_IDLE,
    [DSP_DEC_OPM_RUN                        ] = 1U << DSP_DEC_OPM_RUN,
    [DSP_DEC_OPM_VDEC_IDLE                  ] = 1U << DSP_DEC_OPM_VDEC_IDLE,
    [DSP_DEC_OPM_RUN_2_IDLE                 ] = 1U << DSP_DEC_OPM_RUN_2_IDLE,
    [DSP_DEC_OPM_RUN_2_VDEC_IDLE            ] = 1U << DSP_DEC_OPM_RUN_2_VDEC_IDLE,
    [DSP_DEC_OPM_FLUSHING                   ] = 1U << DSP_DEC_OPM_FLUSHING,
    [DSP_DEC_OPM_FREEZE                     ] = 1U << DSP_DEC_OPM_FREEZE,
    [DSP_DEC_OPM_RUN_2_FREEZE               ] = 1U << DSP_DEC_OPM_RUN_2_FREEZE,
};

const UINT16 DSP_VprocPinVprocMemTypeMap[DSP_VPROC_PIN_NUM] = {
    [DSP_VPROC_PIN_PREVC]   = VPROC_EXT_MEM_TYPE_PREV_C,
    [DSP_VPROC_PIN_PREVA]   = VPROC_EXT_MEM_TYPE_PREV_A,
    [DSP_VPROC_PIN_PREVB]   = VPROC_EXT_MEM_TYPE_PREV_B,
    [DSP_VPROC_PIN_MAIN]    = VPROC_EXT_MEM_TYPE_MCTS,      //Actual Main is MctsLumaOut+MctfChroma
};

static DSP_STATUS_s DspStatus = {0};

/* AAA related parameters */
static UINT8 ValidVinHist[AMBA_DSP_MAX_VIEWZONE_NUM][DSP_VIN_HIST_NUM] GNU_SECTION_NOZEROINIT;
static AMBA_DSP_EVENT_3A_TRANSFER_s CfaTransInfo[AMBA_DSP_MAX_VIEWZONE_NUM] GNU_SECTION_NOZEROINIT;
static AMBA_DSP_EVENT_3A_TRANSFER_s PgTransInfo[AMBA_DSP_MAX_VIEWZONE_NUM] GNU_SECTION_NOZEROINIT;
static AMBA_DSP_EVENT_3A_TRANSFER_s CfaHistTransInfo[AMBA_DSP_MAX_VIEWZONE_NUM][DSP_VIN_HIST_NUM] GNU_SECTION_NOZEROINIT;
static UINT32 AAADataValidPattern[AMBA_DSP_MAX_VIEWZONE_NUM] = {0}; //reference
static UINT32 AAADataValidData[AMBA_DSP_MAX_VIEWZONE_NUM] = {0};

// TBM to ContextUtility
static AMBA_DSP_ENC_CHAN_INFO_s EncBitsInfo GNU_SECTION_NOZEROINIT;
static UINT32 AmbaVideoBitsDescIdx = 0U;

/**
 * Update vproc external data information
 * @param [in] MsgCode Message code
 * @param [in] pMsgInfo Pointer to the message info
 * @return 0 - OK, others - ErrorCode
 */
static UINT32 UpdateVprocExtDataInfo(UINT32 MsgCode, const void *pMsgInfo);

/**
 * Handle encoder bitstream information
 * @param [in] pEncStatus Pointer to the DSP message about encoding status
 * @return 0 - OK, others - ErrorCode
 */
static UINT32 HandlerEncStatus(const enc_status_msg_t *pEncStatus);

/**
 * Parse DSP status messages
 * @param [in] pMsgInfo Pointer to the message info
 * @return 0 - OK, others - ErrorCode
 */
static UINT32 MsgParserDSPStatus(const void *pMsgInfo);

/**
 * Parse Vproc messages
 * @param [in] pMsgInfo Pointer to the message info
 * @return 0 - OK, others - ErrorCode
 */
static UINT32 MsgParserVproc(const void *pMsgInfo);

/**
 * Parse Vin messages
 * @param [in] pMsgInfo Pointer to the message info
 * @return 0 - OK, others - ErrorCode
 */
static UINT32 MsgParserVin(const void *pMsgInfo);

/**
 * Parse Vout messages
 * @param [in] pMsgInfo Pointer to the message info
 * @return 0 - OK, others - ErrorCode
 */
static UINT32 MsgParserVout(const void *pMsgInfo);

/**
 * Parse Encoder messages
 * @param [in] pMsgInfo Pointer to the message info
 * @return 0 - OK, others - ErrorCode
 */
static UINT32 MsgParserEnc(const void *pMsgInfo);

/**
 * Parse Decoder messages
 * @param [in] pMsgInfo Pointer to the message info
 * @return 0 - OK, others - ErrorCode
 */
static UINT32 MsgParserDec(const void *pMsgInfo);

/**
 * Parse DecProc messages
 * @param [in] pMsgInfo Pointer to the message info
 * @return 0 - OK, others - ErrorCode
 */
static UINT32 MsgParserDecProc(const void *pMsgInfo);

/**
 * Parse post process messages
 * @param [in] pMsgInfo Pointer to the message info
 * @return 0 - OK, others - ErrorCode
 */
static UINT32 MsgParserPostProc(const void *pMsgInfo);

/**
 * Parse none implement category messages
 * @param [in] pMsgInfo Pointer to the message info
 * @return 0 - OK, others - ErrorCode
 */
static UINT32 MsgParserNoneImpl(const void *pMsgInfo);

static inline UINT32 GiveRawEventOnParseVin(const msg_vin_sensor_raw_capture_status_t * pVinRawCapStatus)
{
    UINT32 Rval;
    DSP_RAW_CAPTURED_BUF_s *pVinCapInfo = NULL;
    AMBA_DSP_RAW_BUF_s *pRawBuffer = NULL;
    AMBA_DSP_BUF_s *pAuxBuffer = NULL;
    AMBA_DSP_VIDEO_PATH_INFO_s *pVidPathInfo = NULL;
    ULONG VinCapInfoAddr = 0U, VidPathInfoAddr = 0U, StlCapInfoAddr = 0U;
    UINT16 VinCapInfoIdx = 0U, VidPathInfoIdx = 0U, StlCapInfoAIdx = 0U;
    ULONG RawBufBaseAddr = 0UL, AuxBufferBaseAddr = 0UL;

    Rval = DSP_RequestEventInfoBuf(EVENT_INFO_POOL_PROT_BUF, &VinCapInfoAddr, &VinCapInfoIdx, LL_EVENT_LV_RAW_RDY);
    LL_PrintErrLine(Rval, __func__, __LINE__);

    if (Rval == OK) {
        Rval = DSP_RequestEventInfoBuf(EVENT_INFO_POOL_PROT_BUF, &VidPathInfoAddr, &VidPathInfoIdx, LL_EVENT_VIDEO_PATH_STATUS);
        LL_PrintErrLine(Rval, __func__, __LINE__);

        if (Rval == OK) {
            dsp_osal_typecast(&pVinCapInfo, &VinCapInfoAddr);
            Rval = dsp_osal_memset(pVinCapInfo, 0, sizeof(DSP_RAW_CAPTURED_BUF_s));
            LL_PrintErrLine(Rval, __func__, __LINE__);

            pRawBuffer = &pVinCapInfo->RawDataRdy.RawBuffer;
            pAuxBuffer = &pVinCapInfo->RawDataRdy.AuxBuffer;

            // Default raw
            if ((pVinRawCapStatus->is_capture_time_out == 1U) &&
                (pVinRawCapStatus->rpt_frm_cntrl == VIN_RPT_FRM_USE_DEF) &&
                (pVinRawCapStatus->raw_repeat_default_buf_addr != 0U)) {
                Rval = dsp_osal_cli2virt(pVinRawCapStatus->raw_repeat_default_buf_addr, &pRawBuffer->BaseAddr);
                LL_PrintErrLine(Rval, __func__, __LINE__);
                Rval = dsp_osal_cli2virt(pVinRawCapStatus->ce_repeat_default_buf_addr, &pAuxBuffer->BaseAddr);
                LL_PrintErrLine(Rval, __func__, __LINE__);
            } else {
                Rval = dsp_osal_cli2virt(pVinRawCapStatus->raw_cap_buf_addr, &pRawBuffer->BaseAddr);
                LL_PrintErrLine(Rval, __func__, __LINE__);
                Rval = dsp_osal_cli2virt(pVinRawCapStatus->ce_cap_buf_addr, &pAuxBuffer->BaseAddr);
                LL_PrintErrLine(Rval, __func__, __LINE__);
            }

            // pRawBuffer->BaseAddr will be updated to physical address after give LV_RAW_RDY
            // So keep the virtual address used to provide the STL_RAW_RDY event later
            RawBufBaseAddr = pRawBuffer->BaseAddr;
            pRawBuffer->Pitch            = pVinRawCapStatus->raw_cap_buf_pitch;
            pRawBuffer->Window.Width     = pVinRawCapStatus->raw_cap_buf_width;
            pRawBuffer->Window.Height    = pVinRawCapStatus->raw_cap_buf_height;
            pRawBuffer->Window.OffsetX   = 0U;
            pRawBuffer->Window.OffsetY   = 0U;
            pRawBuffer->Compressed       = 0U;

            // pAuxBuffer->BaseAddr will be updated to physical address after give LV_RAW_RDY
            // So keep the virtual address used to provide the STL_RAW_RDY event later
            AuxBufferBaseAddr = pAuxBuffer->BaseAddr;
            pAuxBuffer->Pitch            = pVinRawCapStatus->ce_cap_buf_pitch;
            pAuxBuffer->Window.Width     = pVinRawCapStatus->ce_cap_buf_width;
            pAuxBuffer->Window.Height    = pVinRawCapStatus->ce_cap_buf_height;
            pAuxBuffer->Window.OffsetX   = 0U;
            pAuxBuffer->Window.OffsetY   = 0U;

            pVinCapInfo->RawDataRdy.VinId   = (UINT8)pVinRawCapStatus->vin_id;
            pVinCapInfo->CapSeqU32          = pVinRawCapStatus->raw_cap_cnt;
            pVinCapInfo->CapPtsU32          = pVinRawCapStatus->pts;
            pVinCapInfo->TargetVinId = (UINT16)pVinRawCapStatus->temporal_demux_tar_vin_id;
            pVinCapInfo->CapSliceIdx = (UINT8)pVinRawCapStatus->current_cap_slice_idx;
            pVinCapInfo->CapSliceNum = (UINT8)pVinRawCapStatus->cap_slice_num;
            pVinCapInfo->RawDataRdy.RawBufferNumFree = pVinRawCapStatus->raw_cap_buf_num_free;

            dsp_osal_typecast(&pVidPathInfo, &VidPathInfoAddr);
            Rval = dsp_osal_memset(pVidPathInfo, 0, sizeof(AMBA_DSP_VIDEO_PATH_INFO_s));
            LL_PrintErrLine(Rval, __func__, __LINE__);

            if (pVinRawCapStatus->is_capture_time_out > 0U) {
                pVidPathInfo->Status = VID_PATH_STATUS_VIN_TIMEOUT;
                pVidPathInfo->Data = pVinRawCapStatus->vin_id;
                DSP_SetU8Bit(&pVinCapInfo->RawDataRdy.IsVirtChan, 2U/*B[2] as DefaultRaw indicator */);
            } else {
                pVidPathInfo->Status = VID_PATH_STATUS_NONE;
                pVidPathInfo->Data = pVinRawCapStatus->vin_id;
            }
#ifdef SUPPORT_DSP_VIN_SHRTFRM_DROP
            if (pVinRawCapStatus->rpt_frm_cntrl == VIN_RPT_FRM_DROP) {
                DSP_SetU8Bit(&pVinCapInfo->RawDataRdy.IsVirtChan, 4U/*B[4] as raw drop indicator */);
            }
#endif
            Rval = DSP_GiveEvent(LL_EVENT_LV_RAW_RDY, pVinCapInfo, VinCapInfoIdx);
            if (Rval != OK) {
                AmbaLL_LogUInt5("[Err] Msg code (0x%X) GiveEvent Error %u", MSG_VIN_SENSOR_RAW_CAPTURE_STATUS, __LINE__, 0U, 0U, 0U);
            }
            Rval = DSP_GiveEvent(LL_EVENT_VIDEO_PATH_STATUS, pVidPathInfo, VidPathInfoIdx);
            if (Rval != OK) {
                AmbaLL_LogUInt5("[Err] Msg code (0x%X) GiveEvent Error %u", MSG_VIN_SENSOR_RAW_CAPTURE_STATUS, __LINE__, 0U, 0U, 0U);
            }

            if ((pVinRawCapStatus->is_external_raw_buf_from_system > 0U) ||
                (pVinRawCapStatus->is_external_ce_buf_from_system > 0U)) {
                DSP_STL_RAW_CAP_BUF_s *pStlCapInfo = NULL;
                AMBA_DSP_STL_RAW_DATA_INFO_s *pStlRawData= NULL;

                Rval = DSP_RequestEventInfoBuf(EVENT_INFO_POOL_PROT_BUF, &StlCapInfoAddr, &StlCapInfoAIdx, LL_EVENT_STL_RAW_RDY);
                if (Rval == OK) {
                    dsp_osal_typecast(&pStlCapInfo, &StlCapInfoAddr);
                    Rval = dsp_osal_memset(pStlCapInfo, 0, sizeof(DSP_STL_RAW_CAP_BUF_s));
                    LL_PrintErrLine(Rval, __func__, __LINE__);
                    pStlRawData = &pStlCapInfo->StlRawData;

                    if (pVinRawCapStatus->is_external_raw_buf_from_system > 0U) {
                        pStlRawData->BufFormat = RAW_FORMAT_MEM_RAW;
                        pStlRawData->Buf.BaseAddr = RawBufBaseAddr;
                        pStlRawData->Buf.Pitch = pRawBuffer->Pitch;
                        Rval = dsp_osal_memcpy(&pStlRawData->Buf.Window, &pRawBuffer->Window, sizeof(AMBA_DSP_WINDOW_s));
                        LL_PrintErrLine(Rval, __func__, __LINE__);
                    } else {
                        pStlRawData->BufFormat = (UINT8)0U;
                        pStlRawData->Buf.BaseAddr = 0U;
                        pStlRawData->Buf.Pitch = (UINT16)0U;
                        Rval = dsp_osal_memset(&pStlRawData->Buf.Window, 0, sizeof(AMBA_DSP_WINDOW_s));
                        LL_PrintErrLine(Rval, __func__, __LINE__);
                    }
                    if (pVinRawCapStatus->is_external_ce_buf_from_system > 0U) {
                        pStlRawData->AuxBufFormat = RAW_FORMAT_MEM_RAW;
                        pStlRawData->AuxBuf.BaseAddr = AuxBufferBaseAddr;
                        pStlRawData->AuxBuf.Pitch = pAuxBuffer->Pitch;
                        Rval = dsp_osal_memcpy(&pStlRawData->AuxBuf.Window, &pAuxBuffer->Window, sizeof(AMBA_DSP_WINDOW_s));
                        LL_PrintErrLine(Rval, __func__, __LINE__);
                    } else {
                        pStlRawData->AuxBufFormat = (UINT8)0U;
                        pStlRawData->AuxBuf.BaseAddr = 0U;
                        pStlRawData->AuxBuf.Pitch = (UINT16)0U;
                        Rval = dsp_osal_memset(&pStlRawData->AuxBuf.Window, 0, sizeof(AMBA_DSP_WINDOW_s));
                        LL_PrintErrLine(Rval, __func__, __LINE__);
                    }
                    pStlCapInfo->VinId = pVinRawCapStatus->vin_id;
                    pStlCapInfo->CapSrcType = (UINT8)CAP_SRC_TYPE_VIN;
                    pStlRawData->CapPts = (UINT64)pVinRawCapStatus->pts;
                    pStlRawData->CapSequence = (UINT64)pVinRawCapStatus->raw_cap_cnt;
                    Rval = DSP_GiveEvent(LL_EVENT_STL_RAW_RDY, pStlCapInfo, StlCapInfoAIdx);
                    if (Rval != OK) {
                        AmbaLL_LogUInt5("[Err] Msg code (0x%X) GiveEventError %u", MSG_VIN_SENSOR_RAW_CAPTURE_STATUS, __LINE__, 0U, 0U, 0U);
                    }
                }
            }
        } else {
            /* EventInfoAddr need to be released
             * If you request EventInfoAddr but don't call GiveEvent */
            Rval = DSP_ReleaseEventInfoBuf(VinCapInfoIdx);
            LL_PrintErrLine(Rval, __func__, __LINE__);
        }
    }

    return Rval;
}

static inline void UpdateVprocStatusOnParseVproc(UINT16 ChannelId, UINT32 UpdatedStatus)
{
    UINT8 VprocStatus = DSP_VPROC_STATUS_INVALID;

    if (UpdatedStatus == DSP_REPORTED_VPROC_STATUS_INVALID) {
        VprocStatus = DSP_VPROC_STATUS_INVALID;
    } else if (UpdatedStatus == DSP_REPORTED_VPROC_STATUS_TIMER) {
        VprocStatus = DSP_VPROC_STATUS_TIMER;
    } else if (UpdatedStatus == DSP_REPORTED_VPROC_STATUS_ACTIVE) {
        VprocStatus = DSP_VPROC_STATUS_ACTIVE;
    } else {
        AmbaLL_Log(AMBALLLOG_TYPE_DBG,"Vproc[%d] receives unexpected status (%d)",
                   ChannelId, UpdatedStatus, 0U);
    }

    if (DspStatus.VprocStatus[ChannelId] != VprocStatus) {
        AmbaLL_Log(AMBALLLOG_TYPE_DBG,"Vproc[%d] status change to %d from %d", ChannelId, VprocStatus, DspStatus.VprocStatus[ChannelId]);
        DspStatus.VprocStatus[ChannelId] = VprocStatus;
    }
}

static inline void PrintEfctData(const msg_vproc_effect_data_status_t *pEffectDataStatus)
{
    AmbaLL_Log(AMBALLLOG_TYPE_MSG,"MSG_VPROC_EFFECT_DATA_STATUS", 0U, 0U, 0U);
    AmbaLL_Log(AMBALLLOG_TYPE_MSG,"channel_id                          = %d  ", pEffectDataStatus->channel_id, 0U, 0U);
    AmbaLL_Log(AMBALLLOG_TYPE_MSG,"status                              = %d  ", pEffectDataStatus->status, 0U, 0U);
    AmbaLL_Log(AMBALLLOG_TYPE_MSG,"stream_id                           = %d  ", pEffectDataStatus->stream_id, 0U, 0U);
    AmbaLL_Log(AMBALLLOG_TYPE_MSG,"group_id                            = %d  ", pEffectDataStatus->group_id, 0U, 0U);
    AmbaLL_Log(AMBALLLOG_TYPE_MSG,"cap_seq_no                          = %d  ", pEffectDataStatus->cap_seq_no, 0U, 0U);
    AmbaLL_Log(AMBALLLOG_TYPE_MSG,"effect_luma.buffer_addr             = 0x%X", pEffectDataStatus->effect_luma.buffer_addr, 0U, 0U);
    AmbaLL_Log(AMBALLLOG_TYPE_MSG,"effect_luma.buffer_pitch            = %d  ", pEffectDataStatus->effect_luma.buffer_pitch, 0U, 0U);
    AmbaLL_Log(AMBALLLOG_TYPE_MSG,"effect_luma.img_width               = %d  ", pEffectDataStatus->effect_luma.img_width, 0U, 0U);
    AmbaLL_Log(AMBALLLOG_TYPE_MSG,"effect_luma.img_height              = %d  ", pEffectDataStatus->effect_luma.img_height, 0U, 0U);
    AmbaLL_Log(AMBALLLOG_TYPE_MSG,"effect_chroma.buffer_addr           = 0x%X", pEffectDataStatus->effect_chroma.buffer_addr, 0U, 0U);
    AmbaLL_Log(AMBALLLOG_TYPE_MSG,"effect_chroma.buffer_pitch          = %d  ", pEffectDataStatus->effect_chroma.buffer_pitch, 0U, 0U);
    AmbaLL_Log(AMBALLLOG_TYPE_MSG,"effect_chroma.img_width             = %d  ", pEffectDataStatus->effect_chroma.img_width, 0U, 0U);
    AmbaLL_Log(AMBALLLOG_TYPE_MSG,"effect_chroma.img_height            = %d  ", pEffectDataStatus->effect_chroma.img_height, 0U, 0U);
    AmbaLL_Log(AMBALLLOG_TYPE_MSG,"effect_me0.buffer_addr              = 0x%X", pEffectDataStatus->effect_me0.buffer_addr, 0U, 0U);
    AmbaLL_Log(AMBALLLOG_TYPE_MSG,"effect_me0.buffer_pitch             = %d  ", pEffectDataStatus->effect_me0.buffer_pitch, 0U, 0U);
    AmbaLL_Log(AMBALLLOG_TYPE_MSG,"effect_me0.img_width                = %d  ", pEffectDataStatus->effect_me0.img_width, 0U, 0U);
    AmbaLL_Log(AMBALLLOG_TYPE_MSG,"effect_me0.img_height               = %d  ", pEffectDataStatus->effect_me0.img_height, 0U, 0U);
    AmbaLL_Log(AMBALLLOG_TYPE_MSG,"effect_me1.buffer_addr              = 0x%X", pEffectDataStatus->effect_me1.buffer_addr, 0U, 0U);
    AmbaLL_Log(AMBALLLOG_TYPE_MSG,"effect_me1.buffer_pitch             = %d  ", pEffectDataStatus->effect_me1.buffer_pitch, 0U, 0U);
    AmbaLL_Log(AMBALLLOG_TYPE_MSG,"effect_me1.img_width                = %d  ", pEffectDataStatus->effect_me1.img_width, 0U, 0U);
    AmbaLL_Log(AMBALLLOG_TYPE_MSG,"effect_me1.img_height               = %d  ", pEffectDataStatus->effect_me1.img_height, 0U, 0U);
    AmbaLL_Log(AMBALLLOG_TYPE_MSG,"vcap_audio_clk_counter              = %d  ", pEffectDataStatus->vcap_audio_clk_counter, 0U, 0U);
    AmbaLL_Log(AMBALLLOG_TYPE_MSG,"is_ext                              = %d  ", pEffectDataStatus->is_ext, 0U, 0U);
    AmbaLL_Log(AMBALLLOG_TYPE_MSG,"is_me01_ext                         = %d  ", pEffectDataStatus->is_me01_ext, 0U, 0U);
    AmbaLL_Log(AMBALLLOG_TYPE_MSG,"vproc_seq_no                        = %d  ", pEffectDataStatus->vproc_seq_no, 0U, 0U);
    AmbaLL_Log(AMBALLLOG_TYPE_MSG,"source_fov_bitmask                  = 0x%X", pEffectDataStatus->source_fov_bitmask, 0U, 0U);
}

#ifdef SUPPORT_VPROC_OSD_INSERT
static inline void PrintDlyLineOsdStatus(const msg_vproc_delayline_osd_status_t *pDlyLineOsdStatus)
{
    UINT32 AudClk;
    UINT32 JobProcTime;
    AMBA_LL_SYS_CLK_INFO_s SysClkInfo = {0};
    UINT32 Rval;

    SysClkInfo.Type = LL_SYS_CLK_AUD;
    Rval = DSP_GiveEvent(LL_EVENT_SYS_GET_CLK_INFO, &SysClkInfo, 0U);
    LL_PrintErrLine(Rval, __func__, __LINE__);
    AudClk = (SysClkInfo.Value == 0U) ? (DSP_DEFAULT_AUDIO_CLK/1000U) : (SysClkInfo.Value/1000U);

    AmbaLL_Log(AMBALLLOG_TYPE_MSG,"MSG_VPROC_DELAYLINE_OSD_STATUS", 0U, 0U, 0U);
    AmbaLL_Log(AMBALLLOG_TYPE_MSG,"channel_id                          = %d  ", pDlyLineOsdStatus->channel_id, 0U, 0U);
    AmbaLL_Log(AMBALLLOG_TYPE_MSG,"status                              = %d  ", pDlyLineOsdStatus->status, 0U, 0U);
    AmbaLL_Log(AMBALLLOG_TYPE_MSG,"stream_id                           = %d  ", pDlyLineOsdStatus->stream_id, 0U, 0U);
    AmbaLL_Log(AMBALLLOG_TYPE_MSG,"cap_seq_no                          = %d  ", pDlyLineOsdStatus->cap_seq_no, 0U, 0U);
    AmbaLL_Log(AMBALLLOG_TYPE_MSG,"osd_sync_error                      = %d  ", pDlyLineOsdStatus->osd_sync_error, 0U, 0U);
    AmbaLL_Log(AMBALLLOG_TYPE_MSG,"vin_cap_done_pts                    = %d  ", pDlyLineOsdStatus->vin_cap_done_pts, 0U, 0U);
    AmbaLL_Log(AMBALLLOG_TYPE_MSG,"y2y_done_pts                        = %d  ", pDlyLineOsdStatus->y2y_done_pts, 0U, 0U);
    AmbaLL_Log(AMBALLLOG_TYPE_MSG,"job_start_pts                       = %d  ", pDlyLineOsdStatus->job_start_pts, 0U, 0U);
    JobProcTime = ((pDlyLineOsdStatus->job_done_pts - pDlyLineOsdStatus->job_start_pts)*1000U)/AudClk;
    AmbaLL_Log(AMBALLLOG_TYPE_MSG,"job_done_pts                        = %d Proc[%dus]", pDlyLineOsdStatus->job_done_pts, JobProcTime, 0U);
	AmbaLL_Log(AMBALLLOG_TYPE_MSG,"vproc_done_pts                      = %d  ", pDlyLineOsdStatus->vproc_done_pts, 0U, 0U);
	AmbaLL_Log(AMBALLLOG_TYPE_MSG,"user_id                             = %d  ", pDlyLineOsdStatus->user_id, 0U, 0U);
}
#endif

static inline void PrintExtStatus(const msg_vproc_ext_status_t *pVprocExtStatus)
{
    AmbaLL_Log(AMBALLLOG_TYPE_MSG,"MSG_VPROC_EXT_STATUS", 0U, 0U, 0U);
    AmbaLL_Log(AMBALLLOG_TYPE_MSG," channel_id               = %d  ", pVprocExtStatus->channel_id, 0U, 0U);
    AmbaLL_Log(AMBALLLOG_TYPE_MSG," status                   = %d  ", pVprocExtStatus->status, 0U, 0U);
    AmbaLL_Log(AMBALLLOG_TYPE_MSG," pass_step_id             = %d  ", pVprocExtStatus->pass_step_id, 0U, 0U);
    AmbaLL_Log(AMBALLLOG_TYPE_MSG," is_last_tile             = %d  ", pVprocExtStatus->is_last_tile, 0U, 0U);
    AmbaLL_Log(AMBALLLOG_TYPE_MSG," cap_seq_no               = %d  ", pVprocExtStatus->cap_seq_no, 0U, 0U);
    AmbaLL_Log(AMBALLLOG_TYPE_MSG," batch_cmd_id             = 0x%X", pVprocExtStatus->batch_cmd_id, 0U, 0U);
    LL_PrintBatchInfoId(pVprocExtStatus->batch_cmd_id);
    AmbaLL_Log(AMBALLLOG_TYPE_MSG," sect_mask                = 0x%X", pVprocExtStatus->sect_mask, 0U, 0U);
    AmbaLL_Log(AMBALLLOG_TYPE_MSG," idsp_cfg_hdr_addr[0]     = 0x%X", pVprocExtStatus->idsp_cfg_hdr_addr[0], 0U, 0U);
    AmbaLL_Log(AMBALLLOG_TYPE_MSG," idsp_cfg_hdr_addr[1]     = 0x%X", pVprocExtStatus->idsp_cfg_hdr_addr[1], 0U, 0U);
    AmbaLL_Log(AMBALLLOG_TYPE_MSG," idsp_cfg_hdr_addr[2]     = 0x%X", pVprocExtStatus->idsp_cfg_hdr_addr[2], 0U, 0U);
    AmbaLL_Log(AMBALLLOG_TYPE_MSG," idsp_cfg_hdr_addr[3]     = 0x%X", pVprocExtStatus->idsp_cfg_hdr_addr[3], 0U, 0U);
    AmbaLL_Log(AMBALLLOG_TYPE_MSG," idsp_cfg_hdr_addr[4]     = 0x%X", pVprocExtStatus->idsp_cfg_hdr_addr[4], 0U, 0U);
    AmbaLL_Log(AMBALLLOG_TYPE_MSG," idsp_cfg_hdr_addr[5]     = 0x%X", pVprocExtStatus->idsp_cfg_hdr_addr[5], 0U, 0U);
    AmbaLL_Log(AMBALLLOG_TYPE_MSG," idsp_cfg_hdr_addr[6]     = 0x%X", pVprocExtStatus->idsp_cfg_hdr_addr[6], 0U, 0U);
    AmbaLL_Log(AMBALLLOG_TYPE_MSG," idsp_cfg_hdr_addr[7]     = 0x%X", pVprocExtStatus->idsp_cfg_hdr_addr[7], 0U, 0U);
    AmbaLL_Log(AMBALLLOG_TYPE_MSG," vproc_done_hw_pts        = %d  ", pVprocExtStatus->vproc_done_hw_pts, 0U, 0U);
    AmbaLL_Log(AMBALLLOG_TYPE_MSG," vin_cap_done_pts         = %d  ", pVprocExtStatus->vin_cap_done_pts, 0U, 0U);
    AmbaLL_Log(AMBALLLOG_TYPE_MSG," job_done_pts             = %d  ", pVprocExtStatus->job_done_pts, 0U, 0U);
    AmbaLL_Log(AMBALLLOG_TYPE_MSG," grp_cmd_buf_id           = 0x%X", pVprocExtStatus->grp_cmd_buf_id, 0U, 0U);
}

static inline void PrintAAAStatus(const msg_vproc_aaa_status_t *pVprocAaaStatus)
{
    AmbaLL_Log(AMBALLLOG_TYPE_MSG,"MSG_VPROC_AAA_STATUS", 0U, 0U, 0U);
    AmbaLL_Log(AMBALLLOG_TYPE_MSG," channel_id               = %d  ", pVprocAaaStatus->channel_id, 0U, 0U);
    AmbaLL_Log(AMBALLLOG_TYPE_MSG," status                   = %d  ", pVprocAaaStatus->status, 0U, 0U);
    AmbaLL_Log(AMBALLLOG_TYPE_MSG," raw_compression_mode     = %d  ", pVprocAaaStatus->raw_compression_mode, 0U, 0U);
    AmbaLL_Log(AMBALLLOG_TYPE_MSG," iso_mode                 = %d  ", pVprocAaaStatus->iso_mode, 0U, 0U);
    AmbaLL_Log(AMBALLLOG_TYPE_MSG," cap_seq_no               = %d  ", pVprocAaaStatus->cap_seq_no, 0U, 0U);
    AmbaLL_Log(AMBALLLOG_TYPE_MSG," batch_cmd_id             = 0x%X", pVprocAaaStatus->batch_cmd_id, 0U, 0U);
    LL_PrintBatchInfoId(pVprocAaaStatus->batch_cmd_id);
    AmbaLL_Log(AMBALLLOG_TYPE_MSG," tile_params_addr         = 0x%X", pVprocAaaStatus->tile_params_addr, 0U, 0U);
    AmbaLL_Log(AMBALLLOG_TYPE_MSG," raw_buf_addr             = 0x%X", pVprocAaaStatus->raw_buf_addr, 0U, 0U);
    AmbaLL_Log(AMBALLLOG_TYPE_MSG," raw_buf_pitch            = %d  ", pVprocAaaStatus->raw_buf_pitch, 0U, 0U);
    AmbaLL_Log(AMBALLLOG_TYPE_MSG," raw_buf_width            = %d  ", pVprocAaaStatus->raw_buf_width, 0U, 0U);
    AmbaLL_Log(AMBALLLOG_TYPE_MSG," raw_buf_height           = %d  ", pVprocAaaStatus->raw_buf_height, 0U, 0U);
    AmbaLL_Log(AMBALLLOG_TYPE_MSG," stitch_tile_num_x        = %d  ", pVprocAaaStatus->stitch_tile_num_x, 0U, 0U);
    AmbaLL_Log(AMBALLLOG_TYPE_MSG," stitch_tile_num_y        = %d  ", pVprocAaaStatus->stitch_tile_num_y, 0U, 0U);
    AmbaLL_Log(AMBALLLOG_TYPE_MSG," tile_x_idx               = %d  ", pVprocAaaStatus->tile_x_idx, 0U, 0U);
    AmbaLL_Log(AMBALLLOG_TYPE_MSG," tile_y_idx               = %d  ", pVprocAaaStatus->tile_y_idx, 0U, 0U);
    AmbaLL_Log(AMBALLLOG_TYPE_MSG," tile_start_x             = %d  ", pVprocAaaStatus->tile_start_x, 0U, 0U);
    AmbaLL_Log(AMBALLLOG_TYPE_MSG," tile_start_y             = %d  ", pVprocAaaStatus->tile_start_y, 0U, 0U);
    AmbaLL_Log(AMBALLLOG_TYPE_MSG," tile_w                   = %d  ", pVprocAaaStatus->tile_w, 0U, 0U);
    AmbaLL_Log(AMBALLLOG_TYPE_MSG," tile_h                   = %d  ", pVprocAaaStatus->tile_h, 0U, 0U);
    AmbaLL_Log(AMBALLLOG_TYPE_MSG," cfa_aaa_stat_buf_addr    = 0x%X", pVprocAaaStatus->cfa_aaa_stat_buf_addr, 0U, 0U);
    AmbaLL_Log(AMBALLLOG_TYPE_MSG," cfa_aaa_stat_buf_length  = %d  ", pVprocAaaStatus->cfa_aaa_stat_buf_length, 0U, 0U);
    AmbaLL_Log(AMBALLLOG_TYPE_MSG," rgb_aaa_stat_buf_addr    = 0x%X", pVprocAaaStatus->rgb_aaa_stat_buf_addr, 0U, 0U);
    AmbaLL_Log(AMBALLLOG_TYPE_MSG," rgb_aaa_stat_buf_length  = %d  ", pVprocAaaStatus->rgb_aaa_stat_buf_length, 0U, 0U);
    AmbaLL_Log(AMBALLLOG_TYPE_MSG," vin_stat_buf_addr        = 0x%X", pVprocAaaStatus->vin_stat_buf_addr, 0U, 0U);
    AmbaLL_Log(AMBALLLOG_TYPE_MSG," vin_stat_buf_length      = %d  ", pVprocAaaStatus->vin_stat_buf_length, 0U, 0U);
    AmbaLL_Log(AMBALLLOG_TYPE_MSG," hdr_vin_stat_buf_addr    = 0x%X", pVprocAaaStatus->hdr_vin_stat_buf_addr, 0U, 0U);
    AmbaLL_Log(AMBALLLOG_TYPE_MSG," hdr_vin_stat_buf_length  = %d  ", pVprocAaaStatus->hdr_vin_stat_buf_length, 0U, 0U);
    AmbaLL_Log(AMBALLLOG_TYPE_MSG," hdr2_vin_stat_buf_addr   = 0x%X", pVprocAaaStatus->hdr2_vin_stat_buf_addr, 0U, 0U);
    AmbaLL_Log(AMBALLLOG_TYPE_MSG," hdr2_vin_stat_buf_length = %d  ", pVprocAaaStatus->hdr2_vin_stat_buf_length, 0U, 0U);
    AmbaLL_Log(AMBALLLOG_TYPE_MSG," aaa_hl_pts               = %d  ", pVprocAaaStatus->aaa_hl_pts, 0U, 0U);
    AmbaLL_Log(AMBALLLOG_TYPE_MSG," aaa_hw_duration          = %d  ", pVprocAaaStatus->aaa_hw_duration, 0U, 0U);
}

static inline void PrintCompOutStatus(const msg_vproc_comp_out_status_t *pCompOutStatus)
{
    AmbaLL_Log(AMBALLLOG_TYPE_MSG,"MSG_VPROC_COMP_OUT_STATUS", 0U, 0U, 0U);
    AmbaLL_Log(AMBALLLOG_TYPE_MSG,"channel_id                          = %d  ", pCompOutStatus->channel_id, 0U, 0U);
    AmbaLL_Log(AMBALLLOG_TYPE_MSG,"status                              = %d  ", pCompOutStatus->status, 0U, 0U);
    AmbaLL_Log(AMBALLLOG_TYPE_MSG,"compression_out_enable              = %d  ", pCompOutStatus->compression_out_enable, 0U, 0U);
    AmbaLL_Log(AMBALLLOG_TYPE_MSG,"cap_seq_no                          = %d  ", pCompOutStatus->cap_seq_no, 0U, 0U);
    AmbaLL_Log(AMBALLLOG_TYPE_MSG,"batch_cmd_id                        = 0x%X", pCompOutStatus->batch_cmd_id, 0U, 0U);
    LL_PrintBatchInfoId(pCompOutStatus->batch_cmd_id);
    AmbaLL_Log(AMBALLLOG_TYPE_MSG," compressed_raw_out.buffer_addr     = %d  ", pCompOutStatus->compressed_raw_out.buffer_addr, 0U, 0U);
    AmbaLL_Log(AMBALLLOG_TYPE_MSG," compressed_raw_out.buffer_pitch    = %d  ", pCompOutStatus->compressed_raw_out.buffer_pitch, 0U, 0U);
    AmbaLL_Log(AMBALLLOG_TYPE_MSG," compressed_raw_out.img_width       = %d  ", pCompOutStatus->compressed_raw_out.img_width, 0U, 0U);
    AmbaLL_Log(AMBALLLOG_TYPE_MSG," compressed_raw_out.img_height      = %d  ", pCompOutStatus->compressed_raw_out.img_height, 0U, 0U);
}

static inline void PrintVprocStatusLastWarp(const msg_vproc_status_t *pVprocStatus)
{
    UINT16 i;
    ULONG ULAddr;
    const void *pVoid;
    UINT32 Rval;

    if (pVprocStatus->ext_data_daddr[0U] > 0U) {
        const vproc_li_warp_mctf_prev_ext_data_t *pVprocWarpExtData = NULL;

        Rval = dsp_osal_cli2virt(pVprocStatus->ext_data_daddr[0U], &ULAddr);
        LL_PrintErrLine(Rval, __func__, __LINE__);
        if (Rval == OK) {
            dsp_osal_typecast(&pVprocWarpExtData, &ULAddr);
            dsp_osal_typecast(&pVoid, &ULAddr);
            //VprocExt is Non-Cache
            //Rval = dsp_osal_cache_invalidate(pVoid, sizeof(vproc_li_warp_mctf_prev_ext_data_t));
            //AmbaMisra_TouchUnused(&Rval);
            AmbaLL_Log(AMBALLLOG_TYPE_MSG," Warp                                         ", 0U, 0U, 0U);
            AmbaLL_Log(AMBALLLOG_TYPE_MSG,"   sec3_luma_out.buffer_addr            = 0x%X", pVprocWarpExtData->sec3_luma_out.buffer_addr, 0U, 0U);
            AmbaLL_Log(AMBALLLOG_TYPE_MSG,"   sec3_luma_out.buffer_pitch           = %d  ", pVprocWarpExtData->sec3_luma_out.buffer_pitch, 0U, 0U);
            AmbaLL_Log(AMBALLLOG_TYPE_MSG,"   sec3_luma_out.img_width              = %d  ", pVprocWarpExtData->sec3_luma_out.img_width, 0U, 0U);
            AmbaLL_Log(AMBALLLOG_TYPE_MSG,"   sec3_luma_out.img_height             = %d  ", pVprocWarpExtData->sec3_luma_out.img_height, 0U, 0U);
            AmbaLL_Log(AMBALLLOG_TYPE_MSG,"   sec3_chroma_out.buffer_addr          = 0x%X", pVprocWarpExtData->sec3_chroma_out.buffer_addr, 0U, 0U);
            AmbaLL_Log(AMBALLLOG_TYPE_MSG,"   sec3_chroma_out.buffer_pitch         = %d  ", pVprocWarpExtData->sec3_chroma_out.buffer_pitch, 0U, 0U);
            AmbaLL_Log(AMBALLLOG_TYPE_MSG,"   sec3_chroma_out.img_width            = %d  ", pVprocWarpExtData->sec3_chroma_out.img_width, 0U, 0U);
            AmbaLL_Log(AMBALLLOG_TYPE_MSG,"   sec3_chroma_out.img_height           = %d  ", pVprocWarpExtData->sec3_chroma_out.img_height, 0U, 0U);
            AmbaLL_Log(AMBALLLOG_TYPE_MSG,"   sec3_me1_out.buffer_addr             = 0x%X", pVprocWarpExtData->sec3_me1_out.buffer_addr, 0U, 0U);
            AmbaLL_Log(AMBALLLOG_TYPE_MSG,"   sec3_me1_out.buffer_pitch            = %d  ", pVprocWarpExtData->sec3_me1_out.buffer_pitch, 0U, 0U);
            AmbaLL_Log(AMBALLLOG_TYPE_MSG,"   sec3_me1_out.img_width               = %d  ", pVprocWarpExtData->sec3_me1_out.img_width, 0U, 0U);
            AmbaLL_Log(AMBALLLOG_TYPE_MSG,"   sec3_me1_out.img_height              = %d  ", pVprocWarpExtData->sec3_me1_out.img_height, 0U, 0U);
            AmbaLL_Log(AMBALLLOG_TYPE_MSG,"   sec3_me0_out.buffer_addr             = 0x%X", pVprocWarpExtData->sec3_me0_out.buffer_addr, 0U, 0U);
            AmbaLL_Log(AMBALLLOG_TYPE_MSG,"   sec3_me0_out.buffer_pitch            = %d  ", pVprocWarpExtData->sec3_me0_out.buffer_pitch, 0U, 0U);
            AmbaLL_Log(AMBALLLOG_TYPE_MSG,"   sec3_me0_out.img_width               = %d  ", pVprocWarpExtData->sec3_me0_out.img_width, 0U, 0U);
            AmbaLL_Log(AMBALLLOG_TYPE_MSG,"   sec3_me0_out.img_height              = %d  ", pVprocWarpExtData->sec3_me0_out.img_height, 0U, 0U);
            AmbaLL_Log(AMBALLLOG_TYPE_MSG,"   sec5_luma_out.buffer_addr            = 0x%X", pVprocWarpExtData->sec5_luma_out.buffer_addr, 0U, 0U);
            AmbaLL_Log(AMBALLLOG_TYPE_MSG,"   sec5_luma_out.buffer_pitch           = %d  ", pVprocWarpExtData->sec5_luma_out.buffer_pitch, 0U, 0U);
            AmbaLL_Log(AMBALLLOG_TYPE_MSG,"   sec5_luma_out.img_width              = %d  ", pVprocWarpExtData->sec5_luma_out.img_width, 0U, 0U);
            AmbaLL_Log(AMBALLLOG_TYPE_MSG,"   sec5_luma_out.img_height             = %d  ", pVprocWarpExtData->sec5_luma_out.img_height, 0U, 0U);
            AmbaLL_Log(AMBALLLOG_TYPE_MSG,"   sec5_chroma_out.buffer_addr          = 0x%X", pVprocWarpExtData->sec5_chroma_out.buffer_addr, 0U, 0U);
            AmbaLL_Log(AMBALLLOG_TYPE_MSG,"   sec5_chroma_out.buffer_pitch         = %d  ", pVprocWarpExtData->sec5_chroma_out.buffer_pitch, 0U, 0U);
            AmbaLL_Log(AMBALLLOG_TYPE_MSG,"   sec5_chroma_out.img_width            = %d  ", pVprocWarpExtData->sec5_chroma_out.img_width, 0U, 0U);
            AmbaLL_Log(AMBALLLOG_TYPE_MSG,"   sec5_chroma_out.img_height           = %d  ", pVprocWarpExtData->sec5_chroma_out.img_height, 0U, 0U);
            AmbaLL_Log(AMBALLLOG_TYPE_MSG,"   sec5_me1_out.buffer_addr             = 0x%X", pVprocWarpExtData->sec5_me1_out.buffer_addr, 0U, 0U);
            AmbaLL_Log(AMBALLLOG_TYPE_MSG,"   sec5_me1_out.buffer_pitch            = %d  ", pVprocWarpExtData->sec5_me1_out.buffer_pitch, 0U, 0U);
            AmbaLL_Log(AMBALLLOG_TYPE_MSG,"   sec5_me1_out.img_width               = %d  ", pVprocWarpExtData->sec5_me1_out.img_width, 0U, 0U);
            AmbaLL_Log(AMBALLLOG_TYPE_MSG,"   sec5_me1_out.img_height              = %d  ", pVprocWarpExtData->sec5_me1_out.img_height, 0U, 0U);
            AmbaLL_Log(AMBALLLOG_TYPE_MSG,"   sec5_me0_out.buffer_addr             = 0x%X", pVprocWarpExtData->sec5_me0_out.buffer_addr, 0U, 0U);
            AmbaLL_Log(AMBALLLOG_TYPE_MSG,"   sec5_me0_out.buffer_pitch            = %d  ", pVprocWarpExtData->sec5_me0_out.buffer_pitch, 0U, 0U);
            AmbaLL_Log(AMBALLLOG_TYPE_MSG,"   sec5_me0_out.img_width               = %d  ", pVprocWarpExtData->sec5_me0_out.img_width, 0U, 0U);
            AmbaLL_Log(AMBALLLOG_TYPE_MSG,"   sec5_me0_out.img_height              = %d  ", pVprocWarpExtData->sec5_me0_out.img_height, 0U, 0U);
            AmbaLL_Log(AMBALLLOG_TYPE_MSG,"   sec6_luma_out.buffer_addr            = 0x%X", pVprocWarpExtData->sec6_luma_out.buffer_addr, 0U, 0U);
            AmbaLL_Log(AMBALLLOG_TYPE_MSG,"   sec6_luma_out.buffer_pitch           = %d  ", pVprocWarpExtData->sec6_luma_out.buffer_pitch, 0U, 0U);
            AmbaLL_Log(AMBALLLOG_TYPE_MSG,"   sec6_luma_out.img_width              = %d  ", pVprocWarpExtData->sec6_luma_out.img_width, 0U, 0U);
            AmbaLL_Log(AMBALLLOG_TYPE_MSG,"   sec6_luma_out.img_height             = %d  ", pVprocWarpExtData->sec6_luma_out.img_height, 0U, 0U);
            AmbaLL_Log(AMBALLLOG_TYPE_MSG,"   sec6_chroma_out.buffer_addr          = 0x%X", pVprocWarpExtData->sec6_chroma_out.buffer_addr, 0U, 0U);
            AmbaLL_Log(AMBALLLOG_TYPE_MSG,"   sec6_chroma_out.buffer_pitch         = %d  ", pVprocWarpExtData->sec6_chroma_out.buffer_pitch, 0U, 0U);
            AmbaLL_Log(AMBALLLOG_TYPE_MSG,"   sec6_chroma_out.img_width            = %d  ", pVprocWarpExtData->sec6_chroma_out.img_width, 0U, 0U);
            AmbaLL_Log(AMBALLLOG_TYPE_MSG,"   sec6_chroma_out.img_height           = %d  ", pVprocWarpExtData->sec6_chroma_out.img_height, 0U, 0U);
            AmbaLL_Log(AMBALLLOG_TYPE_MSG,"   sec6_me1_out.buffer_addr             = 0x%X", pVprocWarpExtData->sec6_me1_out.buffer_addr, 0U, 0U);
            AmbaLL_Log(AMBALLLOG_TYPE_MSG,"   sec6_me1_out.buffer_pitch            = %d  ", pVprocWarpExtData->sec6_me1_out.buffer_pitch, 0U, 0U);
            AmbaLL_Log(AMBALLLOG_TYPE_MSG,"   sec6_me1_out.img_width               = %d  ", pVprocWarpExtData->sec6_me1_out.img_width, 0U, 0U);
            AmbaLL_Log(AMBALLLOG_TYPE_MSG,"   sec6_me1_out.img_height              = %d  ", pVprocWarpExtData->sec6_me1_out.img_height, 0U, 0U);
            AmbaLL_Log(AMBALLLOG_TYPE_MSG,"   sec6_me0_out.buffer_addr             = 0x%X", pVprocWarpExtData->sec6_me0_out.buffer_addr, 0U, 0U);
            AmbaLL_Log(AMBALLLOG_TYPE_MSG,"   sec6_me0_out.buffer_pitch            = %d  ", pVprocWarpExtData->sec6_me0_out.buffer_pitch, 0U, 0U);
            AmbaLL_Log(AMBALLLOG_TYPE_MSG,"   sec6_me0_out.img_width               = %d  ", pVprocWarpExtData->sec6_me0_out.img_width, 0U, 0U);
            AmbaLL_Log(AMBALLLOG_TYPE_MSG,"   sec6_me0_out.img_height              = %d  ", pVprocWarpExtData->sec6_me0_out.img_height, 0U, 0U);
            AmbaLL_Log(AMBALLLOG_TYPE_MSG,"   sec7_luma_out.buffer_addr            = 0x%X", pVprocWarpExtData->sec7_luma_out.buffer_addr, 0U, 0U);
            AmbaLL_Log(AMBALLLOG_TYPE_MSG,"   sec7_luma_out.buffer_pitch           = %d  ", pVprocWarpExtData->sec7_luma_out.buffer_pitch, 0U, 0U);
            AmbaLL_Log(AMBALLLOG_TYPE_MSG,"   sec7_luma_out.img_width              = %d  ", pVprocWarpExtData->sec7_luma_out.img_width, 0U, 0U);
            AmbaLL_Log(AMBALLLOG_TYPE_MSG,"   sec7_luma_out.img_height             = %d  ", pVprocWarpExtData->sec7_luma_out.img_height, 0U, 0U);
            AmbaLL_Log(AMBALLLOG_TYPE_MSG,"   sec7_chroma_out.buffer_addr          = 0x%X", pVprocWarpExtData->sec7_chroma_out.buffer_addr, 0U, 0U);
            AmbaLL_Log(AMBALLLOG_TYPE_MSG,"   sec7_chroma_out.buffer_pitch         = %d  ", pVprocWarpExtData->sec7_chroma_out.buffer_pitch, 0U, 0U);
            AmbaLL_Log(AMBALLLOG_TYPE_MSG,"   sec7_chroma_out.img_width            = %d  ", pVprocWarpExtData->sec7_chroma_out.img_width, 0U, 0U);
            AmbaLL_Log(AMBALLLOG_TYPE_MSG,"   sec7_chroma_out.img_height           = %d  ", pVprocWarpExtData->sec7_chroma_out.img_height, 0U, 0U);
            AmbaLL_Log(AMBALLLOG_TYPE_MSG,"   sec7_me1_out.buffer_addr             = 0x%X", pVprocWarpExtData->sec7_me1_out.buffer_addr, 0U, 0U);
            AmbaLL_Log(AMBALLLOG_TYPE_MSG,"   sec7_me1_out.buffer_pitch            = %d  ", pVprocWarpExtData->sec7_me1_out.buffer_pitch, 0U, 0U);
            AmbaLL_Log(AMBALLLOG_TYPE_MSG,"   sec7_me1_out.img_width               = %d  ", pVprocWarpExtData->sec7_me1_out.img_width, 0U, 0U);
            AmbaLL_Log(AMBALLLOG_TYPE_MSG,"   sec7_me1_out.img_height              = %d  ", pVprocWarpExtData->sec7_me1_out.img_height, 0U, 0U);
            AmbaLL_Log(AMBALLLOG_TYPE_MSG,"   sec7_me0_out.buffer_addr             = 0x%X", pVprocWarpExtData->sec7_me0_out.buffer_addr, 0U, 0U);
            AmbaLL_Log(AMBALLLOG_TYPE_MSG,"   sec7_me0_out.buffer_pitch            = %d  ", pVprocWarpExtData->sec7_me0_out.buffer_pitch, 0U, 0U);
            AmbaLL_Log(AMBALLLOG_TYPE_MSG,"   sec7_me0_out.img_width               = %d  ", pVprocWarpExtData->sec7_me0_out.img_width, 0U, 0U);
            AmbaLL_Log(AMBALLLOG_TYPE_MSG,"   sec7_me0_out.img_height              = %d  ", pVprocWarpExtData->sec7_me0_out.img_height, 0U, 0U);
            AmbaLL_Log(AMBALLLOG_TYPE_MSG,"   mctf_luma_out.buffer_addr            = 0x%X", pVprocWarpExtData->mctf_luma_out.buffer_addr, 0U, 0U);
            AmbaLL_Log(AMBALLLOG_TYPE_MSG,"   mctf_luma_out.buffer_pitch           = %d  ", pVprocWarpExtData->mctf_luma_out.buffer_pitch, 0U, 0U);
            AmbaLL_Log(AMBALLLOG_TYPE_MSG,"   mctf_luma_out.img_width              = %d  ", pVprocWarpExtData->mctf_luma_out.img_width, 0U, 0U);
            AmbaLL_Log(AMBALLLOG_TYPE_MSG,"   mctf_luma_out.img_height             = %d  ", pVprocWarpExtData->mctf_luma_out.img_height, 0U, 0U);
            AmbaLL_Log(AMBALLLOG_TYPE_MSG,"   mctf_chroma_out.buffer_addr          = 0x%X", pVprocWarpExtData->mctf_chroma_out.buffer_addr, 0U, 0U);
            AmbaLL_Log(AMBALLLOG_TYPE_MSG,"   mctf_chroma_out.buffer_pitch         = %d  ", pVprocWarpExtData->mctf_chroma_out.buffer_pitch, 0U, 0U);
            AmbaLL_Log(AMBALLLOG_TYPE_MSG,"   mctf_chroma_out.img_width            = %d  ", pVprocWarpExtData->mctf_chroma_out.img_width, 0U, 0U);
            AmbaLL_Log(AMBALLLOG_TYPE_MSG,"   mctf_chroma_out.img_height           = %d  ", pVprocWarpExtData->mctf_chroma_out.img_height, 0U, 0U);
            AmbaLL_Log(AMBALLLOG_TYPE_MSG,"   mcts_luma_out.buffer_addr            = 0x%X", pVprocWarpExtData->mcts_luma_out.buffer_addr, 0U, 0U);
            AmbaLL_Log(AMBALLLOG_TYPE_MSG,"   mcts_luma_out.buffer_pitch           = %d  ", pVprocWarpExtData->mcts_luma_out.buffer_pitch, 0U, 0U);
            AmbaLL_Log(AMBALLLOG_TYPE_MSG,"   mcts_luma_out.img_width              = %d  ", pVprocWarpExtData->mcts_luma_out.img_width, 0U, 0U);
            AmbaLL_Log(AMBALLLOG_TYPE_MSG,"   mcts_luma_out.img_height             = %d  ", pVprocWarpExtData->mcts_luma_out.img_height, 0U, 0U);
            AmbaLL_Log(AMBALLLOG_TYPE_MSG,"   mcts_chroma_out.buffer_addr          = 0x%X", pVprocWarpExtData->mcts_chroma_out.buffer_addr, 0U, 0U);
            AmbaLL_Log(AMBALLLOG_TYPE_MSG,"   mcts_chroma_out.buffer_pitch         = %d  ", pVprocWarpExtData->mcts_chroma_out.buffer_pitch, 0U, 0U);
            AmbaLL_Log(AMBALLLOG_TYPE_MSG,"   mcts_chroma_out.img_width            = %d  ", pVprocWarpExtData->mcts_chroma_out.img_width, 0U, 0U);
            AmbaLL_Log(AMBALLLOG_TYPE_MSG,"   mcts_chroma_out.img_height           = %d  ", pVprocWarpExtData->mcts_chroma_out.img_height, 0U, 0U);
        }
    }

    if (pVprocStatus->ext_data_daddr[1U] > 0U) {
        UINT32 U32Val = 0U;
        const msg_vp_hier_out_lane_out_t *pHierLane = NULL;// = (msg_vp_hier_out_lane_out_t *)pVprocExtDataStatus->ext_data.hier_out_lane_out_daddr;

        Rval = dsp_osal_cli2virt(pVprocStatus->ext_data_daddr[1U], &ULAddr);
        if (Rval == OK) {
            dsp_osal_typecast(&pHierLane, &ULAddr);
            dsp_osal_typecast(&pVoid, &ULAddr);
            //VprocExt is Non-Cache
            //Rval = dsp_osal_cache_invalidate(pVoid, sizeof(msg_vp_hier_out_lane_out_t));
            //AmbaMisra_TouchUnused(&Rval);
            AmbaLL_Log(AMBALLLOG_TYPE_MSG," HeirLane                                     ", 0U, 0U, 0U);
            AmbaLL_Log(AMBALLLOG_TYPE_MSG,"   Header                                     ", 0U, 0U, 0U);
            AmbaLL_Log(AMBALLLOG_TYPE_MSG,"     channel_id                         = %d  ", pHierLane->header.channel_id, 0U, 0U);
            AmbaLL_Log(AMBALLLOG_TYPE_MSG,"     cap_seq_no                         = %d  ", pHierLane->header.cap_seq_no, 0U, 0U);
            AmbaLL_Log(AMBALLLOG_TYPE_MSG,"     batch_cmd_id                       = 0x%X", pHierLane->header.batch_cmd_id, 0U, 0U);
            LL_PrintBatchInfoId(pHierLane->header.batch_cmd_id);
            for(i = 0U; i < MAX_TOKEN_ARRAY; i++) {
                AmbaLL_Log(AMBALLLOG_TYPE_MSG,"     side_band_info[%2d].fov_id           = %d  ", i, pHierLane->header.side_band_info[i].fov_id, 0U);
                AmbaLL_Log(AMBALLLOG_TYPE_MSG,"     side_band_info[%2d].addr             = 0x%X", i, pHierLane->header.side_band_info[i].fov_private_info_addr, 0U);
            }
            for(i = 0U; i < MAX_ROI_CMD_TAG_NUM; i++) {
                AmbaLL_Log(AMBALLLOG_TYPE_MSG,"     roi_cmd_tag[%2d]                     = %d  ", i, pHierLane->header.roi_cmd_tag[i], 0U);
            }
            AmbaLL_Log(AMBALLLOG_TYPE_MSG,"     ln_det_hier_mask                   = 0x%X", pHierLane->header.ln_det_hier_mask, 0U, 0U);
            AmbaLL_Log(AMBALLLOG_TYPE_MSG,"     is_hier_out_tile_mode              = %d  ", pHierLane->header.is_hier_out_tile_mode, 0U, 0U);
            AmbaLL_Log(AMBALLLOG_TYPE_MSG,"     vin_cap_time                       = %d  ", pHierLane->header.vin_cap_time, 0U, 0U);
            AmbaLL_Log(AMBALLLOG_TYPE_MSG,"     proc_roi_cmd_time                  = %d  ", pHierLane->header.proc_roi_cmd_time, 0U, 0U);
            AmbaLL_Log(AMBALLLOG_TYPE_MSG,"     c2y_done_time                      = %d  ", pHierLane->header.c2y_done_time, 0U, 0U);
            AmbaLL_Log(AMBALLLOG_TYPE_MSG,"     warp_mctf_done_time                = %d  ", pHierLane->header.warp_mctf_done_time, 0U, 0U);
            for(i = 0U; i < DSP_HIER_NUM; i++) {
                /*
                 * BufAddr is already ROI start address,
                 * From observation, use same orginal buffer as Non-Crop one,
                 * So BufPitch is the same as NonCrop one,
                 * ImgW/H reflect RoiW/H
                 */
                AmbaLL_Log(AMBALLLOG_TYPE_MSG,"   LumaHier[%2d].BufAddr                 = 0x%X", i, pHierLane->luma_hier_outs[i].buffer_addr, 0U);
                AmbaLL_Log(AMBALLLOG_TYPE_MSG,"   LumaHier[%2d].BufPitch                = %d  ", i, pHierLane->luma_hier_outs[i].buffer_pitch, 0U);
                AmbaLL_Log(AMBALLLOG_TYPE_MSG,"   LumaHier[%2d].ImgWidth                = %d  ", i, pHierLane->luma_hier_outs[i].img_width, 0U);
                AmbaLL_Log(AMBALLLOG_TYPE_MSG,"   LumaHier[%2d].ImgHeight               = %d  ", i, pHierLane->luma_hier_outs[i].img_height, 0U);
            }
            for(i = 0U; i < DSP_HIER_NUM; i++) {
                AmbaLL_Log(AMBALLLOG_TYPE_MSG,"   ChromaHier[%2d].BufAddr               = 0x%X", i, pHierLane->chroma_hier_outs[i].buffer_addr, 0U);
                AmbaLL_Log(AMBALLLOG_TYPE_MSG,"   ChromaHier[%2d].BufPitch              = %d  ", i, pHierLane->chroma_hier_outs[i].buffer_pitch, 0U);
                AmbaLL_Log(AMBALLLOG_TYPE_MSG,"   ChromaHier[%2d].ImgWidth              = %d  ", i, pHierLane->chroma_hier_outs[i].img_width, 0U);
                AmbaLL_Log(AMBALLLOG_TYPE_MSG,"   ChromaHier[%2d].ImgHeight             = %d  ", i, pHierLane->chroma_hier_outs[i].img_height, 0U);
            }
            for(i = 0U; i < DSP_HIER_NUM; i++) {
                /* reflect ROI offsetX/Y */
                U32Val = (UINT32)pHierLane->hier_out_offsets[i].x_offset;
                AmbaLL_Log(AMBALLLOG_TYPE_MSG,"   HierOut[%2d].OffsetX                  = %d  ", i, U32Val, 0U);
                U32Val = (UINT32)pHierLane->hier_out_offsets[i].y_offset;
                AmbaLL_Log(AMBALLLOG_TYPE_MSG,"   HierOut[%2d].OffsetY                  = %d  ", i, U32Val, 0U);
            }
            AmbaLL_Log(AMBALLLOG_TYPE_MSG,"   LumaLaneOut.BufAddr              = 0x%X", pHierLane->luma_lane_det_out.buffer_addr, 0U, 0U);
            AmbaLL_Log(AMBALLLOG_TYPE_MSG,"   LumaLaneOut.BufPitch             = %d  ", pHierLane->luma_lane_det_out.buffer_pitch, 0U, 0U);
            AmbaLL_Log(AMBALLLOG_TYPE_MSG,"   LumaLaneOut.ImgWidth             = %d  ", pHierLane->luma_lane_det_out.img_width, 0U, 0U);
            AmbaLL_Log(AMBALLLOG_TYPE_MSG,"   LumaLaneOut.ImgHeight            = %d  ", pHierLane->luma_lane_det_out.img_height, 0U, 0U);
            AmbaLL_Log(AMBALLLOG_TYPE_MSG,"   ChromaLaneOut.BufAddr            = 0x%X", pHierLane->chroma_lane_det_out.buffer_addr, 0U, 0U);
            AmbaLL_Log(AMBALLLOG_TYPE_MSG,"   ChromaLaneOut.BufPitch           = %d  ", pHierLane->chroma_lane_det_out.buffer_pitch, 0U, 0U);
            AmbaLL_Log(AMBALLLOG_TYPE_MSG,"   ChromaLaneOut.ImgWidth           = %d  ", pHierLane->chroma_lane_det_out.img_width, 0U, 0U);
            AmbaLL_Log(AMBALLLOG_TYPE_MSG,"   ChromaLaneOut.ImgHeight          = %d  ", pHierLane->chroma_lane_det_out.img_height, 0U, 0U);
            U32Val = (UINT32)pHierLane->lane_det_out_offset.x_offset;
            AmbaLL_Log(AMBALLLOG_TYPE_MSG,"   LaneOut.OffsetX                  = %d  ", U32Val, 0U, 0U);
            U32Val = (UINT32)pHierLane->lane_det_out_offset.y_offset;
            AmbaLL_Log(AMBALLLOG_TYPE_MSG,"   LaneOut.OffsetY                  = %d  ", U32Val, 0U, 0U);
        }
    }
}

static inline void PrintVprocStatus(const msg_vproc_status_t *pVprocStatus)
{
    ULONG ULAddr;
    const void *pVoid;
    UINT32 AudClk;
    UINT32 JobProcTime, JobWaitTime;
    UINT8 LastTile = 0U;
    AMBA_LL_SYS_CLK_INFO_s SysClkInfo = {0};
    UINT32 Rval;

    SysClkInfo.Type = LL_SYS_CLK_AUD;
    Rval = DSP_GiveEvent(LL_EVENT_SYS_GET_CLK_INFO, &SysClkInfo, 0U);
    LL_PrintErrLine(Rval, __func__, __LINE__);
    AudClk = (SysClkInfo.Value == 0U) ? (DSP_DEFAULT_AUDIO_CLK/1000U) : (SysClkInfo.Value/1000U);

    AmbaLL_Log(AMBALLLOG_TYPE_MSG,"MSG_VPROC_STATUS", 0U, 0U, 0U);
    AmbaLL_Log(AMBALLLOG_TYPE_MSG," channel_id               = %d  ", pVprocStatus->channel_id, 0U, 0U);
    AmbaLL_Log(AMBALLLOG_TYPE_MSG," status                   = %d  ", pVprocStatus->status, 0U, 0U);
    AmbaLL_Log(AMBALLLOG_TYPE_MSG," pass_step_id             = %d  ", pVprocStatus->pass_step_id, 0U, 0U);
    AmbaLL_Log(AMBALLLOG_TYPE_MSG," iso_mode                 = %d  ", pVprocStatus->iso_mode, 0U, 0U);
    AmbaLL_Log(AMBALLLOG_TYPE_MSG," cap_seq_no               = %d  ", pVprocStatus->cap_seq_no, 0U, 0U);
    AmbaLL_Log(AMBALLLOG_TYPE_MSG," batch_cmd_id             = 0x%X", pVprocStatus->batch_cmd_id, 0U, 0U);
    LL_PrintBatchInfoId(pVprocStatus->batch_cmd_id);
    AmbaLL_Log(AMBALLLOG_TYPE_MSG," ext_buf_idc              = 0x%X", pVprocStatus->ext_buf_idc, 0U, 0U);
    AmbaLL_Log(AMBALLLOG_TYPE_MSG," is_last_tile             = %d  ", pVprocStatus->is_last_tile, 0U, 0U);
    AmbaLL_Log(AMBALLLOG_TYPE_MSG," metadata_status          = %d  ", pVprocStatus->metadata_status, 0U, 0U);
    AmbaLL_Log(AMBALLLOG_TYPE_MSG," group_id                 = %d  ", pVprocStatus->group_id, 0U, 0U);
    AmbaLL_Log(AMBALLLOG_TYPE_MSG," vin_id                   = %d  ", pVprocStatus->vin_id, 0U, 0U);
    AmbaLL_Log(AMBALLLOG_TYPE_MSG," stitch_tile_num_x        = %d  ", pVprocStatus->stitch_tile_num_x, 0U, 0U);
    AmbaLL_Log(AMBALLLOG_TYPE_MSG," stitch_tile_num_y        = %d  ", pVprocStatus->stitch_tile_num_y, 0U, 0U);
    AmbaLL_Log(AMBALLLOG_TYPE_MSG," tile_x_idx               = %d  ", pVprocStatus->tile_x_idx, 0U, 0U);
    AmbaLL_Log(AMBALLLOG_TYPE_MSG," tile_y_idx               = %d  ", pVprocStatus->tile_y_idx, 0U, 0U);
    AmbaLL_Log(AMBALLLOG_TYPE_MSG," sect_mask                = 0x%X", pVprocStatus->sect_mask, 0U, 0U);
    AmbaLL_Log(AMBALLLOG_TYPE_MSG," idsp_cfg_hdr_addr[0]     = 0x%X", pVprocStatus->idsp_cfg_hdr_addr[0U], 0U, 0U);
    AmbaLL_Log(AMBALLLOG_TYPE_MSG," idsp_cfg_hdr_addr[1]     = 0x%X", pVprocStatus->idsp_cfg_hdr_addr[1U], 0U, 0U);
    AmbaLL_Log(AMBALLLOG_TYPE_MSG," idsp_cfg_hdr_addr[2]     = 0x%X", pVprocStatus->idsp_cfg_hdr_addr[2U], 0U, 0U);
    AmbaLL_Log(AMBALLLOG_TYPE_MSG," idsp_cfg_hdr_addr[3]     = 0x%X", pVprocStatus->idsp_cfg_hdr_addr[3U], 0U, 0U);
    AmbaLL_Log(AMBALLLOG_TYPE_MSG," idsp_cfg_hdr_addr[4]     = 0x%X", pVprocStatus->idsp_cfg_hdr_addr[4U], 0U, 0U);
    AmbaLL_Log(AMBALLLOG_TYPE_MSG," idsp_cfg_hdr_addr[5]     = 0x%X", pVprocStatus->idsp_cfg_hdr_addr[5U], 0U, 0U);
    AmbaLL_Log(AMBALLLOG_TYPE_MSG," idsp_cfg_hdr_addr[6]     = 0x%X", pVprocStatus->idsp_cfg_hdr_addr[6U], 0U, 0U);
    AmbaLL_Log(AMBALLLOG_TYPE_MSG," idsp_cfg_hdr_addr[7]     = 0x%X", pVprocStatus->idsp_cfg_hdr_addr[7U], 0U, 0U);
    AmbaLL_Log(AMBALLLOG_TYPE_MSG," vproc_done_hw_pts        = %d  ", pVprocStatus->vproc_done_hw_pts, 0U, 0U);
    AmbaLL_Log(AMBALLLOG_TYPE_MSG," vin_cap_done_pts         = %d  ", pVprocStatus->vin_cap_done_pts, 0U, 0U);
    JobProcTime = ((pVprocStatus->job_done_pts - pVprocStatus->job_start_pts)*1000U)/AudClk;
    AmbaLL_Log(AMBALLLOG_TYPE_MSG," job_done_pts             = %d Proc[%dus]", pVprocStatus->job_done_pts, JobProcTime, 0U);
    AmbaLL_Log(AMBALLLOG_TYPE_MSG," ext_data_daddr[0]        = 0x%X", pVprocStatus->ext_data_daddr[0U], 0U, 0U);
    AmbaLL_Log(AMBALLLOG_TYPE_MSG," ext_data_daddr[1]        = 0x%X", pVprocStatus->ext_data_daddr[1U], 0U, 0U);
    AmbaLL_Log(AMBALLLOG_TYPE_MSG," grp_cmd_buf_id           = 0x%X", pVprocStatus->grp_cmd_buf_id, 0U, 0U);
    JobWaitTime = ((pVprocStatus->job_start_pts - pVprocStatus->vin_cap_done_pts)*1000U)/AudClk;
    AmbaLL_Log(AMBALLLOG_TYPE_MSG," job_start_pts            = %d Pend[%dus]", pVprocStatus->job_start_pts, JobWaitTime, 0U);
    AmbaLL_Log(AMBALLLOG_TYPE_MSG," vproc_seq_no             = %d  ", pVprocStatus->vproc_seq_no, 0U, 0U);
    if (((pVprocStatus->tile_x_idx + 1U) == pVprocStatus->stitch_tile_num_x) &&
        ((pVprocStatus->tile_y_idx + 1U) == pVprocStatus->stitch_tile_num_y)) {
        LastTile = 1U;
    }

    if ((LastTile == 1U) &&
        (pVprocStatus->pass_step_id == DSP_IMG_PASS_C2Y)) {
        const vproc_li_c2y_ext_data_t *pVprocLiC2yExtData = NULL;

        if (pVprocStatus->ext_data_daddr[0U] > 0U) {
            Rval = dsp_osal_cli2virt(pVprocStatus->ext_data_daddr[0U], &ULAddr);
            if (Rval == OK) {
                dsp_osal_typecast(&pVprocLiC2yExtData, &ULAddr);
                dsp_osal_typecast(&pVoid, &ULAddr);
                //VprocExt is Non-Cache
                //Rval = dsp_osal_cache_invalidate(pVoid, sizeof(vproc_li_c2y_ext_data_t));
                //AmbaMisra_TouchUnused(&Rval);
                AmbaLL_Log(AMBALLLOG_TYPE_MSG," li_c2y                                       ", 0U, 0U, 0U);
                AmbaLL_Log(AMBALLLOG_TYPE_MSG,"   sec2_cfg_addr                        = 0x%X", pVprocLiC2yExtData->sec2_cfg_addr, 0U, 0U);
                AmbaLL_Log(AMBALLLOG_TYPE_MSG,"   sec2_cfg_len                         = %d  ", pVprocLiC2yExtData->sec2_cfg_len, 0U, 0U);
                AmbaLL_Log(AMBALLLOG_TYPE_MSG,"   sec4_cfg_addr                        = 0x%X", pVprocLiC2yExtData->sec4_cfg_addr, 0U, 0U);
                AmbaLL_Log(AMBALLLOG_TYPE_MSG,"   sec4_cfg_len                         = %d  ", pVprocLiC2yExtData->sec4_cfg_len, 0U, 0U);
                AmbaLL_Log(AMBALLLOG_TYPE_MSG,"   c2y_luma_out.buffer_addr             = 0x%X", pVprocLiC2yExtData->c2y_luma_out.buffer_addr, 0U, 0U);
                AmbaLL_Log(AMBALLLOG_TYPE_MSG,"   c2y_luma_out.buffer_pitch            = %d  ", pVprocLiC2yExtData->c2y_luma_out.buffer_pitch, 0U, 0U);
                AmbaLL_Log(AMBALLLOG_TYPE_MSG,"   c2y_luma_out.img_width               = %d  ", pVprocLiC2yExtData->c2y_luma_out.img_width, 0U, 0U);
                AmbaLL_Log(AMBALLLOG_TYPE_MSG,"   c2y_luma_out.img_height              = %d  ", pVprocLiC2yExtData->c2y_luma_out.img_height, 0U, 0U);
                AmbaLL_Log(AMBALLLOG_TYPE_MSG,"   c2y_chroma_out.buffer_addr           = 0x%X", pVprocLiC2yExtData->c2y_chroma_out.buffer_addr, 0U, 0U);
                AmbaLL_Log(AMBALLLOG_TYPE_MSG,"   c2y_chroma_out.buffer_pitch          = %d  ", pVprocLiC2yExtData->c2y_chroma_out.buffer_pitch, 0U, 0U);
                AmbaLL_Log(AMBALLLOG_TYPE_MSG,"   c2y_chroma_out.img_width             = %d  ", pVprocLiC2yExtData->c2y_chroma_out.img_width, 0U, 0U);
                AmbaLL_Log(AMBALLLOG_TYPE_MSG,"   c2y_chroma_out.img_height            = %d  ", pVprocLiC2yExtData->c2y_chroma_out.img_height, 0U, 0U);
                AmbaLL_Log(AMBALLLOG_TYPE_MSG,"   compressed_raw_out.buffer_addr       = 0x%X", pVprocLiC2yExtData->compressed_raw_out.buffer_addr, 0U, 0U);
                AmbaLL_Log(AMBALLLOG_TYPE_MSG,"   compressed_raw_out.buffer_pitch      = %d  ", pVprocLiC2yExtData->compressed_raw_out.buffer_pitch, 0U, 0U);
                AmbaLL_Log(AMBALLLOG_TYPE_MSG,"   compressed_raw_out.img_width         = %d  ", pVprocLiC2yExtData->compressed_raw_out.img_width, 0U, 0U);
                AmbaLL_Log(AMBALLLOG_TYPE_MSG,"   compressed_raw_out.img_height        = %d  ", pVprocLiC2yExtData->compressed_raw_out.img_height, 0U, 0U);
            }
        }
    } else if ((LastTile == 1U) &&
               (pVprocStatus->pass_step_id == DSP_IMG_PASS_YUYV2Y)) {
        const vproc_yuyv2y_ext_data_t *pVprocyuyv2yExtData = NULL;

        if (pVprocStatus->ext_data_daddr[0U] > 0U) {
            Rval = dsp_osal_cli2virt(pVprocStatus->ext_data_daddr[0U], &ULAddr);
            if (Rval == OK) {
                dsp_osal_typecast(&pVprocyuyv2yExtData, &ULAddr);
                dsp_osal_typecast(&pVoid, &ULAddr);
                //VprocExt is Non-Cache
                //Rval = dsp_osal_cache_invalidate(pVoid, sizeof(vproc_yuyv2y_ext_data_t));
                //AmbaMisra_TouchUnused(&Rval);
                AmbaLL_Log(AMBALLLOG_TYPE_MSG," yuyv2y                                       ", 0U, 0U, 0U);
                AmbaLL_Log(AMBALLLOG_TYPE_MSG,"   sec2_cfg_addr                        = 0x%X", pVprocyuyv2yExtData->sec2_cfg_addr, 0U, 0U);
                AmbaLL_Log(AMBALLLOG_TYPE_MSG,"   sec2_cfg_len                         = %d  ", pVprocyuyv2yExtData->sec2_cfg_len, 0U, 0U);
                AmbaLL_Log(AMBALLLOG_TYPE_MSG,"   sec6_cfg_addr                        = 0x%X", pVprocyuyv2yExtData->sec6_cfg_addr, 0U, 0U);
                AmbaLL_Log(AMBALLLOG_TYPE_MSG,"   sec6_cfg_len                         = %d  ", pVprocyuyv2yExtData->sec6_cfg_len, 0U, 0U);
                AmbaLL_Log(AMBALLLOG_TYPE_MSG,"   sec7_cfg_addr                        = 0x%X", pVprocyuyv2yExtData->sec7_cfg_addr, 0U, 0U);
                AmbaLL_Log(AMBALLLOG_TYPE_MSG,"   sec7_cfg_len                         = %d  ", pVprocyuyv2yExtData->sec7_cfg_len, 0U, 0U);
                AmbaLL_Log(AMBALLLOG_TYPE_MSG,"   sec2_luma_out.buffer_addr            = 0x%X", pVprocyuyv2yExtData->sec2_luma_out.buffer_addr, 0U, 0U);
                AmbaLL_Log(AMBALLLOG_TYPE_MSG,"   sec2_luma_out.buffer_pitch           = %d  ", pVprocyuyv2yExtData->sec2_luma_out.buffer_pitch, 0U, 0U);
                AmbaLL_Log(AMBALLLOG_TYPE_MSG,"   sec2_luma_out.img_width              = %d  ", pVprocyuyv2yExtData->sec2_luma_out.img_width, 0U, 0U);
                AmbaLL_Log(AMBALLLOG_TYPE_MSG,"   sec2_luma_out.img_height             = %d  ", pVprocyuyv2yExtData->sec2_luma_out.img_height, 0U, 0U);
                AmbaLL_Log(AMBALLLOG_TYPE_MSG,"   luma_out.buffer_addr                 = 0x%X", pVprocyuyv2yExtData->yuyv2y_luma_out.buffer_addr, 0U, 0U);
                AmbaLL_Log(AMBALLLOG_TYPE_MSG,"   luma_out.buffer_pitch                = %d  ", pVprocyuyv2yExtData->yuyv2y_luma_out.buffer_pitch, 0U, 0U);
                AmbaLL_Log(AMBALLLOG_TYPE_MSG,"   luma_out.img_width                   = %d  ", pVprocyuyv2yExtData->yuyv2y_luma_out.img_width, 0U, 0U);
                AmbaLL_Log(AMBALLLOG_TYPE_MSG,"   luma_out.img_height                  = %d  ", pVprocyuyv2yExtData->yuyv2y_luma_out.img_height, 0U, 0U);
                AmbaLL_Log(AMBALLLOG_TYPE_MSG,"   chroma_out.buffer_addr               = 0x%X", pVprocyuyv2yExtData->yuyv2y_chroma_out.buffer_addr, 0U, 0U);
                AmbaLL_Log(AMBALLLOG_TYPE_MSG,"   chroma_out.buffer_pitch              = %d  ", pVprocyuyv2yExtData->yuyv2y_chroma_out.buffer_pitch, 0U, 0U);
                AmbaLL_Log(AMBALLLOG_TYPE_MSG,"   chroma_out.img_width                 = %d  ", pVprocyuyv2yExtData->yuyv2y_chroma_out.img_width, 0U, 0U);
                AmbaLL_Log(AMBALLLOG_TYPE_MSG,"   chroma_out.img_height                = %d  ", pVprocyuyv2yExtData->yuyv2y_chroma_out.img_height, 0U, 0U);
            }
        }
    } else if ((LastTile == 1U) &&
               (pVprocStatus->pass_step_id == DSP_IMG_PASS_WARP)) {
        PrintVprocStatusLastWarp(pVprocStatus);
    } else if ((LastTile == 1U) &&
               ((pVprocStatus->pass_step_id >= DSP_IMG_PASS_HISO_STEP_1) &&
                (pVprocStatus->pass_step_id <= DSP_IMG_PASS_HISO_STEP_13))) {
        const sproc_hi_ext_data_t *pSprocHiExtData = NULL;

        Rval = dsp_osal_cli2virt(pVprocStatus->ext_data_daddr[0U], &ULAddr);
        if (Rval == OK) {
            dsp_osal_typecast(&pSprocHiExtData, &ULAddr);
            dsp_osal_typecast(&pVoid, &ULAddr);
            //VprocExt is Non-Cache
            //Rval = dsp_osal_cache_invalidate(pVoid, sizeof(sproc_hi_ext_data_t));
            //AmbaMisra_TouchUnused(&Rval);
            AmbaLL_Log(AMBALLLOG_TYPE_MSG," Hi                                           ", 0U, 0U, 0U);
            AmbaLL_Log(AMBALLLOG_TYPE_MSG,"   sec2_luma_out.buffer_addr            = 0x%X", pSprocHiExtData->sec2_luma_out.buffer_addr, 0U, 0U);
            AmbaLL_Log(AMBALLLOG_TYPE_MSG,"   sec2_luma_out.buffer_pitch           = %d  ", pSprocHiExtData->sec2_luma_out.buffer_pitch, 0U, 0U);
            AmbaLL_Log(AMBALLLOG_TYPE_MSG,"   sec2_luma_out.img_width              = %d  ", pSprocHiExtData->sec2_luma_out.img_width, 0U, 0U);
            AmbaLL_Log(AMBALLLOG_TYPE_MSG,"   sec2_luma_out.img_height             = %d  ", pSprocHiExtData->sec2_luma_out.img_height, 0U, 0U);
            AmbaLL_Log(AMBALLLOG_TYPE_MSG,"   sec2_chroma_out.buffer_addr          = 0x%X", pSprocHiExtData->sec2_chroma_out.buffer_addr, 0U, 0U);
            AmbaLL_Log(AMBALLLOG_TYPE_MSG,"   sec2_chroma_out.buffer_pitch         = %d  ", pSprocHiExtData->sec2_chroma_out.buffer_pitch, 0U, 0U);
            AmbaLL_Log(AMBALLLOG_TYPE_MSG,"   sec2_chroma_out.img_width            = %d  ", pSprocHiExtData->sec2_chroma_out.img_width, 0U, 0U);
            AmbaLL_Log(AMBALLLOG_TYPE_MSG,"   sec2_chroma_out.img_height           = %d  ", pSprocHiExtData->sec2_chroma_out.img_height, 0U, 0U);
            AmbaLL_Log(AMBALLLOG_TYPE_MSG,"   sec5_luma_out.buffer_addr            = 0x%X", pSprocHiExtData->sec5_luma_out.buffer_addr, 0U, 0U);
            AmbaLL_Log(AMBALLLOG_TYPE_MSG,"   sec5_luma_out.buffer_pitch           = %d  ", pSprocHiExtData->sec5_luma_out.buffer_pitch, 0U, 0U);
            AmbaLL_Log(AMBALLLOG_TYPE_MSG,"   sec5_luma_out.img_width              = %d  ", pSprocHiExtData->sec5_luma_out.img_width, 0U, 0U);
            AmbaLL_Log(AMBALLLOG_TYPE_MSG,"   sec5_luma_out.img_height             = %d  ", pSprocHiExtData->sec5_luma_out.img_height, 0U, 0U);
            AmbaLL_Log(AMBALLLOG_TYPE_MSG,"   sec5_chroma_out.buffer_addr          = 0x%X", pSprocHiExtData->sec5_chroma_out.buffer_addr, 0U, 0U);
            AmbaLL_Log(AMBALLLOG_TYPE_MSG,"   sec5_chroma_out.buffer_pitch         = %d  ", pSprocHiExtData->sec5_chroma_out.buffer_pitch, 0U, 0U);
            AmbaLL_Log(AMBALLLOG_TYPE_MSG,"   sec5_chroma_out.img_width            = %d  ", pSprocHiExtData->sec5_chroma_out.img_width, 0U, 0U);
            AmbaLL_Log(AMBALLLOG_TYPE_MSG,"   sec5_chroma_out.img_height           = %d  ", pSprocHiExtData->sec5_chroma_out.img_height, 0U, 0U);
            AmbaLL_Log(AMBALLLOG_TYPE_MSG,"   sec6_luma_out.buffer_addr            = 0x%X", pSprocHiExtData->sec6_luma_out.buffer_addr, 0U, 0U);
            AmbaLL_Log(AMBALLLOG_TYPE_MSG,"   sec6_luma_out.buffer_pitch           = %d  ", pSprocHiExtData->sec6_luma_out.buffer_pitch, 0U, 0U);
            AmbaLL_Log(AMBALLLOG_TYPE_MSG,"   sec6_luma_out.img_width              = %d  ", pSprocHiExtData->sec6_luma_out.img_width, 0U, 0U);
            AmbaLL_Log(AMBALLLOG_TYPE_MSG,"   sec6_luma_out.img_height             = %d  ", pSprocHiExtData->sec6_luma_out.img_height, 0U, 0U);
            AmbaLL_Log(AMBALLLOG_TYPE_MSG,"   sec6_chroma_out.buffer_addr          = 0x%X", pSprocHiExtData->sec6_chroma_out.buffer_addr, 0U, 0U);
            AmbaLL_Log(AMBALLLOG_TYPE_MSG,"   sec6_chroma_out.buffer_pitch         = %d  ", pSprocHiExtData->sec6_chroma_out.buffer_pitch, 0U, 0U);
            AmbaLL_Log(AMBALLLOG_TYPE_MSG,"   sec6_chroma_out.img_width            = %d  ", pSprocHiExtData->sec6_chroma_out.img_width, 0U, 0U);
            AmbaLL_Log(AMBALLLOG_TYPE_MSG,"   sec6_chroma_out.img_height           = %d  ", pSprocHiExtData->sec6_chroma_out.img_height, 0U, 0U);
            AmbaLL_Log(AMBALLLOG_TYPE_MSG,"   sec7_luma_out.buffer_addr            = 0x%X", pSprocHiExtData->sec7_luma_out.buffer_addr, 0U, 0U);
            AmbaLL_Log(AMBALLLOG_TYPE_MSG,"   sec7_luma_out.buffer_pitch           = %d  ", pSprocHiExtData->sec7_luma_out.buffer_pitch, 0U, 0U);
            AmbaLL_Log(AMBALLLOG_TYPE_MSG,"   sec7_luma_out.img_width              = %d  ", pSprocHiExtData->sec7_luma_out.img_width, 0U, 0U);
            AmbaLL_Log(AMBALLLOG_TYPE_MSG,"   sec7_luma_out.img_height             = %d  ", pSprocHiExtData->sec7_luma_out.img_height, 0U, 0U);
            AmbaLL_Log(AMBALLLOG_TYPE_MSG,"   sec7_chroma_out.buffer_addr          = 0x%X", pSprocHiExtData->sec7_chroma_out.buffer_addr, 0U, 0U);
            AmbaLL_Log(AMBALLLOG_TYPE_MSG,"   sec7_chroma_out.buffer_pitch         = %d  ", pSprocHiExtData->sec7_chroma_out.buffer_pitch, 0U, 0U);
            AmbaLL_Log(AMBALLLOG_TYPE_MSG,"   sec7_chroma_out.img_width            = %d  ", pSprocHiExtData->sec7_chroma_out.img_width, 0U, 0U);
            AmbaLL_Log(AMBALLLOG_TYPE_MSG,"   sec7_chroma_out.img_height           = %d  ", pSprocHiExtData->sec7_chroma_out.img_height, 0U, 0U);
            AmbaLL_Log(AMBALLLOG_TYPE_MSG,"   sec9_luma_out.buffer_addr            = 0x%X", pSprocHiExtData->sec9_luma_out.buffer_addr, 0U, 0U);
            AmbaLL_Log(AMBALLLOG_TYPE_MSG,"   sec9_luma_out.buffer_pitch           = %d  ", pSprocHiExtData->sec9_luma_out.buffer_pitch, 0U, 0U);
            AmbaLL_Log(AMBALLLOG_TYPE_MSG,"   sec9_luma_out.img_width              = %d  ", pSprocHiExtData->sec9_luma_out.img_width, 0U, 0U);
            AmbaLL_Log(AMBALLLOG_TYPE_MSG,"   sec9_luma_out.img_height             = %d  ", pSprocHiExtData->sec9_luma_out.img_height, 0U, 0U);
            AmbaLL_Log(AMBALLLOG_TYPE_MSG,"   sec9_chroma_out.buffer_addr          = 0x%X", pSprocHiExtData->sec9_chroma_out.buffer_addr, 0U, 0U);
            AmbaLL_Log(AMBALLLOG_TYPE_MSG,"   sec8_chroma_out.buffer_pitch         = %d  ", pSprocHiExtData->sec9_chroma_out.buffer_pitch, 0U, 0U);
            AmbaLL_Log(AMBALLLOG_TYPE_MSG,"   sec9_chroma_out.img_width            = %d  ", pSprocHiExtData->sec9_chroma_out.img_width, 0U, 0U);
            AmbaLL_Log(AMBALLLOG_TYPE_MSG,"   sec9_chroma_out.img_height           = %d  ", pSprocHiExtData->sec9_chroma_out.img_height, 0U, 0U);
            AmbaLL_Log(AMBALLLOG_TYPE_MSG,"   mctf_luma_out.buffer_addr            = 0x%X", pSprocHiExtData->mctf_luma_out.buffer_addr, 0U, 0U);
            AmbaLL_Log(AMBALLLOG_TYPE_MSG,"   mctf_luma_out.buffer_pitch           = %d  ", pSprocHiExtData->mctf_luma_out.buffer_pitch, 0U, 0U);
            AmbaLL_Log(AMBALLLOG_TYPE_MSG,"   mctf_luma_out.img_width              = %d  ", pSprocHiExtData->mctf_luma_out.img_width, 0U, 0U);
            AmbaLL_Log(AMBALLLOG_TYPE_MSG,"   mctf_luma_out.img_height             = %d  ", pSprocHiExtData->mctf_luma_out.img_height, 0U, 0U);
            AmbaLL_Log(AMBALLLOG_TYPE_MSG,"   mctf_chroma_out.buffer_addr          = 0x%X", pSprocHiExtData->mctf_chroma_out.buffer_addr, 0U, 0U);
            AmbaLL_Log(AMBALLLOG_TYPE_MSG,"   mctf_chroma_out.buffer_pitch         = %d  ", pSprocHiExtData->mctf_chroma_out.buffer_pitch, 0U, 0U);
            AmbaLL_Log(AMBALLLOG_TYPE_MSG,"   mctf_chroma_out.img_width            = %d  ", pSprocHiExtData->mctf_chroma_out.img_width, 0U, 0U);
            AmbaLL_Log(AMBALLLOG_TYPE_MSG,"   mctf_chroma_out.img_height           = %d  ", pSprocHiExtData->mctf_chroma_out.img_height, 0U, 0U);
            AmbaLL_Log(AMBALLLOG_TYPE_MSG,"   mcts_luma_out.buffer_addr            = 0x%X", pSprocHiExtData->mcts_luma_out.buffer_addr, 0U, 0U);
            AmbaLL_Log(AMBALLLOG_TYPE_MSG,"   mcts_luma_out.buffer_pitch           = %d  ", pSprocHiExtData->mcts_luma_out.buffer_pitch, 0U, 0U);
            AmbaLL_Log(AMBALLLOG_TYPE_MSG,"   mcts_luma_out.img_width              = %d  ", pSprocHiExtData->mcts_luma_out.img_width, 0U, 0U);
            AmbaLL_Log(AMBALLLOG_TYPE_MSG,"   mcts_luma_out.img_height             = %d  ", pSprocHiExtData->mcts_luma_out.img_height, 0U, 0U);
            AmbaLL_Log(AMBALLLOG_TYPE_MSG,"   mcts_chroma_out.buffer_addr          = 0x%X", pSprocHiExtData->mcts_chroma_out.buffer_addr, 0U, 0U);
            AmbaLL_Log(AMBALLLOG_TYPE_MSG,"   mcts_chroma_out.buffer_pitch         = %d  ", pSprocHiExtData->mcts_chroma_out.buffer_pitch, 0U, 0U);
            AmbaLL_Log(AMBALLLOG_TYPE_MSG,"   mcts_chroma_out.img_width            = %d  ", pSprocHiExtData->mcts_chroma_out.img_width, 0U, 0U);
            AmbaLL_Log(AMBALLLOG_TYPE_MSG,"   mcts_chroma_out.img_height           = %d  ", pSprocHiExtData->mcts_chroma_out.img_height, 0U, 0U);
        }
    } else {
        // DO NOTHING
    }
}

static inline void PrintExtDataStatus(const msg_vproc_ext_data_status_t *pVprocExtDataStatus)
{
    UINT16 i = 0U;
    ULONG ULAddr;

    AmbaLL_Log(AMBALLLOG_TYPE_MSG,"MSG_VPROC_EXT_DATA_STATUS", 0U, 0U, 0U);
    AmbaLL_Log(AMBALLLOG_TYPE_MSG," channel_id                             = %d  ", pVprocExtDataStatus->channel_id, 0U, 0U);
    AmbaLL_Log(AMBALLLOG_TYPE_MSG," status                                 = %d  ", pVprocExtDataStatus->status, 0U, 0U);
    AmbaLL_Log(AMBALLLOG_TYPE_MSG," pass_step_id                           = %d  ", pVprocExtDataStatus->pass_step_id, 0U, 0U);
    AmbaLL_Log(AMBALLLOG_TYPE_MSG," cap_seq_no                             = %d  ", pVprocExtDataStatus->cap_seq_no, 0U, 0U);
    AmbaLL_Log(AMBALLLOG_TYPE_MSG," batch_cmd_id                           = 0x%X", pVprocExtDataStatus->batch_cmd_id, 0U, 0U);
    LL_PrintBatchInfoId(pVprocExtDataStatus->batch_cmd_id);
    AmbaLL_Log(AMBALLLOG_TYPE_MSG," stitch_tile_num_x                      = %d  ", pVprocExtDataStatus->stitch_tile_num_x, 0U, 0U);
    AmbaLL_Log(AMBALLLOG_TYPE_MSG," stitch_tile_num_y                      = %d  ", pVprocExtDataStatus->stitch_tile_num_y, 0U, 0U);
    AmbaLL_Log(AMBALLLOG_TYPE_MSG," tile_x_idx                             = %d  ", pVprocExtDataStatus->tile_x_idx, 0U, 0U);
    AmbaLL_Log(AMBALLLOG_TYPE_MSG," tile_y_idx                             = %d  ", pVprocExtDataStatus->tile_y_idx, 0U, 0U);
    AmbaLL_Log(AMBALLLOG_TYPE_MSG," tile_start_x                           = %d  ", pVprocExtDataStatus->tile_start_x, 0U, 0U);
    AmbaLL_Log(AMBALLLOG_TYPE_MSG," tile_start_y                           = %d  ", pVprocExtDataStatus->tile_start_y, 0U, 0U);
    AmbaLL_Log(AMBALLLOG_TYPE_MSG," tile_w                                 = %d  ", pVprocExtDataStatus->tile_w, 0U, 0U);
    AmbaLL_Log(AMBALLLOG_TYPE_MSG," tile_h                                 = %d  ", pVprocExtDataStatus->tile_h, 0U, 0U);
    if (pVprocExtDataStatus->pass_step_id == DSP_IMG_PASS_C2Y) {
        AmbaLL_Log(AMBALLLOG_TYPE_MSG," li_c2y                                       ", 0U, 0U, 0U);
        AmbaLL_Log(AMBALLLOG_TYPE_MSG,"   sec2_cfg_addr                        = 0x%X", pVprocExtDataStatus->ext_data.li_c2y.sec2_cfg_addr, 0U, 0U);
        AmbaLL_Log(AMBALLLOG_TYPE_MSG,"   sec2_cfg_len                         = %d  ", pVprocExtDataStatus->ext_data.li_c2y.sec2_cfg_len, 0U, 0U);
        AmbaLL_Log(AMBALLLOG_TYPE_MSG,"   sec4_cfg_addr                        = 0x%X", pVprocExtDataStatus->ext_data.li_c2y.sec4_cfg_addr, 0U, 0U);
        AmbaLL_Log(AMBALLLOG_TYPE_MSG,"   sec4_cfg_len                         = %d  ", pVprocExtDataStatus->ext_data.li_c2y.sec4_cfg_len, 0U, 0U);
        AmbaLL_Log(AMBALLLOG_TYPE_MSG,"   c2y_luma_out.buffer_addr             = 0x%X", pVprocExtDataStatus->ext_data.li_c2y.c2y_luma_out.buffer_addr, 0U, 0U);
        AmbaLL_Log(AMBALLLOG_TYPE_MSG,"   c2y_luma_out.buffer_pitch            = %d  ", pVprocExtDataStatus->ext_data.li_c2y.c2y_luma_out.buffer_pitch, 0U, 0U);
        AmbaLL_Log(AMBALLLOG_TYPE_MSG,"   c2y_luma_out.img_width               = %d  ", pVprocExtDataStatus->ext_data.li_c2y.c2y_luma_out.img_width, 0U, 0U);
        AmbaLL_Log(AMBALLLOG_TYPE_MSG,"   c2y_luma_out.img_height              = %d  ", pVprocExtDataStatus->ext_data.li_c2y.c2y_luma_out.img_height, 0U, 0U);
        AmbaLL_Log(AMBALLLOG_TYPE_MSG,"   c2y_chroma_out.buffer_addr           = 0x%X", pVprocExtDataStatus->ext_data.li_c2y.c2y_chroma_out.buffer_addr, 0U, 0U);
        AmbaLL_Log(AMBALLLOG_TYPE_MSG,"   c2y_chroma_out.buffer_pitch          = %d  ", pVprocExtDataStatus->ext_data.li_c2y.c2y_chroma_out.buffer_pitch, 0U, 0U);
        AmbaLL_Log(AMBALLLOG_TYPE_MSG,"   c2y_chroma_out.img_width             = %d  ", pVprocExtDataStatus->ext_data.li_c2y.c2y_chroma_out.img_width, 0U, 0U);
        AmbaLL_Log(AMBALLLOG_TYPE_MSG,"   c2y_chroma_out.img_height            = %d  ", pVprocExtDataStatus->ext_data.li_c2y.c2y_chroma_out.img_height, 0U, 0U);
        AmbaLL_Log(AMBALLLOG_TYPE_MSG,"   compressed_raw_out.buffer_addr       = 0x%X", pVprocExtDataStatus->ext_data.li_c2y.compressed_raw_out.buffer_addr, 0U, 0U);
        AmbaLL_Log(AMBALLLOG_TYPE_MSG,"   compressed_raw_out.buffer_pitch      = %d  ", pVprocExtDataStatus->ext_data.li_c2y.compressed_raw_out.buffer_pitch, 0U, 0U);
        AmbaLL_Log(AMBALLLOG_TYPE_MSG,"   compressed_raw_out.img_width         = %d  ", pVprocExtDataStatus->ext_data.li_c2y.compressed_raw_out.img_width, 0U, 0U);
        AmbaLL_Log(AMBALLLOG_TYPE_MSG,"   compressed_raw_out.img_height        = %d  ", pVprocExtDataStatus->ext_data.li_c2y.compressed_raw_out.img_height, 0U, 0U);
    } else if (pVprocExtDataStatus->pass_step_id == IMG_PASS_HDR_BLEND) {
        //TBD, CV22 doesn't have it
    } else if (pVprocExtDataStatus->pass_step_id == DSP_IMG_PASS_HIER_LANE_DET) {
        if (pVprocExtDataStatus->ext_data.li_c2y.sec2_cfg_addr/* hier_out_lane_out_daddr */ != 0U) {
            UINT32 U32Val = 0U;
            const msg_vp_hier_out_lane_out_t *pHierLane = NULL;// = (msg_vp_hier_out_lane_out_t *)pVprocExtDataStatus->ext_data.hier_out_lane_out_daddr;
            const void *pVoid;
            UINT32 Rval;

            Rval = dsp_osal_cli2virt(pVprocExtDataStatus->ext_data.li_c2y.sec2_cfg_addr/* hier_out_lane_out_daddr */, &ULAddr);
            if (Rval == OK) {
                dsp_osal_typecast(&pHierLane, &ULAddr);
                dsp_osal_typecast(&pVoid, &pVprocExtDataStatus->ext_data.li_c2y.sec2_cfg_addr/* hier_out_lane_out_daddr */);
                Rval = dsp_osal_cache_invalidate(pVoid, (UINT32)sizeof(msg_vp_hier_out_lane_out_t));
                AmbaMisra_TouchUnused(&Rval);
                AmbaLL_Log(AMBALLLOG_TYPE_MSG," HeirLane                                     ", 0U, 0U, 0U);
                AmbaLL_Log(AMBALLLOG_TYPE_MSG,"   Header                                     ", 0U, 0U, 0U);
                AmbaLL_Log(AMBALLLOG_TYPE_MSG,"     channel_id                         = %d  ", pHierLane->header.channel_id, 0U, 0U);
                AmbaLL_Log(AMBALLLOG_TYPE_MSG,"     cap_seq_no                         = %d  ", pHierLane->header.cap_seq_no, 0U, 0U);
                AmbaLL_Log(AMBALLLOG_TYPE_MSG,"     batch_cmd_id                       = 0x%X", pHierLane->header.batch_cmd_id, 0U, 0U);
                LL_PrintBatchInfoId(pHierLane->header.batch_cmd_id);
                for(i = 0U; i < MAX_TOKEN_ARRAY; i++) {
                    AmbaLL_Log(AMBALLLOG_TYPE_MSG,"     side_band_info[%2d].fov_id           = %d  ", i, pHierLane->header.side_band_info[i].fov_id, 0U);
                    AmbaLL_Log(AMBALLLOG_TYPE_MSG,"     side_band_info[%2d].addr             = 0x%X", i, pHierLane->header.side_band_info[i].fov_private_info_addr, 0U);
                }
                for(i = 0U; i < MAX_ROI_CMD_TAG_NUM; i++) {
                    AmbaLL_Log(AMBALLLOG_TYPE_MSG,"     roi_cmd_tag[%2d]                     = %d  ", i, pHierLane->header.roi_cmd_tag[i], 0U);
                }
                AmbaLL_Log(AMBALLLOG_TYPE_MSG,"     ln_det_hier_mask                   = 0x%X", pHierLane->header.ln_det_hier_mask, 0U, 0U);
                AmbaLL_Log(AMBALLLOG_TYPE_MSG,"     is_hier_out_tile_mode              = %d  ", pHierLane->header.is_hier_out_tile_mode, 0U, 0U);
                AmbaLL_Log(AMBALLLOG_TYPE_MSG,"     vin_cap_time                       = %d  ", pHierLane->header.vin_cap_time, 0U, 0U);
                AmbaLL_Log(AMBALLLOG_TYPE_MSG,"     proc_roi_cmd_time                  = %d  ", pHierLane->header.proc_roi_cmd_time, 0U, 0U);
                AmbaLL_Log(AMBALLLOG_TYPE_MSG,"     c2y_done_time                      = %d  ", pHierLane->header.c2y_done_time, 0U, 0U);
                AmbaLL_Log(AMBALLLOG_TYPE_MSG,"     warp_mctf_done_time                = %d  ", pHierLane->header.warp_mctf_done_time, 0U, 0U);
                for(i = 0U; i < DSP_HIER_NUM; i++) {
                    /*
                     * BufAddr is already ROI start address,
                     * From observation, use same orginal buffer as Non-Crop one,
                     * So BufPitch is the same as NonCrop one,
                     * ImgW/H reflect ROI W/H
                     */
                    AmbaLL_Log(AMBALLLOG_TYPE_MSG,"   LumaHier[%2d].BufAddr                 = 0x%X", i, pHierLane->luma_hier_outs[i].buffer_addr, 0U);
                    AmbaLL_Log(AMBALLLOG_TYPE_MSG,"   LumaHier[%2d].BufPitch                = %d  ", i, pHierLane->luma_hier_outs[i].buffer_pitch, 0U);
                    AmbaLL_Log(AMBALLLOG_TYPE_MSG,"   LumaHier[%2d].ImgWidth                = %d  ", i, pHierLane->luma_hier_outs[i].img_width, 0U);
                    AmbaLL_Log(AMBALLLOG_TYPE_MSG,"   LumaHier[%2d].ImgHeight               = %d  ", i, pHierLane->luma_hier_outs[i].img_height, 0U);
                }
                for(i = 0U; i < DSP_HIER_NUM; i++) {
                    AmbaLL_Log(AMBALLLOG_TYPE_MSG,"   ChromaHier[%2d].BufAddr               = 0x%X", i, pHierLane->chroma_hier_outs[i].buffer_addr, 0U);
                    AmbaLL_Log(AMBALLLOG_TYPE_MSG,"   ChromaHier[%2d].BufPitch              = %d  ", i, pHierLane->chroma_hier_outs[i].buffer_pitch, 0U);
                    AmbaLL_Log(AMBALLLOG_TYPE_MSG,"   ChromaHier[%2d].ImgWidth              = %d  ", i, pHierLane->chroma_hier_outs[i].img_width, 0U);
                    AmbaLL_Log(AMBALLLOG_TYPE_MSG,"   ChromaHier[%2d].ImgHeight             = %d  ", i, pHierLane->chroma_hier_outs[i].img_height, 0U);
                }
                for(i = 0U; i < DSP_HIER_NUM; i++) {
                    //reflect RIO offsetX/Y
                    U32Val = (UINT32)pHierLane->hier_out_offsets[i].x_offset;
                    AmbaLL_Log(AMBALLLOG_TYPE_MSG,"   HierOut[%2d].OffsetX                  = %d  ", i, U32Val, 0U);
                    U32Val = (UINT32)pHierLane->hier_out_offsets[i].y_offset;
                    AmbaLL_Log(AMBALLLOG_TYPE_MSG,"   HierOut[%2d].OffsetY                  = %d  ", i, U32Val, 0U);
                }
                AmbaLL_Log(AMBALLLOG_TYPE_MSG,"   LumaLaneOut.BufAddr              = 0x%X", pHierLane->luma_lane_det_out.buffer_addr, 0U, 0U);
                AmbaLL_Log(AMBALLLOG_TYPE_MSG,"   LumaLaneOut.BufPitch             = %d  ", pHierLane->luma_lane_det_out.buffer_pitch, 0U, 0U);
                AmbaLL_Log(AMBALLLOG_TYPE_MSG,"   LumaLaneOut.ImgWidth             = %d  ", pHierLane->luma_lane_det_out.img_width, 0U, 0U);
                AmbaLL_Log(AMBALLLOG_TYPE_MSG,"   LumaLaneOut.ImgHeight            = %d  ", pHierLane->luma_lane_det_out.img_height, 0U, 0U);
                AmbaLL_Log(AMBALLLOG_TYPE_MSG,"   ChromaLaneOut.BufAddr            = 0x%X", pHierLane->chroma_lane_det_out.buffer_addr, 0U, 0U);
                AmbaLL_Log(AMBALLLOG_TYPE_MSG,"   ChromaLaneOut.BufPitch           = %d  ", pHierLane->chroma_lane_det_out.buffer_pitch, 0U, 0U);
                AmbaLL_Log(AMBALLLOG_TYPE_MSG,"   ChromaLaneOut.ImgWidth           = %d  ", pHierLane->chroma_lane_det_out.img_width, 0U, 0U);
                AmbaLL_Log(AMBALLLOG_TYPE_MSG,"   ChromaLaneOut.ImgHeight          = %d  ", pHierLane->chroma_lane_det_out.img_height, 0U, 0U);
                U32Val = (UINT32)pHierLane->lane_det_out_offset.x_offset;
                AmbaLL_Log(AMBALLLOG_TYPE_MSG,"   LaneOut.OffsetX                  = %d  ", U32Val, 0U, 0U);
                U32Val = (UINT32)pHierLane->lane_det_out_offset.y_offset;
                AmbaLL_Log(AMBALLLOG_TYPE_MSG,"   LaneOut.OffsetY                  = %d  ", U32Val, 0U, 0U);
            }
        } else {
            AmbaLL_Log(AMBALLLOG_TYPE_MSG,"   HeirLane.Addr                    = 0x%X", pVprocExtDataStatus->ext_data.li_c2y.sec2_cfg_addr/* hier_out_lane_out_daddr */, 0U, 0U);
        }
    } else {
        //
    }
}

static inline void PrintMsgOnParseVproc(UINT32 MsgCode, const void *pMsgInfo)
{
    switch (MsgCode) {
    case MSG_VPROC_STATUS:
    {
        const msg_vproc_status_t *pVprocStatus = NULL;
        dsp_osal_typecast(&pVprocStatus, &pMsgInfo);

        if (DSP_ShowCmdMsgLog(MSG_VPROC_STATUS) != 0U) {
            PrintVprocStatus(pVprocStatus);
        }
        break;
    }
    case MSG_VPROC_AAA_STATUS:
    {
        if (DSP_ShowCmdMsgLog(MSG_VPROC_AAA_STATUS) != 0U) {
            const msg_vproc_aaa_status_t *pVprocAaaStatus = NULL;
            dsp_osal_typecast(&pVprocAaaStatus, &pMsgInfo);
            PrintAAAStatus(pVprocAaaStatus);
        }
        break;
    }
    case MSG_VPROC_EXT_STATUS:
    {
        if (DSP_ShowCmdMsgLog(MSG_VPROC_EXT_STATUS) != 0U) {
            const msg_vproc_ext_status_t *pVprocExtStatus = NULL;
            dsp_osal_typecast(&pVprocExtStatus, &pMsgInfo);
            PrintExtStatus(pVprocExtStatus);
        }
        break;
    }
    case MSG_VPROC_EXT_DATA_STATUS:
    {
        if (DSP_ShowCmdMsgLog(MSG_VPROC_EXT_DATA_STATUS) != 0U) {
            const msg_vproc_ext_data_status_t *pVprocExtDataStatus = NULL;
            dsp_osal_typecast(&pVprocExtDataStatus, &pMsgInfo);
            PrintExtDataStatus(pVprocExtDataStatus);
        }
        break;
    }
    case MSG_VPROC_COMP_OUT_STATUS:
    {
        if (DSP_ShowCmdMsgLog(MSG_VPROC_COMP_OUT_STATUS) != 0U) {
            const msg_vproc_comp_out_status_t *pCompOutStatus = NULL;
            dsp_osal_typecast(&pCompOutStatus, &pMsgInfo);
            PrintCompOutStatus(pCompOutStatus);
        }
        break;
    }
    case MSG_VPROC_EFFECT_DATA_STATUS:
    {
        if (DSP_ShowCmdMsgLog(MSG_VPROC_EFFECT_DATA_STATUS) != 0U) {
            const msg_vproc_effect_data_status_t *pEffectDataStatus = NULL;
            dsp_osal_typecast(&pEffectDataStatus, &pMsgInfo);
            PrintEfctData(pEffectDataStatus);
        }
        break;
    }
#ifdef SUPPORT_VPROC_OSD_INSERT
    case MSG_VPROC_DELAYLINE_OSD_STATUS:
    {
        if (DSP_ShowCmdMsgLog(MSG_VPROC_DELAYLINE_OSD_STATUS) != 0U) {
            const msg_vproc_delayline_osd_status_t *pDlyLineOsdStatus = NULL;
            dsp_osal_typecast(&pDlyLineOsdStatus, &pMsgInfo);
            PrintDlyLineOsdStatus(pDlyLineOsdStatus);
        }
        break;
    }
#endif
    default:
    {
        AmbaLL_LogUInt5("[Err] Unexpected MsgCode[0x%X]", MsgCode, 0U, 0U, 0U, 0U);
        break;
    }
    }
}

static inline UINT32 PassStepOnParseVproc(const msg_vproc_status_t *pVprocStatus, ULONG *pULAddr, const UINT32 PassStepID)
{
    UINT32 Rval = OK;

    *pULAddr = 0UL;
    if (((pVprocStatus->tile_x_idx + 1U) == pVprocStatus->stitch_tile_num_x) &&
        ((pVprocStatus->tile_y_idx + 1U) == pVprocStatus->stitch_tile_num_y)) {
        if (pVprocStatus->ext_data_daddr[0U] > 0U) {
            if (pVprocStatus->pass_step_id == PassStepID) {
                Rval = dsp_osal_cli2virt(pVprocStatus->ext_data_daddr[0U], pULAddr);
            }
        }
    }

    return Rval;
}

static inline UINT32 IsValidBuffer(const UINT32 PhysAddr, ULONG *pVirtAddr, const UINT16 Pitch, const UINT16 Width, const UINT16 Height)
{
    UINT32 Rval = 0U;

    if (PhysAddr > 0U) {
        Rval = dsp_osal_cli2virt(PhysAddr, pVirtAddr);
        LL_PrintErrLine(Rval, __func__, __LINE__);
        if ((Rval == OK) &&
            (*pVirtAddr > 0U) &&
            (Pitch > 0U) &&
            (Width > 0U) &&
            (Height > 0U)) {
            Rval = 1U;
        }
    }

    return Rval;
}

static inline void ValidateSyncJobId(UINT32 SyncJobId, UINT8 *pIsValidGrpCmdId, UINT8 *pGrpCmdIdIsBatchId)
{
    UINT32 Rval;
    AMBA_LL_GRP_CMD_CONVERT_s GrpCmdConvInfo = {0};

    if (SyncJobId > 0U) {
        GrpCmdConvInfo.GrpCmdId = SyncJobId;
        Rval = DSP_GiveEvent(LL_EVENT_VALIDATE_GRP_CMD, &GrpCmdConvInfo, 0U);
        LL_PrintErrLine(Rval, __func__, __LINE__);
        if (GrpCmdConvInfo.IsBatchId == (UINT8)0U) {
            *pIsValidGrpCmdId = (UINT8)1U;
            *pGrpCmdIdIsBatchId = (UINT8)0U;
        } else {
            *pIsValidGrpCmdId = (UINT8)0U;
            *pGrpCmdIdIsBatchId = (UINT8)1U;
        }
    } else {
        *pIsValidGrpCmdId = (UINT8)0U;
        *pGrpCmdIdIsBatchId = (UINT8)0U;
    }
}

static inline UINT32 GiveRawEventOnParseVproc(const msg_vproc_status_t *pVprocStatus)
{
    UINT32 Rval = OK, BufCheck;
    const vproc_li_c2y_ext_data_t *pVprocC2yExtData = NULL;
    const vproc_yuyv2y_ext_data_t *pVprocyuyv2yExtData = NULL;
    ULONG ULAddr;
    DSP_RAW_CAPTURED_BUF_s *pVinCapInfo = NULL;
    AMBA_DSP_RAW_BUF_s *pRawBuffer = NULL;
    AMBA_DSP_YUV_INFO_s *pYuvInfo = NULL;
    ULONG EventBufAddr = 0U;
    UINT16 EventBufIdx = 0U;

    Rval = PassStepOnParseVproc(pVprocStatus, &ULAddr, DSP_IMG_PASS_C2Y);
    if ((Rval == OK) && (ULAddr > 0UL)) {
        dsp_osal_typecast(&pVprocC2yExtData, &ULAddr);
    } else {
        Rval = PassStepOnParseVproc(pVprocStatus, &ULAddr, DSP_IMG_PASS_YUYV2Y);
        if ((Rval == OK) && (ULAddr > 0UL)) {
            dsp_osal_typecast(&pVprocyuyv2yExtData, &ULAddr);
        }
    }

    if (pVprocC2yExtData != NULL) {
        /* Comp Raw */
        BufCheck = IsValidBuffer(pVprocC2yExtData->compressed_raw_out.buffer_addr
                                , &ULAddr
                                , pVprocC2yExtData->compressed_raw_out.buffer_pitch
                                , pVprocC2yExtData->compressed_raw_out.img_width
                                , pVprocC2yExtData->compressed_raw_out.img_height);
        if (BufCheck == 1U) {
            Rval = DSP_RequestEventInfoBuf(EVENT_INFO_POOL_PROT_BUF, &EventBufAddr, &EventBufIdx, LL_EVENT_LV_RAW_RDY);
            LL_PrintErrLine(Rval, __func__, __LINE__);
            if (Rval == OK) {
                UINT8 IsValidGrpCmdId = (UINT8)0U, GrpCmdIdIsBatchId = (UINT8)0U;

                dsp_osal_typecast(&pVinCapInfo, &EventBufAddr);
                Rval = dsp_osal_memset(pVinCapInfo, 0, sizeof(DSP_RAW_CAPTURED_BUF_s));
                LL_PrintErrLine(Rval, __func__, __LINE__);

                pRawBuffer = &pVinCapInfo->RawDataRdy.RawBuffer;

                pRawBuffer->BaseAddr         = ULAddr;
                pRawBuffer->Pitch            = pVprocC2yExtData->compressed_raw_out.buffer_pitch;
                pRawBuffer->Window.Width     = pVprocC2yExtData->compressed_raw_out.img_width;
                pRawBuffer->Window.Height    = pVprocC2yExtData->compressed_raw_out.img_height;
                pRawBuffer->Window.OffsetX   = 0U;
                pRawBuffer->Window.OffsetY   = 0U;
                pRawBuffer->Compressed       = 0U;

                pVinCapInfo->RawDataRdy.VinId   = (UINT8)pVprocStatus->channel_id;
                pVinCapInfo->CapSeqU32          = pVprocStatus->cap_seq_no;
                pVinCapInfo->CapPtsU32          = pVprocStatus->vin_cap_done_pts;
                pVinCapInfo->TargetVinId        = 0U;
                pVinCapInfo->CapSliceIdx        = (UINT8)0U;
                pVinCapInfo->CapSliceNum        = (UINT8)1U; // one slice
                pVinCapInfo->IsCompRaw          = (UINT8)1U; // indicate this is proc_raw

                ValidateSyncJobId(pVprocStatus->grp_cmd_buf_id, &IsValidGrpCmdId, &GrpCmdIdIsBatchId);
                if (GrpCmdIdIsBatchId == (UINT8)1U) {
                    pVinCapInfo->BatchCmdId          = pVprocStatus->grp_cmd_buf_id;
                } else {
                    pVinCapInfo->BatchCmdId          = pVprocStatus->batch_cmd_id;
                }

                Rval = DSP_GiveEvent(LL_EVENT_LV_RAW_RDY, pVinCapInfo, EventBufIdx);
                if (Rval != OK) {
                    AmbaLL_LogUInt5("[Err] Msg code (0x%X) GiveEvent Error %u", LL_EVENT_LV_RAW_RDY, __LINE__, 0U, 0U, 0U);
                }
            }
        } else {
            Rval = BufCheck;
        }
    }

    if (pVprocyuyv2yExtData != NULL) {
        /* Luma main */
        BufCheck = IsValidBuffer(pVprocyuyv2yExtData->sec2_luma_out.buffer_addr
                                , &ULAddr
                                , pVprocyuyv2yExtData->sec2_luma_out.buffer_pitch
                                , pVprocyuyv2yExtData->sec2_luma_out.img_width
                                , pVprocyuyv2yExtData->sec2_luma_out.img_height);
        if (BufCheck == 1U) {
            Rval = DSP_RequestEventInfoBuf(EVENT_INFO_POOL_PROT_BUF, &EventBufAddr, &EventBufIdx, LL_EVENT_LV_INT_MAIN_RDY);
            LL_PrintErrLine(Rval, __func__, __LINE__);
            if (Rval == OK) {
                dsp_osal_typecast(&pYuvInfo, &EventBufAddr);
                Rval = dsp_osal_memset(pYuvInfo, 0, sizeof(AMBA_DSP_YUV_INFO_s));
                LL_PrintErrLine(Rval, __func__, __LINE__);

                pYuvInfo->YuvData.Buffer.BaseAddrY        = ULAddr;
                pYuvInfo->YuvData.Buffer.BaseAddrUV       = 0U;
                pYuvInfo->YuvData.Buffer.DataFmt          = AMBA_DSP_YUV400;
                pYuvInfo->YuvData.Buffer.Pitch            = pVprocyuyv2yExtData->sec2_luma_out.buffer_pitch;
                pYuvInfo->YuvData.Buffer.Window.Width     = pVprocyuyv2yExtData->sec2_luma_out.img_width;
                pYuvInfo->YuvData.Buffer.Window.Height    = pVprocyuyv2yExtData->sec2_luma_out.img_height;
                pYuvInfo->YuvData.Buffer.Window.OffsetX   = 0U;
                pYuvInfo->YuvData.Buffer.Window.OffsetY   = 0U;
                pYuvInfo->YuvData.ViewZoneId              = pVprocStatus->channel_id;
                pYuvInfo->CapPtsU32                       = pVprocStatus->vin_cap_done_pts;
                pYuvInfo->YuvPtsU32                       = pVprocStatus->job_done_pts;
                pYuvInfo->CapSeqU32                       = pVprocStatus->cap_seq_no;
                Rval = DSP_GiveEvent(LL_EVENT_LV_INT_MAIN_RDY, pYuvInfo, EventBufIdx);
                if (Rval != OK) {
                    AmbaLL_LogUInt5("[Err] Msg code (0x%X) GiveEvent Error %u", LL_EVENT_LV_INT_MAIN_RDY, __LINE__, 0U, 0U, 0U);
                }
            }
        } else {
            Rval = BufCheck;
        }
    }

    return Rval;
}

static inline UINT32 GiveSideBandEventOnParseVproc(const msg_vp_hier_out_lane_out_t *pHierInfo)
{
    UINT32 Rval;
    UINT16 i;
    AMBA_DSP_SIDEBAND_DATA_RDY_s *pSideBandInfo = NULL;
    ULONG EventBufAddr = 0U;
    UINT16 EventBufIdx = 0U;

    Rval = DSP_RequestEventInfoBuf(EVENT_INFO_POOL_PROT_BUF, &EventBufAddr, &EventBufIdx, LL_EVENT_LV_SIDEBAND_RDY);
    LL_PrintErrLine(Rval, __func__, __LINE__);
    if (Rval == OK) {
        dsp_osal_typecast(&pSideBandInfo, &EventBufAddr);
        Rval = dsp_osal_memset(pSideBandInfo, 0, sizeof(AMBA_DSP_SIDEBAND_DATA_RDY_s));
        LL_PrintErrLine(Rval, __func__, __LINE__);

        pSideBandInfo->ViewZoneID = pHierInfo->header.channel_id;
        pSideBandInfo->CapSeqNo = pHierInfo->header.cap_seq_no;
        for (i = 0U; i < MAX_TOKEN_ARRAY; i++) {
            if (pHierInfo->header.side_band_info[i].fov_private_info_addr != 0U) {
                pSideBandInfo->Info[i].ViewZoneId = pHierInfo->header.side_band_info[i].fov_id;
                Rval = dsp_osal_cli2virt(pHierInfo->header.side_band_info[i].fov_private_info_addr, &pSideBandInfo->Info[i].PrivateInfoAddr);
                LL_PrintErrLine(Rval, __func__, __LINE__);
            }
        }
        Rval = DSP_GiveEvent(LL_EVENT_LV_SIDEBAND_RDY, pSideBandInfo, EventBufIdx);
    }

    return Rval;
}

static inline UINT32 GiveLndtEventOnParseVproc(const msg_vp_hier_out_lane_out_t *pHierInfo)
{
    UINT32 Rval = OK;
    UINT16 LndtBitMask, LndtMask;
    AMBA_DSP_LNDT_DATA_RDY_s *pLndtData = NULL;
    ULONG EventBufAddr = 0U;
    UINT16 EventBufIdx = 0U;

    LndtMask = (UINT16)((UINT32)1U << DSP_HIER_NUM);
    LndtBitMask = (pHierInfo->header.ln_det_hier_mask & LndtMask);
    if (LndtBitMask > 0U) {
        Rval = DSP_RequestEventInfoBuf(EVENT_INFO_POOL_PROT_BUF, &EventBufAddr, &EventBufIdx, LL_EVENT_LV_LNDT_RDY);
        LL_PrintErrLine(Rval, __func__, __LINE__);
        if (Rval == OK) {
            dsp_osal_typecast(&pLndtData, &EventBufAddr);
            Rval = dsp_osal_memset(pLndtData, 0, sizeof(AMBA_DSP_LNDT_DATA_RDY_s));
            LL_PrintErrLine(Rval, __func__, __LINE__);

            pLndtData->ViewZoneId = pHierInfo->header.channel_id;
            pLndtData->YuvBuf.DataFmt = AMBA_DSP_YUV420;
            Rval = dsp_osal_cli2virt(pHierInfo->luma_lane_det_out.buffer_addr, &pLndtData->YuvBuf.BaseAddrY);
            LL_PrintErrLine(Rval, __func__, __LINE__);
            Rval = dsp_osal_cli2virt(pHierInfo->chroma_lane_det_out.buffer_addr, &pLndtData->YuvBuf.BaseAddrUV);
            LL_PrintErrLine(Rval, __func__, __LINE__);
            pLndtData->YuvBuf.Pitch = pHierInfo->luma_lane_det_out.buffer_pitch;
            pLndtData->YuvBuf.Window.Width = pHierInfo->luma_lane_det_out.img_width;
            pLndtData->YuvBuf.Window.Height = pHierInfo->luma_lane_det_out.img_height;
            pLndtData->YuvBuf.Window.OffsetX = 0U;
            pLndtData->YuvBuf.Window.OffsetY = 0U;

            Rval = DSP_GiveEvent(LL_EVENT_LV_LNDT_RDY, pLndtData, EventBufIdx);
        }
    }

    return Rval;
}

static inline UINT32 GivePymdEventOnParseVproc(const msg_vp_hier_out_lane_out_t *pHierInfo)
{
    UINT16 i;
    UINT32 Rval;
    UINT16 HierBitMask, HierMask;
    AMBA_DSP_PYMD_DATA_RDY_s *pPymdData = NULL;
    AMBA_DSP_VPROC_HIER_IDX_MAPPING_s HierIdxMap;
    ULONG EventBufAddr = 0U;
    UINT16 EventBufIdx = 0U;

    Rval = DSP_RequestEventInfoBuf(EVENT_INFO_POOL_PROT_BUF, &EventBufAddr, &EventBufIdx, LL_EVENT_LV_PYRAMID_RDY);
    LL_PrintErrLine(Rval, __func__, __LINE__);
    if (Rval == OK) {
        dsp_osal_typecast(&pPymdData, &EventBufAddr);
        Rval = dsp_osal_memset(pPymdData, 0, sizeof(AMBA_DSP_PYMD_DATA_RDY_s));
        LL_PrintErrLine(Rval, __func__, __LINE__);

        pPymdData->ViewZoneId = pHierInfo->header.channel_id;
        pPymdData->CapSequence = pHierInfo->header.cap_seq_no;
        pPymdData->CapPts = pHierInfo->header.vin_cap_time;
        pPymdData->YuvPts= pHierInfo->header.warp_mctf_done_time;
        HierMask = (UINT16)((UINT32)1U << DSP_HIER_NUM);
        HierMask -= 1U;
        HierBitMask = (pHierInfo->header.ln_det_hier_mask & HierMask);
        pPymdData->Config.IsPolySqrt2 = 0U;
        pPymdData->Config.HierPolyWidth = pHierInfo->luma_hier_outs[1].img_width;
        pPymdData->Config.HierPolyHeight = pHierInfo->luma_hier_outs[1].img_height;
        pPymdData->Config.HierTag = pHierInfo->header.roi_cmd_tag[0];

        HierIdxMap.ChannelId = pHierInfo->header.channel_id;
        for (i = 0U; i<DSP_HIER_NUM; i++) {
            if (DSP_GetU16Bit(HierBitMask, i, 1U) == 1U) {
                HierIdxMap.InputIdx = i;
                HierIdxMap.OutputIdx = 0xFFFFU;
                Rval = DSP_GiveEvent(LL_EVENT_HIER_IDX_MAPPING, &HierIdxMap, 0U);
                if ((Rval == OK) &&
                    (HierIdxMap.OutputIdx != 0xFFFFU)) {
                    pPymdData->Config.HierCropWindow[HierIdxMap.OutputIdx].Width = pHierInfo->luma_hier_outs[i].img_width;
                    pPymdData->Config.HierCropWindow[HierIdxMap.OutputIdx].Height = pHierInfo->luma_hier_outs[i].img_height;
                    pPymdData->Config.HierCropWindow[HierIdxMap.OutputIdx].OffsetX = (UINT16)pHierInfo->hier_out_offsets[i].x_offset;
                    pPymdData->Config.HierCropWindow[HierIdxMap.OutputIdx].OffsetY = (UINT16)pHierInfo->hier_out_offsets[i].y_offset;

                    pPymdData->YuvBuf[HierIdxMap.OutputIdx].DataFmt = AMBA_DSP_YUV420;
                    Rval = dsp_osal_cli2virt(pHierInfo->luma_hier_outs[i].buffer_addr, &pPymdData->YuvBuf[HierIdxMap.OutputIdx].BaseAddrY);
                    LL_PrintErrLine(Rval, __func__, __LINE__);
                    Rval = dsp_osal_cli2virt(pHierInfo->chroma_hier_outs[i].buffer_addr, &pPymdData->YuvBuf[HierIdxMap.OutputIdx].BaseAddrUV);
                    LL_PrintErrLine(Rval, __func__, __LINE__);
                    pPymdData->YuvBuf[HierIdxMap.OutputIdx].Pitch = pHierInfo->luma_hier_outs[i].buffer_pitch;
                    pPymdData->YuvBuf[HierIdxMap.OutputIdx].Window.Width = pHierInfo->luma_hier_outs[i].img_width;
                    pPymdData->YuvBuf[HierIdxMap.OutputIdx].Window.Height = pHierInfo->luma_hier_outs[i].img_height;
                    pPymdData->YuvBuf[HierIdxMap.OutputIdx].Window.OffsetX = 0U;
                    pPymdData->YuvBuf[HierIdxMap.OutputIdx].Window.OffsetY = 0U;

                    DSP_SetU16Bit(&pPymdData->Config.HierBit, HierIdxMap.OutputIdx);
                }
            }
        }
        Rval = DSP_GiveEvent(LL_EVENT_LV_PYRAMID_RDY, pPymdData, EventBufIdx);
    }

    return Rval;
}

static inline UINT32 GiveYuvEventOnParseVprocSyncJob(const UINT8 JobStatus, const UINT32 JobId)
{
    UINT32 Rval = OK;
    AMBA_DSP_LV_SYNC_JOB_INFO_s *pSyncJobInfo = NULL;
    ULONG EventBufAddr = 0U;
    UINT16 EventBufIdx = 0U;

    Rval = DSP_RequestEventInfoBuf(EVENT_INFO_POOL_PROT_BUF, &EventBufAddr, &EventBufIdx, LL_EVENT_LV_SYNC_JOB_RDY);
    LL_PrintErrLine(Rval, __func__, __LINE__);
    if (Rval == OK) {
        dsp_osal_typecast(&pSyncJobInfo, &EventBufAddr);
        Rval = dsp_osal_memset(pSyncJobInfo, 0, sizeof(AMBA_DSP_LV_SYNC_JOB_INFO_s));
        LL_PrintErrLine(Rval, __func__, __LINE__);

        pSyncJobInfo->Status = JobStatus; //TBD for discard
        pSyncJobInfo->JobId = JobId;
        Rval = DSP_GiveEvent(LL_EVENT_LV_SYNC_JOB_RDY, pSyncJobInfo, EventBufIdx);
    }

    return Rval;
}

static inline UINT32 IsValidSection(const vproc_li_warp_mctf_prev_ext_data_t *pVprocWarpExtData,
                                    const UINT32 TgtSectMaskIdx,
                                    const UINT32 VprocSectMask)
{
    UINT32 Rval = 0U;

    if (1U == DSP_GetBit(VprocSectMask, TgtSectMaskIdx, 1U)) {
        switch (TgtSectMaskIdx) {
        case DSP_SEC3_MASK_INDEX:
        {
            if ((pVprocWarpExtData->sec3_luma_out.buffer_addr > 0U) &&
                (pVprocWarpExtData->sec3_chroma_out.buffer_addr > 0U)) {
                Rval = 1U;
            }
            break;
        }
        case DSP_SEC5_MASK_INDEX:
        {
            if ((pVprocWarpExtData->sec5_luma_out.buffer_addr > 0U) &&
                (pVprocWarpExtData->sec5_chroma_out.buffer_addr > 0U)) {
                Rval = 1U;
            }
            break;
        }
        case DSP_SEC6_MASK_INDEX:
        {
            if ((pVprocWarpExtData->sec6_luma_out.buffer_addr > 0U) &&
                (pVprocWarpExtData->sec6_chroma_out.buffer_addr > 0U)) {
                Rval = 1U;
            }
            break;
        }
        case DSP_SEC7_MASK_INDEX:
        {
            if ((pVprocWarpExtData->sec7_luma_out.buffer_addr > 0U) &&
                (pVprocWarpExtData->sec7_chroma_out.buffer_addr > 0U)) {
                Rval = 1U;
            }
            break;
        }
        default:
            Rval = DSP_ERR_0000;
            break;
        }
    }

    return Rval;
}

static inline UINT32 GiveYuvEventOnParseVprocSec(const vproc_li_warp_mctf_prev_ext_data_t *pVprocWarpExtData,
                                                 const AMBA_DSP_YUV_INFO_s *pYuvInfo,
                                                 const UINT32 ChannelId,
                                                 const UINT32 TgtSectMaskIdx,
                                                 const UINT32 VprocSectMask)
{
    UINT32 Rval = OK;
    AMBA_DSP_YUV_INFO_s *pYuvEventInfoMain = NULL, *pYuvEventInfo = NULL;
    ULONG EventBufAddr = 0U, EventMainBufAddr = 0U;
    UINT16 EventBufIdx = 0U, EventMainBufIdx = 0U;

    if (IsValidSection(pVprocWarpExtData, TgtSectMaskIdx, VprocSectMask) == 1U) {
        Rval = DSP_RequestEventInfoBuf(EVENT_INFO_POOL_PROT_BUF, &EventBufAddr, &EventBufIdx, LL_EVENT_LV_YUV_DATA_RDY);
        LL_PrintErrLine(Rval, __func__, __LINE__);

        if (Rval == OK) {
            switch (TgtSectMaskIdx) {
            case DSP_SEC3_MASK_INDEX:
            {
                Rval = DSP_RequestEventInfoBuf(EVENT_INFO_POOL_PROT_BUF, &EventMainBufAddr, &EventMainBufIdx, LL_EVENT_LV_YUV_DATA_RDY);
                LL_PrintErrLine(Rval, __func__, __LINE__);

                if (Rval == OK) {
                    dsp_osal_typecast(&pYuvEventInfo, &EventBufAddr);
                    Rval = dsp_osal_memcpy(pYuvEventInfo, pYuvInfo, sizeof(AMBA_DSP_YUV_INFO_s));
                    LL_PrintErrLine(Rval, __func__, __LINE__);

                    Rval = dsp_osal_cli2virt(pVprocWarpExtData->sec3_luma_out.buffer_addr, &pYuvEventInfo->YuvData.Buffer.BaseAddrY);
                    LL_PrintErrLine(Rval, __func__, __LINE__);
                    Rval = dsp_osal_cli2virt(pVprocWarpExtData->sec3_chroma_out.buffer_addr, &pYuvEventInfo->YuvData.Buffer.BaseAddrUV);
                    LL_PrintErrLine(Rval, __func__, __LINE__);
                    pYuvEventInfo->YuvData.Buffer.Pitch          = pVprocWarpExtData->sec3_luma_out.buffer_pitch;
                    pYuvEventInfo->YuvData.Buffer.Window.Width   = pVprocWarpExtData->sec3_luma_out.img_width;
                    pYuvEventInfo->YuvData.Buffer.Window.Height  = pVprocWarpExtData->sec3_luma_out.img_height;
                    pYuvEventInfo->YuvData.Buffer.Window.OffsetX = 0U;
                    pYuvEventInfo->YuvData.Buffer.Window.OffsetY = 0U;
                    pYuvEventInfo->YuvType = DSP_VPROC_YUV_MAIN;
                    pYuvEventInfo->SrcBufID = DSP_SEC3_CFG_INDEX;
                    // For ME data
                    Rval = dsp_osal_cli2virt(pVprocWarpExtData->sec3_me1_out.buffer_addr, &pYuvEventInfo->Me1Buf.BaseAddr);
                    LL_PrintErrLine(Rval, __func__, __LINE__);
                    pYuvEventInfo->Me1Buf.Pitch          = pVprocWarpExtData->sec3_me1_out.buffer_pitch;
                    pYuvEventInfo->Me1Buf.Window.Height  = pVprocWarpExtData->sec3_me1_out.img_height;
                    pYuvEventInfo->Me1Buf.Window.Width   = pVprocWarpExtData->sec3_me1_out.img_width;
                    pYuvEventInfo->Me1Buf.Window.OffsetX = 0U;
                    pYuvEventInfo->Me1Buf.Window.OffsetY = 0U;

                    /* One for Main Yuv, another for YuvStream */
                    dsp_osal_typecast(&pYuvEventInfoMain, &EventMainBufAddr);
                    Rval = dsp_osal_memcpy(pYuvEventInfoMain, pYuvEventInfo, sizeof(AMBA_DSP_YUV_INFO_s));
                    LL_PrintErrLine(Rval, __func__, __LINE__);
                    pYuvEventInfoMain->YuvData.ViewZoneId = VIEWZONE_ID_INDICATOR_MAIN;
                    Rval = DSP_GiveEvent(LL_EVENT_LV_YUV_DATA_RDY, pYuvEventInfoMain, EventMainBufIdx);

                    pYuvEventInfo->YuvData.ViewZoneId = ChannelId;
                    Rval |= DSP_GiveEvent(LL_EVENT_LV_YUV_DATA_RDY, pYuvEventInfo, EventBufIdx);
                } else {
                    /* EventInfoAddr need to be released
                     * If you request EventInfoAddr but don't call GiveEvent */
                    Rval = DSP_ReleaseEventInfoBuf(EventBufIdx);
                    LL_PrintErrLine(Rval, __func__, __LINE__);
                }
                break;
            }
            case DSP_SEC5_MASK_INDEX:
            {
                dsp_osal_typecast(&pYuvEventInfo, &EventBufAddr);
                Rval = dsp_osal_memcpy(pYuvEventInfo, pYuvInfo, sizeof(AMBA_DSP_YUV_INFO_s));
                LL_PrintErrLine(Rval, __func__, __LINE__);

                Rval = dsp_osal_cli2virt(pVprocWarpExtData->sec5_luma_out.buffer_addr, &pYuvEventInfo->YuvData.Buffer.BaseAddrY);
                LL_PrintErrLine(Rval, __func__, __LINE__);
                Rval = dsp_osal_cli2virt(pVprocWarpExtData->sec5_chroma_out.buffer_addr, &pYuvEventInfo->YuvData.Buffer.BaseAddrUV);
                LL_PrintErrLine(Rval, __func__, __LINE__);
                pYuvEventInfo->YuvData.Buffer.Pitch          = pVprocWarpExtData->sec5_luma_out.buffer_pitch;
                pYuvEventInfo->YuvData.Buffer.Window.Width   = pVprocWarpExtData->sec5_luma_out.img_width;
                pYuvEventInfo->YuvData.Buffer.Window.Height  = pVprocWarpExtData->sec5_luma_out.img_height;
                pYuvEventInfo->YuvData.Buffer.Window.OffsetX = 0U;
                pYuvEventInfo->YuvData.Buffer.Window.OffsetY = 0U;
                pYuvEventInfo->YuvData.ViewZoneId = ChannelId;
                pYuvEventInfo->YuvType = DSP_VPROC_YUV_PREVA;
                pYuvEventInfo->SrcBufID = DSP_SEC5_CFG_INDEX;
                // For ME data
                Rval = dsp_osal_cli2virt(pVprocWarpExtData->sec5_me1_out.buffer_addr, &pYuvEventInfo->Me1Buf.BaseAddr);
                LL_PrintErrLine(Rval, __func__, __LINE__);
                pYuvEventInfo->Me1Buf.Pitch          = pVprocWarpExtData->sec5_me1_out.buffer_pitch;
                pYuvEventInfo->Me1Buf.Window.Height  = pVprocWarpExtData->sec5_me1_out.img_height;
                pYuvEventInfo->Me1Buf.Window.Width   = pVprocWarpExtData->sec5_me1_out.img_width;
                pYuvEventInfo->Me1Buf.Window.OffsetX = 0U;
                pYuvEventInfo->Me1Buf.Window.OffsetY = 0U;
                Rval = DSP_GiveEvent(LL_EVENT_LV_YUV_DATA_RDY, pYuvEventInfo, EventBufIdx);

                break;
            }
            case DSP_SEC6_MASK_INDEX:
            {
                dsp_osal_typecast(&pYuvEventInfo, &EventBufAddr);
                Rval = dsp_osal_memcpy(pYuvEventInfo, pYuvInfo, sizeof(AMBA_DSP_YUV_INFO_s));
                LL_PrintErrLine(Rval, __func__, __LINE__);

                Rval = dsp_osal_cli2virt(pVprocWarpExtData->sec6_luma_out.buffer_addr, &pYuvEventInfo->YuvData.Buffer.BaseAddrY);
                LL_PrintErrLine(Rval, __func__, __LINE__);
                Rval = dsp_osal_cli2virt(pVprocWarpExtData->sec6_chroma_out.buffer_addr, &pYuvEventInfo->YuvData.Buffer.BaseAddrUV);
                LL_PrintErrLine(Rval, __func__, __LINE__);
                pYuvEventInfo->YuvData.Buffer.Pitch          = pVprocWarpExtData->sec6_luma_out.buffer_pitch;
                pYuvEventInfo->YuvData.Buffer.Window.Width   = pVprocWarpExtData->sec6_luma_out.img_width;
                pYuvEventInfo->YuvData.Buffer.Window.Height  = pVprocWarpExtData->sec6_luma_out.img_height;
                pYuvEventInfo->YuvData.Buffer.Window.OffsetX = 0U;
                pYuvEventInfo->YuvData.Buffer.Window.OffsetY = 0U;
                pYuvEventInfo->YuvData.ViewZoneId = ChannelId;
                pYuvEventInfo->YuvType = DSP_VPROC_YUV_PREVB;
                pYuvEventInfo->SrcBufID = DSP_SEC6_CFG_INDEX;
                // For ME data
                Rval = dsp_osal_cli2virt(pVprocWarpExtData->sec6_me1_out.buffer_addr, &pYuvEventInfo->Me1Buf.BaseAddr);
                LL_PrintErrLine(Rval, __func__, __LINE__);
                pYuvEventInfo->Me1Buf.Pitch          = pVprocWarpExtData->sec6_me1_out.buffer_pitch;
                pYuvEventInfo->Me1Buf.Window.Height  = pVprocWarpExtData->sec6_me1_out.img_height;
                pYuvEventInfo->Me1Buf.Window.Width   = pVprocWarpExtData->sec6_me1_out.img_width;
                pYuvEventInfo->Me1Buf.Window.OffsetX = 0U;
                pYuvEventInfo->Me1Buf.Window.OffsetY = 0U;
                Rval = DSP_GiveEvent(LL_EVENT_LV_YUV_DATA_RDY, pYuvEventInfo, EventBufIdx);

                break;
            }
            case DSP_SEC7_MASK_INDEX:
            {
                dsp_osal_typecast(&pYuvEventInfo, &EventBufAddr);
                Rval = dsp_osal_memcpy(pYuvEventInfo, pYuvInfo, sizeof(AMBA_DSP_YUV_INFO_s));
                LL_PrintErrLine(Rval, __func__, __LINE__);

                Rval = dsp_osal_cli2virt(pVprocWarpExtData->sec7_luma_out.buffer_addr, &pYuvEventInfo->YuvData.Buffer.BaseAddrY);
                LL_PrintErrLine(Rval, __func__, __LINE__);
                Rval = dsp_osal_cli2virt(pVprocWarpExtData->sec7_chroma_out.buffer_addr, &pYuvEventInfo->YuvData.Buffer.BaseAddrUV);
                LL_PrintErrLine(Rval, __func__, __LINE__);
                pYuvEventInfo->YuvData.Buffer.Pitch          = pVprocWarpExtData->sec7_luma_out.buffer_pitch;
                pYuvEventInfo->YuvData.Buffer.Window.Width   = pVprocWarpExtData->sec7_luma_out.img_width;
                pYuvEventInfo->YuvData.Buffer.Window.Height  = pVprocWarpExtData->sec7_luma_out.img_height;
                pYuvEventInfo->YuvData.Buffer.Window.OffsetX = 0U;
                pYuvEventInfo->YuvData.Buffer.Window.OffsetY = 0U;
                pYuvEventInfo->YuvData.ViewZoneId = ChannelId;
                pYuvEventInfo->YuvType = DSP_VPROC_YUV_PREVC;
                pYuvEventInfo->SrcBufID = DSP_SEC7_CFG_INDEX;
                // For ME data
                Rval = dsp_osal_cli2virt(pVprocWarpExtData->sec7_me1_out.buffer_addr, &pYuvEventInfo->Me1Buf.BaseAddr);
                LL_PrintErrLine(Rval, __func__, __LINE__);
                pYuvEventInfo->Me1Buf.Pitch          = pVprocWarpExtData->sec7_me1_out.buffer_pitch;
                pYuvEventInfo->Me1Buf.Window.Height  = pVprocWarpExtData->sec7_me1_out.img_height;
                pYuvEventInfo->Me1Buf.Window.Width   = pVprocWarpExtData->sec7_me1_out.img_width;
                pYuvEventInfo->Me1Buf.Window.OffsetX = 0U;
                pYuvEventInfo->Me1Buf.Window.OffsetY = 0U;
                Rval = DSP_GiveEvent(LL_EVENT_LV_YUV_DATA_RDY, pYuvEventInfo, EventBufIdx);

                break;
            }
            default:
                Rval = DSP_ERR_0000;
                break;
            }
        }
    }

    return Rval;
}

static inline UINT32 GiveYuvExtEventOnParseVproc(const msg_vproc_status_t *pVprocStatus)
{
    UINT32 Rval = OK;
    const msg_vp_hier_out_lane_out_t *pHierInfo;
    ULONG ULAddr = 0U;
    UINT8 IsValidGrpCmdId = (UINT8)0U, GrpCmdIdIsBatchId = (UINT8)0U;

    if (pVprocStatus->ext_data_daddr[1U] > 0U) {
        ULONG ULVirtAddrAlign64;
        UINT32 SizeAlign64;
        const void *pVoid;

        Rval = dsp_osal_cli2virt(pVprocStatus->ext_data_daddr[1U], &ULAddr);
        if (Rval == OK) {
            dsp_osal_typecast(&pHierInfo, &ULAddr);

            ULVirtAddrAlign64 = (ULONG)ALIGN_NUM64((UINT64)ULAddr, 64UL);
            SizeAlign64 = ALIGN_NUM((UINT32)sizeof(msg_vp_hier_out_lane_out_t), 64U) + 64U;
            dsp_osal_typecast(&pVoid, &ULVirtAddrAlign64);
            Rval = dsp_osal_cache_invalidate(pVoid, SizeAlign64);
            if (Rval != OK) {
                AmbaLL_Log(AMBALLLOG_TYPE_DBG, "[%d][%d] Cache_DataInvalidate fail[0x%X]",
                        pHierInfo->header.channel_id, pHierInfo->header.cap_seq_no, Rval);
            }

            Rval |= GivePymdEventOnParseVproc(pHierInfo);
            Rval |= GiveLndtEventOnParseVproc(pHierInfo);
            Rval |= GiveSideBandEventOnParseVproc(pHierInfo);
        }
    }

    if (Rval == OK) {
        ValidateSyncJobId(pVprocStatus->grp_cmd_buf_id, &IsValidGrpCmdId, &GrpCmdIdIsBatchId);
        if (IsValidGrpCmdId == (UINT8)1U) {
            Rval = GiveYuvEventOnParseVprocSyncJob(0U/*TBD for discard*/, pVprocStatus->grp_cmd_buf_id);
        }
    }

    if (Rval == OK) {
        AMBA_LL_BATCH_IDX_INFO_MAPPING_s BatchQInfo = {0};

        if (GrpCmdIdIsBatchId == (UINT8)1U) {
            BatchQInfo.BatchCmdId = pVprocStatus->grp_cmd_buf_id;
        } else {
            BatchQInfo.BatchCmdId = pVprocStatus->batch_cmd_id;
        }

        if (BatchQInfo.BatchCmdId > 0U) {
            Rval = DSP_GiveEvent(LL_EVENT_BATCH_INFO_MAPPING, &BatchQInfo, 0U);
            LL_PrintErrLine(Rval, __func__, __LINE__);
            if (BatchQInfo.YuvStrmGrpId > 0U) {
                Rval = GiveYuvEventOnParseVprocSyncJob(0U/*TBD for discard*/, BatchQInfo.YuvStrmGrpId);
            }
        }
    }

    return Rval;
}

static inline UINT32 GiveYuvEventOnParseVproc(const msg_vproc_status_t *pVprocStatus)
{
    const vproc_li_warp_mctf_prev_ext_data_t *pVprocWarpExtData = NULL;
    UINT32 Rval = OK;
    ULONG ULAddr = 0U;
    AMBA_DSP_YUV_INFO_s YuvInfo = {0};
    AMBA_DSP_YUV_INFO_s *pYuvEventInfoMain = NULL, *pYuvEventInfo = NULL;
    ULONG EventBufAddr = 0U, EventMainBufAddr = 0U;
    UINT16 EventBufIdx = 0U, EventMainBufIdx = 0U;

    /* Obtain data from the msg in last tile */
    Rval = PassStepOnParseVproc(pVprocStatus, &ULAddr, DSP_IMG_PASS_WARP);
    if ((Rval == OK) && (ULAddr > 0UL)) {
        dsp_osal_typecast(&pVprocWarpExtData, &ULAddr);
    }

    if (pVprocWarpExtData != NULL) {
        Rval = DSP_RequestEventInfoBuf(EVENT_INFO_POOL_PROT_BUF, &EventBufAddr, &EventBufIdx, LL_EVENT_LV_YUV_DATA_RDY);
        LL_PrintErrLine(Rval, __func__, __LINE__);
        if (Rval == OK) {
            Rval = DSP_RequestEventInfoBuf(EVENT_INFO_POOL_PROT_BUF, &EventMainBufAddr, &EventMainBufIdx, LL_EVENT_LV_YUV_DATA_RDY);
            LL_PrintErrLine(Rval, __func__, __LINE__);
            if (Rval == OK) {
                UINT8 IsValidGrpCmdId = (UINT8)0U, GrpCmdIdIsBatchId = (UINT8)0U;

                /* Actual MainOut shall be MctsLuma+MctfChroma */
                Rval = dsp_osal_cli2virt(pVprocWarpExtData->mcts_luma_out.buffer_addr, &YuvInfo.YuvData.Buffer.BaseAddrY);
                LL_PrintErrLine(Rval, __func__, __LINE__);
                Rval = dsp_osal_cli2virt(pVprocWarpExtData->mctf_chroma_out.buffer_addr, &YuvInfo.YuvData.Buffer.BaseAddrUV);
                LL_PrintErrLine(Rval, __func__, __LINE__);
                YuvInfo.YuvData.Buffer.Pitch          = pVprocWarpExtData->mcts_luma_out.buffer_pitch;
                YuvInfo.YuvData.Buffer.Window.Width   = pVprocWarpExtData->mcts_luma_out.img_width;
                YuvInfo.YuvData.Buffer.Window.Height  = pVprocWarpExtData->mcts_luma_out.img_height;
                YuvInfo.YuvData.Buffer.Window.OffsetX = 0U;
                YuvInfo.YuvData.Buffer.Window.OffsetY = 0U;
                YuvInfo.ChannelId = pVprocStatus->channel_id;
                YuvInfo.YuvData.CapSequence = pVprocStatus->cap_seq_no;
                YuvInfo.CapSeqU32 = pVprocStatus->cap_seq_no;
                YuvInfo.YuvData.YuvSequence = pVprocStatus->vproc_seq_no;
                YuvInfo.YuvData.VinId = pVprocStatus->vin_id;
                YuvInfo.YuvData.SourceViewBit = 0U;

                // For Pts
                YuvInfo.YuvPtsU32 = pVprocStatus->job_done_pts;
                YuvInfo.CapPtsU32 = pVprocStatus->vin_cap_done_pts;
                YuvInfo.YuvType = DSP_VPROC_YUV_MAIN;
                YuvInfo.ExtBufMask = pVprocStatus->ext_buf_idc;
                // For update CtxViewZone BatchQRptr
                ValidateSyncJobId(pVprocStatus->grp_cmd_buf_id, &IsValidGrpCmdId, &GrpCmdIdIsBatchId);
                if (GrpCmdIdIsBatchId == (UINT8)1U) {
                    YuvInfo.BatchCmdId = pVprocStatus->grp_cmd_buf_id;
                } else {
                    YuvInfo.BatchCmdId = pVprocStatus->batch_cmd_id;
                }
                YuvInfo.SrcBufID = DSP_MCTF_CFG_INDEX;
                // For ME data
                Rval = dsp_osal_cli2virt(pVprocWarpExtData->sec3_me1_out.buffer_addr, &YuvInfo.Me1Buf.BaseAddr);
                LL_PrintErrLine(Rval, __func__, __LINE__);
                YuvInfo.Me1Buf.Pitch          = pVprocWarpExtData->sec3_me1_out.buffer_pitch;
                YuvInfo.Me1Buf.Window.Height  = pVprocWarpExtData->sec3_me1_out.img_height;
                YuvInfo.Me1Buf.Window.Width   = pVprocWarpExtData->sec3_me1_out.img_width;
                YuvInfo.Me1Buf.Window.OffsetX = 0U;
                YuvInfo.Me1Buf.Window.OffsetY = 0U;

                /* One for Main Yuv, another for YuvStream */
                dsp_osal_typecast(&pYuvEventInfoMain, &EventMainBufAddr);
                Rval = dsp_osal_memcpy(pYuvEventInfoMain, &YuvInfo, sizeof(AMBA_DSP_YUV_INFO_s));
                LL_PrintErrLine(Rval, __func__, __LINE__);
                pYuvEventInfoMain->YuvData.ViewZoneId = VIEWZONE_ID_INDICATOR_MAIN;
                Rval = DSP_GiveEvent(LL_EVENT_LV_YUV_DATA_RDY, pYuvEventInfoMain, EventMainBufIdx);

                dsp_osal_typecast(&pYuvEventInfo, &EventBufAddr);
                Rval |= dsp_osal_memcpy(pYuvEventInfo, &YuvInfo, sizeof(AMBA_DSP_YUV_INFO_s));
                LL_PrintErrLine(Rval, __func__, __LINE__);
                pYuvEventInfo->YuvData.ViewZoneId = pVprocStatus->channel_id;
                Rval |= DSP_GiveEvent(LL_EVENT_LV_YUV_DATA_RDY, pYuvEventInfo, EventBufIdx);

                // Sec3
                if (Rval == OK) {
                    Rval = GiveYuvEventOnParseVprocSec(pVprocWarpExtData, &YuvInfo, pVprocStatus->channel_id, DSP_SEC3_MASK_INDEX, pVprocStatus->sect_mask);
                }
                // Sec5
                if (Rval == OK) {
                    Rval = GiveYuvEventOnParseVprocSec(pVprocWarpExtData, &YuvInfo, pVprocStatus->channel_id, DSP_SEC5_MASK_INDEX, pVprocStatus->sect_mask);
                }
                // Sec6
                if (Rval == OK) {
                    Rval = GiveYuvEventOnParseVprocSec(pVprocWarpExtData, &YuvInfo, pVprocStatus->channel_id, DSP_SEC6_MASK_INDEX, pVprocStatus->sect_mask);
                }
                // Sec7
                if (Rval == OK) {
                    Rval = GiveYuvEventOnParseVprocSec(pVprocWarpExtData, &YuvInfo, pVprocStatus->channel_id, DSP_SEC7_MASK_INDEX, pVprocStatus->sect_mask);
                }

                if (Rval != OK) {
                    AmbaLL_LogUInt5("[Err] Msg code (0x%X) GiveEventError for Yuv", MSG_VPROC_STATUS, 0U, 0U, 0U, 0U);
                }
            } else {
                /* EventInfoAddr need to be released
                 * If you request EventInfoAddr but don't call GiveEvent */
                Rval = DSP_ReleaseEventInfoBuf(EventBufIdx);
                LL_PrintErrLine(Rval, __func__, __LINE__);
            }
        }

        if (Rval == OK) {
            Rval = GiveYuvExtEventOnParseVproc(pVprocStatus);
        }
    }

    return Rval;
}

static inline UINT32 GiveAaaEventOnParseVprocRgbIn(const UINT16 ChId,
                                                   const UINT16 CfaMode)
{
    UINT32 Rval = OK;
    IDSP_INFO_s *pIdspInfo = NULL;
    ULONG EventBufAddr = 0U;
    UINT16 i, EventBufIdx = 0U;
    UINT8 EventInfoCat = 0U;
#ifdef USE_STAT_IDSP_BUF
    DSP_MEM_BLK_t IdspInfoMemBlk = {0};
#endif

#ifdef USE_STAT_IDSP_BUF
    Rval = DSP_GetStatBuf(DSP_STAT_BUF_IDSP, ChId, &IdspInfoMemBlk);
    LL_PrintErrLine(Rval, __func__, __LINE__);
    dsp_osal_typecast(&pIdspInfo, &IdspInfoMemBlk.Base);
#else
    pIdspInfo = &AmbaDSP_IdspInfo[ChId];
#endif

    if (CfaMode != VIN_INPUT_MODE_YUV) {
        for (i = 0U; i < DSP_VIN_HIST_NUM; i++) {
            if (ValidVinHist[ChId][i] != 0U) {
                const AMBA_IK_CFA_HIST_STAT_s *pHistStat;

                if (i == DSP_VIN_HIST_MAIN) {
                    EventInfoCat = EVENT_INFO_POOL_STAT_HIST0;
                } else if (i == DSP_VIN_HIST_HDR) {
                    EventInfoCat = EVENT_INFO_POOL_STAT_HIST1;
                } else {
                    EventInfoCat = EVENT_INFO_POOL_STAT_HIST2;
                }
                Rval = DSP_RequestEventInfoBuf(EventInfoCat, &EventBufAddr, &EventBufIdx, LL_EVENT_LV_HIST_AAA_RDY);
                LL_PrintErrLine(Rval, __func__, __LINE__);

                if (Rval == DSP_ERR_NONE) {
                    CfaHistTransInfo[ChId][i].DstAddr = EventBufAddr;
                    pIdspInfo->HistAAA[i].DstAddr = EventBufAddr;
                    dsp_osal_typecast(&pHistStat, &CfaHistTransInfo[ChId][i].DstAddr);

                    if (CfaMode == VIN_INPUT_MODE_RGBIR) {
                        CfaHistTransInfo[ChId][i].Type = DSP_HIST_VIDEO_RGBIR;
                    } else {
                        CfaHistTransInfo[ChId][i].Type = DSP_HIST_VIDEO;
                    }

                    Rval = dsp_osal_aaa_transfer(&CfaHistTransInfo[ChId][i]);
                    if (Rval == OK) {
                        //Update ChanId
                        AMBA_IK_CFA_HIST_STAT_s *pDst;

                        dsp_osal_typecast(&pDst, &CfaHistTransInfo[ChId][i].DstAddr);
                        pDst->Header.VinStatsType = (UINT8)i;
                        pDst->Header.ChannelIndex = (UINT8)ChId;
                        Rval = DSP_GiveEvent(LL_EVENT_LV_HIST_AAA_RDY, pHistStat, EventBufIdx);
                        if (Rval != OK) {
                            AmbaLL_LogUInt5("[Err][%d] [%d]LL_EVENT_LV_HIST_AAA_RDY[0x%X]",
                                        __LINE__, ChId, Rval, 0U, 0U);
                        }
                    } else {
                        AmbaLL_LogUInt5("[Err][%d]Hist[%d] Transfer[0x%X]", __LINE__, ChId, Rval, 0U, 0U);
                        /* If you request EventInfoBuf but don't call GiveEvent
                         * Needs to release EventInfoBuf */
                        Rval = DSP_ReleaseEventInfoBuf(EventBufIdx);
                        LL_PrintErrLine(Rval, __func__, __LINE__);
                    }
                }
            }
        }
    }
    return Rval;
}

static inline UINT32 GiveAaaEventOnParseVproc(const msg_vproc_aaa_status_t *pVprocAaaStatus)
{
    UINT32 Rval = OK;
    ULONG EventBufAddr = 0U;
    UINT16 EventBufIdx = 0U;
    UINT16 ChId = pVprocAaaStatus->channel_id;

    /*
     * Only update Stat when two conditions meet
     *  1) CapSeqNo increment
     *  2) Whole tile is collected
     */
    if ((AAADataValidData[ChId] == AAADataValidPattern[ChId])) {
        const AMBA_IK_CFA_3A_DATA_s *pCfaData;
        const AMBA_IK_PG_3A_DATA_s *pPgData;
        AMBA_DSP_VPROC_CFA_MODE_MAPPING_s CfaMode = {0};
        IDSP_INFO_s *pIdspInfo = NULL;
#ifdef USE_STAT_IDSP_BUF
        DSP_MEM_BLK_t IdspInfoMemBlk = {0};
#endif

#ifdef USE_STAT_IDSP_BUF
        Rval = DSP_GetStatBuf(DSP_STAT_BUF_IDSP, ChId, &IdspInfoMemBlk);
        LL_PrintErrLine(Rval, __func__, __LINE__);
        dsp_osal_typecast(&pIdspInfo, &IdspInfoMemBlk.Base);
#else
        pIdspInfo = &AmbaDSP_IdspInfo[ChId];
#endif

        CfaMode.ChannelId = ChId;
        Rval = DSP_GiveEvent(LL_EVENT_CFA_MODE_MAPPING, &CfaMode, 0U);
        if (Rval != OK) {
            AmbaLL_LogUInt5("[Err][%d] [%d]CFA_MODE_MAPPING[0x%X]",
                    __LINE__, ChId, Rval, 0U, 0U);
            CfaMode.CfaMode = VIN_INPUT_MODE_RGGB;
        }

        if (CfaMode.CfaMode != VIN_INPUT_MODE_YUV) {
            //Invoke IK to merge
            Rval = DSP_RequestEventInfoBuf(EVENT_INFO_POOL_STAT_CFA, &EventBufAddr, &EventBufIdx, LL_EVENT_LV_CFA_AAA_RDY);
            LL_PrintErrLine(Rval, __func__, __LINE__);

            if (Rval == DSP_ERR_NONE) {
                CfaTransInfo[ChId].DstAddr = EventBufAddr;
                pIdspInfo->CfaAAA.DstAddr = EventBufAddr;
                dsp_osal_typecast(&pCfaData, &CfaTransInfo[ChId].DstAddr);
                CfaTransInfo[ChId].Type = DSP_3A_VIDEO;
                Rval = dsp_osal_aaa_transfer(&CfaTransInfo[ChId]);
                if (Rval == OK) {
                    //Update ChanId
                    AMBA_IK_CFA_3A_DATA_s *pDst;

                    dsp_osal_typecast(&pDst, &CfaTransInfo[ChId].DstAddr);
                    pDst->Header.ChanIndex = (UINT8)ChId;
                    pDst->Header.RawPicSeqNum = pVprocAaaStatus->cap_seq_no;
                    pDst->Header.reserved4[0U] = pVprocAaaStatus->batch_cmd_id; //hide batch_cmd_id and remove later
                    Rval = DSP_GiveEvent(LL_EVENT_LV_CFA_AAA_RDY, pCfaData, EventBufIdx);
                    if (Rval != OK) {
                        AmbaLL_LogUInt5("[Err][%d] [%d]LL_EVENT_LV_CFA_AAA_RDY[0x%X]",
                                __LINE__, ChId, Rval, 0U, 0U);
                    }
                } else {
                    AmbaLL_LogUInt5("[Err][%d]CFA_AAA[%d] Transfer[0x%X]", __LINE__, ChId, Rval, 0U, 0U);
                    /* If you request EventInfoBuf but don't call GiveEvent
                     * Needs to release EventInfoBuf */
                    Rval = DSP_ReleaseEventInfoBuf(EventBufIdx);
                    LL_PrintErrLine(Rval, __func__, __LINE__);
                }
            }
        }

        Rval = DSP_RequestEventInfoBuf(EVENT_INFO_POOL_STAT_PG, &EventBufAddr, &EventBufIdx, LL_EVENT_LV_PG_AAA_RDY);
        LL_PrintErrLine(Rval, __func__, __LINE__);

        if (Rval == DSP_ERR_NONE) {
            PgTransInfo[ChId].DstAddr = EventBufAddr;
            pIdspInfo->PgAAA.DstAddr = EventBufAddr;
            dsp_osal_typecast(&pPgData, &PgTransInfo[ChId].DstAddr);
            PgTransInfo[ChId].Type = DSP_3A_VIDEO_PG;
            Rval = dsp_osal_aaa_transfer(&PgTransInfo[ChId]);
            if (Rval == OK) {
                //Update ChanId
                AMBA_IK_PG_3A_DATA_s *pDst;

                dsp_osal_typecast(&pDst, &PgTransInfo[ChId].DstAddr);
                pDst->Header.ChanIndex = (UINT8)ChId;
                pDst->Header.RawPicSeqNum = pVprocAaaStatus->cap_seq_no;
                Rval = DSP_GiveEvent(LL_EVENT_LV_PG_AAA_RDY, pPgData, EventBufIdx);
                if (Rval != OK) {
                    AmbaLL_LogUInt5("[Err][%d] [%d]LL_EVENT_LV_PG_AAA_RDY[0x%X]",
                                __LINE__, ChId, Rval, 0U, 0U);
                }
            } else {
                AmbaLL_LogUInt5("[Err][%d]PG_AAA[%d] Transfer[0x%X]", __LINE__, ChId, Rval, 0U, 0U);
                /* If you request EventInfoBuf but don't call GiveEvent
                 * Needs to release EventInfoBuf */
                Rval = DSP_ReleaseEventInfoBuf(EventBufIdx);
                LL_PrintErrLine(Rval, __func__, __LINE__);
            }
        }

        Rval = GiveAaaEventOnParseVprocRgbIn(ChId,
                                             CfaMode.CfaMode);
        if (Rval != OK) {
            AmbaLL_LogUInt5("[Err][%d] [%d] GiveAaaEventOnParseVprocRgbIn [0x%X]",
                    __LINE__, ChId, Rval, 0U, 0U);
        }
    }

    return OK;
}

static inline UINT32 GiveEfctEventOnParseVproc(const msg_vproc_effect_data_status_t* pEffectDataStatus)
{
    UINT32 Rval;
    AMBA_DSP_YUV_INFO_s YuvInfo = {0};
    AMBA_DSP_YUV_INFO_s *pYuvInfo = NULL;
    ULONG EventBufAddr = 0U;
    UINT16 EventBufIdx = 0U;

    Rval = dsp_osal_cli2virt(pEffectDataStatus->effect_luma.buffer_addr, &YuvInfo.YuvData.Buffer.BaseAddrY);
    LL_PrintErrLine(Rval, __func__, __LINE__);
    Rval = dsp_osal_cli2virt(pEffectDataStatus->effect_chroma.buffer_addr, &YuvInfo.YuvData.Buffer.BaseAddrUV);
    LL_PrintErrLine(Rval, __func__, __LINE__);
    YuvInfo.YuvData.Buffer.Pitch          = pEffectDataStatus->effect_luma.buffer_pitch;
    YuvInfo.YuvData.Buffer.Window.Width   = pEffectDataStatus->effect_luma.img_width;
    YuvInfo.YuvData.Buffer.Window.Height  = pEffectDataStatus->effect_luma.img_height;
    YuvInfo.YuvData.Buffer.Window.OffsetX = 0U;
    YuvInfo.YuvData.Buffer.Window.OffsetY = 0U;
    YuvInfo.YuvData.ViewZoneId = pEffectDataStatus->channel_id;
    YuvInfo.ChannelId = pEffectDataStatus->channel_id;
    YuvInfo.YuvData.CapSequence = pEffectDataStatus->cap_seq_no;
    YuvInfo.CapSeqU32 = pEffectDataStatus->cap_seq_no;
    // For Pts
    YuvInfo.YuvPtsU32 = pEffectDataStatus->vcap_audio_clk_counter;
    YuvInfo.CapPtsU32 = 0U;
    YuvInfo.YuvData.YuvSequence = pEffectDataStatus->vproc_seq_no;
    YuvInfo.YuvData.VinId = 0xFFFFU;
    YuvInfo.YuvData.SourceViewBit = (UINT16)pEffectDataStatus->source_fov_bitmask;
    if (pEffectDataStatus->stream_id == DSP_VPROC_OUT_STRM_PREV_A) {
        YuvInfo.YuvType = DSP_VPROC_YUV_PREVA;
    } else if (pEffectDataStatus->stream_id == DSP_VPROC_OUT_STRM_PREV_B) {
        YuvInfo.YuvType = DSP_VPROC_YUV_PREVB;
    } else if (pEffectDataStatus->stream_id == DSP_VPROC_OUT_STRM_PREV_C) {
        YuvInfo.YuvType = DSP_VPROC_YUV_PREVC;
    } else {
        YuvInfo.YuvType = DSP_VPROC_YUV_MAIN;
    }
    if (pEffectDataStatus->is_ext == 1U) {
        DSP_SetBit(&YuvInfo.ExtBufMask, (UINT32)DSP_VprocPinVprocMemTypeMap[YuvInfo.YuvType]);
    }

    YuvInfo.BatchCmdId = 0U;
    YuvInfo.SrcBufID = 0xFFU;
    // For ME data
    Rval = dsp_osal_cli2virt(pEffectDataStatus->effect_me1.buffer_addr, &YuvInfo.Me1Buf.BaseAddr);
    LL_PrintErrLine(Rval, __func__, __LINE__);
    YuvInfo.Me1Buf.Pitch          = pEffectDataStatus->effect_me1.buffer_pitch;
    YuvInfo.Me1Buf.Window.Height  = pEffectDataStatus->effect_me1.img_height;
    YuvInfo.Me1Buf.Window.Width   = pEffectDataStatus->effect_me1.img_width;
    YuvInfo.Me1Buf.Window.OffsetX = 0U;
    YuvInfo.Me1Buf.Window.OffsetY = 0U;

    Rval = DSP_GiveEvent(LL_EVENT_EFCT_YUV_DATA_RDY, &YuvInfo, 0U);
    if (Rval == OK) {
        Rval = DSP_RequestEventInfoBuf(EVENT_INFO_POOL_PROT_BUF, &EventBufAddr, &EventBufIdx, LL_EVENT_LV_YUV_DATA_RDY);
        LL_PrintErrLine(Rval, __func__, __LINE__);
        if (Rval == OK) {
            dsp_osal_typecast(&pYuvInfo, &EventBufAddr);
            Rval = dsp_osal_memcpy(pYuvInfo, &YuvInfo, sizeof(AMBA_DSP_YUV_INFO_s));
            LL_PrintErrLine(Rval, __func__, __LINE__);
            pYuvInfo->YuvData.ViewZoneId = VIEWZONE_ID_INDICATOR_EFFECT;
            Rval |= DSP_GiveEvent(LL_EVENT_LV_YUV_DATA_RDY, pYuvInfo, EventBufIdx);
        }
    }

    return Rval;
}

#ifdef SUPPORT_VPROC_OSD_INSERT
static inline UINT32 GiveDlyLineOsdEventOnParseVproc(const msg_vproc_delayline_osd_status_t *pDlyLineOsdStatus)
{
    UINT32 Rval = DSP_ERR_NONE;
    AMBA_DSP_VIDEO_BLEND_INFO_s *pVdoBldInfo = NULL;
    ULONG EvntBufAddr = 0U;
    UINT16 EvntBufIdx = 0U;

    Rval = DSP_RequestEventInfoBuf(EVENT_INFO_POOL_PROT_BUF, &EvntBufAddr, &EvntBufIdx, LL_EVENT_VIDEO_BLEND_RDY);
    LL_PrintErrLine(Rval, __func__, __LINE__);

    if (Rval == DSP_ERR_NONE) {
        dsp_osal_typecast(&pVdoBldInfo, &EvntBufAddr);
        pVdoBldInfo->Id = 0U; //fill encode stream-id by preproc
        pVdoBldInfo->Status = pDlyLineOsdStatus->osd_sync_error;
        pVdoBldInfo->UserData = pDlyLineOsdStatus->user_id;
        Rval = DSP_GiveEvent(LL_EVENT_VIDEO_BLEND_RDY, pVdoBldInfo, EvntBufIdx);
    }

    return Rval;
}
#endif

static inline UINT32 GiveHierEventOnParseVproc(const msg_vproc_ext_data_status_t* pVprocExtDataStatus)
{
    UINT32 Rval = OK;
    AMBA_DSP_PYMD_DATA_RDY_s *pPymdInfo = NULL;
    ULONG EventBufAddr = 0U;
    UINT16 EventBufIdx = 0U;
    UINT16 i;
    //ULONG ULAddr;
    //const void *pVoid = NULL;

    if ((pVprocExtDataStatus->pass_step_id == DSP_IMG_PASS_HIER_LANE_DET) && \
        (pVprocExtDataStatus->ext_data.li_c2y.sec2_cfg_addr/* hier_out_lane_out_daddr */ != 0U)) {
        Rval = DSP_RequestEventInfoBuf(EVENT_INFO_POOL_PROT_BUF, &EventBufAddr, &EventBufIdx, LL_EVENT_LV_PYRAMID_RDY);
        LL_PrintErrLine(Rval, __func__, __LINE__);
        if (Rval == OK) {
            const msg_vp_hier_out_lane_out_t *pHierLane = NULL;
            AMBA_DSP_LIVEVIEW_PYRAMID_s *pPymdData = NULL;
            AMBA_DSP_VPROC_HIER_IDX_MAPPING_s HierIdxMap = {0};

            dsp_osal_typecast(&pPymdInfo, &EventBufAddr);
            Rval = dsp_osal_memset(pPymdInfo, 0, sizeof(AMBA_DSP_PYMD_DATA_RDY_s));
            LL_PrintErrLine(Rval, __func__, __LINE__);
            pPymdData = &pPymdInfo->Config;

            dsp_osal_typecast(&pHierLane, &pVprocExtDataStatus->ext_data.li_c2y.sec2_cfg_addr/* hier_out_lane_out_daddr */);
//FIXME no need cache invalidate?
            //dsp_osal_cli2virt(pVprocExtDataStatus->ext_data.hier_out_lane_out_daddr, &ULAddr);
            //dsp_osal_typecast(&pVoid, &ULAddr);
            //Rval = dsp_osal_cache_invalidate(pVoid, sizeof(msg_vp_hier_out_lane_out_t));
            //AmbaMisra_TouchUnused(&Rval);
            pPymdInfo->ViewZoneId = pHierLane->header.channel_id;
            pPymdInfo->CapPts = pHierLane->header.vin_cap_time;
            pPymdInfo->YuvPts = pHierLane->header.warp_mctf_done_time;
            pPymdData->HierBit = pHierLane->header.ln_det_hier_mask & 0x1FFFU;
            pPymdData->IsPolySqrt2 = 0U;
            pPymdData->HierPolyWidth = pHierLane->luma_hier_outs[1].img_width;
            pPymdData->HierPolyHeight = pHierLane->luma_hier_outs[1].img_height;
            pPymdData->HierTag = pHierLane->header.roi_cmd_tag[0];

            HierIdxMap.ChannelId = pHierLane->header.channel_id;

            for (i = 0U; i < DSP_HIER_NUM; i++) {
                HierIdxMap.InputIdx = i;
                HierIdxMap.OutputIdx = 0xFFFFU;
                Rval = DSP_GiveEvent(LL_EVENT_HIER_IDX_MAPPING, &HierIdxMap, 0U);
                if ((Rval == OK) &&
                    (HierIdxMap.OutputIdx != 0xFFFFU)) {
                    pPymdData->HierCropWindow[HierIdxMap.OutputIdx].Width = pHierLane->luma_hier_outs[i].img_width;
                    pPymdData->HierCropWindow[HierIdxMap.OutputIdx].Height = pHierLane->luma_hier_outs[i].img_height;
                    pPymdData->HierCropWindow[HierIdxMap.OutputIdx].OffsetX = (UINT16) pHierLane->hier_out_offsets[i].x_offset;
                    pPymdData->HierCropWindow[HierIdxMap.OutputIdx].OffsetY = (UINT16) pHierLane->hier_out_offsets[i].y_offset;
                }
            }
            Rval = DSP_GiveEvent(LL_EVENT_LV_PYRAMID_RDY, pPymdInfo, EventBufIdx);
        }
    }

    return Rval;
}

static inline void UpdateIdspInfoOnVprocStatus(const msg_vproc_status_t *pVprocStatus)
{
    UINT8 ChId = pVprocStatus->channel_id;
    ULONG ULAddr = 0U;
    UINT16 HiStepId = 0U;
    const void *pVoid;
    IDSP_INFO_s *pIdspInfo = NULL;
#ifdef USE_STAT_IDSP_BUF
    DSP_MEM_BLK_t MemBlk = {0};
#endif
    UINT32 Rval;

#ifdef USE_STAT_IDSP_BUF
    Rval = DSP_GetStatBuf(DSP_STAT_BUF_IDSP, ChId, &MemBlk);
    if (Rval == OK) {
        dsp_osal_typecast(&pIdspInfo, &MemBlk.Base);
#else
    pIdspInfo = &AmbaDSP_IdspInfo[ChId];
#endif

        pIdspInfo->ChannelId = ChId;

        if (pVprocStatus->pass_step_id == DSP_IMG_PASS_C2Y) {
            const vproc_li_c2y_ext_data_t *pVprocLiC2yExtData = NULL;

            if (pVprocStatus->ext_data_daddr[0U] > 0U) {
                Rval = dsp_osal_cli2virt(pVprocStatus->ext_data_daddr[0U], &ULAddr);
                if (Rval == OK) {
                    dsp_osal_typecast(&pVprocLiC2yExtData, &ULAddr);
                    dsp_osal_typecast(&pVoid, &ULAddr);
                    //VprocExt is Non-Cache
                    //Rval = dsp_osal_cache_invalidate(pVoid, sizeof(vproc_li_c2y_ext_data_t));
                    //AmbaMisra_TouchUnused(&Rval);
                    /* Pipe Desc */
                    pIdspInfo->PipeDescProcFmt = (UINT8)IDSP_PROC_FMT_LISO;

                    /*
                     * When PassStep = C2Y,  IdspCfgHdrAddr[0] = Sec2
                     */
                    pIdspInfo->ChannelId = ChId;
                    pIdspInfo->C2Y.CapSeqNo = pVprocStatus->cap_seq_no;

                    Rval = dsp_osal_cli2virt(pVprocLiC2yExtData->sec2_cfg_addr, &pIdspInfo->C2Y.Sec2Cfg.CfgAddr);
                    LL_PrintErrLine(Rval, __func__, __LINE__);
                    pIdspInfo->C2Y.Sec2Cfg.CfgSize = pVprocLiC2yExtData->sec2_cfg_len;
                    Rval = dsp_osal_cli2virt(pVprocLiC2yExtData->sec4_cfg_addr, &pIdspInfo->C2Y.Sec4Cfg.CfgAddr);
                    LL_PrintErrLine(Rval, __func__, __LINE__);
                    pIdspInfo->C2Y.Sec4Cfg.CfgSize = pVprocLiC2yExtData->sec4_cfg_len;

                    Rval = dsp_osal_cli2virt(pVprocLiC2yExtData->c2y_luma_out.buffer_addr, &pIdspInfo->C2Y.Sec2R2y.LumaAddr);
                    LL_PrintErrLine(Rval, __func__, __LINE__);
                    pIdspInfo->C2Y.Sec2R2y.LumaPitch = pVprocLiC2yExtData->c2y_luma_out.buffer_pitch;
                    pIdspInfo->C2Y.Sec2R2y.LumaWidth = pVprocLiC2yExtData->c2y_luma_out.img_width;
                    pIdspInfo->C2Y.Sec2R2y.LumaHeight = pVprocLiC2yExtData->c2y_luma_out.img_height;

                    Rval = dsp_osal_cli2virt(pVprocLiC2yExtData->c2y_chroma_out.buffer_addr, &pIdspInfo->C2Y.Sec2R2y.ChromaAddr);
                    LL_PrintErrLine(Rval, __func__, __LINE__);
                    pIdspInfo->C2Y.Sec2R2y.ChromaPitch = pVprocLiC2yExtData->c2y_chroma_out.buffer_pitch;
                    pIdspInfo->C2Y.Sec2R2y.ChromaWidth = pVprocLiC2yExtData->c2y_chroma_out.img_width;
                    pIdspInfo->C2Y.Sec2R2y.ChromaHeight = pVprocLiC2yExtData->c2y_chroma_out.img_height;
                }
            }
        } else if (pVprocStatus->pass_step_id == DSP_IMG_PASS_WARP) {
            const vproc_li_warp_mctf_prev_ext_data_t *pVprocWarpExtData = NULL;

            if (pVprocStatus->ext_data_daddr[0U] > 0U) {
                Rval = dsp_osal_cli2virt(pVprocStatus->ext_data_daddr[0U], &ULAddr);
                if (Rval == OK) {
                    dsp_osal_typecast(&pVprocWarpExtData, &ULAddr);
                    dsp_osal_typecast(&pVoid, &ULAddr);
                    //VprocExt is Non-Cache
                    //Rval = dsp_osal_cache_invalidate(pVoid, sizeof(vproc_li_warp_mctf_prev_ext_data_t));
                    //AmbaMisra_TouchUnused(&Rval);
                    pIdspInfo->vWarpMctf.CapSeqNo = pVprocStatus->cap_seq_no;

                    // Sec3Cfg
                    Rval = dsp_osal_cli2virt(pVprocStatus->idsp_cfg_hdr_addr[DSP_SEC3_CFG_INDEX], &pIdspInfo->vWarpMctf.Sec3Cfg.CfgAddr);
                    LL_PrintErrLine(Rval, __func__, __LINE__);
                    pIdspInfo->vWarpMctf.Sec3Cfg.CfgSize = DSP_SEC3_CFG_SIZE;

                    // MctfCfg
                    Rval = dsp_osal_cli2virt(pVprocStatus->idsp_cfg_hdr_addr[DSP_MCTF_CFG_INDEX], &pIdspInfo->vWarpMctf.MctfCfg.CfgAddr);
                    LL_PrintErrLine(Rval, __func__, __LINE__);
                    pIdspInfo->vWarpMctf.MctfCfg.CfgSize = DSP_MCTF_CFG_SIZE;

                    // MctsCfg
                    Rval = dsp_osal_cli2virt(pVprocStatus->idsp_cfg_hdr_addr[DSP_MCTS_CFG_INDEX], &pIdspInfo->vWarpMctf.MctsCfg.CfgAddr);
                    LL_PrintErrLine(Rval, __func__, __LINE__);
                    pIdspInfo->vWarpMctf.MctsCfg.CfgSize = DSP_MCTS_CFG_SIZE;

                    if (1U == IsValidU32Addr(pVprocStatus->idsp_cfg_hdr_addr[DSP_MCTF_CFG_INDEX])) {
                        Rval = dsp_osal_cli2virt(pVprocWarpExtData->mctf_luma_out.buffer_addr, &pIdspInfo->vWarpMctf.Mctf.LumaAddr);
                        LL_PrintErrLine(Rval, __func__, __LINE__);
                        pIdspInfo->vWarpMctf.Mctf.LumaPitch = pVprocWarpExtData->mctf_luma_out.buffer_pitch;
                        pIdspInfo->vWarpMctf.Mctf.LumaWidth = pVprocWarpExtData->mctf_luma_out.img_width;
                        pIdspInfo->vWarpMctf.Mctf.LumaHeight = pVprocWarpExtData->mctf_luma_out.img_height;
                        Rval = dsp_osal_cli2virt(pVprocWarpExtData->mctf_chroma_out.buffer_addr, &pIdspInfo->vWarpMctf.Mctf.ChromaAddr);
                        LL_PrintErrLine(Rval, __func__, __LINE__);
                        pIdspInfo->vWarpMctf.Mctf.ChromaPitch = pVprocWarpExtData->mctf_chroma_out.buffer_pitch;
                        pIdspInfo->vWarpMctf.Mctf.ChromaWidth = pVprocWarpExtData->mctf_chroma_out.img_width;
                        pIdspInfo->vWarpMctf.Mctf.ChromaHeight = pVprocWarpExtData->mctf_chroma_out.img_height;
                    } else {
                        Rval = dsp_osal_cli2virt(pVprocWarpExtData->sec3_luma_out.buffer_addr, &pIdspInfo->vWarpMctf.Mctf.LumaAddr);
                        LL_PrintErrLine(Rval, __func__, __LINE__);
                        pIdspInfo->vWarpMctf.Mctf.LumaPitch = pVprocWarpExtData->sec3_luma_out.buffer_pitch;
                        pIdspInfo->vWarpMctf.Mctf.LumaWidth = pVprocWarpExtData->sec3_luma_out.img_width;
                        pIdspInfo->vWarpMctf.Mctf.LumaHeight = pVprocWarpExtData->sec3_luma_out.img_height;
                        Rval = dsp_osal_cli2virt(pVprocWarpExtData->sec3_chroma_out.buffer_addr, &pIdspInfo->vWarpMctf.Mctf.ChromaAddr);
                        LL_PrintErrLine(Rval, __func__, __LINE__);
                        pIdspInfo->vWarpMctf.Mctf.ChromaPitch = pVprocWarpExtData->sec3_chroma_out.buffer_pitch;
                        pIdspInfo->vWarpMctf.Mctf.ChromaWidth = pVprocWarpExtData->sec3_chroma_out.img_width;
                        pIdspInfo->vWarpMctf.Mctf.ChromaHeight = pVprocWarpExtData->sec3_chroma_out.img_height;
                    }

                    if (1U == IsValidU32Addr(pVprocStatus->idsp_cfg_hdr_addr[DSP_MCTS_CFG_INDEX])) {
                        Rval = dsp_osal_cli2virt(pVprocWarpExtData->mcts_luma_out.buffer_addr, &pIdspInfo->vWarpMctf.Mcts.LumaAddr);
                        LL_PrintErrLine(Rval, __func__, __LINE__);
                        pIdspInfo->vWarpMctf.Mcts.LumaPitch = pVprocWarpExtData->mcts_luma_out.buffer_pitch;
                        pIdspInfo->vWarpMctf.Mcts.LumaWidth = pVprocWarpExtData->mcts_luma_out.img_width;
                        pIdspInfo->vWarpMctf.Mcts.LumaHeight = pVprocWarpExtData->mcts_luma_out.img_height;
                        Rval = dsp_osal_cli2virt(pVprocWarpExtData->mcts_chroma_out.buffer_addr, &pIdspInfo->vWarpMctf.Mcts.ChromaAddr);
                        LL_PrintErrLine(Rval, __func__, __LINE__);
                        pIdspInfo->vWarpMctf.Mcts.ChromaPitch = pVprocWarpExtData->mcts_chroma_out.buffer_pitch;
                        pIdspInfo->vWarpMctf.Mcts.ChromaWidth = pVprocWarpExtData->mcts_chroma_out.img_width;
                        pIdspInfo->vWarpMctf.Mcts.ChromaHeight = pVprocWarpExtData->mcts_chroma_out.img_height;
                    } else {
                        if (1U == IsValidU32Addr(pVprocStatus->idsp_cfg_hdr_addr[DSP_MCTF_CFG_INDEX])) {
                            Rval = dsp_osal_memcpy(&pIdspInfo->vWarpMctf.Mcts, &pIdspInfo->vWarpMctf.Mctf, sizeof(IDSP_YUV_INFO_s));
                            LL_PrintErrLine(Rval, __func__, __LINE__);
                        } else {
                            Rval = dsp_osal_cli2virt(pVprocWarpExtData->sec3_luma_out.buffer_addr, &pIdspInfo->vWarpMctf.Mcts.LumaAddr);
                            LL_PrintErrLine(Rval, __func__, __LINE__);
                            pIdspInfo->vWarpMctf.Mcts.LumaPitch = pVprocWarpExtData->sec3_luma_out.buffer_pitch;
                            pIdspInfo->vWarpMctf.Mcts.LumaWidth = pVprocWarpExtData->sec3_luma_out.img_width;
                            pIdspInfo->vWarpMctf.Mcts.LumaHeight = pVprocWarpExtData->sec3_luma_out.img_height;
                            Rval = dsp_osal_cli2virt(pVprocWarpExtData->sec3_chroma_out.buffer_addr, &pIdspInfo->vWarpMctf.Mcts.ChromaAddr);
                            LL_PrintErrLine(Rval, __func__, __LINE__);
                            pIdspInfo->vWarpMctf.Mcts.ChromaPitch = pVprocWarpExtData->sec3_chroma_out.buffer_pitch;
                            pIdspInfo->vWarpMctf.Mcts.ChromaWidth = pVprocWarpExtData->sec3_chroma_out.img_width;
                            pIdspInfo->vWarpMctf.Mcts.ChromaHeight = pVprocWarpExtData->sec3_chroma_out.img_height;
                        }
                    }

                    Rval = dsp_osal_cli2virt(pVprocWarpExtData->mcts_luma_out.buffer_addr, &pIdspInfo->vWarpMctf.Mcts.LumaAddr);
                    LL_PrintErrLine(Rval, __func__, __LINE__);
                    pIdspInfo->vWarpMctf.Mcts.LumaPitch = pVprocWarpExtData->mcts_luma_out.buffer_pitch;
                    pIdspInfo->vWarpMctf.Mcts.LumaWidth = pVprocWarpExtData->mcts_luma_out.img_width;
                    pIdspInfo->vWarpMctf.Mcts.LumaHeight = pVprocWarpExtData->mcts_luma_out.img_height;
                    Rval = dsp_osal_cli2virt(pVprocWarpExtData->mcts_chroma_out.buffer_addr, &pIdspInfo->vWarpMctf.Mcts.ChromaAddr);
                    LL_PrintErrLine(Rval, __func__, __LINE__);
                    pIdspInfo->vWarpMctf.Mcts.ChromaPitch = pVprocWarpExtData->mcts_chroma_out.buffer_pitch;
                    pIdspInfo->vWarpMctf.Mcts.ChromaWidth = pVprocWarpExtData->mcts_chroma_out.img_width;
                    pIdspInfo->vWarpMctf.Mcts.ChromaHeight = pVprocWarpExtData->mcts_chroma_out.img_height;
                }
            }
        } else if ((pVprocStatus->pass_step_id >= DSP_IMG_PASS_HISO_STEP_1) &&
                   (pVprocStatus->pass_step_id <= DSP_IMG_PASS_HISO_STEP_13)) {

            const sproc_hi_ext_data_t *pSprocHiExtData = NULL;

            if (pVprocStatus->ext_data_daddr[0U] > 0U) {
                Rval = dsp_osal_cli2virt(pVprocStatus->ext_data_daddr[0U], &ULAddr);
                HiStepId = pVprocStatus->pass_step_id;
                HiStepId -= DSP_IMG_PASS_HI_BASE;
                if ((Rval == OK) && (HiStepId < DSP_HISO_STEP_NUM)) {
                    dsp_osal_typecast(&pSprocHiExtData, &ULAddr);
                    dsp_osal_typecast(&pVoid, &ULAddr);
                    //VprocExt is Non-Cache
                    //Rval = dsp_osal_cache_invalidate(pVoid, sizeof(sproc_hi_ext_data_t));
                    //AmbaMisra_TouchUnused(&Rval);

                    /* Pipe Desc */
                    pIdspInfo->PipeDescProcFmt = (UINT8)IDSP_PROC_FMT_HISO;

                    // Sec2Cfg
                    Rval = dsp_osal_cli2virt(pVprocStatus->idsp_cfg_hdr_addr[DSP_HI_SEC2_CFG_INDEX], &pIdspInfo->HISO[HiStepId].Sec2Cfg.CfgAddr);
                    LL_PrintErrLine(Rval, __func__, __LINE__);
                    pIdspInfo->HISO[HiStepId].Sec2Cfg.CfgSize = DSP_SEC2_CFG_SIZE;

                    // Sec4Cfg
                    Rval = dsp_osal_cli2virt(pVprocStatus->idsp_cfg_hdr_addr[DSP_HI_SEC4_CFG_INDEX], &pIdspInfo->HISO[HiStepId].Sec4Cfg.CfgAddr);
                    LL_PrintErrLine(Rval, __func__, __LINE__);
                    pIdspInfo->HISO[HiStepId].Sec4Cfg.CfgSize = DSP_SEC4_CFG_SIZE;

                    // Sec5Cfg
                    Rval = dsp_osal_cli2virt(pVprocStatus->idsp_cfg_hdr_addr[DSP_HI_SEC5_CFG_INDEX], &pIdspInfo->HISO[HiStepId].Sec5Cfg.CfgAddr);
                    LL_PrintErrLine(Rval, __func__, __LINE__);
                    pIdspInfo->HISO[HiStepId].Sec5Cfg.CfgSize = DSP_SEC5_CFG_SIZE;

                    // Sec6Cfg
                    Rval = dsp_osal_cli2virt(pVprocStatus->idsp_cfg_hdr_addr[DSP_HI_SEC6_CFG_INDEX], &pIdspInfo->HISO[HiStepId].Sec6Cfg.CfgAddr);
                    LL_PrintErrLine(Rval, __func__, __LINE__);
                    pIdspInfo->HISO[HiStepId].Sec6Cfg.CfgSize = DSP_SEC6_CFG_SIZE;

                    // Sec7Cfg
                    Rval = dsp_osal_cli2virt(pVprocStatus->idsp_cfg_hdr_addr[DSP_HI_SEC7_CFG_INDEX], &pIdspInfo->HISO[HiStepId].Sec7Cfg.CfgAddr);
                    LL_PrintErrLine(Rval, __func__, __LINE__);
                    pIdspInfo->HISO[HiStepId].Sec7Cfg.CfgSize = DSP_SEC7_CFG_SIZE;

                    // Sec9Cfg
                    Rval = dsp_osal_cli2virt(pVprocStatus->idsp_cfg_hdr_addr[DSP_HI_SEC9_CFG_INDEX], &pIdspInfo->HISO[HiStepId].Sec9Cfg.CfgAddr);
                    LL_PrintErrLine(Rval, __func__, __LINE__);
                    pIdspInfo->HISO[HiStepId].Sec9Cfg.CfgSize = DSP_SEC9_CFG_SIZE;

                    // MctfCfg
                    Rval = dsp_osal_cli2virt(pVprocStatus->idsp_cfg_hdr_addr[DSP_HI_MCTF_CFG_INDEX], &pIdspInfo->HISO[HiStepId].MctfCfg.CfgAddr);
                    LL_PrintErrLine(Rval, __func__, __LINE__);
                    pIdspInfo->HISO[HiStepId].MctfCfg.CfgSize = DSP_MCTF_CFG_SIZE;

                    // MctsCfg
                    Rval = dsp_osal_cli2virt(pVprocStatus->idsp_cfg_hdr_addr[DSP_HI_MCTS_CFG_INDEX], &pIdspInfo->HISO[HiStepId].MctsCfg.CfgAddr);
                    LL_PrintErrLine(Rval, __func__, __LINE__);
                    pIdspInfo->HISO[HiStepId].MctsCfg.CfgSize = DSP_MCTS_CFG_SIZE;

                    // Sec2Yuv
                    Rval = dsp_osal_cli2virt(pSprocHiExtData->sec2_luma_out.buffer_addr, &pIdspInfo->HISO[HiStepId].Sec2.LumaAddr);
                    LL_PrintErrLine(Rval, __func__, __LINE__);
                    pIdspInfo->HISO[HiStepId].Sec2.LumaPitch = pSprocHiExtData->sec2_luma_out.buffer_pitch;
                    pIdspInfo->HISO[HiStepId].Sec2.LumaWidth = pSprocHiExtData->sec2_luma_out.img_width;
                    pIdspInfo->HISO[HiStepId].Sec2.LumaHeight = pSprocHiExtData->sec2_luma_out.img_height;
                    Rval = dsp_osal_cli2virt(pSprocHiExtData->sec2_chroma_out.buffer_addr, &pIdspInfo->HISO[HiStepId].Sec2.ChromaAddr);
                    LL_PrintErrLine(Rval, __func__, __LINE__);
                    pIdspInfo->HISO[HiStepId].Sec2.ChromaPitch = pSprocHiExtData->sec2_chroma_out.buffer_pitch;
                    pIdspInfo->HISO[HiStepId].Sec2.ChromaWidth = pSprocHiExtData->sec2_chroma_out.img_width;
                    pIdspInfo->HISO[HiStepId].Sec2.ChromaHeight = pSprocHiExtData->sec2_chroma_out.img_height;

                    // Sec5Yuv
                    Rval = dsp_osal_cli2virt(pSprocHiExtData->sec5_luma_out.buffer_addr, &pIdspInfo->HISO[HiStepId].Sec5.LumaAddr);
                    LL_PrintErrLine(Rval, __func__, __LINE__);
                    pIdspInfo->HISO[HiStepId].Sec5.LumaPitch = pSprocHiExtData->sec5_luma_out.buffer_pitch;
                    pIdspInfo->HISO[HiStepId].Sec5.LumaWidth = pSprocHiExtData->sec5_luma_out.img_width;
                    pIdspInfo->HISO[HiStepId].Sec5.LumaHeight = pSprocHiExtData->sec5_luma_out.img_height;
                    Rval = dsp_osal_cli2virt(pSprocHiExtData->sec5_chroma_out.buffer_addr, &pIdspInfo->HISO[HiStepId].Sec5.ChromaAddr);
                    LL_PrintErrLine(Rval, __func__, __LINE__);
                    pIdspInfo->HISO[HiStepId].Sec5.ChromaPitch = pSprocHiExtData->sec5_chroma_out.buffer_pitch;
                    pIdspInfo->HISO[HiStepId].Sec5.ChromaWidth = pSprocHiExtData->sec5_chroma_out.img_width;
                    pIdspInfo->HISO[HiStepId].Sec5.ChromaHeight = pSprocHiExtData->sec5_chroma_out.img_height;

                    // Sec6Yuv
                    Rval = dsp_osal_cli2virt(pSprocHiExtData->sec6_luma_out.buffer_addr, &pIdspInfo->HISO[HiStepId].Sec6.LumaAddr);
                    LL_PrintErrLine(Rval, __func__, __LINE__);
                    pIdspInfo->HISO[HiStepId].Sec6.LumaPitch = pSprocHiExtData->sec6_luma_out.buffer_pitch;
                    pIdspInfo->HISO[HiStepId].Sec6.LumaWidth = pSprocHiExtData->sec6_luma_out.img_width;
                    pIdspInfo->HISO[HiStepId].Sec6.LumaHeight = pSprocHiExtData->sec6_luma_out.img_height;
                    Rval = dsp_osal_cli2virt(pSprocHiExtData->sec6_chroma_out.buffer_addr, &pIdspInfo->HISO[HiStepId].Sec6.ChromaAddr);
                    LL_PrintErrLine(Rval, __func__, __LINE__);
                    pIdspInfo->HISO[HiStepId].Sec6.ChromaPitch = pSprocHiExtData->sec6_chroma_out.buffer_pitch;
                    pIdspInfo->HISO[HiStepId].Sec6.ChromaWidth = pSprocHiExtData->sec6_chroma_out.img_width;
                    pIdspInfo->HISO[HiStepId].Sec6.ChromaHeight = pSprocHiExtData->sec6_chroma_out.img_height;

                    // Sec7Yuv
                    Rval = dsp_osal_cli2virt(pSprocHiExtData->sec7_luma_out.buffer_addr, &pIdspInfo->HISO[HiStepId].Sec7.LumaAddr);
                    LL_PrintErrLine(Rval, __func__, __LINE__);
                    pIdspInfo->HISO[HiStepId].Sec7.LumaPitch = pSprocHiExtData->sec7_luma_out.buffer_pitch;
                    pIdspInfo->HISO[HiStepId].Sec7.LumaWidth = pSprocHiExtData->sec7_luma_out.img_width;
                    pIdspInfo->HISO[HiStepId].Sec7.LumaHeight = pSprocHiExtData->sec7_luma_out.img_height;
                    Rval = dsp_osal_cli2virt(pSprocHiExtData->sec7_chroma_out.buffer_addr, &pIdspInfo->HISO[HiStepId].Sec7.ChromaAddr);
                    LL_PrintErrLine(Rval, __func__, __LINE__);
                    pIdspInfo->HISO[HiStepId].Sec7.ChromaPitch = pSprocHiExtData->sec7_chroma_out.buffer_pitch;
                    pIdspInfo->HISO[HiStepId].Sec7.ChromaWidth = pSprocHiExtData->sec7_chroma_out.img_width;
                    pIdspInfo->HISO[HiStepId].Sec7.ChromaHeight = pSprocHiExtData->sec7_chroma_out.img_height;

                    // Sec9Yuv
                    Rval = dsp_osal_cli2virt(pSprocHiExtData->sec9_luma_out.buffer_addr, &pIdspInfo->HISO[HiStepId].Sec9.LumaAddr);
                    LL_PrintErrLine(Rval, __func__, __LINE__);
                    pIdspInfo->HISO[HiStepId].Sec9.LumaPitch = pSprocHiExtData->sec9_luma_out.buffer_pitch;
                    pIdspInfo->HISO[HiStepId].Sec9.LumaWidth = pSprocHiExtData->sec9_luma_out.img_width;
                    pIdspInfo->HISO[HiStepId].Sec9.LumaHeight = pSprocHiExtData->sec9_luma_out.img_height;
                    Rval = dsp_osal_cli2virt(pSprocHiExtData->sec9_chroma_out.buffer_addr, &pIdspInfo->HISO[HiStepId].Sec9.ChromaAddr);
                    LL_PrintErrLine(Rval, __func__, __LINE__);
                    pIdspInfo->HISO[HiStepId].Sec9.ChromaPitch = pSprocHiExtData->sec9_chroma_out.buffer_pitch;
                    pIdspInfo->HISO[HiStepId].Sec9.ChromaWidth = pSprocHiExtData->sec9_chroma_out.img_width;
                    pIdspInfo->HISO[HiStepId].Sec9.ChromaHeight = pSprocHiExtData->sec9_chroma_out.img_height;

                    // MctfYuv
                    Rval = dsp_osal_cli2virt(pSprocHiExtData->mctf_luma_out.buffer_addr, &pIdspInfo->HISO[HiStepId].Mctf.LumaAddr);
                    LL_PrintErrLine(Rval, __func__, __LINE__);
                    pIdspInfo->HISO[HiStepId].Mctf.LumaPitch = pSprocHiExtData->mctf_luma_out.buffer_pitch;
                    pIdspInfo->HISO[HiStepId].Mctf.LumaWidth = pSprocHiExtData->mctf_luma_out.img_width;
                    pIdspInfo->HISO[HiStepId].Mctf.LumaHeight = pSprocHiExtData->mctf_luma_out.img_height;
                    Rval = dsp_osal_cli2virt(pSprocHiExtData->mctf_chroma_out.buffer_addr, &pIdspInfo->HISO[HiStepId].Mctf.ChromaAddr);
                    LL_PrintErrLine(Rval, __func__, __LINE__);
                    pIdspInfo->HISO[HiStepId].Mctf.ChromaPitch = pSprocHiExtData->mctf_chroma_out.buffer_pitch;
                    pIdspInfo->HISO[HiStepId].Mctf.ChromaWidth = pSprocHiExtData->mctf_chroma_out.img_width;
                    pIdspInfo->HISO[HiStepId].Mctf.ChromaHeight = pSprocHiExtData->mctf_chroma_out.img_height;

                    // MctsYuv
                    Rval = dsp_osal_cli2virt(pSprocHiExtData->mcts_luma_out.buffer_addr, &pIdspInfo->HISO[HiStepId].Mcts.LumaAddr);
                    LL_PrintErrLine(Rval, __func__, __LINE__);
                    pIdspInfo->HISO[HiStepId].Mcts.LumaPitch = pSprocHiExtData->mcts_luma_out.buffer_pitch;
                    pIdspInfo->HISO[HiStepId].Mcts.LumaWidth = pSprocHiExtData->mcts_luma_out.img_width;
                    pIdspInfo->HISO[HiStepId].Mcts.LumaHeight = pSprocHiExtData->mcts_luma_out.img_height;
                    Rval = dsp_osal_cli2virt(pSprocHiExtData->mcts_chroma_out.buffer_addr, &pIdspInfo->HISO[HiStepId].Mcts.ChromaAddr);
                    LL_PrintErrLine(Rval, __func__, __LINE__);
                    pIdspInfo->HISO[HiStepId].Mcts.ChromaPitch = pSprocHiExtData->mcts_chroma_out.buffer_pitch;
                    pIdspInfo->HISO[HiStepId].Mcts.ChromaWidth = pSprocHiExtData->mcts_chroma_out.img_width;
                    pIdspInfo->HISO[HiStepId].Mcts.ChromaHeight = pSprocHiExtData->mcts_chroma_out.img_height;
                }
            }
        } else {
            // DO NOTHING
        }
#ifdef USE_STAT_IDSP_BUF
    }
#endif
}

static inline void UpdateIdspTileInfoOnVprocStatus(const msg_vproc_status_t *pVprocStatus)
{
    UINT8 ChId = pVprocStatus->channel_id;
    IDSP_INFO_s *pIdspInfo = NULL;
#ifdef USE_STAT_IDSP_BUF
    DSP_MEM_BLK_t MemBlk = {0};
#endif
    UINT32 Rval;

#ifdef USE_STAT_IDSP_BUF
    Rval = DSP_GetStatBuf(DSP_STAT_BUF_IDSP, ChId, &MemBlk);
    if (Rval == OK) {
        dsp_osal_typecast(&pIdspInfo, &MemBlk.Base);
    }
#else
    pIdspInfo = &AmbaDSP_IdspInfo[ChId];
#endif

    if (pVprocStatus->pass_step_id == DSP_IMG_PASS_C2Y) {
        Rval = dsp_osal_cli2virt(pVprocStatus->idsp_cfg_hdr_addr[0U], &pIdspInfo->Sec2TileCfg[pVprocStatus->tile_x_idx][pVprocStatus->tile_y_idx].CfgAddr);
        LL_PrintErrLine(Rval, __func__, __LINE__);
        pIdspInfo->Sec2TileCfg[pVprocStatus->tile_x_idx][pVprocStatus->tile_y_idx].CfgSize = DSP_SEC2_CFG_SIZE;
    } else if (pVprocStatus->pass_step_id == DSP_IMG_PASS_WARP) {
        Rval = dsp_osal_cli2virt(pVprocStatus->idsp_cfg_hdr_addr[DSP_SEC3_CFG_INDEX], &pIdspInfo->Sec3TileCfg[pVprocStatus->tile_x_idx][pVprocStatus->tile_y_idx].CfgAddr);
        LL_PrintErrLine(Rval, __func__, __LINE__);
        pIdspInfo->Sec3TileCfg[pVprocStatus->tile_x_idx][pVprocStatus->tile_y_idx].CfgSize = DSP_SEC3_CFG_SIZE;
    } else {
        // DO NOTHING
    }

    /* Since Yuv data is ready when LastTile, dsp only report dram address at LastTile */
    if (((pVprocStatus->tile_x_idx + 1U) == pVprocStatus->stitch_tile_num_x) &&
        ((pVprocStatus->tile_y_idx + 1U) == pVprocStatus->stitch_tile_num_y)) {
        /* Update Idsp debug buffer */
        UpdateIdspInfoOnVprocStatus(pVprocStatus);
    }
}

static inline void UpdateIdspInfoOnVprocAaaStatus(const msg_vproc_aaa_status_t *pVprocAaaStatus)
{
    UINT8 ChId = pVprocAaaStatus->channel_id;
    UINT16 SliceIdx;
    UINT16 i, TotalSliceNum;
    IDSP_INFO_s *pIdspInfo = NULL;
#ifdef USE_STAT_IDSP_BUF
    DSP_MEM_BLK_t MemBlk = {0};
    UINT32 Rval;
#endif

#ifdef USE_STAT_IDSP_BUF
    Rval = DSP_GetStatBuf(DSP_STAT_BUF_IDSP, ChId, &MemBlk);
    LL_PrintErrLine(Rval, __func__, __LINE__);
    if (Rval == OK) {
        dsp_osal_typecast(&pIdspInfo, &MemBlk.Base);
#else
    pIdspInfo = &AmbaDSP_IdspInfo[ChId];
#endif

        /* Update Idsp database */
        pIdspInfo->CfaAAA.CapSeqNo = pVprocAaaStatus->cap_seq_no;
        pIdspInfo->CfaAAA.SliceX = CfaTransInfo[ChId].SrcSliceX;
        pIdspInfo->CfaAAA.SliceY = CfaTransInfo[ChId].SrcSliceY;
        TotalSliceNum = (UINT16)CfaTransInfo[ChId].SrcSliceX*CfaTransInfo[ChId].SrcSliceY;
        for (SliceIdx = 0U; SliceIdx < TotalSliceNum; SliceIdx++) {
            pIdspInfo->CfaAAA.Addr[SliceIdx] = CfaTransInfo[ChId].SrcAddr[SliceIdx];
        }

        pIdspInfo->PgAAA.CapSeqNo = pVprocAaaStatus->cap_seq_no;
        pIdspInfo->PgAAA.SliceX = PgTransInfo[ChId].SrcSliceX;
        pIdspInfo->PgAAA.SliceY = PgTransInfo[ChId].SrcSliceY;
        TotalSliceNum = (UINT16)PgTransInfo[ChId].SrcSliceX*PgTransInfo[ChId].SrcSliceY;
        for (SliceIdx = 0U; SliceIdx < TotalSliceNum; SliceIdx++) {
            pIdspInfo->PgAAA.Addr[SliceIdx] = PgTransInfo[ChId].SrcAddr[SliceIdx];
        }

        for (i = 0U; i < DSP_VIN_HIST_NUM; i++) {
            if (ValidVinHist[ChId][i] != 0U) {
                /* Update Idsp database */
                pIdspInfo->HistAAA[i].CapSeqNo = pVprocAaaStatus->cap_seq_no;
                pIdspInfo->HistAAA[i].SliceX = CfaHistTransInfo[ChId][i].SrcSliceX;
                pIdspInfo->HistAAA[i].SliceY = CfaHistTransInfo[ChId][i].SrcSliceY;
                TotalSliceNum = (UINT16)CfaHistTransInfo[ChId][i].SrcSliceX*CfaHistTransInfo[ChId][i].SrcSliceY;
                for (SliceIdx = 0U; SliceIdx < TotalSliceNum; SliceIdx++) {
                    pIdspInfo->HistAAA[i].Addr[SliceIdx] = CfaHistTransInfo[ChId][i].SrcAddr[SliceIdx];
                }
            }
        }
#ifdef USE_STAT_IDSP_BUF
    }
#endif
}

static inline void ResetTransInfoOnVprocAaaStatus(const msg_vproc_aaa_status_t *pVprocAaaStatus)
{
    UINT16 ChId = pVprocAaaStatus->channel_id;
    UINT8 i;
    UINT32 Rval;
static UINT8 TotalAAASliceNum[AMBA_DSP_MAX_VIEWZONE_NUM] = {0};

    AAADataValidPattern[ChId] = 0x0U;
    AAADataValidData[ChId] = 0x0U;
    Rval = dsp_osal_memset(&CfaTransInfo[ChId], 0, sizeof(AMBA_DSP_EVENT_3A_TRANSFER_s));
    LL_PrintErrLine(Rval, __func__, __LINE__);
    Rval = dsp_osal_memset(&PgTransInfo[ChId], 0, sizeof(AMBA_DSP_EVENT_3A_TRANSFER_s));
    LL_PrintErrLine(Rval, __func__, __LINE__);
    Rval = dsp_osal_memset(&CfaHistTransInfo[ChId][0U], 0, sizeof(AMBA_DSP_EVENT_3A_TRANSFER_s)*DSP_VIN_HIST_NUM);
    LL_PrintErrLine(Rval, __func__, __LINE__);
    Rval = dsp_osal_memset(&ValidVinHist[ChId][0U], 0, sizeof(UINT8)*DSP_VIN_HIST_NUM);
    LL_PrintErrLine(Rval, __func__, __LINE__);
    CfaTransInfo[ChId].Type = DSP_3A_VIDEO;
    PgTransInfo[ChId].Type = DSP_3A_VIDEO_PG;
    for (i=0U; i<DSP_VIN_HIST_NUM; i++) {
        CfaHistTransInfo[ChId][i].Type = DSP_HIST_VIDEO;
        CfaHistTransInfo[ChId][i].ChannelIdx = pVprocAaaStatus->channel_id;
        CfaHistTransInfo[ChId][i].SrcSliceX = pVprocAaaStatus->stitch_tile_num_x;
        CfaHistTransInfo[ChId][i].SrcSliceY = pVprocAaaStatus->stitch_tile_num_y;
    }
    CfaTransInfo[ChId].ChannelIdx = pVprocAaaStatus->channel_id;
    PgTransInfo[ChId].ChannelIdx = pVprocAaaStatus->channel_id;
    CfaTransInfo[ChId].SrcSliceX = pVprocAaaStatus->stitch_tile_num_x;
    PgTransInfo[ChId].SrcSliceX = pVprocAaaStatus->stitch_tile_num_x;
    CfaTransInfo[ChId].SrcSliceY = pVprocAaaStatus->stitch_tile_num_y;
    PgTransInfo[ChId].SrcSliceY = pVprocAaaStatus->stitch_tile_num_y;
    TotalAAASliceNum[ChId] = (UINT8) (pVprocAaaStatus->stitch_tile_num_x*pVprocAaaStatus->stitch_tile_num_y);
    if ((TotalAAASliceNum[ChId] == 0U) &&
        ((pVprocAaaStatus->cfa_aaa_stat_buf_addr != 0U) || (pVprocAaaStatus->rgb_aaa_stat_buf_addr != 0U))) {//FIXME, using ASSERT!
        AmbaLL_LogUInt5( "[ERROR][%d] MsgDispatcher Invalid SliceNum = 0", __LINE__, 0U, 0U, 0U, 0U);
    }

    for (i = 0U; i < (UINT16)TotalAAASliceNum[ChId]; i++) {
        AAADataValidPattern[ChId] |= (UINT32)(1UL<<i);
    }
}

static inline void UpdateIdspInfoOnVprocExtDataStatus(const msg_vproc_ext_data_status_t *pVprocExtDataStatus)
{
    UINT16 ChId = pVprocExtDataStatus->channel_id;
    IDSP_INFO_s *pIdspInfo = NULL;
#ifdef USE_STAT_IDSP_BUF
    DSP_MEM_BLK_t MemBlk = {0};
#endif
    UINT32 Rval;

#ifdef USE_STAT_IDSP_BUF
    Rval = DSP_GetStatBuf(DSP_STAT_BUF_IDSP, ChId, &MemBlk);
    if (Rval == OK) {
        dsp_osal_typecast(&pIdspInfo, &MemBlk.Base);
#else
    pIdspInfo = &AmbaDSP_IdspInfo[ChId];
#endif

        switch (pVprocExtDataStatus->pass_step_id) {
        case DSP_IMG_PASS_C2Y:
        {
            const vproc_li_c2y_ext_data_t *pVprocLiC2yExtData = &pVprocExtDataStatus->ext_data.li_c2y;

            /* Update Idsp debug buffer */
            {
                pIdspInfo->ChannelId = ChId;
                pIdspInfo->C2Y.CapSeqNo = pVprocExtDataStatus->cap_seq_no;

                Rval = dsp_osal_cli2virt(pVprocLiC2yExtData->sec2_cfg_addr, &pIdspInfo->C2Y.Sec2Cfg.CfgAddr);
                LL_PrintErrLine(Rval, __func__, __LINE__);
                pIdspInfo->C2Y.Sec2Cfg.CfgSize = pVprocLiC2yExtData->sec2_cfg_len;
                Rval = dsp_osal_cli2virt(pVprocLiC2yExtData->sec4_cfg_addr, &pIdspInfo->C2Y.Sec4Cfg.CfgAddr);
                LL_PrintErrLine(Rval, __func__, __LINE__);
                pIdspInfo->C2Y.Sec4Cfg.CfgSize = pVprocLiC2yExtData->sec4_cfg_len;

                Rval = dsp_osal_cli2virt(pVprocLiC2yExtData->c2y_luma_out.buffer_addr, &pIdspInfo->C2Y.Sec2R2y.LumaAddr);
                LL_PrintErrLine(Rval, __func__, __LINE__);
                pIdspInfo->C2Y.Sec2R2y.LumaPitch = pVprocLiC2yExtData->c2y_luma_out.buffer_pitch;
                pIdspInfo->C2Y.Sec2R2y.LumaWidth = pVprocLiC2yExtData->c2y_luma_out.img_width;
                pIdspInfo->C2Y.Sec2R2y.LumaHeight = pVprocLiC2yExtData->c2y_luma_out.img_height;

                Rval = dsp_osal_cli2virt(pVprocLiC2yExtData->c2y_chroma_out.buffer_addr, &pIdspInfo->C2Y.Sec2R2y.ChromaAddr);
                LL_PrintErrLine(Rval, __func__, __LINE__);
                pIdspInfo->C2Y.Sec2R2y.ChromaPitch = pVprocLiC2yExtData->c2y_chroma_out.buffer_pitch;
                pIdspInfo->C2Y.Sec2R2y.ChromaWidth = pVprocLiC2yExtData->c2y_chroma_out.img_width;
                pIdspInfo->C2Y.Sec2R2y.ChromaHeight = pVprocLiC2yExtData->c2y_chroma_out.img_height;

                /* Sec2 Me1 N/A */
            }
            break;
        }
        case DSP_IMG_PASS_HIER_LANE_DET:
        case DSP_IMG_PASS_VIN:
        case DSP_IMG_PASS_HDR_PREBLEND:
        case DSP_IMG_PASS_HDR_BLEND:
        case DSP_IMG_PASS_WARP:
        case DSP_IMG_PASS_MCTF:
            //no such PassStep in CV1 ext data status
            break;
        default:
            AmbaLL_LogUInt5("[Err @ MsgDispatcher] %d: Unknow PassStepId [0x%X]", __LINE__, pVprocExtDataStatus->pass_step_id, 0U, 0U, 0U);
            break;
        }
#ifdef USE_STAT_IDSP_BUF
    }
#endif
}

static inline void UpdateTransInfoOnVprocAaaStatus(const msg_vproc_aaa_status_t *pVprocAaaStatus)
{
    UINT8 SliceIdx = 0;
    ULONG ULAddr;
    const void *pVoid;
    AMBA_LL_BATCH_IDX_INFO_MAPPING_s BatchQInfo = {0};
    UINT16 ChId = pVprocAaaStatus->channel_id;
    UINT32 Rval;

    SliceIdx = (pVprocAaaStatus->stitch_tile_num_x*pVprocAaaStatus->tile_y_idx) + pVprocAaaStatus->tile_x_idx;
    if (SliceIdx < DSP_MAX_3A_SLICE_NUM) {
        Rval = dsp_osal_cli2virt(pVprocAaaStatus->cfa_aaa_stat_buf_addr, &ULAddr);
        if (Rval == OK) {
            dsp_osal_typecast(&pVoid, &ULAddr);
            Rval = dsp_osal_cache_invalidate(pVoid, pVprocAaaStatus->cfa_aaa_stat_buf_length);
            AmbaMisra_TouchUnused(&Rval);
            {
                // Update BatchCmdId to IsoCfgId
                AMBA_IK_CFA_3A_DATA_s *pDst;

                dsp_osal_typecast(&pDst, &ULAddr);
                BatchQInfo.BatchCmdId = pVprocAaaStatus->batch_cmd_id;
                Rval = DSP_GiveEvent(LL_EVENT_BATCH_INFO_MAPPING, &BatchQInfo, 0U);
                LL_PrintErrLine(Rval, __func__, __LINE__);
                pDst->Header.IsoConfigTag = BatchQInfo.IsoCfgId;
            }
            CfaTransInfo[ChId].SrcAddr[SliceIdx] = ULAddr;
        }

        Rval |= dsp_osal_cli2virt(pVprocAaaStatus->rgb_aaa_stat_buf_addr, &ULAddr);
        if (Rval == OK) {
            dsp_osal_typecast(&pVoid, &ULAddr);
            Rval = dsp_osal_cache_invalidate(pVoid, pVprocAaaStatus->rgb_aaa_stat_buf_length);
            AmbaMisra_TouchUnused(&Rval);
            {
                // Update BatchCmdId to IsoCfgId
                AMBA_IK_PG_3A_DATA_s *pDst;

                dsp_osal_typecast(&pDst, &ULAddr);
                BatchQInfo.BatchCmdId = pVprocAaaStatus->batch_cmd_id;
                Rval = DSP_GiveEvent(LL_EVENT_BATCH_INFO_MAPPING, &BatchQInfo, 0U);
                LL_PrintErrLine(Rval, __func__, __LINE__);
                pDst->Header.IsoConfigTag = BatchQInfo.IsoCfgId;
            }
            PgTransInfo[ChId].SrcAddr[SliceIdx] = ULAddr;
        }

        if (pVprocAaaStatus->vin_stat_buf_addr != 0U) {
            Rval |= dsp_osal_cli2virt(pVprocAaaStatus->vin_stat_buf_addr, &ULAddr);
            if (Rval == OK) {
                dsp_osal_typecast(&pVoid, &ULAddr);
                Rval = dsp_osal_cache_invalidate(pVoid, pVprocAaaStatus->vin_stat_buf_length);
                AmbaMisra_TouchUnused(&Rval);
                { //Fill header
                    AMBA_IK_CFA_HIST_CFG_INFO_s *HistHeader = NULL;

                    dsp_osal_typecast(&HistHeader, &ULAddr);
                    if (HistHeader != NULL) {
                        HistHeader->VinStatsType = DSP_VIN_HIST_MAIN;
                        HistHeader->ChannelIndex = (UINT8)ChId;
                    }
                }
                CfaHistTransInfo[ChId][DSP_VIN_HIST_MAIN].SrcAddr[SliceIdx] = ULAddr;
                ValidVinHist[ChId][DSP_VIN_HIST_MAIN] |= 1U;
            }
        }

        if (pVprocAaaStatus->hdr_vin_stat_buf_addr != 0U) {
            Rval |= dsp_osal_cli2virt(pVprocAaaStatus->hdr_vin_stat_buf_addr, &ULAddr);
            if (Rval == OK) {
                dsp_osal_typecast(&pVoid, &ULAddr);
                Rval = dsp_osal_cache_invalidate(pVoid, pVprocAaaStatus->hdr_vin_stat_buf_length);
                AmbaMisra_TouchUnused(&Rval);
                { //Fill header
                    AMBA_IK_CFA_HIST_CFG_INFO_s *HistHeader = NULL;

                    dsp_osal_typecast(&HistHeader, &ULAddr);
                    if (HistHeader != NULL) {
                        HistHeader->VinStatsType = DSP_VIN_HIST_HDR;
                        HistHeader->ChannelIndex = (UINT8)ChId;
                    }
                }
                CfaHistTransInfo[ChId][DSP_VIN_HIST_HDR].SrcAddr[SliceIdx] = ULAddr;
                ValidVinHist[ChId][DSP_VIN_HIST_HDR] |= 1U;
            }
        }

        if (pVprocAaaStatus->hdr2_vin_stat_buf_addr != 0U) {
            Rval |= dsp_osal_cli2virt(pVprocAaaStatus->hdr2_vin_stat_buf_addr, &ULAddr);
            if (Rval == OK) {
                dsp_osal_typecast(&pVoid, &ULAddr);
                Rval = dsp_osal_cache_invalidate(pVoid, pVprocAaaStatus->hdr2_vin_stat_buf_length);
                AmbaMisra_TouchUnused(&Rval);
                { //Fill header
                    AMBA_IK_CFA_HIST_CFG_INFO_s *HistHeader = NULL;

                    dsp_osal_typecast(&HistHeader, &ULAddr);
                    if (HistHeader != NULL) {
                        HistHeader->VinStatsType = DSP_VIN_HIST_HDR2;
                        HistHeader->ChannelIndex = (UINT8)ChId;
                    }
                }
                CfaHistTransInfo[ChId][DSP_VIN_HIST_HDR2].SrcAddr[SliceIdx] = ULAddr;
                ValidVinHist[ChId][DSP_VIN_HIST_HDR2] |= 1U;
            }
        }

        AAADataValidData[ChId] |= (UINT32)(1UL << SliceIdx);
    }
}

static UINT32 PrevVprocStatCapSeqNo[AMBA_DSP_MAX_VIEWZONE_NUM];
static UINT32 PrevAAAStatCapSeqNo[AMBA_DSP_MAX_VIEWZONE_NUM];
static UINT32 PrevExtStatCapSeqNo[AMBA_DSP_MAX_VIEWZONE_NUM];
static UINT32 PrevExtDataCapSeqNo[AMBA_DSP_MAX_VIEWZONE_NUM];
static UINT32 VprocMsgInit(void)
{
    static DSP_MSG_INIT_s DspMsgInitFlag = {0};
    UINT32 Rval = OK;
    INT32 InitVal;
    UINT32 Val = 0xFFFFFFFFU;

    Rval = dsp_osal_memcpy(&InitVal, &Val, sizeof(UINT32));
    LL_PrintErrLine(Rval, __func__, __LINE__);
    if (DspMsgInitFlag.PrevVprocStatCapSeqNumInit == 0U) {
        Rval |= dsp_osal_memset(PrevVprocStatCapSeqNo, InitVal, sizeof(UINT32)*AMBA_DSP_MAX_VIEWZONE_NUM);
        LL_PrintErrLine(Rval, __func__, __LINE__);
        DspMsgInitFlag.PrevVprocStatCapSeqNumInit = 1U;
    }
    if (DspMsgInitFlag.PrevAAAStatCapSeqNumInit == 0U) {
        Rval |= dsp_osal_memset(PrevAAAStatCapSeqNo, InitVal, sizeof(UINT32)*AMBA_DSP_MAX_VIEWZONE_NUM);
        LL_PrintErrLine(Rval, __func__, __LINE__);
        DspMsgInitFlag.PrevAAAStatCapSeqNumInit = 1U;
    }
    if (DspMsgInitFlag.PrevExtStatCapSeqNumInit == 0U) {
        Rval |= dsp_osal_memset(PrevExtStatCapSeqNo, InitVal, sizeof(UINT32)*AMBA_DSP_MAX_VIEWZONE_NUM);
        LL_PrintErrLine(Rval, __func__, __LINE__);
        DspMsgInitFlag.PrevExtStatCapSeqNumInit = 1U;
    }
    if (DspMsgInitFlag.PrevExtDataCapSeqNumInit == 0U) {
        Rval |= dsp_osal_memset(PrevExtDataCapSeqNo, InitVal, sizeof(UINT32)*AMBA_DSP_MAX_VIEWZONE_NUM);
        LL_PrintErrLine(Rval, __func__, __LINE__);
        DspMsgInitFlag.PrevExtDataCapSeqNumInit = 1U;
    }

    return Rval;
}

static void UpdataCapSeqNoOnVprocExtData(UINT32 *pPreCapSeqNo, const UINT32 VprocCapSeqNo)
{
    if (*pPreCapSeqNo != VprocCapSeqNo) {
//            ExtDataValidData[ChId] |= 1UL<<(MSG_VPROC_STATUS&0xFU);
        *pPreCapSeqNo = VprocCapSeqNo;
    }
}

static void UpdateIdspOnVprocExtData(const msg_vproc_ext_status_t *pVprocExtStatus, const UINT8 ChId) {
    UINT32 Rval = OK;
    IDSP_INFO_s *pIdspInfo = NULL;
#ifdef USE_STAT_IDSP_BUF
    DSP_MEM_BLK_t MemBlk = {0};
#endif

#ifdef USE_STAT_IDSP_BUF
    Rval = DSP_GetStatBuf(DSP_STAT_BUF_IDSP, ChId, &MemBlk);
    if (Rval == OK) {
        dsp_osal_typecast(&pIdspInfo, &MemBlk.Base);
#else
    pIdspInfo = &AmbaDSP_IdspInfo[ChId];
#endif

        /* Update Idsp debug buffer */
        pIdspInfo->vWarpMctf.CapSeqNo = pVprocExtStatus->cap_seq_no;

        // Sec3Cfg
        if (pVprocExtStatus->pass_step_id == DSP_IMG_PASS_WARP) {
            Rval = dsp_osal_cli2virt(pVprocExtStatus->idsp_cfg_hdr_addr[0U], &pIdspInfo->vWarpMctf.Sec3Cfg.CfgAddr);
            LL_PrintErrLine(Rval, __func__, __LINE__);
            pIdspInfo->vWarpMctf.Sec3Cfg.CfgSize = DSP_SEC3_CFG_SIZE;
        }

        // MctfCfg, N/A
        // MctsCfg, N/A
        // Mctf2bit, N/A
#ifdef USE_STAT_IDSP_BUF
    } else {
        AmbaLL_LogUInt5("[Error @ UpdateIdspOnVprocExtData] %d: DSP_GetProtBuf[0x%X]", __LINE__, Rval, 0U, 0U, 0U);
    }
#endif
}

static UINT32 UpdateVprocExtDataInfo(UINT32 MsgCode, const void *pMsgInfo)
{
//    static UINT32 ExtDataValidPattern = (1U<<(MSG_VPROC_STATUS&0xFU)) |
//                                        (1U<<(MSG_VPROC_AAA_STATUS&0xFU)) |
//                                        (1U<<(MSG_VPROC_EXT_STATUS&0xFU)) |
//                                        (1U<<(MSG_VPROC_EXT_DATA_STATUS&0xFU)); //reference
//    static UINT32 ExtDataValidData[AMBA_DSP_MAX_VIEWZONE_NUM] = {0x0};
    UINT32 Rval = OK;
    UINT8 ChId = 0;

    Rval = VprocMsgInit();
    if (Rval != OK) {
        AmbaLL_LogUInt5("[Error @ UpdateVprocExtDataInfo] %d: VprocMsgInit[0x%X]", __LINE__, Rval, 0U, 0U, 0U);
    }

    switch (MsgCode) {
    case MSG_VPROC_STATUS:
    {
        const msg_vproc_status_t *pVprocStatus = NULL;//(msg_vproc_status_t *)pMsgInfo;

        dsp_osal_typecast(&pVprocStatus, &pMsgInfo);
        ChId = pVprocStatus->channel_id;

        UpdataCapSeqNoOnVprocExtData(&PrevVprocStatCapSeqNo[ChId], pVprocStatus->cap_seq_no);

        /* Update tile cfg */
        UpdateIdspTileInfoOnVprocStatus(pVprocStatus);

        break;
    }
    case MSG_VPROC_AAA_STATUS:
    {
        const msg_vproc_aaa_status_t *pVprocAaaStatus = NULL;
        AMBA_LL_BATCH_IDX_INFO_MAPPING_s BatchQInfo = {0};
        UINT8 IsStlProcFirstTile = (UINT8)0U;

        dsp_osal_typecast(&pVprocAaaStatus, &pMsgInfo);
        ChId = pVprocAaaStatus->channel_id;

        if (pVprocAaaStatus->batch_cmd_id > 0U) {
            BatchQInfo.BatchCmdId = pVprocAaaStatus->batch_cmd_id;
            Rval = DSP_GiveEvent(LL_EVENT_BATCH_INFO_MAPPING, &BatchQInfo, 0U);
            LL_PrintErrLine(Rval, __func__, __LINE__);
            if ((BatchQInfo.StlProcId > 0U) &&
                (pVprocAaaStatus->tile_x_idx == 0U) &&
                (pVprocAaaStatus->tile_y_idx == 0U)) {
                IsStlProcFirstTile = (UINT8)1U;
            }
        }

        if ((PrevAAAStatCapSeqNo[ChId] != pVprocAaaStatus->cap_seq_no) ||
            (IsStlProcFirstTile == 1U)) {
//            ExtDataValidData[ChId] |= 1UL<<(MSG_VPROC_AAA_STATUS&0xFU);
            PrevAAAStatCapSeqNo[ChId] = pVprocAaaStatus->cap_seq_no;
            ResetTransInfoOnVprocAaaStatus(pVprocAaaStatus);
        }

        UpdateTransInfoOnVprocAaaStatus(pVprocAaaStatus);

        /*
         * Only update Stat when two conditions meet
         *  1) CapSeqNo increment
         *  2) Whole tile is collected
         */
        if ((AAADataValidData[ChId] == AAADataValidPattern[ChId])) {
            UpdateIdspInfoOnVprocAaaStatus(pVprocAaaStatus);
        }
        break;
    }
    case MSG_VPROC_EXT_STATUS:
    {
        const msg_vproc_ext_status_t *pVprocExtStatus = NULL;// = (msg_vproc_ext_status_t *) pMsgInfo;

        dsp_osal_typecast(&pVprocExtStatus, &pMsgInfo);
        ChId = pVprocExtStatus->channel_id;

        UpdataCapSeqNoOnVprocExtData(&PrevExtStatCapSeqNo[ChId], pVprocExtStatus->cap_seq_no);

        /* Update Idsp debug buffer */
        UpdateIdspOnVprocExtData(pVprocExtStatus, ChId);

        break;
    }
    case MSG_VPROC_EXT_DATA_STATUS:
    {
        const msg_vproc_ext_data_status_t *pVprocExtDataStatus = NULL; // = (msg_vproc_ext_data_status_t *) pMsgInfo;

        dsp_osal_typecast(&pVprocExtDataStatus, &pMsgInfo);
        ChId = pVprocExtDataStatus->channel_id;

        UpdataCapSeqNoOnVprocExtData(&PrevExtDataCapSeqNo[ChId], pVprocExtDataStatus->cap_seq_no);

        UpdateIdspInfoOnVprocExtDataStatus(pVprocExtDataStatus);

        break;
    }
    default:
        AmbaLL_LogUInt5("[Error @ MsgDispatcher] %d: Unknow MsgCode[0x%X]", __LINE__, MsgCode, 0U, 0U, 0U);
        break;
    }

//    if ((ExtDataValidData[ChId] == ExtDataValidPattern) &&
//        ((TotalAAASliceNum[ChId] != 0U) && (AAASliceIdx[ChId] == 0U))) {
//        ExtDataValidData[ChId] |= 1UL<<(MSG_VPROC_EXT_DATA_STATUS&0xFU);
//
//        ExtDataValidData[ChId] = 0x0U;
//    }

    return Rval;
}

#define PTS_ENCODE_DISABLE  (0x1U)
#define PTS_CAPTURE_DISABLE (0x2U)
#define PTS_CABAC_DISABLE   (0x4U)
/**
 * Modified PTS in case of 64-bit overflow
 * @param [in] pEncStrmInfo Pointer to the encode stream information (local info inside AmbaDSP)
 * @param [in] pBitsInfo Pointer to the bitstream descriptor (info from DSP)
 * @param [in] Op Operation mode
 */
//FIXME, to be refined?
static void PTSOverflowDetection(AMBA_DSP_ENC_STRM_INFO_s *pEncStrmInfo, const enc_bits_info_t *pBitsInfo, UINT8 Op)
{
    UINT32 PtsDiff = 0U;
    UINT32 Pts_32 = 0U;
    UINT64 Pts_64 = 0U;

    /* Handle Capture pts overflow */
    if (0U == (Op & PTS_CAPTURE_DISABLE)) {
        Pts_32 = pBitsInfo->pts_hw;
        Pts_64 = (UINT64)Pts_32;
        if (pEncStrmInfo->CapturePts32 <= Pts_32) {
            PtsDiff = (Pts_32 - pEncStrmInfo->CapturePts32);
        } else {
            PtsDiff = (pEncStrmInfo->CapturePts32 - Pts_32);
        }

        if (0x0FFFFFFFUL < PtsDiff) {
            pEncStrmInfo->CapturePtsBase += 0x100000000ULL;
            Pts_64                    += pEncStrmInfo->CapturePtsBase;
            pEncStrmInfo->CapturePts32 = Pts_32;
            pEncStrmInfo->CapturePts64 = Pts_64;
        } else {
            Pts_64                      += pEncStrmInfo->CapturePtsBase;
            pEncStrmInfo->CapturePts32   = Pts_32;
            pEncStrmInfo->CapturePts64   = Pts_64;
        }
    }

    /* Handle Cabac pts overflow */
    if (0U == (Op & PTS_CABAC_DISABLE)) {
        Pts_32 = pBitsInfo->encode_done_pts;
        Pts_64 = (UINT64)Pts_32;
        if (pEncStrmInfo->CabacPts32 <= Pts_32) {
            PtsDiff = (Pts_32 - pEncStrmInfo->CabacPts32);
        } else {
            PtsDiff = (pEncStrmInfo->CabacPts32);// - Pts_32);
        }

        if (0x0FFFFFFFUL < PtsDiff) {
            pEncStrmInfo->CabacPtsBase += 0x100000000ULL;
            Pts_64                  += pEncStrmInfo->CabacPtsBase;
            pEncStrmInfo->CabacPts32 = Pts_32;
            pEncStrmInfo->CabacPts64 = Pts_64;
        } else {
            Pts_64                    += pEncStrmInfo->CabacPtsBase;
            pEncStrmInfo->CabacPts32   = Pts_32;
            pEncStrmInfo->CabacPts64   = Pts_64;
        }
    }

}

static ULONG MsgEncBitsBase;
static UINT32 MsgEncBitsSize;
void LL_SetMsgDispatcherBitsInfo(ULONG BitsBase, UINT32 BitsSize)
{
    MsgEncBitsBase = BitsBase;
    MsgEncBitsSize = BitsSize;
}

void LL_SetMsgDispatcherDecoderUsingStatusEos(UINT8 Enable)
{
    UseDecEos = Enable;
}

void LL_PrintBatchInfoId(UINT32 BatchInfoId)
{
    AMBA_LL_BATCH_IDX_INFO_MAPPING_s BatchQInfo = {0};
    UINT32 Rval;

    BatchQInfo.BatchCmdId = BatchInfoId;
    Rval = DSP_GiveEvent(LL_EVENT_BATCH_INFO_MAPPING, &BatchQInfo, 0U);
    if (Rval == OK) {
        AmbaLL_Log(AMBALLLOG_TYPE_MSG,"   IsoCfgId               = 0x%X", BatchQInfo.IsoCfgId, 0U, 0U);
        AmbaLL_Log(AMBALLLOG_TYPE_MSG,"   ImgPrmdId              = 0x%X", BatchQInfo.ImgPrmdId, 0U, 0U);
        AmbaLL_Log(AMBALLLOG_TYPE_MSG,"   YuvStrmGrpId           = 0x%X", BatchQInfo.YuvStrmGrpId, 0U, 0U);
        AmbaLL_Log(AMBALLLOG_TYPE_MSG,"   StlProcId              = 0x%X", BatchQInfo.StlProcId, 0U, 0U);
    }
}

static inline void EncBitsInfoReset(void)
{
    UINT16 i;
    UINT32 Rval;

    Rval = dsp_osal_memset(&EncBitsInfo, 0, sizeof(AMBA_DSP_ENC_CHAN_INFO_s));
    LL_PrintErrLine(Rval, __func__, __LINE__);
    for (i=0U; i<AMBA_DSP_MAX_STREAM_NUM; i++) {
        EncBitsInfo.Stream[i].MvPoolDesc.StartAddr = 0U;
        EncBitsInfo.Stream[i].MvPoolDesc.Size = MAX_ENC_BUF_INFO_NUM;
        EncBitsInfo.Stream[i].MvPoolDesc.Wp = (MAX_ENC_BUF_INFO_NUM - 1U);
        EncBitsInfo.Stream[i].MvPoolDesc.Rp = (MAX_ENC_BUF_INFO_NUM - 1U);
        Rval = dsp_osal_memset(&EncBitsInfo.Stream[i].MvAddr[0U], 0, sizeof(UINT32)*MAX_ENC_BUF_INFO_NUM);
        LL_PrintErrLine(Rval, __func__, __LINE__);
    }
}

/**
 * Reset EncBitsInfo if all the encoders are idle
 */
static void ResetBitsInfo(void)
{
    AMBA_LL_ENC_GET_ACTIVE_BIT_s GetActiveBitInfo = {0};
    UINT32 Rval;

    Rval = DSP_GiveEvent(LL_EVENT_ENC_GET_ACTIVE_BIT, &GetActiveBitInfo, 0U);
    LL_PrintErrLine(Rval, __func__, __LINE__);

    if (GetActiveBitInfo.ActiveBit == 0U) {
        AmbaVideoBitsDescIdx = 0U;
        EncBitsInfoReset();
    }
}

static inline void UpdateAvcMvBuf(const enc_status_msg_t *pEncStatus)
{
    UINT32 Rval;
    UINT32 NewWp;
    ULONG ULAddr;

    if (pEncStatus->mvdump_curr_daddr > 0U) {
        AMBA_DSP_ENC_STRM_INFO_s *pEncStrmInfo;

        pEncStrmInfo = &(EncBitsInfo.Stream[pEncStatus->stream_id]);

        Rval = dsp_osal_cli2virt(pEncStatus->mvdump_curr_daddr, &ULAddr);
        if (Rval == OK) {
            if (ULAddr != pEncStrmInfo->PrevMvAddr) {
                Rval = DSP_ReqBuf(&pEncStrmInfo->MvPoolDesc, 1U, &NewWp, 0U/*FullnessCheck*/);
                if (Rval != OK) {
                    AmbaLL_LogUInt5("[%d] MvBufReq Fail[%d %d %d]",
                                    pEncStatus->stream_id, pEncStrmInfo->MvPoolDesc.Size,
                                    pEncStrmInfo->MvPoolDesc.Wp, pEncStrmInfo->MvPoolDesc.Rp, 0U);
                } else {
                    pEncStrmInfo->MvAddr[NewWp] = ULAddr;
                    pEncStrmInfo->PrevMvAddr = ULAddr;
                    pEncStrmInfo->MvPoolDesc.Wp = NewWp;
                }
            }
        } else {
            AmbaLL_LogUInt5("[%d] MvBufReq dsp_osal_cli2virt return [%d]", __LINE__, Rval, 0U, 0U, 0U);
        }
    }
}

static inline void ConsumeAvcMvBuf(const AMBA_DSP_ENC_PIC_RDY_s *pPicInfo, UINT32 CapSeqNo)
{
    UINT32 Rval;
    AMBA_DSP_ENC_STRM_INFO_s *pEncStrmInfo;
    UINT32 NewRp;

    pEncStrmInfo = &(EncBitsInfo.Stream[pPicInfo->StreamId]);

    /* Report MV when last slice */
    if ((pEncStrmInfo->PrevMvAddr != 0x0U) &&
        (pPicInfo->SliceIdx == (pPicInfo->NumSlice - 1U))) {
        Rval = DSP_RelBuf(&pEncStrmInfo->MvPoolDesc, 1U, &NewRp, 0U/*FullnessCheck*/);
        if (Rval != OK) {
            AmbaLL_LogUInt5("[%d] MvBufRel Fail[%d %d %d]",
                            pPicInfo->StreamId, pEncStrmInfo->MvPoolDesc.Size,
                            pEncStrmInfo->MvPoolDesc.Wp, pEncStrmInfo->MvPoolDesc.Rp, 0U);
        } else {
            AMBA_DSP_ENC_MV_RDY_s *pMvInfo = NULL;
            ULONG EventBufAddr = 0U;
            UINT16 EventBufIdx = 0U;

            pEncStrmInfo->MvPoolDesc.Rp = NewRp;
            Rval = DSP_RequestEventInfoBuf(EVENT_INFO_POOL_PROT_BUF, &EventBufAddr, &EventBufIdx, LL_EVENT_VIDEO_MV_DATA_RDY);
            LL_PrintErrLine(Rval, __func__, __LINE__);
            if (Rval == OK) {
                dsp_osal_typecast(&pMvInfo, &EventBufAddr);
                Rval = dsp_osal_memset(pMvInfo, 0, sizeof(AMBA_DSP_ENC_MV_RDY_s));
                LL_PrintErrLine(Rval, __func__, __LINE__);

                pMvInfo->StreamId = pPicInfo->StreamId;
                pMvInfo->MvBufType = (UINT8)2U;
                pMvInfo->CapPts = 0ULL;
                pMvInfo->CapSequence = CapSeqNo;
                pMvInfo->MvBufAddr = pEncStrmInfo->MvAddr[NewRp];
                Rval = DSP_GiveEvent(LL_EVENT_VIDEO_MV_DATA_RDY, pMvInfo, EventBufIdx);
                LL_PrintErrLine(Rval, __func__, __LINE__);
            }
        }
    }
}

static void SendEncInfoImp(const UINT8 CodingType,
                           AMBA_DSP_ENC_PIC_RDY_s *pPicInfo,
                           const enc_bits_info_t *pBitsInfo,
                           AMBA_DSP_ENC_STRM_INFO_s *pEncStrmInfo,
                           UINT32 *pLogType)
{
    pPicInfo->CaptureTimeStamp = pEncStrmInfo->CapturePts64;
    pPicInfo->EncodeTimeStamp = pEncStrmInfo->CabacPts64;
    // SSP's pre-proc function will fill-in software-pts
    pPicInfo->Pts = 0ULL;

    pPicInfo->FrameType = pBitsInfo->pic_type;
    pPicInfo->PicStruct = pBitsInfo->pic_struct;
    pPicInfo->PicLevel = pBitsInfo->pic_level;
    if (pBitsInfo->bits_size == DSP_ENC_END_MARK) {
        pPicInfo->PicSize = AMBA_DSP_ENC_END_MARK;
    } else {
        pPicInfo->PicSize = pBitsInfo->bits_size;
    }

    pPicInfo->TileIdx = (UINT8)pBitsInfo->tile_id;
    pPicInfo->NumTile = (UINT8)pBitsInfo->tile_num;
    pPicInfo->SliceIdx = (UINT8)pBitsInfo->slice_idx;
    pPicInfo->NumSlice = (UINT8)pBitsInfo->slice_num;
    if (pPicInfo->NumTile == 0U) {
        pPicInfo->NumTile = 1U;
    }

    if (pEncStrmInfo->InfoRCnt != pEncStrmInfo->InfoWCnt) {
        pPicInfo->InputYAddr = pEncStrmInfo->BufInfo[pEncStrmInfo->InfoRCnt].InputYAddr;
        pPicInfo->InputUVAddr = pEncStrmInfo->BufInfo[pEncStrmInfo->InfoRCnt].InputUVAddr;
        pPicInfo->InputYuvPitch = pEncStrmInfo->BufInfo[pEncStrmInfo->InfoRCnt].InputYuvPitch;
    }
    if ((pPicInfo->SliceIdx == (pPicInfo->NumSlice - 0x1U)) && (pPicInfo->TileIdx == (pPicInfo->NumTile - 0x1U))) {
        //Last tile/slice for the frame, update the Rp of buf info
        pEncStrmInfo->InfoRCnt++;
        pEncStrmInfo->InfoRCnt = pEncStrmInfo->InfoRCnt%MAX_ENC_BUF_INFO_NUM;
    }

    if (CodingType == DSP_ENC_FMT_H264) {
        *pLogType = AMBALLLOG_TYPE_VIDEO_H264;
    } else if (CodingType == DSP_ENC_FMT_H265) {
        *pLogType = AMBALLLOG_TYPE_VIDEO_HEVC;
    } else {
        *pLogType = AMBALLLOG_TYPE_JPEG;
    }
}

static UINT32 SendEncInfoVdoPtsTrace(const UINT8 CodingType,
                                     AMBA_DSP_ENC_PIC_RDY_s *pPicInfo,
                                     const enc_bits_info_t *pBitsInfo,
                                     AMBA_DSP_ENC_STRM_INFO_s *pEncStrmInfo)
{
    UINT32 Rval = OK;
    UINT8 Op = 0U;

    if (DSP_ENC_FMT_JPEG != CodingType) {
        pPicInfo->CaptureTimeStamp = (UINT64)pBitsInfo->pts_hw;
        Rval = DSP_GiveEvent(LL_EVENT_VIDEO_PTS_TRACE, pPicInfo, 0U);
        if (OK == Rval) {
            // SSP's pre-proc function will fill-in complete CaptureTimeStamp and EncodeTimeStamp
            pEncStrmInfo->CapturePtsBase = 0U;
            pEncStrmInfo->CapturePts32 = pBitsInfo->pts_hw;
            // Bit[63:32]: CaptureSeq Bit[31:0]: CapturePts
            pEncStrmInfo->CapturePts64 = ((UINT64)pBitsInfo->cap_seq_no << 32U) + (UINT64)pBitsInfo->pts_hw;
            pEncStrmInfo->CabacPtsBase = 0U;
            pEncStrmInfo->CabacPts32 = pBitsInfo->encode_done_pts;
            pEncStrmInfo->CabacPts64 = (UINT64)pBitsInfo->encode_done_pts;

            Op = (UINT8)(PTS_ENCODE_DISABLE | PTS_CAPTURE_DISABLE | PTS_CABAC_DISABLE);
        }
    }
    PTSOverflowDetection(pEncStrmInfo, pBitsInfo, Op);

    return Rval;
}

static UINT32 SendEncInfoDataRdyEvt(const UINT8 CodingType, const AMBA_DSP_ENC_PIC_RDY_s *pPicInfo, const enc_bits_info_t *pBitsInfo)
{
    UINT32 Rval = OK;
    AMBA_DSP_ENC_PIC_RDY_s *pEvtPicInfo = NULL;
    ULONG EventBufAddr = 0U;
    UINT16 EventBufIdx = 0U;

    if ((DSP_ENC_FMT_H264 == CodingType) ||
        (DSP_ENC_FMT_H265 == CodingType) ||
        (DSP_ENC_FMT_JPEG == CodingType)) {
        Rval = DSP_RequestEventInfoBuf(EVENT_INFO_POOL_PROT_BUF, &EventBufAddr, &EventBufIdx, LL_EVENT_VIDEO_DATA_RDY);
        LL_PrintErrLine(Rval, __func__, __LINE__);
        if (Rval == OK) {
            dsp_osal_typecast(&pEvtPicInfo, &EventBufAddr);
            Rval = dsp_osal_memcpy(pEvtPicInfo, pPicInfo, sizeof(AMBA_DSP_ENC_PIC_RDY_s));
            LL_PrintErrLine(Rval, __func__, __LINE__);
            Rval = DSP_GiveEvent(LL_EVENT_VIDEO_DATA_RDY, pEvtPicInfo, EventBufIdx);
            LL_PrintErrLine(Rval, __func__, __LINE__);
        }

        if (DSP_ENC_FMT_H264 == CodingType) {
            ConsumeAvcMvBuf(pPicInfo, pBitsInfo->cap_seq_no);
        }
    }

    if (DSP_ENC_FMT_JPEG == CodingType) {
        Rval = DSP_RequestEventInfoBuf(EVENT_INFO_POOL_PROT_BUF, &EventBufAddr, &EventBufIdx, LL_EVENT_JPEG_DATA_RDY);
        LL_PrintErrLine(Rval, __func__, __LINE__);
        if (Rval == OK) {
            dsp_osal_typecast(&pEvtPicInfo, &EventBufAddr);
            Rval = dsp_osal_memcpy(pEvtPicInfo, pPicInfo, sizeof(AMBA_DSP_ENC_PIC_RDY_s));
            LL_PrintErrLine(Rval, __func__, __LINE__);
            Rval = DSP_GiveEvent(LL_EVENT_JPEG_DATA_RDY, pEvtPicInfo, EventBufIdx);
            LL_PrintErrLine(Rval, __func__, __LINE__);
        }
    }

    return Rval;
}

/**
 * Send the event containing encoder bitstream information to upper layer
 * @param [in] CodingType Coding type of a specified stream (different type, differnt event)
 * @param [in] FrmCnt Count of a specified stream (one frame, one event)
 * @param [in] ConsumedCount Consumed Count
 * @return 0 - OK, others - ErrorCode
 */
static UINT32 SendEncInfo(UINT8 CodingType, UINT32 FrmCnt, UINT32 *ConsumedCount)
{
    UINT32 Rval = OK;
    UINT32 Cnt;
    const enc_bits_info_t *pBitsInfo = NULL; // BsDesc from uCode
    AMBA_DSP_ENC_PIC_RDY_s PicInfo = {0}; // BsDesc to App
    AMBA_DSP_EVENT_ERROR_INFO_s *pErrInfo = NULL;
    AMBA_DSP_ENC_STRM_INFO_s *pEncStrmInfo = NULL;
    UINT32 LogType;
    ULONG BitsStartVirtAddr;
    DSP_MEM_BLK_t MemBlk = {0};
    ULONG EventBufAddr = 0U;
    UINT16 EventBufIdx = 0U;

    if (FrmCnt > 256U) {
        AmbaLL_LogWarnUInt5((UINT16)AMBALLLOG_WARN_FRMCNT_FULL, "[%d] Unreasonable FrmCnt (%u %u)", CodingType, FrmCnt, *ConsumedCount, 0U, 0U);
        Rval = DSP_ERR_0001;
    } else {
        for (Cnt = 0U; Cnt < FrmCnt; Cnt++) {
            ULONG ULAddr;

            Rval = DSP_GetProtBuf(DSP_PROT_BUF_VENC_DESC_BUF, 0U, &MemBlk);
            LL_PrintErrLine(Rval, __func__, __LINE__);
            if (Rval == OK) {
                ULAddr = MemBlk.Base + (((ULONG)AmbaVideoBitsDescIdx * DSP_ENC_BITS_INFO_STEPSIZE)*sizeof(UINT8));

                /* Get bits info from info buffer */
                dsp_osal_typecast(&pBitsInfo, &ULAddr);

                /* Reassign value to structure used at upper service */
                PicInfo.StreamId = (UINT8)pBitsInfo->stream_id;

                if (pBitsInfo->bits_size != DSP_ENC_END_MARK) {
                    Rval = dsp_osal_cli2virt(pBitsInfo->bits_start_addr, &BitsStartVirtAddr);
                    LL_PrintErrLine(Rval, __func__, __LINE__);
                } else {
                    BitsStartVirtAddr = pBitsInfo->bits_start_addr;
                }

                if ((pBitsInfo->stream_id >= AMBA_DSP_MAX_STREAM_NUM) || \
                    ((pBitsInfo->bits_size != DSP_ENC_END_MARK) && \
                     ((BitsStartVirtAddr < MsgEncBitsBase) || \
                      (BitsStartVirtAddr >= (MsgEncBitsBase + MsgEncBitsSize)) || \
                      (pBitsInfo->bits_size > MsgEncBitsSize)))) {
                    //EncDbgDump(pBitsInfo);
                    Rval = DSP_RequestEventInfoBuf(EVENT_INFO_POOL_PROT_BUF, &EventBufAddr, &EventBufIdx, LL_EVENT_ERROR);
                    LL_PrintErrLine(Rval, __func__, __LINE__);
                    if (Rval == OK) {
                        dsp_osal_typecast(&pErrInfo, &EventBufAddr);
                        pErrInfo->Type = AMBA_DSP_ERROR_BITSINFO;
                        pErrInfo->Info = 0U;
                        Rval = DSP_GiveEvent(LL_EVENT_ERROR, pErrInfo, EventBufIdx);
                        LL_PrintErrLine(Rval, __func__, __LINE__);
                    }

                    //still going on
                    *ConsumedCount = *ConsumedCount + 1U;
                    AmbaVideoBitsDescIdx++;
                    if (AMBA_DSP_DESC_BUFFER_NUM <= AmbaVideoBitsDescIdx) {
                        AmbaVideoBitsDescIdx = 0U;
                    }
                } else {
                    pEncStrmInfo = &(EncBitsInfo.Stream[pBitsInfo->stream_id]);
                    PicInfo.FrmNo = pBitsInfo->frame_num;

                    Rval = SendEncInfoVdoPtsTrace(CodingType, &PicInfo, pBitsInfo, pEncStrmInfo);
                    LL_PrintErrLine(Rval, __func__, __LINE__);

                    PicInfo.StartAddr = BitsStartVirtAddr;
                    SendEncInfoImp(CodingType, &PicInfo, pBitsInfo, pEncStrmInfo, &LogType);

                    Rval = SendEncInfoDataRdyEvt(CodingType, &PicInfo, pBitsInfo);
                    LL_PrintErrLine(Rval, __func__, __LINE__);

                    *ConsumedCount = *ConsumedCount + 1U;
                    AmbaVideoBitsDescIdx = (AmbaVideoBitsDescIdx+1U)%AMBA_DSP_DESC_BUFFER_NUM;
                }
            }
        }
    }
    return Rval;
}

static inline void CalcVdoDataNum(const enc_status_msg_t *pEncStatus,
                                  AMBA_DSP_ENC_STRM_INFO_s *pEncStrmInfo,
                                  UINT32 *pH264Cnt,
                                  UINT32 *pHevcCnt,
                                  UINT32 *pJpegCnt)
{
    UINT16 i = 0U;
    UINT32 TotalH264Count = 0U, TotalH264ConsumedCount = 0U;
    UINT32 TotalHevcCount = 0U, TotalHevcConsumedCount = 0U;
    UINT32 TotalJpegCount = 0U, TotalJpegConsumedCount = 0U;

    if (pEncStatus->total_bits_info_ctr_h264 > 0U) {
        UINT32 PrevH264Cnt = 0U;

        for (i = 0U; i < AMBA_DSP_MAX_STREAM_NUM; i++) {
            PrevH264Cnt += EncBitsInfo.Stream[i].H264Count;
        }

        if (pEncStatus->total_bits_info_ctr_h264 >= PrevH264Cnt) {
             pEncStrmInfo->H264Count += (pEncStatus->total_bits_info_ctr_h264 - PrevH264Cnt);
        } else {
            //AmbaLL_LogUInt5("Err? AVC %d %d", pEncStatus->total_bits_info_ctr_h264, PrevH264Cnt, 0U, 0U, 0U);
        }
    }

    if (pEncStatus->total_bits_info_ctr_hevc > 0U) {
        UINT32 PrevHevcCnt = 0U;
        for (i = 0U; i < AMBA_DSP_MAX_STREAM_NUM; i++) {
            PrevHevcCnt += EncBitsInfo.Stream[i].HevcCount;
        }

        if (pEncStatus->total_bits_info_ctr_hevc >= PrevHevcCnt)  {
            pEncStrmInfo->HevcCount += (pEncStatus->total_bits_info_ctr_hevc - PrevHevcCnt);
        } else {
            //AmbaLL_LogUInt5("Err? HEVC %d %d", pEncStatus->total_bits_info_ctr_hevc, PrevHevcCnt, 0U, 0U, 0U);
        }
    }

    if (pEncStatus->total_bits_info_ctr_jpeg > 0U) {
        UINT32 PrevJpegCnt = 0U;
        for (i = 0U; i < AMBA_DSP_MAX_STREAM_NUM; i++) {
            PrevJpegCnt += EncBitsInfo.Stream[i].JpegCount;
        }
        if (pEncStatus->total_bits_info_ctr_jpeg >= PrevJpegCnt) {
            pEncStrmInfo->JpegCount += (pEncStatus->total_bits_info_ctr_jpeg - PrevJpegCnt);
        } else {
            //AmbaLL_LogUInt5("Err? JPG %d %d", pEncStatus->total_bits_info_ctr_jpeg, PrevJpegCnt, 0U, 0U, 0U);
        }
    }

    for (i = 0U; i < AMBA_DSP_MAX_STREAM_NUM; i++) {
        TotalH264Count += EncBitsInfo.Stream[i].H264Count;
        TotalH264ConsumedCount += EncBitsInfo.Stream[i].H264ConsumedCount;
    }
    *pH264Cnt = TotalH264Count - TotalH264ConsumedCount;

    for (i = 0U; i < AMBA_DSP_MAX_STREAM_NUM; i++) {
        TotalHevcCount += EncBitsInfo.Stream[i].HevcCount;
        TotalHevcConsumedCount += EncBitsInfo.Stream[i].HevcConsumedCount;
    }
    *pHevcCnt = TotalHevcCount - TotalHevcConsumedCount;

    for (i = 0U; i < AMBA_DSP_MAX_STREAM_NUM; i++) {
        TotalJpegCount += EncBitsInfo.Stream[i].JpegCount;
        TotalJpegConsumedCount += EncBitsInfo.Stream[i].JpegConsumedCount;
    }
    *pJpegCnt = TotalJpegCount - TotalJpegConsumedCount;
}

static inline void ProcVdoDataNum(AMBA_DSP_ENC_STRM_INFO_s *pEncStrmInfo,
                                  UINT32 H264Cnt,
                                  UINT32 HevcCnt,
                                  UINT32 JpegCnt)
{
    UINT32 Rval;
    UINT32 ConsumedCount = 0U;

    if (H264Cnt > 0U) {
        ConsumedCount = 0U;
        Rval = SendEncInfo(DSP_ENC_FMT_H264, H264Cnt, &ConsumedCount);
        if (Rval != OK) {
            AmbaLL_LogWarnUInt5((UINT16)AMBALLLOG_WARN_SEND_ENCINFO, "SendEncInfo AVC NG", 0U, 0U, 0U, 0U, 0U);
        } else {
            pEncStrmInfo->H264ConsumedCount += ConsumedCount;
        }
    }
    if (HevcCnt > 0U) {
        ConsumedCount = 0U;
        Rval = SendEncInfo(DSP_ENC_FMT_H265, HevcCnt, &ConsumedCount);
        if (Rval != OK) {
            AmbaLL_LogWarnUInt5((UINT16)AMBALLLOG_WARN_SEND_ENCINFO, "SendEncInfo HEVC NG", 0U, 0U, 0U, 0U, 0U);
        } else {
            pEncStrmInfo->HevcConsumedCount += ConsumedCount;
        }
    }
    if (JpegCnt > 0U) {
        ConsumedCount = 0U;
        Rval = SendEncInfo(DSP_ENC_FMT_JPEG, JpegCnt, &ConsumedCount);
        if (Rval != OK) {
            AmbaLL_LogWarnUInt5((UINT16)AMBALLLOG_WARN_SEND_ENCINFO, "SendEncInfo JPG NG", 0U, 0U, 0U, 0U, 0U);
        } else {
            pEncStrmInfo->JpegConsumedCount += ConsumedCount;
        }
    }
}

/**
 * Handle encoder input buffer information
 * @param [in] pEncBufStatus Pointer to the DSP message about memd input buffer
 * @return 0 - OK, others - ErrorCode
 */
static UINT32 HandlerEncInputBufStatus(const enc_input_buffer_status_msg_t *pEncBufStatus)
{
    UINT32 Rval = OK;

    if (DSP_ShowCmdMsgLog(MSG_ENC_INPUT_BUFFER_STATUS) != 0U) {
        AmbaLL_Log(AMBALLLOG_TYPE_MSG,"MSG_ENC_INPUT_BUFFER_STATUS", 0U, 0U, 0U);
        AmbaLL_Log(AMBALLLOG_TYPE_MSG," channel_id                 = %d  ", pEncBufStatus->channel_id, 0U, 0U);
        AmbaLL_Log(AMBALLLOG_TYPE_MSG," enc_status                 = %d  ", pEncBufStatus->enc_status, 0U, 0U);
        AmbaLL_Log(AMBALLLOG_TYPE_MSG," pic_type                   = %d  ", pEncBufStatus->pic_type, 0U, 0U);
        AmbaLL_Log(AMBALLLOG_TYPE_MSG," pic_is_idr                 = %d  ", pEncBufStatus->pic_is_idr, 0U, 0U);
        AmbaLL_Log(AMBALLLOG_TYPE_MSG," is_yuv_addr_valid          = %d  ", pEncBufStatus->is_yuv_addr_valid, 0U, 0U);
        AmbaLL_Log(AMBALLLOG_TYPE_MSG," is_me_addr_valid           = %d  ", pEncBufStatus->is_me_addr_valid, 0U, 0U);
        AmbaLL_Log(AMBALLLOG_TYPE_MSG," input_y_addr               = 0x%X", pEncBufStatus->input_y_addr, 0U, 0U);
        AmbaLL_Log(AMBALLLOG_TYPE_MSG," input_uv_addr              = 0x%X", pEncBufStatus->input_uv_addr, 0U, 0U);
        AmbaLL_Log(AMBALLLOG_TYPE_MSG," input_y_pitch              = %d  ", pEncBufStatus->input_y_pitch, 0U, 0U);
        AmbaLL_Log(AMBALLLOG_TYPE_MSG," input_me_addr              = 0x%X", pEncBufStatus->input_me_addr, 0U, 0U);
        AmbaLL_Log(AMBALLLOG_TYPE_MSG," input_me_pitch             = %d  ", pEncBufStatus->input_me_pitch, 0U, 0U);
        AmbaLL_Log(AMBALLLOG_TYPE_MSG," frame_no                   = %d  ", pEncBufStatus->frame_no, 0U, 0U);
        AmbaLL_Log(AMBALLLOG_TYPE_MSG," hw_pts                     = %d  ", pEncBufStatus->hw_pts, 0U, 0U);
        AmbaLL_Log(AMBALLLOG_TYPE_MSG," sw_pts                     = %d  ", pEncBufStatus->sw_pts, 0U, 0U);
    }

    return Rval;
}

/**
 * Handle encoder recon buffer information
 * @param [in] pEncBufStatus Pointer to the DSP message about memd input buffer
 * @return 0 - OK, others - ErrorCode
 */
static UINT32 HandlerEncBufStatus(const enc_buffer_status_msg_t *pEncBufStatus)
{
    UINT32 Rval = OK;
    AMBA_DSP_ENC_STRM_INFO_s *pEncStrmInfo;
    UINT32 StrmId = pEncBufStatus->channel_id;
    UINT16 i;

    if (DSP_ShowCmdMsgLog(MSG_ENC_BUFFER_STATUS) != 0U) {
        AmbaLL_Log(AMBALLLOG_TYPE_MSG,"MSG_ENC_BUFFER_STATUS", 0U, 0U, 0U);
        AmbaLL_Log(AMBALLLOG_TYPE_MSG," channel_id                 = %d  ", pEncBufStatus->channel_id, 0U, 0U);
        AmbaLL_Log(AMBALLLOG_TYPE_MSG," enc_type                   = %d  ", pEncBufStatus->enc_type, 0U, 0U);
        AmbaLL_Log(AMBALLLOG_TYPE_MSG," enc_status                 = %d  ", pEncBufStatus->enc_status, 0U, 0U);
        AmbaLL_Log(AMBALLLOG_TYPE_MSG," pic_type                   = %d  ", pEncBufStatus->pic_type, 0U, 0U);
        AmbaLL_Log(AMBALLLOG_TYPE_MSG," frame_no                   = %d  ", pEncBufStatus->frame_no, 0U, 0U);
        AmbaLL_Log(AMBALLLOG_TYPE_MSG," cap_seq_no                 = %d  ", pEncBufStatus->cap_seq_no, 0U, 0U);
        AmbaLL_Log(AMBALLLOG_TYPE_MSG," hw_pts                     = %d  ", pEncBufStatus->hw_pts, 0U, 0U);
        AmbaLL_Log(AMBALLLOG_TYPE_MSG," sw_pts                     = %d  ", pEncBufStatus->sw_pts, 0U, 0U);
        AmbaLL_Log(AMBALLLOG_TYPE_MSG," recon", 0U, 0U, 0U);
        AmbaLL_Log(AMBALLLOG_TYPE_MSG,"  buffer_addr_y                    = 0x%X", pEncBufStatus->recon.buffer_addr_y, 0U, 0U);
        AmbaLL_Log(AMBALLLOG_TYPE_MSG,"  buffer_addr_uv                   = 0x%X", pEncBufStatus->recon.buffer_addr_uv, 0U, 0U);
        AmbaLL_Log(AMBALLLOG_TYPE_MSG,"  buffer_pitch                     = %d  ", pEncBufStatus->recon.buffer_pitch, 0U, 0U);
        AmbaLL_Log(AMBALLLOG_TYPE_MSG,"  img_width                        = %d  ", pEncBufStatus->recon.img_width, 0U, 0U);
        AmbaLL_Log(AMBALLLOG_TYPE_MSG,"  img_height_y                     = %d  ", pEncBufStatus->recon.img_height_y, 0U, 0U);
        AmbaLL_Log(AMBALLLOG_TYPE_MSG,"  img_height_uv                    = %d  ", pEncBufStatus->recon.img_height_uv, 0U, 0U);
        for (i = 0U; i<2U; i++) {
            AmbaLL_Log(AMBALLLOG_TYPE_MSG," enc_stat[%d]", i, 0U, 0U);
            AmbaLL_Log(AMBALLLOG_TYPE_MSG,"  buffer_addr                      = 0x%X", pEncBufStatus->enc_stat[i].buffer_addr, 0U, 0U);
            AmbaLL_Log(AMBALLLOG_TYPE_MSG,"  buffer_pitch                     = %d  ", pEncBufStatus->enc_stat[i].buffer_pitch, 0U, 0U);
            AmbaLL_Log(AMBALLLOG_TYPE_MSG,"  img_width                        = %d  ", pEncBufStatus->enc_stat[i].img_width, 0U, 0U);
            AmbaLL_Log(AMBALLLOG_TYPE_MSG,"  img_height                       = %d  ", pEncBufStatus->enc_stat[i].img_height, 0U, 0U);

        }
    }

    pEncStrmInfo = &(EncBitsInfo.Stream[StrmId]);

    Rval = dsp_osal_cli2virt(pEncBufStatus->recon.buffer_addr_y, &pEncStrmInfo->BufInfo[pEncStrmInfo->InfoWCnt].InputYAddr);
    LL_PrintErrLine(Rval, __func__, __LINE__);
    Rval = dsp_osal_cli2virt(pEncBufStatus->recon.buffer_addr_uv, &pEncStrmInfo->BufInfo[pEncStrmInfo->InfoWCnt].InputUVAddr);
    LL_PrintErrLine(Rval, __func__, __LINE__);
    pEncStrmInfo->BufInfo[pEncStrmInfo->InfoWCnt].InputYuvPitch = pEncBufStatus->recon.buffer_pitch;

    pEncStrmInfo->InfoWCnt++;
    if (MAX_ENC_BUF_INFO_NUM <= pEncStrmInfo->InfoWCnt) {
        pEncStrmInfo->InfoWCnt = 0U;
    }

    for (i = 0U; i < 2U/*DSP_MV_BUF_NUM*/; i++) {
        if (pEncBufStatus->enc_stat[i].buffer_addr > 0U) {
            AMBA_DSP_ENC_MV_RDY_s *pMvInfo = NULL;
            ULONG EventBufAddr = 0U;
            UINT16 EventBufIdx = 0U;

            Rval = DSP_RequestEventInfoBuf(EVENT_INFO_POOL_PROT_BUF, &EventBufAddr, &EventBufIdx, LL_EVENT_VIDEO_MV_DATA_RDY);
            LL_PrintErrLine(Rval, __func__, __LINE__);
            if (Rval == OK) {
                dsp_osal_typecast(&pMvInfo, &EventBufAddr);
                Rval = dsp_osal_memset(pMvInfo, 0, sizeof(AMBA_DSP_ENC_MV_RDY_s));
                LL_PrintErrLine(Rval, __func__, __LINE__);

                pMvInfo->StreamId = pEncBufStatus->channel_id;
                pMvInfo->MvBufType = (UINT8)i;
                pMvInfo->CapPts = pEncBufStatus->hw_pts;
                pMvInfo->CapSequence = pEncBufStatus->cap_seq_no;
                Rval = dsp_osal_cli2virt(pEncBufStatus->enc_stat[i].buffer_addr, &pMvInfo->MvBufAddr);
                LL_PrintErrLine(Rval, __func__, __LINE__);
                pMvInfo->MvBufPitch = pEncBufStatus->enc_stat[i].buffer_pitch;
                pMvInfo->MvBufWidth = pEncBufStatus->enc_stat[i].img_width;
                pMvInfo->MvBufHeight = pEncBufStatus->enc_stat[i].img_height;
                Rval = DSP_GiveEvent(LL_EVENT_VIDEO_MV_DATA_RDY, pMvInfo, EventBufIdx);
                if (Rval != OK) {
                    break;
                }
            }
        }
    }

    return Rval;
}

static UINT32 HandlerEncStatus(const enc_status_msg_t *pEncStatus)
{
    UINT32 Rval = OK;
    AMBA_DSP_ENC_STRM_INFO_s *pEncStrmInfo;
    UINT32 StrmId = pEncStatus->stream_id;
    UINT32 H264Cnt, HevcCnt, JpegCnt;
    AMBA_DSP_EVENT_VIDEO_ENC_STATUS_s *pEncStatusInfo = NULL;
    ULONG EventBufAddr = 0U;
    UINT16 EventBufIdx = 0U;

    if (DSP_ShowCmdMsgLog(MSG_ENC_STATUS) != 0U) {
        AmbaLL_Log(AMBALLLOG_TYPE_MSG,"MSG_ENC_STATUS", 0U, 0U, 0U);
        AmbaLL_Log(AMBALLLOG_TYPE_MSG," stream_id                   = %d  ", pEncStatus->stream_id, 0U, 0U);
        AmbaLL_Log(AMBALLLOG_TYPE_MSG," stream_type                 = %d  ", pEncStatus->stream_type, 0U, 0U);                // To-do: uCode nees to define it.
        AmbaLL_Log(AMBALLLOG_TYPE_MSG," enc_status                  = %d  ", pEncStatus->enc_status, 0U, 0U);                 // enc_status_t
        AmbaLL_Log(AMBALLLOG_TYPE_MSG," stat_fifo_wr_idx            = %d  ", pEncStatus->stat_fifo_wr_idx, 0U, 0U);           // stat_fifo_wr_idx
        AmbaLL_Log(AMBALLLOG_TYPE_MSG," total_bits_info_ctr_h264    = %d  ", pEncStatus->total_bits_info_ctr_h264, 0U, 0U);   // Count of H.264 bits into.
        AmbaLL_Log(AMBALLLOG_TYPE_MSG," total_bits_info_ctr_hevc    = %d  ", pEncStatus->total_bits_info_ctr_hevc, 0U, 0U);   // Count of HEVC bits into.
        AmbaLL_Log(AMBALLLOG_TYPE_MSG," total_bits_info_ctr_jpeg    = %d  ", pEncStatus->total_bits_info_ctr_jpeg, 0U, 0U);   // Count of JPEG bits into.
        AmbaLL_Log(AMBALLLOG_TYPE_MSG," start_pts                   = %d  ", pEncStatus->start_pts, 0U, 0U);
        AmbaLL_Log(AMBALLLOG_TYPE_MSG," mvdump_curr_daddr           = 0x%X", pEncStatus->mvdump_curr_daddr, 0U, 0U);
    }

    pEncStrmInfo = &(EncBitsInfo.Stream[StrmId]);
    DspStatus.EncStatus[StrmId] = (UINT8)pEncStatus->enc_status;

    if (pEncStrmInfo->PrevEncodeState != pEncStatus->enc_status) {
        pEncStrmInfo->PrevEncodeState = pEncStatus->enc_status;

        if (pEncStatus->enc_status == DSP_ENC_STATUS_BUSY) {
            Rval = DSP_RequestEventInfoBuf(EVENT_INFO_POOL_PROT_BUF, &EventBufAddr, &EventBufIdx, LL_EVENT_VIDEO_ENC_START);
        } else {
            Rval = DSP_RequestEventInfoBuf(EVENT_INFO_POOL_PROT_BUF, &EventBufAddr, &EventBufIdx, LL_EVENT_VIDEO_ENC_STOP);
        }
        LL_PrintErrLine(Rval, __func__, __LINE__);
        if (Rval == OK) {
            dsp_osal_typecast(&pEncStatusInfo, &EventBufAddr);
            Rval = dsp_osal_memset(pEncStatusInfo, 0, sizeof(AMBA_DSP_EVENT_VIDEO_ENC_STATUS_s));
            LL_PrintErrLine(Rval, __func__, __LINE__);

            pEncStatusInfo->StreamId = pEncStatus->stream_id;
            {
                if (pEncStatus->enc_status == DSP_ENC_STATUS_BUSY) {
                    Rval = DSP_GiveEvent(LL_EVENT_VIDEO_ENC_START, pEncStatusInfo, EventBufIdx);
                } else if (pEncStatus->enc_status == DSP_ENC_STATUS_IDLE) {
                    Rval = DSP_GiveEvent(LL_EVENT_VIDEO_ENC_STOP, pEncStatusInfo, EventBufIdx);
                    //TBD, move this part to Preproc-function after EncBitsInfo being moved to ContextUtility
                    ResetBitsInfo();
                } else {
                    /* EventInfoAddr need to be released
                     * If you request EventInfoAddr but don't call GiveEvent */
                    Rval = DSP_ReleaseEventInfoBuf(EventBufIdx);
                    LL_PrintErrLine(Rval, __func__, __LINE__);
                    Rval = DSP_ERR_0005;
                    AmbaLL_LogUInt5("[VdoEncEvt] enc status check err %d", pEncStatus->enc_status, 0U, 0U, 0U, 0U);
                }
            }
        }
    }

    /* Handle AvcMv */
    UpdateAvcMvBuf(pEncStatus);

    CalcVdoDataNum(pEncStatus,
                   pEncStrmInfo,
                   &H264Cnt,
                   &HevcCnt,
                   &JpegCnt);

    ProcVdoDataNum(pEncStrmInfo,
                   H264Cnt,
                   HevcCnt,
                   JpegCnt);

    if (pEncStrmInfo->PrevEncodeState != pEncStatus->enc_status) {
        pEncStrmInfo->PrevEncodeState = pEncStatus->enc_status;
        if (pEncStrmInfo->PrevEncodeState == DSP_ENC_STATUS_IDLE) {
            /* TBD reset? */
            pEncStrmInfo->InfoRCnt = 0U;
            pEncStrmInfo->InfoWCnt = 0U;
        }
    }

    Rval |= DSP_SetEventFlagExclusive(DSP_EVENT_CAT_GEN,
                                      DSP_EVENT_TYPE_ENC,
                                      DSP_Enc_StatePattern[pEncStatus->enc_status]);
    return Rval;
}

/**
 * Handle decoder status information
 * @param [in] pDecStatus Pointer to the DSP message about decoding status
 * @return 0 - OK, others - ErrorCode
 */
// FIXME, need to check behavior
static UINT32 HandlerDecStatus(const msg_decoder_status_t *pDecStatus)
{
    static const UINT8 LL_DecCodec2DspDecCodec[AMBA_DSP_DEC_BITS_FORMAT_NUM] = {
        [AMBA_DSP_DEC_BITS_FORMAT_JPEG] = 2U,
        [AMBA_DSP_DEC_BITS_FORMAT_H264] = 0U,
        [AMBA_DSP_DEC_BITS_FORMAT_H265] = 1U,
    };
    static ULONG PreDecNextAddr = 0U;
    static UINT32 PreDispNum = 0U;
    static UINT32 PreDecStatus = 0U;
    static UINT32 PreDecErrStatus = 0U;

    UINT32 Rval = OK;
    AMBA_LL_DEC_UPDATE_STATUS_s DecUpdateStatus = {0};
    AMBA_LL_DEC_GET_BITSTRM_FORMAT_s DecGetBitFormat = {0};
    AMBA_DSP_EVENT_VIDEO_DEC_STATUS_UPDATE_s *pDecStatusInfo = NULL;
    AMBA_DSP_EVENT_VIDEO_DEC_STATUS_UPDATE_s *pDecEosInfo = NULL;
    ULONG EventBufAddr = 0U;
    UINT16 EventBufIdx = 0U;

    if (pDecStatus == NULL) {
        Rval = DSP_ERR_0000;
    } else {
        UINT16 DecodeId;
        UINT16 CodecId;
        ULONG BitsFifoNextPrtVirtAddr;

        if (DSP_ShowCmdMsgLog(MSG_DECODER_STATUS) != 0U) {
            AmbaLL_Log(AMBALLLOG_TYPE_MSG,"MSG_DECODER_STATUS", 0U, 0U, 0U);
            AmbaLL_Log(AMBALLLOG_TYPE_MSG," time_code                  = %d  ", pDecStatus->hdr.time_code, 0U, 0U);
            AmbaLL_Log(AMBALLLOG_TYPE_MSG," latest_clock_counter       = %d  ", pDecStatus->hdr.latest_clock_counter, 0U, 0U);
            AmbaLL_Log(AMBALLLOG_TYPE_MSG," decoder_id                 = %d  ", pDecStatus->hdr.decoder_id, 0U, 0U);
            AmbaLL_Log(AMBALLLOG_TYPE_MSG," codec_type                 = %d  ", pDecStatus->hdr.codec_type, 0U, 0U);
            AmbaLL_Log(AMBALLLOG_TYPE_MSG," decode_state               = %d  ", pDecStatus->decode_state, 0U, 0U);
            AmbaLL_Log(AMBALLLOG_TYPE_MSG," bits_fifo_next_rptr        = 0x%X", pDecStatus->bits_fifo_next_rptr, 0U, 0U);
            AmbaLL_Log(AMBALLLOG_TYPE_MSG," decoded_pic_number         = %d  ", pDecStatus->decoded_pic_number, 0U, 0U);
            AmbaLL_Log(AMBALLLOG_TYPE_MSG," reconed_pic_number         = %d  ", pDecStatus->reconed_pic_number, 0U, 0U);
            AmbaLL_Log(AMBALLLOG_TYPE_MSG," error_status               = %d  ", pDecStatus->error_status, 0U, 0U);
            AmbaLL_Log(AMBALLLOG_TYPE_MSG," frm_pitch                  = %d  ", pDecStatus->frm_pitch, 0U, 0U);
            AmbaLL_Log(AMBALLLOG_TYPE_MSG," frm_width                  = %d  ", pDecStatus->frm_width, 0U, 0U);
            AmbaLL_Log(AMBALLLOG_TYPE_MSG," frm_height                 = %d  ", pDecStatus->frm_height, 0U, 0U);
            AmbaLL_Log(AMBALLLOG_TYPE_MSG," chroma_format_idc          = %d  ", pDecStatus->chroma_format_idc, 0U, 0U);
            AmbaLL_Log(AMBALLLOG_TYPE_MSG," is_eostrm                  = %d  ", pDecStatus->is_eostrm, 0U, 0U);
            AmbaLL_Log(AMBALLLOG_TYPE_MSG," batch_cmd_id               = %d  ", pDecStatus->batch_cmd_id, 0U, 0U);
        }

        DecodeId = pDecStatus->hdr.decoder_id;
        CodecId = pDecStatus->hdr.codec_type;
        if (DecodeId < AMBA_DSP_MAX_DEC_STREAM_NUM) {
            DecGetBitFormat.DecoderId = DecodeId;
            Rval = DSP_GiveEvent(LL_EVENT_DEC_GET_BITSTRM_FORMAT, &DecGetBitFormat, 0U);
            if ((Rval == OK) && (CodecId == LL_DecCodec2DspDecCodec[DecGetBitFormat.BitsFormat])) {
                if (pDecStatus->bits_fifo_next_rptr != 0xFFFFFFFFU) {
                    Rval = dsp_osal_cli2virt(pDecStatus->bits_fifo_next_rptr, &BitsFifoNextPrtVirtAddr);
                    LL_PrintErrLine(Rval, __func__, __LINE__);
                } else {
                    BitsFifoNextPrtVirtAddr = pDecStatus->bits_fifo_next_rptr;
                }

                if ((BitsFifoNextPrtVirtAddr != 0xFFFFFFFFU) &&
                    ((PreDecStatus != pDecStatus->decode_state) ||
                     (PreDecNextAddr != BitsFifoNextPrtVirtAddr) ||
                     (PreDispNum != pDecStatus->reconed_pic_number) ||
                     (PreDecErrStatus != pDecStatus->error_status))) {
                    /* Only update bs status when video decode not idle,
                     * To prevent still dec avc/hevc bs status mixed report with vid */
                    if (PreDecStatus != DEC_OPM_IDLE) {
                        Rval = DSP_RequestEventInfoBuf(EVENT_INFO_POOL_PROT_BUF, &EventBufAddr, &EventBufIdx, LL_EVENT_VIDEO_DEC_STATUS);
                        LL_PrintErrLine(Rval, __func__, __LINE__);
                        if (Rval == OK) {
                            dsp_osal_typecast(&pDecStatusInfo, &EventBufAddr);
                            Rval = dsp_osal_memset(pDecStatusInfo, 0, sizeof(AMBA_DSP_EVENT_VIDEO_DEC_STATUS_UPDATE_s));
                            LL_PrintErrLine(Rval, __func__, __LINE__);

                            pDecStatusInfo->DecState = (UINT8)pDecStatus->decode_state;
                            pDecStatusInfo->ErrState = (UINT8)DSP_GetBit(pDecStatus->error_status, 16U, 8U);   //Get err_level in dec_err_code_t
                            pDecStatusInfo->DecErrState = pDecStatus->error_status;
                            pDecStatusInfo->DecoderId = (UINT32)pDecStatus->hdr.decoder_id;
                            pDecStatusInfo->BitsNextReadAddr  = BitsFifoNextPrtVirtAddr;
                            pDecStatusInfo->NumOfDecodedPic   = pDecStatus->decoded_pic_number;
                            pDecStatusInfo->NumOfDisplayPic   = pDecStatus->reconed_pic_number;
                            pDecStatusInfo->DisplayFramePTS   = DspMsgDisplayPTS;
                            Rval = DSP_GiveEvent(LL_EVENT_VIDEO_DEC_STATUS, pDecStatusInfo, EventBufIdx);
                        }
                    }

                    PreDecStatus = pDecStatus->decode_state;
                    PreDecNextAddr = BitsFifoNextPrtVirtAddr;
                    PreDispNum = pDecStatus->reconed_pic_number;
                    PreDecErrStatus = pDecStatus->error_status;
                }

                if ((DSP_DEC_OPM_IDLE == pDecStatus->decode_state) ||
                    (DSP_DEC_OPM_VDEC_IDLE == pDecStatus->decode_state)) {
                    PpEosEent = 0U;
                }

                if (Rval != OK) {
                    AmbaLL_LogUInt5("Error decode event", 0U, 0U, 0U, 0U, 0U);
                }
                DecUpdateStatus.DecoderId = DecodeId;
                DecUpdateStatus.Status = pDecStatus->decode_state;
                Rval = DSP_GiveEvent(LL_EVENT_DEC_UPDATE_STATUS, &DecUpdateStatus, 0U);
                DspStatus.DecStatus[0U/*ChId*/] = (UINT8)pDecStatus->decode_state;
                Rval |= DSP_SetEventFlagExclusive(DSP_EVENT_CAT_GEN,
                                                  DSP_EVENT_TYPE_DEC,
                                                  DSP_Dec_StatePattern[pDecStatus->decode_state]);
            }

            if ((1U == UseDecEos) && (0U != pDecStatus->is_eostrm)) {
                Rval = DSP_RequestEventInfoBuf(EVENT_INFO_POOL_PROT_BUF, &EventBufAddr, &EventBufIdx, LL_EVENT_VIDEO_DEC_STATUS);
                LL_PrintErrLine(Rval, __func__, __LINE__);
                if (Rval == OK) {
                    dsp_osal_typecast(&pDecEosInfo, &EventBufAddr);
                    Rval = dsp_osal_memset(pDecEosInfo, 0, sizeof(AMBA_DSP_EVENT_VIDEO_DEC_STATUS_UPDATE_s));
                    LL_PrintErrLine(Rval, __func__, __LINE__);

                    pDecEosInfo->EndOfStream = pDecStatus->is_eostrm;
                    Rval = DSP_GiveEvent(LL_EVENT_VIDEO_DEC_STATUS, pDecEosInfo, EventBufIdx);
                    //AmbaLL_LogUInt5("MSG_DECODER_STATUS send EOS", 0U, 0U, 0U, 0U, 0U);
                }
            }
        }
    }
    return Rval;
}

/**
 * Handle decoder still decode information
 * @param [in] pDecStatus Pointer to the DSP message about still decoding status
 * @return 0 - OK, others - ErrorCode
 */
static UINT32 HandlerDecStilldec(const msg_decoder_stilldec_t *pDecStilldec)
{
    UINT32 Rval = OK;
    AMBA_DSP_STLDEC_STATUS_s *pDecStatus = NULL;
    ULONG EventBufAddr = 0U;
    UINT16 EventBufIdx = 0U;

    if (pDecStilldec == NULL) {
        Rval = DSP_ERR_0000;
    } else {
        if (DSP_ShowCmdMsgLog(MSG_DECODER_STILLDEC) != 0U) {
            AmbaLL_Log(AMBALLLOG_TYPE_MSG,"MSG_DECODER_STILLDEC", 0U, 0U, 0U);
            AmbaLL_Log(AMBALLLOG_TYPE_MSG," time_code                  = %d  ", pDecStilldec->hdr.time_code, 0U, 0U);
            AmbaLL_Log(AMBALLLOG_TYPE_MSG," latest_clock_counter       = %d  ", pDecStilldec->hdr.latest_clock_counter, 0U, 0U);
            AmbaLL_Log(AMBALLLOG_TYPE_MSG," decoder_id                 = %d  ", pDecStilldec->hdr.decoder_id, 0U, 0U);
            AmbaLL_Log(AMBALLLOG_TYPE_MSG," codec_type                 = %d  ", pDecStilldec->hdr.codec_type, 0U, 0U);
            AmbaLL_Log(AMBALLLOG_TYPE_MSG," yuv_pic.valid              = %d  ", pDecStilldec->yuv_pic.valid, 0U, 0U);
            AmbaLL_Log(AMBALLLOG_TYPE_MSG," yuv_pic.ch_fmt             = %d  ", pDecStilldec->yuv_pic.ch_fmt, 0U, 0U);
            AmbaLL_Log(AMBALLLOG_TYPE_MSG," yuv_pic.buf_pitch          = %d  ", pDecStilldec->yuv_pic.buf_pitch, 0U, 0U);
            AmbaLL_Log(AMBALLLOG_TYPE_MSG," yuv_pic.buf_width          = %d  ", pDecStilldec->yuv_pic.buf_width, 0U, 0U);
            AmbaLL_Log(AMBALLLOG_TYPE_MSG," yuv_pic.buf_height         = %d  ", pDecStilldec->yuv_pic.buf_height, 0U, 0U);
            AmbaLL_Log(AMBALLLOG_TYPE_MSG," yuv_pic.lu_buf_base        = 0x%X", pDecStilldec->yuv_pic.lu_buf_base, 0U, 0U);
            AmbaLL_Log(AMBALLLOG_TYPE_MSG," yuv_pic.ch_buf_base        = 0x%X", pDecStilldec->yuv_pic.ch_buf_base, 0U, 0U);
            AmbaLL_Log(AMBALLLOG_TYPE_MSG," yuv_pic.img_width          = %d  ", pDecStilldec->yuv_pic.img_width, 0U, 0U);
            AmbaLL_Log(AMBALLLOG_TYPE_MSG," yuv_pic.img_height         = %d  ", pDecStilldec->yuv_pic.img_height, 0U, 0U);
            AmbaLL_Log(AMBALLLOG_TYPE_MSG," yuv_pic.img_offset_x       = %d  ", pDecStilldec->yuv_pic.img_offset_x, 0U, 0U);
            AmbaLL_Log(AMBALLLOG_TYPE_MSG," yuv_pic.img_offset_y       = %d  ", pDecStilldec->yuv_pic.img_offset_y, 0U, 0U);
        }

        Rval = DSP_RequestEventInfoBuf(EVENT_INFO_POOL_PROT_BUF, &EventBufAddr, &EventBufIdx, LL_EVENT_STILL_DEC_STATUS);
        LL_PrintErrLine(Rval, __func__, __LINE__);
        if (Rval == OK) {
            dsp_osal_typecast(&pDecStatus, &EventBufAddr);
            Rval = dsp_osal_memset(pDecStatus, 0, sizeof(AMBA_DSP_STLDEC_STATUS_s));
            LL_PrintErrLine(Rval, __func__, __LINE__);

            pDecStatus->Index = pDecStilldec->hdr.decoder_id;
            pDecStatus->Operation = AMBA_DSP_STLDEC_OP_DEC;
            pDecStatus->Status = (pDecStilldec->yuv_pic.valid == (UINT8)0U)? (UINT32)1U: (UINT32)0U;
            if (pDecStilldec->yuv_pic.ch_fmt == DSP_YUV_420) {
                pDecStatus->DataFmt = AMBA_DSP_YUV420;
            } else {
                pDecStatus->DataFmt = AMBA_DSP_YUV422;
            }

            Rval = dsp_osal_cli2virt(pDecStilldec->yuv_pic.lu_buf_base, &pDecStatus->YAddr);
            LL_PrintErrLine(Rval, __func__, __LINE__);
            Rval = dsp_osal_cli2virt(pDecStilldec->yuv_pic.ch_buf_base, &pDecStatus->UVAddr);
            LL_PrintErrLine(Rval, __func__, __LINE__);
            pDecStatus->Width = pDecStilldec->yuv_pic.img_width;
            pDecStatus->Height = pDecStilldec->yuv_pic.img_height;
            pDecStatus->Pitch = pDecStilldec->yuv_pic.buf_pitch;
            Rval = DSP_GiveEvent(LL_EVENT_STILL_DEC_STATUS, pDecStatus, EventBufIdx);
        }
    }
    return Rval;
}

static UINT32 HandlerDecPicInfo(const msg_decoder_picinfo_t *pDecPicInfo)
{
    static const UINT8 DecPicYuvFmtTable[3U] = {
            [YUV_MONO] = AMBA_DSP_YUV400,         //#define YUV_MONO 0u
            [YUV_420 ] = AMBA_DSP_YUV420,         //#define YUV_420  1u
            [YUV_422 ] = AMBA_DSP_YUV422,         //#define YUV_422  2u
                                                  //#define YUV_444  3u
    };
    UINT32 Rval = OK;
    AMBA_LL_EVENT_VIDEO_DEC_PIC_INFO_s *pDecPicEvnt = NULL;
    ULONG EventBufAddr = 0U;
    UINT16 EventBufIdx = 0U;

    if (pDecPicInfo == NULL) {
        Rval = DSP_ERR_0000;
    } else {
        if (DSP_ShowCmdMsgLog(MSG_DECODER_PICINFO) != 0U) {
            AmbaLL_Log(AMBALLLOG_TYPE_MSG,"MSG_DECODER_PICINFO", 0U, 0U, 0U);
            AmbaLL_Log(AMBALLLOG_TYPE_MSG," hdr.time_code               = %d  ", pDecPicInfo->hdr.time_code, 0U, 0U);
            AmbaLL_Log(AMBALLLOG_TYPE_MSG," hdr.latest_clock_counter    = %d  ", pDecPicInfo->hdr.latest_clock_counter, 0U, 0U);
            AmbaLL_Log(AMBALLLOG_TYPE_MSG," hdr.decoder_id              = %d  ", pDecPicInfo->hdr.decoder_id, 0U, 0U);
            AmbaLL_Log(AMBALLLOG_TYPE_MSG," hdr.codec_type              = %d  ", pDecPicInfo->hdr.codec_type, 0U, 0U);
            AmbaLL_Log(AMBALLLOG_TYPE_MSG," pic_no                      = %d  ", pDecPicInfo->pic_no, 0U, 0U);
            AmbaLL_Log(AMBALLLOG_TYPE_MSG," pts_low                     = %d  ", pDecPicInfo->pts_low, 0U, 0U);
            AmbaLL_Log(AMBALLLOG_TYPE_MSG," pts_high                    = %d  ", pDecPicInfo->pts_high, 0U, 0U);
            AmbaLL_Log(AMBALLLOG_TYPE_MSG," yuv_pic.valid               = %d  ", pDecPicInfo->yuv_pic.valid       , 0U, 0U);
            AmbaLL_Log(AMBALLLOG_TYPE_MSG," yuv_pic.ch_fmt              = %d  ", pDecPicInfo->yuv_pic.ch_fmt      , 0U, 0U);
            AmbaLL_Log(AMBALLLOG_TYPE_MSG," yuv_pic.buf_pitch           = %d  ", pDecPicInfo->yuv_pic.buf_pitch   , 0U, 0U);
            AmbaLL_Log(AMBALLLOG_TYPE_MSG," yuv_pic.buf_width           = %d  ", pDecPicInfo->yuv_pic.buf_width   , 0U, 0U);
            AmbaLL_Log(AMBALLLOG_TYPE_MSG," yuv_pic.buf_height          = %d  ", pDecPicInfo->yuv_pic.buf_height  , 0U, 0U);
            AmbaLL_Log(AMBALLLOG_TYPE_MSG," yuv_pic.lu_buf_base         = 0x%X  ", pDecPicInfo->yuv_pic.lu_buf_base , 0U, 0U);
            AmbaLL_Log(AMBALLLOG_TYPE_MSG," yuv_pic.ch_buf_base         = 0x%X  ", pDecPicInfo->yuv_pic.ch_buf_base , 0U, 0U);
            AmbaLL_Log(AMBALLLOG_TYPE_MSG," yuv_pic.img_width           = %d  ", pDecPicInfo->yuv_pic.img_width   , 0U, 0U);
            AmbaLL_Log(AMBALLLOG_TYPE_MSG," yuv_pic.img_height          = %d  ", pDecPicInfo->yuv_pic.img_height  , 0U, 0U);
            AmbaLL_Log(AMBALLLOG_TYPE_MSG," yuv_pic.img_offset_x        = %d  ", pDecPicInfo->yuv_pic.img_offset_x, 0U, 0U);
            AmbaLL_Log(AMBALLLOG_TYPE_MSG," yuv_pic.img_offset_y        = %d  ", pDecPicInfo->yuv_pic.img_offset_y, 0U, 0U);
        }

        if (pDecPicInfo->yuv_pic.valid == 1U) {
            Rval = DSP_RequestEventInfoBuf(EVENT_INFO_POOL_PROT_BUF, &EventBufAddr, &EventBufIdx, LL_EVENT_VIDEO_DEC_PIC_INFO);
            LL_PrintErrLine(Rval, __func__, __LINE__);
            if (Rval == OK) {
                dsp_osal_typecast(&pDecPicEvnt, &EventBufAddr);
                Rval = dsp_osal_memset(pDecPicEvnt, 0, sizeof(AMBA_LL_EVENT_VIDEO_DEC_PIC_INFO_s));
                LL_PrintErrLine(Rval, __func__, __LINE__);

                pDecPicEvnt->PicInfo.Num = pDecPicInfo->pic_no;
                pDecPicEvnt->PicInfo.DataFmt = DecPicYuvFmtTable[pDecPicInfo->yuv_pic.ch_fmt];
                Rval = dsp_osal_cli2virt(pDecPicInfo->yuv_pic.lu_buf_base, &pDecPicEvnt->PicInfo.YAddr);
                LL_PrintErrLine(Rval, __func__, __LINE__);
                Rval = dsp_osal_cli2virt(pDecPicInfo->yuv_pic.ch_buf_base, &pDecPicEvnt->PicInfo.UVAddr);
                LL_PrintErrLine(Rval, __func__, __LINE__);

                pDecPicEvnt->PicInfo.Width = (UINT32)pDecPicInfo->yuv_pic.img_width;
                pDecPicEvnt->PicInfo.Height = (UINT32)pDecPicInfo->yuv_pic.img_height;
                pDecPicEvnt->PicInfo.Pitch = (UINT32)pDecPicInfo->yuv_pic.buf_pitch;
                pDecPicEvnt->PicInfo.DecoderId = (UINT32)pDecPicInfo->hdr.decoder_id;
                pDecPicEvnt->BufferWidth = (UINT32)pDecPicInfo->yuv_pic.buf_width;
                pDecPicEvnt->BufferHeight = (UINT32)pDecPicInfo->yuv_pic.buf_height;
                pDecPicEvnt->PicInfo.DecPts = ((UINT64)pDecPicInfo->pts_high<<32U) | ((UINT64)pDecPicInfo->pts_low);
    //            DecPicInfo.PicInfo.DecDonePts = pDecPicInfo->hdr.latest_clock_counter;
                Rval = DSP_GiveEvent(LL_EVENT_VIDEO_DEC_PIC_INFO, pDecPicEvnt, EventBufIdx);
            }
        }
    }
    return Rval;
}

/**
 * Handle decproc status message information
 * @param [in] pDecProcStatus Pointer to the DSP message about decproc status
 * @return 0 - OK, others - ErrorCode
 */
// FIXME, need to check behavior
static UINT32 HandlerDecProcStatus(const msg_dproc_status_t *pDecProcStatus)
{
    UINT32 Rval = OK;

    if (pDecProcStatus == NULL) {
        Rval = DSP_ERR_0000;
    } else {
        if (DSP_ShowCmdMsgLog(MSG_DECPROC_STATUS) != 0U) {
            AmbaLL_Log(AMBALLLOG_TYPE_MSG,"MSG_DECPROC_STATUS", 0U, 0U, 0U);
            AmbaLL_Log(AMBALLLOG_TYPE_MSG," timecode                  = %d  ", pDecProcStatus->timecode, 0U, 0U);
            AmbaLL_Log(AMBALLLOG_TYPE_MSG," latest_clock_counter      = %d  ", pDecProcStatus->latest_clock_counter, 0U, 0U);
        }
    }
    return Rval;
}

/**
 * Handle decproc Y2Y status message information
 * @param [in] pDecProcY2Y Pointer to the DSP message about decproc Y2Y status
 * @return 0 - OK, others - ErrorCode
 */
static UINT32 HandlerDecProcY2y(const msg_dproc_yuv_to_yuv_t *pDecProcY2Y)
{
    UINT32 Rval = OK;
    AMBA_DSP_STLDEC_STATUS_s *pDecStatus = NULL;
    UINT8 i;
    ULONG EventBufAddr = 0U;
    UINT16 EventBufIdx = 0U;

    if (pDecProcY2Y == NULL) {
        Rval = DSP_ERR_0000;
    } else {
        if (DSP_ShowCmdMsgLog(MSG_DECPROC_YUV_TO_YUV) != 0U) {
            AmbaLL_Log(AMBALLLOG_TYPE_MSG,"MSG_DECPROC_STATUS", 0U, 0U, 0U);
            AmbaLL_Log(AMBALLLOG_TYPE_MSG," timecode                  = %d  ", pDecProcY2Y->timecode, 0U, 0U);
            AmbaLL_Log(AMBALLLOG_TYPE_MSG," latest_clock_counter      = %d  ", pDecProcY2Y->latest_clock_counter, 0U, 0U);
            for (i = 0U; i < DPROC_MAX_YUV_OUT_NUM; i++) {
                AmbaLL_Log(AMBALLLOG_TYPE_MSG,"========= yuv_pic %02d ========", i, 0U, 0U);
                AmbaLL_Log(AMBALLLOG_TYPE_MSG,"   valid               = %d  ", pDecProcY2Y->yuv_pic[i].valid, 0U, 0U);
                AmbaLL_Log(AMBALLLOG_TYPE_MSG,"   ch_fmt              = %d  ", pDecProcY2Y->yuv_pic[i].ch_fmt, 0U, 0U);
                AmbaLL_Log(AMBALLLOG_TYPE_MSG,"   buf_pitch           = %d  ", pDecProcY2Y->yuv_pic[i].buf_pitch, 0U, 0U);
                AmbaLL_Log(AMBALLLOG_TYPE_MSG,"   buf_width           = %d  ", pDecProcY2Y->yuv_pic[i].buf_width, 0U, 0U);
                AmbaLL_Log(AMBALLLOG_TYPE_MSG,"   buf_height          = %d  ", pDecProcY2Y->yuv_pic[i].buf_height, 0U, 0U);
                AmbaLL_Log(AMBALLLOG_TYPE_MSG,"   lu_buf_base         = 0x%X", pDecProcY2Y->yuv_pic[i].lu_buf_base, 0U, 0U);
                AmbaLL_Log(AMBALLLOG_TYPE_MSG,"   ch_buf_base         = 0x%X", pDecProcY2Y->yuv_pic[i].ch_buf_base, 0U, 0U);
                AmbaLL_Log(AMBALLLOG_TYPE_MSG,"   img_width           = %d  ", pDecProcY2Y->yuv_pic[i].img_width, 0U, 0U);
                AmbaLL_Log(AMBALLLOG_TYPE_MSG,"   img_height          = %d  ", pDecProcY2Y->yuv_pic[i].img_height, 0U, 0U);
                AmbaLL_Log(AMBALLLOG_TYPE_MSG,"   img_offset_x        = %d  ", pDecProcY2Y->yuv_pic[i].img_offset_x, 0U, 0U);
                AmbaLL_Log(AMBALLLOG_TYPE_MSG,"   img_offset_y        = %d  ", pDecProcY2Y->yuv_pic[i].img_offset_y, 0U, 0U);
            }
        }

        Rval = DSP_RequestEventInfoBuf(EVENT_INFO_POOL_PROT_BUF, &EventBufAddr, &EventBufIdx, LL_EVENT_STILL_DEC_STATUS);
        LL_PrintErrLine(Rval, __func__, __LINE__);
        if (Rval == OK) {
            dsp_osal_typecast(&pDecStatus, &EventBufAddr);
            Rval = dsp_osal_memset(pDecStatus, 0, sizeof(AMBA_DSP_STLDEC_STATUS_s));
            LL_PrintErrLine(Rval, __func__, __LINE__);

            pDecStatus->Index = 0U; //DONT CARE
            pDecStatus->Operation = AMBA_DSP_STLDEC_OP_Y2Y;
            pDecStatus->Status = (pDecProcY2Y->yuv_pic[0U].valid == (UINT8)0U)? (UINT32)1U: (UINT32)0U;
            if (pDecProcY2Y->yuv_pic[0U].ch_fmt == DSP_YUV_420) {
                pDecStatus->DataFmt = AMBA_DSP_YUV420;
            } else {
                pDecStatus->DataFmt = AMBA_DSP_YUV422;
            }
            Rval = dsp_osal_cli2virt(pDecProcY2Y->yuv_pic[0U].lu_buf_base, &pDecStatus->YAddr);
            LL_PrintErrLine(Rval, __func__, __LINE__);
            Rval = dsp_osal_cli2virt(pDecProcY2Y->yuv_pic[0U].ch_buf_base, &pDecStatus->UVAddr);
            LL_PrintErrLine(Rval, __func__, __LINE__);
            pDecStatus->Width = pDecProcY2Y->yuv_pic[0U].img_width;
            pDecStatus->Height = pDecProcY2Y->yuv_pic[0U].img_height;
            pDecStatus->Pitch = pDecProcY2Y->yuv_pic[0U].buf_pitch;
            Rval = DSP_GiveEvent(LL_EVENT_STILL_DEC_STATUS, pDecStatus, EventBufIdx);
        }
    }
    return Rval;
}

/**
 * Handle decproc yuv paint message information
 * @param [in] pDecProcYuvPaint Pointer to the DSP message about decproc yuv paint
 * @return 0 - OK, others - ErrorCode
 */
// FIXME, need to check behavior
static UINT32 HandlerDecProcYuvPaint(const msg_dproc_yuv_paint_t *pDecProcYuvPaint)
{
    UINT32 Rval = OK;
    // TODO
    if (pDecProcYuvPaint == NULL) {
        Rval = DSP_ERR_0000;
    } else {
        if (DSP_ShowCmdMsgLog(MSG_DECPROC_YUV_PAINT) != 0U) {
            AmbaLL_Log(AMBALLLOG_TYPE_MSG,"MSG_DECPROC_YUV_PAINT", 0U, 0U, 0U);
            AmbaLL_Log(AMBALLLOG_TYPE_MSG," timecode                  = %d  ", pDecProcYuvPaint->timecode, 0U, 0U);
            AmbaLL_Log(AMBALLLOG_TYPE_MSG," latest_clock_counter      = %d  ", pDecProcYuvPaint->latest_clock_counter, 0U, 0U);

            AmbaLL_Log(AMBALLLOG_TYPE_MSG,"========= yuv_pic info ========", 0U, 0U, 0U);
            AmbaLL_Log(AMBALLLOG_TYPE_MSG,"   valid               = %d  ", pDecProcYuvPaint->yuv_pic.valid, 0U, 0U);
            AmbaLL_Log(AMBALLLOG_TYPE_MSG,"   ch_fmt              = %d  ", pDecProcYuvPaint->yuv_pic.ch_fmt, 0U, 0U);
            AmbaLL_Log(AMBALLLOG_TYPE_MSG,"   buf_pitch           = %d  ", pDecProcYuvPaint->yuv_pic.buf_pitch, 0U, 0U);
            AmbaLL_Log(AMBALLLOG_TYPE_MSG,"   buf_width           = %d  ", pDecProcYuvPaint->yuv_pic.buf_width, 0U, 0U);
            AmbaLL_Log(AMBALLLOG_TYPE_MSG,"   buf_height          = %d  ", pDecProcYuvPaint->yuv_pic.buf_height, 0U, 0U);
            AmbaLL_Log(AMBALLLOG_TYPE_MSG,"   lu_buf_base         = 0x%X", pDecProcYuvPaint->yuv_pic.lu_buf_base, 0U, 0U);
            AmbaLL_Log(AMBALLLOG_TYPE_MSG,"   ch_buf_base         = 0x%X", pDecProcYuvPaint->yuv_pic.ch_buf_base, 0U, 0U);
            AmbaLL_Log(AMBALLLOG_TYPE_MSG,"   img_width           = %d  ", pDecProcYuvPaint->yuv_pic.img_width, 0U, 0U);
            AmbaLL_Log(AMBALLLOG_TYPE_MSG,"   img_height          = %d  ", pDecProcYuvPaint->yuv_pic.img_height, 0U, 0U);
            AmbaLL_Log(AMBALLLOG_TYPE_MSG,"   img_offset_x        = %d  ", pDecProcYuvPaint->yuv_pic.img_offset_x, 0U, 0U);
            AmbaLL_Log(AMBALLLOG_TYPE_MSG,"   img_offset_y        = %d  ", pDecProcYuvPaint->yuv_pic.img_offset_y, 0U, 0U);
        }
    }
    return Rval;
}

/**
 * Handle post process message information
 * @param [in] pPostpStatus Pointer to the DSP message about post process
 * @return 0 - OK, others - ErrorCode
 */
static UINT32 HandlerPostProcStatus(const msg_postp_status_t *pPostpStatus)
{
    static const UINT8 PostProcSt[3U] = {
            [YUV_MONO] = AMBA_DSP_YUV400,         //#define YUV_MONO 0u
            [YUV_420 ] = AMBA_DSP_YUV420,         //#define YUV_420  1u
            [YUV_422 ] = AMBA_DSP_YUV422,         //#define YUV_422  2u
                                                  //#define YUV_444  3u
    };
    UINT32 Rval = OK;
    AMBA_DSP_EVENT_VIDEO_DEC_STATUS_UPDATE_s *pDecEosInfo = NULL;
    AMBA_DSP_VIDEO_DEC_YUV_DISP_REPORT_s *pVdoPostpDispInfo = NULL, *pStlPostpDispInfo = NULL;
    ULONG EventBufAddr = 0U, EventBufStlAddr = 0U;
    UINT16 EventBufIdx = 0U, EventBufStlIdx = 0U;

    if (pPostpStatus == NULL) {
        Rval = DSP_ERR_0000;
    } else {
        if (DSP_ShowCmdMsgLog(MSG_POSTPROC_STATUS) != 0U) {
            AmbaLL_Log(AMBALLLOG_TYPE_MSG,"MSG_POSTPROC_STATUS", 0U, 0U, 0U);
            AmbaLL_Log(AMBALLLOG_TYPE_MSG," timecode                  = %d  ", pPostpStatus->timecode, 0U, 0U);
            AmbaLL_Log(AMBALLLOG_TYPE_MSG," latest_clock_counter      = %d  ", pPostpStatus->latest_clock_counter, 0U, 0U);
            AmbaLL_Log(AMBALLLOG_TYPE_MSG," ppvout_id                 = %d  ", pPostpStatus->ppvout_id, 0U, 0U);
            AmbaLL_Log(AMBALLLOG_TYPE_MSG," ppvout_en                 = %d  ", pPostpStatus->ppvout_en, 0U, 0U);
            AmbaLL_Log(AMBALLLOG_TYPE_MSG," ppvout_exist              = %d  ", pPostpStatus->ppvout_exist, 0U, 0U);
            AmbaLL_Log(AMBALLLOG_TYPE_MSG," is_end_of_strm            = %d  ", pPostpStatus->is_end_of_strm, 0U, 0U);
            AmbaLL_Log(AMBALLLOG_TYPE_MSG," ppvout_state              = %d  ", pPostpStatus->ppvout_state, 0U, 0U);
            AmbaLL_Log(AMBALLLOG_TYPE_MSG," last_pts_high             = 0x%X", pPostpStatus->last_pts_high, 0U, 0U);
            AmbaLL_Log(AMBALLLOG_TYPE_MSG," last_pts_low              = 0x%X", pPostpStatus->last_pts_low, 0U, 0U);
            AmbaLL_Log(AMBALLLOG_TYPE_MSG," last_lu_base              = 0x%X", pPostpStatus->last_lu_base, 0U, 0U);
            AmbaLL_Log(AMBALLLOG_TYPE_MSG," last_ch_base              = 0x%X", pPostpStatus->last_ch_base, 0U, 0U);
            AmbaLL_Log(AMBALLLOG_TYPE_MSG," ch_fmt                    = %d  ", pPostpStatus->ch_fmt, 0U, 0U);
        }

        if (pPostpStatus->ppvout_en == 1U) {
            if (0xFFFFFFFFU == pPostpStatus->last_pts_low) {
                DspMsgDisplayPTS = 0xFFFFFFFFU;
            } else {
                DspMsgDisplayPTS = ((UINT64) pPostpStatus->last_pts_high << 32) + pPostpStatus->last_pts_low;
            }
        }

        if ((UINT8)DspStatus.PpvoutExist != pPostpStatus->ppvout_exist) {
            DspStatus.PpvoutExist = pPostpStatus->ppvout_exist;
            if (DspStatus.PpvoutExist == DSP_PPVOUT_STATE_EXIST) {
                Rval = DSP_SetEventFlagExclusive(DSP_EVENT_CAT_GEN, DSP_EVENT_TYPE_PPVOUT, AMBA_DSP_POSTP_VOUT_EXIST_FLAG_PATTERN);
            } else {
                Rval = DSP_ClearEventFlag(DSP_EVENT_CAT_GEN, DSP_EVENT_TYPE_PPVOUT, AMBA_DSP_POSTP_VOUT_EXIST_FLAG_PATTERN);
            }
        }

        if ((0U == UseDecEos) && (0U == PpEosEent) && (0U != pPostpStatus->is_end_of_strm)) {
            PpEosEent = pPostpStatus->is_end_of_strm;
//            dsp_osal_printU5( "HandlerPostProcStatus POSTP EOS %d", pPostpStatus->is_end_of_strm, 0U, 0U, 0U, 0U);

            Rval = DSP_RequestEventInfoBuf(EVENT_INFO_POOL_PROT_BUF, &EventBufAddr, &EventBufIdx, LL_EVENT_VIDEO_DEC_STATUS);
            LL_PrintErrLine(Rval, __func__, __LINE__);
            if (Rval == OK) {
                dsp_osal_typecast(&pDecEosInfo, &EventBufAddr);
                Rval = dsp_osal_memset(pDecEosInfo, 0, sizeof(AMBA_DSP_EVENT_VIDEO_DEC_STATUS_UPDATE_s));
                LL_PrintErrLine(Rval, __func__, __LINE__);

                pDecEosInfo->EndOfStream = pPostpStatus->is_end_of_strm;
                Rval = DSP_GiveEvent(LL_EVENT_VIDEO_DEC_STATUS, pDecEosInfo, EventBufIdx);
            }
        }

        if ((pPostpStatus->ppvout_en == 1U) &&
            (pPostpStatus->ppvout_exist == 1U)) {
            Rval = DSP_RequestEventInfoBuf(EVENT_INFO_POOL_PROT_BUF, &EventBufAddr, &EventBufIdx, LL_EVENT_VIDEO_DEC_YUV_DISP_REPORT);
            LL_PrintErrLine(Rval, __func__, __LINE__);
            if (Rval == OK) {
                Rval = DSP_RequestEventInfoBuf(EVENT_INFO_POOL_PROT_BUF, &EventBufStlAddr, &EventBufStlIdx, LL_EVENT_VIDEO_DEC_YUV_DISP_REPORT);
                LL_PrintErrLine(Rval, __func__, __LINE__);
                if (Rval == OK) {
                    dsp_osal_typecast(&pVdoPostpDispInfo, &EventBufAddr);
                    dsp_osal_typecast(&pStlPostpDispInfo, &EventBufStlAddr);
                    Rval = dsp_osal_memset(pVdoPostpDispInfo, 0, sizeof(AMBA_DSP_VIDEO_DEC_YUV_DISP_REPORT_s));
                    LL_PrintErrLine(Rval, __func__, __LINE__);

                    pVdoPostpDispInfo->VoutIdx = pPostpStatus->ppvout_id;
                    pVdoPostpDispInfo->DataFmt = PostProcSt[pPostpStatus->ch_fmt];
                    if (pPostpStatus->last_lu_base == 0xFFFFFFFFU) {
                        Rval = dsp_osal_cli2virt(0U, &pVdoPostpDispInfo->YAddr);
                        LL_PrintErrLine(Rval, __func__, __LINE__);
                    } else {
                        Rval = dsp_osal_cli2virt(pPostpStatus->last_lu_base, &pVdoPostpDispInfo->YAddr);
                        LL_PrintErrLine(Rval, __func__, __LINE__);
                    }
                    if (pPostpStatus->last_ch_base == 0xFFFFFFFFU) {
                        Rval = dsp_osal_cli2virt(0U, &pVdoPostpDispInfo->UVAddr);
                        LL_PrintErrLine(Rval, __func__, __LINE__);
                    } else {
                        Rval = dsp_osal_cli2virt(pPostpStatus->last_ch_base, &pVdoPostpDispInfo->UVAddr);
                        LL_PrintErrLine(Rval, __func__, __LINE__);
                    }
                    Rval = dsp_osal_memcpy(pStlPostpDispInfo, pVdoPostpDispInfo, sizeof(AMBA_DSP_VIDEO_DEC_YUV_DISP_REPORT_s));
                    LL_PrintErrLine(Rval, __func__, __LINE__);
                    Rval = DSP_GiveEvent(LL_EVENT_VIDEO_DEC_YUV_DISP_REPORT, pVdoPostpDispInfo, EventBufIdx);
                    if (Rval != OK) {
                        //
                    } else {
                        //
                    }
                    Rval = DSP_GiveEvent(LL_EVENT_STILL_DEC_YUV_DISP_REPORT, pStlPostpDispInfo, EventBufStlIdx);
                } else {
                    /* EventInfoAddr need to be released
                     * If you request EventInfoAddr but don't call GiveEvent */
                    Rval = DSP_ReleaseEventInfoBuf(EventBufIdx);
                    LL_PrintErrLine(Rval, __func__, __LINE__);
                }
            }
        }
    }
    return Rval;
}

static UINT32 MsgParserDSPStatus(const void *pMsgInfo)
{
    UINT32 Rval = OK;
    const dsp_msg_t *pMsg = NULL;
#ifdef SUPPORT_DSP_SAFETY_STATE
    AMBA_DSP_EVENT_ERROR_INFO_s *pErrInfo = NULL;
    ULONG EventBufAddr = 0U;
    UINT16 EventBufIdx = 0U;
#endif
    dsp_osal_typecast(&pMsg, &pMsgInfo);

    switch (pMsg->msg_code) {
    case MSG_DSP_STATUS:
    {
        const dsp_status_msg_t *pStatusMsg = NULL;
        UINT32 TimeTick;

        dsp_osal_typecast(&pStatusMsg, &pMsgInfo);
        if (DSP_ShowCmdMsgLog(MSG_DSP_STATUS) != 0U) {
            AmbaLL_Log(AMBALLLOG_TYPE_MSG,"MSG_DSP_STATUS", 0U, 0U, 0U);
            AmbaLL_Log(AMBALLLOG_TYPE_MSG," dsp_prof_id     = %d  ", pStatusMsg->dsp_prof_id, 0U, 0U);    // dsp_prof_cfg_id_t
#ifdef SUPPORT_DSP_SAFETY_STATE
            AmbaLL_Log(AMBALLLOG_TYPE_MSG," fatal_err_th_id = %d  ", pStatusMsg->fatal_err_th_id, 0U, 0U);
#endif
            AmbaLL_Log(AMBALLLOG_TYPE_MSG," time_code       = %d  ", pStatusMsg->time_code, 0U, 0U);      // DSP time tag latched from the system when the msg is sent
            AmbaLL_Log(AMBALLLOG_TYPE_MSG," prev_cmd_seq    = %d  ", pStatusMsg->prev_cmd_seq, 0U, 0U);   // The just processed cmd seq num
            AmbaLL_Log(AMBALLLOG_TYPE_MSG," prev_num_cmds   = %d  ", pStatusMsg->prev_num_cmds, 0U, 0U);  // The num of cmd that DSP received for the most recent cmd block
            AmbaLL_Log(AMBALLLOG_TYPE_MSG," dram_used       = %d  ", pStatusMsg->dram_used, 0U, 0U);
        }

        /* TimeTick update */
        TimeTick = pStatusMsg->time_code;
        Rval = DSP_GiveEvent(LL_EVENT_TIME_TICK_UPDATE, &TimeTick, 0U);
        LL_PrintErrLine(Rval, __func__, __LINE__);

        if (DspStatus.ProfCfgID !=  pStatusMsg->dsp_prof_id) {
//FIXME, reset every profile change?
            /* Reset "DspStatus" first and report */
            Rval = dsp_osal_memset(&DspStatus, 0, sizeof(DspStatus));
            LL_PrintErrLine(Rval, __func__, __LINE__);

            if (DSP_REPORTED_PROF_IDLE == pStatusMsg->dsp_prof_id) {
                AmbaVideoBitsDescIdx = 0U;
                EncBitsInfoReset();

            }
            if (DSP_REPORTED_PROF_INVALID ==  DspStatus.ProfCfgID) {
                //reset Some Status when comes from Invalid
                EncBitsInfoReset();
            }
#ifdef SUPPORT_DSP_SAFETY_STATE
            if (DSP_REPORTED_PROF_SAFETY ==  pStatusMsg->dsp_prof_id) {
                Rval = DSP_RequestEventInfoBuf(EVENT_INFO_POOL_PROT_BUF, &EventBufAddr, &EventBufIdx, LL_EVENT_ERROR);
                LL_PrintErrLine(Rval, __func__, __LINE__);
                if (Rval == DSP_ERR_NONE) {
                    dsp_osal_typecast(&pErrInfo, &EventBufAddr);
                    pErrInfo->Type = AMBA_DSP_ERROR_ASSERT;
                    pErrInfo->Info = 0U;
                    pErrInfo->InfoAux = pStatusMsg->fatal_err_th_id;
                    Rval = DSP_GiveEvent(LL_EVENT_ERROR, pErrInfo, EventBufIdx);
                    LL_PrintErrLine(Rval, __func__, __LINE__);
                }
                DspStatus.ProfStatus = DSP_PROF_STATUS_SAFETY;
            }
#endif
            DspStatus.ProfCfgID =  pStatusMsg->dsp_prof_id;
        }

        {
            UINT32 CurrentDspProf = DSP_PROF_STATUS_INVALID;

            if (DspStatus.ProfCfgID <= DSP_REPORTED_PROF_PLAYBACK) {
                DspStatus.ProfStatus = (UINT8) DspStatus.ProfCfgID;
                CurrentDspProf = DspStatus.ProfCfgID;
            } else if (DspStatus.ProfCfgID == DSP_REPORTED_PROF_2_IDLE) {
                DspStatus.ProfStatus = DSP_PROF_STATUS_2_IDLE;
                CurrentDspProf = DSP_PROF_STATUS_2_IDLE;
            } else if (DspStatus.ProfCfgID == DSP_REPORTED_PROF_SAFETY) {
                CurrentDspProf = DSP_PROF_STATUS_SAFETY;
            } else if (DspStatus.ProfCfgID == DSP_REPORTED_PROF_IDLE) {
                if ((DspStatus.ProfStatus != DSP_PROF_STATUS_2_CAMERA) && \
                    (DspStatus.ProfStatus != DSP_PROF_STATUS_2_YUV_INPUT)) {
                    DspStatus.ProfStatus = DSP_PROF_STATUS_IDLE;
                    CurrentDspProf = DSP_PROF_STATUS_IDLE;
                } else {
                    CurrentDspProf = DSP_PROF_STATUS_IDLE;
                }
            } else {
                // DO NOTHING
            }
            Rval = DSP_SetEventFlagExclusive(DSP_EVENT_CAT_GEN, DSP_EVENT_TYPE_PROF, DSP_PROF_StatePattern[CurrentDspProf]);
        }
        break;
    }
    default:
        Rval = DSP_ERR_0005;
        AmbaLL_LogUInt5("[Err @ MsgDispatcher] Unexpected DSP status msg code (0x%x)", pMsg->msg_code, 0U, 0U, 0U, 0U);
        break;
    }
    return Rval;
}

static UINT32 GiveVprocEventOnParseVproc(const msg_vproc_status_t *pVprocStatus)
{
    UINT32 Rval = OK;

    Rval = GiveRawEventOnParseVproc(pVprocStatus);
    LL_PrintErrLine(Rval, __func__, __LINE__);

    if (Rval == OK) {
        Rval = GiveYuvEventOnParseVproc(pVprocStatus);
        LL_PrintErrLine(Rval, __func__, __LINE__);
    }

    if (Rval == OK) {
        UpdateVprocStatusOnParseVproc(pVprocStatus->channel_id, pVprocStatus->status);
        Rval = DSP_SetEventFlagExclusive(DSP_EVENT_CAT_VPROC, (UINT16)pVprocStatus->channel_id,
                                         DSP_VPROC_StatePattern[DspStatus.VprocStatus[pVprocStatus->channel_id]]);
        LL_PrintErrLine(Rval, __func__, __LINE__);
    }
    /* To-do: need to decide how to report VPROC status message. */
    if (Rval == OK) {
        Rval = DSP_SetEventFlag(DSP_EVENT_CAT_GEN, DSP_EVENT_TYPE_MONITOR, (UINT32)EVENT_FLAG_MONITOR_VIN_FRAMERATE);
        LL_PrintErrLine(Rval, __func__, __LINE__);
    }

    // Update Idsp debug data ready when Mctf Yuv comes, Mctf is final stage
    {
        const void *pVoid;
        const IDSP_INFO_s *pIdspInfo;
#ifdef USE_STAT_IDSP_BUF
        DSP_MEM_BLK_t MemBlk = {0};
#endif

#ifdef USE_STAT_IDSP_BUF
        Rval = DSP_GetStatBuf(DSP_STAT_BUF_IDSP, pVprocStatus->channel_id, &MemBlk);
        LL_PrintErrLine(Rval, __func__, __LINE__);
        if (Rval == OK) {
            dsp_osal_typecast(&pIdspInfo, &MemBlk.Base);
        }
#else
        pIdspInfo = &AmbaDSP_IdspInfo[pVprocStatus->channel_id];
#endif
        dsp_osal_typecast(&pVoid, &pIdspInfo);
        Rval |= DSP_GiveEvent(LL_EVENT_VPROC_IDSP_DATA_RDY, pVoid, 0U);
        LL_PrintErrLine(Rval, __func__, __LINE__);
    }

    return Rval;
}

/* VprocMsgRule observation
 * [2018/12/25]
 *  - In Effect case
 *    - Each VprocStatus message report channel's output result(not include PostJob).
 *      which means in 4View AVM case, View0 will report final address
 *                                     View1[2/3] will report temp buffer address
 *      VprocEffectStatusMsg will report final address after last channel's PostJob done
 * [Idsp Cycle]
 *  - In Single View
 *    - Idsp start pts = job_start_pts with pass_step_id = 1 (DSP_IMG_PASS_C2Y) in the VprocStatus
 *    - Idsp end pts   = job_done_pts with pass_step_id = 2 (DSP_IMG_PASS_WARP) in the VprocStatus
 *  - In Effect case
 *    - Idsp start pts: the first view's c2y start pts
 *        - In 4View AVM case, the first view means View0
 *        - job_start_pts with pass_step_id = 1 (DSP_IMG_PASS_C2Y) in the VprocStatus
 *    - Idsp end pts: the final job end pts with the last view
 *        - In 4View AVM case, the last view means View3
 *        - job_done_pts in the VprocEffectStatusMsg
 */
static UINT32 MsgParserVproc(const void *pMsgInfo)
{
    UINT32 Rval = OK;
    const dsp_msg_t *pMsg = NULL;

    dsp_osal_typecast(&pMsg, &pMsgInfo);

    PrintMsgOnParseVproc(pMsg->msg_code, pMsgInfo);

    switch (pMsg->msg_code) {
    case MSG_VPROC_STATUS:
    {
        const msg_vproc_status_t *pVprocStatus = NULL;

        dsp_osal_typecast(&pVprocStatus, &pMsgInfo);

        Rval = UpdateVprocExtDataInfo(MSG_VPROC_STATUS, pMsgInfo);
        if (Rval == OK) {
            Rval = GiveVprocEventOnParseVproc(pVprocStatus);
        } else {
            AmbaLL_LogUInt5("[Err][%d] Msg code[0x%X] UpdateVprocStatus", __LINE__, pMsg->msg_code, 0U, 0U, 0U);
        }

        break;
    }
    case MSG_VPROC_AAA_STATUS:
    {
        const msg_vproc_aaa_status_t *pVprocAaaStatus = NULL;

        dsp_osal_typecast(&pVprocAaaStatus, &pMsgInfo);

        if (pVprocAaaStatus->raw_buf_addr > 0U) {
            Rval = UpdateVprocExtDataInfo(MSG_VPROC_AAA_STATUS, pMsgInfo);
            if (Rval == OK) {
                Rval = GiveAaaEventOnParseVproc(pVprocAaaStatus);
            } else {
                AmbaLL_LogUInt5("[Err][%d] Msg code[0x%X] UpdateVprocAAA", __LINE__, pMsg->msg_code, 0U, 0U, 0U);
            }
        }

        break;
    }
    case MSG_VPROC_EXT_STATUS:
    {
        const msg_vproc_ext_status_t *pVprocExtStatus = NULL; // = (msg_vproc_ext_status_t *) pMsgInfo;
        AMBA_DSP_YUV_INFO_s YuvInfo;

        dsp_osal_typecast(&pVprocExtStatus, &pMsgInfo);

        Rval = UpdateVprocExtDataInfo(MSG_VPROC_EXT_STATUS, pMsgInfo);

        if (Rval == OK) {
            Rval = dsp_osal_memset(&YuvInfo, 0x0, sizeof(AMBA_DSP_YUV_INFO_s));
            LL_PrintErrLine(Rval, __func__, __LINE__);
            YuvInfo.ChannelId = pVprocExtStatus->channel_id;
            YuvInfo.CapSeqU32 = pVprocExtStatus->cap_seq_no;
            YuvInfo.YuvPtsU32 = pVprocExtStatus->vproc_done_hw_pts;
            Rval |= DSP_GiveEvent(LL_EVENT_VPROC_EXT_STATUS_READY, &YuvInfo, 0U);
        } else {
            AmbaLL_LogUInt5("[Err][%d] Msg code[0x%X] UpdateVprocExt", __LINE__, pMsg->msg_code, 0U, 0U, 0U);
        }
        break;
    }
    case MSG_VPROC_EXT_DATA_STATUS:
    {
        const msg_vproc_ext_data_status_t *pVprocExtDataStatus = NULL; /* = (msg_vproc_ext_data_status_t *) pMsgInfo;*/
        dsp_osal_typecast(&pVprocExtDataStatus, &pMsgInfo);

        Rval = UpdateVprocExtDataInfo(MSG_VPROC_EXT_DATA_STATUS, pMsgInfo);
        if (Rval == OK) {
            Rval = GiveHierEventOnParseVproc(pVprocExtDataStatus);
        } else {
            AmbaLL_LogUInt5("[Err][%d] Msg code[0x%X] UpdateVproc", __LINE__, pMsg->msg_code, 0U, 0U, 0U);
        }
        break;
    }
    case MSG_VPROC_COMP_OUT_STATUS:
    {
        // No event sent
        break;
    }
    case MSG_VPROC_EFFECT_DATA_STATUS:
    {
        const msg_vproc_effect_data_status_t *pEffectDataStatus = NULL;

        dsp_osal_typecast(&pEffectDataStatus, &pMsgInfo);

        Rval = GiveEfctEventOnParseVproc(pEffectDataStatus);
        break;
    }
#ifdef SUPPORT_VPROC_OSD_INSERT
	case MSG_VPROC_DELAYLINE_OSD_STATUS:
	{
		const msg_vproc_delayline_osd_status_t *pDlyLineOsdStatus = NULL;
	
		dsp_osal_typecast(&pDlyLineOsdStatus, &pMsgInfo);

		Rval = GiveDlyLineOsdEventOnParseVproc(pDlyLineOsdStatus);
		break;
	}
#endif
    default:
        Rval = DSP_ERR_0005;
        AmbaLL_LogUInt5("[Err] Unexpected MsgCode[0x%X]", pMsg->msg_code, 0U, 0U, 0U, 0U);
        break;
    }

    return Rval;
}

static UINT32 MsgParserVin(const void *pMsgInfo)
{
    UINT32 Rval = OK;
    UINT32 Idx = 0U;
    const dsp_msg_t *pMsg = NULL;

    dsp_osal_typecast(&pMsg, &pMsgInfo);

    switch (pMsg->msg_code) {
    case MSG_VIN_SENSOR_RAW_CAPTURE_STATUS:
    {
        const msg_vin_sensor_raw_capture_status_t *pVinRawCapStatus = NULL; // = (msg_vin_sensor_raw_capture_status_t *) pMsgInfo;

        dsp_osal_typecast(&pVinRawCapStatus, &pMsgInfo);
        if (DSP_ShowCmdMsgLog(MSG_VIN_SENSOR_RAW_CAPTURE_STATUS) != 0U) {
            AmbaLL_Log(AMBALLLOG_TYPE_MSG,"MSG_VIN_SENSOR_RAW_CAPTURE_STATUS", 0U, 0U, 0U);
            AmbaLL_Log(AMBALLLOG_TYPE_MSG," vin_id                          = %d  ", pVinRawCapStatus->vin_id, 0U, 0U);
            AmbaLL_Log(AMBALLLOG_TYPE_MSG," raw_capture_status              = %d  ", pVinRawCapStatus->raw_capture_status, 0U, 0U);
            AmbaLL_Log(AMBALLLOG_TYPE_MSG," is_capture_time_out             = %d  ", pVinRawCapStatus->is_capture_time_out, 0U, 0U);
            AmbaLL_Log(AMBALLLOG_TYPE_MSG," rpt_frm_cntrl                   = %d  ", pVinRawCapStatus->rpt_frm_cntrl, 0U, 0U);
            AmbaLL_Log(AMBALLLOG_TYPE_MSG," raw_cap_cnt                     = %d  ", pVinRawCapStatus->raw_cap_cnt, 0U, 0U);
            AmbaLL_Log(AMBALLLOG_TYPE_MSG," raw_cap_buf_addr                = 0x%X", pVinRawCapStatus->raw_cap_buf_addr, 0U, 0U);
            AmbaLL_Log(AMBALLLOG_TYPE_MSG," raw_cap_buf_pitch               = %d  ", pVinRawCapStatus->raw_cap_buf_pitch, 0U, 0U);
            AmbaLL_Log(AMBALLLOG_TYPE_MSG," raw_cap_buf_width               = %d  ", pVinRawCapStatus->raw_cap_buf_width, 0U, 0U);
            AmbaLL_Log(AMBALLLOG_TYPE_MSG," raw_cap_buf_height              = %d  ", pVinRawCapStatus->raw_cap_buf_height, 0U, 0U);
            AmbaLL_Log(AMBALLLOG_TYPE_MSG," is_external_raw_buf_from_system = %d  ", pVinRawCapStatus->is_external_raw_buf_from_system, 0U, 0U);
            AmbaLL_Log(AMBALLLOG_TYPE_MSG," raw_cap_buf_fid                 = 0x%X", pVinRawCapStatus->raw_cap_buf_fid, 0U, 0U);
            AmbaLL_Log(AMBALLLOG_TYPE_MSG," pts                             = %d  ", pVinRawCapStatus->pts, 0U, 0U);
            AmbaLL_Log(AMBALLLOG_TYPE_MSG," pts_diff                        = %d  ", pVinRawCapStatus->pts_diff, 0U, 0U);
            AmbaLL_Log(AMBALLLOG_TYPE_MSG," batch_addr                      = 0x%X", pVinRawCapStatus->batch_cmd_set_info.addr, 0U, 0U);
            AmbaLL_Log(AMBALLLOG_TYPE_MSG," batch_id                        = 0x%X", pVinRawCapStatus->batch_cmd_set_info.id, 0U, 0U);
            LL_PrintBatchInfoId(pVinRawCapStatus->batch_cmd_set_info.id);
            AmbaLL_Log(AMBALLLOG_TYPE_MSG," batch_size                      = %d  ", pVinRawCapStatus->batch_cmd_set_info.size, 0U, 0U);
            AmbaLL_Log(AMBALLLOG_TYPE_MSG," event_mask                      = 0x%X", pVinRawCapStatus->event_mask, 0U, 0U);
            AmbaLL_Log(AMBALLLOG_TYPE_MSG," config_addr                     = 0x%X", pVinRawCapStatus->config_addr, 0U, 0U);
            AmbaLL_Log(AMBALLLOG_TYPE_MSG," ce_cap_buf_fid                  = 0x%X", pVinRawCapStatus->ce_cap_buf_fid, 0U, 0U);
            AmbaLL_Log(AMBALLLOG_TYPE_MSG," ce_cap_buf_addr                 = 0x%X", pVinRawCapStatus->ce_cap_buf_addr, 0U, 0U);
            AmbaLL_Log(AMBALLLOG_TYPE_MSG," ce_cap_buf_pitch                = %d  ", pVinRawCapStatus->ce_cap_buf_pitch, 0U, 0U);
            AmbaLL_Log(AMBALLLOG_TYPE_MSG," ce_cap_buf_width                = %d  ", pVinRawCapStatus->ce_cap_buf_width, 0U, 0U);
            AmbaLL_Log(AMBALLLOG_TYPE_MSG," ce_cap_buf_height               = %d  ", pVinRawCapStatus->ce_cap_buf_height, 0U, 0U);
            AmbaLL_Log(AMBALLLOG_TYPE_MSG," is_external_ce_buf_from_system  = %d  ", pVinRawCapStatus->is_external_ce_buf_from_system, 0U, 0U);
            AmbaLL_Log(AMBALLLOG_TYPE_MSG," raw_cap_buf_ch_fmt              = %d  ", pVinRawCapStatus->raw_cap_buf_ch_fmt, 0U, 0U);
            AmbaLL_Log(AMBALLLOG_TYPE_MSG," frm_vproc_dalay_time_ticks      = %d  ", pVinRawCapStatus->frm_vproc_dalay_time_ticks, 0U, 0U);
            AmbaLL_Log(AMBALLLOG_TYPE_MSG," raw_repeat_default_buf_addr     = 0x%X", pVinRawCapStatus->raw_repeat_default_buf_addr, 0U, 0U);
            AmbaLL_Log(AMBALLLOG_TYPE_MSG," ce_repeat_default_buf_addr      = 0x%X", pVinRawCapStatus->ce_repeat_default_buf_addr, 0U, 0U);
            AmbaLL_Log(AMBALLLOG_TYPE_MSG," raw_cap_buf_num_tot             = %d  ", pVinRawCapStatus->raw_cap_buf_num_tot, 0U, 0U);
            AmbaLL_Log(AMBALLLOG_TYPE_MSG," raw_cap_buf_num_free            = %d  ", pVinRawCapStatus->raw_cap_buf_num_free, 0U, 0U);
            AmbaLL_Log(AMBALLLOG_TYPE_MSG," ce_cap_buf_num_tot              = %d  ", pVinRawCapStatus->ce_cap_buf_num_tot, 0U, 0U);
            AmbaLL_Log(AMBALLLOG_TYPE_MSG," ce_cap_buf_num_free             = %d  ", pVinRawCapStatus->ce_cap_buf_num_free, 0U, 0U);
            AmbaLL_Log(AMBALLLOG_TYPE_MSG," current_cap_slice_idx           = %d  ", pVinRawCapStatus->current_cap_slice_idx, 0U, 0U);
            AmbaLL_Log(AMBALLLOG_TYPE_MSG," cap_slice_num                   = %d  ", pVinRawCapStatus->cap_slice_num, 0U, 0U);
            AmbaLL_Log(AMBALLLOG_TYPE_MSG," temporal_demux_tar_vin_id       = %d  ", pVinRawCapStatus->temporal_demux_tar_vin_id, 0U, 0U);
        }

        if (pVinRawCapStatus->raw_cap_buf_addr != 0U) {
            Rval = GiveRawEventOnParseVin(pVinRawCapStatus);
        }

        /* Update Idsp debug buffer */
        if (Rval == OK) {
            if (pVinRawCapStatus->config_addr != 0U) {
                IDSP_VIN_CFG_RDY_s VinSecCfgInfo = {0U};
                ULONG ULAddr;
                const IDSP_INFO_s *pIdspInfo;
#ifdef USE_STAT_IDSP_BUF
                DSP_MEM_BLK_t MemBlk = {0};
#endif

#ifdef USE_STAT_IDSP_BUF
                Rval = DSP_GetStatBuf(DSP_STAT_BUF_IDSP, 0U, &MemBlk);
                if (Rval == OK) {
                    dsp_osal_typecast(&pIdspInfo, &MemBlk.Base);
                }
#else
                pIdspInfo = &AmbaDSP_IdspInfo[0U];
#endif

                dsp_osal_typecast(&ULAddr, &pIdspInfo);
                VinSecCfgInfo.IdspInfoBaseAddr = ULAddr;

                VinSecCfgInfo.VinId = pVinRawCapStatus->vin_id;
                VinSecCfgInfo.CapSeqNo = pVinRawCapStatus->raw_cap_cnt;
                Rval = dsp_osal_cli2virt(pVinRawCapStatus->config_addr, &ULAddr);
                LL_PrintErrLine(Rval, __func__, __LINE__);
                VinSecCfgInfo.CfgAddr = ULAddr;

                /* Update ViewZone id*/
                Rval = DSP_GiveEvent(LL_EVENT_VIN_SECTION_CFG_DATA_RDY, &VinSecCfgInfo, 0U);
            }
        }

        if (Rval == OK) {
            DspStatus.RawCapStatus[pVinRawCapStatus->vin_id] = (UINT8)pVinRawCapStatus->raw_capture_status;
            if (pVinRawCapStatus->raw_capture_status == DSP_RAW_CAP_STATUS_INVALID) {
                Idx = 1U;
            } else if (pVinRawCapStatus->raw_capture_status == DSP_RAW_CAP_STATUS_TIMER) {
                Idx = (UINT32)1UL << DSP_RAW_CAP_STATUS_TIMER;
            } else {
                Idx = (UINT32)1UL << DSP_RAW_CAP_STATUS_VIDEO;
            }
            Rval = DSP_SetEventFlagExclusive(DSP_EVENT_CAT_VIN, (UINT16)pVinRawCapStatus->vin_id, Idx);
        }
        break;
    }
    default:
        Rval = DSP_ERR_0005;
        AmbaLL_LogUInt5("Unexpected Msg code (0x%x)", pMsg->msg_code, 0U, 0U, 0U, 0U);
        break;
    }
    return Rval;
}

static UINT32 MsgParserVout(const void *pMsgInfo)
{
    UINT32 Rval = OK;
    const dsp_msg_t *pMsg = NULL;
    ULONG EventBufAddr = 0U;
    UINT16 EventBufIdx = 0U;

    dsp_osal_typecast(&pMsg, &pMsgInfo);

    switch (pMsg->msg_code) {
    case MSG_VOUT_STATUS:
        {
            const msg_vout_status_t *pVoutStatus = NULL;

            dsp_osal_typecast(&pVoutStatus, &pMsgInfo);
            if (DSP_ShowCmdMsgLog(MSG_VOUT_STATUS) != 0U) {
                AmbaLL_Log(AMBALLLOG_TYPE_MSG,"MSG_VOUT_STATUS", 0U, 0U, 0U);
                AmbaLL_Log(AMBALLLOG_TYPE_MSG,"vout_id                          = %d  ", pVoutStatus->vout_id, 0U, 0U);
                AmbaLL_Log(AMBALLLOG_TYPE_MSG,"vout_status                      = %d  ", pVoutStatus->vout_status, 0U, 0U);
                AmbaLL_Log(AMBALLLOG_TYPE_MSG,"is_vin_vout_locked               = %d  ", pVoutStatus->is_vin_vout_locked, 0U, 0U);
                AmbaLL_Log(AMBALLLOG_TYPE_MSG,"is_in_timeout_mode               = %d  ", pVoutStatus->is_in_timeout_mode, 0U, 0U);
                AmbaLL_Log(AMBALLLOG_TYPE_MSG,"polarity                         = %d  ", pVoutStatus->polarity, 0U, 0U);
                AmbaLL_Log(AMBALLLOG_TYPE_MSG,"vout_disp_fid                    = 0x%X", pVoutStatus->vout_disp_fid, 0U, 0U);
                AmbaLL_Log(AMBALLLOG_TYPE_MSG,"vout_disp_cmd_buf_id             = 0x%X", pVoutStatus->vout_disp_cmd_buf_id, 0U, 0U);
                AmbaLL_Log(AMBALLLOG_TYPE_MSG,"vout_disp_luma_addr              = 0x%X", pVoutStatus->vout_disp_luma_addr, 0U, 0U);
                AmbaLL_Log(AMBALLLOG_TYPE_MSG,"vout_disp_chroma_addr            = 0x%X", pVoutStatus->vout_disp_chroma_addr, 0U, 0U);
                AmbaLL_Log(AMBALLLOG_TYPE_MSG,"vout_disp_start_time             = %d  ", pVoutStatus->vout_disp_start_time, 0U, 0U);
                AmbaLL_Log(AMBALLLOG_TYPE_MSG,"vout_disp_done_time              = %d  ", pVoutStatus->vout_disp_done_time, 0U, 0U);
                AmbaLL_Log(AMBALLLOG_TYPE_MSG,"vout_buffer_pitch                = %d  ", pVoutStatus->vout_buffer_pitch, 0U, 0U);
                AmbaLL_Log(AMBALLLOG_TYPE_MSG,"vout_img_width                   = %d  ", pVoutStatus->vout_img_width, 0U, 0U);
                AmbaLL_Log(AMBALLLOG_TYPE_MSG,"vout_img_height                  = %d  ", pVoutStatus->vout_img_height, 0U, 0U);
            }

            Rval = DSP_SetEventFlagExclusive(DSP_EVENT_CAT_VOUT, (UINT16)pVoutStatus->vout_id, DSP_Vout_StatePattern[pVoutStatus->is_vin_vout_locked]);

            if (pVoutStatus->vout_disp_cmd_buf_id > 0U) {
                AMBA_DSP_LV_SYNC_JOB_INFO_s *pSyncJobInfo = NULL;

                Rval = DSP_RequestEventInfoBuf(EVENT_INFO_POOL_PROT_BUF, &EventBufAddr, &EventBufIdx, LL_EVENT_LV_SYNC_JOB_RDY);
                LL_PrintErrLine(Rval, __func__, __LINE__);
                if (Rval == OK) {
                    dsp_osal_typecast(&pSyncJobInfo, &EventBufAddr);
                    Rval = dsp_osal_memset(pSyncJobInfo, 0, sizeof(AMBA_DSP_LV_SYNC_JOB_INFO_s));
                    LL_PrintErrLine(Rval, __func__, __LINE__);

                    pSyncJobInfo->Status = 0U; //TBD for discard
                    pSyncJobInfo->JobId = pVoutStatus->vout_disp_cmd_buf_id;
                    Rval = DSP_GiveEvent(LL_EVENT_LV_SYNC_JOB_RDY, pSyncJobInfo, EventBufIdx);
                    LL_PrintErrLine(Rval, __func__, __LINE__);
                }
            }

            if (pVoutStatus->vout_disp_luma_addr > 0U) {
                AMBA_DSP_VOUT_DATA_INFO_s *pVoutData = NULL;

                Rval = DSP_RequestEventInfoBuf(EVENT_INFO_POOL_PROT_BUF, &EventBufAddr, &EventBufIdx, LL_EVENT_VOUT_DATA_RDY);
                LL_PrintErrLine(Rval, __func__, __LINE__);
                if (Rval == OK) {
                    dsp_osal_typecast(&pVoutData, &EventBufAddr);
                    Rval = dsp_osal_memset(pVoutData, 0, sizeof(AMBA_DSP_VOUT_DATA_INFO_s));
                    LL_PrintErrLine(Rval, __func__, __LINE__);

                    pVoutData->VoutIdx = (UINT8)pVoutStatus->vout_id;
                    pVoutData->Status = (UINT8)pVoutStatus->polarity;
                    pVoutData->YuvBuf.DataFmt = AMBA_DSP_YUV420;
                    pVoutData->YuvBuf.Pitch = (UINT16)pVoutStatus->vout_buffer_pitch;
                    pVoutData->YuvBuf.Window.Width = (UINT16)pVoutStatus->vout_img_width;
                    pVoutData->YuvBuf.Window.Height = (UINT16)pVoutStatus->vout_img_height;
                    Rval = dsp_osal_cli2virt(pVoutStatus->vout_disp_luma_addr, &pVoutData->YuvBuf.BaseAddrY);
                    LL_PrintErrLine(Rval, __func__, __LINE__);
                    Rval = dsp_osal_cli2virt(pVoutStatus->vout_disp_chroma_addr, &pVoutData->YuvBuf.BaseAddrUV);
                    LL_PrintErrLine(Rval, __func__, __LINE__);
    //FIXME. rounding
                    pVoutData->DispStartTime = (UINT64)pVoutStatus->vout_disp_start_time;
                    pVoutData->DispDoneTime = (UINT64)pVoutStatus->vout_disp_done_time;

                    Rval = DSP_GiveEvent(LL_EVENT_VOUT_DATA_RDY, pVoutData, EventBufIdx);
                    LL_PrintErrLine(Rval, __func__, __LINE__);
                }
            }
        }
        break;
    default:
        Rval = DSP_ERR_0005;
        AmbaLL_LogUInt5("[Err] Unexpected Vout msg code [0x%X]", pMsg->msg_code, 0U, 0U, 0U, 0U);
        break;
    }
    return Rval;
}

static UINT32 MsgParserEnc(const void *pMsgInfo)
{
    UINT32 Rval = OK;
    const dsp_msg_t *pMsg = NULL;

    dsp_osal_typecast(&pMsg, &pMsgInfo);

    switch (pMsg->msg_code) {
        case MSG_ENC_INPUT_BUFFER_STATUS:  // Before memd
            {
                const enc_input_buffer_status_msg_t *pEncIn = NULL;

                dsp_osal_typecast(&pEncIn, &pMsgInfo);
                Rval = HandlerEncInputBufStatus(pEncIn);
            }
            break;
        case MSG_ENC_BUFFER_STATUS:         // After memd
            {
                const enc_buffer_status_msg_t *pEncBuf = NULL;

                dsp_osal_typecast(&pEncBuf, &pMsgInfo);
                Rval = HandlerEncBufStatus(pEncBuf);
            }
            break;
        case MSG_ENC_STATUS:                // After CABAC
            {
                const enc_status_msg_t *pEncS = NULL;

                dsp_osal_typecast(&pEncS, &pMsgInfo);
                Rval = HandlerEncStatus(pEncS);
            }
            break;
        default:
            Rval = DSP_ERR_0005;
            AmbaLL_LogUInt5("[Error] Unexpected Enc msg code (0x%X)", pMsg->msg_code, 0U, 0U, 0U, 0U);
            break;
    }
    return Rval;
}

static UINT32 MsgParserDec(const void *pMsgInfo)
{
    UINT32 Rval = OK;
    const dsp_msg_t *pMsg = NULL;

    if (pMsgInfo == NULL) {
        Rval = DSP_ERR_0000;
    } else {
        dsp_osal_typecast(&pMsg, &pMsgInfo);

        switch (pMsg->msg_code) {
            case MSG_DECODER_STATUS:
                {
                    const msg_decoder_status_t *pDecStatus = NULL;

                    dsp_osal_typecast(&pDecStatus, &pMsgInfo);
                    Rval = HandlerDecStatus(pDecStatus);
                }
                break;
            case MSG_DECODER_STILLDEC:
                {
                    const msg_decoder_stilldec_t *pDecStill = NULL;

                    dsp_osal_typecast(&pDecStill, &pMsgInfo);
                    Rval = HandlerDecStilldec(pDecStill);
                }
                break;
            case MSG_DECODER_PICINFO:
                {
                    const msg_decoder_picinfo_t *pDecPicInfo = NULL;

                    dsp_osal_typecast(&pDecPicInfo, &pMsgInfo);
                    Rval = HandlerDecPicInfo(pDecPicInfo);
                }
                break;
            default:
                Rval = DSP_ERR_0005;
                AmbaLL_LogUInt5("[Err @ MsgDispatcher] Unexpected Dec msg code[0x%X]", pMsg->msg_code, 0U, 0U, 0U, 0U);
                break;
        }
    }
    return Rval;
}

static UINT32 MsgParserDecProc(const void *pMsgInfo)
{
    UINT32 Rval = OK;

    if (pMsgInfo == NULL) {
        Rval = DSP_ERR_0000;
    } else {
        const dsp_msg_t *pMsg = NULL;
        dsp_osal_typecast(&pMsg, &pMsgInfo);

        switch (pMsg->msg_code) {
            case MSG_DECPROC_STATUS:
                {
                    const msg_dproc_status_t *pDProcStatus = NULL;

                    dsp_osal_typecast(&pDProcStatus, &pMsgInfo);
                    Rval = HandlerDecProcStatus(pDProcStatus);
                }
                break;
            case MSG_DECPROC_YUV_TO_YUV:
                {
                    const msg_dproc_yuv_to_yuv_t *pDprocYuvToYuv = NULL;

                    dsp_osal_typecast(&pDprocYuvToYuv, &pMsgInfo);
                    Rval = HandlerDecProcY2y(pDprocYuvToYuv);
                }
                break;
            case MSG_DECPROC_YUV_PAINT:
                {
                    const msg_dproc_yuv_paint_t *pDprocYuvPaint = NULL;

                    dsp_osal_typecast(&pDprocYuvPaint, &pMsgInfo);
                    Rval = HandlerDecProcYuvPaint(pDprocYuvPaint);
                }
                break;
            default:
                Rval = DSP_ERR_0005;
                AmbaLL_LogUInt5("[Error @ MsgDispatcher] Unexpected Dec msg code (0x%x)", pMsg->msg_code, 0U, 0U, 0U, 0U);
                break;
        }
    }
    return Rval;
}

static UINT32 MsgParserPostProc(const void *pMsgInfo)
{
    UINT32 Rval = OK;

    if (pMsgInfo == NULL) {
        Rval = DSP_ERR_0000;
    } else {
        const dsp_msg_t *pMsg = NULL;
        dsp_osal_typecast(&pMsg, &pMsgInfo);

        switch (pMsg->msg_code) {
            case MSG_POSTPROC_STATUS:
                {
                    const msg_postp_status_t *pPostpStatus = NULL;

                    dsp_osal_typecast(&pPostpStatus, &pMsgInfo);
                    Rval = HandlerPostProcStatus(pPostpStatus);
                }
                break;
            default:
                Rval = DSP_ERR_0005;
                AmbaLL_LogUInt5("[Error @ MsgDispatcher] Unexpected Dec msg code (0x%x)", pMsg->msg_code, 0U, 0U, 0U, 0U);
                break;
        }
    }
    return Rval;
}

/**
 * Parse Vp messages
 * @param [in] pMsgInfo Pointer to the message info
 * @return 0 - OK, others - ErrorCode
 */
static UINT32 MsgParserVisionProc(const void *pMsgInfo)
{
    UINT32 Rval = OK;
    const msg_vp_hier_out_lane_out_t *pHierLane = NULL;
    UINT8 i;
    DSP_MEM_BLK_t MemBlk = {0};
    const AMBA_DSP_INIT_DATA_s *pInitData = NULL;

    Rval = DSP_GetProtBuf(DSP_PROT_BUF_INIT_DATA, 0U, &MemBlk);
    if (Rval == OK) {
        dsp_osal_typecast(&pInitData, &MemBlk.Base);

        dsp_osal_typecast(&pHierLane, pMsgInfo);
        if (pInitData->Data.run_with_vp == 1U) {
            UINT32 U32Val = 0U;

            AmbaLL_Log(AMBALLLOG_TYPE_MSG,"MSG_VISION_PROC_STATUS @ VprocMsg", 0U, 0U, 0U);
            AmbaLL_Log(AMBALLLOG_TYPE_MSG," HeirLane                                     ", 0, 0U, 0U);
            AmbaLL_Log(AMBALLLOG_TYPE_MSG,"   Header                                     ", 0, 0U, 0U);
            AmbaLL_Log(AMBALLLOG_TYPE_MSG,"     channel_id                         = %d  ", pHierLane->header.channel_id, 0U, 0U);
            AmbaLL_Log(AMBALLLOG_TYPE_MSG,"     cap_seq_no                         = %d  ", pHierLane->header.cap_seq_no, 0U, 0U);
            AmbaLL_Log(AMBALLLOG_TYPE_MSG,"     batch_cmd_id                       = 0x%X", pHierLane->header.batch_cmd_id, 0U, 0U);
            LL_PrintBatchInfoId(pHierLane->header.batch_cmd_id);
            for(i = 0; i < MAX_TOKEN_ARRAY; i++) {
                AmbaLL_Log(AMBALLLOG_TYPE_MSG,"     side_band_info[%2d].fov_id           = %d  ", i, pHierLane->header.side_band_info[i].fov_id, 0U);
                AmbaLL_Log(AMBALLLOG_TYPE_MSG,"     side_band_info[%2d].addr             = %d  ", i, pHierLane->header.side_band_info[i].fov_private_info_addr, 0U);
            }
            for(i = 0; i < MAX_ROI_CMD_TAG_NUM; i++) {
                AmbaLL_Log(AMBALLLOG_TYPE_MSG,"     roi_cmd_tag[%2d]                     = %d  ", i, pHierLane->header.roi_cmd_tag[i], 0U);
            }
            AmbaLL_Log(AMBALLLOG_TYPE_MSG,"     ln_det_hier_mask                   = 0x%X", pHierLane->header.ln_det_hier_mask, 0U, 0U);
            AmbaLL_Log(AMBALLLOG_TYPE_MSG,"     is_hier_out_tile_mode              = %d  ", pHierLane->header.is_hier_out_tile_mode, 0U, 0U);
            AmbaLL_Log(AMBALLLOG_TYPE_MSG,"     vin_cap_time                       = %d  ", pHierLane->header.vin_cap_time, 0U, 0U);
            AmbaLL_Log(AMBALLLOG_TYPE_MSG,"     proc_roi_cmd_time                  = %d  ", pHierLane->header.proc_roi_cmd_time, 0U, 0U);
            AmbaLL_Log(AMBALLLOG_TYPE_MSG,"     c2y_done_time                      = %d  ", pHierLane->header.c2y_done_time, 0U, 0U);
            AmbaLL_Log(AMBALLLOG_TYPE_MSG,"     warp_mctf_done_time                = %d  ", pHierLane->header.warp_mctf_done_time, 0U, 0U);

            for(i = 0; i < DSP_HIER_NUM; i++) {
                AmbaLL_Log(AMBALLLOG_TYPE_MSG,"     LumaHier[%2d].BufAddr                = 0x%X", i, pHierLane->luma_hier_outs[i].buffer_addr, 0U);
                AmbaLL_Log(AMBALLLOG_TYPE_MSG,"     LumaHier[%2d].BufPitch               = %d  ", i, pHierLane->luma_hier_outs[i].buffer_pitch, 0U);
                AmbaLL_Log(AMBALLLOG_TYPE_MSG,"     LumaHier[%2d].ImgWidth               = %d  ", i, pHierLane->luma_hier_outs[i].img_width, 0U);
                AmbaLL_Log(AMBALLLOG_TYPE_MSG,"     LumaHier[%2d].ImgHeight              = %d  ", i, pHierLane->luma_hier_outs[i].img_height, 0U);
            }
            for(i = 0; i < DSP_HIER_NUM; i++) {
                AmbaLL_Log(AMBALLLOG_TYPE_MSG,"     ChromaHier[%2d].BufAddr               = 0x%X", i, pHierLane->chroma_hier_outs[i].buffer_addr, 0U);
                AmbaLL_Log(AMBALLLOG_TYPE_MSG,"     ChromaHier[%2d].BufPitch              = %d  ", i, pHierLane->chroma_hier_outs[i].buffer_pitch, 0U);
                AmbaLL_Log(AMBALLLOG_TYPE_MSG,"     ChromaHier[%2d].ImgWidth              = %d  ", i, pHierLane->chroma_hier_outs[i].img_width, 0U);
                AmbaLL_Log(AMBALLLOG_TYPE_MSG,"     ChromaHier[%2d].ImgHeight             = %d  ", i, pHierLane->chroma_hier_outs[i].img_height, 0U);
            }
            for(i = 0; i < DSP_HIER_NUM; i++) {
                U32Val = (UINT32)pHierLane->hier_out_offsets[i].x_offset;
                AmbaLL_Log(AMBALLLOG_TYPE_MSG,"     HierOut[%2d].OffsetX                 = %d  ", i, U32Val, 0U);
                U32Val = (UINT32)pHierLane->hier_out_offsets[i].y_offset;
                AmbaLL_Log(AMBALLLOG_TYPE_MSG,"     HierOut[%2d].OffsetY                 = %d  ", i, U32Val, 0U);
            }
            AmbaLL_Log(AMBALLLOG_TYPE_MSG,"     LumaLaneOut.BufAddr                = 0x%X", pHierLane->luma_lane_det_out.buffer_addr, 0U, 0U);
            AmbaLL_Log(AMBALLLOG_TYPE_MSG,"     LumaLaneOut.BufPitch               = %d  ", pHierLane->luma_lane_det_out.buffer_pitch, 0U, 0U);
            AmbaLL_Log(AMBALLLOG_TYPE_MSG,"     LumaLaneOut.ImgWidth               = %d  ", pHierLane->luma_lane_det_out.img_width, 0U, 0U);
            AmbaLL_Log(AMBALLLOG_TYPE_MSG,"     LumaLaneOut.ImgHeight              = %d  ", pHierLane->luma_lane_det_out.img_height, 0U, 0U);
            AmbaLL_Log(AMBALLLOG_TYPE_MSG,"     ChromaLaneOut.BufAddr              = 0x%X", pHierLane->chroma_lane_det_out.buffer_addr, 0U, 0U);
            AmbaLL_Log(AMBALLLOG_TYPE_MSG,"     ChromaLaneOut.BufPitch             = %d  ", pHierLane->chroma_lane_det_out.buffer_pitch, 0U, 0U);
            AmbaLL_Log(AMBALLLOG_TYPE_MSG,"     ChromaLaneOut.ImgWidth             = %d  ", pHierLane->chroma_lane_det_out.img_width, 0U, 0U);
            AmbaLL_Log(AMBALLLOG_TYPE_MSG,"     ChromaLaneOut.ImgHeight            = %d  ", pHierLane->chroma_lane_det_out.img_height, 0U, 0U);
            U32Val = (UINT32)pHierLane->lane_det_out_offset.x_offset;
            AmbaLL_Log(AMBALLLOG_TYPE_MSG,"     LaneOut.OffsetX                    = %d  ", U32Val, 0U, 0U);
            U32Val = (UINT32)pHierLane->lane_det_out_offset.y_offset;
            AmbaLL_Log(AMBALLLOG_TYPE_MSG,"     LaneOut.OffsetY                    = %d  ", U32Val, 0U, 0U);
        }
    }

    return Rval;
}

static UINT32 MsgParserNoneImpl(const void *pMsgInfo)
{
    UINT32 Rval = OK;

    if (pMsgInfo == NULL) {
        Rval = DSP_ERR_0000;
    } else {
        const DSP_MSG *pMsg;

        dsp_osal_typecast(&pMsg, &pMsgInfo);
        AmbaLL_LogUInt5("[MsgParserNoneImpl] Not Impl MsgCat[0x%X]", pMsg->msg_code, 0U, 0U, 0U, 0U);
    }

    return Rval;
}

UINT32 LL_MsgParser(const AMBA_DSP_MSG_STATUS_s *pStatusMsg)
{
    UINT32 MsgCat = 0U;
    UINT32 Rval = OK;

    static const AMBA_DSP_MSG_PARSER_f AmbaDSP_MsgParserTable[CAT_POSTPROC+1U] = {
        [CAT_DSP_HEADER]    = MsgParserNoneImpl,
        [CAT_DSP_CFG]       = MsgParserDSPStatus,
        [CAT_VPROC]         = MsgParserVproc,
        [CAT_SPROC]         = MsgParserNoneImpl,
        [CAT_VIN]           = MsgParserVin,
        [CAT_VOUT]          = MsgParserVout,
        [CAT_ENC]           = MsgParserEnc,
        [CAT_DEC]           = MsgParserDec,
        [CAT_DPROC]         = MsgParserDecProc,
        [CAT_IDSP]          = MsgParserNoneImpl,
        [CAT_POSTPROC]      = MsgParserPostProc,
    };

    const UINT16 *StatusMsg = NULL;

    dsp_osal_typecast(&StatusMsg, &pStatusMsg);
    StatusMsg++;
    if (*StatusMsg == VP_MSG_PATTERN) {
        Rval = MsgParserVisionProc(pStatusMsg);
    } else {
        GET_DSP_CMD_CAT(pStatusMsg->Contents.msg_code, &MsgCat);
        if ((MsgCat <= CAT_POSTPROC) && (AmbaDSP_MsgParserTable[MsgCat] != NULL )) {
            Rval = (AmbaDSP_MsgParserTable[MsgCat])(pStatusMsg);
        } else {
            AmbaLL_LogUInt5("[Err] Unexpected MsgCat[0x%X]", pStatusMsg->Contents.msg_code, 0U, 0U, 0U, 0U);
            Rval = DSP_ERR_0001;
        }
    }
    return Rval;
}

UINT32 DSP_WaitPostPVoutExist(UINT32 TimeOutMs)
{
    UINT32 Rval;
    UINT32 ActualFlags;
    UINT32 WaitFlags = AMBA_DSP_POSTP_VOUT_EXIST_FLAG_PATTERN;

    Rval = DSP_WaitEventFlag(DSP_EVENT_CAT_GEN, DSP_EVENT_TYPE_PPVOUT , WaitFlags, 1U, 0U, &ActualFlags, TimeOutMs);

    if (Rval != OK) {
        AmbaLL_LogUInt5("[Err][%d] Wait PostPVoutExist failed (0x%X 0x%X)", __LINE__, WaitFlags, ActualFlags, 0U, 0U);
        Rval = DSP_ERR_0004;
    }

    return Rval;
}
#if 0
UINT32 DSP_ClearVprocState(UINT8 ChId, UINT8 VprocState)
{
    UINT32 Rval;

    Rval = DSP_ClearEventFlag(DSP_EVENT_CAT_VPROC, (UINT16)ChId, DSP_VPROC_StatePattern[VprocState]);

    return Rval;
}
#endif
UINT32 DSP_WaitVprocState(UINT8 ChId, UINT8 VprocState, UINT32 TimeOutMs, UINT8 NotClearFlag)
{
    UINT32 ActualFlags = OK;
    UINT8 CurrVprocState;
    UINT32 Rval = OK;

    CurrVprocState = (UINT8)DspStatus.VprocStatus[ChId];

    if (CurrVprocState == VprocState) {
        // already in desired state
    } else {
        if (NotClearFlag == 0U) {
            Rval = DSP_ClearEventFlag(DSP_EVENT_CAT_VPROC, (UINT16)ChId, DSP_VPROC_StatePattern[VprocState]);
        }
        if (Rval == OK) {
            Rval = DSP_WaitEventFlag(DSP_EVENT_CAT_VPROC, (UINT16)ChId, DSP_VPROC_StatePattern[VprocState],
                                     1U, 0U, &ActualFlags, TimeOutMs);
        }
        if (Rval != OK) {
            AmbaLL_LogUInt5( "[Err][%d] Can't wait for VprocState[%d]. Current [%d] Act[%d] Rval[0x%x]", __LINE__, VprocState, CurrVprocState, DspStatus.VprocStatus[ChId], Rval);
            Rval = DSP_ERR_0004;
        }
    }
    return Rval;

}

UINT8 DSP_GetVprocState(UINT8 ChId)
{
    UINT8 CurrVprocState;

    CurrVprocState = DspStatus.VprocStatus[ChId];

    return CurrVprocState;
}

UINT32 DSP_WaitVinState(UINT8 VinId, UINT8 VinState, UINT32 TimeOutMs)
{
    UINT32 Rval = OK;
    UINT32 ActualFlags = OK;
    UINT8 CurrVinState;
    UINT32 Idx;

    CurrVinState = DspStatus.RawCapStatus[VinId];

    if (CurrVinState == VinState) {
        Rval = OK;
    } else {
        if (VinState == DSP_RAW_CAP_STATUS_INVALID) {
            Idx = 1U;
        } else if (VinState == DSP_RAW_CAP_STATUS_TIMER) {
            Idx = ((UINT32)1UL << DSP_RAW_CAP_STATUS_TIMER);
        } else {
            Idx = ((UINT32)1UL << DSP_RAW_CAP_STATUS_VIDEO);
        }

        Rval = DSP_ClearEventFlag(DSP_EVENT_CAT_VIN, VinId, Idx);

        if (Rval == OK) {
            Rval = DSP_WaitEventFlag(DSP_EVENT_CAT_VIN, VinId, Idx, 1U, 0U, &ActualFlags, TimeOutMs);
        }
        if (Rval != OK) {
            AmbaLL_LogUInt5( "[Err][%d] Can't wait for VinState[%d][%d]. Current [%d] Rval[0x%x]", __LINE__, VinId, VinState, CurrVinState, Rval);
            Rval = DSP_ERR_0004;
        }
    }

    return Rval;
}

UINT8 DSP_GetVinState(UINT8 VinId)
{
    return DspStatus.RawCapStatus[VinId];
}

UINT8 DSP_GetEncState(UINT16 ChId)
{
    UINT8 CurrEncState = DSP_ENC_STATUS_IDLE;

    if (ChId >= AMBA_DSP_MAX_STREAM_NUM) {
        AmbaLL_LogUInt5( "[Err][%d] EncChanId[%d] exceeds", __LINE__, ChId, 0U, 0U, 0U);
    } else {
        CurrEncState = DspStatus.EncStatus[ChId];
    }
    return CurrEncState;
}

UINT32 DSP_WaitEncState(UINT32 ChId, UINT8 EncState, UINT32 TimeOutMs)
{
    UINT32 ActualFlags = OK;
    UINT8 CurrEncState = DspStatus.EncStatus[ChId];
    UINT32 Rval = DSP_ERR_0004;

    if(CurrEncState == EncState) {
        Rval = OK;
    } else {
        if (DSP_ClearEventFlag(DSP_EVENT_CAT_GEN, DSP_EVENT_TYPE_ENC, DSP_Enc_StatePattern[EncState]) == OK) {
            Rval = DSP_WaitEventFlag(DSP_EVENT_CAT_GEN, DSP_EVENT_TYPE_ENC, DSP_Enc_StatePattern[EncState],
                                     1U, 0U, &ActualFlags, TimeOutMs);
        }
        if (Rval != OK) {
            AmbaLL_LogUInt5( "[Err][%d] Can't wait for EncState[%d]. Current [%d] Rval[0x%x]", __LINE__, CurrEncState, DSP_Enc_StatePattern[EncState], Rval, 0U);
            Rval =  DSP_ERR_0004;
        }
    }
    return Rval;
}

UINT32 DSP_WaitVoutLock(UINT8 VoutId, UINT8 LockState, UINT32 TimeOutMs)
{
    UINT32 ActualFlags = OK;
    UINT8 CurrLockState = DspStatus.VoutLockStatus[VoutId];
    UINT32 Rval = DSP_ERR_0004;

    if(CurrLockState == LockState) {
        Rval = OK;
    } else {
        if (DSP_ClearEventFlag(DSP_EVENT_CAT_VOUT, (UINT16)VoutId, DSP_Vout_StatePattern[LockState]) == OK) {
            Rval = DSP_WaitEventFlag(DSP_EVENT_CAT_VOUT, (UINT16)VoutId, DSP_Vout_StatePattern[LockState],
                                     1U, 0U, &ActualFlags, TimeOutMs);
        }
        if (Rval != OK) {
            AmbaLL_LogUInt5( "[Err][%d] Can't wait for VoutLockState[%d][%d]. Current [%d] Rval[0x%x]", __LINE__, VoutId, CurrLockState, DSP_Vout_StatePattern[LockState], Rval);
            Rval = DSP_ERR_0004;
        } else {
            Rval = OK;
        }
    }
    return Rval;
}

UINT8 DSP_GetProfState(void)
{
    UINT8 CurrProfState = DSP_PROF_STATUS_INVALID;

    if (DspStatus.ProfCfgID == DSP_REPORTED_PROF_2_IDLE) {
        CurrProfState = DSP_PROF_STATUS_2_IDLE;
    } else if ((DspStatus.ProfCfgID == DSP_REPORTED_PROF_IDLE) ||
               (DspStatus.ProfCfgID == DSP_REPORTED_PROF_CAMERA) ||
               (DspStatus.ProfCfgID == DSP_REPORTED_PROF_SAFETY)) {
        CurrProfState = DspStatus.ProfStatus;
    } else if (DspStatus.ProfCfgID <= DSP_REPORTED_PROF_PLAYBACK) {
        CurrProfState = (UINT8)DspStatus.ProfCfgID;
    } else {
        // DO NOTHING
    }

    return CurrProfState;
}

UINT32 DSP_SetProfState(UINT8 Status)
{
    DspStatus.ProfStatus = Status;

    return OK;
}

UINT32 DSP_WaitProfState(UINT8 ProfState, UINT32 TimeOutMs)
{
    UINT32 Rval;
    UINT32 ActualFlags = OK;
    UINT8 CurrProfState = DSP_PROF_STATUS_INVALID;

    Rval = DSP_ClearEventFlag(DSP_EVENT_CAT_GEN, DSP_EVENT_TYPE_PROF, DSP_PROF_StatePattern[ProfState]);

    if (Rval == OK) {
        if (DspStatus.ProfCfgID <= DSP_REPORTED_PROF_PLAYBACK) {
            CurrProfState = (UINT8)DspStatus.ProfCfgID;
        } else if (DspStatus.ProfCfgID == DSP_REPORTED_PROF_2_IDLE) {
            CurrProfState = DSP_PROF_STATUS_2_IDLE;
        } else if (DspStatus.ProfCfgID == DSP_REPORTED_PROF_IDLE) {
            CurrProfState = DSP_PROF_STATUS_IDLE;
        } else {
            // DO NOTHING
        }

        if (CurrProfState == ProfState) {
            Rval = OK;
        } else {
            Rval = DSP_WaitEventFlag(DSP_EVENT_CAT_GEN, DSP_EVENT_TYPE_PROF, DSP_PROF_StatePattern[ProfState],
                                     1U, 0U, &ActualFlags, TimeOutMs);
            if (Rval != OK) {
                AmbaLL_LogUInt5("[Err][%d] Wait ProfState[%d] Current[%d] Rval[0x%x]", __LINE__, ProfState, CurrProfState, Rval, 0U);
                Rval = DSP_ERR_0004;
            }
        }
    }
    return Rval;
}

UINT8 DSP_GetDecState(UINT16 ChId)
{
    UINT8 CurrDecState = DSP_DEC_OPM_INVALID;

    if (ChId >= AMBA_DSP_MAX_DEC_STREAM_NUM) {
        AmbaLL_LogUInt5( "[Err][%d] DecChanId[%d] exceeds", __LINE__, ChId, 0U, 0U, 0U);
    } else {
        CurrDecState = DspStatus.DecStatus[ChId];
    }
    return CurrDecState;
}

UINT32 DSP_WaitDecState(UINT32 ChId, UINT8 DecState, UINT32 TimeOutMs, UINT8 NotClearFlag)
{
    UINT32 ActualFlags = OK;
    UINT8 CurrDecState;
    UINT32 Rval = OK;

    if (ChId >= AMBA_DSP_MAX_DEC_STREAM_NUM) {
        AmbaLL_LogUInt5( "[Err][%d] DecChanId[%d] exceeds", __LINE__, ChId, 0U, 0U, 0U);
    } else {
        CurrDecState = (UINT8)DspStatus.DecStatus[ChId];

        if (CurrDecState == DecState) {
            // already in desired state
        } else {
            if (NotClearFlag == 0U) {
                Rval = DSP_ClearEventFlag(DSP_EVENT_CAT_GEN, DSP_EVENT_TYPE_DEC, DSP_Dec_StatePattern[DecState]);
            }
            if (Rval == OK) {
                Rval = DSP_WaitEventFlag(DSP_EVENT_CAT_GEN, DSP_EVENT_TYPE_DEC, DSP_Dec_StatePattern[DecState],
                                         1U, 0U, &ActualFlags, TimeOutMs);
            }
            if (Rval != OK) {
                AmbaLL_LogUInt5( "[Err][%d] Can't wait for DecStatus[%d]. Current [%d] Act[%d] Rval[0x%x]",
                        __LINE__, DecState, CurrDecState, DspStatus.DecStatus[ChId], Rval);
                Rval = DSP_ERR_0004;
            }
        }
    }
    return Rval;
}

