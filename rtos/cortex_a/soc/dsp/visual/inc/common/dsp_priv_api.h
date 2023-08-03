/**
 *  @file dsp_priv_api.h
 *
 *  @copyright Copyright (c) 2020 Ambarella, Inc.
 *
 *  This file and its contents ("Software") are protected by intellectual property rights including, without limitation,
 *  U.S. and/or foreign copyrights.  This Software is also the confidential and proprietary information of Ambarella, Inc.
 *  and its licensors.  You may not use, reproduce, disclose, distribute, modify, or otherwise prepare derivative works
 *  of this Software or any portion thereof except pursuant to a signed license agreement or nondisclosure agreement with
 *  Ambarella, Inc. or its authorized affiliates.  In the absence of such an agreement, you agree to promptly notify and
 *  return this Software to Ambarella, Inc.
 *
 *  THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 *  WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL
 *  AMBARELLA, INC. OR ITS AFFILIATES BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 *  DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 *  COMPUTER FAILURE OR MALFUNCTION; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 *  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 *  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *  @details Definitions of private(lowercase) api
 *
 */

#ifndef DSP_PRIV_API_H
#define DSP_PRIV_API_H

#include "AmbaDSP.h"
#include "AmbaDSP_Event.h"
#include "AmbaDSP_VOUT.h"
#include "AmbaDSP_Liveview.h"
#include "AmbaDSP_VideoEnc.h"
#include "AmbaDSP_VideoDec.h"
#include "AmbaDSP_StillCapture.h"
#include "AmbaDSP_StillDec.h"
#include "ambadsp_ioctl.h"
#if defined (CONFIG_ENABLE_DSP_DIAG)
#include "dsp_diag_priv_api.h"
#endif

/***************************************
 *            AmbaDSP                  *
 ***************************************/
UINT32 dsp_main_init(const AMBA_DSP_SYS_CONFIG_s *pDspSysConfig);

UINT32 dsp_main_get_defsyscfg(AMBA_DSP_SYS_CONFIG_s *pDspSysConfig);

UINT32 dsp_main_suspend(void);

UINT32 dsp_main_resume(void);

UINT32 dsp_main_set_work_area(ULONG WorkAreaAddr, UINT32 WorkSize);

UINT32 dsp_main_set_protect_area(UINT32 Type, ULONG AreaAddr, UINT32 Size, UINT32 IsCached);

UINT32 dsp_main_get_dsp_ver_info(AMBA_DSP_VERSION_INFO_s *Info);

UINT32 dsp_main_msg_parse_entry(UINT32 EntryArg);

UINT32 dsp_main_wait_vin_interrupt(UINT32 Flag, UINT32 *ActualFlag, UINT32 TimeOut);

UINT32 dsp_main_wait_vout_interrupt(UINT32 Flag, UINT32 *ActualFlag, UINT32 TimeOut);

UINT32 dsp_main_wait_flag(UINT32 Flag, UINT32 *ActualFlag, UINT32 TimeOut);

UINT32 dsp_resource_limit_config(const AMBA_DSP_RESOURCE_s *pResource);

#define DSP_OCTAVE_PLOY_IDX             (0U)
#define DSP_OCTAVE_PLOY_LEN             (3U)
#define DSP_OCTAVE_LUMA_DEP_IDX         (6U)
#define DSP_OCTAVE_LUMA_DEP_LEN         (2U)
#define DSP_OCTAVE_HIER_DISABLE_BIT_IDX (8U)
#define DSP_OCTAVE_HIER_DISABLE_BIT_LEN (8U)
UINT32 dsp_cal_hier_buffer_size(const UINT16 *pMaxMainWidth, const UINT16 *pMaxMainHeight,
                                const UINT16 *pMaxHierWidth, const UINT16 *pMaxHierHeight,
                                const UINT16 OctaveMode,
                                UINT32 *pHierBufSize, UINT16 *pHierBufWidth, UINT16 *pHierBufHeight);


UINT32 dsp_get_cmpr_raw_buf_info(UINT16 Width, UINT16 CmprRate, UINT16 *pRawWidth, UINT16 *pRawPitch);

UINT32 dsp_parload_config(UINT32 Enable, UINT32 Data);

UINT32 dsp_parload_region_unlock(UINT16 RegionIdx, UINT16 SubRegionIdx);

UINT32 dsp_sys_drv_cfg(UINT16 Type, const void *pDrvData);

UINT32 dsp_calc_enc_mv_buf_info(UINT16 Width, UINT16 Height, UINT32 Option, UINT32 *pBufSize);

UINT32 dsp_calc_vp_msg_buf_size(const UINT32 *NumMsgs, UINT32 *MsgSize);

UINT32 dsp_parse_mv_buf(UINT16 Width, UINT16 Height, UINT16 BufType, UINT32 Option, ULONG MvBufAddr);

#if defined (CONFIG_SOC_CV2FS) || defined (CONFIG_SOC_CV22FS)
UINT32 dsp_test_frame_cfg(UINT32 StageId, const AMBA_DSP_TEST_FRAME_CFG_s *pCfg);

UINT32 dsp_test_frame_ctrl(UINT32 NumStage, const AMBA_DSP_TEST_FRAME_CTRL_s *pCtrl);

UINT32 dsp_main_safety_chk(const UINT32 ID, const UINT32 SubID, UINT32 *pStatus);

UINT32 dsp_main_safety_cfg(const UINT32 ID, const UINT32 Val0, const UINT32 Val1);
#endif

#if defined (CONFIG_SOC_CV5) || defined (CONFIG_SOC_CV52)
UINT32 dsp_slice_cfg_calc(UINT16 Id,
                          const AMBA_DSP_WINDOW_DIMENSION_s *pIn,
                          const AMBA_DSP_WINDOW_DIMENSION_s *pOut,
                          AMBA_DSP_SLICE_CFG_s *pSliceCfg);
#endif
UINT32 dsp_get_status(dsp_status_t *pDspStatus);

void dsp_main_api_dump_init(UINT8 Enable, UINT32 Mask);

void dsp_main_api_check_init(UINT8 Disable, UINT32 Mask);

void dsp_cmd_show(UINT32 CmdCode, UINT8 On);

void dsp_cmd_show_all(UINT8 On);

void dsp_msg_show_all(UINT8 On);

void dsp_msg_show(UINT32 MsgCode, UINT8 On);

UINT32 dsp_set_debug_level(UINT32 Module, UINT32 Level, UINT8 CmdType);

#define DSP_DBG_ORCCODE_BIT_IDX         (0U)
#define DSP_DBG_ORCVIN_BIT_IDX          (1U)
#define DSP_DBG_ORCIDSP0_BIT_IDX        (2U)
#define DSP_DBG_ORCIDSP1_BIT_IDX        (3U)
#define DSP_DBG_ORCME_BIT_IDX           (4U)
#define DSP_DBG_ORCMDXF_BIT_IDX         (5U)
#define DSP_DBG_ORCME1_BIT_IDX          (6U)
#define DSP_DBG_ORCMDXF1_BIT_IDX        (7U)
#define DSP_DBG_ORC_LEN                 (1U)

#define DSP_DBG_THRD_ORCCODE_BIT_IDX    (0U)
#define DSP_DBG_THRD_ORCVIN_BIT_IDX     (4U)
#define DSP_DBG_THRD_ORCIDSP0_BIT_IDX   (8U)
#define DSP_DBG_THRD_ORCIDSP1_BIT_IDX   (12U)
#define DSP_DBG_THRD_ORCME_BIT_IDX      (16U)
#define DSP_DBG_THRD_ORCMDXF_BIT_IDX    (18U)
#define DSP_DBG_THRD_ORCME1_BIT_IDX     (20U)
#define DSP_DBG_THRD_ORCMDXF1_BIT_IDX   (22U)
#define DSP_DBG_THRD_LEN                (4U)
#define DSP_DBG_THRD_ME_MDXF_LEN        (2U)
UINT32 dsp_set_debug_thread(UINT32 ThreadValid, UINT32 ThreadMask, UINT8 CmdType);

void dsp_reset_dsp(void);

UINT32 dsp_stop(UINT32 CmdType);

UINT32 dsp_main_get_buf_info(UINT32 Type, AMBA_DSP_BUF_INFO_s *pBufInfo);

UINT32 dsp_main_get_dsp_bin_addr(AMBA_DSP_BIN_ADDR_s *pInfo);

void dsp_set_system_config(UINT8 ParIdx, UINT32 Val);

/***************************************
 *         AmbaDSP_Event               *
 ***************************************/
UINT32 dsp_event_hdlr_ctrl_cfg(UINT16 EventID, UINT16 MaxNumHandlers, AMBA_DSP_EVENT_HANDLER_f *pEventHandlers);

UINT32 dsp_event_hdlr_ctrl_reset(UINT16 EventID);

UINT32 dsp_event_hdlr_register(UINT16 EventID, AMBA_DSP_EVENT_HANDLER_f EventHandler);

UINT32 dsp_event_hdlr_unregister(UINT16 EventID, AMBA_DSP_EVENT_HANDLER_f EventHandler);

#if !defined(CONFIG_THREADX)
UINT32 dsp_event_release_info_pool(UINT16 EvtBufIdx);
#endif

/***************************************
 *         AmbaDSP_VOUT                *
 ***************************************/
UINT32 dsp_vout_reset(const UINT8 VoutIdx);

UINT32 dsp_vout_mixer_cfg(const UINT8 VoutIdx, const AMBA_DSP_VOUT_MIXER_CONFIG_s *pConfig);

UINT32 dsp_vout_mixer_cfg_bgcolor(const UINT8 VoutIdx, UINT32 BackColorYUV);

UINT32 dsp_vout_mixer_cfg_highcolor(const UINT8 VoutIdx, UINT8 LumaThreshold, UINT32 HighlightColorYUV);

UINT32 dsp_vout_mixer_cfg_csc(const UINT8 VoutIdx, const UINT8 CscCtrl);

UINT32 dsp_vout_mixer_ctrl(const UINT8 VoutIdx);

UINT32 dsp_vout_osd_cfg_buf(const UINT8 VoutIdx, const AMBA_DSP_VOUT_OSD_BUF_CONFIG_s *pBufConfig);

UINT32 dsp_vout_osd_ctrl(const UINT8 VoutIdx, UINT8 Enable, UINT8 SyncWithVin, UINT64 *pAttachedRawSeq);

UINT32 dsp_vout_display_cfg(const UINT8 VoutIdx, const AMBA_DSP_DISPLAY_CONFIG_s* pConfig);

UINT32 dsp_vout_display_ctrl(const UINT8 VoutIdx);

UINT32 dsp_vout_display_cfg_gamma(const UINT8 VoutIdx, ULONG TableAddr);

UINT32 dsp_vout_display_ctrl_gamma(const UINT8 VoutIdx, UINT8 Enable);

UINT32 dsp_vout_video_cfg(const UINT8 VoutIdx, const AMBA_DSP_VOUT_VIDEO_CFG_s *pConfig);

UINT32 dsp_vout_video_ctrl(const UINT8 VoutIdx, UINT8 Enable, UINT8 SyncWithVin, UINT64 *pAttachedRawSeq);

UINT32 dsp_vout_cfg_mixer_binding(UINT8 NumVout, const UINT8 *pVoutIdx, const AMBA_DSP_VOUT_DATA_PATH_CFG_s *pConfig);

UINT32 dsp_vout_mixer_csc_matrix_cfg(const UINT8 VoutIdx, const dsp_vout_csc_matrix_s *pCscMatrix);

UINT32 dsp_vout_dve_cfg(const UINT8 VoutIdx, UINT8 DveMode);

UINT32 dsp_vout_dve_ctrl(const UINT8 VoutIdx);

UINT32 dsp_vout_drv_cfg(UINT16 VoutId, UINT16 Type, const void *pDrvData);

/***************************************
 *         AmbaDSP_Liveview            *
 ***************************************/
UINT32 dsp_liveview_cfg(UINT16 NumViewZone, const AMBA_DSP_LIVEVIEW_VIEWZONE_CFG_s *pViewZoneCfg, UINT16 NumYUVStream, const AMBA_DSP_LIVEVIEW_STREAM_CFG_s *pYUVStrmCfg);

UINT32 dsp_liveview_ctrl(UINT16 NumViewZone, const UINT16 *pViewZoneId, const UINT8 *pEnable);

UINT32 dsp_liveview_update_cfg(UINT16 NumYuvStream, const AMBA_DSP_LIVEVIEW_STREAM_CFG_s *pYuvStrmCfg, UINT64 *pAttachedRawSeq);

UINT32 dsp_liveview_update_isocfg(UINT16 NumViewZone, const AMBA_DSP_LIVEVIEW_ISOCFG_CTRL_s *pIsoCfgCtrl, UINT64 *pAttachedRawSeq);

UINT32 dsp_liveview_update_vincfg(UINT16 VinId,
                                  UINT16 SubChNum,
                                  const AMBA_DSP_VIN_SUB_CHAN_s *pSubCh,
                                  const AMBA_DSP_LIVEVIEW_VINCFG_CTRL_s *pLvVinCfgCtrl,
                                  UINT64 *pAttachedRawSeq);

UINT32 dsp_liveview_update_pymdcfg(UINT16 NumViewZone,
                                   const UINT16 *pViewZoneId,
                                   const AMBA_DSP_LIVEVIEW_PYRAMID_s *pPyramid,
                                   const AMBA_DSP_LIVEVIEW_YUV_BUF_s *pPyramidBuf,
                                   UINT64 *pAttachedRawSeq);

UINT32 dsp_liveview_cfg_vin_cap(UINT16 VinId, UINT16 SubChNum, const AMBA_DSP_VIN_SUB_CHAN_CFG_s *pSubChCfg);

UINT32 dsp_liveview_cfg_vin_post(const UINT8 Type, UINT16 VinId);

UINT32 dsp_liveview_feed_raw_data(UINT16 NumViewZone, const UINT16 *pViewZoneId, const AMBA_DSP_LIVEVIEW_EXT_RAW_s *pExtBuf);

UINT32 dsp_liveview_feed_yuv_data(UINT16 NumViewZone, const UINT16 *pViewZoneId, const AMBA_DSP_EXT_YUV_BUF_s *pExtYuvBuf);

UINT32 dsp_liveview_yuvstream_sync(UINT16 YuvStrmIdx, const AMBA_DSP_LIVEVIEW_SYNC_CTRL_s *pYuvStrmSyncCtrl, UINT32 *pSyncJobId, UINT64 *pAttachedRawSeq);

UINT32 dsp_liveview_update_geocfg(UINT16 ViewZoneId, const AMBA_DSP_LIVEVIEW_GEOCFG_CTRL_s *pGeoCfgCtrl, UINT64 *pAttachedRawSeq);

UINT32 dsp_liveview_update_vz_src(UINT16 NumViewZone, const AMBA_DSP_LIVEVIEW_VZ_SRC_CFG_s *pVzSrcCfg, UINT64 *pAttachedRawSeq);

UINT32 dsp_liveview_sb_update(UINT16 ViewZoneId, UINT8 NumBand, const ULONG *pSidebandBufAddr);

UINT32 dsp_liveview_slice_cfg(UINT16 ViewZoneId, const AMBA_DSP_LIVEVIEW_SLICE_CFG_s *pLvSliceCfg);

UINT32 dsp_liveview_slice_cfg_calc(UINT16 ViewZoneId, AMBA_DSP_LIVEVIEW_SLICE_CFG_s *pLvSliceCfg);

UINT32 dsp_liveview_vz_postpone_cfg(UINT16 ViewZoneId, const AMBA_DSP_LV_VZ_POSTPONE_CFG_s *pVzPostPoneCfg);

UINT32 dsp_liveview_update_vin_state(UINT16 NumVin, const AMBA_DSP_LV_VIN_STATE_s *pVinState);

UINT32 dsp_liveview_slow_shutter_ctrl(UINT16 NumViewZone, const UINT16* pViewZoneId, const UINT32* pRatio);

#if defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)
UINT32 dsp_liveview_drop_repeat_ctrl(UINT16 NumViewZone, const UINT16* pViewZoneId, const AMBA_DSP_LIVEVIEW_DROP_REPEAT_s* pDropRptCfg);
#endif

UINT32 dsp_liveview_parse_vp_msg(ULONG VpMsgAddr);

UINT32 dsp_liveview_vin_drv_cfg(UINT16 VinId, UINT16 Type, const void *pDrvData);

UINT32 dsp_liveview_ik_drv_cfg(UINT16 ViewZoneId, UINT16 Type, const void *pDrvData);

UINT32 dsp_liveview_get_idsp_cfg(UINT16 ViewZoneId, ULONG *CfgAddr);

/***************************************
 *         AmbaDSP_VideoEnc            *
 ***************************************/
UINT32 dsp_video_enc_cfg(UINT16 NumStream, const UINT16 *pStreamIdx, const AMBA_DSP_VIDEO_ENC_STRM_CONFIG_s *pStreamConfig);

UINT32 dsp_video_enc_start(UINT16 NumStream, const UINT16 *pStreamIdx, const AMBA_DSP_VIDEO_ENC_START_CONFIG_s *pStartConfig, UINT64 *pAttachedRawSeq);

UINT32 dsp_video_enc_stop(UINT16 NumStream, const UINT16 *pStreamIdx, const UINT8 *pStopOption, UINT64 *pAttachedRawSeq);

UINT32 dsp_video_enc_ctrl_framerate(UINT16 NumStream, const UINT16 *pStreamIdx, const UINT32 *pDivisor, UINT64 *pAttachedRawSeq);

UINT32 dsp_video_enc_ctrl_repeat_drop(UINT16 NumStream, const UINT16* pStreamIdx, const AMBA_DSP_VIDEO_REPEAT_DROP_CONFIG_s *pRepeatDropCfg, const UINT64* pAttachedRawSeq);

UINT32 dsp_video_enc_ctrl_slowshutter(UINT16 NumVin, const UINT16 *pVinIdx, const UINT32 *pUpSamplingRate, const UINT64 *pAttachedRawSeq);

UINT32 dsp_video_enc_ctrl_blend(UINT16 NumStream, const UINT16 *pStreamIdx, const AMBA_DSP_VIDEO_BLEND_CONFIG_s *pBlendCfg);

UINT32 dsp_video_enc_ctrl_quality(UINT16 NumStream, const UINT16 *pStreamIdx, const AMBA_DSP_VIDEO_ENC_QUALITY_s *pQCtrl);

UINT32 dsp_video_enc_exec_intervalcap(UINT16 NumStream, const UINT16 *pStreamIdx);

UINT32 dsp_video_enc_feed_yuv_data(UINT16 NumStream, const UINT16 *pStreamIdx, const AMBA_DSP_EXT_YUV_BUF_s *pExtYuvBuf);

UINT32 dsp_video_enc_grp_cfg(const AMBA_DSP_VIDEO_ENC_GROUP_s *pGrpCfg);

UINT32 dsp_video_enc_mv_cfg(UINT16 StreamIdx, const AMBA_DSP_VIDEO_ENC_MV_CFG_s *pMvCfg);

UINT32 dsp_video_enc_desc_fmt_cfg(UINT16 StreamIdx, UINT16 CatIdx, UINT32 OptVal);

/***************************************
 *         AmbaDSP_VideoDec            *
 ***************************************/
UINT32 dsp_video_dec_cfg(UINT16 MaxNumStream, const AMBA_DSP_VIDDEC_STREAM_CONFIG_s *pStreamConfig);

UINT32 dsp_video_dec_start(UINT16 NumStream, const UINT16 *pStreamIdx, const AMBA_DSP_VIDDEC_START_CONFIG_s *pStartConfig);

UINT32 dsp_video_dec_stop(UINT16 NumStream, const UINT16 *pStreamIdx, const UINT8 *pShowLastFrame);

UINT32 dsp_video_dec_trickplay(UINT16 NumStream, const UINT16 *pStreamIdx, const AMBA_DSP_VIDDEC_TRICKPLAY_s *pTrickPlay);

UINT32 dsp_video_dec_bitsfifo_update(UINT16 NumStream, const UINT16 *pStreamIdx, const AMBA_DSP_VIDDEC_BITS_FIFO_s *pBitsFifo);

UINT32 dps_video_dec_post_ctrl(UINT16 StreamIdx, UINT16 NumPostCtrl, const AMBA_DSP_VIDDEC_POST_CTRL_s *pPostCtrl);

/***************************************
 *         AmbaDSP_StillCapture        *
 ***************************************/
UINT32 dsp_data_cap_cfg(UINT16 CapInstance, const AMBA_DSP_DATACAP_CFG_s *pDataCapCfg);

UINT32 dsp_update_cap_buffer(UINT16 CapInstance, const AMBA_DSP_DATACAP_BUF_CFG_s *pCapBuf, UINT64 *pAttachedRawSeq);

UINT32 dsp_data_cap_ctrl(UINT16 NumCapInstance, const UINT16 *pCapInstance, const AMBA_DSP_DATACAP_CTRL_s *pDataCapCtrl, UINT64 *pAttachedRawSeq);

UINT32 dsp_still_yuv2yuv(const AMBA_DSP_YUV_IMG_BUF_s *pYuvIn, const AMBA_DSP_YUV_IMG_BUF_s *pYuvOut, const AMBA_DSP_ISOCFG_CTRL_s *pIsoCfg, UINT32 Opt, UINT64 *pAttachedRawSeq);

UINT32 dsp_still_encode_ctrl(UINT16 StreamIdx, const AMBA_DSP_STLENC_CTRL_s *pStlEncCtrl, UINT64 *pAttachedRawSeq);

UINT32 dsp_calc_still_yuv_extbuf_size(UINT16 StreamIdx, UINT16 BufType, UINT16 *pBufPitch, UINT32 *pBufUnitSize);

UINT32 dsp_still_raw2yuv(const AMBA_DSP_RAW_BUF_s *pRawIn,
                         const AMBA_DSP_BUF_s *pAuxBufIn,
                         const AMBA_DSP_YUV_IMG_BUF_s *pYuvOut,
                         const AMBA_DSP_ISOCFG_CTRL_s *pIsoCfg,
                         UINT32 Opt,
                         UINT64 *pAttachedRawSeq);

UINT32 dsp_still_raw2raw(const AMBA_DSP_RAW_BUF_s *pRawIn,
                         const AMBA_DSP_BUF_s *pAuxBufIn,
                         const AMBA_DSP_RAW_BUF_s *pRawOut,
                         const AMBA_DSP_ISOCFG_CTRL_s *pIsoCfg,
                         UINT32 Opt,
                         UINT64 *pAttachedRawSeq);

/***************************************
 *         AmbaDSP_StillDec            *
 ***************************************/
UINT32 dsp_still_dec_start(UINT16 StreamIdx, const AMBA_DSP_STLDEC_START_CONFIG_s *pDecConfig);

UINT32 dsp_still_dec_stop(void);

UINT32 dsp_still_dec_yuv2yuv(const AMBA_DSP_YUV_IMG_BUF_s *pSrcYuvBufAddr,
                             const AMBA_DSP_YUV_IMG_BUF_s *pDestYuvBufAddr,
                             const AMBA_DSP_STLDEC_YUV2YUV_s *pOperation);

UINT32 dsp_still_dec_yuv_blend(const AMBA_DSP_YUV_IMG_BUF_s *pSrc1YuvBufAddr,
                               const AMBA_DSP_YUV_IMG_BUF_s *pSrc2YuvBufAddr,
                               const AMBA_DSP_YUV_IMG_BUF_s *pDestYuvBufAddr,
                               const AMBA_DSP_STLDEC_BLEND_s *pOperation);

UINT32 dsp_still_dec_disp_yuv_img(UINT8 VoutIdx,
                                  const AMBA_DSP_YUV_IMG_BUF_s *pYuvBufAddr,
                                  const AMBA_DSP_VOUT_VIDEO_CFG_s *pVoutConfig);
#ifdef CONFIG_ENABLE_DSP_MONITOR
/***************************************
 *         AmbaDSP_Monitor             *
 ***************************************/
UINT32 dsp_mon_init(void);
UINT32 dsp_mon_get_cfg(UINT32 ModuleId, AMBA_DSP_MONITOR_CONFIG_s *pMonCfg);
UINT32 dsp_mon_crc_cmpr(const AMBA_DSP_MONITOR_CRC_CMPR_s *pCrcCmpr);
UINT32 dsp_mon_heartbeat_cfg(const AMBA_DSP_MONITOR_HEARTBEAT_s *pHeartBeatCfg);
UINT32 dsp_mon_error_notify(const AMBA_DSP_MONITOR_ERR_NOTIFY_s *pErrNotify);
#endif /* CONFIG_ENABLE_DSP_MONITOR */

#endif  /* DSP_PRIV_API_H */

