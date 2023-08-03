/**
 *  @file ambadsp_dev.c
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
 *  @details Ambadsp dev APIs in Qnx
 *
 */

#include <stdint.h>
#include "dsp_osal.h"
#include <devctl.h>
#include "AmbaDSP_Def.h"
#include "ambadsp_ioctl.h"

/*
 * Define THREAD_POOL_PARAM_T such that we can avoid a compiler
 * warning when we use the dispatch_*() functions below
 */
#define THREAD_POOL_PARAM_T dispatch_context_t

#include <sys/iofunc.h>
#include <sys/dispatch.h>

static iofunc_attr_t            dsp_iofunc_attr;
static pid_t                    binding_pid = 0U;

static int32_t check_binding_pid(pid_t pid)
{
    int32_t retcode = EOK;

    if (binding_pid == 0U) {
        binding_pid = pid;
        dsp_osal_printU5("[dspdev] binding to pid:%u", (uint32_t)binding_pid, 0U, 0U, 0U, 0U);
    } else {
        if (binding_pid != pid) {
            retcode = EACCES;
            dsp_osal_printU5("[ERR] devdsp is already bound to pid:%u (sending pid:%u)", (uint32_t)binding_pid, (uint32_t)pid, 0U, 0U, 0U);
        }
    }

    return retcode;
}

#define NBYTE_ERROR_SIZE 0xFFFFFFFFFFFFFFFFUL

static int32_t ambadsp_ioctl(resmgr_context_t *ctp, io_devctl_t *msg, RESMGR_OCB_T *ocb)
{
    int32_t status;
    int32_t retcode = 0;
    uint64_t nbytes = 0UL;
    uint32_t rval = 0U;
    void *arg;
    uint32_t idx;

    static uint64_t nbytes_array[AMBADSP_IOCTL_CMD_MAX_NUM] = {
            /******************   Ultility  ******************/
            /* 0x00 ~ 0x1F */
            sizeof(AMBA_DSP_VERSION_INFO_s),            /* 0x00 AMBADSP_GET_VERSION */
            sizeof(AMBA_DSP_BIN_ADDR_s),                /* 0x01 AMBADSP_GET_DSP_BIN_ADDR */
            NBYTE_ERROR_SIZE,                           /* 0x02 */
            sizeof(dsp_debug_dump_t),                   /* 0x03 AMBADSP_DEBUG_DUMP */
            sizeof(dsp_hier_buf_calc_t),                /* 0x04 AMBADSP_CALC_HIER_BUF */
            sizeof(dsp_raw_pitch_calc_t),               /* 0x05 AMBADSP_CALC_RAW_PITCH */
            sizeof(dsp_enc_mv_buf_calc_t),              /* 0x06 AMBADSP_CALC_ENC_MV_BUF */
            sizeof(dsp_enc_mv_buf_parse_t),             /* 0x07 AMBADSP_PARSE_ENC_MV_BUF */
            sizeof(dsp_vp_msg_buf_t),                   /* 0x08 AMBADSP_CALC_VP_MSG_BUF */
            sizeof(AMBA_DSP_PROTECT_s),                 /* 0x09 AMBADSP_GET_PROTECT_BUF */
            sizeof(dsp_buf_info_t),                     /* 0x0A AMBADSP_GET_BUF_INFO */
            NBYTE_ERROR_SIZE,                           /* 0x0B AMBADSP_CACHE_BUF_OPERATE TBD for QNX */
#if defined (CONFIG_SOC_CV2FS) || defined (CONFIG_SOC_CV22FS)
            sizeof(dsp_test_frame_config_t),            /* 0x0C AMBADSP_TEST_FRAME_CFG */
            sizeof(dsp_test_frame_control_t),           /* 0x0D AMBADSP_TEST_FRAME_CTRL */
            sizeof(dsp_main_safety_check_t),            /* 0x0E AMBADSP_MAIN_SAFETY_CHECK */
            sizeof(dsp_main_safety_config_t),           /* 0x0F AMBADSP_MAIN_SAFETY_CONFIG */
#elif defined (CONFIG_SOC_CV5) || defined (CONFIG_SOC_CV52) || defined (CONFIG_SOC_CV7)
            NBYTE_ERROR_SIZE,                           /* 0x0C */
            NBYTE_ERROR_SIZE,                           /* 0x0D */
            sizeof(dsp_slice_cfg_t),                    /* 0x0E AMBADSP_SLICE_CFG_CALC TBD for QNX */
            NBYTE_ERROR_SIZE,                           /* 0x0F */
#else
            NBYTE_ERROR_SIZE,                           /* 0x0C */
            NBYTE_ERROR_SIZE,                           /* 0x0D */
            NBYTE_ERROR_SIZE,                           /* 0x0E */
            NBYTE_ERROR_SIZE,                           /* 0x0F */
#endif
            sizeof(dsp_status_t),                       /* 0x10 AMBADSP_GET_STATUS */
            NBYTE_ERROR_SIZE,                           /* 0x11 */
            NBYTE_ERROR_SIZE,                           /* 0x12 */
            NBYTE_ERROR_SIZE,                           /* 0x13 */
            NBYTE_ERROR_SIZE,                           /* 0x14 */
            NBYTE_ERROR_SIZE,                           /* 0x15 */
            NBYTE_ERROR_SIZE,                           /* 0x16 */
            NBYTE_ERROR_SIZE,                           /* 0x17 */
            NBYTE_ERROR_SIZE,                           /* 0x18 */
            NBYTE_ERROR_SIZE,                           /* 0x19 */
            NBYTE_ERROR_SIZE,                           /* 0x1A */
            NBYTE_ERROR_SIZE,                           /* 0x1B */
            NBYTE_ERROR_SIZE,                           /* 0x1C */
            NBYTE_ERROR_SIZE,                           /* 0x1D */
            NBYTE_ERROR_SIZE,                           /* 0x1E */
            NBYTE_ERROR_SIZE,                           /* 0x1F */

            /******************   Event    ******************/
            /* 0x20 ~ 0x2F */
            sizeof(dsp_evnt_hdlr_cfg_t),                /* 0x20 AMBADSP_EVENT_HDLR_CONFIG */
            sizeof(uint16_t),                           /* 0x21 AMBADSP_EVENT_HDLR_RESET */
            sizeof(dsp_evnt_hdlr_op_t),                 /* 0x22 AMBADSP_EVENT_HDLR_REGISTER */
            sizeof(dsp_evnt_hdlr_op_t),                 /* 0x23 AMBADSP_EVENT_HDLR_DEREGISTER */
            sizeof(dsp_evnt_data_t),                    /* 0x24 AMBADSP_EVENT_GET_DATA */
            sizeof(uint16_t),                           /* 0x25 AMBADSP_EVENT_RELEASE_INFO_POOL */
            NBYTE_ERROR_SIZE,                           /* 0x26 */
            NBYTE_ERROR_SIZE,                           /* 0x27 */
            NBYTE_ERROR_SIZE,                           /* 0x28 */
            NBYTE_ERROR_SIZE,                           /* 0x29 */
            NBYTE_ERROR_SIZE,                           /* 0x2A */
            NBYTE_ERROR_SIZE,                           /* 0x2B */
            NBYTE_ERROR_SIZE,                           /* 0x2C */
            NBYTE_ERROR_SIZE,                           /* 0x2D */
            NBYTE_ERROR_SIZE,                           /* 0x2E */
            NBYTE_ERROR_SIZE,                           /* 0x2F */

            /******************   Main     ******************/
            /* 0x30 ~ 0x4F */
            sizeof(AMBA_DSP_SYS_CONFIG_s),              /* 0x30 AMBADSP_GET_DEFAULT_SYSCFG */
            sizeof(AMBA_DSP_SYS_CONFIG_s),              /* 0x31 AMBADSP_MAIN_INIT */
            0UL,                                        /* 0x32 AMBADSP_MAIN_SUSPEND */
            0UL,                                        /* 0x33 AMBADSP_MAIN_RESUME */
            sizeof(dsp_mem_t),                          /* 0x34 AMBADSP_SET_WORK_MEMORY */
            sizeof(uint32_t),                           /* 0x35 AMBADSP_MAIN_MSG_PARSE */
            sizeof(dsp_wait_sig_t),                     /* 0x36 AMBADSP_MAIN_WAIT_VIN_INT */
            sizeof(dsp_wait_sig_t),                     /* 0x37 AMBADSP_MAIN_WAIT_VOUT_INT */
            sizeof(dsp_wait_sig_t),                     /* 0x38 AMBADSP_MAIN_WAIT_FLAG */
            sizeof(dsp_resc_limit_cfg_t),               /* 0x39 AMBADSP_RESC_LIMIT_CONFIG */
            sizeof(dsp_partial_load_cfg_t),             /* 0x3A AMBADSP_PARLOAD_CONFIG */
            sizeof(dsp_partial_load_unlock_t),          /* 0x3B AMBADSP_PARLOAD_REGION_UNLOCK */
            sizeof(dsp_sys_drv_cfg_t),                  /* 0x3C AMBADSP_SYS_DRV_CFG */
            sizeof(dsp_protect_area_t),                 /* 0x3D AMBADSP_SET_PROTECT_AREA */
            sizeof(dsp_ext_buf_tbl_t),                  /* 0x3E AMBADSP_EXT_RAW_BUF_TBL_CFG */
            NBYTE_ERROR_SIZE,                           /* 0x3F */
            NBYTE_ERROR_SIZE,                           /* 0x40 */
            NBYTE_ERROR_SIZE,                           /* 0x41 */
            NBYTE_ERROR_SIZE,                           /* 0x42 */
            NBYTE_ERROR_SIZE,                           /* 0x43 */
            NBYTE_ERROR_SIZE,                           /* 0x44 */
            NBYTE_ERROR_SIZE,                           /* 0x45 */
            NBYTE_ERROR_SIZE,                           /* 0x46 */
            NBYTE_ERROR_SIZE,                           /* 0x47 */
            NBYTE_ERROR_SIZE,                           /* 0x48 */
            NBYTE_ERROR_SIZE,                           /* 0x49 */
            NBYTE_ERROR_SIZE,                           /* 0x4A */
            NBYTE_ERROR_SIZE,                           /* 0x4B */
            NBYTE_ERROR_SIZE,                           /* 0x4C */
            NBYTE_ERROR_SIZE,                           /* 0x4D */
            NBYTE_ERROR_SIZE,                           /* 0x4E */
            NBYTE_ERROR_SIZE,                           /* 0x4F */

            /******************    Vout    ******************/
            /* 0x50 ~ 0x6F */
            sizeof(uint8_t),                            /* 0x50 AMBADSP_VOUT_RESET */
            sizeof(dsp_vout_mixer_cfg_t),               /* 0x51 AMBADSP_VOUT_MIXER_CFG */
            sizeof(dsp_vout_mixer_bgc_cfg_t),           /* 0x52 AMBADSP_VOUT_MIXER_BGC_CFG */
            sizeof(dsp_vout_mixer_hlc_cfg_t),           /* 0x53 AMBADSP_VOUT_MIXER_HLC_CFG */
            sizeof(dsp_vout_mixer_csc_cfg_t),           /* 0x54 AMBADSP_VOUT_MIXER_CSC_CFG */
            sizeof(dsp_vout_mixer_csc_matrix_cfg_t),    /* 0x55 AMBADSP_VOUT_MIXER_CSC_MATRIX_CFG */
            sizeof(uint8_t),                            /* 0x56 AMBADSP_VOUT_MIXER_CTRL */
            sizeof(dsp_vout_osd_buf_cfg_t),             /* 0x57 AMBADSP_VOUT_OSD_BUF_CFG */
            sizeof(dsp_vout_osd_ctrl_t),                /* 0x58 AMBADSP_VOUT_OSD_CTRL */
            sizeof(dsp_vout_disp_cfg_t),                /* 0x59 AMBADSP_VOUT_DISP_CFG */
            sizeof(uint8_t),                            /* 0x5A AMBADSP_VOUT_DISP_CTRL */
            sizeof(dsp_vout_disp_gamma_cfg_t),          /* 0x5B AMBADSP_VOUT_DISP_GAMMA_CFG */
            sizeof(dsp_vout_disp_gamma_ctrl_t),         /* 0x5C AMBADSP_VOUT_DISP_GAMMA_CTRL */
            sizeof(dsp_vout_video_cfg_t),               /* 0x5D AMBADSP_VOUT_VIDEO_CFG */
            sizeof(dsp_vout_video_ctrl_t),              /* 0x5E AMBADSP_VOUT_VIDEO_CTRL */
            sizeof(dsp_vout_mixer_bind_cfg_t),          /* 0x5F AMBADSP_VOUT_MIXER_BIND_CFG */
            sizeof(dsp_vout_dve_cfg_t),                 /* 0x60 AMBADSP_VOUT_DVE_CFG */
            sizeof(uint8_t),                            /* 0x61 AMBADSP_VOUT_DVE_CTRL */
            sizeof(dsp_vout_drv_cfg_t),                 /* 0x62 AMBADSP_VOUT_DRV_CFG */
            NBYTE_ERROR_SIZE,                           /* 0x63 */
            NBYTE_ERROR_SIZE,                           /* 0x64 */
            NBYTE_ERROR_SIZE,                           /* 0x65 */
            NBYTE_ERROR_SIZE,                           /* 0x66 */
            NBYTE_ERROR_SIZE,                           /* 0x67 */
            NBYTE_ERROR_SIZE,                           /* 0x68 */
            NBYTE_ERROR_SIZE,                           /* 0x69 */
            NBYTE_ERROR_SIZE,                           /* 0x6A */
            NBYTE_ERROR_SIZE,                           /* 0x6B */
            NBYTE_ERROR_SIZE,                           /* 0x6C */
            NBYTE_ERROR_SIZE,                           /* 0x6D */
            NBYTE_ERROR_SIZE,                           /* 0x6E */
            NBYTE_ERROR_SIZE,                           /* 0x6F */

            /******************  Liveview  ******************/
            /* 0x70 ~ 0x8F */
            sizeof(dsp_liveview_cfg_t),                 /* 0x70 AMBADSP_LIVEVIEW_CFG */
            sizeof(dsp_liveview_ctrl_t),                /* 0x71 AMBADSP_LIVEVIEW_CTRL */
            sizeof(dsp_liveview_update_cfg_t),          /* 0x72 AMBADSP_LIVEVIEW_UPDATE_CFG */
            sizeof(dsp_liveview_update_isocfg_t),       /* 0x73 AMBADSP_LIVEVIEW_UPDATE_ISOCFG */
            sizeof(dsp_liveview_update_vincfg_t),       /* 0x74 AMBADSP_LIVEVIEW_UPDATE_VINCFG */
            sizeof(dsp_liveview_update_pymdcfg_t),      /* 0x75 AMBADSP_LIVEVIEW_UPDATE_PYMDCFG */
            sizeof(dsp_liveview_vin_cap_cfg_t),         /* 0x76 AMBADSP_LIVEVIEW_VIN_CAP_CFG */
            sizeof(dsp_liveview_vin_post_cfg_t),        /* 0x77 AMBADSP_LIVEVIEW_VIN_POST_CFG */
            sizeof(dsp_liveview_feed_raw_data_t),       /* 0x78 AMBADSP_LIVEVIEW_FEED_RAW */
            sizeof(dsp_liveview_feed_yuv_data_t),       /* 0x79 AMBADSP_LIVEVIEW_FEED_YUV */
            sizeof(dsp_liveview_yuvstrm_sync_t),        /* 0x7A AMBADSP_LIVEVIEW_YUVSTRM_SYNC */
            sizeof(dsp_liveview_update_geocfg_t),       /* 0x7B AMBADSP_LIVEVIEW_UPDATE_GEOCFG */
            sizeof(dsp_liveview_update_vz_src_t),       /* 0x7C AMBADSP_LIVEVIEW_UPDATE_VZ_SRC */
            sizeof(dsp_liveview_update_sideband_t),     /* 0x7D AMBADSP_LIVEVIEW_UPDATE_SIDEBAND */
            sizeof(dsp_liveview_slice_cfg_t),           /* 0x7E AMBADSP_LIVEVIEW_SLICE_CFG */
            sizeof(dsp_liveview_vz_postpone_cfg_t),     /* 0x7F AMBADSP_LIVEVIEW_VZ_POSTPONE_CFG */
            sizeof(dsp_liveview_update_vin_state_t),    /* 0x80 AMBADSP_LIVEVIEW_UPDATE_VIN_STATE */
            sizeof(ULONG),                              /* 0x81 AMBADSP_LIVEVIEW_PARSE_VP_MSG */
            sizeof(dsp_liveview_vin_drv_cfg_t),         /* 0x82 AMBADSP_LIVEVIEW_VIN_DRV_CFG */
            sizeof(dsp_liveview_ik_drv_cfg_t),          /* 0x83 AMBADSP_LIVEVIEW_IK_DRV_CFG */
            sizeof(dsp_liveview_slice_cfg_t),           /* 0x84 AMBADSP_LIVEVIEW_SLICE_CFG_CALC */
            sizeof(dsp_liveview_idsp_cfg_t),            /* 0x85 AMBADSP_LIVEVIEW_GET_IDSPCFG */
            sizeof(dsp_liveview_slow_shutter_ctrl_t),   /* 0x86 AMBADSP_LIVEVIEW_SLOW_SHUTTER_CTRL */
#if defined (CONFIG_SOC_CV5) || defined (CONFIG_SOC_CV52) || defined (CONFIG_SOC_CV7)
            sizeof(dsp_liveview_drop_repeat_ctrl_t),    /* 0x87 AMBADSP_LIVEVIEW_DROP_REPEAT_CTRL */
#else
            NBYTE_ERROR_SIZE,                           /* 0x87 AMBADSP_LIVEVIEW_DROP_REPEAT_CTRL */
#endif
            NBYTE_ERROR_SIZE,                           /* 0x88 */
            NBYTE_ERROR_SIZE,                           /* 0x89 */
            NBYTE_ERROR_SIZE,                           /* 0x8A */
            NBYTE_ERROR_SIZE,                           /* 0x8B */
            NBYTE_ERROR_SIZE,                           /* 0x8C */
            NBYTE_ERROR_SIZE,                           /* 0x8D */
            NBYTE_ERROR_SIZE,                           /* 0x8E */
            NBYTE_ERROR_SIZE,                           /* 0x8F */

            /******************  VideoEnc  ******************/
            /* 0x90 ~ 0xAF */
            sizeof(dsp_video_enc_cfg_t),                /* 0x90 AMBADSP_VIDEO_ENC_CFG */
            sizeof(dsp_video_enc_start_t),              /* 0x91 AMBADSP_VIDEO_ENC_START */
            sizeof(dsp_video_enc_stop_t),               /* 0x92 AMBADSP_VIDEO_ENC_STOP */
            sizeof(dsp_video_enc_frmrate_ctrl_t),       /* 0x93 AMBADSP_VIDEO_ENC_FRMRATE_CTRL */
            sizeof(dsp_video_enc_blend_ctrl_t),         /* 0x94 AMBADSP_VIDEO_ENC_BLEND_CTRL */
            sizeof(dsp_video_enc_quality_ctrl_t),       /* 0x95 AMBADSP_VIDEO_ENC_QUALITY_CTRL */
            sizeof(dsp_video_enc_feed_yuv_data_t),      /* 0x96 AMBADSP_VIDEO_ENC_FEED_YUV */
            sizeof(AMBA_DSP_VIDEO_ENC_GROUP_s),         /* 0x97 AMBADSP_VIDEO_ENC_GRP_CFG */
            sizeof(dsp_video_enc_mv_cfg_t),             /* 0x98 AMBADSP_VIDEO_ENC_MV_CFG */
            sizeof(dsp_video_enc_desc_fmt_cfg_t),       /* 0x99 AMBADSP_VIDEO_ENC_DESC_FMT_CFG */
            NBYTE_ERROR_SIZE,                           /* 0x9A */
            NBYTE_ERROR_SIZE,                           /* 0x9B */
            NBYTE_ERROR_SIZE,                           /* 0x9C */
            NBYTE_ERROR_SIZE,                           /* 0x9D */
            NBYTE_ERROR_SIZE,                           /* 0x9E */
            NBYTE_ERROR_SIZE,                           /* 0x9F */
            NBYTE_ERROR_SIZE,                           /* 0xA0 */
            NBYTE_ERROR_SIZE,                           /* 0xA1 */
            NBYTE_ERROR_SIZE,                           /* 0xA2 */
            NBYTE_ERROR_SIZE,                           /* 0xA3 */
            NBYTE_ERROR_SIZE,                           /* 0xA4 */
            NBYTE_ERROR_SIZE,                           /* 0xA5 */
            NBYTE_ERROR_SIZE,                           /* 0xA6 */
            NBYTE_ERROR_SIZE,                           /* 0xA7 */
            NBYTE_ERROR_SIZE,                           /* 0xA8 */
            NBYTE_ERROR_SIZE,                           /* 0xA9 */
            NBYTE_ERROR_SIZE,                           /* 0xAA */
            NBYTE_ERROR_SIZE,                           /* 0xAB */
            NBYTE_ERROR_SIZE,                           /* 0xAC */
            NBYTE_ERROR_SIZE,                           /* 0xAD */
            NBYTE_ERROR_SIZE,                           /* 0xAE */
            NBYTE_ERROR_SIZE,                           /* 0xAF */

            /******************  StillEnc  ******************/
            /* 0xB0 ~ 0xBF */
            sizeof(dsp_stl_data_cap_cfg_t),             /* 0xB0 AMBADSP_STL_DATA_CAP_CFG */
            sizeof(dsp_stl_update_cap_buf_t),           /* 0xB1 AMBADSP_STL_UPDATE_CAP_BUF */
            sizeof(dsp_stl_data_cap_ctrl_t),            /* 0xB2 AMBADSP_STL_DATA_CAP_CTRL*/
            sizeof(dsp_stl_y2y_t),                      /* 0xB3 AMBADSP_STL_Y2Y */
            sizeof(dsp_stl_enc_ctrl_t),                 /* 0xB4 AMBADSP_STL_ENC_CTRL */
            sizeof(dsp_stl_yuv_extbuf_clac_t),          /* 0xB5 AMBADSP_STL_YUV_EXTBUF_CACL */
            sizeof(dsp_stl_r2y_t),                      /* 0xB6 AMBADSP_STL_R2Y */
            sizeof(dsp_stl_r2r_t),                      /* 0xB7 AMBADSP_STL_R2R */
            NBYTE_ERROR_SIZE,                           /* 0xB8 */
            NBYTE_ERROR_SIZE,                           /* 0xB9 */
            NBYTE_ERROR_SIZE,                           /* 0xBA */
            NBYTE_ERROR_SIZE,                           /* 0xBB */
            NBYTE_ERROR_SIZE,                           /* 0xBC */
            NBYTE_ERROR_SIZE,                           /* 0xBD */
            NBYTE_ERROR_SIZE,                           /* 0xBE */
            NBYTE_ERROR_SIZE,                           /* 0xBF */

            /******************  Decode    ******************/
            /* 0xc0 ~ 0xCF */
            sizeof(dsp_video_dec_cfg_t),                /* 0xC0 AMBADSP_VIDEO_DEC_CFG */
            sizeof(dsp_video_dec_start_t),              /* 0xC1 AMBADSP_VIDEO_DEC_START */
            sizeof(dsp_video_dec_stop_t),               /* 0xC2 AMBADSP_VIDEO_DEC_STOP */
            sizeof(dsp_video_dec_trickplay_t),          /* 0xC3 AMBADSP_VIDEO_DEC_TRICKPLAY */
            sizeof(dsp_video_dec_bits_update_t),        /* 0xC4 AMBADSP_VIDEO_DEC_BITS_UPDATE */
            sizeof(dsp_video_dec_post_ctrl_t),          /* 0xC5 AMBADSP_VIDEO_DEC_POST_CTRL */
            sizeof(dsp_still_dec_start_t),              /* 0xC6 AMBADSP_STILL_DEC_START */
            0UL,                                        /* 0xC7 AMBADSP_STILL_DEC_STOP */
            sizeof(dsp_still_dec_y2y_t),                /* 0xC8 AMBADSP_STILL_DEC_Y2Y */
            sizeof(dsp_still_dec_blend_t),              /* 0xC9 AMBADSP_STILL_DEC_BLEND */
            sizeof(dsp_still_dec_disp_yuv_t),           /* 0xCA AMBADSP_STILL_DEC_DISP_YUV */
            NBYTE_ERROR_SIZE,                           /* 0xCB */
            NBYTE_ERROR_SIZE,                           /* 0xCC */
            NBYTE_ERROR_SIZE,                           /* 0xCD */
            NBYTE_ERROR_SIZE,                           /* 0xCE */
            NBYTE_ERROR_SIZE,                           /* 0xCF */

            /******************  Diag    ********************/
#if defined (CONFIG_ENABLE_DSP_DIAG)
            sizeof(uint32_t),                           /* 0xD0 AMBADSP_DIAG_CASE TBD for QNX */
#else
            NBYTE_ERROR_SIZE,                           /* 0xD0 */
#endif
#ifdef CONFIG_ENABLE_DSP_MONITOR
            /******************  Monitor    ******************/
            0UL,                                        /* 0xD1 AmbaDSP_MONITOR_INIT */
            sizeof(dsp_mon_get_cfg_t),                  /* 0xD2 AmbaDSP_MONITOR_GET_CFG */
            sizeof(dsp_mon_crc_cmpr_t),                 /* 0xD3 AmbaDSP_MONITOR_CRC_CMPR */
            sizeof(dsp_mon_err_notify_t),               /* 0xD4 AmbaDSP_MONITOR_ERR_NOTIFY */
            sizeof(dsp_mon_heartbeat_cfg_t),            /* 0xD5 AmbaDSP_MONITOR_HEARTBEAT_CFG */
#else
            NBYTE_ERROR_SIZE,                           /* 0xD1 */
            NBYTE_ERROR_SIZE,                           /* 0xD2 */
            NBYTE_ERROR_SIZE,                           /* 0xD3 */
            NBYTE_ERROR_SIZE,                           /* 0xD4 */
            NBYTE_ERROR_SIZE,                           /* 0xD5 */
#endif
            NBYTE_ERROR_SIZE,                           /* 0xD6 */
            NBYTE_ERROR_SIZE,                           /* 0xD7 */
            NBYTE_ERROR_SIZE,                           /* 0xD8 */
            NBYTE_ERROR_SIZE,                           /* 0xD9 */
            NBYTE_ERROR_SIZE,                           /* 0xDA */
            NBYTE_ERROR_SIZE,                           /* 0xDB */
            NBYTE_ERROR_SIZE,                           /* 0xDC */
            NBYTE_ERROR_SIZE,                           /* 0xDD */
            NBYTE_ERROR_SIZE,                           /* 0xDE */
            NBYTE_ERROR_SIZE                            /* 0xDF */
    };

    AmbaMisra_TouchUnused(ocb);
    /*
     Let common code handle DCMD_ALL_* cases.
     You can do this before or after you intercept devctls, depending
     on your intentions.  Here we aren't using any predefined values,
     so let the system ones be handled first. See note 2.
    */
    status = iofunc_devctl_verify(ctp, msg, ocb, _IO_DEVCTL_VERIFY_LEN);
    if (status != EOK) {
        retcode = status;
    } else {
        retcode = check_binding_pid(ctp->info.pid);

        if (retcode == EOK) {
            status = iofunc_attr_unlock(&dsp_iofunc_attr);
            if (status != EOK) {
                retcode = status;
            } else {
                rval = 0U;

                /*
                 Note this assumes that you can fit the entire data portion of
                 the devctl into one message.  In reality you should probably
                 perform a MsgReadv() once you know the type of message you
                 have received to get all of the data, rather than assume
                 it all fits in the message.  We have set in our main routine
                 that we'll accept a total message size of up to 2 KB, so we
                 don't worry about it in this example where we deal with ints.
                */

                /* Get the data from the message. See Note 3. */
                // rx_data = _DEVCTL_DATA(msg->i);
                idx = (msg->i.dcmd & 0xFF);
                if (idx < AMBADSP_IOCTL_CMD_MAX_NUM) {
                    nbytes = nbytes_array[idx];
                } else {
                    nbytes = NBYTE_ERROR_SIZE;
                }
                if (nbytes == NBYTE_ERROR_SIZE) {
                    nbytes = 0UL;
                    dsp_osal_printU5("[ERR] ambadsp_ioctl : unknown IOCTL 0x%X", (uint32_t)msg->i.dcmd, 0U, 0U, 0U, 0U);
                    rval = DSP_ERR_0002;
                }

                if (rval == 0U) {
                    arg = _DEVCTL_DATA(msg->i);
                    status = (int32_t)dsp_ioctl_impl(ocb, (uint32_t)msg->i.dcmd, arg);
                } else {
                    (void)dsp_osal_memcpy(&status, &rval, sizeof(UINT32));
                }

                /* Clear the return message. Note that we saved our data past
                   this location in the message. */
                (void)memset(&msg->o, 0, sizeof(msg->o));

                /*
                 If you wanted to pass something different to the return
                 field of the devctl() you could do it through this member.
                 See note 5.
                */
                msg->o.ret_val = status;

                /* Indicate the number of bytes and return the message */
                msg->o.nbytes = (uint32_t)nbytes;

                status = iofunc_attr_lock(&dsp_iofunc_attr);
                if (status != EOK) {
                    retcode = status;
                } else {
                    retcode = _RESMGR_PTR(ctp, &msg->o, sizeof(msg->o) + nbytes);
                }
            }
        }
    }

    return retcode;
}

static char buffer[512] = {"Hello world\n"};
static int32_t ambadsp_read(resmgr_context_t *ctp, io_read_t *msg, RESMGR_OCB_T *ocb)
{
    size_t nleft;
    size_t nbytes;
    int32_t nparts;
    int32_t status;
    int32_t retcode = 0;

    status = iofunc_read_verify (ctp, msg, ocb, NULL);
    if (status != EOK) {
        retcode = status;
    } else {
        if ((msg->i.xtype & (uint32_t)_IO_XTYPE_MASK) != (uint32_t)_IO_XTYPE_NONE) {
            retcode = ENOSYS;
        } else {
            /*
             * On all reads (first and subsequent), calculate
             * how many bytes we can return to the client,
             * based upon the number of bytes available (nleft)
             * and the client's buffer size
             */

            // Every read return the buffer defined here.
            nleft = strlen(buffer) + 1U - (size_t)ocb->offset;
            nbytes = min(_IO_READ_GET_NBYTES(msg), nleft);
            if (nbytes > 0U) {
                /* set up the return data IOV */
                SETIOV(ctp->iov, &buffer[ocb->offset], nbytes);


                /* set up the number of bytes (returned by client's read()) */
                _IO_SET_READ_NBYTES (ctp, (int64_t)nbytes);
                /*
                 * advance the offset by the number of bytes
                 * returned to the client.
                 */
                ocb->offset += (int32_t)nbytes;
                nparts = 1;
            } else {
                /*
                 * they've asked for zero bytes or they've already previously
                 * read everything
                 */
                _IO_SET_READ_NBYTES (ctp, 0);
                nparts = 0;
            }
            /* mark the access time as invalid (we just accessed it) */
            if (msg->i.nbytes > 0U) {
                ocb->attr->flags |= (uint32_t)IOFUNC_ATTR_ATIME;
            }
            retcode = _RESMGR_NPARTS(nparts);
        }
    }

    return retcode;
}

static int32_t ambadsp_write(resmgr_context_t *ctp, io_write_t *msg, RESMGR_OCB_T *ocb)
{
    int32_t status;
    int32_t retcode = 0;
    size_t nbytes;

    status = iofunc_write_verify(ctp, msg, ocb, NULL);
    if (status != EOK) {
        retcode = status;
    } else {
        if ((msg->i.xtype & (uint32_t)_IO_XTYPE_MASK) != (uint32_t)_IO_XTYPE_NONE) {
            retcode = ENOSYS;
        } else {

            /* Extract the length of the client's message. */
            nbytes = _IO_WRITE_GET_NBYTES(msg);

            /* Filter out malicious write requests that attempt to write more
               data than they provide in the message. */
            if (nbytes > (((size_t)ctp->info.srcmsglen - (size_t)ctp->offset) - (size_t)(sizeof(io_write_t)))) {
                retcode = EBADMSG;
            } else {

                /* set up the number of bytes (returned by client's write()) */
                _IO_SET_WRITE_NBYTES(ctp, (int64_t)nbytes);

                /*
                 *  Reread the data from the sender's message buffer.
                 *  We're not assuming that all of the data fit into the
                 *  resource manager library's receive buffer.
                 */

                status = (int32_t)resmgr_msgread(ctp, buffer, nbytes, sizeof(msg->i));
                if (status == -1) {
                    retcode = errno;
                } else {
                    buffer [nbytes] = '\0'; /* just in case the text is not NULL terminated */

                    if (nbytes > 0U) {
                        ocb->attr->flags |= ((uint32_t)IOFUNC_ATTR_MTIME | (uint32_t)IOFUNC_ATTR_CTIME);
                    }
                    retcode =_RESMGR_NPARTS (0);
                }
            }
        }
    }

    return retcode;
}

static int32_t ambadsp_close(resmgr_context_t *ctp, void *reserved, iofunc_ocb_t *ocb)
{
    int32_t Rval;

#if 0 //FIXME
    krn_flexidag_close_by_owner(ocb);
#endif
    Rval = iofunc_close_ocb_default(ctp, reserved, ocb);
    return Rval;
}

static void ambadsp_of_init(void)
{
    uint32_t Rval = 0;

    Rval = dsp_osal_kernel_init();
    AmbaMisra_TouchUnused(&Rval);
}

int32_t main(int32_t argc, char * const *argv)
{
    /* declare variables we'll be using */
    static resmgr_io_funcs_t        dsp_io_funcs;
    static resmgr_connect_funcs_t   dsp_connect_funcs;
    thread_pool_attr_t              pool_attr;
    resmgr_attr_t                   resmgr_attr;
    dispatch_t                      *dpp;
    thread_pool_t                   *tpp;
    iofunc_attr_t                   io_attr = {0};
    struct _client_info             info = {0};
    int32_t                         id;
    int32_t                         option;
    int32_t                         retcode = 0;
    uint32_t                        rval = 0U;

(void)argc;

    (void)printf("ambadsp dev main start\n");

    /* initialize dispatch interface */
    dpp = dispatch_create();
    if (dpp == NULL) {
        (void)printf("%s: Unable to allocate dispatch handle\n", argv[0]);
        retcode = EXIT_FAILURE;
    } else {
        /* initialize resource manager attributes */
        rval = dsp_osal_memset(&resmgr_attr, 0, sizeof(resmgr_attr));
        if (rval != 0U) {
            (void)printf("Unable to initialize resource manager attributes\n");
        }
        resmgr_attr.nparts_max = 1;
        resmgr_attr.msg_max_size = 16384; // 2^14 byte which followed ioctl spec

        /* initialize functions for handling messages */
        iofunc_func_init((UINT32)_RESMGR_CONNECT_NFUNCS,
                         &dsp_connect_funcs,
                         (UINT32)_RESMGR_IO_NFUNCS,
                         &dsp_io_funcs);

        dsp_io_funcs.devctl = ambadsp_ioctl;
        dsp_io_funcs.read = ambadsp_read;
        dsp_io_funcs.write = ambadsp_write;
        dsp_io_funcs.close_ocb = ambadsp_close;
        /* initialize attribute structure used by the device */
        iofunc_attr_init(&dsp_iofunc_attr, (uint32_t)S_IFNAM | 438U /*Octal 0666*/, &io_attr, &info);

        //(void)printf("ambadsp resmgr_attach\n");
        /* attach our device name */
        id = resmgr_attach(dpp,                 /* dispatch handle        */
                           &resmgr_attr,        /* resource manager attrs */
                           "/dev/ambadsp",      /* device name            */
                           _FTYPE_ANY,          /* open type              */
                           0,                   /* flags                  */
                           &dsp_connect_funcs,  /* connect routines       */
                           &dsp_io_funcs,       /* I/O routines           */
                           &dsp_iofunc_attr);   /* handle                 */
        if (id == -1) {
            (void)printf("%s: Unable to attach name\n", argv[0]);
            retcode = EXIT_FAILURE;
        } else {
            //(void)printf("ambadsp resmgr_attach id[%d]\n", id);
            //(void)printf("ambadsp argument parsing\n");
            do {
                option = getopt(argc, argv, "A:V:D:I:R:v");
            } while (option != -1);

            //(void)printf("ambadsp system init\n");
            ambadsp_of_init();

            /* initialize thread pool attributes */
            rval = dsp_osal_memset(&pool_attr, 0, sizeof(pool_attr));
            if (rval != 0U) {
                (void)printf("Unable to initialize thread pool attributes\n");
            }
            pool_attr.handle = dpp;
            pool_attr.context_alloc = dispatch_context_alloc;
            pool_attr.block_func = dispatch_block;
            pool_attr.unblock_func = dispatch_unblock;
            pool_attr.handler_func = dispatch_handler;
            pool_attr.context_free = dispatch_context_free;
            pool_attr.lo_water = 2;
            pool_attr.hi_water = 4;
            pool_attr.increment = 1;
            pool_attr.maximum = 50;

            //(void)printf("ambadsp thread pool create");
            /* allocate a thread pool handle */
            tpp = thread_pool_create(&pool_attr, POOL_FLAG_EXIT_SELF);
            if (tpp == NULL) {
                (void)printf("%s: Unable to initialize thread pool\n", argv[0]);
                retcode = EXIT_FAILURE;
            } else {
                /* Start the threads. This function doesn't return. */
                //(void)printf("ambadsp thread pool start\n");
                retcode = thread_pool_start(tpp);
                if (retcode != EOK) {
                    (void)printf("%s: Unable to start thread pool\n", argv[0]);
                }

                (void)printf("ambadsp dev main end\n");
                retcode = EXIT_SUCCESS;
            }
        }
    }

    return retcode;
}

