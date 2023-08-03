/**
 *  @file ambadsp_devioctl.c
 *
 *  Copyright (c) 2020 Ambarella International LP
 *
 *  This file and its contents ("Software") are protected by intellectual
 *  property rights including, without limitation, U.S. and/or foreign
 *  copyrights. This Software is also the confidential and proprietary
 *  information of Ambarella International LP and its licensors. You may not use, reproduce,
 *  disclose, distribute, modify, or otherwise prepare derivative works of this
 *  Software or any portion thereof except pursuant to a signed license agreement
 *  or nondisclosure agreement with Ambarella International LP or its authorized affiliates.
 *  In the absence of such an agreement, you agree to promptly notify and return
 *  this Software to Ambarella International LP.
 *
 *  THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
 *  INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF NON-INFRINGEMENT,
 *  MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 *  IN NO EVENT SHALL AMBARELLA INTERNATIONAL LP OR ITS AFFILIATES BE LIABLE FOR ANY DIRECT,
 *  INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 *  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 *  LOSS OF USE, DATA, OR PROFITS; COMPUTER FAILURE OR MALFUNCTION; OR BUSINESS
 *  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 *  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 *  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *  POSSIBILITY OF SUCH DAMAGE.
 *
 *  @details Ambadsp devcie cmd handler
 *
 */

#include "dsp_priv_api.h"
#include "ambadsp_ioctl.h"
#include "dsp_osal.h"
#include "AmbaDSP_Priv.h"
#include "AmbaDSP_Buffers.h"
#include "AmbaDSP_Log.h"

uint32_t dsp_ioctl_impl(const void *f, uint32_t cmd, void *arg)
{
    uint32_t Rval = DSP_ERR_NONE;
    uint16_t i, j, k;
    AMBA_DSP_BIN_ADDR_s *pDspBinAddr = NULL;
    dsp_hier_buf_calc_t *pDspHierBufCalc = NULL;
    dsp_raw_pitch_calc_t *pDspRawPitchCalc = NULL;
    dsp_enc_mv_buf_calc_t *pDspEncMvBufCalc = NULL;
    dsp_enc_mv_buf_parse_t *pDspEncMvBufParse = NULL;
    dsp_vp_msg_buf_t *pDspVpMsgBuf = NULL;
    AMBA_DSP_PROTECT_s *pDspProtect = NULL;
    dsp_buf_info_t *pDspBufInfo = NULL;
    dsp_debug_dump_t *pDspDebugDump = NULL;
    dsp_cache_buf_op_t *pDspCacheOp = NULL;
#if defined (CONFIG_SOC_CV2FS) || defined (CONFIG_SOC_CV22FS)
    dsp_main_safety_check_t *pMainSafetyChk = NULL;
    dsp_main_safety_config_t *pMainSafetyCfg = NULL;
#endif
#if defined (CONFIG_SOC_CV5) || defined (CONFIG_SOC_CV52)
    dsp_slice_cfg_t *pSliceCfg = NULL;
#endif
    dsp_status_t *pDspStatus = NULL;
    dsp_evnt_hdlr_cfg_t *pDspEvntHdlrCfg = NULL;
    uint16_t *pU16 = NULL;
    dsp_evnt_hdlr_op_t *pDspEvntHdlrOp = NULL;
    dsp_evnt_data_t *pDspEvntData = NULL;
    AMBA_DSP_VERSION_INFO_s *pDspVer = NULL;
    AMBA_DSP_SYS_CONFIG_s *pDspSysCfg = NULL;
    dsp_mem_t *pDspMem = NULL;
    uint32_t *pU32 = NULL;
    dsp_wait_sig_t *pDspWaitSig = NULL;
    dsp_resc_limit_cfg_t *pDspResc = NULL;
    dsp_partial_load_cfg_t *pDspParLoadCfg = NULL;
    dsp_partial_load_unlock_t *pDspParLoadUnlock = NULL;
    dsp_sys_drv_cfg_t *pDspSysDrvCfg = NULL;
    dsp_protect_area_t *pDspProtectArea = NULL;
    dsp_ext_buf_tbl_t *pDspExtBufTbl = NULL;
    uint8_t *pU8 = NULL;
    dsp_vout_mixer_cfg_t *pDspVoutMixerCfg = NULL;
    dsp_vout_mixer_bgc_cfg_t *pDspVoutMixerBgcCfg = NULL;
    dsp_vout_mixer_hlc_cfg_t *pDspVoutMixerHlcCfg = NULL;
    dsp_vout_mixer_csc_cfg_t *pDspVoutMixerCscCfg = NULL;
    dsp_vout_mixer_csc_matrix_cfg_t *pDspVoutMixerCscMatrixCfg = NULL;
    dsp_vout_osd_buf_cfg_t *pDspVoutOsdBufCfg = NULL;
    dsp_vout_osd_ctrl_t *pDspVoutOsdCtrl = NULL;
    dsp_vout_disp_cfg_t *pDspVoutDispCfg = NULL;
    dsp_vout_disp_gamma_cfg_t *pDspVoutDispGammaCfg = NULL;
    dsp_vout_disp_gamma_ctrl_t *pDspVoutDispGammaCtrl = NULL;
    dsp_vout_video_cfg_t *pDspVoutVideoCfg = NULL;
    dsp_vout_video_ctrl_t *pDspVoutVideoCtrl = NULL;
    dsp_vout_mixer_bind_cfg_t *pDspVoutMixerBindCfg = NULL;
    dsp_vout_dve_cfg_t *pDspVoutDveCfg = NULL;
    dsp_vout_drv_cfg_t *pDspVoutDrvCfg = NULL;

    dsp_liveview_cfg_t *pDspLiveviewCfg = NULL;
    dsp_liveview_ctrl_t *pDspLiveviewCtrl = NULL;
    dsp_liveview_update_cfg_t *pDspLiveviewUpdateCfg = NULL;
    dsp_liveview_update_isocfg_t *pDspLiveviewUpdateIsocfg = NULL;
    dsp_liveview_update_vincfg_t *pDspLiveviewUpdateVincfg = NULL;
    dsp_liveview_update_pymdcfg_t *pDspLiveviewUpdatePymdcfg = NULL;
    dsp_liveview_vin_cap_cfg_t *pDspLiveviewVinCapCfg = NULL;
    dsp_liveview_vin_post_cfg_t *pDspLiveviewVinPostCfg = NULL;
    dsp_liveview_feed_raw_data_t *pDspLiveviewFeedRawData = NULL;
    dsp_liveview_feed_yuv_data_t *pDspLiveviewFeedYuvData = NULL;
    dsp_liveview_yuvstrm_sync_t *pDspLiveviewYuvStrmSync = NULL;
    dsp_liveview_update_geocfg_t *pDspLiveviewUpdateGeocfg = NULL;
    dsp_liveview_update_vz_src_t *pDspLiveviewUpdateVzSrc = NULL;
    dsp_liveview_update_sideband_t *pDspLiveviewUpdateSideband = NULL;
    dsp_liveview_slice_cfg_t *pDspLiveviewSliceCfg = NULL;
    dsp_liveview_vz_postpone_cfg_t *pDspLiveviewVzPostponeCfg = NULL;
    dsp_liveview_update_vin_state_t *pDspLiveviewUpdateVinState = NULL;
    ULONG *pUL = NULL;
    dsp_liveview_vin_drv_cfg_t *pDspLiveviewVinDrvCfg = NULL;
    dsp_liveview_ik_drv_cfg_t *pDspLiveviewIkDrvCfg = NULL;
    dsp_liveview_idsp_cfg_t *pDspLiveviewIdspCfg = NULL;
    dsp_liveview_slow_shutter_ctrl_t *pDspLiveviewSlowShutterCtrl = NULL;
#if defined (CONFIG_SOC_CV5) || defined (CONFIG_SOC_CV52)
    dsp_liveview_drop_repeat_ctrl_t *pDspLiveviewDropRepeatCtrl = NULL;
#endif
    dsp_video_enc_cfg_t *pDspVideoEncCfg = NULL;
    dsp_video_enc_start_t *pDspVideoEncStart = NULL;
    dsp_video_enc_stop_t *pDspVideoEncStop = NULL;
    dsp_video_enc_frmrate_ctrl_t *pDspVideoEncFrmRateCtrl = NULL;
    dsp_video_enc_blend_ctrl_t *pDspVideoEncBlendCtrl = NULL;
    dsp_video_enc_quality_ctrl_t *pDspVideoEncQualityCtrl = NULL;
    dsp_video_enc_feed_yuv_data_t *pDspVideoEncFeedYuv = NULL;
    AMBA_DSP_VIDEO_ENC_GROUP_s *pDspVideoEncGrpCfg = NULL;
    dsp_video_enc_mv_cfg_t *pDspVideoEncMvCfg = NULL;
    dsp_video_enc_desc_fmt_cfg_t *pDspVideoEncDescFmtCfg = NULL;
    dsp_stl_data_cap_cfg_t *pDspStlDataCapCfg = NULL;
    dsp_stl_update_cap_buf_t *pDspStlUpdateCapBuf = NULL;
    dsp_stl_data_cap_ctrl_t *pDspStlDataCapCtrl = NULL;
    dsp_stl_y2y_t *pDspStlY2Y = NULL;
    dsp_stl_enc_ctrl_t *pDspStlEncCtrl = NULL;
    dsp_stl_yuv_extbuf_clac_t *pDspStlYuvExtBufCalc = NULL;
    dsp_stl_r2y_t *pDspStlR2Y = NULL;
    dsp_stl_r2r_t *pDspStlR2R = NULL;
    dsp_video_dec_cfg_t *pDspVideoDecCfg = NULL;
    dsp_video_dec_start_t *pDspVideoDecStart = NULL;
    dsp_video_dec_stop_t *pDspVideoDecStop = NULL;
    dsp_video_dec_trickplay_t *pDspVideoDecTrickplay = NULL;
    dsp_video_dec_bits_update_t *pDspVideoDecBitsUpdate = NULL;
    dsp_video_dec_post_ctrl_t *pDspVideoDecPostCtrl = NULL;
    dsp_still_dec_start_t *pDspStillDecStart = NULL;
    dsp_still_dec_y2y_t *pDspStillDecY2Y = NULL;
    dsp_still_dec_blend_t *pDspStillDecBlend = NULL;
    dsp_still_dec_disp_yuv_t *pDspStillDecDispYuv = NULL;
    ULONG ULAddr = 0U;
    static ULONG RescExtBufTbl[AMBA_DSP_MAX_VIN_NUM][AMBA_DSP_MAX_VIRT_CHAN_NUM][MAX_EXT_DISTINCT_DEPTH] = {0};
    static ULONG RescExtAuxBufTbl[AMBA_DSP_MAX_VIN_NUM][AMBA_DSP_MAX_VIRT_CHAN_NUM][MAX_EXT_DISTINCT_DEPTH] = {0};

(void)f;

    switch (cmd) {
    /******************   Ultility  ******************/
    case AMBADSP_GET_VERSION:
        dsp_osal_typecast(&pDspVer, &arg);
        Rval = dsp_main_get_dsp_ver_info(pDspVer);
        break;
    case AMBADSP_GET_DSP_BIN_ADDR:
    {
        AMBA_DSP_BIN_ADDR_s DspBinAddr = {0};
        ULONG PhysAddr;

        dsp_osal_typecast(&pDspBinAddr, &arg);

        (void)dsp_main_get_dsp_bin_addr(&DspBinAddr);

        Rval = dsp_osal_virt2phys(DspBinAddr.CoreStartAddr, &PhysAddr);
        dsp_osal_print_err_line(Rval, __func__, __LINE__);
        pDspBinAddr->CoreStartAddr = PhysAddr;
        Rval = dsp_osal_virt2phys(DspBinAddr.MeStartAddr, &PhysAddr);
        dsp_osal_print_err_line(Rval, __func__, __LINE__);
        pDspBinAddr->MeStartAddr = PhysAddr;
        Rval = dsp_osal_virt2phys(DspBinAddr.MdxfStartAddr, &PhysAddr);
        dsp_osal_print_err_line(Rval, __func__, __LINE__);
        pDspBinAddr->MdxfStartAddr = PhysAddr;
        Rval = dsp_osal_virt2phys(DspBinAddr.DefaultDataStartAddr, &PhysAddr);
        dsp_osal_print_err_line(Rval, __func__, __LINE__);
        pDspBinAddr->DefaultDataStartAddr = PhysAddr;
#if defined (CONFIG_SOC_CV5) || defined (CONFIG_SOC_CV52)
        dsp_osal_virt2phys(DspBinAddr.Core1StartAddr, &PhysAddr);
        pDspBinAddr->Core1StartAddr = PhysAddr;
        dsp_osal_virt2phys(DspBinAddr.Core2StartAddr, &PhysAddr);
        pDspBinAddr->Core2StartAddr = PhysAddr;
        dsp_osal_virt2phys(DspBinAddr.Core3StartAddr, &PhysAddr);
        pDspBinAddr->Core3StartAddr = PhysAddr;

        dsp_osal_virt2phys(DspBinAddr.Me1StartAddr, &PhysAddr);
        pDspBinAddr->Me1StartAddr = PhysAddr;
        dsp_osal_virt2phys(DspBinAddr.Mdxf1StartAddr, &PhysAddr);
        pDspBinAddr->Mdxf1StartAddr = PhysAddr;
#endif

        Rval = dsp_osal_virt2phys(DspBinAddr.CoreEndAddr, &PhysAddr);
        dsp_osal_print_err_line(Rval, __func__, __LINE__);
        pDspBinAddr->CoreEndAddr = PhysAddr;
        Rval = dsp_osal_virt2phys(DspBinAddr.MeEndAddr, &PhysAddr);
        dsp_osal_print_err_line(Rval, __func__, __LINE__);
        pDspBinAddr->MeEndAddr = PhysAddr;
        Rval = dsp_osal_virt2phys(DspBinAddr.MdxfEndAddr, &PhysAddr);
        dsp_osal_print_err_line(Rval, __func__, __LINE__);
        pDspBinAddr->MdxfEndAddr = PhysAddr;
        Rval = dsp_osal_virt2phys(DspBinAddr.DefaultDataEndAddr, &PhysAddr);
        dsp_osal_print_err_line(Rval, __func__, __LINE__);
        pDspBinAddr->DefaultDataEndAddr = PhysAddr;
#if defined (CONFIG_SOC_CV5) || defined (CONFIG_SOC_CV52)
        dsp_osal_virt2phys(DspBinAddr.Core1EndAddr, &PhysAddr);
        pDspBinAddr->Core1EndAddr = PhysAddr;
        dsp_osal_virt2phys(DspBinAddr.Core2EndAddr, &PhysAddr);
        pDspBinAddr->Core2EndAddr = PhysAddr;
        dsp_osal_virt2phys(DspBinAddr.Core3EndAddr, &PhysAddr);
        pDspBinAddr->Core3EndAddr = PhysAddr;

        dsp_osal_virt2phys(DspBinAddr.Me1EndAddr, &PhysAddr);
        pDspBinAddr->Me1EndAddr = PhysAddr;
        dsp_osal_virt2phys(DspBinAddr.Mdxf1EndAddr, &PhysAddr);
        pDspBinAddr->Mdxf1EndAddr = PhysAddr;
#endif
    }
        break;
    case AMBADSP_DEBUG_DUMP:
    {
        dsp_osal_typecast(&pDspDebugDump, &arg);

        if (pDspDebugDump->Type == DSP_DBG_TYPE_API) {
            dsp_main_api_dump_init((UINT8)pDspDebugDump->Data[0U], pDspDebugDump->Data[1U]);
        } else if (pDspDebugDump->Type == DSP_DBG_TYPE_API_CHK) {
            dsp_main_api_check_init((UINT8)pDspDebugDump->Data[0U], pDspDebugDump->Data[1U]);
        } else if (pDspDebugDump->Type == DSP_DBG_TYPE_CMD) {
            dsp_cmd_show(pDspDebugDump->Data[0U], (UINT8)pDspDebugDump->Data[1U]);
        } else if (pDspDebugDump->Type == DSP_DBG_TYPE_CMD_ALL) {
            dsp_cmd_show_all((UINT8)pDspDebugDump->Data[0U]);
        } else if (pDspDebugDump->Type == DSP_DBG_TYPE_MSG) {
            dsp_msg_show(pDspDebugDump->Data[0U], (UINT8)pDspDebugDump->Data[1U]);
        } else if (pDspDebugDump->Type == DSP_DBG_TYPE_MSG_ALL) {
            dsp_msg_show_all((UINT8)pDspDebugDump->Data[0U]);
        } else if (pDspDebugDump->Type == DSP_DBG_TYPE_LOG_THRD) {
            Rval = dsp_set_debug_thread(pDspDebugDump->Data[0U], pDspDebugDump->Data[1U], (UINT8)pDspDebugDump->Data[2U]);
        } else if (pDspDebugDump->Type == DSP_DBG_TYPE_LOG_LVL) {
            Rval = dsp_set_debug_level(pDspDebugDump->Data[0U], pDspDebugDump->Data[1U], (UINT8)pDspDebugDump->Data[2U]);
        } else if (pDspDebugDump->Type == DSP_DBG_TYPE_LL_LOG_SHOW) {
            AmbaLL_LogShow(pDspDebugDump->Data[0U]);
        } else if (pDspDebugDump->Type == DSP_DBG_TYPE_INIT_DATA_SHOW) {
            DSP_DumpDspInitDataInfo();
        } else if (pDspDebugDump->Type == DSP_DBG_TYPE_STOP_DSP) {
            Rval = dsp_stop(pDspDebugDump->Data[0U]);
        } else if (pDspDebugDump->Type == DSP_DBG_TYPE_SYSCFG) {
            dsp_set_system_config((UINT8)pDspDebugDump->Data[0U], pDspDebugDump->Data[1U]);
        } else {
            // DO NOTHING
        }
    }
        break;

    case AMBADSP_CALC_HIER_BUF:
        dsp_osal_typecast(&pDspHierBufCalc, &arg);
        Rval = dsp_cal_hier_buffer_size(&pDspHierBufCalc->MaxMainWidth,
                                        &pDspHierBufCalc->MaxMainHeight,
                                        &pDspHierBufCalc->MaxHierWidth,
                                        &pDspHierBufCalc->MaxHierHeight,
                                        pDspHierBufCalc->OctaveMode,
                                        &pDspHierBufCalc->HierBufSize,
                                        &pDspHierBufCalc->HierBufWidth,
                                        &pDspHierBufCalc->HierBufHeight);
        break;
    case AMBADSP_CALC_RAW_PITCH:
        dsp_osal_typecast(&pDspRawPitchCalc, &arg);
        Rval = dsp_get_cmpr_raw_buf_info(pDspRawPitchCalc->Width,
                                         pDspRawPitchCalc->CmprRate,
                                         &pDspRawPitchCalc->RawWidth,
                                         &pDspRawPitchCalc->RawPitch);
        break;
    case AMBADSP_CALC_ENC_MV_BUF:
        dsp_osal_typecast(&pDspEncMvBufCalc, &arg);
        Rval = dsp_calc_enc_mv_buf_info(pDspEncMvBufCalc->Width,
                                        pDspEncMvBufCalc->Height,
                                        pDspEncMvBufCalc->Option,
                                        &pDspEncMvBufCalc->BufSize);
        break;
    case AMBADSP_PARSE_ENC_MV_BUF:
        dsp_osal_typecast(&pDspEncMvBufParse, &arg);
        Rval = dsp_parse_mv_buf(pDspEncMvBufParse->Width,
                                pDspEncMvBufParse->Height,
                                pDspEncMvBufParse->BufType,
                                pDspEncMvBufParse->Option,
                                pDspEncMvBufParse->MvBufAddr);
        break;
    case AMBADSP_CALC_VP_MSG_BUF:
        dsp_osal_typecast(&pDspVpMsgBuf, &arg);
        Rval = dsp_calc_vp_msg_buf_size(&pDspVpMsgBuf->NumMsgs,
                                        &pDspVpMsgBuf->MsgSize);
        break;
    case AMBADSP_GET_PROTECT_BUF:
    {
        UINT32 Size = 0U;
        UINT8 *pU8Val;

        dsp_osal_typecast(&pDspProtect, &arg);

        HL_GetProtectBufInfo(&ULAddr, &Size);
        if ((ULAddr > 0U) &&
            (Size <= AMBA_DSP_PROTECT_DATA_LEN)) {
            dsp_osal_typecast(&pU8Val, &ULAddr);

            Rval = dsp_osal_memcpy(pDspProtect->Data, pU8Val, AMBA_DSP_PROTECT_DATA_LEN);
            dsp_osal_print_err_line(Rval, __func__, __LINE__);
        }
    }
        break;
    case AMBADSP_GET_BUF_INFO:
    {
        AMBA_DSP_BUF_INFO_s BufInfo = {0};

        dsp_osal_typecast(&pDspBufInfo, &arg);

        Rval = dsp_main_get_buf_info(pDspBufInfo->Type, &BufInfo);
        dsp_osal_print_err_line(Rval, __func__, __LINE__);
        pDspBufInfo->Buf.Addr = BufInfo.BaseAddr;
        pDspBufInfo->Buf.Size = BufInfo.Size;
        pDspBufInfo->Buf.IsCached = BufInfo.IsCached;
    }
        break;
    case AMBADSP_CACHE_BUF_OPERATE:
    {
        void *pVoid = NULL;

        dsp_osal_typecast(&pDspCacheOp, &arg);

        if (pDspCacheOp->Op == DSP_CACHE_OP_CLEAN) {
            Rval = dsp_osal_phys2virt(pDspCacheOp->Addr, &ULAddr);
            dsp_osal_print_err_line(Rval, __func__, __LINE__);
            if (ULAddr != 0UL) {
                dsp_osal_typecast(&pVoid, &ULAddr);
                dsp_osal_cache_clean(pVoid, pDspCacheOp->Size);
            }
        } else if (pDspCacheOp->Op == DSP_CACHE_OP_INVLD) {
            Rval = dsp_osal_phys2virt(pDspCacheOp->Addr, &ULAddr);
            dsp_osal_print_err_line(Rval, __func__, __LINE__);
            if (ULAddr != 0UL) {
                dsp_osal_typecast(&pVoid, &ULAddr);
                dsp_osal_cache_invalidate(pVoid, pDspCacheOp->Size);
            }
        } else {
            // DO NOTHING
        }
    }
        break;
#if defined (CONFIG_SOC_CV5) || defined (CONFIG_SOC_CV52)
    case AMBADSP_SLICE_CFG_CALC:
    {
        dsp_osal_typecast(&pSliceCfg, &arg);
        Rval = dsp_slice_cfg_calc(pSliceCfg->Id,
                                  &pSliceCfg->Input,
                                  &pSliceCfg->Output,
                                  &pSliceCfg->SliceCfg);
    }
        break;
#endif
    case AMBADSP_GET_STATUS:
        dsp_osal_typecast(&pDspStatus, &arg);
        Rval = dsp_get_status(pDspStatus);
        break;
    /******************   Event    ******************/
    case AMBADSP_EVENT_HDLR_CONFIG:
        dsp_osal_typecast(&pDspEvntHdlrCfg, &arg);
        Rval = dsp_event_hdlr_ctrl_cfg(pDspEvntHdlrCfg->EventId,
                                       pDspEvntHdlrCfg->MaxNumHandlers,
                                       NULL);
        break;

    case AMBADSP_EVENT_HDLR_RESET:
        dsp_osal_typecast(&pU16, &arg);
        Rval = dsp_event_hdlr_ctrl_reset(*pU16);
        break;
    case AMBADSP_EVENT_HDLR_REGISTER:
        dsp_osal_typecast(&pDspEvntHdlrOp, &arg);
        Rval = dsp_event_hdlr_register(pDspEvntHdlrOp->EventId,
                                       NULL);
        break;
    case AMBADSP_EVENT_HDLR_DEREGISTER:
        dsp_osal_typecast(&pDspEvntHdlrOp, &arg);
        Rval = dsp_event_hdlr_unregister(pDspEvntHdlrOp->EventId,
                                         NULL);
        break;
    case AMBADSP_EVENT_GET_DATA:
    {
        dsp_osal_typecast(&pDspEvntData, &arg);
        (void)dsp_osal_fetch_evnt_data(pDspEvntData);
    }
        break;
    case AMBADSP_EVENT_RELEASE_INFO_POOL:
        dsp_osal_typecast(&pU16, &arg);
        Rval = dsp_event_release_info_pool(*pU16);
        break;
    /******************    Main    ******************/
    case AMBADSP_GET_DEFAULT_SYSCFG:
        dsp_osal_typecast(&pDspSysCfg, &arg);
        Rval = dsp_main_get_defsyscfg(pDspSysCfg);
        break;
    case AMBADSP_MAIN_INIT:
    {
        dsp_osal_typecast(&pDspSysCfg, &arg);

        /* Linux buffer comes from DspDrv */
        //Rval = dsp_osal_alloc_add_buf_addr(DSP_BUF_POOL_WORK, pDspSysCfg->WorkAreaAddr, pDspSysCfg->WorkAreaSize, 1U);
        //dsp_osal_print_err_line(Rval, __func__, __LINE__);
        //Rval = dsp_osal_alloc_add_buf_addr(DSP_BUF_POOL_DBG_LOG, pDspSysCfg->DebugLogDataAreaAddr, pDspSysCfg->DebugLogDataAreaSize, 0U);
        //dsp_osal_print_err_line(Rval, __func__, __LINE__);

        /* convert to VirtAddr */
        Rval = dsp_osal_phys2virt(pDspSysCfg->DebugLogDataAreaAddr, &ULAddr);
        dsp_osal_print_err_line(Rval, __func__, __LINE__);
        pDspSysCfg->DebugLogDataAreaAddr = ULAddr;
        Rval = dsp_osal_phys2virt(pDspSysCfg->WorkAreaAddr, &ULAddr);
        dsp_osal_print_err_line(Rval, __func__, __LINE__);
        pDspSysCfg->WorkAreaAddr = ULAddr;

        Rval = dsp_main_init(pDspSysCfg);
    }
        break;
    case AMBADSP_MAIN_SUSPEND:
        Rval = dsp_main_suspend();
        break;
    case AMBADSP_MAIN_RESUME:
        Rval = dsp_main_resume();
        break;
    case AMBADSP_SET_WORK_MEMORY:
    {
        dsp_osal_typecast(&pDspMem, &arg);

        /* Linux buffer comes from DspDrv */
        //Rval = dsp_osal_alloc_add_buf_addr(DSP_BUF_POOL_WORK, pDspMem->Addr, pDspSysCfg->WorkAreaSize, 1U);
        //dsp_osal_print_err_line(Rval, __func__, __LINE__);

        /* convert to VirtAddr */
        Rval = dsp_osal_phys2virt(pDspMem->Addr, &ULAddr);
        dsp_osal_print_err_line(Rval, __func__, __LINE__);
        pDspMem->Addr = ULAddr;

        Rval = dsp_main_set_work_area(pDspMem->Addr, pDspMem->Size);
    }
        break;
    case AMBADSP_MAIN_MSG_PARSE:
    {
        dsp_osal_typecast(&pU32, &arg);
        Rval = dsp_main_msg_parse_entry(*pU32);
    }
        break;
    case AMBADSP_MAIN_WAIT_VIN_INT:
    {
        dsp_osal_typecast(&pDspWaitSig, &arg);
        Rval = dsp_main_wait_vin_interrupt(pDspWaitSig->Flag,
                                           &pDspWaitSig->ActualFlag,
                                           pDspWaitSig->TimeOut);
    }
        break;
    case AMBADSP_MAIN_WAIT_VOUT_INT:
        dsp_osal_typecast(&pDspWaitSig, &arg);
        Rval = dsp_main_wait_vout_interrupt(pDspWaitSig->Flag,
                                            &pDspWaitSig->ActualFlag,
                                            pDspWaitSig->TimeOut);
        break;
    case AMBADSP_MAIN_WAIT_FLAG:
    {
        dsp_osal_typecast(&pDspWaitSig, &arg);
        Rval = dsp_main_wait_flag(pDspWaitSig->Flag,
                                  &pDspWaitSig->ActualFlag,
                                  pDspWaitSig->TimeOut);
    }
        break;
    case AMBADSP_RESC_LIMIT_CONFIG:
    {
        AMBA_DSP_RESOURCE_s DspResc = {0};

        dsp_osal_typecast(&pDspResc, &arg);
        {
            DspResc.LiveviewResource.VideoPipe = pDspResc->LiveviewResource.VideoPipe;
            DspResc.LiveviewResource.LowDelayMode = pDspResc->LiveviewResource.LowDelayMode;
            DspResc.LiveviewResource.MaxVinBit = pDspResc->LiveviewResource.MaxVinBit;
            DspResc.LiveviewResource.MaxViewZoneNum = pDspResc->LiveviewResource.MaxViewZoneNum;
            Rval = dsp_osal_memcpy(DspResc.LiveviewResource.MaxVinVirtChanBit, pDspResc->LiveviewResource.MaxVinVirtChanBit, sizeof(UINT16)*AMBA_DSP_MAX_VIN_NUM);
            dsp_osal_print_err_line(Rval, __func__, __LINE__);
            Rval = dsp_osal_memcpy(DspResc.LiveviewResource.MaxVinVirtChanOutputNum, pDspResc->LiveviewResource.MaxVinVirtChanOutputNum, (sizeof(UINT16)*AMBA_DSP_MAX_VIN_NUM)*AMBA_DSP_MAX_VIRT_CHAN_NUM);
            dsp_osal_print_err_line(Rval, __func__, __LINE__);
            Rval = dsp_osal_memcpy(DspResc.LiveviewResource.MaxVinVirtChanWidth, pDspResc->LiveviewResource.MaxVinVirtChanWidth, (sizeof(UINT16)*AMBA_DSP_MAX_VIN_NUM)*AMBA_DSP_MAX_VIRT_CHAN_NUM);
            dsp_osal_print_err_line(Rval, __func__, __LINE__);
            Rval = dsp_osal_memcpy(DspResc.LiveviewResource.MaxVinVirtChanHeight, pDspResc->LiveviewResource.MaxVinVirtChanHeight, (sizeof(UINT16)*AMBA_DSP_MAX_VIN_NUM)*AMBA_DSP_MAX_VIRT_CHAN_NUM);
            dsp_osal_print_err_line(Rval, __func__, __LINE__);

            for (i = 0; i < AMBA_DSP_MAX_VIN_NUM; i++) {
                for (j = 0; j < AMBA_DSP_MAX_VIRT_CHAN_NUM; j++) {
                    DspResc.LiveviewResource.RawBuf[i][j].AllocType = pDspResc->LiveviewResource.RawBuf[i][j].AllocType;
                    DspResc.LiveviewResource.RawBuf[i][j].BufNum = pDspResc->LiveviewResource.RawBuf[i][j].BufNum;
                    Rval = dsp_osal_memcpy(&DspResc.LiveviewResource.RawBuf[i][j].Buf, &pDspResc->LiveviewResource.RawBuf[i][j].Buf, sizeof(AMBA_DSP_RAW_BUF_s));
                    dsp_osal_print_err_line(Rval, __func__, __LINE__);
                    if (pDspResc->LiveviewResource.RawBuf[i][j].Buf.BaseAddr > 0U) {
                        dsp_osal_phys2virt(pDspResc->LiveviewResource.RawBuf[i][j].Buf.BaseAddr, &ULAddr);
                        DspResc.LiveviewResource.RawBuf[i][j].Buf.BaseAddr = ULAddr;
                    } else {
                        DspResc.LiveviewResource.RawBuf[i][j].Buf.BaseAddr = 0U;
                    }
                    Rval = dsp_osal_memcpy(&DspResc.LiveviewResource.RawBuf[i][j].AuxBuf, &pDspResc->LiveviewResource.RawBuf[i][j].AuxBuf, sizeof(AMBA_DSP_RAW_BUF_s));
                    dsp_osal_print_err_line(Rval, __func__, __LINE__);
                    if (pDspResc->LiveviewResource.RawBuf[i][j].AuxBuf.BaseAddr > 0U) {
                        dsp_osal_phys2virt(pDspResc->LiveviewResource.RawBuf[i][j].AuxBuf.BaseAddr, &ULAddr);
                        DspResc.LiveviewResource.RawBuf[i][j].AuxBuf.BaseAddr = ULAddr;
                    } else {
                        DspResc.LiveviewResource.RawBuf[i][j].AuxBuf.BaseAddr = 0U;
                    }
                    DspResc.LiveviewResource.RawBuf[i][j].pRawBufTbl = RescExtBufTbl[i][j];
                    DspResc.LiveviewResource.RawBuf[i][j].pAuxBufTbl = RescExtBufTbl[i][j];
                    DspResc.LiveviewResource.RawBuf[i][j].RawFormat = pDspResc->LiveviewResource.RawBuf[i][j].RawFormat;
                }
            }


            Rval = dsp_osal_memcpy(DspResc.LiveviewResource.MaxVinBootTimeout, pDspResc->LiveviewResource.MaxVinBootTimeout, sizeof(UINT32)*AMBA_DSP_MAX_VIN_NUM);
            dsp_osal_print_err_line(Rval, __func__, __LINE__);
            Rval = dsp_osal_memcpy(DspResc.LiveviewResource.MaxVinTimeout, pDspResc->LiveviewResource.MaxVinTimeout, sizeof(UINT32)*AMBA_DSP_MAX_VIN_NUM);
            dsp_osal_print_err_line(Rval, __func__, __LINE__);
            Rval = dsp_osal_memcpy(DspResc.LiveviewResource.MaxRaw2YuvDelay, pDspResc->LiveviewResource.MaxRaw2YuvDelay, sizeof(UINT16)*AMBA_DSP_MAX_VIN_NUM);
            dsp_osal_print_err_line(Rval, __func__, __LINE__);
#if defined (CONFIG_SOC_CV5) || defined (CONFIG_SOC_CV52) || defined (CONFIG_SOC_CV22) || defined (CONFIG_SOC_CV2) || defined (CONFIG_SOC_CV25) || defined (CONFIG_SOC_CV28) || defined (CONFIG_SOC_H32)
            Rval = dsp_osal_memcpy(DspResc.LiveviewResourceEx.MaxVinShortFrameThreshold, pDspResc->LiveviewResourceEx.MaxVinShortFrameThreshold, sizeof(UINT16)*AMBA_DSP_MAX_VIN_NUM);
            dsp_osal_print_err_line(Rval, __func__, __LINE__);
#endif
            Rval = dsp_osal_memcpy(DspResc.LiveviewResource.MaxWarpDma, pDspResc->LiveviewResource.MaxWarpDma, sizeof(UINT16)*AMBA_DSP_MAX_VIEWZONE_NUM);
            dsp_osal_print_err_line(Rval, __func__, __LINE__);
            Rval = dsp_osal_memcpy(DspResc.LiveviewResource.MaxWarpWaitLineLuma, pDspResc->LiveviewResource.MaxWarpWaitLineLuma, sizeof(UINT16)*AMBA_DSP_MAX_VIEWZONE_NUM);
            dsp_osal_print_err_line(Rval, __func__, __LINE__);
            Rval = dsp_osal_memcpy(DspResc.LiveviewResource.MaxWarpWaitLineChroma, pDspResc->LiveviewResource.MaxWarpWaitLineChroma, sizeof(UINT16)*AMBA_DSP_MAX_VIEWZONE_NUM);
            dsp_osal_print_err_line(Rval, __func__, __LINE__);
            Rval = dsp_osal_memcpy(DspResc.LiveviewResource.MaxHierWidth, pDspResc->LiveviewResource.MaxHierWidth, sizeof(UINT16)*AMBA_DSP_MAX_VIEWZONE_NUM);
            dsp_osal_print_err_line(Rval, __func__, __LINE__);
            Rval = dsp_osal_memcpy(DspResc.LiveviewResource.MaxHierHeight, pDspResc->LiveviewResource.MaxHierHeight, sizeof(UINT16)*AMBA_DSP_MAX_VIEWZONE_NUM);
            dsp_osal_print_err_line(Rval, __func__, __LINE__);
            Rval = dsp_osal_memcpy(DspResc.LiveviewResource.MaxLndtWidth, pDspResc->LiveviewResource.MaxLndtWidth, sizeof(UINT16)*AMBA_DSP_MAX_VIEWZONE_NUM);
            dsp_osal_print_err_line(Rval, __func__, __LINE__);
            Rval = dsp_osal_memcpy(DspResc.LiveviewResource.MaxLndtHeight, pDspResc->LiveviewResource.MaxLndtHeight, sizeof(UINT16)*AMBA_DSP_MAX_VIEWZONE_NUM);
            dsp_osal_print_err_line(Rval, __func__, __LINE__);
            DspResc.LiveviewResource.MaxVirtVinBit = pDspResc->LiveviewResource.MaxVirtVinBit;
            Rval = dsp_osal_memcpy(DspResc.LiveviewResource.MaxVirtVinOutputNum, pDspResc->LiveviewResource.MaxVirtVinOutputNum, sizeof(UINT16)*AMBA_DSP_MAX_VIRT_VIN_NUM);
            dsp_osal_print_err_line(Rval, __func__, __LINE__);
            Rval = dsp_osal_memcpy(DspResc.LiveviewResource.MaxVirtVinWidth, pDspResc->LiveviewResource.MaxVirtVinWidth, sizeof(UINT16)*AMBA_DSP_MAX_VIRT_VIN_NUM);
            dsp_osal_print_err_line(Rval, __func__, __LINE__);
            Rval = dsp_osal_memcpy(DspResc.LiveviewResource.MaxVirtVinHeight, pDspResc->LiveviewResource.MaxVirtVinHeight, sizeof(UINT16)*AMBA_DSP_MAX_VIRT_VIN_NUM);
            dsp_osal_print_err_line(Rval, __func__, __LINE__);
            Rval = dsp_osal_memcpy(DspResc.LiveviewResource.DefaultRawIsoConfigAddr, pDspResc->LiveviewResource.DefaultRawIsoConfigAddr, sizeof(AMBA_DSP_DEFAULT_RAW_ISOCFG_CTRL_s)*AMBA_DSP_MAX_VIEWZONE_NUM);
            dsp_osal_print_err_line(Rval, __func__, __LINE__);
            for (i = 0; i < AMBA_DSP_MAX_VIEWZONE_NUM; i++) {
                if (pDspResc->LiveviewResource.DefaultRawIsoConfigAddr[i].CfgAddress > 0U) {
                    dsp_osal_phys2virt(pDspResc->LiveviewResource.DefaultRawIsoConfigAddr[i].CfgAddress, &ULAddr);
                    DspResc.LiveviewResource.DefaultRawIsoConfigAddr[i].CfgAddress = ULAddr;
                } else {
                    DspResc.LiveviewResource.DefaultRawIsoConfigAddr[i].CfgAddress = 0U;
                }
            }

            Rval = dsp_osal_memcpy(DspResc.LiveviewResource.DefaultRawBuf, pDspResc->LiveviewResource.DefaultRawBuf, (sizeof(AMBA_DSP_RAW_BUF_s)*AMBA_DSP_MAX_VIN_NUM)*AMBA_DSP_MAX_VIRT_CHAN_NUM);
            dsp_osal_print_err_line(Rval, __func__, __LINE__);
            for (i = 0; i < AMBA_DSP_MAX_VIN_NUM; i++) {
                for (j = 0; j < AMBA_DSP_MAX_VIRT_CHAN_NUM; j++) {
                    if (pDspResc->LiveviewResource.DefaultRawBuf[i][j].BaseAddr > 0U) {
                        dsp_osal_phys2virt(pDspResc->LiveviewResource.DefaultRawBuf[i][j].BaseAddr, &ULAddr);
                        DspResc.LiveviewResource.DefaultRawBuf[i][j].BaseAddr = ULAddr;
                    } else {
                        DspResc.LiveviewResource.DefaultRawBuf[i][j].BaseAddr = 0U;
                    }
                }
            }

            Rval = dsp_osal_memcpy(DspResc.LiveviewResource.DefaultAuxRawBuf, pDspResc->LiveviewResource.DefaultAuxRawBuf, (sizeof(AMBA_DSP_RAW_BUF_s)*AMBA_DSP_MAX_VIN_NUM)*AMBA_DSP_MAX_VIRT_CHAN_NUM);
            dsp_osal_print_err_line(Rval, __func__, __LINE__);
            for (i = 0; i < AMBA_DSP_MAX_VIN_NUM; i++) {
                for (j = 0; j < AMBA_DSP_MAX_VIRT_CHAN_NUM; j++) {
                    if (pDspResc->LiveviewResource.DefaultAuxRawBuf[i][j].BaseAddr > 0U) {
                        dsp_osal_phys2virt(pDspResc->LiveviewResource.DefaultAuxRawBuf[i][j].BaseAddr, &ULAddr);
                        DspResc.LiveviewResource.DefaultAuxRawBuf[i][j].BaseAddr = ULAddr;
                    } else {
                        DspResc.LiveviewResource.DefaultAuxRawBuf[i][j].BaseAddr = 0U;
                    }
                }
            }

            Rval = dsp_osal_memcpy(DspResc.LiveviewResource.DefaultRawFormat, pDspResc->LiveviewResource.DefaultRawFormat, sizeof(UINT16)*AMBA_DSP_MAX_VIN_NUM);
        }
        Rval = dsp_osal_memcpy(&DspResc.EncodeResource, &pDspResc->EncodeResource, sizeof(AMBA_DSP_ENCODE_RESOURCE_s));
        dsp_osal_print_err_line(Rval, __func__, __LINE__);
        Rval = dsp_osal_memcpy(&DspResc.StillResource, &pDspResc->StillResource, sizeof(AMBA_DSP_STILL_RESOURCE_s));
        dsp_osal_print_err_line(Rval, __func__, __LINE__);
        Rval = dsp_osal_memcpy(&DspResc.DecodeResource, &pDspResc->DecodeResource, sizeof(AMBA_DSP_DECODE_RESOURCE_s));
        dsp_osal_print_err_line(Rval, __func__, __LINE__);
        Rval = dsp_osal_memcpy(&DspResc.DisplayResource, &pDspResc->DisplayResource, sizeof(AMBA_DSP_DISPLAY_RESOURCE_s));
        dsp_osal_print_err_line(Rval, __func__, __LINE__);
        Rval = dsp_osal_memcpy(&DspResc.ExtMemEncResource, &pDspResc->ExtMemEncResource, sizeof(AMBA_DSP_EXTMEM_ENC_RESOURCE_s));
        dsp_osal_print_err_line(Rval, __func__, __LINE__);
        Rval = dsp_osal_memcpy(&DspResc.LiveviewResourceEx, &pDspResc->LiveviewResourceEx, sizeof(AMBA_DSP_LIVEVIEW_RESOURCE_EX_s));
        dsp_osal_print_err_line(Rval, __func__, __LINE__);
        Rval = dsp_resource_limit_config(&DspResc);
    }
        break;
    case AMBADSP_PARLOAD_CONFIG:
        dsp_osal_typecast(&pDspParLoadCfg, &arg);
        Rval = dsp_parload_config(pDspParLoadCfg->Enable,
                                  pDspParLoadCfg->Data);
        break;
    case AMBADSP_PARLOAD_REGION_UNLOCK:
        dsp_osal_typecast(&pDspParLoadUnlock, &arg);
        Rval = dsp_parload_region_unlock(pDspParLoadUnlock->RegionId,
                                         pDspParLoadUnlock->SubRegionId);
        break;
    case AMBADSP_SYS_DRV_CFG:
        dsp_osal_typecast(&pDspSysDrvCfg, &arg);
        Rval = dsp_sys_drv_cfg(pDspSysDrvCfg->Type,
                               &pDspSysDrvCfg->SysDrvCfg.Data);
        break;
    case AMBADSP_SET_PROTECT_AREA:
    {
        dsp_osal_typecast(&pDspProtectArea, &arg);

        if (pDspProtectArea->Type == DSP_PROTECT_AREA_DATA) {
            Rval = dsp_osal_alloc_add_buf_addr(DSP_BUF_POOL_DATA,
                                              pDspProtectArea->Addr,
                                              pDspProtectArea->Size,
                                              pDspProtectArea->IsCached);
            dsp_osal_print_err_line(Rval, __func__, __LINE__);
        }

        Rval = dsp_main_set_protect_area(pDspProtectArea->Type,
                                         pDspProtectArea->Addr,
                                         pDspProtectArea->Size,
                                         pDspProtectArea->IsCached);
    }
        break;
    case AMBADSP_EXT_RAW_BUF_TBL_CFG:
    {
        dsp_osal_typecast(&pDspExtBufTbl, &arg);

        if (pDspExtBufTbl->Type == DSP_EXT_BUF_TYPE_RESC_RAW) {
            if ((pDspExtBufTbl->Id < AMBA_DSP_MAX_VIN_NUM) &&
                (pDspExtBufTbl->SubId < AMBA_DSP_MAX_VIRT_CHAN_NUM)) {
                for (i = 0U; i < MAX_EXT_DISTINCT_DEPTH; i++) {
                    if (pDspExtBufTbl->BufTbl[i] > 0U) {
                        dsp_osal_phys2virt(pDspExtBufTbl->BufTbl[i], &ULAddr);
                        RescExtBufTbl[pDspExtBufTbl->Id][pDspExtBufTbl->SubId][i] = ULAddr;
                    } else {
                        RescExtBufTbl[pDspExtBufTbl->Id][pDspExtBufTbl->SubId][i] = 0U;
                    }

                    if (pDspExtBufTbl->AuxBufTbl[i] > 0U) {
                        dsp_osal_phys2virt(pDspExtBufTbl->AuxBufTbl[i], &ULAddr);
                        RescExtAuxBufTbl[pDspExtBufTbl->Id][pDspExtBufTbl->SubId][i] = ULAddr;
                    } else {
                        RescExtAuxBufTbl[pDspExtBufTbl->Id][pDspExtBufTbl->SubId][i] = 0U;
                    }
                }
            }
        } else {
            // DO NOTHING
        }
    }
        break;
    /******************    Vout    ******************/
    case AMBADSP_VOUT_RESET:
        dsp_osal_typecast(&pU8, &arg);
        Rval = dsp_vout_reset(*pU8);
        break;
    case AMBADSP_VOUT_MIXER_CFG:
        dsp_osal_typecast(&pDspVoutMixerCfg, &arg);
        Rval = dsp_vout_mixer_cfg(pDspVoutMixerCfg->VoutIdx,
                                  &pDspVoutMixerCfg->MixerCfg);
        break;
    case AMBADSP_VOUT_MIXER_BGC_CFG:
        dsp_osal_typecast(&pDspVoutMixerBgcCfg, &arg);
        Rval = dsp_vout_mixer_cfg_bgcolor(pDspVoutMixerBgcCfg->VoutIdx,
                                          pDspVoutMixerBgcCfg->BackColorYUV);
        break;
    case AMBADSP_VOUT_MIXER_HLC_CFG:
        dsp_osal_typecast(&pDspVoutMixerHlcCfg, &arg);
        Rval = dsp_vout_mixer_cfg_highcolor(pDspVoutMixerHlcCfg->VoutIdx,
                                            pDspVoutMixerHlcCfg->LumaThreshold,
                                            pDspVoutMixerHlcCfg->HighlightColorYUV);
        break;
    case AMBADSP_VOUT_MIXER_CSC_CFG:
        dsp_osal_typecast(&pDspVoutMixerCscCfg, &arg);
        Rval = dsp_vout_mixer_cfg_csc(pDspVoutMixerCscCfg->VoutIdx,
                                      pDspVoutMixerCscCfg->CscCtrl);
        break;
    case AMBADSP_VOUT_MIXER_CSC_MATRIX_CFG:
        dsp_osal_typecast(&pDspVoutMixerCscMatrixCfg, &arg);
        Rval = dsp_vout_mixer_csc_matrix_cfg(pDspVoutMixerCscMatrixCfg->VoutIdx,
                                             &pDspVoutMixerCscMatrixCfg->CscMatrix);
        break;
    case AMBADSP_VOUT_MIXER_CTRL:
        dsp_osal_typecast(&pU8, &arg);
        Rval = dsp_vout_mixer_ctrl(*pU8);
        break;
    case AMBADSP_VOUT_OSD_BUF_CFG:
    {
        dsp_osal_typecast(&pDspVoutOsdBufCfg, &arg);

        /* convert to VirtAddr */
        Rval = dsp_osal_phys2virt(pDspVoutOsdBufCfg->BufCfg.BaseAddr, &ULAddr);
        dsp_osal_print_err_line(Rval, __func__, __LINE__);
        pDspVoutOsdBufCfg->BufCfg.BaseAddr = ULAddr;
        Rval = dsp_osal_phys2virt(pDspVoutOsdBufCfg->BufCfg.CLUTAddr, &ULAddr);
        dsp_osal_print_err_line(Rval, __func__, __LINE__);
        pDspVoutOsdBufCfg->BufCfg.CLUTAddr = ULAddr;

        Rval = dsp_vout_osd_cfg_buf(pDspVoutOsdBufCfg->VoutIdx,
                                    &pDspVoutOsdBufCfg->BufCfg);
    }
        break;
    case AMBADSP_VOUT_OSD_CTRL:
        dsp_osal_typecast(&pDspVoutOsdCtrl, &arg);
        Rval = dsp_vout_osd_ctrl(pDspVoutOsdCtrl->VoutIdx,
                                 pDspVoutOsdCtrl->Enable,
                                 pDspVoutOsdCtrl->SyncWithVin,
                                 &pDspVoutOsdCtrl->AttachedRawSeq);
        break;
    case AMBADSP_VOUT_DISP_CFG:
        dsp_osal_typecast(&pDspVoutDispCfg, &arg);
        Rval = dsp_vout_display_cfg(pDspVoutDispCfg->VoutIdx,
                                    &pDspVoutDispCfg->Cfg);
        break;
    case AMBADSP_VOUT_DISP_CTRL:
        dsp_osal_typecast(&pU8, &arg);
        Rval = dsp_vout_display_ctrl(*pU8);
        break;
    case AMBADSP_VOUT_DISP_GAMMA_CFG:
    {
        dsp_osal_typecast(&pDspVoutDispGammaCfg, &arg);

        /* convert to VirtAddr */
        Rval = dsp_osal_phys2virt(pDspVoutDispGammaCfg->TableAddr, &ULAddr);
        dsp_osal_print_err_line(Rval, __func__, __LINE__);
        pDspVoutDispGammaCfg->TableAddr = ULAddr;

        Rval = dsp_vout_display_cfg_gamma(pDspVoutDispGammaCfg->VoutIdx,
                                          pDspVoutDispGammaCfg->TableAddr);
    }
        break;
    case AMBADSP_VOUT_DISP_GAMMA_CTRL:
        dsp_osal_typecast(&pDspVoutDispGammaCtrl, &arg);
        Rval = dsp_vout_display_ctrl_gamma(pDspVoutDispGammaCtrl->VoutIdx,
                                           pDspVoutDispGammaCtrl->Enable);
        break;
    case AMBADSP_VOUT_VIDEO_CFG:
    {
        dsp_osal_typecast(&pDspVoutVideoCfg, &arg);

        /* convert to VirtAddr */
        Rval = dsp_osal_phys2virt(pDspVoutVideoCfg->Cfg.DefaultImgConfig.BaseAddrY, &ULAddr);
        dsp_osal_print_err_line(Rval, __func__, __LINE__);
        pDspVoutVideoCfg->Cfg.DefaultImgConfig.BaseAddrY = ULAddr;
        Rval = dsp_osal_phys2virt(pDspVoutVideoCfg->Cfg.DefaultImgConfig.BaseAddrUV, &ULAddr);
        dsp_osal_print_err_line(Rval, __func__, __LINE__);
        pDspVoutVideoCfg->Cfg.DefaultImgConfig.BaseAddrUV = ULAddr;

        Rval = dsp_vout_video_cfg(pDspVoutVideoCfg->VoutIdx,
                                  &pDspVoutVideoCfg->Cfg);
    }
        break;
    case AMBADSP_VOUT_VIDEO_CTRL:
        dsp_osal_typecast(&pDspVoutVideoCtrl, &arg);
        Rval = dsp_vout_video_ctrl(pDspVoutVideoCtrl->VoutIdx,
                                   pDspVoutVideoCtrl->Enable,
                                   pDspVoutVideoCtrl->SyncWithVin,
                                   &pDspVoutVideoCtrl->AttachedRawSeq);
        break;
    case AMBADSP_VOUT_MIXER_BIND_CFG:
        dsp_osal_typecast(&pDspVoutMixerBindCfg, &arg);
        Rval = dsp_vout_cfg_mixer_binding(pDspVoutMixerBindCfg->NumVout,
                                          pDspVoutMixerBindCfg->VoutIdx,
                                          pDspVoutMixerBindCfg->PathCfg);
        break;
    case AMBADSP_VOUT_DVE_CFG:
        dsp_osal_typecast(&pDspVoutDveCfg, &arg);
        Rval = dsp_vout_dve_cfg(pDspVoutDveCfg->VoutIdx,
                                pDspVoutDveCfg->DveMode);
        break;
    case AMBADSP_VOUT_DVE_CTRL:
        dsp_osal_typecast(&pU8, &arg);
        Rval = dsp_vout_dve_ctrl(*pU8);
        break;
    case AMBADSP_VOUT_DRV_CFG:
        dsp_osal_typecast(&pDspVoutDrvCfg, &arg);
        Rval = dsp_vout_drv_cfg(pDspVoutDrvCfg->VoutId,
                                pDspVoutDrvCfg->Type,
                                pDspVoutDrvCfg->VoutDrvCfg.Data);
        break;

    /******************  Liveview  ******************/
    case AMBADSP_LIVEVIEW_CFG:
    {
        static AMBA_DSP_LIVEVIEW_VIEWZONE_CFG_s DspLvViewZoneCfg[AMBA_DSP_MAX_VIEWZONE_NUM] = {0};
        static AMBA_DSP_LIVEVIEW_STREAM_CFG_s DspLvStrmCfg[AMBA_DSP_MAX_YUVSTRM_NUM] = {0};
        static AMBA_DSP_LIVEVIEW_YUV_BUF_s PyramidBuf[AMBA_DSP_MAX_VIEWZONE_NUM] = {0};
        static AMBA_DSP_LIVEVIEW_YUV_BUF_s LaneDetectBuf[AMBA_DSP_MAX_VIEWZONE_NUM] = {0};
        static ULONG ExtPyramidBufTbl[AMBA_DSP_MAX_VIEWZONE_NUM][MAX_EXT_DISTINCT_DEPTH] = {0};
        static ULONG ExtLaneDetectBuf[AMBA_DSP_MAX_VIEWZONE_NUM][MAX_EXT_DISTINCT_DEPTH] = {0};
#if defined (CONFIG_SOC_CV2FS) || defined (CONFIG_SOC_CV22FS) || defined (CONFIG_SOC_CV5) || defined (CONFIG_SOC_CV52)
        static AMBA_DSP_LIVEVIEW_YUV_BUF_s MainY12Buf[AMBA_DSP_MAX_VIEWZONE_NUM] = {0};
        static ULONG ExtMainY12BufTbl[AMBA_DSP_MAX_VIEWZONE_NUM][MAX_EXT_DISTINCT_DEPTH] = {0};
#endif
#if defined (CONFIG_SOC_CV5) || defined (CONFIG_SOC_CV52)
        static AMBA_DSP_LIVEVIEW_YUV_BUF_s MainIrBuf[AMBA_DSP_MAX_VIEWZONE_NUM] = {0};
        static ULONG ExtMainIrBufTbl[AMBA_DSP_MAX_VIEWZONE_NUM][MAX_EXT_DISTINCT_DEPTH] = {0};
#endif
#if !defined (CONFIG_SOC_CV2FS) && !defined (CONFIG_SOC_CV22FS) && !defined (CONFIG_SOC_CV5) && !defined (CONFIG_SOC_CV52)
        static AMBA_DSP_LIVEVIEW_YUV_BUF_s IntMainBuf[AMBA_DSP_MAX_VIEWZONE_NUM] = {0};
        static ULONG ExtIntMainBufTbl[AMBA_DSP_MAX_VIEWZONE_NUM][MAX_EXT_DISTINCT_DEPTH] = {0};
#endif
        static AMBA_DSP_LIVEVIEW_CHANNEL_WINDOW_s ChanCfg[AMBA_DSP_MAX_YUVSTRM_NUM][AMBA_DSP_MAX_YUVSTRM_VIEW_NUM];
        static AMBA_DSP_BUF_s BldBuf[AMBA_DSP_MAX_YUVSTRM_NUM][AMBA_DSP_MAX_YUVSTRM_VIEW_NUM][AMBA_DSP_MAX_YUVSTRM_BLD_NUM];
        static AMBA_DSP_WINDOW_DIMENSION_s MaxChanWin[AMBA_DSP_MAX_YUVSTRM_NUM][AMBA_DSP_MAX_YUVSTRM_VIEW_NUM] = {0};
        static uint32_t ViewZonePattern = 0U, YuvStrmPattern = 0U;
        static uint32_t ViewZoneBit = 0U, YuvStrmBit = 0U;
        static uint16_t ViewZoneNum = 0U, YuvStrmNum = 0U;
        static uint32_t CfgBit = 0U;
        uint8_t *pU8Val = NULL;
        const dsp_liveview_viewzone_cfg_t *pCfgViewZone = NULL;
        dsp_liveview_strm_cfg_t *pCfgYuvStrm = NULL;

        dsp_osal_typecast(&pDspLiveviewCfg, &arg);
        pU8Val = pDspLiveviewCfg->cfg.Data;
        if (pDspLiveviewCfg->Type == DSP_LV_CFG_TYPE_VIEWZONE) {
            if (pDspLiveviewCfg->FreshNew == 1U) {
                Rval = dsp_osal_memset(DspLvViewZoneCfg, 0, sizeof(AMBA_DSP_LIVEVIEW_VIEWZONE_CFG_s)*AMBA_DSP_MAX_VIEWZONE_NUM);
                dsp_osal_print_err_line(Rval, __func__, __LINE__);
                Rval = dsp_osal_memset(PyramidBuf, 0, sizeof(AMBA_DSP_LIVEVIEW_YUV_BUF_s)*AMBA_DSP_MAX_VIEWZONE_NUM);
                dsp_osal_print_err_line(Rval, __func__, __LINE__);
                Rval = dsp_osal_memset(LaneDetectBuf, 0, sizeof(AMBA_DSP_LIVEVIEW_YUV_BUF_s)*AMBA_DSP_MAX_VIEWZONE_NUM);
                dsp_osal_print_err_line(Rval, __func__, __LINE__);
                Rval = dsp_osal_memset(ExtPyramidBufTbl, 0, (sizeof(ULONG)*AMBA_DSP_MAX_VIEWZONE_NUM)*MAX_EXT_DISTINCT_DEPTH);
                dsp_osal_print_err_line(Rval, __func__, __LINE__);
                Rval = dsp_osal_memset(ExtLaneDetectBuf, 0, (sizeof(ULONG)*AMBA_DSP_MAX_VIEWZONE_NUM)*MAX_EXT_DISTINCT_DEPTH);
                dsp_osal_print_err_line(Rval, __func__, __LINE__);
#if defined (CONFIG_SOC_CV2FS) || defined (CONFIG_SOC_CV22FS) || defined (CONFIG_SOC_CV5) || defined (CONFIG_SOC_CV52)
                Rval = dsp_osal_memset(MainY12Buf, 0, sizeof(AMBA_DSP_LIVEVIEW_YUV_BUF_s)*AMBA_DSP_MAX_VIEWZONE_NUM);
                dsp_osal_print_err_line(Rval, __func__, __LINE__);
                Rval = dsp_osal_memset(ExtMainY12BufTbl, 0, (sizeof(ULONG)*AMBA_DSP_MAX_VIEWZONE_NUM)*MAX_EXT_DISTINCT_DEPTH);
                dsp_osal_print_err_line(Rval, __func__, __LINE__);
#endif
#if defined (CONFIG_SOC_CV5) || defined (CONFIG_SOC_CV52)
                Rval = dsp_osal_memset(MainIrBuf, 0, sizeof(AMBA_DSP_LIVEVIEW_YUV_BUF_s)*AMBA_DSP_MAX_VIEWZONE_NUM);
                dsp_osal_print_err_line(Rval, __func__, __LINE__);
                Rval = dsp_osal_memset(ExtMainIrBufTbl, 0, (sizeof(ULONG)*AMBA_DSP_MAX_VIEWZONE_NUM)*MAX_EXT_DISTINCT_DEPTH);
                dsp_osal_print_err_line(Rval, __func__, __LINE__);
#endif
#if !defined (CONFIG_SOC_CV2FS) && !defined (CONFIG_SOC_CV22FS) && !defined (CONFIG_SOC_CV5) && !defined (CONFIG_SOC_CV52)
                Rval = dsp_osal_memset(IntMainBuf, 0, sizeof(AMBA_DSP_LIVEVIEW_YUV_BUF_s)*AMBA_DSP_MAX_VIEWZONE_NUM);
                dsp_osal_print_err_line(Rval, __func__, __LINE__);
                Rval = dsp_osal_memset(ExtIntMainBufTbl, 0, (sizeof(ULONG)*AMBA_DSP_MAX_VIEWZONE_NUM)*MAX_EXT_DISTINCT_DEPTH);
                dsp_osal_print_err_line(Rval, __func__, __LINE__);
#endif
                ViewZonePattern = ((UINT32)1U << pDspLiveviewCfg->TotalNum) - 1U;
                ViewZoneBit = 0U;
                ViewZoneNum = pDspLiveviewCfg->TotalNum;
            }

            i = pDspLiveviewCfg->Index;

            dsp_osal_typecast(&pCfgViewZone, &pU8Val);
            DspLvViewZoneCfg[i].ViewZoneId = pCfgViewZone->ViewZoneId;
            DspLvViewZoneCfg[i].VinSource = pCfgViewZone->VinSource;
            DspLvViewZoneCfg[i].VinId = pCfgViewZone->VinId;
            DspLvViewZoneCfg[i].VinVirtChId = pCfgViewZone->VinVirtChId;
            DspLvViewZoneCfg[i].IsVirtChan = pCfgViewZone->IsVirtChan;
            Rval = dsp_osal_memcpy(&DspLvViewZoneCfg[i].VinRoi, &pCfgViewZone->VinRoi, sizeof(AMBA_DSP_WINDOW_s));
            dsp_osal_print_err_line(Rval, __func__, __LINE__);
            DspLvViewZoneCfg[i].MainWidth = pCfgViewZone->MainWidth;
            DspLvViewZoneCfg[i].MainHeight = pCfgViewZone->MainHeight;
            DspLvViewZoneCfg[i].RotateFlip = pCfgViewZone->RotateFlip;
            DspLvViewZoneCfg[i].HdrExposureNum = pCfgViewZone->HdrExposureNum;
            DspLvViewZoneCfg[i].HdrBlendHieght = pCfgViewZone->HdrBlendHieght;
            DspLvViewZoneCfg[i].MctfDisable = pCfgViewZone->MctfDisable;
            DspLvViewZoneCfg[i].MctsDisable = pCfgViewZone->MctsDisable;
            DspLvViewZoneCfg[i].LinearCE = pCfgViewZone->LinearCE;

#ifdef CONFIG_SOC_H32
            // No Pymd in H32
            DspLvViewZoneCfg[i].pPyramidBuf = NULL;
#else
            Rval = dsp_osal_memcpy(&DspLvViewZoneCfg[i].Pyramid, &pCfgViewZone->Pyramid, sizeof(AMBA_DSP_LIVEVIEW_PYRAMID_s));
            dsp_osal_print_err_line(Rval, __func__, __LINE__);
            {
                DspLvViewZoneCfg[i].pPyramidBuf = &PyramidBuf[i];
                DspLvViewZoneCfg[i].pPyramidBuf->AllocType = pCfgViewZone->PyramidBuf.AllocType;
                DspLvViewZoneCfg[i].pPyramidBuf->BufNum = pCfgViewZone->PyramidBuf.BufNum;
                Rval = dsp_osal_memcpy(&DspLvViewZoneCfg[i].pPyramidBuf->YuvBuf, &pCfgViewZone->PyramidBuf.YuvBuf, sizeof(AMBA_DSP_YUV_IMG_BUF_s));
                dsp_osal_print_err_line(Rval, __func__, __LINE__);

                for (j = 0U; j < pCfgViewZone->PyramidBuf.BufNum; j++) {
                    Rval = dsp_osal_phys2virt((pCfgViewZone->PyramidBuf.YuvBufTbl[j]), &ULAddr);
                    dsp_osal_print_err_line(Rval, __func__, __LINE__);
                    ExtPyramidBufTbl[i][j] = ULAddr;
                }
                DspLvViewZoneCfg[i].pPyramidBuf->pYuvBufTbl = ExtPyramidBufTbl[i];
                Rval = dsp_osal_phys2virt((DspLvViewZoneCfg[i].pPyramidBuf->YuvBuf.BaseAddrY), &ULAddr);
                dsp_osal_print_err_line(Rval, __func__, __LINE__);
                DspLvViewZoneCfg[i].pPyramidBuf->YuvBuf.BaseAddrY = ULAddr;
            }
#endif
#ifdef CONFIG_SOC_H32
            // No Lndt in H32
            DspLvViewZoneCfg[i].pLaneDetectBuf = NULL;
#else
            Rval = dsp_osal_memcpy(&DspLvViewZoneCfg[i].LaneDetect, &pCfgViewZone->LaneDetect, sizeof(AMBA_DSP_LIVEVIEW_LANE_DETECT_s));
            dsp_osal_print_err_line(Rval, __func__, __LINE__);
            {
                DspLvViewZoneCfg[i].pLaneDetectBuf = &LaneDetectBuf[i];
                DspLvViewZoneCfg[i].pLaneDetectBuf->AllocType = pCfgViewZone->LaneDetectBuf.AllocType;
                DspLvViewZoneCfg[i].pLaneDetectBuf->BufNum = pCfgViewZone->LaneDetectBuf.BufNum;
                Rval = dsp_osal_memcpy(&DspLvViewZoneCfg[i].pLaneDetectBuf->YuvBuf, &pCfgViewZone->LaneDetectBuf.YuvBuf, sizeof(AMBA_DSP_YUV_IMG_BUF_s));
                dsp_osal_print_err_line(Rval, __func__, __LINE__);

                for (j = 0U; j < pCfgViewZone->LaneDetectBuf.BufNum; j++) {
                    Rval = dsp_osal_phys2virt((pCfgViewZone->LaneDetectBuf.YuvBufTbl[j]), &ULAddr);
                    dsp_osal_print_err_line(Rval, __func__, __LINE__);
                    ExtLaneDetectBuf[i][j] = ULAddr;
                }
                DspLvViewZoneCfg[i].pLaneDetectBuf->pYuvBufTbl = ExtLaneDetectBuf[i];
                Rval = dsp_osal_phys2virt((DspLvViewZoneCfg[i].pLaneDetectBuf->YuvBuf.BaseAddrY), &ULAddr);
                dsp_osal_print_err_line(Rval, __func__, __LINE__);
                DspLvViewZoneCfg[i].pLaneDetectBuf->YuvBuf.BaseAddrY = ULAddr;
            }
#endif
            DspLvViewZoneCfg[i].VinDecimation = pCfgViewZone->VinDecimation;
            DspLvViewZoneCfg[i].DramUtilityPipe = pCfgViewZone->DramUtilityPipe;
            DspLvViewZoneCfg[i].InputMuxSelCfg = pCfgViewZone->InputMuxSelCfg;

#if defined (CONFIG_SOC_CV2FS) || defined (CONFIG_SOC_CV22FS) || defined (CONFIG_SOC_CV5) || defined (CONFIG_SOC_CV52)
            {
                if (pCfgViewZone->MainY12Buf.BufNum > 0U) {
                    DspLvViewZoneCfg[i].pMainY12Buf = &MainY12Buf[i];
                    DspLvViewZoneCfg[i].pMainY12Buf->AllocType = pCfgViewZone->MainY12Buf.AllocType;
                    DspLvViewZoneCfg[i].pMainY12Buf->BufNum = pCfgViewZone->MainY12Buf.BufNum;
                    Rval = dsp_osal_memcpy(&DspLvViewZoneCfg[i].pMainY12Buf->YuvBuf, &pCfgViewZone->MainY12Buf.YuvBuf, sizeof(AMBA_DSP_YUV_IMG_BUF_s));
                    dsp_osal_print_err_line(Rval, __func__, __LINE__);

                    for (j = 0U; j < pCfgViewZone->MainY12Buf.BufNum; j++) {
                        Rval = dsp_osal_phys2virt((pCfgViewZone->MainY12Buf.YuvBufTbl[j]), &ULAddr);
                        dsp_osal_print_err_line(Rval, __func__, __LINE__);
                        ExtMainY12BufTbl[i][j] = ULAddr;
                    }
                    DspLvViewZoneCfg[i].pMainY12Buf->pYuvBufTbl = ExtMainY12BufTbl[i];
                    Rval = dsp_osal_phys2virt((DspLvViewZoneCfg[i].pMainY12Buf->YuvBuf.BaseAddrY), &ULAddr);
                    dsp_osal_print_err_line(Rval, __func__, __LINE__);
                    DspLvViewZoneCfg[i].pMainY12Buf->YuvBuf.BaseAddrY = ULAddr;
                } else {
                    DspLvViewZoneCfg[i].pMainY12Buf = NULL;
                }
            }
#endif
#if defined (CONFIG_SOC_CV5) || defined (CONFIG_SOC_CV52)
            {
                if (pCfgViewZone->MainIrBuf.BufNum > 0U) {
                    DspLvViewZoneCfg[i].pMainIrBuf = &MainIrBuf[i];
                    DspLvViewZoneCfg[i].pMainIrBuf->AllocType = pCfgViewZone->MainIrBuf.AllocType;
                    DspLvViewZoneCfg[i].pMainIrBuf->BufNum = pCfgViewZone->MainIrBuf.BufNum;
                    Rval = dsp_osal_memcpy(&DspLvViewZoneCfg[i].pMainIrBuf->YuvBuf, &pCfgViewZone->MainIrBuf.YuvBuf, sizeof(AMBA_DSP_YUV_IMG_BUF_s));
                    dsp_osal_print_err_line(Rval, __func__, __LINE__);

                    for (j = 0U; j < pCfgViewZone->MainIrBuf.BufNum; j++) {
                        Rval = dsp_osal_phys2virt((pCfgViewZone->MainIrBuf.YuvBufTbl[j]), &ULAddr);
                        dsp_osal_print_err_line(Rval, __func__, __LINE__);
                        ExtMainIrBufTbl[i][j] = ULAddr;
                    }
                    DspLvViewZoneCfg[i].pMainIrBuf->pYuvBufTbl = ExtMainIrBufTbl[i];
                    Rval = dsp_osal_phys2virt((DspLvViewZoneCfg[i].pMainIrBuf->YuvBuf.BaseAddrY), &ULAddr);
                    dsp_osal_print_err_line(Rval, __func__, __LINE__);
                    DspLvViewZoneCfg[i].pMainIrBuf->YuvBuf.BaseAddrY = ULAddr;
                } else {
                    DspLvViewZoneCfg[i].pMainIrBuf = NULL;
                }
                DspLvViewZoneCfg[i].ExtraHorWarp = pCfgViewZone->ExtraHorWarp;
            }
#endif
#if !defined (CONFIG_SOC_CV2FS) && !defined (CONFIG_SOC_CV22FS) && !defined (CONFIG_SOC_CV5) && !defined (CONFIG_SOC_CV52)
            DspLvViewZoneCfg[i].UpstreamSource = pCfgViewZone->UpstreamSource;
            {
                DspLvViewZoneCfg[i].pIntMainBuf = &IntMainBuf[i];
                DspLvViewZoneCfg[i].pIntMainBuf->AllocType = pCfgViewZone->IntMainBuf.AllocType;
                DspLvViewZoneCfg[i].pIntMainBuf->BufNum = pCfgViewZone->IntMainBuf.BufNum;
                Rval = dsp_osal_memcpy(&DspLvViewZoneCfg[i].pIntMainBuf->YuvBuf, &pCfgViewZone->IntMainBuf.YuvBuf, sizeof(AMBA_DSP_YUV_IMG_BUF_s));
                dsp_osal_print_err_line(Rval, __func__, __LINE__);

                for (j = 0U; j < pCfgViewZone->IntMainBuf.BufNum; j++) {
                    Rval = dsp_osal_phys2virt((pCfgViewZone->IntMainBuf.YuvBufTbl[j]), &ULAddr);
                    dsp_osal_print_err_line(Rval, __func__, __LINE__);
                    ExtIntMainBufTbl[i][j] = ULAddr;
                }
                DspLvViewZoneCfg[i].pIntMainBuf->pYuvBufTbl = ExtIntMainBufTbl[i];
                Rval = dsp_osal_phys2virt((DspLvViewZoneCfg[i].pIntMainBuf->YuvBuf.BaseAddrY), &ULAddr);
                dsp_osal_print_err_line(Rval, __func__, __LINE__);
                DspLvViewZoneCfg[i].pIntMainBuf->YuvBuf.BaseAddrY = ULAddr;
            }
#endif

            ViewZoneBit |= ((UINT32)1U << i);
            if (ViewZoneBit == ViewZonePattern) {
                CfgBit |= (1U << DSP_LV_CFG_TYPE_VIEWZONE);
            }

        } else {
            if (pDspLiveviewCfg->FreshNew == 1U) {
                Rval = dsp_osal_memset(DspLvStrmCfg, 0, sizeof(AMBA_DSP_LIVEVIEW_STREAM_CFG_s)*AMBA_DSP_MAX_YUVSTRM_NUM);
                dsp_osal_print_err_line(Rval, __func__, __LINE__);
                Rval = dsp_osal_memset(ChanCfg, 0, (sizeof(AMBA_DSP_LIVEVIEW_CHANNEL_WINDOW_s)*AMBA_DSP_MAX_YUVSTRM_NUM)*AMBA_DSP_MAX_YUVSTRM_VIEW_NUM);
                dsp_osal_print_err_line(Rval, __func__, __LINE__);
                Rval = dsp_osal_memset(BldBuf, 0, ((sizeof(AMBA_DSP_BUF_s)*AMBA_DSP_MAX_YUVSTRM_NUM)*AMBA_DSP_MAX_YUVSTRM_VIEW_NUM)*AMBA_DSP_MAX_YUVSTRM_BLD_NUM);
                dsp_osal_print_err_line(Rval, __func__, __LINE__);
                Rval = dsp_osal_memset(MaxChanWin, 0, (sizeof(AMBA_DSP_WINDOW_DIMENSION_s)*AMBA_DSP_MAX_YUVSTRM_NUM)*AMBA_DSP_MAX_YUVSTRM_VIEW_NUM);
                dsp_osal_print_err_line(Rval, __func__, __LINE__);
                YuvStrmPattern = ((UINT32)1U << pDspLiveviewCfg->TotalNum) - 1U;
                YuvStrmBit = 0U;
                YuvStrmNum = pDspLiveviewCfg->TotalNum;
            }

            i = pDspLiveviewCfg->Index;

            dsp_osal_typecast(&pCfgYuvStrm, &pU8Val);
            DspLvStrmCfg[i].StreamId = pCfgYuvStrm->StreamId;
            DspLvStrmCfg[i].Purpose = pCfgYuvStrm->Purpose;
            DspLvStrmCfg[i].DestVout = pCfgYuvStrm->DestVout;
            DspLvStrmCfg[i].DestEnc = pCfgYuvStrm->DestEnc;
            DspLvStrmCfg[i].Width = pCfgYuvStrm->Width;
            DspLvStrmCfg[i].Height = pCfgYuvStrm->Height;
            DspLvStrmCfg[i].MaxWidth = pCfgYuvStrm->MaxWidth;
            DspLvStrmCfg[i].MaxHeight = pCfgYuvStrm->MaxHeight;
            {
                DspLvStrmCfg[i].StreamBuf.AllocType = pCfgYuvStrm->StreamBuf.AllocType;
                DspLvStrmCfg[i].StreamBuf.BufNum = pCfgYuvStrm->StreamBuf.BufNum;
                Rval = dsp_osal_memcpy(&DspLvStrmCfg[i].StreamBuf.YuvBuf, &pCfgYuvStrm->StreamBuf.YuvBuf, sizeof(AMBA_DSP_YUV_IMG_BUF_s));
                dsp_osal_print_err_line(Rval, __func__, __LINE__);
                DspLvStrmCfg[i].StreamBuf.pYuvBufTbl = &pCfgYuvStrm->StreamBuf.YuvBufTbl[0U];
            }

            DspLvStrmCfg[i].NumChan = pCfgYuvStrm->NumChan;
            {
                uint64_t *pUlPtr = NULL;
                AMBA_DSP_BUF_s *pBldBuf = NULL;
                uint64_t virt_addr;

                /* copy ChanCfg */
                DspLvStrmCfg[i].pChanCfg = &ChanCfg[i][0U];
                for (j = 0U; j < AMBA_DSP_MAX_YUVSTRM_VIEW_NUM; j++) {
                    ChanCfg[i][j].ViewZoneId = pCfgYuvStrm->ChanCfg[j].ViewZoneId;
                    Rval = dsp_osal_memcpy(&ChanCfg[i][j].ROI, &pCfgYuvStrm->ChanCfg[j].ROI, sizeof(AMBA_DSP_WINDOW_s));
                    dsp_osal_print_err_line(Rval, __func__, __LINE__);
                    Rval = dsp_osal_memcpy(&ChanCfg[i][j].Window, &pCfgYuvStrm->ChanCfg[j].Window, sizeof(AMBA_DSP_WINDOW_s));
                    dsp_osal_print_err_line(Rval, __func__, __LINE__);
                    ChanCfg[i][j].RotateFlip = pCfgYuvStrm->ChanCfg[j].RotateFlip;
                    ChanCfg[i][j].BlendNum = pCfgYuvStrm->ChanCfg[j].BlendNum;

                    if (ChanCfg[i][j].BlendNum > 0U) {
                        pBldBuf = &BldBuf[i][j][0U];
                        Rval = dsp_osal_memcpy(&pUlPtr, &pBldBuf, sizeof(uint64_t *));
                        dsp_osal_print_err_line(Rval, __func__, __LINE__);
                        dsp_osal_typecast(&ChanCfg[i][j].LumaAlphaTable, &pUlPtr);
                        Rval = dsp_osal_memcpy(&BldBuf[i][j][0U], &pCfgYuvStrm->ChanCfg[j].BldBuf[0U], sizeof(AMBA_DSP_BUF_s)*(pCfgYuvStrm->ChanCfg[j].BlendNum));
                        dsp_osal_print_err_line(Rval, __func__, __LINE__);

                        /* phys2virt */
                        for (k = 0U; k < ChanCfg[i][j].BlendNum; k++) {
                            if(BldBuf[i][j][k].BaseAddr != 0U) {
                                Rval = dsp_osal_phys2virt(BldBuf[i][j][k].BaseAddr, (ULONG *)&virt_addr);
                                dsp_osal_print_err_line(Rval, __func__, __LINE__);
                                BldBuf[i][j][k].BaseAddr = virt_addr;
                            }
                        }
                    } else {
                        ChanCfg[i][j].LumaAlphaTable = 0U;
                    }
                }
            }

            DspLvStrmCfg[i].MaxChanBitMask = pCfgYuvStrm->MaxChanBitMask;
            DspLvStrmCfg[i].MaxChanNum = pCfgYuvStrm->MaxChanNum;

            DspLvStrmCfg[i].pMaxChanWin = &MaxChanWin[i][0U];
            Rval = dsp_osal_memcpy(DspLvStrmCfg[i].pMaxChanWin, &pCfgYuvStrm->MaxChanWin[0U], sizeof(AMBA_DSP_WINDOW_DIMENSION_s)*AMBA_DSP_MAX_YUVSTRM_VIEW_NUM);
            dsp_osal_print_err_line(Rval, __func__, __LINE__);
            DspLvStrmCfg[i].DestDeciRate = pCfgYuvStrm->DestDeciRate;
#if !defined (CONFIG_SOC_CV2FS) && !defined (CONFIG_SOC_CV22FS)
            DspLvStrmCfg[i].OptimizeOption = pCfgYuvStrm->OptimizeOption;
#endif

            YuvStrmBit |= ((UINT32)1U << i);
            if (YuvStrmBit == YuvStrmPattern) {
                CfgBit |= (1U << DSP_LV_CFG_TYPE_YUVSTRM);
            }
        }

        if (CfgBit == pDspLiveviewCfg->CfgMask) {
            CfgBit = 0U;
            Rval = dsp_liveview_cfg(ViewZoneNum,
                                    DspLvViewZoneCfg,
                                    YuvStrmNum,
                                    DspLvStrmCfg);
        }
    }
        break;

    case AMBADSP_LIVEVIEW_CTRL:
        dsp_osal_typecast(&pDspLiveviewCtrl, &arg);
        Rval = dsp_liveview_ctrl(pDspLiveviewCtrl->NumViewZone,
                                 pDspLiveviewCtrl->ViewZoneId,
                                 pDspLiveviewCtrl->Enable);
        break;
    case AMBADSP_LIVEVIEW_UPDATE_CFG:
    {
        static AMBA_DSP_LIVEVIEW_STREAM_CFG_s DspLvStrmCfg[AMBA_DSP_MAX_YUVSTRM_NUM] = {0};
        static AMBA_DSP_LIVEVIEW_CHANNEL_WINDOW_s ChanCfg[AMBA_DSP_MAX_YUVSTRM_NUM][AMBA_DSP_MAX_YUVSTRM_VIEW_NUM];
        static AMBA_DSP_BUF_s BldBuf[AMBA_DSP_MAX_YUVSTRM_NUM][AMBA_DSP_MAX_YUVSTRM_VIEW_NUM][AMBA_DSP_MAX_YUVSTRM_BLD_NUM];
        static AMBA_DSP_WINDOW_DIMENSION_s MaxChanWin[AMBA_DSP_MAX_YUVSTRM_NUM][AMBA_DSP_MAX_YUVSTRM_VIEW_NUM] = {0};
        static uint32_t YuvStrmPattern = 0U;
        static uint32_t YuvStrmBit = 0U;
        static uint16_t YuvStrmNum = 0U;

        dsp_osal_typecast(&pDspLiveviewUpdateCfg, &arg);
        if (pDspLiveviewUpdateCfg->FreshNew == 1U) {
            Rval = dsp_osal_memset(DspLvStrmCfg, 0, sizeof(AMBA_DSP_LIVEVIEW_STREAM_CFG_s)*AMBA_DSP_MAX_YUVSTRM_NUM);
            dsp_osal_print_err_line(Rval, __func__, __LINE__);
            Rval = dsp_osal_memset(ChanCfg, 0, (sizeof(AMBA_DSP_LIVEVIEW_CHANNEL_WINDOW_s)*AMBA_DSP_MAX_YUVSTRM_NUM)*AMBA_DSP_MAX_YUVSTRM_VIEW_NUM);
            dsp_osal_print_err_line(Rval, __func__, __LINE__);
            Rval = dsp_osal_memset(BldBuf, 0, ((sizeof(AMBA_DSP_BUF_s)*AMBA_DSP_MAX_YUVSTRM_NUM)*AMBA_DSP_MAX_YUVSTRM_VIEW_NUM)*AMBA_DSP_MAX_YUVSTRM_BLD_NUM);
            dsp_osal_print_err_line(Rval, __func__, __LINE__);
            Rval = dsp_osal_memset(MaxChanWin, 0, (sizeof(AMBA_DSP_WINDOW_DIMENSION_s)*AMBA_DSP_MAX_YUVSTRM_NUM)*AMBA_DSP_MAX_YUVSTRM_VIEW_NUM);
            dsp_osal_print_err_line(Rval, __func__, __LINE__);
            YuvStrmPattern = ((UINT32)1U << pDspLiveviewUpdateCfg->TotalNum) - 1U;
            YuvStrmBit = 0U;
            YuvStrmNum = pDspLiveviewUpdateCfg->TotalNum;
        }

        i = pDspLiveviewUpdateCfg->Index;

        DspLvStrmCfg[i].StreamId = pDspLiveviewUpdateCfg->YuvStrmCfg.StreamId;
        DspLvStrmCfg[i].Purpose = pDspLiveviewUpdateCfg->YuvStrmCfg.Purpose;
        DspLvStrmCfg[i].DestVout = pDspLiveviewUpdateCfg->YuvStrmCfg.DestVout;
        DspLvStrmCfg[i].DestEnc = pDspLiveviewUpdateCfg->YuvStrmCfg.DestEnc;
        DspLvStrmCfg[i].Width = pDspLiveviewUpdateCfg->YuvStrmCfg.Width;
        DspLvStrmCfg[i].Height = pDspLiveviewUpdateCfg->YuvStrmCfg.Height;
        DspLvStrmCfg[i].MaxWidth = pDspLiveviewUpdateCfg->YuvStrmCfg.MaxWidth;
        DspLvStrmCfg[i].MaxHeight = pDspLiveviewUpdateCfg->YuvStrmCfg.MaxHeight;
        {
            DspLvStrmCfg[i].StreamBuf.AllocType = pDspLiveviewUpdateCfg->YuvStrmCfg.StreamBuf.AllocType;
            DspLvStrmCfg[i].StreamBuf.BufNum = pDspLiveviewUpdateCfg->YuvStrmCfg.StreamBuf.BufNum;
            Rval = dsp_osal_memcpy(&DspLvStrmCfg[i].StreamBuf.YuvBuf, &pDspLiveviewUpdateCfg->YuvStrmCfg.StreamBuf.YuvBuf, sizeof(AMBA_DSP_YUV_IMG_BUF_s));
            dsp_osal_print_err_line(Rval, __func__, __LINE__);
            DspLvStrmCfg[i].StreamBuf.pYuvBufTbl = pDspLiveviewUpdateCfg->YuvStrmCfg.StreamBuf.YuvBufTbl;
        }
        DspLvStrmCfg[i].NumChan = pDspLiveviewUpdateCfg->YuvStrmCfg.NumChan;
        {
            uint64_t *pUlPtr = NULL;
            AMBA_DSP_BUF_s *pBldBuf = NULL;
            uint64_t virt_addr;

            /* copy ChanCfg */
            DspLvStrmCfg[i].pChanCfg = &ChanCfg[i][0U];
            for (j = 0U; j < AMBA_DSP_MAX_YUVSTRM_VIEW_NUM; j++) {
                ChanCfg[i][j].ViewZoneId = pDspLiveviewUpdateCfg->YuvStrmCfg.ChanCfg[j].ViewZoneId;
                Rval = dsp_osal_memcpy(&ChanCfg[i][j].ROI, &pDspLiveviewUpdateCfg->YuvStrmCfg.ChanCfg[j].ROI, sizeof(AMBA_DSP_WINDOW_s));
                dsp_osal_print_err_line(Rval, __func__, __LINE__);
                Rval = dsp_osal_memcpy(&ChanCfg[i][j].Window, &pDspLiveviewUpdateCfg->YuvStrmCfg.ChanCfg[j].Window, sizeof(AMBA_DSP_WINDOW_s));
                dsp_osal_print_err_line(Rval, __func__, __LINE__);
                ChanCfg[i][j].RotateFlip = pDspLiveviewUpdateCfg->YuvStrmCfg.ChanCfg[j].RotateFlip;
                ChanCfg[i][j].BlendNum = pDspLiveviewUpdateCfg->YuvStrmCfg.ChanCfg[j].BlendNum;

                if (ChanCfg[i][j].BlendNum > 0U) {
                    pBldBuf = &BldBuf[i][j][0U];
                    Rval = dsp_osal_memcpy(&pUlPtr, &pBldBuf, sizeof(uint64_t *));
                    dsp_osal_print_err_line(Rval, __func__, __LINE__);
                    dsp_osal_typecast(&ChanCfg[i][j].LumaAlphaTable, &pUlPtr);
                    Rval = dsp_osal_memcpy(&BldBuf[i][j][0U], &pDspLiveviewUpdateCfg->YuvStrmCfg.ChanCfg[j].BldBuf[0U], sizeof(AMBA_DSP_BUF_s)*(pDspLiveviewUpdateCfg->YuvStrmCfg.ChanCfg[j].BlendNum));
                    dsp_osal_print_err_line(Rval, __func__, __LINE__);

                    /* phys2virt */
                    for (k = 0U; k < ChanCfg[i][j].BlendNum; k++) {
                        if(BldBuf[i][j][k].BaseAddr != 0U) {
                            Rval = dsp_osal_phys2virt(BldBuf[i][j][k].BaseAddr, (ULONG *)&virt_addr);
                            dsp_osal_print_err_line(Rval, __func__, __LINE__);
                            BldBuf[i][j][k].BaseAddr = virt_addr;
                        }
                    }
                } else {
                    ChanCfg[i][j].LumaAlphaTable = 0U;
                }
            }
        }

        DspLvStrmCfg[i].MaxChanBitMask = pDspLiveviewUpdateCfg->YuvStrmCfg.MaxChanBitMask;
        DspLvStrmCfg[i].MaxChanNum = pDspLiveviewUpdateCfg->YuvStrmCfg.MaxChanNum;
        DspLvStrmCfg[i].pMaxChanWin = &MaxChanWin[i][0U];
        if (DspLvStrmCfg[i].MaxChanNum > 0U) {
            Rval = dsp_osal_memcpy(DspLvStrmCfg[i].pMaxChanWin, &pDspLiveviewUpdateCfg->YuvStrmCfg.MaxChanWin[0U], sizeof(AMBA_DSP_WINDOW_DIMENSION_s)*AMBA_DSP_MAX_YUVSTRM_VIEW_NUM);
            dsp_osal_print_err_line(Rval, __func__, __LINE__);
        }
        DspLvStrmCfg[i].DestDeciRate = pDspLiveviewUpdateCfg->YuvStrmCfg.DestDeciRate;
#if !defined (CONFIG_SOC_CV2FS) && !defined (CONFIG_SOC_CV22FS)
        DspLvStrmCfg[i].OptimizeOption = pDspLiveviewUpdateCfg->YuvStrmCfg.OptimizeOption;
#endif

        YuvStrmBit |= ((UINT32)1U << i);
        if (YuvStrmBit == YuvStrmPattern) {
            Rval = dsp_liveview_update_cfg(YuvStrmNum,
                                           DspLvStrmCfg,
                                           &pDspLiveviewUpdateCfg->AttachedRawSeq);
        }
    }
        break;
    case AMBADSP_LIVEVIEW_UPDATE_ISOCFG:
    {
        dsp_osal_typecast(&pDspLiveviewUpdateIsocfg, &arg);

        /* convert to VirtAddr */
        for (i = 0U; i < pDspLiveviewUpdateIsocfg->NumViewZone; i++) {
            Rval = dsp_osal_phys2virt((pDspLiveviewUpdateIsocfg->IsoCfgCtrl[i].CfgAddress), &ULAddr);
            dsp_osal_print_err_line(Rval, __func__, __LINE__);
            pDspLiveviewUpdateIsocfg->IsoCfgCtrl[i].CfgAddress = ULAddr;
        }

        Rval = dsp_liveview_update_isocfg(pDspLiveviewUpdateIsocfg->NumViewZone,
                                          pDspLiveviewUpdateIsocfg->IsoCfgCtrl,
                                          &pDspLiveviewUpdateIsocfg->AttachedRawSeq);
    }
        break;
    case AMBADSP_LIVEVIEW_UPDATE_VINCFG:
    {
        AMBA_DSP_VIN_SUB_CHAN_s DspLvVinSubChan[AMBA_DSP_MAX_VIRT_CHAN_NUM] = {0};
        AMBA_DSP_LIVEVIEW_VINCFG_CTRL_s DspLvVinCfgCtrl[AMBA_DSP_MAX_VIRT_CHAN_NUM] = {0};

        dsp_osal_typecast(&pDspLiveviewUpdateVincfg, &arg);

        for (i = 0U; i < pDspLiveviewUpdateVincfg->SubChNum; i++) {
            Rval = dsp_osal_memcpy(&DspLvVinSubChan[i], &pDspLiveviewUpdateVincfg->SubCh[i], sizeof(AMBA_DSP_VIN_SUB_CHAN_s));
            dsp_osal_print_err_line(Rval, __func__, __LINE__);
            Rval = dsp_osal_memcpy(&DspLvVinCfgCtrl[i], &pDspLiveviewUpdateVincfg->LvVinCfgCtrl[i], sizeof(AMBA_DSP_LIVEVIEW_VINCFG_CTRL_s));
            dsp_osal_print_err_line(Rval, __func__, __LINE__);

            /* convert to VirtAddr */
            Rval = dsp_osal_phys2virt(DspLvVinCfgCtrl[i].VinCompandTableAddr, &ULAddr);
            dsp_osal_print_err_line(Rval, __func__, __LINE__);
            DspLvVinCfgCtrl[i].VinCompandTableAddr = ULAddr;

            Rval = dsp_osal_phys2virt(DspLvVinCfgCtrl[i].VinDeCompandTableAddr, &ULAddr);
            dsp_osal_print_err_line(Rval, __func__, __LINE__);
            DspLvVinCfgCtrl[i].VinDeCompandTableAddr = ULAddr;
        }

        Rval = dsp_liveview_update_vincfg(pDspLiveviewUpdateVincfg->VinId,
                                          pDspLiveviewUpdateVincfg->SubChNum,
                                          DspLvVinSubChan,
                                          DspLvVinCfgCtrl,
                                          &pDspLiveviewUpdateVincfg->AttachedRawSeq);
    }
        break;
    case AMBADSP_LIVEVIEW_UPDATE_PYMDCFG:
    {
        AMBA_DSP_LIVEVIEW_YUV_BUF_s DspLvYuvBuf[AMBA_DSP_MAX_VIEWZONE_NUM] = {0};

        dsp_osal_typecast(&pDspLiveviewUpdatePymdcfg, &arg);
        for (i = 0U; i < pDspLiveviewUpdatePymdcfg->NumViewZone; i++) {
            DspLvYuvBuf[i].AllocType = pDspLiveviewUpdatePymdcfg->PyramidBuf[i].AllocType;
            DspLvYuvBuf[i].BufNum = pDspLiveviewUpdatePymdcfg->PyramidBuf[i].BufNum;
            Rval = dsp_osal_memcpy(&DspLvYuvBuf[i].YuvBuf, &pDspLiveviewUpdatePymdcfg->PyramidBuf[i].YuvBuf, sizeof(AMBA_DSP_YUV_IMG_BUF_s));
            dsp_osal_print_err_line(Rval, __func__, __LINE__);
            DspLvYuvBuf[i].pYuvBufTbl = pDspLiveviewUpdatePymdcfg->PyramidBuf[i].YuvBufTbl;
        }
        Rval = dsp_liveview_update_pymdcfg(pDspLiveviewUpdatePymdcfg->NumViewZone,
                                           pDspLiveviewUpdatePymdcfg->ViewZoneId,
                                           pDspLiveviewUpdatePymdcfg->Pyramid,
                                           DspLvYuvBuf,
                                           &pDspLiveviewUpdatePymdcfg->AttachedRawSeq);
    }
        break;
    case AMBADSP_LIVEVIEW_VIN_CAP_CFG:
    {
        AMBA_DSP_VIN_SUB_CHAN_CFG_s DspVinSubChanCfg[AMBA_DSP_MAX_VIRT_CHAN_NUM] = {0};

        dsp_osal_typecast(&pDspLiveviewVinCapCfg, &arg);
        for (i = 0U; i < pDspLiveviewVinCapCfg->SubChNum; i++) {
            Rval = dsp_osal_memcpy(&DspVinSubChanCfg[i].SubChan, &pDspLiveviewVinCapCfg->SubChCfg[i].SubChan, sizeof(AMBA_DSP_VIN_SUB_CHAN_s));
            dsp_osal_print_err_line(Rval, __func__, __LINE__);
            DspVinSubChanCfg[i].Option = pDspLiveviewVinCapCfg->SubChCfg[i].Option;
            DspVinSubChanCfg[i].ConCatNum = pDspLiveviewVinCapCfg->SubChCfg[i].ConCatNum;
            DspVinSubChanCfg[i].IntcNum = pDspLiveviewVinCapCfg->SubChCfg[i].IntcNum;
            Rval = dsp_osal_memcpy(&DspVinSubChanCfg[i].CaptureWindow, &pDspLiveviewVinCapCfg->SubChCfg[i].CaptureWindow, sizeof(AMBA_DSP_WINDOW_s));
            dsp_osal_print_err_line(Rval, __func__, __LINE__);
            DspVinSubChanCfg[i].TDNum = pDspLiveviewVinCapCfg->SubChCfg[i].TDNum;
            DspVinSubChanCfg[i].pTDFrmNum = pDspLiveviewVinCapCfg->SubChCfg[i].TDFrmNum;
#if !defined (CONFIG_SOC_CV2FS) && !defined (CONFIG_SOC_CV22FS) && !defined (CONFIG_SOC_CV5) && !defined (CONFIG_SOC_CV52)
            DspVinSubChanCfg[i].ProcDeciRate = pDspLiveviewVinCapCfg->SubChCfg[i].ProcDeciRate;
#endif
        }
        Rval = dsp_liveview_cfg_vin_cap(pDspLiveviewVinCapCfg->VinId,
                                        pDspLiveviewVinCapCfg->SubChNum,
                                        DspVinSubChanCfg);
    }
        break;
    case AMBADSP_LIVEVIEW_VIN_POST_CFG:
        dsp_osal_typecast(&pDspLiveviewVinPostCfg, &arg);
        Rval = dsp_liveview_cfg_vin_post(pDspLiveviewVinPostCfg->Type,
                                         pDspLiveviewVinPostCfg->VinId);
        break;
    case AMBADSP_LIVEVIEW_FEED_RAW:
    {
        dsp_osal_typecast(&pDspLiveviewFeedRawData, &arg);

        for (i = 0U; i < pDspLiveviewFeedRawData->NumViewZone; i++) {
            /* convert to VirtAddr */
            Rval = dsp_osal_phys2virt((pDspLiveviewFeedRawData->ExtBuf[i].RawBuffer.BaseAddr), &ULAddr);
            dsp_osal_print_err_line(Rval, __func__, __LINE__);
            pDspLiveviewFeedRawData->ExtBuf[i].RawBuffer.BaseAddr = ULAddr;

            Rval = dsp_osal_phys2virt((pDspLiveviewFeedRawData->ExtBuf[i].CeBuffer.BaseAddr), &ULAddr);
            dsp_osal_print_err_line(Rval, __func__, __LINE__);
            pDspLiveviewFeedRawData->ExtBuf[i].CeBuffer.BaseAddr = ULAddr;
        }

        Rval = dsp_liveview_feed_raw_data(pDspLiveviewFeedRawData->NumViewZone,
                                          pDspLiveviewFeedRawData->ViewZoneId,
                                          pDspLiveviewFeedRawData->ExtBuf);
    }
        break;
    case AMBADSP_LIVEVIEW_FEED_YUV:
    {
        AMBA_DSP_EXT_YUV_BUF_s DspExtYuvBuf[AMBA_DSP_MAX_VIEWZONE_NUM] = {0};

        dsp_osal_typecast(&pDspLiveviewFeedYuvData, &arg);
        for (i = 0U; i < pDspLiveviewFeedYuvData->NumViewZone; i++) {
            Rval = dsp_osal_memcpy(&DspExtYuvBuf[i].ExtYuvBuf, &pDspLiveviewFeedYuvData->ExtBuf[i].ExtYuvBuf, sizeof(AMBA_DSP_YUV_IMG_BUF_s));
            dsp_osal_print_err_line(Rval, __func__, __LINE__);

            /* convert to VirtAddr */
            Rval = dsp_osal_phys2virt((pDspLiveviewFeedYuvData->ExtBuf[i].ExtYuvBuf.BaseAddrY), &ULAddr);
            dsp_osal_print_err_line(Rval, __func__, __LINE__);
            DspExtYuvBuf[i].ExtYuvBuf.BaseAddrY = ULAddr;
            Rval = dsp_osal_phys2virt((pDspLiveviewFeedYuvData->ExtBuf[i].ExtYuvBuf.BaseAddrUV), &ULAddr);
            dsp_osal_print_err_line(Rval, __func__, __LINE__);
            DspExtYuvBuf[i].ExtYuvBuf.BaseAddrUV = ULAddr;

            if (pDspLiveviewFeedYuvData->ExtBuf[i].ExtME1Buf.BaseAddr == 0U) {
                DspExtYuvBuf[i].pExtME1Buf = NULL;
            } else {
                DspExtYuvBuf[i].pExtME1Buf = &pDspLiveviewFeedYuvData->ExtBuf[i].ExtME1Buf;

                /* convert to VirtAddr */
                Rval = dsp_osal_phys2virt((pDspLiveviewFeedYuvData->ExtBuf[i].ExtME1Buf.BaseAddr), &ULAddr);
                dsp_osal_print_err_line(Rval, __func__, __LINE__);
                DspExtYuvBuf[i].pExtME1Buf->BaseAddr = ULAddr;
            }

            if (pDspLiveviewFeedYuvData->ExtBuf[i].ExtME0Buf.BaseAddr == 0U) {
                DspExtYuvBuf[i].pExtME0Buf = NULL;
            } else {
                DspExtYuvBuf[i].pExtME0Buf = &pDspLiveviewFeedYuvData->ExtBuf[i].ExtME0Buf;

                /* convert to VirtAddr */
                Rval = dsp_osal_phys2virt((pDspLiveviewFeedYuvData->ExtBuf[i].ExtME0Buf.BaseAddr), &ULAddr);
                dsp_osal_print_err_line(Rval, __func__, __LINE__);
                DspExtYuvBuf[i].pExtME0Buf->BaseAddr = ULAddr;
            }
            DspExtYuvBuf[i].IsLast = pDspLiveviewFeedYuvData->ExtBuf[i].IsLast;
        }
        Rval = dsp_liveview_feed_yuv_data(pDspLiveviewFeedYuvData->NumViewZone,
                                          pDspLiveviewFeedYuvData->ViewZoneId,
                                          DspExtYuvBuf);
    }
        break;
    case AMBADSP_LIVEVIEW_YUVSTRM_SYNC:
        dsp_osal_typecast(&pDspLiveviewYuvStrmSync, &arg);
        Rval = dsp_liveview_yuvstream_sync(pDspLiveviewYuvStrmSync->YuvStrmIdx,
                                           &pDspLiveviewYuvStrmSync->YuvStrmSyncCtrl,
                                           &pDspLiveviewYuvStrmSync->SyncJobId,
                                           &pDspLiveviewYuvStrmSync->AttachedRawSeq);
        break;
    case AMBADSP_LIVEVIEW_UPDATE_GEOCFG:
    {
        dsp_osal_typecast(&pDspLiveviewUpdateGeocfg, &arg);

        /* convert to VirtAddr */
        Rval = dsp_osal_phys2virt((pDspLiveviewUpdateGeocfg->GeoCfgCtrl.GeoCfgAddr), &ULAddr);
        dsp_osal_print_err_line(Rval, __func__, __LINE__);
        pDspLiveviewUpdateGeocfg->GeoCfgCtrl.GeoCfgAddr = ULAddr;

        Rval = dsp_liveview_update_geocfg(pDspLiveviewUpdateGeocfg->ViewZoneId,
                                          &pDspLiveviewUpdateGeocfg->GeoCfgCtrl,
                                          &pDspLiveviewUpdateGeocfg->AttachedRawSeq);
    }
        break;
    case AMBADSP_LIVEVIEW_UPDATE_VZ_SRC:
        dsp_osal_typecast(&pDspLiveviewUpdateVzSrc, &arg);
        Rval = dsp_liveview_update_vz_src(pDspLiveviewUpdateVzSrc->NumViewZone,
                                          pDspLiveviewUpdateVzSrc->VzSrcCfg,
                                          &pDspLiveviewUpdateVzSrc->AttachedRawSeq);
        break;

    case AMBADSP_LIVEVIEW_UPDATE_SIDEBAND:
    {
        dsp_osal_typecast(&pDspLiveviewUpdateSideband, &arg);

        /* convert to VirtAddr */
        for (i = 0U; i < pDspLiveviewUpdateSideband->NumBand; i++) {
            Rval = dsp_osal_phys2virt((pDspLiveviewUpdateSideband->SidebandBufAddr[i]), &ULAddr);
            dsp_osal_print_err_line(Rval, __func__, __LINE__);
            pDspLiveviewUpdateSideband->SidebandBufAddr[i] = ULAddr;
        }

        Rval = dsp_liveview_sb_update(pDspLiveviewUpdateSideband->ViewZoneId,
                                      pDspLiveviewUpdateSideband->NumBand,
                                      pDspLiveviewUpdateSideband->SidebandBufAddr);
    }
        break;
    case AMBADSP_LIVEVIEW_SLICE_CFG:
        dsp_osal_typecast(&pDspLiveviewSliceCfg, &arg);
        Rval = dsp_liveview_slice_cfg(pDspLiveviewSliceCfg->ViewZoneId,
                                      &pDspLiveviewSliceCfg->LvSliceCfg);
        break;
    case AMBADSP_LIVEVIEW_VZ_POSTPONE_CFG:
        dsp_osal_typecast(&pDspLiveviewVzPostponeCfg, &arg);
        Rval = dsp_liveview_vz_postpone_cfg(pDspLiveviewVzPostponeCfg->ViewZoneId,
                                            &pDspLiveviewVzPostponeCfg->VzPostPoneCfg);
        break;
    case AMBADSP_LIVEVIEW_UPDATE_VIN_STATE:
        dsp_osal_typecast(&pDspLiveviewUpdateVinState, &arg);
        Rval = dsp_liveview_update_vin_state(pDspLiveviewUpdateVinState->NumVin,
                                             pDspLiveviewUpdateVinState->VinState);
        break;
    case AMBADSP_LIVEVIEW_PARSE_VP_MSG:
        dsp_osal_typecast(&pUL, &arg);
        Rval = dsp_liveview_parse_vp_msg(*pUL);
        break;
    case AMBADSP_LIVEVIEW_VIN_DRV_CFG:
        dsp_osal_typecast(&pDspLiveviewVinDrvCfg, &arg);
        Rval = dsp_liveview_vin_drv_cfg(pDspLiveviewVinDrvCfg->VinId,
                                        pDspLiveviewVinDrvCfg->Type,
                                        pDspLiveviewVinDrvCfg->VinDrvCfg.Data);
        break;
    case AMBADSP_LIVEVIEW_IK_DRV_CFG:
    {
        uint8_t *pU8Val = NULL;
        dsp_ik_drv_cfg_iso_t *pCfgIso;

        dsp_osal_typecast(&pDspLiveviewIkDrvCfg, &arg);
        pU8Val = pDspLiveviewIkDrvCfg->IkDrvCfg.Data;
        if (pDspLiveviewIkDrvCfg->Type == DSP_IK_DRV_CFG_TYPE_ISO) {
            dsp_osal_typecast(&pCfgIso, &pU8Val);
            Rval = dsp_osal_phys2virt(pCfgIso->Addr, &ULAddr);
            dsp_osal_print_err_line(Rval, __func__, __LINE__);
            pCfgIso->Addr = ULAddr;
        }

        Rval = dsp_liveview_ik_drv_cfg(pDspLiveviewIkDrvCfg->ViewZoneId,
                                       pDspLiveviewIkDrvCfg->Type,
                                       &pDspLiveviewIkDrvCfg->IkDrvCfg.Data);
    }
        break;
    case AMBADSP_LIVEVIEW_SLICE_CFG_CALC:
    {
        dsp_osal_typecast(&pDspLiveviewSliceCfg, &arg);
        Rval = dsp_liveview_slice_cfg_calc(pDspLiveviewSliceCfg->ViewZoneId,
                                           &pDspLiveviewSliceCfg->LvSliceCfg);
    }
        break;
    case AMBADSP_LIVEVIEW_GET_IDSPCFG:
    {
        dsp_osal_typecast(&pDspLiveviewIdspCfg, &arg);
        Rval = dsp_liveview_get_idsp_cfg(pDspLiveviewIdspCfg->ViewZoneId,
                                         &pDspLiveviewIdspCfg->IdspCfgAddr);
    }
        break;
    case AMBADSP_LIVEVIEW_SLOW_SHUTTER_CTRL:
    {
        dsp_osal_typecast(&pDspLiveviewSlowShutterCtrl, &arg);
        Rval = dsp_liveview_slow_shutter_ctrl(pDspLiveviewSlowShutterCtrl->NumViewZone,
                                              pDspLiveviewSlowShutterCtrl->ViewZoneId,
                                              pDspLiveviewSlowShutterCtrl->Ratio);
    }
        break;
#if defined (CONFIG_SOC_CV5) || defined (CONFIG_SOC_CV52)
    case AMBADSP_LIVEVIEW_DROP_REPEAT_CTRL:
    {
        AMBA_DSP_LIVEVIEW_DROP_REPEAT_s DropRepeat[AMBA_DSP_MAX_VIEWZONE_NUM] = {0};

        dsp_osal_typecast(&pDspLiveviewDropRepeatCtrl, &arg);
        for (i = 0U; i < pDspLiveviewDropRepeatCtrl->NumViewZone; i++) {
            DropRepeat[i].Method = pDspLiveviewDropRepeatCtrl->Method[i];
            DropRepeat[i].Option = pDspLiveviewDropRepeatCtrl->Option[i];
        }
        Rval = dsp_liveview_drop_repeat_ctrl(pDspLiveviewDropRepeatCtrl->NumViewZone,
                                             pDspLiveviewDropRepeatCtrl->ViewZoneId,
                                             DropRepeat);
    }
        break;
#endif
    /******************  VideoEnc  ******************/
    case AMBADSP_VIDEO_ENC_CFG:
    {
        dsp_osal_typecast(&pDspVideoEncCfg, &arg);

        /* convert to VirtAddr */
        for (i = 0U; i < pDspVideoEncCfg->NumStream; i++) {
            Rval = dsp_osal_phys2virt((pDspVideoEncCfg->StreamConfig[i].EncConfig.BitsBufAddr), &ULAddr);
            dsp_osal_print_err_line(Rval, __func__, __LINE__);
            pDspVideoEncCfg->StreamConfig[i].EncConfig.BitsBufAddr = ULAddr;

            Rval = dsp_osal_phys2virt((pDspVideoEncCfg->StreamConfig[i].EncConfig.QuantMatrixAddr), &ULAddr);
            dsp_osal_print_err_line(Rval, __func__, __LINE__);
            pDspVideoEncCfg->StreamConfig[i].EncConfig.QuantMatrixAddr = ULAddr;

            Rval = dsp_osal_phys2virt((pDspVideoEncCfg->StreamConfig[i].EncConfig.JpgBitRateCtrl.RateCurvAddr), &ULAddr);
            dsp_osal_print_err_line(Rval, __func__, __LINE__);
            pDspVideoEncCfg->StreamConfig[i].EncConfig.JpgBitRateCtrl.RateCurvAddr = ULAddr;
        }

        Rval = dsp_video_enc_cfg(pDspVideoEncCfg->NumStream,
                                 pDspVideoEncCfg->StreamIdx,
                                 pDspVideoEncCfg->StreamConfig);
    }
        break;
    case AMBADSP_VIDEO_ENC_START:
        dsp_osal_typecast(&pDspVideoEncStart, &arg);
        Rval = dsp_video_enc_start(pDspVideoEncStart->NumStream,
                                   pDspVideoEncStart->StreamIdx,
                                   pDspVideoEncStart->StartConfig,
                                   &pDspVideoEncStart->AttachedRawSeq);
        break;
    case AMBADSP_VIDEO_ENC_STOP:
        dsp_osal_typecast(&pDspVideoEncStop, &arg);
        Rval = dsp_video_enc_stop(pDspVideoEncStop->NumStream,
                                   pDspVideoEncStop->StreamIdx,
                                   pDspVideoEncStop->StopOption,
                                   &pDspVideoEncStop->AttachedRawSeq);
        break;
    case AMBADSP_VIDEO_ENC_FRMRATE_CTRL:
        dsp_osal_typecast(&pDspVideoEncFrmRateCtrl, &arg);
        Rval = dsp_video_enc_ctrl_framerate(pDspVideoEncFrmRateCtrl->NumStream,
                                            pDspVideoEncFrmRateCtrl->StreamIdx,
                                            pDspVideoEncFrmRateCtrl->Divisor,
                                            &pDspVideoEncFrmRateCtrl->AttachedRawSeq);
        break;
    case AMBADSP_VIDEO_ENC_BLEND_CTRL:
    {
        dsp_osal_typecast(&pDspVideoEncBlendCtrl, &arg);

        /* convert to VirtAddr */
        for (i = 0U; i < pDspVideoEncBlendCtrl->NumStream; i++) {
            Rval = dsp_osal_phys2virt((pDspVideoEncBlendCtrl->BlendCfg[i].BlendYuvBuf.BaseAddrY), &ULAddr);
            dsp_osal_print_err_line(Rval, __func__, __LINE__);
            pDspVideoEncBlendCtrl->BlendCfg[i].BlendYuvBuf.BaseAddrY = ULAddr;

            Rval = dsp_osal_phys2virt((pDspVideoEncBlendCtrl->BlendCfg[i].BlendYuvBuf.BaseAddrUV), &ULAddr);
            dsp_osal_print_err_line(Rval, __func__, __LINE__);
            pDspVideoEncBlendCtrl->BlendCfg[i].BlendYuvBuf.BaseAddrUV = ULAddr;

            Rval = dsp_osal_phys2virt((pDspVideoEncBlendCtrl->BlendCfg[i].AlphaBuf.BaseAddrY), &ULAddr);
            dsp_osal_print_err_line(Rval, __func__, __LINE__);
            pDspVideoEncBlendCtrl->BlendCfg[i].AlphaBuf.BaseAddrY = ULAddr;

            Rval = dsp_osal_phys2virt((pDspVideoEncBlendCtrl->BlendCfg[i].AlphaBuf.BaseAddrUV), &ULAddr);
            dsp_osal_print_err_line(Rval, __func__, __LINE__);
            pDspVideoEncBlendCtrl->BlendCfg[i].AlphaBuf.BaseAddrUV = ULAddr;
        }

        Rval = dsp_video_enc_ctrl_blend(pDspVideoEncBlendCtrl->NumStream,
                                        pDspVideoEncBlendCtrl->StreamIdx,
                                        pDspVideoEncBlendCtrl->BlendCfg);
    }
        break;
    case AMBADSP_VIDEO_ENC_QUALITY_CTRL:
        dsp_osal_typecast(&pDspVideoEncQualityCtrl, &arg);
        Rval = dsp_video_enc_ctrl_quality(pDspVideoEncQualityCtrl->NumStream,
                                          pDspVideoEncQualityCtrl->StreamIdx,
                                          pDspVideoEncQualityCtrl->QCtrl);
        break;
    case AMBADSP_VIDEO_ENC_FEED_YUV:
    {
        AMBA_DSP_EXT_YUV_BUF_s DspExtYuvBuf[AMBA_DSP_MAX_STREAM_NUM] = {0};
        AMBA_DSP_BUF_s ExtME1Buf[AMBA_DSP_MAX_STREAM_NUM] = {0};
        AMBA_DSP_BUF_s ExtME0Buf[AMBA_DSP_MAX_STREAM_NUM] = {0};

        dsp_osal_typecast(&pDspVideoEncFeedYuv, &arg);
        for (i = 0U; i < pDspVideoEncFeedYuv->NumStream; i++) {
            Rval = dsp_osal_memcpy(&DspExtYuvBuf[i].ExtYuvBuf, &pDspVideoEncFeedYuv->ExtBuf[i].ExtYuvBuf, sizeof(AMBA_DSP_YUV_IMG_BUF_s));
            dsp_osal_print_err_line(Rval, __func__, __LINE__);

            DspExtYuvBuf[i].IsLast = pDspVideoEncFeedYuv->ExtBuf[i].IsLast;

            /* convert to VirtAddr */
            Rval = dsp_osal_phys2virt(DspExtYuvBuf[i].ExtYuvBuf.BaseAddrY, &ULAddr);
            dsp_osal_print_err_line(Rval, __func__, __LINE__);
            DspExtYuvBuf[i].ExtYuvBuf.BaseAddrY = ULAddr;

            Rval = dsp_osal_phys2virt(DspExtYuvBuf[i].ExtYuvBuf.BaseAddrUV, &ULAddr);
            dsp_osal_print_err_line(Rval, __func__, __LINE__);
            DspExtYuvBuf[i].ExtYuvBuf.BaseAddrUV = ULAddr;

            if (pDspVideoEncFeedYuv->ExtBuf[i].ExtME1Buf.BaseAddr > 0U) {
                DspExtYuvBuf[i].pExtME1Buf = &ExtME1Buf[i];
                Rval = dsp_osal_memcpy(DspExtYuvBuf[i].pExtME1Buf, &pDspVideoEncFeedYuv->ExtBuf[i].ExtME1Buf, sizeof(AMBA_DSP_BUF_s));
                dsp_osal_print_err_line(Rval, __func__, __LINE__);

                Rval = dsp_osal_phys2virt((DspExtYuvBuf[i].pExtME1Buf->BaseAddr), &ULAddr);
                dsp_osal_print_err_line(Rval, __func__, __LINE__);
                DspExtYuvBuf[i].pExtME1Buf->BaseAddr = ULAddr;
            } else {
                DspExtYuvBuf[i].pExtME1Buf = NULL;
            }

            if (pDspVideoEncFeedYuv->ExtBuf[i].ExtME0Buf.BaseAddr > 0U) {
                DspExtYuvBuf[i].pExtME0Buf = &ExtME0Buf[i];
                Rval = dsp_osal_memcpy(DspExtYuvBuf[i].pExtME0Buf, &pDspVideoEncFeedYuv->ExtBuf[i].ExtME0Buf, sizeof(AMBA_DSP_BUF_s));
                dsp_osal_print_err_line(Rval, __func__, __LINE__);

                Rval = dsp_osal_phys2virt((DspExtYuvBuf[i].pExtME0Buf->BaseAddr), &ULAddr);
                dsp_osal_print_err_line(Rval, __func__, __LINE__);
                DspExtYuvBuf[i].pExtME0Buf->BaseAddr = ULAddr;
            } else {
                DspExtYuvBuf[i].pExtME0Buf = NULL;
            }
        }

        Rval = dsp_video_enc_feed_yuv_data(pDspVideoEncFeedYuv->NumStream,
                                           pDspVideoEncFeedYuv->StreamIdx,
                                           DspExtYuvBuf);
    }
        break;
    case AMBADSP_VIDEO_ENC_GRP_CFG:
    {
        dsp_osal_typecast(&pDspVideoEncGrpCfg, &arg);

        /* convert to VirtAddr */
        Rval = dsp_osal_phys2virt((pDspVideoEncGrpCfg->RcCfgAddr), &ULAddr);
        dsp_osal_print_err_line(Rval, __func__, __LINE__);
        pDspVideoEncGrpCfg->RcCfgAddr = ULAddr;

        Rval = dsp_video_enc_grp_cfg(pDspVideoEncGrpCfg);
    }
        break;
    case AMBADSP_VIDEO_ENC_MV_CFG:
    {
        dsp_osal_typecast(&pDspVideoEncMvCfg, &arg);

        /* convert to VirtAddr */
        Rval = dsp_osal_phys2virt((pDspVideoEncMvCfg->MvCfg.BufAddr), &ULAddr);
        dsp_osal_print_err_line(Rval, __func__, __LINE__);
        pDspVideoEncMvCfg->MvCfg.BufAddr = ULAddr;

        Rval = dsp_video_enc_mv_cfg(pDspVideoEncMvCfg->StreamIdx,
                                    &pDspVideoEncMvCfg->MvCfg);
    }
        break;
    case AMBADSP_VIDEO_ENC_DESC_FMT_CFG:
        dsp_osal_typecast(&pDspVideoEncDescFmtCfg, &arg);
        Rval = dsp_video_enc_desc_fmt_cfg(pDspVideoEncDescFmtCfg->StreamIdx,
                                          pDspVideoEncDescFmtCfg->CatIdx,
                                          pDspVideoEncDescFmtCfg->OptVal);
        break;
    /******************  StillEnc  ******************/
    case AMBADSP_STL_DATA_CAP_CFG:
    {
        AMBA_DSP_DATACAP_CFG_s DataCapCfg = {0};

        dsp_osal_typecast(&pDspStlDataCapCfg, &arg);

        DataCapCfg.CapDataType = pDspStlDataCapCfg->DataCapCfg.CapDataType;
        DataCapCfg.AuxDataNeeded = pDspStlDataCapCfg->DataCapCfg.AuxDataNeeded;
        DataCapCfg.Index = pDspStlDataCapCfg->DataCapCfg.Index;
        DataCapCfg.AllocType = pDspStlDataCapCfg->DataCapCfg.AllocType;
        DataCapCfg.BufNum = pDspStlDataCapCfg->DataCapCfg.BufNum;
        DataCapCfg.OverFlowCtrl = pDspStlDataCapCfg->DataCapCfg.OverFlowCtrl;
        DataCapCfg.CmprRate = pDspStlDataCapCfg->DataCapCfg.CmprRate;
        DataCapCfg.CmptRate = pDspStlDataCapCfg->DataCapCfg.CmptRate;
#if defined (CONFIG_SOC_CV5) || defined (CONFIG_SOC_CV52)
        DataCapCfg.HdrExposureNum = pDspStlDataCapCfg->DataCapCfg.HdrExposureNum;
#endif
        Rval = dsp_osal_memcpy(&DataCapCfg.DataBuf, &pDspStlDataCapCfg->DataCapCfg.DataBuf, sizeof(AMBA_DSP_BUF_s));
        dsp_osal_print_err_line(Rval, __func__, __LINE__);
        Rval = dsp_osal_memcpy(&DataCapCfg.AuxDataBuf, &pDspStlDataCapCfg->DataCapCfg.AuxDataBuf, sizeof(AMBA_DSP_BUF_s));
        dsp_osal_print_err_line(Rval, __func__, __LINE__);
        DataCapCfg.pBufTbl = pDspStlDataCapCfg->DataCapCfg.BufTbl;
        DataCapCfg.pAuxBufTbl = pDspStlDataCapCfg->DataCapCfg.AuxBufTbl;

        /* convert to VirtAddr */
        Rval = dsp_osal_phys2virt(DataCapCfg.DataBuf.BaseAddr, &ULAddr);
        dsp_osal_print_err_line(Rval, __func__, __LINE__);
        DataCapCfg.DataBuf.BaseAddr = ULAddr;

        Rval = dsp_osal_phys2virt(DataCapCfg.AuxDataBuf.BaseAddr, &ULAddr);
        dsp_osal_print_err_line(Rval, __func__, __LINE__);
        DataCapCfg.AuxDataBuf.BaseAddr = ULAddr;

        for (i = 0U; i < pDspStlDataCapCfg->DataCapCfg.BufNum; i++) {
            Rval = dsp_osal_phys2virt((pDspStlDataCapCfg->DataCapCfg.BufTbl[i]), &ULAddr);
            dsp_osal_print_err_line(Rval, __func__, __LINE__);
            pDspStlDataCapCfg->DataCapCfg.BufTbl[i] = ULAddr;

            Rval = dsp_osal_phys2virt((pDspStlDataCapCfg->DataCapCfg.AuxBufTbl[i]), &ULAddr);
            dsp_osal_print_err_line(Rval, __func__, __LINE__);
            pDspStlDataCapCfg->DataCapCfg.AuxBufTbl[i] = ULAddr;
        }

        Rval = dsp_data_cap_cfg(pDspStlDataCapCfg->CapInstance,
                                &DataCapCfg);
    }
        break;
    case AMBADSP_STL_UPDATE_CAP_BUF:
    {
        AMBA_DSP_DATACAP_BUF_CFG_s DspCataCapBufCfg = {0};

        dsp_osal_typecast(&pDspStlUpdateCapBuf, &arg);

        DspCataCapBufCfg.BufNum = pDspStlUpdateCapBuf->CapBuf.BufNum;
        DspCataCapBufCfg.AllocType = pDspStlUpdateCapBuf->CapBuf.AllocType;
        DspCataCapBufCfg.BufAddr = pDspStlUpdateCapBuf->CapBuf.BufAddr;
        DspCataCapBufCfg.AuxBufAddr = pDspStlUpdateCapBuf->CapBuf.AuxBufAddr;
        DspCataCapBufCfg.pBufTbl = pDspStlUpdateCapBuf->CapBuf.BufTbl;
        DspCataCapBufCfg.pAuxBufTbl = pDspStlUpdateCapBuf->CapBuf.AuxBufTbl;

        /* convert to VirtAddr */
        Rval = dsp_osal_phys2virt(DspCataCapBufCfg.BufAddr, &ULAddr);
        dsp_osal_print_err_line(Rval, __func__, __LINE__);
        DspCataCapBufCfg.BufAddr = ULAddr;

        Rval = dsp_osal_phys2virt(DspCataCapBufCfg.AuxBufAddr, &ULAddr);
        dsp_osal_print_err_line(Rval, __func__, __LINE__);
        DspCataCapBufCfg.AuxBufAddr = ULAddr;

        for (i = 0U; i<pDspStlUpdateCapBuf->CapBuf.BufNum; i++) {
            Rval = dsp_osal_phys2virt((pDspStlUpdateCapBuf->CapBuf.BufTbl[i]), &ULAddr);
            dsp_osal_print_err_line(Rval, __func__, __LINE__);
            pDspStlUpdateCapBuf->CapBuf.BufTbl[i] = ULAddr;

            Rval = dsp_osal_phys2virt((pDspStlUpdateCapBuf->CapBuf.AuxBufTbl[i]), &ULAddr);
            dsp_osal_print_err_line(Rval, __func__, __LINE__);
            pDspStlUpdateCapBuf->CapBuf.AuxBufTbl[i] = ULAddr;
        }

        Rval = dsp_update_cap_buffer(pDspStlUpdateCapBuf->CapInstance,
                                     &DspCataCapBufCfg,
                                     &pDspStlUpdateCapBuf->AttachedRawSeq);
    }
        break;
    case AMBADSP_STL_DATA_CAP_CTRL:
        dsp_osal_typecast(&pDspStlDataCapCtrl, &arg);
        Rval = dsp_data_cap_ctrl(pDspStlDataCapCtrl->NumCapInstance,
                                 pDspStlDataCapCtrl->CapInstance,
                                 pDspStlDataCapCtrl->DataCapCtrl,
                                 &pDspStlDataCapCtrl->AttachedRawSeq);
        break;
    case AMBADSP_STL_Y2Y:
    {
        dsp_osal_typecast(&pDspStlY2Y, &arg);

        /* convert to VirtAddr */
        Rval = dsp_osal_phys2virt((pDspStlY2Y->YuvIn.BaseAddrY), &ULAddr);
        dsp_osal_print_err_line(Rval, __func__, __LINE__);
        pDspStlY2Y->YuvIn.BaseAddrY = ULAddr;

        Rval = dsp_osal_phys2virt((pDspStlY2Y->YuvIn.BaseAddrUV), &ULAddr);
        dsp_osal_print_err_line(Rval, __func__, __LINE__);
        pDspStlY2Y->YuvIn.BaseAddrUV = ULAddr;

        Rval = dsp_osal_phys2virt((pDspStlY2Y->YuvOut.BaseAddrY), &ULAddr);
        dsp_osal_print_err_line(Rval, __func__, __LINE__);
        pDspStlY2Y->YuvOut.BaseAddrY = ULAddr;

        Rval = dsp_osal_phys2virt((pDspStlY2Y->YuvOut.BaseAddrUV), &ULAddr);
        dsp_osal_print_err_line(Rval, __func__, __LINE__);
        pDspStlY2Y->YuvOut.BaseAddrUV = ULAddr;

        Rval = dsp_osal_phys2virt((pDspStlY2Y->IsoCfg.CfgAddress), &ULAddr);
        dsp_osal_print_err_line(Rval, __func__, __LINE__);
        pDspStlY2Y->IsoCfg.CfgAddress = ULAddr;

        Rval = dsp_still_yuv2yuv(&pDspStlY2Y->YuvIn,
                                 &pDspStlY2Y->YuvOut,
                                 &pDspStlY2Y->IsoCfg,
                                 pDspStlY2Y->Opt,
                                 &pDspStlY2Y->AttachedRawSeq);
    }
        break;
    case AMBADSP_STL_ENC_CTRL:
    {
        dsp_osal_typecast(&pDspStlEncCtrl, &arg);

        /* convert to VirtAddr */
        Rval = dsp_osal_phys2virt(pDspStlEncCtrl->StlEncCtrl.QTblAddr, &ULAddr);
        dsp_osal_print_err_line(Rval, __func__, __LINE__);
        pDspStlEncCtrl->StlEncCtrl.QTblAddr = ULAddr;

        Rval = dsp_osal_phys2virt(pDspStlEncCtrl->StlEncCtrl.RateCurvAddr, &ULAddr);
        dsp_osal_print_err_line(Rval, __func__, __LINE__);
        pDspStlEncCtrl->StlEncCtrl.RateCurvAddr = ULAddr;

        Rval = dsp_osal_phys2virt(pDspStlEncCtrl->StlEncCtrl.BitsBufAddr, &ULAddr);
        dsp_osal_print_err_line(Rval, __func__, __LINE__);
        pDspStlEncCtrl->StlEncCtrl.BitsBufAddr = ULAddr;

        Rval = dsp_osal_phys2virt(pDspStlEncCtrl->StlEncCtrl.YuvBuf.BaseAddrY, &ULAddr);
        dsp_osal_print_err_line(Rval, __func__, __LINE__);
        pDspStlEncCtrl->StlEncCtrl.YuvBuf.BaseAddrY = ULAddr;

        Rval = dsp_osal_phys2virt(pDspStlEncCtrl->StlEncCtrl.YuvBuf.BaseAddrUV, &ULAddr);
        dsp_osal_print_err_line(Rval, __func__, __LINE__);
        pDspStlEncCtrl->StlEncCtrl.YuvBuf.BaseAddrUV = ULAddr;

        Rval = dsp_still_encode_ctrl(pDspStlEncCtrl->StreamIdx,
                                     &pDspStlEncCtrl->StlEncCtrl,
                                     &pDspStlEncCtrl->AttachedRawSeq);
    }
        break;
    case AMBADSP_STL_YUV_EXTBUF_CACL:
    {
        dsp_osal_typecast(&pDspStlYuvExtBufCalc, &arg);
        Rval = dsp_calc_still_yuv_extbuf_size(pDspStlYuvExtBufCalc->StreamIdx,
                                              pDspStlYuvExtBufCalc->BufType,
                                              &pDspStlYuvExtBufCalc->BufPitch,
                                              &pDspStlYuvExtBufCalc->BufUnitSize);
    }
        break;
    case AMBADSP_STL_R2Y:
    {
        dsp_osal_typecast(&pDspStlR2Y, &arg);

        /* convert to VirtAddr */
        Rval = dsp_osal_phys2virt(pDspStlR2Y->RawIn.BaseAddr, &ULAddr);
        dsp_osal_print_err_line(Rval, __func__, __LINE__);
        pDspStlR2Y->RawIn.BaseAddr = ULAddr;

        Rval = dsp_osal_phys2virt(pDspStlR2Y->AuxBufIn.BaseAddr, &ULAddr);
        dsp_osal_print_err_line(Rval, __func__, __LINE__);
        pDspStlR2Y->AuxBufIn.BaseAddr = ULAddr;

        Rval = dsp_osal_phys2virt(pDspStlR2Y->YuvOut.BaseAddrY, &ULAddr);
        dsp_osal_print_err_line(Rval, __func__, __LINE__);
        pDspStlR2Y->YuvOut.BaseAddrY = ULAddr;

        Rval = dsp_osal_phys2virt(pDspStlR2Y->YuvOut.BaseAddrUV, &ULAddr);
        dsp_osal_print_err_line(Rval, __func__, __LINE__);
        pDspStlR2Y->YuvOut.BaseAddrUV = ULAddr;

        Rval = dsp_osal_phys2virt(pDspStlR2Y->IsoCfg.CfgAddress, &ULAddr);
        dsp_osal_print_err_line(Rval, __func__, __LINE__);
        pDspStlR2Y->IsoCfg.CfgAddress = ULAddr;

        Rval = dsp_still_raw2yuv(&pDspStlR2Y->RawIn,
                                 &pDspStlR2Y->AuxBufIn,
                                 &pDspStlR2Y->YuvOut,
                                 &pDspStlR2Y->IsoCfg,
                                 pDspStlR2Y->Opt,
                                 &pDspStlR2Y->AttachedRawSeq);
    }
        break;
    case AMBADSP_STL_R2R:
    {
        dsp_osal_typecast(&pDspStlR2R, &arg);

        /* convert to VirtAddr */
        Rval = dsp_osal_phys2virt(pDspStlR2R->RawIn.BaseAddr, &ULAddr);
        dsp_osal_print_err_line(Rval, __func__, __LINE__);
        pDspStlR2R->RawIn.BaseAddr = ULAddr;

        Rval = dsp_osal_phys2virt(pDspStlR2R->AuxBufIn.BaseAddr, &ULAddr);
        dsp_osal_print_err_line(Rval, __func__, __LINE__);
        pDspStlR2R->AuxBufIn.BaseAddr = ULAddr;

        Rval = dsp_osal_phys2virt(pDspStlR2R->RawOut.BaseAddr, &ULAddr);
        dsp_osal_print_err_line(Rval, __func__, __LINE__);
        pDspStlR2R->RawOut.BaseAddr = ULAddr;

        Rval = dsp_osal_phys2virt(pDspStlR2R->IsoCfg.CfgAddress, &ULAddr);
        dsp_osal_print_err_line(Rval, __func__, __LINE__);
        pDspStlR2R->IsoCfg.CfgAddress = ULAddr;

        Rval = dsp_still_raw2raw(&pDspStlR2R->RawIn,
                                 &pDspStlR2R->AuxBufIn,
                                 &pDspStlR2R->RawOut,
                                 &pDspStlR2R->IsoCfg,
                                 pDspStlR2R->Opt,
                                 &pDspStlR2R->AttachedRawSeq);
     }
        break;
    /******************  Decode    ******************/
    case AMBADSP_VIDEO_DEC_CFG:
    {
        dsp_osal_typecast(&pDspVideoDecCfg, &arg);

        for (i = 0U; i < pDspVideoDecCfg->MaxNumStream; i++) {
            Rval = dsp_osal_phys2virt(pDspVideoDecCfg->StrmCfg[i].BitsBufAddr, &ULAddr);
            dsp_osal_print_err_line(Rval, __func__, __LINE__);
            pDspVideoDecCfg->StrmCfg[i].BitsBufAddr = ULAddr;
        }

        Rval = dsp_video_dec_cfg(pDspVideoDecCfg->MaxNumStream,
                                 pDspVideoDecCfg->StrmCfg);
    }
        break;
    case AMBADSP_VIDEO_DEC_START:
        dsp_osal_typecast(&pDspVideoDecStart, &arg);
        Rval = dsp_video_dec_start(pDspVideoDecStart->NumStream,
                                   pDspVideoDecStart->StreamIdx,
                                   pDspVideoDecStart->StartCfg);
        break;
    case AMBADSP_VIDEO_DEC_STOP:
        dsp_osal_typecast(&pDspVideoDecStop, &arg);
        Rval = dsp_video_dec_stop(pDspVideoDecStop->NumStream,
                                  pDspVideoDecStop->StreamIdx,
                                  pDspVideoDecStop->ShowLastFrame);
        break;
    case AMBADSP_VIDEO_DEC_TRICKPLAY:
        dsp_osal_typecast(&pDspVideoDecTrickplay, &arg);
        Rval = dsp_video_dec_trickplay(pDspVideoDecTrickplay->NumStream,
                                       pDspVideoDecTrickplay->StreamIdx,
                                       pDspVideoDecTrickplay->TrickPlay);
        break;
    case AMBADSP_VIDEO_DEC_BITS_UPDATE:
    {
        dsp_osal_typecast(&pDspVideoDecBitsUpdate, &arg);

        for (i = 0U; i < pDspVideoDecBitsUpdate->NumStream; i++) {
            Rval = dsp_osal_phys2virt(pDspVideoDecBitsUpdate->BitsFifo[i].StartAddr, &ULAddr);
            dsp_osal_print_err_line(Rval, __func__, __LINE__);
            pDspVideoDecBitsUpdate->BitsFifo[i].StartAddr = ULAddr;

            Rval = dsp_osal_phys2virt(pDspVideoDecBitsUpdate->BitsFifo[i].EndAddr, &ULAddr);
            dsp_osal_print_err_line(Rval, __func__, __LINE__);
            pDspVideoDecBitsUpdate->BitsFifo[i].EndAddr = ULAddr;
        }

        Rval = dsp_video_dec_bitsfifo_update(pDspVideoDecBitsUpdate->NumStream,
                                             pDspVideoDecBitsUpdate->StreamIdx,
                                             pDspVideoDecBitsUpdate->BitsFifo);
    }

        break;
    case AMBADSP_VIDEO_DEC_POST_CTRL:
        dsp_osal_typecast(&pDspVideoDecPostCtrl, &arg);
        Rval = dps_video_dec_post_ctrl(pDspVideoDecPostCtrl->StreamIdx,
                                       pDspVideoDecPostCtrl->NumPostCtrl,
                                       pDspVideoDecPostCtrl->PostCtrl);
        break;
    case AMBADSP_STILL_DEC_START:
    {
        dsp_osal_typecast(&pDspStillDecStart, &arg);

        /* convert to VirtAddr */
        Rval = dsp_osal_phys2virt(pDspStillDecStart->DecConfig.BitsAddr, &ULAddr);
        dsp_osal_print_err_line(Rval, __func__, __LINE__);
        pDspStillDecStart->DecConfig.BitsAddr = ULAddr;

        Rval = dsp_osal_phys2virt(pDspStillDecStart->DecConfig.YuvBufAddr, &ULAddr);
        dsp_osal_print_err_line(Rval, __func__, __LINE__);
        pDspStillDecStart->DecConfig.YuvBufAddr = ULAddr;

        Rval = dsp_still_dec_start(pDspStillDecStart->StreamIdx,
                                   &pDspStillDecStart->DecConfig);
    }
        break;
    case AMBADSP_STILL_DEC_STOP:
        Rval = dsp_still_dec_stop();
        break;
    case AMBADSP_STILL_DEC_Y2Y:
    {
        dsp_osal_typecast(&pDspStillDecY2Y, &arg);

        /* convert to VirtAddr */
        Rval = dsp_osal_phys2virt(pDspStillDecY2Y->SrcYuvBufAddr.BaseAddrY, &ULAddr);
        dsp_osal_print_err_line(Rval, __func__, __LINE__);
        pDspStillDecY2Y->SrcYuvBufAddr.BaseAddrY = ULAddr;

        Rval = dsp_osal_phys2virt(pDspStillDecY2Y->SrcYuvBufAddr.BaseAddrUV, &ULAddr);
        dsp_osal_print_err_line(Rval, __func__, __LINE__);
        pDspStillDecY2Y->SrcYuvBufAddr.BaseAddrUV = ULAddr;

        Rval = dsp_osal_phys2virt(pDspStillDecY2Y->DestYuvBufAddr.BaseAddrY, &ULAddr);
        dsp_osal_print_err_line(Rval, __func__, __LINE__);
        pDspStillDecY2Y->DestYuvBufAddr.BaseAddrY = ULAddr;

        Rval = dsp_osal_phys2virt(pDspStillDecY2Y->DestYuvBufAddr.BaseAddrUV, &ULAddr);
        dsp_osal_print_err_line(Rval, __func__, __LINE__);
        pDspStillDecY2Y->DestYuvBufAddr.BaseAddrUV = ULAddr;

        Rval = dsp_still_dec_yuv2yuv(&pDspStillDecY2Y->SrcYuvBufAddr,
                                     &pDspStillDecY2Y->DestYuvBufAddr,
                                     &pDspStillDecY2Y->Operation);
    }
        break;
    case AMBADSP_STILL_DEC_BLEND:
    {
        dsp_osal_typecast(&pDspStillDecBlend, &arg);

        /* convert to VirtAddr */
        Rval = dsp_osal_phys2virt(pDspStillDecBlend->Src1YuvBufAddr.BaseAddrY, &ULAddr);
        dsp_osal_print_err_line(Rval, __func__, __LINE__);
        pDspStillDecBlend->Src1YuvBufAddr.BaseAddrY = ULAddr;

        Rval = dsp_osal_phys2virt(pDspStillDecBlend->Src1YuvBufAddr.BaseAddrUV, &ULAddr);
        dsp_osal_print_err_line(Rval, __func__, __LINE__);
        pDspStillDecBlend->Src1YuvBufAddr.BaseAddrUV = ULAddr;

        Rval = dsp_osal_phys2virt(pDspStillDecBlend->Src2YuvBufAddr.BaseAddrY, &ULAddr);
        dsp_osal_print_err_line(Rval, __func__, __LINE__);
        pDspStillDecBlend->Src2YuvBufAddr.BaseAddrY = ULAddr;

        Rval = dsp_osal_phys2virt(pDspStillDecBlend->Src2YuvBufAddr.BaseAddrUV, &ULAddr);
        dsp_osal_print_err_line(Rval, __func__, __LINE__);
        pDspStillDecBlend->Src2YuvBufAddr.BaseAddrUV = ULAddr;

        Rval = dsp_osal_phys2virt(pDspStillDecBlend->DestYuvBufAddr.BaseAddrY, &ULAddr);
        dsp_osal_print_err_line(Rval, __func__, __LINE__);
        pDspStillDecBlend->DestYuvBufAddr.BaseAddrY = ULAddr;

        Rval = dsp_osal_phys2virt(pDspStillDecBlend->DestYuvBufAddr.BaseAddrUV, &ULAddr);
        dsp_osal_print_err_line(Rval, __func__, __LINE__);
        pDspStillDecBlend->DestYuvBufAddr.BaseAddrUV = ULAddr;

        Rval = dsp_still_dec_yuv_blend(&pDspStillDecBlend->Src1YuvBufAddr,
                                       &pDspStillDecBlend->Src2YuvBufAddr,
                                       &pDspStillDecBlend->DestYuvBufAddr,
                                       &pDspStillDecBlend->Operation);
    }
        break;
    case AMBADSP_STILL_DEC_DISP_YUV:
    {
        dsp_osal_typecast(&pDspStillDecDispYuv, &arg);

        /* convert to VirtAddr */
        Rval = dsp_osal_phys2virt(pDspStillDecDispYuv->YuvBufAddr.BaseAddrY, &ULAddr);
        dsp_osal_print_err_line(Rval, __func__, __LINE__);
        pDspStillDecDispYuv->YuvBufAddr.BaseAddrY = ULAddr;

        Rval = dsp_osal_phys2virt(pDspStillDecDispYuv->YuvBufAddr.BaseAddrUV, &ULAddr);
        dsp_osal_print_err_line(Rval, __func__, __LINE__);
        pDspStillDecDispYuv->YuvBufAddr.BaseAddrUV = ULAddr;

        Rval = dsp_still_dec_disp_yuv_img(pDspStillDecDispYuv->VoutIdx,
                                          &pDspStillDecDispYuv->YuvBufAddr,
                                          &pDspStillDecDispYuv->VoutConfig);
    }
        break;
#if defined (CONFIG_ENABLE_DSP_DIAG)
    /******************  Diag    ********************/
    case AMBADSP_DIAG_CASE:
    {
        dsp_osal_typecast(&pU32, &arg);
        Rval = dsp_diag_case(*pU32);
    }
        break;
#endif

#if defined (CONFIG_SOC_CV2FS) || defined (CONFIG_SOC_CV22FS)
    case AMBADSP_MAIN_SAFETY_CHECK:
    {
        dsp_osal_typecast(&pMainSafetyChk, &arg);
        Rval = dsp_main_safety_chk(pMainSafetyChk->Id,
                                   pMainSafetyChk->SubId,
                                   &pMainSafetyChk->Status);
    }
        break;
    case AMBADSP_MAIN_SAFETY_CONFIG:
    {
        dsp_osal_typecast(&pMainSafetyCfg, &arg);
        Rval = dsp_main_safety_cfg(pMainSafetyCfg->Id,
                                   pMainSafetyCfg->Val0,
                                   pMainSafetyCfg->Val1);
    }
        break;
#endif
    default:
        dsp_osal_printU5("[ERR] dsp_ioctl_impl() : unknown IOCTL 0x%X\n", cmd, 0U, 0U, 0U, 0U);
        Rval = DSP_ERR_0001;
        break;
    }

    return Rval;
}

