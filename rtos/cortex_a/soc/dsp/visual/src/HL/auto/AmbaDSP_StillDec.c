/**
*  @file AmbaDSP_StillDec.c
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
*  @details Implement of SSP Still Decoder related APIs
*
*/

#include "AmbaDSP_Log.h"
#include "AmbaDSP_Cmds.h"
#include "AmbaDSP_ArmComm.h"
#include "AmbaDSP_ApiDump.h"
#include "AmbaDSP_ApiCheck.h"
#include "AmbaDSP_StillDec.h"
#include "AmbaDSP_ApiResource.h"
#include "AmbaDSP_EncodeUtility.h"
#include "AmbaDSP_DecodeUtility.h"
#include "dsp_priv_api.h"

static inline UINT32 HL_StlDecSystemSetupImpl(const UINT8 WriteMode)
{
    UINT32 Rval;
    UINT8 CmdBufferId = 0U; void *CmdBufferAddr = NULL;
    cmd_dsp_config_t *SysSetup = HL_DefCtxCmdBufPtrSys;
    cmd_dsp_hal_inf_t *SysHalSetup = HL_DefCtxCmdBufPtrHal;

    HL_AcqCmdBuffer(&CmdBufferId, &CmdBufferAddr);
    dsp_osal_typecast(&SysSetup, &CmdBufferAddr);
    Rval = HL_FillDecSystemSetup(SysSetup);
    if (Rval != OK) {
        AmbaLL_LogUInt5("[Err][0x%X][%d] SdecSysSetup", Rval, __LINE__, 0U, 0U, 0U);
    } else {
        Rval = AmbaHL_CmdDspConfig(WriteMode, SysSetup);
        if (Rval != OK) {
            AmbaLL_LogUInt5("[Err][0x%X][%d] SdecSysSetup", Rval, __LINE__, 0U, 0U, 0U);
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
                AmbaLL_LogUInt5("[Err][0x%X][%d] SdecSysSetup", Rval, __LINE__, 0U, 0U, 0U);
            }
        } else {
            AmbaLL_LogUInt5("[Err][0x%X][%d] SdecSysSetup", Rval, __LINE__, 0U, 0U, 0U);
        }
    } else {
        AmbaLL_LogUInt5("[Err][0x%X][%d] SdecSysSetup", Rval, __LINE__, 0U, 0U, 0U);
    }
    HL_RelCmdBuffer(CmdBufferId);
    return Rval;
}

static inline UINT32 HL_StlDecDspDebugLevelImpl(const UINT8 WriteMode)
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

static inline UINT32 HL_StlDecFlowMaxCfgImpl(const UINT8 WriteMode)
{
    UINT8 CmdBufferId = 0U; void *CmdBufferAddr = NULL;
    UINT32 Rval;
    cmd_dsp_dec_flow_max_cfg_t *DecFlwMaxCfg = HL_DefCtxCmdBufPtrDecMax;

    HL_AcqCmdBuffer(&CmdBufferId, &CmdBufferAddr);
    dsp_osal_typecast(&DecFlwMaxCfg, &CmdBufferAddr);
    Rval = HL_FillDecFlowMaxCfgSetup(DecFlwMaxCfg);
    if (Rval != OK) {
        AmbaLL_LogUInt5("[Err][0x%X][%d] FillSdecFlowMax", Rval, __LINE__, 0U, 0U, 0U);
    } else {
        Rval = AmbaHL_CmdDspDecFlowMaxCfg(WriteMode, DecFlwMaxCfg);
        if (Rval != OK) {
            AmbaLL_LogUInt5("[Err][0x%X][%d] SdecFlowMax", Rval, __LINE__, 0U, 0U, 0U);
        }
    }
    HL_RelCmdBuffer(CmdBufferId);

    return Rval;
}

static inline UINT32 HL_StlDecFpBindingImpl(const UINT8 WriteMode)
{
    UINT8 CmdBufferId = 0U; void *CmdBufferAddr = NULL;
    UINT32 Rval;
    cmd_binding_cfg_t *FpBindCfg = HL_DefCtxCmdBufPtrBind;

    HL_AcqCmdBuffer(&CmdBufferId, &CmdBufferAddr);
    dsp_osal_typecast(&FpBindCfg, &CmdBufferAddr);
    Rval = HL_FillFpBindSetup(FpBindCfg,
                              BIND_STILL_DECODE,
                              1U/*IsBind*/,
                              0/*ViewZoneId*/,
                              0U/*SrcPinId*/,
                              0U/*StrmId*/);
    if (Rval != OK) {
        AmbaLL_LogUInt5("[Err][0x%X][%d] SdecBind", 0U, 0U, 0U, 0U, 0U);
    } else {
        if (FpBindCfg->num_of_bindings > 0U) {
            Rval = AmbaHL_CmdDspBindingCfg(WriteMode, FpBindCfg);
            if (Rval != OK) {
                AmbaLL_LogUInt5("[Err][0x%X][%d] SdecBind", Rval, __LINE__, 0U, 0U, 0U);
            }
        }
    }
    HL_RelCmdBuffer(CmdBufferId);

//FIXME, Issue it once needed
    HL_AcqCmdBuffer(&CmdBufferId, &CmdBufferAddr);
    dsp_osal_typecast(&FpBindCfg, &CmdBufferAddr);
    Rval = HL_FillFpBindSetup(FpBindCfg,
                              BIND_STILL_DECODE_Y2DISP,
                              1U/*IsBind*/,
                              0/*ViewZoneId*/,
                              0U/*SrcPinId*/,
                              0U/*StrmId*/);
    if (Rval != OK) {
        AmbaLL_LogUInt5("[Err][0x%X][%d] SdecBind Y2Disp", 0U, 0U, 0U, 0U, 0U);
    } else {
        Rval = AmbaHL_CmdDspBindingCfg(WriteMode, FpBindCfg);
        if (Rval != OK) {
            AmbaLL_LogUInt5("[Err][0x%X][%d] SdecBind Y2Disp", Rval, __LINE__, 0U, 0U, 0U);
        }
    }
    HL_RelCmdBuffer(CmdBufferId);

    return Rval;
}

static inline UINT32 HL_StlDecSystemActivateImpl(const UINT8 WriteMode)
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

static inline UINT32 HL_StlDecVoutSetupOnVoutReset(const UINT8 WriteMode,
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

static inline UINT32 HL_StlDecVoutSetupOnVideoOsdSetup(const UINT8 WriteMode,
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
        Rval = HL_VoutVideoSetupSourceSelect(VoutIdx, DSP_VOUT_SRC_POSTP, HL_VOUT_SOURCE_VIDEO_CAP);
        if (Rval != OK) {
            AmbaLL_LogUInt5("[Err][0x%X][%d] DecVoutSourceSetup[%d]", Rval, __LINE__, VoutIdx, 0U, 0U);
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

static UINT32 HL_StlDecVoutSetupImpl(const UINT8 WriteMode)
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
    Rval = HL_StlDecVoutSetupOnVoutReset(WriteMode, &ResetVout[0U], &VoutInfo[0U]);

    // VoutVideoSetup and OSDSetup
    for (i = 0U; i < AMBA_DSP_MAX_VOUT_NUM; i++) {
        if (Rval == OK) {
            Rval = HL_StlDecVoutSetupOnVideoOsdSetup(WriteMode, (UINT8)i, EnablePrev[i], &VoutInfo[i]);
        } else {
            AmbaLL_LogUInt5("[Err][0x%X][%d] DecVoutSetup[%d]", Rval, __LINE__, i, 0U, 0U);
        }
    }

    return Rval;
}

static UINT32 HL_StlDecPostCtrlImpl(const UINT8 WriteMode, UINT16 StreamIdx)
{
    UINT8 CmdBufferId = 0U; void *CmdBufferAddr = NULL;
    UINT32 Rval = OK;
    UINT16 PostCtrlNum = 0U;
    cmd_postproc_t *PostProcCfg = HL_DefCtxCmdBufPtrPostP;
    CTX_VID_DEC_INFO_s DecInfo = {0};
    AMBA_DSP_VIDDEC_POST_CTRL_s PostCtrlCfg[AMBA_DSP_MAX_VOUT_NUM] = {0};
    UINT8 VoutIdx;
    UINT8 IsY2Disp = (UINT8)1U;

    HL_GetVidDecInfo(HL_MTX_OPT_ALL, StreamIdx, &DecInfo);

    for (VoutIdx = 0; VoutIdx < AMBA_DSP_MAX_VOUT_NUM; VoutIdx++) {
        if (DecInfo.PostCtlCfg[VoutIdx].VoutWindow.Width != 0U) {
            PostCtrlCfg[PostCtrlNum] = DecInfo.PostCtlCfg[VoutIdx];
            PostCtrlNum++;
        }
    }

    if (DecInfo.XcodeMode != AMBA_DSP_XCODE_NON_THROTTLE) {
        HL_AcqCmdBuffer(&CmdBufferId, &CmdBufferAddr);
        dsp_osal_typecast(&PostProcCfg, &CmdBufferAddr);
        PostProcCfg->decode_id = (UINT8)StreamIdx;
        Rval = HL_FillDecPostCtrlSetup(PostCtrlNum, PostCtrlCfg, PostProcCfg, IsY2Disp);
        if (Rval != OK) {
            AmbaLL_LogUInt5("[Err][0x%X][%d] DecPostCtrlFill", Rval, __LINE__, 0U, 0U, 0U);
        } else {
            Rval = AmbaHL_CmdPostproc(WriteMode, PostProcCfg);
        }
        HL_RelCmdBuffer(CmdBufferId);
    }

    return Rval;
}

static UINT32 HL_StillDecFillPreProcGroupCmds(const UINT8 WriteMode)
{
    UINT32 Rval;

    AmbaLL_LogUInt5("Setup StlDec DefCmdBuf", 0U, 0U, 0U, 0U, 0U);

    /* SystemSetup */
    Rval = HL_StlDecSystemSetupImpl(WriteMode);
    if (Rval != OK) {
        AmbaLL_LogUInt5("[Err][0x%X][%d] VdecPreProc", Rval, __LINE__, 0U, 0U, 0U);
    }

    /* Debug Level */
    if (Rval == OK) {
        Rval = HL_StlDecDspDebugLevelImpl(WriteMode);
        if (Rval != OK) {
            AmbaLL_LogUInt5("[Err][0x%X][%d] VdecPreProc", Rval, __LINE__, 0U, 0U, 0U);
        }
    }

    /* Decode Flow Max Config */
    if (Rval == OK) {
        Rval = HL_StlDecFlowMaxCfgImpl(WriteMode);
        if (Rval != OK) {
            AmbaLL_LogUInt5("[Err][0x%X][%d] VdecPreProc", Rval, __LINE__, 0U, 0U, 0U);
        }
    }

    /* FlowProcessor Binding */
    if (Rval == OK) {
        Rval = HL_StlDecFpBindingImpl(WriteMode);
        if (Rval != OK) {
            AmbaLL_LogUInt5("[Err][0x%X][%d] VdecPreProc", Rval, __LINE__, 0U, 0U, 0U);
        }
    }

    /* Profile Activate */
    if (Rval == OK) {
        Rval = HL_StlDecSystemActivateImpl(WriteMode);
        if (Rval != OK) {
            AmbaLL_LogUInt5("[Err][0x%X][%d] VdecPreProc", Rval, __LINE__, 0U, 0U, 0U);
        }
    }

    /* Vout relative */
    if (Rval == OK) {
        Rval = HL_StlDecVoutSetupImpl(WriteMode);
        if (Rval != OK) {
            AmbaLL_LogUInt5("[Err][0x%X][%d] VdecPreProc", Rval, __LINE__, 0U, 0U, 0U);
        }
    }

    return Rval;
}


/* Used when dsp is NOT BOOTED */
static UINT32 HL_StillDecBootSetup(UINT8 TargetProfile)
{
    UINT32 Rval;
    UINT8 DspOpModeInit, DspOpModeIdle;

    DspOpModeInit = (DSP_GetProfState() == DSP_PROF_STATUS_INVALID)? 1U: 0U;
    DspOpModeIdle = (DSP_GetProfState() == DSP_PROF_STATUS_IDLE)? 1U: 0U;

    if ((TargetProfile == DSP_PROF_STATUS_PLAYBACK) &&
        ((DspOpModeInit == 1U) || (DspOpModeIdle == 1U))) {
        DSP_ClrDefCfgBuffer();
        Rval = HL_StillDecFillPreProcGroupCmds(AMBA_DSP_CMD_DEFAULT_WRITE);
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

static inline UINT32 HL_StlDecPreprocToPbMode(void)
{
    UINT32 Rval = OK;
    UINT8 DspOpModeInit, DspOpModeIdle;
    CTX_RESOURCE_INFO_s *Resource = HL_CtxResInfoPtr;

    if (HL_IsDec2Vproc() == 1U) {
        //DO NOTHING
    } else {
        DspOpModeInit = (DSP_GetProfState() == DSP_PROF_STATUS_INVALID)? 1U: 0U;
        DspOpModeIdle = (DSP_GetProfState() == DSP_PROF_STATUS_IDLE)? 1U: 0U;

        HL_GetResourcePtr(&Resource);
        if (DspOpModeInit == 1U) {
            Rval = DSP_Bootup(&Resource->DspSysCfg, &Resource->ChipInfoAddr);
        } else if (DspOpModeIdle == 1U) {
            Rval = HL_SwitchProfile(DSP_PROF_STATUS_PLAYBACK, SWITCH_PROF_PROC);
            if (Rval == OK) {
                Rval = HL_SwitchProfile(DSP_PROF_STATUS_PLAYBACK, SWITCH_PROF_POSTPROC);
            }
        } else {
            // DO NOTHING
        }
    }

    return Rval;
}

static inline UINT32 HL_StlDecPreprocWaitPbMode(UINT16 DecIdx)
{
    UINT32 Rval = OK;
    CTX_VID_DEC_INFO_s DecInfo;

    if (HL_IsDec2Vproc() == 1U) {
        //DO NOTHING
    } else {
        Rval = DSP_WaitProfState(DSP_PROF_STATUS_PLAYBACK, MODE_SWITCH_TIMEOUT);
        if (Rval != OK) {
            AmbaLL_LogUInt5( "[Err][0x%X][%d] WaitProf[%d]", Rval, __LINE__, DSP_PROF_STATUS_PLAYBACK, 0U, 0U);
            Rval = DSP_ERR_0007;
        } else {
            HL_SwitchDspSysState(DSP_PROF_STATUS_PLAYBACK);
            HL_SetRescState(HL_RESC_SETTLED);
            HL_GetVidDecInfo(HL_MTX_OPT_GET, DecIdx, &DecInfo);
            DecInfo.DecoderMode = DECODE_MODE_STILL;
            HL_SetVidDecInfo(HL_MTX_OPT_SET, DecIdx, &DecInfo);
        }
    }

    return Rval;
}

static UINT32 HL_StillDecVideoPb(UINT8 RescChanged)
{
    UINT8 CmdBufferId = 0U; void *CmdBufferAddr = NULL;
    UINT32 Rval;
    cmd_binding_cfg_t *FpBindCfg = HL_DefCtxCmdBufPtrBind;

    // Current in Video PB mode
    AmbaLL_LogUInt5("[SSP][HL_StillDecVideoPb] Switch to PLAYBACK mode", 0U, 0U, 0U, 0U, 0U);

    if (RescChanged == 1U) {
        /* Switch2PB */
        Rval = HL_SwitchProfile(DSP_PROF_STATUS_PLAYBACK, SWITCH_PROF_PREPROC);
        if (Rval == OK) {
            Rval = HL_StillDecFillPreProcGroupCmds(AMBA_DSP_CMD_DEFAULT_WRITE);
        } else {
            AmbaLL_LogUInt5("[%d][HL_StillDecVideoPb] HL_SwitchProfile fail", __LINE__, 0U, 0U, 0U, 0U);
        }
    } else {
        /* UnBind frist */
        HL_AcqCmdBuffer(&CmdBufferId, &CmdBufferAddr);
        dsp_osal_typecast(&FpBindCfg, &CmdBufferAddr);
        Rval = HL_FillFpBindSetup(FpBindCfg,
                                  BIND_STILL_DECODE_Y2DISP,
                                  0U/*IsBind*/,
                                  0/*ViewZoneId*/,
                                  0U/*SrcPinId*/,
                                  0U/*StrmId*/);
        if (Rval != OK) {
            AmbaLL_LogUInt5("[%d]FpBind filling fail", __LINE__, 0U, 0U, 0U, 0U);
        } else {
            Rval = AmbaHL_CmdDspBindingCfg(AMBA_DSP_CMD_VDSP_NORMAL_WRITE, FpBindCfg);
        }
        HL_RelCmdBuffer(CmdBufferId);

        if (Rval == OK) {
            /* Bind Dproc and Postp */
            HL_AcqCmdBuffer(&CmdBufferId, &CmdBufferAddr);
            dsp_osal_typecast(&FpBindCfg, &CmdBufferAddr);
            Rval = HL_FillFpBindSetup(FpBindCfg,
                                      BIND_STILL_DECODE_Y2DISP,
                                      1U/*IsBind*/,
                                      0/*ViewZoneId*/,
                                      0U/*SrcPinId*/,
                                      0U/*StrmId*/);
            if (Rval != OK) {
                AmbaLL_LogUInt5("[%d]FpBind filling fail", __LINE__, 0U, 0U, 0U, 0U);
            } else {
                Rval = AmbaHL_CmdDspBindingCfg(AMBA_DSP_CMD_VDSP_NORMAL_WRITE, FpBindCfg);
            }
            HL_RelCmdBuffer(CmdBufferId);
        }
    }

    return Rval;
}

static UINT32 HL_StillDecPreproc(UINT16 DecIdx)
{
    UINT32 Rval = OK;
    CTX_VID_DEC_INFO_s DecInfo;
    UINT8 DspOpModeInit, DspOpModeIdle, DspOpModeCamera, RescChanged;

    HL_GetVidDecInfo(HL_MTX_OPT_ALL, DecIdx, &DecInfo);

    DspOpModeInit = (DSP_GetProfState() == DSP_PROF_STATUS_INVALID)? 1U: 0U;
    DspOpModeIdle = (DSP_GetProfState() == DSP_PROF_STATUS_IDLE)? 1U: 0U;
    DspOpModeCamera = (DSP_GetProfState() == DSP_PROF_STATUS_CAMERA)? 1U: 0U;
    RescChanged = (HL_GetRescState() == HL_RESC_CONFIGED)? 1U: 0U;

    AmbaLL_LogUInt5("[SSP][HL_StillDecPreproc] DecMode[%d] OpInit[%d] OpIdle[%d] OpCamera[%d]",
                    DecInfo.DecoderMode, DspOpModeInit, DspOpModeIdle, DspOpModeCamera, 0U);
    AmbaLL_LogUInt5("                             RescChg[%d]", RescChanged, 0U, 0U, 0U, 0U);

    if (HL_IsDec2Vproc() == 1U) {
        // Under DUPLEX
    } else if (DecInfo.DecoderMode != DECODE_MODE_VIDEO) {
        if (DspOpModeInit == 1U) {
            Rval = HL_StillDecBootSetup(DSP_PROF_STATUS_PLAYBACK); // Boot2PB
        } else if ((DspOpModeIdle == 1U) ||
                   (DspOpModeCamera == 1U) ||
                   (RescChanged == 1U)) {
            /* Switch2PB */
            Rval = HL_SwitchProfile(DSP_PROF_STATUS_PLAYBACK, SWITCH_PROF_PREPROC);
            if (Rval == OK) {
                Rval = HL_StillDecFillPreProcGroupCmds(AMBA_DSP_CMD_DEFAULT_WRITE);
            } else {
                AmbaLL_LogUInt5("[%d][HL_StillDecPreproc] HL_SwitchProfile fail", __LINE__, 0U, 0U, 0U, 0U);
            }
        } else {
            //TBD
        }
    } else if (DecInfo.DecoderMode == DECODE_MODE_VIDEO) {
        Rval = HL_StillDecVideoPb(RescChanged);
    } else {
        // already in Still PB mode
    }

    if (Rval != OK) {
        AmbaLL_LogUInt5("[Err][0x%X] StillDecStart BootSetup", Rval, 0U, 0U, 0U, 0U);
    }

    //DSP boot/switch to PB
    if (Rval == OK) {
        Rval = HL_StlDecPreprocToPbMode();
    }

    //Check DSP OpMode
    if (Rval == OK) {
        Rval = HL_StlDecPreprocWaitPbMode(DecIdx);
    }

    return Rval;
}

static inline UINT32 HL_StillDecStartChk(UINT16 StreamIdx, const AMBA_DSP_STLDEC_START_CONFIG_s *pDecConfig)
{
    UINT32 Rval;
    const Dsp_DumpApi_t* DumpApi = AmbaDSP_GetDumpApiFunc();
    const Dsp_CheckApi_t* CheckApi = AmbaDSP_GetCheckApiFunc();
    const Dsp_ResourceApi_t* ResourceApi = AmbaDSP_GetResourceApiFunc();

    /* DSP CtxInit */
    Rval = HL_CtxInit();

    /* Timing sanity check */
    /* Input sanity check */
    if ((Rval == OK) &&
        (DumpApi != NULL)) {
        DumpApi->pStillDecStart(StreamIdx, pDecConfig);
    }
    if ((Rval == OK) &&
        (CheckApi != NULL)) {
        Rval = CheckApi->pStillDecStart(StreamIdx, pDecConfig);
    }
    /* HW/SW resources check */
    if ((Rval == OK) &&
        (ResourceApi != NULL)) {
        Rval = ResourceApi->pStillDecStart(StreamIdx, pDecConfig);
    }

    /* Logic sanity check */
    return Rval;
}

/**
* Still decode start function
* @param [in]  NumImg number of image
* @param [in]  pDecConfig decode configuration
* @return ErrorCode
*/
UINT32 dsp_still_dec_start(UINT16 StreamIdx, const AMBA_DSP_STLDEC_START_CONFIG_s *pDecConfig)
{
    UINT8 WriteMode = AMBA_DSP_CMD_VDSP_NORMAL_WRITE;
    UINT8 CmdBufferId = 0U; void *CmdBufferAddr = NULL;
    UINT32 Rval;
    CTX_VID_DEC_INFO_s DecInfo;
    cmd_decoder_stilldec_t *StlDec = HL_DefCtxCmdBufPtrDecStlDec;

    Rval = HL_StillDecStartChk(StreamIdx, pDecConfig);

    /* Body */
    if (Rval == OK) {
        Rval = HL_StillDecPreproc(StreamIdx);
    }

    if (Rval == OK) {
        HL_AcqCmdBuffer(&CmdBufferId, &CmdBufferAddr);
        dsp_osal_typecast(&StlDec, &CmdBufferAddr);
        StlDec->hdr.decoder_id = (UINT8)StreamIdx;
        if (AMBA_DSP_DEC_BITS_FORMAT_H264 == pDecConfig->BitsFormat) {
            StlDec->hdr.codec_type = (UINT8)DSP_DEC_TYPE_H264;
        } else if (AMBA_DSP_DEC_BITS_FORMAT_H265 == pDecConfig->BitsFormat) {
            StlDec->hdr.codec_type = (UINT8)DSP_DEC_TYPE_H265;
        } else {
            StlDec->hdr.codec_type = (UINT8)DSP_DEC_TYPE_JPEG;
        }
        (void)dsp_osal_virt2cli(pDecConfig->BitsAddr, &StlDec->bstrm_start);
        StlDec->bstrm_size = pDecConfig->BitsSize;
        (void)dsp_osal_virt2cli(pDecConfig->YuvBufAddr, &StlDec->yuv_buf_base);
        StlDec->yuv_buf_size = pDecConfig->YuvBufSize;

        Rval = AmbaHL_CmdDecoderStilldec(WriteMode, StlDec);
        HL_RelCmdBuffer(CmdBufferId);
    }

    if (Rval == OK) {
        HL_GetVidDecInfo(HL_MTX_OPT_GET, StreamIdx, &DecInfo);
        DecInfo.State = VIDDEC_STATE_RUN;
        DecInfo.BitsFormat = pDecConfig->BitsFormat;
        HL_SetVidDecInfo(HL_MTX_OPT_SET, StreamIdx, &DecInfo);
    } else {
        AmbaLL_LogUInt5("[Err][0x%X] StlDecStart", Rval, 0U, 0U, 0U, 0U);
    }

    return Rval;
}

/**
* Still decode stop function
* @return ErrorCode
*/
UINT32 dsp_still_dec_stop(void)
{
    UINT32 Rval = OK;

    return Rval;
}

static inline UINT32 HL_StillDecYuv2YuvChk(const AMBA_DSP_YUV_IMG_BUF_s *pSrcYuvBufAddr,
                                           const AMBA_DSP_YUV_IMG_BUF_s *pDestYuvBufAddr,
                                           const AMBA_DSP_STLDEC_YUV2YUV_s *pOperation)
{
    UINT32 Rval;
    const Dsp_DumpApi_t* DumpApi = AmbaDSP_GetDumpApiFunc();
    const Dsp_CheckApi_t* CheckApi = AmbaDSP_GetCheckApiFunc();
    const Dsp_ResourceApi_t* ResourceApi = AmbaDSP_GetResourceApiFunc();

    /* DSP CtxInit */
    Rval = HL_CtxInit();

    /* Timing sanity check */
    /* Input sanity check */
    if ((Rval == OK) &&
        (DumpApi != NULL)) {
        DumpApi->pStillDecYuv2Yuv(pSrcYuvBufAddr, pDestYuvBufAddr, pOperation);
    }
    if ((Rval == OK) &&
        (CheckApi != NULL)) {
        Rval = CheckApi->pStillDecYuv2Yuv(pSrcYuvBufAddr, pDestYuvBufAddr, pOperation);
    }
    /* HW/SW resources check */
    if ((Rval == OK) &&
        (ResourceApi != NULL)) {
        Rval = ResourceApi->pStillDecYuv2Yuv(pSrcYuvBufAddr, pDestYuvBufAddr, pOperation);
    }

    /* Logic sanity check */

    return Rval;
}

/**
* Still decode Yuv blending function
* @param [in]  pSrc1YuvBufAddr blend source1
* @param [in]  pSrc2YuvBufAddr blend source2
* @param [in]  pDestYuvBufAddr blend destination
* @param [in]  pOperation blending option
* @return ErrorCode
*/
UINT32 dsp_still_dec_yuv2yuv(const AMBA_DSP_YUV_IMG_BUF_s *pSrcYuvBufAddr,
                             const AMBA_DSP_YUV_IMG_BUF_s *pDestYuvBufAddr,
                             const AMBA_DSP_STLDEC_YUV2YUV_s *pOperation)
{
    UINT8 WriteMode = AMBA_DSP_CMD_VDSP_NORMAL_WRITE;
    UINT8 CmdBufferId = 0U; void *CmdBufferAddr = NULL;
//FIXME, DecIdx
    UINT16 DecIdx = 0U;
    UINT32 Rval;
    cmd_dproc_yuv_to_yuv_t *DprocY2Y = HL_DefCtxCmdBufPtrDpcY2y;

    Rval = HL_StillDecYuv2YuvChk(pSrcYuvBufAddr, pDestYuvBufAddr, pOperation);

    /* Body */
    if (Rval == OK) {
        Rval = HL_StillDecPreproc(DecIdx);
    }

    if (Rval == OK) {
        HL_AcqCmdBuffer(&CmdBufferId, &CmdBufferAddr);
        dsp_osal_typecast(&DprocY2Y, &CmdBufferAddr);

        DprocY2Y->user_data = 0U; //Obsolete
        DprocY2Y->feedback = 0U; //Obsolete
        DprocY2Y->flip = HL_GET_FLIP(pOperation->RotateFlip);
        DprocY2Y->rotate = HL_GET_ROTATE(pOperation->RotateFlip);
        if (pOperation->LumaGain != 0U) {
            INT32 Tmp = 0;
            (void)dsp_osal_memcpy(&Tmp, &pOperation->LumaGain, sizeof(UINT8));
            Tmp -= 128;
            (void)dsp_osal_memcpy(&DprocY2Y->luma_gain, &Tmp, sizeof(INT8));
        } else {
            DprocY2Y->luma_gain = 0;
        }

        DprocY2Y->src_pic.valid = (UINT8)1U;
        if (pSrcYuvBufAddr->DataFmt == AMBA_DSP_YUV420) {
            DprocY2Y->src_pic.ch_fmt = DSP_YUV_420;
        } else {
            DprocY2Y->src_pic.ch_fmt = DSP_YUV_422;
        }

        DprocY2Y->src_pic.buf_pitch = pSrcYuvBufAddr->Pitch;
        DprocY2Y->src_pic.buf_width = pSrcYuvBufAddr->Window.Width;
        DprocY2Y->src_pic.buf_height = pSrcYuvBufAddr->Window.Height;
        (void)dsp_osal_virt2cli(pSrcYuvBufAddr->BaseAddrY, &DprocY2Y->src_pic.lu_buf_base);
        (void)dsp_osal_virt2cli(pSrcYuvBufAddr->BaseAddrUV, &DprocY2Y->src_pic.ch_buf_base);
        DprocY2Y->src_pic.img_width = pSrcYuvBufAddr->Window.Width;
        DprocY2Y->src_pic.img_height = pSrcYuvBufAddr->Window.Height;
        DprocY2Y->src_pic.img_offset_x = pSrcYuvBufAddr->Window.OffsetX;
        DprocY2Y->src_pic.img_offset_y = pSrcYuvBufAddr->Window.OffsetY;

        /* Y2Y have 8X downscale HW limitation */
//FIXME
        DprocY2Y->dst_pic[0U].valid = (UINT8)1U;
        if (pDestYuvBufAddr->DataFmt == AMBA_DSP_YUV420) {
            DprocY2Y->dst_pic[0U].ch_fmt = DSP_YUV_420;
        } else {
            DprocY2Y->dst_pic[0U].ch_fmt = DSP_YUV_422;
        }
        DprocY2Y->dst_pic[0U].buf_pitch = pDestYuvBufAddr->Pitch;
        DprocY2Y->dst_pic[0U].buf_width = pDestYuvBufAddr->Window.Width;
        DprocY2Y->dst_pic[0U].buf_height = pDestYuvBufAddr->Window.Height;
        (void)dsp_osal_virt2cli(pDestYuvBufAddr->BaseAddrY, &DprocY2Y->dst_pic[0U].lu_buf_base);
        (void)dsp_osal_virt2cli(pDestYuvBufAddr->BaseAddrUV, &DprocY2Y->dst_pic[0U].ch_buf_base);
        DprocY2Y->dst_pic[0U].img_width = pDestYuvBufAddr->Window.Width;
        DprocY2Y->dst_pic[0U].img_height = pDestYuvBufAddr->Window.Height;
        DprocY2Y->dst_pic[0U].img_offset_x = pDestYuvBufAddr->Window.OffsetX;
        DprocY2Y->dst_pic[0U].img_offset_y = pDestYuvBufAddr->Window.OffsetY;

        Rval = AmbaHL_CmdDecProcYuvToYuv(WriteMode, DprocY2Y);
        HL_RelCmdBuffer(CmdBufferId);
    }

    if (Rval != OK) {
        AmbaLL_LogUInt5("[Err][0x%X] AmbaDSP_StillDecYuv2Yuv", Rval, 0U, 0U, 0U, 0U);
    }

    return Rval;
}

/**
* Still decode Yuv blending function
* @param [in]  pSrc1YuvBufAddr blend source1
* @param [in]  pSrc2YuvBufAddr blend source2
* @param [in]  pDestYuvBufAddr blend destination
* @param [in]  pOperation blending option
* @return ErrorCode
*/
UINT32 dsp_still_dec_yuv_blend(const AMBA_DSP_YUV_IMG_BUF_s *pSrc1YuvBufAddr,
                               const AMBA_DSP_YUV_IMG_BUF_s *pSrc2YuvBufAddr,
                               const AMBA_DSP_YUV_IMG_BUF_s *pDestYuvBufAddr,
                               const AMBA_DSP_STLDEC_BLEND_s *pOperation)
{
    UINT32 Rval;
    const Dsp_DumpApi_t* DumpApi = AmbaDSP_GetDumpApiFunc();
    const Dsp_CheckApi_t* CheckApi = AmbaDSP_GetCheckApiFunc();
    const Dsp_ResourceApi_t* ResourceApi = AmbaDSP_GetResourceApiFunc();
//FIXME, DecIdx
    UINT16 DecIdx = 0U;

    /* DSP CtxInit */
    Rval = HL_CtxInit();

    /* Timing sanity check */
    /* Input sanity check */
    if ((Rval == OK) &&
        (DumpApi != NULL)) {
        DumpApi->pStillDecYuvBlend(pSrc1YuvBufAddr, pSrc2YuvBufAddr, pDestYuvBufAddr, pOperation);
    }
    if ((Rval == OK) &&
        (CheckApi != NULL)) {
        Rval = CheckApi->pStillDecYuvBlend(pSrc1YuvBufAddr, pSrc2YuvBufAddr, pDestYuvBufAddr, pOperation);
    }
    /* HW/SW resources check */
    if ((Rval == OK) &&
        (ResourceApi != NULL)) {
        Rval = ResourceApi->pStillDecYuvBlend(pSrc1YuvBufAddr, pSrc2YuvBufAddr, pDestYuvBufAddr, pOperation);
    }

    /* Logic sanity check */

    /* Body */
    if (Rval == OK) {
        Rval = HL_StillDecPreproc(DecIdx);
    }

    if (Rval != OK) {
        AmbaLL_LogUInt5("[Err][0x%X] AmbaDSP_StillDecYuvBlend", Rval, 0U, 0U, 0U, 0U);
    }

    return Rval;
}

static inline UINT32 HL_StillDecDispYuvImgChk(UINT8 VoutIdx,
                                              const AMBA_DSP_YUV_IMG_BUF_s *pYuvBufAddr,
                                              const AMBA_DSP_VOUT_VIDEO_CFG_s *pVoutConfig)
{
    UINT32 Rval;
    const Dsp_DumpApi_t* DumpApi = AmbaDSP_GetDumpApiFunc();
    const Dsp_CheckApi_t* CheckApi = AmbaDSP_GetCheckApiFunc();
    const Dsp_ResourceApi_t* ResourceApi = AmbaDSP_GetResourceApiFunc();

    /* DSP CtxInit */
    Rval = HL_CtxInit();

    /* Timing sanity check */
    /* Input sanity check */
    if (DumpApi != NULL) {
        DumpApi->pStillDecDispYuvImg(VoutIdx, pYuvBufAddr, pVoutConfig);
    }
    if (CheckApi != NULL) {
        Rval = CheckApi->pStillDecDispYuvImg(VoutIdx, pYuvBufAddr, pVoutConfig);
    }
    /* HW/SW resources check */
    if ((Rval == OK) &&
        (ResourceApi != NULL)) {
        Rval = ResourceApi->pStillDecDispYuvImg(VoutIdx, pYuvBufAddr, pVoutConfig);
    }

    /* Logic sanity check */
    return Rval;
}

/**
* Still decode display yuv image function
* @param [in]  VoutIdx vout index
* @param [in]  pYuvBufAddr yuv biffer address
* @param [in]  pVoutConfig vout video plane configuration
* @return ErrorCode
*/
UINT32 dsp_still_dec_disp_yuv_img(UINT8 VoutIdx,
                                  const AMBA_DSP_YUV_IMG_BUF_s *pYuvBufAddr,
                                  const AMBA_DSP_VOUT_VIDEO_CFG_s *pVoutConfig)
{
    UINT8 WriteMode = AMBA_DSP_CMD_VDSP_NORMAL_WRITE;
    UINT8 CmdBufferId = 0U; void *CmdBufferAddr = NULL;
//FIXME, DecIdx
    UINT16 DecIdx = 0U;
    UINT32 Rval;
    CTX_VID_DEC_INFO_s DecInfo = {0};
    cmd_dproc_yuv_display_t *DprocY2Disp = HL_DefCtxCmdBufPtrDpcDisp;

    Rval = HL_StillDecDispYuvImgChk(VoutIdx, pYuvBufAddr, pVoutConfig);

    /* Body */
    if (Rval == OK) {
        Rval = HL_StillDecPreproc(DecIdx);
    }

    if (Rval == OK) {
        /* Prepare PostPCmd */
        HL_GetVidDecInfo(HL_MTX_OPT_GET, 0U/*DecIdx*/, &DecInfo);
        (void)dsp_osal_memcpy(&DecInfo.PostCtlCfg[VoutIdx].InputWindow, &pYuvBufAddr->Window, sizeof(AMBA_DSP_WINDOW_s));
        (void)dsp_osal_memcpy(&DecInfo.PostCtlCfg[VoutIdx].TargetWindow, &pYuvBufAddr->Window, sizeof(AMBA_DSP_WINDOW_s));
        DecInfo.PostCtlCfg[VoutIdx].TargetRotateFlip = AMBA_DSP_ROTATE_0;
        DecInfo.PostCtlCfg[VoutIdx].VoutIdx = VoutIdx;
        DecInfo.PostCtlCfg[VoutIdx].VoutRotateFlip = pVoutConfig->RotateFlip;
        (void)dsp_osal_memcpy(&DecInfo.PostCtlCfg[VoutIdx].VoutWindow, &pVoutConfig->Window, sizeof(AMBA_DSP_WINDOW_s));
        HL_SetVidDecInfo(HL_MTX_OPT_SET, 0U/*DecIdx*/, &DecInfo);

        Rval = HL_StlDecPostCtrlImpl(WriteMode, 0U/*DecIdx*/);
        if (Rval != OK) {
            AmbaLL_LogUInt5("[Err][0x%X][%d] SdecPostP", Rval, __LINE__, 0U, 0U, 0U);
        } else {
            HL_AcqCmdBuffer(&CmdBufferId, &CmdBufferAddr);
            dsp_osal_typecast(&DprocY2Disp, &CmdBufferAddr);
            DprocY2Disp->vout_id = VoutIdx;
            DprocY2Disp->vid_win_update = (UINT8)1U;
            DprocY2Disp->vid_win_rotate = HL_GET_ROTATE(pVoutConfig->RotateFlip);
            DprocY2Disp->vid_flip = HL_GET_FLIP(pVoutConfig->RotateFlip);
            DprocY2Disp->vid_win_width = pVoutConfig->Window.Width;
            DprocY2Disp->vid_win_height = pVoutConfig->Window.Height;
            DprocY2Disp->vid_win_offset_x = pVoutConfig->Window.OffsetX;
            DprocY2Disp->vid_win_offset_y = pVoutConfig->Window.OffsetY;

            DprocY2Disp->yuv_pic.valid = (UINT8)1U;
            if (pYuvBufAddr->DataFmt == AMBA_DSP_YUV420) {
                DprocY2Disp->yuv_pic.ch_fmt = DSP_YUV_420;
            } else {
                DprocY2Disp->yuv_pic.ch_fmt = DSP_YUV_422;
            }
            DprocY2Disp->yuv_pic.buf_pitch = pYuvBufAddr->Pitch;
            DprocY2Disp->yuv_pic.buf_width = pYuvBufAddr->Window.Width;
            DprocY2Disp->yuv_pic.buf_height = pYuvBufAddr->Window.Height;
            DprocY2Disp->yuv_pic.lu_buf_base = (UINT8)1U;
            DprocY2Disp->yuv_pic.ch_buf_base = (UINT8)1U;
            (void)dsp_osal_virt2cli(pYuvBufAddr->BaseAddrY, &DprocY2Disp->yuv_pic.lu_buf_base);
            (void)dsp_osal_virt2cli(pYuvBufAddr->BaseAddrUV, &DprocY2Disp->yuv_pic.ch_buf_base);
            DprocY2Disp->yuv_pic.img_width = pYuvBufAddr->Window.Width;
            DprocY2Disp->yuv_pic.img_height = pYuvBufAddr->Window.Height;
            DprocY2Disp->yuv_pic.img_offset_x = pYuvBufAddr->Window.OffsetX;
            DprocY2Disp->yuv_pic.img_offset_y = pYuvBufAddr->Window.OffsetY;

            Rval = AmbaHL_CmdDecProcYuvDisplay(WriteMode, DprocY2Disp);
            HL_RelCmdBuffer(CmdBufferId);
        }
    }

    if (Rval != OK) {
        AmbaLL_LogUInt5("[Err][0x%X] AmbaDSP_StillDecDispYuvImg", Rval, 0U, 0U, 0U, 0U);
    }


    return Rval;
}
