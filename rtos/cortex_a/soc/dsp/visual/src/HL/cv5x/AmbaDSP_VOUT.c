/**
 *  @file AmbaDSP_VOUT.c
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
 *  @details Implementation of SSP video output API
 *
 */

#include "AmbaDSP_Log.h"
#include "AmbaDSP_Cmds.h"
#include "AmbaDSP_ArmComm.h"
#include "AmbaDSP_ApiDump.h"
#include "AmbaDSP_ApiCheck.h"
#include "AmbaDSP_ApiTiming.h"
#include "AmbaDSP_EncodeAPI.h"
#include "AmbaDSP_ApiResource.h"
#include "AmbaDSP_EncodeUtility.h"
#include "dsp_priv_api.h"
#include "ambadsp_ioctl.h"
#include "AmbaDSP_Buffers.h"

static inline UINT32 HL_VoutResetChk(const UINT8 VoutIdx)
{
    UINT32 Rval;
    const Dsp_TimingApi_t* TimingApi = AmbaDSP_GetTimingApiFunc();
    const Dsp_DumpApi_t* DumpApi = AmbaDSP_GetDumpApiFunc();
    const Dsp_CheckApi_t* CheckApi = AmbaDSP_GetCheckApiFunc();
    const Dsp_ResourceApi_t* ResourceApi = AmbaDSP_GetResourceApiFunc();

    /* DSP CtxInit */
    Rval = HL_CtxInit();

    /* Timing sanity check */
    if ((Rval == OK) &&
        (TimingApi != NULL)) {
        Rval = TimingApi->pVoutReset(VoutIdx);
    }
    /* Input sanity check */
    if ((Rval == OK) &&
        (DumpApi != NULL)) {
        DumpApi->pVoutReset(VoutIdx);
    }
    if ((Rval == OK) &&
        (CheckApi != NULL)) {
        Rval = CheckApi->pVoutReset(VoutIdx);
    }
    /* HW/SW resources check */
    if ((Rval == OK) &&
        (ResourceApi != NULL)) {
        Rval = ResourceApi->pVoutReset(VoutIdx);
    }

    /* Logic sanity check */

    return Rval;
}

/**
* Vout reset function
* @param [in]  VoutIdx vout index
* @return ErrorCode
*/
UINT32 dsp_vout_reset(const UINT8 VoutIdx)
{
    UINT32 Rval;

    Rval = HL_VoutResetChk(VoutIdx);

    if (Rval == OK) {
        CTX_VOUT_INFO_s VoutInfo = {0};

        HL_GetVoutInfo(HL_MTX_OPT_ALL, VoutIdx, &VoutInfo);
        if ((DSP_PROF_STATUS_INVALID != DSP_GetProfState()) &&
            (DSP_VOUT_DISPLAY_START == VoutInfo.DisplayStatus)) {

            /* Switch source to BG, to prevent error */
            if ((VoutInfo.VideoEnable == 1U) && (VoutInfo.Source != DSP_VOUT_SRC_BG)) {
                Rval = HL_VoutVideoSetupSourceSelect((UINT8)VoutIdx, DSP_VOUT_SRC_BG, HL_VOUT_SOURCE_NONE);
                if (Rval == OK) {
                    Rval = HL_VoutVideoSetup((UINT8)VoutIdx, AMBA_DSP_CMD_VDSP_NORMAL_WRITE);
                    /* 19.10.15, no need to wait one sync, if doing reset together */
#ifndef SUPPORT_VOUT_BG_ONE_SYNC_RESET
                    if (Rval == OK) {
                        Rval = DSP_WaitVdspEvent(EVENT_FLAG_INT_VDSP0_EVENT, 1, WAIT_INTERRUPT_TIMEOUT);
                        if (Rval != OK) {
                            AmbaLL_LogUInt5("[Err][0x%X] Can't wait for VDSP EVENT", Rval, 0U, 0U, 0U, 0U);
                            Rval = DSP_ERR_0003;
                        }
                    }
#endif
                }
            }

            if (Rval == OK) {
                UINT8 CmdBufferId = 0U;
                void *CmdBufferAddr = NULL;
                cmd_vout_reset_t *pVoutReset = HL_DefCtxCmdBufPtrVoutReset;
                UINT16 VoutPhysId = 0U;

                HL_AcqCmdBuffer(&CmdBufferId, &CmdBufferAddr);
                dsp_osal_typecast(&pVoutReset, &CmdBufferAddr);
                (void)HL_GetVoutPhysId(VoutIdx, &VoutPhysId);
                pVoutReset->vout_id = (UINT8)VoutPhysId;
                pVoutReset->virtual_chan_id = 0U; // DONT CARE
                pVoutReset->reset_mixer = 1U;
                pVoutReset->reset_disp = 1U;

                Rval = AmbaHL_CmdVoutReset(AMBA_DSP_CMD_VDSP_NORMAL_WRITE, pVoutReset);
                HL_RelCmdBuffer(CmdBufferId);
                if (Rval == OK) {
                    HL_GetVoutInfo(HL_MTX_OPT_GET, VoutIdx, &VoutInfo);
                    VoutInfo.DisplayStatus = DSP_VOUT_DISPLAY_RESET;
                    VoutInfo.DveStatus = DSP_VOUT_DVE_RESET;
                    HL_SetVoutInfo(HL_MTX_OPT_SET, VoutIdx, &VoutInfo);
                }
            }
        /* DSP_PROF_STATUS_INVALID,
         * Or vout not enabled */
        } else {
            if (DSP_VOUT_DISPLAY_RESET != VoutInfo.DisplayStatus) {
                HL_GetVoutInfo(HL_MTX_OPT_GET, VoutIdx, &VoutInfo);
                VoutInfo.DisplayStatus = DSP_VOUT_DISPLAY_NO_INIT;
                VoutInfo.DveStatus = DSP_VOUT_DVE_NO_INIT;
                HL_SetVoutInfo(HL_MTX_OPT_SET, VoutIdx, &VoutInfo);
            }
        }
    }

    if (Rval != OK) {
        AmbaLL_LogUInt5("AmbaDSP_VoutReset[%d] Error: %d", VoutIdx, Rval, 0U, 0U, 0U);
    }

    return Rval;
}

static inline UINT32 HL_VoutMixerConfigChk(const UINT8 VoutIdx, const AMBA_DSP_VOUT_MIXER_CONFIG_s *pConfig)
{
    UINT32 Rval;
    const Dsp_TimingApi_t* TimingApi = AmbaDSP_GetTimingApiFunc();
    const Dsp_DumpApi_t* DumpApi = AmbaDSP_GetDumpApiFunc();
    const Dsp_CheckApi_t* CheckApi = AmbaDSP_GetCheckApiFunc();

    /* DSP CtxInit */
    Rval = HL_CtxInit();

    /* Timing sanity check */
    if ((Rval == OK) &&
        (TimingApi != NULL)) {
        Rval = TimingApi->pVoutMixerConfig(VoutIdx, pConfig);
    }
    /* Input sanity check */
    if ((Rval == OK) &&
        (DumpApi != NULL)) {
        DumpApi->pVoutMixerConfig(VoutIdx, pConfig);
    }
    if ((Rval == OK) &&
        (CheckApi != NULL)) {
        Rval = CheckApi->pVoutMixerConfig(VoutIdx, pConfig);
    }

    return Rval;
}

/**
* Vout Mixer config function
* @param [in]  VoutIdx vout index
* @param [in]  pConfig configuration information
* @return ErrorCode
*/
UINT32 dsp_vout_mixer_cfg(const UINT8 VoutIdx, const AMBA_DSP_VOUT_MIXER_CONFIG_s *pConfig)
{
    UINT32 Rval;

    Rval = HL_VoutMixerConfigChk(VoutIdx, pConfig);

    if (Rval == OK) {
        CTX_VOUT_INFO_s VoutInfo = {0};

        HL_GetVoutInfo(HL_MTX_OPT_GET, VoutIdx, &VoutInfo);
        (void)dsp_osal_memcpy(&VoutInfo.MixerCfg, pConfig, sizeof(AMBA_DSP_VOUT_MIXER_CONFIG_s));
        HL_SetVoutInfo(HL_MTX_OPT_SET, VoutIdx, &VoutInfo);
    }

    if (Rval != OK) {
        AmbaLL_LogUInt5("AmbaDSP_VoutMixerConfig[%d] Error: %d", VoutIdx, Rval, 0U, 0U, 0U);
    }

    return Rval;
}

static inline UINT32 HL_VoutMixerConfigBackColorChk(const UINT8 VoutIdx, UINT32 BackColorYUV)
{
    UINT32 Rval;
    const Dsp_TimingApi_t* TimingApi = AmbaDSP_GetTimingApiFunc();
    const Dsp_DumpApi_t* DumpApi = AmbaDSP_GetDumpApiFunc();
    const Dsp_CheckApi_t* CheckApi = AmbaDSP_GetCheckApiFunc();

    /* DSP CtxInit */
    Rval = HL_CtxInit();

    /* Timing sanity check */
    if ((Rval == OK) &&
        (TimingApi != NULL)) {
        Rval = TimingApi->pVoutMixerConfigBackColor(VoutIdx, BackColorYUV);
    }
    /* Input sanity check */
    if ((Rval == OK) &&
        (DumpApi != NULL)) {
        DumpApi->pVoutMixerConfigBackColor(VoutIdx, BackColorYUV);
    }
    if ((Rval == OK) &&
        (CheckApi != NULL)) {
        Rval = CheckApi->pVoutMixerConfigBackColor(VoutIdx, BackColorYUV);
    }

    return Rval;
}

/**
* Vout Mixer background color config function
* @param [in]  VoutIdx vout index
* @param [in]  BackColorYUV background color
* @return ErrorCode
*/
UINT32 dsp_vout_mixer_cfg_bgcolor(const UINT8 VoutIdx, UINT32 BackColorYUV)
{
    UINT32 Rval;

    Rval = HL_VoutMixerConfigBackColorChk(VoutIdx, BackColorYUV);

    if (Rval == OK) {
        CTX_VOUT_INFO_s VoutInfo = {0};

        HL_GetVoutInfo(HL_MTX_OPT_GET, VoutIdx, &VoutInfo);
        VoutInfo.MixerBGColor = BackColorYUV;
        HL_SetVoutInfo(HL_MTX_OPT_SET, VoutIdx, &VoutInfo);
    }

    if (Rval != OK) {
        AmbaLL_LogUInt5("AmbaDSP_VoutMixerConfigBackColor[%d] Error: %d", VoutIdx, Rval, 0U, 0U, 0U);
    }

    return Rval;
}

static inline UINT32 HL_VoutMixerConfigHighlightColorChk(const UINT8 VoutIdx, UINT8 LumaThreshold, UINT32 HighlightColorYUV)
{
    UINT32 Rval;
    const Dsp_TimingApi_t* TimingApi = AmbaDSP_GetTimingApiFunc();
    const Dsp_DumpApi_t* DumpApi = AmbaDSP_GetDumpApiFunc();
    const Dsp_CheckApi_t* CheckApi = AmbaDSP_GetCheckApiFunc();

    /* DSP CtxInit */
    Rval = HL_CtxInit();

    /* Timing sanity check */
    if ((Rval == OK) &&
        (TimingApi != NULL)) {
        Rval = TimingApi->pVoutMixerConfigHighlightColor(VoutIdx, LumaThreshold, HighlightColorYUV);
    }
    /* Input sanity check */
    if ((Rval == OK) &&
        (DumpApi != NULL)) {
        DumpApi->pVoutMixerConfigHighlightColor(VoutIdx, LumaThreshold, HighlightColorYUV);
    }
    if ((Rval == OK) &&
        (CheckApi != NULL)) {
        Rval = CheckApi->pVoutMixerConfigHighlightColor(VoutIdx, LumaThreshold, HighlightColorYUV);
    }

    return Rval;
}

/**
* Vout Mixer Highlight config function
* @param [in]  VoutIdx vout index
* @param [in]  LumaThreshold highlight threshold
* @param [in]  HighlightColorYUV hightlight color
* @return ErrorCode
*/
UINT32 dsp_vout_mixer_cfg_highcolor(const UINT8 VoutIdx, UINT8 LumaThreshold, UINT32 HighlightColorYUV)
{
    UINT32 Rval;
    CTX_VOUT_INFO_s VoutInfo = {0};

    Rval = HL_VoutMixerConfigHighlightColorChk(VoutIdx, LumaThreshold, HighlightColorYUV);

    if (Rval == OK) {
        HL_GetVoutInfo(HL_MTX_OPT_GET, VoutIdx, &VoutInfo);
        VoutInfo.MixerHighlightThreshold = LumaThreshold;
        VoutInfo.MixerHighlightColor = HighlightColorYUV;
        HL_SetVoutInfo(HL_MTX_OPT_SET, VoutIdx, &VoutInfo);
    }

    if (Rval != OK) {
        AmbaLL_LogUInt5("AmbaDSP_VoutMixerConfigHighLightColor[%d] Error: %d", VoutIdx, Rval, 0U, 0U, 0U);
    }

    return Rval;
}

static inline UINT32 HL_VoutMixerConfigCscChk(const UINT8 VoutIdx, const UINT8 CscCtrl)
{
    UINT32 Rval;
    const Dsp_TimingApi_t* TimingApi = AmbaDSP_GetTimingApiFunc();
    const Dsp_DumpApi_t* DumpApi = AmbaDSP_GetDumpApiFunc();
    const Dsp_CheckApi_t* CheckApi = AmbaDSP_GetCheckApiFunc();

    /* DSP CtxInit */
    Rval = HL_CtxInit();

    /* Timing sanity check */
    if ((Rval == OK) &&
        (TimingApi != NULL)) {
        Rval = TimingApi->pVoutMixerConfigCsc(VoutIdx, CscCtrl);
    }
    /* Input sanity check */
    if ((Rval == OK) &&
        (DumpApi != NULL)) {
        DumpApi->pVoutMixerConfigCsc(VoutIdx, CscCtrl);
    }
    if ((Rval == OK) &&
        (CheckApi != NULL)) {
        Rval = CheckApi->pVoutMixerConfigCsc(VoutIdx, CscCtrl);
    }

    return Rval;
}

/**
* Vout Mixer Csc config function
* @param [in]  VoutIdx vout index
* @param [in]  CscCtrl Csc control option
* @return ErrorCode
*/
UINT32 dsp_vout_mixer_cfg_csc(const UINT8 VoutIdx, const UINT8 CscCtrl)
{
    UINT32 Rval;

    Rval = HL_VoutMixerConfigCscChk(VoutIdx, CscCtrl);

    if (Rval == OK) {
        CTX_VOUT_INFO_s VoutInfo = {0};

        HL_GetVoutInfo(HL_MTX_OPT_GET, VoutIdx, &VoutInfo);
        VoutInfo.MixerCscCtrl = CscCtrl;
        HL_SetVoutInfo(HL_MTX_OPT_SET, VoutIdx, &VoutInfo);
    }

    if (Rval != OK) {
        AmbaLL_LogUInt5("AmbaDSP_VoutMixerConfigCsc[%d] Error: %d", VoutIdx, Rval, 0U, 0U, 0U);
    }

    return Rval;
}

static inline UINT32 HL_VoutMixerCscMatrixConfigChk(const UINT8 VoutIdx, const dsp_vout_csc_matrix_s *pCscMatrix)
{
    UINT32 Rval;
    const Dsp_TimingApi_t* TimingApi = AmbaDSP_GetTimingApiFunc();
    const Dsp_DumpApi_t* DumpApi = AmbaDSP_GetDumpApiFunc();
    const Dsp_CheckApi_t* CheckApi = AmbaDSP_GetCheckApiFunc();

    /* DSP CtxInit */
    Rval = HL_CtxInit();

    /* Timing sanity check */
    if ((Rval == OK) &&
        (TimingApi != NULL)) {
        Rval = TimingApi->pVoutMixerCscMatrixConfig(VoutIdx, pCscMatrix);
    }
    /* Input sanity check */
    if ((Rval == OK) &&
        (DumpApi != NULL)) {
        DumpApi->pVoutMixerCscMatrixConfig(VoutIdx, pCscMatrix);
    }
    if ((Rval == OK) &&
        (CheckApi != NULL)) {
        Rval = CheckApi->pVoutMixerCscMatrixConfig(VoutIdx, pCscMatrix);
    }

    return Rval;
}

/**
* Vout mixer CSC matrix config
* @param [in]  VoutIdx vout index
* @param [in]  pCscMatrix CSC matrix
* @return ErrorCode
*/
UINT32 dsp_vout_mixer_csc_matrix_cfg(const UINT8 VoutIdx, const dsp_vout_csc_matrix_s *pCscMatrix)
{
    UINT32 Rval;

    Rval = HL_VoutMixerCscMatrixConfigChk(VoutIdx, pCscMatrix);

    if (Rval == OK) {
typedef struct {
    UINT32  CoefA0:                 13;
    UINT32  Reserved:               3;
    UINT32  CoefA1:                 13;
    UINT32  Reserved1:              3;
} VOUTM_CSC_PARAM_0_REG_s;

typedef struct {
    UINT32  CoefA2:                 13;
    UINT32  Reserved:               3;
    UINT32  CoefA3:                 13;
    UINT32  Reserved1:              3;
} VOUTM_CSC_PARAM_1_REG_s;

typedef struct {
    UINT32  CoefA4:                 13;
    UINT32  Reserved:               3;
    UINT32  CoefA5:                 13;
    UINT32  Reserved1:              3;
} VOUTM_CSC_PARAM_2_REG_s;

typedef struct {
    UINT32  CoefA6:                 13;
    UINT32  Reserved:               3;
    UINT32  CoefA7:                 13;
    UINT32  Reserved1:              3;
} VOUTM_CSC_PARAM_3_REG_s;

typedef struct {
    UINT32  CoefA8:                 13;
    UINT32  Reserved:               3;
    UINT32  ConstB0:                11;
    UINT32  Reserved1:              5;
} VOUTM_CSC_PARAM_4_REG_s;

typedef struct {
    UINT32  ConstB1:                11;
    UINT32  Reserved:               5;
    UINT32  ConstB2:                11;
    UINT32  Reserved1:              5;
} VOUTM_CSC_PARAM_5_REG_s;

typedef struct {
    UINT32  Output0ClampLow:        8;
    UINT32  Reserved:               8;
    UINT32  Output0ClampHigh:       8;
    UINT32  Reserved1:              8;
} VOUTM_CSC_PARAM_6_REG_s;

typedef struct {
    UINT32  Output1ClampLow:        8;
    UINT32  Reserved:               8;
    UINT32  Output1ClampHigh:       8;
    UINT32  Reserved1:              8;
} VOUTM_CSC_PARAM_7_REG_s;

typedef struct {
    UINT32  Output2ClampLow:        8;
    UINT32  Reserved:               8;
    UINT32  Output2ClampHigh:       8;
    UINT32  Reserved1:              8;
} VOUTM_CSC_PARAM_8_REG_s;

typedef struct {
    VOUTM_CSC_PARAM_0_REG_s CSC0; //AMBA_VOUTM_CSC_PARAM_0_REG_s
    VOUTM_CSC_PARAM_1_REG_s CSC1; //AMBA_VOUTM_CSC_PARAM_1_REG_s
    VOUTM_CSC_PARAM_2_REG_s CSC2; //AMBA_VOUTM_CSC_PARAM_2_REG_s
    VOUTM_CSC_PARAM_3_REG_s CSC3; //AMBA_VOUTM_CSC_PARAM_3_REG_s
    VOUTM_CSC_PARAM_4_REG_s CSC4; //AMBA_VOUTM_CSC_PARAM_4_REG_s
    VOUTM_CSC_PARAM_5_REG_s CSC5; //AMBA_VOUTM_CSC_PARAM_5_REG_s
    VOUTM_CSC_PARAM_6_REG_s CSC6; //AMBA_VOUTM_CSC_PARAM_6_REG_s
    VOUTM_CSC_PARAM_7_REG_s CSC7; //AMBA_VOUTM_CSC_PARAM_7_REG_s
    VOUTM_CSC_PARAM_8_REG_s CSC8; //AMBA_VOUTM_CSC_PARAM_8_REG_s
} DSP_VOUT_CSC;
        CTX_VOUT_INFO_s VoutInfo = {0};
        DSP_VOUT_CSC Csc = {0};

        HL_GetVoutInfo(HL_MTX_OPT_GET, VoutIdx, &VoutInfo);
        Csc.CSC0.CoefA0 = (UINT16)pCscMatrix->Coef[0U][0U];
        Csc.CSC0.CoefA1 = (UINT16)pCscMatrix->Coef[0U][1U];
        Csc.CSC1.CoefA2 = (UINT16)pCscMatrix->Coef[0U][2U];
        Csc.CSC1.CoefA3 = (UINT16)pCscMatrix->Coef[1U][0U];
        Csc.CSC2.CoefA4 = (UINT16)pCscMatrix->Coef[1U][1U];
        Csc.CSC2.CoefA5 = (UINT16)pCscMatrix->Coef[1U][2U];
        Csc.CSC3.CoefA6 = (UINT16)pCscMatrix->Coef[2U][0U];
        Csc.CSC3.CoefA7 = (UINT16)pCscMatrix->Coef[2U][1U];
        Csc.CSC4.CoefA8 = (UINT16)pCscMatrix->Coef[2U][2U];
        Csc.CSC4.ConstB0 = (UINT16)pCscMatrix->Offset[0U];
        Csc.CSC5.ConstB1 = (UINT16)pCscMatrix->Offset[1U];
        Csc.CSC5.ConstB2 = (UINT16)pCscMatrix->Offset[2U];
        Csc.CSC6.Output0ClampLow  = (UINT8)pCscMatrix->MinVal[0U];
        Csc.CSC6.Output0ClampHigh = (UINT8)pCscMatrix->MaxVal[0U];
        Csc.CSC7.Output1ClampLow  = (UINT8)pCscMatrix->MinVal[1U];
        Csc.CSC7.Output1ClampHigh = (UINT8)pCscMatrix->MaxVal[1U];
        Csc.CSC8.Output2ClampLow  = (UINT8)pCscMatrix->MinVal[2U];
        Csc.CSC8.Output2ClampHigh = (UINT8)pCscMatrix->MaxVal[2U];
        (void)dsp_osal_memcpy(VoutInfo.CscMatrix, &Csc, sizeof(UINT32)*DSP_VOUT_CSC_MATRIX_DEPTH);
        HL_SetVoutInfo(HL_MTX_OPT_SET, VoutIdx, &VoutInfo);
    }

    if (Rval != OK) {
        AmbaLL_LogUInt5("AmbaDSP_VoutMixerCscMatrixConfig[%d] Error: %d", VoutIdx, Rval, 0U, 0U, 0U);
    }

    return Rval;
}

static inline UINT32 HL_VoutMixerCtrlChk(const UINT8 VoutIdx)
{
    UINT32 Rval;
    const Dsp_TimingApi_t* TimingApi = AmbaDSP_GetTimingApiFunc();
    const Dsp_DumpApi_t* DumpApi = AmbaDSP_GetDumpApiFunc();
    const Dsp_CheckApi_t* CheckApi = AmbaDSP_GetCheckApiFunc();
    const Dsp_ResourceApi_t* ResourceApi = AmbaDSP_GetResourceApiFunc();

    /* DSP CtxInit */
    Rval = HL_CtxInit();

    /* Timing sanity check */
    if ((Rval == OK) &&
        (TimingApi != NULL)) {
        Rval = TimingApi->pVoutMixerCtrl(VoutIdx);
    }
    /* Input sanity check */
    if ((Rval == OK) &&
        (DumpApi != NULL)) {
        DumpApi->pVoutMixerCtrl(VoutIdx);
    }
    if ((Rval == OK) &&
        (CheckApi != NULL)) {
        Rval = CheckApi->pVoutMixerCtrl(VoutIdx);
    }
    /* HW/SW resources check */
    if ((Rval == OK) &&
        (ResourceApi != NULL)) {
        Rval = ResourceApi->pVoutMixerCtrl(VoutIdx);
    }

    return Rval;
}

/**
* Vout Mixer function
* @param [in]  VoutIdx vout index
* @param [in]  Enable enable flag
* @param [in]  SyncWithVin control command sync with vin or not
* @param [out] pAttachedRawSeq attached raw sequence
* @return ErrorCode
*/
UINT32 dsp_vout_mixer_ctrl(const UINT8 VoutIdx)
{
    UINT32 Rval;
    UINT16 i;
    UINT8 ExitLoop;
    AMBA_DSP_VOUT_MIXER_CSC_DATA_s *pVoutMixerCsc = NULL;

    Rval = HL_VoutMixerCtrlChk(VoutIdx);

    if (Rval == OK) {
        UINT32 FrameRate = 0U;
        CTX_VOUT_INFO_s VoutInfo = {0};

        HL_GetVoutInfo(HL_MTX_OPT_GET, VoutIdx, &VoutInfo);

        if ((DSP_PROF_STATUS_INVALID != DSP_GetProfState()) &&
            (DSP_VOUT_DISPLAY_NO_INIT != VoutInfo.DisplayStatus)) {
            UINT8 CmdBufferId = 0U;
            void *CmdBufferAddr = NULL;
            cmd_vout_mixer_setup_t *pVoutMixerSetup = HL_DefCtxCmdBufPtrVoutMx;
            UINT16 VoutPhysId = 0U, VoutVirtId = 0U;
            UINT8 InFmt, OutFmt;

            HL_AcqCmdBuffer(&CmdBufferId, &CmdBufferAddr);
            dsp_osal_typecast(&pVoutMixerSetup, &CmdBufferAddr);
            (void)HL_GetVoutPhysId(VoutIdx, &VoutPhysId);
            (void)HL_GetVoutVirtId(VoutIdx, &VoutVirtId);
            pVoutMixerSetup->vout_id = VoutPhysId;
            pVoutMixerSetup->virtual_chan_id = VoutVirtId;
            pVoutMixerSetup->reverse_en = VoutInfo.MixerCfg.VideoHorReverseEnable;
            (void)HL_FRAME_RATE_MAP(VOUT_FRAME_RATE, VoutInfo.MixerCfg.FrameRate, &FrameRate);
            pVoutMixerSetup->frm_rate = (UINT8)FrameRate;
            pVoutMixerSetup->interlaced = VoutInfo.MixerCfg.FrameRate.Interlace;
            pVoutMixerSetup->act_win_width = VoutInfo.MixerCfg.ActiveWidth;
            pVoutMixerSetup->act_win_height = VoutInfo.MixerCfg.ActiveHeight;
            pVoutMixerSetup->back_ground_v = HL_VOUT_GET_YUV(VOUT_GET_V, VoutInfo.MixerBGColor);
            pVoutMixerSetup->back_ground_u = HL_VOUT_GET_YUV(VOUT_GET_U, VoutInfo.MixerBGColor);
            pVoutMixerSetup->back_ground_y = HL_VOUT_GET_YUV(VOUT_GET_Y, VoutInfo.MixerBGColor);
            InFmt = (UINT8)DSP_GetU8Bit(VoutInfo.MixerCfg.MixerColorFormat, 0U, 4U);
            OutFmt = (UINT8)DSP_GetU8Bit(VoutInfo.MixerCfg.MixerColorFormat, 4U, 4U);
            pVoutMixerSetup->mixer_444 = InFmt;
            pVoutMixerSetup->mixer_420 = OutFmt;
            pVoutMixerSetup->highlight_v = HL_VOUT_GET_YUV(VOUT_GET_V, VoutInfo.MixerHighlightColor);
            pVoutMixerSetup->highlight_u = HL_VOUT_GET_YUV(VOUT_GET_U, VoutInfo.MixerHighlightColor);
            pVoutMixerSetup->highlight_y = HL_VOUT_GET_YUV(VOUT_GET_Y, VoutInfo.MixerHighlightColor);
            pVoutMixerSetup->highlight_thresh = VoutInfo.MixerHighlightThreshold;
//FIXME, only for OSD
//            VoutMixerSetup.csc_en = VoutInfo.MixerCscCtrl;

            /* Set default CSC if user leave it as blank */
            if ((VoutInfo.MixerCscCtrl == MIXER_CSC_FOR_VIDEO) ||
                (VoutInfo.MixerCscCtrl == MIXER_CSC_FOR_OSD)) {
                ExitLoop = 0U;
                for (i=0U; i<DSP_VOUT_CSC_MATRIX_DEPTH; i++) {
                    if (VoutInfo.CscMatrix[i] != 0U) {
                        ExitLoop = 1U;
                        break;
                    }
                }

                if (ExitLoop == 0U) {
                    //No CSC matrix, leverage Display's setting
                    AmbaLL_LogUInt5("Null MixerCscMatrix[%d] use Display one", VoutIdx, 0U, 0U, 0U, 0U);
                    DSP_GetDspVoutMixerCscCfg(VoutIdx, &pVoutMixerCsc);
                    (void)dsp_osal_memcpy(pVoutMixerSetup->csc_parms, pVoutMixerCsc, sizeof(UINT32)*DSP_VOUT_CSC_MATRIX_DEPTH);
                } else {
                    (void)dsp_osal_memcpy(pVoutMixerSetup->csc_parms, VoutInfo.CscMatrix, sizeof(UINT32)*DSP_VOUT_CSC_MATRIX_DEPTH);
                }
            } else {
                //No CSC Matrix needed
            }

//FIXME, only for OSD
            pVoutMixerSetup->csc_en = MIXER_CSC_FOR_OSD;

            Rval = AmbaHL_CmdVoutMixerSetup(AMBA_DSP_CMD_VDSP_NORMAL_WRITE, pVoutMixerSetup);
            HL_RelCmdBuffer(CmdBufferId);
            if (Rval == OK) {
                VoutInfo.MixerStatus = DSP_VOUT_MIXER_START;
            }
        } else {
            VoutInfo.MixerStatus = DSP_VOUT_MIXER_NO_INIT;
        }
        HL_SetVoutInfo(HL_MTX_OPT_SET, VoutIdx, &VoutInfo);
    }

    if (Rval != OK) {
        AmbaLL_LogUInt5("AmbaDSP_VoutMixerCtrl[%d] Error: %d", VoutIdx, Rval, 0U, 0U, 0U);
    }

    return Rval;
}

static inline UINT32 HL_VoutOsdConfigBufChk(const UINT8 VoutIdx, const AMBA_DSP_VOUT_OSD_BUF_CONFIG_s *pBufConfig)
{
    UINT32 Rval;
    const Dsp_TimingApi_t* TimingApi = AmbaDSP_GetTimingApiFunc();
    const Dsp_DumpApi_t* DumpApi = AmbaDSP_GetDumpApiFunc();
    const Dsp_CheckApi_t* CheckApi = AmbaDSP_GetCheckApiFunc();

    /* DSP CtxInit */
    Rval = HL_CtxInit();

    /* Timing sanity check */
    if ((Rval == OK) &&
        (TimingApi != NULL)) {
        Rval = TimingApi->pVoutOsdConfigBuf(VoutIdx, pBufConfig);
    }
    /* Input sanity check */
    if ((Rval == OK) &&
        (DumpApi != NULL)) {
        DumpApi->pVoutOsdConfigBuf(VoutIdx, pBufConfig);
    }
    if ((Rval == OK) &&
        (CheckApi != NULL)) {
        Rval = CheckApi->pVoutOsdConfigBuf(VoutIdx, pBufConfig);
    }

    /* Logic sanity check */
    return Rval;
}

/**
* Vout OSD buffer config function
* @param [in]  VoutIdx vout index
* @param [in]  Enable enable flag
* @param [in]  SyncWithVin control command sync with vin or not
* @param [out] pAttachedRawSeq attached raw sequence
* @return ErrorCode
*/
UINT32 dsp_vout_osd_cfg_buf(const UINT8 VoutIdx, const AMBA_DSP_VOUT_OSD_BUF_CONFIG_s *pBufConfig)
{
    UINT32 Rval;

    Rval = HL_VoutOsdConfigBufChk(VoutIdx, pBufConfig);

    if (Rval == OK) {
        CTX_VOUT_INFO_s VoutInfo = {0};
        UINT16 i, ScalerEn = 0U;

        /* Check if there are more than one scaler usage */
        if ((pBufConfig->InputWidth != pBufConfig->Window.Width) ||
            (pBufConfig->InputHeight != pBufConfig->Window.Height)) {
            ScalerEn = 1U;
        }
        for (i = 0U; i < NUM_VOUT_IDX; i++) {
            if (i == VoutIdx) {
                continue;
            }
            HL_GetVoutInfo(HL_MTX_OPT_ALL, i, &VoutInfo);
            if (((VoutInfo.OsdCfg.InputWidth != VoutInfo.OsdCfg.Window.Width) ||
                (VoutInfo.OsdCfg.InputHeight != VoutInfo.OsdCfg.Window.Height)) && (ScalerEn == 1U)) {
                Rval = DSP_ERR_0003;
                break;
            }
        }

        if (Rval == OK) {
            HL_GetVoutInfo(HL_MTX_OPT_GET, VoutIdx, &VoutInfo);
            (void)dsp_osal_memcpy(&VoutInfo.OsdCfg, pBufConfig, sizeof(AMBA_DSP_VOUT_OSD_BUF_CONFIG_s));
            HL_SetVoutInfo(HL_MTX_OPT_SET, VoutIdx, &VoutInfo);
        }
    }

    if (Rval != OK) {
        AmbaLL_LogUInt5("AmbaDSP_VoutOsdConfigBuf[%d] Error: %d", VoutIdx, Rval, 0U, 0U, 0U);
    }

    return Rval;
}

static inline UINT32 HL_VoutOsdCtrlChk(const UINT8 VoutIdx,
                                       const UINT8 Enable,
                                       const UINT8 SyncWithVin,
                                       const UINT64 *pAttachedRawSeq)
{
    UINT32 Rval;
    const Dsp_TimingApi_t* TimingApi = AmbaDSP_GetTimingApiFunc();
    const Dsp_DumpApi_t* DumpApi = AmbaDSP_GetDumpApiFunc();
    const Dsp_CheckApi_t* CheckApi = AmbaDSP_GetCheckApiFunc();
    const Dsp_ResourceApi_t* ResourceApi = AmbaDSP_GetResourceApiFunc();

    (void)pAttachedRawSeq;

    /* DSP CtxInit */
    Rval = HL_CtxInit();

    /* Timing sanity check */
    if ((Rval == OK) &&
        (TimingApi != NULL)) {
        Rval = TimingApi->pVoutOsdCtrl(VoutIdx, Enable, SyncWithVin, pAttachedRawSeq);
    }
    /* Input sanity check */
    if ((Rval == OK) &&
        (DumpApi != NULL)) {
        DumpApi->pVoutOsdCtrl(VoutIdx, Enable, SyncWithVin, pAttachedRawSeq);
    }
    if ((Rval == OK) &&
        (CheckApi != NULL)) {
        Rval = CheckApi->pVoutOsdCtrl(VoutIdx, Enable, SyncWithVin, pAttachedRawSeq);
    }
    /* HW/SW resources check */
    if ((Rval == OK) &&
        (ResourceApi != NULL)) {
        Rval = ResourceApi->pVoutOsdCtrl(VoutIdx, Enable, SyncWithVin, pAttachedRawSeq);
    }

    /* Logic sanity check */


    return Rval;
}

static inline void HL_VoutSweepYuv(UINT8 SyncWithVin, UINT8 *pIsEfctSync, UINT16 *pYuvStrmIdx)
{
    UINT16 i;
    CTX_RESOURCE_INFO_s *Resource = HL_CtxResInfoPtr;
    CTX_YUV_STRM_INFO_s *YuvStrmInfo = HL_CtxYuvStrmInfoPtr;
    CTX_YUVSTRM_EFCT_SYNC_CTRL_s *EfctSyncCtrl = HL_CtxYuvStrmEfctSyncCtrlPtr;

    HL_GetResourcePtr(&Resource);
    if (SyncWithVin > 0U) {
        /* Sweep YuvStrm with same VoutIdx */
        for (i=0U; i<Resource->YuvStrmNum; i++) {
            HL_GetYuvStrmInfoPtr(i, &YuvStrmInfo);
            if (1U == DSP_GetU16Bit(YuvStrmInfo->Purpose, DSP_LV_STRM_PURPOSE_VOUT_IDX, 1U)) {
                HL_GetYuvStrmEfctSyncCtrlLock(i, &EfctSyncCtrl);
                if (EfctSyncCtrl->State != EFCT_SYNC_OFF) {
                    *pIsEfctSync = 1U;
                    *pYuvStrmIdx = i;
                    HL_GetYuvStrmEfctSyncCtrlUnLock(i);
                    break;
                } else {
                    HL_GetYuvStrmEfctSyncCtrlUnLock(i);
                }
            }
        }
    }
}

/**
* Vout OSD control function
* @param [in]  VoutIdx vout index
* @param [in]  Enable enable flag
* @param [in]  SyncWithVin control command sync with vin or not
* @param [out] pAttachedRawSeq attached raw sequence
* @return ErrorCode
*/
UINT32 dsp_vout_osd_ctrl(const UINT8 VoutIdx, UINT8 Enable, UINT8 SyncWithVin, UINT64 *pAttachedRawSeq)
{
    UINT32 Rval;
    UINT16 i, YuvStrmIdx = 0U;
    UINT8 IsEfctSync = 0U;//, ExitLoop;
    UINT8 WriteMode = AMBA_DSP_CMD_VDSP_NORMAL_WRITE;
    CTX_YUVSTRM_EFCT_SYNC_CTRL_s *EfctSyncCtrl = HL_CtxYuvStrmEfctSyncCtrlPtr;

    AmbaMisra_TouchUnused(pAttachedRawSeq);

    Rval = HL_VoutOsdCtrlChk(VoutIdx, Enable, SyncWithVin, pAttachedRawSeq);

    if (Rval == OK) {
        CTX_VOUT_INFO_s VoutInfo = {0};

        HL_GetVoutInfo(HL_MTX_OPT_ALL, VoutIdx, &VoutInfo);
        if ((DSP_PROF_STATUS_INVALID != DSP_GetProfState()) &&
            (DSP_VOUT_DISPLAY_NO_INIT != VoutInfo.DisplayStatus)) {

            HL_VoutSweepYuv(SyncWithVin, &IsEfctSync, &YuvStrmIdx);

            if (IsEfctSync > 0U) {
                UINT8 JobExisted = 0U;
                UINT16 JobType;
                CTX_EFCT_SYNC_JOB_SET_s *EfctSyncJobSet = HL_CtxYuvStrmEfctSyncJobSetPtr;
                DSP_EFCT_SYNC_JOB_s *pSyncJob = NULL;

                //AmbaLL_LogUInt5("OsdCtrl[%d] for Sync", VoutIdx, 0U, 0U, 0U, 0U);
                /* Assign Job and Copy necessary Data */
                HL_GetYuvStrmEfctSyncCtrlLock(YuvStrmIdx, &EfctSyncCtrl);
                HL_GetYuvStrmEfctSyncJobSetLock(YuvStrmIdx, (UINT16)EfctSyncCtrl->PoolDesc.Wp, &EfctSyncJobSet);

                /* Sweep Existed Job */
                for (i=0U; i<EfctSyncJobSet->JobNum; i++) {
                    JobType = (UINT16)EfctSyncJobSet->Job[i].JobId.JobTypeBit;

                    /* Vout has its own job */
                    if (1U == DSP_GetU16Bit(JobType, EFCT_SYNC_JOB_BIT_DISPLAY, 1U)) {
                        if (VoutIdx == EfctSyncJobSet->Job[i].VoutIdx) {
                            pSyncJob = &EfctSyncJobSet->Job[i];
                            pSyncJob->VoutIdx = VoutIdx;
                            JobExisted = 1U;
                        }
                    }
                }

                if (JobExisted == 0U) {
                    /* Create new Job with prepared YuvStrmIdx/SeqIdx */

                    pSyncJob = &EfctSyncJobSet->Job[EfctSyncJobSet->JobNum];
                    pSyncJob->VoutIdx = VoutIdx;
                    pSyncJob->JobId = EfctSyncJobSet->JobId;
                    JobType = 1U/* << EFCT_SYNC_JOB_BIT_DISPLAY*/;
                    Rval = HL_ComposeEfctSyncJobId(&pSyncJob->JobId,
                                                   EFCT_SYNC_JOB_ID_DONTCARE,
                                                   EFCT_SYNC_JOB_ID_DONTCARE,
                                                   JobType,
                                                   (UINT16)EfctSyncJobSet->JobNum+1U);
                    if (Rval == OK) {
                        /* Advanced JobNum */
                        (EfctSyncJobSet->JobNum)++;
#ifdef DEBUG_SYNC_CMD
                        AmbaLL_LogUInt5("@@[%d] New Display Type[0x%X] Sub[%d]", __LINE__, pSyncJob->JobId.JobTypeBit, pSyncJob->JobId.SubJobIdx, 0, 0);
#endif
                    }
                }

                if (Rval == OK) {
                    HL_GetVoutInfo(HL_MTX_OPT_ALL, VoutIdx, &VoutInfo);
//                    if (pSyncJob != NULL) {
//                        pSyncJob->VoutIdx = VoutIdx;
//                    }
                    EfctSyncJobSet->OsdEnable[VoutIdx] = Enable;
                    (void)dsp_osal_memcpy(&EfctSyncJobSet->OsdCfg[VoutIdx], &VoutInfo.OsdCfg, sizeof(AMBA_DSP_VOUT_OSD_BUF_CONFIG_s));
                }
                HL_GetYuvStrmEfctSyncCtrlUnLock(YuvStrmIdx);
                HL_GetYuvStrmEfctSyncJobSetUnLock(YuvStrmIdx, (UINT16)EfctSyncCtrl->PoolDesc.Wp);
            } else {
                UINT8 CmdBufferId = 0U;
                void *CmdBufferAddr = NULL;
                cmd_vout_osd_setup_t *OsdSetup = HL_DefCtxCmdBufPtrVoutOsd;

                if (VoutInfo.OsdCfg.DataFormat == OSD_8BIT_CLUT_MODE) {
                    cmd_vout_osd_clut_setup_t *OsdClutCfg = HL_DefCtxCmdBufPtrVoutOsdClut;
                    UINT16 VoutPhysId = 0U, VoutVirtId = 0U;

                    HL_AcqCmdBuffer(&CmdBufferId, &CmdBufferAddr);
                    dsp_osal_typecast(&OsdClutCfg, &CmdBufferAddr);
                    (void)HL_GetVoutPhysId(VoutIdx, &VoutPhysId);
                    (void)HL_GetVoutVirtId(VoutIdx, &VoutVirtId);
                    OsdClutCfg->vout_id = (UINT8)VoutPhysId;
                    OsdClutCfg->virtual_chan_id = (UINT8)VoutVirtId;
                    (void)dsp_osal_virt2cli(VoutInfo.OsdCfg.CLUTAddr, &OsdClutCfg->clut_dram_addr);
                    Rval = AmbaHL_CmdVoutOsdClutSetup(WriteMode, OsdClutCfg);
                    HL_RelCmdBuffer(CmdBufferId);
                }

                HL_AcqCmdBuffer(&CmdBufferId, &CmdBufferAddr);
                dsp_osal_typecast(&OsdSetup, &CmdBufferAddr);
                HL_GetVoutInfo(HL_MTX_OPT_GET, VoutIdx, &VoutInfo);
                (void)HL_FillOsdSetup(VoutIdx, &VoutInfo.OsdCfg, OsdSetup);
                OsdSetup->en = Enable;
                VoutInfo.OsdEnable = Enable;
                HL_SetVoutInfo(HL_MTX_OPT_SET, VoutIdx, &VoutInfo);
                if (Rval == OK) {
                    Rval = AmbaHL_CmdVoutOsdSetup(WriteMode, OsdSetup);
                }
                HL_RelCmdBuffer(CmdBufferId);
            }
        } else {
            HL_GetVoutInfo(HL_MTX_OPT_GET, VoutIdx, &VoutInfo);
            VoutInfo.OsdEnable = Enable;
            HL_SetVoutInfo(HL_MTX_OPT_SET, VoutIdx, &VoutInfo);
        }
    }

    if (Rval != OK) {
        AmbaLL_LogUInt5("AmbaDSP_VoutOsdCtrl[%d] Error: %d", VoutIdx, Rval, 0U, 0U, 0U);
    }

    return Rval;
}

static inline UINT32 HL_VoutDisplayConfigChk(const UINT8 VoutIdx, const AMBA_DSP_DISPLAY_CONFIG_s* pConfig)
{
    UINT32 Rval;
    const Dsp_TimingApi_t* TimingApi = AmbaDSP_GetTimingApiFunc();
    const Dsp_DumpApi_t* DumpApi = AmbaDSP_GetDumpApiFunc();
    const Dsp_CheckApi_t* CheckApi = AmbaDSP_GetCheckApiFunc();

    /* DSP CtxInit */
    Rval = HL_CtxInit();

    /* Timing sanity check */
    if ((Rval == OK) &&
        (TimingApi != NULL)) {
        Rval = TimingApi->pVoutDisplayConfig(VoutIdx, pConfig);
    }
    /* Input sanity check */
    if ((Rval == OK) &&
        (DumpApi != NULL)) {
        DumpApi->pVoutDisplayConfig(VoutIdx, pConfig);
    }
    if ((Rval == OK) &&
        (CheckApi != NULL)) {
        Rval = CheckApi->pVoutDisplayConfig(VoutIdx, pConfig);
    }

    /* Logic sanity check */
    return Rval;
}

/**
* Vout Display config function
* @param [in]  VoutIdx vout index
* @param [in]  pConfig configuration information
* @return ErrorCode
*/
UINT32 dsp_vout_display_cfg(const UINT8 VoutIdx, const AMBA_DSP_DISPLAY_CONFIG_s* pConfig)
{
    UINT32 Rval;

    Rval = HL_VoutDisplayConfigChk(VoutIdx, pConfig);

    if (Rval == OK) {
        CTX_VOUT_INFO_s VoutInfo = {0};

        HL_GetVoutInfo(HL_MTX_OPT_GET, VoutIdx, &VoutInfo);
        (void)dsp_osal_memcpy(&VoutInfo.DispCfg, pConfig, sizeof(AMBA_DSP_DISPLAY_CONFIG_s));
        HL_SetVoutInfo(HL_MTX_OPT_SET, VoutIdx, &VoutInfo);
    }

    if (Rval != OK) {
        AmbaLL_LogUInt5("AmbaDSP_VoutDisplayConfig[%d] Error: %d", VoutIdx, Rval, 0U, 0U, 0U);
    }

    return Rval;
}

static inline UINT32 HL_VoutDisplayCtrlChk(const UINT8 VoutIdx)
{
    UINT32 Rval;
    const Dsp_TimingApi_t* TimingApi = AmbaDSP_GetTimingApiFunc();
    const Dsp_DumpApi_t* DumpApi = AmbaDSP_GetDumpApiFunc();
    const Dsp_CheckApi_t* CheckApi = AmbaDSP_GetCheckApiFunc();
    const Dsp_ResourceApi_t* ResourceApi = AmbaDSP_GetResourceApiFunc();

    /* DSP CtxInit */
    Rval = HL_CtxInit();

    /* Timing sanity check */
    if ((Rval == OK) &&
        (TimingApi != NULL)) {
        Rval = TimingApi->pVoutDisplayCtrl(VoutIdx);
    }
    /* Input sanity check */
    if ((Rval == OK) &&
        (DumpApi != NULL)) {
        DumpApi->pVoutDisplayCtrl(VoutIdx);
    }
    if ((Rval == OK) &&
        (CheckApi != NULL)) {
        Rval = CheckApi->pVoutDisplayCtrl(VoutIdx);
    }
    /* HW/SW resources check */
    if ((Rval == OK) &&
        (ResourceApi != NULL)) {
        Rval = ResourceApi->pVoutDisplayCtrl(VoutIdx);
    }

    /* Logic sanity check */

    return Rval;
}

/**
* Vout Display control function
* @param [in]  VoutIdx vout index
* @return ErrorCode
*/
UINT32 dsp_vout_display_ctrl(const UINT8 VoutIdx)
{
    UINT32 Rval;
#if 0
    void *pVoid = NULL;
#endif
    ULONG ULAddr = 0U;
    AMBA_DSP_VOUT_DISP_CFG_DATA_s *pVoutDispCfg;

    Rval = HL_VoutDisplayCtrlChk(VoutIdx);

    if (Rval == OK) {
        UINT32 Enable;
        CTX_VOUT_INFO_s VoutInfo = {0};

        HL_GetVoutInfo(HL_MTX_OPT_GET, VoutIdx, &VoutInfo);
        if ((DSP_PROF_STATUS_INVALID != DSP_GetProfState()) &&
            (DSP_VOUT_DISPLAY_NO_INIT != VoutInfo.DisplayStatus)) {

            UINT8 CmdBufferId = 0U;
            void *CmdBufferAddr = NULL;
            cmd_vout_display_setup_t *DisplaySetup = HL_DefCtxCmdBufPtrVoutDisp;
            UINT16 VoutPhysId = 0U, VoutVirtId = 0U;
            CTX_VIN_INFO_s VinInfo = {0};
            UINT32 VinInterval = 0U;

            HL_AcqCmdBuffer(&CmdBufferId, &CmdBufferAddr);
            dsp_osal_typecast(&DisplaySetup, &CmdBufferAddr);
            (void)HL_GetVoutPhysId(VoutIdx, &VoutPhysId);
            (void)HL_GetVoutVirtId(VoutIdx, &VoutVirtId);
            DisplaySetup->vout_id = (UINT8)VoutPhysId;
            DisplaySetup->virtual_chan_id = (UINT8)VoutVirtId;
            DisplaySetup->vin_vout_vsync_delay_ms_x10 = VoutInfo.DispCfg.VinVoutSyncDelay;
            DisplaySetup->dual_vout_vysnc_delay_ms_x10 = VoutInfo.DispCfg.VoutSyncDelay;

//            if (VoutIdx == VOUT_IDX_A) {
                /* Inside uCode, vout introduces single thread for multi vout, so using HL to control sync timing */
                Enable = (VoutInfo.DispCfg.VinVoutSyncDelay > 0U)? (UINT32)1U: (UINT32)0U;
                DisplaySetup->vout_sync_with_vin = (UINT16)Enable;
                if (Enable > 0U) {
                    /*
                     * 20220506, ucoder suggest to have 2~3 vin interval
                     */
                    HL_GetVinInfo(HL_MTX_OPT_ALL, (UINT16)VoutInfo.DispCfg.VinTargetIndex, &VinInfo);
                    if ((VinInfo.FrameRate.NumUnitsInTick > 0U) &&
                        (VinInfo.FrameRate.TimeScale > 0U)) {
                        VinInterval = (VinInfo.FrameRate.NumUnitsInTick*1000U)/VinInfo.FrameRate.TimeScale;
                        VinInterval *= 3U;
                    }
                    DisplaySetup->vin_vout_vsync_timeout_ms = (VinInterval > 0U)? (UINT16)VinInterval: 20U;
                }
    //            (void)AmbaRTSL_Vout0SetVinVoutSync(Enable);

                DSP_GetDspVoutDispCfg(VoutIdx, &pVoutDispCfg);
                dsp_osal_typecast(&ULAddr, &pVoutDispCfg);
#if 0
                dsp_osal_typecast(&pVoid, &ULAddr);
                (void)dsp_osal_cache_clean(pVoid, 508U/*sizeof(AMBA_VOUT_DISPLAY0_CONFIG_REG_s)*/);
#endif
                (void)dsp_osal_virt2cli(ULAddr, &DisplaySetup->disp_config_dram_addr);
//            } else {
//                Enable = (VoutInfo.DispCfg.VinVoutSyncDelay > 0U)? (UINT32)1U: (UINT32)0U;
//                DisplaySetup->vout_sync_with_vin = (UINT16)Enable;
//                if (Enable > 0U) {
//                    DisplaySetup->vin_vout_vsync_timeout_ms = 20U;
//                }
//
//    //            (void)AmbaRTSL_Vout1SetVinVoutSync(Enable);
//
//                DSP_GetDspVoutDispCfg(VoutIdx, &pVoutDispCfg);
//                dsp_osal_typecast(&ULAddr, &pVoutDispCfg);
//#if 0
//                dsp_osal_typecast(&pVoid, &ULAddr);
//                (void)dsp_osal_cache_clean(pVoid, 508U);  //sizeof(AMBA_VOUT_DISPLAY1_CONFIG_REG_s));
//#endif
//                (void)dsp_osal_virt2cli(ULAddr, &DisplaySetup->disp_config_dram_addr);
//            }

            if (VoutIdx == VOUT_IDX_B) {
                if (VoutInfo.DataPath.DisplayOption == 0U/*HDMI or DVE*/) {
                    DisplaySetup->is_use_disp_c = (UINT8)1U;

                    /*
                     * In DVcam
                     * Normally vout polarity need to be REVERSED when interlace mode
                     * But under DVCam CVBS displayconfig setting,
                     * polarity shall not be REVERSED when CVBS+Interlace.
                     *
                     * Root cause : vout polarity reported timing is different
                     *              so just keep the polarity from vout status
                     * Compare to IpCam/DvCam's display crop area setting
                     * [DVCam]
                     *   VOUTD_frame_size:            0x06b30106
                     *   VOUTD_frame_height_field_1:  0x00000105
                     *   VOUTD_active_region_start_0: 0x01110016
                     *   VOUTD_active_region_end_0:   0x06b00105
                     *   VOUTD_active_region_start_1: 0x01110015
                     *   VOUTD_active_region_end_1:   0x06b00104
                     * [IpCam]
                     *   VOUTD_frame_size:            0x06b30105
                     *   VOUTD_frame_height_field_1:  0x00000106
                     *   VOUTD_active_region_start_0: 0x01120016
                     *   VOUTD_active_region_end_0:   0x06b10105
                     *   VOUTD_active_region_start_1: 0x01120017
                     *   VOUTD_active_region_end_1:   0x06b10106
                     *  we can observe that DvCam's BottomField end line is one line BEFORE TopField end line
                     *                      IpCam's BottomField end line is one line AFTER TopField end line
                     *  the setting seems lead to vout frame done timing differnce then lead to different polarity setting.
                     *  TBD,
                     *  HDMI interlace may share same issue but not be verified, sice HDMI interlace mode display timing is simliar to IpCam style
                     */
                    if (VoutInfo.MixerCfg.FrameRate.Interlace == (UINT8)1U) {
                        if (VoutInfo.DveStatus >= DSP_VOUT_DVE_CFGED) {
                            DisplaySetup->reverse_polarity = 0U;
                        } else {
                            DisplaySetup->reverse_polarity = 1U;
                        }
                    }
                } else {
                    DisplaySetup->is_use_disp_c = (UINT8)0U;
                }
            } else {
                DisplaySetup->is_use_disp_c = (UINT8)0U;
            }

            Rval = AmbaHL_CmdVoutDisplaySetup(AMBA_DSP_CMD_VDSP_NORMAL_WRITE, DisplaySetup);
            HL_RelCmdBuffer(CmdBufferId);
            VoutInfo.DisplayStatus = DSP_VOUT_DISPLAY_START;
        } else {
            VoutInfo.DisplayStatus = DSP_VOUT_DISPLAY_NO_INIT;
        }
        HL_SetVoutInfo(HL_MTX_OPT_SET, VoutIdx, &VoutInfo);
    }

    if (Rval != OK) {
        AmbaLL_LogUInt5("AmbaDSP_VoutDisplayCtrl[%d] Error: %d", VoutIdx, Rval, 0U, 0U, 0U);
    }

    return Rval;
}

static inline UINT32 HL_VoutDisplayConfigGammaChk(const UINT8 VoutIdx, ULONG TableAddr)
{
    UINT32 Rval;
    const Dsp_TimingApi_t* TimingApi = AmbaDSP_GetTimingApiFunc();
    const Dsp_DumpApi_t* DumpApi = AmbaDSP_GetDumpApiFunc();
    const Dsp_CheckApi_t* CheckApi = AmbaDSP_GetCheckApiFunc();

    /* DSP CtxInit */
    Rval = HL_CtxInit();

    /* Timing sanity check */
    if ((Rval == OK) &&
        (TimingApi != NULL)) {
        Rval = TimingApi->pVoutDisplayConfigGamma(VoutIdx, TableAddr);
    }
    /* Input sanity check */
    if ((Rval == OK) &&
        (DumpApi != NULL)) {
        DumpApi->pVoutDisplayConfigGamma(VoutIdx, TableAddr);
    }
    if ((Rval == OK) &&
        (CheckApi != NULL)) {
        Rval = CheckApi->pVoutDisplayConfigGamma(VoutIdx, TableAddr);
    }

    return Rval;
}

/**
* Vout Display Gamma config function
* @param [in]  VoutIdx vout index
* @param [in]  TableAddr gamma table
* @return ErrorCode
*/
UINT32 dsp_vout_display_cfg_gamma(const UINT8 VoutIdx, ULONG TableAddr)
{
    UINT32 Rval;

    Rval = HL_VoutDisplayConfigGammaChk(VoutIdx, TableAddr);

    if (Rval == OK) {
        CTX_VOUT_INFO_s VoutInfo = {0};

        HL_GetVoutInfo(HL_MTX_OPT_GET, VoutIdx, &VoutInfo);
        VoutInfo.GammaTableAddr = TableAddr;
        HL_SetVoutInfo(HL_MTX_OPT_SET, VoutIdx, &VoutInfo);
    }

    if (Rval != OK) {
        AmbaLL_LogUInt5("AmbaDSP_VoutDisplayConfigGamma[%d] Error: %d", VoutIdx, Rval, 0U, 0U, 0U);
    }

    return Rval;
}

static inline UINT32 HL_VoutDisplayControlGammaChk(const UINT8 VoutIdx, UINT8 Enable)
{
    UINT32 Rval;
    const Dsp_TimingApi_t* TimingApi = AmbaDSP_GetTimingApiFunc();
    const Dsp_DumpApi_t* DumpApi = AmbaDSP_GetDumpApiFunc();
    const Dsp_CheckApi_t* CheckApi = AmbaDSP_GetCheckApiFunc();
    const Dsp_ResourceApi_t* ResourceApi = AmbaDSP_GetResourceApiFunc();

    /* DSP CtxInit */
    Rval = HL_CtxInit();

    /* Timing sanity check */
    if ((Rval == OK) &&
        (TimingApi != NULL)) {
        Rval = TimingApi->pVoutDisplayControlGamma(VoutIdx, Enable);
    }
    /* Input sanity check */
    if ((Rval == OK) &&
        (DumpApi != NULL)) {
        DumpApi->pVoutDisplayControlGamma(VoutIdx, Enable);
    }
    if ((Rval == OK) &&
        (CheckApi != NULL)) {
        Rval = CheckApi->pVoutDisplayControlGamma(VoutIdx, Enable);
    }
    /* HW/SW resources check */
    if ((Rval == OK) &&
        (ResourceApi != NULL)) {
        Rval = ResourceApi->pVoutDisplayControlGamma(VoutIdx, Enable);
    }

    return Rval;
}

/**
* Vout Display Gamma control function
* @param [in]  VoutIdx vout index
* @param [in]  Enable enable gamma setting
* @return ErrorCode
*/
UINT32 dsp_vout_display_ctrl_gamma(const UINT8 VoutIdx, UINT8 Enable)
{
    UINT32 Rval;

    Rval = HL_VoutDisplayControlGammaChk(VoutIdx, Enable);

    if (Rval == OK) {
        UINT8 CmdBufferId = 0U;
        void *CmdBufferAddr = NULL;
        CTX_VOUT_INFO_s VoutInfo = {0};
        cmd_vout_gamma_setup_t *GammaSetup = HL_DefCtxCmdBufPtrVoutGamma;
        UINT16 VoutPhysId = 0U, VoutVirtId = 0U;

        HL_GetVoutInfo(HL_MTX_OPT_ALL, VoutIdx, &VoutInfo);
        if ((DSP_PROF_STATUS_INVALID != DSP_GetProfState()) &&
            (DSP_VOUT_DISPLAY_NO_INIT != VoutInfo.DisplayStatus)) {
//FIXME, not support runtime change
            HL_AcqCmdBuffer(&CmdBufferId, &CmdBufferAddr);
            dsp_osal_typecast(&GammaSetup, &CmdBufferAddr);
            (void)HL_GetVoutPhysId(VoutIdx, &VoutPhysId);
            (void)HL_GetVoutVirtId(VoutIdx, &VoutVirtId);
            GammaSetup->vout_id = (UINT8)VoutPhysId;
            GammaSetup->virtual_chan_id = (UINT8)VoutVirtId;
            if (Enable == 1U) {
                GammaSetup->enable = 1;
                GammaSetup->setup_gamma_table = 1;
                HL_GetVoutInfo(HL_MTX_OPT_GET, VoutIdx, &VoutInfo);
                (void)dsp_osal_virt2cli(VoutInfo.GammaTableAddr, &GammaSetup->gamma_dram_addr);
                HL_SetVoutInfo(HL_MTX_OPT_SET, VoutIdx, &VoutInfo);
            } else {
                GammaSetup->enable = 0;
            }
            Rval = AmbaHL_CmdVoutGammaSetup(AMBA_DSP_CMD_VDSP_NORMAL_WRITE, GammaSetup);
            HL_RelCmdBuffer(CmdBufferId);
        } else {
            HL_GetVoutInfo(HL_MTX_OPT_GET, VoutIdx, &VoutInfo);
            VoutInfo.GammaEnable = Enable;
            HL_SetVoutInfo(HL_MTX_OPT_SET, VoutIdx, &VoutInfo);
        }
    }

    if (Rval != OK) {
        AmbaLL_LogUInt5("AmbaDSP_VoutDisplayControlGamma[%d] Error: %d", VoutIdx, Rval, 0U, 0U, 0U);
    }

    return Rval;
}

static inline UINT32 HL_VoutVideoConfigChk(const UINT8 VoutIdx, const AMBA_DSP_VOUT_VIDEO_CFG_s *pConfig)
{
    UINT32 Rval;
    const Dsp_TimingApi_t* TimingApi = AmbaDSP_GetTimingApiFunc();
    const Dsp_DumpApi_t* DumpApi = AmbaDSP_GetDumpApiFunc();
    const Dsp_CheckApi_t* CheckApi = AmbaDSP_GetCheckApiFunc();

    /* DSP CtxInit */
    Rval = HL_CtxInit();

    /* Timing sanity check */
    if ((Rval == OK) &&
        (TimingApi != NULL)) {
        Rval = TimingApi->pVoutVideoConfig(VoutIdx, pConfig);
    }
    /* Input sanity check */
    if ((Rval == OK) &&
        (DumpApi != NULL)) {
        DumpApi->pVoutVideoConfig(VoutIdx, pConfig);
    }
    if ((Rval == OK) &&
        (CheckApi != NULL)) {
        Rval = CheckApi->pVoutVideoConfig(VoutIdx, pConfig);
    }

    /* Logic sanity check */
    return Rval;
}

/**
* Vout video plance config function
* @param [in]  VoutIdx vout index
* @param [in]  pConfig video plane configuration
* @return ErrorCode
*/
UINT32 dsp_vout_video_cfg(const UINT8 VoutIdx, const AMBA_DSP_VOUT_VIDEO_CFG_s *pConfig)
{
    UINT32 Rval;

    Rval = HL_VoutVideoConfigChk(VoutIdx, pConfig);

    if (Rval == OK) {
        CTX_VOUT_INFO_s VoutInfo = {0};
        CTX_RESOURCE_INFO_s *Resource = HL_CtxResInfoPtr;

        HL_GetVoutInfo(HL_MTX_OPT_GET, VoutIdx, &VoutInfo);
        HL_GetResourcePtr(&Resource);

        if (HL_IS_ROTATE(VoutInfo.VideoCfg.RotateFlip) != HL_IS_ROTATE(pConfig->RotateFlip)) {
            Resource->VoutRotate[VoutIdx] = HL_IS_ROTATE(pConfig->RotateFlip);
            HL_SetRescState(HL_RESC_CONFIGED);
        }

        if ((VoutInfo.VideoCfg.VideoSource != pConfig->VideoSource) &&
            (VoutInfo.VideoCfg.VideoSource == VOUT_SOURCE_DEFAULT_IMAGE)){
            (void)dsp_osal_memcpy(&VoutInfo.VideoCfg.Window, &pConfig->Window, sizeof(AMBA_DSP_WINDOW_s));
            VoutInfo.VideoCfg.RotateFlip = pConfig->RotateFlip;
            VoutInfo.VideoCfg.VideoSource = pConfig->VideoSource;
        } else {
            (void)dsp_osal_memcpy(&VoutInfo.VideoCfg, pConfig, sizeof(AMBA_DSP_VOUT_VIDEO_CFG_s));
        }
        VoutInfo.VideoEnable = 1U;
        HL_SetVoutInfo(HL_MTX_OPT_SET, VoutIdx, &VoutInfo);
    }

    if (Rval != OK) {
        AmbaLL_LogUInt5("AmbaDSP_VoutVideoConfig[%d] Error: %d", VoutIdx, Rval, 0U, 0U, 0U);
    }

    return Rval;
}

static inline UINT32 HL_VoutVideoCtrlChk(const UINT8 VoutIdx,
                                         const UINT8 Enable,
                                         const UINT8 SyncWithVin,
                                         const UINT64 *pAttachedRawSeq)
{
    UINT32 Rval;
    const Dsp_TimingApi_t* TimingApi = AmbaDSP_GetTimingApiFunc();
    const Dsp_DumpApi_t* DumpApi = AmbaDSP_GetDumpApiFunc();
    const Dsp_CheckApi_t* CheckApi = AmbaDSP_GetCheckApiFunc();
    const Dsp_ResourceApi_t* ResourceApi = AmbaDSP_GetResourceApiFunc();

    /* DSP CtxInit */
    Rval = HL_CtxInit();

    /* Timing sanity check */
    if ((Rval == OK) &&
        (TimingApi != NULL)) {
        Rval = TimingApi->pVoutVideoCtrl(VoutIdx, Enable, SyncWithVin, pAttachedRawSeq);
    }
    /* Input sanity check */
    if ((Rval == OK) &&
        (DumpApi != NULL)) {
        DumpApi->pVoutVideoCtrl(VoutIdx, Enable, SyncWithVin, pAttachedRawSeq);
    }
    if ((Rval == OK) &&
        (CheckApi != NULL)) {
        Rval = CheckApi->pVoutVideoCtrl(VoutIdx, Enable, SyncWithVin, pAttachedRawSeq);
    }
    /* HW/SW resources check */
    if ((Rval == OK) &&
        (ResourceApi != NULL)) {
        Rval = ResourceApi->pVoutVideoCtrl(VoutIdx, Enable, SyncWithVin, pAttachedRawSeq);
    }

    /* Logic sanity check */
    return Rval;
}

static inline UINT8 HL_VoutVdoCtrlIsNeedCmd(const UINT8 IsDspModeIdle,
                                            const UINT8 IsDspModeCamera,
                                            const UINT8 ProfState,
                                            const UINT8 DisplayStatus)
{
    UINT8 Rval = 0U;

    if (((IsDspModeIdle == 1U) ||
         (IsDspModeCamera == 1U)) &&
        (DSP_PROF_STATUS_INVALID != ProfState) &&
        (DSP_VOUT_DISPLAY_NO_INIT != DisplayStatus)) {
        Rval = 1U;
    }

    return Rval;
}

static inline void HL_VoutVdoCtrlSetSrc(const CTX_VOUT_INFO_s *pVoutInfo,
                                        const UINT8 IsDspModeCamera,
                                        cmd_vout_video_setup_t *pVideoSetup)
{
    if (pVoutInfo->VideoCfg.VideoSource == VOUT_SOURCE_INTERNAL) {
        if (IsDspModeCamera == 1U) {
            pVideoSetup->src = DSP_VOUT_SRC_VCAP;
        } else {
            pVideoSetup->src = DSP_VOUT_SRC_BG;
        }
    } else {
        pVideoSetup->src = pVoutInfo->VideoCfg.VideoSource;
    }
}

static inline void HL_VoutVdoCtrlSetLowDly(const UINT8 VoutIdx, cmd_vout_video_setup_t *pVideoSetup)
{
    UINT8 IsVoutPurpose = 0U, IsLdPurpose = 0U;
    UINT16 i, YuvStrmVoutId = 0U;
    CTX_RESOURCE_INFO_s *pResource = HL_CtxResInfoPtr;
    CTX_YUV_STRM_INFO_s *pYuvStrmInfo = HL_CtxYuvStrmInfoPtr;

    HL_GetResourcePtr(&pResource);
    pVideoSetup->is_video_lowdelay = 0U;
    for (i = 0U; i < pResource->YuvStrmNum; i++) {
        HL_GetYuvStrmInfoPtr(i, &pYuvStrmInfo);
        IsVoutPurpose = (UINT8)DSP_GetU16Bit(pYuvStrmInfo->Purpose, DSP_LV_STRM_PURPOSE_VOUT_IDX, 1U);
        IsLdPurpose = (UINT8)DSP_GetU16Bit(pYuvStrmInfo->Purpose, DSP_LV_STRM_PURPOSE_LD_IDX, 1U);
        DSP_Bit2U16Idx(pYuvStrmInfo->DestVout, &YuvStrmVoutId);
        if ((IsVoutPurpose == 1U) &&
            (IsLdPurpose == 1U) &&
            ((UINT8)YuvStrmVoutId == VoutIdx)) {
            pVideoSetup->is_video_lowdelay = 1U;
        }
    }
}

/**
* Vout video plance control function
* @param [in]  VoutIdx vout index
* @param [in]  Enable enable flag
* @param [in]  SyncWithVin control command sync with vin or not
* @param [out] pAttachedRawSeq attached raw sequence
* @return ErrorCode
*/
UINT32 dsp_vout_video_ctrl(const UINT8 VoutIdx, UINT8 Enable, UINT8 SyncWithVin, UINT64 *pAttachedRawSeq)
{
    UINT32 Rval;
    UINT8 IsDspModeIdle, IsDspModeCamera;
    UINT8 ProfState = DSP_GetProfState();

    Rval = HL_VoutVideoCtrlChk(VoutIdx, Enable, SyncWithVin, pAttachedRawSeq);
    AmbaMisra_TouchUnused(pAttachedRawSeq);

    if (Rval == OK) {
        CTX_VOUT_INFO_s VoutInfo = {0};

        IsDspModeIdle = (DSP_PROF_STATUS_IDLE == DSP_GetProfState())? 1U: 0U;
        IsDspModeCamera = (DSP_PROF_STATUS_CAMERA == DSP_GetProfState())? 1U: 0U;

        HL_GetVoutInfo(HL_MTX_OPT_GET, VoutIdx, &VoutInfo);
        //Anyone who ever controlled VideoSetup will need VideoEnableCmd
        VoutInfo.VideoEnable = Enable;
        if (HL_VoutVdoCtrlIsNeedCmd(IsDspModeIdle,
                                    IsDspModeCamera,
                                    ProfState,
                                    VoutInfo.DisplayStatus) == 1U) {

            UINT8 CmdBufferId = 0U;
            void *CmdBufferAddr = NULL;
            cmd_vout_video_setup_t *pVideoSetup = HL_DefCtxCmdBufPtrVoutVdo;
            UINT16 VoutPhysId = 0U, VoutVirtId = 0U;

            HL_AcqCmdBuffer(&CmdBufferId, &CmdBufferAddr);
            dsp_osal_typecast(&pVideoSetup, &CmdBufferAddr);
            (void)HL_GetVoutPhysId(VoutIdx, &VoutPhysId);
            (void)HL_GetVoutVirtId(VoutIdx, &VoutVirtId);
            pVideoSetup->vout_id = (UINT8)VoutPhysId;
            pVideoSetup->virtual_chan_id = (UINT8)VoutVirtId;
            pVideoSetup->en = Enable;
            HL_VoutVdoCtrlSetSrc(&VoutInfo, IsDspModeCamera, pVideoSetup);
            VoutInfo.Source = pVideoSetup->src;

            pVideoSetup->flip = HL_GET_FLIP(VoutInfo.VideoCfg.RotateFlip);
            pVideoSetup->rotate = HL_GET_ROTATE(VoutInfo.VideoCfg.RotateFlip);
            pVideoSetup->rotate_flip_mode = 0U;
            pVideoSetup->win_width = VoutInfo.VideoCfg.Window.Width;
            pVideoSetup->win_height = VoutInfo.VideoCfg.Window.Height;
            pVideoSetup->win_offset_x = VoutInfo.VideoCfg.Window.OffsetX;
            pVideoSetup->win_offset_y = VoutInfo.VideoCfg.Window.OffsetY;
            (void)dsp_osal_virt2cli(VoutInfo.VideoCfg.DefaultImgConfig.BaseAddrY, &pVideoSetup->default_img_y_addr);
            (void)dsp_osal_virt2cli(VoutInfo.VideoCfg.DefaultImgConfig.BaseAddrUV, &pVideoSetup->default_img_uv_addr);
            pVideoSetup->default_img_pitch = VoutInfo.VideoCfg.DefaultImgConfig.Pitch;
            pVideoSetup->default_img_repeat_field = VoutInfo.VideoCfg.DefaultImgConfig.FieldRepeat;
            pVideoSetup->default_img_ignore_rotate_flip = 0U;
            pVideoSetup->default_img_ch_fmt = VoutInfo.VideoCfg.DefaultImgConfig.DataFormat;
//FIXME, check user config Mixer first to get interlace information
            pVideoSetup->interlaced = VoutInfo.MixerCfg.FrameRate.Interlace;
            HL_VoutVdoCtrlSetLowDly(VoutIdx, pVideoSetup);
            Rval = AmbaHL_CmdVoutVideoSetup(AMBA_DSP_CMD_VDSP_NORMAL_WRITE, pVideoSetup);
            HL_RelCmdBuffer(CmdBufferId);
//FIXME, Update AttachedRawSeq
            if (SyncWithVin >= 1U) {
                (void)pAttachedRawSeq;
            }
        }

        HL_SetVoutInfo(HL_MTX_OPT_SET, VoutIdx, &VoutInfo);
    }

    if (Rval != OK) {
        AmbaLL_LogUInt5("AmbaDSP_VoutVideoCtrl[%d] Error: %d", VoutIdx, Rval, 0U, 0U, 0U);
    }

    return Rval;
}

static inline UINT32 HL_VoutConfigMixerBindingChk(UINT8 NumVout, const UINT8 *pVoutIdx, const AMBA_DSP_VOUT_DATA_PATH_CFG_s *pConfig)
{
    UINT32 Rval;
    const Dsp_TimingApi_t* TimingApi = AmbaDSP_GetTimingApiFunc();
    const Dsp_DumpApi_t* DumpApi = AmbaDSP_GetDumpApiFunc();
    const Dsp_CheckApi_t* CheckApi = AmbaDSP_GetCheckApiFunc();

    /* DSP CtxInit */
    Rval = HL_CtxInit();

    /* Timing sanity check */
    if ((Rval == OK) &&
        (TimingApi != NULL)) {
        Rval = TimingApi->pVoutConfigMixerBinding(NumVout, pVoutIdx, pConfig);
    }
    /* Input sanity check */
    if ((Rval == OK) &&
        (DumpApi != NULL)) {
        DumpApi->pVoutConfigMixerBinding(NumVout, pVoutIdx, pConfig);
    }
    if ((Rval == OK) &&
        (CheckApi != NULL)) {
        Rval = CheckApi->pVoutConfigMixerBinding(NumVout, pVoutIdx, pConfig);
    }
    return Rval;
}

/**
* Vout mixer binding config function
* @param [in]  NumVout number of vout
* @param [in]  pVoutIdx vout index
* @param [in]  pConfig binding configuration
* @return ErrorCode
*/
UINT32 dsp_vout_cfg_mixer_binding(UINT8 NumVout, const UINT8 *pVoutIdx, const AMBA_DSP_VOUT_DATA_PATH_CFG_s *pConfig)
{
    UINT32 Rval;
    UINT8 i = 0;

    Rval = HL_VoutConfigMixerBindingChk(NumVout, pVoutIdx, pConfig);

    if (Rval == OK) {
        CTX_VOUT_INFO_s VoutInfo = {0};

        for (i = 0; i < NumVout; i++) {
            if ((pVoutIdx != NULL) && (pConfig != NULL)) {
                HL_GetVoutInfo(HL_MTX_OPT_GET, pVoutIdx[i], &VoutInfo);
                (void)dsp_osal_memcpy(&VoutInfo.DataPath, pConfig, sizeof(AMBA_DSP_VOUT_DATA_PATH_CFG_s));
                HL_SetVoutInfo(HL_MTX_OPT_SET, pVoutIdx[i], &VoutInfo);
            }
        }
    }

    if (Rval != OK) {
        AmbaLL_LogUInt5("AmbaDSP_VoutConfigMixerBinding Error: %d", Rval, 0U, 0U, 0U, 0U);
    }

    return Rval;
}

/**
* Vout DVE config function
* @param [in]  VoutIdx vout index, only vout1
* @param [in]  DveMode [0]NTSC [1]PAL
* @return ErrorCode
*/
UINT32 dsp_vout_dve_cfg(const UINT8 VoutIdx, UINT8 DveMode)
{
    UINT32 Rval;
    const Dsp_TimingApi_t* TimingApi = AmbaDSP_GetTimingApiFunc();
    const Dsp_DumpApi_t* DumpApi = AmbaDSP_GetDumpApiFunc();
    const Dsp_CheckApi_t* CheckApi = AmbaDSP_GetCheckApiFunc();

    /* DSP CtxInit */
    Rval = HL_CtxInit();

    /* Timing sanity check */
    if ((Rval == OK) &&
        (TimingApi != NULL)) {
        Rval = TimingApi->pVoutDveConfig(VoutIdx, DveMode);
    }

    /* Input sanity check */
    if ((Rval == OK) &&
        (DumpApi != NULL)) {
        DumpApi->pVoutDveConfig(VoutIdx, DveMode);
    }
    if ((Rval == OK) &&
        (CheckApi != NULL)) {
        Rval = CheckApi->pVoutDveConfig(VoutIdx, DveMode);
    }

    /* Logic sanity check */

    if (Rval == OK) {
        CTX_VOUT_INFO_s VoutInfo = {0};

        HL_GetVoutInfo(HL_MTX_OPT_GET, VoutIdx, &VoutInfo);
        VoutInfo.DveMode = DveMode;
        VoutInfo.DveStatus = DSP_VOUT_DVE_CFGED;
        HL_SetVoutInfo(HL_MTX_OPT_SET, VoutIdx, &VoutInfo);
    }

    if (Rval != OK) {
        AmbaLL_LogUInt5("AmbaDSP_VoutDveConfig[%d] Error: %d", VoutIdx, Rval, 0U, 0U, 0U);
    }

    return Rval;
}

static inline UINT32 HL_VoutDveCtrlChk(const UINT8 VoutIdx)
{
    UINT32 Rval;
    const Dsp_TimingApi_t* TimingApi = AmbaDSP_GetTimingApiFunc();
    const Dsp_DumpApi_t* DumpApi = AmbaDSP_GetDumpApiFunc();
    const Dsp_CheckApi_t* CheckApi = AmbaDSP_GetCheckApiFunc();
    const Dsp_ResourceApi_t* ResourceApi = AmbaDSP_GetResourceApiFunc();

    /* DSP CtxInit */
    Rval = HL_CtxInit();

    /* Timing sanity check */
    if ((Rval == OK) &&
        (TimingApi != NULL)) {
        Rval = TimingApi->pVoutDveCtrl(VoutIdx);
    }

    /* Input sanity check */
    if (DumpApi != NULL) {
        DumpApi->pVoutDveCtrl(VoutIdx);
    }
    if (CheckApi != NULL) {
        Rval = CheckApi->pVoutDveCtrl(VoutIdx);
    }
    /* HW/SW resources check */
    if ((Rval == OK) &&
        (ResourceApi != NULL)) {
        Rval = ResourceApi->pVoutDveCtrl(VoutIdx);
    }

    /* Logic sanity check */
    return Rval;
}

/**
* Vout DVE control function
* @param [in]  VoutIdx vout index, only vout1
* @return ErrorCode
*/
UINT32 dsp_vout_dve_ctrl(const UINT8 VoutIdx)
{
    UINT32 Rval;
    ULONG ULAddr = 0U;
    CTX_VOUT_INFO_s VoutInfo = {0};
    AMBA_DSP_VOUT_TVE_DATA_s *pVoutTveCfg;

    Rval = HL_VoutDveCtrlChk(VoutIdx);

    if (Rval == OK) {
        HL_GetVoutInfo(HL_MTX_OPT_GET, VoutIdx, &VoutInfo);
        if ((DSP_PROF_STATUS_INVALID != DSP_GetProfState()) &&
            (DSP_VOUT_DISPLAY_NO_INIT != VoutInfo.DisplayStatus)) {
            if (VoutInfo.DveStatus == DSP_VOUT_DVE_CFGED) {
                UINT8 CmdBufferId = 0U; void *CmdBufferAddr = NULL;
                cmd_vout_dve_setup_t *DevSetup = HL_DefCtxCmdBufPtrVoutDve;

                HL_AcqCmdBuffer(&CmdBufferId, &CmdBufferAddr);
                dsp_osal_typecast(&DevSetup, &CmdBufferAddr);
                DevSetup->vout_id = VoutIdx;
                DSP_GetDspVoutTveCfg(VoutIdx, &pVoutTveCfg);
                dsp_osal_typecast(&ULAddr, &pVoutTveCfg);
                //dsp_osal_typecast(&pVoid, &ULAddr);
                //(void)dsp_osal_cache_clean(pVoid, DSP_TVE_CFG_SIZE);
                (void)dsp_osal_virt2cli(ULAddr , &DevSetup->dve_config_dram_addr);
                Rval = AmbaHL_CmdVoutDveSetup(AMBA_DSP_CMD_VDSP_NORMAL_WRITE, DevSetup);
                HL_RelCmdBuffer(CmdBufferId);
                if (Rval == OK) {
                    VoutInfo.DveStatus = DSP_VOUT_DVE_START;
                }
            }
        } else {
            if ((VoutInfo.DveStatus == DSP_VOUT_DVE_START) ||
                (VoutInfo.DveStatus == DSP_VOUT_DVE_CFGED)) {
                // Do Nothing
            } else {
                VoutInfo.DveStatus = DSP_VOUT_DVE_NO_INIT;
            }
        }
        HL_SetVoutInfo(HL_MTX_OPT_SET, VoutIdx, &VoutInfo);
    }

    if (Rval != OK) {
        AmbaLL_LogUInt5("AmbaDSP_VoutDveCtrl[%d] Error: %d", VoutIdx, Rval, 0U, 0U, 0U);
    }

    return Rval;
}

UINT32 HL_VoutResetSetup(const UINT8 VoutIdx,
                         const UINT8 WriteMode)
{
    UINT8 CmdBufferId = 0U;
    void *CmdBufferAddr = NULL;
    UINT32 Rval;
    cmd_vout_reset_t *pVoutReset = HL_DefCtxCmdBufPtrVoutReset;
    UINT16 VoutPhysId = 0U;

    HL_AcqCmdBuffer(&CmdBufferId, &CmdBufferAddr);
    dsp_osal_typecast(&pVoutReset, &CmdBufferAddr);
    (void)HL_GetVoutPhysId(VoutIdx, &VoutPhysId);
    pVoutReset->vout_id = (UINT8)VoutPhysId;
    pVoutReset->virtual_chan_id = (UINT8)0U; //DONT CARE
    pVoutReset->reset_mixer = 1U;
    pVoutReset->reset_disp = 1U;

    Rval = AmbaHL_CmdVoutReset(WriteMode, pVoutReset);
    HL_RelCmdBuffer(CmdBufferId);
    if (Rval != OK) {
        AmbaLL_LogUInt5("Vout[%d] reset fail", VoutIdx, 0U, 0U, 0U, 0U);
    }

    return Rval;
}

UINT32 HL_VoutDisplaySetup(const UINT8 VoutIdx, const UINT8 WriteMode)
{
    UINT8 CmdBufferId = 0U;
    void *CmdBufferAddr = NULL;
    UINT32 Rval;
    cmd_vout_display_setup_t *DisplaySetup = HL_DefCtxCmdBufPtrVoutDisp;
    CTX_VOUT_INFO_s VoutInfo = {0};
    UINT32 Enable;
#if 0
    void *pVoid = NULL;
#endif
    ULONG ULAddr = 0U;
    AMBA_DSP_VOUT_DISP_CFG_DATA_s *pVoutDispCfg;
    UINT16 VoutPhysId = 0U, VoutVirtId = 0U;
    CTX_VIN_INFO_s VinInfo = {0};
    UINT32 VinInterval = 0U;

    HL_GetVoutInfo(HL_MTX_OPT_GET, VoutIdx, &VoutInfo);
    HL_AcqCmdBuffer(&CmdBufferId, &CmdBufferAddr);
    dsp_osal_typecast(&DisplaySetup, &CmdBufferAddr);
    (void)HL_GetVoutPhysId(VoutIdx, &VoutPhysId);
    (void)HL_GetVoutVirtId(VoutIdx, &VoutVirtId);
    DisplaySetup->vout_id = (UINT8)VoutPhysId;
    DisplaySetup->virtual_chan_id = (UINT8)VoutVirtId;
    DisplaySetup->vin_vout_vsync_delay_ms_x10 = VoutInfo.DispCfg.VinVoutSyncDelay;
    DisplaySetup->dual_vout_vysnc_delay_ms_x10 = VoutInfo.DispCfg.VoutSyncDelay;
    if (VoutIdx == VOUT_IDX_A) {
        /* Inside uCode, vout introduces single thread for multi vout, so using HL to control sync timing */
        Enable = (VoutInfo.DispCfg.VinVoutSyncDelay > 0U)? (UINT32)1U: (UINT32)0U;
        DisplaySetup->vout_sync_with_vin = (UINT16)Enable;
        if (Enable > 0U) {
            /*
             * 20220506, ucoder suggest to have 2~3 vin interval
             */
            HL_GetVinInfo(HL_MTX_OPT_ALL, (UINT16)VoutInfo.DispCfg.VinTargetIndex, &VinInfo);
            if ((VinInfo.FrameRate.NumUnitsInTick > 0U) &&
                (VinInfo.FrameRate.TimeScale > 0U)) {
                VinInterval = (VinInfo.FrameRate.NumUnitsInTick*1000U)/VinInfo.FrameRate.TimeScale;
                VinInterval *= 3U;
            }
            DisplaySetup->vin_vout_vsync_timeout_ms = (VinInterval > 0U)? (UINT16)VinInterval: 20U;
        }
//        (void)AmbaRTSL_Vout0SetVinVoutSync(Enable);

        DSP_GetDspVoutDispCfg(VoutIdx, &pVoutDispCfg);
        dsp_osal_typecast(&ULAddr, &pVoutDispCfg);
#if 0
        dsp_osal_typecast(&pVoid, &ULAddr);
        (void)dsp_osal_cache_clean(pVoid, 508U/*sizeof(AMBA_VOUT_DISPLAY0_CONFIG_REG_s)*/);
#endif
        (void)dsp_osal_virt2cli(ULAddr, &DisplaySetup->disp_config_dram_addr);
    } else {
        Enable = (VoutInfo.DispCfg.VinVoutSyncDelay > 0U)? (UINT32)1U: (UINT32)0U;
        DisplaySetup->vout_sync_with_vin = (UINT16)Enable;
        if (Enable > 0U) {
            /*
             * 20220506, ucoder suggest to have 2~3 vin interval
             */
            HL_GetVinInfo(HL_MTX_OPT_ALL, (UINT16)VoutInfo.DispCfg.VinTargetIndex, &VinInfo);
            if ((VinInfo.FrameRate.NumUnitsInTick > 0U) &&
                (VinInfo.FrameRate.TimeScale > 0U)) {
                VinInterval = (VinInfo.FrameRate.NumUnitsInTick*1000U)/VinInfo.FrameRate.TimeScale;
                VinInterval *= 3U;
            }
            DisplaySetup->vin_vout_vsync_timeout_ms = (VinInterval > 0U)? (UINT16)VinInterval: 20U;
        }
//        (void)AmbaRTSL_Vout1SetVinVoutSync(Enable);

        DSP_GetDspVoutDispCfg(VoutIdx, &pVoutDispCfg);
        dsp_osal_typecast(&ULAddr, &pVoutDispCfg);
#if 0
        dsp_osal_typecast(&pVoid, &ULAddr);
        (void)dsp_osal_cache_clean(pVoid, 508U/*sizeof(AMBA_VOUT_DISPLAY1_CONFIG_REG_s)*/);
#endif
        (void)dsp_osal_virt2cli(ULAddr, &DisplaySetup->disp_config_dram_addr);
    }

    if (VoutIdx == VOUT_IDX_B) {
        if (VoutInfo.DataPath.DisplayOption == 0U/*HDMI or DVE*/) {
            DisplaySetup->is_use_disp_c = (UINT8)1U;

            if (VoutInfo.MixerCfg.FrameRate.Interlace == (UINT8)1U) {
                if (VoutInfo.DveStatus >= DSP_VOUT_DVE_CFGED) {
                    DisplaySetup->reverse_polarity = 0U;
                } else {
                    DisplaySetup->reverse_polarity = 1U;
                }
            }
        } else {
            DisplaySetup->is_use_disp_c = (UINT8)0U;
        }
    } else {
        DisplaySetup->is_use_disp_c = (UINT8)0U;
    }

    Rval = AmbaHL_CmdVoutDisplaySetup(WriteMode, DisplaySetup);
    HL_RelCmdBuffer(CmdBufferId);
    if (Rval == OK) {
        VoutInfo.DisplayStatus = DSP_VOUT_DISPLAY_START;
    } else {
        AmbaLL_LogUInt5("Display[%d] Setup fail", VoutIdx, 0U, 0U, 0U, 0U);
    }

    HL_SetVoutInfo(HL_MTX_OPT_SET, VoutIdx, &VoutInfo);

    return Rval;
}

UINT32 HL_VoutMixerSetup(const UINT8 VoutIdx, const UINT8 WriteMode)
{
    UINT8 ExitLoop;
    UINT8 CmdBufferId = 0U;
    void *CmdBufferAddr = NULL;
    UINT16 i;
    UINT32 Rval;
    UINT32 FrameRate = 0U;
    CTX_VOUT_INFO_s VoutInfo = {0};
    cmd_vout_mixer_setup_t *pVoutMixerSetup = HL_DefCtxCmdBufPtrVoutMx;
    AMBA_DSP_VOUT_MIXER_CSC_DATA_s *pVoutMixerCsc = NULL;
    UINT16 VoutPhysId = 0U, VoutVirtId = 0U;
    UINT8 InFmt, OutFmt;

    HL_GetVoutInfo(HL_MTX_OPT_ALL, VoutIdx, &VoutInfo);
    HL_AcqCmdBuffer(&CmdBufferId, &CmdBufferAddr);
    dsp_osal_typecast(&pVoutMixerSetup, &CmdBufferAddr);
    (void)HL_GetVoutPhysId(VoutIdx, &VoutPhysId);
    (void)HL_GetVoutVirtId(VoutIdx, &VoutVirtId);
    pVoutMixerSetup->vout_id = VoutPhysId;
    pVoutMixerSetup->virtual_chan_id = VoutVirtId;
    pVoutMixerSetup->reverse_en = VoutInfo.MixerCfg.VideoHorReverseEnable;
    (void)HL_FRAME_RATE_MAP(VOUT_FRAME_RATE, VoutInfo.MixerCfg.FrameRate, &FrameRate);
    pVoutMixerSetup->frm_rate = (UINT8)FrameRate;
    pVoutMixerSetup->interlaced = VoutInfo.MixerCfg.FrameRate.Interlace;
    pVoutMixerSetup->act_win_width = VoutInfo.MixerCfg.ActiveWidth;
    pVoutMixerSetup->act_win_height = VoutInfo.MixerCfg.ActiveHeight;
    pVoutMixerSetup->back_ground_v = HL_VOUT_GET_YUV(VOUT_GET_V, VoutInfo.MixerBGColor);
    pVoutMixerSetup->back_ground_u = HL_VOUT_GET_YUV(VOUT_GET_U, VoutInfo.MixerBGColor);
    pVoutMixerSetup->back_ground_y = HL_VOUT_GET_YUV(VOUT_GET_Y, VoutInfo.MixerBGColor);
    InFmt = (UINT8)DSP_GetU8Bit(VoutInfo.MixerCfg.MixerColorFormat, 0U, 4U);
    OutFmt = (UINT8)DSP_GetU8Bit(VoutInfo.MixerCfg.MixerColorFormat, 4U, 4U);
    pVoutMixerSetup->mixer_444 = InFmt;
    pVoutMixerSetup->mixer_420 = OutFmt;

    pVoutMixerSetup->highlight_v = HL_VOUT_GET_YUV(VOUT_GET_V, VoutInfo.MixerHighlightColor);
    pVoutMixerSetup->highlight_u = HL_VOUT_GET_YUV(VOUT_GET_U, VoutInfo.MixerHighlightColor);
    pVoutMixerSetup->highlight_y = HL_VOUT_GET_YUV(VOUT_GET_Y, VoutInfo.MixerHighlightColor);
    pVoutMixerSetup->highlight_thresh = VoutInfo.MixerHighlightThreshold;
    pVoutMixerSetup->csc_en = VoutInfo.MixerCscCtrl;

    /* Set default CSC if user leave it as blank */
    if ((VoutInfo.MixerCscCtrl == MIXER_CSC_FOR_VIDEO) ||
        (VoutInfo.MixerCscCtrl == MIXER_CSC_FOR_OSD)) {
        ExitLoop = 0U;
        for (i=0U; i<DSP_VOUT_CSC_MATRIX_DEPTH; i++) {
            if (VoutInfo.CscMatrix[i] != 0U) {
                ExitLoop = 1U;
                break;
            }
        }

        if (ExitLoop == 0U) {
            //No CSC matrix, leverage Display's setting
            AmbaLL_LogUInt5("Null MixerCscMatrix[%d] use Display one", VoutIdx, 0U, 0U, 0U, 0U);
            DSP_GetDspVoutMixerCscCfg(VoutIdx, &pVoutMixerCsc);
            (void)dsp_osal_memcpy(pVoutMixerSetup->csc_parms, pVoutMixerCsc, sizeof(UINT32)*DSP_VOUT_CSC_MATRIX_DEPTH);
        } else {
            (void)dsp_osal_memcpy(pVoutMixerSetup->csc_parms, VoutInfo.CscMatrix, sizeof(UINT32)*DSP_VOUT_CSC_MATRIX_DEPTH);
        }
    } else {
        //No CSC Matrix needed
    }

    Rval = AmbaHL_CmdVoutMixerSetup(WriteMode, pVoutMixerSetup);
    HL_RelCmdBuffer(CmdBufferId);
    if (Rval == OK) {
        //TBD
    } else {
        AmbaLL_LogUInt5("Mixer[%d] setup fail", VoutIdx, 0U, 0U, 0U, 0U);
    }

    return Rval;
}

UINT32 HL_VoutOsdSetup(const UINT8 VoutIdx, const UINT8 WriteMode)
{
    UINT8 CmdBufferId = 0U;
    void *CmdBufferAddr = NULL;
    UINT32 Rval = OK;
    CTX_VOUT_INFO_s VoutInfo = {0};
    cmd_vout_osd_setup_t *OsdSetup = HL_DefCtxCmdBufPtrVoutOsd;

    HL_GetVoutInfo(HL_MTX_OPT_ALL, VoutIdx, &VoutInfo);
    if (VoutInfo.OsdCfg.DataFormat == OSD_8BIT_CLUT_MODE) {
        cmd_vout_osd_clut_setup_t *OsdClutCfg = HL_DefCtxCmdBufPtrVoutOsdClut;
        UINT16 VoutPhysId = 0U, VoutVirtId = 0U;

        HL_AcqCmdBuffer(&CmdBufferId, &CmdBufferAddr);
        dsp_osal_typecast(&OsdClutCfg, &CmdBufferAddr);
        (void)HL_GetVoutPhysId(VoutIdx, &VoutPhysId);
        (void)HL_GetVoutVirtId(VoutIdx, &VoutVirtId);
        OsdClutCfg->vout_id = (UINT8)VoutPhysId;
        OsdClutCfg->virtual_chan_id = (UINT8)VoutVirtId;

        (void)dsp_osal_virt2cli(VoutInfo.OsdCfg.CLUTAddr, &OsdClutCfg->clut_dram_addr);
        (void)AmbaHL_CmdVoutOsdClutSetup(WriteMode, OsdClutCfg);
        HL_RelCmdBuffer(CmdBufferId);
    }

    HL_GetVoutInfo(HL_MTX_OPT_GET, VoutIdx, &VoutInfo);
    HL_AcqCmdBuffer(&CmdBufferId, &CmdBufferAddr);
    dsp_osal_typecast(&OsdSetup, &CmdBufferAddr);
    (void)HL_FillOsdSetup(VoutIdx, &VoutInfo.OsdCfg, OsdSetup);
    OsdSetup->en = VoutInfo.OsdEnable;
    (void)AmbaHL_CmdVoutOsdSetup(WriteMode, OsdSetup);
    HL_RelCmdBuffer(CmdBufferId);
    HL_SetVoutInfo(HL_MTX_OPT_SET, VoutIdx, &VoutInfo);

    return Rval;
}

UINT32 HL_VoutVideoSetup(const UINT8 VoutIdx, const UINT8 WriteMode)
{
    UINT8 CmdBufferId = 0U;
    void *CmdBufferAddr = NULL;
    UINT32 Rval;
    CTX_VOUT_INFO_s VoutInfo = {0};
    cmd_vout_video_setup_t *pVideoSetup = HL_DefCtxCmdBufPtrVoutVdo;
    UINT16 VoutPhysId = 0U, VoutVirtId = 0U;
    CTX_RESOURCE_INFO_s *pResource = HL_CtxResInfoPtr;
    CTX_YUV_STRM_INFO_s *pYuvStrmInfo = HL_CtxYuvStrmInfoPtr;
    UINT16 i;
    UINT8 IsVoutPurpose = 0U, IsLdPurpose = 0U;
    UINT16 YuvStrmVoutId = 0U;

    HL_GetResourcePtr(&pResource);

    HL_GetVoutInfo(HL_MTX_OPT_ALL, VoutIdx, &VoutInfo);
    HL_AcqCmdBuffer(&CmdBufferId, &CmdBufferAddr);
    dsp_osal_typecast(&pVideoSetup, &CmdBufferAddr);

    (void)HL_GetVoutPhysId(VoutIdx, &VoutPhysId);
    (void)HL_GetVoutVirtId(VoutIdx, &VoutVirtId);
    pVideoSetup->vout_id = (UINT8)VoutPhysId;
    pVideoSetup->virtual_chan_id = (UINT8)VoutVirtId;

    pVideoSetup->en = VoutInfo.VideoEnable;
    pVideoSetup->src = VoutInfo.Source;

    pVideoSetup->flip = HL_GET_FLIP(VoutInfo.VideoCfg.RotateFlip);
    pVideoSetup->rotate = HL_GET_ROTATE(VoutInfo.VideoCfg.RotateFlip);
    pVideoSetup->win_offset_x = VoutInfo.VideoCfg.Window.OffsetX;
    pVideoSetup->win_offset_y = VoutInfo.VideoCfg.Window.OffsetY;
    pVideoSetup->win_width = VoutInfo.VideoCfg.Window.Width;
    pVideoSetup->win_height = VoutInfo.VideoCfg.Window.Height;
    (void)dsp_osal_virt2cli(VoutInfo.VideoCfg.DefaultImgConfig.BaseAddrY, &pVideoSetup->default_img_y_addr);
    (void)dsp_osal_virt2cli(VoutInfo.VideoCfg.DefaultImgConfig.BaseAddrUV, &pVideoSetup->default_img_uv_addr);
    pVideoSetup->default_img_pitch = VoutInfo.VideoCfg.DefaultImgConfig.Pitch;
    pVideoSetup->default_img_repeat_field = VoutInfo.VideoCfg.DefaultImgConfig.FieldRepeat;
    pVideoSetup->default_img_ignore_rotate_flip = 0U;
    pVideoSetup->default_img_ch_fmt = VoutInfo.VideoCfg.DefaultImgConfig.DataFormat;
    pVideoSetup->interlaced = VoutInfo.MixerCfg.FrameRate.Interlace;

    pVideoSetup->is_video_lowdelay = 0U;
    for (i = 0U; i < pResource->YuvStrmNum; i++) {
        HL_GetYuvStrmInfoPtr(i, &pYuvStrmInfo);
        IsVoutPurpose = (UINT8)DSP_GetU16Bit(pYuvStrmInfo->Purpose, DSP_LV_STRM_PURPOSE_VOUT_IDX, 1U);
        IsLdPurpose = (UINT8)DSP_GetU16Bit(pYuvStrmInfo->Purpose, DSP_LV_STRM_PURPOSE_LD_IDX, 1U);
        DSP_Bit2U16Idx(pYuvStrmInfo->DestVout, &YuvStrmVoutId);
        if ((IsVoutPurpose == 1U) &&
            (IsLdPurpose == 1U) &&
            ((UINT8)YuvStrmVoutId == VoutIdx)) {
            pVideoSetup->is_video_lowdelay = 1U;
        }
    }

    Rval = AmbaHL_CmdVoutVideoSetup(WriteMode, pVideoSetup);
    HL_RelCmdBuffer(CmdBufferId);

    return Rval;
}

UINT32 HL_VoutVideoSetupSourceSelect(const UINT8 VoutIdx, const UINT8 Src, const UINT8 Content)
{
    UINT32 Rval = OK;
    CTX_VOUT_INFO_s VoutInfo = {0};
    UINT8 NewSrc = Src;

    if (Src == DSP_VOUT_SRC_DFTIMG) {
        if (Content == HL_VOUT_SOURCE_VIDEO_CAP) {
            // DO NOTHING
        } else if (Content == HL_VOUT_SOURCE_NONE) {
            // DO NOTHING
        } else {
            AmbaLL_LogUInt5("Unknown content[%d %d %d]", Content, Src, VoutIdx, 0U, 0U);
            Rval = DSP_ERR_0001;
        }
    } else if (Src == DSP_VOUT_SRC_BG) {
        // DO NOTHING
    } else if (Src == DSP_VOUT_SRC_VCAP) {
        // At Encode Mode,
        // uCode will update Seamless buffer automatically when dsp_init.seamless = 1
        // SSP don't need to switch vout_src = 9, just keep in Vout_src = 2 if SSP want seamless effect
        HL_GetVoutInfo(HL_MTX_OPT_ALL, VoutIdx, &VoutInfo);
        if (VoutInfo.VideoCfg.VideoSource != VOUT_SOURCE_INTERNAL) {
            NewSrc = VoutInfo.VideoCfg.VideoSource;
        }
    } else {
        AmbaLL_LogUInt5("Unknown Src[%d %d %d]", Content, Src, VoutIdx, 0U, 0U);
        Rval = DSP_ERR_0001;
    }

    if (Rval == OK) {
        HL_GetVoutInfo(HL_MTX_OPT_GET, VoutIdx, &VoutInfo);
        VoutInfo.Source = NewSrc;
        HL_SetVoutInfo(HL_MTX_OPT_SET, VoutIdx, &VoutInfo);
    }

    return Rval;
}

UINT32 HL_VoutDveSetup(const UINT8 VoutIdx, const UINT8 WriteMode)
{
    UINT32 Rval = OK;
    ULONG ULAddr = 0U;
    CTX_VOUT_INFO_s VoutInfo = {0};
    AMBA_DSP_VOUT_TVE_DATA_s *pVoutTveCfg = NULL;

    if (VoutIdx == VOUT_IDX_A) {
        // DO NOTHING
    } else {
        HL_GetVoutInfo(HL_MTX_OPT_GET, VoutIdx, &VoutInfo);
        /* Consider mode swtich case to issue dve setting */
        if ((VoutInfo.DveStatus == DSP_VOUT_DVE_CFGED) ||
            ((VoutInfo.DveStatus == DSP_VOUT_DVE_START) &&
             (WriteMode == AMBA_DSP_CMD_DEFAULT_WRITE))) {
            UINT8 CmdBufferId = 0U;
            void *CmdBufferAddr = NULL;
            cmd_vout_dve_setup_t *pDevSetup = HL_DefCtxCmdBufPtrVoutDve;

            HL_AcqCmdBuffer(&CmdBufferId, &CmdBufferAddr);
            dsp_osal_typecast(&pDevSetup, &CmdBufferAddr);
            pDevSetup->vout_id = VoutIdx;
            DSP_GetDspVoutTveCfg(VoutIdx, &pVoutTveCfg);
            dsp_osal_typecast(&ULAddr, &pVoutTveCfg);
            //dsp_osal_typecast(&pVoid, &ULAddr);
            //(void)dsp_osal_cache_clean(pVoid, DSP_TVE_CFG_SIZE);
            (void)dsp_osal_virt2cli(ULAddr , &pDevSetup->dve_config_dram_addr);
            Rval = AmbaHL_CmdVoutDveSetup(WriteMode, pDevSetup);
            HL_RelCmdBuffer(CmdBufferId);
            if (Rval == OK) {
                VoutInfo.DveStatus = DSP_VOUT_DVE_START;
            } else {
                AmbaLL_LogUInt5("Display[%d] Dve Setup fail", VoutIdx, 0U, 0U, 0U, 0U);
            }
        }

        HL_SetVoutInfo(HL_MTX_OPT_SET, VoutIdx, &VoutInfo);
    }

    return Rval;
}

UINT32 HL_VoutGammaSetup(const UINT8 VoutIdx, const UINT8 WriteMode)
{
    UINT32 Rval = OK;
    CTX_VOUT_INFO_s VoutInfo = {0};

    if (VoutIdx == VOUT_IDX_A) {
        UINT8 CmdBufferId = 0U;
        void *CmdBufferAddr = NULL;
        cmd_vout_gamma_setup_t *GammaSetup = HL_DefCtxCmdBufPtrVoutGamma;
        UINT16 VoutPhysId = 0U, VoutVirtId = 0U;

        HL_GetVoutInfo(HL_MTX_OPT_ALL, VoutIdx, &VoutInfo);
        HL_AcqCmdBuffer(&CmdBufferId, &CmdBufferAddr);
        dsp_osal_typecast(&GammaSetup, &CmdBufferAddr);
        (void)HL_GetVoutPhysId(VoutIdx, &VoutPhysId);
        (void)HL_GetVoutVirtId(VoutIdx, &VoutVirtId);
        GammaSetup->vout_id = (UINT8)VoutPhysId;
        GammaSetup->virtual_chan_id = (UINT8)VoutVirtId;
        if (VoutInfo.GammaEnable == (UINT8)1U) {
            GammaSetup->enable = (UINT8)1U;
            GammaSetup->setup_gamma_table = (UINT8)1U;
            (void)dsp_osal_virt2cli(VoutInfo.GammaTableAddr, &GammaSetup->gamma_dram_addr);
        } else {
            GammaSetup->enable = (UINT8)0U;
        }
        Rval = AmbaHL_CmdVoutGammaSetup(WriteMode, GammaSetup);
        HL_RelCmdBuffer(CmdBufferId);
        if (Rval != OK) {
            AmbaLL_LogUInt5("Display[%d] Gamma Setup fail", VoutIdx, 0U, 0U, 0U, 0U);
        }
    } else {
        // DO NOTHING
    }

    return Rval;
}

UINT32 dsp_vout_drv_cfg(UINT16 VoutId, UINT16 Type, const void *pDrvData)
{
    UINT32 Rval;
    AMBA_DSP_VOUT_DISP_CFG_DATA_s *pVoutDispCfg = NULL;
    AMBA_DSP_VOUT_MIXER_CSC_DATA_s *pVoutMixerCsc = NULL;
    AMBA_DSP_VOUT_TVE_DATA_s *pVoutTveCfg = NULL;

    /* DSP CtxInit */
    Rval = HL_CtxInit();

    /* Input sanity check */
    if (VoutId >= NUM_VOUT_IDX) {
        Rval = DSP_ERR_0001;
    } else if (Type >= NUM_DSP_VOUT_DRV_CFG_TYPE) {
        Rval = DSP_ERR_0001;
    } else if (pDrvData == NULL) {
        Rval = DSP_ERR_0000;
    } else {
        // DO NOTHING
    }

    /* HW/SW resources check */

    /* Timing sanity check */

    /* Logic sanity check */

    /* Body */
    if (Rval == OK) {
        if (Type == DSP_VOUT_DRV_CFG_TYPE_DISP) {
            DSP_GetDspVoutDispCfg(VoutId, &pVoutDispCfg);
            (void)dsp_osal_memcpy(pVoutDispCfg, pDrvData, sizeof(AMBA_DSP_VOUT_DISP_CFG_DATA_s));
        } else if (Type == DSP_VOUT_DRV_CFG_TYPE_MIXER_CSC) {
            DSP_GetDspVoutMixerCscCfg(VoutId, &pVoutMixerCsc);
            (void)dsp_osal_memcpy(pVoutMixerCsc, pDrvData, sizeof(AMBA_DSP_VOUT_MIXER_CSC_DATA_s));
        } else if (Type == DSP_VOUT_DRV_CFG_TYPE_TVE) {
            DSP_GetDspVoutTveCfg(VoutId, &pVoutTveCfg);
            (void)dsp_osal_memcpy(pVoutTveCfg, pDrvData, sizeof(AMBA_DSP_VOUT_TVE_DATA_s));
        } else {
            // DO NOTHING
        }
    }

    if (Rval != OK) {
        AmbaLL_LogUInt5("dsp_vout_drv_cfg Error[0x%X]", Rval, 0U, 0U, 0U, 0U);
    }

    return Rval;
}

