/**
 *  @file SvcDisplay.c
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
 *  @details svc display functions
 *
 */

#include "AmbaMisraFix.h"
#include "AmbaWrap.h"
#include "AmbaHDMI.h"
#include "AmbaDSP_VOUT.h"
#include "AmbaFPD.h"

#include "SvcLog.h"
#include "SvcErrCode.h"
#include "SvcDisplay.h"

#define SVC_LOG_DISP        "DISP"

static UINT32                   g_NumDisp GNU_SECTION_NOZEROINIT;
static SVC_DISP_CFG_s           g_DispCfg[AMBA_DSP_MAX_VOUT_NUM] GNU_SECTION_NOZEROINIT;
static UINT32                   g_DispVideoCtrl[AMBA_DSP_MAX_VOUT_NUM] GNU_SECTION_NOZEROINIT;
static UINT32                   g_DispOsdCtrl[AMBA_DSP_MAX_VOUT_NUM] GNU_SECTION_NOZEROINIT;
static AMBA_DSP_VOUT_CSC_MATRIX_s g_DispCscMatrix[AMBA_DSP_MAX_VOUT_NUM] = {
#ifndef CONFIG_ICAM_32BITS_OSD_USED
    [VOUT_IDX_A] = {
        .Coef = {
            [0] = {  0.29900f,  0.58700f,  0.11400f, },
            [1] = { -0.16874f, -0.33126f,  0.50000f, },
            [2] = {  0.50000f, -0.41869f, -0.08131f, },
        },
        .Offset = { [0] = 0.00f, [1] = 128.00f, [2] = 128.00f  },
        .MinVal = { [0] = 0U,    [1] = 0U,      [2] = 0U       },
        .MaxVal = { [0] = 255U,  [1] = 255U,    [2] = 255U     },
    },
    #if (AMBA_DSP_MAX_VOUT_NUM > 1U)
    [VOUT_IDX_B] = {
        .Coef = {
            [0] = {  0.29900f,  0.58700f,  0.11400f, },
            [1] = { -0.16874f, -0.33126f,  0.50000f, },
            [2] = {  0.50000f, -0.41869f, -0.08131f, },
        },
        .Offset = { [0] = 0.00f, [1] = 128.00f, [2] = 128.00f  },
        .MinVal = { [0] = 0U,    [1] = 0U,      [2] = 0U       },
        .MaxVal = { [0] = 255U,  [1] = 255U,    [2] = 255U     },
    },
    #endif
#else
    [VOUT_IDX_A] = {
        .Coef = {
            [0] = {  0.58700f,  0.29900f,  0.11400f, },
            [1] = { -0.33126f, -0.16874f,  0.50000f, },
            [2] = { -0.41869f,  0.50000f, -0.08131f, },
        },
        .Offset = { [0] = 0.00f, [1] = 128.00f, [2] = 128.00f  },
        .MinVal = { [0] = 0U,    [1] = 0U,      [2] = 0U       },
        .MaxVal = { [0] = 255U,  [1] = 255U,    [2] = 255U     },
    },
    #if (AMBA_DSP_MAX_VOUT_NUM > 1U)
    [VOUT_IDX_B] = {
        .Coef = {
            [0] = {  0.58700f,  0.29900f,  0.11400f, },
            [1] = { -0.33126f, -0.16874f,  0.50000f, },
            [2] = { -0.41869f,  0.50000f, -0.08131f, },
        },
        .Offset = { [0] = 0.00f, [1] = 128.00f, [2] = 128.00f  },
        .MinVal = { [0] = 0U,    [1] = 0U,      [2] = 0U       },
        .MaxVal = { [0] = 255U,  [1] = 255U,    [2] = 255U     },
    },
    #endif
#endif
};

/**
* get info block of display module
* @param [out] pInfo info block of display
* @return none
*/
void SvcDisplay_InfoGet(SVC_DISP_INFO_s *pInfo)
{
    pInfo->pNumDisp = &g_NumDisp;
    pInfo->pDispCfg = g_DispCfg;
}

/**
* initialization of display module
* @return none
*/
void SvcDisplay_Init(void)
{
    g_NumDisp = 0U;
    if (SVC_OK != AmbaWrap_memset(g_DispCfg, 0, sizeof(g_DispCfg))) {
        SvcLog_NG(SVC_LOG_DISP, "SvcDisplay_Init err, AmbaWrap_memset failed, g_DispCfg", 0U, 0U);
    }
    if (SVC_OK != AmbaWrap_memset(g_DispVideoCtrl, 0, sizeof(g_DispVideoCtrl))) {
        SvcLog_NG(SVC_LOG_DISP, "SvcDisplay_Init err, AmbaWrap_memset failed, g_DispVideoCtrl", 0U, 0U);
    }
    if (SVC_OK != AmbaWrap_memset(g_DispOsdCtrl, 0, sizeof(g_DispOsdCtrl))) {
        SvcLog_NG(SVC_LOG_DISP, "SvcDisplay_Init err, AmbaWrap_memset failed, g_DispOsdCtrl", 0U, 0U);
    }
}

/**
* configuration of display module
* @return none
*/
void SvcDisplay_Config(void)
{
    UINT32                    i, Rval, VoutID;
    // AMBA_DSP_DISPLAY_CONFIG_s DispCfg = {0U};
    UINT8                     MixCscCtrl;

    for (i = 0; i < g_NumDisp; i++) {
        VoutID = g_DispCfg[i].VoutID;
        SvcLog_OK(SVC_LOG_DISP, "SvcDisplay_Config %u vout %u", i, VoutID);

        Rval = AmbaDSP_VoutDisplayConfig((UINT8)VoutID, &(g_DispCfg[i].SyncCfg));
        if (Rval != OK) {
            SvcLog_NG(SVC_LOG_DISP, "## fail to config VoutDisplay(%u), rval(%u)", i, Rval);
        }

        /* configure DVE */
        switch (g_DispCfg[i].CvbsMode) {
        case SVC_DISP_CVBS_NTSC:
            Rval = AmbaDSP_VoutDveConfig((UINT8)VoutID, VOUT_DVE_NTSC);
            break;
        case SVC_DISP_CVBS_PAL:
            Rval = AmbaDSP_VoutDveConfig((UINT8)VoutID, VOUT_DVE_PAL);
            break;
        default:
            Rval = OK;
            break;
        }

        if (Rval != OK) {
            SvcLog_NG(SVC_LOG_DISP, "## fail to config DVE(%u), rval(%u)", i, Rval);
        }

        /* configure mixer */
        if (OK != AmbaDSP_VoutMixerCscMatrixConfig((UINT8)VoutID, &(g_DispCscMatrix[VoutID]))) {
            MixCscCtrl = MIXER_CSC_DISABLE;
            SvcLog_NG(SVC_LOG_DISP, "## fail to config mixer-csc matrix(%u), rval(%u)", i, Rval);
        } else {
            MixCscCtrl = MIXER_CSC_FOR_OSD;
        }
        Rval = AmbaDSP_VoutMixerConfigCsc((UINT8)VoutID, MixCscCtrl);
        if (Rval != OK) {
            SvcLog_NG(SVC_LOG_DISP, "## fail to config mixer-csc(%u), rval(%u)", i, Rval);
        }

        Rval = AmbaDSP_VoutMixerConfig((UINT8)VoutID, &(g_DispCfg[i].MixerCfg));
        if (Rval != OK) {
            SvcLog_NG(SVC_LOG_DISP, "## fail to config mixer(%u), rval(%u)", i, Rval);
        }

        Rval = AmbaDSP_VoutMixerConfigHighlightColor((UINT8)VoutID, 255U, 0x9292D2);
        if (Rval != OK) {
            SvcLog_NG(SVC_LOG_DISP, "## fail to config mixer-HC(%u), rval(%u)", i, Rval);
        }

        Rval = AmbaDSP_VoutMixerConfigBackColor((UINT8)VoutID, 0x008080U);
        if (Rval != OK) {
            SvcLog_NG(SVC_LOG_DISP, "## fail to config mixer-BG(%u), rval(%u)", i, Rval);
        }

        /* configure video */
        Rval = AmbaDSP_VoutVideoConfig((UINT8)VoutID, &(g_DispCfg[i].VideoCfg));
        if (Rval != OK) {
            SvcLog_NG(SVC_LOG_DISP, "## fail to config vout video(%u), rval(%u)", i, Rval);
        }
    }
}

/**
* set vout default image
* @param [in] Enable 0 - disable, 1 - enable
* @param [in] VoutID vout index
* @param [in] pCfg pointer to default image configuration
* @return 0-OK, 1-NG
*/
UINT32 SvcDisplay_SetDefImg(UINT8 Enable, UINT8 VoutID, const AMBA_DSP_VOUT_DEFAULT_IMG_CONFIG_s *pCfg)
{
    UINT32 Rval = SVC_NG, i;
    for (i = 0; i < AMBA_DSP_MAX_VOUT_NUM; i ++) {
        if (g_DispCfg[i].VoutID == VoutID) {
            if (Enable == 1U) {
                AMBA_DSP_VOUT_VIDEO_CFG_s VideoCfg;
                Rval = AmbaWrap_memcpy(&VideoCfg, &(g_DispCfg[i].VideoCfg), sizeof(AMBA_DSP_VOUT_VIDEO_CFG_s));
                if (Rval != OK) {
                    SvcLog_NG(SVC_LOG_DISP, "[line %u] AmbaWrap_memcpy return 0x%x", __LINE__, Rval);
                } else {
                    VideoCfg.VideoSource = VOUT_SOURCE_DEFAULT_IMAGE;
                    Rval = AmbaWrap_memcpy(&(VideoCfg.DefaultImgConfig), pCfg, sizeof(AMBA_DSP_VOUT_DEFAULT_IMG_CONFIG_s));
                    if (Rval != OK) {
                        SvcLog_NG(SVC_LOG_DISP, "[line %u] AmbaWrap_memcpy return 0x%x", __LINE__, Rval);
                    }
                }
                if (Rval == OK) {
                    Rval = AmbaDSP_VoutVideoConfig(VoutID, &VideoCfg);
                    if (Rval != OK) {
                        SvcLog_NG(SVC_LOG_DISP, "## failed to config vout video(%u), rval(%u)", VoutID, Rval);
                    } else {
                        Rval = AmbaDSP_VoutVideoCtrl(VoutID, 1, 1, NULL);
                    }
                }
            } else {
                Rval = AmbaDSP_VoutVideoConfig(VoutID, &(g_DispCfg[i].VideoCfg));
                if (Rval != OK) {
                    SvcLog_NG(SVC_LOG_DISP, "## failed to config vout video(%u), rval(%u)", VoutID, Rval);
                } else {
                    Rval = AmbaDSP_VoutVideoCtrl(VoutID, 1, 1, NULL);
                }
            }
            break;
        }
    }
    if (i == AMBA_DSP_MAX_VOUT_NUM) {
        SvcLog_NG(SVC_LOG_DISP, "[SvcDisplay_SetDefImg] failed to find vout %u", VoutID, 0);
    }
    return Rval;
}

/**
* configuration update of display module
* @return none
*/
void SvcDisplay_Update(void)
{
    UINT32 Rval;
    UINT32 VoutID;
    UINT32 i;

    for (i = 0; i < g_NumDisp; i++) {
        VoutID = g_DispCfg[i].VoutID;
        SvcLog_OK(SVC_LOG_DISP, "SvcDisplay_Update %u vout %u", i, VoutID);

        /* configure video */
        Rval = AmbaDSP_VoutVideoConfig((UINT8)VoutID, &(g_DispCfg[i].VideoCfg));
        if (Rval != OK) {
            SvcLog_NG(SVC_LOG_DISP, "## fail to config vout video(%u), rval(%u)", i, Rval);
        }
    }
}

/**
* control of display module
* @param [in] NumCtrl number of display
* @param [in] pCtrlArr array of display control block
* @return none
*/
void SvcDisplay_Ctrl(UINT32 NumCtrl, const SVC_DISP_CTRL_s *pCtrlArr)
{
    UINT32  i, Idx, Rval, VoutID;
    UINT64  RawSeq = 0ULL;

    for (i = 0; i < NumCtrl; i++) {
        VoutID = pCtrlArr[i].VoutID;

        for (Idx = 0U; Idx < g_NumDisp; Idx++) {
            if (g_DispCfg[Idx].VoutID == VoutID) {
                break;
            }
        }

        if ((Idx == g_NumDisp) || (Idx >= AMBA_DSP_MAX_VOUT_NUM)) {
            continue;
        }

        if (pCtrlArr[i].EnableVideo == 1U) {
            Rval = AmbaDSP_VoutDisplayCtrl((UINT8)VoutID);
            if (Rval != OK) {
                SvcLog_NG(SVC_LOG_DISP, "## fail to control display(%u), rval(%u)", i, Rval);
            }

            if ((SVC_DISP_CVBS_NONE < g_DispCfg[Idx].CvbsMode)
                && (g_DispCfg[Idx].CvbsMode <= SVC_DISP_CVBS_PAL)) {
                Rval = AmbaDSP_VoutDveCtrl((UINT8)VoutID);
                if (Rval != OK) {
                    SvcLog_NG(SVC_LOG_DISP, "## fail to control vout dve (%u), rval(%u)", VoutID, Rval);
                }
            }

            Rval = AmbaDSP_VoutMixerCtrl((UINT8)VoutID);
            if (Rval != OK) {
                SvcLog_NG(SVC_LOG_DISP, "## fail to control mixer(%u), rval(%u)", i, Rval);
            }

            Rval = AmbaDSP_VoutVideoCtrl((UINT8)VoutID, 1, 0, &RawSeq);
            if (Rval != OK) {
                SvcLog_NG(SVC_LOG_DISP, "## fail to control vout video(%u), rval(%u)", i, Rval);
            }

            Rval = AmbaDSP_VoutOsdCtrl((UINT8)VoutID, (UINT8)pCtrlArr[i].EnableOsd, 0, &RawSeq);
            if (Rval != OK) {
                SvcLog_NG(SVC_LOG_DISP, "## fail to control osd(%u), rval(%u)", i, Rval);
            }

        } else {

            Rval = AmbaDSP_VoutVideoCtrl((UINT8)VoutID, 0, 0, &RawSeq);
            if (Rval != OK) {
                SvcLog_NG(SVC_LOG_DISP, "## fail to control vout video(%u), rval(%u)", i, Rval);
            }

            Rval = AmbaDSP_VoutOsdCtrl((UINT8)VoutID, (UINT8)pCtrlArr[i].EnableOsd, 0, &RawSeq);
            if (Rval != OK) {
                SvcLog_NG(SVC_LOG_DISP, "## fail to control osd(%u), rval(%u)", i, Rval);
            }

        }

        g_DispVideoCtrl[VoutID] = pCtrlArr[i].EnableVideo;
        g_DispOsdCtrl[VoutID] = pCtrlArr[i].EnableOsd;
    }
}

/**
* status dump of display module
* @return none
*/
void SvcDisplay_Dump(void)
{
    UINT32  i;

    for (i = 0; i < g_NumDisp; i++) {
        if (g_DispVideoCtrl[i] == 1U) {
            SvcLog_DBG(SVC_LOG_DISP, "display(%u) video is enabled", i, 0U);
        } else {
            SvcLog_DBG(SVC_LOG_DISP, "display(%u) video is disabled", i, 0U);
        }

        if (g_DispOsdCtrl[i] == 1U) {
            SvcLog_DBG(SVC_LOG_DISP, "display(%u) osd is enabled", i, 0U);
        } else {
            SvcLog_DBG(SVC_LOG_DISP, "display(%u) osd is disabled", i, 0U);
        }
    }
    AmbaMisra_TouchUnused(g_DispCscMatrix);
}
