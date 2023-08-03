/**
*  @file SvcBufMap.h
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
*  @details svc buffer map
*
*/

#ifndef SVC_BUFMAP_H
#define SVC_BUFMAP_H

#define ICAM_BUF_ALIGN          (((UINT32)AMBA_CACHE_LINE_SIZE) * 2U)

/* fixed memory id */
#define FMEM_ID_DSP_LOG          (0U)
#define FMEM_ID_DSP_WRK          (1U)
#define FMEM_ID_VP_MSG           (2U)
#define FMEM_ID_PYRAMID          (4U)
#define FMEM_ID_ITUNER           (5U)
#define FMEM_ID_CALIB            (6U)
#define FMEM_ID_IK_WRK           (7U)

#if defined(CONFIG_ICAM_PROJECT_ADAS_DVR) && defined(CONFIG_BUILD_AMBA_ADAS)
#define FMEM_ID_ADAS_LDWS_CALIB  (8U)
#define FMEM_ID_ADAS_LDWS        (9U)
#define FMEM_ID_ADAS_FC_CALIB    (10U)
#endif

#if defined(CONFIG_BUILD_AMBA_ADAS)
#define FMEM_ID_CALIB_EMIRROR    (11U)
#endif

#define FMEM_ID_TRANSIENT_BUF    (12U)

#define FMEM_ID_AUDIO_INP_NC     (14U)
#define FMEM_ID_AUDIO_INP_CA     (15U)
#define FMEM_ID_AUDIO_ENC_CA     (16U)
#define FMEM_ID_AUDIO_OUP_NC     (17U)
#define FMEM_ID_AUDIO_OUP_CA     (18U)
#define FMEM_ID_AUDIO_DEC_CA     (19U)
#define FMEM_ID_AUD_ENC_BS       (20U)

#define FMEM_ID_SYS_STAT         (21U)

#if defined(CONFIG_BUILD_AMBA_ADAS)
#define FMEM_ID_EMIRROR_CT       (22U)
#endif

#if defined(CONFIG_AMBA_RTSP)
#define FMEM_ID_RTSP_SERVICE     (23U)
#endif

#define FMEM_ID_DIRT_DETECT_WRK  (24U)
#define FMEM_ID_NVM_CRC_CHECK    (25U)
#define FMEM_ID_BITS_FIFO        (26U)
#define FMEM_ID_VFS              (27U)
#define FMEM_ID_AUD_DEC_BS       (28U)
#define FMEM_ID_PLAYER           (29U)

#if defined(CONFIG_ICAM_PROJECT_SURROUND)
#define FMEM_ID_CAL_SUR_CT       (30U)
#define FMEM_ID_CAL_SUR_3D_VIEW  (31U)
#define FMEM_ID_CAL_INFO_SUR     (32U)
#define FMEM_ID_CAL_SUR_CAR      (33U)
#define FMEM_ID_CAL_SUR_CAR_CMPR (34U)
#define FMEM_ID_CAL_SUR_LV_CAL   (35U)
#if defined(CONFIG_ICAM_CV_AMBANET)
#define FMEM_ID_SUR_CV_PSD       (36U)
#define FMEM_ID_SUR_CV_OWS       (37U)
#define FMEM_ID_SUR_CV_RCTA      (38U)
#endif
#endif
#define FMEM_ID_IMG_AAA_MEM      (39U)
#define FMEM_ID_OSD_BUF          (40U)
#if defined(CONFIG_ICAM_MAIN_Y12_OUTPUT_SUPPORTED)
#define FMEM_ID_MAIN_Y12_BUF     (41U)
#endif
#define FMEM_ID_CV_ALGO          (42U)

#if defined(CONFIG_BUILD_AMBA_ADAS)
#define FMEM_ID_3IN1_EMIRROR     (43U)
#define FMEM_ID_3IN1V_EMIRROR    (44U)
#endif
#define FMEM_ID_VOUT_DEF_IMG     (45U)

#if defined(CONFIG_BUILD_AMBA_ADAS)
#define FMEM_ID_EMIRROR_REC_DET  (46U)
#endif
#define FMEM_ID_CV_FILE_IN       (47U)
#define FMEM_ID_RMG_WRK          (48U)

#define FMEM_ID_CALIB_IK         (49U)
#define FMEM_ID_IMG_MAIN_MEM     (50U)
#define FMEM_ID_IMG_IQ_TBL       (51U)
#define FMEM_ID_MENU_BMP         (52U)
#define FMEM_ID_UCBIST           (53U)

#if defined(CONFIG_BUILD_AMBA_ADAS)
#define FMEM_ID_QUICK_CALIB      (54U)
#endif

#if defined(CONFIG_ICAM_FIXED_ISO_CFG_USED)
#define FMEM_ID_ICFG_IQ_CC_3D    (55U)
#define FMEM_ID_ICFG_IQ_CC_REG   (56U)
#endif

#if defined(CONFIG_ICAM_USB_USED)
#define FMEM_ID_USB_CA           (57U)
#define FMEM_ID_USB_NC           (58U)
#endif

#define FMEM_ID_STEREO_CALIB     (59U)
#define FMEM_ID_STEREO_INFOPACK  (60U)
#define FMEM_ID_ENCRYPT          (61U)
#define FMEM_ID_GUI_LOGO         (62U)
#define FMEM_ID_NUM              (63U)


/* profile 0 memory id */
#define SMEM_PF0_ID_IK_WRK         (0U)
#define SMEM_PF0_ID_VENC_BS        (1U)
#define SMEM_PF0_ID_VRSC_QU        (2U)
#define SMEM_PF0_ID_ARSC_QU        (3U)
#define SMEM_PF0_ID_RBX            (4U)
#define SMEM_PF0_ID_VDEC_BS        (6U)
#define SMEM_PF0_ID_DEMUXER        (7U)
#define SMEM_PF0_ID_STEREO         (8U)
#define SMEM_PF0_ID_NMLEVT_REC     (14U)
#define SMEM_PF0_ID_EMGEVT_REC     (15U)
#define SMEM_PF0_ID_BLENDING       (16U)
#define SMEM_PF0_ID_EMA_ROITBL     (17U)
#define SMEM_PF0_ID_EMA_TRACKER    (18U)
#define SMEM_PF0_ID_DEFAULT_RAW    (19U)
#define SMEM_PF0_ID_TIMELAPSE      (20U)
#define SMEM_PF0_ID_PICT_DISP      (21U)
#define SMEM_PF0_ID_STOR_MODULE    (22U)
#define SMEM_PF0_ID_LV_YUVFEED     (23U)
#define SMEM_PF0_ID_DRSC_QU        (24U)
#define SMEM_PF0_ID_PIP_ALPHA      (25U)
#define SMEM_PF0_ID_STLDEC_YUV     (29U)
#define SMEM_PF0_ID_MVINFO         (30U)
#define SMEM_PF0_ID_SUR_NC         (31U)
#define SMEM_PF0_ID_SUR_CA         (32U)
#define SMEM_PF0_ID_ADAS_LDWS      (33U)
#define SMEM_PF0_ID_ADAS_FC        (34U)
#define SMEM_PF0_ID_AUTO_CALIB     (35U)
#define SMEM_PF0_ID_AUTO_CALIB_MSG (36U)
#define SMEM_PF0_ID_CV_LOGGER      (37U)
#define SMEM_PF0_ID_STOPWATCH      (38U)
#define SMEM_PF0_ID_STEREO_WARP    (39U)

#define SMEM_PF0_ID_STEREO_AC      (40U)
#if defined(CONFIG_ICAM_LV_FEED_EXT_DATA_SUPPORTED)
#define SMEM_PF0_ID_LV_FEED_EXT_DATA (41U)
#endif
#if defined(CONFIG_ICAM_LV_FEED_EXT_YUV_SUPPORTED)
#define SMEM_PF0_ID_LV_FEED_EXT_YUV (42U)
#endif
#define SMEM_PF0_ID_FEED_FILE_YUV  (43U)
#define SMEM_PF0_ID_ADAS_BSD       (44U)
#define SMEM_PF0_ID_CV_LOGGER_OD  (45U)
#define SMEM_PF0_ID_CV_LOGGER_SEG  (46U)
#define SMEM_PF0_ID_NUM            (47U)


void SvcBufMap_Init(void);

#define SMAP_ID_LIVEVIEW        (0U)
#define SMAP_ID_PLAYBACK        (1U)
#define SMAP_ID_NUM             (2U)
void SvcBufMap_Config(UINT32 SharedMapId);

#endif  /* SVC_BUFMAP_H */
