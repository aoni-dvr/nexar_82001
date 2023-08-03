/**
*  @file AmbaDSP_VideoDec.c
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
*  @details Implement of SSP Video Decoder related APIs
*
*/

#include "AmbaDSP_Log.h"
#include "AmbaDSP_Cmds.h"
#include "AmbaDSP_ArmComm.h"
#include "AmbaDSP_ApiDump.h"
#include "AmbaDSP_ApiCheck.h"
#include "AmbaDSP_ApiTiming.h"
#include "AmbaDSP_EncodeAPI.h"
#include "AmbaDSP_EventCtrl.h"
#include "AmbaDSP_ApiResource.h"
#include "AmbaDSP_EncodeUtility.h"
#include "AmbaDSP_DecodeUtility.h"
#include "dsp_priv_api.h"

/**
* Bits Stream config function
* @param [in]  DSP process command mode
* @param [in]  Number of Stream Index
* @return ErrorCode
*/
static UINT32 HL_VideoDecBitsStreamImpl(const UINT8 WriteMode, UINT16 StreamIdx)
{
    UINT8 CmdBufferId = 0U; void *CmdBufferAddr = NULL;
    UINT32 Rval;
    cmd_decoder_setup_t *DecSetupCfg = HL_DefCtxCmdBufPtrDec;

    HL_AcqCmdBuffer(&CmdBufferId, &CmdBufferAddr);
    dsp_osal_typecast(&DecSetupCfg, &CmdBufferAddr);
    Rval = HL_FillDecBitsStreamSetup(StreamIdx, DecSetupCfg);
    if (Rval != OK) {
        AmbaLL_LogUInt5("[Err][0x%X] DecBitsStream fill", Rval, 0U, 0U, 0U, 0U);
    } else {
        Rval = AmbaHL_CmdDecoderSetup(WriteMode, DecSetupCfg);
        if (Rval != OK) {
            AmbaLL_LogUInt5("[Err][0x%X] BitsUpdateCmd", Rval, 0U, 0U, 0U, 0U);
        }
    }
    HL_RelCmdBuffer(CmdBufferId);

    return Rval;
}

static UINT32 HL_VideoDecBitsUpdateImpl(const UINT8 WriteMode, const UINT16 StreamIdx, const AMBA_DSP_VIDDEC_BITS_FIFO_s *pBitsFifo)
{
    UINT8 CmdBufferId = 0U; void *CmdBufferAddr = NULL;
    UINT32 Rval;
    cmd_decoder_bitsfifo_update_t *UpdateCfg = HL_DefCtxCmdBufPtrDecBsUpt;

    HL_AcqCmdBuffer(&CmdBufferId, &CmdBufferAddr);
    dsp_osal_typecast(&UpdateCfg, &CmdBufferAddr);
    Rval = HL_FillBitsUpdateSetup(StreamIdx, pBitsFifo, UpdateCfg);
    if (Rval != OK) {
        AmbaLL_LogUInt5("[Err][0x%X] Bits Update fill", Rval, 0U, 0U, 0U, 0U);
    } else {
        Rval = AmbaHL_CmdDecoderBitsfifoUpdate(WriteMode, UpdateCfg);
        if (Rval != OK) {
            AmbaLL_LogUInt5("[Err][0x%X] BitsUpdateCmd", Rval, 0U, 0U, 0U, 0U);
        }
    }
    HL_RelCmdBuffer(CmdBufferId);

    return Rval;
}

static UINT32 HL_VideoDecSpeedImpl(UINT16 DecIdx, const AMBA_DSP_VIDDEC_START_CONFIG_s *pStartConfig)
{
    UINT8 CmdBufferId = 0U; void *CmdBufferAddr = NULL;
    UINT32 Rval;
    cmd_decoder_speed_t *SpdDirCmd = HL_DefCtxCmdBufPtrDecSpeed;

    HL_AcqCmdBuffer(&CmdBufferId, &CmdBufferAddr);
    dsp_osal_typecast(&SpdDirCmd, &CmdBufferAddr);
    Rval = HL_FillDecSpeedSetup(DecIdx, pStartConfig, SpdDirCmd);
    if (Rval == OK) {
        Rval = AmbaHL_CmdDecoderSpeed(AMBA_DSP_CMD_VDSP_NORMAL_WRITE, SpdDirCmd);
    }
    HL_RelCmdBuffer(CmdBufferId);
    if (Rval != OK) {
        AmbaLL_LogUInt5("[Err][0x%X] DecSpeedCfg", Rval, 0U, 0U, 0U, 0U);
        Rval = DSP_ERR_0007;
    }

    return Rval;
}

static UINT32 HL_VideoDecTrickImpl(UINT16 DecIdx, const AMBA_DSP_VIDDEC_TRICKPLAY_s *pTrickConfig)
{
    UINT8 CmdBufferId = 0U; void *CmdBufferAddr = NULL;
    UINT32 Rval;
    CTX_VID_DEC_INFO_s VidDecInfo;
    cmd_decoder_trickplay_t *pTrickCmd = HL_DefCtxCmdBufPtrDecTrick;

    HL_GetVidDecInfo(HL_MTX_OPT_ALL, DecIdx, &VidDecInfo);
    if (BIND_STILL_PROC == VidDecInfo.CurrentBind) {
        UINT16 ViewZoneId = VidDecInfo.ViewZoneId;
        UINT16 YuvInVirtVinId = VidDecInfo.YuvInVirtVinId + AMBA_DSP_MAX_VIN_NUM;
        cmd_binding_cfg_t *FpBindCfg = HL_DefCtxCmdBufPtrBind;

        /* 4.Unbind virtual vin */
        HL_AcqCmdBuffer(&CmdBufferId, &CmdBufferAddr);
        dsp_osal_typecast(&FpBindCfg, &CmdBufferAddr);
        Rval = HL_FillFpBindSetup(FpBindCfg, BIND_STILL_PROC, 0U/*IsBind*/,
                                  YuvInVirtVinId/*SrcId -> src_fp_id*/,
                                  0U/*SrcPinId*/,
                                  ViewZoneId/*StrmId -> dst_ch_id*/);
        if (Rval != OK) {
            AmbaLL_LogUInt5("[HL_LiveviewDec2VprocIsoCfgPauseImpl] %d FpBind filling fail", __LINE__, 0U, 0U, 0U, 0U);
        } else {
            (void)AmbaHL_CmdDspBindingCfg(AMBA_DSP_CMD_VCAP_NORMAL_WRITE, FpBindCfg);
        }
        HL_RelCmdBuffer(CmdBufferId);
        Rval = DSP_WaitVdspEvent((UINT32)EVENT_FLAG_INT_VDSP0_EVENT, 2, WAIT_INTERRUPT_TIMEOUT);
        if (Rval != OK) {
            AmbaLL_LogUInt5("[HL_LiveviewDec2VprocIsoCfgPauseImpl] %d FpBind DSP_WaitVdspEvent fail", __LINE__, 0U, 0U, 0U, 0U);
        }

        /* 5.Bind decoder back */
        HL_AcqCmdBuffer(&CmdBufferId, &CmdBufferAddr);
        dsp_osal_typecast(&FpBindCfg, &CmdBufferAddr);
        Rval = HL_FillFpBindSetup(FpBindCfg, BIND_VIDEO_DECODE_TO_VPROC, 1U/*IsBind*/,
                                  DecIdx/*SrcId -> src_ch_id*/,
                                  0U/*SrcPinId -> xx*/,
                                  ViewZoneId/*StrmId -> dst_ch_id*/);
        if (Rval != OK) {
            AmbaLL_LogUInt5("[HL_LiveviewDec2VprocIsoCfgPauseImpl] %d FpBind filling fail", __LINE__, 0U, 0U, 0U, 0U);
        } else {
            (void)AmbaHL_CmdDspBindingCfg(AMBA_DSP_CMD_VDSP_NORMAL_WRITE, FpBindCfg);

            HL_GetVidDecInfo(HL_MTX_OPT_GET, DecIdx, &VidDecInfo);
            VidDecInfo.CurrentBind = BIND_VIDEO_DECODE_TO_VPROC;
            HL_SetVidDecInfo(HL_MTX_OPT_SET, DecIdx, &VidDecInfo);

            Rval = DSP_WaitVdspEvent((UINT32)EVENT_FLAG_INT_VDSP0_EVENT, 2, WAIT_INTERRUPT_TIMEOUT);
            if (Rval != OK) {
                AmbaLL_LogUInt5("[HL_LiveviewDec2VprocIsoCfgPauseImpl] %d FpBind DSP_WaitVdspEvent fail",
                        __LINE__, 0U, 0U, 0U, 0U);
            }
        }
        HL_RelCmdBuffer(CmdBufferId);
    }

    HL_AcqCmdBuffer(&CmdBufferId, &CmdBufferAddr);
    dsp_osal_typecast(&pTrickCmd, &CmdBufferAddr);
    Rval = HL_FillDecTrickSetup(DecIdx, pTrickConfig, pTrickCmd);
    if (Rval != OK) {
        AmbaLL_LogUInt5("[Err][0x%X][%d] VideoDecTrickPlay Setup[%u]", Rval, __LINE__, DecIdx, 0U, 0U);
    }

    if (Rval == OK) {
        Rval = AmbaHL_CmdDecoderTrickPlay(AMBA_DSP_CMD_VDSP_NORMAL_WRITE, pTrickCmd);
        if (Rval != OK) {
            AmbaLL_LogUInt5("[Err][0x%X][%d] DecTrickPlay Config", Rval,  __LINE__, 0U, 0U, 0U);
            Rval = DSP_ERR_0007;
        }
    }
    HL_RelCmdBuffer(CmdBufferId);

    HL_GetVidDecInfo(HL_MTX_OPT_GET, DecIdx, &VidDecInfo);
    if (AMBA_DSP_VIDDEC_PAUSE == pTrickConfig[0].Operation) {
        VidDecInfo.State = VIDDEC_STATE_PAUSE;
    } else if (AMBA_DSP_VIDDEC_RESUME == pTrickConfig[0].Operation) {
        VidDecInfo.State = VIDDEC_STATE_RUN;
    } else if (AMBA_DSP_VIDDEC_STEP == pTrickConfig[0].Operation) {
        // don't need to change
    } else {
        AmbaLL_LogUInt5("TrickPlay check error", 0U, 0U, 0U, 0U, 0U);
    }
    HL_SetVidDecInfo(HL_MTX_OPT_SET, DecIdx, &VidDecInfo);

    return Rval;
}

static UINT32 HL_VideoDecBatchImpl(UINT16 DecIdx)
{
    UINT8 CmdBufferId = 0U; void *CmdBufferAddr = NULL;
    UINT32 Rval = OK;
    CTX_VID_DEC_INFO_s VidDecInfo;
    CTX_VIEWZONE_INFO_s *ViewZoneInfo = HL_CtxViewZoneInfoPtr;
#ifdef SUPPORT_DSP_DEC_BATCH
    cmd_decoder_batch_t *DecBchInfo = HL_DefCtxCmdBufPtrDecBch;
#endif

    HL_GetVidDecInfo(HL_MTX_OPT_GET, DecIdx, &VidDecInfo);

    /* Valid viewzone means Dec2Vproc case */
    if (VidDecInfo.ViewZoneId != DSP_VPROC_IDX_INVALID) {
        HL_GetViewZoneInfoLock(VidDecInfo.ViewZoneId, &ViewZoneInfo);

        if ((ViewZoneInfo->StartIsoCfgAddr != 0U) &&
            (VidDecInfo.FirstIsoCfgIssued == (UINT8)0U)) {

#ifdef SUPPORT_DSP_DEC_BATCH
            UINT32 *pBatchQAddr = NULL, BatchCmdId = 0U, NewWp = 0U;
            ULONG ULAddr = 0U;

            /* Request BatchCmdQ buffer */
            Rval = DSP_ReqBuf(&ViewZoneInfo->BatchQPoolDesc, 1U, &NewWp, 1U/*FullnessCheck*/);
            if (Rval != OK) {
                HL_GetViewZoneInfoUnLock(VidDecInfo.ViewZoneId);
            } else {
                ViewZoneInfo->BatchQPoolDesc.Wp = NewWp;
                VidDecInfo.FirstIsoCfgIssued = (UINT8)1U;
                HL_GetViewZoneInfoUnLock(VidDecInfo.ViewZoneId);
                HL_GetPointerToDspBatchQ(VidDecInfo.ViewZoneId,
                                         (UINT16)ViewZoneInfo->BatchQPoolDesc.Wp,
                                         &pBatchQAddr,
                                         &BatchCmdId);
                /* Reset New BatchQ after Wp advanced */
                HL_ResetDspBatchQ(pBatchQAddr);

                dsp_osal_typecast(&pIsoCfgCmd, &pBatchQAddr);
                (void)HL_FillIsoCfgUpdate(VidDecInfo.ViewZoneId, pIsoCfgCmd);
                (void)dsp_osal_virt2cli(ViewZoneInfo->StartIsoCfgAddr, &pIsoCfgCmd->idsp_flow_addr);
                pIsoCfgCmd->ik_cfg_id = ViewZoneInfo->StartIsoCfgIndex;

                HL_SetDspBatchQInfo(BatchCmdId, 1U/*AutoReset*/, BATCHQ_INFO_ISO_CFG, ViewZoneInfo->StartIsoCfgIndex);
                HL_AcqCmdBuffer(&CmdBufferId, &CmdBufferAddr);
                dsp_osal_typecast(&DecBchInfo, &CmdBufferAddr);
                DecBchInfo->batch_cmd_set_info.id = BatchCmdId;
                dsp_osal_typecast(&ULAddr, &pBatchQAddr);
                (void)dsp_osal_virt2cli(ULAddr, &DecBchInfo->batch_cmd_set_info.addr);
                DecBchInfo->batch_cmd_set_info.size = HL_GetBatchCmdNumber(ULAddr)*CMD_SIZE_IN_BYTE;
                DecBchInfo->hdr.decoder_id = 0U;
                DecBchInfo->hdr.codec_type = HL_DecCodec2DspDecCodec[VidDecInfo.BitsFormat];
                Rval = AmbaHL_CmdDecoderBatch(AMBA_DSP_CMD_VDSP_NORMAL_WRITE, DecBchInfo);
                HL_RelCmdBuffer(CmdBufferId);
                if (Rval != OK) {
                    AmbaLL_LogUInt5("DecoderBatch fail[0x%X]", Rval, 0U, 0U, 0U, 0U);
                }
            }
#else
            cmd_vproc_ik_config_t *VprocIkCfg = HL_DefCtxCmdBufPtrVpcIkCfg;

            HL_GetViewZoneInfoUnLock(VidDecInfo.ViewZoneId);

            //Using CMD_VPROC_IK_CONFIG when bring-up
            AmbaLL_LogUInt5("[DEC] Force using CMD_VPROC_IK_CONFIG", 0U, 0U, 0U, 0U, 0U);

            HL_AcqCmdBuffer(&CmdBufferId, &CmdBufferAddr);
            dsp_osal_typecast(&VprocIkCfg, &CmdBufferAddr);
            (void)HL_FillIsoCfgUpdate(VidDecInfo.ViewZoneId, VprocIkCfg);
            (void)dsp_osal_virt2cli(ViewZoneInfo->StartIsoCfgAddr, &VprocIkCfg->idsp_flow_addr);
            VprocIkCfg->ik_cfg_id = ViewZoneInfo->StartIsoCfgIndex;
            Rval = AmbaHL_CmdVprocIkConfig(AMBA_DSP_CMD_VDSP_NORMAL_WRITE, VprocIkCfg);
            HL_RelCmdBuffer(CmdBufferId);
            if (Rval != OK) {
                AmbaLL_LogUInt5("CMD_VPROC_IK_CONFIG fail[0x%X]", Rval, 0U, 0U, 0U, 0U);
            }
            VidDecInfo.FirstIsoCfgIssued = (UINT8)1U;
#endif
        } else {
            HL_GetViewZoneInfoUnLock(VidDecInfo.ViewZoneId);
        }
    }
    HL_SetVidDecInfo(HL_MTX_OPT_SET, DecIdx, &VidDecInfo);

    return Rval;
}

static UINT32 HL_VideoDecStartImpl(UINT16 DecIdx, const AMBA_DSP_VIDDEC_START_CONFIG_s *pStartConfig)
{
    UINT8 CmdBufferId = 0U; void *CmdBufferAddr = NULL;
    UINT32 Rval;
    CTX_VID_DEC_INFO_s VidDecInfo;
    cmd_decoder_start_t *StartCmd = HL_DefCtxCmdBufPtrDecStart;

    HL_GetVidDecInfo(HL_MTX_OPT_ALL, DecIdx, &VidDecInfo);
    HL_AcqCmdBuffer(&CmdBufferId, &CmdBufferAddr);
    dsp_osal_typecast(&StartCmd, &CmdBufferAddr);
    Rval = HL_FillDecStartSetup(DecIdx, pStartConfig, StartCmd);

    if (VidDecInfo.State == VIDDEC_STATE_READY) {
        Rval = AmbaHL_CmdDecoderStart(AMBA_DSP_CMD_VDSP_NORMAL_WRITE, StartCmd);
    } else {
        AmbaLL_LogUInt5("decode[%d] status[%d] Not READY", DecIdx, VidDecInfo.State, 0U, 0U, 0U);
    }
    if (Rval != OK) {
        AmbaLL_LogUInt5("[Err][0x%X] DecStart", Rval, 0U, 0U, 0U, 0U);
        Rval = DSP_ERR_0007;
    }
    HL_RelCmdBuffer(CmdBufferId);

    return Rval;
}

static UINT32 HL_VideoDecStopImpl(const UINT8 WriteMode, UINT16 DecIdx, const UINT8 *pShowLastFrame)
{
    UINT8 CmdBufferId = 0U; void *CmdBufferAddr = NULL;
    UINT32 Rval;
    cmd_decoder_stop_t *StopCmd = HL_DefCtxCmdBufPtrDecStop;
    CTX_VID_DEC_INFO_s VidDecInfo;

    HL_GetVidDecInfo(HL_MTX_OPT_ALL, DecIdx, &VidDecInfo);
    HL_AcqCmdBuffer(&CmdBufferId, &CmdBufferAddr);
    dsp_osal_typecast(&StopCmd, &CmdBufferAddr);
    StopCmd->hdr.decoder_id = 0U;
    (void)dsp_osal_memcpy(&StopCmd->hdr.decoder_id, &VidDecInfo.StreamID, sizeof(UINT8));

    // CV2FS only support AVC
    StopCmd->hdr.codec_type = DEC_CODEC_TYPE_AVC;

    if(pShowLastFrame[DecIdx] == 0U) {
        StopCmd->stop_mode = DEC_STOP_MODE_VOUT_STOP;
    } else if(pShowLastFrame[DecIdx] == 1U) {
        StopCmd->stop_mode = DEC_STOP_MODE_LAST_PIC;
    } else {
        AmbaLL_LogUInt5("wrong stop mode", 0U, 0U, 0U, 0U, 0U);
    }

    Rval =  AmbaHL_CmdDecoderStop(WriteMode, StopCmd);
    HL_RelCmdBuffer(CmdBufferId);
    if (Rval != OK) {
        Rval = DSP_ERR_0007;
        AmbaLL_LogUInt5("[Err][0x%X] Decode Stop", Rval, 0U, 0U, 0U, 0U);
    }

    return Rval;
}

static UINT32 HL_VideoDecConfigChk(UINT16 MaxNumStream, const AMBA_DSP_VIDDEC_STREAM_CONFIG_s *pStreamConfig)
{
    const Dsp_TimingApi_t* TimingApi = AmbaDSP_GetTimingApiFunc();
    const Dsp_DumpApi_t* DumpApi = AmbaDSP_GetDumpApiFunc();
    const Dsp_CheckApi_t* CheckApi = AmbaDSP_GetCheckApiFunc();
    UINT32 Rval;

    /* DSP CtxInit */
    Rval = HL_CtxInit();

    /* Timing sanity check */
    if ((Rval == OK) &&
        (TimingApi != NULL)) {
        Rval = TimingApi->pVideoDecConfig(MaxNumStream, pStreamConfig);
    }
    /* Input sanity check */
    if ((Rval == OK) &&
        (DumpApi != NULL)) {
        DumpApi->pVideoDecConfig(MaxNumStream, pStreamConfig);
    }
    if ((Rval == OK) &&
        (CheckApi != NULL)) {
        Rval = CheckApi->pVideoDecConfig(MaxNumStream, pStreamConfig);
    }

    /* Logic sanity check */
    return Rval;
}

/**
* Video decode config function
* @param [in]  MaxNumStream max decode stream index
* @param [in]  pStreamConfig stream configuration
* @return ErrorCode
*/
UINT32 dsp_video_dec_cfg(UINT16 MaxNumStream, const AMBA_DSP_VIDDEC_STREAM_CONFIG_s *pStreamConfig)
{
    UINT32 Rval;
    CTX_VID_DEC_INFO_s VidDecInfo;
    UINT16 i;

    Rval = HL_VideoDecConfigChk(MaxNumStream, pStreamConfig);

    /* Body */
    if (Rval == OK) {
        for (i = 0; i < MaxNumStream; i++) {
            HL_GetVidDecInfo(HL_MTX_OPT_GET, i, &VidDecInfo);
            VidDecInfo.StreamID = pStreamConfig[i].StreamID;
            VidDecInfo.BitsFormat = pStreamConfig[i].BitsFormat;
            VidDecInfo.BitsBufAddr = pStreamConfig[i].BitsBufAddr;
            VidDecInfo.BitsBufSize = pStreamConfig[i].BitsBufSize;
            VidDecInfo.MaxFrameWidth = pStreamConfig[i].MaxFrameWidth;
            VidDecInfo.MaxFrameHeight = pStreamConfig[i].MaxFrameHeight;
            VidDecInfo.XcodeMode = pStreamConfig[i].XcodeMode;
            VidDecInfo.XcodeWidth = pStreamConfig[i].XcodeWidth;
            VidDecInfo.XcodeHeight = pStreamConfig[i].XcodeHeight;
            VidDecInfo.MaxVideoBufferWidth = pStreamConfig[i].MaxVideoBufferWidth;
            VidDecInfo.MaxVideoBufferHeight = pStreamConfig[i].MaxVideoBufferHeight;
            (void)dsp_osal_memcpy(&VidDecInfo.FrameRate, &pStreamConfig[i].FrameRate, sizeof(AMBA_DSP_FRAME_RATE_s));
//FIXME, state reset when profile switch in VidDecStart
            VidDecInfo.State = VIDDEC_STATE_READY;
            HL_SetVidDecInfo(HL_MTX_OPT_SET, i, &VidDecInfo);
        }
    } else {
        AmbaLL_LogUInt5("[Err][0x%X] VideoDecConfig check", Rval, 0U, 0U, 0U, 0U);
    }

    return Rval;
}

static inline UINT32 HL_VidDecPreprocWaitPbMode(const CTX_VID_DEC_INFO_s *pVidDecInfo, UINT16 DecIdx)
{
    UINT32 Rval = OK;
    CTX_VID_DEC_INFO_s TmpVidDecInfo;

    /* 1. Re-start from stop 0 */
    /* 2. first decode from yuv boot */
    if ((pVidDecInfo->CurrentBind != BIND_VIDEO_DECODE_TO_VPROC) &&
        (pVidDecInfo->CurrentBind != BIND_VIDEO_DECODE_TEST)) {
        UINT8 CmdBufferId = 0U; void *CmdBufferAddr = NULL;
        cmd_binding_cfg_t *FpBindCfg = HL_DefCtxCmdBufPtrBind;

        /* UnBind first */
        HL_AcqCmdBuffer(&CmdBufferId, &CmdBufferAddr);
        dsp_osal_typecast(&FpBindCfg, &CmdBufferAddr);
        Rval = HL_FillFpBindSetup(FpBindCfg, pVidDecInfo->CurrentBind, 0U/*IsBind*/,
                                  pVidDecInfo->YuvInVirtVinId + AMBA_DSP_MAX_VIN_NUM/*SrcId -> src_fp_id*/,
                                  0U/*SrcPinId*/,
                                  pVidDecInfo->ViewZoneId/*StrmId -> dst_ch_id*/);
        if (Rval != OK) {
            AmbaLL_LogUInt5("[%d]FpBind filling fail", __LINE__, 0U, 0U, 0U, 0U);
        } else {
            (void)AmbaHL_CmdDspBindingCfg(AMBA_DSP_CMD_VCAP_NORMAL_WRITE, FpBindCfg);
        }
        HL_RelCmdBuffer(CmdBufferId);
        Rval = DSP_WaitVdspEvent((UINT32)EVENT_FLAG_INT_VDSP0_EVENT, 2, WAIT_INTERRUPT_TIMEOUT);
        if (Rval != OK) {
            AmbaLL_LogUInt5("VideoDecStop Wait VDSP0 fail %u %d", Rval, __LINE__, 0U, 0U, 0U);
        }

        /* Bind video decoder to Vproc */
        HL_AcqCmdBuffer(&CmdBufferId, &CmdBufferAddr);
        dsp_osal_typecast(&FpBindCfg, &CmdBufferAddr);
        Rval = HL_FillFpBindSetup(FpBindCfg, BIND_VIDEO_DECODE_TO_VPROC, 1U/*IsBind*/,
                                  DecIdx/*SrcId -> src_ch_id*/,
                                  0U/*SrcPinId -> xx*/,
                                  pVidDecInfo->ViewZoneId/*StrmId -> dst_ch_id*/);
        if (Rval != OK) {
            AmbaLL_LogUInt5("[%d]FpBind filling fail", __LINE__, 0U, 0U, 0U, 0U);
        } else {
            (void)AmbaHL_CmdDspBindingCfg(AMBA_DSP_CMD_VDSP_NORMAL_WRITE, FpBindCfg);
            HL_GetVidDecInfo(HL_MTX_OPT_GET, DecIdx, &TmpVidDecInfo);
            TmpVidDecInfo.CurrentBind = BIND_VIDEO_DECODE_TO_VPROC;
            HL_SetVidDecInfo(HL_MTX_OPT_SET, DecIdx, &TmpVidDecInfo);
        }
        HL_RelCmdBuffer(CmdBufferId);
    } else {
        //DO NOTHING
    }

    return Rval;
}

static UINT32 HL_VideoDecStartChk(UINT16 NumStream, const UINT16 *pStreamIdx, const AMBA_DSP_VIDDEC_START_CONFIG_s *pStartConfig)
{
    const Dsp_TimingApi_t* TimingApi = AmbaDSP_GetTimingApiFunc();
    const Dsp_DumpApi_t* DumpApi = AmbaDSP_GetDumpApiFunc();
    const Dsp_CheckApi_t* CheckApi = AmbaDSP_GetCheckApiFunc();
    const Dsp_ResourceApi_t* ResourceApi = AmbaDSP_GetResourceApiFunc();
    UINT32 Rval;

    /* DSP CtxInit */
    Rval = HL_CtxInit();

    /* Timing sanity check */
    if ((Rval == OK) &&
        (TimingApi != NULL)) {
        Rval = TimingApi->pVideoDecStart(NumStream, pStreamIdx, pStartConfig);
    }
    /* Input sanity check */
    if ((Rval == OK) &&
        (DumpApi != NULL)) {
        DumpApi->pVideoDecStart(NumStream, pStreamIdx, pStartConfig);
    }
    if ((Rval == OK) &&
        (CheckApi != NULL)) {
        Rval = CheckApi->pVideoDecStart(NumStream, pStreamIdx, pStartConfig);
    }
    /* HW/SW resources check */
    if ((Rval == OK) &&
        (ResourceApi != NULL)) {
        Rval = ResourceApi->pVideoDecStart(NumStream, pStreamIdx, pStartConfig);
    }

    /* Logic sanity check */
    return Rval;
}

/**
* Video decode start function
* @param [in]  NumStream number stream index
* @param [in]  pStreamIdx stream index
* @param [in]  pStartConfig start configuration
* @return ErrorCode
*/
UINT32 dsp_video_dec_start(UINT16 NumStream, const UINT16 *pStreamIdx, const AMBA_DSP_VIDDEC_START_CONFIG_s *pStartConfig)
{
    UINT32 Rval;
    UINT16 i;
    CTX_VID_DEC_INFO_s VidDecInfo = {0};
    UINT16 StrmId = 0U;

    Rval = HL_VideoDecStartChk(NumStream, pStreamIdx, pStartConfig);

    /* Body */
    if (Rval == OK) {
        for (i = 0; i < NumStream; i++) {
            UINT8 ShowLastFrm[AMBA_DSP_MAX_DEC_STREAM_NUM];

            HL_GetVidDecInfo(HL_MTX_OPT_ALL, pStreamIdx[i], &VidDecInfo);
            StrmId = VidDecInfo.StreamID;
            Rval = HL_VidDecPreprocWaitPbMode(&VidDecInfo, pStreamIdx[i]);

            // cv2fs still decode ucode pipline similar with video pipline
            // decode status will change to run after doing still decode
            // before video decode start decode status have to change to idle
            if (Rval == OK) {
                if (DSP_GetDecState(StrmId) == DSP_DEC_OPM_RUN) {
                    ShowLastFrm[StrmId] = (UINT8)DEC_STOP_MODE_VOUT_STOP;//DEC_STOP_MODE_LAST_PIC
                    Rval = HL_VideoDecStopImpl(AMBA_DSP_CMD_VDSP_NORMAL_WRITE, StrmId, ShowLastFrm);

                    if (Rval == OK) {
                        Rval = DSP_WaitDecState(StrmId, (UINT8)DEC_OPM_IDLE, MODE_SWITCH_TIMEOUT, 1U);
                        if (Rval != OK) {
                            AmbaLL_LogUInt5("[Err][%d] Wait DEC_IDLE fail[%d]", __LINE__, Rval, 0U, 0U, 0U);
                        }
                    }
                }
            } else {
                AmbaLL_LogUInt5("[Err][%d] Wait DEC_IDLE fail[%d]", __LINE__, Rval, 0U, 0U, 0U);
            }

            // DecodeSetup
            if (Rval == OK) {
                Rval = HL_VideoDecBitsStreamImpl(AMBA_DSP_CMD_VDSP_NORMAL_WRITE, StrmId);
            }

            if (Rval == OK) {
                // DecodeSpeedCmd
                Rval = HL_VideoDecSpeedImpl(StrmId, &pStartConfig[i]);

                // DecodeBatchCmd
                if (Rval == OK) {
                    Rval = HL_VideoDecBatchImpl(StrmId);
                } else {
                    AmbaLL_LogUInt5("[Err][%d] VideoDecStart DecodeSpeed fail[%d]", __LINE__, Rval, 0U, 0U, 0U);
                }

                // DecodeStartCmd
                if (Rval == OK) {
                    Rval = HL_VideoDecStartImpl(StrmId, &pStartConfig[i]);
                } else {
                    AmbaLL_LogUInt5("[Err][%d] VideoDecStart VideoDecBatch fail[%d]", __LINE__, Rval, 0U, 0U, 0U);
                }
            } else {
                AmbaLL_LogUInt5("[Err][%d] VideoDecStart DecodeSetup fail[%d]", __LINE__, Rval, 0U, 0U, 0U);
            }

            if (Rval == OK) {
                HL_GetVidDecInfo(HL_MTX_OPT_GET, StrmId, &VidDecInfo);
                VidDecInfo.State = VIDDEC_STATE_RUN;
                HL_SetVidDecInfo(HL_MTX_OPT_SET, StrmId, &VidDecInfo);
            } else {
                AmbaLL_LogUInt5("[Err][%d] VideoDecStart DecodeStart fail[%d]", __LINE__, Rval, 0U, 0U, 0U);
            }
        }
    }

    return Rval;
}

/**
* Video decode postproc control function, usually use for Zoom-in/Zoom-out
* @param [in]  StreamIdx stream index
* @param [in]  NumPostCtrl number of post control
* @param [in]  pPostCtrl post control information
* @return ErrorCode
*/
UINT32 dps_video_dec_post_ctrl(UINT16 StreamIdx, UINT16 NumPostCtrl, const AMBA_DSP_VIDDEC_POST_CTRL_s *pPostCtrl)
{
(void)StreamIdx;
(void)NumPostCtrl;
(void)pPostCtrl;

    return ERR_NA;
}

static UINT32 HL_VideoDecStopChk(UINT16 NumStream, const UINT16 *pStreamIdx, const UINT8 *pShowLastFrame)
{
    const Dsp_TimingApi_t* TimingApi = AmbaDSP_GetTimingApiFunc();
    const Dsp_DumpApi_t* DumpApi = AmbaDSP_GetDumpApiFunc();
    const Dsp_CheckApi_t* CheckApi = AmbaDSP_GetCheckApiFunc();
    const Dsp_ResourceApi_t* ResourceApi = AmbaDSP_GetResourceApiFunc();
    UINT32 Rval;

    /* DSP CtxInit */
    Rval = HL_CtxInit();

    /* Timing sanity check */
    if ((Rval == OK) &&
        (TimingApi != NULL)) {
        Rval = TimingApi->pVideoDecStop(NumStream, pStreamIdx, pShowLastFrame);
    }
    /* Input sanity check */
    if ((Rval == OK) &&
        (DumpApi != NULL)) {
        DumpApi->pVideoDecStop(NumStream, pStreamIdx, pShowLastFrame);
    }
    if ((Rval == OK) &&
        (CheckApi != NULL)) {
        Rval = CheckApi->pVideoDecStop(NumStream, pStreamIdx, pShowLastFrame);
    }
    /* HW/SW resources check */
    if ((Rval == OK) &&
        (ResourceApi != NULL)) {
        Rval = ResourceApi->pVideoDecStop(NumStream, pStreamIdx, pShowLastFrame);
    }

    /* Logic sanity check */

    return Rval;
}

/**
* Video decode stop function
* @param [in]  NumStream number stream index
* @param [in]  pStreamIdx stream index
* @param [in]  pShowLastFrame show last frame when decode stop
* @return ErrorCode
*/
UINT32 dsp_video_dec_stop(UINT16 NumStream, const UINT16 *pStreamIdx, const UINT8 *pShowLastFrame)
{
    UINT16 i;
    UINT32 Rval;
    CTX_VID_DEC_INFO_s VidDecInfo = {0};

    Rval = HL_VideoDecStopChk(NumStream, pStreamIdx, pShowLastFrame);

    /* Body */
    if (Rval == OK) {
        for (i = 0; i < NumStream; i++) {
            if (Rval == OK) {
                UINT8 ShowLastFrm[AMBA_DSP_MAX_DEC_STREAM_NUM];
#if 0 // Allways use DEC_STOP_MODE_LAST_PIC with dec2vproc
                /* Always use stop=1 with dec2vproc + effect */
                (void)dsp_osal_memset(&ViewZoneInfo, 0, sizeof(CTX_VIEWZONE_INFO_s));
                HL_GetViewZoneInfo(HL_MTX_OPT_ALL, VidDecInfo.ViewZoneId, &ViewZoneInfo);
                if (ViewZoneInfo.IsEffectChanMember == 1U) {
                    ShowLastFrm[0U] = DEC_STOP_MODE_LAST_PIC;
                } else {
                    ShowLastFrm[0U] = pShowLastFrame[CfgStreamIdx];
                }
#endif
                ShowLastFrm[pStreamIdx[i]] = DEC_STOP_MODE_LAST_PIC;
                Rval = HL_VideoDecStopImpl(AMBA_DSP_CMD_VDSP_NORMAL_WRITE, pStreamIdx[i], ShowLastFrm);
            } else {
                AmbaLL_LogUInt5("VideoDecStop VRSCL stop fail", 0U, 0U, 0U, 0U, 0U);
            }

            if (Rval == OK) {
                HL_GetVidDecInfo(HL_MTX_OPT_GET, pStreamIdx[i], &VidDecInfo);
                //FIXME, state transit to IDLE?
                VidDecInfo.State = VIDDEC_STATE_READY;
                HL_SetVidDecInfo(HL_MTX_OPT_SET, pStreamIdx[i], &VidDecInfo);
            } else {
                AmbaLL_LogUInt5("VideoDecStop decoder stop fail", 0U, 0U, 0U, 0U, 0U);
            }
        }


    } else {
        AmbaLL_LogUInt5("VideoDecStop check error", 0U, 0U, 0U, 0U, 0U);
    }

    return Rval;

}

static UINT32 HL_VideoDecTrickPlayChk(UINT16 NumStream, const UINT16 *pStreamIdx, const AMBA_DSP_VIDDEC_TRICKPLAY_s *pTrickPlay)
{
    const Dsp_TimingApi_t* TimingApi = AmbaDSP_GetTimingApiFunc();
    const Dsp_DumpApi_t* DumpApi = AmbaDSP_GetDumpApiFunc();
    const Dsp_CheckApi_t* CheckApi = AmbaDSP_GetCheckApiFunc();
    const Dsp_ResourceApi_t* ResourceApi = AmbaDSP_GetResourceApiFunc();
    UINT32 Rval;

    /* DSP CtxInit */
    Rval = HL_CtxInit();

    /* Timing sanity check */
    if ((Rval == OK) &&
        (TimingApi != NULL)) {
        Rval = TimingApi->pVideoDecTrickPlay(NumStream, pStreamIdx, pTrickPlay);
    }
    /* Input sanity check */
    if ((Rval == OK) &&
        (DumpApi != NULL)) {
        DumpApi->pVideoDecTrickPlay(NumStream, pStreamIdx, pTrickPlay);
    }
    if ((Rval == OK) &&
        (CheckApi != NULL)) {
        Rval = CheckApi->pVideoDecTrickPlay(NumStream, pStreamIdx, pTrickPlay);
    }
    /* HW/SW resources check */
    if ((Rval == OK) &&
        (ResourceApi != NULL)) {
        Rval = ResourceApi->pVideoDecTrickPlay(NumStream, pStreamIdx, pTrickPlay);
    }

    /* Logic sanity check */
    return Rval;
}

/**
* Video decode trickplay config function
* @param [in]  NumStream number stream index
* @param [in]  pStreamIdx stream index
* @param [in]  pTrickPlay trickplay configuration
* @return ErrorCode
*/
UINT32 dsp_video_dec_trickplay(UINT16 NumStream, const UINT16 *pStreamIdx, const AMBA_DSP_VIDDEC_TRICKPLAY_s *pTrickPlay)
{
    UINT16 i;
    UINT32 Rval;

    Rval = HL_VideoDecTrickPlayChk(NumStream, pStreamIdx, pTrickPlay);

    /* Body */
    if (Rval == OK) {
        for (i = 0; i < NumStream; i++) {
            UINT16 CfgStreamIdx = pStreamIdx[i];

            Rval = HL_VideoDecTrickImpl(CfgStreamIdx, pTrickPlay);
        }
    } else {
        AmbaLL_LogUInt5("TrickPlay check error", 0U, 0U, 0U, 0U, 0U);
    }

    return Rval;
}

static UINT32 HL_VideoDecBitsFifoUpdateChk(UINT16 NumStream, const UINT16 *pStreamIdx, const AMBA_DSP_VIDDEC_BITS_FIFO_s *pBitsFifo)
{
    const Dsp_TimingApi_t* TimingApi = AmbaDSP_GetTimingApiFunc();
    const Dsp_DumpApi_t* DumpApi = AmbaDSP_GetDumpApiFunc();
    const Dsp_CheckApi_t* CheckApi = AmbaDSP_GetCheckApiFunc();
    const Dsp_ResourceApi_t* ResourceApi = AmbaDSP_GetResourceApiFunc();
    UINT32 Rval;

    /* DSP CtxInit */
    Rval = HL_CtxInit();

    /* Timing sanity check */
    if ((Rval == OK) &&
        (TimingApi != NULL)) {
        Rval = TimingApi->pVideoDecBitsFifoUpdate(NumStream, pStreamIdx, pBitsFifo);
    }
    /* Input sanity check */
    if ((Rval == OK) &&
        (DumpApi != NULL)) {
        DumpApi->pVideoDecBitsFifoUpdate(NumStream, pStreamIdx, pBitsFifo);
    }
    if ((Rval == OK) &&
        (CheckApi != NULL)) {
        Rval = CheckApi->pVideoDecBitsFifoUpdate(NumStream, pStreamIdx, pBitsFifo);
        if (Rval != OK) {
            AmbaLL_LogUInt5("[Err][0x%X] BitsFifoUpdate check", Rval, 0U, 0U, 0U, 0U);
        }
    }
    /* HW/SW resources check */
    if ((Rval == OK) &&
        (ResourceApi != NULL)) {
        Rval = ResourceApi->pVideoDecBitsFifoUpdate(NumStream, pStreamIdx, pBitsFifo);
    }

    /* Logic sanity check */
    return Rval;
}

/**
* Video decode bit-stream fifo update function
* @param [in]  NumStream number stream index
* @param [in]  pStreamIdx stream index
* @param [in]  pBitsFifo bit-stream fifo information
* @return ErrorCode
*/
UINT32 dsp_video_dec_bitsfifo_update(UINT16 NumStream, const UINT16 *pStreamIdx, const AMBA_DSP_VIDDEC_BITS_FIFO_s *pBitsFifo)
{
    UINT16 i;
    UINT32 Rval;

    Rval = HL_VideoDecBitsFifoUpdateChk(NumStream, pStreamIdx, pBitsFifo);

    /* Body */
    if (Rval == OK) {
        for (i = 0; i < NumStream; i++) {
            UINT16 CfgStreamIdx = pStreamIdx[i];
            if (AMBA_DSP_MAX_DEC_STREAM_NUM <= CfgStreamIdx) {
                AmbaLL_LogUInt5("[BitsFifoUpdate] Invalid stream index", 0U, 0U, 0U, 0U, 0U);
                continue;
            }
            Rval = HL_VideoDecBitsUpdateImpl(AMBA_DSP_CMD_VDSP_NORMAL_WRITE, CfgStreamIdx, pBitsFifo);
            if (Rval != OK) {
                AmbaLL_LogUInt5("[Err][0x%X] BitsUpdateImpl", Rval, 0U, 0U, 0U, 0U);
                break;
            }
        }
    }

    return Rval;
}



