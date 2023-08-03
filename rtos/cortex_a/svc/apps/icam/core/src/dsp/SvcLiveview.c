/**
 *  @file SvcLiveview.c
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
 *  @details svc liveview functions
 *
 */


#include "AmbaDSP_Liveview.h"
#include "AmbaDSP_Event.h"
#include "AmbaShell.h"
#include "AmbaUtility.h"
#include "AmbaSensor.h"

#include "SvcErrCode.h"
#include "SvcLog.h"
#include "SvcWrap.h"
#include "SvcClock.h"
#include "SvcLiveview.h"
#include "SvcResCfg.h"
#include "SvcVinSrc.h"

#define SVC_LOG_LIVE        "LIVE"

static UINT32                            g_VinPostBitsSet = 0U;
static UINT32                            g_VinPostBitsGot = 0U;
static UINT32                            g_NumVin = 0U;
static UINT32                            g_NumFov = 0U;
static UINT32                            g_NumStrm = 0U;
static SVC_LIV_VINCAP_s                  g_VinCapWin[AMBA_DSP_MAX_VIN_NUM] GNU_SECTION_NOZEROINIT;
static AMBA_DSP_RESOURCE_s               g_DspRes GNU_SECTION_NOZEROINIT;
static AMBA_DSP_LIVEVIEW_VIEWZONE_CFG_s  g_FovCfg[AMBA_DSP_MAX_VIEWZONE_NUM] GNU_SECTION_NOZEROINIT;
#if defined(CONFIG_ICAM_PIPE_LOWDLY_SUPPORTED)
static AMBA_DSP_LIVEVIEW_SLICE_CFG_s     g_LowDlyCfg[AMBA_DSP_MAX_VIEWZONE_NUM] GNU_SECTION_NOZEROINIT;
#endif
static UINT32                            g_FovCtrl[AMBA_DSP_MAX_VIEWZONE_NUM] GNU_SECTION_NOZEROINIT;
static AMBA_DSP_LIVEVIEW_STREAM_CFG_s    g_StrmCfg[AMBA_DSP_MAX_YUVSTRM_NUM] GNU_SECTION_NOZEROINIT;
static AMBA_DSP_LIVEVIEW_STREAM_CFG_s    g_CurStrmCfg[AMBA_DSP_MAX_YUVSTRM_NUM] GNU_SECTION_NOZEROINIT;
static SVC_LIV_STRM_CHAN_t               g_StrmChanCfg[AMBA_DSP_MAX_YUVSTRM_NUM] GNU_SECTION_NOZEROINIT;
static SVC_LIV_STRM_CHAN_t               g_CurStrmChanCfg[AMBA_DSP_MAX_YUVSTRM_NUM] GNU_SECTION_NOZEROINIT;

/**
* get info block of liveview module
* @param [in] pInfo info block of liveview module
* @return none
*/
void SvcLiveview_InfoGet(SVC_LIV_INFO_s *pInfo)
{
    static AMBA_DSP_LIVEVIEW_YUV_BUF_s  g_PyramidBuf[AMBA_DSP_MAX_VIEWZONE_NUM] GNU_SECTION_NOZEROINIT;
    static SVC_LIV_BUFTBL_s             g_PyramidTbl[AMBA_DSP_MAX_VIEWZONE_NUM] GNU_SECTION_NOZEROINIT GNU_ALIGNED_CACHESAFE;
    static AMBA_DSP_LIVEVIEW_YUV_BUF_s  g_PyramidExScaleBuf[AMBA_DSP_MAX_VIEWZONE_NUM] GNU_SECTION_NOZEROINIT;
    static SVC_LIV_BUFTBL_s             g_PyramidExScaleTbl[AMBA_DSP_MAX_VIEWZONE_NUM] GNU_SECTION_NOZEROINIT GNU_ALIGNED_CACHESAFE;
#if defined(CONFIG_ICAM_MAIN_Y12_OUTPUT_SUPPORTED)
    static AMBA_DSP_LIVEVIEW_YUV_BUF_s  g_MainY12Buf[AMBA_DSP_MAX_VIEWZONE_NUM] GNU_SECTION_NOZEROINIT;
    static SVC_LIV_BUFTBL_s             g_MainY12Tbl[AMBA_DSP_MAX_VIEWZONE_NUM] GNU_SECTION_NOZEROINIT GNU_ALIGNED_CACHESAFE;
#endif
#if defined(CONFIG_ICAM_LV_FEED_EXT_DATA_SUPPORTED)
    static AMBA_DSP_LIVEVIEW_YUV_BUF_s  g_FeedExtDataBuf[AMBA_DSP_MAX_VIEWZONE_NUM] GNU_SECTION_NOZEROINIT;
    static SVC_LIV_BUFTBL_s             g_FeedExtDataTbl[AMBA_DSP_MAX_VIEWZONE_NUM] GNU_SECTION_NOZEROINIT GNU_ALIGNED_CACHESAFE;
#endif

    pInfo->pVinPostBitsSet = &g_VinPostBitsSet;
    pInfo->pNumVin     = &g_NumVin;
    pInfo->pVinCapWin  = g_VinCapWin;

    pInfo->pDspRes     = &g_DspRes;
    pInfo->pNumFov     = &g_NumFov;
    pInfo->pFovCfg     = g_FovCfg;
#if defined(CONFIG_ICAM_PIPE_LOWDLY_SUPPORTED)
    pInfo->pLowDlyCfg  = g_LowDlyCfg;
#endif
    pInfo->pPyramidBuf = g_PyramidBuf;
    pInfo->pPyramidTbl = g_PyramidTbl;
    pInfo->pPyramidExScaleBuf = g_PyramidExScaleBuf;
    pInfo->pPyramidExScaleTbl = g_PyramidExScaleTbl;
#if defined(CONFIG_ICAM_MAIN_Y12_OUTPUT_SUPPORTED)
    pInfo->pMainY12Buf = g_MainY12Buf;
    pInfo->pMainY12Tbl = g_MainY12Tbl;
#endif
#if defined(CONFIG_ICAM_LV_FEED_EXT_DATA_SUPPORTED)
    pInfo->pFeedExtDataBuf = g_FeedExtDataBuf;
    pInfo->pFeedExtDataTbl = g_FeedExtDataTbl;
#endif

    pInfo->pNumStrm    = &g_NumStrm;
    pInfo->pStrmCfg    = g_StrmCfg;
    pInfo->pStrmChan   = g_StrmChanCfg;
}

/**
* set VIN post config bit mask
* @param [in] VinPostBits VIN post config bit mask
*/
void SvcLiveview_SetStillVinPostBits(UINT32 VinPostBits)
{
    g_VinPostBitsSet = VinPostBits;
    g_VinPostBitsGot = 0;
}

static UINT32 LivPostConfig(const void *pEventData)
{
    UINT32 i, Rval;
    AMBA_DSP_VIN_POST_CONFIG_STATE_s State = {0};
    UINT32 VinSrc = 0xFF;

    Rval = AmbaWrap_memcpy(&State, pEventData, sizeof(AMBA_DSP_VIN_POST_CONFIG_STATE_s));
    if (SVC_OK != Rval) {
        SvcLog_NG(SVC_LOG_LIVE, "LivPostConfig() err, AmbaWrap_memcpy failed, AMBA_DSP_VIN_POST_CONFIG_STATE_s", 0U, 0U);
    }

    for (i = 0U; i < g_NumVin; i++) {
        UINT32 VinBit = (UINT32)1 << g_VinCapWin[i].VinID;

        Rval = SvcResCfg_GetVinSrc(g_VinCapWin[i].VinID, &VinSrc);
        if (OK == Rval) {
            if (VinSrc != SVC_VIN_SRC_YUV) {
                if ((VinBit & (UINT32)State.VinIdx) == VinBit) {
                    g_VinPostBitsGot |= VinBit;
                }
            }
        }
    }
    SvcLog_DBG(SVC_LOG_LIVE, "g_VinPostBitsGot 0x%x g_VinPostBitsSet 0x%x", g_VinPostBitsGot, g_VinPostBitsSet);
    if (g_VinPostBitsGot == g_VinPostBitsSet) {
        AMBA_SENSOR_CHANNEL_s Chan;
        Rval = AmbaWrap_memset(&Chan, 0, sizeof(Chan));
        if (Rval != OK) {
            SvcLog_NG(SVC_LOG_LIVE, "[line %u] AmbaWrap_memset return 0x%x", __LINE__, Rval);
        } else {
            for (i = 0; i < g_NumVin; i ++) {
                Chan.VinID = g_VinCapWin[i].VinID;
                if ((g_VinPostBitsGot & ((UINT32)1U << Chan.VinID)) > 0U) {
                    Rval = SvcResCfg_GetSensorIDInVinID(g_VinCapWin[i].VinID, &Chan.SensorID);
                    if (Rval == OK) {
                        Rval = AmbaSensor_ConfigPost(&Chan);
                        if (Rval == OK) {
                            SvcLog_OK(SVC_LOG_LIVE, "## vin %u sensor 0x%x post-streaming", Chan.VinID, Chan.SensorID);
                        } else {
                            if (Rval == SENSOR_ERR_INVALID_API) {
                                SvcLog_OK(SVC_LOG_LIVE, "## vin %u sensor not support post-streaming", Chan.VinID, 0U);
                            } else {
                                SvcLog_NG(SVC_LOG_LIVE, "## fail to config post-streaming, return %u", Rval, 0U);
                                SvcLog_NG(SVC_LOG_LIVE, "## vin %u sensor 0x%x", Chan.VinID, Chan.SensorID);
                            }
                        }
                    } else {
                        SvcLog_NG(SVC_LOG_LIVE, "## fail to get sensor id for vin %u, return %u", Chan.VinID, Rval);
                    }
                }
            }
        }
    }

    return Rval;
}


/**
* configuration of liveview module
* @return none
*/
void SvcLiveview_Config(void)
{
    UINT32  i, Rval;

    SvcLog_DBG(SVC_LOG_LIVE, "@@ SvcLiveview_Config begin", 0U, 0U);

    /* reset fov control */
    if (SVC_OK != AmbaWrap_memset(&g_FovCtrl, 0, sizeof(g_FovCtrl))) {
        SvcLog_NG(SVC_LOG_LIVE, "SvcLiveview_Config() err, AmbaWrap_memset failed, g_FovCtrl", 0U, 0U);
    }

    SvcLog_DBG(SVC_LOG_LIVE, "Resource Config start", 0U, 0U);

    /* configure dsp resource */
    Rval = AmbaDSP_ResourceLimitConfig(&g_DspRes);

    SvcLog_DBG(SVC_LOG_LIVE, "Resource Config done", 0U, 0U);

    if (OK == Rval) {
        /* configure fov and yuv stream */
        Rval = AmbaDSP_LiveviewConfig((UINT16)g_NumFov, g_FovCfg, (UINT16)g_NumStrm, g_StrmCfg);
        if (OK == Rval) {
            if (SVC_OK != AmbaWrap_memcpy(g_CurStrmCfg, g_StrmCfg, sizeof(AMBA_DSP_LIVEVIEW_STREAM_CFG_s) * AMBA_DSP_MAX_YUVSTRM_NUM)) {
                SvcLog_NG(SVC_LOG_LIVE, "SvcLiveview_Config() err, AmbaWrap_memcpy failed, AMBA_DSP_LIVEVIEW_STREAM_CFG_s", 0U, 0U);
            }
            if (SVC_OK != AmbaWrap_memcpy(g_CurStrmChanCfg, g_StrmChanCfg, sizeof(SVC_LIV_STRM_CHAN_t) * AMBA_DSP_MAX_YUVSTRM_NUM)) {
                SvcLog_NG(SVC_LOG_LIVE, "SvcLiveview_Config() err, AmbaWrap_memcpy failed, SVC_LIV_STRM_CHAN_t", 0U, 0U);
            }
        } else {
            SvcLog_NG(SVC_LOG_LIVE, "## fail to config liveview, return %u", Rval, 0U);
        }
    } else {
        SvcLog_NG(SVC_LOG_LIVE, "## fail to config dsp resource, return %u", Rval, 0U);
    }

#if defined(CONFIG_ICAM_PIPE_LOWDLY_SUPPORTED)
    /* configure low delay */
    if (OK == Rval) {
        for (i = 0U; i < g_NumFov; i++) {
            if ((1U < g_LowDlyCfg[i].SliceNumRow) || (g_LowDlyCfg[i].WarpLumaWaitLine > 0U)) {
                Rval = AmbaDSP_LiveviewSliceCfg(g_FovCfg[i].ViewZoneId, &(g_LowDlyCfg[i]));
                if (Rval != OK) {
                    SvcLog_NG(SVC_LOG_LIVE, "## fail to config low delay, %u", Rval, 0U);
                }
            }
        }
    }
#endif

    g_VinPostBitsGot = 0;
    /* configure capture window of vin */
    for (i = 0U; i < g_NumVin; i++) {
        Rval = AmbaDSP_LiveviewConfigVinCapture((UINT16)g_VinCapWin[i].VinID,
                                                (UINT16)g_VinCapWin[i].SubChNum,
                                                g_VinCapWin[i].SubChCfg);
        if (OK != Rval) {
            SvcLog_NG(SVC_LOG_LIVE, "## fail to config vin capture, return %u", Rval, 0U);
            break;
        } else {
            Rval = AmbaDSP_LiveviewConfigVinPost(AMBA_DSP_VIN_CONFIG_POST, (UINT16)g_VinCapWin[i].VinID);
            if (OK != Rval) {
                SvcLog_NG(SVC_LOG_LIVE, "## fail to config vin post, return %u", Rval, 0U);
            }
        }
#if defined(AMBA_DSP_VIN_CAP_OPT_EMBD)
        if (g_VinCapWin[i].EmbChCfg.Option == AMBA_DSP_VIN_CAP_OPT_EMBD) {
            Rval = AmbaDSP_LiveviewConfigVinCapture((UINT16)g_VinCapWin[i].VinID,
                                                    1U,
                                                    &(g_VinCapWin[i].EmbChCfg));
            if (OK != Rval) {
                SvcLog_NG(SVC_LOG_LIVE, "## fail to config vin embedded channel, return %u", Rval, 0U);
            }
        }
#endif
    }
    if (OK == Rval) {
        static UINT8 VinPostRegistered = 0;
        if (VinPostRegistered == 0U) {
            Rval = AmbaDSP_EventHandlerRegister(AMBA_DSP_EVENT_VIN_POST_CONFIG, LivPostConfig);
            if (OK != Rval) {
                SvcLog_NG(SVC_LOG_LIVE, "fail to register VIN_POST_CONFIG event, return %u", Rval, 0U);
            }
            VinPostRegistered = 1;
        }
    }

    SvcLog_DBG(SVC_LOG_LIVE, "@@ SvcLiveview_Config end", 0U, 0U);
}

/**
* configuration update of liveview module
* @return none
*/
void SvcLiveview_Update(void)
{
    UINT32 Rval;
    UINT64 RawSeqNum = 0U;

    Rval = AmbaDSP_LiveviewUpdateConfig((UINT16) g_NumStrm, g_StrmCfg, &RawSeqNum);

    if (OK != Rval) {
        SvcLog_NG(SVC_LOG_LIVE, "## fail to config liveview layout, return %u", Rval, 0U);
    } else {
        if (SVC_OK != AmbaWrap_memcpy(g_CurStrmCfg, g_StrmCfg, sizeof(AMBA_DSP_LIVEVIEW_STREAM_CFG_s) * AMBA_DSP_MAX_YUVSTRM_NUM)) {
            SvcLog_NG(SVC_LOG_LIVE, "SvcLiveview_Update() err, AmbaWrap_memcpy failed, AMBA_DSP_LIVEVIEW_STREAM_CFG_s", 0U, 0U);
        }
        if (SVC_OK != AmbaWrap_memcpy(g_CurStrmChanCfg, g_StrmChanCfg, sizeof(SVC_LIV_STRM_CHAN_t) * AMBA_DSP_MAX_YUVSTRM_NUM)) {
            SvcLog_NG(SVC_LOG_LIVE, "SvcLiveview_Update() err, AmbaWrap_memcpy failed, SVC_LIV_STRM_CHAN_t", 0U, 0U);
        }
        SvcLog_OK(SVC_LOG_LIVE, "@@ Successfully update liveview (%d)", (UINT32) RawSeqNum, 0U);
    }
}

/**
* yuv stream update of liveview module
* @param [in] StrmIdx index of yuv stream
* @return none
*/
void SvcLiveview_UpdateStream(UINT32 StrmIdx)
{
    const SVC_RES_CFG_s *pCfg = SvcResCfg_Get();

    AMBA_DSP_LIVEVIEW_SYNC_CTRL_s StrmSyncCtrl = {0};
    UINT32 StrmSyncJob;
    UINT64 RawSeqNum = 0U;
    UINT32 CmpResult = 0U;

    /* The Yuv Stream Update Block Start */
    StrmSyncCtrl.TargetViewZoneMask = ((UINT32) 1U << pCfg->FovNum) - 1U;
    StrmSyncCtrl.Opt = AMBA_DSP_YUVSTRM_SYNC_START;

    if (SVC_OK != AmbaWrap_memcmp(&g_CurStrmCfg[StrmIdx], &g_StrmCfg[StrmIdx], sizeof(AMBA_DSP_LIVEVIEW_STREAM_CFG_s), &CmpResult)) {
        SvcLog_NG(SVC_LOG_LIVE, "SvcLiveview_UpdateStream() err, AmbaWrap_memcmp failed, AMBA_DSP_LIVEVIEW_STREAM_CFG_s", 0U, 0U);
    }
    if (CmpResult == 0U) {
        if (SVC_OK != AmbaWrap_memcmp(&g_CurStrmChanCfg[StrmIdx], g_StrmChanCfg[StrmIdx], sizeof(SVC_LIV_STRM_CHAN_t), &CmpResult)) {
            SvcLog_NG(SVC_LOG_LIVE, "SvcLiveview_UpdateStream() err, AmbaWrap_memcmp failed, SVC_LIV_STRM_CHAN_t", 0U, 0U);
        }
    }

    if (CmpResult == 0U) {
        SvcLog_DBG(SVC_LOG_LIVE, "StrmCfg no change. No need to update the StrmIdx %d", StrmIdx, 0U);
    } else {
        if (SVC_OK != AmbaDSP_LiveviewYuvStreamSync((UINT16) StrmIdx, &StrmSyncCtrl, &StrmSyncJob, &RawSeqNum)) {
            SvcLog_NG(SVC_LOG_LIVE, "SvcLiveview_UpdateStream() err, AmbaDSP_LiveviewYuvStreamSync failed", 0U, 0U);
        }
        if (SVC_OK != AmbaDSP_LiveviewUpdateConfig(1U, &g_StrmCfg[StrmIdx], &RawSeqNum)) {
            SvcLog_NG(SVC_LOG_LIVE, "SvcLiveview_UpdateStream() err, AmbaDSP_LiveviewUpdateConfig failed", 0U, 0U);
        }

        StrmSyncCtrl.Opt = AMBA_DSP_YUVSTRM_SYNC_EXECUTE;
        if (SVC_OK != AmbaDSP_LiveviewYuvStreamSync((UINT16) StrmIdx, &StrmSyncCtrl, &StrmSyncJob, &RawSeqNum)) {
            SvcLog_NG(SVC_LOG_LIVE, "SvcLiveview_UpdateStream() err, AmbaDSP_LiveviewYuvStreamSync failed, AMBA_DSP_YUVSTRM_SYNC_EXECUTE", 0U, 0U);
        }
        if (SVC_OK != AmbaWrap_memcpy(&g_CurStrmCfg[StrmIdx], &g_StrmCfg[StrmIdx], sizeof(AMBA_DSP_LIVEVIEW_STREAM_CFG_s))) {
            SvcLog_NG(SVC_LOG_LIVE, "SvcLiveview_UpdateStream() err, AmbaWrap_memcpy failed, AMBA_DSP_LIVEVIEW_STREAM_CFG_s", 0U, 0U);
        }
        if (SVC_OK != AmbaWrap_memcpy(&g_CurStrmChanCfg[StrmIdx], &g_StrmChanCfg[StrmIdx], sizeof(SVC_LIV_STRM_CHAN_t))) {
            SvcLog_NG(SVC_LOG_LIVE, "SvcLiveview_UpdateStream() err, AmbaWrap_memcpy failed, SVC_LIV_STRM_CHAN_t", 0U, 0U);
        }
        SvcLog_OK(SVC_LOG_LIVE, "@@ Successfully update StrmIdx (%d)", StrmIdx, 0U);
    }
}

/**
* control of liveview module
* @param [in] NumFov number of fov
* @param [in] pFovCtrl array of fov control block
* @return none
*/
void SvcLiveview_Ctrl(UINT32 NumFov, const SVC_LIV_FOV_CTRL_s *pFovCtrl)
{
    UINT32  i, Rval, IsFovOn = 0U;

    SvcLog_DBG(SVC_LOG_LIVE, "@@ SvcLiveview_Ctrl begin", 0U, 0U);

    /* check fov status */
    for (i = 0; i < NumFov; i++) {
        if (0U < pFovCtrl->EnableArr[i]) {
            /* enable feature clock */
            SvcClock_FeatureCtrl(1U, SVC_CLK_FEA_BIT_MCTF);
            break;
        }
    }


    SvcLog_DBG(SVC_LOG_LIVE, "@@ AmbaDSP_LiveviewCtrl begin", 0U, 0U);
    Rval = AmbaDSP_LiveviewCtrl((UINT16)NumFov, pFovCtrl->FovIDArr, pFovCtrl->EnableArr);
    SvcLog_DBG(SVC_LOG_LIVE, "@@ AmbaDSP_LiveviewCtrl end", 0U, 0U);
    if (OK == Rval) {
        SvcLog_OK(SVC_LOG_LIVE, "## liveview control success", 0, 0U);
        for (i = 0U; i < NumFov; i++) {
            g_FovCtrl[pFovCtrl->FovIDArr[i]] = pFovCtrl->EnableArr[i];
        }

        for (i = 0U; i < g_NumFov; i++) {
            if (0U < g_FovCtrl[i]) {
                IsFovOn = 1U;
                break;
            }
        }

        if (IsFovOn == 0U) {
            /* disable feature clock */
            SvcClock_FeatureCtrl(0U, SVC_CLK_FEA_BIT_MCTF);
        }
    } else {
        SvcLog_NG(SVC_LOG_LIVE, "## fail to control liveview %u", Rval, 0U);
    }

    SvcLog_DBG(SVC_LOG_LIVE, "@@ SvcLiveview_Ctrl end", 0U, 0U);
}

/**
* status dump of liveview module
* @param [in] PrintFunc print function of shell
* @return none
*/
void SvcLiveview_Dump(AMBA_SHELL_PRINT_f PrintFunc)
{
    char    StrBuf[256] = {'\0'};
    UINT32  i, c, BufLen = 256U, Rval;

    PrintFunc("#### dsp resource information ####\n");
    SVC_WRAP_SNPRINT            "[liveview] video_pipe(%d)\n"
        SVC_SNPRN_ARG_S         StrBuf
        SVC_SNPRN_ARG_UINT32    g_DspRes.LiveviewResource.VideoPipe SVC_SNPRN_ARG_POST
        SVC_SNPRN_ARG_BSIZE     BufLen
        SVC_SNPRN_ARG_RLEN      &Rval
        SVC_SNPRN_ARG_E
    PrintFunc(StrBuf);

    PrintFunc("#### liveview information ####\n");
    for (i = 0U; i < g_NumVin; i++) {
        SVC_WRAP_SNPRINT            "[vin%u] cap_w/h(%u/%u)\n"
            SVC_SNPRN_ARG_S         StrBuf
            SVC_SNPRN_ARG_UINT32    g_VinCapWin[i].VinID                            SVC_SNPRN_ARG_POST
            SVC_SNPRN_ARG_UINT32    g_VinCapWin[i].SubChCfg[0].CaptureWindow.Width  SVC_SNPRN_ARG_POST
            SVC_SNPRN_ARG_UINT32    g_VinCapWin[i].SubChCfg[0].CaptureWindow.Height SVC_SNPRN_ARG_POST
            SVC_SNPRN_ARG_BSIZE     BufLen
            SVC_SNPRN_ARG_RLEN      &Rval
            SVC_SNPRN_ARG_E
        PrintFunc(StrBuf);
    }

    PrintFunc("\n[fov]\n");
    for (i = 0U; i < g_NumFov; i++) {
        SVC_WRAP_SNPRINT            " [%d_vin%d] roi(%d/%d/%d/%d), main(%d/%d)\n"
            SVC_SNPRN_ARG_S         StrBuf
            SVC_SNPRN_ARG_UINT32    g_FovCfg[i].ViewZoneId     SVC_SNPRN_ARG_POST
            SVC_SNPRN_ARG_UINT32    g_FovCfg[i].VinId          SVC_SNPRN_ARG_POST
            SVC_SNPRN_ARG_UINT32    g_FovCfg[i].VinRoi.OffsetX SVC_SNPRN_ARG_POST
            SVC_SNPRN_ARG_UINT32    g_FovCfg[i].VinRoi.OffsetY SVC_SNPRN_ARG_POST
            SVC_SNPRN_ARG_UINT32    g_FovCfg[i].VinRoi.Width   SVC_SNPRN_ARG_POST
            SVC_SNPRN_ARG_UINT32    g_FovCfg[i].VinRoi.Height  SVC_SNPRN_ARG_POST
            SVC_SNPRN_ARG_UINT32    g_FovCfg[i].MainWidth      SVC_SNPRN_ARG_POST
            SVC_SNPRN_ARG_UINT32    g_FovCfg[i].MainHeight     SVC_SNPRN_ARG_POST
            SVC_SNPRN_ARG_BSIZE     BufLen
            SVC_SNPRN_ARG_RLEN      &Rval
            SVC_SNPRN_ARG_E
        PrintFunc(StrBuf);

#if defined(DSP_DRAM_PIPE_NUM)
        SVC_WRAP_SNPRINT            "    video_pipe(%d)\n"
            SVC_SNPRN_ARG_S         StrBuf
            SVC_SNPRN_ARG_UINT32    g_FovCfg[i].DramUtilityPipe SVC_SNPRN_ARG_POST
            SVC_SNPRN_ARG_BSIZE     BufLen
            SVC_SNPRN_ARG_RLEN      &Rval
            SVC_SNPRN_ARG_E
        PrintFunc(StrBuf);
#endif

#if defined(CONFIG_ICAM_PIPE_LOWDLY_SUPPORTED)
        SVC_WRAP_SNPRINT            "    slice(%d/%d/%d)\n"
            SVC_SNPRN_ARG_S         StrBuf
            SVC_SNPRN_ARG_UINT32    g_LowDlyCfg[i].SliceNumRow SVC_SNPRN_ARG_POST
            SVC_SNPRN_ARG_UINT32    g_LowDlyCfg[i].WarpOverLap SVC_SNPRN_ARG_POST
            SVC_SNPRN_ARG_UINT32    g_LowDlyCfg[i].VinDragLine SVC_SNPRN_ARG_POST
            SVC_SNPRN_ARG_BSIZE     BufLen
            SVC_SNPRN_ARG_RLEN      &Rval
            SVC_SNPRN_ARG_E
        PrintFunc(StrBuf);
#endif
    }

    PrintFunc("\n[yuv_stream]\n");
    for (i = 0U; i < g_NumStrm; i++) {
        SVC_WRAP_SNPRINT            "\n [strm%d] dest_vout/enc(0x%X/0x%X), win/max(%d/%d, %d/%d), max_ch/bits(%d/0x%X)\n"
            SVC_SNPRN_ARG_S         StrBuf
            SVC_SNPRN_ARG_UINT32    g_StrmCfg[i].StreamId       SVC_SNPRN_ARG_POST
            SVC_SNPRN_ARG_UINT32    g_StrmCfg[i].DestVout       SVC_SNPRN_ARG_POST
            SVC_SNPRN_ARG_UINT32    g_StrmCfg[i].DestEnc        SVC_SNPRN_ARG_POST
            SVC_SNPRN_ARG_UINT32    g_StrmCfg[i].Width          SVC_SNPRN_ARG_POST
            SVC_SNPRN_ARG_UINT32    g_StrmCfg[i].Height         SVC_SNPRN_ARG_POST
            SVC_SNPRN_ARG_UINT32    g_StrmCfg[i].MaxWidth       SVC_SNPRN_ARG_POST
            SVC_SNPRN_ARG_UINT32    g_StrmCfg[i].MaxHeight      SVC_SNPRN_ARG_POST
            SVC_SNPRN_ARG_UINT32    g_StrmCfg[i].MaxChanNum     SVC_SNPRN_ARG_POST
            SVC_SNPRN_ARG_UINT32    g_StrmCfg[i].MaxChanBitMask SVC_SNPRN_ARG_POST
            SVC_SNPRN_ARG_BSIZE     BufLen
            SVC_SNPRN_ARG_RLEN      &Rval
            SVC_SNPRN_ARG_E
        PrintFunc(StrBuf);

        for (c = 0; c < g_StrmCfg[i].NumChan; c++) {
            SVC_WRAP_SNPRINT            "  [fov%d] roi(%u/%u/%u/%u), win(%u/%u/%u/%u)\n"
                SVC_SNPRN_ARG_S         StrBuf
                SVC_SNPRN_ARG_UINT32    g_StrmChanCfg[i][c].ViewZoneId     SVC_SNPRN_ARG_POST
                SVC_SNPRN_ARG_UINT32    g_StrmChanCfg[i][c].ROI.OffsetX    SVC_SNPRN_ARG_POST
                SVC_SNPRN_ARG_UINT32    g_StrmChanCfg[i][c].ROI.OffsetY    SVC_SNPRN_ARG_POST
                SVC_SNPRN_ARG_UINT32    g_StrmChanCfg[i][c].ROI.Width      SVC_SNPRN_ARG_POST
                SVC_SNPRN_ARG_UINT32    g_StrmChanCfg[i][c].ROI.Height     SVC_SNPRN_ARG_POST
                SVC_SNPRN_ARG_UINT32    g_StrmChanCfg[i][c].Window.OffsetX SVC_SNPRN_ARG_POST
                SVC_SNPRN_ARG_UINT32    g_StrmChanCfg[i][c].Window.OffsetY SVC_SNPRN_ARG_POST
                SVC_SNPRN_ARG_UINT32    g_StrmChanCfg[i][c].Window.Width   SVC_SNPRN_ARG_POST
                SVC_SNPRN_ARG_UINT32    g_StrmChanCfg[i][c].Window.Height  SVC_SNPRN_ARG_POST
                SVC_SNPRN_ARG_BSIZE     BufLen
                SVC_SNPRN_ARG_RLEN      &Rval
                SVC_SNPRN_ARG_E
            PrintFunc(StrBuf);
        }
    }
    PrintFunc("\n");
}
