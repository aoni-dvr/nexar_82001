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
#include "AmbaDSP_EncodeAPI.h"
#include "AmbaDSP_EventCtrl.h"
#include "AmbaDSP_ApiResource.h"
#include "AmbaDSP_EncodeUtility.h"
#include "AmbaDSP_DecodeUtility.h"
#include "dsp_priv_api.h"

static inline UINT32 HL_VideoDecSystemSetupImpl(const UINT8 WriteMode)
{
    UINT32 Rval;
    UINT8 CmdBufferId = 0U; void *CmdBufferAddr = NULL;
    cmd_dsp_config_t *SysSetup = HL_DefCtxCmdBufPtrSys;
    cmd_dsp_hal_inf_t *SysHalSetup = HL_DefCtxCmdBufPtrHal;

    HL_AcqCmdBuffer(&CmdBufferId, &CmdBufferAddr);
    dsp_osal_typecast(&SysSetup, &CmdBufferAddr);
    Rval = HL_FillDecSystemSetup(SysSetup);
    if (Rval != OK) {
        AmbaLL_LogUInt5("[Err][0x%X][%d] VdecSysSetup", Rval, __LINE__, 0U, 0U, 0U);
    } else {
        Rval = AmbaHL_CmdDspConfig(WriteMode, SysSetup);
        if (Rval != OK) {
            AmbaLL_LogUInt5("[Err][0x%X][%d] VdecSysSetupCmd", Rval, __LINE__, 0U, 0U, 0U);
        }
    }
    HL_RelCmdBuffer(CmdBufferId);
    HL_AcqCmdBuffer(&CmdBufferId, &CmdBufferAddr);
    dsp_osal_typecast(&SysHalSetup, &CmdBufferAddr);
    if (Rval == OK) {
        Rval = HL_FillSystemHalSetup(SysHalSetup);
        if (Rval == OK) {
            Rval = AmbaHL_CmdDspHalInf(WriteMode, SysHalSetup);
            if (Rval != OK) {
                AmbaLL_LogUInt5("[Err][0x%X][%d] VdecSysSetupCmd", Rval, __LINE__, 0U, 0U, 0U);
            }
        } else {
            AmbaLL_LogUInt5("[Err][0x%X][%d] VdecSysSetup", Rval, __LINE__, 0U, 0U, 0U);
        }
    } else {
        AmbaLL_LogUInt5("[Err][0x%X][%d] VdecSysSetup", Rval, __LINE__, 0U, 0U, 0U);
    }
    HL_RelCmdBuffer(CmdBufferId);
    return Rval;
}

static inline UINT32 HL_VideoDecDspDebugLevelImpl(const UINT8 WriteMode)
{
    UINT8 CmdBufferId = 0U; void *CmdBufferAddr = NULL;
    UINT32 Rval = OK;
    cmd_set_debug_level_t *DebugLevel = HL_DefCtxCmdBufPtrDbgLvl;
    cmd_print_th_disable_mask_t *DebugThread = HL_DefCtxCmdBufPtrDbgthd;
    CTX_RESOURCE_INFO_s *Resource = HL_CtxResInfoPtr;

    HL_GetResourcePtr(&Resource);

    HL_AcqCmdBuffer(&CmdBufferId, &CmdBufferAddr);
    dsp_osal_typecast(&DebugLevel, &CmdBufferAddr);
    if ((Resource->DspDbgLvl == 0U) && (Resource->DspDbgMod == 0U) && (Resource->DspDbgThrd == 0U)) {
        DebugLevel->level = 3U;
        DebugLevel->module = 0xFFFFFFFFU;
        Rval = AmbaHL_CmdDspSetDebugLevel(WriteMode, DebugLevel);
    } else {
        DebugLevel->level = Resource->DspDbgLvl;
        DebugLevel->module = Resource->DspDbgMod;
        Rval = AmbaHL_CmdDspSetDebugLevel(WriteMode, DebugLevel);
    }
    HL_RelCmdBuffer(CmdBufferId);

    HL_AcqCmdBuffer(&CmdBufferId, &CmdBufferAddr);
    dsp_osal_typecast(&DebugThread, &CmdBufferAddr);
    if ((Resource->DspDbgThrd == 0U) && (Resource->DspDbgThrdValid == 0U)) {
        DebugThread->orccode_mask_valid = (UINT8)1U;
        DebugThread->orcme_mask_valid = (UINT8)1U;
        DebugThread->orcmdxf_mask_valid = (UINT8)1U;

        DebugThread->orccode_mask = (UINT16)0U;
        DebugThread->orcme_mask = (UINT8)0U;
        DebugThread->orcmdxf_mask = (UINT8)0U;
        Rval = AmbaHL_CmdDspPrintThreadDisableMask(WriteMode, DebugThread);
    } else {
        DebugThread->orccode_mask_valid = (UINT8)DSP_GetBit(Resource->DspDbgThrdValid, 0U, 1U);
        DebugThread->orcme_mask_valid = (UINT8)DSP_GetBit(Resource->DspDbgThrdValid, 1U, 1U);
        DebugThread->orcmdxf_mask_valid = (UINT8)DSP_GetBit(Resource->DspDbgThrdValid, 2U, 1U);

        DebugThread->orccode_mask = (UINT16)DSP_GetBit(Resource->DspDbgThrd, 0U, 12U);
        DebugThread->orcme_mask = (UINT8)DSP_GetBit(Resource->DspDbgThrd, 12U, 2U);
        DebugThread->orcmdxf_mask = (UINT8)DSP_GetBit(Resource->DspDbgThrd, 14U, 2U);
        Rval = AmbaHL_CmdDspPrintThreadDisableMask(WriteMode, DebugThread);
    }
    HL_RelCmdBuffer(CmdBufferId);

    return Rval;
}

static inline UINT32 HL_VideoDecFlowMaxCfgImpl(const UINT8 WriteMode)
{
    UINT8 CmdBufferId = 0U; void *CmdBufferAddr = NULL;
    UINT32 Rval;
    cmd_dsp_dec_flow_max_cfg_t *DecFlwMaxCfg = HL_DefCtxCmdBufPtrDecMax;

    HL_AcqCmdBuffer(&CmdBufferId, &CmdBufferAddr);
    dsp_osal_typecast(&DecFlwMaxCfg, &CmdBufferAddr);
    Rval = HL_FillDecFlowMaxCfgSetup(DecFlwMaxCfg);
    if (Rval != OK) {
        AmbaLL_LogUInt5("[Err][0x%X][%d] VdecFlowMax", Rval, __LINE__, 0U, 0U, 0U);
    } else {
        Rval = AmbaHL_CmdDspDecFlowMaxCfg(WriteMode, DecFlwMaxCfg);
        if (Rval != OK) {
            AmbaLL_LogUInt5("[Err][0x%X][%d] VdecFlowMaxCmd", Rval, __LINE__, 0U, 0U, 0U);
        }
    }
    HL_RelCmdBuffer(CmdBufferId);

    return Rval;
}

static inline UINT32 HL_VideoDecFpBindingImpl(const UINT8 WriteMode)
{
    UINT8 CmdBufferId = 0U; void *CmdBufferAddr = NULL;
    UINT32 Rval;
    cmd_binding_cfg_t *FpBindCfg = HL_DefCtxCmdBufPtrBind;

    HL_AcqCmdBuffer(&CmdBufferId, &CmdBufferAddr);
    dsp_osal_typecast(&FpBindCfg, &CmdBufferAddr);
    Rval = HL_FillFpBindSetup(FpBindCfg,
                              BIND_VIDEO_DECODE,
                              1U/*IsBind*/,
                              0/*ViewZoneId*/,
                              0U/*SrcPinId*/,
                              0U/*StrmId*/);
    if (Rval != OK) {
        AmbaLL_LogUInt5("[Err][0x%X][%d] VdecBind", 0U, 0U, 0U, 0U, 0U);
    } else {
        Rval = AmbaHL_CmdDspBindingCfg(WriteMode, FpBindCfg);
        if (Rval != OK) {
            AmbaLL_LogUInt5("[Err][0x%X][%d] VdecBindCmd", Rval, __LINE__, 0U, 0U, 0U);
        }
    }
    HL_RelCmdBuffer(CmdBufferId);

    return Rval;
}

static inline UINT32 HL_VideoDecSystemActivateImpl(const UINT8 WriteMode)
{
    UINT8 CmdBufferId = 0U; void *CmdBufferAddr = NULL;
    UINT32 Rval;
    cmd_dsp_activate_profile_t *ActProfile = HL_DefCtxCmdBufPtrActPrf;

    HL_AcqCmdBuffer(&CmdBufferId, &CmdBufferAddr);
    dsp_osal_typecast(&ActProfile, &CmdBufferAddr);
    ActProfile->dsp_prof_id = DSP_PROF_STATUS_PLAYBACK;
    Rval = AmbaHL_CmdDspActivateProfile(WriteMode, ActProfile);
    HL_RelCmdBuffer(CmdBufferId);

    return Rval;
}

static inline UINT32 HL_VdoDecVoutSetupOnVoutReset(const UINT8 WriteMode,
                                                   const UINT8 *pResetVout,
                                                   const CTX_VOUT_INFO_s *pVoutInfo)
{
    UINT32 Rval = OK;
#if defined (CONFIG_SOC_CV28)
    UINT16 i = 0U;
#else
    UINT16 i;
#endif

    /* CV28 has only one vout */
#if defined (CONFIG_SOC_CV28)
    AmbaLL_LogUInt5("VoutCmd[%d] ResetVout[%d] VideoEnable[%d]", i, pResetVout[i], pVoutInfo[i].VideoEnable, 0U, 0U);
    if ((pResetVout[i] > 0U) && (pVoutInfo[i].VideoEnable == 1U)) {   //hack orig => VoutInfo[i].VideoEnable == 1U
        Rval = HL_VoutResetSetup((UINT8)i, WriteMode);
        if (Rval != OK) {
            AmbaLL_LogUInt5("[Err][0x%X][%d] DecVoutResetSetup", Rval, __LINE__, 0U, 0U, 0U);
        }

        if (Rval == OK) {
            Rval = HL_VoutDisplaySetup((UINT8)i, WriteMode);
        } else {
            AmbaLL_LogUInt5("[Err][0x%X][%d] DecVoutDispSetup", Rval, __LINE__, 0U, 0U, 0U);
        }

        if (Rval == OK) {
            Rval = HL_VoutGammaSetup((UINT8)i, WriteMode);
        } else {
            AmbaLL_LogUInt5("[Err][0x%X][%d] DecVoutGammaSetup", Rval, __LINE__, 0U, 0U, 0U);
        }

        if (Rval == OK) {
            Rval = HL_VoutDveSetup((UINT8)i, WriteMode);
        } else {
            AmbaLL_LogUInt5("[Err][0x%X][%d] DecVoutDveSetup", Rval, __LINE__, 0U, 0U, 0U);
        }

        if (Rval == OK) {
            Rval = HL_VoutMixerSetup((UINT8)i, WriteMode);
        } else {
            AmbaLL_LogUInt5("[Err][0x%X][%d] DecVoutMixerSetup", Rval, __LINE__, 0U, 0U, 0U);
        }
    }
#else
    for (i = 0U; i<AMBA_DSP_MAX_VOUT_NUM; i++) {
        AmbaLL_LogUInt5("VoutCmd[%d] ResetVout[%d] VideoEnable[%d]", i, pResetVout[i], pVoutInfo[i].VideoEnable, 0U, 0U);
        if ((pResetVout[i] > 0U) && (pVoutInfo[i].VideoEnable == 1U)) {   //hack orig => VoutInfo[i].VideoEnable == 1U
            Rval = HL_VoutResetSetup((UINT8)i, WriteMode);
            if (Rval != OK) {
                AmbaLL_LogUInt5("[Err][0x%X][%d] DecVoutResetSetup", Rval, __LINE__, 0U, 0U, 0U);
            }

            if (Rval == OK) {
                Rval = HL_VoutDisplaySetup((UINT8)i, WriteMode);
            } else {
                AmbaLL_LogUInt5("[Err][0x%X][%d] DecVoutDispSetup", Rval, __LINE__, 0U, 0U, 0U);
            }

            if (Rval == OK) {
                if (i == VOUT_IDX_A) {
                    Rval = HL_VoutGammaSetup((UINT8)i, WriteMode);
                }
            } else {
                AmbaLL_LogUInt5("[Err][0x%X][%d] DecVoutGammaSetup", Rval, __LINE__, 0U, 0U, 0U);
            }

            if (Rval == OK) {
                if (i == VOUT_IDX_B) {
                    Rval = HL_VoutDveSetup((UINT8)i, WriteMode);
                }
            } else {
                AmbaLL_LogUInt5("[Err][0x%X][%d] DecVoutDveSetup", Rval, __LINE__, 0U, 0U, 0U);
            }

            if (Rval == OK) {
                Rval = HL_VoutMixerSetup((UINT8)i, WriteMode);
            } else {
                AmbaLL_LogUInt5("[Err][0x%X][%d] DecVoutMixerSetup", Rval, __LINE__, 0U, 0U, 0U);
            }
        }
    }
#endif

    return Rval;
}

static inline UINT32 HL_VdoDecVoutSetupOnVideoOsdSetup(const UINT8 WriteMode,
                                                       UINT8 VoutIdx,
                                                       UINT8 EnablePrev,
                                                       const CTX_VOUT_INFO_s *pVoutInfo)
{
    UINT32 Rval = OK;
    UINT8 VoutSrc;

    if ((EnablePrev == 0U) && (pVoutInfo->VideoEnable == 1U)) {
        if (pVoutInfo->VideoCfg.VideoSource == VOUT_SOURCE_INTERNAL) {
            VoutSrc = DSP_VOUT_SRC_POSTP;
        } else {
            VoutSrc = pVoutInfo->VideoCfg.VideoSource;
        }
        (void)HL_VoutVideoSetupSourceSelect(VoutIdx, VoutSrc, 0U/*DONT CARE*/);
        Rval = HL_VoutVideoSetup(VoutIdx, WriteMode);
    } else if (EnablePrev == 1U) {
        CTX_VID_DEC_INFO_s VidDecInfo = {0};
        CTX_VOUT_INFO_s VoutInfo = {0};

        Rval = HL_VoutVideoSetupSourceSelect(VoutIdx, DSP_VOUT_SRC_POSTP, HL_VOUT_SOURCE_VIDEO_CAP);
        if (Rval != OK) {
            AmbaLL_LogUInt5("[Err][0x%X][%d] DecVoutSourceSetup[%d]", Rval, __LINE__, VoutIdx, 0U, 0U);
        }

        //check postP w/h before calling HL_VoutVideoSetup
        HL_GetVidDecInfo(HL_MTX_OPT_ALL, 0U, &VidDecInfo);
        if (VidDecInfo.PostCtlCfg[VoutIdx].VoutWindow.Width != 0U) {
            HL_GetVoutInfo(HL_MTX_OPT_GET, VoutIdx, &VoutInfo);
            VoutInfo.VideoCfg.RotateFlip = VidDecInfo.PostCtlCfg[VoutIdx].TargetRotateFlip;
            VoutInfo.VideoCfg.Window = VidDecInfo.PostCtlCfg[VoutIdx].VoutWindow;
            HL_SetVoutInfo(HL_MTX_OPT_SET, VoutIdx, &VoutInfo);
        }

        if (pVoutInfo->OsdEnable == 1U) {
            Rval = HL_VoutOsdSetup(VoutIdx, WriteMode);
            if (Rval != OK) {
                AmbaLL_LogUInt5("[Err][0x%X][%d] DecVoutOsdSetup[%d]", Rval, __LINE__, VoutIdx, 0U, 0U);
            }
        } else {
            // DO NOTHING
        }
        Rval = HL_VoutVideoSetup(VoutIdx, WriteMode);
        if (Rval != OK) {
            AmbaLL_LogUInt5("[Err][0x%X][%d] DecVoutVideoSetup[%d]", Rval, __LINE__, VoutIdx, 0U, 0U);
        }
    } else {
        // DO NOTHING
    }

    return Rval;
}


static UINT32 HL_VideoDecVoutSetupImpl(const UINT8 WriteMode)
{
    UINT32 Rval;
    UINT16 i;
    UINT8 EnablePrev[AMBA_DSP_MAX_VOUT_NUM];
    UINT8 ResetVout[AMBA_DSP_MAX_VOUT_NUM];
    CTX_VOUT_INFO_s VoutInfo[AMBA_DSP_MAX_VOUT_NUM];
    UINT8 DspOpModeInit, DspOpModeIdle;

    for (i = 0U; i < AMBA_DSP_MAX_VOUT_NUM; i++) {
        EnablePrev[i] = 0U;
        ResetVout[i] = 0U;
        HL_GetVoutInfo(HL_MTX_OPT_ALL, (UINT16)i, &VoutInfo[i]);

        if (1U == VoutInfo[i].VideoEnable) {
            EnablePrev[i] |= 1U;
        }
    }


    AmbaLL_LogUInt5("VoutCmd ProfStat[%d]",
                          DSP_GetProfState(), 0U, 0U, 0U, 0U);
    for (i = 0U; i < AMBA_DSP_MAX_VOUT_NUM; i++) {
        AmbaLL_LogUInt5("        Vout[%d] PrevEn[%d] VdoEn[%d] OsdEn[%d] Csc[%d]", i,
                         EnablePrev[i], VoutInfo[i].VideoEnable,
                         VoutInfo[i].OsdEnable, VoutInfo[i].CscUpdated);
    }


    DspOpModeInit = (DSP_GetProfState() == DSP_PROF_STATUS_INVALID)? 1U: 0U;
    DspOpModeIdle = (DSP_GetProfState() == DSP_PROF_STATUS_IDLE)? 1U: 0U;

    //Check reset vout?
    if ((DspOpModeInit == 1U) || (DspOpModeIdle == 1U)) { //booting
        for (i = 0U; i < AMBA_DSP_MAX_VOUT_NUM; i++) {
            if (EnablePrev[i] == 1U) {
                ResetVout[i] = 1U;
            }
        }
    } else { //DSP mode switch, or change resolution
        for (i = 0U; i < AMBA_DSP_MAX_VOUT_NUM; i++) {
            if ((EnablePrev[i] == 1U) && (VoutInfo[i].VideoEnable == 0U)) {
                ResetVout[i] = 1U;
            }
        }
    }

    // VoutReset
    Rval = HL_VdoDecVoutSetupOnVoutReset(WriteMode, &ResetVout[0U], &VoutInfo[0U]);

    // VoutVideoSetup and OSDSetup
    for (i = 0U; i < AMBA_DSP_MAX_VOUT_NUM; i++) {
        if (Rval == OK) {
            Rval = HL_VdoDecVoutSetupOnVideoOsdSetup(WriteMode, (UINT8)i, EnablePrev[i], &VoutInfo[i]);
        } else {
            AmbaLL_LogUInt5("[Err][0x%X][%d] DecVoutSetup[%d]", Rval, __LINE__, i, 0U, 0U);
        }
    }

    return Rval;
}

static UINT32 HL_VideoDecVrsclSetupImpl(const UINT8 WriteMode, const AMBA_DSP_VIDDEC_POST_CTRL_s *pPostCtrl) {
    UINT8 CmdBufferId = 0U; void *CmdBufferAddr = NULL;
    UINT32 Rval;
    cmd_dproc_vrscl_layout_t *VrsclLayout = HL_DefCtxCmdBufPtrDpcLayout;
    cmd_dproc_vrscl_vid_win_t *VrsclWin = HL_DefCtxCmdBufPtrDpcWin;

    AmbaLL_LogUInt5("HL_VideoDecVrsclSetupImpl VoutIdx:%u ", pPostCtrl->VoutIdx, 0U, 0U, 0U, 0U);
    HL_AcqCmdBuffer(&CmdBufferId, &CmdBufferAddr);
    dsp_osal_typecast(&VrsclLayout, &CmdBufferAddr);
    Rval = HL_FillDecVrsclLayoutSetup(pPostCtrl, VrsclLayout);
    if (Rval == OK) {
        Rval = AmbaHL_CmdDecVrsclLayout(WriteMode, VrsclLayout);
    } else {
        AmbaLL_LogUInt5("VideoDecVrscl window Setup VoutIdx:%d error:%d", pPostCtrl->VoutIdx, Rval, 0U, 0U, 0U);
    }
    HL_RelCmdBuffer(CmdBufferId);

    HL_AcqCmdBuffer(&CmdBufferId, &CmdBufferAddr);
    dsp_osal_typecast(&VrsclWin, &CmdBufferAddr);
    if (Rval == OK) {
        Rval = HL_FillDecVrsclWindowSetup(pPostCtrl, VrsclWin);
    } else {
        AmbaLL_LogUInt5("VoutVrscl error:%u %u", Rval, __LINE__, 0U, 0U, 0U);
    }

    if (Rval == OK) {
        Rval = AmbaHL_CmdDecVrsclVidWindow(WriteMode, VrsclWin);
    } else {
        AmbaLL_LogUInt5("VideoDecVrscl layout cmd VoutIdx:%d error:%d", pPostCtrl->VoutIdx, Rval, 0U, 0U, 0U);
    }
    HL_RelCmdBuffer(CmdBufferId);

    return Rval;
}

static UINT32 HL_VideoDecPostCtrlImpl(const UINT8 WriteMode, UINT16 StreamIdx)
{
    UINT8 CmdBufferId = 0U; void *CmdBufferAddr = NULL;
    UINT32 Rval = OK;
    UINT16 PostCtrlNum = 0U;
    cmd_postproc_t *PostProcCfg = HL_DefCtxCmdBufPtrPostP;
    CTX_VID_DEC_INFO_s VidDecInfo = {0};
    AMBA_DSP_VIDDEC_POST_CTRL_s PostCtrlCfg[AMBA_DSP_MAX_VOUT_NUM] = {0};
    UINT8 VoutIdx;
    UINT8 IsY2Disp = (UINT8)0U;

    HL_GetVidDecInfo(HL_MTX_OPT_ALL, StreamIdx, &VidDecInfo);

    for (VoutIdx = 0; VoutIdx < AMBA_DSP_MAX_VOUT_NUM; VoutIdx++) {
        if (VidDecInfo.PostCtlCfg[VoutIdx].VoutWindow.Width != 0U) {
            PostCtrlCfg[PostCtrlNum] = VidDecInfo.PostCtlCfg[VoutIdx];
            PostCtrlNum++;
        }
    }

    if (VidDecInfo.XcodeMode != AMBA_DSP_XCODE_NON_THROTTLE) {
        HL_AcqCmdBuffer(&CmdBufferId, &CmdBufferAddr);
        dsp_osal_typecast(&PostProcCfg, &CmdBufferAddr);
        PostProcCfg->decode_id = (UINT8)StreamIdx;
        Rval = HL_FillDecPostCtrlSetup(PostCtrlNum, PostCtrlCfg, PostProcCfg, IsY2Disp);
        if (Rval != OK) {
            AmbaLL_LogUInt5("Postproc filling fail", 0U, 0U, 0U, 0U, 0U);
        } else {
            Rval = AmbaHL_CmdPostproc(WriteMode, PostProcCfg);
        }
        HL_RelCmdBuffer(CmdBufferId);
    }

    if (Rval == OK) {
        if (VidDecInfo.XcodeMode == (UINT8)AMBA_DSP_XCODE_NONE) {
            // normal decode case
            for (VoutIdx = 0; VoutIdx < AMBA_DSP_MAX_VOUT_NUM; VoutIdx++) {
                if (VidDecInfo.PostCtlCfg[VoutIdx].VoutWindow.Width != 0U) {
                    Rval = HL_VideoDecVrsclSetupImpl(WriteMode, &VidDecInfo.PostCtlCfg[VoutIdx]);
                    if (Rval != OK) { Rval = DSP_ERR_0007;}
                }
            }
        }
    }

    return Rval;
}

static UINT32 HL_VideoDecFillPreProcGroupCmds(const UINT8 WriteMode)
{
    UINT32 Rval;

    AmbaLL_LogUInt5("Setup decode DefCmdBuf", 0U, 0U, 0U, 0U, 0U);

    /* SystemSetup */
    Rval = HL_VideoDecSystemSetupImpl(WriteMode);
    if (Rval != OK) {
        AmbaLL_LogUInt5("[Err][0x%X][%d] VdecPreProc", Rval, __LINE__, 0U, 0U, 0U);
    }

    /* Debug Level */
    if (Rval == OK) {
        Rval = HL_VideoDecDspDebugLevelImpl(WriteMode);
        if (Rval != OK) {
            AmbaLL_LogUInt5("[Err][0x%X][%d] VdecPreProc", Rval, __LINE__, 0U, 0U, 0U);
        }
    }

    /* Decode Flow Max Config */
    if (Rval == OK) {
        Rval = HL_VideoDecFlowMaxCfgImpl(WriteMode);
        if (Rval != OK) {
            AmbaLL_LogUInt5("[Err][0x%X][%d] VdecPreProc", Rval, __LINE__, 0U, 0U, 0U);
        }
    }

    /* FlowProcessor Binding */
    if (Rval == OK) {
        Rval = HL_VideoDecFpBindingImpl(WriteMode);
        if (Rval != OK) {
            AmbaLL_LogUInt5("[Err][0x%X][%d] VdecPreProc", Rval, __LINE__, 0U, 0U, 0U);
        }
    }

    /* Profile Activate */
    if (Rval == OK) {
        Rval = HL_VideoDecSystemActivateImpl(WriteMode);
        if (Rval != OK) {
            AmbaLL_LogUInt5("[Err][0x%X][%d] VdecPreProc", Rval, __LINE__, 0U, 0U, 0U);
        }
    }

    /* Vout relative */
    if (Rval == OK) {
        Rval = HL_VideoDecVoutSetupImpl(WriteMode);
        if (Rval != OK) {
            AmbaLL_LogUInt5("[Err][0x%X][%d] VdecPreProc", Rval, __LINE__, 0U, 0U, 0U);
        }
    }

    /* Video Decode Post Ctrl */
    if (Rval == OK) {
        Rval = HL_VideoDecPostCtrlImpl(WriteMode, 0U);
        if (Rval != OK) {
            AmbaLL_LogUInt5("[Err][0x%X][%d] VdecPreProc", Rval, __LINE__, 0U, 0U, 0U);
        }
    }

    return Rval;
}

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

/* Used when dsp is NOT BOOTED */
static UINT32 HL_VideoDecBootSetup(UINT8 TargetProfile)
{
    UINT32 Rval;
    UINT8 DspOpModeInit, DspOpModeIdle;

    DspOpModeInit = (DSP_GetProfState() == DSP_PROF_STATUS_INVALID)? 1U: 0U;
    DspOpModeIdle = (DSP_GetProfState() == DSP_PROF_STATUS_IDLE)? 1U: 0U;

    if ((TargetProfile == DSP_PROF_STATUS_PLAYBACK) &&
        ((DspOpModeInit == 1U) || (DspOpModeIdle == 1U))) {
        DSP_ClrDefCfgBuffer();
        Rval = HL_VideoDecFillPreProcGroupCmds(AMBA_DSP_CMD_DEFAULT_WRITE);
    } else if (TargetProfile == DSP_PROF_STATUS_XCODE) {
//FIXME, xCode.
        //Rval = Switch2Transcode(DSP_OP_MODE_TRANSCODE);
        Rval = DSP_ERR_0001;
    } else {
        // unknown case
        AmbaLL_LogUInt5("DecBootSetup unknown status", 0U, 0U, 0U, 0U, 0U);
        Rval = DSP_ERR_0001;
    }

    if (Rval != OK) {
        AmbaLL_LogUInt5("[Err][0x%X] DecBootSetup[%d] fail", Rval, TargetProfile, 0U, 0U, 0U);
    }

    return Rval;
}

static UINT32 HL_VideoDecVrsclStartImpl(UINT16 CfgStreamIdx, const AMBA_DSP_VIDDEC_START_CONFIG_s *pStartCfg)
{
    UINT8 CmdBufferId = 0U; void *CmdBufferAddr = NULL;
    UINT32 Rval = OK;
    UINT32 ActualVoutBit = 0U;
    CTX_VOUT_INFO_s VoutInfo = {0};
    CTX_VID_DEC_INFO_s VidDecInfo = {0};
    cmd_dproc_vrscl_start_t *VrsclStart = HL_DefCtxCmdBufPtrDpcStart;

    if (HL_IsDec2Vproc() == 1U) {
        //DO NOTHING
    } else {
        //TODO: Add check 8 times zoom-in/out limitation in CheckApi
        AmbaLL_LogUInt5("VRSCL Command Start!!", 0U, 0U, 0U, 0U, 0U);

        HL_GetVidDecInfo(HL_MTX_OPT_ALL, CfgStreamIdx, &VidDecInfo);
        if (VidDecInfo.XcodeMode == AMBA_DSP_XCODE_NONE) {
            //check if AMBA_DSP_MAX_VOUT_NUM = 1
            ActualVoutBit = HL_GetDecVoutTotalBit();

            //Vout0
            if (1U == DSP_GetBit(ActualVoutBit, VOUT_IDX_A, 1U)) {
                HL_GetVoutInfo(HL_MTX_OPT_ALL, VOUT_IDX_A, &VoutInfo);
                if(VoutInfo.VideoEnable != 0U) {
                    HL_AcqCmdBuffer(&CmdBufferId, &CmdBufferAddr);
                    dsp_osal_typecast(&VrsclStart, &CmdBufferAddr);
                    VrsclStart->stream_id = 1U;
                    (void)dsp_osal_memcpy(&VrsclStart->direction, &pStartCfg->Direction, sizeof(UINT8));
                    VrsclStart->pts64 = pStartCfg->FirstDisplayPTS;
                    Rval = AmbaHL_CmdDecVrsclStart(AMBA_DSP_CMD_VDSP_NORMAL_WRITE, VrsclStart);
                    if (Rval != OK) {
                        AmbaLL_LogUInt5("[Err][0x%X] Vout0 VRSCL start", Rval, 0U, 0U, 0U, 0U);
                        Rval = DSP_ERR_0007;
                    }
                    HL_RelCmdBuffer(CmdBufferId);
                }
            }
            //Vout1
            if (1U == DSP_GetBit(ActualVoutBit, VOUT_IDX_B, 1U)) {
                HL_GetVoutInfo(HL_MTX_OPT_ALL, VOUT_IDX_B, &VoutInfo);
                if(VoutInfo.VideoEnable != (UINT8)0U) {
                    HL_AcqCmdBuffer(&CmdBufferId, &CmdBufferAddr);
                    dsp_osal_typecast(&VrsclStart, &CmdBufferAddr);
                    VrsclStart->stream_id = 0U;
                    (void)dsp_osal_memcpy(&VrsclStart->direction, &pStartCfg->Direction, sizeof(UINT8));
                    VrsclStart->pts64 = pStartCfg->FirstDisplayPTS;
                    Rval = AmbaHL_CmdDecVrsclStart(AMBA_DSP_CMD_VDSP_NORMAL_WRITE, VrsclStart);
                    if (Rval != OK) {
                        AmbaLL_LogUInt5("[Err][0x%X] Vout1 VRSCL start", Rval, 0U, 0U, 0U, 0U);
                        Rval = DSP_ERR_0007;
                    }
                    HL_RelCmdBuffer(CmdBufferId);
                }
            }

        } else {
            AmbaLL_LogUInt5("VRSCL haven't supported Xcode yet", 0U, 0U, 0U, 0U, 0U);
        }
    }

    if (Rval != OK) {
        AmbaLL_LogUInt5("[Err][0x%X] VrsclStart", Rval, 0U, 0U, 0U, 0U);
    }

    return Rval;
}

static UINT32 HL_VideoDecVrsclStopImpl(const UINT8 WriteMode, UINT16 StreamIdx)
{
    UINT8 CmdBufferId = 0U; void *CmdBufferAddr = NULL;
    UINT32 Rval = OK;
    UINT32 ActualVoutBit = 0U;
    CTX_VOUT_INFO_s VoutInfo = {0};
    CTX_VID_DEC_INFO_s VidDecInfo;
    cmd_dproc_vrscl_stop_t *VrsclStopCmd = HL_DefCtxCmdBufPtrDpcStop;

    HL_GetVidDecInfo(HL_MTX_OPT_ALL, StreamIdx, &VidDecInfo);

    if (VidDecInfo.XcodeMode == AMBA_DSP_XCODE_NONE) {
        //check if AMBA_DSP_MAX_VOUT_NUM = 1
        ActualVoutBit = HL_GetDecVoutTotalBit();

        //Vout0
        if (1U == DSP_GetBit(ActualVoutBit, VOUT_IDX_A, 1U)) {
            HL_GetVoutInfo(HL_MTX_OPT_ALL, VOUT_IDX_A, &VoutInfo);
            if (VoutInfo.VideoEnable != 0U) {
                HL_AcqCmdBuffer(&CmdBufferId, &CmdBufferAddr);
                dsp_osal_typecast(&VrsclStopCmd, &CmdBufferAddr);
                VrsclStopCmd->stream_id = 1U;
                VrsclStopCmd->stop_mode = DPROC_STOP_MODE_NORMAL;
                Rval = AmbaHL_CmdDecVrsclStop(WriteMode, VrsclStopCmd);
                if (Rval != OK) {
                    Rval = DSP_ERR_0007;
                    AmbaLL_LogUInt5("[Err][0x%X] Vout0 VRSCL stop", Rval, 0U, 0U, 0U, 0U);
                }
                HL_RelCmdBuffer(CmdBufferId);
            }
        }

        //Vout1
        if (1U == DSP_GetBit(ActualVoutBit, VOUT_IDX_B, 1U)) {
            HL_GetVoutInfo(HL_MTX_OPT_ALL, VOUT_IDX_B, &VoutInfo);
            if (VoutInfo.VideoEnable != 0U) {
                HL_AcqCmdBuffer(&CmdBufferId, &CmdBufferAddr);
                dsp_osal_typecast(&VrsclStopCmd, &CmdBufferAddr);
                VrsclStopCmd->stream_id = 0U;
                VrsclStopCmd->stop_mode = DPROC_STOP_MODE_NORMAL;
                Rval = AmbaHL_CmdDecVrsclStop(WriteMode, VrsclStopCmd);
                if (Rval != OK) {
                    Rval = DSP_ERR_0007;
                    AmbaLL_LogUInt5("[Err][0x%X] Vout1 VRSCL stop", Rval, 0U, 0U, 0U, 0U);
                }
                HL_RelCmdBuffer(CmdBufferId);
            }
        }
    }

    if (Rval != OK) {
        AmbaLL_LogUInt5("[Err][0x%X] VrsclStop", Rval, 0U, 0U, 0U, 0U);
    }

    return Rval;
}

static UINT32 HL_VideoDecSpeedImpl(UINT16 ConfigIdx, const AMBA_DSP_VIDDEC_START_CONFIG_s *pStartConfig)
{
    UINT8 CmdBufferId = 0U; void *CmdBufferAddr = NULL;
    UINT32 Rval;
    cmd_decoder_speed_t *SpdDirCmd = HL_DefCtxCmdBufPtrDecSpeed;

    HL_AcqCmdBuffer(&CmdBufferId, &CmdBufferAddr);
    dsp_osal_typecast(&SpdDirCmd, &CmdBufferAddr);
    Rval = HL_FillDecSpeedSetup(ConfigIdx, pStartConfig, SpdDirCmd);
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

static UINT32 HL_VideoDecBatchImpl(UINT16 CfgStreamIdx)
{
    UINT8 CmdBufferId = 0U; void *CmdBufferAddr = NULL;
    UINT32 Rval = OK;
    CTX_VID_DEC_INFO_s VidDecInfo;
    CTX_VIEWZONE_INFO_s *ViewZoneInfo = HL_CtxViewZoneInfoPtr;
    cmd_decoder_batch_t *DecBchInfo = HL_DefCtxCmdBufPtrDecBch;

    HL_GetVidDecInfo(HL_MTX_OPT_GET, CfgStreamIdx, &VidDecInfo);

    /* Valid viewzone means Dec2Vproc case */
    if (VidDecInfo.ViewZoneId != DSP_VPROC_IDX_INVALID) {
        HL_GetViewZoneInfoLock(VidDecInfo.ViewZoneId, &ViewZoneInfo);

        if ((ViewZoneInfo->StartIsoCfgAddr != 0U) &&
            (VidDecInfo.FirstIsoCfgIssued == (UINT8)0U)) {
            UINT32 *pBatchQAddr = NULL, BatchCmdId = 0U, NewWp = 0U;
            process_idsp_iso_config_cmd_t *pIsoCfgCmd = NULL;
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
                (void)dsp_osal_virt2cli(ViewZoneInfo->StartIsoCfgAddr, &pIsoCfgCmd->iso_cfg_daddr);
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
        } else {
            HL_GetViewZoneInfoUnLock(VidDecInfo.ViewZoneId);
        }
    }
    HL_SetVidDecInfo(HL_MTX_OPT_SET, CfgStreamIdx, &VidDecInfo);

    return Rval;
}

static UINT32 HL_VideoDecStartImpl(UINT16 CfgStreamIdx, const AMBA_DSP_VIDDEC_START_CONFIG_s *pStartConfig)
{
    UINT8 CmdBufferId = 0U; void *CmdBufferAddr = NULL;
    UINT32 Rval;
    CTX_VID_DEC_INFO_s VidDecInfo;
    cmd_decoder_start_t *StartCmd = HL_DefCtxCmdBufPtrDecStart;

    HL_GetVidDecInfo(HL_MTX_OPT_ALL, CfgStreamIdx, &VidDecInfo);
    HL_AcqCmdBuffer(&CmdBufferId, &CmdBufferAddr);
    dsp_osal_typecast(&StartCmd, &CmdBufferAddr);
    Rval = HL_FillDecStartSetup(CfgStreamIdx, pStartConfig, StartCmd);
    if (VidDecInfo.State == VIDDEC_STATE_READY) {
        Rval = AmbaHL_CmdDecoderStart(AMBA_DSP_CMD_VDSP_NORMAL_WRITE, StartCmd);
    } else {
        AmbaLL_LogUInt5("decode[%d] status[%d] Not READY", CfgStreamIdx, VidDecInfo.State, 0U, 0U, 0U);
    }
    if (Rval != OK) {
        AmbaLL_LogUInt5("[Err][0x%X] DecStart", Rval, 0U, 0U, 0U, 0U);
        Rval = DSP_ERR_0007;
    }
    HL_RelCmdBuffer(CmdBufferId);

    return Rval;
}

static UINT32 HL_VideoDecStopImpl(const UINT8 WriteMode, UINT16 StreamIdx, const UINT8 *pShowLastFrame)
{
    UINT8 CmdBufferId = 0U; void *CmdBufferAddr = NULL;
    UINT32 Rval;
    cmd_decoder_stop_t *StopCmd = HL_DefCtxCmdBufPtrDecStop;
    CTX_VID_DEC_INFO_s VidDecInfo;

    HL_GetVidDecInfo(HL_MTX_OPT_ALL, StreamIdx, &VidDecInfo);
    HL_AcqCmdBuffer(&CmdBufferId, &CmdBufferAddr);
    dsp_osal_typecast(&StopCmd, &CmdBufferAddr);
    StopCmd->hdr.decoder_id = 0U;
    (void)dsp_osal_memcpy(&StopCmd->hdr.decoder_id, &VidDecInfo.StreamID, sizeof(UINT8));

    if (AMBA_DSP_DEC_BITS_FORMAT_H264 == VidDecInfo.BitsFormat) {
        StopCmd->hdr.codec_type = DEC_CODEC_TYPE_AVC;
    } else if (AMBA_DSP_DEC_BITS_FORMAT_H265 == VidDecInfo.BitsFormat) {
        StopCmd->hdr.codec_type = DEC_CODEC_TYPE_HEVC;
    } else {
        StopCmd->hdr.codec_type = DEC_CODEC_TYPE_JPEG;
    }

    if(pShowLastFrame[StreamIdx] == 0U) {
        StopCmd->stop_mode = DEC_STOP_MODE_VOUT_STOP;
    } else if(pShowLastFrame[StreamIdx] == 1U) {
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

/**
* Video decode config function
* @param [in]  MaxNumStream max decode stream index
* @param [in]  pStreamConfig stream configuration
* @return ErrorCode
*/
UINT32 dsp_video_dec_cfg(UINT16 MaxNumStream, const AMBA_DSP_VIDDEC_STREAM_CONFIG_s *pStreamConfig)
{
    const Dsp_DumpApi_t* DumpApi = AmbaDSP_GetDumpApiFunc();
    const Dsp_CheckApi_t* CheckApi = AmbaDSP_GetCheckApiFunc();
    UINT32 Rval;
    CTX_VID_DEC_INFO_s VidDecInfo;
    UINT16 i;

    /* DSP CtxInit */
    Rval = HL_CtxInit();

    /* Timing sanity check */
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

    /* Body */
    if (Rval == OK) {
        // turn on  MD/XF
        //AmbaCSL_PllSetVdspClkCtrl(0x3FFF);
        //FIXME, may have risk if CTX_VID_DEC_INFO_s != AMBA_DSP_VIDDEC_STREAM_CONFIG_s
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
//FIXME, state reset when profile switch in VidDecStart
            VidDecInfo.State = VIDDEC_STATE_READY;
            HL_SetVidDecInfo(HL_MTX_OPT_SET, i, &VidDecInfo);
        }
    } else {
        AmbaLL_LogUInt5("[Err][0x%X] VideoDecConfig check", Rval, 0U, 0U, 0U, 0U);
    }

    return Rval;
}

static inline UINT32 HL_VidDecStartPreprocStill(const CTX_VID_DEC_INFO_s *pVidDecInfo, UINT8 RescChanged)
{
    UINT32 Rval = OK;

    // Current in Still PB mode
    UINT8 CmdBufferId = 0U; void *CmdBufferAddr = NULL;
    cmd_binding_cfg_t *FpBindCfg = HL_DefCtxCmdBufPtrBind;

    AmbaLL_LogUInt5("[SSP][HL_VidDecStartPreprocStill] Switch to Video PLAYBACK mode", 0U, 0U, 0U, 0U, 0U);

    if (RescChanged == 1U) {
        /* Switch2PB */
        Rval = HL_SwitchProfile(DSP_PROF_STATUS_PLAYBACK, SWITCH_PROF_PREPROC);
        if (Rval == OK) {
            Rval = HL_VideoDecFillPreProcGroupCmds(AMBA_DSP_CMD_DEFAULT_WRITE);
        }
    } else {
        /* UnBind frist */
        HL_AcqCmdBuffer(&CmdBufferId, &CmdBufferAddr);
        dsp_osal_typecast(&FpBindCfg, &CmdBufferAddr);
        Rval = HL_FillFpBindSetup(FpBindCfg, BIND_VIDEO_DECODE, 0U/*IsBind*/, 0/*ViewZoneId*/, 0U/*SrcPinId*/, 0U/*StrmId*/);
        if (Rval != OK) {
            AmbaLL_LogUInt5("[%d]FpBind filling fail", __LINE__, 0U, 0U, 0U, 0U);
        } else {
            (void)AmbaHL_CmdDspBindingCfg(AMBA_DSP_CMD_VDSP_NORMAL_WRITE, FpBindCfg);
        }
        HL_RelCmdBuffer(CmdBufferId);

        /* Bind Dproc and Postp */
        HL_AcqCmdBuffer(&CmdBufferId, &CmdBufferAddr);
        dsp_osal_typecast(&FpBindCfg, &CmdBufferAddr);
        Rval = HL_FillFpBindSetup(FpBindCfg, BIND_VIDEO_DECODE, 1U/*IsBind*/, 0/*ViewZoneId*/, 0U/*SrcPinId*/, 0U/*StrmId*/);
        if (Rval != OK) {
            AmbaLL_LogUInt5("[%d]FpBind filling fail", __LINE__, 0U, 0U, 0U, 0U);
        } else {
            (void)AmbaHL_CmdDspBindingCfg(AMBA_DSP_CMD_VDSP_NORMAL_WRITE, FpBindCfg);
        }
        HL_RelCmdBuffer(CmdBufferId);
        if (0U != pVidDecInfo->PostpOpCfgEnable) {
            AmbaLL_LogUInt5( "[SSP][HL_VidDecStartPreprocStill] init Postp", 0U, 0U, 0U, 0U, 0U);
            Rval = HL_VideoDecPostCtrlImpl(AMBA_DSP_CMD_VDSP_NORMAL_WRITE, 0U);
        }
    }

    return Rval;
}

static inline UINT32 HL_VidDecStartPreproc(const CTX_VID_DEC_INFO_s *pVidDecInfo)
{
    UINT32 Rval = OK;
    UINT8 DspOpModeInit, DspOpModeIdle, DspOpModeCamera, RescChanged;

    DspOpModeInit = (DSP_GetProfState() == DSP_PROF_STATUS_INVALID)? 1U: 0U;
    DspOpModeIdle = (DSP_GetProfState() == DSP_PROF_STATUS_IDLE)? 1U: 0U;
    DspOpModeCamera = (DSP_GetProfState() == DSP_PROF_STATUS_CAMERA)? 1U: 0U;
    RescChanged = (HL_GetRescState() == HL_RESC_CONFIGED)? 1U: 0U;

    AmbaLL_LogUInt5("[SSP][HL_VidDecStartPreproc] DecMode[%d] OpInit[%d] OpIdle[%d] OpCamera[%d]",
                    pVidDecInfo->DecoderMode, DspOpModeInit, DspOpModeIdle, DspOpModeCamera, 0U);
    AmbaLL_LogUInt5("                             RescChg[%d]", RescChanged, 0U, 0U, 0U, 0U);

    if (HL_IsDec2Vproc() == 1U) {
        // Under DUPLEX
    } else if ((pVidDecInfo->DecoderMode != DECODE_MODE_VIDEO) && (pVidDecInfo->DecoderMode != DECODE_MODE_STILL)) {
        if (DspOpModeInit == 1U) {
            Rval = HL_VideoDecBootSetup(DSP_PROF_STATUS_PLAYBACK); // Boot2PB
        } else if ((DspOpModeIdle == 1U) ||
                   (DspOpModeCamera == 1U) ||
                   (RescChanged == 1U)) {
            /* Switch2PB */
            Rval = HL_SwitchProfile(DSP_PROF_STATUS_PLAYBACK, SWITCH_PROF_PREPROC);
            if (Rval == OK) {
                Rval = HL_VideoDecFillPreProcGroupCmds(AMBA_DSP_CMD_DEFAULT_WRITE);
            }
        } else {
            //TBD
        }
    } else if (pVidDecInfo->DecoderMode == DECODE_MODE_VIDEO) {
        if (RescChanged == 1U) {
            /* Switch2PB */
            Rval = HL_SwitchProfile(DSP_PROF_STATUS_PLAYBACK, SWITCH_PROF_PREPROC);
            if (Rval == OK) {
                Rval = HL_VideoDecFillPreProcGroupCmds(AMBA_DSP_CMD_DEFAULT_WRITE);
            }
        } else {
            //TBD
        }
    } else if (pVidDecInfo->DecoderMode == DECODE_MODE_STILL) {
        // Current in Still PB mode
        Rval = HL_VidDecStartPreprocStill(pVidDecInfo, RescChanged);
    } else {
        // already in Video PB mode
    }

    return Rval;
}

static inline UINT32 HL_VidDecPreprocToPbMode(void)
{
    UINT32 Rval = OK;
    UINT8 DspOpModeInit, DspOpModeIdle;
    CTX_RESOURCE_INFO_s *Resource = HL_CtxResInfoPtr;

    DspOpModeInit = (DSP_GetProfState() == DSP_PROF_STATUS_INVALID)? 1U: 0U;
    DspOpModeIdle = (DSP_GetProfState() == DSP_PROF_STATUS_IDLE)? 1U: 0U;

    HL_GetResourcePtr(&Resource);
    if (DspOpModeInit == 1U) {
        Rval = DSP_Bootup(&Resource->DspSysCfg, &Resource->ChipInfoAddr);
    } else if (DspOpModeIdle == 1U) {
        Rval = HL_SwitchProfile(DSP_PROF_STATUS_PLAYBACK, SWITCH_PROF_PROC);
        (void)HL_SwitchProfile(DSP_PROF_STATUS_PLAYBACK, SWITCH_PROF_POSTPROC);
    } else {
        // DO NOTHING
    }

    return Rval;
}

static inline UINT32 HL_VidDecPreprocWaitPbMode(const CTX_VID_DEC_INFO_s *pVidDecInfo, UINT16 DecIdx)
{
    UINT32 Rval = OK;
    CTX_VID_DEC_INFO_s TmpVidDecInfo;

    if (HL_IsDec2Vproc() == 1U) {
        // Under DUPLEX

        /* 1. Re-start from stop 0 */
        /* 2. first decode from yuv boot */
        if (pVidDecInfo->CurrentBind != BIND_VIDEO_DECODE_TO_VPROC) {
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
            //do nothing
        }
    } else if (DSP_WaitProfState(DSP_PROF_STATUS_PLAYBACK, MODE_SWITCH_TIMEOUT) != OK) {
        // wait op mode change to playback
        AmbaLL_LogUInt5( "[Err][0x%X][%d] WaitProf[%d]", Rval, __LINE__, DSP_PROF_STATUS_PLAYBACK, 0U, 0U);
        Rval = DSP_ERR_0007;
    } else if (DSP_WaitPostPVoutExist(WAIT_FLAG_TIMEOUT) != OK) {
        // wait ppvout_exist change to 1
        //active: 1
        //invalid:0
        AmbaLL_LogUInt5( "[Err][0x%X][%d] WaitPostPVoutExist timeout", Rval, __LINE__, 0U, 0U, 0U);
        Rval = DSP_ERR_0007;
    } else {
        HL_SwitchDspSysState(DSP_PROF_STATUS_PLAYBACK);
        HL_SetRescState(HL_RESC_SETTLED);
        HL_GetVidDecInfo(HL_MTX_OPT_GET, DecIdx, &TmpVidDecInfo);
        TmpVidDecInfo.DecoderMode = DECODE_MODE_VIDEO;
        HL_SetVidDecInfo(HL_MTX_OPT_SET, DecIdx, &TmpVidDecInfo);
    }

    return Rval;
}

static inline UINT32 HL_VideoDecStartChk(UINT16 NumStream, const UINT16 *pStreamIdx, const AMBA_DSP_VIDDEC_START_CONFIG_s *pStartConfig)
{
    const Dsp_DumpApi_t* DumpApi = AmbaDSP_GetDumpApiFunc();
    const Dsp_CheckApi_t* CheckApi = AmbaDSP_GetCheckApiFunc();
    const Dsp_ResourceApi_t* ResourceApi = AmbaDSP_GetResourceApiFunc();
    UINT32 Rval;

    /* DSP CtxInit */
    Rval = HL_CtxInit();

    /* Timing sanity check */
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
            HL_GetVidDecInfo(HL_MTX_OPT_ALL, pStreamIdx[i], &VidDecInfo);
            StrmId = VidDecInfo.StreamID;
            Rval = HL_VidDecStartPreproc(&VidDecInfo);
            if (Rval != OK) {
                AmbaLL_LogUInt5("[Err][0x%X] VideoDecStart BootSetup", Rval, 0U, 0U, 0U, 0U);
            }

            //DSP boot/switch to PB
            if (Rval == OK) {
                Rval = HL_VidDecPreprocToPbMode();
            }

            if (Rval == OK) {
                Rval = HL_VidDecPreprocWaitPbMode(&VidDecInfo, StrmId);

                // DecodeSetup
                if (Rval == OK) {
                    Rval = HL_VideoDecBitsStreamImpl(AMBA_DSP_CMD_VDSP_NORMAL_WRITE, StrmId);
                }

                // Issue VrsclCmd for DramPreview
                if (Rval == OK) {
                    Rval = HL_VideoDecVrsclStartImpl(StrmId, &pStartConfig[i]);
                }
            } else {
                AmbaLL_LogUInt5("[Err][0x%X] Boot2PB fail", Rval, 0U, 0U, 0U, 0U);
            }

            if (Rval == OK) {
                // DecodeSpeedCmd
                Rval = HL_VideoDecSpeedImpl(StrmId, &pStartConfig[i]);

                // DecodeBatchCmd
                if (Rval == OK) {
                    Rval = HL_VideoDecBatchImpl(StrmId);
                } else {
                    AmbaLL_LogUInt5("[Err][0x%X] VideoDecStart DecodeSpeed", Rval, 0U, 0U, 0U, 0U);
                }

                // DecodeStartCmd
                if (Rval == OK) {
                    Rval = HL_VideoDecStartImpl(StrmId, &pStartConfig[i]);
                } else {
                    AmbaLL_LogUInt5("[Err][0x%X] VideoDecStart VideoDecBatch", Rval, 0U, 0U, 0U, 0U);
                }
            } else {
                AmbaLL_LogUInt5("[Err][0x%X] VideoDecStart DecodeSetup", Rval, 0U, 0U, 0U, 0U);
            }

            if (Rval == OK) {
                HL_GetVidDecInfo(HL_MTX_OPT_GET, pStreamIdx[i], &VidDecInfo);
                VidDecInfo.State = VIDDEC_STATE_RUN;
                HL_SetVidDecInfo(HL_MTX_OPT_SET, pStreamIdx[i], &VidDecInfo);
            } else {
                AmbaLL_LogUInt5("[Err][0x%X] VideoDecStart DecodeStart", Rval, 0U, 0U, 0U, 0U);
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
    const Dsp_DumpApi_t* DumpApi = AmbaDSP_GetDumpApiFunc();
    const Dsp_CheckApi_t* CheckApi = AmbaDSP_GetCheckApiFunc();
    const Dsp_ResourceApi_t* ResourceApi = AmbaDSP_GetResourceApiFunc();
    UINT32 Rval;
    CTX_VID_DEC_INFO_s VidDecInfo = {0};
    UINT8 DspOpModePlayback;
    UINT16 i;

    /* DSP CtxInit */
    Rval = HL_CtxInit();

    /* Timing sanity check */
    /* Input sanity check */
    if ((Rval == OK) &&
        (DumpApi != NULL)) {
        DumpApi->pVideoDecPostCtrl(StreamIdx, NumPostCtrl, pPostCtrl);
    }
    if ((Rval == OK) &&
        (CheckApi != NULL)) {
        Rval = CheckApi->pVideoDecPostCtrl(StreamIdx, NumPostCtrl, pPostCtrl);
    }
    /* HW/SW resources check */
    if ((Rval == OK) &&
        (ResourceApi != NULL)) {
        Rval = ResourceApi->pVideoDecPostCtrl(StreamIdx, NumPostCtrl, pPostCtrl);
    }

    /* Logic sanity check */

    DspOpModePlayback = (DSP_GetProfState() == DSP_PROF_STATUS_PLAYBACK)? 1U: 0U;

    /* Body */
    if (Rval == OK) {
        if (HL_IsDec2Vproc() == 0U) {
            //Save Postp Command to DecodeUtility
            HL_GetVidDecInfo(HL_MTX_OPT_GET, StreamIdx, &VidDecInfo);
            for (i = 0; i < NumPostCtrl; i++) {
                UINT8 PpVoutIdx;
                PpVoutIdx = pPostCtrl[i].VoutIdx;
                VidDecInfo.PostCtlCfg[PpVoutIdx] = pPostCtrl[i];
            }
            HL_SetVidDecInfo(HL_MTX_OPT_SET, StreamIdx, &VidDecInfo);

            if (DspOpModePlayback == 1U) {
                Rval = HL_VideoDecPostCtrlImpl(AMBA_DSP_CMD_VDSP_NORMAL_WRITE, StreamIdx);
            }
        }

        /* Wait until PpVoutExist in Video decode :
         * Video decode need PpVoutExist when show result to vout
         * still decode use Dproc_Display to push yuv data
         * from uCoder, it's better to wait it after profile_change
         */
//        if (Rval == OK) {
//            if ((ProfState == DSP_PROF_STATUS_PLAYBACK) && ((PostProcCmd.voutA_enable == 1U) || (PostProcCmd.voutB_enable == 1U))) {
//                Rval = AmbaDSP_WaitPostPVoutExist(PP_VOUT_ACTIVE_TIMEOUT);
//            }
//        }
    } else {
        AmbaLL_LogUInt5("VideoDecPostCtrl check error", 0U, 0U, 0U, 0U, 0U);
    }

    return Rval;
}

static inline UINT32 HL_VideoDecStopChk(UINT16 NumStream, const UINT16 *pStreamIdx, const UINT8 *pShowLastFrame)
{
    const Dsp_DumpApi_t* DumpApi = AmbaDSP_GetDumpApiFunc();
    const Dsp_CheckApi_t* CheckApi = AmbaDSP_GetCheckApiFunc();
    const Dsp_ResourceApi_t* ResourceApi = AmbaDSP_GetResourceApiFunc();
    UINT32 Rval;

    /* DSP CtxInit */
    Rval = HL_CtxInit();

    /* Timing sanity check */
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
    UINT32 Rval, DecIdleFlg = 0U;
    CTX_VID_DEC_INFO_s VidDecInfo = {0};

    Rval = HL_VideoDecStopChk(NumStream, pStreamIdx, pShowLastFrame);

    /* Body */
    if (Rval == OK) {
        for (i = 0; i < NumStream; i++) {
            UINT16 CfgStreamIdx = pStreamIdx[i];

            if (HL_IsDec2Vproc() == 0U) {
                // Normal decode
                Rval = DSP_WaitVdspEvent((UINT32)EVENT_FLAG_INT_VDSP0_EVENT, 1, WAIT_INTERRUPT_TIMEOUT);
                if (Rval != OK) {
                    AmbaLL_LogUInt5("VideoDecStop Wait VDSP0 fail %u %d", Rval, __LINE__, 0U, 0U, 0U);
                }
                Rval = HL_VideoDecVrsclStopImpl(AMBA_DSP_CMD_VDSP_NORMAL_WRITE, CfgStreamIdx);
            } else {
                // Under DUPLEX
            }

            if (Rval == OK) {
                if (HL_IsDec2Vproc() == 0U) {

                    Rval = DSP_WaitVdspEvent((UINT32)EVENT_FLAG_INT_VDSP0_EVENT, 1, WAIT_INTERRUPT_TIMEOUT);
                    if (Rval != OK) {
                        AmbaLL_LogUInt5("VideoDecStop Wait VDSP0 fail %u %d", Rval, __LINE__, 0U, 0U, 0U);
                    } else {
                        Rval = HL_VideoDecStopImpl(AMBA_DSP_CMD_VDSP_NORMAL_WRITE, CfgStreamIdx, pShowLastFrame);

                        HL_GetVidDecInfo(HL_MTX_OPT_ALL, CfgStreamIdx, &VidDecInfo);

                        /*
                         * 2019/10/16, Per uCoder, decoder enter different status base on different StopMethod
                         *   ShowLastFrm = 0(ie black vout, or DecStopMode = 0), DecStatus will be DEC_OPM_IDLE
                         *   ShowLastFrm = 1(or DecStopMode = 1), DecStatus will be DEC_OPM_VDEC_IDLE
                         */
                        DecIdleFlg = (pShowLastFrame[CfgStreamIdx] == 1U)? (UINT32)DEC_OPM_VDEC_IDLE: (UINT32)DEC_OPM_IDLE;
                        if (Rval == OK) {
                            Rval = DSP_WaitDecState(0U, (UINT8)DecIdleFlg, MODE_SWITCH_TIMEOUT, 1U);
                            if (Rval != OK) {
                                AmbaLL_LogUInt5("VideoDecStop %u Wait DEC_IDLE[0x%X] fail[0x%X][%d]",
                                        DSP_GetDecState(0U), DecIdleFlg, Rval, __LINE__, 0U);
                            }
                        }
                    }
                } else {
                    UINT8 ShowLastFrm[AMBA_DSP_MAX_DEC_STREAM_NUM];
#if 0 // Allways use DEC_STOP_MODE_LAST_PIC with dec2vproc
                    /* Always use stop=1 with dec2vproc + effect */
                    HL_GetViewZoneInfo(HL_MTX_OPT_ALL, VidDecInfo.ViewZoneId, &ViewZoneInfo);
                    if (ViewZoneInfo.IsEffectChanMember == 1U) {
                        ShowLastFrm[0U] = DEC_STOP_MODE_LAST_PIC;
                    } else {
                        ShowLastFrm[0U] = pShowLastFrame[CfgStreamIdx];
                    }
#endif
                    ShowLastFrm[CfgStreamIdx] = DEC_STOP_MODE_LAST_PIC;
                    Rval = HL_VideoDecStopImpl(AMBA_DSP_CMD_VDSP_NORMAL_WRITE, CfgStreamIdx, ShowLastFrm);
                }
            } else {
                AmbaLL_LogUInt5("VideoDecStop VRSCL stop fail", 0U, 0U, 0U, 0U, 0U);
            }

            if (Rval == OK) {
                HL_GetVidDecInfo(HL_MTX_OPT_GET, CfgStreamIdx, &VidDecInfo);
                //FIXME, state transit to IDLE?
                VidDecInfo.State = VIDDEC_STATE_READY;
                HL_SetVidDecInfo(HL_MTX_OPT_SET, CfgStreamIdx, &VidDecInfo);
            } else {
                AmbaLL_LogUInt5("VideoDecStop decoder stop fail", 0U, 0U, 0U, 0U, 0U);
            }
        }


    } else {
        AmbaLL_LogUInt5("VideoDecStop check error", 0U, 0U, 0U, 0U, 0U);
    }

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
    const Dsp_DumpApi_t* DumpApi = AmbaDSP_GetDumpApiFunc();
    const Dsp_CheckApi_t* CheckApi = AmbaDSP_GetCheckApiFunc();
    const Dsp_ResourceApi_t* ResourceApi = AmbaDSP_GetResourceApiFunc();
    UINT16 i;
    UINT32 Rval;

    /* DSP CtxInit */
    Rval = HL_CtxInit();

    /* Timing sanity check */
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

    /* Body */
    if (Rval == OK) {
        for (i = 0; i < NumStream; i++) {
            UINT16 CfgStreamIdx = pStreamIdx[i];

            Rval = HL_VideoDecTrickImpl(CfgStreamIdx, pTrickPlay);
            if (Rval != OK) {
                break;
            }
        }
    } else {
        AmbaLL_LogUInt5("TrickPlay check error", 0U, 0U, 0U, 0U, 0U);
    }

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
    const Dsp_DumpApi_t* DumpApi = AmbaDSP_GetDumpApiFunc();
    const Dsp_CheckApi_t* CheckApi = AmbaDSP_GetCheckApiFunc();
    const Dsp_ResourceApi_t* ResourceApi = AmbaDSP_GetResourceApiFunc();
    UINT16 i;
    UINT32 Rval;

    /* DSP CtxInit */
    Rval = HL_CtxInit();

    /* Timing sanity check */
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



